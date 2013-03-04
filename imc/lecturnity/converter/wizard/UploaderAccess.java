package imc.lecturnity.converter.wizard;

import java.awt.Window;
import java.io.IOException;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.converter.DefaultProfiles;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.wizards.WizardPanel;

public class UploaderAccess
{  
   private static Localizer g_Localizer = null;
   
   static
   {
      try
      {
		  System.loadLibrary("Uploader");
      }
      catch (UnsatisfiedLinkError e)
      {
         JOptionPane.showMessageDialog(null,
             "The dynamic link library 'Uploader.dll' was not found in the path.",
             "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(0);
      }
      
      try
      {
         g_Localizer = new Localizer(
            "/imc/lecturnity/converter/wizard/TargetUploadWizardPanel_", "en");
      }
      catch (IOException exc)
      {
         // should not happen
         exc.printStackTrace();
      }
   } 
   
   public static class StringContainer
   {
      public String m_strContent;
   }
   
   public static class TransferSettings
   {
      public String m_strServer;
      public int m_iProtocol;
      public int m_iPort;
      public String m_strUser;
      public String m_strPass;
      public String m_strDir;
      
      public void GetFrom(ProfiledSettings data, boolean bUseFirst)
      {
         if (bUseFirst)
         {
            m_strServer = data.GetStringValue("strUploadFirstServer");
            m_iProtocol = data.GetIntValue("iUploadFirstProtocol");
            m_iPort = data.GetIntValue("iUploadFirstPort");
            m_strUser = data.GetStringValue("strUploadFirstUsername");
            m_strPass = data.GetStringValue("strUploadFirstPassword");
            m_strDir = data.GetStringValue("strUploadFirstTargetDir");
         }
         else
         {
            m_strServer = data.GetStringValue("strUploadSecondServer");
            m_iProtocol = data.GetIntValue("iUploadSecondProtocol");
            m_iPort = data.GetIntValue("iUploadSecondPort");
            m_strUser = data.GetStringValue("strUploadSecondUsername");
            m_strPass = data.GetStringValue("strUploadSecondPassword");
            m_strDir = data.GetStringValue("strUploadSecondTargetDir");
         }
      }
      
      public void SetTo(ProfiledSettings data, boolean bUseFirst)
      {
         if (bUseFirst)
         {
            data.SetStringValue("strUploadFirstServer", m_strServer);
            data.SetIntValue("iUploadFirstProtocol", m_iProtocol);
            data.SetIntValue("iUploadFirstPort", m_iPort);
            data.SetStringValue("strUploadFirstUsername", m_strUser);
            data.SetStringValue("strUploadFirstPassword", m_strPass);
            data.SetStringValue("strUploadFirstTargetDir", m_strDir);
         }
         else
         {
            data.SetStringValue("strUploadSecondServer", m_strServer);
            data.SetIntValue("iUploadSecondProtocol", m_iProtocol);
            data.SetIntValue("iUploadSecondPort", m_iPort);
            data.SetStringValue("strUploadSecondUsername", m_strUser);
            data.SetStringValue("strUploadSecondPassword", m_strPass);
            data.SetStringValue("strUploadSecondTargetDir", m_strDir);
         }
      }
   }

	public static class YtTransferSettings
	{
		public String m_strUser;
		public String m_strPass;
		public String m_strTitle;
		public String m_strDescription;
		public String m_strKeywords;
		public String m_strCategory;
		public int m_iPrivacy;
		

		public void GetFrom(ProfiledSettings data)
		{
			    // read empty values from registry
			    data.ReadUploadSettingsFromRegistry();
			    
				m_strUser = data.GetStringValue("strUploadYtUsername");
				m_strPass = data.GetStringValue("strUploadYtPassword");
				m_strTitle = data.GetStringValue("strUploadYtTitle");
				m_strDescription = data.GetStringValue("strUploadYtDescription");
				m_strKeywords = data.GetStringValue("strUploadYtKeywords");
				m_strCategory = data.GetStringValue("strUploadYtCategory");
				m_iPrivacy = data.GetIntValue("iUploadYtPrivacy");
		}

		public void SetTo(ProfiledSettings data)
		{
				data.SetStringValue("strUploadYtUsername", m_strUser);
				data.SetStringValue("strUploadYtPassword", m_strPass);
				data.SetStringValue("strUploadYtTitle", m_strTitle);
				data.SetStringValue("strUploadYtDescription", m_strDescription);
				data.SetStringValue("strUploadYtKeywords", m_strKeywords);
				data.SetStringValue("strUploadYtCategory", m_strCategory);
				data.SetIntValue("iUploadYtPrivacy", m_iPrivacy);
		}
	}
   
   public static int CheckUpload(boolean bUseFirst, ProfiledSettings data, WizardPanel pnlParent)
   {
      Window wndParent = SwingUtilities.windowForComponent(pnlParent);
      
      TransferSettings ts = new TransferSettings();
      ts.GetFrom(data, bUseFirst);        
      int iResult = CheckUpload(ts, wndParent);
         
//      if (iResult != 0)
//      {
//         JOptionPane.showMessageDialog(pnlParent, 
//                                       g_Localizer.getLocalized("ACCESS_SERVER")+"\n"
//                                       + ts.m_strServer + " ("+ts.m_strUser+")",
//                                       g_Localizer.getLocalized("ERROR"),
//                                       JOptionPane.ERROR_MESSAGE);
//      }

      return iResult;
   }

	public static int CheckYtUpload(ProfiledSettings data, String strRecLength, WizardPanel pnlParent)
	{
		Window wndParent = SwingUtilities.windowForComponent(pnlParent);
		YtTransferSettings ts = new YtTransferSettings();
		ts.GetFrom(data);
		System.out.println("Soare length = " + strRecLength);
		int iResult = CheckYtUpload(ts, strRecLength, wndParent);
		return iResult;
	}

   public static int StartUpload(boolean bUseFirst, ProfiledSettings data, 
                                 String[] aFiles, WizardPanel pnlParent)
   {
      return StartUpload(bUseFirst, data, aFiles, pnlParent, null);
   }
   
   public static int StartUpload(boolean bUseFirst, ProfiledSettings data, 
                                 String[] aFiles, WizardPanel pnlParent, 
                                 String strTargetSubDir)
   {
      TransferSettings ts = new TransferSettings();
      ts.GetFrom(data, bUseFirst); 
      
      if (strTargetSubDir != null && strTargetSubDir.length() > 0)
      {
         if (!ts.m_strDir.endsWith("/"))
            ts.m_strDir += "/";
         
         ts.m_strDir += strTargetSubDir;
      }
      
      int iResult = StartUpload(ts, aFiles);
         
//      if (iResult != 0)
//      {
//         JOptionPane.showMessageDialog(pnlParent, 
//                                       g_Localizer.getLocalized("ACCESS_SERVER")+"\n"
//                                       + ts.m_strServer + " ("+ts.m_strUser+")",
//                                       g_Localizer.getLocalized("ERROR"),
//                                       JOptionPane.ERROR_MESSAGE);
//      }

      return iResult;
   }

	public static int StartYtUpload(ProfiledSettings data, String[] aFiles, WizardPanel pnlPanel)
	{
		YtTransferSettings ts = new YtTransferSettings();
		ts.GetFrom(data);

		int iResult = StartYtUpload(ts, aFiles);
		return iResult;
	}
   
   public static int StartPodcast(String strChannelUrl, String strMediaUrl, String strTitle, String strAuthor,
                                  ProfiledSettings data,
                                  String[] aFiles, WizardPanel pnlParent)
   {
      TransferSettings tsChannel = new TransferSettings();
      tsChannel.GetFrom(data, true);        
      
      TransferSettings tsMedia = new TransferSettings();
      tsMedia.GetFrom(data, false);

	  return StartPodcast(strChannelUrl, strMediaUrl, strTitle, strAuthor, tsChannel, tsMedia, aFiles);     
   }
   
   /**
    * This method returns 1 if the selected direcory or file does not exist.
    * If the selected directory or file name exists then an underscore and a 
    * consecutive number (beginning with 2) is added to the directory or file 
    * name and rechecked again. 
    * The index of the next non-existing directory or file name is then returned.
    *
    * @param strDirOrFileName the directory or file name to be checked. 
    *    If strDirOrFileName is a file then it must be specified without suffix!
    * @param strSuffix the suffix (incl. the dot, e.g. ".xyz") of the filename to be checked.
    *    If strSuffix is not specified or an empty String then it is assumed that 
    *    strDirOrFileName is a directory name.
    * @param strExtension the extionsion which should be added to the directory/file name (e.g. "_l").
    */
   public static int GetIndexOfNextEmptyDirOrFileName(String strDirOrFileName, String strSuffix, String strExtension, 
                                                      boolean bIsFirst, ProfiledSettings data, WizardPanel pnlParent)
   {
      TransferSettings ts = new TransferSettings();
      ts.GetFrom(data, bIsFirst);        

      Window wndParent = SwingUtilities.windowForComponent(pnlParent);

      int idx = 1;
      int idxMax = 500;
      String strNextDirOrFileName = strDirOrFileName + strSuffix;
      
      // In case of SLIDESTAR upload no "CheckTargetExists" should be performed
      boolean bIsSlidestarUpload = data.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR
                                   && data.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER 
                                   && data.GetIntValue("iServerType") == PublisherWizardData.SRV_SLIDESTAR;
      if (bIsSlidestarUpload)
         return idx;

      boolean bAlreadyExists = (0 == CheckTargetExists(strNextDirOrFileName, ts, wndParent));
      if (!bAlreadyExists)
         return idx;

      boolean bIsDirectory = ((strSuffix == null) || strSuffix.equals(""));

      if (bIsDirectory)
      {
         // Check for Separator at the and of the String and remove it
         int nLen = strDirOrFileName.length();
         if (strDirOrFileName.endsWith("/"))
            strDirOrFileName = strDirOrFileName.substring(0, nLen-1);
      }

      do
      {
         idx++;
         strNextDirOrFileName = strDirOrFileName + strExtension + idx + strSuffix;
         bAlreadyExists = (0 == CheckTargetExists(strNextDirOrFileName, ts, wndParent));
      }
      while(bAlreadyExists && (idx <= idxMax));

      if (idx >= idxMax)
      {
         // There must be an error: the above loop seems to be endless
         return -1;
      }

      return idx;
   }

   public static int GetIndexOfNextEmptyDirName(String strDirName, String strExtension, boolean bIsFirst, 
                                                ProfiledSettings data, WizardPanel pnlParent)
   {
      return GetIndexOfNextEmptyDirOrFileName(strDirName, "", strExtension, bIsFirst, data, pnlParent);
   }

   public static int GetIndexOfNextEmptyFileName(String strFileName, String strExtension, boolean bIsFirst, 
                                                 ProfiledSettings data, WizardPanel pnlParent)
   {
      String strSuffix = "";
      int nLastDot = strFileName.lastIndexOf(".");
      if (nLastDot > 0)
      {
         strSuffix = strFileName.substring(nLastDot);
         strFileName = strFileName.substring(0, nLastDot);
      }

      return GetIndexOfNextEmptyDirOrFileName(strFileName, strSuffix, strExtension, bIsFirst, data, pnlParent);
   }

   public static boolean DoesTargetFileExist(String strTargetPathFile, boolean bIsFirst, 
                                             ProfiledSettings data, WizardPanel pnlParent)
   {
      TransferSettings ts = new TransferSettings();
      ts.GetFrom(data, bIsFirst);        

      Window wndParent = SwingUtilities.windowForComponent(pnlParent);

      // In case of SLIDESTAR upload no "CheckTargetExists" should be performed
      boolean bIsSlidestarUpload = data.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR
                                   && data.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER 
                                   && data.GetIntValue("iServerType") == PublisherWizardData.SRV_SLIDESTAR;
	  boolean bIsYouTubeUpload = data.GetIntValue("iServerType") == PublisherWizardData.SRV_YOUTUBE;

	  if (bIsYouTubeUpload)
		  return false;

	  if (bIsSlidestarUpload)
		  return false;

      int res = CheckTargetExists(strTargetPathFile, ts, wndParent);

      if (res == 0) // Target exists
         return true; 
      else
         return false;
   }

   public static boolean IsNotEmptyDir(String strTargetPath, boolean bIsFirst, 
                                       ProfiledSettings data, WizardPanel pnlParent)
   {
      TransferSettings ts = new TransferSettings();
      ts.GetFrom(data, bIsFirst);        

      Window wndParent = SwingUtilities.windowForComponent(pnlParent);
      
      return IsNotEmptyDir(strTargetPath, ts, wndParent);
   }

   public static double GetUploadProgress(int nPrecision)
   {
      // The return value of GetUploadProgress() will sometimes be not precise enough, 
      // e.g. 99.999999999999 is returned instead of 100.0
      int nFactor = 10 ^ nPrecision;
      double dProgress = GetUploadProgress() * nFactor;
      long lProgress = Math.round(dProgress);
      dProgress = (double)( lProgress / (double)nFactor );
      
      return dProgress;
   }
   
   public static int ShowErrorMessageForErrorCode(int iErrorCode, WizardPanel pnlParent)
   {
      System.out.println("*** ShowErrorMessageForErrorCode(" + iErrorCode + ")");
      String strError = "";
      if ((iErrorCode <= -2) && (iErrorCode >= -6))
         strError = g_Localizer.getLocalized("" + iErrorCode);
      else // No defined error --> show generic error message
      {
         ShowGenericErrorMessage(pnlParent);
         return -1;
      }
      
      Object[] options = { g_Localizer.getLocalized("VERIFY_SETTINGS"),
                           g_Localizer.getLocalized("RETRY"), 
                           g_Localizer.getLocalized("CANCEL") };

      int res = 
         JOptionPane.showOptionDialog(pnlParent, 
                                      strError, 
                                      g_Localizer.getLocalized("ERROR"), 
                                      JOptionPane.YES_NO_OPTION, JOptionPane.ERROR_MESSAGE, 
                                      null, options, options[1]);

      return res;
   }
   
   public static void ShowGenericErrorMessage(WizardPanel pnlParent)
   {
      String strGenericError = GetLastError();
      String strError = g_Localizer.getLocalized("GENERIC_ERROR1") 
                        + strGenericError 
                        + g_Localizer.getLocalized("GENERIC_ERROR2");

      JOptionPane.showMessageDialog(pnlParent, 
                                    strError,
                                    g_Localizer.getLocalized("ERROR"),
                                    JOptionPane.ERROR_MESSAGE);
   }

   // Bugfix 5307/4562: disable data directory input field for SLIDESTAR export
   // additional parameter bIsSlidestar added
   public static native int ShowTransferConfigDialog(TransferSettings ts, Window wndParent, boolean bIsSlidestar);
   public static native int GetTransferData(TransferSettings ts);
   public static native int ShowPodcastConfigDialog(
      String strChannel, String strChannelUrl, String strMediaUrl, 
      TransferSettings tsChannel, TransferSettings tsMedia, Window wndParent);
   public static native int GetPodcastData(
      StringContainer scChannel, StringContainer scChannelUrl, StringContainer scMediaUrl,
      TransferSettings tsChannel, TransferSettings tsMedia);
   
   public static native boolean IsNotEmptyDir(String strTarget, 
                                              TransferSettings ts, Window wndParent);
   public static native int CheckTargetExists(String strTarget, 
                                              TransferSettings ts, Window wndParent);
   public static native int CheckUpload(TransferSettings ts, Window wndParent);
   public static native int StartUpload(TransferSettings ts, String[] aFiles);
   public static native int IsSlidestar(boolean bIsSlidestar);
   public static native int StartPodcast(String strChannelUrl, String strMediaUrl, String strTitle, String strAuthor,
                                         TransferSettings tsChannel, 
                                         TransferSettings tsMedia, String[] aFiles);
   public static native double GetUploadProgress();
   
   public static native String GetLastError();
   public static native void CancelSession();
   public static native void StartSession();
   public static native void FinishSession();

   public static native int ShowYtTransferConfigDialog(YtTransferSettings ts, Window wndParent);
   public static native int GetYtTransferData(YtTransferSettings ts);
	public static native int CheckYtUpload(YtTransferSettings ts, String strRecLength, Window wndParent);
   public static native int StartYtUpload(YtTransferSettings ts, String[] aFiles);
}
  