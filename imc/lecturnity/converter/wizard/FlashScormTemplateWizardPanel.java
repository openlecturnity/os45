package imc.lecturnity.converter.wizard;

import javax.swing.*;
import javax.swing.border.*;

import java.awt.*;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.wizards.*;
import imc.epresenter.filesdk.util.Localizer;

class FlashScormTemplateWizardPanel extends ScormTemplateWizardPanel
{
   private WizardPanel nextWizard_;

   private static String GROUP_TEMPLATE = "[!]";
   private static String LABEL_LOGO = "[!]";
   private static String LABEL_LOGO_URL = "[!]";
   private static String NO_LOGO = "[!]";
   private static String LABEL_BASIC_BG_COLOR = "[!]";
   private static String LABEL_BG_COLOR = "[!]";
   private static String LABEL_SHOWTITLE = "[!]";
   private static String LABEL_TEXT_COLOR = "[!]";
   private static String LABEL_NO_TEMPLATE = "[!]";
   private static String LABEL_WEBPLAYER_LOGO = "[!]";
   private static String LABEL_PAGE_SIZE_SETTING = "[!]";
   private static String LABEL_BROWSER_WINDOW_SIZE = "[!]";
   private static String VISIBLE = "[!]";
   private static String DISABLED = "[!]";
   private static String ORIG_SIZE = "[!]";
   private static String FLEXIBLE = "[!]";
   private static String OPTIMIZED = "[!]";
   private static String STANDARD = "[!]";
   private static String YES = "[!]";
   private static String NO = "[!]";
   private static String DIALOG_TITLE = "[!]";
   private static String STANDARD_NAVIGATION = "[!]";
   private static String STANDARD_NAVIGATION_SETTINGS = "[!]";
   private static String USER_DEFINED = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer("/imc/lecturnity/converter/wizard/FlashScormTemplateWizardPanel_",
                                     "en");
         GROUP_TEMPLATE = l.getLocalized("GROUP_TEMPLATE");
         LABEL_LOGO = l.getLocalized("LABEL_LOGO");
         LABEL_LOGO_URL = l.getLocalized("LABEL_LOGO_URL");
         NO_LOGO = l.getLocalized("NO_LOGO");
         LABEL_BASIC_BG_COLOR = l.getLocalized("LABEL_BASIC_BG_COLOR");
         LABEL_BG_COLOR = l.getLocalized("LABEL_BG_COLOR");
         LABEL_SHOWTITLE = l.getLocalized("LABEL_SHOWTITLE");
         LABEL_TEXT_COLOR = l.getLocalized("LABEL_TEXT_COLOR");
         LABEL_NO_TEMPLATE = l.getLocalized("LABEL_NO_TEMPLATE");
         LABEL_WEBPLAYER_LOGO = l.getLocalized("LABEL_WEBPLAYER_LOGO");
         LABEL_PAGE_SIZE_SETTING = l.getLocalized("LABEL_PAGE_SIZE_SETTING");
         LABEL_BROWSER_WINDOW_SIZE = l.getLocalized("LABEL_BROWSER_WINDOW_SIZE");
         VISIBLE = l.getLocalized("VISIBLE");
         DISABLED = l.getLocalized("DISABLED");
         ORIG_SIZE = l.getLocalized("ORIG_SIZE");
         FLEXIBLE = l.getLocalized("FLEXIBLE");
         OPTIMIZED = l.getLocalized("OPTIMIZED");
         STANDARD = l.getLocalized("STANDARD");
         YES = l.getLocalized("YES");
         NO = l.getLocalized("NO");
         DIALOG_TITLE = l.getLocalized("DIALOG_TITLE");
         STANDARD_NAVIGATION = l.getLocalized("STANDARD_NAVIGATION");
         STANDARD_NAVIGATION_SETTINGS = l.getLocalized("STANDARD_NAVIGATION_SETTINGS");
         USER_DEFINED = l.getLocalized("USER_DEFINED");
      }
      catch (Exception e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(null, "Access to Locale database [FlashScormTemplateWizardPanel] failed!\n" +
                                       e.toString(), "Error", 
                                       JOptionPane.ERROR_MESSAGE);
      }
   }
   
   private JLabel logoLabel_;
   private JLabel logoUrlLabel_;
   private JLabel bgColorLabel_;
   private JLabel showTitleLabel_;
   private JLabel textColorLabel_;
   private JLabel navigationLabel_;
   private JLabel noTemplateLabel_;
   private JLabel webPlayerLogo_;
   private JLabel pageDisplay_;
   private JLabel optimizeBrowser_;
   private WizardTextArea templateLogo_;
   private WizardTextArea templateLogoUrl_;
   private WizardColorArea templateBasicColor_;
   private WizardColorArea templateBgColor_;
   private WizardTextArea templateShowTitle_;
   private WizardColorArea templateTextColor_;
   private WizardTextArea standardNavigation_;
   private WizardTextArea templateWebPlayer_;
   private WizardTextArea templatePage_;
   private WizardTextArea templateBrowser_;

   public FlashScormTemplateWizardPanel(PublisherWizardData pwData)
   {
      super(pwData, true);

      Container p = getContentPanel();

      JPanel templatePanel = new JPanel();
      templatePanel.setLayout(null);
///      templatePanel.setSize(460, 130); //90
      templatePanel.setSize(460, 118);
///      templatePanel.setLocation(20, 15);
      templatePanel.setLocation(20, 5);
      templatePanel.setBorder(new TitledBorder(GROUP_TEMPLATE));
      p.add(templatePanel);

      logoLabel_ = new JLabel(LABEL_LOGO);
      logoLabel_.setSize(150, 15);
      logoLabel_.setLocation(20, 20);
      templatePanel.add(logoLabel_);

      logoUrlLabel_ = new JLabel(LABEL_LOGO_URL);
      logoUrlLabel_.setSize(150, 15);
      logoUrlLabel_.setLocation(20, 35);
      templatePanel.add(logoUrlLabel_);

      bgColorLabel_ = new JLabel(LABEL_BASIC_BG_COLOR);
      bgColorLabel_.setSize(150, 15);
      bgColorLabel_.setLocation(20, 50); //35
      templatePanel.add(bgColorLabel_);
      
      showTitleLabel_ = new JLabel(LABEL_SHOWTITLE);
      showTitleLabel_.setSize(150, 15);
      showTitleLabel_.setLocation(20, 65); //50
      templatePanel.add(showTitleLabel_);

      textColorLabel_ = new JLabel(LABEL_TEXT_COLOR);
      textColorLabel_.setSize(150, 15);
      textColorLabel_.setLocation(20, 80); //65
      templatePanel.add(textColorLabel_);

      noTemplateLabel_ = new JLabel(LABEL_NO_TEMPLATE);
      noTemplateLabel_.setSize(200, 15);
      noTemplateLabel_.setLocation(20, 30);
      noTemplateLabel_.setEnabled(false);
      templatePanel.add(noTemplateLabel_);
      
      webPlayerLogo_ = new JLabel(LABEL_WEBPLAYER_LOGO);
      webPlayerLogo_.setSize(200, 15);
      webPlayerLogo_.setLocation(20, 30);
      webPlayerLogo_.setEnabled(false);
      templatePanel.add(webPlayerLogo_);
      
      pageDisplay_ = new JLabel(LABEL_PAGE_SIZE_SETTING);
      pageDisplay_.setSize(200, 15);
      pageDisplay_.setLocation(20, 45);
      pageDisplay_.setEnabled(false);
      templatePanel.add(pageDisplay_);
      
      optimizeBrowser_ = new JLabel(LABEL_BROWSER_WINDOW_SIZE);
      optimizeBrowser_.setSize(200, 15);
      optimizeBrowser_.setLocation(20, 60);
      optimizeBrowser_.setEnabled(false);
      templatePanel.add(optimizeBrowser_);
      
      Font font = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      templateLogo_ = new WizardTextArea(NO_LOGO, new Point(170, 20), new Dimension(300, 16), font);
      templatePanel.add(templateLogo_);
      
      templateLogoUrl_ = new WizardTextArea("", new Point(170, 35), new Dimension(300, 16), font);
      templatePanel.add(templateLogoUrl_);
      
      templateShowTitle_ = new WizardTextArea(YES, new Point(170, 65), new Dimension(200, 16), font); //49
      templatePanel.add(templateShowTitle_);

      templateBasicColor_ = new WizardColorArea(new Point(170, 50), new Dimension(40, 14), 
         new Color(m_PublisherData.GetProfiledSettings().GetIntValue("iFlashColorBasic")));
      templatePanel.add(templateBasicColor_);

      templateBgColor_ = new WizardColorArea(new Point(230, 50), new Dimension(40, 14), 
         new Color(m_PublisherData.GetProfiledSettings().GetIntValue("iFlashColorBackground")));
      templatePanel.add(templateBgColor_);

      templateTextColor_ = new WizardColorArea(new Point(170, 80), new Dimension(40, 14), 
         new Color(m_PublisherData.GetProfiledSettings().GetIntValue("iFlashColorText")));
      templatePanel.add(templateTextColor_);
      
      templateWebPlayer_ = new WizardTextArea(VISIBLE, new Point(170, 30), new Dimension(200, 16), font);
      templatePanel.add(templateWebPlayer_);

      templatePage_ = new WizardTextArea(ORIG_SIZE, new Point(170, 45), new Dimension(200, 16), font);
      templatePanel.add(templatePage_);

      templateBrowser_ = new WizardTextArea(OPTIMIZED, new Point(170, 60), new Dimension(200, 16), font);
      templatePanel.add(templateBrowser_);


      JButton templateConfigureButton = new JButton(BUTTON_CHANGE) {
            public Insets getInsets() { return new Insets(1, 1, 1, 1); }
         };
      templateConfigureButton.setSize(80, 28);
      templateConfigureButton.setLocation(460-80-10, 90-28-10+15); //90-28-10
      templateConfigureButton.setMnemonic(MNEM_BUTTON_CHANGE);
      templateConfigureButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
               FlashScormTemplateWizardPanel.this.templateConfigure();
            }
         });
      templatePanel.add(templateConfigureButton);
      
      
      navigationLabel_ = new JLabel(STANDARD_NAVIGATION);
      navigationLabel_.setSize(150, 15);
///      navigationLabel_.setLocation(20, 105);
      navigationLabel_.setLocation(20, 95);
      templatePanel.add(navigationLabel_);

///      standardNavigation_ = new WizardTextArea(getStandardNavigationState(),
///                                   new Point(170, 103), new Dimension(300, 16), font);
      standardNavigation_ = new WizardTextArea(getStandardNavigationState(),
                                   new Point(170, 95), new Dimension(300, 16), font);
      templatePanel.add(standardNavigation_);

      refreshDataDisplay();
   }

   protected String getHeaderImage()
   {
      return "/imc/lecturnity/converter/images/flash_logo.gif";
   }

   public PublisherWizardData getWizardData()
   {
      PublisherWizardData pwData = super.getWizardData();

      // Note:
      // FlashScormTemplateWizardPanel and FlashSettingsWizardPanel changed in order of appearance
      if (nextWizard_ == null)
         nextWizard_ = new TargetChoiceWizardPanel(pwData);

      pwData.nextWizardPanel = nextWizard_;

      return pwData;
   }

   protected void checkUtfTemplate()
   {
      // Overwrite method from ScormTemplateWizard and do nothing
      return;
   }

   public void refreshDataDisplay()
   {
      super.refreshDataDisplay();
      
      ProfiledSettings ps = m_PublisherData.GetProfiledSettings();

      if (ps.GetStringValue("strLogoImageName").length() > 0 && ps.GetBoolValue("bFlashDisplayLogo"))
         templateLogo_.setText(ps.GetStringValue("strLogoImageName"));
      else
         templateLogo_.setText(NO_LOGO);

      if (ps.GetStringValue("strLogoImageUrl").length() > 0 && ps.GetBoolValue("bFlashDisplayLogo"))
         templateLogoUrl_.setText(ps.GetStringValue("strLogoImageUrl"));
      else
         templateLogoUrl_.setText("");

      templateBasicColor_.setColor(new Color(ps.GetIntValue("iFlashColorBasic")));
      templateBgColor_.setColor(new Color(ps.GetIntValue("iFlashColorBackground")));

      templateShowTitle_.setText(ps.GetBoolValue("bFlashDisplayTitle") ? YES : NO);
      if (ps.GetBoolValue("bFlashDisplayTitle"))
      {
         templateTextColor_.setColor(new Color(ps.GetIntValue("iFlashColorText")));
         templateTextColor_.setVisible(true);
         //textColorLabel_.setVisible(true);
      }
      else
      {
         templateTextColor_.setVisible(false);
         //textColorLabel_.setVisible(false);
      }

      
      if (ps.GetBoolValue("bHideLecturnityIdentity") == true)
         templateWebPlayer_.setText(DISABLED);
      else
         templateWebPlayer_.setText(VISIBLE);
      
      if (ps.GetIntValue("iSlideSizeType") == PublisherWizardData.SLIDESIZE_ORIGINAL)
          templatePage_.setText(ORIG_SIZE);
       else
          templatePage_.setText(FLEXIBLE);
       
      if (ps.GetBoolValue("bFlashStartInOptimizedSize") == true)
          templateBrowser_.setText(OPTIMIZED);
       else
          templateBrowser_.setText(STANDARD);
       
      
      // PSP design (or SLIDESTAR): Template can not be changed (different from normal design)
      boolean bTemplateIsChangeable = true;
      boolean bIsNewFlashPlayer = (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX);
      if ( (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR) 
         || (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_PSP) )
          bTemplateIsChangeable = false;

      if (bTemplateIsChangeable)
      {
         if (bIsNewFlashPlayer) {
            logoLabel_.setEnabled(false);
            logoUrlLabel_.setEnabled(false);
            bgColorLabel_.setEnabled(false);
            showTitleLabel_.setEnabled(false);
            textColorLabel_.setEnabled(false);
            logoLabel_.setVisible(false);
            logoUrlLabel_.setVisible(false);
            bgColorLabel_.setVisible(false);
            showTitleLabel_.setVisible(false);
            textColorLabel_.setVisible(false);
            noTemplateLabel_.setVisible(false);
            
            webPlayerLogo_.setEnabled(true);
            pageDisplay_.setEnabled(true);
            optimizeBrowser_.setEnabled(true);
            webPlayerLogo_.setVisible(true);
            pageDisplay_.setVisible(true);
            optimizeBrowser_.setVisible(true);
            templateWebPlayer_.setVisible(true);
            templatePage_.setVisible(true);
            templateBrowser_.setVisible(true);

            templateLogo_.setVisible(false);
            templateLogoUrl_.setVisible(false);
            templateShowTitle_.setVisible(false);
            templateBasicColor_.setVisible(false);
            templateBgColor_.setVisible(false);
            templateTextColor_.setVisible(false);

            navigationLabel_.setLocation(20, 85);
            standardNavigation_.setLocation(170, 85);
         } else {
            logoLabel_.setEnabled(true);
            logoUrlLabel_.setEnabled(true);
            bgColorLabel_.setEnabled(true);
            showTitleLabel_.setEnabled(true);
            textColorLabel_.setEnabled(true);
            logoLabel_.setVisible(true);
            logoUrlLabel_.setVisible(true);
            bgColorLabel_.setVisible(true);
            showTitleLabel_.setVisible(true);
            textColorLabel_.setVisible(true);
            noTemplateLabel_.setVisible(false);
            
            webPlayerLogo_.setVisible(false);
            pageDisplay_.setVisible(false);
            optimizeBrowser_.setVisible(false);
            templateWebPlayer_.setVisible(false);
            templatePage_.setVisible(false);
            templateBrowser_.setVisible(false);

            templateLogo_.setVisible(true);
            templateLogoUrl_.setVisible(true);
            templateShowTitle_.setVisible(true);
            templateBasicColor_.setVisible(true);
            templateBgColor_.setVisible(true);
            templateTextColor_.setVisible(true);

            navigationLabel_.setLocation(20, 95);
            standardNavigation_.setLocation(170, 95);
         }
      }
      else
      {
         logoLabel_.setEnabled(false);
         logoUrlLabel_.setEnabled(false);
         bgColorLabel_.setEnabled(false);
         showTitleLabel_.setEnabled(false);
         textColorLabel_.setEnabled(false);
         logoLabel_.setVisible(false);
         logoUrlLabel_.setVisible(false);
         bgColorLabel_.setVisible(false);
         showTitleLabel_.setVisible(false);
         textColorLabel_.setVisible(false);
         noTemplateLabel_.setVisible(true);
         
         webPlayerLogo_.setVisible(false);
         pageDisplay_.setVisible(false);
         optimizeBrowser_.setVisible(false);
         templateWebPlayer_.setVisible(false);
         templatePage_.setVisible(false);
         templateBrowser_.setVisible(false);

         templateLogo_.setVisible(false);
         templateLogoUrl_.setVisible(false);
         templateShowTitle_.setVisible(false);
         templateBasicColor_.setVisible(false);
         templateBgColor_.setVisible(false);
         templateTextColor_.setVisible(false);

         navigationLabel_.setLocation(20, 74);
         standardNavigation_.setLocation(170, 74);
      }

      standardNavigation_.setText(getStandardNavigationState());

//      //The next line is outcommented - because it resets the Std.Navigation settings
//      super.refreshDataDisplay();

      scormSupportBox_.setSelected(ps.GetBoolValue("bScormEnabled"));

      boolean bScormEnabled = true;
      bScormEnabled = (ps.GetIntValue("iFlashTemplateType") != PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR);
      scormSupportBox_.setEnabled(bScormEnabled);

      toggleScormSupport();
   }

   public void templateConfigure()
   {
      Window w = SwingUtilities.windowForComponent(this);
      Frame f = null;
      if (w instanceof Frame)
         f = (Frame)w;
 
      FlashExtendedDialog fed = new FlashExtendedDialog(f, m_PublisherData, DIALOG_TITLE);
      fed.show(); // this is blocking

      refreshDataDisplay();
   }
   
   private String getStandardNavigationState()
   {
      int nSumOfStates = 0;

      ProfiledSettings ps = m_PublisherData.GetProfiledSettings();

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

   class WizardColorArea extends JLabel
   {
      private Dimension mySize_ = null;

      public WizardColorArea(Point point, Dimension dim, Color color)
      {
         super();
         setLocation(point);
         setSize(dim);
         setBackground(color);
         setOpaque(true);
      }

      public void setSize(Dimension dim)
      {
         super.setSize(dim);

         mySize_ = (Dimension) dim.clone();
      }

      public void setColor(Color color)
      {
         setBackground(color);
      }

      public void paint(Graphics g)
      {
         super.paint(g);
         
         g.setColor(Color.black);
         g.drawRect(0, 0, mySize_.width-1, mySize_.height-1);
      }
   }
}