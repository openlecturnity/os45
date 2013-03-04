package imc.epresenter.player.audio;                                                                   

import java.io.*;
import javax.sound.sampled.*;

public class AQSInputStream extends InputStream {
	private BitInputStream input;
	private CounterInputStream counter;
	private int frameSize; // in samples !
	private int channels;
	private int sampleSize; // in bytes
   private int[] seekIndex;
   private int seekSize; // # blocks per seek block
   private boolean firstSkip = true;
	
	private byte[] buffer;
	private int readPosition;
	private int[][] lastSamples; // for prediction during one seek block

	// informations from the current (last parsed) seek block:
	private int frameCounter;
	private int framesInSeek;
	private int bytesInSeekBlock;

   // workaround for skip() wants to do readSeekHeader()
   private boolean noMoreSeekBlocks;
	
	public AQSInputStream(InputStream in, int ssize, int chan,
                         int fsize, int[] index, int seekSize) throws IOException {
		sampleSize = ssize;
		channels = chan;
		frameSize = fsize;
      seekIndex = index;
      this.seekSize = seekSize;
      
		counter = new CounterInputStream(in);
		input = new BitInputStream(counter);
		
		buffer = new byte[channels*frameSize*sampleSize]; // decoded samples of one block
		readPosition = buffer.length; // the buffer is empty initialy
		lastSamples = new int[channels][3];

		readSeekHeader(); // read() AND skip() need this information
	}

	public int available() {
		return buffer.length-readPosition;
	}

	public int read() throws IOException {
      if (noMoreSeekBlocks) return -1;

		if (readPosition == buffer.length) if (!fillBuffer()) return -1;
		int v = buffer[readPosition++];
		return v < 0 ? v+256 : v;
	}

	public int read(byte[] array) throws IOException {
		return read(array, 0, array.length);
	}

	public int read(byte[] array, int off, int len) throws IOException {
      if (noMoreSeekBlocks) return -1;
		
      int copiedBytes = 0;
		while(copiedBytes < len) {
			// fill buffer if needed; if that is not possible and copiedByte == 0 return -1
			if (readPosition == buffer.length) if (!fillBuffer()) break;
			
			int toCopy = Math.min(buffer.length-readPosition, len-copiedBytes);
			System.arraycopy(buffer, readPosition, array, off+copiedBytes, toCopy);
			readPosition += toCopy;
			copiedBytes += toCopy;
		}
		if (copiedBytes == 0) return -1;
		else return copiedBytes;
	}

   /**
    * @returns the number of bytes skipped in the raw (decompressed)
    *          AudioInputStream; whereas skipping is done in the compressed
    *          InputStream
    */
	public long skip(long n) throws IOException {
      
      // the code below is complicated enough
      // so I added this special case:
      //firstSkip = false;
      if (firstSkip)
      {
         firstSkip = false;

         if (seekIndex != null && n > bytesInSeekBlock)
         {
            int sample = (int)(n/sampleSize);

            // find seekBlock with this sample
            int samplesPerSeek = seekSize*frameSize;
            int idx = sample / samplesPerSeek;

            if (idx > 0)
            {
               long skipAmount = -5; // first seek header is already read
               for (int i=0; i<idx; i++)
                  skipAmount += seekIndex[i]+5;
              
               long skipped = 0;
					while ((skipped += input.skip(skipAmount-skipped)) < skipAmount);
               
               noMoreSeekBlocks = !readSeekHeader();
               
               return idx*(samplesPerSeek*sampleSize);

				} // else use the below code
         }
      }


		int bytesInBuffer = buffer.length-readPosition;
		if (bytesInBuffer == 0) { // read new buffer or skip entire frames or seek blocks
			if (n <= buffer.length) {
				if (fillBuffer()) {
					//System.out.println("filled buffer and skipped "+n+" bytes");
					readPosition += n;
					return n;
				} else { // no bytes to skip left in file
					return 0; // ??? correct behaviour ???
				}
			} else { // skip some frames or an entire seek block
				int framesToSkip = (int)(n/buffer.length);
				int framesLeftInSeek = framesInSeek-frameCounter;
				if (framesToSkip < framesLeftInSeek) { // "skip" entire frames
					for (int i=0; i<framesToSkip; i++) if (!fillBuffer()) return i*buffer.length;
					//System.out.println("read and skipped "+framesToSkip
					//				   +" full frames with "+(framesToSkip*buffer.length)+" bytes");
					return framesToSkip*buffer.length;
				} else { // skip a full (or the portion not yet read) seek block
					input.discardBufferedBits();
					long bytesLeft = bytesInSeekBlock-counter.bytesProcessed();
					long skipped = 0;
					while ((skipped += input.skip(bytesLeft-skipped)) < bytesLeft);
					//System.out.println("skipped in compressed stream "+skipped
					//					   +" with "+counter.bytesProcessed()+" bytes processed");
					noMoreSeekBlocks = !readSeekHeader();
					//System.out.println("skipped the entire seek block with "
					//				   +(framesLeftInSeek*buffer.length)+" bytes");
					return framesLeftInSeek*buffer.length;
				}
			}
		} else { // some bytes to skip are already buffered
			if (n <= bytesInBuffer) {
				//System.out.println("skipped inside the buffer the "+n+" bytes");
				readPosition += n;
				return n;
			} else {
				//System.out.println("emptied buffer and skipped thereby "+bytesInBuffer+" bytes");
				readPosition += bytesInBuffer;
				return bytesInBuffer;
			}
		}
	}

	private boolean fillBuffer() throws IOException {
		try {
			if (frameCounter == framesInSeek) {
				input.discardBufferedBits();
				readSeekHeader();
			}

			for (int c=0; c<channels; c++) {
           
				int predictor = input.readBitString(3);

				if (predictor == 7) { // every sample in this block is 0
					if (channels == 0) {
						java.util.Arrays.fill(buffer, (byte)0);
					} else {
						for (int s=0; s<frameSize; s++) {
							int idx = s*channels+c;
							if (sampleSize == 1) {
								buffer[idx] = (byte)128; // unsigned 8 bits
							} else {
								buffer[2*idx] = (byte)0; // signed 16 bits
								buffer[2*idx+1] = (byte)0;
							}
						}
					}

					lastSamples[c][2] = 0;
					lastSamples[c][1] = 0;
					lastSamples[c][0] = 0;

					continue;
				}
			
				

				int shift = input.readBitString(5);

				int last0 = lastSamples[c][2];
				int last1 = lastSamples[c][1];
				int last2 = lastSamples[c][0];

				for (int s=0; s<frameSize; s++) {
					int sample = 0;

					// read pod value
					int value = 0;
					int length = 0;
					while(input.readBitString(1) == 0) length++;
					if (length == 1) value = 1;
					else if (length > 1) value = input.readBitString(length-1) | (1 << (length-1));
					if (input.readBitString(1) != 0) value ^= -1;
					/* not faster:
					if (length == 0) {
						if (input.readBitString(1) != 0) value = -1;
					} else {
						int remainder = input.readBitString(length);
						value = (1 << (length-1)) | (remainder >> 1);
						if ((remainder & 1) != 0) value ^= -1;
					}
					*/
					

					int prediction = 0;

					switch(predictor) {
						case 0:
							break;
						case 1:
							prediction = last0;
							break;
						case 2:
							prediction = 2*last0-last1;
							break;
						case 3:
							prediction = 3*last0-3*last1+last2;
							break;
					}

					sample = prediction + value;

					last2 = last1; last1 = last0; last0 = sample;

					// shift
					if (shift > 0) {
						sample = sample << shift;
						if (sampleSize == 1) {
							if (sample > 127) sample = 127;
							if (sample < -128) sample = -128;
						} else {
							if (sample > 32767) sample = 32767;
							if (sample < -32768) sample = -32768;
						}
					}

					// write to buffer
					int idx = s*channels+c;
					if (sampleSize == 1) {
						buffer[idx] = (byte)(sample+128); // unsigned 8 bits
					} else {
						buffer[2*idx] = (byte)sample; // signed 16 bits
						buffer[2*idx+1] = (byte)(sample >> 8);
					}

            } // for samples: decode

            
				lastSamples[c][2] = last0;
				lastSamples[c][1] = last1;
				lastSamples[c][0] = last2;
			} // for channels
		} catch (IOException e) {
			if (e instanceof EOFException) return false;
			else throw e;
		}

		frameCounter++;
		readPosition = 0;
		return true;
	}

	private boolean readSeekHeader() throws IOException {
      try {
         framesInSeek = input.readBitString(12);
         int stopCode = input.readBitString(8);
         if (stopCode != 0xAA) throw new RuntimeException(
            "Found stopCode "+stopCode+" in seek header! File corrupt?");
         bytesInSeekBlock = input.readBitString(20);

         lastSamples = new int[channels][3];
         counter.resetCounter();

         frameCounter = 0;
      } catch (IOException e) {
         if (e instanceof EOFException) return false;
			else throw e;
		}

      return true;
	}
}

