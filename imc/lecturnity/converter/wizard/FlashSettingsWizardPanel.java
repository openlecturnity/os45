package imc.lecturnity.converter.wizard;

import javax.swing.*;

import java.awt.Container;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Window;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.*;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.swing.*;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.ui.DirectoryChooser;
import imc.lecturnity.util.ui.LayoutUtils;
import imc.lecturnity.util.wizards.WizardData;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.StreamingMediaConverter;
import imc.lecturnity.converter.StreamInformation;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;

public class FlashSettingsWizardPanel extends WizardPanel implements ActionListener
{
   protected static String DEFAULT_DIRECTORY = "flash";

   private static Localizer g_Localizer = null;
   static 
   {
      try
      {
         g_Localizer = new Localizer(
            "/imc/lecturnity/converter/wizard/FlashSettingsWizardPanel_", "en");
      }
      catch (IOException exc)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }
   
   // Note:
   // FlashScormTemplateWizardPanel and FlashSettingsWizardPanel changed in order of appearance
   private FlashScormTemplateWizardPanel m_NextWizardPanel;

   private PublisherWizardData m_pwData;

   private Dimension m_sizeMovieNormal;
   private Dimension m_sizeMoviePsp;
   private boolean m_bTempLec3FlashPlayerState = false;
   // Bugfix 5247: 2 "Remember" variables for layout and movie size
   // Assume "custom" (user defined) layout for LEC 3 Flash Player
   private int m_iTempFlashMovieSizeType = PublisherWizardData.MOVIESIZE_CUSTOM; 
   private Dimension m_sizeMovieTemp;
   
   private JCheckBox    m_chkFlashVersion6;
   private JCheckBox    m_chkLec3FlashPlayer;
   //private JCheckBox    m_chkSlidestar;
   private JCheckBox    m_chkPspDesign;
   private JLabel       m_lblBigness;
   private JLabel       m_audioDetailLabel;
   private JLabel       m_videoDetailLabel;
   private JLabel       m_clipsDetailLabel;
   
   public FlashSettingsWizardPanel(PublisherWizardData pwData)
   {
      super();

      setPreferredSize(new Dimension(500, 400));

      m_pwData = pwData;
      
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      Dimension dimWb = new Dimension(ps.GetIntValue("iFlashMovieWidth"), 
                                      ps.GetIntValue("iFlashMovieHeight"));
      if ((dimWb.width <= 0) ||  (dimWb.height <= 0))
         dimWb = pwData.FlashGetWbSizeWithBorders();
      m_sizeMovieNormal = new Dimension(dimWb.width, dimWb.height);
      m_sizeMovieTemp = new Dimension(dimWb.width, dimWb.height);

      m_sizeMoviePsp = new Dimension(LPLibs.PSP_STANDARD_WIDTH, LPLibs.PSP_STANDARD_HEIGHT);

      addButtons(NEXT_BACK_CANCEL);
      useHeaderPanel(g_Localizer.getLocalized("HEADER"), g_Localizer.getLocalized("SUBHEADER"), 
                     "/imc/lecturnity/converter/images/flash_logo.gif");

      addButtons(BUTTON_CUSTOM, g_Localizer.getLocalized("EXTENDED"), 'e');
      setEnableButton(BUTTON_CUSTOM, true);
      
      scanInputAndSetDefaults(m_pwData);
      
      initGui();
   }
      
   public String getDescription()
   {
      return "";
   }
   
   public int displayWizard()
   {
      DocumentData dd = m_pwData.GetDocumentData();
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      boolean bScormEnabled = ps.GetBoolValue("bScormEnableds");

	  //if (bScormEnabled)
	  //{
	  //   // if SCORM package is selected, no SLIDESTAR export is possible;
	  //   m_chkSlidestar.setEnabled(false);
	  //   m_chkSlidestar.setSelected(false);
	  //}
	  //else
	  //{
	  //   m_chkSlidestar.setEnabled(true);
	  //}
      
      // argl: these is the root of all errors and unpleasentnesses:
      // _somebody_ (FlashScormTemplateWizardPanel or more precisely 
      // FlashTemplateDialog) could have changed the output video size
      // -> re-create the size text
      String txtBigness = createBignessText();
      m_lblBigness.setText(txtBigness);

      refreshDataDisplay();
      
      return super.displayWizard();
   }

   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
         
      ps.SetIntValue("iFlashVersion", (m_chkFlashVersion6.isSelected()) ? 6 : 7);
      int iTemplateType = PublisherWizardData.TEMPLATE_FLASH_FLEX ;
      if (m_chkLec3FlashPlayer.isSelected())
         iTemplateType = PublisherWizardData.TEMPLATE_FLASH_NORMAL;
	  //if (m_chkSlidestar.isSelected())
	  //   iTemplateType = PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR;
      if (m_chkPspDesign.isSelected())
         iTemplateType = PublisherWizardData.TEMPLATE_FLASH_PSP;
      ps.SetIntValue("iFlashTemplateType", iTemplateType);

      // Slidestar case: does not use a web page
      if (iTemplateType == PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR)
         ps.SetBoolValue("bFlashCreateWebsite", false);
      
      // Set a default Video size (if not defined already)
      if (m_pwData.HasNormalVideo() && !m_pwData.IsDenverDocument()
         && (ps.GetIntValue("iVideoSizeType") == StreamingConvertWizardData.VIDEOSIZE_STANDARD))
      {
         ps.SetIntValue("iVideoWidth", StreamingMediaConverter.STANDARD_VIDEO_WIDTH);
         ps.SetIntValue("iVideoHeight", StreamingMediaConverter.STANDARD_VIDEO_HEIGHT);
      }
      // "Denver" Document and PSP? --> Recalculate video size
      if(m_pwData.IsDenverDocument() && (iTemplateType == PublisherWizardData.TEMPLATE_FLASH_PSP))
      {
         Dimension dimMovie = new Dimension(ps.GetIntValue("iFlashMovieWidth"), 
                                            ps.GetIntValue("iFlashMovieHeight"));
         Dimension dimSgClip = new Dimension(-1, -1);
         dimSgClip = m_pwData.FlashCalculateVideoSizeFromMovieSize(dimMovie);
         ps.SetIntValue("iVideoWidth", dimSgClip.width);
         ps.SetIntValue("iVideoHeight", dimSgClip.height);
      }

      // Note:
      // FlashScormTemplateWizardPanel and FlashSettingsWizardPanel changed in order of appearance
      if (m_NextWizardPanel == null)
         m_NextWizardPanel = new FlashScormTemplateWizardPanel(m_pwData);

      m_pwData.nextWizardPanel = m_NextWizardPanel;

      return m_pwData;
   }

   public boolean verifyNext()
   {
      int hr = checkVideoAndPageSizes(this);
      if (hr < 0)
         return false;

      // Still image? --> Only supported by the new Flash (Flex) Player
      if (m_chkLec3FlashPlayer.isSelected())
      {
         if (m_pwData.GetDocumentData().HasStillImage())
         {
            String[] buttonStrings = {g_Localizer.getLocalized("JOP_CONTINUE"), 
                                      g_Localizer.getLocalized("JOP_CANCEL")};
            int iResult = 
               JOptionPane.showOptionDialog(this, g_Localizer.getLocalized("CONTAINS_STILL_IMAGE"), 
                                            g_Localizer.getLocalized("WARNING"), 
                                            JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, 
                                            null, buttonStrings, g_Localizer.getLocalized("JOP_CANCEL"));

            if (iResult == JOptionPane.NO_OPTION)
               return false;
         }
      }

      return true;
   }
   
   
   public void actionPerformed(ActionEvent evt)
   {
      if (evt.getSource() == m_chkLec3FlashPlayer)
      {
         if (!m_chkLec3FlashPlayer.isSelected())
         {
            m_chkFlashVersion6.setSelected(false);
         }
         setTempFlashMovieSizeType();
         switchBignessText(false);
      }
	  //else if (evt.getSource() == m_chkSlidestar)
	  //{
	  //   m_chkFlashVersion6.setEnabled(!m_chkSlidestar.isSelected());
	  //   m_chkFlashVersion6.setSelected(false);
	  //   m_chkLec3FlashPlayer.setEnabled(!m_chkSlidestar.isSelected());
	  //   if (m_chkSlidestar.isSelected())
	  //   {
	  //      m_bTempLec3FlashPlayerState = m_chkLec3FlashPlayer.isSelected();
	  //      m_chkLec3FlashPlayer.setSelected(true);
	  //   }
	  //   else
	  //   {
	  //      m_chkLec3FlashPlayer.setSelected(m_bTempLec3FlashPlayerState);
	  //   }
	  //   setTempFlashMovieSizeType();
	  //   m_chkPspDesign.setEnabled(!m_chkSlidestar.isSelected());
	  //   if (m_chkPspDesign.isSelected()) {
	  //      m_chkPspDesign.setSelected(false);
	  //      switchBignessText(true);
	  //   } else {
	  //      switchBignessText(false);
	  //   }
	  //}
      else if (evt.getSource() == m_chkPspDesign)
      {
		 //m_chkSlidestar.setEnabled(!m_chkPspDesign.isSelected());
		 //m_chkSlidestar.setSelected(false);
         m_chkLec3FlashPlayer.setEnabled(!m_chkPspDesign.isSelected());
         if (m_chkPspDesign.isSelected())
         {
            m_bTempLec3FlashPlayerState = m_chkLec3FlashPlayer.isSelected();
            m_chkLec3FlashPlayer.setSelected(true);
         }
         else
         {
            m_chkLec3FlashPlayer.setSelected(m_bTempLec3FlashPlayerState);
         }
         setTempFlashMovieSizeType();
         switchBignessText(true);
         if (m_chkLec3FlashPlayer.isSelected()) {
            switchBignessText(false);
         } else {
            switchBignessText(true);
         }
      }
      else if (evt.getSource() == m_chkFlashVersion6)
      {
         if (m_chkFlashVersion6.isSelected())
         {
            m_bTempLec3FlashPlayerState = m_chkLec3FlashPlayer.isSelected();
            m_chkLec3FlashPlayer.setSelected(true);
         }
         else
         {
            m_chkLec3FlashPlayer.setSelected(m_bTempLec3FlashPlayerState);
         }
         setTempFlashMovieSizeType();
         switchBignessText(false);
      }
   }

   public void customCommand(String command)
   {
      getWizardData();

      ExtendedSettingsDialog esd = new ExtendedSettingsDialog(getFrame(), this, m_pwData);
      esd.show();
   }

   public void refreshDataDisplay()
   {
      String txtBigness = createBignessText();
      m_lblBigness.setText(txtBigness);
      
	  //if (m_chkSlidestar.isSelected())
	  //{
	  //   m_chkFlashVersion6.setEnabled(false);
	  //   m_chkPspDesign.setEnabled(false);
	  //}
	  //else if (m_chkPspDesign.isSelected())
	  //{
	  //   m_chkSlidestar.setEnabled(false);
	  //}
	  //else if (m_chkLec3FlashPlayer.isSelected())
	  //{
	  //   // Nothing to do: all options are enabled
	  //}
      
      displayStreamInformation(m_pwData);
      
      super.refreshDataDisplay();
   }

   
   private void initGui()
   {
      Container container = getContentPanel();
      container.setLayout(null);

      
      int w = 460;
      int x = 20;
      int h = 20;
      
      int y = 20;
      int x1 = 13;
      int y1 = 20;
      
      JPanel pnlAsorted = new JPanel();
      pnlAsorted.setLayout(null);
      pnlAsorted.setSize(w+6, 103);//128);
      pnlAsorted.setLocation(x-3, y);
      pnlAsorted.setBorder(BorderFactory.createTitledBorder(g_Localizer.getLocalized("ASORTED")));
      
      m_chkLec3FlashPlayer = new JCheckBox(g_Localizer.getLocalized("LEC3_FLASH_PLAYER"));
      m_chkLec3FlashPlayer.setSize(420, h);
      m_chkLec3FlashPlayer.setLocation(x1, y1);
      m_chkLec3FlashPlayer.addActionListener(this);
      pnlAsorted.add(m_chkLec3FlashPlayer);

	  //y1 += 25;
	  //m_chkSlidestar = new JCheckBox(g_Localizer.getLocalized("SLIDESTAR"));
	  //m_chkSlidestar.setSize(205, h);
	  //m_chkSlidestar.setLocation(x1, y1);
	  //m_chkSlidestar.addActionListener(this);
	  //pnlAsorted.add(m_chkSlidestar);

      y1 += 25;
      m_chkPspDesign = new JCheckBox(g_Localizer.getLocalized("PSP_DESIGN"));
      m_chkPspDesign.setSize(400, h);
      m_chkPspDesign.setLocation(x1, y1);
      m_chkPspDesign.addActionListener(this);
      pnlAsorted.add(m_chkPspDesign);

      y1 += 25;
      m_chkFlashVersion6 = new JCheckBox(g_Localizer.getLocalized("FLASH_VERSION_6"));
      m_chkFlashVersion6.setSize(205, h);
      m_chkFlashVersion6.setLocation(x1, y1);
      m_chkFlashVersion6.addActionListener(this);
      pnlAsorted.add(m_chkFlashVersion6);

      container.add(pnlAsorted);

      y += 145 - 25;


      JPanel pnlDetailSettings = new JPanel();
      pnlDetailSettings.setLayout(null);
      pnlDetailSettings.setSize(w+6, 95);
      pnlDetailSettings.setLocation(x-3, y);
      pnlDetailSettings.setBorder(BorderFactory.createTitledBorder(g_Localizer.getLocalized("DETAIL_SETTINGS")));

      JLabel resolutionLabel = new JLabel(g_Localizer.getLocalized("RESOLUTION") + ":");
      resolutionLabel.setSize(100, 22);
      resolutionLabel.setLocation(15, 19);
      pnlDetailSettings.add(resolutionLabel);

      m_lblBigness = new JLabel("");
      m_lblBigness.setSize(340, 22);
      m_lblBigness.setLocation(90, 19);
      pnlDetailSettings.add(m_lblBigness);
      switchBignessText(false);

      JLabel audioLabel = new JLabel(g_Localizer.getLocalized("DETAIL_AUDIO"));
      audioLabel.setSize(100, 22);
      audioLabel.setLocation(15, 44);
      pnlDetailSettings.add(audioLabel);

      m_audioDetailLabel = new JLabel(g_Localizer.getLocalized("NOT_PRESENT"));
      m_audioDetailLabel.setSize(240, 22);
      m_audioDetailLabel.setLocation(90, 44);
      pnlDetailSettings.add(m_audioDetailLabel);

      JLabel videoLabel = new JLabel(g_Localizer.getLocalized("DETAIL_VIDEO"));
      videoLabel.setSize(150, 22);
      videoLabel.setLocation(220, 44);
      pnlDetailSettings.add(videoLabel);

      m_videoDetailLabel = new JLabel(g_Localizer.getLocalized("NOT_PRESENT"));
      m_videoDetailLabel.setSize(220, 22);
      m_videoDetailLabel.setLocation(310, 44);
      pnlDetailSettings.add(m_videoDetailLabel);

      JLabel clipsLabel = new JLabel(g_Localizer.getLocalized("DETAIL_CLIPS"));
      clipsLabel.setSize(150, 22);
      clipsLabel.setLocation(220, 64);
      pnlDetailSettings.add(clipsLabel);

      m_clipsDetailLabel = new JLabel(g_Localizer.getLocalized("NOT_PRESENT"));
      m_clipsDetailLabel.setSize(220, 22);
      m_clipsDetailLabel.setLocation(310, 64);
      pnlDetailSettings.add(m_clipsDetailLabel);

      container.add(pnlDetailSettings);

      // Bugfix 5123: Preselect template type (if any)
      ProfiledSettings  ps = m_pwData.GetProfiledSettings();
      int nFlashTemplateType = ps.GetIntValue("iFlashTemplateType");
      switch (nFlashTemplateType) {
         case PublisherWizardData.TEMPLATE_FLASH_NORMAL:
            m_chkLec3FlashPlayer.doClick();
            break;
		 //case PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR:
		 //   m_chkSlidestar.doClick();
		 //   break;
         case PublisherWizardData.TEMPLATE_FLASH_PSP:
            m_chkPspDesign.doClick();
            break;
         case PublisherWizardData.TEMPLATE_FLASH_FLEX:
            // select nothing
            break;
      }
      if (ps.GetIntValue("iFlashVersion") == 6)
         m_chkFlashVersion6.doClick();

      refreshDataDisplay();
   }

   private void scanInputAndSetDefaults(PublisherWizardData pwData)
   {
      ProfiledSettings  ps = pwData.GetProfiledSettings();
      DocumentData      dd = pwData.GetDocumentData();
      
      /* // done by DocumentData.ReadDataFromPresentationFile()
      
      ProfiledSettings  ps = pwData.GetProfiledSettings();
      DocumentData      dd = pwData.GetDocumentData();
      
      // AUDIO
      StreamInformation siAudio = new StreamInformation(StreamInformation.AUDIO_STREAM);
      // Set a default bitrate
      siAudio.m_nBitrate = 32000;
      dd.SetAudioStreamInfo(siAudio);

      if (pwData.HasNormalVideo())
      {
         if (dd.GetVideoInfo() != null)
         {
            // Create a video stream information object
            StreamInformation siVideo = new StreamInformation(
               -1, -1, dd.GetVideoInfo().videoFramerateTenths, dd.GetVideoDimension().width, dd.GetVideoDimension().height);
            // Set a default bitrate
            siVideo.m_nBitrate = 800 * 1000; // default
            dd.SetVideoStreamInfo(siVideo);
         }
         else
         {
            // Known Bug: dd.GetVideoInfo() is created/copied in verifyNext()
            // of SelectPresentationWizardPanel.java, 
            // adding a video with SelectActionWizardPanel.java doesn't create it
            
            JOptionPane.showMessageDialog(this, g_Localizer.getLocalized("RELOAD_CONFIG_CHANGED"), 
                                          g_Localizer.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
            System.exit(1);
         }
      }

      // CLIPS
      if (pwData.HasClips())
      {
         int iFramerateTenths = 0;
         int iMaxClipWidth = 0, iMaxClipHeight = 0;
         boolean bClipsAreScreengrabbing = true;

         for (int i = 0; i < dd.GetVideoClipInfos().length; ++i)
         {
            if (dd.GetVideoClipInfos()[i].videoWidth > iMaxClipWidth)
               iMaxClipWidth = dd.GetVideoClipInfos()[i].videoWidth;
            if (dd.GetVideoClipInfos()[i].videoHeight > iMaxClipHeight)
               iMaxClipHeight = dd.GetVideoClipInfos()[i].videoHeight;
            if (dd.GetVideoClipInfos()[i].videoFramerateTenths > iFramerateTenths)
               iFramerateTenths = dd.GetVideoClipInfos()[i].videoFramerateTenths;
         }

         // Create a video stream information object
         StreamInformation siClips = new StreamInformation(
            -1, -1, iFramerateTenths, iMaxClipWidth, iMaxClipHeight);
         // Set a default bitrate
         siClips.m_nBitrate = 1000 * 1000; // default
         dd.SetClipsStreamInfo(siClips);
      }
      */
      
      
      // Transfer StreamInfo changes to ProfileSettings
      
      // ??? Why only audio bitrate?
      // ??? Is this necessary/useful?
      
      if (!pwData.m_bIsProfileMode && dd.GetAudioStreamInfo() != null)
         ps.SetIntValue("iFlashAudioBitrate", dd.GetAudioStreamInfo().m_nBitrate);
   }

/*
   private String createTargetFileName()
   {
      // it is assumed that both fields (m_fldTargetName, m_fldTargetPath)
      // contain valid texts (paths); can (and will) be checked with verifyNext()
      
      String sTarget = m_fldTargetName.getText();
      if (sTarget.toLowerCase().endsWith(".swf"))
         sTarget = sTarget.substring(0, sTarget.length()-4);
      
      return new File(m_fldTargetPath.getText(), sTarget+".swf").getPath();

   }
*/
   
   private void setTempFlashMovieSizeType() {
      // Bugfix 5247:
      // Remember current LEC 3 layout (only if "screen" or "custom" - "flexible" is 'reserved' for LEC 4)
      ProfiledSettings  ps = m_pwData.GetProfiledSettings();
      int iFlashMovieSizeType = ps.GetIntValue("iFlashMovieSizeType");
      if (iFlashMovieSizeType != PublisherWizardData.MOVIESIZE_FLEXIBLE) {
         m_iTempFlashMovieSizeType = iFlashMovieSizeType;
      } else  {
         // Special case: "flexible" layout for LEC 3 is selected from user --> remember it
         if (!m_chkLec3FlashPlayer.isSelected())
            m_iTempFlashMovieSizeType = iFlashMovieSizeType;
      }
   }

   private void switchBignessText(boolean bSwapSizes)
   {
      ProfiledSettings  ps = m_pwData.GetProfiledSettings();

      // Bugfix 5247:
      // (Re-)Store layout and movie size for LEC 3 and LEC 4 Flash Player 
      int iFlashMovieSizeType = ps.GetIntValue("iFlashMovieSizeType");
      if (m_chkLec3FlashPlayer.isSelected()) {
         // "Flexible" layout selected? --> Change to "screen"/"custom"(/"flexible") layout
         // (which was stored in "actionPerformed()")
         if (iFlashMovieSizeType == PublisherWizardData.MOVIESIZE_FLEXIBLE) {
            ps.SetIntValue("iFlashMovieSizeType", m_iTempFlashMovieSizeType);
            // "Restore" (LEC 3) movie size
            m_sizeMovieNormal = m_sizeMovieTemp;
         }
      } else {
         // "Flexible" layout must be selected
         ps.SetIntValue("iFlashMovieSizeType", PublisherWizardData.MOVIESIZE_FLEXIBLE);
         // "Store" (LEC 3) movie size
         m_sizeMovieTemp = new Dimension(ps.GetIntValue("iFlashMovieWidth"), 
                                         ps.GetIntValue("iFlashMovieHeight"));
      }

      if (m_chkPspDesign.isSelected())
      {
         if (bSwapSizes && m_chkLec3FlashPlayer.isSelected())
            m_sizeMovieNormal = new Dimension(ps.GetIntValue("iFlashMovieWidth"), 
                                              ps.GetIntValue("iFlashMovieHeight"));
         ps.SetIntValue("iFlashMovieWidth", m_sizeMoviePsp.width);
         ps.SetIntValue("iFlashMovieHeight", m_sizeMoviePsp.height);
         m_pwData.m_PreferredMovieDimension = new Dimension(ps.GetIntValue("iFlashMovieWidth"), 
                                                            ps.GetIntValue("iFlashMovieHeight"));
         if (m_pwData.IsDenverDocument())
            m_pwData.m_iVideoExtendedType = m_pwData.PANEL_EXTD_EXTENT;
      }
      else
      {
         if (bSwapSizes && m_chkLec3FlashPlayer.isSelected())
            m_sizeMoviePsp = new Dimension(ps.GetIntValue("iFlashMovieWidth"), 
                                           ps.GetIntValue("iFlashMovieHeight"));
         ps.SetIntValue("iFlashMovieWidth", m_sizeMovieNormal.width);
         ps.SetIntValue("iFlashMovieHeight", m_sizeMovieNormal.height);
         m_pwData.m_PreferredMovieDimension = new Dimension(ps.GetIntValue("iFlashMovieWidth"), 
                                                            ps.GetIntValue("iFlashMovieHeight"));
         if (m_pwData.IsDenverDocument())
            m_pwData.m_iVideoExtendedType = m_pwData.PANEL_EXTD_SLIDES;
      }

      // Remember current Template type
      int iTemplateTypeTemp = ps.GetIntValue("iFlashTemplateType");
      if (m_chkPspDesign.isSelected())
         ps.SetIntValue("iFlashTemplateType", PublisherWizardData.TEMPLATE_FLASH_PSP);
	  //else if (m_chkSlidestar.isSelected())
	  //   ps.SetIntValue("iFlashTemplateType", PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR);
      else if (m_chkLec3FlashPlayer.isSelected())
         ps.SetIntValue("iFlashTemplateType", PublisherWizardData.TEMPLATE_FLASH_NORMAL);
      else
         ps.SetIntValue("iFlashTemplateType", PublisherWizardData.TEMPLATE_FLASH_FLEX);

      // Calculate whiteboard size from movie size
      Dimension dimMovie = new Dimension(ps.GetIntValue("iFlashMovieWidth"), 
                                         ps.GetIntValue("iFlashMovieHeight"));
      Dimension dimWhiteboard = new Dimension(-1, -1);
      dimWhiteboard = m_pwData.FlashCalculatePageSizeFromMovieSize(dimMovie);
      ps.SetIntValue("iSlideWidth", dimWhiteboard.width);
      ps.SetIntValue("iSlideHeight", dimWhiteboard.height);
      System.out.println("- dimWB: " + dimWhiteboard.width + "x" + dimWhiteboard.height + " (Movie: " + dimMovie.width + "x" + dimMovie.height + ")");

      String txtBigness = createBignessText();
      m_lblBigness.setText(txtBigness);

      // (Re-)Set remembered Template type
      ps.SetIntValue("iFlashTemplateType", iTemplateTypeTemp);
   }

   private String createBignessText()
   {
      String txtBigness = "";

      ProfiledSettings  ps = m_pwData.GetProfiledSettings();

      Dimension dimMovie = new Dimension(ps.GetIntValue("iFlashMovieWidth"), 
                                         ps.GetIntValue("iFlashMovieHeight"));

      int iFlashMovieSizeType = ps.GetIntValue("iFlashMovieSizeType");

      if (m_pwData.IsDenverDocument())
      {
         if (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_PSP)
         {
            // Special case: Calculate SG clip size as a "page" size
            Dimension dimSgClip = new Dimension(-1, -1);
            dimSgClip = m_pwData.FlashCalculateVideoSizeFromMovieSize(dimMovie);
            txtBigness += dimMovie.width+"x"+dimMovie.height
                        +" ("+g_Localizer.getLocalized("VIDEO_SIZE")+": "+dimSgClip.width+"x"+dimSgClip.height+")";
         }
         else
         {
            txtBigness += dimMovie.width+"x"+dimMovie.height
                          +" ("+g_Localizer.getLocalized("VIDEO_SIZE")+": "
                          +ps.GetIntValue("iVideoWidth")+"x"+ps.GetIntValue("iVideoHeight")+")";
         }
      }
      else if (iFlashMovieSizeType != PublisherWizardData.MOVIESIZE_FLEXIBLE)
      {
         txtBigness += g_Localizer.getLocalized("VIDEO_OPTIMIZE")+": ";
         Dimension dimWhiteboard = new Dimension(-1, -1);
         dimWhiteboard = m_pwData.FlashCalculatePageSizeFromMovieSize(dimMovie);
         txtBigness += dimMovie.width+"x"+dimMovie.height
                     +" ("+g_Localizer.getLocalized("PAGE_SIZE")+": "
                     +dimWhiteboard.width+"x"+dimWhiteboard.height+")";
      }
      else
         txtBigness += g_Localizer.getLocalized("VIDEO_FLEXIBLE");

      return txtBigness;
   }

   private void displayStreamInformation(PublisherWizardData pwData)
   {
      ProfiledSettings  ps = pwData.GetProfiledSettings();
      DocumentData      dd = pwData.GetDocumentData();
      
//      StreamInformation siAudio = dd.GetAudioStreamInfo();
      StreamInformation siVideo = dd.GetVideoStreamInfo();
      StreamInformation siClips = dd.GetClipsStreamInfo();
      boolean bClipsAndVideoCombined = pwData.HasClips() && !ps.GetBoolValue("bShowClipsOnSlides");
      boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && ps.GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;

      m_audioDetailLabel.setText((ps.GetIntValue("iFlashAudioBitrate")/1000)+" "+g_Localizer.getLocalized("KBPS"));

      // get video bitrate
      int nVideoBitrate = ps.GetIntValue("iFlashVideoBitrate");
      if ( !bIsGenericProfile && siVideo != null && nVideoBitrate < 0)
          nVideoBitrate = GetFlashOptimizedVideoBitrate(siVideo.m_nWidth, siVideo.m_nHeight, siVideo.m_nFrequency/10);
      
      // set video label
      if ( !bIsGenericProfile && siVideo == null ) 
          m_videoDetailLabel.setText(g_Localizer.getLocalized("NOT_PRESENT"));
      else if ( nVideoBitrate < 0 )
          m_videoDetailLabel.setText(g_Localizer.getLocalized("AUTOMATIC"));
      else 
          m_videoDetailLabel.setText((nVideoBitrate/1000)+" "+g_Localizer.getLocalized("KBPS"));
          

      // get clip bitrate
      int nClipBitrate = ps.GetIntValue("iFlashClipBitrate");
      if ( !bIsGenericProfile && siClips != null && nClipBitrate < 0)
         nClipBitrate = GetFlashOptimizedVideoBitrate(siClips.m_nWidth, siClips.m_nHeight, siClips.m_nFrequency/10);

      if ( !bIsGenericProfile && siClips == null )
          m_clipsDetailLabel.setText(g_Localizer.getLocalized("NOT_PRESENT"));
      else if ( !bIsGenericProfile  && (siVideo != null) && bClipsAndVideoCombined)
          m_clipsDetailLabel.setText(g_Localizer.getLocalized("VIDEO_CLIPS_COMBINED"));
      else if ( nClipBitrate < 0 )
          m_clipsDetailLabel.setText(g_Localizer.getLocalized("AUTOMATIC"));
      else
         m_clipsDetailLabel.setText((nClipBitrate/1000)+" "+g_Localizer.getLocalized("KBPS"));
   }

   public int checkVideoAndPageSizes(Component myPanel) {
       return checkVideoAndPageSizes(myPanel, false);
   }

   public int checkVideoAndPageSizes(Component myPanel, boolean bIsInternal)
   {
      int hr = 0; // SUCCESS
      
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      if (!m_pwData.IsDenverDocument())
      {
         Dimension dimMovieSize = new Dimension(ps.GetIntValue("iFlashMovieWidth"), 
                                                ps.GetIntValue("iFlashMovieHeight"));
         int maxVideoWidth = ps.GetIntValue("iSlideWidth") + 3 + ps.GetIntValue("iVideoWidth");
         int maxVideoHeight = (ps.GetIntValue("iSlideHeight") > ps.GetIntValue("iVideoHeight")) ? 
            ps.GetIntValue("iSlideHeight") : ps.GetIntValue("iVideoHeight");
         Dimension maxVideoSize = new Dimension(maxVideoWidth, maxVideoHeight);
         if (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_PSP)
         {
            maxVideoSize.width = Math.round(LPLibs.PSP_CONTENT_WIDTH * (dimMovieSize.width / (float)LPLibs.PSP_STANDARD_WIDTH));
            maxVideoSize.height = Math.round(LPLibs.PSP_CONTENT_HEIGHT * (dimMovieSize.height / (float)LPLibs.PSP_STANDARD_HEIGHT));
         }  

         // Check Page size
         if (ps.GetIntValue("iSlideWidth") <= 0 || ps.GetIntValue("iSlideHeight") <= 0)
         {
            JOptionPane.showMessageDialog(myPanel, g_Localizer.getLocalized("UNDEFINED_PAGE_SIZE"),
                                          g_Localizer.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
            hr = -1;
         }
         // Check Video size
         else if ( (ps.GetIntValue("iVideoWidth") > maxVideoSize.width) 
                   || (ps.GetIntValue("iVideoHeight") > maxVideoSize.height) )
         {
            // ??? UNDEFINED_PAGE_SIZE ??
            JOptionPane.showMessageDialog(myPanel, g_Localizer.getLocalized("UNDEFINED_PAGE_SIZE"),
                                          g_Localizer.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
            hr = -1;
         }

      }

      // Bugfix 3863: Special check for video and clips size which should not exceed 3 MegaPixel
      hr = ExtendedSettingsDialog.checkVideoSizes(myPanel, m_pwData, false, bIsInternal);

      return hr;
   }

   public static int GetFlashOptimizedVideoBitrate(int nWidth, int nHeight, int nFramerate) {
      int nNewBitrate = -1;
      int thousand = 1000;
      int million = 1000000;
      // Multiply width with height with framerate to get a quality factor
      int nQualityFactor = nWidth * nHeight * nFramerate;
      
      if (nQualityFactor <= (1*million))
         nNewBitrate = 800 * thousand;
      if ((nQualityFactor > (1*million)) && (nQualityFactor <= (2*million)))
         nNewBitrate = 1000 * thousand;
      if ((nQualityFactor > (2*million)) && (nQualityFactor <= (5*million)))
         nNewBitrate = 2000 * thousand;
      if ((nQualityFactor > (5*million)) && (nQualityFactor <= (10*million)))
         nNewBitrate = 3500 * thousand;
      if ((nQualityFactor > (10*million)) && (nQualityFactor <= (25*million)))
         nNewBitrate = 5000 * thousand;
      if ((nQualityFactor > (25*million)) && (nQualityFactor <= (50*million)))
         nNewBitrate = 8000 * thousand;
      if (nQualityFactor > (50*million))
         nNewBitrate = 10000 * thousand;
      
      //System.out.println("GetFlashOptimizedVideoBitrate(" + nWidth + ", " + nHeight + ", " + nFramerate + ") --> " + nNewBitrate);
      return nNewBitrate;
   }
}

