#include "stdafx.h"

#include <math.h>
#include <io.h>
#include <stdio.h>

#include "FlashEngine.h"
#include "FlashEngineHelper.h"
#include "imc_lecturnity_converter_LPLibs.h"

#include "KeyGenerator.h"

#include "lutils.h"

// --------------------------------------------------------------------------
// Simple geometric shapes
// --------------------------------------------------------------------------

// Global variables
TCHAR *szGlobalVariables = _T("\
      var g_nFlashVersion;\
      var g_bIsAlreadyLoaded;\
      var g_bIsEvaluation;\
      \
      var g_Sound;\
      var g_bIsSoundOn;\
      var g_nVolume;\
      var g_mousePos;\
      \
      var g_bIsPreloading;\
      var g_bIsSwfLoaded;\
      var g_bIsPlaying;\
      var g_bWasPlaying;\
      var g_bWillBePlaying;\
      var g_bIsInteractivePaused;\
      var g_bIsTimeSliding;\
      var g_bTimeSlidingPerformed;\
      var g_bIsVolumeSliding;\
      var g_bControlIsDisabled;\
      var g_bMessageBoxVisible;\
      var g_nPreviousFeedbackFrame;\
      \
      var g_bIsTooltipOn;\
      var g_tooltipStartInterval;\
      var g_tooltipStopInterval;\
      \
      var g_fSynchronizationDelta;\
      var g_fVideoSynchronizationDelta;\
      var g_fClipSynchronizationDelta;\
      \
      var g_updateInterval;\
      var g_preloadInterval;\
      var g_synchronizationInterval;\
      var g_videoSynchronizationInterval;\
      var g_clipSynchronizationInterval;\
      var g_startDelayedPlayInterval;\
      var g_fFrameRate;\
      var g_bIsSlidestar;\
      var g_bUseFlashInterface;\
      var g_bIsExternJumpCall;\
      \
      var g_nStopmarks;\
      var g_nLastStopmarkFrame;\
      var g_aStopmarkFrames;\
      var g_bIgnoreStopmark;\
      \
      var g_bVideoClipsLoaded;");

TCHAR *szPageStreamVariables = _T("\
      var g_bMovieHasPages;\
      \
      var g_nPages;\
      var g_aPagingFrames;\
      var g_aVisitedPages;");


TCHAR *szLayoutVariables = _T("\
      var g_dGlobalScale;\
      var g_nSlidesWidth;\
      var g_nSlidesHeight;\
      var g_nSlidesOffsetX;\
      var g_nSlidesOffsetY;");

TCHAR *szPreloadVariables = _T("\
       var g_strPreload;\
       var g_strPreloadSlidestar;\
       var g_strSlide;\
       var g_strOf;\
       var g_strWarning;\
       var g_strError;\
       var g_strFlvNotFoundError;\
       var g_strFlvStreamingError;\
       var g_strServerError;\
       var g_strMsgType;\
       var g_strMsgText;\
       var g_strMsgTypeTemp;\
       var g_strMsgTextTemp;\
       var g_MessageBoxId;\
       ");

TCHAR *szNavigationVariables = _T("\
      var g_bIsNavigationChecked ;\
      var g_nNavigationControlBar;\
      var g_nNavigationStandardButtons;\
      var g_nNavigationTimeLine;\
      var g_nNavigationTimeDisplay;\
      var g_nNavigationDocumentStructure;\
      var g_nNavigationPluginContextMenu;\
      var g_bIsAutostartMode;");

TCHAR *szVideoAndClipVariables = _T("\
      var g_bMovieHasVideo;\
      var g_VideoStream;\
      var g_bVideoIsBuffering;\
      var g_sVideoStreamUrl;\
      var g_fVideoStreamLengthSec;\
      \
      var g_bufferingInterval;\
      var g_bIsTrueStreaming;\
      var g_nStartBufferLength;\
      var g_nExpandedBufferLength;\
      var g_nVideoBufferTries;\
      var g_nVideoSeekTries;\
      var g_NetConnection;\
      \
      var g_bMovieHasClips;\
      var g_iNumOfClips;\
      var g_CurrentClipStream;\
      var g_CurrentClipStreamUrl;\
      var g_CurrentClipOffset;\
      var g_CurrentClipLength;\
      var g_iActiveClipNumber;\
      var g_aClipBufferTries;\
      var g_aClipStreams;\
      var g_aClipStreamUrls;\
      var g_aClipBeginTimesSec;\
      var g_aClipEndTimesSec;\
      var g_aClipSprites;\
      var g_bClipIsBuffering;\
      var g_nClipIndex;\
      var g_nLastClipSeekTimeMs;\
      \
      var g_videoBufferInterval;\
      var g_aClipBufferInterval;");

TCHAR *szTestingVariables = _T("\
      var g_bHasTesting;\
      var g_isRandomTest;\
      var g_randomQuestionOrder;\
      var g_relativePassPercentage;\
      var g_firstQuestionTimestamp;\
      var g_JumpRandomFrame;\
      var g_isRandomInitialized;\
      var g_doScInitRandom;\
      var g_nPreviousFrame;\
      var g_bFeedbackMessageVisible;\
      var g_nFeedbackIndex;\
      var g_nFeedbackIndexTemp;\
      var g_nFeedbackTestingIndex;\
      var g_nQuestionnaireIndex;\
      var g_bIsQuestionnaire;\
      var g_bIsQuestionnaireTemp;\
      var g_bAllQuestionsAnswered;\
      var g_bQuestionIsDeactivated;\
      var g_timeoutInterval;\
      var g_FeedbackId;\
      var g_QuestionnaireId;\
      var g_nTotalCurrentlyAchievePoints;\
      var g_interactionType;\
      var g_questionFillIndex;\
      var g_questionDragIndex;\
      var g_MajorStrSeparator;\
      var g_MinorStrSeparator;\
      var g_Responses;\
      var g_currPage;");

TCHAR *szScormVariables = _T("\
      var vector;\
      var count;\
      var g_interactionLearnerResponse;\
      var g_interactionPattern;\
      var g_questionFillIndex;\
      var g_bDoLoadLocationData;\
      var g_resumePos;\
      var g_suspendData;\
      var g_ScopeStrSeparator;\
      var g_bDoLoadSuspendData;\
      var g_bDoLoadLearnerData;\
      var g_bDoSaveSuspendData;\
      var g_bDoSaveSuccessStatus;\
      var g_bDoSaveCompletionStatus;\
      var g_bSuspendDataLoaded;\
      var g_bLearnerDataLoaded;\
      var g_LMSResumeSupported;\
      var g_learnerData;\
      var g_questionIndex;\
      var g_learnerResponse;\
      var g_questionResult;\
      var g_Temp;\
      var g_locationTimeStamp;\
      var g_bScormEnabled;\
	  var g_nSessionTime;\
	  var g_successStatus;\
	  var g_completionStatus;");
TCHAR *szAllowedUrlCharacters = _T("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-._~!?%@$&+,;=() :/\\");

int round(float f) 
{ 
   return (int)(f + 0.5f); 
}

int floorToTwo(float f)
{
   int i = (int)f;
   int s = 0;
   for (; s<32 && (i >> s) > 0; ++s);

   if (s > 0)
      return 1 << (s-1);
   else
      return 0;
}

void CFlashEngine::DrawLine(SWFShape *pShape, float x, float y, float width, float height, 
                            float fLineWidth, bool bDashedLine, bool bDottedDash, 
                            bool bDrawHairline)
{
   if (bDrawHairline && (fLineWidth > 1.5f))
      pShape->setLine(0.75f, (m_argbStd >> 16) & 0xff, 
         (m_argbStd >> 8) & 0xff, (m_argbStd >> 0) & 0xff, (m_argbStd >> 24) & 0xff);

   float dx = x+width-x, dy = y+height-y;

   pShape->movePenTo(x, y);
   if (!bDashedLine)
   {
      pShape->drawLine(width, height);
   }
   else
   {
      float fDashLength = 3.0f;
      float fSpaceLength = 1.0f;
      if (bDottedDash)
         fDashLength = 1.2f;

      if (fLineWidth < 2)
         fDashLength += 1.0f; // in order to avoid them lines disappearing (too faint)


      // Bugfix #2294
      // lines with width 0 have no "line advancement" -> endless loop
      //
      // Anyway this algorithm seems to need at least a line width of 1.0 to 
      // produce good looking results.
      if (fLineWidth < 1.0f)
         fLineWidth = 1.0f;
      

      float fLineLength = sqrt(dx*dx + dy*dy);
      float fFactor = fLineWidth/fLineLength;
         
      float fLinePosition = 0;
      while (fLinePosition < fLineLength)
      {
         float fActualDashLength = fDashLength-1.0f;//min(fDashLength-1.0f, fLineLength-fLinePosition-1.0f);
         
         float fPosAdvanceX = fActualDashLength*fFactor*dx;
         float fPosAdvanceY = fActualDashLength*fFactor*dy;
         float fLineAdvancement = sqrt(fPosAdvanceX*fPosAdvanceX + fPosAdvanceY*fPosAdvanceY);

         if (fLineAdvancement > fLineLength-fLinePosition)
            fActualDashLength *= (fLineLength-fLinePosition)/fLineAdvancement;

         if (fActualDashLength <= 0.2f)
            fActualDashLength = 0.2f;
         float fActualSpaceLength = fSpaceLength+1.0f;
         
         pShape->drawLine(fActualDashLength*fFactor*dx, fActualDashLength*fFactor*dy);
         pShape->movePen(fActualSpaceLength*fFactor*dx, fActualSpaceLength*fFactor*dy);
         
         fPosAdvanceX = fActualDashLength*fFactor*dx + fActualSpaceLength*fFactor*dx;
         fPosAdvanceY = fActualDashLength*fFactor*dy + fActualSpaceLength*fFactor*dy;
         
         fLinePosition += sqrt(fPosAdvanceX*fPosAdvanceX + fPosAdvanceY*fPosAdvanceY);
      }
      
   }

}

void CFlashEngine::DrawLineTo(SWFShape *pShape, float x1, float y1, float x2, float y2, 
                              float fLineWidth, bool bDashedLine, bool bDottedDash, 
                              bool bDrawHairline)
{
   DrawLine(pShape, x1, y1, x2-x1, y2-y1, fLineWidth, bDashedLine, bDottedDash, bDrawHairline);
}

void CFlashEngine::DrawBroadenLine(SWFShape *pShape, float x, float y, float width, float height, 
                                   float fLineWidth)
{
   // Draw lines with angular edging

   // Normalized direction vector
   double dX = (double)width;
   double dY = (double)height;
   double dShortenF = (double)1.0f / sqrt(dX * dX + dY * dY);
   double dXn = dShortenF * dX;
   double dYn = dShortenF * dY;
   // consider line width (and thickness of finest line style)
   dXn *= ((0.5f * fLineWidth) - 0.5f);
   dYn *= ((0.5f * fLineWidth) - 0.5f);

   // Define 4 rectangle points
   double dPoint1X, dPoint2X, dPoint3X, dPoint4X;
   double dPoint1Y, dPoint2Y, dPoint3Y, dPoint4Y;

   dPoint1X = x - dXn - dYn;
   dPoint1Y = y - dYn + dXn;
   dPoint2X = x - dXn + dYn;
   dPoint2Y = y - dYn - dXn;
   dPoint3X = (x + width) + dXn + dYn;
   dPoint3Y = (y + height) + dYn - dXn;
   dPoint4X = (x + width) + dXn - dYn;
   dPoint4Y = (y + height) + dYn + dXn;

   // Use finest linestyle
   pShape->setLine(0.75f, (m_argbStd >> 16) & 0xff, 
      (m_argbStd >> 8) & 0xff, (m_argbStd >> 0) & 0xff, (m_argbStd >> 24) & 0xff);
   // Set fill color
   SWFFill *pFill = pShape->addSolidFill((m_argbStd >> 16) & 0xff, 
      (m_argbStd >> 8) & 0xff, (m_argbStd >> 0) & 0xff, (m_argbStd >> 24) & 0xff);
   pShape->setRightFill(pFill);

//   // Original line (only for testing purposes)
//   pShape->movePenTo(x, y);
//   pShape->drawLineTo(x+width, y+height);

   pShape->movePenTo(dPoint1X, dPoint1Y);
   pShape->drawLineTo(dPoint4X, dPoint4Y);
   pShape->drawLineTo(dPoint3X, dPoint3Y);
   pShape->drawLineTo(dPoint2X, dPoint2Y);
   pShape->drawLineTo(dPoint1X, dPoint1Y); // closed polygon
}

void CFlashEngine::DrawRectangle(SWFShape *pShape, float x, float y, float width, float height,
                                 float fLineWidth, bool bDashedLine, bool bDottedDash, 
                                 bool bDrawHairline)
{
   DrawLine(pShape, x, y, width, 0, fLineWidth, bDashedLine, bDottedDash, bDrawHairline);
   DrawLine(pShape, x+width, y, 0, height, fLineWidth, bDashedLine, bDottedDash, bDrawHairline);
   DrawLine(pShape, x+width, y+height, -width, 0, fLineWidth, bDashedLine, bDottedDash, bDrawHairline);
   DrawLine(pShape, x, y+height, 0, -height, fLineWidth, bDashedLine, bDottedDash, bDrawHairline);
}

void CFlashEngine::DrawPolygon(SWFShape *pShape, DrawSdk::DPoint* pPoints, int iPointCount,
      float fLineWidth, bool bDashedLine, bool bDottedDash, bool bDrawHairline)
{
    for(int i = 0; i < iPointCount - 1; i ++)
    {
        DrawLineTo(pShape, pPoints[i].x, pPoints[i].y, pPoints[i + 1].x, pPoints[i + 1].y, fLineWidth, bDashedLine, bDottedDash, bDrawHairline);
    }
    DrawLineTo(pShape, pPoints[iPointCount -1].x, pPoints[iPointCount - 1].y, pPoints[0].x, pPoints[0].y, fLineWidth, bDashedLine, bDottedDash, bDrawHairline);
}

void CFlashEngine::DrawOval(SWFShape *pShape, float x, float y, float width, float height,
                            float fLineWidth, bool bDashedLine, bool bDottedDash, 
                            bool bDrawHairline)
{
   if ((bDashedLine || bDottedDash) && bDrawHairline && (fLineWidth > 1.5f))
   {
      pShape->setLine(0.75f, (m_argbStd >> 16) & 0xff, 
         (m_argbStd >> 8) & 0xff, (m_argbStd >> 0) & 0xff, (m_argbStd >> 24) & 0xff);
   }

   width = width / 2.0f;
   height = height / 2.0f;
   x = x + width;
   y = y + height;

   float fExtent = 2.0f*PI*((width+height)/2.0f);
   int iDivisor = 4;
   if (fLineWidth > 0.0f)
   {
      float fMinLineWidth = fLineWidth;
      // these are "magic" values/calculations, they seem to produce reasonable results
      if (fMinLineWidth < 4.0f)
         fMinLineWidth = 4.0f; // otherwise points/lines are too tight together

      float fRawDivisor = fExtent/(4.0f*fMinLineWidth); 
      iDivisor = (int)(fRawDivisor);
      if (iDivisor <= 1)
         iDivisor = 2; // otherwise xrCtrl and yrCtrl get very very large

      if (bDottedDash && (fLineWidth < 4.0f))
      {
         iDivisor *= 1.25f + 1.0f/fLineWidth; // this works! result is the correct integer value
      }
   }

   float theta = PI / (float)iDivisor;
   float xrCtrl = width / cos(theta / 2.0f);
   float yrCtrl = height / cos(theta / 2.0f);
   float angle = 0.0f;
   float angleMid = 0.0f;

   bool bSwitch = true;
   float fScaleFactor = 0.05f;

   pShape->movePenTo(x + width, y);
   for (int i = 0; i < iDivisor*2; ++i)
   {
      angle += theta;

      angleMid = angle - (theta / 2.0f);
      float cx = x + cos(angleMid) * xrCtrl;
      float cy = y + sin(angleMid) * yrCtrl;
      float px = x + cos(angle) * width;
      float py = y + sin(angle) * height;
    
      if (!bDottedDash)
      {
         if (bSwitch)
            pShape->drawCurveTo(cx, cy, px, py); // this is the normal case for non-dashed lines
         else
            pShape->movePenTo(px, py);
      }
      else
      {
         pShape->movePenTo(px, py);
      
         float px2 = x + cos(angle+0.01f*theta) * width;
         float py2 = y + sin(angle+0.01f*theta) * height;
         float dx = px2-px;
         float dy = py2-py;
         if ((dx != 0.0f) && (dy != 0.0f))
            fScaleFactor = 1.0f / (sqrt(dx*dx + dy*dy));
         pShape->drawLine(fScaleFactor*dx, fScaleFactor*dy);
      }

      if (bDashedLine)
         bSwitch = !bSwitch;
   }
}

void CFlashEngine::DrawPptLine(SWFSprite *pSprParent, float x, float y, float width, float height, 
                               float fLineWidth, bool bDashedLine, bool bDottedDash)
{
   float dx = x+width-x, dy = y+height-y;

   SWFShape *pShape = NULL;

   if (!bDashedLine)
   {
      pShape = new SWFShape();
      pShape->setLine(fLineWidth, (m_argbStd >> 16) & 0xff, 
         (m_argbStd >> 8) & 0xff, (m_argbStd >> 0) & 0xff, (m_argbStd >> 24) & 0xff);
      pShape->movePenTo(x, y);

      if (fLineWidth < 1.5f)
         pShape->drawLine(width, height);
      else
         // Draw lines with angular edging
         DrawBroadenLine(pShape, x, y, width, height, fLineWidth);

      pSprParent->add(pShape);
   }
   else
   {
      float fDashLength = 3.0f;
      float fSpaceLength = 1.0f;
      if (bDottedDash)
         fDashLength = 1.2f;

      if (fLineWidth < 2)
         fDashLength += 1.0f; // in order to avoid them lines disappearing (too faint)


      // Bugfix #2294
      // lines with width 0 have no "line advancement" -> endless loop
      //
      // Anyway this algorithm seems to need at least a line width of 1.0 to 
      // produce good looking results.
      if (fLineWidth < 1.0f)
         fLineWidth = 1.0f;
      

      float fLineLength = sqrt(dx*dx + dy*dy);
      float fFactor = fLineWidth/fLineLength;
         
      float fLinePosition = 0;
      float fPosX = x;
      float fPosY = y;
      while (fLinePosition < fLineLength)
      {
         pShape = new SWFShape();
         pShape->setLine(fLineWidth, (m_argbStd >> 16) & 0xff, 
            (m_argbStd >> 8) & 0xff, (m_argbStd >> 0) & 0xff, (m_argbStd >> 24) & 0xff);

         float fActualDashLength = fDashLength-1.0f;//min(fDashLength-1.0f, fLineLength-fLinePosition-1.0f);
         
         float fPosAdvanceX = fActualDashLength*fFactor*dx;
         float fPosAdvanceY = fActualDashLength*fFactor*dy;
         float fLineAdvancement = sqrt(fPosAdvanceX*fPosAdvanceX + fPosAdvanceY*fPosAdvanceY);

         if (fLineAdvancement > fLineLength-fLinePosition)
            fActualDashLength *= (fLineLength-fLinePosition)/fLineAdvancement;

         if (fActualDashLength <= 0.2f)
            fActualDashLength = 0.2f;
         float fActualSpaceLength = fSpaceLength+1.0f;
         
         if (fLineWidth < 1.5f)
         {
            pShape->movePenTo(fPosX, fPosY);
            pShape->drawLine(fActualDashLength*fFactor*dx, fActualDashLength*fFactor*dy);
         }
         else
            // Draw lines with angular edging
            DrawBroadenLine(pShape, fPosX, fPosY, fActualDashLength*fFactor*dx, fActualDashLength*fFactor*dy, fLineWidth);

         //pShape->movePen(fActualSpaceLength*fFactor*dx, fActualSpaceLength*fFactor*dy);
         
         fPosAdvanceX = fActualDashLength*fFactor*dx + fActualSpaceLength*fFactor*dx;
         fPosAdvanceY = fActualDashLength*fFactor*dy + fActualSpaceLength*fFactor*dy;
         fPosX += fPosAdvanceX;
         fPosY += fPosAdvanceY;
         
         fLinePosition += sqrt(fPosAdvanceX*fPosAdvanceX + fPosAdvanceY*fPosAdvanceY);

         pSprParent->add(pShape);
      }
   }

//   pSprParent->add(pShape);
}

void CFlashEngine::DrawPptLineTo(SWFSprite *pSprParent, float x1, float y1, float x2, float y2, 
                                 float fLineWidth, bool bDashedLine, bool bDottedDash)
{
   DrawPptLine(pSprParent, x1, y1, x2-x1, y2-y1, fLineWidth, bDashedLine, bDottedDash);
}

void CFlashEngine::DrawPptRectangle(SWFSprite *pSprParent, SWFShape *pShape, 
                                    float x, float y, float width, float height,
                                    float fLineWidth, bool bDashedLine, bool bDottedDash)
{
   // Draw the shape with a hairline
   DrawRectangle(pShape, x, y, width, height, fLineWidth, bDashedLine, bDottedDash, true);
   pSprParent->add(pShape);

   if (fLineWidth > 1.5f)
   {
      DrawPptLine(pSprParent, x, y, width, 0, fLineWidth, bDashedLine, bDottedDash);
      DrawPptLine(pSprParent, x+width, y, 0, height, fLineWidth, bDashedLine, bDottedDash);
      DrawPptLine(pSprParent, x+width, y+height, -width, 0, fLineWidth, bDashedLine, bDottedDash);
      DrawPptLine(pSprParent, x, y+height, 0, -height, fLineWidth, bDashedLine, bDottedDash);
   }
}

void CFlashEngine::DrawPptOval(SWFSprite *pSprParent, SWFShape *pShape, 
                               float x, float y, float width, float height,
                               float fLineWidth, bool bDashedLine, bool bDottedDash)
{
   // Draw the shape with a hairline
   DrawOval(pShape, x, y, width, height, fLineWidth, bDashedLine, bDottedDash, true);
   pSprParent->add(pShape);

   if (bDashedLine && (fLineWidth > 1.5f))
   {
      width = width / 2.0f;
      height = height / 2.0f;
      x = x + width;
      y = y + height;

      float fExtent = 2.0f*PI*((width+height)/2.0f);
      int iDivisor = 4;
      if (fLineWidth > 0.0f)
      {
         float fMinLineWidth = fLineWidth;
         // these are "magic" values/calculations, they seem to produce reasonable results
         if (fMinLineWidth < 4.0f)
            fMinLineWidth = 4.0f; // otherwise points/lines are too tight together

         float fRawDivisor = fExtent/(4.0f*fMinLineWidth); 
         iDivisor = (int)(fRawDivisor);
         if (iDivisor <= 1)
            iDivisor = 2; // otherwise xrCtrl and yrCtrl get very very large

         if (bDottedDash && (fLineWidth < 4.0f))
         {
            iDivisor *= 1.25f + 1.0f/fLineWidth; // this works! result is the correct integer value
         }
      }

      float theta = PI / (float)iDivisor;
      float xrCtrl = width / cos(theta / 2.0f);
      float yrCtrl = height / cos(theta / 2.0f);
      float angle = 0.0f;
      float angleMid = 0.0f;

      bool bSwitch = false;
      float fScaleFactor = 0.05f;

      for (int i = 0; i < iDivisor*2; ++i)
      {
         angle += theta;

         angleMid = angle - (theta / 2.0f);
         float cx = x + cos(angleMid) * xrCtrl;
         float cy = y + sin(angleMid) * yrCtrl;
         float px = x + cos(angle) * width;
         float py = y + sin(angle) * height;

         float px2 = x + cos(angle+0.01f*theta) * width;
         float py2 = y + sin(angle+0.01f*theta) * height;
         float dx = px2-px;
         float dy = py2-py;
         if ((dx != 0.0f) && (dy != 0.0f))
            fScaleFactor = 1.0f / (sqrt(dx*dx + dy*dy));
         DrawPptLine(pSprParent, px, py, fScaleFactor*dx, fScaleFactor*dy, fLineWidth, false, false);

         if (bSwitch && !bDottedDash)
         {
            // simulate curve by 20 rectangles
            float dAngle = angle;
            float dTheta = (theta / 20.0f);
            for (int k = 0; k < 20; ++k)
            {
               dAngle += dTheta;

               float px = x + cos(dAngle) * width;
               float py = y + sin(dAngle) * height;

               float px2 = x + cos(dAngle+0.01f*theta) * width;
               float py2 = y + sin(dAngle+0.01f*theta) * height;
               float dx = px2-px;
               float dy = py2-py;
               if ((dx != 0.0f) && (dy != 0.0f))
                  fScaleFactor = 1.0f / (sqrt(dx*dx + dy*dy));
               DrawPptLine(pSprParent, px, py, fScaleFactor*dx, fScaleFactor*dy, fLineWidth, false, false);
            }
         }

         bSwitch = !bSwitch;
      }
   }
}

void CFlashEngine::DrawLineAndArrowHeads(SWFSprite *pSprParent, 
                   int nArrowStyle, int nArrowConfig, 
                   float x, float y, float dx, float dy, 
                   float fLineWidth, bool bDashedLine, bool bDottedDash, Gdiplus::ARGB fcref)
{
   // Resolve nArrowConfig into its components
   int nBeginType = nArrowConfig / 1000;
   int nBeginSize = (nArrowConfig % 1000) / 100;
   int nEndType   = (nArrowConfig % 100) / 10;
   int nEndSize   = (nArrowConfig % 10);
   //_tprintf(_T("- arrow style: %d - arrow config: %d --> %d %d %d %d\n"), nArrowStyle, nArrowConfig, nBeginType, nBeginSize, nEndType, nEndSize);


   // Normalized direction vector
   double dX = (double)dx;
   double dY = (double)dy;
   double dShortenF = (double)0.0f;
   if (! ((dX == (double)0.0f) && (dY == (double)0.0f)) )
      dShortenF = (double)1.0f / sqrt((dX * dX) + (dY * dY));
   double dXn = dShortenF * dX;
   double dYn = dShortenF * dY;
   

   // Before drawing line and arrow heads:
   // Check for consistency between nArrowStyle and nArrowConfig
   CheckForArrowHeadConsistency(nArrowStyle, nBeginType, nBeginSize, nEndType, nEndSize);

   // Before drawing the line, do some linewidth corrections, if necessary
   // Define 2 shorten multipliers:
   double dShortenMultiplierEnd   = (double)2.0f;
   double dShortenMultiplierBegin = (double)2.0f;

   if (1 == (nEndSize % 3)) // 1, 4, 7
      dShortenMultiplierEnd = (double)1.5f;
   else if (0 == (nEndSize % 3)) // 3, 6, 9
      dShortenMultiplierEnd = (double)3.0f;
   if (1 == (nBeginSize % 3)) // 1, 4, 7
      dShortenMultiplierBegin = (double)1.5f;
   else if (0 == (nBeginSize % 3)) // 3, 6, 9
      dShortenMultiplierBegin = (double)3.0f;

   // The line does not need to be shortened for type 4 and 5
   if (4 == nEndType || 5 == nEndType)
      dShortenMultiplierEnd = (double)0.0f;
   if (4 == nBeginType || 5 == nBeginType)
      dShortenMultiplierBegin = (double)0.0f;

   // Define corrected values for x, y, dx, dy
   float xCorr = x;
   float yCorr = y;
   float dxCorr = dx;
   float dyCorr = dy;

   if (nArrowStyle == 1 || nArrowStyle == 3) // end of line
   {
      dxCorr -= (float)(dShortenMultiplierEnd*fLineWidth*dXn);
      dyCorr -= (float)(dShortenMultiplierEnd*fLineWidth*dYn);
   }

   if (nArrowStyle == 2 || nArrowStyle == 3) // begin of line
   {
      dxCorr -= (float)(dShortenMultiplierBegin*fLineWidth*dXn);
      dyCorr -= (float)(dShortenMultiplierBegin*fLineWidth*dYn);

      // Shift position of start point.
      xCorr += (float)(dShortenMultiplierBegin*fLineWidth*dXn);
      yCorr += (float)(dShortenMultiplierBegin*fLineWidth*dYn);
   }


   // Draw the line
   DrawPptLine(pSprParent, xCorr, yCorr, dxCorr, dyCorr, fLineWidth, bDashedLine, bDottedDash);
        

   // Now the arrow heads:
   // Check arrow style again
   switch (nArrowStyle)
   {
      case 0:  // simple line
         break;
      case 1:  // arrow at end of line
         m_pShpArrowEnd = new SWFShape();
         DrawArrowHead(m_pShpArrowEnd, nEndType, nEndSize, x+dx, y+dy, -dXn, -dYn, fLineWidth, fcref);
         break;

      case 2:  // arrow at begin of line
         m_pShpArrowBegin = new SWFShape();
         DrawArrowHead(m_pShpArrowBegin, nBeginType, nBeginSize, x, y, dXn, dYn, fLineWidth, fcref);
         break;

      case 3:  // arrow at begin and end of line
         m_pShpArrowEnd = new SWFShape();
         m_pShpArrowBegin = new SWFShape();
         DrawArrowHead(m_pShpArrowEnd, nEndType, nEndSize, x+dx, y+dy, -dXn, -dYn, fLineWidth, fcref);
         DrawArrowHead(m_pShpArrowBegin, nBeginType, nBeginSize, x, y, dXn, dYn, fLineWidth, fcref);
         break;
   }
}

void CFlashEngine::DrawArrowHead(SWFShape *pShape, int nArrowType, int nArrowSize, 
      float x, float y, double dXn, double dYn, float fLineWidth, Gdiplus::ARGB fcref)
{
   // Use a minimum line width
   if (fLineWidth < 1.6f)
   {
      if (nArrowType != 2)
         fLineWidth = 1.6f;
      else
         fLineWidth = 1.0f;
   }

   // Use finest linestyle
   pShape->setLine(0.75f, (fcref >> 16) & 0xff, 
      (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);

   // 4th point (for arrow type 2 and 3)
   double dSubtract = (double)1.0f;
   // Arrow head length/width (default values)
   double dLength = (double)(3.0f * fLineWidth);
   double dWidth  = (double)(3.0f * fLineWidth);

   switch (nArrowSize)
   {
      case 1:  // shorter and narrower than normal
               dSubtract = (double)3.0f / (double)5.0f;
               dLength = (double)(2.0f * fLineWidth);
               dWidth = (double)(2.0f * fLineWidth);
               break;

      case 2:  // narrower than normal
               dSubtract = (double)1.0f;
               dLength = (double)(3.0f * fLineWidth);
               dWidth = (double)(2.0f * fLineWidth);
               break;

      case 3:  // longer and narrower than normal
               dSubtract = (double)5.0f / (double)3.0f;
               dLength = (double)(5.0f * fLineWidth);
               dWidth = (double)(2.0f * fLineWidth);
               break;

      case 4:  // shorter than normal
               dSubtract = (double)3.0f / (double)5.0f;
               dLength = (double)(2.0f * fLineWidth);
               dWidth = (double)(3.0f * fLineWidth);
               break;

      case 5:  // normal
               dSubtract = (double)1.0f;
               dLength = (double)(3.0f * fLineWidth);
               dWidth = (double)(3.0f * fLineWidth);
               break;

      case 6:  // longer than normal
               dSubtract = (double)5.0f / (double)3.0f;
               dLength = (double)(5.0f * fLineWidth);
               dWidth = (double)(3.0f * fLineWidth);
               break;

      case 7:  // shorter and wider than normal
               dSubtract = (double)3.0f / (double)5.0f;
               dLength = (double)(2.0f * fLineWidth);
               dWidth = (double)(5.0f * fLineWidth);
               break;

      case 8:  // wider than normal
               dSubtract = (double)1.0f;
               dLength = (double)(3.0f * fLineWidth);
               dWidth = (double)(5.0f * fLineWidth);
               break;

      case 9:  // longer and wider than normal
               dSubtract = (double)5.0f / (double)3.0f;
               dLength = (double)(5.0f * fLineWidth);
               dWidth = (double)(5.0f * fLineWidth);
               break;
   }


   // Define fill color
   SWFFill *pFill = NULL;

   // Define possible arrow points
   double dPoint1X, dPoint2X, dPoint3X, dPoint4X;
   double dPoint1Y, dPoint2Y, dPoint3Y, dPoint4Y;
   double dHelp1X, dHelp2X, dHelp3X, dHelp4X; // control points for type 5 (oval)
   double dHelp1Y, dHelp2Y, dHelp3Y, dHelp4Y; // control points for type 5 (oval)
   double dShiftFactor, dShiftX, dShiftY; // shift vector for type 2 (open arrow)
   double dScaleX, dScaleY; // scaling factor for type 2 (open arrow)


   // Arrow head type
   switch (nArrowType)
   {
      case 1:  // filled arrow head
               dPoint1X = dLength*(+dXn) + 0.5f*dWidth*(+dYn);
               dPoint1Y = dLength*(+dYn) + 0.5f*dWidth*(-dXn);
               dPoint2X = dLength*(+dXn) + 0.5f*dWidth*(-dYn);
               dPoint2Y = dLength*(+dYn) + 0.5f*dWidth*(+dXn);

               pFill = pShape->addSolidFill((fcref >> 16) & 0xff, 
                  (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
               pShape->setRightFill(pFill);

               pShape->movePenTo(x, y);
               pShape->drawLineTo(x+dPoint1X, y+dPoint1Y);
               pShape->drawLineTo(x+dPoint2X, y+dPoint2Y);
               pShape->drawLineTo(x, y); // closed polygon

               break;

      case 2:  // open arrow head
               // this is done in two steps
               // 1st: two simple lines, but shifted by a factor (see below)
               // (and a bit larger than usual)
               dScaleX = (double)1.0f;
               dScaleY = (double)1.1f;
               if (fLineWidth < 1.6f) // special case
               {
                  dScaleX = (double)0.9f;
                  dScaleY = (double)1.5f;
               }
               dPoint1X = dLength*dScaleX*(+dXn) + 0.5f*dWidth*dScaleY*(+dYn);
               dPoint1Y = dLength*dScaleX*(+dYn) + 0.5f*dWidth*dScaleY*(-dXn);
               dPoint2X = dLength*dScaleX*(+dXn) + 0.5f*dWidth*dScaleY*(-dYn);
               dPoint2Y = dLength*dScaleX*(+dYn) + 0.5f*dWidth*dScaleY*(+dXn);

               switch (nArrowSize)
               {
                  // All these values are found empirically
                  case 1:  dShiftFactor = (double)0.50f;
                           break;
                  case 2:  dShiftFactor = (double)0.47f;
                           break;
                  case 3:  dShiftFactor = (double)0.44f;
                           break;
                  case 4:  dShiftFactor = (double)0.38f;
                           break;
                  case 5:  dShiftFactor = (double)0.33f;
                           break;
                  case 6:  dShiftFactor = (double)0.30f;
                           break;
                  case 7:  dShiftFactor = (double)0.31f;
                           break;
                  case 8:  dShiftFactor = (double)0.24f;
                           break;
                  case 9:  dShiftFactor = (double)0.20f;
                           break;
               }
               dShiftX = dShiftFactor*dLength*(+dXn);
               dShiftY = dShiftFactor*dLength*(+dYn);
               if (fLineWidth < 1.6f) // special case
               {
                  dShiftX *= (double)0.5f;
                  dShiftY *= (double)0.5f;
               }

               // use normal linestyle
               pShape->setLine(fLineWidth, (fcref >> 16)  & 0xff, 
                  (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);

               pShape->movePenTo(x+dShiftX, y+dShiftY);
               pShape->drawLineTo(x+dShiftX+dPoint1X, y+dShiftY+dPoint1Y);
               pShape->drawLineTo(x+dShiftX, y+dShiftY);
               pShape->drawLineTo(x+dShiftX+dPoint2X, y+dShiftY+dPoint2Y);
               pShape->drawLineTo(x+dShiftX, y+dShiftY);

               // 2nd: filled triangle als tip of the arrow head 
               // (half the size of "filled arrow head")
               dPoint3X = 0.5f * (dLength*dScaleX*(+dXn) + 0.5f*dWidth*dScaleY*(+dYn));
               dPoint3Y = 0.5f * (dLength*dScaleX*(+dYn) + 0.5f*dWidth*dScaleY*(-dXn));
               dPoint4X = 0.5f * (dLength*dScaleX*(+dXn) + 0.5f*dWidth*dScaleY*(-dYn));
               dPoint4Y = 0.5f * (dLength*dScaleX*(+dYn) + 0.5f*dWidth*dScaleY*(+dXn));

               // use finest linestyle
               pShape->setLine(0.75f, (fcref >> 16) & 0xff, 
                  (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);

               pFill = pShape->addSolidFill((fcref >> 16) & 0xff, 
                  (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
               pShape->setRightFill(pFill);

               pShape->movePenTo(x, y);
               pShape->drawLineTo(x+dPoint3X, y+dPoint3Y);
               pShape->drawLineTo(x+dPoint4X, y+dPoint4Y);
               pShape->drawLineTo(x, y); // closed polygon

               break;

      case 3:  // pointed filled arrow head (?)
               dPoint1X = dLength*(+dXn) + 0.5f*dWidth*(+dYn);
               dPoint1Y = dLength*(+dYn) + 0.5f*dWidth*(-dXn);
               dPoint2X = dLength*(+dXn) + 0.5f*dWidth*(-dYn);
               dPoint2Y = dLength*(+dYn) + 0.5f*dWidth*(+dXn);
               dPoint3X = (dLength - dSubtract*fLineWidth*1.0f)*(+dXn);
               dPoint3Y = (dLength - dSubtract*fLineWidth*1.0f)*(+dYn);

               pFill = pShape->addSolidFill((fcref >> 16) & 0xff, 
                  (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
               pShape->setRightFill(pFill);

               pShape->movePenTo(x, y);
               pShape->drawLineTo(x+dPoint1X, y+dPoint1Y);
               pShape->drawLineTo(x+dPoint3X, y+dPoint3Y);
               pShape->drawLineTo(x+dPoint2X, y+dPoint2Y);
               pShape->drawLineTo(x, y); // closed polygon

               break;

      case 4:  // diamond shape
               dPoint1X = 0.5f*dLength*(+dXn);
               dPoint1Y = 0.5f*dLength*(+dYn);
               dPoint2X = 0.5f*dWidth*(+dYn);
               dPoint2Y = 0.5f*dWidth*(-dXn);
               dPoint3X = 0.5f*dLength*(-dXn);
               dPoint3Y = 0.5f*dLength*(-dYn);
               dPoint4X = 0.5f*dWidth*(-dYn);
               dPoint4Y = 0.5f*dWidth*(+dXn);

               pFill = pShape->addSolidFill((fcref >> 16) & 0xff, 
                  (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
               pShape->setRightFill(pFill);

               pShape->movePenTo(x+dPoint1X, y+dPoint1Y);
               pShape->drawLineTo(x+dPoint2X, y+dPoint2Y);
               pShape->drawLineTo(x+dPoint3X, y+dPoint3Y);
               pShape->drawLineTo(x+dPoint4X, y+dPoint4Y);
               pShape->drawLineTo(x+dPoint1X, y+dPoint1Y); // closed polygon

               break;

      case 5:  // oval shape
               // Use the same 4 anchor points as for diamond shape
               dPoint1X = 0.5f*dLength*(+dXn);
               dPoint1Y = 0.5f*dLength*(+dYn);
               dPoint2X = 0.5f*dWidth*(+dYn);
               dPoint2Y = 0.5f*dWidth*(-dXn);
               dPoint3X = 0.5f*dLength*(-dXn);
               dPoint3Y = 0.5f*dLength*(-dYn);
               dPoint4X = 0.5f*dWidth*(-dYn);
               dPoint4Y = 0.5f*dWidth*(+dXn);

               // 4 control/help points
               dHelp1X = (0.93f)*(dPoint1X+dPoint2X);
               dHelp1Y = (0.93f)*(dPoint1Y+dPoint2Y);
               dHelp2X = (0.93f)*(dPoint2X+dPoint3X);
               dHelp2Y = (0.93f)*(dPoint2Y+dPoint3Y);
               dHelp3X = (0.93f)*(dPoint3X+dPoint4X);
               dHelp3Y = (0.93f)*(dPoint3Y+dPoint4Y);
               dHelp4X = (0.93f)*(dPoint4X+dPoint1X);
               dHelp4Y = (0.93f)*(dPoint4Y+dPoint1Y);

               pFill = pShape->addSolidFill((fcref >> 16) & 0xff, 
                  (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
               pShape->setRightFill(pFill);

               pShape->movePenTo(x+dPoint1X, y+dPoint1Y);
               pShape->drawCurveTo(x+dHelp1X, y+dHelp1Y, x+dPoint2X, y+dPoint2Y);
               pShape->drawCurveTo(x+dHelp2X, y+dHelp2Y, x+dPoint3X, y+dPoint3Y);
               pShape->drawCurveTo(x+dHelp3X, y+dHelp3Y, x+dPoint4X, y+dPoint4Y);
               pShape->drawCurveTo(x+dHelp4X, y+dHelp4Y, x+dPoint1X, y+dPoint1Y); // closed polygon

               break;
   }

}

void CFlashEngine::DrawPointer(SWFShape *pShape, float x, float y, float size, int nType) {
    switch (nType) {
        case imc_lecturnity_converter_LPLibs_TELEPOINTER_TYPE_POINTER:
            DrawTelePointer(pShape, x, y, size);
            break;
        case imc_lecturnity_converter_LPLibs_TELEPOINTER_TYPE_CURSOR:
            DrawMouseCursor(pShape, x, y, size);
            break;
        default:
            // In doubt ... draw TelePointer
            DrawTelePointer(pShape, x, y, size);
            break;
    }
}

void CFlashEngine::DrawTelePointer(SWFShape *pShape, float x, float y, float size)
{
   x -= size;

   // Define 4 points for the Telepointer
   DrawSdk::DPoint P[4];
   P[0].x = x + size - 1.0f;            P[0].y = y + 0.0f;
   P[1].x = x + 0.0f;                   P[1].y = y + (0.5f * size) - 1.0f;
   P[2].x = x + (0.35f * size);         P[2].y = y + (0.65f * size);
   P[3].x = x + (0.5f * size);          P[3].y = y + size - 1.0f;

   // Draw the light red triangle
   pShape->setLine(1.0, 0x00, 0x00, 0x00);
   SWFFill *fill_1 = pShape->addSolidFill(0xFF, 0x00, 0x00);
   pShape->setLeftFill(fill_1);
   pShape->movePenTo(P[0].x, P[0].y);
   pShape->drawLineTo(P[1].x, P[1].y);
   pShape->drawLineTo(P[2].x, P[2].y);
   pShape->drawLineTo(P[0].x, P[0].y);


   // Draw the dark red triangle
   pShape->setLine(1.0, 0x00, 0x00, 0x00);
   SWFFill *fill_2 = pShape->addSolidFill(0x80, 0x00, 0x00);
   pShape->setLeftFill(fill_2);
   pShape->movePenTo(P[0].x, P[0].y);
   pShape->drawLineTo(P[2].x, P[2].y);
   pShape->drawLineTo(P[3].x, P[3].y);
   pShape->drawLineTo(P[0].x, P[0].y);
}

void CFlashEngine::DrawMouseCursor(SWFShape *pShape, float x, float y, float size)
{
    /*
    // Coordinates used in the LEC Player:
    ps2[0] = new Point(0, 0);
    ps2[1] = new Point(0, (int)(16 * dScale));
    ps2[2] = new Point((int)(4 * dScale), (int)(12 * dScale));
    ps2[3] = new Point((int)(4 * dScale), (int)(13 * dScale));
    ps2[4] = new Point((int)(8 * dScale), (int)(20 * dScale));
    ps2[5] = new Point((int)(9 * dScale), (int)(20 * dScale));
    ps2[6] = new Point((int)(10 * dScale), (int)(19 * dScale));
    ps2[7] = new Point((int)(7 * dScale), (int)(12 * dScale));
    ps2[8] = new Point((int)(7 * dScale), (int)(11 * dScale));
    ps2[9] = new Point((int)(11 * dScale), (int)(11 * dScale));
    */

    // Define 10 points for the mouse cursor
    // (Default pointer size is 30x30)
    DrawSdk::DPoint P[10];
    P[0].x = x + 0.0f;                   P[0].y = y + 0.0f;
    P[1].x = x + 0.0f;                   P[1].y = y + (0.53333f * size);
    P[2].x = x + (0.13333f * size);      P[2].y = y + (0.40000f * size);
    P[3].x = x + (0.13333f * size);      P[3].y = y + (0.43333f * size);
    P[4].x = x + (0.26667f * size);      P[4].y = y + (0.66667f * size);
    P[5].x = x + (0.30000f * size);      P[5].y = y + (0.66667f * size);
    P[6].x = x + (0.33333f * size);      P[6].y = y + (0.63333f * size);
    P[7].x = x + (0.23333f * size);      P[7].y = y + (0.40000f * size);
    P[8].x = x + (0.23333f * size);      P[8].y = y + (0.36667f * size);
    P[9].x = x + (0.36667f * size);      P[9].y = y + (0.36667f * size);

    // Draw the shape of the mouse cursor
   pShape->setLine(1.0, 0x00, 0x00, 0x00);
   SWFFill *fill_1 = pShape->addSolidFill(0xFF, 0xFF, 0xFF);
   pShape->setLeftFill(fill_1);
   pShape->movePenTo(P[0].x, P[0].y);
   pShape->drawLineTo(P[1].x, P[1].y);
   pShape->drawLineTo(P[2].x, P[2].y);
   pShape->drawLineTo(P[3].x, P[3].y);
   pShape->drawLineTo(P[4].x, P[4].y);
   pShape->drawLineTo(P[5].x, P[5].y);
   pShape->drawLineTo(P[6].x, P[6].y);
   pShape->drawLineTo(P[7].x, P[7].y);
   pShape->drawLineTo(P[8].x, P[8].y);
   pShape->drawLineTo(P[9].x, P[9].y);
   pShape->drawLineTo(P[0].x, P[0].y);
}

void CFlashEngine::DrawText(SWFText *pSwfText, float x, float y, 
                            SWFFont *pSwfFont, CString csText, 
                            float fFontSize, Gdiplus::ARGB crFontColor)
{
   pSwfText->moveTo(x, y);
   pSwfText->setFont(pSwfFont);
   pSwfText->setHeight(fFontSize);
   // one could consider transparency here, but PPT doesn't support that 
   // so for now it is deactivated
   pSwfText->setColor((crFontColor >> 16) & 0xff, (crFontColor >> 8) & 0xff, (crFontColor >> 0) & 0xff);//, (crFontColor >> 24) & 0xff);
   //pSwfText->addString(csText);

   char *utfText = LString::TCharToUtf8(csText);
   if (utfText)
   {
      pSwfText->addUTF8String(utfText);
      delete[] utfText;
      utfText = NULL;
   }
}

void CFlashEngine::DrawUnderline(SWFSprite *pSprParent, float x, float y, float fLength, 
                                 LOGFONT lf, Gdiplus::ARGB crColor)
{
   float fFontSize = -1.0f * lf.lfHeight;
   float fLineWidth = fFontSize / 10.0f; // empirical value (similar to the value used by the LEC Player)
   float fBaselineOffset = fFontSize / 8.0f; // empirical value
   //// In PowerPoint the line width of bold text is thicker than normal
   //bool bLogFontIsBold = (lf.lfWeight >= 700) ? true : false;
   //if (bLogFontIsBold)
   //   fLineWidth *= 1.5f; // empirical value
   if (fLineWidth < 1.0f)
      fLineWidth = 1.0f;

   //pShape->setLine(fLineWidth, (crColor >> 16) & 0xff, 
   //   (crColor >> 8) & 0xff, (crColor >> 0) & 0xff, (crColor >> 24) & 0xff);
   Gdiplus::ARGB crOrig = m_argbStd;
   m_argbStd = crColor;
   DrawPptLine(pSprParent, x, y+fBaselineOffset, fLength, 0.0f, fLineWidth, false, false);
   m_argbStd = crOrig;
}

void CFlashEngine::DrawImage(SWFShape *pShape, SWFBitmap *pBitmap, bool bConsiderMetadata)
{
   SWFFill *fill_img = pShape->addBitmapFill(pBitmap, SWFFILL_CLIPPED_BITMAP);

   pShape->setRightFill(fill_img);

///   DrawRectangle(pShape, 0.0f, 0.0f, pBitmap->getWidth(), pBitmap->getHeight(), 0.0f, false, false);
///   DrawRectangle(pShape, 0.0f, 0.0f, (float)(pBitmap->getWidth())/m_fScaleX, (float)(pBitmap->getHeight())/m_fScaleY, 0.0f, false, false);

   float imgX = 0.0f;
   float imgY = 0.0f;
   float imgWidth = (float)pBitmap->getWidth();
   float imgHeight = (float)pBitmap->getHeight();

   if (bConsiderMetadata)
   {
      if (imgWidth > (float)(m_rcMetadata.Width()))
         imgWidth = (float)(m_rcMetadata.Width());
      if (imgHeight > (float)(m_rcMetadata.Height()))
         imgHeight = (float)(m_rcMetadata.Height());
   }

   // To be sure we have a closed polygon/rectangle: use movePenTo()/drawLineTo() here
   pShape->movePenTo(imgX, imgY);
   pShape->drawLineTo(imgX+imgWidth, imgY);
   pShape->drawLineTo(imgX+imgWidth, imgY+imgHeight);
   pShape->drawLineTo(imgX, imgY+imgHeight);
   pShape->drawLineTo(imgX, imgY);
}

bool CFlashEngine::CheckForArrowHeadConsistency(int nArrowStyle, int &nBeginType, int &nBeginSize, int &nEndType, int &nEndSize)
{
   // Not only check for consistency.
   // Set to default values, if necessary.
   // Arrow style rules arrow type

   bool bIsConsistent = true;

   switch (nArrowStyle)
   {
      case 0:  // No arrow, simple line
         if (nBeginType > 0)
         {
            bIsConsistent = false;
            nBeginType = 0;
            nBeginSize = 0;
         }
         if (nEndType > 0)
         {
            bIsConsistent = false;
            nEndType = 0;
            nEndSize = 0;
         }
         break;
      case 1:  // arrow at end of line
         if (nBeginType > 0)
         {
            bIsConsistent = false;
            nBeginType = 0;
            nBeginSize = 0;
         }
         if (nEndType == 0)
         {
            bIsConsistent = false;
            nEndType = 1; // default 
            nEndSize = 5; // default
         }
         break;
      case 2:  // arrow at begin of line
         if (nBeginType == 0)
         {
            bIsConsistent = false;
            nBeginType = 1; // default
            nBeginSize = 5; // default
         }
         if (nEndType > 0)
         {
            bIsConsistent = false;
            nEndType = 0;
            nEndSize = 0;
         }
         break;
      case 3:  // arrow at begin and end of line
         if (nBeginType == 0)
         {
            bIsConsistent = false;
            nBeginType = 1; // default
            nBeginSize = 5; // default
         }
         if (nEndType == 0)
         {
            bIsConsistent = false;
            nEndType = 1; // default 
            nEndSize = 5; // default
         }
         break;
   }

   // 2nd check: arrow type with no arrow size
   if (true == bIsConsistent)
   {
      if ((nBeginType > 0) && (nBeginSize == 0))
      {
         bIsConsistent = false;
         nBeginSize = 5; // default
      }
      if ((nEndType > 0) && (nEndSize == 0))
      {
         bIsConsistent = false;
         nEndSize = 5; // default
      }
   }

   if (false == bIsConsistent)
   {
      // Consistency check failed!
      _tprintf(_T("Warning: Arrow type and arrow configuration are not consistent! Default values are used.\n"));
      //_TCHAR tszMessage[1024];
      //_stprintf(tszMessage, "Arrow type (%d) and arrow configuration (%d, %d) are not consistent!", 
      //   nArrowStyle, nBeginType, nEndType);
      //::MessageBox(NULL, tszMessage, NULL, MB_OK);
   }

   return bIsConsistent;
}


// --------------------------------------------------------------------------
// Geometric shapes for the control bar
// --------------------------------------------------------------------------

// Note: Drawing with relative coordinates (movePen(), drawLine()) leads to little rounding errors 
// and therefore to artefacts with filled polygons, if the polygon is not exactly closed.
// ==> Start/End point must be reached with movePenTo()/drawLineTo().

void CFlashEngine::DrawPlayShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb)
{
   MYRGB rgb067 = myRgb;
   MYRGB rgb095 = myRgb;
   MYRGB rgb125 = myRgb;
   InitSymbolShape(pShape, x, y, size, myRgb, rgb067, rgb095, rgb125);

   // Only optical correction
   pShape->movePenTo(x+0.10f*size, y+0.0f);

   pShape->setLine(1, rgb095.r, rgb095.g, rgb095.b);
   pShape->drawLine(size, 0.5f*size);
   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(-size, 0.5f*size);
   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLineTo(x+0.10f*size, y+0.0f);
}

void CFlashEngine::DrawPauseShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb)
{
   MYRGB rgb067 = myRgb;
   MYRGB rgb095 = myRgb;
   MYRGB rgb125 = myRgb;
   InitSymbolShape(pShape, x, y, size, myRgb, rgb067, rgb095, rgb125);

   pShape->movePenTo(x+0.1f*size, y+0.0f);
   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLine(0.3f*size, 0.0f);
   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(0.0f, size);
   pShape->drawLine(-0.3f*size, 0.0f);
   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLineTo(x+0.1f*size, y+0.0f);
   pShape->movePenTo(x+0.6f*size, y+0.0f);
   pShape->drawLine(0.3f*size, 0.0f);
   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(0.0f, size);
   pShape->drawLine(-0.3f*size, 0.0f);
   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLineTo(x+0.6f*size, y+0.0f);
}

void CFlashEngine::DrawGotoFirstFrameShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb)
{
   MYRGB rgb067 = myRgb;
   MYRGB rgb095 = myRgb;
   MYRGB rgb125 = myRgb;
   InitSymbolShape(pShape, x, y, size, myRgb, rgb067, rgb095, rgb125);

   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLine(0.3f*size, 0.0f);
   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(0.0f, size);
   pShape->drawLine(-0.3f*size, 0.0f);
   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLineTo(x+0.0f, y+0.0f);
   pShape->movePenTo(x+size, y+0.0f);
   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(0.0f, size);
   pShape->setLine(1, rgb095.r, rgb095.g, rgb095.b);
   pShape->drawLine(-0.7f*size, -0.5f*size);
   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLineTo(x+size, y+0.0f);

}

void CFlashEngine::DrawGotoLastFrameShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb)
{
   MYRGB rgb067 = myRgb;
   MYRGB rgb095 = myRgb;
   MYRGB rgb125 = myRgb;
   InitSymbolShape(pShape, x, y, size, myRgb, rgb067, rgb095, rgb125);

   pShape->setLine(1, rgb095.r, rgb095.g, rgb095.b);
   pShape->drawLine(0.7f*size, 0.5f*size);
   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(-0.7f*size, 0.5f*size);
   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLineTo(x+0.0f, y+0.0f);
   pShape->movePenTo(x+0.7f*size, y+0.0f);
   pShape->drawLine(0.3f*size, 0.0f);
   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(0.0f*size, size);
   pShape->drawLine(-0.3f*size, 0.0f);
   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLineTo(x+0.7f*size, y+0.0f);

}

void CFlashEngine::DrawGotoPrevPageShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb)
{
   MYRGB rgb067 = myRgb;
   MYRGB rgb095 = myRgb;
   MYRGB rgb125 = myRgb;
   InitSymbolShape(pShape, x, y, size, myRgb, rgb067, rgb095, rgb125);

   // Only optical correction
//   pShape->movePenTo(x-0.1f*size, y+0.0f);

   pShape->movePenTo(x+0.4f*size, y+0.0f);
   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(0.0f, size);
   pShape->setLine(1, rgb095.r, rgb095.g, rgb095.b);
   pShape->drawLine(-0.5f*size, -0.5f*size);
   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLineTo(x+0.4f*size, y+0.0f);
   pShape->movePenTo(x+0.9f*size, y+0.0f);
   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(0.0f, size);
   pShape->setLine(1, rgb095.r, rgb095.g, rgb095.b);
   pShape->drawLine(-0.5f*size, -0.5f*size);
   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLineTo(x+0.9f*size, y+0.0f);
}

void CFlashEngine::DrawGotoNextPageShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb)
{
   MYRGB rgb067 = myRgb;
   MYRGB rgb095 = myRgb;
   MYRGB rgb125 = myRgb;
   InitSymbolShape(pShape, x, y, size, myRgb, rgb067, rgb095, rgb125);

   // Only optical correction
   pShape->movePenTo(x+0.1f*size, y+0.0f);

   pShape->setLine(1, rgb095.r, rgb095.g, rgb095.b);
   pShape->drawLine(0.5f*size, 0.5f*size);
   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(-0.5f*size, 0.5f*size);
   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLineTo(x+0.1f*size, y+0.0f);
   pShape->movePenTo(x+0.6f*size, y+0.0f);
   pShape->setLine(1, rgb095.r, rgb095.g, rgb095.b);
   pShape->drawLine(0.5f*size, 0.5f*size);
   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(-0.5f*size, 0.5f*size);
   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLineTo(x+0.6f*size, y+0.0f);
}

void CFlashEngine::DrawVolumeShape(SWFShape *pShape, float x, float y, float size, MYRGB myRgb)
{
   MYRGB rgb067 = myRgb;
   MYRGB rgb095 = myRgb;
   MYRGB rgb125 = myRgb;
   InitSymbolShape(pShape, x, y, size, myRgb, rgb067, rgb095, rgb125);

   pShape->setLine(1, rgb095.r, rgb095.g, rgb095.b);

   pShape->movePenTo(x-0.30f*size, y+0.125f*size);
//   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLine(0.375*size, 0.0f);
   pShape->drawLine(0.5f*size, -0.5f*size);
//   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(0.0f, 1.75f*size);
//   pShape->setLine(1, rgb095.r, rgb095.g, rgb095.b);
   pShape->drawLine(-0.5f*size, -0.5f*size);
//   pShape->setLine(1, rgb125.r, rgb125.g, rgb125.b);
   pShape->drawLine(-0.375*size, 0.0f);
//   pShape->setLine(1, rgb067.r, rgb067.g, rgb067.b);
   pShape->drawLineTo(x-0.30f*size, y+0.125f*size);

   pShape->movePenTo(x+0.80f*size, y+0.10f*size);
   pShape->drawLine(0.15f*size, 0.30f*size);
   pShape->drawLine(0.0f, 0.25f*size);
   pShape->drawLine(-0.15f*size, 0.30f*size);
   pShape->drawLine(-0.01f*size, 0.0f);
   pShape->drawLine(0.15f*size, -0.30f*size);
   pShape->drawLine(0.0f, -0.25f*size);
   pShape->drawLine(-0.15f*size, -0.30f*size);
   pShape->drawLineTo(x+0.80f*size, y+0.10f*size);

   pShape->movePenTo(x+1.00f*size, y-0.10f*size);
   pShape->drawLine(0.30f*size, 0.40f*size);
   pShape->drawLine(0.0f, 0.40f*size);
   pShape->drawLine(-0.30f*size, 0.40f*size);
   pShape->drawLine(-0.01f*size, 0.0f);
   pShape->drawLine(0.30f*size, -0.40f*size);
   pShape->drawLine(0.0f, -0.40f*size);
   pShape->drawLine(-0.30f*size, -0.40f*size);
   pShape->drawLineTo(x+1.00f*size, y-0.10f*size);
}

void CFlashEngine::InitSymbolShape(SWFShape *pShape, float &x, float &y, float &size, 
                                   MYRGB rgb100, MYRGB &rgb067, MYRGB &rgb095, MYRGB &rgb125)
{
   SWFFill *fill = pShape->addSolidFill(rgb100.r, rgb100.g, rgb100.b);
   pShape->setRightFill(fill);


//   // Create different line colors for the shadow effect
//   if ((rgb100.r == m_rgbUp.r) && (rgb100.g == m_rgbUp.g) && (rgb100.b == m_rgbUp.b))
//   {
//      ChangeColorLightness(rgb100, rgb067, 0.50f);
//      ChangeColorLightness(rgb100, rgb095, 0.95f);
//      ChangeColorLightness(rgb100, rgb125, 1.75f);
//   }
//   else if ((rgb100.r == m_rgbOver.r) && (rgb100.g == m_rgbOver.g) && (rgb100.b == m_rgbOver.b))
//   {
//      ChangeColorLightness(rgb100, rgb067, 0.75f);
//      ChangeColorLightness(rgb100, rgb095, 0.95f);
//      ChangeColorLightness(rgb100, rgb125, 1.10f);
////      rgb067.r = 0x39; rgb067.g = 0x44; rgb067.b = 0x54;
////      rgb125.r = 0x0B; rgb125.g = 0xE6; rgb125.b = 0xF5;
//   }
//   else if ((rgb100.r == m_rgbDown.r) && (rgb100.g == m_rgbDown.g) && (rgb100.b == m_rgbDown.b))
//   {
//      ChangeColorLightness(rgb100, rgb067, 0.67f);
//      ChangeColorLightness(rgb100, rgb095, 0.95f);
//      ChangeColorLightness(rgb100, rgb125, 1.35f);
////      rgb067.r = 0x12; rgb067.g = 0x31; rgb067.b = 0x5C;
////      rgb125.r = 0xAF; rgb125.g = 0xCE; rgb125.b = 0xF8;
//   }
//   else
//   {
//      ChangeColorLightness(rgb100, rgb067, 0.67f);
//      ChangeColorLightness(rgb100, rgb095, 0.95f);
//      ChangeColorLightness(rgb100, rgb125, 1.25f);
//   }

   // Use unique line colors for the shadow effect
   ChangeColorLightness(m_rgbUp, rgb067, 0.50f);
   ChangeColorLightness(m_rgbUp, rgb095, 0.95f);
   ChangeColorLightness(m_rgbUp, rgb125, 1.75f);

   // Less contrast for disabled symbols
   if ((rgb100.r == m_rgbDisabled.r) && (rgb100.g == m_rgbDisabled.g) && (rgb100.b == m_rgbDisabled.b))
   {
      ChangeColorLightness(rgb100, rgb067, 0.67f);
      ChangeColorLightness(rgb100, rgb095, 0.95f);
      ChangeColorLightness(rgb100, rgb125, 1.25f);
   }

   float scale = 0.40f;
   float dx = 0.5f * (size - (size*scale));
   float dy = 0.5f * (size - (size*scale));

   size *= scale;
   x += dx;
   y += dy;

   pShape->movePenTo(x, y);
}

// --------------------------------------------------------------------------

/**
 * --------------------------------------------------------------------------
 * A common method to draw a rectangle with rounded corners and 3D effects
 * --------------------------------------------------------------------------
 * Parameters:
 *    pShape               - a pointer to the shape object
 *    x                    - x coordinate (as float)
 *    y                    - y coordinate (as float)
 *    width                - width (as float)
 *    height               - height (as float)
 *    radius               - radius (as float) of the quarter circle building the corner
 *    penSize              - pen size / line width (as float)
 *    penPaletteFlag       - flag for the pen palette: D3D_NO_PEN_PALETTE, D3D_PEN_PALETTE, D3D_PEN_PALETTE_DARK
 *    shapeFlag            - flag for the shape (e.g. circle or semi-circle): D3D_FULL_ROUND, D3D_LEFT_ROUND, D3D_RIGHT_ROUND
 *    borderFlag           - flag for the border: D3D_NO_BORDER, D3D_BORDER
 *    filledFlag           - flag for the filling: D3D_NO_FILL, D3D_FILL
 *    embossFlag           - flag for the embossing effect: D3D_RAISED, D3D_DEEPENED
 *    gradientFlag         - flag for the gradient: D3D_NO_GRADIENT, D3D_LINEAR_GRADIENT, D3D_RADIAL_GRADIENT
 *    gradient             - a pointer to the gradient object (can be NULL)
 *    rgbBorder            - a pointer to the border color object (can be NULL)
 *    rgbBackground        - a pointer to the background color object (can be NULL)
 */
void CFlashEngine::Draw3DRoundedRectangle(SWFShape *pShape, float x, float y, float width, float height, 
                                          float radius, int penSize, int penPaletteFlag, int shapeFlag, 
                                          int borderFlag, int filledFlag, int embossFlag, int gradientFlag, 
                                          SWFGradient *gradient, MYRGB *rgbBorder, MYRGB *rgbBackground)
{
   // - The rounded rectangle is drawn by 12 segments
   // - 4 different color nuances can be used for the border
   //   for upper left (UL) / down right (DR) / upper right (UR) / down left (DL)
   MYRGB colUL, colDR, colUR, colDL;

   // Border
   if (borderFlag == D3D_BORDER)
   {
      if (penPaletteFlag== D3D_PEN_PALETTE_DARK)
      {
         if (embossFlag == D3D_DEEPENED)
         {
//            colUL.r = 0x0A; colUL.g = 0x09; colUL.b = 0x08; 
//            colDR.r = 0xFD; colDR.g = 0xFD; colDR.b = 0xFC; 
            colUL = m_rgb005;
            colDR = m_rgb125;
            colUR = m_rgb080;
            colDL = m_rgb105;
         }
         else // D3D_RAISED
         {
//            colUL.r = 0xFD; colUL.g = 0xFD; colUL.b = 0xFC; 
//            colDR.r = 0x0A; colDR.g = 0x09; colDR.b = 0x08; 
            colUL = m_rgb125;
            colDR = m_rgb005;
            colUR = m_rgb105;
            colDL = m_rgb080;
         }
      }
      else if (penPaletteFlag == D3D_PEN_PALETTE)
      {
         if (embossFlag == D3D_DEEPENED)
         {
//            colUL.r = 0x90; colUL.g = 0x8F; colUL.b = 0x84; 
//            colDR.r = 0xF3; colDR.g = 0xF1; colDR.b = 0xE7; 
            colUL = m_rgb050;
            colDR = m_rgb125;
            colUR = m_rgb095;
            colDL = m_rgb105;
         }
         else // D3D_RAISED
         {
//            colUL.r = 0xF3; colUL.g = 0xF1; colUL.b = 0xE7; 
//            colDR.r = 0x90; colDR.g = 0x8F; colDR.b = 0x84; 
            colUL = m_rgb125;
            colDR = m_rgb050;
            colUR = m_rgb105;
            colDL = m_rgb095;
         }
      }
      else if (penPaletteFlag == D3D_NO_PEN_PALETTE)
      {
         if (rgbBorder != NULL)
            colUL = colDR = colUR = colDL = *rgbBorder; 
         else
            colUL = colDR = colUR = colDL = m_rgb080;
      }
   }

   // Fill
   if (filledFlag == D3D_FILL)
   {
      SWFFill *fill_1;

      float diameter = (width > height) ? width : height;

      if (gradientFlag == D3D_LINEAR_GRADIENT)
      {
         // Create a linear Gradient
         if (gradient)
            fill_1 = pShape->addGradientFill(gradient, SWFFILL_LINEAR_GRADIENT);
         else
            fill_1 = pShape->addGradientFill(m_swfLinearGradient, SWFFILL_LINEAR_GRADIENT);
         if (embossFlag == D3D_DEEPENED)
            fill_1->rotate(90.0f);
         else
            fill_1->rotate(-90.0f);
         fill_1->scale(diameter / 1500.0f);
         fill_1->move(0.00f, y+(0.5f*height));
         pShape->setLeftFill(fill_1);
      }
      else if (gradientFlag == D3D_RADIAL_GRADIENT)
      {
         if (gradient)
            fill_1 = pShape->addGradientFill(gradient, SWFFILL_RADIAL_GRADIENT);
         else
            fill_1 = pShape->addGradientFill(m_swfRadialGradient, SWFFILL_RADIAL_GRADIENT);
         fill_1->scale(diameter / 750.0f);
         fill_1->move(x+(0.5f*width), y+(0.5f*height));
         if (embossFlag == D3D_DEEPENED)
            fill_1->move(0.33f*width, 0.33f*height);
         else
            fill_1->move(-0.33f*width, -0.33f*height);
         pShape->setLeftFill(fill_1);
      }
      else // D3D_NO_GRADIENT
      {
         if (rgbBackground != NULL)
         {
            fill_1 = pShape->addSolidFill(rgbBackground->r, rgbBackground->g, rgbBackground->b);
            pShape->setLeftFill(fill_1);
         }
      }
   }

   // Determine the linear parts of the rectangle
   float dWidth = width - (2.0f * radius);
   if (radius > (width / 2.0f)) 
   {
      radius = width / 2.0f;
      dWidth = 0.0f;
   }
   float dHeight = height - (2.0f * radius);
   if (radius > (height / 2.0f)) 
   {
      radius = height / 2.0f;
      dHeight = 0.0f;
   }

   float c = (radius / (1.0f + sqrt(2.0f)));
   float d = (radius / (2.0f + sqrt(2.0f)));

   // Draw the rectangle
   pShape->movePenTo(x + radius, y + height);

   if (shapeFlag == D3D_RIGHT_ROUND)
   {
      if (borderFlag == D3D_BORDER)
         pShape->setLine(penSize, colDR.r, colDR.g, colDR.b);
      pShape->drawLine(-radius, 0.0f);
      if (borderFlag == D3D_BORDER)
         pShape->setLine(penSize, colUL.r, colUL.g, colUL.b);
      pShape->drawLine(0.00, -height);
      pShape->drawLine(radius, 0.0f);
   }
   else
   {
      if (borderFlag == D3D_BORDER)
         pShape->setLine(penSize, colDL.r, colDL.g, colDL.b);
      pShape->drawCurve(-c, 0.0f, -d, -d);
      if (borderFlag == D3D_BORDER)
         pShape->setLine(penSize, colUR.r, colUR.g, colUR.b);
      pShape->drawCurve(-d, -d, 0.0f, -c);
      if (borderFlag == D3D_BORDER)
         pShape->setLine(penSize, colUL.r, colUL.g, colUL.b);
      pShape->drawLine(0.00, -dHeight);
      pShape->drawCurve(0.0f, -c, d, -d);
      pShape->drawCurve(d, -d, c, 0.0f);
   }

   pShape->drawLine(dWidth, 0.0f);

   if (shapeFlag == D3D_LEFT_ROUND)
   {
      if (borderFlag == D3D_BORDER)
         pShape->setLine(penSize, colUL.r, colUL.g, colUL.b);
      pShape->drawLine(radius, 0.0f);
      if (borderFlag == D3D_BORDER)
         pShape->setLine(penSize, colDR.r, colDR.g, colDR.b);
      pShape->drawLine(0.00, height);
      pShape->drawLine(-radius, 0.0f);
   }
   else
   {
      if (borderFlag == D3D_BORDER)
         pShape->setLine(penSize, colUR.r, colUR.g, colUR.b);
      pShape->drawCurve(c, 0.0f, d, d);
      if (borderFlag == D3D_BORDER)
         pShape->setLine(penSize, colDL.r, colDL.g, colDL.b);
      pShape->drawCurve(d, d, 0.0f, c);
      if (borderFlag == D3D_BORDER)
         pShape->setLine(penSize, colDR.r, colDR.g, colDR.b);
      pShape->drawLine(0.00, dHeight);
      pShape->drawCurve(0.0f, c, -d, d);
      pShape->drawCurve(-d, d, -c, 0.0f);
   }

   pShape->drawLineTo(x + radius, y + height);
}

// --------------------------------------------------------------------------
// Button shape for the control bar
// --------------------------------------------------------------------------

void CFlashEngine::CreateButtonShape(SWFShape *pShape, int r, int g, int b, int a)
{
   // - without color: creates a white filled shape for use with SWFBUTTON_HIT
   // - with color: creates a colored shape with black border 

   SWFFill *fillBtn;
   if ((r<0) && (g<0) && (b<0))
   {
      fillBtn = pShape->addSolidFill(0xFF, 0xFF, 0xFF, a);
      pShape->setRightFill(fillBtn);
      pShape->setLine(1, 0xFF, 0xFF, 0xFF, a);
   }
   else
   {
     fillBtn = pShape->addSolidFill(r, g, b, a);
     pShape->setRightFill(fillBtn);
     pShape->setLine(1, 0x00, 0x00, 0x00, a);
   }
}

void CFlashEngine::DrawRectButtonShape(SWFShape *pShape, float width, float height, int r, int g, int b, int a)
{
   CreateButtonShape(pShape, r, g, b, a);
   DrawRectangle(pShape, 0.0f, 0.0f, width, height);
}

void CFlashEngine::DrawOvalButtonShape(SWFShape *pShape, float width, float height, int r, int g, int b, int a)
{
   CreateButtonShape(pShape, r, g, b, a);
   DrawOval(pShape, 0.0f, 0.0f, width, height);
}


// --------------------------------------------------------------------------
// Interactive elements
// --------------------------------------------------------------------------

/**
 * --------------------------------------------------------------------------
 * A common method to create an interactive button/area object
 * --------------------------------------------------------------------------
 * Parameters:
 *    movie                - a pointer to the parent movie object
 *    x                    - x coordinate (as float)
 *    y                    - y coordinate (as float)
 *    width                - width (as float)
 *    height               - height (as float)
 *    idx                  - index
 *    pInteractionArea     - a pointer to a CInteractionArea object
 *    scriptClicked        - String with the ActionScript to be executed when button is clicked/pressed - can be NULL
 *    scriptMouseUp        - String with the ActionScript to be executed on "mouse up" event - can be NULL
 *    scriptMouseDown      - String with the ActionScript to be executed on "mouse down" event - can be NULL
 *    scriptMouseOver      - String with the ActionScript to be executed on "mouse over" event - can be NULL
 *    scriptMouseOut       - String with the ActionScript to be executed on "mouse out" event - can be NULL
 */
void CFlashEngine::CreateInteractionArea(SWFSprite *sprPages, float x, float y, float width, float height,
                                       int idx, CInteractionArea *pInteractionArea, 
                                       _TCHAR *scriptClicked, _TCHAR *scriptMouseUp, _TCHAR *scriptMouseDown, 
                                       _TCHAR *scriptMouseOver, _TCHAR *scriptMouseOut)
{
   _tprintf(_T("IAC %d: %g,%g %gx%g\n"), idx, x, y, width, height);

   CString csButtonName;
   if (idx >= 0)
      csButtonName.Format(_T("Interaction_%d"), idx);
   else
      csButtonName.Format(_T("Interaction_Ok"));
   CString csButtonTitle = _T(""); // TODO: extract the button title from the button text

   bool bIsButton = pInteractionArea->IsButton();
   if (bIsButton)
   {
      CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *paInactiveDrawObjects 
         = pInteractionArea->GetInactiveObjects();

      if (paInactiveDrawObjects != NULL)
         // Draw a "disabled" button
         DrawDisabledInteractionButton(sprPages, x, y, width, height, idx, csButtonName, paInactiveDrawObjects);
   }

   // Button must be independent from the pages (/movie) content --> use a new SWFSprite here
   SWFSprite *sprCommon = new SWFSprite();

   // Draw the interaction area/button into this sprite
   DrawInteractionArea(sprCommon, x, y, width, height, idx, pInteractionArea, 
      scriptClicked, scriptMouseUp, scriptMouseDown, scriptMouseOver, scriptMouseOut);

   SWFDisplayItem *di;

   // Add the SWFSprite to the pages (/movie)
   di = sprPages->add(sprCommon);
   di->moveTo(x, y);
//   di->setDepth(15500 + (2*idx) + 1);
   m_iMaxInteractionIndex++;
//#ifdef _DEBUG
   _tprintf(_T("+ InteractionArea           Index: %d\n"), m_iMaxInteractionIndex);
//#endif
   di->setDepth(m_iMaxInteractionIndex);
   CString csId;
   csId.Format(_T("swfSpr%s"), csButtonName);
   di->setName(GetCharFromCString(csId));
}

void CFlashEngine::DrawInteractionArea(SWFSprite *sprCommon, float x, float y, float width, float height,
                                       int idx, CInteractionArea *pInteractionArea, 
                                       _TCHAR *scriptClicked, _TCHAR *scriptMouseUp, _TCHAR *scriptMouseDown, 
                                       _TCHAR *scriptMouseOver, _TCHAR *scriptMouseOut)
{
   CString csButtonName;
   if (idx >= 0)
      csButtonName.Format(_T("Interaction_%d"), idx);
   else
      csButtonName.Format(_T("Interaction_Ok"));
   CString csButtonTitle = _T(""); // TODO: extract the button title from the button text

   bool bIsButton = pInteractionArea->IsButton();
   bool bIsCallout = pInteractionArea->IsCallout();

   SWFDisplayItem *di;

   // Create a common button shape
   SWFShape *shpBtnCommon = new SWFShape();

   // Create a transparent shape
   SWFFill *fillBtn = shpBtnCommon->addSolidFill(0x00, 0x00, 0x00, 0x00);
   shpBtnCommon->setRightFill(fillBtn);
   DrawRectangle(shpBtnCommon, 0.0f, 0.0f, width, height);
   di = sprCommon->add(shpBtnCommon);

   // Create a shape for "Button hit"
   SWFShape *shpBtnHit = new SWFShape();
   shpBtnHit = shpBtnCommon;

   // The button itself
   SWFButton *btnCommon = new SWFButton();

   // Interactive area or button?
   if (bIsButton)
   {
      CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *paNormalDrawObjects 
         = pInteractionArea->GetNormalObjects();
      CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *paOverDrawObjects 
          = bIsCallout? pInteractionArea->GetNormalObjects() : pInteractionArea->GetOverObjects();
      CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *paPressedDrawObjects 
         = bIsCallout? pInteractionArea->GetNormalObjects() : pInteractionArea->GetPressedObjects();

      btnCommon->addShape(shpBtnHit, SWFBUTTON_HIT);
      DrawInteractionButtonShape(btnCommon, paNormalDrawObjects, SWFBUTTON_UP);
      DrawInteractionButtonShape(btnCommon, paOverDrawObjects, SWFBUTTON_OVER);
      DrawInteractionButtonShape(btnCommon, paPressedDrawObjects, SWFBUTTON_DOWN);
   }
   else
   {
      btnCommon->addShape(shpBtnHit, SWFBUTTON_HIT);
      btnCommon->addShape(shpBtnCommon, SWFBUTTON_UP);
      btnCommon->addShape(shpBtnCommon, SWFBUTTON_OVER);
      btnCommon->addShape(shpBtnCommon, SWFBUTTON_DOWN);
   }

   // Add ActionScript for button clicked/pressed
   if (scriptClicked != NULL)
   {
      //CString csClicked;
      //csClicked.Format(_T("%s; if (g_bIsPlaying){lecDoPlay();}"), scriptClicked);
      //btnCommon->addAction(new SWFAction(csClicked), SWFBUTTON_OVERDOWNTOOVERUP);
      PrintActionScript(csButtonName, scriptClicked);
      btnCommon->addAction(new SWFAction(GetCharFromCString(scriptClicked)), SWFBUTTON_OVERDOWNTOOVERUP);
   }

   // Add ActionScript for "mouse up" event
   if (scriptMouseUp != NULL)
   {
      //CString csMouseUp;
      //csMouseUp.Format(_T("%s; if (g_bIsPlaying){lecDoPlay();}"), scriptClicked);
      //btnCommon->addAction(new SWFAction(csMouseUp), SWFBUTTON_MOUSEUP);
      PrintActionScript(csButtonName, scriptMouseUp);
      btnCommon->addAction(new SWFAction(GetCharFromCString(scriptMouseUp)), SWFBUTTON_MOUSEUP);
   }

   // Add ActionScript for "mouse down" event
   if (scriptMouseDown != NULL)
   {
      PrintActionScript(csButtonName, scriptMouseDown);
      btnCommon->addAction(new SWFAction(GetCharFromCString(scriptMouseDown)), SWFBUTTON_MOUSEDOWN);
   }

   // Add ActionScript for "mouse over" event
   if (scriptMouseOver != NULL)
   {
      PrintActionScript(csButtonName, scriptMouseOver);
      btnCommon->addAction(new SWFAction(GetCharFromCString(scriptMouseOver)), SWFBUTTON_MOUSEOVER);
   }

   // Add ActionScript for "mouse out" event
   if (scriptMouseOut != NULL)
   {
      PrintActionScript(csButtonName, scriptMouseOut);
      btnCommon->addAction(new SWFAction(GetCharFromCString(scriptMouseOut)), SWFBUTTON_MOUSEOUT);
   }

   // Tooltip ("umlauts" are only supported with Flash version > 5)
   if ((m_iFlashVersion > 5) && (!csButtonTitle.IsEmpty()))
      CreateTooltip(btnCommon, csButtonTitle, _T("topright"));


   // Add the button to the new SWFSprite
   di = sprCommon->add(btnCommon);
   CString csId;
   csId.Format(_T("swfBtn%s"), csButtonName);
   di->setName(GetCharFromCString(csId));
   // For Flash version 6 and higher: useHandCursor works this way
   CString csUseHand = pInteractionArea->IsHandCursor() ? _T("true") : _T("false");
   _TCHAR script[1024];
   _stprintf(script, _T("\
      %s.useHandCursor = %s;\
   "), csId, csUseHand);
   PrintActionScript(_T("Interactive Object"), script);
   sprCommon->add(new SWFAction(GetCharFromCString(script)));

   sprCommon->nextFrame();
}

void CFlashEngine::DrawDisabledInteractionButton(SWFSprite *sprPages, float x, float y, float width, float height,
                                                 int idx, CString csButtonName, 
                                                 CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *paInactiveDrawObjects)
{
   // Disabled button must be independent from the pages (/movie) content --> use a new SWFSprite here
   SWFSprite *sprCommon = new SWFSprite();

   SWFDisplayItem *di;

   // We have to determine the relative position of the text related to the shape objects.
   // Text is (normally) the last element of the object array, so we have the already 
   // the shape object position before we subtract the text position.
   float xText = 0.0f, yText = 0.0f;
   float xObj = 0.0f, yObj = 0.0f;
   for (int i = 0; i < paInactiveDrawObjects->GetSize(); ++i)
   {
      DrawSdk::DrawObject *pObject = paInactiveDrawObjects->ElementAt(i);

      // Button's DrawObject type can be one of the following:
      // RECTANGLE, TEXT, IMAGE
      int iType = pObject->GetType();

      if (iType == DrawSdk::RECTANGLE || iType == DrawSdk::IMAGE)
      {
         xObj = (float)(pObject->GetX());
         yObj = (float)(pObject->GetY());
      }
      if (iType == DrawSdk::TEXT)
      {
         // Text must be positioned relative to the RECTANGLE/IMAGE position
         xText = (float)(pObject->GetX()) - xObj;
         yText = (float)(pObject->GetY()) - yObj;
      }

      // Create sprite/shape
      if (iType == DrawSdk::TEXT)
      {
         SWFSprite *sprText = new SWFSprite();
         CreateSpriteFromButtonTextObject(sprText, pObject, xText, yText);
         di = sprCommon->add(sprText);
      }
      else
      {
         SWFShape *shpGraph = new SWFShape();
         CreateShapeFromButtonGraphicalObject(shpGraph, pObject);
         di = sprCommon->add(shpGraph);
         // Image size correction
         if (iType == DrawSdk::IMAGE)
            di->scale(1.0f / m_dGlobalScale);
      }
   }

   sprCommon->nextFrame();

   // Add the SWFSprite to the parent
   di = sprPages->add(sprCommon);
   di->moveTo(x, y);
//   di->setDepth(15500 + (2*idx) + 0);
   m_iMaxInteractionIndex++;
//#ifdef _DEBUG
   _tprintf(_T("+ DisabledInteractionButton Index: %d\n"), m_iMaxInteractionIndex);
//#endif
   di->setDepth(m_iMaxInteractionIndex);
   CString csId;
   csId.Format(_T("swfSpr%s_Disabled"), csButtonName);
   di->setName(GetCharFromCString(csId));
}

void CFlashEngine::DrawInteractionButtonShape(SWFButton *pButton, CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> *paDrawObjects, 
                                              byte iFlag)
{
   // We have to determine the relative position of the text related to the shape objects.
   // Text is (normally) the last element of the object array, so we have already 
   // the shape object position before we subtract the text position.
   float xText = 0.0f, yText = 0.0f;
   float xObj = 0.0f, yObj = 0.0f;
   for (int i = 0; i < paDrawObjects->GetSize(); ++i)
   {
      DrawSdk::DrawObject *pObject = paDrawObjects->ElementAt(i);

      // Button's DrawObject type can be one of the following:
      // RECTANGLE, TEXT, IMAGE, POLYGON(added for callouts)
      int iType = pObject->GetType();

      if (iType == DrawSdk::RECTANGLE || iType == DrawSdk::IMAGE)
      {
         xObj = (float)(pObject->GetX());
         yObj = (float)(pObject->GetY());
      }
      if (iType == DrawSdk::POLYGON)
      {
          DrawSdk::Polygon* pPoly = (DrawSdk::Polygon*)pObject;
          pPoly->CalculateDimension();
          xObj = (float)pPoly->GetX();
          yObj = (float)pPoly->GetY();
      }
      if (iType == DrawSdk::TEXT)
      {
         // Text must be positioned relative to the RECTANGLE/IMAGE position
         xText = (float)(pObject->GetX()) - xObj;
         yText = (float)(pObject->GetY()) - yObj;
      }

      // Create sprite/shape
      if (iType == DrawSdk::TEXT)
      {
         SWFSprite *sprText = new SWFSprite();
         CreateSpriteFromButtonTextObject(sprText, pObject, xText, yText);
         pButton->addShape(sprText, iFlag);
      }
      else
      {
         SWFShape *shpGraph = new SWFShape();
         CreateShapeFromButtonGraphicalObject(shpGraph, pObject);
         //pButton->addShape(shpGraph, iFlag);
         if (iType == DrawSdk::IMAGE)
         {
            // Image size correction
            SWFDisplayItem *di;
            SWFSprite *pDummy = new SWFSprite();
            di = pDummy->add(shpGraph);
            di->scale(1.0f / m_dGlobalScale);
            pDummy->nextFrame();
            pButton->addShape(pDummy, iFlag);
         }
         else
            pButton->addShape(shpGraph, iFlag);
      }
   }
}

UINT CFlashEngine::CreateInteractionAreas(SWFSprite *sprPages)
{
   HRESULT hr = S_OK;

   int nNumOfAreas = m_aInteractionAreas.GetSize();

   for (int i = 0; SUCCEEDED(hr) && i < nNumOfAreas; ++i)
   {
      // CheckBoxes, RadioButtons and TextFields are handled separately from the other interaction areas
      if (m_aInteractionAreas.ElementAt(i)->IsCheckBox())
      {
         CRect rcTarget = m_aInteractionAreas.ElementAt(i)->GetActivityArea();
         float x = (float)rcTarget.left;
         float y = (float)rcTarget.top;
         x += 0.0f; y += 2.0f; // empirically found
         float width = (float)rcTarget.Width();
         float height = (float)rcTarget.Height();
         float size = (0.5f * (width + height)) - 3.0f; // -3: empirically found

         hr = CreateCheckBox(sprPages, x, y, size, i);
      }
      else if (m_aInteractionAreas.ElementAt(i)->IsRadioButton())
      {
         CRect rcTarget = m_aInteractionAreas.ElementAt(i)->GetActivityArea();
         float x = (float)rcTarget.left;
         float y = (float)rcTarget.top;
         x += 0.0f; y += 2.0f; // empirically found
         float width = (float)rcTarget.Width();
         float height = (float)rcTarget.Height();
         float size = (0.5f * (width + height)) - 3.0f; // -3: empirically found
         int idxStart = -1, idxEnd = -1; // indices for button group
         GetIndexRangeForRadioButton(i, idxStart, idxEnd);
         if ( (idxStart < 0) || (idxEnd < 0) )
         {
            // This should not happen ...
            hr = imc_lecturnity_converter_LPLibs_WHITEBOARD_CORRUPT;
            // TODO: error handling?
            _tprintf(_T("Error: No valid index range for radio buttons! \n"));
         }
         _tprintf(_T("+ RadioButton: index: %d, range: %d - %d\n"), i, idxStart, idxEnd);

         if (SUCCEEDED(hr))
            hr = CreateRadioButton(sprPages, x, y, size, i, idxStart, idxEnd);
      }
      else if (m_aInteractionAreas.ElementAt(i)->IsTextField())
      {
         CRect rcTarget = m_aInteractionAreas.ElementAt(i)->GetActivityArea();
         float x = (float)rcTarget.left;
         float y = (float)rcTarget.top;
         x += 2.0f; y += 1.0f; // empirically found
         float width = (float)rcTarget.Width();
         float height = (float)rcTarget.Height();
         width -=5.0f; height -=5.0f; // empirically found

         hr = CreateTextField(sprPages, x, y, width, height, i);
      }
      else if (!m_aInteractionAreas.ElementAt(i)->IsSupport())
      {
         // The corresponding page of an interaction area is only necessary for testing
         int nPage = GetPageFromInteractionArea(m_aInteractionAreas.ElementAt(i));

         // Define ActionScript for Event-Handling
         CString csMouseClicked = m_aInteractionAreas.ElementAt(i)->GetMouseClickAction();
         csMouseClicked = ReinterpretActionStringForFlash(csMouseClicked, nPage);
         _TCHAR scriptClicked[1024];
         _stprintf(scriptClicked, _T("\
            _parent._parent.%s;\
         "), csMouseClicked);

         CString csMouseUp = m_aInteractionAreas.ElementAt(i)->GetMouseUpAction();
         csMouseUp = ReinterpretActionStringForFlash(csMouseUp);
         _TCHAR scriptMouseUp[1024];
         _stprintf(scriptMouseUp, _T("\
            _parent._parent.%s;\
         "), csMouseUp);

         CString csMouseDown = m_aInteractionAreas.ElementAt(i)->GetMouseDownAction();
         csMouseDown = ReinterpretActionStringForFlash(csMouseDown);
         _TCHAR scriptMouseDown[1024];
         _stprintf(scriptMouseDown, _T("\
            _parent._parent.%s;\
         "), csMouseDown);

         CString csMouseEnter = m_aInteractionAreas.ElementAt(i)->GetMouseEnterAction();
         csMouseEnter = ReinterpretActionStringForFlash(csMouseEnter);
         _TCHAR scriptMouseOver[1024];
         _stprintf(scriptMouseOver, _T("\
            _parent._parent.%s;\
         "), csMouseEnter);

         CString csMouseLeave = m_aInteractionAreas.ElementAt(i)->GetMouseLeaveAction();
         csMouseLeave = ReinterpretActionStringForFlash(csMouseLeave);
         _TCHAR scriptMouseOut[1024];
         _stprintf(scriptMouseOut, _T("\
            _parent._parent.%s;\
         "), csMouseLeave);

         CRect rcTarget = m_aInteractionAreas.ElementAt(i)->GetActivityArea();
         float x = (float)rcTarget.left;
         float y = (float)rcTarget.top;
         float width = (float)rcTarget.Width();
         float height = (float)rcTarget.Height();

         CreateInteractionArea(sprPages, x, y, width, height,
            i, m_aInteractionAreas.ElementAt(i), 
            scriptClicked, scriptMouseUp, scriptMouseDown, scriptMouseOver, scriptMouseOut);
      }
   }

   return hr; 
}

UINT CFlashEngine::CreateDragableImages(SWFSprite *sprPages)
{
   HRESULT hr = S_OK;

   int idxDragImg = 0;
   int idxTarget = 0;

   bool bHasDragableImages = false;
   for (int i = 0; SUCCEEDED(hr) && i < m_aInteractionObjects.GetSize(); ++i)
   {
      DrawSdk::DrawObject* pDrawObject = m_aInteractionObjects.ElementAt(i);

      // Look for dragable image
      if ( (pDrawObject->GetType() == DrawSdk::IMAGE) && (pDrawObject->IsDndMoveable()) )
      {
         bHasDragableImages = true;
         float x = (float)pDrawObject->GetX();
         float y = (float)pDrawObject->GetY();
         float width = (float)pDrawObject->GetWidth();
         float height = (float)pDrawObject->GetHeight();
         SWFShape *pShape = new SWFShape();
         CreateShapeFromButtonGraphicalObject(pShape, pDrawObject);

         hr = CreateDragableImage(sprPages, pShape, x, y, width, height, idxDragImg);

         idxDragImg++;
      }
      // Look for target area
      if (pDrawObject->GetType() == DrawSdk::TARGET)
      {
         DrawSdk::TargetArea* pDrawObjectTarget = (DrawSdk::TargetArea*)pDrawObject;

         hr = CreateTargetArea(sprPages, idxTarget, pDrawObjectTarget);

         idxTarget++;
      }
   }

   if (SUCCEEDED(hr) && bHasDragableImages)
   {
      // Create a dummy (image) sprite for depth swapping
      SWFDisplayItem *di;
      SWFSprite *sprDragImgDummy = new SWFSprite();
      sprDragImgDummy->nextFrame();
      di = sprPages->add(sprDragImgDummy);
      di->setName("swfSprDragImg_Dummy");
      //m_iMaxIndexSoFar++;
      int iMaxIndex = 15895;
//#ifdef _DEBUG
      _tprintf(_T("+ DragableImage (Dummy)     Index: %d\n"), iMaxIndex);
//#endif
      di->setDepth(iMaxIndex);
   }

   return hr;
}

UINT CFlashEngine::CreateChangeableTextObjects(SWFSprite *sprPages)
{
   HRESULT hr = S_OK;

   int idx = 0;
   for (int i = 0; SUCCEEDED(hr) && i < m_aInteractionAreas.GetSize(); ++i)
   {
      DrawSdk::DrawObject* pDrawObject = m_aInteractionAreas.ElementAt(i)->GetSupportObject();
      if ((pDrawObject != NULL) && (pDrawObject->GetType() == DrawSdk::TEXT) 
         && ((DrawSdk::Text *)pDrawObject)->HasTextType())
      {
         CQuestionnaire *pQuestionnaire = m_aQuestionnaires.GetSize() > 0 ? m_aQuestionnaires[0] : NULL;
         DrawSdk::TextTypeId idTextType = ((DrawSdk::Text *)pDrawObject)->GetTextType();
         boolean bIsChangeableText = ((idTextType == DrawSdk::TEXT_TYPE_CHANGE_TIME) 
                                     || (idTextType == DrawSdk::TEXT_TYPE_CHANGE_TRIES)
                                     || (pQuestionnaire != NULL && pQuestionnaire->IsRandomTest() && idTextType == DrawSdk::TEXT_TYPE_CHANGE_NUMBER) // Exception for random test
                                     ) ? true : false;
         if (bIsChangeableText)
         {
            // Changeable text found
            float x = (float)pDrawObject->GetX();
            float y = (float)pDrawObject->GetY();
            float width = (float)pDrawObject->GetWidth();
            float height = (float)pDrawObject->GetHeight();
            DrawSdk::Text *pTextObject = (DrawSdk::Text *)pDrawObject;
            int nStartMs = m_aInteractionAreas[i]->GetVisibilityStart();
            hr = CreateInteractiveText(sprPages, pTextObject, _T("Change"), x, y, width, height, idx, nStartMs);
            idx++;
         }
      }
   }

   return hr;
}

UINT CFlashEngine::CreateQuestionTextObjects(SWFSprite *sprPages)
{
   HRESULT hr = S_OK;

   int idx = 0;
   for (int i = 0; SUCCEEDED(hr) && i < m_aInteractionAreas.GetSize(); ++i)
   {
      DrawSdk::DrawObject* pDrawObject = m_aInteractionAreas.ElementAt(i)->GetSupportObject();
      if ((pDrawObject != NULL) && (pDrawObject->GetType() == DrawSdk::TEXT) 
         && ((DrawSdk::Text *)pDrawObject)->HasTextType())
      {
         CQuestionnaire *pQuestionnaire = m_aQuestionnaires.GetSize() > 0 ? m_aQuestionnaires[0] : NULL;
         DrawSdk::TextTypeId idTextType = ((DrawSdk::Text *)pDrawObject)->GetTextType();
         boolean bIsChangeableText = ((idTextType == DrawSdk::TEXT_TYPE_CHANGE_TIME) 
                                     || (idTextType == DrawSdk::TEXT_TYPE_CHANGE_TRIES)
                                     || (pQuestionnaire != NULL && pQuestionnaire->IsRandomTest() && idTextType == DrawSdk::TEXT_TYPE_CHANGE_NUMBER) // Exception for random test
                                     ) ? true : false;
         boolean bIsFeedbackText = ((idTextType >= DrawSdk::TEXT_TYPE_EVAL_Q_CORRECT) 
                                    && idTextType <= DrawSdk::TEXT_TYPE_EVAL_QQ_REQUIRED_PERCENT) ? true : false;
         if (!bIsChangeableText && !bIsFeedbackText && !(idTextType == DrawSdk::TEXT_TYPE_NOTHING))
         {
            // Question text found
            float x = (float)pDrawObject->GetX();
            float y = (float)pDrawObject->GetY();
            float width = (float)pDrawObject->GetWidth();
            float height = (float)pDrawObject->GetHeight();
            DrawSdk::Text *pTextObject = (DrawSdk::Text *)pDrawObject;
            int nStartMs = m_aInteractionAreas[i]->GetVisibilityStart();
            hr = CreateInteractiveText(sprPages, pTextObject, _T("Question"), x, y, width, height, idx, nStartMs);
            idx++;
         }
      }
   }

   return hr;
}

UINT CFlashEngine::CreateSpriteFromButtonTextObject(SWFSprite *pSprite, DrawSdk::DrawObject *pObject, float x, float y)
{
   HRESULT hr = S_OK;

   SWFDisplayItem *di;

   // Button's DrawObject type can be one of the following:
   // RECTANGLE, TEXT, IMAGE
   int iType = pObject->GetType();

   SWFShape *pShape = new SWFShape();
   SWFBlock *pBlock = pShape;

   float fLineWidth = 0.0f;
   bool bDashedLine = false;
   bool bDottedDash = false;

   // set properties common to all ColorObjects
   if (iType != DrawSdk::IMAGE && iType != DrawSdk::TELEPOINTER)
   {
      DrawSdk::ColorObject *pColorObject = (DrawSdk::ColorObject *)pObject;
      Gdiplus::ARGB ocref = pColorObject->GetOutlineColor();
      fLineWidth = pColorObject->GetLineWidth();

      pShape->setLine(fLineWidth, (ocref >> 16) & 0xff, 
         (ocref >> 8) & 0xff, (ocref >> 0) & 0xff, (ocref >> 24) & 0xff);
         
      if (pColorObject->GetIsFilled())
      {
         Gdiplus::ARGB fcref = pColorObject->GetFillColor();
         SWFFill *pFill = pShape->addSolidFill((fcref >> 16) & 0xff, 
            (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
         pShape->setRightFill(pFill);
      }

      int nLineStyle = pColorObject->GetLineStyle();
      if (nLineStyle != 0)
      {
         bDashedLine = true;
         bDottedDash = nLineStyle == 1;
      }
   }

   float width = (float)pObject->GetWidth(), height = (float)pObject->GetHeight();

   if (iType == DrawSdk::TEXT)
   {
      // Note: Text does not use a normal SWFShape
      SWFText *pSwfText = new SWFText();

      DrawSdk::Text *pTextObject = (DrawSdk::Text *)pObject;

      hr = MapFlashFontFromTextObject(pTextObject);

      LOGFONT lf = pTextObject->GetLogFont();

      CString csTtfIn = pTextObject->GetTtfName();
      if (csTtfIn.IsEmpty())
      {
         // if no ttf is specified use the face name for the hash key

         csTtfIn = lf.lfFaceName;
      }
      SWFFont *pSwfFont = GetFlashFontFromTextObject(pTextObject);

      if (pSwfFont)
      {
         Gdiplus::ARGB crFontColor = pTextObject->GetOutlineColor();
         CString csText = pTextObject->GetString();
         float fFontSize = -1.0f * lf.lfHeight;
         float fLength = (float)pTextObject->GetWidth();
         bool bIsUnderlined = lf.lfUnderline ? true : false;
         
         if (fFontSize > 0)
         {
            DrawText(pSwfText, x, y, pSwfFont, csText, fFontSize, crFontColor);

            if (bIsUnderlined)
            {
               float fLength = (float)pTextObject->GetWidth();
               DrawUnderline(pSprite, x, y, fLength, lf, crFontColor);
            }
         }
         // else negative font sizes lead to really big texts
      }
      
      pBlock = pSwfText;
      di = pSprite->add(pBlock);
   }

   pSprite->nextFrame();

   return hr; 
}

UINT CFlashEngine::CreateShapeFromButtonGraphicalObject(SWFShape *pShape, DrawSdk::DrawObject *pObject)
{
   HRESULT hr = S_OK;

   CMap<CString, LPCTSTR, SWFShape*, SWFShape*> mapImages;
   CMap<SWFBlock*, SWFBlock*, float, float> mapImageScalingX;
   CMap<SWFBlock*, SWFBlock*, float, float> mapImageScalingY;

   SWFDisplayItem *di = NULL;

   // Button's DrawObject type can be one of the following:
   // RECTANGLE, TEXT, IMAGE, POLYGON(added for callouts)
   int iType = pObject->GetType();

   SWFBlock *pBlock = pShape;

   float fLineWidth = 0.0f;
   bool bDashedLine = false;
   bool bDottedDash = false;

   // set properties common to all ColorObjects
   if (iType != DrawSdk::IMAGE && iType != DrawSdk::TELEPOINTER)
   {
      DrawSdk::ColorObject *pColorObject = (DrawSdk::ColorObject *)pObject;
      Gdiplus::ARGB ocref = pColorObject->GetOutlineColor();
      fLineWidth = pColorObject->GetLineWidth();

      pShape->setLine(fLineWidth, (ocref >> 16) & 0xff, 
         (ocref >> 8) & 0xff, (ocref >> 0) & 0xff, (ocref >> 24) & 0xff);
      
      if (pColorObject->GetIsFilled())
      {
         Gdiplus::ARGB fcref = pColorObject->GetFillColor();
         SWFFill *pFill = pShape->addSolidFill((fcref >> 16) & 0xff, 
            (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
         pShape->setRightFill(pFill);
      }

      int nLineStyle = pColorObject->GetLineStyle();
      if (nLineStyle != 0)
      {
         bDashedLine = true;
         bDottedDash = nLineStyle == 1;
      }
   }

   // shorthand declarations
   float x = 0.0f, y = 0.0f;
   float width = (float)pObject->GetWidth(), height = (float)pObject->GetHeight();
   float fImageWidth = 0.0f, fImageHeight = 0.0f; // might be needed for scaling

   switch (iType)
   {
      case DrawSdk::RECTANGLE:
         {
            DrawRectangle(pShape, x, y, width, height, fLineWidth, bDashedLine, bDottedDash);
         }
         break;
      
      case DrawSdk::IMAGE:
         {
            DrawSdk::Image *pImageObject = (DrawSdk::Image *)pObject;

            hr = MapFlashImageFromImageObject(pImageObject, &pShape, &pBlock, 
               mapImages, mapImageScalingX, mapImageScalingY);

            SWFSprite *pSprite = new SWFSprite(); // dummy sprite for getting 'di'
            di = pSprite->add(pBlock);
         }
         break;
      case DrawSdk::POLYGON:
          {
              DrawSdk::Polygon* pPoly = (DrawSdk::Polygon*)pObject;
              
              DrawSdk::DPoint aPoints[8];
              pPoly->GetPoints(aPoints, 8);
              pPoly->CalculateDimension();
              for(int i = 0; i < 8; i++)
              {
                  aPoints[i].x -= pPoly->GetX();
                  aPoints[i].y -= pPoly->GetY();
              }
              DrawPolygon(pShape, aPoints, 8, fLineWidth, bDashedLine, bDottedDash);
          }
          break;

       default:
         // do nothing
         break;
   } // end switch (itype)

   if (pBlock && iType == DrawSdk::IMAGE)
   {
      di->moveTo(x, y);

      float fFactorX = 1.0;
      float fFactorY = 1.0;
      if (mapImageScalingX.Lookup(pBlock, fFactorX) && mapImageScalingY.Lookup(pBlock, fFactorY))
         di->scaleTo(fFactorX, fFactorY);
   }

   return hr; 
}

int CFlashEngine::GetFrameFromTimestamp(int nTimestamp)
{
   if (nTimestamp < 0)
      return -1;

   float fFrame = (m_fFrameRate * (float)nTimestamp) / 1000.0f;
   int nFrame = (int)fFrame;
   int msPerFrame = (int)(1000.0f / m_fFrameRate);
   if (nTimestamp % msPerFrame != 0)
      nFrame += 1;

   // Check frame timestamps containing stop marks --> correct frame, if necessary
   for (int iFrame = 0; iFrame < m_aiFrameTimestampsMs.GetSize(); ++iFrame)
   {
      int nTimeMs = m_aiFrameTimestampsMs.ElementAt(iFrame);
      if (nTimeMs == nTimestamp)
      {
         nFrame = iFrame;
         break;
      }
   }

   // Check if this timestamp belongs to a stop mark
   bool bIsStopmark = false;
   for (int k = 0; k < m_aFlashStopmarks.GetSize(); ++k)
   {
      if (nTimestamp == m_aFlashStopmarks.ElementAt(k).nMsTime)
      {
         bIsStopmark = true;
         // Get the corresponding frame (-1; +1 is added later on again)
         nFrame = m_aFlashStopmarks.ElementAt(k).nFrame - 1;
         break;
      }
   }

   // Check, if the the timestamp is 1 msec before paging --> use the previous frame
   // (only if this is not already a stop mark)
   if (!bIsStopmark)
   {
      // Get the paging timestamps
      int nLength = m_iMovieLengthMs;
      if (m_iMovieLengthMs < 0)
         nLength = m_pWhiteboardStream->GetLength();
      CArray<CPage*, CPage*>aPages;
      aPages.RemoveAll();
      int nFirst, nLast;
      m_pPageStream->GetPages(aPages, 0, nLength, 0, nFirst, nLast);
      int numOfPages = aPages.GetSize();

      for (int i = 0; i < numOfPages; ++i)
      {
         int pageBeginMs = aPages[i]->GetBegin();
         if ( (nTimestamp == (pageBeginMs-1)) && (nTimestamp > 0) )
            nFrame -= 1;
      }
   }

   // Note: first frame is frame 1 (not 0) --> add one frame here
   nFrame += 1;

#ifdef _DEBUG
   _tprintf(_T("GetFrameFromTimestamp(%d) --> %d\n"), nTimestamp, nFrame);
#endif

   return nFrame;
}

void CFlashEngine::GetIndexRangeForRadioButton(int idx, int &idxStart, int &idxEnd)
{
   idxStart = -1, idxEnd = -1;

   // Get activity time of the radio button with the given index
   CInteractionArea *pInteractionArea = m_aInteractionAreas.ElementAt(idx);
   CArray<CPoint, CPoint> aTimePeriodsActivity;
   aTimePeriodsActivity.RemoveAll();
   pInteractionArea->GetActivityTimes(&aTimePeriodsActivity);
   CPoint ptTimePeriodActivity(-1, -1);
   if (aTimePeriodsActivity.GetSize() > 0)
      ptTimePeriodActivity = aTimePeriodsActivity.ElementAt(0);
   // TODO: Get all time periods 
   // <-> but so far only one time period is implemented --> nothing to do here
   int nThisTimeMsBegin  = ptTimePeriodActivity.x;
   int nThisTimeMsEnd    = ptTimePeriodActivity.y; 

   // Now look for all interaction areas with the same activity time
   for (int i = 0; i < m_aInteractionAreas.GetSize(); ++i)
   {
      pInteractionArea = m_aInteractionAreas.ElementAt(i);
      if (pInteractionArea->IsRadioButton())
      {
         aTimePeriodsActivity.RemoveAll();
         pInteractionArea->GetActivityTimes(&aTimePeriodsActivity);
         ptTimePeriodActivity.x = -1;
         ptTimePeriodActivity.y = -1;
         if (aTimePeriodsActivity.GetSize() > 0)
            ptTimePeriodActivity = aTimePeriodsActivity.ElementAt(0);
         int nTimeMsBegin  = ptTimePeriodActivity.x;
         int nTimeMsEnd    = ptTimePeriodActivity.y; 

         if ( (nTimeMsBegin >= nThisTimeMsBegin) && (nTimeMsEnd <= nThisTimeMsEnd) )
         {
            if (idxStart < 0)
               idxStart = i;
            idxEnd = i;
         }
      }
   }
}

UINT CFlashEngine::CreateCheckBox(SWFSprite *sprPages, float x, float y, float size, int idx)
{
   HRESULT hr = S_OK;

   SWFDisplayItem *di = NULL;
   _TCHAR script[4096];

   // Create a SWFSprite as a container
   SWFSprite *sprChkContainer = new SWFSprite();
   // Create a SWFSprite with 4 frames (1 frame for each state)
   SWFSprite *sprCheckBox = new SWFSprite();

   SWFFill *fillChk;

   // Frame 1: unchecked state
   SWFShape *shpChk1 = new SWFShape();
   fillChk = shpChk1->addSolidFill(0xFF, 0xFF, 0xFF);
   shpChk1->setRightFill(fillChk);
   shpChk1->setLine(1, 0x00, 0x00, 0x00);
   DrawRectangle(shpChk1, x, y, size, size);
   di = sprCheckBox->add(shpChk1);
   _stprintf(script, _T("\
      stop();\
   "));
   sprCheckBox->add(new SWFAction(GetCharFromCString(script)));
   sprCheckBox->nextFrame();

   // Frame 2: checked state
   SWFShape *shpChk2 = new SWFShape();
   fillChk = shpChk2->addSolidFill(0xFF, 0xFF, 0xFF);
   shpChk2->setRightFill(fillChk);
   shpChk2->setLine(1, 0x00, 0x00, 0x00);
   DrawRectangle(shpChk2, x, y, size, size);
   // Inner tickmark
   shpChk2->setLine(2, 0x21, 0xA1, 0x21);
   shpChk2->movePenTo(x+0.7f*size, y+0.3f*size);
   shpChk2->drawLineTo(x+0.5f*size, y+0.7f*size);
   shpChk2->drawLineTo(x+0.35f*size, y+0.5f*size);
   shpChk2->drawLineTo(x+0.5f*size, y+0.7f*size);
   shpChk2->drawLineTo(x+0.7f*size, y+0.3f*size);
   sprCheckBox->remove(di);
   di = sprCheckBox->add(shpChk2);
   _stprintf(script, _T("\
      stop();\
   "));
   sprCheckBox->add(new SWFAction(GetCharFromCString(script)));
   sprCheckBox->nextFrame();

   // Frame 3: disabled unchecked state
   SWFShape *shpChk3 = new SWFShape();
   fillChk = shpChk3->addSolidFill(0xFF, 0xFF, 0xFF);
   shpChk3->setRightFill(fillChk);
   shpChk3->setLine(1, 0xA0, 0xA0, 0xA0);
   DrawRectangle(shpChk3, x, y, size, size);
   sprCheckBox->remove(di);
   di = sprCheckBox->add(shpChk3);
   _stprintf(script, _T("\
      stop();\
   "));
   sprCheckBox->add(new SWFAction(GetCharFromCString(script)));
   sprCheckBox->nextFrame();

   // Frame 4: disabled checked state
   SWFShape *shpChk4 = new SWFShape();
   fillChk = shpChk4->addSolidFill(0xFF, 0xFF, 0xFF);
   shpChk4->setRightFill(fillChk);
   shpChk4->setLine(1, 0xA0, 0xA0, 0xA0);
   DrawRectangle(shpChk4, x, y, size, size);
   // Inner tickmark
   shpChk4->setLine(2, 0xC0, 0xC0, 0xC0);
   shpChk4->movePenTo(x+0.7f*size, y+0.3f*size);
   shpChk4->drawLineTo(x+0.5f*size, y+0.7f*size);
   shpChk4->drawLineTo(x+0.35f*size, y+0.5f*size);
   shpChk4->drawLineTo(x+0.5f*size, y+0.7f*size);
   shpChk4->drawLineTo(x+0.7f*size, y+0.3f*size);
   sprCheckBox->remove(di);
   di = sprCheckBox->add(shpChk4);
   _stprintf(script, _T("\
      stop();\
   "));
   sprCheckBox->add(new SWFAction(GetCharFromCString(script)));
   sprCheckBox->nextFrame();

   di = sprChkContainer->add(sprCheckBox);
   CString csName = _T("swfSprChk");
   di->setName(GetCharFromCString(csName));

   // Create a button for the checkbox
   SWFButton *btnChk = new SWFButton();
   SWFShape *shpBtnChkUp = new SWFShape();
   // Yellow square for the mouse over effect
   SWFShape *shpBtnChkOver = new SWFShape();
   fillChk = shpBtnChkOver->addSolidFill(0x00, 0x80, 0x00, 0x00);
   shpBtnChkOver->setRightFill(fillChk);
   shpBtnChkOver->setLine(2, 0xFC, 0xD3, 0x7C, 0xCC);
   DrawRectangle(shpBtnChkOver, x+1.5f, y+1.5f, size-3.0f, size-3.0f);

   btnChk->addShape(shpBtnChkUp, SWFBUTTON_UP);
   btnChk->addShape(shpBtnChkOver, SWFBUTTON_HIT);
   btnChk->addShape(shpBtnChkOver, SWFBUTTON_OVER);
   btnChk->addShape(shpBtnChkOver, SWFBUTTON_DOWN);

   csName = "swfSprInteraction_";
   _stprintf(script, _T("\
      var objRoot = _parent._parent;\
      var bIsDeactivated = false;\
      var actualFrame = objRoot._currentframe;\
      var qIdx = objRoot.getQuestionIndexFromFrame(actualFrame);\
      if (qIdx >= 0)\
         bIsDeactivated = objRoot.g_aQuestionEntries[qIdx].bIsDeactivated;\
      if (!bIsDeactivated)\
      {\
         if (objRoot.swfSprPages.%s%d.bIsChecked == true)\
         {\
            objRoot.swfSprPages.%s%d.swfSprChk.gotoAndStop(1);\
            objRoot.swfSprPages.%s%d.bIsChecked = false;\
         }\
         else\
         {\
            objRoot.swfSprPages.%s%d.swfSprChk.gotoAndStop(2);\
            objRoot.swfSprPages.%s%d.bIsChecked = true;\
         }\
      }\
   "), csName, idx, csName, idx, csName, idx, csName, idx, csName, idx);
   btnChk->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEDOWN);

   di = sprChkContainer->add(btnChk);
   di->setName("swfBtnChk");
   sprChkContainer->nextFrame();

   di = sprPages->add(sprChkContainer);
   _stprintf(script, _T("%s%d"), csName, idx);
   di->setName(GetCharFromCString(script));
   m_iMaxInteractionIndex++;
//#ifdef _DEBUG
   _tprintf(_T("+ CheckBox                  Index: %d\n"), m_iMaxInteractionIndex);
//#endif
   di->setDepth(m_iMaxInteractionIndex);

   _stprintf(script, _T("\
      /*this.bIsChecked = false;*/\
   "));
   di->addAction(new SWFAction(GetCharFromCString(script)), SWFACTION_ENTERFRAME);

   return hr;
}

UINT CFlashEngine::CreateRadioButton(SWFSprite *sprPages, float x, float y, float size, int idx, int idxStart, int idxEnd)
{
   HRESULT hr = S_OK;

   SWFDisplayItem *di = NULL;
   _TCHAR script[4096];

   // Create a SWFSprite as a container
   SWFSprite *sprRdbContainer = new SWFSprite();
   // Create a SWFSprite with 4 frames (1 frame for each state)
   SWFSprite *sprRadio = new SWFSprite();

   SWFFill *fillRdb;

   // Frame 1: unchecked state
   SWFShape *shpRdb1 = new SWFShape();
   fillRdb = shpRdb1->addSolidFill(0xFF, 0xFF, 0xFF);
   shpRdb1->setRightFill(fillRdb);
   shpRdb1->setLine(1, 0x00, 0x00, 0x00);
   DrawOval(shpRdb1, x, y, size, size);
   di = sprRadio->add(shpRdb1);
   _stprintf(script, _T("\
      stop();\
   "));
   sprRadio->add(new SWFAction(GetCharFromCString(script)));
   sprRadio->nextFrame();

   // Frame 2: checked state
   SWFShape *shpRdb2 = new SWFShape();
   fillRdb = shpRdb2->addSolidFill(0xFF, 0xFF, 0xFF);
   shpRdb2->setRightFill(fillRdb);
   shpRdb2->setLine(1, 0x00, 0x00, 0x00);
   DrawOval(shpRdb2, x, y, size, size);
   // Inner dot
   fillRdb = shpRdb2->addSolidFill(0x21, 0xA1, 0x21);
   shpRdb2->setRightFill(fillRdb);
   shpRdb2->setLine(2, 0x21, 0xA1, 0x21);
   DrawOval(shpRdb2, x+0.35f*size, y+0.35f*size, 0.3f*size, 0.3f*size);
   sprRadio->remove(di);
   di = sprRadio->add(shpRdb2);
   _stprintf(script, _T("\
      stop();\
   "));
   sprRadio->add(new SWFAction(GetCharFromCString(script)));
   sprRadio->nextFrame();

   // Frame 3: disabled unchecked state
   SWFShape *shpRdb3 = new SWFShape();
   fillRdb = shpRdb3->addSolidFill(0xFF, 0xFF, 0xFF);
   shpRdb3->setRightFill(fillRdb);
   shpRdb3->setLine(1, 0xA0, 0xA0, 0xA0);
   DrawOval(shpRdb3, x, y, size, size);
   sprRadio->remove(di);
   di = sprRadio->add(shpRdb3);
   _stprintf(script, _T("\
      stop();\
   "));
   sprRadio->add(new SWFAction(GetCharFromCString(script)));
   sprRadio->nextFrame();

   // Frame 4: disabled checked state
   SWFShape *shpRdb4 = new SWFShape();
   fillRdb = shpRdb4->addSolidFill(0xFF, 0xFF, 0xFF);
   shpRdb4->setRightFill(fillRdb);
   shpRdb4->setLine(1, 0xA0, 0xA0, 0xA0);
   DrawOval(shpRdb4, x, y, size, size);
   // Inner dot
   fillRdb = shpRdb4->addSolidFill(0xC0, 0xC0, 0xC0);
   shpRdb4->setRightFill(fillRdb);
   shpRdb4->setLine(2, 0xC0, 0xC0, 0xC0);
   DrawOval(shpRdb4, x+0.35f*size, y+0.35f*size, 0.3f*size, 0.3f*size);
   sprRadio->remove(di);
   di = sprRadio->add(shpRdb4);
   _stprintf(script, _T("\
      stop();\
   "));
   sprRadio->add(new SWFAction(GetCharFromCString(script)));
   sprRadio->nextFrame();

   di = sprRdbContainer->add(sprRadio);
   CString csName = _T("swfSprRdb");
   di->setName(GetCharFromCString(csName));

   // Create a button for the radio button
   SWFButton *btnRdb = new SWFButton();
   SWFShape *shpBtnRdbUp = new SWFShape();
   // Yellow circle for mouse over effect
   SWFShape *shpBtnRdbOver = new SWFShape();
   fillRdb = shpBtnRdbOver->addSolidFill(0x00, 0x80, 0x00, 0x00);
   shpBtnRdbOver->setRightFill(fillRdb);
   shpBtnRdbOver->setLine(2, 0xFC, 0xD3, 0x7C, 0xCC);
   DrawOval(shpBtnRdbOver, x+1.5f, y+1.5f, size-3.0f, size-3.0f);

   btnRdb->addShape(shpBtnRdbUp, SWFBUTTON_UP);
   btnRdb->addShape(shpBtnRdbOver, SWFBUTTON_HIT);
   btnRdb->addShape(shpBtnRdbOver, SWFBUTTON_OVER);
   btnRdb->addShape(shpBtnRdbOver, SWFBUTTON_DOWN);

   csName = _T("swfSprInteraction_");
   _stprintf(script, _T("\
      var objRoot = _parent._parent;\
      var bIsDeactivated = false;\
      var actualFrame = objRoot._currentframe;\
      var qIdx = objRoot.getQuestionIndexFromFrame(actualFrame);\
      if (qIdx >= 0)\
         bIsDeactivated = objRoot.g_aQuestionEntries[qIdx].bIsDeactivated;\
      if (!bIsDeactivated)\
      {\
         for (var i = %d; i <= %d; ++i)\
         {\
            var obj = eval(\"_parent._parent.swfSprPages.%s\" + i);\
            if (obj)\
            {\
               obj.swfSprRdb.gotoAndStop(1);\
               obj.bIsChecked = false;\
            }\
         }\
         objRoot.swfSprPages.%s%d.swfSprRdb.gotoAndStop(2);\
         objRoot.swfSprPages.%s%d.bIsChecked = true;\
      }\
   "), idxStart, idxEnd, csName, csName, idx, csName, idx);
   btnRdb->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEDOWN);

   di = sprRdbContainer->add(btnRdb);
   di->setName("swfBtnRdb");
   sprRdbContainer->nextFrame();

   di = sprPages->add(sprRdbContainer);
   _stprintf(script, _T("%s%d"), csName, idx);
   di->setName(GetCharFromCString(script));
   m_iMaxInteractionIndex++;
//#ifdef _DEBUG
   _tprintf(_T("+ RadioButton:              Index: %d\n"), m_iMaxInteractionIndex);
//#endif
   di->setDepth(m_iMaxInteractionIndex);

   _stprintf(script, _T("\
      /*this.bIsChecked = false;*/\
   "));
   di->addAction(new SWFAction(GetCharFromCString(script)), SWFACTION_ENTERFRAME);

   return hr;
}

UINT CFlashEngine::CreateDragableImage(SWFSprite *sprPages, SWFShape *pShape, float x, float y, 
                                       float width, float height, int idx)
{
   HRESULT hr = S_OK;

   CString csId;
   _TCHAR sName[512];
   SWFDisplayItem *di = NULL;

   // Moveable image
   csId.Format(_T("DragImg_%d"), idx);
   SWFSprite *pSprImg = new SWFSprite();
   SWFButton *pBtnImg = new SWFButton();
   pBtnImg->addShape(pShape, SWFBUTTON_HIT);
   pBtnImg->addShape(pShape, SWFBUTTON_UP);
   pBtnImg->addShape(pShape, SWFBUTTON_OVER);
   pBtnImg->addShape(pShape, SWFBUTTON_DOWN);

   _TCHAR script[4096];
   _stprintf(script, _T("\
      this.deltaX = _xmouse - (0.5 * this._width);\
      this.deltaY = _ymouse - (0.5 * this._height);\
      var objRoot = _parent._parent;\
      var bIsDeactivated = false;\
      var actualFrame = objRoot._currentframe;\
      var qIdx = objRoot.getQuestionIndexFromFrame(actualFrame);\
      if (qIdx >= 0)\
         bIsDeactivated = objRoot.g_aQuestionEntries[qIdx].bIsDeactivated;\
      if (!bIsDeactivated)\
      {\
         startDrag(false, %g, %g, %g, %g);\
         objRoot.swfSprPages.swfSpr%s.swapDepths(objRoot.swfSprPages.swfSprDragImg_Dummy);\
      }\
   "), 0.0f, 0.0f, (m_iWbInputWidth-width), (m_iWbInputHeight-height), csId);
   pBtnImg->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEDOWN);
   _stprintf(script, _T("\
      var objRoot = _parent._parent;\
      var bIsDeactivated = false;\
      var actualFrame = objRoot._currentframe;\
      var qIdx = objRoot.getQuestionIndexFromFrame(actualFrame);\
      if (qIdx >= 0)\
         bIsDeactivated = objRoot.g_aQuestionEntries[qIdx].bIsDeactivated;\
      if (!bIsDeactivated)\
      {\
         stopDrag();\
         if (!this.isSnappedIn || this.isMasked)\
         {\
            this._x = %d;\
            this._y = %d;\
            this.isMasked = false;\
         }\
         objRoot.swfSprPages.swfSpr%s.swapDepths(objRoot.swfSprPages.swfSprDragImg_Dummy);\
      }\
   "), (int)(x+0.5f), (int)(y+0.5f), csId);
   pBtnImg->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEUP | SWFBUTTON_MOUSEUPOUTSIDE);

   di = pSprImg->add(pBtnImg);
   _stprintf(sName, _T("swfBtn%s"), csId);
//   di->setName(sName);
   pSprImg->nextFrame();
   di = sprPages->add(pSprImg);
   _stprintf(sName, _T("swfSpr%s"), csId);
   di->setName(GetCharFromCString(sName));
   di->moveTo(x, y);
   // Image size correction
   di->scale(1.0f / m_dGlobalScale);
   m_iMaxInteractionIndex++;
//#ifdef _DEBUG
   _tprintf(_T("+ DragableImage             Index: %d\n"), m_iMaxInteractionIndex);
//#endif
   di->setDepth(m_iMaxInteractionIndex);

   _stprintf(script, _T("\
      var objRoot = _parent._parent;\
      this.origX = %d;\
      this.origY = %d;\
      var bIsDeactivated = false;\
      var actualFrame = objRoot._currentframe;\
      var qIdx = objRoot.getQuestionIndexFromFrame(actualFrame);\
      if (qIdx >= 0)\
         bIsDeactivated = objRoot.g_aQuestionEntries[qIdx].bIsDeactivated;\
      if (!bIsDeactivated)\
      {\
         this.isSnappedIn = false;\
         this.isMasked = false;\
         objRoot.checkPos(this);\
      }\
   "), (int)(x+0.5f), (int)(y+0.5f));
   di->addAction(new SWFAction(GetCharFromCString(script)), SWFACTION_ENTERFRAME);

   return hr;
}

UINT CFlashEngine::CreateTargetArea(SWFSprite *sprPages, int idx, 
                                    DrawSdk::TargetArea* pDrawObjectTarget)
{
   HRESULT hr = S_OK;

   CString csId;
   _TCHAR sName[512];
   SWFDisplayItem *di = NULL;

   // Target area as rectangle with a black line
   if (pDrawObjectTarget != NULL)
   {
      int fX = pDrawObjectTarget->GetX();
      int fY = pDrawObjectTarget->GetY();
      int fWidth = pDrawObjectTarget->GetWidth();
      int fHeight = pDrawObjectTarget->GetHeight();

      csId.Format(_T("Target_%d"), idx);
      SWFSprite *pSprTarget = new SWFSprite();
      SWFShape *pShpTarget = new SWFShape();
      pShpTarget->setLine(1, 0x00, 0x00, 0x00);
      DrawRectangle(pShpTarget, 0.0f, 0.0f, fWidth, fHeight);
      di = pSprTarget->add(pShpTarget);
      pSprTarget->nextFrame();
      di = sprPages->add(pSprTarget);
      _stprintf(sName, _T("swfSpr%s"), csId);
      di->setName(GetCharFromCString(sName));
      di->moveTo(fX, fY);
      m_iMaxInteractionIndex++;
//#ifdef _DEBUG
      _tprintf(_T("+ TargetArea                Index: %d\n"), m_iMaxInteractionIndex);
//#endif
      di->setDepth(m_iMaxInteractionIndex);
   }

   return hr;
}

UINT CFlashEngine::CreateTextField(SWFSprite *sprPages, float x, float y, float width, float height, int idx)
{
   HRESULT hr = S_OK;

   CString csId;
   SWFDisplayItem *di = NULL;
   SWFSprite *sprTextField = new SWFSprite();

   SWFTextField *txtField = new SWFTextField();
   txtField->setFont(m_pFntStd);
   txtField->setBounds(width, height);
   txtField->setHeight(height - 2.5f); // empirically found
   txtField->setIndentation(0.5f);
   txtField->setColor(0x00, 0x00, 0x00);
   txtField->align(SWFTEXTFIELD_ALIGN_LEFT);
   txtField->setFlags(SWFTEXTFIELD_DRAWBOX | SWFTEXTFIELD_AUTOSIZE | SWFTEXTFIELD_WORDWRAP | SWFTEXTFIELD_HTML);
   txtField->addUTF8String("");
   txtField->setVariableName("text");
   di = sprTextField->add(txtField);
   di->setName("swfTxtField");
   sprTextField->nextFrame();
   di = sprPages->add(sprTextField);
   csId.Format(_T("swfSprInteraction_%d"), idx);
   di->setName(GetCharFromCString(csId));
   di->moveTo(x, y);
   m_iMaxInteractionIndex++;
//#ifdef _DEBUG
   _tprintf(_T("+ TextField                 Index: %d\n"), m_iMaxInteractionIndex);
//#endif
   di->setDepth(m_iMaxInteractionIndex);


   // Alternative to the code above: Dynamically created text field
   // + advantage: works on Sony PSP
   // - disadvantage: layering problems eith the feedback objects 
/*
   SWFDisplayItem *di = NULL;
   CString csIdDummy;
   // Dummy sprite to swap depth with the dynamically created textfield later on
   SWFSprite *sprTextField = new SWFSprite();
   sprTextField->nextFrame();
   di = sprPages->add(sprTextField);
   csIdDummy.Format(_T("swfSprInteraction_%d_Dummy"), idx);
   di->setName(GetCharFromCString(csIdDummy));
   m_iMaxInteractionIndex++;
   di->setDepth(m_iMaxInteractionIndex);
//#ifdef _DEBUG
   _tprintf(_T("+ TextField                 Index: %d\n"), m_iMaxInteractionIndex);
//#endif
*/

/*
   CString csId;
   //m_iMaxInteractionIndex++;
   csId.Format(_T("swfSprInteraction_%d"), idx);
   // 2 px margin: empirically found
   int nX = (int)(x - 2.0f);
   int nY = (int)(y - 2.0f);
   int nWidth = (int)(width + 4.0f);
   int nHeight = (int)(height + 4.0f);
   int nFontSize = (int)(height - 3.0f); // -3: empirically found
   _TCHAR script[1024];
   _stprintf(script, _T("\
      if (!_parent.g_bIsAlreadyLoaded)\
      {\
         this.createEmptyMovieClip(\"%s\", %d);\
         this.%s.createTextField(\"swfTxtField\", %d, %d, %d, %d, %d);\
         this.%s.swfTxtField.type = \"input\";\
         this.%s.swfTxtField.text = \" \";\
         this.%s.swfTxtField.border = true;\
         this.%s.swfTxtField.selectable = true;\
         var myformat = new TextFormat();\
         myformat.font = \"Arial\";\
         myformat.size = %d;\
         this.%s.swfTxtField.setNewTextFormat(myformat);\
         this.%s.swfTxtField.text = \"\";\
         this.%s.nextFrame();\
      }\
   "), csId, m_iMaxInteractionIndex, csId, 1, nX, nY, nWidth, nHeight, 
   csId, csId, csId, csId, nFontSize, csId, csId, csId);
   PrintActionScript("sprPages", script);
   sprPages->add(new SWFAction(GetCharFromCString(script)));
//#ifdef _DEBUG
   _tprintf(_T("+ TextField                 Index: %d\n"), m_iMaxInteractionIndex);
//#endif
*/

   return hr;
}

UINT CFlashEngine::CreateInteractiveText(SWFSprite *sprPages, DrawSdk::Text *pTextObject, CString csType, 
                                         float x, float y, float width, float height, int idx, int nStartMs)
{
   HRESULT hr = S_OK;

   CString csId;
   SWFDisplayItem *di = NULL;
   SWFSprite *sprTextField = new SWFSprite();

   hr = MapFlashFontFromTextObject(pTextObject);
   
   CString csTtfIn = pTextObject->GetTtfName();
   if (csTtfIn.IsEmpty())
   {
      // if no ttf is specified use the face name for the hash key
      LOGFONT lf = pTextObject->GetLogFont();
      csTtfIn = lf.lfFaceName;
   }
   SWFFont *pSwfFont = GetFlashFontFromTextObject(pTextObject);

   if (pSwfFont)
   {
      Gdiplus::ARGB ocref = pTextObject->GetOutlineColor();
      float fFontSize = -1.0f * pTextObject->GetLogFont().lfHeight;
      CString csText = pTextObject->GetString();

      // Special case: TEXT_TYPE_CHANGE_NUMBER, e.g. 'Question 2/4'
      // '2/4' has to be generated here
      bool bIsChangeNumberType = false;  // If is random test, consider TEXT_TYPE_CHANGE_NUMBER exactly as a "Change" type. 
                                         // TODO CF: Analyze how this exception should be removed.
      if (pTextObject->HasChangeableType() && pTextObject->GetTextType() == DrawSdk::TEXT_TYPE_CHANGE_NUMBER)
      {
         CQuestionnaire *pQuestionnaire = GetQuestionnaireFromStartMs(nStartMs);
         if (pQuestionnaire != NULL)
         {
            int qIdx = 0;
            int nQuestions = pQuestionnaire->GetQuestionCount();
            for (int k = 0; k < nQuestions; ++k)
            {
               CQuestion *pQ = pQuestionnaire->GetQuestions().ElementAt(k);
               CPoint activityTime;
               HRESULT hr2 = pQ->GetActivityTimes(&activityTime);
               if ((SUCCEEDED(hr2)) && (nStartMs >= activityTime.x) && (nStartMs <= activityTime.y))
               {
                  qIdx = k;
                  break;
               }
            }
            csText.Format(_T("%d/%d"), (qIdx+1), nQuestions);
            if (pQuestionnaire->IsRandomTest())
                bIsChangeNumberType = true;
         }
      }

////      _tprintf(_T("csTtfIn: 0x%08x %s %d/%d --> %s\n"), pSwfFont, csTtfIn, m_mapSwfFonts.GetCount(), m_mapFontNames.GetCount(), csText);
      if (csType.Find(_T("Change")) == 0 || bIsChangeNumberType)
      {
         SWFTextField *txtField = new SWFTextField();
         txtField->setFont(pSwfFont);
         txtField->setBounds(width, height);
         txtField->setHeight(fFontSize); // -1.0f: empirically found
         txtField->setIndentation(1.0f);
         txtField->setColor((ocref >> 16) & 0xff, (ocref >> 8) & 0xff, (ocref >> 0) & 0xff, (ocref >> 24) & 0xff);
         txtField->setVariableName("text");
         txtField->setAlignment(SWFTEXTFIELD_ALIGN_LEFT);
         txtField->setFlags(SWFTEXTFIELD_USEFONT | SWFTEXTFIELD_NOEDIT | SWFTEXTFIELD_NOSELECT | SWFTEXTFIELD_HASLAYOUT);
         csText = _T("0123456789:/");
         CString csInitialText = _T(" ");
         char *utfText = LString::TCharToUtf8(csText);
         char *utfInitialText = LString::TCharToUtf8(csInitialText);
         if (!bIsChangeNumberType && utfText)
         {
            txtField->addUTF8Chars(utfText);
            txtField->addUTF8String(utfInitialText);
            delete[] utfText;
            utfText = NULL;
            delete[] utfInitialText;
            utfInitialText = NULL;
         }


         di = sprTextField->add(txtField);
         csId.Format(_T("swfTxt%s"), csType);
         di->setName(GetCharFromCString(csId));
         sprTextField->nextFrame();
         di = sprPages->add(sprTextField);
         csId.Format(_T("swfSpr%sText_%d"), csType, idx);
         di->setName(GetCharFromCString(csId));
         di->moveTo((x+0.0f), (y-height+5.5f)); // empirically found
      }
      else
      {
         SWFText *txt = new SWFText();
         DrawText(txt, 0.0f, 0.0f, pSwfFont, csText, fFontSize, ocref);

         di = sprTextField->add(txt);
         csId.Format(_T("swfTxt%s"), csType);
         di->setName(GetCharFromCString(csId));
         sprTextField->nextFrame();
         di = sprPages->add(sprTextField);
         csId.Format(_T("swfSpr%sText_%d"), csType, idx);
         di->setName(GetCharFromCString(csId));
         di->moveTo(x+0.0f, y+1.0f); // empirically found
      }

      m_iMaxInteractionIndex++;
//#ifdef _DEBUG
      _tprintf(_T("+ Interactive Text: %d %s\n"), pTextObject->GetTextType(), csText);
      _tprintf(_T("+ Interactive Text          Index: %d\n"), m_iMaxInteractionIndex);
//#endif
      di->setDepth(m_iMaxInteractionIndex);
   }

   return hr;
}

UINT CFlashEngine::CreateInteractionMaskingLayer(SWFSprite *sprPages)
{
   HRESULT hr = S_OK;

   // BUGFIX BLECPUB-1864: 
   // Add a dark grey rectangle as a "protection layer" on top of all interactive elements 
   // to make the interactive elements invisible during the loading time. 

   // a) Create a CInteractionArea object from a SGML string
   CInteractionArea *pInteractionArea = NULL;
   pInteractionArea = new CInteractionArea();

   CString csRecordPath;
   csRecordPath.Format(_T("%s"), m_csInputPath);
   CString csWidth;
   csWidth.Format(_T("%d"), m_iWbOutputWidth);
   CString csHeight;
   csHeight.Format(_T("%d"), m_iWbOutputHeight);
   CString csSgml;
   csSgml.Format(_T("<BUTTON x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" button-type=\"other\" \
demoDocumentObject=\"false\" onClick=\"\">\n\
<normal>\n\
<FILLEDRECT x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" rgb=\"0x515967\"></FILLEDRECT>\n\
</normal>\n\
<over>\n\
<FILLEDRECT x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" rgb=\"0x515967\"></FILLEDRECT>\n\
</over>\n\
<pressed>\n\
<FILLEDRECT x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" rgb=\"0x515967\"></FILLEDRECT>\n\
</pressed>\n\
<inactive>\n\
<FILLEDRECT x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" rgb=\"0x515967\"></FILLEDRECT>\n\
</inactive>\n\
</BUTTON>\n"), m_iWbOutputWidth, m_iWbOutputHeight, 
               m_iWbOutputWidth, m_iWbOutputHeight, 
               m_iWbOutputWidth, m_iWbOutputHeight, 
               m_iWbOutputWidth, m_iWbOutputHeight, 
               m_iWbOutputWidth, m_iWbOutputHeight);
   DWORD dwBufferSize(csSgml.GetLength());
   PBYTE aSgmlData = new BYTE[dwBufferSize + 1];
   USES_CONVERSION; // Required for 'T2CA' in the next line
   memcpy(aSgmlData, T2CA(csSgml), dwBufferSize);
   aSgmlData[dwBufferSize] = (BYTE)'\0';

   SgmlFile sgmlFile;
   sgmlFile.Init();
   sgmlFile.ReadMemory(aSgmlData, dwBufferSize, _T("<BUTTON"));
   
   SgmlElement *pSgmlRoot = sgmlFile.GetRoot();
   if (!pSgmlRoot)
      return E_WB_INVALID_DATA;


   pInteractionArea->ReadFromSgml(pSgmlRoot, csRecordPath);

   m_aInteractionAreas.Add(pInteractionArea);


   // b) Create a Rectangle shape as an Inreraction Area
   int nNumOfAreas = m_aInteractionAreas.GetSize() - 1;
   int idx = nNumOfAreas; 
   float x = 0.0f;
   float y = 0.0f;
   float width = (float)m_iWbOutputWidth;
   float height = (float)m_iWbOutputHeight;

   DrawSdk::Rectangle *pProtectionRect = NULL;
   pProtectionRect = new DrawSdk::Rectangle();
   Gdiplus::ARGB clrBgr = 0xFF515967; // Background color of the surrounding FLEX Player
   pProtectionRect->SetWidth((double)m_iWbOutputWidth);
   pProtectionRect->SetHeight((double)m_iWbOutputHeight);
   pProtectionRect->SetLineColor(clrBgr);
   pProtectionRect->SetLineWidth(1.0);
   pProtectionRect->SetLineStyle(0); // solid
   pProtectionRect->SetFillColor(clrBgr);
   
   SWFShape *shpGraph = new SWFShape();
   CreateShapeFromButtonGraphicalObject(shpGraph, pProtectionRect);

   CreateInteractionArea(sprPages, x, y, width, height, idx, pInteractionArea, 
       NULL, NULL, NULL, NULL, NULL); 

   return hr;
}

CString CFlashEngine::ReinterpretActionStringForFlash(CString csAction, int nPage)
{
   CString csNewFlashAction = _T("");

   if (csAction.Find(_T("start")) == 0)
   {
      csNewFlashAction = _T("lecOnPlay()");
   }
   if (csAction.Find(_T("stop")) == 0)
   {
      csNewFlashAction = _T("lecOnPause()");
   }
   if (csAction.Find(_T("close")) == 0)
   {
      csNewFlashAction = _T("lecOnClose()");
   }
   if (csAction.Find(_T("jump")) == 0)
   {
      if (csAction.Find(_T("jump next")) == 0)
      {
         CString csArgument = (csAction.GetLength() >= 9) ? csAction.Mid(9) : _T("");
         CString cs2ndCommand = Get2ndCommandFromActionArgument(csArgument, false);
         csNewFlashAction.Format(_T("lecOnForward(%s)"), cs2ndCommand);
      }
      else if (csAction.Find(_T("jump prev")) == 0)
      {
         CString csArgument = (csAction.GetLength() >= 9) ? csAction.Mid(9) : _T("");
         CString cs2ndCommand = Get2ndCommandFromActionArgument(csArgument, false);
         csNewFlashAction.Format(_T("lecOnBack(%s)"), cs2ndCommand);
      }
      else if (csAction.Find(_T("jump page")) == 0)
      {
         CString csArgument = (csAction.GetLength() >= 9) ? csAction.Mid(9) : _T("");
         CString cs2ndCommand = Get2ndCommandFromActionArgument(csArgument, true);
         int pageNumber = GetIntegerFromActionArgument(csArgument);
         csNewFlashAction.Format(_T("lecOnGotoSlide(%d%s)"), pageNumber, cs2ndCommand);
         // pageNumber is a timestamp
         int targetFrame = GetFrameFromTimestamp(pageNumber);
         csNewFlashAction.Format(_T("lecOnGotoFrame(%d%s)"), targetFrame, cs2ndCommand);
      }
      else if (csAction.Find(_T("jump mark")) == 0)
      {
         CString csArgument = (csAction.GetLength() >= 9) ? csAction.Mid(9) : _T("");
         CString cs2ndCommand = Get2ndCommandFromActionArgument(csArgument, true);
         int markNumber = GetIntegerFromActionArgument(csArgument);
         //csNewFlashAction.Format(_T("lecOnGotoTargetmark(%d%s)"), markNumber, cs2ndCommand); 
         // markNumber is a timestamp
         int targetFrame = GetFrameFromTimestamp(markNumber);
         csNewFlashAction.Format(_T("lecOnGotoFrame(%d%s)"), targetFrame, cs2ndCommand);
      }
      else if (csAction.Find(_T("jump question")) == 0)
      {
         CString csArgument = (csAction.GetLength() >= 13) ? csAction.Mid(13) : _T("");
         CString cs2ndCommand = Get2ndCommandFromActionArgument(csArgument, true);
         csNewFlashAction.Format(_T("lecOnGotoQuestion(%s)"), cs2ndCommand);
      }
      else
      {
         CString csArgument = (csAction.GetLength() >= 4) ? csAction.Mid(4) : _T("");
         CString cs2ndCommand = Get2ndCommandFromActionArgument(csArgument, true);
         int timeMs = GetIntegerFromActionArgument(csArgument);
         int targetFrame = GetFrameFromTimestamp(timeMs);
         csNewFlashAction.Format(_T("lecOnGotoframe(%d%s)"), targetFrame, cs2ndCommand);
      }
   }
   if (csAction.Find(_T("audio on")) == 0)
   {
      CString csArgument = (csAction.GetLength() >= 8) ? csAction.Mid(8) : _T("");
      CString cs2ndCommand = Get2ndCommandFromActionArgument(csArgument, false);
      csNewFlashAction.Format(_T("lecOnMuteOff(%s)"), cs2ndCommand);
   }
   if (csAction.Find(_T("audio off")) == 0)
   {
      CString csArgument = (csAction.GetLength() >= 9) ? csAction.Mid(9) : _T("");
      CString cs2ndCommand = Get2ndCommandFromActionArgument(csArgument, false);
      csNewFlashAction.Format(_T("lecOnMuteOn(%s)"), cs2ndCommand);
   }
   if (csAction.Find(_T("open-url")) == 0)
   {
      CString csArgument = (csAction.GetLength() >= 8) ? csAction.Mid(9) : _T("");
      CString cs2ndCommand = Get2ndCommandFromActionArgument(csArgument, true);
      int nPosSeparator = csArgument.Find(_T(';'));
      if (nPosSeparator > -1)
         csArgument = csArgument.Mid(0, nPosSeparator);
      csArgument = GetSlashSubstitutedString(csArgument);

      // Copy file to target directory (if possible)
      CString csSource = m_csInputPath + csArgument;
      CString csTarget;
      csTarget.Format(_T("%s"), m_tszSwfFile);
      StringManipulation::GetPath(csTarget);
      csTarget += _T("\\");
      csTarget += csArgument;

      BOOL bCanBeCopied = ::CopyFile(csSource, csTarget, FALSE);
      if (bCanBeCopied)
         _tprintf(_T("%s copied."), csArgument);

      csNewFlashAction.Format(_T("lecOnOpenUrl(\"%s\"%s)"), csArgument, cs2ndCommand);
   }
   if (csAction.Find(_T("open-file")) == 0)
   {
      // Bugfix 4898: New Flash player: Add "content/" as path to the file
      CString csPath = _T("");
      if (m_bIsNewLecPlayer)
         csPath.Format(_T("%s/"), STR_CONTENT);;

      CString csArgument = (csAction.GetLength() >= 9) ? csAction.Mid(10) : _T("");
      CString cs2ndCommand = Get2ndCommandFromActionArgument(csArgument, true);
      int nPosSeparator = csArgument.Find(_T(';'));
      if (nPosSeparator > -1)
         csArgument = csArgument.Mid(0, nPosSeparator);
      csArgument = GetSlashSubstitutedString(csArgument);

      // Copy file to target directory (if possible)
      CString csSource = m_csInputPath + csArgument;
      CString csTarget;
      csTarget.Format(_T("%s"), m_tszSwfFile);
      StringManipulation::GetPath(csTarget);
      csTarget += _T("\\");
      csTarget += csArgument;

      BOOL bCanBeCopied = ::CopyFile(csSource, csTarget, FALSE);
      if (bCanBeCopied)
      {
         _tprintf(_T("%s copied.\n"), csArgument);
         csNewFlashAction.Format(_T("lecOnOpenFile(\"%s%s\"%s)"), csPath, csArgument, cs2ndCommand);
         // Remember SCORM
         if (m_bScormEnabled)
         {
            // Add this file name to the corresponding list
            m_mapInteractiveOpenedFileNames.SetAt(csArgument, csTarget);
            // Delete this file after SCORM package is created --> temporary file
            m_mapTempFiles.SetAt(csTarget, NULL);
         }
      }
      else
      {
         // File can not be copied.
         // Assuming the file is specified with absolute path: "file://" has to be added
         csNewFlashAction.Format(_T("lecOnOpenFile(\"file://%s%s\"%s)"), csPath, csArgument, cs2ndCommand);
      }
   }
   if (csAction.Find(_T("check")) == 0)
   {
      csNewFlashAction.Format(_T("onDoSubmitAnswer(%d)"), nPage);
   }
   if ( (csAction.Find(_T("reset")) == 0) && (csAction.Find(_T("reset-all")) != 0) )
   {
      csNewFlashAction.Format(_T("onDoResetQuestion(%d)"), nPage);
   }
   if (csAction.Find(_T("finish")) == 0)
   {
      csNewFlashAction.Format(_T("onDoSubmitTesting(%d)"), nPage);
   }
   if (csAction.Find(_T("reset-all")) == 0)
   {
      csNewFlashAction.Format(_T("onDoResetQuestionnaire(%d)"), nPage);
   }

   // Should not happen: unknown command --> do nothing
   if (csNewFlashAction.GetLength() == 0)
      csNewFlashAction = _T("lecOnDoNothing()");

   _tprintf(_T("Action: %s --> %s\n"), csAction, csNewFlashAction);

   return csNewFlashAction;
}

CString CFlashEngine::Get2ndCommandFromActionArgument(CString csArgument, bool bAddSeparator)
{
   CString cs2ndCommand = _T("");

   int nPosSeparator = csArgument.Find(_T(';'));
   if (nPosSeparator > -1)
   {
      CString csFlag = csArgument.Mid(nPosSeparator+1);
      if (csFlag.Find(_T("start")) == 0)
         cs2ndCommand.Format(_T("\"play\""));
      if (csFlag.Find(_T("stop")) == 0)
         cs2ndCommand.Format(_T("\"pause\""));

      if (bAddSeparator)
         cs2ndCommand.Insert(0, _T(", "));
   }

   return cs2ndCommand;
}

CString CFlashEngine::GetSlashSubstitutedString(CString csArgument)
{
   CString csSubstituted;
   csSubstituted.Empty();

   // this seems not to work here - csArgument does not contain any single backslash
   for (int i = 0; i < csArgument.GetLength(); ++i)
   {
      _TCHAR c = csArgument.GetAt(i);
      if (c == _T('\\'))
      {
         if (((i+1) < csArgument.GetLength()) && (csArgument.GetAt(i+1) != _T('\\')))
            csSubstituted += _T('/');
      }
      else
         csSubstituted += c;
   }

   return csSubstituted;
}

int CFlashEngine::GetIntegerFromActionArgument(CString csArgument)
{
   CString csValue = _T("");
   bool bDigitFound = false;
   int i = 0;

   while (i < csArgument.GetLength())
   {
      if ((csArgument.GetAt(i) > 47) && (csArgument.GetAt(i) < 58))
      {
         csValue += csArgument.GetAt(i);
         bDigitFound = true;
      }
      else
      {
         // only one Integer per String can be found --> stop after the 1st one.
         if (bDigitFound)
            break;
      }

      i++;
   }

   int nValue = csValue.GetLength() > 0 ? _ttoi(csValue) : 0;
   return nValue;
}

int CFlashEngine::GetPageFromInteractionArea(CInteractionArea *pInteractionArea)
{
   int nPage = -1;

   CArray <CPoint, CPoint> aTimePeriodsActivity;
   aTimePeriodsActivity.RemoveAll();
   pInteractionArea->GetActivityTimes(&aTimePeriodsActivity);
   // So far only one activity period is defined
   CPoint ptTimePeriodActivity(-1, -1);
   if (aTimePeriodsActivity.GetSize() > 0)
      ptTimePeriodActivity = aTimePeriodsActivity.ElementAt(0);

   int nActivityBeginMs = ptTimePeriodActivity.x;
   int nActivityEndMs = ptTimePeriodActivity.y;

   // Bugfix 3176: The next code lines may lead to the wrong page, 
   // if the LMD data differs from the EVQ data.
//   CPage *pPage = m_pPageStream->FindPageAtTimestamp(nActivityBeginMs);
//   if (pPage != NULL)
//      nPage = pPage->GetPageNumber();
//   else
//   {
//      // timestamp before first page?
//      pPage = m_pPageStream->GetFirstPage(ptTimePeriodActivity.x, ptTimePeriodActivity.y, 0);
//      if (pPage != NULL)
//         nPage = pPage->GetPageNumber();
//   }

   int nFrameBegin = GetFrameFromTimestamp(nActivityBeginMs);
   int nFrameEnd = GetFrameFromTimestamp(nActivityEndMs);

   nPage = GetPageFromActivityFrames(nFrameBegin, nFrameEnd);

   return nPage;
}

int CFlashEngine::GetPageFromActivityFrames(int iFrameBegin, int iFrameEnd)
{
   int nPage = -1;
   int i = 0;
   for (i = 0; i < m_aiPagingFrames.GetSize() - 1; ++i)
   {
      if( (iFrameBegin >= m_aiPagingFrames.ElementAt(i)) && (iFrameEnd < m_aiPagingFrames.ElementAt(i+1)) )
      {
         nPage = i;
         break;
      }
   }
   if (iFrameBegin < m_aiPagingFrames.ElementAt(0))
      nPage = 0;
   i = m_aiPagingFrames.GetSize() - 1;
   if (iFrameBegin >= m_aiPagingFrames.ElementAt(i))
      nPage = i;

   // 1st page is 1 (not 0)
   if (nPage < 0)
      return nPage;
   else
      return (nPage + 1);
}

bool CFlashEngine::AreInteractionsInTimeInterval(int nBeginMs, int nEndMs) {
    bool bInteracionsFound = false;
    int i = 0;

    // Check for interaction objects
    int nNumOfInteractions = m_aInteractionObjects.GetCount();
    for (i = 0; i < nNumOfInteractions; ++i) {
        DrawSdk::DrawObject *pDrawObject = m_aInteractionObjects.ElementAt(i);
        CString csVisibility = pDrawObject->GetVisibility();
        if (csVisibility.GetLength() > 0) {
            CArray<CPoint, CPoint> aVisibilityTimes;
            StringManipulation::ExtractAllTimePeriodsFromString(csVisibility, &aVisibilityTimes);
            int iTimeMsVisibleBegin  = aVisibilityTimes.ElementAt(0).x;
            int iTimeMsVisibleEnd    = aVisibilityTimes.ElementAt(0).y;
            if ( (iTimeMsVisibleBegin >= nBeginMs) 
              && (iTimeMsVisibleBegin <= nEndMs) 
              && (iTimeMsVisibleEnd >= nBeginMs) 
              && (iTimeMsVisibleEnd <= nEndMs) ) {
                // Interaction object found
                bInteracionsFound = true;
            }
        }
    }

    // Check for interaction areas
    int nNumOfInteractionAreas = m_aInteractionAreas.GetSize();
    for (i = 0; i < nNumOfInteractionAreas; ++i) {
        CArray<CPoint, CPoint> aTimePeriodsActivity;
        aTimePeriodsActivity.RemoveAll();
        m_aInteractionAreas.ElementAt(i)->GetActivityTimes(&aTimePeriodsActivity);
        CPoint ptTimePeriodActivity(-1, -1);
        if (aTimePeriodsActivity.GetSize() > 0) {
            ptTimePeriodActivity = aTimePeriodsActivity.ElementAt(0);
            int iTimeMsVisibleBegin  = ptTimePeriodActivity.x;
            int iTimeMsVisibleEnd    = ptTimePeriodActivity.y; 
            if ( (iTimeMsVisibleBegin >= nBeginMs) 
              && (iTimeMsVisibleBegin <= nEndMs) 
              && (iTimeMsVisibleEnd >= nBeginMs) 
              && (iTimeMsVisibleEnd <= nEndMs) ) {
                // Interaction object found
                bInteracionsFound = true;
            }
        }
    }
        
    // Check for stop or target marks
    int nStopmarks = m_aFlashStopmarks.GetCount();
    int nTargetmarks = m_aFlashTargetmarks.GetCount();
    if (nStopmarks > 0) {
        for (i = 0; i < nStopmarks; ++i) {
            if ( (m_aFlashStopmarks.ElementAt(i).nMsTime >= nBeginMs)
              && (m_aFlashStopmarks.ElementAt(i).nMsTime <= nEndMs) ) {
                // Stop mark found
                bInteracionsFound = true;
            }
        }
    }
    if (nTargetmarks > 0) {
        for (i = 0; i < nTargetmarks; ++i) {
            if ( (m_aFlashTargetmarks.ElementAt(i).nMsTime >= nBeginMs)
              && (m_aFlashTargetmarks.ElementAt(i).nMsTime <= nEndMs) ) {
                // Target mark found
                bInteracionsFound = true;
            }
        }
    }

    return bInteracionsFound;
}

// --------------------------------------------------------------------------
// Buttons for the control bar
// --------------------------------------------------------------------------

/**
 * --------------------------------------------------------------------------
 * A common method to create a 3D style button
 * --------------------------------------------------------------------------
 * Parameters:
 *    pParentSprite     - a pointer to the parent SWFSprite object
 *    x                 - x coordinate (as float)
 *    y                 - y coordinate (as float)
 *    width             - width (as float)
 *    height            - height (as float)
 *    buttonShapeFlag   - D3D_RECT_BUTTON / D3D_OVAL_BUTTON / D3D_SLIDER_BUTTON
 *    csButtonName      - CString for a unique Button Name (or ID)
 *    csButtonTitle     - CString for the Button Title (Tooltip)
 *    shpBtnUp          - a pointer to the "Up" SWFShape object
 *    shpBtnOver        - a pointer to the "Over" SWFShape object
 *    shpBtnDown        - a pointer to the "Down" SWFShape object
 *    shpBtnDisabled    - a pointer to the "Disabled" SWFShape object
 *    scriptPressed     - String with the ActionScript to be executed when button is pressed - can be NULL
 *    scriptReleased    - String with the ActionScript to be executed when button is released - can be NULL
 *    scriptEntered     - String with the ActionScript to be executed when the frame with the button is entered - can be NULL
 */

void CFlashEngine::DrawCommon3dButton(SWFSprite *pParentSprite, float x, float y, float width, float height,
                                      int buttonShapeFlag, CString csButtonName, CString csButtonTitle, 
                                      SWFShape *shpBtnUp, SWFShape *shpBtnOver, SWFShape *shpBtnDown, SWFShape *shpBtnDisabled, 
                                      _TCHAR *scriptPressed, _TCHAR *scriptReleased, _TCHAR *scriptEntered)
{
   if (shpBtnDisabled != NULL)
   {
      // Draw a "disabled" button
      DrawDisabledButton(pParentSprite, x, y, width, height, buttonShapeFlag, csButtonName, shpBtnDisabled, scriptEntered);
   }

   // Button must be independent from the movie content --> use a new SWFSprite here
   SWFSprite *sprCommon = new SWFSprite();

   SWFDisplayItem *di;

   // Determine diameter (2*radius) from the minimum of width/height
   float diameter = (width < height) ? width : height;

   if ((buttonShapeFlag != D3D_SLIDER_BUTTON) && (!m_bIsPspDesign))
   {
      // Draw a dummy inverted button which is 6 px greater and lighter than the button itself ==> deepening
      SWFShape *shpBtnDummy1 = new SWFShape();
      Draw3DRoundedRectangle(shpBtnDummy1, -2.0f, -2.0f, (width+4.0f), (height+4.0f), (0.5f*(diameter+4.0f)), 1, D3D_PEN_PALETTE, 
         D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_DEEPENED, D3D_RADIAL_GRADIENT, m_swfRadialGradientLighter);
      di = sprCommon->add(shpBtnDummy1);
   }

   // Draw a dummy button which is 2 px greater and darker than the button itself ==> border line
   SWFShape *shpBtnDummy2 = new SWFShape();
   if (m_bIsPspDesign)
      Draw3DRoundedRectangle(shpBtnDummy2, 0.0f, 0.0f, width, height, (0.0f), 1, D3D_PEN_PALETTE, 
         D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_RAISED, D3D_RADIAL_GRADIENT, m_swfRadialGradientDarker);
   else
      Draw3DRoundedRectangle(shpBtnDummy2, 0.0f, 0.0f, width, height, (0.5f*diameter), 1, D3D_PEN_PALETTE, 
         D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_RAISED, D3D_RADIAL_GRADIENT, m_swfRadialGradientDarker);
   di = sprCommon->add(shpBtnDummy2);

   // Draw the button (without any symbol)
   SWFShape *shpBtnCommon = new SWFShape();
   if (m_bIsPspDesign)
      Draw3DRoundedRectangle(shpBtnCommon, (1.00f), (1.00f), (width-2.0f), (height-2.0f), (0.0f), 1, D3D_PEN_PALETTE, 
         D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_RAISED, D3D_RADIAL_GRADIENT, m_swfRadialGradient);
   else
      Draw3DRoundedRectangle(shpBtnCommon, (1.00f), (1.00f), (width-2.0f), (height-2.0f), (0.5f*(diameter-2.0f)), 1, D3D_PEN_PALETTE, 
         D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_RAISED, D3D_RADIAL_GRADIENT, m_swfRadialGradient);
   di = sprCommon->add(shpBtnCommon);


   // Create a shape for "Button hit"
   SWFShape *shpBtnHit = new SWFShape();

   // The button itself
   SWFButton *btnCommon = new SWFButton();

   if (buttonShapeFlag == D3D_SLIDER_BUTTON)
   {
//      Draw3DRoundedRectangle(shpBtnHit, 0.0f, 0.0f, width, height, 2.0f, 1, D3D_PEN_PALETTE_DARK, 
//         D3D_BORDER, D3D_FILL, D3D_RAISED, D3D_LINEAR_GRADIENT, m_swfLinearGradientMouseUp);
      shpBtnHit  = shpBtnDummy2;
      shpBtnUp   = shpBtnCommon;

      if (m_bIsPspDesign)
         Draw3DRoundedRectangle(shpBtnOver, (1.00f), (1.00f), (width-2.0f), (height-2.0f), (0.0f), 1, D3D_PEN_PALETTE, 
            D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_RAISED, D3D_RADIAL_GRADIENT, m_swfRadialGradientSlider);
      else
         Draw3DRoundedRectangle(shpBtnOver, (1.00f), (1.00f), (width-2.0f), (height-2.0f), (0.5f*(diameter-2.0f)), 1, D3D_PEN_PALETTE, 
            D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_RAISED, D3D_RADIAL_GRADIENT, m_swfRadialGradientSlider);

      shpBtnDown = shpBtnOver;
   }
   else if (buttonShapeFlag == D3D_OVAL_BUTTON)
   {
      DrawOvalButtonShape(shpBtnHit, width, height);
   }
   else if (buttonShapeFlag == D3D_RECT_BUTTON)
   {
      DrawRectButtonShape(shpBtnHit, width, height);
   }

   btnCommon->addShape(shpBtnHit, SWFBUTTON_HIT);
   btnCommon->addShape(shpBtnUp, SWFBUTTON_UP);
   btnCommon->addShape(shpBtnOver, SWFBUTTON_OVER);
   btnCommon->addShape(shpBtnDown, SWFBUTTON_DOWN);

   // Add ActionScript for button pressed
   if (scriptPressed != NULL)
   {
      PrintActionScript(csButtonName, scriptPressed);
      if (buttonShapeFlag == D3D_SLIDER_BUTTON)
         btnCommon->addAction(new SWFAction(GetCharFromCString(scriptPressed)), SWFBUTTON_MOUSEDOWN);
      else
         btnCommon->addAction(new SWFAction(GetCharFromCString(scriptPressed)), SWFBUTTON_OVERDOWNTOOVERUP);
   }

   // Add ActionScript for button released
   if (scriptReleased != NULL)
   {
      PrintActionScript(csButtonName, scriptReleased);
      btnCommon->addAction(new SWFAction(GetCharFromCString(scriptReleased)), SWFBUTTON_MOUSEUP | SWFBUTTON_MOUSEUPOUTSIDE);
   }

   // Tooltip ("umlauts" are only supported with Flash version > 5)
   if ((m_iFlashVersion > 5) && (!csButtonTitle.IsEmpty()))
   {
      if (x > (m_rcMovie.Width() - 10*csButtonTitle.GetLength()))
         CreateTooltip(btnCommon, csButtonTitle, _T("topleft"));
      else
         CreateTooltip(btnCommon, csButtonTitle, _T("topright"));
   }


   // Add the button to the new SWFSprite
   di = sprCommon->add(btnCommon);
   CString csId;
   csId.Format(_T("swfBtn%s"), csButtonName);
   di->setName(GetCharFromCString(csId));
   // For Flash version 6 and higher: useHandCursor works this way
   CString csUseHand = _T("false");
   _TCHAR script[1024];
   _stprintf(script, _T("\
      %s.useHandCursor = %s;\
   "), csId, csUseHand);
   PrintActionScript(_T("Common 3D button"), script);
   sprCommon->add(new SWFAction(GetCharFromCString(script)));

   sprCommon->nextFrame();

   // Add the SWFSprite to the parent
   di = pParentSprite->add(sprCommon);
   csId.Format(_T("swfSpr%s"), csButtonName);
   di->setName(GetCharFromCString(csId));

   di->moveTo(x, y);
   
   // Add ActionScript for button frame entered
   if (scriptEntered != NULL)
   {
      PrintActionScript(csButtonName, scriptEntered);
      di->addAction(new SWFAction(GetCharFromCString(scriptEntered)), SWFACTION_ENTERFRAME);
   }
}

void CFlashEngine::DrawDisabledButton(SWFSprite *pSprite, float x, float y, float width, float height, 
                                      int buttonShapeFlag, CString csButtonName, SWFShape *shpBtnDisabled, 
                                      _TCHAR *scriptEntered)
{
   SWFSprite *sprCommon = new SWFSprite();

   SWFDisplayItem *di;

   // Determine diameter (2*radius) from the minimum of width/height
   float diameter = (width < height) ? width : height;

   if ((buttonShapeFlag != D3D_SLIDER_BUTTON) && (!m_bIsPspDesign))
   {
      // Draw a dummy inverted button which is 6 px greater and lighter than the button itself ==> deepening
      SWFShape *shpBtnDummy1 = new SWFShape();
      Draw3DRoundedRectangle(shpBtnDummy1, -2.0f, -2.0f, (width+4.0f), (height+4.0f), (0.5f*(diameter+4.0f)), 1, D3D_PEN_PALETTE, 
         D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_DEEPENED, D3D_RADIAL_GRADIENT, m_swfRadialGradientLighter);
      di = sprCommon->add(shpBtnDummy1);
   }

   // Draw a dummy button which is 2 px greater and darker than the button itself ==> border line
   SWFShape *shpBtnDummy2 = new SWFShape();
   if (m_bIsPspDesign)
      Draw3DRoundedRectangle(shpBtnDummy2, 0.0f, 0.0f, width, height, (0.0f), 1, D3D_PEN_PALETTE, 
         D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_RAISED, D3D_RADIAL_GRADIENT, m_swfRadialGradientDarker);
   else
      Draw3DRoundedRectangle(shpBtnDummy2, 0.0f, 0.0f, width, height, (0.5f*diameter), 1, D3D_PEN_PALETTE, 
         D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_RAISED, D3D_RADIAL_GRADIENT, m_swfRadialGradientDarker);
   di = sprCommon->add(shpBtnDummy2);

   // Draw the button (without any symbol)
   SWFShape *shpBtnCommon = new SWFShape();
   if (m_bIsPspDesign)
      Draw3DRoundedRectangle(shpBtnCommon, (1.00f), (1.00f), (width-2.0f), (height-2.0f), (0.0f), 1, D3D_PEN_PALETTE, 
         D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_RAISED, D3D_RADIAL_GRADIENT, m_swfRadialGradient);
   else
      Draw3DRoundedRectangle(shpBtnCommon, (1.00f), (1.00f), (width-2.0f), (height-2.0f), (0.5f*(diameter-2.0f)), 1, D3D_PEN_PALETTE, 
         D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_RAISED, D3D_RADIAL_GRADIENT, m_swfRadialGradient);
   di = sprCommon->add(shpBtnCommon);

   // Draw the disabled symbol shape
   di = sprCommon->add(shpBtnDisabled);

   sprCommon->nextFrame();

   // Add the SWFSprite to the parent
   di = pSprite->add(sprCommon);
   CString csId;
   csId.Format(_T("swfSpr%s_Disabled"), csButtonName);
   di->setName(GetCharFromCString(csId));

   di->moveTo(x, y);

   // Add ActionScript for button frame entered
   if (scriptEntered != NULL)
   {
      PrintActionScript(csButtonName, scriptEntered);
      di->addAction(new SWFAction(GetCharFromCString(scriptEntered)), SWFACTION_ENTERFRAME);
   }
}

// --------------------------------------------------------------------------
  UINT CFlashEngine::CreateSlidestarPlayButton(SWFSprite *pSprite, float x, float y)
{
	HRESULT hr = S_OK;

	CString csTitle;
	csTitle.LoadString(IDS_START_REPLAY_PAUSE);

	// Add ActionScript
	_TCHAR scriptPressed[1024];
	_stprintf(scriptPressed, _T("\
								var objRoot = _parent._parent;\
								if (!objRoot.g_bControlIsDisabled)\
								{\
                           /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
                           objRoot.g_bIgnoreStopmark = true;\
                           objRoot.g_bIsInteractivePaused = false;\
									objRoot.lecDoGotoAndPlay(objRoot._currentframe);\
								}\
								"));

	CreateSlidestarCommonButtons(pSprite, x, y, _T("Play"), csTitle, 
								".\\Flash\\slidestar\\play_up.swf", ".\\Flash\\slidestar\\play_over.swf", scriptPressed, NULL, NULL);

	return hr;
}

UINT CFlashEngine::CreateSlidestarPauseButton(SWFSprite *pSprite, float x, float y)
{
	HRESULT hr = S_OK;
	
	_TCHAR scriptPressed[1024];
	CString csTitle;
	csTitle.LoadString(IDS_START_REPLAY_PAUSE);
	
	_stprintf(scriptPressed, _T("\
	   var objRoot = _parent._parent;\
	   if (!objRoot.g_bControlIsDisabled)\
	   {\
		   objRoot.lecDoGotoAndStop(objRoot._currentframe);\
	   }\
	"));
		

	CreateSlidestarCommonButtons(pSprite, x, y, _T("Pause"), csTitle, 
								".\\Flash\\slidestar\\pause_up.swf", ".\\Flash\\slidestar\\pause_over.swf", scriptPressed, NULL, NULL);

	return hr;
}

UINT CFlashEngine::CreateSlidestarGotoFirstFrameButton(SWFSprite *pSprite, float x, float y)
{
	HRESULT hr = S_OK;

	_TCHAR scriptPressed[1024];
	CString csTitle;
	csTitle.LoadString(IDS_JUMP_TO_BEGIN);
	_stprintf(scriptPressed, _T("\
								var objRoot = _parent._parent;\
								if (!objRoot.g_bControlIsDisabled)\
								{\
									if (objRoot.g_bIsPlaying || objRoot.g_bIsInteractivePaused)\
									{\
										objRoot.g_bIsInteractivePaused = false;\
										objRoot.lecDoGotoAndPlay(1);\
									}\
									else\
									{\
										objRoot.lecDoGotoAndStop(1);\
									}\
								}\
								"));

	CreateSlidestarCommonButtons(pSprite, x, y, _T("GotoFirstFrame"), csTitle, 
								  ".\\Flash\\slidestar\\first_slide_up.swf", ".\\Flash\\slidestar\\first_slide_over.swf",scriptPressed, NULL, NULL);
	
	return hr;
}

UINT CFlashEngine::CreateSlidestarGotoPrevPageButton(SWFSprite *pSprite, float x, float y)
{
	HRESULT hr = S_OK;
	
	_TCHAR scriptPressed[1024];
	CString csTitle;
	csTitle.LoadString(IDS_SLIDE_BACKWARDS);
	_stprintf(scriptPressed, _T("\
		var objRoot = _parent._parent;\
		if (!objRoot.g_bControlIsDisabled)\
		{\
			objRoot.lecGotoPrevPage();\
		}\
		objRoot.g_bIsInteractivePaused = false;\
		"));

	CreateSlidestarCommonButtons(pSprite, x, y, _T("GotoPrevPage"), csTitle, 
								  ".\\Flash\\slidestar\\previous_slide_up.swf", ".\\Flash\\slidestar\\previous_slide_over.swf", scriptPressed, NULL, NULL);

	return hr;
}

UINT CFlashEngine::CreateSlidestarGotoNextPageButton(SWFSprite *pSprite, float x, float y)
{
	HRESULT hr = S_OK;
	
	_TCHAR scriptPressed[1024];
	CString csTitle;
	csTitle.LoadString(IDS_SLIDE_FORWARD);
	_stprintf(scriptPressed, _T("\
		var objRoot = _parent._parent;\
		if (!objRoot.g_bControlIsDisabled)\
		{\
			objRoot.lecGotoNextPage();\
		}\
	   objRoot.g_bIsInteractivePaused = false;\
		"));

	CreateSlidestarCommonButtons(pSprite, x, y, _T("GotoNextPage"), csTitle, 
								  ".\\Flash\\slidestar\\next_slide_up.swf", ".\\Flash\\slidestar\\next_slide_over.swf", scriptPressed, NULL, NULL);
	return hr;

}

UINT CFlashEngine::CreateSlidestarGotoLastFrameButton(SWFSprite *pSprite, float x, float y)
{

	HRESULT hr = S_OK;
	
	_TCHAR scriptPressed[1024];
	CString csTitle;
	csTitle.LoadString(IDS_JUMP_TO_END);
	_stprintf(scriptPressed, _T("\
								var objRoot = _parent._parent;\
								if (!objRoot.g_bControlIsDisabled)\
								{\
									objRoot.lecDoGotoAndStop(objRoot._totalframes);\
								}\
								"));

	CreateSlidestarCommonButtons(pSprite, x, y, _T("GotoLastFrame"), csTitle, 
								  ".\\Flash\\slidestar\\last_slide_up.swf", ".\\Flash\\slidestar\\last_slide_over.swf", scriptPressed, NULL, NULL);
	return hr;


}

void CFlashEngine::CreateSlidestarCommonButtons(SWFSprite *pSprite, float x, float y, CString csButtonName, CString csButtonTitle, 
								  char* swfImageNormal, char* swfImageOver, _TCHAR *scriptPressed, _TCHAR *scriptReleased, _TCHAR *scriptEntered)
{
	
	// Insert swf file

	
	SWFDisplayItem *di;
	SWFSprite *sprCommon = new SWFSprite();
	//SWFPrebuiltClip *preclip = new SWFPrebuiltClip(swfImageNormal);
	

	SWFPrebuiltClip *preClipNormal = new SWFPrebuiltClip(swfImageNormal);
	SWFPrebuiltClip *preClipOver = new SWFPrebuiltClip(swfImageOver);

    float width, height;
	if(csButtonName != "VolumeSlider" && csButtonName != "TimeSlider")
		width = height = 27.0f;
	else
	{
		width = 14.0f;
		height = 13.0f;
	}

	// Create a transparent button

	SWFButton *btnCommon = new SWFButton();
	SWFShape *shpBtn = new SWFShape();
	SWFFill *fillBtn = shpBtn->addSolidFill(0x00, 0x00, 0x00, 0x00);
	shpBtn->setRightFill(fillBtn);
	DrawRectangle(shpBtn, 0.0f, 0.0f, width, height);
	btnCommon->addShape(shpBtn, SWFBUTTON_HIT);
	//btnCommon->addShape(preclip, SWFBUTTON_UP | SWFBUTTON_OVER | SWFBUTTON_DOWN);
	//btnCommon->addShape(shpBtn, SWFBUTTON_HIT | SWFBUTTON_DOWN);
	
	
	btnCommon->addShape(preClipNormal, SWFBUTTON_UP | SWFBUTTON_DOWN);
	btnCommon->addShape(preClipOver, SWFBUTTON_OVER);
	
	

	if (scriptPressed != NULL)
	{
		PrintActionScript(csButtonName, scriptPressed);
		if(csButtonName != "TimeSlider" && csButtonName != "VolumeSlider")
			btnCommon->addAction(new SWFAction(GetCharFromCString(scriptPressed)), SWFBUTTON_OVERDOWNTOOVERUP);
		else
			btnCommon->addAction(new SWFAction(GetCharFromCString(scriptPressed)), SWFBUTTON_HIT  | SWFBUTTON_DOWN);
	}

	if (scriptReleased != NULL)
	{
		PrintActionScript(csButtonName, scriptReleased);
		btnCommon->addAction(new SWFAction(GetCharFromCString(scriptReleased)), SWFBUTTON_MOUSEUP | SWFBUTTON_MOUSEUPOUTSIDE);
	}

	//if ((m_iFlashVersion > 5) && (!csButtonTitle.IsEmpty()))
		//CreateTooltip(btnCommon, csButtonTitle, _T("topright"));
	


	//di = sprCommon->add(preclip);
	di = sprCommon->add(btnCommon);
	
	CString csId;
    csId.Format(_T("swfBtn%s"), csButtonName);
    di->setName(GetCharFromCString(csId));
	sprCommon->nextFrame();

	// Add the SWFSprite to the parent
	di = pSprite->add(sprCommon);
	csId.Format(_T("swfSpr%s"), csButtonName);
	di->setName(GetCharFromCString(csId));

	di->moveTo(x, y);

	// Don't show hand cursor when mouse is over button area

	_TCHAR script[1024];
	_stprintf(script, _T("\
						 swfSpr%s.swfBtn%s.useHandCursor = false;\
						 "), csButtonName, csButtonName);
	PrintActionScript(_T("Common Slidestar buttons"), script);
	pSprite->add(new SWFAction(GetCharFromCString(script)));


	// Add ActionScript for button frame entered

	if (scriptEntered != NULL)
	{
		PrintActionScript(csButtonName, scriptEntered);
		di->addAction(new SWFAction(GetCharFromCString(scriptEntered)), SWFACTION_ENTERFRAME);
	}
}


UINT CFlashEngine::CreatePlayButton(SWFSprite *pSprite, float x, float y, float width, float height)
{
   HRESULT hr = S_OK;

   SWFShape *shpBtnUp = new SWFShape();
   SWFShape *shpBtnOver = new SWFShape();
   SWFShape *shpBtnDown = new SWFShape();
   SWFShape *shpBtnDisabled = new SWFShape();

   float size = (width < height) ? width : height;
   float dx = (width < height) ? 0.0f : (0.5f * (width-height));
   float dy = (width < height) ? (0.5f * (height-width)) : 0.0f;
   DrawPlayShape(shpBtnUp, dx, dy, size, m_rgbUp);
   DrawPlayShape(shpBtnOver, dx, dy, size, m_rgbOver);
   DrawPlayShape(shpBtnDown, dx, dy, size, m_rgbDown);
   DrawPlayShape(shpBtnDisabled, dx, dy, size, m_rgbDisabled);

   // Add ActionScript
   _TCHAR scriptPressed[1024];
   _stprintf(scriptPressed, _T("\
      var objRoot = _parent._parent;\
      if (!objRoot.g_bControlIsDisabled)\
      {\
         /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
         objRoot.g_bIgnoreStopmark = true;\
         objRoot.g_bIsInteractivePaused = false;\
         objRoot.lecDoGotoAndPlay(objRoot._currentframe);\
      }\
   "));

   CString csTitle;
   csTitle.LoadString(IDS_START_REPLAY_PAUSE);

   if (m_bIsPspDesign)
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_RECT_BUTTON, _T("Play"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);
   else
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_OVAL_BUTTON, _T("Play"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);

   return hr;
}

UINT CFlashEngine::CreatePauseButton(SWFSprite *pSprite, float x, float y, float width, float height)
{
   HRESULT hr = S_OK;

   SWFShape *shpBtnUp = new SWFShape();
   SWFShape *shpBtnOver = new SWFShape();
   SWFShape *shpBtnDown = new SWFShape();
   SWFShape *shpBtnDisabled = new SWFShape();

   float size = (width < height) ? width : height;
   float dx = (width < height) ? 0.0f : (0.5f * (width-height));
   float dy = (width < height) ? (0.5f * (height-width)) : 0.0f;
   DrawPauseShape(shpBtnUp, dx, dy, size, m_rgbUp);
   DrawPauseShape(shpBtnOver, dx, dy, size, m_rgbOver);
   DrawPauseShape(shpBtnDown, dx, dy, size, m_rgbDown);
   DrawPauseShape(shpBtnDisabled, dx, dy, size, m_rgbDisabled);

   // Add ActionScript
   _TCHAR scriptPressed[1024];
   _stprintf(scriptPressed, _T("\
      var objRoot = _parent._parent;\
      if (!objRoot.g_bControlIsDisabled)\
      {\
         objRoot.lecDoGotoAndStop(objRoot._currentframe);\
      }\
   "));

   CString csTitle;
   csTitle.LoadString(IDS_START_REPLAY_PAUSE);

   if (m_bIsPspDesign)
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_RECT_BUTTON, _T("Pause"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);
   else
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_OVAL_BUTTON, _T("Pause"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);
   return hr;
}

UINT CFlashEngine::CreateGotoFirstFrameButton(SWFSprite *pSprite, float x, float y, float width, float height)
{
   HRESULT hr = S_OK;

   SWFShape *shpBtnUp = new SWFShape();
   SWFShape *shpBtnOver = new SWFShape();
   SWFShape *shpBtnDown = new SWFShape();
   SWFShape *shpBtnDisabled = new SWFShape();

   float size = (width < height) ? width : height;
   float dx = (width < height) ? 0.0f : (0.5f * (width-height));
   float dy = (width < height) ? (0.5f * (height-width)) : 0.0f;
   DrawGotoFirstFrameShape(shpBtnUp, dx, dy, size, m_rgbUp);
   DrawGotoFirstFrameShape(shpBtnOver, dx, dy, size, m_rgbOver);
   DrawGotoFirstFrameShape(shpBtnDown, dx, dy, size, m_rgbDown);
   DrawGotoFirstFrameShape(shpBtnDisabled, dx, dy, size, m_rgbDisabled);

   // Add ActionScript
   // (First frame is 1 - not 0)
   _TCHAR scriptPressed[1024];
   _stprintf(scriptPressed, _T("\
      var objRoot = _parent._parent;\
      if (!objRoot.g_bControlIsDisabled)\
      {\
         if (objRoot.g_bIsPlaying || objRoot.g_bWillBePlaying || objRoot.g_bIsInteractivePaused)\
         {\
            objRoot.g_bIsInteractivePaused = false;\
            objRoot.lecDoGotoAndPlay(1);\
         }\
         else\
         {\
            objRoot.lecDoGotoAndStop(1);\
         }\
      }\
   "));

   CString csTitle;
   csTitle.LoadString(IDS_JUMP_TO_BEGIN);

   if (m_bIsPspDesign)
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_RECT_BUTTON, _T("GotoFirstFrame"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);
   else
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_OVAL_BUTTON, _T("GotoFirstFrame"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);
   return hr;
}

UINT CFlashEngine::CreateGotoLastFrameButton(SWFSprite *pSprite, float x, float y, float width, float height)
{
   HRESULT hr = S_OK;

   SWFShape *shpBtnUp = new SWFShape();
   SWFShape *shpBtnOver = new SWFShape();
   SWFShape *shpBtnDown = new SWFShape();
   SWFShape *shpBtnDisabled = new SWFShape();

   float size = (width < height) ? width : height;
   float dx = (width < height) ? 0.0f : (0.5f * (width-height));
   float dy = (width < height) ? (0.5f * (height-width)) : 0.0f;
   DrawGotoLastFrameShape(shpBtnUp, dx, dy, size, m_rgbUp);
   DrawGotoLastFrameShape(shpBtnOver, dx, dy, size, m_rgbOver);
   DrawGotoLastFrameShape(shpBtnDown, dx, dy, size, m_rgbDown);
   DrawGotoLastFrameShape(shpBtnDisabled, dx, dy, size, m_rgbDisabled);

   // Add ActionScript
   _TCHAR scriptPressed[1024];
   _stprintf(scriptPressed, _T("\
      var objRoot = _parent._parent;\
      if (!objRoot.g_bControlIsDisabled)\
      {\
         objRoot.lecDoGotoAndStop(objRoot._totalframes);\
      }\
   "));

   CString csTitle;
   csTitle.LoadString(IDS_JUMP_TO_END);

   if (m_bIsPspDesign)
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_RECT_BUTTON, _T("GotoLastFrame"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);
   else
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_OVAL_BUTTON, _T("GotoLastFrame"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);
   return hr;
}


UINT CFlashEngine::CreateGotoPrevPageButton(SWFSprite *pSprite, float x, float y, float width, float height)
{
   HRESULT hr = S_OK;

   SWFShape *shpBtnUp = new SWFShape();
   SWFShape *shpBtnOver = new SWFShape();
   SWFShape *shpBtnDown = new SWFShape();
   SWFShape *shpBtnDisabled = new SWFShape();

   float size = (width < height) ? width : height;
   float dx = (width < height) ? 0.0f : (0.5f * (width-height));
   float dy = (width < height) ? (0.5f * (height-width)) : 0.0f;
   DrawGotoPrevPageShape(shpBtnUp, dx, dy, size, m_rgbUp);
   DrawGotoPrevPageShape(shpBtnOver, dx, dy, size, m_rgbOver);
   DrawGotoPrevPageShape(shpBtnDown, dx, dy, size, m_rgbDown);
   DrawGotoPrevPageShape(shpBtnDisabled, dx, dy, size, m_rgbDisabled);

   // Add ActionScript
   _TCHAR scriptPressed[1024];
   _stprintf(scriptPressed, _T("\
      var objRoot = _parent._parent;\
      if (!objRoot.g_bControlIsDisabled)\
      {\
         objRoot.lecGotoPrevPage();\
      }\
      objRoot.g_bIsInteractivePaused = false;\
   "));

   CString csTitle;
   csTitle.LoadString(IDS_SLIDE_BACKWARDS);

   if (m_bIsPspDesign)
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_RECT_BUTTON, _T("GotoPrevPage"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);
   else
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_OVAL_BUTTON, _T("GotoPrevPage"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);

   return hr;
}

UINT CFlashEngine::CreateGotoNextPageButton(SWFSprite *pSprite, float x, float y, float width, float height)
{
   HRESULT hr = S_OK;

   SWFShape *shpBtnUp = new SWFShape();
   SWFShape *shpBtnOver = new SWFShape();
   SWFShape *shpBtnDown = new SWFShape();
   SWFShape *shpBtnDisabled = new SWFShape();

   float size = (width < height) ? width : height;
   float dx = (width < height) ? 0.0f : (0.5f * (width-height));
   float dy = (width < height) ? (0.5f * (height-width)) : 0.0f;
   DrawGotoNextPageShape(shpBtnUp, dx, dy, size, m_rgbUp);
   DrawGotoNextPageShape(shpBtnOver, dx, dy, size, m_rgbOver);
   DrawGotoNextPageShape(shpBtnDown, dx, dy, size, m_rgbDown);
   DrawGotoNextPageShape(shpBtnDisabled, dx, dy, size, m_rgbDisabled);

   // Add ActionScript
   _TCHAR scriptPressed[1024];
   _stprintf(scriptPressed, _T("\
      var objRoot = _parent._parent;\
      if (!objRoot.g_bControlIsDisabled)\
      {\
         objRoot.lecGotoNextPage();\
      }\
      objRoot.g_bIsInteractivePaused = false;\
   "));

   CString csTitle;
   csTitle.LoadString(IDS_SLIDE_FORWARD);

   if (m_bIsPspDesign)
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_RECT_BUTTON, _T("GotoNextPage"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);
   else
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_OVAL_BUTTON, _T("GotoNextPage"), csTitle,
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);

   return hr;
}

UINT CFlashEngine::CreateSoundOnButton(SWFSprite *pSprite, float x, float y, float width, float height)
{
   HRESULT hr = S_OK;

   SWFShape *shpBtnUp = new SWFShape();
   SWFShape *shpBtnOver = new SWFShape();
   SWFShape *shpBtnDown = new SWFShape();
   SWFShape *shpBtnDisabled = new SWFShape();

   float size = (width < height) ? width : height;
   float dx = (width < height) ? 0.0f : (0.5f * (width-height));
   float dy = (width < height) ? (0.5f * (height-width)) : 0.0f;
   DrawVolumeShape(shpBtnUp, dx, dy, size, m_rgbDown);
   DrawVolumeShape(shpBtnOver, dx, dy, size, m_rgbOver);
   DrawVolumeShape(shpBtnDown, dx, dy, size, m_rgbUp);
   DrawVolumeShape(shpBtnDisabled, dx, dy, size, m_rgbDisabled);

   // Add ActionScript
   _TCHAR scriptPressed[1024];
   _stprintf(scriptPressed, _T("\
      var objRoot = _parent._parent;\
      objRoot.g_Sound.setVolume(0);\
      objRoot.g_bIsSoundOn = false;\
   "));

   CString csTitle;
   csTitle.LoadString(IDS_SOUND_ON_OFF);

   if (m_bIsPspDesign)
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_RECT_BUTTON, _T("SoundOn"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);
   else
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_OVAL_BUTTON, _T("SoundOn"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);

   return hr;
}

UINT CFlashEngine::CreateSoundOffButton(SWFSprite *pSprite, float x, float y, float width, float height)
{
   HRESULT hr = S_OK;

   SWFShape *shpBtnUp = new SWFShape();
   SWFShape *shpBtnOver = new SWFShape();
   SWFShape *shpBtnDown = new SWFShape();
   SWFShape *shpBtnDisabled = new SWFShape();

   float size = (width < height) ? width : height;
   float dx = (width < height) ? 0.0f : (0.5f * (width-height));
   float dy = (width < height) ? (0.5f * (height-width)) : 0.0f;
   DrawVolumeShape(shpBtnUp, dx, dy, size, m_rgbUp);
   DrawVolumeShape(shpBtnOver, dx, dy, size, m_rgbOver);
   DrawVolumeShape(shpBtnDown, dx, dy, size, m_rgbDown);
   DrawVolumeShape(shpBtnDisabled, dx, dy, size, m_rgbDisabled);

   // Add ActionScript
   _TCHAR scriptPressed[1024];
   _stprintf(scriptPressed, _T("\
      var objRoot = _parent._parent;\
      objRoot.g_Sound.setVolume(objRoot.g_nVolume);\
      objRoot.g_bIsSoundOn = true;\
   "));

   CString csTitle;
   csTitle.LoadString(IDS_SOUND_ON_OFF);

   if (m_bIsPspDesign)
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_RECT_BUTTON, _T("SoundOff"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);
   else
      DrawCommon3dButton(pSprite, x, y, width, height, D3D_OVAL_BUTTON, _T("SoundOff"), csTitle, 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, NULL, NULL);

   return hr;
}   

UINT CFlashEngine::CreateSlidestarSoundOnButton(SWFSprite *pSprite, float x, float y)
{
	HRESULT hr = S_OK;

	// Add ActionScript
   _TCHAR scriptPressed[1024];
   _stprintf(scriptPressed, _T("\
      var objRoot = _parent._parent;\
      objRoot.g_Sound.setVolume(0);\
      objRoot.g_bIsSoundOn = false;\
   "));

   CString csTitle;
   csTitle.LoadString(IDS_SOUND_ON_OFF);

   CreateSlidestarCommonButtons(pSprite, x, y, _T("SoundOn"), csTitle, ".\\Flash\\Slidestar\\speaker_on_up.swf", ".\\Flash\\Slidestar\\speaker_on_over.swf", scriptPressed, NULL, NULL);

   return hr;
}

UINT CFlashEngine::CreateSlidestarSoundOffButton(SWFSprite *pSprite, float x, float y)
{
	HRESULT hr = S_OK;

	// Add ActionScript
   _TCHAR scriptPressed[1024];
   _stprintf(scriptPressed, _T("\
      var objRoot = _parent._parent;\
      objRoot.g_Sound.setVolume(objRoot.g_nVolume);\
      objRoot.g_bIsSoundOn = true;\
   "));

   CString csTitle;
   csTitle.LoadString(IDS_SOUND_ON_OFF);

   CreateSlidestarCommonButtons(pSprite, x, y, _T("SoundOff"), csTitle, ".\\Flash\\Slidestar\\speaker_off_up.swf", ".\\Flash\\Slidestar\\speaker_off_over.swf", scriptPressed, NULL, NULL);
   
   return hr;
}
// --------------------------------------------------------------------------
UINT CFlashEngine::CreateSlidestarTimeSliderButton(SWFSprite *pSprite, float x, float y, float length,
                                                   float width, float height)
{
	HRESULT hr = S_OK;

   // 'length' is the length of the time line
   float left = x;
   float top = y;
   float right = x;
   float bottom = y;


   left -= 0.5f * width;
   right += length - (0.5f * width);

   _TCHAR scriptPressed[1024];
   _stprintf(scriptPressed, _T("\
      var objRoot = _parent._parent;\
      if (!objRoot.g_bControlIsDisabled)\
      {\
         objRoot.g_bIsTimeSliding = true;\
         /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
         objRoot.g_bIgnoreStopmark = true;\
         g_mousePos = this._x;\
         startDrag(false, %g, %g, %g, %g);\
         /*Stop Replay during sliding*/\
         objRoot.g_bWasPlaying = objRoot.g_bIsPlaying;\
         objRoot.lecDoStop();\
      }\
   "), left, top, right, bottom);

   _TCHAR scriptReleased[1024];
   _stprintf(scriptReleased, _T("\
         var objRoot = _parent._parent;\
         if (!objRoot.g_bControlIsDisabled)\
         {\
            if (objRoot.g_bIsTimeSliding)\
            {\
               g_mousePos = this._x;\
			      stopDrag();\
               objRoot.g_bIsTimeSliding = false;\
               /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
               objRoot.g_bIgnoreStopmark = true;\
               \
			      var x = objRoot._xmouse;\
               x = x - objRoot.swfSprControl._x - %g;\
               var nDestFrame = 1 + Math.round((objRoot._totalframes-1) * (x / (1.0*%g)));\
               \
               if (objRoot.g_bWasPlaying  || objRoot.g_bWillBePlaying|| objRoot.g_bIsInteractivePaused)\
               {\
                  objRoot.g_bIsInteractivePaused = false;\
                  objRoot.lecDoGotoAndPlay(nDestFrame);\
               }\
               else\
               {\
                  objRoot.lecDoGotoAndStop(nDestFrame);\
               }\
            }\
         }\
      "), left, length);
 

   // (First frame is 1 - not 0)
   _TCHAR scriptEntered[4096];
   _stprintf(scriptEntered, _T("\
         var objRoot = _parent._parent;\
		 var lineWidth = %g;\
		 var x = 4;\
		 var y = 6;\
		 if (objRoot.g_bIsTimeSliding)\
         {\
            var xMouse = objRoot._xmouse;\
            xMouse = xMouse - objRoot.swfSprControl._x - %g;\
            var nDestFrame = 1 + Math.round((objRoot._totalframes-1) * (xMouse / (1.0*%g)));\
            objRoot.lecScrollTo(nDestFrame);\
			   objRoot.g_bTimeSlidingPerformed = true;\
         }\
		 else\
         {\
            var actualFrame = objRoot._currentframe - 1;\
			if (actualFrame < 0)\
				actualFrame = 0;\
            this._x = %g + Math.round(%g * (actualFrame / (1.0*(objRoot._totalframes-1))));\
         }\
		 this.clear();\
		 if(!objRoot.g_bIsTimeSliding && objRoot.g_bTimeSlidingPerformed)\
		 {\
			this.moveTo(x, y);\
			this.lineStyle(1,0x00FF00,100);\
			this.lineTo((lineWidth-this._x), y);\
		 }\
		 else\
			if(!objRoot.g_bIsTimeSliding && !objRoot.g_bTimeSlidingPerformed)\
			{\
				this.moveTo(x, (y+1));\
				this.lineStyle(1,0x00FF00,100);\
				this.lineTo((lineWidth-this._x), (y+1));\
			}\
		 else\
		 {\
			if(g_mousePos == this._x)\
			{\
				this.moveTo(x, y);\
				this.lineStyle(1,0x00FF00,100);\
				this.lineTo((lineWidth-this._x), y);\
			}\
			else\
			{\
				g_mousePos = this._x;\
			}\
		 }\
      "), x, (left+width), length, (left-0.5f*width), length);

   CreateSlidestarCommonButtons(pSprite, left, top-0.5f, _T("TimeSlider"), _T(""), 
								".\\Flash\\slidestar\\position_arrow.swf", ".\\Flash\\slidestar\\position_arrow.swf", scriptPressed, scriptReleased, scriptEntered);

   return hr;
}

UINT CFlashEngine::CreateTimeSliderButton(SWFSprite *pSprite, float x, float y, float length, 
                                          float width, float height, bool bIsVertical)
{
   HRESULT hr = S_OK;

   // 'length' is the length of the time line
   float left = x;
   float top = y;
   float right = x;
   float bottom = y;

   if (bIsVertical)
   {
      top -= 1.0f * height;
      bottom += length - (1.0f * height);
   }
   else
   {
      left -= 1.0f * width;
      right += length - (1.0f * width);
   }


   SWFShape *shpBtnUp = new SWFShape();
   SWFShape *shpBtnOver = new SWFShape();
   SWFShape *shpBtnDown = new SWFShape();
   SWFShape *shpBtnDisabled = new SWFShape();


//   Draw3DRoundedRectangle(shpBtnUp, 0.0f, 0.0f, width, height, 2.0f, 1, D3D_PEN_PALETTE_DARK, 
//      D3D_FULL_ROUND, D3D_BORDER, D3D_FILL, D3D_RAISED, D3D_LINEAR_GRADIENT, m_swfLinearGradientMouseUp);
//   Draw3DRoundedRectangle(shpBtnOver, 0.0f, 0.0f, width, height, 2.0f, 1, D3D_PEN_PALETTE_DARK, 
//      D3D_FULL_ROUND, D3D_BORDER, D3D_FILL, D3D_RAISED, D3D_LINEAR_GRADIENT, m_swfLinearGradientMouseDown);
//   Draw3DRoundedRectangle(shpBtnDown, 0.0f, 0.0f, width, height, 2.0f, 1, D3D_PEN_PALETTE_DARK, 
//      D3D_FULL_ROUND, D3D_BORDER, D3D_FILL, D3D_RAISED, D3D_LINEAR_GRADIENT, m_swfLinearGradientMouseDown);
//   Draw3DRoundedRectangle(shpBtnDisabled, 0.0f, 0.0f, width, height, 2.0f, 1, D3D_PEN_PALETTE_DARK, 
//      D3D_FULL_ROUND, D3D_BORDER, D3D_FILL, D3D_RAISED, D3D_LINEAR_GRADIENT, m_swfLinearGradientMouseUp);


   // Add ActionScript to the Button
   _TCHAR scriptPressed[1024];
   if (bIsVertical)
   {
      _stprintf(scriptPressed, _T("\
         var objRoot = _parent._parent;\
         this.dy = objRoot.swfSprControl._ymouse - (this._y + this._height);\
         if (!objRoot.g_bControlIsDisabled)\
         {\
            objRoot.g_bIsTimeSliding = true;\
            /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
            objRoot.g_bIgnoreStopmark = true;\
            startDrag(false, %g, %g, %g, %g);\
            /*Stop Replay during sliding*/\
            objRoot.g_bWasPlaying = objRoot.g_bIsPlaying;\
            objRoot.lecDoStop();\
         }\
      "), left, top, right, bottom);
   }
   else
   {
      _stprintf(scriptPressed, _T("\
         var objRoot = _parent._parent;\
         this.dx = objRoot.swfSprControl._xmouse - this._x;\
         if (!objRoot.g_bControlIsDisabled)\
         {\
            objRoot.g_bIsTimeSliding = true;\
            /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
            objRoot.g_bIgnoreStopmark = true;\
            startDrag(false, %g, %g, %g, %g);\
            /*Stop Replay during sliding*/\
            objRoot.g_bWasPlaying = objRoot.g_bIsPlaying;\
            objRoot.lecDoStop();\
         }\
      "), left, top, right, bottom);
   }

   _TCHAR scriptReleased[1024];
   if (bIsVertical)
   {
      _stprintf(scriptReleased, _T("\
         var objRoot = _parent._parent;\
         if (!objRoot.g_bControlIsDisabled)\
         {\
            if (objRoot.g_bIsTimeSliding)\
            {\
               stopDrag();\
               objRoot.g_bIsTimeSliding = false;\
               /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
               objRoot.g_bIgnoreStopmark = true;\
               \
               var y = objRoot.swfSprControl._ymouse;\
               y = y - this.dy - %g;\
               var nDestFrame = 1 + Math.round((objRoot._totalframes-1) * ((%g - y) / %g));\
               \
               if (objRoot.g_bWasPlaying || objRoot.g_bWillBePlaying || objRoot.g_bIsInteractivePaused)\
               {\
                  objRoot.g_bIsInteractivePaused = false;\
                  objRoot.lecDoGotoAndPlay(nDestFrame);\
               }\
               else\
               {\
                  objRoot.lecDoGotoAndStop(nDestFrame);\
               }\
            }\
         }\
      "), (top+height), length, length);
   }
   else
   {
      _stprintf(scriptReleased, _T("\
         var objRoot = _parent._parent;\
         if (!objRoot.g_bControlIsDisabled)\
         {\
            if (objRoot.g_bIsTimeSliding)\
            {\
               stopDrag();\
               objRoot.g_bIsTimeSliding = false;\
               /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
               objRoot.g_bIgnoreStopmark = true;\
               \
               var x = objRoot.swfSprControl._xmouse;\
               x = x - this.dx - %g;\
               var nDestFrame = 1 + Math.round((objRoot._totalframes-1) * (x / %g));\
               \
               if (objRoot.g_bWasPlaying || objRoot.g_bWillBePlaying || objRoot.g_bIsInteractivePaused)\
               {\
                  objRoot.g_bIsInteractivePaused = false;\
                  objRoot.lecDoGotoAndPlay(nDestFrame);\
               }\
               else\
               {\
                  objRoot.lecDoGotoAndStop(nDestFrame);\
               }\
            }\
         }\
      "), left, length);
   }

   // (First frame is 1 - not 0)
   _TCHAR scriptEntered[4096];
   if (bIsVertical)
   {
      _stprintf(scriptEntered, _T("\
         var objRoot = _parent._parent;\
         if (objRoot.g_bIsTimeSliding)\
         {\
            var y = objRoot.swfSprControl._ymouse;\
            y = y - objRoot.swfSprControl.swfSprTimeSlider.dy - %g;\
            var nDestFrame = 1 + Math.round((objRoot._totalframes-1) * ((%g - y) / %g));\
            objRoot.lecScrollTo(nDestFrame);\
         }\
         else\
         {\
            var actualFrame = objRoot._currentframe - 1;\
            if (actualFrame < 0)\
               actualFrame = 0;\
            this._y = %g + %g - Math.round(%g * (actualFrame / (objRoot._totalframes-1)));\
         }\
         \
      "), (top+height), length, length, top, length, length);
   }
   else
   {
      _stprintf(scriptEntered, _T("\
         var objRoot = _parent._parent;\
         if (objRoot.g_bIsTimeSliding)\
         {\
            var x = objRoot.swfSprControl._xmouse;\
            x = x - objRoot.swfSprControl.swfSprTimeSlider.dx - %g;\
            var nDestFrame = 1 + Math.round((objRoot._totalframes-1) * (x / %g));\
            objRoot.lecScrollTo(nDestFrame);\
         }\
         else\
         {\
            var actualFrame = objRoot._currentframe - 1;\
            if (actualFrame < 0)\
               actualFrame = 0;\
            this._x = %g + Math.round(%g * (actualFrame / (objRoot._totalframes-1)));\
         }\
         \
      "), left, length, left, length);
   }

   DrawCommon3dButton(pSprite, left, top-0.5f, width, height, D3D_SLIDER_BUTTON, _T("TimeSlider"), _T(""), 
                      shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                      scriptPressed, scriptReleased, scriptEntered);

   return hr;
}

// --------------------------------------------------------------------------

   UINT CFlashEngine::CreateSlidestarVolumeSliderButton(SWFSprite *pSprite, float x, float y, float length,
      float width, float height)
{
	HRESULT hr = S_OK;

   // 'length' is the length of the volume line
   float left = x;
   float top = y;
   float right = x;
   float bottom = y;

   left -= 0.5f * width;
   right += length - (0.5f * width);

 
   // Add ActionScript to the Button
   _TCHAR scriptPressed[1024];
   _stprintf(scriptPressed, _T("\
      var objRoot = _parent._parent;\
      startDrag(false, %g, %g, %g, %g);\
	  objRoot.g_bIsVolumeSliding=true;\
   "), left, top, right, bottom);

   _TCHAR scriptReleased[1024];
   _stprintf(scriptReleased, _T("\
      var objRoot = _parent._parent;\
      stopDrag();\
      objRoot.g_bIsVolumeSliding=false;\
   "));

   _TCHAR scriptEntered[4096];
   
   _stprintf(scriptEntered, _T("\
     var objRoot = _parent._parent;\
     if (objRoot.g_bIsVolumeSliding)\
     {\
		if (objRoot.g_bIsSoundOn)\
        {\
			var x = objRoot._xmouse;\
            x = x - objRoot.swfSprControl._x - %g;\
            if (x > 100)\
				x = 100;\
            x = Math.floor(100.0 * (x / (1.0*%g)));\
            objRoot.g_Sound.setVolume(Math.floor(x));\
            objRoot.g_nVolume = Math.floor(x);\
        }\
     }\
     else\
     {\
		if (objRoot.g_nVolume > 100)\
			objRoot.g_nVolume = 100;\
        this._x = %g + Math.round((1.0*%g) * (objRoot.g_nVolume / 100.0));\
      }\
    "), left, length, (left-0.5f*width), length);
  
   CreateSlidestarCommonButtons(pSprite, left+60.0f, top-0.5f, _T("VolumeSlider"), _T(""), 
								".\\Flash\\slidestar\\volume_arrow.swf", ".\\Flash\\slidestar\\volume_arrow.swf", scriptPressed, scriptReleased, scriptEntered);

   return hr;
}
UINT CFlashEngine::CreateVolumeSliderButton(SWFSprite *pSprite, float x, float y, float length, 
                                            float width, float height, bool bIsVertical)
{
   HRESULT hr = S_OK;

   // 'length' is the length of the volume line
   float left = x;
   float top = y;
   float right = x;
   float bottom = y;

   if (bIsVertical)
   {
      top -= 1.0f * height;
      bottom += length - (1.0f * height);
   }
   else
   {
      left -= 1.0f * width;
      right += length - (1.0f * width);
   }



   SWFShape *shpBtnUp = new SWFShape();
   SWFShape *shpBtnOver = new SWFShape();
   SWFShape *shpBtnDown = new SWFShape();
   SWFShape *shpBtnDisabled = new SWFShape();


//   Draw3DRoundedRectangle(shpBtnUp, 0.0f, 0.0f, width, height, 2.0f, 1, D3D_PEN_PALETTE_DARK, 
//      D3D_FULL_ROUND, D3D_BORDER, D3D_FILL, D3D_RAISED, D3D_LINEAR_GRADIENT, m_swfLinearGradientMouseUp);
//   Draw3DRoundedRectangle(shpBtnOver, 0.0f, 0.0f, width, height, 2.0f, 1, D3D_PEN_PALETTE_DARK, 
//      D3D_FULL_ROUND, D3D_BORDER, D3D_FILL, D3D_RAISED, D3D_LINEAR_GRADIENT, m_swfLinearGradientMouseDown);
//   Draw3DRoundedRectangle(shpBtnDown, 0.0f, 0.0f, width, height, 2.0f, 1, D3D_PEN_PALETTE_DARK, 
//      D3D_FULL_ROUND, D3D_BORDER, D3D_FILL, D3D_RAISED, D3D_LINEAR_GRADIENT, m_swfLinearGradientMouseDown);
//   Draw3DRoundedRectangle(shpBtndisabled, 0.0f, 0.0f, width, height, 2.0f, 1, D3D_PEN_PALETTE_DARK, 
//      D3D_FULL_ROUND, D3D_BORDER, D3D_FILL, D3D_RAISED, D3D_LINEAR_GRADIENT, m_swfLinearGradientMouseUp);


   // Add ActionScript to the Button
   _TCHAR scriptPressed[1024];
   if (bIsVertical)
   {
      _stprintf(scriptPressed, _T("\
         var objRoot = _parent._parent;\
         this.dy = objRoot.swfSprControl._ymouse - this._y;\
         startDrag(false, %g, %g, %g, %g);\
         objRoot.g_bIsVolumeSliding = true;\
      "), left, top, right, bottom);
   }
   else
   {
      _stprintf(scriptPressed, _T("\
         var objRoot = _parent._parent;\
         this.dx = objRoot.swfSprControl._xmouse - this._x;\
         startDrag(false, %g, %g, %g, %g);\
         objRoot.g_bIsVolumeSliding = true;\
      "), left, top, right, bottom);
   }

   _TCHAR scriptReleased[1024];
   _stprintf(scriptReleased, _T("\
      var objRoot = _parent._parent;\
      stopDrag();\
      objRoot.g_bIsVolumeSliding = false;\
   "));

   _TCHAR scriptEntered[4096];
   if (bIsVertical)
   {
      _stprintf(scriptEntered, _T("\
         var objRoot = _parent._parent;\
         if (objRoot.g_bIsVolumeSliding)\
         {\
            if (objRoot.g_bIsSoundOn)\
            {\
               var y = objRoot.swfSprControl._ymouse;\
               y = y - %g;\
               y = Math.floor(100.0 * ((%g - y) / %g));\
               if (y > 100)\
                  y = 100;\
               objRoot.g_Sound.setVolume(Math.floor(y));\
               objRoot.g_nVolume = Math.floor(y);\
            }\
         }\
         else\
         {\
            if (objRoot.g_nVolume > 100)\
               objRoot.g_nVolume = 100;\
            if (!this.dy)\
               this.dy = 0.0;\
            this._y = %g - this.dy + %g - Math.round((1.0*%g) * (objRoot.g_nVolume / 100.0));\
         }\
      "), top, length, length, top, length, length);
   }
   else
   {
      _stprintf(scriptEntered, _T("\
         var objRoot = _parent._parent;\
         if (objRoot.g_bIsVolumeSliding)\
         {\
            if (objRoot.g_bIsSoundOn)\
            {\
               var x = objRoot.swfSprControl._xmouse;\
               x = x - %g;\
               if (x > 100)\
                  x = 100;\
               x = Math.floor(100.0 * (x / %g));\
               objRoot.g_Sound.setVolume(Math.floor(x));\
               objRoot.g_nVolume = Math.floor(x);\
            }\
         }\
         else\
         {\
            if (objRoot.g_nVolume > 100)\
               objRoot.g_nVolume = 100;\
            if (!this.dx)\
               this.dx = 0.0;\
            this._x = %g - this.dx + Math.round((1.0*%g) * (objRoot.g_nVolume / 100.0));\
         }\
      "), left, length, left, length);
   }

   if (m_bIsPspDesign)
      DrawCommon3dButton(pSprite, left-0.0f, top+60.f, width, height, D3D_SLIDER_BUTTON, _T("VolumeSlider"), _T(""), 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, scriptReleased, scriptEntered);
   else
      DrawCommon3dButton(pSprite, left+40.0f, top-0.5f, width, height, D3D_SLIDER_BUTTON, _T("VolumeSlider"), _T(""), 
                         shpBtnUp, shpBtnOver, shpBtnDown, shpBtnDisabled, 
                         scriptPressed, scriptReleased, scriptEntered);

   return hr;
}

// --------------------------------------------------------------------------
 UINT CFlashEngine::CreateSlidestarSliderLine(SWFSprite *pSprite, float x, float y, 
                                    CString csName, char *cSwfName)
{
   HRESULT hr = S_OK;

   SWFSprite *sprLine = new SWFSprite();
   SWFPrebuiltClip *preclip = new SWFPrebuiltClip(cSwfName);

   SWFDisplayItem *di;
   di = sprLine->add(preclip);
   sprLine->nextFrame();
   di = pSprite->add(sprLine);
   CString csId;
   csId.Format(_T("swfSpr%s"), csName);
   di->setName(GetCharFromCString(csId));
   di->move(x,y);

   return hr;
}

UINT CFlashEngine::CreateSliderLine(SWFSprite *pSprite, float x, float y, float length, float size, 
                                    CString csName, bool bIsVertical)
{
   HRESULT hr = S_OK;

   SWFSprite *sprLine = new SWFSprite();

   SWFShape *shpLine = new SWFShape();
   MYRGB rgbBackground;
//   rgbBackground.r = 0xCC; rgbBackground.g = 0xC4; rgbBackground.b = 0xA4; 
   rgbBackground = m_rgb080;
   if (bIsVertical)
      Draw3DRoundedRectangle(shpLine, x, y, size, length, 0.25f*size, 1, D3D_PEN_PALETTE_DARK, 
         D3D_FULL_ROUND, D3D_BORDER, D3D_FILL, D3D_DEEPENED, D3D_NO_GRADIENT, NULL, NULL, &rgbBackground);
   else
      Draw3DRoundedRectangle(shpLine, x, y, length, size, 0.25f*size, 1, D3D_PEN_PALETTE_DARK, 
         D3D_FULL_ROUND, D3D_BORDER, D3D_FILL, D3D_DEEPENED, D3D_NO_GRADIENT, NULL, NULL, &rgbBackground);

   SWFDisplayItem *di;
   di = sprLine->add(shpLine);
   sprLine->nextFrame();
   di = pSprite->add(sprLine);
   CString csId;
   csId.Format(_T("swfSpr%s"), csName);
   di->setName(GetCharFromCString(csId));

   return hr;
}

UINT CFlashEngine::CreateTextFieldBox(SWFSprite *pSprite, float x, float y, float length, float size, 
                                      CString csName)
{
   HRESULT hr = S_OK;

   SWFSprite *sprBox = new SWFSprite();

   SWFShape *shpTextBox = new SWFShape();

   if (m_bIsPspDesign)
   {
      shpTextBox->setLine(1, m_rgb067.r, m_rgb067.g, m_rgb067.b);
      DrawRectangle(shpTextBox, x, y, length, size, 1.0f);
   }
   else
   {
      MYRGB rgbBackground;
//      rgbBackground.r = 0xDC; rgbBackground.g = 0xD7; rgbBackground.b = 0xC0; 
      rgbBackground = m_rgb090;
      Draw3DRoundedRectangle(shpTextBox, x, y, length, size, 0.33f*size, 2, D3D_PEN_PALETTE, 
         D3D_FULL_ROUND, D3D_BORDER, D3D_FILL, D3D_DEEPENED, D3D_NO_GRADIENT, NULL, NULL, &rgbBackground);
   }

   SWFDisplayItem *di;
   di = sprBox->add(shpTextBox);
   sprBox->nextFrame();
   di = pSprite->add(sprBox);
   CString csId;
   csId.Format(_T("swfSprTextFieldBox%s"), csName);
   di->setName(GetCharFromCString(csId));

   return hr;
}

// --------------------------------------------------------------------------

UINT CFlashEngine::CreateTimeDisplay(SWFSprite *pSprite, float x, float y, float fontHeight, float scalePsp)
{
   HRESULT hr = S_OK;

   SWFDisplayItem *di;

   SWFSprite *sprTextfield = new SWFSprite();

   SWFTextField *txtField = new SWFTextField();
   if(!m_bIsSlidestar)
   {
	   txtField->setFont(m_pFntStd);
	   txtField->setBounds(100.0f*scalePsp, 12.0f*scalePsp); // scalePsp: 1.0 for non Psp design
	   txtField->setColor(0x00, 0x00, 0x00);
   }
   else
   {
		txtField->setFont(new SWFFont("Verdana"));
		txtField->setBounds(90.0f, 12.0f);
		txtField->setColor(255, 255, 255, 255);
   }
   txtField->setHeight(fontHeight);
   txtField->setAlignment(SWFTEXTFIELD_ALIGN_LEFT);
   txtField->setVariableName("text");
   txtField->setFlags(SWFTEXTFIELD_USEFONT | SWFTEXTFIELD_NOEDIT | SWFTEXTFIELD_NOSELECT | SWFTEXTFIELD_HASLAYOUT);
   CString csText;
   csText.LoadString(IDS_FLASH_PRELOAD);
   csText += _T("0123456789:");
   CString csInitialText = _T("00:00:00");
   char *utfText = LString::TCharToUtf8(csText);
   char *utfInitialText = LString::TCharToUtf8(csInitialText);
   if (utfText)
   {
      txtField->addUTF8Chars(utfText);
      txtField->addUTF8String(utfInitialText);
      delete[] utfText;
      utfText = NULL;
      delete[] utfInitialText;
      utfInitialText = NULL;
   }

   di = sprTextfield->add(txtField);
   di->setName("swfTxtTime");
   sprTextfield->nextFrame();
   di = pSprite->add(sprTextfield);
   di->setName("swfSprTxtTime");
   di->moveTo(x,y);

   return hr;
}

UINT CFlashEngine::CreateSlideDisplay(SWFSprite *pSprite, float x, float y, float fontHeight, float scalePsp)
{
   HRESULT hr = S_OK;

   SWFDisplayItem *di;

   SWFSprite *sprTextfield = new SWFSprite();

   SWFTextField *txtField = new SWFTextField();
   txtField->setFont(m_pFntStd);
   txtField->setBounds(100.0f*scalePsp, 12.0f*scalePsp); // scalePsp: 1.0 for non Psp design
//   txtField->setLength(10);
   txtField->setHeight(fontHeight);
   txtField->setColor(0x00, 0x00, 0x00);
   txtField->setVariableName("text");
   CString csTxtSlide;
   int nNumOfPages = m_aiPagingFrames.GetSize();
   csTxtSlide.Format(_T(""));

   txtField->setAlignment(SWFTEXTFIELD_ALIGN_LEFT);
   txtField->setVariableName("text");
   txtField->setFlags(SWFTEXTFIELD_USEFONT | SWFTEXTFIELD_NOEDIT | SWFTEXTFIELD_NOSELECT | SWFTEXTFIELD_HASLAYOUT);
   
   CString csSlide;
   csSlide.LoadString(IDS_FLASH_PAGE); //IDS_FLASH_SLIDE
   CString csOf;
   csOf.LoadString(IDS_FLASH_OF);

   CString csText;
   csText.Format(_T("0123456789 %s%s"), csSlide, csOf);
   CString csInitialText = _T(" ");
   char *utfText = LString::TCharToUtf8(csText);
   char *utfInitialText = LString::TCharToUtf8(csInitialText);
   if (utfText)
   {
      txtField->addUTF8Chars(utfText);
      txtField->addUTF8String(utfInitialText);
      delete[] utfText;
      utfText = NULL;
      delete[] utfInitialText;
      utfInitialText = NULL;
   }
   di = sprTextfield->add(txtField);
   di->setName("swfTxtSlide");
   sprTextfield->nextFrame();
   di = pSprite->add(sprTextfield);
   di->setName("swfSprTxtSlide");
   di->moveTo(x, y);

   return hr;
}

 UINT CFlashEngine::CreateSlidestarActiveTimeLine(SWFSprite *pSprite, float x, float y, float length, float size)
{
   HRESULT hr = S_OK;

   // Create a transparent button
   SWFButton *btnActive = new SWFButton();
   SWFShape *shpBtn = new SWFShape();
   SWFFill *fillBtn = shpBtn->addSolidFill(0x00, 0x00, 0x00, 0x00);
   shpBtn->setRightFill(fillBtn);
   DrawRectangle(shpBtn, x, y, length, size);
   btnActive->addShape(shpBtn, SWFBUTTON_HIT);
			

   float left = x; 
   _TCHAR script[1024];
   _stprintf(script, _T("\
         var objRoot = _parent;\
         if (!objRoot.g_bIsTimeSliding && !objRoot.g_bControlIsDisabled && objRoot.g_nNavigationTimeLine < 1)\
         {\
            /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
            objRoot.g_bIgnoreStopmark = true;\
            var x = objRoot._xmouse;\
            x = x  - objRoot.swfSprControl._x - %g;\
            var nDestFrame = 1 + Math.round((objRoot._totalframes-1) * (x / (1.0*%g)));\
            if (objRoot.g_bIsPlaying || objRoot.g_bWillBePlaying || objRoot.g_bIsInteractivePaused)\
            {\
               objRoot.g_bIsInteractivePaused = false;\
               objRoot.lecDoGotoAndPlay(nDestFrame);\
            }\
            else\
            {\
               objRoot.lecDoGotoAndStop(nDestFrame);\
            }\
         }\
         \
      "), left, length);
   
   PrintActionScript(_T("Active Line"), script);
   btnActive->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEDOWN);

   SWFDisplayItem *di;
   di = pSprite->add(btnActive);

   CString csId;
   csId.Format(_T("swfActiveTimeLine"));
   di->setName(GetCharFromCString(csId));
   // For Flash version 6 and higher: useHandCursor works this way
   CString csUseHand = _T("false");
   _stprintf(script, _T("\
      %s.useHandCursor = %s;\
   "), csId, csUseHand);
   PrintActionScript(_T("Active Time Line"), script);
   pSprite->add(new SWFAction(GetCharFromCString(script)));

   return hr;
   
}
UINT CFlashEngine::CreateActiveTimeLine(SWFSprite *pSprite, float x, float y, float length, float size, 
                                        float btnWidth, bool bIsVertical)
{
   HRESULT hr = S_OK;

   // Create a transparent button
   SWFButton *btnActive = new SWFButton();
   SWFShape *shpBtn = new SWFShape();
   SWFFill *fillBtn = shpBtn->addSolidFill(0x00, 0x00, 0x00, 0x00);
   shpBtn->setRightFill(fillBtn);
   if (bIsVertical)
      DrawRectangle(shpBtn, x, y, size, length);
   else
      DrawRectangle(shpBtn, x, y, length, size);
   btnActive->addShape(shpBtn, SWFBUTTON_HIT);

   float left = x; //- (0.5f*btnWidth);
   float btnHeight = btnWidth;
   float top  = y; //- (0.5f*btnHeight);

   // Add ActionScript
   _TCHAR scriptPressed[1024];
   if (bIsVertical)
   {
      _stprintf(scriptPressed, _T("\
         var objRoot = _parent;\
         if (!objRoot.g_bIsTimeSliding && !objRoot.g_bControlIsDisabled && objRoot.g_nNavigationTimeLine < 1)\
         {\
            /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
            objRoot.g_bIgnoreStopmark = true;\
            var y = this._ymouse;\
            y = y - %g;\
            var nDestFrame = 1 + Math.round((objRoot._totalframes-1) * ((%g - y) / %g));\
            if (objRoot.g_bIsPlaying || objRoot.g_bWillBePlaying || objRoot.g_bIsInteractivePaused)\
            {\
               objRoot.g_bIsInteractivePaused = false;\
               objRoot.lecDoGotoAndPlay(nDestFrame);\
            }\
            else\
            {\
               objRoot.lecDoGotoAndStop(nDestFrame);\
            }\
         }\
         \
      "), top, length, length);
   }
   else
   {
      _stprintf(scriptPressed, _T("\
         var objRoot = _parent;\
         if (!objRoot.g_bIsTimeSliding && !objRoot.g_bControlIsDisabled && objRoot.g_nNavigationTimeLine < 1)\
         {\
            /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
            objRoot.g_bIgnoreStopmark = true;\
            var x = this._xmouse;\
            x = x - %g;\
            var nDestFrame = 1 + Math.round((objRoot._totalframes-1) * (x / %g));\
            if (objRoot.g_bIsPlaying || objRoot.g_bWillBePlaying || objRoot.g_bIsInteractivePaused)\
            {\
               objRoot.g_bIsInteractivePaused = false;\
               objRoot.lecDoGotoAndPlay(nDestFrame);\
            }\
            else\
            {\
               objRoot.lecDoGotoAndStop(nDestFrame);\
            }\
         }\
         \
      "), left, length);
   }
   PrintActionScript(_T("Active Time Line"), scriptPressed);

   btnActive->addAction(new SWFAction(GetCharFromCString(scriptPressed)), SWFBUTTON_MOUSEDOWN);

   SWFDisplayItem *di;
   di = pSprite->add(btnActive);

   CString csId;
   csId.Format(_T("swfActiveTimeLine"));
   di->setName(GetCharFromCString(csId));
   // For Flash version 6 and higher: useHandCursor works this way
   CString csUseHand = _T("false");
   _stprintf(scriptPressed, _T("\
      %s.useHandCursor = %s;\
   "), csId, csUseHand);
   PrintActionScript(_T("Active Time Line"), scriptPressed);
   pSprite->add(new SWFAction(GetCharFromCString(scriptPressed)));

   return hr;
}

UINT CFlashEngine::CreateSlidestarActiveVolumeLine(SWFSprite *pSprite, float x, float y, float length, float size, float btnWidth)
{
	HRESULT hr = S_OK;

   // Create a transparent button
   SWFButton *btnActive = new SWFButton();
   SWFShape *shpBtn = new SWFShape();
   SWFFill *fillBtn = shpBtn->addSolidFill(0x00, 0x00, 0x00, 0x00);
   shpBtn->setRightFill(fillBtn);
   DrawRectangle(shpBtn, x, y, length, size);
   btnActive->addShape(shpBtn, SWFBUTTON_HIT );

   float left = x;

   _TCHAR script[1024];
    _stprintf(script, _T("\
         var objRoot = _parent;\
         if (!objRoot.g_bIsVolumeSliding)\
         {\
            if (objRoot.g_bIsSoundOn)\
            {\
               var x = objRoot._xmouse;\
			   x = x - objRoot.swfSprControl._x - %g;\
			   x = Math.floor(100.0 * x ) / (1.0*%g);\
               if (x > 100)\
                  x = 100;\
               objRoot.g_Sound.setVolume(Math.floor(x));\
               objRoot.g_nVolume = Math.floor(x);\
            }\
         }\
      "), left, length, left, length);
   
   PrintActionScript(_T("Active Line"), script);
   btnActive->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEDOWN);
   
   SWFDisplayItem *di;
   di = pSprite->add(btnActive);

   CString csId;
   csId.Format(_T("swfActiveVolumeLine"));
   di->setName(GetCharFromCString(csId));
   // For Flash version 6 and higher: useHandCursor works this way
   CString csUseHand = _T("false");
   _stprintf(script, _T("\
      %s.useHandCursor = %s;\
   "), csId, csUseHand);
   PrintActionScript(_T("Active Volume Line"), script);

   pSprite->add(new SWFAction(GetCharFromCString(script)));

   return hr;
}


UINT CFlashEngine::CreateActiveVolumeLine(SWFSprite *pSprite, float x, float y, float length, float size, 
                                          float btnWidth, bool bIsVertical)
{
   HRESULT hr = S_OK;

   // Create a transparent button
   SWFButton *btnActive = new SWFButton();
   SWFShape *shpBtn = new SWFShape();
   SWFFill *fillBtn = shpBtn->addSolidFill(0x00, 0x00, 0x00, 0x00);
   shpBtn->setRightFill(fillBtn);
   if (bIsVertical)
      DrawRectangle(shpBtn, x, y, size, length);
   else
      DrawRectangle(shpBtn, x, y, length, size);
   btnActive->addShape(shpBtn, SWFBUTTON_HIT);

   float left = x; // - (0.5f*btnWidth);
   float btnHeight = btnWidth;
   float top  = y; // - (0.5f*btnHeight);

   // Add ActionScript
   _TCHAR scriptPressed[1024];
   if (bIsVertical)
   {
      _stprintf(scriptPressed, _T("\
         var objRoot = _parent;\
         if (!objRoot.g_bIsVolumeSliding)\
         {\
            objRoot.swfSprControl.swfSprVolumeSlider.dy = 0.0;\
            if (objRoot.g_bIsSoundOn)\
            {\
               var y = this._ymouse;\
               y = y - %g;\
               y = Math.floor(100.0 * (%g - y) / %g);\
               if (y > 100)\
                  y = 100;\
               objRoot.g_Sound.setVolume(Math.floor(y));\
               objRoot.g_nVolume = Math.floor(y);\
            }\
         }\
      "), top, length, length);
   }
   else
   {
      _stprintf(scriptPressed, _T("\
         var objRoot = _parent;\
         if (!objRoot.g_bIsVolumeSliding)\
         {\
            objRoot.swfSprControl.swfSprVolumeSlider.dx = 0.0;\
            if (objRoot.g_bIsSoundOn)\
            {\
               var x = this._xmouse;\
               x = x - %g;\
               x = Math.floor(100.0 * x / %g);\
               if (x > 100)\
                  x = 100;\
               objRoot.g_Sound.setVolume(Math.floor(x));\
               objRoot.g_nVolume = Math.floor(x);\
            }\
         }\
      "), left, length);
   }
   PrintActionScript(_T("Active Volume Line"), scriptPressed);

   btnActive->addAction(new SWFAction(GetCharFromCString(scriptPressed)), SWFBUTTON_MOUSEDOWN);

   SWFDisplayItem *di;
   di = pSprite->add(btnActive);

   CString csId;
   csId.Format(_T("swfActiveVolumeLine"));
   di->setName(GetCharFromCString(csId));
   // For Flash version 6 and higher: useHandCursor works this way
   CString csUseHand = _T("false");
   _stprintf(scriptPressed, _T("\
      %s.useHandCursor = %s;\
   "), csId, csUseHand);
   PrintActionScript(_T("Active Volume Line"), scriptPressed);

   pSprite->add(new SWFAction(GetCharFromCString(scriptPressed)));

   return hr;
}

UINT CFlashEngine::DrawEvalNote(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   SWFDisplayItem *di = NULL;

   SWFSprite *sprEvalNote = new SWFSprite();

   CString csText;
   csText.LoadString(IDS_EVAL_NOTE);

   SWFText *pSwfText = new SWFText();
   SWFFont *pSwfFont = m_pFntStd;
   // Define semi-transparent red color
   Gdiplus::ARGB crFontColor = DWORD((0x80 << 24) + (0xff << 16) + (0x00 << 8) + 0x00);
   // Determine font size (via Pythagoras)
   float fFontSize = 0.08f * (float)(sqrt((double)(m_iWbInputWidth*m_iWbInputWidth + m_iWbInputHeight*m_iWbInputHeight)));

   pSwfText->moveTo(0.0f, 0.0f);
   pSwfText->setFont(pSwfFont);
   pSwfText->setHeight(fFontSize);

   pSwfText->setColor((crFontColor >> 16) & 0xff, (crFontColor >> 8) & 0xff, (crFontColor >> 0) & 0xff, (crFontColor >> 24) & 0xff);

   char *utfText = LString::TCharToUtf8(csText);
   if (utfText)
   {
      pSwfText->addUTF8String(utfText);
      delete[] utfText;
      utfText = NULL;
   }

   di = sprEvalNote->add(pSwfText);
   sprEvalNote->nextFrame();
   di = movie->add(sprEvalNote);
   di->setDepth(15980);
   di->setName("swfSprEvalNote");
   di->moveTo(0.7f*fFontSize, (float)(m_iWbInputHeight - 0.3f*fFontSize));
   // Determine rotating angle
   float fAngleRad = atan( (double)m_iWbInputHeight / double(m_iWbInputWidth) );
   float fAngleDeg = fAngleRad * 180.0f / PI;
   di->rotate(fAngleDeg);

   return hr;
}

// --------------------------------------------------------------------------
// ActionScripts for Movie, Control Bar, Buttons, Slider, etc.
// --------------------------------------------------------------------------

UINT CFlashEngine::AddActionScriptVariables(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   // Action script: Add some configurable global variables
   AddActionScriptConfigurableVariables(movie);

   // Action script: Add some global variables
   PrintActionScript(_T("Movie"), szGlobalVariables);
   movie->add(new SWFAction(GetCharFromCString(szGlobalVariables)));

   // Action script: Add layout variables
   PrintActionScript(_T("Movie"), szLayoutVariables);
   movie->add(new SWFAction(GetCharFromCString(szLayoutVariables)));

   // Action script: Add layout variables
   PrintActionScript(_T("Movie"), szPageStreamVariables);
   movie->add(new SWFAction(GetCharFromCString(szPageStreamVariables)));

   // Action script: Add preload variables
   PrintActionScript(_T("Movie"), szPreloadVariables);
   movie->add(new SWFAction(GetCharFromCString(szPreloadVariables)));

   // Action script: Add navigation variables
   PrintActionScript(_T("Movie"), szNavigationVariables);
   movie->add(new SWFAction(GetCharFromCString(szNavigationVariables)));
   
   // Action script: Add video and clip variables
   PrintActionScript(_T("Movie"), szVideoAndClipVariables);
   movie->add(new SWFAction(GetCharFromCString(szVideoAndClipVariables)));

   // Action script: Add testing variables
   PrintActionScript(_T("Movie"), szTestingVariables);
   movie->add(new SWFAction(GetCharFromCString(szTestingVariables)));

   // Action script: Add scorm variables
   PrintActionScript(_T("Movie"), szScormVariables);
   movie->add(new SWFAction(GetCharFromCString(szScormVariables)));

   return hr;
}

UINT CFlashEngine::AddActionScriptConfigurableVariables(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   CString csScript;

   // Action script: Initialization of configurable global variables
   csScript.Format(_T("\
      var serverPrePath = \"\";\
      var baseUrl;\
      var streamUrl;\
      var streamStripFlvSuffix;\
      var videoFlvName;\
      var autoStart;\
      \
   "));

   CString csSubScript;
   for (int i = 0; i < m_iNumOfClips; ++i)
   {
      csSubScript.Format(_T("var clip%dFlvName;"), i);
      csScript += csSubScript;
   }

   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   return hr;
}

UINT CFlashEngine::AddActionScriptInitFunctions(SWFMovie *movie, int nFlashVersion, 
                                                bool bHasPages, bool bHasVideo, bool bHasClips)
{
   HRESULT hr = S_OK;

   CString csIsEvaluation = (m_bDrawEvalNote) ? _T("true") : _T("false");
   CString csIsSlidestar = (m_bIsSlidestar && !m_bIsNewLecPlayer) ? _T("true") : _T("false");
   CString csUseFlashInterface = (m_bIsNewLecPlayer) ? _T("true") : _T("false");

   CString csScript;

   // Action script: Initialization of global variables
   csScript.Format(_T("\
      function lecInitGlobalVariables()\
      {\
         g_nFlashVersion = %d;\
         g_bIsEvaluation = %s;\
         \
         g_Sound = new Sound();\
         g_bIsSoundOn = true;\
         g_nVolume = 80;\
		   g_mousePos = 0;\
         g_Sound.setVolume(g_nVolume);\
         g_bIsVolumeSliding = false;\
         \
         g_bIsPreloading = true;\
         g_bIsSwfLoaded = false;\
         g_bIsPlaying = false;\
         g_bWillBePlaying = false;\
         g_bWasPaying = false;\
         g_bIsInteractivePaused = false;\
         g_bIsTimeSliding = false;\
         g_bTimeSlidingPerformed = false;\
         g_bMessageBoxVisible = false;\
         \
         g_bControlIsDisabled = false;\
         \
         g_bIsTooltipOn = false;\
         g_tooltipStartInterval = 0;\
         g_tooltipStopInterval = 0;\
         \
         g_fFrameRate = %f;\
         g_bIsSlidestar = %s;\
         g_bUseFlashInterface = %s;\
         g_bIsExternJumpCall = false;\
         \
         g_nLastStopmarkFrame = -1;\
         g_bIgnoreStopmark = false;\
         \
         g_updateInterval = 0;\
         g_preloadInterval = 0;\
         g_synchronizationInterval = 0;\
         g_videoSynchronizationInterval = 0;\
         g_clipSynchronizationInterval = 0;\
         g_startDelayedPlayInterval = 0;\
         g_fSynchronizationDelta = 0.125;\
         g_fVideoSynchronizationDelta = 1.0;\
         g_fClipSynchronizationDelta = %f;\
         \
		 g_bVideoClipsLoaded = false;\
      }"), nFlashVersion, csIsEvaluation, m_fFrameRate, csIsSlidestar, csUseFlashInterface, m_fClipSynchronisationDelta);
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   // Action script: Preloader functions
   CString csPreload;
   csPreload.LoadString(IDS_FLASH_PRELOAD);
   CString csPreloadSlidestar;
   csPreloadSlidestar.LoadString(IDS_FLASH_PRELOAD_SLIDESTAR);
   CString csSlide;
   csSlide.LoadString(IDS_FLASH_PAGE); //IDS_FLASH_SLIDE
   CString csOf;
   csOf.LoadString(IDS_FLASH_OF);
   CString csWarning;
   csWarning.LoadString(IDS_WARNING);
   CString csError;
   csError.LoadString(IDS_ERROR);
   CString csFlvNotFoundError;
   csFlvNotFoundError.LoadString(IDS_FLV_NOT_FOUND_ERROR);
   CString csFlvStreamingError;
   csFlvStreamingError.LoadString(IDS_FLV_STREAMING_ERROR);
   CString csServerError;
   csServerError.LoadString(IDS_SERVER_ERROR);

   char *utfPreload = LString::TCharToUtf8(csPreload);
   char *utfPreloadSlidestar = LString::TCharToUtf8(csPreloadSlidestar);
   char *utfSlide = LString::TCharToUtf8(csSlide);
   char *utfOf = LString::TCharToUtf8(csOf);
   char *utfWarning = LString::TCharToUtf8(csWarning);
   char *utfError = LString::TCharToUtf8(csError);
   char *utfFlvNotFoundError = LString::TCharToUtf8(csFlvNotFoundError);
   char *utfFlvStreamingError = LString::TCharToUtf8(csFlvStreamingError);
   char *utfServerError = LString::TCharToUtf8(csServerError);

   if (utfPreload && utfSlide && utfOf && utfWarning && utfError 
      && utfFlvNotFoundError && utfFlvStreamingError && utfServerError)
   {
#ifdef _UNICODE
       csScript.Format(_T("\
          function lecInitPreloadVariables()\
          {\
             g_strPreload = \"%S\";\
             g_strPreloadSlidestar = \"         %S\";\
             g_strSlide = \"%S\";\
             g_strOf = \"%S\";\
             g_strWarning = \"%S\";\
             g_strError = \"%S\";\
             g_strFlvNotFoundError = \"%S\";\
             g_strFlvStreamingError = \"%S\";\
             g_strServerError = \"%S\";\
             g_strMsgType =\"\";\
             g_strMsgText =\"\";\
             g_strMsgTypeTemp =\"\";\
             g_strMsgTextTemp =\"\";\
             g_MessageBoxId = null;\
          }\
       "), utfPreload, utfPreloadSlidestar, utfSlide, utfOf, utfWarning, utfError, utfFlvNotFoundError, utfFlvStreamingError, utfServerError);
#else
       csScript.Format(_T("\
          function lecInitPreloadVariables()\
          {\
             g_strPreload = \"%s\";\
             g_strPreloadSlidestar = \"         %s\";\
             g_strSlide = \"%s\";\
             g_strOf = \"%s\";\
             g_strWarning = \"%s\";\
             g_strError = \"%s\";\
             g_strFlvNotFoundError = \"%s\";\
             g_strFlvStreamingError = \"%s\";\
             g_strServerError = \"%s\";\
             g_strMsgType =\"\";\
             g_strMsgText =\"\";\
             g_strMsgTypeTemp =\"\";\
             g_strMsgTextTemp =\"\";\
             g_MessageBoxId = null;\
          }\
       "), utfPreload, utfPreloadSlidestar, utfSlide, utfOf, utfWarning, utfError, utfFlvNotFoundError, utfFlvStreamingError, utfServerError);
#endif //_UNICODE
       delete[] utfPreload; utfPreload = NULL;
       delete[] utfPreloadSlidestar; utfPreloadSlidestar = NULL;
       delete[] utfSlide; utfSlide = NULL;
       delete[] utfOf; utfOf = NULL;
       delete[] utfWarning; utfWarning = NULL;
       delete[] utfError; utfError = NULL;
       delete[] utfFlvNotFoundError; utfFlvNotFoundError = NULL;
       delete[] utfFlvStreamingError; utfFlvStreamingError = NULL;
       delete[] utfServerError; utfServerError = NULL;
   }
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   // Navigation variables
   CString csAutostartMode;
   csAutostartMode.Format( (m_bAutostartMode ? _T("true") : _T("false")) );
   
   // BUGFIX BLECPUB-1863: No autostart mode in case of new Flex Player 
   // - autostart is handled by the new Flex Player itself.
   if (m_bIsNewLecPlayer)
       csAutostartMode.Format(_T("false"));

   csScript.Format(_T("\
      function lecInitNavigationVariables()\
      {\
         g_bIsNavigationChecked = false;\
         g_nNavigationControlBar = %d;\
         g_nNavigationStandardButtons = %d;\
         g_nNavigationTimeLine = %d;\
         g_nNavigationTimeDisplay = %d;\
         g_nNavigationDocumentStructure = %d;\
         g_nNavigationPluginContextMenu = %d;\
         g_bIsAutostartMode = %s;\
      }"),
      m_iNavigationControlBar, m_iNavigationStandardButtons, m_iNavigationTimeLine, 
      m_iNavigationTimeDisplay, m_iNavigationDocumentStructure, m_iNavigationPluginContextMenu, 
      csAutostartMode);
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   // Layout variables
   csScript.Format(_T("\
      function lecInitLayoutVariables()\
      {\
         g_dGlobalScale = %f;\
         g_nSlidesWidth = %d;\
         g_nSlidesHeight = %d;\
         g_nSlidesOffsetX = %d;\
         g_nSlidesOffsetY = %d;\
      }"), 
      m_dGlobalScale, m_iWbInputWidth, m_iWbInputHeight, 
      m_rcPages.left, m_rcPages.top);
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   // Page stream variables
   CString csHasPages = (bHasPages ? _T("true") : _T("false"));
   csScript.Format(_T("\
      function lecInitPages()\
      {\
         g_bMovieHasPages = %s;\
         g_nPages = 0;\
         g_aPagingFrames = null;\
         g_aVisitedPages = null;\
      }\
   "), csHasPages);
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   // Video and Clip variables
   CString csHasVideo = (bHasVideo ? _T("true") : _T("false"));
   csScript.Format(_T("\
      function lecInitVideoVariables()\
      {\
         g_bMovieHasVideo = %s;\
         \
         g_VideoStream = null;\
         g_bVideoIsBuffering = false;\
         g_nVideoBufferTries = 0;\
         g_nVideoSeekTries = 0;\
         \
         g_sVideoStreamUrl = null;\
         g_fVideoStreamLengthSec = 0.0;\
         g_bufferingInterval = 0;\
         g_nStartBufferLength = 1;\
         g_nExpandedBufferLength = 10;\
         g_NetConnection = null;\
         g_videoBufferInterval = 0;\
         \
      }"), 
      csHasVideo);
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   CString csHasClips = (bHasClips ? _T("true") : _T("false"));
   int iNumOfClips = (bHasClips ? m_iNumOfClips : 0);
   csScript.Format(_T("\
      function lecInitClipVariables()\
      {\
         g_bMovieHasClips = %s;\
         g_iNumOfClips = %d;\
         \
         g_bClipIsBuffering = false;\
         g_iActiveClipNumber = -1;\
         g_aClipBufferTries = null;\
         g_nClipIndex = 0;\
         g_nLastClipSeekTimeMs = 0;\
         \
         g_CurrentClipStream = null;\
         g_CurrentClipStreamUrl = \"\";\
         g_CurrentClipOffset = 0.0;\
         g_CurrentClipLength = 0.0;\
         g_aClipStreams = null;\
         g_aClipStreamUrls = null;\
         g_aClipBeginTimesSec = null;\
         g_aClipEndTimesSec = null;\
         g_aClipSprites = null;\
         g_aClipBufferInterval = null;\
      }\
   "), csHasClips, iNumOfClips);
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   // Testing variables
   CString csMovieHasTesting = (m_aQuestionnaires.GetSize() > 0) ? _T("true") : _T("false");
   CString csIsRandomTest = _T("false");
   CString csRelativePassPercentage = _T("0");
   CString csRandomQuestionOrder = _T("g_randomQuestionOrder = new Array();\r\n");
   CQuestionnaire *pQuestionnaire = m_aQuestionnaires.GetSize() > 0 ? m_aQuestionnaires[0] : NULL;
   if (pQuestionnaire != NULL) {
       csIsRandomTest = pQuestionnaire->IsRandomTest() ? _T("true") : _T("false");
       if (pQuestionnaire->IsRelative())
         csRelativePassPercentage.Format(_T("%d"), pQuestionnaire->GetPassPoints());
       if (pQuestionnaire->IsRandomTest()) {
           for (int i = 0; i < pQuestionnaire->GetNumberOfRandomQuestions(); ++i) {
               CString csLine(_T(""));
               csLine.Format(_T("g_randomQuestionOrder[%d] = 0;"), i);
               csRandomQuestionOrder += csLine;
           }
       }
   }
  
   csScript.Format(_T("\
      function lecInitTestingVariables()\
      {\
         g_bHasTesting = %s;\
         g_isRandomTest = %s;\
         g_relativePassPercentage = %s;\
         g_firstQuestionTimestamp = -1;\
         %s\
         g_JumpRandomFrame = -1;\
         g_nPreviousFrame = -1;\
         g_isRandomInitialized = false;\
         g_doScInitRandom = true;\
         g_bFeedbackMessageVisible = false;\
         g_nFeedbackIndex = -1;\
         g_nFeedbackIndexTemp = -1;\
         g_nFeedbackTestingIndex = -1;\
         g_nQuestionnaireIndex = -1;\
         g_bIsQuestionnaire = false;\
         g_bIsQuestionnaireTemp = false;\
         g_bAllQuestionsAnswered = false;\
         g_bQuestionIsDeactivated = false;\
         g_timeoutInterval = 0;\
         g_FeedbackId = null;\
         g_QuestionnaireId = null;\
         g_nTotalCurrentlyAchievePoints = 0;\
         g_interactionType = \"\";\
         g_questionFillIndex = 0;\
         g_questionDragIndex = 0;\
         g_Responses = \"ABCDEFGHIJKLMNOPRS\";")
         _T("vector = new Array();\
         count = 0;\
         g_interactionLearnerResponse = '';\
         g_interactionPattern = '';\
         g_questionFillIndex = 0;\
         g_bDoLoadLearnerData = false;\
         g_questionIndex = -1;\
         g_learnerResponse = '';\
         g_questionResult = '';\
         g_currPage = -1;\
         closeFeedbackMessage();\
         if(g_isRandomTest)\
            randomTestingInit();\
         if (g_bHasTesting)\
            g_timeoutInterval = setInterval(checkForTimeout, 1000);\
       }"), csMovieHasTesting, csIsRandomTest, csRelativePassPercentage, csRandomQuestionOrder);

   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));
   
   // Testing variables
   csScript.Format(_T("\
      function lecInitScormVariables()\
      {\
         g_bScormEnabled = %s;\
         \
         g_ScopeStrSeparator = \"<,>\";\
         g_MajorStrSeparator = \"{,}\";\
         g_MinorStrSeparator = \"[,]\";\
         \
         g_suspendData = '';\
         g_bDoLoadSuspendData = false;\
         g_bDoSaveSuspendData = false;\
         g_bDoSaveSuccessStatus = false;\
         g_bDoSaveCompletionStatus = false;\
         g_bDoLoadLocationData = false;\
         g_bSuspendDataLoaded = false;\
         g_bLearnerDataLoaded = false;\
         \
         g_nPreviousFeedbackFrame = -1;\
         g_locationTimeStamp = -1;\
         \
         g_resumePos = 0;\
         g_Temp = 0;\
         g_LMSResumeSupported = false;\
		 g_nSessionTime = 0;\
		 g_successStatus = \"failed\";\
		 g_completionStatus = \"incomplete\";\
      }"), GetScormEnabled());
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForLoading(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   CString csScript;
   CString csSlidestarCall = _T("");

   /*if(m_bIsSlidestar)
   {
	   csSlidestarCall = _T("stageResize();\n");
   }*/

   if(m_bIsSlidestar)
   {
   csScript.Format(_T("\
      onEnterFrame = function()\
      {\
         if(!g_bIsAlreadyLoaded)\
         {\
            onLoad();\
            delete onEnterFrame;\
         }\
      };"));
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));
   }

   // Action script: "onLoad": called only once at the beginning
   csScript.Format(_T("\
      onLoad = function()\
      {\
         if (g_bIsAlreadyLoaded == true)\
            return;\
         \
         stop();\
         \
         System.security.allowDomain(\"*\");\
         g_bIsAlreadyLoaded = true;\
         \
         closeMessageBox();\
         swfSprMessageBox.swapDepths(swfSprMessageBox_Dummy);\
         \
         lecInitGlobalVariables();\
         lecInitPreloadVariables();\
         lecInitNavigationVariables();\
         lecInitLayoutVariables();\
         lecInitPages();\
         lecInitVideoVariables();\
         lecInitClipVariables();\
         lecInitScormVariables();\
         lecInitTestingVariables();\
         lecCheckConfigurableVariables();\
         \
         if (g_bMovieHasPages)\
            lecSetPagesParameters();\
         \
         if (g_bScormEnabled == true)\
         {\
            getURL(\"javascript:scutils_Init('\" + g_nTotalMinAchievablePoints + \"','\" + g_nTotalMaxAchievablePoints + \"')\", \"_self\");\
         }\
         \
         if ((g_preloadInterval == 0) && (g_bMovieHasVideo || g_bMovieHasClips))\
            g_preloadInterval = setInterval(lecSetNetParameters, 125);\
         \
         if (g_updateInterval == 0)\
            g_updateInterval = setInterval(lecCheckPlayState, 125);\
         \
         if (!(g_bMovieHasVideo || g_bMovieHasClips))\
         {\
            var goToTimeStamp = 1;\
            if (g_isRandomTest && g_aQuestionEntries[0].nFrameBegin == 1) {\
               goToTimeStamp = g_aQuestionEntries[g_randomQuestionOrder[0]].nFrameBegin;\
            }\
            if (g_bIsAutostartMode)\
               lecDoGotoAndPlay(goToTimeStamp);\
            else\
               lecDoGotoAndStop(goToTimeStamp);\
         } else {\
            if (g_isRandomTest && g_aQuestionEntries[0].nFrameBegin == 1) {\
               lecDoGotoAndStop(g_aQuestionEntries[g_randomQuestionOrder[0]].nFrameBegin);\
            }\
         }\
      };"));
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   // Action script: "onUnload"
   csScript.Format(_T("\
      /*onUnload = function()*/\
      /*{*/\
         /*lecUnloadStreams();*/\
      /*};*/\
      \
      function lecUnloadStreams()\
      {\
         if (g_bMovieHasVideo || g_bMovieHasClips)\
         {\
            if (g_bMovieHasVideo)\
            {\
               g_VideoStream.close();\
            }\
            \
            if (g_bMovieHasClips)\
            {\
               for (var i = 0; i < g_iNumOfClips; ++i)\
                  g_aClipStreams[i].close();\
            }\
            \
            g_NetConnection.close();\
         }\
      }\
      "));
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   CString csScript1;
   CString csScript2;
   csScript1.Format(_T("\
      function lecCheckConfigurableVariables()\
      {\
         if (serverPrePath == eval(\"undefined\"))\
            serverPrePath = \"\";\
         if (baseUrl == eval(\"undefined\"))\
            baseUrl = \"\";\
         if (serverPrePath.length > 0)\
            baseUrl = serverPrePath;\
         if (streamUrl == eval(\"undefined\"))\
            streamUrl = \"\";\
         if (streamStripFlvSuffix == eval(\"undefined\"))\
            streamStripFlvSuffix = \"true\";\
         if (videoFlvName == eval(\"undefined\"))\
            videoFlvName = \"\";\
   "));

   for (int i = 0; i < m_iNumOfClips; ++i)
   {
      CString csSubScript;
      csSubScript.Format(_T("\
         if (clip%dFlvName == undefined)\
            clip%dFlvName = \"\";\
      "), i, i);
      csScript1 += csSubScript;
   }

   csScript2.Format(_T("\
         if (autoStart == eval(\"undefined\"))\
            autoStart = \"\";\
         if (autoStart.length > 0)\
         {\
            if (autoStart == \"true\")\
               g_bIsAutostartMode = true;\
            if (autoStart == \"false\")\
               g_bIsAutostartMode = false;\
         }\
      }\
   "));

   // BUGFIX BLECPUB-1863: No autostart mode in case of new Flex Player 
   // - autostart is handled by the new Flex Player itself.
   if (m_bIsNewLecPlayer)
   csScript2.Format(_T("\
         g_bIsAutostartMode = false;\
      }\
   "));

   csScript = csScript1 + csScript2;
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript.Format(_T("\
      function lecSetNetParameters()\
      {\
         if (!g_bIsSwfLoaded)\
         {\
            return;\
         }\
         \
         clearInterval(g_preloadInterval);\
         g_preloadInterval = 0;\
         \
         lecCheckConfigurableVariables();\
         \
         lecSetNetConnection();\
         \
         if (g_bMovieHasVideo)\
         {\
            lecSetVideoParameters();\
            lecInitVideoStream();\
         }\
         \
         if (g_bMovieHasClips)\
         {\
            lecSetClipParameters();\
            lecInitClipStreams();\
         }\
         \
         if (g_bIsAutostartMode)\
            lecDoGotoAndPlay(1);\
         else\
            lecDoGotoAndStop(1);\
		 g_bVideoClipsLoaded = true;\
      }\
   "));
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));


   return hr;
}

UINT CFlashEngine::AddActionScriptForSlidestar(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   CString csScript;

   csScript.Format(_T("\
      function lecGetMovieWidth()\
      {\
         return %d;\
      }\
      ;"), m_rcMovie.Width());
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript.Format(_T("\
      function lecGetMovieHeight()\
      {\
         return %d;\
      }\
      ;"), m_rcMovie.Height());
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecOnGotoTimemark(timeMs)\
      {\
         g_bIsExternJumpCall = true;\
         \
         var timeSec = (1.0*timeMs / 1000.0);\
         var targetFrame = lecGetFrameFromTimeSec(timeSec);\
         \
         if(g_bIsPlaying)\
            lecOnGotoFrame(targetFrame, \"play\");\
         else\
            lecOnGotoFrame(targetFrame, \"pause\");\
         \
         g_bIsExternJumpCall = false;\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecGetCurPos()\
      {\
         var actualFrame = _currentframe - 1;\
         var msec = actualFrame * Math.floor(1000.0 / (1.0*g_fFrameRate));\
         return msec;\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecGetVideoLoadingPercentage()\
      {\
         var videoPerc = 0;\
         if (g_bMovieHasVideo && g_VideoStream.bytesTotal > 0)\
            videoPerc = (g_VideoStream.bytesLoaded / g_VideoStream.bytesTotal) * 100;\
         return videoPerc;\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecSetAudioVolume(nVolume)\
      {\
         g_Sound.setVolume(nVolume);\
         g_nVolume = nVolume;\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
		  function lecUpdatePreloaderSlidestar()\
		  {\
          var percentLoadedSwf = getBytesLoaded() / (1.0*getBytesTotal());\
          if (percentLoadedSwf >= 1.0)\
            g_bIsSwfLoaded = true;\
          else\
            g_bIsSwfLoaded = false;\
          \
          var allBytesLoaded = 0;\
          var allBytesTotal = 0;\
          allBytesLoaded += getBytesLoaded();\
          allBytesTotal += getBytesTotal();\
          if (g_bMovieHasVideo && !g_bIsTrueStreaming)\
          {\
             if (g_VideoStream.bytesTotal > 0)\
             {\
               allBytesLoaded += g_VideoStream.bytesLoaded;\
               allBytesTotal += g_VideoStream.bytesTotal;\
             }\
             else\
               allBytesTotal += 500000000;\
          }\
          if (g_bMovieHasClips && !g_bIsTrueStreaming)\
          {\
             for (var i = 0; i < g_iNumOfClips; ++i)\
             {\
                if (g_aClipStreams[i].bytesTotal > 0)\
                {\
                  allBytesLoaded += g_aClipStreams[i].bytesLoaded;\
                  allBytesTotal += g_aClipStreams[i].bytesTotal;\
                }\
                else\
                  allBytesTotal += 100000000;\
             }\
          }\
          var percentLoadedAll = allBytesLoaded / (1.0*allBytesTotal);\
          \
			 if (g_bIsSwfLoaded && percentLoadedAll >= 1.0)\
			 {\
				g_bIsPreloading = false;\
			 }\
             else\
			 {\
				g_bIsPreloading = true;\
			 }\
          }\
	   ");
      PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
      movie->add(new SWFAction(GetCharFromCString(csScript)));

   return S_OK;

}


  UINT CFlashEngine::AddActionScriptForSlidestar(SWFMovie *movie, bool bHasPages, bool bHasVideo, bool bHasClips)
{
	HRESULT hr = S_OK;
	CString csScript;
	CString csResizeMethods = _T("");
	// Action script: "SLIDESTAR": called only once at the beginning if SLIDESTAR export
	CArray<CString, CString>aResizeMethods;
	CArray<CString, CString>aResizeMethodsCode;
	CString csClipRepos = _T("");

	if(bHasVideo)
	{
		aResizeMethods.Add(_T("slsResizeVideo();\n"));
		CString csCode;
		//old---swfSprVideo._width *= (Stage.width * g_nVideoWidth)/g_nStageWidth;
		csCode.Format(_T("\
			var g_nVideoWidth = swfSprVideo._width;\
			function slsResizeVideo()\
			{\
				swfSprVideo._width = g_zoomFactor * g_nVideoWidth;\
				swfSprVideo._height = swfSprVideo._width * 3 / 4;\
				/*swfSprVideo._y = ((Stage.height - 15 - swfSprControl._height)/2) - ((swfSprVideo._height)/2);*/\
				g_VideoW = swfSprVideo._width;\
			}"));
		aResizeMethodsCode.Add(csCode);
	}

	if(bHasPages)
	{
		aResizeMethods.Add(_T("slsResizePages();\n"));
		CString csCode;
		//old--- swfSprPages._width = (Stage.width * g_nSlidesWidth)/g_nStageWidth;
		csCode.Format(_T("\
			function slsResizePages()\
			{\
				swfSprPages._width = g_zoomFactor * g_nSlidesWidth;\
				swfSprPages._height = swfSprPages._width * 3 / 4;\
				/*swfSprPages._x = (Stage.width - swfSprPages._width - g_nSlidesOffsetX + g_offX)/ 2 ;*/\
				/*swfSprPages._y = ((Stage.height - 15 - swfSprControl._height)/2) - ((swfSprPages._height)/2);*/\
				g_PageW = swfSprPages._width;\
			}"));
		aResizeMethodsCode.Add(csCode);
	}

	if(bHasClips)
	{
		for(int i = 0; i < m_iNumOfClips; i++)
		{
			CString csClipFunc;
			csClipFunc.Format(_T("slsResizeClip%d();\n"), i);
			aResizeMethods.Add(csClipFunc);
			CString csCode;
			
			csCode.Format(_T("\
				 var g_Clip%dW = swfSprClip%d._width;\
				 var g_Clip%dH = swfSprClip%d._height;\
				 var g_Clip%dR = g_Clip%dH / g_Clip%dW;\
				function slsResizeClip%d()\
				{\
					swfSprClip%d._width = g_Clip%dW * g_zoomFactor;\
					swfSprClip%d._height = swfSprClip%d._width * g_Clip%dR;\
				}\
				"), i, i, i, i, i, i, i, i, i, i, i, i, i);
			
			aResizeMethodsCode.Add(csCode);
		}
	}
	for(int i = 0; i < m_iNumOfClips; i++)
	{
		CString csSubScript;
		csSubScript.Format(_T("\
							  if(g_bMovieHasPages)\
							  {\
							  swfSprClip%d._x = swfSprPages._x + (swfSprPages._width - swfSprClip%d._width)/2 ;\
							  }\
							  else\
							  {\
								if(g_bMovieHasVideo)\
								{\
									swfSprClip%d._x = swfSprVideo._x + swfSprVideo._width + 30 * g_zoomFactor ;\
								}\
								else\
								{\
								}\
							  }\
							  swfSprClip%d._y = ((Stage.height - 15 - swfSprControl._height)/2) - ((swfSprClip%d._height)/2);\
							  \
							  "), i, i, i, i, i);
		csClipRepos += csSubScript;
	}

	aResizeMethods.Add(_T("slsReposElements();\n"));
	CString csCode;
		//old---swfSprVideo._width *= (Stage.width * g_nVideoWidth)/g_nStageWidth;
		csCode.Format(_T("\
			function slsReposElements()\
			{\
				if(g_bMovieHasVideo)\
				{\
					var space = 0;\
					if(g_bMovieHasPages)\
						space = 40 * g_zoomFactor;\
					swfSprVideo._x = (Stage.width - g_VideoW - space - g_PageW) / 2;\
				    swfSprVideo._y = ((Stage.height - 15 - swfSprControl._height)/2) - ((swfSprVideo._height)/2);\
				}\
				if(g_bMovieHasPages)\
				{\
					var space = 0;\
					if(g_bMovieHasVideo)\
						space = 40 * g_zoomFactor;\
					swfSprPages._x = (Stage.width - g_VideoW - space - g_PageW) / 2 + g_VideoW + space;\
				    swfSprPages._y = ((Stage.height - 15 - swfSprControl._height)/2) - ((swfSprPages._height)/2);\
				}\
				if(g_bMovieHasClips)\
				{\
					%s\
				}\
			}"), csClipRepos);
		aResizeMethodsCode.Add(csCode);

	for(int i = 0; i < aResizeMethods.GetCount(); i++)
	{
		csResizeMethods += aResizeMethods.GetAt(i);
	}
   csScript.Format(_T("\
	  var g_nStageWidth = Stage.width;\
	  var g_nStageHeight = Stage.height;\
	  var g_zoomFactor;\
	  var g_VideoW = 0;\
	  var g_PageW = 0;\
	  var g_isScalingAllowed;\
      function stageResize()\
	  {\
	  CalcZoomFactor();\
	  if(g_isScalingAllowed)\
	  {\
	  RepositionControlBar();\
	  %s\
	  }\
	  }\
	  Stage.scaleMode = \"noScale\";\
	  Stage.align = \"TL\";\
	  stageListener = new Object();\
	  stageListener.onResize = function() \
	  {\
		stageResize();\
	  };\
	  Stage.addListener(stageListener);\
	  \
	  function RepositionControlBar()\
	  {\
		swfSprControl._x = (Stage.width - swfSprControl._width)/2;\
		swfSprControl._y = Stage.height - 15 - swfSprControl._height;\
	  }\
	  \
	  function CalcZoomFactor()\
	  {\
			g_isScalingAllowed = true;\
			var offX = Stage.width / g_nStageWidth;\
			var offY = Stage.height / g_nStageHeight;\
			if(offX > offY)\
			{\
				g_zoomFactor = offY;\
			}\
			else\
			{\
				g_zoomFactor = offX;\
			}\
			if(Stage.width < swfSprControl._width)\
				g_isScalingAllowed = false;\
			if(Stage.height < (swfSprControl._height * 3))\
				g_isScalingAllowed = false;\
	  }\
      "), csResizeMethods);
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   for(int i = 0; i < aResizeMethodsCode.GetCount(); i++)
   {
	   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)aResizeMethodsCode.GetAt(i));
		movie->add(new SWFAction(GetCharFromCString(aResizeMethodsCode.GetAt(i))));
   }
	return hr;
}

UINT CFlashEngine::AddActionScriptCommonFunctions(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   AddActionScriptForNavigation(movie);
   
   AddActionScriptForEventHandling(movie);

   AddActionScriptForSynchronization(movie);

   AddActionScriptForDisplayUpdates(movie);

   AddActionScriptForTooltips(movie);
   
   // Action script: Time (sec) <-> Frame
   // (First frame is 1 - not 0)
   CString csScript;
   csScript = _T("\
      function lecGetTimeSecFromFrame(nFrame)\
      {\
         if (nFrame < 1)\
            nFrame = 1;\
         var fTimestamp = (nFrame - 1) / (1.0*g_fFrameRate);\
         if (isNaN(fTimestamp))\
            return 0.0;\
         else\
            return fTimestamp;\
      }\
      \
      function lecGetFrameFromTimeSec(fSeconds)\
      {\
         return (1 + Math.round(g_fFrameRate * fSeconds));\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   
   csScript = _T("\
      function lecCheckForDocumentCompletion()\
      {\
         if(!g_bIsPreloading)\
         {\
            var actualPage = lecGetActualPage();\
            if(g_aVisitedPages[actualPage-1] == \"false\")\
            {\
               g_aVisitedPages[actualPage-1] = \"true\";\
               g_bDoSaveCompletionStatus = true;\
            }")
            _T("\
            if (g_bHasTesting)\
            {\
               var actualFrame = _currentframe;\
               var qIdx = getQuestionIndexFromFrame(actualFrame);\
               if(qIdx >= 0)\
               {\
                  var pageNumber = actualPage;\
                  checkMultipleChoice(pageNumber);\
                  checkDragAndDrop(pageNumber);\
                  checkFillInBlank(pageNumber);\
                  if(g_interactionType == \"singlechoice\" || g_interactionType == \"multiplechoice\")\
                  {\
                     lecAddInteractionChoice(pageNumber);\
                     g_interactionType = 'choice';\
                  }")
                  _T("\
                  else if(g_interactionType == \"fill-in\")\
                     lecAddInteractionFillInBlank(pageNumber);\
                  else if(g_interactionType == \"matching\")\
                     lecAddInteractionDragAndDrop(pageNumber);\
                  \
                  if (g_bScormEnabled && g_aQuestionEntries[qIdx].nQuestionAddedLMS == \"false\")\
                  {\
                     if (g_LMSResumeSupported && g_bLearnerDataLoaded)\
                     {\
                        var rightNow = new Date();\
                        getURL(\"javascript:scutils_addInteraction('\" + qIdx + \"','\" + rightNow + \"','\" + g_aQuestionEntries[qIdx].nAchievablePoints + \"','\" + g_interactionType + \"','\" + g_interactionPattern + \"')\", \"_self\");\
                        g_aQuestionEntries[qIdx].nQuestionAddedLMS = \"true\";\
                     }\
                  }\
               }\
            }\
         }\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecCheckStandardNavigation()\
      {\
         if (!g_bIsNavigationChecked)\
         {\
            if (g_nNavigationControlBar == 0)\
               swfSprControl.enableControlBar(true);\
            else if (g_nNavigationControlBar == 1)\
               swfSprControl.enableControlBar(false);\
            else if (g_nNavigationControlBar == 2)\
               swfSprControl.hideControlBar();\
            if (g_nNavigationStandardButtons == 0)\
               swfSprControl.enableStandardButtons(true);\
            else if (g_nNavigationStandardButtons == 1)\
               swfSprControl.enableStandardButtons(false);\
            else if (g_nNavigationStandardButtons == 2)\
               swfSprControl.hideStandardButtons();")
               _T("\
            if (g_nNavigationTimeLine == 0)\
               swfSprControl.enableTimeLine(true);\
            else if (g_nNavigationTimeLine == 1)\
               swfSprControl.enableTimeLine(false);\
            else if (g_nNavigationTimeLine == 2)\
               swfSprControl.hideTimeLine();\
            if (g_nNavigationTimeDisplay == 0)\
               swfSprControl.enableTimeDisplay(true);\
            else if (g_nNavigationTimeDisplay == 1)\
               swfSprControl.enableTimeDisplay(false);\
            else if (g_nNavigationTimeDisplay == 2)\
               swfSprControl.hideTimeDisplay();\
            if (g_nNavigationDocumentStructure == 0)\
               swfSprControl.enableDocumentStructure(true);\
            else if (g_nNavigationDocumentStructure == 1)\
               swfSprControl.enableDocumentStructure(false);\
            else if (g_nNavigationDocumentStructure == 2)\
               swfSprControl.hideDocumentStructure();\
            \
            g_bIsNavigationChecked = true;\
         }\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecCheckPlayPauseButton()\
      {\
         if (!g_bControlIsDisabled && g_nNavigationStandardButtons == 0)\
         {\
            if (g_bIsPlaying || g_bWillBePlaying)\
            {\
               swfSprControl.swfSprPause._visible = true;\
               swfSprControl.swfSprPlay._visible = false;\
            }\
            else\
            {\
               swfSprControl.swfSprPause._visible = false;\
               swfSprControl.swfSprPlay._visible = true;\
            }\
         }\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   hr = AddActionScriptForTestsToMovie(movie);

   return hr;
}

UINT CFlashEngine::AddActionScriptForNavigation(SWFMovie *movie)
{
   HRESULT hr = S_OK;
   _TCHAR script[4096];

   // action script for seeking in streams
   _stprintf(script, _T("\
      function lecDoStreamSeek(stream, timeSec, bIsPaused)\
      {\
         var seekTime = timeSec;\
         if (seekTime < 0)\
            seekTime = 0;\
         \
         \
         stream.seek(seekTime);\
         \
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));
   
   // Action script: Play/Stop
   _stprintf(script, _T("\
      function lecDoPlay()\
      {\
         var actualTime = lecGetTimeSecFromFrame(_currentframe);\
         \
         if (g_bMovieHasVideo)\
         {\
            /*lecCheckVideoFlvFile(g_VideoStream, g_sVideoStreamUrl);*/\
            g_VideoStream.pause(false);\
         }\
         \
         if (g_CurrentClipStream)\
         {\
            /*lecCheckClipFlvFile(g_CurrentClipStream, g_CurrentClipStreamUrl);*/\
            g_CurrentClipStream.pause(false);\
         }\
         \
         if (g_bMovieHasPages)\
            swfSprPages.play();\
         \
         play();\
         \
         g_bIsPlaying = true;\
         g_bWillBePlaying = false;\
         g_bIsInteractivePaused = false;\
         \
         if (g_synchronizationInterval == 0)\
            g_synchronizationInterval = setInterval(lecSynchronizeReplay, 125);\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));
   
   _stprintf(script, _T("\
      function lecDoStop()\
      {\
         if (g_synchronizationInterval > 0)\
            clearInterval(g_synchronizationInterval);\
         g_synchronizationInterval = 0;\
         \
         if (g_bufferingInterval > 0)\
            clearInterval(g_bufferingInterval);\
         g_bufferingInterval = 0;\
         g_nVideoSeekTries = 0;\
         \
         if (g_startDelayedPlayInterval > 0)\
            clearInterval(g_startDelayedPlayInterval);\
         g_startDelayedPlayInterval = 0;\
         \
         if (g_VideoSynchronizationInterval > 0)\
            clearInterval(g_VideoSynchronizationInterval);\
         g_VideoSynchronizationInterval = 0;\
         \
         if (g_ClipSynchronizationInterval > 0)\
            clearInterval(g_ClipSynchronizationInterval);\
         g_ClipSynchronizationInterval = 0;\
         \
         if (g_bMovieHasVideo)\
         {\
            g_VideoStream.pause(true);\
         }\
         \
         if (g_CurrentClipStream)\
         {\
            g_CurrentClipStream.pause(true);\
         }\
         \
         if (g_bMovieHasPages)\
            swfSprPages.stop();\
         \
         stop();\
         \
         g_bIsPlaying = false;\
         g_bWillBePlaying = false;\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecScrollTo(nFrame)\
      {\
         lecDoStop();\
         \
         var actualTime = lecGetTimeSecFromFrame(nFrame);\
         \
         if (g_bMovieHasVideo)\
         {\
            var nTimeDiff = Math.abs(g_VideoStream.time-actualTime);\
            if (nTimeDiff > g_fVideoSynchronizationDelta)\
            {\
               lecDoStreamSeek(g_VideoStream, actualTime, true);\
            }\
         }\
         if (g_bMovieHasClips)\
         {\
            lecCheckAllClipsStatus(actualTime, true);\
            if (g_CurrentClipStream != null)\
            {\
               var nTimeDiff = Math.abs(g_CurrentClipStream.time-(actualTime - g_CurrentClipOffset));\
               if (nTimeDiff > g_fClipSynchronizationDelta)\
               {\
                  var bCanSeek = true;\
                  var now = new Date();\
                  var currMs = now.getTime();\
                  var deltaTime = currMs - g_nLastClipSeekTimeMs;\
                  if (g_bIsTrueStreaming && (deltaTime < (1000*g_nStartBufferLength)))\
                     bCanSeek = false;\
                  if (bCanSeek)\
                  {\
                     lecDoStreamSeek(g_CurrentClipStream, actualTime - g_CurrentClipOffset, true);\
                     g_nLastClipSeekTimeMs = currMs;\
                  }\
               }\
            }\
         }\
         if (g_bMovieHasPages)\
            swfSprPages.gotoAndStop(nFrame);\
         gotoAndStop(nFrame);\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Action script: Jumping to a new frame (Scrolling)
   _stprintf(script, _T("\
      function lecDoGotoAndPlay(nFrame)\
      {\
         lecDoStop();\
         g_bWillBePlaying = true;\
         \
         lecGotoFrame(nFrame);\
         \
         if (nFrame < _totalframes)\
         {\
            if (g_bIsTrueStreaming && g_bMovieHasVideo)\
            {\
               g_VideoStream.pause(false);\
               g_bufferingInterval = setInterval(lecPlayIfBuffered, 125);\
            }\
            else\
            {\
               if (g_VideoSynchronizationInterval > 0 || g_ClipSynchronizationInterval > 0)\
               {\
                  g_startDelayedPlayInterval = setInterval(lecStartDelayedPlay, 125);\
               }\
               else\
               {\
                  lecDoPlay();\
               }\
            }\
         }\
         else\
            g_bWillBePlaying = false;\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecDoGotoAndStop(nFrame)\
      {\
         lecDoStop();\
         g_bWillBePlaying = false;\
         \
         lecGotoFrame(nFrame);\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecGotoFrame(nFrame)\
      {\
         /* first jump to the correct position in the page and main stream */\
         if (g_bMovieHasPages)\
            swfSprPages.gotoAndStop(nFrame);\
         gotoAndStop(nFrame);\
         \
         var actualTime = lecGetTimeSecFromFrame(nFrame);\
         \
         if (g_bMovieHasVideo)\
         {\
            if (g_bIsTrueStreaming)\
            {\
               lecDoStreamSeek(g_VideoStream, actualTime, true);\
            }\
            else\
            {\
               var nTimeDiff = g_VideoStream.time - actualTime;\
               var absTimeDiff = Math.abs(nTimeDiff);\
               if (absTimeDiff > g_fSynchronizationDelta)\
               {\
                  if (nTimeDiff > 0 || absTimeDiff > g_fVideoSynchronizationDelta)\
                  {\
                     var lowerTime = actualTime - g_fVideoSynchronizationDelta;\
                     if (lowerTime < 0)\
                        lowerTime = 0;\
                     lecDoStreamSeek(g_VideoStream, lowerTime, true);\
                     absTimeDiff = Math.abs(g_VideoStream.time-actualTime);\
                  }\
                  if (absTimeDiff > g_fSynchronizationDelta)\
                  {\
                     g_VideoStream.pause(false);\
                     g_VideoSynchronizationInterval = setInterval(lecForwardVideo, 125);\
                  }\
               }\
            }\
         }\
         if (g_bMovieHasClips)\
         {\
            lecCheckAllClipsStatus(actualTime, false);\
            if (g_bIsTrueStreaming)\
            {\
               if (g_CurrentClipStream != null)\
                  lecDoStreamSeek(g_CurrentClipStream, actualTime - g_CurrentClipOffset, true);\
            }\
            else\
            {\
               if (g_CurrentClipStream != null)\
               {\
                  var nTimeDiff = g_CurrentClipStream.time - (actualTime - g_CurrentClipOffset);\
                  var absTimeDiff = Math.abs(nTimeDiff);\
                  if (absTimeDiff > g_fSynchronizationDelta)\
                  {\
                     if (nTimeDiff > 0 || absTimeDiff > g_fClipSynchronizationDelta)\
                     {\
                        var lowerTime = (actualTime - g_CurrentClipOffset) - g_fClipSynchronizationDelta;\
                        if (lowerTime < 0)\
                           lowerTime = 0;\
                        lecDoStreamSeek(g_CurrentClipStream, lowerTime, true);\
                        var timeDiff = g_CurrentClipStream.time - (actualTime - g_CurrentClipOffset);\
                        if (timeDiff < 0)\
                           absTimeDiff = Math.abs(timeDiff);\
                        else\
                           absTimeDiff = 0.0;\
                     }\
                     if (absTimeDiff > g_fSynchronizationDelta)\
                     {\
                        g_CurrentClipStream.pause(false);\
                        g_ClipSynchronizationInterval = setInterval(lecForwardClip, 125);\
                     }\
                  }\
               }\
            }\
            \
         }\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecForwardVideo()\
      {\
         var actualTime = lecGetTimeSecFromFrame(_currentframe);\
         var minimumTime = actualTime - g_fSynchronizationDelta;\
         if (g_fVideoStreamLengthSec < actualTime || \
             g_VideoStream.time >= minimumTime)\
         {\
            g_VideoStream.pause(true);\
            clearInterval(g_VideoSynchronizationInterval);\
            g_VideoSynchronizationInterval = 0;\
         }\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecForwardClip()\
      {\
         var actualTime = lecGetTimeSecFromFrame(_currentframe);\
         var minimumTime = actualTime - g_fSynchronizationDelta;\
         if ((g_CurrentClipLength + g_CurrentClipOffset) < actualTime || \
             (g_CurrentClipStream.time + g_CurrentClipOffset) >= minimumTime)\
         {\
            g_CurrentClipStream.pause(true);\
            clearInterval(g_ClipSynchronizationInterval);\
            g_ClipSynchronizationInterval = 0;\
         }\
      }\
      "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Note: Special conditions for Slidestar only (<-> Slidestar & New Flex Player)
   bool bIsSlidestar = (m_bIsSlidestar && !m_bIsNewLecPlayer);
   _stprintf(script, _T("\
      function lecPlayIfBuffered()\
      {\
         if(!%d)\
            swfSprControl.lecUpdateBufferingPercentage();\
         var actualTime = lecGetTimeSecFromFrame(_currentframe);\
         lecCheckVideoStatus(actualTime);\
         \
         if(!%d)\
            swfSprControl.lecUpdateTimeDisplay();\
         \
         if (!g_bVideoIsBuffering)\
         {\
            var nTimeDiff = Math.abs(g_VideoStream.time-actualTime);\
            if (g_nVideoSeekTries < 10 && nTimeDiff > g_fVideoSynchronizationDelta)\
            {\
               g_nVideoSeekTries = g_nVideoSeekTries + 1;\
               lecDoStreamSeek(g_VideoStream, actualTime, false);\
            }\
            else\
            {\
               clearInterval(g_bufferingInterval);\
               g_bufferingInterval = 0;\
               g_nVideoSeekTries = 0;\
               lecDoPlay();\
            }\
            \
         }\
      }\
      "), bIsSlidestar, bIsSlidestar);
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecStartDelayedPlay()\
      {\
         if (g_VideoSynchronizationInterval == 0 && g_ClipSynchronizationInterval == 0)\
         {\
            lecDoPlay();\
            clearInterval(g_startDelayedPlayInterval);\
            g_startDelayedPlayInterval = 0;\
         }\
      }\
      "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForEventHandling(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   CString csScript;

   // Action script: Actions I (for interactive objects etc.)
   csScript = _T("\
      function lecOnPlay()\
      {\
         lecDoGotoAndPlay(_currentframe);\
      }\
      \
      function lecOnPause()\
      {\
         lecDoGotoAndStop(_currentframe);\
      }\
      \
      function lecOnStop()\
      {\
         lecDoGotoAndStop(_currentframe);\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecOnBack(playState)\
      {\
         var targetFrame = lecGetPrevPageFrame();\
         \
         lecOnGotoFrame(targetFrame, playState);\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecOnForward(playState)\
      {\
         var targetFrame = lecGetNextPageFrame();\
         \
         lecOnGotoFrame(targetFrame, playState);\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecOnGotoSlide(slideNumber, playState)\
      {\
         var targetFrame = g_aPagingFrames[slideNumber];\
         \
         lecOnGotoFrame(targetFrame, playState);\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   // Action script: Actions II (for interactive objects etc.)
   csScript = _T("\
      function lecOnGotoTargetmark(idTarget, playState)\
      {\
         var targetFrame = g_aTargetmarkFrames[idTarget];\
         \
         lecOnGotoFrame(targetFrame, playState);\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecOnGotoTimestamp(timeMs, playState)\
      {\
         var timeSec = (1.0*timeMs / 1000.0);\
         var targetFrame = lecGetFrameFromTimeSec(timeSec);\
         \
         lecOnGotoFrame(targetFrame, playState);\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));


   csScript = _T("\
      function lecOnGotoFrameR(targetFrame, playState)\
      {\
         g_JumpRandomFrame = targetFrame;\
         lecOnGotoFrame(targetFrame, playState);\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecOnBackR(targetPage, playState)\
      {\
         var targetFrame = 1;\
         if(g_aPagingFrames != null && g_aPagingFrames.length >= targetPage)\
            targetFrame = g_aPagingFrames[targetPage - 1];\
         lecOnGotoFrame(targetFrame, playState);\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecOnGotoQuestion(targetFrame, playState)\
      {\
         if (g_bAllQuestionsAnswered)\
            return;\
         lecOnGotoFrame(g_JumpRandomFrame, \"play\");\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecOnGotoFrame(targetFrame, playState)\
      {\
         /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
         g_bIgnoreStopmark = true;\
         var bTargetFrameIsStopmark = lecSpecialFrameHasStopmark(targetFrame);\
         if (bTargetFrameIsStopmark)\
            g_bIgnoreStopmark = false;\
         var s = new String(playState);\
         \
         if (g_bUseFlashInterface && !g_bIsExternJumpCall)\
         {\
            var jscript = 'javascript:flash_callInteractiveJumpToTimeSec(' + lecGetTimeSecFromFrame(targetFrame) + ')';\
            getURL(jscript, \"_self\");\
         }\
         \
         if (s.indexOf(\"play\") > -1 || s.indexOf(\"pause\") > -1)\
         {\
            if (s.indexOf(\"play\") > -1)\
               lecDoGotoAndPlay(targetFrame);\
            else if (s.indexOf(\"pause\") > -1)\
               lecDoGotoAndStop(targetFrame);\
         }\
         else\
         {\
            if (g_bIgnoreStopmark && (g_bIsPlaying || g_bWillBePlaying || g_bIsInteractivePaused))\
               lecDoGotoAndPlay(targetFrame);\
            else\
               lecDoGotoAndStop(targetFrame);\
         }\
         \
         g_bIsInteractivePaused = false;\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

/*
   csScript = _T("\
      function lecCountTextObjects()\
      {\
         var nTextObjects = 0;\
         var sText = \"\";\
         var obj = swfSprPages;\
         for (i in obj)\
         {\
            if (typeof(obj[i]) == \"string\")\
            {\
               sText += obj[i];\
               sText += \"\n\";\
               nTextObjects++;\
            }\
         }\
         getURL(\"javascript:alert('Objects: \n\" + nTextObjects + \"')\", \"_self\");\
         getURL(\"javascript:alert('Objects: \n\" + sText + \"')\", \"_self\");\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));
*/

   csScript = _T("\
      function lecOnOpenUrl(url, playState)\
      {\
         getUrl(url, \"_blank\");\
         \
         var s = new String(playState);\
         if (s.indexOf(\"play\") > -1)\
            lecOnPlay();\
         if (s.indexOf(\"pause\") > -1)\
            lecOnPause();\
         \
         g_bIsInteractivePaused = false;\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecOnOpenFile(fileName, playState)\
      {\
         getUrl(fileName, \"_blank\");\
         \
         var s = new String(playState);\
         if (s.indexOf(\"play\") > -1)\
            lecOnPlay();\
         if (s.indexOf(\"pause\") > -1)\
            lecOnPause();\
         \
         g_bIsInteractivePaused = false;\
      }\
      \
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   // Action script: Actions III (for interactive objects etc.)
   csScript = _T("\
      function lecOnMute(playState)\
      {\
         if (g_bIsSoundOn)\
         {\
            g_Sound.setVolume(0);\
            g_bIsSoundOn = false;\
         }\
         else\
         {\
            g_Sound.setVolume(g_nVolume);\
            g_bIsSoundOn = true;\
         }\
         \
         var s = new String(playState);\
         if (s.indexOf(\"play\") > -1)\
            lecOnPlay();\
         if (s.indexOf(\"pause\") > -1)\
            lecOnPause();\
         \
         g_bIsInteractivePaused = false;\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecOnMuteOn(playState)\
      {\
         g_Sound.setVolume(0);\
         g_bIsSoundOn = false;\
         \
         var s = new String(playState);\
         if (s.indexOf(\"play\") > -1)\
            lecOnPlay();\
         if (s.indexOf(\"pause\") > -1)\
            lecOnPause();\
         \
         g_bIsInteractivePaused = false;\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecOnMuteOff(playState)\
      {\
         g_Sound.setVolume(g_nVolume);\
         g_bIsSoundOn = true;\
         \
         var s = new String(playState);\
         if (s.indexOf(\"play\") > -1)\
            lecOnPlay();\
         if (s.indexOf(\"pause\") > -1)\
            lecOnPause();\
         \
         g_bIsInteractivePaused = false;\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecOnClose()\
      {\
         lecUnloadStreams();\
         unloadMovie(\"this\");\
         if (g_bUseFlashInterface)\
		      getURL(\"javascript:flash_callClose()\", \"_self\");\
         /*getUrl(\"javascript:top.window.close()\");*/\
      }\
      \
      function lecOnDoNothing()\
      {\
      }\
      \
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   return S_OK;
}

UINT CFlashEngine::AddActionScriptForSynchronization(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   CString csScript;

   csScript = _T("\
      function lecSynchronizeReplay()\
      {\
         if (g_bMovieHasVideo) \
         {\
            lecCheckVideoStatus(g_VideoStream.time);\
            if (!g_bVideoIsBuffering) \
            {\
               var currentFrame = lecGetFrameFromTimeSec(g_VideoStream.time);\
               if (currentFrame <= _totalFrames)\
               {\
                  lecSynchronizeFlashToVideo();\
               }\
               \
               if (g_bMovieHasClips)\
                  lecSynchronizeClipsToVideo();\
            }\
            \
         }\
         else\
         {\
            var currentFrame = swfSprPages._currentframe;\
            if (currentFrame <= _totalFrames)\
            {\
               lecSynchronizePagesToAudio();\
            }\
            if (g_bMovieHasClips)\
               lecSynchronizeClipsToAudio();\
         }\
         \
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecSynchronizeFlashToVideo()\
      {\
         var currentFrame = lecGetFrameFromTimeSec(g_VideoStream.time);\
         var nFrameDiff = Math.abs(currentFrame - _currentframe);\
         if (nFrameDiff > 1)\
         {\
            var previousFrame = _currentframe;\
            if (g_bIsPlaying)\
            {\
               gotoAndPlay(currentFrame);\
               swfSprPages.gotoAndPlay(currentFrame);\
            }\
            else\
            {\
               gotoAndStop(currentFrame);\
               swfSprPages.gotoAndStop(currentFrame);\
            }\
            if (g_bIsTrueStreaming && (g_nStopmarks > 0) && g_bIsPlaying && !g_bIsTimeSliding)\
            {\
               /* Bugfix 3835/4356: Check for stopmark */\
               for (var i = 0; i < g_aStopmarkFrames.length; ++i)\
               {\
                  if ((g_aStopmarkFrames[i] > previousFrame) && (g_aStopmarkFrames[i] <= currentFrame))\
                  {\
                     g_bIgnoreStopmark = false;\
                     g_nLastStopmarkFrame = -1;\
                     lecStopAtStopmarkFrame(g_aStopmarkFrames[i]);\
                     break;\
                  }\
               }\
            }\
         }\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecSynchronizeClipsToVideo()\
      {\
         lecCheckActiveClip(g_VideoStream.time, true);\
         /*lecCheckAllClipsStatus(g_VideoStream.time, true);*/\
         if (g_CurrentClipStream != null)\
         {\
            var nClipTime = g_CurrentClipOffset + g_CurrentClipStream.time;\
            var nTimeDiff = Math.abs(g_VideoStream.time - nClipTime);\
            if (nTimeDiff > g_fClipSynchronizationDelta)\
            {\
               lecDoStreamSeek(g_CurrentClipStream, g_VideoStream.time - g_CurrentClipOffset, false);\
               g_CurrentClipStream.pause(false);\
            }\
         }\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecSynchronizeClipsToAudio()\
      {\
         var nAudioTime = lecGetTimeSecFromFrame(_currentframe);\
         lecCheckActiveClip(nAudioTime, true);\
         /*lecCheckAllClipsStatus(nAudioTime, true);*/\
         if (g_iActiveClipNumber >= 0)\
         {\
            var nClipTime = g_aClipBeginTimesSec[g_iActiveClipNumber] + g_aClipStreams[g_iActiveClipNumber].time;\
            var nTimeDiff = Math.abs(nAudioTime - nClipTime);\
            if (nTimeDiff > g_fClipSynchronizationDelta)\
            {\
               lecDoStreamSeek(g_aClipStreams[g_iActiveClipNumber], nAudioTime - g_aClipBeginTimesSec[g_iActiveClipNumber], false);\
               g_aClipStreams[g_iActiveClipNumber].pause(false);\
            }\
         }\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecSynchronizePagesToAudio()\
      {\
         var currentFrame = swfSprPages._currentframe;\
         var nFrameDiff = Math.abs(currentFrame - _currentframe);\
         if (nFrameDiff > 0)\
         {\
            if (g_bIsPlaying)\
            {\
               swfSprPages.gotoAndPlay(_currentframe);\
            }\
            else\
            {\
               swfSprPages.gotoAndStop(_currentframe);\
            }\
         }\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForDisplayUpdates(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   CString csScript;

   // Note: Special conditions for Slidestar only (<-> Slidestar & New Flex Player)
   bool bIsSlidestar = (m_bIsSlidestar && !m_bIsNewLecPlayer);

   // Part III: methods used for time display update - and the call of the updates
   csScript.Format(_T("\
      function lecCheckPlayState()\
      {\
         if (g_bIsPlaying && (_currentframe >= _totalframes))\
            lecDoGotoAndStop(_totalframes);\
         \
         if (g_bIsPreloading)\
         {\
            if(!%d)\
               swfSprControl.lecUpdatePreloader();\
            else\
               lecUpdatePreloaderSlidestar();\
         }\
         else\
         {\
            if(!%d)\
            {\
               swfSprControl.lecUpdateTimeDisplay();\
               swfSprControl.lecUpdateSlideDisplay();\
            }\
         }\
         \
         lecCheckForDocumentCompletion();\
         if(!%d)\
         {\
            lecCheckForScormSettings();\
            lecCheckStandardNavigation();\
            lecCheckPlayPauseButton();\
         }\
         \
         if (g_bMovieHasClips)\
         {\
            var actualTime = lecGetTimeSecFromFrame(_currentframe);\
            lecCheckActiveClip(actualTime, true);\
         }\
         \
         if ((g_nStopmarks > 0) && g_bIsPlaying && !g_bIsTimeSliding)\
            lecCheckForStopmarks();\
         \
         if (g_isRandomTest && !g_bIsTimeSliding)\
            lecCheckJumpToRandomQuestion();\
         \
         if (g_aInteractionEntries)\
            lecCheckForInteractiveObjects();\
         \
         /*if (g_bHasTesting)*/\
            /*lecCheckForFeedbacks(); TODO CF: ask why this was needed*/\
         \
         if(!%d)\
            swfSprControl.lecCheckSound();\
         \
      }\
   "), m_bIsSlidestar, bIsSlidestar, bIsSlidestar, bIsSlidestar);
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForTooltips(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   CString csScript;

// Action script: Tooltips
   // (the shift of the text about 20% of the text width in the code below is a bit mystical...)
   // Note: difference between Flash version 5 and 6,7,... 
   // in changing a textfield text
   csScript = _T("\
      function lecDoToolTip(displayText, textWidth, position)\
      {\
         _bIsTooltipOn = true;\
         if (g_nFlashVersion > 5 && g_tooltipStartInterval == 0)\
            g_tooltipStartInterval = setInterval(lecStartToolTip, 500, displayText, textWidth, position);\
         else\
            lecStartToolTip(displayText, textWidth, position);\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecStartToolTip(displayText, textWidth, position)\
      {\
         if (!_bIsTooltipOn)\
            return;\
         \
         if (g_tooltipStartInterval > 0)\
            clearInterval(g_tooltipStartInterval);\
         g_tooltipStartInterval = 0;\
         \
         var xOff = 30;\
         var yOff = 10;\
         if (position == \"topleft\")\
         {\
            var xOff = -textWidth-30;\
            var yOff = -20;\
         }\
         else if (position == \"topright\")\
         {\
            var xOff = 30;\
            var yOff = -20;\
         }\
         else if (position == \"bottomleft\")\
         {\
            var xOff = -textWidth-30;\
            var yOff = 10;\
         }\
         else if (position == \"bottomright\")\
         {\
            var xOff = 30;\
            var yOff = 10;\
         }\
         \
         swfSprTooltip.ttTextfield.text = displayText;\
         swfSprTooltip.ttTextfield._width = textWidth;\
         swfSprTooltip._x = _xmouse + xOff;\
         swfSprTooltip._y = _ymouse + yOff;\
         swfSprTooltip._visible = true;\
         \
         if (g_nFlashVersion > 5 && g_tooltipStopInterval == 0)\
            g_tooltipStopInterval = setInterval(lecStopToolTip, 4000);\
      }");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecStopToolTip()\
      {\
         if (g_tooltipStopInterval > 0)\
            clearInterval(g_tooltipStopInterval);\
         g_tooltipStopInterval = 0;\
         \
         swfSprTooltip.ttTextfield.text = \"\";\
         swfSprTooltip._x = -1000;\
         swfSprTooltip._y = -1000;\
         swfSprTooltip._visible = false;\
         _bIsTooltipOn = false;\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForPagingToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   int nNumOfPages = m_aiPagingFrames.GetSize();

   CString csScript;
   CString csSubScript;

   csScript.Format(_T("\
      function lecSetPagesParameters()\
      {\
         swfSprMaskLayer.stop();\
         swfSprMaskLayer._x = swfSprPages._x;\
         swfSprMaskLayer._y = swfSprPages._y;\
         swfSprMaskLayer._width = g_nSlidesWidth;\
         swfSprMaskLayer._height = g_nSlidesHeight;\
         swfSprPages.setMask(swfSprMaskLayer);\
         g_nPages = %d;\
         g_aPagingFrames = new Array(g_nPages);\
         g_aVisitedPages = new Array(g_nPages);\
         for (var i = 0; i < g_aVisitedPages.length; ++i)\
            g_aVisitedPages[i] = \"false\";\
   "), nNumOfPages);

   // Paging frames
   for (int i = 0; i < nNumOfPages; ++i)
   {
      csSubScript.Format(_T("g_aPagingFrames[%d] = %d;"), 
                         i, m_aiPagingFrames.ElementAt(i));
      csScript += csSubScript;
   }
   csScript += _T("}");

   PrintActionScript(_T("Movie"), (_TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecGotoNextPage()\
      {\
         var nNextPageFrame = lecGetNextPageFrame();\
         /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
         g_bIgnoreStopmark = true;\
         var bTargetFrameIsStopmark = lecSpecialFrameHasStopmark(nNextPageFrame);\
         if (bTargetFrameIsStopmark)\
            g_bIgnoreStopmark = false;\
         if (g_bIgnoreStopmark && (g_bIsPlaying || g_bWillBePlaying || g_bIsInteractivePaused))\
         {\
            if (nNextPageFrame == _totalframes)\
               lecDoGotoAndStop(_totalframes);\
            else\
               lecDoGotoAndPlay(nNextPageFrame);\
         }\
         else\
            lecDoGotoAndStop(nNextPageFrame);\
      }");
   PrintActionScript(_T("Movie"), (_TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));
   
   csScript = _T("\
      function lecGotoPrevPage()\
      {\
         var nPrevPageFrame = lecGetPrevPageFrame();\
         /*g_bIgnoreStopmark is resetted in lecCheckForStopmarks*/\
         g_bIgnoreStopmark = true;\
         var bTargetFrameIsStopmark = lecSpecialFrameHasStopmark(nPrevPageFrame);\
         if (bTargetFrameIsStopmark)\
            g_bIgnoreStopmark = false;\
         if (g_bIgnoreStopmark && (g_bIsPlaying || g_bWillBePlaying||g_bIsInteractivePaused))\
         {\
            if (nPrevPageFrame == 1)\
               lecDoGotoAndPlay(1);\
            else\
               lecDoGotoAndPlay(nPrevPageFrame);\
         }\
         else\
            lecDoGotoAndStop(nPrevPageFrame);\
      }");
   PrintActionScript(_T("Movie"), (_TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   // Part I: methods for 'Paging'
   // (First frame is 1 - not 0)
   //    (Most documents didn't start exactly at 0 msec, so a few msec later leads to start at frame 2 - not 1)
   //    (--> Treat frame 2 like frame 1 - bug fix #1731)
   csScript.Format(_T("\
      function lecGetNextPageFrame()\
      {\
         for (var i = 0; i < g_nPages; ++i)\
         {\
            if ((i == 0) && (g_aPagingFrames[0] > _currentframe))\
            {\
               if(g_nPages > 1)\
                  return g_aPagingFrames[1];\
               else\
                  return _totalframes;\
            }\
            if (g_aPagingFrames[i] > _currentframe)\
               return g_aPagingFrames[i];\
         }\
         return _totalframes;\
      }"));
   PrintActionScript(_T("Movie"), (_TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));


   csScript.Format(_T("\
      function lecGetPrevPageFrame()\
      {\
         for (var i = (g_nPages-1); i >= 0; --i)\
         {\
            if ((i == 0) && (g_aPagingFrames[0] < _currentframe))\
               return 1;\
            if (g_aPagingFrames[i] <= _currentframe)\
            {\
               if (i > 0)\
                  return g_aPagingFrames[i-1];\
               else\
                  return g_aPagingFrames[i];\
            }\
         }\
         return 1;\
      }"));
   PrintActionScript(_T("Movie"), (_TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript.Format(_T("\
      function lecGetActualPage()\
      {\
         if (_currentframe < g_aPagingFrames[0])\
            return 1;\
         for (var i = 1; i < g_nPages; ++i)\
         {\
            if ((_currentframe >= g_aPagingFrames[i-1]) && (_currentframe < g_aPagingFrames[i]))\
               return i;\
         }\
         return i;\
      }\
   "));
   PrintActionScript(_T("Movie"), (_TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));
   
   return hr;
}

UINT CFlashEngine::AddActionScriptForTestsToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   CString csScript;

   csScript = _T("\
      function lecCheckForScormSettings()\
      {\
         if(g_bScormEnabled)\
         {\
            if (g_bDoLoadLocationData)\
            {\
				if(g_bMovieHasVideo || g_bMovieHasClips)\
				{\
					if(g_bVideoClipsLoaded)\
					{\
						lecOnGotoTimestamp(g_resumePos, 'pause');\
						g_bDoLoadLocationData = false;\
					}\
				}\
				else\
				{\
					lecOnGotoTimestamp(g_resumePos, 'pause');\
					g_bDoLoadLocationData = false;\
				}\
            }\
            if(g_bIsAlreadyLoaded)\
            {\
               if(g_bDoLoadLearnerData)\
                  LoadLearnerData();\
               \
               if(g_bDoLoadSuspendData)\
                  LoadSuspendData();\
               if(g_bSuspendDataLoaded)\
               {\
                  if(g_isRandomTest && g_doScInitRandom)\
                    randomScTestingInit();\
                  CreateAndSaveSuspendData();\
                  \
                  if(g_bDoSaveSuccessStatus && g_bHasTesting)\
                     UpdateSuccessStatus();\
                  \
                  if(g_bDoSaveCompletionStatus)\
                     UpdateCompletionStatus();\
               }\
            }\
         }\
         \
         g_nSessionTime++;\
         var timeStamp = lecGetTimeSecFromFrame(_currentframe);\
         if(g_Temp >= 8)\
         {\
            if(g_bScormEnabled && g_LMSResumeSupported)\
            {\
               getURL(\"javascript:scutils_SetLocationAndSessionTime()\", \"_self\");\
            }\
            g_Temp = 0;\
         }\
         \
         g_Temp++;\
         if(g_locationTimeStamp == -1)\
         {\
            g_locationTimeStamp = timeStamp;\
         }\
         else\
            if(g_locationTimeStamp != timeStamp)\
            {\
              g_locationTimeStamp = timeStamp;\
            }\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));
   
   csScript = _T("\
      function lecAddInteractionChoice(pageNumber)\
      {\
         if(g_interactionType == \"singlechoice\")\
         {\
            for (var i = 0; i < g_aRadioButtonEntries.length; ++i)\
            {\
               var pageNr = g_aRadioButtonEntries[i].nPage;\
               if (pageNr == pageNumber)\
               {\
                  if(g_aRadioButtonEntries[i].bIsChecked)\
                  {\
                     var idx = g_aRadioButtonEntries[i].nIdx;\
                     g_interactionPattern = g_Responses.substr(idx,1);\
                  }\
               }\
            }\
         }")
         _T("\
         else\
         if(g_interactionType == \"multiplechoice\")\
         {\
            var commaPattern = 0;\
            for (var i = 0; i < g_aCheckBoxEntries.length; ++i)\
            {\
               var pageNr = g_aCheckBoxEntries[i].nPage;\
               if (pageNr == pageNumber)\
               {\
                  if (g_aCheckBoxEntries[i].bIsChecked && commaPattern == 0)\
                  {\
                     var idx = g_aCheckBoxEntries[i].nIdx;\
                     g_interactionPattern = g_Responses.substr(g_aCheckBoxEntries[i].nIdx,1);\
                     commaPattern = 1;\
                  }\
                  else\
                     if (g_aCheckBoxEntries[i].bIsChecked && commaPattern == 1)\
                        g_interactionPattern += '[,]' + g_Responses.substr(g_aCheckBoxEntries[i].nIdx,1);\
               }\
            }\
         }\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecAddInteractionFillInBlank(pageNumber)\
      {\
         var commaPattern = 0;\
         g_interactionPattern = '';\
         for (var i = 0; i < g_aTextFieldEntries.length; ++i)\
         {\
            var pageNr = g_aTextFieldEntries[i].nPage;\
            if (pageNr == pageNumber)\
            {\
               if (commaPattern == 0)\
               {\
                  g_interactionPattern = g_aTextFieldEntries[i].strText;\
                  commaPattern = 1;\
               }\
               else\
                  g_interactionPattern += '[,]' + g_aTextFieldEntries[i].strText;\
            }\
         }\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecAddInteractionDragAndDrop(pageNumber)\
      {\
         var commaPattern = 0;\
         g_interactionPattern = '';\
         var index = 0;\
         var vector = new Array();\
         while(index < g_aTargetPointEntries.length && g_aTargetPointEntries[index].nPage != pageNumber)\
            index++;\
         var count = index;\
         while(count < g_aTargetPointEntries.length && g_aTargetPointEntries[count].nPage == pageNumber)\
            count++;\
         for(var k = index; k < count; k++)\
            vector[k] = g_aTargetPointEntries[k].nX;\
         vector.sort(16);")
         _T("\
         for (var i = 0; i < g_aTargetPointEntries.length; ++i)\
         {\
            var pageNr = g_aTargetPointEntries[i].nPage;\
            if (pageNr == pageNumber)\
            {\
               var j=0;\
               while(j < count && g_aTargetPointEntries[i].nX != vector[j])\
                  j ++;\
               if(commaPattern == 0)\
               {\
                  g_interactionPattern = i +'[.]'+ g_Responses.substr(j,1);\
                  commaPattern = 1;\
               }\
               else\
                  g_interactionPattern += '[,]' + i +'[.]'+ g_Responses.substr(j,1);\
            }\
         }\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));


   csScript = _T("\
      function lecCheckForFeedbacks()\
      {\
         var actualFrame = _currentframe;\
         if ((g_nPreviousFeedbackFrame != actualFrame) && (g_bFeedbackMessageVisible))\
            closeFeedbackMessage();\
         g_nPreviousFeedbackFrame = actualFrame;\
      }\
   ");
   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function randomTestingInit()\
      {\
         if(g_bScormEnabled)\
            return;\
         var randomIndexes = new Array();\
         var totalNumOfQuestions = g_aQuestionEntries.length;\
         var nIx = 0;\
         for (var i = 0; i < totalNumOfQuestions; ++i)\
         {\
            randomIndexes[i] = 0;\
         }\
         var diffRandNrFound = 0;\n\
         while(diffRandNrFound < g_randomQuestionOrder.length) {\
            var randNo = Math.floor(totalNumOfQuestions * Math.random());\
            if(randomIndexes[randNo] == 0) {\
               randomIndexes[randNo] = 1;\
               diffRandNrFound++;\
               g_randomQuestionOrder[nIx++] = randNo;\
            }\
         }\n\
         var firstQuestionTimestamp = g_aQuestionEntries[0].nFrameBegin;\
         g_firstQuestionTimestamp = (firstQuestionTimestamp <= 0) ? 1 : firstQuestionTimestamp;\
         g_JumpRandomFrame = g_aQuestionEntries[g_randomQuestionOrder[0]].nFrameBegin;\
         var nTotalRandomAcheivablePoints = 0;\n\
         for (var j = 0; j < g_randomQuestionOrder.length; ++j) {\
            var qIdx = g_randomQuestionOrder[j];\
            var fbIdxC = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_CORRECT, false);\
            var fbIdxW = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_WRONG, false);\
            var fbIdxT = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_TIME, false);\
            var fbIdxQQC = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_QQ_PASSED, false);\
            var fbIdxQQW = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_QQ_FAILED, false);\
            var nextJumpMs = 0;\
            nTotalRandomAcheivablePoints += g_aQuestionEntries[qIdx].nAchievablePoints;\
            if (j < g_randomQuestionOrder.length - 1) {\
               nextJumpMs = g_aQuestionEntries[g_randomQuestionOrder[j + 1]].nFrameBegin;\
            } else {\
               nextJumpMs = g_aQuestionEntries[g_aQuestionEntries.length - 1].nFrameEnd + 1;\
            }\
            var action = \"lecOnGotoFrameR(\" + nextJumpMs + \",\\\"play\\\")\";\
            g_aFeedbackEntries[fbIdxC].strAction = action;\
            g_aFeedbackEntries[fbIdxW].strAction = action;\
            g_aFeedbackEntries[fbIdxT].strAction = action;\n\
            processEvaluationAction(fbIdxQQC);\
            processEvaluationAction(fbIdxQQW);\
            \
            for(var k = 0; k < g_aChangeTextEntries.length; ++k)\
            {\
               if( (g_aChangeTextEntries[k].nPage == g_aQuestionEntries[qIdx].nPage) && (g_aChangeTextEntries[k].nType == TEXT_TYPE_CHANGE_NUMBER))\
               {\
                   var iIndex = j + 1;\
                   var qNumber = \"\" + iIndex + \"/\" + g_randomQuestionOrder.length;\
                   var strId = g_aChangeTextEntries[k].strId;\
                   var objText = eval ((\"swfSprPages.swfSpr\" + strId) + \".swfTxtChange\");\
                   objText.text = qNumber;\
               }\
            }\
         }\
         if(g_relativePassPercentage != 0) {\n\
            g_nTotalPassAchievablePoints = Math.ceil(g_relativePassPercentage * nTotalRandomAcheivablePoints / 100.0);\
         }\
         g_nTotalMaxAchievablePoints = nTotalRandomAcheivablePoints;\
      }");

   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   //soare begin
   csScript = _T("\
      function randomScTestingInit()\
      {\
          g_doScInitRandom = false;\
          var firstQuestionTimestamp = g_aQuestionEntries[0].nFrameBegin;\
          g_firstQuestionTimestamp = (firstQuestionTimestamp <= 0) ? 1 : firstQuestionTimestamp;\
          if(!g_isRandomInitialized)\
          {\
             var randomIndexes = new Array();\
             var totalNumOfQuestions = g_aQuestionEntries.length;\
             var nIx = 0;\
             for (var i = 0; i < totalNumOfQuestions; ++i)\
             {\
                randomIndexes[i] = 0;\
             }\
             var diffRandNrFound = 0;\n\
             while(diffRandNrFound < g_randomQuestionOrder.length) {\
                var randNo = Math.floor(totalNumOfQuestions * Math.random());\
                if(randomIndexes[randNo] == 0) {\
                   randomIndexes[randNo] = 1;\
                   diffRandNrFound++;\
                   g_randomQuestionOrder[nIx++] = randNo;\
                }\
             }\n\
             g_JumpRandomFrame = g_aQuestionEntries[g_randomQuestionOrder[0]].nFrameBegin;\
         }\
         else\
            g_JumpRandomFrame = g_resumePos;\
         var nTotalRandomAcheivablePoints = 0;\n\
         for (var j = 0; j < g_randomQuestionOrder.length; ++j) {\
            var qIdx = g_randomQuestionOrder[j];\
            var fbIdxC = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_CORRECT, false);\
            var fbIdxW = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_WRONG, false);\
            var fbIdxT = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_TIME, false);\
            var fbIdxQQC = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_QQ_PASSED, false);\
            var fbIdxQQW = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_QQ_FAILED, false);\
            var nextJumpMs = 0;\
            nTotalRandomAcheivablePoints += g_aQuestionEntries[qIdx].nAchievablePoints;\
            if (j < g_randomQuestionOrder.length - 1) {\
               nextJumpMs = g_aQuestionEntries[g_randomQuestionOrder[j + 1]].nFrameBegin;\
            } else {\
               nextJumpMs = g_aQuestionEntries[g_aQuestionEntries.length - 1].nFrameEnd + 1;\
            }\
            var action = \"lecOnGotoFrameR(\" + nextJumpMs + \",\\\"play\\\")\";\
            g_aFeedbackEntries[fbIdxC].strAction = action;\
            g_aFeedbackEntries[fbIdxW].strAction = action;\
            g_aFeedbackEntries[fbIdxT].strAction = action;\n\
            processEvaluationAction(fbIdxQQC);\
            processEvaluationAction(fbIdxQQW);\
            \
            for(var k = 0; k < g_aChangeTextEntries.length; ++k)\
            {\
               if( (g_aChangeTextEntries[k].nPage == g_aQuestionEntries[qIdx].nPage) && (g_aChangeTextEntries[k].nType == TEXT_TYPE_CHANGE_NUMBER))\
               {\
                   var iIndex = j + 1;\
                   var qNumber = \"\" + iIndex + \"/\" + g_randomQuestionOrder.length;\
                   var strId = g_aChangeTextEntries[k].strId;\
                   var objText = eval ((\"swfSprPages.swfSpr\" + strId) + \".swfTxtChange\");\
                   objText.text = qNumber;\
               }\
            }\
         }\
         if(g_relativePassPercentage != 0) {\n\
            g_nTotalPassAchievablePoints = Math.ceil(g_relativePassPercentage * nTotalRandomAcheivablePoints / 100.0);\
         }\
         g_nTotalMaxAchievablePoints = nTotalRandomAcheivablePoints;\
      }");

   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function processEvaluationAction(fbIdx)\
      {\
        var strAction = g_aFeedbackEntries[fbIdx].strAction;\
        var strNewAction = \"\";\
        if(strAction.indexOf(\"OnForward\") != -1 || strAction.indexOf(\"OnPlay\") != -1)\
        {\
            var qqJump = g_aQuestionEntries[g_aQuestionEntries.length - 1].nFrameEnd + 1;\
            if(qqJump >= _totalframes)\
            {\
                strNewAction = \"lecOnPause()\";\
            }\
            else\
            {\
                strNewAction = \"lecOnGotoFrameR(\" + qqJump + \",\\\"play\\\")\";\
            }\
        }\
        else if(strAction.indexOf(\"OnBack\") != -1)\
        {\
            var qqPage = g_aQuestionEntries[0].nPage * 1;\
            if(qqPage <= 1)\
            {\
                strNewAction = \"lecOnPause()\";\
            }\
            else\
            {\
                var iJumpPage = qqPage - 1;\
                strNewAction = \"lecOnBackR(\" + iJumpPage + \",\\\"play\\\")\";\
            }\
        }\
        else if(strAction.indexOf(\"OnDoNothing\") != -1)\
        {\
            strNewAction = \"lecOnPause()\";\
        }\
        else\
        {\
            strNewAction = strAction;\
        }\
        g_aFeedbackEntries[fbIdx].strAction = strNewAction;\
      }");

   PrintActionScript(_T("Movie"), (TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   //soare end
   return hr;
}

UINT CFlashEngine::AddActionScriptForStopmarksToMovie(SWFMovie *movie, int numOfStopmarks, int* anStopmarkFrames)
{
   HRESULT hr = S_OK;

   _TCHAR script[4096];

   _stprintf(script, _T("\
      if (!g_bIsAlreadyLoaded)\
      {\
         g_nLastStopmarkFrame = -1;\
         g_nStopmarks = %d;\
         g_aStopmarkFrames = new Array(%d);\
         g_bIgnoreStopmark = false;\
      }\
   "), numOfStopmarks, numOfStopmarks);
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Stopmark frames
   for (int i = 0; i < numOfStopmarks; ++i)
   {
      // Note: 
      // - first frame is frame 1 (not 0)
      _stprintf(script, _T("\
         if (!g_bIsAlreadyLoaded)\
            g_aStopmarkFrames[%d] = %d;\
      "), i, anStopmarkFrames[i]);
      PrintActionScript(_T("Movie"), script);
      movie->add(new SWFAction(GetCharFromCString(script)));
   }

   _stprintf(script, _T("\
      function lecSpecialFrameHasStopmark(nFrame)\
      {\
         var bStopmarkFound = false;\
         var nFrameStopmark = -1;\
         \
         if (g_nStopmarks > 0)\
         {\
            for (var i = 0; i < %d; ++i)\
            {\
               if (g_aStopmarkFrames[i] == nFrame)\
                  bStopmarkFound = true;\
            }\
         }\
         \
         return bStopmarkFound;\
      }\
   "), numOfStopmarks);
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecCheckForStopmarks()\
      {\
         if (g_nStopmarks <= 0)\
            return;\
         \
         var actualFrame = _currentframe;\
         if (actualFrame < g_nPreviousFrame)\
         {\
            /*g_nLastStopmarkFrame = -1;*/\
            g_nPreviousFrame = actualFrame;\
            return;\
         }\
         \
         var iNextStopmark = %d;\
         for (var i = 0; i < %d; ++i)\
         {\
            if (g_aStopmarkFrames[i] > actualFrame)\
            {\
               iNextStopmark = i;\
               break;\
            }\
         }\
         \
         var bStopmarkFound = false;\
         var iActualStopmarkFrame = -1;\
         if (iNextStopmark > 0)\
         {\
            iActualStopmarkFrame = g_aStopmarkFrames[iNextStopmark-1];\
            \
            if (iActualStopmarkFrame <= actualFrame)\
            {\
               bStopmarkFound = true;\
            }\
         }\
         \
         if (bStopmarkFound)\
         {\
            lecStopAtStopmarkFrame(iActualStopmarkFrame);\
         }\
         else\
         {\
            g_nLastStopmarkFrame = -1;\
         }\
         \
         g_nPreviousFrame = actualFrame;\
         g_bIgnoreStopmark = false;\
      }\
      \
      function lecStopAtStopmarkFrame(iActualStopmarkFrame)\
      {\
         if (g_bIgnoreStopmark)\
         {\
            g_nLastStopmarkFrame = iActualStopmarkFrame;\
         }\
         else if (iActualStopmarkFrame != g_nLastStopmarkFrame)\
         {\
            if (g_bIsPlaying || g_bWillBePlaying)\
            {\
               g_bIsInteractivePaused = true;\
            }\
            if (g_bUseFlashInterface)\
            {\
               var jscript = 'javascript:flash_callStopmarkFoundAtFrame(' + iActualStopmarkFrame + ')';\
               getURL(jscript, \"_self\");\
            }\
            lecDoGotoAndStop(iActualStopmarkFrame);\
            g_bIsPlaying = false;\
            g_nLastStopmarkFrame = iActualStopmarkFrame;\
         }\
      }\
   "), numOfStopmarks, numOfStopmarks);
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecCheckJumpToRandomQuestion()\
      {\
         var actualFrame = _currentframe;\
         if (actualFrame < g_nPreviousFrame)\
         {\
            /*g_nLastStopmarkFrame = -1;*/\
            g_nPreviousFrame = actualFrame;\
            return;\
         }\
         \
         if ( g_isRandomTest \
             && g_firstQuestionTimestamp >= g_nPreviousFrame\
             && g_firstQuestionTimestamp <= actualFrame)\
         {\
             if (g_firstQuestionTimestamp == 1)\
                g_firstQuestionTimestamp = -1;\
            lecOnGotoFrameR(g_JumpRandomFrame, \"play\");\
            \
            if (g_bScormEnabled && g_LMSResumeSupported)\
            {\
               getURL(\"javascript:scutils_saveSuspendData()\", \"_self\");\
            }\
            \
            g_nPreviousFrame = actualFrame;\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForTargetmarksToMovie(SWFMovie *movie, int numOfTargetmarks, int* anTargetmarkFrames)
{
   HRESULT hr = S_OK;

   _TCHAR script[4096];

   _stprintf(script, _T("\
      var g_nTargetmarks;\
      var g_aTargetmarkFrames;\
      if (!g_bIsAlreadyLoaded)\
      {\
         g_nTargetmarks = %d;\
         g_aTargetmarkFrames = new Array(%d);\
      }\
   "), numOfTargetmarks, numOfTargetmarks);
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Targetmark frames
   for (int i = 0; i < numOfTargetmarks; ++i)
   {
      // Note: 
      // - first frame is frame 1 (not 0) --> add one frame here
      _stprintf(script, _T("\
         if (!g_bIsAlreadyLoaded)\
            g_aTargetmarkFrames[%d] = %d;\
      "), i, (anTargetmarkFrames[i] + 1));
      PrintActionScript(_T("Movie"), script);
      movie->add(new SWFAction(GetCharFromCString(script)));
   }

   return hr;
}

UINT CFlashEngine::AddActionScriptForInteractionAreasToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   _TCHAR script[4096];

   // Define classes "InteractionEntry", "RadioButtonEntry", "CheckBoxEntry" and "TextFieldEntry"
   _stprintf(script, _T("\
      function InteractionEntry(strId, bIsButton, bIsTestingButton, nFrameVisibleBegin, nFrameVisibleEnd, nFrameActiveBegin, nFrameActiveEnd)\
      {\
         this.strId = strId;\
         this.bIsButton = bIsButton;\
         this.bIsTestingButton = bIsTestingButton;\
         this.nFrameVisibleBegin = nFrameVisibleBegin;\
         this.nFrameVisibleEnd = nFrameVisibleEnd;\
         this.nFrameActiveBegin = nFrameActiveBegin;\
         this.nFrameActiveEnd = nFrameActiveEnd;\
      }\
      \
      function RadioButtonEntry(strId, nIdx, bIsChecked, nPage, nFrameBegin, nFrameEnd)\
      {\
         this.strId = strId;\
         this.nIdx = nIdx;\
         this.bIsChecked = bIsChecked;\
         this.nPage = nPage;\
         this.nFrameBegin = nFrameBegin;\
         this.nFrameEnd = nFrameEnd;\
      }")
      _T("\
      function CheckBoxEntry(strId, nIdx, bIsChecked, nPage, nFrameBegin, nFrameEnd)\
      {\
         this.strId = strId;\
         this.nIdx = nIdx;\
         this.bIsChecked = bIsChecked;\
         this.nPage = nPage;\
         this.nFrameBegin = nFrameBegin;\
         this.nFrameEnd = nFrameEnd;\
      }\
      \
      function TextFieldEntry(strId, nIdx, strText, nPage, nFrameBegin, nFrameEnd)\
      {\
         this.strId = strId;\
         this.nIdx = nIdx;\
         this.strText = strText;\
         this.nPage = nPage;\
         this.nFrameBegin = nFrameBegin;\
         this.nFrameEnd = nFrameEnd;\
      }")
      _T("\
      var g_aInteractionEntries;\
      var g_aRadioButtonEntries;\
      var g_aCheckBoxEntries;\
      var g_aTextFieldEntries;\
      if (!g_bIsAlreadyLoaded)\
      {\
         g_aInteractionEntries = new Array();\
         g_aRadioButtonEntries = new Array();\
         g_aCheckBoxEntries = new Array();\
         g_aTextFieldEntries = new Array();\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Add "InteractionEntry" and "RadioButtonEntry"/"CheckBoxEntry" objects
   int nPageBefore = -1;
   int nIdx = 0;
   for (int i = 0; (SUCCEEDED(hr)) && i < m_aInteractionAreas.GetSize(); ++i)
   {
      CInteractionArea *pInteractionArea = m_aInteractionAreas.ElementAt(i);
      //_tprintf(_T("Interaction_%d: isBtn:%d isSpt:%d isChk:%d isRdb:%d isTxt:%d\n"), i, pInteractionArea->IsButton(), pInteractionArea->IsSupport(), pInteractionArea->IsCheckBox(), pInteractionArea->IsRadioButton(), pInteractionArea->IsTextField());
      // Get the activity/visibility times
      CArray<CPoint, CPoint> aTimePeriodsActivity;
      CArray<CPoint, CPoint> aTimePeriodsVisibility;
      aTimePeriodsActivity.RemoveAll();
      aTimePeriodsVisibility.RemoveAll();

      if (SUCCEEDED(hr))
         hr = pInteractionArea->GetActivityTimes(&aTimePeriodsActivity);
      if (SUCCEEDED(hr))
         hr = pInteractionArea->GetVisibilityTimes(&aTimePeriodsVisibility);

      if (SUCCEEDED(hr) && !pInteractionArea->IsSupport())
      {
         CString csId;
         csId.Format(_T("Interaction_%d"), i);
         CString csIsButton = (pInteractionArea->IsButton()) ? _T("true") : _T("false");
         CString csIsTestingButton = _T("false");
         if (pInteractionArea->IsButton())
         {
            CString strAction = pInteractionArea->GetMouseClickAction();
            if (strAction.IsEmpty())
               strAction = pInteractionArea->GetMouseDownAction();
            if ( (strAction.Find(_T("check")) == 0) || (strAction.Find(_T("reset")) == 0) )
               csIsTestingButton = _T("true");
         }
         CPoint ptTimePeriodActivity(-1, -1);
         if (aTimePeriodsActivity.GetSize() > 0)
            ptTimePeriodActivity = aTimePeriodsActivity.ElementAt(0);
         CPoint ptTimePeriodVisibility(-1, -1);
         if (aTimePeriodsVisibility.GetSize() > 0)
            ptTimePeriodVisibility = aTimePeriodsVisibility.ElementAt(0);
         // TODO: Get all time periods 
         // <-> but so far only one time period is implemented --> nothing to do here

         int nTimeMsActiveBegin  = ptTimePeriodActivity.x;
         int nTimeMsActiveEnd    = ptTimePeriodActivity.y; 
         int nTimeMsVisibleBegin = ptTimePeriodVisibility.x;
         int nTimeMsVisibleEnd   = ptTimePeriodVisibility.y; 


         int nFrameActiveBegin  = GetFrameFromTimestamp(nTimeMsActiveBegin);
         int nFrameActiveEnd    = GetFrameFromTimestamp(nTimeMsActiveEnd);
         int nFrameVisibleBegin = GetFrameFromTimestamp(nTimeMsVisibleBegin);
         int nFrameVisibleEnd   = GetFrameFromTimestamp(nTimeMsVisibleEnd);

         if (nFrameActiveBegin > nFrameActiveEnd)
             nFrameActiveBegin = nFrameActiveEnd;
         if (nFrameVisibleBegin > nFrameVisibleEnd)
             nFrameVisibleBegin = nFrameVisibleEnd;

         _stprintf(script, _T("\
            if (!g_bIsAlreadyLoaded)\
            {\
               var iacArea = new InteractionEntry(\"%s\", %s, %s, %d, %d, %d, %d);\
               g_aInteractionEntries.push(iacArea);\
            }\
         "), csId, csIsButton, csIsTestingButton, nFrameVisibleBegin, nFrameVisibleEnd, nFrameActiveBegin, nFrameActiveEnd);
         PrintActionScript(_T("Movie"), script);
         movie->add(new SWFAction(GetCharFromCString(script)));

         if ( (pInteractionArea->IsRadioButton()) || (pInteractionArea->IsCheckBox()) || (pInteractionArea->IsTextField()) )
         {
            bool bIsChecked = pInteractionArea->HasToBeChecked();
            CString csIsChecked = bIsChecked ? _T("true") : _T("false");
            int nPage = GetPageFromActivityFrames(nFrameActiveBegin, nFrameActiveEnd);
            if (nPage != nPageBefore)
               nIdx = 0;

            if (pInteractionArea->IsRadioButton())
            {
               _stprintf(script, _T("\
                  if (!g_bIsAlreadyLoaded)\
                  {\
                     var rdbEntry = new RadioButtonEntry(\"%s\", %d, %s, %d, %d, %d);\
                     g_aRadioButtonEntries.push(rdbEntry);\
                  }\
               "), csId, nIdx, csIsChecked, nPage, nFrameActiveBegin, nFrameActiveEnd);
            }
            else if (pInteractionArea->IsCheckBox())
            {
               _stprintf(script, _T("\
                  if (!g_bIsAlreadyLoaded)\
                  {\
                     var chkEntry = new CheckBoxEntry(\"%s\", %d, %s, %d, %d, %d);\
                     g_aCheckBoxEntries.push(chkEntry);\
                  }\
               "), csId, nIdx, csIsChecked, nPage, nFrameActiveBegin, nFrameActiveEnd);
            }
            else if (pInteractionArea->IsTextField())
            {
               // Entry for the "disabled" TextField --> it has no activity/visibility times
               _stprintf(script, _T("\
                  if (!g_bIsAlreadyLoaded)\
                  {\
                     var iacArea = new InteractionEntry(\"%s_Disabled\", %s, %s, %d, %d, %d, %d);\
                     g_aInteractionEntries.push(iacArea);\
                  }\
               "), csId, csIsButton, csIsTestingButton, -1, -1, -1, -1);
               PrintActionScript(_T("Movie"), script);
               movie->add(new SWFAction(GetCharFromCString(script)));

               CString csAnswer = _T("");
               CArray<CString, CString> acsAnswers;
               acsAnswers.RemoveAll();
               hr = pInteractionArea->GetCorrectTexts(&acsAnswers);
               if (acsAnswers.GetSize() > 0)
               {
                  for (int k = 0; k < acsAnswers.GetSize(); ++k)
                  {
                     csAnswer += acsAnswers.ElementAt(k);
                     if (k < (acsAnswers.GetSize()-1))
                        csAnswer += _T(";");
                  }
               }
               // The text string has to be converted into UTF8
               char *utfAnswer = LString::TCharToUtf8(csAnswer);
               if (utfAnswer)
               {
#ifdef _UNICODE
                  _stprintf(script, _T("\
                     if (!g_bIsAlreadyLoaded)\
                     {\
                        var txtEntry = new TextFieldEntry(\"%s\", %d, \"%S\", %d, %d, %d);\
                        g_aTextFieldEntries.push(txtEntry);\
                     }\
                  "), csId, i, utfAnswer, nPage, nFrameActiveBegin, nFrameActiveEnd);
#else
                  _stprintf(script, _T("\
                     if (!g_bIsAlreadyLoaded)\
                     {\
                        var txtEntry = new TextFieldEntry(\"%s\", %d, \"%s\", %d, %d, %d);\
                        g_aTextFieldEntries.push(txtEntry);\
                     }\
                  "), csId, i, utfAnswer, nPage, nFrameActiveBegin, nFrameActiveEnd);
#endif //_UNICODE
                  delete[] utfAnswer;
                  utfAnswer = NULL;
               }
            }
            PrintActionScript(_T("Movie"), script);
            movie->add(new SWFAction(GetCharFromCString(script)));

            nPageBefore = nPage;
            nIdx++;
         }
      }
   }

   // Method: check for interactive objects
   _stprintf(script, _T("\
      function lecCheckForInteractiveObjects()\
      {\
         var actualFrame = _currentframe;\
         if (actualFrame == _totalframes)\
            checkInteractionsAtEnd();\
         \
         var n = g_aInteractionEntries.length;\
         for (var i = 0; i < n; ++i)\
         {\
            var strId = g_aInteractionEntries[i].strId;\
            var bIsButton = g_aInteractionEntries[i].bIsButton;\
            var objNormal = eval(\"swfSprPages.swfSpr\" + strId);\
            var objDisabled = eval(\"swfSprPages.swfSpr\" + strId + \"_Disabled\");\
            var bIsVisible = false;\
            if ((actualFrame >= g_aInteractionEntries[i].nFrameVisibleBegin) && (actualFrame <= g_aInteractionEntries[i].nFrameVisibleEnd))\
               bIsVisible = true;\
            var bIsActive = false;\
            if ((actualFrame >= g_aInteractionEntries[i].nFrameActiveBegin) && (actualFrame <= g_aInteractionEntries[i].nFrameActiveEnd))\
               bIsActive = true;\
            checkInteractiveObjects(objNormal, objDisabled, bIsButton, bIsVisible, bIsActive);\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function checkInteractionsAtEnd()\
      {\
         var nFrame = -1;\
         var bIsStopmarkOnLastFrame = false;\
         var bIsInteractionOnLastFrame = false;\
         var bIsNewPageOnLastFrame = false;\
         \
         if (g_aPagingFrames[g_aPagingFrames.length-1] == _totalframes)\
            bIsNewPageOnLastFrame = true;\
         \
         if (g_nStopmarks > 0)\
         {\
            for (var i = (g_aStopmarkFrames.length-1); i >= 0 ; --i)\
            {\
               if (g_aStopmarkFrames[i] == _totalframes)\
                  bIsStopmarkOnLastFrame = true;\
               if ((!bIsStopmarkOnLastFrame) && (g_aStopmarkFrames[i] == (_totalframes-1)) && (!bIsNewPageOnLastFrame))\
               {\
                  nFrame = (_totalframes-1);\
                  break;\
               }\
            }\
         }\
         \
         if (nFrame < 0)\
         {\
            for (var k = (g_aInteractionEntries.length-1); k >= 0 ; --k)\
            {\
               if (g_aInteractionEntries[k].nFrameActiveEnd == _totalframes)\
                  bIsInteractionOnLastFrame = true;\
               if ((!bIsStopmarkOnLastFrame) && (!bIsInteractionOnLastFrame) && (g_aInteractionEntries[k].nFrameActiveEnd == (_totalframes-1)) && (!bIsNewPageOnLastFrame))\
               {\
                  nFrame = (_totalframes-1);\
                  break;\
               }\
            }\
         }\
         \
         if (nFrame > 0)\
            gotoAndStop(_totalframes-1);\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function checkInteractiveObjects(objNormal, objDisabled, bIsButton, bIsVisible, bIsActive)\
      {\
         if (bIsButton)\
         {\
            if (bIsVisible)\
            {\
               if (bIsActive)\
               {\
                  /*if (objNormal._visible != true)*/\
                  {\
                     objNormal._visible = true;\
                     objDisabled._visible = false;\
                  }\
               }\
               else\
               {\
                  /*if (objNormal._visible != false)*/\
                  {\
                     objNormal._visible = false;\
                     objDisabled._visible = true;\
                  }\
               }\
            }\
            else\
            {\
               objNormal._visible = false;\
               objDisabled._visible = false;\
            }\
         }\
         else\
         {\
            if (bIsActive)\
            {\
               /*if (objNormal._visible != true)*/\
                  objNormal._visible = true;\
            }\
            else\
            {\
               /*if (objNormal._visible != false)*/\
                  objNormal._visible = false;\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Add a mouse listener (necessary for input text fields in SLIDESTAR)
   _stprintf(script, _T("\
      someListener = new Object();\
      someListener.onMouseDown = function()\
      {\
         checkFillInBlankForMouseEvents();\
      };\
      Mouse.addListener(someListener);\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Check FiB for mouse events (necessary for SLIDESTAR)
   _stprintf(script, _T("\
      function checkFillInBlankForMouseEvents()\
      {\
         var actualFrame = this._currentframe;\
         for (var i = 0; i < g_aTextFieldEntries.length; ++i)\
         {\
            var beginFrame = g_aTextFieldEntries[i].nFrameBegin;\
            var endFrame = g_aTextFieldEntries[i].nFrameEnd;\
            if ((actualFrame >= beginFrame) && (actualFrame <= endFrame))\
            {\
               var mx = this._xmouse - g_nSlidesOffsetX;\
               var my = this._ymouse - g_nSlidesOffsetY;\
               var txtObj = eval(\"swfSprPages.swfSpr\" + g_aTextFieldEntries[i].strId);\
               var tx1 = txtObj._x;\
               var ty1 = txtObj._y;\
               var tx2 = txtObj._x + txtObj._width;\
               var ty2 = txtObj._y + txtObj._height;\
               if ((mx >= tx1) && (mx <= tx2) && (my >= ty1) && (my <= ty2))\
               {\
                  Selection.setFocus(txtObj.swfTxtField);\
               }\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));


   return hr;
}

UINT CFlashEngine::AddActionScriptForDragableImagesToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   _TCHAR script[4096];

   // 'Snap-In' for target points: calculated by the maximum size of moveable images
   float fMaxW = 0.0f, fMaxH = 0.0f;
   int i = 0;
   for (i = 0; (SUCCEEDED(hr)) && i < m_aInteractionObjects.GetSize(); ++i)
   {
      DrawSdk::DrawObject *pDrawObject = m_aInteractionObjects.ElementAt(i);
      if ( (pDrawObject->GetType() == DrawSdk::IMAGE) && (pDrawObject->IsDndMoveable()) )
      {
         float w = (float)pDrawObject->GetWidth();
         float h = (float)pDrawObject->GetHeight();
         if (w > fMaxW)
            fMaxW = w;
         if (h > fMaxH)
            fMaxH = h;
      }
   }
   int nRadius = (int)(0.5f + (m_dGlobalScale * (fMaxW + fMaxH) / 5.0f));

   // Add methods, define class "TargetPointEntry"
   _stprintf(script, _T("\
      function TargetPointEntry(nX, nY, nWidth, nHeight, nCenterX, nCenterY, nRadius, strObjId, nPage, nFrameBegin, nFrameEnd)\
      {\
         this.nX = nX;\
         this.nY = nY;\
         this.nWidth = nWidth;\
         this.nHeight = nHeight;\
         this.nCenterX = nCenterX;\
         this.nCenterY = nCenterY;\
         this.nRadius = nRadius;\
         this.strObjId = strObjId;\
         this.nPage = nPage;\
         this.nFrameBegin = nFrameBegin;\
         this.nFrameEnd = nFrameEnd;\
      }\
      \
      var g_aTargetPointEntries;\
      if (!g_bIsAlreadyLoaded)\
         g_aTargetPointEntries = new Array();\
      \
      function getIntersectionIndex(x, y, dX, dY)\
      {\
         var mx = _xmouse - dX;\
         var my = _ymouse - dY;\
         var idx = -1;\
         var actualFrame = _currentframe;\
         for (var i = 0; i < g_aTargetPointEntries.length; ++i)\
         {\
            if ((actualFrame >= g_aTargetPointEntries[i].nFrameBegin) && (actualFrame <= g_aTargetPointEntries[i].nFrameEnd))\
            {\
               var tX1 = g_aTargetPointEntries[i].nX;\
               var tY1 = g_aTargetPointEntries[i].nY;\
               var tX2 = g_aTargetPointEntries[i].nX + g_aTargetPointEntries[i].nWidth;\
               var tY2 = g_aTargetPointEntries[i].nY + g_aTargetPointEntries[i].nHeight;\
               if ((x > tX1) && (x < tX2) && (y > tY1) && (y < tY2) && (mx > tX1) && (mx < tX2) && (my > tY1) && (my < tY2))\
               {\
                  idx = i;\
                  break;\
               }\
            }\
         }\
         return idx;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function inersectionExists(obj1, obj2)\
      {\
         if (obj1 == obj2)\
            return true;\
         var startX1 = obj1._x;\
         var startY1 = obj1._y;\
         var startX2 = obj2._x;\
         var startY2 = obj2._y;\
         var endX1 = obj1._x + obj1._width;\
         var endY1 = obj1._y + obj1._height;\
         var endX2 = obj2._x + obj2._width;\
         var endY2 = obj2._y + obj2._height;\
         var bNoIntersection = endX1 < startX2 || endY1 < startY2 || startX1 > endX2 || startY1 > endY2;\
         return !bNoIntersection;\
      }\
      \
      function targetPointInImageBoundaries(targetPointEntry, imgObj)\
      {\
         var tpX = targetPointEntry.nCenterX;\
         var tpY = targetPointEntry.nCenterY;\
         var startX1 = (imgObj._x * g_dGlobalScale) + g_nSlidesOffsetX;\
         var startY1 = (imgObj._y * g_dGlobalScale) + g_nSlidesOffsetY;\
         var endX1 = startX1 + imgObj._width;\
         var endY1 = startY1 + imgObj._height;\
         var bIsInside = tpX >= startX1 && tpX <= endX1 && tpY >= startY1 && tpY <= endY1;\
         return bIsInside;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function checkPos(obj)\
      {\
         var x = ((obj._x + (0.5 * obj._width)) * g_dGlobalScale) + g_nSlidesOffsetX;\
         var y = ((obj._y + (0.5 * obj._height)) * g_dGlobalScale) + g_nSlidesOffsetY;\
         obj.isSnappedIn = false;\
         obj.isMasked = false;\
         var idx = getIntersectionIndex(x, y, obj.deltaX, obj.deltaY);\
         if (idx > -1)\
         {\
            obj._x = ((g_aTargetPointEntries[idx].nCenterX - g_nSlidesOffsetX) / g_dGlobalScale) - (0.5 * obj._width);\
            obj._y = ((g_aTargetPointEntries[idx].nCenterY - g_nSlidesOffsetY) / g_dGlobalScale) - (0.5 * obj._height);\
            obj.isSnappedIn = true;\
         }\
         for (var i = 0; i < g_aTargetPointEntries.length; ++i)\
         {\
            var actualFrame = _currentframe;\
            if ((actualFrame >= g_aTargetPointEntries[i].nFrameBegin) && (actualFrame <= g_aTargetPointEntries[i].nFrameEnd))\
            {\
               if (g_aTargetPointEntries[i].strObjId != \"\")\
               {\
                  var obj2 = eval(\"swfSprPages.swfSpr\" + g_aTargetPointEntries[i].strObjId);\
                  if (inersectionExists(obj, obj2) && (obj != obj2))\
                     obj.isMasked = true;\
               }\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   int idxDragImg = 0, idxTarget = 0;
   int nTimeMsActiveBegin = -1, nTimeMsActiveEnd = -1;
   int nFrameActiveBegin = -1, nFrameActiveEnd = -1;
   int nPage = -1, nCurrentPage = -1;
   CString csId;
   for (i = 0; (SUCCEEDED(hr)) && i < m_aInteractionAreas.GetSize(); ++i)
   {
      DrawSdk::DrawObject *pDrawObject= m_aInteractionAreas[i]->GetSupportObject();
      if (pDrawObject != NULL)
      {      
         CString csIsButton = _T("false");
         CString csIsTestingButton = _T("false");

         // Get current page of activity (activity = visibility)
         nTimeMsActiveBegin  = m_aInteractionAreas[i]->GetVisibilityStart();
         nTimeMsActiveEnd    = m_aInteractionAreas[i]->GetVisibilityEnd();
         nFrameActiveBegin  = GetFrameFromTimestamp(nTimeMsActiveBegin);
         nFrameActiveEnd    = GetFrameFromTimestamp(nTimeMsActiveEnd);
         nPage = GetPageFromActivityFrames(nFrameActiveBegin, nFrameActiveEnd);
         if (nPage != nCurrentPage)
            nCurrentPage = nPage;

         if ( (pDrawObject->GetType() == DrawSdk::IMAGE) && (pDrawObject->IsDndMoveable()) )
         {
            // Add "InteractionEntry" objects
            csId.Format(_T("DragImg_%d"), idxDragImg);

            _stprintf(script, _T("\
               if (!g_bIsAlreadyLoaded)\
               {\
                  var iacArea = new InteractionEntry(\"%s\", %s, %s, %d, %d, %d, %d);\
                  g_aInteractionEntries.push(iacArea);\
               }\
            "), csId, csIsButton, csIsTestingButton, 
            nFrameActiveBegin, nFrameActiveEnd, nFrameActiveBegin, nFrameActiveEnd);
            PrintActionScript(_T("Movie"), script);
            movie->add(new SWFAction(GetCharFromCString(script)));

            idxDragImg++;
         }

         if (pDrawObject->GetType() == DrawSdk::TARGET)
         {
            // Add "InteractionEntry" objects
            csId.Format(_T("Target_%d"), idxTarget);

            _stprintf(script, _T("\
               if (!g_bIsAlreadyLoaded)\
               {\
                  var iacArea = new InteractionEntry(\"%s\", %s, %s, %d, %d, %d, %d);\
                  g_aInteractionEntries.push(iacArea);\
               }\
            "), csId, csIsButton, csIsTestingButton, 
            nFrameActiveBegin, nFrameActiveEnd, nFrameActiveBegin, nFrameActiveEnd);
            PrintActionScript(_T("Movie"), script);
            movie->add(new SWFAction(GetCharFromCString(script)));

            // Add "TargetPointEntry" objects
            int nX = (int)((((DrawSdk::TargetArea*)pDrawObject)->GetX() * m_dGlobalScale) + m_rcPages.left);
            int nY = (int)((((DrawSdk::TargetArea*)pDrawObject)->GetY() * m_dGlobalScale) + m_rcPages.top);
            int nWidth = (int)(((DrawSdk::TargetArea*)pDrawObject)->GetWidth() * m_dGlobalScale);
            int nHeight = (int)(((DrawSdk::TargetArea*)pDrawObject)->GetHeight() * m_dGlobalScale);
            int nCenterX = (int)((((DrawSdk::TargetArea*)pDrawObject)->GetCenterX() * m_dGlobalScale) + m_rcPages.left);
            int nCenterY = (int)((((DrawSdk::TargetArea*)pDrawObject)->GetCenterY() * m_dGlobalScale) + m_rcPages.top);
            int nPage = GetPageFromActivityFrames(nFrameActiveBegin, nFrameActiveEnd);
            CString csObjId = _T("");

            // Get the corresponding image
            int iBelongsKey = pDrawObject->GetBelongsKey();
            int idxTemp = 0;
            for (int k = 0; k < m_aInteractionObjects.GetSize(); ++k)
            {
               DrawSdk::DrawObject *pTempObject = m_aInteractionObjects.ElementAt(k);
               if ( (pTempObject->GetType() == DrawSdk::IMAGE) && (pTempObject->IsDndMoveable()) )
               {
                  // Get current page of activity (activity = visibility)
                  int iPage = GetPageFromInteractionObject(pTempObject);

                  int iTempBelongsKey = pTempObject->GetBelongsKey();
                  if ( (iPage == nCurrentPage) && (iBelongsKey == iTempBelongsKey) )
                  {
                     csObjId.Format(_T("DragImg_%d"), idxTemp);
                     break;
                  }
                  idxTemp++;
               }
            }
            if (csObjId.IsEmpty())
            {
//               // this should not happen
//               _TCHAR tszMessage[1024];
//               _stprintf(tszMessage, "Error: Target point does not belong to a moveable image!\n");
//               _tprintf(tszMessage);
//               return imc_lecturnity_converter_LPLibs_WHITEBOARD_CORRUPT;
            }

            _stprintf(script, _T("\
               if (!g_bIsAlreadyLoaded)\
               {\
                  var tpe = new TargetPointEntry(%d, %d, %d, %d, %d, %d, %d, \"%s\", %d, %d, %d);\
                  g_aTargetPointEntries.push(tpe);\
               }\
            "), nX, nY, nWidth, nHeight, nCenterX, nCenterY, nRadius, csObjId, nPage, nFrameActiveBegin, nFrameActiveEnd);
            PrintActionScript(_T("Movie"), script);
            movie->add(new SWFAction(GetCharFromCString(script)));

            idxTarget++;
         }
      }
   }

   return hr;
}

UINT CFlashEngine::AddActionScriptForChangeableTextsToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   _TCHAR script[4096];

   // Define class "ChangeTextEntry"
   _stprintf(script, _T("\
      function ChangeTextEntry(nX, nY, nType, strId, nPage, nFrameBegin, nFrameEnd)\
      {\
         this.nX = nX;\
         this.nY = nY;\
         this.nType = nType;\
         this.strId = strId;\
         this.nPage = nPage;\
         this.nFrameBegin = nFrameBegin;\
         this.nFrameEnd = nFrameEnd;\
      }\
      \
      if (!g_bIsAlreadyLoaded)\
         var g_aChangeTextEntries = new Array();\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   int idx = 0;
   int nFrameActiveBegin = -1, nFrameActiveEnd = -1;
   CString csId;
   for (int i = 0; (SUCCEEDED(hr)) && i < m_aInteractionAreas.GetSize(); ++i)
   {
      DrawSdk::DrawObject *pDrawObject= m_aInteractionAreas[i]->GetSupportObject();
      if (pDrawObject != NULL)
      if ((pDrawObject != NULL) && (pDrawObject->GetType() == DrawSdk::TEXT) 
         && ((DrawSdk::Text *)pDrawObject)->HasTextType())
      {
         CQuestionnaire *pQuestionnaire = m_aQuestionnaires.GetSize() > 0 ? m_aQuestionnaires[0] : NULL;
         DrawSdk::TextTypeId idTextType = ((DrawSdk::Text *)pDrawObject)->GetTextType();
         boolean bIsChangeableText = ((idTextType == DrawSdk::TEXT_TYPE_CHANGE_TIME) 
                                     || (idTextType == DrawSdk::TEXT_TYPE_CHANGE_TRIES)
                                     || (pQuestionnaire != NULL && pQuestionnaire->IsRandomTest() && idTextType == DrawSdk::TEXT_TYPE_CHANGE_NUMBER) // Exception for random test
                                     ) ? true : false;
         if (bIsChangeableText)
         {
            // Add "InteractionEntry" objects
            csId.Format(_T("ChangeText_%d"), idx);
            CString csIsButton = _T("false");
            CString csIsTestingButton = _T("false");
            // Activity = Visibility:
            int nTimeMsActiveBegin  = m_aInteractionAreas[i]->GetVisibilityStart();
            int nTimeMsActiveEnd    = m_aInteractionAreas[i]->GetVisibilityEnd() ;

            nFrameActiveBegin  = GetFrameFromTimestamp(nTimeMsActiveBegin);
            nFrameActiveEnd    = GetFrameFromTimestamp(nTimeMsActiveEnd);

            _stprintf(script, _T("\
               if (!g_bIsAlreadyLoaded)\
               {\
                  var iacArea = new InteractionEntry(\"%s\", %s, %s, %d, %d, %d, %d);\
                  g_aInteractionEntries.push(iacArea);\
               }\
            "), csId, csIsButton, csIsTestingButton, 
            nFrameActiveBegin, nFrameActiveEnd, nFrameActiveBegin, nFrameActiveEnd);
            PrintActionScript(_T("Movie"), script);
            movie->add(new SWFAction(GetCharFromCString(script)));

            // Add "ChangeTextEntry" objects
            csId.Format(_T("ChangeText_%d"), idx);
            int nX = (int)((pDrawObject->GetX() * m_dGlobalScale) + 0.5f);
            int nY = (int)((pDrawObject->GetY() * m_dGlobalScale) + 0.5f);
            int nType = idTextType;
               int nPage = GetPageFromActivityFrames(nFrameActiveBegin, nFrameActiveEnd);
            _stprintf(script, _T("\
               if (!g_bIsAlreadyLoaded)\
               {\
                  var cte = new ChangeTextEntry(%d, %d, %d, \"%s\", %d, %d, %d);\
                  g_aChangeTextEntries.push(cte);\
               }\
            "), nX, nY, nType, csId, nPage, nFrameActiveBegin, nFrameActiveEnd);
            PrintActionScript(_T("Movie"), script);
            movie->add(new SWFAction(GetCharFromCString(script)));

            idx++;
         }
      }
   }

   return hr;
}

UINT CFlashEngine::AddActionScriptForQuestionTextsToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   _TCHAR script[4096];

   // Define class "QuestionTextEntry"
   _stprintf(script, _T("\
      function QuestionTextEntry(nX, nY, nType, strId, nPage, nFrameBegin, nFrameEnd)\
      {\
         this.nX = nX;\
         this.nY = nY;\
         this.nType = nType;\
         this.strId = strId;\
         this.nPage = nPage;\
         this.nFrameBegin = nFrameBegin;\
         this.nFrameEnd = nFrameEnd;\
      }\
      \
      var g_aQuestionTextEntries;\
      if (!g_bIsAlreadyLoaded)\
         g_aQuestionTextEntries = new Array();\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   int idx = 0;
   int nFrameActiveBegin = -1, nFrameActiveEnd = -1;
   CString csId;
   for (int i = 0; (SUCCEEDED(hr)) && i < m_aInteractionAreas.GetSize(); ++i)
   {
      DrawSdk::DrawObject *pDrawObject= m_aInteractionAreas[i]->GetSupportObject();
      if ((pDrawObject != NULL) && (pDrawObject->GetType() == DrawSdk::TEXT) 
         && ((DrawSdk::Text *)pDrawObject)->HasTextType())
      {
         CQuestionnaire *pQuestionnaire = m_aQuestionnaires.GetSize() > 0 ? m_aQuestionnaires[0] : NULL;
         DrawSdk::TextTypeId idTextType = ((DrawSdk::Text *)pDrawObject)->GetTextType();
         boolean bIsChangeableText = ((idTextType == DrawSdk::TEXT_TYPE_CHANGE_TIME) 
                                     || (idTextType == DrawSdk::TEXT_TYPE_CHANGE_TRIES)
                                     || (pQuestionnaire != NULL && pQuestionnaire->IsRandomTest() && idTextType == DrawSdk::TEXT_TYPE_CHANGE_NUMBER)  // Exception for random test
                                     ) ? true : false;
         boolean bIsFeedbackText = ((idTextType >= DrawSdk::TEXT_TYPE_EVAL_Q_CORRECT) 
                                    && idTextType <= DrawSdk::TEXT_TYPE_EVAL_QQ_REQUIRED_PERCENT) ? true : false;
         if (!bIsChangeableText && !bIsFeedbackText && !(idTextType == DrawSdk::TEXT_TYPE_NOTHING))
         {
            // Add "InteractionEntry" objects
            csId.Format(_T("QuestionText_%d"), idx);
            CString csIsButton = _T("false");
            CString csIsTestingButton = _T("false");
            // Activity = Visibility:
            int nTimeMsActiveBegin  = m_aInteractionAreas[i]->GetVisibilityStart();
            int nTimeMsActiveEnd    = m_aInteractionAreas[i]->GetVisibilityEnd() ;

            nFrameActiveBegin  = GetFrameFromTimestamp(nTimeMsActiveBegin);
            nFrameActiveEnd    = GetFrameFromTimestamp(nTimeMsActiveEnd);

            if ((nFrameActiveBegin > 0) && (nFrameActiveEnd > 0))
            {
               _stprintf(script, _T("\
                  if (!g_bIsAlreadyLoaded)\
                  {\
                     var iacArea = new InteractionEntry(\"%s\", %s, %s, %d, %d, %d, %d);\
                     g_aInteractionEntries.push(iacArea);\
                  }\
               "), csId, csIsButton, csIsTestingButton, 
               nFrameActiveBegin, nFrameActiveEnd, nFrameActiveBegin, nFrameActiveEnd);
               PrintActionScript(_T("Movie"), script);
               movie->add(new SWFAction(GetCharFromCString(script)));

               // Add "QuestionTextEntry" objects
               csId.Format(_T("QuestionText_%d"), idx);
               int nX = (int)((pDrawObject->GetX() * m_dGlobalScale) + 0.5f);
               int nY = (int)((pDrawObject->GetY() * m_dGlobalScale) + 0.5f);
               int nType = idTextType;
                  int nPage = GetPageFromActivityFrames(nFrameActiveBegin, nFrameActiveEnd);
               _stprintf(script, _T("\
                  if (!g_bIsAlreadyLoaded)\
                  {\
                     var qte = new QuestionTextEntry(%d, %d, %d, \"%s\", %d, %d, %d);\
                     g_aQuestionTextEntries.push(qte);\
                  }\
               "), nX, nY, nType, csId, nPage, nFrameActiveBegin, nFrameActiveEnd);
               PrintActionScript(_T("Movie"), script);
               movie->add(new SWFAction(GetCharFromCString(script)));

               idx++;
            }
         }
      }
   }

   return hr;
}

UINT CFlashEngine::AddActionScriptForTestingToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   _TCHAR script[4096];

   CString csIsFeedbackSummaryEnabled;
   csIsFeedbackSummaryEnabled.Format(_T("true"));
   for (int k = 0; SUCCEEDED(hr) && k < m_aQuestionnaires.GetSize(); ++k)
   {
      CQuestionnaire *pQuestionnaire = m_aQuestionnaires.ElementAt(k);
      if (!pQuestionnaire->IsShowEvaluation())
      {
         csIsFeedbackSummaryEnabled.Format(_T("false"));
         break;
      }
   }

   // Constants
   _stprintf(script, _T("\
      var FEEDBACK_TYPE_NOTHING = %d;\
      var FEEDBACK_TYPE_Q_CORRECT = %d;\
      var FEEDBACK_TYPE_Q_WRONG = %d;\
      var FEEDBACK_TYPE_Q_REPEAT = %d;\
      var FEEDBACK_TYPE_Q_TIME = %d;\
      var FEEDBACK_TYPE_Q_TRIES = %d;\
      var FEEDBACK_TYPE_QQ_PASSED = %d;\
      var FEEDBACK_TYPE_QQ_FAILED = %d;\
      var FEEDBACK_TYPE_QQ_EVAL = %d;\
      \
      var TEXT_TYPE_NOTHING = 0;\
      var TEXT_TYPE_CHANGE_TIME = 1;\
      var TEXT_TYPE_CHANGE_TRIES = 2;\
      var TEXT_TYPE_CHANGE_NUMBER = 3;\
      var TEXT_TYPE_EVAL_Q_CORRECT = 4;\
      var TEXT_TYPE_EVAL_Q_POINTS = 5;\
      var TEXT_TYPE_EVAL_QQ_PASSED = 6;\
      var TEXT_TYPE_EVAL_QQ_FAILED = 7;\
      var TEXT_TYPE_EVAL_QQ_ACHIEVED = 8;\
      var TEXT_TYPE_EVAL_QQ_ACHIEVED_PERCENT = 9;\
      var TEXT_TYPE_EVAL_QQ_TOTAL = 10;\
      var TEXT_TYPE_EVAL_QQ_REQUIRED = 11;\
      var TEXT_TYPE_EVAL_QQ_REQUIRED_PERCENT = 12;\
      var TEXT_TYPE_CHANGE_LAST = 13;\
      var TEXT_TYPE_BLANK_SUPPORT = 14;\
      var TEXT_TYPE_RADIO_SUPPORT = 15;\
      var TEXT_TYPE_QUESTION_TITLE = 16;\
      var TEXT_TYPE_QUESTION_TEXT = 17;\
      var TEXT_TYPE_NUMBER_SUPPORT = 18;\
      var TEXT_TYPE_TRIES_SUPPORT = 19;\
      var TEXT_TYPE_TIME_SUPPORT = 20;\
      var TEXT_TYPE_TARGET_SUPPORT = 21;\
      \
      var g_bFeedbackSummaryEnabled = %s;\
   "), FEEDBACK_TYPE_NOTHING, FEEDBACK_TYPE_Q_CORRECT, FEEDBACK_TYPE_Q_WRONG, FEEDBACK_TYPE_Q_REPEAT, 
   FEEDBACK_TYPE_Q_TIME, FEEDBACK_TYPE_Q_TRIES, FEEDBACK_TYPE_QQ_PASSED, FEEDBACK_TYPE_QQ_FAILED, 
   FEEDBACK_TYPE_QQ_EVAL, csIsFeedbackSummaryEnabled);
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Check timeout in Intervals
   _stprintf(script, _T("\
      function checkForTimeout()\
      {\
         var actualFrame = _currentframe;\
         var qIdx = getQuestionIndexFromFrame(actualFrame);\
         if (qIdx >= 0)\
         {\
		 if (g_aQuestionEntries[qIdx].nMaximumAttempts >= 0)\
			updateTriesFromQuestionIndex(qIdx);\
            if (g_aQuestionEntries[qIdx].nTimeoutSec > 0)\
			{\
			   if(!g_aQuestionEntries[qIdx].bIsDeactivated)\
					g_aQuestionEntries[qIdx].nViewedSec += 1;\
               updateTimerFromQuestionIndex(qIdx);\
               if (g_aQuestionEntries[qIdx].nViewedSec > g_aQuestionEntries[qIdx].nTimeoutSec && !g_aQuestionEntries[qIdx].bIsDeactivated)\
               {\
                  /*Deactivate question*/\
                  g_bQuestionIsDeactivated = true;\
                  deactivateQuestion(g_aQuestionEntries[qIdx].nPage);\
                  var qqIdx = g_aQuestionEntries[qIdx].nQuestionnaireIndex;\
                  g_bAllQuestionsAnswered = areAllQuestionsAnswered(qqIdx);\
                  var fbIdx = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_TIME, false);\
                  if (fbIdx >= 0)\
                     openFeedbackMessage(fbIdx, false);\
                  \
                  if (g_bAllQuestionsAnswered)\
                     submitTesting(qIdx);\
               }\
            }\
         }\
         \
         var nPage = getPageFromFrame(actualFrame);\
         if(g_currPage != nPage)\
         {\
            g_bDoSaveSuspendData = true;\
            g_currPage = nPage;\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Deactivate Question
   _stprintf(script, _T("\
      function deactivateQuestion(pageNumber)\
      {\
         var qIdx = getQuestionIndex(pageNumber);\
         if (qIdx >= 0)\
         {\
            deactivateDragAndDrop(pageNumber);\
            deactivateMultipleChoice(pageNumber);\
            deactivateFillInBlank(pageNumber);\
            deactivateTestingButtons(pageNumber);\
            g_aQuestionEntries[qIdx].bIsDeactivated = true;\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Event handling
   _stprintf(script, _T("\
      function onDoResetQuestion(nPage)\
      {\
         /*var actualFrame = _currentframe;*/\
         /*if (g_bIsPlaying)*/\
         /*{*/\
            /*g_bIsInteractivePaused = true;*/\
            /*if (g_bUseFlashInterface)*/\
            /*{*/\
               /*var jscript = 'javascript:flash_callStopmarkFoundAtFrame(' + actualFrame + ')';*/\
               /*getURL(jscript, \"_self\");*/\
            /*}*/\
         /*}*/\
         /*lecOnPause();*/\
         resetDragAndDrop(nPage);\
         resetMultipleChoice(nPage);\
         resetFillInBlank(nPage);\
         \
         var qIdx = getQuestionIndex(nPage);\
         if (qIdx >= 0)\
            g_aQuestionEntries[qIdx].bSuccessfullyAnswered = false;\
         else\
            getURL(\"javascript:alert('Error: Corrupt testing data!')\", \"_self\");\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function onDoSubmitAnswer(nPage)\
      {\
         var actualFrame = _currentframe;\
         if (g_bIsPlaying)\
         {\
            g_bIsInteractivePaused = true;\
            if (g_bUseFlashInterface)\
            {\
               var jscript = 'javascript:flash_callStopmarkFoundAtFrame(' + actualFrame + ')';\
               getURL(jscript, \"_self\");\
            }\
         }\
         lecOnPause();\
         var qIdx = getQuestionIndex(nPage);\
         if (qIdx >= 0)\
         {\
            g_aQuestionEntries[qIdx].nTakenAttempts++;\
            updateTriesFromQuestionIndex(qIdx);\
            var bIsCorrect = false;\
            if (checkDragAndDrop(nPage) == true)\
               bIsCorrect = true;\
            if (checkMultipleChoice(nPage) == true)\
               bIsCorrect = true;\
            if (checkFillInBlank(nPage) == true)\
               bIsCorrect = true;\
            if(g_interactionType == \"singlechoice\")\
            {\
               submitSingleChoiceAnswer(nPage);\
            }\
            else\
               if(g_interactionType == \"multiplechoice\")\
                  submitMultipleChoiceAnswer(nPage);\
            else\
               if(g_interactionType == \"fill-in\")\
                  submitFillInBlankAnswer(nPage);\
            else\
               if(g_interactionType == \"matching\")\
                  sortTargets(nPage);\
            submitAnswer(qIdx, bIsCorrect);\
            if(g_bScormEnabled /*&& g_bSuspendDataLoaded*/)\
            {\
               if(g_LMSResumeSupported && g_bLearnerDataLoaded)\
               {\
                  g_Temp = 0;\
                  submitAnswerLMS(qIdx, bIsCorrect);\
               }\
            }\
            if(bIsCorrect)\
               g_bDoSaveSuccessStatus = true;\
         }\
         else\
            getURL(\"javascript:alert('Error: Corrupt testing data!')\", \"_self\");\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function submitAnswer(qIdx, bIsCorrect)\
      {\
         var qqIdx = g_aQuestionEntries[qIdx].nQuestionnaireIndex;\
         \
         if (bIsCorrect)\
         {\
            g_aQuestionEntries[qIdx].bSuccessfullyAnswered = true;\
            g_nTotalCurrentlyAchievePoints = (1 * g_nTotalCurrentlyAchievePoints) + g_aQuestionEntries[qIdx].nAchievablePoints;\
            /*Deactivate question*/\
            g_bQuestionIsDeactivated = true;\
            deactivateQuestion(g_aQuestionEntries[qIdx].nPage);\
            g_bAllQuestionsAnswered = areAllQuestionsAnswered(qqIdx);\
            var fbIdx = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_CORRECT, false);\
            if (fbIdx >= 0)\
               openFeedbackMessage(fbIdx, false);\
         }\
         else\
         {\
            g_aQuestionEntries[qIdx].bSuccessfullyAnswered = false;\
            if ((g_aQuestionEntries[qIdx].nMaximumAttempts > 0) && (g_aQuestionEntries[qIdx].nTakenAttempts >= g_aQuestionEntries[qIdx].nMaximumAttempts))\
            {\
               if(!g_aQuestionEntries[qIdx].bIsDeactivated)\
               {\
                  /*Deactivate question*/\
                  g_bQuestionIsDeactivated = true;\
                  deactivateQuestion(g_aQuestionEntries[qIdx].nPage);\
                  g_bAllQuestionsAnswered = areAllQuestionsAnswered(qqIdx);\
                  var fbIdx = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_WRONG, false);\
                  if (fbIdx >= 0)\
                     openFeedbackMessage(fbIdx, false);\
               }\
            }\
            else\
            {\
               g_bQuestionIsDeactivated = false;\
               var fbIdx = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_REPEAT, false);\
               if (fbIdx >= 0)\
                  openFeedbackMessage(fbIdx, false);\
            }\
         }\
         if (g_bAllQuestionsAnswered)\
            submitTesting(qIdx);\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));
   ///////////////////////begin us
   _stprintf(script, _T("\
      function submitAnswerLMS(qIdx, bIsCorrect)\
      {\
         var rightNow = new Date();\
         if (bIsCorrect)\
         {\
            getURL(\"javascript:scutils_setInteractionResult('\" + qIdx + \"','correct','\" + g_interactionLearnerResponse + \"','\" + g_nTotalCurrentlyAchievePoints + \"','\" + rightNow + \"')\", \"_self\");\
         }\
         else\
            getURL(\"javascript:scutils_setInteractionResult('\" + qIdx + \"','incorrect','\" + g_interactionLearnerResponse + \"','\" + g_nTotalCurrentlyAchievePoints + \"','\" + rightNow + \"')\", \"_self\");\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   /*************************************************** SUBMIT ANSWER ************************************************/

   _stprintf(script, _T("\
      function submitSingleChoiceAnswer(pageNumber)\
      {\
         for (var i = 0; i < g_aRadioButtonEntries.length; ++i)\
         {\
            var pageNr = g_aRadioButtonEntries[i].nPage;\
            if (pageNr == pageNumber)\
            {\
               var rdbObj = eval(\"swfSprPages.swfSpr\" + g_aRadioButtonEntries[i].strId);\
               if (rdbObj.bIsChecked)\
                  g_interactionLearnerResponse = g_Responses.substr(g_aRadioButtonEntries[i].nIdx,1);\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));
 
   _stprintf(script, _T("\
      function submitMultipleChoiceAnswer(pageNumber)\
      {\
         var commaResponse = 0;\
         for (var i = 0; i < g_aCheckBoxEntries.length; ++i)\
         {\
            var pageNr = g_aCheckBoxEntries[i].nPage;\
            if (pageNr == pageNumber)\
            {\
               var chkObj = eval(\"swfSprPages.swfSpr\" + g_aCheckBoxEntries[i].strId);\
               if (chkObj.bIsChecked && commaResponse == 0)\
               {\
                  g_interactionLearnerResponse = g_Responses.substr(g_aCheckBoxEntries[i].nIdx,1);\
                  commaResponse = 1;\
               }\
               else\
                  if (chkObj.bIsChecked && commaResponse == 1)\
                     g_interactionLearnerResponse += \"[,]\" + g_Responses.substr(g_aCheckBoxEntries[i].nIdx,1);\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));
   
   _stprintf(script, _T("\
      function submitFillInBlankAnswer(pageNumber)\
      {\
         var commaResponse = 0;\
         for (var i = 0; i < g_aTextFieldEntries.length; ++i)\
         {\
            var pageNr = g_aTextFieldEntries[i].nPage;\
            if (pageNr == pageNumber)\
            {\
               var txtObj = eval(\"swfSprPages.swfSpr\" + g_aTextFieldEntries[i].strId + \".swfTxtField\");\
               if (commaResponse == 0)\
               {\
                  g_interactionLearnerResponse = txtObj.text;\
                  commaResponse = 1;\
               }\
               else\
                  g_interactionLearnerResponse += \"[,]\" + txtObj.text;\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));
 

   _stprintf(script, _T("\
      function sortTargets(pageNumber)\
      {\
          var index = 0;\
         while(index < g_aTargetPointEntries.length && g_aTargetPointEntries[index].nPage != pageNumber)\
            index ++;\
         count = index;\
         while(count < g_aTargetPointEntries.length && g_aTargetPointEntries[count].nPage == pageNumber)\
            count ++;\
         for(var k = index; k < count; k++)\
            vector[k] = g_aTargetPointEntries[k].nX;\
         vector.sort(16);\
         submitDragAndDropAnswer(pageNumber);\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));


   _stprintf(script, _T("\
      function submitDragAndDropAnswer(pageNumber)\
      {\
         var commaResponse = 0;\
         for (var i = 0; i < g_aTargetPointEntries.length; ++i)\
         {\
            var pageNr = g_aTargetPointEntries[i].nPage;\
            if ((pageNr == pageNumber) && (g_aTargetPointEntries[i].strObjId != \"\"))\
            {\
               var imgObj = eval(\"swfSprPages.swfSpr\" + g_aTargetPointEntries[i].strObjId);\
               var startX1 = (imgObj._x * g_dGlobalScale) + g_nSlidesOffsetX;\
               var startY1 = (imgObj._y * g_dGlobalScale) + g_nSlidesOffsetY;\
               var endX1 = startX1 + imgObj._width;\
               var endY1 = startY1 + imgObj._height;\
               var j=0;\
               var k=0;\
               var bIsInside = false;\
               imgObj = eval(\"swfSprPages.swfSpr\" + g_aTargetPointEntries[i].strObjId);\
               while(j < g_aTargetPointEntries.length)")
               _T("\
               {\
                  var tpX = g_aTargetPointEntries[j].nCenterX;\
                  var tpY = g_aTargetPointEntries[j].nCenterY;\
                  var bIsInside = tpX >= startX1 && tpX <= endX1 && tpY >= startY1 && tpY <= endY1;\
                  if(bIsInside && commaResponse == 0)\
                  {\
                     while(k < count && g_aTargetPointEntries[j].nX != vector[k])\
                        k ++;\
                     g_interactionLearnerResponse = '' + i +'[.]' + g_Responses.substr(k,1);\
                     commaResponse = 1;\
                     break;\
                  }\
                  else\
                     if(bIsInside && commaResponse == 1)\
                     {\
                        while(k < count && g_aTargetPointEntries[j].nX != vector[k])\
                           k ++;\
                        g_interactionLearnerResponse += '[,]' + i +'[.]' + g_Responses.substr(k,1);\
                        break;\
                     }\
                  else\
                     j ++;\
               }")
               _T("\
               if(!bIsInside && commaResponse == 0)\
               {\
                  g_interactionLearnerResponse = i +'[.]'+ '-';\
                  commaResponse = 1;\
               }\
               else\
               if(!bIsInside && commaResponse == 1)\
                  g_interactionLearnerResponse += '[,]' + i +'[.]'+ '-';\
             }\
          }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));
   ///////////////////////end us
   // Not used so far
   _stprintf(script, _T("\
      function onDoResetQuestionnaire(nPage)\
      {\
         /*(Not implemented so far)*/\
         /*var actualFrame = _currentframe;*/\
         /*if (g_bIsPlaying)*/\
         /*{*/\
            /*g_bIsInteractivePaused = true;*/\
            /*if (g_bUseFlashInterface)*/\
            /*{*/\
               /*var jscript = 'javascript:flash_callStopmarkFoundAtFrame(' + actualFrame + ')';*/\
               /*getURL(jscript, \"_self\");*/\
            /*}*/\
         /*}*/\
         /*lecOnPause();*/\
         var qqIdx = getQuestionnaireIndex(nPage);\
         if (qqIdx >= 0)\
         {\
            var qqPageStart = g_aQuestionnaireEntries[qqIdx].nPageBegin;\
            var qqPageEnd = g_aQuestionnaireEntries[qqIdx].nPageEnd;\
            for (var page = qqPageStart; page <= qqPageEnd; ++page)\
               onDoResetQuestion(page);\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Not used so far
   _stprintf(script, _T("\
      function onDoSubmitTesting(nPage)\
      {\
         /*(Not implemented so far)*/\
         var actualFrame = _currentframe;\
         if (g_bIsPlaying)\
         {\
            g_bIsInteractivePaused = true;\
            if (g_bUseFlashInterface)\
            {\
               var jscript = 'javascript:flash_callStopmarkFoundAtFrame(' + actualFrame + ')';\
               getURL(jscript, \"_self\");\
            }\
         }\
         lecOnPause();\
         var qqIdx = getQuestionnaireIndex(nPage);\
         if (qqIdx >= 0)\
         {\
            var qqPageStart = g_aQuestionnaireEntries[qqIdx].nPageBegin;\
            var qqPageEnd = g_aQuestionnaireEntries[qqIdx].nPageEnd;\
            for (var page = qqPageStart; page <= qqPageEnd; ++page)\
               onDoSubmitAnswer(page);\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Define classes "QuestionnaireEntry"/"QuestionEntry"
   _stprintf(script, _T("\
      function QuestionnaireEntry(nNumOfQuestions, nTotalPoints, nPassPoints, nPageBegin, nPageEnd, nFrameBegin, nFrameEnd)\
      {\
         this.nNumOfQuestions = nNumOfQuestions;\
         this.nTotalPoints = nTotalPoints;\
         this.nPassPoints = nPassPoints;\
         this.nPageBegin = nPageBegin;\
         this.nPageEnd = nPageEnd;\
         this.nFrameBegin = nFrameBegin;\
         this.nFrameEnd = nFrameEnd;\
         g_nTotalMaxAchievablePoints += nTotalPoints;\
         g_nTotalPassAchievablePoints += nPassPoints;\
      }")
      _T("\
      function QuestionEntry(nQuestionnaireIndex, strId, nAchievablePoints, bSussessfullyAnswered, bIsDeactivated, nMaximumAttempts, nTakenAttempts, nTimeoutSec, nViewedSec, nPage, nFrameBegin, nFrameEnd)\
      {\
         this.nQuestionnaireIndex = nQuestionnaireIndex;\
         this.strId = strId;\
         this.nAchievablePoints = nAchievablePoints;\
         this.bSussessfullyAnswered = bSussessfullyAnswered;\
         this.bIsDeactivated = bIsDeactivated;\
         this.nMaximumAttempts = nMaximumAttempts;\
         this.nTakenAttempts = nTakenAttempts;\
         this.nTimeoutSec = nTimeoutSec;\
         this.nViewedSec = nViewedSec;\
         this.nPage = nPage;\
         this.nFrameBegin = nFrameBegin;\
         this.nFrameEnd = nFrameEnd;\
         this.nQuestionAddedLMS = \"false\";\
      }")
      _T("\
      var g_aQuestionnaireEntries;\
      var g_aQuestionEntries;\
	  var g_nQuestionsCount;\
      if (!g_bIsAlreadyLoaded)\
      {\
         g_aQuestionnaireEntries = new Array();\
         g_aQuestionEntries = new Array();\
		 g_nQuestionsCount = 0;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));
   
   // Reset interactive objects (DnD, MC, FiB)
   _stprintf(script, _T("\
      function resetDragAndDrop(nPage)\
      {\
         for (var i = 0; i < g_aTargetPointEntries.length; ++i)\
         {\
            var pageNr = g_aTargetPointEntries[i].nPage;\
            if (pageNr == nPage)\
            {\
               if (g_aTargetPointEntries[i].strObjId != \"\")\
               {\
                  var imgObj = eval(\"swfSprPages.swfSpr\" + g_aTargetPointEntries[i].strObjId);\
                  imgObj._x = imgObj.origX;\
                  imgObj._y = imgObj.origY;\
               }\
            }\
         }\
      }")
      _T("\
      function resetMultipleChoice(nPage)\
      {\
         for (var i = 0; i < g_aRadioButtonEntries.length; ++i)\
         {\
            var pageNr = g_aRadioButtonEntries[i].nPage;\
            if (pageNr == nPage)\
            {\
               var rdbObj = eval(\"swfSprPages.swfSpr\" + g_aRadioButtonEntries[i].strId);\
               rdbObj.swfSprRdb.gotoAndStop(1);\
               rdbObj.bIsChecked = false;\
               \
            }\
         }\
         \
         for (var i = 0; i < g_aCheckBoxEntries.length; ++i)\
         {\
            var pageNr = g_aCheckBoxEntries[i].nPage;\
            if (pageNr == nPage)\
            {\
               var chkObj = eval(\"swfSprPages.swfSpr\" + g_aCheckBoxEntries[i].strId);\
               chkObj.swfSprChk.gotoAndStop(1);\
               chkObj.bIsChecked = false;\
               \
            }\
         }\
      }")
      _T("\
      function resetFillInBlank(nPage)\
      {\
         for (var i = 0; i < g_aTextFieldEntries.length; ++i)\
         {\
            var pageNr = g_aTextFieldEntries[i].nPage;\
            if (pageNr == nPage)\
            {\
               var txtObj = eval(\"swfSprPages.swfSpr\" + g_aTextFieldEntries[i].strId + \".swfTxtField\");\
               txtObj.text = \"\";\
            }\
         }\
      }\
      \
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Check (and interpret) interactive objects (DnD, MC, FiB)
   _stprintf(script, _T("\
      function checkDragAndDrop(nPage)\
      {\
         var res = false;\
         var bHasTargetPoints  = false;\
         var bIsCorrect = true;\
         for (var i = 0; i < g_aTargetPointEntries.length; ++i)\
         {\
            var pageNr = g_aTargetPointEntries[i].nPage;\
            if (pageNr == nPage)\
            {\
               g_interactionType = \"matching\";\
               bHasTargetPoints = true;\
               if (g_aTargetPointEntries[i].strObjId != \"\")\
               {\
                  var imgObj = eval(\"swfSprPages.swfSpr\" + g_aTargetPointEntries[i].strObjId);\
                  bIsCorrect = targetPointInImageBoundaries(g_aTargetPointEntries[i], imgObj);\
                  if (!bIsCorrect)\
                     break;\
               }\
            }\
         }\
         if (bHasTargetPoints && bIsCorrect)\
            res = true;\
         return res;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function checkMultipleChoice(nPage)\
      {\
         var res = false;\
         var bHasMultipleChoice = false;\
         var bIsCorrect = true;\
         for (var i = 0; i < g_aRadioButtonEntries.length; ++i)\
         {\
            var pageNr = g_aRadioButtonEntries[i].nPage;\
            if (pageNr == nPage)\
            {\
               g_interactionType = \"singlechoice\";\
               bHasMultipleChoice = true;\
               var rdbObj = eval(\"swfSprPages.swfSpr\" + g_aRadioButtonEntries[i].strId);\
               if (rdbObj.bIsChecked && !g_aRadioButtonEntries[i].bIsChecked)\
                  bIsCorrect = false;\
               if (!rdbObj.bIsChecked && g_aRadioButtonEntries[i].bIsChecked)\
                  bIsCorrect = false;\
               if (!bIsCorrect)\
                  break;\
            }\
         }")
         _T("\
         for (var i = 0; i < g_aCheckBoxEntries.length; ++i)\
         {\
            var pageNr = g_aCheckBoxEntries[i].nPage;\
            if (pageNr == nPage)\
            {\
               g_interactionType = \"multiplechoice\";\
               bHasMultipleChoice = true;\
               var chkObj = eval(\"swfSprPages.swfSpr\" + g_aCheckBoxEntries[i].strId);\
               if (chkObj.bIsChecked && !g_aCheckBoxEntries[i].bIsChecked)\
                  bIsCorrect = false;\
               if (!chkObj.bIsChecked && g_aCheckBoxEntries[i].bIsChecked)\
                  bIsCorrect = false;\
               if (!bIsCorrect)\
                  break;\
            }\
         }\
         if (bHasMultipleChoice && bIsCorrect)\
            res = true;\
         return res;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function checkFillInBlank(nPage)\
      {\
         var res = false;\
         var bHasTextFields = false;\
         var bIsCorrect = true;\
         for (var i = 0; i < g_aTextFieldEntries.length; ++i)\
         {\
            var pageNr = g_aTextFieldEntries[i].nPage;\
            if (pageNr == nPage)\
            {\
               g_interactionType = \"fill-in\";\
               bHasTextFields = true;\
               var txtObj = eval(\"swfSprPages.swfSpr\" + g_aTextFieldEntries[i].strId + \".swfTxtField\");\
               bIsCorrect = isAnswerStringInStringArray(txtObj.text, g_aTextFieldEntries[i].strText);\
               if (!bIsCorrect)\
                  break;\
            }\
         }\
         if (bHasTextFields && bIsCorrect)\
            res = true;\
         return res;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Deactivate interactive objects (DnD, MC, FiB)
   _stprintf(script, _T("\
      function deactivateDragAndDrop(nPage)\
      {\
         /*Event handler for DnD check themselves for deactivation*/\
         /*Stop every dragging:*/\
         for (var i = 0; i < g_aTargetPointEntries.length; ++i)\
         {\
            var pageNr = g_aTargetPointEntries[i].nPage;\
            if (pageNr == nPage)\
            {\
               if (g_aTargetPointEntries[i].strObjId != \"\")\
               {\
                  var imgObj = eval(\"swfSprPages.swfSpr\" + g_aTargetPointEntries[i].strObjId);\
                  imgObj.stopDrag();\
               }\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function deactivateMultipleChoice(nPage)\
      {\
         for (var i = 0; i < g_aRadioButtonEntries.length; ++i)\
         {\
            var pageNr = g_aRadioButtonEntries[i].nPage;\
            if (pageNr == nPage)\
            {\
               var rdbObj = eval(\"swfSprPages.swfSpr\" + g_aRadioButtonEntries[i].strId);\
               rdbObj.swfBtnRdb._visible = false;\
               if (rdbObj.bIsChecked)\
                  rdbObj.swfSprRdb.gotoAndStop(4);\
               else\
                  rdbObj.swfSprRdb.gotoAndStop(3);\
            }\
         }")
         _T("\
         for (var i = 0; i < g_aCheckBoxEntries.length; ++i)\
         {\
            var pageNr = g_aCheckBoxEntries[i].nPage;\
            if (pageNr == nPage)\
            {\
               var chkObj = eval(\"swfSprPages.swfSpr\" + g_aCheckBoxEntries[i].strId);\
               chkObj.swfBtnChk._visible = false;\
               if (chkObj.bIsChecked)\
                  chkObj.swfSprChk.gotoAndStop(4);\
               else\
                  chkObj.swfSprChk.gotoAndStop(3);\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function deactivateFillInBlank(nPage)\
      {\
         for (var i = 0; i < g_aTextFieldEntries.length; ++i)\
         {\
            var pageNr = g_aTextFieldEntries[i].nPage;\
            if (pageNr == nPage)\
            {\
               var txtObj = eval(\"swfSprPages.swfSpr\" + g_aTextFieldEntries[i].strId + \".swfTxtField\");\
               txtObj.selectable = false;\
               txtObj.borderColor = 0xC0C0C0;\
               txtObj.textColor = 0xC0C0C0;\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Deactivate Testing Buttons
   _stprintf(script, _T("\
      function deactivateTestingButtons(nPage)\
      {\
         for (var i = 0; i < g_aInteractionEntries.length; ++i)\
         {\
            if (g_aInteractionEntries[i].bIsTestingButton == true)\
            {\
               var pageNr = getPageFromFrame(g_aInteractionEntries[i].nFrameActiveBegin);\
               if (pageNr == nPage)\
               {\
                  var strId = g_aInteractionEntries[i].strId;\
                  var objNormal = eval(\"swfSprPages.swfSpr\" + strId);\
                  var objDisabled = eval(\"swfSprPages.swfSpr\" + strId + \"_Disabled\");\
                  objNormal._visible = false;\
                  objDisabled._visible = true;\
                  g_aInteractionEntries[i].nFrameActiveBegin = -1;\
                  g_aInteractionEntries[i].nFrameActiveEnd = -1;\
               }\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // All questions answered?
   _stprintf(script, _T("\
      function areAllQuestionsAnswered(qqIndex)\
      {\
         var questionnaire = g_aQuestionnaireEntries[qqIndex];\
         var qqPageStart = questionnaire.nPageBegin;\
         var qqPageEnd = questionnaire.nPageEnd;\
         var bAllQuestionsAnswered = true;\
         if (!g_isRandomTest)\
         {\
             for (var page = qqPageStart; page <= qqPageEnd; ++page)\
             {\
                if (getQuestionIndex(page) > -1)\
                {\
                   var bIsDeactivated = g_aQuestionEntries[getQuestionIndex(page)].bIsDeactivated;\
                   var bSuccessfullyAnswered = g_aQuestionEntries[getQuestionIndex(page)].bSuccessfullyAnswered;\
                   var bQuestionAnswered = (bIsDeactivated || bSuccessfullyAnswered) ? true : false;\
                   if (!bQuestionAnswered)\
                   {\
                      bAllQuestionsAnswered = false;\
                      break;\
                   }\
                }\
             }\
         }\
         else\
         {\
             for (var i = 0; i < g_randomQuestionOrder.length; ++i )\
             {\
                 var bIsDeactivated = g_aQuestionEntries[g_randomQuestionOrder[i]].bIsDeactivated;\
                 var bSuccessfullyAnswered = g_aQuestionEntries[g_randomQuestionOrder[i]].bSuccessfullyAnswered;\
                 var bQuestionAnswered = (bIsDeactivated || bSuccessfullyAnswered) ? true : false;\
                 if (!bQuestionAnswered)\
                 {\
                    bAllQuestionsAnswered = false;\
                    break;\
                 }\
             }\
             if(bAllQuestionsAnswered == true)\
                g_firstQuestionTimestamp = -1;\
         }\
         return bAllQuestionsAnswered;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Submit Testing
   _stprintf(script, _T("\
      function submitTesting(qIndex)\
      {\
         if (g_timeoutInterval > 0)\
            clearInterval(g_timeoutInterval);\
         var fbIdx = -1;\
         var qqIndex = g_aQuestionEntries[qIndex].nQuestionnaireIndex;\
         if (getIsQuestionnairePassed(qqIndex))\
            fbIdx = getFeedbackIndexFromQuestionIndex(qIndex, FEEDBACK_TYPE_QQ_PASSED, false);\
         else\
            fbIdx = getFeedbackIndexFromQuestionIndex(qIndex, FEEDBACK_TYPE_QQ_FAILED, false);\
         \
         if (fbIdx >= 0)\
         {\
            g_nFeedbackTestingIndex = fbIdx;\
            openFeedbackMessage(fbIdx, false);\
         }\
         \
         if (g_bFeedbackSummaryEnabled)\
            submitSummary(qqIndex);\
      }")
      _T("\
      function submitSummary(qqIndex)\
      {\
         clearInterval(g_QuestionnaireId);\
         if ((g_FeedbackId != null) || (g_nFeedbackIndexTemp >= 0))\
         {\
            /*'setInterval' needs global variables as parameters*/\
            g_nQuestionnaireIndex = qqIndex;\
            g_QuestionnaireId = setInterval(submitSummary, 333, g_nQuestionnaireIndex);\
            return;\
         }\
         \
         var fbIdx = -1;\
         fbIdx = getFeedbackIndexFromQuestionIndex(qqIndex, FEEDBACK_TYPE_QQ_EVAL, true);\
         \
         g_nQuestionnaireIndex = -1;\
         if (fbIdx >= 0)\
            openFeedbackMessage(fbIdx, true);\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Update Tries from Question
   _stprintf(script, _T("\
      function updateTriesFromQuestionIndex(qIdx)\
      {\
         var nTries = g_aQuestionEntries[qIdx].nMaximumAttempts - g_aQuestionEntries[qIdx].nTakenAttempts;\
         if (nTries < 0)\
		    nTries = 0;\
		 else\
			if (g_aQuestionEntries[qIdx].nMaximumAttempts == 0)\
				nTries = 1;\
         var strTries = \"\" + nTries;\
         var actualPage = g_aQuestionEntries[qIdx].nPage;\
         for (var i = 0; i < g_aChangeTextEntries.length; ++i)\
         {\
         if((g_aChangeTextEntries[i].nPage == actualPage) && (g_aChangeTextEntries[i].nType == TEXT_TYPE_CHANGE_TRIES))\
            {\
               var strId = g_aChangeTextEntries[i].strId;\
               var objText = eval(\"swfSprPages.swfSpr\" + strId + \".swfTxtChange\");\
               objText.text = strTries;\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Update Timer from Question
   _stprintf(script, _T("\
      function updateTimerFromQuestionIndex(qIdx)\
      {\
         var nSec = g_aQuestionEntries[qIdx].nTimeoutSec - g_aQuestionEntries[qIdx].nViewedSec;\
         if (nSec < 0)\
            nSec = 0;\
         var strTime = getTimeFromSeconds(nSec);\
         var actualPage = g_aQuestionEntries[qIdx].nPage;\
         for (var i = 0; i < g_aChangeTextEntries.length; ++i)\
         {\
         if((g_aChangeTextEntries[i].nPage == actualPage) && (g_aChangeTextEntries[i].nType == TEXT_TYPE_CHANGE_TIME))\
            {\
               var strId = g_aChangeTextEntries[i].strId;\
               var objText = eval(\"swfSprPages.swfSpr\" + strId + \".swfTxtChange\");\
               objText.text = strTime;\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Utils
   _stprintf(script, _T("\
      function isAnswerStringInStringArray(strAnswer, strAllAnswers)\
      {\
         var bIsInArray = false;\
         var alAnswers = strAllAnswers.split(\";\");\
         for (var i = 0; i < alAnswers.length; ++i)\
         {\
            if (alAnswers[i] == strAnswer)\
            {\
               bIsInArray = true;\
               break;\
            }\
         }\
         return bIsInArray;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function getTimeFromSeconds(nSec)\
      {\
         var strTime = new String(\"\");\
         /*var hrs = Math.floor(nSec / 3600.0);*/\
         /*nSec = nSec - (3600 * hrs);*/\
         var min = Math.floor(nSec / 60.0);\
         nSec = nSec - (60 * min);\
         var sec = nSec;\
         /*if (hrs < 10)*/\
            /*strTime += \"0\";*/\
         /*strTime += hrs;*/\
         /*strTime += \":\";*/\
         if (min < 10)\
            strTime += \"0\";\
         strTime += min;\
         strTime += \":\";\
         if (sec < 10)\
            strTime += \"0\";\
         strTime += sec;\
         return strTime;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function getQuestionnaireIndex(nPage)\
      {\
         var qqIdx = -1;\
         for (var i = 0; i < g_aQuestionnaireEntries.length; ++i)\
         {\
            var pageStart = g_aQuestionnaireEntries[i].nPageBegin;\
            var pageEnd = g_aQuestionnaireEntries[i].nPageEnd;\
            if ((nPage >= pageStart) && (nPage <= pageEnd))\
            {\
               qqIdx = i;\
               break;\
            }\
         }\
         return qqIdx;\
      }\
      \
      function getQuestionIndex(nPage)\
      {\
         var qIdx = -1;\
         for (var i = 0; i < g_aQuestionEntries.length; ++i)\
         {\
            var pageNr = g_aQuestionEntries[i].nPage;\
            if (nPage == pageNr)\
            {\
               qIdx = i;\
               break;\
            }\
         }\
         return qIdx;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function getQuestionIndexFromFrame(nFrame)\
      {\
         var nPage = getPageFromFrame(nFrame);\
         return getQuestionIndex(nPage);\
      }\
      \
      function getPageFromFrame(nFrame)\
      {\
         var nPage = -1;\
         for (var i = 0; i < (g_aPagingFrames.length-1); ++i)\
         {\
            if ((nFrame >= g_aPagingFrames[i]) && (nFrame < g_aPagingFrames[i+1]))\
            {\
               nPage = i;\
               break;\
            }\
         }\
         if (nFrame < g_aPagingFrames[0])\
            nPage = 0;\
         i = g_aPagingFrames.length-1;\
         if (nFrame >= g_aPagingFrames[i])\
            nPage = i;\
         \
         if (nPage < 0)\
            return nPage;\
         else\
            return (nPage+1);\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Add "QuestionnaireEntry"/"QuestionEntry" objects
   for (int i = 0; SUCCEEDED(hr) && i < m_aQuestionnaires.GetSize(); ++i)
   {
      // "QuestionnaireEntry"
      CQuestionnaire *pQuestionnaire = m_aQuestionnaires.ElementAt(i);
      int nNumOfQuestions = pQuestionnaire->GetQuestionCount();
      int nTotalPoints = pQuestionnaire->GetMaximumPoints();
      int nPassPoints = pQuestionnaire->GetPassPoints();
      if (pQuestionnaire->IsRelative())
         nPassPoints = (int)(0.5f +(nPassPoints * nTotalPoints / 100.0f));
      CPoint activityTimes;
      pQuestionnaire->GetActivityTimes(&activityTimes);
      int nFrameBegin = GetFrameFromTimestamp(activityTimes.x);
      int nFrameEnd = GetFrameFromTimestamp(activityTimes.y);
      int nPageBegin = GetPageFromActivityFrames(nFrameBegin, nFrameBegin);
      int nPageEnd = GetPageFromActivityFrames(nFrameEnd, nFrameEnd);

      _stprintf(script, _T("\
         if (!g_bIsAlreadyLoaded)\
         {\
            var g_nTotalMaxAchievablePoints = 0;\
            var g_nTotalMinAchievablePoints = 0;\
            var g_nTotalPassAchievablePoints = 0;\
            var qqEntry = new QuestionnaireEntry(%d, %d, %d, %d, %d, %d, %d);\
            g_aQuestionnaireEntries.push(qqEntry);\
         }\
      "), nNumOfQuestions, nTotalPoints, nPassPoints, nPageBegin, nPageEnd, nFrameBegin, nFrameEnd);
      PrintActionScript(_T("Movie"), script);
      movie->add(new SWFAction(GetCharFromCString(script)));

      // "QuestionEntry"
      CArray<CQuestion *, CQuestion *> aQuestions;
      aQuestions.RemoveAll();
      aQuestions.Append(pQuestionnaire->GetQuestions());

      for (int k = 0; k < aQuestions.GetSize(); ++k)
      {
         CQuestion *pQuestion = aQuestions.ElementAt(k);
         CString strId = pQuestion->GetScormId();
         int nQuestionnaireIndex = i;
         int nAchievablePoints = pQuestion->GetAchievablePoints();
         CString csSussessfullyAnswered = _T("false");
         CString csIsDeactivated = _T("false");
         int nMaximumAttempts = pQuestion->GetMaximumAttempts();
         int nTakenAttempts = 0;
         int nTimeoutSec = pQuestion->GetMaximumTime();
         int nViewedSec = 0;
         CPoint activitiyTimePeriod;
         pQuestion->GetActivityTimes(&activitiyTimePeriod);
         nFrameBegin = GetFrameFromTimestamp(activitiyTimePeriod.x);
         nFrameEnd = GetFrameFromTimestamp(activitiyTimePeriod.y);;
         int nPage = GetPageFromActivityFrames(nFrameBegin, nFrameEnd);

         _stprintf(script, _T("\
            if (!g_bIsAlreadyLoaded)\
            {\
               var qEntry = new QuestionEntry(%d, \"%s\", %d, %s, %s, %d, %d, %d, %d, %d, %d, %d);\
               g_aQuestionEntries.push(qEntry);\
			   g_nQuestionsCount++;\
            }\
         "), nQuestionnaireIndex, strId, nAchievablePoints, csSussessfullyAnswered, csIsDeactivated, nMaximumAttempts, 
         nTakenAttempts, nTimeoutSec, nViewedSec, nPage, nFrameBegin, nFrameEnd);
         PrintActionScript(_T("Movie"), script);
         movie->add(new SWFAction(GetCharFromCString(script)));
      }
   }

   return hr;
}

UINT CFlashEngine::AddActionScriptForSaveLoadSuspendData(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   _TCHAR script[4096];

   // Create Suspend Data
   _stprintf(script, _T("\
      function CreateAndSaveSuspendData()\
      {\
         g_suspendData = '';\
         if(g_bHasTesting && g_LMSResumeSupported)\
         {\
            var qLooping = false;\
            for (var i = 0; i < g_aQuestionEntries.length; ++i)\
            {\
               if(qLooping)\
                  g_suspendData+=g_MajorStrSeparator;\
               else\
                  qLooping = true;\
               g_suspendData += i + g_MinorStrSeparator +\
               g_aQuestionEntries[i].nQuestionAddedLMS + g_MinorStrSeparator +\
               g_aQuestionEntries[i].bIsDeactivated + g_MinorStrSeparator +\
               g_aQuestionEntries[i].nTakenAttempts + g_MinorStrSeparator +\
               g_aQuestionEntries[i].nViewedSec ;\
            }\
         }")
         _T("\
         g_suspendData += g_ScopeStrSeparator;\
         for(var i = 0; i < g_aVisitedPages.length-1; i++)\
            g_suspendData += g_aVisitedPages[i] + g_MinorStrSeparator;\
         if(g_aVisitedPages.length >= 1)\
            g_suspendData += g_aVisitedPages[g_aVisitedPages.length-1];\
         \
         if(g_isRandomTest)\
		{\
		    g_suspendData += g_ScopeStrSeparator;\
		    for( var k = 0 ; k < g_randomQuestionOrder.length - 1; k++)\
		    {\
		        g_suspendData += g_randomQuestionOrder[k] + g_MinorStrSeparator;\
		    }\
		    if(g_randomQuestionOrder.length >= 1)\
		        g_suspendData += g_randomQuestionOrder[g_randomQuestionOrder.length - 1];\
		}\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Load Suspend Data
   _stprintf(script, _T("\
      function LoadSuspendData()\
      {\
         var a_suspendData = new Array();\
         a_suspendData = g_suspendData.split(g_ScopeStrSeparator);\
         if(g_bHasTesting && g_LMSResumeSupported)\
         {\
            var a_QuestionData = new Array();\
            a_QuestionData = a_suspendData[0].split(g_MajorStrSeparator);\
            var a_QuestionValue;\
            for ( var j=0;j<a_QuestionData.length;j++)\
            {\
               a_QuestionValue =  new Array();\
               a_QuestionValue = a_QuestionData[j].split(g_MinorStrSeparator);\
               var a_QuestionIndex = a_QuestionValue[0];\
               g_aQuestionEntries[a_QuestionIndex].nQuestionAddedLMS = a_QuestionValue[1];\
               g_aQuestionEntries[a_QuestionIndex].nTakenAttempts = parseInt(a_QuestionValue[3]);\
               g_aQuestionEntries[a_QuestionIndex].nViewedSec = parseInt(a_QuestionValue[4]);\
			   if(a_QuestionValue[2]==\"true\")\
			   {\
				  g_aQuestionEntries[a_QuestionIndex].bIsDeactivated = true;\
                  deactivateQuestion(g_aQuestionEntries[a_QuestionIndex].nPage);\
			   }\
            }\
         }")
         _T("\
         var a_visitedPages = new Array();\
         a_visitedPages = a_suspendData[1].split(g_MinorStrSeparator);\
         for ( var i = 0; i < a_visitedPages.length; i++)\
            if(a_visitedPages[i] == \"true\")\
               g_aVisitedPages[i] = a_visitedPages[i];\
        if(g_isRandomTest)\
			{\
			    var a_randomQuestionOrder = new Array();\
			    a_randomQuestionOrder = a_suspendData[2].split(g_MinorStrSeparator);\
			    for (var k = 0 ; k < a_randomQuestionOrder.length; k ++)\
			    {\
			        g_randomQuestionOrder[k] = parseInt(a_randomQuestionOrder[k]);\
			    }\
			    g_isRandomInitialized = true;\
			}\
         g_bDoLoadSuspendData = false;\
         g_bSuspendDataLoaded = true;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Check Completion Status
   _stprintf(script, _T("\
      function UpdateCompletionStatus()\
      {\
         var a_visitedCount = 0;\
         for(var i = 0; i < g_aVisitedPages.length; i++)\
         if(g_aVisitedPages[i] == \"true\")\
            a_visitedCount++;\
         if(a_visitedCount == g_aVisitedPages.length)\
         {\
         	g_completionStatus = \"completed\";\
            if(g_bScormEnabled && g_LMSResumeSupported)\
            {\
               getURL(\"javascript:scutils_SetCompletionStatus()\", \"_self\");\
            }\
         }\
         g_bDoSaveCompletionStatus = false;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Check Success Status
   _stprintf(script, _T("\
      function UpdateSuccessStatus()\
      {\
         if(g_nTotalCurrentlyAchievePoints >= g_nTotalPassAchievablePoints)\
         {\
         	g_successStatus = \"passed\";\
            if(g_bScormEnabled && g_LMSResumeSupported)\
            {\
               getURL(\"javascript:scutils_SetSuccessStatus()\", \"_self\");\
            }\
         }\
         g_bDoSaveSuccessStatus = false;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForLoadTestingData(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   _TCHAR script[4096];

   _stprintf(script, _T("\
      function LoadLearnerData()\
      {\
         if(g_bHasTesting && g_LMSResumeSupported)\
         {\
            var aIndexes = new Array();\
            var aResponses = new Array();\
            var aResults = new Array();\
            if(g_questionIndex == null || g_questionIndex.length == 0)\
            {\
                g_bDoLoadLearnerData = false;\
                g_bLearnerDataLoaded = true;\
                return;\
            }\
            aIndexes = g_questionIndex.split(\",\");\
            aResponses = g_learnerResponse.split(\"<,>\");\
            aResults = g_questionResult.split(\",\");\
            for(var i = 0; i < aIndexes.length; i++)\
            {\
               var qIdx = aIndexes[i] * 1;\
               var pageNumber = g_aQuestionEntries[qIdx].nPage;\
               checkMultipleChoice(pageNumber);\
               checkFillInBlank(pageNumber);\
               checkDragAndDrop(pageNumber);\
               var str = aResponses[i];")
               _T("\
               if(str != null && str != 'undefined')\
               {\
                  if(g_interactionType == \"singlechoice\")\
                     onSetSingleChoiceData(pageNumber,str);\
                  else\
                     if(g_interactionType == \"multiplechoice\")\
                        onSetMultipleChoiceData(pageNumber,str);\
                     else\
                        if(g_interactionType == \"fill-in\")\
                        onSetFillInBlankData(pageNumber,str);\
                        else\
                           if(g_interactionType == \"matching\")\
                              onSetDragAndDropData(pageNumber,str);\
               }")
               _T("\
               var result = aResults[i];\
               var qqIdx = g_aQuestionEntries[qIdx].nQuestionnaireIndex;\
               if(result == 'c')\
               {\
                  g_aQuestionEntries[qIdx].bSuccessfullyAnswered = true;\
                  g_bQuestionIsDeactivated = true;\
                  deactivateQuestion(pageNumber);\
                  g_bAllQuestionsAnswered = areAllQuestionsAnswered(qqIdx);\
               }\
               else\
                  g_aQuestionEntries[qIdx].bSuccessfullyAnswered = false;\
            }\
         }\
         g_bDoLoadLearnerData = false;\
         g_bLearnerDataLoaded = true;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function onSetSingleChoiceData(pageNumber,str)\
      {\
         for (var i = 0; i < g_aRadioButtonEntries.length; ++i)\
         {\
            var pageNr = g_aRadioButtonEntries[i].nPage;\
            var nIdx = g_Responses.indexOf(str);\
            if (pageNr == pageNumber && g_aRadioButtonEntries[i].nIdx == nIdx)\
            {\
               var rdbObj = eval(\"swfSprPages.swfSpr\" + g_aRadioButtonEntries[i].strId);\
               rdbObj.swfSprRdb.gotoAndStop(2);\
               rdbObj.bIsChecked = true;\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function onSetMultipleChoiceData(pageNumber,str)\
      {\
         var index = 0;\
         for (var i = 0; i < g_aCheckBoxEntries.length; ++i)\
         {\
            var pageNr = g_aCheckBoxEntries[i].nPage;\
            if (pageNr == pageNumber)\
            {\
               var nIdx = g_Responses.indexOf(str.substr(index,1));\
               if(g_aCheckBoxEntries[i].nIdx == nIdx)\
               {\
                  var chkObj = eval(\"swfSprPages.swfSpr\" + g_aCheckBoxEntries[i].strId);\
                  chkObj.swfSprChk.gotoAndStop(2);\
                  chkObj.bIsChecked = true;\
                  index += 4;\
               }\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function onSetFillInBlankData(pageNumber,str)\
      {\
         var index = 0;\
         for (var i = 0; i < g_aTextFieldEntries.length; ++i)\
         {\
            var pageNr = g_aTextFieldEntries[i].nPage;\
            if (pageNr == pageNumber)\
            {\
               var strAux = '';\
               while (str.substr(index,1)!='[' && index < str.length)\
               {\
                  strAux += str.substr(index,1);\
                  index ++;\
               }\
               var txtObj = eval(\"swfSprPages.swfSpr\" + g_aTextFieldEntries[g_questionFillIndex].strId + \".swfTxtField\");\
               txtObj.text = strAux;\
               g_questionFillIndex ++;\
			   index += 3;\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function onSetDragAndDropData(pageNumber,str)\
      {\
         var index = 0;\
         while(index < g_aTargetPointEntries.length && g_aTargetPointEntries[index].nPage != pageNumber)\
            index ++;\
         var count = index;\
         while(count < g_aTargetPointEntries.length && g_aTargetPointEntries[count].nPage == pageNumber)\
            count ++;\
         var vector = new Array();\
         for(var k = index; k < count; k++)\
            vector[k] = g_aTargetPointEntries[k].nX;\
         vector.sort(16);\
         var i=0;\
         g_questionDragIndex = index;")
         _T("\
         while(i < str.length)\
         {\
            var imgObj = eval(\"swfSprPages.swfSpr\" + g_aTargetPointEntries[g_questionDragIndex].strObjId);\
            i += 4;\
            var response = str.substr(i,1);\
            var l = index;\
            while(l < count && !(response.length == 1 && response.substr(0,1) == '-'))\
            {\
               if(g_aTargetPointEntries[l].nX == vector[g_Responses.indexOf(response)])\
               {\
                  imgObj._x = ((g_aTargetPointEntries[l].nCenterX - g_nSlidesOffsetX) / g_dGlobalScale) - (0.5 * imgObj._width);\
                  imgObj._y = ((g_aTargetPointEntries[l].nCenterY - g_nSlidesOffsetY) / g_dGlobalScale) - (0.5 * imgObj._height);\
                  break;\
               }\
               else\
                  l++;\
            }\
            i += 4;\
            g_questionDragIndex ++;\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));


   return hr;
}

UINT CFlashEngine::AddActionScriptForFeedbackToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   CString csCorrect, csWrong; 
   CString csResult, csQuestion, csAnswer, csPoints; 
   CString csMaximumPoints, csRequiredPoints, csRequiredPercentage;
   CString csAchievedPoints, csAchievedPercentage;

   // Init strings with predefined items from the resource 
   // (if the next step should fail)
   csCorrect.LoadString(IDS_TESTING_CORRECT);
   csWrong.LoadString(IDS_TESTING_WRONG);
   csResult.LoadString(IDS_TESTING_RESULT);
   csQuestion.LoadString(IDS_TESTING_QUESTION);
   csAnswer.LoadString(IDS_TESTING_ANSWER);
   csPoints.LoadString(IDS_TESTING_POINTS);
   csMaximumPoints.LoadString(IDS_TESTING_MAXIMUM_POINTS);
   csRequiredPoints.LoadString(IDS_TESTING_REQUIRED_POINTS);
   csRequiredPercentage.LoadString(IDS_TESTING_REQUIRED_PERCENTAGE);
   csAchievedPoints.LoadString(IDS_TESTING_ACHIEVED_POINTS);
   csAchievedPercentage.LoadString(IDS_TESTING_ACHIEVED_PERCENTAGE);

   // Get the items from the document itself
   int nTextHeight = FillFeedbackSummaryStrings(csResult, csQuestion, csAnswer, 
                                                csPoints, csMaximumPoints, 
                                                csRequiredPoints, csRequiredPercentage, 
                                                csAchievedPoints, csAchievedPercentage);

   // Rescale Feedback, if necessary
   int nMaxFeedbackHeight = 0;
   for (int k = 0; k < m_aQuestionnaires.GetSize(); ++k)
   {
      int nTempHeight = 240 + ((nTextHeight + 6) * m_aQuestionnaires.ElementAt(k)->GetQuestionCount());
      if (nTempHeight > nMaxFeedbackHeight)
         nMaxFeedbackHeight = nTempHeight;
   }
   if (nMaxFeedbackHeight > m_iWbInputHeight)
      m_dFeedbackScale = (double)m_iWbInputHeight / (double)nMaxFeedbackHeight;

   _TCHAR script[4096];

   // Define class "FeedbackEntry"
   _stprintf(script, _T("\
      function FeedbackEntry(nX, nY, nWidth, nHeight, bHasQuestionnaire, nIdx, nType, bEnabled, strTextPassed, strText, strAction, clrBorder, clrBackground, clrText, strFontFamily, nFontSizeHeader, nFontSize, nFontSizeTable, strFontStyle, strFontWeight, nOkWidth, nOkHeight)\
      {\
         this.nX = nX;\
         this.nY = nY;\
         this.nWidth = nWidth;\
         this.nHeight = nHeight;\
         this.bHasQuestionnaire = bHasQuestionnaire;\
         this.nIdx = nIdx;\
         this.nType = nType;\
         this.bEnabled = bEnabled;\
         this.strTextPassed = strTextPassed;\
         this.strText = strText;\
         this.strAction = strAction;\
         this.clrBorder = clrBorder;\
         this.clrBackground = clrBackground;\
         this.clrText = clrText;\
         this.strFontFamily = strFontFamily;\
         this.nFontSizeHeader = nFontSizeHeader;\
         this.nFontSize = nFontSize;\
         this.nFontSizeTable = nFontSizeTable;\
         this.strFontStyle = strFontStyle;\
         this.strFontWeight = strFontWeight;\
         this.nOkWidth = nOkWidth;\
         this.nOkHeight = nOkHeight;\
      }")
      _T("\
      var g_dFbScale = %g;\
      var g_aFeedbackEntries;\
      if (!g_bIsAlreadyLoaded)\
         g_aFeedbackEntries = new Array();\
   "), m_dFeedbackScale);
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));
   
   // Feedback
   _stprintf(script, _T("\
      function getFeedbackIndexFromQuestionIndex(qIdx, nFeedbackType, bIsQuestionnaire)\
      {\
         var fbIdx = -1;\
         for (var i = 0; i < g_aFeedbackEntries.length; ++i)\
         {\
            var fb = g_aFeedbackEntries[i];\
            if ((fb.nIdx == qIdx) && (fb.nType == nFeedbackType) && (fb.bHasQuestionnaire == bIsQuestionnaire))\
            {\
               fbIdx = i;\
               break;\
            }\
         }\
         return fbIdx;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Bugfix 2444:
   // The 'nTimeoutMs' is used to avoid a side effect of the feature 'hide feedback, if navigation is used':
   // The result feedback message is not visible (because it is hidden so fast), if a 'jump' action 
   // is defined for question feedbacks.
   int nTimeoutMs = (int)(3.0f * 1000.0f / m_fFrameRate) + 5;
   _stprintf(script, _T("\
      function openFeedbackMessage(nFeedbackIndex, bIsQuestionnaire)\
      {\
         clearInterval(g_FeedbackId);\
         g_FeedbackId = null;\
         g_nFeedbackIndexTemp = nFeedbackIndex;\
         g_bIsQuestionnaireTemp = bIsQuestionnaire;\
         \
         if(!g_aFeedbackEntries[nFeedbackIndex].bEnabled)\
         {\
            /* feedback is not visible --> just execute the feedback action */\
            g_nFeedbackIndex = nFeedbackIndex;\
            \
            /* exception: Invisible QQ_PASSED/QQ_FAILED feedback */\
            if ((g_aFeedbackEntries[nFeedbackIndex].nType == FEEDBACK_TYPE_QQ_PASSED) || (g_aFeedbackEntries[nFeedbackIndex].nType == FEEDBACK_TYPE_QQ_FAILED))\
               return;\
            \
            executeFeedbackAction();\
            return;\
         }\
         \
         if (g_bFeedbackMessageVisible)\
         {\
            /*'setInterval' needs global variables as parameters*/\
            g_FeedbackId = setInterval(openFeedbackMessage, %d, g_nFeedbackIndexTemp, g_bIsQuestionnaireTemp);\
            return;\
         }\
         \
         g_bFeedbackMessageVisible = true;\
         g_nFeedbackIndex = nFeedbackIndex;\
         g_bIsQuestionnaire = bIsQuestionnaire;\
         showTestingFeedback(nFeedbackIndex, bIsQuestionnaire);\
      }\
   "), nTimeoutMs);
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function closeFeedbackMessage()\
      {\
         hideTestingFeedback();\
         g_bFeedbackMessageVisible = false;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function showTestingFeedback(nFeedbackIndex, bIsSummary)\
      {\
         if (bIsSummary)\
         {\
            swfSprPages.swfSprTestingFeedback.swfTxtFeedback.htmlText = getFeedbackString(nFeedbackIndex, bIsSummary);\
            var qqIdx = g_aFeedbackEntries[nFeedbackIndex].nIdx;\
            swfSprPages.swfSprTestingFeedback.swfTxtSummary.htmlText = getSummaryString(nFeedbackIndex, qqIdx);\
         }\
         else\
         {\
            swfSprPages.swfSprTestingFeedback.swfTxtFeedback.htmlText = getFeedbackString(nFeedbackIndex, bIsSummary);\
            swfSprPages.swfSprTestingFeedback.swfTxtSummary.htmlText = \"\";\
         }\
         formatMessageBox(nFeedbackIndex, bIsSummary);\
         swfSprPages.swfSprProtection._visible = true;\
         swfSprPages.swfSprTestingWindow._visible = true;\
         swfSprPages.swfSprTestingFeedback._visible = true;\
      }\
      \
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function hideTestingFeedback()\
      {\
         swfSprPages.swfSprTestingFeedback._visible = false;\
         swfSprPages.swfSprTestingWindow._visible = false;\
         swfSprPages.swfSprProtection._visible = false;\
         \
         if ( g_nFeedbackIndex < 0)\
            return;\
         \
         executeFeedbackAction();\
         \
         g_nFeedbackIndexTemp = -1;\
         g_bIsQuestionnaireTemp = false;\
         /*g_nFeedbackIndex = -1;*/\
         /*g_bIsQuestionnaire = false;*/\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function executeFeedbackAction()\
      {\
         var bQuestionAnswered = (!g_bAllQuestionsAnswered && g_bQuestionIsDeactivated);\
         var bTestingEvaluation = (g_bAllQuestionsAnswered && !g_bFeedbackSummaryEnabled && g_aFeedbackEntries[g_nFeedbackIndex].nType>=FEEDBACK_TYPE_QQ_PASSED);\
         var bTestingSummary = (g_bAllQuestionsAnswered && g_bIsQuestionnaire && g_bFeedbackSummaryEnabled);\
         if ( bQuestionAnswered || bTestingEvaluation || bTestingSummary)\
         {\
            /* To call a function from a string via 'apply' needs the function name without brackets */\
            var action;\
            if (bTestingSummary)\
               action = g_aFeedbackEntries[g_nFeedbackTestingIndex].strAction;\
            else\
               action = g_aFeedbackEntries[g_nFeedbackIndex].strAction;\
            var nDot = action.indexOf('.');\
            /*Ignore '.' inside URLs or file names*/\
            var nDoubleQuotes = action.indexOf('\"');\
            if (nDoubleQuotes >= 0 && nDoubleQuotes < nDot)\
               nDot = -1;\
            var nBracket = action.indexOf('(');\
            var func = action.substring(nDot+1, nBracket);\
            var path = \"this\";\
            if (nDot >= 0)\
               path += action.substring(0, nDot);\
            var parameters = action.substring(nBracket+1, (action.length-1));\
            var aParam = parameters.split(\",\");\
            for (var i = 0; i < aParam.length; ++i)\
            {\
               /* remove quotes from string parameters */\
               if (aParam[i].indexOf('\"') >= 0)\
                  aParam[i] = aParam[i].substring(1, aParam[i].length-1);\
            }\
            eval(path)[func].apply(null, aParam);\
            if (g_bIsInteractivePaused && action.indexOf(\"lecOnPause\") == -1)\
            {\
               g_bIsInteractivePaused = false;\
               lecOnPlay();\
            }\
         }\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function formatMessageBox(nFeedbackIndex, bIsSummary)\
      {\
         var fb = g_aFeedbackEntries[nFeedbackIndex];\
         var nWidth = 280; var nHeight = 240;\
         if (bIsSummary)\
         {\
            nWidth = 280;\
            var qqIdx = g_aFeedbackEntries[nFeedbackIndex].nIdx;\
            if (g_isRandomTest)\
               nHeight = 240+8+((fb.nFontSizeTable+6)* g_randomQuestionOrder.length);\
            else \
               nHeight = 240+8+((fb.nFontSizeTable+6)*g_aQuestionnaireEntries[qqIdx].nNumOfQuestions);\
         }\
         else\
         {\
            nWidth = fb.nWidth;\
            nHeight = fb.nHeight;\
         }\
         if(nWidth < (fb.nOkWidth+16))\
            nWidth = fb.nOkWidth+16;\
         ")
         _T("\
         var nWidthScaled = Math.round(g_dFbScale*nWidth);\
         var nHeightScaled = Math.round(g_dFbScale*nHeight);\
         var dW = Math.round(0.5*(280-nWidth));\
         var dH = Math.round(0.5*(240-nHeight));\
         \
         var nX = fb.nX;\
         var nY = fb.nY;\
         if ((nX+nWidthScaled) > g_nSlidesWidth)\
            nX = g_nSlidesWidth-nWidthScaled;\
         if ((nY+nHeightScaled) > g_nSlidesHeight)\
            nY = g_nSlidesHeight-nHeightScaled;\
         var dX = fb.nX - nX;\
         var dY = fb.nY - nY;\
         \
         formatMessageBoxObjects(fb, nX, nY, nWidth, nHeight, nWidthScaled, nHeightScaled, dX, dY, dW, dH, bIsSummary);\
         formatMessageBoxColors(fb);\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function formatMessageBoxObjects(fb, nX, nY, nWidth, nHeight, nWidthScaled, nHeightScaled, dX, dY, dW, dH, bIsSummary)\
      {\
         var objWindow = swfSprPages.swfSprTestingWindow;\
         var objFeedback = swfSprPages.swfSprTestingFeedback.swfTxtFeedback;\
         var objSummary = swfSprPages.swfSprTestingFeedback.swfTxtSummary;\
         var objOk = swfSprPages.swfSprTestingFeedback.swfSprOk;\
         var dWs = Math.round(280*0.75*(1.0-g_dFbScale));\
         var dHs = Math.round(240*0.75*(1.0-g_dFbScale));\
         dWs += fb.nX - dX - Math.round(0.5*(g_nSlidesWidth-nWidthScaled));\
         dHs += fb.nY - dY - Math.round(0.5*(g_nSlidesHeight-nHeightScaled));\
         objWindow._x = nX; objWindow._y = nY;\
         objWindow._width = nWidthScaled; objWindow._height = nHeightScaled;\
         objFeedback._width = nWidth-16;\
         objSummary._width = nWidth-16+4;")
         _T("\
         if (bIsSummary)\
         {\
            objFeedback._height = 32;\
            objFeedback._x = dW+dWs+5; objFeedback._y = dH+dHs+16;\
            objSummary._height = nHeight-16-32-fb.nOkHeight-10;\
            objSummary._x = dW+dWs+5+8-4; objSummary._y = dH+dHs+16+32;\
         }\
         else\
         {\
            objFeedback._height = nHeight-8-fb.nOkHeight-10;\
            objFeedback._x = dW+dWs+5; objFeedback._y = dH+dHs+16;\
            objSummary._height = 0;\
            objSummary._x = dW+dWs+5+8-4; objSummary._y = dH+dHs+nHeight+16-32-fb.nOkHeight-10;\
         }")
         _T("\
         objOk._x = dW+dWs+Math.round(0.5*(nWidth-fb.nOkWidth));\
         objOk._y = dH+dHs+nHeight-6-fb.nOkHeight;\
         /*objOk._width = fb.nOkWidth;*/\
         /*objOk._height = fb.nOkHeight;*/\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function formatMessageBoxColors(fb)\
      {\
         var objBackground = swfSprPages.swfSprTestingWindow.swfSprBackground.swfShpBackground;\
         var objBorder = swfSprPages.swfSprTestingWindow.swfSprBorder.swfShpBorder;\
         var clrBackground = new Color(objBackground);\
         clrBackground.setRGB(fb.clrBackground);\
         var clrBorder = new Color(objBorder);\
         clrBorder.setRGB(fb.clrBorder);\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function getIsQuestionnairePassed(nQuestionnaireIndex)\
      {\
         var qq = g_aQuestionnaireEntries[nQuestionnaireIndex];\
         var nSumPoints = 0;\
         if(!g_isRandomTest)\
         {\
             for (var page = qq.nPageBegin; page <= qq.nPageEnd; ++page)\
             {\
                if (getQuestionIndex(page) > -1)\
                {\
                   var bPassed = g_aQuestionEntries[getQuestionIndex(page)].bSuccessfullyAnswered;\
                   var nPoints = (bPassed) ? (g_aQuestionEntries[getQuestionIndex(page)].nAchievablePoints) : (0);\
                   nSumPoints += nPoints;\
                }\
             }\
         }\
         else\
         {\
            for (var i = 0; i < g_randomQuestionOrder.length; ++i)\
            {\
                var bPassed = g_aQuestionEntries[g_randomQuestionOrder[i]].bSuccessfullyAnswered;\
                var nPoints = (bPassed) ? (g_aQuestionEntries[g_randomQuestionOrder[i]].nAchievablePoints) : (0);\
                nSumPoints += nPoints;\
            }\
         }\
         if (nSumPoints >= g_nTotalPassAchievablePoints)\
            return true;\
         else\
            return false;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Notes for the scripting below: 
   // - <textformat> and <tab> are no HTML standard tags (maybe a mix of HTML and SQL tags). 
   // - 'leading' is used to cange the space between lines.
   _stprintf(script, _T("\
      function getFeedbackString(nFeedbackIndex, bIsSummary)\
      {\
         var fb = g_aFeedbackEntries[nFeedbackIndex];\
         var fntSize = fb.nFontSize;\
         if (bIsSummary)\
            fntSize = fb.nFontSizeHeader;\
         var strFeedback = \"\";\
         strFeedback += \"<p align='center'>\";\
         strFeedback += \"<textformat leading='3'>\";\
         strFeedback += \"<font face='\";\
         strFeedback += fb.strFontFamily;\
         strFeedback += \"' color='\";\
         strFeedback += fb.clrText;\
         strFeedback += \"' size='\";\
         strFeedback += fntSize;\
         strFeedback += \"px'>\";\
         if (bIsSummary)\
            strFeedback += \"%s\";\
         else\
            strFeedback += fb.strText;\
         strFeedback += \"</font>\";\
         strFeedback += \"</textformat>\";\
         strFeedback += \"</p>\";\
         return strFeedback;\
      }\
   "), csResult);
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function getSummaryString(nFeedbackIndex, nQuestionnaireIndex)\
      {\
         var strSummaryHeader = getSummaryHeaderString(nFeedbackIndex);\
         var strSummaryQuestions = getSummaryQuestionsString(nFeedbackIndex, nQuestionnaireIndex);\
         var strSummaryFooter = getSummaryFooterString(nFeedbackIndex, nQuestionnaireIndex);\
         var strSummaryAll = \"\";\
         strSummaryAll += strSummaryHeader;\
         strSummaryAll += strSummaryQuestions;\
         strSummaryAll += strSummaryFooter;\
         return strSummaryAll;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function getSummaryHeaderString(nFeedbackIndex)\
      {\
         var fb = g_aFeedbackEntries[nFeedbackIndex];\
         var strSummary = \"\";\
         strSummary += \"<textformat leading='-13' tabstops='100,200,248'>\";\
         strSummary += \"<font face='\";\
         strSummary += fb.strFontFamily;\
         strSummary += \"' color='\";\
         strSummary += fb.clrText;\
         strSummary += \"' size='\";\
         strSummary += fb.nFontSize;\
         strSummary += \"px'>\";\
         strSummary += \"%s\";\
         strSummary += \"<tab>\";\
         strSummary += \"%s\";\
         strSummary += \"<tab>\";\
         strSummary += \"%s\";\
         strSummary += \"<tab>\";\
         strSummary += \"</font>\";\
         strSummary += \"</textformat>\";\
         strSummary += \"<br>\";")
         _T("\
         strSummary += \"<textformat leading='3' tabstops='100,200,258'>\";\
         strSummary += \"<font face='\";\
         strSummary += fb.strFontFamily;\
         strSummary += \"' color='\";\
         strSummary += fb.clrText;\
         strSummary += \"' size='1pt'><u>\";\
         strSummary += \"<tab><tab><tab>\";\
         strSummary += \"</u></font>\";\
         strSummary += \"</textformat>\";\
         strSummary += \"<br>\";\
         return strSummary;\
      }\
   "), csQuestion, csAnswer, csPoints);
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function getSummaryQuestionsString(nFeedbackIndex, nQuestionnaireIndex)\
      {\
         var fb = g_aFeedbackEntries[nFeedbackIndex];\
         var qq = g_aQuestionnaireEntries[nQuestionnaireIndex];\
         var idx = 0;\
         var strSummary = \"\";\
         if(!g_isRandomTest)\
         {\
             for (var page = qq.nPageBegin; page <= qq.nPageEnd; ++page)\
             {\
                if (getQuestionIndex(page) > -1)\
                {\
                   idx++;\
                   var bPassed = g_aQuestionEntries[getQuestionIndex(page)].bSuccessfullyAnswered;\
                   var nPoints = (bPassed) ? (g_aQuestionEntries[getQuestionIndex(page)].nAchievablePoints) : (0);\
                   var strPassed = (bPassed) ? (\"%s\") : (\"%s\");\
                   if (page == qq.nPageEnd)\
                      strSummary += \"<textformat leading='-13' tabstops='100,200,248'>\";\
                   else\
                      strSummary += \"<textformat leading='3' tabstops='100,200,248'>\";")
                   _T("\
                   strSummary += \"<font face='\";\
                   strSummary += fb.strFontFamily;\
                   strSummary += \"' color='\";\
                   strSummary += fb.clrText;\
                   strSummary += \"' size='\";\
                   strSummary += fb.nFontSizeTable;\
                   strSummary += \"px'>\";\
                   strSummary += \"%s \";\
                   strSummary += idx;\
                   strSummary += \"<tab>\";\
                   strSummary += strPassed;\
                   strSummary += \"<tab>\";\
                   strSummary += nPoints;\
                   strSummary += \"<tab>\";\
                   strSummary += \"</font>\";\
                   strSummary += \"</textformat>\";\
                   strSummary += \"<br>\";\
                }\
             }\
         } else {\
             for (var i = 0; i < g_randomQuestionOrder.length; ++i)\
             {\
               idx++;\
               var bPassed = g_aQuestionEntries[g_randomQuestionOrder[i]].bSuccessfullyAnswered;\
               var nPoints = (bPassed) ? (g_aQuestionEntries[g_randomQuestionOrder[i]].nAchievablePoints) : (0);\
               var strPassed = (bPassed) ? (\"%s\") : (\"%s\");\
               if (i == g_randomQuestionOrder.length)\
                  strSummary += \"<textformat leading='-13' tabstops='100,200,248'>\";\
               else\
                  strSummary += \"<textformat leading='3' tabstops='100,200,248'>\";\
               strSummary += \"<font face='\";\
               strSummary += fb.strFontFamily;\
               strSummary += \"' color='\";\
               strSummary += fb.clrText;\
               strSummary += \"' size='\";\
               strSummary += fb.nFontSizeTable;\
               strSummary += \"px'>\";\
               strSummary += \"%s \";\
               strSummary += idx;\
               strSummary += \"<tab>\";\
               strSummary += strPassed;\
               strSummary += \"<tab>\";\
               strSummary += nPoints;\
               strSummary += \"<tab>\";\
               strSummary += \"</font>\";\
               strSummary += \"</textformat>\";\
               strSummary += \"<br>\";\
             }\
         }\
         ")
         _T("strSummary += \"<textformat leading='-14' tabstops='100,200,258'>\";\
         strSummary += \"<font face='\";\
         strSummary += fb.strFontFamily;\
         strSummary += \"' color='\";\
         strSummary += fb.clrText;\
         strSummary += \"' size='1pt'><u>\";\
         strSummary += \"<tab><tab><tab></u></font></textformat><br>\";\
         strSummary += \"<textformat leading='3' tabstops='100,200,258'>\";\
         strSummary += \"<font face='\";\
         strSummary += fb.strFontFamily;\
         strSummary += \"' color='\";\
         strSummary += fb.clrText;\
         strSummary += \"' size='1pt'><u>\";\
         strSummary += \"<tab><tab><tab></u></font></textformat><br>\";\
         return strSummary;\
      }\
   "), csCorrect, csWrong, csQuestion, csCorrect, csWrong, csQuestion);
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function getSumPoints(qq)\
      {\
         var nSumPoints = 0;\
         if(!g_isRandomTest)\
         {\
             for (var page = qq.nPageBegin; page <= qq.nPageEnd; ++page)\
             {\
                var bPassed = g_aQuestionEntries[getQuestionIndex(page)].bSuccessfullyAnswered;\
                var nPoints = (bPassed) ? (g_aQuestionEntries[getQuestionIndex(page)].nAchievablePoints) : (0);\
                nSumPoints += nPoints;\
             }\
         } else {\
             for (var i = 0; i < g_randomQuestionOrder.length; ++i)\
             {\
                var bPassedR = g_aQuestionEntries[g_randomQuestionOrder[i]].bSuccessfullyAnswered;\
                var nPointsR = (bPassedR) ? (g_aQuestionEntries[g_randomQuestionOrder[i]].nAchievablePoints) : (0);\
                nSumPoints += nPointsR;\
             }\
         }\
         return nSumPoints;\
      }\
    "));
    PrintActionScript(_T("Movie"), script);
    movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function getSummaryFooterString(nFeedbackIndex, nQuestionnaireIndex)\
      {\
         var fb = g_aFeedbackEntries[nFeedbackIndex];\
         var qq = g_aQuestionnaireEntries[nQuestionnaireIndex];\
         var passPercentage = (g_nTotalMaxAchievablePoints>0) ? (Math.round(100.0 * (g_nTotalPassAchievablePoints/g_nTotalMaxAchievablePoints))) : (0);\
         if(g_relativePassPercentage != 0)\
            passPercentage = g_relativePassPercentage;\
         var strSummary = \"\";\
         strSummary += \"<textformat leading='3' tabstops='80,200,248'>\";\
         strSummary += \"<font face='\";\
         strSummary += fb.strFontFamily;\
         strSummary += \"' color='\";\
         strSummary += fb.clrText;\
         strSummary += \"' size='\";\
         strSummary += fb.nFontSizeTable;\
         strSummary += \"px'>\";\
         strSummary += \"%s<tab>\";\
         strSummary += g_nTotalMaxAchievablePoints;\
         strSummary += \"</font></textformat>\";\
         strSummary += \"<br>\";\
         strSummary += \"<textformat leading='3' tabstops='80,200,248'>\";\
         strSummary += \"<font face='\";\
         strSummary += fb.strFontFamily;\
         strSummary += \"' color='\";\
         strSummary += fb.clrText;\
         strSummary += \"' size='\";\
         strSummary += fb.nFontSizeTable;\
         strSummary += \"px'>\";\
         strSummary += \"%s<tab>\";\
         strSummary += g_nTotalPassAchievablePoints;\
         strSummary += \"</font></textformat>\";\
         strSummary += \"<br>\";")
         _T("\
         strSummary += \"<textformat leading='3' tabstops='80,200,248'>\";\
         strSummary += \"<font face='\";\
         strSummary += fb.strFontFamily;\
         strSummary += \"' color='\";\
         strSummary += fb.clrText;\
         strSummary += \"' size='\";\
         strSummary += fb.nFontSizeTable;\
         strSummary += \"px'>\";\
         strSummary += \"%s<tab>\";\
         strSummary += passPercentage;\
         strSummary += \"&#37;</font></textformat>\";\
         strSummary += \"<br>\";\
         var nSumPoints = getSumPoints(qq);")
         _T("\
         var bTestingPassed = (nSumPoints >= g_nTotalPassAchievablePoints) ? true : false;\
         var strPassed = \"\";\
         var fbIdx = -1;\
         if (bTestingPassed)\
            strPassed = fb.strTextPassed;\
         else\
            strPassed = fb.strText;\
         strSummary += \"<textformat leading='3' tabstops='80,200,248'>\";\
         strSummary += \"<font face='\";\
         strSummary += fb.strFontFamily;\
         strSummary += \"' color='\";\
         strSummary += fb.clrText;\
         strSummary += \"' size='\";\
         strSummary += fb.nFontSizeTable;\
         strSummary += \"px'>\";\
         strSummary += \"%s<tab>\";\
         strSummary += nSumPoints;\
         strSummary += \"</font></textformat>\";\
         strSummary += \"<br>\";")
         _T("\
         strSummary += \"<textformat leading='3' tabstops='80,200,248'>\";\
         strSummary += \"<font face='\";\
         strSummary += fb.strFontFamily;\
         strSummary += \"' color='\";\
         strSummary += fb.clrText;\
         strSummary += \"' size='\";\
         strSummary += fb.nFontSizeTable;\
         strSummary += \"px'>\";\
         strSummary += \"%s<tab>\";\
         strSummary += (g_nTotalMaxAchievablePoints>0) ? (Math.round(100.0 * (nSumPoints/g_nTotalMaxAchievablePoints))) : (0);\
         strSummary += \"&#37;</font></textformat>\";\
         strSummary += \"<br><textformat leading='-9'><br></textformat>\";\
         strSummary += \"<p align='left'>\";\
         strSummary += \"<font face='\";\
         strSummary += fb.strFontFamily;\
         strSummary += \"' color='\";\
         strSummary += fb.clrText;\
         strSummary += \"' size='\";\
         strSummary += fb.nFontSize;\
         strSummary += \"px'>\";\
         strSummary += strPassed;\
         strSummary += \"</font></p>\";\
         return strSummary;\
      }\
   "),  csMaximumPoints, csRequiredPoints, csRequiredPercentage, csAchievedPoints, csAchievedPercentage);
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Add "FeedbackEntry" objects
   int qIdx = 0;
   CString csAction = _T("");
   CArray<CFeedbackDefinition *, CFeedbackDefinition *> aQQFeedbacks;
   aQQFeedbacks.RemoveAll();
   _tprintf(_T("###  QQs: %d\n"), m_aQuestionnaires.GetSize());
   int i = 0;
   for (i = 0; i < m_aQuestionnaires.GetSize(); ++i)
   {
      CQuestionnaire *pQuestionnaire = m_aQuestionnaires.ElementAt(i);
      aQQFeedbacks.Append(pQuestionnaire->GetFeedback());
      _tprintf(_T("### QQ Feedbacks: %d\n"), aQQFeedbacks.GetSize());
      int j = 0;
      for (j = 0; j < aQQFeedbacks.GetSize(); ++j)
      {
         int nType = GetIndexFromFeedbackType(aQQFeedbacks.ElementAt(j)->GetType());
         if (nType == FEEDBACK_TYPE_QQ_PASSED)
            csAction = pQuestionnaire->GetSuccessAction();
         if (nType == FEEDBACK_TYPE_QQ_FAILED)
            csAction = pQuestionnaire->GetFailureAction();

         bool bEnabled = !aQQFeedbacks.ElementAt(j)->IsDisabled();
         FillFeedbackEntry(movie, aQQFeedbacks.ElementAt(j), i, csAction, bEnabled, true);
      }

      // Get the questions
      CArray<CQuestion *, CQuestion *> aQuestions;
      aQuestions.RemoveAll();
      aQuestions.Append(pQuestionnaire->GetQuestions());
      _tprintf(_T("###  Qs: %d\n"), aQuestions.GetSize());
      for (j = 0; j < aQuestions.GetSize(); ++j)
      {
         CQuestion *pQuestion = aQuestions.ElementAt(j);
         CArray<CFeedbackDefinition *, CFeedbackDefinition *> aQFeedbacks;
         aQFeedbacks.RemoveAll();
         aQFeedbacks.Append(pQuestion->GetFeedback());
         _tprintf(_T("###  Q Feedbacks: %d\n"), aQFeedbacks.GetSize());
         int k = 0;
         // The 'normal' feedbacks FEEDBACK_TYPE_Q_XXX
         for (k = 0; k < aQFeedbacks.GetSize(); ++k)
         {
            CFeedbackDefinition *pFeedback = aQFeedbacks.ElementAt(k);

            int nType = GetIndexFromFeedbackType(aQFeedbacks.ElementAt(k)->GetType());
            if (nType == FEEDBACK_TYPE_Q_CORRECT)
               csAction = pQuestion->GetSuccessAction();
            if (nType == FEEDBACK_TYPE_Q_WRONG)
               csAction = pQuestion->GetFailureAction();

            bool bEnabled = !aQFeedbacks.ElementAt(k)->IsDisabled();
            FillFeedbackEntry(movie, aQFeedbacks.ElementAt(k), qIdx, csAction, bEnabled, false);
         }
         // FEEDBACK_TYPE_QQ_XXX are inherited from CQuestionnaire object
         for (k = 0; k < aQQFeedbacks.GetSize(); ++k)
         {
            CFeedbackDefinition *pFeedback = aQQFeedbacks.ElementAt(k);

            int nType = GetIndexFromFeedbackType(aQQFeedbacks.ElementAt(k)->GetType());
            if (nType == FEEDBACK_TYPE_QQ_PASSED)
               csAction = pQuestionnaire->GetSuccessAction();
            if (nType == FEEDBACK_TYPE_QQ_FAILED)
               csAction = pQuestionnaire->GetFailureAction();

            bool bEnabled = !aQQFeedbacks.ElementAt(k)->IsDisabled();
            FillFeedbackEntry(movie, aQQFeedbacks.ElementAt(k), qIdx, csAction, bEnabled, false);
         }

         qIdx++;
      }
   }

   return hr;
}

UINT CFlashEngine::AddActionScriptForMessageBoxToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   _TCHAR script[4096];

   // Define class "FeedbackEntry"
   _stprintf(script, _T("\
      function openMessageBox(strMsgType, strMsgText)\
      {\
         clearInterval(g_MessageBoxId);\
         g_MessageBoxId = null;\
         g_strMsgTypeTemp = strMsgType;\
         g_strMsgTextTemp = strMsgText;\
         if (g_bMessageBoxVisible)\
         {\
            /*'setInterval' needs global variables as parameters*/\
            g_MessageBoxId = setInterval(openMessageBox, 333, g_strMsgTypeTemp, g_strMsgTextTemp);\
            return;\
         }\
         \
         g_bMessageBoxVisible = true;\
         g_strMsgType = strMsgType;\
         g_strMsgText = strMsgText;\
         showMessageBox(strMsgType, strMsgText);\
      }\
      \
      function showMessageBox(strMsgType, strMsgText)\
      {\
         var strText = \"<p>\";\
         strText += strMsgType;\
         strText += \"<br><br>\";\
         strText += strMsgText;\
         strText += \"</p>\";\
         swfSprMessageBox.swfTxtFeedback.htmlText = strText;\
         swfSprMessageBox._visible = true;\
      }\
      \
      function hideMessageBox()\
      {\
         swfSprMessageBox._visible = false;\
         g_strMsgTypeTemp = \"\";\
         g_strMsgTextTemp = \"\";\
      }\
      \
      function closeMessageBox()\
      {\
         hideMessageBox();\
         g_bMessageBoxVisible = false;\
      }\
   "));
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   return hr;
}

UINT CFlashEngine::AddActionScriptToControlBar(SWFSprite *sprControl, float fScale)
{
   HRESULT hr = S_OK;

   CString csScript;

   // Part II: methods used for display updates
   csScript = _T("\
      function lecUpdateTimeDisplay()\
      {\
         var objRoot = _parent;\
         var actualFrame = objRoot._currentframe - 1;\
         if (actualFrame < 0)\
            actualFrame = 0;\
         \
         var msec = actualFrame * Math.floor(1000.0 / (1.0*objRoot.g_fFrameRate));\
         var timeStamp = msec;\
         var hrs = Math.floor(msec / 3600000.0);\
         msec -= (hrs * 3600000);\
         var min = Math.floor(msec / 60000.0);\
         msec -= (min * 60000);\
         var sec = Math.floor(msec / 1000.0);\
         \
         var strTime = new String(\"         \");\
         if (hrs < 10)\
            strTime += \"0\";\
         strTime += hrs;\
         strTime += \":\";\
         if (min < 10)\
            strTime += \"0\";\
         strTime += min;\
         strTime += \":\";\
         if (sec < 10)\
            strTime += \"0\";\
         strTime += sec;\
         \
         swfSprTxtTime.swfTxtTime.text = strTime;\
         \
         /*swfSprTxtTime.swfTxtTime._x = -30;*/\
         /*swfSprTxtTime.swfTxtTime._width = 250;*/\
      }\
   ");
   PrintActionScript(_T("ControlBar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecUpdateSlideDisplay()\
      {\
         var objRoot = _parent;\
         if(objRoot.g_bMovieHasPages && !objRoot.g_bIsPreloading)\
         {\
            var actualPage = objRoot.lecGetActualPage();\
            var strSlide = new String(objRoot.g_strSlide);\
            strSlide += \" \";\
            strSlide += actualPage;\
            strSlide += \" \";\
            strSlide += objRoot.g_strOf;\
            strSlide += \" \";\
            strSlide += objRoot.g_nPages;\
            \
            swfSprTxtSlide.swfTxtSlide.text = strSlide;\
         }\
      }\
   ");
   PrintActionScript(_T("ControlBar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function lecCheckSound()\
      {\
         var objRoot = _parent;\
         if (objRoot.g_bIsSoundOn)\
         {\
            swfSprSoundOn._visible = true;\
            swfSprSoundOff._visible = false;\
         }\
         else\
         {\
            swfSprSoundOn._visible = false;\
            swfSprSoundOff._visible = true;\
         }\
      }\
   ");
   PrintActionScript(_T("ControlBar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   // Preloader
   float progressbarHeight = fScale * 10.0f;
   float progressbarWidth = fScale * (98.0f - (0.33f * progressbarHeight));

   if (m_bIsSlidestar)
   {
	   progressbarHeight = 4.0f;
	   progressbarWidth = 196.0f;
	   csScript.Format(_T("\
		  function lecUpdatePreloader()\
		  {\
          var objRoot = _parent;\
          var percentLoadedSwf = objRoot.getBytesLoaded() / (1.0*objRoot.getBytesTotal());\
          if (percentLoadedSwf >= 1.0)\
            objRoot.g_bIsSwfLoaded = true;\
          else\
            objRoot.g_bIsSwfLoaded = false;\
          \
          var allBytesLoaded = 0;\
          var allBytesTotal = 0;\
          allBytesLoaded += objRoot.getBytesLoaded();\
          allBytesTotal += objRoot.getBytesTotal();\
          if (objRoot.g_bMovieHasVideo && !objRoot.g_bIsTrueStreaming)\
          {\
             if (objRoot.g_VideoStream.bytesTotal > 0)\
             {\
               allBytesLoaded += objRoot.g_VideoStream.bytesLoaded;\
               allBytesTotal += objRoot.g_VideoStream.bytesTotal;\
             }\
             else\
               allBytesTotal += 500000000;\
          }\
          if (objRoot.g_bMovieHasClips && !objRoot.g_bIsTrueStreaming)\
          {\
             for (var i = 0; i < objRoot.g_iNumOfClips; ++i)\
             {\
                if (objRoot.g_aClipStreams[i].bytesTotal > 0)\
                {\
                  allBytesLoaded += objRoot.g_aClipStreams[i].bytesLoaded;\
                  allBytesTotal += objRoot.g_aClipStreams[i].bytesTotal;\
                }\
                else\
                  allBytesTotal += 100000000;\
             }\
          }\
          var percentLoadedAll = allBytesLoaded / (1.0*allBytesTotal);\
          \
			 if (objRoot.g_bIsSwfLoaded && percentLoadedAll >= 1.0)\
			 {\
				objRoot.g_bIsPreloading = false;\
				swfSprProgressBar._visible = true;\
				swfSprProgressBar._width = Math.round(%g);\
			 }\
			 else\
			 {\
				objRoot.g_bIsPreloading = true;\
				swfSprProgressBar._visible = true;\
				swfSprProgressBar._width = Math.round(%g * percentLoadedAll);\
			   swfSprTxtTime.swfTxtTime.text = objRoot.g_strPreloadSlidestar;\
				swfSprTxtSlide.swfTxtSlide.text = \"\";\
			 }\
		  }\
	   "), progressbarWidth, progressbarWidth);
   }
   else
   {
   csScript.Format(_T("\
      function lecUpdatePreloader()\
      {\
         var objRoot = _parent;\
         var percentLoadedSwf = objRoot.getBytesLoaded() / (1.0*objRoot.getBytesTotal());\
         if (percentLoadedSwf >= 1.0)\
            objRoot.g_bIsSwfLoaded = true;\
         else\
            objRoot.g_bIsSwfLoaded = false;\
         \
         var allBytesLoaded = 0;\
         var allBytesTotal = 0;\
         allBytesLoaded += objRoot.getBytesLoaded();\
         allBytesTotal += objRoot.getBytesTotal();\
         if (objRoot.g_bMovieHasVideo && !objRoot.g_bIsTrueStreaming)\
         {\
            if (objRoot.g_VideoStream.bytesTotal > 0)\
            {\
               allBytesLoaded += objRoot.g_VideoStream.bytesLoaded;\
               allBytesTotal += objRoot.g_VideoStream.bytesTotal;\
            }\
            else\
               allBytesTotal += 500000000;\
         }\
         if (objRoot.g_bMovieHasClips && !objRoot.g_bIsTrueStreaming)\
         {\
            for (var i = 0; i < objRoot.g_iNumOfClips; ++i)\
            {\
               if (objRoot.g_aClipStreams[i].bytesTotal > 0)\
               {\
                  allBytesLoaded += objRoot.g_aClipStreams[i].bytesLoaded;\
                  allBytesTotal += objRoot.g_aClipStreams[i].bytesTotal;\
               }\
               else\
                  allBytesTotal += 100000000;\
            }\
         }\
         var percentLoadedAll = allBytesLoaded / (1.0*allBytesTotal);\
         \
         if (objRoot.g_bIsSwfLoaded && percentLoadedAll >= 1.0)\
         {\
            objRoot.g_bIsPreloading = false;\
            swfSprProgressBarL._visible = false;\
            swfSprProgressBarM._visible = false;\
         }\
         else\
         {\
            objRoot.g_bIsPreloading = true;\
            swfSprProgressBarL._visible = true;\
            swfSprProgressBarM._visible = true;\
            swfSprProgressBarM._width = Math.round(%g * percentLoadedAll);\
            swfSprTxtTime.swfTxtTime.text = objRoot.g_strPreload;\
            swfSprTxtSlide.swfTxtSlide.text = \"\";\
         }\
      }\
   "), progressbarWidth);
   }
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));
	
   if (m_bIsSlidestar)
   {
	   	   csScript.Format(_T("\
		  function lecUpdateBufferingPercentage()\
		  {\
			 var objRoot = _parent;\
			 var bufferLength = objRoot.g_VideoStream.bufferLength / objRoot.g_VideoStream.bufferTime;\
			 if (bufferLength >= 1)\
			 {\
				swfSprProgressBar._visible = true;\
				swfSprProgressBar._width = Math.round(%g);\
			 }\
			 else\
			 {\
				swfSprProgressBar._visible = true;\
				swfSprProgressBar._width = Math.round(%g * bufferLength);\
			 }\
		  }\
	   "), progressbarWidth, progressbarWidth);
   }
   else
   {
   csScript.Format(_T("\
      function lecUpdateBufferingPercentage()\
      {\
         var objRoot = _parent;\
         var bufferLength = objRoot.g_VideoStream.bufferLength / objRoot.g_VideoStream.bufferTime;\
         if (bufferLength >= 1)\
         {\
            swfSprProgressBarL._visible = false;\
            swfSprProgressBarM._visible = false;\
         }\
         else\
         {\
            swfSprProgressBarL._visible = true;\
            swfSprProgressBarM._visible = true;\
            swfSprProgressBarM._width = Math.round(%g * bufferLength);\
         }\
      }\
   "), progressbarWidth);   
   }
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));


   // Part IV: methods used for 'standard navigation'
  csScript = _T("\
      function enableControlBar(bEnable)\
      {\
         enableStandardButtons(bEnable);\
         enableTimeLine(bEnable);\
         enableTimeDisplay(bEnable);\
         /*Sound is always enabled*/\
         enableVolumeLine(true);\
         enableSoundOffButton(true);\
         enableSoundOnButton(true);\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function enableStandardButtons(bEnable)\
      {\
         enablePauseButton(bEnable);\
         enablePlayButton(bEnable);\
         enableGotoFirstFrameButton(bEnable);\
         enableGotoLastFrameButton(bEnable);\
         if (_parent.g_bMovieHasPages)\
         {\
            enableGotoPrevPageButton(bEnable);\
            enableGotoNextPageButton(bEnable);\
          }\
          else\
          {\
            enableGotoPrevPageButton(false);\
            enableGotoNextPageButton(false);\
          }\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function enableTimeLine(bEnable)\
      {\
         if (bEnable)\
         {\
            swfSprTimeSlider._visible=true;\
            swfSprTimeSlider_Disabled._visible=false;\
         }\
         else\
         {\
            swfSprTimeSlider._visible=false;\
            swfSprTimeSlider_Disabled._visible=true;\
         }\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function enableTimeDisplay(bEnable)\
      {\
         if (bEnable)\
         {\
            swfSprTxtTime._visible=true;\
            swfSprTxtSlide._visible=true;\
         }\
         else\
         {\
            swfSprTxtTime._visible=false;\
            swfSprTxtSlide._visible=false;\
         }\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));


   csScript = _T("\
      function enableVolumeLine(bEnable)\
      {\
         if (bEnable)\
         {\
            swfSprVolumeSlider._visible=true;\
            swfSprVolumeSlider_Disabled._visible=false;\
         }\
         else\
         {\
            swfSprVolumeSlider._visible=false;\
            swfSprVolumeSlider_Disabled._visible=true;\
         }\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function enableDocumentStructure(bEnable)\
      {\
         /*not implemented*/\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function enablePlayButton(bEnable)\
      {\
         if (bEnable)\
         {\
            swfSprPlay._visible=true;\
            swfSprPlay_Disabled._visible=false;\
         }\
         else\
         {\
            swfSprPlay._visible=false;\
            swfSprPlay_Disabled._visible=true;\
         }\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function enablePauseButton(bEnable)\
      {\
         if (bEnable)\
         {\
            swfSprPause._visible=true;\
            swfSprPause_Disabled._visible=false;\
         }\
         else\
         {\
            swfSprPause._visible=false;\
            swfSprPause_Disabled._visible=true;\
         }\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function enableGotoFirstFrameButton(bEnable)\
      {\
         if (bEnable)\
         {\
            swfSprGotoFirstFrame._visible=true;\
            swfSprGotoFirstFrame_Disabled._visible=false;\
         }\
         else\
         {\
            swfSprGotoFirstFrame._visible=false;\
            swfSprGotoFirstFrame_Disabled._visible=true;\
         }\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function enableGotoLastFrameButton(bEnable)\
      {\
         if (bEnable)\
         {\
            swfSprGotoLastFrame._visible=true;\
            swfSprGotoLastFrame_Disabled._visible=false;\
         }\
         else\
         {\
            swfSprGotoLastFrame._visible=false;\
            swfSprGotoLastFrame_Disabled._visible=true;\
         }\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function enableGotoPrevPageButton(bEnable)\
      {\
         if (bEnable)\
         {\
            swfSprGotoPrevPage._visible=true;\
            swfSprGotoPrevPage_Disabled._visible=false;\
         }\
         else\
         {\
            swfSprGotoPrevPage._visible=false;\
            swfSprGotoPrevPage_Disabled._visible=true;\
         }\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function enableGotoNextPageButton(bEnable)\
      {\
         if (bEnable)\
         {\
            swfSprGotoNextPage._visible=true;\
            swfSprGotoNextPage_Disabled._visible=false;\
         }\
         else\
         {\
            swfSprGotoNextPage._visible=false;\
            swfSprGotoNextPage_Disabled._visible=true;\
         }\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript =  _T("\
      function enableSoundOnButton(bEnable)\
      {\
         if (bEnable)\
         {\
            swfSprSoundOn._visible=true;\
            swfSprSoundOn_Disabled._visible=false;\
         }\
         else\
         {\
            swfSprSoundOn._visible=false;\
            swfSprSoundOn_Disabled._visible=true;\
         }\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function enableSoundOffButton(bEnable)\
      {\
         if (bEnable)\
         {\
            swfSprSoundOff._visible=true;\
            swfSprSoundOff_Disabled._visible=false;\
         }\
         else\
         {\
            swfSprSoundOff._visible=false;\
            swfSprSoundOff_Disabled._visible=true;\
         }\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function hideControlBar()\
      {\
         hideVolumeLine();\
         swfSprSoundOn._visible=false;\
         swfSprSoundOff._visible=false;\
         swfSprSoundOn_Disabled._visible=false;\
         swfSprSoundOff_Disabled._visible=false;\
         swfSprSeparator1._visible=false;\
         swfSprSeparator2._visible=false;\
         swfSprWebplayerLogo._visible=false;\
         swfSprControlbarBackground._visible=false;\
         this._visible=false;\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function hideStandardButtons()\
      {\
         swfSprPlay._visible=false;\
         swfSprPause._visible=false;\
         swfSprGotoFirstFrame._visible=false;\
         swfSprGotoLastFrame._visible=false;\
         swfSprGotoPrevPage._visible=false;\
         swfSprGotoNextPage._visible=false;\
         swfSprPlay_Disabled._visible=false;\
         swfSprPause_Disabled._visible=false;\
         swfSprGotoFirstFrame_Disabled._visible=false;\
         swfSprGotoLastFrame_Disabled._visible=false;\
         swfSprGotoPrevPage_Disabled._visible=false;\
         swfSprGotoNextPage_Disabled._visible=false;\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function hideTimeLine()\
      {\
         swfSprTimeSlider._visible=false;\
         swfSprTimeSlider_Disabled._visible=false;\
         swfSprTimeLine._visible=false;\
         swfActiveTimeLine._visible=false;\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function hideVolumeLine()\
      {\
         swfSprVolumeSlider._visible=false;\
         swfSprVolumeSlider_Disabled._visible=false;\
         swfSprVolumeLine._visible=false;\
         swfActiveVolumeLine._visible=false;\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   csScript = _T("\
      function hideTimeDisplay()\
      {\
         swfSprTextFieldBoxTime._visible=false;\
         swfSprTextFieldBoxSlide._visible=false;\
         swfSprTxtTime._visible=false;\
         swfSprTxtSlide._visible=false;\
      }\
      \
      function hideDocumentStructure()\
      {\
         /*not implemented*/\
      }\
   ");
   PrintActionScript(_T("Control Bar"), (TCHAR *)(LPCTSTR)csScript);
   sprControl->add(new SWFAction(GetCharFromCString(csScript)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForPageSprite(SWFSprite *sprPages)
{
   _TCHAR script[1024];

   _stprintf(script, _T("\
        stop();\
   "));
   PrintActionScript("Movie", script);
   sprPages->add(new SWFAction(GetCharFromCString(script)));

   return S_OK;
}

UINT CFlashEngine::AddActionScriptForNetConnectionToMovie(SWFMovie *movie, CString csNetConnection)
{
   HRESULT hr = S_OK;

   // The Net Connection String has to be set in quotes (if not empty --> 'null'):
   if (!csNetConnection.IsEmpty())
      csNetConnection.Format(_T("\"%s\""), csNetConnection);
   else
      csNetConnection.Format(_T("%s"), _T("null"));

   // If csNetConnection begins with "rtmp" we have real streaming
   // and 'g_bIsTrueStreaming' hat to be set to 'true' (else 'false').
   CString csBool;
   csBool.Format(_T("false"));

   CString csTempNC(csNetConnection);
   csTempNC.MakeLower();
   if (csTempNC.Find(_T("rtmp://")) > 0)
      csBool.Format(_T("true"));
   
   char *utfBool = LString::TCharToUtf8(csBool);
   char *utfNetConnection = LString::TCharToUtf8(csNetConnection);

   _TCHAR script[1024];

#ifdef _UNICODE
   _stprintf(script, _T("\
      function lecSetNetConnection()\
      {\
         g_bIsTrueStreaming = %S;\
         \
         var sNetConnection = %S;\
         g_NetConnection = new NetConnection();\
         if (streamUrl.length > 0)\
         {\
            sNetConnection = streamUrl;\
            if (streamUrl == \"null\")\
            {\
               g_NetConnection.connect(null);\
               g_bIsTrueStreaming = false;\
            }\
            else\
            {\
               g_NetConnection.connect(streamUrl);\
               g_bIsTrueStreaming = true;\
            }\
         }\
         else\
            g_NetConnection.connect(sNetConnection);\
         \
         lecCheckNetConnection(g_NetConnection, sNetConnection);\
      }\
   "), utfBool, utfNetConnection);
#else
   _stprintf(script, _T("\
      function lecSetNetConnection()\
      {\
         g_bIsTrueStreaming = %s;\
         \
         var sNetConnection = %s;\
         g_NetConnection = new NetConnection();\
         if (streamUrl.length > 0)\
         {\
            sNetConnection = streamUrl;\
            if (streamUrl == \"null\")\
            {\
               g_NetConnection.connect(null);\
               g_bIsTrueStreaming = false;\
            }\
            else\
            {\
               g_NetConnection.connect(streamUrl);\
               g_bIsTrueStreaming = true;\
            }\
         }\
         else\
            g_NetConnection.connect(sNetConnection);\
         \
         lecCheckNetConnection(g_NetConnection, sNetConnection);\
      }\
   "), utfBool, utfNetConnection);
#endif //_UNICODE

   if (utfBool)
      delete[] utfBool;
   if (utfNetConnection)
      delete[] utfNetConnection;

   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecCheckNetConnection(netConnection, strServerUrl)\
      {\
         netConnection.onStatus = function(infoObject)\
         {\
            if (infoObject.code != \"NetConnection.Connect.Success\")\
            {\
               /*getURL(\"javascript:alert('NetConnection Error!')\", \"_self\");*/\
               var strMsgType = g_strError;\
               var strMsgText = g_strServerError;\
               strMsgText += \"<br>\";\
               strMsgText += strServerUrl;\
               if (g_bIsTrueStreaming)\
                  openMessageBox(strMsgType, strMsgText);\
            }\
         };\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForFlvStreamsToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   _TCHAR script[1024];
   _stprintf(script, _T("\
      function lecCheckVideoFlvStream(videoStream, strFlvUrl)\
      {\
         videoStream.onStatus = function(infoObject)\
         {\
            switch (infoObject.code)\
            {\
               case \"NetStream.Buffer.Empty\":\
                  videoStream.setBufferTime(g_nStartBufferLength);\
                  break;\
               case \"NetStream.Buffer.Full\":\
                  videoStream.setBufferTime(g_nExpandedBufferLength);\
                  break;\
               case \"NetStream.Play.StreamNotFound\":\
                  /*getURL(\"javascript:alert('Video Error: StreamNotFound!')\", \"_self\");*/\
                  var strMsgType = g_strError;\
                  var strMsgText = g_strFlvNotFoundError;\
                  strMsgText += \"<br>\";\
                  strMsgText += strFlvUrl;\
                  openMessageBox(strMsgType, strMsgText);\
                  break;\
            }\
         };\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecCheckClipFlvStream(clipStream, strFlvUrl)\
      {\
         clipStream.onStatus = function(infoObject)\
         {\
            switch (infoObject.code)\
            {\
               case \"NetStream.Buffer.Empty\":\
                  clipStream.setBufferTime(g_nStartBufferLength);\
                  break;\
               case \"NetStream.Buffer.Full\":\
                  clipStream.setBufferTime(g_nExpandedBufferLength);\
                  break;\
               case \"NetStream.Play.StreamNotFound\":\
                  /*getURL(\"javascript:alert('Clip Error: StreamNotFound!')\", \"_self\");*/\
                  var strMsgType = g_strError;\
                  var strMsgText = g_strFlvNotFoundError;\
                  strMsgText += \"<br>\";\
                  strMsgText += strFlvUrl;\
                  openMessageBox(strMsgType, strMsgText);\
                  break;\
            }\
         };\
      }\
      \
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecCheckVideoFlvFile(videoStream, strFlvUrl)\
      {\
         if (!g_bIsTrueStreaming && videoStream.bytesTotal <= 0)\
         {\
            var strMsgType = g_strError;\
            var strMsgText = g_strFlvNotFoundError;\
            strMsgText += \"<br>\";\
            strMsgText += strFlvUrl;\
            openMessageBox(strMsgType, strMsgText);\
         }\
      }\
      \
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecCheckClipFlvFile(clipStream, strFlvUrl)\
      {\
         if (!g_bIsTrueStreaming && clipStream.bytesTotal <= 0)\
         {\
            var strMsgType = g_strError;\
            var strMsgText = g_strFlvNotFoundError;\
            strMsgText += \"<br>\";\
            strMsgText += strFlvUrl;\
            openMessageBox(strMsgType, strMsgText);\
         }\
      }\
      \
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecCheckFlvSuffix(flvName)\
      {\
         var newFlvName = new String(flvName);\
         \
         if (streamStripFlvSuffix == \"false\")\
         {\
            return newFlvName;\
         }\
         else\
         {\
            var nIdx = newFlvName.indexOf(\".flv\");\
            \
            if (g_bIsTrueStreaming)\
            {\
               if (nIdx > -1)\
                  return newFlvName.substring(0, nIdx);\
            }\
            else\
            {\
               if (nIdx == -1)\
               {\
                  newFlvName = newFlvName + \".flv\";\
                  return newFlvName;\
               }\
            }\
         }\
         \
         return newFlvName;\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForVideoStreamUrlToMovie(SWFMovie *movie, CString csVideoStreamUrl)
{
   HRESULT hr = S_OK;

   // Action script: initialize video stream
   _TCHAR script[2048];

   char *utfVideoStreamUrl = LString::TCharToUtf8(csVideoStreamUrl);
   float fVideoLengthSec = (float)m_iVideoLengthMs / 1000.0f;

#ifdef _UNICODE
   _stprintf(script, _T("\
      function lecSetVideoParameters()\
      {\
         g_sVideoStreamUrl = baseUrl + \"%S\";\
         if (videoFlvName.length > 0)\
         {\
            var adaptedVideoFlvName = lecCheckFlvSuffix(videoFlvName);\
            g_sVideoStreamUrl = baseUrl + adaptedVideoFlvName;\
         }\
         g_fVideoStreamLengthSec = %f;\
      }\
      \
      function lecInitVideoStream()\
      {\
         g_VideoStream = new NetStream(g_NetConnection);\
         swfSprVideo.accVideo.attachVideo(g_VideoStream);\
         g_VideoStream.setBufferTime(g_nStartBufferLength);\
         g_VideoStream.play(g_sVideoStreamUrl);\
         g_VideoStream.seek(0);\
         g_VideoStream.pause(true);\
         \
         lecCheckVideoFlvStream(g_VideoStream, g_sVideoStreamUrl);\
         g_bufferingInterval = 0;\
         \
         g_videoBufferInterval = setInterval(lecCheckVideoBuffer, 125, g_sVideoStreamUrl);\
      }\
   "), utfVideoStreamUrl, fVideoLengthSec);
#else
   _stprintf(script, _T("\
      function lecSetVideoParameters()\
      {\
         g_sVideoStreamUrl = baseUrl + \"%s\";\
         if (videoFlvName.length > 0)\
         {\
            var adaptedVideoFlvName = lecCheckFlvSuffix(videoFlvName);\
            g_sVideoStreamUrl = baseUrl + adaptedVideoFlvName;\
         }\
         g_fVideoStreamLengthSec = %f;\
      }\
      \
      function lecInitVideoStream()\
      {\
         g_VideoStream = new NetStream(g_NetConnection);\
         swfSprVideo.accVideo.attachVideo(g_VideoStream);\
         g_VideoStream.setBufferTime(g_nStartBufferLength);\
         g_VideoStream.play(g_sVideoStreamUrl);\
         g_VideoStream.seek(0);\
         g_VideoStream.pause(true);\
         \
         lecCheckVideoFlvStream(g_VideoStream, g_sVideoStreamUrl);\
         g_bufferingInterval = 0;\
         \
         g_videoBufferInterval = setInterval(lecCheckVideoBuffer, 125, g_sVideoStreamUrl);\
      }\
   "), utfVideoStreamUrl, fVideoLengthSec);
#endif //_UNICODE

   if (utfVideoStreamUrl)
      delete[] utfVideoStreamUrl;

   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecCheckVideoBuffer(sVideoStreamUrl)\
      {\
         if (g_bWillBePlaying && g_VideoStream.bufferLength <= 0)\
         {\
            g_nVideoBufferTries++;\
            \
            if (g_bIsTrueStreaming)\
            {\
               if (g_nVideoBufferTries > 80)\
               {\
                  clearInterval(g_videoBufferInterval);\
                  g_videoBufferInterval = 0;\
                  g_nVideoBufferTries = 0;\
                  lecDoStop();\
                  \
                  var strMsgType = g_strError;\
                  var strMsgText = g_strFlvStreamingError;\
                  strMsgText += \"<br>\";\
                  strMsgText += g_sVideoStreamUrl;\
                  openMessageBox(strMsgType, strMsgText);\
               }\
            }\
            else\
            {\
               if (g_nVideoBufferTries > 4)\
               {\
                  g_nVideoBufferTries = 0;\
                  lecGotoLastLoadedVideoBuffer();\
               }\
            }\
         }\
         else\
         {\
            g_nVideoBufferTries = 0;\
         }\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecGotoLastLoadedVideoBuffer()\
      {\
         var bytesLoaded = g_VideoStream.bytesLoaded;\
         var bytesTotal = g_VideoStream.bytesTotal;\
         var pctLoaded = 0.0;\
         if (bytesTotal > 0)\
            pctLoaded = 1.0 * (bytesLoaded / (1.0*bytesTotal));\
         if (pctLoaded >= 1.0)\
            return;\
         \
         var curFrame = _currentframe;\
         var newFrame = Math.round(pctLoaded * _totalframes) + 1;\
         \
         if (newFrame < curFrame)\
         {\
            /* Jump 10 seconds before the current loaded frame */\
            newFrame -= Math.round(g_fFrameRate * 10.0);\
            if (newFrame < 1)\
               newFrame = 1;\
            \
            if (g_bWillBePlaying || g_bIsPlaying)\
               lecDoGotoAndPlay(newFrame);\
            else\
               lecDoGotoAndStop(newFrame);\
         }\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForVideoStreamingToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   _TCHAR script[4096];

   // Action script: Check video status
   double dVideoLength = (double)m_iVideoLengthMs / (double)1000.0f;
   _stprintf(script, _T("\
      function lecCheckVideoStatus(actualTime)\
      {\
         var pctBuffer = Math.min(Math.round((g_VideoStream.bufferLength / g_VideoStream.bufferTime) * 100), 100);\
         \
         g_bVideoIsBuffering = false;\
         if (pctBuffer < 100)\
         {\
            if ((g_bIsTrueStreaming) && (actualTime < (g_fVideoStreamLengthSec - g_VideoStream.bufferTime)))\
            {\
               g_bVideoIsBuffering = true;\
            }\
         }\
      }\
   "));
   PrintActionScript("Movie", script);

   movie->add(new SWFAction(GetCharFromCString(script)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForClipStreamUrlsToMovie(SWFMovie *movie, CString *acsClipStreamUrls)
{
   HRESULT hr = S_OK;

   CString csScript;
   CString csSubScript;

   // Init Clip Streams array
   csScript.Format(_T("\
      function lecSetClipParameters()\
      {\
         g_iActiveClipNumber = -1;\
         \
         g_aClipStreams = new Array(g_iNumOfClips);\
         g_aClipStreamUrls = new Array(g_iNumOfClips);\
         g_aClipBeginTimesSec = new Array(g_iNumOfClips);\
         g_aClipEndTimesSec = new Array(g_iNumOfClips);\
         g_aClipSprites = new Array(g_iNumOfClips);\
         g_aClipBufferInterval = new Array(g_iNumOfClips);\
         g_aClipBufferTries = new Array(g_iNumOfClips);\
         \
         lecInitClips();\
      }\
   "));
   PrintActionScript("Movie", (_TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript.Format(_T("\
      function lecInitClips()\
      {"));

   for (int i = 0; i < m_iNumOfClips; ++i)
   {
      csSubScript.Format(_T("lecInitClip%d();"), i);
      csScript += csSubScript;
   }
   csScript += _T("}");

   PrintActionScript("Movie", (_TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   csScript.Format(_T("\
      function lecInitClipStreams()\
      {"));

   for (int i = 0; i < m_iNumOfClips; ++i)
   {
      csSubScript.Format(_T("lecInitClipStream%d();"), i);
      csScript += csSubScript;
   }
   csScript += _T("}");

   PrintActionScript("Movie", (_TCHAR *)(LPCTSTR)csScript);
   movie->add(new SWFAction(GetCharFromCString(csScript)));

   for (int i = 0; i < m_iNumOfClips; ++i)
   {
      char *utfClipStreamUrl = LString::TCharToUtf8(acsClipStreamUrls[i]);
      double dClipBeginSec = (double)m_alClipsBeginMs[i] / (double)1000.0f;
      double dClipEndSec = (double)m_alClipsEndMs[i] / (double)1000.0f;

#ifdef _UNICODE
      csScript.Format(_T("\
         function lecInitClip%d()\
         {\
            g_aClipStreamUrls[%d] = baseUrl + \"%S\";\
            if (clip%dFlvName.length > 0)\
            {\
               var adaptedClipFlvName = lecCheckFlvSuffix(clip%dFlvName);\
               g_aClipStreamUrls[%d] = baseUrl + adaptedClipFlvName;\
            }\
            g_aClipBeginTimesSec[%d] = %f;\
            g_aClipEndTimesSec[%d] = %f;\
            g_aClipBufferInterval[%d] = 0;\
            g_aClipBufferTries[%d] = 0;\
         }\
         \
         function lecInitClipStream%d()\
         {\
            g_aClipStreams[%d] = new NetStream(g_NetConnection);\
            g_aClipSprites[%d] = swfSprClip%d;\
            g_aClipSprites[%d].Clip%d.attachVideo(g_aClipStreams[%d]);\
            g_aClipSprites[%d]._visible = false;\
            g_aClipStreams[%d].setBufferTime(g_nStartBufferLength);\
            g_aClipStreams[%d].play(g_aClipStreamUrls[%d]);\
            g_aClipStreams[%d].seek(0);\
            g_aClipStreams[%d].pause(true);\
            \
            lecCheckClipFlvStream(g_aClipStreams[%d], g_aClipStreamUrls[%d]);\
            g_nClipIndex = %d;\
            g_aClipBufferInterval[%d] = setInterval(lecCheckClipBuffer, 125, g_nClipIndex, g_aClipStreamUrls[%d]);\
         }\
      "), i, i, utfClipStreamUrl, i, i, i, i, (float)dClipBeginSec, i, (float)dClipEndSec,
          i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i);
#else
      csScript.Format(_T("\
         function lecInitClip%d()\
         {\
            g_aClipStreamUrls[%d] = baseUrl + \"%s\";\
            if (clip%dFlvName.length > 0)\
            {\
               var adaptedClipFlvName = lecCheckFlvSuffix(clip%dFlvName);\
               g_aClipStreamUrls[%d] = baseUrl + adaptedClipFlvName;\
            }\
            g_aClipBeginTimesSec[%d] = %f;\
            g_aClipEndTimesSec[%d] = %f;\
            g_aClipBufferInterval[%d] = 0;\
            g_aClipBufferTries[%d] = 0;\
         }\
         \
         function lecInitClipStream%d()\
         {\
            g_aClipStreams[%d] = new NetStream(g_NetConnection);\
            g_aClipSprites[%d] = swfSprClip%d;\
            g_aClipSprites[%d].Clip%d.attachVideo(g_aClipStreams[%d]);\
            g_aClipSprites[%d]._visible = false;\
            g_aClipStreams[%d].setBufferTime(g_nStartBufferLength);\
            g_aClipStreams[%d].play(g_aClipStreamUrls[%d]);\
            g_aClipStreams[%d].seek(0);\
            g_aClipStreams[%d].pause(true);\
            \
            lecCheckClipFlvStream(g_aClipStreams[%d], g_aClipStreamUrls[%d]);\
            g_nClipIndex = %d;\
            g_aClipBufferInterval[%d] = setInterval(lecCheckClipBuffer, 125, g_nClipIndex, g_aClipStreamUrls[%d]);\
         }\
      "), i, i, utfClipStreamUrl, i, i, i, i, (float)dClipBeginSec, i, (float)dClipEndSec,
          i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i);
#endif //_UNICODE

      if (utfClipStreamUrl)
         delete[] utfClipStreamUrl;

      PrintActionScript("Movie", (_TCHAR *)(LPCTSTR)csScript);
      movie->add(new SWFAction(GetCharFromCString(csScript)));
   }

   _TCHAR script[4096];
   _stprintf(script, _T("\
      function lecCheckClipBuffer(idx, sClipStreamUrl)\
      {\
         if (g_bWillBePlaying && g_aClipStreams[idx].bufferLength <= 0)\
         {\
            g_aClipBufferTries[idx]++;\
            \
            if (g_bIsTrueStreaming)\
            {\
               if (g_aClipBufferTries[idx] > 80)\
               {\
                  clearInterval(g_aClipBufferInterval[idx]);\
                  g_aClipBufferInterval[idx] = 0;\
                  g_aClipBufferTries[idx] = 0;\
                  lecDoStop();\
                  \
                  var strMsgType = g_strError;\
                  var strMsgText = g_strFlvStreamingError;\
                  strMsgText += \"<br>\";\
                  strMsgText += sClipStreamUrl;\
                  openMessageBox(strMsgType, strMsgText);\
               }\
            }\
            else\
            {\
               if (g_aClipBufferTries[idx] > 4)\
               {\
                  g_aClipBufferTries[idx] = 0;\
                  lecGotoLastLoadedClipBuffer(idx);\
               }\
            }\
         }\
         else\
         {\
            g_aClipBufferTries[idx] = 0;\
         }\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   _stprintf(script, _T("\
      function lecGotoLastLoadedClipBuffer(idx)\
      {\
         var bytesLoaded = g_aClipStreams[idx].bytesLoaded;\
         var bytesTotal = g_aClipStreams[idx].bytesTotal;\
         var pctLoaded = 0.0;\
         if (bytesTotal > 0)\
            pctLoaded = 1.0 * (bytesLoaded / (1.0*bytesTotal));\
         if (pctLoaded >= 1.0)\
            return;\
         \
         var clipLengthSec = g_aClipEndTimesSec[idx] - g_aClipBeginTimesSec[idx];\
         var clipTimeSec = g_aClipBeginTimesSec[idx] + Math.round(pctLoaded * clipLengthSec);\
         \
         var curFrame = _currentframe;\
         var newFrame = lecGetFrameFromTimeSec(clipTimeSec);\
         \
         if (newFrame < curFrame)\
         {\
            /* Jump 10 seconds before the current loaded frame */\
            newFrame -= Math.round(g_fFrameRate * 10.0);\
            if (newFrame < 1)\
               newFrame = 1;\
            \
            if (g_bWillBePlaying || g_bIsPlaying)\
               lecDoGotoAndPlay(newFrame);\
            else\
               lecDoGotoAndStop(newFrame);\
         }\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   return hr;
}

UINT CFlashEngine::AddActionScriptForClipStreamingToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   _TCHAR script[4096];

   // Action script: Check clip status
   double dVideoLength = (double)m_iVideoLengthMs / (double)1000.0f;
   _stprintf(script, _T("\
      function lecCheckClipStatus(netStream, startTimeSec, endTimeSec)\
      {\
         var pctBuffer = Math.min(Math.round((netStream.bufferLength / netStream.bufferTime) * 100), 100);\
         \
         g_bClipIsBuffering = false;\
         if (g_bIsTrueStreaming && (pctBuffer < 100))\
         {\
            var clipLength = endTimeSec - startTimeSec;\
            var clipEndPoint = clipLength - netStream.bufferTime;\
            if (g_bIsTrueStreaming && clipEndPoint > 0 && (netStream.time < clipEndPoint))\
            {\
               g_bClipIsBuffering = true;\
            }\
         }\
      }\
   "));
   PrintActionScript("Movie", script);

   movie->add(new SWFAction(GetCharFromCString(script)));

   // Check Status of all clips
   // Check Status of all clips
   _stprintf(script, _T("\
      function lecCheckAllClipsStatus(time, bDoSeek)\
      {\
         lecCheckActiveClip(time, bDoSeek);\
         \
         if (g_iActiveClipNumber >= 0)\
         {\
            lecCheckClipStatus(g_aClipStreams[g_iActiveClipNumber], \
                               g_aClipBeginTimesSec[g_iActiveClipNumber], \
                               g_aClipEndTimesSec[g_iActiveClipNumber]);\
         }\
         else\
         {\
            g_bClipIsBuffering = false;\
         }\
      }\
      \
   "));
   PrintActionScript("Movie", script);

   movie->add(new SWFAction(GetCharFromCString(script)));
   
   _stprintf(script, _T("\
      function lecCheckActiveClip(time, bDoSeek)\
      {\
         var nCurrentTimeSec = time;\
         \
         var currentActiveClipNumber = lecGetActiveClipNumber(nCurrentTimeSec);\
         \
         if (currentActiveClipNumber != g_iActiveClipNumber)\
         {\
            if (g_iActiveClipNumber >= 0)\
            {\
               g_aClipStreams[g_iActiveClipNumber].seek(0);\
               g_aClipStreams[g_iActiveClipNumber].pause(true);\
            }\
            if (currentActiveClipNumber >= 0)\
            {\
               if (bDoSeek)\
               {\
                  var nClipTime = nCurrentTimeSec - g_CurrentClipOffset;\
                  var fTimeDiff = Math.abs(g_CurrentClipStream.time - nClipTime);\
                  if (fTimeDiff > g_fClipSynchronizationDelta)\
                  {\
                     if (nClipTime >= g_fClipSynchronizationDelta)\
                        lecDoStreamSeek(g_CurrentClipStream, nClipTime, true);\
                     else\
                        lecDoStreamSeek(g_CurrentClipStream, 0, true);\
                  }\
               }\
               if (g_bIsPlaying)\
                  g_CurrentClipStream.pause(false);\
            }\
            \
            g_iActiveClipNumber = currentActiveClipNumber;\
         }\
      }\
   "));
   PrintActionScript("Movie", script);
   movie->add(new SWFAction(GetCharFromCString(script)));

   // Get the active clip number at given timestamp
   // and make it visible
   // --> returns -1, if no clip is active/visible
   _stprintf(script, _T("\
      function lecGetActiveClipNumber(timeSec)\
      {\
         var activeClipNumber = -1;\
         for (var i = 0; i < g_iNumOfClips; ++i)\
         {\
            if ((timeSec >= g_aClipBeginTimesSec[i]) && (timeSec < g_aClipEndTimesSec[i]))\
            {\
               activeClipNumber = i;\
               g_aClipSprites[i]._visible = true;\
            }\
            else\
            {\
               g_aClipSprites[i]._visible = false;\
            }\
         }\
         \
         if (activeClipNumber >= 0)\
         {\
            g_CurrentClipStream = g_aClipStreams[activeClipNumber];\
            g_CurrentClipStreamUrl = g_aClipStreamUrls[activeClipNumber];\
            g_CurrentClipOffset = g_aClipBeginTimesSec[activeClipNumber];\
            g_CurrentClipLength = g_aClipEndTimesSec[activeClipNumber] - g_aClipBeginTimesSec[activeClipNumber];\
         }\
         else\
         {\
            g_CurrentClipStream = null;\
            g_CurrentClipStreamUrl = \"\";\
            g_CurrentClipOffset = 0.0;\
            g_CurrentClipLength = 0.0;\
         }\
         \
         return activeClipNumber;\
      }\
   "));
   PrintActionScript("Movie", script);

   movie->add(new SWFAction(GetCharFromCString(script)));

   return hr;
}

void CFlashEngine::FillFeedbackEntry(SWFMovie *movie, CFeedbackDefinition *pFeedback, int qIdx, 
                                     CString csAction, bool bEnabled, bool bIsQuestionnaire)
{
   CArray< DrawSdk::DrawObject *, DrawSdk::DrawObject *> aDrawObjects;
   aDrawObjects.RemoveAll();
   aDrawObjects.Append(pFeedback->GetObjects());

   int nX = -1;
   int nY = -1;
   int nWidth = 0;
   int nHeight = 0;
   int nOkWidth = 0;
   int nOkHeight = 0;
   CString csTextPassed = _T("");
   CString csText = _T("");
   CString csIsQuestionnaire = bIsQuestionnaire ? _T("true") : _T("false");
   CString csColorBorder = _T("");
   CString csColorBackground = _T("");
   CString csColorText = _T("");
   CString csFontFamily = _T("");
   int nFontSizeHeader = 65536;
   int nFontSize = 65536;
   int nFontSizeTable = 65536;
   bool bIsFontSizeHeaderSet = false;
   bool bIsFontSizeSet = false;
   bool bIsFontSizeTableSet = false;
   CString csFontStyle = _T("");
   CString csFontWeight = _T("");
   CString csFeedbackType = pFeedback->GetType();
   int nType = GetIndexFromFeedbackType(csFeedbackType);
   CString csIsEnabled = bEnabled ? _T("true") : _T("false");

   if (aDrawObjects.GetSize() > 0)
   {
      CInteractionArea *pOkBtn = pFeedback->GetOkButton();
      if (pOkBtn != NULL)
      {
         // Store one 'OK' button for all feedbacks
         if (m_pOkButton == NULL)
            m_pOkButton = pOkBtn;

         nOkWidth = (int)m_pOkButton->GetActivityArea().Width();
         nOkHeight = (int)m_pOkButton->GetActivityArea().Height();
      }

      for (int i = 0; i < aDrawObjects.GetSize(); ++i)
      {
         DrawSdk::DrawObject *pObject = aDrawObjects.ElementAt(i);
         if (pObject->GetType() == DrawSdk::RECTANGLE)
         {
            DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle*)pObject;
            // Get the size of the message box - it is the largest object
            if((pObject->GetWidth() >= nWidth) && (pObject->GetHeight() >= nHeight))
            {
               nX = pObject->GetX();
               nY = pObject->GetY();
               nWidth = pObject->GetWidth();
               nHeight = pObject->GetHeight();
               if (pRectangle->GetIsFilled())
               {
                  Gdiplus::ARGB clrBackground = pRectangle->GetOutlineColor();
                  csColorBackground.Format(_T("%08x"), clrBackground);
                  csColorBackground = _T("0x") + csColorBackground.Mid(2); // cutoff alpha byte
                  //_tprintf(_T("csColorBackground: %s (%d)\n"), csColorBackground, clrBackground);
               }
               if (!pRectangle->GetIsFilled())
               {
                  Gdiplus::ARGB clrBorder = pRectangle->GetOutlineColor();
                  csColorBorder.Format(_T("%08x"), clrBorder);
                  csColorBorder = _T("0x") + csColorBorder.Mid(2); // cutoff alpha byte
                  //_tprintf(_T("csColorBorder: %s (%d)\n"), csColorBorder, clrBorder);
               }
            }
         }
         if (pObject->GetType() == DrawSdk::TEXT)
         {
            // Get the text properties
            DrawSdk::Text *pText = (DrawSdk::Text*)pObject;
            // Special case: QQ_EVAL
            if (nType == FEEDBACK_TYPE_QQ_EVAL)
            {
               if (pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_QQ_PASSED)
               {
                  //csTextPassed = pText->GetString();
                  csTextPassed.Format(_T("%s"), pText->GetString());
                  m_csFeedbackMsg += csTextPassed;
               }
               if (pText->GetTextType() == DrawSdk::TEXT_TYPE_EVAL_QQ_FAILED)
               {
                  //csText = pText->GetString();
                  csText.Format(_T("%s"), pText->GetString());
                  m_csFeedbackMsg += csText;
               }
            }
            else
            {
               //csText = pText->GetString();
               csText.Format(_T("%s"), pText->GetString());
               m_csFeedbackMsg += csText;
            }
            if (csColorText.IsEmpty())
            {
               Gdiplus::ARGB clrText = pText->GetOutlineColor();
               csColorText.Format(_T("%08x"), clrText);
               csColorText = _T("#") + csColorText.Mid(2); // cutoff alpha byte
            }
            if (csFontFamily.IsEmpty())
               csFontFamily = pText->GetLogFont().lfFaceName;
            if (csFontStyle.IsEmpty())
               csFontStyle = (pText->GetLogFont().lfItalic) ? _T("italic") : _T("normal");
            if (csFontWeight.IsEmpty())
               csFontWeight = (pText->GetLogFont().lfWeight == 700) ? _T("bold") : _T("normal");
            // Get 3 different font sizes for Evaluation feedback (or 1 for 'normal' feedbacks)
            int fntSize = (int)(-1.0f * pText->GetLogFont().lfHeight);
            if ( (!bIsFontSizeHeaderSet) && (fntSize < nFontSizeHeader) )
            {
               nFontSizeHeader = fntSize;
               nFontSize = fntSize;
               nFontSizeTable = fntSize;
               bIsFontSizeHeaderSet = true;
            }
            if ( (!bIsFontSizeSet) && (fntSize < nFontSize) )
            {
               nFontSize = fntSize;
               nFontSizeTable = fntSize;
               bIsFontSizeSet = true;
            }
            if ( (!bIsFontSizeTableSet) && (fntSize < nFontSizeTable) )
            {
               nFontSizeTable = fntSize;
               bIsFontSizeHeaderSet = true;
            }
         }
      }
   }

   // The text strings have to be converted into UTF8
   char *utfTextPassed = LString::TCharToUtf8(csTextPassed);
   char *utfText = LString::TCharToUtf8(csText);

   // The action string has to be reinterpreted
   csAction = ReinterpretActionStringForFlash(csAction, -1);

   // Use default values if font size detemination fails
   if (nFontSizeHeader > 65535)
      nFontSizeHeader = 16;
   if (nFontSize > 65535)
      nFontSize = 14;
   if (nFontSizeTable > 65535)
      nFontSizeTable = 12;

   _TCHAR script[4096];
   if ((utfTextPassed) && (utfText))
   {
#ifdef _UNICODE
      _stprintf(script, _T("\
         if (!g_bIsAlreadyLoaded)\
         {\
            var fbEntry = new FeedbackEntry(%d, %d, %d, %d, %s, %d, %d, %s, '%S', '%S', '%s', '%s', '%s', '%s', '%s', %d, %d, %d, '%s', '%s', %d, %d);\
            g_aFeedbackEntries.push(fbEntry);\
         }\
      "), nX, nY, nWidth, nHeight, csIsQuestionnaire, qIdx, nType, csIsEnabled, utfTextPassed, utfText, csAction, 
      csColorBorder, csColorBackground, csColorText, csFontFamily, nFontSizeHeader, nFontSize, nFontSizeTable, csFontStyle, csFontWeight, 
      nOkWidth, nOkHeight);
#else 
      _stprintf(script, _T("\
         if (!g_bIsAlreadyLoaded)\
         {\
            var fbEntry = new FeedbackEntry(%d, %d, %d, %d, %s, %d, %d, %s, '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, %d, '%s', '%s', %d, %d);\
            g_aFeedbackEntries.push(fbEntry);\
         }\
      "), nX, nY, nWidth, nHeight, csIsQuestionnaire, qIdx, nType, csIsEnabled, utfTextPassed, utfText, csAction, 
      csColorBorder, csColorBackground, csColorText, csFontFamily, nFontSizeHeader, nFontSize, nFontSizeTable, csFontStyle, csFontWeight, 
      nOkWidth, nOkHeight);
#endif //_UNICODE
      delete[] utfTextPassed;
      utfTextPassed = NULL;
      delete[] utfText;
      utfText = NULL;
   }
   PrintActionScript(_T("Movie"), script);
   movie->add(new SWFAction(GetCharFromCString(script)));
}

int CFlashEngine::GetIndexFromFeedbackType(CString csFeedbackType)
{
   int nType = FEEDBACK_TYPE_NOTHING;
   
   if (csFeedbackType.Find(_T("q-correct")) == 0)
      nType = FEEDBACK_TYPE_Q_CORRECT;
   if (csFeedbackType.Find(_T("q-wrong")) == 0)
      nType = FEEDBACK_TYPE_Q_WRONG;
   if (csFeedbackType.Find(_T("q-repeat")) == 0)
      nType = FEEDBACK_TYPE_Q_REPEAT;
   if (csFeedbackType.Find(_T("q-time")) == 0)
      nType = FEEDBACK_TYPE_Q_TIME;
   if (csFeedbackType.Find(_T("q-tries")) == 0)
      nType = FEEDBACK_TYPE_Q_TRIES;
   if (csFeedbackType.Find(_T("qq-passed")) == 0)
      nType = FEEDBACK_TYPE_QQ_PASSED;
   if (csFeedbackType.Find(_T("qq-failed")) == 0)
      nType = FEEDBACK_TYPE_QQ_FAILED;
   if (csFeedbackType.Find(_T("qq-eval")) == 0)
      nType = FEEDBACK_TYPE_QQ_EVAL;

   return nType;
}

int CFlashEngine::GetPageFromInteractionObject(DrawSdk::DrawObject *pDrawObject)
{
   // Get current page of activity (activity = visibility)
   CString csVisibility = pDrawObject->GetVisibility();
   if (csVisibility.GetLength() > 0)
   {
      CArray<CPoint, CPoint> aVisibilityTimes;
      StringManipulation::ExtractAllTimePeriodsFromString(csVisibility, &aVisibilityTimes);
      int iTimeMsActiveBegin  = aVisibilityTimes.ElementAt(0).x;
      int iTimeMsActiveEnd    = aVisibilityTimes.ElementAt(0).y;
      int iFrameActiveBegin  = GetFrameFromTimestamp(iTimeMsActiveBegin);
      int iFrameActiveEnd    = GetFrameFromTimestamp(iTimeMsActiveEnd);
      int iPage = GetPageFromActivityFrames(iFrameActiveBegin, iFrameActiveEnd);

      return iPage;
   }
   else
   {
      // Some interaction objects (e.g. button texts) have no activity
      return -1;
   }
}

int CFlashEngine::FillFeedbackSummaryStrings(CString &csResult, CString &csQuestion, CString &csAnswer, 
                                             CString &csPoints, CString &csMaximumPoints, 
                                             CString &csRequiredPoints, CString &csRequiredPercentage, 
                                             CString &csAchievedPoints, CString &csAchievedPercentage)
{
   // This method returns the font size of table elements
   int nFontSizeTable = 12;

   // Assuming only one Questionnaire/Testing
   CQuestionnaire *pQuestionnaire = m_aQuestionnaires[0];
   if (pQuestionnaire != NULL)
   {
      // Get the Feedbacks
      CArray<CFeedbackDefinition *, CFeedbackDefinition *> aFeedbacks;
      aFeedbacks.RemoveAll();
      aFeedbacks.Append(pQuestionnaire->GetFeedback());
      
      // Scan all feedbacks
      //_tprintf(_T(">>> Feedbacks: %d\n"), aFeedbacks.GetSize());
      for (int i = 0; i < aFeedbacks.GetSize(); ++i)
      {
         // Search for Evaluation Feedback
         CFeedbackDefinition *pFeedback = aFeedbacks.ElementAt(i);
         //_tprintf(_T(">>> %d. Feedback: Type %s\n"), i, pFeedback->GetType());
         if (pFeedback->GetType().Find(_T("qq-eval")) == 0)
         {
            // Get the DrawObjects from the Evaluation Feedback
            CArray<DrawSdk::DrawObject*, DrawSdk::DrawObject*> aDrawObjects;
            aDrawObjects.RemoveAll();
            aDrawObjects.Append(pFeedback->GetObjects());

            // Scan all objects
            int idxTextObject = 0;
            int iNumOfLines = 0;
            //_tprintf(_T(">>> Evaluation Feedback: %d DrawObjects\n"), aDrawObjects.GetSize());
            for (int k = 0; k < aDrawObjects.GetSize(); ++k)
            {
               // The required text objects are not marked in the EVQ file 
               // --> "Hack":
               // - Feedback Draw objects are always in a well defined order.
               // - The text objects of the question listing are surrounded by 
               //   horizontal lines and can be ignored.
               // - Changeable texts can also be ignored.

               // Get the line objects
               if (aDrawObjects.ElementAt(k)->GetType() == DrawSdk::LINE)
                  iNumOfLines++;

               // Get the text objects, which are not surrounded by lines 
               // and which are not changeable
               if ( (aDrawObjects.ElementAt(k)->GetType() == DrawSdk::TEXT)
                  && (iNumOfLines != 1) )
               {
                  DrawSdk::Text *pText = (DrawSdk::Text *)aDrawObjects.ElementAt(k);
                  if (!pText->HasChangeableType())
                  {
                     //_tprintf(_T(">>>  %s\n"), pText->GetString());
                     idxTextObject++;

                     switch (idxTextObject)
                     {
                        case 1: // Result
                           csResult = pText->GetString();
                           break;
                        case 2: // Question
                           csQuestion = pText->GetString();
                           break;
                        case 3: // Answer
                           csAnswer = pText->GetString();
                           break;
                        case 4: // Points
                           csPoints = pText->GetString();
                           break;
                        case 5: // Maximum Points
                           csMaximumPoints = pText->GetString();
                           nFontSizeTable = (int)(-1.0f * pText->GetLogFont().lfHeight);
                           break;
                        case 6: // Required Points
                           csRequiredPoints = pText->GetString();
                           break;
                        case 7: // Required Percentage
                           csRequiredPercentage = pText->GetString();
                           break;
                        case 8: // Achieved Points
                           csAchievedPoints = pText->GetString();
                           break;
                        case 9: // Achieved Percentage
                           csAchievedPercentage = pText->GetString();
                           break;
                        default:
                           break;
                     } // end switch
                  }
               }
            } // end k-loop
         }
      } // end i-loop
   }

   return nFontSizeTable;
}

void CFlashEngine::PrintActionScript(CString csObject, _TCHAR *script, bool bForceReleaseMode)
{
   // Debug info only
#ifdef _DEBUG
   _tprintf(_T("ActionScript (%s):\n%s\n\n"), csObject, script);
#else
   if (bForceReleaseMode)
      _tprintf(_T("ActionScript (%s):\n%s\n\n"), csObject, script);
#endif
}

// --------------------------------------------------------------------------
// WebPlayer templates stuff
// --------------------------------------------------------------------------

void CFlashEngine::DrawBorderLines(SWFShape *shpBorder, float x, float y, float width, float height, int fillFlag)
{
   // Define colors for the border lines (pseudo gradient)
   MYRGB rgbL[5];
   MYRGB rgbD[5];
   ChangeColorLightness(m_rgb100, rgbL[0], 1.35f);
   ChangeColorLightness(m_rgb100, rgbL[1], 1.25f);
   ChangeColorLightness(m_rgb100, rgbL[2], 1.17f);
   ChangeColorLightness(m_rgb100, rgbL[3], 1.10f);
   ChangeColorLightness(m_rgb100, rgbL[4], 1.05f);
   ChangeColorLightness(m_rgb100, rgbD[0], 0.70f);
   ChangeColorLightness(m_rgb100, rgbD[1], 0.75f);
   ChangeColorLightness(m_rgb100, rgbD[2], 0.80f);
   ChangeColorLightness(m_rgb100, rgbD[3], 0.85f);
   ChangeColorLightness(m_rgb100, rgbD[4], 0.90f);

   if (fillFlag == D3D_FILL)
   {
      // Define a fill gradient
      SWFFill *fillBorder = shpBorder->addGradientFill(m_swfLinearGradientBackground, SWFFILL_LINEAR_GRADIENT);
      shpBorder->setRightFill(fillBorder);
      fillBorder->rotate(90.0f);
      fillBorder->scale(height / 1500.0f);
      fillBorder->move(0.00f, 3.0f+(0.5f*height));
   }

   float dWidth = width; float dHeight = height;
   float dx = 0.0f; float dy = 0.0f;

   float delta = 0.6f;
///   float delta = 0.6f / m_dGlobalScale; // consider global scaling factor

   // Starting point is the bottom left corner
   x += 0.0f; 
   y += height;

   // Draw the border
   for (int i = 0; i < 5; ++i)
   {   
      shpBorder->movePenTo(x+dx, y-dy);
      shpBorder->setLine(1, rgbL[i].r, rgbL[i].g, rgbL[i].b);
      shpBorder->drawLine(0.0f, -dHeight);
      shpBorder->drawLine(dWidth, 0.0f);
      shpBorder->setLine(1, rgbD[i].r, rgbD[i].g, rgbD[i].b);
      shpBorder->drawLine(0.0f, dHeight);
      shpBorder->drawLineTo(x+dx, y-dy);
      dWidth -= (2.0f*delta); dHeight -= (2.0f*delta);
      dx += delta; dy += delta;
   }
}

void CFlashEngine::DrawControlbarBackground(SWFSprite *sprControl, float width, float height)
{
   SWFSprite *sprBorder = new SWFSprite();
   SWFShape *shpBorder = new SWFShape();

   DrawBorderLines(shpBorder, 0.0f, 0.0f, width, height, D3D_FILL);

   SWFDisplayItem *di = NULL;
   di = sprBorder->add(shpBorder);
   sprBorder->nextFrame();
   di = sprControl->add(sprBorder);
   di->setName("swfSprControlbarBackground");
}

 void CFlashEngine::DrawSlidestarControlbarBackground(SWFSprite *sprControl, float width, float height)
{

	SWFShape *shpBackground = new SWFShape();
	SWFFill *fillColor = shpBackground->addSolidFill(0, 0, 0, 255);
	shpBackground->setRightFill(fillColor);
	DrawRectangle(shpBackground, 0.0f, 0.0f, width, height);

	SWFDisplayItem *di = NULL;
	di = sprControl->add(shpBackground);
	di->setName("swfSprControlbarBackground");
}

void CFlashEngine::DrawSlidesBorder(SWFMovie *movie, bool bShowMetadata)
{
   SWFShape *shpBorder = new SWFShape();

   CRect m_rcBorder;
   m_rcBorder.SetRectEmpty();
   if (!m_rcVideo.IsRectEmpty())
      m_rcBorder.UnionRect(m_rcBorder, m_rcVideo);
   if (!m_rcPages.IsRectEmpty())
      m_rcBorder.UnionRect(m_rcBorder, m_rcPages);
   if (!m_rcControlBar.IsRectEmpty())
      m_rcBorder.UnionRect(m_rcBorder, m_rcControlBar);

   m_rcBorder.InflateRect(FLASH_BORDERSIZE, FLASH_BORDERSIZE);
   if (m_bHasControlBar)
      m_rcBorder.bottom -= FLASH_BORDERSIZE;

   DrawBorderLines(shpBorder, 0.0f, 0.0f, 
                   (float)m_rcBorder.Width(), (float)m_rcBorder.Height(), D3D_NO_FILL);

   SWFDisplayItem *di = movie->add(shpBorder);

   float x = m_rcBorder.left;
   float y = m_rcBorder.top;

///   // Consider global scale
///   x *= m_dGlobalScale;
///   y *= m_dGlobalScale;

   x += m_fMovieOffsetX;
   y += m_fMovieOffsetY;

   di->moveTo(x, y);
///   di->scale(m_dGlobalScale, m_dGlobalScale);

   di->setDepth(15971); // above "dummy" background, control bar 
}

UINT CFlashEngine::DrawWebplayerLogo(SWFSprite *sprControl, SWFFont *fntLogo, 
                                     float x, float y, float scale)
{
   HRESULT hr = S_OK;

   SWFDisplayItem *di = NULL;


   float fntSize = 14.0f;
   MYRGB rgbTxt;
   //rgbTxt.r = 0x7C; rgbTxt.g = 0x80; rgbTxt.b = 0x5F; 
   rgbTxt = m_rgb045;
   Gdiplus::ARGB argb = DWORD((0xff << 24) + (rgbTxt.r << 16) + (rgbTxt.g << 8) + rgbTxt.b);

   // A SWFSprite for the whole webplayer logo
   SWFSprite *sprWebplayerLogo = new SWFSprite();

   // An extra SWFSprite for the skewing
   SWFSprite *sprTxt= new SWFSprite();

   // "LECTURNITY webplayer"
   SWFText *swfTxt = new SWFText();
   DrawText(swfTxt, 0.0f, fntSize, fntLogo, _T("LECTURNITY"), fntSize, argb);
   di = sprTxt->add(swfTxt);
   di->skewXTo(-0.25f);
   if (!m_bIsPspDesign)
   {
      DrawText(swfTxt, (5.25f*fntSize), fntSize, fntLogo, _T("webplayer"), (0.75f*fntSize), argb);
      di = sprTxt->add(swfTxt);
      di->skewXTo(-0.25f);
   }
   // Version ...
   _TCHAR sVersion[256];
   KerberokHandler *kh = new KerberokHandler();
   kh->GetVersionStringShortShort(sVersion);
   CString csVersion;
   csVersion.LoadString(IDS_VERSION);
   csVersion += _T(" ");
   csVersion += sVersion;
//   DrawText(swfTxt, (7.25f*fntSize), (1.65*fntSize), fntLogo, csVersion, (0.5f*fntSize), 
//      RGB(rgbTxt.r, rgbTxt.g, rgbTxt.b));
//   di = sprTxt->add(swfTxt);
//   di->skewXTo(-0.25f);

   sprTxt->nextFrame();
   di = sprWebplayerLogo->add(sprTxt);
   di->setName(GetCharFromCString(csVersion));

   // Two squares ...
   float offX = -0.175f;
   SWFShape *shpQ = new SWFShape();
   SWFFill *fillQ1 = shpQ->addSolidFill(0x00, 0x6C, 0xB7);
   shpQ->setRightFill(fillQ1);
   shpQ->movePenTo((offX*fntSize), 0.0f+(0.15f*fntSize));
   shpQ->drawLine(fntSize/6.0f, 0.0f);
   shpQ->drawLine(0.0f, fntSize/6.0f);
   shpQ->drawLine(-fntSize/6.0f, 0.0f);
   shpQ->drawLineTo((offX*fntSize), 0.0f+(0.15f*fntSize));
   di = sprWebplayerLogo->add(shpQ);
   SWFFill *fillQ2 = shpQ->addSolidFill(m_rgb100.r, m_rgb100.g, m_rgb100.b);
   shpQ->setRightFill(fillQ2);
   shpQ->movePenTo((offX*fntSize), 0.0f+(0.15f*fntSize)+(fntSize/6.0f));
   shpQ->drawLine(fntSize/6.0f, 0.0f);
   shpQ->drawLine(0.0f, fntSize/6.0f);
   shpQ->drawLine(-fntSize/6.0f, 0.0f);
   shpQ->drawLineTo((offX*fntSize), 0.0f+(0.15f*fntSize)+(fntSize/6.0f));
   di = sprWebplayerLogo->add(shpQ);

   // Create a transparent link button for www.lecturnity.de
   SWFButton *btnLink = new SWFButton();
   SWFShape *shpBtnLink = new SWFShape();
   SWFFill *fillBtnLink = shpBtnLink->addSolidFill(0x00, 0x00, 0x00, 0x00);
   shpBtnLink->setRightFill(fillBtnLink);
//   DrawRectangle(shpBtnLink, -10.0f, 0.0f, 130.0f, fntSize);
   DrawRectangle(shpBtnLink, -10.0f, 0.0f, (5.5f*fntSize), fntSize);
   btnLink->addShape(shpBtnLink, SWFBUTTON_HIT);
   // Add ActionScript to the link button
   CString csWwwLecturnity;
   csWwwLecturnity.LoadString(IDS_WEBPAGE_LECTURNITY);
   _TCHAR script[1024];
//
//      var objRoot = _parent._parent;\
//      objRoot.getUrl(\"%s\", \"_blank\");\
//      objRoot.lecOnOpenUrl(\"%s\");\
//      objRoot.lecOnClose();\
//
   _stprintf(script, _T("\
      this.useHandCursor = true;\
      var objRoot = _parent._parent;\
      objRoot.lecOnOpenUrl(\"%s\");\
      objRoot.swfSprTooltip._visible = false;\
   "), csWwwLecturnity);
   PrintActionScript(_T("WebPlayer Link Button"), script);
   btnLink->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEDOWN);
   _stprintf(script, _T("\
      this.useHandCursor = true;\
   "));
   PrintActionScript(_T("WebPlayer Link Button"), script);
   btnLink->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEOVER);
   CreateTooltip(btnLink, csWwwLecturnity, _T("topleft"));

   di = sprWebplayerLogo->add(btnLink);
   di->setName("swfBtnLink");

   // Create a transparent version button
   SWFButton *btnVersion = new SWFButton();
   SWFShape *shpBtnVersion = new SWFShape();
   SWFFill *fillBtnVersion = shpBtnVersion->addSolidFill(0x00, 0x00, 0x00, 0x00);
   shpBtnVersion->setRightFill(fillBtnVersion);
   if (m_bIsPspDesign)
      DrawRectangle(shpBtnVersion, (-10.0f+(5.6f*fntSize)), 0.0f, (0.5f*fntSize), fntSize);
   else
      DrawRectangle(shpBtnVersion, (-10.0f+(5.6f*fntSize)), 0.0f, (3.4f*fntSize), fntSize);
   btnVersion->addShape(shpBtnVersion, SWFBUTTON_HIT);
   // Add ActionScript to the version button
   _stprintf(script, _T("\
      this.useHandCursor = false;\
   "));
   PrintActionScript(_T("WebPlayer Version Button"), script);
   btnVersion->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEDOWN);
   _stprintf(script, _T("\
      this.useHandCursor = false;\
   "));
   PrintActionScript(_T("WebPlayer Version Button"), script);
   btnVersion->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEOVER);
   CreateTooltip(btnVersion, csVersion, _T("topleft"));

   di = sprWebplayerLogo->add(btnVersion);
   di->setName("swfBtnVersion");

   // For Flash version 6 and higher: useHandCursor works this way
   _stprintf(script, _T("\
      swfBtnVersion.useHandCursor = false;\
   "));
   PrintActionScript(_T("WebPlayer Version Button"), script);
   sprWebplayerLogo->add(new SWFAction(GetCharFromCString(script)));


   // Now add the whole webplayer logo to the parent movie clip
   sprWebplayerLogo->nextFrame();
   di = sprControl->add(sprWebplayerLogo);
   di->setName("swfSprWebplayerLogo");
   di->moveTo(x, y);
   di->scaleTo(scale);

   return hr;
}

void CFlashEngine::DrawSeparator(SWFSprite *sprControl, float x, float height, CString csName)
{
   SWFSprite *sprSep = new SWFSprite();

   SWFShape *shpSep = new SWFShape();
//   Draw3DRoundedRectangle(shpSep, x, 7.0f, 1.0f, (height-14.0f), 0.5f, 1, D3D_PEN_PALETTE, 
//      D3D_BORDER, D3D_FILL, D3D_DEEPENED, D3D_NO_GRADIENT);
   shpSep->movePenTo(x+1.0f, 7.0f);
//   shpSep->setLine(1, 0xC7, 0xC5, 0xB3);
   shpSep->setLine(1, m_rgb080.r, m_rgb080.g, m_rgb080.b);
   shpSep->drawLine(-1.0f, 0.0f);
   shpSep->drawLine(0.0f, height-14.0f);
//   shpSep->setLine(1, 0xFF, 0xFF, 0xFF);
   shpSep->setLine(1, m_rgb150.r, m_rgb150.g, m_rgb150.b);
   shpSep->drawLine(1.0f, 0.0f);
   shpSep->drawLine(0.0f, -height+14.0f);

   SWFDisplayItem *di = NULL;
   di = sprSep->add(shpSep);
   sprSep->nextFrame();
   di = sprControl->add(sprSep);
   CString csId;
   csId.Format(_T("swfSprSeparator%s"), csName);
   di->setName(GetCharFromCString(csId));
}

UINT CFlashEngine::CreateTooltipField(SWFMovie *movie, TCHAR *tszLogoUrl)
{
   HRESULT hr = S_OK;

   float fFontHeight = 11.0f;
   if (!m_bIsPspDesign)
      fFontHeight *= m_dGlobalScale;

   SWFText *pText = new SWFText();

   pText->setFont(m_pFntStd);
   pText->setHeight(fFontHeight);

   SWFDisplayItem *di = NULL;
   SWFSprite *sprTooltip = new SWFSprite();

   // Create a textfield
   SWFTextField *txtField = new SWFTextField();
   txtField->setFont(m_pFntStd);
   txtField->setBounds(1000.0f, (fFontHeight+2.0f)); // Initial width, which is changed dynamically by ActionScript
   txtField->setHeight(fFontHeight);
   txtField->setColor(0x33, 0x33, 0x33);
   
   txtField->setVariableName("text");
   txtField->setAlignment(SWFTEXTFIELD_ALIGN_CENTER);
   txtField->setFlags(SWFTEXTFIELD_DRAWBOX | SWFTEXTFIELD_USEFONT | SWFTEXTFIELD_NOEDIT | SWFTEXTFIELD_NOSELECT | SWFTEXTFIELD_HASLAYOUT);

   CString csPossibleString;
   CString csText = _T("");
   csPossibleString.LoadString(IDS_START_REPLAY_PAUSE);
   csText += csPossibleString;
   csPossibleString.LoadString(IDS_JUMP_TO_BEGIN);
   csText += csPossibleString;
   csPossibleString.LoadString(IDS_JUMP_TO_END);
   csText += csPossibleString;
   csPossibleString.LoadString(IDS_SLIDE_BACKWARDS);
   csText += csPossibleString;
   csPossibleString.LoadString(IDS_SLIDE_FORWARD);
   csText += csPossibleString;
   csPossibleString.LoadString(IDS_SOUND_ON_OFF);
   csText += csPossibleString;
   csPossibleString.LoadString(IDS_VERSION);
   csText += csPossibleString;
   csPossibleString.LoadString(IDS_WEBPAGE_LECTURNITY);
   csText += csPossibleString;

   csText += tszLogoUrl;

   CString csInitialText = _T(" ");
   char *utfText = LString::TCharToUtf8(csText);
   char *utfInitialText = LString::TCharToUtf8(csInitialText);
   if (utfText)
   {
      txtField->addUTF8Chars(utfText);
      txtField->addUTF8String(utfInitialText);
      delete[] utfText;
      utfText = NULL;
      delete[] utfInitialText;
      utfInitialText = NULL;
   }


   //txtField->addUTF8String(GetUtf8CharFromCString(_T("")));
//   // All possible Flags:
//   SWFTEXTFIELD_HASFONT|SWFTEXTFIELD_HASLENGTH|SWFTEXTFIELD_HASCOLOR|SWFTEXTFIELD_NOEDIT|
//   SWFTEXTFIELD_PASSWORD|SWFTEXTFIELD_MULTILINE|SWFTEXTFIELD_WORDWRAP|SWFTEXTFIELD_HASTEXT|
//   SWFTEXTFIELD_USEFONT|SWFTEXTFIELD_HTML|SWFTEXTFIELD_DRAWBOX|SWFTEXTFIELD_NOSELECT|
//   SWFTEXTFIELD_HASLAYOUT|SWFTEXTFIELD_AUTOSIZE

   di = sprTooltip->add(txtField);
   di->moveTo(0.0f, 0.0f);
   di->setName("ttTextfield");
   sprTooltip->nextFrame();

   di = movie->add(sprTooltip);
   di->setDepth(15981); // above "dummy" background and borders
   di->setName("swfSprTooltip");
   di->moveTo(-1000.0f, -1000.0f);  // should be wide enough outside the drawing area

   return hr;
}

UINT CFlashEngine::CreateTooltip(SWFButton *pButton, CString csTooltip, 
                                 CString csPosition)
{
   HRESULT hr = S_OK;

   float fFontHeight = 11.0f;
   if (!m_bIsPspDesign)
      fFontHeight *= m_dGlobalScale;

   SWFText *pText = new SWFText();

   pText->setFont(m_pFntStd);
   pText->setHeight(fFontHeight);

   csTooltip.Format(_T(" %s "), csTooltip);
   char *utfTooltip = LString::TCharToUtf8(csTooltip);
   float fMaxStringWidth = pText->getUTF8Width((unsigned char *) utfTooltip);
   // Empirical correction
   fMaxStringWidth *= 1.15f;
   fMaxStringWidth += 4.0f;

   // Add Actionscript to the button
   _TCHAR script[1024];
   if (utfTooltip)
   {
#ifdef _UNICODE
      _stprintf(script, _T("\
         var objRoot = _parent._parent;\
         objRoot.lecDoToolTip(\"%S\", %g, \"%s\");\
      "), utfTooltip, fMaxStringWidth, csPosition);
#else
      _stprintf(script, _T("\
      var objRoot = _parent._parent;\
         objRoot.lecDoToolTip(\"%s\", %g, \"%s\");\
      "), utfTooltip, fMaxStringWidth, csPosition);
#endif //_UNICODE
      delete[] utfTooltip;
      utfTooltip = NULL;
   }
   PrintActionScript(_T("Tooltip"), script);
   pButton->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEOVER);

   _stprintf(script, _T("\
      var objRoot = _parent._parent;\
      objRoot.lecStopToolTip();\
   "));
   PrintActionScript(_T("Tooltip"), script);
   pButton->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEOUT);

   delete[] utfTooltip;

   return hr;
}

// --------------------------------------------------------------------------
// The "Preloader"
// --------------------------------------------------------------------------

 UINT CFlashEngine::CreateSlidestarPreloader(SWFSprite *sprControl, float x, float y, float controlbarHeight)
{
   HRESULT hr = S_OK;

   SWFDisplayItem *di = NULL;

   float posX = x;
   float posY = y;
   
   // consider video (if any)
   //if (m_iAccVideoWidth > 0)
   //{
   //   posX -= (float)(FLASH_PADDING + m_iAccVideoWidth + FLASH_PADDING);
   //}
   
   float progressbarWidth = 194.0f;
   float progressbarHeight = 4.0f;
   float scale = controlbarHeight / DEFAULT_CONTROLBAR_HEIGHT;
   
   SWFShape *shpProgressBar = new SWFShape();
   //SWFFill *fill = shpProgressBar->addSolidFill(0,255,0);
   //shpProgressBar->setLeftFill(fill);
   //  SWFFill *fill = shpProgressBar->addSolidFill(0, 255, 0);
	//shpProgressBar->setRightFill(fill);
	//DrawRectangle(shpProgressBar, 0.0f, 0.0f, progressbarWidth, progressbarHeight);
	
   shpProgressBar->setLine(progressbarHeight,0,255,0);
   shpProgressBar->movePen(0,0);
   shpProgressBar->drawLineTo(progressbarWidth,0);
   SWFSprite *sprPreload = new SWFSprite(); 
   di = sprPreload->add(shpProgressBar);
   sprPreload->nextFrame();

  
   di = sprControl->add(sprPreload);
   //di->setDepth(15850); // above "dummy" background, control bar
   di->setName("swfSprProgressBar");
   di->moveTo(posX, posY);

   return hr;
}
UINT CFlashEngine::CreatePreloader(SWFSprite *sprControl, float x, float y, 
                                   SWFFont *fntLogo, float controlbarHeight)
{
   HRESULT hr = S_OK;

   SWFDisplayItem *di = NULL;

   float posX = x;
   float posY = y;
   // consider video (if any)
   //if (m_iAccVideoWidth > 0)
   //{
   //   posX -= (float)(FLASH_PADDING + m_iAccVideoWidth + FLASH_PADDING);
   //}

   float progressbarWidth = 98.0f;
   float progressbarHeight = 10.0f;
   float scale = controlbarHeight / DEFAULT_CONTROLBAR_HEIGHT;

   // Create the progress bar
   // --> use 2 objects here: 
   //   - 3DRoundRectangle for the left (rounded) side
   //   - "normal" rectangle for middle part
   //   - (the right part is omitted here)
   SWFShape *shpProgressBarL = new SWFShape();
   SWFShape *shpProgressBarM = new SWFShape();

   SWFGradient *gradient = new SWFGradient();
   gradient->addEntry(0.00f, 0x00, 0x00, 0x80);
   gradient->addEntry(0.50f, 0x00, 0x00, 0x80);
   gradient->addEntry(1.00f, 0x00, 0x00, 0x80);

   // Left part of the progress bar
   Draw3DRoundedRectangle(shpProgressBarL, 0.0f, 0.0f, 0.67f*scale*progressbarHeight, scale*progressbarHeight, 
      0.33f*scale*progressbarHeight, 1, D3D_PEN_PALETTE, 
      D3D_LEFT_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_DEEPENED, D3D_LINEAR_GRADIENT, gradient, NULL, NULL);

   SWFSprite *sprPreloadL = new SWFSprite(); 
   di = sprPreloadL->add(shpProgressBarL);
   sprPreloadL->nextFrame();

   di = sprControl->add(sprPreloadL);
   di->setDepth(15975); // above "dummy" background, control bar
   di->setName("swfSprProgressBarL");
   di->moveTo(posX, posY);

   // Middle part of the progress bar
   Draw3DRoundedRectangle(shpProgressBarM, 0.0f, 0.0f, 
      scale*progressbarWidth-(0.33f*scale*progressbarHeight), scale*progressbarHeight, 
      0.33f*scale*progressbarHeight, 1, D3D_PEN_PALETTE, 
      D3D_FULL_ROUND, D3D_NO_BORDER, D3D_FILL, D3D_DEEPENED, D3D_LINEAR_GRADIENT, gradient, NULL, NULL);

   SWFSprite *sprPreloadM = new SWFSprite(); 
   di = sprPreloadM->add(shpProgressBarM);
   sprPreloadM->nextFrame();

   di = sprControl->add(sprPreloadM);
   di->setDepth(15976); // above "dummy" background, control bar
   di->setName("swfSprProgressBarM");
   di->moveTo(posX+(0.33f*scale*progressbarHeight), posY);

   return hr;
}

UINT CFlashEngine::CheckForSlidestarControlBarFlash()
{
	/*HRESULT hr = S_OK;*/
   CFile f;
   CString s(_T(".\\Flash\\slidestar\\first_slide_over.swf"));
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\first_slide_up.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\last_slide_over.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\last_slide_up.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\load.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\next_slide_over.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\next_slide_up.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\pause_over.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\pause_up.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\play_over.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\play_up.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\position.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\position_arrow.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\previous_slide_over.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\previous_slide_up.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\speaker_off_over.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\speaker_off_up.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\speaker_on_over.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\speaker_on_up.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\volume_arrow.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   s = _T(".\\Flash\\slidestar\\volume_bar.swf");
   if(!f.Open(s,CFile::modeRead))
	   return imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
   else
	   f.Close();
   return S_OK;
}

// --------------------------------------------------------------------------
// The control bar itself
// --------------------------------------------------------------------------

 UINT CFlashEngine::CreateSlidestarControlBar(SWFMovie *movie)
{
	HRESULT hr = S_OK;

	float x = m_rcControlBar.left - FLASH_BORDERSIZE;
    float y = m_rcControlBar.top;
    float width = m_rcControlBar.Width() + 2*FLASH_BORDERSIZE;
    float height = m_rcControlBar.Height();

	//float x = m_fSlideOffsetX;
	//float y = m_fSlideOffsetY + (float)(m_iWbInputHeight) + FLASH_BORDERSIZE;
	//float width = (float)(m_iWbInputWidth) + (2.0f * FLASH_BORDERSIZE);
	//float height = m_fControlbarHeight;
	float fCBWidth = 544;

	// consider video (if any)
	if (m_iAccVideoWidth > 0)
	{
		x -= (float)(FLASH_BORDERSIZE + 40.0f + FLASH_BORDERSIZE + m_iAccVideoWidth + FLASH_BORDERSIZE);
		width += (float)(40.0f + FLASH_BORDERSIZE + m_iAccVideoWidth + FLASH_BORDERSIZE);
	}

	y += 46.0f;

	x += (width - fCBWidth) / 2;
	width = fCBWidth;

	float scale = height / DEFAULT_CONTROLBAR_HEIGHT;

	SWFDisplayItem *di = NULL;

	SWFSprite *sprControl = new SWFSprite();


	// Create the background style
	DrawSlidestarControlbarBackground(sprControl, width, height);


	// Create buttons

	if (SUCCEEDED(hr))
		hr = CreateSlidestarPauseButton(sprControl, 0.0f, 0.0f); 

	if (SUCCEEDED(hr))
		hr = CreateSlidestarPlayButton(sprControl, 0.0f, 0.0f);

	if (SUCCEEDED(hr))
		hr = CreateSlidestarGotoFirstFrameButton(sprControl, 95.0f, 0.0f);

	if (SUCCEEDED(hr))
		hr = CreateSlidestarGotoPrevPageButton(sprControl, 132.0f, 0.0f);

	if (SUCCEEDED(hr))
		hr = CreateSlidestarGotoNextPageButton(sprControl, 379.0f, 0.0f);

	if (SUCCEEDED(hr))
		hr = CreateSlidestarGotoLastFrameButton(sprControl, 416.0f, 0.0f);

	
	//Time display
	
	if (SUCCEEDED(hr))
		hr = CreateTimeDisplay(sprControl, 0.0f, 8.0f, 10.0f);


	//Preloader

	if (SUCCEEDED(hr))
		hr = CreateSlidestarSliderLine(sprControl, 169.0f, 5.0f, _T("Preloader"), ".\\Flash\\Slidestar\\load.swf");

	if (SUCCEEDED(hr))
		hr = CreateSlidestarPreloader(sprControl,172.0f, 7.7f, height);

	// Time slider elements

	if (SUCCEEDED(hr))
		hr = CreateSlidestarSliderLine(sprControl, 176.5f, 19.0f, _T("TimeLine"), ".\\Flash\\Slidestar\\position.swf");

	if (SUCCEEDED(hr))
		hr = CreateSlidestarActiveTimeLine(sprControl, 176.5f, 18.0f, 185.0f, 6.0f);

	if (SUCCEEDED(hr))
		hr = CreateSlidestarTimeSliderButton(sprControl, 176.5f, 14.0f, 185.0f, 10.0f, 10.0f);



	// Volume slider line

	if (SUCCEEDED(hr))
		hr = CreateSlidestarSliderLine(sprControl, 453.0f, 10.5f, _T("VolumeLine"), ".\\Flash\\Slidestar\\volume_bar.swf");
	
	// Active region above the time slider line
	if (SUCCEEDED(hr))
		hr = CreateSlidestarActiveVolumeLine(sprControl, 454.0f, 10.0f, 53.0f, 6.0f, 10.0f);

	// Volume slider button 
	if (SUCCEEDED(hr))
		hr = CreateSlidestarVolumeSliderButton(sprControl, 453.0f, 7.0f, 53.0f, 10.0f, 10.0f);



	// Mute (Sound on/off) button

	if (SUCCEEDED(hr))
		hr = CreateSlidestarSoundOffButton(sprControl, 517.0f, 0.0f);
	if (SUCCEEDED(hr))
		hr = CreateSlidestarSoundOnButton(sprControl, 517.0f, 0.0f);

	// Add ActionScript

	if (SUCCEEDED(hr))
		hr = AddActionScriptToControlBar(sprControl, scale);

	sprControl->nextFrame();
	di = movie->add(sprControl);
	di->setName("swfSprControl");

	di->moveTo(x, y);
	
	return hr;

}

UINT CFlashEngine::CreateControlBar(SWFMovie *movie, SWFFont *fntLogo)
{
   HRESULT hr = S_OK;

   float width = m_rcControlBar.Width() + 2*FLASH_BORDERSIZE;
   float height = m_rcControlBar.Height();

   float scale = (float)m_fScaleControlBar; // = 1.0f;

   SWFDisplayItem *di = NULL;

   SWFSprite *sprControl = new SWFSprite();

   // Create the background style
   DrawControlbarBackground(sprControl, width, height);

   // Left part of the control bar
   // Create buttons
   if (SUCCEEDED(hr))
      hr = CreateGotoFirstFrameButton(sprControl, scale*10.0f, scale*5.0f, scale*19.0f, scale*19.0f);
   if (SUCCEEDED(hr))
      hr = CreateGotoLastFrameButton(sprControl, scale*38.0f, scale*5.0f, scale*19.0f, scale*19.0f);
   if (SUCCEEDED(hr))
      hr = CreateGotoPrevPageButton(sprControl, scale*66.0f, scale*5.0f, scale*19.0f, scale*19.0f);
   if (SUCCEEDED(hr))
      hr = CreateGotoNextPageButton(sprControl, scale*94.0f, scale*5.0f, scale*19.0f, scale*19.0f);
   if (SUCCEEDED(hr))
      hr = CreatePauseButton(sprControl, scale*122.0f, scale*5.0f, scale*25.0f, scale*25.0f);
   if (SUCCEEDED(hr))
      hr = CreatePlayButton(sprControl, scale*122.0f, scale*5.0f, scale*25.0f, scale*25.0f);

   // Time slider line
   if (SUCCEEDED(hr))
      hr = CreateSliderLine(sprControl, scale*160.0f, scale*8.0f, scale*310.0f, scale*4.0f, _T("TimeLine"));
   // Active region above the time slider line
   if (SUCCEEDED(hr))
      hr = CreateActiveTimeLine(sprControl, scale*160.0f, scale*7.0f, scale*310.0f, scale*6.0f, scale*10.0f);

   // Time slider button (with 2 pixel cut on right border)
   if (SUCCEEDED(hr))
      hr = CreateTimeSliderButton(sprControl, scale*165.0f, scale*5.0f, scale*308.0f, scale*10.0f, scale*10.0f);

   // Mute (Sound on/off) button
   if (SUCCEEDED(hr))
      hr = CreateSoundOffButton(sprControl, scale*160.0f, scale*18.0f, scale*12.0f, scale*12.0f);
   if (SUCCEEDED(hr))
      hr = CreateSoundOnButton(sprControl, scale*160.0f, scale*18.0f, scale*12.0f, scale*12.0f);

   // Volume slider line
   if (SUCCEEDED(hr))
      hr = CreateSliderLine(sprControl, scale*180.0f, scale*22.0f, scale*75.0f, scale*4.0f, _T("VolumeLine"));
   // Active region above the time slider line
   if (SUCCEEDED(hr))
      hr = CreateActiveVolumeLine(sprControl, scale*180.0f, scale*21.0f, scale*75.0f, scale*6.0f, scale*10.0f);

   // Volume slider button (with 2 pixel cut on right border)
   if (SUCCEEDED(hr))
      hr = CreateVolumeSliderButton(sprControl, scale*185.0f, scale*19.0f, scale*73.0f, scale*10.0f, scale*10.0f);

   // Text field boxes for the display of time ...
   if (SUCCEEDED(hr))
      hr = CreateTextFieldBox(sprControl, scale*260.0f, scale*19.0f, scale*100.0f, scale*12.0f, _T("Time"));
   // ... and the display of the actual slide
   if (SUCCEEDED(hr))
      hr = CreateTextFieldBox(sprControl, scale*370.0f, scale*19.0f, scale*100.0f, scale*12.0f, _T("Slide"));

   // Time Display
   if (SUCCEEDED(hr))
      hr = CreateTimeDisplay(sprControl, scale*270.0f, scale*19.0f, scale*9.0f);

   // Slide Display
   if (SUCCEEDED(hr))
      hr = CreateSlideDisplay(sprControl, scale*390.0f, scale*19.0f, scale*9.0f);

   // Separator
   if (SUCCEEDED(hr))
      DrawSeparator(sprControl, scale*480.0f, height, _T("1"));


   // Right part of the control bar
   // Separator
   if (SUCCEEDED(hr) && !m_bLecIdentityHidden)
      DrawSeparator(sprControl, width-(scale*140.0f), height, _T("2"));

   // The webplayer logo
   if (SUCCEEDED(hr) && !m_bLecIdentityHidden)
      hr = DrawWebplayerLogo(sprControl, fntLogo, width-(scale*125.0f), 0.5f*(height-(scale*14.0f))-2.0f, scale);

   // A simple preloader
   if (SUCCEEDED(hr))
      hr = CreatePreloader(sprControl, scale*370.0f, scale*19.0f, m_pFntLogo, height);

   // Add ActionScript
   if (SUCCEEDED(hr))
      hr = AddActionScriptToControlBar(sprControl, scale);

   sprControl->nextFrame();
   di = movie->add(sprControl);
   di->setName("swfSprControl");
   di->setDepth(15970); // above "dummy" background; below 3-pixel border

   float x = m_rcControlBar.left - FLASH_BORDERSIZE;
   float y = m_rcControlBar.top;

///   // Consider global scale factor
///   x *= m_dGlobalScale;
///   y *= m_dGlobalScale;

   x += m_fMovieOffsetX;
   y += m_fMovieOffsetY;
   di->moveTo(x, y);
///   di->scale(m_dGlobalScale, m_dGlobalScale);

   return hr;
}

UINT CFlashEngine::CreateControlBarPsp(SWFMovie *movie, SWFFont *fntLogo)
{
   HRESULT hr = S_OK;

   float x = 0.0f;
   float y = 0.0f;
   float width = PSP_MOVIE_WIDTH;
   float height = PSP_MOVIE_HEIGHT;

   float scale = m_rcMovie.Width() / PSP_MOVIE_WIDTH;

   SWFDisplayItem *di = NULL;

   SWFSprite *sprControl = new SWFSprite();

   // Create buttons
   if (SUCCEEDED(hr))
      hr = CreateGotoFirstFrameButton(sprControl, scale*0.0f, scale*227.0f, scale*30.0f, scale*45.0f);
   if (SUCCEEDED(hr))
      hr = CreateGotoLastFrameButton(sprControl, scale*0.0f, scale*0.0f, scale*30.0f, scale*45.0f);
   if (SUCCEEDED(hr))
      hr = CreateGotoPrevPageButton(sprControl, scale*0.0f, scale*182.0f, scale*30.0f, scale*45.0f);
   if (SUCCEEDED(hr))
      hr = CreateGotoNextPageButton(sprControl, scale*0.0f, scale*45.0f, scale*30.0f, scale*45.0f);
   if (SUCCEEDED(hr))
      hr = CreatePauseButton(sprControl, scale*0.0f, scale*90.0f, scale*30.0f, scale*92.0f);
   if (SUCCEEDED(hr))
      hr = CreatePlayButton(sprControl, scale*0.0f, scale*90.0f, scale*30.0f, scale*92.0f);

   // Text field boxes around the 2 slider
   if (SUCCEEDED(hr))
      hr = CreateTextFieldBox(sprControl, scale*397.0f, scale*5.0f, scale*29.0f, scale*190.0f, _T("Time"));
   if (SUCCEEDED(hr))
      hr = CreateTextFieldBox(sprControl, scale*445.0f, scale*5.0f, scale*29.0f, scale*150.0f, _T("Time"));

   // Time slider line
   if (SUCCEEDED(hr))
      hr = CreateSliderLine(sprControl, scale*410.0f, scale*20.0f, scale*160.0f, scale*4.0f, _T("TimeLine"), true);
   // Active region above the time slider line
   if (SUCCEEDED(hr))
      hr = CreateActiveTimeLine(sprControl, scale*409.0f, scale*20.0f, scale*160.0f, scale*6.0f, scale*10.0f, true);
   // Time slider button (with 2 pixel cut on right border)
   if (SUCCEEDED(hr))
      hr = CreateTimeSliderButton(sprControl, scale*402.0f, scale*40.0f, scale*150.0f, scale*20.0f, scale*30.0f, true);

   // Mute (Sound on/off) button
   if (SUCCEEDED(hr))
      hr = CreateSoundOffButton(sprControl, scale*445.0f, scale*165.0f, scale*30.0f, scale*30.0f);
   if (SUCCEEDED(hr))
      hr = CreateSoundOnButton(sprControl, scale*445.0f, scale*165.0f, scale*30.0f, scale*30.0f);

   // Volume slider line
   if (SUCCEEDED(hr))
      hr = CreateSliderLine(sprControl, scale*458.0f, scale*20.0f, scale*120.0f, scale*4.0f, _T("VolumeLine"), true);
   // Active region above the time slider line
   if (SUCCEEDED(hr))
      hr = CreateActiveVolumeLine(sprControl, scale*457.0f, scale*20.0f, scale*120.0f, scale*6.0f, scale*10.0f, true);
   // Volume slider button (with 2 pixel cut on right border)
   if (SUCCEEDED(hr))
      hr = CreateVolumeSliderButton(sprControl, scale*450.0f, scale*40.0f, scale*110.0f, scale*20.0f, scale*30.0f, true);

   // Text field boxes for the display of time ...
   if (SUCCEEDED(hr))
      hr = CreateTextFieldBox(sprControl, scale*397.0f, scale*210.0f, scale*79.0f, scale*16.0f, _T("Time"));
   // ... and the display of the actual slide
   if (SUCCEEDED(hr))
      hr = CreateTextFieldBox(sprControl, scale*397.0f, scale*232.0f, scale*79.0f, scale*16.0f, _T("Slide"));

   // Time Display
   if (SUCCEEDED(hr))
      hr = CreateTimeDisplay(sprControl, scale*392.0f, scale*212.5f, scale*10.0f, scale);
   // Slide Display
   if (SUCCEEDED(hr))
      hr = CreateSlideDisplay(sprControl, scale*403.0f, scale*234.5f, scale*10.0f, scale);

   // The webplayer logo
   if (SUCCEEDED(hr) && !m_bLecIdentityHidden)
      //hr = DrawWebplayerLogo(sprControl, fntLogo, scale*396.0f, scale*255.0f, 0.70f*scale);
      hr = DrawWebplayerLogo(sprControl, fntLogo, scale*404.0f, scale*252.0f, 1.0f*scale);

   // Add ActionScript
   if (SUCCEEDED(hr))
      hr = AddActionScriptToControlBar(sprControl, scale);

   sprControl->nextFrame();
   di = movie->add(sprControl);
   di->setName("swfSprControl");
   di->setDepth(15970); // above "dummy" background; below 3-pixel border
   di->moveTo(x, y);

   return hr;
}

// --------------------------------------------------------------------------
// Additional Logo and Metadata
// --------------------------------------------------------------------------

UINT CFlashEngine::LoadLogoImage(SWFBitmap **ppBitmap, CString csImageFile, 
                                 int &logoWidth, int &logoHeight)
{
   HRESULT hr = S_OK;

   CString csImageOut;
   CString csImageTmp = csImageFile;

   // Do we have to write a rescaled image before conversion to DBL?
   if (m_dGlobalScale != (double)(1.00f))
   {
      // Create a temporary DrawSdk::Image object
      DrawSdk::Image* pImageObject = new DrawSdk::Image(0.0f, 0.0f, -1.0f, -1.0f, csImageFile);
      CString csImageName = pImageObject->GetImageName();

      hr = WriteRescaledImage(pImageObject, csImageName, csImageTmp, m_dGlobalScale, m_dGlobalScale);

      // set the (temporary) image output file name
      m_mapImageNames.SetAt(csImageFile, csImageTmp);

      delete pImageObject;
      pImageObject = NULL;
   }

   // Convert image to DBL
   if (SUCCEEDED(hr))
      hr = ConvertImage(csImageFile, csImageTmp, csImageOut);

   if (SUCCEEDED(hr))
   {
      FILE *pFile = NULL;
      if (NULL == (pFile = _tfopen(csImageOut, _T("rb"))))
      {
         hr = imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;
         
         _TCHAR tszMessage[1024];
         _stprintf(tszMessage, _T("Error: Cannot open image file! %s\n"), csImageOut);
         _tprintf(tszMessage);
      }

      if (SUCCEEDED(hr))
      {
         SWFBitmap *pBitmap = new SWFBitmap(pFile, csImageOut.Right(4) == _T(".dbl"));
         m_mapTempFiles.SetAt(csImageOut, pFile);
         *ppBitmap = pBitmap;

///         logoWidth = (int)(pBitmap->getWidth() + 0.5f);
///         logoHeight = (int)(pBitmap->getHeight() + 0.5f);

         // Consider the scaling factor of the rescaled logo image
         // --> we need the size of the original logo image
         logoWidth = (int)(0.5f + ((float)pBitmap->getWidth() / m_dGlobalScale));
         logoHeight = (int)(0.5f + ((float)pBitmap->getHeight() / m_dGlobalScale));
      }
   }

   return hr;
}

UINT CFlashEngine::DrawLogoImage(SWFMovie *movie, SWFBitmap *bmpLogo, float x, float y, 
                                 _TCHAR *tszLogoUrl)
{
   HRESULT hr = S_OK;

///   // Consider global scale factor
///   x *= m_dGlobalScale;
///   y *= m_dGlobalScale;

   float imgWidth = bmpLogo->getWidth();
   float imgHeight =  bmpLogo->getHeight();
   if (m_bHasVideo && m_bHasPages)
   {
      if (imgWidth > (float)(m_rcMetadata.Width()))
         imgWidth = (float)(m_rcMetadata.Width());
      if (imgHeight > (float)(m_rcMetadata.Height()))
         imgHeight = (float)(m_rcMetadata.Height());
   }

   CString csLogoUrl = tszLogoUrl;

   // Bugfix 3738: Use an additional dummy SWFSprite (for the ActionScript code)
   SWFSprite *sprDummy = new SWFSprite();

   SWFSprite *sprLogo = new SWFSprite();

   SWFShape *shpLogo = new SWFShape();
   if (m_bHasVideo && m_bHasPages)
      DrawImage(shpLogo, bmpLogo, true);
   else
      DrawImage(shpLogo, bmpLogo);

   SWFDisplayItem *di = NULL;
   di = sprLogo->add(shpLogo);

   // Do we have to create a tooltip?
   if (!csLogoUrl.IsEmpty())
   {
      // Create a transparent URL button
      SWFButton *btnUrl = new SWFButton();
      SWFShape *shpBtn = new SWFShape();
      SWFFill *fillBtn = shpBtn->addSolidFill(0x00, 0x00, 0x00, 0x00);
      shpBtn->setRightFill(fillBtn);
      DrawRectangle(shpBtn, 0.0f, 0.0f, imgWidth, imgHeight);
      btnUrl->addShape(shpBtn, SWFBUTTON_HIT);
      // Add ActionScript to the URL button
      _TCHAR script[1024];
      char* utfLogoUrl = LString::TCharToUtf8(csLogoUrl);
      if (utfLogoUrl)
      {
#ifdef _UNICODE
         _stprintf(script, _T("\
            this.useHandCursor = true;\
            var objRoot = _parent._parent;\
            objRoot.getUrl(\"%S\", \"_blank\");\
            objRoot.swfSprTooltip._visible = false;\
         "), utfLogoUrl);
#else
         _stprintf(script, _T("\
            this.useHandCursor = true;\
            var objRoot = _parent._parent;\
            objRoot.getUrl(\"%s\", \"_blank\");\
            objRoot.swfSprTooltip._visible = false;\
         "), utfLogoUrl);
#endif //_UNICODE
         delete[] utfLogoUrl;
         utfLogoUrl = NULL;
      }
      PrintActionScript(_T("LogoUrl Button"), script);
      btnUrl->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEDOWN);
      _stprintf(script, _T("\
         this.useHandCursor = true;\
      "));
      PrintActionScript(_T("LogoUrl Button"), script);
      btnUrl->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEOVER);

      CreateTooltip(btnUrl, csLogoUrl, _T("bottomright"));
      di = sprLogo->add(btnUrl);
      di->setName("swfBtnUrl");
   }

   sprLogo->nextFrame();

   sprDummy->add(sprLogo);
   sprDummy->nextFrame();
   
   di = movie->add(sprDummy);
   di->setName("swfSprLogo");
   x += m_fMovieOffsetX;
   y += m_fMovieOffsetY;
   di->moveTo(x, y);
   di->scale(1.0f / m_dGlobalScale); // consider global scaling factor, because the logo image is rescaled
   di->setDepth(15972); // above "dummy" background

   return hr;
}

UINT CFlashEngine::DrawMetadataText(SWFMovie *movie, Gdiplus::ARGB refTextColor, SWFFont *fnt)
{
   if (m_bHasVideo && m_bHasPages)
      return DrawMetadataTextAboveVideo(movie, refTextColor, fnt);

   HRESULT hr = S_OK;

   CString csMetadata;
   csMetadata.Format(_T("%s: %s"), m_csAuthor, m_csTitle);

   float fLogoWidth = m_iLogoWidth;
   float fLogoHeight = m_iLogoHeight;
   float x = (fLogoWidth > 0.0f) ? 
             (float)(FLASH_PADDING + fLogoWidth + 2*FLASH_PADDING) : (float)FLASH_PADDING;
   // vertical align: top (of logo)
   float y = (fLogoHeight>0.0f) ? (float)(FLASH_PADDING + 0.75f*FLASH_METADATAHEIGHT) : (float)(FLASH_PADDING + FLASH_METADATAHEIGHT);
   x += m_fMovieOffsetX;
   y += m_fMovieOffsetY;

   SWFText *pText = new SWFText();

   float fFontSize = FLASH_METADATAHEIGHT;
   pText->setFont(fnt);
   pText->setHeight(fFontSize);

   // maxTextWidth is in pixel units
   float fMaxStringWidth = m_rcMetadata.Width() - x - FLASH_PADDING;
   float fStringWidth = 0.0f;
   bool bWasShortened = false;

   do
   {
      unsigned char *utfText = (unsigned char *) LString::TCharToUtf8(csMetadata);
      if (utfText)
      {
         //fStringWidth = fnt->getUTF8StringWidth(utfText); // this is about 3 times too large
         fStringWidth = pText->getUTF8Width(utfText);

         if (fStringWidth > fMaxStringWidth)
         {
            csMetadata.Delete(csMetadata.GetLength()-1 , 1);

            if (!bWasShortened)
            {
               unsigned char *utfDots = (unsigned char *) LString::TCharToUtf8(_T("..."));
               fMaxStringWidth -= pText->getUTF8Width(utfDots);
               delete[] utfDots;
            }

            bWasShortened = true;
         }

         delete[] utfText;
      }

   }
   while ((fStringWidth > fMaxStringWidth) && (csMetadata.GetLength() > 0));

///   // Consider global scale factor
///   x *= m_dGlobalScale;
///   y *= m_dGlobalScale;
///   fFontSize *= m_dGlobalScale;

   // In case of a very large logo image the metadata string can be reduced to an empty string
   // which has to be considered
   if (csMetadata.GetLength() > 0)
   {
      if (bWasShortened)
         csMetadata += _T("...");

      DrawText(pText, x, y, fnt, csMetadata, fFontSize, refTextColor);
   }

   SWFDisplayItem *di = NULL;
   di = movie->add(pText);
   di->setDepth(15973); // above "dummy" background


   return hr;
}

UINT CFlashEngine::DrawMetadataTextAboveVideo(SWFMovie *movie, Gdiplus::ARGB refTextColor, SWFFont *fnt)
{
   HRESULT hr = S_OK;

   CString csMetadata;
   csMetadata.Format(_T("<font size='14'>%s</font><br><font size='4'><br></font><font size='12'>%s</font>"), m_csTitle, m_csAuthor);

   float fLogoWidth = m_iLogoWidth;
   float fLogoHeight = m_iLogoHeight;
   float x = (float)FLASH_PADDING;
   float y = (fLogoHeight>0.0f) ? 
             (float)(FLASH_PADDING + fLogoHeight + FLASH_PADDING)
             : (float)(FLASH_PADDING);
   x += m_fMovieOffsetX;
   y += m_fMovieOffsetY;
   float width = m_rcMetadata.Width() - x;
   float height = m_rcMetadata.Height() - y;

   float fFontSize = 1.0f * FLASH_METADATAHEIGHT;

///   // Consider global scale factor
///   x *= m_dGlobalScale;
///   y *= m_dGlobalScale;
///   width *= m_dGlobalScale;
///   height *= m_dGlobalScale;
///   fFontSize *= m_dGlobalScale;


   SWFTextField *txtField = new SWFTextField();
   txtField->setFont(fnt);
   txtField->setBounds(width, height);
   txtField->setHeight(fFontSize);
   txtField->setColor((refTextColor >> 16) & 0xff, (refTextColor >> 8) & 0xff, (refTextColor >> 0) & 0xff);//, (refTextColor >> 24) & 0xff);
   txtField->setVariableName("text");

   txtField->setAlignment(SWFTEXTFIELD_ALIGN_LEFT);
   txtField->setVariableName("text");
   txtField->setFlags(SWFTEXTFIELD_USEFONT | SWFTEXTFIELD_HTML | SWFTEXTFIELD_NOEDIT | SWFTEXTFIELD_NOSELECT | SWFTEXTFIELD_HASLAYOUT | SWFTEXTFIELD_MULTILINE | SWFTEXTFIELD_WORDWRAP);
   
   char *utfText = LString::TCharToUtf8(csMetadata);
   char *utfInitialText = LString::TCharToUtf8(csMetadata);
   if (utfText)
   {
      txtField->addUTF8Chars(utfInitialText);
      txtField->addUTF8String(utfText);
      delete[] utfText;
      utfText = NULL;
      delete[] utfInitialText;
      utfInitialText = NULL;
   }

   SWFDisplayItem *di = NULL;
   di = movie->add(txtField);
   di ->moveTo(x, y);
   di->setDepth(15973); // above "dummy" background


   return hr;
}

UINT CFlashEngine::DrawDummyBackground(SWFMovie *movie, float x, float y, float width, float height, 
                                       Gdiplus::ARGB bgColor)
{
   HRESULT hr = S_OK;

   // x, y, width, height are coordinates of the slide area
   // --> we have to fill the "inverse" area with the background color 
   //     to compensate (freehand) drawings outside the whiteboard
   //     because we can't do real clipping here

   SWFDisplayItem *di = NULL;

   // 1000 pixel border should be enough to cover all objects outside the slide area
   float fBorder = 1000.0f;

   SWFShape *shpBg = new SWFShape();
   SWFFill *fillBg = shpBg->addSolidFill((bgColor >> 16) & 0xff, (bgColor >> 8) & 0xff, (bgColor >> 0) & 0xff); //, (bgColor >> 24) & 0xff);
   shpBg->setRightFill(fillBg);

   DrawRectangle(shpBg, x-fBorder-1.0f, y-fBorder-1.0f, width+(2*fBorder)+2.0f, fBorder);
   di = movie->add(shpBg);
   di->setDepth(15966); // below header / 3-pixel border / control bar

   DrawRectangle(shpBg, x-fBorder-1.0f, y+height+1.0f, width+(2*fBorder)+2.0f, fBorder);
   di = movie->add(shpBg);
   di->setDepth(15967);

   DrawRectangle(shpBg, x-fBorder-1.0f, y-1.0f, fBorder, height+2.0f);
   di = movie->add(shpBg);
   di->setDepth(15968);

   DrawRectangle(shpBg, x+width+1.0f, y-1.0f, fBorder, height+2.0f);
   di = movie->add(shpBg);
   di->setDepth(15969);

   return hr;
}

UINT CFlashEngine::DrawPlayerBackground(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   SWFShape *shpPlayerBackground = new SWFShape();

   CRect rcBackground;
   rcBackground.SetRectEmpty();

   if (m_bIsPspDesign)
   {
      rcBackground.UnionRect(rcBackground, m_rcPspContent);
      if (!m_rcControlBar.IsRectEmpty())
         rcBackground.UnionRect(rcBackground, m_rcControlBar);
   }
   else
   {
      if (!m_rcVideo.IsRectEmpty())
         rcBackground.UnionRect(rcBackground, m_rcVideo);
      if (!m_rcPages.IsRectEmpty())
         rcBackground.UnionRect(rcBackground, m_rcPages);
      if (!m_rcControlBar.IsRectEmpty())
         rcBackground.UnionRect(rcBackground, m_rcControlBar);
      rcBackground.InflateRect(1, 1);
   }

   SWFFill *fillPlayerBackground = 
      shpPlayerBackground->addSolidFill(m_rgb100.r, m_rgb100.g, m_rgb100.b);
   shpPlayerBackground->setRightFill(fillPlayerBackground);
   DrawRectangle(shpPlayerBackground, 0.0f, 0.0f, 
                                      rcBackground.Width(), rcBackground.Height());


   SWFDisplayItem *di = movie->add(shpPlayerBackground);
   di->setName("shpPlayerBackground");

   float x = rcBackground.left;
   float y = rcBackground.top;

   if (m_bIsPspDesign)
   {
      di->moveTo(x*m_fScalePspContent, y*m_fScalePspContent);
      di->scale(m_fScalePspContent, m_fScalePspContent);
   }
   else
   {
///      x *= m_dGlobalScale;
///      Y *= m_dGlobalScale;
      x += m_fMovieOffsetX;
      y += m_fMovieOffsetY;
      di->moveTo(x, y);
///      di->scale(m_dGlobalScale, m_dGlobalScale);
   }
   //_tprintf(_T("* di-depth: %d (Player Background)\n"), di->getDepth());

   return hr;
}

// --------------------------------------------------------------------------
// Audio
// --------------------------------------------------------------------------
UINT CFlashEngine::AddAudioToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   FILE *pFile;
   if (NULL == (pFile = _tfopen(m_tszMp3File, _T("rb"))))
   {
      hr = imc_lecturnity_converter_LPLibs_EMBED_FILE_MISSING;

      _TCHAR tszMessage[1024];
      _stprintf(tszMessage, _T("Error: Cannot open audio file! %s\n"), m_tszMp3File);
      _tprintf(tszMessage);
   }

   if (SUCCEEDED(hr))
   {
      SWFSoundStream *sound = new SWFSoundStream(pFile);
      movie->setSoundStream(sound);
      CString csMp3File(m_tszMp3File);
      m_mapTempFiles.SetAt(csMp3File, pFile);
   }

   return hr;
}

// --------------------------------------------------------------------------
// Video and Clips
// --------------------------------------------------------------------------
UINT CFlashEngine::AddVideoToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

    // Calculate position of the video 
   /*float fVideoX = m_fSlideOffsetX - m_iAccVideoWidth;
   if (m_iWbInputWidth > 0)
      fVideoX -= FLASH_PADDING;
   float fVideoY = m_fSlideOffsetY + m_iWbInputHeight - m_iAccVideoHeight;
   if (m_iWbInputHeight > 0)
      fVideoY -= FLASH_PADDING;
   if (m_bIsSlidestar)
   {
	   fVideoX = m_fSlideOffsetX - FLASH_BORDERSIZE - 40.0f - FLASH_BORDERSIZE - m_iAccVideoWidth;
	   fVideoY = m_fSlideOffsetY + (m_iWbInputHeight -m_iAccVideoHeight) / 2;
   }*/
   SWFDisplayItem *di = NULL;
   SWFSprite *sprVideo = new SWFSprite();

   SWFVideoStream *videostream = new SWFVideoStream();
   videostream->setVideoDimension(m_iAccVideoWidth, m_iAccVideoHeight);

   	/*if (m_bIsSlidestar)
	{
		SWFShape *shpBorder = new SWFShape();
		DrawBorderLines(shpBorder, -FLASH_BORDERSIZE, -FLASH_BORDERSIZE, m_iAccVideoWidth + 2 * FLASH_BORDERSIZE, m_iAccVideoHeight + 2 * FLASH_BORDERSIZE, D3D_NO_FILL);
		di = sprVideo->add(shpBorder);
	}*/
   di = sprVideo->add(videostream);
   di->setName("accVideo");
   sprVideo->nextFrame();

   float fVideoX; 
   float fVideoY;
   fVideoX = m_rcVideo.left + m_fMovieOffsetX;
   fVideoY = m_rcVideo.top + m_fMovieOffsetY;   
       
   _tprintf(_T("- video rect: %d,%d %dx%d\n"), m_rcVideo.left, m_rcVideo.top, m_rcVideo.Width(), m_rcVideo.Height());

///   if (!m_bIsPspDesign)
///   {
///      // Consider global scale factor
///      fVideoX *= m_dGlobalScale;
///      fVideoY *= m_dGlobalScale;
///   }

   di = movie->add(sprVideo);
   di->moveTo(fVideoX, fVideoY);
///   if (!m_bIsPspDesign)
///      di->scale(m_dGlobalScale, m_dGlobalScale);
///   else
///   {
///      float fScaleVideo = (float)m_rcVideo.Width() / (float)m_iAccVideoWidth;
///      di->scale(fScaleVideo, fScaleVideo);
///   }
   di->setName("swfSprVideo");
   di->setDepth(15899);
   //_tprintf(_T("* di-depth: %d (Video)\n"), di->getDepth());

   hr = AddActionScriptForVideoStreamingToMovie(movie);

   CString csVideoStreamUrl;
   csVideoStreamUrl.Format(_T("%s"), m_tszVideoFlvFile);
   StringManipulation::GetFilename(csVideoStreamUrl);

   if (_tcslen(m_tszRtmpPath) > 0) // RTMP
      StringManipulation::GetFilePrefix(csVideoStreamUrl);
   if(m_bIsSlidestar && !m_bIsNewLecPlayer)
   {
      csVideoStreamUrl = _T("document.flv");
   }
   hr = AddActionScriptForVideoStreamUrlToMovie(movie, csVideoStreamUrl);

   return hr;
}

UINT CFlashEngine::AddClipsToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   SWFDisplayItem *di = NULL;
   SIZE sizeClip;

   for (int i = 0; i < m_iNumOfClips; ++i)
   {
      if (SUCCEEDED(hr))
      {
         // Read video size from clip
         bool bSucceeded = VideoInformation::GetVideoSize(m_atszClipFiles[i], sizeClip);

         if (bSucceeded)
         {
            sizeClip = m_aSizeClips[i];

            float fDeltaX = 0.5f * ((float)(m_iWbOutputWidth - sizeClip.cx));
            float fDeltaY = 0.5f * ((float)(m_iWbOutputHeight - sizeClip.cy));
            if (fDeltaX < 0.0f)
               fDeltaX = 0.0f;
            if (fDeltaY < 0.0f)
               fDeltaY = 0.0f;

            float fClipX = m_rcPages.left + fDeltaX;
            float fClipY = m_rcPages.top + fDeltaY;

            SWFDisplayItem *di = NULL;
            SWFSprite *sprClip = new SWFSprite();

            // Add a "border" rectangle (Bugfix #3733)
            SWFShape *pShpBorder = new SWFShape();
            pShpBorder->setLine(1, 0x00, 0x00, 0x00);
            DrawRectangle(pShpBorder, -0.5f, -0.5f, sizeClip.cx+1.0f, sizeClip.cy+1.0f, 1.0f);
            di = sprClip->add(pShpBorder);

            // Add the clip stream
            SWFVideoStream *clipstream = new SWFVideoStream();
            clipstream->setVideoDimension(sizeClip.cx, sizeClip.cy);

            CString csClipName;
            csClipName.Format(_T("Clip%d"), i);
            di = sprClip->add(clipstream);
            di->setName(GetCharFromCString(csClipName));
            sprClip->nextFrame();

            fClipX += m_fMovieOffsetX;
            fClipY += m_fMovieOffsetY;

///            if (!m_bIsPspDesign)
///            {
///               // Consider global scale factor
///               fClipX *= m_dGlobalScale;
///               fClipY *= m_dGlobalScale;
///            }

            CString csSprClipName;
            csSprClipName.Format(_T("swfSprClip%d"), i);
            di = movie->add(sprClip);
            di->moveTo(fClipX, fClipY);
///            di->scale(m_dGlobalScale, m_dGlobalScale);
            di->setName(GetCharFromCString(csSprClipName));
            di->setDepth(15900 + i);
            //_tprintf(_T("* di-depth: %d (Clip)\n"), di->getDepth());
         }
      }
   }

   // ActionScript: Clip Stream URLs
   CString *acsClipStreamUrls = new CString[m_iNumOfClips]; 

   for (int i = 0; i < m_iNumOfClips; ++i)
   {
      acsClipStreamUrls[i].Format(_T("%s"), m_atszClipFlvFiles[i]);
      StringManipulation::GetFilename(acsClipStreamUrls[i]);

      if (_tcslen(m_tszRtmpPath) > 0) // RTMP
         StringManipulation::GetFilePrefix(acsClipStreamUrls[i]);
      
      if(m_bIsSlidestar && !m_bIsNewLecPlayer)
      {
         acsClipStreamUrls[i].Format(_T("clip_%d.flv"), i);
      }
   }

   hr = AddActionScriptForClipStreamUrlsToMovie(movie, acsClipStreamUrls);

   hr = AddActionScriptForClipStreamingToMovie(movie);
   return hr;
}

UINT CFlashEngine::AddNetConnectionToMovie(SWFMovie *movie)
{
   HRESULT hr = S_OK;

   // ActionScript: Net Connection
   CString csNetConnection;
   csNetConnection.Empty();

   if (_tcslen(m_tszRtmpPath) > 0) // RTMP
      csNetConnection.SetString(m_tszRtmpPath);
   else // Local, HTTP: null
      csNetConnection.SetString(_T(""));

   if (SUCCEEDED(hr))
      hr = AddActionScriptForNetConnectionToMovie(movie, csNetConnection);

   if (SUCCEEDED(hr))
      hr = AddActionScriptForFlvStreamsToMovie(movie);

   return hr;
}

// --------------------------------------------------------------------------
// WhiteboardObjects
// --------------------------------------------------------------------------
UINT CFlashEngine::AddWbObjectsToMovie(SWFMovie *movie, int nMsecPerFrame)
{
   HRESULT hr = S_OK;

   // Use Sprite for Pages
   SWFDisplayItem *di = NULL;
   SWFSprite *sprMaskLayer = NULL;
   SWFSprite *sprPages = NULL;
   if (SUCCEEDED(hr))
   {
      // Bugfix 4799:
      // Create a mask layer for the pages
      // This prevents drawing content outside the page area
      sprMaskLayer = new SWFSprite();
      SWFShape *shpMask = new SWFShape();
      SWFFill *fillQ1 = shpMask->addSolidFill(0x80, 0x80, 0x80);
      shpMask->setRightFill(fillQ1);
      shpMask->movePenTo(0.0f, 0.0f);
      shpMask->drawLine(m_rcPages.Width(), 0.0f);
      shpMask->drawLine(0.0f, m_rcPages.Height());
      shpMask->drawLine(-m_rcPages.Width(), 0.0f);
      shpMask->drawLineTo(0.0f, 0.0f);
      di = sprMaskLayer->add(shpMask);
      sprMaskLayer->nextFrame();
      di = movie->add(sprMaskLayer);
      di->setName("swfSprMaskLayer");

      // Now create the Sprite for Pages
      sprPages = new SWFSprite();
   }

   //
   // Try to create the movie
   //

   // the boundary leaves some room (below 16000) for the replay controls et al.
   int nObjectBoundary = FLASH_OBJECT_BOUNDARY; 
   int nInteractionsBoundary = FLASH_INTERACTIONS_BOUNDARY;
   DWORD dwAddedObjects = 0;
   DWORD dwAddedObjects2 = 0;

   DWORD anfTime = timeGetTime();

   // first make a dry run to check how many objects would be used 
   _tprintf(_T("1. run (dry, unreducing): dwAddedObjects: %d; m_iMaxIndexSoFar: %d\n"), dwAddedObjects, m_iMaxIndexSoFar);
   if (SUCCEEDED(hr) && !m_bCancelRequested)
      hr = EmbedObjects(sprPages, true, false, nMsecPerFrame, &dwAddedObjects);
   _tprintf(_T(">> ... (dwAddedObjects: %d) --> Done(hr=%d)\n"), dwAddedObjects, hr);
   
   // Update MaxIndexSoFar 4x: Is used as depth counter for interactions lateron!
   if (SUCCEEDED(hr))
      m_iMaxIndexSoFar = (int)dwAddedObjects;

   DWORD endTime = timeGetTime();

   _RPT2(_CRT_WARN, "Embed Time (dry, unreducing): %dms; #Objects: %d\n", endTime-anfTime, dwAddedObjects);

   // Now continue creating the movie
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      if (dwAddedObjects > nObjectBoundary)
      {
         // now make a dry run to check if ignoring some objects
         // (LINE, POLYGON; displayed/used during the actual painting process)
         // helps in reducing the object count
         anfTime = timeGetTime();
         _tprintf(_T("2. run (dry, reducing): %d objects (max: %d), dwAddedObjects: %d, m_iMaxIndexSoFar: %d\n"), dwAddedObjects, nObjectBoundary, dwAddedObjects, m_iMaxIndexSoFar);
         hr = EmbedObjects(sprPages, true, true, nMsecPerFrame, &dwAddedObjects2);
         _tprintf(_T(">> ... (dwAddedObjects2: %d) --> Done(hr=%d)\n"), dwAddedObjects2, hr);
         endTime = timeGetTime();

         _RPT2(_CRT_WARN, "Embed Time (dry, reducing): %dms; #Objects: %d\n", endTime-anfTime, dwAddedObjects2);


         if (SUCCEEDED(hr) && !m_bCancelRequested)
         {
            m_iMaxIndexSoFar = (int)dwAddedObjects2;

            if (dwAddedObjects2 > nObjectBoundary)
               hr = imc_lecturnity_converter_LPLibs_DOCUMENT_TOO_COMPLEX;
            else
            {
               // reducing object complexity did help: make a reduced conversion
               anfTime = timeGetTime();
               _tprintf(_T("3. run (undry, reducing): %d objects (max: %d), m_iMaxIndexSoFar: %d\n"), dwAddedObjects2, nObjectBoundary, m_iMaxIndexSoFar);
               if (SUCCEEDED(hr))
                  hr = AddOtherPageRelatedContent(movie, sprPages);
               if (SUCCEEDED(hr))
                  hr = EmbedObjects(sprPages, false, true, nMsecPerFrame, &dwAddedObjects2);
               if (hr > 0)
                  m_hrPossibleWarning = m_hrPossibleWarning | hr;

               if (SUCCEEDED(hr))
                  m_iMaxIndexSoFar = (int)dwAddedObjects2;

               _tprintf(_T(">> ... (dwAddedObjects2: %d) --> Done(hr=%d)\n"), dwAddedObjects2, hr);
               endTime = timeGetTime();

               _RPT2(_CRT_WARN, "Embed Time (undry, reducing): %dms; #Objects: %d\n", endTime-anfTime, dwAddedObjects2);
            }
         }
      }
      else
      {
         // everything alright: make a normal conversion
         anfTime = timeGetTime();

         dwAddedObjects = 0;
         _tprintf(_T("4. run (undry, unreducing): %d objects (max: %d), m_iMaxIndexSoFar: %d\n"), dwAddedObjects, nObjectBoundary, m_iMaxIndexSoFar);
         if (SUCCEEDED(hr))
            hr = AddOtherPageRelatedContent(movie, sprPages);
         if (SUCCEEDED(hr))
            hr = EmbedObjects(sprPages, false, false, nMsecPerFrame, &dwAddedObjects);
         if (hr > 0)
            m_hrPossibleWarning = m_hrPossibleWarning | hr;

         if (SUCCEEDED(hr))
            m_iMaxIndexSoFar = (int)dwAddedObjects;

         _tprintf(_T(">> ... (dwAddedObjects: %d) --> Done(hr=%d)\n"), dwAddedObjects, hr);

         endTime = timeGetTime();

         _RPT2(_CRT_WARN, "Embed Time (undry, unreducing): %dms; #Objects: %d\n", endTime-anfTime, dwAddedObjects);
      }

      // This should not happen: Max index exceeds the maximum of possible indices
      _tprintf(_T(">> ... max Object Index so far: %d\n"), m_iMaxIndexSoFar);
      _tprintf(_T(">> ... max Interactions Index so far: %d\n"), m_iMaxInteractionIndex);
      if (m_iMaxIndexSoFar > FLASH_OBJECT_BOUNDARY)
         hr = imc_lecturnity_converter_LPLibs_DOCUMENT_TOO_COMPLEX;
      if (m_iMaxInteractionIndex > FLASH_INTERACTIONS_BOUNDARY)
         hr = imc_lecturnity_converter_LPLibs_DOCUMENT_TOO_COMPLEX;
   }

   // NEW: Add sprite to movie
   di = movie->add(sprPages);
   di->setName("swfSprPages");

   //float fPageScale = (2.0f / 3.0f);

   float fPosX = m_rcPages.left + m_fMovieOffsetX;
   float fPosY = m_rcPages.top + m_fMovieOffsetY;

///   if (!m_bIsPspDesign)
///   {
///      // Consider global scale factor
///      fPosX *= m_dGlobalScale;
///      fPosY *= m_dGlobalScale;
///   }

   di->moveTo(fPosX, fPosY);
   if (!m_bIsPspDesign)
      di->scale(m_dGlobalScale, m_dGlobalScale);
   else
   {
      float fScalePages = (float)m_rcPages.Width() / (float)m_iWbInputWidth;
      di->scale(fScalePages, fScalePages);
   }
   //di->scaleTo(fPageScale, fPageScale);

   for (int i = 0; i < m_iAbsoluteFramesCount; ++i)
      movie->nextFrame();


   //_TCHAR tszMessage[1024];
   //_stprintf(tszMessage, _T("Objekte ursprünglich hinzugefügt: %d %dms"), dwAddedObjects, endTime-anfTime);
   //::MessageBox(NULL, tszMessage, NULL, MB_OK);
   //_TCHAR tszMessage[1024];
   //_stprintf(tszMessage, _T("Objekte schlußendlich hinzugefügt: %d %dms"), dwAddedObjects2, endTime2-anfTime2);
   //::MessageBox(NULL, tszMessage, NULL, MB_OK);
   DWORD dwAddedObjects3 = (dwAddedObjects > nObjectBoundary) ? dwAddedObjects2 : dwAddedObjects;
   DWORD dwInteractionObjects = (m_iMaxInteractionIndex > 0) ? (m_iMaxInteractionIndex-dwAddedObjects3-100) : 0;
   _tprintf(_T("Objects originally added:  %d\n"), dwAddedObjects);
   _tprintf(_T("Objects finally added:     %d\n"), dwAddedObjects3);
   _tprintf(_T("Interaction objects added: %d\n"), dwInteractionObjects);

   //_tprintf(_T("- CMap Images: %d objects.\n"), m_mapImageNames.GetCount());
   //_tprintf(_T("- CMap FNames: %d objects.\n"), m_mapFontNames.GetCount());
   //_tprintf(_T("- CMap Fonts:  %d objects:\n"), m_mapSwfFonts.GetCount());

   return hr;
}

UINT CFlashEngine::AddOtherPageRelatedContent(SWFMovie *movie, SWFSprite *sprPages)
{
	HRESULT hr = S_OK;

   /*if (m_bIsSlidestar)
	{
		SWFShape *shpBorder = new SWFShape();
		DrawBorderLines(shpBorder, -FLASH_BORDERSIZE, -FLASH_BORDERSIZE, m_iWbInputWidth + 2 * FLASH_BORDERSIZE, m_iWbInputHeight + 2 * FLASH_BORDERSIZE, D3D_NO_FILL);
		SWFDisplayItem *di = NULL;
      di = sprPages->add(shpBorder);
      di->setDepth(15971);
	}*/

   // Draw Eval note?
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      if (m_bDrawEvalNote)
         hr = DrawEvalNote(movie);
   }

   // Intermediate step: Interaction stuff
   if (SUCCEEDED(hr) && !m_bCancelRequested)
   {
      hr = AddInteractionToMovie(movie, sprPages);
   }

   if (SUCCEEDED(hr) && !m_bCancelRequested)
      AddActionScriptForPageSprite(sprPages);

   return hr;
}

// --------------------------------------------------------------------------
// General MessageBox for Error/Warning messages ...
// --------------------------------------------------------------------------
UINT CFlashEngine::DrawMessageBox(SWFMovie *movie)
{
	HRESULT hr = S_OK;

   // Hard coded default values
   float width = 340.0f; 
   float height = 180.0f;
   float okBtnWidth = 80.0f;
   float okBtnHeight = 20.0f;
   float okBtnX = (0.5f * (width - okBtnWidth));
   float okBtnY = height - okBtnHeight - 16.0f;
   float fScaleX = (m_rcMovie.Width()-width < 0.0f) ? (m_rcMovie.Width()/width) : 1.0f;
   float fScaleY = (m_rcMovie.Height()-height < 0.0f) ? (m_rcMovie.Height()/height) : 1.0f;
   float fScale = (fScaleX <fScaleY) ? fScaleX : fScaleY;
   float x = (0.5f * (m_rcMovie.Width() - width*fScale));
   float y = (0.5f * (m_rcMovie.Height() - height*fScale));

   SWFDisplayItem *di;
   SWFSprite *sprMessageBox = new SWFSprite();

   // Create the background: a white (rounded) rectangle with a border line
   SWFSprite *pSprWindow = new SWFSprite();
   MYRGB rgbBorder, rgbBackground;
   rgbBorder.r = 0x0A; rgbBorder.g = 0x3F, rgbBorder.b = 0x67;
   rgbBackground.r = 0xFF; rgbBackground.g = 0xFF, rgbBackground.b = 0xFF;
   float fRadius = 0.0f; //8.0f;
   SWFSprite *pSprBackground = new SWFSprite();
   SWFShape *pShapeMessageBackground = new SWFShape();
   Draw3DRoundedRectangle(pShapeMessageBackground, 0.0f, 0.0f, width, height, fRadius, 1, D3D_NO_PEN_PALETTE, D3D_FULL_ROUND, 
      D3D_NO_BORDER, D3D_FILL, D3D_DEEPENED, D3D_NO_GRADIENT, NULL, NULL, &rgbBackground);
   SWFSprite *pSprBorder = new SWFSprite();
   SWFShape *pShapeMessageBorder = new SWFShape();
   Draw3DRoundedRectangle(pShapeMessageBorder, 0.0f, 0.0f, width, height, fRadius, 1, D3D_NO_PEN_PALETTE, D3D_FULL_ROUND, 
      D3D_BORDER, D3D_NO_FILL, D3D_DEEPENED, D3D_NO_GRADIENT, NULL, &rgbBorder, NULL);
   di = pSprBackground->add(pShapeMessageBackground);
   di->setName("swfShpMsgBackground");
   pSprBackground->nextFrame();
   di = pSprBorder->add(pShapeMessageBorder);
   di->setName("swfShpMsgBorder");
   pSprBorder->nextFrame();
   di = pSprWindow->add(pSprBackground);
   di->setName("swfSprMsgBackground");
   di = pSprWindow->add(pSprBorder);
   di->setName("swfSprMsgBorder");
   pSprWindow->nextFrame();
   di = sprMessageBox->add(pSprWindow);

   // Create a 'Message' textfield
   float msgFontSize = 14.0f;
   float msgTxtX = 16.0f;
   float msgTxtY = 16.0f;
   float msgTxtWidth = width - 32.0f;
   float msgTxtHeight = height - (16.0f+20.0f);
   SWFTextField *txtFieldMsg = new SWFTextField();
   txtFieldMsg->setFont(m_pFntStd);
   txtFieldMsg->setBounds(msgTxtWidth, msgTxtHeight);
   txtFieldMsg->setHeight(msgFontSize);
   txtFieldMsg->setColor(0x0A, 0x3F, 0x67);
   txtFieldMsg->align(SWFTEXTFIELD_ALIGN_CENTER);
   txtFieldMsg->setFlags(SWFTEXTFIELD_NOEDIT | SWFTEXTFIELD_HASLAYOUT | SWFTEXTFIELD_HTML | 
                         SWFTEXTFIELD_MULTILINE | SWFTEXTFIELD_WORDWRAP |SWFTEXTFIELD_USEFONT);
   txtFieldMsg->setVariableName("textMsg");

   CString csInitialText = _T(" ");
   CString csText = _T("!");
   CString csResourceText;
   csResourceText.LoadString(IDS_ERROR);
   csText += csResourceText;
   csResourceText.LoadString(IDS_WARNING);
   csText += csResourceText;
   csResourceText.LoadString(IDS_FLV_NOT_FOUND_ERROR);
   csText += csResourceText;
   csResourceText.LoadString(IDS_FLV_STREAMING_ERROR);
   csText += csResourceText;
   csResourceText.LoadString(IDS_SERVER_ERROR);
   csText += csResourceText;
   csResourceText.Format(_T("%s"), szAllowedUrlCharacters);
   csText += csResourceText;
   if (m_bHasVideo && m_tszVideoFlvFile != NULL)
   {
      csResourceText.Format(_T("%s"), m_tszVideoFlvFile);
      csText += csResourceText;
   }
   if (m_bHasClips && m_atszClipFlvFiles != NULL)
   {
      for (int i = 0; i < m_iNumOfClips; ++i)
      {
         csResourceText.Format(_T("%s"), m_atszClipFlvFiles[i]);
         csText += csResourceText;
      }
   }

   char *utfText = LString::TCharToUtf8(csText);
   char *utfInitialText = LString::TCharToUtf8(csInitialText);
   if (utfText)
   {
      txtFieldMsg->addUTF8Chars(utfText);
      txtFieldMsg->addUTF8String(utfInitialText);
      delete[] utfText;
      utfText = NULL;
      delete[] utfInitialText;
      utfInitialText = NULL;
   }
   di = sprMessageBox->add(txtFieldMsg);
   di->setName("swfTxtFeedback");
   di->moveTo(msgTxtX, msgTxtY);

   // Create the 'OK' button
   // - Create the button shape
   SWFShape *shpBtn = new SWFShape();
   shpBtn->setLine(1, 0x0A, 0x3F, 0x67);
   SWFFill *fillBtn = shpBtn->addSolidFill(0xEE, 0xEE, 0xEE);
   shpBtn->setRightFill(fillBtn);
   DrawRectangle(shpBtn, okBtnX, okBtnY, okBtnWidth, okBtnHeight);
   di = sprMessageBox->add(shpBtn);
   // - Write 'OK' into a text field
   float okFontSize = 12.0f;
   SWFTextField *txtFieldOk = new SWFTextField();
   txtFieldOk->setFont(m_pFntStd);
   txtFieldOk->setBounds(okBtnWidth, okFontSize * 1.5f);
   txtFieldOk->setHeight(okFontSize);
   txtFieldOk->setColor(0x00, 0x00, 0x00);
   txtFieldOk->align(SWFTEXTFIELD_ALIGN_CENTER);
   txtFieldOk->setFlags(SWFTEXTFIELD_NOEDIT | SWFTEXTFIELD_USEFONT);
   txtFieldOk->addUTF8Chars("OK");
   txtFieldOk->addUTF8String("OK");
   di = sprMessageBox->add(txtFieldOk);
   di->moveTo(okBtnX, okBtnY + 0.5 * (okBtnHeight - okFontSize));
   // - Create a transparent button
   fillBtn = shpBtn->addSolidFill(0x00, 0x00, 0x00, 0x00);
   shpBtn->setRightFill(fillBtn);
   DrawRectangle(shpBtn, okBtnX, okBtnY, okBtnWidth, okBtnHeight);
   SWFButton *btnOk = new SWFButton();
   btnOk->addShape(shpBtn, SWFBUTTON_HIT);
   // Add ActionScript to the 'OK' button
   _TCHAR script[1024];
   _stprintf(script, _T("\
      this.useHandCursor = true;\
      var objRoot = _parent;\
      objRoot.closeMessageBox();\
      objRoot.swfSprTooltip._visible = false;\
   "));
   PrintActionScript("MessageBox OK Button", script);
   btnOk->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEDOWN);
   _stprintf(script, _T("\
      this.useHandCursor = true;\
   "));
   PrintActionScript("MessageBox OK Button", script);
   btnOk->addAction(new SWFAction(GetCharFromCString(script)), SWFBUTTON_MOUSEOVER);
   di = sprMessageBox->add(btnOk);


   sprMessageBox->nextFrame();
   di = movie->add(sprMessageBox);
   di->moveTo(x, y);
   di->scale(fScale, fScale);
   di->setName("swfSprMessageBox");
   //di->setDepth(15982);

   // Create a dummy Sprite to flip the depth lateron
   SWFSprite *sprDummy = new SWFSprite();
   sprDummy->nextFrame();
   di = movie->add(sprDummy);
   di->setName("swfSprMessageBox_Dummy");
   di->setDepth(15982);


   return hr;
}

// --------------------------------------------------------------------------
// Testing Messages
// --------------------------------------------------------------------------
UINT CFlashEngine::DrawTestingFeedback(SWFSprite *sprPages)
{
   HRESULT hr = S_OK;

   SWFDisplayItem *di;
   // Create a new sprite
   SWFSprite *pSprite = new SWFSprite();

   // Hard coded default values
   float width = 280.0f; 
   float height = 240.0f; //140.0f;
   float okBtnWidth = m_pOkButton->GetActivityArea().Width();//80.0f;
   float okBtnHeight = m_pOkButton->GetActivityArea().Height(); //20.0f;
   float x = (0.5f * (m_iWbInputWidth - width));
   float y = (0.5f * (m_iWbInputHeight - height));

   _TCHAR script[1024];

   // Create a "big" transparent button as a "protective layer" --> simulate a modal dialog
   SWFSprite *pSprBtnP = new SWFSprite();
   SWFButton *btnProtection = new SWFButton();
   SWFShape *shpBtnP = new SWFShape();
   SWFFill *fillBtnP = shpBtnP->addSolidFill(0x00, 0x00, 0x00, 0x00);
   shpBtnP->setRightFill(fillBtnP);
   //DrawRectangle(shpBtnP, 0.0f, 0.0f, m_fMovieWidth, m_fMovieHeight);
   DrawRectangle(shpBtnP, 0.0f, 0.0f, (float)m_iWbInputWidth, (float)m_iWbInputHeight);
   btnProtection->addShape(shpBtnP, SWFBUTTON_HIT | SWFBUTTON_OVER | SWFBUTTON_DOWN | SWFBUTTON_UP);
   di = pSprBtnP->add(btnProtection);
   di->setName("swfBtnProtection");
   _stprintf(script, _T("\
      swfBtnProtection.useHandCursor = false;\
   "));
   PrintActionScript(_T("Protection Button"), script);
   pSprBtnP->add(new SWFAction(GetCharFromCString(script)));
   pSprBtnP->nextFrame();
   di = sprPages->add(pSprBtnP);
   di->moveTo(0.0f, 0.0f);
   di->setName("swfSprProtection");
   di->setDepth(15977);

   // Create the background: a white (rounded) rectangle with a border line
   SWFSprite *pSprWindow = new SWFSprite();
   MYRGB rgbBorder, rgbBackground;
   rgbBorder.r = 0x0A; rgbBorder.g = 0x3F, rgbBorder.b = 0x67;
   rgbBackground.r = 0xFF; rgbBackground.g = 0xFF, rgbBackground.b = 0xFF;
   float fRadius = 0.0f; //8.0f;
   SWFSprite *pSprBackground = new SWFSprite();
   SWFShape *pShapeTestingBackground = new SWFShape();
   Draw3DRoundedRectangle(pShapeTestingBackground, 0.0f, 0.0f, width, height, fRadius, 1, D3D_NO_PEN_PALETTE, D3D_FULL_ROUND, 
      D3D_NO_BORDER, D3D_FILL, D3D_DEEPENED, D3D_NO_GRADIENT, NULL, NULL, &rgbBackground);
   SWFSprite *pSprBorder = new SWFSprite();
   SWFShape *pShapeTestingBorder = new SWFShape();
   Draw3DRoundedRectangle(pShapeTestingBorder, 0.0f, 0.0f, width, height, fRadius, 1, D3D_NO_PEN_PALETTE, D3D_FULL_ROUND, 
      D3D_BORDER, D3D_NO_FILL, D3D_DEEPENED, D3D_NO_GRADIENT, NULL, &rgbBorder, NULL);
   di = pSprBackground->add(pShapeTestingBackground);
   di->setName("swfShpBackground");
   pSprBackground->nextFrame();
   di = pSprBorder->add(pShapeTestingBorder);
   di->setName("swfShpBorder");
   pSprBorder->nextFrame();
   di = pSprWindow->add(pSprBackground);
   di->setName("swfSprBackground");
   di = pSprWindow->add(pSprBorder);
   di->setName("swfSprBorder");
   pSprWindow->nextFrame();
   di = sprPages->add(pSprWindow);
   di->moveTo(x, y);
   di->setName("swfSprTestingWindow");
   di->setDepth(15978);

   // Create a 'Feedback' textfield
   float msgFontSize = 14.0f;
   float msgTxtX = 16.0f;
   float msgTxtY = 16.0f; // + 0.5f * (height - 32.0f - okBtnHeight - msgFontSize);
   float msgTxtWidth = width - 32.0f;
   float msgTxtHeight = msgFontSize * 1.5f;
   SWFTextField *txtFieldMsg = new SWFTextField();
   txtFieldMsg->setFont(m_pFntStd);
   txtFieldMsg->setBounds(msgTxtWidth, msgTxtHeight);
   txtFieldMsg->setHeight(msgFontSize);
   txtFieldMsg->setColor(0x0A, 0x3F, 0x67);
   txtFieldMsg->align(SWFTEXTFIELD_ALIGN_CENTER);
   txtFieldMsg->setFlags(SWFTEXTFIELD_NOEDIT | SWFTEXTFIELD_HASLAYOUT | SWFTEXTFIELD_HTML | 
                         SWFTEXTFIELD_MULTILINE | SWFTEXTFIELD_WORDWRAP |SWFTEXTFIELD_USEFONT);
   txtFieldMsg->setVariableName("textMsg");

   CString csCorrect, csWrong; 
   CString csResult, csQuestion, csAnswer, csPoints; 
   CString csMaximumPoints, csRequiredPoints, csRequiredPercentage;
   CString csAchievedPoints, csAchievedPercentage;

   // Init strings with predefined items from the resource 
   // (if the next step should fail)
   csResult.LoadString(IDS_TESTING_RESULT);
   csQuestion.LoadString(IDS_TESTING_QUESTION);
   csAnswer.LoadString(IDS_TESTING_ANSWER);
   csPoints.LoadString(IDS_TESTING_POINTS);
   csMaximumPoints.LoadString(IDS_TESTING_MAXIMUM_POINTS);
   csRequiredPoints.LoadString(IDS_TESTING_REQUIRED_POINTS);
   csRequiredPercentage.LoadString(IDS_TESTING_REQUIRED_PERCENTAGE);
   csAchievedPoints.LoadString(IDS_TESTING_ACHIEVED_POINTS);
   csAchievedPercentage.LoadString(IDS_TESTING_ACHIEVED_PERCENTAGE);

   // Get the items from the document itself
   FillFeedbackSummaryStrings(csResult, csQuestion, csAnswer, 
                              csPoints, csMaximumPoints, 
                              csRequiredPoints, csRequiredPercentage, 
                              csAchievedPoints, csAchievedPercentage);

   CString csInitialText = _T(" ");
   CString csResourceText;
   CString csText = m_csFeedbackMsg;
   csText += csResult;
   csText += csQuestion;
   csText += csAnswer;
   csText += csPoints;
   csText += csMaximumPoints;
   csText += csRequiredPoints;
   csText += csRequiredPercentage;
   csText += csAchievedPoints;
   csText += csAchievedPercentage;
   csResourceText.LoadString(IDS_TESTING_PASSED);
   csText += csResourceText;
   csResourceText.LoadString(IDS_TESTING_FAILED);
   csText += csResourceText;
   csResourceText.LoadString(IDS_TESTING_CORRECT);
   csText += csResourceText;
   csResourceText.LoadString(IDS_TESTING_WRONG);
   csText += csResourceText;

   char *utfText = LString::TCharToUtf8(csText);
   char *utfInitialText = LString::TCharToUtf8(csInitialText);
   if (utfText)
   {
      txtFieldMsg->addUTF8Chars(utfText);
      txtFieldMsg->addUTF8String(utfInitialText);
      delete[] utfText;
      utfText = NULL;
      delete[] utfInitialText;
      utfInitialText = NULL;
   }
   di = pSprite->add(txtFieldMsg);
   di->setName("swfTxtFeedback");
   di->moveTo(msgTxtX, msgTxtY);

   // Create a 'summary' textfield
   msgFontSize = 14.0f;
   float sumTxtX = 16.0f;
   float sumTxtY = 16.0f + msgTxtHeight + 20.0f;
   float sumTxtWidth = msgTxtWidth;
   float sumTxtHeight = height - 32.0f - msgTxtHeight - 20.0f - okBtnHeight;
   SWFTextField *txtFieldSum = new SWFTextField();
   txtFieldSum->setFont(m_pFntStd);
   txtFieldSum->setBounds(sumTxtWidth, sumTxtHeight);
   txtFieldSum->setHeight(msgFontSize);
   txtFieldSum->setColor(0x00, 0x00, 0x00);
   txtFieldSum->align(SWFTEXTFIELD_ALIGN_CENTER);
   txtFieldSum->setFlags(SWFTEXTFIELD_NOEDIT | SWFTEXTFIELD_HASLAYOUT | SWFTEXTFIELD_HTML | 
                         SWFTEXTFIELD_MULTILINE | SWFTEXTFIELD_WORDWRAP |SWFTEXTFIELD_USEFONT);
   txtFieldSum->setVariableName("textSum");

   csInitialText = _T(" ");
   utfText = LString::TCharToUtf8(m_csFeedbackMsg);
   utfInitialText = LString::TCharToUtf8(csInitialText);
   if (utfText)
   {
      txtFieldSum->addUTF8Chars(utfText);
      txtFieldSum->addUTF8String(utfInitialText);
      delete[] utfText;
      utfText = NULL;
      delete[] utfInitialText;
      utfInitialText = NULL;
   }

   di = pSprite->add(txtFieldSum);
   di->setName("swfTxtSummary");
   di->moveTo(sumTxtX, sumTxtY);

   // Create the 'OK' button
   _stprintf(script, _T("\
      var objRoot = _parent._parent._parent;\
      objRoot.closeFeedbackMessage();\
      objRoot.swfSprTooltip._visible = false;\
   "));
   SWFSprite *pSprBtnOk = new SWFSprite();
   DrawInteractionArea(pSprBtnOk, 0.0f, 0.0f, okBtnWidth, okBtnHeight, -1, m_pOkButton, script, NULL, NULL, NULL, NULL);

   di = pSprite->add(pSprBtnOk);
   di->setName("swfSprOk");
   float okBtnX = 0.5f * (width - okBtnWidth);
   float okBtnY = height - 16.0f - okBtnHeight;
   di->moveTo(okBtnX, okBtnY);

   pSprite->nextFrame();
   
   di = sprPages->add(pSprite);
   di->moveTo(x, y);
   di->setName("swfSprTestingFeedback");
   di->setDepth(15979);

   // Rescale Feedback, if necessary
   di->scale(m_dFeedbackScale);

   return hr;
}


// --------------------------------------------------------------------------
// Read ActionScript from file (not used so far)
// --------------------------------------------------------------------------

UINT CFlashEngine::ReadActionScriptFromFile(CString csFilename, _TCHAR* tszScript)
{
   HRESULT hr = S_OK;

   FILE *pFile;
   if ( (pFile = _tfopen(csFilename, _T("rb"))) != NULL)
   {
      int nBytesRead = fread(tszScript, sizeof(char), MAX_ACTIONSCRIPT_BUFFER, pFile);
      tszScript[nBytesRead] = _T('\0');
      fclose(pFile);
   }
   else
   {
      // File not found
      m_csErrorDetail.Empty();
      m_csErrorDetail += csFilename;
         
      hr = imc_lecturnity_converter_LPLibs_INPUT_FILE_MISSING;
   }

   return hr;
}













