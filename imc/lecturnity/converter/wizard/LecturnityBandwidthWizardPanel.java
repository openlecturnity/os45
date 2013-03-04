package imc.lecturnity.converter.wizard;

import java.awt.Dimension;
import java.awt.Container;
import java.awt.Point;
import java.awt.Font;

import javax.swing.ButtonGroup;
import javax.swing.JRadioButton;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

import javax.swing.border.TitledBorder;

import java.io.IOException;

import imc.lecturnity.util.wizards.*;
import imc.epresenter.filesdk.util.Localizer;

public class LecturnityBandwidthWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String DESCRIPTION = "[!]";
   private static String BANDWIDTH = "[!]";
   private static String BW_256 = "[!]";
   private static char   MNEM_BW_256 = '?';
   private static String BW_048 = "[!]";
   private static char   MNEM_BW_048 = '?';

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/LecturnityBandwidthWizardPanel_",
             "en");
         
         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         DESCRIPTION = l.getLocalized("DESCRIPTION");
         BANDWIDTH = l.getLocalized("BANDWIDTH");
         BW_256 = l.getLocalized("BW_256");
         MNEM_BW_256 = l.getLocalized("MNEM_BW_256").charAt(0);
         BW_048 = l.getLocalized("BW_048");
         MNEM_BW_048 = l.getLocalized("MNEM_BW_048").charAt(0);
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private LecturnitySettingsWizardPanel nextWizardPanel_ = null;

   private PublisherWizardData pwData_;

   private JRadioButton bw256Radio_;
   private JRadioButton bw048Radio_;

   public LecturnityBandwidthWizardPanel(PublisherWizardData pwData)
   {
      super();
      
      pwData_ = pwData;

      setPreferredSize(new Dimension(500, 400));

      addButtons(NEXT_BACK_CANCEL);

      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");

      Container r = getContentPanel();
      r.setLayout(null);

      r.add(new WizardTextArea(DESCRIPTION, new Point(30, 20),
                               new Dimension(440, 80),
                               imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12)));

      JPanel bwPanel = new JPanel();
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         bwPanel.setBackground(Wizard.BG_COLOR);
      bwPanel.setLayout(null);
      bwPanel.setSize(260, 100);
      bwPanel.setLocation(120, 100);
      bwPanel.setBorder(new TitledBorder(" " + BANDWIDTH + " "));
      r.add(bwPanel);

      bw048Radio_ = new JRadioButton(BW_048, true);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         bw048Radio_.setBackground(Wizard.BG_COLOR);
      bw048Radio_.setLocation(30, 30);
      bw048Radio_.setSize(200, 20);
      bw048Radio_.setMnemonic(MNEM_BW_048);
      bw048Radio_.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      bwPanel.add(bw048Radio_);

      bw256Radio_ = new JRadioButton(BW_256, false);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         bw256Radio_.setBackground(Wizard.BG_COLOR);
      bw256Radio_.setLocation(30, 60);
      bw256Radio_.setSize(200, 20);
      bw256Radio_.setMnemonic(MNEM_BW_256);
      bw256Radio_.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      bwPanel.add(bw256Radio_);

      ButtonGroup bg = new ButtonGroup();
      bg.add(bw256Radio_);
      bg.add(bw048Radio_);
   }

   public String getDescription()
   {
      return "";
   }

   public void refreshDataDisplay()
   {
      if (pwData_.GetProfiledSettings().GetBoolValue("bLpdCompressAudio"))
      {
         bw256Radio_.setSelected(false);
         bw048Radio_.setSelected(true);
      }
      else
      {
         bw256Radio_.setSelected(true);
         bw048Radio_.setSelected(false);
      }

      super.refreshDataDisplay();
   }

   public PublisherWizardData getWizardData()
   {
      if (bw256Radio_.isSelected())
         pwData_.GetProfiledSettings().SetBoolValue("bLpdCompressAudio", false);
      else if (bw048Radio_.isSelected())
         pwData_.GetProfiledSettings().SetBoolValue("bLpdCompressAudio", true);

      if (nextWizardPanel_ == null)
      {
         nextWizardPanel_ = new LecturnitySettingsWizardPanel(pwData_);
      }

      pwData_.nextWizardPanel = nextWizardPanel_;

      return pwData_;
   }
}
