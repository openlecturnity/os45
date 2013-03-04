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

import java.io.IOException;
import java.io.File;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.FileUtils;

import imc.lecturnity.util.ui.DirectoryChooser;

class CdProjectTemplateWizardPanel extends AbstractCdProjectWizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String DESC = "[!]";
   private static String FIND = "[!]";
   private static char   MNEM_FIND = '?';
   private static String ERR_INDEX_NF = "[!]";
   private static String ERR_DOC_NF = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/CdProjectTemplateWizardPanel_",
             "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         DESC = l.getLocalized("DESC");
         FIND = l.getLocalized("FIND");
         MNEM_FIND = l.getLocalized("MNEM_FIND").charAt(0);
         ERR_INDEX_NF = l.getLocalized("ERR_INDEX_NF");
         ERR_DOC_NF = l.getLocalized("ERR_DOC_NF");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private JTextField templateDirField_;
   private WizardPanel nextWizard_ = null;

   private boolean isDirVerified_ = false;

   public CdProjectTemplateWizardPanel(WizardData data)
   {
      super(data);

      setPreferredSize(new Dimension(500, 400));

      addButtons(BUTTON_BACK | BUTTON_FINISH |
                 BUTTON_QUIT | BUTTON_CANCEL);

      setButtonToolTip(BUTTON_QUIT, TT_QUIT);
      setButtonToolTip(BUTTON_CANCEL, TT_CANCEL);

      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");

      Container r = getContentPanel();
      r.setLayout(null);

      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      WizardTextArea desc = new WizardTextArea
         (DESC, new Point(30, 40), new Dimension(440, 80), descFont);
      r.add(desc);

      // we needn't init the content of the templateDirField_
      // here; it's done in the refreshDataDisplay() method which
      // is called right before the displayWizard() method is
      // called:
      templateDirField_ = new JTextField("Dummi");
      templateDirField_.setSize(440, 22);
      templateDirField_.setLocation(30, 140);
      templateDirField_.addKeyListener(new TemplateDirListener());
      r.add(templateDirField_);

      JButton findButton = new JButton(FIND);
      findButton.setSize(120, 30);
      findButton.setLocation(30, 168);
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
         nextWizard_ = new CdProjectWriterWizardPanel(cdData);
      
      cdData.finishWizardPanel = nextWizard_;

      return cdData;
   }

   public void refreshDataDisplay()
   {
      // if we have a new project, cdData.templateDir is null;
      // then we take the last used template directory from
      // cdData.lastTemplateDir, if such exists. Otherwise 
      // take the default directory.
      // (install path + Publisher\CD Templates\Default).
      String defaultDir = getInstallDir() + "Publisher" + File.separatorChar +
         "CD Templates" + File.separatorChar + "Default";

      if (cdData.templateDir != null && !cdData.templateDir.equals(""))
      {
         templateDirField_.setText(cdData.templateDir);
      }
      else
      {
         if (cdData.lastTemplateDir != null && !cdData.lastTemplateDir.equals("") && (new File(cdData.lastTemplateDir)).exists())
            templateDirField_.setText(cdData.lastTemplateDir);
         else
            templateDirField_.setText(defaultDir);
      }
   }

   public boolean verifyNext()
   {
      if (!isDirVerified_)
         isDirVerified_ = verifyDir();

      if (isDirVerified_)
      {
         // write project/settings!
         cdData.templateDir = templateDirField_.getText();
         cdData.lastTemplateDir = cdData.templateDir;
         
         writeSettings();
         isDirVerified_ = writeProject();
      }

      return isDirVerified_;
   }

   public boolean verifyQuit()
   {
      cdData.templateDir = templateDirField_.getText();
      cdData.lastTemplateDir = cdData.templateDir;
      
      writeSettings();
      return writeProject();
   }

   private void findTemplateDir()
   {
      String curDirName = templateDirField_.getText();

      File curDir = new File(curDirName);

      if (curDirName.equals(""))
         curDirName = (new File(cdData.projectFileName)).getParent();

      if (curDirName == null)
         curDirName = "";
      
      DirectoryChooser dc = new DirectoryChooser(getFrame());
      if (!curDirName.equals(""))
         dc.setSelectedDir(new File(curDirName));

      int result = dc.showDialog();
      if (result == DirectoryChooser.OPTION_APPROVE)
      {
         File selectedDir = dc.getSelectedDir();
         String backup = templateDirField_.getText();
         templateDirField_.setText(selectedDir.getAbsolutePath());
         isDirVerified_ = verifyDir();
         if (!isDirVerified_)
            templateDirField_.setText(backup);
      }
   }

   private boolean verifyDir()
   {
      String dir = templateDirField_.getText() + File.separatorChar;
      File indexFile = new File(dir + "index.html.tmpl");
      File docFile = new File(dir + "doc.html.tmpl");

      if (!indexFile.exists())
      {
         JOptionPane.showMessageDialog(this, ERR_INDEX_NF, ERROR,
                                       JOptionPane.ERROR_MESSAGE);
         return false;
      }

      if (!docFile.exists())
      {
         JOptionPane.showMessageDialog(this, ERR_DOC_NF, ERROR,
                                       JOptionPane.ERROR_MESSAGE);
         return false;
      }

      return true;
   }

   private class FindButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         findTemplateDir();
      }
   }

   private class TemplateDirListener extends KeyAdapter
   {
      public void keyTyped(KeyEvent e)
      {
         isDirVerified_ = false;
      }
   }
}
