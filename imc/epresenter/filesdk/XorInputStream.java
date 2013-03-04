package imc.epresenter.filesdk;

/*
 * File:             XorInputStream.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: XorInputStream.java,v 1.6 2002-02-07 12:35:34 danielss Exp $
 */
import java.io.FilterInputStream;
import java.io.InputStream;
import java.io.IOException;

/**
 * This class implements an input stream for decrypting an input
 * stream using a simple XOR (<tt>^</tt>) decrypting method.
 *
 * <p>Instances support truncating the underlying input streams. By
 * using the {@link #setMaxBytesRead(int) setMaxBytesRead} method, a
 * maximum number of bytes to be read from the current stream position
 * on may be defined. If the {@link #read(byte[]) read} methods are
 * called further, nothing will be read (the methods will return
 * <tt>0</tt>).</p>
 *
 * @see XorOutputStream
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class XorInputStream extends FilterInputStream 
{
   private byte[] key_;
   private int readBytes_;
   private byte[] intBuf_ = new byte[4];
   private long totalRead_ = 0;
   private long markReadMark_ = -1;
   private int markReadBytes_ = -1;
   private long marker_    = 0;
   private int truncAfterBytes_ = 0;

   /**
    * Creates an input stream from the given parameters.
    *
    * @param inputStream the input stream to decrypt
    * @param key the key to use in the decryption
    */
   public XorInputStream(InputStream inputStream, byte[] key) throws IOException
   {
      super(inputStream);
      key_ = key;
      readBytes_ = 0;
      totalRead_ = 0;
   }

   /**
    * @return <i>false</i>
    */
   public boolean markSupported()
   {
//       System.out.println("XorInputStream::markSupported()");
      return in.markSupported();
      // return false;
   }

   /**
    * Returns the number of bytes that can be read from this input
    * stream without blocking.
    */
   public int available() throws IOException
   {
      int tmp = in.available();
      if (truncAfterBytes_ > 0)
         return tmp < getRemains() ? tmp : ((int) getRemains());
      else
         return tmp;
   }

   /**
    * Reads a byte from the input stream and returns it xor'ed.
    *
    * @return the xored return byte as an <tt>int</tt>
    */
   public int read() throws IOException
   {
      if (truncAfterBytes_ > 0)
	 if (getRemains() < 1)
         {
//             System.out.println("===> EOF??!");
	    return -1;
         }

      int tmp2 = in.read();
//       System.out.println("in.read() == " + tmp2);
      //byte tmp = (byte) (tmp2);
      int tmp = ((tmp2 & 0xff) ^ (key_[(readBytes_++)%key_.length] & 0xff));
      totalRead_++;
//       System.out.println("XorInputStream::read() avail == " + available() + ", '" + tmp + "'.");
      return tmp;
   }

   public int read(byte[] buf, int off, int len) throws IOException
   {
//       System.out.println("XorInputStream::read(buf, "+off+", "+len+")");
      if (truncAfterBytes_ > 0)
      {
         long remains = getRemains();
         if (remains < len)
            len = (int) remains;
      }

      if (len == 0)
	 return -1;

      int readBytes = in.read(buf, off, len);
      if (readBytes != -1)
      {
         for (int i=0; i<readBytes; i++)
            buf[i + off] = (byte) (buf[i + off] ^ key_[(readBytes_++)%key_.length]);

         readBytes_ = readBytes_%key_.length;
         totalRead_ += readBytes;
      }
      return readBytes;
   }

   /**
    * Skip <tt>n</tt> bytes in the input stream. Instances of this
    * class assume that the xor key is applied continuously onto the
    * underlying stream, i.e. by doing a <tt>skip()</tt>, the key
    * pointer is also increased about the same amount.
    *
    * @param n number of bytes to skip
    */   
   public long skip(long n) throws IOException
   {
//       System.out.println("XorInputStream::skip("+n+")");
      long skipwish = n;

      if (truncAfterBytes_ > 0)
      {
	 long remains = getRemains();
	 if (n > remains)
	    skipwish = remains;
      }
      long tmp = in.skip(skipwish);
      totalRead_ += tmp;
      readBytes_ += ((int) tmp)%key_.length;

//       System.out.println("XorInputStream::skip(" + n + "), sw=" + skipwish + ", " +
//                          "skip " + tmp + " b, rem=" + getRemains());
      return tmp;
   }

   /**
    * Returns the total number of bytes read by this stream
    * (<b>Note:</b> a {@link #skip(long) skip()} counts as a
    * <tt>read()</tt>!)
    *
    *  @return the total number of bytes read by
    * this stream */
   public long getTotalRead()
   {
      return totalRead_;
   }

   /**
    * Define a maximum number of bytes to read from this stream
    * position on. After that point, no more bytes will be read from
    * the underlying input stream. Use <tt>0</tt> as argument to
    * disable this feature (default).
    *
    * @param bytes maximum number of bytes to read, or 0 to disable
    * stream truncating
    */
   public void setMaxBytesRead(int bytes)
   {
//       System.out.println("XorInputStream::setMaxBytesRead(" + bytes +
//                          ") at " + getTotalRead() + " bytes read.");
      if (bytes > 0)
      {
	 marker_ = totalRead_;
	 truncAfterBytes_ = bytes;
      }
      else
      {
	 marker_ = 0;
	 truncAfterBytes_ = 0;
      }
   }

   public void mark(int maxRead)
   {
//       System.out.println("XorInputStream::mark(" + maxRead + ")");
      markReadMark_ = totalRead_;
      markReadBytes_ = readBytes_;
      in.mark(maxRead);
   }

   public void reset()
      throws IOException
   {
//       System.out.println("XorInputStream::reset()");
      in.reset();
      totalRead_ = markReadMark_;
      readBytes_ = markReadBytes_;
   }

   private long getRemains() throws IOException
   {
      if (truncAfterBytes_ > 0)
         return (truncAfterBytes_ - (totalRead_ - marker_));
      else
         return available();
   }
}
