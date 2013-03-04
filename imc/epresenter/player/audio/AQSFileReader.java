package imc.epresenter.player.audio;                                                                   

import java.io.*;
import java.net.URL;
import javax.sound.sampled.*;
import javax.sound.sampled.spi.AudioFileReader;

public class AQSFileReader extends AudioFileReader {
   private final int HAS_PADDING_CORRECTION = 1;
   
    private int id = (int)'a'<<24 | (int)'q'<<16 | (int)'s'<<8 | (int)'f';
    private int[] baseRates = { 8000, 11025 };

    private int frameSize = 0;
    private int frameCount = 0;
    private long frameCountUncompressed = 0;
    private int[] seekIndex;
    private int seekSize;

    public AudioFileFormat getAudioFileFormat(File file) throws UnsupportedAudioFileException, IOException {
        return getAudioFileFormat(new BufferedInputStream(new FileInputStream(file)));
    }
    
    public AudioFileFormat getAudioFileFormat(URL url) throws UnsupportedAudioFileException, IOException {
        return getAudioFileFormat(new BufferedInputStream(url.openStream()));
    }
    
    public AudioFileFormat getAudioFileFormat(InputStream input) throws UnsupportedAudioFileException, IOException {
       return getAudioFileFormat(input, true);
    }

    public AudioFileFormat getAudioFileFormat(InputStream input, boolean closeIt) throws UnsupportedAudioFileException, IOException {
       if (input.markSupported())
           input.mark(4);
       else
          throw new IOException("InputStream does not support mark(); which is a must.");
       BitInputStream bin = new BitInputStream(input);

       // read header
       int fileId = bin.readBitString(32);
       if (fileId != id)
       {
          input.reset();
          throw new UnsupportedAudioFileException("Id "+fileId+" not recognized.");
       }
       int version = bin.readBitString(8);
       boolean hasSeekIndex = bin.readBitString(1) == 1;
       int sampleRate = baseRates[bin.readBitString(4)]*bin.readBitString(4);
       int externalBits = bin.readBitString(3)*8;
       int sampleSize = externalBits/8; // in bytes
       int channels = bin.readBitString(4);
       frameSize = bin.readBitString(8)*8; // # of samples per block
       frameCount = bin.readBitString(32); // # of blocks in total; may not be -1
       int hoursSince111970 = bin.readBitString(24);

       int flags = bin.readBitString(8);
       
       boolean bHasPaddingCorrection = false;
       if ((flags & HAS_PADDING_CORRECTION) != 0)
          bHasPaddingCorrection = true;
       
       int iValidInLastBlock = 0;
       
        seekSize = (int)((sampleRate*5)/(float)frameSize);
        // see AQSFileWriter for where this formula comes from

        if (hasSeekIndex)
        {
           int numEntries = bin.readBitString(24);
           if (bHasPaddingCorrection)
              numEntries--;
           seekIndex = new int[numEntries];

           for (int i=0; i<numEntries; i++)
              seekIndex[i] = bin.readBitString(24);
           
           if (bHasPaddingCorrection)
              iValidInLastBlock = bin.readBitString(24);
        }




        // format inherent things
        AudioFormat.Encoding encoding = null;
        if (externalBits == 8) encoding = AudioFormat.Encoding.PCM_UNSIGNED;
        else encoding = AudioFormat.Encoding.PCM_SIGNED;
        boolean bigEndian = false;

        if (closeIt)
           input.close();

        AudioFormat format =  new AudioFormat(encoding, sampleRate, externalBits, channels,
                                              sampleSize*channels, sampleRate, bigEndian);
        
        frameCountUncompressed = AudioSystem.NOT_SPECIFIED;
        if (frameCount != -1)
        {
           frameCountUncompressed = (long)frameSize*(long)frameCount;
           
           if (bHasPaddingCorrection)
           {
              // BUGFIX #3083:
              // the true length is different from frameSize*frameCount
              // due to padding
         
              frameCountUncompressed -= (frameSize - iValidInLastBlock);
           }
        }
        
        
        return new AudioFileFormat(new AQSFileType("AQS", "Adaptivley Quantized Sound"), 
                                   format, (int)frameCountUncompressed);

   }

    public AudioInputStream getAudioInputStream(File file) throws UnsupportedAudioFileException, IOException {
        return getAudioInputStream(new BufferedInputStream(new FileInputStream(file)));
    }
    
    public AudioInputStream getAudioInputStream(URL url) throws UnsupportedAudioFileException, IOException {
        return getAudioInputStream(new BufferedInputStream(url.openStream()));
    }
    
    public AudioInputStream getAudioInputStream(InputStream input) throws UnsupportedAudioFileException, IOException {
       //System.out.println("try");
        
       // sets some instance variables by side effect
       AudioFormat externalFormat = getAudioFileFormat(input, false).getFormat();

       int sampleSizeInBytes = externalFormat.getSampleSizeInBits()/8;
       int channels = externalFormat.getChannels();
        
       AQSInputStream aqsIn = new AQSInputStream(input, sampleSizeInBytes,
                                                 channels, frameSize,
                                                 seekIndex, seekSize); 

       return new AudioInputStream(aqsIn, externalFormat, frameCountUncompressed);
    }

}