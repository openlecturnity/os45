#include <stdio.h>
#include <crtdbg.h>
#include <math.h>
#include <vector>
#include <crtdbg.h>
#include "twlc.h"
#include "ZlibWrapper.h"
#include "DataWrapper.h"
#include "ColorStuff.h"

#include "lutils.h"


HMODULE invoker = 0;

// Currently debugging output disabled
#define DEBUG(x)

#define CLICK_DATA_MARKER 0x74

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD reason, LPVOID) 
{
   invoker = (HMODULE) hinstDLL;

   return TRUE;
}



TCHAR szDescription[] = TEXT("Lecturnity Screen Grabbing Codec v1.2");
TCHAR szName[]        = TEXT("LSGC");


int w(RECT r) {
   return r.right-r.left+1;
}

int h(RECT r) {
   return r.bottom-r.top+1;
}

bool javaCalling() {
   TCHAR apppath[MAX_PATH];
   if (GetModuleFileName(NULL, apppath, MAX_PATH)) {
      TCHAR* appname = strrchr(apppath, '\\');
      appname = appname ? appname+1 : apppath;

      if (!lstrcmpi((appname), TEXT("javaw.exe")) ||
         !lstrcmpi((appname), TEXT("java.exe")))
         return true;
   }
   return false;
}


/********************************************************************
********************************************************************/

CodecInst* Open(ICOPEN* icinfo) {
   if (icinfo && icinfo->fccType != ICTYPE_VIDEO)
      return NULL;
   
   CodecInst* pinst = new CodecInst();
   
   if (icinfo) icinfo->dwError = pinst ? ICERR_OK : ICERR_MEMORY;
   
   return pinst;
}

DWORD Close(CodecInst* pinst) {
   //    delete pinst;       // this caused problems when deleting at app close time
   return 1;
}


/********************************************************************
********************************************************************/

BOOL CodecInst::QueryAbout() { return FALSE; }

DWORD CodecInst::About(HWND hwnd) {
   return ICERR_ERROR;
}

BOOL CodecInst::QueryConfigure() { return FALSE; }

DWORD CodecInst::Configure(HWND hwnd) {
   return ICERR_ERROR;
}


/********************************************************************
********************************************************************/


// we have no state information which needs to be stored

DWORD CodecInst::GetState(LPVOID pv, DWORD dwSize) { return 0; }

DWORD CodecInst::SetState(LPVOID pv, DWORD dwSize) { return 0; }


DWORD CodecInst::GetInfo(ICINFO* icinfo, DWORD dwSize) {
   if (icinfo == NULL)
      return sizeof(ICINFO);
   
   if (dwSize < sizeof(ICINFO))
      return 0;
   
   icinfo->dwSize            = sizeof(ICINFO);
   icinfo->fccType           = ICTYPE_VIDEO;
   icinfo->fccHandler        = FOURCC_LSGC;
   icinfo->dwFlags           = VIDCF_TEMPORAL | VIDCF_FASTTEMPORALD | VIDCF_FASTTEMPORALC;
   // you could add here | VIDCF_FASTTEMPORALD | VIDCF_FASTTEMPORALC;
   // if you want to handle/store the last image data (lpbiPrev or lpbiOut) yourself
   
   icinfo->dwVersion         = VERSION;
   icinfo->dwVersionICM      = ICVERSION;
   MultiByteToWideChar(CP_ACP, 0, szDescription, -1, icinfo->szDescription, sizeof(icinfo->szDescription)/sizeof(WCHAR));
   MultiByteToWideChar(CP_ACP, 0, szName, -1, icinfo->szName, sizeof(icinfo->szName)/sizeof(WCHAR));
   
   return sizeof(ICINFO);
}


/********************************************************************
********************************************************************/


struct PrintBitmapType {
  char s[32];
  PrintBitmapType(LPBITMAPINFOHEADER lpbi) {
    if (!lpbi)
      strcpy(s,  "(null)");
    else {
      *(DWORD*)s = lpbi->biCompression;
      s[4] = 0;
      if (!isalnum(s[0]) || !isalnum(s[1]) || !isalnum(s[2]) || !isalnum(s[3]))
        wsprintfA(s, "%x", lpbi->biCompression);
      wsprintfA(strchr(s, 0), ", %d bits", lpbi->biBitCount);
    }
  }
};


/********************************************************************
********************************************************************/

static int GetBitmapType(LPBITMAPINFOHEADER lpbi) {
   if (!lpbi)
      return BM_UNKNOWN;
   
   const int fourcc = lpbi->biCompression;
   const int bitcount = lpbi->biBitCount;
   
   if (fourcc == BI_RGB || fourcc == ' BID') { //what's this?
      if (bitcount == 16)
         return BM_UNCOMPRESSED_RGB15;   
      else if (bitcount == 24)
         return BM_UNCOMPRESSED_RGB24;
      else if (bitcount == 32)
         return BM_UNCOMPRESSED_RGB32;
   } else if (fourcc == FOURCC_LSGC) {
      if (bitcount == 24)
         return BM_COMPRESSED_RGB24;
      else if (bitcount == 16)
         return BM_COMPRESSED_RGB15;
   }
 
   return BM_UNKNOWN;
}


static bool CanCompress(LPBITMAPINFOHEADER lpbiIn) {
   int intype = GetBitmapType(lpbiIn);
   
   return (intype > 0);
}


static bool CanCompress(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut) {
   if (!lpbiOut)
      return CanCompress(lpbiIn);
   
   int outtype = GetBitmapType(lpbiOut);
   
   return (CanCompress(lpbiIn) && outtype < 0);
}


/********************************************************************
********************************************************************/

DWORD CodecInst::CompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut) {
   return CanCompress(lpbiIn, lpbiOut) ? ICERR_OK : ICERR_BADFORMAT;
}


DWORD CodecInst::CompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut) {
   if (!CanCompress(lpbiIn))
      return ICERR_BADFORMAT;
   
   
   if (!lpbiOut)
      return sizeof(BITMAPINFOHEADER);
   // you can also return more than this (eg +4) and then add
   // some extra information after lpbiOut + sizeof(BITMAPINFOHEADER)
   // this extra information is permanently stored in the avi header
   // and can be read upon decompression
   // note: you must also add this (+4) to lpbiOut->biSize
  
   *lpbiOut = *lpbiIn;
   lpbiOut->biSize = sizeof(BITMAPINFOHEADER);
   lpbiOut->biCompression = FOURCC_LSGC;
   lpbiOut->biClrImportant = lpbiOut->biClrUsed = 0;
   lpbiOut->biBitCount = lpbiIn->biBitCount > 16 ? 24 : 16;
   
#ifdef _DEBUG
   char buf[500];
   sprintf(buf, "CompressGetFormat: input = %s, output = %s\n", &PrintBitmapType(lpbiIn), &PrintBitmapType(lpbiOut)); 
   DEBUG(buf);
#endif
    
   return ICERR_OK;
}


DWORD CodecInst::CompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut) {
#ifdef _DEBUG
   char buf[512];
   sprintf(buf, "CompressBegin: input = %s, output = %s\n", 
      &PrintBitmapType(lpbiIn), &PrintBitmapType(lpbiOut));
   DEBUG(buf);
#endif

   if (!CanCompress(lpbiIn, lpbiOut))
      return ICERR_BADFORMAT;
   
   CompressEnd();  // free old resources if necessary
   
   int intype = GetBitmapType(lpbiIn);
   int outtype = GetBitmapType(lpbiOut);


   
   return ICERR_OK;
}

DWORD CodecInst::CompressGetSize(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut) {
    // Assume a bad worst case...
    // This space (in the output buffer) is also used
    // to specify a little bit of extra data at the end of the buffer 
    // (Quick capture  or mouse or click data ie.)

    int iSecurityMarginForExtraInfo = 100000;

    return lpbiIn->biWidth * lpbiIn->biHeight * 5 + iSecurityMarginForExtraInfo;
}



double currentTime() {
   LARGE_INTEGER liStart;     
   LARGE_INTEGER liFreq; 

   QueryPerformanceCounter(&liStart); 
   QueryPerformanceFrequency(&liFreq);
   double seconds = ((double)liStart.LowPart/(double)liFreq.LowPart); 

   return seconds;
}


DWORD CodecInst::Compress(ICCOMPRESS* icinfo, DWORD dwSize) {

   if (icinfo->lpckid)
      *icinfo->lpckid = FOURCC_LSGC;
 
   if (!CanCompress(icinfo->lpbiInput, icinfo->lpbiOutput))
	   return ICERR_BADFORMAT;

#ifdef _DEBUG
   char buf[500];
   sprintf(buf, "### Compress: input = %s, output = %s ", 
      &PrintBitmapType(icinfo->lpbiInput), &PrintBitmapType(icinfo->lpbiOutput)); 
   DEBUG(buf);
   sprintf(buf, "%dx%d ", icinfo->lpbiInput->biWidth, icinfo->lpbiInput->biHeight);
   DEBUG(buf);
   sprintf(buf, "%d inBit, %d outBit\n", 
      icinfo->lpbiInput->biBitCount, icinfo->lpbiOutput->biBitCount);
   DEBUG(buf);
#endif

#ifdef _DEBUG
   bool writePrevImage = false;
   if (writePrevImage)
   {
      BITMAPINFOHEADER *pHeader = icinfo->lpbiPrev;
      void *data = icinfo->lpPrev;
      BITMAPFILEHEADER bmpHeader;
      bmpHeader.bfType      = 'BM';
      bmpHeader.bfReserved1 = 0;
      bmpHeader.bfReserved2 = 0;
      bmpHeader.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
      bmpHeader.bfSize      = bmpHeader.bfOffBits + pHeader->biSizeImage;

      FILE* file = fopen("C:\\imagecap.bmp\000", "wb");
      fwrite(&bmpHeader, sizeof(char), sizeof BITMAPFILEHEADER, file);
      fwrite(pHeader, sizeof(char), sizeof BITMAPINFOHEADER, file);
      fwrite(data, sizeof(char), pHeader->biSizeImage, file);
      fclose(file);
//      FILE *file = fopen("C:\\lpprev.raw", "wb");
//      fwrite(icinfo->lpPrev, icinfo->lpbiPrev->biSizeImage, 1, file);
//      fflush(file);
//      fclose(file);
   }
#endif

   int inBitCount = icinfo->lpbiInput->biBitCount;
   int inStride = inBitCount / 8;
   int outStride = icinfo->lpbiOutput->biBitCount / 8;
   char compressedFormat = inBitCount > 16 ? BM_COMPRESSED_RGB24 : BM_COMPRESSED_RGB15;
   int biWidth = icinfo->lpbiInput->biWidth;
   int biHeight = icinfo->lpbiInput->biHeight;
   // if width * stride is not a multiple of 4, each image scan line has to be
   // padded to a multiple of 4 bytes.
   int inPadding = LMisc::calcPadding(biWidth, inStride);
   unsigned long outLength = CompressGetSize(icinfo->lpbiInput, NULL);
   long compressLength = 0;

   unsigned char* in = (unsigned char*)icinfo->lpInput;
   unsigned char* out = (unsigned char*)icinfo->lpOutput;
   //unsigned char* prev = (unsigned char*)icinfo->lpPrev;
   unsigned char* prev = (unsigned char*) m_lastCompressedImage;

   char frameType = KEY_FRAME;
   if ((icinfo->dwFlags & ICCOMPRESS_KEYFRAME) == 0 && prev != NULL)
      frameType = DELTA_FRAME;
   
   bool hasQuickCaptureInfo = 
      out[outLength-3] == 'Q' && out[outLength-2] == 'u' && out[outLength-1] == 'C';
   // Note: Quick capture is no longer sent to Lsgc (4.0.p6) and thus obsolete here.
   //       The mouse and click handling below achieves the same (and better).
   // TODO: Only caveat here: full images need to be compared though there is no change...


   bool bHasMousePosition = 
       out[outLength-2] == 'M' && out[outLength-1] == 'o';
   int iMouseDataLength = 0;

   // Mostly this will be false because mouse data is always present
   // Second check see below
   bool bHasClickData = 
       out[outLength-2] == 'C' && out[outLength-1] == 'k';
   BYTE *pClickDataEnd = out + outLength;

   bool bMousePositionDifferent = m_bWriteInitialFrame;
   bool bMouseShapeDifferent = m_bWriteInitialFrame;

   if (bHasMousePosition) {

       // Data is also written in the array like this:
       short *pValues = (short *)(out + outLength - 10);
       short x = pValues[0];
       short y = pValues[1];
       short w = pValues[2];
       short h = pValues[3];

       RECT rcCurrentMouse;

       rcCurrentMouse.left = x;
       rcCurrentMouse.top = y;
       rcCurrentMouse.right = x + w;
       rcCurrentMouse.bottom = y + h;

       if (rcCurrentMouse.left != m_rcLastMouseCompress.left ||
           rcCurrentMouse.top != m_rcLastMouseCompress.top ||
           rcCurrentMouse.right != m_rcLastMouseCompress.right ||
           rcCurrentMouse.bottom != m_rcLastMouseCompress.bottom) {

            bMousePositionDifferent = true;
            m_rcLastMouseCompress = rcCurrentMouse;
       }

       int iCursorDataSize = w*h*4; // ARGB data

       // Also used below... esp. +/-10
       iMouseDataLength = iCursorDataSize + 10;

       BYTE *pData = (out + outLength - iMouseDataLength);

       CRC32 crc;
       crc.Update(pData, 0, iCursorDataSize);

       ULONG lCrc32Compress = crc.Value();

       bMouseShapeDifferent = m_lLastMouseCrc32Compress != lCrc32Compress;
       if (!bMouseShapeDifferent && frameType == KEY_FRAME)
           bMouseShapeDifferent = true; // not really but write it anyway for a key frame

       m_lLastMouseCrc32Compress = lCrc32Compress;


       // Do check for click data again
       if (!bHasClickData) {
           pClickDataEnd = pData;
           pData -= 2;
           bHasClickData = pData[0] == 'C' && pData[1] == 'k';
       }
   }

   bool bWillWriteMouse = bMousePositionDifferent || bMouseShapeDifferent || frameType == KEY_FRAME;

   bool bWillWriteClickData = false;
   if (bHasClickData) {
       // Also read below like this
       short *pValues = (short *)(pClickDataEnd - 4);
       int iClickCount = pValues[0]; // Data is also written in the array like this
       bWillWriteClickData = iClickCount > 0 && iClickCount < 256; // fits in a length byte
   }


   //
   // determine initial change area if it is a delta frame
   //
   RECT change; // default is key frame bounds
   change.left = change.top = 0;
   change.right = biWidth-1; change.bottom = biHeight-1;


   int iChangedPixelCount = 0;

   if (frameType == DELTA_FRAME) {
      *icinfo->lpdwFlags = 0; // no AVIIF_KEYFRAME
   
      bool bChangeDetected = false;

      if (hasQuickCaptureInfo) {
         bChangeDetected = out[outLength-4] != 0;
      } else {
         iChangedPixelCount = LMisc::determineChangeArea(prev, in, biWidth, biHeight, inStride, &change);
         
         bChangeDetected = iChangedPixelCount > 0;

         if (!bChangeDetected)
             bChangeDetected = bWillWriteMouse;

         if (!bChangeDetected)
             bChangeDetected = bWillWriteClickData;
      }


      if (!bChangeDetected)
         frameType = IDENTITY_FRAME;
   } else {
      *icinfo->lpdwFlags = AVIIF_KEYFRAME;
   }


   //
   // if delta frame: find motion and compensate it
   // for all frames: write compressed data in the change area
   // 
   bool motionFound = false;
   POINT motionVector = {-1, -1};
   RECT motionArea = {-1, -1, -1, -1};
   if (frameType != IDENTITY_FRAME)
   {
      if (frameType == DELTA_FRAME)
      {
         int changeWidth = change.right-change.left+1;
         int changeHeight = change.bottom-change.top+1;
         if (changeWidth > 20 && changeHeight > 20)
         {
            if (DO_THE_MOTION)
            {
#ifdef _DEBUG
               double second1 = currentTime();
#endif
               motionFound = findMotion(prev, in, biWidth, biHeight,
                  inStride, change, &motionVector, &motionArea);
#ifdef _DEBUG
               double second2 = currentTime();
               char buf[80];
               sprintf(buf, "Motion check time = %2.7fs\n", second2-second1);
               DEBUG(buf);
#endif
            }


            if (motionFound)
            {
#ifdef _DEBUG
               char guf[320];
               sprintf(guf, "*** Motion found = %d,%d,%d,%d -> %d,%d\n",
                  motionArea.left, motionArea.top, 
                  motionArea.right, motionArea.bottom, motionVector.x, motionVector.y);
               DEBUG(guf);
               double second3 = currentTime();
#endif

               compensateMotion(prev, biWidth, inStride, motionArea, motionVector);

               // recompute change area
               //bool changeDetected = determineChangeArea(
               //   prev, in, biWidth, biHeight, inStride, &change)>0; //PZI: return value changed from bool to int
              

#ifdef _DEBUG
               double second4 = currentTime();
            
               sprintf(guf, "Motion compensation time = %2.7fs\n", second4-second3);
               DEBUG(guf);
#endif
            }
            
         }
      } // if DELTA_FRAME
   } // != IDENTITY_FRAME
   
   //
   // write (uncompressed) header
   //
   OutputDataWrapper dataOutH(out, 4);
   dataOutH.write((char)0); // version
   dataOutH.write(frameType);
   
   short flags = 0;

   if (motionFound)
      flags |= CONTAINS_MOTION;
   if (bHasMousePosition && bWillWriteMouse)
       flags |= CONTAINS_MOUSE;
   if (bHasClickData && bWillWriteClickData)
       flags |= CONTAINS_CLICKS;

   dataOutH.write(flags); // flags
   
#ifdef _DEBUG
   sprintf(buf, "Writing frame type %d.\n", frameType);
   DEBUG(buf);
#endif
   
   
   out += 4;
   compressLength += 4;
   outLength -= 4;
   
   if (frameType != IDENTITY_FRAME)
   {
      RECT *changes = NULL;
      int splitLength = 0;
      if (frameType == DELTA_FRAME)
      {
         if (!hasQuickCaptureInfo)
         {
            splitLength = SPLIT_LIMIT;
            changes = (RECT*)malloc(SPLIT_LIMIT*sizeof(RECT));
            // changes = new RECT[SPLIT_LIMIT]; // ==> delete[] changes;
            LMisc::splitChangeArea(prev, in, biWidth, biHeight, inStride, 
               change, changes, &splitLength);
         }
         else
         {
            splitLength = (unsigned)out[outLength-4];
            changes = (RECT*)malloc(splitLength*sizeof(RECT));
            InputDataWrapper dataInR(out+outLength-4-splitLength*8, splitLength*8);

#ifdef _DEBUG
            sprintf(buf, "Found %d quick compress rects.\n", splitLength);
            DEBUG(buf);
#endif

            for (int i=0; i<splitLength; ++i)
            {
               changes[i].left = dataInR.readIntelShort();
               changes[i].top = dataInR.readIntelShort();
               changes[i].right = dataInR.readIntelShort();
               changes[i].bottom = dataInR.readIntelShort();
            }

         }
      }
      else // !DELTA_FRAME -> KEY_FRAME
      {
         changes = (RECT*)malloc(sizeof(RECT));
         changes[0] = change;
         splitLength = 1;
      }


      // PZI: calculate parameters for structuring performance optimization (SG extended)
      if (frameType == KEY_FRAME && prev)
      {
         // calculate changes for keyframes
         RECT changeTemp; // default is key frame bounds // defined again to avoid overwriting original
         changeTemp.left = changeTemp.top = 0;
         changeTemp.right = biWidth-1; changeTemp.bottom = biHeight-1;
         iChangedPixelCount = LMisc::determineChangeArea(prev, in, biWidth, biHeight, inStride, &changeTemp);

         free(changes);
         splitLength = SPLIT_LIMIT;
         changes = (RECT*)malloc(SPLIT_LIMIT*sizeof(RECT));
         LMisc::splitChangeArea(prev, in, biWidth, biHeight, inStride, changeTemp, changes, &splitLength);
      }

      // count changes
      int iChangedAreaCount = 0;
      for (int i=0; i<splitLength; ++i)
      {
         int width = changes[i].right-changes[i].left+1;
         int height = changes[i].bottom-changes[i].top+1;
         iChangedAreaCount += width * height;
      }

      // write parameters
      if(iChangedAreaCount > 0) {
         // adjust pixel counter for first frame (to maximum)
         if( m_bWriteInitialFrame ) {
            iChangedPixelCount = iChangedAreaCount;
            m_bWriteInitialFrame = false;
         }

         // send data to callback function
         if(m_pStructuringCallback) {
            LMisc::StructuringValues values;
            values.iFrameNr = icinfo->lFrameNum; // BUGFIX 5025: set correct frame number
            values.iPixelCount = iChangedPixelCount;
            values.iAreaCount = iChangedAreaCount;
            m_pStructuringCallback(0, ICSTATUS_STATUS, (LONG)&values);
         }
      }

      // now reset original keyframe values
      if (frameType == KEY_FRAME)
      {
         free(changes);
         changes = (RECT*)malloc(sizeof(RECT));
         changes[0] = change;
         splitLength = 1;
      }
      // PZI: end (performance optimization)


#ifdef _DEBUG
      sprintf(buf, "Change area = %d,%d %dx%d\n", change.left, change.top, w(change), h(change));
      DEBUG(buf);
      DEBUG("Split areas = ");
      for (int i=0; i<splitLength; ++i)
      {
         sprintf(buf, "%d,%d %dx%d ", changes[i].left, changes[i].top, w(changes[i]), h(changes[i]));
         DEBUG(buf);
      }
      DEBUG("\n");
#endif
      
      //
      // write (compressed) data
      //
#ifdef _DEBUG
      double second3 = currentTime();
#endif


      OutputZlibWrapper zlib(out, outLength);

      _ASSERT(motionFound == false);
      if (motionFound)
      {
         unsigned char bufferM[12];
         OutputDataWrapper dataOutM(bufferM, 12);
         dataOutM.write((short)motionArea.left);
         dataOutM.write((short)motionArea.top);
         dataOutM.write((short)(motionArea.right-motionArea.left+1));
         dataOutM.write((short)(motionArea.bottom-motionArea.top+1));
         dataOutM.write((short)motionVector.x);
         dataOutM.write((short)motionVector.y);

         zlib.compress(bufferM, 12);
      }
     
      unsigned char* control = NULL;
         
      //
      // write change rectangle (for key frames this is the whole area)
      //

      unsigned char bufferR[12];
      OutputDataWrapper dataOutR(bufferR, 12);
      
      // splitLength can now also be 0: if the pointer is changed which is not
      // part of the image anymore...
      // _ASSERT(splitLength > 0);


      dataOutR.write((short)splitLength); // number of change rects

      // Can also be "0": flush it directly
      zlib.compress(bufferR, dataOutR.countWrittenBytes());
      dataOutR.clear();

#ifdef _DEBUG
      sprintf(buf, "Writing %d change areas\n", splitLength);
      DEBUG(buf);
#endif

      for (int r=0; r<splitLength; ++r)
      {
         RECT rect = changes[r];

         dataOutR.write((short)rect.left);
         dataOutR.write((short)rect.top);
         dataOutR.write((short)(rect.right-rect.left+1));
         dataOutR.write((short)(rect.bottom-rect.top+1));
         dataOutR.write(compressedFormat);
         dataOutR.write((char)0); // (pre-)compression type
         
         zlib.compress(bufferR, dataOutR.countWrittenBytes());
         dataOutR.clear();
         
         
         // write pixel data in the rect area (or whole image for key frames)
         int bufferLength = (rect.right-rect.left+1)*outStride;
         
         
         if (motionFound && biWidth*biHeight < 401*300)
         {
            control = (unsigned char*)malloc(biWidth*biHeight*inStride + biHeight*inPadding);


            memcpy(control, prev, biWidth*biHeight*inStride + biHeight*inPadding);

         }
         
         // ???
         _ASSERT(control == NULL);


         unsigned char* buffer = (unsigned char*)malloc(bufferLength);
         
         OutputDataWrapper dataOutP(buffer, bufferLength);
         for (int y=rect.top; y<=rect.bottom; ++y)
         {
            long idx = (y*biWidth+rect.left)*inStride+y*inPadding;
            for (int x=rect.left; x<=rect.right; ++x)
            {
               if (outStride > 2)
               {
                  dataOutP.write(*(in+idx), *(in+idx+1), *(in+idx+2)); // BGR
                  
                  if (control)
                  {
                     *(control+idx) = *(in+idx);
                     *(control+idx+1) = *(in+idx+1);
                     // ???
                     *(control+idx+1) = *(in+idx+1);
                  }

               }
               else
                  dataOutP.write(*(in+idx+1), *(in+idx)); // intel byte order!
               
               idx += inStride;
            }
            zlib.compress(dataOutP.getData(), bufferLength);
            dataOutP.clear();
         }
         free(buffer);

      } // for all change rects

      free(changes);



      if (bWillWriteMouse) {
          //
          // The mouse position

          short x = m_rcLastMouseCompress.left;
          short y = m_rcLastMouseCompress.top;
          short w = m_rcLastMouseCompress.right - m_rcLastMouseCompress.left;
          short h = m_rcLastMouseCompress.bottom - m_rcLastMouseCompress.top;

          BYTE aBufferM[10];
          OutputDataWrapper outMouse(aBufferM, 10);
          outMouse.write(x);
          outMouse.write(y);
          outMouse.write(w);
          outMouse.write(h);

          outMouse.write((BYTE)(bMousePositionDifferent ? 0xff : 0), (BYTE)(bMouseShapeDifferent ? 0xff : 0));

          zlib.compress(outMouse.getData(), 10);


          if (bMouseShapeDifferent) {
              // The data itself
              int iCursorDataSize = w*h*4; // ARGB data

              BYTE *pData = (out + outLength - iCursorDataSize - 10);
              zlib.compress(pData, iCursorDataSize);
          }
      }

      
      if (bWillWriteClickData) {
          short *pClickCount= (short *)(pClickDataEnd - 4);
          int iClickCount = pClickCount[0]; // Data is also written in the array like this

          int iDataLength = iClickCount * 6 + 2;
          BYTE *aBuffer = new BYTE[iDataLength];
          OutputDataWrapper outClicks(aBuffer, iDataLength);

          outClicks.write((BYTE)CLICK_DATA_MARKER);
          outClicks.write((BYTE)iClickCount);


          BYTE *pData = pClickDataEnd - 4 - iClickCount * 6;
          for (int i=0; i<iClickCount; ++i) {
              short *pValues = (short *)pData;

              short x = pValues[0];
              short y = pValues[1];

              pData += 4;

              bool bIsLeftMouse = pData[0] == 0xff;
              int  iRadius = pData[1] & 0xff;

              pData += 2;

              outClicks.write(x);
              outClicks.write(y);
              outClicks.write(bIsLeftMouse ? (BYTE)0xff : (BYTE)0);
              outClicks.write((BYTE)iRadius);
          }


          zlib.compress(outClicks.getData(), outClicks.countWrittenBytes());

          delete[] aBuffer;
      }


      compressLength += zlib.finish();

#ifdef _DEBUG
      if (control)
      {
         RECT r;
         r.left = 0;
         r.top = 0;
         r.right = biWidth;
         r.bottom = biHeight;
         char fbuf[80];
         sprintf(fbuf, "c:\\image%d.ppm", icinfo->lFrameNum);
         //RawDiskImage::write(control, biWidth, biHeight, inStride, r, fbuf); 

         free(control);
      }
#endif

#ifdef _DEBUG
      double second4 = currentTime();
      
      char buf0[80];
      sprintf(buf0, "Compression time = %2.7fs\n", second4-second3);
      DEBUG(buf0);
#endif     
   }


   icinfo->lpbiOutput->biSizeImage = compressLength;

   if (m_lastCompressedImage == NULL ||
       m_lastCompressedImageSize != icinfo->lpbiInput->biSizeImage)
   {
      if (m_lastCompressedImage)
      {
         free(m_lastCompressedImage);
         m_lastCompressedImage = NULL;
      }

      m_lastCompressedImageSize = icinfo->lpbiInput->biSizeImage;
      m_lastCompressedImage = (char *) malloc(m_lastCompressedImageSize);
   }

   if (m_lastCompressedImage)
      memcpy(m_lastCompressedImage, icinfo->lpInput, m_lastCompressedImageSize);

   return ICERR_OK;
}


DWORD CodecInst::CompressEnd() {
   if (m_lastCompressedImage)
      free(m_lastCompressedImage);
   m_lastCompressedImage = NULL;
   m_lastCompressedImageSize = 0;
   return ICERR_OK;
}


/********************************************************************
********************************************************************/

static bool CanDecompress(LPBITMAPINFOHEADER lpbiIn) {
   int intype = GetBitmapType(lpbiIn);
   return intype < 0;
}


static bool CanDecompress(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut) {
   int intype = GetBitmapType(lpbiIn);
   int outtype2 = GetBitmapType(lpbiOut);
   
   if (!lpbiOut)
      return CanDecompress(lpbiIn);
   
   // must be 1:1 (no stretching)
   if (lpbiOut && (lpbiOut->biWidth != lpbiIn->biWidth || lpbiOut->biHeight != lpbiIn->biHeight))
   {
      //MessageBox(NULL, "no stretching", NULL, MB_OK);
      return false;
   }
   
   int outtype = GetBitmapType(lpbiOut);
   
   return (CanDecompress(lpbiIn) && outtype > 0);
}

/********************************************************************
********************************************************************/


DWORD CodecInst::DecompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut) {
#ifdef _DEBUG
   char buf[500];
   sprintf(buf, "DecompressQuery: input = %s, output = %s\n", &PrintBitmapType(lpbiIn), &PrintBitmapType(lpbiOut)); 
   DEBUG(buf);
#endif
   
   return CanDecompress(lpbiIn, lpbiOut) ? ICERR_OK : ICERR_BADFORMAT;
}


DWORD CodecInst::DecompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut) {
   // if lpbiOut == NULL, then return the size required to hold an output format struct
   if (lpbiOut == NULL)
      return sizeof(BITMAPINFOHEADER);
   
   if (!CanDecompress(lpbiIn))
      return ICERR_BADFORMAT;
 
   
   *lpbiOut = *lpbiIn;
   lpbiOut->biSize = sizeof(BITMAPINFOHEADER);
   lpbiOut->biPlanes = 1;
   lpbiOut->biBitCount = lpbiIn->biBitCount > 16 ? 32 : 16;
   // if you return 24 and the color depth is 32
   // Hypercam will crash in determineChangeArea 
   // upon compression due to lpbiPrev being of length 0
   lpbiOut->biCompression = BI_RGB;
   int padding = LMisc::calcPadding(lpbiOut->biWidth, lpbiOut->biBitCount/8);
   lpbiOut->biSizeImage = lpbiOut->biWidth * lpbiOut->biHeight *
      lpbiOut->biBitCount / 8 + lpbiOut->biHeight * padding;
 
#ifdef _DEBUG
   char buf[500];
   sprintf(buf, "DecompressGetFormat: input = %s, output = %s\n", 
      &PrintBitmapType(lpbiIn), &PrintBitmapType(lpbiOut)); 
   DEBUG(buf);
#endif
    
   return ICERR_OK;
}

DWORD CodecInst::DecompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut) {
  
   DecompressEnd();  // free resources if necessary

   if (!CanDecompress(lpbiIn, lpbiOut))
    return ICERR_BADFORMAT;

   m_bDecompressBeginCalled = true;
   m_iCurrentDecompressFrame = 0;

   return ICERR_OK;
}


DWORD CodecInst::Decompress(ICDECOMPRESS* icinfo, DWORD dwSize) {
   // If you insert a Lsgc-compressed AVI to a Premiere project and then
   // drag it on to the timeline, the following dialogue occurs:
   //
   // 1. Premiere calls ICDecompressBegin, asking Lsgc to decompress
   //    to a bitmap with different dimensions than the compressed frame.
   //
   // 2. Lsgc can't resize, so it returns ICERR_BADFORMAT.
   //
   // 3. Premiere calls ICDecompress without making another call to
   //    ICDecompressBegin.
   //
   // Therefore I now check for this case and compensate for Premiere's
   // negligence by making the DecompressBegin call myself.

    FILE *fileTimings = NULL;
    //fileTimings = fopen("c:\\time.txt", "a");

    ::timeBeginPeriod(1);

    DWORD dwTime1 = ::timeGetTime();

   
   if (!m_bDecompressBeginCalled) {
      DWORD retval = DecompressBegin(icinfo->lpbiInput, icinfo->lpbiOutput);
      if (retval != ICERR_OK)
         return retval;
   }
   
   if (!CanDecompress(icinfo->lpbiInput, icinfo->lpbiOutput))
      return ICERR_BADFORMAT;

   
   unsigned char* in = (unsigned char*)icinfo->lpInput;
   unsigned char* out = (unsigned char*)icinfo->lpOutput;

   unsigned long lengthIn = icinfo->lpbiInput->biSizeImage;

#ifdef _DEBUG
   char buf[500];
   sprintf(buf, "### Decompress: input = %s, output = %s ", 
      &PrintBitmapType(icinfo->lpbiInput), &PrintBitmapType(icinfo->lpbiOutput)); 
   DEBUG(buf);
   sprintf(buf, "%d inBit, %d outBit\n", 
      icinfo->lpbiInput->biBitCount, icinfo->lpbiOutput->biBitCount);
#endif

   //
   // read (uncompressed) header
   // 
   InputDataWrapper dataInH(in, lengthIn);
   int version = dataInH.readUChar();
   // the flag (icinfo->dwFlags & ICDECOMPRESS_NOTKEYFRAME) == 0
   // cannot be trusted, so we need the information of the frame header:
   char type = dataInH.readChar();
   unsigned short flags = dataInH.readShort();

   in += 4;
   lengthIn -= 4;

   bool containsMotion = (flags & CONTAINS_MOTION) != 0;
   
   _ASSERT(containsMotion == false);

   bool bContainsMouse = (flags & CONTAINS_MOUSE) != 0;
   bool bContainsClicks = (flags & CONTAINS_CLICKS) != 0;
   
   int outBitCount = icinfo->lpbiOutput->biBitCount;
   int outStride = outBitCount / 8;
   int iImageWidth = icinfo->lpbiOutput->biWidth;
   int iImageHeight = icinfo->lpbiOutput->biHeight;
   int outPadding = LMisc::calcPadding(iImageWidth, outStride);

#ifdef _DEBUG
   int origSize = icinfo->lpbiOutput->biSizeImage;
   sprintf(buf, "biSizeImage= %d\n", origSize);
   DEBUG(buf);
#endif

   icinfo->lpbiOutput->biSizeImage = iImageWidth*iImageHeight*outStride + iImageHeight*outPadding;

   // Only checks if variable is present/is normally true:
   bool bDrawPointerOrClick = GetEnvironmentVariable(_T("LSGC_NO_POINTER"), NULL, 0) <= 0;

  
   if (type != KEY_FRAME) {
      //unsigned char* prev = (unsigned char*)icinfo->lpPrev;
      // the above doesn't exits; no copying of last image data
      // possible/needed as it is already in the output image
      // if you do not specify VIDCF_FASTTEMPORALD
      // NOTE: Java/JMF makes something stupid (bug) with the last
      //       image data, so we do support/specify VIDCF_FASTTEMPORALD
      //       and handle an additional last image for decompression
       
      _ASSERT(m_pLastImage != NULL);

      if (m_pLastImage != NULL) {
         memcpy(out, m_pLastImage, icinfo->lpbiOutput->biSizeImage);
      }

   }

   // Will probably be overwritten below (will be used when identity frame)
   short sMouseX = m_ptLastMousePos.x;
   short sMouseY = m_ptLastMousePos.y;
   short sMouseW = m_ptLastMouseSize.x;
   short sMouseH = m_ptLastMouseSize.y;

   bool bMouseShapeDifferent = false;

   DWORD dwTime2 = dwTime1;
   DWORD dwTime3 = dwTime1;

   if (type != IDENTITY_FRAME) {

      
      InputZlibWrapper zlib(in, lengthIn);
        
      if (containsMotion) {
         RECT motionArea;
         POINT motionVector;

         unsigned char bufferM[12];

         zlib.decompress(bufferM, 12);

         InputDataWrapper dataInM(bufferM, 12);
         
         motionArea.left = dataInM.readShort();
         motionArea.top = dataInM.readShort();
         int motionWidth = dataInM.readShort();
         int motionHeight = dataInM.readShort();
         motionVector.x = dataInM.readShort();
         motionVector.y = dataInM.readShort();

         motionArea.right = motionArea.left+motionWidth-1;
         motionArea.bottom = motionArea.top+motionHeight-1;

         compensateMotion(out, iImageWidth, outStride, motionArea, motionVector);
      }

      // read change rectangles (for a key frame this will be the whole area)
      unsigned char bufferR[12];
      zlib.decompress(bufferR, 2);
      
      InputDataWrapper dataInR(bufferR, 2);
      int splitLength = dataInR.readShort(); // number of change rects; can be 0 (= at least mouse changed)

#ifdef _DEBUG
      if (type != KEY_FRAME)
      {
         sprintf(buf, "Decoding %d change rects.\n", splitLength);
         DEBUG(buf);
      }
#endif

      for (int r=0; r<splitLength; ++r)
      {
          zlib.decompress(bufferR, 10);
          dataInR.resetData(bufferR, 10);

         RECT rect;
         rect.left = dataInR.readShort();
         rect.top = dataInR.readShort();
         int rectWidth = dataInR.readShort();
         int rectHeight = dataInR.readShort();
         char format = dataInR.readChar(); // compressed format type (ie color depth)
         dataInR.readChar(); // (pre-)compression type
         
         rect.right = rect.left+rectWidth-1;
         rect.bottom = rect.top+rectHeight-1;
#ifdef _DEBUG
      if (type != KEY_FRAME)
      {
         sprintf(buf, "  LSGC change rect Nr.%i : %i,%i - %i x %i\n", r+1, rect.left,rect.top,rectWidth, rectHeight);
         DEBUG(buf);
      }
#endif
         
         
         
         // read pixel data line-wise
         int inStride = (format == BM_COMPRESSED_RGB24) ? 3 : 2;
         int inBufferLength = rectWidth*inStride;
         int outBufferLength = inBufferLength;
         // a stretch is needed if either in- or output is 16-Bit and both differ
         bool stretchNeeded = (outStride < 3 || inStride < 3) && outStride != inStride;
         
         unsigned char* inBuffer = (unsigned char*)malloc(inBufferLength);
         unsigned char* outBuffer = inBuffer;
         if (stretchNeeded)
         {
            // padding doesn't need to be taken into accout for this 
            // working inBuffer's length as data is processed linewise
            outBufferLength = rectWidth*min(3, outStride);
            outBuffer = (unsigned char*)malloc(outBufferLength);
         }
         
         InputDataWrapper dataInP(NULL, 0);
         for (int y=rect.top; y<=rect.bottom; ++y)
         {
            zlib.decompress(inBuffer, inBufferLength);
            if (stretchNeeded)
               stretchPixelLine(inBuffer, outBuffer, rectWidth, inStride, min(3, outStride));
            dataInP.resetData(outBuffer, outBufferLength);
            
            int offset = (y*iImageWidth + rect.left)*outStride + y*outPadding;
            for (int x=rect.left; x<=rect.right; ++x)
            {
               // Note: read() here means: reading from outBuffer and writing to out
               // (that is two steps in one: reading from the input array and writing to the output)

               if (outStride > 2)
                  dataInP.read(out+offset, out+offset+1, out+offset+2); // BGR
               else
                  dataInP.read(out+offset+1, out+offset); // intel byte order!!
               offset += outStride;
            }

            // according to the documentation padding bytes should be 0
            /*
            if (outPadding > 0) 
            {
               // (y + 1)*iImageWidth means the next line
               // but y*outPadding is a bit shorter than needed to get to the next line
               offset = ((y + 1)*iImageWidth)*outStride + y*outPadding;
               for (int p=0; p<outPadding; ++p)
                  *(out+offset+p) = (char)0;

            }
            //*/
         }

         
         free(inBuffer);
         if (stretchNeeded)
            free(outBuffer);
      }

      // Save the last image before painting the mouse over it
       if (m_pLastImage == NULL)
          m_pLastImage = (char*)malloc(icinfo->lpbiOutput->biSizeImage);

       memcpy(m_pLastImage, out, icinfo->lpbiOutput->biSizeImage);

       if (bContainsMouse) {
           BYTE aBufferM[10];
           zlib.decompress(aBufferM, 10);

           InputDataWrapper dataInM(aBufferM, 10);
           sMouseX = dataInM.readShort();
           sMouseY = dataInM.readShort();
           sMouseW = dataInM.readShort();
           sMouseH = dataInM.readShort();
           bool bDummyMousePosDifferent = dataInM.readUChar() == 0xff;
           bMouseShapeDifferent = dataInM.readUChar() == 0xff;

           m_ptLastMousePos.x = sMouseX;
           m_ptLastMousePos.y = sMouseY;
           m_ptLastMouseSize.x = sMouseW;
           m_ptLastMouseSize.y = sMouseH;

           if (bMouseShapeDifferent) {
               int iCursorDataSize = sMouseW*sMouseH*4; // BGRA data

               if (m_pCurrentMouseData == NULL || iCursorDataSize != m_iCurrentMouseLength) {
                   if (m_pCurrentMouseData != NULL)
                       delete[] m_pCurrentMouseData;

                   m_pCurrentMouseData = new BYTE[iCursorDataSize];
                   m_iCurrentMouseLength = iCursorDataSize;
               }

               zlib.decompress(m_pCurrentMouseData, iCursorDataSize);
           }
       }

       dwTime2 = ::timeGetTime();

       if (bContainsClicks) {
           // This click data is always frame specific: no references to
           // other frames (like with pointers or image data) is present.

           BYTE aBufferH[2];
           zlib.decompress(aBufferH, 2);

           InputDataWrapper dataInH(aBufferH, 2);

           BYTE id = dataInH.readUChar();

           _ASSERT(id == CLICK_DATA_MARKER);

           if (id == CLICK_DATA_MARKER) {
               int iClickCount = dataInH.readUChar();

               if (iClickCount > 0) {
                   int iDataLength = iClickCount * 6;

                   BYTE *aBufferData = new BYTE[iDataLength];
                   zlib.decompress(aBufferData, iDataLength);

                   InputDataWrapper dataInC(aBufferData, iDataLength);
                   for (int i=0; i<iClickCount; ++i) {
                       short x = dataInC.readShort();
                       short y = dataInC.readShort();
                       bool bIsLeftMouse = dataInC.readUChar() == (BYTE)0xff;
                       int iRadius = dataInC.readUChar();

                       if (bDrawPointerOrClick) {
                           POINT ptClickImageSize;
                           BYTE *pClickImageData = CreateClickImage(iRadius, 
                               bIsLeftMouse ? RGB(255, 0, 0) : RGB(0, 0, 255), &ptClickImageSize);
                           // TODO this image data could and should be cached
                           // (only 10 small images or so). However creation is quite fast...

                           if (pClickImageData != NULL) {
                               PlaceImageInArray(pClickImageData, ptClickImageSize.x, ptClickImageSize.y, 
                                   out, outStride, iImageWidth, iImageHeight, 
                                   x - ptClickImageSize.x/2, y - ptClickImageSize.y/2);

                               free(pClickImageData);
                           }
                       }
                   }

                   delete[] aBufferData;

               }
           } 
       }
       dwTime3 = ::timeGetTime();

   } // ! IDENTITY_FRAME
 
   DWORD dwTime4 = ::timeGetTime();

   if (bDrawPointerOrClick && m_pCurrentMouseData != NULL) {

       // m_pCurrentMouseData != NULL: odd case with cut&paste in DES:
       // It is partly called with a key frame (probably after a DecompressEnd())
       // which has bMouseShapeDifferent == false.


       int iCursorDataSize = sMouseW*sMouseH*4; // BGRA data

       // Make sure value is in some meaningful range
       _ASSERT(iCursorDataSize >= 0 && iCursorDataSize < 4000000);

       //
       // Painting the mouse on the current output image
       // Currently this is a direct pixel copy/setting.

       PlaceImageInArray(m_pCurrentMouseData, sMouseW, sMouseH, 
           out, outStride, iImageWidth, iImageHeight, sMouseX, sMouseY);


       /* Disabled: Not used at the moment and takes lots of time

       // The same specification is used when grabbing/transfering the cursor
       BITMAPINFO bitmapInfoCursor;
       ZeroMemory(&bitmapInfoCursor, sizeof BITMAPINFO);
       bitmapInfoCursor.bmiHeader.biSize = sizeof BITMAPINFOHEADER;
       bitmapInfoCursor.bmiHeader.biBitCount = 32;
       bitmapInfoCursor.bmiHeader.biWidth = sMouseW;
       bitmapInfoCursor.bmiHeader.biHeight = sMouseH;
       bitmapInfoCursor.bmiHeader.biCompression = BI_RGB;
       bitmapInfoCursor.bmiHeader.biPlanes = 1;
       bitmapInfoCursor.bmiHeader.biSizeImage = sMouseW*sMouseH*4; // normally you could use DIBSIZE()


       HDC hImageDc = ::CreateCompatibleDC(NULL);
       ::SetStretchBltMode(hImageDc, HALFTONE);

       BYTE *pData = NULL;
       BITMAPINFO bitmapInfoOutput;
       ZeroMemory(&bitmapInfoOutput, sizeof BITMAPINFO);
       bitmapInfoOutput.bmiHeader = *icinfo->lpbiOutput;


       HBITMAP hImage = CreateDIBSection(hImageDc, &bitmapInfoOutput, DIB_RGB_COLORS, (PVOID *) &pData, NULL, 0);

       if (hImageDc != NULL && hImage != NULL) {
           ::SelectObject(hImageDc, hImage);

           dwTime3 = ::timeGetTime();

           // Paint the current output data on the intermediate image
           ::StretchDIBits(hImageDc, 0, 0, iImageWidth, iImageHeight, 
               0, 0, iImageWidth, iImageHeight, out, &bitmapInfoOutput, DIB_RGB_COLORS, SRCCOPY);

           HPEN hPen = ::CreatePen(PS_SOLID, 3, RGB(0, 0, 255));

           //::SelectObject(hImageDc, hPen);
           //::Ellipse(hImageDc, sMouseX-2, sMouseY-2, sMouseX+2, sMouseY+2);

           memcpy(out, pData, icinfo->lpbiOutput->biSizeImage);

           ::DeleteObject(hPen);
           ::DeleteObject(hImage);
           ::DeleteDC(hImageDc);
       }
       */

   }

   DWORD dwTime5 = ::timeGetTime();

   if (fileTimings != NULL) {
       char szMsg[200];
       sprintf_s(szMsg, 200, "%d %d %d %d = %d\n",
           dwTime2-dwTime1, dwTime3-dwTime2, dwTime4-dwTime3, dwTime5-dwTime4, dwTime5-dwTime1);
       fwrite(szMsg, 1, strlen(szMsg), fileTimings);
   }

   if (fileTimings != NULL)
       fclose(fileTimings);

   // However this is only an increasing counter after last call to DecompressBegin()
   ++m_iCurrentDecompressFrame;

   return ICERR_OK;
}


// palette-mapped output only
DWORD CodecInst::DecompressGetPalette(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut) {
   return ICERR_BADFORMAT;
}


DWORD CodecInst::DecompressEnd() {
   m_bDecompressBeginCalled = false;

   if (m_pLastImage != NULL) {
      free(m_pLastImage);
      m_pLastImage = NULL;
   }

   if (m_pCurrentMouseData != NULL) {
       delete[] m_pCurrentMouseData;
       m_pCurrentMouseData = NULL;
       m_iCurrentMouseLength = 0;
   }

   return ICERR_OK;
}


DWORD CodecInst::SetStructuringCallbackProc(ICSETSTATUSPROC *icsetstatusProc, DWORD cbSize) {
   m_pStructuringCallback = icsetstatusProc->Status;

   return ICERR_OK;
}


void CodecInst::PlaceImageInArray(BYTE *pImage, int iImageWidth, int iImageHeight,
                                  BYTE *pArray, int iOutStride, int iArrayWidth, int iArrayHeight,
                                  int iOffsetX, int iOffsetY) { 

    int iOutPadding = LMisc::calcPadding(iArrayWidth, iOutStride);

    for (int ry=0; ry<iImageHeight; ++ry) {
        for (int rx=0; rx<iImageWidth; ++rx) {

            int iIdxInData = 4*(ry*iImageWidth + rx);

            bool bTransparent = pImage[iIdxInData+3] != 0xff;
            int iInArrayX = iOffsetX + rx;
            int iInArrayY = iOffsetY + ry;
            if (iArrayHeight > 0)
                iInArrayY = iArrayHeight - iInArrayY - 1; // flip position

            if (iInArrayX >= 0 && iInArrayX < iArrayWidth && iInArrayY >= 0 && iInArrayY < iArrayHeight) {

                int iArrayOffset = (iInArrayY*iArrayWidth + iInArrayX)*iOutStride + iInArrayY*iOutPadding;

                // This copies one pixel:
                if (!bTransparent) {
                    stretchPixelLine(pImage + iIdxInData, pArray + iArrayOffset, 1, 3, min(3, iOutStride));
                }
            }
        }
    }
}

BYTE* CodecInst::CreateClickImage(int iRadius, COLORREF clr, POINT *pptImageSize) {

    BYTE *pDataForReturn = NULL;

    if (iRadius > 0) {
        int iImageWidth = iRadius * 2;
        int iImageHeight = iRadius * 2;

        BITMAPINFO bitmapInfo;
        ZeroMemory(&bitmapInfo, sizeof BITMAPINFO);
        bitmapInfo.bmiHeader.biSize = sizeof BITMAPINFOHEADER;
        bitmapInfo.bmiHeader.biBitCount = 32;
        bitmapInfo.bmiHeader.biWidth = iImageWidth;
        bitmapInfo.bmiHeader.biHeight = iImageHeight;
        bitmapInfo.bmiHeader.biCompression = BI_RGB;
        bitmapInfo.bmiHeader.biPlanes = 1;
        bitmapInfo.bmiHeader.biSizeImage = iImageWidth*iImageHeight*4; // normally you could use DIBSIZE()

        HDC hImageDc = ::CreateCompatibleDC(NULL);
        ::SetStretchBltMode(hImageDc, HALFTONE);

        BYTE *pData = NULL;
        HBITMAP hImage = ::CreateDIBSection(hImageDc, &bitmapInfo, DIB_RGB_COLORS, (PVOID *) &pData, NULL, 0);
        memset(pData, 0xff, bitmapInfo.bmiHeader.biSizeImage); // all non-transparent; see below


        if (hImageDc != NULL && hImage != NULL) {
            ::SelectObject(hImageDc, hImage);
            HBRUSH hNullBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
            ::SelectObject(hImageDc, hNullBrush); // Otherwise circles are filled

            HPEN hPen = ::CreatePen(PS_SOLID, 1, clr);

            ::SelectObject(hImageDc, hPen);
            ::Ellipse(hImageDc, 0, 0, iImageWidth, iImageHeight);

            pDataForReturn = (BYTE *)malloc(bitmapInfo.bmiHeader.biSizeImage);
            memcpy(pDataForReturn, pData, bitmapInfo.bmiHeader.biSizeImage);

            // TODO
            // The image created is not as expected: drawn parts are made transparent (and nothing more)
            // 
            for (int i=0; i<bitmapInfo.bmiHeader.biSizeImage; i+=4) {
                bool bTransparent = pDataForReturn[i+3] != 0xff;

                if (bTransparent)  {
                    // make it non-transparent and set the color
                    pDataForReturn[i] = ((clr >> 16) & 0xff);
                    pDataForReturn[i+1] = ((clr >> 8) & 0xff);
                    pDataForReturn[i+2] = ((clr) & 0xff);

                    pDataForReturn[i+3] = 0xff;

                } else {
                    // make it transparent
                    pDataForReturn[i+3] = 0;
                }
            }

            if (pptImageSize != NULL) {
                pptImageSize->x = iImageWidth;
                pptImageSize->y = iImageWidth;
            }

            ::DeleteObject(hNullBrush);
            ::DeleteObject(hPen);
            ::DeleteObject(hImage);
            ::DeleteDC(hImageDc);
        }
    }

    return pDataForReturn;
}

/***********************************************************************
***********************************************************************/

// moved to lutils (lmisc.h/.cpp)
/*
struct BGR
{
   unsigned char b;
   unsigned char g;
   unsigned char r;
};

bool operator== (BGR c1, BGR c2)
{
   return c1.b == c2.b && c1.g == c2.g && c1.r == c2.r;
}

bool operator!= (BGR c1, BGR c2)
{
   return c1.b != c2.b || c1.g != c2.g || c1.r != c2.r;
}
*/

// moved to lutils (lmisc.h/.cpp)
/*
// PZI: return value changed from bool to int - will return number of changed pixels now
//
// [in]  bm1, bm2: bitmaps of pixeldata
// [in]  width, height: dimensions of bitmaps
// [in]  stride: bytes per pixel, e.g. 2 for 16 bit or 3 for 24 bit
// [in]  result: rectangle describing in which area pixel changes should be determined
// [out] result: outer bounds of change area
// [return] number of changed pixels
// note: RECT* result is used for input AND output
// note: rect.right, rect.bottom is included (may be confused with width and height where -1 must be applied)
//       e.g. rect (0,0,1023,767) and width x height 1024x768
int CodecInst::determineChangeArea(unsigned char* bm1, unsigned char* bm2, 
                                    int width, int height, int stride, RECT* result)
{
   // count number of changed pixels
   int count = 0;
   int minX = width, maxX = 0, minY = height, maxY = 0;

   int padding = calcPadding(width, stride);

   for (int y=result->top; y<=result->bottom; ++y)
   {
      bool changeInLineFound = false;
      bool pixelChanged;
      
      int offset = (y*width + result->left)*stride + y*padding;
      for (int x=result->left; x<=result->right; ++x)
      {
         pixelChanged = false;
         if (stride == 2)
         {
            if ((*((short *) (bm1+offset))) != (*((short *) (bm2+offset))))
               pixelChanged = true;
         }
         else
         {
            if (*((BGR*)(bm1+offset)) != *((BGR*)(bm2+offset)))
               pixelChanged = true;
         }

         if (pixelChanged)
         {
            // count number of changed pixels
            count++;
            if (x < minX)
               minX = x;
            if (x > maxX)
               maxX = x;
            changeInLineFound = true;
         }

         offset += stride;
      }
      
      if (changeInLineFound)
      {
         if (y < minY)
            minY = y;
         if (y > maxY)
            maxY = y;
         
      }
   
   }

   
   if (minX <= maxX)
   {
      result->left = minX;
      result->top = minY;
      result->right = maxX;
      result->bottom = maxY;
   }

   // return number of changed pixels
   return count;

   //   return true;
   //}
   //else
   //   return false;

*/
/*
   if (stride > 2)
   {
      //bm1 += result->left*stride;
      //bm2 += result->left*stride;

      // double code below
      for (int y=result->top; y<=result->bottom; ++y)
      {
         bool changeFound = false;
         
         int offset = (y*width+result->left)*stride+y*padding;
         for (int x=result->left; x<=result->right; ++x)
         {
            //if (*(bm1) != *(bm2) || *(bm1+1) != *(bm2+1) || *(bm1+2) != *(bm2+2))
            //if (*(bm1+offset) != *(bm2+offset) || *(bm1+offset+1) != *(bm2+offset+1) || *(bm1+offset+2) != *(bm2+offset+2))
            if (*((BGR*)(bm1+offset)) != *((BGR*)(bm2+offset)))
            {
               if (x < minX)
                  minX = x;
               if (x > maxX)
                  maxX = x;
               changeFound = true;
            }
            
            //bm1 += stride;
            //bm2 += stride;
            offset += stride;
         }
         
         if (changeFound)
         {
            if (y < minY)
               minY = y;
            if (y > maxY)
               maxY = y;
            
         }
         
         //bm1 += (width-result->right-1)*stride + result->left*stride;
         //bm2 += (width-result->right-1)*stride + result->left*stride;

         
      }
   }
   else
   {
      bm1 += result->left*stride;
      bm2 += result->left*stride;

      // double code above
      for (int y=result->top; y<=result->bottom; ++y)
      {
         bool changeFound = false;
         
         for (int x=result->left; x<=result->right; ++x)
         {
            if (*(bm1) != *(bm2) || *(bm1+1) != *(bm2+1))
            {
               if (x < minX)
                  minX = x;
               if (x > maxX)
                  maxX = x;
               changeFound = true;
            }
            
            bm1 += stride;
            bm2 += stride;
         }
         
         if (changeFound)
         {
            if (y < minY)
               minY = y;
            if (y > maxY)
               maxY = y;
            
         }
         
         bm1 += (width-result->right-1)*stride + result->left*stride + padding;
         bm2 += (width-result->right-1)*stride + result->left*stride + padding;

      }
   }
   //*/
//}


// untest/unreviewed!
int CodecInst::matchingCount(unsigned char* bm1, unsigned char* bm2,
      int width, int stride, RECT &rect1, RECT &rect2, bool fullMatchNeeded)
{
   int diffX = rect2.left - rect1.left;
   int diffY = rect2.top - rect1.top;

   int padding = LMisc::calcPadding(width, stride);
 
   int matchCounter = 0;

   for (int y=rect1.top; y<=rect1.bottom; ++y)
   {
      int offset1 = (y*width+rect1.left)*stride+y*padding;
      int offset2 = ((y+diffY)*width+rect2.left)*stride+(y+diffY)*padding;

      for (int x=rect1.left; x<=rect1.right; ++x)
      {
         if (stride > 2)
         {
            BGR24 c1(bm1+offset1);
            BGR24 c2(bm2+offset2);

            if (c1.getColor() == c2.getColor())
               ++matchCounter;
            else if (fullMatchNeeded)
               return 0;
         }
         else
         {
            RGB15 c1(bm1+offset1);
            RGB15 c2(bm2+offset2);
                  
            if (c1.getColor() == c2.getColor())
               ++matchCounter;
            else if (fullMatchNeeded)
               return 0;
         }

         offset1 += stride;
         offset2 += stride;
      }
   }

   return matchCounter;

}

// moved to lutils (lmisc.h/.cpp)
/*
void CodecInst::splitChangeArea(unsigned char* bm1, unsigned char* bm2, 
                                int width, int height, int stride, RECT bounds, 
                                RECT *areas, int *count)
{
   int SPLIT_SIZE = 30;


   int boundsWidth = bounds.right-bounds.left+1;
   int boundsHeight = bounds.bottom-bounds.top+1;

   if (boundsWidth > SPLIT_SIZE && boundsHeight > SPLIT_SIZE && 
      (boundsWidth > 2*SPLIT_SIZE || boundsHeight > 2*SPLIT_SIZE))
   {

      //
      // determine appropriate number of splits (below a maximum)
      //
      int countW = boundsWidth / SPLIT_SIZE;
      int countH = boundsHeight / SPLIT_SIZE;


      //char euf[200];
      
      //sprintf(euf, "Bounds are = %d,%d %dx%d; Split Counts = %d, %d\n",
      //   bounds.left, bounds.top, boundsWidth, boundsHeight, countW, countH);
      //DEBUG(euf);

      if (countW*countH > *count)
      {
         SPLIT_SIZE = ceil(sqrt((boundsWidth*boundsHeight)/(double)*count));

         countW = boundsWidth / SPLIT_SIZE;
         countH = boundsHeight / SPLIT_SIZE;
      }

      //sprintf(euf, "New Counts = %d, %d; New Split Size = %d\n", countW, countH, SPLIT_SIZE);
      //DEBUG(euf);

      int normW = boundsWidth / countW;
      int endW = boundsWidth % countW;

      int normH = boundsHeight / countH;
      int endH = boundsHeight % countH;

      std::vector<RECT> changeVector;
      changeVector.reserve(40);

      for (int y=0; y<countH; ++y)
      {
         int idx = y*countW;
         for (int x=0; x<countW; ++x)
         {
            RECT sample;
            sample.left = bounds.left+x*normW;
            sample.top = bounds.top+y*normH;

            sample.right = sample.left+normW-1;
            sample.bottom = sample.top+normH-1;

            if (x == countW-1 && endW > 0)
               sample.right += endW;

            if (y == countH-1 && endH > 0)
               sample.bottom += endH;

            //sprintf(euf, "1 Split area: %d,%d %dx%d\n", sample.left, sample.top,
            //   w(sample), h(sample));
            //DEBUG(euf);
            

            bool changed = false;
            if (determineChangeArea(bm1, bm2, width, height, stride, &sample)>0) //PZI: return value changed from bool to int
            {
               changeVector.push_back(sample);
               changed = true;
            }
            
            //sprintf(euf, "2 %d Split area: %d,%d %dx%d\n", changed, sample.left, sample.top,
            //   w(sample), h(sample));
            //DEBUG(euf);


            ++idx;
         } // for countW
      } // for countH

      std::vector<RECT>::iterator iter;
      //for (int i=0; i<changeVector.size(); ++i)
      //   areas[i] = changeVector[i];
      int i=0;
      for (iter = changeVector.begin(); iter != changeVector.end(); ++iter)
         areas[i++] = *iter;
      *count = changeVector.size();
   }
   else // change area (bounds) is not big enough to be split
   {
      areas[0] = bounds;
      *count = 1;
   }

}
*/


void CodecInst::stretchPixelLine(unsigned char* inBuffer, unsigned char* outBuffer, 
                                 int pixelCount, int inStride, int outStride)
{
   InputDataWrapper inData(inBuffer, pixelCount*inStride);
   OutputDataWrapper outData(outBuffer, pixelCount*outStride);

   if (inStride == 2 && outStride > 2) // enlarge: 16 -> 24,32
   {
      
      for (int i=0; i<pixelCount; ++i)
      {
         short rgb555 = inData.readShort(); // is in network byte order
         //*
         outData.write((char)(((rgb555 & 0x001f) << 3) | 0x07)); // b
         outData.write((char)(((rgb555 & 0x03e0) >> 2) | 0x07)); // g
         outData.write((char)(((rgb555 & 0x7c00) >> 7) | 0x07)); // r
         if (outStride > 3)
            outData.write((char)0);
         //*/
         
      }
   }
   else if (inStride > 2 && outStride == 2) // shrink: 24,32 -> 16
   {
      for (int i=0; i<pixelCount; ++i)
      {
         char b = inData.readChar();
         char g = inData.readChar();
         char r = inData.readChar();
         if (inStride > 3)
            inData.readChar();
         
         short rgb555 = (short)((r & 0xf8) << 7 | (g & 0xf8) << 2 | (b & 0xf8) >> 3);

         outData.write(rgb555);
      }
   }
   else if (inStride == outStride)
   {
      memcpy(outBuffer, inBuffer, pixelCount * inStride);
   }
}





int midletRand(int start, int end)
{
   int ra = rand() - (RAND_MAX+1)/2;
   
   if (abs(ra) > rand()/2)
   {
      if (ra > 0)
         ra -= rand()/4;
      else
         ra += rand()/4;
   }

   ra += (RAND_MAX+1)/2;

   int width = end - start + 1;

   return start+(int)((ra/(double)RAND_MAX)*width);
}

// untested/unreviewed!

// NOTE! The usage of bounds.right is wrong: it actually (normally) denotes the pixel
//       on right to the last pixel. So width=right-left.

bool CodecInst::findMotion(unsigned char* bm1, unsigned char* bm2, 
                int width, int height, int stride, RECT bounds, POINT* vector, RECT* area)
{
   int padding = LMisc::calcPadding(width, stride);
 
   int boundsWidth = bounds.right-bounds.left+1;
   int boundsHeight = bounds.bottom-bounds.top+1;
   int boundsSize = boundsWidth*boundsHeight;

   int sampleCount = 3+boundsSize/3000;
   if (sampleCount > 30)
      sampleCount = 30;

   int sampleSize = 16;

   srand(GetTickCount());
   std::vector<RECT> sampleRects(120);

   /*
   char suf[120];
   sprintf(suf, "Change area: %d,%d,%d,%d\n", bounds.left, bounds.top,
         bounds.right, bounds.bottom);
   DEBUG(suf);
   //*/

   int totalCompareBoundary = boundsWidth*boundsHeight*7;
   int totalCompareCounter = 0;

   for (int s=0; s<sampleCount; ++s)
   {
      sampleRects[s].left = midletRand(bounds.left, bounds.right-sampleSize);//bounds.left+(int)((rand()/(double)RAND_MAX)*(boundsWidth-sampleSize));  
      sampleRects[s].top = midletRand(bounds.top, bounds.bottom-sampleSize);//bounds.top+midletRand(0, (int)((rand()/(double)RAND_MAX)*(boundsHeight-sampleSize));
      sampleRects[s].right = sampleRects[s].left+sampleSize-1;
      sampleRects[s].bottom = sampleRects[s].top+sampleSize-1;

      int sampleCompareBoundary = totalCompareBoundary / 5;
      int sampleCompareCounter = 0;

      //
      // gather color statistics data
      // and find rarest color in sample (its top-left-most occurence)
      // 
      Palette pal((16*16*3)/2);
      // double code (below)
      for (int y=sampleRects[s].top; y<=sampleRects[s].bottom; ++y)
      {
         long offset = (y*width+sampleRects[s].left)*stride+y*padding;
         for (int x=sampleRects[s].left; x<=sampleRects[s].right; ++x)
         {
            if (stride > 2)
            {
               BGR24 c(bm2+offset);  // hmpf!
               pal.addPixel(&c);
            }
            else
            {
               RGB15 c(bm2+offset); // hmpf!
               pal.addPixel(&c);
            }

            offset += stride;
         }
      }


      if (pal.countDifferentColors() > 4) // is significant pattern
      {
         int rarestColor = pal.getRarestColor();
         int posX = -1;
         int posY = -1;
         int y = 0;
         // double code (above)
         for (y=sampleRects[s].top; y<=sampleRects[s].bottom; ++y)
         {
            bool found = false;
            long offset = (y*width+sampleRects[s].left)*stride+y*padding;
            for (int x=sampleRects[s].left; x<=sampleRects[s].right; ++x)
            {
               if (stride > 2)
               {
                  BGR24 c(bm2+offset);  // hmpf!
                  
                  if (c.getColor() == rarestColor)
                  {
                     posX = x;
                     posY = y;
                     found = true;
                     break;
                  }
               }
               else
               {
                  RGB15 c(bm2+offset); // hmpf!
                  
                  if (c.getColor() == rarestColor)
                  {
                     posX = x;
                     posY = y;
                     found = true;
                     break;
                  }
               }
               
               offset += stride;
               
            }
            
            if (found)
               break;
         }
         
         
         /*
         char buf8[160];
         sprintf(buf8, "Number of colors in sample = %d\n", pal.countDifferentColors());
         DEBUG(buf8);
         
         sprintf(buf8, "With an area of = %d,%d,%d,%d\n", sampleRects[s].left, sampleRects[s].top,
            sampleRects[s].right, sampleRects[s].bottom);
         DEBUG(buf8);
         
         sprintf(buf8, "Rarest color position = %d,%d\n", posX, posY);
         DEBUG(buf8);
         //*/


         //
         // go through bm1's change area and look for this rarest color
         // 
         
         posX -= sampleRects[s].left;
         posY -= sampleRects[s].top;
         int startX = bounds.left+posX, endX = bounds.right-(sampleSize-posX);
         int startY = bounds.top+posY, endY = bounds.bottom-(sampleSize-posY);
         for (y=startY; y<=endY; ++y)
         {
            int offset = (y*width+startX)*stride+y*padding;
            for (int x=startX; x<=endX; ++x)
            { 
               
               if (stride > 2)
               {
                  BGR24 c(bm1+offset);
                  
                  if (c.getColor() == rarestColor)
                  {
                     RECT rect1;
                     rect1.left = x-posX; rect1.right = rect1.left+sampleSize-1;
                     rect1.top = y-posY; rect1.bottom = rect1.top+sampleSize-1;

                     RECT rect2;
                     rect2.left = sampleRects[s].left; rect2.right = rect2.left+sampleSize-1;
                     rect2.top = sampleRects[s].top; rect2.bottom = rect2.top+sampleSize-1;

                     int count = matchingCount(bm1, bm2, width, stride, rect1, rect2, true);
                     totalCompareCounter += sampleSize*sampleSize;
                     sampleCompareCounter += sampleSize*sampleSize;

                     if (count == sampleSize*sampleSize)
                     {

                        //
                        // calculate candidate motion vector
                        // 
                        int motionX = rect2.left-rect1.left;
                        int motionY = rect2.top-rect1.top;

                        rect1.left = bounds.left;
                        rect1.top = bounds.top;
                        rect2.left = bounds.left;
                        rect2.top = bounds.top;

                        int areaWidth = boundsWidth - abs(motionX);
                        int areaHeight = boundsHeight - abs(motionY);

                        if (motionX > 0)
                           rect2.left += motionX;
                        else
                           rect1.left -= motionX;

                        if (motionY > 0)
                           rect2.top += motionY;
                        else
                           rect1.top -= motionY;
                        
                        rect1.right = rect1.left+areaWidth-1;
                        rect1.bottom = rect1.top+areaHeight-1;
                        rect2.right = rect2.left+areaWidth-1;
                        rect2.bottom = rect2.top+areaHeight-1;

                        int otherCount = 0;
                        int maximumCount = areaWidth*areaHeight;
                        if (motionX != 0 || motionY != 0)
                        {
                           otherCount = matchingCount(bm1, bm2, width, stride, rect1, rect2, false);
                           
                           totalCompareCounter += areaWidth*areaHeight;
                           sampleCompareCounter += areaWidth*areaHeight;
                           
                           
                           //*
#ifdef _DEBUG
                           char buf[240];
                           sprintf(buf, "Candidate Motion is = %d,%d with #hits = %d (%2.3f)\n", 
                              motionX, motionY, otherCount, otherCount*100/(float)maximumCount);
                           DEBUG(buf);
#endif
                           /*
                           sprintf(buf, "That is a percentage of = %f\n", otherCount*100/(float)maximumCount);
                           DEBUG(buf);
                           //sprintf(buf, "Compare Area 1 = %d,%d,%d,%d * Compare Area 2 = %d,%d,%d,%d\n",
                           //   rect1.left, rect1.top, rect1.right, rect1.bottom,
                           //   rect2.left, rect2.top, rect2.right, rect2.bottom);
                           //DEBUG(buf);
                           //*/
                        }

                        if (otherCount > 0.94*maximumCount && (motionX != 0 || motionY != 0))
                        {
                           vector->x = motionX;
                           vector->y = motionY;

                           area->left = rect1.left;
                           area->top = rect1.top;
                           area->right = rect1.right;
                           area->bottom = rect1.bottom;


                           return true;
                        } 
                        
                     }
                     
                     
                  }
               }
               else
               {
                  RGB15 c(bm1+offset);
                  
                  if (c.getColor() == rarestColor)
                  {
                     RECT rect1;
                     rect1.left = x-posX; rect1.right = rect1.left+sampleSize-1;
                     rect1.top = y-posY; rect1.bottom = rect1.top+sampleSize-1;

                     RECT rect2;
                     rect2.left = sampleRects[s].left; rect2.right = rect2.left+sampleSize-1;
                     rect2.top = sampleRects[s].top; rect2.bottom = rect2.top+sampleSize-1;

                     int count = matchingCount(bm1, bm2, width, stride, rect1, rect2, true);

                     if (count == sampleSize*sampleSize)
                     {

                        //
                        // calculate candidate motion vector
                        // 
                        int motionX = rect2.left-rect1.left;
                        int motionY = rect2.top-rect1.top;

                        
                        rect1.left = bounds.left;
                        rect1.top = bounds.top;
                        rect2.left = bounds.left;
                        rect2.top = bounds.top;

                        int areaWidth = boundsWidth - abs(motionX);
                        int areaHeight = boundsHeight - abs(motionY);

                        if (motionX > 0)
                           rect2.left += motionX;
                        else
                           rect1.left -= motionX;

                        if (motionY > 0)
                           rect2.top += motionY;
                        else
                           rect1.top -= motionY;
                        
                        rect1.right = rect1.left+areaWidth-1;
                        rect1.bottom = rect1.top+areaHeight-1;
                        rect2.right = rect2.left+areaWidth-1;
                        rect2.bottom = rect2.top+areaHeight-1;

                        int otherCount = matchingCount(bm1, bm2, width, stride, rect1, rect2, false);
                        int maximumCount = areaWidth*areaHeight;

                        totalCompareCounter += areaWidth*areaHeight;
                        sampleCompareCounter += areaWidth*areaHeight;

#ifdef _DEBUG
                        char buf[240];
                        sprintf(buf, "Candidate Motion is = %d,%d with #hits = %d\n", motionX, motionY, otherCount);
                        DEBUG(buf);
                        sprintf(buf, "That is a percentage of = %f\n", otherCount*100/(float)maximumCount);
                        DEBUG(buf);
#endif

                        
                        if (otherCount > 0.94*maximumCount)
                        {
                           vector->x = motionX;
                           vector->y = motionY;

                           area->left = rect1.left;
                           area->top = rect1.top;
                           area->right = rect1.right;
                           area->bottom = rect1.bottom;


                           return true;
                        }

                     }
                  }
               }
               
               
               if (totalCompareCounter > totalCompareBoundary)
                  return false;
                  
               
               if (sampleCompareCounter > sampleCompareBoundary)
                  break;
               
               
               offset += stride;
            } 

            if (sampleCompareCounter > sampleCompareBoundary)
               break;

         } // go through bm1's change area


      } // is significant pattern
   }
  

   return false;
}


// untested/unreviewed!
void CodecInst::compensateMotion(unsigned char* bm, 
                                 int width, int stride, RECT area, POINT vector)
{
   int padding = LMisc::calcPadding(width, stride);
 
   int areaWidth = area.right-area.left+1;
  
   if (vector.y > 0)
   {
      for (int y=area.bottom; y>=area.top; --y)
      {
         int idxSource = (y*width+area.left)*stride+y*padding;
         int idxTarget = ((y + vector.y)*width + (area.left + vector.x))*stride + (y+vector.y)*padding;
      
         memcpy(bm+idxTarget, bm+idxSource, areaWidth*stride);
      }
   }
   else
   {
      for (int y=area.top; y<=area.bottom; ++y)
      {
         int idxSource = (y*width+area.left)*stride+y*padding;
         int idxTarget = ((y + vector.y)*width + (area.left + vector.x))*stride + (y+vector.y)*padding;
      
         memcpy(bm+idxTarget, bm+idxSource, areaWidth*stride);
      }
   }





   /*
   unsigned char* helper = (unsigned char*)malloc(areaWidth*stride);

   bool inverse = vector.y > 0;
   int startY = inverse ? area.bottom : area.top;
   int endY = inverse ? area.top : area.bottom;
   
   int offsetSource = (startY*width+area.left)*stride;
   int offsetTarget = ((startY + vector.y)*width + (area.left + vector.x))*stride;
   int offsetIncrement = inverse ? -width*stride : width*stride;
   for (int y=area.top; y<=area.bottom; ++y)
   {
      // due to inverse y might not be the current y-axis position
      // but height-y

      memcpy(helper, bm+offsetSource, areaWidth*stride);
      memcpy(bm+offsetTarget, helper, areaWidth*stride);

      offsetSource += offsetIncrement;
      offsetTarget += offsetIncrement;
   }


   delete(helper);
   //*/


}


