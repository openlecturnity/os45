package imc.epresenter.player;

import java.awt.Canvas;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.event.ActionListener; // for DelayedTimer
import java.awt.event.ActionEvent;
import java.awt.event.MouseMotionListener; // for main()
import java.awt.event.MouseMotionAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.net.URL;
import java.util.Arrays;
import java.util.Iterator;
import java.util.Vector;
import javax.media.*;
import javax.media.format.VideoFormat;
import javax.media.format.RGBFormat;
import javax.media.protocol.ContentDescriptor;
import javax.media.protocol.DataSource;
import javax.media.protocol.Positionable;
import javax.media.renderer.VideoRenderer;

import imc.epresenter.media.parser.video.AviUnbugParser;
import imc.epresenter.player.util.VideoTimeBase;
import imc.lecturnity.util.DelayedTimer;

public class VideoChain implements Runnable, ActionListener
{
   private final boolean SMART_SCROLLING = true; // means fast here, because when done slow
   // only key frames are displayed during scrolling

   private Demultiplexer videoDemux_;
   private Track videoTrack_;
   private Codec videoCodec_;
   private VideoFormat videoFormat_;
   private VideoRenderer videoRenderer_;

   private Thread accessThread_;
   private DelayedTimer delayThread_;

   private Buffer codedBuffer_;
   private Buffer decodedBuffer_;
   private VideoTimeBase timeBase_;
   private long syncNanos_;
   private long syncedWhen_;
   private boolean running_;
   private boolean eomReached_;
   private boolean showSingleFrame_;
   private boolean renderKeyFramesOnly_;
   private long[] lastJumpMillis_;
   private boolean isFastScrolling_ = false;
   private int frameCount_;
   private long nanosPerFrame_;
   private int demuxFrameNumber_;

   private long lastDisplayTime_ = System.currentTimeMillis();

   private Object onHold_ = new Object();



   public static void main(String[] args) throws Exception
   {
      final VideoChain vc = new VideoChain(new MediaLocator(new File(args[0]).toURL()));
      java.awt.Frame f = new java.awt.Frame("Sundi");
      f.add("Center", vc.getVisualComponent());

      final Component c = new Canvas() {
         public Dimension getPreferredSize()
         {
            return new Dimension(200,30);
         }
      };
      c.addMouseMotionListener(new MouseMotionAdapter() {
         public void mouseDragged(MouseEvent e)
         {
            float f = e.getX()/(float)c.getSize().width;
            if (f >= 0 && f <= 1)
               vc.jumpToMillis((int)(f*vc.getDuration().getSeconds()*1000));
         }
      });
       
      f.add("South", c);



      f.pack();
      f.setVisible(true);
      //vc.start();





      /*
      try { Thread.sleep(5000); } catch (InterruptedException e ) {}
      vc.jumpToMillis(2140*1000);
      try { Thread.sleep(10000); } catch (InterruptedException e ) {}
      vc.jumpToMillis(0*1000);
      */
   }

   public VideoChain(MediaLocator sourceLocator) throws NoPlayerException
   {
      //javax.media.Manager.setHint(
      //   javax.media.Manager.LIGHTWEIGHT_RENDERER, new Boolean(true));

      //System.out.println("ding");
      try
      {
         DataSource source = javax.media.Manager.createDataSource(sourceLocator);
         ContentDescriptor descriptor = new ContentDescriptor(source.getContentType()); 

         //
         // find demultiplexer (or at least a reader)
         //
         Iterator iter = PlugInManager.getPlugInList(
            descriptor, null, PlugInManager.DEMULTIPLEXER).iterator();
         if(iter.hasNext())
            videoDemux_ = (Demultiplexer)loadClassInstance((String)iter.next());
         

         if (videoDemux_ == null)
            throw new NoPlayerException("No demultiplexer found for "+descriptor);

         System.out.println("demux: "+videoDemux_.getClass());
         //System.out.println("demux: "+videoDemux_);

         videoDemux_.setSource(source);
         Track[] tracks = videoDemux_.getTracks();
         // search for (first) Track with VideoFormat
         VideoFormat videoFormat = null;
         for (int i=0; i<tracks.length; i++)
         {
            if (tracks[i].getFormat() instanceof VideoFormat)
            {
               videoTrack_ = tracks[i];
               videoFormat = (VideoFormat)tracks[i].getFormat();
               break;
            }
         }

         if (videoTrack_ == null)
            throw new NoPlayerException("Video did not contain a video track.");



         if (true)
         {
            //
            // Sun-style version of finding the chain
            //
            Vector formati = new Vector();
            Vector chain = com.sun.media.SimpleGraphBuilder.findRenderingChain(videoFormat, formati);

            System.out.println("--- Chain ---");
            printOut(chain);
            System.out.println("--- Formats ---");
            printOut(formati);
            
            
            videoRenderer_ = (VideoRenderer)chain.get(0);
            videoCodec_ = (Codec)chain.get(1);

            videoFormat_ = (VideoFormat)formati.get(0);
            videoCodec_.setOutputFormat(videoFormat_);


         }
         else
         {
            //System.out.println("track: "+videoTrack_);

            //
            // find a decoder
            //
            iter = PlugInManager.getPlugInList(
               videoFormat, null, PlugInManager.CODEC).iterator();
            String decoderClassName = null;
            if (iter.hasNext())
            {
               decoderClassName = (String)iter.next();
               videoCodec_ = (Codec)loadClassInstance(decoderClassName);
            }

            if (videoCodec_ == null)
               throw new NoPlayerException("No codec for the video format "+videoFormat);

            //System.out.println("codec: "+videoCodec_);

            //
            // find an appropriate renderer for one of the output format
            //
            videoCodec_.setInputFormat(videoFormat);
            Format[] formats = videoCodec_.getSupportedOutputFormats(videoFormat);
            Format itsFormat = null;
            for (int i=0; i<formats.length && videoRenderer_ == null; i++)
            {
               //System.out.println("trying format "+formats[i]);
               //*
               iter = PlugInManager.getPlugInList(
                  formats[i], null, PlugInManager.RENDERER).iterator();
               while (iter.hasNext())
               {
                  String renderer = (String)iter.next();
                  //System.out.println("  renderer "+renderer);
                  //if (renderer.endsWith("GDIRenderer"))
                  {
                     VideoRenderer r = (VideoRenderer)loadClassInstance(renderer);
                     if (r != null)
                     {
                        r.setInputFormat(formats[i]);

                        Component c = r.getComponent();
                        Dimension d = c.getPreferredSize();
                        //System.out.println("       "+d.width+"x"+d.height);

                        if (videoRenderer_ == null && d.width > -1)
                        {
                           videoRenderer_ = r;
                           itsFormat = formats[i];

                           break;
                        }
                     }
                  }
               }
               //*/
            }


            if (videoRenderer_ == null)
               throw new NoPlayerException("No video renderer for codec output "+formats[0]);



            videoCodec_.setOutputFormat(itsFormat);


            videoFormat_ = (VideoFormat)itsFormat;

            //System.out.println("input support: "+videoRenderer_.setInputFormat(videoFormat_));


            //System.out.println("format: "+itsFormat);
            //System.out.println("renderer: "+videoRenderer_);

         } // else of if (true | false)

         videoCodec_.open();
         videoRenderer_.open();

         com.sun.media.ExtBuffer cb = new com.sun.media.ExtBuffer();
         com.sun.media.ExtBuffer db = new com.sun.media.ExtBuffer();
         codedBuffer_ = cb;
         decodedBuffer_ = db;

         //codedBuffer_ = new Buffer();
         //decodedBuffer_ = new Buffer();

         timeBase_ = new VideoTimeBase();

         float rate = videoFormat.getFrameRate();
         nanosPerFrame_ = (long)(1000000000L/rate);
         frameCount_ = (int)(videoDemux_.getDuration().getNanoseconds()/nanosPerFrame_);
         demuxFrameNumber_ = 0;
         syncNanos_ = 0;
         syncedWhen_ = timeBase_.getNanoseconds();


         lastJumpMillis_ = new long[4];
         long time = System.currentTimeMillis();
         for (int i=0; i<lastJumpMillis_.length; i++)
            lastJumpMillis_[i] = time;

         /*
         Thread t = new Thread() 
         {
            public void run()
            {
               try { sleep(10000); } catch (InterruptedException e) {}

               for (int i=0; i<20; i++)
               {
                  jumpToMillis(5000+i*500);
                  try { sleep(500); } catch (InterruptedException e) {}

               }
               
               for (int i=0; i<400; i++)
               {
                  jumpToMillis(10000+i*100);
                  try { sleep(50); } catch (InterruptedException e) {}

               }

            }
         };
         t.start();
         */

         //
         // display the first frame (or at least prepare it)
         //
         jumpToMillis(0);

      }
      catch (Exception e)
      {
         if (e instanceof NoPlayerException)
            throw (NoPlayerException)e;
         else
         {
            e.printStackTrace();
            throw new NoPlayerException("Data access failed due to "+e);
         }
      }
    }


   public Component getVisualComponent()
   {
      return videoRenderer_.getComponent();
   }

   public Dimension getVideoSize()
   {
      return videoFormat_.getSize();
   }

   public synchronized void jumpToMillis(int millis)
   {
      if (!running_)
      {
         long nanos = millis*1000000L;
         long frameNumber = nanos/nanosPerFrame_;
         if (frameNumber < 0)
            frameNumber = 0;
         else if (frameNumber > frameCount_)
            frameNumber = frameCount_;

         if (frameNumber != demuxFrameNumber_)
         {
            
            for (int i=lastJumpMillis_.length-1; i>0; i--)
               lastJumpMillis_[i] = lastJumpMillis_[i-1];
            lastJumpMillis_[0] = System.currentTimeMillis(); 
            
            if (SMART_SCROLLING)
            {
               if (isFastScrolling_) // check if switch to precise scrolling is appropriate
               {
                  boolean allDiffsMajor = true;
                  for (int i=1; i<lastJumpMillis_.length; i++)
                     if (lastJumpMillis_[i-1] - lastJumpMillis_[i] <= 120)
                     {
                     //System.out.print("major=false ");
                     allDiffsMajor = false;
                     }

                  if (allDiffsMajor == true)
                  {
                     isFastScrolling_ = false;
                     for (int i=1; i<lastJumpMillis_.length; i++)
                        System.out.print((lastJumpMillis_[i-1] - lastJumpMillis_[i])+" ");
                     System.out.println("s");
                  }
               }
               else // check if switch to fast scrolling is appropriate
               {
                  boolean allDiffsMinor = true;
                  for (int i=1; i<lastJumpMillis_.length; i++)
                     if (lastJumpMillis_[i-1] - lastJumpMillis_[i] > 120)
                     {
                     //System.out.print("minor=false ");
                     allDiffsMinor = false;
                     }

                  if (allDiffsMinor == true)
                  {
                     isFastScrolling_ = true;
                     for (int i=1; i<lastJumpMillis_.length; i++)
                        System.out.print((lastJumpMillis_[i-1] - lastJumpMillis_[i])+" ");
                     System.out.println("f");
                  }
               }
            }
             
            
            if (eomReached_)
            {
               if (codedBuffer_.isEOM())
               {
                  codedBuffer_.setEOM(false); // otherwise it keeps this flag
                  // and spoils the rendering process
                  decodedBuffer_.setEOM(false);
               }
               eomReached_ = false;
            }
            syncToExternalMillis(millis);
            showSingleFrame_ = true;

            if (SMART_SCROLLING && isFastScrolling_)
            {
               renderKeyFramesOnly_ = true;
               testOrActivateDelayThread();
            }


            testOrStartAccessThread();

         }
      }

      else
      {
         stop();
         jumpToMillis(millis);
         start();
      }

   }

   //
   // from outside use this only for fine tuning (some milliseconds)
   // from inside it is also used for setting global variables
   //
   public void syncToExternalMillis(int millis)
   {
      long nanos = millis*1000000L;
      //if (running_ && nanos < syncNanos_)
      //   throw new RuntimeException("No jump backwards allowed during replay!");
      syncNanos_ = nanos;
      syncedWhen_ = timeBase_.getNanoseconds();
   }

   public int getMediaMillis()
   {
      return (int)((demuxFrameNumber_*nanosPerFrame_)/1000000);
   }

   public Time getDuration()
   {
      return new Time(frameCount_*nanosPerFrame_);
   }

   public synchronized void start()
   {
      if (running_ || eomReached_)
         return;

      
      running_ = true;
      videoRenderer_.start();
      if (delayThread_ != null && delayThread_.isAlive())
         delayThread_.deactivate();
      testOrStartAccessThread();
   }

   public void stop()
   {
      if (!running_)
         return;
      
      running_ = false;
      videoRenderer_.stop();
   }

   public void actionPerformed(ActionEvent e) // delay is up
   {
      renderKeyFramesOnly_ = false;
      showSingleFrame_ = true;
      testOrStartAccessThread();
   }

   public void run ()
   {
      
      while (true) // GDIRenderer needs always the same Thread accessing it...
      {
         while (running_ || showSingleFrame_)
         {
            long externalSyncNanos = syncNanos_ + timeBase_.getNanoseconds() - syncedWhen_;
            long currentShownNanos = demuxFrameNumber_*nanosPerFrame_;

            if (externalSyncNanos > currentShownNanos)
            {
               if (showSingleFrame_)
               {
                  showSingleFrame_ = false;

                  // for greater jumps in stopped mode
                  int diffMillis = (int)((syncNanos_ - currentShownNanos)/1000000);

                  boolean fallBackNeeded = true;
                  if (videoDemux_ instanceof AviUnbugParser)
                  {
                     Time t = ((AviUnbugParser)videoDemux_).findPreviousKeyFrame(new Time(syncNanos_));
                     if (t != null)
                     {
                        fallBackNeeded = false;

                        long keyFrameNanos = t.getNanoseconds(); 
                     
                        //System.out.println((keyFrameNanos/1000000)+" vs. "+(currentShownNanos/1000000));
                     

                        if (keyFrameNanos > currentShownNanos)
                           changeDemuxPosition(syncNanos_);
                     }
                  }

                  if (fallBackNeeded && diffMillis > 1000) // fall back
                  {
                     changeDemuxPosition(syncNanos_);
                  }
               }

               int desiredFrameNumber = (int)(externalSyncNanos/nanosPerFrame_);

               boolean validFrameFound = false;
               if (desiredFrameNumber-demuxFrameNumber_ <= 1)
                  validFrameFound = true;
               while (demuxFrameNumber_ < desiredFrameNumber)
               {
                  videoTrack_.readFrame(codedBuffer_);
                  //System.out.println((codedBuffer_.getDuration()/1000000));
                  //System.out.print(" b:"+(codedBuffer_.getTimeStamp()/1000000));
                  if (codedBuffer_.isEOM())
                  {
                     eomReached_ = true;
                     running_ = false;
                  }
                  if (codedBuffer_.getDuration() > 0)
                  {
                     demuxFrameNumber_++;

                     videoCodec_.process(codedBuffer_, decodedBuffer_);
                     //System.out.print(" deceom="+decodedBuffer_.isEOM());
                     //System.out.print(" d:"+(decodedBuffer_.getTimeStamp()/1000000));
                     if (!validFrameFound && demuxFrameNumber_ == desiredFrameNumber)
                        validFrameFound = true;
                     //System.out.println(" valid="+validFrameFound+" keyF="+renderKeyFramesOnly_);
                     if (validFrameFound || renderKeyFramesOnly_)
                     {
                        int suc = videoRenderer_.process(decodedBuffer_);
                        long time = System.currentTimeMillis();
                        System.out.print("d"+(time-lastDisplayTime_)+" ");
                        lastDisplayTime_ = time;
                        //System.out.print((codedBuffer_.getTimeStamp()/1000000)
                        //                 +(renderKeyFramesOnly_ ? "k" : "i")+" ");

                     }
                     if (renderKeyFramesOnly_)
                     {
                        break;
                     }
                  }
               }

               // wait for the time to display next frame
               if (running_)
               {
                  externalSyncNanos = syncNanos_ + timeBase_.getNanoseconds() - syncedWhen_;
                  currentShownNanos = demuxFrameNumber_*nanosPerFrame_;
                  int nextFrameNumber = (int)(externalSyncNanos/nanosPerFrame_) + 1;
                  //(externalSyncNanos%nanosPerFrame_ != 0 ? 1 : 0);
                  if (nextFrameNumber == demuxFrameNumber_)
                     nextFrameNumber++;
                  int waitMillis = (int)((nextFrameNumber*nanosPerFrame_ - externalSyncNanos)/1000000);
                  if (waitMillis > 0)
                     try { Thread.sleep(waitMillis); } catch (InterruptedException e) {}
               }
            }
            else // sync was backward
            {
               if (showSingleFrame_) // ie !running_
                  changeDemuxPosition(syncNanos_);
            }

         } // while(running ...)

         synchronized(onHold_) { try { onHold_.wait(); } catch (InterruptedException e) {}  }
      }
   }

   private void testOrStartAccessThread()
   {
      if (accessThread_ == null)
      {
         accessThread_ = new Thread(this);
         accessThread_.start();
      }
      else
      {
          if (!accessThread_.isAlive())
             throw new RuntimeException("Fatal incident: Thread-death not allowed!");

          synchronized (onHold_) { onHold_.notify(); }
      }
   }

   private void testOrActivateDelayThread()
   {
      if (delayThread_ == null || !delayThread_.isAlive())
      {
         delayThread_ = new DelayedTimer(this, 200);
         delayThread_.activate();
      }
      else
         delayThread_.delayFurther();
   }

   private void changeDemuxPosition(long nanos)
   {
      //System.out.println("change");
      Time t = videoDemux_.setPosition(new Time(nanos), Positionable.RoundDown);
      videoCodec_.reset();
      videoRenderer_.reset();
      demuxFrameNumber_ = (int)(t.getNanoseconds()/nanosPerFrame_);
   }

   private Object loadClassInstance(String name)
   {
      try
      {
         Class c = Class.forName(name);
         return c.newInstance();
      } 
      catch (Exception e)
      {
         e.printStackTrace();
         return null;
      }
   }


   private void printOut(Vector v)
   {
      if (v == null)
      {
         System.out.println("Vector is null!");
         return;
      }
      Iterator iter = v.iterator();
      while(iter.hasNext())
      {
         System.out.println(iter.next());
      }
   }
   
   private void printOut(long[] l)
   {
      for (int i=0; i<l.length; i++)
         System.out.print(l[i]+" ");
   }

}