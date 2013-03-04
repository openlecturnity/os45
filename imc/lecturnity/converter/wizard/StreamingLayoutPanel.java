package imc.lecturnity.converter.wizard;

import java.awt.Dimension;
import java.awt.Container;
import java.awt.Point;
import java.awt.Font;
import java.awt.Color;
import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.Insets;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

import javax.swing.*;
import javax.swing.border.BevelBorder;
import javax.swing.border.TitledBorder;
import javax.swing.filechooser.FileFilter;

import javax.swing.JOptionPane;

import java.io.File;

import imc.lecturnity.util.wizards.*;

import imc.lecturnity.util.ui.PopupHelp;
import imc.lecturnity.util.ui.LazyTextArea;

import imc.epresenter.filesdk.util.Localizer;

class StreamingLayoutPanel extends WizardPanel
{
   private static String LAYOUT_CAPTION = "[!]";
   private static String SCALABLE_PAGE_LAYOUT = "[!]";
   private static String SCALABLE_PAGE_ANNOTATION = "[!]";
   private static char   MNEM_SCALABLE_PAGE_LAYOUT = '?';
   private static String TT_SCALABLE_PAGE_LAYOUT = "[!]";
   private static String TT_SCALABLE_PAGE_LAYOUT2 = "[!]";
   private static String CONSIDER_VIDEOSIZE = "[!]";
   private static char   MNEM_CONSIDER_VIDEOSIZE = '?';
   private static String TT_CONSIDER_VIDEOSIZE = "[!]";
//   private static String TT_CONSIDER_VIDEOSIZE2 = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/StreamingLayoutPanel_",
             "en");
         LAYOUT_CAPTION = l.getLocalized("LAYOUT_CAPTION");
         SCALABLE_PAGE_LAYOUT = l.getLocalized("SCALABLE_PAGE_LAYOUT");
         SCALABLE_PAGE_ANNOTATION = l.getLocalized("SCALABLE_PAGE_ANNOTATION");
         MNEM_SCALABLE_PAGE_LAYOUT = l.getLocalized("MNEM_SCALABLE_PAGE_LAYOUT").charAt(0);
         TT_SCALABLE_PAGE_LAYOUT = l.getLocalized("TT_SCALABLE_PAGE_LAYOUT");
         TT_SCALABLE_PAGE_LAYOUT2 = l.getLocalized("TT_SCALABLE_PAGE_LAYOUT2");
         CONSIDER_VIDEOSIZE = l.getLocalized("CONSIDER_VIDEOSIZE");
         MNEM_CONSIDER_VIDEOSIZE = l.getLocalized("MNEM_CONSIDER_VIDEOSIZE").charAt(0);
         TT_CONSIDER_VIDEOSIZE = l.getLocalized("TT_CONSIDER_VIDEOSIZE");
//         TT_CONSIDER_VIDEOSIZE2 = l.getLocalized("TT_CONSIDER_VIDEOSIZE2");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open StreamingLayoutPanel locale database!\n"+
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private StreamingConvertWizardData data_;

   private JCheckBox useScalingBox_;
   private JCheckBox useAdaptVideoBox_;
   private WizardTextArea annotationScalablePageLayout_;
   private WizardTextArea descScalablePageLayout_;
   private WizardTextArea descScalablePageLayout2_;
   private WizardTextArea descAdaptVideo_;
//   private WizardTextArea descAdaptVideo2_;

   private ExtendedSettingsDialog parent_ = null;
   
   public StreamingLayoutPanel(ExtendedSettingsDialog parent, StreamingConvertWizardData data)
   {
      super();
      
      parent_ = parent;
      data_ = data;
      
      initGui();
   }

//    public Insets getInsets()
//    { 
//       return new Insets(10, 10, 10, 10);
//    }
   
   public WizardData getWizardData()
   {
      data_.documentIsScalable = useScalingBox_.isSelected();
      data_.doConsiderVideosize = useAdaptVideoBox_.isSelected();

      return data_;
   }

   public String getDescription()
   {
      return "";
   }

   private void initGui()
   {
      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      ActionListener checkListener = new CheckListener();

      this.setLayout(new BorderLayout());
      JPanel extentPanel = new JPanel();
      extentPanel.setLayout(null);
///      extentPanel.setBorder(new TitledBorder(EXTENT_CAPTION));
      this.add(extentPanel, BorderLayout.CENTER);

      int y = 20;
      int s1 = 160;
      int s2 = 400;
      int x1 = 20;
      int x2 = 40;

      RadioListener rl = new RadioListener();

      useScalingBox_ = new JCheckBox(SCALABLE_PAGE_LAYOUT, data_.documentIsScalable);
      useScalingBox_.setFont(headerFont);
      useScalingBox_.setSize(s1, 22);
      useScalingBox_.setLocation(x1, y);
      useScalingBox_.addActionListener(checkListener);
//      useScalingBox_.setToolTipText(TT_SCALABLE_PAGE_LAYOUT);
      useScalingBox_.setMnemonic(MNEM_SCALABLE_PAGE_LAYOUT);
      extentPanel.add(useScalingBox_);

      annotationScalablePageLayout_ = new WizardTextArea
         (SCALABLE_PAGE_ANNOTATION, new Point(x2+s1, y+3), new Dimension(s2-s1, 22), descFont);
      extentPanel.add(annotationScalablePageLayout_);
      y += 25;
      descScalablePageLayout_ = new WizardTextArea
         (TT_SCALABLE_PAGE_LAYOUT, new Point(x2, y), new Dimension(s2, 35), descFont);
      extentPanel.add(descScalablePageLayout_);
      y += 35;
      descScalablePageLayout2_ = new WizardTextArea
         (TT_SCALABLE_PAGE_LAYOUT2, new Point(x2, y), new Dimension(s2, 35), descFont);
      extentPanel.add(descScalablePageLayout2_);

      y += 50;
      useAdaptVideoBox_ = new JCheckBox(CONSIDER_VIDEOSIZE, data_.doConsiderVideosize);
      useAdaptVideoBox_.setFont(headerFont);
      useAdaptVideoBox_.setSize(s2, 22);
      useAdaptVideoBox_.setLocation(x1, y);
      useAdaptVideoBox_.addActionListener(checkListener);
//      useAdaptVideoBox_.setToolTipText(TT_CONSIDER_VIDEOSIZE);
      useAdaptVideoBox_.setMnemonic(MNEM_CONSIDER_VIDEOSIZE);
      extentPanel.add(useAdaptVideoBox_);

      y += 25;
      descAdaptVideo_ = new WizardTextArea
         (TT_CONSIDER_VIDEOSIZE, new Point(x2, y), new Dimension(s2, 35), descFont);
      extentPanel.add(descAdaptVideo_);
      y += 35;
//      descAdaptVideo2_ = new WizardTextArea
//         (TT_CONSIDER_VIDEOSIZE2, new Point(x2, y), new Dimension(s2, 35), descFont);
//      extentPanel.add(descAdaptVideo2_);

      checkEnabled();
   }

   private void checkEnabled()
   {
      // do nothing
   }

   private class CheckListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         checkEnabled();
      }
   }

   private class RadioListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         checkEnabled();
      }
   }
}