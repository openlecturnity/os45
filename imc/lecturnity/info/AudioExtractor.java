package imc.lecturnity.info;

import java.io.File;

import imc.epresenter.player.audio.SoundPlayer;

/**
 * Helper class for obtaining audio information and do audio extraction.
 */
public class AudioExtractor
{
   private SoundPlayer m_SoundPlayer;
   
   AudioExtractor(SoundPlayer player)
   {
      m_SoundPlayer = player;
   }
   
   /**
    * Saves the audio stream in an LPD or LRD file to the given
    * location. The output will be an uncompressed WAVE file. The
    * audio parameters (channels, sampling rate, ...) of the stream
    * will be preserved.
    *
    * @return <code>true</code> if the operation finished without error.
    */
   public boolean extractToFile(File wavFile)
   {
      return m_SoundPlayer.decompressAll(wavFile);
   }

   /**
    * @return the number of audio channels contained in the audio
    * stream.
    */
   public int getChannels()
   {
      return m_SoundPlayer.getInputFormat().getChannels();
   }

   /**
    * @return the sampling rate used in the audio stream.
    */
   public float getSamplingRate()
   {
      return m_SoundPlayer.getInputFormat().getSampleRate();
   }

   /**
    * @return the number of bits per sample in the audio stream.
    */
   public int getBitsPerSample()
   {
      return m_SoundPlayer.getInputFormat().getSampleSizeInBits();
   }
} 