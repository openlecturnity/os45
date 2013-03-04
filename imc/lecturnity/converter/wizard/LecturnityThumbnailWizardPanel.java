package imc.lecturnity.converter.wizard;

import java.awt.Dimension;
import java.awt.Container;
import java.awt.Point;
import java.awt.Font;

import javax.swing.JPanel;
import javax.swing.JCheckBox;
import javax.swing.JOptionPane;

import javax.swing.border.TitledBorder;

import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.wizards.*;
import imc.epresenter.filesdk.util.Localizer;

public class LecturnityThumbnailWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String DESCRIPTION = "[!]";
   private static String[] THUMBSIZES = {"[!]", "[!]", "[!]"};
   private static char[]   MNEM_THUMBSIZES = {'?', '?', '?'};
   private static String BORDERTHUMB = "[!]";
   private static String ICON_RESOURCE = 
      "/imc/lecturnity/converter/images/lecturnity_logo.gif";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/LecturnityThumbnailWizardPanel_",
             "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         DESCRIPTION = l.getLocalized("DESCRIPTION");
         BORDERTHUMB = l.getLocalized("BORDERTHUMB");
         for (int i=0; i<3; i++)
         {
            THUMBSIZES[i] = l.getLocalized("THUMBSIZES" + i);
            MNEM_THUMBSIZES[i] = l.getLocalized("MNEM_THUMBSIZES" + i).charAt(0);
         }
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "LecturnityThumbnailWizardPanel\n" + 
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         System.exit(1);
      }
   }

   private PublisherWizardData pwData_;

   private WizardPanel nextWizardPanel_;

   private boolean isNextEnabled_ = true;
   private boolean[] m_bThumbnailsSizes = { false, false, false};


   private JCheckBox[] thumbSizeCheckBox_;

   public LecturnityThumbnailWizardPanel(PublisherWizardData pwData)
   {
      super();

      setPreferredSize(new Dimension(500, 400));
      
      pwData_ = pwData;

      addButtons(NEXT_BACK_CANCEL);
      
      useHeaderPanel(HEADER, SUBHEADER,
                     ICON_RESOURCE);

      ProfiledSettings ps = pwData_.GetProfiledSettings();

      m_bThumbnailsSizes[0] = ps.GetBoolValue("bLpdThumbnailsSizeSmall");
      m_bThumbnailsSizes[1] = ps.GetBoolValue("bLpdThumbnailsSizeMedium"); 
      m_bThumbnailsSizes[2] = ps.GetBoolValue("bLpdThumbnailsSizeLarge");

      Container r = getContentPanel();
      r.setLayout(null);

      r.add(new WizardTextArea(DESCRIPTION,
                               new Point(30, 16),
                               new Dimension(440, 60),
                               imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12)));

      JPanel thumbnailSizePanel = new JPanel();
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         thumbnailSizePanel.setBackground(Wizard.BG_COLOR);
      thumbnailSizePanel.setLayout(null);
      thumbnailSizePanel.setSize(260, 120);
      thumbnailSizePanel.setLocation(120, 100);
      thumbnailSizePanel.setBorder(new TitledBorder(" " + BORDERTHUMB + " "));
      r.add(thumbnailSizePanel);

      ThumbnailBoxListener tbl = new ThumbnailBoxListener();

      thumbSizeCheckBox_ = new JCheckBox[3];

      for (int i = 0; i < 3; i++)
      {
         thumbSizeCheckBox_[i] = new JCheckBox(THUMBSIZES[i], 
                                               m_bThumbnailsSizes[i]);
         if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
            thumbSizeCheckBox_[i].setBackground(Wizard.BG_COLOR);
         thumbSizeCheckBox_[i].setSize(220, 18);
         thumbSizeCheckBox_[i].setLocation(20, 25 + i*25);
         thumbSizeCheckBox_[i].setMnemonic(MNEM_THUMBSIZES[i]);
         thumbSizeCheckBox_[i].addChangeListener(tbl);
         thumbnailSizePanel.add(thumbSizeCheckBox_[i]);
      }
   }

   public void refreshDataDisplay()
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      thumbSizeCheckBox_[0].setSelected(ps.GetBoolValue("bLpdThumbnailsSizeSmall")); 
      thumbSizeCheckBox_[1].setSelected(ps.GetBoolValue("bLpdThumbnailsSizeMedium")); 
      thumbSizeCheckBox_[2].setSelected(ps.GetBoolValue("bLpdThumbnailsSizeLarge"));

      super.refreshDataDisplay();
   }

   public String getDescription()
   {
      return "";
   }

   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      ps.SetBoolValue("bLpdThumbnailsSizeSmall", thumbSizeCheckBox_[0].isSelected()); 
      ps.SetBoolValue("bLpdThumbnailsSizeMedium", thumbSizeCheckBox_[1].isSelected()); 
      ps.SetBoolValue("bLpdThumbnailsSizeLarge", thumbSizeCheckBox_[2].isSelected());

      if (nextWizardPanel_ == null)
         nextWizardPanel_ = new TargetChoiceWizardPanel(pwData_);
      
      pwData_.nextWizardPanel = nextWizardPanel_;
      
      return pwData_;
   }

   private class ThumbnailBoxListener implements ChangeListener
   {
      public void stateChanged(ChangeEvent e)
      {
         boolean isOneOn = false;
         for (int i = 0; i < 3; i++)
            isOneOn = isOneOn || thumbSizeCheckBox_[i].isSelected();
         
         if (isOneOn != isNextEnabled_)
         {
            setEnableButton(BUTTON_NEXT, isOneOn);
            isNextEnabled_ = isOneOn;
         }
      }
   }
}

