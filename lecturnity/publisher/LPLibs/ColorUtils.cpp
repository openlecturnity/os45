/* FILE ColorUtils.cpp */

#include "stdafx.h"

#include "ColorUtils.h"

// From MSDN Article ID: Q29240 
#define  HLSMAX   360 /* H,L, and S vary over 0-HLSMAX */
#define  RGBMAX   255   /* R,G, and B vary over 0-RGBMAX */
                        /* HLSMAX BEST IF DIVISIBLE BY 6 */
                        /* RGBMAX, HLSMAX must each fit in a byte. */
#define UNDEFINED (HLSMAX*2/3)


/*****************************************************************************/
// From MSDN Article ID: Q29240 

float* RGBToHLS(float* rgb, float range)
{
   WORD R,G,B;          /* input RGB values */
   WORD H,L,S;          /* input RGB values */
   BYTE cMax,cMin;      /* max and min RGB values */
   WORD  Rdelta,Gdelta,Bdelta; /* intermediate value: % of spread from max*/

   /* get R, G, and B out of DWORD */
   R = (WORD)rgb[0];
   G = (WORD)rgb[1];
   B = (WORD)rgb[2];

   /* calculate lightness */
   cMax = max( max(R,G), B);
   cMin = min( min(R,G), B);
   L = ( ((cMax+cMin)*HLSMAX) + RGBMAX )/(2*RGBMAX);

   if (cMax == cMin) 
   {                             /* r=g=b --> achromatic case */
      S = 0;                     /* saturation */
      H = UNDEFINED;             /* hue */
   }
   else 
   {                             /* chromatic case */
      /* saturation */
      if (L <= (HLSMAX/2))
         S = ( ((cMax-cMin)*HLSMAX) + ((cMax+cMin)/2) ) / (cMax+cMin);
      else
         S = ( ((cMax-cMin)*HLSMAX) + ((2*RGBMAX-cMax-cMin)/2) )
            / (2*RGBMAX-cMax-cMin);

      /* hue */
      Rdelta = ( ((cMax-R)*(HLSMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin);
      Gdelta = ( ((cMax-G)*(HLSMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin);
      Bdelta = ( ((cMax-B)*(HLSMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin);

      if (R == cMax)
         H = Bdelta - Gdelta;
      else if (G == cMax)
         H = (HLSMAX/3) + Rdelta - Bdelta;
      else /* B == cMax */
         H = ((2*HLSMAX)/3) + Gdelta - Rdelta;

      if (H < 0)
         H += HLSMAX;
      if (H > HLSMAX)
         H -= HLSMAX;
   }

   float* hlsOut = new float[3];
   hlsOut[0] = (float)H;
   hlsOut[1] = (float)L;
   hlsOut[2] = (float)S;

   return hlsOut;
}

/*****************************************************************************/
// From MSDN Article ID: Q29240 

float* HLSToRGB(float* hls, float range)
{
   WORD hue = (WORD)hls[0];
   WORD lum = (WORD)hls[1];
   WORD sat = (WORD)hls[2];

   WORD R,G,B;                /* RGB component values */
   WORD  Magic1,Magic2;       /* calculated magic numbers (really!) */

   if (sat == 0) 
   {                          /* achromatic case */
      R=G=B=(lum*RGBMAX)/HLSMAX;
      if (hue != UNDEFINED) 
      {
         /* ERROR */
      }
   }
   else  {                    /* chromatic case */
      /* set up magic numbers */
      if (lum <= (HLSMAX/2))
         Magic2 = (lum*(HLSMAX + sat) + (HLSMAX/2))/HLSMAX;
      else
         Magic2 = lum + sat - ((lum*sat) + (HLSMAX/2))/HLSMAX;
      Magic1 = 2*lum-Magic2;

      /* get RGB, change units from HLSMAX to RGBMAX */
      R = (HueToRGB(Magic1,Magic2,hue+(HLSMAX/3))*RGBMAX + (HLSMAX/2))/HLSMAX; 
      G = (HueToRGB(Magic1,Magic2,hue)*RGBMAX + (HLSMAX/2)) / HLSMAX;
      B = (HueToRGB(Magic1,Magic2,hue-(HLSMAX/3))*RGBMAX + (HLSMAX/2))/HLSMAX; 
   }

   float* rgbOut = new float[3];
   rgbOut[0] = (float)R;
   rgbOut[1] = (float)G;
   rgbOut[2] = (float)B;

   return rgbOut;
}

/*****************************************************************************/
// From MSDN Article ID: Q29240 

WORD HueToRGB(WORD n1, WORD n2, WORD hue)
{
   /* range check: note values passed add/subtract thirds of range */
   if (hue < 0)
      hue += HLSMAX;

   if (hue > HLSMAX)
      hue -= HLSMAX;

   /* return r,g, or b value from this tridrant */
   if (hue < (HLSMAX/6))
      return ( n1 + (((n2-n1)*hue+(HLSMAX/12))/(HLSMAX/6)) );
   if (hue < (HLSMAX/2))
      return ( n2 );
   if (hue < ((HLSMAX*2)/3))
      return ( n1 + (((n2-n1)*(((HLSMAX*2)/3)-hue)+(HLSMAX/12))/(HLSMAX/6))); 
   else
      return ( n1 );
}

/*****************************************************************************/
/*****************************************************************************/

UINT RgbToHls(MYRGB rgbIn, MYHLS &hlsOut)
{
   float rgbTmpIn[3];

   rgbTmpIn[0] = (float)rgbIn.r;
   rgbTmpIn[1] = (float)rgbIn.g;
   rgbTmpIn[2] = (float)rgbIn.b;

   float *hlsTmp = RGBToHLS(rgbTmpIn, 255.0f);

   // Debug info
   //_tprintf(_T("RgbToHls: %g, %g, %g  -->  %g, %g, %g\n"), rgbTmpIn[0], rgbTmpIn[1], rgbTmpIn[2], hlsTmp[0], hlsTmp[1], hlsTmp[2]);
   //_tprintf(_T("\n"));

   hlsOut.h = (int)(hlsTmp[0]);
   hlsOut.l = (int)(hlsTmp[1]);
   hlsOut.s = (int)(hlsTmp[2]);

   delete[] hlsTmp;
   hlsTmp = NULL;

   return 0;
}

/*****************************************************************************/

UINT HlsToRgb(MYHLS hlsIn, MYRGB & rgbOut)
{
   float hlsTmpIn[3];

   hlsTmpIn[0] = (float)hlsIn.h;
   hlsTmpIn[1] = (float)hlsIn.l;
   hlsTmpIn[2] = (float)hlsIn.s;

   float *rgbTmp = HLSToRGB(hlsTmpIn, 255.0f);

   // Debug info
   //_tprintf(_T("HlsToRgb: %g, %g, %g  -->  %g, %g, %g\n"), hlsTmpIn[0], hlsTmpIn[1], hlsTmpIn[2], rgbTmp[0], rgbTmp[1], rgbTmp[2]);
   //_tprintf(_T("\n"));

   rgbOut.r = (int)(rgbTmp[0]);
   rgbOut.g = (int)(rgbTmp[1]);
   rgbOut.b = (int)(rgbTmp[2]);

   delete[] rgbTmp;
   rgbTmp = NULL;

   return 0;
}

/*****************************************************************************/
/*****************************************************************************/

// Change Color Lightness
// Color values: int [0x00..0xFF] 

UINT ChangeColorLightness(MYRGB rgbIn, MYRGB &rgbOut, float lightnessFactor)
{
   float rgbTmpIn[3];

   rgbTmpIn[0] = (float)rgbIn.r;
   rgbTmpIn[1] = (float)rgbIn.g;
   rgbTmpIn[2] = (float)rgbIn.b;


   float *hlsTmp = RGBToHLS(rgbTmpIn, 255.0f);

   // Debug info
   //_tprintf(_T("RgbToHls: %g, %g, %g  -->  %g, %g, %g\n"), rgbTmpIn[0], rgbTmpIn[1], rgbTmpIn[2], hlsTmp[0], hlsTmp[1], hlsTmp[2]);
   //_tprintf(_T("lightnessFactor: %g\n"), lightnessFactor);

   
   // Change color lightness
   hlsTmp[1] = hlsTmp[1] * lightnessFactor;

   if (hlsTmp[1] < 0.0f)
      hlsTmp[1] = 0.0f;
   if (hlsTmp[1] > (float)HLSMAX)
      hlsTmp[1] = (float)HLSMAX;


   float *rgbTmpOut = HLSToRGB(hlsTmp, 255.0f);

   // Debug info
   //_tprintf(_T("HlsToRgb: %g, %g, %g  -->  %g, %g, %g\n"), hlsTmp[0], hlsTmp[1], hlsTmp[2], rgbTmpOut[0], rgbTmpOut[1], rgbTmpOut[2]);
   //_tprintf(_T("\n"));

   rgbOut.r = (int)(rgbTmpOut[0]);
   rgbOut.g = (int)(rgbTmpOut[1]);
   rgbOut.b = (int)(rgbTmpOut[2]);

   delete[] hlsTmp;
   delete[] rgbTmpOut;
   hlsTmp = NULL;
   rgbTmpOut = NULL;

   return 0;
}

/*****************************************************************************/

