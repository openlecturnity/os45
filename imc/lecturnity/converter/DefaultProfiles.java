package imc.lecturnity.converter;

import imc.lecturnity.converter.wizard.PublisherWizardData;

/* REVIEW UK
 * GetProfileId(): if (bIsPowerTrainerMode) "overwrites" the whole switch above
 *   -> switch should only be executed if !bIsPowerTrainerMode
 * isDefaultProfileId() -> IsDefaultProfileId()
 * // profileSettings should already be reset before calling this method
 *   -> Expected side effect is not good
 * Several comments with // since 4.0.p6
 *   -> Comments should normally describe the current status and not the past/changes
 *   -> Maybe with a postfix "this was changed with 4.0.p6"
 * EXPORT_TYPE_VIDEO_YOUTUBE: adding magic numbers
 *   -> Is the logic behind (default) profile ids described somehwere?
 *   -> If so mention that here otherwise document it
 * SetDefaultSettings() has lots of double code; e.g. "iVideoWidth" is always the same
 *   -> only differences should be in the "cases"
 * (int) iProfileId / 100
 *   -> When exactly is the cast performed? (It does not matter here but it is confusing.)
 * SetDefaultSettings(profiledSettings, (int)iProfileId)
 *   -> This cast here is also confusing: Why does this always work?
 *   -> In this case better use a long as parameter and do the calculations
 *   -> (with comment) in the method
 */

public class DefaultProfiles {

   static final int EXPORT_TYPE_FLASH_POWERTRAINER = PublisherWizardData.EXPORT_TYPE_FLASH + 10;
   static final int EXPORT_TYPE_FLASH_SLIDESTAR = PublisherWizardData.EXPORT_TYPE_FLASH + 20;
   static final int EXPORT_TYPE_VIDEO_YOUTUBE = PublisherWizardData.EXPORT_TYPE_VIDEO + 30;

   public static void SetDefaultProfileValues(ProfiledSettings profiledSettings, long iProfileId) {
     // since 4.0.p6: always create default profile and do not load/save profile file
     if (isDefaultProfileId(iProfileId) && profiledSettings != null ) {
        // profileSettings should already be reset before calling this method
        // since 4.0.p6: default profiles are generic profiles without type         
        //profiledSettings.SetType(getProfileDocumentType(iProfileId));
        profiledSettings.SetType(PublisherWizardData.DOCUMENT_TYPE_UNKNOWN);
        profiledSettings.SetId(iProfileId);
        profiledSettings.SetIntValue("iExportType", (int)iProfileId%10);

        SetDefaultSettings(profiledSettings, (int)iProfileId);
     }
     profiledSettings.ReadUploadSettingsFromRegistry();     
  }
   
   public static boolean isDefaultProfileId(long iProfileId) {
      if (iProfileId >= 0 && iProfileId < 1000) {
         // ID is in reserved range
         int iProfileType = (int) iProfileId / 100;
         int iProfileExportFormat = (int) iProfileId % 100;

         switch (iProfileType) {
         // since 4.0.p6 default profiles are generic profiles without type
         case PublisherWizardData.DOCUMENT_TYPE_UNKNOWN: 
//         case PublisherWizardData.DOCUMENT_TYPE_AUDIO_PAGES:
//         case PublisherWizardData.DOCUMENT_TYPE_AUDIO_VIDEO_PAGES:
//         case PublisherWizardData.DOCUMENT_TYPE_AUDIO_CLIPS_PAGES:
//         case PublisherWizardData.DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES:
//         case PublisherWizardData.DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES:
//         case PublisherWizardData.DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES:
//         case PublisherWizardData.DOCUMENT_TYPE_DENVER:
            // ID matches a valid type
            switch (iProfileExportFormat) {
            case PublisherWizardData.EXPORT_TYPE_LECTURNITY:
            case PublisherWizardData.EXPORT_TYPE_REAL:
            case PublisherWizardData.EXPORT_TYPE_WMT:
            case PublisherWizardData.EXPORT_TYPE_FLASH:
            case PublisherWizardData.EXPORT_TYPE_VIDEO:
            case EXPORT_TYPE_FLASH_POWERTRAINER:
            case EXPORT_TYPE_FLASH_SLIDESTAR:
            case EXPORT_TYPE_VIDEO_YOUTUBE:
               // ID specifies a valid export format
               return true;
            default:
            }
         default:
         }
      }
      return false;
   }
   
   public static int GetProfileId(int iExportFormat, boolean bIsPowerTrainerMode) {
       int iProfileId = 0;
       if ( iExportFormat >= 0 ) {
           // since 4.0.p6 all default profiles are generic profiles without type
           // determine special export type
           // NOTE: Studio handles YOUTUBE and SLIDESTAR as Export Format
           // but Publisher handles YOUTUBE/SLIDESTAR as VIDEO/FLASH with special parameters
           switch (iExportFormat) {
           case PublisherWizardData.EXPORT_TYPE_LECTURNITY:
           case PublisherWizardData.EXPORT_TYPE_REAL:
           case PublisherWizardData.EXPORT_TYPE_WMT:
           case PublisherWizardData.EXPORT_TYPE_FLASH:
           case PublisherWizardData.EXPORT_TYPE_VIDEO:
               iProfileId = iExportFormat;
               break;
           case 5:
               iProfileId = DefaultProfiles.EXPORT_TYPE_VIDEO_YOUTUBE;
               break;
           case 6:
               iProfileId = DefaultProfiles.EXPORT_TYPE_FLASH_SLIDESTAR;
               break;
           }
           if ( bIsPowerTrainerMode ) // not called with Export Format but with "-dpt"
               iProfileId = DefaultProfiles.EXPORT_TYPE_FLASH_POWERTRAINER;
       }
       return iProfileId;
   }
   
      
   public static void SetControlVisibilityForPowertrainer(ProfiledSettings ps, int iShow) {
       // iShow: "0" means "show" and "2" means "hide"
       ps.SetIntValue("iControlBarVisibility", iShow);
       ps.SetIntValue("iDocumentStructureVisibility", iShow);
       ps.SetIntValue("iNavigationButtonsVisibility", iShow);
       ps.SetIntValue("iTimeDisplayVisibility", iShow);
       ps.SetIntValue("iTimeLineVisibility", iShow);
   }

   private static void SetDefaultSettings(ProfiledSettings profiledSettings, int iProfileId) {
      // determine profile name
      // TODO: language support (currently names are English only)
      String strProfileName = "";

      // determine export text for profile name
      switch (iProfileId) {
        case PublisherWizardData.EXPORT_TYPE_LECTURNITY:
            strProfileName = "Default LECTURNITY";
            break;
        case PublisherWizardData.EXPORT_TYPE_REAL:
            strProfileName = "Default RealMedia";
            break;
        case PublisherWizardData.EXPORT_TYPE_WMT:
            strProfileName = "Default Windows Media";
            break;
        case PublisherWizardData.EXPORT_TYPE_FLASH:
            strProfileName = "Default Flash";
            break;
        case EXPORT_TYPE_FLASH_POWERTRAINER:
            strProfileName = "Default Flash [PowerTrainer]";
            break;
        case EXPORT_TYPE_FLASH_SLIDESTAR:
            strProfileName = "Default Flash [Slidestar]";
            break;
        case PublisherWizardData.EXPORT_TYPE_VIDEO:
            strProfileName = "Default iPod";
            break;
        case EXPORT_TYPE_VIDEO_YOUTUBE:
            strProfileName = "Default YouTube";
            break;
      }
      // set profile name
      profiledSettings.SetName(strProfileName);      
      
      switch (iProfileId) {
      /* LPD */
      case PublisherWizardData.EXPORT_TYPE_LECTURNITY:
//      case 100: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_LECTURNITY
//      case 200: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_LECTURNITY
//      case 300: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_LECTURNITY
//      case 400: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_LECTURNITY
//      case 500: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_LECTURNITY
//      case 600: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_LECTURNITY
//      case 700: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_LECTURNITY
          profiledSettings.SetBoolValue("bHideLecturnityIdentity", false);
          profiledSettings.SetIntValue("iMaxCustomClipHeight", 450);
          profiledSettings.SetIntValue("iMaxCustomClipWidth", 600);
          profiledSettings.SetIntValue("iSlideHeight", 450);
          profiledSettings.SetIntValue("iSlideWidth", 600);
          profiledSettings.SetIntValue("iVideoHeight", 240);
          profiledSettings.SetIntValue("iVideoWidth", 320);          
          break;
        
      /* REAL MEDIA */
      case PublisherWizardData.EXPORT_TYPE_REAL:
//      case 101: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_REAL
//      case 201: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_REAL
//      case 301: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_REAL
//      case 401: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_REAL
//      case 501: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_REAL
//      case 601: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_REAL
//      case 701: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_REAL
          profiledSettings.SetBoolValue("bHideLecturnityIdentity", false);
          profiledSettings.SetIntValue("iMaxCustomClipHeight", 450);
          profiledSettings.SetIntValue("iMaxCustomClipWidth", 600);
          profiledSettings.SetIntValue("iSlideHeight", 450);
          profiledSettings.SetIntValue("iSlideWidth", 600);
          profiledSettings.SetIntValue("iVideoHeight", 240);
          profiledSettings.SetIntValue("iVideoWidth", 320);          
          profiledSettings.SetIntValue("iSlideSizeType", PublisherWizardData.SLIDESIZE_ORIGINAL);
          LPLibs.templateInit();         
          LPLibs.templateReadSettings(); 
          profiledSettings.SetStringValue("strRmWmTemplateConfigFilePath", LPLibs.templateGetActivePath());
          profiledSettings.SetStringValue("strRmWmTemplateConfigParameterList", LPLibs.templateGetSettings());
          profiledSettings.SetStringValue("strScormSettings", ";1.3;Final;1;0;1;;;0;8;;;;;;;;;;;;;;;;;");
          break;

      /* WINDOWS MEDIA */
      case PublisherWizardData.EXPORT_TYPE_WMT:
//      case 102: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_WMT
//      case 202: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_WMT
//      case 302: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_WMT
//      case 402: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_WMT
//      case 502: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_WMT
//      case 602: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_WMT
//      case 702: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_WMT
          profiledSettings.SetBoolValue("bHideLecturnityIdentity", false);
          profiledSettings.SetIntValue("iMaxCustomClipHeight", 450);
          profiledSettings.SetIntValue("iMaxCustomClipWidth", 600);
          profiledSettings.SetIntValue("iSlideHeight", 450);
          profiledSettings.SetIntValue("iSlideWidth", 600);
          profiledSettings.SetIntValue("iVideoHeight", 240);
          profiledSettings.SetIntValue("iVideoWidth", 320);          
          profiledSettings.SetIntValue("iSlideSizeType", PublisherWizardData.SLIDESIZE_ORIGINAL);
          LPLibs.templateInit();         
          LPLibs.templateReadSettings(); 
          profiledSettings.SetStringValue("strRmWmTemplateConfigFilePath", LPLibs.templateGetActivePath());
          profiledSettings.SetStringValue("strRmWmTemplateConfigParameterList", LPLibs.templateGetSettings());
          profiledSettings.SetStringValue("strScormSettings", ";1.3;Final;1;0;1;;;0;8;;;;;;;;;;;;;;;;;");
          profiledSettings.SetStringValue("strScormSettings", ";1.3;Final;1;0;1;;;0;8;;;;;;;;;;;;;;;;;");
          break;
         
      /* FLASH */
      case PublisherWizardData.EXPORT_TYPE_FLASH:
//      case 103: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_FLASH
//      case 203: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_FLASH
//      case 303: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_FLASH
//      case 403: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_FLASH
//      case 503: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_FLASH
//      case 603: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_FLASH
//      case 703: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_FLASH
          profiledSettings.SetBoolValue("bHideLecturnityIdentity", false);
          profiledSettings.SetIntValue("iMaxCustomClipHeight", 450);
          profiledSettings.SetIntValue("iMaxCustomClipWidth", 600);
          profiledSettings.SetIntValue("iSlideHeight", 540);
          profiledSettings.SetIntValue("iSlideWidth", 720);
          profiledSettings.SetIntValue("iVideoHeight", 240);
          profiledSettings.SetIntValue("iVideoWidth", 320);          
          profiledSettings.SetIntValue("iSlideSizeType", PublisherWizardData.SLIDESIZE_ORIGINAL);
          profiledSettings.SetBoolValue("bFlashStartInOptimizedSize", false);
          profiledSettings.SetStringValue("strScormSettings", ";1.3;Final;1;0;1;;;0;8;;;;;;;;;;;;;;;;;");
          
          break;
          
      /* FLASH (PowerTrainer) */
      case EXPORT_TYPE_FLASH_POWERTRAINER:
//      case 113: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_FLASH_POWERTRAINER
//      case 213: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_FLASH_POWERTRAINER
//      case 313: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_FLASH_POWERTRAINER
//      case 413: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_FLASH_POWERTRAINER
//      case 513: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_FLASH_POWERTRAINER
//      case 613: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_FLASH_POWERTRAINER
//      case 713: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_FLASH_POWERTRAINER
          profiledSettings.SetBoolValue("bHideLecturnityIdentity", false);
          profiledSettings.SetIntValue("iMaxCustomClipHeight", 450);
          profiledSettings.SetIntValue("iMaxCustomClipWidth", 600);
          profiledSettings.SetIntValue("iSlideHeight", 540);
          profiledSettings.SetIntValue("iSlideWidth", 720);
          profiledSettings.SetIntValue("iVideoHeight", 240);
          profiledSettings.SetIntValue("iVideoWidth", 320);          
          profiledSettings.SetIntValue("iSlideSizeType", PublisherWizardData.SLIDESIZE_ORIGINAL);
          profiledSettings.SetBoolValue("bFlashStartInOptimizedSize", false);
          profiledSettings.SetBoolValue("bScormEnabled", true);
          profiledSettings.SetStringValue("strScormSettings", ";1.3;Final;1;0;1;;;0;8;;;;;;;;;;;;;;;;;");
          
          // do not show control elements
          profiledSettings.SetIntValue("iControlBarVisibility", PublisherWizardData.GUI_ELEMENT_HIDDEN);
          profiledSettings.SetIntValue("iDocumentStructureVisibility", PublisherWizardData.GUI_ELEMENT_HIDDEN);
          profiledSettings.SetIntValue("iNavigationButtonsVisibility", PublisherWizardData.GUI_ELEMENT_HIDDEN);
          profiledSettings.SetIntValue("iTimeDisplayVisibility", PublisherWizardData.GUI_ELEMENT_HIDDEN);
          profiledSettings.SetIntValue("iTimeLineVisibility", PublisherWizardData.GUI_ELEMENT_HIDDEN);
		  // BUGFIX 5873: auto start in PowerTrainer mode
		  profiledSettings.SetBoolValue("bDoAutostart", true);
          break;
         
      /* FLASH (SLIDESTAR) */
      case EXPORT_TYPE_FLASH_SLIDESTAR:
//      case 123: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_FLASH_SLIDESTAR
//      case 223: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_FLASH_SLIDESTAR
//      case 323: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_FLASH_SLIDESTAR
//      case 423: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_FLASH_SLIDESTAR
//      case 523: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_FLASH_SLIDESTAR
//      case 623: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_FLASH_SLIDESTAR
//      case 723: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_FLASH_SLIDESTAR
          profiledSettings.SetStringValue("strUploadFirstTargetDir","/");
          profiledSettings.SetBoolValue("bFlashCreateWebsite", false);
          profiledSettings.SetIntValue("iFlashTemplateType", PublisherWizardData.TEMPLATE_FLASH_SLIDESTAR);
          profiledSettings.SetIntValue("iReplayType", PublisherWizardData.REPLAY_SERVER);
          profiledSettings.SetIntValue("iServerType", PublisherWizardData.SRV_SLIDESTAR);
          profiledSettings.SetIntValue("iTransferType", PublisherWizardData.TRANSFER_UPLOAD);          
          profiledSettings.SetBoolValue("bHideLecturnityIdentity", false);
          profiledSettings.SetIntValue("iMaxCustomClipHeight", 450);
          profiledSettings.SetIntValue("iMaxCustomClipWidth", 600);
          profiledSettings.SetIntValue("iSlideHeight", 540);
          profiledSettings.SetIntValue("iSlideWidth", 720);
          profiledSettings.SetIntValue("iVideoHeight", 240);
          profiledSettings.SetIntValue("iVideoWidth", 320);          
          profiledSettings.SetIntValue("iSlideSizeType", PublisherWizardData.SLIDESIZE_ORIGINAL);
          profiledSettings.SetBoolValue("bFlashStartInOptimizedSize", false);
          profiledSettings.SetStringValue("strScormSettings", ";1.3;Final;1;0;1;;;0;8;;;;;;;;;;;;;;;;;");
          break;
         
      /* MP4 (iPod) */
      case PublisherWizardData.EXPORT_TYPE_VIDEO:
//      case 104: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_VIDEO
//      case 204: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_VIDEO
//      case 304: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_VIDEO
//      case 404: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_VIDEO
//      case 504: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_VIDEO
//      case 604: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_VIDEO
//      case 704: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_VIDEO
          profiledSettings.SetBoolValue("bHideLecturnityIdentity", false);
          profiledSettings.SetIntValue("iMaxCustomClipHeight", 450);
          profiledSettings.SetIntValue("iMaxCustomClipWidth", 600);
          profiledSettings.SetIntValue("iSlideHeight", 450);
          profiledSettings.SetIntValue("iSlideWidth", 600);
          profiledSettings.SetIntValue("iVideoHeight", 240);
          profiledSettings.SetIntValue("iVideoWidth", 320);          
          break;
          
      /* MP4 (YouTube) */
      case EXPORT_TYPE_VIDEO_YOUTUBE:
//      case 134: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_VIDEO_YOUTUBE
//      case 234: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_VIDEO_YOUTUBE
//      case 334: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_VIDEO_YOUTUBE
//      case 434: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_VIDEO_YOUTUBE
//      case 534: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_VIDEO_YOUTUBE
//      case 634: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_VIDEO_YOUTUBE
//      case 734: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_VIDEO_YOUTUBE
          profiledSettings.SetBoolValue("bHideLecturnityIdentity", false);
          profiledSettings.SetIntValue("iMaxCustomClipHeight", 450);
          profiledSettings.SetIntValue("iMaxCustomClipWidth", 600);
          profiledSettings.SetIntValue("iSlideHeight", 450);
          profiledSettings.SetIntValue("iSlideWidth", 600);
          profiledSettings.SetIntValue("iVideoHeight", 240);
          profiledSettings.SetIntValue("iVideoWidth", 320);          
          profiledSettings.SetIntValue("iReplayType", PublisherWizardData.REPLAY_SERVER);
          profiledSettings.SetIntValue("iServerType", PublisherWizardData.SRV_YOUTUBE);
          profiledSettings.SetIntValue("iTransferType", PublisherWizardData.TRANSFER_UPLOAD);
          profiledSettings.SetBoolValue("bMp4YouTubeOptimized", true);          
          break;
      }
   }

   /*
    * code used for debugging only
    * 
   
    public static void main(String[] args) {
        ProfiledSettings defaultSettings = new ProfiledSettings(0);
        defaultSettings.SetId(0);

        System.out.println(("######################### default #########################"));
        System.out.println("strTitle:\t\t"+defaultSettings.GetName());
        System.out.println("iFormatVersion:\t\t"+defaultSettings.GetFormatVersion());
        System.out.println("iDocumentType:\t\t"+defaultSettings.GetType());

        Iterator iterKeys = defaultSettings.m_mapValues.keySet().iterator();                
        while (iterKeys.hasNext())
        {
           String key = (String)iterKeys.next();
           String value = (String)defaultSettings.m_mapValues.get(key);
           System.out.println(key+":\t\t"+value);                   
//           System.out.println(key);                   
        }
//        iterKeys = defaultSettings.m_mapValues.keySet().iterator();                
//        while (iterKeys.hasNext())
//        {
//           String key = (String)iterKeys.next();
//           String value = (String)defaultSettings.m_mapValues.get(key);
//           System.out.println(value);                   
//        }
        System.out.println("#########################################################");
        System.out.println();
        HashMap<Integer, ProfiledSettings> settingsMap = new HashMap<Integer, ProfiledSettings>();
        settingsMap.put(0, defaultSettings);
        
        for (int iProfileId = 0; iProfileId < 1000; iProfileId++)
            switch (iProfileId) {
            case 100: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_LECTURNITY
            case 101: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_REAL
            case 102: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_WMT
            case 113: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_FLASH_POWERTRAINER
            case 103: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_FLASH
            case 104: // DOCUMENT_TYPE_AUDIO_PAGES + EXPORT_TYPE_VIDEO
            case 200: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_LECTURNITY
            case 201: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_REAL
            case 202: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_WMT
            case 213: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_FLASH_POWERTRAINER
            case 203: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_FLASH
            case 204: // DOCUMENT_TYPE_AUDIO_VIDEO_PAGES + EXPORT_TYPE_VIDEO
            case 300: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_LECTURNITY
            case 301: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_REAL
            case 302: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_WMT
            case 313: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_FLASH_POWERTRAINER
            case 303: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_FLASH
            case 304: // DOCUMENT_TYPE_AUDIO_CLIPS_PAGES + EXPORT_TYPE_VIDEO
            case 400: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_LECTURNITY
            case 401: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_REAL
            case 402: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_WMT
            case 413: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_FLASH_POWERTRAINER
            case 403: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_FLASH
            case 404: // DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES + EXPORT_TYPE_VIDEO
            case 500: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_LECTURNITY
            case 501: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_REAL
            case 502: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_WMT
            case 513: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_FLASH_POWERTRAINER
            case 503: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_FLASH
            case 504: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES + EXPORT_TYPE_VIDEO
            case 600: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_LECTURNITY
            case 601: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_REAL
            case 602: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_WMT
            case 613: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_FLASH_POWERTRAINER
            case 603: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_FLASH
            case 604: // DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES + EXPORT_TYPE_VIDEO
            case 700: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_LECTURNITY
            case 701: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_REAL
            case 702: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_WMT
            case 713: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_FLASH_POWERTRAINER
            case 703: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_FLASH
            case 704: // DOCUMENT_TYPE_DENVER + EXPORT_TYPE_VIDEO
                System.out.println(("########################## "+iProfileId+" ##########################"));
                ProfiledSettings profiledSettings = CreateDefaultProfile(iProfileId);
                settingsMap.put(iProfileId, profiledSettings);
//                
//                System.out.println("strTitle:\t\t"+profiledSettings.GetName());
//                System.out.println("iFormatVersion:\t\t"+profiledSettings.GetFormatVersion());
//                System.out.println("iDocumentType:\t\t"+profiledSettings.GetType());
//
//                iterKeys = profiledSettings.m_mapValues.keySet().iterator();                
//                while (iterKeys.hasNext())
//                {
//                   String key = (String)iterKeys.next();
//                   String value = (String)profiledSettings.m_mapValues.get(key);
//                   String defaultValue = (String)defaultSettings.m_mapValues.get(key);
//                   if(value.equals(defaultValue))
//                       System.out.println();
//                   else
//                       System.out.println(key+":\t\t"+value);                   
////                       System.out.println(value);                   
//                }
//                System.out.println("#########################################################");
//                System.out.println();

                break;
            default:
                break;
            }

        
        System.out.print("KEY\tDEFAULT");
        for(int i=0;i<100;i++)
            for(int j=1;j<10;j++)
            {
                int iProfileId = j*100 + i; 
//        for (int iProfileId = 1; iProfileId < 1000; iProfileId++) {
            if(settingsMap.containsKey(iProfileId))                    
                System.out.print("\t"+iProfileId);
            }
        System.out.println();
        
        String key = "strTitle";
        String valueDefault = defaultSettings.GetName();
        System.out.print(key+"\t"+valueDefault);
        for(int i=0;i<100;i++)
            for(int j=1;j<10;j++)
            {
                int iProfileId = j*100 + i; 
//        for (int iProfileId = 1; iProfileId < 1000; iProfileId++) {
            if(settingsMap.containsKey(iProfileId)) {                   
                String value = (String)settingsMap.get(iProfileId).GetName();
                System.out.print("\t");
                if(!value.equals(valueDefault))
                    System.out.print(value);                   
            }
        }
        System.out.println();

        key = "iDocumentType";
        int iValueDefault = defaultSettings.GetType();
        System.out.print(key+"\t"+iValueDefault);
        for(int i=0;i<100;i++)
            for(int j=1;j<10;j++)
            {
                int iProfileId = j*100 + i; 
//        for (int iProfileId = 1; iProfileId < 1000; iProfileId++) {
            if(settingsMap.containsKey(iProfileId)) {                   
                int iValue = settingsMap.get(iProfileId).GetType();
                System.out.print("\t");
                if(iValue != iValueDefault)
                    System.out.print(iValue);                   
            }
        }
        System.out.println();
        
        key = "iFormatVersion";
        iValueDefault = defaultSettings.GetFormatVersion();
        System.out.print(key+"\t"+iValueDefault);
        for(int i=0;i<100;i++)
            for(int j=1;j<10;j++)
            {
                int iProfileId = j*100 + i; 
//        for (int iProfileId = 1; iProfileId < 1000; iProfileId++) {
            if(settingsMap.containsKey(iProfileId)) {                   
                int iValue = settingsMap.get(iProfileId).GetFormatVersion();
                System.out.print("\t");
                if(iValue != iValueDefault)
                    System.out.print(iValue);                   
            }
        }
        System.out.println();

        iterKeys = defaultSettings.m_mapValues.keySet().iterator();                
        while (iterKeys.hasNext())
        {
           key = (String)iterKeys.next();
           valueDefault = (String)defaultSettings.m_mapValues.get(key);
           System.out.print(key+"\t"+valueDefault); 
           for(int i=0;i<100;i++)
               for(int j=1;j<10;j++)
               {
                   int iProfileId = j*100 + i; 
//           for (int iProfileId = 1; iProfileId < 1000; iProfileId++) {
               if(settingsMap.containsKey(iProfileId)) {                   
                   String value = (String)settingsMap.get(iProfileId).m_mapValues.get(key);
                   System.out.print("\t");
                   if(!value.equals(valueDefault))
                       System.out.print(value);                   
               }
           }
           System.out.println();
        }
        
        System.exit(0);
    }   		  
	*/
}
