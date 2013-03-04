#include "stdafx.h"

#include <math.h>
#include <io.h>
#include <stdio.h>

#include "FlashEngine.h"
#include "FlashEngineHelper.h"
#include "DrawObjects.h"

#include "mingpp.h"

#include "lutils.h"
#include "KeyGenerator.h"

// --------------------------------------------------------------------------
// Simple geometric shapes
// --------------------------------------------------------------------------

#define EMBED_FILE_MISSING 200
#define INPUT_FILE_MISSING 201


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


/*
 * Note: m_fScaleX and m_fScaleY are currently always 1.0f, as Flash is able to scale itself
 * (that's its purpose).
 */

void CFlashEngine::DrawLine(SWFShape *pShape, float x, float y, float width, float height, 
                            float fLineWidth, bool bDashedLine, bool bDottedDash)
{
   x *= m_fScale; y *= m_fScale;
   width *= m_fScale; height *= m_fScale;

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
                              float fLineWidth, bool bDashedLine, bool bDottedDash)
{
   DrawLine(pShape, x1, y1, x2-x1, y2-y1, fLineWidth, bDashedLine, bDottedDash);
}

void CFlashEngine::DrawRectangle(SWFShape *pShape, float x, float y, float width, float height,
                                 float fLineWidth, bool bDashedLine, bool bDottedDash)
{
   DrawLine(pShape, x, y, width, 0, fLineWidth, bDashedLine, bDottedDash);
   DrawLine(pShape, x+width, y, 0, height, fLineWidth, bDashedLine, bDottedDash);
   DrawLine(pShape, x+width, y+height, -width, 0, fLineWidth, bDashedLine, bDottedDash);
   DrawLine(pShape, x, y+height, 0, -height, fLineWidth, bDashedLine, bDottedDash);
}

void CFlashEngine::DrawOval(SWFShape *pShape, float x, float y, float width, float height,
                            float fLineWidth, bool bDashedLine, bool bDottedDash)
{
   width = (m_fScale * width) / 2.0f;
   height = (m_fScale * height) / 2.0f;
   x = (m_fScale * x) + width;
   y = (m_fScale * y) + height;

   float fExtent = 2.0f*PI*((width+height)/2.0f);
   int iDivisor = 4;
   if (fLineWidth > 0.0f)
   {
      // these are "magic" values/calculations, they seem to produce reasonable results
      if (fLineWidth < 4.0f)
         fLineWidth = 4.0f; // otherwise points/lines are too tight together

      float fRawDivisor = fExtent/(4.0f*fLineWidth); 
      iDivisor = (int)(fRawDivisor);
      if (iDivisor <= 1)
         iDivisor = 2; // otherwise xrCtrl and yrCtrl get very very large

      if (bDottedDash)
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
      
         pShape->drawLine(1,1);
      }

      if (bDashedLine)
         bSwitch = !bSwitch;
   }
}

void CFlashEngine::DrawLineAndArrowHeads(SWFShape *pShape, 
                   int nArrowStyle, int nArrowConfig, 
                   float x, float y, float dx, float dy, 
                   float fLineWidth, bool bDashedLine, bool bDottedDash, Gdiplus::ARGB fcref)
{
/*
   bool bOpen = false;

   if (size < 2)
      size = 2;

   x *= m_fScaleX; y *= m_fScaleY;
   dx *= m_fScaleX; dy *= m_fScaleY;
   size *= min(m_fScaleX, m_fScaleY);


   float fVectorLength = sqrt(dx*dx + dy*dy);
   float fFactor = size/fVectorLength;

   float fALength = 3.0f;
   float fAWidth = 1.4f;

   float dpoint1x = fALength*fFactor*(+dx) + fAWidth*fFactor*(+dy);
   float dpoint1y = fALength*fFactor*(+dy) + fAWidth*fFactor*(-dx);
   
   float dpoint2x = fALength*fFactor*(+dx) + fAWidth*fFactor*(-dy);
   float dpoint2y = fALength*fFactor*(+dy) + fAWidth*fFactor*(+dx);

   if (bOpen)
   {
      pShape->movePenTo(x, y);
      pShape->drawLine(dpoint1x, dpoint1y);
      pShape->movePenTo(x, y);
      pShape->drawLine(dpoint2x, dpoint2y);
   }
   else
   {
      SWFFill *pFill = pShape->addSolidFill((fcref >> 16) & 0xff, 
         (fcref >> 8) & 0xff, (fcref >> 0) & 0xff, (fcref >> 24) & 0xff);
      pShape->setRightFill(pFill);

      pShape->movePenTo(x, y);
      pShape->drawLine(dpoint1x, dpoint1y);
      pShape->drawLine(dpoint2x-dpoint1x, dpoint2y-dpoint1y);
      pShape->drawLine(-dpoint2x, -dpoint2y);
   
   }
*/


   x *= m_fScale; y *= m_fScale;
   dx *= m_fScale; dy *= m_fScale;
   fLineWidth *=m_fScale;


   // Resolve nArrowConfig into its components
   int nBeginType = nArrowConfig / 1000;
   int nBeginSize = (nArrowConfig % 1000) / 100;
   int nEndType   = (nArrowConfig % 100) / 10;
   int nEndSize   = (nArrowConfig % 10);
   //printf("- arrow style: %d - arrow config: %d --> %d %d %d %d\n", nArrowStyle, nArrowConfig, nBeginType, nBeginSize, nEndType, nEndSize);


   // Normalized direction vector
   double dX = (double)dx;
   double dY = (double)dy;
   double dShortenF = (double)1.0f / sqrt(dX * dX + dY * dY);
   double dXn = dShortenF * dX;
   double dYn = dShortenF * dY;
   

   // Before drawing line and arrow heads:
   // Check for consistency between nArrowStyle and nArrowConfig
   CheckForArrowHeadConsistency(nArrowStyle, nBeginType, nBeginSize, nEndType, nEndSize);

   // Before drawing the line, do some linewidth corrections, if necessary
   // Define 2 shorten multipliers:
   double dShortenMultiplierEnd   = (double)1.5f;
   double dShortenMultiplierBegin = (double)1.5f;

   if (1 == (nEndSize % 3)) // 1, 4, 7
      dShortenMultiplierEnd = (double)1.0f;
   else if (0 == (nEndSize % 3)) // 3, 6, 9
      dShortenMultiplierEnd = (double)2.6f;
   if (1 == (nBeginSize % 3)) // 1, 4, 7
      dShortenMultiplierBegin = (double)1.0f;
   else if (0 == (nBeginSize % 3)) // 3, 6, 9
      dShortenMultiplierBegin = (double)2.6f;

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
   DrawLine(pShape, xCorr, yCorr, dxCorr, dyCorr, fLineWidth, bDashedLine, bDottedDash);
        

   // Now the arrow heads:
   // Check arrow style again
   switch (nArrowStyle)
   {
      case 0:  // simple line
         break;
      case 1:  // arrow at end of line
         DrawArrowHead(pShape, nEndType, nEndSize, x+dx, y+dy, -dXn, -dYn, fLineWidth, fcref);
         break;

      case 2:  // arrow at begin of line
         DrawArrowHead(pShape, nBeginType, nBeginSize, x, y, dXn, dYn, fLineWidth, fcref);
         break;

      case 3:  // arrow at begin and end of line
         DrawArrowHead(pShape, nEndType, nEndSize, x+dx, y+dy, -dXn, -dYn, fLineWidth, fcref);
         DrawArrowHead(pShape, nBeginType, nBeginSize, x, y, dXn, dYn, fLineWidth, fcref);
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

void CFlashEngine::DrawTelepointer(SWFShape *pShape, float x, float y, float size)
{
   x *= m_fScale; y *= m_fScale;
   size *= m_fScale;

   x -= size;

   // Define 4 points for the Telepointer
   float p0_x = x + size - 1.0f;
   float p0_y = y + 0.0f;
   float p1_x = x + 0.0f;
   float p1_y = y + (int)( 0.5f * size ) - 1.0f;
   float p2_x = x + (int)( 0.35f * size );
   float p2_y = y + (int)( 0.65f * size );
   float p3_x = x + (int)( 0.5f * size );
   float p3_y = y + size - 1.0f;

   // Draw the light red triangle
   pShape->setLine(m_fScale, 0x00, 0x00, 0x00);
   SWFFill *fill_1 = pShape->addSolidFill(0xFF, 0x00, 0x00);
   pShape->setLeftFill(fill_1);
   pShape->movePenTo(p0_x, p0_y);
   pShape->drawLineTo(p1_x, p1_y);
   pShape->drawLineTo(p2_x, p2_y);
   pShape->drawLineTo(p0_x, p0_y);


   // Draw the dark red triangle
   pShape->setLine(m_fScale, 0x00, 0x00, 0x00);
   SWFFill *fill_2 = pShape->addSolidFill(0x80, 0x00, 0x00);
   pShape->setLeftFill(fill_2);
   pShape->movePenTo(p0_x, p0_y);
   pShape->drawLineTo(p2_x, p2_y);
   pShape->drawLineTo(p3_x, p3_y);
   pShape->drawLineTo(p0_x, p0_y);
}

void CFlashEngine::DrawText(SWFText *pSwfText, float x, float y, 
                            SWFFont *pSwfFont, CString csText, 
                            float fFontSize, Gdiplus::ARGB crFontColor)
{
   x *= m_fScale; y *= m_fScale;
   fFontSize *= min(m_fScale, m_fScale);

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

void CFlashEngine::DrawImage(SWFShape *pShape, SWFBitmap *pBitmap)
{
   SWFFill *fill_img = pShape->addBitmapFill(pBitmap, SWFFILL_CLIPPED_BITMAP);

   pShape->setRightFill(fill_img);

///   DrawRectangle(pShape, 0.0f, 0.0f, pBitmap->getWidth(), pBitmap->getHeight(), 0.0f, false, false);
///   DrawRectangle(pShape, 0.0f, 0.0f, (float)(pBitmap->getWidth())/m_fScale, (float)(pBitmap->getHeight())/m_fScale, 0.0f, false, false);

   float imgX = 0.0f;
   float imgY = 0.0f;
   float imgWidth = (float)(pBitmap->getWidth()); ///m_fScale;
   float imgHeight = (float)(pBitmap->getHeight()); ///m_fScale;

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
      //printf("Warning: Arrow type and arrow configuration are not consistent! Default values are used.\n");
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

int CFlashEngine::GetFrameFromTimestamp(int nTimestamp)
{
   if (nTimestamp < 0)
      return -1;

   float fFrame = (m_fFrameRate * (float)nTimestamp) / 1000.0f;
   int nFrame = (int)fFrame;
   if (nTimestamp % 1000 != 0)
      nFrame += 1;

   // Note: first frame is frame 1 (not 0) --> add one frame here
   nFrame += 1;

#ifdef _DEBUG
   printf("GetFrameFromTimestamp(%d) --> %d\n", nTimestamp, nFrame);
#endif

   return nFrame;
}
