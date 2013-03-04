package imc.epresenter.media.protocol.fastfile;
 
import java.io.*;
import javax.media.protocol.ContentDescriptor;
import javax.media.protocol.PullSourceStream;
import javax.media.protocol.SourceStream;
import javax.media.protocol.Seekable;

import imc.epresenter.filesdk.*;

public class FastFileSourceStream implements PullSourceStream, Seekable
{
   /** You may set the buffer size here, defaults to 128 kB (131072 bytes). */
   public static int BUFFER_SIZE = 131072; // 128 kB

   private InputStream in_ = null;
   private FileResources fileResources_;
   private String resource_;
   private boolean endMarker_ = false;
   private long length_ = LENGTH_UNKNOWN;
   // private long offset_;
   private boolean newSeekRequest_ = true;
   private long seekRequest_ = 0;
   private long totalRead_;
   
   public FastFileSourceStream(FileResources fileResources, String resource)
      throws IOException
   {
      super();
      try
      {
         //System.out.println("FastFileSourceStream()");
         fileResources_ = fileResources;
         resource_ = resource;
         initInputStream();
      }
      catch (IOException e)
      {
         e.printStackTrace();
         throw e;
      }
   }

   private void initInputStream() throws IOException
   {
      in_ = fileResources_.createBufferedInputStream(resource_, BUFFER_SIZE);
      
      length_ = in_.available();
      // offset_ = in_.getTotalRead();
      totalRead_ = 0;
   }

   public int read(byte[] buffer, int offset, int length)
      throws IOException
   {
      try
      {
         int tmp = in_.read(buffer, offset, length);
         if (tmp == -1)
            endMarker_ = true;

         totalRead_ += tmp;
         return tmp;
      }
      catch (IOException e)
      {
         System.err.println("FASTFILESOURCESTREAM: EXCEPTION IN read(buffer, offset, length)");
         e.printStackTrace();
         throw e;
      }
   }

   public boolean isRandomAccess()
   {
      return true;
   }

   public long seek(long where)
   {
      try
      {
         if (totalRead_ > where)
         {
            initInputStream();
            return seek(where);
         }

         long rest = where - totalRead_;
         long totalSkip = 0;
         long skipped = 0; 
         while (totalSkip < rest)
         {
            skipped = in_.skip(rest - totalSkip);
            totalSkip += skipped;
         }

//          System.out.println("totalSkip == " + totalSkip);
         totalRead_ += totalSkip;
      }
      catch (IOException e)
      {
         e.printStackTrace();
         return -1;
      }
      
      return totalRead_;
   }

   public long tell()
   {
      return totalRead_;
   }

   public boolean endOfStream()
   {
//       System.out.println("FastFileSourceStream::endOfStream()");
      return endMarker_;
   }

   public int available() throws IOException
   {
//       System.out.println("FastFileSourceStream::available()");
//       int avail = in_.available();
//       int avail2 = tmpIn_.available();
//       if (avail != avail2)
//          System.out.println("avail != avail2");
      return in_.available();
   }
   
   public ContentDescriptor getContentDescriptor()
   {
//       System.out.println("FastFileSourceStream::getContentDescriptor()");
      return new ContentDescriptor("unknown");
   }

   public long getContentLength()
   {
//       System.out.println("FastFileSourceStream::getContentLength() == " + length_);
      return length_;
   }

   public boolean willReadBlock()
   {
//       System.out.println("FastFileSourceStream::willReadBlock()");
      return false;
   }

   public Object[] getControls()
   {
//       System.out.println("FastFileSourceStream::getControls()");
      return new Object[0];
   }

   public Object getControl(String control)
   {
//       System.out.println("FastFileSourceStream::getControl()");
      return null;
   }
}
