/*
 * @(#)GDIRenderer.cc	1.13 01/03/15
 *
 * Copyright (c) 1996-2001 Sun Microsystems, Inc. All Rights Reserved.
 *
 * Sun grants you ("Licensee") a non-exclusive, royalty free, license to use,
 * modify and redistribute this software in source and binary code form,
 * provided that i) this copyright notice and license appear on all copies of
 * the software; and ii) Licensee does not utilize the software in a manner
 * which is disparaging to Sun.
 *
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. SUN AND ITS LICENSORS SHALL NOT BE
 * LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THE SOFTWARE OR ITS DERIVATIVES. IN NO EVENT WILL SUN OR ITS
 * LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR DIRECT,
 * INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER
 * CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING OUT OF THE USE OF
 * OR INABILITY TO USE SOFTWARE, EVEN IF SUN HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 *
 * This software is not designed or intended for use in on-line control of
 * aircraft, air traffic, aircraft navigation or aircraft communications; or in
 * the design, construction, operation or maintenance of any nuclear
 * facility. Licensee represents and warrants that it will not use or
 * redistribute the Software for such purposes.
 */

#include <malloc.h>
#include <windows.h>
#include <vfw.h>
#include "jni-util.h"
#include "com_sun_media_renderer_video_GDIRenderer.h"

FILE *debugFile = NULL;

typedef struct {
   BOOL                    bActive;		      // is application active?
   HWND                    hwnd;		      // Handle to the window
   int                     inWidth;		      // Input width
   int                     inHeight;		      // Input height
   int                     outWidth;		      // Output width
   int                     outHeight;		      // Output height
   int                     inStride;		      // Stride for input lines
   int                     bitsPerPixel;	      // For display adapter
   int                     bpp;		      // bytes per pixel
   unsigned int            rMask;
   unsigned int            gMask;
   unsigned int            bMask;
   jobject                 component;
   BITMAPINFO             *bmi;
   HDC                     hdc;
   void *                  bits;
   HDRAWDIB                drawdib;
   int                     lastDstWidth;
   int                     lastDstHeight;
   int                     lastBytesPerPixel;
    
} GDIBlitter;

/*************************************************************************
 * Implementation for GDI Blitter - on win32
 *************************************************************************/

JNIEXPORT jboolean JNICALL
Java_com_sun_media_renderer_video_GDIRenderer_gdiInitialize(JNIEnv *env,
                                                            jobject gdiRenderer)
{

#ifdef _DEBUG
   if (debugFile == NULL)
   {
      debugFile = fopen("c:\\gdilog.txt", "w");
   }
#endif

   int rmask = 0x00FF0000;
   int gmask = 0x0000FF00;
   int bmask = 0x000000FF;
   int bitsPerPixel, rasterCaps;

   HWND hRootWindow   = ::GetDesktopWindow();
   HDC  hdcRootWindow = ::GetDC(hRootWindow);
   if (hdcRootWindow == NULL)
   {
      if (debugFile != NULL)
      {
         fprintf(debugFile, "DC in Init == 0\n");
      }
   }
   bitsPerPixel = ::GetDeviceCaps(hdcRootWindow, BITSPIXEL);
   rasterCaps   = ::GetDeviceCaps(hdcRootWindow, RASTERCAPS);    
   ::ReleaseDC(hRootWindow, hdcRootWindow);

   if (debugFile != NULL) 
   {
      fprintf(debugFile, "Init; bitsPerPixel: %d, rasterCaps: %d\n",bitsPerPixel, rasterCaps);
   }
   if (bitsPerPixel == 15) {
      rmask = 0x00007C00;
      gmask = 0x000003E0;
      bmask = 0x0000001F;
   } else if (bitsPerPixel == 16) {
      rmask = 0x0000F800;
      gmask = 0x000007E0;
      bmask = 0x0000001F;
   } else if (bitsPerPixel == 24) {
      rmask = 0x00FF0000;
      gmask = 0x0000FF00;
      bmask = 0x000000FF;
   }
    
   // Set the values in the FastBlt class
   SetIntField(env, gdiRenderer, "defrMask", rmask);
   SetIntField(env, gdiRenderer, "defgMask", gmask);
   SetIntField(env, gdiRenderer, "defbMask", bmask);
   SetIntField(env, gdiRenderer, "defbitsPerPixel", bitsPerPixel);

   GDIBlitter * blitter = new GDIBlitter;
   if (blitter)
   {
      blitter->hwnd = (HWND) 0;
      blitter->component = NULL;
      blitter->hdc = NULL;
      blitter->bmi = NULL;
      blitter->drawdib = NULL;
   }
    
   SetIntField(env, gdiRenderer, "blitter", (int) blitter);
    
   return 1;
}

JNIEXPORT jboolean JNICALL
Java_com_sun_media_renderer_video_GDIRenderer_gdiSetComponent(JNIEnv *env,
                                                              jobject gdiRenderer,
                                                              jint handle)
{
   
   
   if (debugFile != NULL)
   {
      fprintf(debugFile, "SetComponent\n");
   }

   GDIBlitter * blitter = (GDIBlitter*) GetIntField(env, gdiRenderer, "blitter");
   if (blitter == NULL)
      return 0;
   blitter->hwnd = (HWND) handle;
   return 1; // So far so good.
}

JNIEXPORT jboolean JNICALL
Java_com_sun_media_renderer_video_GDIRenderer_gdiSetOutputSize(JNIEnv *env,
                                                               jobject gdiRenderer,
                                                               jint width,
                                                               jint height)
{
   if (debugFile != NULL)
   {
      fprintf(debugFile, "SetOutputSize\n");
   }
   GDIBlitter * blitter = (GDIBlitter*) GetIntField(env, gdiRenderer, "blitter");
   if (blitter == NULL)
      return 0;
   if (width < 1)
      width = 1;
   if (height < 1)
      height = 1;
   blitter->outWidth = width;
   blitter->outHeight = height;
   return 1;
}

JNIEXPORT jboolean JNICALL
Java_com_sun_media_renderer_video_GDIRenderer_gdiDraw(JNIEnv *env,
                                                      jobject gdiRenderer,
                                                      jobject buffer,
                                                      jlong dataBytes,
                                                      jint elSize,
                                                      jint bytesPerPixel,
                                                      jint srcWidth,
                                                      jint srcHeight,
                                                      jint srcStride,
                                                      jint dstWidth,
                                                      jint dstHeight,
                                                      jint rMask,
                                                      jint gMask,
                                                      jint bMask,
                                                      jboolean flipped,
                                                      jint windowHandle)
{
   GDIBlitter * blitter = (GDIBlitter*) GetIntField(env, gdiRenderer, "blitter");

   if (blitter == NULL)
   {
      if (debugFile != NULL)
      {
         fprintf(debugFile, "blitter == NULL\n");
      }
      return 0;
   }
   
   if (debugFile != NULL)
   {
      fprintf(debugFile, "Draw: windowHandle=%07d", windowHandle);
   }
   if (windowHandle == 0)
   {
      if (debugFile != NULL)
      {
         fprintf(debugFile, "windowHandle == NULL\n");
      }
      return 0;
   }
   // Do the handle stuff
   blitter->hwnd = (HWND) windowHandle;


   blitter->hdc = GetDC(blitter->hwnd);
   if (blitter->hdc == NULL)
   {
      if (debugFile != NULL)
      {
         fprintf(debugFile, "blitter->hdc == NULL\n");
      }
      return 0;
   }

#ifdef _DEBUG
   RECT rect;
   if (GetWindowRect(blitter->hwnd, &rect) != 0 && debugFile)
   {
      fprintf(debugFile, "left=%d top=%d right=%d bottom=%d", rect.left, rect.top, rect.right, rect.bottom);
   }
#endif

   HDC hdc = blitter->hdc;
   void *bitmapData = (void*) dataBytes;

   if (dataBytes == 0) {
      if (elSize == 1)
         bitmapData = (void *) env->GetByteArrayElements((jbyteArray) buffer, 0);
      else if (elSize == 2)
         bitmapData = (void *) env->GetShortArrayElements((jshortArray) buffer, 0);
      else if (elSize == 3)
         bitmapData = (void *) env->GetByteArrayElements((jbyteArray) buffer, 0);
      else if (elSize == 4)
         bitmapData = (void *) env->GetIntArrayElements((jintArray) buffer, 0);
   }
    
   int size = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 3;
   if (blitter->bmi == NULL)
      blitter->bmi = (BITMAPINFO *) malloc(size);


   BITMAPINFO *bmi = blitter->bmi;
   BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER *) bmi;
   DWORD *masks = (DWORD*) (bmih+1);
   bmih->biSize = sizeof(BITMAPINFOHEADER);
   bmih->biWidth = srcStride;
   if (flipped)
      bmih->biHeight = srcHeight;
   else
      bmih->biHeight = -srcHeight;
   bmih->biPlanes = 1;
   bmih->biBitCount = bytesPerPixel * 8;
   bmih->biCompression = BI_RGB;
   if ((bytesPerPixel % 2) == 0)
      bmih->biCompression = BI_BITFIELDS;
   bmih->biSizeImage = srcStride * srcHeight * bytesPerPixel;
   bmih->biClrUsed = 0;
   bmih->biClrImportant = 0;
   bmih->biXPelsPerMeter = 10000;
   bmih->biYPelsPerMeter = 10000;
   masks[0] = rMask;
   masks[1] = gMask;
   masks[2] = bMask;

   
   if (srcWidth == dstWidth && srcHeight == dstHeight) 
   {
      if (debugFile)
      {
         fprintf(debugFile, ", srcWidth=%d, srcHeight=%d", srcWidth, srcHeight);
      }
      ::SetDIBitsToDevice(hdc, 0, 0, srcWidth, srcHeight, 0, 0, 
         0, srcHeight, bitmapData, bmi, DIB_RGB_COLORS);
   } 
   else 
   {
      if (debugFile)
      {
         fprintf(debugFile, ", srcWidth=%d srcHeight=%d destWidth=%d destHeight=%d", srcWidth, srcHeight, dstWidth, dstHeight);
      }
      ::SetStretchBltMode(hdc, STRETCH_HALFTONE);
      // unit is pixel:
      ::SetMapMode(hdc, MM_TEXT); 
      ::StretchDIBits(hdc, 0, 0, dstWidth, dstHeight,
         0, 0, srcWidth, srcHeight,
         bitmapData, bmi, DIB_RGB_COLORS, SRCCOPY);
   }

   if (dataBytes == 0) 
   {
      if (elSize == 1)
         env->ReleaseByteArrayElements((jbyteArray) buffer,
                                       (signed char *) bitmapData, 0);
      else if (elSize == 2)
         env->ReleaseShortArrayElements((jshortArray) buffer,
                                        (short *) bitmapData, 0);
      else if (elSize == 3)
         env->ReleaseByteArrayElements((jbyteArray) buffer,
                                       (signed char *) bitmapData, 0);
      else if (elSize == 4)
         env->ReleaseIntArrayElements((jintArray) buffer,
                                      (long *) bitmapData, 0);
   }

   ReleaseDC(blitter->hwnd, blitter->hdc);
   blitter->hdc = NULL;

   if (debugFile)
   {
      fprintf(debugFile, " END\n");
   }
   return 1;
}


JNIEXPORT jboolean JNICALL
Java_com_sun_media_renderer_video_GDIRenderer_gdiFree(JNIEnv *env,
                                                      jobject gdiRenderer)
{

   GDIBlitter * blitter = (GDIBlitter*) GetIntField(env, gdiRenderer, "blitter");
   if (blitter == NULL)
      return 0;
   if (blitter->bmi != NULL)
      free(blitter->bmi);
   blitter->bmi = NULL;

   if (blitter->drawdib != NULL) {
      DrawDibClose(blitter->drawdib);
      blitter->drawdib = NULL;
   }

   if (blitter->hdc != NULL)
      ReleaseDC(blitter->hwnd, blitter->hdc);
   blitter->hdc = NULL;
    
   SetIntField(env, gdiRenderer, "blitter", 0);
    

   if (debugFile != NULL)
   {
      fprintf(debugFile, "Free\n");
      fclose(debugFile);
      debugFile = NULL;
   }
   return 1;
}
