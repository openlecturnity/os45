package imc.lecturnity.converter;

import imc.lecturnity.converter.wizard.PublisherWizardData;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;

import javax.swing.*;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.TreeSet;
import java.util.Iterator;
import java.util.Random;

public class ProfiledSettings
{
    static {
        try {
            System.loadLibrary("lsutl32");
        } catch (UnsatisfiedLinkError e) {
            javax.swing.JOptionPane.showMessageDialog(
                null, "The dynamic link library 'lsutl32.dll' was not found in the path. Reinstalling the application may help.",
                "Error", javax.swing.JOptionPane.ERROR_MESSAGE);
            System.exit(-1);
        }
   }
       
   public static final String STR_TARGET_DOC_DIR = "TargetDocuments";
   public static final String STR_RM_DIR = "RealMedia";
   public static final String STR_WM_DIR = "WindowsMedia";
   public static final String STR_FLASH_DIR = "Flash";

   public class ProfileInfo
   {
      // these informations are also written to/read from the profile
      public long m_id;
      public int m_iFormatVersion;
      public String m_strTitle;
      public int m_iDocumentType; // encodes information about the streams
      public long m_lCreateTimeMs;
      public String m_strFile;
      
      // information only (not written but set during reading)
      public boolean m_bIsReadonly;
      
      public ProfileInfo()
      {
      	Init();
      }
      
      public void Init()
      {	
      }
   }
   
   public class ProfileEntry
   {
      String m_strKey;
      String m_strValue;
      
      public ProfileEntry()
      {
      	Init();
      }
      
      public ProfileEntry(String strKey, String strValue)
      {
         m_strKey = strKey;
         m_strValue = strValue;
      }
      
      public void Init()
      {
         m_strKey = "";
         m_strValue = "";	
      }
            
      void SetEntry(String strKey, String strValue)
      {
         m_strKey = strKey;
         m_strValue = strValue;
      }
      
      String getKey()
      {
      	return m_strKey;
      }
      
      String getValue()
      {
      	return m_strValue;
      }
   }
   
   /**
    * @returns the active Profile settings; null if none is active.
    */
   public static native ProfileInfo GetActiveProfile();
   
   /**
    * Shows the manage dialog. Only returns after Ok or Cancel is selected.
    * @param strLrdFilename The name of the document with which the
    *                       Publisher was started. It is needed for creating a
    *                       new or editing an old profile (their streams must match).
    * @returns the active Profile settings; null if none is active.
    */
   public static native ProfileInfo ShowFormatDialog(JFrame callingPanel, String strLrdFilename);
   /**
    * Is called if the Cancel button was clicked and thus no profile will be 
    * written (normal end signal).
    */
   public static native void SignalCancel();
   
   // file name is chosen automatically by the "backend" (must be unique; maybe use "id")
   // directory is chosen automatically by the "backend" (see document)
   private native int WriteToProfile(ProfileInfo info, ProfileEntry[] aEntries);
   private native ProfileEntry[] ReadFromProfile(ProfileInfo info);
   // especially for ReadFromProfile
   private native int GetLastProfileErrorCode();

   
   private ProfileInfo m_ThisInfo;
   private HashMap m_mapValues = new HashMap(200);
   
   private final int FORMAT_VERSION = 1;
   
   public ProfiledSettings() // for reading one; use "Load()" below
   {
      m_ThisInfo = new ProfileInfo();

      InitProfileHashTable();
   }
   
   public ProfiledSettings(int iDocumentType) // for creating a new one
   {
      m_ThisInfo = new ProfileInfo();
      
      Random r = new Random();
      m_ThisInfo.m_id = 0;
      while (m_ThisInfo.m_id >= 0 && m_ThisInfo.m_id <= 1000) // may not be 0-1000 (reserved for default profiles)
         m_ThisInfo.m_id = r.nextLong();
      m_ThisInfo.m_iFormatVersion = FORMAT_VERSION;
      m_ThisInfo.m_strTitle = "<unnamed>";
      m_ThisInfo.m_lCreateTimeMs = System.currentTimeMillis();
      m_ThisInfo.m_iDocumentType = iDocumentType;
      m_ThisInfo.m_bIsReadonly = false;
      
      InitProfileHashTable();
   }
   
   public void SetType(int iType)
   {
      m_ThisInfo.m_iDocumentType = iType;
   }
 
   public int GetType()
   {
      return m_ThisInfo.m_iDocumentType;
   }
   
   public String GetName()
   {
      return m_ThisInfo.m_strTitle;
   }
   
   public void SetName(String strTitle)
   {
      m_ThisInfo.m_strTitle = strTitle;
   }
   
   public void SetId(long id)
   {
      m_ThisInfo.m_id = id;
   }
   
   public boolean IsDefaultProfile()
   {
      return DefaultProfiles.isDefaultProfileId(m_ThisInfo.m_id);
   }
   
   private boolean m_bNewlyCreated = true;
   public boolean IsNewlyCreatedProfile()
   {
      return m_bNewlyCreated;
   }   
   
   public int Load(long id, String strFile) {      
      m_ThisInfo.m_id = id;
      m_ThisInfo.m_strFile = strFile;
      
      if ( DefaultProfiles.isDefaultProfileId(id) ) {
          // since 4.0.p6: always create default profile and do not load/store profile file
          // reset profileSettings
          m_mapValues.clear();
          InitProfileHashTable();
          // set Default Profile settings
          DefaultProfiles.SetDefaultProfileValues(this, id);
//        JOptionPane.showMessageDialog(null, "DEBUG INFO:\n\"Debug Profile\"\nwith ID "+m_ThisInfo.m_id+"\nType = "+GetType());

      } else {
          // load custom profile
          
          ProfileEntry[] aEntries = ReadFromProfile(m_ThisInfo);
    
//          if ( aEntries==null)
//              JOptionPane.showMessageDialog(null, "DEBUG INFO:\nCreated new style \"Generic Profile\"\nwith ID "+m_ThisInfo.m_id);
//          else if (GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN)
//              JOptionPane.showMessageDialog(null, "DEBUG INFO:\nLoaded new style \"Generic Profile\"\nwith ID "+m_ThisInfo.m_id);
//          else
//              JOptionPane.showMessageDialog(null, "DEBUG INFO:\nLoaded old style profile for document type "+GetType()+"\nwith ID "+m_ThisInfo.m_id);
          
          // TODO maybe also consider error codes (GetLastProfileErrorCode()) here
          // TODO: is this check required? in which case are no values returned and no error code set?
          if (aEntries == null || aEntries.length == 0)
          {
             int iError = GetLastProfileErrorCode();
             
             if (iError == 0)
             {
                System.err.println("ProfiledSettings: Error code == 0 but no elements returned.");
                return -1;
             }
          }
          
          // PZI: remove clip structuring flag in order to distinguish old profiles from new ones
          m_mapValues.remove("bStructuredClips");
          
          // mark as "newly created custom profile" if no values could be loaded
          m_bNewlyCreated = (aEntries == null);
          
          if (aEntries != null)
          {
             for (int i=0; i<aEntries.length; ++i)
             {	
             	String strKey = aEntries[i].getKey();
                String strValue = aEntries[i].getValue();
                
                if (strKey.length() <= 0)
                   return -2; // ??? format error; also see general error -1 above
                
            		m_mapValues.put(strKey, strValue);
             }
          }
          
          // PZI: set clip structuring flag for old profiles
          if(!IsDefined("bStructuredClips")) {
             boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
             SetBoolValue("bStructuredClips", GetType() != PublisherWizardData.DOCUMENT_TYPE_DENVER || bIsGenericProfile );
          }
      }
      
      return 0;
   }
   
   public int Write()
   {
      int iElementCount = m_mapValues.size();
      ArrayList aEntries = new ArrayList(iElementCount + 20);
  
      Iterator iterKeys = m_mapValues.keySet().iterator();
      
      while (iterKeys.hasNext())
      {
         String key = (String)iterKeys.next();
         String value = (String)m_mapValues.get(key);
         
         ProfileEntry pe = new ProfileEntry(key, value);
         aEntries.add(pe);
      }
      
      ProfileEntry[] aEntriesPlain = new ProfileEntry[aEntries.size()];
      aEntries.toArray(aEntriesPlain);
      
      return WriteToProfile(m_ThisInfo, aEntriesPlain);
   }
   
   public boolean IsDefined(String strKey)
   {
      return m_mapValues.containsKey(strKey);
   }
   
   public void SetIntValue(String strKey, int iValue)
   {
      m_mapValues.put(strKey, iValue+"");
   }
   
   public void SetBoolValue(String strKey, boolean bValue)
   {
      m_mapValues.put(strKey, bValue+"");
   }
   
   public void SetStringValue(String strKey, String strValue)
   {
      m_mapValues.put(strKey, strValue);
   }
   
   public int GetIntValue(String strKey)
   {
      if (IsDefined(strKey))
      {
         String strValue = (String)m_mapValues.get(strKey);
         int iValue = 0;
         try
         {
            iValue = Integer.parseInt(strValue);
         }
         catch (NumberFormatException exc)
         {
            exc.printStackTrace();
         }
         return iValue;
      }
      else
         return 0;
   }
   
   public boolean GetBoolValue(String strKey)
   {
      if (IsDefined(strKey))
      {
         String strValue = (String)m_mapValues.get(strKey);
         return strValue.equalsIgnoreCase("true");
      }
      else
         return false;
   }
   
   public String GetStringValue(String strKey)
   {
      if (IsDefined(strKey))
      {
         String strValue = (String)m_mapValues.get(strKey);
         if (strValue != null)
            return strValue;
         else
            return "";
      }
      else
         return "";
   }
   
   /**
    * @returns A string (if defined) which ends with a directory separator char.
    */
   public String GetPathValue(String strKey)
   {
      String strValue = GetStringValue(strKey);
      if (!strValue.endsWith(File.separator))
         strValue += File.separator;
      return strValue;
   }
   
   /**
    * @returns A String with the full path and target file name, but without extension
    */
   public String GetTargetFileName()
   {
      String strTargetPath = GetStringValue("strTargetDirLocal");
      if (GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER)
      {
         if (GetIntValue("iTransferType") == PublisherWizardData.TRANSFER_NETWORK_DRIVE)
            strTargetPath = GetStringValue("strTargetDirHttpServer");
         else // Upload
            strTargetPath = GetStringValue("strUploadFirstTargetDir");
      }

      String strTargetFileName = strTargetPath + GetStringValue("strTargetPathlessDocName");
      return strTargetFileName;
   }

   /**
    * @returns A String with the target subdirectory name depending on the selected export type
    */
   public String GetExportTypeTargetSubDir()
   {
      String strTargetSubDir = ""; // Default for LPD and MP4 export
      
      if (GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_REAL)
         strTargetSubDir = STR_RM_DIR;
      else if (GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_WMT)
         strTargetSubDir = STR_WM_DIR;
      else if (GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH)
         strTargetSubDir = STR_FLASH_DIR;
      
      return strTargetSubDir;
   }
  
   public String toString()
   {
      StringBuffer sb = new StringBuffer();
      sb.append(m_ThisInfo.m_strTitle+": ");
      sb.append(m_ThisInfo.m_id+"\n");
      sb.append("Output: "+GetIntValue("iExportType")+" Document: "+GetType());
      
      return sb.toString();
   }


   public void Reset()
   {
      InitProfileHashTable();
   }
   
   private void InitProfileHashTable()
   {
      SetIntValue("iExportType", PublisherWizardData.EXPORT_TYPE_LECTURNITY);
      SetIntValue("iReplayType", PublisherWizardData.REPLAY_LOCAL);
      SetIntValue("iServerType", PublisherWizardData.SRV_FILE_SERVER);
      SetIntValue("iTransferType", PublisherWizardData.TRANSFER_NETWORK_DRIVE);

      SetIntValue("iControlBarVisibility", PublisherWizardData.GUI_ELEMENT_ACTIVE);
      SetIntValue("iDocumentStructureVisibility", PublisherWizardData.GUI_ELEMENT_ACTIVE);
      SetIntValue("iPlayerConfigButtonsVisibility", PublisherWizardData.GUI_ELEMENT_ACTIVE);
      SetIntValue("iPlayerSearchFieldVisibility", PublisherWizardData.GUI_ELEMENT_ACTIVE);
      SetIntValue("iPluginContextMenuVisibility", PublisherWizardData.GUI_ELEMENT_ACTIVE);
      SetIntValue("iNavigationButtonsVisibility", PublisherWizardData.GUI_ELEMENT_ACTIVE);
      SetIntValue("iTimeDisplayVisibility", PublisherWizardData.GUI_ELEMENT_ACTIVE);
      SetIntValue("iTimeLineVisibility", PublisherWizardData.GUI_ELEMENT_ACTIVE);

      SetBoolValue("bDoAutostart", false);

      // Obsolete: "strTargetFileName": Can not be used in Profile mode! 
      // --> Use "GetTargetFileName()" instead.
      ///SetStringValue("strTargetFileName", "");
      SetStringValue("strTargetPathlessDocName", "");
      SetStringValue("strTargetDirLocal", "");
      SetStringValue("strTargetDirHttpServer", "");
      SetStringValue("strTargetUrlHttpServer", "");
      SetStringValue("strTargetDirStreamingServer", "");
      SetStringValue("strTargetUrlStreamingServer", "");
      // For generic created subdirectories:
      SetStringValue("strUploadAppendPath", "");

      // Bugfix 5606: "iSlideSizeType" - SLIDESIZE_ORIGINAL instead of SLIDESIZE_STANDARD
      SetIntValue("iSlideSizeType", PublisherWizardData.SLIDESIZE_ORIGINAL);
      SetIntValue("iSlideWidth", -1);
      SetIntValue("iSlideHeight", -1);

      SetIntValue("iVideoFramerateTenths", PublisherWizardData.STD_FRAMERATE_TENTHS_VIDEO);
      SetIntValue("iClipFramerateTenths", PublisherWizardData.STD_FRAMERATE_TENTHS_CLIPS);

      SetIntValue("iVideoSizeType", PublisherWizardData.VIDEOSIZE_STANDARD);
      SetIntValue("iVideoWidth", -1);
      SetIntValue("iVideoHeight", -1);

      SetIntValue("iClipSizeType", PublisherWizardData.CLIPSIZE_MAXSLIDE); // CLIPSIZE_STANDARD);
      SetIntValue("iMaxCustomClipWidth", -1);
      SetIntValue("iMaxCustomClipHeight", -1);
      SetBoolValue("bShowClipsOnSlides", true);

      SetBoolValue("bHideLecturnityIdentity", false);

      SetStringValue("strLogoImageName", "");
      SetStringValue("strLogoImageUrl", "");

      SetBoolValue("bScormEnabled", false);
      SetBoolValue("bScormStrict", false);
      SetStringValue("strScormSettings", "");

      SetBoolValue("bLpdCompressAudio", true);
      SetBoolValue("bLpdAddToCdProject", false);
      //SetBoolValue("bLpdDoQuickStart", false); // == "bDoAutostart"
      SetBoolValue("bLpdStartInFullScreen", false);
      SetBoolValue("bLpdEmbedFonts", false);
      SetBoolValue("bLpdUseThumbnails", true);
      SetBoolValue("bLpdThumbnailsSizeSmall", true);
      SetBoolValue("bLpdThumbnailsSizeMedium", true);
      SetBoolValue("bLpdThumbnailsSizeLarge", true);

      SetBoolValue("bFlashCreateWebsite", true);
      SetBoolValue("bFlashDisplayLogo", false);
      SetBoolValue("bFlashDisplayTitle", true);
      SetBoolValue("bFlashDoNotCutVideos", false);
      SetBoolValue("bFlashOptimizeClipSync", false);
      SetBoolValue("bFlashStartInOptimizedSize", false);
      SetIntValue("iFlashMovieSizeType", PublisherWizardData.MOVIESIZE_FLEXIBLE);
      SetIntValue("iFlashMovieWidth", -1);
      SetIntValue("iFlashMovieHeight", -1);
      SetIntValue("iFlashTemplateType", PublisherWizardData.TEMPLATE_FLASH_FLEX);
      SetIntValue("iFlashVersion", 7);
      SetIntValue("iFlashAudioBitrate", PublisherWizardData.STD_BITRATE_AUDIO_FLASH);
      SetIntValue("iFlashVideoBitrate", -1); //PublisherWizardData.STD_BITRATE_VIDEO_FLASH);
      SetIntValue("iFlashClipBitrate", -1); //PublisherWizardData.STD_BITRATE_CLIPS_FLASH);
      SetIntValue("iFlashColorBasic", 0xEAE7D7);
      SetIntValue("iFlashColorBackground", 0x2D3351);
      SetIntValue("iFlashColorText", 0xEAE7D7);

      SetIntValue("iRealMinimalVersion", 8);
      SetBoolValue("bRealAutoVideoQuality", true);
      SetIntValue("iRealVideoQuality", 0);
      SetIntValue("iRealPossibleBandwidths", 9);
      SetBoolValue("bRealBitrate0", false);     // 28k Modem
      SetBoolValue("bRealBitrate1", false);     // 56k Modem
      SetBoolValue("bRealBitrate2", true);      // Single ISDN
      SetBoolValue("bRealBitrate3", false);     // Dual ISDN
      SetBoolValue("bRealBitrate4", false);     // DSL Cable
      SetBoolValue("bRealBitrate5", true);      // Corporate LAN
      SetBoolValue("bRealBitrate6", false);     // 256k DSL
      SetBoolValue("bRealBitrate7", false);     // 384k DSL
      SetBoolValue("bRealBitrate8", true);      // 512k DSL
      SetBoolValue("bRealPlayer8", true);
      SetBoolValue("bRealPlayer7", false);
      SetBoolValue("bRealPlayer6", false);
      SetBoolValue("bRealServer8", true);
      SetBoolValue("bRealServer7", false);
      SetBoolValue("bRealServer6", false);

      SetBoolValue("bWmUseAdvancedSuffix", false);
      SetIntValue("iWmVideoProfileType", PublisherWizardData.PLAYER_CP); //PublisherWizardData.PLAYER_70);
      SetIntValue("iWmClipProfileType", PublisherWizardData.PLAYER_CP); //PublisherWizardData.PLAYER_70);
      SetIntValue("iWmCodecType", PublisherWizardData.WM_CODEC_90); //PublisherWizardData.WM_CODEC_80);
      SetIntValue("iWmBandwidthType", PublisherWizardData.WM_BANDWIDTH_DSL);
      SetIntValue("iWmAudioBitrate", PublisherWizardData.STD_BITRATE_AUDIO_WM);
      SetIntValue("iWmVideoBitrate", PublisherWizardData.STD_BITRATE_VIDEO_WM);
      SetIntValue("iWmClipBitrate", PublisherWizardData.STD_BITRATE_CLIPS_WM);

      SetBoolValue("bRmWmUseDataReduction", true);
      SetBoolValue("bRmWmUseDynamic", true);
      SetBoolValue("bRmWmUsePointer", true);
      SetIntValue("iRmWmScreenResolutionIndex", 0);
      SetStringValue("strRmWmTemplateConfigFilePath", "");
      SetStringValue("strRmWmTemplateConfigParameterList", "");

      SetBoolValue("bMp4EmbedClips", true);
      SetBoolValue("bMp4VideoInsteadOfPages", false);
      SetBoolValue("bMp4ChangeDetails", false);
      SetIntValue("iMp4QualityType", PublisherWizardData.MP4_QUALITY_HIGH);
      SetIntValue("iMp4AudioBitrate", PublisherWizardData.STD_BITRATE_AUDIO_MP4);
      SetIntValue("iMp4FrameRate", 10);
      SetIntValue("iMp4VideoQFactor", 2); // lower is better
      // see VideoSettingsWizardPanel.getInitValues()
      // 0: 100%
      // 1:  95%
      // 2:  95%
      // 3:  90%
      // 4:  85%
      // 5:  85%
      // ...
      
      // BUGFIX 5894: new default resolution for MP4 (physical iPod/iPhone resolution 480 x 320)
      //SetIntValue("iMp4VideoWidth", 320);
      //SetIntValue("iMp4VideoHeight", 240);
      SetIntValue("iMp4VideoWidth", 480);
      SetIntValue("iMp4VideoHeight", 320);

      SetStringValue("strUploadFirstServer", "");
      SetIntValue("iUploadFirstProtocol", PublisherWizardData.UPLOAD_SCP);
      SetIntValue("iUploadFirstPort", 22);
      SetStringValue("strUploadFirstUsername", "");
      SetStringValue("strUploadFirstPassword", "");
      SetStringValue("strUploadFirstTargetDir", "");
      SetStringValue("strUploadSecondServer", "");
      SetIntValue("iUploadSecondProtocol", PublisherWizardData.UPLOAD_SCP);
      SetIntValue("iUploadSecondPort", 22);
      SetStringValue("strUploadSecondUsername", "");
      SetStringValue("strUploadSecondPassword", "");
      SetStringValue("strUploadSecondTargetDir", "");
      SetStringValue("strUploadMediaUrl", "");
      SetStringValue("strUploadChannelUrl", "");
      SetStringValue("strUploadMetadataTitle", "");
      
      SetStringValue("strPodcastChannelName", "");
      SetStringValue("strPodcastChannelUrl", "");
      SetStringValue("strPodcastMediaUrl", "");
      
      // PZI: store structuring mode
      SetBoolValue("bStructuredClips", true);
      
      // Sort the HashMap by Key
      m_mapValues = getSortedMap(m_mapValues, true);
   }

   private HashMap getSortedMap(HashMap hmap, boolean bSortByKey)
	{
      HashMap map = new LinkedHashMap();
      ArrayList mapKeys = new ArrayList(hmap.keySet());
      ArrayList mapValues = new ArrayList(hmap.values());
      hmap.clear();
      
      // Sort by key or by value?
      TreeSet sortedSet;
      if (bSortByKey)
         sortedSet = new TreeSet(mapKeys);
      else
         sortedSet = new TreeSet(mapValues);

      Object[] sortedArray = sortedSet.toArray();
      int size = sortedArray.length;
      //Ascending sort
      for (int i = 0; i < size; ++i)
      {
         if (bSortByKey)
            map.put(sortedArray[i], mapValues.get(mapKeys.indexOf(sortedArray[i])));
         else
            map.put(mapKeys.get(mapValues.indexOf(sortedArray[i])), sortedArray[i]);
      }

      return map;
	}
   
   public void ReadUploadSettingsFromRegistry() {
       if ( GetIntValue("iServerType") == PublisherWizardData.SRV_YOUTUBE ) {
           // read user-specific YouTube parameters from registry
           if (GetStringValue("strUploadYtUsername").length()==0) {
               String strUsername = NativeUtils.getRegistryValue("HKCU", 
                       "Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default", "UserName");
               if (strUsername != null && strUsername.length()>0)
                   SetStringValue("strUploadYtUsername",strUsername);
           }
           if (GetStringValue("strUploadYtPassword").length()==0) {
               String strPassword = NativeUtils.getRegistryValue("HKCU", 
                       "Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default", "Password");
               if (strPassword != null && strPassword.length()>0)
                   SetStringValue("strUploadYtPassword",strPassword);
           }
           
           if (GetStringValue("strUploadYtDescription").length()<2) {
               String strDescription = NativeUtils.getRegistryValue("HKCU", 
                       "Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default", "Description");
               if (strDescription != null && strDescription.length()>1)
                   SetStringValue("strUploadYtDescription",strDescription);
               else
                   SetStringValue("strUploadYtDescription","LECTURNITY");
           }           

           if (GetStringValue("strUploadYtCategory").length()<2) {
               String strCategory = NativeUtils.getRegistryValue("HKCU", 
                       "Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default", "Category");
               if (strCategory != null && strCategory.length()>1)
                   SetStringValue("strUploadYtCategory",strCategory);
               else
                   // set default
                   SetStringValue("strUploadYtCategory","Education");
           }

           if ( IsNewlyCreatedProfile() ) {
               // read privacy only for new profiles
               // because cannot decide whether flag already is set in profile or not
               int iPrivacy = 1; // default: set privacy
               try {
                   // hack to read "Privacy" from Registry stored as DWORD
                   String strPrivacy = NativeUtils.getRegistryValue("HKCU", 
                       "Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default", "Privacy");
                   if (strPrivacy.length() == 0)
                       iPrivacy = 0;
                   else
                       iPrivacy = strPrivacy.getBytes()[0];
               } catch (Exception e) {}
               SetIntValue("iUploadYtPrivacy", iPrivacy);
           }
           
       } else if (GetIntValue("iServerType") == PublisherWizardData.SRV_SLIDESTAR) {
           // read user specific SLIDESTAR parameters from registry
           if (GetStringValue("strUploadFirstServer").length()==0) {
               String strHost = NativeUtils.getRegistryValue("HKCU", 
                       "Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Slidestar_Default", "HostName");
               if (strHost != null && strHost.length()>0)
                   SetStringValue("strUploadFirstServer",strHost);
           }
           if (GetStringValue("strUploadFirstUsername").length()==0) {
               String strUsername = NativeUtils.getRegistryValue("HKCU", 
                       "Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Slidestar_Default", "UserName");
               if (strUsername != null && strUsername.length()>0)
                   SetStringValue("strUploadFirstUsername",strUsername);
           }
           if (GetStringValue("strUploadFirstPassword").length()==0) {
               String strPassword = NativeUtils.getRegistryValue("HKCU", 
                       "Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Slidestar_Default", "Password");
               if (strPassword != null && strPassword.length()>0)
                   SetStringValue("strUploadFirstPassword",strPassword);
           }
       }
   }
   
   public  void AdjustTargetNameAndDirectory(DocumentData dd) {
       boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
       if ( bIsGenericProfile ) {    
           // Bugfix 5260: only compute target path if not specified by profile
           String strTargetDir = GetStringValue("strTargetDirLocal");
           if(strTargetDir == null || strTargetDir.length()==0) {
              strTargetDir = dd.GetPresentationPath();
           
              // set output folder   
              strTargetDir += (ProfiledSettings.STR_TARGET_DOC_DIR + File.separator);
              switch (GetIntValue("iExportType")) {
              case PublisherWizardData.EXPORT_TYPE_REAL:
                 strTargetDir += (ProfiledSettings.STR_RM_DIR + File.separator);
                 break;
              case PublisherWizardData.EXPORT_TYPE_WMT:
                 strTargetDir += (ProfiledSettings.STR_WM_DIR + File.separator);
                 break;
              case PublisherWizardData.EXPORT_TYPE_FLASH:
                 strTargetDir += (ProfiledSettings.STR_FLASH_DIR + File.separator);
                 break;
              default:
                 break;
              }
              SetStringValue("strTargetDirLocal",strTargetDir);
           }
           
           // set doc name (w/o path or ending) in profile - used to create subfolder
           String strTargetDocName = new File(dd.GetPresentationFileName()).getName();
           strTargetDocName = strTargetDocName.substring(0, strTargetDocName.lastIndexOf('.'));
           SetStringValue("strTargetPathlessDocName", strTargetDocName);           
       } else {
           // Bugfix 5258: "Publish" with SLIDESTAR profile leads to NullPointerException
           // "Publish advanced" mode sets "strTargetPathlessDocName" in SelectPresentationWizardPanel
           // which is not called in "Publish" mode (direct mode) but required
           String strTargetDocName = dd.GetPresentationFileName();
           strTargetDocName = FileUtils.extractNameFromPathFile(strTargetDocName);
           SetStringValue("strTargetPathlessDocName", strTargetDocName);
       }
//       JOptionPane.showMessageDialog(null, "bIsGenericProfile = "+bIsGenericProfile+"\n"
//               +"strTargetDirLocal = "+GetStringValue("strTargetDirLocal") +"\n"
//               +"strTargetPathlessDocName = "+GetStringValue("strTargetPathlessDocName"));
    }
}