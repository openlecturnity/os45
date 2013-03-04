package imc.epresenter.filesdk.util;

/*
 * File:             CountInputStream.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: CountInputStream.java,v 1.3 2002-01-25 09:20:24 danielss Exp $
 */

import java.io.InputStream;
import java.io.FilterInputStream;
import java.io.IOException;

/**
 * This class is filtering {@link java.io.InputStream InputStream}
 * which may be used in order to count the bytes that are being read
 * from another {@link java.io.InputStream InputStream}. All actions
 * on the {@link java.io.InputStream InputStream} are simply delegated
 * to that stream after the bytes read have been counted. Instances of
 * the <tt>CountInputStream</tt> report the bytes read to an instance
 * of a class implementing the {@link ByteCounter ByteCounter}
 * interface. <b>Note:</b> The notify action is non-blocking, i.e. if
 * the notify thread takes longer, data will still be read in the
 * background!
 *
 * @see ByteCounter
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class CountInputStream extends FilterInputStream
{
   protected ByteCounter byteCounter = null;

   private int readBytes_ = 0;
   private EventDispatcher eventDispatcher_;

   /**
    * Creates a new <tt>CountInputStream</tt> with the given
    * parameters.
    *
    * @param in the input stream to count the bytes in
    * @param byteCounter the {@link ByteCounter ByteCounter} instance
    * to report the counted bytes to.
    * @see ByteCounter
    */
   public CountInputStream(InputStream in, ByteCounter byteCounter)
   {
      super(in);
      this.byteCounter = byteCounter;
      readBytes_ = 0;

      eventDispatcher_ = new EventDispatcher();
      eventDispatcher_.start();
   }

   /**
    * Read one byte of data from the underlying {@link
    * java.io.InputStream InputStream}, and then report the read byte
    * to the {@link ByteCounter ByteCounter}, if ready.
    *
    * @return a byte of data read, or <tt>-1</tt> if the end of the
    * stream has been reached.
    */
   public int read()
      throws IOException
   {
      int tmp = in.read();
      if (tmp != -1)
	 readBytes_++;
      
      eventDispatcher_.requestDisplay();
      return tmp;
   }

   /**
    * Reads up to <tt>len</tt> bytes into the buffer <tt>b</tt>,
    * starting at the offset <tt>off</tt>. Then report the read bytes
    * to the {@link ByteCounter ByteCounter}, if it is ready.
    *
    * @param b the buffer to fill
    * @param off the offset in the buffer to use
    * @param len the maximum number of bytes to read
    * @return the number of bytes read in this read action
    */
   public int read(byte[] b, int off, int len)
      throws IOException
   {
      int tmp = in.read(b, off, len);
      readBytes_ += tmp;
      eventDispatcher_.requestDisplay();
      return tmp;
   }

   /**
    * Close the underlying stream. Shut down the read events
    * dispatcher thread.
    */
   public void close()
      throws IOException
   {
      super.close();
      eventDispatcher_.requestStop();
      eventDispatcher_ = null;
   }

   /**
    * Return the total number of bytes read up to now.
    *
    * @return the total number of bytes read up to now.
    */
   public int getReadBytes()
   {
      return readBytes_;
   }

   private class EventDispatcher extends Thread
   {
      private boolean requestStop_ = false;
      private boolean isWaiting_   = false;
      private boolean newRequest_  = false;

      public void run()
      {
	 while (!requestStop_)
	 {
	    if (!newRequest_)
	    {
	       try
	       {
		  synchronized(this)
		  {
		     isWaiting_ = true;
		     wait();
		  }
	       }
	       catch (InterruptedException e)
	       {
	       }
	       isWaiting_ = false;
	    }

	    newRequest_ = false;
	    byteCounter.displayCountedBytes(readBytes_);

            try 
            {
               // new: we do not need to update that often
               sleep(500);
            }
            catch (InterruptedException e)
            {
            }
	 }
      }

      public void requestDisplay()
      {
	 newRequest_ = true;
	 if (isWaiting_)
	    synchronized(this)
	    {
	       notify();
	    }
      }

      public void requestStop()
      {
	 requestStop_ = true;
	 if (isWaiting_)
	    interrupt();
      }
   }
}
