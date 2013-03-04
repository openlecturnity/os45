package imc.lecturnity.converter.wizard;

import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.File;
import java.io.IOException;
import javax.swing.*;

import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.filesdk.Metadata;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.converter.ProgressManager;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.wizards.WizardPanel;

public class VideoWriterWizardPanel extends WizardPanel implements ActionListener, Runnable
{
   private static Localizer g_Localizer = null;
   static 
   {
      try
      {
         g_Localizer = new Localizer(
            "/imc/lecturnity/converter/wizard/VideoWriterWizardPanel_", "en");
      }
      catch (IOException exc)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }
   
   private static boolean g_bEngineLoaded = false;
   static
   {
      try
      {
         System.loadLibrary("lplibs");
         g_bEngineLoaded = true;
      }
      catch (UnsatisfiedLinkError e)
      {
         System.err.println("!!! lplibs.dll not found !!!\n");
         // no error dialog needed as this is handled below
      }

      
   }

   
 
   
   private PublisherWizardData m_pwData;
   
   private boolean m_bCancelRequested = false;
   
   private JLabel m_lblProgress;
   private JProgressBar m_barProgress;
   private JButton m_btnCancel;
   private JLabel m_lblRemaining;
   private JLabel m_lblRemainingTime;

   private String m_strTargetMp4;
   private String m_strYtUploadErrorMessage = "";
   private boolean m_bYtUploadSucces = true;
   private ProgressManager m_progressManager;
   
   public VideoWriterWizardPanel(PublisherWizardData pwData)
   {
      super();

      setPreferredSize(new Dimension(500, 400));

      m_pwData = pwData;
      m_progressManager = m_pwData.m_progressManager;
      
      addButtons(BACK_QUIT_CANCEL);
      
      // set logo
      String imageResource = "/imc/lecturnity/converter/images/videologo.jpg";      
      if (m_pwData.GetProfiledSettings().GetIntValue("iServerType") == PublisherWizardData.SRV_YOUTUBE) 
          imageResource = "/imc/lecturnity/converter/images/pngYouTube.png";
      
      useHeaderPanel(g_Localizer.getLocalized("HEADER"), g_Localizer.getLocalized("SUBHEADER"), imageResource);

      if (g_bEngineLoaded)
      {
         addButtons(BUTTON_CUSTOM, g_Localizer.getLocalized("START"), 's');
         setEnableButton(BUTTON_CUSTOM, false);
      }

      Container container = getContentPanel();
      container.setLayout(null);

      
      int w = 460;
      int w1 = 60;
      int x = 20;
      int y = 20;
      
      JLabel lblTask = new JLabel(g_Localizer.getLocalized("TASK")+": ");
      lblTask.setLocation(x,y);
      lblTask.setSize(w1,20);
      //container.add(lblTask);
      
      
      m_lblProgress = new JLabel(g_Localizer.getLocalized("CONVERSION_PROGRESS"));
      m_lblProgress.setLocation(x,y);//+w1+10,y);
      m_lblProgress.setSize(w-(x+w1+10),20);
      container.add(m_lblProgress);
      
      m_barProgress = new JProgressBar(0, 10000);
      m_barProgress.setLocation(x, y+25);
      m_barProgress.setSize(w, 20);
      container.add(m_barProgress);
      
      m_btnCancel = new JButton(g_Localizer.getLocalized("CANCEL"));
      m_btnCancel.addActionListener(this);
      m_btnCancel.setLocation(w+20-100, y+50);
      m_btnCancel.setSize(100, 25);
      container.add(m_btnCancel);
      
      m_lblRemaining = new JLabel(g_Localizer.getLocalized("REMAINING"));
      m_lblRemaining.setLocation(w-140+20, 260);
      m_lblRemaining.setSize(140, 20);
      container.add(m_lblRemaining);
  
      m_lblRemainingTime = new JLabel("0:00");
      m_lblRemainingTime.setLocation(w-40+20, 260);
      m_lblRemainingTime.setSize(40, 20);
      m_lblRemainingTime.setHorizontalAlignment(JLabel.RIGHT);
      container.add(m_lblRemainingTime);
  
      m_btnCancel.requestFocus();
   }
   
   public int displayWizard()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      DocumentData dd = m_pwData.GetDocumentData();

      String strSourceLrd = dd.GetPresentationFileName();
      m_strTargetMp4 = ps.GetTargetFileName() + ".mp4";

      boolean bDoUpload = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                           && (ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_UPLOAD) );
      boolean bUseNetworkDrive = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                                  &&(ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_NETWORK_DRIVE) );

      // For the ProgressManager:
      m_barProgress.setMaximum(100);
      defineProgressBarSteps(bDoUpload, bUseNetworkDrive);

      int iFrameRateTenth = ps.GetIntValue("iMp4FrameRate") * 10;
      int iVideoQFactor = ps.GetIntValue("iMp4VideoQFactor");
      int iAudioRate = ps.GetIntValue("iMp4AudioBitrate");
      int iOutputWidth = ps.GetIntValue("iMp4VideoWidth");
      int iOutputHeight = ps.GetIntValue("iMp4VideoHeight");
      boolean bDoEmbedClips = dd.HasClips() && ps.GetBoolValue("bMp4EmbedClips");
      boolean bVideoInsteadOfPages = m_pwData.IsDenverDocument() || ps.GetBoolValue("bMp4VideoInsteadOfPages");
      
      if (ps.GetBoolValue("bMp4YouTubeOptimized")) {
         // YouTube optimized setting overrule other settings
         iFrameRateTenth = 250;
         iVideoQFactor = 3; // 3 means 90%
         iAudioRate = PublisherWizardData.STD_BITRATE_AUDIO_MP4;
//         iOutputHeight = 720; // YouTube HD resolution
//         iOutputWidth = 960;
         
         // adjust resolution
         // TODO: should be reworked
         Dimension dimension = null;
         if (m_pwData.IsDenverDocument())
             dimension = new Dimension(m_pwData.GetDocumentData().GetClipsStreamInfo().m_nWidth, m_pwData.GetDocumentData().GetClipsStreamInfo().m_nHeight);                     
         else
             dimension = m_pwData.GetDocumentData().GetWhiteboardDimension();
         if (dimension != null) {
             if (dimension.width > iOutputWidth) {
                 iOutputWidth = dimension.width;
                 iOutputWidth += 16 - iOutputWidth%16; // TODO: is this required??
             }
             if (dimension.height > iOutputHeight) {
                 iOutputHeight = dimension.height;
                 iOutputHeight += 16 - iOutputHeight%16; // TODO: is this required??
             }
         }
      }

      if (iFrameRateTenth < 10 || iVideoQFactor < 1 || iAudioRate < 16
          || iOutputWidth < 1 || iOutputHeight < 1)
      {
         System.err.println("!!! Faulty parameters in profile.");
      }


      m_progressManager.SetCurrentSubProgress(0.0f);
      m_barProgress.setValue(m_progressManager.GetTotalProgressPercentage());
      
      setEnableButton(BACK_QUIT_CANCEL, false);
 
      boolean bErrorDisplayed = false;
      
      boolean bUsePodcastServer = 
         ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER && 
         ps.GetIntValue("iServerType") == PublisherWizardData.SRV_PODCAST_SERVER;

	  boolean bUseYoutube = ps.GetIntValue("iServerType") == PublisherWizardData.SRV_YOUTUBE;
      int hr = 0;

      // BUGFIX 5140: Direct Publish should use different dir (analog to Profile Mode)
      if (m_pwData.m_bIsProfileMode || m_pwData.m_bIsDirectMode || bDoUpload || bUseNetworkDrive)
      {
         if (bDoUpload)
         {
            m_lblProgress.setText(g_Localizer.getLocalized("CHECK_UPLOAD_SERVER"));
            m_progressManager.SetProgressStep("checkUpload");
            m_progressManager.SetProgressLabel(m_lblProgress.getText());

            if (!m_pwData.m_bIsUploadSessionRunning)
            {
               // Start the Uploader Session
               UploaderAccess.StartSession();
			   if (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_SLIDESTAR)
				   UploaderAccess.IsSlidestar(true);
			   else
				   UploaderAccess.IsSlidestar(false);
               m_pwData.m_bIsUploadSessionRunning = true;

               // check access and collect passwords (if necessary); also shows error message
			   if (bUseYoutube)
			   {
				   Metadata metadata = dd.GetMetadata();
				   String strRecLength = metadata.recordLength;
				   hr = UploaderAccess.CheckYtUpload(ps, strRecLength, this);
				   //BUGFIX for bug5402
				   if (hr == 1) // cancel was pressed in password dialog
				   {
					   m_bCancelRequested = true;
				   }
			   }
			   else
				   hr = UploaderAccess.CheckUpload(true, ps, this);

               // both first and second server must be set for a mp4 upload (podcast)
               if (hr == 0 && bUsePodcastServer)
                  hr = UploaderAccess.CheckUpload(false, ps, this);
            }
         }
 
         // Use generic paths and names for Profile/Upload/Network drive
         if (hr >= 0 && !m_bCancelRequested)
            hr = SelectPresentationWizardPanel.DetermineTargetPathsAndFiles(m_pwData, this);

         if (hr >= 0 && !m_bCancelRequested)
         {
            // Use temporary local upload dir
            String strTargetDir = ps.GetStringValue("strTargetDirLocal"); 
            if (!strTargetDir.endsWith(File.separator))
               strTargetDir += File.separatorChar;
            m_strTargetMp4 = strTargetDir 
                             + ps.GetStringValue("strTargetPathlessDocName") 
                             + ".mp4";
         }
      }
      
      // BUGFIX 5236: Check length of target path and files
      int hrPathCheck = 0;
      if (hr >= 0 && !m_bCancelRequested)
         hrPathCheck = SelectPresentationWizardPanel.CheckTargetPathFileLength(m_pwData, this);
      if (hrPathCheck == LPLibs.CANCELLED)
         m_bCancelRequested = true;
      
      // Check target directory and create it, if necessary
      if (hr >= 0 && !m_bCancelRequested) {
         File targetDir = new File(FileUtils.extractPathFromPathFile(m_strTargetMp4));
         targetDir.mkdirs();
      }

      if (hr != 0 && !m_bCancelRequested)
      {
         if (hr > 0)
         {
            // "Cancel" was pressed
            m_bCancelRequested = true;
         }
         else
         {
            System.out.println("!!! Upload error in VideoWriterWizardPanel::displayWizard()!");

            // Finish Uploader Session
            UploaderAccess.FinishSession();
            m_pwData.m_bIsUploadSessionRunning = false;

            int res = UploaderAccess.ShowErrorMessageForErrorCode(hr, (WizardPanel)this);
            bErrorDisplayed = true;

            if (res == 0) // "Verify Settings"
            {
               PublisherWizardData.B_GOTO_START_DIALOG = true;

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


      if (hr >= 0 && !m_bCancelRequested)
      {
         m_lblProgress.setText(g_Localizer.getLocalized("SCAN_INPUT"));
         m_progressManager.SetProgressStep("scanInput");
         m_progressManager.SetProgressLabel(m_lblProgress.getText());
         boolean bTargetIPod = !ps.GetBoolValue("bMp4YouTubeOptimized");
         hr = LPLibs.videoPrepare(strSourceLrd, m_strTargetMp4,
                                      iFrameRateTenth, iVideoQFactor, iAudioRate,
                                      iOutputWidth, iOutputHeight, bDoEmbedClips, 
                                      bVideoInsteadOfPages, bTargetIPod);
      }
      
      if (hr < 0 && !bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("SCAN_INPUT"), hr);
         bErrorDisplayed = true;
      }
      
      
      if (hr >= 0 && !m_bCancelRequested)
      {
         m_lblProgress.setText(g_Localizer.getLocalized("CREATE_FILE"));
         m_progressManager.SetProgressStep("createFile");
         m_progressManager.SetProgressLabel(m_lblProgress.getText());
         hr = LPLibs.videoConvertStart();
      }
      
      if (hr < 0 && !bErrorDisplayed)
      {
         displayErrorMessage(g_Localizer.getLocalized("CREATE_FILE"), hr);
         bErrorDisplayed = true;
      }
      
      if (hr >= 0 && !m_bCancelRequested)
      {
         // everything alright: start progress thread
         new Thread(this).start();
      }
      else
      {
         LPLibs.videoCleanup();
         
         m_btnCancel.setEnabled(false);
         setEnableButton(BUTTON_CANCEL, true);
      }
      
      return super.displayWizard();
   }

   public PublisherWizardData getWizardData()
   {
      m_pwData.nextWizardPanel = null;
      m_pwData.finishWizardPanel = null;

      if (PublisherWizardData.B_GOTO_START_DIALOG)
      {
         m_pwData.finishWizardPanel = null;
         // Bugfix 5448:
         // If Start Dialog is not defined then try the TargetUpload Dialog 
         // If TargetUpload Dialog is not defined then create a new one
         if (m_pwData.GetStartDialog() != null) {
            m_pwData.nextWizardPanel = m_pwData.GetStartDialog();
         } else {
            // Next line outcommented. Reason: 'getWizardData()' is called twice (why?)
            // and then 'nextWizardPanel' will be set to 'null' after the 2nd call 
            //m_pwData.B_GOTO_START_DIALOG = false;
            if (m_pwData.GetUploadDialog() != null) {
               m_pwData.nextWizardPanel = m_pwData.GetUploadDialog();
            } else {
               TargetUploadWizardPanel tuwp = new TargetUploadWizardPanel(m_pwData);
               m_pwData.SetUploadDialog(tuwp);
               m_pwData.nextWizardPanel = tuwp;
            }
         }
      }

      return m_pwData;
   }
   

   public String getDescription()
   {
      // BUGFIX 4982: set title in direct mode
      return "Publisher";
      // return "Conversion to Video Dialog";
   }
   
   
   public void actionPerformed(ActionEvent evt)
   {
      if (evt.getSource() == m_btnCancel)
      {
         m_bCancelRequested = true;
      
         LPLibs.videoConvertCancel();
         m_btnCancel.setEnabled(false);
      
         m_lblProgress.setText(g_Localizer.getLocalized("WAIT_FOR_CANCEL"));
         m_progressManager.SetProgressLabel(m_lblProgress.getText());
      }
   }
   
   public void run()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      boolean bDoUpload = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                           && (ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_UPLOAD) );
      boolean bUseNetworkDrive = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                                  &&(ps.GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_NETWORK_DRIVE) );
      boolean bUsePodcastServer = 
         ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER && 
         ps.GetIntValue("iServerType") == PublisherWizardData.SRV_PODCAST_SERVER;

      boolean bFinished = false;
      long lStartMs = System.currentTimeMillis();
      
      int iLoopCounter = 0;
      while (!m_bCancelRequested)
      {
         int iVideoProgress = LPLibs.videoGetProgress(); // can also be error code
         float fProgressPercent = iVideoProgress / 100.0f;
         
         if (fProgressPercent >= 0)
         {
            m_progressManager.SetCurrentSubProgress(fProgressPercent/100.0f);
            m_barProgress.setValue(m_progressManager.GetTotalProgressPercentage());
            
            long lCurrentMs = System.currentTimeMillis();
            float fSpeed = 1.0f;
            if (fProgressPercent > 0)
               fSpeed = (lCurrentMs - lStartMs) / fProgressPercent;
            long lRemainingMs = (long)((100-fProgressPercent) * fSpeed);
            int iRemainingMins = (int)(lRemainingMs / (1000 * 60));
            lRemainingMs -= (long)iRemainingMins * 1000 * 60;
            int iRemainingSecs = (int)(lRemainingMs / 1000);
            m_lblRemainingTime.setText("" + iRemainingMins 
                                       + ":" + ((iRemainingSecs < 10) ? "0" : "")
                                       + iRemainingSecs);
            
            
            if (fProgressPercent < 100)
            {
               int iWaitMs = 500;
               if (lRemainingMs > 10*1000)
                  iWaitMs = 2000;
               try { Thread.sleep(iWaitMs); } catch (InterruptedException exc) {}
            }
            else
            {
               bFinished = true;
               break;
            }
         }
         else
         {
            // error occured
            displayErrorMessage(g_Localizer.getLocalized("CREATE_FILE"), 
                                iVideoProgress);
            break;
         }
         
         ++iLoopCounter;
      }
      
      if (m_bCancelRequested)
      {
         JOptionPane.showMessageDialog(this, 
                                       g_Localizer.getLocalized("CONVERSION_CANCELLED"), 
                                       g_Localizer.getLocalized("INFORMATION"),
                                       JOptionPane.INFORMATION_MESSAGE);
      }
      
      // Upload or Network drive?
      boolean bUploadFinished = true;
      // Upload?
      if (bFinished && bDoUpload && !m_bCancelRequested) {
         m_lblRemaining.setVisible(false);
         m_lblRemainingTime.setVisible(false);
         bUploadFinished = doUploadCopy(bUsePodcastServer);
      }
      // Use Network drive?
      if (bFinished && bUseNetworkDrive && !m_bCancelRequested) {
         m_lblRemaining.setVisible(false);
         m_lblRemainingTime.setVisible(false);
         bUploadFinished = doNetworkDriveCopy();
      }

      m_btnCancel.setEnabled(false);

      boolean bAllFinished = bFinished && bUploadFinished;  

      if (bAllFinished)
      {
/*
         JOptionPane.showMessageDialog(this, 
                                       g_Localizer.getLocalized("CONVERSION_SUCCEEDED"), 
                                       g_Localizer.getLocalized("INFORMATION"),
                                       JOptionPane.INFORMATION_MESSAGE);
*/
         String strTargetDir = ps.GetStringValue("strTargetDirLocal");
         String[] opt = {g_Localizer.getLocalized("OK"), g_Localizer.getLocalized("OPEN_DIR")};
         int jres;
         boolean bShowSuccessDialog = true;
		 if ( m_pwData.m_bIsLiveContext )
		     bShowSuccessDialog = false;
		 if (bShowSuccessDialog)
		 {
			 if (m_bYtUploadSucces == false)
			 {
				 jres = JOptionPane.showOptionDialog(this,
														(g_Localizer.getLocalized("YT_UPLOAD_FAILED1")
														+ m_strYtUploadErrorMessage + "\n\n"
														+ g_Localizer.getLocalized("YT_UPLOAD_FAILED2")
														+ strTargetDir + "\n\n"),
														g_Localizer.getLocalized("ERROR"),
														JOptionPane.DEFAULT_OPTION,
														JOptionPane.ERROR_MESSAGE,
														null, opt, opt[0]);
			 }
			 else
			 {
				 String strConversionSucceeded = g_Localizer.getLocalized("CONVERSION_SUCCEEDED");
				 if (bDoUpload || bUseNetworkDrive)
					 strConversionSucceeded = g_Localizer.getLocalized("CONVERSION_UPLOAD_SUCCEEDED");
				 jres = JOptionPane.showOptionDialog(this,
													 (strConversionSucceeded
													 + strTargetDir + "\n\n"),
													 g_Localizer.getLocalized("INFORMATION"),
													 JOptionPane.DEFAULT_OPTION,
													 JOptionPane.INFORMATION_MESSAGE,
													 null, opt, opt[0]);
			 }

			 if (jres == JOptionPane.NO_OPTION)
			 {
				 NativeUtils.startFile(strTargetDir);
			 }
		 }
      }
      else if (PublisherWizardData.B_GOTO_START_DIALOG)
         doNext();

      if (bAllFinished || m_bCancelRequested)
         setEnableButton(BUTTON_QUIT, true);
      else
         setEnableButton(BUTTON_CANCEL, true);
      
      LPLibs.videoCleanup();
      
      // LiveContext mode: close the Publisher automatically 
      if (m_pwData.m_bIsLiveContext) {
          if (!m_bCancelRequested)
              doQuit(); // Set the correct "action" value: QUIT (not the default CANCEL)
          else
              cancelWizardPanel();
      }
      // Note: This is double code (but different to the other 4 formats)
   }
   
   protected void customCommand(String command)
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      String strMp4File = ps.GetStringValue("strTargetDirLocal") 
                          + ps.GetStringValue("strTargetPathlessDocName") 
                          + ".mp4";
      
      if (NativeUtils.isLibraryLoaded())
      {
         NativeUtils.startFile(strMp4File);

         //doQuit();
         // Bugfix #4085:
         return;
      }
     
   }
   
   private boolean doUploadCopy(boolean bUsePodcastServer)
   {
      boolean bUploadFinished = false;
      
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
            
      if(m_pwData.m_bIsDirectMode && ps.GetIntValue("iServerType") == PublisherWizardData.SRV_YOUTUBE) {
          // quick fix to handle empty / too short values
          // TODO: maybe do some advanced check as in YouTube settings dialog
          
          // TODO: what are the exact requirements for YouTube title??
          
          // set document-specific parameters for YouTube upload
          String strTitle = m_pwData.GetDocumentData().GetMetadata().title;
          if (strTitle.length()>1)
              ps.SetStringValue("strUploadYtTitle", strTitle);
          else
              ps.SetStringValue("strUploadYtTitle","LECTURNITY");

          if (ps.GetStringValue("strUploadYtDescription").length()<2)
              // TODO: what are the exact requirements for YouTube description??
              ps.SetStringValue("strUploadYtDescription","LECTURNITY");
              
          String strKeywordsArray[] = m_pwData.GetDocumentData().GetMetadata().keywords;
          String strKeywords = "";
          if(strKeywordsArray!=null) {
              for(String strKeyword: strKeywordsArray) {
                  if(strKeywords.length()>0)
                      strKeywords += ",";
                  strKeywords += strKeyword;
              }              
          }
          // TODO: what are the exact requirements for YouTube keywords??
          if (strKeywords.length()<2)
              strKeywords = "LECTURNITY";
          ps.SetStringValue("strUploadYtKeywords", strKeywords);
      }

      String strChannelUrl = ps.GetStringValue("strPodcastChannelUrl");
      String strMediaUrl = ps.GetStringValue("strPodcastMediaUrl");
      
      File f = new File(m_strTargetMp4);
      if (f.exists())
      {
         int iSizeKb = (int)(f.length() / 1024);
         if (iSizeKb < 1)
            iSizeKb = 1;

         m_lblProgress.setText(g_Localizer.getLocalized("TRANSFERRING_FILE"));
         m_progressManager.SetProgressStep("doUploadCopy");
         m_progressManager.SetProgressLabel(m_lblProgress.getText());
         m_progressManager.SetCurrentSubProgress(0.0f);
         m_barProgress.setValue(m_progressManager.GetTotalProgressPercentage());

         int iResult = 0;
         if (bUsePodcastServer)
         {
            iResult = UploaderAccess.StartPodcast(strChannelUrl, strMediaUrl, m_pwData.GetDocumentData().GetMetadata().title, m_pwData.GetDocumentData().GetMetadata().author,
                                                  ps, new String[] {m_strTargetMp4}, this);
         }
		 else if (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_YOUTUBE) //youtube upload
		 {
			 iResult = UploaderAccess.StartYtUpload(ps, new String[] { m_strTargetMp4 }, this); 
		 }
		 else // "Normal" upload (File server)
		 {
			 iResult = UploaderAccess.StartUpload(true, ps,
												  new String[] { m_strTargetMp4 }, this);
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
               System.out.println("!!! Upload error in VideoWriterWizardPanel::run()!");

               // Finish Uploader Session
               UploaderAccess.FinishSession();
               m_pwData.m_bIsUploadSessionRunning = false;

               int res = UploaderAccess.ShowErrorMessageForErrorCode(iResult, (WizardPanel)this);

               if (res == 0) // "Verify Settings"
               {
                  PublisherWizardData.B_GOTO_START_DIALOG = true;

                  //doNext();
                  return false;
               }
               else if (res == 1) // "Retry"
               {
                  return doUploadCopy(bUsePodcastServer);
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
            
            m_strYtUploadErrorMessage = UploaderAccess.GetLastError();            
            if (m_strYtUploadErrorMessage.length() != 0)
            {
                // TODO: proper error handling
                m_bYtUploadSucces = false;
                JOptionPane.showMessageDialog(this, m_strYtUploadErrorMessage,
                        g_Localizer.getLocalized("ERROR"), 
                        JOptionPane.ERROR_MESSAGE);
                break;
            }

            if (m_bCancelRequested)
            {
               UploaderAccess.CancelSession();
               dProgress = 100.0;
               m_progressManager.SetCurrentSubProgress(1.0f);
               m_barProgress.setValue(m_progressManager.GetTotalProgressPercentage());
               JOptionPane.showMessageDialog(this, 
                                             g_Localizer.getLocalized("CONVERSION_CANCELLED"), 
                                             g_Localizer.getLocalized("INFORMATION"),
                                             JOptionPane.INFORMATION_MESSAGE);
               break;
            }

            m_progressManager.SetCurrentSubProgress((float)(dProgress/100.0));
            m_barProgress.setValue(m_progressManager.GetTotalProgressPercentage());
            
            int iWaitMs = 500;
            try { Thread.sleep(iWaitMs); } catch (InterruptedException exc) {}
         }

         if (iResult >= 0 && !m_bCancelRequested)
            bUploadFinished = true;
      }
      else
      {
         // ??? (unexpected) error!
         System.err.println("File does not exist for transfer! "+m_strTargetMp4);
      }
	  if (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_YOUTUBE) //youtube upload
	  {
		  m_strYtUploadErrorMessage = UploaderAccess.GetLastError();
		  if (m_strYtUploadErrorMessage.length() != 0)
		  {
			  m_bYtUploadSucces = false;
		  }
	  }
      // Finish Uploader Session
      UploaderAccess.FinishSession();
      m_pwData.m_bIsUploadSessionRunning = false;
      // Remove temporary local dir (if any)
      SelectPresentationWizardPanel.RemoveTempLocalDirForSingleTargetFile(m_pwData, ".mp4");
      
      return bUploadFinished;
   }
   
   private boolean doNetworkDriveCopy()
   {
      boolean bCopyFinished = false;
      
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      // Copy file to Web (HTTP) server
      String strSource = m_strTargetMp4;
      String strTarget = ps.GetStringValue("strTargetDirHttpServer") 
                         + new File(strSource).getName();
      
      m_lblProgress.setText(g_Localizer.getLocalized("TRANSFERRING_FILE"));
      m_progressManager.SetProgressStep("doNetworkDriveCopy");
      m_progressManager.SetProgressLabel(m_lblProgress.getText());
      m_progressManager.SetCurrentSubProgress(0.0f);
      m_barProgress.setValue(m_progressManager.GetTotalProgressPercentage());

      try
      {
         FileUtils.copyFile(strSource, strTarget, this, "Copying...");
         bCopyFinished = true;
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(this, e.toString(), 
                                       g_Localizer.getLocalized("ERROR"), 
                                       JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
      }

      m_progressManager.SetCurrentSubProgress(1.0f);
      m_barProgress.setValue(m_progressManager.GetTotalProgressPercentage());
      
      return bCopyFinished;
   }
   
   private void displayErrorMessage(String sFacility, int nErrorCode)
   {
       // Bugfix 5403: Check for "disk full" error
       boolean bIsDiskFull = false;
       ProfiledSettings ps = m_pwData.GetProfiledSettings(); 
       String strTargetPath = ps.GetStringValue("strTargetDirLocal");
       int nMinKb = 100*1024; // less than 100 kB left
       if (NativeUtils.getSpaceLeftOnDevice(strTargetPath) < nMinKb)
           bIsDiskFull = true;

      String sErrorText = null;
      try
      {
         sErrorText = g_Localizer.getLocalized(nErrorCode+""); 
         if (bIsDiskFull) {
             if (nErrorCode == -23) // -23: DISK_FULL --> Replace error message by "disk full" message 
                 sErrorText = g_Localizer.getLocalized("DISK_FULL");
             else // Append "disk full" message
                 sErrorText = sErrorText + "\n" + g_Localizer.getLocalized("DISK_FULL");
         }
      }
      catch (Exception exc)
      {
         // maybe this error code is not specified
      }
      
      if (sErrorText == null || sErrorText.length() == 0) {
         sErrorText = g_Localizer.getLocalized("-1")+" (0x"+Integer.toHexString(nErrorCode)+")";
         if (bIsDiskFull)
             sErrorText = g_Localizer.getLocalized("DISK_FULL");
      }
      String sMessageText = g_Localizer.getLocalized("ERROR_DURING")+
         ":\n"+sFacility+"\n"+g_Localizer.getLocalized("ERROR")+": "+sErrorText+".";
      String sNativeText = LPLibs.flashLastErrorDetail();
      if (sNativeText != null)
         sMessageText = sMessageText+"\n"+sNativeText;
            
      
      JOptionPane.showMessageDialog(this, 
                                    sMessageText,
                                    g_Localizer.getLocalized("ERROR"),
                                    JOptionPane.ERROR_MESSAGE);
      
   }
   
   private void defineProgressBarSteps(boolean bDoUploadCopy, boolean bDoNetworkDriveCopy) {
       m_progressManager.Clear();
       // Step 1: Check Upload Server (Server/Upload only) 
       if (bDoUploadCopy)
           m_progressManager.AddProgressStep("checkUpload", 1);
       // Step 2: Scan Input
       m_progressManager.AddProgressStep("scanInput", 1);
       // Step 3: Create File
       m_progressManager.AddProgressStep("createFile", 10);
       // Step 4a: DoUploadCopy (Server/Upload only)
       if (bDoUploadCopy)
           m_progressManager.AddProgressStep("doUploadCopy", 5);
       // Step 4b: DoNetworkDriveCopy (Server/NetworkDrive only)
       if (bDoNetworkDriveCopy)
           m_progressManager.AddProgressStep("doNetworkDriveCopy", 1);
   }
}

