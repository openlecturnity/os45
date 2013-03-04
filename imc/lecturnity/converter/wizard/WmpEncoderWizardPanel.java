package imc.lecturnity.converter.wizard;

import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.BorderLayout;
import java.awt.Font;
import java.awt.Point;
import java.awt.Window;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import java.io.File;
import java.io.IOException;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.SwingConstants;

import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.wizards.*;
import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.filesdk.VideoClipInfo;

import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.converter.StreamInformation;
import imc.lecturnity.converter.StreamingMediaConverter;
import imc.lecturnity.converter.WindowsMediaConverter;
import imc.lecturnity.converter.WmpProfileInfo;

import imc.lecturnity.util.NativeUtils;

class WmpEncoderWizardPanel extends WizardPanel implements ActionListener
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
//   private static String COMPATIBILITY_SETTINGS = "[!]";
//   private static String UNRESTRAINED_WM6_COMPATIBLE = "[!]";
//   private static String FULL_WM6_COMPATIBLE = "[!]";
//   private static String WM9_COMPATIBLE = "[!]";
   private static String TARGET_AUDIENCE = "[!]";
   private static String TARGET_AUDIENCE_DESCR1 = "[!]";
   private static String TARGET_AUDIENCE_DESCR2 = "[!]";
   private static String DETAIL_SETTINGS = "[!]";
   private static String DETAIL_AUDIO = "[!]";
   private static String DETAIL_VIDEO = "[!]";
   private static String DETAIL_CLIPS = "[!]";
   private static String DETAIL_SUM = "[!]";
//   private static String MULTIPLE_BITRATES = "[!]";
   private static String KBPS = "[!]";
   private static String FPS = "[!]";
   private static String VIDEO_CLIPS_COMBINED = "[!]";
   private static String CUSTOM_PROFILE_DESELECT = "[!]";
   private static String NOT_PRESENT = "[!]";
//   private static String CONFIGURE = "[!]";
   private static String EXTENDED = "[!]";
   private static String INSTALLING_MESSAGE = "[!]";
   private static String JOP_YES = "[!]";
   private static String JOP_NO = "[!]";
   private static String JOP_ERROR = "[!]";
   private static String JOP_INFO = "[!]";
   private static String JOP_WARNING = "[!]";
   private static String NO_DLL = "[!]";
   private static String NO_WM7_CODECS = "[!]";
   private static String NO_WM9_CODECS = "[!]";
   private static String NO_WM9_CODECS_DETECTED = "[!]";
   private static String NO_WM7_AND_9_CODECS = "[!]";
   private static String NO_REDIST_PACKAGE = "[!]";
   private static String SYSTEM_NEEDS_REBOOT = "[!]";
   private static String NOT_ADMINISTRATOR = "[!]";
   private static String WM7_RECHECK_FAILED = "[!]";
   private static String RELOAD_CONFIG_CHANGED = "[!]";
   private static String NO_WMSYSPRX_COPY = "[!]";
   private static String[] BANDWIDTHS = {"[!]", "[!]", "[!]", "[!]", "[!]"};
   private static char   MNEM_UNRESTRAINED_WM6_COMPATIBLE = '?';
   private static char   MNEM_FULL_WM6_COMPATIBLE = '?';
   private static char   MNEM_WM9_COMPATIBLE = '?';
//   private static char   MNEM_CONFIGURE = '?';

   private static int WM7_ENCODER_PACKAGE = 7;
   private static int WM9_FORMAT_PACKAGE = 9;

   static
   {
      try
      {
         Localizer l = new Localizer("/imc/lecturnity/converter/wizard/WmpEncoderWizardPanel_",
                                     "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
//         COMPATIBILITY_SETTINGS = l.getLocalized("COMPATIBILITY_SETTINGS");
//         UNRESTRAINED_WM6_COMPATIBLE = l.getLocalized("UNRESTRAINED_WM6_COMPATIBLE");
//         FULL_WM6_COMPATIBLE = l.getLocalized("FULL_WM6_COMPATIBLE");
//         WM9_COMPATIBLE = l.getLocalized("WM9_COMPATIBLE");
         TARGET_AUDIENCE = l.getLocalized("TARGET_AUDIENCE");
         TARGET_AUDIENCE_DESCR1 = l.getLocalized("TARGET_AUDIENCE_DESCR1");
         TARGET_AUDIENCE_DESCR2 = l.getLocalized("TARGET_AUDIENCE_DESCR2");
         DETAIL_SETTINGS = l.getLocalized("DETAIL_SETTINGS");
         DETAIL_AUDIO = l.getLocalized("DETAIL_AUDIO");
         DETAIL_VIDEO = l.getLocalized("DETAIL_VIDEO");
         DETAIL_CLIPS = l.getLocalized("DETAIL_CLIPS");
         DETAIL_SUM = l.getLocalized("DETAIL_SUM");
//         MULTIPLE_BITRATES = l.getLocalized("MULTIPLE_BITRATES");
         KBPS = l.getLocalized("KBPS");
         FPS = l.getLocalized("FPS");
         VIDEO_CLIPS_COMBINED = l.getLocalized("VIDEO_CLIPS_COMBINED");
         NOT_PRESENT = l.getLocalized("NOT_PRESENT");
         CUSTOM_PROFILE_DESELECT =l.getLocalized("CUSTOM_PROFILE_DESELECT");
//         CONFIGURE = l.getLocalized("CONFIGURE");
         EXTENDED = l.getLocalized("EXTENDED");
         INSTALLING_MESSAGE = l.getLocalized("INSTALLING_MESSAGE");
         JOP_YES = l.getLocalized("JOP_YES");
         JOP_NO = l.getLocalized("JOP_NO");
         JOP_ERROR = l.getLocalized("JOP_ERROR");
         JOP_INFO = l.getLocalized("JOP_INFO");
         JOP_WARNING = l.getLocalized("JOP_WARNING");
         NO_DLL = l.getLocalized("NO_DLL");
         NO_WM7_CODECS = l.getLocalized("NO_WM7_CODECS");
         NO_WM9_CODECS = l.getLocalized("NO_WM9_CODECS");
         NO_WM9_CODECS_DETECTED = l.getLocalized("NO_WM9_CODECS_DETECTED");
         NO_WM7_AND_9_CODECS = l.getLocalized("NO_WM7_AND_9_CODECS");
         NO_REDIST_PACKAGE = l.getLocalized("NO_REDIST_PACKAGE");
         SYSTEM_NEEDS_REBOOT = l.getLocalized("SYSTEM_NEEDS_REBOOT");
         NOT_ADMINISTRATOR =  l.getLocalized("NOT_ADMINISTRATOR");
         WM7_RECHECK_FAILED =  l.getLocalized("WM7_RECHECK_FAILED");
         RELOAD_CONFIG_CHANGED = l.getLocalized("RELOAD_CONFIG_CHANGED");
         NO_WMSYSPRX_COPY = l.getLocalized("NO_WMSYSPRX_COPY");
         for (int i = 0; i < BANDWIDTHS.length; ++i)
         {
            BANDWIDTHS[i] = l.getLocalized("BANDWIDTH_" + i);
         }
         MNEM_UNRESTRAINED_WM6_COMPATIBLE = l.getLocalized("MNEM_UNRESTRAINED_WM6_COMPATIBLE").charAt(0);
         MNEM_FULL_WM6_COMPATIBLE = l.getLocalized("MNEM_FULL_WM6_COMPATIBLE").charAt(0);
         MNEM_WM9_COMPATIBLE = l.getLocalized("MNEM_WM9_COMPATIBLE").charAt(0);
//         MNEM_CONFIGURE = l.getLocalized("MNEM_CONFIGURE").charAt(0);
      }
      catch (Exception e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(null, "Access to Locale database [WmpEncoder] failed!\n" +
                                       e.toString(), "Error", 
                                       JOptionPane.ERROR_MESSAGE);
      }
   }

   private PublisherWizardData pwData_;

   private WizardPanel nextWizard_;

///   private JRadioButton strictWm6CompatibleRadio_;
///   private JRadioButton extendedWm6CompatibleRadio_;
///   private JRadioButton wm9CompatibleRadio_;
   
   private JComboBox targetAudienceBox_;

   private JLabel audioDetailLabel_;
   private JLabel videoDetailLabel_;
   private JLabel clipsDetailLabel_;
   private JLabel sumDetailLabel_;

   private JWindow waitingWindow_;
   private JLabel  waitingLabel_; 
   
   private int iPreviousSelectedIndex_;
   private String sLastConfigIdentifier_ = null;
   private boolean bDisableSelectionListening_ = false;
   
   // these two variables:
   // first time do the check in refreshDataDisplay() and
   // after that NOT in displayWizard()
   // every time after that do it only in displayWizard().
   private boolean bFirstCodecCheckDone_ = false;
   private boolean bCodecCheckDoneInRefresh_ = false;
   private boolean bClipsAreScreengrabbing_ = false;
   
   
   // for change check after the extended dialog closes
   private StreamInformation siAudioBackup_ = null;
   private StreamInformation siVideoBackup_ = null;
   private StreamInformation siClipsBackup_ = null;

   
   public WmpEncoderWizardPanel(PublisherWizardData pwData)
   {
      super();

      pwData_ = pwData;

      setPreferredSize(new Dimension(500, 400));

      addButtons(NEXT_BACK_CANCEL);
      useHeaderPanel(HEADER, SUBHEADER, 
                     "/imc/lecturnity/converter/images/windowsmedia_logo.png");
      addButtons(BUTTON_CUSTOM, EXTENDED, 'e');

      Container p = getContentPanel();
      p.setLayout(null);


      createGuiComponents(p);
   }
   
   public String getDescription()
   {
      return "WmpEncoder";
   }

   public int displayWizard()
   {
      if (!bCodecCheckDoneInRefresh_)
         checkForWmCodecs();
      else
         bCodecCheckDoneInRefresh_ = false;

      return super.displayWizard(); 
   }

   public void refreshDataDisplay()
   {
      // gets called everytime before displayWizard()
      // and additionally when customCommand() finishes

      DocumentData dd = pwData_.GetDocumentData();

      String sPresentationFileName = dd.GetPresentationFileName();
      StreamInformation siAudio = dd.GetAudioStreamInfo();
      StreamInformation siVideo = dd.GetVideoStreamInfo();
      StreamInformation siClips = dd.GetClipsStreamInfo();

      if (!bFirstCodecCheckDone_)
      {
         checkForWmCodecs();
         bFirstCodecCheckDone_ = true;
         bCodecCheckDoneInRefresh_ = true;
      }
   
      String sConfigIdentifier = 
         new java.io.File(sPresentationFileName).lastModified() + sPresentationFileName;
      
      if (!pwData_.m_bIsProfileMode)
      {
         if (!sConfigIdentifier.equals(sLastConfigIdentifier_)) // first call, or configuration changed
         {
            // bandwidth check CUSTOM must be reset in this case
            if (targetAudienceBox_.getSelectedIndex() == targetAudienceBox_.getItemCount() -1)
            {
               bDisableSelectionListening_ = true;
               targetAudienceBox_.setSelectedIndex(2);
               iPreviousSelectedIndex_ = 2;
               bDisableSelectionListening_ = false;
               
               // if SelectionListening is not disabled a warning message will appear
            }
      
            scanInputAndSetDefaults(pwData_);
            
            sLastConfigIdentifier_ = sConfigIdentifier;
         }
      }

      
      if (siAudioBackup_ != null) 
      {
         // invoked by the closed extended dialog
         // maybe something was there changed
         
         boolean bOutputSomethingChanged = false;
      
         if (siAudio != null && siAudio.differs(siAudioBackup_))
            bOutputSomethingChanged = true;
      
         if (siVideo != null && siVideo.differs(siVideoBackup_))
            bOutputSomethingChanged = true;
 
         if (siClips != null && siClips.differs(siClipsBackup_))
            bOutputSomethingChanged = true;
 
         if (bOutputSomethingChanged) // set bandwidth selection to "Custom"
            targetAudienceBox_.setSelectedIndex(targetAudienceBox_.getItemCount() -1);
         
         siAudioBackup_ = null;
         siVideoBackup_ = null;
         siClipsBackup_ = null;
      }
      
      // bandwidth selection is NOT "Custom"?
      if (targetAudienceBox_.getSelectedIndex() != targetAudienceBox_.getItemCount() -1)
         determineAndSetAutoParameters(pwData_, targetAudienceBox_.getSelectedIndex());

      displayStreamInformation(pwData_);
      
      super.refreshDataDisplay();
   }

   public void customCommand(String command)
   {
      // update data object
      getWizardData();

      DocumentData dd = pwData_.GetDocumentData();

      StreamInformation siAudio = dd.GetAudioStreamInfo();
      StreamInformation siVideo = dd.GetVideoStreamInfo();
      StreamInformation siClips = dd.GetClipsStreamInfo();

      // in order to determine differences later on:
      siAudioBackup_ = siAudio != null ? new StreamInformation(siAudio) : null;
      siVideoBackup_ = siVideo != null ? new StreamInformation(siVideo) : null;
      siClipsBackup_ = siClips != null ? new StreamInformation(siClips) : null;
      
      ExtendedSettingsDialog esd = new ExtendedSettingsDialog(getFrame(), this, pwData_);
      esd.show();
   }

   public boolean verifyNext() {
       // Bugfix 3863/4507: Special check for video and clips size which should not exceed 3 MegaPixel
       // Bugfix 5731: video and clips size should not exceed 1280*1024.
       int hr = ExtendedSettingsDialog.checkVideoSizes(this, pwData_, false);
       
       if (hr < 0)
           return false;

       return true;
   }

   public PublisherWizardData getWizardData()
   {
      DocumentData dd = pwData_.GetDocumentData();
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      // mostly everything is already done and set in DocumentData.m_XYZStreamInfo
      StreamInformation siAudio = dd.GetAudioStreamInfo();
      StreamInformation siVideo = dd.GetVideoStreamInfo();
      StreamInformation siClips = dd.GetClipsStreamInfo();
      boolean bClipsAndVideoCombined = !ps.GetBoolValue("bShowClipsOnSlides");
      
      ps.SetIntValue("iWmVideoProfileType", PublisherWizardData.PLAYER_CP);
      ps.SetIntValue("iWmClipProfileType", PublisherWizardData.PLAYER_CP);
      
///      int iCodecType = 0;
///      if (strictWm6CompatibleRadio_.isSelected())
///         iCodecType = 0;
///      else if (extendedWm6CompatibleRadio_.isSelected())
///         iCodecType = 1;
///      else if (wm9CompatibleRadio_.isSelected())
///         iCodecType = 2;

      int iCodecType = 2; // WM 9 compatible

      ps.SetIntValue("iWmCodecType", iCodecType);
      ps.SetIntValue("iWmBandwidthType", targetAudienceBox_.getSelectedIndex());

      pwData_.m_strWmVideoProfileDescription = (siAudio.m_nBitrate/1000)+" "+KBPS;
      if (siVideo != null)
      {
         pwData_.m_strWmVideoProfileDescription += " + "+
            (siVideo.m_nBitrate/1000)+" "+KBPS+", "+(siVideo.m_nFrequency/10.0)+" "+FPS;
      }

      if (siClips != null && !bClipsAndVideoCombined)
      {
         pwData_.m_strWmClipsProfileDescription = 
            (siClips.m_nBitrate/1000)+" "+KBPS+", "+(siClips.m_nFrequency/10.0)+" "+FPS;
      }

      // Copied from the obsolete WmpProfileWizardPanel
      // though there is no standard profile video size anymore
      //
      // Check for the profile video size:
      if (ps.GetIntValue("iVideoSizeType") == PublisherWizardData.VIDEOSIZE_STANDARD)
      {
         ps.SetIntValue("iVideoWidth", StreamingMediaConverter.STANDARD_VIDEO_WIDTH);
         ps.SetIntValue("iVideoHeight", StreamingMediaConverter.STANDARD_VIDEO_HEIGHT);
      }
      else
      {
         // otherwise it's already done.
      }

      if (nextWizard_ == null)
         nextWizard_ = new TargetChoiceWizardPanel(pwData_);

      pwData_.nextWizardPanel = nextWizard_;

      return pwData_;
   }
 
   public void actionPerformed(ActionEvent evt)
   {
///      if (evt.getSource() == strictWm6CompatibleRadio_ ||
///          evt.getSource() == extendedWm6CompatibleRadio_ ||
///          evt.getSource() == wm9CompatibleRadio_)
///      {
///         setCodecs(pwData_);
///         
///         // TODO: maybe display/hide warning messages
///      }
      
      if (evt.getSource() == targetAudienceBox_ && !bDisableSelectionListening_)
      {
         int iSelectedIndex = targetAudienceBox_.getSelectedIndex();
         
         boolean bRevertChoice = false;
         if (iPreviousSelectedIndex_ == targetAudienceBox_.getItemCount() -1 &&
             iSelectedIndex != iPreviousSelectedIndex_)
         {
            // selection was custom and isn't anymore
            
            int iAnswer = JOptionPane.showConfirmDialog(this, CUSTOM_PROFILE_DESELECT, 
                                                        JOP_WARNING, 
                                                        JOptionPane.YES_NO_OPTION, 
                                                        JOptionPane.WARNING_MESSAGE);
            
            if (iAnswer == JOptionPane.NO_OPTION)
               bRevertChoice = true;
         }
         
         if (bRevertChoice)
            targetAudienceBox_.setSelectedIndex(iPreviousSelectedIndex_); 
         else
         {
            if (iSelectedIndex != targetAudienceBox_.getItemCount() -1)
               determineAndSetAutoParameters(pwData_, iSelectedIndex);
 
            displayStreamInformation(pwData_);
            
            iPreviousSelectedIndex_ = iSelectedIndex;
         }
      }
   }

   private void scanInputAndSetDefaults(PublisherWizardData pwData)
   {
      DocumentData dd = pwData.GetDocumentData();
      ProfiledSettings ps = pwData.GetProfiledSettings();

      // !!! if (dd.GetAudioStreamInfo() == null)
         dd.SetAudioStreamInfo(new StreamInformation(StreamInformation.AUDIO_STREAM));
      
      // ??? Why are these StreamInformations overwritten here (everytime;
      // called from displayWizard()).
      
      if (pwData.HasNormalVideo())
      {
         if (dd.GetVideoInfo() != null)
         {
            // create a video stream information object
            dd.SetVideoStreamInfo(new StreamInformation(-1, -1, 
               dd.GetVideoInfo().videoFramerateTenths, 
               ps.GetIntValue("iVideoWidth"), ps.GetIntValue("iVideoHeight")));
            
            // ??? Why use ProfiledSettings for width and height here and not 
            // in the other formats as well (ie Flash)?
         }
         else 
         {
            // TODO: Prove the following statement!
            // Known Bug: DocumentData.m_VideoInfo is created/copied in verifyNext()
            // of SelectPresentationWizardPanel.java, 
            // adding a video with SelectActionWizardPanel.java doesn't create it
            
            JOptionPane.showMessageDialog(this, RELOAD_CONFIG_CHANGED, JOP_ERROR, JOptionPane.ERROR_MESSAGE);
            System.exit(1);
         }
      }
      
      if (pwData.HasClips())
      {
         // ??? checkClipsForScreengrabbing() has side effects
         
         bClipsAreScreengrabbing_ = checkClipsForScreengrabbing(dd, null);
      }
   }

   public static boolean checkClipsForScreengrabbing(DocumentData dd, StreamInformation siClips)
   {
      boolean bClipsAreScreengrabbing = true;
      int iFramerateTenths = 0;
      int iMaxClipWidth = 0, iMaxClipHeight = 0;
      
      for (int i = 0; i < dd.GetVideoClipInfos().length; ++i)
      {
         if (dd.GetVideoClipInfos()[i].videoWidth > iMaxClipWidth)
            iMaxClipWidth = dd.GetVideoClipInfos()[i].videoWidth;
         if (dd.GetVideoClipInfos()[i].videoHeight > iMaxClipHeight)
            iMaxClipHeight = dd.GetVideoClipInfos()[i].videoHeight;
         if (dd.GetVideoClipInfos()[i].videoFramerateTenths > iFramerateTenths)
            iFramerateTenths = dd.GetVideoClipInfos()[i].videoFramerateTenths;
         
         File pathDir = new File(dd.GetPresentationFileName()).getParentFile();
         VideoClipInfo vci  = new VideoClipInfo(
            new File(pathDir, dd.GetVideoClipInfos()[i].clipFileName)+"", 
                     dd.GetVideoClipInfos()[i].startTimeMs);
         
         if (NativeUtils.isLibraryLoaded() && NativeUtils.retrieveVideoInfo("", vci))
         {
            String sCodecIdentifier = retrieveCodecString(vci);
            
            boolean bIsSgCodec1 = sCodecIdentifier.equals("lsgc");
            boolean bIsSgCodec2 = sCodecIdentifier.equals("tscc");
            
            if (!bIsSgCodec1 && !bIsSgCodec2)
               bClipsAreScreengrabbing = false;
            
            //System.out.println("bClipsAreScreengrabbing="+bClipsAreScreengrabbing);
         }
      }

      if (siClips == null)
      {
         // create a clips stream information object
         dd.SetClipsStreamInfo(new StreamInformation(-1, -1, 
            iFramerateTenths, iMaxClipWidth, iMaxClipHeight));
      }
      else
      {
         siClips.m_nFrequency = iFramerateTenths;
         siClips.m_nWidth = iMaxClipWidth;
         siClips.m_nHeight = iMaxClipHeight;
      }

      return bClipsAreScreengrabbing;
   }

   /**
    * Determines reasonable parameters for the media streams given
    * a bandwidth constraint.
    * @returns true if something was changed in the stream configurations
    */
   private boolean determineAndSetAutoParameters(PublisherWizardData pwData, int nBitrateIdentifier)
   {
      DocumentData dd = pwData.GetDocumentData();
      ProfiledSettings ps = pwData.GetProfiledSettings();

      // short hand declarations:
      StreamInformation siAudio = dd.GetAudioStreamInfo();
      StreamInformation siVideo = dd.GetVideoStreamInfo();
      StreamInformation siClips = dd.GetClipsStreamInfo();
      
      // in order to determine differences later on:
      StreamInformation siAudioBackup = siAudio != null ? new StreamInformation(siAudio) : null;
      StreamInformation siVideoBackup = siVideo != null ? new StreamInformation(siVideo) : null;
      StreamInformation siClipsBackup = siClips != null ? new StreamInformation(siClips) : null;

      //
      // do the codec selection depending on the compatibility setting
      // and available codecs
      //
//      setCodecs(pwData);
      // TODO: is this required here? Is also called in WmpWriterWizardPanel
      setStreamInfos(pwData, siAudio, siVideo, siClips, PublisherWizardData.WM_CODEC_90);
      
      //
      // set bitrates depending on input properties and bandwidth selection
      //
      autoAdjustBitrates(pwData, siAudio, siVideo, siClips, nBitrateIdentifier);

      
      boolean bOutputSomethingChanged = false;
      
      if (siAudio != null && siAudio.differs(siAudioBackup))
         bOutputSomethingChanged = true;
      
      if (siVideo != null && siVideo.differs(siVideoBackup))
         bOutputSomethingChanged = true;
 
      if (siClips != null && siClips.differs(siClipsBackup))
         bOutputSomethingChanged = true;
     

      // Transfer StreamInfo changes to ProfileSettings
      if (siAudio != null)
         ps.SetIntValue("iWmAudioBitrate", siAudio.m_nBitrate);
      if (siVideo != null)
         ps.SetIntValue("iWmVideoBitrate", siVideo.m_nBitrate);
      if (siClips != null)
         ps.SetIntValue("iWmClipBitrate", siClips.m_nBitrate);

      return bOutputSomethingChanged;
   }
   
   public static void autoAdjustBitrates(PublisherWizardData pwData, 
           StreamInformation siAudio, 
           StreamInformation siVideo, 
           StreamInformation siClips, 
           int nBitrateIdentifier) {
       //
       // set bitrates depending on input properties and bandwidth selection
       //
       boolean bInputSgClipsGiven = (siClips != null);
       boolean bInputVideoGiven = (siVideo != null);
       boolean bInputCombineVideoAndClips = !pwData.GetProfiledSettings().GetBoolValue("bShowClipsOnSlides");
       
       if (bInputVideoGiven && bInputSgClipsGiven && !bInputCombineVideoAndClips) // A + V + C
       {
          switch (nBitrateIdentifier)
          {
             case 0: // Modem/ISDN
                siAudio.m_nBitrate = 6000;
                siVideo.m_nBitrate = 16000;
                siClips.m_nBitrate = 22000;
                break;
             case 1: // Dual ISDN
                siAudio.m_nBitrate = 10000;
                siVideo.m_nBitrate = 48000;
                siClips.m_nBitrate = 48000;
                break;
             case 2: // DSL
                siAudio.m_nBitrate = 20000;
                siVideo.m_nBitrate = 120000;
                siClips.m_nBitrate = 150000;
                break;
             case 3: // LAN
                siAudio.m_nBitrate = 48000;
                siVideo.m_nBitrate = 500000;
                siClips.m_nBitrate = 200000;
                break;
          }
       }
       else if (bInputVideoGiven || bInputSgClipsGiven) // A + V
       {
          StreamInformation siThat = bInputVideoGiven ? siVideo : siClips;
          
          switch (nBitrateIdentifier)
          {
             case 0: // Modem/ISDN
                siAudio.m_nBitrate = 10000;
                siThat.m_nBitrate = 34000;
                break;
             case 1: // Dual ISDN
                siAudio.m_nBitrate = 20000;
                siThat.m_nBitrate = 84000;
                break;
             case 2: // DSL
                siAudio.m_nBitrate = 48000;
                if (!bInputSgClipsGiven || bInputVideoGiven && bInputCombineVideoAndClips)
                   siThat.m_nBitrate = 250000;
                else
                   siThat.m_nBitrate = 150000;
                break;
             case 3: // LAN
                siAudio.m_nBitrate = 48000;
                if (!bInputSgClipsGiven || bInputVideoGiven && bInputCombineVideoAndClips)
                   siThat.m_nBitrate = 600000;
                else
                   siThat.m_nBitrate = 200000;
                break;
          }
       }
       else // A only
       {
          switch (nBitrateIdentifier)
          {
             case 0: // Modem/ISDN
                siAudio.m_nBitrate = 20000;
                break;
             case 1: // Dual ISDN
             case 2: // DSL
             case 3: // LAN
                siAudio.m_nBitrate = 48000;
                break;
          }
       }       
   }
   
   // This method is 'public static' because it is not only called from 'setCodecs()' (see below)
   // but also called from WmpWriterWizardPanel in case of 'Profile mode' 
   public static void setStreamInfos(PublisherWizardData pwData, 
                                     StreamInformation siAudio, 
                                     StreamInformation siVideo, 
                                     StreamInformation siClips, 
                                     int iWmCodecType)
   {
      DocumentData dd = pwData.GetDocumentData();

///      boolean bIs9erInstalled = 
///         WindowsMediaConverter.isEngineLoaded() && WindowsMediaConverter.is9erCodecsInstalled();
      boolean bVideoIsScreengrabbing = false;
      boolean bClipsAreScreengrabbing = true; 
      if (pwData.HasClips())
         checkClipsForScreengrabbing(dd, siClips);

      //
      // Do the codec selection depending on the compatibility setting
      // and available codecs
      //

      siAudio.m_nCodecIndex = StreamInformation.CODEC_AUDIO_WMA2;

      if (pwData.HasNormalVideo())
      {
         String sCodecIdentifier = "";
         if (dd.GetVideoInfo() != null)
            sCodecIdentifier = WmpEncoderWizardPanel.retrieveCodecString(dd.GetVideoInfo());

         boolean bIsSgCodec1 = sCodecIdentifier.equals("lsgc");
         boolean bIsSgCodec2 = sCodecIdentifier.equals("tscc");
         if (bIsSgCodec1 || bIsSgCodec2)
            bVideoIsScreengrabbing = true;

///         if (bIs9erInstalled)
///            siVideo.m_nCodecIndex = StreamInformation.CODEC_VIDEO_WMV9;
///         else
///            siVideo.m_nCodecIndex = StreamInformation.CODEC_VIDEO_WMV7;
///         
///         if (bVideoIsScreengrabbing)
///            siVideo.m_nCodecIndex = StreamInformation.CODEC_VIDEO_SG7;

            siVideo.m_nCodecIndex = StreamInformation.CODEC_VIDEO_WMV9;
            
            if (bVideoIsScreengrabbing)
               siVideo.m_nCodecIndex = StreamInformation.CODEC_VIDEO_SG9;
      }
      
      if (pwData.HasClips())
      {
///         if (bClipsAreScreengrabbing)
///            siClips.m_nCodecIndex = StreamInformation.CODEC_VIDEO_SG7;
///         else
///            siClips.m_nCodecIndex = StreamInformation.CODEC_VIDEO_WMV7;

         if (bClipsAreScreengrabbing)
            siClips.m_nCodecIndex = StreamInformation.CODEC_VIDEO_SG9;
         else
            siClips.m_nCodecIndex = StreamInformation.CODEC_VIDEO_WMV9;
      }

///      if (iWmCodecType == PublisherWizardData.WM_CODEC_90)
///      {
///         if (siClips != null && bIs9erInstalled)
///         {
///            if (bClipsAreScreengrabbing)
///               siClips.m_nCodecIndex = StreamInformation.CODEC_VIDEO_SG9;
///            else
///               siClips.m_nCodecIndex = StreamInformation.CODEC_VIDEO_WMV9;
///         }
///         
///         if (siVideo != null && bVideoIsScreengrabbing)
///            siVideo.m_nCodecIndex = StreamInformation.CODEC_VIDEO_SG9;
///      }
///      else if (iWmCodecType == PublisherWizardData.WM_CODEC_64)
///      {
///         if (siVideo != null)
///            siVideo.m_nCodecIndex = StreamInformation.CODEC_VIDEO_MP43;
///         if (siClips != null)
///            siClips.m_nCodecIndex = StreamInformation.CODEC_VIDEO_MP43;
///      }
   }

//   private void setCodecs(PublisherWizardData pwData)
//   {
//      DocumentData dd = pwData.GetDocumentData();
////      ProfiledSettings ps = pwData.GetProfiledSettings();
//
//      // short hand declarations:
//      StreamInformation siAudio = dd.GetAudioStreamInfo();
//      StreamInformation siVideo = dd.GetVideoStreamInfo();
//      StreamInformation siClips = dd.GetClipsStreamInfo();
//
/////      int iWmCodecType = PublisherWizardData.WM_CODEC_80;
/////      if (wm9CompatibleRadio_.isSelected())
/////         iWmCodecType = PublisherWizardData.WM_CODEC_90;
/////      else if (strictWm6CompatibleRadio_.isSelected())
/////         iWmCodecType = PublisherWizardData.WM_CODEC_64;
//
//      int iWmCodecType = PublisherWizardData.WM_CODEC_90;
//
//      setStreamInfos(pwData, siAudio, siVideo, siClips, 
//                     iWmCodecType);
//   }

   public static String retrieveCodecString(VideoClipInfo vci)
   {
      /*
      System.out.println((char)((vci.fccCodec) & 0xff) + " " +
      (char)((vci.fccCodec >> 8) & 0xff) + " " +
      (char)((vci.fccCodec >> 16) & 0xff) + " " +
      (char)((vci.fccCodec >> 24) & 0xff) + " ");
      */
               
      // BUGFIX: the codec identifier (in the file) must not be lower case
      // especially older lsgc is identified as LSGC
      byte[] baCodecSeed = new byte[] { 
         (byte)(vci.fccCodec & 0xff), (byte)((vci.fccCodec >> 8) & 0xff),
         (byte)((vci.fccCodec >> 16) & 0xff), (byte)((vci.fccCodec >> 24) & 0xff)
      };
               
      String sCodecIdentifier = new String(baCodecSeed);
      sCodecIdentifier = sCodecIdentifier.toLowerCase(); 
      //System.out.println(sCodecIdentifier);
               
      /*
      int iSgCodec1 = 
      (((int)'l') & 0xff) | (((int)'s') & 0xff) << 8  | (((int)'g') & 0xff) << 16 | (((int)'c') & 0xff) << 24;
      int iSgCodec2 = 
      (((int)'t') & 0xff) | (((int)'s') & 0xff) << 8  | (((int)'c') & 0xff) << 16 | (((int)'c') & 0xff) << 24;
      if (vci.fccCodec != iSgCodec1 && vci.fccCodec != iSgCodec2)
      {
      bClipsAreScreengrabbing = false;
      }
               */
      
      return sCodecIdentifier;
   }
   
   private void displayStreamInformation(PublisherWizardData pwData)
   {
      DocumentData dd = pwData.GetDocumentData();
      ProfiledSettings ps = pwData.GetProfiledSettings();

      StreamInformation siAudio = dd.GetAudioStreamInfo();
      StreamInformation siVideo = dd.GetVideoStreamInfo();
      StreamInformation siClips = dd.GetClipsStreamInfo();
      boolean bClipsAndVideoCombined = !ps.GetBoolValue("bShowClipsOnSlides");
      
      int nBitrateSum = siAudio.m_nBitrate;
      
      audioDetailLabel_.setText((siAudio.m_nBitrate/1000)+" "+KBPS);
      
      videoDetailLabel_.setText(NOT_PRESENT);
      if (siVideo != null)
      {
         String text = "";
         
         nBitrateSum += siVideo.m_nBitrate;
         text = (siVideo.m_nBitrate/1000)+" "+KBPS+", "+(siVideo.m_nFrequency/10.0)+" "+FPS;
         
         videoDetailLabel_.setText(text);
      }

      clipsDetailLabel_.setText(NOT_PRESENT);
      if (siClips != null)
      {
         if (!bClipsAndVideoCombined || pwData.IsDenverDocument())
         {
            String text = "";
         
            nBitrateSum += siClips.m_nBitrate;
            text = (siClips.m_nBitrate/1000)+" "+KBPS+", "+(siClips.m_nFrequency/10.0)+" "+FPS;
         
            clipsDetailLabel_.setText(text);
         }
         else if (siVideo != null)
            clipsDetailLabel_.setText(VIDEO_CLIPS_COMBINED);
      }

      sumDetailLabel_.setText((nBitrateSum/1000)+" "+KBPS);
   }
   
   private void createGuiComponents(Container container)
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

///      JPanel compatibilityPanel = new JPanel();
///      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
///         compatibilityPanel.setBackground(Wizard.BG_COLOR);
///      compatibilityPanel.setLayout(null);
///      compatibilityPanel.setSize(460, 90);
///      compatibilityPanel.setLocation(20, 10);
///      compatibilityPanel.setBorder(new TitledBorder(" " + COMPATIBILITY_SETTINGS + " "));
///      container.add(compatibilityPanel);
///
///      ButtonGroup bg = new ButtonGroup();
///
///      strictWm6CompatibleRadio_ = new JRadioButton(UNRESTRAINED_WM6_COMPATIBLE, false);
///      strictWm6CompatibleRadio_.setSize(445, 20);
///      strictWm6CompatibleRadio_.setLocation(10, 20);
///      strictWm6CompatibleRadio_.setMnemonic(MNEM_UNRESTRAINED_WM6_COMPATIBLE);
///      strictWm6CompatibleRadio_.addActionListener(this);
///      strictWm6CompatibleRadio_.setSelected(ps.GetIntValue("iWmVideoProfileType") == PublisherWizardData.PLAYER_64);
///      compatibilityPanel.add(strictWm6CompatibleRadio_);
///      bg.add(strictWm6CompatibleRadio_);
///
///      extendedWm6CompatibleRadio_ = new JRadioButton(FULL_WM6_COMPATIBLE, true);
///      extendedWm6CompatibleRadio_.setSize(445, 20);
///      extendedWm6CompatibleRadio_.setLocation(10, 40);
///      extendedWm6CompatibleRadio_.setMnemonic(MNEM_FULL_WM6_COMPATIBLE);
///      extendedWm6CompatibleRadio_.addActionListener(this);
///      compatibilityPanel.add(extendedWm6CompatibleRadio_);
///      bg.add(extendedWm6CompatibleRadio_);
///
///      wm9CompatibleRadio_ = new JRadioButton(WM9_COMPATIBLE, true);
///      wm9CompatibleRadio_.setSize(445, 20);
///      wm9CompatibleRadio_.setLocation(10, 60);
///      wm9CompatibleRadio_.setMnemonic(MNEM_WM9_COMPATIBLE);
///      wm9CompatibleRadio_.addActionListener(this);
///      compatibilityPanel.add(wm9CompatibleRadio_);
///      bg.add(wm9CompatibleRadio_);

      // Target Audience
      JPanel targetAudiencePanel = new JPanel();
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         targetAudiencePanel.setBackground(Wizard.BG_COLOR);
      targetAudiencePanel.setLayout(null);
      targetAudiencePanel.setSize(460, 85);
///      targetAudiencePanel.setLocation(20, 105);
      targetAudiencePanel.setLocation(20, 25);
      targetAudiencePanel.setBorder(new TitledBorder(" " + TARGET_AUDIENCE + " "));
      container.add(targetAudiencePanel);

      JLabel targetAudienceDescr1Label = new JLabel(TARGET_AUDIENCE_DESCR1);
      targetAudienceDescr1Label.setSize(460, 22);
      targetAudienceDescr1Label.setLocation(20, 20);
      targetAudiencePanel.add(targetAudienceDescr1Label);

      WizardTextArea targetAudienceDescr2Label = new WizardTextArea
         (TARGET_AUDIENCE_DESCR2,
          new Point(20, 42), new Dimension(250, 38),
          imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      targetAudiencePanel.add(targetAudienceDescr2Label);

      targetAudienceBox_ = new JComboBox();
      targetAudienceBox_.setSize(170, 22);
      targetAudienceBox_.setLocation(270, 45);
      for (int i = 0; i < BANDWIDTHS.length; ++i)
      {
         targetAudienceBox_.addItem(BANDWIDTHS[i]);
      }
      targetAudienceBox_.setSelectedIndex(2);
      iPreviousSelectedIndex_ = 2;
      targetAudienceBox_.addActionListener(this);
      targetAudiencePanel.add(targetAudienceBox_);

      // Detail Settings
      JPanel detailSettingsPanel = new JPanel();
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         detailSettingsPanel.setBackground(Wizard.BG_COLOR);
      detailSettingsPanel.setLayout(null);
      detailSettingsPanel.setSize(460, 85);
///      detailSettingsPanel.setLocation(20, 195);
      detailSettingsPanel.setLocation(20, 125);
      detailSettingsPanel.setBorder(new TitledBorder(" " + DETAIL_SETTINGS + " "));
      container.add(detailSettingsPanel);

      JLabel audioLabel = new JLabel(DETAIL_AUDIO);
      audioLabel.setSize(150, 22);
      audioLabel.setLocation(20, 20);
      detailSettingsPanel.add(audioLabel);

      audioDetailLabel_ = new JLabel(NOT_PRESENT);
      audioDetailLabel_.setSize(240, 22);
      audioDetailLabel_.setLocation(110, 20);
      detailSettingsPanel.add(audioDetailLabel_);

      JLabel videoLabel = new JLabel(DETAIL_VIDEO);
      videoLabel.setSize(150, 22);
      videoLabel.setLocation(20, 38);
      detailSettingsPanel.add(videoLabel);

      videoDetailLabel_ = new JLabel(NOT_PRESENT);
      videoDetailLabel_.setSize(240, 22);
      videoDetailLabel_.setLocation(110, 38);
      detailSettingsPanel.add(videoDetailLabel_);

      JLabel clipsLabel = new JLabel(DETAIL_CLIPS);
      clipsLabel.setSize(150, 22);
      clipsLabel.setLocation(20, 56);
      detailSettingsPanel.add(clipsLabel);

      clipsDetailLabel_ = new JLabel(NOT_PRESENT);
      clipsDetailLabel_.setSize(240, 22);
      clipsDetailLabel_.setLocation(110, 56);
      detailSettingsPanel.add(clipsDetailLabel_);
      
      JLabel sumLabel = new JLabel(DETAIL_SUM);
      sumLabel.setSize(100, 22);
      sumLabel.setLocation(320, 56);
      detailSettingsPanel.add(sumLabel);

      sumDetailLabel_ = new JLabel(NOT_PRESENT);
      sumDetailLabel_.setSize(240, 22);
      sumDetailLabel_.setLocation(374, 56);
      detailSettingsPanel.add(sumDetailLabel_);
      
      if (pwData_.m_bIsProfileMode)
         initProfileValues();
   }
   
   private void initProfileValues()
   {
      DocumentData dd = pwData_.GetDocumentData();
      ProfiledSettings ps = pwData_.GetProfiledSettings();

//      int iCodecType = ps.GetIntValue("iWmCodecType");
///      if (iCodecType == 0)
///         strictWm6CompatibleRadio_.setSelected(true);
///      else if (iCodecType == 1)
///         extendedWm6CompatibleRadio_.setSelected(true);
///      else if (iCodecType == 2)
///         wm9CompatibleRadio_.setSelected(true);
      
      int iAudioBitrate = ps.GetIntValue("iWmAudioBitrate");
      int iVideoBitrate = ps.GetIntValue("iWmVideoBitrate");
      int iClipBitrate = ps.GetIntValue("iWmClipBitrate");
      int iVideoFramerateTenths = ps.GetIntValue("iVideoFramerateTenths");
      int iClipFramerateTenths = ps.GetIntValue("iClipFramerateTenths");

      StreamInformation siAudio = dd.GetAudioStreamInfo();
      StreamInformation siVideo = dd.GetVideoStreamInfo();
      StreamInformation siClips = dd.GetClipsStreamInfo();

      // Create dummy Stream Infos, if necessary      
      // Note: for Generic Profiles all streams are required to fill all options
      //       bitrates are adapted on publishing (except custom bandwidth)
      boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && ps.GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
      if (siAudio == null)
         dd.SetAudioStreamInfo(new StreamInformation(-1, iAudioBitrate, -1, -1));
      if (siVideo == null && (dd.HasNormalVideo() || bIsGenericProfile))
         dd.SetVideoStreamInfo(new StreamInformation(-1, iVideoBitrate, iVideoFramerateTenths, 320, 240));
      if (siClips == null && ((dd.HasClips() && ps.GetBoolValue("bShowClipsOnSlides")) || bIsGenericProfile))
         dd.SetClipsStreamInfo(new StreamInformation(-1, iClipBitrate, iClipFramerateTenths, 320, 240));

      int iBandwidthType = ps.GetIntValue("iWmBandwidthType");
      bDisableSelectionListening_ = true;
      targetAudienceBox_.setSelectedIndex(iBandwidthType);
      bDisableSelectionListening_ = false;

      displayStreamInformation(pwData_);
   }

   private void createWaitingWindow()
   {
      int x = this.getLocationOnScreen().x + (int)(0.5 * this.getSize().width) - 100;
      int y = this.getLocationOnScreen().y + (int)(0.5 * this.getSize().height) - 25;
      waitingWindow_ = new JWindow(this.getFrame());
      waitingWindow_.setLocation(x, y);
      waitingWindow_.setSize(200, 50);
      Container r = waitingWindow_.getContentPane();
      r.setLayout(new BorderLayout());
      JPanel p = new JPanel();
      p.setLayout(null);
      p.setPreferredSize(new Dimension(200, 50));
      p.setBackground(javax.swing.UIManager.getColor("ToolTip.background"));
      p.setBorder(new LineBorder(Color.black, 1));
      waitingLabel_ = new JLabel(INSTALLING_MESSAGE);
      waitingLabel_.setSize(200, 50);
      waitingLabel_.setFont(NORMAL_FONT);
      waitingLabel_.setHorizontalAlignment(SwingConstants.CENTER);
      waitingLabel_.setVerticalAlignment(SwingConstants.CENTER);
      p.add(waitingLabel_);
      r.add(p, BorderLayout.CENTER);
      
      return;
   }
   
   public static int CheckForWmCodecs(PublisherWizardData pwData, WizardPanel parent, 
                                      JWindow waitingWindow, JLabel waitingLabel, int[] jopRes) 
   {
      // Option Dialog
      Object[] buttons = {JOP_YES, JOP_NO};
      Object init = JOP_YES;

      int res = NativeUtils.SUCCESS;

///      // Check for WM 7 Codecs
///      boolean bIs7erCodecsInstalled = 
///         WindowsMediaConverter.isEngineLoaded() && WindowsMediaConverter.is7erCodecsInstalled();
///      
///      if (bIs7erCodecsInstalled)
///      {
         // Check for WM 9 Codecs
         boolean bIs9erCodecsInstalled = 
            WindowsMediaConverter.isEngineLoaded() && WindowsMediaConverter.is9erCodecsInstalled();
         
         if (!bIs9erCodecsInstalled && !osIsNT40())
         {
            String sDialogText = NO_WM9_CODECS_DETECTED;
            
            if (!isUserAdmin())
            {
               sDialogText += "\n\n"+NOT_ADMINISTRATOR;
               
               JOptionPane.showMessageDialog(parent, sDialogText, JOP_INFO, JOptionPane.INFORMATION_MESSAGE);
            }
            else
            {
               jopRes[0] = JOptionPane.showOptionDialog(parent,  
                                                        sDialogText, 
                                                        JOP_INFO, 
                                                        JOptionPane.YES_NO_OPTION, 
                                                        JOptionPane.INFORMATION_MESSAGE, 
                                                        null, buttons, init);
            }
            
            if (jopRes[0] == JOptionPane.YES_OPTION)
            {
               // Optional: Install WM 9 Format Package
               res = InstallRedistributionPackage(pwData, WM9_FORMAT_PACKAGE, parent, 
                                                  waitingWindow, waitingLabel);
            }
         }
///      }
///      else // No WM 7 Codecs installed 
///      {
///         // Check for WM 9 Codecs
///         boolean bIs9erCodecsInstalled = 
///            WindowsMediaConverter.isEngineLoaded() && WindowsMediaConverter.is9erCodecsInstalled();
///         
///         if (!bIs9erCodecsInstalled)
///         {
///            String sDialogText = NO_WM7_AND_9_CODECS;
///            if (osIsNT40())
///               sDialogText = NO_WM7_CODECS;
///            
///            if (!isUserAdmin())
///            {
///               sDialogText += "\n\n"+NOT_ADMINISTRATOR;
///               
///               JOptionPane.showMessageDialog(parent, sDialogText, JOP_WARNING, JOptionPane.WARNING_MESSAGE);
///            }
///            else
///            {
///               jopRes[0] = JOptionPane.showOptionDialog(parent,  
///                                                        sDialogText, 
///                                                        JOP_WARNING, 
///                                                        JOptionPane.YES_NO_OPTION, 
///                                                        JOptionPane.WARNING_MESSAGE, 
///                                                        null, buttons, init);
///            }
///            
///            if (jopRes[0] == JOptionPane.YES_OPTION)
///            {
///               // A must: Install WM 7 Encoder Package
///               res = InstallRedistributionPackage(pwData, WM7_ENCODER_PACKAGE, parent, 
///                                                  waitingWindow, waitingLabel);
///
///               if (res == NativeUtils.SUCCESS && !osIsNT40())
///               {
///                  // Optional: Install WM 9 Format Package
///                  res = InstallRedistributionPackage(pwData, WM9_FORMAT_PACKAGE, parent, 
///                                                     waitingWindow, waitingLabel);
///               }
///            }
///
///         }
///         else
///         {
///            String sDialogText = NO_WM7_CODECS;
///            
///            if (!isUserAdmin())
///            {
///               sDialogText += "\n\n"+NOT_ADMINISTRATOR;
///               
///               JOptionPane.showMessageDialog(parent, sDialogText, JOP_WARNING, JOptionPane.WARNING_MESSAGE);
///            }
///            else
///            {
///               jopRes[0] = JOptionPane.showOptionDialog(parent,  
///                                                        sDialogText, 
///                                                        JOP_WARNING, 
///                                                        JOptionPane.YES_NO_OPTION, 
///                                                        JOptionPane.WARNING_MESSAGE, 
///                                                        null, buttons, init);
///            }
///            
///            if (jopRes[0] == JOptionPane.YES_OPTION)
///            {
///               // A must: Install WM 7 Encoder Package
///               res = InstallRedistributionPackage(pwData, WM7_ENCODER_PACKAGE, parent, 
///                                                  waitingWindow, waitingLabel);
///            }
///         }
///      }

      return res;
   }

   private void checkForWmCodecs()
   {
      if (waitingWindow_ == null)
         createWaitingWindow();
      waitingWindow_.hide();
      
      // Check, if WindowsMediaConverter DLLs are loaded
      //
      // NO! Don't do this check.
      // avitowmv.dll cannot be loaded as long as there is no wmvcore.dll!
      if (false && !WindowsMediaConverter.isEngineLoaded())
      {
         setEnableButton(BUTTON_NEXT, false);
         setEnableButton(BUTTON_BACK | BUTTON_CANCEL, true);
         setEnableButton(BUTTON_CUSTOM, false);

         // Error Message
         JOptionPane.showMessageDialog(this, NO_DLL, JOP_ERROR,
                                       JOptionPane.ERROR_MESSAGE);

         return;
      }

      int[] jopRes = new int[1];
      jopRes[0] = JOptionPane.NO_OPTION;

      // Call the static "CheckForWmCodecs()" method
      int res = CheckForWmCodecs(pwData_, this, waitingWindow_, waitingLabel_, jopRes);

      // Check for the flag 'System needs reboot' --> Abort
      if ((jopRes[0] == JOptionPane.YES_OPTION) && (res == NativeUtils.SYSTEM_NEEDS_REBOOT))
      {
         setEnableButton(BUTTON_NEXT, false);
         setEnableButton(BUTTON_BACK, false);
         setEnableButton(BUTTON_CANCEL, true);
         setEnableButton(BUTTON_CUSTOM, false);
         return;
      }
      else // Recheck for WM 7 codecs is not done any more --> enable buttons
      {
         setEnableButton(BUTTON_NEXT, true);
         setEnableButton(BUTTON_BACK | BUTTON_CANCEL, true);
         setEnableButton(BUTTON_CUSTOM, true);
      }
      
///      // Recheck for WM 7 codecs
///      boolean bIs7erCodecsInstalled = 
///         WindowsMediaConverter.isEngineLoaded() && WindowsMediaConverter.is7erCodecsInstalled();
///
///      if (bIs7erCodecsInstalled)
///      {
///         setEnableButton(BUTTON_NEXT, true);
///         setEnableButton(BUTTON_BACK | BUTTON_CANCEL, true);
///         setEnableButton(BUTTON_CUSTOM, true);
///      }
///      else
///      {
///         setEnableButton(BUTTON_NEXT, false);
///         setEnableButton(BUTTON_CANCEL, true);
///         setEnableButton(BUTTON_CUSTOM, false);
///         
///         // Bug 2014: Redistribution Packages have been installed, but the recheck has failed
///         if ((jopRes[0] == JOptionPane.YES_OPTION) && (res == NativeUtils.SUCCESS))
///         {
///            setEnableButton(BUTTON_BACK, false); // If one goes back and returns to this dialog, the question for WM 7 codecs appears again. Abort Publisher here
///            JOptionPane.showMessageDialog(this, WM7_RECHECK_FAILED, JOP_WARNING, JOptionPane.WARNING_MESSAGE);
///         }
///         else
///         {
///            setEnableButton(BUTTON_BACK, true);
///         }
///      }
   }

   private static int InstallRedistributionPackage(PublisherWizardData pwData, int packageType, 
                                                   WizardPanel parent, 
                                                   JWindow waitingWindow, JLabel waitingLabel) 
   {
      int res = NativeUtils.SUCCESS;
      
      boolean bInstallDirAvailable = false;
      String strInstallDir = "";
      bInstallDirAvailable = pwData.B_INSTALL_DIR_AVAILABLE;
      strInstallDir = pwData.STR_INSTALL_DIR;
      
      if (bInstallDirAvailable)
      {
         /// Warning: Hard coded subdirectory!
         String installPath = strInstallDir + "Publisher\\";
         System.out.println("Redistribution Package Path: " + installPath);
      
         if (waitingLabel != null)
            waitingLabel.setText(INSTALLING_MESSAGE);
         if (waitingWindow != null)
         waitingWindow.show();

         if (packageType == WM9_FORMAT_PACKAGE)
         {
            // Now execute the WM Format 9 Redistribution Package
            res = NativeUtils.doInstallWmFormatRedistribution(installPath, true, false);
         }
///         else
///         {
///            // Execute the WM Encoder 7 Redistribution Package
///            res = NativeUtils.doInstallWmEncoderRedistribution(installPath, true, false);
///            
///            // Check for "WmSysPrx.prx" in Windows root directory
///            if (res == NativeUtils.SUCCESS)
///            {
///               String strSystemRoot = NativeUtils.getEnvVariable("SYSTEMROOT");
///               String strSource = installPath + "WmSysPrx.prx";
///               String strDestination = strSystemRoot + "\\WmSysPrx.prx";
///               //System.out.println("\n--- " + strSource + "\n--- " + strDestination + "\n");
///               File sourceFile = new File(strSource);
///               File destinationFile = new File(strDestination);
///
///               if (!destinationFile.exists())
///               {
///                  if (sourceFile.exists())
///                  {
///                     // We have to copy "WmSysPrx.prx" into the Windows root directory
///                     try
///                     {
///                        FileUtils.copyFile(strSource, strDestination, null, "");
///                     }
///                     catch (IOException e)
///                     {
///                        e.printStackTrace();
///                        res = StreamingMediaConverter.NO_WMSYSPRX_COPY;
///                     }
///                  }
///                  else
///                     res = StreamingMediaConverter.NO_WMSYSPRX_COPY;
///               }
///            }
///         }
         
         if (res == NativeUtils.SUCCESS)
         {
///            // Check, if Installation of the Redistribution Package was cancelled
///            // ==> Check codecs again 
///            // If there are still no WM7 codecs, return failure
///            boolean bIs7erCodecsInstalled 
///               = WindowsMediaConverter.isEngineLoaded() && WindowsMediaConverter.is7erCodecsInstalled();
///            if (!bIs7erCodecsInstalled)
///            {
///               System.out.println("WME Redistribution Package not installed !!");
///               if (waitingWindow != null)
///                  waitingWindow.hide();
///               return NativeUtils.NO_REDIST_PACKAGE;
///            }
///            else
///            {
               //System.out.println("WME Redistribution Package successfully installed!");
               if (waitingWindow != null)
                  waitingWindow.hide();
               return NativeUtils.SUCCESS;
///            }
         }
         else if (res == NativeUtils.NO_REDIST_PACKAGE)
         {
            // Error Message
            System.out.println("No WME Redistribution Package available !!");
            JOptionPane.showMessageDialog(parent, NO_REDIST_PACKAGE, JOP_ERROR, 
                                          JOptionPane.ERROR_MESSAGE);
            if (waitingWindow != null)
               waitingWindow.hide();
            return NativeUtils.NO_REDIST_PACKAGE;
         }
         else if (res == NativeUtils.SYSTEM_NEEDS_REBOOT)
         {
            // Information Message
            System.out.println("System needs reboot !!");
            JOptionPane.showMessageDialog(parent, SYSTEM_NEEDS_REBOOT, JOP_INFO, 
                                          JOptionPane.INFORMATION_MESSAGE);
            if (waitingWindow != null)
               waitingWindow.hide();
            return NativeUtils.SYSTEM_NEEDS_REBOOT;
         }
///         else if (res == StreamingMediaConverter.NO_WMSYSPRX_COPY)
///         {
///            // Error Message
///            System.out.println("WmSysPrx copy failed !!");
///            JOptionPane.showMessageDialog(parent, NO_WMSYSPRX_COPY, JOP_ERROR, 
///                                          JOptionPane.ERROR_MESSAGE);
///            if (waitingWindow != null)
///               waitingWindow.hide();
///            return StreamingMediaConverter.NO_WMSYSPRX_COPY;
///         }
         else
         {
            // This case should be impossible
            if (waitingWindow != null)
               waitingWindow.hide();
            return NativeUtils.NO_REDIST_PACKAGE;
         }
      }
      else // No Install Path
      {
         // Error Message
         System.out.println("No Install Path available !!");
         JOptionPane.showMessageDialog(parent, NO_REDIST_PACKAGE, JOP_ERROR, 
                                       JOptionPane.ERROR_MESSAGE);
         if (waitingWindow != null)
            waitingWindow.hide();
         return NativeUtils.NO_REDIST_PACKAGE;
      }
   }
   
   private static boolean osIsNT40()
   {
      // Check the operating system:
      // We can't install the WM Format 9 Redistribution Package on NT 4.0
      String osName = System.getProperty("os.name");
      //String osArch = System.getProperty("os.arch");
      String osVersion = System.getProperty("os.version");
      
      if ( osName.equalsIgnoreCase("Windows NT") 
         && osVersion.equalsIgnoreCase("4.0") )
      {
         // Special case: NT 4.0
         return true;
      }
      else
      {
         return false;
      }
      
   }
   
   private static boolean isUserAdmin()
   {
      return NativeUtils.isLibraryLoaded() && NativeUtils.isUserAdmin();
   }
}
