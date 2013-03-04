#include "StdAfx.h"
#include "avinfo.h"
#include "vfw.h"
#include <vector>
#include "wavdest.h"
#include "lutils.h" // LIo::GetDiskSpace

void PromptForGraphInspection(IGraphBuilder *pGraph, bool addRemove);

#define SAFE_RELEASE(p) \
   if (p) \
      p->Release(); \
   p = NULL;

/*static*/ void AVInfo::StaticMakeUncompressed(AM_MEDIA_TYPE *type)
{
   _ASSERT(type->majortype == MEDIATYPE_Audio || type->majortype == MEDIATYPE_Video);

   type->bFixedSizeSamples = true;
   type->bTemporalCompression = false;

   if (type->majortype == MEDIATYPE_Audio)
   {
      WAVEFORMATEX *pWavHeader = (WAVEFORMATEX *)type->pbFormat;
      _ASSERT(pWavHeader);

      bool isPcm = pWavHeader->wFormatTag == WAVE_FORMAT_PCM;
      
      if (!isPcm)
      {
         pWavHeader->wFormatTag = WAVE_FORMAT_PCM;
         
         int uncompressedBitCount = pWavHeader->wBitsPerSample == 8 ? 8 : 16;

        
         type->subtype = MEDIASUBTYPE_PCM;
         
         pWavHeader->wBitsPerSample = uncompressedBitCount;
         pWavHeader->nBlockAlign = 
            (pWavHeader->wBitsPerSample*pWavHeader->nChannels)/8;
         
         type->lSampleSize = 
            (pWavHeader->nSamplesPerSec*pWavHeader->nBlockAlign);

         pWavHeader->nAvgBytesPerSec = type->lSampleSize;
         
      }
   }
   else if (type->majortype == MEDIATYPE_Video)
   {
      VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER *)type->pbFormat;
      _ASSERT(pVideoHeader);

      // TODO: Das sind nicht alle unkomprimierten Fälle:
      if (type->subtype != MEDIASUBTYPE_RGB24)
      {
         type->subtype = MEDIASUBTYPE_RGB24;
         
         pVideoHeader->bmiHeader.biCompression = BI_RGB;
         pVideoHeader->bmiHeader.biBitCount = 24;
         pVideoHeader->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
         pVideoHeader->bmiHeader.biSizeImage = DIBSIZE(pVideoHeader->bmiHeader);
         
         type->lSampleSize = DIBSIZE(pVideoHeader->bmiHeader);
      }
   }
}

/*static*/ bool AVInfo::StaticIsAudio(AM_MEDIA_TYPE *type)
{
   _ASSERT(type);

   if (type == NULL)
      return false;

   return (type->majortype == MEDIATYPE_Audio) ? true : false;
}

/*static*/ bool AVInfo::StaticIsPcmAudio(AM_MEDIA_TYPE *type)
{
   if (type == NULL)
      return false;

   if (StaticIsAudio(type))
   {
      if ((type->subtype == MEDIASUBTYPE_PCM) &&
          (type->formattype == FORMAT_WaveFormatEx))
      {
         WAVEFORMATEX *pWfe = (WAVEFORMATEX *) type->pbFormat;
         return (pWfe->wFormatTag == WAVE_FORMAT_PCM);
      }
      else
      {
         return false;
      }
   }

   return false;
}

/*static*/ bool AVInfo::StaticIsFilePcmAudio(const _TCHAR *tszAudioFile)
{
   if (NULL == tszAudioFile)
      return false;
   LFile file(tszAudioFile);
   if (!file.Exists())
      return false;

   bool success = false;
   AVInfo avInfo;
   HRESULT hr = ::CoInitialize(NULL);
   if (SUCCEEDED(hr))
      hr = avInfo.Initialize(tszAudioFile);
   if (SUCCEEDED(hr))
   {
      AM_MEDIA_TYPE mt;
      ZeroMemory(&mt, sizeof AM_MEDIA_TYPE);
      hr = avInfo.GetStreamFormat(0, &mt);
      if (SUCCEEDED(hr))
      {
         if (true == StaticIsPcmAudio(&mt))
         {
            success = true;
         }
      }
   }

   ::CoUninitialize();

   return success;
}

/*static*/ bool AVInfo::StaticIsCompressed(AM_MEDIA_TYPE *type)
{
   _ASSERT(type);
   if (type == NULL)
      return false;

   if (type->majortype == MEDIATYPE_Audio)
   {
      WAVEFORMATEX *pWavHeader = (WAVEFORMATEX *)type->pbFormat;
      _ASSERT(pWavHeader);

      return pWavHeader->wFormatTag != WAVE_FORMAT_PCM;
   }
   else if (type->majortype == MEDIATYPE_Video)
   {
      VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER *)type->pbFormat;
      _ASSERT(pVideoHeader);

      // TODO: Is that sufficient? Likely not...
      return (type->bTemporalCompression) ? true : false;
   }

   return false;
}

HRESULT AVInfo::StaticCheckRenderable(const _TCHAR *tszVideoFileName)
{
   HRESULT hr = ::CoInitialize(NULL);

   if (SUCCEEDED(hr))
   {
      CComPtr<IGraphBuilder> pGraph;
      hr = ::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_ALL, IID_IGraphBuilder, (void **) &pGraph);
      WCHAR wszFileName[1024];
#ifdef _UNICODE
      wcscpy(wszFileName, tszVideoFileName);
#else
      ::MultiByteToWideChar(CP_ACP, 0, tszVideoFileName, -1, wszFileName, 1024);
#endif
      if (SUCCEEDED(hr))
         hr = pGraph->RenderFile(wszFileName, NULL);
      if (SUCCEEDED(hr))
      {
         CComQIPtr<IVideoWindow> pVideoWindow(pGraph);
         if (pVideoWindow)
         {
            pVideoWindow->put_Visible(OAFALSE);
            pVideoWindow->put_AutoShow(OAFALSE);
         }
         else
            hr = E_FAIL;
      }

      if (SUCCEEDED(hr))
      {
         CComQIPtr<IBasicAudio> pBasicAudio(pGraph);
         if (pBasicAudio)
         {
            pBasicAudio->put_Volume(-10000); // silence!
         }
      }

      CComPtr<IMediaControl> pControl;
      if (SUCCEEDED(hr))
         hr = pGraph->QueryInterface(IID_IMediaControl, (void **) &pControl);
      if (SUCCEEDED(hr))
      {
         if (pControl)
            hr = pControl->Run();
         else
            hr = E_FAIL;
      }

      if (SUCCEEDED(hr))
      {
         ::Sleep(500);
         hr = pControl->Stop();
      }
   }

   ::CoUninitialize();

   return hr;
}

AVInfo::AVInfo()
{
   m_tszSourceFile = NULL;
   m_pMediaDet = NULL;

   m_iVideoIndex = -2; // undetermined
   m_iAudioIndex = -2;

   m_codecListSize = 0;
   m_plCodecList = NULL;
}

AVInfo::~AVInfo()
{
   if (m_tszSourceFile)
      delete[] m_tszSourceFile;
   m_tszSourceFile = NULL;

   if (m_pMediaDet)
      m_pMediaDet->Release();
   m_pMediaDet = NULL;

   if (m_plCodecList)
   {
      for (int i=0; i<m_codecListSize; ++i)
         delete m_plCodecList[i];
      delete[] m_plCodecList;
      m_plCodecList = NULL;
   }
   m_codecListSize = 0;
}

HRESULT AVInfo::Initialize(const _TCHAR *tszSourceFile)
{
   _ASSERT(m_tszSourceFile == NULL);
   _ASSERT(tszSourceFile);
   _ASSERT(_taccess(tszSourceFile, 04) == 0); // file exists and is readable


   m_tszSourceFile = new _TCHAR[_tcslen(tszSourceFile)+1];
   _tcscpy(m_tszSourceFile, tszSourceFile);
   
   IMediaDet *pMediaDet = NULL;
   HRESULT hr = ::CoCreateInstance(
      CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER,
      IID_IMediaDet, (void **) &pMediaDet);
   
   if (SUCCEEDED(hr))
   {
      hr = pMediaDet->put_Filename(_bstr_t(m_tszSourceFile));
   }
   
   if (SUCCEEDED(hr))
   {
      m_pMediaDet = pMediaDet;
   }
   else if (pMediaDet)
   {
      pMediaDet->Release();
   }

   
   return hr;

}


HRESULT AVInfo::GetLength(double *length) const
{
   _ASSERT(m_pMediaDet);

   HRESULT hr = S_OK;
   
   if (SUCCEEDED(hr))
      hr = m_pMediaDet->get_StreamLength(length);

   return hr;
}

HRESULT AVInfo::GetStreamCount(long *count) const
{
   _ASSERT(m_pMediaDet);

   HRESULT hr = S_OK;

   if (SUCCEEDED(hr))
      hr = m_pMediaDet->get_OutputStreams(count);
   
   return hr;
}

HRESULT AVInfo::GetStreamFormat(const int id, AM_MEDIA_TYPE *type) const
{
   _ASSERT(m_pMediaDet);

   HRESULT hr = S_OK;
   
   if (SUCCEEDED(hr))
      hr = m_pMediaDet->put_CurrentStream(id);
   
   if (SUCCEEDED(hr))
   {
      hr = m_pMediaDet->get_StreamMediaType(type);
   }

   return hr;
}

// private
HRESULT AVInfo::DetermineStreamIndices()
{
   _ASSERT(m_pMediaDet);

   HRESULT hr = S_OK;

   long iStreamCount = 0;
   if (SUCCEEDED(hr))
      hr = GetStreamCount(&iStreamCount);
   
   int iStreamId = 0;
   while (SUCCEEDED(hr) && iStreamId < iStreamCount)
   {
      AM_MEDIA_TYPE mt;
      hr = GetStreamFormat(iStreamId, &mt);
      
      if (SUCCEEDED(hr))
      {
         if (mt.majortype == MEDIATYPE_Video)
            m_iVideoIndex = iStreamId;
         else if (mt.majortype == MEDIATYPE_Audio)
            m_iAudioIndex = iStreamId;
         
         FreeMediaType(mt);
      }
      
      ++iStreamId;
   }

   if (SUCCEEDED(hr))
   {
      if (m_iVideoIndex == -2)
         m_iVideoIndex = -1; // video not found
      if (m_iAudioIndex == -2)
         m_iAudioIndex = -1; // audio not found
   }

   return hr;
}

HRESULT AVInfo::GetVideoStreamIndex(int *pIndex)
{
   HRESULT hr = S_OK;

   if (m_iVideoIndex < -1)
      hr = DetermineStreamIndices();
   
   if (SUCCEEDED(hr))
      *pIndex = m_iVideoIndex;

   return hr;
}

HRESULT AVInfo::GetAudioStreamIndex(int *pIndex)
{
   HRESULT hr = S_OK;

   if (m_iAudioIndex < -1)
      hr = DetermineStreamIndices();
   
   if (SUCCEEDED(hr))
      *pIndex = m_iAudioIndex;

   return hr;
}


int AVInfo::GetCodecListSize(int width, int height)
{
   if (m_plCodecList)
   {
      for (int i=0; i<m_codecListSize; ++i)
         delete m_plCodecList[i];
      delete[] m_plCodecList;
      m_plCodecList = NULL;
      m_codecListSize = 0;
   }

   BITMAPINFO bmi;
   Make24BitRGB(&bmi.bmiHeader, width, height);

   ICINFO icInfo;
   ZeroMemory(&icInfo, sizeof ICINFO);
   icInfo.dwSize = sizeof ICINFO;
   int count = 0;

   std::vector<CODECINFO *> codecList;

   // The first codec is always uncompressed RGB
   // data. We can be sure this is the case here
   CODECINFO *pInfo = new CODECINFO;
   pInfo->fcc = MAKEFOURCC('D', 'I', 'B', ' ');
   pInfo->alternativeFcc = BI_RGB;
   pInfo->bHasAboutDialog = false;
   pInfo->bHasConfigureDialog = false;
   pInfo->bSupportsKeyframes = false;
   pInfo->bSupportsQuality = false;
   pInfo->nDefaultKeyframeRate = 1;
   pInfo->nDefaultQuality = 100;
   _tcscpy(pInfo->tszDescription, _T("DIB : <uncompressed>"));
   codecList.push_back(pInfo);

   while (ICInfo(ICTYPE_VIDEO, count, &icInfo))
   {
      HIC hic = ICOpen(icInfo.fccType, icInfo.fccHandler, ICMODE_QUERY);
      if (hic)
      {
         ICINFO moreInfo;
         ZeroMemory(&moreInfo, sizeof ICINFO);
         moreInfo.dwSize = sizeof ICINFO;

         if (ICGetInfo(hic, &moreInfo, sizeof ICINFO) > 0)
         {
            // Ignore MSVC/CRAM
            if (moreInfo.fccHandler != 'CVSM' &&
                moreInfo.fccHandler != 'MARC')
            {
               if (ICERR_OK == ICCompressQuery(hic, &bmi, NULL))
               {
                  // Ok, this codec seems to be ok; put that
                  // into the list
                  CODECINFO *pInfo = new CODECINFO;
                  ZeroMemory(pInfo, sizeof CODECINFO);

                  char szFcc[5];
                  if (moreInfo.fccHandler != BI_RGB)
                  {
                     szFcc[0] = (moreInfo.fccHandler & 0x000000ff);
                     szFcc[1] = (moreInfo.fccHandler & 0x0000ff00) >> 8;
                     szFcc[2] = (moreInfo.fccHandler & 0x00ff0000) >> 16;
                     szFcc[3] = (moreInfo.fccHandler & 0xff000000) >> 24;
                     szFcc[4] = 0;
                  }
                  else
                     strcpy(szFcc, "DIB ");

                  bool bIsReallyOk = true;

                  // We know that IV50 has restrictions.
                  if (moreInfo.fccHandler == '05vi' ||
                      moreInfo.fccHandler == '05VI')
                  {
                     if (width % 4 != 0 ||
                         height % 4 != 0)
                        bIsReallyOk = false;
                  }

                  // The following code should normally check if the codec
                  // is really able encode a certain BITMAPINFO type. But,
                  // This will fail for the HuffYUV codec. Thus, we have to
                  // assume that it might fail for other codecs, too.
                  /*
                  if (moreInfo.fccHandler == BI_RGB)
                  {
                     bIsReallyOk = true;
                  }
                  else
                  {
                     // The codec says that the input format is ok.
                     // Let's try to create an output format and try
                     // again. For some resolutions, e.g. Indeo Video 5
                     // now fails to compress the format.
                     AM_MEDIA_TYPE mtTmp;
                     ZeroMemory(&mtTmp, sizeof AM_MEDIA_TYPE);
                     bool cool = CreateStreamFormat(moreInfo.fccHandler, width, height, &mtTmp);

                     if (cool)
                     {
                        BITMAPINFO bmiOut;
                        ZeroMemory(&bmiOut, sizeof BITMAPINFO);
                        bmiOut.bmiHeader = ((VIDEOINFOHEADER *) mtTmp.pbFormat)->bmiHeader;
                        DWORD dwOk = ICCompressBegin(hic, &bmi, &bmiOut);
                        if (ICERR_OK == dwOk)
                        {
                           // It's really ok to use this codec.
                           bIsReallyOk = true;
                           ICCompressEnd(hic);
                        }
                     }

                     FreeMediaType(mtTmp);
                  }
                  */

                  if (bIsReallyOk)
                  {
                     // Copy the name
#ifndef _UNICODE
                     _TCHAR tszDesc[128];
                     if (WideCharToMultiByte(CP_ACP, 0, moreInfo.szDescription, -1, tszDesc, 128, NULL, NULL) == 0)
                        strcpy(tszDesc, "<unknown>");
                     sprintf(pInfo->tszDescription, "(%s): %s", szFcc, tszDesc);
#else
                     _stprintf(pInfo->tszDescription, _T("(%S): %s"), szFcc, moreInfo.szDescription);
#endif

                     pInfo->fcc = moreInfo.fccHandler;
                     pInfo->alternativeFcc = icInfo.fccHandler;
                     pInfo->bSupportsKeyframes = (moreInfo.dwFlags & VIDCF_TEMPORAL) > 0;
                     pInfo->bSupportsQuality = (moreInfo.dwFlags & VIDCF_QUALITY) > 0;
                     if (pInfo->bSupportsKeyframes)
                        pInfo->nDefaultKeyframeRate = ICGetDefaultKeyFrameRate(hic);
                     else
                        pInfo->nDefaultKeyframeRate = 0;
                     if (pInfo->bSupportsQuality)
                        pInfo->nDefaultQuality = ICGetDefaultQuality(hic) / 100;
                     else
                        pInfo->nDefaultQuality = 0;

                     pInfo->bHasConfigureDialog = ICQueryConfigure(hic) == TRUE ? true : false;
                     pInfo->bHasAboutDialog = ICQueryAbout(hic) == TRUE ? true : false;

                     codecList.push_back(pInfo);
                  }
               }
            }
         }

         ICClose(hic);
      }

      count++;
   }

   m_codecListSize = codecList.size();
   m_plCodecList = new CODECINFO*[m_codecListSize];
   for (int i=0; i<m_codecListSize; ++i)
      m_plCodecList[i] = codecList[i];
   codecList.clear();

   return m_codecListSize;
}

void AVInfo::GetCodecList(CODECINFO **plCI)
{
   if (!plCI)
      return;
   for (int i=0; i<m_codecListSize; ++i)
      *plCI[i] = *m_plCodecList[i];
}

bool AVInfo::HasVideoVfwCodec(const _TCHAR *tszVideoFileName)
{
   HRESULT hr = ::CoInitialize(NULL);
   if (SUCCEEDED(hr))
      hr = Initialize(tszVideoFileName);
   AM_MEDIA_TYPE mt;
   ZeroMemory(&mt, sizeof AM_MEDIA_TYPE);
   int nStream = 0;
   bool bFoundVideo = false;
   while (!bFoundVideo && SUCCEEDED(hr))
   {
      if (SUCCEEDED(hr))
         hr = GetStreamFormat(nStream, &mt);
      if (SUCCEEDED(hr))
      {
         if (mt.majortype == MEDIATYPE_Video &&
             mt.formattype == FORMAT_VideoInfo)
         {
            bFoundVideo = true;
         }
         else
         {
            nStream++;
            FreeMediaType(mt);
         }
      }
   }

   DWORD dwFcc = 0;
   int nWidth = 320;
   int nHeight = 240;
   if (SUCCEEDED(hr))
   {
      VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *) mt.pbFormat;
      dwFcc = pVih->bmiHeader.biCompression;
      nWidth = pVih->bmiHeader.biWidth;
      nHeight = pVih->bmiHeader.biHeight;
      FreeMediaType(mt);
   }

   if (SUCCEEDED(hr))
   {
      int nSize = GetCodecListSize(nWidth, nHeight);
      bool bFoundCodec = false;
      for (int i=0; i<m_codecListSize; ++i)
      {
         if (m_plCodecList[i]->fcc == dwFcc ||
             m_plCodecList[i]->alternativeFcc == dwFcc)
            bFoundCodec = true;
      }

      if (!bFoundCodec)
         hr = E_FAIL;
   }

   ::CoUninitialize();

   return (SUCCEEDED(hr));
}

bool AVInfo::CreateStreamFormat(DWORD fcc, int cx, int cy, AM_MEDIA_TYPE *pMt)
{
   if (!pMt)
      return false;

   bool success = false;

   if (fcc == MAKEFOURCC('D', 'I', 'B', ' ') ||
       fcc == BI_RGB)
   {
      // Uncompressed format
      VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER *) CoTaskMemAlloc(sizeof VIDEOINFOHEADER);
      if (pVIH)
      {
         FreeMediaType(*pMt);
         ZeroMemory(pMt, sizeof AM_MEDIA_TYPE);
         ZeroMemory(pVIH, sizeof VIDEOINFOHEADER);
         Make24BitRGB(&pVIH->bmiHeader, cx, cy);
         pMt->majortype = MEDIATYPE_Video;
         pMt->subtype = MEDIASUBTYPE_RGB24;
         pMt->bFixedSizeSamples = TRUE;
         pMt->bTemporalCompression = FALSE;
         pMt->lSampleSize = pVIH->bmiHeader.biSizeImage;
         pMt->formattype = FORMAT_VideoInfo;
         pMt->cbFormat = sizeof VIDEOINFOHEADER;
         pMt->pbFormat = (BYTE *) pVIH;

         success = true;
      }

      return success;
   }

   // Bugfix #4593: ICOpen does not find the codecs with the "wrong" fcc delivered by the system.
   if (fcc == MAKEFOURCC('C', 'R', 'A', 'M') || fcc == MAKEFOURCC('c', 'r', 'a', 'm'))
      fcc = MAKEFOURCC('M', 'S', 'V', 'C');

   if (fcc == MAKEFOURCC('D', 'X', '5', '0') || fcc == MAKEFOURCC('d', 'x', '5', '0'))
      fcc = MAKEFOURCC('D', 'I', 'V', 'X');
   

   HIC hic = ICOpen(ICTYPE_VIDEO, fcc, ICMODE_QUERY);
   if (hic)
   {
      ICINFO icInfo;
      ZeroMemory(&icInfo, sizeof ICINFO);
      icInfo.dwSize = sizeof ICINFO;
      if (0 != ICGetInfo(hic, &icInfo, sizeof ICINFO))
      {
         BITMAPINFO bi;
         Make24BitRGB(&bi.bmiHeader, cx, cy);

         if (ICERR_OK == ICCompressQuery(hic, &bi, NULL))
         {
            DWORD formatSize = ICCompressGetFormatSize(hic, &bi);
            if (formatSize < 100000)
            {
               BITMAPINFOHEADER *pOut = (BITMAPINFOHEADER *) new char[formatSize];
               ZeroMemory(pOut, formatSize);
               if (ICERR_OK == ICCompressGetFormat(hic, &bi, pOut))
               {
                  int videoInfoHeaderSize = 
                     sizeof VIDEOINFOHEADER - sizeof BITMAPINFOHEADER + formatSize;

                  VIDEOINFOHEADER *pFormat = (VIDEOINFOHEADER *) 
                     CoTaskMemAlloc(videoInfoHeaderSize);
                  if (pFormat)
                  {
                     BOOL hasTemporalCompression = 
                        (icInfo.dwFlags & VIDCF_TEMPORAL) > 0 ||
                        (icInfo.dwFlags & VIDCF_FASTTEMPORALC) > 0;

                     ZeroMemory(pFormat, videoInfoHeaderSize);
                     // Note that the bitmapinfoheader structure
                     // of the given output format does not have
                     // to have sizeof BITMAPINFOHEADER!
                     memcpy(&pFormat->bmiHeader, pOut, formatSize);

                     FreeMediaType(*pMt);
                     ZeroMemory(pMt, sizeof AM_MEDIA_TYPE);
                     FOURCCMap fcc(pOut->biCompression);

                     pMt->bFixedSizeSamples = hasTemporalCompression ? FALSE : TRUE;
                     pMt->bTemporalCompression = hasTemporalCompression;
                     pMt->lSampleSize = 0;
                     pMt->formattype = FORMAT_VideoInfo;
                     pMt->pbFormat = (BYTE *) pFormat;
                     pMt->cbFormat = videoInfoHeaderSize;
                     pMt->majortype = MEDIATYPE_Video;
                     pMt->subtype = fcc;

                     success = true;
                  }
               }

               delete[] ((char *) pOut);
            }
         }
      }

      ICClose(hic);
   }

   return success;
}

void AVInfo::ConfigureCodec(DWORD fcc, HWND hWndParent)
{
   HIC hic = ICOpen(ICTYPE_VIDEO, fcc, ICMODE_QUERY);
   if (hic)
   {
      ICConfigure(hic, hWndParent);
      ICClose(hic);
   }
}

void AVInfo::AboutCodec(DWORD fcc, HWND hWndParent)
{
   HIC hic = ICOpen(ICTYPE_VIDEO, fcc, ICMODE_QUERY);
   if (hic)
   {
      ICAbout(hic, hWndParent);
      ICClose(hic);
   }
}

void AVInfo::Make24BitRGB(BITMAPINFOHEADER *pBih, int cx, int cy)
{
   ZeroMemory(pBih, sizeof BITMAPINFOHEADER);

   pBih->biSize = sizeof BITMAPINFOHEADER;
   pBih->biBitCount = 24;
   pBih->biClrImportant = 0;
   pBih->biClrUsed = 0;
   pBih->biCompression = BI_RGB;
   pBih->biWidth = cx;
   pBih->biHeight = cy;
   pBih->biPlanes = 1;
   pBih->biXPelsPerMeter = 96;
   pBih->biYPelsPerMeter = 96;
   pBih->biSizeImage = DIBSIZE(*pBih);
}


/*static*/HRESULT AVInfo::StaticCreateCompressor(
   AM_MEDIA_TYPE *pTargetType, 
   DWORD dwFcc,
   IBaseFilter **pCompressor,
   int nKeyframeRate, 
   int nQuality
   )
{
   // Create the System Device Enumerator.
   HRESULT hr;
   ICreateDevEnum *pSysDevEnum = NULL;
   hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
       IID_ICreateDevEnum, (void **)&pSysDevEnum);
   if (FAILED(hr))
   {
       return hr;
   }

   BITMAPINFOHEADER *pBmih = HEADER(pTargetType->pbFormat);
   DWORD dwWantedFcc = pBmih->biCompression;
   if (dwWantedFcc == BI_RGB)
   {
      *pCompressor = NULL;
      return S_OK;
   }

   char szWantedFcc[5];
   szWantedFcc[0] = (char) (dwWantedFcc & 0x000000ff);
   szWantedFcc[1] = (char) ((dwWantedFcc & 0x0000ff00) >> 8);
   szWantedFcc[2] = (char) ((dwWantedFcc & 0x00ff0000) >> 16);
   szWantedFcc[3] = (char) ((dwWantedFcc & 0xff000000) >> 24);
   szWantedFcc[4] = '\000';
   // In some cases, dwWantedFcc is different from the dwFcc
   // number. In that case, use both FCC codes for searching
   // a suitable compressor.
   char szWantedFcc2[5];
   if (dwFcc != -1)
   {
      szWantedFcc2[0] = (char) (dwFcc & 0x000000ff);
      szWantedFcc2[1] = (char) ((dwFcc & 0x0000ff00) >> 8);
      szWantedFcc2[2] = (char) ((dwFcc & 0x00ff0000) >> 16);
      szWantedFcc2[3] = (char) ((dwFcc & 0xff000000) >> 24);
      szWantedFcc2[4] = '\000';
   }
   else
   {
      szWantedFcc2[0] = '\000';
   }

   // Obtain a class enumerator for the video compressor category.
   IEnumMoniker *pEnumCat = NULL;
   hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoCompressorCategory, &pEnumCat, 0);

   bool foundCodec = false;

   if (hr == S_OK) 
   {
       // Enumerate the monikers.
       IMoniker *pMoniker = NULL;
       ULONG cFetched;
       while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK && !foundCodec)
       {
           IPropertyBag *pPropBag;
           hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
               (void **)&pPropBag);
           if (SUCCEEDED(hr))
           {
               // To retrieve the filter's friendly name, do the following:
               VARIANT varName;
               VariantInit(&varName);
               hr = pPropBag->Read(L"FriendlyName", &varName, 0);
               VariantClear(&varName);
               hr = pPropBag->Read(L"FccHandler", &varName, 0);
               if (SUCCEEDED(hr))
               {
                  _bstr_t bsFcc(varName);
                  char *szFcc = (char *) bsFcc;

//                  DWORD dwThisFcc = ((DWORD) szFcc[0]) +
//                     (((DWORD) szFcc[1]) << 8) + (((DWORD) szFcc[2]) << 16) + 
//                     (((DWORD) szFcc[3]) << 24);

                  //if (dwThisFcc == dwWantedFcc)
                  if ((_strnicmp(szFcc, szWantedFcc, 4) == 0) ||
                      (dwFcc != -1 && _strnicmp(szFcc, szWantedFcc2, 4) == 0) ||
                      (_strnicmp(szFcc, "divx", 4) == 0 && _strnicmp(szWantedFcc, "DX50", 4) == 0) ||
                      (_strnicmp(szFcc, "msvc", 4) == 0 && _strnicmp(szWantedFcc, "CRAM", 4) == 0))
                  {
                     hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void **) pCompressor);
                     foundCodec = true;
                  }
               }
               else
               {
                  hr = S_FALSE; // this is no failure
               }
               VariantClear(&varName);

               // To create an instance of the filter, do the following:
               //IBaseFilter *pFilter;
               //hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
               //    (void**)&pFilter);
               // Now add the filter to the graph. 
               //Remember to release pFilter later.
               pPropBag->Release();
           }
           pMoniker->Release();
       }
       pEnumCat->Release();
   }
   pSysDevEnum->Release();

   if (!foundCodec)
      return VFW_E_NOT_FOUND;
   else
   {
      // And now we'd like to configure the codec
      hr = StaticConfigureCompressor(*pCompressor, nKeyframeRate, nQuality);
   }

   if (FAILED(hr) && *pCompressor)
   {
      (*pCompressor)->Release();
      *pCompressor = NULL;
   }

   return hr;
}

HRESULT AVInfo::StaticConfigureCompressor(IBaseFilter *pCompressor, int nKeyframeRate, int nQuality)
{
   HRESULT hr = S_OK;
   CComPtr<IEnumPins> pEnumPins;
   IPin *pPins[] = {NULL, NULL};
   IPin *pOut = NULL;
   if (SUCCEEDED(hr))
      hr = pCompressor->EnumPins(&pEnumPins);
   if (SUCCEEDED(hr) && pEnumPins)
   {
      ULONG cFetched = 0;
      hr = pEnumPins->Next(2, pPins, &cFetched);
   }
   if (SUCCEEDED(hr))
   {
      PIN_DIRECTION pinDir;
      hr = pPins[0]->QueryDirection(&pinDir);
      if (SUCCEEDED(hr))
      {
         if (pinDir == PINDIR_OUTPUT)
            pOut = pPins[0];
         else
            pOut = pPins[1];
      }
   }
   CComQIPtr< IAMVideoCompression > pAMVC(pOut);

   if (SUCCEEDED(hr) && pAMVC)
   {
      long capabilities = 0;
      int cbVersion = 0;
      int cbDescription = 0;
      long nDefaultKeyframes = 0;
      long nDefaultPFrames = 0;
      double defaultQuality = 0.75;
      //hr = pAMVC->GetInfo(NULL, NULL, NULL, NULL, NULL, NULL, NULL, &capabilities);
      hr = pAMVC->GetInfo(
         NULL, &cbVersion,
         NULL, &cbDescription,
         &nDefaultKeyframes,
         &nDefaultPFrames,
         &defaultQuality,
         &capabilities);
      if (SUCCEEDED(hr))
      {
         if ((capabilities & CompressionCaps_CanQuality) > 0)
            hr = pAMVC->put_Quality(((double) nQuality) / 100.0);
         if (SUCCEEDED(hr) && ((capabilities & CompressionCaps_CanKeyFrame) > 0))
            hr = pAMVC->put_KeyFrameRate(nKeyframeRate);
         if (SUCCEEDED(hr) && ((capabilities & CompressionCaps_CanBFrame) > 0))
            hr = pAMVC->put_PFramesPerKeyFrame(nKeyframeRate - 1);
      }
   }
   else
      hr = E_NOINTERFACE;

   if (pPins[0])
      pPins[0]->Release();
   if (pPins[1])
      pPins[1]->Release();
   pOut = NULL;

   return hr;
}


class CWriteWavSampleCounter : public ISampleGrabberCB
{
public:
   CWriteWavSampleCounter(HWND hWndReport) : m_hWndReport(hWndReport)
   {
      m_nTotalMs = 0;
      m_nCurrentMs = 0;
      m_dLastPosted = 0.0;
   }

   virtual void SetTotalMs(int nTotalMs) { m_nTotalMs = nTotalMs; }

   int m_nTotalMs;
   int m_nCurrentMs;
   double m_dLastPosted;
   HWND m_hWndReport;

   virtual ~CWriteWavSampleCounter() {}

   STDMETHODIMP QueryInterface(REFIID riid, LPVOID *pUnk)
   {
      return E_NOINTERFACE;
   }

   virtual ULONG STDMETHODCALLTYPE AddRef() { return 1; }
   virtual ULONG STDMETHODCALLTYPE Release() { return 1; }

   virtual HRESULT STDMETHODCALLTYPE BufferCB(double sampleTime, BYTE *pBuffer, long bufferLen)
   {
      return SampleCB(sampleTime, NULL);
   }

   virtual HRESULT STDMETHODCALLTYPE SampleCB(double sampleTime, IMediaSample *pSample)
   {
      if (m_hWndReport && (sampleTime - m_dLastPosted > 1.0))
      {
         double dTotal = ((double) m_nTotalMs) / 1000;
         int nPercent = (int) (100.0 * sampleTime / dTotal);
         ::PostMessage(m_hWndReport, WM_USER, 999, nPercent);
         m_dLastPosted = sampleTime;
      }
      return S_OK;
   }
};

bool AVInfo::m_bWriteWavFromFileCancelRequest = false;

HRESULT AVInfo::StaticWriteWavFromFile(const _TCHAR *tszSrcFileName, int nStreamId, 
                                       const _TCHAR *tszTargetFileName, 
                                       bool bWindowsMediaMode,
                                       HWND hWndReport)
{
   m_bWriteWavFromFileCancelRequest = false;

   HRESULT hr = CoInitialize(NULL);

   // Start with checking disk space on target drive.
   {
      __int64 i64DiskSpace = LIo::GetDiskSpace(tszTargetFileName);

      // If we don't have enough space (at least 1MB), don't even start.
      if (i64DiskSpace != -1 && i64DiskSpace < 1048576i64)
         hr = E_OUTOFMEMORY;
   }

   if (SUCCEEDED(hr))
   {
      CComPtr<IGraphBuilder> pGraph;
      CComPtr<ICaptureGraphBuilder2> pBuilder;
      CComPtr<IFileSourceFilter> pFileSource;
      CComPtr<IBaseFilter> pBaseFileSource;
      CComPtr<ISampleGrabber> pSampleGrabber;
      CComPtr<IBaseFilter> pBaseSampleGrabber;
      CComPtr<CWavDestFilter> pWavDestFilter;
      CComPtr<IBaseFilter> pWavTransform;
      CComPtr<IBaseFilter> pMux;
      CComPtr<IMediaControl> pControl;
      CComPtr<IMediaEvent> pEvent;

      CWriteWavSampleCounter sampleCounter(hWndReport);
      // How long is this file?
      if (SUCCEEDED(hr))
      {
         AVInfo avInfo;
         hr = avInfo.Initialize(tszSrcFileName);
         double dLength = 0.0;
         if (SUCCEEDED(hr))
            hr = avInfo.GetLength(&dLength);
         if (SUCCEEDED(hr))
            sampleCounter.SetTotalMs((int) (1000.0 * dLength));
      }

      // First, create a filter graph instance
      if (SUCCEEDED(hr))
         hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_ALL, IID_IGraphBuilder, (void **) &pGraph);
      // We also need a Capture graph builder because of the
      // RenderStream() method (easier to use). The term
      // capture graph builder does not mean that you can't
      // use it to build other graphs than capture graphs.
      if (SUCCEEDED(hr))
         hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_ALL, IID_ICaptureGraphBuilder2, (void **) &pBuilder);
      // Attach the filter graph to the Capture graph builder
      if (SUCCEEDED(hr))
         hr = pBuilder->SetFiltergraph(pGraph);
      // Create a file source filter to be able to read the 
      // file asynchronuously

      if (!bWindowsMediaMode)
      {
         if (SUCCEEDED(hr))
           hr = CoCreateInstance(CLSID_AsyncReader, NULL, CLSCTX_ALL, IID_IFileSourceFilter, (void **) &pFileSource);
      }
      else
      {
         if (SUCCEEDED(hr))
            hr = CoCreateInstance(CLSID_WMAsfReader, NULL, CLSCTX_ALL, IID_IFileSourceFilter, (void **) &pFileSource);
      }

      // In order to add the file source filter to the filter
      // graph, we need its IBaseFilter interface, too.
      if (SUCCEEDED(hr))
        hr = pFileSource->QueryInterface(IID_IBaseFilter, (void **) &pBaseFileSource);
      // Add the IBaseFilter of the file source filter to the graph
      // This must always be done before connecting a filter to
      // other filters.
      if (SUCCEEDED(hr))
         hr = pGraph->AddFilter(pBaseFileSource, L"File Source");
      // Now set the file name of the file to be loaded
      // by the file source filter
      if (SUCCEEDED(hr))
      {
         // We get the file name as a CString, but we need
         // a wide character string, a WCHAR * (LPWSTR)
         int fileNameLength = _tcslen(tszSrcFileName);
         WCHAR *wszFilename = new WCHAR[fileNameLength + 1];
#ifdef _UNICODE
         wcscpy(wszFilename, tszSrcFileName);
#else
         int res = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, tszSrcFileName, -1, wszFilename, fileNameLength + 1);
#endif
         hr = pFileSource->Load(wszFilename, NULL);
         delete[] wszFilename;
      }

      // We need a sample grabber instance in order to grab
      // the samples from the audio file.
      if (SUCCEEDED(hr))
         hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_ALL, IID_ISampleGrabber, (void **) &pSampleGrabber);
      // Set our sample counter as the callback
      if (SUCCEEDED(hr))
         hr = pSampleGrabber->SetCallback((ISampleGrabberCB *) &sampleCounter, 0);
      // As with the file source filter, we need the IBaseFilter
      // pointer to the sample grabber in order to insert the
      // filter into the filter graph.
      if (SUCCEEDED(hr))
         hr = pSampleGrabber->QueryInterface(IID_IBaseFilter, (void **) &pBaseSampleGrabber);
      // Ok, now insert the sample grabber into the graph
      if (SUCCEEDED(hr))
         hr = pGraph->AddFilter(pBaseSampleGrabber, L"Sample Grabber");
      // Specify the media type to use in the sample grabber: 
      // We need an Audio PCM format of some kind.
      if (SUCCEEDED(hr))
      {
         CMediaType mtInput;
         mtInput.InitMediaType();
         mtInput.SetType(&MEDIATYPE_Audio);
         mtInput.SetSubtype(&MEDIASUBTYPE_PCM);
         hr = pSampleGrabber->SetMediaType(&mtInput);
      }

      // Now connect the file source with the sample grabber.
      // Depending on the type of the source file, other inter-
      // mediate filters may be inserted in order to create
      // a PCM audio stream for the sample grabber
      if (SUCCEEDED(hr))
         hr = pBuilder->RenderStream(NULL, NULL, pBaseFileSource, NULL, pBaseSampleGrabber);

      // If everything has gone alright up to now, we should be
      // able to read out the format of the stream coming from
      // the audio file now
      if (SUCCEEDED(hr))
      {
         // Read out the format:
         AM_MEDIA_TYPE mt;
         ZeroMemory(&mt, sizeof AM_MEDIA_TYPE);
         hr = pSampleGrabber->GetConnectedMediaType(&mt);

         if (SUCCEEDED(hr))
         {
            if (mt.formattype == FORMAT_WaveFormatEx)
            {
            }
            else
               hr = VFW_E_INVALIDMEDIATYPE;
         }

         FreeMediaType(mt);
      }

      // We need to connect the sample grabber to a Renderer;
      // you always have to render a filter graph in some way.
      // The renderer is a file writer being passed through
      // a WAV file transform filter.
      if (SUCCEEDED(hr))
         hr = CWavDestFilter::CreateInstance(&pWavDestFilter);
      if (SUCCEEDED(hr))
         hr = pWavDestFilter->QueryInterface(IID_IBaseFilter, (void **) &pWavTransform);

      if (SUCCEEDED(hr))
         hr = pGraph->AddFilter(pWavTransform, L"WAV Transform");

      // Now connect up the file writer and the WAV transform
      if (SUCCEEDED(hr))
         hr = pBuilder->SetOutputFileName(&CLSID_FileWriter, _bstr_t(tszTargetFileName), &pMux, NULL);
      if (SUCCEEDED(hr))
         hr = pBuilder->RenderStream(NULL, NULL, pBaseSampleGrabber, pWavTransform, pMux);

      // We need an IMediaControl interface to the graph in order
      // to start the processing of the source file
      if (SUCCEEDED(hr))
         hr = pGraph->QueryInterface(IID_IMediaControl, (void **) &pControl);
      // ... and an IMediaEvent interface in order to know when
      // the processing has finished.
      if (SUCCEEDED(hr))
         hr = pGraph->QueryInterface(IID_IMediaEvent, (void **) &pEvent);

      // In order to process the samples as fast as possible, we
      // need the IMediaFilter interface of the filter graph.
      // By setting the sync source to NULL the samples are
      // processed without checking for the reference time. Without
      // doing this, the samples would be processed in real time,
      // which is not desirable here
      {
         CComPtr<IMediaFilter> pMediaFilter;
         if (SUCCEEDED(hr))
            hr = pGraph->QueryInterface(IID_IMediaFilter, (void **) &pMediaFilter);
         if (SUCCEEDED(hr))
            hr = pMediaFilter->SetSyncSource(NULL);
      }

      if (SUCCEEDED(hr))
         hr = pControl->Run();
      // Wait until it's ready
      long evCode = 0;
      if (SUCCEEDED(hr))
      {
         while (SUCCEEDED(hr) && evCode != EC_COMPLETE && evCode != EC_ERRORABORT && evCode != EC_USERABORT)
         {
            hr = pEvent->WaitForCompletion(100, &evCode);
            if (hr == E_ABORT)
            {
               if (m_bWriteWavFromFileCancelRequest)
               {
                  pControl->Stop();
                  hr = E_ABORT;
               }
               else
               {
                  // Continue checking
                  hr = S_OK;
               }
            }
         }

         if (SUCCEEDED(hr) && evCode != EC_COMPLETE)
         {
            // Check for "out of disk space"
            __int64 i64DiskSpace = LIo::GetDiskSpace(tszTargetFileName);
            if (i64DiskSpace != -1 && i64DiskSpace < 1048576i64)
               hr = E_OUTOFMEMORY;
            else
               hr = E_FAIL;
         }
      }
   }

   ::CoUninitialize();

   if (FAILED(hr))
   {
      // Delete the file
      ::DeleteFile(tszTargetFileName);
   }

   return hr;
}

void AVInfo::StaticWriteWavFromFileCancel()
{
   m_bWriteWavFromFileCancelRequest = true;
}