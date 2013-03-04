package imc.lecturnity.converter; 
 
/**
 * Contains all information needed to display conversion codec information
 * and create appropriate profiles. Currently specific for WM.
 */
public class StreamInformation
{
   public static int AUDIO_STREAM = 1;
   public static int VIDEO_STREAM = 2;
      
   public static int CODEC_AUDIO_WMA2 = 0;
   public static int CODEC_AUDIO_ACELP = 1;
      
   public static int CODEC_VIDEO_MP43 = 0;
   public static int CODEC_VIDEO_WMV7 = 1;
   public static int CODEC_VIDEO_WMV8 = 2;
   public static int CODEC_VIDEO_WMV9 = 3;
   public static int CODEC_VIDEO_WMV9P = 4;
   public static int CODEC_VIDEO_WMV9V = 5;
      
   public static int CODEC_VIDEO_SG7 = 1000;
   public static int CODEC_VIDEO_SG9 = 1001;
      
      
      
      
   public int m_nStreamType = AUDIO_STREAM;
      
   public int m_nBitrate = 6000;
   public int m_nFrequency = 8000; // sample rate for audio, frame rate * 10 for video
   public int m_nChannels = 1; // unused for video
   public int m_nQuality = 100; // reserved
   public int m_nMaxBitrate = 120000; // reserved
   public int m_nWidth = 320; // unused for audio
   public int m_nHeight = 240; // unused for audio
      
   public int m_nCodecIndex = 0;
   public int m_nSubCodecIndex = 0;
      
      
   public StreamInformation(int nStreamType)
   {
      m_nStreamType = nStreamType;
   }
      
   public StreamInformation(int nCodecIndex, int nBitrate, int nFramerateTenths, int nWidth, int nHeight)
   {
      m_nStreamType = VIDEO_STREAM;
         
      m_nCodecIndex = nCodecIndex;
      m_nBitrate = nBitrate;
      m_nFrequency = nFramerateTenths;
      m_nWidth = nWidth;
      m_nHeight = nHeight;
         
   }
      
   public StreamInformation(int nCodecIndex, int nBitrate, int nSamplerate, int nChannels)
   {
      m_nStreamType = AUDIO_STREAM;
         
      m_nCodecIndex = nCodecIndex;
      m_nBitrate = nBitrate;
      m_nFrequency = nSamplerate;
      m_nChannels = nChannels;
   }
      
   public StreamInformation(StreamInformation toCopy)
   {
      m_nStreamType = toCopy.m_nStreamType;
      
      m_nBitrate = toCopy.m_nBitrate;
      m_nFrequency = toCopy.m_nFrequency;
      m_nChannels = toCopy.m_nChannels;
      m_nQuality = toCopy.m_nQuality;
      m_nMaxBitrate = toCopy.m_nMaxBitrate;
      m_nWidth = toCopy.m_nWidth;
      m_nHeight = toCopy.m_nHeight;
      
      m_nCodecIndex = toCopy.m_nCodecIndex;
      m_nSubCodecIndex = toCopy.m_nSubCodecIndex;
   }
   
   
   public boolean differs(StreamInformation toCompare)
   {
      if (m_nStreamType != toCompare.m_nStreamType)
         return true;
      
      if (m_nBitrate != toCompare.m_nBitrate)
         return true;
      if (m_nFrequency != toCompare.m_nFrequency)
         return true;
      if (m_nChannels != toCompare.m_nChannels)
         return true;
      if (m_nQuality != toCompare.m_nQuality)
         return true;
      if (m_nMaxBitrate != toCompare.m_nMaxBitrate)
         return true;
      if (m_nWidth != toCompare.m_nWidth)
         return true;
      if (m_nHeight != toCompare.m_nHeight)
         return true;
      
      if (m_nCodecIndex != toCompare.m_nCodecIndex)
         return true;
      if (m_nSubCodecIndex != toCompare.m_nSubCodecIndex)
         return true;
   
      return false;
   }

}