// ppt2lecturnity.cpp : Definiert den Einsprungpunkt für die DLL-Anwendung.
//
#include <windows.h>

#include <png.h>
#include <ocidl.h>
#include <olectl.h>


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
   MessageBox(NULL,"ENTRY 1",NULL,MB_OK);
    return TRUE;
}

__declspec(dllexport) bool ConvertWMFtoPNGA(char *bwmfName, char *bpngName, bool eraseTransparenz)
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
		return false;

	if (OleLoadPicture(pStream,dwSize,false,IID_IPicture,
		reinterpret_cast<LPVOID *>(&image)) != S_OK)
	{
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

   int offTop = 0;
   int offLeft = 0;
   int offBottom = 0;
   int offRight = 0;

   if (eraseTransparenz)
   {
      for (y = 0; y < height; ++y)
      {
         p1 = bitmapData1 + (width*4*y);
         p2 = bitmapData2 + (width*4*y);

         for (x = 0; x < width; ++x)
         {
            transparent = true;
            if (*p1 == *p2)
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
            if (*p1 == *p2)
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
            if (*p1 == *p2)
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
            if (*p1 == *p2)
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

   png_set_IHDR(png_ptr, info_ptr, pngWidth, pngHeight, 8, PNG_COLOR_TYPE_RGB_ALPHA,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	int row_stride = 4 * pngWidth;

	info_ptr->pixel_depth = 32;
	info_ptr->channels = 4;
	info_ptr->bit_depth = 8;
	info_ptr->color_type = PNG_COLOR_TYPE_RGB_ALPHA;
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
         transparent = true;
         if (*p1 == *p2)
         {
            if (*(p1+1) == *(p2+1))
            {
               if (*(p1+2) == *(p2+2))
               {
                  transparent = false;
               }
            }
         }
          
         *q = *(p1+2); ++q;
		   *q = *(p1+1); ++q;
			*q = *(p1); ++q;
         if (transparent)
            *q = 0;
         else
            *q = 255;
         ++q;
         p1 += 4;
         p2 += 4;
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

