/* FILE FlvUtils.cpp */

#include "stdafx.h"

#include "FlvUtils.h"


/* ************************************************************************* */

CFlvUtils::CFlvUtils()
{
   m_szFlvFile = NULL;

   m_pAVOutputFormat = NULL;
   m_pAVFormatContext = NULL;
   m_pAVStreamVideo = NULL;
   m_pAVStreamAudio = NULL;

   m_pAVFramePicture = NULL;
   m_pAVFrameTmpPicture = NULL;
   m_pAVFrameTmpPicture2 = NULL;
   m_pVideoOutbuf = NULL;
   m_nFrameCount = 0;
   m_nVideoOutbufSize = 0;

   m_pSamples = NULL;
   m_pAudioOutbuf = NULL;
   m_nAudioOutbufSize = 0;
   m_nAudioInputFrameSize = 0;

   // Set some default values
   m_nVideoBitrate = 200000;
   m_nVideoPixelFormat = PIX_FMT_YUV420P;
   m_nSrcVideoWidth = 0;
   m_nSrcVideoHeight = 0;
   m_nDestVideoWidth = 320;
   m_nDestVideoHeight = 240;
   m_nVideoFramerate = 25;
   m_nVideoFramerateBase = 1;
   m_nVideoGopSize = 12; // Emit one intra frame every twelve frames at most

   m_nAudioBitrate = 64000;
   m_nAudioSamplerate = 44100;
   m_nAudioChannels = 2;

   m_colorBackground.r = (char)0xEA;
   m_colorBackground.g = (char)0xE7;
   m_colorBackground.b = (char)0xD7;
}

CFlvUtils::~CFlvUtils()
{
   if (m_szFlvFile)
      delete[] m_szFlvFile;
   m_szFlvFile = NULL;

   Clear();
}

UINT CFlvUtils::Clear()
{
   HRESULT hr = S_OK;
   _tprintf(_T("CFlvUtils::Clear()\n"));

   // Free audio/video stream
   if (m_pAVStreamVideo)
      CloseVideoStream();
   m_pAVStreamVideo = NULL;

   if (m_pAVStreamAudio)
      CloseAudioStream();
   m_pAVStreamAudio = NULL;

   // Free the AV context streams & AV format context
   if (m_pAVFormatContext)
   {
      for (int i = 0; i < m_pAVFormatContext->nb_streams; i++)
      {
         av_freep(&m_pAVFormatContext->streams[i]);
      }
      av_free(m_pAVFormatContext);
   }
   m_pAVFormatContext = NULL;

   // Free AV output format
   if (m_pAVOutputFormat)
      av_free(m_pAVOutputFormat);
   m_pAVOutputFormat = NULL;

   return hr;
}

UINT CFlvUtils::Init()
{
   HRESULT hr = S_OK;

   if (!m_szFlvFile)
      hr = E_FAIL;

   if (SUCCEEDED(hr))
   {
      // Must be called before using avcodec lib
      avcodec_init();

      // Initialize libavcodec
      // and register all formats and codecs
      av_register_all();
   }

   if (SUCCEEDED(hr))
   {
      //Auto detect the output format from the name. Default is flv.
      m_pAVOutputFormat = guess_format(NULL, m_szFlvFile, NULL);
      if (!m_pAVOutputFormat)
      {
         _tprintf(_T("Could not deduce output format from file extension: using FLV.\n"));
         m_pAVOutputFormat = guess_format("flv", NULL, NULL);
      }
      if (!m_pAVOutputFormat)
      {
         _ftprintf(stderr, _T("Error in CFlvUtils::Init():\n Could not find suitable output format!\n"));
         hr = E_FAIL;
         // TODO: error handling?
      }
   }

   if (SUCCEEDED(hr))
   {
      // Allocate the output media context
      m_pAVFormatContext = av_alloc_format_context();
      if (!m_pAVFormatContext)
      {
         _ftprintf(stderr, _T("Error in CFlvUtils::Init():\n Could not allocate the output media context!\n"));
         hr = E_FAIL;
         // TODO: error handling?
      }
      else
      {
         m_pAVFormatContext->oformat = m_pAVOutputFormat;
         //_sntprintf(m_pAVFormatContext->filename, sizeof(m_pAVFormatContext->filename), _T("%s"), m_szFlvFile);
         _snprintf(m_pAVFormatContext->filename, sizeof(m_pAVFormatContext->filename), "%s", m_szFlvFile);
      }
   }

   return hr;
}

UINT CFlvUtils::InitStreams()
{
   HRESULT hr = S_OK;

   if (!m_szFlvFile)
      hr = E_FAIL;

   // Add the audio and video streams using the default format codecs
   // and initialize the codecs
   if (SUCCEEDED(hr))
   {
      if (m_pAVOutputFormat->video_codec != CODEC_ID_NONE)
      {
         //_tprintf(_T("### codec supports video.\n"));
         m_pAVStreamVideo = AddVideoStream(m_pAVOutputFormat->video_codec);
         if (m_pAVStreamVideo == NULL)
         {
            hr = E_FAIL;
            _ftprintf(stderr, _T("Error in CFlvUtils::Init():\n Could not allocate video stream!\n"));
            // TODO: error handling?
         }
      }
   }
   if (SUCCEEDED(hr))
   {
      if (m_pAVOutputFormat->audio_codec != CODEC_ID_NONE)
      {
         //_tprintf(_T("### codec supports audio.\n"));
         m_pAVStreamAudio = AddAudioStream(m_pAVOutputFormat->audio_codec);
         if (m_pAVStreamAudio == NULL)
         {
            hr = E_FAIL;
            _ftprintf(stderr, _T("Error in CFlvUtils::Init():\n Could not allocate audio stream!\n"));
            // TODO: error handling?
         }
      }
   }
   if (SUCCEEDED(hr))
   {
      // Set the output parameters (must be done even if no parameters).
      if (av_set_parameters(m_pAVFormatContext, NULL) < 0)
      {
         _ftprintf(stderr, _T("Error in CFlvUtils::Init():\n Invalid output format parameters!\n"));
         hr = E_FAIL;
         // TODO: error handling?
      }
   }

   if (SUCCEEDED(hr))
   {
      // Dump codec format info to the shell
      dump_format(m_pAVFormatContext, 0, m_szFlvFile, 1);
   }

   // Now that all the parameters are set, we can open the audio and 
   // video codecs and allocate the necessary encode buffers
   if (SUCCEEDED(hr))
   {
      if (m_pAVStreamVideo)
         hr = OpenVideoStream();
   }
   if (SUCCEEDED(hr))
   {
      if (m_pAVStreamAudio)
         hr = OpenAudioStream();
   }

   return hr;
}

/* ************************************************************************* */

UINT CFlvUtils::OpenFlvFile()
{
   HRESULT hr = S_OK;

   if (!m_szFlvFile)
      hr = E_FAIL;

   if (SUCCEEDED(hr))
   {
      if (!(m_pAVOutputFormat->flags & AVFMT_NOFILE))
      {
         if (url_fopen(&m_pAVFormatContext->pb, m_szFlvFile, URL_WRONLY) < 0)
         {
            hr = E_FAIL;
            _ftprintf(stderr, _T("Error in CFlvUtils::OpenFlvFile():\n Could not open '%s'!\n"), m_szFlvFile);
            // TODO: error handling?
         }
      }

      // Write the stream header, if any 
      av_write_header(m_pAVFormatContext);
   }

   return hr;
}

UINT CFlvUtils::CloseFlvFile()
{
   HRESULT hr = S_OK;

   // Write the trailer, if any
   av_write_trailer(m_pAVFormatContext);

   if (!(m_pAVOutputFormat->flags & AVFMT_NOFILE))
   {
     url_fclose(&m_pAVFormatContext->pb);
   }

   // Close resample context
   img_resample_close(m_pImgResampleContext);

   return hr;
}

UINT CFlvUtils::PutRgbBufferToFrame(char* pBuffer)
{
   // Note: pBuffer includes a BITMAPINFOHEADER structure at the beginning

   HRESULT hr = S_OK;

/*
   BITMAPINFOHEADER *pBIH = (BITMAPINFOHEADER *)pBuffer;
   int width = pBIH->biWidth;
   int height = pBIH->biHeight;
   //_tprintf(_T("%dx%d "), width, height);

   // Note: 
   // If we have a video (e.g. SG clip) with odd numbers for width/height, 
   // e.g. 331x259 pixel, the BITMAPINFOHEADER says the bitmap has this size. 
   // But the buffer pBuffer allocated with IMediaDet->GetBitmapBits(..) 
   // leads to an image size of 332x259. 

   // Does the bitmpap size matches the input video size?
   // Note: input video width/height must be even numbers.
   if ((width != m_nSrcVideoWidth) || (height != m_nSrcVideoHeight))
   {
      //_tprintf(_T("Video size mismatch in PutRgbBufferToFrame() !\n"));
      //hr = E_FAIL;
   }
*/

   if (SUCCEEDED(hr))
   {
      // pBuffer includes a BITMAPINFOHEADER structure (40 bytes) at the beginning
      int idx = sizeof(BITMAPINFOHEADER);
      int x, y;
      int ls = m_pAVFrameTmpPicture->linesize[0];

      int width = m_nSrcVideoWidth;
      int height = m_nSrcVideoHeight;

      // Bitmap is upside down - so flip it
      //for (y = 0; y < m_nSrcVideoHeight; y++)
      for (y = (height-1); y >= 0; y--)
      {
         for (x = 0; x < width; x++)
         {
            // BGR --> RGB? It works this way
            m_pAVFrameTmpPicture->data[0][(y * ls) + (x * 3) + 2] = pBuffer[idx++];
            m_pAVFrameTmpPicture->data[0][(y * ls) + (x * 3) + 1] = pBuffer[idx++];
            m_pAVFrameTmpPicture->data[0][(y * ls) + (x * 3) + 0] = pBuffer[idx++];
         }
      }
   }

   return hr;
}

UINT CFlvUtils::WriteVideoFrame()
{
   HRESULT hr = S_OK;

   AVCodecContext *avCodecContext = &m_pAVStreamVideo->codec;
   AVFrame *pPicturePointer = NULL;

//   img_convert((AVPicture *)m_pAVFramePicture, avCodecContext->pix_fmt, 
//               (AVPicture *)m_pAVFrameTmpPicture, PIX_FMT_RGB24,
//               avCodecContext->width, avCodecContext->height);

   // Is resampling necessary?
   if ((m_nDestVideoWidth != m_nSrcVideoWidth) || (m_nDestVideoHeight != m_nSrcVideoHeight))
   {
      // Convert RGB data to the output format
      img_convert((AVPicture *)m_pAVFrameTmpPicture2, avCodecContext->pix_fmt, 
                  (AVPicture *)m_pAVFrameTmpPicture, PIX_FMT_RGB24,
                  m_nSrcVideoWidth, m_nSrcVideoHeight);

      // Resample
      img_resample(m_pImgResampleContext, 
                   (AVPicture *)m_pAVFramePicture, 
                   (AVPicture *)m_pAVFrameTmpPicture2);
   }
   else
   {
      // Convert RGB data to the output format
      img_convert((AVPicture *)m_pAVFramePicture, avCodecContext->pix_fmt, 
                  (AVPicture *)m_pAVFrameTmpPicture, PIX_FMT_RGB24,
                  m_nDestVideoWidth, m_nDestVideoHeight);
   }
   
   if (m_pAVFramePicture)
      pPicturePointer = m_pAVFramePicture;

   int nReturn;
   int nOutSize = 0;

   if (m_pAVFormatContext->oformat->flags & AVFMT_RAWPICTURE)
   {
      // Raw video case. 
      AVPacket pkt;
      av_init_packet(&pkt);

      pkt.flags |= PKT_FLAG_KEY;
      pkt.stream_index= m_pAVStreamVideo->index;
      pkt.data= (uint8_t *)pPicturePointer;
      pkt.size= sizeof(AVPicture);

      // Write the frame into the media file
      nReturn = av_write_frame(m_pAVFormatContext, &pkt);
   }
   else
   {
      // Encode the image
      nOutSize = avcodec_encode_video(avCodecContext, m_pVideoOutbuf, m_nVideoOutbufSize, pPicturePointer);

      if (nOutSize != 0)
      {
         AVPacket pkt;
         av_init_packet(&pkt);

         pkt.pts= avCodecContext->coded_frame->pts;
         if(avCodecContext->coded_frame->key_frame)
            pkt.flags |= PKT_FLAG_KEY;
         pkt.stream_index= m_pAVStreamVideo->index;
         pkt.data= m_pVideoOutbuf;
         pkt.size= nOutSize;

         // Write the compressed frame in the media file
         nReturn = av_write_frame(m_pAVFormatContext, &pkt);
      }
      else
         nReturn = 0;
   }

   if (0 == nReturn)
      hr = E_FAIL;

   return hr;
}

UINT CFlvUtils::PutSrcVideoDimensions(int nVideoWidth, int nVideoHeight)
{
   m_nSrcVideoWidth = nVideoWidth;
   m_nSrcVideoHeight = nVideoHeight;

   // Video width must be a multiple of 4.
   // Video height must be a multiple of 2.
   // This has to be checked before calling this method!

//   // Video resolution: width/height must be even (a multiple of two)
//   if ((m_nSrcVideoWidth % 2) !=0)
//      m_nSrcVideoWidth +=1;
//   if ((m_nSrcVideoHeight % 2) !=0)
//      m_nSrcVideoHeight +=1;

   return S_OK;
}

UINT CFlvUtils::SetOutputFlvName(const _TCHAR *tszFlvFile)
{
   HRESULT hr = S_OK;

   if (tszFlvFile)
   {
///      m_tszFlvFile = new _TCHAR[_tcslen(tszFlvFile)+1];
///      _tcscpy(m_tszFlvFile, tszFlvFile);
///      m_tszFlvFile[_tcslen(tszFlvFile)] = '\0';
#ifdef _UNICODE
      int nLen = ::WideCharToMultiByte(CP_UTF8, 0, tszFlvFile, -1, NULL, 0, NULL, NULL);
      m_szFlvFile = new char[nLen + 1];
      ::WideCharToMultiByte(CP_UTF8, 0, tszFlvFile, -1, m_szFlvFile, nLen + 1, NULL, NULL);
#else
      m_szFlvFile = new char[strlen(tszFlvFile)+1];
      strcpy(m_szFlvFile, tszFlvFile);
      m_szFlvFile[strlen(tszFlvFile)] = '\0';
#endif //_UNICODE
   }
   else
   {
      hr = E_FAIL;
   }

   return hr;
}

UINT CFlvUtils::SetVideoCodecParams(int &nVideoWidth, int &nVideoHeight, int nVideoBitrate, int nVideoFramerate)
{
   // Note: PutSrcVideoDimensions() has to be called before this

   HRESULT hr = S_OK;

   m_nDestVideoWidth = nVideoWidth;
   m_nDestVideoHeight = nVideoHeight;
   m_nVideoBitrate = nVideoBitrate;
   m_nVideoFramerate = nVideoFramerate;

//   // Video resolution: width/height must be even (a multiple of two)
//   // (but seems to be not necessary for reampling)
//   if ((m_nDestVideoWidth % 2) !=0)
//      m_nDestVideoWidth +=1;
//   if ((m_nDestVideoHeight % 2) !=0)
//      m_nDestVideoHeight +=1;

   // Video width must be a multiple of 4,
   // video height must be a multiple of 2.
   // --> Expand video size, if necessary
   if (m_nDestVideoWidth % 4 != 0)
      m_nDestVideoWidth += (4 - (m_nDestVideoWidth % 4));
   if (m_nDestVideoHeight % 2 != 0)
      m_nDestVideoHeight += (2 - (m_nDestVideoHeight % 2));

   // Resample context
   m_pImgResampleContext = img_resample_init(m_nDestVideoWidth, m_nDestVideoHeight, 
                                             m_nSrcVideoWidth, m_nSrcVideoHeight);

   nVideoWidth = m_nDestVideoWidth;
   nVideoHeight = m_nDestVideoHeight;

   return hr;
}

UINT CFlvUtils::SetAudioCodecParams(int nAudioBitrate, int nAudioSamplerate, int nAudioChannels)
{
   m_nAudioBitrate = nAudioBitrate;
   m_nAudioSamplerate = nAudioSamplerate;
   m_nAudioChannels = nAudioChannels;

   return S_OK;
}
/* ************************************************************************* */

AVStream* CFlvUtils::AddVideoStream(int codecId)
{
   AVStream *avStream;
   AVCodecContext *avCodecContext;

   avStream = av_new_stream(m_pAVFormatContext, 0);
   if (!avStream)
   {
      return NULL;
   }

   avCodecContext = &avStream->codec;
   avCodecContext->codec_id = (CodecID)codecId;
   avCodecContext->codec_type = CODEC_TYPE_VIDEO;

   // Put sample parameters
   avCodecContext->bit_rate = m_nVideoBitrate;
   avCodecContext->pix_fmt = m_nVideoPixelFormat;
   avCodecContext->width = m_nDestVideoWidth;  
   avCodecContext->height = m_nDestVideoHeight;
   avCodecContext->frame_rate = m_nVideoFramerate;  
   avCodecContext->frame_rate_base = m_nVideoFramerateBase;
   avCodecContext->gop_size = m_nVideoGopSize; 

   if (avCodecContext->codec_id == CODEC_ID_MPEG2VIDEO)
   {
      // Just for testing, we also add B frames
      avCodecContext->max_b_frames = 2;
   }
   if (avCodecContext->codec_id == CODEC_ID_MPEG1VIDEO)
   {
      // Needed to avoid using macroblocks in which some coeffs overflow 
      // this doesnt happen with normal video, it just happens here as the 
      // motion of the chroma plane doesnt match the luma plane 
      avCodecContext->mb_decision = 2;
   }
   // some formats want stream headers to be seperate
   if(!strcmp(m_pAVFormatContext->oformat->name, "mp4") 
      || !strcmp(m_pAVFormatContext->oformat->name, "mov") 
      || !strcmp(m_pAVFormatContext->oformat->name, "3gp"))
   {
      avCodecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;
   }

   return avStream;
}

AVStream* CFlvUtils::AddAudioStream(int codecId)
{
   AVStream *avStream;
   AVCodecContext *avCodecContext;

   avStream = av_new_stream(m_pAVFormatContext, 1);
   if (!avStream)
   {
      _ftprintf(stderr, _T("Error in CFlvUtils::AddAudioStream():\n Could not allocate stream!\n"));
      return NULL;
   }

   avCodecContext = &avStream->codec;
   avCodecContext->codec_id = (CodecID)codecId;
   avCodecContext->codec_type = CODEC_TYPE_AUDIO;

   // Put sample parameters
   avCodecContext->bit_rate = m_nAudioBitrate;
   avCodecContext->sample_rate = m_nAudioSamplerate;
   avCodecContext->channels = m_nAudioChannels;

   return avStream;
}

UINT CFlvUtils::OpenVideoStream()
{
   HRESULT hr = S_OK;

   AVCodec *avCodec;
   AVCodecContext *avCodecContext;

   avCodecContext = &m_pAVStreamVideo->codec;

   if (SUCCEEDED(hr))
   {
      // Find the audio encoder
      avCodec = avcodec_find_encoder(avCodecContext->codec_id);
      if (!avCodec)
      {
         _ftprintf(stderr, _T("Error in CFlvUtils::OpenVideoStream():\n Codec not found!\n"));
         hr = E_FAIL;
         // TODO: error handling?
      }
   }

   if (SUCCEEDED(hr))
   {
      // Open it
      if (avcodec_open(avCodecContext, avCodec) < 0)
      {
         _ftprintf(stderr, _T("Error in CFlvUtils::OpenVideoStream():\n Could not open codec!\n"));
         hr = E_FAIL;
         // TODO: error handling?
      }
   }

   if (SUCCEEDED(hr))
   {
      m_pVideoOutbuf = NULL;
      if (!(m_pAVFormatContext->oformat->flags & AVFMT_RAWPICTURE))
      {
         // Allocate output buffer
         // XXX: API change will be done
         m_nVideoOutbufSize = 200000;
         m_pVideoOutbuf = (uint8_t*)malloc(m_nVideoOutbufSize * sizeof(uint8_t));
      }

      // Allocate the encoded raw picture
//      m_pAVFramePicture = AllocPicture(avCodecContext->pix_fmt, avCodecContext->width, avCodecContext->height);
      m_pAVFramePicture = AllocPicture(avCodecContext->pix_fmt, m_nDestVideoWidth, m_nDestVideoHeight);
      if (!m_pAVFramePicture)
      {
         _ftprintf(stderr, _T("Error in CFlvUtils::OpenVideoStream():\n Could not allocate picture!\n"));
         hr = E_FAIL;
         // TODO: error handling?
      }
   }

   if (SUCCEEDED(hr))
   {
      // A temporary RGB24 picture is needed too. 
      // It is then converted to the required output format
      m_pAVFrameTmpPicture = NULL;
//      m_pAVFrameTmpPicture = AllocPicture(PIX_FMT_RGB24, avCodecContext->width, avCodecContext->height);
      m_pAVFrameTmpPicture = AllocPicture(PIX_FMT_RGB24, m_nSrcVideoWidth, m_nSrcVideoHeight);
      if (!m_pAVFrameTmpPicture)
      {
         _ftprintf(stderr, _T("Error in CFlvUtils::OpenVideoStream():\n Could not allocate temporary picture!\n"));
         hr = E_FAIL;
         // TODO: error handling?
      }
   }

   if (SUCCEEDED(hr))
   {
      // Is resampling necessary?
      if ((m_nDestVideoWidth != m_nSrcVideoWidth) || (m_nDestVideoHeight != m_nSrcVideoHeight))
      {
         AVCodecContext *avCodecContext = &m_pAVStreamVideo->codec;
         m_pAVFrameTmpPicture2 = NULL;
//         m_pAVFrameTmpPicture2 = AllocPicture(PIX_FMT_RGB24, m_nDestVideoWidth, m_nDestVideoHeight);
         m_pAVFrameTmpPicture2 = AllocPicture(avCodecContext->pix_fmt, m_nSrcVideoWidth, m_nSrcVideoHeight);
         if (!m_pAVFrameTmpPicture2)
         {
            _ftprintf(stderr, _T("Error in CFlvUtils::OpenVideoStream():\n Could not allocate temporary picture 2!\n"));
            hr = E_FAIL;
            // TODO: error handling?
         }
      }
   }

   return hr;
}

UINT CFlvUtils::OpenAudioStream()
{
   HRESULT hr = S_OK;

   AVCodec *avCodec;
   AVCodecContext *avCodecContext;

   avCodecContext = &m_pAVStreamAudio->codec;

   if (SUCCEEDED(hr))
   {
      // Find the audio encoder
      avCodec = avcodec_find_encoder(avCodecContext->codec_id);
      if (!avCodec)
      {
         _ftprintf(stderr, _T("Error in CFlvUtils::OpenAudioStream():\n Codec not found!\n"));
         hr = E_FAIL;
         // TODO: error handling?
      }
   }

   if (SUCCEEDED(hr))
   {
      // Open it
      if (avcodec_open(avCodecContext, avCodec) < 0)
      {
         _ftprintf(stderr, _T("Error in CFlvUtils::OpenAudioStream():\n Could not open codec!\n"));
         hr = E_FAIL;
         // TODO: error handling?
      }
   }

   if (SUCCEEDED(hr))
   {
      // !! HARD CODED !!
      // Init signal generator
      m_fT = 0;
      m_fTIncr = 2.0f * M_PI * 110.0f / avCodecContext->sample_rate;
      // Increment frequency by 110 Hz per second
      m_fTIncr2 = 2.0f * M_PI * 110.0f / avCodecContext->sample_rate / avCodecContext->sample_rate;

      m_nAudioOutbufSize = 10000;
      m_pAudioOutbuf = (uint8_t*)malloc(m_nAudioOutbufSize * sizeof(uint8_t));

      // Ugly hack for PCM codecs (will be removed ASAP with new PCM
      // support to compute the input frame size in samples)
      if (avCodecContext->frame_size <= 1)
      {
         m_nAudioInputFrameSize = m_nAudioOutbufSize / avCodecContext->channels;
         switch(m_pAVStreamAudio->codec.codec_id)
         {
            case CODEC_ID_PCM_S16LE:
            case CODEC_ID_PCM_S16BE:
            case CODEC_ID_PCM_U16LE:
            case CODEC_ID_PCM_U16BE:
               m_nAudioInputFrameSize >>= 1;
               break;
            default:
               break;
         }
      } 
      else
      {
         m_nAudioInputFrameSize = avCodecContext->frame_size;
      }
      m_pSamples = (int16_t*)malloc(m_nAudioInputFrameSize * 2 * avCodecContext->channels * sizeof(int16_t));
   }

   return hr;
}

void CFlvUtils::CloseVideoStream()
{
   _tprintf(_T("CFlvUtils::CloseVideoStream()\n"));

//   if (m_pAVStreamVideo->codec.codec != NULL)
   if (m_pAVStreamVideo && &m_pAVStreamVideo->codec)
      avcodec_close(&m_pAVStreamVideo->codec);
   _tprintf(_T("CFlvUtils::CloseVideoStream(1)\n"));

   if (m_pAVFramePicture != NULL)
   {
      av_free(m_pAVFramePicture->data[0]);
      av_free(m_pAVFramePicture);
   }
   m_pAVFramePicture = NULL;

   if (m_pAVFrameTmpPicture != NULL)
   {
      av_free(m_pAVFrameTmpPicture->data[0]);
      av_free(m_pAVFrameTmpPicture);
   }
   m_pAVFrameTmpPicture = NULL;

   if (m_pAVFrameTmpPicture2 != NULL)
   {
      av_free(m_pAVFrameTmpPicture2->data[0]);
      av_free(m_pAVFrameTmpPicture2);
   }
   m_pAVFrameTmpPicture2 = NULL;

   if (m_pVideoOutbuf != NULL)
      av_free(m_pVideoOutbuf);
   m_pVideoOutbuf = NULL;
}

void CFlvUtils::CloseAudioStream()
{
//   if (m_pAVStreamAudio->codec.codec != NULL)
   if (m_pAVStreamAudio && &m_pAVStreamAudio->codec)
      avcodec_close(&m_pAVStreamAudio->codec);

   if (m_pSamples != NULL)
      av_free(m_pSamples);
   if (m_pAudioOutbuf != NULL)
      av_free(m_pAudioOutbuf);
}

/* ************************************************************************* */

AVFrame* CFlvUtils::AllocPicture(int nPixFmt, int nWidth, int nHeight)
{
   AVFrame *avFramePicture;
   uint8_t *pPictureBuffer;
   int nSize;

   avFramePicture = avcodec_alloc_frame();
   if (!avFramePicture)
      return NULL;
   nSize = avpicture_get_size(nPixFmt, nWidth, nHeight);
   pPictureBuffer = (uint8_t*)malloc(nSize * sizeof(uint8_t));
   if (!pPictureBuffer)
   {
      av_free(avFramePicture);
      return NULL;
   }
   avpicture_fill((AVPicture *)avFramePicture, pPictureBuffer, 
                  nPixFmt, nWidth, nHeight);

   _tprintf(_T("- AllocPicture(): PixFmt: %d - size: %d (%dx%d)\n"), nPixFmt, nSize, nWidth, nHeight);

   return avFramePicture;
}

/*
int avcodec_close(AVCodecContext *avctx)
{
    if (avctx->codec->close)
        avctx->codec->close(avctx);
    avcodec_default_free_buffers(avctx);
    av_freep(&avctx->priv_data);
    avctx->codec = NULL;
    return 0;
}
*/

/*
static void avcodec_default_free_buffers(AVCodecContext *s)
{
    int i, j;

    if(s->internal_buffer==NULL) return;
    
    for(i=0; i<INTERNAL_BUFFER_SIZE; i++){
        InternalBuffer *buf= &((InternalBuffer*)s->internal_buffer)[i];
        for(j=0; j<4; j++){
            av_freep(&buf->base[j]);
            buf->data[j]= NULL;
        }
    }
    av_freep(&s->internal_buffer);
    
    s->internal_buffer_count=0;
}
*/
