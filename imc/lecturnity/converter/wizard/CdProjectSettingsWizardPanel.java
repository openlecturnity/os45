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
import javax.swing.filechooser.FileSystemView;

import java.io.IOException;
import java.io.File;

import imc.epresenter.converter.PresentationConverter;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;

import imc.lecturnity.util.ui.DirectoryChooser;

public class CdProjectSettingsWizardPanel extends AbstractCdProjectWizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String TARGETDIR_DESC = "[!]";
   private static String NO_INSTALL = "[!]";
   private static char   MNEM_NO_INSTALL = '?';
   private static String NO_DESC = "[!]";
   private static String YES_INSTALL = "[!]";
   private static char   MNEM_YES_INSTALL = '?';
   private static String YES_DESC = "[!]";
   private static String FIND = "[!]";
   private static char   MNEM_FIND = '?';
   private static String ERROR = "[!]";
   private static String ERR_NOT_DIR = "[!]";
   private static String ERR_NO_PATH = "[!]";

   private static String LICENSE_NOTE = "[!]";
   private static String INFORMATION = "[!]";

   private static String EXTENDED = "[!]";
   private static char   MNEM_EXTENDED = '?';
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/CdProjectSettingsWizardPanel_",
             "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         TARGETDIR_DESC = l.getLocalized("TARGETDIR_DESC");
         NO_INSTALL = l.getLocalized("NO_INSTALL");
         MNEM_NO_INSTALL = l.getLocalized("MNEM_NO_INSTALL").charAt(0);
         NO_DESC = l.getLocalized("NO_DESC");
         YES_INSTALL = l.getLocalized("YES_INSTALL");
         MNEM_YES_INSTALL = l.getLocalized("MNEM_YES_INSTALL").charAt(0);
         YES_DESC = l.getLocalized("YES_DESC");
         FIND = l.getLocalized("FIND");
         MNEM_FIND = l.getLocalized("MNEM_FIND").charAt(0);
         ERROR = l.getLocalized("ERROR");
         ERR_NOT_DIR = l.getLocalized("ERR_NOT_DIR");
         ERR_NO_PATH = l.getLocalized("ERR_NO_PATH");
         LICENSE_NOTE = l.getLocalized("LICENSE_NOTE");
         INFORMATION = l.getLocalized("INFORMATION");

         EXTENDED = l.getLocalized("EXTENDED");
         MNEM_EXTENDED = l.getLocalized("MNEM_EXTENDED").charAt(0);
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         System.exit(1);
      }
   }

   private WizardPanel nextDocsWizard_ = null;
   private WizardPanel nextPlayerWizard_ = null;

   private JTextField targetDirField_;
   private JCheckBox noInstallBox_;
   private JCheckBox yesInstallBox_;

   private boolean isDirVerified_ = false;
   private boolean readProjectOnDisplay_ = false;

   private File lastDirectory_ = null;

   public CdProjectSettingsWizardPanel(WizardData data)
   {
      super(data);

      setPreferredSize(new Dimension(500, 400));

      addButtons(NEXT_BACK_CANCEL | BUTTON_CUSTOM, EXTENDED, MNEM_EXTENDED);

      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");

      Container r = getContentPanel();
      r.setLayout(null);

      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      JLabel targetDirLabel = new JLabel(TARGETDIR_DESC);
      targetDirLabel.setSize(440, 20);
      targetDirLabel.setLocation(30, 20);
      r.add(targetDirLabel);
      
      targetDirField_ = new JTextField(cdData.targetDir);
      targetDirField_.setSize(440, 22);
      targetDirField_.setLocation(30, 45);
      targetDirField_.addKeyListener(new TargetDirListener());
      r.add(targetDirField_);

      JButton findButton = new JButton(FIND);
      findButton.setSize(120, 30);
      findButton.setLocation(30, 73);
      findButton.setMnemonic(MNEM_FIND);
      findButton.addActionListener(new FindButtonListener());
      r.add(findButton);

      noInstallBox_ = new JCheckBox(NO_INSTALL, cdData.standalonePlayer);
      noInstallBox_.setFont(headerFont);
      noInstallBox_.setSize(440, 20);
      noInstallBox_.setLocation(30, 120);
      noInstallBox_.setMnemonic(MNEM_NO_INSTALL);

      r.add(noInstallBox_);

      WizardTextArea noInstallDesc = new WizardTextArea
         (NO_DESC, new Point(60, 145), new Dimension(410, 44), descFont);
      r.add(noInstallDesc);

      yesInstallBox_ = new JCheckBox(YES_INSTALL, cdData.copyPlayerSetup);
      yesInstallBox_.setFont(headerFont);
      yesInstallBox_.setSize(440, 20);
      yesInstallBox_.setLocation(30, 190);
      yesInstallBox_.setMnemonic(MNEM_YES_INSTALL);
      r.add(yesInstallBox_);

      WizardTextArea yesInstallDesc = new WizardTextArea
         (YES_DESC, new Point(60, 215), new Dimension(410, 66), descFont);
      r.add(yesInstallDesc);
   }

   public String getDescription()
   {
      String caption = CAPTION;

      return caption;
   }

   public WizardData getWizardData()
   {
      if (yesInstallBox_.isSelected())
      {
         if (nextPlayerWizard_ == null)
            nextPlayerWizard_ = new CdProjectPlayerWizardPanel(cdData);

         cdData.nextWizardPanel = nextPlayerWizard_;
      }
      else
      {
         if (nextDocsWizard_ == null)
            nextDocsWizard_ = new CdProjectDocsWizardPanel(cdData);
         
         cdData.nextWizardPanel = nextDocsWizard_;
      }

      return cdData;
   }

   public void readProjectOnDisplay()
   {
      readProjectOnDisplay_ = true;
   }

   public void refreshDataDisplay()
   {
      targetDirField_.setText(cdData.targetDir);
      noInstallBox_.setSelected(cdData.standalonePlayer);
      yesInstallBox_.setSelected(cdData.copyPlayerSetup);

      String projectName = (new File(cdData.projectFileName)).getName();

      getFrame().setTitle(getDescription() + " [" + projectName + "]");
   }

   public int displayWizard()
   {
      String instDir = System.getProperty("install.path");
      if (instDir == null)
      {
         JOptionPane.showMessageDialog(this, ERR_NO_PATH, ERROR,
                                       JOptionPane.ERROR_MESSAGE);
         setEnableButton(BUTTON_NEXT, false);
         setEnableButton(BUTTON_CANCEL, true);
      }
      else
      {
         setInstallDir(instDir);

         if (readProjectOnDisplay_)
         {
            readSettings();
            
            boolean success = readProject();
            
            if (!success)
            {
               setEnableButton(BUTTON_NEXT, false);
            }
            else
            {
               setEnableButton(BUTTON_NEXT, true);
            }
            
            readProjectOnDisplay_ = false;
            
            refreshDataDisplay();
         }
      }

      return super.displayWizard();
   }

   protected void customCommand(String command)
   {
//       System.out.println("Command: " + command);
      CdProjectWizardData data2 = new CdProjectWizardData();
      cdData.fillOtherWizardData(data2);
      CdProjectCacheDialog cpcd = new CdProjectCacheDialog(getFrame(), data2);
      cpcd.show();
      if (cpcd.getActionTaken() == CdProjectCacheDialog.ACTION_OK)
      {
         data2.fillOtherWizardData(cdData);
      }
   }

   private void canonizeDirName()
   {
      String dirName = targetDirField_.getText();
      if (dirName.indexOf('\\') >= 0 ||
          dirName.indexOf('/') >= 0)
         return;

      // dirName does not contain any separator characters,
      // put the new project into the home dir by default
      FileSystemView fsv = FileSystemView.getFileSystemView();
      File home = fsv.getHomeDirectory();

      dirName = home.toString() + File.separatorChar + dirName;

      targetDirField_.setText(dirName);
   }


   public boolean verifyNext()
   {
//       if (yesInstallBox_.isSelected())
//       {
//          JOptionPane.showMessageDialog(this, "Not supported: Install package for Player",
//                                        ERROR, JOptionPane.INFORMATION_MESSAGE);
//          return false;
//       }

      if (!isDirVerified_)
      {
         canonizeDirName();

         File targetDir = new File(targetDirField_.getText());

         isDirVerified_ = FileUtils.checkDirectory(this, targetDirField_.getText());

         if (!isDirVerified_)
            return false;

//          if (!targetDir.isDirectory())
//          {
//             JOptionPane.showMessageDialog
//                (this, ERR_NOT_DIR, ERROR,
//                 JOptionPane.ERROR_MESSAGE);
//             return false;
//          }

         isDirVerified_ = FileUtils.isWritable
            (this, targetDirField_.getText() + File.separatorChar + "#tmp");

         if (!isDirVerified_)
            return false;
      }

      cdData.targetDir = targetDirField_.getText();
      cdData.copyPlayerSetup = yesInstallBox_.isSelected();
      cdData.standalonePlayer = noInstallBox_.isSelected();

      boolean success = writeProject();

      // show a message box for the license for the player
      if (success && yesInstallBox_.isSelected())
      {
         JOptionPane.showMessageDialog(this, LICENSE_NOTE, INFORMATION,
                                       JOptionPane.INFORMATION_MESSAGE);
      }

      return success;
   }

   private void findTargetDir()
   {
      String curDirName = targetDirField_.getText();

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
         targetDirField_.setText(selectedDir.getAbsolutePath());
         isDirVerified_ = FileUtils.isWritable
            (this, selectedDir.getAbsolutePath() + File.separatorChar + "#tmp");
      }
   }

   private class TargetDirListener extends KeyAdapter
   {
      public void keyTyped(KeyEvent e)
      {
         isDirVerified_ = false;
      }
   }

   private class FindButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         findTargetDir();
      }
   }
}
