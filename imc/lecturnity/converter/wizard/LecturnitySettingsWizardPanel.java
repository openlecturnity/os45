package imc.lecturnity.converter.wizard;

import java.awt.*;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import javax.swing.ButtonGroup;
import javax.swing.JCheckBox;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JButton;
import javax.swing.SwingUtilities;
import javax.swing.border.TitledBorder;

import java.io.IOException;

import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.ui.PopupHelp;
import imc.epresenter.filesdk.util.Localizer;

public class LecturnitySettingsWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String THUMBNAILS = "[!]";
   private static char   MNEM_THUMBNAILS = '?';
   private static String THUMB_DESC = "[!]";
   private static String HELPDOC = "[!]";
   private static char   MNEM_HELPDOC = '?';
   private static String HELPDOC_DESC = "[!]";
   private static String EMBED = "[!]";
   private static char   MNEM_EMBED = '?';
   private static String EMBED_DESC = "[!]";

   private static String FULLSCREEN = "[!]";
   private static char   MNEM_FULLSCREEN = '?';
   private static String FULLSCREEN_DESC = "[!]";
   private static String CLIPSONSLIDES = "[!]";
   private static char   MNEM_CLIPSONSLIDES = '?';
   private static String CLIPSONSLIDES_DESC = "[!]";

   private static String CONVERT_OPTIONS = "[!]";
   private static String START_OPTIONS = "[!]";
   
   private static String STANDARD_NAVIGATION = "[!]";
   private static String STANDARD_NAVIGATION_SETTINGS = "[!]";
   private static String USER_DEFINED = "[!]";
   private static String BUTTON_CHANGE = "[!]";
   private static char   MNEM_BUTTON_CHANGE = '?';
   private static String DIALOG_TITLE = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/LecturnitySettingsWizardPanel_",
             "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");

         CONVERT_OPTIONS = l.getLocalized("CONVERT_OPTIONS");
         START_OPTIONS = l.getLocalized("START_OPTIONS");

         THUMBNAILS = l.getLocalized("THUMBNAILS");
         MNEM_THUMBNAILS = l.getLocalized("MNEM_THUMBNAILS").charAt(0);
         THUMB_DESC = l.getLocalized("THUMB_DESC");
         HELPDOC = l.getLocalized("HELPDOC");
         MNEM_HELPDOC = l.getLocalized("MNEM_HELPDOC").charAt(0);
         HELPDOC_DESC = l.getLocalized("HELPDOC_DESC");
         EMBED = l.getLocalized("EMBED");
         MNEM_EMBED = l.getLocalized("MNEM_EMBED").charAt(0);
         EMBED_DESC = l.getLocalized("EMBED_DESC");

         FULLSCREEN = l.getLocalized("FULLSCREEN");
         MNEM_FULLSCREEN = l.getLocalized("MNEM_FULLSCREEN").charAt(0);
         FULLSCREEN_DESC = l.getLocalized("FULLSCREEN_DESC");
         CLIPSONSLIDES = l.getLocalized("CLIPSONSLIDES");
         MNEM_CLIPSONSLIDES = l.getLocalized("MNEM_CLIPSONSLIDES").charAt(0);
         CLIPSONSLIDES_DESC = l.getLocalized("CLIPSONSLIDES_DESC");

         STANDARD_NAVIGATION = l.getLocalized("STANDARD_NAVIGATION");
         STANDARD_NAVIGATION_SETTINGS  = l.getLocalized("STANDARD_NAVIGATION_SETTINGS");
         USER_DEFINED = l.getLocalized("USER_DEFINED");
         BUTTON_CHANGE = l.getLocalized("BUTTON_CHANGE");
         MNEM_BUTTON_CHANGE = l.getLocalized("MNEM_BUTTON_CHANGE").charAt(0);
         DIALOG_TITLE = l.getLocalized("DIALOG_TITLE");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private PublisherWizardData pwData_;
   
   private TargetChoiceWizardPanel nextWizardPanel_;
   private LecturnityThumbnailWizardPanel nextThumbnailWizardPanel_;
   
   private JCheckBox useThumbnailsBox_;
///   private JCheckBox createHelpDocBox_;
   private JCheckBox embedFontsBox_;
   private JCheckBox fullScreenBox_;
   private JCheckBox clipsOnSlidesBox_;
   private JLabel lblStandardNavigationSettings_;

   private PopupHelp helpPopup_;

   public LecturnitySettingsWizardPanel(PublisherWizardData pwData)
   {
      super();
      
      pwData_ = pwData;

      setPreferredSize(new Dimension(500, 400));

      addButtons(NEXT_BACK_CANCEL);

      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");

      Container r = getContentPanel();
      r.setLayout(null);

      // Convert options panel
      JPanel convertOptionsPanel = new JPanel();
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         convertOptionsPanel.setBackground(Wizard.BG_COLOR);
      convertOptionsPanel.setLayout(null);
      convertOptionsPanel.setSize(440, 130); //(440, 95)
      convertOptionsPanel.setLocation(30, 20);
      convertOptionsPanel.setBorder(new TitledBorder(" " + CONVERT_OPTIONS + " "));
      r.add(convertOptionsPanel);
      

      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      // Thumbnails
      useThumbnailsBox_ = new WizardCheckBox(THUMBNAILS,
                                             new Point(20, 25),
                                             new Dimension(400, 20),
                                             descFont);
      useThumbnailsBox_.setMnemonic(MNEM_THUMBNAILS);
      new PopupHelp(useThumbnailsBox_,
                    "<html><font face=\"Arial; Helvetica\" size=\"-1\">" +
                    THUMB_DESC +
                    "</font></html>");
      convertOptionsPanel.add(useThumbnailsBox_);

      // Embed fonts
      embedFontsBox_ = new WizardCheckBox(EMBED,
                                          new Point(20, 55),
                                          new Dimension(400, 20),
                                          descFont);
      embedFontsBox_.setMnemonic(MNEM_EMBED);
      new PopupHelp(embedFontsBox_,
                    "<html><font face=\"Arial; Helvetica\" size=\"-1\">" +
                    EMBED_DESC +
                    "</font></html>");
      convertOptionsPanel.add(embedFontsBox_);

      // Standard Navigation
      JLabel lblStandardNavigation = new JLabel(STANDARD_NAVIGATION + ":");
      lblStandardNavigation.setSize(120, 20);
      lblStandardNavigation.setLocation(20, 95);
      convertOptionsPanel.add(lblStandardNavigation);
      
      lblStandardNavigationSettings_ = new JLabel(STANDARD_NAVIGATION_SETTINGS);
      lblStandardNavigationSettings_.setSize(210, 20);
      lblStandardNavigationSettings_.setLocation(140, 95);
      convertOptionsPanel.add(lblStandardNavigationSettings_);

      JButton navigationConfigureButton = new JButton(BUTTON_CHANGE) {
            public Insets getInsets() { return new Insets(1, 1, 1, 1); }
         };
      navigationConfigureButton.setSize(80, 28);
      navigationConfigureButton.setLocation(350, 90);
      navigationConfigureButton.setMnemonic(MNEM_BUTTON_CHANGE);
      navigationConfigureButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {
               LecturnitySettingsWizardPanel.this.standardNavigationConfigure();
            }
         });
      convertOptionsPanel.add(navigationConfigureButton);


      // Start options panel
      JPanel startOptionsPanel = new JPanel();
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         startOptionsPanel.setBackground(Wizard.BG_COLOR);
      startOptionsPanel.setLayout(null);
      startOptionsPanel.setSize(440, 95);
      startOptionsPanel.setLocation(30, 165); //(30, 135)
      startOptionsPanel.setBorder(new TitledBorder(" " + START_OPTIONS + " "));
      r.add(startOptionsPanel);

///      // Autostart document
///      createHelpDocBox_ = new WizardCheckBox(HELPDOC,
///                                             new Point(20, 25),
///                                             new Dimension(400, 20),
///                                             descFont);
///      createHelpDocBox_.setMnemonic(MNEM_HELPDOC);
///      new PopupHelp(createHelpDocBox_,
///                    "<html><font face=\"Arial; Helvetica\" size=\"-1\">" +
///                    HELPDOC_DESC +
///                    "</font></html>");
///      startOptionsPanel.add(createHelpDocBox_);

      // Start in full screen mode
      fullScreenBox_ = new WizardCheckBox(FULLSCREEN,
                                          new Point(20, 25),
                                          new Dimension(400, 20),
                                          descFont);
      fullScreenBox_.setMnemonic(MNEM_FULLSCREEN);
      new PopupHelp(fullScreenBox_,
                    "<html><font face=\"Arial; Helvetica\" size=\"-1\">" +
                    FULLSCREEN_DESC +
                    "</font></html>");
      startOptionsPanel.add(fullScreenBox_);

      // Display clips on slides
      clipsOnSlidesBox_ = new WizardCheckBox(CLIPSONSLIDES,
                                             new Point(20, 55),
                                             new Dimension(400, 20),
                                             descFont);
      clipsOnSlidesBox_.setMnemonic(MNEM_CLIPSONSLIDES);
      new PopupHelp(clipsOnSlidesBox_,
                    "<html><font face=\"Arial; Helvetica\" size=\"-1\">" +
                    CLIPSONSLIDES_DESC +
                    "</font></html>");
      startOptionsPanel.add(clipsOnSlidesBox_);

      String helpText = 
         "<html><font face=\"Arial; Helvetica\" size=\"-1\">" +
         "<b>" + THUMBNAILS + "</b><br>" +
         THUMB_DESC + "<br><br>" +
         "<b>" + EMBED + "</b><br>" +
         EMBED_DESC + "<br><br>" +
         "<b>" + HELPDOC + "</b><br>" +
         HELPDOC_DESC + "<br><br>" +
         "<b>" + FULLSCREEN + "</b><br>" +
         FULLSCREEN_DESC + "<br><br>" +
         "<b>" + CLIPSONSLIDES + "</b><br>" + 
         CLIPSONSLIDES_DESC +
         "</font></html>";


      if (getHelpButton() != null)
         helpPopup_ = new PopupHelp(getHelpButton(), helpText);
      
   }

   public String getDescription()
   {
      return "";
   }

   public void refreshDataDisplay()
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      useThumbnailsBox_.setSelected(ps.GetBoolValue("bLpdUseThumbnails") 
                                    && !pwData_.IsDenverDocument());
      useThumbnailsBox_.setEnabled(!pwData_.IsDenverDocument());
///      createHelpDocBox_.setSelected(ps.GetBoolValue("bDoAutostart"));
      embedFontsBox_.setSelected(ps.GetBoolValue("bLpdEmbedFonts") 
                                 && !pwData_.IsDenverDocument());
      embedFontsBox_.setEnabled(!pwData_.IsDenverDocument());
      fullScreenBox_.setSelected(ps.GetBoolValue("bLpdStartInFullScreen"));
      clipsOnSlidesBox_.setSelected(ps.GetBoolValue("bShowClipsOnSlides")
                                    && !pwData_.IsDenverDocument()
                                    && pwData_.HasMultipleVideos());
      clipsOnSlidesBox_.setEnabled(!pwData_.IsDenverDocument() && pwData_.HasMultipleVideos());
      
      if (ConverterWizard.m_bGenericProfilesEnabled && ps.GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN) 
      {   // generic profiles: all options enabled
          clipsOnSlidesBox_.setSelected(ps.GetBoolValue("bShowClipsOnSlides"));
          clipsOnSlidesBox_.setEnabled(true);          
      }

      lblStandardNavigationSettings_.setText(getStandardNavigationState());
      
      super.refreshDataDisplay();
   }

   protected void helpCommand()
   {
      helpPopup_.popupHelp();
   }

   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      ps.SetBoolValue("bLpdUseThumbnails", useThumbnailsBox_.isSelected());
///      ps.SetBoolValue("bDoAutostart", createHelpDocBox_.isSelected());
      ps.SetBoolValue("bLpdEmbedFonts", embedFontsBox_.isSelected());
      ps.SetBoolValue("bLpdStartInFullScreen", fullScreenBox_.isSelected());
      ps.SetBoolValue("bShowClipsOnSlides", clipsOnSlidesBox_.isSelected());

      if (ps.GetBoolValue("bLpdUseThumbnails"))
      {
         if (nextThumbnailWizardPanel_ == null)
            nextThumbnailWizardPanel_ = new LecturnityThumbnailWizardPanel(pwData_);

         pwData_.nextWizardPanel = nextThumbnailWizardPanel_;
      }
      else
      {
         if (nextWizardPanel_ == null)
            nextWizardPanel_ = new TargetChoiceWizardPanel(pwData_);
         
         pwData_.nextWizardPanel = nextWizardPanel_;
      }

      return pwData_;
   }

   private class WizardCheckBox extends JCheckBox
   {
      public WizardCheckBox(String text,
                            Point location,
                            Dimension size,
                            Font font)
      {
         super(text, false);

         if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
            setBackground(Wizard.BG_COLOR);
         setLocation(location);
         setSize(size);
         setFont(font);
      }
   }

   private void standardNavigationConfigure()
   {
      Window w = SwingUtilities.windowForComponent(this);
      Frame f = null;
      if (w instanceof Frame)
         f = (Frame)w;
 
      LecturnityExtendedDialog led = new LecturnityExtendedDialog(f, pwData_, DIALOG_TITLE);
      led.show(); // this is blocking

      //BUGFIX #2522: refresh is not necessary
      //refreshDataDisplay();
      lblStandardNavigationSettings_.setText(getStandardNavigationState());
   }
   
   private String getStandardNavigationState()
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      int nSumOfStates = ps.GetIntValue("iControlBarVisibility")
                         + ps.GetIntValue("iNavigationButtonsVisibility")
                         + ps.GetIntValue("iTimeLineVisibility")
                         + ps.GetIntValue("iTimeDisplayVisibility")
                         + ps.GetIntValue("iDocumentStructureVisibility")
                         + ps.GetIntValue("iPlayerSearchFieldVisibility")
                         + ps.GetIntValue("iPlayerConfigButtonsVisibility");

      if (nSumOfStates > 0)
         return USER_DEFINED;
      else
         return STANDARD_NAVIGATION_SETTINGS;
   }
}
