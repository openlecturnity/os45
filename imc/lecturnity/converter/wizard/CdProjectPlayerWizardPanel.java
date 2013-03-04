package imc.lecturnity.converter.wizard;

import java.awt.Dimension;
import java.awt.Container;
import java.awt.Point;
import java.awt.Font;
import java.awt.Color;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

import javax.swing.*;
import javax.swing.filechooser.FileFilter;

import java.io.IOException;
import java.io.File;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.FileUtils;

import imc.lecturnity.util.ui.DirectoryChooser;

class CdProjectPlayerWizardPanel extends AbstractCdProjectWizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String DESC = "[!]";
   private static String FIND = "[!]";
   private static char   MNEM_FIND = '?';

   private static String DESC_EXE_FILES = "[!]";
   private static String ERR_SIZE_ZERO = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/CdProjectPlayerWizardPanel_",
             "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         DESC = l.getLocalized("DESC");
         FIND = l.getLocalized("FIND");
         MNEM_FIND = l.getLocalized("MNEM_FIND").charAt(0);
         DESC_EXE_FILES = l.getLocalized("DESC_EXE_FILES");
         ERR_SIZE_ZERO = l.getLocalized("ERR_SIZE_ZERO");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private WizardPanel nextWizard_ = null;

   private JTextField playerSetupField_;
   private boolean isFileVerified_ = false;
   private File lastDirectory_ = null;

   public CdProjectPlayerWizardPanel(WizardData data)
   {
      super(data);

      setPreferredSize(new Dimension(500, 400));

      addButtons(NEXT_BACK_CANCEL);

      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");

      Container r = getContentPanel();
      r.setLayout(null);

      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      WizardTextArea wta = new WizardTextArea(DESC, new Point(30, 30),
                                              new Dimension(440, 70),
                                              descFont);
      r.add(wta);

      if (cdData.playerSetup != null)
         playerSetupField_ = new JTextField(cdData.playerSetup);
      else
         playerSetupField_ = new JTextField();

      playerSetupField_.setSize(440, 22);
      playerSetupField_.setLocation(30, 120);
      playerSetupField_.addKeyListener(new TargetKeyListener());
      r.add(playerSetupField_);

      JButton findButton = new JButton(FIND);
      findButton.setSize(120, 30);
      findButton.setLocation(30, 148);
      findButton.setMnemonic(MNEM_FIND);
      findButton.addActionListener(new FindButtonListener());
      r.add(findButton);
   }

   public String getDescription()
   {
      return "";
   }

   public WizardData getWizardData()
   {
      if (nextWizard_ == null)
         nextWizard_ = new CdProjectDocsWizardPanel(cdData);

      cdData.nextWizardPanel = nextWizard_;
      
      return cdData;
   }

   public void refreshDataDisplay()
   {
      if (cdData.playerSetupProject != null)
         if (!cdData.playerSetupProject.equals(""))
            playerSetupField_.setText(cdData.playerSetupProject);
      isFileVerified_ = false;
   }

   public boolean verifyNext()
   {
      if (!isFileVerified_)
      {
         File setupFile = new File(playerSetupField_.getText());

         if (!setupFile.exists())
         {
            JOptionPane.showMessageDialog(this, FILE_NOT_FOUND + '\n' + setupFile,
                                          ERROR, JOptionPane.ERROR_MESSAGE);
            return false;
         }

         if (!(setupFile.length() > 0))
         {
            JOptionPane.showMessageDialog(this, ERR_SIZE_ZERO + '\n' + setupFile,
                                          ERROR, JOptionPane.ERROR_MESSAGE);
            return false;
         }
         
         isFileVerified_ = true;
      }

      if (isFileVerified_)
      {
         cdData.playerSetupProject = playerSetupField_.getText();
         cdData.playerSetup = playerSetupField_.getText();

         writeSettings();
         boolean success = writeProject();
         if (!success)
            return false;
      }

      return isFileVerified_;
   }

   private void findFile()
   {
      JFileChooser fileChooser = new JFileChooser();
      if (lastDirectory_ != null)
         fileChooser.setCurrentDirectory(lastDirectory_);

      FileFilter fileFilter = FileUtils.createFileFilter(DESC_EXE_FILES, ".exe");
      fileChooser.addChoosableFileFilter(fileFilter);
      fileChooser.addChoosableFileFilter(fileChooser.getAcceptAllFileFilter());
      fileChooser.setFileFilter(fileFilter);
      
      Dimension d = fileChooser.getPreferredSize();
      fileChooser.setPreferredSize(new Dimension(d.width + 100, d.height + 50));

      int action = fileChooser.showOpenDialog(this);

      lastDirectory_ = fileChooser.getCurrentDirectory();

      if (action == fileChooser.APPROVE_OPTION)
      {
         File selectedFile = fileChooser.getSelectedFile();
         playerSetupField_.setText(selectedFile.getAbsolutePath());
         isFileVerified_ = selectedFile.exists() && (selectedFile.length() > 0);
      }
   }

   private class FindButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         findFile();
      }
   }

   private class TargetKeyListener extends KeyAdapter
   {
      public void keyTyped(KeyEvent e)
      {
         isFileVerified_ = false;
      }
   }
}
