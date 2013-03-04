#ifndef __TWLCMAIN_H__
#define __TWLCMAIN_H__

#pragma warning(disable: 4018 4244 4996)

#include <windows.h>
#include <vfw.h>
#include <vector>

#ifdef LSGC_EXPORTS
   #define LSGC_EXPORT __declspec(dllexport)
#else
   #define LSGC_EXPORT __declspec(dllimport)
#endif

static const DWORD FOURCC_LSGC = mmioFOURCC('L','S','G','C');   // our compressed format

static const int VERSION = 0;

static const int SPLIT_LIMIT = 40;

// internal bitmap type identifier
static const char BM_COMPRESSED_RGB15 = -15; // 555
static const char BM_COMPRESSED_RGB16 = -16; // 565
static const char BM_COMPRESSED_RGB24 = -24;
static const char BM_UNCOMPRESSED_RGB15 = 15;
static const char BM_UNCOMPRESSED_RGB16 = 16;
static const char BM_UNCOMPRESSED_RGB24 = 24;
static const char BM_UNCOMPRESSED_RGB32 = 32;
static const char BM_UNKNOWN = 0;

// internal frame type identifier
static const char KEY_FRAME = 0;
static const char DELTA_FRAME = 2; // delta frame with data
static const char IDENTITY_FRAME = 127; // delta frame without data

// external header flags
static const short CONTAINS_MOTION = 0x0100;
static const short CONTAINS_MOUSE  = 0x0400;
static const short CONTAINS_CLICKS = 0x0800;

static const bool DO_THE_MOTION = false;


extern HMODULE invoker;


struct LSGC_EXPORT CodecInst {
   bool m_bDecompressBeginCalled;
   int  m_iCurrentDecompressFrame;
   char *m_pLastImage;
   BYTE *m_pCurrentMouseData;
   int   m_iCurrentMouseLength;
   POINT m_ptLastMousePos;
   POINT m_ptLastMouseSize;

   char *m_lastCompressedImage;
   int   m_lastCompressedImageSize;
   RECT  m_rcLastMouseCompress;
   ULONG m_lLastMouseCrc32Compress;

   bool m_bWriteInitialFrame;

   // callback method for structuring optimisation
   LONG (CALLBACK *m_pStructuringCallback) (LPARAM lParam, UINT message, LONG l);
   
   CodecInst() {
      m_bDecompressBeginCalled = false;
      m_iCurrentDecompressFrame = 0;
      m_pLastImage = NULL;
      m_pCurrentMouseData = NULL;
      m_iCurrentMouseLength = 0;
      m_lastCompressedImage = NULL;
      m_lastCompressedImageSize = 0;
      ZeroMemory(&m_rcLastMouseCompress, sizeof RECT);
      m_lLastMouseCrc32Compress = 0;
      m_bWriteInitialFrame = true;
      m_pStructuringCallback = NULL;
   }
   
   BOOL QueryAbout();
   DWORD About(HWND hwnd);
   
   BOOL QueryConfigure();
   DWORD Configure(HWND hwnd);
   
   DWORD GetState(LPVOID pv, DWORD dwSize);
   DWORD SetState(LPVOID pv, DWORD dwSize);
   
   DWORD GetInfo(ICINFO* icinfo, DWORD dwSize);
   
   DWORD CompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
   DWORD CompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
   DWORD CompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
   DWORD CompressGetSize(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
   DWORD Compress(ICCOMPRESS* icinfo, DWORD dwSize);
   DWORD CompressEnd();
   
   DWORD DecompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
   DWORD DecompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
   DWORD DecompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
   DWORD Decompress(ICDECOMPRESS* icinfo, DWORD dwSize);
   DWORD DecompressGetPalette(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
   DWORD DecompressEnd();
   
   DWORD SetStructuringCallbackProc(ICSETSTATUSPROC *icsetstatusProc, DWORD cbSize);

   // PZI: methods defined as static (since they do static stuff only)
   //      determineChangeArea & splitChangeArea are used by SG extended for structuring
   // PZI: return value changed from bool to int - will return number of changed pixels now
   //
   // moved to lutils (lmisc.h/.cpp)
   //
   //static int determineChangeArea(unsigned char* bm1, unsigned char* bm2, 
   //   int width, int height, int stride, RECT* result);
   //static void splitChangeArea(unsigned char* bm1, unsigned char* bm2, 
   //   int width, int height, int stride, RECT bounds, RECT *areas, int *count);

   static bool findMotion(unsigned char* bm1, unsigned char* bm2, 
      int width, int height, int stride, RECT bounds, POINT* vector, RECT* area);
   static void compensateMotion(unsigned char* bm, 
      int width, int stride, RECT area, POINT vector);
   static void stretchPixelLine(unsigned char* inBuffer, unsigned char* outBuffer,  
      int pixelCount, int inStride, int outStride);
   static int matchingCount(unsigned char* bm1, unsigned char* bm2,
      int width, int stride, RECT &rect1, RECT &rect2, bool fullMatchNeeded);

private:
    // Pixel format of image data array must be BGRA!
    void PlaceImageInArray(BYTE *pImage, int iImageWidth, int iImageHeight,
        BYTE *pArray, int iOutStride, int iArrayWidth, int iArrayHeight,
        int iOffsetX, int iOffsetY);

    // Pixel format of data returned is BGRA!
    BYTE* CreateClickImage(int iRadius, COLORREF clr, POINT *pptImageSize);
};

CodecInst* Open(ICOPEN* icinfo);
DWORD Close(CodecInst* pinst);


class RawDiskImage
{
public:
   static void write(unsigned char* bm, int width, int height, int stride, RECT bounds,
      char* outFileName)
   {
      FILE* file = fopen(outFileName, "wb");
      int writtenBytes = 0;


      int writeWidth = bounds.right-bounds.left;
      int writeHeight = bounds.bottom-bounds.top;
      
      char sizes[40];
      sprintf(sizes, "%d %d\n", writeWidth, writeHeight);

      fwrite("P6\n", 1, 3, file);
      fwrite(sizes, 1, strlen(sizes), file);
      fwrite("255\n", 1, 4, file);

      unsigned char alpha = 255;

      for (int y=bounds.bottom; y>=bounds.top; --y)
      {
         int offset = (y*width+bounds.left)*stride;
         for (int x=bounds.left; x<bounds.right; ++x)
         {
            if (stride > 2)
            {
               //fwrite(&alpha, 1, 1, file);
               fwrite(bm+offset+2, 1, 1, file);
               fwrite(bm+offset+1, 1, 1, file);
               fwrite(bm+offset, 1, 1, file);
               writtenBytes += 3;
            }
            else
               fwrite(bm+offset, 1, 2, file);

            offset += stride;
         }
         //fwrite("\n", 1, 1, file);
      }
      fflush(file);
      fclose(file);
   }
};

// moved to lutils (lmisc.h/.cpp)
//// PZI: for performance optimization of structuring clips (SG extended)
//struct StructuringValues{
//   int iFrameNr;
//   int iPixelCount;
//   int iAreaCount;
//};

#endif