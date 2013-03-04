package imc.epresenter.player.audio;

import java.io.*;
import java.net.URL;
import java.util.*;
import java.util.zip.*;
import java.awt.Label;
import javax.sound.sampled.*;
import javax.swing.JOptionPane;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.FileStruct;
import imc.epresenter.filesdk.ResourceNotFoundException;
import imc.epresenter.player.Manager;
import imc.epresenter.player.util.TimeFormat;
import imc.epresenter.player.util.XmlTag;
import imc.epresenter.player.audio.*;


public class SoundPlayer implements LineListener {
   static boolean DEBUG = false;
   static
   {
      if (System.getProperty("app.sounddebug") != null)
         DEBUG = true;
   }
	
	//private URL audioLocation;
   private HashMap inputStreams_;
	private FileResources resources_;
	private String audioLocation_;
   private AudioInputStream input_;
   
   // This variable new for audio retrieval (decompressAll()); most older methods
   // redefine a local format variable. Didn't have the time/bravery to 
   // change them to this instance variable.
   private AudioFormat format_;
   
	private SourceDataLine output_;
	private AudioTransporter transporter_;
	private EOFListener eofListener_;
	
	private int audioDuration_;
   private int startOffset_;
	private int startTime_;
	private int failMillis_; // microsecondPosition is not resetted on stop()
   boolean m_bSetActive = false;
	

	private boolean paused_;
	private boolean started_;
	private boolean outputClosed_;
	private boolean stopReceived_;
   private boolean m_bFirstAudioOpening;

	private HashMap controlTypes_;
	private HashMap controlValues_;
	

	public SoundPlayer(FileResources resources, String location, EOFListener eofl) throws IOException, LineUnavailableException, UnsupportedAudioFileException {
		boolean bConsoleOperation = System.getProperty("mode.consoleoperation", "false").equals("true");
      
      eofListener_ = eofl;
		
		//audioLocation = audio;
		resources_ = resources;
		audioLocation_ = location;

      inputStreams_ = new HashMap();
      
		AudioInputStream input = null;

		
		input = retrieveInputAndSkip();
      if (!bConsoleOperation)
         output_ = retrieveOutput(input);
		
		format_ = input.getFormat();
		float audioSeconds = input.getFrameLength()/format_.getSampleRate();
		audioDuration_ = (int)(audioSeconds*1000);

      if (output_ != null)
         output_.addLineListener(this);
		stopReceived_ = true;
      
      m_bFirstAudioOpening = true;

		controlTypes_ = new HashMap();
		controlValues_ = new HashMap();
		// add all usefull controlls
		controlTypes_.put("VOLUME", FloatControl.Type.VOLUME);
		controlTypes_.put("MASTER_GAIN", FloatControl.Type.MASTER_GAIN);
		controlTypes_.put("RATE", FloatControl.Type.SAMPLE_RATE);
      controlTypes_.put("MUTE", BooleanControl.Type.MUTE);
      
      // the validness of the control types is checked in start()
      // as since Java 1.5 only opened output lines expose control types
        
      closeFileInput(input);
	}

	private AudioInputStream retrieveInputAndSkip() throws IOException, UnsupportedAudioFileException {
		AudioInputStream input = null;
      
      InputStream in = new BufferedInputStream(
			resources_.createInputStream(audioLocation_), 16384);
      try
      {
         input = AudioSystem.getAudioInputStream(in);
      } catch (UnsupportedAudioFileException e) {
         File file = resources_.getFile(audioLocation_);
         if (file != null)
         {
            input = new AiffFileReader().getAudioInputStream(
               new BufferedInputStream(new FileInputStream(file.getAbsolutePath()), 16384));
         }
         /*
         System.out.println("FILE!");
         if (file != null) input = AudioSystem.getAudioInputStream(file);
         */
         else throw e;
      }
      


		AudioFormat format = input.getFormat();
		
		// others than PCM must be converted first (FormatConversionProvider)
		// this works for ULAW, ALAW and GSM (with plugin)
		// may not work for other encodings (due to sample and frame size 'hack')
		if ((format.getEncoding() != AudioFormat.Encoding.PCM_SIGNED) &&
			(format.getEncoding() != AudioFormat.Encoding.PCM_UNSIGNED)) {
			
			AudioFormat conv = null;
			if (format.getSampleSizeInBits() == 8) { // ULAW, ALAW
				conv = new AudioFormat(
					AudioFormat.Encoding.PCM_SIGNED, 
					format.getSampleRate(),
					format.getSampleSizeInBits() * 2,
					format.getChannels(),
					format.getFrameSize() * 2,
					format.getFrameRate(),
					true);
			} else {
				conv = new AudioFormat(
					AudioFormat.Encoding.PCM_SIGNED,
					format.getSampleRate(),
					16,
					format.getChannels(),
					format.getChannels() * 2,
					format.getSampleRate(),
					false);
			}
                        
			input = AudioSystem.getAudioInputStream(conv, input);
			format = conv;
		}
                

		if (startTime_ != 0 || startOffset_ > 0) {
         //long time1 = System.currentTimeMillis();
         
         int toSkipMillis = startTime_ > startOffset_ ? startTime_ : startOffset_;

			int frameSize = format.getFrameSize();
         long maximumBytes = input.getFrameLength()*frameSize;
			long skipBytes = (long)(format.getFrameRate()*frameSize*toSkipMillis/1000.0F);
			skipBytes = (skipBytes/frameSize)*frameSize;
         if (skipBytes > maximumBytes) skipBytes = maximumBytes;
			long skipped = 0;
			while (skipped < skipBytes)
				skipped += input.skip(skipBytes-skipped);
         
         //long time2 = System.currentTimeMillis();
      }

      inputStreams_.put(input, in);
		return input;
	}

	private SourceDataLine retrieveOutput(AudioInputStream ais) throws IOException, LineUnavailableException {
		AudioFormat format = ais.getFormat();
		// total byte length of input
		int byteLength = (int)(ais.getFrameLength()*format.getFrameSize());
		DataLine.Info info = new DataLine.Info(
			SourceDataLine.class, format, byteLength); 
		SourceDataLine sdl = (SourceDataLine)AudioSystem.getLine(info);
		
		return sdl;
	}
   
   private void closeFileInput(AudioInputStream key)
   {
      try
      {
         key.close();
         if (inputStreams_.get(key) != null)
            ((InputStream)inputStreams_.get(key)).close();
         inputStreams_.put(key, null);
      }
      catch (IOException exc)
      {
      }
   }

   public synchronized void setStartOffset(int millis)
   {
      startOffset_ = millis;
   }

	
	public synchronized void start() {
		//System.out.println("SP: audio starting...");
		if (started_) return;

		// for every start: rebuild input
		AudioInputStream input = null;
		try {
			input = retrieveInputAndSkip();
		} catch (IOException ioe) {
			ioe.printStackTrace();
			return;
		} catch (UnsupportedAudioFileException uae) {
			// should never happen; checked in constructor
			uae.printStackTrace();
		}

		if (outputClosed_) { // once closed output_ must be rebuild
			Object placeholder = null;
			try {
				output_ = retrieveOutput(input);
			} catch (Exception e) {
				// should never happen; done in the constructor
				e.printStackTrace();
			}
			output_.addLineListener(this);

			outputClosed_ = false;
			stopReceived_ = true;
		}

		if (!stopReceived_ && !paused_) { //we shan't start until properly stoped
			try { wait(); } catch(InterruptedException ie) {}
		}

		if (!output_.isOpen()) {
			try {
				AudioFormat format = input.getFormat();
				float bytesPerSecond = format.getFrameSize()*format.getFrameRate();
				int bufferSize = (int)(bytesPerSecond*0.25F);
            if (DEBUG)
               System.out.println("Line bufferSize = "+bufferSize);
				output_.open(format, bufferSize);
            
            if (m_bFirstAudioOpening)
            {
               m_bFirstAudioOpening = false;
               
               // now check if the controls are valid
               // and if yes get (and put - remember) the value or reset it if already specified
               Iterator iter = controlTypes_.keySet().iterator();
               while(iter.hasNext()) {
                  try {
                     Object typeString = iter.next();
                     Control control = output_.getControl((Control.Type)controlTypes_.get(typeString));
                     
                     Object valueObject = controlValues_.get(typeString);
                     
                     boolean bValueAlreadySet = valueObject != null;
                     
                     if (control instanceof FloatControl)
                     {
                        if (!bValueAlreadySet)
                           valueObject = new Float(((FloatControl)control).getValue());
                        else
                           setControlValue((String)typeString, ((Float)valueObject).floatValue());
                     }
                     else // BooleanControl
                     {
                        if (!bValueAlreadySet)
                           valueObject = new Boolean(((BooleanControl)control).getValue());
                        else
                        {
                           Boolean b = (Boolean)valueObject;
                           if (b.booleanValue())
                              setControlValue((String)typeString, 1.0f);
                           else
                              setControlValue((String)typeString, 0.0f);
                        }
                     }

                     controlValues_.put(typeString, valueObject);
                  } catch (IllegalArgumentException e) {
                     iter.remove();
                  }
               }
            }
   
         

				// always set the control values on the opened output_
				Iterator iter = controlTypes_.keySet().iterator();
            while(iter.hasNext()) {
					Object typeString = iter.next();
               // bugaround (gain resets mute):
               if (typeString.equals("MASTER_GAIN") || typeString.equals("MUTE"))
                   continue; // postponed
					Control control = output_.getControl((Control.Type)controlTypes_.get(typeString));
               if (control instanceof FloatControl)
                  setFloatControlValue((FloatControl)control, ((Float)controlValues_.get(typeString)).floatValue());
               else // BooleanControl
                  ((BooleanControl)control).setValue(
                     ((Boolean)controlValues_.get(typeString)).booleanValue());
            }
            // bugaround (gain resets mute):
            if (controlTypes_.get("MASTER_GAIN") != null)
            {
               FloatControl c = ((FloatControl)output_.getControl((Control.Type)controlTypes_.get("MASTER_GAIN")));
               float v = ((Float)controlValues_.get("MASTER_GAIN")).floatValue();
               setFloatControlValue(c, v);
            }
            if (controlTypes_.get("MUTE") != null)
            {
               ((BooleanControl)output_.getControl((Control.Type)controlTypes_.get("MUTE"))).setValue(
                  ((Boolean)controlValues_.get("MUTE")).booleanValue());
            }

			} catch (LineUnavailableException lue) {
				Manager.showError(Manager.getLocalized("audioUnav")+"\n"+lue, Manager.WARNING, lue);
				return;
			}
		}

		output_.start();
		transporter_ = new AudioTransporter(input, output_, eofListener_);
		transporter_.startOnce();

		paused_ = false;
		started_ = true;
		//System.out.println("SP: audio started_.");

      input_ = input;
	}

	public synchronized void pause() {
		//System.out.println("SP: audio pausing...");
		if (paused_) return;

		transporter_.stopForever();
		output_.stop();
		output_.flush();

		paused_ = true;
		started_ = false;
		//System.out.println("SP: audio paused_.");
	}

	public synchronized void stop() {
		//System.out.println("SP: audio stoping...");
		if (paused_) throw new RuntimeException("Not able to stop in paused_ mode!");
		if (!started_) return;

		transporter_.stopForever();
		//System.out.println("SP: trying to output_ stop");
		output_.stop();
		startTime_ = getMediaTime();
		output_.close();
		failMillis_ = 0;
		outputClosed_ = true;

      closeFileInput(input_); 

		started_ = false;
		//System.out.println("SP: audio stoped.");
	}


   public synchronized void setMediaTime(int millis) {
      m_bSetActive = true;
		if (millis >= 0 && millis <= audioDuration_) {
         startTime_ = millis;
			
         boolean wasActive = started_;
			if (wasActive) pause();

			if (output_.isOpen())
				failMillis_ = (int)(output_.getMicrosecondPosition()/1000);
         
         if (wasActive) start();
      }
      m_bSetActive = false;
	}

	public int getMediaTime() {
		int time = startTime_;
     
		if (output_.isOpen() && !m_bSetActive)
      	time += (int)(output_.getMicrosecondPosition()/1000)-failMillis_;
      
      if (time < startOffset_)
         time = startOffset_;
       
      if (time > audioDuration_ - 1)
         time = audioDuration_ - 1;
      
      return time;
	}

	public int getDuration() {
		return audioDuration_ - 1;
	}

	public boolean controlSupported(String identifier) {
		return controlTypes_.containsKey(identifier);
	}

	public void setControlValue(String identifier, float value) {
      Object valueObject = null;
      Control.Type type = (Control.Type)controlTypes_.get(identifier);
      if (type != null)
      {
         if (type instanceof FloatControl.Type)
            valueObject = new Float(value);
         else // BooleanControl.Type
            valueObject = new Boolean(value == 1.0f);
         controlValues_.put(identifier, valueObject);

         // bug around (gain resets mute):
         boolean isMute = controlTypes_.get("MUTE") != null && output_.isOpen() && ((BooleanControl)output_.getControl((Control.Type)controlTypes_.get("MUTE"))).getValue();
         if (isMute && identifier.equals("MASTER_GAIN")) return;

         if (output_.isOpen()) {
            Control control = output_.getControl(type);
            if (control instanceof FloatControl)
            {
               FloatControl fc = (FloatControl)control;
               
               if (value > fc.getMaximum())
                  value = fc.getMaximum();
               if (value < fc.getMinimum())
                  value = fc.getMinimum();
               
               fc.setValue(value);
            }
            else // BooleanControl
            {
               ((BooleanControl)control).setValue(value == 1.0f);
            }
         }
      }
   }
	
	public synchronized void update(LineEvent e) {
		//System.out.println("SP: line event: "+e);

		if (e.getType() == LineEvent.Type.START) {
			stopReceived_ = false;
		} else if (e.getType() == LineEvent.Type.STOP) {
			// notification for start() that we have properly stoped
			stopReceived_ = true;
			notify();
		}
	}
   
   public boolean decompressAll(File outputFile)
   {
      return decompressAll(outputFile, false);
   }
   
   public boolean decompressAll(File outputFile, boolean bAppend)
   {
      AudioInputStream input = null;
		try
      {
			input = retrieveInputAndSkip();
		}
      catch (Exception exc)
      {
         System.err.println("Error: AUDIO, could not open audio input.");
         return false;
		}
   
		int channels = format_.getChannels();
		int bits = format_.getSampleSizeInBits();
		int sampleSize = bits/8;
		int sampleRate = (int)format_.getSampleRate();
		int sampleCount = (int)((input.getFrameLength()*format_.getFrameSize())/sampleSize);
      
      int lengthInBytes = sampleCount*sampleSize;
      int nBlockSize = sampleSize*channels;
      int bytesPerSecond = sampleRate*nBlockSize;
      
		int nReadSamples = 0;
      
      try
      {
         DataOutputStream output = new DataOutputStream(
            new BufferedOutputStream(new FileOutputStream(outputFile, bAppend)));

         // write WAVE-RIFF header
         output.write(new byte[] { (byte)'R', (byte)'I', (byte)'F', (byte)'F' });
         writeBEInt(output, lengthInBytes+36); // 36 = header length
         output.write(new byte[] { (byte)'W', (byte)'A', (byte)'V', (byte)'E' });

         output.write(new byte[] { (byte)'f', (byte)'m', (byte)'t', (byte)' ' }); 
         output.write(new byte[] { (byte)16, (byte)0, (byte)0, (byte)0 }); // length of fmt-tag
         writeBEShort(output, (short)1); // = PCM
         writeBEShort(output, (short)channels);
         writeBEInt(output, sampleRate);
         writeBEInt(output, bytesPerSecond);
         writeBEShort(output, (short)nBlockSize);
         writeBEShort(output, (short)bits);
         output.write(new byte[] { (byte)'d', (byte)'a', (byte)'t', (byte)'a' });
         writeBEInt(output, lengthInBytes);

         
         long time1 = System.currentTimeMillis();
		
         int[] samples = new int[256];
         int read = extractSamples(input, samples);
         while (read != -1) 
         {
            writeSamples(output, samples, read, sampleSize);
            nReadSamples += read;
            read = extractSamples(input, samples);
         }
		
         output.flush();
         output.close();
        
         long time2 = System.currentTimeMillis();
      }
      catch (IOException exc)
      {
         System.err.println("Error: AUDIO, an IOException occured.");
         exc.printStackTrace();
         return false;
      }
      finally
      {
         closeFileInput(input);
      }
      
      if (nReadSamples != sampleCount)
         System.err.println("Warning: AUDIO, output length did not match header length.");
      
      return nReadSamples == sampleCount;
   }
   
   public AudioFormat getInputFormat()
   {
      return format_;
   }
   
   
   private int extractSamples(AudioInputStream ais, int[] samples) throws IOException 
   {
      int count = samples.length;
		int sampleSize = ais.getFormat().getSampleSizeInBits()/8;

      byte[] data = new byte[count*sampleSize];
      int read = 0;
      while(read < data.length) 
      {
			int r = ais.read(data, read, data.length-read);
			if (r == -1) break;
			read += r;
		}

		if (sampleSize > 1) 
      {
			if (read % sampleSize != 0) 
				System.err.println("Ooops! Unappropriate number of bytes in Input!");
			read /= sampleSize;
		}

      for (int i=0; i<read; i++) 
      {
         if (sampleSize == 1) 
         {
            if (ais.getFormat().getEncoding() != AudioFormat.Encoding.PCM_SIGNED) 
            {
               samples[i] = data[i] < 0 ? data[i]+128 : data[i]-128;
            } 
            else 
            {
               samples[i] = data[i];
            }
         } 
         else 
         {
            // seems to be wrong, but generates the right results !?
            if (ais.getFormat().isBigEndian()) samples[i] = data[2*i]<<8 | (data[2*i+1]&0xff);
            else samples[i] = data[2*i+1]<<8 | (data[2*i]&0xff);
         }
			//if (System.getProperty("app.debug") != null)
			//	System.out.print(samples[i]+" ");
      }

		if (read == 0) 
         return -1;
      
      return read;
   }
       
   private void writeSamples(OutputStream out, int[] samples, int length, int sampleSize) throws IOException 
   {
	    for (int s=0; s<length; s++) 
       {
			if (sampleSize == 1)  // 8 bit unsigned
         {
				out.write(samples[s]+128);
			} 
         else // 16 bit signed; big endian (intel)
         { 
				byte[] el = new byte[2];
				el[0] = (byte)samples[s];
				el[1] = (byte)(samples[s] >> 8);
				out.write(el);
			}
		}
	}

   private void writeBEShort(DataOutputStream out, short s) throws IOException 
   {
      out.writeByte((byte)s);
      out.writeByte((byte)(s >> 8));
   }

   private void writeBEInt(DataOutputStream out, int i) throws IOException 
   {
      out.writeByte((byte)i);
      out.writeByte((byte)(i >> 8));
      out.writeByte((byte)(i >> 16));
      out.writeByte((byte)(i >> 24));
   }
   
   private void setFloatControlValue(FloatControl control, float fValue)
   {
      if (fValue >= control.getMinimum() && fValue <= control.getMaximum())
         control.setValue(fValue);
   }

   
   private static File extractLpd(String strLpdFilename, String strTargetDirectory)
   throws IOException
   {
      // no error checking here
      
      File fileLpd = new File(strLpdFilename);
      File dirTarget = new File(strTargetDirectory);
      File filePossibleLrd = null;
      
      FileResources fr = FileResources.createFileResources(strLpdFilename);
      
      Hashtable ht = fr.getArchivedFiles();
      
      Enumeration enumKeys = ht.keys();
      while(enumKeys.hasMoreElements())
      {
         String strKey = (String)enumKeys.nextElement();
         
         FileStruct fs = (FileStruct)ht.get(strKey);
         
         if (fs != null)
         {
            System.out.println("Extracting "+fs.name+" ...");
            
            File fileOutput = new File(dirTarget, fs.name);
            // What about this file existing?
            
            if (fs.name.toLowerCase().endsWith(".lrd"))
               filePossibleLrd = fileOutput;
            
            InputStream in = fr.createInputStream(fs.name);
            OutputStream out = new FileOutputStream(fileOutput);
            
            byte[] aBuffer = new byte[16384];
            
            int iRead = 0;
            while ((iRead = in.read(aBuffer)) > 0)
               out.write(aBuffer, 0, iRead);
            
            in.close();
            out.close();
         }
      }
      System.out.println("All extracted from LPD.");
      
      return filePossibleLrd;
   }
   
   private static void extractAqsToLad(File fileLrd)
   throws IOException
   {
      // only some error checking here
      
      if (fileLrd == null || !fileLrd.exists())
      {
         System.err.println("Lrd file does not exist or is null.");
         return;
      }
      
      // double code (Document, LpdFile)

      FileResources resources = FileResources.createFileResources(fileLrd.getPath());
      InputStream configStream = resources.createConfigFileInputStream();

      if (configStream == null)
      {
         System.err.println("No proper lrd file available?");
         return;
      }
            
      XmlTag configTag = (XmlTag.parse(configStream))[0];
      
      configStream.close();

      String strAqsLocation = (String)( (configTag.getValues("AUDIO"))[0] );
      
      if (configStream == null)
      {
         System.err.println("No audio location in lrd file.");
         return;
      }
      
      String strLadFilename = strAqsLocation + ".lad";
      if (strAqsLocation.toLowerCase().endsWith(".aqs"))
         strLadFilename = strAqsLocation.substring(0, strAqsLocation.length() - 4) + ".lad";
      File fileOutput = new File(fileLrd.getParent(), strLadFilename);
      
      System.out.println("Extracting "+strAqsLocation+" to "+strLadFilename+" ...");
      
      System.setProperty("nyckel.musik", "forte");
      
      SoundPlayer sound = null;      
      try
      {
         sound = new SoundPlayer(resources, strAqsLocation, null);
      }
      catch (Exception exc)
      {
         System.err.println("No valid audio?");
         exc.printStackTrace();
      }
      
      
      // write LAD header (with empty xor key: wav can be just appended)
      FileOutputStream fos = new FileOutputStream(fileOutput);
      fos.write(new byte[] { (byte)'L', (byte)'A', (byte)'D', (byte)' ' });
      byte[] aBuffer = new byte[1020];
      fos.write(aBuffer, 0, 892); // empty xor
      
      InputStream in2 = null;
      try
      {
         in2 = resources.createInputStream("_internal_versiontypeinfo");
      }
      catch (ResourceNotFoundException exc)
      {
         System.err.println("_internal_versiontypeinfo could not be found. LAD will be corrupt.");
      }
      
      if (in2 != null)
      {
         int iRead = in2.read(aBuffer, 0, 128);    
         in2.close();
      }
      
      fos.write(aBuffer, 0, 128);
      
      fos.close();
      
      sound.decompressAll(fileOutput, true);
         
      System.out.println("Audio finished.");
   }
   
   public static void main(String[] args)
   {
      //
      // Used for extracting a given lpd file.
      
      if (args.length == 0)
      {
         System.err.println("Usage: java SoundPlayer <input.lpd> [output-dir]");
         System.exit(1);
      }
      
      File fileInput = new File(args[0]);
      
      if (!fileInput.exists())
      {
         System.err.println("Input lpd cannot be found: "+fileInput);
         System.exit(2);
      }
      
      fileInput = fileInput.getAbsoluteFile();
      
      boolean bExtractAudio = true;
      
      if (args.length > 1 && args[1].equals("/noaudioex"))
         bExtractAudio = false;
      
      File dirOutput = null;
      if (args.length > 1 && bExtractAudio)
      {
         dirOutput = new File(args[1]);
      }
      else
      {
         dirOutput = new File(fileInput.getParent(), fileInput.getName() + "_ext");
      }
      
      if (!dirOutput.exists())
         dirOutput.mkdirs();
      
      if (!dirOutput.isDirectory())
      {
         System.err.println("Output dir is no dir or cannot be created: "+dirOutput);
         System.exit(3);
      }
      
      if (bExtractAudio && args.length > 2 && args[1].equals("/noaudioex"))
          bExtractAudio = false;
      
      try
      {
         File fileLrdTarget = extractLpd(fileInput.getPath(), dirOutput.getPath());
         
         if (fileLrdTarget != null && bExtractAudio)
            extractAqsToLad(fileLrdTarget);
      }
      catch (IOException exc)
      {
         exc.printStackTrace();
      }
   }
}