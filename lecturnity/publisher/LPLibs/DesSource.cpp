#include "stdafx.h"
#include "DesSource.h"

CDesSource::CDesSource(void)
{
   m_bIsClipsOnly = false;
   m_bIsEmptyVideo = false;
   m_dClipRateMax = 0;
   m_dimClipsMax.x = 0;
   m_dimClipsMax.y = 0;
   m_dClipsMaxSecond = 0;
}

CDesSource::~CDesSource(void)
{
}

// private static
HRESULT CDesSource::FillInfo(CString& csMediaFile, CDesSourceMediaInfo *pTarget)
{
   if (pTarget == NULL)
      return E_POINTER;

   if (csMediaFile.IsEmpty())
      return E_INVALIDARG;

   if (_taccess(csMediaFile, 04) != 0) // does not exist / is not readable
      return E_INVALIDARG;


   HRESULT hr = S_OK;

   CComPtr<IMediaDet> pMediaDet;
   hr = ::CoCreateInstance(
      CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, IID_IMediaDet, (void **) &pMediaDet);
   
   if (SUCCEEDED(hr))
      hr = pMediaDet->put_Filename(_bstr_t(csMediaFile));

   double dLength = -1.0;
   if (SUCCEEDED(hr))
      hr = pMediaDet->get_StreamLength(&dLength);

   if (SUCCEEDED(hr))
      pTarget->m_rtMediaLength = (REFERENCE_TIME)(dLength * (__int64)(1000 * 10 * 1000));

   GUID guidMajorType;
   if (SUCCEEDED(hr))
      hr = pMediaDet->get_StreamType(&guidMajorType);

   if (SUCCEEDED(hr))
   {
      if (guidMajorType == MEDIATYPE_Audio)
      {
         pTarget->m_bIsAudio = true;

         /*
         AM_MEDIA_TYPE mtAudio;
         hr = pMediaDet->get_StreamMediaType(&mtAudio);
         if (SUCCEEDED(hr))
         {
            if (mtAudio.subtype		== MEDIASUBTYPE_PCM  
               &&	mtAudio.formattype	== FORMAT_WaveFormatEx)
            {
               WAVEFORMATEX *pAudioFormat = (WAVEFORMATEX *)mtAudio.pbFormat;
            }

            FreeMediaType(mtVideo);
         }
         */
      }
      else if (guidMajorType == MEDIATYPE_Video)
      {
         pTarget->m_bIsAudio = false;

         AM_MEDIA_TYPE mtVideo;
         hr = pMediaDet->get_StreamMediaType(&mtVideo);
         if (SUCCEEDED(hr))
         {
            VIDEOINFOHEADER *pVideoFormat = (VIDEOINFOHEADER *)mtVideo.pbFormat;

            pTarget->m_dimVideo.x = pVideoFormat->bmiHeader.biWidth;
            pTarget->m_dimVideo.y = pVideoFormat->bmiHeader.biHeight;

            FreeMediaType(mtVideo);
         }

         double dFps = -1.0;
         if (SUCCEEDED(hr))
            hr = pMediaDet->get_FrameRate(&dFps);

         //rtPerFrame = (__int64)((1000 / dFps) * 10 * 1000);

         if (SUCCEEDED(hr))
            pTarget->m_dFrameRate = dFps;

      }
      else
         return E_INVALIDARG;
   }

   if (SUCCEEDED(hr))
   {
      pTarget->m_csMediaSource = csMediaFile;
   }

   return hr;
}
   

HRESULT CDesSource::Init(CString& csMediaFile, REFERENCE_TIME rtOffset)
{
   m_MainInfo.m_rtMediaOffset = rtOffset;
   return FillInfo(csMediaFile, &m_MainInfo);
}

HRESULT CDesSource::InitClipsOnly()
{
    m_bIsClipsOnly = true;

    return S_OK;
}  

HRESULT CDesSource::InitEmptyVideo(int iOutputWidth, int iOutputHeight) {
    // Default frame rate for an empty video is 8
    // because a LEC/Ming generated SWF has always a frame rate of 8 (or lower)
    m_MainInfo.m_dFrameRate = 8.0;

    m_MainInfo.m_dimVideo.x = iOutputWidth;
    m_MainInfo.m_dimVideo.y = iOutputHeight;

    m_bIsEmptyVideo = true;

    return S_OK;
}

HRESULT CDesSource::AddClip(CString csClipFile, REFERENCE_TIME rtOffset)
{
   if (!m_bIsClipsOnly)
   {
      // if not clips only it must have a normal video before adding clips

      if (m_MainInfo.m_rtMediaLength <= 0 || m_MainInfo.m_bIsAudio)
         return E_UNEXPECTED;
   }

   if (csClipFile.IsEmpty())
      return E_INVALIDARG;

   if (_taccess(csClipFile, 04) != 0) // does not exist / is not readable
      return E_INVALIDARG;

   if (rtOffset < 0 || !m_bIsClipsOnly && rtOffset >= m_MainInfo.m_rtMediaLength)
      return E_INVALIDARG;

   CDesSourceMediaInfo info;
   info.m_rtMediaOffset = rtOffset;
   HRESULT hr = FillInfo(csClipFile, &info);

   if (SUCCEEDED(hr))
   {
      m_aClipInfos.Add(info);

      if (info.m_dFrameRate > m_dClipRateMax)
         m_dClipRateMax = info.m_dFrameRate;

      if (info.m_dimVideo.x > m_dimClipsMax.x)
         m_dimClipsMax.x = info.m_dimVideo.x;

      if (info.m_dimVideo.y > m_dimClipsMax.y)
         m_dimClipsMax.y = info.m_dimVideo.y;

      double dEndSecond = (info.m_rtMediaOffset + info.m_rtMediaLength) / (1000.0 * 10 * 1000);

      if (dEndSecond > m_dClipsMaxSecond)
         m_dClipsMaxSecond = dEndSecond;
   }

   return hr;
}

// private static
HRESULT CDesSource::AddToTrack(CDesSourceMediaInfo& sMediaInfo, int iOutputWidth, int iOutputHeight,
                               IAMTimelineTrack *pTrack, IAMTimeline *pTimeline)
{
   if (sMediaInfo.m_rtMediaLength <= 0)
      return E_INVALIDARG;

   if (pTrack == NULL || pTimeline == NULL)
      return E_POINTER;

   if (!sMediaInfo.m_bIsAudio && (iOutputWidth <= 0 || iOutputHeight <= 0))
      return E_INVALIDARG;

   HRESULT hr = S_OK;

   // SOURCE: Add a source to the track.
   CComPtr<IAMTimelineSrc> pSource;
   CComPtr<IAMTimelineObj> pSourceObj;
   if (SUCCEEDED(hr))
      hr = pTimeline->CreateEmptyNode(&pSourceObj, TIMELINE_MAJOR_TYPE_SOURCE);
   if (SUCCEEDED(hr))
      hr = pSourceObj->QueryInterface(IID_IAMTimelineSrc, (void **)&pSource);

   // Set the times and the file name.
   REFERENCE_TIME rtTimelineStart = 0;
   REFERENCE_TIME rtTimelineStop = sMediaInfo.m_rtMediaLength;

   REFERENCE_TIME rtMediaStart = 0;
   REFERENCE_TIME rtMediaStop = rtTimelineStop;

   if (sMediaInfo.m_rtMediaOffset >= 0)
   {
      // media starts later in the timeline

      rtTimelineStart += sMediaInfo.m_rtMediaOffset;
      rtTimelineStop += sMediaInfo.m_rtMediaOffset;
   }
   else
   {
      // take fewer (a later part) of the source media

      rtMediaStart += (-sMediaInfo.m_rtMediaOffset); // take fewer from input media
      rtTimelineStop -= (-sMediaInfo.m_rtMediaOffset); // total length gets shorter, too
   }

   if (SUCCEEDED(hr))
      hr = pSource->SetMediaName(_bstr_t(sMediaInfo.m_csMediaSource)); 
   if (SUCCEEDED(hr))
      hr = pSourceObj->SetStartStop(rtTimelineStart, rtTimelineStop);
   if (SUCCEEDED(hr))
      hr = pSource->SetMediaTimes(rtMediaStart, rtMediaStop);

   if (!sMediaInfo.m_bIsAudio)
   {
      int iClipWidth = sMediaInfo.m_dimVideo.x;
      int iClipHeight = sMediaInfo.m_dimVideo.y;

      if (iClipWidth <= iOutputWidth && iClipHeight <= iOutputHeight)
         hr = pSource->SetStretchMode(RESIZEF_CROP);
      else
         hr = pSource->SetStretchMode(RESIZEF_PRESERVEASPECTRATIO);
   }

   if (SUCCEEDED(hr))
      hr = pTrack->SrcAdd(pSourceObj);

   return hr;
}


HRESULT CDesSource::AddTo(IAMTimeline *pTimeline, int iOutputWidth, int iOutputHeight, 
                          double dExpectedLength, bool bDoPadding, 
                          int iDivisableWidth, int iDivisableHeight)
{
   if (pTimeline == NULL)
      return E_POINTER;

   if (!m_bIsClipsOnly && !m_bIsEmptyVideo && m_MainInfo.m_csMediaSource.IsEmpty()) // not successfully initialized
      return E_UNEXPECTED;

   if (m_bIsClipsOnly && m_aClipInfos.GetSize() <= 0)
      return E_UNEXPECTED;

   HRESULT hr = S_OK;

   bool bIsVideo = m_bIsEmptyVideo || m_bIsClipsOnly || !m_MainInfo.m_bIsAudio;

   if (bIsVideo)
   {
      // set default values if necessary
      if (iOutputWidth <= 0)
      {
         if (!m_bIsClipsOnly)
            iOutputWidth = m_MainInfo.m_dimVideo.x;
         else
            iOutputWidth = m_dimClipsMax.x;
      }
      if (iOutputHeight <= 0)
      {
         if (!m_bIsClipsOnly)
            iOutputHeight = m_MainInfo.m_dimVideo.y;
         else
            iOutputHeight = m_dimClipsMax.y;
      }

      // Workaround Bug #3786
      if (iOutputWidth > 1488 && iOutputWidth < 1512 && iOutputHeight > 1168 && iOutputHeight < 1280)
      {
         if (!bDoPadding)
            iOutputWidth = 1488;
         else
            iOutputWidth = 1512;

         // otherwise the H263Codec will crash
      }

      // Workaround Bug #5367
      if (iOutputWidth > 1239 && iOutputWidth < 1256 && iOutputHeight > 625 && iOutputHeight < 632)
      {
         if (!bDoPadding)
            iOutputWidth = 1236;
         else
            iOutputWidth = 1256;

         // otherwise the H263Codec will crash
      }

      if (bDoPadding && (iDivisableWidth > 1 || iDivisableHeight > 1)) {

          // Output size should be a multiple of iDivisableWidth x iDivisableHeight

          if (iDivisableWidth > 1 && iOutputWidth % iDivisableWidth != 0)
              iOutputWidth += (iDivisableWidth - (iOutputWidth % iDivisableWidth));
          if (iDivisableHeight > 1 && iOutputHeight % iDivisableHeight != 0)
              iOutputHeight += (iDivisableHeight - (iOutputHeight % iDivisableHeight));
      }
      // else if padding or cutting is necessary then
      // cutting will be done automatically by the video compressor (?)

   }

   // GROUP: Add a group to the timeline. 
   CComPtr<IAMTimelineGroup> pGroup;
   CComPtr<IAMTimelineObj> pGroupObj;
   if (SUCCEEDED(hr))
      hr = pTimeline->CreateEmptyNode(&pGroupObj, TIMELINE_MAJOR_TYPE_GROUP);
   if (SUCCEEDED(hr))
      hr = pGroupObj->QueryInterface(IID_IAMTimelineGroup, (void **)&pGroup);
   
   // Set the group media type. This example sets a simple type.
   // DES chooses other defaults.
   AM_MEDIA_TYPE mtGroup;  
   ZeroMemory(&mtGroup, sizeof(AM_MEDIA_TYPE));
   mtGroup.majortype = (m_bIsClipsOnly || !m_MainInfo.m_bIsAudio) ? MEDIATYPE_Video : MEDIATYPE_Audio;
   if (SUCCEEDED(hr))
      hr = pGroup->SetMediaType(&mtGroup);
   if (SUCCEEDED(hr))
      hr = pTimeline->AddGroup(pGroupObj);

   CComPtr<IAMTimelineComp> pComp;

   if (SUCCEEDED(hr))
      hr = pGroup->QueryInterface(IID_IAMTimelineComp, (void **)&pComp);
   
   if (dExpectedLength > 0)
   {
      // BUGFIX #4331: Add an empty (black) track with the desired length as lowest track

      double dCurrentLength = 0;
      if (SUCCEEDED(hr))
         hr = pTimeline->GetDuration2(&dCurrentLength);

      if (SUCCEEDED(hr) && dCurrentLength < dExpectedLength)
      {
         // Add empty object (no file) to create a timeline of the expected length

         CComPtr<IAMTimelineObj> pEmptyTrackObj;
         CComPtr<IAMTimelineTrack> pEmptyTrack;

         if (SUCCEEDED(hr))
            hr = pTimeline->CreateEmptyNode(&pEmptyTrackObj, TIMELINE_MAJOR_TYPE_TRACK);
         if (SUCCEEDED(hr))
            hr = pComp->VTrackInsBefore(pEmptyTrackObj, -1);
         if (SUCCEEDED(hr))
            hr = pEmptyTrackObj->QueryInterface(IID_IAMTimelineTrack, (void **)&pEmptyTrack);

         // SOURCE: Add a source to the track.
         CComPtr<IAMTimelineSrc> pSource;
         CComPtr<IAMTimelineObj> pSourceObj;
         if (SUCCEEDED(hr))
            hr = pTimeline->CreateEmptyNode(&pSourceObj, TIMELINE_MAJOR_TYPE_SOURCE);
         if (SUCCEEDED(hr))
            hr = pSourceObj->QueryInterface(IID_IAMTimelineSrc, (void **)&pSource);

         if (SUCCEEDED(hr))
            hr = pSourceObj->SetStartStop2(0, dExpectedLength);
         
         if (SUCCEEDED(hr))
            hr = pEmptyTrack->SrcAdd(pSourceObj);
      }
   }


   if (!m_bIsClipsOnly && !m_bIsEmptyVideo)
   {
      // TRACK: Add a track to the group. 
      CComPtr<IAMTimelineObj> pTrackObj;
      CComPtr<IAMTimelineTrack> pTrack;

      if (SUCCEEDED(hr))
         hr = pTimeline->CreateEmptyNode(&pTrackObj, TIMELINE_MAJOR_TYPE_TRACK);
      if (SUCCEEDED(hr))
         hr = pComp->VTrackInsBefore(pTrackObj, -1);
      if (SUCCEEDED(hr))
         hr = pTrackObj->QueryInterface(IID_IAMTimelineTrack, (void **)&pTrack);

      if (SUCCEEDED(hr))
         hr = AddToTrack(m_MainInfo, iOutputWidth, iOutputHeight, pTrack, pTimeline);
   }

   for (int i=0; i<m_aClipInfos.GetSize() && SUCCEEDED(hr); ++i)
   {
      // TRACK: Add a track to the group for each clip. 
      CComPtr<IAMTimelineObj> pClipTrackObj;
      CComPtr<IAMTimelineTrack> pClipTrack;

      if (SUCCEEDED(hr))
         hr = pTimeline->CreateEmptyNode(&pClipTrackObj, TIMELINE_MAJOR_TYPE_TRACK);
      if (SUCCEEDED(hr))
         hr = pComp->VTrackInsBefore(pClipTrackObj, -1);
      if (SUCCEEDED(hr))
         hr = pClipTrackObj->QueryInterface(IID_IAMTimelineTrack, (void **)&pClipTrack);

      if (SUCCEEDED(hr))
         hr = AddToTrack(m_aClipInfos[i], iOutputWidth, iOutputHeight, pClipTrack, pTimeline);
   }


   AM_MEDIA_TYPE mtGroupWith;
   if (SUCCEEDED(hr))
      hr = pGroup->GetMediaType(&mtGroupWith);

   if (SUCCEEDED(hr))
   {
      if (!bIsVideo)
      {
         if (mtGroupWith.subtype == MEDIASUBTYPE_PCM  
            &&	mtGroupWith.formattype == FORMAT_WaveFormatEx)
         {
            WAVEFORMATEX *pAudioFormat = (WAVEFORMATEX *)mtGroupWith.pbFormat;

            /*
            pAudioFormat->nChannels = 2;
            pAudioFormat->nSamplesPerSec = 22050;
            pAudioFormat->nBlockAlign = (pAudioFormat->wBitsPerSample / 8) * pAudioFormat->nChannels;
            pAudioFormat->nAvgBytesPerSec = pAudioFormat->nBlockAlign * pAudioFormat->nSamplesPerSec;
            hr = pGroup->SetMediaType(&mtGroupWith);
            */

            int iDummy = 0;
         }
      }
      else
      {
         double dFrameRate = GetTargetFrameRate();
         REFERENCE_TIME rtTimePerFrame = (__int64)((1000 / dFrameRate) * 10 * 1000);

         VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER *)mtGroupWith.pbFormat;
         pVideoHeader->AvgTimePerFrame = rtTimePerFrame;

         pVideoHeader->bmiHeader.biBitCount = 24;
         mtGroupWith.subtype = MEDIASUBTYPE_RGB24;
         pVideoHeader->bmiHeader.biSizeImage = iOutputWidth * iOutputHeight * 3;
         // otherwise 16 bit will be default

         pVideoHeader->bmiHeader.biWidth = iOutputWidth;
         pVideoHeader->bmiHeader.biHeight = iOutputHeight;
         
         hr = pGroup->SetMediaType(&mtGroupWith);

         if (SUCCEEDED(hr))
            pGroup->SetOutputFPS(dFrameRate);

         // Note
         // not setting the smart recompress format will crash with xvid and iv45 
         // upon rendering/running the graph (??)
         SCompFmt0 cFormat;
         ZeroMemory(&cFormat, sizeof SCompFmt0);
         CopyMediaType(&(cFormat.MediaType), &mtGroupWith);
         if (SUCCEEDED(hr))
            hr = pGroup->SetSmartRecompressFormat((long *) &cFormat);
         
         FreeMediaType(mtGroupWith);
      }
   }

   return hr;
}

double CDesSource::GetTargetFrameRate()
{
   double dFrameRate = 0;

   if (m_bIsClipsOnly)
      dFrameRate = m_dClipRateMax;
   else 
      dFrameRate = m_MainInfo.m_dFrameRate; // could be 0 if not initialized

   if (dFrameRate <= 2)
      dFrameRate = 2; // important for "MaxGop" in Video2Flv.cpp

   return dFrameRate;
}

int CDesSource::GetOutputWidth()
{
   if (m_bIsClipsOnly)
      return m_dimClipsMax.x;
   else 
      return m_MainInfo.m_dimVideo.x; // could be 0 if not initialized
}

int CDesSource::GetOutputHeight()
{
   if (m_bIsClipsOnly)
      return m_dimClipsMax.y;
   else 
      return m_MainInfo.m_dimVideo.y; // could be 0 if not initialized
}

double CDesSource::GetOutputLength()
{
   if (m_bIsClipsOnly)
      return m_dClipsMaxSecond;
   else
   {
      double dLength = 
         (m_MainInfo.m_rtMediaOffset + m_MainInfo.m_rtMediaLength) / (1000.0 * 10 * 1000);
      return dLength; // could be 0 if not initialized
   }
}