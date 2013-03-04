#include "StdAfx.h"

#include "LecturnitySource.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// CClipInfo Class
//////////////////////////////////////////////////////////////////////

CClipInfo::CClipInfo()
{
   m_tszClipFilename = NULL;
   m_dFrameRate = 0.0;
   m_iStartMs = 0;
   m_nLengthMs = 0;
   m_pAviGetFrame = NULL;
}

/*
CClipInfo::CClipInfo(CClipInfo *pSource)
{
   int iNameLength = _tcslen(pSource->m_tszClipFilename);
   m_tszClipFilename = new _TCHAR[iNameLength + 1];
   ZeroMemory(m_tszClipFilename, iNameLength + 1);
   _tcsncpy(m_tszClipFilename, pSource->m_tszClipFilename, iNameLength);

   m_dFrameRate = pSource->m_dFrameRate;
   m_nStartMs = pSource->m_nStartMs;
   m_nLengthMs = pSource->m_nLengthMs;
   m_pAviGetFrame = NULL; // must be opened for/by the new object
}
*/

CClipInfo::~CClipInfo()
{
   if (m_tszClipFilename != NULL)
      delete[] m_tszClipFilename;
   m_tszClipFilename = NULL;

   if (m_pAviGetFrame != NULL)
      Close();
}

HRESULT CClipInfo::Init(LPCTSTR tszClipFilename, int iStartMs)
{
   HRESULT hr = S_OK;

   if (tszClipFilename == NULL)
      return E_POINTER;
   
   PAVIFILE pAviFile = NULL;
   hr = AVIFileOpen(&pAviFile, tszClipFilename, OF_READ, NULL);
   
   PAVISTREAM pAviStream = NULL;
   if (SUCCEEDED(hr))
      hr = AVIFileGetStream(pAviFile, &pAviStream, streamtypeVIDEO, 0);
   
   AVIFILEINFO aviFileInfo;
   if (SUCCEEDED(hr))
      hr = AVIFileInfo(pAviFile, &aviFileInfo, sizeof AVIFILEINFO);
   
   if (SUCCEEDED(hr))
   {
      m_iStartMs = iStartMs;
      m_dFrameRate = aviFileInfo.dwRate/(double)aviFileInfo.dwScale;
      m_nLengthMs = (UINT)((aviFileInfo.dwLength*(1000/m_dFrameRate)));

      int iNameLength = _tcslen(tszClipFilename);
      m_tszClipFilename = new _TCHAR[iNameLength + 1];
      ZeroMemory(m_tszClipFilename, iNameLength + 1);
      _tcsncpy(m_tszClipFilename, tszClipFilename, iNameLength);
      m_tszClipFilename[iNameLength] = 0; // strangely _tcsncpy seems to break the ZeroMemory...
   }

   if (pAviFile != NULL)
      AVIFileClose(pAviFile);
   
   return hr;
}

bool CClipInfo::ContainsMediaTime(int iMediaMs)
{
   return iMediaMs >= m_iStartMs && iMediaMs < m_iStartMs + m_nLengthMs;
}

LONG CClipInfo::GetFrameNumber(UINT nMediaMs)
{
   double dPosSecond = ((signed)nMediaMs - m_iStartMs) / 1000.0;

   if (dPosSecond < 0)
      dPosSecond = 0;
   if (dPosSecond >= m_nLengthMs / 1000.0)
      dPosSecond = (m_nLengthMs - 1) / 1000.0;
   // requesting a frame after the length will return NULL (below)
   // "-1": otherwise it will be one over the length
       
   return (LONG)(dPosSecond * m_dFrameRate);
}


   
PGETFRAME CClipInfo::Open()
{
   if (m_pAviGetFrame == NULL)
   {
      PAVIFILE pAviFile = NULL;
      HRESULT hr = AVIFileOpen(&pAviFile, m_tszClipFilename, OF_READ, NULL);
      
      PAVISTREAM pAviStream = NULL;
      if (SUCCEEDED(hr))
         hr = AVIFileGetStream(pAviFile, &pAviStream, streamtypeVIDEO, 0);
     
      if (SUCCEEDED(hr))
         m_pAviGetFrame = AVIStreamGetFrameOpen(pAviStream, NULL);

      if (SUCCEEDED(hr) && m_pAviGetFrame == NULL)
      {
         // For some codecs (DivX, Xvid) specifying no format (NULL) above
         // does not work: Try again with a format set.
         //
         // Code from NativeUtils.

         AVIFILEINFO aviFileInfo;
         ZeroMemory(&aviFileInfo, sizeof AVIFILEINFO);
         HRESULT hr2 = AVIFileInfo(pAviFile, &aviFileInfo, sizeof AVIFILEINFO);
   
         BITMAPINFO *pBMI = NULL;
         LONG cbBMI = 0;

         if (SUCCEEDED(hr2))
            hr2 = AVIStreamFormatSize(pAviStream, 0, &cbBMI);
         if (SUCCEEDED(hr2))
         {
            pBMI = (BITMAPINFO *) (new BYTE[cbBMI]);
            ZeroMemory(pBMI, cbBMI);
            hr2 = AVIStreamReadFormat(pAviStream, 0, pBMI, &cbBMI);
         }
         if (SUCCEEDED(hr2))
         {
            pBMI->bmiHeader.biSize = sizeof BITMAPINFOHEADER;
            pBMI->bmiHeader.biWidth = aviFileInfo.dwWidth;
            pBMI->bmiHeader.biHeight = aviFileInfo.dwHeight;
            pBMI->bmiHeader.biPlanes = 1;
            pBMI->bmiHeader.biBitCount = 24;
            pBMI->bmiHeader.biCompression = BI_RGB;
            int iPadding = (3*pBMI->bmiHeader.biWidth) % 4 == 0 ? 0 : 4 - (3*pBMI->bmiHeader.biWidth) % 4;
            pBMI->bmiHeader.biSizeImage = 
               pBMI->bmiHeader.biWidth * 3 * pBMI->bmiHeader.biHeight + pBMI->bmiHeader.biHeight * iPadding;
            
            // Try again with specifc BITMAPINFO now
            m_pAviGetFrame = AVIStreamGetFrameOpen(pAviStream, &pBMI->bmiHeader);
         }
         
         if (pBMI)
            delete[] pBMI;
         pBMI = NULL;
      }
   }
   
   return m_pAviGetFrame;
}

void CClipInfo::Close()
{
   if (m_pAviGetFrame != NULL)
      AVIStreamGetFrameClose(m_pAviGetFrame);
   m_pAviGetFrame = NULL;
}


//////////////////////////////////////////////////////////////////////
// CLecturnitySource Class
//////////////////////////////////////////////////////////////////////

STDMETHODIMP CLecturnitySource::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
   if (ppv == NULL)
      return E_POINTER;

    if (riid == IID_ILecturnitySourceControl) {
        return GetInterface((ILecturnitySourceControl *) this, ppv);

    } else {
        return CSource::NonDelegatingQueryInterface(riid, ppv);
    }
}

CLecturnitySource::CLecturnitySource(LPUNKNOWN lpunk, HRESULT *phr) :
   CSource(_T("Whiteboard Video Source"), lpunk, CLSID_LecturnitySource)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   m_paStreams = (CSourceStream **) new CLecturnityVideoStream*[1];
   
   m_paStreams[0] = new CLecturnityVideoStream(phr, this, L"Whiteboard Video Data");
}

CLecturnitySource::~CLecturnitySource()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   // delete[] m_paStreams; // should be done by ~CSource
}

// static
CUnknown * WINAPI CLecturnitySource::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   
   ASSERT(phr);
   
   CUnknown *punk = new CLecturnitySource(lpunk, phr);
   return punk;
}

STDMETHODIMP CLecturnitySource::SetVideoSizes(UINT nWidth, UINT nHeight, UINT nInputWidth, UINT nInputHeight)
{
   if (nWidth == 0 || nHeight == 0)
      return E_INVALIDARG;

   if (nWidth % 4 != 0 || nHeight % 4 != 0) // require an mpeg4 compatible size
      return E_INVALIDARG;

   if (m_paStreams != NULL)
      return ((CLecturnityVideoStream *)m_paStreams[0])->SetVideoSizes(nWidth, nHeight, nInputWidth, nInputHeight);
   else
      return E_FAIL;
}

STDMETHODIMP CLecturnitySource::SetFrameRate(float fFrameRate)
{
   if (fFrameRate <= 0)
      return E_INVALIDARG;
  
   if (m_paStreams != NULL)
      return ((CLecturnityVideoStream *)m_paStreams[0])->SetFrameRate(fFrameRate);
   else
      return E_FAIL;
}

STDMETHODIMP CLecturnitySource::SetIsEvaluation(bool bIsEvaluation)
{
   if (m_paStreams != NULL)
      return ((CLecturnityVideoStream *)m_paStreams[0])->SetIsEvaluation(bIsEvaluation);
   else
      return E_FAIL;
}
 
STDMETHODIMP CLecturnitySource::SetMinimumLength(UINT nLengthMs)
{
   if (m_paStreams != NULL)
      return ((CLecturnityVideoStream *)m_paStreams[0])->SetMinimumLength(nLengthMs);
   else
      return E_FAIL;
}


STDMETHODIMP CLecturnitySource::ParseFiles(LPCTSTR tszEvqFilename, LPCTSTR tszObjFilename)
{
   if (tszEvqFilename == NULL || tszObjFilename == NULL)
      return E_POINTER;

   if (m_paStreams != NULL)
      return ((CLecturnityVideoStream *)m_paStreams[0])->ParseFiles(tszEvqFilename, tszObjFilename);
   else
      return E_FAIL;
}

STDMETHODIMP CLecturnitySource::AddClip(LPCTSTR tszClipName, int iClipStartMs)
{
   if (tszClipName == NULL)
      return E_POINTER;

   if (m_paStreams != NULL)
      return ((CLecturnityVideoStream *)m_paStreams[0])->AddClip(tszClipName, iClipStartMs);
   else
      return E_FAIL;
}

STDMETHODIMP CLecturnitySource::GetLastSampleTime(UINT *pnSampleTimeMs)
{
   if (pnSampleTimeMs == NULL)
      return E_POINTER;
   
   if (m_paStreams != NULL)
      return ((CLecturnityVideoStream *)m_paStreams[0])->GetLastSampleTime(pnSampleTimeMs);
   else
      return E_FAIL;
}




//////////////////////////////////////////////////////////////////////
// CLecturnityVideoStream Class
//////////////////////////////////////////////////////////////////////

CLecturnityVideoStream::CLecturnityVideoStream(HRESULT *phr,
                                               CSource *pParent, LPCWSTR pPinName) :
    CSourceStream(_T("Whiteboard Video Source"), phr, pParent, pPinName)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   m_rtSampleTime = 0;

   // default values
   m_iRepeatTime = 200;
   m_nOutputWidth = 320;
   m_nOutputHeight = 240;
   m_dZoomFactor = 0.44444444; // for 720x540 input

   m_pWhiteboard = NULL;
   m_bWhiteboardOpened = false;
   m_pBitmap = NULL;
   m_bIsEvaluation = false;
   m_nMinimumLengthMs = 0;

   // Initialize GDI+:
   Gdiplus::GdiplusStartup(&m_gdipToken, &m_gdipStartupInput, NULL);

   ::AVIFileInit();

   m_pLastCurrentClip = NULL;

   // m_pClipDet is a CComPtr
   m_bClipWasActive = false;
}

CLecturnityVideoStream::~CLecturnityVideoStream()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (m_pBitmap)
      delete m_pBitmap;

   if (m_pWhiteboard)
      delete m_pWhiteboard; // do this before gdi+ shutdown

   // Stop GDI+
   Gdiplus::GdiplusShutdown(m_gdipToken);

   for (int i=0; i<m_aAllClips.GetSize(); ++i)
      delete m_aAllClips[i];

   ::AVIFileExit();
}

// private
HRESULT CLecturnityVideoStream::CalculateDrawingRegion(UINT nOutputWidth, UINT nOutputHeight, 
                                                       UINT nInputWidth, UINT nInputHeight,
                                                       CRect *prcTarget, double *pdZoomFactor)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   if (prcTarget == NULL)
      return E_POINTER;

   double dZoom = 1.0;
   double dZoomFactorX = (double)nOutputWidth / nInputWidth;
   double dZoomFactorY = (double)nOutputHeight / nInputHeight;
   if (dZoomFactorX > dZoomFactorY)
      dZoom = dZoomFactorY;
   else
      dZoom = dZoomFactorX;

   // do not upscale
   if (dZoom > 1.0)
      dZoom = 1.0;
   
   // calculate drawing rectangle
   prcTarget->left = (int)((double)nOutputWidth - (nInputWidth * dZoom)) / 2;
   prcTarget->top = (int)((double)nOutputHeight - (nInputHeight * dZoom)) / 2;
   prcTarget->right = prcTarget->left + (int)(nInputWidth * dZoom);
   prcTarget->bottom = prcTarget->top + (int)(nInputHeight * dZoom);

   if (pdZoomFactor != NULL)
      *pdZoomFactor = dZoom;

   return S_OK;
}

HRESULT CLecturnityVideoStream::SetVideoSizes(UINT nOutputWidth, UINT nOutputHeight, UINT nInputWidth, UINT nInputHeight)
{
   if (nOutputWidth == 0 || nOutputHeight == 0)
      return E_INVALIDARG;

   if (nInputWidth == 0 || nInputHeight == 0)
      return E_INVALIDARG;

   if (nOutputWidth % 4 != 0 || nOutputHeight % 4 != 0) // require an mpeg4 compatible size
      return E_INVALIDARG;

   m_nOutputWidth = nOutputWidth;
   m_nOutputHeight = nOutputHeight;

   m_dZoomFactor = 1.0;

   CalculateDrawingRegion(nOutputWidth, nOutputHeight, 
      nInputWidth, nInputHeight, &m_rcWbArea, &m_dZoomFactor);
 
   m_pBitmap = new Gdiplus::Bitmap(m_nOutputWidth, m_nOutputHeight, PixelFormat32bppARGB);
   
   return S_OK;
}

HRESULT CLecturnityVideoStream::SetFrameRate(float fFrameRate)
{
   if (fFrameRate <= 0)
      return E_INVALIDARG;

   int iTimeForFrameMs = (int)(1000.0f/fFrameRate);

   if (iTimeForFrameMs < 10 || iTimeForFrameMs > 100000)
      return E_INVALIDARG; // require resonable values: 100 fps - 0.01 fps

   m_iRepeatTime = iTimeForFrameMs;

   return S_OK;
}

HRESULT CLecturnityVideoStream::SetIsEvaluation(bool bIsEvaluation)
{
   m_bIsEvaluation = bIsEvaluation;

   return S_OK;
}

HRESULT CLecturnityVideoStream::SetMinimumLength(UINT nLengthMs)
{
   m_nMinimumLengthMs = nLengthMs;

   return S_OK;
}

HRESULT CLecturnityVideoStream::ParseFiles(LPCTSTR tszEvqFilename, LPCTSTR tszObjFilename)
{
   if (tszEvqFilename == NULL || tszObjFilename == NULL)
      return E_POINTER;

   HRESULT hr = S_OK;

   if (m_pWhiteboard)
      delete m_pWhiteboard;

   m_pWhiteboard = new CWhiteboardStream();
   m_pWhiteboard->SetObjFilename(tszObjFilename);
   m_pWhiteboard->SetEvqFilename(tszEvqFilename);

   if (!m_bWhiteboardOpened)
   {
      hr = m_pWhiteboard->Open();
      
      if (SUCCEEDED(hr))
         m_bWhiteboardOpened = true;
   }
 
   return hr;
}

HRESULT CLecturnityVideoStream::AddClip(LPCTSTR tszClipName, int iClipStartMs)
{
   CClipInfo *pClip = new CClipInfo();
   HRESULT hr = pClip->Init(tszClipName, iClipStartMs);

   _tprintf(_T("%s: %d\n"), tszClipName, iClipStartMs);

   if (SUCCEEDED(hr))
      m_aAllClips.Add(pClip);

   return hr;
}

HRESULT CLecturnityVideoStream::GetLastSampleTime(UINT *pnSampleTimeMs)
{
   if (pnSampleTimeMs == NULL)
      return E_POINTER;

   *pnSampleTimeMs = 0;
   if (m_rtSampleTime.Millisecs() >= m_iRepeatTime)
       *pnSampleTimeMs = m_rtSampleTime.Millisecs() - m_iRepeatTime;
      // m_rtSampleTime already contains the time for the next sample
   
   return S_OK;
}


HRESULT CLecturnityVideoStream::GetMediaType(CMediaType *pMediaType)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CheckPointer(pMediaType,E_POINTER);
   
   VIDEOINFO *pvi = (VIDEOINFO *) pMediaType->AllocFormatBuffer(sizeof(VIDEOINFO));
   
   ZeroMemory(pvi, sizeof(VIDEOINFO));
   
   pvi->bmiHeader.biCompression = BI_RGB;
   pvi->bmiHeader.biBitCount    = 32;
  
   pvi->bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
   pvi->bmiHeader.biWidth      = m_nOutputWidth;
   pvi->bmiHeader.biHeight     = m_nOutputHeight;
   pvi->bmiHeader.biPlanes     = 1;
   pvi->bmiHeader.biSizeImage  = GetBitmapSize(&pvi->bmiHeader);
   pvi->bmiHeader.biClrImportant = 0;
   
   SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.
   SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle
   
   pMediaType->SetType(&MEDIATYPE_Video);
   pMediaType->SetFormatType(&FORMAT_VideoInfo);
   pMediaType->SetTemporalCompression(FALSE);
   
   // Work out the GUID for the subtype from the header info.
   const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
   pMediaType->SetSubtype(&SubTypeGUID);
   pMediaType->SetSampleSize(pvi->bmiHeader.biSizeImage);
   
   return S_OK;
} // GetMediaType


HRESULT CLecturnityVideoStream::CheckMediaType(const CMediaType *pMediaType)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CheckPointer(pMediaType,E_POINTER);
   
   if((*(pMediaType->Type()) != MEDIATYPE_Video) ||   // we only accept output video
      !(pMediaType->IsFixedSize()))                   // in fixed size samples
   {                                                  
      return E_INVALIDARG;
   }
   
   const GUID *SubType = pMediaType->Subtype();
   if (SubType == NULL)
      return E_INVALIDARG;
   
   if(*SubType != MEDIASUBTYPE_RGB32)
   {
      return E_INVALIDARG;
   }
   
   VIDEOINFO *pvi = (VIDEOINFO *) pMediaType->Format();
   
   if(pvi == NULL)
      return E_INVALIDARG;
  
   if((pvi->bmiHeader.biWidth != (signed)m_nOutputWidth) 
      || ( abs(pvi->bmiHeader.biHeight) != (signed)m_nOutputHeight))
   {
      return E_INVALIDARG;
   }
  
   return S_OK;  // This format is acceptable.
   
} // CheckMediaType


/*
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

   Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}
//*/

// private
Gdiplus::Bitmap* CLecturnityVideoStream::RetrieveClipFrame(UINT nMediaMs)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   HRESULT hr = S_OK;

   UINT nStartMs = 10000;


   // Using and storing of an IMediaDet about different method calls does not
   // seem to work. Recreating it every time is very time consuming.
   // Two alternatives: Use a filter for selecting between different image inputs
   // (whiteboard data OR clip frames).
   // Or use other means for obtaining video images like the old AVIFile functions.

   m_pClipDet = NULL; // !!!!
   if (m_pClipDet == NULL)
   {
      hr = ::CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, IID_IMediaDet, (void **)&m_pClipDet);
   
      // method is not used....
      CComBSTR bstrClipname(_T("D:\\LECTURNITY\\Aufzeichnungen\\BrauchbareDreiSeiten_mitPointer\\mini_clip.avi"));
      if (SUCCEEDED(hr))
         hr = m_pClipDet->put_Filename(bstrClipname);
   
      // TODO search for video
      if (SUCCEEDED(hr))
         hr = m_pClipDet->put_CurrentStream(0); // only one stream in a clip file
   }

   double dStreamLength = 0;
   if (SUCCEEDED(hr))
      hr = m_pClipDet->get_StreamLength(&dStreamLength);

   if (SUCCEEDED(hr))
   {
      if (nMediaMs < nStartMs || nMediaMs > nStartMs + (dStreamLength * 1000))
      {
         _RPT1(_CRT_WARN, "%d -> NULL\n", nMediaMs);
         return NULL;
      }
   }
   
   AM_MEDIA_TYPE mt;
   ZeroMemory(&mt, sizeof mt);
   if (SUCCEEDED(hr))
      hr = m_pClipDet->get_StreamMediaType(&mt);
   
   _ASSERT(SUCCEEDED(hr));

   long lClipWidth = 0, lClipHeight = 0;
   BYTE *pClipBuffer = NULL;
   if (SUCCEEDED(hr) && mt.formattype == FORMAT_VideoInfo)
   {
      VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)(mt.pbFormat);
      lClipWidth = pVih->bmiHeader.biWidth;
      lClipHeight = pVih->bmiHeader.biHeight;
      
      long lSize = 0;
      hr = m_pClipDet->GetBitmapBits(0.0, &lSize, NULL, lClipWidth, lClipHeight);
      
      _ASSERT(SUCCEEDED(hr));

      if (SUCCEEDED(hr))
      {
         pClipBuffer = new BYTE[lSize];
         hr = m_pClipDet->GetBitmapBits(
            (nMediaMs - nStartMs) / 1000.0, &lSize, (char *)pClipBuffer, lClipWidth, lClipHeight);
      }
   }
   
   _ASSERT(SUCCEEDED(hr));

   Gdiplus::Bitmap *pBitmapClip = NULL;
   if (SUCCEEDED(hr))
   {
      BITMAPINFO oldBitmapDefine;
      ZeroMemory(&oldBitmapDefine, sizeof BITMAPINFO);
      
      VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)(mt.pbFormat);
      oldBitmapDefine.bmiHeader = pVih->bmiHeader;
      
      pBitmapClip = Gdiplus::Bitmap::FromBITMAPINFO(
         &oldBitmapDefine, pClipBuffer + sizeof BITMAPINFOHEADER);
      
      /*
      if (pBitmapClip != NULL)
      {
         CLSID pngClsid;
         GetEncoderClsid(L"image/png", &pngClsid);
         WCHAR wszFilename[40];
         swprintf(wszFilename, L"d:\\first%d.png", nMediaMs);
         pBitmapClip->Save(wszFilename, &pngClsid, NULL);
      }
      */
   }
   
   // TODO why can I not close or delete anything here??

   //if (pClipBuffer != NULL)
   //   delete[] pClipBuffer;
   
   // who deletes this??
   //if (mt.cbFormat != 0) // initialized and contains extra format data
   //   delete mt.pbFormat;

   return pBitmapClip; // can be NULL
}

HRESULT CLecturnityVideoStream::GetAviClipFrame(UINT nMediaMs, bool bGetSomeImage, Gdiplus::Bitmap **ppTargetBitmap)
{
   Gdiplus::Bitmap *pBitmapClip = NULL;

   HRESULT hr = S_OK;

   if (m_aAllClips.GetSize() == 0)
   {
      if (!bGetSomeImage)
         return S_FALSE;
      else
         return E_FAIL;
   }

   if (ppTargetBitmap == NULL)
      return E_POINTER;

   CClipInfo *pCurrentClip = NULL;
   for (int i=0; i<m_aAllClips.GetSize(); ++i)
   {
      if (m_aAllClips[i]->ContainsMediaTime(nMediaMs))
      {
         pCurrentClip = m_aAllClips[i];
         break;
      }
   }

   if (bGetSomeImage && pCurrentClip == NULL)
   {
      // there is some clip; checked above
      pCurrentClip = m_aAllClips[m_aAllClips.GetSize()-1]; // video will/should be last
   }


   // closes open PGETFRAME even if pCurrentClip == NULL
   if (m_pLastCurrentClip != NULL && pCurrentClip != m_pLastCurrentClip)
   {
      m_pLastCurrentClip->Close();
      m_pLastCurrentClip = NULL;
   }


   if (SUCCEEDED(hr) && pCurrentClip != NULL)
   {
      BITMAPINFOHEADER *pExtractedBitmap = NULL;

      PGETFRAME pClipGetFrame = pCurrentClip->Open();
      
      if (pClipGetFrame != NULL)
      {
         LONG lSamplePos = pCurrentClip->GetFrameNumber(nMediaMs);

         pExtractedBitmap = (BITMAPINFOHEADER *)AVIStreamGetFrame(pClipGetFrame, lSamplePos);

         if (pExtractedBitmap == NULL)
            printf("No get for sample %d\n", lSamplePos);
      }
      else
      {
         hr = E_FAIL;
         _RPT0(_CRT_ERROR, "No PGETFRAME!\n");
      }
      
      
      if (SUCCEEDED(hr))
      {
         if (pExtractedBitmap == NULL)
         {
            hr = E_FAIL;
            _RPT0(_CRT_ERROR, "No extraction (AVIStreamGetFrame)!\n");
         }
         else
         {
            BITMAPINFO oldBitmapDefine;
            ZeroMemory(&oldBitmapDefine, sizeof BITMAPINFO);

            oldBitmapDefine.bmiHeader = *pExtractedBitmap; // makes a copy

            BYTE *pData = ((BYTE *)pExtractedBitmap) + sizeof BITMAPINFOHEADER;
            pBitmapClip = Gdiplus::Bitmap::FromBITMAPINFO(&oldBitmapDefine, pData);

            /*
            if (pBitmapClip != NULL)
            {
            CLSID pngClsid;
            GetEncoderClsid(L"image/png", &pngClsid);
            WCHAR wszFilename[40];
            swprintf(wszFilename, L"d:\\first%d.png", nMediaMs);
            pBitmapClip->Save(wszFilename, &pngClsid, NULL);
            }
            */
         }
      }

      m_pLastCurrentClip = pCurrentClip;
   }

   if (SUCCEEDED(hr))
      *ppTargetBitmap = pBitmapClip; // can be NULL (if bGetSomeImage == false)

   return hr; 
}

HRESULT CLecturnityVideoStream::FillBuffer(IMediaSample *pSample)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   CheckPointer(pSample,E_POINTER);

   if (m_pBitmap == NULL)
       return E_FAIL;

   // Note m_pWhiteboard can be NULL if it is a Denver document or should be video only.
  
   BYTE *pData;
   pSample->GetPointer(&pData);
   
   // The current time is the sample's start
   CRefTime rtStart = m_rtSampleTime;
   
   // end time of this frame and start time of the next:
   m_rtSampleTime += (LONG)m_iRepeatTime;
   
   pSample->SetTime((REFERENCE_TIME *) &rtStart,(REFERENCE_TIME *) &m_rtSampleTime);

   //
   // Now try obtaining the current image.
   // This can be either the whiteboard image OR
   // the image of a clip if one is defined and active.
   //


   HRESULT hr = S_OK;
   bool bFirstAfterClip = false;

   bool bForceVideoImage = m_pWhiteboard == NULL;

   Gdiplus::Bitmap *pBitmapClip = NULL;
   hr = GetAviClipFrame(rtStart.Millisecs(), bForceVideoImage, &pBitmapClip);

   if (FAILED(hr))
      hr = E_LS_VIDEO_FAILED;

   if (SUCCEEDED(hr) && m_pWhiteboard == NULL && pBitmapClip == NULL)
   {
      // This should not happen: if there is a video "bForceVideoImage" should ensure
      // that an image is returned.

      pBitmapClip = new Gdiplus::Bitmap(m_nOutputWidth, m_nOutputHeight, PixelFormat24bppRGB);
   }

   if (pBitmapClip != NULL)
   {
      m_bClipWasActive = true;
   }
   else if (m_bClipWasActive)
   {
      m_bClipWasActive = false;
      bFirstAfterClip = true;
   }
   
   // Note we are using a Gdiplus::Bitmap here instead of a CBitmap:
   // - It can have an arbitrary color format and you can create a CDC from it.
   //   A CDC with selecting a CBitmap has to be created with display color depth.
   // - You can easily flip the image before writing.
   // However this takes considerably more time:
   // - LockBits: 200 microseconds
   // - Flipping: 1600 microseconds
   // This makes the time needed here tenfold than that for CBitmap.
   // That is 1800 microseconds instead of 200 for a 320x240 image.

   if (SUCCEEDED(hr)) 
   {
      Gdiplus::Graphics graphics(m_pBitmap);

      if (pBitmapClip == NULL)
      {
         // normal case: use whiteboard
         HDC hDC = graphics.GetHDC();
         CDC *pDC = CDC::FromHandle(hDC);
         
         if (pDC != NULL)
         {
            // everything alright
            
            DrawSdk::ZoomManager *zoom = new DrawSdk::ZoomManager(m_dZoomFactor);
            bool bIncludeInteractives = true;
            m_pWhiteboard->Draw(
               pDC, rtStart.Millisecs(), m_rcWbArea, zoom, bFirstAfterClip, bIncludeInteractives);
            // it is much faster not doing a full repaint every time (bFirstAfterClip)
            delete zoom;

            // doesn't seem to be allowed: results in a still image video
            //pDC->DeleteDC();
         }
         else
            _ASSERT(false);
         
         graphics.ReleaseHDC(hDC);
      }
      else
      {
         // use the image of the clip

         Gdiplus::SolidBrush brushBlack(Gdiplus::Color(0x00, 0x00, 0x00));
         graphics.FillRectangle(&brushBlack, 0, 0, m_nOutputWidth, m_nOutputHeight);

         CRect rcDrawArea;
         rcDrawArea.top = 0;
         rcDrawArea.left = 0;
         rcDrawArea.right = pBitmapClip->GetWidth();
         rcDrawArea.bottom = pBitmapClip->GetHeight();
            
         double dScale = 1.0;
         this->CalculateDrawingRegion(m_nOutputWidth, m_nOutputHeight, 
            pBitmapClip->GetWidth(), pBitmapClip->GetHeight(), &rcDrawArea, &dScale);
         // TODO automatically does upscaling?

         Gdiplus::Rect gdiRect(rcDrawArea.left, rcDrawArea.top, rcDrawArea.Width(), rcDrawArea.Height());

         // Of the four interpolation modes this seems to be the best for
         // screen content and it doesn't seem to be the slowest.
         // But it is still three times slower (10ms vs 3ms) as the standard
         // interpolation mode.
         graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
         graphics.DrawImage(pBitmapClip, gdiRect);

         //Gdiplus::Pen penGreen(Gdiplus::Color(0x7e, 0xee, 0x9f), 3);
         //graphics.DrawRectangle(&penGreen, 1, 1, m_nOutputWidth-2, m_nOutputHeight-2);
                  
         delete pBitmapClip;
      }
      
      Gdiplus::Status stat = Gdiplus::Ok;


      if (m_bIsEvaluation)
      {
         // draws an evaluation banner across each image

         WCHAR *wszEvalMessage = L"LECTURNITY Evaluation";

         float fDiagonalLength = (float)sqrt((float)(m_nOutputWidth*m_nOutputWidth + m_nOutputHeight*m_nOutputHeight));

         float fFontSize = 16.0f;
         if (m_nOutputWidth * 3 / 4 >= m_nOutputHeight)
            fFontSize *= (m_nOutputHeight / 240.0f);
         else
            fFontSize *= (m_nOutputWidth / 320.0f);
         
         Gdiplus::Font myFont(L"Arial", fFontSize);
         Gdiplus::PointF ptOrigin(0.0f, 0.0f);

         Gdiplus::RectF rcText;
         rcText.Width = 0;
         graphics.MeasureString(wszEvalMessage, wcslen(wszEvalMessage), &myFont, ptOrigin, &rcText);

         if (rcText.Width > 0)
         {
            fFontSize *= (fDiagonalLength * 0.9f) / rcText.Width;
            Gdiplus::Font myFontCorrected(L"Arial", fFontSize);
            graphics.MeasureString(wszEvalMessage, wcslen(wszEvalMessage), &myFontCorrected, ptOrigin, &rcText);

            ptOrigin.X = m_nOutputWidth / 2.0f;
            ptOrigin.Y = m_nOutputHeight / 2.0f;

            float fAngle = (float)(acos(m_nOutputWidth / fDiagonalLength) / 3.14f) * 180;

            Gdiplus::Matrix RotateMatrix;
            RotateMatrix.RotateAt(-fAngle, ptOrigin);

            graphics.SetTransform(&RotateMatrix);


            ptOrigin.X = (m_nOutputWidth - rcText.Width) / 2;
            ptOrigin.Y = (m_nOutputHeight - rcText.Height) / 2;

            Gdiplus::SolidBrush brshRed(Gdiplus::Color(96, 255, 0, 0));
            
            graphics.DrawString(wszEvalMessage, wcslen(wszEvalMessage), &myFontCorrected, ptOrigin, &brshRed);
            
            graphics.ResetTransform();
         }
      }
      
      // video formats store images flipped
      stat = m_pBitmap->RotateFlip(Gdiplus::RotateNoneFlipY);
      
      if (stat == Gdiplus::Ok)
      {
         // Note: Working with LockBits() is much faster than working with 
         // GetHBITMAP(): 200 vs. 1400 microseconds.
         // the latter one is also a memory leak.
         
         Gdiplus::BitmapData bitmapData;
         Gdiplus::Rect rect(0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());
         
         stat = m_pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);

         if (stat == Gdiplus::Ok)
         {
            if (bitmapData.Stride < 0)
               _ASSERT(false);
            
            // TODO what about "4" here?
            // TODO no padding needed?: output format is mpeg size?
            // If it is needed you can use "Stride".
            memcpy(pData, bitmapData.Scan0, m_nOutputWidth * m_nOutputHeight * 4);
         }

         m_pBitmap->UnlockBits(&bitmapData);
      }
      
      // flip it back for further painting here
      stat = m_pBitmap->RotateFlip(Gdiplus::RotateNoneFlipY);
   }
  
   if (SUCCEEDED(hr))
      pSample->SetSyncPoint(TRUE);
   
   UINT nDocLengthMs = 0;
   if (m_pWhiteboard != NULL)
      nDocLengthMs = m_pWhiteboard->GetLength();
   else if (m_aAllClips.GetSize() > 0)
      nDocLengthMs = m_aAllClips[m_aAllClips.GetSize()-1]->GetEndTimeMs(); // probably Denver or video only document
   // else should not happen

   int iStopTime = nDocLengthMs;
   if (m_nMinimumLengthMs > 0 && m_nMinimumLengthMs > nDocLengthMs)
      iStopTime = m_nMinimumLengthMs;


   if (FAILED(hr))
      m_pFilter->NotifyEvent(EC_ERRORABORT, (LONG_PTR)hr, 0);
   else {
       if (m_rtSampleTime.Millisecs() - m_iRepeatTime > iStopTime) {
           hr = S_FALSE;
           
           // Proper signalling of stop?
           // A "pConnectPin->EndOfStream()" is probably not the right one;
           // it crashes for example the Mainconcept Muxer.

           // Note: It signals "end" only for the first sample _after_ the length
           //       (which was already the case earlier as EndOfStream() was used).
       } else
           hr = S_OK;
   }

   return hr;
}

HRESULT CLecturnityVideoStream::DecideBufferSize(IMemAllocator *pMemAllocator, 
                                                 ALLOCATOR_PROPERTIES *pAllocProperties)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

   HRESULT hr = S_OK;
   
   CheckPointer(pMemAllocator,E_POINTER);
   CheckPointer(pAllocProperties,E_POINTER);
  
   VIDEOINFO *pvi = (VIDEOINFO *) m_mt.Format();
   pAllocProperties->cBuffers = 1;
   pAllocProperties->cbBuffer = pvi->bmiHeader.biSizeImage;
   
   // Ask the allocator to reserve us some sample memory, NOTE the function
   // can succeed (that is return NOERROR) but still not have allocated the
   // memory that we requested, so we must check we got whatever we wanted.
   
   ALLOCATOR_PROPERTIES propActual;
   hr = pMemAllocator->SetProperties(pAllocProperties, &propActual);
   
   // Is this allocator unsuitable?
   if (SUCCEEDED(hr))
   {
      if(propActual.cbBuffer < pAllocProperties->cbBuffer)
         hr = E_FAIL;
   }
   
   // Make sure that we have only 1 buffer (we erase the data in the
   // old buffer to save having to zero a 200k+ buffer every time
   // we draw a frame)
   
   ASSERT(propActual.cBuffers == 1);

   
   return hr;
}

STDMETHODIMP CLecturnityVideoStream::Notify(IBaseFilter * pSender, Quality q)
{
   // must be implemented for GraphEdit
   return S_OK;

} // Notify

/*
LARGE_INTEGER liFreq;
::QueryPerformanceFrequency(&liFreq);

  LARGE_INTEGER liOne;
  ::QueryPerformanceCounter(&liOne);
  
    
      LARGE_INTEGER liTwo;
      ::QueryPerformanceCounter(&liTwo);
      
        _RPT1(_CRT_WARN, "%f ", ((liTwo.QuadPart - liOne.QuadPart)/(double)liFreq.QuadPart));
        
*/
