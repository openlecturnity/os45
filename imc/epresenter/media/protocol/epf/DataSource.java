package imc.epresenter.media.protocol.epf;
 
import javax.media.protocol.PullDataSource;
import javax.media.protocol.SourceStream;
import javax.media.protocol.PullSourceStream;
import javax.media.protocol.Seekable;
import javax.media.Time;
import javax.media.Duration;
import javax.media.MediaLocator;
import javax.media.format.AudioFormat;
import com.sun.media.MimeManager;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Hashtable;

import imc.epresenter.filesdk.FileResources;

public class DataSource extends PullDataSource
{
   private String archive_ = null;
   private String resource_ = null;

   private Hashtable mimeTable_ = MimeManager.getDefaultMimeTable();
   
   // in order to be able to close all openened stream upon disconnect()
   private ArrayList streamCollector_ = new ArrayList(3);

   public void start()
   {
//       System.out.println("EpfDataSource::start()");
   }

   public void stop()
   {
//       System.out.println("EpfDataSource::stop()");
   }

   public void connect()
   {
//       System.out.println("EpfDataSource::connect()");
      initCheck();

      // parse locater string
      MediaLocator locator = getLocator();
//       System.out.println("URL: " + locator.toString());
      String rem = locator.getRemainder();
      int p1, p2 = 0;
//       System.out.println("rem==\"" + rem + "\"");
      p1 = rem.indexOf("//");
//       System.out.println("indexOf(//)==" + p1);
      p2 = rem.lastIndexOf('/');
//       System.out.println("lastIndexOf(/)==" + p2);
      archive_ = rem.substring(p1 + 2, p2);
      resource_ = rem.substring(p2 + 1);
//       System.out.println("Archive: " + archive_ + ", resource " + resource_);
//       try
//       {
//          FileResources resources = FileResources.createFileResources(archive_);
//       }
//       catch (IOException e)
//       {
//          e.printStackTrace();
//       }
   }

   public void disconnect()
   {
      //System.out.println("EpfDataSource::disconnect() "+streamCollector_.size());
      for (int i=0; i<streamCollector_.size(); ++i)
         ((EpfSourceStream)streamCollector_.get(i)).close();
      streamCollector_.clear();
   }

   public PullSourceStream[] getStreams()
   {
      //System.out.println("EpfDataSource::getStreams()");
      PullSourceStream[] streams = new PullSourceStream[1];
      try
      {
         FileResources resources = FileResources.createFileResources(archive_);
         streams[0] = new EpfSourceStream(resources, resource_);
         
         streamCollector_.add(streams[0]);
      }
      catch (IOException e)
      {
         e.printStackTrace();
      }
      return streams;
   }

   public Time getDuration()
   {
//       System.out.println("EpfDataSource::getDuration()");
      return Duration.DURATION_UNKNOWN;
   }

   public String getContentType()
   {
      // return AudioFormat.MPEGLAYER3;
      // return "audio.mpeg";
      int dotPos = resource_.lastIndexOf('.');
      String extension = resource_.substring(dotPos + 1).toLowerCase();
      String content = "unknown";

      String mimeContent = (String) (mimeTable_.get(extension));
      if (mimeContent != null) 
      {
         content = mimeContent;
         content = content.replace('/', '.');
      }
      
//       if (extension.equals("mp3"))
//          content = "audio.mpeg";
//       else if (extension.equals("avi"))
//          content = "video.x_msvideo";

//       System.out.println("EpfDataSource::getContentType() == " + content);
      return content;
   }

   public Object[] getControls()
   {
//       System.out.println("EpfDataSource::getControls()");
      return new Object[] {};
   }

   public Object getControl(String control)
   {
//       System.out.println("EpfDataSource::getControl()");
      return null;
   }
}
