package imc.lecturnity.converter.wizard;

import javax.swing.*;
import javax.swing.border.*;

import java.awt.*;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.NativeUtils;
import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.ImagePanel;

abstract class ScormTemplateWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String HEADER_FLASH = "[!]";
   private static String SUBHEADER = "[!]";
   private static String SUBHEADER_FLASH = "[!]";
   private static String GROUP_WEB_TEMPLATE = "[!]";

   private static String TEMPLATE_TITLE = "[!]";
   private static String TEMPLATE_SCORM_COMPATIBLE = "[!]";
   private static String TEMPLATE_AUTHOR = "[!]";
   private static String TEMPLATE_COPYRIGHT = "[!]";

   protected static String BUTTON_CHANGE = "[!]";
   protected static char MNEM_BUTTON_CHANGE = '?';
   protected static char MNEM_BUTTON_CHANGE2 = '?';

   private static String SCORM_SUPPORT = "[!]";
   private static String GROUP_SCORM = "[!]";
   private static String SCORM_STRICT = "[!]";
   private static String SCORM_DEFAULT_LANGUAGE = "[!]";
   private static String SCORM_LANGUAGE = "[!]";
   private static String SCORM_VERSION = "[!]";
   private static String SCORM_STATUS = "[!]";
   private static String SCORM_FULLTEXT = "[!]";

   private static String SCORM_STRICT_WARNING = "[!]";
   private static String SCORM_REAL_STRICT_WARNING = "[!]";

   private static String WARNING = "[!]";
   private static String UTF_WARNING = "[!]";

   private static String STANDARD_NAVIGATION = "[!]";
   private static String STANDARD_NAVIGATION_SETTINGS = "[!]";
   private static String USER_DEFINED = "[!]";

   private static String YES = "[!]";
   private static String NO    = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer("/imc/lecturnity/converter/wizard/ScormTemplateWizardPanel_",
                                     "en");
         HEADER = l.getLocalized("HEADER");
         HEADER_FLASH = l.getLocalized("HEADER_FLASH");
         SUBHEADER = l.getLocalized("SUBHEADER");
         SUBHEADER_FLASH = l.getLocalized("SUBHEADER_FLASH");
         GROUP_WEB_TEMPLATE = l.getLocalized("GROUP_WEB_TEMPLATE");
         TEMPLATE_TITLE = l.getLocalized("TEMPLATE_TITLE");
         TEMPLATE_SCORM_COMPATIBLE = l.getLocalized("TEMPLATE_SCORM_COMPATIBLE");
         TEMPLATE_AUTHOR = l.getLocalized("TEMPLATE_AUTHOR");
         TEMPLATE_COPYRIGHT = l.getLocalized("TEMPLATE_COPYRIGHT");
         BUTTON_CHANGE = l.getLocalized("BUTTON_CHANGE");
         MNEM_BUTTON_CHANGE = l.getLocalized("MNEM_BUTTON_CHANGE").charAt(0);
         MNEM_BUTTON_CHANGE2 = l.getLocalized("MNEM_BUTTON_CHANGE2").charAt(0);
         SCORM_SUPPORT = l.getLocalized("SCORM_SUPPORT");
         GROUP_SCORM = l.getLocalized("GROUP_SCORM");
         SCORM_STRICT = l.getLocalized("SCORM_STRICT");
         SCORM_DEFAULT_LANGUAGE = l.getLocalized("SCORM_DEFAULT_LANGUAGE");
         SCORM_LANGUAGE = l.getLocalized("SCORM_LANGUAGE");
         SCORM_VERSION = l.getLocalized("SCORM_VERSION");
         SCORM_STATUS = l.getLocalized("SCORM_STATUS");
         SCORM_FULLTEXT = l.getLocalized("SCORM_FULLTEXT");
         SCORM_STRICT_WARNING = l.getLocalized("SCORM_STRICT_WARNING");
         SCORM_REAL_STRICT_WARNING = l.getLocalized("SCORM_REAL_STRICT_WARNING");
         STANDARD_NAVIGATION = l.getLocalized("STANDARD_NAVIGATION");
         STANDARD_NAVIGATION_SETTINGS = l.getLocalized("STANDARD_NAVIGATION_SETTINGS");
         USER_DEFINED = l.getLocalized("USER_DEFINED");
         YES = l.getLocalized("YES");
         NO = l.getLocalized("NO");
         WARNING = l.getLocalized("WARNING");
         UTF_WARNING = l.getLocalized("UTF_WARNING");
      }
      catch (Exception e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(null, "Access to Locale database [ScormTemplateWizardPanel] failed!\n" +
                                       e.toString(), "Error", 
                                       JOptionPane.ERROR_MESSAGE);
      }
   }

   protected PublisherWizardData m_PublisherData;
   
   protected WizardTextArea templateTitle_;
   protected WizardTextArea templateScormComp_;
   protected WizardTextArea templateAuthor_;
   protected WizardTextArea templateCopyright_;
   protected WizardTextArea templateStandardNavigation_;

   protected JButton templateConfigureButton_;

   protected JCheckBox scormSupportBox_;
   protected JPanel scormPanel_;
   protected JCheckBox scormStrictBox_;
   protected JLabel scormLanguageLabel_;
   protected WizardTextArea scormLanguage_;
   protected JLabel scormVersionLabel_;
   protected WizardTextArea scormVersion_;
   protected JLabel scormStatusLabel_;
   protected WizardTextArea scormStatus_;
   protected JLabel scormFullTextLabel_;
   protected WizardTextArea scormFullText_;

   protected JButton scormConfigureButton_;

   protected JPanel warningPanel_;

   protected abstract String getHeaderImage();

   public ScormTemplateWizardPanel(PublisherWizardData pwData)
   {
      this(pwData, false);
   }

   public ScormTemplateWizardPanel(PublisherWizardData pwData, boolean forFlash)
   {
      super();

      m_PublisherData = pwData;
      
      ProfiledSettings ps = m_PublisherData.GetProfiledSettings();

      if (!forFlash)
      {
         if (m_PublisherData.m_bIsProfileMode)
         {
            String strTemplatePath = ps.GetStringValue("strRmWmTemplateConfigFilePath");
            String strTemplateSettings = ps.GetStringValue("strRmWmTemplateConfigParameterList");
            
            if (strTemplatePath != null && strTemplatePath.length() > 0)
               LPLibs.templateReadSettingsFromFile(strTemplatePath);
            else
               LPLibs.templateReadSettings(); // for a new profile
            
            if (strTemplateSettings != null && strTemplateSettings.length() > 0)
               LPLibs.templateUseSettings(strTemplateSettings);
            
         }
         else
            LPLibs.templateReadSettings();
      }
      
      if (m_PublisherData.m_bIsProfileMode)
      {
         String strScormSettings = ps.GetStringValue("strScormSettings");
         
         if (strScormSettings != null && strScormSettings.length() > 0)
            LPLibs.scormUseSettings(strScormSettings);
      }
      

      setPreferredSize(new Dimension(500, 400));

      addButtons(NEXT_BACK_CANCEL);

      useHeaderPanel(HEADER, SUBHEADER, 
                     getHeaderImage());

      Container p = getContentPanel();
      p.setLayout(null);

      JPanel templatePanel = new JPanel();
      templatePanel.setLayout(null);
      templatePanel.setSize(460, 105); //(460, 90)
      templatePanel.setLocation(20, 15);
      templatePanel.setBorder(new TitledBorder(GROUP_WEB_TEMPLATE));
      if (!forFlash)
         p.add(templatePanel);

      JLabel tmpLabel = new JLabel(TEMPLATE_TITLE);
      tmpLabel.setSize(150, 15);
      tmpLabel.setLocation(20, 20);
      templatePanel.add(tmpLabel);

      tmpLabel = new JLabel(TEMPLATE_SCORM_COMPATIBLE);
      tmpLabel.setSize(150, 15);
      tmpLabel.setLocation(20, 35);
      templatePanel.add(tmpLabel);
      
      tmpLabel = new JLabel(TEMPLATE_AUTHOR);
      tmpLabel.setSize(150, 15);
      tmpLabel.setLocation(20, 50);
      templatePanel.add(tmpLabel);

      tmpLabel = new JLabel(TEMPLATE_COPYRIGHT);
      tmpLabel.setSize(150, 15);
      tmpLabel.setLocation(20, 65);
      templatePanel.add(tmpLabel);

      tmpLabel = new JLabel(STANDARD_NAVIGATION);
      tmpLabel.setSize(150, 15);
      tmpLabel.setLocation(20, 83);
      templatePanel.add(tmpLabel);

      Font font = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      templateTitle_ = new WizardTextArea("LECTURNITY WebPlayer 1", new Point(170, 20), new Dimension(300, 16), font);
      templatePanel.add(templateTitle_);
      templateScormComp_ = new WizardTextArea("Ja", new Point(170, 35), new Dimension(200, 16), font);
      templatePanel.add(templateScormComp_);
      templateAuthor_ = new WizardTextArea("imc AG", new Point(170, 50), new Dimension(200, 16), font);
      templatePanel.add(templateAuthor_);
      templateCopyright_ = new WizardTextArea("(c) 2004 imc AG", new Point(170, 65), new Dimension(200, 16), font);
      templatePanel.add(templateCopyright_);
      templateStandardNavigation_ = new WizardTextArea("Standard-Navigation", new Point(170, 83), new Dimension(200, 16), font);
      templatePanel.add(templateStandardNavigation_);

      templateConfigureButton_ = new JButton(BUTTON_CHANGE) {
            public Insets getInsets() { return new Insets(1, 1, 1, 1); }
         };
      templateConfigureButton_.setSize(80, 28);
      templateConfigureButton_.setLocation(460-80-10, 90-28-10);
      templateConfigureButton_.setMnemonic(MNEM_BUTTON_CHANGE);
      templateConfigureButton_.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
               ScormTemplateWizardPanel.this.templateConfigure();
            }
         });
      templatePanel.add(templateConfigureButton_);

      int scormOffset = 128;

      scormSupportBox_ = new JCheckBox(SCORM_SUPPORT);
      scormSupportBox_.setSize(460, 19);
      scormSupportBox_.setLocation(25, scormOffset);
      scormSupportBox_.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
               ScormTemplateWizardPanel.this.toggleScormSupport();
            }
         });
      p.add(scormSupportBox_);

      scormPanel_ = new JPanel();
      scormPanel_.setLayout(null);
      scormPanel_.setSize(460, 108);
      scormPanel_.setLocation(20, scormOffset + 20);
      scormPanel_.setBorder(new TitledBorder(GROUP_SCORM));
      p.add(scormPanel_);

      scormStrictBox_ = new JCheckBox(SCORM_STRICT);
      scormStrictBox_.setSize(420, 19);
      scormStrictBox_.setLocation(20, 20);
      scormStrictBox_.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
               ScormTemplateWizardPanel.this.toggleStrictScormSupport();
            }
         });
      scormPanel_.add(scormStrictBox_);

      // "Full text as Metadata" (SCORM_FULLTEXT) is a very long string in vietnamese
      int dW = 0;
      if (NativeUtils.getLanguageCode() == "vi")
         dW = 80;
      scormLanguageLabel_ = new JLabel(SCORM_LANGUAGE);
      scormLanguageLabel_.setSize(150+dW, 15);
      scormLanguageLabel_.setLocation(20, 40);
      scormPanel_.add(scormLanguageLabel_);

      scormVersionLabel_ = new JLabel(SCORM_VERSION);
      scormVersionLabel_.setSize(150+dW, 15);
      scormVersionLabel_.setLocation(20, 55);
      scormPanel_.add(scormVersionLabel_);

      scormStatusLabel_ = new JLabel(SCORM_STATUS);
      scormStatusLabel_.setSize(150+dW, 15);
      scormStatusLabel_.setLocation(20, 70);
      scormPanel_.add(scormStatusLabel_);

      scormFullTextLabel_ = new JLabel(SCORM_FULLTEXT);
      scormFullTextLabel_.setSize(150+dW, 15);
      scormFullTextLabel_.setLocation(20, 85);
      scormPanel_.add(scormFullTextLabel_);

      scormLanguage_ = new WizardTextArea("German (de)", new Point(170+dW, 38), new Dimension(200, 16), font);
      scormPanel_.add(scormLanguage_);
      scormVersion_ = new WizardTextArea("1.3", new Point(170+dW, 53), new Dimension(200, 16), font);
      scormPanel_.add(scormVersion_);
      scormStatus_ = new WizardTextArea("Final", new Point(170+dW, 68), new Dimension(200, 16), font);
      scormPanel_.add(scormStatus_);
      scormFullText_ = new WizardTextArea("Ja", new Point(170+dW, 83), new Dimension(200, 16), font);
      scormPanel_.add(scormFullText_);

      scormConfigureButton_ = new JButton(BUTTON_CHANGE) {
            public Insets getInsets() { return new Insets(1, 1, 1, 1); }
         };
      scormConfigureButton_.setSize(80, 28);
      scormConfigureButton_.setLocation(460-80-10, 110-28-25);
      scormConfigureButton_.setMnemonic(MNEM_BUTTON_CHANGE2);
      scormConfigureButton_.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
               ScormTemplateWizardPanel.this.scormConfigure();
            }
         });
      scormPanel_.add(scormConfigureButton_);

      warningPanel_ = new JPanel();
      warningPanel_.setLayout(null);
      warningPanel_.setSize(450, 32);
      warningPanel_.setLocation(20, 255);
      ImagePanel imagePanel = new ImagePanel("/imc/lecturnity/converter/images/warning.gif");
      imagePanel.setLayout(null);
      imagePanel.setSize(31, 32);
      imagePanel.setLocation(0, 0);
      warningPanel_.add(imagePanel);
      String warningAreaText = SCORM_STRICT_WARNING;
      if (m_PublisherData.GetProfiledSettings().GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_REAL)
         warningAreaText = SCORM_REAL_STRICT_WARNING;
      WizardTextArea warningText = new WizardTextArea(warningAreaText, 
                                                      new Point(40, 0),
                                                      new Dimension(410, 32),
                                                      font);
      warningPanel_.add(warningText);
      warningPanel_.setVisible(false);
      p.add(warningPanel_);
   }

   public void refreshDataDisplay()
   {
      super.refreshDataDisplay();

      ProfiledSettings ps = m_PublisherData.GetProfiledSettings();

      if (this instanceof FlashScormTemplateWizardPanel)
      {
         scormSupportBox_.setSelected(ps.GetBoolValue("bScormEnabled"));
         scormSupportBox_.setEnabled(true);
      }
      else
      {
         scormSupportBox_.setSelected(ps.GetBoolValue("bScormEnabled") && LPLibs.templateIsScormCompliant());
         scormSupportBox_.setEnabled(LPLibs.templateIsScormCompliant());
      }
      scormStrictBox_.setSelected(ps.GetBoolValue("bScormStrict"));

      toggleScormSupport();

      refreshModuleData();
   }

   private void refreshModuleData()
   {
      if (!(this instanceof FlashScormTemplateWizardPanel)) // Bugfix 4339
      {
         templateTitle_.setText(LPLibs.templateGetTitle());
         templateScormComp_.setText(LPLibs.templateIsScormCompliant() ? YES : NO);
         templateAuthor_.setText(LPLibs.templateGetAuthor());
         templateCopyright_.setText(LPLibs.templateGetCopyright());
         templateStandardNavigation_.setText(getStandardNavigationState());
      }

      scormLanguage_.setText(LPLibs.scormGetLanguage());
      if (LPLibs.scormGetLanguage().length() == 0)
         scormLanguage_.setText(SCORM_DEFAULT_LANGUAGE);
      scormVersion_.setText(LPLibs.scormGetVersion());
      scormStatus_.setText(LPLibs.scormGetStatus());
      scormFullText_.setText(LPLibs.scormGetFullTextAsKeywords() ? YES : NO);
   }

   protected void checkUtfTemplate()
   {
      if (!LPLibs.templateIsUtfTemplate())
      {
         System.out.println("checkUtfTemplate: Template is not UTF-8 compliant");
         JOptionPane.showMessageDialog(getFrame(), UTF_WARNING, WARNING, JOptionPane.WARNING_MESSAGE);
      }
      else
      {
         System.out.println("checkUtfTemplate: OK: Template is UTF-8 compliant");
      }
   }

   public int displayWizard()
   {
      checkUtfTemplate();
      
      refreshDataDisplay();

      return super.displayWizard();
   }

   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = m_PublisherData.GetProfiledSettings();

      ps.SetBoolValue("bScormEnabled", scormSupportBox_.isSelected());
      ps.SetBoolValue("bScormStrict", scormStrictBox_.isSelected());

      ps.SetStringValue("strScormSettings", LPLibs.scormGetSettings());

      return m_PublisherData;
   }

   public String getDescription()
   {
      return "ScormTemplate";
   }

   private void templateConfigure()
   {
      LPLibs.templateConfigure(getFrame(), m_PublisherData.m_bIsProfileMode);
      refreshModuleData();
      
      // Do we need to output a warning if the template is
      // not an UTF-8 template?
      checkUtfTemplate();

      // SCORM enabled?
      boolean bScormCompliant = LPLibs.templateIsScormCompliant();
      if (bScormCompliant)
      {
         scormSupportBox_.setEnabled(true);
      }
      else
      {
         scormSupportBox_.setSelected(false);
         scormSupportBox_.setEnabled(false);
         toggleScormSupport();
      }
   }

   private void scormConfigure()
   {
      LPLibs.scormConfigure(getFrame(), m_PublisherData.m_bIsProfileMode);
      refreshModuleData();
   }

   protected void toggleScormSupport()
   {
      boolean bScorm = scormSupportBox_.isSelected();
      if (!scormSupportBox_.isEnabled())
         bScorm = false;
      scormPanel_.setEnabled(bScorm);
      scormStrictBox_.setEnabled(bScorm);
      scormLanguageLabel_.setEnabled(bScorm);
      scormLanguage_.setEnabled(bScorm);
      scormVersionLabel_.setEnabled(bScorm);
      scormVersion_.setEnabled(bScorm);
      scormStatusLabel_.setEnabled(bScorm);
      scormStatus_.setEnabled(bScorm);
      scormFullTextLabel_.setEnabled(bScorm);
      scormFullText_.setEnabled(bScorm);
      scormConfigureButton_.setEnabled(bScorm);

      toggleStrictScormSupport();
   }

   private void toggleStrictScormSupport()
   {
      boolean bWarn = scormStrictBox_.isSelected() && scormSupportBox_.isSelected();
      warningPanel_.setVisible(bWarn);
   }
   
   private String getStandardNavigationState()
   {
      // 8 'standard navigation' states are used in LPLibs
      int[] aiStandardNavigationStates = LPLibs.templateGetStandardNavigationStates();
      int nSumOfStates = 0;

      ProfiledSettings ps = m_PublisherData.GetProfiledSettings();

      ps.SetIntValue("iControlBarVisibility", aiStandardNavigationStates[0]);
      ps.SetIntValue("iNavigationButtonsVisibility", aiStandardNavigationStates[1]);
      ps.SetIntValue("iTimeLineVisibility", aiStandardNavigationStates[2]);
      ps.SetIntValue("iTimeDisplayVisibility", aiStandardNavigationStates[3]);
      ps.SetIntValue("iDocumentStructureVisibility", aiStandardNavigationStates[4]);
      ps.SetIntValue("iPluginContextMenuVisibility", aiStandardNavigationStates[7]);
      System.out.print("* Navigation states: ");
      for (int i = 0; i < 8; ++i)
         System.out.print(aiStandardNavigationStates[i] + " ");
      System.out.println("");

      nSumOfStates = ps.GetIntValue("iControlBarVisibility") 
                     + ps.GetIntValue("iNavigationButtonsVisibility") 
                     + ps.GetIntValue("iTimeLineVisibility") 
                     + ps.GetIntValue("iTimeDisplayVisibility") 
                     + ps.GetIntValue("iDocumentStructureVisibility") 
                     + ps.GetIntValue("iPluginContextMenuVisibility");

      if (nSumOfStates > 0)
         return USER_DEFINED;
      else
         return STANDARD_NAVIGATION_SETTINGS;
   }
}
