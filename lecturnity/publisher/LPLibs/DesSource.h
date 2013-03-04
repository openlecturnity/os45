#pragma once

class CDesSourceMediaInfo
{
public:
   CDesSourceMediaInfo()
   {
      m_rtMediaLength = -1;
      m_rtMediaOffset = 0;
      m_bIsAudio = false;
      m_dFrameRate = -1.0;
      m_dimVideo.x = 0;
      m_dimVideo.y = 0;
   }

   CString m_csMediaSource;
   REFERENCE_TIME m_rtMediaLength;
   REFERENCE_TIME m_rtMediaOffset;
   bool m_bIsAudio;
   double m_dFrameRate;
   POINT m_dimVideo;
};

class CDesSource
{
public:
   CDesSource(void);
   virtual ~CDesSource(void);

   HRESULT Init(CString& csMediaFile, REFERENCE_TIME rtOffset);
   HRESULT InitClipsOnly();
   HRESULT InitEmptyVideo(int iOutputWidth = 160, int iOutputHeight = 32);
   HRESULT AddClip(CString csClipFile, REFERENCE_TIME rtOffset);
   HRESULT AddTo(IAMTimeline *pTimeline, int iOutputWidth = 0, int iOutputHeight = 0, 
      double dExpectedLength = 0, bool bDoPadding = false, 
      int iDivisableWidth = 1, int iDivisableHeight = 1);

   double GetTargetFrameRate();
   int GetOutputWidth();
   int GetOutputHeight();
   double GetOutputLength();

private:
   CDesSourceMediaInfo m_MainInfo;
   CArray<CDesSourceMediaInfo, CDesSourceMediaInfo> m_aClipInfos;
   bool m_bIsClipsOnly;
   bool m_bIsEmptyVideo;
   POINT m_dimClipsMax;
   double m_dClipRateMax;
   double m_dClipsMaxSecond;

   static HRESULT FillInfo(CString& csMediaFile, CDesSourceMediaInfo *pTarget);
   static HRESULT AddToTrack(CDesSourceMediaInfo& sMediaInfo, 
      int iOutputWidth, int iOutputHeight,
      IAMTimelineTrack *pTrack, IAMTimeline *pTimeline);
};
