package imc.lecturnity.converter;

/* REVIEW UK
 * Class too complex, should be separated
 * startConvert(): ScreengrabbingExtended: Do not call methods in if(); also indentation
 */

import javax.swing.JOptionPane;

import java.awt.*;
import java.io.*;
import java.util.*;
import java.text.DateFormat;
import javax.swing.ImageIcon;

import imc.epresenter.filesdk.*;
import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.player.audio.Audio2Wav;
import imc.epresenter.player.util.RangeGroup;
import imc.epresenter.player.whiteboard.*;
import imc.epresenter.player.whiteboard.EventQueue; // otherwise confused with java.awt.EventQueue
import imc.epresenter.converter.*;
import imc.lecturnity.converter.wizard.PublisherWizardData;
import imc.lecturnity.converter.wizard.UploaderAccess;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.ui.LayoutUtils;
import imc.lecturnity.util.wizards.WizardPanel;

/******************************************************************************/

public abstract class StreamingMediaConverter extends Component
{
   /** Standard video size is 320 x 240 pixel */
   public static final int STANDARD_VIDEO_WIDTH   = 320;

   /** Standard video size is 320 x 240 pixel */
   public static final int STANDARD_VIDEO_HEIGHT  = 240;

   /** Standard screen size is 1024 x 768 pixel */
   public static final int STANDARD_SCREEN_WIDTH  = 1024;

   /** Standard screen size is 1024 x 768 pixel */
   public static final int STANDARD_SCREEN_HEIGHT = 768;

   /** Standard slide size, if we have no slides (Denver mode) */
   public static final int STANDARD_SLIDE_WIDTH   = 720;

   /** Standard slide size, if we have no slides (Denver mode) */
   public static final int STANDARD_SLIDE_HEIGHT  = 540;

   /** Standard maximum slide size for a screen resolution of 1024x768 pixel */
   public static final int MAX_SLIDE_WIDTH        = 600; //= 528;

   /** Standard maximum slide size for a screen resolution of 1024x768 pixel */
   public static final int MAX_SLIDE_HEIGHT       = 450; //= 396;

   /** The difference between the size of the browser window and the slide size */
   public static final int DELTA_BROWSER_TO_SLIDE_WIDTH  = 424; //= 418;

   /** The difference between the size of the browser window and the slide size */
   public static final int DELTA_BROWSER_TO_SLIDE_HEIGHT = 318; //= 377;

   public static final int S_OK = 0;


   /**
    * Some static variables indicating success
    * or specific error codes.
    * -----------------------------------------
    */

   /**
    *   0 = Success, everything was well done.
    */
   public static final int SUCCESS              = 0;

   /**
    *   1 = Warning message, that maybe the clip writing went rather bad.
    */
   public static final int CLIP_STARTS_BLACK    = 1;
  
   /**
    *   2 = Warning message, that there were too many frame drops.
    */
   public static final int TOO_HIGH_FRAME_DROP  = 2;
  
   /**
    *  -1 = Unable to create the platform specific object.
    */
   public static final int NO_OBJECT            = -1;

   /**
    *  -2 = Unable to parse the command line arguments.
    */
   public static final int PARSING_FAILED       = -2;

   /**
    *  -3 = Build Engine Initialization failed.
    */
   public static final int NO_BUILD_ENGINE      = -3;

   /**
    *  -4 = Encoding of the data stream failed.
    */
   public static final int ENCODING_FAILED      = -4;

   /**
    *  -5 = Unable to set name of output file.
    */
   public static final int NO_OUTPUT_FILE       = -5;

   /**
    *  -6 = Unable to open input file(s).
    */
   public static final int NO_INPUT_FILE        = -6;

   /**
    *  -7 = Decoding of the data stream failed.
    */
   public static final int DECODING_FAILED      = -7;

   /**
    *  -8 = Building the System Profiles failed.
    */
   public static final int PROFILES_FAILED      = -8;

   /**
    *  -9 = Initializing the converter failed.
    */
   public static final int CONVERTER_INIT_FAILED = -9;

   /**
    * -10 = Unable to create PictureExtracter.
    */
   public static final int NO_PIC_EXTRACT       = -10;

   /**
    * -11 = Could not find native DLLs.
    */
   public static final int DLL_NOT_FOUND        = -11;

   /**
    * -12 = Audio to WAV conversion failed
    */
   public static final int AUDIO2WAV_FAILED     = -12;

   /**
    * -13 = Unable to create RealPix files.
    */
   public static final int REALPIX_FAILED       = -13;

   /**
    * -14 = Unable to create SMIL file.
    */
   public static final int SMIL_FAILED          = -14;

   /**
    * -15 = Unable to create RPM file.
    */
   public static final int RPM_FAILED           = -15;

   /**
    * -16 = Unable to create RealText file.
    */
   public static final int REALTEXT_FAILED      = -16;

   /**
    * -17 = Unable to create HTML files.
    */
   public static final int HTML_FAILED          = -17;

   /**
    * -18 = Unable to copy image files.
    */
   public static final int NO_IMAGE_COPY        = -18;

   /**
    * -19 = Unable to copy Real Server files.
    */
   public static final int NO_REALSERVER_COPY   = -19;

   /**
    * -20 = Unable to create ASX file.
    */
   public static final int ASX_FAILED           = -20;

   /**
    * -21 = Unable to execute Redistributable Package.
    */
   public static final int NO_REDIST_PACKAGE    = -21;

   /**
    * -22 = The system need a reboot.
    */
   public static final int SYSTEM_NEEDS_REBOOT  = -22;

   /**
    * -23 = Unable to copy WindowsMedia Server files.
    */
   public static final int NO_WMSERVER_COPY     = -23;

   /**
    * -24 = Unable to copy WmSysPrx.prx.
    */
   public static final int NO_WMSYSPRX_COPY     = -24;

   /**
    * -25 = CoInitialize failed.
    */
   public static final int COINITIALIZE_FAILED  = -25;

   /**
    * -26 = Cleanup failed.
    */
   public static final int CLEANUP_FAILED       = -26;

   /**
    * -27 = Resizing of the WM profile failed.
    */
   public static final int WMPROFILE_SIZE_FAILED = -27;

   /**
    * -28 = Reading of the SG clip list failed.
    */
   public static final int SG_CLIPLIST_FAILED   = -28;

   /**
    * -29 = Audio codec not installed (Windows Media).
    */
   public static final int AUDIO_CODEC_NOT_INSTALLED = -29;

   /**
    * -30 = Custom: Conversion failed.
    */
   public static final int CONVERSION_FAILED    = -30;

   /**
    * -31 = TEMP/TMP variable not set.
    */
   public static final int NO_ENV_TEMP_VAR      = -31;

   /**
    * -32 = No write access on TEMP/TMP.
    */
   public static final int WRITE_ON_TEMP_FAILED = -32;

   /**
    * -33 = Creation of VML files failed.
    */
   public static final int VML_CREATION_FAILED  = -33;

   /**
    * -34 = Creating a directory failed.
    */
   public static final int CREATE_DIR_FAILED    = -34;

   /**
    * -35 = Invalid negative slide size.
    */
   public static final int INVALID_SLIDE_SIZE   = -35;

   /**
    * -36 = Invalid (too small) video size.
    */
   public static final int INVALID_VIDEO_SIZE   = -36;

   /**
    * -99 = Encoding process cancelled!
    */
   public static final int CANCELLED            = -99;


   /**
    * String with the Error Message
    */
   public static String STR_ERROR_MESSAGE =  "";



   public static Localizer m_Localizer;

   // Protected Members (used by Real and WindowsMediaConverter)
   protected DocumentInteractionInfo m_DocInfo = new DocumentInteractionInfo();
   protected ArrayList m_aStreamingFiles = null;
   protected String m_strAudioFileName = ""; // might change during conversion (temporary file)
   protected int m_nConversionStatus = SUCCESS; // can be used for signal/check cancelling
   protected long m_lDuration; // will be set by Real- and WindowsMediaConverter
   protected Dimension m_dimVideo = new Dimension(); // ??? set by Real-/WindowsMediaConverter?
   
   // "old" variables from TemplateParsingContainer; should probably replaced by
   // local method variables // ???
   protected String m_strPathFileName = null;
   protected String m_strPathName = null;
   protected String m_strFileName = null; // only used here
   protected String m_strInputPathName = null;
   protected String m_strHttpTargetPath = null;
   protected String m_strHttpTargetUrl = null; // only used here
   protected String m_strStreamingTargetPath = null;
   protected String m_strStreamingTargetUrl = null; // only used here
   
   
   private EventQueue m_EventQueue = null;
   private ObjectQueue m_ObjectQueue = null;
   private FileResources m_Resources = null;
   private ConversionDisplay m_Display = null;
   private ArrayList m_aJavaWrittenFiles = null;
   private PictureExtracter m_PictureExtracter = null;   
   private Dimension m_dimPagePicture = null;
   private Dimension m_dimPages = new Dimension();
   private Dimension m_dimStillImage = new Dimension();  
   private int m_nNumOfSlides;
   private int m_nNumOfEvents;
   private int m_nMaxLayerIndex;
   private String m_strZipFileName = "";

   
   private double m_dScaling = 1.0;
   private String m_strTempPath = null;
   
   private final String STR_IMAGE_SUFFIX = ".png";
   
   // This variable is used in conjunction with the SCORM progress
   // display update thread.
   private boolean m_bStopThread = false;

   
   // Private Members
   private static String SLIDE = "[!]";
   private static String WARNING = "[!]";
   private static String ERROR = "[!]";
   private static String METADATA_WARNING = "[!]";
   private static String COPYING_TO_SERVER_MESSAGE = "[!]";
   private static String WAIT_FOR_CANCEL = "[!]";
   private static String INIT_IMAGE = "[!]";
   private static String JOP_CONTINUE = "[!]";
   private static String JOP_CANCEL = "[!]";
   private static String CREATING_HTML = "[!]";
   private static String CREATING_SCORM = "[!]";
   private static String CREATING_IMAGES = "[!]";
   private static String TRANSFERRING_FILES = "[!]";
   private static String TRANSFERRING_STREAMING_SERVER = "[!]";

   protected PublisherWizardData m_pwData = null;


   // Dynamic bounded methods (= Virtual methods)
   protected abstract int convertAudioAndVideo();
   protected abstract int convertClipsOnly();
   // protected abstract int writeMetaFile();
   protected abstract int checkStreamingFileWritable();
   protected abstract int copyStreamingServerFiles();
   protected abstract ArrayList getStreamingServerFileNames();
   protected abstract int ReadAndSetVideoClips(
      String[] aClipFilenames, long[] aClipTimestamps);

   /***************************************************************************/

   /**
    * Create the Localizer
    */
   static
   {
      try
      {
         Localizer l 
            = new Localizer("/imc/lecturnity/converter/StreamingMediaConverter_",
                            "en");

         SLIDE = l.getLocalized("SLIDE");
         WARNING = l.getLocalized("WARNING");
         ERROR = l.getLocalized("ERROR");
         METADATA_WARNING = l.getLocalized("METADATA_WARNING");
         COPYING_TO_SERVER_MESSAGE = l.getLocalized("COPYING_TO_SERVER_MESSAGE");
         WAIT_FOR_CANCEL = l.getLocalized("WAIT_FOR_CANCEL");
         INIT_IMAGE = l.getLocalized("INIT_IMAGE");
         JOP_CONTINUE = l.getLocalized("JOP_CONTINUE");
         JOP_CANCEL = l.getLocalized("JOP_CANCEL");
         CREATING_HTML = l.getLocalized("CREATING_HTML");
         CREATING_SCORM = l.getLocalized("CREATING_SCORM");
         CREATING_IMAGES = l.getLocalized("CREATING_IMAGES");
         TRANSFERRING_FILES = l.getLocalized("TRANSFERRING_FILES");
         TRANSFERRING_STREAMING_SERVER = l.getLocalized("TRANSFERRING_STREAMING_SERVER");
         
         m_Localizer = l;
      }
      catch (IOException e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(null, 
                                       "Could not open the Locale database!",
                                       "Severe Error", 
                                       JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   /***************************************************************************/

   /**
    * Constructor
    *
    *    @param: PublisherWizardData pwData: the data from the Publisher Wizard.
    *    @param: FileResources resources: the file resourses. 
    *    @param: ConversionDisplay display: the interaction with a progress bar. 
    */
   public StreamingMediaConverter(PublisherWizardData pwData, 
                                  FileResources rsrc, 
                                  ConversionDisplay dspl)
   {
      super();

      m_pwData     = pwData;
      m_Resources  = rsrc;
      m_Display    = dspl;

      if (m_Display == null)
         m_Display = new DefaultConversionDisplay();
   }

   /***************************************************************************/

   private int initSlidesSize(Dimension dimOutputPageSize, 
                              Dimension dimOutputMaxClipsSize)
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      DocumentData dd = m_pwData.GetDocumentData();
      
      boolean bSeparateClips = ps.GetBoolValue("bShowClipsOnSlides");
      
      boolean bHasStillimage = dd.HasStillImage();
      boolean bHasVideoInVideoArea = dd.IsDenver() || dd.HasNormalVideo() ||
         (dd.HasClips() && !bSeparateClips);
   
      int iVideoWidth = bHasVideoInVideoArea ? ps.GetIntValue("iVideoWidth") : 0;
      int iVideoHeight = bHasVideoInVideoArea ? ps.GetIntValue("iVideoHeight") : 0;
  
      String strInputPath = dd.GetPresentationPath();
      String strStillimageName = null;
      if (bHasStillimage)
         strStillimageName = dd.GetStillImageName(true);
   
      Dimension dimWhiteboard = dd.GetWhiteboardDimension();
      Dimension dimMatchScreen = m_pwData.m_MatchingScreenDimension;
      Dimension dimCustomSlides = m_pwData.m_CustomPageDimension;

      int iSlideSizeType = ps.GetIntValue("iSlideSizeType");
      boolean bCalculateSlideSize = iSlideSizeType == PublisherWizardData.SLIDESIZE_STANDARD ||
         iSlideSizeType == PublisherWizardData.SLIDESIZE_SCREEN_RES;
      int iClipSizeType = ps.GetIntValue("iClipSizeType");
      boolean bClipSizeMaxed = iClipSizeType == PublisherWizardData.CLIPSIZE_MAXSLIDE;
      
      boolean bSlideSizeOriginal = iSlideSizeType == PublisherWizardData.SLIDESIZE_ORIGINAL;
      boolean bSlideSizeScreen = iSlideSizeType == PublisherWizardData.SLIDESIZE_SCREEN_RES;
      boolean bSlideSizeCustom = iSlideSizeType == PublisherWizardData.SLIDESIZE_CUSTOM;
      
      
      int screenWidth = 1024; // Default
      int screenHeight = 768;

      if (bSlideSizeOriginal)
      {
         dimOutputPageSize.width = dimWhiteboard.width;
         dimOutputPageSize.height = dimWhiteboard.height;
      }
      else if (bSlideSizeScreen)
      {
         screenWidth = dimMatchScreen.width;
         screenHeight = dimMatchScreen.height;
      }
      else if (bSlideSizeCustom)
      {
         dimOutputPageSize.width = dimCustomSlides.width;
         dimOutputPageSize.height = dimCustomSlides.height;
      }

      if (bCalculateSlideSize)
      {
         int stillImageWidth = 0;
         int stillImageHeight = 0;
         
         if (bHasStillimage)
         {
            Dimension dimImage = GetImageSize(strStillimageName);
            stillImageWidth = dimImage.width;
            stillImageHeight = dimImage.height;
         }
         
         int w = LPLibs.templateGetMaxSlidesWidth(screenWidth, iVideoWidth, stillImageWidth);
         int h = LPLibs.templateGetMaxSlidesHeight(screenHeight, iVideoHeight, stillImageHeight);
         
         if(dd.isClipStructured()) {
            // avoid upscaling of Whiteboard for structured SG clips
            w = Math.min(w, dimWhiteboard.width);
            h = Math.min(h, dimWhiteboard.height);
         }
         
         // Check aspect ratio
         Dimension origSize = new Dimension(dimWhiteboard.width, dimWhiteboard.height);
         float f = LayoutUtils.scale(origSize, new Dimension(w, h), 99f);
         
         dimOutputPageSize.width = origSize.width;
         dimOutputPageSize.height = origSize.height;
      }

      if (bClipSizeMaxed)
      {
         dimOutputMaxClipsSize.width = dimOutputPageSize.width;
         dimOutputMaxClipsSize.height = dimOutputPageSize.height;
         
         // PZI: Update  Clips size
         // Fix Bug 4696
         ps.SetIntValue("iMaxCustomClipWidth", dimOutputMaxClipsSize.width);
         ps.SetIntValue("iMaxCustomClipHeight", dimOutputMaxClipsSize.height);

         System.out.println("Max clip size: " + dimOutputMaxClipsSize.width + "x" + dimOutputMaxClipsSize.height);
      }

      System.out.println("Suggested slide size: " + dimOutputPageSize.width + "x" + dimOutputPageSize.height);
      
      // Check for negative slide dimensions 
      // which can be produced e.g. by large still images or large videos (Bug 2018)
      if (dimOutputPageSize.width <= 0 || dimOutputPageSize.height <= 0)
      {
         return INVALID_SLIDE_SIZE;
      }

      return SUCCESS;
   }

   public int checkStructuredScreengrabbing() {
      int res = SUCCESS;

      DocumentData dd = m_pwData.GetDocumentData();
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      // PZI: structured screengrabbing clips
      // Add/remove structure and fulltextsearch to/of screengrabbing clips
      if (res == SUCCESS) {
         boolean bResult = ScreengrabbingExtended.updateClipStructure(ps, dd);
         if (!bResult)
             res = CANCELLED;
      }
      
      return res;
   }

   /**
    * Starts the Converting routine.
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   public int startConvert()
   {
      int res = SUCCESS;

      m_nConversionStatus = SUCCESS;
      
      DocumentData dd = m_pwData.GetDocumentData();
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      boolean bScormEnabled = ps.GetBoolValue("bScormEnabled");
      boolean bIsDenver = dd.IsDenver();
      boolean bHasClips = dd.HasClips();
      boolean bSeparateClips = ps.GetBoolValue("bShowClipsOnSlides");
      boolean bHasStillimage = dd.HasStillImage();
      boolean bDoUpload = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                           && (ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_UPLOAD) );
      boolean bUseNetworkDrive = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                                  &&(ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_NETWORK_DRIVE) );
      boolean bUseStreamingServer = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                                   && (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_STREAMING_SERVER) );
      int iClipCount = dd.GetVideoClipInfos() != null ? dd.GetVideoClipInfos().length : 0;
      
      String strStillimageName = dd.GetStillImageName();
      String strInputPath = dd.GetPresentationPath();
      
      String strTemplatePath = ps.GetStringValue("strRmWmTemplateConfigFilePath"); // only in ProfiledSettings
      String strTemplateSettings = ps.GetStringValue("strRmWmTemplateConfigParameterList");
      
     
      // Note m_lDuration will be set by Real-/WindowsMediaConverter.convertAudioAndVideo()

      
      // Create a new File Container to collect the names of the new created files
      m_aStreamingFiles = new ArrayList(100);

      // In this container we put the relative file names of the slides
      // images. Relative to the temp file path.
      m_aJavaWrittenFiles = new ArrayList(100);

      LPLibs.templateInit();
      
      if (strTemplatePath.length() > 0)
         LPLibs.templateReadSettingsFromFile(strTemplatePath);
      else
         LPLibs.templateReadSettings(); // ??? use this as fallback; but: handle error correctly?
      
      // maybe already done by ScormTemplateWizardPanel (ReadSettings()!);
      // but in profile mode is not done yet
      if (strTemplateSettings.length() > 0)
         LPLibs.templateUseSettings(strTemplateSettings);

      LPLibs.templateSetScormEnabled(bScormEnabled);
      // Use a temporary directory for the HTML files. Thank you.
      LPLibs.templateSetTargetDirectory(null);
      // Where do the HTML files go?
      m_strTempPath = LPLibs.templateGetTargetDirectory();
      String strTargetPath = null;
      
      // (1.) Extract Path and Filenames
      if (res == SUCCESS) {      
         res = extractPathAndNames();
         // Now we are able to set the target path
         strTargetPath = m_strHttpTargetPath;
         if (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL)
            strTargetPath = m_strPathName;
      }

      // Meta Files are not used any more (1.6.1)
//      // (2.) Write Meta File 
//      if (res == SUCCESS)
//         res = writeMetaFile();
      
      // (2.a) Check if the StreamingMedia file can be written
      if (res == SUCCESS)
         res = checkStreamingFileWritable();

      // used as output variables below
      Dimension dimOutputPageSize = new Dimension(-1, -1);
      Dimension dimOutputMaxClipsSize = new Dimension(-1, -1);
      
      // (2.b) Init slides size
      if ((res == SUCCESS) && !bIsDenver)
         res = initSlidesSize(dimOutputPageSize, dimOutputMaxClipsSize);

      // (2.c) Convert Audio/Video into Streaming-Media
      if (res == SUCCESS)
         res = convertAudioAndVideo();
      
      String[] buttonStrings = {JOP_CONTINUE, JOP_CANCEL};
      String initString = JOP_CONTINUE;
      if (res != SUCCESS && res > 0 && m_nConversionStatus != CANCELLED)
      {
         String strWarningText = null;
         try { strWarningText = m_Localizer.getLocalized("WARNING_"+res); } catch (Exception exc) {}
         
         if (strWarningText != null && strWarningText.length() > 0)
         {
            // display warning message
            int iResult = JOptionPane.showOptionDialog(
               (Component)m_Display, strWarningText,
               WARNING, JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE,
               null, buttonStrings, initString);
         
            if (iResult == JOptionPane.YES_OPTION)
               res = SUCCESS;
            else
               m_nConversionStatus = CANCELLED;
         }
      }

      // Check, if process is cancelled
      if (m_nConversionStatus == CANCELLED)
         res = CANCELLED;
      
      // (2.c2) Convert Clips into Streaming-Media if needed
      if (res == SUCCESS && bHasClips && bSeparateClips && !bIsDenver)
         res = convertClipsOnly();
      
      if (res != SUCCESS && res > 0 && m_nConversionStatus != CANCELLED)
      {
         String strWarningText = null;
         try { strWarningText = m_Localizer.getLocalized("WARNING_"+res); } catch (Exception exc) {}
         
         if (strWarningText != null && strWarningText.length() > 0)
         {
            // display warning message
            int iResult = JOptionPane.showOptionDialog(
               (Component)m_Display, strWarningText,
               WARNING, JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE,
               null, buttonStrings, initString);
         
            if (iResult == JOptionPane.YES_OPTION)
               res = SUCCESS;
            else
               m_nConversionStatus = CANCELLED;
         }
      }
      
      // Check, if process is cancelled
      if (m_nConversionStatus == CANCELLED)
         res = CANCELLED;
  
    
      if ((res == SUCCESS) && !bIsDenver)
      {
         // Some things do not need to be done in the
         // Standalone Mode.

         
         // (3.) Resize the slide dimensions and set the scale factor
         if (res == SUCCESS)
            // PZI note: resizing was done in 2.b. Why here again? leads to rounding errors of a few pixels
            res = resizeSlideAndVideoDimensions(
               dimOutputPageSize, strInputPath + strStillimageName);
         // ??? notice side effect; 
         // changes m_dimPagePicture and m_dimPages.width
         

         // (4.) Create the Picture Extracter
         if (res == SUCCESS)
            res = createPictureExtracter();
         
         
         // (5a.) Get Slide Entries and Telepointer Entries vom the Event Queue
         if (res == SUCCESS)
         {
            // Start with 250ms == 4 fps
            long lMinTimeDifference = 250;
            
            do // a maximum of 4 runs, while trying to get the picture count below a boundary
            {
               //long time1 = System.currentTimeMillis();
               res = getEntriesFromEventQueue(lMinTimeDifference);
               //long time2 = System.currentTimeMillis();
               
               // Check, if process is cancelled
               if (m_nConversionStatus == CANCELLED)
                  res = CANCELLED;
              
               lMinTimeDifference *= 4;
            } 
            while (m_DocInfo.aSlideEntries.length > 600 && lMinTimeDifference < 20000 && res == SUCCESS);
            
         }
         
         // (5b.) Get Thumbnail Entries from Metadata
         if (res == SUCCESS)
            res = getThumbnailEntriesFromMetadata();

         // PZI:
         // (5c.) Get Fulltext from Object data
         if (res == SUCCESS)
            res = getFulltextFromEventQueue();
         
         // Check, if process is cancelled
         if (m_nConversionStatus == CANCELLED)
            res = CANCELLED;
         
         // (6.) Write Images and Thumbnails
         if (res == SUCCESS)
            res = writeImagesAndThumbs();
         
         // Check, if process is cancelled
         if (m_nConversionStatus == CANCELLED)
            res = CANCELLED;
      }
      
      // (7.) Set the keywords for the Template Parser
      if (res == SUCCESS)
         res = setKeywordsForParser();
      

      /******************************************
      // (8.) Write the HTML-Files and copy the images for the icon and buttons
      if (res == SUCCESS)
         res = writeHtmlFiles();
      *******************************************/

      /******************************************
      if (res == SUCCESS)
         res = copyImageFiles();
      *******************************************/

      // (8.) Write the HTML-Files according to template
      if (res == SUCCESS) {
         m_Display.displayCurrentFile("msg", 100, CREATING_HTML);
         m_Display.displayCurrentFileProgress(0);
         res = writeHtmlFilesNative();
         m_Display.displayCurrentFileProgress(100);
      }

      // (8.b) Copy the still image, if necessary
      if ( (res == SUCCESS) && bHasStillimage)
      {
         String strFileIn = strInputPath + strStillimageName;
         String strFileOut = m_strTempPath + strStillimageName;
         res = copyFile(strFileIn, strFileOut);
         
         m_aJavaWrittenFiles.add(strStillimageName);
      }
         
      // Check, if process is cancelled
      if (m_nConversionStatus == CANCELLED)
         res = CANCELLED;


      // (9.) SCORM
      if (bScormEnabled)
      {
         // SCORM packetizes everything it needs into a ZIP file,
         // so there is no need to copy anything else to the target
         // directory. In case there is a SMIL file, and the replay
         // type is not local, the SMIL file is not put into the
         // ZIP file, but only moved to the target directory and added
         // to the list of streaming files (m_aStreamingFiles ArrayList).

         m_Display.displayCurrentFile("msg", 100, CREATING_SCORM);
         
         if (res == SUCCESS)
            res = processScorm();
      }
      else
      {
         if (res == SUCCESS)
         {
            // Copy all files to the target directory
            java.io.FileFilter fileFilter = new java.io.FileFilter() {
               public boolean accept(File file)
               {
                  return true;
               }
            };
            FileUtils fileUtils = new FileUtils(null);
            boolean bCopyAll = m_pwData.m_bOverwriteAll;
            boolean success = fileUtils.copyRecursive(
               new File(m_strTempPath), new File(strTargetPath), fileFilter, null, bCopyAll);

            if (!success)
               res = HTML_FAILED;
         }

         if (res == SUCCESS)
         {
            // Check for a SMIL file. Is there one in the target directory
            // now? Add it to the streaming files list.
            java.io.FileFilter smiFilter = new java.io.FileFilter() {
               public boolean accept(File file)
               {
                  return file.getName().endsWith(".smi");
               }
            };
            File targetDir = new File(strTargetPath);
            File[] files = targetDir.listFiles(smiFilter);
            if (files.length > 0)
               m_aStreamingFiles.add(files[0].toString());
         }
      }

      // (10.) Upload and/or StreamingServer?
      if (res == SUCCESS && (bDoUpload || bUseNetworkDrive || bUseStreamingServer))
      {
         // Do we have to consider interactive opened files?
         String strEvqFileName = dd.GetPresentationPath() + dd.GetEvqFileName();
         String strObjFileName = dd.GetPresentationPath() + dd.GetObjFileName();
         m_pwData.m_aListOfInteractiveOpenedFiles
            = LPLibs.getListOfInteractiveOpenedFiles(new File(strEvqFileName)+"", 
                                                     new File(strObjFileName)+"");

         // Upload or Connected network drive?
         if (bDoUpload)
            res = doUploadAndStreamingServerCopies(bUseStreamingServer);
         else if (bUseNetworkDrive)
            res = useNetworkDriveAndDoStreamingServerCopies(bUseStreamingServer);
         else if (bUseStreamingServer)
         {
            int nNum = 1 + (bSeparateClips ? iClipCount : 0);
            //m_Display.displayCurrentFile("msg", nNum, COPYING_TO_SERVER_MESSAGE);
            m_Display.displayCurrentFile("msg", nNum, TRANSFERRING_STREAMING_SERVER);
            res = copyStreamingServerFiles();
         }
      }
      
      // Check, if process is cancelled
      if (m_nConversionStatus == CANCELLED)
         res = CANCELLED;
      
      // Check, if process is cancelled
      if (m_nConversionStatus == CANCELLED)
         res = CANCELLED;
      

      // Delete all new created files, if process is cancelled
      if (res == CANCELLED)
      {
         deleteNewBuildFiles();
      }

      // Clean up; Note: this deletes the slides images, too.
      LPLibs.templateCleanUp();
      
      return res;
   }

   /***************************************************************************/

   /**
    * Cancels the Converting process
    */
   public void cancelConvert()
   {
      m_Display.displayCurrentFile("msg", 0, WAIT_FOR_CANCEL);
      m_nConversionStatus = CANCELLED;

      //requestCancel();
   }

   /***************************************************************************/

   /**
    * Get the File List of all created Files (with full path & file name)
    *
    *    @return String[]: String Array with the File list.
    */
   public String[] getStreamingFileList()
   {
      String[] fileList = new String[m_aStreamingFiles.size()];
      m_aStreamingFiles.toArray(fileList);
      
      return fileList;
   }

   /***************************************************************************/

   /**
    * Get the output path and file name
    *
    *    @return String: String with the output path and file name but without suffix.
    */
   public String getPathFileName()
   {
      // used by Real- and WmpWriterWizardPanel
      return GetOutputPrefix();
   }

   /***************************************************************************/

   /**
    * Extracts Input/Output Paths and Filenames.
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int extractPathAndNames()
   {    
      int res = SUCCESS;
      
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      String strPresentationFilename = m_pwData.GetDocumentData().GetPresentationFileName();
      
      String strTargetName = ps.GetStringValue("strTargetPathlessDocName");
      String strLocalTargetDir = ps.GetStringValue("strTargetDirLocal");
      String strHttpTargetDir = ps.GetStringValue("strTargetDirHttpServer");
      String strStreamingTargetServer = ps.GetStringValue("strTargetDirStreamingServer");
      String strHttpUrl = ps.GetStringValue("strTargetUrlHttpServer");
      String strStreamingUrl = ps.GetStringValue("strTargetUrlStreamingServer");
      boolean bDoUpload = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                           && (ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_UPLOAD) );
      boolean bUseNetworkDrive = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                                  &&(ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_NETWORK_DRIVE) );
      if (bDoUpload || bUseNetworkDrive)
      {
         // Use temporary local upload dir
         strHttpTargetDir = strLocalTargetDir; //ps.GetStringValue("strTargetDirLocal"); 
         if (!strHttpTargetDir.endsWith(File.separator))
            strHttpTargetDir += File.separatorChar;
      }
      // Check target directory and create it, if necessary
      File targetDir = new File(strLocalTargetDir);
      targetDir.mkdirs();
      

      // Extract the input path from the presentation file name
      int iSeparatorPos = strPresentationFilename.lastIndexOf("\\");
      if (iSeparatorPos > -1)
      {
         m_strInputPathName =
            strPresentationFilename.substring(0, iSeparatorPos + 1);
         // includes \
      }
      else
         m_strInputPathName = "";
      
      String strSeparator = File.separator;

      // Extract the output path and the filename
      // Append '\' or '/' to the path, if necessary
      m_strPathName = strLocalTargetDir;
      m_strPathName = appendSeparator(m_strPathName, strSeparator);
     
      m_strFileName = strTargetName;
      m_strPathFileName = m_strPathName + m_strFileName;
   
      
      // HTTP target Path. Append '\' or '/', if necessary
      m_strHttpTargetPath = strHttpTargetDir;
      m_strHttpTargetPath = appendSeparator(m_strHttpTargetPath, strSeparator);

      // Streaming target Path. Append '\' or '/', if necessary
      m_strStreamingTargetPath = strStreamingTargetServer;
      m_strStreamingTargetPath = appendSeparator(m_strStreamingTargetPath, strSeparator);
         
      // HTTP target URL. Append '/', if necessary
      m_strHttpTargetUrl = strHttpUrl;
      m_strHttpTargetUrl = appendSlash(m_strHttpTargetUrl);

      // Streaming target URL. Append '/', if necessary
      m_strStreamingTargetUrl = strStreamingUrl;
      m_strStreamingTargetUrl = appendSlash(m_strStreamingTargetUrl);

      return res;
   }
 
   /***************************************************************************/

   /**
    * Appends a Slash '/' to a String, if necessary.
    *
    *    @param String sPathIn: Input Path String.
    *    @return String: Path String ending with a Slash.
    */
   protected String appendSlash(String strPathIn)
   {
      if (strPathIn == null)
         return null;

      String strPathOut = new String(strPathIn);
      
      if (strPathOut.endsWith("/") == false)
      {
         if (strPathOut.endsWith("\\") == false)
            strPathOut += "/";
         else
         {
            strPathOut = strPathOut.substring(0, (strPathOut.length()-1));
            strPathOut += "/";
         }
      }
      
      return strPathOut;
   }
   
   /***************************************************************************/

   /**
    * Appends a Separator (Slash '/' or Backslash '\') to a String, if necessary.
    *
    *    @param String sPathIn: Input Path String.
    *    @return String: Path String ending with the Separator.
    */
   protected String appendSeparator(String strPathIn, String strSeparator)
   {     
      if (!strPathIn.endsWith(strSeparator))
         return strPathIn + strSeparator;
      else
         return strPathIn;
   }
   
   /***************************************************************************/

   /**
    * Check the audio file and convert AIF to WAV (temporarily), if necessary.
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int checkAudioFile()
   {
      String strAudioInputName = m_pwData.GetDocumentData().GetAudioFileName();
      
      // Check, if the audio file has AIF format (suffix: .aif). 
      // If so ==> Convert Audio2Wav.
      if (strAudioInputName.toLowerCase().endsWith(".aif") == true)
      {
         m_strAudioFileName = m_strFileName + "_aif.tmp";
         System.out.println("- Convert: " + m_strInputPathName + strAudioInputName 
                                + " ==> " + m_strPathName + m_strAudioFileName);
         
         try
         {
            //// Issue: interface for requestCancel() (?)
            Audio2Wav.convert(m_strInputPathName + strAudioInputName, 
                              m_strPathName + m_strAudioFileName);
         }
         catch (Exception e)
         {
            e.printStackTrace();
            System.out.println("Unable to convert " + strAudioInputName);
            return AUDIO2WAV_FAILED;
         }
         
         m_strAudioFileName = m_strPathName + m_strAudioFileName;
      }
      else
         m_strAudioFileName = m_strInputPathName + strAudioInputName;

      return SUCCESS;
   }
   
   /***************************************************************************/

   /**
    * Create a Picture Extracter Object
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int createPictureExtracter()
   {
      int res = SUCCESS;

      try 
      {
         // Create the Picture Extracter
         m_PictureExtracter = 
            new PictureExtracter(m_pwData, m_Resources, m_dimPagePicture, m_Display);
      }
      catch (Exception e)
      {
         e.printStackTrace();
         System.out.println("Unable to create PictureExtracter.");
         return NO_PIC_EXTRACT;
      }

      // Get the Version Type of the ConverterWizard instance 
      // and set the Evaluation Flag, if necessary
      int nVersion = ConverterWizard.getVersionType();
      System.out.println("Version Type: " + nVersion);
      switch(nVersion)
      {
         case  NativeUtils.FULL_VERSION:
                  m_PictureExtracter.setDrawEvalNote(false);
                  break;
         case  NativeUtils.EVALUATION_VERSION:
                  m_PictureExtracter.setDrawEvalNote(true);
                  break;
         case  NativeUtils.UNIVERSITY_VERSION:
                  m_PictureExtracter.setDrawEvalNote(false);
                  break;
         case  NativeUtils.UNIVERSITY_EVALUATION_VERSION:
                  m_PictureExtracter.setDrawEvalNote(true);
                  break;
         default:
                  m_PictureExtracter.setDrawEvalNote(true);
                  break;
      }
      return res;
   }
   
   /***************************************************************************/

   /**
    * Get Slide Entries and Telepointer Entries from the Event Queue
    * (also Entries for interactive components).
    *
    *    @param long: minimum time difference between two events
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int getEntriesFromEventQueue(long lMinTimeDifference)
   {
      int res = SUCCESS;
      
      // ObjectQueue: Get the Visual Components from the Picture Extracter
      m_ObjectQueue = m_PictureExtracter.getObjectQueue();
      VisualComponent[] aVisualComponents = m_ObjectQueue.getVisualComponents();
      int numOfVisualComponents = aVisualComponents.length;

      // EventQueue: Get the Event Objects from the Picture Extracter
      // Consider: we are getting (n+1) events, because the very last event 
      // is a copy of the previous one
      m_EventQueue = m_PictureExtracter.getEventQueue();
      EventQueue.Event[] events = m_EventQueue.getEvents();
      
      // Get Interactive Areas/Buttons from ObjectQueue and process the data
      getAndProcessInteractionData();
      
      double dScale = m_dScaling;

      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      String strFilePrefix = ps.GetStringValue("strTargetPathlessDocName");
      
      boolean bUsePointer = ps.GetBoolValue("bRmWmUsePointer");

      // Get Stopmarks from ObjectQueue (if not done already)
      if (m_DocInfo.aStopmarkTimes == null)
         m_DocInfo.aStopmarkTimes = m_ObjectQueue.getStopTimes();
      // used only in TemplateSnippetCreator.createSnippets()
      
      int iSlideCount = 0;
      int iEventCount = 0;

      // Create ArrayLists as collectors for the slide entries and the telepointer entries 
      ArrayList slideEntryCollector          = null;
      ArrayList telepointerEntryCollector    = null;
      SlideEntry slideEntry                  = null;
      TelepointerEntry telepointerEntry      = null;

      // Create and initialize collectors for testing stuff: 
      // radio buttons ...
      ArrayList radioButtonCollector         = new ArrayList();
      // ... text fields ...  
      ArrayList textFieldCollector           = new ArrayList();
      // ... moveable pictures ...  
      ArrayList moveablePictureCollector     = new ArrayList();
      // ... target points ...
      ArrayList targetPointCollector         = new ArrayList();
      // ... changeable text ...
      ArrayList changeTextCollector          = new ArrayList();
      // ... question text.
      ArrayList questionTextCollector        = new ArrayList();

      // Get the number of Telepointer Objects
      int numOfPointers = 0;
      // Also get the testing stuff
      int numOfRadioButtons = 0;
      int numOfTextFields = 0;
      int numOfMoveablePictures = 0;
      int numOfTargetPoints = 0;
      int numOfChangeTexts = 0;
      int numOfQuestionTexts = 0;
      for (int i = 0; i < numOfVisualComponents; ++i)
      {
         if (aVisualComponents[i] instanceof Pointer)
            numOfPointers++;

         if (aVisualComponents[i] instanceof RadioDynamic)
         {
            radioButtonCollector.add((RadioDynamic)aVisualComponents[i]);
            numOfRadioButtons++;
         }
         if (aVisualComponents[i] instanceof TextFieldDynamic)
         {
            textFieldCollector.add((TextFieldDynamic)aVisualComponents[i]);
            numOfTextFields++;
         }
         if ((aVisualComponents[i] instanceof Picture) && (aVisualComponents[i].IsMoveable()))
         {
            moveablePictureCollector.add((Picture)aVisualComponents[i]);
            numOfMoveablePictures++;
         }
         if (aVisualComponents[i] instanceof TargetPoint)
         {
            targetPointCollector.add((TargetPoint)aVisualComponents[i]);
            System.out.println("TargetPoint[" + numOfTargetPoints + "]: " + ((TargetPoint)aVisualComponents[i]).GetCenterPoint().x + ", " + ((TargetPoint)aVisualComponents[i]).GetCenterPoint().y);
            numOfTargetPoints++;
         }
         if (aVisualComponents[i] instanceof Text)
         {
            if (((Text)aVisualComponents[i]).HasChangeableType())
            {
               changeTextCollector.add((Text)aVisualComponents[i]);
               numOfChangeTexts++;
            }
            else if (((Text)aVisualComponents[i]).HasTextType())
            {
               questionTextCollector.add((Text)aVisualComponents[i]);
               numOfQuestionTexts++;
            }
            // PZI: extract full text search data
            Text text = (Text)aVisualComponents[i];
//            System.out.println("TEXT ["+i+"]: "+text.getAllContent());
         }
      }

      // Initialize the collectors for slide/telepointer entries
      slideEntryCollector = new ArrayList(numOfVisualComponents - numOfPointers);
      telepointerEntryCollector = new ArrayList(numOfPointers);

      //
      // Now scan the event queue
      //
      EventQueue.Event lastEvent = null;
      EventQueue.Event thisEvent = null;
      boolean bLastEventIsTelepointer = false;
      long timestamp = 0;
      int nEventIndex = 0;
      int nPageIndex = 0;
      
      //
      // Begin loop: event queue
      //
      do
      {
         // Get the actual Event
         thisEvent = events[nEventIndex];

         // Get the timestamp from the actual Event
         timestamp = (long)thisEvent.getTimestamp();


         // Check the actual event. Is it a Telepointer?
         // tpIndex >=  0: Telepointer
         // tpIndex == -1: No Telepointer
         int tpIndex = getTelepointerIndexFromEvent(thisEvent);
         boolean bThisEventTelepointer = tpIndex >= 0;
         
         //if (tpIndex >= 0)
         {
            // Event is Telepointer
            
            // Check, if this is a new page 
            // - or if the number of objects has changed while the telepointer is visible
            // If so, we also have to add a new Slide Entry
            boolean isNewPage = false; 
            boolean needsNewSlideEntry = false;
   
            int nThisPage = thisEvent.getPage();
            int nThisNumOfObjects = thisEvent.content().size();
            int nLastPage = -1;
            int nLastNumOfObjects = -1;
            if (lastEvent != null)
            {
               nLastPage = lastEvent.getPage();
               nLastNumOfObjects = lastEvent.content().size();
            }
   
            // New page?
            if (nThisPage != nLastPage)
               needsNewSlideEntry = true;
            
            // A regular event?
            if (!bThisEventTelepointer)
               needsNewSlideEntry = true;
            
            if (bThisEventTelepointer && bLastEventIsTelepointer && nThisNumOfObjects != nLastNumOfObjects)
               needsNewSlideEntry = true;
            // ??? telepointer movements are not displayed??
               
            if (needsNewSlideEntry)
            {
               if (nThisPage != nLastPage)
               {
                  isNewPage = true;
                  nPageIndex++;
                  iSlideCount++;
               }
   
               iEventCount++;            
   
               int imageNumber = (nPageIndex-1); // Image number starts at 0
   
               // Image Name is something like "<prefix>_00001234"         
               String imageName = strFilePrefix + "_" + GetNumberString(timestamp, 8);
    
               // Create a new slide entry
               slideEntry = new SlideEntry(imageNumber, 
                                           timestamp,
                                           imageName,
                                           isNewPage,
                                           0,
                                           0,
                                           0,
                                           m_dimPagePicture.width,
                                           m_dimPagePicture.height);
   
   
               // Add the slide entry to the ArrayList ...
               slideEntryCollector.add(slideEntry);
               // ... and check the entries of the ArrayList
               checkSlideEntryCollector(slideEntryCollector, lMinTimeDifference);
               
               // Create a new Telepointer entry, if necessary
               telepointerEntry = new TelepointerEntry(timestamp, 
                                                       -1, 
                                                       -1,
                                                       false);
   
               // Add the Telepointer entry to the ArrayList ...
               telepointerEntryCollector.add(telepointerEntry);
               // ... and check the entries of the ArrayList
               checkTelepointerEntryCollector(telepointerEntryCollector);

               bLastEventIsTelepointer = false;
            }
            
            // Check, if Telepointer are used
            if (bThisEventTelepointer && bUsePointer)
            {
               // Get the Telepointer data
               timestamp = (long)thisEvent.getTimestamp();
               Pointer pointer = (Pointer)aVisualComponents[tpIndex];
               int x = pointer.getClip(dScale).x;
               int y = pointer.getClip(dScale).y; 
               // Check for pointer type (Pointer or mouse cursor)
               int nPointerType = TelepointerEntry.TYPE_POINTER;
               if (pointer.IsMouseCursor())
                   nPointerType = TelepointerEntry.TYPE_CURSOR;
            
               // Create a new Telepointer entry
               telepointerEntry = new TelepointerEntry(timestamp, 
                                                       x,
                                                       y,
                                                       nPointerType, 
                                                       true);

               // Add the Telepointer entry to the ArrayList ...
               telepointerEntryCollector.add(telepointerEntry);
               // ... and check the entries of the ArrayList
               if (bLastEventIsTelepointer)
                  checkTelepointerEntryCollector(telepointerEntryCollector);
            
               bLastEventIsTelepointer = true;
            }
            else
            {
               // do nothing
            }
         }

         // Increase the Event Index Number
         lastEvent = thisEvent;
         nEventIndex++;
      } // do ..
      while (nEventIndex < (events.length));
      //
      // End loop: event queue
      //
      
      System.out.println();
      
      m_nNumOfSlides = iSlideCount;
      m_nNumOfEvents = iEventCount;

      // Debug info
      System.out.println("+++ All Events:              " + (events.length-1));
      System.out.println("+++ Visual Components:       " + numOfVisualComponents);
      System.out.println("+++ Slides:                  " + m_nNumOfSlides);
      System.out.println("+++ Telepointer:             " + numOfPointers);
      System.out.println("+++ RadioButtons/CheckBoxes: " + numOfRadioButtons);
      System.out.println("+++ TextFields:              " + numOfTextFields);
      System.out.println("+++ MoveablePictures:        " + numOfMoveablePictures);
      System.out.println("+++ TargetPoints:            " + numOfTargetPoints);
      System.out.println("+++ ChangeTexts:             " + numOfChangeTexts);
      System.out.println("+++ QuestionTexts:           " + numOfQuestionTexts);
      System.out.println("+++ New Slide/Event Index:   " + slideEntryCollector.size() + " (" + m_nNumOfEvents + ")");
      System.out.println("+++ New Telepointer Index:   " + telepointerEntryCollector.size());
      

      // Convert the ArrayLists into normal Arrays
      m_DocInfo.aSlideEntries = new SlideEntry[slideEntryCollector.size()];
      slideEntryCollector.toArray(m_DocInfo.aSlideEntries);
      m_DocInfo.aTelepointerEntries = new TelepointerEntry[telepointerEntryCollector.size()];      
      telepointerEntryCollector.toArray(m_DocInfo.aTelepointerEntries);

      m_DocInfo.aTestingRadioButtons = new RadioDynamic[radioButtonCollector.size()];
      radioButtonCollector.toArray(m_DocInfo.aTestingRadioButtons);
      m_DocInfo.aTestingTextFields = new TextFieldDynamic[textFieldCollector.size()];
      textFieldCollector.toArray(m_DocInfo.aTestingTextFields);
      m_DocInfo.aTestingMoveablePictures = new Picture[moveablePictureCollector.size()];
      moveablePictureCollector.toArray(m_DocInfo.aTestingMoveablePictures);
      m_DocInfo.aTestingTargetPoints = new TargetPoint[targetPointCollector.size()];
      targetPointCollector.toArray(m_DocInfo.aTestingTargetPoints);
      m_DocInfo.aTestingChangeTexts = new Text[changeTextCollector.size()];
      changeTextCollector.toArray(m_DocInfo.aTestingChangeTexts);
      m_DocInfo.aTestingQuestionTexts = new Text[questionTextCollector.size()];
      questionTextCollector.toArray(m_DocInfo.aTestingQuestionTexts);
      
      return res;
   }
      
   // PZI:
   // extract Text objects as searchbase for full text search
   protected int getFulltextFromEventQueue()
   {
      int res = SUCCESS;

      // ObjectQueue: Get the Visual Components from the Picture Extracter
      if(m_ObjectQueue == null)
         m_ObjectQueue = m_PictureExtracter.getObjectQueue();
      VisualComponent[] aVisualComponents = m_ObjectQueue.getVisualComponents();
      
      // EventQueue: Get the Event Objects from the Picture Extracter
      // Consider: we are getting (n+1) events, because the very last event 
      // is a copy of the previous one
      if(m_EventQueue == null)
         m_EventQueue = m_PictureExtracter.getEventQueue();
      EventQueue.Event[] events = m_EventQueue.getEvents();
      
      // get text for each slide entry
      int eventNr = 0;
      int lastPageNr = -1;
      for (int thumbnailEntryNr = 0; thumbnailEntryNr < m_DocInfo.aThumbnailEntries.length; thumbnailEntryNr++) {
         ThumbnailEntry thumbnailEntry = m_DocInfo.aThumbnailEntries[thumbnailEntryNr];

         // buffer strings
         ArrayList<Text> aTextObjects = new ArrayList<Text>();

         // get range of slide entry
         long endOfSlideEntry = Long.MAX_VALUE;
         if (thumbnailEntryNr + 1 < m_DocInfo.aThumbnailEntries.length)
            // use lBeginTimestamp of next because lEndTimestamp may be inconsistent
            endOfSlideEntry = m_DocInfo.aThumbnailEntries[thumbnailEntryNr+1].lBeginTimestamp;
         else
            endOfSlideEntry = m_DocInfo.aThumbnailEntries[thumbnailEntryNr].lEndTimestamp;

         // get events
         while (eventNr < events.length
               && events[eventNr].getTimestamp() < endOfSlideEntry) {
            imc.epresenter.player.whiteboard.EventQueue.Event event = events[eventNr];
            lastPageNr = event.getPage();
            
            // iterate over objects
            RangeGroup rangeGroup = event.content();
            rangeGroup.initIterator();
            while (rangeGroup.hasNextValue()) {
               // get text object
               int objIndex = rangeGroup.nextValue();
               if (aVisualComponents[objIndex] instanceof Text) {
                  // add to searchbase
                  Text text = (Text) aVisualComponents[objIndex];
                                   
                  // objects may appear multiple times on subsequent events
                  if(!aTextObjects.contains(text)) {
                     // set timestamp to enable accessing search matches by event time
                     text.setTimestamp(event.getTimestamp());

                     aTextObjects.add(text);
                  }
               }
            }
            
            // get next event
            eventNr++;
         }
         thumbnailEntry.aSearchableText = aTextObjects.toArray(new Text[0]);
         
         if(eventNr < events.length) {            
            // skip event if part of current page (last event can have same timestamp as first event of next page)
            imc.epresenter.player.whiteboard.EventQueue.Event event = events[eventNr];
            if(event.getTimestamp()==endOfSlideEntry && event.getPage() == lastPageNr) {
               // System.out.println("    SKIPPED "+event);
               eventNr++;
            }
         }
      }
      return res;
   }
   
   /***************************************************************************/

   /**
    * Get Interactive Areas/Buttons from the Object Queue and process the data
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int getAndProcessInteractionData()
   {
      int res = SUCCESS;
      
      // Get "Interaction objects"
      int[] anInteractiveIndices = m_ObjectQueue.GetAllInteractionIndices();
      
      if ((anInteractiveIndices != null) && (anInteractiveIndices.length > 0))
      {
         System.out.println("- All interaction indices: " + anInteractiveIndices.length);
         // Create ArrayLists as collectors for interaction areas and interactive buttons
         ArrayList interactionAreaCollector     = new ArrayList(anInteractiveIndices.length);
         ArrayList interactionButtonCollector   = new ArrayList(anInteractiveIndices.length);

         String strIACType = "";
         for (int i = 0; i < anInteractiveIndices.length; ++i)
         {
            if (m_ObjectQueue.IsInteraction(anInteractiveIndices[i]))
            {
               InteractionArea iacArea = m_ObjectQueue.GetInteractionArea(anInteractiveIndices[i]);
               if (iacArea instanceof ButtonArea)
               {
                  strIACType = "Button ";
                  ButtonArea iacButton = (ButtonArea)iacArea;
                  MyButtonArea myIacButton = new MyButtonArea(iacButton);
                  interactionButtonCollector.add(myIacButton);
               }
               else if (iacArea instanceof RadioDynamic)
               {
                  // RadioButtons/CheckBoxes are handled seperately
                  if (((RadioDynamic)iacArea).IsCheckBox())
                     strIACType = "Check    ";
                  else
                     strIACType = "Radio    ";
               }
               else if (iacArea instanceof TextFieldDynamic)
               {
                  // TextFields are handled seperately
                  strIACType = "TextField";
               }
               else
               {
                  strIACType = "Area     ";
                  interactionAreaCollector.add(iacArea);
               }
               
               System.out.println("IAC " + i + ": " + strIACType + iacArea.toString());
            }
            else
            {
               // Question text, moveable object, changeable text, target point possible
               // They are handled in 'getEntriesFromEventQueue' --> do nothing here
            }
         }

         // Convert the ArrayLists into normal Arrays
         m_DocInfo.aInteractionAreas = new InteractionArea[interactionAreaCollector.size()];
         interactionAreaCollector.toArray(m_DocInfo.aInteractionAreas);
         m_DocInfo.aMyButtonAreas = new MyButtonArea[interactionButtonCollector.size()];
         interactionButtonCollector.toArray(m_DocInfo.aMyButtonAreas);
      }

      // Get "Questionnaires"
      Questionnaire[] aQuestionnaires = m_ObjectQueue.GetQuestionnaires(true);

      if ((aQuestionnaires != null) && (aQuestionnaires.length > 0))
      {
         System.out.println("Questionnaires: " + aQuestionnaires.length);
         m_DocInfo.aQuestionnaires = aQuestionnaires;

         for (int i = 0; i < aQuestionnaires.length; ++i)
         {
            System.out.println("+ Questionnaire " + i + ":"); 
            Question[] aQuestions = aQuestionnaires[i].GetQuestions();
            if ((aQuestions != null) && (aQuestions.length > 0))
            {
               for (int k = 0; k < aQuestions.length; ++k)
               {
                  System.out.println(" - Question " + k + ":"); 
                  System.out.println("     " + aQuestions[k].GetStartMs() + " - " + aQuestions[k].GetEndMs() + " ms"); 
                  System.out.println("     " + aQuestions[k].GetPoints() + " Pts. - " + aQuestions[k].GetMaximumAttempts() + " MaxAttempts - " + aQuestions[k].GetMaximumTime() + " MaxTimeMs"); 
               }
            }
         }
      }
      
      return res;
   }

   /***************************************************************************/

   /**
    * Upload files to a File server and/or copy files to a Streaming server.
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int doUploadAndStreamingServerCopies(boolean bUseStreamingServer)
   {
      int hr = SUCCESS;

      DocumentData dd = m_pwData.GetDocumentData();
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      boolean bScormEnabled = ps.GetBoolValue("bScormEnabled");
      boolean bIsDenver = dd.IsDenver();
      int i = 0;

      if (m_nConversionStatus != CANCELLED)
      {
         double dProgress = 0.0;
         int iWaitMs = 500;

         if (bScormEnabled)
         {
            String[] aFilesPlain = new String[] {m_strZipFileName};

            int iNumSteps = 10; // steps for progress
            m_Display.displayCurrentFile("msg", iNumSteps, TRANSFERRING_FILES);

            if ((aFilesPlain != null) && (aFilesPlain.length > 0))
               hr = UploaderAccess.StartUpload(
                  true, ps, aFilesPlain, (WizardPanel)m_Display);

            dProgress = 0.0;
            while (dProgress < 100.0 && hr == SUCCESS)
            {
               dProgress = UploaderAccess.GetUploadProgress(5);

               if (m_nConversionStatus == CANCELLED)
               {
                  UploaderAccess.CancelSession();
                  dProgress = 100.0;
                  break;
               }

               m_Display.displayCurrentFileProgress((int)(iNumSteps * dProgress/100.0));
               try { Thread.sleep(iWaitMs); } catch (Exception exc) { }
            }
            if (hr >= 0)
            {
               String strFileName = FileUtils.extractNameFromPathFile(m_strZipFileName);
               if (!strFileName.equals(m_strFileName))
               {
                  // Rename the local target document
                  moveFile(m_strZipFileName, m_strPathName + m_strFileName + ".zip");
               }
            }

            if (hr != 0)
            {                  
               if (hr > 0)
               {
                  // "Cancel" was pressed --> "Re"set hr
                  hr = CANCELLED;
               }
               else
               {
                  System.out.println("!!! Upload error in StreamingMediaConverter::doUploadAndStreamingServerCopies()1!");

                  // Finish Uploader Session
                  UploaderAccess.FinishSession();
                  m_pwData.m_bIsUploadSessionRunning = false;

                  int res = UploaderAccess.ShowErrorMessageForErrorCode(hr, (WizardPanel)m_Display);

                  if (res == 0) // "Verify"
                  {
                     // Goto Start dialog ...
                     m_pwData.B_GOTO_START_DIALOG = true;
                  }
                  else if (res == 1) // "Retry"
                  {
                     hr = doUploadAndStreamingServerCopies(bUseStreamingServer);
                     return hr;
                  }
                  else // "Cancel" or Generic error
                  {
                     // do nothing special, process is aborted
                  }
               }
            }
         }
         else // No SCORM, "normal" upload
         {
            ArrayList aFilesOne = new ArrayList();
            ArrayList aVideoFiles = getStreamingServerFileNames();
            fillStreamingArrayLists(aFilesOne, aVideoFiles);

            if (!bUseStreamingServer)
               aFilesOne.addAll(aVideoFiles);

            int iNumFilesOne = aFilesOne.size();
            int iNumVideoFiles = aVideoFiles.size();

            String[] aFilesPlain = new String[iNumFilesOne];
            aFilesOne.toArray(aFilesPlain);
            String[] aVideoFilesPlain = new String[iNumVideoFiles];
            aVideoFiles.toArray(aVideoFilesPlain);

            int iSizeKbPlain = FileUtils.getSizeKbOfFileList(aFilesPlain);
            int iSizeKbVideo = FileUtils.getSizeKbOfFileList(aVideoFilesPlain);
            
            int iSizeKbAll = iSizeKbPlain;
            if (bUseStreamingServer)
               iSizeKbAll += iSizeKbVideo;

            m_Display.displayCurrentFile("msg", iSizeKbAll, TRANSFERRING_FILES);

            // 4 Uploads necessary: "", "images\", "js\" and "slides\"
            int[] piSizeKbUploaded = new int[1]; // "Call by Reference"
            piSizeKbUploaded[0] = 0;
            // 1. "" (== 'root')
            if (hr == SUCCESS && (m_nConversionStatus != CANCELLED))
            {
               hr = doUploadOfSubDirFiles(aFilesPlain, 
                                          "", piSizeKbUploaded);
            }
            // 2. "images" 
            if (hr == SUCCESS && (m_nConversionStatus != CANCELLED))
            {
               // Empirical waiting time before another upload can start
               try { Thread.sleep(1500); } catch (Exception exc) { }
               hr = doUploadOfSubDirFiles(aFilesPlain, 
                                          "images", piSizeKbUploaded);
            }
            // 3. "js" 
            if (hr == SUCCESS && (m_nConversionStatus != CANCELLED))
            {
               try { Thread.sleep(1500); } catch (Exception exc) { }
               hr = doUploadOfSubDirFiles(aFilesPlain, 
                                          "js", piSizeKbUploaded);
            }
            // 4. "slides" (Not for 'Denver' documents)
            if (hr == SUCCESS && !bIsDenver && (m_nConversionStatus != CANCELLED))
            {
               try { Thread.sleep(1500); } catch (Exception exc) { }
               hr = doUploadOfSubDirFiles(aFilesPlain, 
                                          "slides", piSizeKbUploaded);
            }

            // Streaming server used?
            if (hr == SUCCESS && bUseStreamingServer && (m_nConversionStatus != CANCELLED))
            {
               try { Thread.sleep(1500); } catch (Exception exc) { }
               //m_Display.displayCurrentFile("msg", iSizeKbAll, COPYING_TO_SERVER_MESSAGE);
               m_Display.displayCurrentFile("msg", iSizeKbAll, TRANSFERRING_STREAMING_SERVER);

               if ((aVideoFilesPlain != null) && (aVideoFilesPlain.length > 0))
                  hr = UploaderAccess.StartUpload(
                     false, ps, aVideoFilesPlain, (WizardPanel)m_Display);
               
               dProgress = 0.0;
               while (dProgress < 100.0 && hr == SUCCESS)
               {
                  dProgress = UploaderAccess.GetUploadProgress(5);

                  if (m_nConversionStatus == CANCELLED)
                  {
                     UploaderAccess.CancelSession();
                     dProgress = 100.0;
                     m_Display.displayCurrentFileProgress(iSizeKbPlain + iSizeKbVideo);
                     break;
                  }

                  m_Display.displayCurrentFileProgress(iSizeKbPlain +(int)(iSizeKbVideo * dProgress/100.0));
                  try { Thread.sleep(iWaitMs); } catch (Exception exc) { }
               }

               if (hr != 0)
               {                  
                  if (hr > 0)
                  {
                     // "Cancel" was pressed --> "Re"set hr
                     hr = CANCELLED;
                  }
                  else
                  {
                     System.out.println("!!! Upload error in StreamingMediaConverter::doUploadAndStreamingServerCopies()2!");

                     // Finish Uploader Session
                     UploaderAccess.FinishSession();
                     m_pwData.m_bIsUploadSessionRunning = false;

                     int res = UploaderAccess.ShowErrorMessageForErrorCode(hr, (WizardPanel)m_Display);

                     if (res == 0) // "Verify"
                     {
                        // Goto Start dialog ...
                        m_pwData.B_GOTO_START_DIALOG = true;
                     }
                     else if (res == 1) // "Retry"
                     {
                        hr = doUploadAndStreamingServerCopies(bUseStreamingServer);
                        return hr;
                     }
                     else // "Cancel" or Generic error
                     {
                        // do nothing special, process is aborted
                     }
                  }
               }
            }
         }
      }

      return hr;
   }

   /**
    * Write files to a local subdirectory and copy files to connected network drive(s).
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int useNetworkDriveAndDoStreamingServerCopies(boolean bUseStreamingServer)
   {
      int res = SUCCESS;

      DocumentData dd = m_pwData.GetDocumentData();
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      boolean bScormEnabled = ps.GetBoolValue("bScormEnabled");
      boolean bIsDenver = dd.IsDenver();

      if (m_nConversionStatus != CANCELLED)
      {
         String strHttpTargetDir = ps.GetStringValue("strTargetDirHttpServer");
         String strStreamingTargetDir = strHttpTargetDir;
         if (bUseStreamingServer)
            strStreamingTargetDir = ps.GetStringValue("strTargetDirStreamingServer");

         if (bScormEnabled)
         {
            int iNumSteps = 10; // steps for progress
            m_Display.displayCurrentFile("msg", iNumSteps, TRANSFERRING_FILES);

            // Copy ZIP/LZP to target network drive
            copyFile(m_strZipFileName, strHttpTargetDir + new File(m_strZipFileName).getName());

            String strFileName = FileUtils.extractNameFromPathFile(m_strZipFileName);
            if (!strFileName.equals(m_strFileName))
            {
               // Rename the local target document
               moveFile(m_strZipFileName, m_strPathName + m_strFileName + ".zip");
            }
         }
         else // No SCORM, "normal" files
         {
            ArrayList aFilesOne = new ArrayList();
            ArrayList aVideoFiles = getStreamingServerFileNames();
            fillStreamingArrayLists(aFilesOne, aVideoFiles, true);

            int iNumFilesOne = aFilesOne.size();
            int iNumVideoFiles = aVideoFiles.size();

            int iNumFilesAll = iNumFilesOne + iNumVideoFiles;

            m_Display.displayCurrentFile("msg", iNumFilesAll, TRANSFERRING_FILES);

            String[] aFilesPlain = new String[iNumFilesOne];
            aFilesOne.toArray(aFilesPlain);
            String[] aVideoFilesPlain = new String[iNumVideoFiles];
            aVideoFiles.toArray(aVideoFilesPlain);

            // Create subdirecories "images", "js" and "slides" on the corresponding server 
            // (if they not exist already)
            File tmpFile = new File(strHttpTargetDir + "images");
            tmpFile.mkdir();
            tmpFile = new File(strHttpTargetDir + "js");
            tmpFile.mkdir();
            if (!bIsDenver)
            {
               tmpFile = new File(strHttpTargetDir + "slides");
               tmpFile.mkdir();
            }
            
            // Copy files to the corresponding server(s)
            int i = 0;
            for (i = 0; i < iNumFilesOne; ++i)
            {
               // 'aFilesPlain' elements do not contain the full path
               copyFile(GetOutputPath() + aFilesPlain[i], strHttpTargetDir + aFilesPlain[i]);
               m_Display.displayCurrentFileProgress(i);
            }

            for (i = 0; i < iNumVideoFiles; ++i)
            {
               // 'aVideoFilesPlain' elements contain the full path
               copyFile(aVideoFilesPlain[i], strStreamingTargetDir + new File(aVideoFilesPlain[i]).getName());
               m_Display.displayCurrentFileProgress(iNumFilesOne + i);
            }
         }
      }

      return res;
   }

   /**
    * Fill the ArrayLists for a) all (Template) files except video files and b) video files 
    */
   protected void fillStreamingArrayLists(ArrayList aFilesOne, ArrayList aVideoFiles)
   {
      fillStreamingArrayLists(aFilesOne, aVideoFiles, false);
   }

   protected void fillStreamingArrayLists(ArrayList aFilesOne, ArrayList aVideoFiles, boolean bIgnoreFullPath)
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      // a) Add all Template files 
      // ??? It seems that "<FileName>.html" is not part of LPLibs.templateGetFile() ...
      aFilesOne.add(ps.GetStringValue("strTargetPathlessDocName") + ".html");
      String smilFileName = ""; // The SMIL file must be handled in a special way (see below)
      int nCount = LPLibs.templateGetFileCount();
      for (int i = 0; i < nCount; ++i)
      {
         String resourceFileName = LPLibs.templateGetFile(i);
         if (!resourceFileName.endsWith(".smi")) // 'SMIL' file should not be added here
            aFilesOne.add(resourceFileName);
         else
            smilFileName = resourceFileName;
      }
      // Add all images files (which are written to the '\slides' directory
      aFilesOne.addAll(m_aJavaWrittenFiles);

      // Add Interactive opened files (if there are any)
      if (m_pwData.m_aListOfInteractiveOpenedFiles != null)
      {
         for (int i = 0; i < m_pwData.m_aListOfInteractiveOpenedFiles.length; ++i)
            aFilesOne.add(m_pwData.m_aListOfInteractiveOpenedFiles[i]);
      }

      // Add the full target path to the files in the list (if it should not be ignored)
      if (!bIgnoreFullPath)
         addTargetPathsToFileList(aFilesOne);


      // b) Video files
      // Unlike the Template and image files above these files are already listed with the full target path.
      if (aVideoFiles == null)
         aVideoFiles = new ArrayList();
      // Add the SMIL file to the video files (if it exists)
      if (!smilFileName.equals(""))
      {
         // Add the target path to the SMIL before adding it
         smilFileName = m_strPathName + smilFileName;
         aVideoFiles.add(smilFileName);
      }
   }

   /**
    * Add target path to each file from a file list. 
    *
    *    @return String[]: The file list with leading Separator.
    */
   protected void addTargetPathsToFileList(ArrayList aFileList)
   {
      // Create a new ArrayList which contains the path for each entry
      ArrayList aNewFileList = new ArrayList(aFileList.size());
      for (int i = 0; i < aFileList.size(); ++i)
      {
         String strFile = new String(aFileList.get(i)+"");
         strFile = m_strPathName + strFile;
         aNewFileList.add(strFile);
      }
      
      // Copy the new ArrayList back to the old one
      aFileList.clear();
      for (int i = 0; i < aNewFileList.size(); ++i)
         aFileList.add(aNewFileList.get(i)+"");
   }

   /**
    * Uploads the parts of a file list which matches the specified subdirectory. 
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int doUploadOfSubDirFiles(String[] aFilesPlain, 
                                       String strSubDirectory, int[] piSizeKbUploaded)
   {
      int hr = SUCCESS;

      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      double dProgress = 0.0;
      int iWaitMs = 500;

      String[] aFilesSubDir = getSubDirFilesFromFileList(aFilesPlain, strSubDirectory);
      int iSizeKbFiles = FileUtils.getSizeKbOfFileList(aFilesSubDir);

      if (strSubDirectory.equals(""))
      {
         if ((aFilesSubDir != null) && (aFilesSubDir.length > 0))
            hr = UploaderAccess.StartUpload(
               true, ps, aFilesSubDir, (WizardPanel)m_Display);
      }
      else
      {
         if (!strSubDirectory.endsWith("/"))
            strSubDirectory += "/";
         if ((aFilesSubDir != null) && (aFilesSubDir.length > 0))
            hr = UploaderAccess.StartUpload(
               true, ps, aFilesSubDir, (WizardPanel)m_Display, strSubDirectory);
      }

      dProgress = 0.0;
      while (dProgress < 100.0 && hr == SUCCESS)
      {
         dProgress = UploaderAccess.GetUploadProgress(5);

         if (m_nConversionStatus == CANCELLED)
         {
            UploaderAccess.CancelSession();
            dProgress = 100.0;
            m_Display.displayCurrentFileProgress(piSizeKbUploaded[0] + iSizeKbFiles);
            break;
         }

         m_Display.displayCurrentFileProgress(piSizeKbUploaded[0] + (int)(iSizeKbFiles * dProgress/100.0));
         try { Thread.sleep(iWaitMs); } catch (Exception exc) { }
      }

      piSizeKbUploaded[0] += iSizeKbFiles;

      if (hr != 0)
      {                  
         if (hr > 0)
         {
            // "Cancel" was pressed --> "Re"set hr
            hr = CANCELLED;
         }
         else
         {
            System.out.println("!!! Upload error in StreamingMediaConverter::doUploadOfSubDirFiles()!");

            // Finish Uploader Session
            UploaderAccess.FinishSession();
            m_pwData.m_bIsUploadSessionRunning = false;

            int res = UploaderAccess.ShowErrorMessageForErrorCode(hr, (WizardPanel)m_Display);

            if (res == 0) // "Verify"
            {
               // Goto Start dialog ...
               m_pwData.B_GOTO_START_DIALOG = true;
            }
            else if (res == 1) // "Retry"
            {
               hr = doUploadOfSubDirFiles(aFilesPlain, strSubDirectory, piSizeKbUploaded);
               return hr;
            }
            else // "Cancel" or Generic error
            {
               // do nothing special, process is aborted
            }
         }
      }

      return hr;
   }

   /**
    * Extract all files from a specified subdirectory from a file list. 
    * If no subdirectory is specified than all files from the "root" directory 
    * are extracted. 
    *
    *    @return String[]: The file list with the extracted files.
    */
   protected String[] getSubDirFilesFromFileList(String[] aPathFileList, String strSubDirectory)
   {
      if (!strSubDirectory.endsWith(File.separator))
         strSubDirectory += File.separator;

      String[] aFileList = new String[aPathFileList.length];
      int i = 0;
      for (i = 0; i < aFileList.length; ++i)
      {
         // Remove the target path from each file
         aFileList[i] = aPathFileList[i];
         if (aFileList[i].startsWith(m_strPathName))
            aFileList[i] = aFileList[i].substring(m_strPathName.length());
      }

      ArrayList aSubDirFiles = new ArrayList();

      for (i = 0; i < aFileList.length; ++i)
      {
         if (!strSubDirectory.equals(File.separator))
         {
            // Normal case: a subdirectory is defined
            if (aFileList[i].startsWith(strSubDirectory))
               aSubDirFiles.add(aPathFileList[i]);
         }
         else
         {
            // Special case: no subdirectory is defined
            // --> get all File from the "root" directory
            String strImages = "images" + File.separator;
            String strJs     = "js" + File.separator;
            String strSlides = "slides" + File.separator;
            
            boolean bIsRootDir = true;
            if (aFileList[i].startsWith(strImages))
               bIsRootDir = false;
            if (aFileList[i].startsWith(strJs))
               bIsRootDir = false;
            if (aFileList[i].startsWith(strSlides))
               bIsRootDir = false;

            if (bIsRootDir)
               aSubDirFiles.add(aPathFileList[i]);
         }
      }

      String[] aNewFileList = new String[aSubDirFiles.size()];
      aSubDirFiles.toArray(aNewFileList);
      
      return aNewFileList;
   }

   /***************************************************************************/

   /**
    * Get Thumbnail Entries from the Metadata
    * If no metadata file exists, we have to extract the information from the SlideEntries
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int getThumbnailEntriesFromMetadata()
   {
      int res = SUCCESS;
      
      DocumentData dd = m_pwData.GetDocumentData();
      
      Metadata metadata = dd.GetMetadata();
      VideoClipInfo[] aClipInfos = dd.GetVideoClipInfos();
      int iClipCount = aClipInfos != null ? aClipInfos.length : 0;

      int nNumOfThumbnails = 0;
      
      int clipIndex = 0;
      long lBeginTimestamp = 0;
      long lEndTimestamp = 0;
      String strImageName = "";
      String strTitle = "";
      String strCaption = "";
      String strClipFileName = "";
      boolean bHasErrors = false;

      if (metadata != null)
      {
         // Create and fill Thumbnail Entry Objects
         nNumOfThumbnails = metadata.thumbnails.length;
         m_DocInfo.aThumbnailEntries = new ThumbnailEntry[nNumOfThumbnails];
         //tpcAll.nNumOfThumbnails = nNumOfThumbnails;

         for (int i = 0; i < nNumOfThumbnails; ++i)
         {
            // Get the Timestamps
            lBeginTimestamp = metadata.thumbnails[i].beginTimestamp;
            lEndTimestamp = metadata.thumbnails[i].endTimestamp;
            
            // Get the Image Name and - if necessaray - the Clip File Name
            // This can be either an Image or a Video Clip
            // Image Name is something like "<prefix>_00001234_thumb"
            strImageName = m_strFileName + "_" + GetNumberString(lBeginTimestamp, 8);
 
            if (metadata.thumbnails[i].isVideoClip)
            {
               strImageName += "_clip_thumb";
               if (clipIndex < iClipCount && aClipInfos[clipIndex] != null)
               {
                  strClipFileName = aClipInfos[clipIndex].clipFileName;
               }
               else
               {
                  // The metadata file seems to be corrupt
                  // TODO: Show a warning message box
                  // This will be done after the loop 
                  strClipFileName = "Clip " + clipIndex;
                  bHasErrors = true;
               }
               
               clipIndex++;
            }
            else
            {
               strImageName += "_thumb";
               strClipFileName = "";
            }
            
            // Get the Title (it can be an empty string, if it is not shown)
            int offset = metadata.thumbnails[i].titleOffset;
            if (offset < 0)
               offset = 0;
            if ( (metadata.thumbnails[i].chapterTitles != null) 
              && (metadata.thumbnails[i].displayTitle == true) )
            {
               // Bugfix #2692
               try
               {
                  strTitle = metadata.thumbnails[i].chapterTitles[offset];
               }
               catch (Exception e)
               {
                  e.printStackTrace();
                  strTitle = "";
               }
            }
            else
               strTitle = "";
            
            // Get the Caption
            strCaption = metadata.thumbnails[i].title;
            
            // Fill the Thumbnail Entry Object 
            m_DocInfo.aThumbnailEntries[i] = new ThumbnailEntry(lBeginTimestamp, 
                                                            lEndTimestamp, 
                                                            strImageName, 
                                                            strTitle, 
                                                            strCaption, 
                                                            strClipFileName);
         }
      }
      else
      {
         System.out.println("In getThumbnailEntriesFromMetadata(): No Metadata!"); 

         // We have to extract the information from the SlideEntries
         
         // Get the timestamps of new pages
         int i = 0;
         int idx = 0;
         long[] pagingTimes = new long[(m_DocInfo.aSlideEntries.length + 1)];
         
         for (i = 0; i < m_DocInfo.aSlideEntries.length; ++i)
         {
            if (m_DocInfo.aSlideEntries[i].bIsNewPage)
            {
               pagingTimes[idx] = m_DocInfo.aSlideEntries[i].lTimestamp;
               idx++;
            }
         }
         pagingTimes[idx] = m_lDuration;
         idx++;

         nNumOfThumbnails = (idx - 1);
         m_DocInfo.aThumbnailEntries = new ThumbnailEntry[nNumOfThumbnails];
         //tpcAll.nNumOfThumbnails = nNumOfThumbnails;

         for (i = 0; i < nNumOfThumbnails; ++i)
         {
            // Get the Timestamps
            lBeginTimestamp = pagingTimes[i];
            lEndTimestamp = (pagingTimes[i+1] - 1);
            
            // Get the Image Name
            // Image Name is something like "<prefix>_00001234_thumb"
            strImageName = m_strFileName + "_" + GetNumberString(lBeginTimestamp, 8) + "_thumb";
            /*
            for (int j = 7; j > 0; --j)
            {
               if (lBeginTimestamp < Math.pow(10, j))
               strImageName += "0";
            }
            strImageName += lBeginTimestamp; 
            strImageName += "_thumb";
            */

            strClipFileName = "";
            strTitle = "";
            
            // Get the Caption
            strCaption = String.valueOf(i+1) + ". " + SLIDE;

            // Fill the Thumbnail Entry Object 
            m_DocInfo.aThumbnailEntries[i] = new ThumbnailEntry(lBeginTimestamp, 
                                                            lEndTimestamp, 
                                                            strImageName, 
                                                            strTitle, 
                                                            strCaption, 
                                                            strClipFileName);
         }
      }

      if (iClipCount > 0)
      {
         if (bHasErrors || (!dd.isClipStructured() && clipIndex != iClipCount))
         {
            // Show a warning message box
            JOptionPane.showMessageDialog(null, 
                                          METADATA_WARNING,
                                          WARNING, 
                                          JOptionPane.ERROR_MESSAGE);
         }
      }
      
      // Debug Info
      System.out.println("+++ Thumbnails:              " + nNumOfThumbnails);
      System.out.println("+++ Clips:                   " + clipIndex);
      
      return res;
   }
      
   /***************************************************************************/

   /**
    * Get the Telepointer Index from a given Event
    * 
    *    @param Event actualEvent: An Event from the EventQueue
    *    @return int: Object Index of Telepointer (if successful); 
    *                 -1 otherwise, if no Telepointer
    */
   protected int getTelepointerIndexFromEvent(EventQueue.Event actualEvent)
   {
      int nObjectIndex = actualEvent.getLastNumber();
      
      VisualComponent[] aVisualComponents = 
         m_ObjectQueue != null ? m_ObjectQueue.getVisualComponents() : null;
      
      if (aVisualComponents != null && aVisualComponents[nObjectIndex] instanceof Pointer)
         return nObjectIndex;
      else
         return -1;
   }
   
   /***************************************************************************/

   /**
    * Check the ArrayList with the Telepointer entries. 
    * Remove the previous entry, if it is too near to the last entry.
    * 
    *    @param ArrayList tpList: Telepointer Entry Collector
    *    @return int: 0: no changes; 
    *                 1: previous entry removed;
    *                 2: last entry removed.
    */
   protected int checkTelepointerEntryCollector(ArrayList tpList)
   {
      if (tpList.size() < 2)
         return 0;
      
      int nMaxDeltaDistance = 1;
      int nMaxDeltaTime = 100;
         
      TelepointerEntry lastTpEntry = (TelepointerEntry)tpList.get(tpList.size()-2);
      TelepointerEntry thisTpEntry = (TelepointerEntry)tpList.get(tpList.size()-1);

      // Previous Hidden Telepointer should not be removed
      boolean lastTpIsVisible = lastTpEntry.bIsVisible;
      boolean thisTpIsVisible = thisTpEntry.bIsVisible;
      if (!lastTpIsVisible)
      {
         if (thisTpIsVisible)
            return 0;
         else
         {
            // We have to remove the last entry
            tpList.remove(tpList.size()-1);
            return 2;
         }
      }
            
      long d_timestamp;
      int d_x;
      int d_y;
      int d_xy;
      
      d_timestamp = thisTpEntry.lTimestamp - lastTpEntry.lTimestamp;
      d_x         = thisTpEntry.nX         - lastTpEntry.nX; 
      d_y         = thisTpEntry.nY         - lastTpEntry.nY; 
      d_xy = (int)Math.sqrt((d_x * d_x) + (d_y * d_y));
         
      if (d_timestamp < 0) // This should be impossible (but has happened)
      {
         // Debug info
         System.out.println("Warning: Event with d_timestamp < 0 removed!");
         // We have to remove the last entry
         tpList.remove(tpList.size()-1);
         return 2;
      }

      boolean eventsInSameIntervall = 
      	eventsAreInSameTimeInterval(lastTpEntry.lTimestamp, thisTpEntry.lTimestamp, 
                                     nMaxDeltaTime);
      
      boolean isFarAway = true;
      if (d_xy < nMaxDeltaDistance)
         isFarAway = false;
         
      if (!eventsInSameIntervall && isFarAway)
      {
         return 0;
      }
      else
      {
      	if (eventsInSameIntervall)
      	{
	         // We have to remove the previous entry
	         thisTpEntry.lTimestamp = lastTpEntry.lTimestamp;
	         tpList.remove(tpList.size()-2);
         	return 1;
      	}
      	else if (!isFarAway)
      	{
	         // We have to remove the actual entry
	         tpList.remove(tpList.size()-1);
         	return 1;
      	}
      }
      
      return 0;
   }
   
   /***************************************************************************/

   /**
    * Check the ArrayList with the Slide entries. 
    * Remove the previous entry, if it is too near to the last entry.
    * 
    *    @param ArrayList sldList: Slide Entry Collector
    *    @param long: minimum time difference between two events
    *    @return int: 0: no changes; 
    *                 1: previous entry removed;
    *                 2: last entry removed.
    */
   protected int checkSlideEntryCollector(ArrayList sldList, long lMinTimeDifference)
   {
      if (sldList.size() < 2)
         return 0;
      
      boolean bUseDynamic = m_pwData.GetProfiledSettings().GetBoolValue("bRmWmUseDynamic");
         
      SlideEntry lastSldEntry = (SlideEntry)sldList.get(sldList.size()-2);
      SlideEntry thisSldEntry = (SlideEntry)sldList.get(sldList.size()-1);

      long lastTimestamp = lastSldEntry.lTimestamp;
      long thisTimestamp = thisSldEntry.lTimestamp;
      int lastImageNumber = lastSldEntry.nImageNumber;
      int thisImageNumber = thisSldEntry.nImageNumber;
      boolean lastIsNewPage = lastSldEntry.bIsNewPage;
      boolean thisIsNewPage = thisSldEntry.bIsNewPage;
      
      long d_timestamp = thisSldEntry.lTimestamp - lastSldEntry.lTimestamp;
      
      // We split the time line into intervals. Are the current
      // event and the previous event in different intervals?
      // If yes, then keep the previous event, otherwise replace
      // that event with the current event: This event is in the
      // same interval, but has a larger time stamp than the
      // previous. See below: The current event will always
      // be added to the queue, and if we need to replace an
      // event, it is enough to simply remove the previous event.
      boolean replaceEvent = eventsAreInSameTimeInterval(lastTimestamp, thisTimestamp, 
                                                         lMinTimeDifference);
      
      // If Dynamic is not used, we only need the Paging Events
      if (bUseDynamic == false)
      {
         if (!thisIsNewPage)
         {
            // We have to remove the last entry
            sldList.remove(sldList.size()-1);
            m_nNumOfEvents--;
            return 2;
         }
      }
            
      // Incorrect timeline
      if (d_timestamp < 0) // This should be impossible (but has happened)
      {
         // Debug info
         System.out.println("Warning: Event with d_timestamp < 0 removed!");
         // We have to remove the last entry
         sldList.remove(sldList.size()-1);
         m_nNumOfEvents--;
         return 2;
      }

      // Ignore Events at the very end (and 20 ms before)
      if (thisTimestamp >= (m_lDuration - 20))
      {
         // ??? This necessary anymore? This special case was removed elsewhere...
         
         // Debug info
         System.out.println("Warning: Event at the very end removed!");
         // We have to remove the last entry
         sldList.remove(sldList.size()-1);
         m_nNumOfEvents--;
         return 2;
      }

	  // Now perform the check: Is the current event
	  // near the previous event? If so, eventually
	  // replace the previous event with the current
	  // (this is done by discarding the previous
	  // event; the new event is inserted in any case,
	  // further below).
      if (d_timestamp <= lMinTimeDifference) // default: 250 ms
      {
         // Bugfix 4127/4359:
         if ((d_timestamp == 0) && thisIsNewPage && !lastIsNewPage && (lastSldEntry.nLayerIndex == 0))
         {
            // We have to remove the previous entry
            sldList.remove(sldList.size()-2);
            m_nNumOfEvents--;
            return 1;
         }

         if (lastIsNewPage && thisIsNewPage)
         {
            return 0;
         }
         else if (lastIsNewPage && !thisIsNewPage) 
         {
            return 0;
         }
         else if (!lastIsNewPage && thisIsNewPage)
         {
            if (!replaceEvent)
            {
               return 0;
            }
            else
            {
               if (lMinTimeDifference <= 1000)
               {
                  // We have to remove the previous entry
                  sldList.remove(sldList.size()-2);
                  m_nNumOfEvents--;
                  return 1;
               }
               else
               {
                  // Do not remove the (last) action before a (distatnt) slide change
                  // it might carry important information 
                  return 0;
               }
            }
         }
         else
         {
            if (!replaceEvent)
            {
               return 0;
            }
            else
            {
               // We have to remove the previous entry
               sldList.remove(sldList.size()-2);
               m_nNumOfEvents--;
               return 1;
            }
         }
      }

      return 0;
   }
      
   /***************************************************************************/

   /**
    * Check, if two timestamps are in the same time interval
    * 
    *    @param long: timestamp of event 1
    *    @param long: timestamp of event 2
    *    @param long: minimum time difference between two events
    *    @return boolean: true: events are in the same time interval; 
    *                     false: they are not in the same time interval
    */
   protected boolean eventsAreInSameTimeInterval(long time1, long time2, 
                                                 long lMinTimeDifference)
   {
      boolean isSameInterval = false;
      int lastMod = (int)(time1 / lMinTimeDifference);
      int thisMod = (int)(time2 / lMinTimeDifference);
      if (lastMod == thisMod)
      {
         // both events are in the same time interval,
         isSameInterval = true;
      }
      return isSameInterval;
   }

   /***************************************************************************/

   /**
    * Resize the width and the height of the video/slides and set the scale factor
    * (depending on video, still image, etc.)
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int resizeSlideAndVideoDimensions(Dimension dimSlideOutput,
                                               String strStillImageName)
   {
      // Video or Still Image?
      DocumentData dd = m_pwData.GetDocumentData();
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      boolean bSeparateClips = m_pwData.GetProfiledSettings().GetBoolValue("bShowClipsOnSlides");
      
      boolean bHasVideoInVideoArea = dd.HasNormalVideo() || dd.HasClips() && !bSeparateClips;
      boolean bMixClips = dd.HasClips() && !bSeparateClips;
      boolean bHasStillImage = dd.HasStillImage() && !bMixClips;
      
      
      double scaleX = 1.0;
      double scaleY = 1.0;
      int nMinVideoWidth = STANDARD_VIDEO_WIDTH;
      
      //
      // Resize slide dimensions
      //

      // The slide size is not fixed, it could have been set by the user
      
///      scaleX = (double)dimSlideOutput.width / (double)m_dimPagePicture.width;
///      scaleY = (double)dimSlideOutput.height / (double)m_dimPagePicture.height;
      scaleX = (double)dimSlideOutput.width / (double)dd.GetWhiteboardDimension().width;
      scaleY = (double)dimSlideOutput.height / (double)dd.GetWhiteboardDimension().height;

      m_dScaling = (scaleX < scaleY) ? scaleX : scaleY;

///      m_dimPagePicture.width = (int)( (double)m_dimPagePicture.width * m_dScaling + 0.5 );
///      m_dimPagePicture.height = (int)( (double)m_dimPagePicture.height * m_dScaling + 0.5 );
     
      int iPagePictureWidth = (int)( (double)dd.GetWhiteboardDimension().width * m_dScaling + 0.5 );
      int iPagePictureHeight = (int)( (double)dd.GetWhiteboardDimension().height * m_dScaling + 0.5 );
      m_dimPagePicture = new Dimension(iPagePictureWidth, iPagePictureHeight);

      m_dimPages.width = dimSlideOutput.width;
      m_dimPages.height = dimSlideOutput.height;

      // Debug info
      System.out.println("resized Slide size: " + m_dimPages.width + " x " + m_dimPages.height + "  (" + (m_dScaling*100.0) + "%)");
      
      // Set the Telepointer Size, depending on the Slide Size
      // There exist 3 telepointer images in different resolutions
      
      // ??? why is this not used anywhere??
      /*
      int slideSizeMax = (m_dimPagePicture.width > m_dimPagePicture.height) ? m_dimPagePicture.width : m_dimPagePicture.height;
      int telepointerSize = 32;
      if (slideSizeMax < 600)
         telepointerSize = 24;
      if (slideSizeMax < 400)
         telepointerSize = 16;
         
      tpcAll.nTelepointerSize = telepointerSize;
      */

      
      //
      // Resize video / still image / metadata dimensions
      //
      
      // Default: 
      // No Video, no Still Image ==> Metadata table
      //

      m_dimStillImage.width = STANDARD_VIDEO_WIDTH;
      m_dimStillImage.height = STANDARD_VIDEO_HEIGHT; //tpcAll.nMetadataHeight;


      System.out.println("Original Video size: " + m_dimVideo.width + " x " + m_dimVideo.height);


      if (bHasVideoInVideoArea)
      {
         // Video
         // do nothing here

         m_dimStillImage.width = STANDARD_VIDEO_WIDTH;
         m_dimStillImage.height = 0;
      }
      else if (bHasStillImage)
      {
         // Still Image
         Dimension dimImage = GetImageSize(strStillImageName);
         if (dimImage == null || dimImage.width <= 0)
         {
            System.out.println("Unable to load image " + strStillImageName);
            return NO_INPUT_FILE;
         }
         
         m_dimStillImage.width = dimImage.width;
         m_dimStillImage.height = dimImage.height;
         
         
         System.out.println(strStillImageName + ": - old size: " + m_dimStillImage.width + " x " + m_dimStillImage.height);

         // Set a dummy video size
         m_dimVideo.width = (m_dimStillImage.width > nMinVideoWidth) ? m_dimStillImage.width : nMinVideoWidth;
         m_dimVideo.height = 0;
      }
      else
      {
         // Set a dummy video size
         m_dimVideo.width = nMinVideoWidth;
         m_dimVideo.height = 0;
      }
      

      int usedVideoWidth  = bHasStillImage ? m_dimStillImage.width : m_dimVideo.width;
      int usedVideoHeight = bHasStillImage ? m_dimStillImage.height : m_dimVideo.height;

      // Debug info
      System.out.println("Used Video size: " + m_dimVideo.width + " x " + m_dimVideo.height);

      return SUCCESS;
   }
  
   /***************************************************************************/

   /**
    * Write Images and Thumbnails
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int writeImagesAndThumbs()
   {
      int res = SUCCESS;

      System.out.println("");
      
      // Set the number for the Progress Bar
      int nNumOfImages = m_DocInfo.aSlideEntries.length; 
      nNumOfImages += m_DocInfo.aThumbnailEntries.length;
      if (m_DocInfo.aMyButtonAreas != null)
         nNumOfImages += m_DocInfo.aMyButtonAreas.length;
      if (m_DocInfo.aTestingMoveablePictures != null)
         nNumOfImages += m_DocInfo.aTestingMoveablePictures.length;
      m_Display.displayCurrentFile("msg", nNumOfImages, CREATING_IMAGES);

      // int currentProgress = 0; 
      // "currentProgress" is used as a parameter for methods. 
      // --> 'call by reference' needs an array.
      int[] currentProgress = {0};
      int nLayerIndex = 0;

      // Check the global settings from the template (config.xml)
      boolean bDrawBorderLine = true;
      boolean bDrawFrame      = false;
      boolean bDrawBorderLineThumbs = true;
      boolean bDrawFrameThumbs      = false;

      {
         // Note: These are the (global) settings of the template
         // that was last read. In this case it should be the one of 
         // LPLibs.templateReadSettingsFromFile() in startConvert().
         
         String strDrawFrame = LPLibs.templateGetGlobalSetting("drawFrame");
         if (strDrawFrame != null)
         {
            if (strDrawFrame.equalsIgnoreCase("true"))
               bDrawFrame = true;
            else
               bDrawFrame = false;
         }
         String strDrawBorderLine = LPLibs.templateGetGlobalSetting("drawBorderLine");
         if (strDrawBorderLine != null)
         {
            if (strDrawBorderLine.equalsIgnoreCase("true"))
               bDrawBorderLine = true;
            else
               bDrawBorderLine = false;
         }
         String strDrawFrameThumbs = LPLibs.templateGetGlobalSetting("drawFrameThumbs");
         if (strDrawFrameThumbs != null)
         {
            if (strDrawFrameThumbs.equalsIgnoreCase("true"))
               bDrawFrameThumbs = true;
            else
               bDrawFrameThumbs = false;
         }
         String strDrawBorderLineThumbs = LPLibs.templateGetGlobalSetting("drawBorderLineThumbs");
         if (strDrawBorderLineThumbs != null)
         {
            if (strDrawBorderLineThumbs.equalsIgnoreCase("true"))
               bDrawBorderLineThumbs = true;
            else
               bDrawBorderLineThumbs = false;
         }
      }

      // Create a "slides" subdir
//       if (!imc.lecturnity.util.FileUtils.createDir(m_strPathName + File.separatorChar + "slides"))
//          return CREATE_DIR_FAILED;
      if (!imc.lecturnity.util.FileUtils.createDir(m_strTempPath + File.separatorChar + "slides"))
         return CREATE_DIR_FAILED;

      // Note: We do not any longer need to remember that we created a directory; the complete directory
      // is removed while doing the cleanup.
      //      else
      //         m_aStreamingFiles.add(m_strTempPath + File.separatorChar + "slides");

      // Write the Images
      if (res == SUCCESS)
      {
         res = writeImageFiles(
            m_DocInfo.aSlideEntries, nLayerIndex, currentProgress, bDrawFrame, bDrawBorderLine);
      }

      // Write the Thumbnails
      if (res == SUCCESS)
      {
         res = writeThumbnailFiles(currentProgress, bDrawFrameThumbs, bDrawBorderLineThumbs);
      }

      // Write the images for the interactive buttons
      if (res == SUCCESS)
      {
         res = writeInteractiveButtonFiles(currentProgress);
      }

      // Write the images for the moveable pictures
      if (res == SUCCESS)
      {
         res = writeMoveablePictureFiles(currentProgress);
      }
      
      // Update the Progress Bar
      m_Display.displayCurrentFileProgress(currentProgress[0]);

      // Debug Info
      System.out.println("written Slide size: " + m_dimPages.width + " x " + m_dimPages.height);
      //System.out.println("");


      return res; //SUCCESS;
   }
   
   /**
    * Write image files
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int writeImageFiles(SlideEntry[] aSlideEntries, 
                                 int nLayerIndex, int[] currentProgress, 
                                 boolean bDrawFrame, boolean bDrawBorderLine)
   {
      int res = SUCCESS;
      
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      boolean bUseDynamic = ps.GetBoolValue("bRmWmUseDynamic");
      boolean bUseDataReduction = ps.GetBoolValue("bRmWmUseDataReduction");
      
      for (int i = 0; i < aSlideEntries.length; ++i)
      {
         // Update the Progress Bar
         m_Display.displayCurrentFileProgress(currentProgress[0]);
         currentProgress[0]++;

         // Check, if process is cancelled
         if (m_nConversionStatus == CANCELLED)
            continue;

         // Dynamic used?
         // - yes: The timestamp of the image to be written is identical to the 
         //        the timestamp, when the slide appears.
         // - no:  The timestamp of the image to be written is 1 ms before the
         //        next Paging Event (or 1 ms before the end).
         long lTimestamp = 0;
         if (bUseDynamic)
            lTimestamp = aSlideEntries[i].lTimestamp;
         else
         {
            if (i < (aSlideEntries.length - 1))
               lTimestamp = (aSlideEntries[i+1].lTimestamp) - 1;
            else
               lTimestamp = m_lDuration - 1;
         }

         // Check, if this and the next slide have the same image name
         // If so, ignore this slide
         String strImageName = aSlideEntries[i].strImageName;
         if (i < (aSlideEntries.length - 1))
         {
            String strNextImageName = aSlideEntries[i+1].strImageName;
            if (strImageName.equals(strNextImageName))
               continue;
         }
         
         // Get the clipping region from the current event
         Rectangle rect = m_EventQueue.getClip((int)lTimestamp, m_dScaling);

         // Correct the boundaries
         rect = correctBoundaries(rect);
         
         boolean bIsClippingRegion = false;
         if (bUseDataReduction
              && !aSlideEntries[i].bIsNewPage 
              && rect.width > 0 && rect.height > 0)
         {
            // Write clipping region
            nLayerIndex++;
            bIsClippingRegion = true;
         }
         else
         {
            // Write full image
            bIsClippingRegion = false;
         }         


         // Start writing image files
         try
         {
            // Write the slide to a PNG image
            //m_PictureExtracter.setDrawFrame(false);
            //m_PictureExtracter.setDrawBorderLine(true);
            m_PictureExtracter.setDrawFrame(bDrawFrame);
            m_PictureExtracter.setDrawBorderLine(bDrawBorderLine);
            m_PictureExtracter.setShadowThickness(0); // (2)
            m_PictureExtracter.setShadowBackground(new Color(0xFFFFFF));
            m_PictureExtracter.setPictureSize(m_dimPagePicture);

//             String strFilename = m_strPathName + "slides" + File.separatorChar + strImageName + STR_IMAGE_SUFFIX;
            String strRelFileName = "slides" + File.separatorChar + strImageName + STR_IMAGE_SUFFIX;
            String strFilename = m_strTempPath + strRelFileName;
            
            if (bIsClippingRegion)
            {
               m_PictureExtracter.writePngPicture((int)lTimestamp, 
                                                strFilename, 
                                                rect);
            }
            else
            {
               m_PictureExtracter.writePngPicture((int)lTimestamp, 
                                                strFilename); 
            }
            
            System.out.println(strImageName + STR_IMAGE_SUFFIX + " written. (at " + lTimestamp + "ms)");
               
            // Add the output file names to the File Container
            //fileContainer.add(strFilename);

            // Add the relative file name to the slides image container
            m_aJavaWrittenFiles.add(strRelFileName);

            // Feedback: Get the in fact written picture size
            int nPictureWidth = m_PictureExtracter.getPictureSize().width;
            int nPictureHeight = m_PictureExtracter.getPictureSize().height;

            // Put the clipping region to the SlideEntries
            if (bIsClippingRegion)
            {
               aSlideEntries[i].nLayerIndex  = nLayerIndex;
               aSlideEntries[i].nX           = rect.x;
               aSlideEntries[i].nY           = rect.y;
               aSlideEntries[i].nWidth       = rect.width;
               aSlideEntries[i].nHeight      = rect.height;
            }
            else
            {
               aSlideEntries[i].nLayerIndex  = 0;
               aSlideEntries[i].nX           = 0;
               aSlideEntries[i].nY           = 0;
               aSlideEntries[i].nWidth       = nPictureWidth; //m_dimPages.width;
               aSlideEntries[i].nHeight      = nPictureHeight; //m_dimPages.height;

               m_dimPages.width = nPictureWidth;
               m_dimPages.height = nPictureHeight;
            }
            //System.out.println("At " + lTimestamp + "ms -  Rectangle corr.:  Layer: " + nLayerIndex + "  x: " + aSlideEntries[i].nX + "  y: " + aSlideEntries[i].nY + "  w: " + aSlideEntries[i].nWidth + "  h: " + aSlideEntries[i].nHeight);
         }
         catch (Exception e)
         {
            e.printStackTrace();
            System.out.println("Unable to write the slide pictures.");
            STR_ERROR_MESSAGE = e.getMessage();
            return NO_PIC_EXTRACT;
         }
      }

      // Set the layer index for the parser
      m_nMaxLayerIndex = nLayerIndex;

      return res; //SUCCESS;
   }

   /**
    * Write thumbnail files
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int writeThumbnailFiles(int[] currentProgress, 
                                     boolean bDrawFrameThumbs, boolean bDrawBorderLineThumbs)
   {
      int res = SUCCESS;

      for (int i = 0; i < m_DocInfo.aThumbnailEntries.length; ++i)
      {
         // Update the Progress Bar
         m_Display.displayCurrentFileProgress(currentProgress[0]);
         currentProgress[0]++;

         // Check, if process is cancelled
         if (m_nConversionStatus == CANCELLED)
            continue;

         // The timestamp of the thumbnail to be written is 1 ms before the end timestamp
         long lTimestamp = m_DocInfo.aThumbnailEntries[i].lEndTimestamp - 1;
         String strImageName = m_DocInfo.aThumbnailEntries[i].strImageName;

         // Start writing thumbnail files
         try
         {
            m_PictureExtracter.setDrawFrame(bDrawFrameThumbs);
            m_PictureExtracter.setDrawBorderLine(bDrawBorderLineThumbs);
            m_PictureExtracter.setPictureSize(new Dimension(100, 75));
            m_PictureExtracter.setShadowThickness(0); // (1)
            m_PictureExtracter.setShadowBackground(new Color(0xFFFFFF));

//             String strFileName = m_strPathName + "slides" + File.separatorChar + strImageName + STR_IMAGE_SUFFIX;
            String strFileName = m_strTempPath + "slides" + File.separatorChar + strImageName + STR_IMAGE_SUFFIX;
            
            if (m_DocInfo.aThumbnailEntries[i].strClipFileName.equals(""))
            {
               // Write the thumb to a PNG image
               // (The last parametet 'true' includes the drawing of interactive parts)
               m_PictureExtracter.writePngPicture((int)lTimestamp, 
                                                strFileName, true);

            }
            else
            {
               // Write the first frame of the clip to a PNG image
               String strAviFileName = m_DocInfo.aThumbnailEntries[i].strClipFileName;
               try
               {
                  m_PictureExtracter.writeAviFramePngPicture(m_strInputPathName + strAviFileName, 
                                                           strFileName);
               }
               catch (Exception e)
               {
                  // TODO:
                  // Warning message & copy standard SG clip thumbnail image
               }
            }
            System.out.println(strImageName + STR_IMAGE_SUFFIX + " written.");

            // Add the output file names to the File Container
            //fileContainer.add(strFileName);

            // Add the relative file name to the slides images file container
            m_aJavaWrittenFiles.add("slides" + File.separatorChar + strImageName + STR_IMAGE_SUFFIX);
         }
         catch (Exception e)
         {
            e.printStackTrace();
            System.out.println("Unable to write the thumbnail pictures.");
            STR_ERROR_MESSAGE = e.getMessage();
            return NO_PIC_EXTRACT;
         }
      }

      return res; //SUCCESS;
   }

   /**
    * Write interactive button files
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int writeInteractiveButtonFiles(int[] currentProgress)
   {
      int res = SUCCESS;

      if (m_DocInfo.aMyButtonAreas != null)
      {
         // Write the images for the interactive buttons
         for (int i = 0; (res == SUCCESS) && (i < m_DocInfo.aMyButtonAreas.length); ++i)
         {
            // Update the Progress Bar
            m_Display.displayCurrentFileProgress(currentProgress[0]);
            currentProgress[0]++;
            
            ButtonArea buttonArea = m_DocInfo.aMyButtonAreas[i].m_buttonArea;

            String strFileName = "slides" + File.separatorChar 
               + "Button_" + i + "_";
            String strFileId = "Button_" + i + "_";
            
            res = writeButtonPictureFiles(buttonArea, m_strTempPath, strFileName, strFileId);

            m_DocInfo.aMyButtonAreas[i].m_strId = strFileId;
         }
      }
      
      // Interactive "OK" button for feedbacks
      if ((res == SUCCESS) && (m_DocInfo.aQuestionnaires != null))
      {
         // We need only one unique button for all feedbacks
         boolean bOkButtonFound = false;
         
         ButtonArea buttonArea = null;
         
         Feedback fb = null;
         for (int i = 0; i < m_DocInfo.aQuestionnaires.length; ++i)
         {
            // 1. Look for Questionnaire feedbacks
            Questionnaire qq = m_DocInfo.aQuestionnaires[i];
            fb = qq.GetFeedback(Feedback.FEEDBACK_TYPE_QQ_PASSED);
            if (! ((fb != null) && (!fb.GetComponents().isEmpty()) && (fb.IsEnabled())) )
               fb = qq.GetFeedback(Feedback.FEEDBACK_TYPE_QQ_FAILED);
            if (! ((fb != null) && (!fb.GetComponents().isEmpty()) && (fb.IsEnabled())) )
               fb = qq.GetFeedback(Feedback.FEEDBACK_TYPE_QQ_EVAL);
            
            if ((fb != null) && (!fb.GetComponents().isEmpty()) && (fb.IsEnabled()))
            {
               buttonArea = getButtonAreaFromFeedback(fb);
               if (buttonArea != null)
               {
                  bOkButtonFound = true;
                  break;
               }
            }
            
            // 2. If not successful, look for Question feedbacks
            Question[] aQuestions = qq.GetQuestions();
            if ((!bOkButtonFound) && (aQuestions != null) && (aQuestions.length > 0))
            {
               for (int k = 0; k < aQuestions.length; ++k)
               {
                  Question q = aQuestions[k];
                  fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_Q_CORRECT, true);
                  if (! ((fb != null) && (!fb.GetComponents().isEmpty()) && (fb.IsEnabled())) )
                     fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_Q_WRONG, true);
                  if (! ((fb != null) && (!fb.GetComponents().isEmpty()) && (fb.IsEnabled())) )
                     fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_Q_REPEAT, true);
                  if (! ((fb != null) && (!fb.GetComponents().isEmpty()) && (fb.IsEnabled())) )
                     fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_Q_TRIES, true);
                  if (! ((fb != null) && (!fb.GetComponents().isEmpty()) && (fb.IsEnabled())) )
                     fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_Q_TIME, true);

                  if ((fb != null) && (!fb.GetComponents().isEmpty()) && (fb.IsEnabled()))
                  {
                     buttonArea = getButtonAreaFromFeedback(fb);
                     if (buttonArea != null)
                     {
                        bOkButtonFound = true;
                        break;
                     }
                  }
               }// end k-loop
            }
         }// end i-loop
         
         if (bOkButtonFound)
         {
            String strFileName = "slides" + File.separatorChar 
               + "Button_Ok_";
            String strFileId = "Button_Ok_";
            
            res = writeButtonPictureFiles(buttonArea, m_strTempPath, strFileName, strFileId);
         }
      }

      return res; //SUCCESS;
   }

   /**
    */
   protected ButtonArea getButtonAreaFromFeedback(Feedback fb)
   {
      ButtonArea buttonArea = null;

      ArrayList alComponents = fb.GetComponents();
      if (!alComponents.isEmpty())
      {
         for (int i = 0; i < alComponents.size(); ++i)
         {
            VisualComponent vc = (VisualComponent)alComponents.get(i);
            if (vc instanceof ButtonArea)
            {
               buttonArea = (ButtonArea)vc;
               break;
            }
         }
      }
      
      return buttonArea;
   }

   /**
    * Write (interactive) button picture files
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int writeButtonPictureFiles(ButtonArea buttonArea, 
                                         String strTempPath, String strFileName, String strFileId)
   {
      int res = SUCCESS;

      try
      {
         Rectangle rect = buttonArea.getClip(m_dScaling);
         // This is maybe more precise, but there are drawing problems 
         // with the border of text buttons: 
//         Rectangle rect = buttonArea.GetActivityArea();
//         // Consider scale factor
//         rect.x = (int)(m_dScaling * rect.x + 0.5f);
//         rect.y = (int)(m_dScaling * rect.y + 0.5f);
//         rect.width = (int)(m_dScaling * rect.width + 0.5f);
//         rect.height = (int)(m_dScaling * rect.height + 0.5f);

         // Correct Boundaries
         rect = correctBoundaries(rect);

         //boolean bUsePaletteColors = true;
         // The button images from Alexander Jäger have semi transparent colors 
         // This leads to display problems with IE 6 and older browsers
         // --> Write 2 types of pictures: 32-bit ARGB and 256 palette colors

         m_PictureExtracter.setPictureSize(m_dimPagePicture);
         m_PictureExtracter.setDrawFrame(false);
         m_PictureExtracter.setDrawBorderLine(false);
         m_PictureExtracter.setShadowThickness(0);
         m_PictureExtracter.setBackground(new Color(0xFFFFFF));

         // "Normal" state
         buttonArea.SetActive(true);
         buttonArea.Visualize(false, false);
         // 32-bit ARGB
         String strNormal = strFileName + "normal" + STR_IMAGE_SUFFIX;
         String strTempNormal = strTempPath + strNormal;
         m_PictureExtracter.writePngPicture(buttonArea, strTempNormal, rect, false);
         System.out.println(strFileId + "normal" + STR_IMAGE_SUFFIX + " written.");
         // Add the relative file name to the slides images file container
         m_aJavaWrittenFiles.add(strNormal);
         // 256 color palette
         String strNormal256 = strFileName + "normal256" + STR_IMAGE_SUFFIX;
         String strTempNormal256 = strTempPath + strNormal256;
         m_PictureExtracter.writePngPicture(buttonArea, strTempNormal256, rect, true);
         System.out.println(strFileId + "normal256" + STR_IMAGE_SUFFIX + " written.");
         // Add the relative file name to the slides images file container
         m_aJavaWrittenFiles.add(strNormal256);
         
         // "Over" state
         buttonArea.SetActive(true);
         buttonArea.Visualize(true, false);
         // 32-bit ARGB
         String strOver = strFileName + "over" + STR_IMAGE_SUFFIX;
         String strTempOver = strTempPath + strOver;
         m_PictureExtracter.writePngPicture(buttonArea, strTempOver, rect, false);
         System.out.println(strFileId + "over" + STR_IMAGE_SUFFIX + " written.");
         // Add the relative file name to the slides images file container
         m_aJavaWrittenFiles.add(strOver);
         // 256 color palette
         String strOver256 = strFileName + "over256" + STR_IMAGE_SUFFIX;
         String strTempOver256 = strTempPath + strOver256;
         m_PictureExtracter.writePngPicture(buttonArea, strTempOver256, rect, true);
         System.out.println(strFileId + "over256" + STR_IMAGE_SUFFIX + " written.");
         // Add the relative file name to the slides images file container
         m_aJavaWrittenFiles.add(strOver256);
         
         // "Pressed" state
         buttonArea.SetActive(true);
         buttonArea.Visualize(true, true);
         // 32-bit ARGB
         String strPressed = strFileName + "pressed" + STR_IMAGE_SUFFIX;
         String strTempPressed = strTempPath + strPressed;
         m_PictureExtracter.writePngPicture(buttonArea, strTempPressed, rect, false);
         System.out.println(strFileId + "pressed" + STR_IMAGE_SUFFIX + " written.");
         // Add the relative file name to the slides images file container
         m_aJavaWrittenFiles.add(strPressed);
         // 256 color palette
         String strPressed256 = strFileName + "pressed256" + STR_IMAGE_SUFFIX;
         String strTempPressed256 = strTempPath + strPressed256;
         m_PictureExtracter.writePngPicture(buttonArea, strTempPressed256, rect, true);
         System.out.println(strFileId + "pressed256" + STR_IMAGE_SUFFIX + " written.");
         // Add the relative file name to the slides images file container
         m_aJavaWrittenFiles.add(strPressed256);
         
         // "inactive" state
         buttonArea.SetActive(false);
         buttonArea.Visualize(false, false);
         // 32-bit ARGB
         String strInactive = strFileName + "inactive" + STR_IMAGE_SUFFIX;
         String strTempInactive = strTempPath + strInactive;
         m_PictureExtracter.writePngPicture(buttonArea, strTempInactive, rect, false);
         System.out.println(strFileId + "inactive" + STR_IMAGE_SUFFIX + " written.");
         // Add the relative file name to the slides images file container
         m_aJavaWrittenFiles.add(strInactive);
         // 256 color palette
         String strInactive256 = strFileName + "inactive256" + STR_IMAGE_SUFFIX;
         String strTempInactive256 = strTempPath + strInactive256;
         m_PictureExtracter.writePngPicture(buttonArea, strTempInactive256, rect, true);
         System.out.println(strFileId + "inactive256" + STR_IMAGE_SUFFIX + " written.");
         // Add the relative file name to the slides images file container
         m_aJavaWrittenFiles.add(strInactive256);
      }
      catch (Exception e)
      {
         e.printStackTrace();
         System.out.println("Unable to write the interactive button pictures.");
         STR_ERROR_MESSAGE = e.getMessage();
         return NO_PIC_EXTRACT;
      }

      return res; //SUCCESS;
   }

   /**
    * Write moveable picture files
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int writeMoveablePictureFiles(int[] currentProgress)
   {
      int res = SUCCESS;

      if (m_DocInfo.aTestingMoveablePictures != null)
      {
         // Write the images for the interactive buttons
         for (int i = 0; i < m_DocInfo.aTestingMoveablePictures.length; ++i)
         {
            // Update the Progress Bar
            m_Display.displayCurrentFileProgress(currentProgress[0]);
            currentProgress[0]++;
            
            try
            {
               Picture picture = m_DocInfo.aTestingMoveablePictures[i];

               Rectangle rect = picture.GetOrigImageArea();
               rect.x = (int)(0.5 + (m_dScaling * rect.x));
               rect.y = (int)(0.5 + (m_dScaling * rect.y));
               rect.width = (int)(0.5 + (m_dScaling * rect.width));
               rect.height = (int)(0.5 + (m_dScaling * rect.height));
               // Correct Boundaries
               rect = correctBoundaries(rect);
               System.out.println("writeMoveablePictureFiles(): " + rect.x + "," + rect.y + " " + rect.width + "x" + rect.height);

               m_PictureExtracter.setPictureSize(m_dimPagePicture);
               m_PictureExtracter.setDrawFrame(false);
               m_PictureExtracter.setDrawBorderLine(false);
               m_PictureExtracter.setShadowThickness(0);
               m_PictureExtracter.setShadowBackground(new Color(0xFFFFFF));

               // Bugfix 2558: independent from the input format (png, jpg, ...) always a png is written
               String strImageName = picture.GetFileName();
               int lastDot = strImageName.lastIndexOf('.');
               strImageName = strImageName.substring(0, lastDot);
               strImageName += ("_" + i);
//               String strImageName256 = strImageName + "256.png";
               strImageName += ".png";
               // 32-bit ARGB
               String strTarget = "slides" + File.separatorChar + strImageName;
               String strTempTarget = m_strTempPath + strTarget;
               m_PictureExtracter.writePngPicture(picture, strTempTarget, rect, false);
               System.out.println(strImageName + " written.");
               // Add the relative file name to the slides images file container
               m_aJavaWrittenFiles.add(strTarget);
//               // 256 color palette
//               String strTarget256 = "slides" + File.separatorChar + strImageName256;
//               String strTempTarget256 = m_strTempPath + strTarget256;
//               m_PictureExtracter.writePngPicture(picture, strTempTarget256, rect, true);
//               System.out.println(strImageName256 + " written.");
//               // Add the relative file name to the slides images file container
//               m_aJavaWrittenFiles.add(strTarget256);
            }
            catch (Exception e)
            {
               e.printStackTrace();
               System.out.println("Unable to write the moveable pictures.");
               STR_ERROR_MESSAGE = e.getMessage();
               return NO_PIC_EXTRACT;
            }
         }
      }

      return res; //SUCCESS;
   }

   /***************************************************************************/

   protected Rectangle correctBoundaries(Rectangle rect)
   {
      if (rect.width < 0)
         rect.width = 0;
      if (rect.height < 0)
         rect.height = 0;

      int right = rect.x + rect.width;
      int bottom = rect.y + rect.height;
         
      if (rect.x < 0)
         rect.x = 0;
      if (rect.y < 0)
         rect.y = 0;
      if (right > m_dimPages.width)
         right = m_dimPages.width;
      if (bottom > m_dimPages.height)
         bottom = m_dimPages.height;

      // A minimum size of 16x16 for correct display in the browser 
      if (right - rect.x < 16)
      {
         if (rect.x > (right - rect.x))
            rect.x -= (16 - (right - rect.x));
         else
            right += 16 - (right - rect.x);
      }
      if (bottom - rect.y < 16)
      {
         if (rect.y > (bottom - rect.y))
            rect.y -= 16 - (bottom - rect.y);
         else
            bottom += 16 - (bottom - rect.y);
      }
         
      rect.width = right - rect.x;
      rect.height = bottom - rect.y;

      return rect;
   }
    
   /***************************************************************************/

   /**
    * This routine sets the keywords for the (HTML-)Parser
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int setKeywordsForParser()
   {
      DocumentData dd = m_pwData.GetDocumentData();
      
      boolean bIsDenver = dd.IsDenver();
      Metadata metadata = dd.GetMetadata();
      
      if (!bIsDenver)
      {
         checkForStillimageAndMetadataFlags();
      }

      return SUCCESS;
   }
   
   /***************************************************************************/

   /**
    * Check for the flags showStillimage and showMetadata
    */
   public void checkForStillimageAndMetadataFlags()
   {
      // There are only some special cases, if we have to show still image or metadata
      //
      // hasVideoFile videoIsStillImage containsNormalVideo containsMultipleVideos separateClipsFromVideo | Still Image | Metadata
      // -------------------------------------------------------------------------------------------------+-------------+---------
      //       +              +                   -                    +                      +           |      +      |         
      //       +              +                   -                    -                     (-)          |      +      |         
      // -------------------------------------------------------------------------------------------------+-------------+---------
      //       +              -                   -                    +                      +           |             |    x    
      //       -             (-)                 (-)                  (-)                    (-)          |             |    x    
  
      DocumentData dd = m_pwData.GetDocumentData();
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      boolean bClipsSeparated = dd.HasClips() && ps.GetBoolValue("bShowClipsOnSlides");
      
      boolean bHasStillImageInVideoAreaOnly = dd.HasStillImage() && bClipsSeparated;
      boolean bHasStillImageAndNoClips = dd.HasStillImage() && !dd.HasClips();
      boolean bHasSeparatedClipsOnly = !dd.HasStillImage() && bClipsSeparated;
      boolean bHasNoVideoAtAll = !dd.HasStillImage() && !dd.HasNormalVideo() && !dd.HasClips();
      
      
      //if (bHasStillImageInVideoAreaOnly || bHasStillImageAndNoClips)
      //   tpcAll.bShowStillimage = true;
      
      //if (bHasSeparatedClipsOnly || bHasNoVideoAtAll)
      //   tpcAll.bShowMetadata = true;
      // ??? where could this have been used??
   }

   /***************************************************************************/

   protected int writeHtmlFilesNative()
   {
      int res = SUCCESS;
      
      boolean bIsDenver = m_pwData.GetDocumentData().IsDenver();
      boolean bExportReal = m_pwData.GetProfiledSettings().GetIntValue("iExportType")
         == PublisherWizardData.EXPORT_TYPE_REAL;         
      

      TemplateSnippetCreator tsc = new TemplateSnippetCreator();
      Dimension dimPages = new Dimension(m_dimPages.width, m_dimPages.height);
      Dimension dimVideo = new Dimension(m_dimVideo.width, m_dimVideo.height);
      Dimension dimStillimage = new Dimension(m_dimStillImage.width, m_dimStillImage.height);
      tsc.createSnippets(m_pwData, m_DocInfo, m_dScaling, 
                         m_lDuration, m_nMaxLayerIndex, dimPages,
                         dimVideo, dimStillimage, m_nNumOfSlides, m_nNumOfEvents);
      Snippet[] snippets = tsc.getSnippets();

      for (int i=0; i<snippets.length; ++i)
         LPLibs.templateAddKeyword(snippets[i].keyword, snippets[i].replace);

      if (!bIsDenver)
         LPLibs.templateSetDocumentType("normalDocument");
      else
         LPLibs.templateSetDocumentType("standaloneDocument");

      if (bExportReal)
         LPLibs.templateSetTargetFormat("realMedia");
      else
         LPLibs.templateSetTargetFormat("windowsMedia");

      //parserSetTargetPath(m_strPathName);

      res = LPLibs.templateDoParse(null);

      return res;
   }
  
   /***************************************************************************/

   /**
    * Copy a file from the source directory to the target directory.
    *
    *    @param String inFile: the input file name, including the whole path.
    *    @param String outFile: the output file name, including the whole path.
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int copyFile(String fileIn, String fileOut)
   {
      File inFile = new File(fileIn);
      File outFile = new File(fileOut);

      if (inFile.equals(outFile))
      {
         System.out.println("Warning: " + fileIn + " can't be copied to itself!");

         return SUCCESS;
      }

      try
      {
         File outputFile = new File(fileOut);

         byte[] buffer = new byte[16384];
      
         InputStream inStream 
            = new BufferedInputStream(new FileInputStream(fileIn));

         OutputStream outStream
            = new BufferedOutputStream(new FileOutputStream(outputFile));

         int bytesRead = 1;
         int totalRead = 0;
         
         while (bytesRead != -1)
         {
            bytesRead = inStream.read(buffer, 0, buffer.length);
            if (bytesRead != -1)
            {
               outStream.write(buffer, 0, bytesRead);
            }
         }

         inStream.close();
         outStream.flush();
         outStream.close();

         System.out.println(new File(fileOut).getName()+ " copied.");
         
         // Add the output file name to the File Container
         // fileContainer.add(fileOut);      
         
         return SUCCESS;
      }      
      catch (IOException e)
      {
         e.printStackTrace();
         System.out.println("Error while copying " + fileIn);
         STR_ERROR_MESSAGE = e.getMessage();
         return NO_INPUT_FILE;
      }
   }
   
   /***************************************************************************/

   /**
    * Copy (Move) a file from the source directory to the target directory and remove the old one.
    *
    *    @param String inFile: the input file name, including the whole path.
    *    @param String outFile: the output file name, including the whole path.
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int moveFile(String fileIn, String fileOut)
   {
      int res = SUCCESS;
      
      res = copyFile(fileIn, fileOut);
      
      File inFile = new File(fileIn);
      File outFile = new File(fileOut);

      if ((res == SUCCESS) && (inFile.equals(outFile)))
      {
         System.out.println("Warning: " + fileIn + " can't be moved to itself!");

         return SUCCESS;
      }

      if (res == SUCCESS)
      {
         try
         {
            inFile.delete();
         }
         catch (SecurityException e)
         {
            e.printStackTrace();
            System.out.println("Error while removing " + fileIn);
            STR_ERROR_MESSAGE = e.getMessage();
            return NO_INPUT_FILE;
         }
      }
      
      return res;
   }
  
   /***************************************************************************/

   /**
    * This routine deletes the previous written files after cancelling the process
    */
   protected void deleteNewBuildFiles()
   {
      //
      // new style: as of 1.6.1.p1 most files (except *.xm) are written natively
      //
      // parserDeleteAllWrittenFiles();
      LPLibs.templateCleanUp();
      System.out.println("Removed natively written files.");
         
      //
      // old style: every file written with Java is recorded in fileContainer
      //
      File file;

      String[] fileList = getStreamingFileList();
      
      for (int i = fileList.length-1; i>=0; --i)
      {
         // as directories are first it is important to iterate in reverse order
         
         file = new File(fileList[i]);
         if (file.exists())
         {
            file.delete();
            System.out.println("Removing file " + new File(fileList[i]).getName());
         }
      }

      // Remove temporary audio file, if any
      file = new File(m_strPathName + m_strFileName + "_aif.tmp");
      if (file.exists())
         file.delete();

   }
   
   /***************************************************************************/

   protected int processScorm()
   {
      boolean bScormEnabled = m_pwData.GetProfiledSettings().GetBoolValue("bScormEnabled");
      
      if (!bScormEnabled)
         return SUCCESS;

      int res = S_OK;
      
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      boolean bIsLocalReplay = 
         (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL);
      Metadata metadata = m_pwData.GetDocumentData().GetMetadata();
      String strScormSettings = ps.GetStringValue("strScormSettings");
      
      int iReplayType = ps.GetIntValue("iReplayType");
      int iServerType = ps.GetIntValue("iServerType");

      VisualComponent[] aVisualComponents = m_ObjectQueue.getVisualComponents();

      // Ok, let's SCORM!
      res = LPLibs.scormInit();
      
      if (S_OK == res && strScormSettings != null && strScormSettings.length() > 0)
         res = LPLibs.scormUseSettings(strScormSettings);
      
      if (S_OK == res)
      {
         TemplateSnippetCreator tsc = new TemplateSnippetCreator();
         Dimension dimPages = new Dimension(m_dimPages.width, m_dimPages.height);
         Dimension dimVideo = new Dimension(m_dimVideo.width, m_dimVideo.height);
         Dimension dimStillimage = new Dimension(m_dimStillImage.width, m_dimStillImage.height);   
         tsc.createSnippets(m_pwData, m_DocInfo, m_dScaling, 
                            m_lDuration, m_nMaxLayerIndex, dimPages,
                            dimVideo, dimStillimage, m_nNumOfSlides, m_nNumOfEvents);
         Snippet[] snippets = tsc.getSnippets();
         for (int i=0; i<snippets.length; ++i)
            LPLibs.scormAddGeneralKeyword(snippets[i].keyword, snippets[i].replace);
      }

      if (S_OK == res)
      {
         // Add Metadata keywords
         if (metadata != null)
         {
            String[] keywords = metadata.keywords;
            if (keywords != null)
            {
               for (int i=0; i<keywords.length; ++i)
                  LPLibs.scormAddMetadataKeyword(keywords[i]);
            }

            // Add thumbnail titles and thumbnail keywords as full text
            if (metadata.thumbnails != null)
            {
               StringBuffer sbThumbnailData = new StringBuffer();
               
               for (int i=0; i<metadata.thumbnails.length; ++i)
               {
                  // add the title 
                  sbThumbnailData.append(metadata.thumbnails[i].title);
                  sbThumbnailData.append(" ");

                  // add the keywords
                  for (int k=0; k<metadata.thumbnails[i].keywords.length; ++k)
                  {
                     sbThumbnailData.append(metadata.thumbnails[i].keywords[k]);
                     sbThumbnailData.append(" ");
                  }
               }
               LPLibs.scormAddFullTextString(sbThumbnailData.toString());
            }
         }

         // Add full text, if item is selected
         if (null != aVisualComponents && LPLibs.scormGetFullTextAsKeywords())
         {
            for (int i=0; i<aVisualComponents.length; ++i)
            {
               if (aVisualComponents[i] instanceof Text)
               {
                  Text textObject = 
                     (Text) aVisualComponents[i];
                  LPLibs.scormAddFullTextString(textObject.getAllContent());
               }
            }
         }
      }

      if (S_OK == res)
      {
         String indexFileName = LPLibs.templateGetIndexFile();
         LPLibs.scormSetIndexFileName(m_strTempPath + indexFileName, indexFileName);
         int nCount = LPLibs.templateGetFileCount();
         for (int i=0; i<nCount; ++i)
         {
            String resourceFileName = LPLibs.templateGetFile(i);
            if (resourceFileName.endsWith(".smi"))
            {
               // The SMIL file is created by the template library, but does belong to the
               // streaming files. Move it to the target path and add it to the
               // list of streaming files, so that it either can be added to the SCORM
               // package (local replay case) or copied to the streaming server (streaming
               // replay case).
               moveFile(m_strTempPath + resourceFileName, m_strPathName + resourceFileName);
               m_aStreamingFiles.add(m_strPathName + resourceFileName);
            }
            else
            {
               LPLibs.scormAddResourceFile(m_strTempPath + resourceFileName, resourceFileName);
            }
         }
         
         String[] imageList = new String[m_aJavaWrittenFiles.size()];
         m_aJavaWrittenFiles.toArray(imageList);
         for (int i=0; i<imageList.length; ++i)
         {
            LPLibs.scormAddResourceFile(m_strTempPath + imageList[i], imageList[i]);
         }
      }

      if (S_OK == res)
      {
         // Do we have to put in the streaming files into the package?
         ///if (iReplayType == PublisherWizardData.REPLAY_LOCAL)
         boolean bIsLocal = (iReplayType == PublisherWizardData.REPLAY_LOCAL);
         // File server/Web server are allowed, too
         boolean bIsAllowedServer = ( (iReplayType == PublisherWizardData.REPLAY_SERVER) 
                                     && ((iServerType == PublisherWizardData.SRV_FILE_SERVER) 
                                         || (iServerType == PublisherWizardData.SRV_WEB_SERVER)) );
         if (bIsLocal || bIsAllowedServer)
         {
            // Yes. If we're in strict SCORM mode, the replay type
            // will always be LOCAL, so we don't have to check that again
            String[] m_aStreamingFiles = getStreamingFileList();

            for (int i=0; i<m_aStreamingFiles.length; ++i)
            {
               File file = new File(m_aStreamingFiles[i]);
               LPLibs.scormAddResourceFile(m_aStreamingFiles[i], file.getName());
            }
         }
      }

      if (S_OK == res)
      {
         String strFileName = m_strFileName;
         strFileName = CheckTargetZipExists(strFileName, m_pwData, (WizardPanel)m_Display);
         String zipFileName = m_strPathName + strFileName;
         System.out.println("Setting target file name to: " + zipFileName);
         m_strZipFileName = zipFileName + ".zip";
         LPLibs.scormSetTargetFileName(strFileName);

         final Thread thread = new Thread() 
            {
               public void run()
               {
                  while (!StreamingMediaConverter.this.m_bStopThread)
                  {
                     StreamingMediaConverter.this.m_Display.displayCurrentFileProgress(LPLibs.scormGetProgress());
                     try
                     {
                        sleep(250);
                     }
                     catch (InterruptedException e)
                     {
                        // Ignore
                     }
                  }
               }
            };

         m_bStopThread = false;
         thread.start();
         res = LPLibs.scormCreatePackage(null);
         m_bStopThread = true;
      }

      if (S_OK == res)
      {
         File targetFile = new File(LPLibs.scormGetTargetFilePath());
         copyFile(targetFile.toString(), m_strPathName + targetFile.getName());
      }

      LPLibs.scormCleanUp();
      // If we're in local mode, remove the streaming files from the target dir
      ///if (iReplayType == PublisherWizardData.REPLAY_LOCAL)
      boolean bIsLocal = (iReplayType == PublisherWizardData.REPLAY_LOCAL);
      // File server/Web server are allowed, too
      boolean bIsAllowedServer = ( (iReplayType == PublisherWizardData.REPLAY_SERVER) 
                                  && ((iServerType == PublisherWizardData.SRV_FILE_SERVER) 
                                      || (iServerType == PublisherWizardData.SRV_WEB_SERVER)) );
      if(bIsLocal || bIsAllowedServer)
      {
         String[] m_aStreamingFiles = getStreamingFileList();
         for (int i=0; i<m_aStreamingFiles.length; ++i)
         {
            File file = new File(m_aStreamingFiles[i]);
            file.delete();
         }
      }

      return res;
   }

   /***************************************************************************/


   public static String CheckTargetZipExists(String strFileName, PublisherWizardData pwData, 
                                             WizardPanel pnlParent)
   {
      return CheckTargetZipExists(strFileName, pwData, pnlParent, false);
   }
                                                
   public static String CheckTargetZipExists(String strFileName, PublisherWizardData pwData, 
                                             WizardPanel pnlParent, boolean bZipIsLzp)
   {
      String strNewFileName = new String(strFileName);

      String strTargetDirExtern = "";
      String strSuffix = ".zip";
      if (bZipIsLzp)
         strSuffix = ".lzp";
      int idxT = -1;
      ProfiledSettings ps = pwData.GetProfiledSettings();
      boolean bIsProfileMode = pwData.m_bIsProfileMode;
      boolean bDoUpload = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                           && (ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_UPLOAD) );
      boolean bUseNetworkDrive = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                                  &&(ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_NETWORK_DRIVE) );
      if (bUseNetworkDrive)
      {
         strTargetDirExtern = ps.GetStringValue("strTargetDirHttpServer");
         if (!strTargetDirExtern.endsWith(File.separator))
            strTargetDirExtern += File.separator;

         idxT = FileUtils.getIndexOfNextEmptyFileName(strTargetDirExtern + strFileName + strSuffix, "_t");
         if (bIsProfileMode && idxT > 1)
            strNewFileName = strFileName + "_t" + idxT;
      }
      else if (bDoUpload)
      {
         strTargetDirExtern = ps.GetStringValue("strUploadFirstTargetDir");
         if (strTargetDirExtern.length() > 0)
         {
            if (strTargetDirExtern.charAt(strTargetDirExtern.length()-1) != '/')
               strTargetDirExtern += '/';
         }

         idxT = UploaderAccess.GetIndexOfNextEmptyFileName(strTargetDirExtern + strFileName + strSuffix, "_t", 
                                                           true, ps, pnlParent);
         if (bIsProfileMode && idxT > 1)
            strNewFileName = strFileName + "_t" + idxT;
      }

      return strNewFileName;
   }

   protected int CheckStreamingFileWriteable(String strOutputFile)
   {
      try
      {
         // Open Output File
         PrintWriter fileOut =
            new PrintWriter(new BufferedWriter(new FileWriter(strOutputFile)));

         // Close Output File
         fileOut.flush();
         fileOut.close();
      }
      catch (IOException e)
      {
         e.printStackTrace();
         System.out.println("Unable to open " + strOutputFile);
         return (this instanceof WindowsMediaConverter) ? ASX_FAILED : RPM_FAILED;
      }
            
      return SUCCESS;
   }
   
   protected int CopyStreamingServerFiles(ArrayList aFilenames, String strTargetPath)
   {
      int res = SUCCESS;
      
      // Reset progress bar
      int iCurrentProgress = 0;
      m_Display.displayCurrentFileProgress(iCurrentProgress);

      for (int i=0; i<aFilenames.size() && res == SUCCESS; ++i)
      {
         String strInputFile = (String)aFilenames.get(i);
         String strFilename = new File(strInputFile).getName();
         String strOutputFile = strTargetPath + strFilename;
         
         // Copy the media file to the "server" directory
         ///res = moveFile(strInputFile, strOutputFile);
         res = copyFile(strInputFile, strOutputFile);
         
         // Update progress bar
         iCurrentProgress++;
         m_Display.displayCurrentFileProgress(iCurrentProgress);
      }

      if (res == SUCCESS)
         return SUCCESS;
      else
         return (this instanceof WindowsMediaConverter) ? NO_WMSERVER_COPY : NO_REALSERVER_COPY;
   }
   
   /**
    * Read the video clip info and set filenames and timestamps to the Converter
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int HandleVideoClipsSeparated(VideoClipInfo[] aClips, String strSourcePath)
   {
     int size = aClips.length;
      String[] clipFilenames = new String[size];
      long[] clipTimestamps = new long[size];
      
      // Fill the arrays
      for (int i = 0; i < size; ++i)
      {
         // Add the input path to the filenames
         clipFilenames[i] = strSourcePath + aClips[i].clipFileName;
         clipTimestamps[i] = aClips[i].startTimeMs;
      }
      
      return ReadAndSetVideoClips(clipFilenames, clipTimestamps);
   }
      
   protected String GetOutputPrefix()
   {
      String strTargetPath = GetOutputPath();
      
      return (strTargetPath + m_pwData.GetProfiledSettings().GetStringValue("strTargetPathlessDocName"));
   }
   
   protected String GetOutputPath()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      boolean bIsLocal = (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL);
      boolean bDoUpload = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                           && (ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_UPLOAD) );
      boolean bUseNetworkDrive = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                                  &&(ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_NETWORK_DRIVE) );

      String strTargetPath = ps.GetPathValue("strTargetDirHttpServer");
      if (bIsLocal || bDoUpload || bUseNetworkDrive)
         strTargetPath = ps.GetPathValue("strTargetDirLocal");

      return strTargetPath;
   }
   
   protected String GetFccCodecString(int iFccHandler)
   {
      char[] fcc = new char[4];
      fcc[0] = (char)( (iFccHandler)     &~(~0<<8) );
      fcc[1] = (char)( (iFccHandler>>8)  &~(~0<<8) );
      fcc[2] = (char)( (iFccHandler>>16) &~(~0<<8) );
      fcc[3] = (char)( (iFccHandler>>24) &~(~0<<8) );
      
      return new String(fcc);
   }
   
   private String GetNumberString(long lValue, int iMinLength)
   {
      String strNumber = lValue+"";
      if (lValue >= 0 && strNumber.length() < iMinLength)
      {
         for (int i=strNumber.length(); i<iMinLength; ++i)
            strNumber = "0" + strNumber; // prepend with missing zeros
      }
      return strNumber;
   }
   
   private Dimension GetImageSize(String strImagePath)
   {
      ImageIcon icon = new ImageIcon(strImagePath);
      Dimension dimImage = new Dimension(icon.getIconWidth(), icon.getIconHeight());
      return dimImage;
   }
}
