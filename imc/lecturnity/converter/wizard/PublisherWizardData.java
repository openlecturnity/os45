package imc.lecturnity.converter.wizard;

import imc.lecturnity.converter.ProgressManager;
import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.DptCommunicator;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.StreamingMediaConverter;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.util.wizards.WizardData;
import imc.lecturnity.util.wizards.WizardPanel;

import java.awt.Dimension;
import java.awt.Insets;
import java.io.File;
import javax.swing.ImageIcon;

public class PublisherWizardData extends WizardData
{
   // The export types for the presentation
   public static final int EXPORT_TYPE_UNDEFINED      = -1;
   public static final int EXPORT_TYPE_LECTURNITY     = 0;
   public static final int EXPORT_TYPE_REAL           = 1;
   public static final int EXPORT_TYPE_WMT            = 2;
   public static final int EXPORT_TYPE_FLASH          = 3;
   public static final int EXPORT_TYPE_VIDEO          = 4;
   
   // The document type
   public static final int DOCUMENT_TYPE_UNKNOWN                        = 0;
   public static final int DOCUMENT_TYPE_AUDIO_PAGES                    = 1;
   public static final int DOCUMENT_TYPE_AUDIO_VIDEO_PAGES              = 2;
   public static final int DOCUMENT_TYPE_AUDIO_CLIPS_PAGES              = 3;
   public static final int DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES        = 4;
   public static final int DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES         = 5;
   public static final int DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES   = 6;
   public static final int DOCUMENT_TYPE_DENVER                         = 7;

   // Visibility of GUI elements
   public static final int GUI_ELEMENT_ACTIVE         = 0;
   public static final int GUI_ELEMENT_DISABLED       = 1;
   public static final int GUI_ELEMENT_HIDDEN         = 2;
   
   // Replay type (RM, WM, Flash)
   public static final int REPLAY_LOCAL               = 0;
   public static final int REPLAY_HTTP                = 1; // obsolete?
   public static final int REPLAY_SERVER              = 2;

   // Server type (all formats)
   public static final int SRV_FILE_SERVER            = 0;
   public static final int SRV_WEB_SERVER             = 1;
   public static final int SRV_STREAMING_SERVER       = 2;
   public static final int SRV_PODCAST_SERVER         = 3;
   public static final int SRV_SLIDESTAR              = 4;
   public static final int SRV_YOUTUBE                = 5;

   // Transfer type (all formats)
   public static final int TRANSFER_NETWORK_DRIVE     = 0;
   public static final int TRANSFER_UPLOAD            = 1;

   // Different Slide sizes (RM, WM, Flash)
   public static final int SLIDESIZE_STANDARD         = 0;
   public static final int SLIDESIZE_ORIGINAL         = 1;
   public static final int SLIDESIZE_SCREEN_RES       = 2;
   public static final int SLIDESIZE_CUSTOM           = 3;

   // Different Video sizes (RM, WM, Flash)
   public static final int VIDEOSIZE_STANDARD         = 0;
   public static final int VIDEOSIZE_ORIGINAL         = 1;
   public static final int VIDEOSIZE_MAXCLIP          = 2;
   public static final int VIDEOSIZE_CUSTOM           = 3;

   // Different Clip sizes (RM, WM, Flash)
   public static final int CLIPSIZE_STANDARD          = 0;
   public static final int CLIPSIZE_MAXSLIDE          = 1;
   public static final int CLIPSIZE_CUSTOM            = 2;

   // Different Movie sizes (Flash)
   public static final int MOVIESIZE_SCREEN           = 0;
   public static final int MOVIESIZE_FLEXIBLE         = 1;
   public static final int MOVIESIZE_CUSTOM           = 2;

   // Template types (Flash)
   public static final int TEMPLATE_FLASH_NORMAL      = 0;
   public static final int TEMPLATE_FLASH_SLIDESTAR   = 1;
   public static final int TEMPLATE_FLASH_PSP         = 2;
   public static final int TEMPLATE_FLASH_FLEX        = 3;

   // WM Profile types (WM)
   public static final int PLAYER_64                  = 0x40000;
   public static final int PLAYER_70                  = 0x70000;
   public static final int PLAYER_80                  = 0x80000;
   public static final int PLAYER_90                  = 0x90000;
   public static final int PLAYER_CP                  = 0x20000;

   // WM Codec types (WM)
   public static final int WM_CODEC_64                = 0;
   public static final int WM_CODEC_80                = 1;
   public static final int WM_CODEC_90                = 2;

   // WM Bandwidth types (WM)
   public static final int WM_BANDWIDTH_MODEM         = 0;
   public static final int WM_BANDWIDTH_ISDN          = 1;
   public static final int WM_BANDWIDTH_DSL           = 2;
   public static final int WM_BANDWIDTH_LAN           = 3;
   public static final int WM_BANDWIDTH_USER_DEFINED  = 4;

   // MP4 Quality types
   public static final int MP4_QUALITY_LOW            = 0;
   public static final int MP4_QUALITY_MEDIUM         = 1;
   public static final int MP4_QUALITY_HIGH           = 2;
   public static final int MP4_QUALITY_USER_DEFINED   = 3;

   // Standard Bitrates and Framerates (WM, Flash)
   public static final int STD_BITRATE_AUDIO_MP4      = 48; //32000;
   public static final int STD_BITRATE_AUDIO_WM       = 48000; //20000;
   public static final int STD_BITRATE_AUDIO_FLASH    = 48000;
   public static final int STD_BITRATE_VIDEO_WM       = 120000;
   public static final int STD_BITRATE_VIDEO_FLASH    = 800000;
   public static final int STD_BITRATE_CLIPS_WM       = 150000;
   public static final int STD_BITRATE_CLIPS_FLASH    = 5000000;
   public static final int STD_FRAMERATE_TENTHS_VIDEO = 250;
   public static final int STD_FRAMERATE_TENTHS_CLIPS = 80;

   // Upload Protocols (all export formats)
   public static final int UPLOAD_FTP                 = 0;
   public static final int UPLOAD_SCP                 = 1;
   public static final int UPLOAD_SFTP                = 2;
   public static final int UPLOAD_HTTP				  = 3;

   // Cache (CD export)
   public static final int CACHE_SETTINGS_TEMP        = 0;
   public static final int CACHE_SETTINGS_HOME        = 1;
   public static final int CACHE_SETTINGS_REGISTRY    = 2;

   public static final int CACHE_WHEN_ALWAYS          = 0;
   public static final int CACHE_WHEN_CD              = 1;
   public static final int CACHE_WHEN_NEVER           = 2;

   public static final int CACHE_WHERE_TEMP           = 0;
   public static final int CACHE_WHERE_CUSTOM         = 1;

   // "Conversion format" > "Extended" Dialog 
   public static final int PANEL_VIDEO_SELECT         = 0;
   public static final int PANEL_VIDEO_PROPERTIES     = 1;

   // RM/WM/Flash "Settings" > "Extended" Dialog 
   public static final int PANEL_EXTD_EXTENT          = 0;
   public static final int PANEL_EXTD_SLIDES          = 1;
   public static final int PANEL_EXTD_SIZES           = 2;
   public static final int PANEL_EXTD_RATES           = 3;


   public static boolean B_GOTO_START_DIALOG          = false;
   public static boolean B_RETRY_UPLOAD               = false;
   public static boolean B_INSTALL_DIR_AVAILABLE      = false;
   public static String  STR_INSTALL_DIR              = null;


   static 
   {
      String strInstDir = System.getProperty("install.path");
      if (strInstDir == null)
      {
         B_INSTALL_DIR_AVAILABLE = false;
         STR_INSTALL_DIR = null;
      }
      else
      {
         B_INSTALL_DIR_AVAILABLE = true;
         if (!strInstDir.endsWith(File.separator))
            strInstDir += File.separatorChar;
         STR_INSTALL_DIR = strInstDir;
      }
   }


   //-----------------------------------------------------------------
   // GUI variables
   /** Panel type in ExtendedVideoDialog ("Conversion format" --> "Advanced") */
   public int              m_iVideoPanelType          = PANEL_VIDEO_SELECT;
   /** Panel type in ExtendedSettingsDialog ("Settings" --> "Advanced") */
   public int              m_iVideoExtendedType       = PANEL_EXTD_EXTENT;
   //-----------------------------------------------------------------
   /** LPD: Boolean flag for adding LPD to CD project */
   public boolean          m_bLpdAddToCdProject       = false;
   /** FCC Codec */
   public String           m_strFccCodec              = "";
   /** Matching Screen Resolution */
   public Dimension        m_MatchingScreenDimension  = new Dimension(-1, -1);
   /** Preferred Screen Resolution */
   public Dimension        m_PreferredScreenDimension = new Dimension(-1, -1);
   /** Preferred Slide Resolution */
   public Dimension        m_PreferredSlideDimension  = new Dimension(-1, -1);
   /** Preferred Flash Movie Resolution */
   public Dimension        m_PreferredMovieDimension  = new Dimension(-1, -1);
   /** Custom Page size */
   public Dimension        m_CustomPageDimension      = new Dimension(-1, -1);
   /** Custom Video size */
   public Dimension        m_CustomVideoDimension     = new Dimension(-1, -1);
   /** Custom Clips size */
   public Dimension        m_CustomClipsDimension     = new Dimension(-1, -1);
   /** Whiteboard scale factor (Flash) */
   public double           m_dWhiteboardScale         = 1.0d;
   /** Minimum RM version (RM) */
   public int              m_iMinRealVersion          = 8;
   /** Description of the video profile */
   public String           m_strWmVideoProfileDescription   = "";
   /** Description of the video profile */
   public String           m_strWmClipsProfileDescription   = "";
   /** WM suffix (default: ".wm") */
   public String           m_strWmSuffix              = ".wm";

//   /** File list of created plain files for the target document (with or without streaming files) */   
//   public String[]         m_aListOfPlainFiles           = null;
//   /** File list of created streaming files for the target document */   
//   public String[]         m_aListOfStreamingFiles       = null;
   /** File list of interactive opened files for the target document to be copied */   
   public String[]         m_aListOfInteractiveOpenedFiles = null;
   /** Set to true if all existing files can be overwritten */
   public boolean          m_bOverwriteAll            = false;
   /** Set to true if -output is specified */
   public boolean          m_bFixedOutputPathGiven    = false;

   /** Set to true if this Publisher instance is for setting profile informations only. */
   public boolean          m_bIsProfileMode           = false;

   // BUGFIX 5486: "Publish advanced" should open "Select Format Dialog"
   // disable "back" button to switch to "Select Presentation" 
   public boolean          m_bDisableSelectPresentation = false;

   /** Set to true if this Publisher instance is in direct publishing mode. */
   public boolean          m_bIsDirectMode            = false;

   /** Set to true if this Publisher instance is in publishing mode for Dynamic Power Trainer. */
   public boolean          m_bIsPowerTrainerMode      = false;
   
   /** Set to true if this Publisher instance is in Live Context mode. */
   public boolean          m_bIsLiveContext           = false;

   /** ProgressManager which handles Progress values and labels */
   public ProgressManager  m_progressManager = null; 

   /** In Dynamic Power Trainer Mode all communication is handled by Dynamic Power Trainer. */ 
   public DptCommunicator  m_dptCommunicator = null;

   /** Set to true if an Upload Session is running */
   public boolean          m_bIsUploadSessionRunning  = false;

   /** Start Dialog: An object of SelectPresentationWizardPanel */
   public SelectPresentationWizardPanel m_StartDialog = null;
   /** Upload Dialog: An object of TargetUploadWizardPanel */
   public TargetUploadWizardPanel m_UploadDialog = null;

   //-----------------------------------------------------------------
   /** Parent Panel (necessary for error messages, etc.) */
   private WizardPanel              m_parentPanel                    = null;

   /** The "Profile Settings" object */
   private ProfiledSettings         m_ProfiledSettings               = null;
   /** The Document specific data */
   private DocumentData             m_DocumentData                   = null;
   

   //-----------------------------------------------------------------
   // Constructor
   public PublisherWizardData()
   {
      this(null);
   }

   public PublisherWizardData(WizardPanel parent)
   {
      m_parentPanel = parent;
      m_ProfiledSettings = new ProfiledSettings();
      m_DocumentData = new DocumentData(parent);

      m_PreferredScreenDimension = new Dimension(StreamingMediaConverter.STANDARD_SCREEN_WIDTH, 
                                                 StreamingMediaConverter.STANDARD_SCREEN_HEIGHT);
      m_PreferredSlideDimension = new Dimension(StreamingMediaConverter.STANDARD_SLIDE_WIDTH, 
                                                StreamingMediaConverter.STANDARD_SLIDE_HEIGHT);
   }


   //-----------------------------------------------------------------
   // Public methods
   public boolean HasNormalVideo()
   {
      return ( (m_DocumentData.GetVideoFileName().length() > 0)
               && (m_DocumentData.GetVideoDimension().width > 0) 
               && (m_DocumentData.GetVideoDimension().height > 0) );
   }

   public boolean HasMultipleVideos()
   {
      return m_DocumentData.HasMultipleVideos();
   }

   public boolean HasClips()
   {
      return m_DocumentData.HasMultipleVideos();
   }

   public boolean HasStillImage()
   {
      return m_DocumentData.HasStillImage();
   }

   public boolean IsDenverDocument()
   {
      return (m_DocumentData.GetDocumentType() == DOCUMENT_TYPE_DENVER);
   }

   public SelectPresentationWizardPanel GetStartDialog()
   {
      return m_StartDialog;
   }

   public void SetStartDialog(SelectPresentationWizardPanel startDialog)
   {
      m_StartDialog = startDialog;
   }

   public TargetUploadWizardPanel GetUploadDialog()
   {
      return m_UploadDialog;
   }

   public void SetUploadDialog(TargetUploadWizardPanel uploadDialog)
   {
      m_UploadDialog = uploadDialog;
   }

   public boolean ReadDocumentDataFromPresentationFile(String strPresentationFileName)
   {
      return ReadDocumentDataFromPresentationFile(strPresentationFileName, false);
   }

   public boolean ReadDocumentDataFromPresentationFile(String strPresentationFileName, boolean bDoNotUpdateProfileData)
   {
      boolean bSuccess = strPresentationFileName != null && strPresentationFileName.length() > 0;
      
      if (bSuccess)
         bSuccess = m_DocumentData.ReadDocumentDataFromPresentationFile(strPresentationFileName);

      if (bSuccess)
         UpdateDataAndSettings(bDoNotUpdateProfileData);

      return bSuccess;
   }

   public ProfiledSettings GetProfiledSettings()
   {
      return m_ProfiledSettings;
   }

   public DocumentData GetDocumentData()
   {
      return m_DocumentData;
   }

   public Dimension FlashCalculatePageSizeFromMovieSize(Dimension sizeMovie)
   {
      return FlashCalculateContentSizeFromMovieSize(sizeMovie, false);
   }

   public Dimension FlashCalculateVideoSizeFromMovieSize(Dimension sizeMovie)
   {
      return FlashCalculateContentSizeFromMovieSize(sizeMovie, true);
   }

   public Dimension FlashGetWbSizeWithBorders()
   {
      ProfiledSettings  ps = m_ProfiledSettings;
      DocumentData      dd = m_DocumentData;

      Dimension dimSmaller;
      Dimension dimGreater;
      Insets insets = new Insets(0,0,0,0);
      Dimension dimLogoSize = FlashGetLogoSize();
      Dimension dimVideoSize = FlashGetAccVideoOutputSize(); // Accompanying video
      if (dd.HasMultipleVideos() && !ps.GetBoolValue("bShowClipsOnSlides") 
         && dimVideoSize.width <= 0 && dimVideoSize.height <= 0)
      {
         dimVideoSize = new Dimension(ps.GetIntValue("iVideoWidth"), ps.GetIntValue("iVideoHeight"));
      }
      boolean bIsControlbarVisible = (ps.GetIntValue("iControlBarVisibility") == 2) ? false : true;

      if (dd.GetDocumentType() == DOCUMENT_TYPE_DENVER) // Denver document
      {
         LPLibs.flashSetSgStandAloneMode(true);
         dimSmaller = new Dimension(0, 0);
         dimVideoSize = new Dimension(ps.GetIntValue("iVideoWidth"), ps.GetIntValue("iVideoHeight"));
         if (ps.GetIntValue("iVideoSizeType") == VIDEOSIZE_CUSTOM)
            dimVideoSize = new Dimension(m_CustomVideoDimension);
      }
      else 
      {
         LPLibs.flashSetSgStandAloneMode(false);
         dimSmaller = dd.GetWhiteboardDimension();
      }
      
      // Before calculating movie insets: Set PSP design flag
      LPLibs.flashSetPspDesign(ps.GetIntValue("iFlashTemplateType") == TEMPLATE_FLASH_PSP);

      int nResult = LPLibs.flashObtainMovieInsets(insets, dimSmaller, ps.GetBoolValue("bFlashDisplayTitle"), 
                                                  dimLogoSize, dimVideoSize, bIsControlbarVisible);
      // TODO: can this error value be ignored?
      
      
      if (dd.GetDocumentType() == DOCUMENT_TYPE_DENVER) // Denver document
         dimSmaller = new Dimension(dimVideoSize);

      dimGreater = new Dimension(
         dimSmaller.width + insets.left + insets.right, dimSmaller.height + insets.top + insets.bottom);
      
      return dimGreater;
   }
   
   public Dimension FlashSubtractScaledFlashBorders(Dimension dimGreater)
   {
      ProfiledSettings  ps = m_ProfiledSettings;
      DocumentData      dd = m_DocumentData;

      Dimension dimContent;
      Dimension dimSmaller;
      Insets insets = new Insets(0,0,0,0);
      Dimension dimLogoSize = FlashGetLogoSize();
      Dimension dimVideoSize = FlashGetAccVideoOutputSize(); // Accompanying video
      boolean bIsControlbarVisible = (ps.GetIntValue("iControlBarVisibility") == 2) ? false : true;

      if (dd.GetDocumentType() == DOCUMENT_TYPE_DENVER) // Denver document
      {
         LPLibs.flashSetSgStandAloneMode(true);
         dimContent = new Dimension(0, 0);
         dimVideoSize = new Dimension(ps.GetIntValue("iVideoWidth"), ps.GetIntValue("iVideoHeight"));
         if (ps.GetIntValue("iVideoSizeType") == VIDEOSIZE_CUSTOM)
            dimVideoSize = new Dimension(m_CustomVideoDimension);
      }
      else
      {
         LPLibs.flashSetSgStandAloneMode(false);
         dimContent = dd.GetWhiteboardDimension();
      }

      // Before calculating movie insets: Set PSP design flag
      LPLibs.flashSetPspDesign(ps.GetIntValue("iFlashTemplateType") == TEMPLATE_FLASH_PSP);

      int nResult = LPLibs.flashObtainMovieInsets(insets, dimContent, ps.GetBoolValue("bFlashDisplayTitle"), 
                                                  dimLogoSize, dimVideoSize, bIsControlbarVisible);
      // TODO: can this error value be ignored?
      
      if (dd.GetDocumentType() == DOCUMENT_TYPE_DENVER) // Denver document
      {
         dimSmaller = new Dimension((insets.left + insets.right), (insets.top + insets.bottom));
      }
      else
      {
         int nNormalWidth = dimContent.width + insets.left + insets.right;
         float fEnlargementFactor = dimGreater.width/(float)nNormalWidth;
         
         dimSmaller = new Dimension(
            dimGreater.width - Math.round(fEnlargementFactor * (insets.left + insets.right)), 
            dimGreater.height - Math.round(fEnlargementFactor * (insets.top + insets.bottom)));
      }
      
      return dimSmaller;
   }

   public void SetWmSuffix(boolean bWmUseAdvancedSuffix)
   {
      if (bWmUseAdvancedSuffix)
      {
         if (HasNormalVideo() || HasClips() || IsDenverDocument())
            m_strWmSuffix = ".wmv";
         else
            m_strWmSuffix = ".wma";
      }
      else
         m_strWmSuffix = ".wm";
   }

   //-----------------------------------------------------------------
   // Private Methods
   private Dimension FlashGetLogoSize()
   {
      ProfiledSettings  ps = m_ProfiledSettings;

      if (ps.GetBoolValue("bFlashDisplayTitle") && ps.GetStringValue("strLogoImageName").length() > 0)
      {
         ImageIcon icon = new ImageIcon(ps.GetStringValue("strLogoImageName"));
         int nLogoWidth = icon.getIconWidth();
         int nLogoHeight = icon.getIconHeight();
            
         return new Dimension(nLogoWidth, nLogoHeight);
      }
      
      return new Dimension(0, 0);
   }

   private Dimension FlashGetAccVideoOutputSize()
   {
      ProfiledSettings  ps = m_ProfiledSettings;
      DocumentData      dd = m_DocumentData;

   	if ((dd.GetVideoFileName().length() > 0) 
   	   && (ps.GetIntValue("iVideoWidth") > 0) && (ps.GetIntValue("iVideoHeight") > 0))
      	return new Dimension(ps.GetIntValue("iVideoWidth"), ps.GetIntValue("iVideoHeight"));
      else
      	return new Dimension(0, 0);
   }

   private Dimension FlashCalculateContentSizeFromMovieSize(Dimension sizeMovie, boolean bContentIsVideo)
   {
      // content size is either page size or video size

      ProfiledSettings  ps = m_ProfiledSettings;
      DocumentData      dd = m_DocumentData;

      Dimension sizePagesOut = new Dimension(0, 0);
      Dimension sizeVideoOut = new Dimension(-1, -1);
      
      Dimension sizeVideoIn = FlashGetAccVideoOutputSize(); // Accompanying video
      if (dd.GetDocumentType() == DOCUMENT_TYPE_DENVER)
      {
         sizeVideoIn = new Dimension(ps.GetIntValue("iVideoWidth"), ps.GetIntValue("iVideoHeight"));
         if (ps.GetIntValue("iVideoSizeType") == VIDEOSIZE_CUSTOM)
            sizeVideoIn = new Dimension(m_CustomVideoDimension);
      }
      Dimension sizePagesIn = dd.GetWhiteboardDimension();
      Dimension sizeLogo = FlashGetLogoSize();
      boolean bIsControlbarVisible = (ps.GetIntValue("iControlBarVisibility") == 2) ? false : true;
      boolean bShowVideoOnPages = dd.isClipStructured();

      // Before calculating movie insets: Set PSP design flag
      LPLibs.flashSetPspDesign(ps.GetIntValue("iFlashTemplateType") == TEMPLATE_FLASH_PSP);

      LPLibs.flashGetContentDimensions(sizeMovie, sizeVideoIn, sizePagesIn, sizeLogo, 
                                       ps.GetBoolValue("bFlashDisplayTitle"), bIsControlbarVisible, bShowVideoOnPages, 
                                       sizeVideoOut, sizePagesOut);

      if (bContentIsVideo)
         return sizeVideoOut;
      else
         return sizePagesOut;
   }

//   private void UpdateDataAndSettings()
//   {
//      UpdateDataAndSettings(false);
//   }

   private void UpdateDataAndSettings(boolean bDoNotUpdateProfileData)
   {
      DocumentData      dd = m_DocumentData;

      if (!bDoNotUpdateProfileData) {
         int type = dd.GetDocumentType();
         if (ConverterWizard.m_bGenericProfilesEnabled && this.m_bIsProfileMode) {
             // generic profiles have no type
             type = PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
         }
         m_ProfiledSettings = new ProfiledSettings(type);
      }
      
      ProfiledSettings  ps = m_ProfiledSettings;    
      
      if (dd.GetVideoDimension() != null)
      {
         // Update video size
         ///ps.SetIntValue("iVideoWidth", dd.GetVideoDimension().width);
         ///ps.SetIntValue("iVideoHeight", dd.GetVideoDimension().height);
         // Bugfix 4269/4364: default output video size is 320x240
         ps.SetIntValue("iVideoWidth", StreamingMediaConverter.STANDARD_VIDEO_WIDTH);
         ps.SetIntValue("iVideoHeight", StreamingMediaConverter.STANDARD_VIDEO_HEIGHT);
      }

      if (dd.GetWhiteboardDimension() != null)
      {
         // Calculate output slide size
         double dWbWidth = (double)dd.GetWhiteboardDimension().width;
         double dWbHeight = (double)dd.GetWhiteboardDimension().height;
         double scaleX = (double)StreamingMediaConverter.MAX_SLIDE_WIDTH / dWbWidth;
         double scaleY = (double)StreamingMediaConverter.MAX_SLIDE_HEIGHT / dWbHeight;
         double slideScale = (scaleX < scaleY) ? scaleX : scaleY;

         m_PreferredSlideDimension = new Dimension((int)(slideScale * dWbWidth + 0.5), 
                                                   (int)(slideScale * dWbHeight + 0.5));

         // Update Slide size
         ps.SetIntValue("iSlideWidth", m_PreferredSlideDimension.width);
         ps.SetIntValue("iSlideHeight", m_PreferredSlideDimension.height);

         //Update  Clips size
         ps.SetIntValue("iMaxCustomClipWidth", m_PreferredSlideDimension.width);
         ps.SetIntValue("iMaxCustomClipHeight", m_PreferredSlideDimension.height);
      }

      if (IsDenverDocument())
         m_iVideoExtendedType = PANEL_EXTD_SIZES;

      if (dd.HasMultipleVideos())
      {
         if (IsDenverDocument())
         {
            // StreamingVideoSizePanel Special case I:
            // strange hack, as StreamingVideoSizePanel uses
            // in this case (Denver) the video radio buttons
            // to display the clip properties
            ps.SetIntValue("iVideoSizeType", VIDEOSIZE_MAXCLIP);

            Dimension maxVideoSize = GetMaximumVideoSize(true);
            ps.SetIntValue("iVideoWidth", maxVideoSize.width);
            ps.SetIntValue("iVideoHeight", maxVideoSize.height);
         }
         else // Normal case: no 'Denver' document
         {
            ps.SetBoolValue("bShowClipsOnSlides", true);

            // Preselect "clips not larger than slide size"
            ps.SetIntValue("iClipSizeType", CLIPSIZE_MAXSLIDE);
         }
      }

      // StreamingVideoSizePanel Special case II:
      // Another strange hack caused by a special case in StreamingVideoSizePanel:
      // videos smaller than 320x240 should not be sized up to standard size
      // (equal if we have clips or not - clips are separated from the video)
      if (dd.HasNormalVideo() && !IsDenverDocument())
      {
         if (dd.GetVideoDimension().width < StreamingMediaConverter.STANDARD_VIDEO_WIDTH
            && dd.GetVideoDimension().height < StreamingMediaConverter.STANDARD_VIDEO_HEIGHT)
         {
            ps.SetIntValue("iVideoSizeType", VIDEOSIZE_ORIGINAL);
         }
      }

      // TODO: should this be done for Flash export only??
      // --> In principle: yes. But in 'non profile' mode the export type is not 
      // known yet (default: LPD), because this method is called before an export format 
      // is selected. 
//      if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH) { 
          // Another special case: Flash
          // Metadata: Change text color if we have video and pages
          if (dd.HasNormalVideo() && !IsDenverDocument())
             ps.SetIntValue("iFlashColorText", 0x000000);
    
          ps.SetBoolValue("bFlashDisplayTitle", true);
    
          // Calculate a default movie size
          Dimension movieSize = FlashGetWbSizeWithBorders();
          ps.SetIntValue("iFlashMovieWidth", movieSize.width);
          ps.SetIntValue("iFlashMovieHeight", movieSize.height);
//      }
   }

   // Bugfix 5329: Wrong clip size for RM/WM
   // GetMaximumVideoSize() is used in SelectActionWizardPanel.verifyNext() to recalculate video size
   //private 
   Dimension GetMaximumVideoSize(boolean bConsiderNormal)
   {
      Dimension maxVideoSize = new Dimension( -1, -1);

      DocumentData      dd = m_DocumentData;

      if (dd.HasMultipleVideos() && (dd.GetVideoClipInfos() != null))
      {
         for (int i = 0; i < dd.GetVideoClipInfos().length; ++i)
         {
            int w = dd.GetVideoClipInfos()[i].videoWidth;
            if (w > maxVideoSize.width)
               maxVideoSize.width = w;
            int h = dd.GetVideoClipInfos()[i].videoHeight;
            if (h > maxVideoSize.height)
               maxVideoSize.height = h;
         }
      }

      if (dd.HasNormalVideo() && bConsiderNormal)
      {
         int w = dd.GetVideoDimension().width;
         if (w > maxVideoSize.width)
            maxVideoSize.width = w;
         int h = dd.GetVideoDimension().height;
         if (h > maxVideoSize.height)
            maxVideoSize.height = h;
      }

      return maxVideoSize;
   }
}