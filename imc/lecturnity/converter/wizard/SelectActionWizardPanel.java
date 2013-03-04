package imc.lecturnity.converter.wizard;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.wizards.Wizard;
import imc.lecturnity.util.wizards.WizardPanel;

import java.awt.Container;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.*;

import javax.swing.ButtonGroup;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.border.TitledBorder;

public class SelectActionWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String CAPTION = "[!]";
   private static String REALMEDIA = "[!]";
   private static char   MNEM_REALMEDIA = '?';
   //private static String REALMEDIA_DESC = "[!]";
   private static String LECTURNITY = "[!]";
   private static char   MNEM_LECTURNITY = '?';
   //private static String LECTURNITY_DESC = "[!]";
   private static String WINDOWSMEDIA = "[!]";
   private static char   MNEM_WINDOWSMEDIA = '?';
   private static String FLASH = "[!]";
   private static char   MNEM_FLASH = '?';
   private static String VIDEO = "[!]";
   private static char   MNEM_VIDEO = '?';
   //private static String STREAMING = "[!]";
   private static String EXTENDED = "[!]";
//   private static String ERROR = "[!]";
//   private static String WARNING = "[!]";

   private static String FORMATS = "[!]";
   private static String PROPERTIES = "[!]";
   private static String WEB_DOCUMENT = "[!]";
   private static String STREAMING_ENABLED = "[!]";
   private static String SCORM_COMPLIANT = "[!]";
   private static String VIDEO_SG = "[!]";
   private static String NAVIGATION = "[!]";
   private static String FULLTEXT = "[!]";
   private static String YES = "[!]";
   private static String NO = "[!]";
   private static String COND = "[!]";
   private static String[] YESNOCOND = {"[!]", "[!]", "[!]"};
//   private static String IS_STANDALONE = "[!]";
//   private static String CONTAINS_UNSUPPORTED = "[!]";
//   private static String CONTAINS_STILL_IMAGE = "[!]";
//   private static String UNSUPPORTED_PATH = "[!]";
//   private static String PROBLEMATIC_PATH = "[!]";
   
   private static String STRUCTURE_CLIPS = "[!]";
   private static char   MNEM_STRUCTURE_CLIPS = '?';
   private static String CONTAINS_TEST_MESSAGE = "[!]";
   private static String CONTAINS_TEST_MESSAGE_TITLE = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer("/imc/lecturnity/converter/wizard/SelectActionWizardPanel_",
                                     "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         CAPTION = l.getLocalized("CAPTION");
         REALMEDIA = l.getLocalized("REALMEDIA");
         MNEM_REALMEDIA = l.getLocalized("MNEM_REALMEDIA").charAt(0);
         //REALMEDIA_DESC = l.getLocalized("REALMEDIA_DESC");
         LECTURNITY = l.getLocalized("LECTURNITY");
         MNEM_LECTURNITY = l.getLocalized("MNEM_LECTURNITY").charAt(0);
         //LECTURNITY_DESC = l.getLocalized("LECTURNITY_DESC");
         WINDOWSMEDIA = l.getLocalized("WINDOWSMEDIA");
         MNEM_WINDOWSMEDIA = l.getLocalized("MNEM_WINDOWSMEDIA").charAt(0);
         FLASH = l.getLocalized("FLASH");
         MNEM_FLASH = l.getLocalized("MNEM_FLASH").charAt(0);
         VIDEO = l.getLocalized("VIDEO");
         MNEM_VIDEO = l.getLocalized("MNEM_VIDEO").charAt(0);
         //STREAMING = l.getLocalized("STREAMING");
         EXTENDED = l.getLocalized("EXTENDED");
//         ERROR = l.getLocalized("ERROR");
//         WARNING = l.getLocalized("WARNING");

         FORMATS = l.getLocalized("FORMATS");
         PROPERTIES = l.getLocalized("PROPERTIES");
         WEB_DOCUMENT = l.getLocalized("WEB_DOCUMENT");
         STREAMING_ENABLED = l.getLocalized("STREAMING_ENABLED");
         SCORM_COMPLIANT = l.getLocalized("SCORM_COMPLIANT");
         VIDEO_SG = l.getLocalized("VIDEO_SG");
         NAVIGATION = l.getLocalized("NAVIGATION");
         FULLTEXT = l.getLocalized("FULLTEXT");
         YES = l.getLocalized("YES");
         NO = l.getLocalized("NO");
         COND = l.getLocalized("COND");
//         IS_STANDALONE = l.getLocalized("IS_STANDALONE");
//         CONTAINS_UNSUPPORTED = l.getLocalized("CONTAINS_UNSUPPORTED");
//         CONTAINS_STILL_IMAGE = l.getLocalized("CONTAINS_STILL_IMAGE");
//         UNSUPPORTED_PATH = l.getLocalized("UNSUPPORTED_PATH");
//         PROBLEMATIC_PATH = l.getLocalized("PROBLEMATIC_PATH");

         YESNOCOND[0] = YES;
         YESNOCOND[1] = NO;
         YESNOCOND[2] = COND;
         
         STRUCTURE_CLIPS = l.getLocalized("STRUCTURE_CLIPS");
         MNEM_STRUCTURE_CLIPS = l.getLocalized("MNEM_STRUCTURE_CLIPS").charAt(0);
         CONTAINS_TEST_MESSAGE = l.getLocalized("CONTAINS_TEST_MESSAGE");
         CONTAINS_TEST_MESSAGE_TITLE = l.getLocalized("CONTAINS_TEST_MESSAGE_TITLE");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Access to Locale database failed!", "Error", 
                                       JOptionPane.ERROR_MESSAGE);
      }
   }

   public static WizardPanel createInstance(PublisherWizardData pwData)
   {
      return new SelectActionWizardPanel(pwData);
   }

   private PublisherWizardData pwData_;

   private WizardPanel nextRealWizard_ = null;
   private WizardPanel nextLecturnityWizard_ = null;
   private WizardPanel nextWindowsWizard_ = null;
   private WizardPanel nextFlashWizard_ = null;
   private WizardPanel nextVideoWizard_ = null;

   private JRadioButton realExportButton_;
   private JRadioButton lecturnityExportButton_;
   private JRadioButton windowsExportButton_;
   private JRadioButton flashExportButton_;
   private JRadioButton videoExportButton_;
   
   private JCheckBox structureClipBox_; 

   private int iLastExportType_ = -1;
   private int m_iLastProfileExportType = -1;

//    private JLabel presentationFileNameLabel_;
   private JLabel[] propertiesLabels_;

   // Properties column (see 'titles' below): 
   // WEB_DOCUMENT, STREAMING_ENABLED, SCORM_COMPLIANT, VIDEO_SG, NAVIGATION, FULLTEXT 
   // Values: 0=yes, 1=no, 2=conditional
   private int[][] properties_ = {{0, 0, 2, 0, 0, 0}, // RM
                                  {0, 0, 2, 0, 0, 0}, // WM
                                  {0, 0, 2, 0, 2, 2}, // Flash
                                  {2, 2, 1, 0, 1, 1}, // Video (Podcast)
                                  {1, 1, 1, 0, 0, 0}}; // LPD

   public SelectActionWizardPanel(PublisherWizardData pwData)
   {
      super();

      setPreferredSize(new Dimension(500, 400));

      pwData_ = pwData;

      int iButtonsMask = BUTTON_NEXT | BUTTON_CANCEL;
      if (!pwData_.m_bIsProfileMode && !pwData_.m_bDisableSelectPresentation)
         iButtonsMask |= BUTTON_BACK;
      addButtons(iButtonsMask);
      useHeaderPanel(HEADER, SUBHEADER, 
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");
      addButtons(BUTTON_CUSTOM, EXTENDED, 'e');
      setEnableButton(BUTTON_CUSTOM, true);

      Container p = getContentPanel();
      p.setLayout(null);

      JPanel formatPanel = new JPanel();
      formatPanel.setLayout(null);
      formatPanel.setBorder(new TitledBorder(FORMATS));
      formatPanel.setSize(460, 125);
      formatPanel.setLocation(20, 5); // (20, 20);
      p.add(formatPanel);


      ButtonGroup bg = new ButtonGroup();
      ActionListener actionListener = new ActionListener()
         {
            public void actionPerformed(ActionEvent e)
            {
               updateFormatProperties();
            }
         };

      int yPos = 15;
      int yStep = 20;
      
      realExportButton_ = new JRadioButton(REALMEDIA, false);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         realExportButton_.setBackground(Wizard.BG_COLOR);
      realExportButton_.setLocation(20, yPos);
      realExportButton_.setSize(420, yStep);
      realExportButton_.setMnemonic(MNEM_REALMEDIA);
      bg.add(realExportButton_);
      formatPanel.add(realExportButton_);
      realExportButton_.setEnabled(ConverterWizard.hasRealMedia());
      realExportButton_.addActionListener(actionListener);
      
      yPos += yStep;

      windowsExportButton_ = new JRadioButton(WINDOWSMEDIA, false);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         windowsExportButton_.setBackground(Wizard.BG_COLOR);
      //windowsExportButton_.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12));
      windowsExportButton_.setLocation(20, yPos);
      windowsExportButton_.setSize(420, yStep);
      windowsExportButton_.setMnemonic(MNEM_WINDOWSMEDIA);
      bg.add(windowsExportButton_);
      formatPanel.add(windowsExportButton_);
      windowsExportButton_.setEnabled(ConverterWizard.hasWindowsMedia());
      windowsExportButton_.addActionListener(actionListener);

      yPos += yStep;

      flashExportButton_ = new JRadioButton(FLASH, false);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         flashExportButton_.setBackground(Wizard.BG_COLOR);
      flashExportButton_.setLocation(20, yPos);
      flashExportButton_.setSize(420, yStep);
      flashExportButton_.setMnemonic(MNEM_FLASH);
      bg.add(flashExportButton_);
      formatPanel.add(flashExportButton_);
      flashExportButton_.addActionListener(actionListener);

      yPos += yStep;

      videoExportButton_ = new JRadioButton(VIDEO, false);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         videoExportButton_.setBackground(Wizard.BG_COLOR);
      videoExportButton_.setLocation(20, yPos);
      videoExportButton_.setSize(420, yStep);
      videoExportButton_.setMnemonic(MNEM_VIDEO);
      bg.add(videoExportButton_);
      formatPanel.add(videoExportButton_);
      videoExportButton_.setEnabled(ConverterWizard.hasMp4Publisher());
      videoExportButton_.addActionListener(actionListener);
      
      yPos += yStep;

      lecturnityExportButton_ = new JRadioButton(LECTURNITY, true);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         lecturnityExportButton_.setBackground(Wizard.BG_COLOR);
      lecturnityExportButton_.setLocation(20, yPos);
      lecturnityExportButton_.setSize(420, yStep);
      lecturnityExportButton_.setMnemonic(MNEM_LECTURNITY);
      bg.add(lecturnityExportButton_);
      formatPanel.add(lecturnityExportButton_);
      lecturnityExportButton_.addActionListener(actionListener);
      
//       WizardTextArea lecturnityText = new WizardTextArea
//          (LECTURNITY_DESC,
//           new Point(70, 185),
//           new Dimension(400,90),
//           imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
         
//       p.add(lecturnityText);
      
      JPanel propertiesPanel = new JPanel();
      propertiesPanel.setLayout(null);
      propertiesPanel.setBorder(new TitledBorder(PROPERTIES));
      propertiesPanel.setSize(460, 120);
      propertiesPanel.setLocation(20, 135); // (20, 155);
      p.add(propertiesPanel);

      String[] titles = {WEB_DOCUMENT,
                         STREAMING_ENABLED,
                         SCORM_COMPLIANT,
                         VIDEO_SG,
                         NAVIGATION,
                         FULLTEXT};
      propertiesLabels_ = new JLabel[titles.length];
      for (int i=0; i<titles.length; ++i)
      {
         JLabel tmpLabel = new JLabel(titles[i]);
         tmpLabel.setSize(200, 15);
         tmpLabel.setLocation(20, 20+i*15);
         propertiesPanel.add(tmpLabel);
         propertiesLabels_[i] = new JLabel(YESNOCOND[properties_[4][i]]); // Default is LECTURNITY format
         propertiesLabels_[i].setSize(220, 15);
         propertiesLabels_[i].setLocation(220, 20+i*15);
         propertiesPanel.add(propertiesLabels_[i]);
      }
      
      // PZI: checkbox to enable/disable automated processing of clip structure
      // create always to enable access (to avoid additional check before each access)
      structureClipBox_ = new JCheckBox(STRUCTURE_CLIPS, true);
      structureClipBox_.setSize(420, yStep);
      structureClipBox_.setLocation(25, 260); // (40, 110);
      structureClipBox_.setMnemonic(MNEM_STRUCTURE_CLIPS);
      structureClipBox_.setEnabled(true);
      // only add if required
      boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && pwData.GetProfiledSettings().GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
      if (!bIsGenericProfile && (pwData.IsDenverDocument() || pwData.GetDocumentData().isClipStructured())) {
         p.add(structureClipBox_);
         // // add border
         // JPanel structuringPanel = new JPanel();
         // structuringPanel.setLayout(null);
         // structuringPanel.setBorder(new TitledBorder("Advanced"));
         // structuringPanel.setSize(460, 25+yStep);
         // structuringPanel.setLocation(20, 240);
         // structureClipBox_.setLocation(20, 16);
         // structuringPanel.add(structureClipBox_);
         // p.add(structuringPanel);
      }
      
      refreshDataDisplay();
   }

   public String getDescription()
   {
      return CAPTION;
   }

   public void refreshDataDisplay()
   {
      super.refreshDataDisplay();

      ProfiledSettings.ProfileInfo pi = ProfiledSettings.GetActiveProfile();
      if ((pi != null) || pwData_.m_bIsProfileMode)
         setButtonVisible(BUTTON_CUSTOM, false);
      else
         setButtonVisible(BUTTON_CUSTOM, true);

      int iExportType = PublisherWizardData.EXPORT_TYPE_LECTURNITY;
      // use either registry (first attempt) or the value from the profile (if specified)
      
      /* Not yet desired (19.05.08); see above
         "New profile" should also use the saved type as default
         Also note: The type should not be always reset; eg not when 
         reaching this dialog with "Previous".
         
      String strLastExportType = NativeUtils.getPublisherProperty("LastExportType");
      if (strLastExportType != null && strLastExportType.length() > 0)
      {
         try
         {
            iExportType = Integer.parseInt(strLastExportType);
         }
         catch (NumberFormatException exc)
         {
            System.err.println("Wrong export type in registry: "+strLastExportType);
            // do nothing
         }
      }
      */
      
      if (pwData_.m_bIsProfileMode)
      {
         // PZI: set structuring mode 
         structureClipBox_.setSelected(pwData_.GetProfiledSettings().GetBoolValue("bStructuredClips"));
         
         iExportType = pwData_.GetProfiledSettings().GetIntValue("iExportType");
         
         if (iExportType != m_iLastProfileExportType)
         {
            m_iLastProfileExportType = iExportType;
      
            JRadioButton btnActive = null;
      
            if (iExportType == PublisherWizardData.EXPORT_TYPE_VIDEO)
               btnActive = videoExportButton_;
            else if (iExportType == PublisherWizardData.EXPORT_TYPE_REAL)
               btnActive = realExportButton_;
            else if (iExportType == PublisherWizardData.EXPORT_TYPE_WMT)
               btnActive = windowsExportButton_;
            else if (iExportType == PublisherWizardData.EXPORT_TYPE_FLASH)
               btnActive = flashExportButton_;
            else
               btnActive = lecturnityExportButton_;
         
            btnActive.setSelected(true);
            btnActive.requestFocus();

            updateFormatProperties();
         }
         // else: avoid resetting a recent user change; will also be set in getWizardData()
         
      }
   }

   private void updateFormatProperties()
   {
      int format = 4; // LECTURNITY
      if (realExportButton_.isSelected())
         format = 0;
      else if (windowsExportButton_.isSelected())
         format = 1;
      else if (flashExportButton_.isSelected())
         format = 2;
      else if (videoExportButton_.isSelected())
         format = 3;

      for (int i=0; i<6; ++i)
      {
         propertiesLabels_[i].setText(YESNOCOND[properties_[format][i]]);
      }
   }

   public boolean verifyNext()
   {
      // PZI: prepare automated structuring of Denver documents or reset
      // This must only be done for SG only ("Denver") or Structured SG documents
      if (pwData_.IsDenverDocument() || pwData_.GetDocumentData().isClipStructured()) {
         if (pwData_.GetDocumentData().isAddClipStructure() != structureClipBox_.isSelected()) {
            // reload document to reload profile settings
            // used to reset data if user uses back button in subsequent dialogs
            // NOTE: pwData_.UpdateDataAndSettings() would do, but is not visible:

            // Bugfix 5089: The next call ("ReadDocumentDataFromPresentationFile") 
            // leads to a reset of the profiles (which will be not desired in case of 
            // changing an already existing profile)
            // --> outcommented (this may lead to the inconsistencies 
            // if the user uses the back button, but that will be a new bug (if any))
//            pwData_.ReadDocumentDataFromPresentationFile(pwData_.GetDocumentData()
//                    .GetPresentationFileName());
            pwData_.GetDocumentData().setAddClipStructure(structureClipBox_.isSelected());
            pwData_.GetDocumentData().updateForStructuredScreengrabbing(
                    pwData_.GetProfiledSettings());
         }
         // Bugfix 5329: Wrong clip size for RM/WM
         if (pwData_.GetDocumentData().isClipStructured() && !structureClipBox_.isSelected()) {
             // reset data to pure SG values
             ProfiledSettings ps = pwData_.GetProfiledSettings();
             
             ps.SetIntValue("iClipSizeType", PublisherWizardData.CLIPSIZE_STANDARD);
             
             ps.SetIntValue("iMaxCustomClipHeight", 0);
             ps.SetIntValue("iMaxCustomClipWidth", 0);
             
             ps.SetIntValue("iSlideHeight", 0);
             ps.SetIntValue("iSlideWidth", 0);
             
             ps.SetIntValue("iVideoSizeType", PublisherWizardData.VIDEOSIZE_MAXCLIP);
             Dimension maxVideoSize = pwData_.GetMaximumVideoSize(true);
             ps.SetIntValue("iVideoWidth", maxVideoSize.width);
             ps.SetIntValue("iVideoHeight", maxVideoSize.height);
         }
      }
      
      if(CheckForTests())
      	javax.swing.JOptionPane.showMessageDialog(
                this, CONTAINS_TEST_MESSAGE,
                CONTAINS_TEST_MESSAGE_TITLE, javax.swing.JOptionPane.WARNING_MESSAGE);
      
//// Since LEC 4.0: The next check is not necessary for the new Flash Player
//      if (flashExportButton_.isSelected())
//      {
//         if (pwData_.GetDocumentData().HasStillImage())
//         {
//            JOptionPane.showMessageDialog(this, CONTAINS_STILL_IMAGE, WARNING, 
//                                          JOptionPane.WARNING_MESSAGE);
//         }
//      }

      return true;
   }
   
   private boolean CheckForTests()
   {
   		boolean bRetVal = false;
   		
   		String lrdFile = pwData_.GetDocumentData().GetPresentationFileName();
   		
   		
   		String searchText = "QUESTIONNAIRE";
   		String strLep = lrdFile.substring(0, lrdFile.length() - 3) + "lep";
		String fileName = strLep;
		
		File lepFile = new File(strLep);
		if(lepFile.exists())
		{
			String fileText = ReadFileContent(fileName);
			if (fileText != null && fileText.length() > 0)
			{
				int indexOfQQ = fileText.indexOf(searchText);
				System.out.println(strLep + " - " + indexOfQQ);

				if (indexOfQQ != -1)
					bRetVal = true;
			}
		}
		else
		{
			String strObj = pwData_.GetDocumentData().GetObjFileName(true);
			File objFile = new File(strObj);
			if (objFile.exists())
			{
				String strObjText = ReadFileContent(strObj);
				if (strObjText != null && strObjText.length() > 0)
				{
					int indexOfQQ = strObjText.indexOf(searchText);
					System.out.println(strObj + " - " + indexOfQQ);
					if (indexOfQQ != -1)
						bRetVal = true;
				}
			}
		}
				
   		return bRetVal;
   }

	private String ReadFileContent(String strFilename)
	{
		StringBuilder sb = new StringBuilder();

		try
		{
			BufferedReader reader = new BufferedReader(new FileReader(strFilename));

			//Reads until the end-of-file met
			while (reader.ready())
			{
				//Read line-by-line directly
				sb.append(reader.readLine());
			}

		}
		catch (IOException ex)
		{
			ex.printStackTrace();
		}

		return sb.toString();
	}

   public PublisherWizardData getWizardData()
   {
      // PZI: set structuring mode 
      pwData_.GetProfiledSettings().SetBoolValue("bStructuredClips", structureClipBox_.isSelected()); 
      
      int iExportType = -1;
      if (realExportButton_.isSelected())
      {
         // RealMedia export
         iExportType = PublisherWizardData.EXPORT_TYPE_REAL;
         pwData_.GetProfiledSettings().SetIntValue("iExportType", iExportType);
         
         if (nextRealWizard_ == null)
            // it's important that the creation is after setting the replay type to REAL
            nextRealWizard_ = new RealScormTemplateWizardPanel(pwData_);
         
         pwData_.nextWizardPanel = nextRealWizard_;
      }
      else if (windowsExportButton_.isSelected())
      {
         // Windows Media Technologies export
         iExportType = PublisherWizardData.EXPORT_TYPE_WMT;
         pwData_.GetProfiledSettings().SetIntValue("iExportType", iExportType);

         if (nextWindowsWizard_ == null)
            nextWindowsWizard_ = new WmpScormTemplateWizardPanel(pwData_);

         pwData_.nextWizardPanel = nextWindowsWizard_;
      }
      else if (flashExportButton_.isSelected())
      {
         // Flash export
         iExportType = PublisherWizardData.EXPORT_TYPE_FLASH;
         pwData_.GetProfiledSettings().SetIntValue("iExportType", iExportType);

         if (nextFlashWizard_ == null)
            // Note:
            // FlashScormTemplateWizardPanel and FlashSettingsWizardPanel changed in order of appearance
            //nextFlashWizard_ = new FlashScormTemplateWizardPanel(pwData_);
            nextFlashWizard_ = new FlashSettingsWizardPanel(pwData_);
         
         pwData_.nextWizardPanel = nextFlashWizard_;   
      }
      else if (lecturnityExportButton_.isSelected())
      {
         // LPD export
         iExportType = PublisherWizardData.EXPORT_TYPE_LECTURNITY;
         pwData_.GetProfiledSettings().SetIntValue("iExportType", iExportType);

         if (nextLecturnityWizard_ == null)
            nextLecturnityWizard_ = new LecturnityBandwidthWizardPanel(pwData_);

         pwData_.nextWizardPanel = nextLecturnityWizard_;            
      }
      else // have a return in any case // if (videoExportButton_.isSelected())
      {
         // Video/MP4 export
         iExportType = PublisherWizardData.EXPORT_TYPE_VIDEO;
         pwData_.GetProfiledSettings().SetIntValue("iExportType", iExportType);
            
         if (nextVideoWizard_ == null)
            nextVideoWizard_ = new VideoSettingsWizardPanel(pwData_);
         
         pwData_.nextWizardPanel = nextVideoWizard_;   
      }

      /* Not yet desired (19.05.08);
      if (iExportType > -1 && !pwData_.m_bIsProfileMode)
         NativeUtils.setPublisherProperty("LastExportType", iExportType+"");
         */
      
      // If the export type has changed then the local target dir must be resetted
      if ((iLastExportType_ >= 0) && (iExportType != iLastExportType_))
         pwData_.GetProfiledSettings().SetStringValue("strTargetDirLocal", "");

      iLastExportType_ = iExportType;
      
      if (pwData_.m_bIsProfileMode)
         m_iLastProfileExportType = iExportType;

      return pwData_;
   }

   public void customCommand(String command)
   {
      ExtendedVideoDialog evd 
         = new ExtendedVideoDialog(getFrame(), this, pwData_);
      evd.show();
   }
}
