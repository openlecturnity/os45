package imc.lecturnity.converter.wizard;

import java.awt.Dimension;
import java.awt.Container;
import java.awt.Point;
import java.awt.Font;
import java.awt.Color;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import javax.swing.*;
import javax.swing.border.BevelBorder;
import javax.swing.filechooser.FileFilter;

import java.io.IOException;
import java.io.File;

import java.util.Vector;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.ui.CustomJList;
import imc.lecturnity.util.ui.IconListCellRenderer;
import imc.lecturnity.util.ColorManager;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.FileUtils;

import imc.epresenter.filesdk.util.Localizer;

import imc.epresenter.converter.PresentationConverter;

public class CdProjectWizardPanel extends AbstractCdProjectWizardPanel
{
//    private static String CAPTION = "[!]";
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String NEW_PROJECT = "[!]";
   private static char   MNEM_NEW_PROJECT = '?';
   private static String OPEN_PROJECT = "[!]";
   private static char   MNEM_OPEN_PROJECT = '?';
   private static String OTHER_FILES = "[!]";
   private static String ERR_NO_PATH = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/CdProjectWizardPanel_",
             "en");

//          CAPTION = l.getLocalized("CAPTION");
         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         NEW_PROJECT = l.getLocalized("NEW_PROJECT");
         MNEM_NEW_PROJECT = l.getLocalized("MNEM_NEW_PROJECT").charAt(0);
         OPEN_PROJECT = l.getLocalized("OPEN_PROJECT");
         MNEM_OPEN_PROJECT = l.getLocalized("MNEM_OPEN_PROJECT").charAt(0);
         OTHER_FILES = l.getLocalized("OTHER_FILES");
         ERR_NO_PATH = l.getLocalized("ERR_NO_PATH");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private CdProjectNewWizardPanel nextNewWizard_ = null;
   private CdProjectSettingsWizardPanel nextSettingsWizard_ = null;

   private JRadioButton newProjectRadio_;
   private JRadioButton openProjectRadio_;

   private JList recentList_;
   private JScrollPane listScroller_;

   private DefaultListModel recentProjectsList_;
   private File lastDirectory_;

   public CdProjectWizardPanel(CdProjectWizardData data)
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

      ActionListener radioButtonListener = new ButtonListener();

      newProjectRadio_ = new JRadioButton(NEW_PROJECT, true);
      newProjectRadio_.setFont(headerFont);
      newProjectRadio_.setSize(420, 20);
      newProjectRadio_.setLocation(50, 40);
      newProjectRadio_.setMnemonic(MNEM_NEW_PROJECT);
      newProjectRadio_.addActionListener(radioButtonListener);      
      r.add(newProjectRadio_);

      openProjectRadio_ = new JRadioButton(OPEN_PROJECT, false);
      openProjectRadio_.setFont(headerFont);
      openProjectRadio_.setSize(420, 20);
      openProjectRadio_.setLocation(50, 90);
      openProjectRadio_.setMnemonic(MNEM_OPEN_PROJECT);
      openProjectRadio_.addActionListener(radioButtonListener);      
      r.add(openProjectRadio_);

      ButtonGroup buttonGroup = new ButtonGroup();
      buttonGroup.add(newProjectRadio_);
      buttonGroup.add(openProjectRadio_);

      recentProjectsList_ = new DefaultListModel();
      initSettings();
      
      IconListCellRenderer ilcr = new IconListCellRenderer
         ("/imc/lecturnity/converter/images/cd_small.png");
      
      recentList_ = new CustomJList(recentProjectsList_);
      recentList_.setCellRenderer(ilcr);
      DefaultListSelectionModel dlsm = new DefaultListSelectionModel();
      dlsm.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
      recentList_.setSelectionModel(dlsm);
      recentList_.setSelectedIndex(0);
      recentList_.setEnabled(false);
      
      listScroller_ = new JScrollPane(recentList_);
      listScroller_.setSize(440, 120);
      listScroller_.setLocation(30, 130);
      listScroller_.setEnabled(false);
      listScroller_.setBorder(new BevelBorder(BevelBorder.LOWERED));

      r.add(listScroller_);
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
      }

      setEnableButton(BUTTON_BACK, false);

      return super.displayWizard();
   }

   public String getDescription()
   {
      String caption = CAPTION;

      return caption;
   }

   public WizardData getWizardData()
   {
      if (newProjectRadio_.isSelected())
      {
         // user wants to create a new project
         if (nextNewWizard_ == null)
            nextNewWizard_ = new CdProjectNewWizardPanel(cdData);
         cdData.nextWizardPanel = nextNewWizard_;
      }
      else
      {
         if (nextSettingsWizard_ == null)
            nextSettingsWizard_ = new CdProjectSettingsWizardPanel(cdData);
         cdData.nextWizardPanel = nextSettingsWizard_;
      }

      return cdData;
   }

   public void refreshDataDisplay()
   {
      initSettings();
      recentList_.setSelectedIndex(0);
      getFrame().setTitle(getDescription());
   }

   public boolean verifyNext()
   {
      if (newProjectRadio_.isSelected())
         return true;

      // load an old project...
      int index = recentList_.getSelectedIndex();
      if (index == 0)
      {
         // open other file...
         boolean success = openOtherFile();
         return success;
      }
      else
      {
         cdData.projectFileName = recentProjectsList_.get(index).toString();
         System.out.println("opening: " + cdData.projectFileName);
         boolean success = readProject();

         if (success)
         {
            addToRecentFiles(cdData.projectFileName);
            writeSettings();
         }

         return success;
      }
   }

   private boolean openOtherFile()
   {
      JFileChooser fileChooser = new JFileChooser();
      if (lastDirectory_ != null)
         fileChooser.setCurrentDirectory(lastDirectory_);
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

      int action = fileChooser.showOpenDialog(this);

      lastDirectory_ = fileChooser.getCurrentDirectory();

      if (action == fileChooser.APPROVE_OPTION)
      {
         File selectedFile = fileChooser.getSelectedFile();
         if (selectedFile.exists())
         {
            cdData.projectFileName = selectedFile.toString();
            return readProject();
         }
         else
         {
            JOptionPane.showMessageDialog
               (this, FILE_NOT_FOUND + " " + selectedFile,
                ERROR, JOptionPane.ERROR_MESSAGE);
         }
      }
      
      return false;
   }

   private void initSettings()
   {
      readSettings();
      recentProjectsList_.removeAllElements();
      recentProjectsList_.addElement(OTHER_FILES);
      for (int i=0; i<cdData.recentFiles.size(); i++)
         recentProjectsList_.addElement(cdData.recentFiles.elementAt(i));
   }

   private void updateList()
   {
      recentList_.setEnabled(openProjectRadio_.isSelected());
      listScroller_.setEnabled(openProjectRadio_.isSelected());
   }

   private class ButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         updateList();
      }
   }
}
