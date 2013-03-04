#include "stdafx.h"

#include  <io.h> // _access

#include "Lrd2VideoConverter.h"

#include "..\..\common\ladfilter\ladfilter.h"
#include "imc_lecturnity_converter_LPLibs.h" // for error return code
#include "DsHelper.h" // for CDsHelper::GetPin() and others

#include "ILMDSKernel2.h"
#include "ILEncAAC.h"
#include "ILEncMPEG4.h"
#include "ILMIsoMux.h"

// Mainconcept header files for h.264/aac
#include "main_enc_avc_guid.h"
#include "main_enc_aac_guid.h"
#include "main_mux_mp4_guid.h"
#include "main_ModuleConfig.h"
#include "main_common_mc.h"
#include "main_enc_avc_mc.h"
#include "main_enc_aac_mc.h"
#include "main_mux_mp4_mc.h"

//DEFINE_GUID(CLSID_ElecardMux,
//            0xdf696702, 0xd81b, 0x4250, 0xb0, 0x8f, 0x55, 0x56, 0x60, 0x30, 0x98, 0x49);

//DEFINE_GUID(CLSID_RgbToYv12Filter,
//            0x94B33A11, 0x866F, 0x468d, 0xA3, 0x60, 0xD9, 0xA3, 0x13, 0xE2, 0xBF, 0x09);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
   IMoniker * pMoniker;
   IRunningObjectTable *pROT;
   if (FAILED(GetRunningObjectTable(0, &pROT))) {
      return E_FAIL;
   }
   WCHAR wsz[256];
   wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
   HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
   if (SUCCEEDED(hr)) {
      hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
      pMoniker->Release();
   }
   pROT->Release();
   return hr;
}

void RemoveFromRot(DWORD pdwRegister)
{
   IRunningObjectTable *pROT;
   if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
      pROT->Revoke(pdwRegister);
      pROT->Release();
   }
}

DWORD g_dwRegister;

HRESULT SaveGraphFile(IGraphBuilder *pGraph, WCHAR *wszPath) 
{
    const WCHAR wszStreamName[] = L"ActiveMovieGraph"; 
    HRESULT hr;
    
    IStorage *pStorage = NULL;
    hr = StgCreateDocfile(
        wszPath,
        STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
        0, &pStorage);
    if(FAILED(hr)) 
    {
        return hr;
    }

    IStream *pStream;
    hr = pStorage->CreateStream(
        wszStreamName,
        STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
        0, 0, &pStream);
    if (FAILED(hr)) 
    {
        pStorage->Release();    
        return hr;
    }

    IPersistStream *pPersist = NULL;
    pGraph->QueryInterface(IID_IPersistStream, (void**)&pPersist);
    hr = pPersist->Save(pStream, TRUE);
    pStream->Release();
    pPersist->Release();
    if (SUCCEEDED(hr)) 
    {
        hr = pStorage->Commit(STGC_DEFAULT);
    }
    pStorage->Release();
    return hr;
}


CLrd2VideoConverter::CLrd2VideoConverter()
{
   m_pLrdFile = NULL;
   m_dAudioSeconds = 0;
   m_bConversionAborted = false;
   m_iCurrentProgress = 0;
   m_bConversionActive = false;
   
   // if called from java this call is probably superfluous (needed later, too)
   HRESULT hr = ::CoInitialize(NULL);
}

CLrd2VideoConverter::~CLrd2VideoConverter()
{
   if (m_pLrdFile != NULL)
      delete m_pLrdFile;

   if (m_bConversionActive)
      AbortConversion();

   // must be deleted before CoUninitialize()
   m_pLecturnityControl = NULL;
   m_pGraph = NULL;

   ::CoUninitialize();
}

HRESULT CLrd2VideoConverter::SetInputFile(LPCTSTR tszLrdFile)
{
   if (tszLrdFile == NULL)
      return E_POINTER;

   if (m_pLrdFile != NULL)
      delete m_pLrdFile;

   bool bFileExists = _taccess(tszLrdFile, 00) == 0;
   if (!bFileExists)
      return E_INVALIDARG;

   m_pLrdFile = new CLrdFile();
   bool bOpenSuccess = m_pLrdFile->Open(tszLrdFile);

   if (bOpenSuccess)
   {
      m_csLrdFilename = tszLrdFile;

      return S_OK;
   }
   else
   {
      delete m_pLrdFile;
      m_pLrdFile = NULL;

      return E_FAIL;
   }
}

HRESULT CLrd2VideoConverter::CreateVideoCompressor(
    CComPtr<IBaseFilter> &pVideoCompressor, int nVideoQFactor, float fFrameRate, 
    bool bTargetIPod, int iOutputWidth, int iOutputHeight) {

    HRESULT hr = S_OK;


    hr = ::CoCreateInstance(
        CLSID_H264VideoEncoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pVideoCompressor);

    if (SUCCEEDED(hr)) {
        CComPtr<IModuleConfig> pMainConfig;
        HRESULT hrConfig = pVideoCompressor->QueryInterface(IID_IModuleConfig, (void **)&pMainConfig);

        if (SUCCEEDED(hrConfig)) {
            VARIANT variant = {0};
            variant.vt = VT_INT;

            bool bHighResolutionIPod = bTargetIPod && (iOutputWidth > 352 || iOutputHeight > 288);

            variant.lVal = H264VE::Profile_Baseline;
            pMainConfig->SetValue(&MC_PROFILE, &variant);

            variant.lVal = H264VE::VideoType_BASELINE;
            if (bTargetIPod)
                variant.lVal = H264VE::VideoType_iPOD;
            pMainConfig->SetValue(&MC_PRESET, &variant);

            variant.lVal = H264VE::Level_4; // for Youtube

            if (bTargetIPod) {
                if (bHighResolutionIPod)
                    variant.lVal = H264VE::Level_3; // newer iPods
                else
                    variant.lVal = H264VE::Level_13; // older iPods

                // TODO this is only implicitly and depending on resolution...
            }

            // Newer iPods also support Level_3;
            // The Level_4 is chosen rather arbitrarily (its resolution limit is sufficient)
            pMainConfig->SetValue(&MC_LEVEL, &variant);

            // TODO depending on resolution?
            //      And/or: Allow direct setting of bitrates and/or make the nVideoQFactor
            //      again a true quality factor.

            int iMaxBitrate = 3500; // somewhat arbitrary...
            if (bTargetIPod) {
                if (bHighResolutionIPod)
                    iMaxBitrate = 2400;
                else
                    iMaxBitrate = 720; // small resolution: make compatible with older iPods
            }

            int iBitrate = ((32 - nVideoQFactor) / 31.0) * iMaxBitrate;

            variant.lVal = iBitrate * 1000;
            pMainConfig->SetValue(&MC_BITRATE_AVG, &variant);


            variant.llVal = MC_COMMON::State_On;
            pMainConfig->SetValue(&EH264VE_FixedRate, &variant);
           

            variant.vt = VT_R8;

            variant.dblVal = fFrameRate;
            pMainConfig->SetValue(&EH264VE_Framerate, &variant);


            VARIANT returnReason = {0};
            hrConfig = pMainConfig->CommitChanges(&returnReason);

            if (FAILED(hrConfig)) {
                _tprintf(_T("Lrd2Video: Mainconcept h.264 video config failed.\n"));
            }
        }


    } else if (FAILED(hr)) {
        hr = ::CoCreateInstance(
            CLSID_LMMP4Encoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pVideoCompressor);

        if (SUCCEEDED(hr)) {
            CComPtr<ILMMP4Encoder> pLeadConfig;
            hr = pVideoCompressor->QueryInterface(IID_ILMMP4Encoder, (void **)&pLeadConfig);

            HRESULT hrConfig = pLeadConfig->put_QFactor(nVideoQFactor);
            
            if (SUCCEEDED(hrConfig)) {
                if (bTargetIPod)
                    hrConfig = pLeadConfig->put_BFrames(0); // important for replay on iPod
                else
                    hrConfig = pLeadConfig->put_BFrames(3);
            }

            if (FAILED(hrConfig))
                hr = hrConfig;
        }

    }

           
    return hr;
}

HRESULT CLrd2VideoConverter::CreateAudioCompressor(CComPtr<IBaseFilter> &pAudioCompressor, UINT nAudioKBit) {
    HRESULT hr = S_OK;

    hr = ::CoCreateInstance(
        CLSID_AACEncoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pAudioCompressor);

    if (SUCCEEDED(hr)) {
        CComPtr<IModuleConfig> pMainConfig;
        HRESULT hrConfig = pAudioCompressor->QueryInterface(IID_IModuleConfig, (void **)&pMainConfig);

        if (SUCCEEDED(hrConfig)) {
            VARIANT variant = {0};
            variant.vt = VT_INT;

            // Strange way of specifying a bit rate...
            int iAudioBitrateIndex = EAACENC::IMC_AACENC_BR_64;

            for (int i=0; i<EAACENC::IMC_AACENC_BR_END; ++i) {
                if (EAACENC::idx2bit_rate[i] > nAudioKBit && i > 0) {
                    iAudioBitrateIndex = i - 1;
                    break;
                } else if (EAACENC::idx2bit_rate[i] == nAudioKBit) {
                    iAudioBitrateIndex = i;
                    break;
                }
            }

            variant.lVal = iAudioBitrateIndex;
            pMainConfig->SetValue(&MC_BITRATE_AVG, &variant);

            variant.lVal = EAACENC::IMC_AACENC_MV_M4;
            pMainConfig->SetValue(&ELAACENC_MPEG_Version, &variant);

            variant.lVal = EAACENC::IMC_AACENC_AOT_LC;
            pMainConfig->SetValue(&ELAACENC_Object_Type, &variant);

            variant.lVal = EAACENC::IMC_AACENC_OUT_RAW;
            pMainConfig->SetValue(&ELAACENC_Output_Format, &variant);


            VARIANT returnReason = {0};
            hrConfig = pMainConfig->CommitChanges(&returnReason);

            if (FAILED(hrConfig)) {
                _tprintf(_T("Lrd2Video: Mainconcept Aac audio config failed.\n"));
            }

        }
    }


    if (FAILED(hr)) {

       hr = ::CoCreateInstance(
           CLSID_LMAACEncoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pAudioCompressor);
    }

    return hr;
}

HRESULT CLrd2VideoConverter::PrepareConversion(LPCTSTR tszOutputFilename, bool bIsIgnored,
                                               float fFrameRate, UINT nVideoQFactor, UINT nAudioRateKbit,
                                               UINT nOutputWidth, UINT nOutputHeight, bool bEmbedClips,
                                               bool bVideoInsteadOfPages, bool bTargetIPod)
{
   _tprintf(_T("Lrd2Video: PrepareConversion()...\n"));
   bool bUncompressedAvi = false;

   // any method handling COM objects and being called from java needs its own:
   HRESULT hr = ::CoInitialize(NULL);

   if (tszOutputFilename == NULL)
      return E_POINTER;

   if (m_pLrdFile == NULL)
      return E_FAIL;

   if (fFrameRate <= 0 || nVideoQFactor == 0 || nAudioRateKbit == 0 || nOutputWidth == 0 || nOutputHeight == 0)
      return E_INVALIDARG;

   // 640x480 is the limit of the iPods; the used h.264 level 3.0 would also support slightly more

   if (bTargetIPod && (nOutputWidth > 640 || nOutputHeight > 480) 
       || !bTargetIPod && (nOutputWidth > 1920 || nOutputHeight > 1080)) {

       return E_INVALIDARG; // The profile resolution bounds may not be exceeded...
   }

   CWhiteboardStream *pWhiteboard = m_pLrdFile->CreateWhiteboardStream(false);

   LPCTSTR tszEvqFilename = NULL;
   LPCTSTR tszObjFilename = NULL;
   CSize dimPage;
   dimPage.cx = 0; 
   dimPage.cy = 0;

   if (pWhiteboard != NULL)
   {
      tszEvqFilename = pWhiteboard->GetEvqFilename();
      tszObjFilename = pWhiteboard->GetObjFilename();
      pWhiteboard->GetPageDimension(dimPage);
   }
   // else Denver document

   CString csLadFilename = m_pLrdFile->GetAudioFilename();
   LPCTSTR tszLadFilename = csLadFilename;

   if (pWhiteboard != NULL && (tszEvqFilename == NULL || tszObjFilename == NULL))
      return E_FAIL;

   if (tszLadFilename == NULL)
      return E_FAIL;

   _tprintf(_T("Lrd2Video: Audio filename: %s\n"), tszLadFilename);

   CString csVideoFilename;
   if (bVideoInsteadOfPages)
      csVideoFilename = m_pLrdFile->GetVideoFilename();

   if (dimPage.cx == 0)
   {
      // defaults
      dimPage.cx = 720;
      dimPage.cy = 540;
   }

   _tprintf(_T("Lrd2Video: Page dimensions: %dx%d\n"), dimPage.cx, dimPage.cy);

   if (bUncompressedAvi)
   {
      CString csOut = tszOutputFilename; 
      csOut += _T(".avi");
      tszOutputFilename = (LPCTSTR)csOut;
   }
   


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
            // TODO check for overwriting??
            //int iResult = ::MessageBox(
            //   pDlg->m_hWnd, _T("Output file exists. Do you want to overwrite it?"),
            //   _T("Question"), MB_YESNO);
            
            //if (iResult == IDNO)
            //   return S_FALSE;


            if (bUncompressedAvi)
               ::DeleteFile(tszOutputFilename); // avi files are not erased upon opening
         }
         else
         {
            //::MessageBox(
            //   pDlg->m_hWnd, _T("Output file exists but cannot be overwritten."),
            //   _T("Error"), MB_OK | MB_ICONEXCLAMATION);
         }
      }
   }

   
   if (m_pGraph)
      m_pGraph = NULL;

   // find out about clips
   CClipStream *pClipStream = NULL;
   CArray<CClipInfo*, CClipInfo*> aClips;
   if (SUCCEEDED(hr))
   {
      pClipStream = m_pLrdFile->CreateClipStream();
      if (pClipStream != NULL && pClipStream->HasClips())
      {
         int i = 0;
         CClipInfo *pClip = pClipStream->FindClip(i);
         while (pClip != NULL)
         {
            aClips.Add(pClip);
            ++i;
            pClip = pClipStream->FindClip(i);
         }
      }
   }

   _tprintf(_T("Lrd2Video: Found clips #: %d\n"), aClips.GetSize());

   // determine audio length and thus approximate document length
   CComPtr<IMediaDet> pMediaDet;
   if (SUCCEEDED(hr))
      hr = ::CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, IID_IMediaDet, (void **)&pMediaDet);
   
   CComBSTR bstrAudioname(tszLadFilename);
   if (SUCCEEDED(hr))
      hr = pMediaDet->put_Filename(bstrAudioname);
   
   if (SUCCEEDED(hr))
      hr = pMediaDet->put_CurrentStream(0); // only one stream in a audio file
   
   double dStreamLength = 0;
   if (SUCCEEDED(hr))
      hr = pMediaDet->get_StreamLength(&dStreamLength);

   pMediaDet = NULL; // release file

   if (SUCCEEDED(hr) && dStreamLength > 0)
      m_dAudioSeconds = dStreamLength;

   if (SUCCEEDED(hr))
      _tprintf(_T("Lrd2Video: Audio length [seconds]: %f\n"), m_dAudioSeconds);

   //
   // all necessary information gathered, now build the render graph
   //

   if (SUCCEEDED(hr)) {

      // TODO Lead kernel and its unlocking is not required anymore if other codecs are used.
      // Currently still here to have Lead as a fallback.

      CComPtr<ILMDSKernel2> pLeadKernel;

      HRESULT hrLeadUnlocking = ::CoCreateInstance(CLSID_LMDSKernel2, 
          NULL, CLSCTX_INPROC_SERVER, IID_ILMDSKernel2, (void **)&pLeadKernel);

      CComBSTR bstrAppId(_T("LECTURNITY Video"));
      if (SUCCEEDED(hrLeadUnlocking)) {
         hrLeadUnlocking = pLeadKernel->UnlockModule(
            CComBSTR(_T("591407-7050000-001")), UNLOCK_APP_ONCE, bstrAppId);
      }

      if (FAILED(hrLeadUnlocking))
          _tprintf(_T("Lrd2Video: Kernel unlocking did NOT work.\n"));



      CComPtr<ICaptureGraphBuilder2> pBuilder;
      if (SUCCEEDED(hr))
      {
         hr = ::CoCreateInstance(CLSID_CaptureGraphBuilder2, 
            NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void **)&pBuilder);
      }
     
      CComPtr<IBaseFilter> pMux;
      if (SUCCEEDED(hr))
      {
         CComBSTR bstrOutputname(tszOutputFilename);

         GUID guidMuxer = CLSID_EMP4Mux;
               
         CComPtr<IBaseFilter> pTestMux;
         HRESULT hrTestMux = ::CoCreateInstance(
             guidMuxer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pTestMux);

        
         if (FAILED(hrTestMux))
            guidMuxer = CLSID_LMIsoMux; // old one


         hr = pBuilder->SetOutputFileName(
             bUncompressedAvi ? &MEDIASUBTYPE_Avi : &guidMuxer, bstrOutputname, &pMux, NULL);

         if (SUCCEEDED(hr) && guidMuxer == CLSID_EMP4Mux) {
             // Configure the mux; especially the "Quicktime" compatibility setting

             CComPtr<IModuleConfig> pMainConfig;
             HRESULT hrConfig = pMux->QueryInterface(IID_IModuleConfig, (void **)&pMainConfig);

             if (SUCCEEDED(hrConfig)) {
                 VARIANT variant = {0};
                 variant.vt = VT_INT;


                 variant.lVal = EMP4Muxer::CF_QT; // for (older) Quicktime player (<7.6.6)
                 pMainConfig->SetValue(&EMP4Mux_Compatibility, &variant);


                 VARIANT returnReason = {0};
                 hrConfig = pMainConfig->CommitChanges(&returnReason);

                 if (FAILED(hrConfig)) {
                     _tprintf(_T("Lrd2Video: Mainconcept Mux config failed.\n"));
                 }
             }
         }

      }


      

      if (SUCCEEDED(hr))
         hr = pBuilder->GetFiltergraph(&m_pGraph);

            
      if (SUCCEEDED(hr))
         _tprintf(_T("Lrd2Video: Graph created and output set.\n"));


      CComPtr<IBaseFilter> pSrc;
      if (SUCCEEDED(hr)) {
         hr = ::CoCreateInstance(CLSID_LecturnitySource, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pSrc);
      }

      CComPtr<ILecturnitySourceControl> pLecturnityControl;
      if (SUCCEEDED(hr))
         hr = pSrc->QueryInterface(IID_ILecturnitySourceControl, (void **)&pLecturnityControl);

      if (SUCCEEDED(hr) && pWhiteboard != NULL && !bVideoInsteadOfPages) {
         hr = pLecturnityControl->ParseFiles(tszEvqFilename, tszObjFilename);
      }

      if (SUCCEEDED(hr))
         hr = pLecturnityControl->SetVideoSizes(nOutputWidth, nOutputHeight, dimPage.cx, dimPage.cy);

      if (SUCCEEDED(hr))
         hr = pLecturnityControl->SetFrameRate(fFrameRate);
            
      if (SUCCEEDED(hr))
         _tprintf(_T("Lrd2Video: Lecturnity Source configured: fps %f, out %dx%d\n"), fFrameRate, nOutputWidth, nOutputHeight);
      
      if (SUCCEEDED(hr) && bEmbedClips && aClips.GetSize() > 0) {
         for (int i=0; i<aClips.GetSize() && SUCCEEDED(hr); ++i) {
            LPCTSTR szClipFilename = aClips[i]->GetFilename();
            UINT nBeginTime = aClips[i]->GetTimestamp();
            hr = pLecturnityControl->AddClip(szClipFilename, nBeginTime);
         }

         if (SUCCEEDED(hr))
            _tprintf(_T("Lrd2Video: Clips added.\n"));

      }

      // add video last so it will be found last during encoding (clips "on top" )
      if (SUCCEEDED(hr) && bVideoInsteadOfPages && csVideoFilename.GetLength() > 0) {
         int iVideoOffset = m_pLrdFile->GetVideoOffset();

         hr = pLecturnityControl->AddClip(csVideoFilename, iVideoOffset);

         if (SUCCEEDED(hr))
            _tprintf(_T("Lrd2Video: Video added.\n"));
      }

      // video should be as long as the audio at least; otherwise there are white frames at the end
      if (SUCCEEDED(hr))
         hr = pLecturnityControl->SetMinimumLength((UINT)(m_dAudioSeconds * 1000.0));

      // for obtaining position information in the conversion thread
      if (SUCCEEDED(hr)) {
         if (m_pLecturnityControl != NULL)
            m_pLecturnityControl = NULL;

         hr = pLecturnityControl->QueryInterface(IID_ILecturnitySourceControl, (void **)&m_pLecturnityControl);
      }

      if (SUCCEEDED(hr))
         hr = m_pGraph->AddFilter(pSrc, L"LEC Data Source");

      CComPtr<IBaseFilter> pSrcAudio;
      if (SUCCEEDED(hr)) {
         CComBSTR bstrAudioname(tszLadFilename);
         hr = m_pGraph->AddSourceFilter(bstrAudioname, L"Audio Source Filter", &pSrcAudio);
      }

            
      if (SUCCEEDED(hr))
         _tprintf(_T("Lrd2Video: Input (audio, lecturnity) set.\n"));


      // find filter
      CComPtr<IBaseFilter> pVideoCompressor; // remains NULL for uncompressed avi
      if (!bUncompressedAvi) {
          if (SUCCEEDED(hr)) {
              hr = CreateVideoCompressor(pVideoCompressor, nVideoQFactor, fFrameRate, 
                  bTargetIPod, nOutputWidth, nOutputHeight);
          }
                 
         if (SUCCEEDED(hr))
            hr = m_pGraph->AddFilter(pVideoCompressor, L"Video Compressor");

         if (SUCCEEDED(hr))
            _tprintf(_T("Lrd2Video: Video Filter added and configured: qfactor %d\n"), nVideoQFactor);

      }


      CComPtr<IBaseFilter> pAudioCompressor;
      if (!bUncompressedAvi)
      {
         // TODO If this codec(s) works then make all configuration in CreateAudioCompressor()
         if (SUCCEEDED(hr)) {
            hr = CreateAudioCompressor(pAudioCompressor, nAudioRateKbit);
         }
         
         if (SUCCEEDED(hr))
            hr = m_pGraph->AddFilter(pAudioCompressor, L"Audio Compressor");

         if (SUCCEEDED(hr)) {
            CComPtr<ILMAACEncoder> pLeadConfig;
            HRESULT hrLeadAudio = pAudioCompressor->QueryInterface(IID_ILMAACEncoder, (void **)&pLeadConfig);
            
            if (SUCCEEDED(hrLeadAudio)) {
                hrLeadAudio = pLeadConfig->put_OutputChannels(OutputChannel_UNCHANGED);

                if (SUCCEEDED(hrLeadAudio))
                    hrLeadAudio = pLeadConfig->put_OutputBitrate(nAudioRateKbit);
                if (SUCCEEDED(hrLeadAudio))
                    hrLeadAudio = pLeadConfig->put_ObjectType(ObjectType_MAIN); // important for replay on iPod

                if (FAILED(hrLeadAudio))
                    _tprintf(_T("Lrd2Video: Lead audio config failed.\n"));
            }
         }

         if (SUCCEEDED(hr))
            _tprintf(_T("Lrd2Video: Audio Filter added and configured: rate %d.\n"), nAudioRateKbit);

      }

      if (SUCCEEDED(hr)) {
          if (pVideoCompressor != NULL) {
              //hr = pBuilder->RenderStream(NULL, &MEDIATYPE_Video, pSrc, pVideoCompressorMono, pMux);

              // Note: RenderStream() does not work (E_FAIL) on Monogram video compressor?

              /*
              if (bVideoIsMonogram) {
                  CComPtr<IBaseFilter> pYv12ColorTransform;
                  if (SUCCEEDED(hr))
                      hr = ::CoCreateInstance(CLSID_RgbToYv12Filter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pYv12ColorTransform);

                  if (SUCCEEDED(hr))
                      hr = m_pGraph->AddFilter(pYv12ColorTransform, L"Yv12 Color Transform");
                  // Will be used by the Connect() below: The new x264 needs Yv12 as input
              }
              */


              hr = CDsHelper::ConnectFiltersSimple(m_pGraph, pSrc, pVideoCompressor);

              if (SUCCEEDED(hr))
                  hr = CDsHelper::ConnectFiltersSimple(m_pGraph, pVideoCompressor, pMux);
          } else {
              hr = CDsHelper::ConnectFiltersSimple(m_pGraph, pSrc, pMux);
          }

      }


      if (SUCCEEDED(hr))
      {
         //hr = pBuilder->RenderStream(NULL, &MEDIATYPE_Audio, pSrcAudio, pAudioCompressor, pMux);
         
         // for lad input it needs two intermediate filters: LAD Parser and Wave Parser
         // and it won't find them with MEDIATYPE_Audio specified
         hr = pBuilder->RenderStream(NULL, NULL, pSrcAudio, pAudioCompressor, pMux);

      }
      
      if (SUCCEEDED(hr))
         _tprintf(_T("Lrd2Video: Filters connected.\n"));


      
      HRESULT hrEvalSetting = hr;
     
      CComPtr<IFilterGraph> pFilterGraph;
      if (SUCCEEDED(hrEvalSetting))
         hrEvalSetting = m_pGraph->QueryInterface(IID_IFilterGraph, (void **)&pFilterGraph);

      CComPtr<IEnumFilters> pEnumFilters;
      if (SUCCEEDED(hrEvalSetting))
         hrEvalSetting = pFilterGraph->EnumFilters(&pEnumFilters);

      if (SUCCEEDED(hrEvalSetting))
      {
         CComPtr<IBaseFilter> pFilter;
         ULONG cFetched;
         
         while(pEnumFilters->Next(1, &pFilter, &cFetched) == S_OK)
         {
            CComPtr<ILadParser> pLadParser;
            HRESULT hrLadQuery = pFilter->QueryInterface(IID_ILadParser, (void **)&pLadParser);

            if (SUCCEEDED(hrLadQuery))
            {
               char szLadInfo[128];
               szLadInfo[0] = 0;

               int iDummy = 0;
               hrLadQuery = pLadParser->QueryLadInfo(szLadInfo, &iDummy);

               if (SUCCEEDED(hrLadQuery) && szLadInfo[0] == 'e')
               {
                  pLecturnityControl->SetIsEvaluation(true);

                  _tprintf(_T("Lrd2Video: LAD is evaluation.\n"));
               }
            }
            

            pFilter = NULL;
         }
      } 
   }

#ifdef _DEBUG
   if (false && SUCCEEDED(hr))
   {
      AddToRot(m_pGraph, &g_dwRegister);
      int iResult = ::MessageBox(::GetForegroundWindow(), _T("Inspect now. Continue?"), _T("Waiting"), MB_YESNO);
      RemoveFromRot(g_dwRegister);

      if (iResult == IDNO)
         hr = E_ABORT;
   }
#endif

   if (pClipStream != NULL)
      delete pClipStream;

   ::CoUninitialize();


   return hr;
}

// static
DWORD WINAPI CLrd2VideoConverter::ConversionLauncher(void *pParameter)
{
   if (pParameter != NULL)
   {
      HRESULT hr = ((CLrd2VideoConverter *)pParameter)->ConversionThread();
      if (SUCCEEDED(hr))
         return 0;
      else
         return hr;
   }
   return
      E_FAIL;
}

HRESULT CLrd2VideoConverter::StartConversion()
{
   if (m_pGraph == NULL)
      return E_FAIL;

   m_bConversionAborted = false;
   m_iCurrentProgress = 0;

   HANDLE hThread = ::CreateThread(NULL, 0, ConversionLauncher, this, CREATE_SUSPENDED, NULL);
   if (hThread != NULL)
   {
      _tprintf(_T("Lrd2Video: Thread created successfully.\n"));

      ::ResumeThread(hThread);
   }
   else
   {
      _tprintf(_T("Lrd2Video: Thread creation failed.\n"));

      return E_FAIL;
   }
   
   return S_OK;
}

HRESULT CLrd2VideoConverter::AbortConversion()
{
   HRESULT hr = ::CoInitialize(NULL);

   if (m_pGraph == NULL)
      return S_FALSE;

   CComPtr<IMediaControl> pControl;
   if (SUCCEEDED(hr))
      hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
   
   m_bConversionAborted = true;

   if (SUCCEEDED(hr))
      hr = pControl->Stop();

   m_bConversionActive = false;

   ::CoUninitialize();

   return hr;
}

HRESULT CLrd2VideoConverter::CheckForFilters() {
    HRESULT hr = ::CoInitialize(NULL);

    bool bMainMuxMissing = false;
    bool bMainH264Missing = false;
    bool bMainAacMissing = false;

    if (SUCCEEDED(hr)) {
        CComPtr<IBaseFilter> pMainMux;
        hr = ::CoCreateInstance(CLSID_EMP4Mux, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pMainMux);

        if (FAILED(hr)) {
            _tprintf(_T("Lrd2Video: Main mux missing.\n"));
            bMainMuxMissing = true;
        }
    }

    if (SUCCEEDED(hr)) {
        CComPtr<IBaseFilter> pMainH264;
        hr = ::CoCreateInstance(CLSID_H264VideoEncoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pMainH264);

        if (FAILED(hr)) {
            _tprintf(_T("Lrd2Video: Main h264 missing.\n"));
            bMainH264Missing = true;
        }
    }

    /*
    if (SUCCEEDED(hr)) {
        CComPtr<IBaseFilter> pRgb2Yv12;
        hr = ::CoCreateInstance(CLSID_RgbToYv12Filter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pRgb2Yv12);

        if (FAILED(hr))
            _tprintf(_T("Lrd2Video: Rgb2Yv12 missing.\n"));
    }
    */

    if (SUCCEEDED(hr)) {
        CComPtr<IBaseFilter> pMainAac;
        hr = ::CoCreateInstance(CLSID_AACEncoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pMainAac);

        if (FAILED(hr)) {
            _tprintf(_T("Lrd2Video: Main Aac missing.\n"));
            bMainAacMissing = true;
        }
    }

    bool bMainAllMissing = bMainMuxMissing && bMainH264Missing && bMainAacMissing;
    // Else: report an error because there might otherwise be undesired 
    //       filter combinations between MainConcept and Lead


    if (FAILED(hr) && bMainAllMissing) {
        // Some filter is missing; try Leadtools instead
        //

        CComPtr<ILMDSKernel2> pLeadKernel;
        hr = ::CoCreateInstance(CLSID_LMDSKernel2, NULL, CLSCTX_INPROC_SERVER, IID_ILMDSKernel2, (void **)&pLeadKernel);

        if (FAILED(hr))
            _tprintf(_T("Lrd2Video: LEAD Kernel missing.\n"));

        CComPtr<IBaseFilter> pLmMux;
        if (SUCCEEDED(hr)) {
            hr = ::CoCreateInstance(CLSID_LMIsoMux, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pLmMux);

            if (FAILED(hr))
                _tprintf(_T("Lrd2Video: Lm Mux filter missing.\n"));
        }


        CComPtr<IBaseFilter> pLmVideo;
        if (SUCCEEDED(hr)) {
            hr = ::CoCreateInstance(CLSID_LMMP4Encoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pLmVideo);

            if (FAILED(hr))
                _tprintf(_T("Lrd2Video: Lm Video encoder filter missing.\n"));
        }


        CComPtr<IBaseFilter> pLmAudio;
        if (SUCCEEDED(hr)) {
            hr = ::CoCreateInstance(CLSID_LMAACEncoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pLmAudio);

            if (FAILED(hr))
                _tprintf(_T("Lrd2Video: Lm Audio encoder filter missing.\n"));
        }
    }

    
    if (SUCCEEDED(hr)) {
        CComPtr<IBaseFilter> pSrc;
        hr = ::CoCreateInstance(CLSID_LecturnitySource, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pSrc);

        if (FAILED(hr))
            _tprintf(_T("Lrd2Video: Lecturnity Source Filter missing.\n"));
    }


   ::CoUninitialize();

   if (FAILED(hr))
      hr = E_VID_FILTERS_MISSING;

   return hr;
}

// private
HRESULT CLrd2VideoConverter::ConversionThread()
{
   _tprintf(_T("Lrd2Video: ConversionThread()...\n"));

   // apart from running and stopping the graph (2 lines)
   // this method only does progress tracking (many lines)

   HRESULT hr = ::CoInitialize(NULL);

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

   //SaveGraphFile(m_pGraph, L"C:\\TEMP\\graph-l.grf");
   
   if (SUCCEEDED(hr))
      hr = pControl->Run();

   if (SUCCEEDED(hr))
      _tprintf(_T("Lrd2Video: Graph run started.\n"));

   if (SUCCEEDED(hr))
      m_bConversionActive = true;

   bool bCanObtainPosition = false;
   REFERENCE_TIME tCurrent;
   if (SUCCEEDED(hr))
      bCanObtainPosition = SUCCEEDED(pSeeking->GetCurrentPosition(&tCurrent));
   UINT nCurrentPosMs = 0;

   // Note: a graph with the LEAD multiplexer does not have a proper IMediaSeeking
   // interface/capability and neither does the LEAD_MediaMuxer filter.


   if (SUCCEEDED(hr))
   {
      if (m_dAudioSeconds > 0 && (bCanObtainPosition || m_pLecturnityControl != NULL))
      {
         while (SUCCEEDED(hr))
         {
            if (SUCCEEDED(hr))
            {
               if (bCanObtainPosition)
                  hr = pSeeking->GetCurrentPosition(&tCurrent);
               else
               {
                  hr = m_pLecturnityControl->GetLastSampleTime(&nCurrentPosMs);
                  tCurrent = (LONGLONG)nCurrentPosMs * (LONGLONG)10000;
               }
            }

            long lEventCode = 0;
            LONG_PTR lParam1 = 0;
            LONG_PTR lParam2 = 0;
            HRESULT hrEvent = pEvent->GetEvent(&lEventCode, &lParam1, &lParam2, 0); // do not wait/block

            if (hrEvent != E_ABORT)
            {
               // actually there was an event
               if (lEventCode == EC_ERRORABORT)
               {
                  hr = (HRESULT)lParam1;
               }
            }

            pEvent->FreeEventParams(lEventCode, lParam1, lParam2);
            
            if (SUCCEEDED(hr))
            {
               int iCurrentMillis = tCurrent/10000;
               double dCurrentSeconds = iCurrentMillis/1000.0;
               // Note: directly doing tCurrent/10000*1000.0 does not work for tCurrent being negative!?
               
               if (dCurrentSeconds >= m_dAudioSeconds)
               {
                  _tprintf(_T("Lrd2Video: End reached %f >= %f.\n"), dCurrentSeconds, m_dAudioSeconds);
                  break;
               }
               else
               {
                  UINT nNewProgress = (UINT)(dCurrentSeconds*(10000/m_dAudioSeconds));
                  
                  //if (nNewProgress < m_iCurrentProgress && nNewProgress == 0)
                  //    break; // looks finished, with x264 the last timestamp is "0"...

                  m_iCurrentProgress = nNewProgress;
                  _tprintf(_T("\r%d %f"), m_iCurrentProgress, dCurrentSeconds);
                  if (m_iCurrentProgress >= 10000)
                     m_iCurrentProgress = 9999; // do not set progress to 100% until writing is really finished
                  
                  ::Sleep(50);
               }
            }
         }
         _tprintf(_T("\n")); // finish progress output above
      }
      else
      {
         // something went wrong; this shouldn't happen; wait anyway for some time

         long evCode;
         pEvent->WaitForCompletion(600000, &evCode);
         
         // Note: Do not use INFINITE in a real application, because it
         // can block indefinitely.
         
         _tprintf(_T("Lrd2Video: !!! No progress tracking. Waiting without tracking.\n"));

      }
   }

   if (!m_bConversionAborted && SUCCEEDED(hr))
   {
      // wait some more in case it didn't really finish yet
      long evCode;
      pEvent->WaitForCompletion(10000, &evCode);
   }

   if (SUCCEEDED(hr))
      hr = pControl->Stop(); // finish the file

   if (SUCCEEDED(hr))
      _tprintf(_T("Lrd2Video: Graph run stopped.\n"));

   m_bConversionActive = false;
 
   if (SUCCEEDED(hr))
      m_iCurrentProgress = 10000; // file finished
   else
   {
      if (hr == E_LS_VIDEO_FAILED)
         hr = imc_lecturnity_converter_LPLibs_VIDEO_DECODING_FAILED;
      if (hr == E_NOT_ENOUGH_DISK_SPACE)
         hr = imc_lecturnity_converter_LPLibs_NOT_ENOUGH_DISK_SPACE;
      m_iCurrentProgress = hr; // is now a negative value (error signalling)
   }

   if (FAILED(hr))
      _tprintf(_T("Lrd2Video: Graph run failed: 0x%x\n"), hr);

   ::CoUninitialize();

   return hr;
}


// private
HRESULT CLrd2VideoConverter::FindFilter(bool bAudio, LPCTSTR tszCodecName, IBaseFilter **ppTargetFilter)
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