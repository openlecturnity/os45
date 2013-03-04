package imc.lecturnity.converter.wizard;

import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.IOException;
import javax.swing.*;

import java.io.File;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.ui.DirectoryChooser;
import imc.lecturnity.util.wizards.WizardPanel;


public class TargetDirsWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String TARGET_DIR_WEB_SERVER = "[!]";
   private static String TARGET_DIR_STREAMING_SERVER = "[!]";
   private static String BROWSE = "[!]";
   private static String WARNING = "[!]";
   private static String ERROR = "[!]";
   private static String JOP_CONTINUE = "[!]";
   private static String JOP_CANCEL = "[!]";
   private static String WARNING_WEB_DIR_NOT_EMPTY = "[!]";
   private static String WARNING_STREAMING_DIR_NOT_EMPTY = "[!]";
   private static String WARNING_BOTH_DIRS_NOT_EMPTY = "[!]";
   private static String ERROR_NO_WEB_TARGET_DIR = "[!]";
   private static String ERROR_NO_STREAMING_TARGET_DIR = "[!]";
   private static String WARN_INVALID_PATH = "[!]";
   private static String YES = "[!]";
   private static String NO = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer("/imc/lecturnity/converter/wizard/TargetDirsWizardPanel_", "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         TARGET_DIR_WEB_SERVER = l.getLocalized("TARGET_DIR_WEB_SERVER");
         TARGET_DIR_STREAMING_SERVER = l.getLocalized("TARGET_DIR_STREAMING_SERVER");
         BROWSE = l.getLocalized("BROWSE");
         WARNING = l.getLocalized("WARNING");
         ERROR = l.getLocalized("ERROR");
         JOP_CONTINUE = l.getLocalized("JOP_CONTINUE");
         JOP_CANCEL = l.getLocalized("JOP_CANCEL");
         WARNING_WEB_DIR_NOT_EMPTY = l.getLocalized("WARNING_WEB_DIR_NOT_EMPTY");
         WARNING_STREAMING_DIR_NOT_EMPTY = l.getLocalized("WARNING_STREAMING_DIR_NOT_EMPTY");
         WARNING_BOTH_DIRS_NOT_EMPTY = l.getLocalized("WARNING_BOTH_DIRS_NOT_EMPTY");
         ERROR_NO_WEB_TARGET_DIR = l.getLocalized("ERROR_NO_WEB_TARGET_DIR");
         ERROR_NO_STREAMING_TARGET_DIR = l.getLocalized("ERROR_NO_STREAMING_TARGET_DIR");
         WARN_INVALID_PATH = l.getLocalized("WARN_INVALID_PATH");
         YES = l.getLocalized("YES");
         NO = l.getLocalized("NO");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "TargetDirsWizardPanel\n" + 
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         System.exit(1);
      }
   }

   public static WizardPanel createInstance(PublisherWizardData pwData)
   {
      return new TargetDirsWizardPanel(pwData);
   }

   private PublisherWizardData m_pwData;

//   private WizardPanel m_nextWizard = null;
   private LecturnityWriterWizardPanel m_nextLecturnityWriterWizard = null;
   private RealWriterWizardPanel m_nextRealWriterWizard = null;
   private WmpWriterWizardPanel m_nextWmpWriterWizard = null;
   private FlashWriterWizardPanel m_nextFlashWriterWizard = null;
   private VideoWriterWizardPanel m_nextVideoWriterWizard = null;

   private JLabel m_targetDirWebServerLabel;
   private JLabel m_targetDirStreamingServerLabel;
   private JTextField m_targetDirWebServerTextField;
   private JTextField m_targetDirStreamingServerTextField;
   private JButton m_findDirWebServerButton;
   private JButton m_findDirStreamingServerButton;
   
   private int m_iExportType;
   private boolean m_bUseStreamingServer;

   public TargetDirsWizardPanel(PublisherWizardData pwData)
   {
      super();

      m_pwData = pwData;

      m_iExportType = m_pwData.GetProfiledSettings().GetIntValue("iExportType");

      initGui();
   }
   
   public String getDescription()
   {
      return "";
   }

   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      if (m_bUseStreamingServer)
      {
         String strTargetStreamingServer = new File(m_targetDirStreamingServerTextField.getText()) + "";
         if (!strTargetStreamingServer.endsWith(File.separator))
            strTargetStreamingServer += File.separatorChar;
         ps.SetStringValue("strTargetDirStreamingServer", strTargetStreamingServer);
      }
      String strTargetWebServer = new File(m_targetDirWebServerTextField.getText()) + "";
      if (!strTargetWebServer.endsWith(File.separator))
         strTargetWebServer += File.separatorChar;
      ps.SetStringValue("strTargetDirHttpServer", strTargetWebServer);

      // Set the target filename (full path and name)
      String strTargetDocName = ps.GetStringValue("strTargetPathlessDocName");

      m_pwData.finishWizardPanel = SelectPresentationWizardPanel.GetWriterWizardPanel(m_pwData);
      m_pwData.nextWizardPanel = null;

      return m_pwData;
   }

   public int displayWizard()
   {
      return super.displayWizard();
   }
   
   public void refreshDataDisplay()
   {
      super.refreshDataDisplay();
   
      m_bUseStreamingServer = 
         (m_pwData.GetProfiledSettings().GetIntValue("iServerType") == PublisherWizardData.SRV_STREAMING_SERVER);

      checkEnabled();
   }
   
   public boolean verifyQuit()
   {
      if (m_pwData.m_bIsProfileMode)
      {
         if (!verifyNext())
            return false;
      }
      
      return true;
   }
   
   public boolean verifyNext()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      String strTargetWebServer = m_targetDirWebServerTextField.getText();
      if (strTargetWebServer.equals(""))
      {
         JOptionPane.showMessageDialog
            (this, ERROR_NO_WEB_TARGET_DIR, ERROR,
             JOptionPane.ERROR_MESSAGE);
         return false;
      }

      boolean bIsPathUrlOk = FileUtils.checkUrlPathName(strTargetWebServer);
      if (!bIsPathUrlOk )
      {
         Object[] options = {YES, NO};
         int res = 
            JOptionPane.showOptionDialog(this, WARN_INVALID_PATH, WARNING, 
                                         JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, 
                                         null, options, options[1]);
         if (res != 0) // Not 'yes'
            return false;
      }

      if (!FileUtils.checkDirectory(this, strTargetWebServer))
          return false;

      boolean bIsNotEmtyDirOnTargetWebServer = false;
      boolean bIsNotEmtyDirOnTargetStreamingServer = false;

      bIsNotEmtyDirOnTargetWebServer = FileUtils.isNotEmptyDir(strTargetWebServer);

      String strTargetStreamingServer = "";
      if (m_bUseStreamingServer)
      {
         strTargetStreamingServer = m_targetDirStreamingServerTextField.getText();
         if (strTargetStreamingServer.equals(""))
         {
            JOptionPane.showMessageDialog
               (this, ERROR_NO_STREAMING_TARGET_DIR, ERROR,
                JOptionPane.ERROR_MESSAGE);
            return false;
         }

         boolean bIsPath2UrlOk = FileUtils.checkUrlPathName(strTargetStreamingServer);
         boolean bAreUrlsOk = bIsPathUrlOk && bIsPath2UrlOk;
         if (!bAreUrlsOk)
         {
            Object[] options = {YES, NO};
            int res = 
               JOptionPane.showOptionDialog(this, WARN_INVALID_PATH, WARNING, 
                                            JOptionPane.YES_NO_OPTION, JOptionPane.ERROR_MESSAGE, 
                                            null, options, options[1]);
            if (res != 0) // Not 'yes'
               return false;
         }

         if (!FileUtils.checkDirectory(this, strTargetStreamingServer))
             return false;

         bIsNotEmtyDirOnTargetStreamingServer = FileUtils.isNotEmptyDir(strTargetStreamingServer);
      }

      // Overwrite check: does the main file already exist?
      String strDocName = ps.GetStringValue("strTargetPathlessDocName");

      boolean bIsWebServerFileWritable 
         = TargetChoiceWizardPanel.IsTargetWritable(this, m_pwData, strTargetWebServer, strDocName);
      if (!bIsWebServerFileWritable)
         return false;

      // "Target dir not empty" check
      String strWarningNotEmpty = "";
      if (bIsNotEmtyDirOnTargetWebServer)
      {
         strWarningNotEmpty = WARNING_WEB_DIR_NOT_EMPTY;
         if (bIsNotEmtyDirOnTargetStreamingServer)
            strWarningNotEmpty = WARNING_BOTH_DIRS_NOT_EMPTY;
      }
      if (bIsNotEmtyDirOnTargetStreamingServer)
      {
         strWarningNotEmpty = WARNING_STREAMING_DIR_NOT_EMPTY;
         if (bIsNotEmtyDirOnTargetWebServer)
            strWarningNotEmpty = WARNING_BOTH_DIRS_NOT_EMPTY;
      }

      if ((bIsNotEmtyDirOnTargetWebServer || bIsNotEmtyDirOnTargetStreamingServer) && !m_pwData.m_bIsProfileMode)
      {
         if ( (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_REAL) 
              || (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_WMT) 
              || (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH) )
         {
            String[] buttonStrings = {JOP_CONTINUE, JOP_CANCEL};
            int iResult = JOptionPane.showOptionDialog(
               this, strWarningNotEmpty,
               WARNING, JOptionPane.YES_NO_OPTION, 
               JOptionPane.WARNING_MESSAGE, 
               null, buttonStrings, JOP_CANCEL);
            
            if (iResult == JOptionPane.NO_OPTION)
            return false;
         }
      }
      
      if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH)
      {
         getWizardData();
         // sets "strTargetDirLocal" in m_pwData
         // OR: sets "strTargetDirHttpServer" and maybe "strTargetDirStreamingServer" 
         // and sets "strTargetPathlessDocName"
         
         if (!TargetChoiceWizardPanel.CheckFlvOverwrite(m_pwData, this)) // also displays error
            return false;
      }

      return true;
   }
   
   private void initGui()
   {
      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);
      
      setPreferredSize(new Dimension(500, 400));

      addButtons(BACK_FINISH_CANCEL | BUTTON_QUIT, m_pwData.m_bIsProfileMode);
      
      useHeaderPanel(HEADER, SUBHEADER, TargetChoiceWizardPanel.getHeaderLogoName());

      Container r = getContentPanel();
      r.setLayout(null);

      int x = 30;
      int y = 20;

      m_targetDirWebServerLabel = new JLabel(TARGET_DIR_WEB_SERVER);
      m_targetDirWebServerLabel.setFont(descFont);
      m_targetDirWebServerLabel.setSize(440, 20);
      m_targetDirWebServerLabel.setLocation(x, y);
      r.add(m_targetDirWebServerLabel);

      y += 25;
      String strTargetDirWebServer 
         = m_pwData.GetProfiledSettings().GetStringValue("strTargetDirHttpServer");
      m_targetDirWebServerTextField = new JTextField(strTargetDirWebServer);
      m_targetDirWebServerTextField.setSize(300, 20);
      m_targetDirWebServerTextField.setLocation(x, y);
      r.add(m_targetDirWebServerTextField);

      m_findDirWebServerButton = new JButton(BROWSE);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         m_findDirWebServerButton.setBackground(Wizard.BG_COLOR);
      m_findDirWebServerButton.setFont(descFont);
      m_findDirWebServerButton.setSize(120, 30);
      m_findDirWebServerButton.setLocation(x+320, y-5);
      m_findDirWebServerButton.addActionListener(new BrowseButtonListener());
      r.add(m_findDirWebServerButton);

      y += 50;
      m_targetDirStreamingServerLabel = new JLabel(TARGET_DIR_STREAMING_SERVER);
      m_targetDirStreamingServerLabel.setFont(descFont);
      m_targetDirStreamingServerLabel.setSize(440, 20);
      m_targetDirStreamingServerLabel.setLocation(x, y);
      r.add(m_targetDirStreamingServerLabel);

      y += 25;
      String strTargetDirStreamingServer 
         = m_pwData.GetProfiledSettings().GetStringValue("strTargetDirStreamingServer");
      m_targetDirStreamingServerTextField = new JTextField(strTargetDirStreamingServer);
      m_targetDirStreamingServerTextField.setSize(300, 20);
      m_targetDirStreamingServerTextField.setLocation(x, y);
      r.add(m_targetDirStreamingServerTextField);

      m_findDirStreamingServerButton = new JButton(BROWSE);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         m_findDirStreamingServerButton.setBackground(Wizard.BG_COLOR);
      m_findDirStreamingServerButton.setFont(descFont);
      m_findDirStreamingServerButton.setSize(120, 30);
      m_findDirStreamingServerButton.setLocation(x+320, y-5);
      m_findDirStreamingServerButton.addActionListener(new BrowseButtonListener());
      r.add(m_findDirStreamingServerButton);

      refreshDataDisplay();
   }

   private void checkEnabled()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      if (m_bUseStreamingServer)
      {
         m_targetDirStreamingServerLabel.setEnabled(true);
         m_targetDirStreamingServerTextField.setEnabled(true);
         m_findDirStreamingServerButton.setEnabled(true);
      }
      else
      {
         m_targetDirStreamingServerLabel.setEnabled(false);
         m_targetDirStreamingServerTextField.setEnabled(false);
         m_findDirStreamingServerButton.setEnabled(false);
      }

      if (m_pwData.m_bIsProfileMode)
      {
         setButtonVisible(WizardPanel.BUTTON_QUIT, true);
         setButtonVisible(WizardPanel.BUTTON_FINISH, false);
      }
      else
      {
         setButtonVisible(WizardPanel.BUTTON_FINISH, true);
         setButtonVisible(WizardPanel.BUTTON_QUIT, false);
      }
   }

   private String extractPathName()
   {
      String fileName = m_pwData.GetDocumentData().GetPresentationFileName();
      int lastSlash = fileName.lastIndexOf(File.separatorChar);

      String strPath = null;

      if (lastSlash >= 0)
         strPath= fileName.substring(0, lastSlash + 1);
      else
         strPath = "." + File.separator;

      return strPath;
   }

   private class BrowseButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         String strTargetDir = "";
         
         if (e.getSource() == m_findDirWebServerButton)
            strTargetDir = m_targetDirWebServerTextField.getText();
         if (e.getSource() == m_findDirStreamingServerButton)
            strTargetDir = m_targetDirStreamingServerTextField.getText();

         DirectoryChooser dirChooser = new DirectoryChooser(getFrame());
      
         if (!strTargetDir.equals(""))
            dirChooser.setSelectedDir(new File(strTargetDir));
         else
            dirChooser.setSelectedDir(new File(extractPathName()));

         int result = dirChooser.showDialog();
         if (result == DirectoryChooser.OPTION_APPROVE)
         {
            if (e.getSource() == m_findDirWebServerButton)
               m_targetDirWebServerTextField.setText(dirChooser.getSelectedDir().toString());
            if (e.getSource() == m_findDirStreamingServerButton)
               m_targetDirStreamingServerTextField.setText(dirChooser.getSelectedDir().toString());
         }
      }
   }

}