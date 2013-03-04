#include "StdAfx.h"
#include "avedit.h"
#include "avinfo.h"
#include "wavdest.h"
#include <vector>

AVTarget::AVTarget(const AVSource **paSources, int nNumSources,
                   const AM_MEDIA_TYPE *pFormatDefinition, 
                   const _TCHAR *tszTargetFileName,
                   bool useSmartRecompression,
                   int nMasterStart, int nMasterEnd,
                   DWORD dwFcc,
                   int nKeyframeRate, int nQuality)
{
   m_nNumSources = nNumSources;
   m_useSmartRecompression = useSmartRecompression;
   m_nKeyframeRate = nKeyframeRate;
   m_nQuality = nQuality;
   m_dwFcc = dwFcc;
   m_paSources = new AVSource*[m_nNumSources];
   for (int i=0; i<m_nNumSources; ++i)
      m_paSources[i] = paSources[i]->DeepClone();

   m_pFormatDefinition = NULL;
   if (pFormatDefinition)
   {
      // m_pFormatDefinition = CreateMediaType(pFormatDefinition);
      m_pFormatDefinition = new AM_MEDIA_TYPE;
      CopyMediaType(m_pFormatDefinition, pFormatDefinition);
   }

   m_tszTargetFileName = NULL;
   if (tszTargetFileName)
   {
      m_tszTargetFileName = new _TCHAR[_tcslen(tszTargetFileName)+1];
      _tcscpy(m_tszTargetFileName, tszTargetFileName);
   }

   m_nMasterStart = nMasterStart;
   m_nMasterEnd = nMasterEnd;

   m_hasLadInfo = false;
   m_bIsStartFillClip = false;
}


AVTarget::~AVTarget()
{
   for (int i=0; i<m_nNumSources; ++i)
      delete m_paSources[i];

   if (m_pFormatDefinition != NULL)
   {
      // DeleteMediaType(m_pFormatDefinition);
      FreeMediaType(*m_pFormatDefinition);
      delete m_pFormatDefinition;
      m_pFormatDefinition = NULL;
   }

   if (m_tszTargetFileName != NULL)
      delete[] m_tszTargetFileName;
   m_tszTargetFileName = NULL;
}

void AVTarget::SetLadInfo(const char *szLadInfo)
{
   if (szLadInfo)
   {
      strncpy(m_szLadInfo, szLadInfo, 128);
      m_hasLadInfo = true;
   }
   else
      m_hasLadInfo = false;
}

AVTarget* AVTarget::DeepClone() const
{
   AVTarget *clone = new AVTarget(
      (const AVSource**)m_paSources, m_nNumSources, m_pFormatDefinition,
      m_tszTargetFileName, m_useSmartRecompression, 
      m_nMasterStart, m_nMasterEnd,
      m_dwFcc,
      m_nKeyframeRate, m_nQuality);

   memcpy(clone->m_szLadInfo, m_szLadInfo, 128);
   clone->m_hasLadInfo = m_hasLadInfo;
   clone->m_bIsStartFillClip = m_bIsStartFillClip;

   return clone;
}

HRESULT AVTarget::AddTo(IAMTimeline *pTimeline, bool bForPreview)
{
   _ASSERT(pTimeline);

   HRESULT hr = S_OK;

   CComPtr< IAMTimelineObj >  pGroupObj;
   if (SUCCEEDED(hr))
   {
      hr = pTimeline->CreateEmptyNode(&pGroupObj, TIMELINE_MAJOR_TYPE_GROUP);
   }
   CComQIPtr< IAMTimelineGroup, &IID_IAMTimelineGroup> pGroup(pGroupObj);
   CComQIPtr< IAMTimelineComp, &IID_IAMTimelineComp> pComp(pGroupObj);

   std::vector<IAMTimelineObj *> trackObjList;
   std::vector<IAMTimelineTrack *> trackList;
   std::vector<_TCHAR *> trackSourceFiles;
   std::vector<int> trackLastTimes;

   for (int i=0; i<m_nNumSources && SUCCEEDED(hr); ++i)
   {
      IAMTimelineObj *pTrackObj = NULL;
      IAMTimelineTrack *pTrack = NULL;
      
      // Note: Simple version of binpacking:
      // Into which track should we add this source?
      // Take the first track does not have a source later
      // than the current source.

      _TCHAR *tszSourceFileName = _T("");
      if (m_paSources[i]->HasInputFile())
          tszSourceFileName = m_paSources[i]->m_tszSourceFileName;
       
      //_RPT1(_CRT_WARN, "adding object %s\n", tszSourceFileName);
         
      int nTrack = 0;
      bool bFoundTrack = false;
      while (nTrack < trackList.size() && !bFoundTrack)
      {
         //bool bMatchesName = _tcscmp(szSourceFileName, trackSourceFiles[nTrack]) == 0;
         // Don't know why one should match names: a track can contain different files.

         if (m_paSources[i]->m_nStreamStart >= trackLastTimes[nTrack]) // && bMatchesName)
            bFoundTrack = true;
         else
            nTrack++;
      }
      
      if (!bFoundTrack)
      {
         // Create a new track!
         if (SUCCEEDED(hr))
            hr = pTimeline->CreateEmptyNode(&pTrackObj, TIMELINE_MAJOR_TYPE_TRACK);
         if (SUCCEEDED(hr))
         {
            trackObjList.push_back(pTrackObj);
            hr = pTrackObj->QueryInterface(IID_IAMTimelineTrack, (void **) &pTrack);
         }
         if (SUCCEEDED(hr))
         {
            trackList.push_back(pTrack);
            trackLastTimes.push_back(0);
            trackSourceFiles.push_back(_tcsdup(tszSourceFileName));
            
            hr = pComp->VTrackInsBefore(pTrackObj, -1);
            
            if (SUCCEEDED(hr) && m_paSources[i]->GetMainVolume() != 100)
            {
               if (nTrack == 1)
               {
                  IAMTimelineTrack *pTrackBelow = trackList[0];
                  CComQIPtr<IAMTimelineObj> pTrackObjBelow(pTrackBelow);
                  
                  hr = AVEdit::AddVolumeEffect(pTrackObjBelow, pTimeline, 
                     m_paSources[i]->GetMainVolume(), m_paSources[i]->GetStreamStart(), 
                     m_paSources[i]->GetStreamEnd());
               }
               else
               {
                  _ASSERT(false); // only one audio track and one clip audio track "above" (with effects)
               }
            }
         }
      }
      else
      {
         pTrackObj = trackObjList[nTrack];
         pTrack = trackList[nTrack];
      }

      if (SUCCEEDED(hr))
      {
         hr = m_paSources[i]->AddTo(pTrack, pTimeline);
         trackLastTimes[nTrack] = m_paSources[i]->m_nStreamEnd;


         if (bForPreview && i == 0 && !m_paSources[0]->IsAudio()) {
             // Check whether the first segment should also be shown at the very beginning

             int iTotalLength = GetMasterEnd() + 1;

             int iSegmentLength = m_paSources[0]->GetStreamLength();
             bool bIsNearlyAll = false;

             if (m_nNumSources > 1) {
                 for (int s=1; s<m_nNumSources; ++s) {
                     if (m_paSources[s]->IsSameFile(m_paSources[s-1]))
                        iSegmentLength += m_paSources[s]->GetStreamLength();
                 }
             }

             bIsNearlyAll = iSegmentLength >= 0.95 * iTotalLength; // heuristic
             if (SUCCEEDED(hr) && bIsNearlyAll && m_paSources[0]->GetStreamStart() > 0) {
                 // This is probably a pure/structured SG clip: it should be shown at the beginning
                 // This is heuristic and of course only done for preview.

                 hr = m_paSources[0]->AddTo(pTrack, pTimeline, true);
                 m_bIsStartFillClip = true;
             }
         }
      }
   }

   
   //
   // set group formats (compressed and uncompressed)
   // TODO: if target format (m_pFormatDefinition) is undefined
   //       make this aware of more than one input (with
   //       more than one format)?
   //
   AM_MEDIA_TYPE inputFormat;
   ZeroMemory(&inputFormat, sizeof AM_MEDIA_TYPE);

   if (SUCCEEDED(hr))
   {
      if (m_pFormatDefinition)
      {
         // copy the given format type to inputFormat:
         CopyMediaType(&inputFormat, m_pFormatDefinition);
      }
      else
      {
         hr = GetInputFormat(&inputFormat);
         // TODO this does not consider different sample rates for audio in the input.
         // Maybe this is done outside? What about video and frame rates?

         if (SUCCEEDED(hr))
         {
            m_pFormatDefinition = new AM_MEDIA_TYPE;
            hr = CopyMediaType(m_pFormatDefinition, &inputFormat);
         }
      }
   }

   // The next lines are a bugfix for a problem which
   // prevents DirectShow to resample 22.050 kHz files
   // to 16 kHz files. If we have a 16 kHz target media
   // type, simply change it to a 22.050 kHz target
   if (SUCCEEDED(hr))
   {
      if (m_pFormatDefinition->majortype == MEDIATYPE_Audio)
      {
         ASSERT(m_pFormatDefinition->formattype == FORMAT_WaveFormatEx);

         WAVEFORMATEX *pWfe = (WAVEFORMATEX *) m_pFormatDefinition->pbFormat;
         if (pWfe->nSamplesPerSec == 16000)
         {
            pWfe->nSamplesPerSec = 22050;
            pWfe->nAvgBytesPerSec = pWfe->nBlockAlign * pWfe->nSamplesPerSec;
         }
      }
   }


   AM_MEDIA_TYPE uncompressedFormat;
   CopyMediaType(&uncompressedFormat, &inputFormat);
   if (SUCCEEDED(hr))
      AVInfo::StaticMakeUncompressed(&uncompressedFormat);

//   CopyMediaType(&inputFormat, &uncompressedFormat);
//   CopyMediaType(m_pFormatDefinition, &uncompressedFormat);
   
   if (SUCCEEDED(hr))
   {
      hr = pGroup->SetMediaType(&uncompressedFormat);

      if (SUCCEEDED(hr) && uncompressedFormat.majortype == MEDIATYPE_Audio)
      {
         pGroup->SetOutputFPS(500.0);

         // Note:
         // Make sure the audio (preview) length is millisecond-accurat
         // otherwise it will get truncated and does not fit with the preview segments anymore.
         //
         // However this only works for other output file formats than Avi.
         // Using it on Avi (for audio) enlarges the Avi size considerably and leads to 
         // unexpected results (distorted audio) for values higher than 1000.
         //
         // Note:
         // You cannot set it 1000.0. Otherwise if you have two audio tracks
         // (one with clip audio) the audio rendering will stall at some position:
         // No samples are written anymore. It seems to work however with 500.0.
         // The possible small imperfections introduced with rounding to 500 fps will
         // be corrected by CWavDestFilter::StopStreaming().
      }
   }

   //*
   if (SUCCEEDED(hr))
   {
      SCompFmt0 *pFormat = new SCompFmt0;
      ZeroMemory(pFormat, sizeof SCompFmt0);
   
      //
      // Seltsam, seltsam:
      // Nur wenn eine SmartRenderEngine verwendet wird und dann
      // hier ein SmartRecompressFormat gesetzt wird stimmt die 
      // Video-Länge (ansonsten wird jedes zweite Frame weggeworfen)
      // 
      if (m_pFormatDefinition)
      {
         CopyMediaType(&(pFormat->MediaType), m_pFormatDefinition);
         hr = pGroup->SetSmartRecompressFormat((long *) pFormat);
      }
      else if (AVInfo::StaticIsCompressed(&inputFormat))
      {
         // TODO: Is this a memory leak?
         CopyMediaType(&(pFormat->MediaType), &inputFormat);
         hr = pGroup->SetSmartRecompressFormat((long *) pFormat);
      }

      delete pFormat;
      pFormat = NULL;
   }
   //*/

   FreeMediaType(uncompressedFormat);
   FreeMediaType(inputFormat);




   if (SUCCEEDED(hr))
   {
      // Wichtig, damit pTrack und pGroup (und pSource)
      // frei gegeben werden können.
      hr = pTimeline->AddGroup(pGroupObj);
   }

   std::vector<IAMTimelineObj *>::iterator tlIter;
   for (tlIter = trackObjList.begin(); tlIter != trackObjList.end(); ++tlIter)
      (*tlIter)->Release();
   std::vector<IAMTimelineTrack *>::iterator trIter;
   for (trIter = trackList.begin(); trIter != trackList.end(); ++trIter)
      (*trIter)->Release();
   std::vector<_TCHAR *>::iterator tsIter;
   for (tsIter = trackSourceFiles.begin(); tsIter != trackSourceFiles.end(); ++tsIter)
      delete[] *tsIter;

   return hr;
}

// helper method
HRESULT AVTarget::SetExpectedWavLength(IPin *pGroupPin, CWavDestFilter *pWavDest, IRenderEngine *pRenderEngine)
{
   if (pGroupPin == NULL || pWavDest == NULL)
      return E_POINTER;

   CComPtr<IEnumMediaTypes> pEnum;
   HRESULT hrQuery = pGroupPin->EnumMediaTypes(&pEnum);
   
   AM_MEDIA_TYPE *pMtPin = NULL;
   if (SUCCEEDED(hrQuery))
   {
      ULONG nFetched = 0;
      hrQuery = pEnum->Next(1, &pMtPin, &nFetched);
   }
   
   if (SUCCEEDED(hrQuery))
   {
      if (pMtPin->majortype == MEDIATYPE_Audio)
      {
         CComPtr<IAMTimeline> pTimeline;
         hrQuery = pRenderEngine->GetTimelineObject(&pTimeline);
         
         REFERENCE_TIME rtDuration;
         if (SUCCEEDED(hrQuery))
            hrQuery = pTimeline->GetDuration(&rtDuration);
         
         UINT nLengthMs = (UINT)(rtDuration / 10000i64);
         if (SUCCEEDED(hrQuery))
            pWavDest->SetExpectedLength(nLengthMs);
         
      }
   }
   
   if (pMtPin != NULL)
      FreeMediaType(*pMtPin);
   
   return hrQuery;
}


HRESULT AVTarget::SetFile(IRenderEngine *pRenderEngine, 
                          ICaptureGraphBuilder2 *pBuilder,
                          const int nGroupNumber, const _TCHAR *tszDirPrefix)
{
   _ASSERT(m_tszTargetFileName);

   int nStringLength = 
      (tszDirPrefix != NULL ? _tcslen(tszDirPrefix) : 0) + _tcslen(m_tszTargetFileName) + 2;
   _TCHAR *tszOutputPath = new _TCHAR[nStringLength];
   tszOutputPath[0] = 0;
   if (tszDirPrefix != NULL)
   {
      _tcscat(tszOutputPath, tszDirPrefix);
      _tcscat(tszOutputPath, _T("\\"));
   }
   _tcscat(tszOutputPath, m_tszTargetFileName);

   //_ASSERT(_access(szOutputPath, 02) == 0); // file writeable

   HRESULT hr = S_OK;
   
   AM_MEDIA_TYPE inputFormat;
   ZeroMemory(&inputFormat, sizeof AM_MEDIA_TYPE);
   if (SUCCEEDED(hr))
      hr = GetInputFormat(&inputFormat);

   bool bIsAudio = false;
      
   CComPtr<IBaseFilter> pWavBase;
   CComPtr<CWavDestFilter> pWavDest;
   CComPtr<IBaseFilter> pMux;
   CComQIPtr<ISmartRenderEngine> pSmart(pRenderEngine);

   if (SUCCEEDED(hr) && inputFormat.majortype == MEDIATYPE_Audio)
   {
      bIsAudio = true;
      
      if (SUCCEEDED(hr))
         hr = CWavDestFilter::CreateInstance(&pWavDest);

      if (m_hasLadInfo)
         pWavDest->SetLadInfo(m_szLadInfo);

      if (SUCCEEDED(hr))
         hr = pWavDest->QueryInterface(IID_IBaseFilter, (void **) &pWavBase);
   }
   
   // Delete the file before writing to it again:
   if (SUCCEEDED(hr))
      BOOL success = DeleteFile(tszOutputPath);
   
   bool bIsOutputFailure = false;
   if (SUCCEEDED(hr))
   {
      hr = pBuilder->SetOutputFileName(
         bIsAudio ? &CLSID_FileWriter : &MEDIASUBTYPE_Avi, 
         _bstr_t(tszOutputPath), &pMux, NULL);

      if (FAILED(hr))
         bIsOutputFailure = true;
   }

   bool bFileFallback = false;

   if (FAILED(hr) && bIsOutputFailure && bIsAudio)
   {
      // BUGFIX #3566: The above SetOutputFilename()
      // fails on some Win2000 computers at eon.se.

      hr = ::CoCreateInstance(CLSID_FileWriter, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **) &pMux);
   
      CComPtr< IGraphBuilder > pGraph;
      if (SUCCEEDED(hr))
         hr = pBuilder->GetFiltergraph(&pGraph);

      if (SUCCEEDED(hr))
         hr = pGraph->AddFilter(pMux, L"File Output Fallback");

      if (SUCCEEDED(hr))
         bFileFallback = true;
   }

   if (SUCCEEDED(hr) && pMux != NULL)
   {
      CComQIPtr<IFileSinkFilter2> pSink(pMux);
      if (pSink != NULL)
      {
         hr = pSink->SetMode(AM_FILE_OVERWRITE); // delete file before overwriting it

         if (SUCCEEDED(hr) && bFileFallback)
            hr = pSink->SetFileName(_bstr_t(tszOutputPath), NULL);
      }
   }

  
   FreeMediaType(inputFormat);
  
   CComPtr< IPin > pPin;
   if (SUCCEEDED(hr))
   {
      if ((hr = pRenderEngine->GetGroupOutputPin(nGroupNumber, &pPin)) == S_OK)
      {
       
         // Set the expected audio length; after cut&paste the resulting length (of the file)
         // is often a bit shorter than expected.
         if (SUCCEEDED(hr) && pWavDest != NULL && pPin != NULL)
         {
            SetExpectedWavLength(pPin, pWavDest, pRenderEngine);
         }

         // Let's start with retrieving a pointer to
         // the filter graph of the DES group:
         CComPtr< IGraphBuilder > pGraph;
         if (SUCCEEDED(hr))
            hr = pBuilder->GetFiltergraph(&pGraph);
         
         // Ok, now let's see if we need a compressor here.
         // We only need a compressor if smart recompression
         // is not active and if we're not dealing with
         // audio.
         CComPtr<IBaseFilter> pCompressor = NULL;
         if (!m_useSmartRecompression && !bIsAudio)
         {
            ASSERT(m_pFormatDefinition->formattype == FORMAT_VideoInfo);
            VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER *) m_pFormatDefinition->pbFormat;
            
            // For an RGB format, we don't need a compressor
            if (pVIH->bmiHeader.biCompression != BI_RGB)
            {
               // Ok, now let's try to create a matching
               // compressor.
               if (SUCCEEDED(hr))
                  hr = AVInfo::StaticCreateCompressor(
                  m_pFormatDefinition, m_dwFcc, &pCompressor,
                  m_nKeyframeRate, m_nQuality); //, m_dwAlternativeFcc);
               // Add the compressor to the graph. Otherwise
               // RenderStream() will fail.
               if (SUCCEEDED(hr))
                  hr = pGraph->AddFilter(pCompressor, L"AVI Compressor");
            }
         }
         else if (m_useSmartRecompression && !bIsAudio)
         {
            // In case we do use smart recompression, let's
            // configure the compression filter
            CComQIPtr<ISmartRenderEngine> pSmart(pRenderEngine);
            ASSERT(pSmart);
            
            CComPtr<IBaseFilter> pGroupComp = NULL;
            hr = pSmart->GetGroupCompressor(nGroupNumber, &pGroupComp);
            if (SUCCEEDED(hr) && pGroupComp)
            {
               hr = AVInfo::StaticConfigureCompressor(pGroupComp, 
                  m_nKeyframeRate, m_nQuality);
            }
            else
            {
               // Now it has to be a RGB format
               ASSERT(m_pFormatDefinition->formattype == FORMAT_VideoInfo);
               VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER *) m_pFormatDefinition->pbFormat;
               
               // For an RGB format, we don't need a compressor
               ASSERT(pVIH->bmiHeader.biCompression == BI_RGB);
            }
         }
         
         if (pWavBase != NULL)
         {
            if (SUCCEEDED(hr))
               hr = pGraph->AddFilter(pWavBase, L"Wav Dest");
         }
         
         if (SUCCEEDED(hr))
         {
            // damit dies funktioniert:
            // 1. die notwendigen Filter müssen im Graphen sein (zB pTransform)
            // 2. es dürfen nicht zuviele (unverbundene) sein (zB Audio und Video)
            hr = pBuilder->RenderStream(NULL, NULL, pPin, bIsAudio ? pWavBase : pCompressor, pMux);
         }
      }
   }
   
   delete[] tszOutputPath;
   
   
   return hr;
}

int AVTarget::GetMasterEnd()
{
   if (m_nMasterEnd != AV_VALUE_UNDEFINED)
      return m_nMasterEnd;

   if (m_nNumSources <= 0)
      return AV_VALUE_UNDEFINED;

   int masterEnd = -1;
   for (int i=0; i<m_nNumSources; ++i)
   {
      int thisEnd = m_paSources[i]->m_nStreamEnd;
      if (thisEnd > masterEnd)
         masterEnd = thisEnd;
   }

   m_nMasterEnd = masterEnd;
   return m_nMasterEnd;
}

int AVTarget::GetMasterStart()
{
   if (m_nMasterStart != AV_VALUE_UNDEFINED)
      return m_nMasterStart;

   if (m_nNumSources <= 0)
      return AV_VALUE_UNDEFINED;

   int masterStart = 0x7fffffff;
   for (int i=0; i<m_nNumSources; ++i)
   {
      int thisStart = m_paSources[i]->m_nStreamStart;
      if (thisStart < masterStart)
         masterStart = thisStart;
   }

   m_nMasterStart = masterStart;
   return m_nMasterStart;
}

// private
HRESULT AVTarget::GetInputFormat(AM_MEDIA_TYPE *pFormatDefinition)
{
   HRESULT hr = S_FALSE;

   if (pFormatDefinition == NULL)
      return E_POINTER;

   for (int i=0; i<m_nNumSources && SUCCEEDED(hr); ++i)
   {
      if (m_paSources[i]->HasInputFile())
      {
         hr = m_paSources[i]->GetFormat(pFormatDefinition);
         break;
      }
   }

   return hr;
}
