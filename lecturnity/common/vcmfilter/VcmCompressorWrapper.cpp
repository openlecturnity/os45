//
// VcmCompressorWrapper.cpp
//
// VCMCompressorWrapper

#include <streams.h>     // DirectShow (includes windows.h)

#include <initguid.h>    // declares DEFINE_GUID to declare an EXTERN_C const.
                         // Use once per project.
#include <vfw.h>
#include <mmsystem.h>
#include <stdio.h>
#include <tchar.h>
#include "VcmCompressorWrapper.h"

#ifndef MODIFIED_DLLSETUP_BASECLASSES
#error "Es muss eine Änderung an dllsetup.cpp aus BaseClasses vorgenommen werden! Martin fragen!"
#endif

#ifdef _DEBUG
#define DEBUGOUT(x) OutputDebugString(x)
#else
#define DEBUGOUT(x) ;
#endif

//
// Common DLL routines and tables
//

// Object table - all com objects in this DLL
CFactoryTemplate g_Templates[]=
{   { L"VCM Compressor Wrapper"
    , &CLSID_VCMCOMPRESSORWRAPPER
    , CVCMCompressorWrapper::CreateInstance
    , NULL
    , &CVCMCompressorWrapper::sudFilter 
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
// Filter VcmCompressorWrapper routines
//
const AMOVIESETUP_MEDIATYPE CVCMCompressorWrapper::sudInputTypes[] = {
   { &MEDIATYPE_Video, &GUID_NULL },
};

const AMOVIESETUP_MEDIATYPE CVCMCompressorWrapper::sudOutputTypes[] = {
   { &MEDIATYPE_Video, &GUID_NULL },
};

const AMOVIESETUP_PIN CVCMCompressorWrapper::sudPins[] = { {
   L"",
      FALSE,          // Is this pin rendered?
      FALSE,          // Is it an output pin?
      FALSE,          // Can the filter create zero instances?
      FALSE,          // Does the filter create multiple instances?
      &GUID_NULL,     // Obsolete.
      NULL,           // Obsolete.
      1,              // Number of media types.
      CVCMCompressorWrapper::sudInputTypes   // Pointer to media types.
}, {
   L"",
      FALSE,          // Is this pin rendered?
      TRUE,           // Is it an output pin?
      FALSE,          // Can the filter create zero instances?
      FALSE,          // Does the filter create multiple instances?
      &GUID_NULL,     // Obsolete.
      NULL,           // Obsolete.
      1,              // Number of media types.
      CVCMCompressorWrapper::sudOutputTypes   // Pointer to media types.

} };
// setup data - allows the self-registration to work.
const AMOVIESETUP_FILTER CVCMCompressorWrapper::sudFilter = {
    g_Templates[0].m_ClsID    // clsID
  , g_Templates[0].m_Name     // strName
  , MERIT_SW_COMPRESSOR      // dwMerit
  // TODO: Add pin details here.
  , 2                         // nPins
  , CVCMCompressorWrapper::sudPins // lpPin
};  


//
// Constructor
//
// If any part of construction fails *phr should be set to 
// a failure code.
CVCMCompressorWrapper::CVCMCompressorWrapper(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
: CTransformFilter(tszName, punk, *sudFilter.clsID)
, m_FilterLock()
{
   DEBUGOUT("VCM Compressor: Constructor\n");
   m_mediaTypeIn.InitMediaType();
   m_mediaTypeOut.InitMediaType();
   m_codecOffset   = 0;
   m_hic           = NULL;
   m_outBufferSize = 0;
   m_pPrevImage    = NULL;
   m_pOutput       = NULL;
   m_pInput        = NULL;
   m_firstTransformDone = false;
}


//
// Destructor
//
CVCMCompressorWrapper::~CVCMCompressorWrapper() 
{
   DEBUGOUT("VCM Compressor: Destructor\n");
   if (m_pInput)
      delete m_pInput;
   m_pInput = NULL;
   if (m_pOutput)
      delete m_pOutput;
   m_pOutput = NULL;
}


//
// CreateInstance
//
// Provide the way for the COM support routines in <streams.h>
// to create a CVCMCompressorWrapper object
CUnknown * WINAPI CVCMCompressorWrapper::CreateInstance(LPUNKNOWN punk, HRESULT *phr) {

    CUnknown *pNewObject = new CVCMCompressorWrapper(NAME("imc VCM Compressor Wrapper"), punk, phr );
    if (pNewObject == NULL) {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
}

HRESULT CVCMCompressorWrapper::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
   if (riid == IID_IAMVfwCompressDialogs)
      return GetInterface((IAMVfwCompressDialogs *) this, ppv);
   return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
}

//
// GetPin
//
// return pointer to a pin.
CBasePin* CVCMCompressorWrapper::GetPin(int n)
{
   HRESULT hr = S_OK;
   if (n == 0)
   {
      if (!m_pInput)
      {
         m_pInput = new CVCMWrapperInputPin(
            _T("In"),
            this,
            &hr,
            L"In");
      }
      if (SUCCEEDED(hr))
         return m_pInput;
   }
   else if (n == 1)
   {
      if (!m_pOutput)
      {
         m_pOutput = new CVCMWrapperOutputPin(
            _T("Out"),
            this,
            &hr,
            L"Out");
      }
      if (SUCCEEDED(hr))
         return m_pOutput;
   }

   return NULL;
}

HRESULT CVCMCompressorWrapper::CheckInputType(const CMediaType *pmtIn)
{
   //BITMAPINFOHEADER *pBmih = HEADER(pmtIn->pbFormat);

   // We'll say we accept any kind of input media type;
   // as long as it has FORMAT_VideoInfo as format type,
   // and as long as it is a video format at all.
   // Well, and as long as it's RGB 24 data. Haha.
   // If that's not true, that will turn out in the
   // GetMediaType() method: That method would then
   // return VFW_S_NO_MORE_ITEMS directly.
   DEBUGOUT("VCM Compressor: CheckInputType\n");

   if (!(*(pmtIn->Type()) == MEDIATYPE_Video) ||
       !(*(pmtIn->FormatType()) == FORMAT_VideoInfo) ||
       !(*(pmtIn->Subtype()) == MEDIASUBTYPE_RGB24))
      return VFW_E_TYPE_NOT_ACCEPTED;

   m_mediaTypeIn = *pmtIn; // real copy! not shallow

   return S_OK;
}

HRESULT CVCMCompressorWrapper::CheckTransform(const CMediaType *pmtIn, const CMediaType *pmtOut)
{
   DEBUGOUT("VCM Compressor: CheckTransform\n");
   if (*pmtIn == m_mediaTypeIn &&
       *pmtOut == m_mediaTypeOut)
      return S_OK;

   // Ok, we have something that does not match exactly;
   // let's try and see if we can get it right anyway

   // Do we accept the input type at all?
   HRESULT hr = CheckInputType(pmtIn);

   if (SUCCEEDED(hr))
   {
      // Yes, we do. Now let's try and find an output format which
      // matches pmtOut.
      HRESULT hr2 = S_OK;
      bool foundFormat = false;
      int iPosition = 0;
      while (!foundFormat && hr2 != VFW_S_NO_MORE_ITEMS)
      {
         CMediaType out;
         hr2 = GetMediaType(iPosition, &out);
         if (out == *pmtOut)
            foundFormat = true;
         else
            iPosition++;
      }

      if (foundFormat)
         return S_OK;
   }

   return VFW_E_TYPE_NOT_ACCEPTED;
}

HIC CVCMCompressorWrapper::OpenAndCheckCompressor()
{
   DEBUGOUT("VCM Compressor: OpenAndCheckCompressor\n");
   // Note the usage of m_fccHandler here. For DivX, biCompression
   // is not always equal to the fccHandler (DivX 5.x):
   HIC hic = ::ICOpen(ICTYPE_VIDEO, m_fccHandler, ICMODE_QUERY);
   if (hic)
   {
      BITMAPINFOHEADER *pbmiIn  = HEADER(m_mediaTypeIn.Format());
      BITMAPINFOHEADER *pbmiOut = HEADER(m_mediaTypeOut.Format());
      BITMAPINFO bmiIn;
      BITMAPINFO bmiOut;
      bmiIn.bmiHeader = *pbmiIn;
      bmiOut.bmiHeader = *pbmiOut;

      DWORD res = ICCompressQuery(hic, &bmiIn, &bmiOut);
      if (ICERR_OK != res)
      {
#ifdef _DEBUG
         printf("CVCMCompressorWrapper::OpenAndCheckCompressor(): ICCompressQuery failed.\n");
#endif
         ::ICClose(hic);
         hic = NULL;
      }
   }

   return hic;
}

HRESULT CVCMCompressorWrapper::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest)
{
   DEBUGOUT("VCM Compressor: DecideBufferSize\n");
   // We have passed all media type checks; now
   // we'll try to get the buffer size from the
   // current compressor.

   // Try to open the compressor in query mode
   // and ask it whether it still can manage the input and
   // output formats 'n stuff
   HRESULT hr = S_OK;
   HIC hic = OpenAndCheckCompressor();

   BITMAPINFOHEADER *pbmiIn  = HEADER(m_mediaTypeIn.Format());
   BITMAPINFOHEADER *pbmiOut = HEADER(m_mediaTypeOut.Format());
   BITMAPINFO bmiIn;
   BITMAPINFO bmiOut;
   bmiIn.bmiHeader = *pbmiIn;
   bmiOut.bmiHeader = *pbmiOut;

   if (hic)
   {
      if (ppropInputRequest->cBuffers == 0)
         ppropInputRequest->cBuffers = 1;
      if (ppropInputRequest->cbAlign == 0)
         ppropInputRequest->cbAlign = 1;

      m_outBufferSize = ICCompressGetSize(hic, &bmiIn, &bmiOut);
#ifdef _DEBUG
      char t[256];
      sprintf(t, "VCM Compressor: m_outBufferSize == %d\n", m_outBufferSize);
      DEBUGOUT(t);
#endif

      // We do not need the compressor anymore right now:
      ::ICClose(hic);
      hic = NULL;

      ppropInputRequest->cbBuffer = m_outBufferSize;
      
      ALLOCATOR_PROPERTIES actualProps;
      hr = pAlloc->SetProperties(ppropInputRequest, &actualProps);
      if (FAILED(hr))
         return hr;

      if (ppropInputRequest->cbBuffer > actualProps.cbBuffer)
      {
         // We cannot accept smaller buffers than requested!
         return E_FAIL;
      }
      return S_OK;
   }

#ifdef _DEBUG
   printf("CVCMCompressorWrapper::DecideBufferSize() failed.\n");
#endif
   return E_FAIL;
}

HRESULT CVCMCompressorWrapper::CheckConnect(PIN_DIRECTION pinDir, IPin *pPin)
{
   DEBUGOUT("VCM Compressor: CheckConnect\n");
   HRESULT hr = S_OK;

   if (pinDir == PINDIR_OUTPUT)
   {
      BITMAPINFOHEADER *pbmihIn = HEADER(m_mediaTypeIn.Format());
      BITMAPINFOHEADER *pbmihOut = HEADER(m_mediaTypeOut.Format());

      // Reset the frame counter properly:
      m_lFrameNum = 0;
      m_lastKeyFrame = 0;
      m_lastFrameTime = 0i64;

      // Now open the codec
      m_hic = ::ICOpen(ICTYPE_VIDEO, m_fccHandler, ICMODE_COMPRESS);

      // Did we succeed in that?
      if (m_hic)
      {
         // Yes, start the compression sequence
         BITMAPINFO bmiIn;
         BITMAPINFO bmiOut;
         bmiIn.bmiHeader = *pbmihIn;
         bmiOut.bmiHeader = *pbmihOut;

         // Do we need a back buffer? Only
         // if the TEMPORAL flag is set, we need
         // the backbuffer; otherwise the codec does
         // not create any delta frame, or it has
         // fast temporal compression, which means
         // that the codec manages its backbuffer by
         // itself.
         if ((m_icInfo.dwFlags & VIDCF_TEMPORAL) > 0 &&
             !((m_icInfo.dwFlags & VIDCF_FASTTEMPORALC) > 0))
         {
            // Yes, we need a back buffer. Create one.
            // First delete the old one, if it exists (should not).
            if (m_pPrevImage)
               delete[] m_pPrevImage;
            m_pPrevImage = NULL;
            m_pPrevImage = new BYTE[pbmihIn->biSizeImage];
            m_prevImageExists = false;
            if (!m_pPrevImage)
            {
               hr = VFW_E_BUFFER_NOTSET;
   #ifdef _DEBUG
               printf("CVCMCompressorWrapper::Transform() Could not create backbuffer!\n");
   #endif
            }
         }
      }
      else
         hr = E_FAIL;
   }

   return hr;
}

HRESULT CVCMCompressorWrapper::BreakConnect(PIN_DIRECTION pinDir)
{
   DEBUGOUT("VCM Compressor: BreakConnect\n");
   if (pinDir == PINDIR_OUTPUT)
   {
      if (m_hic)
      {
         ::ICCompressEnd(m_hic);
         ::ICClose(m_hic);
      }
      m_hic = NULL;
      if (m_pPrevImage)
         delete[] m_pPrevImage;
      m_pPrevImage = NULL;
   }
   return S_OK;
}

HRESULT CVCMCompressorWrapper::GetMediaType(int iPosition, CMediaType *pMediaType)
{
#ifdef _DEBUG
   char t[64];
   sprintf(t, "VCM Compressor: GetMediaType(%d, ...)\n", iPosition);
   DEBUGOUT(t);
#endif
   if (iPosition < 0)
      return E_INVALIDARG;

   if (iPosition == 0)
      m_codecOffset = 0;

   ICINFO icInfo;
   ZeroMemory(&icInfo, sizeof ICINFO);

   bool formatFound = false;

   BOOL success = TRUE;
   while (TRUE == success && !formatFound)
   {
      // Note that the documentation says that ICInfo
      // returns ICERR_OK if everything is fine. Not
      // true, it returns FALSE if it fails, and
      // TRUE if it's successful:
      success = ::ICInfo(ICTYPE_VIDEO, iPosition + m_codecOffset, &icInfo);
      if (TRUE == success)
      {
         BITMAPINFO bmiIn;
         ZeroMemory(&bmiIn, sizeof BITMAPINFO);
         // Now retrieve the BITMAPINFOHEADER from the
         // input media type. Note the usage of the HEADER()
         // macro.
         BITMAPINFOHEADER *pBmih = HEADER(m_mediaTypeIn.Format());

         // copy the BITMAPINFOHEADER part to bmiIn
         bmiIn.bmiHeader = *pBmih; 

         HIC hic = ::ICOpen(ICTYPE_VIDEO, icInfo.fccHandler, ICMODE_COMPRESS);
         if (hic)
         {
            DWORD fccHandler = icInfo.fccHandler;
            // Can this compressor handle this format at all?
            if (ICERR_OK == ICCompressQuery(hic, &bmiIn, NULL))
            {
               // Yes, what kind of format does it produce?
               int formatSize = ICCompressGetFormatSize(hic, &bmiIn);
               if (formatSize > 0 && formatSize < 10000) // heuristic
               {
                  BITMAPINFO *pBmiOut = (BITMAPINFO *) (new char[formatSize]);

                  // We have a reasonable size for the format, now
                  // retrieve the format (it's a BITMAPINFO structure)
                  if (ICERR_OK == ::ICCompressGetFormat(hic, &bmiIn, pBmiOut))
                  {
                     // This is highly interesting; retrieve some more
                     // information on the current codec:
                     // Note that ICGetInfo does not return ICERR_OK if
                     // everything is ok, but rather the number of copied
                     // bytes, that is, 0 means an error has occurred.
                     if (0 != ::ICGetInfo(hic, &m_icInfo, sizeof ICINFO))
                     {
                        // Because of some weird bug problably, the fccHandler
                        // is changed between the ICInfo() and ICGetInfo() call,
                        // so we want to reinstate the old fccHandler instead
                        // of the new one:
                        // m_icInfo.fccHandler = fccHandler;

                        char szFcc[5];
                        szFcc[0] = (char) (m_icInfo.fccHandler & 0x000000ff);
                        szFcc[1] = (char) ((m_icInfo.fccHandler & 0x0000ff00) >> 8);
                        szFcc[2] = (char) ((m_icInfo.fccHandler & 0x00ff0000) >> 16);
                        szFcc[3] = (char) ((m_icInfo.fccHandler & 0xff000000) >> 24);
                        szFcc[4] = 0;
                        printf("Codec #%d: %s (%s)\n", iPosition, m_icInfo.szName, szFcc);
                        // Jot down the fccHandler: We will need the fccHandler
                        // later on. Normally, m_icInfo.fccHandler equals the 
                        // biCompression field of the output BITMAPINFOHEADER.
                        // With DivX, this is not the case: It has the fccHandler
                        // 'divx', but enters 'DX50' as biCompression. It does
                        // no longer accept a ICQueryCompress() with 'divx' given
                        // as biCompression! Bastard.
                        m_fccHandler = m_icInfo.fccHandler;

                        m_supportsKeyFrames = (m_icInfo.dwFlags & VIDCF_TEMPORAL) > 0;
                        m_useKeyFrames      = m_supportsKeyFrames;
                        m_supportsQuality   = (m_icInfo.dwFlags & VIDCF_QUALITY) > 0;
                        m_quality           = 0.75; // 75% quality
                        m_supportsDatarate  = (m_icInfo.dwFlags & VIDCF_CRUNCH) > 0;
                        m_useDataRate       = false; // don't crunch by default
                        m_keyFrameRate      = 25; // in frames

                        // While we're at it: retrieve default quality and 
                        // keyframe rates:
                        if (m_supportsKeyFrames)
                           m_defaultKeyFrameRate = ICGetDefaultKeyFrameRate(hic);
                        else
                           m_defaultKeyFrameRate = 0;
                        if (m_supportsQuality)
                        {
                           DWORD dwDefQ = ICGetDefaultQuality(hic);
                           m_defaultQuality = ((double) dwDefQ) / 10000.0;
                        }
                        else
                           m_defaultQuality = 0.0;

                        // Nasty divx != DX50 fix:
                        // pBmiOut->bmiHeader.biCompression = m_icInfo.fccHandler;

                        // Ok, this is what we get then:
                        // FOURCCMap fcc(m_icInfo.fccHandler);
                        FOURCCMap fcc(pBmiOut->bmiHeader.biCompression);
                        BOOL hasTemporalCompression = 
                           (m_icInfo.dwFlags & VIDCF_TEMPORAL) > 0 ||
                           (m_icInfo.dwFlags & VIDCF_FASTTEMPORALC) > 0;
                        // Initialize a VIDEOINFOHEADER structure
                        VIDEOINFOHEADER vih;
                        ZeroMemory(&vih, sizeof VIDEOINFOHEADER);
                        vih.bmiHeader = pBmiOut->bmiHeader;
                        // Note: We don't care about the rcSource and
                        // rcTarget settings; leave them at zero, that's
                        // ok, as it seems.

                        m_mediaTypeOut.InitMediaType();
                        m_mediaTypeOut.SetType(&MEDIATYPE_Video);
                        m_mediaTypeOut.SetSubtype(&fcc);
                        m_mediaTypeOut.SetVariableSize();
                        m_mediaTypeOut.SetTemporalCompression(hasTemporalCompression);
                        m_mediaTypeOut.SetFormatType(&FORMAT_VideoInfo);
                        m_mediaTypeOut.SetFormat((BYTE *) &vih, sizeof VIDEOINFOHEADER);

                        *pMediaType = m_mediaTypeOut;

                        formatFound = true;
                     } // if (ICGetInfo())
                  }

                  delete[] ((char *) pBmiOut);
               }
            }
         }

         if (hic)
            ::ICClose(hic);
      } // if (ICInfo == ICERR_OK)

      if (!formatFound)
         m_codecOffset++;
   } // { while }

   if (formatFound)
      return S_OK;

   return VFW_S_NO_MORE_ITEMS;
}

HRESULT CVCMCompressorWrapper::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
#ifdef _DEBUG
   {
      LONGLONG time, time2;
      pIn->GetTime(&time, &time2);
      long lTime = (long) (time / 10000i64);
      char t[64];
      sprintf(t, "VCM Compressor: Transform@%d ms\n", lTime);
      DEBUGOUT(t);
   }
#endif
   HRESULT hr = S_FALSE;

   BITMAPINFOHEADER *pbmihIn = HEADER(m_mediaTypeIn.Format());
   BITMAPINFOHEADER *pbmihOut = HEADER(m_mediaTypeOut.Format());

   // Before compressing the first frame, send
   // some extra information to the codec, in case
   // they need it. Normally, the IV50 codec is not
   // used in conjunction with this compressor wrapper,
   // as it has an own "wrapper", but still it does
   // not hurt to initialize the codec as it were
   // the Indeo codec.
   if (!m_firstTransformDone)
   {
      VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *) m_mediaTypeOut.Format();

      // This message has to be sent to different codecs,
      // amongst them is the very screwy IV50 codec.
	   ICCOMPRESSFRAMES icf;

	   memset(&icf, 0, sizeof icf);

	   icf.dwFlags		= 0;
	   icf.lpbiOutput	= pbmihOut;
	   icf.lpbiInput	= pbmihIn;
	   icf.lStartFrame = 0;
	   icf.lFrameCount = 0x0FFFFFFF;
      if (m_supportsQuality)
   	   icf.lQuality	= (DWORD) (m_quality * 10000.0);
      else
         icf.lQuality   = 0;
//      if (codecInfo_.bSupportsDatarate && codecInfo_.bUseDatarate)
//         icf.lDataRate = codecInfo_.iDatarate * 1024; // bytes/s
//      else
      icf.lDataRate = 0; // 0x7fffffff; // no rate limit

      if (m_supportsKeyFrames)
         icf.lKeyRate	= m_keyFrameRate;
      else
         icf.lKeyRate   = 0;
	   icf.dwRate		= 1000000;
	   icf.dwScale		= (DWORD) (pVih->AvgTimePerFrame / 10i64); // 100 ns -> us

      // Ok, now send the message to the compressor:
	   ICSendMessage(m_hic, ICM_COMPRESS_FRAMES_INFO, (WPARAM)&icf, sizeof(ICCOMPRESSFRAMES));

      m_firstTransformDone = true;
   }

   if (m_hic)
   {
      // You have to differentiate between
      // two kinds of keyframes:
      // 1) The keyframes which have to be created if the
      //    codec has the flag VIDCF_TEMPORAL, i.e. "normal"
      //    keyframes. These can be created using the
      //    ICCOMPRESS_KEYFRAME flag in the dwFlags variable
      // 2) Forced keyframes which are needed if the last frame
      //    was not near the current frame (jumping DES re-
      //    compression). In that case, force a keyframe by
      //    using the ICCompressBegin() macro.
      bool createKeyFrame = false;
      bool forceKeyFrame  = false;

      // Check if we need a forced keyframe:
      // Heuristic is: If the last frame is more than
      // one second away from the current, then we'll
      // force a keyframe! If we're on the first frame,
      // then a keyframe is forced anyhow
      LONGLONG frameTimeStart = 0i64;
      LONGLONG frameTimeStop  = 0i64;
      HRESULT h2 = pIn->GetTime(&frameTimeStart, &frameTimeStop);
      if (SUCCEEDED(h2))
      {
         if (abs((int) ((frameTimeStart - m_lastFrameTime) / 10000i64)) > 1000) // 1 second
         {
            forceKeyFrame = true;
         }
      }

      if (m_lFrameNum == 0)
         forceKeyFrame = true;

      // We only need to check if we need a keyframe
      // if the following criteria are satisfied:
      // 1) the codec supports keyframes,
      // 2) we should also care about the keyframes,
      // 3) a keyframe is not already being forced,
      // 4) we already know we need a keyframe (first frame)
      if (m_supportsKeyFrames && m_useKeyFrames && !forceKeyFrame && !createKeyFrame)
      {
         int f = m_lFrameNum;
         int k = m_lastKeyFrame;
         if (f - k >= ((int) m_keyFrameRate) - 1)
            createKeyFrame = true;
      }

      // If we reeeally need a keyframe, then we should make
      // sure one is created by using the ICCompressBegin()
      // macro. That macro resets the codec to its initial
      // state; this makes the codec start off again with 
      // a keyframe.
      if (forceKeyFrame)
      {
#ifdef _DEBUG
         printf("Forcing a keyframe at time %dms\n", (int) (frameTimeStart/10000i64));
#endif
         BITMAPINFO bmiIn;
         BITMAPINFO bmiOut;
         bmiIn.bmiHeader = *pbmihIn;
         bmiOut.bmiHeader = *pbmihOut;
         m_lFrameNum = 0;
         m_lastKeyFrame = 0;

         DWORD res = ::ICCompressBegin(m_hic, &bmiIn, &bmiOut);
         if (ICERR_OK != res)
         {
#ifdef _DEBUG
            printf("CVCMCompressorWrapper::Transform() ICCompressBegin() failed!\n");
#endif
         }
      }

      createKeyFrame = createKeyFrame || forceKeyFrame;

      BYTE *pInBuffer = NULL;
      BYTE *pOutBuffer = NULL;
      hr = pIn->GetPointer(&pInBuffer);
      if (SUCCEEDED(hr) && pInBuffer)
         hr = pOut->GetPointer(&pOutBuffer);
      else
         hr = E_POINTER;

      if (SUCCEEDED(hr))
      {
         DWORD dwFlags       = createKeyFrame ? ICCOMPRESS_KEYFRAME : 0;
         DWORD ckId          = 0;
         DWORD dwReturnFlags = 0;
         DWORD dwFrameSize   = m_lFrameNum == 0 ? 0xffffff : 0; // not supported at the moment
         DWORD dwQuality     = m_supportsQuality  ? ((DWORD) (m_quality * 10000.0)) : 0;
         BITMAPINFOHEADER *pbmihPrev = createKeyFrame || !m_prevImageExists ? NULL : pbmihIn;
         BYTE             *pPrevBuffer = createKeyFrame || !m_prevImageExists ? NULL : m_pPrevImage;

         DWORD res = ICCompress(
            m_hic,
            dwFlags, // dwFlags
            pbmihOut, // lpbiOutput
            (LPVOID) pOutBuffer, // lpData [out]
            pbmihIn, // lpbiInput
            (LPVOID) pInBuffer, // lpBits [in]
            &ckId, // lpckid
            &dwReturnFlags, // lpdwFlags
            m_lFrameNum++, // lFrameNum
            dwFrameSize, // dwFrameSize,
            dwQuality, // dwQuality
            pbmihPrev, // lpbiPrev
            pPrevBuffer // lpPrev [previous buffer]
         );

#ifdef _DEBUG
         if (ICERR_OK != res)
            printf("VCVMCompressorWrapper::Transform() ICCompress(@%d ms) == %08x\n", (int) (frameTimeStart / 10000i64), res);
         printf("%d ms: data length: %d, encoded length: %d\n", (int) (frameTimeStart / 10000i64), pIn->GetActualDataLength(), pbmihOut->biSizeImage);
#endif

         bool wasKeyFrameCreated = (dwReturnFlags & AVIIF_KEYFRAME) > 0;

         // Save the time of this frame
         if (SUCCEEDED(h2))
            m_lastFrameTime = frameTimeStart;
         else
            m_lastFrameTime = 0i64;

         hr = pOut->SetActualDataLength(pbmihOut->biSizeImage);
         if (SUCCEEDED(hr))
         {
#ifdef _DEBUG
            if (!wasKeyFrameCreated && createKeyFrame)
            {
               printf("Keyframe was not creted, but codec was told to.\n");
            }
#endif
            hr = pOut->SetSyncPoint(wasKeyFrameCreated ? TRUE : FALSE);

            if (wasKeyFrameCreated)
            {
               m_lastKeyFrame = m_lFrameNum;
#ifdef _DEBUG
               printf("* keyframe@%d ms\n", (int) (frameTimeStart / 10000i64));
#endif
            }
         }

         if (m_pPrevImage)
         {
            memcpy(m_pPrevImage, pInBuffer, pIn->GetSize());
            m_prevImageExists = true;
         }
      }

   }
   else
   {
      hr = E_FAIL;
#ifdef _DEBUG
      printf("CVCMCompressorWrapper::Transform() m_hic == NULL\n");
#endif
   }

#ifdef _DEBUG
   if (FAILED(hr))
   {
      printf("CVCMCompressorWrapper::Transform() failed: hr == %08x\n", hr);
   }
#endif

   return hr;
}

// IAMVfwCompressDialogs
HRESULT CVCMCompressorWrapper::ShowDialog(int iDialog, HWND hWnd)
{
   if (m_hic == NULL)
      return VFW_E_WRONG_STATE;

   HRESULT hr = S_FALSE;

   switch (iDialog)
   {
   case VfwCompressDialog_Config:
      {
         DWORD res = ICConfigure(m_hic, hWnd);
         if (FALSE != res)
            hr = S_OK;
      }
      break;

   case VfwCompressDialog_About:
      {
         DWORD res = ICAbout(m_hic, hWnd);
         if (FALSE != res)
            hr = S_OK;
      }
      break;

   case VfwCompressDialog_QueryConfig:
      {
         DWORD res = ICQueryConfigure(m_hic);
         if (FALSE != res)
            hr = S_OK;
      }
      break;

   case VfwCompressDialog_QueryAbout:
      {
         DWORD res = ICQueryAbout(m_hic);
         if (FALSE != res)
            hr = S_OK;
      }
      break;
   }

   return S_OK;
}

HRESULT CVCMCompressorWrapper::GetState(LPVOID pState, int *pcbState)
{
   if (NULL == m_hic)
      return VFW_E_WRONG_STATE;

   HRESULT hr = S_OK;

   if (pState == NULL)
   {
      *pcbState = ICGetStateSize(m_hic);
   }
   else
   {
      int size = ICGetStateSize(m_hic);
      if (*pcbState >= size)
      {
         DWORD res = ICGetState(m_hic, pState, size);
         if (ICERR_OK == res)
         {
            *pcbState = size;
         }
         else
            hr = E_FAIL;
      }
      else
         hr = VFW_E_BUFFER_OVERFLOW;
   }

   return hr;
}

HRESULT CVCMCompressorWrapper::SetState(LPVOID pState, int cbState)
{
   if (NULL == m_hic)
      return VFW_E_WRONG_STATE;

   DWORD res = ICSetState(m_hic, pState, cbState);
   if (res == 0)
      return E_FAIL;

   return S_OK;
}

HRESULT CVCMCompressorWrapper::SendDriverMessage(int uMsg, long dw1, long dw2)
{
   if (NULL == m_hic)
      return VFW_E_WRONG_STATE;

   return ICSendMessage(m_hic, uMsg, dw1, dw2);
}



//
// In- and output pins
//

// Input pin
CVCMWrapperInputPin::CVCMWrapperInputPin(TCHAR *pObjectName, 
                                         CTransformFilter *pTransformFilter,
                                         HRESULT *phr,
                                         LPCWSTR pName) :
CTransformInputPin(pObjectName,
                   pTransformFilter,
                   phr,
                   pName)
{
}

CVCMWrapperInputPin::~CVCMWrapperInputPin()
{
}

// Output pin
CVCMWrapperOutputPin::CVCMWrapperOutputPin(TCHAR *pObjectName, 
                                         CTransformFilter *pTransformFilter,
                                         HRESULT *phr,
                                         LPCWSTR pName) :
CTransformOutputPin(pObjectName,
                   pTransformFilter,
                   phr,
                   pName)
{
}

CVCMWrapperOutputPin::~CVCMWrapperOutputPin()
{
}

HRESULT CVCMWrapperOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
   if (riid == IID_IAMVideoCompression)
      return GetInterface((IAMVideoCompression *) this, ppv);

   return CTransformOutputPin::NonDelegatingQueryInterface(riid, ppv);
}

HRESULT CVCMWrapperOutputPin::put_KeyFrameRate(long lKeyFrameRate)
{
   CVCMCompressorWrapper *pVCM = (CVCMCompressorWrapper *) m_pFilter;
   if (pVCM->m_supportsKeyFrames)
   {
      if (lKeyFrameRate == 0)
      {
         pVCM->m_useKeyFrames = false;
         pVCM->m_keyFrameRate = 0;
      }
      else if (lKeyFrameRate < 0)
      {
         pVCM->m_useKeyFrames = true;
         pVCM->m_keyFrameRate = pVCM->m_defaultKeyFrameRate;
      }
      else
      {
         pVCM->m_useKeyFrames = true;
         pVCM->m_keyFrameRate = lKeyFrameRate;
      }
      return S_OK;
   }

   // Doesn't hurt, but doesn't do anything
   return S_FALSE;
}

HRESULT CVCMWrapperOutputPin::get_KeyFrameRate(long *plKeyFrameRate)
{
   CVCMCompressorWrapper *pVCM = (CVCMCompressorWrapper *) m_pFilter;

   if (pVCM->m_supportsKeyFrames)
   {
      *plKeyFrameRate = pVCM->m_keyFrameRate;
   }
   return E_NOTIMPL;
}

HRESULT CVCMWrapperOutputPin::put_PFramesPerKeyFrame(long lPFramesPerKeyFrame)
{
   return E_NOTIMPL;
}

HRESULT CVCMWrapperOutputPin::get_PFramesPerKeyFrame(long *plPFramesPerKeyFrame)
{
   return E_NOTIMPL;
}

HRESULT CVCMWrapperOutputPin::put_Quality(double dQuality)
{
   CVCMCompressorWrapper *pVCM = (CVCMCompressorWrapper *) m_pFilter;
   if (pVCM->m_supportsQuality)
   {
      pVCM->m_quality = dQuality;
      if (pVCM->m_quality < 0)
         pVCM->m_quality = pVCM->m_defaultQuality;
      else if (pVCM->m_quality > 1.0)
         pVCM->m_quality = 1.0;
      return S_OK;
   }

   return E_NOTIMPL;
}

HRESULT CVCMWrapperOutputPin::get_Quality(double *dQuality)
{
   CVCMCompressorWrapper *pVCM = (CVCMCompressorWrapper *) m_pFilter;
   if (pVCM->m_supportsQuality)
   {
      *dQuality = pVCM->m_quality;
      return S_OK;
   }

   return E_NOTIMPL;
}

HRESULT CVCMWrapperOutputPin::put_WindowSize(DWORDLONG dwWindowSize)
{
   return E_NOTIMPL;
}

HRESULT CVCMWrapperOutputPin::get_WindowSize(DWORDLONG *pdwWindowSize)
{
   return E_NOTIMPL;
}

HRESULT CVCMWrapperOutputPin::GetInfo(WCHAR *pszVersion,
                                      int   *pcbVersion,
                                      LPWSTR pszDescription,
                                      int   *pcbDescription,
                                      long  *pDefaultKeyFrameRate,
                                      long  *pDefaultPFramesPerKeyFrame,
                                      double *pDefaultQuality,
                                      long  *pCapabilities)
{
   wcsncpy(pszVersion, L"Version 1.6", *pcbVersion);
   *pcbVersion = wcslen(pszVersion);
   wcsncpy(pszDescription, L"imc VCM Compressor [Don't use]", *pcbDescription);
   *pcbDescription = wcslen(pszDescription);

   CVCMCompressorWrapper *pVCM = (CVCMCompressorWrapper *) m_pFilter;
   if (pVCM->m_hic == NULL)
      return E_FAIL;

   *pDefaultKeyFrameRate = 0;
   DWORD dwFlags = pVCM->m_icInfo.dwFlags;

   if (pVCM->m_supportsKeyFrames)
      *pDefaultKeyFrameRate = pVCM->m_defaultKeyFrameRate;
   else
      *pDefaultKeyFrameRate = 0;

   if (pVCM->m_supportsQuality)
      *pDefaultQuality = pVCM->m_defaultQuality;
   else
      *pDefaultQuality = 0.0;

   *pDefaultPFramesPerKeyFrame = 0; // not supported by VCM codecs

   *pCapabilities = 0;
   if (pVCM->m_supportsQuality)
      *pCapabilities |= CompressionCaps_CanQuality;
   if (pVCM->m_supportsKeyFrames)
      *pCapabilities |= CompressionCaps_CanKeyFrame;

   // disabled for now:
   //if (pVCM->m_supportsDatarate)
   //   *pCapabilities |= CompressionCaps_CanCrunch;

   *pDefaultPFramesPerKeyFrame = 0; // not supported

   return S_OK;
}

HRESULT CVCMWrapperOutputPin::OverrideKeyFrame(long lFrameNumber)
{
   DEBUGOUT("VCM Compressor: OverrideKeyFrame\n");
   return E_NOTIMPL;
}

HRESULT CVCMWrapperOutputPin::OverrideFrameSize(long lFrameNumber, long lSize)
{
   DEBUGOUT("VCM Compressor: OverrideFrameSize\n");
   return E_NOTIMPL;
}
