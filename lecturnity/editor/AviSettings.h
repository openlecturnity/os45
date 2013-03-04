#ifndef __AVISETTINGS_H__67D60225_2132_4cbc_8B00_189603234381
#define __AVISETTINGS_H__67D60225_2132_4cbc_8B00_189603234381

#include "mtype.h" // BaseClasses

class CAviSettings
{
public:
   CAviSettings() : bIsVideoFormatInitialized(false), dwFCC(0), quality(75), keyframeRate(15), 
      bUseKeyframes(true), bUseSmartRecompression(false),
      bRememberAspectRatio(true),
      nAudioVolume(100), nOriginalAudioVolume(100),
      nOriginalWidth(0), nOriginalHeight(0), dOriginalFps(1.0), bHasOriginalSize(false)
   {
      ZeroMemory(&videoFormat, sizeof AM_MEDIA_TYPE);
   }

   virtual ~CAviSettings()
   {
      ReleaseMemory();
   }

   virtual void ReleaseMemory()
   {
      if (bIsVideoFormatInitialized)
      {
         if (videoFormat.pbFormat != NULL && videoFormat.cbFormat != 0)
            FreeMediaType(videoFormat);
         videoFormat.pbFormat = NULL;
         videoFormat.cbFormat = 0;
         videoFormat.formattype = GUID_NULL;
      }
   }

   virtual void Clone(CAviSettings *pOther)
   {
      if (NULL == pOther)
         return;

      ReleaseMemory();

      // Copy entire structure (done normally/automatically by 
      // c++ with "="); this will not copy the format block
      // of the AM_MEDIA_TYPE structure.
      *this = *pOther;

      // That's why we have to do that here
      if (pOther->bIsVideoFormatInitialized && pOther->videoFormat.pbFormat != NULL)
      {
         CopyMediaType(&this->videoFormat, &pOther->videoFormat);
      }
   }

   // The export video format
   AM_MEDIA_TYPE videoFormat;
   // True if export video format has been initialized
   bool bIsVideoFormatInitialized;

   // The FCC code of the export video codec
   DWORD dwFCC;
   // The quality setting for the codec, 0 to 100 percent.
   int   quality;
   // The key frame rate; only important if m_videoExportUseKeyframes
   // is set to true
   int   keyframeRate;
   // True if keyframes should be used
   bool  bUseKeyframes;
//   bool  m_videoExportUseQuality;
   bool bUseSmartRecompression;
   //bool m_clipExportUseSmartRecompression;
   //bool m_clipExportStretchDenverClips;

   bool bRememberAspectRatio;

   int nOriginalWidth;
   int nOriginalHeight;
   double dOriginalFps;
   bool bHasOriginalSize;

   //bool bHasAudio;
   //CString csAudioSourceFile;
   //int  nAudioStreamId;
   int  nAudioVolume;

   // The volume for the underlying original audio to use
   int  nOriginalAudioVolume;
};

#endif
