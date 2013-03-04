#include "stdafx.h"
#include "Video2Flv.h"
#include "DesSource.h"
#include "BmpWriter.h"

#include <initguid.h> // before Guid definition needed

#include "iHDX4FLVMuxer.h"
#include "iHDX4x263codec.h"
#include <mmreg.h>

DEFINE_GUID(CLSID_LadFilter,
            0x67cd5cef, 0xe69b, 0x43f2, 0xac, 0x36, 0x87, 0x44, 0x1e, 0x88, 0x23, 0xe2);

DEFINE_GUID(CLSID_LameMp3,
            0xb8d27088, 0xff5f, 0x4b7c, 0x98, 0xdc, 0x0e, 0x91, 0xa1, 0x69, 0x62, 0x86);

DEFINE_GUID(CLSID_MonogramFLVMuxer, 
			0xc1fb436d, 0x3a1a, 0x4f7b, 0x8d, 0x69, 0xb8, 0xb8, 0xbc, 0x65, 0x27, 0x18);


#include "..\..\lecturnity\common\lresizer\lresizer.h"

#include "DsHelper.h" // for CDsHelper::GetPin() and others

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//bool g_bWarnedOnce = false;
//#define HRCALL(line) line; if (FAILED(hr) && !g_bWarnedOnce) { g_bWarnedOnce = true; ::MessageBox(NULL, _T(#line), NULL, S_OK); }

DWORD g_dwRegisterFlv = 0;


CVideo2Flv::CVideo2Flv()
{
   m_iOutputWidth = 160;
   m_iOutputHeight = 120;
   m_iVideoKBit = 768;
   m_iAudioKBit = 32;
   m_dOutputFps = 0.0;
   m_bIncreaseKeyframes = false;
   m_bDoPadding = false;
   m_bHasEmptyVideo = false;
   m_bForVersion9 = true;

   m_pGraph = NULL;
   m_pEngine = NULL;
   m_rtVideoOffset = 0;
   m_rtAudioOffset = 0;
   m_aClipFiles.SetSize(0, 10);
   m_aClipOffsets.SetSize(0, 10);
   
   m_bConversionAborted = false;
   m_iCurrentProgress = 0;
   
   m_pSourcePinVideo = NULL;
   m_pSourcePinAudio = NULL;

   m_pSourceDialog = NULL;
   
   m_lReferenceCount = 0;
  
   ::CoInitialize(NULL);
}

void CVideo2Flv::RemoveGraph()
{
   if (g_dwRegisterFlv != 0)
       CDsHelper::RemoveFromRot(g_dwRegisterFlv);
   g_dwRegisterFlv = 0;

   // release CComPtr
   m_pSourcePinVideo = NULL;
   m_pSourcePinAudio = NULL;
   m_pEngine = NULL;
   m_pGraph = NULL;

   m_aClipFiles.RemoveAll();
   m_aClipOffsets.RemoveAll();
}

CVideo2Flv::~CVideo2Flv()
{
   RemoveGraph();

   ::CoUninitialize();
}


//private
HRESULT CVideo2Flv::MakeAudioVideoSource(bool bUseAudio, double dAudioLength, bool bDoPadding)
{
   HRESULT hr = S_OK;

   printf("CVideo2Flv::MakeAudioVideoSource() ...\n");

   bool bAnyContentAdded = false;

   CComPtr<IAMTimeline> pTimeline;
   hr = CoCreateInstance(CLSID_AMTimeline, NULL, CLSCTX_INPROC_SERVER, IID_IAMTimeline, (void**)&pTimeline);

   if (SUCCEEDED(hr) && !m_csAudioFile.IsEmpty() && bUseAudio)
   {
      CDesSource srcAudio;
      hr = srcAudio.Init(m_csAudioFile, m_rtAudioOffset);

      if (SUCCEEDED(hr))
         hr = srcAudio.AddTo(pTimeline);

      if (SUCCEEDED(hr))
         dAudioLength = srcAudio.GetOutputLength();

      bAnyContentAdded = true;
   }

   if (SUCCEEDED(hr))
      printf("CVideo2Flv::MakeAudioVideoSource(): Audio step successful.\n");

   // Has a video in any case: can be dummy/black
   if (SUCCEEDED(hr))
   {
      CDesSource srcVideo;

      printf("CVideo2Flv::MakeAudioVideoSource() ... Video step name=%S clips=%d.\n", m_csVideoFile, m_aClipFiles.GetSize());

      // TODO if the video codec is not available (here) there will
      //   be a silent failure and just a message "Step video failed".
      //   -> More meaningful error.
         
      if (!m_csVideoFile.IsEmpty())
      {
         hr = srcVideo.Init(m_csVideoFile, m_rtVideoOffset);
      }
      else if (m_aClipFiles.GetSize() > 0)
      {
         hr = srcVideo.InitClipsOnly();
      }
      else
      {
         // Since 4.0 empty video (for audio only) is allowed.

         hr = srcVideo.InitEmptyVideo();
      }

      if (SUCCEEDED(hr))
         printf("CVideo2Flv::MakeAudioVideoSource(): Video step init ok.\n");

      if (SUCCEEDED(hr) && m_aClipFiles.GetSize() > 0)
      {
         for (int i=0; i<m_aClipFiles.GetSize() && SUCCEEDED(hr); ++i)
         {
            REFERENCE_TIME rtOffset = 0;
            if (i < m_aClipOffsets.GetSize())
               rtOffset = m_aClipOffsets.GetAt(i);
            hr = srcVideo.AddClip(m_aClipFiles.GetAt(i), rtOffset);
         }
      }

      if (SUCCEEDED(hr))
         m_dOutputFps = srcVideo.GetTargetFrameRate();

      double dVideoLength = 0;
      if (SUCCEEDED(hr))
         dVideoLength = srcVideo.GetOutputLength();

      if (SUCCEEDED(hr))
         printf("CVideo2Flv::MakeAudioVideoSource(): Video step variables ok.\n");

      if (SUCCEEDED(hr))
      {
         if (dAudioLength < 0)
            dAudioLength = 0; // 0 means "not set"
         else if (dVideoLength >= dAudioLength)
            dAudioLength = 0; // no correction of video stream length necessary 
         
         int iPaddingWidth = 2, iPaddingHeight = 2;

         if (!m_bForVersion9)
             iPaddingWidth = 8; // for hdx4/h.263

         hr = srcVideo.AddTo(pTimeline, m_iOutputWidth, m_iOutputHeight, dAudioLength, 
             bDoPadding, iPaddingWidth, iPaddingHeight);
      }

      if (SUCCEEDED(hr))
         printf("CVideo2Flv::MakeAudioVideoSource(): Video step adding ok.\n");

      if (SUCCEEDED(hr) && (m_iOutputWidth == 0 || m_iOutputHeight == 0))
      {
         if (m_iOutputWidth == 0)
            m_iOutputWidth = srcVideo.GetOutputWidth();

         if (m_iOutputHeight == 0)
            m_iOutputHeight = srcVideo.GetOutputHeight();
      }

      bAnyContentAdded = true;
   }

   if (SUCCEEDED(hr))
      printf("CVideo2Flv::MakeAudioVideoSource(): Video step successful.\n");

   if (SUCCEEDED(hr))
      hr = CoCreateInstance(CLSID_RenderEngine, NULL, CLSCTX_INPROC_SERVER, IID_IRenderEngine, (void**)&m_pEngine);

   CComPtr<IRenderEngine2> pEngine2;
   if (SUCCEEDED(hr))
   {
      // errors are ignored

      HRESULT hr2 = m_pEngine->QueryInterface(IID_IRenderEngine2, (void **)&pEngine2);
      if (SUCCEEDED(hr2))
         pEngine2->SetResizerGUID(CLSID_StretchDIBitsResizer);
   }

   REFERENCE_TIME rtDuration = 0;
   if (SUCCEEDED(hr))
      hr = pTimeline->GetDuration(&rtDuration);
   double dDuration = (rtDuration / (10*1000)) / 1000.0;

   if (SUCCEEDED(hr))
      hr = m_pEngine->SetTimelineObject(pTimeline);
   if (SUCCEEDED(hr))
      hr = m_pEngine->ConnectFrontEnd();
   if (SUCCEEDED(hr))
      hr = m_pEngine->GetFilterGraph(&m_pGraph);

   if (SUCCEEDED(hr))
      printf("CVideo2Flv::MakeAudioVideoSource(): Engine successful.\n");


   // adding color space converter avoids problems with color format from xvid and lsgc;
   // otherwise you will get a long queue of lresizer filter instances (?)
   CComPtr<IBaseFilter> pColorFilter;
   if (SUCCEEDED(hr))
      hr = CoCreateInstance(CLSID_Colour, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pColorFilter);

   if (SUCCEEDED(hr))
      hr = m_pGraph->AddFilter(pColorFilter, L"RGB Color Converter");

   // create renderers; they will be removed lateron but their connection is reused
   if (SUCCEEDED(hr) && bAnyContentAdded)
      hr = m_pEngine->RenderOutputPins();

   if (SUCCEEDED(hr))
      printf("CVideo2Flv::MakeAudioVideoSource(): Rendering successful.\n");

   if (FAILED(hr))
      printf("CVideo2Flv::MakeAudioVideoSource(): Failed!\n");

   //AddToRotFlv(*ppTargetGraph, &g_dwRegisterFlv);
   //::MessageBox(NULL, _T("with renderers"), NULL, MB_OK);

   return hr;
}

HRESULT CVideo2Flv::SetInputFiles(LPCTSTR tszVideoFile, REFERENCE_TIME rtVideoOffset,
                                  LPCTSTR tszAudioFile, REFERENCE_TIME rtAudioOffset)
{
   if (m_pGraph != NULL)
      RemoveGraph();

   if (tszVideoFile != NULL)
   {
      m_csVideoFile = tszVideoFile;
      m_rtVideoOffset = rtVideoOffset;
   }

   if (tszAudioFile != NULL)
   {
      if (rtAudioOffset != 0)
         return E_NOTIMPL;

      m_csAudioFile = tszAudioFile;
      m_rtAudioOffset = rtAudioOffset;
   }

   return S_OK;
}

HRESULT CVideo2Flv::AddClip(LPCTSTR tszClipFile, REFERENCE_TIME rtClipOffset)
{
   if (m_pGraph != NULL) // must be before PrepareInput() and after SetInputFiles()
      return E_UNEXPECTED;

   if (tszClipFile == NULL)
      return E_POINTER;

   m_aClipFiles.Add(CString(tszClipFile));
   m_aClipOffsets.Add(rtClipOffset);

   return S_OK;
}   

HRESULT CVideo2Flv::SetOutputParams(LPCTSTR tszOutputFile, 
                                    int iOutputWidth, int iOutputHeight, 
                                    int iVideoKBit, int iAudioKBit, 
                                    bool bIncreaseKeyframes, bool bDoPadding, bool bForVersion9)
{
   if (m_pGraph != NULL) // output size is needed for PrepareInput() so this must be called first
      return E_UNEXPECTED;

   if (tszOutputFile == NULL)
      return E_POINTER;

   if (iOutputWidth < 0 || iOutputHeight < 0)
      return E_INVALIDARG;

   if (iVideoKBit < 0 || iAudioKBit < 0)
      return E_INVALIDARG;

   printf("CVideo2Flv::SetOutputParams() %dx%d %d kbit/s %d kbit/s\n", 
       iOutputWidth, iOutputHeight, iVideoKBit, iAudioKBit);


   m_iOutputWidth = iOutputWidth;
   m_iOutputHeight = iOutputHeight;

   if (iVideoKBit > 0)
      m_iVideoKBit = iVideoKBit;

   if (iAudioKBit > 0)
      m_iAudioKBit = iAudioKBit;

   m_bIncreaseKeyframes = bIncreaseKeyframes;
   m_bDoPadding = bDoPadding;
   m_bForVersion9 = bForVersion9;

   m_csOutputFile = tszOutputFile;

   return S_OK;
}
   
HRESULT CVideo2Flv::PrepareInput()
{
   HRESULT hr = S_OK;

   printf("CVideo2Flv::PrepareInput() ...\n");

   m_bHasEmptyVideo = m_csVideoFile.IsEmpty() && m_aClipFiles.GetSize() <= 0;

   LPCTSTR tszVideoFile = NULL, tszAudioFile = NULL;
   REFERENCE_TIME rtVideoOffset = m_rtVideoOffset, rtAudioOffset = m_rtAudioOffset;

   if (m_csVideoFile.GetLength() > 0)
      tszVideoFile = (LPCTSTR)m_csVideoFile;
   if (m_csAudioFile.GetLength() > 0)
      tszAudioFile = (LPCTSTR)m_csAudioFile;

   double dAudioLength = 0;
   if (SUCCEEDED(hr) && tszAudioFile != NULL && _taccess(tszAudioFile, 04) == 0)
   {
      CComPtr<IMediaDet> pMediaDet;
      if (SUCCEEDED(hr))
         hr = ::CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, IID_IMediaDet, (void **) &pMediaDet);

      CComBSTR bstrInputAudio(tszAudioFile);
      if (SUCCEEDED(hr))
         hr = pMediaDet->put_Filename(bstrInputAudio);

      double dLength = -1.0;
      if (SUCCEEDED(hr))
         hr = pMediaDet->get_StreamLength(&dAudioLength);

      //CComBSTR bstrInput2(tszVideoFile);
      //if (SUCCEEDED(hr))
      //   hr = m_pGraph->RenderFile(bstrInput2, NULL);
   }

   if (SUCCEEDED(hr))
       printf("CVideo2Flv::PrepareInput(): Length worked.\n");

   // Note: Audio is added after MakeAudioVideoSource().
   // (Reason is probably because here/later the LAD parser can be added first and thus
   //  adding audio is faster or possible at all. MakeAudioVideoSource() operates with DES...)

   bool bUseAudio = false;
   if (SUCCEEDED(hr))
      hr = MakeAudioVideoSource(bUseAudio, dAudioLength, m_bDoPadding);
   // Also creates the m_pGraph!

   if (SUCCEEDED(hr))
      printf("CVideo2Flv::PrepareInput(): Make audio video worked.\n");
 
   if (SUCCEEDED(hr) && tszAudioFile != NULL && _taccess(tszAudioFile, 04) == 0)
   {
      CComPtr<IBaseFilter> pLadParser;
      if (SUCCEEDED(hr))
         hr = CoCreateInstance(CLSID_LadFilter, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pLadParser);

      if (SUCCEEDED(hr))
         hr = m_pGraph->AddFilter(pLadParser, L"LAD Parser");

      CComBSTR bstrInputAudio(tszAudioFile);
      if (SUCCEEDED(hr))
      {
         hr = m_pGraph->RenderFile(bstrInputAudio, NULL);

         if (FAILED(hr))
            ::MessageBox(NULL, _T("Unexpected: Cannot render audio to default device. Does not exist?"), NULL, MB_OK);
      }
   }

   if (SUCCEEDED(hr))
      printf("CVideo2Flv::PrepareInput(): Adding audio worked.\n");

	//Connect current DirectShow graph to the ROT.
   if (SUCCEEDED(hr) && g_dwRegisterFlv == 0)
       CDsHelper::AddToRot(m_pGraph, &g_dwRegisterFlv);

   if (SUCCEEDED(hr))
       hr = CDsHelper::RemoveRenderersSaveConnections(m_pGraph, &m_pSourcePinAudio, &m_pSourcePinVideo);

   if (FAILED(hr))
      printf("CVideo2Flv::PrepareInput(): Failed!\n");

   return hr;
}


HRESULT InspectType(IPin *pPin, AM_MEDIA_TYPE **ppTargetType = NULL)
{
   if (pPin == NULL)
      return E_POINTER;

   HRESULT hr = S_OK;
   
   CComPtr<IEnumMediaTypes> pEnumTypes;
   if (SUCCEEDED(hr))
      hr = pPin->EnumMediaTypes(&pEnumTypes);

   AM_MEDIA_TYPE *pmtPin = NULL;
   ULONG lFetched = 0;
   if (SUCCEEDED(hr))
      hr = pEnumTypes->Next(1, &pmtPin, &lFetched);

   if (SUCCEEDED(hr) && hr == S_OK) // S_FALSE means not that many (1) output types
   {
      if (pmtPin->majortype == MEDIATYPE_Audio)
      {
         if (pmtPin->subtype== MEDIASUBTYPE_PCM  
            &&	pmtPin->formattype == FORMAT_WaveFormatEx)
         {
            WAVEFORMATEX *pAudioFormat = (WAVEFORMATEX *)pmtPin->pbFormat;
         }
      }
      else if (pmtPin->majortype == MEDIATYPE_Video)
      {
         if ((pmtPin->formattype == FORMAT_VideoInfo))
         {
            VIDEOINFOHEADER *pVideoFormat = (VIDEOINFOHEADER *)pmtPin->pbFormat;
            int iDummy =0;
         }
         if ((pmtPin->formattype == FORMAT_VideoInfo2))
         {
            VIDEOINFOHEADER2 *pVideoFormat = (VIDEOINFOHEADER2 *)pmtPin->pbFormat;
            int iDummy =0;
         }
      }

      if (ppTargetType == NULL)
         DeleteMediaType(pmtPin);
      else
         *ppTargetType = pmtPin;
   }

   return hr;
}

HRESULT CVideo2Flv::BufferCB(double dSampleTime, BYTE *pBuffer, long lBufferLen)
{
   //TRACE("BufferCB for time %f\n", dSampleTime);

   HRESULT hr = S_OK;
   CString csOutBmp = m_csOutputFile;
   csOutBmp += _T("_13409838600.bmp");
   if (SUCCEEDED(hr))
      hr = CBmpWriter::WriteBmpImage(pBuffer, lBufferLen, m_iOutputWidth, m_iOutputHeight, csOutBmp);

   ::DeleteFileW(csOutBmp);

   return hr;
}

HRESULT CVideo2Flv::SampleCB(double dSampleTime, IMediaSample *pSample)
{
   return S_FALSE;
}

// static
HRESULT CVideo2Flv::MakeMp3Target(IGraphBuilder *pGraph, IPin *pSourcePin, IBaseFilter *pMux, 
                                  int iAudioKBit, HWND hwndParent, bool bUseMonogramMux)
{
   HRESULT hr = S_OK;

   if (pGraph == NULL || pSourcePin == NULL || pMux == NULL)
      return E_POINTER;

   // enumerate the Types/Formats of the Audio Source
   CMediaType mtOutput;
   CComPtr<IEnumMediaTypes> pMTEnum;
   if (SUCCEEDED(hr))
      hr = pSourcePin->EnumMediaTypes(&pMTEnum);

   // try to find a valid media type
   bool bFound8khz = false;
   AM_MEDIA_TYPE* pMType = NULL;
   while(SUCCEEDED(hr) && (hr = pMTEnum->Next(1, &pMType, NULL)) == S_OK)
   {
      if(!pMType)
         continue;

      if ((pMType->majortype == MEDIATYPE_Audio) 
         &&	(pMType->subtype == MEDIASUBTYPE_PCM)			
         &&	(pMType->formattype == FORMAT_WaveFormatEx))
      {
         WAVEFORMATEX* pTestFormat = (WAVEFORMATEX *)pMType->pbFormat;

         //	Check input sample rate.
         if(pTestFormat->nSamplesPerSec == 44100 
            ||	pTestFormat->nSamplesPerSec == 22050
            || pTestFormat->nSamplesPerSec == 11025
            //||	pwfmt->nSamplesPerSec == 5512 //Not unsupported in MP3 
            )
         {
            mtOutput.Set(*pMType);
            DeleteMediaType(pMType), pMType = NULL;
            break;
         }
         else if (pTestFormat->nSamplesPerSec == 16000
            || pTestFormat->nSamplesPerSec == 8000)
         {
            bFound8khz = true;
         }
      }
      DeleteMediaType(pMType), pMType = NULL;
   }

   if (SUCCEEDED(hr) && !mtOutput.IsValid() && bFound8khz)
      hr = VFW_E_TYPE_NOT_ACCEPTED; // 8 and 16 khz not supported by FLV

   // check media type
   if(SUCCEEDED(hr) && !mtOutput.IsValid())
   {
      ::MessageBox(hwndParent, _T("Unexpected: No proper audio information could be found."), NULL, MB_OK);
      hr = E_FAIL;
   }

   // get media type as WAVEFORMATEX
   WAVEFORMATEX* pWaveFormat = NULL;
   if (SUCCEEDED(hr))
      pWaveFormat = (WAVEFORMATEX *)mtOutput.pbFormat;

   CComPtr<IBaseFilter> pMp3Encoder;
   if (SUCCEEDED(hr))
       hr = CDsHelper::CreateLameMp3Filter(pMp3Encoder, iAudioKBit, pWaveFormat->nSamplesPerSec);

   // add MP3 Encoder to the graph
   if (SUCCEEDED(hr))
      hr = pGraph->AddFilter(pMp3Encoder, L"MP3 Encoder");


   CComPtr<IPin> pMP3EncoderInPin;
   CComPtr<IPin> pMP3EncoderOutPin;

   // get MP3 Encoder input-pin
   if (SUCCEEDED(hr))
      hr = CDsHelper::GetPin(pMP3EncoderInPin, pMp3Encoder, PINDIR_INPUT, 0);

   // connect source audio-pin with MP3 Encoder input-pin
   if (SUCCEEDED(hr))
   {
      hr = pGraph->ConnectDirect(pSourcePin, pMP3EncoderInPin, &mtOutput);

      if (FAILED(hr))
         ::MessageBox(hwndParent, _T("Unexpected: No connection to audio encoder."), NULL, MB_OK);
   }

   // Bugfix 5940 ("Redo" of Bugfix 5331): Monogram FLV mux is required for A/V synchronity
   if (bUseMonogramMux && SUCCEEDED(hr)) {
      // With the Monogram FLV mux
      // we need to insert a (null rendered) avi mux to get a proper position.
      // This works best/or at all with uncompressed audio data (this location here).

      CComPtr<IPin> pTeeOutPin2;
      if (SUCCEEDED(hr))
          hr = CDsHelper::InsertTeeAndConnect(pGraph, pSourcePin, pMP3EncoderInPin, pTeeOutPin2);

      if (SUCCEEDED(hr))
          hr = CDsHelper::ConnectEmptyAviMux(pGraph, pTeeOutPin2);
     
      if (FAILED(hr))
         ::MessageBox(hwndParent, _T("Unexpected: No connection for empty tracking mux."), NULL, MB_OK);

   }


   // get MP3 Encoder output-pin
   if (SUCCEEDED(hr))
      hr = CDsHelper::GetPin(pMP3EncoderOutPin, pMp3Encoder, PINDIR_OUTPUT, 0);

   //Find output media type with same sample rate as input
   pMTEnum.Release();

   if (SUCCEEDED(hr))
      hr = pMP3EncoderOutPin->EnumMediaTypes(&pMTEnum);

   UINT nAudioBytesPerSec = (UINT)(iAudioKBit * 1000) / 8;

   pMType = NULL;
   while(SUCCEEDED(hr) && (hr = pMTEnum->Next(1,&pMType,NULL ))== S_OK)
   {
       // This makes sure that the pin connection has the right type.
       // However the lame encoder must be configured specifically (see above).

      if(!pMType)
      {
         continue;
      }

      if(pMType->majortype == MEDIATYPE_Audio && pMType->formattype == FORMAT_WaveFormatEx)
      {
         MPEGLAYER3WAVEFORMAT* pMp3Format = (MPEGLAYER3WAVEFORMAT *)pMType->pbFormat;

         if(pMp3Format->wfx.nSamplesPerSec == pWaveFormat->nSamplesPerSec 
            && pMp3Format->wfx.nChannels == pWaveFormat->nChannels )
            //&& (pMp3Format->wfx.nAvgBytesPerSec > nAudioBytesPerSec * 0.94 
            // && pMp3Format->wfx.nAvgBytesPerSec < nAudioBytesPerSec * 1.06))
         {
            pMp3Format->wfx.nAvgBytesPerSec = nAudioBytesPerSec;
            break;
         }
      }
      DeleteMediaType(pMType), pMType = NULL;
   }

   if (SUCCEEDED(hr) && pMType == NULL)
   {
      hr = E_FAIL;
      ::MessageBox(hwndParent, _T("Unexpected: No proper audio compression format could be found."), NULL, MB_OK);
   }

   CComPtr<IPin> pFLVMuxerInPin2;

   // find empty Flash Muxer input-pin; it has to be the second one/video is required (by our FLV mux)
   if (SUCCEEDED(hr))
       hr = CDsHelper::GetPin(pFLVMuxerInPin2, pMux, PINDIR_INPUT, 1);

   // connect MP3 Encoder output-pin with Flash Muxer input-pin
   if (SUCCEEDED(hr))
   {
       hr = pGraph->ConnectDirect(pMP3EncoderOutPin, pFLVMuxerInPin2, pMType);
       if(FAILED(hr))
           hr = pGraph->Connect(pMP3EncoderOutPin, pFLVMuxerInPin2);        

       if (FAILED(hr))
           ::MessageBox(hwndParent, _T("Unexpected: No audio connection to muxer."), NULL, MB_OK);
   }

   //::MessageBox(hwndParent, _T("Ready."), NULL, MB_OK);

   DeleteMediaType(pMType), pMType = NULL;

   return hr;
}

HRESULT CVideo2Flv::PrepareConversion(CDialog *pDlg)
{
   HRESULT hr = S_OK;
 
   if (m_csOutputFile.IsEmpty())
      return E_UNEXPECTED;

   if (m_pGraph == NULL) // || m_pSourcePinVideo == NULL)
      return E_UNEXPECTED;

   if (m_iVideoKBit <= 0 || m_iAudioKBit < 0)
      return E_FAIL;

   // BUGFIX #3839: Check if the file is blocked by the browser.
   // pFileSink->SetFileName() below will not yield an error in that case.
   if (!CheckOverwrite(m_csOutputFile))
      return E_ACCESSDENIED;

   m_pSourceDialog = pDlg;  

   HWND hwndParent = NULL;
   if (pDlg != NULL)
      hwndParent = pDlg->m_hWnd;

    // Bugfix 5940 ("Redo" of Bugfix 5331): Monogram FLV mux is required for A/V synchronity
    // TODO bUseMonogramMuxer can be removed (and the hdx4 filter dll) if monogram muxer works
    bool bUseMonogramMuxer = true;
	CComPtr<IBaseFilter> pFLVMuxer;
    if (SUCCEEDED(hr)) {
	   hr = CoCreateInstance(CLSID_MonogramFLVMuxer, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pFLVMuxer);

       // Note: Using the old mux with the x264 video codec will not properly connect (endless loop).

       if (FAILED(hr)) {
           pFLVMuxer = NULL;
           hr = CoCreateInstance(CLSID_HDX4FLVMuxer, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pFLVMuxer);
           bUseMonogramMuxer = false;
       }
    }

	// add Flash Muxer to the graph
	if (SUCCEEDED(hr))
	   hr = m_pGraph->AddFilter(pFLVMuxer, L"Flash Video Muxer");

    CComPtr<IBaseFilter> pFLVWriter;
    if (bUseMonogramMuxer) {
        if (SUCCEEDED(hr))
            hr = CoCreateInstance(CLSID_FileWriter, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pFLVWriter);

        if (SUCCEEDED(hr))
            hr = m_pGraph->AddFilter(pFLVWriter, L"File writer");
    }

    CComPtr<IFileSinkFilter2> pFileSink;
    if (SUCCEEDED(hr)) {
        if (bUseMonogramMuxer) {
            hr = pFLVWriter->QueryInterface(IID_IFileSinkFilter2, (void**)&pFileSink);
        } else {
            hr = pFLVMuxer->QueryInterface(IID_IFileSinkFilter2, (void**)&pFileSink);
        }
    }

	// set output filename
   if (SUCCEEDED(hr))
	   hr = pFileSink->SetFileName(_bstr_t(m_csOutputFile), NULL);

   if (bUseMonogramMuxer && SUCCEEDED(hr))
       hr = CDsHelper::ConnectFiltersSimple(m_pGraph, pFLVMuxer, pFLVWriter);


	// get Flash Muxer input-pin
	CComPtr<IPin> pFLVMuxerInPin;
	if (SUCCEEDED(hr))
      hr = CDsHelper::GetPin(pFLVMuxerInPin, pFLVMuxer, PINDIR_INPUT, 0);

   if (FAILED(hr))
      ::MessageBox(hwndParent, _T("Unexpected: Muxer did not work."), NULL, MB_OK);

   //InspectType(pFLVMuxerInPin);

   if (m_pSourcePinVideo != NULL && SUCCEEDED(hr))
   {
      bool bDoPadding = false;

      CComPtr<IBaseFilter> pFLVEncoder;

      hr = CoCreateInstance(CLSID_HDX4x263Encoder, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pFLVEncoder);


      // add Flash Encoder to the graph
      if (SUCCEEDED(hr))
         hr = m_pGraph->AddFilter(pFLVEncoder, L"Video Codec");

      // get Flash Encoder input-pin
      CComPtr<IPin> pFLVEncoderInPin;
      CComPtr<IPin> pFLVEncoderOutPin;
      if (SUCCEEDED(hr))
         hr = CDsHelper::GetPin(pFLVEncoderInPin, pFLVEncoder, PINDIR_INPUT, 0);


      // connect source video-pin with encoder input-pin
      if (SUCCEEDED(hr))
      {
          if (SUCCEEDED(hr))
              hr = m_pGraph->Connect(m_pSourcePinVideo, pFLVEncoderInPin);

          bool bUsesAudio = m_pSourcePinAudio != NULL;
          if (bUseMonogramMuxer && !bUsesAudio) {
              // If there is no audio (ohter location) but the Monogram muxer is used
              // there must still be an empty avi mux for the position.

              CComPtr<IPin> pTeeOutPin2;
              if (SUCCEEDED(hr))
                  hr = CDsHelper::InsertTeeAndConnect(m_pGraph, m_pSourcePinVideo, pFLVEncoderInPin, pTeeOutPin2);

              if (SUCCEEDED(hr))
                  hr = CDsHelper::ConnectEmptyAviMux(m_pGraph, pTeeOutPin2);
        }
         
         if (FAILED(hr))
            ::MessageBox(hwndParent, _T("Unexpected: No connection to video encoder."), NULL, MB_OK);
      }

      // get Flash Encoder output-pin
      if (SUCCEEDED(hr))
         hr = CDsHelper::GetPin(pFLVEncoderOutPin, pFLVEncoder, PINDIR_OUTPUT, 0);

      
      int iMaxGop = 30; // every one second
      if (m_dOutputFps > 0 && !m_bHasEmptyVideo) {
          bool bUsesAudio = m_pSourcePinAudio != NULL;
          // !bUsesAudio means that it is a separate clip: allow greater keyframe distance

          if (bUsesAudio || m_bIncreaseKeyframes)
              iMaxGop = (int)(m_dOutputFps);
          else
              iMaxGop = (int)(m_dOutputFps * 3);

          if (iMaxGop <= 0)
              iMaxGop = 1;
      } else if (m_bHasEmptyVideo) {
          iMaxGop = 1; // image black anyway; improve scrolling?
      }

      // get Flash encoder interface
      CComPtr<IHDX4x263EncoderConfig> pEncoderConfig;
      if (SUCCEEDED(hr))
          hr = pFLVEncoder->QueryInterface(IID_IHDX4x263EncoderConfig, (void**)&pEncoderConfig);


      // get encoder settings as ByteString (from Helper Class)
      WCHAR wszEncoderConfig[1024];
      wsprintf(wszEncoderConfig, L"<HDX4x2FLVEncoderDoc\
                                  dBitrateVariance=\"0.5\"\
                                  iBitrate=\"%d\"\
                                  iMaxGOPInterval=\"%d\"\
                                  iMinGOPInterval=\"1\"\
                                  iQuality=\"100\"\
                                  iVBVMaxBitrate=\"0\"\
                                  iVBVMinBitrate=\"0\"\
                                  sEncodingMode=\"onepass_abr\"\
                                  sTempFileName=\"c:\\hdx4flvencoder.dat\"\
                                  sMotionEstimationMethod=\"normal\"/>", m_iVideoKBit, iMaxGop);

      CComBSTR bstrEncoderSettings(wszEncoderConfig);


      // set encoder settings (ByteString)
      if (SUCCEEDED(hr))
      {
          hr = pEncoderConfig->SetXMLSettings(bstrEncoderSettings);

          if (FAILED(hr))
          {
              ::MessageBox(hwndParent, _T("Unexpected: Video settings cannot be set. Continuing..."), NULL, MB_OK);
              hr = S_OK;
          }
      }

      //InspectType(pFLVEncoderOutPin);

      // connect Flash Encoder output-pin with Flash Muxer input pin
      if (SUCCEEDED(hr))
      {
         hr = m_pGraph->Connect(pFLVEncoderOutPin, pFLVMuxerInPin);
            
         if (FAILED(hr))
            ::MessageBox(hwndParent, _T("Unexpected: No video connection to muxer."), NULL, MB_OK);
      }
   }

   if(m_pSourcePinAudio != NULL && SUCCEEDED(hr))
   {
      hr = MakeMp3Target(m_pGraph, m_pSourcePinAudio, pFLVMuxer, m_iAudioKBit, hwndParent, bUseMonogramMuxer);
   }

#ifdef _DEBUG
   //::MessageBox(hwndParent, _T("Ready to inspect."), NULL, MB_OK);
#endif

   return hr;
}

HRESULT CVideo2Flv::PrepareConversionAvi(CDialog *pDlg)
{
   HRESULT hr = S_OK;

   m_pSourceDialog = pDlg;

   CComPtr<ICaptureGraphBuilder2> pBuilder2;
   if (SUCCEEDED(hr))
      hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, 0, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pBuilder2);

   if (SUCCEEDED(hr))
      hr = pBuilder2->SetFiltergraph(m_pGraph);

   CString csOutputAvi = m_csOutputFile;
   csOutputAvi += _T(".avi");

   CComPtr<IBaseFilter> pMux;
   if (SUCCEEDED(hr))
      hr = pBuilder2->SetOutputFileName(&MEDIASUBTYPE_Avi, _bstr_t(csOutputAvi), &pMux, NULL);

   if (m_pSourcePinVideo != NULL && SUCCEEDED(hr))
   {
      CComPtr<IPin> pMuxVideoIn;
      if (SUCCEEDED(hr))
         hr = CDsHelper::GetPin(pMuxVideoIn, pMux, PINDIR_INPUT, 0);

      if (SUCCEEDED(hr))
         hr = m_pGraph->Connect(m_pSourcePinVideo, pMuxVideoIn);

      /*
      if (m_pSourcePinAudio != NULL && SUCCEEDED(hr))
      {
         CComPtr<IPin> pMuxAudioIn;
         if (SUCCEEDED(hr))
            hr = CDsHelper::GetPin(pMuxAudioIn, pMux, PINDIR_INPUT, 1);

         if (SUCCEEDED(hr))
            hr = m_pGraph->Connect(m_pSourcePinAudio, pMuxAudioIn);
      }
      */
   }

   if(m_pSourcePinAudio != NULL && SUCCEEDED(hr))
   {
      hr = MakeMp3Target(m_pGraph, m_pSourcePinAudio, pMux, m_iAudioKBit, NULL, false);
   }

   return hr;
}

// static
DWORD WINAPI CVideo2Flv::ConversionLauncher(void *pParameter)
{
   if (pParameter != NULL)
   {
      HRESULT hr = ((CVideo2Flv *)pParameter)->ConversionThread();
      if (SUCCEEDED(hr))
         return 0;
      else
         return hr;
   }
   return
      E_FAIL;
}

HRESULT CVideo2Flv::StartConversion()
{
   if (m_pGraph == NULL)
      return E_FAIL;

   HANDLE hThread = ::CreateThread(NULL, 0, ConversionLauncher, this, CREATE_SUSPENDED, NULL);
   if (hThread != NULL)
      ::ResumeThread(hThread);
   else
      return E_FAIL;
   
   return S_OK;
}

HRESULT CVideo2Flv::AbortConversion()
{
   HRESULT hr = S_OK;

   if (m_pGraph == NULL)
      return S_FALSE;

   CComPtr<IMediaControl> pControl;
   if (SUCCEEDED(hr))
      hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
   
   m_bConversionAborted = true;

   if (SUCCEEDED(hr))
      hr = pControl->Stop();

   return hr;
}

HRESULT CVideo2Flv::CheckForFilters()
{
   HRESULT hr = S_OK;

   CComPtr<IBaseFilter> pMp3Encoder;
   if (SUCCEEDED(hr))
      hr = CoCreateInstance(CLSID_LameMp3, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pMp3Encoder);

   CComPtr<IBaseFilter> pFLVEncoder;
   if (SUCCEEDED(hr))
      hr = CoCreateInstance(CLSID_HDX4x263Encoder, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pFLVEncoder);

   CComPtr<IBaseFilter> pFLVMuxer;
   if (SUCCEEDED(hr))
	   hr = CoCreateInstance(CLSID_HDX4FLVMuxer, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pFLVMuxer);

   return hr;
}

// private
HRESULT CVideo2Flv::ConversionThread()
{
   HRESULT hr = S_OK;

   HWND hwndParent = NULL;
   if (m_pSourceDialog != NULL)
      hwndParent = m_pSourceDialog->m_hWnd;

   if (m_pGraph == NULL)
      return E_FAIL;

   CComPtr<IMediaControl> pControl;
   CComPtr<IMediaEvent>   pEvent;
   CComPtr<IMediaSeeking> pSeeking;
   CComPtr<IMediaFilter>  pFilter;
   if (SUCCEEDED(hr))
      hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
   if (SUCCEEDED(hr))
      hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
   if (SUCCEEDED(hr))
      hr = m_pGraph->QueryInterface(IID_IMediaSeeking, (void **)&pSeeking);
   if (SUCCEEDED(hr))
      hr = m_pGraph->QueryInterface(IID_IMediaFilter, (void **)&pFilter);
   
   // should be default; just to make sure
   if (SUCCEEDED(hr))
      hr = pSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME); 

   if (SUCCEEDED(hr))
      pFilter->SetSyncSource(NULL); 
   // otherwise you'll get the system/running time from GetCurrentPosition() below

   if (SUCCEEDED(hr))
      hr = pControl->Run();
   
   double dLengthSeconds = 0;
   bool bPositionObtainable = false;
   if (SUCCEEDED(hr))
   {
      REFERENCE_TIME tCurrent;
      HRESULT hrPos = pSeeking->GetCurrentPosition(&tCurrent);

      if (SUCCEEDED(hrPos))
         bPositionObtainable = true;

      if (SUCCEEDED(hrPos))
      {
         REFERENCE_TIME rtDuration;
         hrPos = pSeeking->GetDuration(&rtDuration);

         if (SUCCEEDED(hrPos))
            dLengthSeconds = rtDuration/(10*1000*1000.0);
      }
   }

   if (SUCCEEDED(hr) && bPositionObtainable)
   {
      DWORD dwStart = ::timeGetTime();

      while (SUCCEEDED(hr))
      {
         long lEventCode = 0;
         LONG_PTR lParam1 = NULL;
         LONG_PTR lParam2 = NULL;

         HRESULT hrEvent = pEvent->GetEvent(&lEventCode, &lParam1, &lParam2, 500);

         if (hrEvent == S_OK)
         {
            // no timeout: a true event received

            if (lEventCode == EC_ERRORABORT)
               hr = (HRESULT)lParam1;
            else if (lEventCode == EC_COMPLETE)
               break;
            //else
            //   TRACE("Unknown event code received: %d\n", lEventCode);
         }
         else
         {
            // normal operation: there is no special event

            REFERENCE_TIME rtCurrent;
            if (SUCCEEDED(hr))
               hr = pSeeking->GetCurrentPosition(&rtCurrent);

            if (SUCCEEDED(hr))
            {
               double dCurrentSeconds = rtCurrent/(10*1000*1000.0);

               //TRACE("%f / %f\n", dCurrentSeconds, dLengthSeconds);

               if (dCurrentSeconds == 0)
               {
                  DWORD dwCurrent = ::timeGetTime();

                  if (dwCurrent - dwStart > 20000)
                  {
                     m_bConversionAborted = true;
                     break;
                  }
               }

               if (dCurrentSeconds >= dLengthSeconds)
                  break;
               else
               {
                  double dCurrentProgress = dCurrentSeconds/dLengthSeconds;
                  m_iCurrentProgress = (int)(dCurrentProgress * 10000);

                  if (m_pSourceDialog != NULL && !m_bConversionAborted)
                     m_pSourceDialog->PostMessage(WM_COMMAND, 0, (long)this);

#ifdef _DEBUG
                  _tprintf(_T("\r%f"), dCurrentProgress);
#endif

                  // ::Sleep(500); // done with GetEvent() above
               }
            }
         }

         pEvent->FreeEventParams(lEventCode, lParam1, lParam2);
      }
   }
   else
   {
      if (SUCCEEDED(hr)) // !bPositionObtainable
      {
         ::MessageBox(hwndParent, _T("Unexpected: Cannot obtain position."), NULL, MB_OK);

         long evCode;
         pEvent->WaitForCompletion(600000, &evCode);
      
         // Note: Do not use INFINITE in a real application, because it
         // can block indefinitely.
      }
      else
      {
         _TCHAR tszMsg[300];
         _stprintf(tszMsg, _T("Unexpected: Cannot start graph: %x"), hr);
         ::MessageBox(hwndParent, tszMsg, NULL, MB_OK);
      }
   }
   
   if (SUCCEEDED(hr))
      hr = pControl->Stop(); // finish the file


   // Fix an (old; not existing anymore?) bug with a missing length field in the file.
   // This missing length field only disturbed the Flash streaming server.
   if (_taccess(m_csOutputFile, 02) == 0)
   {
       // output file exists and is writable

       HANDLE hFile = ::CreateFile(m_csOutputFile, GENERIC_READ | GENERIC_WRITE, 0, 
           NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

       if (hFile != INVALID_HANDLE_VALUE)
       {
           DWORD dwBytes = 0;
           BYTE aBuffer[3];
           DWORD dwMove = ::SetFilePointer(hFile, 14, NULL, FILE_BEGIN);

           BOOL bSuccess = ::ReadFile(hFile, aBuffer, 3, &dwBytes, NULL); // metadata length

           if (bSuccess)
           {
               int nMetaLength = (aBuffer[0] << 16) & 0xff0000 |
                   (aBuffer[1] << 8) & 0xff00 | (aBuffer[2]) & 0xff;

               int iLengthBytePos = nMetaLength + 25;

               ::SetFilePointer(hFile, iLengthBytePos, NULL, FILE_BEGIN);

               ::ReadFile(hFile, aBuffer, 3, &dwBytes, NULL); // metadata length

               if (aBuffer[0] == 0 && aBuffer[1] == 0 && aBuffer[2] == 0)
               {
                   // correct the (trailing) length of the metadata tag of hdx4
                   // it should not be 0

                   int nChunkLength = nMetaLength + 11;

                   aBuffer[0] = (nChunkLength >> 16) & 0xff;
                   aBuffer[1] = (nChunkLength >> 8) & 0xff;
                   aBuffer[2] = (nChunkLength) & 0xff;

                   ::SetFilePointer(hFile, iLengthBytePos, NULL, FILE_BEGIN);
                   ::WriteFile(hFile, aBuffer, 3, &dwBytes, NULL);
               }

               ::CloseHandle(hFile);
           }
           else
           {
               DWORD dwError = ::GetLastError();
               ::MessageBox(hwndParent, _T("Unexpected: Cannot access output file for correction."), NULL, MB_OK);
           }
       }
   }

   if (SUCCEEDED(hr))
      m_iCurrentProgress = 10000; // file finished
   else
      m_iCurrentProgress = hr; // signal error

   // post finish message
   if (m_pSourceDialog != NULL && !m_bConversionAborted)
      m_pSourceDialog->PostMessage(WM_COMMAND, 0, (long)this);
   // TODO what to do if m_pSourceDialog is invalid (deleted)


   return hr;
}

// static 
bool CVideo2Flv::CheckOverwrite(LPCTSTR tszFilename)
{
   if (tszFilename == NULL)
      return false;

   HANDLE hOverwrite = ::CreateFile(tszFilename, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hOverwrite == INVALID_HANDLE_VALUE)
      return false;
   else
      ::CloseHandle(hOverwrite);

   return true;
}

/*
HRESULT CVideo2Flv::CheckOut(LPCTSTR tszAviFile, CDialog *pDlg)
{
   // check for output file
   if (SUCCEEDED(hr))
   {
      bool bFileExists = false;
      bool bFileWriteable = false;

      bFileExists = _taccess(tszOutputFilename, 00) == 0;
      if (bFileExists)
         bFileWriteable = _taccess(tszOutputFilename, 02) == 0;

      if (bFileExists)
      {
         if (bFileWriteable)
         {
            int iResult = ::MessageBox(
               pDlg->m_hWnd, _T("Output file exists. Do you want to overwrite it?"),
               _T("Question"), MB_YESNO);
            
            if (iResult == IDNO)
               return S_FALSE;
         }
         else
         {
            ::MessageBox(
               pDlg->m_hWnd, _T("Output file exists but cannot be overwritten."),
               _T("Error"), MB_OK | MB_ICONEXCLAMATION);

            return E_FAIL;
         }
      }
   }

}
*/

/*
// private
HRESULT CVideo2Flv::FindFilter(bool bAudio, LPCTSTR tszCodecName, IBaseFilter **ppTargetFilter)
{
   HRESULT hr = S_OK;

   if (tszCodecName == NULL || ppTargetFilter == NULL)
      return E_POINTER;

   if (_tcslen(tszCodecName) < 1)
      return E_INVALIDARG;
     
   CComBSTR bstrCodecName(tszCodecName);

   if (SUCCEEDED(hr))
   {
      CComPtr<ICreateDevEnum> pSysDevEnum;
      CComPtr<IEnumMoniker> pEnum;
      CComPtr<IMoniker> pMoniker;
      
      hr = ::CoCreateInstance(CLSID_SystemDeviceEnum, NULL, 
         CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pSysDevEnum);
      
      GUID guidCategory = bAudio ? CLSID_AudioCompressorCategory : CLSID_VideoCompressorCategory;
      if (SUCCEEDED(hr))
         hr = pSysDevEnum->CreateClassEnumerator(guidCategory, &pEnum, 0);

      if (hr == S_OK)  // S_FALSE means nothing in this category.
      {
         while (S_OK == pEnum->Next(1, &pMoniker, NULL))
         {
            CComPtr<IPropertyBag> pPropBag;
            pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
            VARIANT var;
            VariantInit(&var);
            hr = pPropBag->Read(L"FriendlyName", &var, 0);
            if (SUCCEEDED(hr))
            {
               if (wcsstr(var.bstrVal, bstrCodecName) != NULL)
               {
                  hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)ppTargetFilter);
                  
                  if (SUCCEEDED(hr))
                     break;
                  
               }
            }   
            VariantClear(&var);
            
            pMoniker = NULL; // release for the next loop
         }
      }
   }

   return hr;
}
*/