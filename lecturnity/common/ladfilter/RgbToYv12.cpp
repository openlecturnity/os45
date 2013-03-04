//------------------------------------------------------------------------------
// File: RgbToYv12.cpp
//
// Desc: DirectShow filter: RGB to YV12 conversion.
//------------------------------------------------------------------------------


#include <streams.h>     // DirectShow (includes windows.h)
#include <initguid.h>    // declares DEFINE_GUID to declare an EXTERN_C const.
#include <tchar.h>       // For Debug info text
#include <stdio.h>       // For Debug info output

#include "RgbToYv12.h"

/*
// setup data - allows the self-registration to work.

const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
  &MEDIATYPE_Video       // clsMajorType
, &MEDIASUBTYPE_NULL     // clsMinorType
};

const AMOVIESETUP_PIN psudPins[] =
{ { L"Input"            // strName
  , FALSE               // bRendered
  , FALSE               // bOutput
  , FALSE               // bZero
  , FALSE               // bMany
  , &CLSID_NULL         // clsConnectsToFilter
  , L""                 // strConnectsToPin
  , 1                   // nTypes
  , &sudPinTypes        // lpTypes
  }
, { L"Output"           // strName
  , FALSE               // bRendered
  , TRUE                // bOutput
  , FALSE               // bZero
  , FALSE               // bMany
  , &CLSID_NULL         // clsConnectsToFilter
  , L""                 // strConnectsToPin
  , 1                   // nTypes
  , &sudPinTypes        // lpTypes
  }
};

const AMOVIESETUP_FILTER sudRgbToYv12 =
{ 
  &CLSID_RgbToYv12                // clsID
, L"RGB to YV12"                  // strName
, MERIT_DO_NOT_USE                // dwMerit
, 2                               // nPins
, psudPins                        // lpPin
};

// Class factory template - needed for the CreateInstance mechanism
CFactoryTemplate g_Templates[]=
    {   { L"RGB to YV12"
        , &CLSID_RgbToYv12
        , CRgbToYv12::CreateInstance
        , NULL
        , &sudRgbToYv12 }
    };

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);
*/


//
// Constructor
//
CRgbToYv12::CRgbToYv12(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr) :
  CTransformFilter (tszName, punk, CLSID_RgbToYv12Filter) {

    //MessageBox(NULL, _T("CRgbToYv12"), _T("Info"), MB_OK);
    m_VideoSubtype = GUID_NULL;

    m_nVideoWidth = -1;
    m_nVideoHeight = -1;

} // Constructor

//
// Destructor
//
CRgbToYv12::~CRgbToYv12() {

    //MessageBox(NULL, _T("~CRgbToYv12"), _T("Info"), MB_OK);
    m_VideoSubtype = GUID_NULL;

    m_nVideoWidth = -1;
    m_nVideoHeight = -1;

} // Destructor


//
// CreateInstance
//
// Provide the way for COM to create a CRgbToYv12 object
//
CUnknown * WINAPI CRgbToYv12::CreateInstance(LPUNKNOWN punk, HRESULT *phr) {

    //MessageBox(NULL, _T("CreateInstance"), _T("Info"), MB_OK);
    CheckPointer(phr, NULL);
    
    CRgbToYv12 *pNewObject = 
        new CRgbToYv12(NAME("RGB to YV12 filter"), punk, phr);

    if (pNewObject == NULL) {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;

} // CreateInstance


//
// Transform
//
HRESULT CRgbToYv12::Transform(IMediaSample *pIn, IMediaSample *pOut) {
 
    //MessageBox(NULL, _T("Transform(pIn, pOut)"), _T("Info"), MB_OK);
    HRESULT hr = S_OK;

    CheckPointer(pIn, E_POINTER);
    CheckPointer(pOut, E_POINTER);

    // Convert the data

    // Alternative 1:
    //ConvertRgbToYv12(pIn, pOut);
    // Alternative 2 (huffyuv):
    InitClip();
    ConvertRgb32ToHuffYv12(pIn, pOut);

    // Copy the sample times

    REFERENCE_TIME TimeStart, TimeEnd;
    if (NOERROR == pIn->GetTime(&TimeStart, &TimeEnd)) {
        pOut->SetTime(&TimeStart, &TimeEnd);
    }

    LONGLONG MediaStart, MediaEnd;
    if (pIn->GetMediaTime(&MediaStart ,&MediaEnd) == NOERROR) {
        pOut->SetMediaTime(&MediaStart, &MediaEnd);
    }

    // Copy the Sync point property

    hr = pIn->IsSyncPoint();
    if (hr == S_OK) {
        pOut->SetSyncPoint(TRUE);
    } else if (hr == S_FALSE) {
        pOut->SetSyncPoint(FALSE);
    } else {
        // an unexpected error has occured...
        return E_UNEXPECTED;
    }


    // Next lines: Copy preroll/discontinuity property
    // Outcommented because they lead to unexpected behaviour
/*
    // Copy the preroll property

    hr = pIn->IsPreroll();
    if (hr == S_OK) {
        pOut->SetPreroll(TRUE);
    } else if (hr == S_FALSE) {
        pOut->SetPreroll(FALSE);
    } else {  
        // an unexpected error has occured...
        return E_UNEXPECTED;
    }
*/
/*
    // Copy the discontinuity property

    hr = pIn->IsDiscontinuity();

    if (hr == S_OK) {
        pOut->SetDiscontinuity(TRUE);
    } else if (hr == S_FALSE) {
        pOut->SetDiscontinuity(FALSE);
    } else {  
        // an unexpected error has occured...
        return E_UNEXPECTED;
    }
*/

    return hr;

} // Transform


HRESULT CRgbToYv12::ConvertRgbToYv12(IMediaSample *pIn, IMediaSample *pOut) {

    //MessageBox(NULL, _T("ConvertRgbToYv12"), _T("Info"), MB_OK);
    HRESULT hr = S_OK;

    // Input: 3/4 byte (24/32 bit) data?
    int byteLength = 3;
    if (m_VideoSubtype != MEDIASUBTYPE_RGB24)
        byteLength = 4;

    // Byte shift (bs)
    int bs0, bs1, bs2;
    if (m_VideoSubtype == MEDIASUBTYPE_RGB32) {
        // RGB32 --> 0, 1, 2
        bs0 = 0;
        bs1 = 1;
        bs2 = 2;
    } else if (m_VideoSubtype == MEDIASUBTYPE_ARGB32) {
        // ARGB32 --> 1, 2, 3 ??? (Not tested so far)
        bs0 = 1;
        bs1 = 2;
        bs2 = 3;
    } else {
        // RGB24 --> 0, 1, 2
        bs0 = 0;
        bs1 = 1;
        bs2 = 2;
    }

    // Check buffer lengths
    long lSourceSize = pIn->GetActualDataLength();
    long lDestSize = pOut->GetSize();


    // Get the pointers to input/output buffer
    BYTE *pRgbImage;
    BYTE *pYuvImage;
    pIn->GetPointer(&pRgbImage);
    pOut->GetPointer(&pYuvImage);

    // Shortcuts for video width/height
    int w = m_nVideoWidth;
    int h = m_nVideoHeight;

    //TCHAR tszMessage[256];
    //sprintf_s(tszMessage, 256, _T("ConvertRgbToYv12: Video: %d x %d"), w, h);
    //MessageBox(NULL, tszMessage, _T("Info"), MB_OK);

    // "Converting Between YUV and RGB"
    // from http://msdn.microsoft.com/en-us/library/ms893078.aspx
    // and also from http://en.wikipedia.org/wiki/YUV
    // Y = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16
    // U = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128
    // V = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128

    unsigned int c00 =  66, c01 = 129, c02 =  25;
    unsigned int c10 = -38, c11 = -74, c12 = 112;
    unsigned int c20 = 112, c21 = -94, c22 = -18;
    unsigned int d0 = 128, d1 = 128, d2 = 128;
    unsigned int s0 = 8, s1 = 8, s2 = 8;
    unsigned int t0 = 16, t1 = 128, t2 = 128;

    // The Y plane is the first one
    BYTE *Y = pYuvImage;
    // Then the U plane after a width*height Y plane
    BYTE *U = Y + (w * h);
    // Then the V plane after a (width/2)*(height/2) U plane
    BYTE *V = U + ((w * h / 4));

    // Next line: image starting point in the upper left corner expected ...
    //for (int y = 0; y < h; y+=2) {
    // ... but it is in the lower left corner
    // --> Flip image horizontally (upside down)
    for (int y = h-2; y >= 0; y-=2) {
        for (int x = 0; x < w; x+=2) {
            // Average uv value for a 2x2 px unit
            int uAvg, vAvg;

            // Get R, G, B values for a 2x2 px unit
            // Flip y positions (y+0, y+1 --> y+1, y+0)
            BYTE R00 = pRgbImage[(((y+1) * w) + (x+0)) * byteLength + bs0];
            BYTE G00 = pRgbImage[(((y+1) * w) + (x+0)) * byteLength + bs1];
            BYTE B00 = pRgbImage[(((y+1) * w) + (x+0)) * byteLength + bs2];

            BYTE R01 = pRgbImage[(((y+1) * w) + (x+1)) * byteLength + bs0];
            BYTE G01 = pRgbImage[(((y+1) * w) + (x+1)) * byteLength + bs1];
            BYTE B01 = pRgbImage[(((y+1) * w) + (x+1)) * byteLength + bs2];

            BYTE R10 = pRgbImage[(((y+0) * w) + (x+0)) * byteLength + bs0];
            BYTE G10 = pRgbImage[(((y+0) * w) + (x+0)) * byteLength + bs1];
            BYTE B10 = pRgbImage[(((y+0) * w) + (x+0)) * byteLength + bs2];

            BYTE R11 = pRgbImage[(((y+0) * w) + (x+1)) * byteLength + bs0];
            BYTE G11 = pRgbImage[(((y+0) * w) + (x+1)) * byteLength + bs1];
            BYTE B11 = pRgbImage[(((y+0) * w) + (x+1)) * byteLength + bs2];

            // Compute a 2x2 Y block from current 2x2 RGB block
            *(Y + 0 + 0) = ( ( c00 * R00 + c01 * G00 + c02 * B00 + d0) >> s0) + t0;
            *(Y + 0 + 1) = ( ( c00 * R01 + c01 * G01 + c02 * B01 + d0) >> s0) + t0;
            *(Y + w + 0) = ( ( c00 * R10 + c01 * G10 + c02 * B10 + d0) >> s0) + t0;
            *(Y + w + 1) = ( ( c00 * R11 + c01 * G11 + c02 * B11 + d0) >> s0) + t0;

            // We first compute four U samples corresponding to our four RGB pixels
            uAvg =  ( ( c10 * R00 + c11 * G00 + c12 * B00 + d1) >> s1) + t1;
            uAvg += ( ( c10 * R01 + c11 * G01 + c12 * B01 + d1) >> s1) + t1;
            uAvg += ( ( c10 * R10 + c11 * G10 + c12 * B10 + d1) >> s1) + t1;
            uAvg += ( ( c10 * R11 + c11 * G11 + c12 * B11 + d1) >> s1) + t1;
            // Then we average these four U samples that have been accumulated
            uAvg = (unsigned int)(0.5f + (uAvg / 4.0f));
            *U = (BYTE)uAvg;

            // Same thing for the V sample
            vAvg =  ( ( c20 * R00 + c21 * G00 + c22 * B00 + d2) >> s2) + t2;
            vAvg += ( ( c20 * R01 + c21 * G01 + c22 * B01 + d2) >> s2) + t2;
            vAvg += ( ( c20 * R10 + c21 * G10 + c22 * B10 + d2) >> s2) + t2;
            vAvg += ( ( c20 * R11 + c21 * G11 + c22 * B11 + d2) >> s2) + t2;
            // Then we average these four V samples that have been accumulated
            vAvg = (unsigned int)(0.5f + (vAvg / 4.0f));
            *V = (BYTE)vAvg;

            // Then we update Y, U, V pointers so they point to the following 
            // block of 2x2 pixels
            Y += 2; /* we computed 2 pixels on the horizontal direction */
            // At the end of one line: skip the next line
            if (x >= (w-2))
                Y += w;
            U += 1;
            V += 1;
        }
    }

    return hr;
}

// From huffyuv v2.1.1 (http://neuron2.net/www.math.berkeley.edu/benrg/huffyuv.html)
void CRgbToYv12::InitClip() {
    memset(clip, 0, 320);
    for (int i = 0; i < 256; ++i)
        clip[i+320] = i;
    memset(clip+320+256, 255, 320);
}

// From huffyuv v2.1.1 (http://neuron2.net/www.math.berkeley.edu/benrg/huffyuv.html)
unsigned char CRgbToYv12::Clip(int x) {
    return clip[320 + ((x+0x8000) >> 16)];
}

HRESULT CRgbToYv12::ConvertRgb32ToHuffYv12(IMediaSample *pIn, IMediaSample *pOut) {

    //MessageBox(NULL, _T("ConvertRgbToYv12"), _T("Info"), MB_OK);
    HRESULT hr = S_OK;

    // Input: 3/4 byte (24/32 bit) data?
    int byteLength = 3;
    if (m_VideoSubtype != MEDIASUBTYPE_RGB24)
        byteLength = 4;

    // Byte shift (bs)
    int bs0, bs1, bs2;
    if (m_VideoSubtype == MEDIASUBTYPE_RGB32) {
        // RGB32 --> 0, 1, 2
        bs0 = 0;
        bs1 = 1;
        bs2 = 2;
    } else if (m_VideoSubtype == MEDIASUBTYPE_ARGB32) {
        // ARGB32 --> 1, 2, 3 ??? (Not tested so far)
        bs0 = 1;
        bs1 = 2;
        bs2 = 3;
    } else {
        // RGB24 --> 0, 1, 2
        bs0 = 0;
        bs1 = 1;
        bs2 = 2;
    }

    // Check buffer lengths
    long lSourceSize = pIn->GetActualDataLength();
    long lDestSize = pOut->GetSize();


    // Get the pointers to input/output buffer
    BYTE *pRgbImage;
    BYTE *pYuvImage;
    pIn->GetPointer(&pRgbImage);
    pOut->GetPointer(&pYuvImage);

    // Shortcuts for video width/height
    int w = m_nVideoWidth;
    int h = m_nVideoHeight;


    // From huffyuv v2.1.1 (http://neuron2.net/www.math.berkeley.edu/benrg/huffyuv.html)
    int c00 = int(0.299 * 219.0/255.0 * 65536.0 + 0.5);
    int c01 = int(0.587 * 219.0/255.0 * 65536.0 + 0.5);
    int c02 = int(0.114 * 219.0/255.0 * 65536.0 + 0.5);
    int d0 = 0x108000;
    int s0 = 16;
    int t0 = 0;

    // The Y plane is the first one
    BYTE *Y = pYuvImage;
    // Then the U plane after a width*height Y plane
    BYTE *U = Y + (w * h);
    // Then the V plane after a (width/2)*(height/2) U plane
    BYTE *V = U + ((w * h / 4));

    // Now start conversion ...

    // Next line: image starting point in the upper left corner expected ...
    //for (int y = 0; y < h; y+=2) {
    // ... but it is in the lower left corner
    // --> Flip image horizontally (upside down)
    for (int y = h-2; y >= 0; y-=2) {
        for (int x = 0; x < w; x+=2) {
            // Get R, G, B values for a 2x2 px unit
            // Flip y positions (y+0, y+1 --> y+1, y+0)
            BYTE R00 = pRgbImage[(((y+1) * w) + (x+0)) * byteLength + bs0];
            BYTE G00 = pRgbImage[(((y+1) * w) + (x+0)) * byteLength + bs1];
            BYTE B00 = pRgbImage[(((y+1) * w) + (x+0)) * byteLength + bs2];

            BYTE R01 = pRgbImage[(((y+1) * w) + (x+1)) * byteLength + bs0];
            BYTE G01 = pRgbImage[(((y+1) * w) + (x+1)) * byteLength + bs1];
            BYTE B01 = pRgbImage[(((y+1) * w) + (x+1)) * byteLength + bs2];

            BYTE R10 = pRgbImage[(((y+0) * w) + (x+0)) * byteLength + bs0];
            BYTE G10 = pRgbImage[(((y+0) * w) + (x+0)) * byteLength + bs1];
            BYTE B10 = pRgbImage[(((y+0) * w) + (x+0)) * byteLength + bs2];

            BYTE R11 = pRgbImage[(((y+0) * w) + (x+1)) * byteLength + bs0];
            BYTE G11 = pRgbImage[(((y+0) * w) + (x+1)) * byteLength + bs1];
            BYTE B11 = pRgbImage[(((y+0) * w) + (x+1)) * byteLength + bs2];

            // Compute a 2x2 Y block from current 2x2 RGB block
            int y00 = ( ( c00 * R00 + c01 * G00 + c02 * B00 + d0) >> s0) + t0;
            int y01 = ( ( c00 * R01 + c01 * G01 + c02 * B01 + d0) >> s0) + t0;
            int y10 = ( ( c00 * R10 + c01 * G10 + c02 * B10 + d0) >> s0) + t0;
            int y11 = ( ( c00 * R11 + c01 * G11 + c02 * B11 + d0) >> s0) + t0;
            *(Y + 0 + 0) = (BYTE)y00;
            *(Y + 0 + 1) = (BYTE)y01;
            *(Y + w + 0) = (BYTE)y10;
            *(Y + w + 1) = (BYTE)y11;

            // From huffyuv v2.1.1 (http://neuron2.net/www.math.berkeley.edu/benrg/huffyuv.html)
            // huffyuv converts to YUY2 (2x1 px) 
            // --> Split 2x2 px unit into 2 (2x1 px) units and merge them
            // Calculate a scaled Y value 
            int scaled_y0 
                = (y00+y01 - 32) * int(255.0/219.0 * 32768.0 + 0.5);
            int scaled_y1 
                = (y10+y11 - 32) * int(255.0/219.0 * 32768.0 + 0.5);
            // Determine U
            int b_y0 
                = ((B00+B01) << 15) - scaled_y0;
            int b_y1 
                = ((B10+B11) << 15) - scaled_y1;
            unsigned char u0 = Clip((b_y0 >> 10) * int(1.0/2.018 * 1024.0 + 0.5) + 0x800000);
            unsigned char u1 = Clip((b_y1 >> 10) * int(1.0/2.018 * 1024.0 + 0.5) + 0x800000);
            *U = (BYTE)((u0 + u1) >> 1);
            // Determine V
            int r_y0 
                = ((R00+R01) << 15) - scaled_y0;
            int r_y1 
                = ((R10+R11) << 15) - scaled_y1;
            unsigned char v0 = Clip((r_y0 >> 10) * int(1.0/1.596 * 1024.0 + 0.5) + 0x800000);
            unsigned char v1 = Clip((r_y1 >> 10) * int(1.0/1.596 * 1024.0 + 0.5) + 0x800000);
            *V = (BYTE)((v0 + v1) >> 1);

            // Then we update Y, U, V pointers so they point to the following 
            // block of 2x2 pixels
            Y += 2; /* we computed 2 pixels on the horizontal direction */
            // At the end of one line: skip the next line
            if (x >= (w-2))
                Y += w;
            U += 1;
            V += 1;
        }
    }
    return hr;
}



//
// CheckInputType
//
// Check the input type is OK, return an error otherwise
//
HRESULT CRgbToYv12::CheckInputType(const CMediaType *mtIn) {

    //MessageBox(NULL, _T("CheckInputType"), _T("Info"), MB_OK);
    CheckPointer(mtIn, E_POINTER);

    // Remember video subtype and dimension
    GUID oldVideoSubtype = m_VideoSubtype;
    int oldVideoWidth = m_nVideoWidth;
    int oldVideoHeight = m_nVideoHeight;

    // Check this is a VIDEOINFO type

    if (*mtIn->FormatType() != FORMAT_VideoInfo) {
        //MessageBox(NULL, _T("CheckInputType: no video format!"), _T("Error"), MB_OK);
        return E_INVALIDARG;
    }

    // Check for RGB24, RGB32 or ARGB32

    bool bCorrectSubtypeFound = false;

    if ( *mtIn->Subtype( ) == MEDIASUBTYPE_RGB24 ) {
        //MessageBox(NULL, _T("CheckInputType: Input type: RGB24"), _T("Info"), MB_OK);
        bCorrectSubtypeFound = true;
        m_VideoSubtype = MEDIASUBTYPE_RGB24;
    }

    if ( *mtIn->Subtype( ) == MEDIASUBTYPE_RGB32 ) {
        //MessageBox(NULL, _T("CheckInputType: Input type: RGB32"), _T("Info"), MB_OK);
        bCorrectSubtypeFound = true;
        m_VideoSubtype = MEDIASUBTYPE_RGB32;
    }

    if ( *mtIn->Subtype( ) == MEDIASUBTYPE_ARGB32 ) {
        //MessageBox(NULL, _T("CheckInputType: Input type: ARGB32"), _T("Info"), MB_OK);
        bCorrectSubtypeFound = true;
        m_VideoSubtype = MEDIASUBTYPE_ARGB32;
    }

    if (bCorrectSubtypeFound) {

        // Determine Video width and height
        // Values must be a multiple of 2

        VIDEOINFO *pInput  = (VIDEOINFO *) mtIn->Format();
        BITMAPINFOHEADER bmi = pInput->bmiHeader;
        int nVideoWidth = bmi.biWidth;
        int nVideoHeight = bmi.biHeight;

        if ( (nVideoWidth > 0) && (nVideoHeight > 0) ) {
            if ( ((nVideoWidth % 2) == 0) && ((nVideoHeight % 2) == 0) ) {
                // Proper video size detected

                // Does video subtype/dimension differ from the current values?
                bool bVideoPropertiesHaveChanged = false;

                if (oldVideoSubtype != m_VideoSubtype)
                    bVideoPropertiesHaveChanged = true;
                if (oldVideoWidth != m_nVideoWidth)
                    bVideoPropertiesHaveChanged = true;
                if (oldVideoHeight != m_nVideoHeight)
                    bVideoPropertiesHaveChanged = true;

                if (bVideoPropertiesHaveChanged) {
                    // (Re-) initialize input/output image buffer
                    m_nVideoWidth = nVideoWidth;
                    m_nVideoHeight = nVideoHeight;
                    //TCHAR tszMessage[256];
                    //sprintf_s(tszMessage, 256, _T("CheckInputType: Video: %d x %d"), m_nVideoWidth, m_nVideoHeight);
                    //MessageBox(NULL, tszMessage, _T("Info"), MB_OK);
                    return NOERROR;
                }
            } else {
                MessageBox(NULL, _T("CheckInputType: odd video size!"), _T("Error"), MB_OK);
                return E_INVALIDARG;
            }
        } else {
            MessageBox(NULL, _T("CheckInputType: invalid video size!"), _T("Error"), MB_OK);
            return E_INVALIDARG;
        }

        return NOERROR;
    }

    return E_FAIL;

} // CheckInputType


//
// CheckTransform
//
HRESULT CRgbToYv12::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut) {

    //MessageBox(NULL, _T("CheckTransform"), _T("Info"), MB_OK);
    CheckPointer(mtIn, E_POINTER);
    CheckPointer(mtOut, E_POINTER);

    HRESULT hr;
    if (FAILED(hr = CheckInputType(mtIn))) {
        return hr;
    }

    // Format must be a VIDEOINFOHEADER
    if (*mtOut->FormatType() != FORMAT_VideoInfo) {
        return E_INVALIDARG;
    }

    // Formats must be big enough 
    if (mtIn->FormatLength() < sizeof(VIDEOINFOHEADER) ||
        mtOut->FormatLength() < sizeof(VIDEOINFOHEADER))
        return E_INVALIDARG;

    // Check subtypes
    // a) input must be RGB24, RGB32 or ARGB32
    if ( (*mtIn->Subtype( ) == MEDIASUBTYPE_RGB24) 
      || (*mtIn->Subtype( ) == MEDIASUBTYPE_RGB32)
      || (*mtIn->Subtype( ) == MEDIASUBTYPE_ARGB32) ) {
        // b) output must be YV12
        if (*mtOut->Subtype( ) != MEDIASUBTYPE_YV12) {
            MessageBox(NULL, _T("CheckTransform: Invalid output subtype!"), _T("Error"), MB_OK);
            return E_INVALIDARG;
        }
    } else {
        MessageBox(NULL, _T("CheckTransform: Invalid input subtype!"), _T("Error"), MB_OK);
        return E_INVALIDARG;
    }

    return NOERROR;

} // CheckTransform


//
// DecideBufferSize
//
// Tell the output pin's allocator what size buffers we
// require. Can only do this when the input is connected
//
HRESULT CRgbToYv12::DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties) {

    //MessageBox(NULL, _T("DecideBufferSize"), _T("Info"), MB_OK);

    // Is the input pin connected

    if (m_pInput->IsConnected() == FALSE) {
        return E_UNEXPECTED;
    }

    CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pProperties, E_POINTER);
    HRESULT hr = NOERROR;

    pProperties->cBuffers = 1;
    pProperties->cbBuffer = m_pInput->CurrentMediaType().GetSampleSize();

    ASSERT(pProperties->cbBuffer);

    // If we don't have fixed sized samples we must guess some size

    if (!m_pInput->CurrentMediaType().bFixedSizeSamples) {
        if (pProperties->cbBuffer < 100000) {
            // nothing more than a guess!!
            pProperties->cbBuffer = 100000;
        }
    }

    // Ask the allocator to reserve us some sample memory, NOTE the function
    // can succeed (that is return NOERROR) but still not have allocated the
    // memory that we requested, so we must check we got whatever we wanted

    ALLOCATOR_PROPERTIES Actual;

    hr = pAlloc->SetProperties(pProperties, &Actual);
    if (FAILED(hr)) {
        return hr;
    }

    ASSERT(Actual.cBuffers == 1);

    if (pProperties->cBuffers > Actual.cBuffers ||
        pProperties->cbBuffer > Actual.cbBuffer) {
        return E_FAIL;
    }

    return NOERROR;

} // DecideBufferSize

//
// GetMediaType
//
HRESULT CRgbToYv12::GetMediaType(int iPosition, CMediaType *pMediaType) {

    //MessageBox(NULL, _T("GetMediaType"), _T("Info"), MB_OK);
    // Is the input pin connected

    if (m_pInput->IsConnected() == FALSE) {
        return E_UNEXPECTED;
    }

    // This should never happen

    if (iPosition < 0) {
        return E_INVALIDARG;
    }

    // Do we have more items to offer

    if (iPosition > 0) {
        return VFW_S_NO_MORE_ITEMS;
    }

    CheckPointer(pMediaType, E_POINTER);

    //TCHAR tszMessage[256];
    //sprintf_s(tszMessage, 256, _T("GetMediaType: Video: %d x %d"), m_nVideoWidth, m_nVideoHeight);
    //MessageBox(NULL, tszMessage, _T("Info"), MB_OK);

    VIDEOINFOHEADER vih;
    memset( &vih, 0, sizeof( vih ) );
    vih.bmiHeader.biCompression  = DWORD('21VY');
    vih.bmiHeader.biBitCount     = 12;
    vih.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    vih.bmiHeader.biWidth        = m_nVideoWidth;
    vih.bmiHeader.biHeight       = m_nVideoHeight;
    vih.bmiHeader.biPlanes       = 1;
    vih.bmiHeader.biSizeImage    = GetBitmapSize(&vih.bmiHeader);
    vih.bmiHeader.biClrImportant = 0;
    vih.bmiHeader.biClrUsed      = 0;
    vih.bmiHeader.biXPelsPerMeter = 0;
    vih.bmiHeader.biYPelsPerMeter = 0;
    vih.AvgTimePerFrame          = 1000000;

    pMediaType->SetType(&MEDIATYPE_Video);
    pMediaType->SetFormatType(&FORMAT_VideoInfo);
    pMediaType->SetFormat( (BYTE*) &vih, sizeof( vih ) );
    pMediaType->SetSubtype(&MEDIASUBTYPE_YV12);
    pMediaType->SetSampleSize(vih.bmiHeader.biSizeImage);

    return NOERROR;

} // GetMediaType



////////////////////////////////////////////////////////////////////////
//
// Exported entry points for registration and unregistration 
// (in this case they only call through to default implementations).
//
////////////////////////////////////////////////////////////////////////

/*
STDAPI DllRegisterServer() {
  return AMovieDllRegisterServer2( TRUE );
}

STDAPI DllUnregisterServer() {
  return AMovieDllRegisterServer2( FALSE );
}
*/


//
// DllEntryPoint
//
/*
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved) {

	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}
*/


// Microsoft C Compiler will give hundreds of warnings about
// unused inline functions in header files.  Try to disable them.
#pragma warning(disable:4514)
