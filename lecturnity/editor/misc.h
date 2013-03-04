#ifndef __MISC_H__
#define __MISC_H__

#include "editorDoc.h"

class Misc
{
public:
   static void FormatTickString(CString& string, int millis, STREAM_UNIT su, bool expanded=false);
   static void EnableChildWindows(CWnd *pRoot, BOOL bEnable);

   static void GetStreamConfigDataSizes(AM_MEDIA_TYPE *pMT, int *pMediaSize, int *pVideoHeaderSize);
   static bool GetStreamConfigData(AM_MEDIA_TYPE *pMT, char *pMedia, int *pMediaSize, 
                                                       char *pVideoHeader, int *pVideoHeaderSize);
   static bool SetStreamConfigData(AM_MEDIA_TYPE *pMT, BYTE *pMediaHex, int inMediaSize, 
                                                       BYTE *pVideoHeaderHex, int inVideoHeaderSize);
   static void GetImageHome(CString &csImageHome);
   static bool GetShellFolders(LPCTSTR tszSubFolder, CString &returnValue);

private:
   static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);
   static char MakeHex(char c);
   static char MakeInt(char c);
   static char MakeChar(char c1, char c2);
   static void Make4Bit(char *pSrc, char *pDest, int length);
   static void Make8Bit(char *pSrc, char *pDest, int length);
};

#endif