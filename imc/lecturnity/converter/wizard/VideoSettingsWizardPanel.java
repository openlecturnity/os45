package imc.lecturnity.converter.wizard;

import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.swing.*;
import javax.swing.border.Border;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.ui.LayoutUtils;
import imc.lecturnity.util.wizards.WizardPanel;

public class VideoSettingsWizardPanel extends WizardPanel implements ActionListener
{

   private static Localizer g_Localizer = null;
   static 
   {
      try
      {
         g_Localizer = new Localizer(
            "/imc/lecturnity/converter/wizard/VideoSettingsWizardPanel_", "en");
      }
      catch (IOException exc)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }
   
///   private VideoWriterWizardPanel m_NextWizardPanel;
   private WizardPanel m_NextWizardPanel;
   private PublisherWizardData m_pwData;
   
   private JPanel m_pnlContent;
   private JPanel m_pnlMedia;
   private JCheckBox m_chkSingle;
   private JCheckBox m_chkYouTubeOptimzed;
///   private JTextField m_txtName;
///   private JButton m_btnSearch;
   private JCheckBox m_chkClips;
   private JRadioButton m_rbtPages;
   private JRadioButton m_rbtVideo;
   private JPanel m_pnlParameters;
   private JPanel  m_pnlQuality; 
   private JComboBox m_boxQuality;
   private JComboBox m_boxFps;
   private JComboBox m_boxVideoQFactor;
   private JComboBox m_boxAudioRate;
   private JTextField m_txtVideoWidth;
   private JTextField m_txtVideoHeight;
   
   private boolean m_bFillActive = false;
   
   // fps, video, audio
   private String[][] m_aaDefaults = new String[][] {
      { g_Localizer.getLocalized("LOW"), "2", "60%", "32" },
      { g_Localizer.getLocalized("MEDIUM"), "5", "80%", "32" },
      { g_Localizer.getLocalized("HIGH"), "10", "95%", "48" }
   };
   // Note: for the parsing and translation of the video quality below:
   // percent value must be two digits (no 1-9 and no 100%).
         
   
 
   public VideoSettingsWizardPanel(PublisherWizardData pwData)
   {
      super();

      setPreferredSize(new Dimension(500, 400));

      m_pwData = pwData;

///      addButtons(BUTTON_BACK | BUTTON_FINISH | BUTTON_CANCEL);
      addButtons(NEXT_BACK_CANCEL);
      useHeaderPanel(g_Localizer.getLocalized("HEADER"), g_Localizer.getLocalized("SUBHEADER"), 
                     "/imc/lecturnity/converter/images/videologo.jpg");

      //addButtons(BUTTON_CUSTOM, g_Localizer.getLocalized("TEMPLATE"), 'v');
      //setEnableButton(BUTTON_CUSTOM, true);

      int h = 19;
      
      
      Border borderEmpty5 = BorderFactory.createEmptyBorder(0, 5, 4, 5);
      Border borderEmpty10 = BorderFactory.createEmptyBorder(0, 10, 0, 10);
      
      JPanel pnlContainer = getContentPanel();
      pnlContainer.setLayout(new BorderLayout(0, 5));
      pnlContainer.setBorder(borderEmpty10);
     
      
///      JPanel pnlFind = new JPanel(new BorderLayout(5, 0));
///      m_txtName = new JTextField();
///      LimitHeight(m_txtName, h);
///      m_btnSearch = new JButton(g_Localizer.getLocalized("SEARCH"));
///      m_btnSearch.addActionListener(this);
///      LimitHeight(m_btnSearch, h);
///      pnlFind.add("Center", m_txtName);
///      pnlFind.add("East", m_btnSearch);
///      Border borderTitle1 = BorderFactory.createTitledBorder(g_Localizer.getLocalized("OUTPUT_FILE"));
///      pnlFind.setBorder(BorderFactory.createCompoundBorder(borderTitle1, borderEmpty5));
      
     
      m_pnlMedia = new JPanel(new GridLayout(2, 2));
     
      m_chkClips = new JCheckBox(g_Localizer.getLocalized("EMBED_CLIPS"));
      LimitHeight(m_chkClips, h);
      m_rbtPages = new JRadioButton(g_Localizer.getLocalized("PAGES"));
      LimitHeight(m_rbtPages, h);
      m_rbtVideo = new JRadioButton(g_Localizer.getLocalized("ACCOMPANYING_VIDEO"));
      LimitHeight(m_rbtVideo, h);
      
      m_pnlMedia.add(m_rbtPages);
      m_pnlMedia.add(m_chkClips);
      m_pnlMedia.add(m_rbtVideo);
      
      Border borderTitle7 = BorderFactory.createTitledBorder(g_Localizer.getLocalized("MEDIA"));
      m_pnlMedia.setBorder(BorderFactory.createCompoundBorder(borderTitle7, borderEmpty5));
      
      ButtonGroup group = new ButtonGroup();
      group.add(m_rbtPages);
      group.add(m_rbtVideo);
      
      
      String[] aQualityTexts = new String[m_aaDefaults.length + 1];
      aQualityTexts[aQualityTexts.length - 1] = g_Localizer.getLocalized("CUSTOM");
      for (int i=0; i<m_aaDefaults.length; ++i)
      {
         aQualityTexts[i] = m_aaDefaults[i][0];
      }
      String[] aVideoTexts = new String[] { "30%", "60%",  "80%", "90%", "95%" };
      String[] aAudioTexts = new String[] { "32", "48", "64", "96" };
      
      
      m_pnlQuality = new JPanel(new FlowLayout(FlowLayout.LEFT, 0, 0));
      JLabel lblQuality = new JLabel(g_Localizer.getLocalized("QUALITY"));
      LimitHeight(lblQuality, h);
      m_pnlQuality.add(lblQuality);
      m_pnlQuality.add(Box.createHorizontalStrut(10));
      m_boxQuality = new JComboBox(aQualityTexts);
      m_boxQuality.setSelectedItem(g_Localizer.getLocalized("MEDIUM"));
      m_boxQuality.addActionListener(this);
      LimitHeight(m_boxQuality, h);
      m_pnlQuality.add(m_boxQuality);
      
      m_chkSingle = new JCheckBox(g_Localizer.getLocalized("CHANGE_DETAILS"));
      m_chkSingle.addActionListener(this);
      LimitHeight(m_chkSingle, h);

      m_chkYouTubeOptimzed = new JCheckBox(g_Localizer.getLocalized("YOUTUBE_OPTIMIZED"));
      m_chkYouTubeOptimzed.addActionListener(this);
      LimitHeight(m_chkYouTubeOptimzed, h);
      
      ImageIcon youTubeIcon = new ImageIcon(getClass().getResource("/imc/lecturnity/converter/images/pngYouTubeSmall.png"));
      JLabel lblYouTube = new JLabel(youTubeIcon);
      LimitHeight(lblYouTube, h);
      
      JPanel pnlYouTube = new JPanel(new FlowLayout(FlowLayout.LEFT, 0, 0));
      pnlYouTube.add(m_chkYouTubeOptimzed);
      pnlYouTube.add(lblYouTube);
      
      JPanel pnlGlobal = new JPanel(new GridLayout(0, 2));
      pnlGlobal.add(m_pnlQuality);
      pnlGlobal.add(m_chkSingle);
      pnlGlobal.add(new JPanel()); // dummy
      pnlGlobal.add(pnlYouTube);
      Border borderTitle2 = BorderFactory.createTitledBorder(g_Localizer.getLocalized("SETTINGS"));
      pnlGlobal.setBorder(BorderFactory.createCompoundBorder(borderTitle2, borderEmpty5));
       
      
      
      JLabel lblFps = new JLabel(g_Localizer.getLocalized("FRAMERATE")+":  ");
      LimitHeight(lblFps, h);
      m_boxFps = new JComboBox(new String[] { "1", "2", "5", "10", "25", "30" });
      LimitHeight(m_boxFps, h);
      JLabel lblFps2 = new JLabel(g_Localizer.getLocalized("FPS"));
      LimitHeight(lblFps2, h);
      
      
      JLabel lblVideoRate = new JLabel(g_Localizer.getLocalized("VIDEO_QUALITY")+":  ");
      LimitHeight(lblVideoRate, h);
      m_boxVideoQFactor = new JComboBox(aVideoTexts);
      m_boxVideoQFactor.setSelectedItem("6");
      LimitHeight(m_boxVideoQFactor, h);
      JLabel lblVideoRate2 = new JLabel(""); // empty
      LimitHeight(lblVideoRate2, h);
    
      
      JLabel lblAudioRate = new JLabel(g_Localizer.getLocalized("AUDIO_QUALITY")+":  "); 
      LimitHeight(lblAudioRate, h);
      m_boxAudioRate = new JComboBox(aAudioTexts);
      m_boxAudioRate.setSelectedItem("32");
      LimitHeight(m_boxAudioRate, h);
      JLabel lblAudioRate2 = new JLabel(g_Localizer.getLocalized("KBITS"));
      LimitHeight(lblAudioRate2, h);
     
      
      JLabel lblVideoSize = new JLabel(g_Localizer.getLocalized("RESOLUTION")+":  ");
      LimitHeight(lblVideoSize, h);
      m_txtVideoWidth = new JTextField("320");
      m_txtVideoWidth.setHorizontalAlignment(JTextField.RIGHT);
      LimitHeight(m_txtVideoWidth, h);
      EnlargePreferredWidth(m_txtVideoWidth, 10);
      m_txtVideoHeight = new JTextField("240");
      m_txtVideoHeight.setHorizontalAlignment(JTextField.RIGHT);
      LimitHeight(m_txtVideoHeight, h);
      EnlargePreferredWidth(m_txtVideoHeight, 10);
      
      JPanel pnlVideoSize = new JPanel(new FlowLayout(FlowLayout.LEFT, 0, 0));
      pnlVideoSize.add(lblVideoSize);
      pnlVideoSize.add(m_txtVideoWidth);
      JLabel lblVideoSizeSpace = new JLabel(" x ");
      LimitHeight(lblVideoSizeSpace, h);
      pnlVideoSize.add(lblVideoSizeSpace);
      pnlVideoSize.add(m_txtVideoHeight);
   
      JPanel pnlTitlesLeft = new JPanel(new GridLayout(0, 1, 0, 1));
      pnlTitlesLeft.add(lblFps);
      pnlTitlesLeft.add(lblVideoRate);
     
      JPanel pnlValuesLeft = new JPanel(new GridLayout(0, 1, 0, 1));
      pnlValuesLeft.add(m_boxFps);
      pnlValuesLeft.add(m_boxVideoQFactor);
     
      JPanel pnlUnitiesLeft = new JPanel(new GridLayout(0, 1, 0, 1));
      pnlUnitiesLeft.add(lblFps2);
      pnlUnitiesLeft.add(lblVideoRate2);
      
      JPanel pnlTitlesRight = new JPanel(new GridLayout(0, 1, 0, 1));
      pnlTitlesRight.add(lblAudioRate);
      pnlTitlesRight.add(lblVideoSize);
      
      JPanel pnlValuesRight = new JPanel(new GridLayout(0, 1, 0, 1));
      pnlValuesRight.add(m_boxAudioRate);
      pnlValuesRight.add(pnlVideoSize);
      
      JPanel pnlUnitiesRight = new JPanel(new GridLayout(0, 1, 0, 1));
      pnlUnitiesRight.add(lblAudioRate2);
      JLabel lblDummy = new JLabel();
      LimitHeight(lblDummy, h);
      pnlUnitiesRight.add(lblDummy);
      
      
      JPanel pnlDetailsInnerLeftLeft = new JPanel(new BorderLayout(5, 0));
      pnlDetailsInnerLeftLeft.add("West", pnlValuesLeft);
      pnlDetailsInnerLeftLeft.add("Center", pnlUnitiesLeft);
     
      JPanel pnlDetailsInnerLeft = new JPanel(new BorderLayout(5, 0));
      pnlDetailsInnerLeft.add("West", pnlTitlesLeft);
      pnlDetailsInnerLeft.add("Center", pnlDetailsInnerLeftLeft);

      JPanel pnlDetailsInnerRightLeft = new JPanel(new BorderLayout(5, 0));
      pnlDetailsInnerRightLeft.add("West", pnlValuesRight);
      pnlDetailsInnerRightLeft.add("Center", pnlUnitiesRight);
     
      JPanel pnlDetailsInnerRight = new JPanel(new BorderLayout(5, 0));
      pnlDetailsInnerRight.add("West", pnlTitlesRight);
      pnlDetailsInnerRight.add("Center", pnlDetailsInnerRightLeft);
      
      
      m_pnlParameters = new JPanel(new GridLayout(1, 0));
      m_pnlParameters.add(pnlDetailsInnerLeft);
      m_pnlParameters.add(pnlDetailsInnerRight);
      Border borderTitle3 = BorderFactory.createTitledBorder(g_Localizer.getLocalized("DETAIL_SETTINGS"));
      m_pnlParameters.setBorder(BorderFactory.createCompoundBorder(borderTitle3, borderEmpty5));
      
      
      EnablePanel(m_pnlParameters, false, null);
      FillQuality();
      
      // if done earlier FillQuality() will fire actionPerformed events
      // and thus call FindQuality() which sets the wrong quality string
      m_boxFps.addActionListener(this);
      m_boxVideoQFactor.addActionListener(this);
      m_boxAudioRate.addActionListener(this);
      
      
      m_pnlContent = new JPanel(new BorderLayout(0, 3));
      m_pnlContent.add("North", m_pnlMedia);
      m_pnlContent.add("Center", pnlGlobal);
      m_pnlContent.add("South", m_pnlParameters);
      
      JPanel pnlOuter = new JPanel(new BorderLayout());
      pnlOuter.add("North", m_pnlContent);
      
///      pnlContainer.add("North", pnlFind);
      pnlContainer.add("Center", pnlOuter);

      // Get init values from m_pwData
      getInitValues();
   }


   public String getDescription()
   {
      return "Settings for Video Conversion Dialog";
   }
   
   public int displayWizard()
   {
      DocumentData dd = m_pwData.GetDocumentData();
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

///      if (ps.GetTargetFileName().length() > 0)
///         m_txtName.setText(ps.GetTargetFileName() + ".mp4");
///      else
///      {
///         String strPrefix = "output";
///         
///         if (dd.GetPresentationFileName().length() > 0)
///         {
///            //File pathFile = new File(dd.GetPresentationFileName()).getParentFile();
///
///            strPrefix = dd.GetPresentationFileName();
///            if (strPrefix.toLowerCase().endsWith(".lrd") || strPrefix.toLowerCase().endsWith(".mp4"))
///               strPrefix = strPrefix.substring(0, strPrefix.length()-4);
///         }
///            
///         m_txtName.setText(strPrefix + ".mp4");
///      }
      
      // defaults to "true" but is not always visible and used
      m_chkClips.setSelected(ps.GetBoolValue("bMp4EmbedClips"));
      
      if (m_pwData.IsDenverDocument() || ps.GetBoolValue("bMp4VideoInsteadOfPages"))
         m_rbtVideo.setSelected(true);
      else
         m_rbtPages.setSelected(true);
      
      if (dd.HasNormalVideo() && !LayoutUtils.Contains(m_pnlMedia, m_rbtVideo))
         m_pnlMedia.add(m_rbtVideo);
      else if (!dd.HasNormalVideo() && LayoutUtils.Contains(m_pnlMedia, m_rbtVideo))
         m_pnlMedia.remove(m_rbtVideo);
      
      if (!dd.HasNormalVideo())
         m_rbtPages.setSelected(true); // just make sure
      
      if (!dd.IsDenver() && dd.HasClips())
         m_chkClips.setVisible(true);
      else
         m_chkClips.setVisible(false);
      
      boolean bShowMediaPanel = 
         !dd.IsDenver() && (dd.HasNormalVideo() || dd.HasClips());

      boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && ps.GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
      
      if (bIsGenericProfile) {
          // show all options for generic profiles
          m_chkClips.setVisible(true);
          bShowMediaPanel = true;
          if (!LayoutUtils.Contains(m_pnlMedia, m_rbtVideo))
              m_pnlMedia.add(m_rbtVideo);
      }
     
      if (bShowMediaPanel && !LayoutUtils.Contains(m_pnlContent, m_pnlMedia))
         m_pnlContent.add("North", m_pnlMedia);
      else if (!bShowMediaPanel && LayoutUtils.Contains(m_pnlContent, m_pnlMedia))
         m_pnlContent.remove(m_pnlMedia);
      
      ((JFrame)getFrame()).getContentPane().validate();
      //m_pnlContent.validate();
     
      // remainder of initialization (value setting) done
      // by FillQuality() in the constructor
      
      // not regarded at the moment:
      //m_txtVideoWidth.setText(ps.GetIntValue("iMp4VideoWidth")+"");
      //m_txtVideoHeight.setText(ps.GetIntValue("iMp4VideoHeight")+"");
       
      return super.displayWizard();
   }

   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      // integrity/validity of data has been checked in verifyNext()
      
      String strPercent = (String)m_boxVideoQFactor.getSelectedItem();
      int iPercent = Integer.parseInt(strPercent.substring(0,2));
      int iWidth = Integer.parseInt(m_txtVideoWidth.getText());
      int iHeight = Integer.parseInt(m_txtVideoHeight.getText());

      ps.SetIntValue("iMp4QualityType", m_boxQuality.getSelectedIndex());
      ps.SetBoolValue("bMp4ChangeDetails", m_chkSingle.isSelected());
      ps.SetBoolValue("bMp4YouTubeOptimized", m_chkYouTubeOptimzed.isSelected());
//      if( m_chkYouTubeOptimzed.isSelected() && !m_pwData.m_bIsProfileMode && !m_pwData.m_bIsDirectMode) {
//          // bugfix 5471 - YouTube upload should be preselected
//          ps.SetIntValue("iReplayType", PublisherWizardData.REPLAY_SERVER);
//          ps.SetIntValue("iServerType", PublisherWizardData.SRV_YOUTUBE);
//          ps.SetIntValue("iTransferType", PublisherWizardData.TRANSFER_UPLOAD);
//      }

      ps.SetIntValue("iMp4FrameRate", Integer.parseInt((String)m_boxFps.getSelectedItem()));
      ps.SetIntValue("iMp4AudioBitrate", Integer.parseInt((String)m_boxAudioRate.getSelectedItem()));
      ps.SetIntValue("iMp4VideoQFactor", 31 - (int)Math.round(iPercent/100.0 * 31));
      ps.SetIntValue("iMp4VideoWidth", iWidth);
      ps.SetIntValue("iMp4VideoHeight", iHeight);
///      ps.SetStringValue("strTargetFileName", m_txtName.getText());
      ps.SetBoolValue("bMp4EmbedClips", m_pwData.IsDenverDocument() || m_chkClips.isSelected());
      ps.SetBoolValue("bMp4VideoInsteadOfPages", m_pwData.IsDenverDocument() || m_rbtVideo.isSelected());

      if (m_NextWizardPanel == null)
         m_NextWizardPanel = new TargetChoiceWizardPanel(m_pwData);
///         m_NextWizardPanel = new VideoWriterWizardPanel(m_pwData);

      m_pwData.nextWizardPanel = m_NextWizardPanel; 
      
      return m_pwData;
   }
   
   public boolean verifyNext()
   {
      DocumentData dd = m_pwData.GetDocumentData();
      
      File pathFile = new File(dd.GetPresentationFileName()).getParentFile();
      File audioFile = new File(pathFile, dd.GetAudioFileName());
 
      float fSampleRate = -1.0f;
      try 
      {
         BufferedInputStream bis = new BufferedInputStream(new FileInputStream(audioFile));
         AudioInputStream input = AudioSystem.getAudioInputStream(bis);
            
         fSampleRate = input.getFormat().getSampleRate();
            
         input.close();
         bis.close();
      } 
      catch (Exception exc)
      {
         exc.printStackTrace();
      }
      
      if (fSampleRate > 0)
      {
         if ((int)fSampleRate == 22050)
         {
            Object[] options = {g_Localizer.getLocalized("YES"), g_Localizer.getLocalized("NO")};
         
            int res = 
               JOptionPane.showOptionDialog(this, g_Localizer.getLocalized("RATE_22_QUICKTIME_74"),
                                            g_Localizer.getLocalized("WARNING"), JOptionPane.YES_NO_OPTION, 
                                            JOptionPane.WARNING_MESSAGE, null, options, options[0]);
            if (res != 0) // Not 'yes'
               return false;
         }
      }
      
///      if (m_txtName.getText().equals(""))
///      {
///         JOptionPane.showMessageDialog(this, g_Localizer.getLocalized("SPECIFY_NAME"),
///                                       g_Localizer.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
///         return false;
///      }

///      boolean bPathFileIsOk = FileUtils.checkUrlPathName(m_txtName.getText());
///      if (!bPathFileIsOk)
///      {
///         Object[] options = {g_Localizer.getLocalized("YES"), g_Localizer.getLocalized("NO")};
///         int res = 
///            JOptionPane.showOptionDialog(this, g_Localizer.getLocalized("WARN_INVALID_PATH_FILE"),
///                                         g_Localizer.getLocalized("WARNING"), JOptionPane.YES_NO_OPTION, 
///                                         JOptionPane.WARNING_MESSAGE, null, options, options[1]);
///         if (res != 0) // Not 'yes'
///            return false;
///      }

///      if (!FileUtils.isWritable(this, m_txtName.getText()))
///         return false;
      
      int iFrameRate = 0, iAudioRate = 0, iVideoQFactor = 0, iWidth = 0, iHeight = 0;
      try
      {
         //iFrameRate = Integer.parseInt((String)m_boxFps.getSelectedItem());
         iAudioRate = Integer.parseInt((String)m_boxAudioRate.getSelectedItem());
         String strPercent = (String)m_boxVideoQFactor.getSelectedItem();
         int iPercent = Integer.parseInt(strPercent.substring(0,2));
         iVideoQFactor = 31 - (int)(iPercent/100.0 * 31);
         iWidth = Integer.parseInt(m_txtVideoWidth.getText());
         iHeight = Integer.parseInt(m_txtVideoHeight.getText());
      }
      catch (NumberFormatException exc)
      {
         // ignore, one of the variables remains 0
      }
 
      if (iAudioRate == 0 || iVideoQFactor == 0 || iWidth == 0 || iHeight == 0)
      {
         JOptionPane.showMessageDialog(this, g_Localizer.getLocalized("POSITIVE_SIZE"),
                                       g_Localizer.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
         return false;
      }
      
      if (iWidth % 16 != 0 || iHeight % 16 != 0)
      {
         JOptionPane.showMessageDialog(this, g_Localizer.getLocalized("DIVISABLE_16"),
                                       g_Localizer.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
         return false;
      }
      
      // Bugfix 4507 (also 3863): Special check for video size which should not exceed 3 MegaPixel
      int nMaxPixelSize = 3000000;
      if (iWidth * iHeight > nMaxPixelSize)
      {
         JOptionPane.showMessageDialog(this, g_Localizer.getLocalized("VIDEO_SIZE_TOO_BIG"),
                                       g_Localizer.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
         return false;
      }
        
      return true;
   }

   public void actionPerformed(ActionEvent evt)
   {
///      if (evt.getSource() == m_btnSearch)
///      {
///         JFileChooser fileChooser = new JFileChooser();
///         if (!m_txtName.getText().equals(""))
///            fileChooser.setCurrentDirectory(new File(m_txtName.getText()));
///         if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
///            fileChooser.setBackground(Wizard.BG_COLOR);
///         Dimension fcSize = fileChooser.getPreferredSize();
///         fileChooser.setPreferredSize(new Dimension(fcSize.width + 100, fcSize.height + 50));
///
///         /*
///         LecturnityFileFilter lff = new LecturnityFileFilter();
///         fileChooser.addChoosableFileFilter(lff);
///         EPresentationFileFilter eff = new EPresentationFileFilter();
///         fileChooser.addChoosableFileFilter(eff);
///         AofFileFilter aff = new AofFileFilter();
///         fileChooser.addChoosableFileFilter(aff);
///         fileChooser.addChoosableFileFilter(fileChooser.getAcceptAllFileFilter());
///         fileChooser.setFileFilter(lff);
///         fileChooser.setDialogTitle(CHOOSER_TITLE);
///         fileChooser.setApproveButtonText(APPROVE);
///         fileChooser.setApproveButtonToolTipText(APPROVE);
///         */
///
///         int action = fileChooser.showSaveDialog(this);
///
///         if (action == fileChooser.APPROVE_OPTION)
///         {
///            File selectedFile = fileChooser.getSelectedFile();
///            m_txtName.setText(selectedFile.getAbsolutePath());
///         }
///
///      }
///      else if (evt.getSource() == m_chkSingle)

      if (evt.getSource() == m_chkSingle)
      {
         EnablePanel(m_pnlParameters, m_chkSingle.isSelected(), null);
      }
      else if (evt.getSource() == m_chkYouTubeOptimzed)
      {
         EnablePanel(m_pnlParameters, !m_chkYouTubeOptimzed.isSelected() && m_chkSingle.isSelected(), null);
         EnablePanel(m_pnlQuality, !m_chkYouTubeOptimzed.isSelected(), null);
         m_chkSingle.setEnabled(!m_chkYouTubeOptimzed.isSelected());
      }
      else if (evt.getSource() == m_boxQuality)
      {
         FillQuality(); // "Custom" will be ignored
      }
      else // one of the detail fields
      {
         if (!m_bFillActive)
            FindQuality();
      }
   }
 
   private void getInitValues()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      int iQFactor = ps.GetIntValue("iMp4VideoQFactor");
      int iPercent = (int)Math.round((100.0 / 31.0) * (31-iQFactor));
      // The above formula is not exact enough to get the correct percentage.
      // iPercent must be a multiple of 5
      int mod = iPercent % 5;
      if (mod > 2)
         iPercent += (5-mod);
      else
         iPercent -= mod;
      String strPercent = iPercent + "%";

      m_boxFps.setSelectedItem(ps.GetIntValue("iMp4FrameRate")+"");
      m_boxAudioRate.setSelectedItem(ps.GetIntValue("iMp4AudioBitrate")+"");
      m_boxVideoQFactor.setSelectedItem(strPercent);
      if (ps.GetIntValue("iMp4VideoWidth") > 0)
         m_txtVideoWidth.setText(ps.GetIntValue("iMp4VideoWidth")+"");
      if (ps.GetIntValue("iMp4VideoHeight") > 0)
         m_txtVideoHeight.setText(ps.GetIntValue("iMp4VideoHeight")+"");
      m_chkClips.setSelected(ps.GetBoolValue("bMp4EmbedClips"));
      if (m_pwData.HasNormalVideo())
         m_rbtVideo.setSelected(m_pwData.IsDenverDocument() || ps.GetBoolValue("bMp4VideoInsteadOfPages"));

      m_boxQuality.setSelectedIndex(ps.GetIntValue("iMp4QualityType"));
      m_chkSingle.setSelected(ps.GetBoolValue("bMp4ChangeDetails"));
      m_chkYouTubeOptimzed.setSelected(ps.GetBoolValue("bMp4YouTubeOptimized"));
      m_chkSingle.setEnabled(!m_chkYouTubeOptimzed.isSelected());
      EnablePanel(m_pnlQuality, !m_chkYouTubeOptimzed.isSelected(), null);
      EnablePanel(m_pnlParameters, m_chkSingle.isSelected() && !m_chkYouTubeOptimzed.isSelected(), null);
   }

   private void LimitHeight(JComponent comp, int iPrefHeight)
   {
      Dimension dimComp = comp.getPreferredSize();
      if (dimComp.height > iPrefHeight)
      {
         dimComp.height = iPrefHeight;
         comp.setPreferredSize(dimComp);
      }
   }
   
   private void EnlargePreferredWidth(JComponent comp, int iAddWidth)
   {
      Dimension dimComp = comp.getPreferredSize();
      dimComp.width += iAddWidth;
      comp.setPreferredSize(dimComp);
   }
  
   static private void EnablePanel(Container pnlTarget, boolean bEnable, JComponent compException)
   {
      int iCount = pnlTarget.getComponentCount();
      for (int i=0; i<iCount; ++i)
      {
         Component comp = pnlTarget.getComponent(i);
         if (comp instanceof JComponent)
         {
            JComponent c = (JComponent)comp;
            if (c != compException)
            {
               EnablePanel(c, bEnable, compException);
               c.setEnabled(bEnable);
            }
         }
      }
   }
   
   private void FillQuality()
   {
      m_bFillActive = true;
      for (int i=0; i<m_aaDefaults.length; ++i)
      {
         String strSelectedQuality = (String)m_boxQuality.getSelectedItem();
         if (strSelectedQuality.equals(m_aaDefaults[i][0]))
         {
            m_boxFps.setSelectedItem(m_aaDefaults[i][1]);
            m_boxVideoQFactor.setSelectedItem(m_aaDefaults[i][2]);
            m_boxAudioRate.setSelectedItem(m_aaDefaults[i][3]);
            
            break;
         }
      }
      m_bFillActive = false;
   }
   
   // sets the quality string
   private void FindQuality()
   {
      boolean bFound = false;
      for (int i=0; i<m_aaDefaults.length; ++i)
      {
         boolean bEveryThingMatches = true;
      
         if (!m_aaDefaults[i][1].equals((String)m_boxFps.getSelectedItem()))
            bEveryThingMatches = false;
         if (!m_aaDefaults[i][2].equals((String)m_boxVideoQFactor.getSelectedItem()))
            bEveryThingMatches = false;
         if (!m_aaDefaults[i][3].equals((String)m_boxAudioRate.getSelectedItem()))
            bEveryThingMatches = false;
         
         if (bEveryThingMatches)
         {
            m_boxQuality.setSelectedItem(m_aaDefaults[i][0]);
            bFound = true;
            break;
         }
      }
      
      if (!bFound)
         m_boxQuality.setSelectedItem(g_Localizer.getLocalized("CUSTOM"));
   }
   
   
}

