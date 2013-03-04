#if !defined(__AVINFO__H__)
#define __AVINFO__H__

#include <qedit.h>
#include <dshow.h>

#ifdef AVEDIT_EXPORTS
#define AVEDIT_API __declspec(dllexport)
#else
#define AVEDIT_API __declspec(dllimport)
#endif

typedef struct
{
   DWORD fcc;
   DWORD alternativeFcc;
   _TCHAR tszDescription[160];
   bool  bSupportsKeyframes;
   bool  bSupportsQuality;
   int   nDefaultKeyframeRate;
   int   nDefaultQuality;

   bool  bHasConfigureDialog;
   bool  bHasAboutDialog;
} CODECINFO;

class AVEDIT_API AVInfo
{
   _TCHAR *m_tszSourceFile;
   IMediaDet *m_pMediaDet;

   int m_iVideoIndex;
   int m_iAudioIndex;

   int         m_codecListSize;
   CODECINFO **m_plCodecList;

public:
   /*
   static void StaticDelete();
   static void StaticSet(char *path, IMediaDet *det);
   */
   static void StaticMakeUncompressed(AM_MEDIA_TYPE *type);
   static bool StaticIsAudio(AM_MEDIA_TYPE *type);
   static bool StaticIsPcmAudio(AM_MEDIA_TYPE *type);
   static bool StaticIsFilePcmAudio(const _TCHAR *tszAudioFile);
   static bool StaticIsCompressed(AM_MEDIA_TYPE *type);
   static HRESULT StaticCreateCompressor(AM_MEDIA_TYPE *pTargetType,
                                         DWORD dwFcc,
                                         IBaseFilter **pCompressor,
                                         int nKeyframeRate = -1, int nQuality = -1);
   static HRESULT StaticConfigureCompressor(IBaseFilter *pCompressor,
                                         int nKeyframeRate = -1, int nQuality = -1);
   static HRESULT StaticWriteWavFromFile(const _TCHAR *tszSrcFileName, int nStreamId, 
                                         const _TCHAR *tszTargetFileName,
                                         bool bWindowsMediaMode,
                                         HWND hWndReport);
   static void StaticWriteWavFromFileCancel();
   static HRESULT StaticCheckRenderable(const _TCHAR *tszVideoFileName);
   AVInfo();
   ~AVInfo();
   
   HRESULT Initialize(const _TCHAR *tsz_SourceFile);
   HRESULT GetStreamCount(long *count) const;
   HRESULT GetLength(double *length) const;
   HRESULT GetStreamFormat(const int id, AM_MEDIA_TYPE *type) const;
   HRESULT GetVideoStreamIndex(int *pIndex);
   HRESULT GetAudioStreamIndex(int *pIndex);

   // Returns the number of codecs which are able to
   // encode a video of this size; the color space is
   // always assumed to be 24 bit RGB data
   int GetCodecListSize(int width, int height);
   // Returns the previously (in GetCodecListSize()) 
   // allocated codec list.
   void GetCodecList(CODECINFO **plCI);

   bool HasVideoVfwCodec(const _TCHAR *tszVideoFileName);

   // Creates a compressed media type for the given FOURCC
   // code; assumes that the input media type is cx x cy
   // and RGB 24 Bit data. Returns true if successful,
   // otherwise false.
   bool CreateStreamFormat(DWORD fcc, int cx, int cy, AM_MEDIA_TYPE *pMt);

   void ConfigureCodec(DWORD fcc, HWND hWndParent=NULL);
   void AboutCodec(DWORD fcc, HWND hWndParent=NULL);

private:
   void Make24BitRGB(BITMAPINFOHEADER *pBi, int cx, int cy);
   static bool m_bWriteWavFromFileCancelRequest;
   HRESULT DetermineStreamIndices();
};

/*
class AVInfoInitializationException
{
};
*/


#endif