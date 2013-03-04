package imc.lecturnity.converter.wizard;

import imc.lecturnity.converter.StreamingMediaConverter;
import imc.lecturnity.converter.ProfiledSettings;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.ui.PopupHelp;

import imc.epresenter.filesdk.util.Localizer;

import java.io.IOException;

import javax.swing.*;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Container;
import java.awt.Point;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

abstract class ExportWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   
   private static String RADIO_FILE = "[!]";
   private static String RADIO_FILE_HTTP = "[!]";
   private static char   MNEM_RADIO_FILE = '?';
   private static String RADIO_HTTP = "[!]";
   private static char   MNEM_RADIO_HTTP = '?';

   protected static String RADIO_SERVER = "[!]";
   protected static char   MNEM_RADIO_SERVER = '?';

   private static String ENABLE_AUTOSTART = "[!]";
   
   private static String DESC_FILE = "[!]";
   private static String DESC_FILE_HTTP = "[!]";
   private static String DESC_HTTP = "[!]";
   protected static String DESC_SERVER = "[!]";

   private static String DESC_FILE_SHORT = "[!]";
   private static String DESC_FILE_SHORT_HTTP = "[!]";
   private static String DESC_HTTP_SHORT = "[!]";
   protected static String DESC_SERVER_SHORT = "[!]";

   private static String HELP_TEXT = "[!]";

   // private static String TEMPLATE = "[!]";
   // private static char MNEM_TEMPLATE = '?';

   private static String ERR_NO_PARSER = "[!]";
   
   protected static String ICON_RESOURCE = 
      "/imc/lecturnity/converter/images/real_networks_logo.gif";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/ExportWizardPanel_",
             "en");
         
         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");

         RADIO_FILE = l.getLocalized("RADIO_FILE");
         RADIO_FILE_HTTP = l.getLocalized("RADIO_FILE_HTTP");
         MNEM_RADIO_FILE = l.getLocalized("MNEM_RADIO_FILE").charAt(0);
         RADIO_HTTP = l.getLocalized("RADIO_HTTP");
         MNEM_RADIO_HTTP = l.getLocalized("MNEM_RADIO_HTTP").charAt(0);

         ENABLE_AUTOSTART = l.getLocalized("ENABLE_AUTOSTART");
         
         DESC_FILE = l.getLocalized("DESC_FILE");
         DESC_FILE_HTTP = l.getLocalized("DESC_FILE_HTTP");
         DESC_HTTP = l.getLocalized("DESC_HTTP");

         DESC_FILE_SHORT = l.getLocalized("DESC_FILE_SHORT");
         DESC_FILE_SHORT_HTTP = l.getLocalized("DESC_FILE_SHORT_HTTP");
         DESC_HTTP_SHORT = l.getLocalized("DESC_HTTP_SHORT");

         ERR_NO_PARSER = l.getLocalized("ERR_NO_PARSER");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database: ExportWizardPanel\n" +
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private PublisherWizardData pwData_;

//    private WizardPanel nextTargetWizard_;
//    private WizardPanel nextHttpWizard_;
//    private WizardPanel nextRealWizard_;

   // swing components
   private JRadioButton fileRadio_;
   private JRadioButton httpRadio_;
   private JRadioButton serverRadio_;
   protected JCheckBox  autostartCheck_;

   private PopupHelp      helpPopup_;
   private PopupHelp      fileHelp_;
   private WizardTextArea fileDesc_;
   private WizardTextArea httpDesc_;
   private WizardTextArea serverDesc_;
      

   public ExportWizardPanel(PublisherWizardData pwData)
   {
      super();

      pwData_ = pwData;
     
      useHeaderPanel(HEADER, SUBHEADER, ICON_RESOURCE);
      setPreferredSize(new Dimension(500, 400));
      addButtons(NEXT_BACK_CANCEL | BUTTON_HELP); // | BUTTON_CUSTOM, TEMPLATE, MNEM_TEMPLATE);

      Container r = getContentPanel();
      r.setLayout(null);
      
      
      // some variable declarations to ease placement changes (below)
      int w = 440;
      int w1 = 60;
      int w2 = w-w1-40;
      int x1 = 40;
      int ch = 20;
      int ah = 40;

      // 20, 105, 190 (440x20)@30
      fileRadio_ = new JRadioButton(RADIO_FILE, true);
      fileRadio_.setSize(w, ch);
      fileRadio_.setLocation(x1, 30);
      fileRadio_.setMnemonic(MNEM_RADIO_FILE);
      fileRadio_.setFont(HEADER_FONT);
      fileHelp_ = new PopupHelp(fileRadio_, "<html><font face=\"Arial; Helvetica\" size=\"-1\">" + 
                    DESC_FILE + "</font></html>");
      r.add(fileRadio_);

      fileDesc_ = new WizardTextArea
         (DESC_FILE_SHORT, new Point(x1+20, 50), new Dimension(w-30, ah),
          imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      r.add(fileDesc_);

      httpRadio_ = new JRadioButton(RADIO_HTTP, false);
      httpRadio_.setSize(w, ch);
      httpRadio_.setLocation(x1, 100);
      httpRadio_.setMnemonic(MNEM_RADIO_HTTP);
      httpRadio_.setFont(HEADER_FONT);
      new PopupHelp(httpRadio_, "<html><font face=\"Arial; Helvetica\" size=\"-1\">" + 
                    DESC_HTTP + "</font></html>");
      r.add(httpRadio_);
         
      httpDesc_ = new WizardTextArea
      (DESC_HTTP_SHORT, new Point(x1+20, 120), new Dimension(w-30, ah),
       imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      r.add(httpDesc_);
      
      /*
      JLabel lblUrl = new JLabel("URL: ");
      lblUrl.setSize(w1, ch);
      lblUrl.setLocation(x1+20, 140);
      JTextField txtUrl = new JTextField();
      txtUrl.setSize(w2, ch);
      txtUrl.setLocation(x1+20+w1+10, 140);
      
      r.add(lblUrl);
      r.add(txtUrl);
      */

      serverRadio_ = new JRadioButton(RADIO_SERVER, false);
      serverRadio_.setSize(w, ch);
      serverRadio_.setLocation(x1, 170);
      serverRadio_.setMnemonic(MNEM_RADIO_SERVER);
      serverRadio_.setFont(HEADER_FONT);
      new PopupHelp(serverRadio_, "<html><font face=\"Arial; Helvetica\" size=\"-1\">" + 
                    DESC_SERVER + "</font></html>");
      r.add(serverRadio_);

      serverDesc_ = new WizardTextArea
         (DESC_SERVER_SHORT, new Point(x1+20, 190), new Dimension(w-30, ah),
          imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      r.add(serverDesc_);
      
      /*
      JLabel lblUrl2 = new JLabel("RTSP: ");
      lblUrl2.setSize(w1, ch);
      lblUrl2.setLocation(x1+20, 235);
      JTextField txtUrl2 = new JTextField();
      txtUrl2.setSize(w2, ch);
      txtUrl2.setLocation(x1+20+w1+10, 235);
      
      r.add(lblUrl2);
      r.add(txtUrl2);
      */
      
      /*
      JLabel lblUrl3 = new JLabel("RTSP: ");
      lblUrl3.setSize(w1, ch);
      lblUrl3.setLocation(x1+20, 265);
      JTextField txtUrl3 = new JTextField();
      txtUrl3.setSize(w2, ch);
      txtUrl3.setLocation(x1+20+w1+10, 265);
      
      r.add(lblUrl3);
      r.add(txtUrl3);
      */

      ButtonGroup buttonGroup = new ButtonGroup();
      buttonGroup.add(fileRadio_);
      buttonGroup.add(httpRadio_);
      buttonGroup.add(serverRadio_);

      autostartCheck_ = new JCheckBox(ENABLE_AUTOSTART, pwData_.GetProfiledSettings().GetBoolValue("bDoAutostart"));
      autostartCheck_.setSize(w, ch);
      autostartCheck_.setLocation(x1, 260);
      r.add(autostartCheck_);
      
      HELP_TEXT = "<html><font face=\"Arial; Helvetica\" size=\"-1\">" +
         "<b>" + RADIO_FILE + "</b><br>" + DESC_FILE + "<br><br>" +
         "<b>" + RADIO_HTTP + "</b><br>" + DESC_HTTP + "<br><br>" +
         "<b>" + RADIO_SERVER + "</b><br>" + DESC_SERVER + "<br><br>" +
         "</font></html>";

      // help button popup thingies
      helpPopup_ = new PopupHelp(getHelpButton(), HELP_TEXT);
   }

   protected void helpCommand()
   {
      helpPopup_.popupHelp();
   }

   public String getDescription()
   {
      return "";
   }

   public void refreshDataDisplay()
   {
      boolean enableServer = false;
      boolean bHasClips = false;
      boolean bSmilFile = false;
      boolean bDisableFile = false;

      ProfiledSettings ps = pwData_.GetProfiledSettings();

      if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH)
      {
         autostartCheck_.setVisible(false);
         if (pwData_.HasNormalVideo() || pwData_.HasClips())
            enableServer = true;
      }
      else
         enableServer = !(ps.GetBoolValue("bScormEnabled") && ps.GetBoolValue("bScormStrict"));

      if (!enableServer)
         fileRadio_.setSelected(true);
      serverRadio_.setEnabled(enableServer);
      serverDesc_.setEnabled(enableServer);

      bHasClips = pwData_.HasClips();
      bSmilFile = (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_REAL)
         && bHasClips && ps.GetBoolValue("bShowClipsOnSlides");

      bDisableFile = ps.GetBoolValue("bScormEnabled") 
         && !ps.GetBoolValue("bScormStrict") && bSmilFile;
      
      
      // everytime you have a SMIL file
      boolean bEnableHttp = bSmilFile;
      
      httpRadio_.setVisible(bEnableHttp);
      httpDesc_.setVisible(bEnableHttp);
      
      // SCORM AND SMIL together require a server of some kind
      fileRadio_.setVisible(!bDisableFile);
      fileDesc_.setVisible(!bDisableFile);
      
      httpRadio_.setSelected(!fileRadio_.isVisible() && !serverRadio_.isSelected());
      fileRadio_.setSelected(!httpRadio_.isVisible() && !serverRadio_.isSelected());
      
      autostartCheck_.setSelected(pwData_.GetProfiledSettings().GetBoolValue("bDoAutostart"));
     
      try
      {
         Localizer l = new Localizer(
            "/imc/lecturnity/converter/wizard/ExportWizardPanel_", "en");
      
         RADIO_FILE = l.getLocalized("RADIO_FILE");
         DESC_FILE = l.getLocalized("DESC_FILE");
         DESC_FILE_SHORT = l.getLocalized("DESC_FILE_SHORT");
      
         if (!bEnableHttp)
         {
            RADIO_FILE += "/"+RADIO_FILE_HTTP;
            DESC_FILE += ", "+DESC_FILE_HTTP;
            DESC_FILE_SHORT += ", "+DESC_FILE_SHORT_HTTP;
         }
         else
         {
            DESC_FILE += ".";
            DESC_FILE_SHORT += ".";
         }
     
         fileRadio_.setText(RADIO_FILE);
         fileHelp_.setText("<html><font face=\"Arial; Helvetica\" size=\"-1\">" + 
                           DESC_FILE + "</font></html>");
         fileDesc_.setText(DESC_FILE_SHORT);
         
         
         HELP_TEXT = "<html><font face=\"Arial; Helvetica\" size=\"-1\">" +
            "<b>" + RADIO_FILE + "</b><br>" + DESC_FILE + "<br><br>" +
            (bEnableHttp ? ("<b>" + RADIO_HTTP + "</b><br>" + DESC_HTTP + "<br><br>") : "") +
            "<b>" + RADIO_SERVER + "</b><br>" + DESC_SERVER + "<br><br>" +
            "</font></html>";
      
         helpPopup_.setText(HELP_TEXT);
      }
      catch (IOException exc)
      {
         // do nothing; shouldn't happen (see above)
      }

      super.refreshDataDisplay();
   }

   public abstract PublisherWizardData getWizardData();

   protected int getSelectedType()
   {
      if (fileRadio_.isSelected())
         return PublisherWizardData.REPLAY_LOCAL;
      else if (httpRadio_.isSelected())
         return PublisherWizardData.REPLAY_HTTP;
      else if (serverRadio_.isSelected())
         return PublisherWizardData.REPLAY_SERVER;
      else
         return PublisherWizardData.REPLAY_LOCAL;
   }

   public int displayWizard()
   {
      return super.displayWizard();
   }

}
