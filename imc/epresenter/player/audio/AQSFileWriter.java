package imc.epresenter.player.audio;                                                                   

import java.io.*;
import java.util.Random;
import javax.sound.sampled.*;
import javax.sound.sampled.spi.AudioFileWriter;

/*
* File:             PresentationConverter.java
* Code conventions: 18.09.2001
* Author:           Ulrich Kuhn
* Reviewed by:      Martin Danielsson (2001/10/08)
	*
* $Id: AQSFileWriter.java,v 1.13 2007-05-30 10:18:22 kuhn Exp $
*/

public class AQSFileWriter {
   private final int HAS_PADDING_CORRECTION = 1;
   private static boolean DEBUG = false;
   
   public static void main(String[] args) throws Exception {
		
      DEBUG = true;
      //System.setProperty("nyckel.musik", "forte");

      if (args.length < 2) System.out.println("Invoke with AQSFileReader <infile> <outfile>");
      else {
         /*
           AQSFileType type = new AQSFileType("", "AQS3");
           if (args[1].endsWith(".aqs4") || args[1].endsWith(".AQS4"))
           type = new AQSFileType("", "AQS4");
         */
         new AQSFileWriter().write(
                                   AudioSystem.getAudioInputStream(
                                      new BufferedInputStream(new FileInputStream(args[0]))),
                                   new File(args[1]), 3.0f, true);
      }
   }


   public AudioFileFormat.Type[] getAudioFileTypes() {
      return new AudioFileFormat.Type[] { new AQSFileType("Adaptivley Quantized Sound", "AQS") };
   }

   public AudioFileFormat.Type[] getAudioFileTypes(AudioInputStream input) {
      AudioFormat.Encoding encoding = input.getFormat().getEncoding();
      if (encoding == AudioFormat.Encoding.PCM_SIGNED || encoding == AudioFormat.Encoding.PCM_UNSIGNED)
      return getAudioFileTypes();
      else 
      return new AudioFileFormat.Type[0];
   }

   private boolean cancelRequest_;
   
   public AQSFileWriter()
   {
      super();
      cancelRequest_ = false;
   }

   /*
     public boolean isFileTypeSupported(AudioFileFormat.Type fileType) {
     }
     public boolean isFileTypeSupported(AudioFileFormat.Type fileType, AudioInputStream input) {
     }
   */

   // "legacy" method; invokes "better" method
   public int write(AudioInputStream input, AudioFileFormat.Type type,
                    OutputStream out) throws IOException
   {
      float internalBits = 3.0f;
      if (type != null && type.getExtension().equals("AQS4")) internalBits = 4.0f;
      boolean hasSeekIndex = false;
      return write(input, out, internalBits, hasSeekIndex)[0];	
   }
   
   // "better" methods
   public int write(AudioInputStream input, OutputStream out,
                    float internalBits) throws IOException {
      boolean hasSeekIndex = false;
      return write(input, out, internalBits, hasSeekIndex)[0]; 
   }
	
   public int write(AudioInputStream input, File output,
                    float internalBits, boolean hasSeekIndex) throws IOException {
		
      int[] seekIndex = write(input, new FileOutputStream(output),
                              internalBits, hasSeekIndex);

      if (hasSeekIndex)
      {
         RandomAccessFile handle = new RandomAccessFile(output, "rw");
         handle.seek(16);
         
         byte[] value = new byte[3];

         value[0] = (byte)(seekIndex.length >> 16 & 0xff);
         value[1] = (byte)(seekIndex.length >> 8 & 0xff);
         value[2] = (byte)(seekIndex.length & 0xff);

         handle.write(value); // # of entries
         
         for (int i=0; i<seekIndex.length; i++)
         {
            value[0] = (byte)(seekIndex[i] >> 16 & 0xff);
            value[1] = (byte)(seekIndex[i] >> 8 & 0xff);
            value[2] = (byte)(seekIndex[i] & 0xff);

            handle.write(value);
         }
         
         handle.close();
      }

      return (int)output.length();
   }
	
   private int[] write(AudioInputStream input, OutputStream out,
                       float internalBits, boolean hasSeekIndex) throws IOException 
   {
      
      long time1 = System.currentTimeMillis();
		
      AudioFormat format = input.getFormat();
      AudioFormat.Encoding encoding = format.getEncoding();
		
      if (!(encoding == AudioFormat.Encoding.PCM_SIGNED || encoding == AudioFormat.Encoding.PCM_UNSIGNED))
      throw new IllegalArgumentException("The encoding "+encoding+" is not supported.");
      if (input.getFrameLength() == AudioSystem.NOT_SPECIFIED) throw new UnsupportedOperationException(
                                                                                                       "Currently the length of the audio data is needed in advance, "
                                                                                                       +"so AudioSystem.NOT_SPECIFIED is not allowed.");
		
      DataOutputStream output = new DataOutputStream(new BufferedOutputStream(out));

      int[] baseFrequencies = new int[] { 8000, 11025 };
      int id = (int)'a'<<24 | (int)'q'<<16 | (int)'s'<<8 | (int)'f';
      int version = 0;
      int blockSize = (int)(256*(format.getSampleRate()/16000));
      // make blockSize dividable by 8, blockSize is written as blockSize/8
      // when writing to the stream! (Bug #1037)
      blockSize = (blockSize/8)*8;
      
      if (DEBUG)
         System.out.println("block size: "+blockSize);

      int externalBits = format.getSampleSizeInBits();
      int sampleSize = externalBits/8;
      int sampleCount = (int)((input.getFrameLength()*format.getFrameSize())/sampleSize);
      int frameCount = sampleCount/blockSize;
      boolean paddingNeeded = sampleCount%blockSize != 0;
      if (paddingNeeded) frameCount++; // the last block is padded (filled with 0)
      int seekSize = (int)((format.getSampleRate()*5)/blockSize);
      // *5 = index entry every 5 seconds
      // that makes approx. 1k index entries for a 90 minutes audio file
      int seekCount = frameCount/seekSize;
      if (frameCount%seekSize != 0) seekCount++;

      if (DEBUG)
         System.out.println("seek size: "+seekSize+" in # of blocks");
      
      if (DEBUG)
         System.out.println("#seek blocks: "+seekCount);
      int bytesPerSeek = (int)((frameCount/(float)seekCount)
                               *(internalBits/8*blockSize+1)+5);
      if (DEBUG)
         System.out.println("with bytes: "+bytesPerSeek);

      int sampleRate = (int)format.getSampleRate();
      int channels = format.getChannels();
      int externalFrameSizeInSamples = blockSize*channels;
      int hoursSince111970 = (int)(System.currentTimeMillis()/(1000*60*60)); 
      
      int flags = 0;
      if (hasSeekIndex)
         flags = HAS_PADDING_CORRECTION;
		
                          // if channels < 1
      if (channels > 1) throw new UnsupportedOperationException(
                                                                "More than one channel is currently not supported.");
      int baseFrequencyIndex = -1;
      int sampleRateMultiplier = -1;
      //if (sampleRate < 1
      for (int i=0; i<baseFrequencies.length; i++) {
         if (sampleRate%baseFrequencies[i] == 0) {
            baseFrequencyIndex = i;
            sampleRateMultiplier = sampleRate/baseFrequencies[i];
         }
      }
      if (baseFrequencyIndex == -1) throw new IllegalArgumentException(
                                                                       "Sample rate ("+sampleRate+") not supported. It must be i*8000 or i*11025.");
      // if externalBits < 1 
      if (externalBits%8 != 0) throw new IllegalArgumentException(
                                                                  "Number of bits of input ("+externalBits+") must be one of i*8.");
      if (externalBits > 16) throw new UnsupportedOperationException(
                                                                     "Currently only <= 16 bits for input supported (is "+externalBits+").");

      // write header
      BitOutputStream headbou = new BitOutputStream(output);
      headbou.writeBitString(id, 32);
      headbou.writeBitString(version, 8);
      headbou.writeBitString(hasSeekIndex ? 1 : 0, 1);
      headbou.writeBitString(baseFrequencyIndex, 4);
      headbou.writeBitString(sampleRateMultiplier, 4);
      headbou.writeBitString(externalBits/8, 3);
      headbou.writeBitString(channels, 4);
      headbou.writeBitString(blockSize/8, 8);
      headbou.writeBitString(frameCount, 32);
      headbou.writeBitString(hoursSince111970, 24);
      headbou.writeBitString(flags, 8);



      if (hasSeekIndex)
      {
         // write dummies
         for (int i=0; i<(seekCount+2); i++) // "+2": length and padding correction
            headbou.writeBitString(0, 24);
      }
		

      int writtenSamples = 0;
      int totalSamples = blockSize*channels*frameCount;
      int bitCounter = 0;
      int lastBitCounter = 0;
      int[] bitTable = new int[512]; // # of bits needed to encode i
      for (int i=0; i<bitTable.length; i++) bitTable[i] = bitsOf(i);
      int[] seekIndex = new int[seekCount + (hasSeekIndex ? 1 : 0)];

      if (hasSeekIndex)
      {
         // BUGFIX #3083:
         // store information about padding in the file; so that the true length
         // can be restored/calculated upon reading
         
         int iValidInLastBlock = blockSize;
         if (paddingNeeded)
            iValidInLastBlock = sampleCount%blockSize; 
         seekIndex[seekCount] = iValidInLastBlock;
      }

      Random rand = new Random();

      int framesLeft = frameCount;

      // in every "big" block we check for cancel requests.
      for(int k=0; k<seekCount && !cancelRequest_; k++) {
         ByteArrayOutputStream baos = new ByteArrayOutputStream();
         BitOutputStream blockbou = new BitOutputStream(baos);

         int[][] lastSamples = new int[channels][3];
         // is 0 at the beginning of every seek block
			
         int framesToWrite = Math.min(seekSize, framesLeft);
         for (int f=0; f<framesToWrite; f++) {
            int[][] channelSplitSamples = new int[channels][blockSize];
                                // must contain 0 on every iteration
				
            int read = copyAndChannelSplitSamples(input, channelSplitSamples);
            if (paddingNeeded && read < blockSize && k<seekCount-1)
               throw new RuntimeException("Unexpected underrun in sample input.");
				
            for (int c=0; c<channels; c++) {
               int[] samples = channelSplitSamples[c]; 
					
               //
               // prediction step
               //
               int[][] values = new int[4][blockSize];
               int[] error = new int[4];
               int[] sum = new int[4];
               int plast0 = lastSamples[c][2];
               int plast1 = lastSamples[c][2]-lastSamples[c][1];
               int plast2 = plast1-(lastSamples[c][1]-lastSamples[c][0]);

               for(int s=0; s<blockSize; s++) {
						
                  error[0] = samples[s];
                  error[1] = error[0]-plast0;
                  error[2] = error[1]-plast1;
                  error[3] = error[2]-plast2;
					
                  plast0 = error[0];
                  plast1 = error[1];
                  plast2 = error[2];
						
                  error[0] = error[0] < 0 ?  -1 ^ error[0] : error[0];
                  error[1] = error[1] < 0 ?  -1 ^ error[1] : error[1];
                  error[2] = error[2] < 0 ?  -1 ^ error[2] : error[2];
                  error[3] = error[3] < 0 ?  -1 ^ error[3] : error[3];
						
                  sum[0] += error[0];
                  sum[1] += error[1];
                  sum[2] += error[2];
                  sum[3] += error[3];
						
                  values[0][s] = error[0];
                  values[1][s] = error[1];
                  values[2][s] = error[2];
                  values[3][s] = error[3];
						
               } // for samples: prediction

					
               if (sum[0] == 0) { // every sample value is 0
                  blockbou.writeBitString(7, 3);

                  lastSamples[c][2] = 0;
                  lastSamples[c][1] = 0;
                  lastSamples[c][0] = 0;
						
                  writtenSamples += blockSize;

                  continue;
               }
					

               Sorty[] sorters = { 
                  new Sorty(0, sum[0], values[0]), 
                  new Sorty(1, sum[1], values[1]),
                  new Sorty(2, sum[2], values[2]), 
                  new Sorty(3, sum[3], values[3]) 
                     };

               java.util.Arrays.sort(sorters);

               //
               // coding step
               //
               int predictor = sorters[0].num;  // which predictor to choose
					


               int clast0 = lastSamples[c][2];
               int clast1 = lastSamples[c][1];
               int clast2 = lastSamples[c][0];

               // old part
               /*
                 int bits = (int)Math.round(Math.log(Math.log(2)*(sorters[0].sum/(double)blockSize))/Math.log(2));
                 int shift = 0;
                 if (bits > internalBits) shift = (int)Math.round(bits-(internalBits-3));
               */

               // new part
               
               int mid = Math.round(sorters[0].sum/(float)blockSize+0.999f);
               float LN_2 = (float)Math.log(2);
               float vxv = -(float)Math.log(5)/LN_2+(5-internalBits);
               int shift = Math.round((float)Math.log(mid)/LN_2+vxv);
               if (shift < 0) shift = 0;
               
               /*
                 else if (shift > 0 && writtenSamples > 0) {
                 float meanBits = bitCounter/(float)writtenSamples;
                 if (meanBits < internalBits)
                 if (rand.nextFloat() > 1-(internalBits-meanBits))
                 shift--;
                 }
                 if (sorters[0].sum < blockSize) System.out.println(shift);
               */



               // quantize and round samples
               if (shift > 0) {
                  int rounder = (1 << (shift-1)) - 1;
                  for (int s=0; s<blockSize; s++) 
                     samples[s] = (samples[s]+rounder) >> shift;
               }

               blockbou.writeBitString(predictor, 3);
               blockbou.writeBitString(shift, 5);

               // predict and code values
               for(int s=0; s<blockSize; s++) {
                  int prediction = 0;
                  switch(predictor) {
                  case 0:
                     break;
                  case 1:
                     prediction = clast0;
                     break;
                  case 2:
                     prediction = 2*clast0-clast1;
                     break;
                  case 3:
                     prediction = 3*clast0-3*clast1+clast2;
                     break;
                  }

                  int value = samples[s]-prediction; // error

                  // write value in pod encoding
                  int sign = 0;
                  if (value < 0) { 
                     value ^= -1; 
                     sign = 1; 
                  }
                  int length = 0;
                  if (value < bitTable.length) length = bitTable[value];
                  else length = bitsOf(value);
                  if (value == 0) {
                     blockbou.writeBitString(1, 1);
                  } else {
                     blockbou.writeBitString(0, length);
                     blockbou.writeBitString(value, length);
                  }
                  blockbou.writeBitString(sign, 1);



                  if (value == 0) bitCounter += 2;
                  else bitCounter += 2*length+1;

                  clast2 = clast1; clast1 = clast0; clast0 =samples[s];

               } // for samples: coding
              
               int diff = bitCounter - lastBitCounter;
               //System.out.println(" bits="+(diff/(float)blockSize));

               lastBitCounter = bitCounter;

               /*
                 if (variance/(float)sorters[0].sum > 6) {
                 for(int v=0; v<sorters[0].values.length; v++) System.out.print(sorters[0].values[v]+" ");
                 System.out.println();
                 }
               */


               lastSamples[c][2] = samples[blockSize-1];
               lastSamples[c][1] = samples[blockSize-2];
               lastSamples[c][0] = samples[blockSize-3];
		


               writtenSamples += blockSize;
					
            } // for channels


         } // for frames in seek block

         blockbou.flushBufferedBits();
         byte[] seekBlock = baos.toByteArray();
         if (seekBlock.length > ((1 << 20) -1)) throw new RuntimeException(
                                                                           "Ooops! Number of bytes in compressed seek block ("+seekBlock.length+")"
                                                                           +" is greater than the encodable maximum ("+((1 << 20) -1)+")."); 

         // write seek block header
         // the length informations (also in total) are used in:
         // AQSInputStream.readSeekHeader()
         // AQSInputStream.skip() firstSkip part
         headbou.writeBitString(framesToWrite, 12);
         headbou.writeBitString(0xAA, 8); // verify string; used during decoding
         headbou.writeBitString(seekBlock.length, 20);

         headbou.write(seekBlock);
         


         seekIndex[k] = seekBlock.length;



         framesLeft -= framesToWrite;

      } // for seek blocks

      headbou.close();

      long time2 = System.currentTimeMillis();
		
      //int writtenSamples = blockSize*channels*frameCount;

      if (DEBUG)
      {
         System.out.println();
         System.out.println("internal bit size: "+internalBits);
         System.out.println("#bits per sample: "+(bitCounter/(float)sampleCount));
         System.out.println("total written #samples: "+writtenSamples);
         System.out.println("used time [ms]: "+(time2-time1));
      }

      //return output.size();
      if (hasSeekIndex)
      return seekIndex;
      else
      return new int[] { output.size() };
   }

   public void cancelWrite()
   {
      cancelRequest_ = true;
   }
	
   private int bitsOf(int x) {
      int size = 32;
      for (; size > 0; size--) if (((1 << (size-1)) & x) != 0) break;
      return size;
   }

   private int copyAndChannelSplitSamples(AudioInputStream input, int[][] samples) throws IOException {
      AudioFormat format = input.getFormat();
      int sampleSize = format.getSampleSizeInBits()/8;
      int channels = format.getChannels();
      int blockSize = samples[0].length;
      byte[] data = new byte[blockSize*sampleSize*channels];

      int read = 0;
      while(read < data.length) {
         int r = input.read(data, read, data.length-read);
         if (r == -1) break;
         read += r;
      }
      if (read == 0) return -1;
      if (read % (channels*sampleSize) != 0) throw new RuntimeException(
                                                                        "Ooops! Number of read bytes ("+read+") is not evenly divisable by"
                                                                        +" channels*sampleSize ("+(channels*sampleSize)+").");
				
      for (int c=0; c<channels; c++) {
         for (int i=0; i<blockSize; i++) {
            int idx = i*channels+c;
            if (sampleSize == 1) {
               if (format.getEncoding() != AudioFormat.Encoding.PCM_SIGNED) {
                  samples[c][i] = data[idx] < 0 ? data[idx]+128 : data[idx]-128;
               } else {
                  samples[c][i] = data[idx];
               }
               // shouldn't the above read
               // if SIGNED sampes = data
               // else samples = data-128
               // ????
            } else {
               // why here no check on SIGNED and UNSIGNED ???

               if (format.isBigEndian()) samples[c][i] = data[2*idx]<<8 | (data[2*idx+1]&0xff);
               else samples[c][i] = data[2*idx+1]<<8 | (data[2*idx]&0xff);
            }
         }
      }

      return read/(sampleSize*channels);
   }

   private class Sorty implements Comparable {
      int num; // predictor number
      int sum; // sum of the values to be encoded 
      int[] values;

      Sorty(int n, int s, int[] vs) {
         num = n;
         sum = s;
         values = vs;
      }

      public int compareTo(Object o) {
         Sorty s = (Sorty)o;
         if (this.sum < s.sum) return -1;
         else if(this.sum == s.sum)
            if (this.num < s.num) return -1;
            else return 1;
         else return 1;
      }
   }

}

