package imc.lecturnity.converter.wizard;

import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.image.BufferedImage;
import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import javax.swing.*;
import java.util.StringTokenizer;
import java.lang.ProcessBuilder;

import com.sun.media.jai.codec.ImageEncoder;
import com.sun.media.jai.codec.ImageCodec;
import com.sun.media.jai.codec.PNGEncodeParam;

import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.filesdk.Metadata;
import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.VideoClipInfo;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.FlashSnippetCreator;
import imc.lecturnity.converter.ScreengrabbingExtended;
import imc.lecturnity.converter.Snippet;
import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.StreamingMediaConverter;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.converter.TemplateSnippetCreator;
import imc.lecturnity.converter.ThumbnailEntry;
import imc.lecturnity.converter.PictureExtracter;
import imc.lecturnity.converter.StreamInformation;
import imc.lecturnity.converter.DptCommunicator;
import imc.lecturnity.converter.ProgressManager;
import imc.lecturnity.util.TemplateParser;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.wizards.WizardData;
import imc.lecturnity.util.wizards.WizardPanel;

// for LAD2WAV
import imc.epresenter.player.audio.Audio2Wav;

/* REVIEW UK
 * Class too complex; should be separated
 * Eg Ming (slide) conversion code should be a separate class.
 */

/* REVIEW UK
 * updateProgress() and updateProgressLabel()
 *  -> methods without parameter/pure side effect
 *  -> should/maybe can be handled directly by ProgressManager
 *  -> for example: SetCurrentSubProgress() is called anyway
 * Why is m_progressManager a global variable?
 *  -> The only instance generating progress is the respective writer panel?
 */

/* REVIEW UK
 * Double (and tripple) code; for example for upload code
 * There are some arbitrary waits (sleep()) that should be properly solved/removed
 */

public class FlashWriterWizardPanel extends WizardPanel implements ActionListener
{
   private static Localizer g_Localizer = null;
   static 
   {
      try
      {
         g_Localizer = new Localizer(
            "/imc/lecturnity/converter/wizard/FlashWriterWizardPanel_", "en");
      }
      catch (IOException exc)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }
   
   private static boolean g_bEngineLoaded = false;
   static
   {
      try
      {
         System.loadLibrary("lplibs");
         g_bEngineLoaded = true;
      }
      catch (UnsatisfiedLinkError e)
      {
         System.out.println("!!! lplibs.dll not found !!!\n");
         // no error dialog needed as this is handled below
      }

      
   }



	public final static String STR_CONTENT = "content";
	private final static String STR_IMAGE_SUFFIX = ".jpeg";
   private static boolean m_bCreateAudioFlv = false;

   private PublisherWizardData m_pwData;
   
   private boolean m_bCancelRequested = false;
   
   private JLabel m_lblProgress;
   private JProgressBar m_barProgress;
   private JButton m_btnCancel;
   
   private String m_strHtmlTarget;
   private String m_strSwfTarget;
   private String m_strLzpTarget;
   private ArrayList htmlFileList;
   private ArrayList aRemoveUponCancel ;
   private String  JsDir =  "js" + File.separatorChar;
   private String  commonDir =  "common" + File.separatorChar;
   private String  extendDir =  "extend" + File.separatorChar;
   private String  scormDir =  "scorm" + File.separatorChar;
   private String  uniqueDir =  "unique" + File.separatorChar;
   private String  vocabDir =  "vocab" + File.separatorChar;
   private String  contentDir =  STR_CONTENT + File.separatorChar;
   private String  Player2Dir =  "player2" + File.separatorChar;

   private File m_pathFile;
   private File m_pathTargetFile;
   private String m_strDocumentName;
   private String m_strLogoFileName;
   //private String m_strWavFileName;
   private String m_strFlvVideoName;
   private String[] m_strFlvClipNames;
   private Dimension m_dimOuterFlashSize;
   private Dimension m_dimScaledWhiteboardSize;
   private int m_hrPossibleWarning;
   private boolean m_bErrorDisplayed;
   private String m_sThumbFiles;

   private static float m_fCurrentProgress = 0.0f;
   private static float m_fTotalProgress = 0.0f;
   private static String m_strProgressLabel = "";
   
   private ThumbnailEntry[] aThumbnailEntries;
   private DptCommunicator m_dptCommunicator = null;
   private ProgressManager m_progressManager;
   
   public FlashWriterWizardPanel(PublisherWizardData pwData) {
      super();
      m_pwData = pwData;
      m_dptCommunicator = pwData.m_dptCommunicator;
      m_progressManager = pwData.m_progressManager;
      
      initGui();
   }

   public static float GetProgress() {
      return m_fTotalProgress;
   }
   
   public int displayWizard()
   {
      int hr = 0;
      m_hrPossibleWarning = 0;

      m_bErrorDisplayed = false;
      m_strLogoFileName = "";
      
      
      aRemoveUponCancel = new ArrayList(4);

      ProfiledSettings ps = m_pwData.GetProfiledSettings(); 
      DocumentData dd = m_pwData.GetDocumentData();
      
      // PZI: structured screengrabbing clips
      // add/remove structure and fulltextsearch to/of screengrabbing clips
      if (hr >= 0 && !m_bCancelRequested) {          
         if (!ScreengrabbingExtended.updateClipStructure(ps, dd)) {
             // cancel pressed or error
             m_bCancelRequested = true;
         }
      }

      boolean bSeparateClips = ps.GetBoolValue("bShowClipsOnSlides");
      
      boolean bIsDenver = dd.IsDenver();
      boolean bIsStructured = IsUseStructuredAsVideo(dd);
      boolean bLecIdentityHidden = ps.GetBoolValue("bHideLecturnityIdentity");
          
      boolean bHasSomeVideo = dd.HasNormalVideo() || (dd.HasClips() && !bSeparateClips) || bIsDenver;
      boolean bHasClips = dd.HasClips() && bSeparateClips;
      boolean bHasClipsCombined = dd.HasClips() && !bSeparateClips;
      boolean bIsSlidestarExport = 
         ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR;
      boolean bIsPsp = 
         ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_PSP;
      boolean bIsFlexPlayerExport = 
         ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX;
      boolean bUseStreamingServer = 
         ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER && 
         ps.GetIntValue("iServerType") == PublisherWizardData.SRV_STREAMING_SERVER;
      boolean bScormEnabled = ps.GetBoolValue("bScormEnabled");
      boolean bCreateWebsite = ps.GetBoolValue("bFlashCreateWebsite");
      boolean bDoUpload = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
              && (ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_UPLOAD) );
      boolean bUseNetworkDrive = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                     &&(ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_NETWORK_DRIVE) );
      int iClipCount = dd.HasClips() ? dd.GetVideoClipInfos().length : 0;
      

      // Define the steps for the ProgressManager
      if (hr >= 0 && !m_bCancelRequested) {
         m_barProgress.setMaximum(100);
         DefineProgressBarSteps(bHasSomeVideo, bIsStructured, bHasClips, bIsDenver, 
                                bIsSlidestarExport, bIsFlexPlayerExport, bCreateWebsite, 
                                bUseStreamingServer, bDoUpload,
                                bUseNetworkDrive, bScormEnabled, iClipCount);
      }

      m_pathFile = new File(dd.GetPresentationFileName()).getParentFile();
      String strTargetFilename = ps.GetTargetFileName() + ".swf";
      if (bIsFlexPlayerExport)
      {
         // Add "content" directory to the path of the target
         String strPath = FileUtils.extractPathFromPathFile(strTargetFilename) + contentDir;
         aRemoveUponCancel.add(strPath);
         strTargetFilename = strPath + ps.GetStringValue("strTargetPathlessDocName") + ".swf";
         // New Flex Player reads audio from external file (in case of no video)
         m_bCreateAudioFlv = true;
      }
      String strLogoUrl = ps.GetStringValue("strLogoImageUrl");

      VideoClipInfo[] aClipInfos = null;
      if (iClipCount > 0)        
         aClipInfos = dd.GetVideoClipInfos();

      m_pathTargetFile = new File(strTargetFilename).getParentFile();
      String strTargetDir = m_pathTargetFile.toString();
      if (!strTargetDir.endsWith(File.separator))
         strTargetDir += File.separatorChar;

      setEnableButton(BACK_QUIT_CANCEL, false);

      // Bugfix 4295: Special cases: SCORM and SLIDESTAR
      // - SLIDESTAR --> SCORM must be disabled
      if (bIsSlidestarExport)
      {
         ps.SetBoolValue("bScormEnabled", false);
         ps.SetBoolValue("bScormStrict", false);
         bScormEnabled = false;
      }
      // - SCORM enabled --> the flag 'create web page' must be set
      if (bScormEnabled)
         ps.SetBoolValue("bFlashCreateWebsite", true);

      // Special case: SCORM and SLIDESTAR: In the end there is only one ZIP or LZP file resp.
      boolean bTargetIsZipOrLzp = (bScormEnabled || bIsSlidestarExport);

      // BUGFIX 5140: Direct Publish should use different dir (analog to Profile Mode)
      if (hr >= 0 && !m_bCancelRequested && (m_pwData.m_bIsProfileMode || m_pwData.m_bIsDirectMode || bDoUpload || bUseNetworkDrive))
      {
         if (bDoUpload)
         {
            m_strProgressLabel = g_Localizer.getLocalized("CHECK_UPLOAD_SERVER");
            m_progressManager.SetProgressLabel(m_strProgressLabel);
            m_progressManager.SetProgressStep("checkUpload");
            m_progressManager.SetCurrentSubProgress(0.0f);
            updateProgressLabel();
            updateProgress();

            if (!m_pwData.m_bIsUploadSessionRunning)
            {
               // Start the Uploader Session
               UploaderAccess.StartSession();
			   if (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_SLIDESTAR)
				   UploaderAccess.IsSlidestar(true);
			   else
				   UploaderAccess.IsSlidestar(false);
               m_pwData.m_bIsUploadSessionRunning = true;

               // check access and collect passwords (if necessary); also shows error message
               hr = UploaderAccess.CheckUpload(true, ps, this);

               if (hr == 0 && bUseStreamingServer)
                  hr = UploaderAccess.CheckUpload(false, ps, this);
            }
            m_progressManager.SetCurrentSubProgress(1.0f);
            updateProgress();
         }

         if (hr >= 0)
         {
            // Use generic paths and names for Profile/Upload/Network drive
            if (hr >= 0)
               hr = SelectPresentationWizardPanel.DetermineTargetPathsAndFiles(m_pwData, this);

            // Use temporary local upload dir
            strTargetDir = ps.GetStringValue("strTargetDirLocal"); 
            if (!strTargetDir.endsWith(File.separator))
               strTargetDir += File.separatorChar;
            if (bIsFlexPlayerExport)
            {
               // Add "content" directory to the path of the target
               strTargetDir += contentDir;
               aRemoveUponCancel.add(strTargetDir);
            }
            strTargetFilename = strTargetDir 
                                + ps.GetStringValue("strTargetPathlessDocName") 
                                + ".swf";
            m_pathTargetFile = new File(strTargetFilename).getParentFile();
         }
      }

      // BUGFIX 5236: Check length of target path and files
      int hrPathCheck = 0;
      if (hr >= 0)
         hrPathCheck = SelectPresentationWizardPanel.CheckTargetPathFileLength(m_pwData, this);
      if (hrPathCheck == LPLibs.CANCELLED)
         m_bCancelRequested = true;
      
      // Check target directory and create it, if necessary
      if (hr >= 0 && !m_bCancelRequested) {
         File targetDir = new File(FileUtils.extractPathFromPathFile(strTargetFilename));
         if (hr >= 0 && !m_bCancelRequested)
            targetDir.mkdirs();
      }

      if (hr != 0 && !m_bCancelRequested)
      {
         if (hr > 0)
         {
            // "Cancel" was pressed
            m_bCancelRequested = true;
         }
         else
         {
            System.out.println("!!! Upload error in FlashWriterWizardPanel::displayWizard()1!");

            // Finish Uploader Session
            UploaderAccess.FinishSession();
            m_pwData.m_bIsUploadSessionRunning = false;

            // Get the error text from the error code (= hr) 
            String strError = "";
            try {
               Localizer l = new Localizer(
                  "/imc/lecturnity/converter/wizard/TargetUploadWizardPanel_", "en");
               strError = l.getLocalized("" + hr);
            } catch (IOException exc) {
               // should not happen
               exc.printStackTrace();
            }
            // Inform the ProgressManager
            m_progressManager.SendErrorMessage(strError);

            if( m_pwData.m_bIsPowerTrainerMode && (m_dptCommunicator != null) ) {
               // Put the error text to the DPT Communicator
               if (strError.length() > 0) {
                  m_dptCommunicator.ShowErrorMessage(strError);
                  m_bErrorDisplayed = true;
               }
            } else {
               int res = UploaderAccess.ShowErrorMessageForErrorCode(hr, (WizardPanel)this);
               m_bErrorDisplayed = true;

               if (res == 0) // "Verify Settings"
               {
                  m_pwData.B_GOTO_START_DIALOG = true;

                  doNext();
                  return BUTTON_NEXT;
               }
               else if (res == 1) // "Retry"
               {
                  return displayWizard();
               }
               else // "Cancel" or Generic error
               {
                  // do nothing special, process is aborted
               }
            }
         }
      }
      
      String strStreamingUrl = "";
      String strStreamingTargetDir = strTargetDir;
      if (hr >= 0 && bUseStreamingServer)
      {
         strStreamingUrl = ps.GetStringValue("strTargetUrlStreamingServer");
         
         if (!bDoUpload)
            strStreamingTargetDir = ps.GetStringValue("strTargetDirStreamingServer");
      }
      int[] aiNavigationStates = new int[] { ps.GetIntValue("iControlBarVisibility"), 
         ps.GetIntValue("iNavigationButtonsVisibility"), ps.GetIntValue("iTimeLineVisibility"), 
         ps.GetIntValue("iTimeDisplayVisibility"), ps.GetIntValue("iDocumentStructureVisibility"),
         ps.GetIntValue("iPluginContextMenuVisibility") };
      boolean bIsAutostartMode = ps.GetBoolValue("bDoAutostart");
      int iFlashVersion = ps.GetIntValue("iFlashVersion");
      boolean bMakeVersion9Video = iFlashVersion >= 7; // only relevant here != 6
         

      // Calculate sizes for Flash Movie and Whiteboard
      Dimension dimOutput = new Dimension(-1, -1);
      Dimension dimVideoOutput = new Dimension(-1, -1);
      if (hr >= 0 && !m_bCancelRequested) {
          // nasty hack to avoid error in LPlibs, which always uses dimensions although only required for old Flash player
          // TODO: remove usage of dimensions in LPlibs whenever not required
          if (bMakeVersion9Video && ps.GetIntValue("iFlashMovieWidth") < 0)
              ps.SetIntValue("iFlashMovieWidth", 10000);          
          if (bMakeVersion9Video && ps.GetIntValue("iFlashMovieHeight") < 0)
              ps.SetIntValue("iFlashMovieHeight", 10000);          

         CalculateMovieDimensions();
      
         dimOutput = new Dimension(ps.GetIntValue("iFlashMovieWidth"), 
                                   ps.GetIntValue("iFlashMovieHeight"));
         dimVideoOutput = new Dimension(ps.GetIntValue("iVideoWidth"), 
                                        ps.GetIntValue("iVideoHeight"));
      }

      if (hr >= 0 && bIsStructured && (iClipCount > 0) && !m_bCancelRequested) {
         // If the clip of the structured SG document is treated like a video 
         // then the video size must be determined from the SG clip 
         // and maybe also rescaled (depending on profile)
         int clipWidth = aClipInfos[0].videoWidth;
         int clipHeight = aClipInfos[0].videoHeight;
         // Rescale clip sizes?
         Dimension clipSize = GetRescaledClipSize(clipWidth, clipHeight);
         dimVideoOutput.width = clipSize.width;
         dimVideoOutput.height = clipSize.height;
      }

      Dimension dimWhiteboard = new Dimension(-1, -1);
      if (!bIsDenver)
         dimWhiteboard = dd.GetWhiteboardDimension();
      boolean bDisplayLogo = ps.GetBoolValue("bFlashDisplayLogo");
      String strLogoFilename = "";
      if (bDisplayLogo)
         strLogoFilename = ps.GetStringValue("strLogoImageName");
      String strAudioFilename = dd.GetAudioFileName(true);
      String strVideoFilename = dd.GetVideoFileName(true);

      // Bugfix 4945: Check for audio/video bitrates necessary
      int iAudioKBit = ps.GetIntValue("iFlashAudioBitrate") / 1000;
      int iVideoBitrate = ps.GetIntValue("iFlashVideoBitrate");
      if (hr >= 0 && iVideoBitrate < 0 && !m_bCancelRequested) {
         StreamInformation siVideo = dd.GetVideoStreamInfo();
         if (siVideo != null)
            iVideoBitrate = FlashSettingsWizardPanel.GetFlashOptimizedVideoBitrate(siVideo.m_nWidth, 
                                                                                   siVideo.m_nHeight, 
                                                                                   (siVideo.m_nFrequency/10));
         else
            iVideoBitrate = PublisherWizardData.STD_BITRATE_VIDEO_FLASH;
      }
      int iVideoKBit = iVideoBitrate / 1000;
      int iClipBitrate = ps.GetIntValue("iFlashClipBitrate");
      if (hr >= 0 && iClipBitrate < 0 && !m_bCancelRequested) {
         StreamInformation siClips = dd.GetClipsStreamInfo();
         if (siClips != null)
         iClipBitrate = FlashSettingsWizardPanel.GetFlashOptimizedVideoBitrate(siClips.m_nWidth, 
                                                                               siClips.m_nHeight, 
                                                                               (siClips.m_nFrequency/10));
         else
            iClipBitrate = PublisherWizardData.STD_BITRATE_CLIPS_FLASH;
      }
      int iClipKBit = iClipBitrate / 1000;

      boolean bClipsCombinedButWithoutVideo =
         dd.HasClips() && !dd.HasNormalVideo() && !ps.GetBoolValue("bShowClipsOnSlides");
      int iVideoOffset = 0;
      if (dd.HasNormalVideo())
         iVideoOffset = dd.GetVideoOffsetMs();
      boolean bIncreaseKeyframes = ps.GetBoolValue("bFlashOptimizeClipSync");
      boolean bDoPadding = ps.GetBoolValue("bFlashDoNotCutVideos");
      boolean bDisplayTitle = ps.GetBoolValue("bFlashDisplayTitle");
      Metadata metadata = dd.GetMetadata();
      String strAuthor = "";
      String strTitle = "";
      if (bDisplayTitle && metadata != null)
      {
         strAuthor = metadata.author;
         strTitle = metadata.title;
      }
      int iColorBasic = ps.GetIntValue("iFlashColorBasic");
      int iColorBackground = ps.GetIntValue("iFlashColorBackground");
      int iColorText = ps.GetIntValue("iFlashColorText");
      int iMovieSizeType = ps.GetIntValue("iFlashMovieSizeType");
      boolean bFixedSize = iMovieSizeType == PublisherWizardData.MOVIESIZE_SCREEN
         || iMovieSizeType == PublisherWizardData.MOVIESIZE_CUSTOM;
      boolean bMenuEnabled = ps.GetIntValue("iPluginContextMenuVisibility") == 0;

      boolean bUseOriginalSlideSize = ps.GetIntValue("iSlideSizeType") == PublisherWizardData.SLIDESIZE_ORIGINAL;
      boolean bDoStartInOptimizedSize = ps.GetBoolValue("bFlashStartInOptimizedSize");
      
      boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && ps.GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
      if ( bIsGenericProfile) {
          // Generic Profile: Hack default text color for old FlashPlayer
          if ( dd.HasNormalVideo() && !m_pwData.IsDenverDocument() ) {
              // with video
              if ( iColorText==0xEAE7D7 && iColorBasic==0xEAE7D7 )
                  // adjust default colors w/o video to default colors w/ video
                  iColorText = 0x000000;
          } else { // no video
              if ( iColorText==0x000000 && iColorBasic==0xEAE7D7 )
                  // adjust default colors w/ video to default colors w/o video
                  iColorText = 0xEAE7D7;                
          }
      }

      m_strDocumentName = new File(strTargetFilename).getName();
      m_strDocumentName = m_strDocumentName.substring(0, m_strDocumentName.length()-4);
      // Assuming that audio file is a temporary WAV file (see below)
      //m_strWavFileName =  new File(m_pathTargetFile, m_strDocumentName + ".wav") + "";



      // Disable buttons during conversion
      setEnableButton(BACK_QUIT_CANCEL, false);  
      
      
      // Reset all previously set dimensions
      if (hr >= 0 && !m_bCancelRequested)
         hr = LPLibs.flashResetAllDimensions();

      // Set flag for Scorm Support mode
      if (hr >= 0 && !m_bCancelRequested)
         hr = LPLibs.flashSetScormEnabled(bScormEnabled);


      if (hr >= 0 && bHasClips)
         m_strFlvClipNames = new String[iClipCount];

      // for Bugfix 5598: Clips in video area 
      if (hr >= 0)
         hr = LPLibs.flashSetShowClipsInVideoArea(!bSeparateClips);
            
      // Set the output file for the Flash engine
      if (hr >= 0 && !m_bCancelRequested)
         hr = SetOutputFile(hr, strTargetFilename);
      
      // Set the input files for the Flash engine
      if (hr >= 0 && !m_bCancelRequested)
         hr = SetInputFiles(hr);
      
      // Set the target URL for Streaming  Server (or empty string)
      if (hr >= 0 && !m_bCancelRequested)
      {
         hr = LPLibs.flashSetRtmpPath(strStreamingUrl);
      }


      // Set flags for 'standard navigation'
      if (hr >= 0 && !m_bCancelRequested)
      {
         hr = LPLibs.flashSetStandardNavigationStates(
            aiNavigationStates[0], aiNavigationStates[1], aiNavigationStates[2],
            aiNavigationStates[3], aiNavigationStates[4], aiNavigationStates[5]);
      }

      // Set flag for autostart mode
      if (hr >= 0 && !m_bCancelRequested)
      {
         hr = LPLibs.flashSetAutostartMode(bIsAutostartMode);
      }
      
      // Set flag for Flash version
      if (hr >= 0 && !m_bCancelRequested)
      {
         hr = LPLibs.flashSetFlashVersion(iFlashVersion);
      }
      
      // Set flag to show/hide LECTURNITY "identity" (logo, text)
      if (hr >= 0 && !m_bCancelRequested)
      {
         hr = LPLibs.flashHideLecturnityIdentity(bLecIdentityHidden);
      }
      
      // Set flag for "Draw Eval Note"
      if (hr >= 0 && !m_bCancelRequested)
      {
         hr = SetDrawEvalNote(hr);
      }


      // Set input/output dimensions for the Flash engine
      if (hr >= 0 && !m_bCancelRequested)
      {
         hr = SetInputOutputDimensions(hr, dimOutput.width, dimOutput.height,
                                       dimWhiteboard.width, dimWhiteboard.height,
                                       bIsDenver);
      }


      // Bugfix 5403: make a "disk full" check before font conversion
      if (IsDiskFull())
          hr = LPLibs.COMMAND_EXECUTION_FAILED;

      if (hr < 0 && !m_bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("CONVERT_FONTS"), hr);
         m_bErrorDisplayed = true;
      }

      // Convert the fonts
      if (hr >= 0 && !m_bCancelRequested)
      {
         hr = ConvertFonts(hr);
      }
 
      // Convert the images
      if (hr >= 0 && !m_bCancelRequested)
      {
         hr = ConvertImages(hr, bDisplayLogo, strLogoFilename);
      }

 
      // Convert audio
      //
      // This is only necessary if the document does not contain any video (video, denver or clips in video area)
      // - otherwise the audio is included into the FLV file
         
      if (hr >= 0 && !m_bCancelRequested && !bHasSomeVideo && !bIsStructured)
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;
         
         m_strProgressLabel = g_Localizer.getLocalized("CONVERT_AUDIO");
         m_progressManager.SetProgressLabel(m_strProgressLabel);
         m_progressManager.SetProgressStep("convertAudio");
         m_progressManager.SetCurrentSubProgress(0.0f);
         updateProgress();
         updateProgressLabel();

         if (m_bCreateAudioFlv)
         {
            // Prepare the audio flv conversion
            hr = LPLibs.flvPrepare(null, 0, strAudioFilename, m_strFlvVideoName, 
                                   0, iAudioKBit, 
                                   0, 0, 
                                   false, false, false, bMakeVersion9Video);
            aRemoveUponCancel.add(m_strFlvVideoName);
         }
         else
         {
            // MP3 conversion
            // 1st step here: convert audio to a temporary WAV file
            
            /* Not needed anymore (24 June 09): LAD->MP3 done directly
            try
            {
               System.setProperty("nyckel.musik", "forte");
               Audio2Wav.convert(strAudioFilename, m_strWavFileName);
            }
            catch (Exception e)
            {
               e.printStackTrace();
                  
               hr = LPLibs.AUDIO_CONVERSION_FAILED;
            }
            */
         }
         m_fCurrentProgress = 1.0f;
         updateProgressLabel();
      }
      
      String strMp3FileName = null;
      if (hr >= 0 && !m_bCancelRequested && !bHasSomeVideo && !bIsStructured)
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;
         
         if (m_bCreateAudioFlv)
         {
            // Now start the conversion
            if (hr >= 0 && !m_bCancelRequested)
               hr = LPLibs.flvConvertStart();

            while (hr >= 0 && !m_bCancelRequested && LPLibs.flvGetProgress() < 10000)
            {
               m_fCurrentProgress = 1.0f * LPLibs.flvGetProgress() / 10000.0f;
               m_progressManager.SetCurrentSubProgress(m_fCurrentProgress);
               updateProgress();
               try { Thread.sleep(100); } catch (Exception exc) { }
            }
            hr = LPLibs.flvCleanup();

            try { Thread.sleep(500); } catch (InterruptedException exc) {}
            m_progressManager.SetCurrentSubProgress(1.0f);
            updateProgress();
         }
         else
         {
            // Convert audio to MP3
            strMp3FileName =  new File(m_pathTargetFile, m_strDocumentName + ".tmp") + "";
            hr = LPLibs.flashConvertAudioToMp3(strMp3FileName, iAudioKBit);

             // TODO What are these sleeps here?
            try { Thread.sleep(500); } catch (InterruptedException exc) {}
            m_progressManager.SetCurrentSubProgress(1.0f);
            updateProgress();
         }
      }
      
            
      if (hr < 0 && !m_bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("CONVERT_AUDIO"), hr);
         m_bErrorDisplayed = true;
      }
 

      // Step: convert video (if any)
      if (hr >= 0 && !m_bCancelRequested && (bHasSomeVideo || bIsStructured))
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;
         
         m_strProgressLabel = g_Localizer.getLocalized("CONVERT_VIDEO");
         m_progressManager.SetProgressLabel(m_strProgressLabel);
         m_progressManager.SetProgressStep("convertVideo");
         m_progressManager.SetCurrentSubProgress(0.0f);
         updateProgress();
         updateProgressLabel();

         String strVideo = "";
         int videoBitrate = 0;
         int videoOffset = 0;
         boolean bCombineWithClips = false;

         // "Denver" document?
         if (bIsDenver || bIsStructured)
         {
            strVideo = new File(m_pathFile, aClipInfos[0].clipFileName)+"";

            videoBitrate = iClipKBit;
            videoOffset = (int)aClipInfos[0].startTimeMs;
         }
         else if (bClipsCombinedButWithoutVideo)
         {
            strVideo = "";

            videoBitrate = iClipKBit;
            videoOffset = (int)aClipInfos[0].startTimeMs; // will be ignored lateron

            bCombineWithClips = true;
         }
         else
         {
            strVideo = strVideoFilename;

            videoBitrate = iVideoKBit;
            videoOffset = iVideoOffset;

            if (bHasClipsCombined)
               bCombineWithClips = true;
         }

         aRemoveUponCancel.add(m_strFlvVideoName);

         String strAudio = strAudioFilename; 

         // Bugfix 5601:
         // Video size must be 80x52 or higher - otherwise clip conversion "hangs"
         // (This "upscaling" might be only visible with a little quality loss
         // because the final video is then rescaled to the desired size)
         int videoWidth = dimVideoOutput.width;
         int videoHeight = dimVideoOutput.height;
         if ((videoWidth < 80) || (videoHeight < 52)) {
             double scaleX = 80.0 / (double)videoWidth;
             double scaleY = 52.0 / (double)videoHeight;
             double scaleVideo = (scaleX > scaleY) ? scaleX : scaleY;

             videoWidth = (int)( (double)videoWidth * scaleVideo + 0.5 );
             videoHeight = (int)( (double)videoHeight * scaleVideo + 0.5 );
         }
         
         // Prepare the conversion
         if (hr >= 0 && !m_bCancelRequested)
            hr = LPLibs.flvPrepare(strVideo, videoOffset, strAudio, m_strFlvVideoName, 
                                   videoBitrate, iAudioKBit, 
                                   videoWidth, videoHeight, 
                                   bCombineWithClips, bIncreaseKeyframes, bDoPadding,
                                   bMakeVersion9Video);

         // Combine clips with video?
         if (hr >= 0 && !m_bCancelRequested && bCombineWithClips)
         {
            for (int i = 0; (hr >= 0 && i < iClipCount); ++i)
            {
               String strInputClipName = new File(m_pathFile, aClipInfos[i].clipFileName)+"";
               int iClipOffsetMs = (int)(aClipInfos[i].startTimeMs);
               boolean bIsLastClip = i >= (iClipCount - 1);

               hr = LPLibs.flvAddClip(strInputClipName, iClipOffsetMs, bIsLastClip);
            }
         }
         
         // Now start the conversion
         if (hr >= 0 && !m_bCancelRequested)
            hr = LPLibs.flvConvertStart();
         
         while (hr >= 0 && !m_bCancelRequested && LPLibs.flvGetProgress() < 10000)
         {
            m_fCurrentProgress = 1.0f * LPLibs.flvGetProgress() / 10000.0f;
            m_progressManager.SetCurrentSubProgress(m_fCurrentProgress);
            updateProgress();
            try { Thread.sleep(100); } catch (Exception exc) { }
         }
         LPLibs.flvCleanup();

         aRemoveUponCancel.add(m_strFlvVideoName);
         
         try { Thread.sleep(500); } catch (InterruptedException exc) {}
         m_progressManager.SetCurrentSubProgress(1.0f);
         updateProgress();
      }

      if (hr < 0 && !m_bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("CONVERT_VIDEO"), hr);
         m_bErrorDisplayed = true;
      }
 

      // Step: convert clips (if any - and not combined with video)
      if (hr >= 0 && !m_bCancelRequested && bHasClips && !bIsDenver && !bIsStructured)
      {
         for (int i = 0; (hr >= 0 && i < iClipCount); ++i)
         {
            if (hr > 0)
               m_hrPossibleWarning = hr;
               
            m_strProgressLabel = g_Localizer.getLocalized("CONVERT_CLIP") + " " + i;
            m_progressManager.SetProgressLabel(m_strProgressLabel);
            String strClipId = "convertClip" + i;
            m_progressManager.SetProgressStep(strClipId);
            m_progressManager.SetCurrentSubProgress(0.0f);
            updateProgress();
            updateProgressLabel();

            String strClip = new File(m_pathFile, aClipInfos[i].clipFileName)+"";
            String strAudio = "";
            aRemoveUponCancel.add(m_strFlvClipNames[i]);

            int clipWidth = aClipInfos[i].videoWidth;
            int clipHeight = aClipInfos[i].videoHeight;
            // Rescale clip sizes?
            Dimension clipSize = GetRescaledClipSize(clipWidth, clipHeight);
            clipWidth = clipSize.width;
            clipHeight = clipSize.height;
            // Bugfix 5601:
            // Clip size must be 80x52 or higher - otherwise clip conversion "hangs"
            // (This "upscaling" might be only visible with a little quality loss
            // because the final clip is then rescaled to the desired size)
            if ((clipWidth < 80) || (clipHeight < 52)) {
                double scaleX = 80.0 / (double)clipWidth;
                double scaleY = 52.0 / (double)clipHeight;
                double scaleClips = (scaleX > scaleY) ? scaleX : scaleY;

                clipWidth = (int)( (double)clipWidth * scaleClips + 0.5 );
                clipHeight = (int)( (double)clipHeight * scaleClips + 0.5 );
            }

            boolean bCombineWithClips = false;
            
            // Now prepare and start the conversion
            if (hr >= 0 && !m_bCancelRequested)
               hr = LPLibs.flvPrepare(strClip, 0, strAudio, m_strFlvClipNames[i], 
                                      iClipKBit, 
                                      0, 
                                      clipWidth, clipHeight, 
                                      bCombineWithClips, bIncreaseKeyframes, bDoPadding, 
                                      bMakeVersion9Video);

            if (hr >= 0 && !m_bCancelRequested)
               hr = LPLibs.flvConvertStart();
         
            while (hr >= 0 && !m_bCancelRequested && LPLibs.flvGetProgress() < 10000)
            {
               m_fCurrentProgress = 1.0f * LPLibs.flvGetProgress() / 10000.0f;
               m_progressManager.SetCurrentSubProgress(m_fCurrentProgress);
               updateProgress();
               try { Thread.sleep(100); } catch (Exception exc) { }
            }
            LPLibs.flvCleanup();

            aRemoveUponCancel.add(m_strFlvClipNames[i]);

            try { Thread.sleep(500); } catch (InterruptedException exc) {}
            m_progressManager.SetCurrentSubProgress(1.0f);
            updateProgress();
         }
      }

      if (hr < 0 && !m_bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("CONVERT_CLIP"), hr);
         m_bErrorDisplayed = true;
      }
 

      // Now build the movie 
      if (hr >= 0 && !m_bCancelRequested)
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;
         
          
         m_strProgressLabel = g_Localizer.getLocalized("CREATE_FILE");
         m_progressManager.SetProgressLabel(m_strProgressLabel);
         m_progressManager.SetProgressStep("buildMovie");
         m_progressManager.SetCurrentSubProgress(0.0f);
         updateProgress();
         updateProgressLabel();

         aRemoveUponCancel.add(strTargetFilename);
         if (!bIsFlexPlayerExport)
         {
            String strPreloader = strTargetDir + LPLibs.flashGetPreloaderFilename();
            aRemoveUponCancel.add(strPreloader);
         }
         
         boolean bShowVideoOnPages = bIsStructured;
         
         hr = LPLibs.flashBuildMovie(iColorBasic, iColorBackground, 
                                     strAuthor, strTitle,
                                     iColorText, m_strLogoFileName, strLogoUrl, 
                                     ps.GetIntValue("iFlashTemplateType"), 
                                     bShowVideoOnPages);

         try { Thread.sleep(500); } catch (InterruptedException exc) {}
         m_progressManager.SetCurrentSubProgress(1.0f);
         updateProgress();
      }

      if (hr >= 0 && !m_bCancelRequested)
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;
         // For Bugfix 5302: 
         // "page too short" warning and Slidestar export
         // This can not be ignored because LMD and thumbnails include the "too short" pages
         if (hr == LPLibs.WARN_PAGE_TOO_SHORT && bIsSlidestarExport) {
            // Replace warning message by an error message
            hr = LPLibs.PAGE_TOO_SHORT;
         }
      }
      

      // Write Thumbnail images
      if (hr >= 0 && bIsSlidestarExport && !m_bCancelRequested)
      {
         hr = WriteSlidestarThumbs();
      }
      if (hr >= 0 && bIsFlexPlayerExport && !m_bCancelRequested)
      {
         hr = WriteFlexThumbs();
      }


      // New Flash Player export (optional)
      if (hr >= 0 && !m_bCancelRequested && bIsFlexPlayerExport)
      {
         // TODO(?) Extra progress step for ProgressManager
         // (This is a very fast step and does not need an extra progress step)
         String strBaseUrl = ""; // default: empty Base URL
         String strStreamUrl = ps.GetStringValue("strTargetUrlStreamingServer");
         boolean bStripFlvSuffix = true; // default: true
         boolean bDoAutostart = ps.GetBoolValue("bDoAutostart");

         aRemoveUponCancel.add(strTargetDir + "document.xml");
         aRemoveUponCancel.add(strTargetDir + "document.lmd");
         aRemoveUponCancel.add(strTargetDir + "document.cfg");
         
         boolean bShowVideoOnPages = bIsStructured;
         boolean bShowClipsOnPages = ps.GetBoolValue("bShowClipsOnSlides");
         boolean bShowClipsInVideoArea = !bShowClipsOnPages;
         hr = LPLibs.flashCreateFlexFiles(strBaseUrl, strStreamUrl, 
                                          bShowVideoOnPages, bShowClipsInVideoArea, 
                                          bStripFlvSuffix, bDoAutostart, bScormEnabled, 
                                          bUseOriginalSlideSize);
         
         try { Thread.sleep(500); }
         catch (InterruptedException exc) { }
      }

      // SLIDESTAR export (optional)
      String strDocumentName = m_strDocumentName;
      if (hr >= 0 && !m_bCancelRequested && bIsSlidestarExport)
      {
         m_strProgressLabel = g_Localizer.getLocalized("CREATE_SLIDESTAR");
         m_progressManager.SetProgressLabel(m_strProgressLabel);
         m_progressManager.SetProgressStep("createSlidestar");
         m_progressManager.SetCurrentSubProgress(0.0f);
         updateProgress();
         updateProgressLabel();

         hr = LPLibs.flashCreateSlidestar(m_sThumbFiles);
         try { Thread.sleep(500); }
         catch (InterruptedException exc) { }
         m_strLzpTarget = m_pathTargetFile + File.separator + m_strDocumentName + ".lzp";
		 
         if (hr >= 0 && !m_bCancelRequested)
         {
            m_strProgressLabel = g_Localizer.getLocalized("SLIDESTAR_FINISH");
            updateProgressLabel();
            m_progressManager.SetCurrentSubProgress(1.0f);
            updateProgress();

///            if (bDoUpload || bUseNetworkDrive)
///            {
///               // Check, if target LZP already exists and use a new name then
///               strDocumentName = StreamingMediaConverter.CheckTargetZipExists(strDocumentName, m_pwData, 
///                                                                             (WizardPanel)this, true);
///               if (!strDocumentName.equals(m_strDocumentName))
///               {
///                  moveFile(m_pathTargetFile + File.separator + m_strDocumentName + ".lzp" , 
///                           m_pathTargetFile + File.separator + strDocumentName + ".lzp");
///                  m_strLzpTarget = m_pathTargetFile + File.separator + strDocumentName + ".lzp";
///               }
///            }
///            System.out.println("Setting target file name to: " + strDocumentName + ".lzp");
         }
      }
            
      if (hr < 0 && !m_bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("CREATE_FILE"), hr);
         m_bErrorDisplayed = true;
      }
 

      // Prepare "Snippets" for HTML file Parser
      //StringBuffer keywordsBuffer = new StringBuffer(2048);
      Snippet[] snippets = null;
      HashMap keywordsMap = new HashMap();

      if (hr >= 0 && !m_bCancelRequested)
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;
         
         FlashSnippetCreator fsc = new FlashSnippetCreator();
         
         String hexRed = Integer.toHexString((iColorBackground & 0xff0000) >> 16);
         String hexGreen = Integer.toHexString((iColorBackground & 0xff00) >> 8);
         String hexBlue = Integer.toHexString((iColorBackground & 0xff));
         if (hexRed.length() < 2)
            hexRed = "0" + hexRed;
         if (hexGreen.length() < 2)
            hexGreen = "0" + hexGreen;
         if (hexBlue.length() < 2)
            hexBlue = "0" + hexBlue;
         
         fsc.createSnippets(m_pwData, bFixedSize, bMenuEnabled,
                            bScormEnabled, bLecIdentityHidden, 
                            hexRed + hexGreen + hexBlue, 
                            strTargetFilename,
                            m_dimOuterFlashSize);
         
         snippets = fsc.getSnippets();
         for (int i=0; i<snippets.length; ++i)
         {
            String kw = snippets[i].keyword;
            kw = kw.substring(1, kw.length() - 1);
            keywordsMap.put(kw, snippets[i].replace);
         }
      }


      // New Flash Player: Enable (or share resp.) the output directory
      if (hr >= 0 && !m_bCancelRequested && bIsFlexPlayerExport)
      {
         // Cut the 'content/' directory from the output path
         m_pathTargetFile = m_pathTargetFile.getParentFile();
         // Enable/Share the output directory
         LPLibs.flashEnableOutputDir("" + m_pathTargetFile + File.separatorChar);
      }


      // Parse and copy HTML files
      htmlFileList = new ArrayList(2);
      
      if (hr >= 0 && !m_bCancelRequested)
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;
         
         if (bCreateWebsite)
         {
            m_strProgressLabel = g_Localizer.getLocalized("CREATE_WEBSITE");
            m_progressManager.SetProgressLabel(m_strProgressLabel);
            m_progressManager.SetProgressStep("createWebsite");
            m_progressManager.SetCurrentSubProgress(0.0f);
            updateProgress();
            updateProgressLabel();
                        
            try
            {
               String defaultDir =  System.getProperty("install.path") +
                  File.separatorChar + "Publisher" + File.separatorChar +
                  "Flash" + File.separatorChar;
               String defaultCommonDir  = System.getProperty("install.path") +
                  File.separatorChar + "Publisher" + File.separatorChar +
                  "CommonFiles" + File.separatorChar;

                      
               // Parse and create 'index.html'
               TemplateParser templateParser;
               FileInputStream htmlFileIn;
               FileOutputStream htmlFileOut;
               String strHtmlTemplate;

               if (bIsFlexPlayerExport)
               {
                  // Reset 'strTargetDir' (cut 'content/' from the path)
                  strTargetDir = "" + m_pathTargetFile + File.separatorChar;
                  // Set the HTML Template name
                  if (bDoStartInOptimizedSize) // Special case: Parse the file "index.html.tmpl"
                      strHtmlTemplate = "" + Player2Dir + "index.html.tmpl";
                  else
                      strHtmlTemplate = "" + Player2Dir + "LecturnityFlashPlayer.html.tmpl";
               }
               else
               {
                  // Set the HTML Template name
                  strHtmlTemplate = "index.html.tmpl";
               }

               // Set the target HTML file name
               m_strHtmlTarget = strTargetDir + m_strDocumentName + ".html";
               String tmpHtmlTarget = m_strHtmlTarget;

               // Parse the HTML Template and create the 'index.html' ('<DocumentName>.html' for FlexPlayer)
               // Use the Target Document name as file name
               htmlFileIn = new FileInputStream(defaultDir + strHtmlTemplate);
               htmlFileOut = new FileOutputStream(tmpHtmlTarget);
               //System.out.println("Webseite: "+new File(m_pathTargetFile, m_strDocumentName+".html"));
               templateParser =
                  new TemplateParser(htmlFileIn, htmlFileOut, keywordsMap);
               aRemoveUponCancel.add(strTargetDir + m_strDocumentName + ".html");
               htmlFileList.add(strTargetDir+m_strDocumentName + ".html\n"+m_strDocumentName + ".html");
               templateParser.doParse();
               htmlFileIn.close();
               htmlFileOut.close();

               if (bIsFlexPlayerExport && bDoStartInOptimizedSize) {
                   // Special case: Parse "LecturnityFlashPlayer.html.tmpl" as an additional file 
                   strHtmlTemplate = "" + Player2Dir + "LecturnityFlashPlayer.html.tmpl";

                   // Parse the HTML Template and create '__content.html'
                   // Use the Target Document name as file name
                   htmlFileIn = new FileInputStream(defaultDir + strHtmlTemplate);
                   tmpHtmlTarget = strTargetDir + "__content.html";
                   htmlFileOut = new FileOutputStream(tmpHtmlTarget);
                   templateParser =
                      new TemplateParser(htmlFileIn, htmlFileOut, keywordsMap);
                   aRemoveUponCancel.add(strTargetDir + "__content.html");
                   htmlFileList.add(strTargetDir + "__content.html\n" + "__content.html");
                   templateParser.doParse();
                   htmlFileIn.close();
                   htmlFileOut.close();
               }

               // BUGFIX BLECPUB-1866: Add 'Mark of the Web' in the header of 'htmlFileOut'
               // (but not for PowerTrainer)
               if (!m_pwData.m_bIsPowerTrainerMode) {
                   File inFile = new File(tmpHtmlTarget);
                   // Use a temp file as output
                   File outFile = new File(strTargetDir + "$$$$$$$$.tmp");
                   htmlFileIn = new FileInputStream(inFile);
                   // Input
                   BufferedReader in = new BufferedReader(new InputStreamReader(htmlFileIn));
                   // Output
                   htmlFileOut = new FileOutputStream(outFile);
                   PrintWriter out = new PrintWriter(htmlFileOut);
                   // Insert the 'Mark of the Web' (MOTW) line
                   String strMotw = "<!-- saved from url=(0014)about:internet -->";
                   out.println(strMotw);
                   // Now copy the other lines
                   String strLine = "";
                   while ((strLine = in.readLine()) != null)
                       out.println(strLine);
                   out.flush();
                   out.close();
                   in.close();
                   // Replace the 'inFile' with the temp file
                   inFile.delete();
                   outFile.renameTo(inFile);
               }
               
               if (bIsFlexPlayerExport)
               {
                  // Copy the necessary files from the "player2" directory into the target directory 

                  // Copy 'LecturnityFlashPlayer.swf'
                  FileUtils.copyFile(defaultDir + Player2Dir + "LecturnityFlashPlayer.swf", strTargetDir + "LecturnityFlashPlayer.swf", this, "Copying...");
                  htmlFileList.add(strTargetDir + "LecturnityFlashPlayer.swf\n" + "LecturnityFlashPlayer.swf");
                  aRemoveUponCancel.add(strTargetDir + "LecturnityFlashPlayer.swf");        
                  // Copy 'LecturnityProxieAS2.swf'
                  FileUtils.copyFile(defaultDir + Player2Dir + "LecturnityProxieAS2.swf", strTargetDir + "LecturnityProxieAS2.swf", this, "Copying...");
                  htmlFileList.add(strTargetDir + "LecturnityProxieAS2.swf\n" + "LecturnityProxieAS2.swf");
                  aRemoveUponCancel.add(strTargetDir + "LecturnityProxieAS2.swf");        
                  // Copy 'playerProductInstall.swf'
                  FileUtils.copyFile(defaultDir + Player2Dir + "playerProductInstall.swf", strTargetDir + "playerProductInstall.swf", this, "Copying...");
                  htmlFileList.add(strTargetDir + "playerProductInstall.swf\n" + "playerProductInstall.swf");
                  aRemoveUponCancel.add(strTargetDir + "playerProductInstall.swf");        
                  // Copy 'AC_OETags.js'
                  FileUtils.copyFile(defaultDir + Player2Dir + "AC_OETags.js", strTargetDir + "AC_OETags.js", this, "Copying...");
                  htmlFileList.add(strTargetDir + "AC_OETags.js\n" + "AC_OETags.js");
                  aRemoveUponCancel.add(strTargetDir + "AC_OETags.js");        
                  if (!bScormEnabled) {
                     // Copy 'start.exe' (Bugfix 4855, 5204)
                     FileUtils.copyFile(defaultDir + Player2Dir + "start.exe", strTargetDir + "start.exe", this, "Copying...");
                     // 'start.exe' is not added to the HTML file list because it should only be used in the target directory 
                     // (and not on File/HTTP/Streaming server or inside SCORM ZIP packages)
                     //htmlFileList.add(strTargetDir + "start.exe\n" + "start.exe");
                     aRemoveUponCancel.add(strTargetDir + "start.exe");
                  }

                  // Add the "document.xml", "document.lmd" and "document.cfg" to the HTML file list
                  htmlFileList.add(strTargetDir + contentDir + "document.xml\n" + contentDir + "document.xml");
                  htmlFileList.add(strTargetDir + contentDir + "document.lmd\n" + contentDir + "document.lmd");
                  htmlFileList.add(strTargetDir + contentDir + "document.cfg\n" + contentDir + "document.cfg");

                  // Add the thumb images to the HTML file list
                  for (int i = 0; i < aThumbnailEntries.length; ++i)
                  {
                     String strImageName = aThumbnailEntries[i].strImageName;
                     String strFileName = strTargetDir + contentDir + strImageName + ".png";
                     htmlFileList.add(strFileName + "\n" + contentDir + strImageName + ".png");
                  }
                  
                  // Still image? --> Copy the still image to the target directory
                  if (dd.HasStillImage())
                  {
                     String strStillImageName = dd.GetStillImageName();     // File name only
                     String strStillImageFile = dd.GetStillImageName(true); // File with full path
                     FileUtils.copyFile(strStillImageFile, strTargetDir + contentDir + strStillImageName, this, "Copying...");
                     htmlFileList.add(strTargetDir + contentDir + strStillImageName + "\n" + strStillImageName);
                     aRemoveUponCancel.add(strTargetDir + contentDir + strStillImageName);        
                  }
               }

               // Add video/clips to 'htmlFileList' (if they exist)
               if (bHasSomeVideo || m_bCreateAudioFlv || bIsStructured)
               {
                  String fileName = new File(m_strFlvVideoName).getName();
                  if (bIsFlexPlayerExport)
                     htmlFileList.add(strTargetDir + contentDir+fileName + "\n"
                                      + contentDir + fileName);
                  else
                     htmlFileList.add(strTargetDir + fileName + "\n" + fileName);
               }
            
               if (bHasClips && !bIsDenver && !bIsStructured)
               {
                  for (int i = 0; i < iClipCount; ++i)
                  {
                     String fileName = new File(m_strFlvClipNames[i]).getName();
                     if (bIsFlexPlayerExport)
                        htmlFileList.add(strTargetDir + contentDir + fileName + "\n"
                                         + contentDir + fileName);
                     else
                        htmlFileList.add(strTargetDir + fileName + "\n" + fileName);
                  }
               }

               if (!bIsFlexPlayerExport)
               {
                  // Create the 'js' subfolder
                  new File(strTargetDir + JsDir).mkdirs();
                  aRemoveUponCancel.add(strTargetDir + JsDir);
                  // Parse and create 'flash.js'
                  FileInputStream jsFileIn = new FileInputStream(defaultDir + JsDir +"flash.js.tmpl");
                  FileOutputStream jsFileOut = new FileOutputStream(strTargetDir + JsDir +"flash.js");
                  templateParser =
                     new TemplateParser(jsFileIn, jsFileOut, keywordsMap);
                  aRemoveUponCancel.add(strTargetDir + JsDir + "flash.js");
                  htmlFileList.add(strTargetDir + JsDir + "flash.js\n" + JsDir + "flash.js");
                  templateParser.doParse();
                  jsFileIn.close();
                  jsFileOut.close();
               }
                  
               // SCORM ?
               if (bScormEnabled)
               {
                  if (bIsFlexPlayerExport)
                  {
                     // Create the 'js' subfolder
                     new File(strTargetDir + JsDir).mkdirs();
                     aRemoveUponCancel.add(strTargetDir + JsDir);
                  }
                  // ('js' subfolder already exists)
                  // Parse and create 'variables.js'
                  FileInputStream jsFileIn = new FileInputStream(defaultDir + JsDir + "variables.js.tmpl");
                  FileOutputStream jsFileOut = new FileOutputStream(strTargetDir + JsDir + "variables.js");
                  templateParser =
                  new TemplateParser(jsFileIn, jsFileOut, keywordsMap);
                  aRemoveUponCancel.add(strTargetDir + JsDir+ "variables.js");
                  htmlFileList.add(strTargetDir + JsDir + "variables.js\n" + JsDir + "variables.js");
                  templateParser.doParse();
                  jsFileIn.close();
                  jsFileOut.close();
           

                  // Now copy 'APIwrapper.js'     
                  FileUtils.copyFile(defaultCommonDir + "js" + File.separatorChar + "APIWrapper.js", strTargetDir + JsDir + "APIWrapper.js", this, "Copying...");
                  htmlFileList.add(strTargetDir + JsDir + "APIWrapper.js\n" + JsDir + "APIWrapper.js");
                  aRemoveUponCancel.add(strTargetDir + JsDir + "APIWrapper.js");        
                   
                  // Now copy 'scorm.js'           
                  FileUtils.copyFile(defaultDir + JsDir + "scorm.js", strTargetDir + JsDir +  "scorm.js", this, "Copying...");
                  htmlFileList.add(strTargetDir + JsDir + "scorm.js\n" + JsDir + "scorm.js");
                  aRemoveUponCancel.add(strTargetDir + JsDir + "scorm.js");
                   
                  // Parse and create 'scorm_utils.js'
                  jsFileIn = new FileInputStream(defaultDir + JsDir + "scorm_utils.js.tmpl");
                  jsFileOut = new FileOutputStream(strTargetDir + JsDir + "scorm_utils.js");
                  templateParser =
                  new TemplateParser(jsFileIn, jsFileOut, keywordsMap);
                  aRemoveUponCancel.add(strTargetDir + JsDir+ "scorm_utils.js");
                  htmlFileList.add(strTargetDir + JsDir + "scorm_utils.js\n" + JsDir + "scorm_utils.js");
                  templateParser.doParse();
                  jsFileIn.close();
                  jsFileOut.close();
              
                  // Now create the "common" folder and copy it containt
              
                  new File(strTargetDir + extendDir ).mkdirs();
                  aRemoveUponCancel.add(strTargetDir + extendDir);
                
                  FileUtils.copyFile(defaultCommonDir + "Extend" + File.separatorChar + "custom.xsd", strTargetDir + extendDir  + "custom.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + extendDir  + "custom.xsd\n" + extendDir  + "custom.xsd");
                  aRemoveUponCancel.add(strTargetDir + extendDir  + "custom.xsd");
                    
                  FileUtils.copyFile(defaultCommonDir + "Extend" + File.separatorChar + "strict.xsd", strTargetDir + extendDir  + "strict.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + extendDir  + "strict.xsd\n" + extendDir  + "strict.xsd");
                  aRemoveUponCancel.add(strTargetDir + extendDir  + "strict.xsd");
        
                                        
                  // Now create the "extend" folder and copy it containt
          
                  new File(strTargetDir + commonDir).mkdirs();
                  aRemoveUponCancel.add(strTargetDir + commonDir);
                  
                  FileUtils.copyFile(defaultCommonDir + "Common" + File.separatorChar + "anyElement.xsd", strTargetDir + commonDir + "anyElement.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + commonDir + "anyElement.xsd\n" + commonDir + "anyElement.xsd");
                  aRemoveUponCancel.add(strTargetDir + commonDir + "anyElement.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Common" + File.separatorChar + "dataTypes.xsd", strTargetDir + commonDir + "dataTypes.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + commonDir + "dataTypes.xsd\n" + commonDir + "dataTypes.xsd");
                  aRemoveUponCancel.add(strTargetDir + commonDir + "dataTypes.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Common" + File.separatorChar + "elementNames.xsd", strTargetDir + commonDir + "elementNames.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + commonDir + "elementNames.xsd\n" + commonDir + "elementNames.xsd");
                  aRemoveUponCancel.add(strTargetDir + commonDir + "elementNames.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Common" + File.separatorChar + "elementTypes.xsd", strTargetDir + commonDir + "elementTypes.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + commonDir + "elementTypes.xsd\n" + commonDir + "elementTypes.xsd");
                  aRemoveUponCancel.add(strTargetDir + commonDir + "elementTypes.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Common" + File.separatorChar + "rootElement.xsd", strTargetDir + commonDir + "rootElement.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + commonDir + "rootElement.xsd\n" + commonDir + "rootElement.xsd");
                  aRemoveUponCancel.add(strTargetDir + commonDir + "rootElement.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Common" + File.separatorChar + "vocabTypes.xsd", strTargetDir + commonDir + "vocabTypes.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + commonDir + "vocabTypes.xsd\n" + commonDir + "vocabTypes.xsd");
                  aRemoveUponCancel.add(strTargetDir + commonDir + "vocabTypes.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Common" + File.separatorChar + "vocabValues.xsd", strTargetDir + commonDir + "vocabValues.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + commonDir + "vocabValues.xsd\n" + commonDir + "vocabValues.xsd");
                  aRemoveUponCancel.add(strTargetDir + commonDir + "vocabValues.xsd");
                  
                  // Now copy the "scorm" containt
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "adlcp_v1p3.xsd", strTargetDir  + "adlcp_v1p3.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "adlcp_v1p3.xsd\nadlcp_v1p3.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "adlcp_v1p3.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "adlnav_v1p3.xsd", strTargetDir  + "adlnav_v1p3.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "adlnav_v1p3.xsd\nadlnav_v1p3.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "adlnav_v1p3.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "adlseq_v1p3.xsd", strTargetDir  + "adlseq_v1p3.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "adlseq_v1p3.xsd\nadlseq_v1p3.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "adlseq_v1p3.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "datatypes.dtd", strTargetDir  + "datatypes.dtd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "datatypes.dtd\ndatatypes.dtd");
                  aRemoveUponCancel.add(strTargetDir  + "datatypes.dtd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "imscp_v1p1.xsd", strTargetDir  + "imscp_v1p1.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "imscp_v1p1.xsd\nimscp_v1p1.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "imscp_v1p1.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "imsss_v1p0.xsd", strTargetDir  + "imsss_v1p0.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "imsss_v1p0.xsd\nimsss_v1p0.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "imsss_v1p0.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "imsss_v1p0auxresource.xsd", strTargetDir  + "imsss_v1p0auxresource.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "imsss_v1p0auxresource.xsd\nimsss_v1p0auxresource.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "imsss_v1p0auxresource.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" +File.separatorChar + "imsss_v1p0control.xsd", strTargetDir  + "imsss_v1p0control.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "imsss_v1p0control.xsd\nimsss_v1p0control.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "imsss_v1p0control.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "imsss_v1p0delivery.xsd", strTargetDir  + "imsss_v1p0delivery.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "imsss_v1p0delivery.xsd\nimsss_v1p0delivery.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "imsss_v1p0delivery.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "imsss_v1p0limit.xsd", strTargetDir  + "imsss_v1p0limit.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "imsss_v1p0limit.xsd\nimsss_v1p0limit.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "imsss_v1p0limit.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "imsss_v1p0objective.xsd", strTargetDir  + "imsss_v1p0objective.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "imsss_v1p0objective.xsd\nimsss_v1p0objective.xsd");
                  aRemoveUponCancel.add(strTargetDir + "imsss_v1p0objective.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "imsss_v1p0random.xsd", strTargetDir  + "imsss_v1p0random.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "imsss_v1p0random.xsd\nimsss_v1p0random.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "imsss_v1p0random.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "imsss_v1p0rollup.xsd", strTargetDir  + "imsss_v1p0rollup.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "imsss_v1p0rollup.xsd\nimsss_v1p0rollup.xsd");
                  aRemoveUponCancel.add(strTargetDir + "imsss_v1p0rollup.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "imsss_v1p0seqrule.xsd", strTargetDir  + "imsss_v1p0seqrule.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "imsss_v1p0seqrule.xsd\nimsss_v1p0seqrule.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "imsss_v1p0seqrule.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "imsss_v1p0util.xsd", strTargetDir  + "imsss_v1p0util.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "imsss_v1p0util.xsd\nimsss_v1p0util.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "imsss_v1p0util.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "lom.xsd", strTargetDir  + "lom.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "lom.xsd\nlom.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "lom.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "lomCustom.xsd", strTargetDir  + "lomCustom.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "lomCustom.xsd\nlomCustom.xsd");
                  aRemoveUponCancel.add(strTargetDir + "lomCustom.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "lomLoose.xsd", strTargetDir  + "lomLoose.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "lomLoose.xsd\nlomLoose.xsd");
                  aRemoveUponCancel.add(strTargetDir  + "lomLoose.xsd");
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "lomStrict.xsd", strTargetDir  + "lomStrict.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "lomStrict.xsd\nlomStrict.xsd");
                  aRemoveUponCancel.add(strTargetDir + "lomStrict.xsd");               
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "xml.xsd", strTargetDir  + "xml.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "xml.xsd\nxml.xsd");
                  aRemoveUponCancel.add(strTargetDir + "xml.xsd");        
                  
                  FileUtils.copyFile(defaultCommonDir + "Scorm" + File.separatorChar + "XMLSchema.dtd", strTargetDir  + "XMLSchema.dtd", this, "Copying...");
                  htmlFileList.add(strTargetDir  + "XMLSchema.dtd\nXMLSchema.dtd");
                  aRemoveUponCancel.add(strTargetDir  + "XMLSchema.dtd");        
                          
                  // Now create the "unique" folder and copy it containt
                  new File(strTargetDir + uniqueDir ).mkdirs();
                  aRemoveUponCancel.add(strTargetDir + uniqueDir);
                  
                  FileUtils.copyFile(defaultCommonDir + "Unique" + File.separatorChar + "loose.xsd", strTargetDir + uniqueDir  + "loose.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + uniqueDir  + "loose.xsd\n" + uniqueDir  + "loose.xsd");
                  aRemoveUponCancel.add(strTargetDir + uniqueDir  + "loose.xsd");        
                  
                  FileUtils.copyFile(defaultCommonDir + "Unique" + File.separatorChar + "strict.xsd", strTargetDir + uniqueDir  + "strict.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + uniqueDir  + "strict.xsd\n" + uniqueDir  + "strict.xsd");
                  aRemoveUponCancel.add(strTargetDir + uniqueDir  + "strict.xsd");
                                  
                  // Now create the "vocab" folder and copy it containt
                  new File(strTargetDir + vocabDir ).mkdirs();
                  aRemoveUponCancel.add(strTargetDir + vocabDir);
                  
                  FileUtils.copyFile(defaultCommonDir + "Vocab" + File.separatorChar + "adlmd_vocabv1p0.xsd", strTargetDir + vocabDir  + "adlmd_vocabv1p0.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + vocabDir  + "adlmd_vocabv1p0.xsd\n" + vocabDir  + "adlmd_vocabv1p0.xsd");
                  aRemoveUponCancel.add(strTargetDir + vocabDir  + "adlmd_vocabv1p0.xsd");        
                  
                  FileUtils.copyFile(defaultCommonDir + "Vocab" + File.separatorChar + "custom.xsd", strTargetDir + vocabDir  + "custom.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + vocabDir  + "custom.xsd\n" + vocabDir  + "custom.xsd");
                  aRemoveUponCancel.add(strTargetDir + vocabDir  + "custom.xsd");        
                  
                  FileUtils.copyFile(defaultCommonDir + "Vocab" + File.separatorChar + "loose.xsd", strTargetDir + vocabDir  + "loose.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + vocabDir  + "loose.xsd\n" + vocabDir  + "loose.xsd");
                  aRemoveUponCancel.add(strTargetDir + vocabDir  + "loose.xsd");        
                  
                  FileUtils.copyFile(defaultCommonDir + "Vocab" + File.separatorChar + "strict.xsd", strTargetDir + vocabDir  + "strict.xsd", this, "Copying...");
                  htmlFileList.add(strTargetDir + vocabDir  + "strict.xsd\n" + vocabDir  + "strict.xsd");
                  aRemoveUponCancel.add(strTargetDir + vocabDir  + "strict.xsd");        
               }
            }
            catch (IOException exc)
            {
               exc.printStackTrace();
               hr = -1;
            }
            
            try { Thread.sleep(500); } catch (InterruptedException exc) {}
         } // if (m_bCreateWebsite)
         m_progressManager.SetCurrentSubProgress(1.0f);
         updateProgress();
      }
      
      if (hr < 0 && !m_bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("CREATE_WEBSITE"), hr);
         m_bErrorDisplayed = true;
      }


      // Prepare SCORM
      String strScormZipFile = "";
      if (hr >= 0 && !m_bCancelRequested && bScormEnabled)
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;
         
         m_strProgressLabel = g_Localizer.getLocalized("CREATE_SCORM");
         m_progressManager.SetProgressLabel(m_strProgressLabel);
         m_progressManager.SetProgressStep("createScorm");
         m_progressManager.SetCurrentSubProgress(0.0f);
         updateProgress();
         updateProgressLabel();

         LPLibs.scormInit();
         
///         strDocumentName = StreamingMediaConverter.CheckTargetZipExists(strDocumentName, m_pwData, 
///                                                                       (WizardPanel)this);
         strScormZipFile = strTargetDir + strDocumentName + ".zip";
///         System.out.println("Setting target file name to: " + strScormZipFile);

         String strScormSettings = m_pwData.GetProfiledSettings().GetStringValue("strScormSettings");
         
         if (strScormSettings != null && strScormSettings.length() > 0)
            LPLibs.scormUseSettings(strScormSettings); // ??? error ignored (hr overwritten below)

         // Then prepare the rest
         for (int i=0; i<snippets.length; ++i)
            LPLibs.scormAddGeneralKeyword(snippets[i].keyword, snippets[i].replace);
         
         if (metadata != null)
         {
            if (metadata.keywords != null)
            {
               String keywords[] = metadata.keywords;
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

         // Add the full text keywords to the SCORM manifest.
         // This is not done directly by "scormAddFullTextString" (as in RM-/WM-Export).
         // "flashPrepareScorm" does that for us.
         if (LPLibs.scormGetFullTextAsKeywords())
            LPLibs.flashPrepareScorm();

         // Set the index file name
         LPLibs.scormSetIndexFileName(strTargetDir + m_strDocumentName + ".html", m_strDocumentName + ".html");
         // Add the HTML file list
         for (int i=1; i<htmlFileList.size(); ++i)
         {
            StringTokenizer st = new StringTokenizer( ((String) htmlFileList.get(i)), "\n");
            LPLibs.scormAddResourceFile(st.nextToken(),st.nextToken());
         }
         // Add the Flash SWF file
         if (bIsFlexPlayerExport)
            LPLibs.scormAddResourceFile(strTargetDir + contentDir + m_strDocumentName + ".swf", 
                                        contentDir + m_strDocumentName + ".swf");
         else
            LPLibs.scormAddResourceFile(strTargetDir + m_strDocumentName + ".swf", m_strDocumentName + ".swf");
         // Add the Preloader
         if (!bIsFlexPlayerExport)
            LPLibs.scormAddResourceFile(strTargetDir + LPLibs.flashGetPreloaderFilename(), LPLibs.flashGetPreloaderFilename());
         // Set the SCORM target file name
         LPLibs.scormSetTargetFileName(m_strDocumentName);

         // Create a temporary directory and create the ZIP package there
         hr = LPLibs.scormCreatePackage(getFrame());

         if (hr >= 0)
         {
            // Get the path & filename of the temporary ZIP package
            String resultZip = LPLibs.scormGetTargetFilePath();

            if (m_pwData.m_bIsPowerTrainerMode) {
               // Get the 'imsmanifest.xml' instead of '<targetfile>.zip' 
               // from the temporary directory
               String strManifest = "imsmanifest.xml";
               String source = FileUtils.extractPathFromPathFile(resultZip);
               String target = strTargetDir;

               try
               {
                  // Copy the 'imsmanifest.xml' from the temporary directory to the target path
                  FileUtils.copyFile((source+strManifest), (target+strManifest), this, "Copying file...");
               }
               catch (IOException e)
               {
                  hr = -1;
               }
            } else {
               try
               {
                  // Copy the ZIP package from the temporary directory to the target path
                  FileUtils.copyFile(resultZip, strScormZipFile, this, "Copying package...");
               }
               catch (IOException e)
               {
                  hr = -1;
               }
            }
         }

         // Remove temporary directory and its content
         LPLibs.scormCleanUp();

         if (!m_pwData.m_bIsPowerTrainerMode) {
            // Remove all the other files...
            File file;
            for (int i=0; i<htmlFileList.size(); ++i)
            {
               StringTokenizer st = new StringTokenizer( ((String) htmlFileList.get(i)), "\n");
               file = new File((String) st.nextToken());
               file.delete();
            }
            // Remove SWF file
            if (bIsFlexPlayerExport)
               file = new File(strTargetDir + contentDir + m_strDocumentName + ".swf");
            else
               file = new File(strTargetDir + m_strDocumentName + ".swf");
            file.delete();
            // Remove Preloader SWF file
            if (!bIsFlexPlayerExport)
            {
               file = new File(strTargetDir + LPLibs.flashGetPreloaderFilename());
               file.delete();
            }

            try { Thread.sleep(500); } catch (InterruptedException exc) {}

            // Delete the created folders
            file = new File(strTargetDir + JsDir);
            file.delete();

            file = new File(strTargetDir + commonDir);
            file.delete();
            file = new File(strTargetDir + extendDir);
            file.delete();
            file = new File(strTargetDir + scormDir);
            file.delete();
            file = new File(strTargetDir + uniqueDir);
            file.delete();            
            file = new File(strTargetDir + vocabDir);
            file.delete();        
         
            //  New Flex Player: The 'content' dir still contains temporary files
            // --> Delete this folder after the temporary files have been removed
         }
         
         m_progressManager.SetCurrentSubProgress(1.0f);
         updateProgress();
      }

      if (hr > 0)
         m_hrPossibleWarning = hr;
         

      // Special case: SCORM and SLIDESTAR: In the end there is only one ZIP or LZP file resp.
      // Rename the local target document, if necessary
      String strNewDocumentName = strDocumentName;
      if (hr >= 0 && bTargetIsZipOrLzp && !m_bCancelRequested)
      {
         if (bScormEnabled)
         {
            strDocumentName = FileUtils.extractNameFromPathFile(strScormZipFile);
            strNewDocumentName = StreamingMediaConverter.CheckTargetZipExists(strDocumentName, m_pwData, 
                                                                                     (WizardPanel)this);
            if (!strNewDocumentName.equals(strDocumentName))
            {
               moveFile(strTargetDir + strDocumentName + ".zip" , 
                        strTargetDir + strNewDocumentName + ".zip");
            }
            strScormZipFile = strTargetDir + strNewDocumentName + ".zip";
         }
		 else if (bIsSlidestarExport)
         {
            strDocumentName = FileUtils.extractNameFromPathFile(m_strLzpTarget);
            strNewDocumentName = StreamingMediaConverter.CheckTargetZipExists(strDocumentName, m_pwData, 
                                                                                     (WizardPanel)this, true);
            if (!strNewDocumentName.equals(strDocumentName))
            {
               moveFile(strTargetDir + strDocumentName + ".lzp" , 
                        strTargetDir + strNewDocumentName + ".lzp");
            }
            m_strLzpTarget = strTargetDir + strNewDocumentName + ".lzp";
         }
      }


      // Streaming Server or Upload/Network Drive? --> Copy files to external Server
      if (hr >= 0 && !m_bCancelRequested && (bUseStreamingServer || bDoUpload || bUseNetworkDrive))
      {
         // Create and fill file lists:
         // - Two file list for non-video files 
         //   (one for "normal" files and one for files in subdirectories, e.g. "js/" or "content/")
         // - One file list for video files
         //   (they have to be separated in case of streaming server)
         ArrayList aNormalFiles = new ArrayList();
         ArrayList aSubdirFiles = new ArrayList();
         ArrayList aVideoFiles = new ArrayList();

         // Different non-video files for old and new Flash Player
         if (bIsFlexPlayerExport)
         {
            // Main "HTML"
            aNormalFiles.add(m_strHtmlTarget);
            // 'LecturnityFlashPlayer.swf'
            aNormalFiles.add(strTargetDir + "LecturnityFlashPlayer.swf");
            // 'LecturnityProxieAS2.swf'
            aNormalFiles.add(strTargetDir + "LecturnityProxieAS2.swf");
            // 'playerProductInstall.swf'
            aNormalFiles.add(strTargetDir + "playerProductInstall.swf");
            // 'AC_OETags.js'
            aNormalFiles.add(strTargetDir + "AC_OETags.js");
            // 'document.xml' in 'content/'
            aSubdirFiles.add(strTargetDir + contentDir + "document.xml");
            // 'document.lmd' in 'content/'
            aSubdirFiles.add(strTargetDir + contentDir + "document.lmd");
            // 'document.cfg' in 'content/'
            aSubdirFiles.add(strTargetDir + contentDir + "document.cfg");
            // Main "SWF" in 'content/'
            aSubdirFiles.add(strTargetFilename);
            // All thumb images in 'content/'
            for (int i = 0; i < aThumbnailEntries.length; ++i)
               aSubdirFiles.add(strTargetDir + contentDir + aThumbnailEntries[i].strImageName + ".png");
         }
         else // "Old" Flash Player
         {
            // Main "SWF"
            aNormalFiles.add(strTargetFilename);
            // Preloader
            aNormalFiles.add(strTargetDir + LPLibs.flashGetPreloaderFilename());
            if (bCreateWebsite)
            {
               // Main "HTML"
               aNormalFiles.add(m_strHtmlTarget);
               // 'flash.js' in 'js/'
               aSubdirFiles.add(strTargetDir + JsDir + "flash.js");
            }
         }
         // Do we have to consider interactive opened (non-video) files?
         String strEvqFileName = dd.GetPresentationPath() + dd.GetEvqFileName();
         String strObjFileName = dd.GetPresentationPath() + dd.GetObjFileName();
         m_pwData.m_aListOfInteractiveOpenedFiles
            = LPLibs.getListOfInteractiveOpenedFiles(new File(strEvqFileName)+"", 
                                                     new File(strObjFileName)+"");
         if (m_pwData.m_aListOfInteractiveOpenedFiles != null)
         {
            int len = m_pwData.m_aListOfInteractiveOpenedFiles.length;
            for (int i = 0; i < len; ++i)
            {
               String strPath = ps.GetStringValue("strTargetDirLocal");
               aNormalFiles.add(strPath + m_pwData.m_aListOfInteractiveOpenedFiles[i]);
            }
         }
         // Video files
         String subdir = bIsFlexPlayerExport ? contentDir : "";
         if (bHasSomeVideo || m_bCreateAudioFlv || bIsStructured)
            aVideoFiles.add(strTargetDir + subdir + new File(m_strFlvVideoName).getName());
         if (bHasClips && !bIsDenver && !bIsStructured)
         {
            for (int i = 0; i < iClipCount; ++i)
               aVideoFiles.add(strTargetDir + subdir + new File(m_strFlvClipNames[i]).getName());
         }


         // Upload or Connected network drive?
         if (bDoUpload)
         {
            int iResult = 0;
            double dProgress = 0.0;
            float fCurrentProgress = 0.0f;
            int iWaitMs = 500;
            float fMaxKb = 0.0f;

            if (bScormEnabled || bIsSlidestarExport)
            {
               m_strProgressLabel = g_Localizer.getLocalized("TRANSFERRING_FILES");
               m_progressManager.SetProgressLabel(m_strProgressLabel);
               m_progressManager.SetProgressStep("doUploadCopy");
               m_progressManager.SetCurrentSubProgress(0.0f);
               updateProgress();
               updateProgressLabel();

               String[] aFilesPlain = null;
               if (bScormEnabled)
                  aFilesPlain = new String[] {strScormZipFile};
               else if (bIsSlidestarExport)
                  aFilesPlain = new String[] {m_strLzpTarget};

               int iSizeKb = FileUtils.getSizeKbOfFileList(aFilesPlain);
               fMaxKb = (float)iSizeKb;

               if ((aFilesPlain != null) && (aFilesPlain.length > 0))
                  iResult = UploaderAccess.StartUpload(
                     true, ps, aFilesPlain, this);

               while (dProgress < 100.0 && iResult == 0)
               {
                  dProgress = UploaderAccess.GetUploadProgress(5);

                  if (m_bCancelRequested)
                  {
                     UploaderAccess.CancelSession();
                     dProgress = 100.0;
                     fCurrentProgress = (iSizeKb/fMaxKb);
                     m_progressManager.SetCurrentSubProgress(fCurrentProgress);
                     updateProgress();
                     break;
                  }

                  fCurrentProgress = (iSizeKb/fMaxKb) * (float)dProgress/100.f;
                  m_progressManager.SetCurrentSubProgress(fCurrentProgress);
                  updateProgress();
                  try { Thread.sleep(iWaitMs); } catch (Exception exc) { }
               }

               if (iResult >= 0 && !strNewDocumentName.equals(strDocumentName))
               {
                  // Rename the local target document
                  if (bScormEnabled)
                  {
                     moveFile(strTargetDir + strNewDocumentName + ".zip" , 
                              strTargetDir + strDocumentName + ".zip");
                  }
                  else
                  {
                     moveFile(strTargetDir + strNewDocumentName + ".lzp" , 
                              strTargetDir + strDocumentName + ".lzp");
                  }
               }

               if (iResult != 0)
               {                  
                  if (iResult > 0)
                  {
                     // "Cancel" was pressed
                     m_bCancelRequested = true;
                  }
                  else
                  {
                     System.out.println("!!! Upload error in FlashWriterWizardPanel::displayWizard()2!");

                     // Finish Uploader Session
                     UploaderAccess.FinishSession();
                     m_pwData.m_bIsUploadSessionRunning = false;
                     
                     // Get the error text from the error code (= iResult) 
                     String strError = "";
                     try {
                        Localizer l = new Localizer(
                           "/imc/lecturnity/converter/wizard/TargetUploadWizardPanel_", "en");
                        strError = l.getLocalized("" + iResult);
                     } catch (IOException exc) {
                        // should not happen
                        exc.printStackTrace();
                     }
                     // Inform the ProgressManager
                     m_progressManager.SendErrorMessage(strError);

                     if( m_pwData.m_bIsPowerTrainerMode && (m_dptCommunicator != null) ) {
                        // Put the error text to the DPT Communicator
                        if (strError.length() > 0) {
                            m_dptCommunicator.ShowErrorMessage(strError);
                            m_bErrorDisplayed = true;
                        }
                     } else {
                        int res = UploaderAccess.ShowErrorMessageForErrorCode(iResult, (WizardPanel)this);
                        m_bErrorDisplayed = true;

                        if (res == 0) // "Verify Settings"
                        {
                           m_pwData.B_GOTO_START_DIALOG = true;

                           doNext();
                           return BUTTON_NEXT;
                        }
                        else if (res == 1) // "Retry"
                        {
                           return displayWizard();
                        }
                        else // "Cancel" or Generic error
                        {
                           // do nothing special, process is aborted
                        }
                     }
                  }
               }
            }
            else // No SCORM/LZP, "normal" files
            {
               if (!bUseStreamingServer)
               {
                  if (bIsFlexPlayerExport)
                     aSubdirFiles.addAll(aVideoFiles);
                  else
                     aNormalFiles.addAll(aVideoFiles);
               }
               
               int iNumNormalFiles = aNormalFiles.size();
               int iNumSubdirFiles = aSubdirFiles.size();
               int iNumVideoFiles = aVideoFiles.size();
   
               String[] aNormalFilesPlain = new String[iNumNormalFiles];
               aNormalFiles.toArray(aNormalFilesPlain);
               String[] aSubdirFilesPlain = new String[iNumSubdirFiles];
               aSubdirFiles.toArray(aSubdirFilesPlain);
               String[] aVideoFilesPlain = new String[iNumVideoFiles];
               aVideoFiles.toArray(aVideoFilesPlain);

               int iSizeKbNormal = FileUtils.getSizeKbOfFileList(aNormalFilesPlain);
               int iSizeKbSubdir = FileUtils.getSizeKbOfFileList(aSubdirFilesPlain);
               if (bCreateWebsite && (iSizeKbSubdir < 10))
                  iSizeKbSubdir = 10; // Additional 10 KB for "flash.js" (even if it has only 1 KB)
               int iSizeKbVideo = FileUtils.getSizeKbOfFileList(aVideoFilesPlain);

               int iSizeKbAll = iSizeKbNormal + iSizeKbSubdir;
               if (bUseStreamingServer)
                  iSizeKbAll += iSizeKbVideo;

               m_strProgressLabel = g_Localizer.getLocalized("TRANSFERRING_FILES");
               m_progressManager.SetProgressLabel(m_strProgressLabel);
               m_progressManager.SetProgressStep("doUploadCopy");
               m_progressManager.SetCurrentSubProgress(0.0f);
               updateProgress();
               updateProgressLabel();

               fMaxKb = iSizeKbAll;

               // Upload "normal" files
               if ((aNormalFilesPlain != null) && (aNormalFilesPlain.length > 0))
                  iResult = UploaderAccess.StartUpload(
                     true, ps, aNormalFilesPlain, this);

               dProgress = 0.0;
               while (dProgress < 100.0 && iResult == 0)
               {
                  dProgress = UploaderAccess.GetUploadProgress(5);

                  if (m_bCancelRequested)
                  {
                     UploaderAccess.CancelSession();
                     dProgress = 100.0;
                     fCurrentProgress = (iSizeKbNormal/fMaxKb);
                     m_progressManager.SetCurrentSubProgress(fCurrentProgress);
                     updateProgress();
                     break;
                  }

                  fCurrentProgress = (iSizeKbNormal/fMaxKb) * (float)dProgress/100.0f;
                  m_progressManager.SetCurrentSubProgress(fCurrentProgress);
                  updateProgress();
                  try { Thread.sleep(iWaitMs); } catch (Exception exc) { }
               }
               
               // Upload "Subdir" files
               if ((iNumSubdirFiles > 0) && (iResult == 0) && !m_bCancelRequested)
               {
                  // Empirical waiting time before another upload can start
                  try { Thread.sleep(1500); } catch (Exception exc) { }

                  subdir = (bIsFlexPlayerExport ? "content/" : "js/");

                  if ((aSubdirFilesPlain != null) && (aSubdirFilesPlain.length > 0))
                     iResult = UploaderAccess.StartUpload(
                        true, ps, aSubdirFilesPlain, this, subdir);
                  
                  dProgress = 0.0;
                  while (dProgress < 100.0 && iResult == 0)
                  {
                     dProgress = UploaderAccess.GetUploadProgress(5);

                     if (m_bCancelRequested)
                     {
                        UploaderAccess.CancelSession();
                        dProgress = 100.0;
                        fCurrentProgress = ((iSizeKbNormal+iSizeKbSubdir)/fMaxKb);
                        m_progressManager.SetCurrentSubProgress(fCurrentProgress);
                        updateProgress();
                        break;
                     }

                     fCurrentProgress = (iSizeKbNormal/fMaxKb) 
                                        + ((iSizeKbSubdir/fMaxKb) * (float)dProgress/100.0f);
                     m_progressManager.SetCurrentSubProgress(fCurrentProgress);
                     updateProgress();
                     try { Thread.sleep(iWaitMs); } catch (Exception exc) { }
                  }
               }

               if (iResult != 0)
               {                  
                  if (iResult > 0)
                  {
                     // "Cancel" was pressed
                     m_bCancelRequested = true;
                  }
                  else
                  {
                     System.out.println("!!! Upload error in FlashWriterWizardPanel::displayWizard()3!");

                     // Finish Uploader Session
                     UploaderAccess.FinishSession();
                     m_pwData.m_bIsUploadSessionRunning = false;

                     // Get the error text from the error code (= iResult) 
                     String strError = "";
                     try {
                        Localizer l = new Localizer(
                           "/imc/lecturnity/converter/wizard/TargetUploadWizardPanel_", "en");
                        strError = l.getLocalized("" + iResult);
                     } catch (IOException exc) {
                        // should not happen
                        exc.printStackTrace();
                     }
                     // Inform the ProgressManager
                     m_progressManager.SendErrorMessage(strError);

                     if( m_pwData.m_bIsPowerTrainerMode && (m_dptCommunicator != null) ) {
                        // Put the error text to the DPT Communicator
                        if (strError.length() > 0) {
                           m_dptCommunicator.ShowErrorMessage(strError);
                           m_bErrorDisplayed = true;
                        }
                     } else {
                        int res = UploaderAccess.ShowErrorMessageForErrorCode(iResult, (WizardPanel)this);
                        m_bErrorDisplayed = true;

                        if (res == 0) // "Verify Settings"
                        {
                           m_pwData.B_GOTO_START_DIALOG = true;

                           doNext();
                           return BUTTON_NEXT;
                        }
                        else if (res == 1) // "Retry"
                        {
                           return displayWizard();
                        }
                        else // "Cancel" or Generic error
                        {
                           // do nothing special, process is aborted
                        }
                     }
                  }
               }
   
               if (bUseStreamingServer && iResult == 0 && !m_bCancelRequested)
               {
                  // Empirical waiting time before another upload can start
                  try { Thread.sleep(1500); } catch (Exception exc) { }

                  //m_strProgressLabel = g_Localizer.getLocalized("COPYING_TO_SERVER");
                  m_strProgressLabel = g_Localizer.getLocalized("TRANSFERRING_FILES");
                  m_progressManager.SetProgressLabel(m_strProgressLabel);
                  m_progressManager.SetProgressStep("doStreamingServerCopy");
                  m_progressManager.SetCurrentSubProgress(0.0f);
                  updateProgress();
                  updateProgressLabel();
   
                  if ((aVideoFilesPlain != null) && (aVideoFilesPlain.length > 0))
                     iResult = UploaderAccess.StartUpload(
                        false, ps, aVideoFilesPlain, this);
                  
                  dProgress = 0.0;
                  while (dProgress < 100.0 && iResult == 0)
                  {
                     dProgress = UploaderAccess.GetUploadProgress(5);

                     if (m_bCancelRequested)
                     {
                        UploaderAccess.CancelSession();
                        dProgress = 100.0;
                        fCurrentProgress = ((iSizeKbNormal+iSizeKbSubdir+iSizeKbVideo)/fMaxKb);
                        m_progressManager.SetCurrentSubProgress(fCurrentProgress);
                        updateProgress();
                        break;
                     }

                     fCurrentProgress = ((iSizeKbNormal+iSizeKbSubdir)/fMaxKb) 
                                        + ((iSizeKbVideo/fMaxKb) * (float)dProgress/100.0f);
                     m_progressManager.SetCurrentSubProgress(fCurrentProgress);
                     updateProgress();
                     try { Thread.sleep(iWaitMs); } catch (Exception exc) { }
                  }
   
                  if (iResult != 0)
                  {                  
                     if (iResult > 0)
                     {
                        // "Cancel" was pressed
                        m_bCancelRequested = true;
                     }
                     else
                     {
                        System.out.println("!!! Upload error in FlashWriterWizardPanel::displayWizard()4!");

                        // Finish Uploader Session
                        UploaderAccess.FinishSession();
                        m_pwData.m_bIsUploadSessionRunning = false;

                        // Get the error text from the error code (= iResult) 
                        String strError = "";
                        try {
                           Localizer l = new Localizer(
                              "/imc/lecturnity/converter/wizard/TargetUploadWizardPanel_", "en");
                           strError = l.getLocalized("" + iResult);
                        } catch (IOException exc) {
                           // should not happen
                           exc.printStackTrace();
                        }
                        // Inform the ProgressManager
                        m_progressManager.SendErrorMessage(strError);

                        if( m_pwData.m_bIsPowerTrainerMode && (m_dptCommunicator != null) ) {
                           // Put the error text to the DPT Communicator
                           if (strError.length() > 0) {
                              m_dptCommunicator.ShowErrorMessage(strError);
                              m_bErrorDisplayed = true;
                           }
                        } else {
                           int res = UploaderAccess.ShowErrorMessageForErrorCode(iResult, (WizardPanel)this);
                           m_bErrorDisplayed = true;

                           if (res == 0) // "Verify Settings"
                           {
                              m_pwData.B_GOTO_START_DIALOG = true;

                              doNext();
                              return BUTTON_NEXT;
                           }
                           else if (res == 1) // "Retry"
                           {
                              return displayWizard();
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
            
            // Finish Uploader Session
            UploaderAccess.FinishSession();
            m_pwData.m_bIsUploadSessionRunning = false;
         }
         else if (bUseNetworkDrive)
         {
            String strHttpTargetDir = ps.GetStringValue("strTargetDirHttpServer");
            //strStreamingTargetDir = ps.GetStringValue("strTargetDirStreamingServer");

            if (bScormEnabled || bIsSlidestarExport)
            {
               // Copy ZIP/LZP to target network drive
               m_strProgressLabel = g_Localizer.getLocalized("TRANSFERRING_FILES");
               m_progressManager.SetProgressLabel(m_strProgressLabel);
               m_progressManager.SetProgressStep("doNetworkDriveCopy");
               m_progressManager.SetCurrentSubProgress(0.0f);
               updateProgress();
               updateProgressLabel();

               String strArchiveFile = "";
               if (bScormEnabled)
                  strArchiveFile = new File(strScormZipFile).getName();
               else if (bIsSlidestarExport)
                  strArchiveFile = new File(m_strLzpTarget).getName();
                  
               copyFile(strTargetDir + strArchiveFile, strHttpTargetDir + strArchiveFile);

               if (!strNewDocumentName.equals(strDocumentName))
               {
                  // Rename the local target document
                  if (bScormEnabled)
                  {
                     moveFile(strTargetDir + strNewDocumentName + ".zip" , 
                              strTargetDir + strDocumentName + ".zip");
                  }
                  else
                  {
                     moveFile(strTargetDir + strNewDocumentName + ".lzp" , 
                              strTargetDir + strDocumentName + ".lzp");
                  }
               }
            }
            else // No SCORM/LZP, "normal" files
            {
               // Copy files to target network drive(s)
               m_strProgressLabel = g_Localizer.getLocalized("TRANSFERRING_FILES");
               m_progressManager.SetProgressLabel(m_strProgressLabel);
               m_progressManager.SetProgressStep("doNetworkDriveCopy");
               m_progressManager.SetCurrentSubProgress(0.0f);
               updateProgress();
               updateProgressLabel();

               String strSource;

               int iNumNormalFiles = aNormalFiles.size();
               int iNumSubdirFiles = aSubdirFiles.size();
               int iNumVideoFiles = aVideoFiles.size();

               String[] aNormalFilesPlain = new String[iNumNormalFiles];
               aNormalFiles.toArray(aNormalFilesPlain);
               String[] aSubdirFilesPlain = new String[iNumSubdirFiles];
               aSubdirFiles.toArray(aSubdirFilesPlain);
               String[] aVideoFilesPlain = new String[iNumVideoFiles];
               aVideoFiles.toArray(aVideoFilesPlain);

               // Copy "normal" files
               for (int i = 0; i < aNormalFilesPlain.length; ++i)
               {
                  strSource = aNormalFilesPlain[i];
                  copyFile(strSource, strHttpTargetDir + new File(strSource).getName());
               }

               // Copy "subdir" files
               for (int i = 0; i < aSubdirFilesPlain.length; ++i)
               {
                  subdir = (bIsFlexPlayerExport ? ("content" + File.separator) : ("js" + File.separator));
                  // Create subdir, if it does not exist
                  File subdirFile = new File(strHttpTargetDir + subdir);
                  if (!subdirFile.isDirectory())
                     subdirFile.mkdirs();
                  strSource = aSubdirFilesPlain[i];
                  copyFile(strSource, strHttpTargetDir + subdir + new File(strSource).getName());
               }

               if (!bUseStreamingServer)
                  strStreamingTargetDir = strHttpTargetDir;

               // Copy video files
               for (int i = 0; i < aVideoFilesPlain.length; ++i)
               {
                  subdir = (bIsFlexPlayerExport ? ("content" + File.separator) : (""));
                  if ((subdir.length() > 0) && !bUseStreamingServer)
                  {
                     // Create subdir, if it does not exist
                     File subdirFile = new File(strStreamingTargetDir + subdir);
                     if (!subdirFile.isDirectory())
                        subdirFile.mkdirs();
                  }
                  strSource = new File(aVideoFilesPlain[i]).getName();
                  copyFile(aVideoFilesPlain[i], 
                           strStreamingTargetDir + (bUseStreamingServer ? "" : subdir) + new File(strSource).getName());
               }
            }
         }
         else if (!strStreamingTargetDir.equals(strTargetDir))
         {
            // Copy video files to Streaming server
            m_strProgressLabel = g_Localizer.getLocalized("TRANSFERRING_FILES");
            m_progressManager.SetProgressLabel(m_strProgressLabel);
            m_progressManager.SetProgressStep("doStreamingServerCopy");
            m_progressManager.SetCurrentSubProgress(0.0f);
            updateProgress();
            updateProgressLabel();

            String strSourceDir = strTargetDir;
         
            if (!strStreamingTargetDir.endsWith(File.separator))
               strStreamingTargetDir += File.separatorChar;
            String fileName; 
 
            int iNumVideoFiles = aVideoFiles.size();
            String[] aVideoFilesPlain = new String[iNumVideoFiles];
            aVideoFiles.toArray(aVideoFilesPlain);

            for (int i = 0; i < aVideoFilesPlain.length; ++i)
            {
               subdir = (bIsFlexPlayerExport ? ("content" + File.separator) : (""));
               fileName = new File(aVideoFilesPlain[i]).getName();
               ///moveFile(aVideoFilesPlain[i], strStreamingTargetDir + fileName);
               copyFile(aVideoFilesPlain[i], strStreamingTargetDir + fileName);
            }
         }

         m_progressManager.SetCurrentSubProgress(1.0f);
         updateProgress();
      }
      
      
      // Cleanup ...
      LPLibs.flashCleanup();
      
         
      if (strMp3FileName != null && new File(strMp3FileName).exists())
         new File(strMp3FileName).delete();
      
      if (m_strLogoFileName != null && new File(m_strLogoFileName).exists())
          new File(m_strLogoFileName).delete();
      
      
      if (m_bCancelRequested)
      {
     //    System.out.println("Cancel request ...   ");
         if (aRemoveUponCancel.size() > 0)
         {
         System.out.println("Deleting  "+aRemoveUponCancel.size()+ " files and folders ...   ");
            for (int i=aRemoveUponCancel.size()-1; i>=0 ; --i)
            {
             //     System.out.println("Deleting ...   "+(String)aRemoveUponCancel.get(i));
               File f = new File((String)aRemoveUponCancel.get(i));
               f.delete();
            }
         }
         
         // Inform the ProgressManager
         m_progressManager.SendErrorMessage(g_Localizer.getLocalized("CONVERSION_CANCELLED"));
         if( m_pwData.m_bIsPowerTrainerMode && (m_dptCommunicator != null) ) {
            // should probably not happen because cancel button is not visible
            m_dptCommunicator.ShowErrorMessage(g_Localizer.getLocalized("CONVERSION_CANCELLED"));
         } else {
            JOptionPane.showMessageDialog(this, 
                                          g_Localizer.getLocalized("CONVERSION_CANCELLED"), 
                                          g_Localizer.getLocalized("INFORMATION"),
                                          JOptionPane.INFORMATION_MESSAGE);
         }

         if (NativeUtils.isLibraryLoaded())
            setEnableButton(BUTTON_CUSTOM, false);
      }
      else if (hr >= 0)
      {
         if (m_hrPossibleWarning > 0)
         {
            String sWarningText = null;
            
            int hrFlags = 1;
            while (m_hrPossibleWarning >= hrFlags)
            {
               if ((m_hrPossibleWarning & hrFlags) != 0)
               {
                  sWarningText = "";

                  try
                  {
                     sWarningText = g_Localizer.getLocalized(hrFlags+""); 
                  }
                  catch (Exception exc)
                  {
                     // maybe this warning code is not specified
                  }
            
                  if (sWarningText == null || sWarningText.length() == 0)
                     sWarningText = g_Localizer.getLocalized("0") + " ("+m_hrPossibleWarning+")";
                  
//                  sWarningText = g_Localizer.getLocalized("WARNING_OCCURED")+
//                     (sWarningText.length() > 0 ? ":\n" : "")+sWarningText;
                  
                  if (hrFlags == 1)
                     sWarningText = sWarningText+"\n"+LPLibs.flashGetFontListFontNotFound()+"\n\n";
                  else if (hrFlags == 4)
                     sWarningText = sWarningText+"\n"+LPLibs.flashGetFontListStylesNotSupported()+"\n\n";
                  else if (hrFlags == 8)
                     sWarningText = sWarningText+LPLibs.flashGetTooShortPagesTimes()+"\n\n";
                  else if (hrFlags == 16)
                     sWarningText = sWarningText+"\n"+LPLibs.flashGetFontListGlyphNotFound()+"\n\n";
                  else
                  {
                     String sNativeText = LPLibs.flashLastErrorDetail();
                     if (sNativeText != null)
                        sWarningText = sWarningText+"\n\n"+sNativeText;
                  }
            
                  // Inform the ProgressManager
                  m_progressManager.SendWarningMessage(sWarningText);
                  if( m_pwData.m_bIsPowerTrainerMode && (m_dptCommunicator != null) ) {
                     m_dptCommunicator.ShowWarningMessage(sWarningText);
                  } else {
                     JOptionPane.showMessageDialog(this, 
                                                   sWarningText, 
                                                   g_Localizer.getLocalized("WARNING"),
                                                   JOptionPane.WARNING_MESSAGE);
                  }
               }

               // Next flag: 2^n
               hrFlags *= 2;
            }
         }

         // New Flex Player and SCORM: 
         // --> The 'content' folder can only be deleted after the temporary files are removed
         if (bIsFlexPlayerExport && bScormEnabled && !m_pwData.m_bIsPowerTrainerMode)
         {
            File file = new File(strTargetDir + contentDir);
            file.delete();
         }
         

/*
         JOptionPane.showMessageDialog(this, 
                                       g_Localizer.getLocalized("CONVERSION_SUCCEEDED"), 
                                       g_Localizer.getLocalized("INFORMATION"),
                                       JOptionPane.INFORMATION_MESSAGE);
*/
         // Inform the ProgressManager
         m_progressManager.SendArbitraryMessage("TargetDir", strTargetDir);
         if(m_pwData.m_bIsPowerTrainerMode && (m_dptCommunicator != null)) {
             // Only Error and Warning messages are handled by the DPT Communicator
             // --> Do nothing here; just report output directory
             
             m_dptCommunicator.SendArbitraryMessage("TargetDir", strTargetDir);
         } else {
            if (!m_pwData.m_bIsLiveContext) {
                String[] opt = {g_Localizer.getLocalized("OK"), g_Localizer.getLocalized("OPEN_DIR")};
                String strConversionSucceeded = g_Localizer.getLocalized("CONVERSION_SUCCEEDED");
                if (bDoUpload || bUseNetworkDrive)
                   strConversionSucceeded = g_Localizer.getLocalized("CONVERSION_UPLOAD_SUCCEEDED");
                int jres = JOptionPane.showOptionDialog(this,
                                                        (strConversionSucceeded + strTargetDir + "\n\n"), 
                                                        g_Localizer.getLocalized("INFORMATION"), 
                                                        JOptionPane.DEFAULT_OPTION, 
                                                        JOptionPane.INFORMATION_MESSAGE, 
                                                        null, opt, opt[0]);
                if (jres == JOptionPane.NO_OPTION) {
                   NativeUtils.startFile(strTargetDir);
                }
            }
         }

         if (NativeUtils.isLibraryLoaded())
         {
            if (bCreateWebsite && !bScormEnabled)
            setEnableButton(BUTTON_CUSTOM, true);
         }
      }
      else if (hr < 0)
      {
         // Remove the already written files
         if (aRemoveUponCancel.size() > 0)
         {
            System.out.println("Deleting  "+aRemoveUponCancel.size()+ " files and folders ...   ");
            for (int i=aRemoveUponCancel.size()-1; i>=0 ; --i)
            {
               //System.out.println("Deleting ...   "+(String)aRemoveUponCancel.get(i));
               File f = new File((String)aRemoveUponCancel.get(i));
               f.delete();
            }
         }
      }
      
      
      m_btnCancel.setEnabled(false);
      
      if (hr >= 0 || m_bCancelRequested)
      {
         setEnableButton(BUTTON_QUIT, true);
         
         /* currently the native Flash components crash when used twice
            namely Flash_Cleanup().
         if (m_bCancelRequested)
         {
            setEnableButton(BUTTON_BACK, true);
            m_bCancelRequested = false;
            m_btnCancel.setEnabled(true);
         }
         */
      }
      else
      {         
          setEnableButton(BUTTON_CANCEL, true);
      }

     if(m_pwData.m_bIsPowerTrainerMode || m_pwData.m_bIsLiveContext) {
         super.doQuitOnDoQuit(); // Signal that super should return from displayWizard()
         if (!m_bCancelRequested)
             doQuit(); // Set the correct "action" value: QUIT (not the default CANCEL)
     }
     // Note: This is double code to the other formats
    
      return super.displayWizard();
   }

   public PublisherWizardData getWizardData()
   {
      m_pwData.nextWizardPanel = null;
      m_pwData.finishWizardPanel = null;
   
      if (m_pwData.B_GOTO_START_DIALOG)
      {
         m_pwData.finishWizardPanel = null;
         // Bugfix 5448:
         // If Start Dialog is not defined then try the TargetUpload Dialog 
         // If TargetUpload Dialog is not defined then create a new one
         if (m_pwData.GetStartDialog() != null) {
            m_pwData.nextWizardPanel = m_pwData.GetStartDialog();
         } else {
            // Next line outcommented. Reason: 'getWizardData()' is called twice (why?)
            // and then 'nextWizardPanel' will be set to 'null' after the 2nd call 
            //m_pwData.B_GOTO_START_DIALOG = false;
            if (m_pwData.GetUploadDialog() != null) {
               m_pwData.nextWizardPanel = m_pwData.GetUploadDialog();
            } else {
               TargetUploadWizardPanel tuwp = new TargetUploadWizardPanel(m_pwData);
               m_pwData.SetUploadDialog(tuwp);
               m_pwData.nextWizardPanel = tuwp;
            }
         }
      }

      return m_pwData;
   }
   

   public String getDescription()
   {
      // BUGFIX 4982: set title in direct mode
      return "Publisher";
      // return "";
   }
   
   
   public void actionPerformed(ActionEvent evt)
   {
      if (evt.getSource() == m_btnCancel)
      {
         m_bCancelRequested = true;
         LPLibs.flashCancel();
         if (m_pwData.GetProfiledSettings().GetBoolValue("bScormEnabled"))
            LPLibs.scormCancel();
         m_btnCancel.setEnabled(false);

         //setEnableButton(BUTTON_QUIT, true); // will be done at the end of displayWizard()
         m_strProgressLabel = g_Localizer.getLocalized("WAIT_FOR_CANCEL");
         updateProgressLabel();
         return;
      }
   }
   
   protected void customCommand(String command)
   {
      if (NativeUtils.isLibraryLoaded())
      {
         // Check if the server type enables replay
         ProfiledSettings ps = m_pwData.GetProfiledSettings();
         boolean bIsRunnableServerType 
            = ( (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_FILE_SERVER)
               || (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_WEB_SERVER) );

         if (bIsRunnableServerType)
         {
            // Open HTML file
            this.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));

            // New Flah Player used? 
            // Workaround for bug 4797: Start the file and close the Publisher
            if (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX)
            {
               /*NativeUtils.startFile(m_strHtmlTarget);*/
               // Start the file using a Java method
               // (this method returns immediately back to the 'javaw' process)
               try
               {
                  Runtime.getRuntime().exec("cmd.exe /c start /b&&\"" + m_strHtmlTarget + "\"");
                  //Alternative call (1):
                  //Runtime.getRuntime().exec("rundll32 url.dll,FileProtocolHandler \"" + m_strHtmlTarget + "\"");
                  //Alternative call (2):
                  //ProcessBuilder pb = new ProcessBuilder("rundll32", "url.dll,FileProtocolHandler", m_strHtmlTarget);
                  //pb.start();

                  // Quit the Publisher
                  doQuit();
               }
               catch (Exception e)
               {
                  e.printStackTrace();
               }
            }
            else
            {
               // Start the file "the normal way" using JNI and NativeUtils
               // (this method waits until the file (or browser) has started 
               // and then returns back to the 'javaw' process)
               NativeUtils.startFile(m_strHtmlTarget);
            }

            this.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
         }
         else
         {
            // TODO: This message box has to be specified in the user concept ...
            JOptionPane.showMessageDialog(this, 
                                          g_Localizer.getLocalized("NOT_RUNNABLE"), 
                                          g_Localizer.getLocalized("ERROR"),
                                          JOptionPane.ERROR_MESSAGE);
         }

         //doQuit();
         // Bugfix #4085:
         return;
      }
   }
   
   private void initGui() {
      setPreferredSize(new Dimension(500, 400));

      aThumbnailEntries = null;
      m_sThumbFiles = "";
      
      addButtons(BACK_QUIT_CANCEL);
      
      // set logo
      String imageResource = "/imc/lecturnity/converter/images/flash_logo.gif";      
      if (m_pwData.GetProfiledSettings().GetIntValue("iServerType") == PublisherWizardData.SRV_SLIDESTAR) {
          // check if SLIDESTAR logo is available
          // TODO: check can be removed if availability is ensured
          String slidestarImage = "/imc/lecturnity/converter/images/pngSlidestar.png";
          if ((getClass().getResource(slidestarImage)) != null)
              imageResource = slidestarImage;          
      }
      useHeaderPanel(g_Localizer.getLocalized("HEADER"), g_Localizer.getLocalized("SUBHEADER"), imageResource);

      if (NativeUtils.isLibraryLoaded())
      {
         addButtons(BUTTON_CUSTOM, g_Localizer.getLocalized("START"), 's');
         setEnableButton(BUTTON_CUSTOM, false);
      }

      Container container = getContentPanel();
      container.setLayout(null);

      
      int w = 460;
      int w1 = 60;
      int x = 20;
      int y = 20;
      
      JLabel lblTask = new JLabel(g_Localizer.getLocalized("TASK")+": ");
      lblTask.setLocation(x,y);
      lblTask.setSize(w1,20);
      //container.add(lblTask);
      
      
      m_lblProgress = new JLabel("");
      m_lblProgress.setLocation(x,y);//+w1+10,y);
      m_lblProgress.setSize(w-(x+w1+10),20);
      container.add(m_lblProgress);
      
      m_barProgress = new JProgressBar(0, 100);
      m_barProgress.setLocation(x, y+25);
      m_barProgress.setSize(w, 20);
      container.add(m_barProgress);
      
      m_btnCancel = new JButton(g_Localizer.getLocalized("CANCEL"));
      m_btnCancel.addActionListener(this);
      m_btnCancel.setLocation(w+20-100, y+50);
      m_btnCancel.setSize(100, 25);
      container.add(m_btnCancel);
      
      m_btnCancel.requestFocus();
   }

   private void DefineProgressBarSteps(boolean bHasSomeVideo, 
                                       boolean bIsStructured, 
                                       boolean bHasClips, 
                                       boolean bIsSgStandalone, 
                                       boolean bIsSlidestarExport, 
                                       boolean bIsFlexPlayerExport, 
                                       boolean bCreateWebsite, 
                                       boolean bUseStreamingServer,
                                       boolean bDoUpload,
                                       boolean bUseNetworkDrive, 
                                       boolean bScormEnabled,
                                       int iClipCount)
   {
       boolean bUseStreamingServerOrUpload = 
           bUseStreamingServer || bDoUpload;

       m_progressManager.Clear();
       // Step 1: Check Upload Server (Server/Upload only) 
       if (bUseStreamingServerOrUpload)
           m_progressManager.AddProgressStep("checkUpload", 1);
       // Step 2: Set output files
       m_progressManager.AddProgressStep("setOutputFiles", 1);
       // Step 3: Scan input files
       m_progressManager.AddProgressStep("scanInputFiles", 1);
       // Step 4: Convert Fonts
       m_progressManager.AddProgressStep("convertFonts", 1);
       // Step 5: Convert images
       m_progressManager.AddProgressStep("convertImages", 1);
       // Step 6a: convert audio flv (if any)
       if (!bHasSomeVideo && !bIsStructured) {
           if (bIsFlexPlayerExport)
               m_progressManager.AddProgressStep("convertAudio", 10);
           else // old LEC 3 Player 
               m_progressManager.AddProgressStep("convertAudio", 1);
       }
       // Step 6b: convert video flv (if any)
       if (bHasSomeVideo || bIsStructured) // Accompanying video or SG video
           m_progressManager.AddProgressStep("convertVideo", 10);
       // Step 7: convert clips (if any)
       if (bHasClips && !bIsSgStandalone && !bIsStructured) {
          for (int i = 0; i < iClipCount; ++i) {
              String strClipId = "convertClip" + i;
              m_progressManager.AddProgressStep(strClipId, 10);
          }
       }
       // Step 8: Build movie
       m_progressManager.AddProgressStep("buildMovie", 1);
       // Step 9: Write thumbs (only SLIDESTAR or New Flash Player)
       if (bIsSlidestarExport || bIsFlexPlayerExport)
           m_progressManager.AddProgressStep("createThumbs", 1);
       // Step 10: Crate web site (if any)
       if (bCreateWebsite)
           m_progressManager.AddProgressStep("createWebsite", 1);
       // Step 11: SCORM (if any)
       if (bScormEnabled)
           m_progressManager.AddProgressStep("createScorm", 1);
       // Step 12: SLIDESTAR (if any)
       if (bIsSlidestarExport)
           m_progressManager.AddProgressStep("createSlidestar", 1);
       // Step 13a: Do Upload Copy (Server/Upload only)
       if (bUseStreamingServerOrUpload)
           m_progressManager.AddProgressStep("doUploadCopy", 5);
       // Step 13b: Do NetworkDrive Copy (Server/NetworkDrive only)
       if (bUseNetworkDrive)
           m_progressManager.AddProgressStep("doNetworkDriveCopy", 1);
       // Step 13c: Do StreamingServer Copy (Streaming server only)
       if (bUseStreamingServer)
           m_progressManager.AddProgressStep("doStreamingServerCopy", 1);

       m_fCurrentProgress = 0.0f;
   }

   private void CalculateMovieDimensions()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      DocumentData     dd = m_pwData.GetDocumentData();
      
      Dimension dimInputSlides = dd.GetWhiteboardDimension();
      Dimension dimOutputSlides = new Dimension(ps.GetIntValue("iSlideWidth"),
                                                ps.GetIntValue("iSlideHeight"));
      Dimension dimOutputMovie = new Dimension(ps.GetIntValue("iFlashMovieWidth"),
                                               ps.GetIntValue("iFlashMovieHeight"));
      int iMovieSizeType = ps.GetIntValue("iFlashMovieSizeType");
      boolean bOptimizeOutputSize = iMovieSizeType == PublisherWizardData.MOVIESIZE_SCREEN
         || iMovieSizeType == PublisherWizardData.MOVIESIZE_CUSTOM;        
      
      m_dimScaledWhiteboardSize = dimInputSlides;

      m_dimOuterFlashSize = m_pwData.FlashGetWbSizeWithBorders();
      
      if (bOptimizeOutputSize)
      {
         // determine the scaled whiteboard size:
         // 1. scale the movie WITH borders
         // 2. subtract the SCALED borders
      
         m_dimScaledWhiteboardSize = dimOutputSlides;
         m_dimOuterFlashSize = dimOutputMovie;
      }
   }
   
   private int SetOutputFile(int hr, String strFilename)
   {
      // Set the output file for the Flash engine
      
      if (hr >= 0 && !m_bCancelRequested)
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;

         // This is a "silent" step without a progress label  
         m_progressManager.SetProgressStep("setOutputFiles");
         m_progressManager.SetCurrentSubProgress(0.0f);
         updateProgress();

         hr = LPLibs.flashSetOutputFile(strFilename);

         m_strSwfTarget = strFilename;
         try { Thread.sleep(500); } catch (InterruptedException exc) {}
         m_progressManager.SetCurrentSubProgress(1.0f);
         updateProgress();
      }
      
      if (hr < 0 && !m_bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("SET_OUTPUT"), hr);
         m_bErrorDisplayed = true;
      }
      
      return hr;
   }

   private int SetInputFiles(int hr)
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      DocumentData     dd = m_pwData.GetDocumentData();
      
      boolean bClipsCombinedButWithoutVideo =
         dd.HasClips() && !dd.HasNormalVideo() && !ps.GetBoolValue("bShowClipsOnSlides");
      boolean bIsDenver = dd.IsDenver();
      boolean bIsStructured = IsUseStructuredAsVideo(dd);
      File fileLrd = new File(dd.GetPresentationFileName());
      File fileEvq = new File(dd.GetEvqFileName(true));
      File fileObj = new File(dd.GetObjFileName(true));
      File fileAudio = new File(dd.GetAudioFileName(true));
      File fileVideo = null;
      boolean bHasNormalVideo = dd.HasNormalVideo();
      if (bHasNormalVideo)
         fileVideo = new File(dd.GetVideoFileName(true));
      int iVideoOutputWidth = ps.GetIntValue("iVideoWidth");
      int iVideoOutputHeight = ps.GetIntValue("iVideoHeight");
      VideoClipInfo[] aClipInfos = null;
      int iClipCount = 0;
      boolean bHasClipsSeparated = dd.HasClips() && ps.GetBoolValue("bShowClipsOnSlides");
      if (bHasClipsSeparated || bIsDenver || bIsStructured)
      {          
         aClipInfos = dd.GetVideoClipInfos();
         iClipCount = aClipInfos.length;
      }
      if (bIsStructured) {
         // If the clip of the structured SG document is treated like a video 
         // then the video size must be determined from the SG clip 
         // and maybe also rescaled (depending on profile)
         int clipWidth = aClipInfos[0].videoWidth;
         int clipHeight = aClipInfos[0].videoHeight;
         // Rescale clip sizes?
         Dimension clipSize = GetRescaledClipSize(clipWidth, clipHeight);
         iVideoOutputWidth = clipSize.width;
         iVideoOutputHeight = clipSize.height;
      }
      
      File filePresentation = new File(dd.GetPresentationFileName());
      

      if (hr >= 0 && !m_bCancelRequested)
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;
         
         m_strProgressLabel = g_Localizer.getLocalized("SCAN_INPUT");
         m_progressManager.SetProgressLabel(m_strProgressLabel);
         m_progressManager.SetProgressStep("scanInputFiles");
         m_progressManager.SetCurrentSubProgress(0.0f);
         updateProgress();
         updateProgressLabel();
         
         // "Denver" documents have no EVQ/OBJ
         if (bIsDenver)
         {
            LPLibs.flashSetSgStandAloneMode(true);
            hr = LPLibs.flashSetInputFiles(fileLrd+"", "", "", fileAudio+""); 
         }
         else
         {
            LPLibs.flashSetSgStandAloneMode(false);
            hr = LPLibs.flashSetInputFiles(fileLrd+"", fileEvq+"", fileObj+"", fileAudio+"");
         }

         String[] aFlvOutputNames = GetFlvFilenames(m_pwData, null);
         
         boolean bDoPadding = ps.GetBoolValue("bFlashDoNotCutVideos");
         
         // Video? Or an Audio FLV?
         if (hr >= 0 && !m_bCancelRequested && aFlvOutputNames != null 
             && aFlvOutputNames.length > 0 && aFlvOutputNames[0] != null)
         {
            String strVideoFile = filePresentation+"" ;
            
            m_strFlvVideoName = aFlvOutputNames[0];

            if (bHasNormalVideo)
               strVideoFile = fileVideo+"";
             
            boolean bHasAnyVideo = false;
            if (bHasNormalVideo || bClipsCombinedButWithoutVideo || bIsStructured)
            {
               bHasAnyVideo = true;
               hr = LPLibs.flashSetInputVideoFile(strVideoFile, m_strFlvVideoName, 
                                                  iVideoOutputWidth, iVideoOutputHeight, bDoPadding);
            }    
            if (bIsDenver)
            {
               bHasAnyVideo = true;
               strVideoFile = new File(m_pathFile, aClipInfos[0].clipFileName)+"";
               hr = LPLibs.flashSetInputVideoFile(strVideoFile, m_strFlvVideoName, 
                                                  iVideoOutputWidth, iVideoOutputHeight, bDoPadding);
            }
            if (!bHasAnyVideo && m_bCreateAudioFlv && !bIsStructured)
            {
               // Audio FLV - which is m_strFlvVideoName
               hr = LPLibs.flashSetInputAudioIsFromFlv(true, m_strFlvVideoName);
            }
         }

         // Clips?
         if (hr >= 0 && !m_bCancelRequested && bHasClipsSeparated && !bIsDenver && !bIsStructured)
         {
            LPLibs.flashSetNumberOfClips(iClipCount); 
            
            for (int i = 0; i < iClipCount; ++i)
            {
               if (hr >= 0 && !m_bCancelRequested) 
               {
                  if (i+1 < aFlvOutputNames.length)
                  {
                     String clipFileName = new File(m_pathFile, aClipInfos[i].clipFileName)+"";

                     m_strFlvClipNames[i] = aFlvOutputNames[i + 1];
                     // +1: first entry contains video file name or is empty

                     int clipWidth = aClipInfos[i].videoWidth;
                     int clipHeight = aClipInfos[i].videoHeight;
                     // Rescale clip sizes?
                     Dimension clipSize = GetRescaledClipSize(clipWidth, clipHeight);
                     clipWidth = clipSize.width;
                     clipHeight = clipSize.height;

                     long startTimeMs = aClipInfos[i].startTimeMs;
                     long endTimeMs = aClipInfos[i].startTimeMs 
                     + aClipInfos[i].videoDurationMs;

                     hr = LPLibs.flashSetInputClipFile(i, clipFileName, m_strFlvClipNames[i], 
                                                       clipWidth, clipHeight, bDoPadding, 
                                                       startTimeMs, endTimeMs);
                  }
                  else
                  {
                     System.err.println("FlashWriterWizardPanel: Unexpected error: output names array too short on clips!");
                  }
               }
            }
         }
         
         // Still Image? (Only for New Flashplayer)
         boolean bIsFlexPlayerExport = 
            ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX;
         if (hr >= 0 && !m_bCancelRequested && bIsFlexPlayerExport && dd.HasStillImage())
         {
            // Get StillImage file and its dimensions
            String strStillImageName = dd.GetStillImageName(true);
            Dimension dimImage = GetImageSize(strStillImageName);
            if (dimImage == null || dimImage.width <= 0)
            {
               System.out.println("Unable to load image " + strStillImageName);
               hr = LPLibs.INPUT_FILE_MISSING;
            }
            
            if (hr >= 0 && !m_bCancelRequested) 
               hr = LPLibs.flashSetInputStillImageFile(strStillImageName, 
                                                       dimImage.width, dimImage.height);
         }
         
         m_progressManager.SetCurrentSubProgress(1.0f);
         updateProgress();
      }
     
      if (hr < 0 && !m_bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("SCAN_INPUT"), hr);
         m_bErrorDisplayed = true;
      }
      
      return hr;
   }
   
   public static String[] GetFlvFilenames(PublisherWizardData data, String strTargetPrefix)
   {
      ProfiledSettings ps = data.GetProfiledSettings();
      DocumentData dd = data.GetDocumentData();
      
      // find the right (local) target directory
      String strTargetDir = ps.GetStringValue("strTargetDirLocal");
      if (strTargetDir == null || strTargetDir.length() == 0)
      {
         strTargetDir = ps.GetStringValue("strTargetDirStreamingServer");
         
         if (strTargetDir == null || strTargetDir.length() == 0)
         {
            strTargetDir = ps.GetStringValue("strTargetDirHttpServer");
         }
      }
    
      if (data == null || dd == null || ps == null || 
          strTargetDir == null || strTargetDir.length() <= 0 ||
          ps.GetStringValue("strTargetPathlessDocName").length() <= 0)
      {
         System.err.println("FlashWriterWizardPanel: Unexpected error: Data for GetFlvFilenames() invalid!");
         return null;
      }
      if (!strTargetDir.endsWith(File.separator))
         strTargetDir += File.separatorChar;
      // New Flash Player? --> Adapt target dir
      boolean bIsFlexPlayerExport = 
         ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX;
      if (bIsFlexPlayerExport)
      {
         // Add "content" directory to the path of the target
         strTargetDir += STR_CONTENT + File.separatorChar;
      }
      
      ArrayList aFlvNamesCollector = new ArrayList(10);
      
      if (strTargetPrefix == null)
         strTargetPrefix = ps.GetStringValue("strTargetPathlessDocName");

      File pathTargetFile = new File(strTargetDir);
      
      
      boolean bClipsCombinedButWithoutVideo = 
         dd.HasClips() && !ps.GetBoolValue("bShowClipsOnSlides") && !dd.HasNormalVideo();
      boolean bIsStructured = IsUseStructuredAsVideo(dd);
        
      // Bugfix 3815: Video file name can contain non ASCII characters 
      // --> use "<filename>.flv" instead
      String strFlvVideoName = new File(pathTargetFile, strTargetPrefix + ".flv").toString();

      boolean bUseVideo = false;
      if (dd.HasNormalVideo())
         bUseVideo = true;
      else if (dd.IsDenver())
         bUseVideo = true;
      else if (bClipsCombinedButWithoutVideo)
         bUseVideo = true;
      
      if (bIsStructured)
         bUseVideo = true;
         
      // Audio FLV is treated like a video
      if (m_bCreateAudioFlv)
         bUseVideo = true;
      // --> bUseVideo is true in any case, 
      // because each recording contains audio 
      // which is written to a FLV file (with or without video)
      
      if (bUseVideo)
         aFlvNamesCollector.add(strFlvVideoName);
      else
         aFlvNamesCollector.add(null); // empty placeholder
      
      
      if (dd.HasClips() && ps.GetBoolValue("bShowClipsOnSlides") && !bIsStructured)
      {
         int iClipCount = dd.GetVideoClipInfos().length;
         for (int i = 0; i < iClipCount; ++i)
         {
            // Bugfix 3737: Clip file names can contain non ASCII characters 
            // --> use "<filename>_clipXX.flv" instead
     
            String clipName = strTargetPrefix + ("_clip" + i);
            aFlvNamesCollector.add(new File(pathTargetFile, clipName + ".flv").toString());
         }
      }
      
      String[] aReturnNames = new String[aFlvNamesCollector.size()];
      aFlvNamesCollector.toArray(aReturnNames);
      
      return aReturnNames;
   }

   private Dimension GetRescaledClipSize(int origClipWidth, int origClipHeight)
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      int iClipSizeType = ps.GetIntValue("iClipSizeType");
      boolean bSizeTypeMax = iClipSizeType == PublisherWizardData.CLIPSIZE_MAXSLIDE;
      boolean bSizeTypeCustom = iClipSizeType == PublisherWizardData.CLIPSIZE_CUSTOM;
      int iMaxClipWidth =  m_dimScaledWhiteboardSize.width;
      int iMaxClipHeight = m_dimScaledWhiteboardSize.height;
      if (bSizeTypeCustom)
      {
         iMaxClipWidth = ps.GetIntValue("iMaxCustomClipWidth");
         iMaxClipHeight = ps.GetIntValue("iMaxCustomClipHeight");
      }
      // Clips in video area?
      boolean bShowClipsInVideoArea = !ps.GetBoolValue("bShowClipsOnSlides");
      if (bShowClipsInVideoArea) {
          // --> Use video size for clip size
          int iVideoSizeType = ps.GetIntValue("iVideoSizeType");
          if (iVideoSizeType == PublisherWizardData.VIDEOSIZE_STANDARD) {
              // Use standard 320x240
              iMaxClipWidth = StreamingMediaConverter.STANDARD_VIDEO_WIDTH;
              iMaxClipHeight = StreamingMediaConverter.STANDARD_VIDEO_HEIGHT;
          } else {
              // Use video size
              iMaxClipWidth = ps.GetIntValue("iVideoWidth");
              iMaxClipHeight = ps.GetIntValue("iVideoHeight");
          }
      }

      int clipWidth = origClipWidth;
      int clipHeight = origClipHeight;
       
      if (bSizeTypeMax || bSizeTypeCustom || bShowClipsInVideoArea)
      {        
         double scaleX = (double)iMaxClipWidth / (double)clipWidth;
         double scaleY = (double)iMaxClipHeight / (double)clipHeight;
         double scaleClips = (scaleX < scaleY) ? scaleX : scaleY;
         if (scaleClips > (double)1.0)
            scaleClips = (double)1.0;

         clipWidth = (int)( (double)clipWidth * scaleClips + 0.5 );
         clipHeight = (int)( (double)clipHeight * scaleClips + 0.5 );
      }
      
      return new Dimension(clipWidth, clipHeight);
   }

   private int SetDrawEvalNote(int hr)
   {
      // Set flag for "Draw Eval Note"
      if (hr >= 0 && !m_bCancelRequested)
      {
         // Get the Version Type of the ConverterWizard instance 
         // and set the Evaluation Flag, if necessary
         int nVersion = ConverterWizard.getVersionType();
         System.out.println("Version Type: " + nVersion);
         boolean bDrawEvalNote = true;
         switch(nVersion)
         {
            case  NativeUtils.FULL_VERSION:
                     bDrawEvalNote = false;
                     break;
            case  NativeUtils.EVALUATION_VERSION:
                     bDrawEvalNote = true;
                     break;
            case  NativeUtils.UNIVERSITY_VERSION:
                     bDrawEvalNote = false;
                     break;
            case  NativeUtils.UNIVERSITY_EVALUATION_VERSION:
                     bDrawEvalNote = true;
                     break;
            default:
                     bDrawEvalNote = true;
                     break;
         }

         hr = LPLibs.flashSetDrawEvalNote(bDrawEvalNote);
      }
      
      return hr;
   }

   private int SetInputOutputDimensions(int hr, int iOutputWidth, int iOutputHeight,
                                        int iWbWidth, int iWbHeight, boolean bIsDenver)
   {
      // Set input/output dimensions for the Flash engine

      if (hr >= 0 && !m_bCancelRequested)
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;
         
         // Movie size
         if (hr >= 0)
            hr = LPLibs.flashSetMovieDimension(iOutputWidth, iOutputHeight);
         
         // Whiteboard sizes
         if (hr >= 0 && bIsDenver)
         {
            // Special case: "Denver" Documents
            hr = LPLibs.flashSetWbInputDimension(0, 0);
            if (hr >= 0)
               hr = LPLibs.flashSetWbOutputDimension(0, 0);
         }
         else
         {
            hr = LPLibs.flashSetWbInputDimension(iWbWidth, iWbHeight);
            if (hr >= 0)
               hr = LPLibs.flashSetWbOutputDimension(m_dimScaledWhiteboardSize.width, m_dimScaledWhiteboardSize.height);
         }
      }
      
      if (hr < 0 && !m_bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("SET_DIMENSIONS"), hr);
         m_bErrorDisplayed = true;
      }
      
      return hr;
   }

   private int ConvertFonts(int hr)
   {
      // Convert the fonts
      
      if (hr >= 0 && !m_bCancelRequested)
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;
         
         m_strProgressLabel = g_Localizer.getLocalized("CONVERT_FONTS");
         m_progressManager.SetProgressLabel(m_strProgressLabel);
         m_progressManager.SetProgressStep("convertFonts");
         m_progressManager.SetCurrentSubProgress(0.0f);
         updateProgress();
         updateProgressLabel();

         hr = LPLibs.flashConvertFonts();
         try { Thread.sleep(500); } catch (InterruptedException exc) {}

         m_progressManager.SetCurrentSubProgress(1.0f);
         updateProgress();
      }
      
      if (hr < 0 && !m_bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("CONVERT_FONTS"), hr);
         m_bErrorDisplayed = true;
      }
      
      return hr;
   }

   private int ConvertImages(int hr, boolean bDisplayLogo, String strLogoFilename)
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      // Convert the images
      if (hr >= 0 && !m_bCancelRequested)
      {
         if (hr > 0)
            m_hrPossibleWarning = hr;
         
         m_strProgressLabel = g_Localizer.getLocalized("CONVERT_IMAGES");
         m_progressManager.SetProgressLabel(m_strProgressLabel);
         m_progressManager.SetProgressStep("convertImages");
         m_progressManager.SetCurrentSubProgress(0.0f);
         updateProgress();
         updateProgressLabel();

         hr = LPLibs.flashConvertImages();

         try { Thread.sleep(500); } catch (InterruptedException exc) {}
         m_progressManager.SetCurrentSubProgress(0.0f);
         updateProgress();
         
         if (hr >= 0 && bDisplayLogo)
         {
            //
            // this procedure (writing a new png)
            // avoids problems with progressive jpgs (not supported by Ming)
            //
            
            String strLogoImageName = ps.GetStringValue("strLogoImageName");
            Image inputImage = Toolkit.getDefaultToolkit().createImage(strLogoImageName);
            MediaTracker mt = new MediaTracker(this);
            mt.addImage(inputImage, 0);
            try { mt.waitForID(0); } catch (InterruptedException exc) {}
            
            if (inputImage.getWidth(null) > 0)
            {
               BufferedImage bi = new BufferedImage(
                  inputImage.getWidth(null), inputImage.getHeight(null), BufferedImage.TYPE_INT_ARGB);
               
               bi.getGraphics().drawImage(inputImage, 0,0, this);
                                
               try
               {
                  m_strLogoFileName = new File(m_pathTargetFile, m_strDocumentName+ "_logo_210777.png")+"";
      
               
                  BufferedOutputStream out = new BufferedOutputStream(
                     new FileOutputStream(m_strLogoFileName));
         
                  ImageEncoder pngWriter = 
                     ImageCodec.createImageEncoder("PNG", out,
                                                   PNGEncodeParam.getDefaultEncodeParam(bi));
                  pngWriter.encode(bi);
         
                  out.close();
               }
               catch (IOException exc)
               {
                  exc.printStackTrace();
                  
                  hr = LPLibs.IMAGE_CONVERSION_FAILED;
               }
            }
            else // image width <= 0
            {
               hr = LPLibs.IMAGE_CONVERSION_FAILED;
            }
         }

         m_progressManager.SetCurrentSubProgress(1.0f);
         updateProgress();
      }
      
            
      if (hr < 0 && !m_bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("CONVERT_IMAGES"), hr);
         m_bErrorDisplayed = true;
      }
      
      return hr;
   }

	private int WriteSlidestarThumbs()
	{
		int res = getThumbnailEntriesFromMetadata();
	    if (res >= 0)
		   res = writeThumbnailFiles(false, true, true);
		return res;
	}

	private int WriteFlexThumbs()
	{
		boolean bIsDenver = m_pwData.GetDocumentData().IsDenver();
		int res = getThumbnailEntriesFromMetadata();
		if (res >= 0 && !bIsDenver)
		   res = writeThumbnailFiles(true, true, true);
		return res;
	}

	/***************************************************************************/

	/**
	 * Get Thumbnail Entries from the Metadata
	 * If no metadata file exists, we have to extract the information from the SlideEntries
	 *
	 *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
	 * 
	 */
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

	protected int getThumbnailEntriesFromMetadata()
	{
		int res = 0;

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
			//tpcAll.nNumOfThumbnails = nNumOfThumbnails;

			// Bugfix 5388: Special case "Denver" document: 
			boolean bIsDenver = m_pwData.GetDocumentData().IsDenver();
			if (bIsDenver) {
	            // Create a thumbnail for the SG clip
	            aThumbnailEntries = new ThumbnailEntry[1];
			    lBeginTimestamp = 0;
			    lEndTimestamp = aClipInfos[0].videoDurationMs;
			    strImageName = "slide_0";
			    strImageName += "_thumb"; // "_clip_thumb";
			    strClipFileName = aClipInfos[0].clipFileName;
			    strTitle = "";
			    strCaption = "";
                // Fill the Thumbnail Entry Object 
                aThumbnailEntries[0] = new ThumbnailEntry(lBeginTimestamp,
                                                          lEndTimestamp,
                                                          strImageName,
                                                          strTitle,
                                                          strCaption,
                                                          strClipFileName);
			} else {
    			// No "Denver", normal case
	            aThumbnailEntries = new ThumbnailEntry[nNumOfThumbnails];
			    for (int i = 0; i < nNumOfThumbnails; ++i)
    			{
    				// Get the Timestamps
    				lBeginTimestamp = metadata.thumbnails[i].beginTimestamp;
    				lEndTimestamp = metadata.thumbnails[i].endTimestamp;
    
    				// Get the Image Name and - if necessaray - the Clip File Name
    				// This can be either an Image or a Video Clip
    				// Image Name is something like "<prefix>_00001234_thumb"
    
    				strImageName = "slide_" + i; //metadata.title + "_" + GetNumberString(lBeginTimestamp, 1);
    
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
    						//strClipFileName = "Clip " + clipIndex;
    						// Bugfix 4848: strClipFileName must be empty
    						strClipFileName = "";
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
    				if ((metadata.thumbnails[i].chapterTitles != null)
    				  && (metadata.thumbnails[i].displayTitle == true))
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
    				aThumbnailEntries[i] = new ThumbnailEntry(lBeginTimestamp,
    																lEndTimestamp,
    																strImageName,
    																strTitle,
    																strCaption,
    																strClipFileName);
    			}
			}
		}
		//else
		//{
		//    System.out.println("In getThumbnailEntriesFromMetadata(): No Metadata!");

		//    // We have to extract the information from the SlideEntries

		//    // Get the timestamps of new pages
		//    int i = 0;
		//    int idx = 0;
		//    long[] pagingTimes = new long[(m_DocInfo.aSlideEntries.length+1)];

		//    for (i = 0; i < m_DocInfo.aSlideEntries.length; ++i)
		//    {
		//        if (m_DocInfo.aSlideEntries[i].bIsNewPage)
		//        {
		//            pagingTimes[idx] = m_DocInfo.aSlideEntries[i].lTimestamp;
		//            idx++;
		//        }
		//    }
		//    pagingTimes[idx] = m_lDuration;
		//    idx++;

		//    nNumOfThumbnails = (idx - 1);
		//    m_DocInfo.aThumbnailEntries = new ThumbnailEntry[nNumOfThumbnails];
		//    //tpcAll.nNumOfThumbnails = nNumOfThumbnails;

		//    for (i = 0; i < nNumOfThumbnails; ++i)
		//    {
		//        // Get the Timestamps
		//        lBeginTimestamp = pagingTimes[i];
		//        lEndTimestamp = (pagingTimes[i + 1] - 1);

		//        // Get the Image Name
		//        // Image Name is something like "<prefix>_00001234_thumb"
		//        strImageName = m_strFileName + "_" + GetNumberString(lBeginTimestamp, 8) + "_thumb";
		//        /*
		//        for (int j = 7; j > 0; --j)
		//        {
		//           if (lBeginTimestamp < Math.pow(10, j))
		//           strImageName += "0";
		//        }
		//        strImageName += lBeginTimestamp; 
		//        strImageName += "_thumb";
		//        */

		//        strClipFileName = "";
		//        strTitle = "";

		//        // Get the Caption
		//        strCaption = String.valueOf(i + 1) + ". " + SLIDE;

		//        // Fill the Thumbnail Entry Object 
		//        m_DocInfo.aThumbnailEntries[i] = new ThumbnailEntry(lBeginTimestamp,
		//                                                        lEndTimestamp,
		//                                                        strImageName,
		//                                                        strTitle,
		//                                                        strCaption,
		//                                                        strClipFileName);
		//    }
		//}

      // Bugfix 4848: Clip Inconsistency can also occur in cases where iClipCount == 0
//		if (iClipCount > 0)
//		{
			// bugfix 4744
			// The next check "clipIndex != iClipCount" will fail in case of structured SG documents 
			boolean bHasClipInconsistency = m_pwData.GetDocumentData().isClipStructured() ? false : (clipIndex != iClipCount);  
         
			// Bugfix 4946: 
			// Clip inconsistencies belonging to (edited) structured SG documents can be ignored. 
			// However, manipulated LRD files, which may lead to 'bHasErrors' (Bug 4848), must be remembered.
			//if (bHasErrors || bHasClipInconsistency)
			if (bHasErrors)
			{
				// Show a warning message box
            String sWarningText = g_Localizer.getLocalized("METADATA_INCONSISTENCY");

            //Inform the ProgressManager
            m_progressManager.SendWarningMessage(sWarningText);
            if( m_pwData.m_bIsPowerTrainerMode && (m_dptCommunicator != null) ) {
               m_dptCommunicator.ShowWarningMessage(sWarningText);
            } else {
   				JOptionPane.showMessageDialog(this,
	   										  sWarningText,
		   									  g_Localizer.getLocalized("WARNING"),
			   								  JOptionPane.WARNING_MESSAGE);
			   }
			}
//		}

		// Debug Info
		System.out.println("+++ Thumbnails:              " + nNumOfThumbnails);
		System.out.println("+++ Clips:                   " + clipIndex);

		return res;
	}
	/**
   * Write thumbnail files
   *
   *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
   */
	protected int writeThumbnailFiles(boolean bDrawFlexThumbs, 
	                                  boolean bDrawFrameThumbs, 
	                                  boolean bDrawBorderLineThumbs)
	{
		int res = 0;

        m_strProgressLabel = g_Localizer.getLocalized("CREATE_THUMBS");
        m_progressManager.SetProgressLabel(m_strProgressLabel);
        m_progressManager.SetProgressStep("createThumbs");
        m_progressManager.SetCurrentSubProgress(0.0f);
        updateProgress();
        updateProgressLabel();

		try
		{
			String strLrdFilename = m_pwData.GetDocumentData().GetPresentationFileName();
			FileResources resources =
			FileResources.createFileResources(strLrdFilename);
			PictureExtracter pExtr = new PictureExtracter(m_pwData, resources);
			for (int i = 0; i < aThumbnailEntries.length; ++i)
			{


				// The timestamp of the thumbnail to be written is 1 ms before the end timestamp
				long lTimestamp = aThumbnailEntries[i].lEndTimestamp - 1;
				String strImageName = aThumbnailEntries[i].strImageName;

				// Start writing thumbnail files
				try
				{
					pExtr.setDrawFrame(bDrawFrameThumbs);
					pExtr.setDrawBorderLine(bDrawBorderLineThumbs);
					pExtr.setPictureSize(new Dimension(200, 200));
					pExtr.setShadowThickness(0); // (1)
					pExtr.setShadowBackground(new Color(0xFFFFFF));

					//             String strFileName = m_strPathName + "slides" + File.separatorChar + strImageName + STR_IMAGE_SUFFIX;
					String strFileName = /*m_strTempPath*/ m_pathTargetFile + "" + File.separatorChar + strImageName + "200x200" + STR_IMAGE_SUFFIX;

					if (bDrawFlexThumbs)
   				{
   				   strFileName = /*m_strTempPath*/ m_pathTargetFile + "" + File.separatorChar + strImageName + ".png";

   					if (aThumbnailEntries[i].strClipFileName.equals(""))
   					{
   						// Write the thumb to a png image
   						// (The last parametet 'true' includes the drawing of interactive parts)
      					pExtr.setPictureSize(new Dimension(70, 70));
   						pExtr.writePngPicture((int)lTimestamp,
   														 strFileName, true);
                     aRemoveUponCancel.add(strFileName);
   						m_sThumbFiles += strImageName + ".png" + "<>";
						if (i == 0)
						{
							int iThumbWidth = 300;
							int iThumbHeight = 300;
							int iDisplayWidth = 300;
							int iDisplayHeight = 300;
							DocumentData dd = m_pwData.GetDocumentData();

							if (dd.IsDenver() == false)
							{
								Dimension dim = dd.GetWhiteboardDimension();
								if (dim != null)
								{
									iDisplayWidth = dim.width;
									iDisplayHeight = dim.height;
								}
							}
							else
							{
								//VideoClipInfo[] aClipInfos = dd.GetVideoClipInfos();
								StreamInformation si = dd.GetClipsStreamInfo();
								if (si != null)
								{
									iDisplayWidth = si.m_nWidth;
									iDisplayHeight = si.m_nHeight;
								}
							}


							if (iDisplayWidth < iThumbWidth)
							{
								iThumbWidth = iDisplayWidth;
								iThumbHeight = iDisplayHeight;
							}
							
							strFileName = m_pathTargetFile + "" + File.separatorChar + strImageName + "_LC" + ".png";
							pExtr.setPictureSize(new Dimension(iThumbWidth, iThumbHeight));
							pExtr.writePngPicture((int)lTimestamp,
															 strFileName, true);
							aRemoveUponCancel.add(strFileName);
							m_sThumbFiles += strImageName + "_LC.png" + "<>";
						}
   					}
   					else
   					{
   					   // Write the first frame of the clip to a PNG image
   					   String strAviFileName = aThumbnailEntries[i].strClipFileName;
   					   try
   					   {
   					      pExtr.setPictureSize(new Dimension(70, 70));
   					      pExtr.writeAviFramePngPicture(/*m_strInputPathName*/m_pathFile + File.separator + strAviFileName,
   					                                                 strFileName);
                        aRemoveUponCancel.add(strFileName);
   						   m_sThumbFiles += strImageName + ".png" + "<>";
   					   }
   					   catch (Exception e)
   					   {
   					      // TODO:
   					      // Warning message & copy standard SG clip thumbnail image
   					   }
   					}
   				}
   				else // Slidestar thumbs
					{
   					if (aThumbnailEntries[i].strClipFileName.equals(""))
   					{
   						// Write the thumb to a jpeg image
   						// (The last parametet 'true' includes the drawing of interactive parts)
   						pExtr.writeJpegPicture((int)lTimestamp,
   														 strFileName, true);
   						m_sThumbFiles += strImageName + "200x200" + STR_IMAGE_SUFFIX + "<>";
   						strFileName = m_pathTargetFile + "" + File.separatorChar + strImageName + "400x400" + STR_IMAGE_SUFFIX;
   						pExtr.setPictureSize(new Dimension(400, 400));
   						pExtr.writeJpegPicture((int)lTimestamp,
   														 strFileName, true);
                     aRemoveUponCancel.add(strFileName);
   						m_sThumbFiles += strImageName + "400x400" + STR_IMAGE_SUFFIX + "<>";
   					}
   
   					//else
   					//{
   					//    // Write the first frame of the clip to a PNG image
   					//    String strAviFileName = aThumbnailEntries[i].strClipFileName;
   					//    try
   					//    {
   					//        pExtr.writeAviFramePngPicture(/*m_strInputPathName*/m_pathFile + strAviFileName,
   					//                                                 strFileName);
   					//    }
   					//    catch (Exception e)
   					//    {
   					//        // TODO:
   					//        // Warning message & copy standard SG clip thumbnail image
   					//    }
   					//}
   					//System.out.println(strImageName + STR_IMAGE_SUFFIX + " written.");
   
   					// Add the output file names to the File Container
   					//fileContainer.add(strFileName);
   
   					// Add the relative file name to the slides images file container
   					//m_aJavaWrittenFiles.add("slides" + File.separatorChar + strImageName + STR_IMAGE_SUFFIX); 
   				}

				}
				catch (Exception e)
				{
					e.printStackTrace();
					System.out.println("Unable to write the thumbnail pictures.");
					return LPLibs.IMAGE_CONVERSION_FAILED;
				}
			}
			
			// Bugfix 5388: Special case: SLIDESTAR export with "Denver" document
			// (Slidestar: bDrawFlexThumbs is false)
			boolean bIsDenver = m_pwData.GetDocumentData().IsDenver();
			if (bIsDenver && !bDrawFlexThumbs) {
                // Write the first frame of the clip to a PNG image
                String strFileName;
			    String strImageName = aThumbnailEntries[0].strImageName;
			    String strAviFileName = aThumbnailEntries[0].strClipFileName;
                try
                {
                   // Next line: this leads to a square thumbnail with black borders
                   //pExtr.setPictureSize(new Dimension(200, 200));
                   // --> consider real SG vlip width/height
                   DocumentData dd = m_pwData.GetDocumentData();
                   VideoClipInfo[] aClipInfos = dd.GetVideoClipInfos();
                   int clipWidth = aClipInfos[0].videoWidth;
                   int clipHeight = aClipInfos[0].videoHeight;
                   double fScaleX = 200.0 / aClipInfos[0].videoWidth;
                   double fScaleY = 200.0 / aClipInfos[0].videoHeight;
                   double fScale = Math.min(fScaleX, fScaleY);
                   int thumbWidth = (int)Math.round(fScale * clipWidth);
                   int thumbHeight = (int)Math.round(fScale * clipHeight);
                   pExtr.setPictureSize(new Dimension(thumbWidth, thumbHeight));
                   strFileName = m_pathTargetFile + "" + File.separatorChar + strImageName + "200x200" + STR_IMAGE_SUFFIX;
                   pExtr.writeAviFramePngPicture(m_pathFile + File.separator + strAviFileName,
                                                 strFileName);
                   aRemoveUponCancel.add(strFileName);
                   m_sThumbFiles += strImageName + "200x200" + STR_IMAGE_SUFFIX+ "<>";

                   //pExtr.setPictureSize(new Dimension(400, 400));
                   pExtr.setPictureSize(new Dimension(2*thumbWidth, 2*thumbHeight));
                   strFileName = m_pathTargetFile + "" + File.separatorChar + strImageName + "400x400" + STR_IMAGE_SUFFIX;
                   pExtr.writeAviFramePngPicture(m_pathFile + File.separator + strAviFileName,
                                                 strFileName);
                   aRemoveUponCancel.add(strFileName);
                   m_sThumbFiles += strImageName + "400x400" + STR_IMAGE_SUFFIX+ "<>";
                }
                catch (Exception e)
                {
                   // TODO:
                   // Warning message & copy standard SG clip thumbnail image
                }
			}
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.out.println("Unable to write the thumbnail pictures.");
			return LPLibs.IMAGE_CONVERSION_FAILED;
		}

		m_progressManager.SetCurrentSubProgress(1.0f);
		updateProgress();

        return res; //SUCCESS;
	}

   /**
    * Copy (Move) a file from the source directory to the target directory and remove the old one.
    *
    *    @param String inFile: the input file name, including the whole path.
    *    @param String outFile: the output file name, including the whole path.
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   private int copyFile(String fileIn, String fileOut)
   {
      int hr = 0;
      try
      {
         FileUtils.copyFile(fileIn, fileOut, this, "Copying...");
      }
      catch (IOException exc)
      {
         exc.printStackTrace();
         System.out.println("Error while copying " + fileIn + "!");
         hr = -1;
      }
      
      return hr;
   }

   private int moveFile(String fileIn, String fileOut)
   {
      int hr = copyFile(fileIn, fileOut);

      if (hr >= 0)
      {
         File inFile = new File(fileIn);
         try
         {
            inFile.delete();
         }
         catch (SecurityException e)
         {
            e.printStackTrace();
            System.out.println("Error while removing " + fileIn + "!");
            hr = -1;
         }
      }
      
      return hr;
   }

   private Dimension GetImageSize(String strImagePath)
   {
      ImageIcon icon = new ImageIcon(strImagePath);
      Dimension dimImage = new Dimension(icon.getIconWidth(), icon.getIconHeight());
      return dimImage;
   }

   private void updateProgress() {
      m_fCurrentProgress = m_progressManager.GetCurrentSubProgress();
      m_fTotalProgress = m_progressManager.GetTotalProgress();
      m_barProgress.setValue(m_progressManager.GetTotalProgressPercentage());
      if (m_pwData.m_bIsPowerTrainerMode && (m_dptCommunicator != null))
         m_dptCommunicator.ShowProgress(GetProgress());
   }

   private void updateProgressLabel() {
      m_lblProgress.setText(m_strProgressLabel);
      if (m_pwData.m_bIsPowerTrainerMode && (m_dptCommunicator != null))
         m_dptCommunicator.ShowProgressLabel(m_strProgressLabel);
   }

   private boolean IsDiskFull() {
       boolean bIsDiskFull = false;
       ProfiledSettings ps = m_pwData.GetProfiledSettings(); 
       String strTargetPath = ps.GetStringValue("strTargetDirLocal");
       int nMinKb = 100*1024; // less than 100 kB left
       if (NativeUtils.getSpaceLeftOnDevice(strTargetPath) < nMinKb)
           bIsDiskFull = true;

       return bIsDiskFull;
   }
   private void displayErrorMessage(String sFacility, int nErrorCode)
   {
       // Bugfix 5403: Check for "disk full" error
       boolean bIsDiskFull = IsDiskFull();

      String sErrorText = null;
      try
      {
          sErrorText = g_Localizer.getLocalized(nErrorCode+""); 
          if (bIsDiskFull)
              sErrorText = sErrorText + "\n" + g_Localizer.getLocalized("DISK_FULL");
      }
      catch (Exception exc)
      {
         // maybe this error code is not specified
      }
      
      if (sErrorText == null || sErrorText.length() == 0) {
          sErrorText = g_Localizer.getLocalized("-1");
          if (bIsDiskFull)
              sErrorText = g_Localizer.getLocalized("DISK_FULL");
      }

      // Bugfix 5616:
      // Special case: -99 (CANCELLED)
      if (nErrorCode == LPLibs.CANCELLED)
          return;
      
      // Bugfix 4883:
      // Special case: error code -18 (FLV_NO_OVERWRITE): 
      // The error message must be modified
      if (nErrorCode == LPLibs.FLV_NO_OVERWRITE)
      {
          if (bIsDiskFull) {
              sErrorText = g_Localizer.getLocalized("DISK_FULL");
          } else {
              if (m_bCreateAudioFlv)
                  sErrorText = g_Localizer.getLocalized("AUDIO") + sErrorText;
              else
                  sErrorText = g_Localizer.getLocalized("VIDEO") + sErrorText;
          }
      }
      // Bugfix 5302:
      // Special case: error code -16 (PAGE_TOO_SHORT) or -20 (INTERACTIVE_PAGE_TOO_SHORT):
      // The error message must be modified
      if ( (nErrorCode == LPLibs.PAGE_TOO_SHORT)
          || (nErrorCode == LPLibs.INTERACTIVE_PAGE_TOO_SHORT) ) {
          if (bIsDiskFull) {
              sErrorText = g_Localizer.getLocalized("DISK_FULL");
          } else {
              sErrorText += LPLibs.flashGetTooShortPagesTimes();
              sErrorText += "\n";
          }
      }
      String sMessageText = g_Localizer.getLocalized("ERROR_DURING")+
         ":\n"+sFacility+"\n"+g_Localizer.getLocalized("ERROR")+": "+sErrorText+".";
      String sNativeText = LPLibs.flashLastErrorDetail();
      if (sNativeText != null)
         sMessageText = sMessageText+"\n"+sNativeText;
      
      // Inform the ProgressManager
      m_progressManager.SendErrorMessage(sMessageText);
      // in Power Trainer mode the Publisher is not visible
      if( m_pwData.m_bIsPowerTrainerMode && (m_dptCommunicator != null) ) {
         m_dptCommunicator.ShowErrorMessage(sMessageText);
      } else {
         JOptionPane.showMessageDialog(this, 
                                    sMessageText,
                                    g_Localizer.getLocalized("ERROR"),
                                    JOptionPane.ERROR_MESSAGE);
      }

   }
   
   private static boolean IsUseStructuredAsVideo(DocumentData dd) {
      boolean bUse = dd.isClipStructured();
      String strProhibit = NativeUtils.getPublisherProperty("flashProhibitStructuredAsVideo");
      if (strProhibit != null && strProhibit.length() > 0)
          bUse = false;
      return bUse;
   }
}

