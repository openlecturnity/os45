package imc.epresenter.filesdk.audio;

import javax.sound.sampled.spi.AudioFileReader;

import javax.sound.sampled.UnsupportedAudioFileException;
import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;

import imc.epresenter.filesdk.XorInputStream;

import java.net.URL;
import java.io.*;

public class LadFileReader extends AudioFileReader
{
   private byte[] key_;
   private XorInputStream xorIn_;

   public LadFileReader()
   {
      super();
   }

   public AudioFileFormat getAudioFileFormat(URL url)
      throws UnsupportedAudioFileException, IOException
   {
      return getAudioFileFormat(url.openStream());
   }

   public AudioFileFormat getAudioFileFormat(File file)
      throws UnsupportedAudioFileException, IOException
   {
      return getAudioFileFormat(new FileInputStream(file));
   }

   public AudioFileFormat getAudioFileFormat(InputStream inputStream)
      throws UnsupportedAudioFileException, IOException
   {
      String unlockKey = System.getProperty("nyckel.musik");
      boolean startable = false;
      if (unlockKey != null)
         if (unlockKey.equals("forte"))
            startable = true;

      if (!startable)
         throw new UnsupportedAudioFileException("Audio format not supported.");

      if (!inputStream.markSupported())
         throw new UnsupportedAudioFileException("mark() not supported by input stream.");
      inputStream.mark(1024);

//       System.out.println("checking for 'LAD '...");
      
      byte[] ladFcc = new byte[4];
      int readBytes = inputStream.read(ladFcc);

      //String ladPerhaps = new String(ladFcc);
      //System.out.println("found '" + ladPerhaps + "'.");

      if (!(ladFcc[0] == 'L' &&
            ladFcc[1] == 'A' &&
            ladFcc[2] == 'D' &&
            ladFcc[3] == ' '))
      {
         inputStream.reset();
         throw new UnsupportedAudioFileException("Audio format not supported.");
      }

      //System.out.println("reading key...");

      key_ = new byte[892];
      readBytes = inputStream.read(key_);
      
      if (readBytes != key_.length)
      {
         inputStream.reset();
         throw new UnsupportedAudioFileException("Could not completely decode audio header.");
      }

      xorIn_ = new XorInputStream(inputStream, key_);
      //System.out.println("reading audio mark...");

      byte[] audioMark = new byte[128];
      readBytes = xorIn_.read(audioMark);

      if (readBytes != audioMark.length)
      {
         inputStream.reset();
         throw new UnsupportedAudioFileException("Could not completely decode Lecturnity audio mark.");
      }

		String tempMark = new String(audioMark);

      int nullByteIndex = 128;
      for (int i=0; i<128; i++)
         if (tempMark.charAt(i) == ((char) 0))
         {
            nullByteIndex = i;
            break;
         }

      String markString = tempMark.substring(0, nullByteIndex);

 		//System.out.println("audio mark: '" + markString + "'.");

      AudioFileFormat aff = null;

      aff = AudioSystem.getAudioFileFormat(xorIn_);

      // note: if the above method fails, this whole mechanism will
      // fail, as the input stream cannot be resetted,

      return aff;
   }

   public AudioInputStream getAudioInputStream(URL url)
      throws UnsupportedAudioFileException, IOException
   {
      return getAudioInputStream(url.openStream());
   }

   public AudioInputStream getAudioInputStream(File file)
      throws UnsupportedAudioFileException, IOException
   {
      return getAudioInputStream(new FileInputStream(file));
   }

   public AudioInputStream getAudioInputStream(InputStream inputStream)
      throws UnsupportedAudioFileException, IOException
   {
      // retrieve audio format with side effect
      AudioFileFormat aff = getAudioFileFormat(inputStream);

      AudioInputStream ais = AudioSystem.getAudioInputStream(xorIn_);
      return ais;
   }
   
   
   public static void main(String[] args) throws IOException, UnsupportedAudioFileException
   {
      BufferedInputStream inputStream = new BufferedInputStream(new FileInputStream(args[0]));
      FileOutputStream fos = new FileOutputStream(args[1]);
       
      inputStream.mark(1024);

      System.out.println("checking for 'LAD '...");
      
      byte[] ladFcc = new byte[4];
      int readBytes = inputStream.read(ladFcc);

      String ladPerhaps = new String(ladFcc);
      System.out.println("found '" + ladPerhaps + "'.");

      if (!(ladFcc[0] == 'L' &&
            ladFcc[1] == 'A' &&
            ladFcc[2] == 'D' &&
            ladFcc[3] == ' '))
      {
         inputStream.reset();
         throw new UnsupportedAudioFileException("Audio format not supported.");
      }

      System.out.println("reading key...");

      byte[] key = new byte[892];
      readBytes = inputStream.read(key);
      
      if (readBytes != key.length)
      {
         inputStream.reset();
         throw new UnsupportedAudioFileException("Could not completely decode audio header.");
      }

      XorInputStream xorIn = new XorInputStream(inputStream, key);
      
      System.out.println("reading audio mark...");

      byte[] audioMark = new byte[128];
      readBytes = xorIn.read(audioMark);

      if (readBytes != audioMark.length)
      {
         inputStream.reset();
         throw new UnsupportedAudioFileException("Could not completely decode Lecturnity audio mark.");
      }

		String tempMark = new String(audioMark);

      int nullByteIndex = 128;
      for (int i=0; i<128; i++)
         if (tempMark.charAt(i) == ((char) 0))
         {
            nullByteIndex = i;
            break;
         }

      String markString = tempMark.substring(0, nullByteIndex);

 		System.out.println("audio mark: '" + markString + "'.");

      System.out.println("extracting data....");
      
      int iRead = 0;
      byte[] buffer = new byte[8192];
      
      iRead = xorIn.read(buffer);
      while (iRead > 0)
      {
         fos.write(buffer, 0, iRead);
         
         iRead = xorIn.read(buffer);
      }
      
      fos.close();

   }
}
