package imc.epresenter.player;

import java.awt.*;
import java.awt.event.*;
import java.awt.print.*;
import java.awt.geom.Rectangle2D;
import java.io.*;
import java.net.*;
import java.text.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.Border;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.ResourceNotFoundException;
import imc.epresenter.filesdk.Metadata;
import imc.epresenter.filesdk.SearchEngine;
import imc.epresenter.filesdk.StopMarkReader;
import imc.epresenter.player.util.*;
import imc.epresenter.player.whiteboard.EventQueue;
import imc.epresenter.player.whiteboard.WhiteBoardPlayer;
import imc.epresenter.player.master.MasterPlayer;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.SecurityUtils;


public class Document implements Coordinator, Timeable, AWTEventListener, Pageable, Printable {
	private static int instanceCounter_;
   private static boolean lookAndFeelSet_;
	private static StartWindow startWindow_;

   // "Standard navigation"
   // This stuff is also defined in "LPLibs.java" (imc.lecturnity.converter)
   public static final int NUMBER_OF_NAVIGATION_STATES = 8;
   public static final String STR_NAVIGATION_NODE = "NAVIGATION";
   public static final String[] STR_NAVIGATION_SUBNODES = {
      "CONTROLBAR",
      "STANDARDBUTTONS",
      "TIMELINE",
      "TIMEDISPLAY",
      "DOCUMENTSTRUCTURE",
      "PLAYERSEARCHFIELD",
      "PLAYERCONFIGBUTTONS",
      "PLUGINCONTEXTMENU"
   };

   public static final int NAV_CONTROLBAR          = 0;
   public static final int NAV_STANDARDBUTTONS     = 1;
   public static final int NAV_TIMELINE            = 2;
   public static final int NAV_TIMEDISPLAY         = 3;
   public static final int NAV_DOCUMENTSTRUCTURE   = 4;
   public static final int NAV_PLAYERSEARCHFIELD   = 5;
   public static final int NAV_PLAYERCONFIGBUTTONS = 6;
   public static final int NAV_PLUGINCONTEXTMENU   = 7;

   public static final int NAV_STATE_ENABLED       = 0;
   public static final int NAV_STATE_DISABLED      = 1;
   public static final int NAV_STATE_HIDDEN        = 2;

   public static final String[] STR_NAVIGATION_STATES = {
      "enabled",  /* 0 */
      "disabled", /* 1 */
      "hidden"    /* 2 */
   };

   public static int getInstanceCount()
   {
      return instanceCounter_;
   }

   public static void incrementCounter()
   {
      instanceCounter_++;
      //System.out.println(instanceCounter_);
   }

   public static void decrementCounter(String why)
   {
      instanceCounter_--;
      //System.out.println(instanceCounter_+" "+why);
   }


   public static int determineLength(String location, Window parent)
   {
      // double code (below)
      if (Manager.isWindows())
         location = location.replace('/', '\\');
      
      FileResources resources = null;
      InputStream configStream = null;
      try 
      {
         resources = FileResources.createFileResources(location);
         configStream = resources.createConfigFileInputStream();
      }
      catch (ResourceNotFoundException re)
      {
         Manager.showError(parent, "", Manager.ERROR, re);
         Manager.putLoadWindow(null);
         return -1;
      }
      catch (IOException ie)
      {
         Manager.showError(parent, "", Manager.ERROR, ie);
         Manager.putLoadWindow(null);
         return -1;
      }
      XmlTag configTag = (XmlTag.parse(configStream))[0];

      try
      {
         configStream.close();
      }
      catch (IOException exc)
      {
      }
      
      String audioLocation = (String)( (configTag.getValues("AUDIO"))[0] );
      
      MasterPlayer masterPlayer = new MasterPlayer();
      masterPlayer.init(resources, new String[] { audioLocation }, null);
     
      return masterPlayer.getDuration();
         
   }
   
   //
   // extract metadata and slide full text
   //
   private static final int OK = 0;
   private static final int E_ARGS = -23;
   private static final int E_IO = -25;
   private static final int E_RESOURCE = -27;
   private static final int E_NOOPT = -29;
   private static final int E_WHITEBOARD = -31;
   
   public static void analyze(String strLocation)
   {
      int iReturn = OK;
      
      /*
      String strLocation = null;
      if (args.length > 0)
         strLocation = args[0];
      else
      {
         iReturn = E_ARGS;
         System.err.println("Error: Missing parameter: <lpd|lrd>");
      }
      */
      
      if (null == strLocation)
      {
         iReturn = E_ARGS;
         System.err.println("Error: Missing parameter: <lpd|lrd>");
      }
      
      // double code (above)
      FileResources resources = null;
      InputStream configStream = null;
      
      if (iReturn >= OK && Manager.isWindows())
         strLocation = strLocation.replace('/', '\\');
      
      if (iReturn >= OK)
      {
         try 
         {
            resources = FileResources.createFileResources(strLocation);
            configStream = resources.createConfigFileInputStream();
         }
         catch (ResourceNotFoundException re)
         {
            iReturn = E_RESOURCE;
            System.out.println("Error: Configuration not found.");
         }
         catch (IOException ie)
         {
            iReturn = E_IO;
            System.out.println("Error: IO, maybe file not found: "+strLocation);
         }
      }
      XmlTag configTag = null;
      if (iReturn >= OK)
         configTag = (XmlTag.parse(configStream))[0];

      if (null != configStream)
      {
         try
         {
            configStream.close();
         }
         catch (IOException exc)
         {
         }
      }
      
      String strCodepage = "Cp1252";
      Object[] arrCodepages = configTag.getValues("CODEPAGE");
      if (arrCodepages != null && arrCodepages.length > 0)
      {
         strCodepage = (String)arrCodepages[0];
         if (FileUtils.isInteger(strCodepage))
            strCodepage = "Cp"+strCodepage;
      }
      
      if (configTag.IsUtf8()) 
         strCodepage = "utf-8";
      // the format of the LRD file (if utf-8) overrides the codepage setting in the LRD

      // jetzt brauchen wir noch Metadaten
      Metadata metadata = null;
      if (iReturn >= OK)
      {
         Object[] metaA = configTag.getValues("METADATA");
         if (metaA != null && metaA.length > 0)
         {
            String metaFile = (String)metaA[0];
           
            try
            {
               metadata = resources.getMetadata(metaFile);
            }
            catch (IOException ioe)
            {
            }
         }
      }
      if (null == metadata)
         System.err.println("Warning: No metadata found.");
            
   
      // und ein non-image-loading Whiteboard ("-Dpixel.limit = 0")
      String strConfigValue = System.getProperty("pixel.limit", "");
      System.setProperty("pixel.limit", "0"); // disable image loading
      
      String strEvqFile = null;
      String strObjFile = null;
      WhiteBoardPlayer whiteboard = null;
      if (iReturn >= OK)
      {
         Object[] arrOpt = configTag.getValues("OPT");
         if (null != arrOpt && 0 < arrOpt.length)
         {
            for (int i=0; i<arrOpt.length; ++i)
            {
               String strOption = (String)arrOpt[i];
               if (strOption.endsWith(".evq"))
                  strEvqFile = strOption;
               else if (strOption.endsWith(".obj"))
                  strObjFile = strOption;
            }
         }
         else
         {
            iReturn = E_NOOPT;
            System.out.println("Error: No options found. Maybe configuration file corrupt.");
         }
      }
      if (null == strEvqFile || null == strObjFile)
         System.err.println("Warning: No information about whiteboard found.");
      else
      {
         whiteboard = new WhiteBoardPlayer();
         try
         {
            whiteboard.init(resources, new String[] { strEvqFile, strObjFile, "-cp", strCodepage }, null);
         }
         catch (RuntimeException exc)
         {
            iReturn = E_WHITEBOARD;
            System.err.println("Error: Cannot initialize whiteboard.");
         }
      }
      
      SearchEngine engine = null;
      if (iReturn >= 0)
         engine = new SearchEngine(metadata, whiteboard.getObjectQueue(), 
                                   whiteboard.getEventQueue(), true);
      
      imc.epresenter.filesdk.SearchEngine.Slide[] slides = null;
      if (null != engine)
      {
         slides = engine.getSlideInformation();
      
         System.out.println("Slide count: "+slides.length);
      }
      
      if (null != slides && slides.length > 0)
      {
         for (int i=0; i<slides.length; ++i)
         {
            System.out.println("Slide "+i+": ");
            for (int j=0; j<slides[i].fullContent.size(); ++j)
            {
               imc.epresenter.player.whiteboard.Text text = 
                  (imc.epresenter.player.whiteboard.Text)slides[i].fullContent.get(j);
               System.out.println(" "+text.getAllContent());
            }
         }
      }
      
      
      if (!strConfigValue.equals(""))
         System.setProperty("pixel.limit", strConfigValue);
   }


   private File configFile_;
   private String protectLocation_;
   private MasterPlayer masterPlayer_;
   private WhiteBoardPlayer whiteBoardPlayer_;
   private JMFVideoPlayer videoPlayer_;
   private StopMarkPlayer stopPlayer_;
   private boolean videoDeactivated_;
	private SRStreamPlayer[] players_; // all players_ (incl. master at idx 0)
	private int[] offsets_;
   private DocumentController daemon_; // see above
	private boolean started_;
	private boolean m_bSpecialPause = false;
	private boolean paused_;
   private int lastDispatchedTime_ = -1;
   private int audioDuration_;
   private HashMap metaInfo_;
	private TimeSource timer_;
	private boolean isClosed_;
   private JComponent visualComponent_;
   private LayoutNov2006 layout_;
   private boolean switchingToFullScreen_ = false;
   private boolean autoFullScreen_ = false;
   private boolean doClipsOnSlides_ = false;
   private boolean isStructuredClip = false;
   private boolean autoLoop_ = false;
	private int[] anNavigationStates_;
   private boolean m_bIgnoreRegularTimeOnce = false;
   
	
   public Document(final DocumentController d) // used for dummy purpose
   {
      final JButton b = new JButton(createIcon("/imc/epresenter/player/player-dummy.png"));
      b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e)
         {
            if (e.getSource() == b)
            {
               //Daemon.stopSplashJostling();
               String location = Manager.showFileDialog(
                  SwingUtilities.windowForComponent(visualComponent_));
               if (location != null)
               {
                  d.loadDocument(location, 0, false, this);
                  b.setEnabled(false);
               }
            }
            else
               b.setEnabled(true);
         }  
      });
      b.setBorder(null);
      b.setFocusPainted(false);
      visualComponent_ = b;
      visualComponent_.setOpaque(true);

      incrementCounter(); // to prevent closing on errors during loading
   }

   public Document(String configLocation, int time, DocumentController d) 
   throws IllegalAccessException, InstantiationException
   {
      this(configLocation, null, time, d);
   }

   public Document(String configLocation, String originalLocation, int time, DocumentController d) 
   throws IllegalAccessException, InstantiationException
   {
      Window parent = Manager.getLoadWindow();
      if (parent != null)
         Manager.putLoadWindow(null);
      
      
      if (Manager.isWindows())
         configLocation = configLocation.replace('/', '\\');
     
      
      protectLocation_ = configLocation;
      // originalLocation is not null if the document is loaded from cache
      // or is (was) an lpl
      if (originalLocation != null)
      {
         protectLocation_ = originalLocation;
      }
      
      //if (protectLocation_.endsWith("tutorial.lpd"))
      //   protectLocation_ = protectLocation_.substring(0, protectLocation_.length()-12)+"Tutorial.lpd";
      
      
      boolean isProtected = SecurityUtils.isDocumentCleared(protectLocation_);

      // double code (Daemon)

      int versionType = NativeUtils.UNIVERSITY_VERSION; // default
      
      if (Manager.isWindows())
      {
         if (!isProtected)
         {
            if (SecurityUtils.getInstallationType(Manager.versionNumber) == NativeUtils.VERSION_ERROR)
            {
               // tutorial on eval cd and lpds on cd export cd are "unprotected"
               // the remainder (online help, tutorial after installation) are
               // replayed with an installed player
               // so the following shouldn't occur to often
               Manager.showError(parent, Manager.getLocalized("versionConflict"), Manager.ERROR, null);
               throw new IllegalAccessException(Manager.getLocalized("versionConflict"));
            }
         

            versionType = SecurityUtils.getInstallationType(Manager.versionNumber);
            // cannot be VERSION_ERROR; see above

            boolean isEvaluationVersion = 
               versionType == NativeUtils.EVALUATION_VERSION || 
               versionType == NativeUtils.UNIVERSITY_EVALUATION_VERSION;


            int remainingDays = 1;
            Date endDate = null;
            if (isEvaluationVersion)
            {
               remainingDays = SecurityUtils.getRemainingEvaluationDays();

               if (remainingDays < 0)
               {
                  Manager.showError(parent, Manager.getLocalized("evalExpired"), Manager.ERROR, null);
                  throw new IllegalAccessException(Manager.getLocalized("evalExpired"));
               }
            }
            else
            {
               endDate = SecurityUtils.getEndDate(false);

               if (endDate != null && endDate.before(new Date()))
               {
                  Manager.showError(parent, Manager.getLocalized("fullExpired"), Manager.ERROR, null);
                  throw new IllegalAccessException(Manager.getLocalized("fullExpired"));
               }
            }

        }
      }
      
      StartWindow greet = null;

      try
      {
         configFile_ = new File(configLocation);
         if (!configFile_.exists())
         {
            Manager.showError(parent, 
                              Manager.getLocalized("fileNotFound1")
                              +" "+configLocation+" "
                              +Manager.getLocalized("fileNotFound2"),
                              Manager.ERROR, null);
            throw new InstantiationException("");
         }

         incrementCounter(); 
         
         greet = buildGreeting(
            Manager.getLocalized("loading")+" "+configFile_.getName()+" ...");
         greet.showNow();
         

         daemon_ = d;

         timer_ = new TimeSource(this, 20);

         //
         // start parsing
         //
         FileResources resources = null;
         try 
         {
            resources = FileResources.createFileResources(configLocation);
         }
         catch (ResourceNotFoundException re)
         {
            decrementCounter("error on"+configLocation);
            Manager.showError(parent, "", Manager.ERROR, re);
            throw new InstantiationException("");
         }
         
         String markString = resources.getMarkString();

         // As of version 1.6.0, the Lecturnity Player only supports
         // documents which have been marked (i.e. which have LAD audio)
         if (markString == null)
         {
            Manager.showError(parent,
                              Manager.getLocalized("audioNotMarked"),
                              Manager.ERROR, null);
            decrementCounter("not marked collision on "+configLocation);
            throw new IllegalAccessException(Manager.getLocalized("audioNotMarked"));
      
         }
         
          
         if (!isProtected)
         {
            if (SecurityUtils.isEvaluationConflict(resources.getDocumentType()))
            {
               Manager.showError(parent, Manager.getLocalized("typeCollision")
                                 +"\n\n"+Manager.getLocalized("typeCollisionEval"),
                                 Manager.ERROR, null);
               decrementCounter("type collision on "+configLocation);
               throw new IllegalAccessException(Manager.getLocalized("typeCollision"));
      
            }

            /* Bugfix #3831: Ommit campus check
            if (SecurityUtils.isCampusConflict(resources.getDocumentType()))
            {
               Manager.showError(parent, Manager.getLocalized("typeCollision")
                                 +"\n\n"+Manager.getLocalized("typeCollisionCampus"),
                                 Manager.ERROR, null);
               decrementCounter("type collision on "+configLocation);
               throw new IllegalAccessException(Manager.getLocalized("typeCollision"));
      
            }
            */
         }

         InputStream configStream = resources.createConfigFileInputStream();
         XmlTag configTag = (XmlTag.parse(configStream))[0];
         configStream.close();
         
         
         // check if the config file is new enough
         Object events = configTag.getValues("EVQ");
         Object objects = configTag.getValues("OBJ");
         if (events != null || objects != null)
            throw new UnsupportedOperationException(
               "Documents in old style (with EVQ and OBJ tag in the config file) are not supported.");


         File rootDir = configFile_.getParentFile();
         // sometimes rootDir is null !?! :
         if (rootDir == null) rootDir = new File(".");


         doClipsOnSlides_ = false;
         if (FileResources.isClipsOnSlidesDocument(configLocation))
            doClipsOnSlides_ = true;
         //if (new File("c:\\doClipsOnSlides.txt").exists())
         //   doClipsOnSlides_ = true;

         ArrayList collector = new ArrayList(5);
         ArrayList pArgs = new ArrayList(5);


         String audioLocation = (String)( (configTag.getValues("AUDIO"))[0] );

         // double code (ArchivedFileResources)
         // 
         // since 1.7.0.p3 there might be a codepage specification
         // in the lrd file this is relevant for all text files (obj, lmd)
         String strCodepage = null;
         Object[] arrCodepages = configTag.getValues("CODEPAGE");
         if (arrCodepages != null && arrCodepages.length > 0)
         {
            strCodepage = (String)arrCodepages[0];
            if (FileUtils.isInteger(strCodepage))
               strCodepage = "Cp"+strCodepage;
         }
         
         //if (configTag.IsUtf8()) 
         //   strCodepage = "utf-8";
         // the format of the LRD file (if utf-8) overrides the codepage setting in the LRD
         // No: every text file (eg obj, lmd) automatically checks
         // for being "utf-8" encoded. If not the specified codepage should be used.
                     
         

         //
         // 'Standard navigation' states
         //
         // LRD example:
         //  ...
         //  <NAVIGATION>
         //   <CONTROLBAR>enabled</CONTROLBAR>
         //   <STANDARDBUTTONS>disabled</STANDARDBUTTONS>
         //   <TIMELINE>hidden</TIMELINE>
         //   ...
         //  </NAVIGATION>

         anNavigationStates_ = new int[NUMBER_OF_NAVIGATION_STATES];
         
         Object[] navigationNode = configTag.getValues(STR_NAVIGATION_NODE); // <NAVIGATION>
         if ((navigationNode != null && navigationNode.length > 0))
         {
            // <NAVIGATION> tag found
            for (int i = 0; i < NUMBER_OF_NAVIGATION_STATES; ++i)
            {
               String strNavigationState = "";
               Object[] navigationSubNode = configTag.getValues(STR_NAVIGATION_SUBNODES[i]);
               if ((navigationSubNode != null && navigationSubNode.length > 0))
                  strNavigationState = (String)navigationSubNode[0];

               anNavigationStates_[i] = NAV_STATE_ENABLED; // default
               // Determine the index of the 'navigation' state
               // (0: enabled, 1: disabled, 2: hidden)
               for (int k = 0; k < STR_NAVIGATION_STATES.length; ++k)
               {
                  if (strNavigationState.indexOf(STR_NAVIGATION_STATES[k]) > -1)
                  {
                     anNavigationStates_[i] = k;
                     break;
                  }
               }
            }
         }
         else
         {
            // <NAVIGATION> tag not found
            // default: all states are enabled
            for (int i = 0; i < NUMBER_OF_NAVIGATION_STATES; ++i)
               anNavigationStates_[i] = NAV_STATE_ENABLED; 
         }
///         System.out.print("Standard Navigation states:");
///         for (int i = 0; i < NUMBER_OF_NAVIGATION_STATES; ++i)
///            System.out.print(" " + anNavigationStates_[i]);
///         System.out.println("");

         // 'Standard Navigation' must be considered in GlobalMenuBar (viaGlobalMasterAndUI)
         // --> STANDARDBUTTONS
         if (d instanceof GlobalMasterAndUI)
            ((GlobalMasterAndUI)d).setStandardNavigationState(anNavigationStates_[1]);

         //
         // setup master
         //
         masterPlayer_ = new MasterPlayer();
         //masterPlayer_.init(rootDir, , this);
         masterPlayer_.setStandardNavigationStates(anNavigationStates_);
         pArgs.add(new String[] { audioLocation, time+"" });
         collector.add(masterPlayer_);

         SRStreamPlayer applicationPlayer = null;
         SRStreamPlayer thumbnailPlayer = null;
         SRStreamPlayer searchPlayer = null;
         SRStreamPlayer videoPlayerClips = null;

         //System.out.println("D: master set; now helpers ...");

         // BUGFIX 5679 Publish "Simulation" instead of "structured screen recording"
         String viewtype = "";
         Object[] viewtypeScan = configTag.getValues("VIEWTYPE");
         if (viewtypeScan != null && viewtypeScan.length > 0) 
            viewtype = (String)viewtypeScan[0];
         boolean bIsSimulationMode = viewtype.equals("simulation");
         
         // locate and load clip structures (extended/structured screengrabbing)
         Object[] clips = configTag.getValues("CLIPSTRUCTURE");
         ArrayList<Boolean> clipStructuringStatus = new ArrayList<Boolean>();
            for (int j = 0; clips != null && j < clips.length; j++)
            {
               if (!(clips[j] instanceof XmlTag)) break;
               //System.out.println("	<CLIP>");
               XmlTag clip = (XmlTag)(clips[j]);
               Object[] options = clip.getValues("STRUCTURE");
               String available = (String)(options[0]);
               //System.out.println("	<STRUCTURE>" + available);
               if (available.equals("available")) {
                  if (!bIsSimulationMode) {
                      // do not change state in simulation mode
                      doClipsOnSlides_ = true;
                      isStructuredClip = true;
                  }
                  clipStructuringStatus.add(new Boolean(true));
               } else
                  clipStructuringStatus.add(new Boolean(false));

               options = clip.getValues("IMAGES");
               available = (String)(options[0]);
               //System.out.println("	<IMAGES>" + available);
               options = clip.getValues("FULLTEXT");
               available = (String)(options[0]);
               //System.out.println("	<FULLTEXT>" + available);
            }

         // locate and load helpers
         Object[] helpers = configTag.getValues("HELPERS");
         for (int i=0; helpers != null && i<helpers.length; i++) {
            if (!(helpers[i] instanceof XmlTag)) break;
            XmlTag helper = (XmlTag)(helpers[i]);
            Object[] options = helper.getValues("OPT");
            String app = (String)(options[0]);
            if (app.equals("java")) {
               FileClassLoader loader = null;
               int idx = 0;
               String fArg = (String)options[1];
               if (fArg.equals("-classpath") || fArg.equals("-cp")) {
                  loader = new FileClassLoader((String)options[2]);
                  idx = 3;
               } else {
                  loader = new FileClassLoader(rootDir.toString());
                  idx = 1;
               }
               String pluginName = (String)options[idx];
               try {
                  SRStreamPlayer sp = null;
                  SRStreamPlayer spClips = null;
                  /*
                  if (pluginName.endsWith("JMFVideoPlayer"))
                     sp = new JMFVideoPlayer();
                  else
                  */
                  if (!pluginName.endsWith("JMFVideoPlayer"))
                     sp = loader.findPlugin(pluginName);
                  else
                  {
                     if (doClipsOnSlides_)
                     {
                        sp = new JMFVideoPlayer(JMFVideoPlayer.HANDLE_VIDEO_OR_STILL);
                        
                        // video AND clip (video) can be active at the same time
                        // this is handled by two differently configured
                        // JMFVideoPlayer, one handling the video and one handling the clips
                        spClips = new JMFVideoPlayer(JMFVideoPlayer.HANDLE_CLIPS_ONLY);
                     }
                     else
                     {
                        sp = new JMFVideoPlayer(JMFVideoPlayer.HANDLE_DEFAULT);
                     }
                  }
                  if (sp instanceof JMFVideoPlayer)
                     videoPlayer_ = (JMFVideoPlayer)sp;
                  if (spClips != null)
                     videoPlayerClips = (JMFVideoPlayer)spClips;
                  String[] videoArgs = null;
                  if (options.length > idx+1) 
                     videoArgs = createArgs(options, idx+1);
                  else
                     videoArgs = new String[0];
                  
                  // BUGFIX 5679 Publish "Simulation" instead of "structured screen recording"
                  if (bIsSimulationMode) {
                      // remove structured clips, which are not shown in simulation mode
                      ArrayList<String> args = new ArrayList<String>();
                      int clipNr = 0;
                      boolean bContainsClips = false;
                      for (int j=0; j<videoArgs.length; j++) {                          
                          if ( videoArgs[j].equalsIgnoreCase("-multi") ) {
                              if (clipNr<clipStructuringStatus.size() && clipStructuringStatus.get(clipNr++) ) {
                                  // skip structured clip
                                  j += 2;
                                  continue;
                              }
                              // keep clip; clips player is required
                              bContainsClips = true;
                          }
                          // keep argument
                          args.add(videoArgs[j]);
                      }
                      // reset videoArgs
                      videoArgs = args.toArray(new String[0]);
                      if (!bContainsClips)
                          // remove clip player
                          spClips = null;
                      if (videoArgs.length == 0)
                          // remove video player
                          sp = null;
                  }
                  
                  if (sp != null) {
                      pArgs.add(videoArgs);
                      collector.add(sp);
                  }
                  if (spClips != null)
                  {
                     // both video player have the same arguments;
                     // each picks the right subset of arguments 
                     // according to its configuration
                     pArgs.add(videoArgs);
                     collector.add(spClips);
                  }
               } catch (RuntimeException e) {
                  Manager.showError(parent, pluginName+" "
                                    +Manager.getLocalized("pluginFail"), Manager.WARNING, e);
               } catch (NoClassDefFoundError e) {
                  // shouldn't occur here (but in JMFVideoPlayer itself) but it
                  // does occur here...
                  if (e.getMessage().equals("javax/media/Clock")) 
                     Manager.showError(Manager.getLocalized("jmfFail"), Manager.WARNING, e);
                  else
                     throw e;
               }
               
            } else if (app.equals("wbPlay")) {
                // hack: set metadata before reading event queue (used to adjust pages according to LMD)
                // TODO: avoid double reading of metadata (see line 800++)
                Object[] metaA = configTag.getValues("METADATA");
                if (metaA != null && metaA.length > 0)
                    try {
                        EventQueue.metadata = resources.getMetadata((String)metaA[0]);
                    } catch (Exception e) {};
                      
               WhiteBoardPlayer wbp = new WhiteBoardPlayer();
               
               whiteBoardPlayer_ = wbp;
               if (options.length > 1 && isStructuredClip) pArgs.add(
                  createArgs(options, 1, new String[] { "-cp", strCodepage, "-structuredClips" }));
               else if (options.length > 1) pArgs.add(
                  createArgs(options, 1, new String[] { "-cp", strCodepage }));
               else pArgs.add(new String[0]); //wbp.init(rootDir, , this);
               collector.add(wbp);
               wbp.init(resources, (String[])pArgs.get(pArgs.size()-1), this); // gr hack for SearchHelper
               
               // only required for initialisation
               EventQueue.metadata = null;;
            } else if (app.equals("appPlay")) {
               ApplicationPlayer appp = new ApplicationPlayer();
               applicationPlayer = appp;
               if (options.length > 1) pArgs.add(createArgs(options, 1));
               else pArgs.add(new String[0]); // appp.init(rootDir, , this);
               collector.add(appp);
            } else if (app.equals("aofStat")) {
               // do nothing; currently not working/implemented
            } else {
               Manager.showError(parent, Manager.getLocalized("unknownHelper")
                                 +": "+app, Manager.WARNING, null);
               /*
               try {
					Runtime.getRuntime().exec(app);
					System.out.println("Starting unknown helper "+app+" externaly...");
               } catch (IOException e) {
					System.err.println("Cannot start unknown helper "+app+" externaly...");
               }
               */
               // Und die Synchronisierung !?
               // -> players_.add(new SRSPWrapper(exec));
            }
         }


         // thumbnail helper
         // always if thumbs are provided?
         boolean metadataAvailable = false;
         Metadata metadata = null;
         Object[] metaA = configTag.getValues("METADATA");
         if (metaA != null && metaA.length > 0)
         {
            String metaFile = (String)metaA[0];
           
            try
            {
               metadata = resources.getMetadata(metaFile);
               metadataAvailable = true;
            }
            catch (IOException ioe)
            {
               metadataAvailable = false;
            }

            if (whiteBoardPlayer_ != null)
            {
               thumbnailPlayer = new ThumbnailHelper();
               pArgs.add(new String[] { metaFile });
               collector.add(thumbnailPlayer);

               searchPlayer = new SearchHelper(metadata, whiteBoardPlayer_.getObjectQueue(),
                     whiteBoardPlayer_.getEventQueue());
               pArgs.add(new String[0]);
               collector.add(searchPlayer);
               
               // cannot be added like a normal helper (above)
               // as offsets (in lrd) are index based
         
            }
         }

         if (whiteBoardPlayer_ != null && StopMarkReader.stopsPresent())
         {
            int[] aiStopTimes = StopMarkReader.getStopTimes(true);
            stopPlayer_ = new StopMarkPlayer(aiStopTimes);
            collector.add(stopPlayer_);
            pArgs.add(new String[0]);
            // cannot be added like a normal helper (above)
            // as offsets (in lrd) are index based
         }


         players_ = new SRStreamPlayer[collector.size()];
         collector.toArray(players_);
         //frames_ = new JFrame[players_.length];
         offsets_ = new int[players_.length];

         // doesn't say anything...
         //if (players_.length != pArgs.size()) System.out.println(players_.length+" != "+pArgs.size());

         greet.setMaxProgress(players_.length+1); // +1 for player loading
         greet.increment();


         //System.out.println("D: helpers done; now infos ...");

         //  setup of all players_
         for (int i=0; i<players_.length; i++) {
            SRStreamPlayer player = players_[i];
            String[] eArgs = (String[])pArgs.get(i);
            //System.out.print(player+" ");
            if (player != whiteBoardPlayer_)  // gr hack for SearchHelper
               player.init(resources, eArgs, this);

            greet.increment();
         }
         audioDuration_ = players_[0].getDuration();
         for (int i=1; i<players_.length; i++) {
            players_[i].setDuration(audioDuration_);
         }

         // do the offset thingy
         Object[] offs = configTag.getValues("OFFSETS");
         if (offs != null) {
            for (int i=0; i<offs.length && i < offsets_.length-1; i++) {
               Object[] off = ((XmlTag)offs[i]).getValues("O");
               if (off != null && off.length > 0)
					offsets_[i+1] = Integer.parseInt((String)off[0]);
            }
         }

         EventInfo timeInfo = new EventInfo(EventInfo.EXTERNAL,
                                            System.currentTimeMillis(),
                                            0);
         if (time != 0) sendTimeToAllHelpers(time, timeInfo);
         // this occurs below again to initialize the display of a JMFVideoPlayer

         String author = "";
         Object[] authorScan = configTag.getValues("AUTHOR");
         if (authorScan != null && authorScan.length > 0) 
            author = (String)authorScan[0];
         String title = "";
         Object[] titleScan = configTag.getValues("TITLE");
         if (titleScan != null && titleScan.length > 0) 
            title = (String)titleScan[0];
         String recorded = "";
         Object[] recordedScan = configTag.getValues("RECORDDATE");
         if (recordedScan != null && recordedScan.length > 0) 
            recorded = (String)recordedScan[0];
         String creator = "";
         Object[] creatorScan = configTag.getValues("CREATOR");
         if (creatorScan != null && creatorScan.length > 0) 
            creator = (String)creatorScan[0];
         String keywords = "";
         Object[] keywordScan = configTag.getValues("KEYWORDS");
         if (keywordScan != null && keywordScan.length > 0) 
            keywords = (String)keywordScan[0];
         
         
         if (metadataAvailable)
         {
            title = metadata.title;
            author = metadata.author;
            creator = metadata.creator;
            recorded = metadata.recordDate;
            for (int i=0; i<metadata.keywords.length; i++)
               keywords += (i > 0 ? " "+metadata.keywords[i] : metadata.keywords[i]);
         }


         metaInfo_ = new HashMap();

         String length = new TimeFormat().format(audioDuration_);

         String[] keys = new String[] { "author", "title", "length", "recorded", "creator", "keywords" };
         String[] values = new String[] { author, title, length, recorded, creator, keywords };

         for (int i=0; i<values.length; i++)
         {
            if (values[i] != null && values[i].length() > 0)
            {
               String keyString = keys[i];
               String localizedKey = Manager.getLocalized(keys[i]);
               if (localizedKey != null && localizedKey.length() > 0) keyString = localizedKey;

               metaInfo_.put(keyString, values[i]);
               //System.out.println(keyString+","+values[i]);
            }
         }  

         //
         // Layout
         //
         layout_ = new LayoutNov2006(
            this, anNavigationStates_, 
            masterPlayer_, whiteBoardPlayer_, videoPlayer_, videoPlayerClips,
            applicationPlayer, thumbnailPlayer, searchPlayer);
         visualComponent_ = layout_.doLayout();


         //System.out.println("D: Setting greet visible false");
         greet.hideNow();

      }
      catch (Throwable e)
      {
         
         if (greet != null)
            greet.hideNow();
            
         if (!(e instanceof IllegalAccessException) && !(e instanceof InstantiationException))
         {
            // "unexpected" (uncaught) failures
            decrementCounter("error");
            Manager.showError(parent, Manager.getLocalized("unexpectedError"), Manager.ERROR, e);
            throw new InstantiationException("");
         }
         else
         {
            // "normal" (caught) failures
            if (e instanceof IllegalAccessException)
               throw (IllegalAccessException)e;
            else
               throw (InstantiationException)e;
         }
      }
      
      
      //Toolkit.getDefaultToolkit().addAWTEventListener(this, AWTEvent.KEY_EVENT_MASK);

	}

   public void eventDispatched(AWTEvent evt)
   {
      if (evt.getID() == KeyEvent.KEY_RELEASED)
      {
         KeyEvent kevt = (KeyEvent)evt;


         if (kevt.getKeyChar() == 'p')
         {
         }
         

      }
   }

   public String getLocation()
   {
      if (configFile_ != null)
         return configFile_.toString();
      else
         return null;
   }

   public JComponent getVisualComponent()
   {
      return visualComponent_;
   }

   public HashMap getMetaInfo()
   {
      return metaInfo_;
   }


   public void setStartOffset(int millis)
   {
      masterPlayer_.setStartOffset(millis);
   }


	public void tickTime()
   {
      if (m_bSpecialPause)
         return;
      
      EventInfo timeInfo = new EventInfo(EventInfo.MASTER,
										   System.currentTimeMillis(),
										   masterPlayer_.hashCode());
		int time = masterPlayer_.getMediaTime();
      
      if (time > audioDuration_) 
         time = audioDuration_;
      
      //DebugOut("tickTime()", time, timeInfo);
      
      if (time != lastDispatchedTime_)
      {
         try
         {
            sendTimeToAllHelpers(time, timeInfo);
         }
         catch (Exception exc)
         {
            exc.printStackTrace();
         }
         lastDispatchedTime_ = time;
      }
      
      
      // Does this work in any case together with a StopMarkPlayer?
      // -> Display of wrong times should be impossible.
      //
      // This "tickTime()" is not synchronized with UI operations (clicking
      // thumbnails or using the slider). However that is not necessary as only
      // normal replay will lead to a stop by the StopMarkPlayer.
      // And time stamp dispatching during normal replay is handled only
      // by "tickTime()". And during that dispatching ("sendTimeToAllHelpers()")
      // the StopMarkPlayer is always asked first.
   }
    
	public synchronized void requestStart(SRStreamPlayer origin) {
		
      if (started_ && !paused_) return;
		started_ = true;
		paused_ = false;
      m_bSpecialPause = false;
      
      if (whiteBoardPlayer_ != null)
         whiteBoardPlayer_.RemoveAllFeedback();
      
		EventInfo startInfo = new EventInfo(EventInfo.HELPER,
                                          System.currentTimeMillis(),
                                          origin.hashCode());
     
      //DebugOut("requestStart()", masterPlayer_.getMediaTime(), startInfo);
      
      // let it (re-)start at the end of a document
      int time = masterPlayer_.getMediaTime();
      if (time > audioDuration_-50)
         time = 0;
      
      // hack for video player with negative offset (and thus greater time)
      sendTimeToAllHelpers(time, startInfo);

      for (int i=0; i<players_.length; i++) {
         if (!(videoDeactivated_ && players_[i] == videoPlayer_))
         {
            players_[i].start(startInfo);
         }
		}
      
      timer_.start();
	}

	public synchronized void requestScrollStart(SRStreamPlayer origin) {
		if (paused_) return;
		if (started_) requestPause(origin);
	}
    
	public synchronized void requestScrollStop(SRStreamPlayer origin) {
		if (!paused_) return;
		if (started_) {
			requestStart(origin);
			//System.out.println("D: did request start");
		}
	}
    
	public synchronized void requestPause(SRStreamPlayer origin) {
		if (paused_) return;
		if (!started_) throw new IllegalStateException("Pause during stopped.");
		paused_ = true;

		EventInfo pauseInfo = new EventInfo(EventInfo.HELPER,
											System.currentTimeMillis(),
											origin.hashCode());
		for (int i=0; i<players_.length; i++) {
         if (!(videoDeactivated_ && players_[i] == videoPlayer_))
         {
         
            players_[i].pause(pauseInfo);
         }
		}

		timer_.stop();
   }
	
	public synchronized void requestStop(SRStreamPlayer origin)
   {
      requestStop(false, origin);
   }
   
   public synchronized void requestStop(boolean bSpecial, SRStreamPlayer origin)
   {
      if (!started_) return;
		if (paused_) throw new IllegalStateException("Stop during scroll.");
      started_ = false;
      m_bSpecialPause = bSpecial;
      
      EventInfo stopInfo = new EventInfo(EventInfo.HELPER,
										   System.currentTimeMillis(),
										   origin.hashCode());
		
      //DebugOut("requestStop()", masterPlayer_.getMediaTime(), stopInfo);
      
		for (int i=0; i<players_.length; i++) {
         if (!(videoDeactivated_ && players_[i] == videoPlayer_))
         {
            players_[i].stop(stopInfo);
         }
		}
      
		timer_.stop();
   }
    
   public synchronized void requestTime(int time, SRStreamPlayer origin)
   {
      //System.out.println("Requesting time: "+time+", "+origin);
     
      EventInfo timeInfo = new EventInfo(EventInfo.HELPER,
                                         System.currentTimeMillis(),
                                         origin.hashCode());
        
      //DebugOut("requestTime()", time, timeInfo);
      
		if (whiteBoardPlayer_ != null)
            whiteBoardPlayer_.RemoveAllFeedback();
   
      boolean bWasSpecialPause = m_bSpecialPause;
      
            
      if (started_ && stopPlayer_ != null)
      {
         if (stopPlayer_.isExactStopAt(time))
         {
            boolean bSpecial = true;
            requestStop(bSpecial, origin);
            bWasSpecialPause = false; // reset for this special case: no start
         }
      }
      
      // with a StopMarkPlayer this one must be informed of new timestamps
      // first in any case, thus special handling during replay deactivated:
		//if (started_ && !paused_) masterPlayer_.setMediaTime(time, timeInfo);
		//else {
			sendTimeToAllHelpers(time, timeInfo);
         lastDispatchedTime_ = time;
		//}
      
      
      m_bIgnoreRegularTimeOnce = true;
  
      if (bWasSpecialPause && origin != stopPlayer_)
         requestStart(origin);
   
   }
   
   public synchronized void requestNextSlide(SRStreamPlayer origin)
   {
      if (whiteBoardPlayer_ != null)
         whiteBoardPlayer_.nextPage();
	}
   
   public synchronized void requestPreviousSlide(boolean bRealPrevious, SRStreamPlayer origin)
   {
      if (whiteBoardPlayer_ != null)
         whiteBoardPlayer_.previousPage(bRealPrevious);
	}
 
   public void requestMute(boolean bDoMute, SRStreamPlayer origin)
   {
      if (bDoMute != masterPlayer_.isMute())
         masterPlayer_.muteOnOff();
   }
     
   public void requestClose(SRStreamPlayer origin)
   {
      if (daemon_ != null)
         daemon_.closeDocument(this);
   }
 
   
   public synchronized void informEndOfMediaReached(SRStreamPlayer origin)
   {
      EventInfo timeInfo = new EventInfo(EventInfo.MASTER,
                                         System.currentTimeMillis(),
                                         masterPlayer_.hashCode());
      
      //DebugOut("informEndOfMediaReached()", masterPlayer_.getMediaTime(), timeInfo);
      
      int iStopTime = masterPlayer_.getDuration();
      
      if (!m_bSpecialPause)
      {
         sendTimeToAllHelpers(iStopTime, timeInfo);
         if (!m_bSpecialPause)
         {
            // no stop mark at the end
            requestStop(false, masterPlayer_);
         }
      }
      // else stop mark encountered at the very end
         
      
      if (autoLoop_)
      {
         requestTime(0, masterPlayer_);
         requestStart(masterPlayer_);
         
         
         // if auto-loop is activated neither deactivate full screen 
         // nor bring the html display to the front
      }
      else
      {
         
         if (autoFullScreen_ == true && !m_bSpecialPause)
         {
            switchToFullScreen(true); // deactivate if activated
         }
         
         
         final String htmlTitle = Manager.mapLocationToTitle(protectLocation_);
         if (htmlTitle != null)
         {

            //if (System.getProperty("tutorial.mode") != null)
            {
               (new Thread() {
                  public void run()
                  {
                     try { sleep(1500); } catch (InterruptedException exc) {}
                     Manager.activateHtmlFrame(htmlTitle);
                  }
               }).start();
            }
         }
      }

   }

	 
	

	// daemon_ (external) time
	public void setTime(int time) {
		EventInfo timeInfo = new EventInfo(EventInfo.EXTERNAL,
										   System.currentTimeMillis(),
										   0);
		sendTimeToAllHelpers(time, timeInfo);

      //layout_.toFront(); // hm!
	}

   // bugfix for video to display its content
   // (must be called after being displayed)
   public void resetTime()
   {
      if (masterPlayer_ != null && !started_)
      {
         EventInfo timeInfo = new EventInfo(EventInfo.EXTERNAL,
                                            System.currentTimeMillis(),
                                            0);

         sendTimeToAllHelpers(masterPlayer_.getMediaTime(), timeInfo);
      }
   }

   public void closeIndirect()
   {
      ((GlobalMasterAndUI)daemon_).closeDocument(this);
   }

	public void close() {
      if (isClosed()) 
         return;

      
		EventInfo closeInfo = new EventInfo(EventInfo.EXTERNAL,
											System.currentTimeMillis(),
											0);
		closeAllPlayers(masterPlayer_, closeInfo); // hack: masterPlayer_ is not correct here
      // ALSO does a decrementCounter()

      if (layout_ != null)
         layout_.shutDown();

      isClosed_ = true;
   }

	public void start() {
		requestStart(masterPlayer_); // hack: masterPlayer_ is not correct here
	}
	
	public void stop() {
		requestStop(masterPlayer_); // hack: masterPlayer_ is not correct here
	}

   public void doStartOrStop()
   {
      if (started_)
         stop();
      else
         start();
   }

   public void jumpForward(int millis)
   {
      if (millis > 0)
      {
         int time = lastDispatchedTime_+millis;
         if (time > audioDuration_) time = audioDuration_;
         requestTime(time, masterPlayer_);
      }
      else
         requestTime(0, masterPlayer_);

   }

   public void jumpBackward(int millis)
   {
      if (millis > 0)
      {
         int time = lastDispatchedTime_-millis;
         if (time < 0) time = 0;
         requestTime(time, masterPlayer_);
      }
      else
         requestTime(audioDuration_, masterPlayer_);
   }

   public void increaseVolume()
   {
      masterPlayer_.increaseVolume();
   }

   public void decreaseVolume()
   {
      masterPlayer_.decreaseVolume();
   }

   public void muteOnOff()
   {
      masterPlayer_.muteOnOff();
   }

   public void activateLooping(boolean bLoop)
   {
      autoLoop_ = bLoop;
   }
   
   public void activateVideo()
   {
      if (videoDeactivated_)
      {
         videoDeactivated_ = false;
         if (videoPlayer_ != null && started_)
         {
            videoPlayer_.setMediaTime(masterPlayer_.getMediaTime(), null);
            videoPlayer_.start(null);
         }
      }
   }

   public void deactivateVideo()
   {
      if (!doClipsOnSlides_)  // TODO: review, this is a small hack to get the clips on the slides updating even if video is off
      {
         videoDeactivated_ = true;
         if (videoPlayer_ != null && started_)
            videoPlayer_.stop(null);
      }
   }

   public void restoreVideoSize()
   {
      if (videoPlayer_ != null)
      {
         layout_.restoreVideoSize();
      }
   }

   public void switchToFullScreenAuto()
   {
      autoFullScreen_ = true;
      switchToFullScreen(false);
   }

   public void switchToFullScreen(boolean escapePressed)
   {
      switchToFullScreen(escapePressed, false);
   }

   public void switchToFullScreen(final boolean escapePressed, boolean force)
   {  
      long seconds = (System.currentTimeMillis()/1000)%60;
      //System.out.println(seconds+" switchToFullScreen()");

      if (force)
         while (switchingToFullScreen_)
            try { Thread.sleep(300); } catch (InterruptedException exc) {}
      else
         if (switchingToFullScreen_)
            return;

      switchingToFullScreen_ = true;
      
      Thread t = new Thread("Document: Switch to fullscreen") {
         public void run()
         {
            boolean videoEnabled = true;
            if (videoPlayer_ != null)
               videoEnabled = !videoDeactivated_;//&& !videoPlayer_.isAccompaningVideo();
            // the video is disabled in full screen mode if it is 
            // deactivated (user button) or if it is only one (covering) video
      
            layout_.switchToFullScreen(escapePressed, videoEnabled, videoPlayer_);
            // should be blocking until layout operations et al are finished
            
            switchingToFullScreen_ = false;
         }
      };
      t.start();

      if (force)
         try { t.join(); } catch (InterruptedException exc) {}
   }
 
   public boolean isClosed()
   {
      return isClosed_;
   }

   public void layoutDrivenSetTimeAgain()
   {
      int time = masterPlayer_.getMediaTime();
      requestTime(time, masterPlayer_);
   }

   public void setHelperOffset(int helperIndex, int helperOffset)
   {
      offsets_[helperIndex+1] = helperOffset;
   }
   
   // interface Printable, Pageable
   public int getNumberOfPages()
   {
      if (getLocation() == null || whiteBoardPlayer_ == null)
         return 0;
      else
      {
         // TODO make object orientated??
         int[] aPageTimes = whiteBoardPlayer_.getEventQueue().pageChangeTimes();
         
         return aPageTimes.length;
      }
   }
    
   // interface Printable, Pageable
   public PageFormat getPageFormat(int pageIndex)
   {
      // Get the default page of the default printer.
      
      PrinterJob job = PrinterJob.getPrinterJob();
      PageFormat pf = job.defaultPage();
      
      // TODO make object orientated??
      
      Dimension dimPage = whiteBoardPlayer_.GetBoardSize();
      
      if (dimPage.width > dimPage.height)
         pf.setOrientation(PageFormat.LANDSCAPE);
      
      return pf;
   }
 
   // interface Printable, Pageable
   public Printable getPrintable(int pageIndex)
   {
      return this;
   }
    
	// interface Printable, Pageable
   public int print(Graphics g, PageFormat pageFormat, int iPageIndex)
   {    
      if (iPageIndex >= getNumberOfPages())
         return Printable.NO_SUCH_PAGE;
      
      Dimension dimPage = whiteBoardPlayer_.GetBoardSize();
      EventQueue eq = whiteBoardPlayer_.getEventQueue();
      int[] aPageTimes = eq.pageChangeTimes();
      int iMaxTimestamp = whiteBoardPlayer_.getDuration();
      
      int iTimeToPaint = 0;
      if (iPageIndex + 1 >= aPageTimes.length) // last page
         iTimeToPaint = iMaxTimestamp;
      else
         iTimeToPaint = aPageTimes[iPageIndex + 1] - 1; // last time of this page
          
      Rectangle2D.Double rcDrawArea = new Rectangle2D.Double(
         pageFormat.getImageableX(), pageFormat.getImageableY(),
         pageFormat.getImageableWidth(), pageFormat.getImageableHeight());
      
      Graphics2D g2 = (Graphics2D)g;
      
      RenderingHints rh = new RenderingHints(null);
      rh.put(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
      g2.setRenderingHints(rh);
      
      g2.translate(rcDrawArea.x, rcDrawArea.y);
      
      double dZoom = Math.min(rcDrawArea.width / dimPage.width, rcDrawArea.height / dimPage.height);
      
      eq.paint(g, iTimeToPaint, dZoom);
      
      return Printable.PAGE_EXISTS;
   }
   
   public String GetWindowTitle(String strSuffix)
   {
      String strWindowTitle = null;
      
      if (getMetaInfo() != null)
      {
         String strAuthor = (String)getMetaInfo().get(Manager.getLocalized("author"));
         String strTitle = (String)getMetaInfo().get(Manager.getLocalized("title"));
         if (strAuthor != null || strTitle != null)
         {
            if (strAuthor != null)
               if (strTitle != null)
                  strWindowTitle = strAuthor+": "+strTitle;
               else
                  strWindowTitle = strAuthor;
            else
               strWindowTitle = strTitle;
            
            if (strSuffix != null && strSuffix.length() > 0)
               strWindowTitle += " - "+strSuffix;
         }
      }
      
      return strWindowTitle;
   }

   private synchronized void sendTimeToAllHelpers(int time, EventInfo timeInfo)
   {
      //DebugOut("sendTimeToAllHelpers()", time, timeInfo);
  
      if (timeInfo != null)
      {
         if (timeInfo.source == EventInfo.MASTER && m_bIgnoreRegularTimeOnce)
         {
            m_bIgnoreRegularTimeOnce = false;
            return;
         }
      }
      
       
      if (stopPlayer_ != null)
      {
         // special handling of stop mark player; it may stop replay
         // and change the time during that
      
         boolean bWasStarted = started_;
         stopPlayer_.setMediaTime(time >= 0 ? time : 0, timeInfo);
         if (bWasStarted && !started_)
            time = masterPlayer_.getMediaTime();

      }
      
		for (int i=0; i<players_.length; i++) {
         if (!(videoDeactivated_ && players_[i] == videoPlayer_) && players_[i] != stopPlayer_)
         {
            int value = time-offsets_[i];
            value = (value >= 0) ? value : 0;
            players_[i].setMediaTime(value, timeInfo);
         }
		}
	}
	
	private void closeAllPlayers(SRStreamPlayer source, EventInfo closeInfo) {
		if (started_) requestStop(source);

      decrementCounter("closeAllPlayers for "+configFile_);
		
      if (players_ != null)
         for (int i=0; i<players_.length; i++)
            players_[i].close(closeInfo);
   }

   
   private ImageIcon createIcon(String location)
   {
		URL loc = getClass().getResource(location);
      if (loc != null)
         return new ImageIcon(loc);
      else
         return null;
	}
   
	

	
	private String[] createArgs(Object[] input, int offset) {
      return createArgs(input, offset, null);
   }
   
	private String[] createArgs(Object[] input, int offset, String[] additional) {
		int size = input.length-offset;
      if (additional != null)
         size += additional.length;
		if (size < 0) throw new IllegalArgumentException(
			"Invalid offset for options array copy ("+offset+" >= "+input.length+").");
		String[] output = new String[size];
		for (int i=0; i<input.length-offset; i++) output[i] = (String)input[offset+i];
      if (additional != null)
         System.arraycopy(additional, 0, output, input.length-offset, additional.length);
		
		return output;
	}
	

	private StartWindow buildGreeting(String title) {
      if (startWindow_ == null)
         startWindow_ = new StartWindow();
      else
         startWindow_.reset();

      startWindow_.setTitle(title);
      
      return startWindow_;
	}
   
   
   static class StartWindow extends JWindow {
		JProgressBar progress;
      JLabel label;

		StartWindow() {
         
			progress = new JProgressBar(0, 1);
         label = new JLabel("", JLabel.CENTER);
         
         JPanel holder = new JPanel(new GridLayout(0,1));
			holder.add(label);
			holder.add(progress);
			Border b1 = BorderFactory.createEmptyBorder(5,5,5,5);
         Border b2 = BorderFactory.createLineBorder(Color.black);
         holder.setBorder(BorderFactory.createCompoundBorder(b2,b1));
			setContentPane(holder);
			pack();


         Rectangle r = Daemon.getSplashBounds();
         setBounds(r.x, r.y+r.height, r.width, getPreferredSize().height);

		}

      public void setTitle(String title)
      {
         label.setText(title);
         //progress.setString(title);
      }

		public void setMaxProgress(int max) {
			progress.setMaximum(max);
		}
	
		public void increment() {
			progress.setValue(progress.getValue()+1);
         showNow();
      }

      public void reset()
      {
         progress.setValue(0);
      }

      public void showNow()
      {
         if (!isShowing())
            setVisible(true);
         else
            toFront();
      }

      public void hideNow()
      {
         dispose();
      }
   }

   private void DebugOut(String strMethod, int iTime, EventInfo info)
   {
      if (true)
      {
         System.out.print(strMethod+": "+iTime);
         if (info != null)
         {
            System.out.print(" ");
            long id = info.helperID;
            if (id == masterPlayer_.hashCode())
               System.out.print("MASTER");
            else if (whiteBoardPlayer_ != null && id == whiteBoardPlayer_.hashCode())
               System.out.print("WHITEBOARD");
            else if (stopPlayer_ != null && id == stopPlayer_.hashCode())
               System.out.print("STOP");
            else if (videoPlayer_ != null && id == videoPlayer_.hashCode())
               System.out.print("VIDEO");
            else
               System.out.print("UNKNOWN");
         }
         System.out.println();
      }
   }
   
   // PZI: added to force repaint if search was performed
   public void requestRepaintOfWhiteboard() {
      whiteBoardPlayer_.repaint();
   }
}
   