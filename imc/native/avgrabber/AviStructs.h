#ifndef __AVISTRUCTS
#define __AVISTRUCTS

//#include <list>
//#include "global.h"

#define VIDEOCAP_OK					0
#define VIDEOCAP_WRONG_OUTPUT		1
#define VIDEOCAP_WRONG_INPUT		2
#define VIDEOCAP_CODEC_NOT_FOUND	3
#define VIDEOCAP_NO_MEMORY			4

typedef __int64         TIME_US;

typedef struct
{
	bool bSupportsQuality;
	bool bSupportsDatarate;
	bool bUseDatarate;
	bool bSupportsKeyframes;
	bool bUseKeyframes;
	int iQuality;
   // The target datarate, measured in KB/s
	int iDatarate;
	int iKeyframeRate;
	int iDefaultKeyframeRate;
	bool bHasConfigDialog;
	bool bHasInfoDialog;
	FOURCC fccHandler;
	DWORD dwFlags; // save'm for later
	_TCHAR fccString[8];
	_TCHAR tszName[128];
	_TCHAR tszDescription[128];
} CodecInfo;

typedef struct
{
   _TCHAR tszName[128];
   int nSampleRates;
   int sampleRates[16];
   int nBitRates;
   int bitRates[16];
} AudioDevice;

typedef struct 
{
	int nDeviceID;
	_TCHAR tszName[128];
	bool bHasFormatDlg;
	bool bHasSourceDlg;
	bool bHasDisplayDlg;
	bool isEnabled;
} VideoDevice;

typedef struct 
{
	int width;
	int height;
	int colorDepth;
} VideoSettings;

typedef struct 
{
    FOURCC		fccType;
    FOURCC		fccHandler;
    DWORD		dwFlags;	/* Contains AVITF_* flags */
    WORD		wPriority;
    WORD		wLanguage;
    DWORD		dwInitialFrames;
    DWORD		dwScale;	
    DWORD		dwRate;	/* dwRate / dwScale == samples/second */
    DWORD		dwStart;
    DWORD		dwLength; /* In units above... */
    DWORD		dwSuggestedBufferSize;
    DWORD		dwQuality;
    DWORD		dwSampleSize;
	WORD		left;
	WORD		top;
	WORD		right;
	WORD		bottom;
    // RECT		rcFrame;
} LecturnityAVIStreamHeader;

typedef struct
{
   BITMAPINFO* info;
   char*       data;
} ThumbnailData;

#endif