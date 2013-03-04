package imc.lecturnity.converter.wizard;

import java.awt.*;
import java.awt.event.*;

import java.io.*;

import javax.swing.*;
import javax.swing.filechooser.FileFilter;

import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.DefaultProfiles;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.DptCommunicator;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.converter.LPLibs;

import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.LecturnitySecurityException;
import imc.lecturnity.util.wizards.Wizard;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.WizardData;
import imc.lecturnity.util.ui.LayoutUtils;
import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.converter.PresentationConverter;
import imc.epresenter.filesdk.ConsistencyChecker;
import imc.epresenter.filesdk.PresentationData;
import imc.epresenter.filesdk.FileResources;

public class SelectPresentationWizardPanel extends WizardPanel implements Runnable, ActionListener
{
   private static Localizer localizer_;

   private static String CAPTION = "[!]";
   private static String HEADER = "[Select presentation]";
   private static String SUBHEADER = "[WARNING: Localizer could not be instanciated!]";
   private static String FIND = "[Find...]";
   private static char   MNEM_FIND = 'f';
   private static String SELECT = "[Please enter the name of the presentation you want to convert:]";
   private static String ERROR_MSG = "[Error]";
   private static String WARNING_MSG = "[!]";
   private static String FILE1_DESCRIPTION = "[Lecturnity Recording Document (*.lrd)]";
   private static String FILE2_DESCRIPTION = "[ePresentation Document (*.epf)]";
   private static String FILE3_DESCRIPTION = "[AOF Document (*.aof)]";
   private static String APPROVE = "[Open]";
   private static String CHOOSER_TITLE = "[Please select a Lecturnity presentation]";
   private static String WRONG_DOCTYPE1 = "[!]";
   private static String WRONG_DOCTYPE2 = "[!]";
   private static String DT_ENTERPRISE = "[!]";
   private static String DT_EVAL = "[!]";
   private static String DT_CAMPUS = "[!]";
   private static String ERR_NOMARK = "[!]";
   private static String CODEPAGE_MISMATCH = "[!]";
   private static String VIDEO_2GB = "[!]";
   private static String FILE_MISSING = "[!]";
   private static String PROFILE_LONG = "[!]";
   private static String PROFILE = "[!]";
   private static String CHANGE = "[!]";
   private static String RECORDING_DOC = "[!]";
   private static String NO_PROFILE = "[!]";
   private static String PROFILE_ERROR = "[!]";
   private static String PROFILE_TYPE_ERROR = "[!]";
   private static String ERR_GENERIC = "[!]";
   private static String WARN_INVALID_PATH = "[!]";
   private static String WARN_INVALID_FILE = "[!]";
   private static String ERR_NO_LOGO1 = "[!]";
   private static String ERR_NO_LOGO2 = "[!]";
   private static String YES = "[!]";
   private static String NO = "[!]";
   private static String WARN_LRD_NOT_FINALIZED = "[!]";
   private static String STUDIO_CANNOT_BE_FOUND = "[!]";
   private static String WARN_PATH_TOO_LONG = "[!]";
   private static String CANCEL = "[!]";
   private static String CONTINUE = "[!]";

   // Target directory type (all formats)
   private static int TARGET_DIR_LOCAL                = 0;
   private static int TARGET_DIR_HTTP                 = 1;
   private static int TARGET_DIR_STREAMING            = 2;
   private static int TARGET_DIR_UPLOAD_FIRST         = 3;
   private static int TARGET_DIR_UPLOAD_SECOND        = 4;

   static 
   {
      try
      {
         localizer_ = new Localizer
            ("/imc/lecturnity/converter/wizard/SelectPresentationWizardPanel_", "en");
         
         CAPTION = localizer_.getLocalized("CAPTION");
         HEADER = localizer_.getLocalized("HEADER");
         SUBHEADER = localizer_.getLocalized("SUBHEADER");
         FIND = localizer_.getLocalized("FIND");
         MNEM_FIND = localizer_.getLocalized("MNEM_FIND").charAt(0);
         SELECT = localizer_.getLocalized("SELECT");
         ERROR_MSG = localizer_.getLocalized("ERROR_MSG");
         WARNING_MSG = localizer_.getLocalized("WARNING_MSG");
         FILE1_DESCRIPTION = localizer_.getLocalized("FILE1_DESCRIPTION");
         FILE2_DESCRIPTION = localizer_.getLocalized("FILE2_DESCRIPTION");
         FILE3_DESCRIPTION = localizer_.getLocalized("FILE3_DESCRIPTION");
         APPROVE = localizer_.getLocalized("APPROVE");
         CHOOSER_TITLE = localizer_.getLocalized("CHOOSER_TITLE");
         WRONG_DOCTYPE1 = localizer_.getLocalized("WRONG_DOCTYPE1");
         WRONG_DOCTYPE2 = localizer_.getLocalized("WRONG_DOCTYPE2");
         DT_ENTERPRISE = localizer_.getLocalized("DT_ENTERPRISE");
         DT_EVAL = localizer_.getLocalized("DT_EVAL");
         DT_CAMPUS = localizer_.getLocalized("DT_CAMPUS");
         ERR_NOMARK = localizer_.getLocalized("ERR_NOMARK");
         CODEPAGE_MISMATCH = localizer_.getLocalized("CODEPAGE_MISMATCH");
         VIDEO_2GB = localizer_.getLocalized("VIDEO_2GB");
         FILE_MISSING = localizer_.getLocalized("FILE_MISSING");
         PROFILE_LONG = localizer_.getLocalized("PROFILE_LONG");
         PROFILE = localizer_.getLocalized("PROFILE");
         CHANGE = localizer_.getLocalized("CHANGE");
         RECORDING_DOC = localizer_.getLocalized("RECORDING_DOC");
         NO_PROFILE = localizer_.getLocalized("NO_PROFILE");
         PROFILE_ERROR = localizer_.getLocalized("PROFILE_ERROR");
         PROFILE_TYPE_ERROR = localizer_.getLocalized("PROFILE_TYPE_ERROR");
         ERR_GENERIC = localizer_.getLocalized("ERR_GENERIC");
         WARN_INVALID_PATH = localizer_.getLocalized("WARN_INVALID_PATH");
         WARN_INVALID_FILE = localizer_.getLocalized("WARN_INVALID_FILE");
         ERR_NO_LOGO1 = localizer_.getLocalized("ERR_NO_LOGO1");
         ERR_NO_LOGO2 = localizer_.getLocalized("ERR_NO_LOGO2");
         YES = localizer_.getLocalized("YES");
         NO = localizer_.getLocalized("NO");
         WARN_LRD_NOT_FINALIZED = localizer_.getLocalized("WARN_LRD_NOT_FINALIZED");
         STUDIO_CANNOT_BE_FOUND = localizer_.getLocalized("STUDIO_CANNOT_BE_FOUND");
         WARN_PATH_TOO_LONG = localizer_.getLocalized("WARN_PATH_TOO_LONG");
         CANCEL = localizer_.getLocalized("CANCEL");
         CONTINUE = localizer_.getLocalized("CONTINUE");
      }
      catch (IOException e)
      {
         System.err.println("SelectPresentationWizardPanel: Could not instanciate Localizer.");
         e.printStackTrace();
         localizer_ = null;
      }
   }
  
   private PublisherWizardData m_PublisherData;
   private WizardPanel m_SelectActionWizard = null;

   private JTextField m_txtPresentationFile;
   //private JLabel m_lblSelectedProfile;
   //private JButton m_btnChangeProfile;

   private boolean m_bCheckOnDisplay;
   private File m_LastDir = null;
   private String m_strNewTargetDocName;
   private static File m_LastTempDir = null;
   private static boolean m_bTargetIsZipOrLzp;
   private String m_strLastCheckedLrd;

   public static WizardPanel createInstance(PublisherWizardData pwData)
   {
      return new SelectPresentationWizardPanel(pwData);
   }

   public SelectPresentationWizardPanel(PublisherWizardData pwData)
   {
      super();
      
      m_PublisherData = (PublisherWizardData) pwData;
      m_bTargetIsZipOrLzp = false;
      m_strNewTargetDocName = null;
   
      setPreferredSize(new Dimension(500, 400));

      addButtons(BUTTON_NEXT | BUTTON_CANCEL | BUTTON_FINISH);
      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");
     
      int w = 440;

      getContentPanel().setLayout(null);

      JLabel lblDescSelect = new JLabel(SELECT);
      
      m_txtPresentationFile = new JTextField("");
      m_txtPresentationFile.setSize(w - 10, 22);
      m_txtPresentationFile.setPreferredSize(m_txtPresentationFile.getSize());
      TextFieldListener tfl = new TextFieldListener();
      m_txtPresentationFile.addActionListener(tfl); 

      JButton findButton = new JButton(FIND);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         findButton.setBackground(Wizard.BG_COLOR);
      findButton.setSize(findButton.getPreferredSize());
      findButton.setMnemonic(MNEM_FIND);
      findButton.addActionListener(this);
      
      JPanel pnlDummyRight = new JPanel(new BorderLayout());
      pnlDummyRight.setSize(w - 10, findButton.getHeight());
      pnlDummyRight.setPreferredSize(pnlDummyRight.getSize());
      pnlDummyRight.add(findButton, BorderLayout.EAST);
      
      
      FlowLayout fl = new FlowLayout();
      fl.setAlignment(FlowLayout.LEFT);
      JPanel pnlSelectLrd = new JPanel(fl);
      pnlSelectLrd.setLocation(20, 20);
      pnlSelectLrd.setSize(w + 20, 120);    
      pnlSelectLrd.setBorder(BorderFactory.createCompoundBorder(
         BorderFactory.createTitledBorder(RECORDING_DOC), BorderFactory.createEmptyBorder(0, 5, 0, 5)));
         
      pnlSelectLrd.add(lblDescSelect, "North");
      pnlSelectLrd.add(m_txtPresentationFile, "Center");
      pnlSelectLrd.add(pnlDummyRight, "South");  

         
      /* Profiles now handled in Studio
      JLabel lblDescProfile = new JLabel(PROFILE_LONG + ":");
      
      m_lblSelectedProfile = new JLabel();
      
      m_btnChangeProfile = new JButton(CHANGE);
      m_btnChangeProfile.addActionListener(this);
     
      JPanel pnlDummy2 = new JPanel(new BorderLayout());
      pnlDummy2.add("South", lblDescProfile);
      
      JPanel pnlDummy1 = new JPanel(new BorderLayout());      
      pnlDummy1.add("Center", pnlDummy2);
      pnlDummy1.add("East", m_btnChangeProfile);
      
      
      JPanel pnlDummyUpper = new JPanel(new BorderLayout(0, 5));
      pnlDummyUpper.add("North", pnlDummy1);
      pnlDummyUpper.add("Center", m_lblSelectedProfile);
      
      JPanel pnlSelectProfile = new JPanel(new BorderLayout());      
      pnlSelectProfile.setLocation(20, 160);
      pnlSelectProfile.setSize(w + 20, 90);    
      pnlSelectProfile.setBorder(BorderFactory.createCompoundBorder(
         BorderFactory.createTitledBorder(PROFILE), BorderFactory.createEmptyBorder(0, 10, 0, 10)));

      pnlSelectProfile.add("North", pnlDummyUpper);
      */
         
         
      getContentPanel().add(pnlSelectLrd);
      //getContentPanel().add(pnlSelectProfile);
   }

   public String getDescription()
   {
      String caption = CAPTION;
      return caption;
   }

   public PublisherWizardData getWizardData()
   {  
      DocumentData dd = m_PublisherData.GetDocumentData();
      ProfiledSettings ps = m_PublisherData.GetProfiledSettings();
      
      String strGivenLrd = m_txtPresentationFile.getText();
      if (m_strLastCheckedLrd != null)
         strGivenLrd = m_strLastCheckedLrd; // might be different or might be the same
      // Do not check it again with GetAutomaticFinalizedLrd();
      // this is done by verifyNext().
      
      // Do not reset here; this method is called twice
      // m_strLastCheckedLrd = null;
      
      dd.SetPresentationFileName(strGivenLrd);

      /* Profiles now handled in Studio
      ProfiledSettings.ProfileInfo pi = ProfiledSettings.GetActiveProfile();
      
      if (pi == null)
      {
      */
         // normal old (non-profile) mode
         if (m_SelectActionWizard == null)
            m_SelectActionWizard = new SelectActionWizardPanel(m_PublisherData);

         m_PublisherData.nextWizardPanel = m_SelectActionWizard;

         // Clear the target document name
         ps.SetStringValue("strTargetPathlessDocName", "");

      /* Profiles now handled in Studio         
      }
      else
      {
         boolean bSuccess = ps.Load(pi.m_id) == 0;
         
         if (bSuccess)
         {
            // BUGFIX 5135: adjust empty target directory
            DefaultProfiles.AdjustTargetNameAndDirectory(dd, ps, pi.m_id);
            
            // Profile mode
            m_PublisherData.m_bIsProfileMode = true;

            // Get the correct WriterWizardPanel
            m_PublisherData.finishWizardPanel = GetWriterWizardPanel(m_PublisherData);
            
            // Set the target document name
            if (m_strNewTargetDocName != null)
               ps.SetStringValue("strTargetPathlessDocName", m_strNewTargetDocName);
            else
            {
               String strTargetDocName = dd.GetPresentationFileName();
               strTargetDocName = FileUtils.extractNameFromPathFile(strTargetDocName);
               ps.SetStringValue("strTargetPathlessDocName", strTargetDocName);
            }
            
            // Bugfix 4272: Set the WM suffix depending on the "Advanced suffix" flag
            m_PublisherData.SetWmSuffix(ps.GetBoolValue("bWmUseAdvancedSuffix"));
            
            // Note: no need to store this new panel (like above)
            // as there is (should be) no way back once writing is finished.
         }
         else
         {
            // does not happen; is already checked in verifyNext()
            
            return null;
         }
      }
      */

      // Remember the Start Dialog (if not done already)
      m_PublisherData.B_GOTO_START_DIALOG = false;
      if (m_PublisherData.GetStartDialog() == null)
         m_PublisherData.SetStartDialog(this);

      return m_PublisherData;
   }
   
   public void actionPerformed(ActionEvent evt)
   {
      /* Profiles now handled in Studio
      if (evt.getSource() == m_btnChangeProfile)
      {
         new Thread(this, "Call to ShowManageDialog()").start();
      }
      else
      { */
         findPresentation();
      //}
   }

   public void checkConsistencyOnDisplay()
   {
      m_bCheckOnDisplay = true;
   }

//    public int displayWizard()
//    {
//       if (m_bCheckOnDisplay)
//       {
//          checkForConsistency();
//          m_bCheckOnDisplay = false;
//       }
      
//       return super.displayWizard();
//    }

   public void refreshDataDisplay()
   {
      super.refreshDataDisplay();

      m_txtPresentationFile.setText(m_PublisherData.GetDocumentData().GetPresentationFileName());
      eliminateSlashes();
      
      /* Profiles now handled in Studio
      ProfiledSettings.ProfileInfo pi = ProfiledSettings.GetActiveProfile();
      
      if (pi != null)
      {
         m_lblSelectedProfile.setText(pi.m_strTitle);
         
         setButtonVisible(BUTTON_NEXT, false);        
         setButtonVisible(BUTTON_FINISH, true);
         setButtonFocused(WizardPanel.BUTTON_FINISH);
      }
      else
      {
         m_lblSelectedProfile.setText("<" + NO_PROFILE + ">");
      */         
         setButtonVisible(BUTTON_FINISH, false);
         setButtonVisible(BUTTON_NEXT, true);        
         setButtonFocused(WizardPanel.BUTTON_NEXT);
      //}
   }
   
   protected void customCommand(String command)
   {
      
   }
   
   public void run() // for calling the blocking ShowManageDialog()
   {
      JFrame frmParent = (JFrame)SwingUtilities.windowForComponent(this);
      
      // ??? Store file name (also) somewhere else than m_PublisherData?
      String strLrdName = GetLrdFilenameFromInput();
                  
      String strAutoFinLrd = GetAutomaticFinalizedLrd(strLrdName, this);
      if (strAutoFinLrd != null) {
         strLrdName = strAutoFinLrd;
 
         ProfiledSettings.ShowFormatDialog(frmParent, strLrdName);
         
         refreshDataDisplay();
      }
      // else: does not want to continue        
   }

   public boolean verifyNext()
   {
      String strLrdName = GetLrdFilenameFromInput(); // checks input field and data
      
      if (strLrdName == null || strLrdName.length() == 0)
      {
         JOptionPane.showMessageDialog(this, FILE_MISSING, ERROR_MSG, JOptionPane.ERROR_MESSAGE);
         
         return false;
      }
      
      String strAutoFinLrd = GetAutomaticFinalizedLrd(strLrdName, this);
      if (strAutoFinLrd != null)
         strLrdName = strAutoFinLrd;
      else
         return false; // either cancelled or Editor/Studio started; do not continue
         
      m_strLastCheckedLrd = strLrdName;
      

          
      boolean bSuccess = m_PublisherData.ReadDocumentDataFromPresentationFile(strLrdName, m_PublisherData.B_GOTO_START_DIALOG);

      if (bSuccess)
      {
          /* Profiles now handled in Studio
         ProfiledSettings.ProfileInfo pi = ProfiledSettings.GetActiveProfile();
         
         if (pi != null)
         {
            ProfiledSettings psDummy = new ProfiledSettings();
            // set type the enable adjustment of default profiles while loading (for BUGFIX 4806)
            psDummy.SetType(m_PublisherData.GetDocumentData().GetDocumentType());
            int iError = psDummy.Load(pi.m_id);
               
            if (iError != 0)
            {
               JOptionPane.showMessageDialog(this, PROFILE_ERROR + " " + pi.m_id, 
                                             ERROR_MSG, JOptionPane.ERROR_MESSAGE);
               bSuccess = false;
            }
               
            if (bSuccess)
            {       
               DocumentData dd = m_PublisherData.GetDocumentData();

               // BUGFIX 5135: adjust empty target directory
               DefaultProfiles.AdjustTargetNameAndDirectory(dd, psDummy, pi.m_id);

               // check if the document type matches the profile's type
               bSuccess = CheckDocumentTypes(psDummy, dd, this, false);
  
               if (bSuccess)
               {
                  // Check the target document name
                  String strTargetPath = psDummy.GetStringValue("strTargetDirLocal");
                  String strPathDocName = dd.GetPresentationFileName();
                  String strTargetDocName = FileUtils.extractNameFromPathFile(strPathDocName);
                  // Check for not supported (RFC 1630, RFC 3986) characters in the path
                  boolean bIsPathUrlOk = FileUtils.checkUrlPathName(strTargetPath);
                  if (!bIsPathUrlOk)
                     bSuccess = ContinueAfterNonSupportedCharacterCheckInPath(this, strTargetPath);
                  if (bSuccess) {
                     // Check for not supported (non ASCII) characters in document name
                     boolean bIsFileUrlOk = FileUtils.checkUrlFileName(strTargetDocName);
                     if (!bIsFileUrlOk) {
                        String strNewTargetDocName = 
                           GetTargetFilenameAfterNonSupportedCharacterCheck(this, strTargetDocName);
                        if ((strNewTargetDocName == null) || (strNewTargetDocName.length() == 0)) {
                           bSuccess = false;
                        } else {
                           m_strNewTargetDocName = strNewTargetDocName;
                        }
                     }
                  } 
               }

               //                  // Check if target is writable
               //                  String strTargetPath = psDummy.GetStringValue("strTargetDirLocal");
               //                  String strDocName = psDummy.GetStringValue("strTargetPathlessDocName");
               //                  bSuccess 
               //                     = TargetChoiceWizardPanel.IsTargetWritable(this, m_PublisherData,
               //                                                                strTargetPath, strDocName);
            }
               
            if (bSuccess)
            {
               // PZI: prepare automated structuring of Denver documents or reset
               // TODO: maybe transferred to getWizardData()
               m_PublisherData.GetDocumentData().setAddClipStructure(psDummy.GetBoolValue("bStructuredClips"));
               m_PublisherData.GetDocumentData().updateForStructuredScreengrabbing(psDummy);
            }
               
            if (bSuccess)
            {
               // Bugfix 4160: Check for Logo image name and if the specified file exists
               // (only necessary for Flash export)
               String strLogo = psDummy.GetStringValue("strLogoImageName");
               bSuccess = ContinueAfterLogoImageCheck(this, strLogo);
            }
         }
         */
            
         /* do not do this: if no profile is selected this is the normal case...
         else
         {
         JOptionPane.showMessageDialog(this, PROFILE_ERROR, 
         ERROR_MSG, JOptionPane.ERROR_MESSAGE);
         bSuccess = false;
         }
         */
      }

      return bSuccess;

   }

   private boolean isSourceDirWritable()
   {
      try
      {
         String tempFileName = m_txtPresentationFile.getText() + ".tmp";
         File tempFile = new File(tempFileName);
         tempFile.createNewFile();
         tempFile.delete();
         return true;
      }
      catch (Exception e)
      {
//          e.printStackTrace();
         return false;
      }
   }

   private void eliminateSlashes()
   {
      char fileSep = File.separatorChar;
      String fileName = m_txtPresentationFile.getText();
      char otherSep;
      if (fileSep == '/')
         otherSep = '\\';
      else
         otherSep = '/';
      
      String newFileName = fileName.replace(otherSep, fileSep);
      m_txtPresentationFile.setText(newFileName);
   }

   public static int DetermineTargetPathsAndFiles(PublisherWizardData pwData, WizardPanel pnlParent)
   {
      if (pwData.m_bFixedOutputPathGiven)
         return 0;
   
      
      // LPD and MP4 export must be handled different from the other export formats:
      // LPD/MP4 export produces only 1 file 
      // --> no extra subdirecory is necessary, 
      // --> A new _filename_ must be found to prevent overwriting of existing files.
      //
      // In all other export formats the creation of subdirectories is necessary 
      // because many files are produced. 
      // --> A new _directory_ name must be found to prevent overwriting of existing files.

      DocumentData dd = pwData.GetDocumentData();
      ProfiledSettings ps = pwData.GetProfiledSettings();
      
      // Special case: SCORM and SLIDESTAR: In the end there is only one ZIP or LZP file resp.
      m_bTargetIsZipOrLzp
         = (ps.GetBoolValue("bScormEnabled") 
            || (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR));

      // Check, if "<SourcePath>\TargetDocuments" exists and create this subdirectory, if necessary
      // Bugfix 4218: Don't do that in 'profile mode' and 'local replay'
      // BUGFIX 5140: Direct Publish should behave like Profile mode (use different dir)
      if (! ((pwData.m_bIsProfileMode || pwData.m_bIsDirectMode) && (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL)) )
      {
         String strTargetUpload = dd.GetPresentationPath() + ps.STR_TARGET_DOC_DIR + File.separator;
         File uploadDir = new File(strTargetUpload);
         if ( !(uploadDir.exists() && uploadDir.isDirectory()) )
            uploadDir.mkdirs();
      }
      

      // LPD and MP4: different handling (because no subdirecory is required) 
      if ( (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_LECTURNITY) 
          || (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_VIDEO))
      {
         // Determine a so far unused target Document name using the Presentation Document name
         // Example: 
         // - Document name is "test".
         // --> New document name: "test", "test_l2", "test_l3", etc.
         determineNewTargetDocName(pwData, pnlParent);
      }
      else
      {
         // Determine empty target directories using the Presentation Document name
         // Example: 
         // - Document name is "test".
         // - Original target directory is "/var/www/lec/".
         // --> New target directory: "/var/www/lec/test/", "/var/www/lec/test_l2/", "/var/www/lec/test_l3/", etc.

         // Consider special case: ZIP/LZP 
         if (m_bTargetIsZipOrLzp)
         {
            determineNewTargetDocName(pwData, pnlParent);
         }
         else
         {
            String strAppendedPath = "";

            if (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL)
            {
               // Local Path
               strAppendedPath = createAndGetAppendPathForTargetType(pwData, TARGET_DIR_LOCAL, pnlParent);
            }
            else
            {
               // Server mode
               if (ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_NETWORK_DRIVE)
               {
                  // HTTP Path
                  strAppendedPath = createAndGetAppendPathForTargetType(pwData, TARGET_DIR_HTTP, pnlParent);
               }
               else // TRANSFER_UPLOAD
               {
                  // Upload Path (First)
                  strAppendedPath = createAndGetAppendPathForTargetType(pwData, TARGET_DIR_UPLOAD_FIRST, pnlParent);
               }
   
               // HTTP URL
               String strTargetUrlHttp = ps.GetStringValue("strTargetUrlHttpServer");
               if (!strTargetUrlHttp.endsWith("/"))
                  strTargetUrlHttp = strTargetUrlHttp + "/";
               // Replace any "\" in 'strAppendedPath' by "/"
               strAppendedPath = strAppendedPath.replace('\\', '/');
               strTargetUrlHttp = strTargetUrlHttp + strAppendedPath;
               ps.SetStringValue("strTargetUrlHttpServer", strTargetUrlHttp);
               //System.out.println(">>> strTargetUrlHttp: " + strTargetUrlHttp);
                     
               if (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_STREAMING_SERVER)
               {
                  // Streaming URL
                  String strTargetUrlStreaming = ps.GetStringValue("strTargetUrlStreamingServer");
                  if (!strTargetUrlStreaming.endsWith("/"))
                     strTargetUrlStreaming = strTargetUrlStreaming + "/";
                  // Replace any "\" in 'strAppendedPath' by "/"
                  strAppendedPath = strAppendedPath.replace('\\', '/');
                  strTargetUrlStreaming = strTargetUrlStreaming + strAppendedPath;
                  ps.SetStringValue("strTargetUrlStreamingServer", strTargetUrlStreaming);
                  //System.out.println(">>> strTargetUrlStreaming: " + strTargetUrlStreaming);
               }
            }
         }
      }

      //System.out.println(">> strTargetDirLocal: " + ps.GetStringValue("strTargetDirLocal"));
      //System.out.println(">> strTargetDirHttpServer: " + ps.GetStringValue("strTargetDirHttpServer"));
      //System.out.println(">> strTargetDirStreamingServer: " + ps.GetStringValue("strTargetDirStreamingServer"));
      //System.out.println(">> strTargetUrlHttpServer: " + ps.GetStringValue("strTargetUrlHttpServer"));
      //System.out.println(">> strTargetUrlStreamingServer: " + ps.GetStringValue("strTargetUrlStreamingServer"));
      //System.out.println(">> strUploadFirstTargetDir: " + ps.GetStringValue("strUploadFirstTargetDir"));
      //System.out.println(">> strUploadSecondTargetDir: " + ps.GetStringValue("strUploadSecondTargetDir"));
      //System.out.println(">> GetTargetFileName(): " + ps.GetTargetFileName());

      return 0; // S_OK
   }

   public static int CheckTargetPathFileLength(PublisherWizardData pwData, WizardPanel pnlParent)
   {
      // Check if the length of the "path & file" of the target 
      // exceeds the limit of "MAX_PATH = 260" characters
      int MAX_PATH = 260;

      DocumentData dd = pwData.GetDocumentData();
      ProfiledSettings ps = pwData.GetProfiledSettings();

      // Get the length of the local target directory
      int nLocalPathLength = ps.GetStringValue("strTargetDirLocal").length();
      // Get the length of the target dirctories in case of HTTP/Streaming and Upload 
      int nHttpPathLength = ps.GetStringValue("strTargetDirHttpServer").length();
      int nStreamingPathLength = ps.GetStringValue("strTargetDirStreamingServer").length();
      int nUpload1PathLength = ps.GetStringValue("strUploadFirstTargetDir").length();
      int nUpload2PathLength = ps.GetStringValue("strUploadSecondTargetDir").length();
      // The maximum length of the above 5 paths is required
      int nPathLength = Math.max(nLocalPathLength, 
                                 Math.max(nHttpPathLength, 
                                 Math.max(nStreamingPathLength, 
                                 Math.max(nUpload1PathLength, nUpload2PathLength))));

      // Get the length ot the target document name (without suffix)
      int nFileNameLength = ps.GetStringValue("strTargetPathlessDocName").length();
      // Add the length of a suffix (e.g. ".html", ".wmv" or ".png") 
      // --> assuming a maximum of 5 characters
      int nFullFileNameLength = nFileNameLength + 5;
      // Consider Clip files "_clipXX" --> another 6-7 characters (assuming 7)
      if (dd.HasClips() && ps.GetBoolValue("bShowClipsOnSlides"))
         nFullFileNameLength += 7;
      
      
      // Which other files and subdirectories (with files) are possible?
      // --> This depends on the export type (target format)
      // Define an "ExtendedLength". 
      // At least the maximum of "nFileNameLength" and "nExtendedLength" must be considered
      int nExtendedLength = 0;
      // Check the different export types
      int nExportType = ps.GetIntValue("iExportType");
      switch (nExportType) {
         case PublisherWizardData.EXPORT_TYPE_LECTURNITY:
            // No other files/subdirectories
            break;
         case PublisherWizardData.EXPORT_TYPE_REAL:
         case PublisherWizardData.EXPORT_TYPE_WMT:
            // The longest possible Template file is "images/button_horizontalslides_passive.png"
            // which has a length of 42. 
            // On the other side we have "slides/<filename>_XXXXXXXX_thumb.png" 
            // which has a length of (nFileNameLength + 26)
            nExtendedLength = Math.max(42, (nFileNameLength + 26));
            break;
         case PublisherWizardData.EXPORT_TYPE_FLASH:
            // The longest possible file is "content/slide_xxx_thumb.png" 
            // which has a length of 27 
            // - or "content/<filename>.flv" or "content/<filename>_clipXX.flv" 
            // which has a length of (8 + nFullFileNameLength)
            nExtendedLength = Math.max(27, (8 + nFullFileNameLength));
            break;
         case PublisherWizardData.EXPORT_TYPE_VIDEO:
            // No other files/subdirectories
            break;
      }

      // Now determine the maximum length of all paths and files
      // and consider the maximum of "nFullFileNameLength" and "nExtendedLength"
      int nMaxLength = nPathLength + Math.max(nFullFileNameLength, nExtendedLength);
      
      // Does the maximum length exceed the MAX_PATH limit?
      if (nMaxLength >= MAX_PATH) {
         // Warning message about too long path
         System.out.println("*** Path and file length (" + nMaxLength + ") exeeds the maximum length of " + MAX_PATH + "!");
         String[] opt = {CANCEL, CONTINUE};
         int jres = JOptionPane.showOptionDialog(pnlParent,
                                                 WARN_PATH_TOO_LONG, 
                                                 WARNING_MSG, 
                                                 JOptionPane.DEFAULT_OPTION, 
                                                 JOptionPane.WARNING_MESSAGE, 
                                                 null, opt, opt[0]);
         if (jres == JOptionPane.NO_OPTION) {
            // "Continue" was pressed
            return 0; // S_OK
         } else {
            // Otherwise "Cancel" was pressed
            return LPLibs.CANCELLED;
         }
      }
      
      return 0; // S_OK
   }

   private static void determineNewTargetDocName(PublisherWizardData pwData, WizardPanel pnlParent)
   {
      ProfiledSettings ps = pwData.GetProfiledSettings();

      if (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL)
      {
         // Local Path
         createNewTargetDocNameForTargetType(pwData, TARGET_DIR_LOCAL, pnlParent);
      }
      else
      {
         // Server mode
         if (ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_NETWORK_DRIVE)
         {
            // HTTP Path
            createNewTargetDocNameForTargetType(pwData, TARGET_DIR_HTTP, pnlParent);
         }
         else // TRANSFER_UPLOAD
         {
            // Uploader Path
            createNewTargetDocNameForTargetType(pwData, TARGET_DIR_UPLOAD_FIRST, pnlParent);
         }
      }
   }

   private static String createAndGetAppendPathForTargetType(PublisherWizardData pwData, 
                                                             int iTargetType, 
                                                             WizardPanel pnlParent)
   {
      int hr = 0;
      DocumentData dd = pwData.GetDocumentData();
      ProfiledSettings ps = pwData.GetProfiledSettings();

      String strPresentationFileName = dd.GetPresentationFileName();
      String strDocName = FileUtils.extractNameFromPathFile(strPresentationFileName);
      File targetDir;

      // BUGFIX 5140: Direct Publish should behave like Profile mode (use different dir)
      if ((pwData.m_bIsProfileMode || pwData.m_bIsDirectMode))
      {
         // Bugfix 4091: In case of "illegal" characters this document name hcan be received from the user input
         if (ps.GetStringValue("strTargetPathlessDocName").length() > 0)
            strDocName = ps.GetStringValue("strTargetPathlessDocName");
      }
      else
      {
         // Bugfix 4073: In "Wizard mode" this document name has to be received from the user input
         strDocName = ps.GetStringValue("strTargetPathlessDocName");
      }

      // Append path: 2 different paths for local and remote target
      String strAppendedLocalPath = "";
      String strAppendedRemotePath = "";

      // A1. Subdirectory depending on the export type (RealMedia, WindowsMedia or Flash)
      // Bugfix 4267: Different local target dir in case of 'profile mode' and 'local replay'
      // BUGFIX 5140: Direct Publish should behave like Profile mode (use different dir)
      if (! ((pwData.m_bIsProfileMode || pwData.m_bIsDirectMode) && (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL)) )
         strAppendedLocalPath += ps.GetExportTypeTargetSubDir();
      if (strAppendedLocalPath.length() > 0 && !strAppendedLocalPath.endsWith(File.separator))
         strAppendedLocalPath += File.separator;
      // BUGFIX 5140: Direct Publish should behave like Profile mode (use different dir)
      if (!(pwData.m_bIsProfileMode || pwData.m_bIsDirectMode)) // In Profile mode this subdirectory is not used
      {
         strAppendedRemotePath += ps.GetExportTypeTargetSubDir();
         if (!strAppendedRemotePath.endsWith(File.separator))
            strAppendedRemotePath += File.separator;
      }
      // A2. Source Document name
      strAppendedLocalPath += strDocName;
      strAppendedRemotePath += strDocName;


      String strTargetDirLocal = "";
      // Bugfix 4267: Different local target dir in case of 'profile mode' and 'local replay'
      // BUGFIX 5140: Direct Publish should behave like Profile mode (use different dir)
      if ( ((pwData.m_bIsProfileMode || pwData.m_bIsDirectMode) && (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL)) )
      {
         strTargetDirLocal = ps.GetStringValue("strTargetDirLocal");
         targetDir = new File(strTargetDirLocal);
      }
      else
      {
         strTargetDirLocal = dd.GetPresentationPath() + ps.STR_TARGET_DOC_DIR + File.separator;
         // Create subdirectory depending on the export type (RealMedia, WindowsMedia or Flash), if necessary.
         targetDir = new File(strTargetDirLocal + ps.GetExportTypeTargetSubDir());
         if ( !(targetDir.exists() && targetDir.isDirectory()) )
            targetDir.mkdirs();
      }

      String strNewPathLocal = "";
      String strNewPathRemote = "";


      // Step 1: Determine LOCAL path
      if ( (iTargetType == TARGET_DIR_LOCAL)
           ||(iTargetType == TARGET_DIR_HTTP) 
           || (iTargetType == TARGET_DIR_UPLOAD_FIRST) )
      {
         strNewPathLocal = strAppendedLocalPath;
         strNewPathRemote = strAppendedRemotePath;

         int idxL = FileUtils.getIndexOfNextEmptyDirName(strTargetDirLocal + strAppendedLocalPath, "_l");
         if (idxL > 1)
         {
            strNewPathLocal = strAppendedLocalPath + "_l" + idxL;
            strNewPathRemote = strAppendedRemotePath + "_l" +idxL;
         }
         if (idxL < 0) // Error
            hr = -1;
            
         ps.SetStringValue("strTargetDirLocal", strTargetDirLocal + strNewPathLocal + File.separator);
         // Create subdirectory, if necessary. 
         targetDir = new File(strTargetDirLocal + strNewPathLocal + File.separator);
         if ( !(targetDir.exists() && targetDir.isDirectory()) )
            targetDir.mkdirs();

         strAppendedLocalPath = strNewPathLocal;
         strAppendedRemotePath = strNewPathRemote;
      }
      else
      {
         // For the target types TARGET_DIR_STREAMING and TARGET_DIR_UPLOAD_SECOND 
         // this is already done
      }


      // Step 2: Determine TARGET path
      // This must only be done in "Profile Mode"
      // - Otherwise: return an empty append path
      // BUGFIX 5140: Direct Publish should behave like Profile mode (use different dir)
      if (!(pwData.m_bIsProfileMode || pwData.m_bIsDirectMode))
         return "";

      String strNewPathTarget = strNewPathRemote; //strNewPathLocal;(?)
      //System.out.println(">>  strNewPathRemote: " + strNewPathRemote);

      if (iTargetType != TARGET_DIR_LOCAL)
      {
         String strTargetDirRemote = "";
         int idxT = 0;

         if (iTargetType == TARGET_DIR_HTTP)
         {
            strTargetDirRemote = ps.GetStringValue("strTargetDirHttpServer");
            //System.out.println(">>> strTargetDirRemote: " + strTargetDirRemote);
            idxT = FileUtils.getIndexOfNextEmptyDirName(strTargetDirRemote + strNewPathRemote, "_t");
            if (idxT > 1)
               strNewPathTarget = strNewPathRemote + "_t" + idxT;
            if (idxT < 0) // Error
               hr = -1;
            ps.SetStringValue("strTargetDirHttpServer", 
                               strTargetDirRemote + strNewPathTarget + File.separator);
            // Create subdirectories, if they do not exist already
            targetDir = new File(strTargetDirRemote + strNewPathTarget + File.separator);
            if ( !(targetDir.exists() && targetDir.isDirectory()) )
               targetDir.mkdirs();

            // TARGET_DIR_STREAMING
            strTargetDirRemote = ps.GetStringValue("strTargetDirStreamingServer");
            //System.out.println(">>> strTargetDirRemote: " + strTargetDirRemote);
            if (idxT > 1)
               strNewPathTarget = strNewPathRemote + "_t" + idxT;
            if (idxT < 0) // Error
               hr = -1;
            ps.SetStringValue("strTargetDirStreamingServer", 
                               strTargetDirRemote + strNewPathTarget + File.separator);
            // Create subdirectories, if they do not exist already
            targetDir = new File(strTargetDirRemote + strNewPathTarget + File.separator);
            if ( !(targetDir.exists() && targetDir.isDirectory()) )
               targetDir.mkdirs();
         }
         else if (iTargetType == TARGET_DIR_UPLOAD_FIRST)
         {
            strTargetDirRemote = ps.GetStringValue("strUploadFirstTargetDir");
            //System.out.println(">>> strTargetDirRemote: " + strTargetDirRemote);
            // Replace any "\" in 'strNewPathRemote' by "/"
            strNewPathRemote = strNewPathRemote.replace('\\', '/');
            //System.out.println(">>> strNewPathRemote: " + strNewPathRemote);
            idxT = UploaderAccess.GetIndexOfNextEmptyDirName(strTargetDirRemote + strNewPathRemote, "_t", 
                                                             true, ps, pnlParent);
            if (idxT > 1)
               strNewPathTarget = strNewPathRemote + "_t" + idxT;
            else
               strNewPathTarget = strNewPathRemote;
            if (idxT < 0) // Error
               hr = -1;
            ps.SetStringValue("strUploadFirstTargetDir", 
                              strTargetDirRemote + strNewPathTarget + "/");

            // TARGET_DIR_UPLOAD_SECOND
            strTargetDirRemote = ps.GetStringValue("strUploadSecondTargetDir");
            //System.out.println(">>> strTargetDirRemote: " + strTargetDirRemote);
            // Replace any "\" in 'strNewPathRemote' by "/"
            strNewPathRemote = strNewPathRemote.replace('\\', '/');
            //System.out.println(">>> strNewPathRemote: " + strNewPathRemote);
            if (idxT > 1)
               strNewPathTarget = strNewPathRemote + "_t" + idxT;
            else
               strNewPathTarget = strNewPathRemote;
            if (idxT < 0) // Error
               hr = -1;
            ps.SetStringValue("strUploadSecondTargetDir", 
                              strTargetDirRemote + strNewPathTarget + "/");
         }
         //System.out.println(">>> strNewTargetDir: " + strTargetDirRemote + strNewPathTarget);
         strAppendedRemotePath = strNewPathTarget;
      }

      if (!strAppendedRemotePath.endsWith(File.separator))
         strAppendedRemotePath += File.separator;

      //System.out.println(">>> strAppendedRemotePath: " + strAppendedRemotePath);
      // Store strAppendedRemotePath in the Profile Settings to use it lateron in the WriterWizardPanels
      ps.SetStringValue("strUploadAppendPath", strAppendedRemotePath);

      if (hr < 0)
      {
         // An error during "GetIndexOfNextEmpty..." had occured
         JOptionPane.showMessageDialog(pnlParent, ERR_GENERIC, ERROR_MSG, JOptionPane.ERROR_MESSAGE);
      }

      return strAppendedRemotePath;
   }

   private static void createNewTargetDocNameForTargetType(PublisherWizardData pwData, 
                                                           int iTargetType, 
                                                           WizardPanel pnlParent)
   {
      int hr = 0;
      DocumentData dd = pwData.GetDocumentData();
      ProfiledSettings ps = pwData.GetProfiledSettings();

      String strPresentationFileName = dd.GetPresentationFileName();
      String strDocName = FileUtils.extractNameFromPathFile(strPresentationFileName);

      String strSuffix = "";
      if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_LECTURNITY)
         strSuffix = ".lpd";
      if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_VIDEO)
         strSuffix = ".mp4";

      // BUGFIX 5140: Direct Publish should behave like Profile mode (use different dir)
      if ((pwData.m_bIsProfileMode || pwData.m_bIsDirectMode))
      {
         // Bugfix 4091: In case of "illegal" characters this document name hcan be received from the user input
         if (ps.GetStringValue("strTargetPathlessDocName").length() > 0)
            strDocName = ps.GetStringValue("strTargetPathlessDocName");
      }
      else
      {
         // Bugfix 4073: In "Wizard mode" this document name has to be received from the user input
         strDocName = ps.GetStringValue("strTargetPathlessDocName");
      }


      // Step 1: Determine LOCAL document name
      String strNewDocNameLocal = strDocName;

      // Get the local target directory
      String strTargetDirLocal;
      if (iTargetType == TARGET_DIR_LOCAL)
      {
         // The Local target dir is already known
         strTargetDirLocal = ps.GetStringValue("strTargetDirLocal");
         File targetDir = new File(strTargetDirLocal);
         if ( !(targetDir.exists() && targetDir.isDirectory()) )
            targetDir.mkdirs();
      }
      else
      {
         strTargetDirLocal = dd.GetPresentationPath() + ps.STR_TARGET_DOC_DIR + File.separator;
         if (m_bTargetIsZipOrLzp)
         {
            strTargetDirLocal += ps.GetExportTypeTargetSubDir();
            if (!strTargetDirLocal.endsWith(File.separator))
               strTargetDirLocal += File.separator;
         }
         ps.SetStringValue("strTargetDirLocal", strTargetDirLocal);
      }

      // Bugfix 4266: Special case: 'wizard mode' and 'network drive'/'upload': No "_lx" extension (file can be overwritten)
      // BUGFIX 5140: Direct Publish should behave like Profile mode (use different dir)
      boolean bIsWizardAndServerMode = ( (!(pwData.m_bIsProfileMode || pwData.m_bIsDirectMode)) 
                                        && (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) );
      if (!bIsWizardAndServerMode)
      {
         int idxL = FileUtils.getIndexOfNextEmptyFileName(strTargetDirLocal + strDocName + strSuffix, "_l");
         if (idxL > 1)
            strNewDocNameLocal = strDocName + "_l" + idxL;
         if (idxL < 0) // Error
            hr = -1;
      }


      // Step 2: Determine TARGET document name
      // This must only be done in "Profile Mode"
      // - Otherwise: Consider special case (see below)
      // BUGFIX 5140: Direct Publish should behave like Profile mode (use different dir)
      if (!(pwData.m_bIsProfileMode || pwData.m_bIsDirectMode) || m_bTargetIsZipOrLzp)
      {
         // Special case: Use this local document name as a temporary local directory name
         String strTempLocalDir = strTargetDirLocal + strNewDocNameLocal + File.separator;
         m_LastTempDir = new File(strTempLocalDir);
         // Create this subdir, if it does not exist
         if (! (m_LastTempDir.exists() && m_LastTempDir.isDirectory()) )
            m_LastTempDir.mkdirs();
         ps.SetStringValue("strTargetDirLocal", strTempLocalDir);
         return;
      }

      String strNewDocNameTarget = strNewDocNameLocal;

      if (iTargetType != TARGET_DIR_LOCAL)
      {
         String strTargetDirRemote = "";
         int idxT = 0;

         if (iTargetType == TARGET_DIR_HTTP)
         {
            strTargetDirRemote = new File(ps.GetStringValue("strTargetDirHttpServer")) + "";
            if (!strTargetDirRemote.endsWith(File.separator))
               strTargetDirRemote += File.separator;
            idxT = FileUtils.getIndexOfNextEmptyFileName(strTargetDirRemote + strNewDocNameLocal + strSuffix, "_t");

            // TARGET_DIR_STREAMING
            strTargetDirRemote = new File(ps.GetStringValue("strTargetDirStreamingServer")) + "";
            if (!strTargetDirRemote.endsWith(File.separator))
               strTargetDirRemote += File.separator;
         }
         else if (iTargetType == TARGET_DIR_UPLOAD_FIRST)
         {
            strTargetDirRemote = ps.GetStringValue("strUploadFirstTargetDir");
            if (strTargetDirRemote.length() > 0)
            {
               if (strTargetDirRemote.charAt(strTargetDirRemote.length()-1) != '/')
                  strTargetDirRemote += '/';
            }
            else
               strTargetDirRemote = "" + '/' ;
            idxT = UploaderAccess.GetIndexOfNextEmptyFileName(strTargetDirRemote + strNewDocNameLocal + strSuffix, "_t", 
                                                              true, ps, pnlParent);

            // TARGET_DIR_UPLOAD_SECOND
            strTargetDirRemote = ps.GetStringValue("strUploadSecondTargetDir");
            if (strTargetDirRemote.length() > 0)
            {
               if (strTargetDirRemote.charAt(strTargetDirRemote.length()-1) != '/')
                  strTargetDirRemote += '/';
            }
            else
               strTargetDirRemote = "" + '/' ;
         }

         if (idxT > 1)
         {
            strNewDocNameTarget = strNewDocNameLocal + "_t" + idxT;

            // Special case: Use the local document name as a temporary local directory name
            String strTempLocalDir = strTargetDirLocal + strNewDocNameLocal + File.separator;
            m_LastTempDir = new File(strTempLocalDir);
            // Create this subdir, if it does not exist
            if (! (m_LastTempDir.exists() && m_LastTempDir.isDirectory()) )
               m_LastTempDir.mkdirs();
            ps.SetStringValue("strTargetDirLocal", strTempLocalDir);
         }
         if (idxT < 0) // Error
            hr = -1;
      }

      //System.out.println(">>> strNewDocNameTarget: " + strNewDocNameTarget);
      ps.SetStringValue("strTargetPathlessDocName", strNewDocNameTarget);

      if (hr < 0)
      {
         // An error during "GetIndexOfNextEmpty..." had occured
         JOptionPane.showMessageDialog(pnlParent, ERR_GENERIC, ERROR_MSG, JOptionPane.ERROR_MESSAGE);
      }
   }

   public static int RemoveTempLocalDirForSingleTargetFile(PublisherWizardData pwData, String strSuffix)
   {
      System.out.println("+RemoveTempLocalDirForSingleTargetFile()");
      int result = 0;

      DocumentData dd = pwData.GetDocumentData();
      ProfiledSettings ps = pwData.GetProfiledSettings();

      if (strSuffix.equals(""))
      {
         // TODO: error message?
         return -1;
      }

      if (m_LastTempDir != null)
      {
         String strSourcePath = m_LastTempDir + "";
         if (!strSourcePath.endsWith(File.separator))
            strSourcePath += File.separator;
         String strSource = strSourcePath + ps.GetStringValue("strTargetPathlessDocName") + strSuffix;
         String strSourceName = m_LastTempDir.getName();

         String strTargetPath = dd.GetPresentationPath() + ps.STR_TARGET_DOC_DIR + File.separator;
         // Special case: ZIP and LZP: Append "Flash|RealMedia|WindowsMedia\"
         if (strSuffix.equals(".zip") || strSuffix.equals(".lzp"))
         {
            strTargetPath += ps.GetExportTypeTargetSubDir();
            if (!strTargetPath.endsWith(File.separator))
               strTargetPath += File.separator;
         }
         String strTarget = strTargetPath + strSourceName + strSuffix;
         
         try
         {
            // Copy the file from the temporary dir back to STR_TARGET_DOC_DIR
            FileUtils.copyFile(strSource, strTarget, null, "Copying...");
            System.out.println(" Copying " + strSource + " --> " + strTarget);
            // Remove the source file
            File fileSource = new File(strSource);
            fileSource.delete();

            // Interactive Opened Files?
            if (pwData.m_aListOfInteractiveOpenedFiles != null)
            {
               int len = pwData.m_aListOfInteractiveOpenedFiles.length;
               for (int i = 0; i < len; ++i)
               {
                  strSource = strSourcePath + pwData.m_aListOfInteractiveOpenedFiles[i];
                  strTarget = strTargetPath + pwData.m_aListOfInteractiveOpenedFiles[i];
                  // Copy the file from the temporary dir back to STR_TARGET_DOC_DIR
                  System.out.println(i + " Copying " + strSource + " --> " + strTarget);
                  FileUtils.copyFile(strSource, strTarget, null, "Copying...");
                  // Remove the source file
                  fileSource = new File(strSource);
                  fileSource.delete();
               }
            }

            // Remove the temporary local dir
            boolean bDeleted = m_LastTempDir.delete();
            if (!bDeleted)
               m_LastTempDir.deleteOnExit();
            else
               m_LastTempDir = null;
            
            // Redirect local target dir and file name
            ps.SetStringValue("strTargetDirLocal", strTargetPath);
            ps.SetStringValue("strTargetPathlessDocName", strSourceName);
         }
         catch (IOException exc)
         {
            // TODO: error message?
            exc.printStackTrace();
            return -1;
         }

      }
      else
      {
         // TODO: error message?
         return -1;
      }
      
      return result;
   }

   private void findPresentation()
   {
      JFileChooser fileChooser = new JFileChooser();
      if (m_LastDir != null)
         fileChooser.setCurrentDirectory(m_LastDir);
      else
      {
         String recPath = NativeUtils.getRecordingsPath();
         if (recPath != null)
            fileChooser.setCurrentDirectory(new File(recPath));
      }
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         fileChooser.setBackground(Wizard.BG_COLOR);
      LecturnityFileFilter lff = new LecturnityFileFilter();
      fileChooser.addChoosableFileFilter(lff);
      EPresentationFileFilter eff = new EPresentationFileFilter();
      fileChooser.addChoosableFileFilter(eff);
      AofFileFilter aff = new AofFileFilter();
      fileChooser.addChoosableFileFilter(aff);
      fileChooser.addChoosableFileFilter(fileChooser.getAcceptAllFileFilter());
      fileChooser.setFileFilter(lff);
      fileChooser.setDialogTitle(CHOOSER_TITLE);
      fileChooser.setApproveButtonText(APPROVE);
      fileChooser.setApproveButtonToolTipText(APPROVE);

      Dimension fcSize = fileChooser.getPreferredSize();
      fileChooser.setPreferredSize(new Dimension(fcSize.width + 100, fcSize.height + 50));

      int action = fileChooser.showDialog(this, APPROVE);

      m_LastDir = fileChooser.getCurrentDirectory();

      if (action == fileChooser.APPROVE_OPTION)
      {
         File selectedFile = fileChooser.getSelectedFile();
         m_txtPresentationFile.setText(selectedFile.getAbsolutePath());
//          checkForConsistency();
         m_PublisherData.GetDocumentData().SetPresentationFileName(
            selectedFile.getAbsolutePath());
      }
   }
   
   private String GetLrdFilenameFromInput()
   {
      int iDummy = 1;
      String strLrdName = m_txtPresentationFile.getText();
      String strLrdNameInData = m_PublisherData.GetDocumentData().GetPresentationFileName();
      if (strLrdName != null && !strLrdName.equals(strLrdNameInData))
         iDummy = 0; // take strLrdName; field content was changed manually
      else if (strLrdNameInData != null && strLrdNameInData.length() > 0)
         strLrdName = strLrdNameInData;
      
      return strLrdName;
   }
   
   /**
    * @returns the specified filename if no project file (.lep) was found.
    *          If there is a project file the last export is looked up and
    *          if found that lrd is returned.
    */
   // BUGFIX 5132 - make "static public" to be called from ConverterWizard
   // TODO: should be transfered to ConverterWizard or some helper class
   static public String GetAutomaticFinalizedLrd(String strLrdFilename, Component parent) {
      return GetAutomaticFinalizedLrd(strLrdFilename, parent, null);
   }
   static public String GetAutomaticFinalizedLrd(String strLrdFilename, Component parent, DptCommunicator dptCommunicator) {
      String strLrdReturn = strLrdFilename;
      
      boolean bShowDialog = (dptCommunicator == null);
 
      boolean bNeedsFinalization = NativeUtils.checkNeedsFinalization(strLrdFilename);
      if (!bNeedsFinalization) {
            
         String strNativeReturn = NativeUtils.getFinalizedPathName(strLrdFilename);
         if (strNativeReturn != null && strNativeReturn.length() > 0)
            strLrdReturn = strNativeReturn;
            
      } else { //if (bNeedsFinalization)
         // This is a normal case: document was modified but it was not
         // finalized/exported yet.

         // BUGFIX 5147: finalization message hidden by other windows
         // add a parent frame to show finalization message always on top
         JFrame jFrame = null;
         if (parent == null && bShowDialog) {
            try {
               jFrame = new JFrame("Publisher");
               jFrame.setAlwaysOnTop(true);
               jFrame.setSize(0, 0);
               ImageIcon DIALOG_ICON = new ImageIcon(jFrame.getClass().getResource(
                     "/imc/lecturnity/converter/images/LECTURNITY_Publisher.png"));
               jFrame.setIconImage(DIALOG_ICON.getImage());
               Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
               jFrame.setLocation(screenSize.width / 2, screenSize.height / 2);
               jFrame.setVisible(true);
               parent = jFrame;
            } catch (Exception e) {               
            }
         }
         
         int iAnswer;         
         if(bShowDialog) 
            iAnswer = JOptionPane.showConfirmDialog(
            parent, WARN_LRD_NOT_FINALIZED, WARNING_MSG, 
            JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.WARNING_MESSAGE);
         else
            // always YES if no dialog
            iAnswer = JOptionPane.YES_OPTION;
         
         if( jFrame != null )
            jFrame.setVisible(false);
            
         // no continues
         // yes and cancel break (return null)
         // yes starts the Editor export/finalization
         // all immediately return
         if (iAnswer == JOptionPane.NO_OPTION)
            ; // strLrdReturn stays untouched
         else {
            boolean bSuccess = false; 
            if (iAnswer == JOptionPane.YES_OPTION) {
               // start Editor/Studio
                  
               // This is the easy solution; depends on lrd being tied to Editor/Studio
               //NativeUtils.startFile(strLrdFilename);
                  
               // A bit more complex: starting with parameter which triggers automatic export 
               Runtime r = Runtime.getRuntime();
               
               String strInstallDir = NativeUtils.getRegistryValue(
                  "HKLM", "Software\\imc AG\\LECTURNITY", "InstallDir");
               
               if (strInstallDir != null && strInstallDir.length() > 0) {
                  String strStudioExe = 
                     new File(new File(strInstallDir, "Studio"), "Studio.exe").getPath();
                  if (new File(strStudioExe).exists()) {
                     try {                        
                        final Process p = r.exec(new String[] { strStudioExe, "/finalize", strLrdFilename });
                  
                        // Note the native process cannot finish if its output is unprocessed!
                        Thread t = new Thread("ProcessOutputEater") {
                           public void run() {
                              InputStream is = p.getInputStream();
                              byte[] buffer = new byte[1024];
                              try {
                                 // process/delete process output
                                 while (is.read(buffer) >= 0);
                              } catch (IOException exc) {}
                           }
                        };
                        t.start();
                        
                        int iExitValue = p.waitFor(); //p.exitValue();
                        
                        // After the process finishes there will already have been two progresses
                        // BUGFIX 5256: Finalize document fails - caused by undefined dptCommunicator in non PT mode
                        if( dptCommunicator != null )
                           dptCommunicator.ActivateProgressOffset(2.0f/3.0f);
  
                        if (iExitValue != 0) {
                           // There was an exit value but it indicates error
                           
                           System.err.println("!! Exit with error: " + strStudioExe + ": " + iExitValue);
                        } else
                           bSuccess = true;
                     } catch (IOException exc) {
                        exc.printStackTrace();
                     } catch (IllegalThreadStateException exc) {
                        // This one is alright: program started and not yet finished (with error)
                     } catch (InterruptedException exc) {
                        exc.printStackTrace();
                     }
                     
                  } else {
                     if( jFrame != null )
                        jFrame.setVisible(true);

                     if( bShowDialog )
                        JOptionPane.showMessageDialog(
                           parent, STUDIO_CANNOT_BE_FOUND + "\n" + strStudioExe,
                           ERROR_MSG, JOptionPane.ERROR_MESSAGE);
                     else
                        // no dialog mode is currently used only in Power Trainer mode
                        dptCommunicator.ShowErrorMessage(STUDIO_CANNOT_BE_FOUND + "\n" + strStudioExe);
                     
                     if( jFrame != null )
                        jFrame.setVisible(false);
                  }
               }
                  
               // More complex solution might be starting
               // the programm and monitoring its (command line) output.
               // With Process p = Runtime.getRuntime().exec(...)
            }
            // cleanup of BUGFIX 5147
            if (jFrame != null)
               jFrame.dispose();
                        
            if( bSuccess )
               // finalization performed - try again
               return GetAutomaticFinalizedLrd(strLrdFilename, parent);
               // TODO: what about infinite loop?
            else
               return null;
         }
         
      }
      
      
      return strLrdReturn;
   }
   
   
   public static Dimension getMaximumVideoSize(PublisherWizardData pwData, boolean considerNormal)
   {
      DocumentData dd = pwData.GetDocumentData();

      Dimension maxVideoSize = new Dimension( -1, -1);

      if (pwData.HasClips())
      {
         for (int i = 0; i < dd.GetVideoClipInfos().length; ++i)
         {
            int w = dd.GetVideoClipInfos()[i].videoWidth;
            if (w > maxVideoSize.width)
               maxVideoSize.width = w;
            int h = dd.GetVideoClipInfos()[i].videoHeight;
            if (h > maxVideoSize.height)
               maxVideoSize.height = h;
         }
      }

      if (pwData.HasNormalVideo() && considerNormal)
      {
         int w = dd.GetVideoDimension().width;
         if (w > maxVideoSize.width)
            maxVideoSize.width = w;
         int h = dd.GetVideoDimension().height;
         if (h > maxVideoSize.height)
            maxVideoSize.height = h;
      }

      return maxVideoSize;
   }
   
   public static WizardPanel GetWriterWizardPanel(PublisherWizardData data)
   {
      WizardPanel wp = null;
            
      int iExportType = data.GetProfiledSettings().GetIntValue("iExportType");
            
      if (iExportType == PublisherWizardData.EXPORT_TYPE_VIDEO)
         wp = new VideoWriterWizardPanel(data);
      else if (iExportType == PublisherWizardData.EXPORT_TYPE_REAL)
         wp = new RealWriterWizardPanel(data);
      else if (iExportType == PublisherWizardData.EXPORT_TYPE_WMT)
         wp = new WmpWriterWizardPanel(data);
      else if (iExportType == PublisherWizardData.EXPORT_TYPE_FLASH)
         wp = new FlashWriterWizardPanel(data);
      else
         wp = new LecturnityWriterWizardPanel(data);
     
      return wp;
   }
   
   public static boolean CheckDocumentTypes(ProfiledSettings ps, DocumentData dd, Component wndParent, boolean bIsProfileMode) {
      // check if the document type matches the profile's type 
      int iProfileType = ps.GetType();
      int iDocumentType = dd.GetDocumentType();

      if (ConverterWizard.m_bGenericProfilesEnabled && iProfileType == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN)
          // generic profile always match
          return true;
      
      // exact match
      boolean bMatchingTypes = (iProfileType == iDocumentType);
      // else
      if(!bMatchingTypes) {
         // PZI: override type check for clip structuring                     
         if ((iProfileType == PublisherWizardData.DOCUMENT_TYPE_AUDIO_CLIPS_PAGES)
             && (iDocumentType == PublisherWizardData.DOCUMENT_TYPE_DENVER))
            // pure SG clips (Denver) can be transferred to structured clips
            bMatchingTypes = true;
                      
         else if ((iProfileType == PublisherWizardData.DOCUMENT_TYPE_DENVER)
                  && (iDocumentType == PublisherWizardData.DOCUMENT_TYPE_AUDIO_CLIPS_PAGES)
                  && (dd.isClipStructured()))
            // structured clips can be unstructured to pure SG clips (Denver) 
            bMatchingTypes = true;                      
      }
                  
      if (!bMatchingTypes)
      {
         if (!bIsProfileMode) {
            JOptionPane.showMessageDialog(wndParent, PROFILE_TYPE_ERROR, 
                                          ERROR_MSG, JOptionPane.ERROR_MESSAGE);
         } // else error message (an option dialog) is handled by ConverterWizard (calling one)
      }
      
      return bMatchingTypes;
   }

   public static boolean ContinueAfterNonSupportedCharacterCheckInPath(Component parent, 
                                                                       String strTargetPath) {
      Object[] options = {YES, NO};
      int res = 
         JOptionPane.showOptionDialog(parent, WARN_INVALID_PATH, WARNING_MSG, 
                                      JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, 
                                      null, options, options[1]);
      if (res != 0) // Not 'yes'
         return false;

      return true;
   }

   public static String GetTargetFilenameAfterNonSupportedCharacterCheck(Component parent, 
                                                                         String strTargetDocName) {
      String strNewTargetDocName = 
         (String)JOptionPane.showInputDialog(parent, WARN_INVALID_FILE, WARNING_MSG, 
                                             JOptionPane.WARNING_MESSAGE, null, 
                                             null, strTargetDocName);
      // Note: If "Cancel" is pressed in the Input Dialog then "null" is returned!
      return strNewTargetDocName;
   }

   public static boolean ContinueAfterLogoImageCheck(Component parent, String strLogo) {
      if (strLogo.length() > 0) {
         boolean bImageFileExists = FileUtils.checkTargetExists(strLogo);
         if (!bImageFileExists) {
            String strLogoWarning = ERR_NO_LOGO1 + strLogo + ERR_NO_LOGO2;
            JOptionPane.showMessageDialog(parent, strLogoWarning, ERROR_MSG, 
                                          JOptionPane.ERROR_MESSAGE);
            return false;
         }
      }

      return true;
   }

   private class TextFieldListener implements ActionListener, TextListener
   {
      public void actionPerformed(ActionEvent e)
      {
         // check for complete presentation files here! or there.
//          checkForConsistency();
      }

      public void textValueChanged(TextEvent e)
      {
         setEnableButton(BUTTON_NEXT, false);
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

