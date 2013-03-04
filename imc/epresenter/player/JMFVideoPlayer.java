package imc.epresenter.player;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import java.lang.ref.WeakReference;
import java.net.URL;
import java.net.MalformedURLException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Vector;
import javax.media.*;
import javax.media.control.TrackControl;
import javax.media.control.FrameGrabbingControl;
import javax.media.format.AudioFormat;
import javax.media.renderer.VideoRenderer;
import javax.swing.*;
import javax.swing.border.Border;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.media.parser.video.AviUnbugParser;
import imc.epresenter.media.codec.video.LsgcDecoder;
import imc.epresenter.player.util.*;
import imc.epresenter.player.Manager;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.ui.LayoutUtils;

public class JMFVideoPlayer implements SRStreamPlayer, Runnable
{
   public static int HANDLE_VIDEO_OR_STILL = 1;
   public static int HANDLE_CLIPS_ONLY = 2;
   public static int HANDLE_DEFAULT = HANDLE_VIDEO_OR_STILL | HANDLE_CLIPS_ONLY;
   
   
   private static boolean libraryLoaded_;

   static boolean DEBUG = false;
   static
   {
      if (System.getProperty("app.videodebug") != null)
         DEBUG = true;
   }

   static 
   {
      try 
      {
         Class.forName("javax.media.Manager");
         libraryLoaded_ = true;
      
         // PZI: use lightweight components for JMF videos
         // otherwise the tooltips used by full text search are behind the clip and not fully visible
         // TODO: setting LIGHTWEIGHT_RENDERER causes artefacts after resizing
         // javax.media.Manager.setHint(javax.media.Manager.LIGHTWEIGHT_RENDERER, new Boolean(true));
         
         String imcPrefix = new String("imc.epresenter");

         // check for correct package prefixes for the java media
         // framework protocols. here we know that the JMF is
         // installed, so it is safe to do this:
         Vector prefixList1 = PackageManager.getProtocolPrefixList();

         // has our prefix already been added?
         if (!prefixList1.contains(imcPrefix))
         {
            // no, add it!
            prefixList1.addElement(imcPrefix);
            // set the new prefix list (with our prefix added)
            PackageManager.setProtocolPrefixList(prefixList1);
            // and commit the changes:
            //PackageManager.commitProtocolPrefixList();
         }

         // now the "epf://" protocol should be recognized


         //
         // add wrapper for direct native access to LSGC decoding
         // (without the help of the codec being installed)
         //
         LsgcDecoder decoder = new LsgcDecoder();

         // first it needs to be added ...
         PlugInManager.addPlugIn("imc.epresenter.media.codec.video.LsgcDecoder",
                                 decoder.getSupportedInputFormats(), 
                                 decoder.getSupportedOutputFormats(null),
                                 PlugInManager.CODEC);
                                 
         
         // ... then the list can be reordered, as LsgcDecoder must be 
         // positioned before NativeDecoder (which neglects padding)
         Vector cL = PlugInManager.getPlugInList(null, null, PlugInManager.CODEC);
         for (int i=0; i<cL.size(); i++)
         {
            if (((String)cL.get(i)).equals("imc.epresenter.media.codec.video.LsgcDecoder"))
            {
               if (i != 0)
                  cL.add(0, cL.remove(i));
               break;
            }
         }
         PlugInManager.setPlugInList(cL, PlugInManager.CODEC);

         //
         // replace buggy AviParser
         //
         Format[] in = PlugInManager.getSupportedInputFormats(
            "com.sun.media.parser.video.AviParser", 
            PlugInManager.DEMULTIPLEXER);
         Format[] out = PlugInManager.getSupportedOutputFormats(
            "com.sun.media.parser.video.AviUnbugParser", 
            PlugInManager.DEMULTIPLEXER);

         if (in != null && in.length > 0)
         {
            // we need proper output formats for adding the UnbugParser
            // so it can only be done if the AviParser is avaible;
            // which then of course must be removed 
            PlugInManager.removePlugIn("com.sun.media.parser.video.AviParser", 
                                       PlugInManager.DEMULTIPLEXER);
            PlugInManager.addPlugIn("imc.epresenter.media.parser.video.AviUnbugParser",
                                    in, out, PlugInManager.DEMULTIPLEXER);
         }


         //
         // replace not so buggy vcm.NativeDecoder to fix padding problem
         //
         Format[] in2 = PlugInManager.getSupportedInputFormats(
            "com.sun.media.codec.video.vcm.NativeDecoder", 
            PlugInManager.CODEC);
         Format[] out2 = PlugInManager.getSupportedOutputFormats(
            "com.sun.media.codec.video.vcm.NativeDecoder", 
            PlugInManager.CODEC);
        
         /* The NativeDecoderPadded is not compiled anymore (2.0.p2) as it isn't used anyway.
         PlugInManager.removePlugIn("com.sun.media.codec.video.vcm.NativeDecoder",
                                    PlugInManager.CODEC);
         PlugInManager.addPlugIn("imc.epresenter.media.codec.video.vcm.NativeDecoderPadded",
                                 in2, out2, PlugInManager.CODEC);
         //*/

         //
         // rearrange DDRenderer behind GDIRenderer (the DD one is bad)
         //
         Vector pL = PlugInManager.getPlugInList(null, null, PlugInManager.RENDERER);

         int gdiIdx = -1;
         int ddrIdx = -1;
         for (int i=0; i<pL.size(); i++)
         {
            //System.out.println(pL.get(i)+" vs "+pL.get(i).getClass().getName());

            if (pL.get(i).equals("com.sun.media.renderer.video.GDIRenderer"))
               gdiIdx = i;
            if (pL.get(i).equals("com.sun.media.renderer.video.DDRenderer"))
               ddrIdx = i;

         }

         //
         // actually if present the DDRenderer is temporarily removed
         // 
         if (ddrIdx != -1)
         {
            pL.remove(ddrIdx);
            PlugInManager.setPlugInList(pL, PlugInManager.RENDERER);
         }
      
      } 
      catch (NoClassDefFoundError e) 
      {
         // Somehow both cases do not work (are not accessed)
         // the error occurs upon calling the constructor, so from external code,
         // and must be caught there, weird...
         libraryLoaded_ = false;
         Manager.showError(Manager.getLocalized("jmfFail"), Manager.WARNING, e);
      }
      catch (ClassNotFoundException ex) 
      {
         libraryLoaded_ = false;
         Manager.showError(Manager.getLocalized("jmfFail"), Manager.WARNING, ex);
      }

   }


   private Coordinator       coordinator_;
	private String[]          videoUrls_;
   private Player[]          players_;
   private boolean[]         isClipPlayer_;
   private AviUnbugParser[]  parsers_;
   private int[]             startAndStopTimes_; // has 2x the entries of players_
   private int               bigVideoIndex_ = -1;
   private boolean           fullScreenActive_;
   private boolean           playerPresent_ = false;
   private boolean           normalVideoPresent_ = false;
   private Player            activePlayer_ = null;
   private boolean           coveringPlayerOnly_ = false;
   private boolean           denverWorkaround_ = false;
	private VideoTimeBase     timeBase_;
	private JComponent        visualComponent_;
   private JLabel            stillImageComponent_;
   private boolean           closed_;
	private boolean           started_;
	private boolean           scrolling_;
   private int               bugAroundCounter_;
   private int               lastMillis_ = 0;
   private Thread            cleanupHook_;
  
   //
   // global variables for double thread variant
   //
   private Thread            managementThread_ = null;
   private long              threadStartTime_ = 0;
   private int               syncMillis_          = 0;
   private boolean           afterComponentUpdate_ = false;
   private boolean           runOnceMore_ = false;

   //
   // global variables for clips on slides variant
   // 
   private int               nAssignmentMask_ = HANDLE_DEFAULT;
   private JComponent        visualComponentClips_ = null;
 
   public JMFVideoPlayer(int nAssignmentMask)
   {
      nAssignmentMask_ = nAssignmentMask;
   }
  
   //
   // Interface SRStreamPlayer
   //
   public void setStartOffset(int millis)
   {
   }
   
   public void init(FileResources resources, String[] args, Coordinator c) {
      coordinator_ = c;

      boolean doHandleVideoOrStill = (nAssignmentMask_ & HANDLE_VIDEO_OR_STILL) != 0;
      boolean doHandleClips = (nAssignmentMask_ & HANDLE_CLIPS_ONLY) != 0;
      boolean doClipsOnSlides = doHandleClips && !doHandleVideoOrStill;
               
      //
      // parse parameters and initialize arrays
      //
      String givenStillImage = null;
      boolean givenNormalVideo = false;
      boolean givenMultiVideo = false;
      ArrayList videoUrlCollector = new ArrayList(10);

      if (args.length == 1) // old style specification for one video
      {
         videoUrlCollector.add(resources.getResourceURL(args[0]));
         givenNormalVideo = true;
      }
      else
      {
         
         for (int i=0; i<args.length; )
         {
            if (args[i].equals("-video") && !givenNormalVideo)
            {
               if (doHandleVideoOrStill && i+1 < args.length)
               {
                  videoUrlCollector.add(resources.getResourceURL(args[i+1]));
                  givenNormalVideo = true;
               }
               i += 2;
            }
            else if (args[i].equals("-still") && givenStillImage == null)
            {
               if (doHandleVideoOrStill && i+1 < args.length)
                  givenStillImage = args[i+1];
               i += 2;
            }
            else if (args[i].equals("-multi"))
            {
               i += 1;

               // this must be the last (qualified) parameter:
               int multiVideoCount = args.length - i;
               
               if (multiVideoCount % 2 == 0)
               {
                  multiVideoCount /= 2; // every video has an offset

                  startAndStopTimes_ = new int[multiVideoCount*2];

                  for (int v=0; v<multiVideoCount; ++v)
                  {
                     if (doHandleClips)
                     {
                        try 
                        {
                           startAndStopTimes_[2*v] = Integer.parseInt(args[i+1]);
                           startAndStopTimes_[2*v+1] = startAndStopTimes_[2*v]+1; // fallback
                      
                           videoUrlCollector.add(resources.getResourceURL(args[i]));
                     
                           givenMultiVideo = true;

                        }
                        catch (NumberFormatException exc)
                        {
                        }
                     }
                     
                     i += 2;
                  }
               }
               else
                  i += multiVideoCount; // make the loop stop
            } // -multi
         } // for args
      } // args.length > 1

      //
      // load still image
      // 
      if (givenStillImage != null)
      {
         Image stillImage = null;
         IOException exception = null;
         try
         {
            int fileSize = (int)resources.getFileSize(givenStillImage);
            byte[] imageData = new byte[fileSize];
            DataInputStream dis = new DataInputStream(
               resources.createInputStream(givenStillImage));
            dis.readFully(imageData);
            dis.close();
            stillImage = Toolkit.getDefaultToolkit().createImage(imageData);
         }
         catch (IOException ex)
         {
            exception = ex;
         }
         ImageIcon icon = new ImageIcon(stillImage);
         if (icon.getIconWidth() > 0)
         {
            stillImageComponent_ = new JLabel(icon) {
               public void paint(Graphics g) // does correct positioning and scaling
               {
                  Icon pIcon = getIcon();
                  if (pIcon != null && pIcon instanceof ImageIcon)
                  {
                     ImageIcon iIcon = (ImageIcon)pIcon;

                     Dimension p = new Dimension(
                        pIcon.getIconWidth(), pIcon.getIconHeight());
                     Dimension d = getSize();


                     LayoutUtils.scale(p, d, 1.0f);

                     if (getLayout() == null)
                        g.translate(getLocation().x, getLocation().y);

                     g.drawImage(iIcon.getImage(), 
                                 (d.width-p.width)/2, 
                                 (d.height-p.height)/2,
                                 p.width, p.height, this);
                  }
               }
            };
            
            playerPresent_ = true; // hmpf!
         }
         else
         {
            String errorMessage = Manager.getLocalized("stillFail");
            errorMessage += "\n"+givenStillImage;
            Manager.showError(errorMessage, Manager.WARNING, exception);
            
         }
      } // load still image
     
      //
      // try to load the videos
      //
      if (givenNormalVideo || givenMultiVideo)
      {
         if (!libraryLoaded_)
         {
            Manager.showError(Manager.getLocalized("jmfFail"), Manager.WARNING, null);
            players_ = new Player[0];
            parsers_ = new AviUnbugParser[0];
         }
         else
         {
            players_ = new Player[videoUrlCollector.size()];
            isClipPlayer_ = new boolean[videoUrlCollector.size()];
            parsers_ = new AviUnbugParser[videoUrlCollector.size()];

            if (givenNormalVideo && givenMultiVideo) // make the normal video the last one
               videoUrlCollector.add(videoUrlCollector.remove(0));

            videoUrls_ = new String[videoUrlCollector.size()];
            videoUrlCollector.toArray(videoUrls_);

            ArrayList exceptionCollector = new ArrayList(videoUrls_.length*2);
         
            timeBase_ = new VideoTimeBase();

            for (int i=0; i<videoUrls_.length; i++)
            {
               // some (or all) players may fail 
               // these will not be included in the players_ array 
               // (players_[i] remains null) and their stop time is 1 ms after the
               // start time
             
               try {
                  Player player = 
                     javax.media.Manager.createRealizedPlayer(
                        new MediaLocator(videoUrls_[i]));

                  // at least one player did not fail
                  playerPresent_ = true;
                  if (givenNormalVideo && i == videoUrls_.length -1)
                     bigVideoIndex_ = i;


                  int duration = (int)(player.getDuration().getNanoseconds()/1000000);
                  if (!givenNormalVideo || i != videoUrls_.length -1)
                     startAndStopTimes_[2*i+1] = startAndStopTimes_[2*i]+duration;
      
                  player.setTimeBase(timeBase_);
           
                  // if there is an audio track disable that
                  Control[] cs = player.getControls();
                  for (int j=0; j<cs.length; j++)
                  {
                     if (cs[j] instanceof TrackControl)
                     {
                        TrackControl tc = (TrackControl)cs[j];
                        if (tc.getFormat() instanceof AudioFormat)
                           tc.setEnabled(false);
                     }
                  }
               
                  players_[i] = player;
                  if (givenNormalVideo && i == players_.length-1)
                     isClipPlayer_[i] = false;
                  else
                     isClipPlayer_[i] = true;


                  //
                  // dodgy workaround/bugfix for the problem that a native renderer
                  // (eg GDIRenderer) doesn't get garbage collected
                  //
                  // registers the last created instance with the name videoURL_ as key
                  //
                  parsers_[i] = AviUnbugParser.register(videoUrls_[i]);

                  /* might be handy for future problems
                  Vector formats = new Vector();
                  Vector plugins = com.sun.media.SimpleGraphBuilder.findRenderingChain(
                     parsers_[i].getTracks()[0].getFormat(), formats); 
                     //*/

               } catch (Exception e) {
                  exceptionCollector.add(videoUrls_[i]);
                  exceptionCollector.add(e);

                  if (givenNormalVideo && i == videoUrls_.length -1)
                     givenNormalVideo = false;
               }
            } // for all videoURLs
           
            normalVideoPresent_ = givenNormalVideo;

            //
            // install a cleanup hook to avoid the DIVX-crashes
            //
            cleanupHook_ = new Thread() {
               public void run()
               {
                  if (!closed_)
                     close(null, false);
               }
            };
            Runtime.getRuntime().addShutdownHook(cleanupHook_);
            

            //
            // display error message if one video failed
            //
            if (exceptionCollector.size() > 0)
            {
               Exception firstException = (Exception)exceptionCollector.get(1);

               String errorMessage = Manager.getLocalized("videoFail1")+"\n";

               boolean someTextFound = false;

               for (int i=0; i<exceptionCollector.size()/2; i++)
               {
                  Exception e = (Exception)exceptionCollector.get(i*2+1);
                  String file = (String)exceptionCollector.get(i*2);

                  if (file != null)
                  {
                     file = file.replace('\\', '/');
                     if (file.indexOf('/') > -1)
                        file = file.substring(file.lastIndexOf('/')+1);

                     errorMessage += "\n"+file+": "+e.getClass().getName();

                     someTextFound = true;
                  }
               }

               errorMessage += "\n\n"+Manager.getLocalized("videoFail2");

               if (someTextFound)
                  Manager.showError(errorMessage, Manager.WARNING, null);
            }
         } // libraryLoaded
      } // givenNormalVideo || givenMultiVideo // try loading the videos
      
      //
      // prepare the visual component(s)
      // 
      int maximumWidth = Integer.MIN_VALUE;
      int maximumHeight = Integer.MIN_VALUE;

      int playerCount = 0;

      if (players_ != null) // some video given
      {
         for (int i=0; i<players_.length; i++)
         {
            if (players_[i] != null)
            {
               ++playerCount;

               if (!(doClipsOnSlides && isClipPlayer_[i]))
               {

                  Component playerComponent = players_[i].getVisualComponent();
                  int videoWidth = playerComponent.getPreferredSize().width;
                  int videoHeight = playerComponent.getPreferredSize().height;

                  if (videoWidth > maximumWidth)
                     maximumWidth = videoWidth;
                  if (videoHeight > maximumHeight)
                     maximumHeight = videoHeight;
               }
            }
         }
      } // players_ != null

      // check for the very unlikely case that there is both:
      // still image and background video; in that case: the video rulez!
      boolean stillImageValid = 
         stillImageComponent_ != null && !givenNormalVideo;
      int siv = stillImageValid ? 1 : 0;
     
      Component[] components = new Component[playerCount + siv];
      if (playerCount > 0)
      {
         
         playerCount = 0;
         for (int i=0; i<players_.length; i++)
         {
            if (players_[i] != null)
               components[playerCount++] = players_[i].getVisualComponent();
         }
      }
      if (stillImageValid)
         components[components.length-1] = stillImageComponent_;
            
      if (doClipsOnSlides)
      {
         if (stillImageValid || givenNormalVideo)
         {
            if (components.length > 1) // there is at least one clip AND another video/image
            {
               Component[] components1 = 
                  new Component[] { components[components.length-1] };
               Component[] components2 = 
                  new Component[components.length-1];
               System.arraycopy(components, 0, components2, 0, components2.length);

               visualComponent_ = new VideoVisiblePanel(components1, false, true);
               visualComponentClips_ = new VideoVisiblePanel(components2, true, false);

               // for latter (see below) size adjustments
               // do not use the clips
               components = components1;
            }
            else // there is only a normal video/image
            {
               visualComponent_ = new VideoVisiblePanel(components);
            }

         }
         else if (components.length > 0) // there are only clips
         {
            // Bug 1456: Due to changes outside of here (Document) 
            // video and clips are always separated (in different VideoPlayer instances),
            // so if we want to have the blurred border painted we have
            // to specify "true" here (as above):
            visualComponentClips_ = new VideoVisiblePanel(components, true, false);
         }
      }
      else // old mode: switch normal video and clips
      {
         visualComponent_ = new VideoVisiblePanel(components);
      }

      if (visualComponent_ != null)
      {
         // pSize is used when not stand-alone mode
         // otherwise mSize is used from the Layout class
         Dimension pSize = new Dimension(300, 200);
         Dimension mSize = new Dimension(900, 600);

         if (maximumWidth > 0)
         {
            // for mode Denver:
            mSize = new Dimension(maximumWidth, maximumHeight);
            pSize = new Dimension(maximumWidth, maximumHeight);
            
            int widthBoundary = 360;
            int heightBoundary = 300;

            if (stillImageValid)
            {
               // if a still image is present and not too small
               // use it to downsize the video sizes
               Dimension sSize = stillImageComponent_.getPreferredSize();
               if (sSize.width >= 280 && sSize.height >= 220)
               {
                  widthBoundary = sSize.width;
                  heightBoundary = sSize.height;
               }
            }

            if (maximumWidth > widthBoundary || maximumHeight > heightBoundary)
               LayoutUtils.scale(
                  pSize, new Dimension(widthBoundary, heightBoundary), 1.0f);
            
            if (!doClipsOnSlides && givenNormalVideo)
            {
               // avoid making the video area bigger than the normal video
               
               Dimension normalVideoSize = 
                  components[components.length-1].getPreferredSize();
               
               if (normalVideoSize.width >= 280 && normalVideoSize.height >= 220)
                  if (pSize.width > normalVideoSize.width || pSize.height > normalVideoSize.height)
                     pSize = new Dimension(normalVideoSize);
            }
         }
         else if (stillImageValid)
         {
            // in this case use the size of the still image (below)
            pSize = new Dimension(0, 0);
         }

         if (stillImageValid)
         {
            Dimension sSize = stillImageComponent_.getPreferredSize();
            if (sSize.width > pSize.width)
               pSize.width = sSize.width;
            if (sSize.height > pSize.height)
               pSize.height = sSize.height;
         }

         visualComponent_.setPreferredSize(pSize);
         visualComponent_.setMaximumSize(mSize);
      } // visualComponent_ != null meaning old mode or video/image given
         
      //System.out.println(visualComponent_+", "+visualComponentClips_);

 
   } // init
	
	
	
	public void enableVisualComponents(boolean bEnable)
   {
      // not implemented here
   }

	public Component[] getVisualComponents() {
      if ((nAssignmentMask_ & HANDLE_VIDEO_OR_STILL) != 0) // this or HANDLE_DEFAULT
          return new Component[] { visualComponent_ };
      else
          return new Component[] { visualComponentClips_ };
	}

	public String getTitle() {
		return "JMFVideoPlayer";
	}	

	public String getDescription() {
		return "Replay of an video stream with JMF.";
	}


   public void activateFullScreen()
   {
      fullScreenActive_ = true;

      startThread(true);
     
  }

   public void deactivateFullScreen()
   {
      fullScreenActive_ = false;

      startThread(true);
    
   }

   public void waitForComponentUpdate()
   {
      /*
      boolean wasAfter = afterComponentUpdate_;
      if (managementThread_ != null && managementThread_.isAlive())
      {
         synchronized(managementThread_) 
         { 
            try { managementThread_.wait(); } catch (InterruptedException exc) {}
            if (wasAfter && managementThread_ != null)
               try { managementThread_.wait(1000); } catch (InterruptedException exc) {}
         }
      }
      //*/
     
   }
	

   public void start(EventInfo info)
   {
      if (!playerPresent_)
         return;
      started_ = true;
      scrolling_ = false;
      
      startThread();
   }

   public void pause(EventInfo info)
   {
      if (!playerPresent_)
         return;
      scrolling_ = true;
      
      
      
   }

   public void stop(EventInfo info)
   {
      if (!playerPresent_)
         return;
      started_ = false;
      
   }

   public void setMediaTime(int millis, EventInfo info)
   {
      if (!playerPresent_)
         return;

      syncMillis_ = millis;
      
      startThread();
   }

   //
   // main management loop for double thread variant
   //
   public void run()
   {
      int beginSyncMillis = 0;
      do
      {
         //System.out.print(".");

         afterComponentUpdate_ = false;
         beginSyncMillis = syncMillis_;

         int index = findMatchingPlayerIndex(syncMillis_);

         Player matchingPlayer = null;
         AviUnbugParser matchingParser = null;
         int playerTime = syncMillis_;
         boolean durationExceeded = false;
         if (index > -1)
         {
            matchingPlayer = players_[index];
            matchingParser = parsers_[index];
            if (startAndStopTimes_ != null && index*2 < startAndStopTimes_.length)
               playerTime -= startAndStopTimes_[index*2];

            if (matchingPlayer != null)
            {
               // players may not get time stamps greater than their duration
               // or else they'll do nothing (but should display the last frame)
               double matchingDuration = matchingPlayer.getDuration().getSeconds()*1000;
               if (playerTime >= matchingDuration)
               {
                  playerTime = (int)(matchingDuration) - 100;
                  if (matchingPlayer.getState() == Controller.Started)
                     matchingPlayer.stop();
                  durationExceeded = true;
               }
               else if (playerTime < 0)
                  playerTime = 0;
            }
         }

         // TODO: make this blocking somehow
         Player newActivePlayer = null;
        
         // sets the right component visible
         // if none is eligible null is returned
         newActivePlayer = updateComponents(matchingPlayer, activePlayer_);
         
         afterComponentUpdate_ = true;
         boolean bugAroundReset = false;
         
         // if there was a player (component) change stop the previous one
         if (activePlayer_ != null && activePlayer_ != newActivePlayer)
         {
            activePlayer_.stop();
            bugAroundReset = true;
         }
         activePlayer_ = newActivePlayer;
           
         if (activePlayer_ != null)
         {
            boolean running = started_ && !scrolling_;
      
            if (running && !durationExceeded)
            {
               boolean isBigVideo = bigVideoIndex_ > -1 && activePlayer_ == players_[bigVideoIndex_];
                  
               if (activePlayer_.getState() == Controller.Started)
               {
                  if (fullScreenActive_ && isBigVideo)
                  {
                     activePlayer_.stop();
                  }
                  else
                  {
                     syncPlayer(activePlayer_, playerTime);
                  }
               }
               else // state != Started but should be
               {
                  if (fullScreenActive_ && isBigVideo)
                  {
                     // do not start normal video in full screen
                  }
                  else if (playerTime > 0)
                  {
                     // start video only if it's timestamp is greater than 0
                     // otherwise there might be a long time with only 
                     // timestamp 0 when e.g. positive video offset is given
                     // in that case the video will be slowed down to a halt
                     // and after it's starting point sped up, which is undesired
                     activePlayer_.setMediaTime(new Time(playerTime*1000000L));
                     activePlayer_.start();
                     bugAroundCounter_ = 0;
                  }
                  else
                  {
                     // not sure if this is needed:
                     // if replay (audio) is started but the video is not
                     // make sure that at least a (the first one) frame is
                     // displayed
                     activePlayer_.setMediaTime(
                        new Time(playerTime*1000000L+(++bugAroundCounter_)));
                  }
               }
                             
            } 
            else // !running
            {
               // stop if needed
               if (activePlayer_.getState() == Controller.Started)
                  activePlayer_.stop();
               else
               {
                  
                  // do a key frame jump if needed
                  Time keyFrameTime = new Time(playerTime*1000000L); 
                  /* this is never true (during not-start) anyway
                  if (scrolling_)
                  {
                     keyFrameTime = findPreviousKeyFrameTime(matchingParser, playerTime);
                  }
                  */

                  long mediaNanos =  activePlayer_.getMediaNanoseconds();
                  int activePlayerMillis = (int)(mediaNanos/1000000L);

                  //System.out.print((activePlayerMillis-syncMillis_)+" ");
               
                  boolean millisDiffer = 
                     (keyFrameTime.getNanoseconds()/1000000L !=
                      mediaNanos/1000000L);

                  //System.out.print((keyFrameTime.getNanoseconds()/1000000L-mediaNanos/1000000L)+" ");

                  if (millisDiffer || mediaNanos == 0 || bugAroundReset)
                  {
                     Time t = new Time(keyFrameTime.getNanoseconds()+(++bugAroundCounter_));
                     activePlayer_.setMediaTime(t);
                  }
               }        
            }
         } // activePlayer_ != null

         synchronized(managementThread_)
         {
            managementThread_.notify();
         }
         try { Thread.sleep(scrolling_ ? 50 : 100); } catch (InterruptedException exc) {}
      
         /*
         boolean doGoOn = started_ || beginSyncMillis != syncMillis_;
         if (!doGoOn)
         {
            runOnceMore_ = !runOnceMore_;
         }
         else
            runOnceMore_ = false;
      
         if (doGoOn || runOnceMore_)
            ; // continue
         else
            break;
            //*/
      }
      while (!closed_); // end of do ... while()




      //managementThread_ = null;
   }


   private boolean startThread()
   {
      return startThread(false);
   }

   private boolean startThread(boolean force)
   {
      if (managementThread_ == null || !managementThread_.isAlive())
      {
         managementThread_ = new Thread(this, "JMFVideoPlayer");
         managementThread_.start();

         threadStartTime_ = System.currentTimeMillis();

         return true;
      }
      else if (force && !started_) // is alive, keep that for some time
      {
         threadStartTime_ = System.currentTimeMillis();

         managementThread_.interrupt(); // wake thread to update components

         //try { managementThread_.join(2000); } catch (InterruptedException exc) {}
         //return startThread(false);
      }

      return false;
   }
  
   private int findMatchingPlayerIndex(int millis)
   {
      //
      // search for matching player that is/should be active at this time
      //
      if (players_ != null && startAndStopTimes_ != null) // clips present
      {
         int idx = Arrays.binarySearch(startAndStopTimes_, millis);
         if (idx < 0)
            idx = -idx-2;

         boolean matchesPlayer = idx%2 == 0;

         if (matchesPlayer)
            return idx/2;
         else if (bigVideoIndex_ > -1)
            return bigVideoIndex_;
         else if (denverWorkaround_)
            return 0;
         else
            return -1;
      }
      else if (players_ != null && playerPresent_)
         return 0;
      else
         return -1;
   }

   private Player updateComponents(Player matchingPlayer, Player activePlayer)
   {  
      boolean isClip = false;
      if (matchingPlayer != null)
      {
         for (int i=0; i<players_.length; ++i)
         {
            if (players_[i] == matchingPlayer)
               isClip = isClipPlayer_[i];
         }
      }
     

      Component c = null;
      boolean isBigVideo = bigVideoIndex_ > -1 && matchingPlayer == players_[bigVideoIndex_];
      if (matchingPlayer != null)
      {
         if (!(fullScreenActive_ && isBigVideo))
         {
            c = matchingPlayer.getVisualComponent();
         }
      }
      else if (!fullScreenActive_ && stillImageComponent_ != null)
         c = stillImageComponent_;
      
      if (visualComponentClips_ != null) // auch doClipsOnSlides_
         ((VideoVisiblePanel)visualComponentClips_).activate(c);
     
      if (visualComponent_ != null) // nicht der Fall bei nur Clips (oder Standalone-Clip)
      {
         if (visualComponentClips_ != null && !fullScreenActive_ && stillImageComponent_ != null)
            c = stillImageComponent_; // the other component is already handled
      
         ((VideoVisiblePanel)visualComponent_).activate(c);
      }
     
      

      
      if (c != null)
         return matchingPlayer;
      else
         return null;
    
     
   }


   private boolean syncPlayer(Player player, int millis)
   {
      int diff = (int)(player.getMediaNanoseconds()/1000000)-millis;
      
      float rate = 0;
      int xvx = 4000-Math.abs(diff);
      if (xvx > 1000) { 
         // upto 3 seconds delay are corrected with rate adjustment
         rate = 4000.0f/xvx;
         if (diff > 0) rate = 1/rate;
         rate -= 1.0f-rate; // double the adjustment rate
        
         // all players have the same TimeBase (time source)
         timeBase_.setRate(rate);

         if (DEBUG)
            System.out.print(diff+" > "+rate+" # ");

         return true;
      } else { 
         // delay is uncorrectable -> jump
         player.setMediaTime(new Time(millis*1000000L));
         
         return false;
      }
   }

   private Time findPreviousKeyFrameTime(AviUnbugParser parser, int millis)
   {
      Time keyFrameTime = null;
      if (parser != null)
         keyFrameTime = parser.findPreviousKeyFrame(new Time(millis*1000000L));
      if (keyFrameTime == null)
         keyFrameTime = new Time(millis*1000000L); 

      return keyFrameTime;
   }


   private String getURL(Player p)
   {
      String url = null;
      for (int i=0; i<players_.length; ++i)
         if (p == players_[i])
            url = videoUrls_[i];

      if (url != null)
      {
         url = url.replace('\\', '/');
         if (url.indexOf('/') > -1)
            return url.substring(url.lastIndexOf('/')+1);
         else
            return url;
      }
      else
         return "";
   }


   public void setDuration(int millis) 
   {
      if (playerPresent_)
      {
         // if there is only one video and that is rather long
         // with respect to the duration it shall never be removed;
         // possible scenario: video is a bit too short
         if (players_ != null && players_.length == 1 &&  players_[0] != null)
         {
             if (startAndStopTimes_ != null) // -multi ? 
             {

                if (startAndStopTimes_[0] < 1000 &&
                    startAndStopTimes_[1] >= (millis*9)/10)
                   denverWorkaround_ = true;

                // not used anymore?
                if (startAndStopTimes_[0] == 0 &&
                    startAndStopTimes_[1] >= (millis*9)/10)
                {
                   coveringPlayerOnly_ = true;
                }
                else
                {
                   coveringPlayerOnly_ = false; // only one small clip
                }
             }
             else
                coveringPlayerOnly_ = true; // no clips, must be normal video
         }
      }
   }


	public int getMediaTime() { return 0; }
	public int getDuration() { return 0; }
	public void setInfo(String key, String value) {}

   public void close(EventInfo info)
   {
      close(info, true);
   }

	public void close(EventInfo info, boolean removeHook) {
      closed_ = true;
		if (!playerPresent_) return;

      if (activePlayer_ != null)
         activePlayer_.stop();

      if (players_ != null)
      {
         for (int i=0; i<players_.length; i++)
         {
            if (players_[i] != null)
            {
               players_[i].close();
               players_[i] = null;
               parsers_[i] = null;
               if (videoUrls_[i] != null)
                  AviUnbugParser.cleanUp(videoUrls_[i]);
            }
         }
      }
      
      coordinator_ = null;
      
      if (cleanupHook_ != null)
      {
         if (removeHook)
         {
            Runtime.getRuntime().removeShutdownHook(cleanupHook_);
         }
         
         cleanupHook_.start(); // make it finish and thus garbage-collectable
      }

   }

   public boolean isAccompaningVideo()
   {
      return coveringPlayerOnly_;
   }

   public boolean isDisplayingSomething()
   {
      return playerPresent_;
   }
}