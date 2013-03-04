package imc.lecturnity.converter.wizard;

/*
 * File:             LecturnityWriterWizardPanel.java
 * Code Conventions: 18.09.2001
 * Author:           Martin Danielsson
 */

import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.File;
import java.io.IOException;
import java.util.*;
import javax.swing.*;

import imc.epresenter.converter.PresentationConverter;
import imc.epresenter.converter.ConversionDisplay;

import imc.epresenter.filesdk.Archiver;
import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.filesdk.ThumbnailData;

import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.ScreengrabbingExtended;
import imc.lecturnity.converter.ProgressManager;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.FileUtils;

import imc.lecturnity.util.wizards.Wizard;
import imc.lecturnity.util.wizards.WizardData;
import imc.lecturnity.util.wizards.WizardPanel;

public class LecturnityWriterWizardPanel extends WizardPanel
   implements ConversionDisplay
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String ERROR = "[!]";
   private static String WARNING = "[!]";
   private static String INFO = "[!]";
   private static String CONVERT_ERROR = "[!]";
   private static String CANCELLED = "[!]";
   private static String WARNINGS1 = "[!]";
   private static String WARNINGS2 = "[!]";
   private static String START = "[!]";
   private static char   MNEM_CANCEL = '?';
   private static String TRANSFERRING = "[!]";
   private static String CONVERSION_FINISHED = "[!]";
   private static String CONVERSION_UPLOAD_FINISHED = "[!]";
   private static String OK = "[!]";
   private static String OPEN_DIR = "[!]";
   private static String CHECK_UPLOAD_SERVER = "[!]";
   private static String DISK_FULL = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/LecturnityWriterWizardPanel_",
             "en");
         
         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         ERROR = l.getLocalized("ERROR");
         WARNING = l.getLocalized("WARNING");
         INFO = l.getLocalized("INFO");
         CONVERT_ERROR = l.getLocalized("CONVERT_ERROR");
         CANCELLED = l.getLocalized("CANCELLED");
         WARNINGS1 = l.getLocalized("WARNINGS1");
         WARNINGS2 = l.getLocalized("WARNINGS2");
         START = l.getLocalized("START");
         MNEM_CANCEL = l.getLocalized("MNEM_CANCEL").charAt(0);
         TRANSFERRING = l.getLocalized("TRANSFERRING");
         CONVERSION_FINISHED = l.getLocalized("CONVERSION_FINISHED");
         CONVERSION_UPLOAD_FINISHED = l.getLocalized("CONVERSION_UPLOAD_FINISHED");
         OK = l.getLocalized("OK");
         OPEN_DIR = l.getLocalized("OPEN_DIR");
         CHECK_UPLOAD_SERVER = l.getLocalized("CHECK_UPLOAD_SERVER");
         DISK_FULL = l.getLocalized("DISK_FULL");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "LecturnityWriterWizardPanel\n" +
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private PublisherWizardData pwData_;

   private WizardPanel nextCdWizard_ = null;

   private PresentationConverter converter_ = null;

   private JTextArea conversionLogArea_;
   private JScrollPane conversionLogScroller_;
   private JProgressBar overallProgress_;
   private JProgressBar fileProgress_;
   private ProgressManager m_progressManager;

   private JButton cancelButton_;

   private int warningCounter_;
   private boolean m_bCancelRequested;

   public LecturnityWriterWizardPanel(PublisherWizardData pwData)
   {
      super();
      
      pwData_ = pwData;
      m_progressManager = pwData_.m_progressManager;
      m_bCancelRequested = false;

      setPreferredSize(new Dimension(500, 400));

      if (pwData_.GetProfiledSettings().GetBoolValue("bLpdAddToCdProject"))
      {
         addButtons(BACK_QUIT_CANCEL | BUTTON_NEXT);
         setEnableButton(BACK_QUIT_CANCEL | BUTTON_NEXT, false);
      }
      else
      {
         addButtons(BACK_QUIT_CANCEL);
         setEnableButton(BACK_QUIT_CANCEL, false);
      }

      if (NativeUtils.isLibraryLoaded())
      {
         addButtons(BUTTON_CUSTOM, START, 's');
         setEnableButton(BUTTON_CUSTOM, false);
      }
      
      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");

      Container r = getContentPanel();
      r.setLayout(null);

      conversionLogArea_ = new JTextArea();
      conversionLogArea_.setEditable(false);
      conversionLogArea_.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 10));
      conversionLogScroller_ = new JScrollPane(conversionLogArea_);
      conversionLogScroller_.setLocation(new Point(20, 30));
      conversionLogScroller_.setSize(new Dimension(460, 170));
      r.add(conversionLogScroller_);

      overallProgress_ = new JProgressBar(JProgressBar.HORIZONTAL,
                                          0, 100);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         overallProgress_.setBackground(Wizard.BG_COLOR);
      overallProgress_.setLocation(20, 225);
      overallProgress_.setSize(460, 20);
      r.add(overallProgress_);

      // File progress: only for debug purposes (invisible)
      fileProgress_ = new JProgressBar(JProgressBar.HORIZONTAL,
                                       0, 100);
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         fileProgress_.setBackground(Wizard.BG_COLOR);
      fileProgress_.setLocation(20, 255);
      fileProgress_.setSize(340, 20);
      fileProgress_.setVisible(false); // invisible progress bar
      r.add(fileProgress_); 

      cancelButton_ = new JButton(CANCEL) {
            public Insets getInsets()
            {
               return new Insets(1, 1, 1, 1);
            }
         };
      if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         cancelButton_.setBackground(Wizard.BG_COLOR);
      cancelButton_.setSize(100, 25);
      cancelButton_.setLocation(380, 250);
      cancelButton_.setEnabled(false);
      cancelButton_.setMnemonic(MNEM_CANCEL);
      cancelButton_.addActionListener(new CancelButtonListener());
      r.add(cancelButton_);
   }

   public int displayWizard()
   {
      DocumentData dd = pwData_.GetDocumentData();
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      if (ps.GetBoolValue("bLpdAddToCdProject"))
         setEnableButton(BACK_QUIT_CANCEL | BUTTON_NEXT, false);
      else
         setEnableButton(BACK_QUIT_CANCEL, false);
      
      if (NativeUtils.isLibraryLoaded())
         setEnableButton(BUTTON_CUSTOM, false);

      refreshDataDisplay();

      String strTargetFileName = ps.GetTargetFileName();
      
      int iResult = 0;

      boolean bDoUpload = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                           && (ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_UPLOAD) );
      boolean bUseNetworkDrive = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                                  &&(ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_NETWORK_DRIVE) );

      // For the ProgressManager:
      overallProgress_.setMaximum(100);
      defineProgressBarSteps(bDoUpload, bUseNetworkDrive);

      // BUGFIX 5140: Direct Publish should use different dir (analog to Profile Mode)
      if ((pwData_.m_bIsProfileMode || pwData_.m_bIsDirectMode || bDoUpload || bUseNetworkDrive) && !m_bCancelRequested)
      {
         if (bDoUpload)
         {
            // ConversionDisplay
            this.logMessage(CHECK_UPLOAD_SERVER);
            m_progressManager.SetProgressStep("checkUpload");
            m_progressManager.SetProgressLabel(CHECK_UPLOAD_SERVER);
            overallProgress_.setValue(m_progressManager.GetTotalProgressPercentage());

            if (!pwData_.m_bIsUploadSessionRunning)
            {
               // Start the Uploader Session
				UploaderAccess.StartSession(); 
				if (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_SLIDESTAR)
					UploaderAccess.IsSlidestar(true);
				else
					UploaderAccess.IsSlidestar(false);
				pwData_.m_bIsUploadSessionRunning = true;

               // Check access and collect passwords (if necessary); also shows error message
               iResult = UploaderAccess.CheckUpload(true, ps, this);
            }
         }

         // Use generic paths and names for Profile/Upload/Network drive
         if (iResult >= 0)
            iResult = SelectPresentationWizardPanel.DetermineTargetPathsAndFiles(pwData_, this);
         
         if (iResult >= 0)
         {
            // Use temporary local upload dir
            String strTargetDir = ps.GetStringValue("strTargetDirLocal"); 
            if (!strTargetDir.endsWith(File.separator))
               strTargetDir += File.separatorChar;
            strTargetFileName = strTargetDir 
                                + ps.GetStringValue("strTargetPathlessDocName");
         }

         if (iResult != 0)
         {
            if (iResult > 0)
            {
               // "Cancel" was pressed
               m_bCancelRequested = true;
            }
            else
            {
               System.out.println("!!! Upload error in LecturnityWriterWizardPanel::displayWizard()1!");

               // Finish Uploader Session
               UploaderAccess.FinishSession();
               pwData_.m_bIsUploadSessionRunning = false;

               int res = UploaderAccess.ShowErrorMessageForErrorCode(iResult, (WizardPanel)this);

               if (res == 0) // "Verify Settings"
               {
                  pwData_.B_GOTO_START_DIALOG = true;

                  doNext();
                  return BUTTON_NEXT;
               }
               else if (res == 1) // "Retry"
               {
                  return displayWizard();
               }
               else // "Cancel" or Generic error
               {
                  // do nothing special, process is aborted
               }
            }
         }
      }

      // BUGFIX 5236: Check length of target path and files
      int hrPathCheck = 0;
      if (iResult >= 0)
         hrPathCheck = SelectPresentationWizardPanel.CheckTargetPathFileLength(pwData_, this);
      if (hrPathCheck == LPLibs.CANCELLED)
         m_bCancelRequested = true;
      
      // Check target directory and create it, if necessary
      if (iResult >= 0 && !m_bCancelRequested) {
         File targetDir = new File(FileUtils.extractPathFromPathFile(strTargetFileName));
         targetDir.mkdirs();
      }

      // PZI: structured screengrabbing clips
      // add/remove structure and fulltextsearch to/of screengrabbing clips
      if (iResult == 0 && !m_bCancelRequested)
          if (!ScreengrabbingExtended.updateClipStructure(ps, dd))
              requestCancel();

      
      if (iResult == 0 && !m_bCancelRequested)
      {
         try
         {
            warningCounter_ = 0;
   
            converter_ = 
               new PresentationConverter
                  (dd.GetPresentationFileName(),
                   strTargetFileName,
                   this, // ConversionDisplay
                   pwData_,
                   (ps.GetBoolValue("bLpdCompressAudio")));
   
            converter_.setEmbedFonts(ps.GetBoolValue("bLpdEmbedFonts"));
            converter_.setCreateHelpDocument(ps.GetBoolValue("bDoAutostart"));
            // bugfix 5569: do not create thumbnails for denver documents 
            converter_.setCreateThumbnails(ps.GetBoolValue("bLpdUseThumbnails") && !pwData_.IsDenverDocument());
            converter_.setStartInFullScreen(ps.GetBoolValue("bLpdStartInFullScreen"));
            converter_.setShowClipsOnSlides(ps.GetBoolValue("bShowClipsOnSlides") && !pwData_.IsDenverDocument());
   
            boolean isEvalVersion = 
               (ConverterWizard.getVersionType() & NativeUtils.EVALUATION_VERSION) > 0;
            converter_.setIsEvalVersion(isEvalVersion);
   
            cancelButton_.setEnabled(true);
   
            int result = converter_.performConvert();
            
            switch (result)
            {
            case PresentationConverter.SUCCESS:
            
               String strFilename = strTargetFileName + ".lpd";
               
               // Do we have to consider interactive opened files?
               String strEvqFileName = dd.GetPresentationPath() + dd.GetEvqFileName();
               String strObjFileName = dd.GetPresentationPath() + dd.GetObjFileName();
               pwData_.m_aListOfInteractiveOpenedFiles
                  = LPLibs.getListOfInteractiveOpenedFiles(new File(strEvqFileName)+"", 
                                                           new File(strObjFileName)+"");
               
               ArrayList aFilesOne = new ArrayList();
               String[] aFilesPlain = null;

               if ((bDoUpload || bUseNetworkDrive) && !m_bCancelRequested)
               {
                  aFilesOne.add(strFilename);
                  if (pwData_.m_aListOfInteractiveOpenedFiles != null)
                  {
                     int len = pwData_.m_aListOfInteractiveOpenedFiles.length;
                     for (int i = 0; i < len; ++i)
                     {
                        String strPath = ps.GetStringValue("strTargetDirLocal");
                        aFilesOne.add(strPath + pwData_.m_aListOfInteractiveOpenedFiles[i]);
                     }
                  }
                  aFilesPlain = new String[aFilesOne.size()];
                  aFilesOne.toArray(aFilesPlain);
               }

               // Upload or Connected network drive?
               if (bDoUpload && !m_bCancelRequested)
               {
                  File f = new File(strFilename);
                  if (f.exists())
                  {
                     int iSizeKb = FileUtils.getSizeKbOfFileList(aFilesPlain);
                     if (iSizeKb < 1)
                        iSizeKb = 1;
                     displayCurrentFile(strFilename, iSizeKb, TRANSFERRING);
                     logMessage(TRANSFERRING);
                     m_progressManager.SetProgressStep("doUploadCopy");
                     m_progressManager.SetProgressLabel(TRANSFERRING);
                     overallProgress_.setValue(m_progressManager.GetTotalProgressPercentage());
                                 
                     if ((aFilesPlain != null) && (aFilesPlain.length > 0))
                        iResult = UploaderAccess.StartUpload(true,  ps, 
                                                             aFilesPlain, this);
            
                     if (iResult != 0)
                     {
                        if (iResult > 0)
                        {
                           // "Cancel" was pressed
                           m_bCancelRequested = true;
                        }
                        else
                        {
                           System.out.println("!!! Upload error in LecturnityWriterWizardPanel::displayWizard()2!");

                           // Finish Uploader Session
                           UploaderAccess.FinishSession();
                           pwData_.m_bIsUploadSessionRunning = false;

                           int res = UploaderAccess.ShowErrorMessageForErrorCode(iResult, (WizardPanel)this);

                           if (res == 0) // "Verify Settings"
                           {
                              pwData_.B_GOTO_START_DIALOG = true;

                              doNext();
                              return BUTTON_NEXT;
                           }
                           else if (res == 1) // "Retry"
                           {
                              return displayWizard();
                           }
                           else // "Cancel" or Generic error
                           {
                              // do nothing special, process is aborted
                           }
                        }
                     }
   
                     double dProgress = 0;
                     while (iResult == 0 && dProgress < 100.0)
                     {
                        dProgress = UploaderAccess.GetUploadProgress(5);
                        m_progressManager.SetCurrentSubProgress((float)dProgress/100.0f);
                        overallProgress_.setValue(m_progressManager.GetTotalProgressPercentage());
                        
                        if (m_bCancelRequested)
                        {
                           UploaderAccess.CancelSession();
                           dProgress = 100.0;
                           displayCurrentFileProgress(iSizeKb);
                           //logMessage("*** " + CANCELLED);
                           JOptionPane.showMessageDialog(this, CANCELLED, INFO,
                                                         JOptionPane.INFORMATION_MESSAGE);
                           break;
                        }

                        int iProgress = (int)((dProgress/100.0) * iSizeKb);
                        displayCurrentFileProgress(iProgress);
                        // has to match the call of displayCurrentFile() above
               
                        int iWaitMs = 500;
                        try { Thread.sleep(iWaitMs); } catch (InterruptedException exc) {}
                     }
                  }
                  else
                  {
                     // ??? (unexpected) error!
                     System.err.println("File does not exist for transfer! "+strFilename);
                  }

                  // Finish Uploader Session
                  UploaderAccess.FinishSession();
                  pwData_.m_bIsUploadSessionRunning = false;
                  // Remove temporary local dir (if any)
                  SelectPresentationWizardPanel.RemoveTempLocalDirForSingleTargetFile(pwData_, ".lpd");
               }
               else if (bUseNetworkDrive && !m_bCancelRequested)
               {
                  int iSizeKb = FileUtils.getSizeKbOfFileList(aFilesPlain);
                  if (iSizeKb < 1)
                     iSizeKb = 1;
                  displayCurrentFile(strFilename, iSizeKb, TRANSFERRING);
                  logMessage(TRANSFERRING);
                  m_progressManager.SetProgressStep("doNetworkDriveCopy");
                  m_progressManager.SetProgressLabel(TRANSFERRING);
                  overallProgress_.setValue(m_progressManager.GetTotalProgressPercentage());

                  // Copy file(s) to Web (HTTP) server
                  for (int i = 0; i < aFilesPlain.length; ++i)
                  { 
                     String strSource = aFilesPlain[i];
                     String strTarget = ps.GetStringValue("strTargetDirHttpServer") 
                                        + new File(strSource).getName();
                  
                     FileUtils.copyFile(strSource, strTarget, this, "Copying...");
                  }
               }
               
               m_progressManager.SetCurrentSubProgress(1.0f);
               overallProgress_.setValue(m_progressManager.GetTotalProgressPercentage());

               if (ps.GetBoolValue("bLpdAddToCdProject"))
               {
                  setEnableButton(BUTTON_BACK | BUTTON_CANCEL, false);
                  setEnableButton(BUTTON_QUIT | BUTTON_NEXT, true);
               }
               else
               {
                  setEnableButton(BUTTON_BACK | BUTTON_CANCEL, false);
                  setEnableButton(BUTTON_QUIT, true);
               }
   
               if (NativeUtils.isLibraryLoaded())
                  setEnableButton(BUTTON_CUSTOM, true);
   
               fileProgress_.setValue(fileProgress_.getMaximum());
               if (warningCounter_ <= 0)
               {
                  if ((iResult >= 0) && !m_bCancelRequested)
                  {
/*
                     JOptionPane.showMessageDialog(this, CONVERSION_FINISHED,
                                                   INFO, JOptionPane.INFORMATION_MESSAGE);
*/
                     if (!pwData_.m_bIsLiveContext) {
                         String strTargetDir = ps.GetStringValue("strTargetDirLocal");
                         String[] opt = {OK, OPEN_DIR};
                         String strConversionFinished = CONVERSION_FINISHED;
                         if (bDoUpload || bUseNetworkDrive)
                            strConversionFinished = CONVERSION_UPLOAD_FINISHED;
                         int jres = JOptionPane.showOptionDialog(this,
                                                                 (strConversionFinished 
                                                                 + strTargetDir + "\n\n"), 
                                                                 INFO, 
                                                                 JOptionPane.DEFAULT_OPTION, 
                                                                 JOptionPane.INFORMATION_MESSAGE, 
                                                                 null, opt, opt[0]);
                         if (jres == JOptionPane.NO_OPTION) {
                            NativeUtils.startFile(strTargetDir);
                         }
                     }
                  }
               }
               else
               {
                  String strMessage = WARNINGS1 + warningCounter_ + " " + WARNINGS2;
                  JOptionPane.showMessageDialog(this, strMessage, WARNING, 
                                                JOptionPane.WARNING_MESSAGE);
                  m_progressManager.SendWarningMessage(strMessage);
               }
               
               break;
   
            case PresentationConverter.CANCELLED:
               if (ps.GetBoolValue("bLpdAddToCdProject"))
               {
                  setEnableButton(BUTTON_BACK | BUTTON_CANCEL, true);
                  setEnableButton(BUTTON_QUIT | BUTTON_NEXT, false);
               }
               else
               {
                  setEnableButton(BUTTON_BACK | BUTTON_CANCEL, true);
                  setEnableButton(BUTTON_QUIT, false);
               }
   
               if (NativeUtils.isLibraryLoaded())
                  setEnableButton(BUTTON_CUSTOM, false);
   
               logMessage("*** " + CANCELLED);
               JOptionPane.showMessageDialog(this, CANCELLED, INFO,
                                             JOptionPane.INFORMATION_MESSAGE);
               break;
            }
         }
         catch (Exception e)
         {
             // Bugfix 5403: Check for "disk full" error
             boolean bIsDiskFull = false;
             String strTargetPath = ps.GetStringValue("strTargetDirLocal");
             int nMinKb = 100*1024; // less than 100 kB left
             if (NativeUtils.getSpaceLeftOnDevice(strTargetPath) < nMinKb)
                 bIsDiskFull = true;
             
             String strError = e.toString();
             if (bIsDiskFull)
                 strError = DISK_FULL;

            JOptionPane.showMessageDialog(this, CONVERT_ERROR + strError, 
                                          ERROR, JOptionPane.ERROR_MESSAGE);
            e.printStackTrace();
            if (ps.GetBoolValue("bLpdAddToCdProject"))
            {
               setEnableButton(BUTTON_BACK | BUTTON_CANCEL, true);
               setEnableButton(BUTTON_QUIT | BUTTON_NEXT, false);
            }
            else
            {
               setEnableButton(BUTTON_BACK | BUTTON_CANCEL, true);
               setEnableButton(BUTTON_QUIT, false);
            }
   
            if (NativeUtils.isLibraryLoaded())
               setEnableButton(BUTTON_CUSTOM, false);
         }
         finally
         {
            converter_ = null;
            cancelButton_.setEnabled(false);
         }
      }

      // LiveContext mode: close the Publisher automatically 
      if (pwData_.m_bIsLiveContext) {
         super.doQuitOnDoQuit(); // Signal that super should return from displayWizard()

         if (!m_bCancelRequested)
             doQuit(); // Set the correct "action" value: QUIT (not the default CANCEL)
      }
      // Note: This is double code to the other formats

      return super.displayWizard();
   }

   public String getDescription()
   {
      // BUGFIX 4982: set title in direct mode
      return "Publisher";
      // return "";
   }

   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      pwData_.finishWizardPanel = null;
      
      if (ps.GetBoolValue("bLpdAddToCdProject"))
      {
         if (nextCdWizard_ == null)
         {
            CdProjectWizardData cdData = new CdProjectWizardData();
            cdData.filePending = true;
            cdData.addThisFile = ps.GetTargetFileName() + Archiver.ARCHIVE_EXTENSION;
            nextCdWizard_ = new CdProjectWizardPanel(cdData);
         }
         
         pwData_.nextWizardPanel = nextCdWizard_;
      }
      else
         pwData_.nextWizardPanel = null;

      if (pwData_.B_GOTO_START_DIALOG)
      {
         pwData_.finishWizardPanel = null;
         // Bugfix 5448:
         // If Start Dialog is not defined then try the TargetUpload Dialog 
         // If TargetUpload Dialog is not defined then create a new one
         if (pwData_.GetStartDialog() != null) {
             pwData_.nextWizardPanel = pwData_.GetStartDialog();
         } else {
            // Next line outcommented. Reason: 'getWizardData()' is called twice (why?)
            // and then 'nextWizardPanel' will be set to 'null' after the 2nd call 
            //m_pwData.B_GOTO_START_DIALOG = false;
            if (pwData_.GetUploadDialog() != null) {
                pwData_.nextWizardPanel = pwData_.GetUploadDialog();
            } else {
               TargetUploadWizardPanel tuwp = new TargetUploadWizardPanel(pwData_);
               pwData_.SetUploadDialog(tuwp);
               pwData_.nextWizardPanel = tuwp;
            }
         }
      }

      return pwData_;
   }

   private void requestCancel()
   {
      // you cannot cancel twice:
      cancelButton_.setEnabled(false);

      m_bCancelRequested = true;      

      if (converter_ != null)
         converter_.cancelConversion();
   }

   protected void customCommand(String command)
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      String strLpdFile = ps.GetStringValue("strTargetDirLocal") 
                          + ps.GetStringValue("strTargetPathlessDocName") 
                          + Archiver.ARCHIVE_EXTENSION;

      NativeUtils.startFile(strLpdFile);
      if (ps.GetBoolValue("bLpdAddToCdProject"))
         return;
      else
         //doQuit();
         // Bugfix #4085:
         return;
   }
   
   public void logMessage(String message)
   {
      boolean popupMessage = false;

      // Warning message
      if (message.startsWith("[!]"))
      {
         message = message.substring(3);
         popupMessage = true;
         m_progressManager.SendWarningMessage(message);
      }
      conversionLogArea_.append(message + '\n');

      JViewport jvp = conversionLogScroller_.getViewport();
      if (jvp != null)
      {
         Dimension extent = jvp.getExtentSize();
         Dimension size   = jvp.getViewSize();
         jvp.setViewPosition(new Point(0, size.height - extent.height));
      }

      // Internal error message - handled as warning
      if (message.startsWith("***"))
         warningCounter_++;

      if (popupMessage)
         JOptionPane.showMessageDialog(this, message,
                                       WARNING, JOptionPane.WARNING_MESSAGE);
   }

   public void displayCurrentFile(String fileName, int fileSize, 
                                  String actionDescription)
   {
//       conversionLogArea_.append(actionDescription + fileName + " (" +
//                                 fileSize + " bytes)\n");
      fileProgress_.setMaximum(fileSize);
   }

   public void displayCurrentFileProgress(int bytesCount)
   {
      fileProgress_.setValue(bytesCount);
      int maxBytes = fileProgress_.getMaximum();
      float fProgress = 1.0f * bytesCount / maxBytes;
      m_progressManager.SetCurrentSubProgress(fProgress);
      overallProgress_.setValue(m_progressManager.GetTotalProgressPercentage());
   }

   public void displayCurrentProgress(float fProgress)
   {
       m_progressManager.SetCurrentSubProgress(fProgress);
       overallProgress_.setValue(m_progressManager.GetTotalProgressPercentage());
   }

   public void displayCurrentProgressStep(String strStepId, String strLabel) {
       m_progressManager.SetProgressStep(strStepId);
       m_progressManager.SetProgressLabel(strLabel);
       overallProgress_.setValue(m_progressManager.GetTotalProgressPercentage());
   }

   private class CancelButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         requestCancel();
      }
   }

   private void defineProgressBarSteps(boolean bDoUploadCopy, boolean bDoNetworkDriveCopy) {
       m_progressManager.Clear();
       // Step 1: Check Upload Server (Server/Upload only) 
       if (bDoUploadCopy)
           m_progressManager.AddProgressStep("checkUpload", 1);
       // Step 2: Scan Input
       m_progressManager.AddProgressStep("scanInput", 1);
       // Step 3: Convert Audio
       m_progressManager.AddProgressStep("convertAudio", 10);
       // Step 4: Prepare OBJ for Thumbnails
       m_progressManager.AddProgressStep("prepareObjForThumbs", 5);
       // Step 5: Prepare EVQ for Thumbnails
       m_progressManager.AddProgressStep("prepareEvqForThumbs", 5);
       // Step 6: Create Thumbnails
       m_progressManager.AddProgressStep("createThumbnails", 10);
       // Step 7: Write files
       m_progressManager.AddProgressStep("writeFiles", 10);
       // Step 8: Write archive
       m_progressManager.AddProgressStep("writeArchive", 10);
       // Step 9: Cleaning up
       m_progressManager.AddProgressStep("cleaningUp", 1);
       // Step 10a: DoUploadCopy (Server/Upload only)
       if (bDoUploadCopy)
           m_progressManager.AddProgressStep("doUploadCopy", 5);
       // Step 10b: DoNetworkDriveCopy (Server/NetworkDrive only)
       if (bDoNetworkDriveCopy)
           m_progressManager.AddProgressStep("doNetworkDriveCopy", 1);
   }
}
