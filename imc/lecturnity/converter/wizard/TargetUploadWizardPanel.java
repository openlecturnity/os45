package imc.lecturnity.converter.wizard;

import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.IOException;
import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.TitledBorder;

import imc.epresenter.filesdk.util.Localizer;
//import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.wizards.WizardPanel;


public class TargetUploadWizardPanel extends WizardPanel implements ActionListener, Runnable
{   
   private PublisherWizardData m_PublisherData;
   private Localizer m_Localizer;
   
   private TitledBorder m_tbTitleServerOne;
   private TitledBorder m_tbTitleServerTwo;
   private JLabel m_lblDirOrChannelOne;

   private JLabel m_lblHostOne;
   private JLabel m_lblServiceOne;
   private JLabel m_lblUserOne;
   private JLabel m_lblDirOne;
   
   private JLabel m_lblHostTwo;
   private JLabel m_lblServiceTwo;
   private JLabel m_lblUserTwo;
   private JLabel m_lblDirTwo;

	private JLabel m_ytUser;
	private JLabel m_ytTitle;
	private JLabel m_ytDescription;
	private JLabel m_ytKeywords;
	private JLabel m_ytCategory;
	private JLabel m_ytPivacy;
   
   private JButton m_btnChangeOne;
   private JButton m_btnChangeTwo;
//	private JButton m_btnChangeYt;
   
   private JPanel m_pnlUpper;
   private JPanel m_pnlDummy;
   private JPanel m_pnlBorderOne;
   private JPanel m_pnlBorderTwo;
   private JPanel m_pnlTransferOne;
   private JPanel m_pnlTransferTwo;
	private JPanel m_pnlTransferYt;
   
   private JWindow waitingWindow_ = null;
   private JLabel  waitingLabel_ = null; 

   private boolean m_bChangeFirst;
   
   private UploaderAccess.TransferSettings m_tsOne = null;
   private UploaderAccess.TransferSettings m_tsTwo = null;
   private UploaderAccess.YtTransferSettings m_ytTsOne = null;
   
   
   public TargetUploadWizardPanel(PublisherWizardData data)
   {
      super();
      try
      {
         m_Localizer = new Localizer(
            "/imc/lecturnity/converter/wizard/TargetUploadWizardPanel_", "en");
      }
      catch (IOException exc)
      {
         // should not happen
         exc.printStackTrace();
      }
      setPreferredSize(new Dimension(500, 400));
      addButtons(BACK_FINISH_CANCEL | BUTTON_QUIT, data.m_bIsProfileMode);
      useHeaderPanel(m_Localizer.getLocalized("HEADER"), m_Localizer.getLocalized("SUBHEADER"),
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");
      
      m_PublisherData = data;
      ProfiledSettings ps = m_PublisherData.GetProfiledSettings();
   
      if (m_PublisherData.GetUploadDialog() == null)
          m_PublisherData.SetUploadDialog(this);
    
      m_pnlUpper = new JPanel(new BorderLayout(0, 10));
      getContentPanel().add("North", m_pnlUpper);

      m_pnlDummy = new JPanel(new BorderLayout()); // "placeholder"
      m_pnlDummy.add(new JLabel(" "));
      
      //int iReplayType = ps.GetIntValue("iReplayType");
      //boolean bDoServer = iReplayType == PublisherWizardData.REPLAY_SERVER;
      // Note: Podcast is always Server
      
	  boolean bIsYouTube = ps.GetIntValue("iServerType") == PublisherWizardData.SRV_YOUTUBE;
	  if (bIsYouTube) {
          // set document-specific parameters for YouTube upload
          String strTitle = m_PublisherData.GetDocumentData().GetMetadata().title;
          if (strTitle.length()>1)
              ps.SetStringValue("strUploadYtTitle", strTitle);
              
          String strKeywordsArray[] = m_PublisherData.GetDocumentData().GetMetadata().keywords;
          String strKeywords = "";
          if(strKeywordsArray!=null) {
              for(String strKeyword: strKeywordsArray) {
                  if(strKeywords.length()>0)
                      strKeywords += ",";
                  strKeywords += strKeyword;
              }              
          }
          if (strKeywords.length()>0)
              ps.SetStringValue("strUploadYtKeywords", strKeywords);          
	  }
      
      m_pnlTransferOne = CreateSettingsPanel(true, false, "", "", "", "");
      m_pnlBorderOne = CreateBorderPanel(m_Localizer.getLocalized("WEB_SERVER"), true);
      m_pnlBorderOne.add("North", m_pnlTransferOne);
      //m_pnlUpper.add("North", m_pnlBorderOne);

	  m_pnlTransferYt = CreateSettingsPanel(true, true, "", "", "", "");

	  m_pnlBorderOne.add("South", m_pnlTransferYt);
	  m_pnlUpper.add("North", m_pnlBorderOne);
     
      m_pnlTransferTwo = CreateSettingsPanel(false, false, "", "", "", "");
      m_pnlBorderTwo = CreateBorderPanel(m_Localizer.getLocalized("WEB_SERVER"), false);
      m_pnlBorderTwo.add("North", m_pnlTransferTwo);
      m_pnlUpper.add("Center", m_pnlBorderTwo);
   
      //if (bDoServer)
      //   m_pnlTransferTwo.setVisible(true);
      //else
      //   m_pnlTransferTwo.setVisible(false);
      
      m_pnlUpper.setSize(460, m_pnlUpper.getPreferredSize().height+5);
      m_pnlUpper.setPreferredSize(m_pnlUpper.getSize());
      
      // Bugfix 5122:
      // In Profile mode the TransferSettings must be read from the profile data
      if (m_PublisherData.m_bIsProfileMode) {
         if (m_tsOne == null)
            m_tsOne = new UploaderAccess.TransferSettings();
         m_tsOne.GetFrom(ps, true);

         if (m_tsTwo == null)
            m_tsTwo = new UploaderAccess.TransferSettings();
         m_tsTwo.GetFrom(ps, false);

		 if (m_ytTsOne == null)
			 m_ytTsOne = new UploaderAccess.YtTransferSettings();
		 m_ytTsOne.GetFrom(ps);
      }

      refreshDataDisplay(); // sets current transfer settings to labels
   }
   
   public String getDescription()
   {
      return "";
   }

   public PublisherWizardData getWizardData()
   {
      m_PublisherData.nextWizardPanel = null;
      m_PublisherData.finishWizardPanel = null;
      if (!m_PublisherData.m_bIsProfileMode)
      {
         m_PublisherData.finishWizardPanel = 
            SelectPresentationWizardPanel.GetWriterWizardPanel(m_PublisherData);
      }
      
//      ProfiledSettings ps = m_PublisherData.GetProfiledSettings();
//      DocumentData dd = m_PublisherData.GetDocumentData();

      if (PublisherWizardData.B_GOTO_START_DIALOG)
      {
         m_PublisherData.nextWizardPanel = null;
         m_PublisherData.finishWizardPanel = m_PublisherData.GetStartDialog();
         // Bugfix 5448:
         // If the Start Dialog is not defined then try the TargetUpload Dialog 
         // If TargetUpload Dialog is not defined then create a new one
         if (m_PublisherData.GetStartDialog() != null) {
            m_PublisherData.finishWizardPanel = m_PublisherData.GetStartDialog();
         } else {
            m_PublisherData.B_GOTO_START_DIALOG = false;
            if (m_PublisherData.GetUploadDialog() != null)
               m_PublisherData.finishWizardPanel = m_PublisherData.GetUploadDialog();
            else
               m_PublisherData.finishWizardPanel = this;
         }
      }

      return m_PublisherData;
   }

   public void refreshDataDisplay()
   {
      ProfiledSettings ps = m_PublisherData.GetProfiledSettings();
//      DocumentData dd = m_PublisherData.GetDocumentData();
      
      if (m_tsOne != null)
         m_tsOne.SetTo(ps, true);
      if (m_tsTwo != null)
         m_tsTwo.SetTo(ps, false);
	 if (m_ytTsOne != null)
		 m_ytTsOne.SetTo(ps);

      // Note: iReplayType should always be "SERVER" here
      int iServerType = ps.GetIntValue("iServerType");
      boolean bUseSecond = iServerType == PublisherWizardData.SRV_STREAMING_SERVER ||
         iServerType == PublisherWizardData.SRV_PODCAST_SERVER;
      
	   boolean bIsYouTube = ps.GetIntValue("iServerType") == PublisherWizardData.SRV_YOUTUBE;
      // Determine correct texts for the 2 Panels
      String strTransferOne = m_Localizer.getLocalized("WEB_SERVER");
      String strTransferTwo = m_Localizer.getLocalized("STREAMING_SERVER");

      if (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_FILE_SERVER) 
         strTransferOne = m_Localizer.getLocalized("FILE_SERVER");
	 if (bIsYouTube)
		 strTransferOne = "YouTube";

	 if (!bIsYouTube)
	 {
		 if ((ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_VIDEO)
			 && (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_PODCAST_SERVER))
		 {
			 strTransferOne = m_Localizer.getLocalized("PODCAST_CHANNEL");
			 strTransferTwo = m_Localizer.getLocalized("PODCAST_MEDIA_SERVER");
			 // The "Podcast" layout is different from the normal layout
			 m_btnChangeTwo.setVisible(false);
			 m_pnlUpper.remove(m_pnlBorderTwo);
			 m_pnlBorderOne.remove(m_pnlTransferYt);
			 m_pnlBorderOne.add("North", m_pnlTransferOne);
			 m_pnlBorderOne.add("Center", m_pnlDummy);
			 m_pnlBorderOne.add("South", m_pnlBorderTwo);
			 System.out.println("pe podcast");
		 }
		 else
		 {
			 // Use "normal" layout
			 m_btnChangeTwo.setVisible(true);
			 m_pnlBorderOne.remove(m_pnlBorderTwo);
			 m_pnlBorderOne.remove(m_pnlTransferYt);
			 m_pnlBorderOne.remove(m_pnlDummy);
			 m_pnlBorderOne.add("North", m_pnlTransferOne);
			 m_pnlUpper.add("Center", m_pnlBorderTwo);
			 System.out.println("pe file server");
		 }
	 }
	 else
	 {
		 m_btnChangeTwo.setVisible(true);
		 m_pnlBorderOne.remove(m_pnlBorderTwo);
		 m_pnlBorderOne.remove(m_pnlTransferOne);
		 m_pnlBorderOne.remove(m_pnlDummy);
		 m_pnlBorderOne.add("North", m_pnlTransferYt);
		 m_pnlUpper.add("Center", m_pnlBorderTwo);
		 System.out.println("pe youtube");
	 }
	  
	  
      m_pnlBorderOne.validate();
      m_pnlUpper.validate();

      if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_REAL) 
         strTransferTwo = m_Localizer.getLocalized("STREAMING_SERVER") + " (" 
                          + m_Localizer.getLocalized("RM_SERVER") + ")";
      if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_WMT) 
         strTransferTwo = m_Localizer.getLocalized("STREAMING_SERVER") + " (" 
                          + m_Localizer.getLocalized("WM_SERVER") + ")";
      if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH)
      {
         if (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_SLIDESTAR) 
            strTransferOne = m_Localizer.getLocalized("SLIDESTAR");

         strTransferTwo = m_Localizer.getLocalized("STREAMING_SERVER") + " (" 
                          + m_Localizer.getLocalized("FLASH_SERVER") + ")";
      }

      m_tbTitleServerOne.setTitle(strTransferOne);
      m_tbTitleServerTwo.setTitle(strTransferTwo);

	  if (!bIsYouTube)
	  {
		  System.out.println("is not youtube");
		  String strServer = ps.GetStringValue("strUploadFirstServer");

		  if (strServer != null && strServer.length() > 0)
		  {
			  System.out.println("servr len > 0");
			  m_lblHostOne.setText(ps.GetStringValue("strUploadFirstServer"));
			  m_lblServiceOne.setText(GetProtocolName(ps.GetIntValue("iUploadFirstProtocol")));
			  m_lblUserOne.setText(ps.GetStringValue("strUploadFirstUsername"));
			  m_lblDirOne.setText(ps.GetStringValue("strUploadFirstTargetDir"));
		  }
		  else
		  {
			  System.out.println("servr len < 0 sau null");
			  m_lblHostOne.setText("");
			  m_lblServiceOne.setText("");
			  m_lblUserOne.setText("");
			  m_lblDirOne.setText("");
		  }


		  if ((ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_VIDEO)
			  && (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_PODCAST_SERVER))
			  m_lblDirOrChannelOne.setText(m_Localizer.getLocalized("PODCAST_CHANNEL") + ":");
		  else
			  m_lblDirOrChannelOne.setText(m_Localizer.getLocalized("DATA_DIR") + ":");

	  }
	  else
	  {
		  String strUser = ps.GetStringValue("strUploadYtUsername");
		  System.out.println("!!!Soare, user = " + strUser);
		  if (strUser != null && strUser.length() > 0)
		  {
			  m_ytUser.setText(ps.GetStringValue("strUploadYtUsername"));
			  m_ytTitle.setText(ps.GetStringValue("strUploadYtTitle"));
			  m_ytDescription.setText(ps.GetStringValue("strUploadYtDescription"));
			  m_ytKeywords.setText(ps.GetStringValue("strUploadYtKeywords"));
			  m_ytCategory.setText(GetCategory(ps.GetStringValue("strUploadYtCategory")));
			  m_ytPivacy.setText(GetPrivacy(ps.GetIntValue("iUploadYtPrivacy")));
		  }
		  else
		  {
			  m_ytUser.setText("");
			  m_ytTitle.setText("");
			  m_ytDescription.setText("");
			  m_ytKeywords.setText("");
			  m_ytCategory.setText("");
			  m_ytPivacy.setText("");
		  }
	  }

      if (bUseSecond)
      {
         String strServerTwo = ps.GetStringValue("strUploadSecondServer");
                
         if (strServerTwo != null && strServerTwo.length() > 0)
         {
            m_lblHostTwo.setText(ps.GetStringValue("strUploadSecondServer"));
            m_lblServiceTwo.setText(GetProtocolName(ps.GetIntValue("iUploadSecondProtocol")));
            m_lblUserTwo.setText(ps.GetStringValue("strUploadSecondUsername"));
            m_lblDirTwo.setText(ps.GetStringValue("strUploadSecondTargetDir"));
         }
         else
         {
            m_lblHostTwo.setText("");
            m_lblServiceTwo.setText("");
            m_lblUserTwo.setText("");
            m_lblDirTwo.setText("");
         }
         
         m_pnlBorderTwo.setVisible(true);
      }
      else
      {
         m_pnlBorderTwo.setVisible(false);
      }
      
      if (m_PublisherData.m_bIsProfileMode)
      {
         setButtonVisible(WizardPanel.BUTTON_QUIT, true);
         setButtonVisible(WizardPanel.BUTTON_FINISH, false);
      }
      else
      {
         setButtonVisible(WizardPanel.BUTTON_FINISH, true);
         setButtonVisible(WizardPanel.BUTTON_QUIT, false);
      }

      super.refreshDataDisplay();
   }

   public int displayWizard()
   {
      if (waitingWindow_ == null)
      {
         createWaitingWindow();
         locateWaitingWindow();
         waitingWindow_.show();
         try { Thread.sleep(5); } catch (Exception exc) { }
         waitingWindow_.hide();
      }

      return super.displayWizard();
   }
   
   public boolean verifyQuit()
   {
      if (m_PublisherData.m_bIsProfileMode)
      {
         if (!verifyNext())
            return false;
      }
            
      if (m_PublisherData.m_bIsUploadSessionRunning)
      {
         UploaderAccess.FinishSession();
         m_PublisherData.m_bIsUploadSessionRunning = false;
      }

      return true;
   }

   public boolean verifyNext()
   {
      ProfiledSettings ps = m_PublisherData.GetProfiledSettings();
      int iServerType = ps.GetIntValue("iServerType");
      boolean bUseSecond = iServerType == PublisherWizardData.SRV_STREAMING_SERVER ||
         iServerType == PublisherWizardData.SRV_PODCAST_SERVER;

	  boolean bIsYoutube = iServerType == PublisherWizardData.SRV_YOUTUBE;
      String strServer = ps.GetStringValue("strUploadFirstServer");
      String strServerTwo = "";
      
      boolean bSettingsMissing = strServer == null || strServer.length() == 0;

	  if (bIsYoutube)
	  {
		  String strUser = ps.GetStringValue("strUploadYtUsername");
		  bSettingsMissing = strUser == null || strUser.length() == 0;
	  }
      
      /*
      if (strServer == null || strServer.length() == 0)
      {
         ps.SetStringValue("strUploadFirstServer", "10.10.0.10");
         ps.SetIntValue("iUploadFirstProtocol", 1);
         ps.SetIntValue("iUploadFirstPort", 22);
         ps.SetStringValue("strUploadFirstUsername", "tester");
         ps.SetStringValue("strUploadFirstPassword", "testeR");
         ps.SetStringValue("strUploadFirstTargetDir", "/apache/htdocs/podcast/");
         
         ps.SetStringValue("strPodcastChannelUrl", "http://www.freiburg.im-c.de/podcast/ulrich1.xml");
      }
      */
      
      if (!bSettingsMissing && bUseSecond)
      {
         strServerTwo = ps.GetStringValue("strUploadSecondServer");
         bSettingsMissing = strServerTwo == null || strServerTwo.length() == 0;
         
         /*
         if (strServerTwo == null || strServerTwo.length() == 0)
         {
            ps.SetStringValue("strUploadSecondServer", "10.10.0.10");
            ps.SetIntValue("iUploadSecondProtocol", 1);
            ps.SetIntValue("iUploadSecondPort", 22);
            ps.SetStringValue("strUploadSecondUsername", "tester");
            ps.SetStringValue("strUploadSecondPassword", "testeR");
            ps.SetStringValue("strUploadSecondTargetDir", "/apache/htdocs/podcast/");
            
            ps.SetStringValue("strPodcastMediaUrl", "http://www.freiburg.im-c.de/podcast/");
         }
         */
      }
      
      if (bSettingsMissing)
      {
         JOptionPane.showMessageDialog(getFrame(), m_Localizer.getLocalized("NOTHING_SET"),
                                       m_Localizer.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
         
         return false;
      }
      
      if (!m_PublisherData.m_bIsProfileMode)
      {
         // Overwrite check: does the main file already exist?
         String strDocName = ps.GetStringValue("strTargetPathlessDocName");
         String strTargetPath = ps.GetStringValue("strUploadFirstTargetDir");

         boolean bIsUploadServerFileWritable = false;
         // For bugfix 5448: 'checkUploadServer' should not be called in case of 
         // an upload error and "verify settings" is selected 
         // (otherwise this check is done twice) 
         if (PublisherWizardData.B_GOTO_START_DIALOG)
             bIsUploadServerFileWritable = true;
         else
             bIsUploadServerFileWritable = checkUploadServer(strTargetPath, strDocName);
      
         while (PublisherWizardData.B_RETRY_UPLOAD)
         {
             PublisherWizardData.B_RETRY_UPLOAD = false;
            bIsUploadServerFileWritable = checkUploadServer(strTargetPath, strDocName);
         }

         if (!bIsUploadServerFileWritable)
            return false;
      }


      // "Target dir not empty" check
      String strTargetDirOne = (m_tsOne != null) ? m_tsOne.m_strDir : "";
      String strTargetDirTwo = (m_tsTwo != null) ? m_tsTwo.m_strDir : "";

      boolean bIsNotEmtyDirOnTargetWebServer = false;
      // "UploaderAccess.IsNotEmptyDir" should not be checked on a SLIDESTAR server
      boolean bIsSlidestarUpload = ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR
                                   && ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER 
                                   && ps.GetIntValue("iServerType") == PublisherWizardData.SRV_SLIDESTAR;
      if (!bIsSlidestarUpload && !bIsYoutube)
         bIsNotEmtyDirOnTargetWebServer 
            = UploaderAccess.IsNotEmptyDir(strTargetDirOne, true, ps, (WizardPanel)this);

      boolean bIsNotEmtyDirOnTargetStreamingServer = false;
      if (bUseSecond)
         bIsNotEmtyDirOnTargetStreamingServer 
            = UploaderAccess.IsNotEmptyDir(strTargetDirTwo, false, ps, (WizardPanel)this);

      String strWarningNotEmpty = "";
      if (bIsNotEmtyDirOnTargetWebServer)
      {
         strWarningNotEmpty = m_Localizer.getLocalized("WARNING_WEB_DIR_NOT_EMPTY");
         if (bIsNotEmtyDirOnTargetStreamingServer)
            strWarningNotEmpty = m_Localizer.getLocalized("WARNING_BOTH_DIRS_NOT_EMPTY");
      }
      if (bIsNotEmtyDirOnTargetStreamingServer)
      {
         strWarningNotEmpty = m_Localizer.getLocalized("WARNING_STREAMING_DIR_NOT_EMPTY");
         if (bIsNotEmtyDirOnTargetWebServer)
            strWarningNotEmpty = m_Localizer.getLocalized("WARNING_BOTH_DIRS_NOT_EMPTY");
      }

      if (bIsNotEmtyDirOnTargetWebServer || bIsNotEmtyDirOnTargetStreamingServer)
      {
         // Warning message only for export formats which create more than 1 file (exclude LPD and MP4)
         if ( (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_REAL) 
              || (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_WMT) 
              || (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH) )
         {
            // Exclude also ZIP and LZP: no warning message
            if ( ps.GetBoolValue("bScormEnabled") 
                 || (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR) )
            {
               // No warning message
            }
            else
            {
               String[] buttonStrings = {m_Localizer.getLocalized("JOP_CONTINUE"), 
                                         m_Localizer.getLocalized("JOP_CANCEL")};
               int iResult = JOptionPane.showOptionDialog(
                  this, strWarningNotEmpty,
                  m_Localizer.getLocalized("WARNING"), JOptionPane.YES_NO_OPTION, 
                  JOptionPane.WARNING_MESSAGE, 
                  null, buttonStrings, m_Localizer.getLocalized("JOP_CANCEL"));
            
               if (iResult == JOptionPane.NO_OPTION)
               return false;
            }
         }
      }

      return true;
   }
   
   public void actionPerformed(ActionEvent evt)
   {
      m_bChangeFirst = evt.getSource() == m_btnChangeOne;
      
      // TODO: is this correct???
	   // "new Thread(this).run()" is the same as "run()" and will not start new Thread
	   // to start Thread use "new Thread(this).start()"
      new Thread(this).run();
   }
   
   boolean m_bIsShownYtTransferConfigDialog = false;
   
   public void run()
   {
      ProfiledSettings ps = m_PublisherData.GetProfiledSettings();
      boolean bIsPodcast = ps.GetIntValue("iServerType") == PublisherWizardData.SRV_PODCAST_SERVER;
//	  boolean bIsSlidestar = ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR;
	  boolean bIsYouTube = ps.GetIntValue("iServerType") == PublisherWizardData.SRV_YOUTUBE;
	  
	  if (!bIsYouTube)
	  {
		  UploaderAccess.TransferSettings tsOne = new UploaderAccess.TransferSettings();
		  UploaderAccess.TransferSettings tsPodcastMedia = null;
		  if (bIsPodcast)
			  tsPodcastMedia = new UploaderAccess.TransferSettings();
		  // tsOne is the podcast channel (file)

		  if (m_bChangeFirst || bIsPodcast)
		  {
			  tsOne.GetFrom(ps, true);
		  }

		  if (!m_bChangeFirst || bIsPodcast)
		  {
			  UploaderAccess.TransferSettings tsTwo = tsOne;
			  if (bIsPodcast)
				  tsTwo = tsPodcastMedia;

			  tsTwo.GetFrom(ps, false);
		  }

		  // ??? What about errors?

		  int iResult = 0;
		  if (!bIsPodcast)
		  {
			  boolean bIsSlidestarExport = ps.GetIntValue("iServerType") == PublisherWizardData.SRV_SLIDESTAR;

			  // Bugfix 5307/4562: disable data directory input field for SLIDESTAR export
			  // using UploaderAccess.IsSlidestar() as introduced with bugfix 4562 only works the first time
			  // added additional parameter to ShowTransferConfigDialog instead
			  /*
			  if (bIsSlidestarExport)
			  {
				  //tsOne.m_strServer = "slidestar.de";
				  tsOne.m_iProtocol = PublisherWizardData.UPLOAD_SFTP;
				  UploaderAccess.IsSlidestar(bIsSlidestarExport);
			  }
			  */
			  iResult = UploaderAccess.ShowTransferConfigDialog(tsOne, getFrame(), bIsSlidestarExport);
			  System.out.println("ShowTransferConfigDialog - result = " + iResult);
		  }
		  else
		  {
			  String strName = ps.GetStringValue("strPodcastChannelName");
			  String strChannelUrl = ps.GetStringValue("strPodcastChannelUrl");
			  String strMediaUrl = ps.GetStringValue("strPodcastMediaUrl");

			  iResult = UploaderAccess.ShowPodcastConfigDialog(strName, strChannelUrl, strMediaUrl,
															   tsOne, tsPodcastMedia, getFrame());
		  }

		  boolean bOkSelected = iResult == 0;

		  if (bOkSelected)
		  {
			  if (!bIsPodcast)
			  {
				  iResult = UploaderAccess.GetTransferData(tsOne);
				  System.out.println("bOkSelected1 - GetTransferData - result = " + iResult);
			  }
			  else
			  {
				  UploaderAccess.StringContainer scName = new UploaderAccess.StringContainer();
				  UploaderAccess.StringContainer scChannelUrl = new UploaderAccess.StringContainer();
				  UploaderAccess.StringContainer scMediaUrl = new UploaderAccess.StringContainer();

				  iResult = UploaderAccess.GetPodcastData(
					 scName, scChannelUrl, scMediaUrl, tsOne, tsPodcastMedia);

				  ps.SetStringValue("strPodcastChannelName", scName.m_strContent);
				  ps.SetStringValue("strPodcastChannelUrl", scChannelUrl.m_strContent);
				  ps.SetStringValue("strPodcastMediaUrl", scMediaUrl.m_strContent);
			  }
		  }
		  else if (iResult < 0)
		  {
			  // shouldn't happen
			  JOptionPane.showMessageDialog(getFrame(), m_Localizer.getLocalized("NO_CONFIG"),
											m_Localizer.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
		  }
		  // else it was S_FALSE which means "Cancel" in the dialog

		  if (bOkSelected)
		  {
			  System.out.println("bOkSelected2 - m_bChangeFirst = " + m_bChangeFirst + " -bIsPodcast="+bIsPodcast);
			  if (m_bChangeFirst || bIsPodcast)
			  {
				  System.out.println("bOkSelected2 - tsOne");
				  tsOne.SetTo(ps, true);
				  m_tsOne = tsOne;
				  System.out.println("bOkSelected2 - tsOne server = " + m_tsOne.m_strServer);
			  }

			  if (!m_bChangeFirst || bIsPodcast)
			  {
				  UploaderAccess.TransferSettings tsTwo = tsOne;
				  if (bIsPodcast)
					  tsTwo = tsPodcastMedia;

				  tsTwo.SetTo(ps, false);
				  m_tsTwo = tsTwo;
			  }

			  refreshDataDisplay();
		  }
	  }
	  else
	  {
          // Bugfix 5467: incorrect "privacy" setting
          // keep  current "privacy" value if the settings dialog was called before
          // temp required because tsOne.GetFrom(ps) will always overwrite "privacy" for newly created profiles
          int iPrivacyTemp = 0;
          if ( m_bIsShownYtTransferConfigDialog )
              iPrivacyTemp = ps.GetIntValue("iUploadYtPrivacy");
          
		  UploaderAccess.YtTransferSettings tsOne = new UploaderAccess.YtTransferSettings();
		  tsOne.GetFrom(ps);
		  
          // Bugfix 5467: incorrect "privacy" setting
		  // keep previous "privacy" if settings dialog was called before
          if ( m_bIsShownYtTransferConfigDialog ) {
              tsOne.m_iPrivacy = iPrivacyTemp;
              ps.SetIntValue("iUploadYtPrivacy", iPrivacyTemp);
          }

//		  System.out.println("!!!Soare pe run");
		  if (tsOne.m_strKeywords.equals("")) {
			  if (m_PublisherData.m_bIsProfileMode)
			  {
				  tsOne.m_strKeywords = "LECTURNITY";
			  }
			  else
			  {
				  String strKeywordsArray[] = m_PublisherData.GetDocumentData().GetMetadata().keywords;
				  String strKeywords = "";
				  if(strKeywordsArray!=null) {
					for(String strKeyword: strKeywordsArray) {
						if(strKeywords.length()>0)
							strKeywords += ",";
						strKeywords += strKeyword;
					}              
				 }
				  tsOne.m_strKeywords = strKeywords;
			  }
		  }
		  int iResult = UploaderAccess.ShowYtTransferConfigDialog(tsOne, getFrame());
		  boolean bOkSelected = iResult == 0;
		  if(bOkSelected)
			  iResult = UploaderAccess.GetYtTransferData(tsOne);
		  else if (iResult < 0)
		  {
			  // shouldn't happen
			  JOptionPane.showMessageDialog(getFrame(), m_Localizer.getLocalized("NO_CONFIG"),
											m_Localizer.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
		  }
//		  System.out.println("Soare iResult="+iResult);
		  if (bOkSelected)
		  {
//			  System.out.println("Soare pe bOkSelected -" + tsOne.m_strUser);
			  tsOne.SetTo(ps);
			  m_ytTsOne = tsOne;
			  refreshDataDisplay();
              m_bIsShownYtTransferConfigDialog = true;
		  }
	  }
   }
   
   private String GetProtocolName(int iProtocolType)
   {
      String strProtocol = "<?>";
      if (iProtocolType == PublisherWizardData.UPLOAD_FTP)
         strProtocol = "FTP";
      if (iProtocolType == PublisherWizardData.UPLOAD_SCP)
         strProtocol = "SCP";
      if (iProtocolType == PublisherWizardData.UPLOAD_SFTP)
         strProtocol = "SFTP";
	 if (iProtocolType == PublisherWizardData.UPLOAD_HTTP)
		 strProtocol = "HTTP";

      return strProtocol;
   }

	private String GetPrivacy(int iPrivacy)
	{
		System.out.println("privacy = " + iPrivacy);
		String strPrivacy = "<?>";
		if (iPrivacy == 0)
			strPrivacy = m_Localizer.getLocalized("NO");
		else
			strPrivacy = m_Localizer.getLocalized("YES");
		return strPrivacy;
	}

	private String GetCategory(String strCategory)
	{
		System.out.println("strCategory = " + strCategory);
		String aCategoryes[] = {"Autos",
			"Comedy",
			"Education",
			"Entertainment",
			"Film",
			"Games",
			"Howto",
			"Music",
			"News",
			"Nonprofit",
			"People",
			"Animals",
			"Tech",
			"Sports",
			"Travel",	
		};
		String strRetVal = "";
		for (int i = 0; i < aCategoryes.length; i++)
		{
			if (strCategory.equals(aCategoryes[i]))
			{
				strRetVal = m_Localizer.getLocalized("YT_CATEGORY_" + aCategoryes[i].toUpperCase());
				break;
			}
		}
		return strRetVal;
	}

   private boolean checkUploadServer(String strTargetPath, String strDocName)
   {
      this.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
      locateWaitingWindow();
      waitingWindow_.show();

      boolean bIsUploadServerFileWritable 
         = TargetChoiceWizardPanel.IsTargetWritable(this, m_PublisherData, strTargetPath, strDocName, true);

      waitingWindow_.hide();
      this.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
      
      return bIsUploadServerFileWritable;
   }

   private void createWaitingWindow()
   {
      waitingWindow_ = new JWindow(this.getFrame());
      //locateWaitingWindow();
      Container r = waitingWindow_.getContentPane();
      r.setLayout(new BorderLayout());
      JPanel p = new JPanel();
      p.setLayout(null);
      p.setPreferredSize(new Dimension(200, 50));
      p.setBackground(javax.swing.UIManager.getColor("ToolTip.background"));
      Border raisedbevel = BorderFactory.createRaisedBevelBorder();
      p.setBorder(raisedbevel);
      waitingLabel_ = new JLabel(m_Localizer.getLocalized("SERVER_CHECK"));
      waitingLabel_.setSize(200, 50);
      waitingLabel_.setFont(NORMAL_FONT);
      waitingLabel_.setHorizontalAlignment(SwingConstants.CENTER);
      waitingLabel_.setVerticalAlignment(SwingConstants.CENTER);
      p.add(waitingLabel_);
      r.add(p, BorderLayout.CENTER);
      
      return;
   }
   
   private void locateWaitingWindow()
   {
      int x = this.getLocationOnScreen().x + (int)(0.5 * this.getSize().width) - 100;
      int y = this.getLocationOnScreen().y + (int)(0.5 * this.getSize().height) - 25;
      waitingWindow_.setSize(200, 50);
      waitingWindow_.setLocation(x, y);
   }

   private JPanel CreateSettingsPanel(boolean bFirst, boolean bYoutube,
                                      String strHost, String strService,
                                      String strUser, String strDir)
   {
	   System.out.println("!!!!Soare in CreateSettingsPanel");
      JPanel pnlAll = new JPanel(new BorderLayout(5,0));
      JPanel pnlLeft = new JPanel(new BorderLayout(5,0));
      
      JPanel pnlColOne = new JPanel(new GridLayout(0,1,0,5));
      JPanel pnlColTwo = new JPanel(new GridLayout(0,1,0,5));
      
      JPanel pnlRight = new JPanel(new BorderLayout());
      
      pnlLeft.add("West", pnlColOne);
      pnlLeft.add("Center", pnlColTwo);
      
      pnlAll.add("Center", pnlLeft);
      pnlAll.add("East", pnlRight);
	  if (!bYoutube)
	  {
		  pnlColOne.add(new JLabel(m_Localizer.getLocalized("HOST") + ":"));
		  pnlColOne.add(new JLabel(m_Localizer.getLocalized("SERVICE") + ":"));
		  pnlColOne.add(new JLabel(m_Localizer.getLocalized("USER") + ":"));
		  JLabel lblDataDir = new JLabel(m_Localizer.getLocalized("DATA_DIR") + ":");
		  pnlColOne.add(lblDataDir);
		  if (bFirst)
			  m_lblDirOrChannelOne = lblDataDir;

		  JLabel lblHost = new JLabel(strHost);
		  JLabel lblService = new JLabel(strService);
		  JLabel lblUser = new JLabel(strUser);
		  JLabel lblDir = new JLabel(strDir);

		  pnlColTwo.add(lblHost);
		  pnlColTwo.add(lblService);
		  pnlColTwo.add(lblUser);
		  pnlColTwo.add(lblDir);

		  // store these labels for later changes
		  if (bFirst)
		  {
			  m_lblHostOne = lblHost;
			  m_lblServiceOne = lblService;
			  m_lblUserOne = lblUser;
			  m_lblDirOne = lblDir;
		  }
		  else
		  {
			  m_lblHostTwo = lblHost;
			  m_lblServiceTwo = lblService;
			  m_lblUserTwo = lblUser;
			  m_lblDirTwo = lblDir;
		  }
	  }
	  else
	  {
		  pnlColOne.add(new JLabel(m_Localizer.getLocalized("USER") + ":"));
		  pnlColOne.add(new JLabel(m_Localizer.getLocalized("TITLE") + ":"));
		  pnlColOne.add(new JLabel(m_Localizer.getLocalized("DESCRIPTION") + ":"));
		  pnlColOne.add(new JLabel(m_Localizer.getLocalized("KEYWORDS") + ":"));
		  pnlColOne.add(new JLabel(m_Localizer.getLocalized("CATEGORY") + ":"));
		  pnlColOne.add(new JLabel(m_Localizer.getLocalized("PRIVACY") + ":"));

		  m_ytUser = new JLabel("");
		  m_ytTitle = new JLabel("");
		  m_ytDescription = new JLabel("");
		  m_ytKeywords = new JLabel("");
		  m_ytCategory = new JLabel("");
		  m_ytPivacy = new JLabel("");

		  pnlColTwo.add(m_ytUser);
		  pnlColTwo.add(m_ytTitle);
		  pnlColTwo.add(m_ytDescription);
		  pnlColTwo.add(m_ytKeywords);
		  pnlColTwo.add(m_ytCategory);
		  pnlColTwo.add(m_ytPivacy);
	  }
      
      JButton btnChange = new JButton(m_Localizer.getLocalized("CHANGE"));
      btnChange.addActionListener(this);

	  if (!bYoutube)
	  {
		  if (bFirst)
			  m_btnChangeOne = btnChange;
		  else
			  m_btnChangeTwo = btnChange;
	  }
//	  else
//	  {
//		  m_btnChangeYt = btnChange;
//	  }
      
      pnlRight.add("North", btnChange);
      
      return pnlAll;
   }

   private JPanel CreateBorderPanel(String strTitle, boolean bFirst)
   {
      JPanel pnlBorder = new JPanel(new BorderLayout());

      TitledBorder tbTitle = BorderFactory.createTitledBorder(strTitle);
      if (bFirst)
         m_tbTitleServerOne = tbTitle;
      else
         m_tbTitleServerTwo = tbTitle;

      pnlBorder.setBorder(BorderFactory.createCompoundBorder(
         tbTitle, BorderFactory.createEmptyBorder(5,5,5,5)));

      return pnlBorder;
   }
}