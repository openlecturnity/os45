package imc.lecturnity.converter.wizard;

import java.awt.Container;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Font;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.filechooser.FileFilter;

import java.io.IOException;
import java.io.File;

import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.ui.DirectoryChooser;
///import imc.lecturnity.util.ui.PopupHelp;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;
import imc.epresenter.filesdk.util.Localizer;

public class TargetTypeWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String SERVER_TYPE = "[!]";
   private static String WEB_SERVER = "[!]";
   private static String STREAMING_SERVER = "[!]";
   private static String SLIDESTAR = "[!]";
   private static String YOUTUBE = "[!]";
   private static String PODCAST_SERVER = "[!]";
   private static String FILE_SERVER = "[!]";
   private static String STREAMING_URL = "[!]";
   private static String TRANSFER_TYPE = "[!]";
   private static String NETWORK_DRIVE = "[!]";
   private static String UPLOAD = "[!]";
   private static String NO_URL = "[!]";
   private static String INFO_URL_MISSING = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/TargetTypeWizardPanel_",
             "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         SERVER_TYPE = l.getLocalized("SERVER_TYPE");
         WEB_SERVER = l.getLocalized("WEB_SERVER");
         STREAMING_SERVER = l.getLocalized("STREAMING_SERVER");
         SLIDESTAR = l.getLocalized("SLIDESTAR");
         YOUTUBE = l.getLocalized("YOUTUBE");
         PODCAST_SERVER = l.getLocalized("PODCAST_SERVER");
         FILE_SERVER = l.getLocalized("FILE_SERVER");
         STREAMING_URL = l.getLocalized("STREAMING_URL");
         TRANSFER_TYPE = l.getLocalized("TRANSFER_TYPE");
         NETWORK_DRIVE = l.getLocalized("NETWORK_DRIVE");
         UPLOAD = l.getLocalized("UPLOAD");
         NO_URL = l.getLocalized("NO_URL");
         INFO_URL_MISSING = l.getLocalized("INFO_URL_MISSING");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "TargetTypeWizardPanel\n" + 
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         System.exit(1);
      }
   }

   public static WizardPanel createInstance(PublisherWizardData pwData)
   {
      return new TargetTypeWizardPanel(pwData);
   }

   private PublisherWizardData m_pwData;

//   private WizardPanel m_nextWizard = null;
   private TargetDirsWizardPanel m_nextTargetDirsWizard = null;
   private TargetUploadWizardPanel m_nextTargetUploadWizard = null;

   private JRadioButton m_webServerRadio;
   private JRadioButton m_streamingServerRadio;
   //private JRadioButton m_slidestarRadio;
   private JRadioButton m_youTubeRadio;
   private JRadioButton m_podcastServerRadio;
   private JRadioButton m_fileServerRadio;
   private JRadioButton m_networkDriveRadio;
   private JRadioButton m_uploadRadio;
   private JLabel m_streamingUrlLabel;
   private JLabel m_protocolLabel;
   private JTextField m_protocolTextField;
   private JTextField m_urlTextField;

///   private PopupHelp m_helpPopup;

   private int m_iExportType;
   private int m_iServerType;
   private int m_iTransferType;

   private String m_strHttpProtocol;
   private String m_strStreamingProtocol;
   private String m_strUrlPath;
   private boolean m_bIsRealWithSmilFile;


   public TargetTypeWizardPanel(PublisherWizardData pwData)
   {
      super();

      m_pwData = pwData;

      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      m_iExportType = ps.GetIntValue("iExportType");
      m_iServerType = ps.GetIntValue("iServerType");
      m_iTransferType = ps.GetIntValue("iTransferType");
      
      if (m_pwData.m_bIsProfileMode)
         initProfileModeValues();
      else
         initDefaultValues();

      // One special case: 
      // The "http://" URL is used in the following case:
      // - RM export and SMIL file.
      // A SMIL file is only used if the document has clips and clips are seperated from video.
      m_bIsRealWithSmilFile = false;
      if ( (m_iExportType == PublisherWizardData.EXPORT_TYPE_REAL)
           && m_pwData.HasClips() && ps.GetBoolValue("bShowClipsOnSlides") )
         m_bIsRealWithSmilFile = true;

      initGui();
   }
   public String getDescription()
   {
      return HEADER;
   }

   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      ps.SetIntValue("iServerType", m_iServerType);
      ps.SetIntValue("iTransferType", m_iTransferType);

      if (m_urlTextField.isEnabled() && m_urlTextField.getText().length() > 0)
      {
         String strProtocol = m_protocolTextField.getText();
         if (strProtocol.length() > 0)
            strProtocol += "://";
         String strUrl = strProtocol + m_urlTextField.getText();
         // Append "/", if not done already
         if (strUrl.charAt(strUrl.length()-1) != '/')
            strUrl = strUrl + '/';

         if (m_bIsRealWithSmilFile && (m_iServerType == PublisherWizardData.SRV_WEB_SERVER))
            ps.SetStringValue("strTargetUrlHttpServer", strUrl);
         else
            ps.SetStringValue("strTargetUrlStreamingServer", strUrl);
      }

      switch(m_iServerType)
      {
         case PublisherWizardData.SRV_FILE_SERVER:
            // Upload
            createAndSetLocalUploadDirectory();
            if (m_nextTargetUploadWizard == null)
               m_nextTargetUploadWizard = new TargetUploadWizardPanel(m_pwData);
            m_pwData.nextWizardPanel = m_nextTargetUploadWizard;
            break;
         case PublisherWizardData.SRV_WEB_SERVER:
            if (m_iTransferType == PublisherWizardData.TRANSFER_NETWORK_DRIVE)
            {
               // Connected network drive
               if (m_nextTargetDirsWizard == null)
                  m_nextTargetDirsWizard = new TargetDirsWizardPanel(m_pwData);
               m_pwData.nextWizardPanel = m_nextTargetDirsWizard;
            }
            else
            {
               // Upload
               createAndSetLocalUploadDirectory();
               if (m_nextTargetUploadWizard == null)
                  m_nextTargetUploadWizard = new TargetUploadWizardPanel(m_pwData);
               m_pwData.nextWizardPanel = m_nextTargetUploadWizard;
            }
            break;
         case PublisherWizardData.SRV_STREAMING_SERVER:
            if (m_iTransferType == PublisherWizardData.TRANSFER_NETWORK_DRIVE)
            {
               // Connected network drive
               if (m_nextTargetDirsWizard == null)
                  m_nextTargetDirsWizard = new TargetDirsWizardPanel(m_pwData);
               m_pwData.nextWizardPanel = m_nextTargetDirsWizard;
            }
            else
            {
               // Upload
               createAndSetLocalUploadDirectory();
               if (m_nextTargetUploadWizard == null)
                  m_nextTargetUploadWizard = new TargetUploadWizardPanel(m_pwData);
               m_pwData.nextWizardPanel = m_nextTargetUploadWizard;
            }
            break;
         case PublisherWizardData.SRV_PODCAST_SERVER:
            // Upload
            createAndSetLocalUploadDirectory();
            if (m_nextTargetUploadWizard == null)
               m_nextTargetUploadWizard = new TargetUploadWizardPanel(m_pwData);
            m_pwData.nextWizardPanel = m_nextTargetUploadWizard;
            break;
         case PublisherWizardData.SRV_SLIDESTAR:
            // Upload
            createAndSetLocalUploadDirectory();
            if (m_nextTargetUploadWizard == null)
               m_nextTargetUploadWizard = new TargetUploadWizardPanel(m_pwData);
            m_pwData.nextWizardPanel = m_nextTargetUploadWizard;
            break;
		  case PublisherWizardData.SRV_YOUTUBE:
			  createAndSetLocalUploadDirectory();
			  if (m_nextTargetUploadWizard == null)
				  m_nextTargetUploadWizard = new TargetUploadWizardPanel(m_pwData);
			  m_pwData.nextWizardPanel = m_nextTargetUploadWizard;
			  break;
         default:
            break;
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

      // Select "Server Type" radio and set URL text
      switch(m_iServerType)
      {
         case PublisherWizardData.SRV_FILE_SERVER:
            m_fileServerRadio.setSelected(true);
            break;
         case PublisherWizardData.SRV_WEB_SERVER:
            m_webServerRadio.setSelected(true);
            m_urlTextField.setText(m_strUrlPath);
            break;
         case PublisherWizardData.SRV_STREAMING_SERVER:
            m_streamingServerRadio.setSelected(true);
            m_urlTextField.setText(m_strUrlPath);
            break;
         case PublisherWizardData.SRV_PODCAST_SERVER:
            m_podcastServerRadio.setSelected(true);
            break;
		 //case PublisherWizardData.SRV_SLIDESTAR:
		 //   m_slidestarRadio.setSelected(true);
		 //   break;
		case PublisherWizardData.SRV_YOUTUBE:
			m_youTubeRadio.setSelected(true);
			break;
         default:
            break;
      }

      // Set "Protocol" text 
      switch (m_iExportType)
      {
         case PublisherWizardData.EXPORT_TYPE_LECTURNITY:
            m_protocolTextField.setText("");
            break;
         case PublisherWizardData.EXPORT_TYPE_REAL:
            if (m_webServerRadio.isSelected())
            {
               if (m_bIsRealWithSmilFile)
                  m_protocolTextField.setText(m_strHttpProtocol);
               else
                  m_protocolTextField.setText("");
            }
            else if (m_streamingServerRadio.isSelected())
               m_protocolTextField.setText(m_strStreamingProtocol);
            else
               m_protocolTextField.setText("");
            break;
         case PublisherWizardData.EXPORT_TYPE_WMT:
            if (m_streamingServerRadio.isSelected())
               m_protocolTextField.setText(m_strStreamingProtocol);
            else
               m_protocolTextField.setText("");
            break;
         case PublisherWizardData.EXPORT_TYPE_FLASH:
            if (m_streamingServerRadio.isSelected())
               m_protocolTextField.setText(m_strStreamingProtocol);
            else
               m_protocolTextField.setText("");
            break;
         case PublisherWizardData.EXPORT_TYPE_VIDEO:
            m_protocolTextField.setText("");
            break;
         default:
            break;
      }


      // Select "Transfer Type" radio
      switch(m_iTransferType)
      {
         case PublisherWizardData.TRANSFER_NETWORK_DRIVE:
            m_networkDriveRadio.setSelected(true);
            break;
         case PublisherWizardData.TRANSFER_UPLOAD:
            m_uploadRadio.setSelected(true);
            break;
         default:
            break;
      }

      checkEnabled();
   }

   public boolean verifyNext()
   {
      if (m_urlTextField.isEnabled() && m_urlTextField.getText().equals(""))
      {
         JOptionPane.showMessageDialog
            (this, INFO_URL_MISSING, NO_URL,
             JOptionPane.INFORMATION_MESSAGE);
         return false;
      }

      return true;
   }

   private void initGui()
   {
      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);
      
      setPreferredSize(new Dimension(500, 400));

      addButtons(NEXT_BACK_CANCEL);
///      addButtons(NEXT_BACK_CANCEL | BUTTON_HELP);
      
      useHeaderPanel(HEADER, SUBHEADER, TargetChoiceWizardPanel.getHeaderLogoName());

///      // help button popup thingies
///      m_helpPopup = new PopupHelp(getHelpButton(), 
///                                 "<html>" + "blubb" + "</html>");

      Container r = getContentPanel();
      r.setLayout(null);

      ButtonGroup bgServer = new ButtonGroup();
      ButtonGroup bgTransfer = new ButtonGroup();
      ServerRadioListener srl = new ServerRadioListener();
      TargetRadioListener trl = new TargetRadioListener();

      int x = 30;
      int y = 15;
      int x1 = 20;
      int y1 = 20;
      int x2 = 20;
      int y2 = 20;

      JPanel serverPanel = new JPanel();
      serverPanel.setLayout(null);
      serverPanel.setFont(descFont);
      serverPanel.setSize(445, 200/*180*/);
      serverPanel.setLocation(x-3, y - 10);
      serverPanel.setBorder(new TitledBorder(SERVER_TYPE));
      r.add(serverPanel);

      m_webServerRadio = new JRadioButton(WEB_SERVER, false);
      m_webServerRadio.setFont(descFont);
      m_webServerRadio.setSize(380, 20);
      m_webServerRadio.setLocation(x1, y1);
      m_webServerRadio.addActionListener(srl);
      bgServer.add(m_webServerRadio);
      serverPanel.add(m_webServerRadio);

      y1 += 20;
      m_streamingServerRadio = new JRadioButton(STREAMING_SERVER, false);
      m_streamingServerRadio.setFont(descFont);
      m_streamingServerRadio.setSize(380, 20);
      m_streamingServerRadio.setLocation(x1, y1);
      m_streamingServerRadio.addActionListener(srl);
      bgServer.add(m_streamingServerRadio);
      serverPanel.add(m_streamingServerRadio);

	  //y1 += 20;
	  //m_slidestarRadio = new JRadioButton(SLIDESTAR, false);
	  //m_slidestarRadio.setFont(descFont);
	  //m_slidestarRadio.setSize(380, 20);
	  //m_slidestarRadio.setLocation(x1, y1);
	  //m_slidestarRadio.addActionListener(srl);
	  //bgServer.add(m_slidestarRadio);
	  //serverPanel.add(m_slidestarRadio);

      y1 += 20;
      m_podcastServerRadio = new JRadioButton(PODCAST_SERVER, false);
      m_podcastServerRadio.setFont(descFont);
      m_podcastServerRadio.setSize(380, 20);
      m_podcastServerRadio.setLocation(x1, y1);
      m_podcastServerRadio.addActionListener(srl);
      bgServer.add(m_podcastServerRadio);
      serverPanel.add(m_podcastServerRadio);

	  y1 += 20;
	  m_youTubeRadio = new JRadioButton(YOUTUBE, false);
	  m_youTubeRadio.setFont(descFont);
	  m_youTubeRadio.setSize(380, 20);
	  m_youTubeRadio.setLocation(x1, y1);
	  m_youTubeRadio.addActionListener(srl);
	  bgServer.add(m_youTubeRadio);
	  serverPanel.add(m_youTubeRadio);

      y1 += 20;
      m_fileServerRadio = new JRadioButton(FILE_SERVER, false);
      m_fileServerRadio.setFont(descFont);
      m_fileServerRadio.setSize(380, 20);
      m_fileServerRadio.setLocation(x1, y1);
      m_fileServerRadio.addActionListener(srl);
      bgServer.add(m_fileServerRadio);
      serverPanel.add(m_fileServerRadio);

      y1 += 25;
      m_streamingUrlLabel = new JLabel(STREAMING_URL);
      m_streamingUrlLabel.setFont(descFont);
      m_streamingUrlLabel.setSize(440, 20);
      m_streamingUrlLabel.setLocation(x1+20, y1);
      serverPanel.add(m_streamingUrlLabel);

      y1 += 25;
      m_protocolTextField = new JTextField("");
      m_protocolTextField.setSize(40, 20);
      m_protocolTextField.setLocation(x1+20, y1);
      serverPanel.add(m_protocolTextField);
      
      m_protocolLabel = new JLabel("://");
      m_protocolLabel.setFont(descFont);
      m_protocolLabel.setSize(12, 20);
      m_protocolLabel.setLocation(x1+63, y1);
      serverPanel.add(m_protocolLabel);

      m_urlTextField = new JTextField("");
      m_urlTextField.setSize(325, 20);
      m_urlTextField.setLocation(x1+75, y1);
      serverPanel.add(m_urlTextField);


      y += 190;
      JPanel transferPanel = new JPanel();
      transferPanel.setLayout(null);
      transferPanel.setFont(descFont);
      transferPanel.setSize(445, 70);
      transferPanel.setLocation(x-3, y + 5);
      transferPanel.setBorder(new TitledBorder(TRANSFER_TYPE));
      r.add(transferPanel);

      m_networkDriveRadio = new JRadioButton(NETWORK_DRIVE, false);
      m_networkDriveRadio.setFont(descFont);
      m_networkDriveRadio.setSize(380, 20);
      m_networkDriveRadio.setLocation(x2, y2);
      m_networkDriveRadio.addActionListener(trl);
      bgTransfer.add(m_networkDriveRadio);
      transferPanel.add(m_networkDriveRadio);

      y2 += 20;
      m_uploadRadio = new JRadioButton(UPLOAD, false);
      m_uploadRadio.setFont(descFont);
      m_uploadRadio.setSize(380, 20);
      m_uploadRadio.setLocation(x2, y2);
      m_uploadRadio.addActionListener(trl);
      bgTransfer.add(m_uploadRadio);
      transferPanel.add(m_uploadRadio);
      

      refreshDataDisplay();
   }

   private void checkEnabled()
   {
      // Enable/Disable "Server Type" radios
      // If a disabled radio was selected then the default radio is selected. 
      switch (m_iExportType)
      {
         case PublisherWizardData.EXPORT_TYPE_LECTURNITY:
            // Enabled: FileServer
            m_webServerRadio.setEnabled(false);
            if (m_webServerRadio.isSelected())
               m_fileServerRadio.setSelected(true);
            m_streamingServerRadio.setEnabled(false);
            if (m_streamingServerRadio.isSelected())
               m_fileServerRadio.setSelected(true);
			//m_slidestarRadio.setEnabled(false);
			//if (m_slidestarRadio.isSelected())
			//   m_fileServerRadio.setSelected(true);
            m_youTubeRadio.setEnabled(false);
            if(m_youTubeRadio.isSelected())
                m_fileServerRadio.setSelected(true);
            m_podcastServerRadio.setEnabled(false);
            if (m_podcastServerRadio.isSelected())
               m_fileServerRadio.setSelected(true);
            m_fileServerRadio.setEnabled(true);
            break;
         case PublisherWizardData.EXPORT_TYPE_REAL:
            // Enabled: WebServer, StreamingServer, FileServer
            m_webServerRadio.setEnabled(true);
            m_streamingServerRadio.setEnabled(true);
			//m_slidestarRadio.setEnabled(false);
			//if (m_slidestarRadio.isSelected())
			//   m_webServerRadio.setSelected(true);
            m_youTubeRadio.setEnabled(false);
            if(m_youTubeRadio.isSelected())
                m_webServerRadio.setSelected(true);
            m_podcastServerRadio.setEnabled(false);
            if (m_podcastServerRadio.isSelected())
               m_webServerRadio.setSelected(true);
            m_fileServerRadio.setEnabled(true);
            break;
         case PublisherWizardData.EXPORT_TYPE_WMT:
            // Enabled: WebServer, StreamingServer, FileServer
            m_webServerRadio.setEnabled(true);
            m_streamingServerRadio.setEnabled(true);
			//m_slidestarRadio.setEnabled(false);
			//if (m_slidestarRadio.isSelected())
			//   m_webServerRadio.setSelected(true);
            m_youTubeRadio.setEnabled(false);
            if(m_youTubeRadio.isSelected())
                m_webServerRadio.setSelected(true);
            m_podcastServerRadio.setEnabled(false);
            if (m_podcastServerRadio.isSelected())
               m_webServerRadio.setSelected(true);
            m_fileServerRadio.setEnabled(true);
            break;
         case PublisherWizardData.EXPORT_TYPE_FLASH:
            // Enabled: WebServer, StreamingServer, Slidestar, FileServer
            m_webServerRadio.setEnabled(true);
            m_streamingServerRadio.setEnabled(true);
            // Bugfix 4075: Streaming server is disabled for Flash movies without any video
            DocumentData dd = m_pwData.GetDocumentData();
            boolean bHasAnyVideo = dd.HasNormalVideo() || dd.HasClips(); // denver -> has clips
            ProfiledSettings ps = m_pwData.GetProfiledSettings();
            boolean bIsNewFlashPlayer = (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX);
            if (!(bHasAnyVideo || bIsNewFlashPlayer))
            {
               m_streamingServerRadio.setEnabled(false);
               if (m_streamingServerRadio.isSelected())
                  m_webServerRadio.setSelected(true);
            }
            //m_slidestarRadio.setEnabled(true);
            m_youTubeRadio.setEnabled(false);
            if(m_youTubeRadio.isSelected())
                m_webServerRadio.setSelected(true);
            m_podcastServerRadio.setEnabled(false);
            if (m_podcastServerRadio.isSelected())
               m_webServerRadio.setSelected(true);
            m_fileServerRadio.setEnabled(true);
            break;
         case PublisherWizardData.EXPORT_TYPE_VIDEO:
            // Enabled: PodcastServer, FileServer, YouTube 
            m_webServerRadio.setEnabled(false);
            if (m_webServerRadio.isSelected())
               m_podcastServerRadio.setSelected(true);
            m_streamingServerRadio.setEnabled(false);
            if (m_streamingServerRadio.isSelected())
               m_podcastServerRadio.setSelected(true);
			//m_slidestarRadio.setEnabled(false);
			//if (m_slidestarRadio.isSelected())
			//   m_podcastServerRadio.setSelected(true);
            m_youTubeRadio.setEnabled(true);
            m_podcastServerRadio.setEnabled(true);
            m_fileServerRadio.setEnabled(true);
            break;
         default:
            break;
      }

      // Strict SCORM? --> Only Web server and File server are enabled
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      if (ps.GetBoolValue("bScormStrict"))
      {
         // Enabled: WebServer (default), FileServer
         m_streamingServerRadio.setEnabled(false);
         if (m_streamingServerRadio.isSelected())
            m_webServerRadio.setSelected(true);
         //m_slidestarRadio.setEnabled(false);
		 //if (m_slidestarRadio.isSelected())
		 //   m_webServerRadio.setSelected(true);
         m_podcastServerRadio.setEnabled(false);
         if (m_podcastServerRadio.isSelected())
            m_webServerRadio.setSelected(true);
      }
      
      // Bugfix 4057: Disable "Slidestar" in general as the functionality of "Slidestar" 
      // is not completely implemented yet.
	  //m_slidestarRadio.setEnabled(false);
	  //if (m_slidestarRadio.isSelected())
	  //{
	  //   if (m_webServerRadio.isEnabled())
	  //      m_webServerRadio.setSelected(true);
	  //   else // (m_fileServerRadio is always enabled)
	  //      m_fileServerRadio.setSelected(true);
	  //}
	  boolean bIsSlidestarExport = ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR;

	  //if (bIsSlidestarExport)
	  //{
	  //    m_webServerRadio.setEnabled(false);
	  //    m_slidestarRadio.setEnabled(true);
	  //    m_streamingServerRadio.setEnabled(false);
		  
	  //    // BUGFIX 5487: "file server" without function for SLIDESTAR upload
	  //    m_fileServerRadio.setEnabled(false);
					  
	  //  // BUGFIX 5254: SLIDESTAR Profile: File upload preselected
	  //  // webServerRadio button is only preselected in non-profile mode and therefore switch to SLIDESTAR failed here
	  //  // always preselect SLIDESTAR now
	  //  //if (m_webServerRadio.isSelected())
	  //        m_slidestarRadio.setSelected(true);
	  //  //if (m_slidestarRadio.isSelected())
	  //        m_iServerType = PublisherWizardData.SRV_SLIDESTAR;
	  //}
   
      // Enable/Disable URL TextField
      if (m_protocolTextField.getText().length() > 0)
      {
         m_streamingUrlLabel.setEnabled(true);
         m_protocolLabel.setEnabled(true);
         m_protocolTextField.setEnabled(true);
         m_urlTextField.setEnabled(true);
      }
      else
      {
         m_streamingUrlLabel.setEnabled(false);
         m_protocolLabel.setEnabled(false);
         m_protocolTextField.setEnabled(false);
         m_urlTextField.setEnabled(false);
      }

      // Enable/Disable "Transfer Type" radios
      if ( (m_iServerType == PublisherWizardData.SRV_WEB_SERVER)
          || (m_iServerType == PublisherWizardData.SRV_STREAMING_SERVER) )
      {
         m_networkDriveRadio.setEnabled(true);
      }
      else
      {
         m_networkDriveRadio.setEnabled(false);
         if (m_networkDriveRadio.isSelected())
         {
            m_uploadRadio.setSelected(true);
            m_iTransferType = PublisherWizardData.TRANSFER_UPLOAD;
         }
      }
   }

   private void initProfileModeValues()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      m_strHttpProtocol = getProtocolFromUrl(ps.GetStringValue("strTargetUrlHttpServer"));
      if (m_strHttpProtocol.length() > 0)
      {
         m_strUrlPath = removeProtocolFromUrl(ps.GetStringValue("strTargetUrlHttpServer"));
      }
      else
      {
         initDefaultHttpProtocol();
         m_strUrlPath = "";
      }

      m_strStreamingProtocol = getProtocolFromUrl(ps.GetStringValue("strTargetUrlStreamingServer"));
      if (m_strStreamingProtocol.length() > 0)
      {
         m_strUrlPath = removeProtocolFromUrl(ps.GetStringValue("strTargetUrlStreamingServer"));
      }
      else
      {
         initDefaultStreamingProtocol();
         m_strUrlPath = "";
      }
	  
      // bugfix 5471 - YouTube upload should be preselected (for new profiles)
      if (m_pwData.m_bIsProfileMode && ps.IsNewlyCreatedProfile() && ps.GetBoolValue("bMp4YouTubeOptimized"))
         m_iServerType = PublisherWizardData.SRV_YOUTUBE;
   }

   private void initDefaultValues()
   {
      switch (m_iExportType)
      {
         case PublisherWizardData.EXPORT_TYPE_LECTURNITY:
            m_iServerType = PublisherWizardData.SRV_FILE_SERVER;
            break;
         case PublisherWizardData.EXPORT_TYPE_REAL:
            m_iServerType = PublisherWizardData.SRV_WEB_SERVER;
            break;
         case PublisherWizardData.EXPORT_TYPE_WMT:
            m_iServerType = PublisherWizardData.SRV_WEB_SERVER;
            break;
         case PublisherWizardData.EXPORT_TYPE_FLASH:
            m_iServerType = PublisherWizardData.SRV_WEB_SERVER;
            break;
         case PublisherWizardData.EXPORT_TYPE_VIDEO:
            // bugfix 5471 - YouTube upload should be preselected
            if (m_pwData.GetProfiledSettings().GetBoolValue("bMp4YouTubeOptimized"))
                m_iServerType = PublisherWizardData.SRV_YOUTUBE;
            else
                m_iServerType = PublisherWizardData.SRV_PODCAST_SERVER;
            break;
         default:
            break;
      }

      initDefaultHttpProtocol();
      initDefaultStreamingProtocol();
      m_strUrlPath = "";
   }

   private void initDefaultHttpProtocol()
   {
      switch (m_iExportType)
      {
         case PublisherWizardData.EXPORT_TYPE_LECTURNITY:
            m_strHttpProtocol = "";
            break;
         case PublisherWizardData.EXPORT_TYPE_REAL:
            m_strHttpProtocol = "http";
            break;
         case PublisherWizardData.EXPORT_TYPE_WMT:
            m_strHttpProtocol = "http";
            break;
         case PublisherWizardData.EXPORT_TYPE_FLASH:
            m_strHttpProtocol = "http";
            break;
         case PublisherWizardData.EXPORT_TYPE_VIDEO:
            m_strHttpProtocol = "";
            break;
         default:
            m_strHttpProtocol = "";
            break;
      }
   }

   private void initDefaultStreamingProtocol()
   {
      switch (m_iExportType)
      {
         case PublisherWizardData.EXPORT_TYPE_LECTURNITY:
            m_strStreamingProtocol = "";
            break;
         case PublisherWizardData.EXPORT_TYPE_REAL:
            m_strStreamingProtocol = "rtsp";
            break;
         case PublisherWizardData.EXPORT_TYPE_WMT:
            m_strStreamingProtocol = "mms";
            break;
         case PublisherWizardData.EXPORT_TYPE_FLASH:
            m_strStreamingProtocol = "rtmp";
            break;
         case PublisherWizardData.EXPORT_TYPE_VIDEO:
            m_strStreamingProtocol = "";
            break;
         default:
            m_strStreamingProtocol = "";
            break;
      }
   }

   private void createAndSetLocalUploadDirectory()
   {
      // Use the ProfiledSettings key "strTargetDirLocal" to set a local directory 
      // for the files to be created for the 'Upload' process.
      // For this local directory an "upload" subfolder should be created in the 'Source path'.
      ProfiledSettings ps =  m_pwData.GetProfiledSettings();
      String strTargetUpload = 
         FileUtils.extractPathFromPathFile(m_pwData.GetDocumentData().GetPresentationFileName());
      strTargetUpload = strTargetUpload + ps.STR_TARGET_DOC_DIR + File.separatorChar;

      if (!m_pwData.m_bIsProfileMode) // In "Profile mode" this entry should be left empty
         ps.SetStringValue("strTargetDirLocal", strTargetUpload);

      // Create the "TargetDocuments" directory, if it does not exist already
      File uploadDir = new File(strTargetUpload);
      if ( !(uploadDir.exists() && uploadDir.isDirectory()) )
         uploadDir.mkdir();
   }

   private String getProtocolFromUrl(String strUrl)
   {
      String strProtocol = "";
      int idx = strUrl.indexOf("://");
      if (idx > 0)
         return strUrl.substring(0, idx);

      return strProtocol;
   }

   private String removeProtocolFromUrl(String strUrl)
   {
      String strReducedUrl = strUrl;
      int idx = strUrl.indexOf("://");
      if (idx >= 0)
         strReducedUrl = strUrl.substring(idx+3);
      
      return strReducedUrl;
   }

   private void storeUrlTexts()
   {
      // Save protocol text before updating m_iServerType
      if (m_iServerType == PublisherWizardData.SRV_WEB_SERVER)
         m_strHttpProtocol = m_protocolTextField.getText();
      else if (m_iServerType == PublisherWizardData.SRV_STREAMING_SERVER)
         m_strStreamingProtocol = m_protocolTextField.getText();
      // Save Url path
      m_strUrlPath = m_urlTextField.getText();
   }

///   protected void helpCommand()
///   {
///      m_helpPopup.popupHelp();
///   }

   private class ServerRadioListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         storeUrlTexts();

         // Update m_iServerType
         if (e.getSource() == m_webServerRadio)
            m_iServerType = PublisherWizardData.SRV_WEB_SERVER;
         if (e.getSource() == m_streamingServerRadio)
            m_iServerType = PublisherWizardData.SRV_STREAMING_SERVER;
		 //if (e.getSource() == m_slidestarRadio)
		 //   m_iServerType = PublisherWizardData.SRV_SLIDESTAR;
         if (e.getSource() == m_podcastServerRadio)
            m_iServerType = PublisherWizardData.SRV_PODCAST_SERVER;
         if (e.getSource() == m_fileServerRadio)
            m_iServerType = PublisherWizardData.SRV_FILE_SERVER;
         if (e.getSource() == m_youTubeRadio)
             m_iServerType = PublisherWizardData.SRV_YOUTUBE;

         refreshDataDisplay();
      }
   }

   private class TargetRadioListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         storeUrlTexts();

         // Update m_iTransferType
         if (e.getSource() == m_networkDriveRadio)
            m_iTransferType = PublisherWizardData.TRANSFER_NETWORK_DRIVE;
         if (e.getSource() == m_uploadRadio)
            m_iTransferType = PublisherWizardData.TRANSFER_UPLOAD;

         refreshDataDisplay();
      }
   }

}
