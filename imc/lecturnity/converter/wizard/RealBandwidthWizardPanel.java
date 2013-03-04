package imc.lecturnity.converter.wizard;

import java.awt.Dimension;
import java.awt.Container;
import java.awt.Point;
import java.awt.Font;

import javax.swing.*;
import javax.swing.border.TitledBorder;
import javax.swing.border.BevelBorder;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

import java.io.IOException;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.StreamingMediaConverter;
import imc.lecturnity.converter.ProfiledSettings;
import imc.epresenter.filesdk.util.Localizer;

public class RealBandwidthWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String BANDWIDTH = "[!]";
   private static String AUDIO = "[!]";
   private static String VIDEO = "[!]";

   private static String EXTENDED = "[!]";

   private static String VIDEO_QUALITY = "[!]";
   private static String AUTOMATIC = "[!]";
   private static String USER_DEFINED = "[!]";
   private static String NORMAL = "[!]";
   private static String SMOOTHEST_MOTION = "[!]";
   private static String SHARPEST_IMAGE = "[!]";
   private static String SLIDESHOW = "[!]";

   private static String[] BITRATE = {"[!]", "[!]", "[!]", "[!]", "[!]", "[!]", "[!]", "[!]", "[!]"};
   private static char[]   MNEM_BITRATE = {'?', '?', '?', '?', '?', '?', '?', '?', '?'};
   private static char     MNEM_AUTO = '?';
   private static char     MNEM_USER = '?';
//    private static String[] VIDEO_BITRATE = {"[!]", "[!]", "[!]", "[!]", "[!]", "[!]"};

   static
   {
      try
      {
         Localizer l = new Localizer("/imc/lecturnity/converter/wizard" +
                           "/RealBandwidthWizardPanel_", "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         BANDWIDTH = l.getLocalized("BANDWIDTH");
         AUDIO = l.getLocalized("AUDIO");
         VIDEO = l.getLocalized("VIDEO");

         EXTENDED = l.getLocalized("EXTENDED");

         VIDEO_QUALITY = l.getLocalized("VIDEO_QUALITY");
         AUTOMATIC = l.getLocalized("AUTOMATIC");
         USER_DEFINED = l.getLocalized("USER_DEFINED");
         NORMAL = l.getLocalized("NORMAL");
         SMOOTHEST_MOTION  = l.getLocalized("SMOOTHEST_MOTION");
         SHARPEST_IMAGE = l.getLocalized("SHARPEST_IMAGE");
         SLIDESHOW = l.getLocalized("SLIDESHOW");

         for (int i=0; i<BITRATE.length; i++)
         {
            BITRATE[i] = l.getLocalized("BITRATE" + i);
            MNEM_BITRATE[i] = l.getLocalized("MNEM_BITRATE" + i).charAt(0);
         }
         MNEM_AUTO = l.getLocalized("MNEM_AUTO").charAt(0);
         MNEM_USER = l.getLocalized("MNEM_USER").charAt(0);
//          for (int i=0; i<6; i++)
//             VIDEO_BITRATE[i] = l.getLocalized("VIDEO_BITRATE" + i);
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Access to Locale database failed!", "Error", 
                                       JOptionPane.ERROR_MESSAGE);
      }
   }

   private PublisherWizardData pwData_;

   private WizardPanel nextWizardPanel_ = null;
   private WizardPanel nextPixWizard_   = null;
   
   private boolean[] abRealBitrates_ = null;
   private boolean isNextEnabled_ = true;
   private boolean isAutoVideoQuality_ = true;

   private JPanel audioPanel_;
   private JCheckBox[] audioBitrateBoxes_ = new JCheckBox[BITRATE.length];
   private JPanel videoPanel_;
   private JCheckBox[] videoBitrateBoxes_ = new JCheckBox[BITRATE.length];

   private JRadioButton automaticRadio_;
   private JRadioButton userDefinedRadio_;
   private JComboBox videoQualityBox_;

   public RealBandwidthWizardPanel(PublisherWizardData pwData)
   {
      super();
      
      pwData_ = pwData;
      
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      abRealBitrates_ = new boolean[] { ps.GetBoolValue("bRealBitrate0"),
         ps.GetBoolValue("bRealBitrate1"), ps.GetBoolValue("bRealBitrate2"), 
         ps.GetBoolValue("bRealBitrate3"), ps.GetBoolValue("bRealBitrate4"), 
         ps.GetBoolValue("bRealBitrate5"), ps.GetBoolValue("bRealBitrate6"), 
         ps.GetBoolValue("bRealBitrate7"), ps.GetBoolValue("bRealBitrate8") };

      setPreferredSize(new Dimension(500, 400));

      addButtons(NEXT_BACK_CANCEL);
      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/real_networks_logo.gif");
      addButtons(BUTTON_CUSTOM, EXTENDED, 'e');

      Container p = getContentPanel();
      p.setLayout(null);

      WizardTextArea wta = new WizardTextArea(BANDWIDTH,
                                              new Point(20, 16),
                                              new Dimension(180, 80),
                                              imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      p.add(wta);

//       BevelBorder border = new BevelBorder(BevelBorder.LOWERED);

      BandwidthBoxesListener bbl = new BandwidthBoxesListener();

      audioPanel_ = new JPanel();
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         audioPanel_.setBackground(Wizard.BG_COLOR);
      audioPanel_.setLayout(null);
      audioPanel_.setSize(240, 250);
      audioPanel_.setLocation(240, 16);
      audioPanel_.setBorder(new TitledBorder(" " + AUDIO + " "));
      p.add(audioPanel_);

      for (int i=0; i<BITRATE.length; i++)
      {
         audioBitrateBoxes_[i] = new JCheckBox(BITRATE[i], 
                                               abRealBitrates_[i]);
         if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
            audioBitrateBoxes_[i].setBackground(Wizard.BG_COLOR);
         audioBitrateBoxes_[i].setSize(200, 18);
         audioBitrateBoxes_[i].setLocation(20, 23 + i*24);
         audioBitrateBoxes_[i].setMnemonic(MNEM_BITRATE[i]);
         audioBitrateBoxes_[i].addChangeListener(bbl);
         audioPanel_.add(audioBitrateBoxes_[i]);
      }

      videoPanel_ = new JPanel();
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         videoPanel_.setBackground(Wizard.BG_COLOR);
      videoPanel_.setLayout(null);
      videoPanel_.setSize(200, 150);
      videoPanel_.setLocation(20, 116);
      videoPanel_.setBorder(new TitledBorder(" " + VIDEO_QUALITY + " "));
      p.add(videoPanel_);
      
      ButtonGroup bg = new ButtonGroup();
      RadioListener rl = new RadioListener();

      isAutoVideoQuality_ = ps.GetBoolValue("bRealAutoVideoQuality");
      automaticRadio_ = new JRadioButton(AUTOMATIC, isAutoVideoQuality_);
      automaticRadio_.setSize(120, 20);
      automaticRadio_.setLocation(20, 30);
      automaticRadio_.setMnemonic(MNEM_AUTO);
      automaticRadio_.addActionListener(rl);
      videoPanel_.add(automaticRadio_);
      bg.add(automaticRadio_);

      userDefinedRadio_ = new JRadioButton(USER_DEFINED, !isAutoVideoQuality_);
      userDefinedRadio_.setSize(160, 20);
      userDefinedRadio_.setLocation(20, 70);
      userDefinedRadio_.setMnemonic(MNEM_USER);
      userDefinedRadio_.addActionListener(rl);
      videoPanel_.add(userDefinedRadio_);
      bg.add(userDefinedRadio_);

      
      videoQualityBox_ = new JComboBox();
      videoQualityBox_.addItem(NORMAL);
      videoQualityBox_.addItem(SMOOTHEST_MOTION);
      videoQualityBox_.addItem(SHARPEST_IMAGE);
      videoQualityBox_.addItem(SLIDESHOW);
      videoQualityBox_.setSize(142, 20);
      videoQualityBox_.setLocation(38, 100);
      videoQualityBox_.addActionListener(rl);
      videoPanel_.add(videoQualityBox_);
      videoQualityBox_.setEnabled(!isAutoVideoQuality_);
      int iQualityIndex = ps.GetIntValue("iRealVideoQuality");
      videoQualityBox_.setSelectedIndex(iQualityIndex);


      isNextEnabled_ = true;
   }

   public String getDescription()
   {
      return "";
   }

   public void refreshDataDisplay()
   {
      super.refreshDataDisplay();

      boolean bHasAnyVideo = (pwData_.HasNormalVideo() || pwData_.HasClips());
      boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && pwData_.GetProfiledSettings().GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
      
      for (int i=0; i<BITRATE.length; i++)
         audioBitrateBoxes_[i].setSelected(abRealBitrates_[i]);

      if (bHasAnyVideo || bIsGenericProfile)
      {
         setEnableButton(BUTTON_CUSTOM, true);
         videoPanel_.setEnabled(true);
         automaticRadio_.setEnabled(true);
         userDefinedRadio_.setEnabled(true);
         videoQualityBox_.setEnabled(true);
      }
      else
      {
///         setEnableButton(BUTTON_CUSTOM, false);
         setEnableButton(BUTTON_CUSTOM, true);
         videoPanel_.setEnabled(false);
         automaticRadio_.setEnabled(false);
         userDefinedRadio_.setEnabled(false);
         videoQualityBox_.setEnabled(false);
      }

      updateVideoQualityControls();
   }
   
   private void updateVideoQualityControls()
   {
      isAutoVideoQuality_ = automaticRadio_.isSelected();
      videoQualityBox_.setEnabled(!isAutoVideoQuality_);
   }

   public void customCommand(String command)
   {
      getWizardData();

      ExtendedSettingsDialog esd = new ExtendedSettingsDialog(getFrame(), this, pwData_);
      esd.show();
   }

   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      for (int i=0; i<BITRATE.length; i++)
      {
         abRealBitrates_[i] = audioBitrateBoxes_[i].isSelected();
         ps.SetBoolValue("bRealBitrate"+i, abRealBitrates_[i]);
      }

      if (ps.GetIntValue("iVideoSizeType") == PublisherWizardData.VIDEOSIZE_STANDARD)
      {
         ps.SetIntValue("iVideoWidth", StreamingMediaConverter.STANDARD_VIDEO_WIDTH);
         ps.SetIntValue("iVideoHeight", StreamingMediaConverter.STANDARD_VIDEO_HEIGHT);
      }

      ps.SetBoolValue("bRealAutoVideoQuality", isAutoVideoQuality_);
      if (isAutoVideoQuality_)
      {
         if (pwData_.HasNormalVideo() || pwData_.HasClips())
         {
            if (pwData_.HasNormalVideo())
               ps.SetIntValue("iRealVideoQuality", 1); // smoothest motion
            else
               ps.SetIntValue("iRealVideoQuality", 2); // sharpest image
         }
         else
            ps.SetIntValue("iRealVideoQuality", 0); // normal
      }
      else
      {
         ps.SetIntValue("iRealVideoQuality", videoQualityBox_.getSelectedIndex());
      }

      if (nextWizardPanel_ == null)
         nextWizardPanel_ = new RealVersionWizardPanel(pwData_);

      pwData_.nextWizardPanel = nextWizardPanel_;

      return pwData_;
   }

   public boolean verifyNext() {
       // Bugfix 3863/4507: Special check for video and clips size which should not exceed 3 MegaPixel
       // Bugfix 5731: video and clips size should not exceed 1280*1024.
       int hr = ExtendedSettingsDialog.checkVideoSizes(this, pwData_, true);
       
       if (hr < 0)
           return false;

       return true;
   }

   private class BandwidthBoxesListener implements ChangeListener 
   {
      public void stateChanged(ChangeEvent e)
      {
         boolean oneIsOn = false;
         for (int i=0; i<BITRATE.length; i++)
            oneIsOn = oneIsOn || audioBitrateBoxes_[i].isSelected();
         
         if (oneIsOn != isNextEnabled_)
         {
            isNextEnabled_ = oneIsOn;
            setEnableButton(BUTTON_NEXT, isNextEnabled_);
         }
      }
   }

   private class RadioListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         updateVideoQualityControls();
      }
   }
}
