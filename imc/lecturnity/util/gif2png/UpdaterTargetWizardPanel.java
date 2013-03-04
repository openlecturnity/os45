package imc.lecturnity.util.gif2png;

import java.awt.Dimension;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.io.File;
import java.io.IOException;

import javax.swing.*;
import javax.swing.filechooser.FileFilter;

import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.WizardData;
import imc.lecturnity.util.wizards.Wizard;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.ConsistencyChecker;
import imc.epresenter.filesdk.PresentationData;
import imc.epresenter.filesdk.Archiver;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.util.FileUtils;

class UpdaterTargetWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String SELECT = "[!]";
   private static String FIND = "[!]";
   private static char   MNEM_FIND = '?';

   private static String FILE4_DESCRIPTION = "[!]";

   private static String ERROR = "[!]";
   private static String ERR_OTHER = "[!]";
   private static String ERR_RESOLVE = "[!]";

   private static String INFORMATION = "[!]";
   private static String INFO_FILE = "[!]";
   
   private static boolean USE_CUSTOM_COLORS = false;

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/util/gif2png/UpdaterTargetWizardPanel_", "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         SELECT = l.getLocalized("SELECT");
         FIND = l.getLocalized("FIND");
         MNEM_FIND = l.getLocalized("MNEM_FIND").charAt(0);

         FILE4_DESCRIPTION = l.getLocalized("FILE4_DESCRIPTION");

         ERROR = l.getLocalized("ERROR");
         ERR_OTHER = l.getLocalized("ERR_OTHER");
         ERR_RESOLVE = l.getLocalized("ERR_RESOLVE");

         INFORMATION = l.getLocalized("INFORMATION");
         INFO_FILE = l.getLocalized("INFO_FILE");
      }
      catch (Exception e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog
            (null, "Localizer error in imc.lecturnity.util.gif2png.UpdaterTargetWizardPanel:\n" +
             e.toString(), "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(0);
      }
   }

   private UpdaterData data_ = null;
   private WizardPanel nextWizard_ = null;
   private JTextField targetFileField_;

   private File lastDirectory_ = null;

   public UpdaterTargetWizardPanel(UpdaterData data)
   {
      super();

      data_ = data;

      setPreferredSize(new Dimension(500, 400));

      addButtons(BACK_FINISH_CANCEL);
      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");

      getContentPanel().setLayout(null);

      JLabel desc = new JLabel(SELECT);
      desc.setFont(NORMAL_FONT);
      desc.setLocation(30, 100);
      desc.setSize(460, 15);
      getContentPanel().add(desc);

      targetFileField_ = new JTextField("");
      targetFileField_.setFont(NORMAL_FONT);
      targetFileField_.setLocation(30, 120);
      targetFileField_.setSize(440, 22);
//       TextFieldListener tfl = new TextFieldListener();
//       targetFileField_.addActionListener(tfl); 
//       targetFileField_.addTextListener(tfl);
      getContentPanel().add(targetFileField_);

      JButton findButton = new JButton(FIND);
      //if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
      if (USE_CUSTOM_COLORS)
         findButton.setBackground(Wizard.BG_COLOR);
      findButton.setSize(120, 30);
      findButton.setLocation(30, 148);
      findButton.setMnemonic(MNEM_FIND);
      findButton.addActionListener(new FindButtonListener());
      getContentPanel().add(findButton);

   }

   public String getDescription()
   {
      return "";
   }

   public boolean verifyNext()
   {
      fixExtension();

      String destName = targetFileField_.getText();

      if (destName.equals(""))
      {
         JOptionPane.showMessageDialog
            (this, INFO_FILE, INFORMATION, JOptionPane.INFORMATION_MESSAGE);
         return false;
      }

      try
      {
         File src  = new File(data_.fileName);
         File dest = new File(destName);

         String srcResName = src.getCanonicalPath();
         String destResName = dest.getCanonicalPath();
         
         targetFileField_.setText(destResName);

         if (srcResName.equalsIgnoreCase(destResName + Archiver.ARCHIVE_EXTENSION))
         {
            JOptionPane.showMessageDialog
               (this, ERR_OTHER, ERROR, JOptionPane.ERROR_MESSAGE);
            return false;
         }

         return FileUtils.isWritable(this, destResName + Archiver.ARCHIVE_EXTENSION);
      }
      catch (IOException e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog
            (this, ERR_RESOLVE, ERROR, JOptionPane.ERROR_MESSAGE);
         return false;
      }
   }

   private void fixExtension()
   {
      String destName = targetFileField_.getText();
      if (destName.toUpperCase().endsWith(Archiver.ARCHIVE_EXTENSION.toUpperCase()))
         targetFileField_.setText(destName.substring(0, destName.length() - 4));
   }

   public WizardData getWizardData()
   {
      if (nextWizard_ == null)
         nextWizard_ = new UpdaterUpdateWizardPanel(data_);

      data_.finishWizardPanel = nextWizard_;

      data_.targetFileName = targetFileField_.getText();

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
      LpdFileFilter lpf = new LpdFileFilter();
      fileChooser.addChoosableFileFilter(lpf);
      fileChooser.addChoosableFileFilter(fileChooser.getAcceptAllFileFilter());
      fileChooser.setFileFilter(lpf);

      Dimension fcSize = fileChooser.getPreferredSize();
      fileChooser.setPreferredSize(new Dimension(fcSize.width + 100, fcSize.height + 50));

      int action = fileChooser.showSaveDialog(this);

      lastDirectory_ = fileChooser.getCurrentDirectory();

      if (action == fileChooser.APPROVE_OPTION)
      {
         File selectedFile = fileChooser.getSelectedFile();
         targetFileField_.setText(selectedFile.getAbsolutePath());
      }
   }

   private class FindButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         findPresentation();
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
}
