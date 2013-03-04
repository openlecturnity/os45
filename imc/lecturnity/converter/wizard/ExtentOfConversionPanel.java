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

import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.wizards.*;

import imc.lecturnity.util.ui.PopupHelp;
import imc.lecturnity.util.ui.LazyTextArea;

import imc.epresenter.filesdk.util.Localizer;

class ExtentOfConversionPanel extends WizardPanel
{
   private static String EXTENT = "[!]";
   private static String EXTENT_CAPTION = "[!]";
   private static String POINTER = "[!]";
   private static char   MNEM_POINTER = '?';
   private static String DYNAMICS = "[!]";
   private static char   MNEM_DYNAMICS = '?';
   private static String DATA_REDUCTION = "[!]";
   private static char   MNEM_DATA_REDUCTION = '?';
//    private static String SCALABLE_PAGE_LAYOUT = "[!]";
//    private static char   MNEM_SCALABLE_PAGE_LAYOUT = '?';
   private static String TT_POINTER = "[!]";
   private static String TT_DYNAMICS = "[!]";
   private static String TT_DATA_REDUCTION = "[!]";
//    private static String TT_SCALABLE_PAGE_LAYOUT = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/ExtentOfConversionPanel_",
             "en");
         EXTENT = l.getLocalized("EXTENT");
         EXTENT_CAPTION = l.getLocalized("EXTENT_CAPTION");
         POINTER = l.getLocalized("POINTER");
         MNEM_POINTER = l.getLocalized("MNEM_POINTER").charAt(0);
         DYNAMICS = l.getLocalized("DYNAMICS");
         MNEM_DYNAMICS = l.getLocalized("MNEM_DYNAMICS").charAt(0);
         DATA_REDUCTION = l.getLocalized("DATA_REDUCTION");
         MNEM_DATA_REDUCTION = l.getLocalized("MNEM_DATA_REDUCTION").charAt(0);
//          SCALABLE_PAGE_LAYOUT = l.getLocalized("SCALABLE_PAGE_LAYOUT");
//          MNEM_SCALABLE_PAGE_LAYOUT = l.getLocalized("MNEM_SCALABLE_PAGE_LAYOUT").charAt(0);
         TT_POINTER = l.getLocalized("TT_POINTER");
         TT_DYNAMICS = l.getLocalized("TT_DYNAMICS");
         TT_DATA_REDUCTION = l.getLocalized("TT_DATA_REDUCTION");
//          TT_SCALABLE_PAGE_LAYOUT = l.getLocalized("TT_SCALABLE_PAGE_LAYOUT");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open ExtentOfConversionPanel locale database!\n"+
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private PublisherWizardData pwData_;

   private JCheckBox useDynamicsBox_;
   private JCheckBox usePointerBox_;
   private JCheckBox useDataReductionBox_;
//    private JCheckBox useScalingBox_;
   
   private WizardTextArea descPointerBox_;
   private WizardTextArea descDynamics_;
   private WizardTextArea descDataReduction_;
//    private WizardTextArea descScalablePageLayout_;

   private ExtendedSettingsDialog parent_ = null;
   
   public ExtentOfConversionPanel(ExtendedSettingsDialog parent, PublisherWizardData pwData)
   {
      super();
      
      parent_ = parent;
      pwData_ = pwData;

      initGui();
   }

//    public Insets getInsets()
//    { 
//       return new Insets(10, 10, 10, 10);
//    }
   
   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      ps.SetBoolValue("bRmWmUsePointer", usePointerBox_.isSelected());
      ps.SetBoolValue("bRmWmUseDynamic", useDynamicsBox_.isSelected());
      ps.SetBoolValue("bRmWmUseDataReduction", useDataReductionBox_.isSelected());

      return pwData_;
   }

   public String getDescription()
   {
      return "";
   }

   private void initGui()
   {
      //Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      ActionListener checkListener = new CheckListener();

      this.setLayout(new BorderLayout());
      JPanel extentPanel = new JPanel();
      extentPanel.setLayout(null);
///      extentPanel.setBorder(new TitledBorder(EXTENT_CAPTION));
      this.add(extentPanel, BorderLayout.CENTER);

      int y = 20;
      int s1 = 240;
      int s2 = 400;
      int x1 = 20;
      int x2 = 40;
      
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      boolean bUsePointer = ps.GetBoolValue("bRmWmUsePointer");
      boolean bUseDynamic = ps.GetBoolValue("bRmWmUseDynamic");
      boolean bUseDataReduction = ps.GetBoolValue("bRmWmUseDataReduction");

      usePointerBox_ = new JCheckBox(POINTER, bUsePointer);
      usePointerBox_.setFont(headerFont);
      usePointerBox_.setSize(s1, 22);
      usePointerBox_.setLocation(x1, y);
      usePointerBox_.addActionListener(checkListener);
//      usePointerBox_.setToolTipText(TT_POINTER);
      usePointerBox_.setMnemonic(MNEM_POINTER);
      extentPanel.add(usePointerBox_);

      y += 25;
      descPointerBox_ = new WizardTextArea
         (TT_POINTER, new Point(x2, y), new Dimension(s2, 45), descFont);
      extentPanel.add(descPointerBox_);

      y += 60;
      useDynamicsBox_ = new JCheckBox(DYNAMICS, bUseDynamic);
      useDynamicsBox_.setFont(headerFont);
      useDynamicsBox_.setSize(s1, 22);
      useDynamicsBox_.setLocation(x1, y);
      useDynamicsBox_.addActionListener(checkListener);
//      useDynamicsBox_.setToolTipText(TT_DYNAMICS);
      useDynamicsBox_.setMnemonic(MNEM_DYNAMICS);
      extentPanel.add(useDynamicsBox_);

      y += 25;
      descDynamics_ = new WizardTextArea
         (TT_DYNAMICS, new Point(x2, y), new Dimension(s2, 45), descFont);
      extentPanel.add(descDynamics_);

      y += 50;
      useDataReductionBox_ = new JCheckBox(DATA_REDUCTION, bUseDataReduction);
      useDataReductionBox_.setFont(headerFont);
      useDataReductionBox_.setSize(s1, 22);
      useDataReductionBox_.setLocation(x1+20, y);
      useDataReductionBox_.addActionListener(checkListener);
//      useDataReductionBox_.setToolTipText(TT_DATA_REDUCTION);
      useDataReductionBox_.setMnemonic(MNEM_DATA_REDUCTION);
      extentPanel.add(useDataReductionBox_);

      y += 25;
      descDataReduction_ = new WizardTextArea
         (TT_DATA_REDUCTION, new Point(x2+20, y), new Dimension(s2-20, 60), descFont);
      extentPanel.add(descDataReduction_);

      checkEnabled();
   }

   private void checkEnabled()
   {
      useDataReductionBox_.setEnabled(useDynamicsBox_.isSelected());
      descDataReduction_.setEnabled(useDynamicsBox_.isSelected());
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
