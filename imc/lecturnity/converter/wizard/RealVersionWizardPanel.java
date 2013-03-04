package imc.lecturnity.converter.wizard;

import java.awt.Dimension;
import java.awt.Container;
import java.awt.Point;
import java.awt.Font;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import javax.swing.*;
import javax.swing.border.TitledBorder;
import javax.swing.border.BevelBorder;

import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

import java.io.IOException;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.converter.ProfiledSettings;
import imc.epresenter.filesdk.util.Localizer;

public class RealVersionWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String DESC = "[!]";
   private static String PLAYER = "[!]";
   private static String SERVER = "[!]";
   private static String EXTENT = "[!]";
   private static String V8 = "[!]";
   private static String V7 = "[!]";
   private static String V6 = "[!]";

   private static char   MNEM_V8_PLY = '?';
   private static char   MNEM_V7_PLY = '?';
   private static char   MNEM_V6_PLY = '?';
   private static char   MNEM_V8_SRV = '?';
   private static char   MNEM_V7_SRV = '?';
   private static char   MNEM_V6_SRV = '?';

   static
   {
      try
      {
         Localizer l = new Localizer("/imc/lecturnity/converter/wizard" +
                           "/RealVersionWizardPanel_", "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         V6 = l.getLocalized("V6");
         V7 = l.getLocalized("V7");
         V8 = l.getLocalized("V8");
         SERVER = l.getLocalized("SERVER");
         PLAYER = l.getLocalized("PLAYER");
         EXTENT = l.getLocalized("EXTENT");
         DESC = l.getLocalized("DESC");

         MNEM_V8_PLY = l.getLocalized("MNEM_V8_PLY").charAt(0);
         MNEM_V7_PLY = l.getLocalized("MNEM_V7_PLY").charAt(0);
         MNEM_V6_PLY = l.getLocalized("MNEM_V6_PLY").charAt(0);
         MNEM_V8_SRV = l.getLocalized("MNEM_V8_SRV").charAt(0);
         MNEM_V7_SRV = l.getLocalized("MNEM_V7_SRV").charAt(0);
         MNEM_V6_SRV = l.getLocalized("MNEM_V6_SRV").charAt(0);
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Access to Locale database failed!", "Error", 
                                       JOptionPane.ERROR_MESSAGE);
      }
   }

   private PublisherWizardData pwData_;

   private WizardPanel nextWizard_ = null;

   private JCheckBox playerV8Box_;
   private JCheckBox playerV7Box_;
   private JCheckBox playerV6Box_;

   private JCheckBox serverV8Box_;
   private JCheckBox serverV7Box_;
   private JCheckBox serverV6Box_;

   private int minVersion_ = 8;

   public RealVersionWizardPanel(PublisherWizardData pwData)
   {
      super();
      
      pwData_ = pwData;

      ProfiledSettings ps = pwData_.GetProfiledSettings();

      setPreferredSize(new Dimension(500, 400));

      addButtons(NEXT_BACK_CANCEL);
      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/real_networks_logo.gif");

      Container r = getContentPanel();
      r.setLayout(null);

      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      ActionListener checkListener = new CheckListener();

      WizardTextArea desc = new WizardTextArea
         (DESC, new Point(30, 15), new Dimension(440, 34), descFont);
      r.add(desc);

      JPanel playerPanel = new JPanel();
      playerPanel.setLayout(null);
      playerPanel.setBorder(new TitledBorder(" " + PLAYER + " "));
      playerPanel.setSize(440, 105); //(210, 105);
      playerPanel.setLocation(30, 57);

      JPanel serverPanel = new JPanel();
      serverPanel.setLayout(null);
      serverPanel.setBorder(new TitledBorder(" " + SERVER + " "));
      serverPanel.setSize(440, 105); //(210, 105);
      serverPanel.setLocation(30, 165);
      
      playerV8Box_ = new JCheckBox(V8, true);
      playerV8Box_.setLocation(40, 20); //(18, 20);
      playerV8Box_.setSize(180, 22);
      playerV8Box_.setMnemonic(MNEM_V8_PLY);
      playerV8Box_.addActionListener(checkListener);
      playerV8Box_.setSelected(ps.GetBoolValue("bRealPlayer8"));
      playerPanel.add(playerV8Box_);
      playerV7Box_ = new JCheckBox(V7, false);
      playerV7Box_.setLocation(40, 45); //(18, 45);
      playerV7Box_.setSize(130, 22);
      playerV7Box_.setMnemonic(MNEM_V7_PLY);
      playerV7Box_.addActionListener(checkListener);
      playerV7Box_.setSelected(ps.GetBoolValue("bRealPlayer7"));
      playerPanel.add(playerV7Box_);
      playerV6Box_ = new JCheckBox(V6, false);
      playerV6Box_.setLocation(40, 70); //(18, 70);
      playerV6Box_.setSize(130, 22);
      playerV6Box_.setMnemonic(MNEM_V6_PLY);
      playerV6Box_.addActionListener(checkListener);
      playerV6Box_.setSelected(ps.GetBoolValue("bRealPlayer6"));
      playerPanel.add(playerV6Box_);

      r.add(playerPanel);

      serverV8Box_ = new JCheckBox(V8, true);
      serverV8Box_.setLocation(40, 20); //(18, 20);
      serverV8Box_.setSize(180, 22);
      serverV8Box_.setMnemonic(MNEM_V8_SRV);
      serverV8Box_.addActionListener(checkListener);
      serverV8Box_.setSelected(ps.GetBoolValue("bRealServer8"));
      serverPanel.add(serverV8Box_);
      serverV7Box_ = new JCheckBox(V7, false);
      serverV7Box_.setLocation(40, 45); //(18, 45);
      serverV7Box_.setSize(130, 22);
      serverV7Box_.setMnemonic(MNEM_V7_SRV);
      serverV7Box_.addActionListener(checkListener);
      serverV7Box_.setSelected(ps.GetBoolValue("bRealServer7"));
      serverPanel.add(serverV7Box_);
      serverV6Box_ = new JCheckBox(V6, false);
      serverV6Box_.setLocation(40, 70); //(18, 70);
      serverV6Box_.setSize(130, 22);
      serverV6Box_.setMnemonic(MNEM_V6_SRV);
      serverV6Box_.addActionListener(checkListener);
      serverV6Box_.setSelected(ps.GetBoolValue("bRealServer6"));
      serverPanel.add(serverV6Box_);

      r.add(serverPanel);
   }

   public String getDescription()
   {
      return "";
   }

   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      ps.SetIntValue("iRealMinimalVersion", minVersion_);

      ps.SetBoolValue("bRealPlayer8", playerV8Box_.isSelected());
      ps.SetBoolValue("bRealPlayer7", playerV7Box_.isSelected());
      ps.SetBoolValue("bRealPlayer6", playerV6Box_.isSelected());
      ps.SetBoolValue("bRealServer8", serverV8Box_.isSelected());
      ps.SetBoolValue("bRealServer7", serverV7Box_.isSelected());
      ps.SetBoolValue("bRealServer6", serverV6Box_.isSelected());

      if (nextWizard_ == null)
         nextWizard_ = new TargetChoiceWizardPanel(pwData_);
///         nextWizard_ = new RealExportWizardPanel(pwData_);

      pwData_.nextWizardPanel = nextWizard_;

      return pwData_;
   }

   public void refreshDataDisplay()
   {
      checkLabels();
   }

   private void checkLabels()
   {
      boolean playerChecked = (playerV8Box_.isSelected() ||
                               playerV7Box_.isSelected() ||
                               playerV6Box_.isSelected());
      boolean serverChecked = (serverV8Box_.isSelected() ||
                               serverV7Box_.isSelected() ||
                               serverV6Box_.isSelected());
      
      if (playerChecked && serverChecked)
         setEnableButton(BUTTON_NEXT, true);
      else
      {
         setEnableButton(BUTTON_NEXT, false);
         return;
      }
      
      // now we know: some checkboxes are checked for player _and_ server
      minVersion_ = 8;
      if (playerV7Box_.isSelected() ||
          serverV7Box_.isSelected())
         minVersion_ = 7;
      if (playerV6Box_.isSelected() ||
          serverV6Box_.isSelected())
         minVersion_ = 6;
   }

   private class CheckListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         checkLabels();
      }
   }
}
