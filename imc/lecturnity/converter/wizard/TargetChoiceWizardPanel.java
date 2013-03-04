package imc.lecturnity.converter.wizard;

import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Font;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import javax.swing.*;
import javax.swing.border.*;

import java.io.File;

import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.ui.DirectoryChooser;
import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.ProfiledSettings;
import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.filesdk.Metadata;

public class TargetChoiceWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String TARGET_DOCUMENT_NAME = "[!]";
   private static String DESTINATION = "[!]";
   private static String LOCAL_DATA_VOLUME = "[!]";
   private static String TARGET_DIRECTORY = "[!]";
   private static String DYNAMIC_TARGET_DIRECTORY = "[!]";
   private static String BROWSE = "[!]";
   private static String SERVER = "[!]";
   private static String AUTO_REPLAY = "[!]";
   private static String WARNING = "[!]";
   private static String ERROR = "[!]";
   private static String JOP_CONTINUE = "[!]";
   private static String JOP_CANCEL = "[!]";
   private static String WARNING_DIR_NOT_EMPTY = "[!]";
   private static String ERROR_NO_DOC_NAME = "[!]";
   private static String ERROR_NO_TARGET_DIR = "[!]";
   private static String ERROR_FLV_NO_OVERWRITE = "[!]";
   private static String WARN_INVALID_PATH_FILE = "[!]";
//   private static String WARN_INVALID_FILE = "[!]";
   private static String WARN_INVALID_FILE2 = "[!]";
   private static String WARN_INVALID_PATH = "[!]";
   private static String WARN_FILE_EXISTS = "[!]";
   private static String WARN_FILE_EXISTS_UPLOAD = "[!]";
   private static String WARN_FILE_EXISTS2 = "[!]";
   private static String YES = "[!]";
   private static String NO = "[!]";
//   private static String YES_ALL = "[!]";
//   private static String NO_ALL = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/TargetChoiceWizardPanel_",
             "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         TARGET_DOCUMENT_NAME = l.getLocalized("TARGET_DOCUMENT_NAME");
         DESTINATION = l.getLocalized("DESTINATION");
         LOCAL_DATA_VOLUME = l.getLocalized("LOCAL_DATA_VOLUME");
         TARGET_DIRECTORY = l.getLocalized("TARGET_DIRECTORY");
         DYNAMIC_TARGET_DIRECTORY = l.getLocalized("DYNAMIC_TARGET_DIRECTORY");
         BROWSE = l.getLocalized("BROWSE");
         SERVER = l.getLocalized("SERVER");
         AUTO_REPLAY = l.getLocalized("AUTO_REPLAY");
         WARNING = l.getLocalized("WARNING");
         ERROR = l.getLocalized("ERROR");
         JOP_CONTINUE = l.getLocalized("JOP_CONTINUE");
         JOP_CANCEL = l.getLocalized("JOP_CANCEL");
         WARNING_DIR_NOT_EMPTY = l.getLocalized("WARNING_DIR_NOT_EMPTY");
         ERROR_NO_DOC_NAME = l.getLocalized("ERROR_NO_DOC_NAME");
         ERROR_NO_TARGET_DIR = l.getLocalized("ERROR_NO_TARGET_DIR");
         ERROR_FLV_NO_OVERWRITE = l.getLocalized("ERROR_FLV_NO_OVERWRITE");
         WARN_INVALID_PATH_FILE = l.getLocalized("WARN_INVALID_PATH_FILE");
//         WARN_INVALID_FILE = l.getLocalized("WARN_INVALID_FILE");
         WARN_INVALID_FILE2 = l.getLocalized("WARN_INVALID_FILE2");
         WARN_INVALID_PATH = l.getLocalized("WARN_INVALID_PATH");
         WARN_FILE_EXISTS = l.getLocalized("WARN_FILE_EXISTS");
         WARN_FILE_EXISTS_UPLOAD = l.getLocalized("WARN_FILE_EXISTS_UPLOAD");
         WARN_FILE_EXISTS2 = l.getLocalized("WARN_FILE_EXISTS2");
         YES = l.getLocalized("YES");
         NO = l.getLocalized("NO");
//         YES_ALL = l.getLocalized("YES_ALL");
//         NO_ALL = l.getLocalized("NO_ALL");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "TargetChoiceWizardPanel\n" + 
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         System.exit(1);
      }
   }

   public static WizardPanel createInstance(PublisherWizardData pwData)
   {
      return new TargetChoiceWizardPanel(pwData);
   }

   private PublisherWizardData m_pwData;

//   private WizardPanel nextWizard_ = null;
   private TargetTypeWizardPanel m_nextTargetTypeWizard = null;
//   private LecturnityWriterWizardPanel m_nextLecturnityWriterWizard = null;
//   private RealWriterWizardPanel m_nextRealWriterWizard = null;
//   private WmpWriterWizardPanel m_nextWmpWriterWizard = null;
//   private FlashWriterWizardPanel m_nextFlashWriterWizard = null;
//   private VideoWriterWizardPanel m_nextVideoWriterWizard = null;

   private JTextField m_targetFileNameField;
   private JTextField m_targetLocalDirectoryField;
   private JRadioButton m_localTargetRadio;
   private JCheckBox m_localDynamicTargetCheckbox;
   private JRadioButton m_serverTargetRadio;
   private JLabel m_filenameLabel;
   private JLabel m_targetDirLabel;
   private JButton m_findButton;
   private JCheckBox  m_autostartCheck;

///   private PopupHelp m_helpPopup;

   private int m_iExportType;

   private boolean m_bIsLocalReplay;
   private boolean m_bIsDynamicTargetDirectory;
   private boolean m_bEnableDynamicTargetDirectory;
   private static boolean m_bDoOverwrite;


   public TargetChoiceWizardPanel(PublisherWizardData pwData)
   {
      super();

      m_pwData = pwData;
      
      m_iExportType = m_pwData.GetProfiledSettings().GetIntValue("iExportType");

      m_bIsLocalReplay 
         = (m_pwData.GetProfiledSettings().GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL);
            
      String strTargetDirLocal = m_pwData.GetProfiledSettings().GetStringValue("strTargetDirLocal");

      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      // enable dynamic target directory field for default and generic profiles
      boolean bIsDefaultProfile = ps.IsDefaultProfile();
      boolean bIsOldFlash = ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_NORMAL;
      boolean bIsPspFlash = ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_PSP;
      boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && ps.GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
      if ( bIsOldFlash || bIsPspFlash )
          // Generic Profiles cannot handle old Flash player and PSP because size computation depends on document type
          // hence type will be overwritten in ConverterWizard.wizardFinished() before writing to file
          // required here to disable dynamic target directory, which is not supported for old style profiles
          bIsGenericProfile =  false;

      m_bEnableDynamicTargetDirectory = bIsGenericProfile || bIsDefaultProfile;
      if ( m_bEnableDynamicTargetDirectory )
          m_bIsDynamicTargetDirectory = m_bIsLocalReplay && strTargetDirLocal.length() == 0;
      
      m_bDoOverwrite = false;

      initGui();
   }
   public String getDescription()
   {
      return HEADER;
   }

   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      String strTargetDocName = m_targetFileNameField.getText();

      String strTargetDirLocal = new File(m_targetLocalDirectoryField.getText()) + "";
      if (!strTargetDirLocal.endsWith(File.separator))
         strTargetDirLocal += File.separatorChar;

      ps.SetStringValue("strTargetPathlessDocName", strTargetDocName);
      ps.SetBoolValue("bDoAutostart", m_autostartCheck.isSelected());

      m_bIsLocalReplay = m_localTargetRadio.isSelected();
      if (m_bIsLocalReplay)
      {
         // "Local data volume" selected

         // Bugfix 5260: set empty path if using dynamic path (default profiles only)
         if(/*ps.IsDefaultProfile() &&*/ m_localDynamicTargetCheckbox.isSelected())
            strTargetDirLocal = "";

         ps.SetIntValue("iReplayType", PublisherWizardData.REPLAY_LOCAL);         
         ps.SetStringValue("strTargetDirLocal", strTargetDirLocal);

         m_pwData.finishWizardPanel = SelectPresentationWizardPanel.GetWriterWizardPanel(m_pwData);
         m_pwData.nextWizardPanel = null;
      }
      else
      {
         // "Server" selected

         ps.SetIntValue("iReplayType", PublisherWizardData.REPLAY_SERVER);

         if (m_nextTargetTypeWizard == null)
            m_nextTargetTypeWizard = new TargetTypeWizardPanel(m_pwData);
         m_pwData.nextWizardPanel = m_nextTargetTypeWizard;
         m_pwData.finishWizardPanel = null;
      }

      return m_pwData;
   }

   public int displayWizard()
   {
      return super.displayWizard();
   }
   
   public void refreshDataDisplay()
   {
      super.refreshDataDisplay();

      if (m_bIsLocalReplay)
      {
         m_localTargetRadio.setSelected(true);
         m_localDynamicTargetCheckbox.setSelected(m_bIsDynamicTargetDirectory);
      }
      else
         m_serverTargetRadio.setSelected(true);

      checkEnabled();
   }

   public boolean verifyQuit()
   {
      if (m_pwData.m_bIsProfileMode)
      {
         if (!verifyNext())
            return false;
      }
      
      if (m_pwData.m_bIsUploadSessionRunning)
      {
         UploaderAccess.FinishSession();
         m_pwData.m_bIsUploadSessionRunning = false;
      }

      return true;
   }

   public boolean verifyNext()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
            
      String strDocName = m_targetFileNameField.getText();
      if (strDocName.equals("") && !m_pwData.m_bIsProfileMode)
      {
         JOptionPane.showMessageDialog
            (this, ERROR_NO_DOC_NAME, ERROR,
             JOptionPane.ERROR_MESSAGE);
         return false;
      }

      // Check for not supported (non ASCII) characters in document name
      // (this check detects a blank as a non URL-compliant character)
      boolean bIsFileUrlOk = FileUtils.checkUrlFileName(strDocName);
      // Bugfix 5058: allow blank as a URL-compliant character for local replay
      boolean bIsFileUrlWithBlanksOk = FileUtils.checkUrlFileNameAndAllowBlanks(strDocName);

      // Server (non local) mode: blanks in file name are not allowed.
      if (!bIsFileUrlOk && !m_bIsLocalReplay)
      {
         Object[] options = {YES, NO};
         int res = 
            JOptionPane.showOptionDialog(this, WARN_INVALID_FILE2, WARNING, 
                                         JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, 
                                         null, options, options[1]);
         if (res != 0) // Not 'yes'
            return false;
      }
      
      // Local mode: blanks in file name are allowed. Additional check for path required.
      if (m_bIsLocalReplay)
      {
         // BUGFIX 5820: no path checks required in case of dynamic path 
         if ( m_localDynamicTargetCheckbox.isSelected() )
            return true;
             
         String strTargetLocal = m_targetLocalDirectoryField.getText();
         if (strTargetLocal.equals(""))
         {
            JOptionPane.showMessageDialog
               (this, ERROR_NO_TARGET_DIR, ERROR,
                JOptionPane.ERROR_MESSAGE);
            return false;
         }

         boolean bIsPathUrlOk = FileUtils.checkUrlPathName(strTargetLocal);
         boolean bAreUrlsOk = bIsFileUrlWithBlanksOk && bIsPathUrlOk;
         if (!bAreUrlsOk)
         {
            String strMessage = "";
            if (bIsFileUrlWithBlanksOk && !bIsPathUrlOk)
               strMessage =WARN_INVALID_PATH;
            if (!bIsFileUrlWithBlanksOk && bIsPathUrlOk)
               strMessage =WARN_INVALID_FILE2;
            if (!bIsFileUrlWithBlanksOk && !bIsPathUrlOk)
               strMessage =WARN_INVALID_PATH_FILE;

            Object[] options = {YES, NO};
            int res = 
               JOptionPane.showOptionDialog(this, strMessage, WARNING, 
                                            JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, 
                                            null, options, options[1]);
            if (res != 0) // Not 'yes'
               return false;
         }

             if (!FileUtils.checkDirectory(this, strTargetLocal))
                 return false;

         // Overwrite check: does the main file already exist?
         if (!IsTargetWritable(this, m_pwData, strTargetLocal, strDocName))
            return false;
            
         // m_bDoOverwrite is set with side effect in IsTargetWritable()
         if (m_bDoOverwrite)
         {
            if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH)
            {
               getWizardData();
               // sets "strTargetDirLocal" in m_pwData
               // OR: sets "strTargetDirHttpServer" and maybe "strTargetDirStreamingServer" 
               // and sets "strTargetPathlessDocName"
               
               if (!CheckFlvOverwrite(m_pwData, this))
                  return false;
            }
            
            m_bDoOverwrite = false;
            return true; // skip further overwrite checks if once answered with "yes"
         }

         // "Target dir not empty" check
         if(FileUtils.isNotEmptyDir(strTargetLocal) && !m_pwData.m_bIsProfileMode)
         {
            if ( (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_REAL) 
                 || (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_WMT) 
                 || (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH) )
            {
               String[] buttonStrings = {JOP_CONTINUE, JOP_CANCEL};
               int iResult = JOptionPane.showOptionDialog(
                  this, WARNING_DIR_NOT_EMPTY,
                  WARNING, JOptionPane.YES_NO_OPTION, 
                  JOptionPane.WARNING_MESSAGE, 
                  null, buttonStrings, JOP_CANCEL);
               
               if (iResult != JOptionPane.YES_OPTION)
                  return false;
            }
         }
         
      }

      return true;
   }
   
   public static boolean CheckFlvOverwrite(PublisherWizardData data, WizardPanel parent)
   {
      // BUGFIX #4332: Check flv output files specifically for overwrite 
      // (can be blocked by browser).
      // A normal File.canWrite() does not work in this case.
      
      ProfiledSettings ps = data.GetProfiledSettings();
      
      if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH)
      {
         if (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL
             || ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_NETWORK_DRIVE)
         {       
            if (ps.GetStringValue("strTargetPathlessDocName") == null ||
                ps.GetStringValue("strTargetPathlessDocName").length() == 0)
            {
               System.err.println("TargetChoiceWizardPanel: Unexpected error: Data for CheckFlvOverwrite() invalid!");
               return true; // but do not fail
            }
         
            String[] aFlvOutputNames = 
               FlashWriterWizardPanel.GetFlvFilenames(data, ps.GetStringValue("strTargetPathlessDocName"));
       
            boolean bOverwriteable = true;
         
            if (aFlvOutputNames != null)
            {
               for (int i=0; i<aFlvOutputNames.length; ++i)
               {
                  if (aFlvOutputNames[i] != null && !LPLibs.flvCheckOverwrite(aFlvOutputNames[i]))
                     bOverwriteable = false;
               }

               // Special case 'New Flex Player': FLVs are written to a "content/" subfolder
               // If "content/" does not exist then the check above must fail 
               if (!bOverwriteable && (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX))
               {
                  // Make a re-check
                  bOverwriteable = true;
                  for (int i=0; i<aFlvOutputNames.length; ++i)
                  {
                     if (aFlvOutputNames[i] != null)
                     {
                        // --> Set bOverwriteable only to 'false' if "content/" exists and the check fails
                        String strPath = FileUtils.extractPathFromPathFile(aFlvOutputNames[i]);
                        boolean bPathEndsWithContent = strPath.endsWith(FlashWriterWizardPanel.STR_CONTENT + File.separatorChar);
                        if (!bPathEndsWithContent && !LPLibs.flvCheckOverwrite(aFlvOutputNames[i]))
                           bOverwriteable = false;
                     }
                  }
               } 

            }
         
            if (!bOverwriteable)
            {
               JOptionPane.showMessageDialog(parent, ERROR_FLV_NO_OVERWRITE, ERROR,
                                             JOptionPane.ERROR_MESSAGE);
               
               return false;
            }
         }
      }
      
      return true;
   }

   public static boolean IsTargetWritable(Component parent, PublisherWizardData pwData, 
                                          String strTargetPath, String strDocName)
   {
      return IsTargetWritable(parent, pwData, strTargetPath, strDocName, false);
   }

   public static boolean IsTargetWritable(Component parent, PublisherWizardData pwData, 
                                          String strTargetPath, String strDocName, 
                                          boolean bIsUpload)
   {
      int hr = 0;

      ProfiledSettings ps = pwData.GetProfiledSettings();

      char cSeparator = File.separatorChar;
      if (bIsUpload) // which means that UNIX/Linux paths are used
         cSeparator = '/';

      if (strTargetPath.length() > 0) 
      {
         if (strTargetPath.charAt(strTargetPath.length()-1) != cSeparator)
            strTargetPath = strTargetPath + cSeparator;
      }
      //else
      //   strTargetPath = "" + cSeparator;

      if (bIsUpload)
      {
         boolean bUsePodcastServer = 
            ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER && 
            ps.GetIntValue("iServerType") == PublisherWizardData.SRV_PODCAST_SERVER;
         boolean bUseStreamingServer = 
            ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER && 
            ps.GetIntValue("iServerType") == PublisherWizardData.SRV_STREAMING_SERVER;
		 boolean bUseYoutube = ps.GetIntValue("iServerType") == PublisherWizardData.SRV_YOUTUBE;

         if (!pwData.m_bIsUploadSessionRunning)
         {
            // Check Upload server first
            UploaderAccess.StartSession();
			if (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_SLIDESTAR)
				UploaderAccess.IsSlidestar(true);
			else
				UploaderAccess.IsSlidestar(false);
            pwData.m_bIsUploadSessionRunning = true;

			if (bUseYoutube)
			{
				DocumentData dd = pwData.GetDocumentData();
				Metadata metadata = dd.GetMetadata();
				String strRecLength = metadata.recordLength;
				hr = UploaderAccess.CheckYtUpload(ps, strRecLength, (WizardPanel)parent);
			}
			else
				hr = UploaderAccess.CheckUpload(true, ps, (WizardPanel)parent);
         
            // both first and second server must be set for podcast or streaming server
            if (hr == 0 && bUsePodcastServer)
               hr = UploaderAccess.CheckUpload(false, ps, (WizardPanel)parent);
            if (hr == 0 && bUseStreamingServer)
               hr = UploaderAccess.CheckUpload(false, ps, (WizardPanel)parent);
         }
      }

      boolean bIsWritable = true;
      boolean bFileExists = false;

      if (hr == 0)
      {
         String strPathDocName = strTargetPath + strDocName; // without suffix
         //System.out.println(">> strPathDocName: " + strPathDocName);
         String strFullPathDocName = strPathDocName; // with suffix

         if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH)
         {
            if (ps.GetBoolValue("bScormEnabled"))
            {
               strFullPathDocName = strPathDocName + ".zip";
               if (bIsUpload)
                  bFileExists = UploaderAccess.DoesTargetFileExist(
                     strFullPathDocName, 
                     true, ps, (WizardPanel)parent);
               else
                  bFileExists = FileUtils.doesTargetFileExist(strFullPathDocName);
            }
            else
            {
               // New Flash Player? --> Consider the "content/" subdirectory
               if (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX)
                  strPathDocName = strTargetPath + FlashWriterWizardPanel.STR_CONTENT + File.separatorChar + strDocName;
               strFullPathDocName = strPathDocName + ".swf";
               if (bIsUpload)
                  bFileExists = UploaderAccess.DoesTargetFileExist(
                     strFullPathDocName, 
                     true, ps, (WizardPanel)parent);
               else
                  bFileExists = FileUtils.doesTargetFileExist(strFullPathDocName);
            }
   
            boolean bLzpFileExists = false;
            if (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR)
            {
               strFullPathDocName = strPathDocName + ".lzp";
               if (bIsUpload)
                  bLzpFileExists = UploaderAccess.DoesTargetFileExist(
                     strFullPathDocName, 
                     true, ps, (WizardPanel)parent);
               else
                  bLzpFileExists = FileUtils.doesTargetFileExist(strFullPathDocName);
            }
            bFileExists |= bLzpFileExists;
         }
         else if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_LECTURNITY)
         {
            strFullPathDocName = strPathDocName + ".lpd";
            if (bIsUpload)
               bFileExists = UploaderAccess.DoesTargetFileExist(
                  strFullPathDocName, 
                  true, ps, (WizardPanel)parent);
            else
               bFileExists = FileUtils.doesTargetFileExist(strFullPathDocName);
         }
         else if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_VIDEO)
         {
            strFullPathDocName = strPathDocName + ".mp4";
            if (bIsUpload)
               bFileExists = UploaderAccess.DoesTargetFileExist(
                  strFullPathDocName, 
                  true, ps, (WizardPanel)parent);
            else
               bFileExists = FileUtils.doesTargetFileExist(strFullPathDocName);
         }
         else // RM or WM export
         {
            if (ps.GetBoolValue("bScormEnabled"))
            {
               strFullPathDocName = strPathDocName + ".zip";
               if (bIsUpload)
                  bFileExists = UploaderAccess.DoesTargetFileExist(
                     strFullPathDocName, 
                     true, ps, (WizardPanel)parent);
               else
                  bFileExists = FileUtils.doesTargetFileExist(strFullPathDocName);
            }
            else
            {
               strFullPathDocName = strPathDocName + ".html";
               if (bIsUpload)
                  bFileExists = UploaderAccess.DoesTargetFileExist(
                     strFullPathDocName,
                     true, ps, (WizardPanel)parent);
               else
                  bFileExists = FileUtils.doesTargetFileExist(strFullPathDocName);
            }
         }
         
         if (bFileExists)
         {
            String strMessage = WARN_FILE_EXISTS + strFullPathDocName + WARN_FILE_EXISTS2;
            if (bIsUpload)
            {
               File fileName = new File(strFullPathDocName);
               strMessage = WARN_FILE_EXISTS_UPLOAD + fileName.getName() + WARN_FILE_EXISTS2;
            }

            Object[] options = {YES, NO};
            int res = 
               JOptionPane.showOptionDialog(parent, strMessage, WARNING, 
                                            JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, 
                                            null, options, options[1]);

            if (res != 0) // Not 'yes'
            {
               m_bDoOverwrite = false;
               pwData.m_bOverwriteAll = false;
               bIsWritable = false;
            }
            else
            {
               m_bDoOverwrite = true;
               pwData.m_bOverwriteAll = true;
               bIsWritable = true;
            }
         }
      }
      else // hr != 0
      {
         if (hr > 0)
         {
            // "Cancel" was pressed
            bIsWritable = false;
            // Finish Uploader Session
            if (pwData.m_bIsUploadSessionRunning)
			   {
				   UploaderAccess.FinishSession();
				   pwData.m_bIsUploadSessionRunning = false;
			   }
         }
         else
         {
            System.out.println("!!! Upload error in TargetChoiceWizardPanel::IsTargetWritable()!");

            bIsWritable = false;

            // Finish Uploader Session
            UploaderAccess.FinishSession();
            pwData.m_bIsUploadSessionRunning = false;

            int res = UploaderAccess.ShowErrorMessageForErrorCode(hr, (WizardPanel)parent);

            if (res == 0) // "Verify Settings"
            {
               // Goto Start dialog ...
               pwData.B_GOTO_START_DIALOG = true;
               // "true" must be returned to finish the calls of "verifyNext()" and "getWizardData()"
               return true;
            }
            else if (res == 1) // "Retry"
            {
               // Retry ...
               pwData.B_RETRY_UPLOAD = true;
            }
            else // "Cancel" or Generic error
            {
               // do nothing special, process is aborted
            }
         }
      }

      return bIsWritable;
   }

   public static String getHeaderLogoName()
   {
      String strLogoName = "/imc/lecturnity/converter/images/lecturnity_logo.gif";
/*      
      switch (m_iExportType)
      {
         case PublisherWizardData.EXPORT_TYPE_LECTURNITY:
            strLogoName = "/imc/lecturnity/converter/images/lecturnity_logo.gif";
            break;
         case PublisherWizardData.EXPORT_TYPE_REAL:
            strLogoName = "/imc/lecturnity/converter/images/real_networks_logo.gif";
            break;
         case PublisherWizardData.EXPORT_TYPE_WMT:
            strLogoName = "/imc/lecturnity/converter/images/windowsmedia_logo.png";
            break;
         case PublisherWizardData.EXPORT_TYPE_FLASH:
            strLogoName = "/imc/lecturnity/converter/images/flash_logo.gif";
            break;
         case PublisherWizardData.EXPORT_TYPE_VIDEO:
            strLogoName = "/imc/lecturnity/converter/images/videologo.jpg";
            break;
         default:
            break;
      }
*/
      return strLogoName;
   }

   private void initGui()
   {
//      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);
//      Font hintFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);
      
      setPreferredSize(new Dimension(500, 400));

      addButtons(NEXT_BACK_CANCEL | BUTTON_FINISH | BUTTON_QUIT, m_pwData.m_bIsProfileMode);
///      addButtons(NEXT_BACK_CANCEL | BUTTON_FINISH | BUTTON_QUIT | BUTTON_HELP);
      
      useHeaderPanel(HEADER, SUBHEADER, getHeaderLogoName());

///      // help button popup thingies
///      m_helpPopup = new PopupHelp(getHelpButton(), 
///                                 "<html>" + "blubb" + "</html>");

      Container r = getContentPanel();
      r.setLayout(null);

      ButtonGroup bgTarget = new ButtonGroup();
      RadioListener rl = new RadioListener();
      TextFieldListener tfl = new TextFieldListener();

      int x = 30;
      int y = 20;
      int x1 = 20;
      int y1 = 25;

      m_filenameLabel = new JLabel(TARGET_DOCUMENT_NAME);
      m_filenameLabel.setFont(descFont);
      m_filenameLabel.setSize(440, 20);
      m_filenameLabel.setLocation(x, y);
      r.add(m_filenameLabel);

      y += 25;
      String fileName = m_pwData.GetDocumentData().GetPresentationFileName();
      fileName = FileUtils.extractNameFromPathFile(fileName);
      m_targetFileNameField = new JTextField(fileName);
      m_targetFileNameField.setSize(300, 20);
      m_targetFileNameField.setLocation(x, y);
      m_targetFileNameField.addActionListener(tfl);
      r.add(m_targetFileNameField);

      y += 30;
      JPanel destinationPanel = new JPanel();
      destinationPanel.setLayout(null);
      destinationPanel.setFont(descFont);
      destinationPanel.setSize(445, 168);
      destinationPanel.setLocation(x-3, y);
      destinationPanel.setBorder(new TitledBorder(DESTINATION));
      r.add(destinationPanel);


      m_localTargetRadio = new JRadioButton(LOCAL_DATA_VOLUME, m_bIsDynamicTargetDirectory);
      m_localTargetRadio.setFont(descFont);
      m_localTargetRadio.setSize(380, 20);
      m_localTargetRadio.setLocation(x1, y1);
      m_localTargetRadio.addActionListener(rl);
      bgTarget.add(m_localTargetRadio);
      destinationPanel.add(m_localTargetRadio);

      y1 += 25;
      m_targetDirLabel = new JLabel(TARGET_DIRECTORY);
      m_targetDirLabel.setFont(descFont);
      m_targetDirLabel.setSize(440, 20);
      m_targetDirLabel.setLocation(x1+33, y1);
      destinationPanel.add(m_targetDirLabel);

      y1 += 25;
      m_targetLocalDirectoryField = new JTextField(getPreferredPathName());
      m_targetLocalDirectoryField.setSize(250, 20);
      m_targetLocalDirectoryField.setLocation(x1+33, y1);
      m_targetLocalDirectoryField.addActionListener(tfl);
      destinationPanel.add(m_targetLocalDirectoryField);

      m_findButton = new JButton(BROWSE);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         m_findButton.setBackground(Wizard.BG_COLOR);
      m_findButton.setFont(descFont);
      m_findButton.setSize(120, 30);
      m_findButton.setLocation(x1+290, y1-5);
      m_findButton.addActionListener(new BrowseButtonListener());
      destinationPanel.add(m_findButton);

      // Bugfix 5260: support dynamic paths AND normal paths for default profiles
      // also for Generic Profiles
      m_localDynamicTargetCheckbox = new JCheckBox(DYNAMIC_TARGET_DIRECTORY, false);
      if ( m_bEnableDynamicTargetDirectory)
      {
         y1 += 25;
         m_localDynamicTargetCheckbox.setFont(descFont);
         m_localDynamicTargetCheckbox.setSize(380, 20);
         m_localDynamicTargetCheckbox.setLocation(x1+33, y1);
         m_localDynamicTargetCheckbox.addActionListener(rl);
         destinationPanel.add(m_localDynamicTargetCheckbox);
         y1 += 10;
      } else
         y1 += 35;

      y1 += 25;
      m_serverTargetRadio = new JRadioButton(SERVER, false);
      m_serverTargetRadio.setFont(descFont);
      m_serverTargetRadio.setSize(380, 20);
      m_serverTargetRadio.setLocation(x1, y1);
      m_serverTargetRadio.addActionListener(rl);
      bgTarget.add(m_serverTargetRadio);
      destinationPanel.add(m_serverTargetRadio);



      y += 180;
      m_autostartCheck = new JCheckBox(AUTO_REPLAY, 
                                      m_pwData.GetProfiledSettings().GetBoolValue("bDoAutostart"));
      m_autostartCheck.setFont(descFont);
      m_autostartCheck.setSize(440, 20);
      m_autostartCheck.setLocation(x, y);
      r.add(m_autostartCheck);
      
      refreshDataDisplay();
   }

   private void checkEnabled()
   {
      m_bIsLocalReplay = m_localTargetRadio.isSelected();
      m_bIsDynamicTargetDirectory = m_localDynamicTargetCheckbox.isSelected();

      m_targetDirLabel.setEnabled(m_bIsLocalReplay);
      m_targetLocalDirectoryField.setEnabled(m_bIsLocalReplay && !m_bIsDynamicTargetDirectory);
      m_findButton.setEnabled(m_bIsLocalReplay && !m_bIsDynamicTargetDirectory);
      m_localDynamicTargetCheckbox.setEnabled(m_bIsLocalReplay);

      if (m_bIsLocalReplay)
      {
         setButtonVisible(WizardPanel.BUTTON_NEXT, false);
         if (m_pwData.m_bIsProfileMode)
         {
            setButtonVisible(WizardPanel.BUTTON_QUIT, true);
            setButtonVisible(WizardPanel.BUTTON_FINISH, false);
            setButtonFocused(WizardPanel.BUTTON_QUIT);
         }
         else
         {
            setButtonVisible(WizardPanel.BUTTON_FINISH, true);
            setButtonVisible(WizardPanel.BUTTON_QUIT, false);
            setButtonFocused(WizardPanel.BUTTON_FINISH);
         }
      }
      else
      {
         setButtonVisible(WizardPanel.BUTTON_NEXT, true);
         setButtonVisible(WizardPanel.BUTTON_FINISH, false);
         setButtonVisible(WizardPanel.BUTTON_QUIT, false);
         setButtonFocused(WizardPanel.BUTTON_NEXT);
      }

      // "Profile mode"? --> Disable TextField "Target file name"
      if (m_pwData.m_bIsProfileMode)
      {
         m_filenameLabel.setEnabled(false);
         m_targetFileNameField.setText("");
         m_targetFileNameField.setEnabled(false);
      }
      else
      {
         m_filenameLabel.setEnabled(true);
         m_targetFileNameField.setEnabled(true);
      }

      // "Autostart" must be hidden for MP4 export
      int iExportType = m_pwData.GetProfiledSettings().GetIntValue("iExportType");
      if (iExportType == PublisherWizardData.EXPORT_TYPE_VIDEO)
         m_autostartCheck.setVisible(false);
      else
         m_autostartCheck.setVisible(true);
   }

   private String getPreferredPathName()
   {
      String strPreferredPath = "";
      
      String strTargetDirLocal = m_pwData.GetProfiledSettings().GetStringValue("strTargetDirLocal");
      if (strTargetDirLocal.length() > 0)
         strPreferredPath = strTargetDirLocal;
      else
      {
         strPreferredPath = 
            FileUtils.extractPathFromPathFile(m_pwData.GetDocumentData().GetPresentationFileName());
         // Append the "TargetDocuments" ("Upload") path
         strPreferredPath += (ProfiledSettings.STR_TARGET_DOC_DIR + File.separator);

         switch (m_iExportType)
         {
            case PublisherWizardData.EXPORT_TYPE_REAL:
               strPreferredPath += (ProfiledSettings.STR_RM_DIR + File.separator);
               break;
            case PublisherWizardData.EXPORT_TYPE_WMT:
               strPreferredPath += (ProfiledSettings.STR_WM_DIR + File.separator);
               break;
            case PublisherWizardData.EXPORT_TYPE_FLASH:
               strPreferredPath += (ProfiledSettings.STR_FLASH_DIR + File.separator);
               break;
            default:
               break;
         }
      }

      return strPreferredPath;
   }

   private void findTargetDirectory()
   {
      DirectoryChooser dirChooser = new DirectoryChooser(getFrame());

      if (!m_targetLocalDirectoryField.getText().equals(""))
         dirChooser.setSelectedDir(new File(m_targetLocalDirectoryField.getText()));
      
      int result = dirChooser.showDialog();
      if (result == DirectoryChooser.OPTION_APPROVE)
      {
         m_targetLocalDirectoryField.setText(dirChooser.getSelectedDir().toString());
      }
   }

///   protected void helpCommand()
///   {
///      m_helpPopup.popupHelp();
///   }

   private class RadioListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         checkEnabled();
      }
   }

   private class TextFieldListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         checkEnabled();
      }
   }

   private class BrowseButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         findTargetDirectory();
      }
   }

}
