package imc.epresenter.media.codec.video;

import java.awt.Dimension;
import java.awt.Component;
import javax.media.*;
import javax.media.control.FrameProcessingControl;
import javax.media.format.VideoFormat;
import javax.media.format.RGBFormat;

import com.sun.media.BasicPlugIn;
import com.sun.media.vfw.BitMapInfo;

public class LsgcDecoder extends BasicPlugIn implements Codec
{
   private static boolean libraryLoaded_ = false;
   static 
   {
      try
      {
         System.loadLibrary("lsgc");
         libraryLoaded_ = true;
      }
      catch (UnsatisfiedLinkError e) 
      {
			libraryLoaded_ = false;
		}
	
   }
   static Integer processLock_ = new Integer(1);


   private VideoFormat defaultInputFormat_ = new VideoFormat("LSGC");
   private VideoFormat defaultOutputFormat_ = new RGBFormat();

   private VideoFormat activeInputFormat_ = null;
   private VideoFormat activeOutputFormat_ = null;
   private BitMapInfo activeInputBitMapInfo_ = null;
   private BitMapInfo activeOutputBitMapInfo_ = null;
   private long handle_;

   private Control [] controls_ = null;
   private DropControl dropControl_ = null;
   private boolean dropFrame_ = false;

   public LsgcDecoder() 
   {
   }

   public Format [] getSupportedInputFormats() 
   {

      if (libraryLoaded_)
      {
         return new Format[] { defaultInputFormat_ };
      }
      else
      {
         return new Format[0];
      }
   }

   public Format [] getSupportedOutputFormats(Format in) 
   {
      if (in == null)
         return new Format[] { defaultOutputFormat_ };

      if (!in.matches(defaultInputFormat_) || !libraryLoaded_)
         return new Format[0];

      VideoFormat video = (VideoFormat) in;
      Dimension size = video.getSize();
      if (size == null)
         size = new Dimension(320, 240);
      // this is a rather dodgy workaround so that 
      // twlc-videos can have uneven widths
      int padding = 0;
      if (size.width*3 % 4 != 0)
         padding = 4 - size.width*3 % 4;
      
      RGBFormat [] outFormats = new RGBFormat[] 
      {
         new RGBFormat(size, size.width * size.height * 3 + size.height * padding,
                       Format.byteArray,
                       video.getFrameRate(), // frame rate
                       24,
                       3, 2, 1,
                       3,
                       size.width * 3 + padding,
                       Format.TRUE,
                       Format.NOT_SPECIFIED), // endian

         new RGBFormat(size, size.width * size.height * 3 + size.height * padding,
                       Format.byteArray,
                       video.getFrameRate(), // frame rate
                       24,
                       3, 2, 1,
                       3,
                       size.width * 3 + padding,
                       Format.FALSE, // flipped
                       Format.NOT_SPECIFIED) // endian
      } ;

      return outFormats;
   }


   public Format setInputFormat(Format in) 
   {
      if (in instanceof VideoFormat) 
      {
         if (in.matches(defaultInputFormat_))
         {
            activeInputFormat_ = (VideoFormat) in;

            // TODO: Handle case that the codec is already open ? 

            return in;
         }
      }

      return null;
   }

   public Format setOutputFormat(Format out) {
      if (libraryLoaded_ && out.matches(defaultOutputFormat_))
      {
         activeOutputFormat_ = (RGBFormat) out;

         return out;
      }

      return null;
   }

   public void open() throws ResourceUnavailableException 
   {
      if (activeInputFormat_ == null || activeOutputFormat_ == null)
      {
         throw new ResourceUnavailableException("Formats not set!");
      }

      Dimension size = activeInputFormat_.getSize();
      
      // Translate from VideoFormat to BitMapInfo
      activeInputBitMapInfo_ = new BitMapInfo(activeInputFormat_.getEncoding(),
                                              size.width, size.height);
      activeOutputBitMapInfo_ = new BitMapInfo(activeOutputFormat_.getEncoding(),
                                               size.width, size.height);

      long handle = nDecompressBegin(activeInputBitMapInfo_, 
                                     activeOutputBitMapInfo_);

      if (handle < 0) {
         close();
         throw new ResourceUnavailableException("Formats not supported.");
      }

      handle_ = handle;
   }

   public void close()
   {
      nDecompressEnd(handle_);
      activeInputBitMapInfo_ = null;
    
   }

   public void reset() 
   {
      if (activeInputBitMapInfo_ != null)
      {
         nDecompressEnd(handle_);
         handle_ = nDecompressBegin(activeInputBitMapInfo_, 
                                    activeOutputBitMapInfo_);
      }
   }

   public int process(Buffer inBuffer, Buffer outBuffer) 
   {
      if (inBuffer.isEOM())
         outBuffer.setEOM(true);
      else
         outBuffer.setEOM(false);
 
      if (outBuffer.isEOM())
         return BUFFER_PROCESSED_OK;


      Format outf = outBuffer.getFormat();
      outBuffer.setFormat(activeOutputFormat_);
      Object outBuf = validateData(outBuffer, 0, true);
      Object inBuf = getInputData(inBuffer);
      
      int size = inBuffer.getLength();
      if (size < 2)
         return BUFFER_PROCESSED_FAILED;
      activeInputBitMapInfo_.biSizeImage = size;
      

      long flags = 0;
      if (dropFrame_)
         flags |= 0x20000000L;

      int result;
      synchronized (processLock_) 
      {
         // retrieve pointer to native data (char * in C)
         long outBytes = getNativeData(outBuf);
         long inBytes = getNativeData(inBuf);
         result = nDecompress(handle_, flags, 
                              activeInputBitMapInfo_, inBuf, inBytes,
                              activeOutputBitMapInfo_, outBuf, outBytes);
      }

      if (dropFrame_)
         outBuffer.setDiscard(true);
      else
         outBuffer.setLength(activeOutputFormat_.getMaxDataLength());

      if (result < 0)
         return BUFFER_PROCESSED_FAILED;
      else
         return BUFFER_PROCESSED_OK;
   }

   public Object getControl(String name)
   {
      return null;
   }
 
   public Object [] getControls() {
      if (dropControl_ == null) {
         dropControl_ = new DropControl();
         controls_ = new Control[1];
         controls_[0] = dropControl_;
      }

      return controls_;
   }

   public String getName() 
   {
      return "VCM Decoder";
   }


   void dropNextFrame(boolean on) {
      dropFrame_ = on;
   }



   class DropControl implements FrameProcessingControl {

      public Component getControlComponent() {
         return null;
      }

      public boolean setMinimalProcessing(boolean on) {
         dropNextFrame(true);
         return true;
      }

      /**
       * Informs the codec that it is behind by some number of frames
       * and that it needs to either speed up by dropping quality or by
       * dropping frames as it sees fit. The value <code>framesBehind</code>
       * can either be positive, zero or negative. A negative value indicates
       * that the codec is ahead by that many frames and can possibly improve
       * quality if its not at maximum. This method needs to be called before a
       * call to <code>process</code>. The value is remembered by the codec
       * until it is explicitly changed again.
       */
      public void setFramesBehind(float framesBehind) {
         if (framesBehind > 0)
            dropNextFrame(true);
         else
            dropNextFrame(false);
      }

      public int getFramesDropped() {
         return 0;       ///XXX not implemented
      }
   }


   private native int nDecompressBegin(BitMapInfo biIn, BitMapInfo biOut);
   
   private native int nDecompress(long handle, long flags, 
                                  BitMapInfo biIn, Object dataIn, long inBytes,
                                  BitMapInfo biOut, Object dataOut, long outByte);
    
   private native void nDecompressEnd(long handle);
}

