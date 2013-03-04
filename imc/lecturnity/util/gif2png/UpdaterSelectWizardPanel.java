package imc.lecturnity.util.gif2png;

import java.awt.Dimension;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.io.File;

import javax.swing.*;
import javax.swing.filechooser.FileFilter;

import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.WizardData;
import imc.lecturnity.util.wizards.Wizard;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.ConsistencyChecker;
import imc.epresenter.filesdk.PresentationData;

import imc.epresenter.filesdk.util.Localizer;

class UpdaterSelectWizardPanel extends WizardPanel
{
   private static String CAPTION = "[!]";
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String SELECT = "[!]";
   private static String FIND = "[!]";
   private static char   MNEM_FIND = '?';

   private static String FILE1_DESCRIPTION = "[!]";
   private static String FILE2_DESCRIPTION = "[!]";
   private static String FILE3_DESCRIPTION = "[!]";
   private static String FILE4_DESCRIPTION = "[!]";

   private static String ERROR = "[!]";
   private static String ERR_OPEN = "[!]";

   private static String INFORMATION = "[!]";
   private static String INFO_LRD = "[!]";
   
   private static boolean USE_CUSTOM_COLORS = false;
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/util/gif2png/UpdaterSelectWizardPanel_", "en");

         CAPTION = l.getLocalized("CAPTION");
         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         SELECT = l.getLocalized("SELECT");
         FIND = l.getLocalized("FIND");
         MNEM_FIND = l.getLocalized("MNEM_FIND").charAt(0);

         FILE1_DESCRIPTION = l.getLocalized("FILE1_DESCRIPTION");
         FILE2_DESCRIPTION = l.getLocalized("FILE2_DESCRIPTION");
         FILE3_DESCRIPTION = l.getLocalized("FILE3_DESCRIPTION");
         FILE4_DESCRIPTION = l.getLocalized("FILE4_DESCRIPTION");

         ERROR = l.getLocalized("ERROR");
         ERR_OPEN = l.getLocalized("ERR_OPEN");

         INFORMATION = l.getLocalized("INFORMATION");
         INFO_LRD = l.getLocalized("INFO_LRD");
      }
      catch (Exception e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog
            (null, "Localizer error in imc.lecturnity.util.gif2png.UpdaterSelectWizardPanel:\n" +
             e.toString(), "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(0);
      }
   }

   private UpdaterData data_ = null;
   private WizardPanel nextTargetWizard_ = null;
   private WizardPanel nextUpdateWizard_ = null;
   private JTextField presentationFileField_;

   private File lastDirectory_ = null;

   private boolean hasNewFormat_ = false;

   public UpdaterSelectWizardPanel(UpdaterData data)
   {
      super();

      data_ = data;

      setPreferredSize(new Dimension(500, 400));

      addButtons(NEXT_BACK_CANCEL);
      setEnableButton(BUTTON_BACK, false);
      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");
      

      getContentPanel().setLayout(null);

      JLabel desc = new JLabel(SELECT);
      desc.setFont(NORMAL_FONT);
      desc.setLocation(30, 100);
      desc.setSize(460, 15);
      getContentPanel().add(desc);

      presentationFileField_ = new JTextField("");
      presentationFileField_.setFont(NORMAL_FONT);
      presentationFileField_.setLocation(30, 120);
      presentationFileField_.setSize(440, 22);
      getContentPanel().add(presentationFileField_);

      JButton findButton = new JButton(FIND);
      // if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
      if (USE_CUSTOM_COLORS)
         findButton.setBackground(Wizard.BG_COLOR);
      findButton.setSize(120, 30);
      findButton.setLocation(30, 148);
      findButton.setMnemonic(MNEM_FIND);
      findButton.addActionListener(new FindButtonListener());
      getContentPanel().add(findButton);
   }

   public boolean verifyNext()
   {
      try
      {
         String fileName = presentationFileField_.getText();

         FileResources res = FileResources.createFileResources(fileName);

         if (FileResources.hasNewFormat(fileName))
         {
            // as we have come this far, everything should be fine.
            // do nothing.
            hasNewFormat_ = true;
         }
         else
         {
            // LPD file, check for consistency
            hasNewFormat_ = false;
            boolean success = ConsistencyChecker.checkForConsistency(fileName);
            if (!success)
               throw new Exception
                  (ConsistencyChecker.getLastConsistencyError());

            // display an information message
            if (JOptionPane.showConfirmDialog
                (this, INFO_LRD, INFORMATION,
                 JOptionPane.OK_CANCEL_OPTION, 
                 JOptionPane.INFORMATION_MESSAGE) != JOptionPane.OK_OPTION)
            {
               return false;
            }
         }
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(this, ERR_OPEN + e.getMessage(), ERROR,
                                       JOptionPane.ERROR_MESSAGE);

         e.printStackTrace();
         return false;
      }
      return true;
   }

   public WizardData getWizardData()
   {
      if (hasNewFormat_)
      {
         if (nextTargetWizard_ == null)
            nextTargetWizard_ = new UpdaterTargetWizardPanel(data_);
         
         data_.nextWizardPanel = nextTargetWizard_;
      }
      else
      {
         if (nextUpdateWizard_ == null)
            nextUpdateWizard_ = new UpdaterUpdateWizardPanel(data_);

         data_.nextWizardPanel = nextUpdateWizard_;
      }

      data_.fileName = presentationFileField_.getText();

      return data_;
   }

   private void findPresentation()
   {
      JFileChooser fileChooser = new JFileChooser();
      if (lastDirectory_ != null)
         fileChooser.setCurrentDirectory(lastDirectory_);
      //if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
      if (USE_CUSTOM_COLORS)
         fileChooser.setBackground(Wizard.BG_COLOR);
      LecturnityFileFilter lff = new LecturnityFileFilter();
      fileChooser.addChoosableFileFilter(lff);
      LpdFileFilter lpf = new LpdFileFilter();
      fileChooser.addChoosableFileFilter(lpf);
      EPresentationFileFilter eff = new EPresentationFileFilter();
      fileChooser.addChoosableFileFilter(eff);
      AofFileFilter aff = new AofFileFilter();
      fileChooser.addChoosableFileFilter(aff);
      fileChooser.addChoosableFileFilter(fileChooser.getAcceptAllFileFilter());
      fileChooser.setFileFilter(lff);

      Dimension fcSize = fileChooser.getPreferredSize();
      fileChooser.setPreferredSize(new Dimension(fcSize.width + 100, fcSize.height + 50));

      int action = fileChooser.showOpenDialog(this);

      lastDirectory_ = fileChooser.getCurrentDirectory();

      if (action == fileChooser.APPROVE_OPTION)
      {
         File selectedFile = fileChooser.getSelectedFile();
         presentationFileField_.setText(selectedFile.getAbsolutePath());
      }
   }

   public void refreshDataDisplay()
   {
      if (data_.fileName != null)
      {
         if (!presentationFileField_.getText().equals(data_.fileName))
            presentationFileField_.setText(data_.fileName);
      }
   }

   public String getDescription()
   {
      return CAPTION + DocumentUpdater.VERSION;
   }

   private class FindButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         findPresentation();
      }
   }

   private class LecturnityFileFilter extends FileFilter
   {
      public boolean accept(File f)
      {
         if (f.isDirectory())
            return true;
         String fileName = f.getName().toUpperCase();
         return (fileName.endsWith(".LRD"));
      }

      public String getDescription()
      {
         return FILE1_DESCRIPTION;
      }
   }

   private class LpdFileFilter extends FileFilter
   {
      public boolean accept(File f)
      {
         if (f.isDirectory())
            return true;
         String fileName = f.getName().toUpperCase();
         return (fileName.endsWith(".LPD"));
      }

      public String getDescription()
      {
         return FILE4_DESCRIPTION;
      }
   }

   private class EPresentationFileFilter extends FileFilter
   {
      public boolean accept(File f)
      {
         if (f.isDirectory())
            return true;
         String fileName = f.getName().toUpperCase();
         return (fileName.endsWith(".EPF"));
      }

      public String getDescription()
      {
         return FILE2_DESCRIPTION;
      }
   }

   private class AofFileFilter extends FileFilter
   {
      public boolean accept(File f)
      {
         if (f.isDirectory())
            return true;
         String fileName = f.getName().toUpperCase();
         return (fileName.endsWith(".AOF"));
      }

      public String getDescription()
      {
         return FILE3_DESCRIPTION;
      }
   }

}
