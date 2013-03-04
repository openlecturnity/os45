// ppt2lecturnity.cpp : Definiert den Einsprungpunkt für die DLL-Anwendung.
//

#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0500		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0500		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif	


#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0500 // Change this to the appropriate value to target Windows Me or later.
#endif

#include <windows.h>

#include <ocidl.h>
#include <olectl.h>

#include <tchar.h>
#include <gdiplus.h>

#include <stdio.h>

#include "ppt2lecturnity.h"

#include "png.h"
#include "lutils.h"

ULONG_PTR gdiplusToken;

#pragma warning(disable: 4018 4244 4996)

#ifdef _DEBUG
#define _DEBUGFILE
#endif

int iWidth, iHeight;
const int iMAX_IMAGE_WIDTH = 400;
const int iMAX_IMAGE_HEIGHT = 400;

#ifdef _DEBUGFILE
void PPT2LECTURNITYDebugMsg(WCHAR *szFormat)
{
   static FILE *debugFile = NULL;
   
   if (!debugFile)
      debugFile = _wfopen(L"C:\\ppt2lecturnity.txt", L"w");
   fwprintf(debugFile, szFormat);
   fflush(debugFile);
}
#endif


BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved
                      )
{
   if (DLL_PROCESS_ATTACH == ul_reason_for_call)
   {
#ifdef _DEBUG
      WCHAR wszTmp[1024];
      _swprintf(wszTmp, L"DllMain DLL_PROCESS_ATTACH\n");
      PPT2LECTURNITYDebugMsg(wszTmp);
#endif
   }
   if (DLL_PROCESS_DETACH == ul_reason_for_call)
   {
#ifdef _DEBUG
      WCHAR wszTmp[1024];
      _swprintf(wszTmp, L"DllMain DLL_PROCESS_DETACH\n");
      PPT2LECTURNITYDebugMsg(wszTmp);
#endif
   }
   return TRUE;
}



/////////////////////////////////////////////////////////////////////////
///////   function to convert wmf-Files to png (only PPT 2000)   ////////
/////////////////////////////////////////////////////////////////////////

short __declspec(dllexport) CALLBACK  ConvertWMFtoPNG(BSTR bwmfName, BSTR bpngName, short eraseTransparenz)
{
   
   FILE 
      *wmfFile,
      *pngFile;
   HGLOBAL 
      hGlobal;
   DWORD 
      dwSize;
   IPicture 
      *image;
   int
      width, height;
   long 
      hmWidth, hmHeight;
   BYTE 
      *p1,
      *p2;
   int 
      x, 
      y;
   LPSTR
      wmfName,
      pngName;
   
   wmfName = (LPSTR)bwmfName;
   pngName = (LPSTR)bpngName;
   
   if (!(wmfFile = fopen(wmfName,"rb")))
      return false;
   
   fseek(wmfFile, 0, SEEK_END);
   dwSize = ftell(wmfFile);
   fseek(wmfFile, 0, SEEK_SET);
   
   hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, dwSize);
   if (!hGlobal)
   {
      fclose(wmfFile);
      return false;
   }
   
   char *pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
   if (!pData)
   {
      GlobalFree(hGlobal);
      fclose(wmfFile);
      return false;
   }
   
   if (fread(pData,1,dwSize,wmfFile) != dwSize)
   {
      GlobalFree(hGlobal);
      fclose(wmfFile);
      return false;
   }
   
   GlobalUnlock(hGlobal);
   fclose(wmfFile);
   
   IStream *pStream = NULL;
   
   if (CreateStreamOnHGlobal(hGlobal,TRUE,&pStream) != S_OK)
   {
      GlobalFree(hGlobal);
      return false;
   }
   
   if (OleLoadPicture(pStream,dwSize,false,IID_IPicture,
      reinterpret_cast<LPVOID *>(&image)) != S_OK)
   {
      GlobalFree(hGlobal);
      pStream->Release();
      return false;
   }
   
   pStream->Release();
   
   GlobalFree(hGlobal);
   
   image->get_Width(&hmWidth);
   image->get_Height(&hmHeight);
   
   width  = MulDiv(hmWidth, 96, 2540);
   height = MulDiv(hmHeight, 96, 2540);
   
   BITMAPINFO *bitmapInfo = (BITMAPINFO *)NULL;
   
   bitmapInfo = (BITMAPINFO *)LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT, sizeof(BITMAPINFO) ); 
   bitmapInfo->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
   bitmapInfo->bmiHeader.biWidth         = width;
   bitmapInfo->bmiHeader.biHeight        = height;
   bitmapInfo->bmiHeader.biPlanes        = 1;
   bitmapInfo->bmiHeader.biBitCount      = 32;
   bitmapInfo->bmiHeader.biCompression   = BI_RGB;
   bitmapInfo->bmiHeader.biSizeImage     = width * height * 4;
   
   PBYTE bitmapData1; 
   HDC myHdc1 =  CreateCompatibleDC(NULL);
   HBITMAP bitmap1 = CreateDIBSection(myHdc1, bitmapInfo, DIB_RGB_COLORS, (PVOID *) &bitmapData1, NULL, 0);
   SetStretchBltMode(myHdc1, HALFTONE);
   SelectObject(myHdc1, bitmap1); 
   memset(bitmapData1, 0x00ffffff, 4 * width * height);
   
   PBYTE bitmapData2; 
   HDC myHdc2 =  CreateCompatibleDC(NULL);
   SetStretchBltMode(myHdc2, HALFTONE);
   HBITMAP bitmap2 = CreateDIBSection(myHdc2, bitmapInfo, DIB_RGB_COLORS, (PVOID *) &bitmapData2, NULL, 0);
   SelectObject(myHdc2, bitmap2); 
   memset(bitmapData2, 0x00000000, 4 * width * height);
   
   image->Render(myHdc1, 0, 0, width, height, 0, hmHeight, hmWidth, -hmHeight, NULL);
   image->Render(myHdc2, 0, 0, width, height, 0, hmHeight, hmWidth, -hmHeight, NULL);
   
   image->Release();
   
   int offTop = 0;
   int offLeft = 0;
   int offBottom = 0;
   int offRight = 0;
   
   bool transparent;
   if (eraseTransparenz)
   {
      for (y = 0; y < height; ++y)
      {
         p1 = bitmapData1 + (width*4*y);
         p2 = bitmapData2 + (width*4*y);
         
         for (x = 0; x < width; ++x)
         {
            transparent = true;
            if ((*p1 != 255))
            {
               transparent = false;
               break;
            }
            else if (*p1 == *p2)
            {
               if (*(p1+1) == *(p2+1))
               {
                  if (*(p1+2) == *(p2+2))
                  {
                     transparent = false;
                     break;
                  }
               }
            }
            p1 += 4;
            p2 += 4;
         }
         if (!transparent)
            break;
      }
      offTop = y;
      
      for (x = 0; x < width; ++x)
      {
         p1 = bitmapData1 + (x*4);
         p2 = bitmapData2 + (x*4);
         
         for (y = 0; y < height; ++y)
         {
            transparent = true;
            if ((*p1 != 255))
            {
               transparent = false;
               break;
            }
            else if (*p1 == *p2)
            {
               if (*(p1+1) == *(p2+1))
               {
                  if (*(p1+2) == *(p2+2))
                  {
                     transparent = false;
                     break;
                  }
               }
            }
            p1 += (4*width);
            p2 += (4*width);
         }
         if (!transparent)
            break;
      }
      offLeft = x;
      
      for (y = height-1; y >= 0; --y)
      {
         p1 = bitmapData1 + y * (width * 4);
         p2 = bitmapData2 + y * (width * 4);
         
         for (x = 0; x < width; ++x)
         {
            transparent = true;
            if ((*p1 != 255))
            {
               transparent = false;
               break;
            }
            else if (*p1 == *p2)
            {
               if (*(p1+1) == *(p2+1))
               {
                  if (*(p1+2) == *(p2+2))
                  {
                     transparent = false;
                     break;
                  }
               }
            }
            p1 += 4;
            p2 += 4;
         }
         if (!transparent)
            break;
      }
      offBottom = height - y - 1;
      
      transparent = true;
      for (x = width-1; x >= 0; --x)
      {
         p1 = bitmapData1 + x*4;
         p2 = bitmapData2 + x*4;
         
         for (y = 0; y < height; ++y)
         {
            if ((*p1 != 255))
            {
               transparent = false;
               break;
            }
            else if (*p1 == *p2)
            {
               if (*(p1+1) == *(p2+1))
               {
                  if (*(p1+2) == *(p2+2))
                  {
                     transparent = false;
                     break;
                  }
               }
            }
            p1 += width*4;
            p2 += width*4;
         }
         if (!transparent)
            break;
      }
      offRight = width - x - 1;
   }
   
   bool isOpaque = true;
   int z;
   p1 = bitmapData1;
   p2 = bitmapData2;
   for (y=0; y<height && isOpaque; ++y)
   {
      for (x=0; x<width && isOpaque; ++x)
      {
         for (z=0; z<4; ++z)
         {
            if (*p1 != *p2)
            {
               isOpaque = false;
            }
            ++p1;
            ++p2;
         }
      }
   }
   
   png_structp 
      png_ptr;
   png_infop 
      info_ptr;
   
   pngFile = fopen(pngName, "wb");
   if (pngFile == NULL)
      return false;
   
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (void *)pngName, NULL, NULL);
   
   if (png_ptr == NULL)
   {
      fclose(pngFile);
      return false;
   }
   
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      fclose(pngFile);
      png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
      return false;
   }
   
   if (setjmp(png_jmpbuf(png_ptr)))
   {
      fclose(pngFile);
      png_destroy_write_struct(&png_ptr, &info_ptr);
      return false;
   }
   
   png_init_io(png_ptr, pngFile);
   
   int pngWidth = width - (offLeft + offRight);
   int pngHeight = height - (offTop + offBottom);
   
   int pixelDepth = isOpaque ? 3 : 4;
   
   png_set_IHDR(png_ptr, 
      info_ptr, 
      pngWidth, 
      pngHeight, 
      8, 
      isOpaque ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA,
      PNG_INTERLACE_NONE, 
      PNG_COMPRESSION_TYPE_BASE, 
      PNG_FILTER_TYPE_BASE);
   
   int row_stride = pixelDepth * pngWidth;
   
   info_ptr->pixel_depth = pixelDepth * 8;
   info_ptr->channels = pixelDepth;
   info_ptr->bit_depth = 8;
   info_ptr->color_type = isOpaque ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA;
   info_ptr->rowbytes = row_stride;
   
   png_write_info(png_ptr, info_ptr);
   
   BYTE *row_pointers = new BYTE[10+row_stride];
   BYTE *q;
   
   for (y = 0; y < pngHeight; ++y)
   {
      
      q = row_pointers;
      p1 = bitmapData1 + (((height - 1) - (y + offBottom)) * width + offLeft) * 4;
      p2 = bitmapData2 + (((height - 1) - (y + offBottom)) * width + offLeft) * 4;
      
      for (x = 0; x < pngWidth; ++x)
      {
         if (!isOpaque)
         {
            transparent = true;
            if (((*p1) != 255))
            {
               transparent = false;
            }
            else if (*p1 == *p2)
            {
               if (*(p1+1) == *(p2+1))
               {
                  if (*(p1+2) == *(p2+2))
                  {
                     transparent = false;
                  }
               }
            }
         }
         
         *q = *(p1+2); ++q;
         *q = *(p1+1); ++q;
         *q = *(p1); ++q;
         
         if (!isOpaque)
         {
            if (transparent)
               *q = 0;
            else
               *q = 255;
            ++q;
         }
         p1 += 4; // pixelDepth;
         p2 += 4; // pixelDepth;
      }
      png_write_row(png_ptr, row_pointers);
   }
   
   delete [] row_pointers;
   png_write_end(png_ptr, info_ptr);
   png_destroy_write_struct(&png_ptr, &info_ptr);
   
   fclose(pngFile);
   
   if (bitmapInfo) 
      LocalFree(bitmapInfo); 
   
   DeleteObject(bitmap1);
   DeleteDC(myHdc1);
   DeleteObject(bitmap2);
   DeleteDC(myHdc2); 
   
   
   return true;
}



/////////////////////////////////////////////////////////////////////////
//////////////////   class to grab image from clipboard   ////////////////////////
/////////////////////////////////////////////////////////////////////////

int CALLBACK EnhMetaFileProc(
                             HDC hDC,                      // handle to DC
                             HANDLETABLE *lpHTable,        // metafile handle table
                             CONST ENHMETARECORD *lpEMFR,  // metafile record
                             int nObj,                     // count of objects
                             LPARAM lpData                 // optional data
                             )
{
   if (!lpData)
      return 1;
   
#ifdef _DEBUG
   WCHAR wszTmp[1024];
   _swprintf(wszTmp, L"EnhMetaFileProc\n");
   PPT2LECTURNITYDebugMsg(wszTmp);
#endif

   ImageWriter *pImageWriter = (ImageWriter *)lpData;
   if ((lpEMFR->iType >= 2 && lpEMFR->iType <= 8) ||
      (lpEMFR->iType >= 41 && lpEMFR->iType <= 47) ||
      (lpEMFR->iType >= 54 && lpEMFR->iType <= 63) ||
      (lpEMFR->iType >= 83 && lpEMFR->iType <= 97))
   {
      pImageWriter->SetImageNotSupported();
      return 1;
   }
   

   if (lpEMFR->iType == EMR_STRETCHBLT)
   {
      
#ifdef _DEBUG
      _swprintf(wszTmp, L"EMR_STRETCHBLT\n");
      PPT2LECTURNITYDebugMsg(wszTmp);
#endif
      
      if (!pImageWriter->IsImageSupported())
         return 1;
      
      EMRSTRETCHBLT *pEsb = (EMRSTRETCHBLT *) lpEMFR;
      
      if (pEsb->dwRop == SRCPAINT || pEsb->dwRop == SRCAND || pEsb->dwRop == SRCCOPY)
      {
         BITMAPINFO *pBmi = (BITMAPINFO *) (((char *) pEsb) + pEsb->offBmiSrc);
         char *pData = ((char *) pEsb) + pEsb->offBitsSrc;
         
         if (!pImageWriter->IsValidBitmap(pEsb->cxDest, pEsb->cyDest, pBmi->bmiHeader.biWidth, pBmi->bmiHeader.biHeight))
         {
            pImageWriter->SetImageNotSupported();
            return 1;
         }
         pImageWriter->AddBitmap(pBmi, pData, pBmi->bmiHeader.biWidth, pBmi->bmiHeader.biHeight);
      }
   }
   else if (lpEMFR->iType == EMR_STRETCHDIBITS)
   {
      
#ifdef _DEBUG
      _swprintf(wszTmp, L"EMR_STRETCHDIBITS\n");
      PPT2LECTURNITYDebugMsg(wszTmp);
#endif
      
      if (!pImageWriter->IsImageSupported())
         return 1;
      
      // PowerPoint 2002 and 2000 seem to use StretchDIBits
      EMRSTRETCHDIBITS *pEsb = (EMRSTRETCHDIBITS *) lpEMFR;
      
      if (pEsb->dwRop == SRCPAINT || pEsb->dwRop == SRCAND || pEsb->dwRop == SRCCOPY)
      {
         BITMAPINFO *pBmi = (BITMAPINFO *) (((char *) pEsb) + pEsb->offBmiSrc);
         
         if (!pImageWriter->IsValidBitmap(pEsb->cxDest, pEsb->cyDest, pBmi->bmiHeader.biWidth, pBmi->bmiHeader.biHeight))
         {
            pImageWriter->SetImageNotSupported();
            return 1;
         }
         
         char *pData = ((char *) pEsb) + pEsb->offBitsSrc;
         pImageWriter->AddBitmap(pBmi, pData, pBmi->bmiHeader.biWidth, pBmi->bmiHeader.biHeight);
      }
   }
   
   return 1;
}

ImageWriter::ImageWriter(const WCHAR *wszImagePath, const WCHAR *wszImagePrefix, const WCHAR *wszImageSuffix)
{
   m_wszImageFilename = NULL;
   
#ifdef _DEBUG
   WCHAR wszTmp[1024];
   _swprintf(wszTmp, L"ImageWriter %s %s %s\n", wszImagePath, wszImagePrefix, wszImageSuffix);
   PPT2LECTURNITYDebugMsg(wszTmp);
#endif
   
   m_wszImagePath = new WCHAR[wcslen(wszImagePath) + 1];
   wcscpy(m_wszImagePath, wszImagePath);
   
   m_wszImagePrefix = new WCHAR[wcslen(wszImagePrefix) + 1];
   wcscpy(m_wszImagePrefix, wszImagePrefix);
   
   m_wszImageSuffix = new WCHAR[wcslen(wszImageSuffix) + 1];
   wcscpy(m_wszImageSuffix, wszImageSuffix);
   
   m_hEmf = NULL;
   m_iBitmapCount = 0;
   
   m_bSuccess = false;
   m_bSupportedImage = true;
   m_bEmptyImage = false;
}

ImageWriter::ImageWriter(const WCHAR *wszImagePath)
{
   m_wszImageFilename = NULL;
   
#ifdef _DEBUG
   WCHAR wszTmp[1024];
   _swprintf(wszTmp, L"ImageWriter %s\n", wszImagePath);
   PPT2LECTURNITYDebugMsg(wszTmp);
#endif
   
   m_wszImagePath = new WCHAR[wcslen(wszImagePath) + 1];
   wcscpy(m_wszImagePath, wszImagePath);
   
   m_wszImagePrefix = NULL;
   
   m_wszImageSuffix = NULL;
   
   m_hEmf = NULL;
   m_iBitmapCount = 0;
   
   m_bSuccess = false;
   m_bSupportedImage = true;
   m_bEmptyImage = false;
}

ImageWriter::~ImageWriter()
{
   if (m_wszImagePath)
      delete[] m_wszImagePath;
   
   if (m_wszImagePrefix)
      delete[] m_wszImagePrefix;
   
   if (m_wszImageFilename)
      delete[] m_wszImageFilename;
   
   if (m_wszImageSuffix)
      delete[] m_wszImageSuffix;
}

Gdiplus::Image *ImageWriter::CreateScaledImageFixedSize(Gdiplus::Metafile *emfImage)
{
   Gdiplus::REAL sourceWidth = (Gdiplus::REAL)emfImage->GetWidth();
   Gdiplus::REAL sourceHeight = (Gdiplus::REAL)emfImage->GetHeight();
   Gdiplus::REAL sourceX = 0;
   Gdiplus::REAL sourceY = 0;
   
   Gdiplus::REAL scaleX = (Gdiplus::REAL)(96.0 / emfImage->GetHorizontalResolution());
   Gdiplus::REAL scaleY = (Gdiplus::REAL)(96.0 / emfImage->GetVerticalResolution());
   Gdiplus::REAL destWidth = sourceWidth * scaleX;
   Gdiplus::REAL destHeight = sourceHeight * scaleY;
   Gdiplus::REAL destX = 0;
   Gdiplus::REAL destY = 0; 
   
   Gdiplus::Bitmap *scaledImage = new Gdiplus::Bitmap((int)destWidth, (int)destHeight);
   scaledImage->SetResolution(96.0, 96.0); //emfImage->GetHorizontalResolution(), emfImage->GetVerticalResolution());

   Gdiplus::Graphics grPhoto(scaledImage);
   
   Gdiplus::RectF boundRect;
   Gdiplus::Unit boundUnit;
   emfImage->GetBounds(&boundRect, &boundUnit);
   
   grPhoto.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
   grPhoto.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
   grPhoto.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
   
   Gdiplus::RectF destRect(0.0, 0.0, destWidth, destHeight);
   
   grPhoto.DrawImage(emfImage, destRect,
      boundRect.X, boundRect.Y, sourceWidth, sourceHeight,
      Gdiplus::UnitPixel);
   
   return scaledImage;
}


void ImageWriter::ConvertEnhancedMetafile()
{    
   ::GlobalLock(m_hEmf);
   
   HDC hDC = ::CreateCompatibleDC(NULL);
   RECT rect;
   rect.left = 0;
   rect.top = 0;
   rect.right = 100;
   rect.bottom = 100;
   
   ::EnumEnhMetaFile(hDC, m_hEmf, EnhMetaFileProc, this, &rect);
   
   if (m_bSupportedImage)
   {
      if (m_iBitmapCount == 1)         // no transparency
      {
         Write(m_gdipBitmap[0]);
      }
      else if (m_iBitmapCount == 2)    // m_gdipBitmap[0] is the mask
      {
         Gdiplus::Bitmap transparentBitmap(m_iWidth, m_iHeight, PixelFormat32bppARGB);
         
         Gdiplus::Color color, colorTemp;
         for(INT iRow = 0; iRow < m_iHeight; iRow++)
         {
            for(INT iColumn = 0; iColumn < m_iWidth; iColumn++)
            {
               m_gdipBitmap[0]->GetPixel(iColumn, iRow, &color);
               
               if (color.GetRed() == 0 && color.GetGreen() == 0 && color.GetBlue() == 0)
               {
                  colorTemp.SetValue(color.MakeARGB(0, 0, 0, 0)); 
               }
               else
               {
                  m_gdipBitmap[1]->GetPixel(iColumn, iRow, &colorTemp);
               }
               transparentBitmap.SetPixel(iColumn, iRow, colorTemp);
            }
         }
         Write(&transparentBitmap, false);
         
      }
   }
   else
   {  
      WCHAR *emfReadImageFilename = ComposeFilename(m_wszImageSuffix);
      WCHAR *pngSaveImageFilename = ComposeFilename(L".png");
      
      Gdiplus::Metafile *gdipMetafile = new Gdiplus::Metafile(emfReadImageFilename);

      if (gdipMetafile->GetLastStatus() == Gdiplus::Ok)
      {
         CLSID encoderClsid;
         INT result = GetEncoderClsid(L"image/png", &encoderClsid);
         
         Gdiplus::Image *gdipScaled = CreateScaledImageFixedSize(gdipMetafile);
         
         Gdiplus::Status status = gdipScaled->Save(pngSaveImageFilename,  &encoderClsid, NULL);
         
         if (status == Gdiplus::Ok)
            m_bSuccess = true;

         delete gdipScaled;
              
      }
      
      if (gdipMetafile)
      {
         delete gdipMetafile;
         gdipMetafile = NULL;
      }
      
      if (pngSaveImageFilename)
         delete[] pngSaveImageFilename;
      
      if (emfReadImageFilename)
         delete[] emfReadImageFilename;
   }


   for (int i = 0; i < m_iBitmapCount; ++i)
      delete m_gdipBitmap[i];
   
   ::DeleteDC(hDC);
   ::GlobalUnlock(m_hEmf);
   DeleteEnhMetaFile(m_hEmf);
}

#define MAKESHORT(a, b)  ((short)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((short)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))

BOOL CALLBACK metaCallback(Gdiplus::EmfPlusRecordType recordType, unsigned int flags, unsigned int dataSize, 
                           const unsigned char* pStr, void* callbackData)
{ 
   UsedCallbackData *pCallbackData = static_cast < UsedCallbackData* > (callbackData);

   if (recordType == Gdiplus::WmfRecordTypeSetWindowOrg)
   {
      pCallbackData->bIsWMF = true;
      pCallbackData->iY = (int)(((int)MAKESHORT(pStr[0], pStr[1])) * pCallbackData->fScaleY);
      pCallbackData->iX = (int)(((int)MAKESHORT(pStr[2], pStr[3])) * pCallbackData->fScaleX);
   }

   if (recordType == Gdiplus::EmfPlusRecordTypeFillRects) 
   {
      ++pCallbackData->iFillRectCount;
   }

   pCallbackData->gdipMetafile->PlayRecord(recordType, flags, dataSize, pStr);

   return TRUE; 
}


void ImageWriter::SaveEnhancedMetafile()
{    
   WCHAR *pngReadImageFilename = ComposeFilename(m_wszImageSuffix);
   WCHAR *pngSaveImageFilename = ComposeFilename(L".png");
   
   Gdiplus::Metafile *gdipMetafile = new Gdiplus::Metafile(pngReadImageFilename);
   
   if (gdipMetafile->GetLastStatus() == Gdiplus::Ok)
   {
      Gdiplus::MetafileHeader gdipMetafileHeader;
      Gdiplus::Rect rect;
      gdipMetafileHeader.GetBounds(&rect);
      gdipMetafile->GetMetafileHeader(&gdipMetafileHeader);
      gdipMetafileHeader.GetBounds(&rect);


      int iLogicalDpiX = gdipMetafileHeader.LogicalDpiX;
      int iLogicalDpiY = gdipMetafileHeader.LogicalDpiY;
      if (iLogicalDpiX == 0 && iLogicalDpiY == 0)
      {
         iLogicalDpiX = gdipMetafileHeader.GetDpiX();
         iLogicalDpiY = gdipMetafileHeader.GetDpiY();
      }
      Gdiplus::REAL fScaleLogicalX = ((Gdiplus::REAL)m_iHorizontalResolution) / iLogicalDpiX;
      Gdiplus::REAL fScaleLogicalY = ((Gdiplus::REAL)m_iVerticalResolution) / iLogicalDpiY;
      
      m_iDestX = (int)(rect.X * fScaleLogicalX);
      m_iDestY = (int)(rect.Y * fScaleLogicalY);
      m_iWidth = (int)(rect.Width * fScaleLogicalX);
      m_iHeight = (int)(rect.Height * fScaleLogicalY);

      Gdiplus::REAL fScaleX = ((Gdiplus::REAL)m_iHorizontalResolution) / gdipMetafile->GetHorizontalResolution();
      Gdiplus::REAL fScaleY = ((Gdiplus::REAL)m_iVerticalResolution) / gdipMetafile->GetVerticalResolution();
      
      Gdiplus::REAL fNewWidth =  gdipMetafile->GetWidth() * fScaleX * 3;
      Gdiplus::REAL fNewHeight =  gdipMetafile->GetHeight() * fScaleY * 3;
      Gdiplus::Bitmap *scaledImage = new Gdiplus::Bitmap((int)fNewWidth, (int)fNewHeight);
      scaledImage->SetResolution((Gdiplus::REAL)(m_iHorizontalResolution*3), (Gdiplus::REAL)(m_iVerticalResolution*3)); //emfImage->GetHorizontalResolution(), emfImage->GetVerticalResolution());
      
      Gdiplus::Graphics gdipGraphics(scaledImage);
     /*
      gdipGraphics.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
      gdipGraphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
      gdipGraphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
      gdipGraphics.SetSmoothingMode(Gdiplus::QualityModeDefault);
      gdipGraphics.SetTextRenderingHint(Gdiplus::TextRenderingHintSingleBitPerPixelGridFit);
      */

      UsedCallbackData callbackData;
      callbackData.gdipMetafile = gdipMetafile;
      callbackData.bIsWMF = false;
      callbackData.iX = 0;
      callbackData.iY = 0;
      callbackData.fScaleX = fScaleX;
      callbackData.fScaleY = fScaleY;
      callbackData.iFillRectCount = 0;

      gdipGraphics.EnumerateMetafile(gdipMetafile, Gdiplus::PointF(0.0f, 0.0f), metaCallback, &callbackData);
      
      // WMF images have no correct x and y position in metafile header
      if (callbackData.bIsWMF)
      {
         m_iDestX = callbackData.iX;
         m_iDestY = callbackData.iY;
      }
      
      CLSID encoderClsid;
      INT result = GetEncoderClsid(L"image/png", &encoderClsid);
      
      Gdiplus::Status status = scaledImage->Save(pngSaveImageFilename,  &encoderClsid, NULL);
      
      m_bSuccess = true;

      delete scaledImage;
   }
   
   if (gdipMetafile)
   {
      delete gdipMetafile;
      gdipMetafile = NULL;
   }
   
   if (pngReadImageFilename && pngSaveImageFilename && m_bSuccess)
   {
      DeleteFileW(pngReadImageFilename);
   }
   
   if (pngSaveImageFilename)
      delete[] pngSaveImageFilename;
   
   if (pngReadImageFilename)
      delete[] pngReadImageFilename;
   
   
}

void ImageWriter::SaveEnhancedMetafileWithoutGrid()
{    
   WCHAR *pngReadImageFilename = ComposeFilename(m_wszImageSuffix);
   WCHAR *pngSaveImageFilename = ComposeFilename(L".png");

   Gdiplus::Metafile *gdipMetafile = new Gdiplus::Metafile(pngReadImageFilename);
   
   HRESULT hr = S_OK;
   Gdiplus::REAL fNewWidth = 0;
   Gdiplus::REAL fNewHeight = 0;
   if (gdipMetafile->GetLastStatus() == Gdiplus::Ok)
   {
      Gdiplus::MetafileHeader gdipMetafileHeader;
      Gdiplus::Rect rect;
      gdipMetafileHeader.GetBounds(&rect);
      gdipMetafile->GetMetafileHeader(&gdipMetafileHeader);
      gdipMetafileHeader.GetBounds(&rect);
    
      Gdiplus::REAL fScaleX = ((Gdiplus::REAL)m_iHorizontalResolution) / gdipMetafile->GetHorizontalResolution();
      Gdiplus::REAL fScaleY = ((Gdiplus::REAL)m_iVerticalResolution) / gdipMetafile->GetVerticalResolution();
      
      fNewWidth =  gdipMetafile->GetWidth() * fScaleX * 3;
      fNewHeight =  gdipMetafile->GetHeight() * fScaleY * 3;
      
      int iLogicalDpiX = gdipMetafileHeader.LogicalDpiX;
      int iLogicalDpiY = gdipMetafileHeader.LogicalDpiY;
      if (iLogicalDpiX == 0 && iLogicalDpiY == 0)
      {
         iLogicalDpiX = gdipMetafileHeader.GetDpiX();
         iLogicalDpiY = gdipMetafileHeader.GetDpiY();
      }
      Gdiplus::REAL fScaleLogicalX = ((Gdiplus::REAL)m_iHorizontalResolution) / iLogicalDpiX;
      Gdiplus::REAL fScaleLogicalY = ((Gdiplus::REAL)m_iVerticalResolution) / iLogicalDpiY;
      
      m_iDestX = (int)(rect.X * fScaleLogicalX);
      m_iDestY = (int)(rect.Y * fScaleLogicalY);
      m_iWidth = (int)(rect.Width * fScaleLogicalX);
      m_iHeight = (int)(rect.Height * fScaleLogicalY);
   }
   else
      hr = E_FAIL;

   if (gdipMetafile)
   {
      delete gdipMetafile;
      gdipMetafile = NULL;
   }

   if (SUCCEEDED(hr))
   {
      Gdiplus::Bitmap *scaledImage = new Gdiplus::Bitmap((int)fNewWidth, (int)fNewHeight);
      scaledImage->SetResolution((Gdiplus::REAL)(m_iHorizontalResolution*3), (Gdiplus::REAL)(m_iVerticalResolution*3)); //emfImage->GetHorizontalResolution(), emfImage->GetVerticalResolution());
      
      Gdiplus::Graphics gdipGraphics(scaledImage);

      // Retrieve a handle to a window device context. 
      HDC hDC = gdipGraphics.GetHDC();
     
      HENHMETAFILE hemf = GetEnhMetaFileW(pngReadImageFilename); 

      // Retrieve the client rectangle dimensions. 
      RECT rct;
      rct.left = 0;
      rct.top = 0;
      rct.right = fNewWidth;
      rct.bottom = fNewHeight;

      // Draw the picture. 

      BOOL bResult = PlayEnhMetaFile(hDC, hemf, &rct); 

      // Release the metafile handle. 

      DeleteEnhMetaFile(hemf); 

      // Release the graphics DC. 
      gdipGraphics.ReleaseHDC(hDC);
      
      CLSID encoderClsid;
      INT result = GetEncoderClsid(L"image/png", &encoderClsid);
      
      Gdiplus::Status status = scaledImage->Save(pngSaveImageFilename,  &encoderClsid, NULL);
      
      m_bSuccess = true;

      delete scaledImage;
   }
   
   if (pngReadImageFilename && pngSaveImageFilename && m_bSuccess)
   {
      DeleteFileW(pngReadImageFilename);
   }
   
   if (pngSaveImageFilename)
      delete[] pngSaveImageFilename;
   
   if (pngReadImageFilename)
      delete[] pngReadImageFilename;
   
   
}


HRESULT ImageWriter::WriteMetafileAsPNG(bool bUseGDIFunctions)
{
   HRESULT hResult = S_OK;
   
#ifdef _DEBUG
   WCHAR wszTmp[1024];
   _swprintf(wszTmp, L"SaveMetafileAsPNG\n");
   PPT2LECTURNITYDebugMsg(wszTmp);
#endif
   
   WCHAR *emfImageFilename = ComposeFilename(m_wszImageSuffix);
   
   if (hResult == S_OK)
   {
      if (bUseGDIFunctions)
         SaveEnhancedMetafileWithoutGrid();
      else
         SaveEnhancedMetafile();
   }
   
   if (emfImageFilename)
   {
      //DeleteFile(emfImageFilename);
      delete[] emfImageFilename;
   }
   
   return hResult;
}

HRESULT ImageWriter::ExtractDimensionFromMetafile(bool bDeleteImage)
{
   HRESULT hResult = S_OK;

#ifdef _DEBUG
   WCHAR wszTmp[1024];
   _swprintf(wszTmp, L"SaveMetafileAsPNG\n");
   PPT2LECTURNITYDebugMsg(wszTmp);
#endif

   if (hResult == S_OK)
   {
      WCHAR *emfImageFilename = NULL;
      if (m_wszImageSuffix != NULL)
         emfImageFilename = ComposeFilename(m_wszImageSuffix);
      else
         emfImageFilename = m_wszImagePath;

      Gdiplus::Metafile *gdipMetafile = new Gdiplus::Metafile(emfImageFilename);

      if (gdipMetafile->GetLastStatus() == Gdiplus::Ok)
      {
         Gdiplus::MetafileHeader gdipMetafileHeader;
         Gdiplus::Rect rect;
         gdipMetafileHeader.GetBounds(&rect);
         gdipMetafile->GetMetafileHeader(&gdipMetafileHeader);
         gdipMetafileHeader.GetBounds(&rect);

         int iLogicalDpiX = gdipMetafileHeader.LogicalDpiX;
         int iLogicalDpiY = gdipMetafileHeader.LogicalDpiY;
         if (iLogicalDpiX == 0 && iLogicalDpiY == 0)
         {
            iLogicalDpiX = gdipMetafileHeader.GetDpiX();
            iLogicalDpiY = gdipMetafileHeader.GetDpiY();
         }
         Gdiplus::REAL fScaleLogicalX = ((Gdiplus::REAL)m_iHorizontalResolution) / iLogicalDpiX;
         Gdiplus::REAL fScaleLogicalY = ((Gdiplus::REAL)m_iVerticalResolution) / iLogicalDpiY;

         m_iDestX = (int)(rect.X * fScaleLogicalX);
         m_iDestY = (int)(rect.Y * fScaleLogicalY);
         m_iWidth = (int)(rect.Width * fScaleLogicalX);
         m_iHeight = (int)(rect.Height * fScaleLogicalY);

         m_bSuccess = true;
      }

      if (gdipMetafile)
      {
         delete gdipMetafile;
         gdipMetafile = NULL;
      }

      if (emfImageFilename && bDeleteImage)
      {
         DeleteFileW(emfImageFilename);
         delete[] emfImageFilename;
      }
   }

   return hResult;
}


HRESULT ImageWriter::FindEmfImageData()
{
   HRESULT hResult = S_OK;
   
#ifdef _DEBUG
   WCHAR wszTmp[1024];
   _swprintf(wszTmp, L"FindEmfImageData\n");
   PPT2LECTURNITYDebugMsg(wszTmp);
#endif
   
   
   WCHAR *emfImageFilename = ComposeFilename(m_wszImageSuffix);
   m_hEmf = (HENHMETAFILE)GetEnhMetaFileW(emfImageFilename);
   
   if (m_hEmf == NULL)
      hResult = E_FAIL;
   
   if (hResult == S_OK)
   {
      ConvertEnhancedMetafile();
   }

   if (emfImageFilename)
   {
      BOOL bResult = DeleteFileW(emfImageFilename);

      delete[] emfImageFilename;
   }
   
   return hResult;
}


HRESULT ImageWriter::CheckEnhancedMetafile()
{    
   
   HRESULT hResult = S_OK;

   Gdiplus::Metafile *gdipMetafile = new Gdiplus::Metafile(m_wszImagePath);
   
   if (gdipMetafile->GetLastStatus() == Gdiplus::Ok)
   {
      Gdiplus::MetafileHeader gdipMetafileHeader;
      Gdiplus::Rect rect;
      gdipMetafileHeader.GetBounds(&rect);
      gdipMetafile->GetMetafileHeader(&gdipMetafileHeader);
      gdipMetafileHeader.GetBounds(&rect);


      int iLogicalDpiX = gdipMetafileHeader.LogicalDpiX;
      int iLogicalDpiY = gdipMetafileHeader.LogicalDpiY;
      Gdiplus::REAL fScaleLogicalX = ((Gdiplus::REAL)m_iHorizontalResolution) / iLogicalDpiX;
      Gdiplus::REAL fScaleLogicalY = ((Gdiplus::REAL)m_iVerticalResolution) / iLogicalDpiY;
      
      m_iDestX = (int)(rect.X * fScaleLogicalX);
      m_iDestY = (int)(rect.Y * fScaleLogicalY);
      m_iWidth = (int)(rect.Width * fScaleLogicalX);
      m_iHeight = (int)(rect.Height * fScaleLogicalY);

      Gdiplus::REAL fScaleX = ((Gdiplus::REAL)m_iHorizontalResolution) / gdipMetafile->GetHorizontalResolution();
      Gdiplus::REAL fScaleY = ((Gdiplus::REAL)m_iVerticalResolution) / gdipMetafile->GetVerticalResolution();
      
      Gdiplus::REAL fNewWidth =  gdipMetafile->GetWidth() * fScaleX;
      Gdiplus::REAL fNewHeight =  gdipMetafile->GetHeight() * fScaleY;
      Gdiplus::Bitmap *scaledImage = new Gdiplus::Bitmap((int)fNewWidth, (int)fNewHeight);
      scaledImage->SetResolution((Gdiplus::REAL)m_iHorizontalResolution, (Gdiplus::REAL)m_iVerticalResolution);
      
      Gdiplus::Graphics gdipGraphics(scaledImage);
      Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));
      gdipGraphics.FillRectangle(&whiteBrush, 0, 0, (int)fNewWidth, (int)fNewHeight);
    

      UsedCallbackData callbackData;
      callbackData.gdipMetafile = gdipMetafile;
      callbackData.bIsWMF = false;
      callbackData.iX = 0;
      callbackData.iY = 0;
      callbackData.fScaleX = fScaleX;
      callbackData.fScaleY = fScaleY;
      callbackData.iFillRectCount = 0;

      gdipGraphics.EnumerateMetafile(gdipMetafile, Gdiplus::PointF(0.0f, 0.0f), metaCallback, &callbackData);


      if (scaledImage->GetWidth() <= iMAX_IMAGE_WIDTH && scaledImage->GetHeight() <= iMAX_IMAGE_HEIGHT)
      {
         if (callbackData.iFillRectCount == 2) 
         {
            m_bEmptyImage = true;
            for (UINT x = 0; x < scaledImage->GetWidth(); ++x)
            {
               for (UINT y = 0; y < scaledImage->GetHeight(); ++y)
               {
                  Gdiplus::Color gdipColor;
                  scaledImage->GetPixel(x, y, &gdipColor);
                  if ((gdipColor.GetRed() == 255 && 
                     gdipColor.GetGreen() == 255 && gdipColor.GetBlue() == 255) ||
                     (gdipColor.GetRed() == 254 && 
                     gdipColor.GetGreen() == 254 && gdipColor.GetBlue() == 254))
                  {
                  }
                  else
                  {
                     m_bEmptyImage = false;
                     break;
                  }
               }
               if (!m_bEmptyImage)
                  break;
            }
         }
      }
      
      if (!m_bEmptyImage)
      {
         m_bSuccess = true;
      }

      delete scaledImage;
      
   }
   
   if (gdipMetafile)
   {
      delete gdipMetafile;
      gdipMetafile = NULL;
   }
   
   return hResult;
}

HRESULT ImageWriter::FindWmfImageData()
{
#ifdef _DEBUG
   WCHAR wszTmp[1024];
   _swprintf(wszTmp, L"FindWmfImageData\n");
   PPT2LECTURNITYDebugMsg(wszTmp);
#endif
   HRESULT hResult = S_OK;
   
   WCHAR *wmfImageFilename = ComposeFilename(m_wszImageSuffix);
   
   FILE *wmfFile = _wfopen(wmfImageFilename, L"rb");
   if (!wmfFile)
      hResult = E_FAIL;
   
   // Create an IPicture to get HMETAFILE
   DWORD dwSize = 0;
   if (hResult == S_OK)
   {
      fseek(wmfFile, 0, SEEK_END);
      dwSize = ftell(wmfFile);
      fseek(wmfFile, 0, SEEK_SET);
   }
   
   HGLOBAL hGlobal = NULL;
   if (hResult == S_OK)
   {
      hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, dwSize);
      if (!hGlobal)
         hResult = E_FAIL;
   }
   
   char *pData = NULL;
   if (hResult == S_OK)
   {
      pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
      if (!pData)
         hResult = E_FAIL;
   }
   
   if (hResult == S_OK)
   {
      DWORD nRead = fread(pData, 1, dwSize, wmfFile);
      if (nRead != dwSize)
         hResult = E_FAIL;
   }
   
   if (hGlobal)
      GlobalUnlock(hGlobal);
   
   if (wmfFile)
      fclose(wmfFile);
   
   IStream *pStream = NULL;
   if (hResult == S_OK)
   {
      hResult = CreateStreamOnHGlobal(hGlobal,TRUE,&pStream);
   }
   
   IPicture *image = NULL;
   if (hResult == S_OK)
   {
      hResult = OleLoadPicture(pStream, dwSize, false, IID_IPicture, reinterpret_cast<LPVOID *>(&image));
   }
   
   if (pStream)
      pStream->Release();
   
   short pType;
   if (hResult == S_OK)
   {
      hResult = image->get_Type(&pType);
   }
   
   HMETAFILE hMetafile;
   if (hResult == S_OK && pType == PICTYPE_METAFILE)
   {
      hResult = image->get_Handle((unsigned int *)&hMetafile);
   }
   
   // Convert WMF image data to EMF image data
   UINT nCount = 0;
   if (hResult == S_OK)
   {
      nCount =  GetMetaFileBitsEx(hMetafile, 0, NULL);
      if (nCount <= 0)
         hResult = E_FAIL;
   }
   
   // Create Enhanced Metafile
   if (hResult == S_OK)
   {
      BYTE *pbBuffer = new BYTE[nCount];
      GetMetaFileBitsEx(hMetafile, nCount, pbBuffer);
      
      m_hEmf = SetWinMetaFileBits(nCount, pbBuffer, NULL, NULL);
      
      ConvertEnhancedMetafile();
      if (pbBuffer)
         delete[] pbBuffer;
   }
   
   if (image)
      image->Release();
   
   if (hGlobal)
      GlobalFree(hGlobal);
   
   if (wmfImageFilename)
   {
      DeleteFileW(wmfImageFilename);
      delete[] wmfImageFilename;
   }
   
   return hResult;
}

HRESULT ImageWriter::RewriteImage()
{
   HRESULT hr = ::CoInitialize(NULL);
   
   WCHAR *pngReadImageFilename = ComposeFilename(m_wszImageSuffix);
   
   WCHAR *pngSaveImageFilename = new WCHAR[wcslen(pngReadImageFilename) + 5];
   _swprintf(pngSaveImageFilename, L"%s_", pngReadImageFilename);
   
   Gdiplus::Image *gdipImage = new Gdiplus::Image(pngReadImageFilename, FALSE);
   if (gdipImage->GetLastStatus() == Gdiplus::Ok)
   {
      CLSID encoderClsid;
      INT result = GetEncoderClsid(L"image/png", &encoderClsid);
      
      Gdiplus::Status status = gdipImage->Save(pngSaveImageFilename,  &encoderClsid, NULL);
      
      m_bSuccess = true;
      
   }
   
   if (gdipImage)
   {
      delete gdipImage;
      gdipImage = NULL;
   }
   
   if (pngReadImageFilename && pngSaveImageFilename && m_bSuccess)
   {
      DeleteFileW(pngReadImageFilename);
      MoveFileW(pngSaveImageFilename, pngReadImageFilename);
   }
   
   if (pngSaveImageFilename)
      delete[] pngSaveImageFilename;
   
   if (pngReadImageFilename)
      delete[] pngReadImageFilename;
   
   ::CoUninitialize();
   
   return hr;
}

void ImageWriter::AddBitmap(BITMAPINFO *pBmi, char *pData, int iWidth, int iHeight)
{
   m_iWidth = iWidth;
   m_iHeight = iHeight;
   
   m_gdipBitmap[m_iBitmapCount] = Gdiplus::Bitmap::FromBITMAPINFO(pBmi, pData);
   m_iBitmapCount++;
   
}

bool ImageWriter::IsValidBitmap(int iDestX, int iDestY, int iWidth, int iHeight)
{
   
   if (m_iBitmapCount == 0)
   {
      m_iDestX = iDestX;
      m_iDestY = iDestY;
      return true;
   }
   
   if (m_iBitmapCount >= 2)
      return false;
   
   if (iDestX != m_iDestX || iDestY != m_iDestY ||
      iWidth != m_iWidth || iHeight != m_iHeight)
      return false;
   
   return true;
   
}

int ImageWriter::GetEncoderClsid(const WCHAR* wszFormat, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes
  
   
   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
   
   Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
   {
      return -1;  // Failure
   }
   
   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
   {
      return -1;  // Failure
   }
   
   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
   
   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, wszFormat) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }
   
   free(pImageCodecInfo);
   
   return -1;  // Failure
}

int ImageWriter::GetFileSize()
{
   WCHAR *wszFullImageName = new WCHAR[wcslen(m_wszImagePath) + wcslen(m_wszImageFilename) + 1];
   _swprintf(wszFullImageName, L"%s%s", m_wszImagePath, m_wszImageFilename);
   
   HANDLE hFile = CreateFileW(wszFullImageName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
   if (hFile == 0)
      return 0;
   
   int iFileSize = ::GetFileSize(hFile, NULL);
   
   CloseHandle(hFile);
   delete[] wszFullImageName;
   
   return iFileSize;
}

WCHAR *ImageWriter::ComposeFilename(WCHAR *wszFileSuffix)
{ 
   if (m_wszImageFilename)
      delete m_wszImageFilename;
   
   m_wszImageFilename = new WCHAR[wcslen(m_wszImagePrefix) + wcslen(wszFileSuffix) + 1];
   _swprintf(m_wszImageFilename, L"%s%s", m_wszImagePrefix, wszFileSuffix);
   
   WCHAR *wszFullImageName = new WCHAR[wcslen(m_wszImagePath) + wcslen(m_wszImageFilename) + 1];
   _swprintf(wszFullImageName, L"%s%s", m_wszImagePath, m_wszImageFilename);
   
   return wszFullImageName;
}

HRESULT ImageWriter::Write(Gdiplus::Bitmap *gdipBitmap, bool bConvertToJpeg)
{
   
#ifdef _DEBUG
   WCHAR wszTmp[1024];
   _swprintf(wszTmp, L"Write\n");
   PPT2LECTURNITYDebugMsg(wszTmp);
#endif
   
   HRESULT hr = ::CoInitialize(NULL);
   
   {
      WCHAR *wszImageFilename = ComposeFilename(L".png");
      
      CLSID  encoderClsid;
      INT result = GetEncoderClsid(L"image/png", &encoderClsid);
      
      if (-1 != result)
      {
         // Save image as png
         gdipBitmap->Save(wszImageFilename, &encoderClsid, NULL);
         m_bSuccess = true;
         
         if (bConvertToJpeg)
         {
            // check file size
            int iFileSize = GetFileSize();
            
            // if file size greater 1 MB, write JPG
            if (iFileSize > PNG_MAXSIZE)
            {
               DeleteFileW(wszImageFilename);
               
               m_bSuccess = false;
               if (wszImageFilename)
                  delete[] wszImageFilename;
               wszImageFilename = ComposeFilename(L".jpg");
               
               CLSID  encoderClsid;
               INT result = GetEncoderClsid(L"image/jpeg", &encoderClsid);
               
               if (-1 != result)
               {
                  gdipBitmap->Save(wszImageFilename, &encoderClsid, NULL);
                  m_bSuccess = true;
                  
               }
            }
         }
      }
      
      if (wszImageFilename)
         delete[] wszImageFilename;
   }
   
   ::CoUninitialize();
   
   return hr;
}

/////////////////////////////////////////////////////////////////////////
//////////////   Get Font information from metafile   ///////////////////
/////////////////////////////////////////////////////////////////////////

int CALLBACK EnhMetaFileProcForCharCount(HDC hDC,                      // handle to DC
                                         HANDLETABLE *lpHTable,        // metafile handle table
                                         CONST ENHMETARECORD *lpEMFR,  // metafile record
                                         int nObj,                     // count of objects
                                         LPARAM lpData                 // optional data
                                         )
{
   if (!lpData)
      return 1;
   
#ifdef _DEBUG
   WCHAR wszTmp[1024];
   _swprintf(wszTmp, L"EnhMetaFileProc\n");
   PPT2LECTURNITYDebugMsg(wszTmp);
#endif
   

   FontExtractor *pFontExtractor = (FontExtractor *)lpData;
   if (lpEMFR->iType == EMR_EXTTEXTOUTW)
   {
      EMREXTTEXTOUTW* emr=(PEMREXTTEXTOUTW)lpEMFR;

		if (emr != NULL && emr->emrtext.nChars >= 1)
		{
			wchar_t buf[16384];

			if (emr == NULL || emr->emrtext.nChars > 4095)
				fprintf(stderr, "Error: EMR_EXTTEXTOUTW: Too long\n");
			else
			{
				memcpy(buf, (char*)emr + emr->emrtext.offString, emr->emrtext.nChars * sizeof(WCHAR));
				buf[emr->emrtext.nChars]='\0';
				buf[emr->emrtext.nChars+1]='\0';
			}
         pFontExtractor->IncreaseCharCount(emr->emrtext.nChars);
		}

      
   }

   return 1;
}

int CALLBACK EnhMetaFileProcForFont(HDC hDC,                      // handle to DC
                                    HANDLETABLE *lpHTable,        // metafile handle table
                                    CONST ENHMETARECORD *lpEMFR,  // metafile record
                                    int nObj,                     // count of objects
                                    LPARAM lpData                 // optional data
                                    )
{
   if (!lpData)
      return 1;
   
#ifdef _DEBUG
   WCHAR wszTmp[1024];
   _swprintf(wszTmp, L"EnhMetaFileProc\n");
   PPT2LECTURNITYDebugMsg(wszTmp);
#endif
   

   FontExtractor *pFontExtractor = (FontExtractor *)lpData;
   if (lpEMFR->iType == EMR_EXTCREATEFONTINDIRECTW)
   {
      EMREXTCREATEFONTINDIRECTW *pEcfi = (EMREXTCREATEFONTINDIRECTW *) lpEMFR;
      pFontExtractor->SetFontFamily(pEcfi->elfw.elfLogFont.lfFaceName);
   }

   if (lpEMFR->iType == EMR_EXTTEXTOUTA)
   {
      EMREXTTEXTOUTA *pEcfi = (EMREXTTEXTOUTA *) lpEMFR;
   }

   if (lpEMFR->iType == EMR_EXTTEXTOUTW)
   {
      EMREXTTEXTOUTW* emr=(PEMREXTTEXTOUTW)lpEMFR;

		if (emr != NULL && emr->emrtext.nChars >= 1)
		{
			wchar_t buf[16384];

			if (emr == NULL || emr->emrtext.nChars > 4095)
				fprintf(stderr, "Error: EMR_EXTTEXTOUTW: Too long\n");
			else
			{
				memcpy(buf, (char*)emr + emr->emrtext.offString, emr->emrtext.nChars * sizeof(WCHAR));
				buf[emr->emrtext.nChars]='\0';
				buf[emr->emrtext.nChars+1]='\0';
			}
         pFontExtractor->IncreaseActualCharCount(emr->emrtext.nChars);
		}

      if (pFontExtractor->GetActualCharCount() == pFontExtractor->GetCharCount())
         pFontExtractor->m_bDoCopy = false;
      
   }

   return 1;
}

HRESULT FontExtractor::ExtractWmfFontInfo(LPCWSTR wszImageName)
{
   HRESULT hResult = S_OK;
   
   FILE *wmfFile = _wfopen(wszImageName, L"rb");
   if (!wmfFile)
      hResult = E_FAIL;
   
   // Create an IPicture to get HMETAFILE
   DWORD dwSize = 0;
   if (hResult == S_OK)
   {
      fseek(wmfFile, 0, SEEK_END);
      dwSize = ftell(wmfFile);
      fseek(wmfFile, 0, SEEK_SET);
   }
   
   HGLOBAL hGlobal = NULL;
   if (hResult == S_OK)
   {
      hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, dwSize);
      if (!hGlobal)
         hResult = E_FAIL;
   }
   
   char *pData = NULL;
   if (hResult == S_OK)
   {
      pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
      if (!pData)
         hResult = E_FAIL;
   }
   
   if (hResult == S_OK)
   {
      DWORD nRead = fread(pData, 1, dwSize, wmfFile);
      if (nRead != dwSize)
         hResult = E_FAIL;
   }
   
   if (hGlobal)
      GlobalUnlock(hGlobal);
   
   if (wmfFile)
      fclose(wmfFile);
   
   IStream *pStream = NULL;
   if (hResult == S_OK)
   {
      hResult = CreateStreamOnHGlobal(hGlobal,TRUE,&pStream);
   }
   
   IPicture *image = NULL;
   if (hResult == S_OK)
   {
      hResult = OleLoadPicture(pStream, dwSize, false, IID_IPicture, reinterpret_cast<LPVOID *>(&image));
   }
   
   if (pStream)
      pStream->Release();
   
   short pType;
   if (hResult == S_OK)
   {
      hResult = image->get_Type(&pType);
   }
   
   HMETAFILE hMetafile;
   if (hResult == S_OK && pType == PICTYPE_METAFILE)
   {
      hResult = image->get_Handle((unsigned int *)&hMetafile);
   }
   
   // Convert WMF image data to EMF image data
   UINT nCount = 0;
   if (hResult == S_OK)
   {
      nCount =  GetMetaFileBitsEx(hMetafile, 0, NULL);
      if (nCount <= 0)
         hResult = E_FAIL;
   }
   
   // Create Enhanced Metafile
   if (hResult == S_OK)
   {
      BYTE *pbBuffer = new BYTE[nCount];
      GetMetaFileBitsEx(hMetafile, nCount, pbBuffer);
      
      m_hEmf = SetWinMetaFileBits(nCount, pbBuffer, NULL, NULL);
      
      ExtractEmfFontInfo();
      if (pbBuffer)
         delete[] pbBuffer;
   }
   
   if (image)
      image->Release();
   
   if (hGlobal)
      GlobalFree(hGlobal);
   
   return hResult;
}

HRESULT FontExtractor::ExtractEmfFontInfo()
{
   HRESULT hResult = S_OK;

   if (m_hEmf != NULL)
   {
      if (m_hEmf == NULL)
         hResult = E_FAIL;

      if (hResult == S_OK)
      {

         ::GlobalLock(m_hEmf);

         HDC hDC = ::CreateCompatibleDC(NULL);
         RECT rect;
         rect.left = 0;
         rect.top = 0;
         rect.right = 100;
         rect.bottom = 100;

         ::EnumEnhMetaFile(hDC, m_hEmf, EnhMetaFileProcForCharCount, this, &rect);

         ::EnumEnhMetaFile(hDC, m_hEmf, EnhMetaFileProcForFont, this, &rect);


         ::DeleteDC(hDC);
         ::GlobalUnlock(m_hEmf);
         DeleteEnhMetaFile(m_hEmf);
         m_hEmf = NULL;
      }
   }

   return hResult;
}

FontExtractor::FontExtractor(LPCWSTR wszImagePath)
{
   m_bDoCopy = true;
   m_hEmf = NULL;
   m_nCharCount = 0;
   m_nActualChar = 0;
   m_wcFontFamily[0] = _T('\0');

   const WCHAR *pPoint = wcsrchr(wszImagePath, L'.');
   WCHAR wszSuffix[4];
   if (pPoint)
   {
      wcsncpy(wszSuffix, pPoint+1, 3);
      wszSuffix[3] = L'\0';
   }

   if (wcscmp(wszSuffix, L"emf") == 0)
   {
      m_hEmf = (HENHMETAFILE)GetEnhMetaFileW(wszImagePath);
      ExtractEmfFontInfo();
   }
   else
      ExtractWmfFontInfo(wszImagePath);

   if (wszImagePath)
   {
      BOOL bResult = DeleteFileW(wszImagePath);
   }
}

WCHAR *FontExtractor::GetFontFamily()
{
   return m_wcFontFamily;
}

void FontExtractor::SetFontFamily(WCHAR *wcFontFamily)
{
   if (m_bDoCopy)
      wcscpy(m_wcFontFamily, wcFontFamily);
}

short __declspec(dllexport) CALLBACK  ExtractFontFamily(const WCHAR *wszImagePath, WCHAR *wszFontFamily, int *iStringLength)
{
   FontExtractor *pFontExtractor = new FontExtractor(wszImagePath);

   wcscpy(wszFontFamily, pFontExtractor->GetFontFamily());
   *iStringLength = wcslen(wszFontFamily);

   if (pFontExtractor)
      delete pFontExtractor;

   return true;
}

short __declspec(dllexport) CALLBACK  FontSupportsString(const WCHAR *wszString, int iLength, const WCHAR *wszFontFamily, int isBold, int isItalic)
{
   HDC hDC = ::CreateCompatibleDC(NULL);

   if (iLength <= 0)
      return false;

   // Characters arre in Unicode private area
   if (wszString[0] >= 0xE000 && wszString[0] <= 0xF8FF)
      return false;

   LOGFONTW logFont;

   ::ZeroMemory(&(logFont), sizeof(LOGFONTW));  
												
   logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE; 
   logFont.lfHeight = -10;

   logFont.lfWidth       = 0;
   logFont.lfEscapement  = 0 ;
   logFont.lfOrientation = 0 ;

   if (isBold != 0)
      logFont.lfWeight = FW_BOLD;
   else
      logFont.lfWeight = FW_NORMAL;

   if (isItalic != 0)
      logFont.lfItalic = true;
   else
      logFont.lfItalic = false;

   logFont.lfUnderline     = FALSE ;
   logFont.lfStrikeOut     = FALSE ;
   logFont.lfCharSet       = DEFAULT_CHARSET ;
   logFont.lfOutPrecision  = OUT_TT_PRECIS; //OUT_TT_ONLY_PRECIS ;   
   logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS ;   
   logFont.lfQuality       = PROOF_QUALITY; 
   
   wcscpy(logFont.lfFaceName, wszFontFamily);  

   HFONT hFont = ::CreateFontIndirectW(&logFont);
   SelectObject(hDC, hFont);

   WORD  *pgi = (WORD *)malloc((iLength+10)*sizeof(WORD));
   DWORD dwReturn = ::GetGlyphIndicesW(hDC, wszString, iLength, pgi, GGI_MARK_NONEXISTING_GLYPHS);

   bool bResult = true;
   if (dwReturn != GDI_ERROR)
   {
      for (int i = 0; i < iLength && bResult; ++i)
      {
         if (pgi[i] == 0xffff)
            bResult = false;
      }
   }
   if (pgi)
      delete pgi;

   DeleteObject(hFont);

   ::DeleteDC(hDC);
   return bResult;
}

int GetValidFontStyle(Gdiplus::FontFamily &fontFamily, bool bIsBold, bool bIsItalic)
{
   if (bIsBold && bIsItalic && fontFamily.IsStyleAvailable(Gdiplus::FontStyleBoldItalic))
      return Gdiplus::FontStyleBoldItalic;

   if (bIsBold && fontFamily.IsStyleAvailable(Gdiplus::FontStyleBold))
      return Gdiplus::FontStyleBold;
   
   if (bIsItalic && fontFamily.IsStyleAvailable(Gdiplus::FontStyleItalic))
      return Gdiplus::FontStyleItalic;

   if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleRegular))
      return Gdiplus::FontStyleRegular;

   if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleBold))
      return Gdiplus::FontStyleBold;
   if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleItalic))
      return Gdiplus::FontStyleItalic;
   if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleBoldItalic))
      return Gdiplus::FontStyleBoldItalic;
   if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleUnderline))
      return Gdiplus::FontStyleUnderline;
   if (fontFamily.IsStyleAvailable(Gdiplus::FontStyleStrikeout))
      return Gdiplus::FontStyleStrikeout;
   return 0;
}

short __declspec(dllexport) CALLBACK  GetFontMetrics(const WCHAR *wszFontFamily, int iFontSize, int isBold, int isItalic, double *fAscent, double *fDescent)
{
   bool bResult = true;

   HDC hDC = ::CreateCompatibleDC(NULL);

   LOGFONTW logFont;

   ::ZeroMemory(&(logFont), sizeof(LOGFONTW));  

   logFont.lfHeight        = (-1)*abs(iFontSize);
   logFont.lfWidth         = 0;
   logFont.lfEscapement    = 0 ;
   logFont.lfOrientation   = 0 ;
   logFont.lfUnderline     = FALSE ;
   logFont.lfStrikeOut     = FALSE ;
   logFont.lfCharSet       = DEFAULT_CHARSET ;
   logFont.lfOutPrecision  = OUT_TT_PRECIS; //OUT_TT_ONLY_PRECIS ;   
   logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS ;  
   logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;  
   logFont.lfQuality       = PROOF_QUALITY;    
   if (isBold != 0)
      logFont.lfWeight = FW_BOLD;
   else
      logFont.lfWeight = FW_NORMAL;
   if (isItalic != 0)
      logFont.lfItalic = true;
   else
      logFont.lfItalic = false;
   wcscpy(logFont.lfFaceName, wszFontFamily);  

   Gdiplus::Font font(hDC, &logFont);
   if (font.GetLastStatus() != Gdiplus::Ok)
      bResult = false;

   Gdiplus::FontFamily fontFamily;
   if (bResult)
   {
      font.GetFamily(&fontFamily);
      if (fontFamily.GetLastStatus() != Gdiplus::Ok)
         bResult = false;
   }

   if (bResult)
   {
      int fontStyle = GetValidFontStyle(fontFamily, (isBold == 0 ? false : true), (isItalic == 0 ? false : true));

      *fAscent = fontFamily.GetCellAscent(fontStyle) * 
         (font.GetSize() / fontFamily.GetEmHeight(fontStyle)) ;

      *fDescent = fontFamily.GetCellDescent(fontStyle) * 
         (font.GetSize() / fontFamily.GetEmHeight(fontStyle)) ;
   }

   ::DeleteDC(hDC);
   return bResult;
}
/////////////////////////////////////////////////////////////////////////
//////////////////   Grab image from clipboard   ////////////////////////
/////////////////////////////////////////////////////////////////////////


short __declspec(dllexport) CALLBACK  ConvertMetafile(const WCHAR *wszImagePath, WCHAR *wszImageName, const WCHAR *wszImagePrefix, WCHAR *wszImageSuffix, int *iStringLength)
{
   bool bImageWritten = false;

   try {
      //TCHAR *szImagePath = ImageWriter::ConvertToTCHAR((WCHAR *)wszImagePath);
      //TCHAR *szImagePrefix = ImageWriter::ConvertToTCHAR((WCHAR *)wszImagePrefix);
      //TCHAR *szImageSuffix = ImageWriter::ConvertToTCHAR((WCHAR *)wszImageSuffix);

      ImageWriter *pImageWriter = new ImageWriter(wszImagePath, wszImagePrefix, wszImageSuffix);
      
      if (_wcsicmp(wszImageSuffix, L".emf") == 0)
      {
         pImageWriter->FindEmfImageData();
      }
      else
      {
         pImageWriter->FindWmfImageData();
      }
      
      
      bImageWritten = pImageWriter->GetResult();
      if (bImageWritten)
      {
         wcscpy(wszImageName, pImageWriter->GetImageFilename());
         *iStringLength = wcslen(wszImageName);
      }
      //delete szImagePath;
      //delete szImagePrefix;
      //delete szImageSuffix;
      delete pImageWriter;
   } 
   catch (...)
   {
   }
   
   
   
   return bImageWritten;
}

/////////////////////////////////////////////////////////////////////////
//////////////////   Grab image from clipboard   ////////////////////////
/////////////////////////////////////////////////////////////////////////


short __declspec(dllexport) CALLBACK  RewritePng(const WCHAR *wszImagePath, WCHAR *wszImageName, const WCHAR *wszImagePrefix, WCHAR *wszImageSuffix, int *iStringLength)
{
   bool bImageWritten = false;
   
   try {
      
      ImageWriter *pImageWriter = new ImageWriter(wszImagePath, wszImagePrefix, wszImageSuffix);
      
      pImageWriter->RewriteImage();
      bImageWritten = pImageWriter->GetResult();
      if (bImageWritten)
      {
         wcscpy(wszImageName, pImageWriter->GetImageFilename());
         *iStringLength = wcslen(wszImageName);
      }
      
      if (pImageWriter)
         delete pImageWriter;
      
   } 
   catch (...)
   {
   }
   
   
   
   return bImageWritten;
}

/////////////////////////////////////////////////////////////////////////
//////////////////   Write Metafile AS PNG       ////////////////////////
/////////////////////////////////////////////////////////////////////////


short __declspec(dllexport) CALLBACK  SaveMetafileAsPNG(const WCHAR *wszImagePath, WCHAR *wszImageName, const WCHAR *wszImagePrefix, WCHAR *wszImageSuffix, int *iStringLength, 
                                                        int iHorizontalResolution, int iVerticalResolution, int *iX, int *iY, int *iWidth, int *iHeight)
{
   bool bImageWritten = false;
   
   try {
      
      ImageWriter *pImageWriter = new ImageWriter(wszImagePath, wszImagePrefix, wszImageSuffix);
      pImageWriter->SetHorizontalResolution(iHorizontalResolution);
      pImageWriter->SetVerticalResolution(iVerticalResolution);
      
      pImageWriter->WriteMetafileAsPNG(false);
      
      
      bImageWritten = pImageWriter->GetResult();
      if (bImageWritten)
      {
         wcscpy(wszImageName, pImageWriter->GetImageFilename());
         *iStringLength = wcslen(wszImageName);
         *iX = pImageWriter->GetX();
         *iY = pImageWriter->GetY();
         *iWidth = pImageWriter->GetWidth();
         *iHeight = pImageWriter->GetHeight();
      }
      
      delete pImageWriter;
   } 
   catch (...)
   {
   }
   
   
   
   return bImageWritten;
}

short __declspec(dllexport) CALLBACK  SaveMetafileAsPNGWithoutGrid(const WCHAR *wszImagePath, WCHAR *wszImageName, const WCHAR *wszImagePrefix, WCHAR *wszImageSuffix, int *iStringLength, 
                                                                   int iHorizontalResolution, int iVerticalResolution, int *iX, int *iY, int *iWidth, int *iHeight)
{
   bool bImageWritten = false;
   
   try {
      
      ImageWriter *pImageWriter = new ImageWriter(wszImagePath, wszImagePrefix, wszImageSuffix);
      pImageWriter->SetHorizontalResolution(iHorizontalResolution);
      pImageWriter->SetVerticalResolution(iVerticalResolution);
      
      pImageWriter->WriteMetafileAsPNG(true);
      
      
      bImageWritten = pImageWriter->GetResult();
      if (bImageWritten)
      {
         wcscpy(wszImageName, pImageWriter->GetImageFilename());
         *iStringLength = wcslen(wszImageName);
         *iX = pImageWriter->GetX();
         *iY = pImageWriter->GetY();
         *iWidth = pImageWriter->GetWidth();
         *iHeight = pImageWriter->GetHeight();
      }
      
      delete pImageWriter;
   } 
   catch (...)
   {
   }
   
   
   
   return bImageWritten;
}

/////////////////////////////////////////////////////////////////////////
/////////   Extract image dimension and position from Metafile  /////////
/////////////////////////////////////////////////////////////////////////


short __declspec(dllexport) CALLBACK  GetImageDimension(const WCHAR *wszImagePath, const WCHAR *wszImagePrefix, const WCHAR *wszImageSuffix,
                                                        int iHorizontalResolution, int iVerticalResolution, int *iX, int *iY, int *iWidth, int *iHeight)
{
   bool bImageWritten = false;
   
   try {
      
      ImageWriter *pImageWriter = new ImageWriter(wszImagePath, wszImagePrefix, wszImageSuffix);
      pImageWriter->SetHorizontalResolution(iHorizontalResolution);
      pImageWriter->SetVerticalResolution(iVerticalResolution);
      
      pImageWriter->ExtractDimensionFromMetafile(true);
      
      
      bImageWritten = pImageWriter->GetResult();
      if (bImageWritten)
      {
         *iX = pImageWriter->GetX();
         *iY = pImageWriter->GetY();
         *iWidth = pImageWriter->GetWidth();
         *iHeight = pImageWriter->GetHeight();
      }
      
      delete pImageWriter;
   } 
   catch (...)
   {
   }
   
   
   
   return bImageWritten;
}

short __declspec(dllexport) CALLBACK  GetEMFDimension(const WCHAR *wszFullImageName, 
                                                      int iHorizontalResolution, int iVerticalResolution, 
                                                      int *iX, int *iY, int *iWidth, int *iHeight)
{
   bool bImageWritten = false;
   
   try {
      
      ImageWriter *pImageWriter = new ImageWriter(wszFullImageName);
      pImageWriter->SetHorizontalResolution(iHorizontalResolution);
      pImageWriter->SetVerticalResolution(iVerticalResolution);
      
      pImageWriter->ExtractDimensionFromMetafile(false);
      
      
      bImageWritten = pImageWriter->GetResult();
      if (bImageWritten)
      {
         *iX = pImageWriter->GetX();
         *iY = pImageWriter->GetY();
         *iWidth = pImageWriter->GetWidth();
         *iHeight = pImageWriter->GetHeight();
      }
      
      delete pImageWriter;
   } 
   catch (...)
   {
   }
   
   
   
   return bImageWritten;
}

/////////////////////////////////////////////////////////////////////////
//////////////////   END Write Metafile As PNG   ////////////////////////
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//////////////////   check if metafile is empty  ////////////////////////
/////////////////////////////////////////////////////////////////////////


short __declspec(dllexport) CALLBACK  CheckMetafile(const WCHAR *wszImagePath)
                                                        
{
   bool bImageCorrect = false;
   
   try {
      
      ImageWriter *pImageWriter = new ImageWriter(wszImagePath);
      pImageWriter->SetHorizontalResolution(72);
      pImageWriter->SetVerticalResolution(72);
      
      pImageWriter->CheckEnhancedMetafile();
      
      
      bImageCorrect = pImageWriter->GetResult();
      
      delete pImageWriter;
 
      if (!bImageCorrect)
         DeleteFileW(wszImagePath);
   } 
   catch (...)
   {
   }
   
   
   
   return bImageCorrect;
}
/////////////////////////////////////////////////////////////////////////
//////////////////  END check if metafile is empty //////////////////////
/////////////////////////////////////////////////////////////////////////


short __declspec(dllexport) CALLBACK  StartGdiplus()
{
   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   
   Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
   
#ifdef _DEBUG
   WCHAR wszTmp[1024];
   _swprintf(wszTmp, L"StartGdiplus %d\n", gdiplusToken);
   PPT2LECTURNITYDebugMsg(wszTmp);
#endif
   
   return true;
}

short __declspec(dllexport) CALLBACK  ShutdownGdiplus()
{
   Gdiplus::GdiplusShutdown(gdiplusToken);
   
#ifdef _DEBUG
   WCHAR wszTmp[1024];
   _swprintf(wszTmp, L"ShutdownGdiplus %d\n", gdiplusToken);
   PPT2LECTURNITYDebugMsg(wszTmp);
#endif
   
   return true;
}

