/* FILE FlvUtils.h */

#ifndef __FLV_UTILS_H__
#define __FLV_UTILS_H__

// from ffmpeg 0.4.9-pre1
#ifndef EMULATE_INTTYPES
#define EMULATE_INTTYPES
#include "avcodec.h"
#include "avformat.h"
#endif //EMULATE_INTTYPES

// The code used for this class comes more or less from "output_example.c" (ffmpeg-0.4.9-pre1)

typedef struct color
{
   char r;
   char g;
   char b;
} color;

class CFlvUtils
{
public:
	CFlvUtils();
   ~CFlvUtils();

   UINT Clear();
   UINT Init();
   UINT InitStreams();

   UINT OpenFlvFile();
   UINT CloseFlvFile();

   UINT PutRgbBufferToFrame(char* pBuffer);
   UINT WriteVideoFrame();

   UINT PutSrcVideoDimensions(int nVideoWidth, int nVideoHeight);
   UINT SetOutputFlvName(const _TCHAR *tszFlvFile);
   UINT SetVideoCodecParams(int &nVideoWidth, int &nVideoHeight, int nVideoBitrate, int nVideoFramerate);
   UINT SetAudioCodecParams(int nAudioBitrate, int nAudioSamplerate, int nAudioChannels);

   void SetVideoGopSize(int nVideoGopSize) {m_nVideoGopSize = nVideoGopSize;}

private:
   AVStream* AddVideoStream(int codecId);
   AVStream* AddAudioStream(int codecId);
   UINT OpenVideoStream();
   UINT OpenAudioStream();
   void CloseVideoStream();
   void CloseAudioStream();
   AVFrame* AllocPicture(int nPixFmt, int nWidth, int nHeight);

private:
   //_TCHAR              *m_tszFlvFile;
   char                *m_szFlvFile;
   
   AVOutputFormat      *m_pAVOutputFormat;
   AVFormatContext     *m_pAVFormatContext;
   AVStream            *m_pAVStreamVideo;
   AVStream            *m_pAVStreamAudio;
   ImgReSampleContext  *m_pImgResampleContext;

   AVFrame             *m_pAVFramePicture;
   AVFrame             *m_pAVFrameTmpPicture;
   AVFrame             *m_pAVFrameTmpPicture2;
   uint8_t             *m_pVideoOutbuf;
   int                  m_nFrameCount;
   int                  m_nVideoOutbufSize;

   int16_t             *m_pSamples;
   uint8_t             *m_pAudioOutbuf;
   int                  m_nAudioOutbufSize;
   int                  m_nAudioInputFrameSize;

   int                  m_nVideoBitrate;
   enum PixelFormat     m_nVideoPixelFormat;
   int                  m_nSrcVideoWidth;
   int                  m_nSrcVideoHeight;
   int                  m_nDestVideoWidth;
   int                  m_nDestVideoHeight;
   int                  m_nVideoFramerate;
   int                  m_nVideoFramerateBase;
   int                  m_nVideoGopSize;

   int                  m_nAudioBitrate;
   int                  m_nAudioSamplerate;
   int                  m_nAudioChannels;

   color                m_colorBackground;

   // Only for dummy audio stream
   float                m_fT, m_fTIncr, m_fTIncr2;
};

#endif /* !__FLV_UTILS_H__ */
