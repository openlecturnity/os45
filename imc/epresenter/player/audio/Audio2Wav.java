package imc.epresenter.player.audio;

import java.io.*;
import javax.sound.sampled.*;

/**
 * @deprecated Now done in SoundPlayer.decompressAll().
 */
public class Audio2Wav {
   public static void convert(String file1, String file2)  throws Exception
   {
//      try
//      {
         main(new String[] { file1, file2 });
//      }
//      catch (Exception e)
//      {
//         e.printStackTrace();
//      }
   }

   public static void main(String[] args) throws Exception
   {
      InputStream fileInput = 
         (new BufferedInputStream(new FileInputStream(new File(args[0]))));

		AudioInputStream input = null;
      try
      {
		   input = AudioSystem.getAudioInputStream(fileInput);
		}
		catch (UnsupportedAudioFileException e)
		{
		   if (args[0].toLowerCase().endsWith(".aif"))
		   {
            fileInput = 
               (new BufferedInputStream(new FileInputStream(new File(args[0]))));
            AiffFileReader aiffFileReader = new AiffFileReader();
		      input = aiffFileReader.getAudioInputStream(fileInput);
		   }
		   else
		      throw e;
		}

      AudioFormat format = input.getFormat();

		int channels = format.getChannels();
		int bits = format.getSampleSizeInBits();
		int sampleSize = bits/8;
		int sampleRate = (int)format.getSampleRate();
		int sampleCount = (int)((input.getFrameLength()*format.getFrameSize())/sampleSize);
      
      int lengthInBytes = sampleCount*sampleSize;
      int bytesPerSecond = sampleRate*sampleSize;

		//System.out.println("Input claims to have #channels="+channels+" #bits="+bits
		//				   +" sampleRate="+sampleRate+" #samples="+sampleCount);
		
		
		DataOutputStream output = new DataOutputStream(
         new BufferedOutputStream(new FileOutputStream(args[1])));

      // write WAVE-RIFF header
      output.write(new byte[] { (byte)'R', (byte)'I', (byte)'F', (byte)'F' });
      writeBEInt(output, lengthInBytes+36); // 36 = header length
      output.write(new byte[] { (byte)'W', (byte)'A', (byte)'V', (byte)'E' });

      output.write(new byte[] { (byte)'f', (byte)'m', (byte)'t', (byte)' ' }); 
      output.write(new byte[] { (byte)16, (byte)0, (byte)0, (byte)0 }); // length of fmt-tag
      writeBEShort(output, (short)1); // = PCM
      writeBEShort(output, (short)format.getChannels()); // = channels
      writeBEInt(output, sampleRate);
      writeBEInt(output, bytesPerSecond);
      writeBEShort(output, (short)(bits/8)); // sample size in bytes
      writeBEShort(output, (short)bits); // sample size in bits
      output.write(new byte[] { (byte)'d', (byte)'a', (byte)'t', (byte)'a' });
      writeBEInt(output, lengthInBytes);

		
		long time1 = System.currentTimeMillis();
		
		int[] samples = new int[256];
		int counter = 0;
		int read = extractSamples(input, samples);
		while (read != -1) {
			//System.out.print("r");
			writeSamples(output, samples, read, sampleSize);
			//System.out.print("w");
			counter += read;
			read = extractSamples(input, samples);
		}
		//System.out.println();

		output.flush();
		output.close();
        
		long time2 = System.currentTimeMillis();

		//System.out.println("sound samples written: "+counter);
		//System.out.println("used time [ms]: "+(time2-time1));
	}


    static int extractSamples(AudioInputStream ais, int[] samples) throws IOException {
        int count = samples.length;
		int sampleSize = ais.getFormat().getSampleSizeInBits()/8;

        byte[] data = new byte[count*sampleSize];
        int read = 0;
        while(read < data.length) {
			int r = ais.read(data, read, data.length-read);
			if (r == -1) break;
			read += r;
		}

		if (sampleSize > 1) {
			if (read % sampleSize != 0) 
				System.err.println("Ooops! Unappropriate number of bytes in Input!");
			read /= sampleSize;
		}

        for (int i=0; i<read; i++) {
            if (sampleSize == 1) {
                if (ais.getFormat().getEncoding() != AudioFormat.Encoding.PCM_SIGNED) {
                    samples[i] = data[i] < 0 ? data[i]+128 : data[i]-128;
                } else {
                    samples[i] = data[i];
                }
            } else {
                // seems to be wrong, but generates the right results !?
                if (ais.getFormat().isBigEndian()) samples[i] = data[2*i]<<8 | (data[2*i+1]&0xff);
                else samples[i] = data[2*i+1]<<8 | (data[2*i]&0xff);
            }
			//if (System.getProperty("app.debug") != null)
			//	System.out.print(samples[i]+" ");
        }

		if (read == 0) return -1;
        return read;
    }

	private static void writeSamples(OutputStream out, int[] samples, int length, int sampleSize) throws IOException {
	    for (int s=0; s<length; s++) {
			if (sampleSize == 1) { // 8 bit unsigned
				out.write(samples[s]+128);
			} else { // 16 bit signed; big endian (intel)
				byte[] el = new byte[2];
				el[0] = (byte)samples[s];
				el[1] = (byte)(samples[s] >> 8);
				out.write(el);
			}
		}
	}

   private static void writeBEShort(DataOutputStream out, short s) throws IOException {
      out.writeByte((byte)s);
      out.writeByte((byte)(s >> 8));
   }

   private static void writeBEInt(DataOutputStream out, int i) throws IOException {
      out.writeByte((byte)i);
      out.writeByte((byte)(i >> 8));
      out.writeByte((byte)(i >> 16));
      out.writeByte((byte)(i >> 24));
   }

}