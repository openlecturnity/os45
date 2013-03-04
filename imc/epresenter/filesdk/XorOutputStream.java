package imc.epresenter.filesdk;

/*
 * File:             XorOutputStream.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: XorOutputStream.java,v 1.4 2002-10-14 11:54:09 danielss Exp $
 */
import java.io.FilterOutputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.io.FileOutputStream;

/**
 * This class implements a stream filter for encrypting a stream using
 * a simple XOR (<tt>^</tt>) encryption.
 *
 * @see XorInputStream
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class XorOutputStream extends FilterOutputStream
{
   private byte[] key_;
   private int writtenBytes_ = 0;
   private int totalWrittenBytes_ = 0;
   private byte[] intBuf_ = new byte[1];

   private static int __c = 0;
   private FileOutputStream __out;

   public static boolean DEBUG = false;

   /**
    * Creates a new output stream using the given input stream and the
    * given key (as a <tt>byte</tt> array).
    *
    * @param outputStream the output stream to filter (xor)
    * @param key the key to use in the encryption
    */
   public XorOutputStream(OutputStream outputStream, byte[] key) throws IOException
   {
      super(outputStream);
      key_ = key;
      writtenBytes_ = 0;
      totalWrittenBytes_ = 0;

      if (DEBUG)
         __out = new FileOutputStream("__" + (__c++) + ".png");
   }
   
   /**
    * Write a byte xor'ed to the underlying stream. <b>Note:</b> Not
    * an <tt>int</tt> is written, but only the lowest eight bits!
    *
    * @param b the byte to be written
    */
   public void write(int b) throws IOException 
   {
      if (DEBUG)
         __out.write(b);
      intBuf_[0] = (byte) (b&0x000000ff);
      write(intBuf_, 0, 1);
   }

   public void write(byte[] buf, int off, int len) throws IOException
   {
      if (DEBUG)
         __out.write(buf, 0, len);

      for (int i=0; i<len; i++) 
	 buf[i] = (byte) (buf[i + off] ^ key_[(writtenBytes_++)%key_.length]);

      writtenBytes_ = writtenBytes_ % key_.length;
      totalWrittenBytes_ += len;
      out.write(buf, 0, len);
   }

   /**
    * This method calls <tt>write(buf, 0, bef.length)</tt>.
    */
   public void write(byte[] buf) throws IOException
   {
      if (DEBUG)
         __out.write(buf);
      write(buf, 0, buf.length);
   }

   /**
    * Returns the total number of bytes written by (through) this object.
    * @return the total number of bytes written by (through) this object.
    */
   public int getTotalWrittenBytes() 
   {
      return totalWrittenBytes_;
   }

   public void flush() throws IOException
   {
      if (DEBUG)
         __out.flush();
      out.flush();
   }

   public void close() throws IOException
   {
      if (DEBUG)
         __out.close();
      out.close();
   }
}
