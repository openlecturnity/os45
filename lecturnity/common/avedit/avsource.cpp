#include "StdAfx.h"
#include "avedit.h"
#include "avinfo.h"

AVSource::AVSource(const _TCHAR *tszSourceFileName,
                   int nStreamStart, int nStreamEnd, 
                   int nSourceStart, int nSourceEnd, 
                   int nStreamSelector, int nStretchMode,
                   int nVolume, int nMainVolume, bool bAllowNullFile)
{
   if (!bAllowNullFile)
      _ASSERT(tszSourceFileName != NULL);

   if (tszSourceFileName != NULL)
   {
      _ASSERT(_taccess(tszSourceFileName, 04) == 0); // file exists and is readable

      m_tszSourceFileName = new _TCHAR[_tcslen(tszSourceFileName)+1];
      _tcscpy(m_tszSourceFileName, tszSourceFileName);
   }
   else
      m_tszSourceFileName = NULL;

   m_nStreamSelector = nStreamSelector;
   m_nStretchMode    = nStretchMode;
   m_nVolume = nVolume;
   m_nMainVolume = nMainVolume;

   int nStreamLength = -1;

   m_bIsAudio = true;
   bool bMediaTypeDetermined = false;
   if (nStreamEnd == AV_LENGTH_SOURCE || nSourceEnd == AV_LENGTH_SOURCE)
   {
      if (!bAllowNullFile)
      {
         AVInfo info;
         HRESULT hr = info.Initialize(m_tszSourceFileName);
         
         double dStreamLength;
         if (SUCCEEDED(hr))
            info.GetLength(&dStreamLength);
         
         if (SUCCEEDED(hr))
            nStreamLength = (int)(dStreamLength*1000);

         AM_MEDIA_TYPE mt;
         if (SUCCEEDED(hr))
             hr = GetFormat(&mt);

         if (SUCCEEDED(hr)) {
             m_bIsAudio = !(mt.majortype == MEDIATYPE_Video);
             bMediaTypeDetermined = true;
         }
         
         _ASSERT(nStreamLength > -1);
      }
      else
         _ASSERT(false); // no file: no length
   }

   // m_tszSourceFileName == NULL: is dummy audio
   if (!bMediaTypeDetermined && m_tszSourceFileName != NULL) {
       int iNameLength = _tcslen(m_tszSourceFileName);
       if (iNameLength > 4) {
           _TCHAR *tszSuffix = m_tszSourceFileName + iNameLength - 4;
           m_bIsAudio = _tcsicmp(tszSuffix, _T(".avi")) != 0;
           bMediaTypeDetermined = true;
       }
   }
    

   m_nStreamStart = nStreamStart;
   if (nStreamEnd == AV_LENGTH_SOURCE)
      m_nStreamEnd = m_nStreamStart+nStreamLength;
   else
      m_nStreamEnd = nStreamEnd;

   m_nSourceStart = nSourceStart;
   if (nSourceEnd == AV_LENGTH_SOURCE)
      m_nSourceEnd = nStreamLength;
   else
      m_nSourceEnd = nSourceEnd;
}

AVSource::~AVSource()
{
   if (m_tszSourceFileName != NULL)
      delete[] m_tszSourceFileName;
   m_tszSourceFileName = NULL;
}

AVSource* AVSource::DeepClone() const
{
   AVSource* clone = new AVSource(
      m_tszSourceFileName, m_nStreamStart, m_nStreamEnd,
      m_nSourceStart, m_nSourceEnd, 
      m_nStreamSelector, m_nStretchMode, m_nVolume, m_nMainVolume, true);
  
   return clone;
}

HRESULT AVSource::GetFormat(AM_MEDIA_TYPE *type) const
{
   if (m_tszSourceFileName != NULL)
   {
      AVInfo info;
      HRESULT hr = info.Initialize(m_tszSourceFileName);
      if (SUCCEEDED(hr))
         hr = info.GetStreamFormat(m_nStreamSelector, type);
   
      return hr;
   }
   else
      return S_FALSE;
}

HRESULT AVSource::AddTo(IAMTimelineTrack *pTrack, IAMTimeline *pTimeline, bool bFillBegin) const
{
   HRESULT hr = S_OK;

   if (bFillBegin && m_nStreamStart <= 0) {
       _ASSERT(false);
       return S_FALSE;
   } // else bFillBegin: Special case for Bug #5048

   // NOTE: Only setting/correcting the times here might not be enough as
   //       CVideoVideo shows and hides the video window whether there is a clip.
 
   CComPtr< IAMTimelineObj > pSourceObj;
   if (SUCCEEDED(hr))
   {
      hr = pTimeline->CreateEmptyNode(&pSourceObj, TIMELINE_MAJOR_TYPE_SOURCE);
   }
   CComQIPtr< IAMTimelineSrc, &IID_IAMTimelineSrc > pSource(pSourceObj);

   if (m_nVolume != 100 && m_tszSourceFileName != NULL)
   {
      hr = AVEdit::AddVolumeEffect(pSourceObj, pTimeline, m_nVolume, 0, m_nStreamEnd - m_nStreamStart);
      // m_nStreamEnd - m_nStreamStart: effects are relative to their media object
   }
   
   if (SUCCEEDED(hr) && m_tszSourceFileName != NULL)
   {
      hr = pSource->SetMediaName(_bstr_t(m_tszSourceFileName));
  
   }

   int iStreamStart = m_nStreamStart;
   int iStreamEnd = m_nStreamEnd;
   int iSourceStart = m_nSourceStart;
   int iSourceEnd = m_nSourceEnd;

   if (bFillBegin && iStreamStart > 0) {
       int diff = iStreamStart;
       iStreamEnd = diff;
       iStreamStart = 0;
       iSourceStart = 0;
       iSourceEnd = diff;
   }
    
   if (SUCCEEDED(hr))
   {
      hr = pSourceObj->SetStartStop(
         iStreamStart*10000i64, iStreamEnd*10000i64); 
      // time range that this object takes in the target stream (file)

   }

   if (SUCCEEDED(hr))
   {
      hr = pSource->SetMediaTimes(
         iSourceStart*10000i64, iSourceEnd*10000i64);
      // time range that is used from the source file
   }
 
   if (SUCCEEDED(hr))
   {
      hr = pSource->SetStretchMode(m_nStretchMode);
   }

   if (SUCCEEDED(hr))
   {
      hr = pTrack->SrcAdd(pSourceObj);
   }


   return hr;
}

bool AVSource::IsSameFile(AVSource *pOther) {
    if (pOther != NULL && m_tszSourceFileName != NULL && pOther->m_tszSourceFileName != NULL)
        return _tcscmp(m_tszSourceFileName, pOther->m_tszSourceFileName) == 0;

    return false;
}