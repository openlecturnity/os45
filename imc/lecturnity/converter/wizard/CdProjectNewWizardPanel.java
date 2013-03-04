package imc.lecturnity.converter.wizard;

import java.awt.Dimension;
import java.awt.Container;
import java.awt.Point;
import java.awt.Font;
import java.awt.Color;

import java.awt.event.*;

import javax.swing.*;
import javax.swing.filechooser.FileFilter;
import javax.swing.filechooser.FileSystemView;

import java.io.IOException;
import java.io.File;

import java.util.Vector;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;

class CdProjectNewWizardPanel extends AbstractCdProjectWizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String DESC = "[!]";
   private static String FIND = "[!]";
   private static char   MNEM_FIND = '?';

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/CdProjectNewWizardPanel_",
             "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         DESC = l.getLocalized("DESC");
         FIND = l.getLocalized("FIND");
         MNEM_FIND = l.getLocalized("MNEM_FIND").charAt(0);
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private WizardPanel nextWizard_ = null;

   private JTextField newProjectField_;
   private boolean isFileVerified_ = false;
   private File lastDirectory_ = null;
   
   public CdProjectNewWizardPanel(CdProjectWizardData data)
   {
      super(data);

      setPreferredSize(new Dimension(500, 400));

      addButtons(NEXT_BACK_CANCEL);
      setEnableButton(BUTTON_NEXT, false);

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

      newProjectField_ = new JTextField();
      newProjectField_.setSize(440, 22);
      newProjectField_.setLocation(30, 120);
      newProjectField_.addKeyListener(new TargetKeyListener());
      r.add(newProjectField_);

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
      cdData.projectFileName = newProjectField_.getText();

      if (nextWizard_ == null)
         nextWizard_ = new CdProjectSettingsWizardPanel(cdData);
      
      cdData.nextWizardPanel = nextWizard_;

      return cdData;
   }

   public boolean verifyNext()
   {
      if (!isFileVerified_)
      {
         canonizeFileName();
         fixExtension();
         isFileVerified_ = FileUtils.isWritable(this, newProjectField_.getText());
      }

      boolean allOk = true;

      if (isFileVerified_)
      {
         // put this file into the recent file list...
         addToRecentFiles(newProjectField_.getText());
         cdData.projectFileName = newProjectField_.getText();
         cdData.playerSetupProject = "";
         cdData.copyPlayerSetup = false;
         cdData.standalonePlayer = true;
         // Create a default target directory:
         File pf = new File(cdData.projectFileName);
         cdData.targetDir = pf.toString();
         cdData.targetDir = cdData.targetDir.substring(0, cdData.targetDir.length() - 4);

         // Set the rest of the defaults
         cdData.projectDocs = new Vector();
         cdData.templateDir = "";
         
         // cache settings
         cdData.cacheSettings  = cdData.cacheSettingsDefault;
         cdData.cacheWhere     = cdData.cacheWhereDefault;
         cdData.cacheWhen      = cdData.cacheWhenDefault;
         cdData.cacheDirectory = cdData.cacheDirectoryDefault;
         cdData.cacheSize      = cdData.cacheSizeDefault;

         writeSettings();
         
         allOk = writeProject();
      }

      return isFileVerified_ && allOk;
   }

   private void findFile()
   {
      JFileChooser fileChooser = new JFileChooser();
      if (lastDirectory_ != null)
         fileChooser.setCurrentDirectory(lastDirectory_);
      else
      {
         String cdPath = NativeUtils.getCdProjectsPath();
         if (cdPath != null)
            fileChooser.setCurrentDirectory(new File(cdPath));
      }

      FileFilter fileFilter = FileUtils.createFileFilter(LCP_DESC, EXTENSION);
      fileChooser.addChoosableFileFilter(fileFilter);
      fileChooser.addChoosableFileFilter(fileChooser.getAcceptAllFileFilter());
      fileChooser.setFileFilter(fileFilter);
      
      Dimension d = fileChooser.getPreferredSize();
      fileChooser.setPreferredSize(new Dimension(d.width + 100, d.height + 50));

      int action = fileChooser.showSaveDialog(this);

      lastDirectory_ = fileChooser.getCurrentDirectory();

      if (action == fileChooser.APPROVE_OPTION)
      {
         File selectedFile = fileChooser.getSelectedFile();
         newProjectField_.setText(selectedFile.getAbsolutePath());
         fixExtension();
         isFileVerified_ = FileUtils.isWritable
            (this, selectedFile.getAbsolutePath() + EXTENSION);
         setEnableButton(BUTTON_NEXT, isFileVerified_);
      }
   }

   private void fixExtension()
   {
      String fileName = newProjectField_.getText();
      if (!fileName.toUpperCase().endsWith(EXTENSION.toUpperCase()))
         fileName += EXTENSION;
      
      File file = new File(fileName);
      try
      {
         fileName = file.getCanonicalPath();
      }
      catch (IOException e)
      {
      }

      newProjectField_.setText(fileName);
   }

   private void canonizeFileName()
   {
      String fileName = newProjectField_.getText();
      if (fileName.indexOf('\\') >= 0 ||
          fileName.indexOf('/') >= 0)
         return;

      // fileName does not contain any separator characters,
      // put the new project into the cd projects dir (if
      // available)
      String projectsPath = NativeUtils.getCdProjectsPath();
      if (projectsPath != null)
      {
         File pp = new File(projectsPath);
         if (!pp.isDirectory())
         {
            projectsPath = NativeUtils.getUserHome();
            pp = new File(projectsPath);
         }
         if (!pp.isDirectory())
            projectsPath = ".";
      }

      newProjectField_.setText(projectsPath + File.separatorChar + fileName);
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
      public void keyReleased(KeyEvent e)
      {
         isFileVerified_ = false;
         setEnableButton(BUTTON_NEXT, !newProjectField_.getText().equals(""));
      }
   }
}
