//
// lresizer.cpp
//
// lresizer

#include <streams.h>     // DirectShow (includes windows.h)

#include <initguid.h>    // declares DEFINE_GUID to declare an EXTERN_C const.
                         // Use once per project.
#include <atlbase.h>
#include "lresizer.h"

//
// Common DLL routines and tables
//

// Object table - all com objects in this DLL
CFactoryTemplate g_Templates[]=
{   { L"lresizer"
    , &CLSID_StretchDIBitsResizer
    , CStretchDIBitsResizer::CreateInstance
    , NULL
    , &CStretchDIBitsResizer::sudFilter 
    }
};
int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);

// The streams.h DLL entrypoint.
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

// The entrypoint required by the MSVC runtimes. This is used instead
// of DllEntryPoint directly to ensure global C++ classes get initialised.
BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved) {

    return DllEntryPoint(reinterpret_cast<HINSTANCE>(hDllHandle), dwReason, lpreserved);
}


STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);
}



//
// Filter lresizer routines
//
const AMOVIESETUP_MEDIATYPE CStretchDIBitsResizer::sudInputTypes[] = {
   { &MEDIATYPE_Video, &GUID_NULL },
};

const AMOVIESETUP_MEDIATYPE CStretchDIBitsResizer::sudOutputTypes[] = {
   { &MEDIATYPE_Video, &GUID_NULL },
};

const AMOVIESETUP_PIN CStretchDIBitsResizer::sudPins[] = { {
   L"",
      FALSE,          // Is this pin rendered?
      FALSE,          // Is it an output pin?
      FALSE,          // Can the filter create zero instances?
      FALSE,          // Does the filter create multiple instances?
      &GUID_NULL,     // Obsolete.
      NULL,           // Obsolete.
      1,              // Number of media types.
      CStretchDIBitsResizer::sudInputTypes   // Pointer to media types.
}, {
   L"",
      FALSE,          // Is this pin rendered?
      TRUE,           // Is it an output pin?
      FALSE,          // Can the filter create zero instances?
      FALSE,          // Does the filter create multiple instances?
      &GUID_NULL,     // Obsolete.
      NULL,           // Obsolete.
      1,              // Number of media types.
      CStretchDIBitsResizer::sudOutputTypes   // Pointer to media types.

} };

// setup data - allows the self-registration to work.
const AMOVIESETUP_FILTER CStretchDIBitsResizer::sudFilter = {
    g_Templates[0].m_ClsID    // clsID
  , g_Templates[0].m_Name     // strName
  , MERIT_NORMAL              // dwMerit
  // TODO: Add pin details here.
  , 2                         // nPins
  , CStretchDIBitsResizer::sudPins // lpPin
};  


//
// Constructor
//
// If any part of construction fails *phr should be set to 
// a failure code.
CStretchDIBitsResizer::CStretchDIBitsResizer(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
: CTransformFilter(tszName, punk, *sudFilter.clsID)
{
   m_outTypeInitialized = false;
   m_outTypeHasChanged = true;
   m_mtOut.InitMediaType();
   m_mtIn.InitMediaType();
   m_nStretchMode = RESIZEF_STRETCH;

   m_hInBitmap   = NULL;
   m_hInDC       = NULL;
   m_pInBits     = NULL;
   m_inBitsSize  = 0;
   
   m_hOutBitmap  = NULL;
   m_hOutDC      = NULL;
   m_pOutBits    = NULL;
   m_outBitsSize = 0;
}


//
// Destructor
//
CStretchDIBitsResizer::~CStretchDIBitsResizer() 
{
   FreeResources();
}

void CStretchDIBitsResizer::FreeResources()
{
   if (m_hInBitmap)
   {
      ::DeleteObject(m_hInBitmap);
      ::DeleteDC(m_hInDC);

      m_hInBitmap = NULL;
      m_hInDC     = NULL;
      m_pInBits   = NULL;
   }

   if (m_hOutBitmap)
   {
      ::DeleteObject(m_hOutBitmap);
      ::DeleteDC(m_hOutDC);

      m_hOutBitmap = NULL;
      m_hOutDC     = NULL;
      m_pOutBits   = NULL;
   }
}

//
// CreateInstance
//
// Provide the way for the COM support routines in <streams.h>
// to create a CStretchDIBitsResizer object
CUnknown * WINAPI CStretchDIBitsResizer::CreateInstance(LPUNKNOWN punk, HRESULT *phr) {

    CUnknown *pNewObject = new CStretchDIBitsResizer(NAME("lresizer Object"), punk, phr );
    if (pNewObject == NULL) {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
}

HRESULT CStretchDIBitsResizer::NonDelegatingQueryInterface(REFIID riid, LPVOID *ppv)
{
   if (riid == IID_IResize) 
   {
      return GetInterface((IResize *) this, ppv);
   } 
   else 
   {
      return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
   }
}

/*
//
// GetPinCount
//
// return the number of pins this filter has
int CStretchDIBitsResizer::GetPinCount() {

    // TODO: replace this if you create variable numbers of pins at runtime
    return sudFilter.nPins;
}


//
// GetPin
//
// return pointer to a pin.
CBasePin* CStretchDIBitsResizer::GetPin(int n) {

    // TODO: Create each pin, and return
    // a pointer to the requested pin.
    return NULL;
}
*/

HRESULT CStretchDIBitsResizer::CheckInputType(const CMediaType *pmtIn)
{
   if ((*(pmtIn->Type()) == MEDIATYPE_Video) &&
       (*(pmtIn->FormatType()) == FORMAT_VideoInfo))
   {
      VIDEOINFOHEADER *pVH = (VIDEOINFOHEADER *) pmtIn->Format();

      if ((pVH->bmiHeader.biCompression == BI_RGB) ||
          (pVH->bmiHeader.biCompression == BI_BITFIELDS))
         return S_OK;
   }
   
   return VFW_E_INVALIDMEDIATYPE;
}

HRESULT CStretchDIBitsResizer::CheckTransform(const CMediaType *pmtIn, const CMediaType *pmtOut)
{
   HRESULT hr = CheckInputType(pmtIn);
   if (SUCCEEDED(hr))
      m_mtIn = *pmtIn;
   if (SUCCEEDED(hr))
      hr = CheckInputType(pmtOut);
   if (SUCCEEDED(hr))
   {
      m_mtOut = *pmtOut;
      m_outTypeInitialized = true;
      m_outTypeHasChanged  = true;
   }

   return hr;
}

HRESULT CStretchDIBitsResizer::DecideBufferSize(IMemAllocator *pAlloc, 
                                                ALLOCATOR_PROPERTIES *ppropInputRequest)
{
   BITMAPINFOHEADER *pBih = HEADER(m_mtOut.Format());

   if (ppropInputRequest->cBuffers == 0)
      ppropInputRequest->cBuffers = 1;
   if (ppropInputRequest->cbAlign != 4)
      ppropInputRequest->cbAlign = 4;

   ppropInputRequest->cbBuffer = pBih->biSizeImage;
   
   ALLOCATOR_PROPERTIES actualProps;
   HRESULT hr = pAlloc->SetProperties(ppropInputRequest, &actualProps);
   if (FAILED(hr))
      return hr;

   if (ppropInputRequest->cbBuffer > actualProps.cbBuffer)
   {
      // We cannot accept smaller buffers than requested!
      return E_FAIL;
   }
   return S_OK;
}

HRESULT CStretchDIBitsResizer::GetMediaType(int iPosition, CMediaType *pMediaType)
{
   if (iPosition < 0)
      return E_INVALIDARG;

   if (iPosition == 0)
   {
      *pMediaType = m_mtOut;
      return S_OK;
   }

   return VFW_S_NO_MORE_ITEMS;
}

HRESULT CStretchDIBitsResizer::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
   BITMAPINFOHEADER *pBihIn = HEADER(m_mtIn.Format());
   BITMAPINFOHEADER *pBihOut = HEADER(m_mtOut.Format());

   if (m_outTypeHasChanged)
   {
      // Free any resources
      FreeResources();

      // And reallocate them:
      // We need some in- and output bitmaps...
      //m_hInDC  = ::CreateCompatibleDC(NULL);
      m_hOutDC = ::CreateCompatibleDC(NULL);

      /*
      m_hInBitmap = ::CreateDIBSection(
         m_hInDC,
         (BITMAPINFO *) pBihIn,
         DIB_RGB_COLORS,
         &m_pInBits,
         NULL, 0 // ignored
         );
      */
      m_hOutBitmap = ::CreateDIBSection(
         m_hOutDC,
         (BITMAPINFO *) pBihOut,
         DIB_RGB_COLORS,
         &m_pOutBits,
         NULL, 0 // ignored
         );

      //ASSERT(m_hInBitmap);
      ASSERT(m_hOutBitmap);

      m_inBitsSize = pBihIn->biSizeImage;
      m_outBitsSize = pBihOut->biSizeImage;

      //::SelectObject(m_hInDC, m_hInBitmap);
      ::SelectObject(m_hOutDC, m_hOutBitmap);

      // Now we "only" have to calculate the correct
      // parameters for the StretchDIBits command:
      CalculateStretchParameters();

      if (m_nStretchMode != RESIZEF_CROP)
      {
         // If we really have to resize something,
         // let's set the stretch mode to HALFTONE
         // which renders nicely resized images. This
         // is somewhat slower than other implementations,
         // but it does look much better.
         ::SetStretchBltMode(m_hOutDC, HALFTONE);
         ::SetBrushOrgEx(m_hOutDC, m_destX, m_destY, NULL);
      }
      else
      {
         // If we are in Crop mode, don't use the
         // HALFTONE mode as it may result in strange
         // pictures, even if a picture is not resized
         // at all (smoothing).
         ::SetStretchBltMode(m_hOutDC, COLORONCOLOR);
      }

      m_outTypeHasChanged = false;
   }

   BYTE *pInBytes = NULL;
   BYTE *pOutBytes = NULL;
   HRESULT hr = pIn->GetPointer(&pInBytes);
   if (SUCCEEDED(hr))
      hr = pOut->GetPointer(&pOutBytes);

   // This is faster than a FillRect:-)
   ZeroMemory(m_pOutBits, m_outBitsSize);

   ::StretchDIBits(
      m_hOutDC, 
      m_destX, //0,
      m_destY, //0,
      m_destWidth, //pBihIn->biWidth,
      m_destHeight, //pBihIn->biHeight,
      m_srcX, //0,
      m_srcY, //0,
      m_srcWidth, //pBihIn->biWidth,
      m_srcHeight, //pBihIn->biHeight,
      pInBytes,
      (BITMAPINFO *) pBihIn,
      DIB_RGB_COLORS,
      SRCCOPY);

   memcpy(pOutBytes, m_pOutBits, m_outBitsSize);

   return hr;
}

void CStretchDIBitsResizer::CalculateStretchParameters()
{
   BITMAPINFOHEADER *pBihIn = HEADER(m_mtIn.Format());
   BITMAPINFOHEADER *pBihOut = HEADER(m_mtOut.Format());

   int inWidth = pBihIn->biWidth;
   int inHeight = pBihIn->biHeight;
   int outWidth = pBihOut->biWidth;
   int outHeight = pBihOut->biHeight;

   // Init everything to 0
   m_destX = 0;
   m_destY = 0;
   m_destWidth = 0;
   m_destHeight = 0;
   m_srcX = 0;
   m_srcY = 0;
   m_srcWidth = 0;
   m_srcHeight = 0;

   double yFactor = ((double) outHeight) / ((double) inHeight);
   double xFactor = ((double) outWidth) / ((double) inWidth);

   switch (m_nStretchMode)
   {
   case RESIZEF_STRETCH:
      m_destX = 0;
      m_destY = 0;
      m_destWidth = outWidth;
      m_destHeight = outWidth;
      m_srcX  = 0;
      m_srcY  = 0;
      m_srcWidth = inWidth;
      m_srcHeight = inWidth;
      break;

   case RESIZEF_CROP:
      // Handle X pos and width first
      if (outWidth >= inWidth)
      {
         // No cropping, center image
         m_srcX      = 0;
         m_srcWidth  = inWidth;
         m_destX     = (outWidth - inWidth) / 2;
         m_destWidth = inWidth;
      }
      else
      {
         // We need to crop, in image too large
         m_srcX      = (inWidth - outWidth) / 2;
         m_srcWidth  = outWidth;
         m_destX     = 0;
         m_destWidth = outWidth;
      }

      // Now Y pos and height
      if (outHeight >= inHeight)
      {
         m_srcY       = 0;
         m_srcHeight  = inHeight;
         m_destY      = (outHeight - inHeight) / 2;
         m_destHeight = inHeight;
      }
      else
      {
         // Crop height
         m_srcY       = (inHeight - outHeight) / 2;
         m_srcHeight  = outHeight;
         m_destY      = 0;
         m_destHeight = outHeight;
      }
      break;

   case RESIZEF_PRESERVEASPECTRATIO:
      {
         // We want the whole in image
         m_srcX = 0;
         m_srcY = 0;
         m_srcWidth = inWidth;
         m_srcHeight = inHeight;
         double minFactor = yFactor < xFactor ? yFactor : xFactor;
         m_destWidth = (int) (minFactor * inWidth);
         m_destHeight = (int) (minFactor * inHeight);
         m_destX = (outWidth - m_destWidth) / 2;
         m_destY = (outHeight - m_destHeight) / 2;
      }
      break;

   case RESIZEF_PRESERVEASPECTRATIO_NOLETTERBOX:
      {
         // We want the whole out image
         m_destX = 0;
         m_destY = 0;
         m_destWidth = outWidth;
         m_destHeight = outHeight;
         m_srcX = 0;
         m_srcY = 0;
         m_srcWidth = inWidth;
         m_srcHeight = inHeight;
      }
      break;
   }
}

HRESULT CStretchDIBitsResizer::get_InputSize(int *piHeight, int *piWidth)
{
   if (piHeight == NULL || piWidth == NULL)
      return E_POINTER;

   BITMAPINFOHEADER *pBih = HEADER(m_mtIn.Format());

   *piHeight = pBih->biHeight;
   *piWidth  = pBih->biWidth;

   return S_OK;
}

HRESULT CStretchDIBitsResizer::get_MediaType(AM_MEDIA_TYPE *pmt)
{
   FreeMediaType(*pmt);

   HRESULT hr = CopyMediaType(pmt, &m_mtOut);

   return hr;
}

HRESULT CStretchDIBitsResizer::get_Size(int *piHeight, int *piWidth, long *pFlag)
{
   if (piHeight == NULL || piWidth == NULL || pFlag == NULL)
      return E_POINTER;

   BITMAPINFOHEADER *pBih = HEADER(m_mtOut.Format());

   *piHeight = pBih->biHeight;
   *piWidth  = pBih->biWidth;

   *pFlag    = m_nStretchMode;

   return S_OK;
}

HRESULT CStretchDIBitsResizer::put_MediaType(const AM_MEDIA_TYPE *pmt)
{
   CMediaType mtTemp(*pmt);
   HRESULT hr = CheckInputType(&mtTemp);
   if (SUCCEEDED(hr))
   {
      m_mtOut = mtTemp;
      m_outTypeInitialized = true;
      m_outTypeHasChanged = true;
   }

   return hr;
}

HRESULT CStretchDIBitsResizer::put_Size(int iHeight, int iWidth, long iFlags)
{
   m_nStretchMode = iFlags;

   if (m_outTypeInitialized)
   {
      BITMAPINFOHEADER *pBih = HEADER(m_mtOut.Format());
      pBih->biWidth = iWidth;
      pBih->biHeight = iHeight;
      pBih->biSizeImage = DIBSIZE(*pBih);
   }
   else
   {
      BITMAPINFOHEADER bmi;
      bmi.biSize = sizeof BITMAPINFOHEADER;
      bmi.biCompression = BI_RGB;
      bmi.biClrImportant = 0;
      bmi.biClrUsed = 0;
      bmi.biHeight = iHeight;
      bmi.biWidth = iWidth;
      bmi.biPlanes = 1;
      bmi.biBitCount = 24;
      bmi.biXPelsPerMeter = 96;
      bmi.biYPelsPerMeter = 96;
      bmi.biSizeImage = DIBSIZE(bmi);

      m_mtOut.SetType(&MEDIATYPE_Video);
      m_mtOut.SetSubtype(&MEDIASUBTYPE_RGB24);
      m_mtOut.SetFormatType(&FORMAT_VideoInfo);
      m_mtOut.SetTemporalCompression(FALSE);
      m_mtOut.SetSampleSize(bmi.biSizeImage);
      m_mtOut.SetFormat((BYTE *) &bmi, sizeof BITMAPINFOHEADER);
   }

   m_outTypeHasChanged = true;

   HRESULT hr = S_OK;

   // Do we have to reconnect the output pin?
   if (m_pOutput)
   {
      // We do have an output pin already,
      // so let's check if it's connected to anything
      CComPtr<IPin> pPin2OutPin;
      hr = m_pOutput->ConnectedTo(&pPin2OutPin);
      if (SUCCEEDED(hr))
      {
         // Yes, that pin is already connected.
         // Now reconnect it:
         CComQIPtr<IPin> pOutPin(m_pOutput);
         ASSERT(pOutPin);
         if (SUCCEEDED(hr))
            hr = ReconnectPin(pOutPin, &m_mtOut);
      }
      else if (hr == VFW_E_NOT_CONNECTED)
      {
         hr = S_OK;
      }
   }

   return hr;
}
