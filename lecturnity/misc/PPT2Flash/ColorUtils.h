/* FILE ColorUtils.h */

#ifndef __COLOR_UTILS_H__
#define __COLOR_UTILS_H__

typedef struct
{
   int r;
   int g;
   int b;
} MYRGB;

typedef struct
{
   int h;
   int l;
   int s;
} MYHLS;


// From MSDN Article ID: Q29240 
float* RGBToHLS(float* rgb, float range = 255.0f);
float* HLSToRGB(float* hls, float range = 255.0f);
WORD HueToRGB(WORD n1, WORD n2, WORD hue);

// Convert RGB <--> HLS
UINT RgbToHls(MYRGB rgbIn, MYHLS &hlsOut);
UINT HlsToRgb(MYHLS hlsIn, MYRGB &rgbOut);

// Change the lightness of a color
//int* changeColorLightness(int* rgb, float lightnessFactor);
UINT ChangeColorLightness(MYRGB rgbIn, MYRGB &rgbOut, float lightnessFactor);

#endif /* !__COLOR_UTILS_H__ */
