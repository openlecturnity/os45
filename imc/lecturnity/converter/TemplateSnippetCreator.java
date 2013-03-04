package imc.lecturnity.converter;

import java.io.*;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Rectangle;
import java.util.ArrayList;
import java.util.StringTokenizer;
import javax.swing.JOptionPane;

import imc.epresenter.filesdk.*;
import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.player.whiteboard.*;
import imc.lecturnity.converter.MyButtonArea;
import imc.lecturnity.converter.wizard.PublisherWizardData;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;

public class TemplateSnippetCreator extends SnippetCreator
{
   private static String SLIDES_OVERVIEW = "[!]";
   private static String NO_STD_BROWSER = "[!]";
   private static String LOGO_FILENAME = "[!]";
   private static String NETSCAPE_RESIZE_WARNING = "[!]";
   private static String JUMP_TO_BEGIN = "[!]";
   private static String JUMP_TO_END = "[!]";
   private static String SLIDE_FORWARD = "[!]";
   private static String SLIDE_BACKWARDS = "[!]";
   private static String START_REPLAY_PAUSE = "[!]";
   private static String SOUND_ON_OFF = "[!]";
   private static String PLUGIN_NOT_DETECTED = "[!]";
   private static String PLUGIN_RM_NOT_DETECTED = "[!]";
   private static String PLUGIN_WM_NOT_DETECTED = "[!]";
   private static String PLUGINS_CANT_BE_DETECTED = "[!]";
   private static String AUTHOR = "[!]";
   private static String RECORD_DATE = "[!]";
   private static String LANGUAGE_CODE = "[!]";
   private static String BUFFERING = "[!]";
   private static String STANDBY_TEXT = "[!]";
   private static String TAB_OVERVIEW = "[!]";
   private static String TAB_SLIDES = "[!]";
   private static String TAB_INFO = "[!]";
   private static String TAB_SEARCH = "[!]";
   private static String NO_RESULTS = "[!]";
   private static String MORE_RESULTS = "[!]";
   private static String CREATOR = "[!]";
   private static String KEYWORDS = "[!]";
   private static String SLIDE_LABEL = "[!]";
   private static String SLIDE_LABEL_OF = "[!]";
   private static String LECTURNITY_LINK = "[!]";

   private static String RM_PNR_DOC_MISSING = "[!]";
   private static String RM_PNR_NET_CONNECT = "[!]";
   private static String RM_PNR_PROXY_NET_CONNECT = "[!]";
   private static String RM_PNR_INVALID_HOST = "[!]";
   private static String RM_PNR_DNR = "[!]";
   private static String RM_PNR_RTSP_NOT_SUPPORTED = "[!]";
   private static String RM_PNR_SERVER_ALERT = "[!]";
   private static String RM_PLUGIN_CHECK_ERROR = "[!]";
   private static String RM_PNR_SERVER_TIMEOUT = "[!]";
   private static String RM_PNR_FAIL = "[!]";
   private static String RM_UNKNOWN_ERROR = "[!]";
   private static String RM_HTTPS_WARNING = "[!]";
   private static String WM_NETSCAPE_LESS_71 = "[!]";
   private static String WM_NETSCAPE_8 = "[!]";
   private static String WM_FIREFOX_LESS_10 = "[!]";
   private static String WM_MOZILLA_LESS_16 = "[!]";
   private static String WM_FIREFOX_PLUGIN_REQUIRED = "[!]";
   private static String WM_BROWSER_NOT_SUPPORTED = "[!]";
   private static String WM_ACTIVEX_CONTROL_REQUIRED = "[!]";
   private static String WM_ACTIVEX_CONTROL_WARNING = "[!]";
   private static String WM_ACTIVEX_CONTROL_DOWNLOAD = "[!]";
   private static String WM_ACTIVEX_CONTROL_UNKNOWN = "[!]";
   private static String WM_PROGRESSIVE_DOWNLOAD_STATUS = "[!]";
   private static String WM_PROGRESSIVE_DOWNLOAD_ALERT = "[!]";
   private static String WM_PROGRESSIVE_DOWNLOAD_WMP64 = "[!]";
   private static String URL_HAS_ILLEGAL_CHARACTERS = "[!]";
   private static String URL_NO_REPLAY_POSSIBLE = "[!]";
   private static String NO_JAVASCRIPT_HEADER = "[!]";
   private static String NO_JAVASCRIPT_REASON = "[!]";
   private static String NO_JAVASCRIPT_CAUSE1 = "[!]";
   private static String NO_JAVASCRIPT_SPECIAL = "[!]";
   private static String NO_JAVASCRIPT_CAUSE2 = "[!]";
   private static String NO_JAVASCRIPT_IMAGE = "[!]";
   private static String POPUP_BLOCKER_ALERT = "[!]";
   private static String POPUP_BLOCKER_TEXT1 = "[!]";
   private static String POPUP_BLOCKER_TEXT2 = "[!]";
   private static String POPUP_BLOCKER_TEXT3 = "[!]";
   private static String POPUP_BLOCKER_TEXT4 = "[!]";
   private static String POPUP_BLOCKER_TEXT5 = "[!]";
   private static String TESTING_RESULT = "[!]";
   private static String TESTING_QUESTION = "[!]";
   private static String TESTING_ANSWER = "[!]";
   private static String TESTING_POINTS = "[!]";
   private static String TESTING_MAXIMUM_POINTS = "[!]";
   private static String TESTING_REQUIRED_POINTS = "[!]";
   private static String TESTING_REQUIRED_PERCENTAGE = "[!]";
   private static String TESTING_ACHIEVED_POINTS = "[!]";
   private static String TESTING_ACHIEVED_PERCENTAGE = "[!]";
   private static String TESTING_SUM_OF_ALL_POINTS = "[!]";
   private static String TESTING_ACCURACY = "[!]";
   private static String TESTING_ATTEMPTS = "[!]";
   private static String TESTING_CORRECT = "[!]";
   private static String TESTING_WRONG = "[!]";
   private static String TESTING_PASSED = "[!]";
   private static String TESTING_FAILED = "[!]";
   private static String INITIALIZE = "[!]";
   
   // The DOS format of a line feed (CR + LF)
   public static final String END_LN = "\r\n";

   static
   {
      try
      {
         Localizer l 
            = new Localizer("/imc/lecturnity/converter/TemplateSnippetCreator_",
                            "en");

         SLIDES_OVERVIEW = l.getLocalized("SLIDES_OVERVIEW");
         NO_STD_BROWSER = l.getLocalized("NO_STD_BROWSER");
         LOGO_FILENAME = l.getLocalized("LOGO_FILENAME");
         NETSCAPE_RESIZE_WARNING = l.getLocalized("NETSCAPE_RESIZE_WARNING");
         JUMP_TO_BEGIN = l.getLocalized("JUMP_TO_BEGIN");
         JUMP_TO_END = l.getLocalized("JUMP_TO_END");
         SLIDE_FORWARD = l.getLocalized("SLIDE_FORWARD");
         SLIDE_BACKWARDS = l.getLocalized("SLIDE_BACKWARDS");
         START_REPLAY_PAUSE = l.getLocalized("START_REPLAY_PAUSE");
         SOUND_ON_OFF = l.getLocalized("SOUND_ON_OFF");
         PLUGIN_NOT_DETECTED = l.getLocalized("PLUGIN_NOT_DETECTED");
         PLUGIN_RM_NOT_DETECTED = l.getLocalized("PLUGIN_RM_NOT_DETECTED");
         PLUGIN_WM_NOT_DETECTED = l.getLocalized("PLUGIN_WM_NOT_DETECTED");
         PLUGINS_CANT_BE_DETECTED = l.getLocalized("PLUGINS_CANT_BE_DETECTED");
         AUTHOR  = l.getLocalized("AUTHOR");
         RECORD_DATE = l.getLocalized("RECORD_DATE");
         LANGUAGE_CODE = l.getLocalized("LANGUAGE_CODE");
         BUFFERING  = l.getLocalized("BUFFERING");
         STANDBY_TEXT  = l.getLocalized("STANDBY_TEXT");
         TAB_OVERVIEW = l.getLocalized("TAB_OVERVIEW");
         TAB_SLIDES = l.getLocalized("TAB_SLIDES");
         TAB_INFO = l.getLocalized("TAB_INFO");
         TAB_SEARCH = l.getLocalized("TAB_SEARCH");
         NO_RESULTS = l.getLocalized("NO_RESULTS");
         MORE_RESULTS = l.getLocalized("MORE_RESULTS");
         CREATOR = l.getLocalized("CREATOR");
         KEYWORDS = l.getLocalized("KEYWORDS");
         SLIDE_LABEL = l.getLocalized("SLIDE_LABEL");
         SLIDE_LABEL_OF = l.getLocalized("SLIDE_LABEL_OF");
         LECTURNITY_LINK = l.getLocalized("LECTURNITY_LINK");
         
         RM_PNR_DOC_MISSING = l.getLocalized("RM_PNR_DOC_MISSING");
         RM_PNR_NET_CONNECT = l.getLocalized("RM_PNR_NET_CONNECT");
         RM_PNR_PROXY_NET_CONNECT = l.getLocalized("RM_PNR_PROXY_NET_CONNECT");
         RM_PNR_INVALID_HOST = l.getLocalized("RM_PNR_INVALID_HOST");
         RM_PNR_DNR = l.getLocalized("RM_PNR_DNR");
		 RM_PNR_RTSP_NOT_SUPPORTED = l.getLocalized("RM_PNR_RTSP_NOT_SUPPORTED");
         RM_PNR_SERVER_ALERT = l.getLocalized("RM_PNR_SERVER_ALERT");
         RM_PLUGIN_CHECK_ERROR = l.getLocalized("RM_PLUGIN_CHECK_ERROR");
         RM_PNR_SERVER_TIMEOUT = l.getLocalized("RM_PNR_SERVER_TIMEOUT");
         RM_PNR_FAIL = l.getLocalized("RM_PNR_FAIL");
         RM_UNKNOWN_ERROR = l.getLocalized("RM_UNKNOWN_ERROR");
         RM_HTTPS_WARNING = l.getLocalized("RM_HTTPS_WARNING");
         WM_NETSCAPE_LESS_71 = l.getLocalized("WM_NETSCAPE_LESS_71");
         WM_NETSCAPE_8 = l.getLocalized("WM_NETSCAPE_8");
         WM_FIREFOX_LESS_10 = l.getLocalized("WM_FIREFOX_LESS_10");
         WM_MOZILLA_LESS_16 = l.getLocalized("WM_MOZILLA_LESS_16");
         WM_FIREFOX_PLUGIN_REQUIRED = l.getLocalized("WM_FIREFOX_PLUGIN_REQUIRED");
         WM_BROWSER_NOT_SUPPORTED = l.getLocalized("WM_BROWSER_NOT_SUPPORTED");
         WM_ACTIVEX_CONTROL_REQUIRED = l.getLocalized("WM_ACTIVEX_CONTROL_REQUIRED");
         WM_ACTIVEX_CONTROL_WARNING = l.getLocalized("WM_ACTIVEX_CONTROL_WARNING");
         WM_ACTIVEX_CONTROL_DOWNLOAD = l.getLocalized("WM_ACTIVEX_CONTROL_DOWNLOAD");
         WM_ACTIVEX_CONTROL_UNKNOWN = l.getLocalized("WM_ACTIVEX_CONTROL_UNKNOWN");
         WM_PROGRESSIVE_DOWNLOAD_STATUS = l.getLocalized("WM_PROGRESSIVE_DOWNLOAD_STATUS");
         WM_PROGRESSIVE_DOWNLOAD_ALERT = l.getLocalized("WM_PROGRESSIVE_DOWNLOAD_ALERT");
         WM_PROGRESSIVE_DOWNLOAD_WMP64 = l.getLocalized("WM_PROGRESSIVE_DOWNLOAD_WMP64");
         URL_HAS_ILLEGAL_CHARACTERS = l.getLocalized("URL_HAS_ILLEGAL_CHARACTERS");
         URL_NO_REPLAY_POSSIBLE = l.getLocalized("URL_NO_REPLAY_POSSIBLE");
         NO_JAVASCRIPT_HEADER = l.getLocalized("NO_JAVASCRIPT_HEADER");
         NO_JAVASCRIPT_REASON = l.getLocalized("NO_JAVASCRIPT_REASON");
         NO_JAVASCRIPT_CAUSE1 = l.getLocalized("NO_JAVASCRIPT_CAUSE1");
         NO_JAVASCRIPT_SPECIAL = l.getLocalized("NO_JAVASCRIPT_SPECIAL");
         NO_JAVASCRIPT_CAUSE2 = l.getLocalized("NO_JAVASCRIPT_CAUSE2");
         NO_JAVASCRIPT_IMAGE = l.getLocalized("NO_JAVASCRIPT_IMAGE");
         POPUP_BLOCKER_ALERT = l.getLocalized("POPUP_BLOCKER_ALERT");
         POPUP_BLOCKER_TEXT1 = l.getLocalized("POPUP_BLOCKER_TEXT1");
         POPUP_BLOCKER_TEXT2 = l.getLocalized("POPUP_BLOCKER_TEXT2");
         POPUP_BLOCKER_TEXT3 = l.getLocalized("POPUP_BLOCKER_TEXT3");
         POPUP_BLOCKER_TEXT4 = l.getLocalized("POPUP_BLOCKER_TEXT4");
         POPUP_BLOCKER_TEXT5 = l.getLocalized("POPUP_BLOCKER_TEXT5");
         TESTING_RESULT = l.getLocalized("TESTING_RESULT");
         TESTING_QUESTION = l.getLocalized("TESTING_QUESTION");
         TESTING_ANSWER = l.getLocalized("TESTING_ANSWER");
         TESTING_POINTS = l.getLocalized("TESTING_POINTS");
         TESTING_MAXIMUM_POINTS = l.getLocalized("TESTING_MAXIMUM_POINTS");
         TESTING_REQUIRED_POINTS = l.getLocalized("TESTING_REQUIRED_POINTS");
         TESTING_REQUIRED_PERCENTAGE = l.getLocalized("TESTING_REQUIRED_PERCENTAGE");
         TESTING_ACHIEVED_POINTS = l.getLocalized("TESTING_ACHIEVED_POINTS");
         TESTING_ACHIEVED_PERCENTAGE = l.getLocalized("TESTING_ACHIEVED_PERCENTAGE");
         TESTING_SUM_OF_ALL_POINTS = l.getLocalized("TESTING_SUM_OF_ALL_POINTS");
         TESTING_ACCURACY = l.getLocalized("TESTING_ACCURACY");
         TESTING_ATTEMPTS = l.getLocalized("TESTING_ATTEMPTS");
         TESTING_CORRECT = l.getLocalized("TESTING_CORRECT");
         TESTING_WRONG = l.getLocalized("TESTING_WRONG");
         TESTING_PASSED = l.getLocalized("TESTING_PASSED");
         TESTING_FAILED = l.getLocalized("TESTING_FAILED");
         INITIALIZE = l.getLocalized("INITIALIZE");
      }
      catch (IOException e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(null, 
                                       "Could not open the Locale database!",
                                       "Severe Error", 
                                       JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }
   
   
   private PublisherWizardData m_pwData;
   private DocumentInteractionInfo m_DocInfo;
   private double m_dScaling;
   private long m_lDuration;
   private int m_nMaxLayerIndex;
   

   public void createSnippets(PublisherWizardData pwData,
                              DocumentInteractionInfo docInfo,
                              double dScaling, long lDuration,
                              int nMaxLayerIndex, Dimension dimPages,
                              Dimension dimVideo, Dimension dimStillimage,
                              int nPageCount, int nEventCount)
   {
      m_pwData = pwData;
      m_DocInfo = docInfo;
      m_dScaling = dScaling;
      m_lDuration = lDuration;
      m_nMaxLayerIndex = nMaxLayerIndex;
      

      DocumentData dd = m_pwData.GetDocumentData();
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      boolean bClipsSeparated = ps.GetBoolValue("bShowClipsOnSlides");
      boolean bHasVideo = dd.HasNormalVideo() || (dd.HasClips() && !bClipsSeparated) || dd.IsDenver();
      boolean bHasClips = dd.HasClips() && bClipsSeparated;
      boolean bHasClipsSeparated = dd.HasClips() && bClipsSeparated;
      boolean bShowStillImage = dd.HasStillImage() && (bClipsSeparated || !dd.HasClips());
      boolean bScormEnabled = ps.GetBoolValue("bScormEnabled");
      boolean bIsDenver = dd.IsDenver();
      boolean bIsAutostartMode = ps.GetBoolValue("bDoAutostart");
      boolean bPluginContextMenuEnabled = 
         ps.GetIntValue("iPluginContextMenuVisibility") == PublisherWizardData.GUI_ELEMENT_ACTIVE;
      boolean bIsRealExport = ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_REAL;
      boolean bIsWmExport = ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_WMT;
      
      VideoClipInfo[] aClipInfos = dd.GetVideoClipInfos();
      int iClipCount = aClipInfos != null ? aClipInfos.length : 0;
      if (bIsDenver)
         iClipCount = 0;
   
      String strTargetName = ps.GetStringValue("strTargetPathlessDocName");
      String strStillImageName = dd.GetStillImageName();
      String strLrdFilename = dd.GetPresentationFileName();

      Metadata metadata = dd.GetMetadata();
      int iNumOfThumbnails = 0;
      if (m_DocInfo.aThumbnailEntries != null)
         iNumOfThumbnails = m_DocInfo.aThumbnailEntries.length;
      // else Denver documents have no thumbnails
      
      
      String strTitle = strLrdFilename; // dummy
      String strAuthor = "";
      String strCreator = null;
      String strRecordDate = "";
      String strRecordLength = "";
	  String strScormRmClips = "";
      
      if (metadata != null)
      {
         strTitle = metadata.title;
         strAuthor = metadata.author;
         if (metadata.creator != null)
            strCreator = metadata.creator;
         strRecordDate = metadata.recordDate;
         strRecordLength = metadata.recordLength;
      }
     

      addSnippet(new Snippet("Title", checkHtmlString(strTitle)));
      addSnippet(new Snippet("Author", checkHtmlString(strAuthor)));
      if (strCreator != null && strCreator.length() > 0)
         addSnippet(new Snippet("Creator", checkHtmlString(strCreator)));
      else
         addSnippet(new Snippet("Creator", "-"));
      addSnippet(new Snippet("RecordDate", strRecordDate));
      
      StringTokenizer st = new StringTokenizer(strRecordLength, ":");
      String strMyRecLen = "PT";
      int nTokenCount = st.countTokens();
      while (st.hasMoreTokens()) 
      {
         switch(nTokenCount) 
         {
            case 3:
            {
               strMyRecLen +=  (st.nextToken() + "H");
               break;
            }
            case 2:
            {
               strMyRecLen +=  (st.nextToken() + "M");
               break;
            }
            case 1:
            {
               strMyRecLen +=  (st.nextToken() + "S");
               break;
            }
            default:
               break;
         }	 
         nTokenCount --;
     }
     
      String scormIncludes= "";
      if(bScormEnabled == true)
      {
	      String f1 = "<script src=\"js/APIWrapper.js\" type=\"text/javascript\"></script>";
	      String f2 = "<script src=\"js/scorm.js\" type=\"text/javascript\"></script>";
	      String f3= "<script src=\"js/scorm_utils.js\" type=\"text/javascript\"></script>";
	      scormIncludes = "\t" + f1 + "\n" + "\t" + f2 + "\n" + "\t" + f3;
      }
      addSnippet(new Snippet("ScormFiles", scormIncludes));
      addSnippet(new Snippet("RecordLength", strRecordLength));
      System.out.println("RecordLengthNew:" + strMyRecLen);
      addSnippet(new Snippet("RecordLengthNew", strMyRecLen));
      addSnippet(new Snippet("BaseDocumentName", strTargetName));
      addSnippet(new Snippet("SlidesWidth", "" + dimPages.width));
      addSnippet(new Snippet("SlidesHeight", "" + dimPages.height));
      addSnippet(new Snippet("MaxLayerIndex", "" + m_nMaxLayerIndex));
      addSnippet(new Snippet("NumOfSlides", "" + nPageCount));
      addSnippet(new Snippet("NumOfEvents", "" + nEventCount));
      addSnippet(new Snippet("NumOfThumbnails", "" + iNumOfThumbnails));
      addSnippet(new Snippet("ScormEnabled", "" + bScormEnabled));
      addSnippet(new Snippet("PluginContextMenuEnabled", "" + bPluginContextMenuEnabled));
      
      //Bugfix 5326
      if (ps.GetBoolValue("bHideLecturnityIdentity") == true) {
         addSnippet(new Snippet("LecturnityWebPlayer", ""));
         addSnippet(new Snippet("Version", ""));
      } else {
         addSnippet(new Snippet("LecturnityWebPlayer", "- LECTURNITY WebPlayer"));
         addSnippet(new Snippet("Version", NativeUtils.getVersionStringShortShort()));
      }

      addSnippet(new Snippet("HasClips", "" + bHasClips));
      int iNumOfClips = (ps.GetBoolValue("bShowClipsOnSlides") ? iClipCount : 0);
      addSnippet(new Snippet("NumOfClips", "" + iNumOfClips));

      addSnippet(new Snippet("HasVideo", "" + (bHasVideo)));
      addSnippet(new Snippet("VideoWidth", "" + (bHasVideo ? dimVideo.width : 0)));
      addSnippet(new Snippet("VideoHeight", "" + (bHasVideo ? dimVideo.height : 0)));

      addSnippet(new Snippet("HasStillImage", "" + (bShowStillImage)));
      addSnippet(new Snippet("StillImageWidth", "" + (bShowStillImage ? dimStillimage.width : 0)));
      addSnippet(new Snippet("StillImageHeight", "" + (bShowStillImage ? dimStillimage.height : 0)));
	  if (bScormEnabled && bIsRealExport && bHasClips)
	  {
		  strScormRmClips += "  if(g_bModifiedByLocationUpdate==false)" + END_LN;
		  strScormRmClips += "    {" + END_LN;
		  strScormRmClips += "      var masterPlayer = control_getMasterPlayer();" + END_LN;
		  strScormRmClips += "      var playerUrl = \"\" + masterPlayer.GetSource();" + END_LN;
		  strScormRmClips += "      var playerProtocol = playerUrl.toLowerCase().substr(0,5);" + END_LN;
		  strScormRmClips += "      if (playerProtocol.indexOf(\"http\")> -1)" + END_LN;
		  strScormRmClips += "      {" + END_LN;
		  strScormRmClips += "        var returnRes = \"rtsp\" + playerUrl.substr(4, playerUrl.length);" + END_LN;
		  strScormRmClips += "        masterPlayer.SetSource(returnRes);" + END_LN;
		  strScormRmClips += "        g_isRtspProtocol = true;" + END_LN;
		  strScormRmClips += "        var strLength = playerUrl.length-1;" + END_LN;
		  strScormRmClips += "        while(playerUrl[strLength]!='/')" + END_LN;
		  strScormRmClips += "				strLength--;" + END_LN;
		  strScormRmClips += "		  g_playerUrl = playerUrl.substr(0, strLength);" + END_LN;
		  strScormRmClips += "      }" + END_LN;
		  strScormRmClips += "    }" + END_LN;
	  }
	  addSnippet(new Snippet("ScormRMClips", strScormRmClips));
      if (bShowStillImage)
      {
         addSnippet(new Snippet("StillImage", "<img src=\"" + strStillImageName + "\" width=\"" + dimStillimage.width + 
                                "\" height=\"" + dimStillimage.height + "\" alt=\"Still Image\"><br>" + END_LN));
      }
      else
      {
         addSnippet(new Snippet("StillImage", ""));
      }
      
      addSnippet(new Snippet("IsStandalone", "" + bIsDenver));
      boolean bHasTesting = (m_DocInfo.aQuestionnaires != null) ? true : false;
      addSnippet(new Snippet("HasTesting", "" + (bHasTesting)));
      addSnippet(new Snippet("IsAutostartMode", "" + (bIsAutostartMode)));

      addSnippet(new Snippet("NoStandardBrowser", checkHtmlString(NO_STD_BROWSER)));
      addSnippet(new Snippet("LogoFilename", "" + LOGO_FILENAME));
      addSnippet(new Snippet("NetscapeResizeWarning", checkHtmlString(NETSCAPE_RESIZE_WARNING)));
      addSnippet(new Snippet("JumpToBegin", checkHtmlString(JUMP_TO_BEGIN)));
      addSnippet(new Snippet("JumpToEnd", checkHtmlString(JUMP_TO_END)));
      addSnippet(new Snippet("SlideForward", checkHtmlString(SLIDE_FORWARD)));
      addSnippet(new Snippet("SlideBackwards", checkHtmlString(SLIDE_BACKWARDS)));
      addSnippet(new Snippet("StartReplayPause", checkHtmlString(START_REPLAY_PAUSE)));
      addSnippet(new Snippet("SoundOnOff", checkHtmlString(SOUND_ON_OFF)));
      
      addSnippet(new Snippet("PluginNotDetected", checkHtmlString(PLUGIN_NOT_DETECTED)));
      addSnippet(new Snippet("PluginsCantBeDetected", checkHtmlString(PLUGINS_CANT_BE_DETECTED)));
      if (bIsRealExport)
      {
         addSnippet(new Snippet("DetectRmOrWm", "detectReal()" ));
         addSnippet(new Snippet("PluginRmWmNotDetected", checkHtmlString(PLUGIN_RM_NOT_DETECTED)));
         addSnippet(new Snippet("StatusbarHeight", "" + 24));
      }
      else if (bIsWmExport)
      {
         addSnippet(new Snippet("DetectRmOrWm", "detectWindowsMedia()" ));
         addSnippet(new Snippet("PluginRmWmNotDetected", checkHtmlString(PLUGIN_WM_NOT_DETECTED)));
         addSnippet(new Snippet("StatusbarHeight", "" + 19));
      }

      addSnippet(new Snippet("AuthorLC", checkHtmlString(AUTHOR)));
      addSnippet(new Snippet("LanguageCode", "" + LANGUAGE_CODE));
      addSnippet(new Snippet("Buffering", checkHtmlString(BUFFERING)));

      addSnippet(new Snippet("RecordDateLC", checkHtmlString(RECORD_DATE)));
      addSnippet(new Snippet("CreatorLC", checkHtmlString(CREATOR)));
      addSnippet(new Snippet("KeywordsLC", checkHtmlString(KEYWORDS)));
      addSnippet(new Snippet("TabOverview", checkHtmlString(TAB_OVERVIEW)));
      addSnippet(new Snippet("TabSlides", checkHtmlString(TAB_SLIDES)));
      addSnippet(new Snippet("TabInfo", checkHtmlString(TAB_INFO)));
      addSnippet(new Snippet("TabSearch", checkHtmlString(TAB_SEARCH)));
      addSnippet(new Snippet("NoResults", checkHtmlString(NO_RESULTS)));
      addSnippet(new Snippet("MoreResults", checkHtmlString(MORE_RESULTS)));
      addSnippet(new Snippet("SlideLabel", checkHtmlString(SLIDE_LABEL)));
      addSnippet(new Snippet("SlideLabelOf", checkHtmlString(SLIDE_LABEL_OF)));
      addSnippet(new Snippet("LecturnityLink", LECTURNITY_LINK));

      addSnippet(new Snippet("RmPnrDocMissing", checkHtmlString(RM_PNR_DOC_MISSING)));
      addSnippet(new Snippet("RmPnrNetConnect", checkHtmlString(RM_PNR_NET_CONNECT)));
      addSnippet(new Snippet("RmPnrProxyNetConnect", checkHtmlString(RM_PNR_PROXY_NET_CONNECT)));
      addSnippet(new Snippet("RmPnrInvalidHost", checkHtmlString(RM_PNR_INVALID_HOST)));
      addSnippet(new Snippet("RmPnrDnr", checkHtmlString(RM_PNR_DNR)));
	  addSnippet(new Snippet("RmPnrRtspNotSupported", RM_PNR_RTSP_NOT_SUPPORTED));
      addSnippet(new Snippet("RmPnrServerAlert", checkHtmlString(RM_PNR_SERVER_ALERT)));
      addSnippet(new Snippet("RmPluginCheckError", checkHtmlString(RM_PLUGIN_CHECK_ERROR)));
      addSnippet(new Snippet("RmPnrServerTimeout", checkHtmlString(RM_PNR_SERVER_TIMEOUT)));
      addSnippet(new Snippet("RmPnrFail", checkHtmlString(RM_PNR_FAIL)));
      addSnippet(new Snippet("RmUnknownError", checkHtmlString(RM_UNKNOWN_ERROR)));
      addSnippet(new Snippet("RmHttpsWarning", checkHtmlString(RM_HTTPS_WARNING)));
      addSnippet(new Snippet("WmNetscapeLess71", checkHtmlString(WM_NETSCAPE_LESS_71)));
      addSnippet(new Snippet("WmNetscape8", checkHtmlString(WM_NETSCAPE_8)));
      addSnippet(new Snippet("WmFirefoxLess10", checkHtmlString(WM_FIREFOX_LESS_10)));
      addSnippet(new Snippet("WmMozillaLess16", checkHtmlString(WM_MOZILLA_LESS_16)));
      addSnippet(new Snippet("WmFirefoxPluginRequired", WM_FIREFOX_PLUGIN_REQUIRED));
      addSnippet(new Snippet("WmBrowserNotSupported", checkHtmlString(WM_BROWSER_NOT_SUPPORTED)));
      addSnippet(new Snippet("WmActivexControlRequired", checkHtmlString(WM_ACTIVEX_CONTROL_REQUIRED)));
      addSnippet(new Snippet("WmActivexControlWarning", checkHtmlString(WM_ACTIVEX_CONTROL_WARNING)));
      addSnippet(new Snippet("WmActivexControlDownload", checkHtmlString(WM_ACTIVEX_CONTROL_DOWNLOAD)));
      addSnippet(new Snippet("WmActivexControlUnknown", checkHtmlString(WM_ACTIVEX_CONTROL_UNKNOWN)));
      addSnippet(new Snippet("WmProgressiveDownloadStatus", checkHtmlString(WM_PROGRESSIVE_DOWNLOAD_STATUS)));
      addSnippet(new Snippet("WmProgressiveDownloadAlert", checkHtmlString(WM_PROGRESSIVE_DOWNLOAD_ALERT)));
      addSnippet(new Snippet("WmProgressiveDownloadWmp64", checkHtmlString(WM_PROGRESSIVE_DOWNLOAD_WMP64)));
      addSnippet(new Snippet("UrlHasIllegalCharacters", checkHtmlString(URL_HAS_ILLEGAL_CHARACTERS)));
      addSnippet(new Snippet("UrlNoReplayPossible", checkHtmlString(URL_NO_REPLAY_POSSIBLE)));
      addSnippet(new Snippet("NoJavascriptHeader", checkHtmlString(NO_JAVASCRIPT_HEADER)));
      addSnippet(new Snippet("NoJavascriptReason", checkHtmlString(NO_JAVASCRIPT_REASON)));
      addSnippet(new Snippet("NoJavascriptCause1", checkHtmlString(NO_JAVASCRIPT_CAUSE1)));
      addSnippet(new Snippet("NoJavascriptSpecial", checkHtmlString(NO_JAVASCRIPT_SPECIAL)));
      addSnippet(new Snippet("NoJavascriptCause2", checkHtmlString(NO_JAVASCRIPT_CAUSE2)));
      addSnippet(new Snippet("NoJavascriptImage", checkHtmlString(NO_JAVASCRIPT_IMAGE)));
      addSnippet(new Snippet("PopupBlockerAlert", POPUP_BLOCKER_ALERT));
      addSnippet(new Snippet("PopupBlockerText1", POPUP_BLOCKER_TEXT1));
      addSnippet(new Snippet("PopupBlockerText2", POPUP_BLOCKER_TEXT2));
      addSnippet(new Snippet("PopupBlockerText3", POPUP_BLOCKER_TEXT3));
      addSnippet(new Snippet("PopupBlockerText4", POPUP_BLOCKER_TEXT4));
      addSnippet(new Snippet("PopupBlockerText5", POPUP_BLOCKER_TEXT5));
//      addSnippet(new Snippet("TestingResult", checkHtmlString(TESTING_RESULT)));
//      addSnippet(new Snippet("TestingQuestion", checkHtmlString(TESTING_QUESTION)));
//      addSnippet(new Snippet("TestingAnswer", checkHtmlString(TESTING_ANSWER)));
//      addSnippet(new Snippet("TestingPoints", checkHtmlString(TESTING_POINTS)));
//      addSnippet(new Snippet("TestingMaximumPoints", checkHtmlString(TESTING_MAXIMUM_POINTS)));
//      addSnippet(new Snippet("TestingRequiredPoints", checkHtmlString(TESTING_REQUIRED_POINTS)));
//      addSnippet(new Snippet("TestingRequiredPercentage", checkHtmlString(TESTING_REQUIRED_PERCENTAGE)));
//      addSnippet(new Snippet("TestingAchievedPoints", checkHtmlString(TESTING_ACHIEVED_POINTS)));
//      addSnippet(new Snippet("TestingAchievedPercentage", checkHtmlString(TESTING_ACHIEVED_PERCENTAGE)));
      addSnippet(new Snippet("TestingCorrect", checkHtmlString(TESTING_CORRECT)));
      addSnippet(new Snippet("TestingWrong", checkHtmlString(TESTING_WRONG)));
      addSnippet(new Snippet("TestingSumOfAllPoints", checkHtmlString(TESTING_SUM_OF_ALL_POINTS)));
      addSnippet(new Snippet("TestingAccuracy", checkHtmlString(TESTING_ACCURACY)));
      addSnippet(new Snippet("TestingAttempts", checkHtmlString(TESTING_ATTEMPTS)));
      
      addSnippet(new Snippet("FeedbackTypeUndefined", ("" + Feedback.FEEDBACK_TYPE_UNDEFINED)));
      addSnippet(new Snippet("FeedbackTypeQCorrect", ("" + Feedback.FEEDBACK_TYPE_Q_CORRECT)));
      addSnippet(new Snippet("FeedbackTypeQWrong", ("" + Feedback.FEEDBACK_TYPE_Q_WRONG)));
      addSnippet(new Snippet("FeedbackTypeQRepeat", ("" + Feedback.FEEDBACK_TYPE_Q_REPEAT)));
      addSnippet(new Snippet("FeedbackTypeQTime", ("" + Feedback.FEEDBACK_TYPE_Q_TIME)));
      addSnippet(new Snippet("FeedbackTypeQTries", ("" + Feedback.FEEDBACK_TYPE_Q_TRIES)));
      addSnippet(new Snippet("FeedbackTypeQqPassed", ("" + Feedback.FEEDBACK_TYPE_QQ_PASSED)));
      addSnippet(new Snippet("FeedbackTypeQqFailed", ("" + Feedback.FEEDBACK_TYPE_QQ_FAILED)));
      addSnippet(new Snippet("FeedbackTypeQqEval", ("" + Feedback.FEEDBACK_TYPE_QQ_EVAL)));
      addSnippet(new Snippet("TextTypeNothing", ("" + Text.TEXT_TYPE_NOTHING)));
      addSnippet(new Snippet("TextTypeChangeTime", ("" + Text.TEXT_TYPE_CHANGE_TIME)));
      addSnippet(new Snippet("TextTypeChangeTries", ("" + Text.TEXT_TYPE_CHANGE_TRIES)));
      addSnippet(new Snippet("TextTypeCangeNumber", ("" + Text.TEXT_TYPE_CHANGE_NUMBER)));
      addSnippet(new Snippet("TextTypeEvalQCorrect", ("" + Text.TEXT_TYPE_EVAL_Q_CORRECT)));
      addSnippet(new Snippet("TextTypeEvalQPoints", ("" + Text.TEXT_TYPE_EVAL_Q_POINTS)));
      addSnippet(new Snippet("TextTypeEvalQqPassed", ("" + Text.TEXT_TYPE_EVAL_QQ_PASSED)));
      addSnippet(new Snippet("TextTypeEvalQqFailed", ("" + Text.TEXT_TYPE_EVAL_QQ_FAILED)));
      addSnippet(new Snippet("TextTypeEvalQqAchieved", ("" + Text.TEXT_TYPE_EVAL_QQ_ACHIEVED)));
      addSnippet(new Snippet("TextTypeEvalQqAchievedPercent", ("" + Text.TEXT_TYPE_EVAL_QQ_ACHIEVED_PERCENT)));
      addSnippet(new Snippet("TextTypeEvalQqTotal", ("" + Text.TEXT_TYPE_EVAL_QQ_TOTAL)));
      addSnippet(new Snippet("TextTypeEvalQqRequired", ("" + Text.TEXT_TYPE_EVAL_QQ_REQUIRED)));
      addSnippet(new Snippet("TextTypeEvalQqRequiredPercent", ("" + Text.TEXT_TYPE_EVAL_QQ_REQUIRED_PERCENT)));
//      addSnippet(new Snippet("TextTypeChangeLast", ("" + Text.TEXT_TYPE_CHANGE_LAST))); // This is a private constant
      addSnippet(new Snippet("TextTypeBlankSupport", ("" + Text.TEXT_TYPE_BLANK_SUPPORT)));
      addSnippet(new Snippet("TextTypeRadioSupport", ("" + Text.TEXT_TYPE_RADIO_SUPPORT)));
      addSnippet(new Snippet("TextTypeQuestionTitle", ("" + Text.TEXT_TYPE_QUESTION_TITLE)));
      addSnippet(new Snippet("TextTypeQuestionText", ("" + Text.TEXT_TYPE_QUESTION_TEXT)));
      addSnippet(new Snippet("TextTypeNumberSupport", ("" + Text.TEXT_TYPE_NUMBER_SUPPORT)));
      addSnippet(new Snippet("TextTypeTriesSupport", ("" + Text.TEXT_TYPE_TRIES_SUPPORT)));
      addSnippet(new Snippet("TextTypeTimeSupport", ("" + Text.TEXT_TYPE_TIME_SUPPORT)));
      addSnippet(new Snippet("TextTypeTargetSupport", ("" + Text.TEXT_TYPE_TARGET_SUPPORT)));

      StringBuffer buffer = new StringBuffer(1024); // Stringbuffer with initial size of 1 kb

      // -------------------------------------------------------
      // Keywords from Metadata
      addSnippet(new Snippet("MetadataKeywords", 
                             checkHtmlString(getMetadataKeywords(metadata))));

      boolean hasKeywords = false;
      // -------------------------------------------------------
      // Only the metadata keywords, without "imc, lecturnity,..."
      buffer = new StringBuffer(1024);
      if (metadata != null)
      {
         for (int i = 0; i < metadata.keywords.length; ++i)
         {
            if (i != 0)
               buffer.append("; ");
            buffer.append(metadata.keywords[i]);
            hasKeywords = true;
         }
      }
      if (!hasKeywords)
         buffer.append("-");
      addSnippet(new Snippet("MetadataKeywords2", checkHtmlString(buffer.toString())));
      
      // -------------------------------------------------------
      // SlideEntries ("SlideEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_slideEntries = new Array();" + END_LN);
      if (m_DocInfo.aSlideEntries != null) // Denver documents have no slides!
      {
         for (int i = 0; i < m_DocInfo.aSlideEntries.length; ++i)
         {
            SlideEntry se = m_DocInfo.aSlideEntries[i];
            buffer.append("g_slideEntries[" + i + "] = new SlideEntry(" + se.nImageNumber +
                          ", " + se.lTimestamp +
                          ", \"" + se.strImageName + ".png\"" + 
                          ", " + se.nLayerIndex + 
                          ", " + se.nX +
                          ", " + se.nY + 
                          ", " + se.nWidth + 
                          ", " + se.nHeight + ");" + END_LN);
         }
      }
      addSnippet(new Snippet("SlideEntries", buffer.toString()));
      
      // -------------------------------------------------------
      // PZI: SearchEntries ("SearchEntries")
      // use ThumbEntries, because ThumbEntries relate to Pages in LMD, which is not necessarily the case for SlideEntries
      buffer = new StringBuffer(1024);
      
      buffer.append("var g_searchEntries = new Array();" + END_LN);
      if (m_DocInfo.aThumbnailEntries != null) // Denver documents have no slides!
      {
         for (int i = 0; i < m_DocInfo.aThumbnailEntries.length; ++i)
         {
            //System.out.println("\nPage "+(i+1)+":");
            
            buffer.append("g_searchEntries[" + i + "] = new Array(");
            ThumbnailEntry te = m_DocInfo.aThumbnailEntries[i];
            // add title and timestamp
            // maybe use: metadata.thumbnails[i].title
            String word = te.strCaption;
            word = word.replace("\\", "\\\\"); // Bugfix 5410: escape backslash
            word = word.replace("\"", "\\\""); // Bugfix 5330: escape double quotes
            buffer.append("\""+word+"\"");   
            buffer.append(", "+m_DocInfo.aThumbnailEntries[i].lBeginTimestamp);
            
            // add keywords and timestamps
            if (metadata != null && metadata.thumbnails != null)
            {               
               for (int j = 0; j < metadata.thumbnails[i].keywords.length; ++j)
               {
                  // if (j != 0)
                     buffer.append(", ");
                  word = metadata.thumbnails[i].keywords[j];
                  word = word.replace("\\", "\\\\"); // Bugfix 5410: escape backslash
                  word = word.replace("\"", "\\\""); // Bugfix 5330: escape double quotes
                  buffer.append("\""+word+"\"");   
                  buffer.append(", "+m_DocInfo.aThumbnailEntries[i].lBeginTimestamp);
               }
            }
            
            // add full text and timestamps
            for (int j = 0; j < te.aSearchableText.length; ++j)
            {
               word = te.aSearchableText[j].getAllContent();               
               word = word.replace("\\", "\\\\"); // Bugfix 5410: escape backslash
               word = word.replace("\"", "\\\""); // Bugfix 5330: escape double quotes
               // if(j>0)
                  buffer.append(", ");
                  buffer.append("\""+word+"\"");
                  buffer.append(", "+te.aSearchableText[j].getTimestamp());
            }
            buffer.append(");" + END_LN);
         }
      }
      addSnippet(new Snippet("SearchEntries", buffer.toString()));
      
      // -------------------------------------------------------
      // ThumbEntry array ("ThumbEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_thumbEntries = new Array();" + END_LN);
      if (m_DocInfo.aThumbnailEntries != null) // Denver documents have no thumbnails!
      {
         for (int i = 0; i < m_DocInfo.aThumbnailEntries.length; ++i)
         {
            ThumbnailEntry te = m_DocInfo.aThumbnailEntries[i];
            String strCaption = te.strCaption;
            String strCaptionShort = truncateTo(te.strCaption, 28);
            // Check for HTML Syntax
            strCaption = checkHtmlString(strCaption);
            strCaptionShort = checkHtmlString(strCaptionShort);

            buffer.append("g_thumbEntries[" + i + "] = new ThumbEntry(\"" + strCaptionShort + "\"" + 
                          ", \"" + strCaption + "\"" +
                          ", " + te.lBeginTimestamp + 
                          ", \"" + te.strImageName + ".png\");" + END_LN);
         }
      }
      addSnippet(new Snippet("ThumbEntries", buffer.toString()));

      // -------------------------------------------------------
      // ClipEntries ("ClipEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_clipEntries = new Array();" + END_LN);
      if (bHasClips)
      {
         for (int i = 0; i < iClipCount; ++i)
         {
            VideoClipInfo vci = aClipInfos[i];
            buffer.append("g_clipEntries[" + i + "] = new ClipEntry(" + vci.startTimeMs + 
                          ", " + vci.videoWidth +
                          ", " + vci.videoHeight + ");" + END_LN);
         }
      }
      addSnippet(new Snippet("ClipEntries", buffer.toString()));

      // -------------------------------------------------------
      // Media durations and start times ("MediaData")
      buffer = new StringBuffer(1024);
      buffer.append("var g_mediaStartTimes = new Array();" + END_LN);
      buffer.append("var g_mediaDurations = new Array();" + END_LN);
      buffer.append("g_mediaStartTimes[0] = 0;" + END_LN);
      buffer.append("g_mediaDurations[0] = " + m_lDuration + ";" + END_LN);
      if (bHasClips)
      {
         for (int i = 0; i < aClipInfos.length; ++i)
         {
            VideoClipInfo vci = aClipInfos[i];
            buffer.append("g_mediaStartTimes[" + (i+1) + "] = " + vci.startTimeMs + ";" + END_LN);
            buffer.append("g_mediaDurations[" + (i+1) + "] = " + vci.videoDurationMs + ";" + END_LN);
         }
      }
      addSnippet(new Snippet("MediaData", buffer.toString()));

      // -------------------------------------------------------
      // TpEntry objects ("TelepointerEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_tpEntries = new Array();" + END_LN);
      if (m_DocInfo.aTelepointerEntries != null)
      {
         for (int i = 0; i < m_DocInfo.aTelepointerEntries.length; ++i)
         {
            TelepointerEntry te = m_DocInfo.aTelepointerEntries[i];
            buffer.append("g_tpEntries[" + i + "] = new TpEntry(" + te.lTimestamp +
                          ", " + te.nX +
                          ", " + te.nY + 
                          ", " + te.nType + 
                          ", " + te.bIsVisible + ");" + END_LN);
         }
      }
      addSnippet(new Snippet("TelepointerEntries", buffer.toString()));

      // -------------------------------------------------------
      // InteractionEntry objects ("InteractionEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_interactionEntries = new Array();" + END_LN);
      int idxIac = 0;
      if (m_DocInfo.aInteractionAreas != null)
      {
         for (int i = 0; i < m_DocInfo.aInteractionAreas.length; ++i)
         {
            InteractionArea iacArea = m_DocInfo.aInteractionAreas[i];
            String strId = "Interaction_" + idxIac;
            buffer.append("g_interactionEntries[" + idxIac + "] = new InteractionEntry('" + strId +
                          "', " + "false" +
                          ", " + "false" +
                          ", " + (int)(0.5 + iacArea.GetActivityArea().x * m_dScaling) +
                          ", " + (int)(0.5 + iacArea.GetActivityArea().y * m_dScaling) +
                          ", " + iacArea.GetVisibilityTimes()[0] +
                          ", " + iacArea.GetVisibilityTimes()[1] + 
                          ", " + iacArea.GetActivityTimes()[0] + 
                          ", " + iacArea.GetActivityTimes()[1] + ");" + END_LN);
            idxIac++;
         }
         // TODO: Consider more than 1 activity time period
      }
      if (m_DocInfo.aMyButtonAreas != null)
      {
         for (int i = 0; i < m_DocInfo.aMyButtonAreas.length; ++i)
         {
            MyButtonArea mbArea = m_DocInfo.aMyButtonAreas[i];
            String strId = "Interaction_" + idxIac;
            // Check for 'Testing' action: 'check'/'reset'
            boolean bIsTestingButton = false;
            String strActionMouseClick = 
               mbArea.m_buttonArea.GetActionString(InteractionArea.ACTION_MOUSE_CLICK);
            if (strActionMouseClick == null)
               strActionMouseClick = 
                  mbArea.m_buttonArea.GetActionString(InteractionArea.ACTION_MOUSE_DOWN);
            if (strActionMouseClick == null)
            {
               System.out.println("Error: no action for button defined!");
               strActionMouseClick = "";
            }
            if ((strActionMouseClick.indexOf("check") == 0) || (strActionMouseClick.indexOf("reset") == 0))
               bIsTestingButton = true;
            buffer.append("g_interactionEntries[" + idxIac + "] = new InteractionEntry('" + strId +
                          "', " + "true" +
                          ", " + bIsTestingButton +
//                          ", " + (int)(0.5 + mbArea.m_buttonArea.GetActivityArea().x * m_dScaling) +
//                          ", " + (int)(0.5 + mbArea.m_buttonArea.GetActivityArea().y * m_dScaling) +
                          ", " + mbArea.m_buttonArea.getClip(m_dScaling).x +
                          ", " + mbArea.m_buttonArea.getClip(m_dScaling).y +
                          ", " + mbArea.m_buttonArea.GetVisibilityTimes()[0] +
                          ", " + mbArea.m_buttonArea.GetVisibilityTimes()[1] + 
                          ", " + mbArea.m_buttonArea.GetActivityTimes()[0] + 
                          ", " + mbArea.m_buttonArea.GetActivityTimes()[1] + ");" + END_LN);
            idxIac++;
         }
         // TODO: Consider more than 1 activity time period
      }
      if (m_DocInfo.aTestingMoveablePictures != null)
      {
         for (int i = 0; i < m_DocInfo.aTestingMoveablePictures.length; ++i)
         {
            Picture pic = m_DocInfo.aTestingMoveablePictures[i];
            int nPage = getPageFromActivityTimes(pic.GetActivityTimes());
            String strId = "DragImg" + nPage + "_" + pic.GetBelongsKey();
            // Activity time is equal to visibility time
            buffer.append("g_interactionEntries[" + idxIac + "] = new InteractionEntry('" + strId +
                          "', " + "false" +
                          ", " + "false" +
                          ", " + (int)(0.5 + pic.GetOrigImageArea().x * m_dScaling) +
                          ", " + (int)(0.5 + pic.GetOrigImageArea().y * m_dScaling) +
//                          ", " + pic.getClip(m_dScaling).x +
//                          ", " + pic.getClip(m_dScaling).y +
                          ", " + pic.GetVisibilityTimes()[0] +
                          ", " + pic.GetVisibilityTimes()[1] + 
                          ", " + pic.GetVisibilityTimes()[0] + 
                          ", " + pic.GetVisibilityTimes()[1] + ");" + END_LN);
            idxIac++;
         }
      }
      if (m_DocInfo.aTestingTargetPoints != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingTargetPoints.length; ++i)
         {
            TargetPoint tpt = m_DocInfo.aTestingTargetPoints[i];
            int nPage = getPageFromActivityTimes(tpt.GetVisibilityTimes());
            if (nPage != nPageBefore)
               idx = 0;
            String strId = "Target" + nPage + "_" + idx;
            // Activity time is equal to visibility time
            buffer.append("g_interactionEntries[" + idxIac + "] = new InteractionEntry('" + strId +
                          "', " + "false" +
                          ", " + "false" +
                          ", " + (int)((tpt.GetArea().x * m_dScaling) + 0.5) +
                          ", " + (int)((tpt.GetArea().y * m_dScaling) + 0.5) +
                          ", " + tpt.GetVisibilityTimes()[0] +
                          ", " + tpt.GetVisibilityTimes()[1] + 
                          ", " + tpt.GetVisibilityTimes()[0] + 
                          ", " + tpt.GetVisibilityTimes()[1] + ");" + END_LN);
            nPageBefore = nPage;
            idx++;
            idxIac++;
         }
      }
      if (m_DocInfo.aTestingRadioButtons != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingRadioButtons.length; ++i)
         {
            RadioDynamic rdb = m_DocInfo.aTestingRadioButtons[i];
            int nPage = getPageFromActivityTimes(rdb.GetActivityTimes());
            if (nPage != nPageBefore)
               idx = 0;
         	String strId = "Radio" + nPage + "_" + idx;
            // Activity time is equal to visibility time
            buffer.append("g_interactionEntries[" + idxIac + "] = new InteractionEntry('" + strId +
                          "', " + "false" +
                          ", " + "false" +
                          ", " + ((int)(0.5 + rdb.GetActivityArea().x * m_dScaling) - 2) +
                          ", " + ((int)(0.5 + rdb.GetActivityArea().y * m_dScaling) - 2) +
//                          ", " + (rdb.getClip(m_dScaling).x - 2) +
//                          ", " + (rdb.getClip(m_dScaling).y - 2) +
                          ", " + rdb.GetVisibilityTimes()[0] +
                          ", " + rdb.GetVisibilityTimes()[1] + 
                          ", " + rdb.GetActivityTimes()[0] + 
                          ", " + rdb.GetActivityTimes()[1] + ");" + END_LN);
            nPageBefore = nPage;
            idx++;
            idxIac++;
         }
      }
      if (m_DocInfo.aTestingTextFields != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingTextFields.length; ++i)
         {
            TextFieldDynamic tfd = m_DocInfo.aTestingTextFields[i];
            int nPage = getPageFromActivityTimes(tfd.GetActivityTimes());
            if (nPage != nPageBefore)
               idx = 0;
         	String strId = "Text" + nPage + "_" + idx;
            // Activity time is equal to visibility time
            buffer.append("g_interactionEntries[" + idxIac + "] = new InteractionEntry('" + strId +
                          "', " + "false" +
                          ", " + "false" +
                          ", " + ((int)(0.5 + tfd.GetActivityArea().x * m_dScaling) + 0 ) +
                          ", " + ((int)(0.5 + tfd.GetActivityArea().y * m_dScaling) + 0 ) +
//                          ", " + (tfd.getClip(m_dScaling).x + 0) +
//                          ", " + (tfd.getClip(m_dScaling).y + 0) +
                          ", " + tfd.GetVisibilityTimes()[0] +
                          ", " + tfd.GetVisibilityTimes()[1] + 
                          ", " + tfd.GetActivityTimes()[0] + 
                          ", " + tfd.GetActivityTimes()[1] + ");" + END_LN);
            nPageBefore = nPage;
            idx++;
            idxIac++;
         }
      }
      if (m_DocInfo.aTestingChangeTexts != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingChangeTexts.length; ++i)
         {
            Text txt = m_DocInfo.aTestingChangeTexts[i];
            int nPage = getPageFromActivityTimes(txt.GetVisibilityTimes());
            if (nPage != nPageBefore)
               idx = 0;
         	String strId = "ChangeText" + nPage + "_" + idx;
            // Activity time is equal to visibility time
            buffer.append("g_interactionEntries[" + idxIac + "] = new InteractionEntry('" + strId +
                          "', " + "false" +
                          ", " + "false" +
                          ", " + (txt.getClip(m_dScaling).x + 2) +
                          ", " + (txt.getClip(m_dScaling).y + 7) + 
                          ", " + txt.GetVisibilityTimes()[0] +
                          ", " + txt.GetVisibilityTimes()[1] + 
                          ", " + txt.GetVisibilityTimes()[0] + 
                          ", " + txt.GetVisibilityTimes()[1] + ");" + END_LN);
            nPageBefore = nPage;
            idx++;
            idxIac++;
         }
      }
      if (m_DocInfo.aTestingQuestionTexts != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingQuestionTexts.length; ++i)
         {
            Text txt = m_DocInfo.aTestingQuestionTexts[i];
            int nPage = getPageFromActivityTimes(txt.GetVisibilityTimes());
            if (nPage != nPageBefore)
               idx = 0;
         	String strId = "QuestionText" + nPage + "_" + idx;
            // Activity time is equal to visibility time
            buffer.append("g_interactionEntries[" + idxIac + "] = new InteractionEntry('" + strId +
                          "', " + "false" +
                          ", " + "false" +
                          ", " + (txt.getClip(m_dScaling).x + 2) +
                          ", " + (txt.getClip(m_dScaling).y + 7) + 
                          ", " + txt.GetVisibilityTimes()[0] +
                          ", " + txt.GetVisibilityTimes()[1] + 
                          ", " + txt.GetVisibilityTimes()[0] + 
                          ", " + txt.GetVisibilityTimes()[1] + ");" + END_LN);
            nPageBefore = nPage;
            idx++;
            idxIac++;
         }
      }
      addSnippet(new Snippet("InteractionEntries", buffer.toString()));

      // -------------------------------------------------------
      // Create Interactive objects ("CreateInteractiveObjects")
      buffer = new StringBuffer(1024);
      idxIac = 0;
      if (m_DocInfo.aInteractionAreas != null)
      {
         for (int i = 0; i < m_DocInfo.aInteractionAreas.length; ++i)
         {
            InteractionArea iacArea = m_DocInfo.aInteractionAreas[i];
            String strId = "Interaction_" + idxIac;
            buffer.append("button_outputInteractionArea('" + strId +
                          "', '" + (int)(0.5 + iacArea.GetActivityArea().x * m_dScaling) +
                          "', '" + (int)(0.5 + iacArea.GetActivityArea().y * m_dScaling) +
                          "', '" + (int)(0.5 + iacArea.GetActivityArea().width * m_dScaling) +
                          "', '" + (int)(0.5 + iacArea.GetActivityArea().height * m_dScaling) +
                          "', '" + idxIac +
                          "', '" + iacArea.HasHandCursor() +
                          "');" + END_LN);
            idxIac++;
         }
      }
      if (m_DocInfo.aMyButtonAreas != null)
      {
         for (int i = 0; i < m_DocInfo.aMyButtonAreas.length; ++i)
         {
            MyButtonArea mbArea = m_DocInfo.aMyButtonAreas[i];
            String strId = "Interaction_" + idxIac;
            buffer.append("button_outputInteractionButton('" + strId +
//                          "', '" + (int)(0.5 + mbArea.m_buttonArea.GetActivityArea().x * m_dScaling) +
//                          "', '" + (int)(0.5 + mbArea.m_buttonArea.GetActivityArea().y * m_dScaling) +
//                          "', '" + (int)(0.5 + mbArea.m_buttonArea.GetActivityArea().width * m_dScaling) +
//                          "', '" + (int)(0.5 + mbArea.m_buttonArea.GetActivityArea().height * m_dScaling) +
                          "', '" + mbArea.m_buttonArea.getClip(m_dScaling).x +
                          "', '" + mbArea.m_buttonArea.getClip(m_dScaling).y +
                          "', '" + mbArea.m_buttonArea.getClip(m_dScaling).width +
                          "', '" + mbArea.m_buttonArea.getClip(m_dScaling).height  +
                          "', '" + idxIac +
                          "', '" + mbArea.m_buttonArea.HasHandCursor() +
                          "');" + END_LN);
            idxIac++;
         }
      }
      addSnippet(new Snippet("CreateInteractiveObjects", buffer.toString()));

      // -------------------------------------------------------
      // Prepare Interactive objects ("PrepareInteractiveObjects")
      buffer = new StringBuffer(1024);
      idxIac = 0;
      if (m_DocInfo.aInteractionAreas != null)
      {
         for (int i = 0; i < m_DocInfo.aInteractionAreas.length; ++i)
         {
            InteractionArea iacArea = m_DocInfo.aInteractionAreas[i];
            String strId = "Interaction_" + idxIac;

            String strActionMouseClick = 
               iacArea.GetActionString(InteractionArea.ACTION_MOUSE_CLICK);
            strActionMouseClick = reinterpretActionStringForWebTemplates(strActionMouseClick, -1);

            String strActionMouseEnter = 
               iacArea.GetActionString(InteractionArea.ACTION_MOUSE_ENTER);
            strActionMouseEnter = reinterpretActionStringForWebTemplates(strActionMouseEnter, -1);

            String strActionMouseLeave = 
               iacArea.GetActionString(InteractionArea.ACTION_MOUSE_LEAVE);
            strActionMouseLeave = reinterpretActionStringForWebTemplates(strActionMouseLeave, -1);

            String strActionMouseDown = 
               iacArea.GetActionString(InteractionArea.ACTION_MOUSE_DOWN);
            strActionMouseDown = reinterpretActionStringForWebTemplates(strActionMouseDown, -1);

            String strActionMouseUp = 
               iacArea.GetActionString(InteractionArea.ACTION_MOUSE_UP);
            strActionMouseUp = reinterpretActionStringForWebTemplates(strActionMouseUp, -1);

//            buffer.append("button_prepareInteractionArea('" + strId +
//                          "', '" + "" + 
//                          "', '" + "false" +
//                          "', '" + strActionMouseClick +
//                          "', '" + strActionMouseEnter +
//                          "', '" + strActionMouseLeave +
//                          "', '" + strActionMouseDown +
//                          "', '" + strActionMouseUp +
//                          "');" + END_LN);

            // Note for Bugfix 3770: 
            // 'button_prepareInteractionArea(...)' used 'false' as 3. parameter for bEnabled
            // and I was wondering why ... InteractionAreas should always be enabled when visible.
            
            // Bugfix 3770:
            fillBufferForInteraction(buffer, 
                                     strId, 
                                     "", 
                                     "true", 
                                     strActionMouseClick, 
                                     strActionMouseEnter, 
                                     strActionMouseLeave, 
                                     strActionMouseDown, 
                                     strActionMouseUp, 
                                     "", 
                                     "");

            idxIac++;
         }
      }
      if (m_DocInfo.aMyButtonAreas != null)
      {
         // Add cross browser code for IE6 and older (which can not display images with semi transparent colors
         // --> switch between 32-bit ARGB and 256 color palette PNGs

         buffer.append("  var isIE6AndOlder = false;" + END_LN);
         buffer.append("  var ua = navigator.userAgent.toLowerCase();" + END_LN);
         buffer.append("  var idxIEComp = ua.indexOf('compatible');" + END_LN);
         buffer.append("  var av = navigator.appVersion.toLowerCase();" + END_LN);
         buffer.append("  var idxMsie = av.indexOf('msie');" + END_LN);
         buffer.append("  if ((idxIEComp >= 0) && (idxMsie >= 0))" + END_LN);
         buffer.append("  {" + END_LN);
         buffer.append("    var av2 = av.substring(idxMsie+5, av.length);" + END_LN);
         buffer.append("    var version = av2.substring(0, av2.indexOf('.')) * 1;" + END_LN);
         buffer.append("    if (version < 7)" + END_LN);
         buffer.append("      isIE6AndOlder = true;" + END_LN);
         buffer.append("  }" + END_LN);
         buffer.append("  " + END_LN);
         buffer.append("  var strNormal, strOver, strPressed, strInactive;" + END_LN);
         buffer.append("  if (isIE6AndOlder)" + END_LN);
         buffer.append("  {" + END_LN);
         buffer.append("    strNormal = 'normal256.png';" + END_LN);
         buffer.append("    strOver = 'over256.png';" + END_LN);
         buffer.append("    strPressed = 'pressed256.png';" + END_LN);
         buffer.append("    strInactive = 'inactive256.png';" + END_LN);
         buffer.append("  }" + END_LN);
         buffer.append("  else" + END_LN);
         buffer.append("  {" + END_LN);
         buffer.append("    strNormal = 'normal.png';" + END_LN);
         buffer.append("    strOver = 'over.png';" + END_LN);
         buffer.append("    strPressed = 'pressed.png';" + END_LN);
         buffer.append("    strInactive = 'inactive.png';" + END_LN);
         buffer.append("  }" + END_LN);
         buffer.append("" + END_LN);

         for (int i = 0; i < m_DocInfo.aMyButtonAreas.length; ++i)
         {
            MyButtonArea mbArea = m_DocInfo.aMyButtonAreas[i];
            String strId = "Interaction_" + idxIac;
            int nPage = getPageFromActivityTimes(mbArea.m_buttonArea.GetActivityTimes());
            String strActionMouseClick = 
               mbArea.m_buttonArea.GetActionString(InteractionArea.ACTION_MOUSE_CLICK);
            if (strActionMouseClick == null)
               strActionMouseClick = 
                  mbArea.m_buttonArea.GetActionString(InteractionArea.ACTION_MOUSE_DOWN);
            if (strActionMouseClick == null)
            {
               System.out.println("Error: no action for button defined!");
               strActionMouseClick = "";
            }
            
            strActionMouseClick = reinterpretActionStringForWebTemplates(strActionMouseClick, nPage);

//            buffer.append("button_prepareInteractionButton('" + strId +
//                          "', '" + "" + // mbArea.strLabel + 
//                          "', '" + "false" +
//                          "', '" + strActionMouseClick +
//                          "', '" + "" +
//                          "', '" + "" +
//                          "', '" + "" +
//                          "', '" + "" +
//                          "', '" + "slides/" + 
//                          "', '" + mbArea.m_strId + "' + strNormal" + 
//                          ", '" + mbArea.m_strId + "' + strOver" + 
//                          ", '" + mbArea.m_strId + "' + strPressed" + 
//                          ", '" + mbArea.m_strId + "' + strInactive" + 
//                          ");" + END_LN);

            // Bugfix 3770:
            fillBufferForInteraction(buffer, 
                                     strId, 
                                     "", /* mbArea.strLabel */
                                     "false", 
                                     strActionMouseClick, 
                                     "", 
                                     "", 
                                     "", 
                                     "", 
                                     "slides/", 
                                     mbArea.m_strId);

            idxIac++;
         }
      }
      addSnippet(new Snippet("PrepareInteractiveObjects", buffer.toString()));

      // -------------------------------------------------------
      // Stop mark times ("StopmarkEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_stopmarkTimes = new Array();" + END_LN);
      if (m_DocInfo.aStopmarkTimes != null)
      {
         for (int i = 0; i < m_DocInfo.aStopmarkTimes.length; ++i)
         {
         	int msStopmark = m_DocInfo.aStopmarkTimes[i];
            buffer.append("g_stopmarkTimes[" + i + "] = " + msStopmark + ";" + END_LN);
         }
      }
      addSnippet(new Snippet("StopmarkEntries", buffer.toString()));

      // -------------------------------------------------------
      // Target mark times ("TargetmarkEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_targetmarkTimes = new Array();" + END_LN);
      if (m_DocInfo.aTargetmarkTimes != null)
      {
         for (int i = 0; i < m_DocInfo.aTargetmarkTimes.length; ++i)
         {
         	int msTargetmark = m_DocInfo.aTargetmarkTimes[i];
            buffer.append("g_targetmarkTimes[" + i + "] = " + msTargetmark + ";" + END_LN);
         }
      }
      addSnippet(new Snippet("TargetmarkEntries", buffer.toString()));

      // -------------------------------------------------------
      // TestingInit ("TestingInit")
      buffer = new StringBuffer(1024);
      if (m_DocInfo.aQuestionnaires != null)
        buffer.append("testingInit();");
      addSnippet(new Snippet("TestingInit", buffer.toString()));

      // -------------------------------------------------------
      // Target Point Entries ("TargetPointEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_targetPointEntries = new Array();" + END_LN);
      if (m_DocInfo.aTestingTargetPoints != null)
      {
         // Get the maximum (moveable) picture size
         int wMax = 0, hMax = 0;
         if (m_DocInfo.aTestingMoveablePictures != null)
         {
            for (int i = 0; i < m_DocInfo.aTestingMoveablePictures.length; ++i)
            {
               Picture pic = m_DocInfo.aTestingMoveablePictures[i];
               int w = pic.getClip(m_dScaling).width;
               int h = pic.getClip(m_dScaling).height;
               if (w > wMax)
                  wMax = w;
               if (h > hMax)
                  hMax = h;
            }
         }
         
         for (int i = 0; i < m_DocInfo.aTestingTargetPoints.length; ++i)
         {
            Rectangle rect = m_DocInfo.aTestingTargetPoints[i].GetArea();
            int x = (int)(0.5 + (m_dScaling * rect.x));
            int y = (int)(0.5 + (m_dScaling * rect.y));
            int w = (int)(0.5 + (m_dScaling * rect.width));
            int h = (int)(0.5 + (m_dScaling * rect.height));
            int cx = (int)(0.5 + (m_dScaling * m_DocInfo.aTestingTargetPoints[i].GetCenterPoint().x));
            int cy = (int)(0.5 + (m_dScaling * m_DocInfo.aTestingTargetPoints[i].GetCenterPoint().y));
            int radius = (int)(((wMax + hMax) / 5.0) + 0.5);
            int nPage = getPageFromActivityTimes(m_DocInfo.aTestingTargetPoints[i].GetVisibilityTimes());
            short sBelongsKey = m_DocInfo.aTestingTargetPoints[i].GetBelongsKey();
            String strObjectId = "";
            Picture pic = null;
            if (m_DocInfo.aTestingMoveablePictures != null)
            {
               for (int k = 0; k < m_DocInfo.aTestingMoveablePictures.length; ++k)
               {
                  pic = m_DocInfo.aTestingMoveablePictures[k];
                  int iPage = getPageFromActivityTimes(m_DocInfo.aTestingMoveablePictures[k].GetVisibilityTimes());
                  if ( (nPage == iPage) && (sBelongsKey == pic.GetBelongsKey()) )
                  {
                     strObjectId = "DragImg" + nPage + "_" + sBelongsKey;
                     break;
                  }
               }
            }
            int startMs = m_DocInfo.aTestingTargetPoints[i].GetVisibilityTimes()[0];
            int endMs = m_DocInfo.aTestingTargetPoints[i].GetVisibilityTimes()[1];
            buffer.append("g_targetPointEntries[" + i + "] = new TargetPointEntry(" + x + ", " + y + 
                          ", " + w + ", " + h + ", " + cx + ", " + cy + 
                          ", " + radius + ", \"" + strObjectId + "\", " + nPage + 
                          ", " + startMs + ", " + endMs + ");" + END_LN);
         }
      }
      addSnippet(new Snippet("TargetPointEntries", buffer.toString()));

      // -------------------------------------------------------
      // Moveable objects ("MoveableObjects")
      buffer = new StringBuffer(1024);
      if (m_DocInfo.aTestingMoveablePictures != null)
      {
         for (int i = 0; i < m_DocInfo.aTestingMoveablePictures.length; ++i)
         {
            Picture pic = m_DocInfo.aTestingMoveablePictures[i];
            int nPage = getPageFromActivityTimes(pic.GetVisibilityTimes());
            String strId = "DragImg" + nPage + "_" + pic.GetBelongsKey();
            Rectangle rect = pic.GetOrigImageArea();
            int x = (int)(0.5 + (m_dScaling * rect.x));
            int y = (int)(0.5 + (m_dScaling * rect.y));
            int w = (int)(0.5 + (m_dScaling * rect.width));
            int h = (int)(0.5 + (m_dScaling * rect.height));
            // Bugfix 2558: independent from the input format (png, jpg, ...) always a png is written
            int lastDot = pic.GetFileName().lastIndexOf('.');
            String strImageName = pic.GetFileName().substring(0, lastDot) + "_" + i + ".png";
            buffer.append("<div id=\"" + strId + "Layer\" name=\"moveLayer\" " + 
                          "style=\"position:absolute;" + 
                          "left:" + x + "px;" + 
                          "top:" + y + "px;" + 
                          "width:" + w + "px;" + 
                          "height:" + h + "px;" + 
                          "z-index:" + (m_nMaxLayerIndex + 10000 + m_DocInfo.aTestingMoveablePictures.length) + ";" + 
                          "visibility:hidden;" + 
                          "background-image:url('slides/" + strImageName + "')\">" + 
                          "</div>" + END_LN);
         }
      }
      if (m_DocInfo.aTestingTargetPoints != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingTargetPoints.length; ++i)
         {
            int nPage = getPageFromActivityTimes(m_DocInfo.aTestingTargetPoints[i].GetVisibilityTimes());
            if (nPage != nPageBefore)
               idx = 0;
            String strId = "Target" + nPage + "_" + idx;
         	Rectangle rect = m_DocInfo.aTestingTargetPoints[i].GetArea();
         	int x = (int)(m_dScaling * rect.x + 0.5);
         	int y = (int)(m_dScaling * rect.y + 0.5);
         	int w = (int)(m_dScaling * rect.width + 0.5);
         	int h = (int)(m_dScaling * rect.height + 0.5);
            buffer.append("<div id=\"" + strId + "Layer\" " + 
                          "style=\"position:absolute;" + 
                          "left:" + x + "px;" + 
                          "top:" + y + "px;" + 
                          "width:" + w + "px;" + 
                          "height:" + h + "px;" + 
                          "z-index:" + (m_nMaxLayerIndex + 9999) + ";" + 
                          "border:solid 1px #000000;visibility:hidden;\">" + 
                          "</div>" + END_LN);
            nPageBefore = nPage;
            idx++;
         }
      }
      addSnippet(new Snippet("MoveableObjects", buffer.toString()));
      
      // -------------------------------------------------------
      // RadioButton/CheckBox Entries ("RadioDynamicEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_radioDynamicEntries = new Array();" + END_LN);
      if (m_DocInfo.aTestingRadioButtons != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingRadioButtons.length; ++i)
         {
            RadioDynamic rdb = m_DocInfo.aTestingRadioButtons[i];
            int x = (int)(0.5 + rdb.GetActivityArea().x * m_dScaling);
            int y = (int)(0.5 + rdb.GetActivityArea().y * m_dScaling);
//         	int x = rdb.getClip(m_dScaling).x;
//         	int y = rdb.getClip(m_dScaling).y;
         	x -= 2; y -= 2; // empirically found
         	int size = (int)(0.5 * (rdb.getClip(m_dScaling).width + rdb.getClip(m_dScaling).height));
         	boolean bIsChecked = rdb.IsCorrectAnswer();
            int nPage = getPageFromActivityTimes(rdb.GetActivityTimes());
            if (nPage != nPageBefore)
               idx = 0;
            String strId = "Radio" + nPage + "_" + idx;
         	int startMs = rdb.GetActivityTimes()[0];
         	int endMs = rdb.GetActivityTimes()[1];
            buffer.append("g_radioDynamicEntries[" + i + "] = new RadioDynamicEntry(" + x + ", " + y + 
                          ", " + size + ", " + bIsChecked + ", \"" + strId + "\", " + idx + 
                          ", " + nPage + ", " + startMs + ", " + endMs + ");" + END_LN);
            nPageBefore = nPage;
            idx++;
         }
      }
      addSnippet(new Snippet("RadioDynamicEntries", buffer.toString()));
      
      // -------------------------------------------------------
      // RadioButtons/CheckBoxes  ("RadioDynamics")
      buffer = new StringBuffer(1024);
      if (m_DocInfo.aTestingRadioButtons != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingRadioButtons.length; ++i)
         {
            RadioDynamic rdb = m_DocInfo.aTestingRadioButtons[i];
            int x = (int)(0.5 + rdb.GetActivityArea().x * m_dScaling);
            int y = (int)(0.5 + rdb.GetActivityArea().y * m_dScaling);
//         	int x = rdb.getClip(m_dScaling).x;
//         	int y = rdb.getClip(m_dScaling).y;
         	x -= 2; y -= 2; // empirically found
         	String strType = rdb.IsCheckBox() ? "checkbox" : "radio";
         	String strName = rdb.IsCheckBox() ? "Check" : "Radio";
            int nPage = getPageFromActivityTimes(rdb.GetActivityTimes());
            if (nPage != nPageBefore)
               idx = 0;
         	buffer.append("<div id=\"Radio" + nPage + "_" + idx + "Layer\" " + 
         	              "style=\"position:absolute;left:" + x + "px;top:" + y + "px;" + 
         	              "z-index:" + (m_nMaxLayerIndex + 10000) + ";visibility:hidden;\">" +
         	              "<input type=\"" + strType + "\" name=\"" + strName + nPage /* + "_" + idx */ + 
         	              "\" value=\"\"></div>" + END_LN);
            nPageBefore = nPage;
            idx++;
         }
      }
      addSnippet(new Snippet("RadioDynamics", buffer.toString()));

      // -------------------------------------------------------
      // Text Field Entries ("TextFieldEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_textFieldEntries = new Array();" + END_LN);
      if (m_DocInfo.aTestingTextFields != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingTextFields.length; ++i)
         {
            TextFieldDynamic tfd = m_DocInfo.aTestingTextFields[i];
            int x = (int)(0.5 + tfd.GetActivityArea().x * m_dScaling);
            int y = (int)(0.5 + tfd.GetActivityArea().y * m_dScaling);
//         	int x = tfd.getClip(m_dScaling).x;
//         	int y = tfd.getClip(m_dScaling).y;
         	x += 0; y += 0; // empirically found
         	float fCharWidth = (float)12.0;
         	int size = (int)(tfd.getClip(m_dScaling).width / fCharWidth);
         	String strAnswer = "";
         	ArrayList alAnswers = tfd.GetCorrectTexts();
         	if (!alAnswers.isEmpty())
         	{
         	   for (int k = 0; k < alAnswers.size(); ++k)
         	   {
         	      strAnswer += (String)(alAnswers.get(k));
         	      if (k < (alAnswers.size()-1))
         	         strAnswer +=";";
         	   }
         	}
            int nPage = getPageFromActivityTimes(tfd.GetActivityTimes());
            if (nPage != nPageBefore)
               idx = 0;
            String strId = "Text" + nPage + "_" + idx;
         	int startMs = tfd.GetActivityTimes()[0];
         	int endMs = tfd.GetActivityTimes()[1];
            buffer.append("g_textFieldEntries[" + i + "] = new TextFieldEntry(" + x + ", " + y + 
                          ", " + size + ", \"" + strAnswer + "\", \"" + strId + "\", " + idx + 
                          ", " + nPage + ", " + startMs + ", " + endMs + ");" + END_LN);
            nPageBefore = nPage;
            idx++;
         }
      }
      addSnippet(new Snippet("TextFieldEntries", buffer.toString()));
      
      // -------------------------------------------------------
      // TextFields  ("TextFieldDynamics")
      buffer = new StringBuffer(1024);
      if (m_DocInfo.aTestingTextFields != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingTextFields.length; ++i)
         {
            TextFieldDynamic tfd = m_DocInfo.aTestingTextFields[i];
         	int x = (int)(0.5 + tfd.GetActivityArea().x * m_dScaling);
         	int y = (int)(0.5 + tfd.GetActivityArea().y * m_dScaling);
//         	int x = tfd.getClip(m_dScaling).x;
//         	int y = tfd.getClip(m_dScaling).y;
         	x += 0; y += 0; // empirically found
         	int width = (int)(0.5 + tfd.GetActivityArea().width * m_dScaling);
         	int height = (int)(0.5 + tfd.GetActivityArea().height * m_dScaling);
         	int textSize = height - 6; // empirically found
         	//int tfLength = (int)( 0.5 + ((0.85 * width) / textSize)); // empirically found
            int nPage = getPageFromActivityTimes(tfd.GetActivityTimes());
            if (nPage != nPageBefore)
               idx = 0;
         	buffer.append("<div id=\"Text" + nPage + "_" + idx + "Layer\" " + 
         	              "style=\"position:absolute;left:" + x + "px;top:" + y + "px;" + 
         	              "z-index:" + (m_nMaxLayerIndex + 10000) + ";visibility:hidden;\">" + 
         	              "<input style=\"position:absolute;width:" + width + "px;height:" + height + 
         	              "px;font-size:" + textSize + "px;\" type=\"text\" name=\"Text" + 
         	              nPage  + "_" + idx  + 
         	              "\" class=\"TextField\" " + 
         	              /*"size=\"" + tfLength + "\""*/ "></div>" + END_LN);
            nPageBefore = nPage;
            idx++;
         }
      }
      addSnippet(new Snippet("TextFieldDynamics", buffer.toString()));

      // -------------------------------------------------------
      // Changeable Texts ("ChangeTexts")
      buffer = new StringBuffer(1024);
      if (m_DocInfo.aTestingChangeTexts != null)
      {
         fillTextObjects(buffer, m_DocInfo.aTestingChangeTexts, "ChangeText");
      }
      addSnippet(new Snippet("ChangeTexts", buffer.toString()));
      
      // -------------------------------------------------------
      // Question Texts ("QuestionTexts")
      buffer = new StringBuffer(1024);
      if (m_DocInfo.aTestingQuestionTexts != null)
      {
         fillTextObjects(buffer, m_DocInfo.aTestingQuestionTexts, "QuestionText");
      }
      addSnippet(new Snippet("QuestionTexts", buffer.toString()));
      
      // -------------------------------------------------------
      // Changeable Text Entries ("ChangeTextEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_changeTextEntries = new Array();" + END_LN);
      if (m_DocInfo.aTestingChangeTexts != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingChangeTexts.length; ++i)
         {
         	Text txt = m_DocInfo.aTestingChangeTexts[i];
         	int x = txt.getClip(m_dScaling).x;
         	int y = txt.getClip(m_dScaling).y;
         	x += 2; y += 7; // empirically found
         	int nType = txt.GetTextType();
            int nPage = getPageFromActivityTimes(txt.GetVisibilityTimes());
            if (nPage != nPageBefore)
               idx = 0;
            String strId = "ChangeText" + nPage + "_" + idx;
         	int startMs = txt.GetVisibilityTimes()[0];
         	int endMs = txt.GetVisibilityTimes()[1];
            buffer.append("g_changeTextEntries[" + i + "] = new ChangeTextEntry(" + x + ", " + y + 
                          ", " + nType + ", \"" + strId + "\", " + nPage + 
                          ", " + startMs + ", " + endMs + ");" + END_LN);
            nPageBefore = nPage;
            idx++;
         }
      }
      addSnippet(new Snippet("ChangeTextEntries", buffer.toString()));

      // -------------------------------------------------------
      // Question Text Entries ("QuestionTextEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_questionTextEntries = new Array();" + END_LN);
      if (m_DocInfo.aTestingQuestionTexts != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingQuestionTexts.length; ++i)
         {
         	Text txt = m_DocInfo.aTestingQuestionTexts[i];
            int x = txt.getClip(m_dScaling).x;
            int y = txt.getClip(m_dScaling).y;
         	x += 2; y += 7; // empirically found
         	int nType = txt.GetTextType();
            int nPage = getPageFromActivityTimes(txt.GetVisibilityTimes());
            if (nPage != nPageBefore)
               idx = 0;
            String strId = "QuestionText" + nPage + "_" + idx;
         	int startMs = txt.GetVisibilityTimes()[0];
         	int endMs = txt.GetVisibilityTimes()[1];
            buffer.append("g_questionTextEntries[" + i + "] = new QuestionTextEntry(" + x + ", " + y + 
                          ", " + nType + ", \"" + strId + "\", " + nPage + 
                          ", " + startMs + ", " + endMs + ");" + END_LN);
            nPageBefore = nPage;
            idx++;
         }
      }
      addSnippet(new Snippet("QuestionTextEntries", buffer.toString()));

      // -------------------------------------------------------
      // Questionnaire Entries ("QuestionnaireEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_questionnaireEntries = new Array();" + END_LN);
	  int iRandomPassPercentage = -1;
      if (m_DocInfo.aQuestionnaires != null)
      {
         for (int i = 0; i < m_DocInfo.aQuestionnaires.length; ++i)
         {
            Questionnaire qq = m_DocInfo.aQuestionnaires[i];
            int nNumOfQuestions = qq.GetQuestions().length;
            int nTotalPoints = qq.GetTotalPoints();
            int nPassPoints = qq.GetPassPoints();
            int nStartMs = qq.GetStartMs();
            int nEndMs = qq.GetEndMs();
            //int[] anStartMs = {nStartMs, nStartMs};
            //int[] anEndMs = {nEndMs, nEndMs};
            // Bugfix #2651:
            Question[] aQuestions = qq.GetQuestions();
            if (aQuestions != null && aQuestions.length > 0)
            {
               int nQuestionsStartMs = aQuestions[0].GetStartMs();
               int nQuestionsEndMs = aQuestions[aQuestions.length-1].GetStartMs();
               int[] anStartMs = {nQuestionsStartMs, nQuestionsStartMs};
               int[] anEndMs = {nQuestionsEndMs, nQuestionsEndMs};
               int nPageStart = getPageFromActivityTimes(anStartMs);
               int nPageEnd = getPageFromActivityTimes(anEndMs);
               buffer.append("g_questionnaireEntries[" + i + "] = new QuestionnaireEntry(" + 
                             nNumOfQuestions + ", " + nTotalPoints + ", " + nPassPoints + ", " + 
                             nPageStart + ", " + nPageEnd + ", " + nStartMs + ", " + nEndMs + 
                             ");" + END_LN);
            }
			 iRandomPassPercentage = qq.GetRandomPassPercentage();
			 buffer.append("var g_relativePassPercentage = " + iRandomPassPercentage + ";" + END_LN);
         }
      }
      addSnippet(new Snippet("QuestionnaireEntries", buffer.toString()));
      
       // -------------------------------------------------------
      // Random Questionnaire Entries ("QuestionnaireRandomEntries")
      buffer = new StringBuffer(1024);
      boolean bIsRandom = false;
      int iNrOfRandomQuestions = -1;
      
      if (m_DocInfo.aQuestionnaires != null)
      {
      	Questionnaire qq = m_DocInfo.aQuestionnaires[0];
      	bIsRandom = qq.IsTestRandomized();
      	iNrOfRandomQuestions = qq.GetNumberOfRandomQuestions();
      }	
      
      buffer.append("var g_isRandomTest = " + bIsRandom + ";" + END_LN);
      buffer.append("var g_nrOfRandomQuestions = " + iNrOfRandomQuestions + ";" + END_LN);
      buffer.append("" + END_LN);
      buffer.append("var g_firstQuestionTimestamp = -1;" + END_LN);
      buffer.append("" + END_LN);
      buffer.append("var g_JumpRandomTimestamp = -1;" + END_LN);
      buffer.append("" + END_LN);
      buffer.append("var g_isRandomQuestionOrderInitialized = false;" + END_LN);
      buffer.append("var g_randomQuestionOrder = new Array();" + END_LN);
      for (int i = 0; i < iNrOfRandomQuestions; ++i)
      {
       	buffer.append("g_randomQuestionOrder[" + i + "] = 0;" + END_LN);
      }
	  buffer.append("" + END_LN);
      
     // System.out.println("!!!!Soare - QuestionnaireRandomEntries" + buffer.toString());
      addSnippet(new Snippet("QuestionnaireRandomEntries", buffer.toString()));


      // -------------------------------------------------------
      // Question Entries ("QuestionEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_questionEntries = new Array();" + END_LN);
      if (m_DocInfo.aQuestionnaires != null)
      {
         int idx = 0;
         for (int i = 0; i < m_DocInfo.aQuestionnaires.length; ++i)
         {
            Questionnaire qq = m_DocInfo.aQuestionnaires[i];
            Question[] aQuestions = qq.GetQuestions();
            if (aQuestions != null && aQuestions.length > 0)
            {
               for (int k = 0; k < aQuestions.length; ++k)
               {
                  Question q = aQuestions[k];
                  String strId = q.GetScormId();
                  int nPoints = q.GetPoints();
                  boolean bSuccessfullyAnswered = false;
                  boolean bIsDeactivated = false;
                  int nMaximumAttempts = q.GetMaximumAttempts();
                  int nTakenAttempts = 0;
                  int nTimeoutSec = (int)((float)q.GetMaximumTime() / 1000.0);
                  int nViewedSec = 0;
                  int nStartMs = q.GetStartMs();
                  int nEndMs = q.GetEndMs();
                  int[] anTimesMs = {nStartMs, nEndMs};
                  int nPage = getPageFromActivityTimes(anTimesMs);
                  buffer.append("g_questionEntries[" + idx + "] = new QuestionEntry(" + i + ", \"" + strId + "\", " + 
                              nPoints + ", " + bSuccessfullyAnswered + ", " + bIsDeactivated + ", " + 
                                nMaximumAttempts + ", " + nTakenAttempts + ", " + 
                                nTimeoutSec + ", " + nViewedSec + ", " +
                                nPage + ", " + nStartMs + ", " + nEndMs + 
                                ");" + END_LN);
                  idx++;
               }
            }
         }
      }
      addSnippet(new Snippet("QuestionEntries", buffer.toString()));

      // -------------------------------------------------------
      // Feedback Entries ("FeedbackEntries")
      buffer = new StringBuffer(1024);
      buffer.append("var g_feedbackEntries = new Array();" + END_LN);
      if (m_DocInfo.aQuestionnaires != null)
      {
         int idx = 0;
         int qIdx = 0;
         Feedback fb = null;
         for (int i = 0; i < m_DocInfo.aQuestionnaires.length; ++i)
         {
            Questionnaire qq = m_DocInfo.aQuestionnaires[i];
            fb = qq.GetFeedback(Feedback.FEEDBACK_TYPE_Q_CORRECT);
            if (fb != null)
            {
               fillFeedbackEntry(buffer, idx, fb, i, Feedback.FEEDBACK_TYPE_Q_CORRECT, "", fb.IsEnabled(), true);
               idx++;
            }
            fb = qq.GetFeedback(Feedback.FEEDBACK_TYPE_Q_WRONG);
            if (fb != null)
            {
               fillFeedbackEntry(buffer, idx, fb, i, Feedback.FEEDBACK_TYPE_Q_WRONG, "", fb.IsEnabled(), true);
               idx++;
            }
            fb = qq.GetFeedback(Feedback.FEEDBACK_TYPE_Q_REPEAT);
            if (fb != null)
            {
               fillFeedbackEntry(buffer, idx, fb, i, Feedback.FEEDBACK_TYPE_Q_REPEAT, "", fb.IsEnabled(), true);
               idx++;
            }
            // "Tries" have no feedback message.
            //fb = qq.GetFeedback(Feedback.FEEDBACK_TYPE_Q_TRIES);
            //if (fb != null)
            //{
            //   fillFeedbackEntry(tpc, buffer, idx, fb, i, Feedback.FEEDBACK_TYPE_Q_TRIES, "", fb.IsEnabled(), true);
            //   idx++;
            //}
            fb = qq.GetFeedback(Feedback.FEEDBACK_TYPE_Q_TIME);
            if (fb != null)
            {
               fillFeedbackEntry(buffer, idx, fb, i, Feedback.FEEDBACK_TYPE_Q_TIME, "", fb.IsEnabled(), true);
               idx++;
            }
            fb = qq.GetFeedback(Feedback.FEEDBACK_TYPE_QQ_PASSED);
            if (fb != null)
            {
               String strAction = qq.GetActionString(true);
               fillFeedbackEntry(buffer, idx, fb, i, Feedback.FEEDBACK_TYPE_QQ_PASSED, strAction, fb.IsEnabled(), true);
               idx++;
            }
            fb = qq.GetFeedback(Feedback.FEEDBACK_TYPE_QQ_FAILED);
            if (fb != null)
            {
               String strAction = qq.GetActionString(false);
               fillFeedbackEntry(buffer, idx, fb, i, Feedback.FEEDBACK_TYPE_QQ_FAILED, strAction, fb.IsEnabled(), true);
               idx++;
            }
            fb = qq.GetFeedback(Feedback.FEEDBACK_TYPE_QQ_EVAL);
            if (fb != null)
            {
               fillFeedbackEntry(buffer, idx, fb, i, Feedback.FEEDBACK_TYPE_QQ_EVAL, "", fb.IsEnabled(), true);
               idx++;
            }

            Question[] aQuestions = qq.GetQuestions();
            if (aQuestions != null && aQuestions.length > 0)
            {
               for (int k = 0; k < aQuestions.length; ++k)
               {
                  Question q = aQuestions[k];
                  fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_Q_CORRECT, true);
                  if (fb != null)
                  {
                     String strAction = q.GetActionString(true);
                     fillFeedbackEntry(buffer, idx, fb, qIdx, Feedback.FEEDBACK_TYPE_Q_CORRECT, strAction, fb.IsEnabled(), false);
                     idx++;
                  }
                  fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_Q_WRONG, true);
                  if (fb != null)
                  {
                     String strAction = q.GetActionString(false);
                     fillFeedbackEntry(buffer, idx, fb, qIdx, Feedback.FEEDBACK_TYPE_Q_WRONG, strAction, fb.IsEnabled(), false);
                     idx++;
                  }
                  fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_Q_REPEAT, true);
                  if (fb != null)
                  {
                     fillFeedbackEntry(buffer, idx, fb, qIdx, Feedback.FEEDBACK_TYPE_Q_REPEAT, "", fb.IsEnabled(), false);
                     idx++;
                  }
                  // "Tries" have no feedback message.
                  //fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_Q_TRIES, true);
                  //if (fb != null)
                  //{
                  //   fillFeedbackEntry(tpc, buffer, idx, fb, qIdx, Feedback.FEEDBACK_TYPE_Q_TRIES, "", fb.IsEnabled(), false);
                  //   idx++;
                  //}
                  fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_Q_TIME, true);
                  if (fb != null)
                  {
                     fillFeedbackEntry(buffer, idx, fb, qIdx, Feedback.FEEDBACK_TYPE_Q_TIME, "", fb.IsEnabled(), false);
                     idx++;
                  }
                  //fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_QQ_PASSED, true);
                  fb = q.GetQuestionnaire().GetFeedback(Feedback.FEEDBACK_TYPE_QQ_PASSED);
                  if (fb != null)
                  {
                     String strAction =  q.GetQuestionnaire().GetActionString(true);
                     fillFeedbackEntry(buffer, idx, fb, qIdx, Feedback.FEEDBACK_TYPE_QQ_PASSED, strAction, fb.IsEnabled(), false);
                     idx++;
                  }
                  //fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_QQ_FAILED, true);
                  fb = q.GetQuestionnaire().GetFeedback(Feedback.FEEDBACK_TYPE_QQ_FAILED);
                  if (fb != null)
                  {
                     String strAction =  q.GetQuestionnaire().GetActionString(false);
                     fillFeedbackEntry(buffer, idx, fb, qIdx, Feedback.FEEDBACK_TYPE_QQ_FAILED, strAction, fb.IsEnabled(), false);
                     idx++;
                  }
                  //fb = q.GetFeedback(Feedback.FEEDBACK_TYPE_QQ_EVAL, true);
                  fb = q.GetQuestionnaire().GetFeedback(Feedback.FEEDBACK_TYPE_QQ_EVAL);
                  if (fb != null)
                  {
                     fillFeedbackEntry(buffer, idx, fb, qIdx, Feedback.FEEDBACK_TYPE_QQ_EVAL, "", fb.IsEnabled(), false);
                     idx++;
                  }
                  qIdx++;
               }
            }
         }
      }
      addSnippet(new Snippet("FeedbackEntries", buffer.toString()));

      // -------------------------------------------------------
      // Feedback Objects ("FeedbackObjects")
      buffer = new StringBuffer(1024);
      if (m_DocInfo.aQuestionnaires != null)
      {
         buffer.append("<!-- Feedback -->"+ END_LN);
         buffer.append("<div id=\"protectiveLayer\" style=\"position:absolute;left:0px;top:0px;visibility:hidden;z-index:999998;\"><img id=\"protectiveImg\" src=\"images/trans.gif\" width=\"1\" height=\"1\" alt=\"\"></iframe></div>"+ END_LN);
         buffer.append("<div id=\"feedbackLayer\" style=\"position:absolute;left:0px;top:0px;visibility:hidden;z-index:999999;\"><iframe name=\"iframeMessage\" src=\"__feedback.html\" width=\"100%\" height=\"100%\" allowTransparency=\"true\" frameborder=\"0\" marginwidth=\"0\" marginheight=\"0\" scrolling=\"no\"></iframe></div>"+ END_LN);
      }
      addSnippet(new Snippet("FeedbackObjects", buffer.toString()));

      // -------------------------------------------------------
      // Testing Result Feedbacks
      buffer = new StringBuffer(1024);
      String strResult = getResultFeedbackStringFromIndex(1);
      buffer.append(checkHtmlString( (strResult.length() > 0) ? strResult : TESTING_RESULT ));
      addSnippet(new Snippet("TestingResult", buffer.toString()));

      buffer = new StringBuffer(1024);
      String strQuestion = getResultFeedbackStringFromIndex(2);
      buffer.append(checkHtmlString( (strQuestion.length() > 0) ? strQuestion : TESTING_QUESTION ));
      addSnippet(new Snippet("TestingQuestion", buffer.toString()));

      buffer = new StringBuffer(1024);
      String strAnswer = getResultFeedbackStringFromIndex(3);
      buffer.append(checkHtmlString( (strAnswer.length() > 0) ? strAnswer : TESTING_ANSWER ));
      addSnippet(new Snippet("TestingAnswer", buffer.toString()));

      buffer = new StringBuffer(1024);
      String strPoints = getResultFeedbackStringFromIndex(4);
      buffer.append(checkHtmlString( (strPoints.length() > 0) ? strPoints : TESTING_POINTS ));
      addSnippet(new Snippet("TestingPoints", buffer.toString()));

      buffer = new StringBuffer(1024);
      String strMaximumPoints = getResultFeedbackStringFromIndex(5);
      buffer.append(checkHtmlString( (strMaximumPoints.length() > 0) ? strMaximumPoints : TESTING_MAXIMUM_POINTS ));
      addSnippet(new Snippet("TestingMaximumPoints", buffer.toString()));

      buffer = new StringBuffer(1024);
      String strRequiredPoints = getResultFeedbackStringFromIndex(6);
      buffer.append(checkHtmlString( (strRequiredPoints.length() > 0) ? strRequiredPoints : TESTING_REQUIRED_POINTS ));
      addSnippet(new Snippet("TestingRequiredPoints", buffer.toString()));

      buffer = new StringBuffer(1024);
      String strRequiredPercentage = getResultFeedbackStringFromIndex(7);
      buffer.append(checkHtmlString( (strRequiredPercentage.length() > 0) ? strRequiredPercentage : TESTING_REQUIRED_PERCENTAGE ));
      addSnippet(new Snippet("TestingRequiredPercentage", buffer.toString()));

      buffer = new StringBuffer(1024);
      String strAchievedPoints = getResultFeedbackStringFromIndex(8);
      buffer.append(checkHtmlString( (strAchievedPoints.length() > 0) ? strAchievedPoints : TESTING_ACHIEVED_POINTS ));
      addSnippet(new Snippet("TestingAchievedPoints", buffer.toString()));

      buffer = new StringBuffer(1024);
      String strAchievedPercentage = getResultFeedbackStringFromIndex(9);
      buffer.append(checkHtmlString( (strAchievedPercentage.length() > 0) ? strAchievedPercentage : TESTING_ACHIEVED_PERCENTAGE ));
      addSnippet(new Snippet("TestingAchievedPercentage", buffer.toString()));

      // -------------------------------------------------------
      // Feedback Summary Enabled ("FeedbackSummaryEnabled")
      buffer = new StringBuffer(1024);
      boolean bIsFeedbackSummaryEnabled = true;
      if (m_DocInfo.aQuestionnaires != null)
      {
         for (int i = 0; i < m_DocInfo.aQuestionnaires.length; ++i)
         {
            Questionnaire qq = m_DocInfo.aQuestionnaires[i];
            bIsFeedbackSummaryEnabled = qq.IsShowEvaluation();
            if (bIsFeedbackSummaryEnabled == false)
               break;
         }
      }
      String strBoolean = bIsFeedbackSummaryEnabled ? "true" : "false";
      buffer.append(strBoolean);
      addSnippet(new Snippet("FeedbackSummaryEnabled", buffer.toString()));

      // -------------------------------------------------------
      // Feedback Border Color ("FeedbackBorderColor")
      buffer = new StringBuffer(1024);
      buffer.append("#0A3F67");
      addSnippet(new Snippet("FeedbackBorderColor", buffer.toString()));

      // -------------------------------------------------------
      // Feedback Border Color ("FeedbackBackgroundColor")
      buffer = new StringBuffer(1024);
      buffer.append("#FFFFFF");
      addSnippet(new Snippet("FeedbackBackgroundColor", buffer.toString()));

      // -------------------------------------------------------
      // Feedback Text Color ("FeedbackTextColor")
      buffer = new StringBuffer(1024);
      buffer.append("#0A3F67");
      addSnippet(new Snippet("FeedbackTextColor", buffer.toString()));

      // -------------------------------------------------------
      // Formular Begin ("FormBegin")
      buffer = new StringBuffer(1024);
      if (m_DocInfo.aTestingRadioButtons != null)
         buffer.append("<form name=\"TestingForm\" action=\"\">"+ END_LN);
      addSnippet(new Snippet("FormBegin", buffer.toString()));

      // -------------------------------------------------------
      // Formular Begin ("FormEnd")
      buffer = new StringBuffer(1024);
      if (m_DocInfo.aTestingRadioButtons != null)
         buffer.append("</form>"+ END_LN);
      addSnippet(new Snippet("FormEnd", buffer.toString()));

      // -------------------------------------------------------
      // Slides & Clips HTML and CSS code ("Slides", "SlidesCss")
      buffer = new StringBuffer(1024);
      StringBuffer buffer2 = new StringBuffer(1024);
      // Init Image
      buffer.append("<div id=\"MessageLayer\">" + END_LN);
      buffer.append("  <img name=\"InitImage\"" + END_LN);
      buffer.append("    src=\"images/initialize.png\"" + END_LN);
      buffer.append("    width=\"180\"" + END_LN);
      buffer.append("    height=\"75\"" + END_LN);
      buffer.append("    border=\"0\"" + END_LN);
      buffer.append("    alt=\"\"><br>" + END_LN);
      buffer.append("    <div style=\"position:relative;left:60px;top:-35px;font-family:Arial,Helvetica,sans-serif;font-size:14px;\">" + INITIALIZE + "</div>" + END_LN);
      buffer.append("</div>" + END_LN);
      // CSS for MessageLayer (init image)
      buffer2.append("#MessageLayer{position:absolute;left:194;top:184;z-index:" + (m_nMaxLayerIndex + 10000) + ";visibility:visible;}" + END_LN);

      // First slide DIVs
      if (m_DocInfo.aSlideEntries != null) // Denver documents have no slides!
      {
         for (int i = 0; i <= m_nMaxLayerIndex; ++i)
         {
            int idx = 0;
            for (int k = 0; k < m_DocInfo.aSlideEntries.length; ++k)
            {
               idx = k;
               if (m_DocInfo.aSlideEntries[k].nLayerIndex == i)
                  break;
            }
   
            SlideEntry se = m_DocInfo.aSlideEntries[idx];
            buffer.append("<div id=\"SlideareaLayer" + i + "\">" + END_LN);
            buffer.append("  <img name=\"Slidefield" + i + "\"" + END_LN);
            buffer.append("       src=\"slides/" + se.strImageName + ".png\"" + END_LN);
            buffer.append("       width=\"" + se.nWidth + "\"" + END_LN);
            buffer.append("       height=\"" + se.nHeight + "\"" + END_LN);
            buffer.append("       border=\"0\"" + END_LN);
            buffer.append("       alt=\"\">" + END_LN);
            buffer.append("</div>" + END_LN + END_LN);
   
            String sVisible = "hidden";
            if (idx == 0)
               sVisible = "visible";
   
            // CSS
            buffer2.append("#SlideareaLayer" + i  + "{position:absolute;left:" + se.nX + ";top:" + se.nY + ";z-index:" + (i+2) + ";visibility:" + sVisible + ";}" + END_LN);
         }
      }

      // -------------------------------------------------------
      // The Telepointer
      // The original Telepointer has a size of 30x30 
      // Consider scale factor
      long size = Math.round(dScaling * 30.0); 
      buffer.append("<div id=\"TelepointerLayer\">" + END_LN);
      buffer.append("  <img name=\"Telepointer24\" " + END_LN);
      buffer.append("       src=\"images/telepointer24.gif\" " + END_LN);
      buffer.append("       width=\"" + size + "\" " + END_LN);
      buffer.append("       height=\"" + size + "\" " + END_LN);
      buffer.append("       border=\"0\" " + END_LN);
      buffer.append("       alt=\"/\">" + END_LN);
      buffer.append("</div>" + END_LN + END_LN);
      buffer2.append("#TelepointerLayer{position:absolute;left:0;top:0;z-index:" + (m_nMaxLayerIndex + 2) + ";visibility:hidden;}" + END_LN);

      // -------------------------------------------------------
      // Then the Clips DIVs. Perhaps.
      if (bHasClips)
      {
         for (int i = 0; i < iClipCount; ++i)
         {
            VideoClipInfo vci = aClipInfos[i];
            if (bIsWmExport)
            {
               // WM clips
               buffer.append("<script type=\"text/javascript\">" + END_LN);
               buffer.append("<!--" + END_LN);
               buffer.append("if (isFirefox && isFirefoxWmPluginInstalled)" + END_LN);
               buffer.append("{" + END_LN);
               buffer.append("   // Use the WM Player 7 Object model" + END_LN);
               buffer.append("   document.writeln('<div id=\"ClipLayer" + i + "\" style=\"padding:0px; margin:0px; border:1px solid black;\">');" + END_LN);
               buffer.append("   document.writeln('<embed ID=\"ClipPlayer" + i + "\" ');" + END_LN);
               buffer.append("   document.writeln('   width=\"" + vci.videoWidth + "\" ');" + END_LN);
               buffer.append("   document.writeln('   height=\"" + vci.videoHeight + "\" ');" + END_LN);
               buffer.append("   document.writeln('   type=\"application/x-ms-wmp\" ');" + END_LN);
               buffer.append("   document.writeln('   src=\"" + getWmtTarget(true, i) + "\" ');" + END_LN);
               buffer.append("   document.writeln('   uimode=\"none\" ');" + END_LN);
               if (!bPluginContextMenuEnabled)
                  buffer.append("   document.writeln('   enableContextMenu=\"0\" ');" + END_LN);
               buffer.append("   document.writeln('   autoStart=\"0\">');" + END_LN);
               buffer.append("   document.writeln('</div>')" + END_LN);
               buffer.append("}" + END_LN);
               buffer.append("else" + END_LN);
               buffer.append("{" + END_LN);
//               buffer.append("   document.writeln('   <div id=\"ClipLayer" + i + "\">');" + END_LN);
               // border around the clips (with css) - does not work with NS4
               buffer.append("   document.writeln('<div id=\"ClipLayer" + i + "\" style=\"padding:0px; margin:0px; border:1px solid black;\">');" + END_LN);
               buffer.append("   document.writeln('   <object id=\"ClipPlayer" + i + "\" width=" + vci.videoWidth + " height=" + vci.videoHeight + "');" + END_LN);
               buffer.append("   document.writeln('      classid=\"CLSID:6BF52A52-394A-11d3-B153-00C04F79FAA6\"');" + END_LN);
               buffer.append("   document.writeln('      standby=\"" + checkHtmlString(STANDBY_TEXT) + "\"');" + END_LN);
               buffer.append("   document.writeln('      type=\"application/x-oleobject\">');" + END_LN);
               buffer.append("   document.writeln('      <param name=\"URL\" value=\"" + getWmtTarget(true, i) + "\">');" + END_LN);
               buffer.append("   document.writeln('      <param name=\"uiMode\" value=\"none\">');" + END_LN);
               if (!bPluginContextMenuEnabled)
                  buffer.append("   document.writeln('      <param name=\"enableContextMenu\" value=\"0\">');" + END_LN);
               buffer.append("   document.writeln('      <param name=\"AutoStart\" value=\"0\">');" + END_LN);
               buffer.append("   document.writeln('      <embed type=\"application/x-mplayer2\" ');" + END_LN);
               buffer.append("   document.writeln('         pluginspage=\"http://www.microsoft.com/windows/windowsmedia/download/plugin.asp\" ');" + END_LN);
               buffer.append("   document.writeln('         src=\"" + getWmtTarget(true, i) + "\" ');" + END_LN);
               buffer.append("   document.writeln('         name=\"ClipPlayer" + i + "\" ');" + END_LN);
               buffer.append("   document.writeln('         width=\"" + vci.videoWidth + "\" ');" + END_LN);
               buffer.append("   document.writeln('         height=\"" + vci.videoHeight + "\" ');" + END_LN);
               buffer.append("   document.writeln('         ShowControls=\"0\" ');" + END_LN);
               buffer.append("   document.writeln('         ShowPositionControls=\"0\" ');" + END_LN);
               buffer.append("   document.writeln('         ShowStatusBar=\"0\" ');" + END_LN);
               buffer.append("   document.writeln('         ShowCaptioning=\"0\" ');" + END_LN);
               buffer.append("   document.writeln('         AutoSize=\"0\" ');" + END_LN);
               buffer.append("   document.writeln('         AutoRewind=\"-1\" ');" + END_LN);
               buffer.append("   document.writeln('         AutoStart=\"0\" ');" + END_LN);
               buffer.append("   document.writeln('         ClickToPlay=\"0\" ');" + END_LN);
               buffer.append("   document.writeln('         WindowlessVideo=\"-1\" ');" + END_LN);
               if (!bPluginContextMenuEnabled)
                  buffer.append("   document.writeln('         EnableContextMenu=\"0\" ');" + END_LN);
               buffer.append("   document.writeln('         TransparentAtStart=\"0\">');" + END_LN);
               buffer.append("   document.writeln('      </embed>');" + END_LN);
               buffer.append("   document.writeln('   </object>');" + END_LN);
               buffer.append("   document.writeln('</div>')" + END_LN);
               buffer.append("}" + END_LN);
               buffer.append("// -->" + END_LN);
               buffer.append("</script>" + END_LN + END_LN);
            }
            else if (bIsRealExport)
            {
               // RM clips
//               buffer.append("<div id=\"ClipLayer" + i + "\"> " + END_LN);
               // border around the clips (with css) - does not work with NS4
               buffer.append("<div id=\"ClipLayer" + i + "\" style=\"padding:0px; margin:0px; border:1px solid black;\"> " + END_LN);
               buffer.append("  <embed name=\"ClipImageWindow" + i + "\" " + END_LN);
               buffer.append("         width=\"" + vci.videoWidth + "\" " + END_LN);
               buffer.append("         height=\"" + vci.videoHeight + "\" " + END_LN);
               buffer.append("         type=\"audio/x-pn-realaudio-plugin\" " + END_LN);
               buffer.append("         region=\"clip" + i + "_region\" " + END_LN);
               buffer.append("         console=\"one\" " + END_LN);
               buffer.append("         controls=\"ImageWindow\" " + END_LN);
               buffer.append("         autostart=\"false\" " + END_LN);
               buffer.append("         nojava=\"false\" " + END_LN);
               buffer.append("         nologo=\"true\" " + END_LN);
               buffer.append("         src=\"" + getRealTarget(true, i, false) + "\" " + END_LN);
               buffer.append("         alt=\"\"></div> " + END_LN);
            }

            // CSS Code
            buffer2.append("#ClipLayer" + i + "{position:absolute;left:0;top:0;z-index:" + (m_nMaxLayerIndex + 5) + ";visibility:visible;}" + END_LN);
         }
      }
      addSnippet(new Snippet("Slides", buffer.toString()));

      // Moveable Pictures (perhaps)
      if (m_DocInfo.aTestingMoveablePictures != null)
      {
         for (int i = 0; i < m_DocInfo.aTestingMoveablePictures.length; ++i)
         {
            Picture pic = m_DocInfo.aTestingMoveablePictures[i];
            int lastDot = pic.GetFileName().lastIndexOf('.');
            String strId = pic.GetFileName().substring(0, lastDot) + "_" + i;
            String strLayer = "#" + strId + "Layer";
            int left = pic.getClip(m_dScaling).x;
            int top = pic.getClip(m_dScaling).y;
            buffer2.append(strLayer + "{position:absolute;left:" + left + ";top:" + top + ";z-index:" + (m_nMaxLayerIndex + 10000) + ";visibility:hidden;}" + END_LN);
         }
      }
      
      // Radio Dynamics (perhaps)
      if (m_DocInfo.aTestingRadioButtons != null)
      {
         int idx = 0;
         int nPageBefore = 0;
         for (int i = 0; i < m_DocInfo.aTestingRadioButtons.length; ++i)
         {
            RadioDynamic rdb = m_DocInfo.aTestingRadioButtons[i];
            int nPage = getPageFromActivityTimes(rdb.GetActivityTimes());
            if (nPage != nPageBefore)
               idx = 0;
         	String strLayer = "#Radio" + nPage + "_" + idx + "Layer";
            int left = rdb.getClip(m_dScaling).x;
            int top = rdb.getClip(m_dScaling).y;
            buffer2.append(strLayer + "{position:absolute;left:" + left + ";top:" + top + ";z-index:" + (m_nMaxLayerIndex + 10000) + ";visibility:hidden;}" + END_LN);
            nPageBefore = nPage;
            idx++;
         }
      }
      
      addSnippet(new Snippet("SlidesCss", buffer2.toString()));

      // -------------------------------------------------------
      // Netscape 4 style dropdown list ("Ns4Select")
      buffer = new StringBuffer(1024);
      buffer.append("<form name=\"Form1\">" + END_LN);
      buffer.append("  <span style=\"font-family:Arial,Helvetica,sans-serif; font-weight:bold; font-size:11px;\">" + SLIDES_OVERVIEW + ":</span><br>" + END_LN);
      buffer.append("  <select name=\"slideoption\" width=\"296\" onChange=\"javascript:control_seekTime(this.form.slideoption.options[this.form.slideoption.selectedIndex].value);\">" + END_LN);
      if (m_DocInfo.aThumbnailEntries != null) // Denver documents have no thumbnails!
      {
         for (int i = 0; i < m_DocInfo.aThumbnailEntries.length; ++i)
         {
            ThumbnailEntry te = m_DocInfo.aThumbnailEntries[i];
            String strCaptionShort = truncateTo(te.strCaption, 28);
            // Check for HTML Syntax
            strCaptionShort = checkHtmlString(strCaptionShort);

            buffer.append("    <option value=\"" + te.lBeginTimestamp + "\">" + msToTime(te.lBeginTimestamp) + " " + strCaptionShort + "</option>" + END_LN);
         }
      }
      buffer.append("  </select>" + END_LN);
      buffer.append("</form>" + END_LN);
      addSnippet(new Snippet("Ns4Select", buffer.toString()));

      // -------------------------------------------------------
      // The MasterPlayer snippet ("MasterPlayer")
      buffer = new StringBuffer(1024);
      int playerWidth  = 300;
      int playerHeight = 0;

///      if ((m_StreamingData.hasVideoFile && m_StreamingData.containsNormalVideo) || m_StreamingData.isSgStandAloneMode)
      if (bHasVideo)
      {
         playerWidth  = dimVideo.width;
         playerHeight = dimVideo.height;
      }
      else if (bShowStillImage)
      {
         playerWidth  = dimStillimage.width >= 300 ? dimStillimage.width : 300;
         playerHeight = 0;
      }

      if (bIsWmExport)
      {
         buffer.append("<iframe name=\"masterPlayerFrame\" src=\"__masterplayer_wm.html\" width=\"" + playerWidth + "\" height=\"" + (playerHeight + 19) + "\" scrolling=\"no\" marginheight=\"0\" marginwidth=\"0\" frameborder=\"0\">" + END_LN);
         buffer.append("</iframe>" + END_LN);
      }
      else if (bIsRealExport)
      {
         // RM video
         int statusbarWidth = playerWidth;
         if (bHasVideo && playerWidth < 300)
            statusbarWidth = 300;

         buffer.append("<embed name=\"PlayerImageWindow\" " + END_LN);
         buffer.append("       width=\"" + playerWidth + "\" " + END_LN);
         buffer.append("       height=\"" + (playerHeight > 0 ? playerHeight : 1) + "\" " + END_LN);
         buffer.append("       type=\"audio/x-pn-realaudio-plugin\" " + END_LN);
         if (bHasClips && !bIsDenver)
            buffer.append("       region=\"video_region\"" + END_LN);
         buffer.append("       console=\"one\" " + END_LN);
         buffer.append("       controls=\"ImageWindow\" " + END_LN);
         buffer.append("       autostart=\"false\" " + END_LN);
         buffer.append("       nojava=\"false\" " + END_LN);
         buffer.append("       src=\"" + getRealTarget(false, 0, false) + "\" " + END_LN);
         buffer.append("       alt=\"\"><br>" + END_LN);
         buffer.append("<embed name=\"PlayerStatusBar\" " + END_LN);
         buffer.append("       width=\"" + statusbarWidth + "\" " + END_LN);
         buffer.append("       height=\"24\" " + END_LN);
         buffer.append("       type=\"audio/x-pn-realaudio-plugin\" " + END_LN);
         buffer.append("       console=\"one\" " + END_LN);
         buffer.append("       controls=\"StatusBar\" " + END_LN);
         buffer.append("       autostart=\"false\" " + END_LN);
         buffer.append("       nojava=\"false\" " + END_LN);
         buffer.append("       alt=\"\">" + END_LN);
      }
      addSnippet(new Snippet("MasterPlayer", buffer.toString()));

      // -------------------------------------------------------
      // The MasterPlayer for Windows Media snippet ("MasterPlayerWm")
      buffer = new StringBuffer(1024);
      if (bIsWmExport)
      {
         // WM video
         buffer.append("<script type=\"text/javascript\">" + END_LN);
         buffer.append("<!--" + END_LN);
         buffer.append("if (isFirefox && isFirefoxWmPluginInstalled)" + END_LN);
         buffer.append("{" + END_LN);
         buffer.append("   // Use the WM Player 7 Object model" + END_LN);
         buffer.append("   document.writeln('<embed ID=\"MasterPlayer\" ');" + END_LN);
         buffer.append("   document.writeln('   width=\"" + playerWidth + "\" ');" + END_LN);
         buffer.append("   document.writeln('   height=\"" + (playerHeight + 64) + "\" ');" + END_LN);
         buffer.append("   document.writeln('   type=\"application/x-ms-wmp\" ');" + END_LN);
         buffer.append("   document.writeln('   src=\"" + getWmtTarget(false, 0) + "\" ');" + END_LN);
         buffer.append("   document.writeln('   uimode=\"mini\" ');" + END_LN);
         if (!bPluginContextMenuEnabled)
            buffer.append("   document.writeln('   enableContextMenu=\"0\" ');" + END_LN);
         buffer.append("   document.writeln('   autoStart=\"0\">');" + END_LN);
         buffer.append("}" + END_LN);
         buffer.append("else" + END_LN);
         buffer.append("{" + END_LN);
         buffer.append("   document.writeln('<object ID=\"MasterPlayer\" ');" + END_LN);
         buffer.append("   document.writeln('   width=\"" + playerWidth + "\" ');" + END_LN);
         buffer.append("   document.writeln('   height=\"" + (playerHeight + 64) + "\" ');" + END_LN);
         buffer.append("   document.writeln('   classid=\"CLSID:6BF52A52-394A-11d3-B153-00C04F79FAA6\" ');" + END_LN);
         buffer.append("   document.writeln('   standby=\"" + checkHtmlString(STANDBY_TEXT) + "\" ');" + END_LN);
         buffer.append("   document.writeln('   type=\"application/x-oleobject\"> ');" + END_LN);
         buffer.append("   document.writeln('   <param name=\"URL\" value=\"" + getWmtTarget(false, 0) + "\"> ');" + END_LN);
         buffer.append("   document.writeln('   <param name=\"uiMode\" value=\"mini\"> ');" + END_LN);
         if (!bPluginContextMenuEnabled)
            buffer.append("   document.writeln('   <param name=\"enableContextMenu\" value=\"0\"> ');" + END_LN);
         buffer.append("   document.writeln('   <param name=\"AutoStart\" value=\"0\"> ');" + END_LN);
         buffer.append("   document.writeln('   <embed type=\"application/x-mplayer2\" ');" + END_LN);
         buffer.append("   document.writeln('      pluginspage=\"http://www.microsoft.com/windows/windowsmedia/download/plugin.asp\" ');" + END_LN);
         buffer.append("   document.writeln('      src=\"" + getWmtTarget(false, 0) + "\" ');" + END_LN);
         buffer.append("   document.writeln('      name=\"MasterPlayer\" ');" + END_LN);
         buffer.append("   document.writeln('      width=\"" + playerWidth + "\" ');" + END_LN);
         buffer.append("   document.writeln('      height=\"" + (playerHeight + 24) + "\" ');" + END_LN);
         buffer.append("   document.writeln('      ShowControls=\"0\" ');" + END_LN);
         buffer.append("   document.writeln('      ShowPositionControls=\"0\" ');" + END_LN);
         buffer.append("   document.writeln('      ShowStatusBar=\"-1\" ');" + END_LN);
         buffer.append("   document.writeln('      ShowCaptioning=\"0\" ');" + END_LN);
         buffer.append("   document.writeln('      AutoSize=\"0\" ');" + END_LN);
         buffer.append("   document.writeln('      AutoRewind=\"-1\" ');" + END_LN);
         buffer.append("   document.writeln('      AutoStart=\"0\" ');" + END_LN);
         buffer.append("   document.writeln('      ClickToPlay=\"0\" ');" + END_LN);
         buffer.append("   document.writeln('      WindowlessVideo=\"-1\" ');" + END_LN);
         if (!bPluginContextMenuEnabled)
            buffer.append("   document.writeln('      EnableContextMenu=\"0\" ');" + END_LN);
         buffer.append("   document.writeln('      TransparentAtStart=\"0\"> ');" + END_LN);
//         buffer.append("   document.writeln('   </embed> ');" + END_LN); // the </embed> tag can be omitted
         buffer.append("   document.writeln('</object>');" + END_LN);
         buffer.append("}" + END_LN);
         buffer.append("// -->" + END_LN);
         buffer.append("</script>" + END_LN + END_LN);
      }
      addSnippet(new Snippet("MasterPlayerWm", buffer.toString()));

      // -------------------------------------------------------
      // SMIL file, if needed ("SmilData")
      buffer = new StringBuffer(1024);
      if (bHasClips && bIsRealExport && !bIsDenver)
      {
         buffer.append("<smil>" + END_LN);
         buffer.append("" + END_LN);
         buffer.append("  <head>" + END_LN);
         buffer.append("    <meta name=\"author\" content=\"" + strAuthor + "\" />" + END_LN);
         buffer.append("    <meta name=\"title\" content=\"" + strTitle + "\" />" + END_LN);
         buffer.append("    <preroll=\"10s\" />" + END_LN);
         buffer.append("    <layout>" + END_LN);
         buffer.append("      <root-layout />" + END_LN);
         buffer.append("    </layout>" + END_LN);
         buffer.append("  </head>" + END_LN);
         buffer.append("" + END_LN);
         buffer.append("  <body>" + END_LN);
         buffer.append("    <par>" + END_LN);
         buffer.append("      <seq>" + END_LN);
         buffer.append("        <par id=\"presentation\">" + END_LN);
         buffer.append("          <video src=\"" + getRealTarget(false, 0, true) + "\" region=\"video_region\" fill=\"freeze\" begin=\"0ms\" />" + END_LN);
         for (int i = 0; i < aClipInfos.length; ++i)
         {
            VideoClipInfo vci = aClipInfos[i];
            buffer.append("          <video src=\"" + getRealTarget(true, i, true) + "\" region=\"clip" + i + "_region\" fill=\"remove\" begin=\"" + vci.startTimeMs + "ms\" />" + END_LN);
         }
         buffer.append("        </par>" + END_LN);
         buffer.append("      </seq>" + END_LN);
         buffer.append("    </par>" + END_LN);
         buffer.append("  </body>" + END_LN);
         buffer.append("" + END_LN);
         buffer.append("</smil>" + END_LN);
         
      }
      addSnippet(new Snippet("SmilData", buffer.toString()));
   }

   /**
    * forClip == false ==> For MasterPlayer, then clipNumber does not mattes
    */
   private String getRealTarget(boolean forClip, int clipNumber, boolean ignoreSmi)
   {
      // double code (sort of) below
      
      DocumentData dd = m_pwData.GetDocumentData();
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      boolean bHasClipsSeparated = dd.HasClips() &&
         ps.GetBoolValue("bShowClipsOnSlides");
      boolean bReplayLocal = (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL);
      boolean bReplayHttp= ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                            && (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_WEB_SERVER) );   
      boolean bReplayServer = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                               && (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_STREAMING_SERVER) );
      
      boolean bIsDenver = dd.IsDenver();
      String strTargetFilename = ps.GetStringValue("strTargetPathlessDocName");
      String strHttpUrl = ps.GetStringValue("strTargetUrlHttpServer");
      String strStreamingUrl = ps.GetStringValue("strTargetUrlStreamingServer");
      
      boolean bWriteSmilFile = bHasClipsSeparated && !bIsDenver && !ignoreSmi;
      
      String strFileAndSuffix = strTargetFilename + ".rm";
      if (bWriteSmilFile)
         strFileAndSuffix = strTargetFilename + ".smi";
      else if (forClip)
         strFileAndSuffix = strTargetFilename + "_clip" + clipNumber + ".rm";
      
      String strPrefix = "";
      if (bReplayHttp || bReplayServer)
      {
         if (bReplayHttp)
            strPrefix = strHttpUrl;
         else
            strPrefix = strStreamingUrl;
         
         if (!strPrefix.endsWith("/"))
            strPrefix += "/";

         if (strPrefix.equals("/"))
            strPrefix = "";
      }
      
      return strPrefix + strFileAndSuffix;
      
      /*
      String addSlash = "";

      if (bReplayLocal)
      {
         return strFileAndSuffix;

      }
      else if (bReplayHttp)
      {
         if (!tpc.strHttpTargetUrl.endsWith("/"))
            addSlash = "/";
         
         return tpc.strHttpTargetUrl + addSlash + strFileAndSuffix;

      }
      else if (bReplayServer)
      {
         if (!tpc.strStreamingTargetUrl.endsWith("/"))
               addSlash = "/";
            
         return tpc.strStreamingTargetUrl + addSlash + strFileAndSuffix;

      }

      return "";
      */
   }

   /**
    * forClip == false ==> For MasterPlayer, then clipNumber does not matter
    */
   private String getWmtTarget(boolean forClip, int clipNumber)
   {  
      // double code (sort of) above
      
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      DocumentData dd = m_pwData.GetDocumentData();
      
      boolean bHasClipsOnlyAndSeparated = dd.HasClips() && !dd.HasNormalVideo() &&
         ps.GetBoolValue("bShowClipsOnSlides");
      boolean bUseAdvancedWmaSuffix = bHasClipsOnlyAndSeparated && ps.GetBoolValue("bWmUseAdvancedSuffix");
      boolean bReplayLocal = ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL;
      boolean bReplayHttp= ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                            && (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_WEB_SERVER) );   
      boolean bReplayServer = ( (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_SERVER) 
                               && (ps.GetIntValue("iServerType") == PublisherWizardData.SRV_STREAMING_SERVER) );
      
      String strTargetFilename = ps.GetStringValue("strTargetPathlessDocName");
      String strWmSuffix = 
         ps.GetBoolValue("bWmUseAdvancedSuffix") ? m_pwData.m_strWmSuffix : ".wm";
      
      String strHttpUrl = ps.GetStringValue("strTargetUrlHttpServer");
      String strStreamingUrl = ps.GetStringValue("strTargetUrlStreamingServer");
      
      String strFileAndSuffix = strTargetFilename + strWmSuffix;
      if (forClip)
         strFileAndSuffix = strTargetFilename + "_clip" + clipNumber + strWmSuffix;
      else if (bUseAdvancedWmaSuffix)
         strFileAndSuffix = strTargetFilename + ".wma";
      
      String strPrefix = "";
      if (bReplayHttp || bReplayServer)
      {
         if (bReplayHttp)
            strPrefix = strHttpUrl;
         else
            strPrefix = strStreamingUrl;
         
         if (!strPrefix.endsWith("/"))
            strPrefix += "/";

         if (strPrefix.equals("/"))
            strPrefix = "";
      }
      
      return strPrefix + strFileAndSuffix;
      
      /*
      String addSlash = "";


      if (bReplayLocal)
      {
         return strFileAndSuffix;

      }
      else if (bReplayHttp)
      {
         if (!tpc.strHttpTargetUrl.endsWith("/"))
            addSlash = "/";
         
         return tpc.strHttpTargetUrl + addSlash + strFileAndSuffix;

      }
      else if (bReplayServer)
      {
         if (!tpc.strStreamingTargetUrl.endsWith("/"))
            addSlash = "/";
         
         return tpc.strStreamingTargetUrl + addSlash + strFileAndSuffix;

      }

      return "";
      */
   }
   

   // Reinterpretation of the the action commands for use in the Web Templates
   // The second parameter @int nPage is for Testing only
   private String reinterpretActionStringForWebTemplates(String strAction, int nPage)
   {
      DocumentData dd = m_pwData.GetDocumentData();
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      String strInputPath = dd.GetPresentationPath();
      String strTargetPath = ps.GetStringValue("strTargetDirHttpServer");
      if (ps.GetIntValue("iReplayType") == PublisherWizardData.REPLAY_LOCAL)
         strTargetPath = ps.GetPathValue("strTargetDirLocal");
      
      if (!strInputPath.endsWith(File.separator))
         strInputPath += File.separator;
      if (!strTargetPath.endsWith(File.separator))
         strTargetPath += File.separator;

      
      String strNewWebAction = "";
      
      if (strAction == null)
         strAction = new String("");
      
      if (strAction.indexOf("start") == 0)
      {
         strNewWebAction = "layout_onPlay()";
      }
      if (strAction.indexOf("stop") == 0)
      {
         strNewWebAction = "layout_onPause()";
      }
      if (strAction.indexOf("close") == 0)
      {
         strNewWebAction = "layout_onClose()";
      }
      if (strAction.indexOf("jump") == 0)
      {
         if (strAction.indexOf("jump next") == 0)
         {
            String strArgument = (strAction.length() >= 9) ? strAction.substring(9) : "";
            String str2ndCommand = get2ndCommandFromActionArgument(strArgument, false);
            strNewWebAction = "layout_onForward(" + str2ndCommand + ")";
         }
         else if (strAction.indexOf("jump prev") == 0)
         {
            String strArgument = (strAction.length() >= 9) ? strAction.substring(9) : "";
            String str2ndCommand = get2ndCommandFromActionArgument(strArgument, false);
            strNewWebAction = "layout_onBack(" + str2ndCommand + ")";
         }
         else if (strAction.indexOf("jump page") == 0)
         {
            String strArgument = (strAction.length() >= 9) ? strAction.substring(9) : "";
            String str2ndCommand = get2ndCommandFromActionArgument(strArgument, true);
            int pageNumber = getIntegerFromActionArgument(strArgument);
            strNewWebAction = "layout_onGotoSlide(" + pageNumber + str2ndCommand + ")";
            // pageNumber is a timestamp
            strNewWebAction = "layout_onGotoTimestamp(" + pageNumber + str2ndCommand + ")";
         }
         else if (strAction.indexOf("jump mark") == 0)
         {
            String strArgument = (strAction.length() >= 9) ? strAction.substring(9) : "";
            String str2ndCommand = get2ndCommandFromActionArgument(strArgument, true);
            int markNumber = getIntegerFromActionArgument(strArgument);
            //strNewWebAction = "layout_onGotoTargetmark(" + markNumber + str2ndCommand + ")";
            // markNumber is a timestamp
            strNewWebAction = "layout_onGotoTimestamp(" + markNumber + str2ndCommand + ")";
         }
         else if (strAction.indexOf("jump question") == 0)
         {
        	 String strArgument = (strAction.length() >= 13) ? strAction.substring(13) : "";
        	 String str2ndCommand = get2ndCommandFromActionArgument(strArgument, true);
        	 //strNewWebAction = "layout_onGotoTimestampR(g_JumpRandomTimestamp" + str2ndCommand + ")";
        	 strNewWebAction = "layout_onGotoQuestion()";
         }
         else
         {
            String strArgument = (strAction.length() >= 4) ? strAction.substring(4) : "";
            String str2ndCommand = get2ndCommandFromActionArgument(strArgument, true);
            int timeMs = getIntegerFromActionArgument(strArgument);
            strNewWebAction = "layout_onGotoTimestamp(" + timeMs + str2ndCommand + ")";
         }
      }
      if (strAction.indexOf("audio on") == 0)
      {
         String strArgument = (strAction.length() >= 8) ? strAction.substring(8) : "";
         String str2ndCommand = get2ndCommandFromActionArgument(strArgument, false);
         strNewWebAction = "layout_onMuteOff(" + str2ndCommand + ")";
      }
      if (strAction.indexOf("audio off") == 0)
      {
         String strArgument = (strAction.length() >= 9) ? strAction.substring(9) : "";
         String str2ndCommand = get2ndCommandFromActionArgument(strArgument, false);
         strNewWebAction = "layout_onMuteOn(" + str2ndCommand + ")";
      }
      if (strAction.indexOf("open-url") == 0)
      {
         String strArgument = (strAction.length() >= 8) ? strAction.substring(9) : "";
         String str2ndCommand = get2ndCommandFromActionArgument(strArgument, true);
         int nPosSeparator = strArgument.indexOf(';');
         if (nPosSeparator > -1)
            strArgument = strArgument.substring(0, nPosSeparator);
         strArgument = getSlashSubstitutedString(strArgument);
         
         // Copy file to target directory (if possible)
         String strSource = strInputPath + strArgument;
         String strTarget = strTargetPath + strArgument;
         try
         {
            FileUtils.copyFile(strSource, strTarget, null, "Please wait");
            System.out.println(strArgument + " copied.");
         }
         catch (IOException e)
         {
         }
         
         strNewWebAction = "layout_onOpenUrl(\"" + strArgument + "\"" + str2ndCommand + ")";
      }
      if (strAction.indexOf("open-file") == 0)
      {
         String strArgument = (strAction.length() >= 9) ? strAction.substring(10) : "";
         String str2ndCommand = get2ndCommandFromActionArgument(strArgument, true);
         int nPosSeparator = strArgument.indexOf(';');
         if (nPosSeparator > -1)
            strArgument = strArgument.substring(0, nPosSeparator);
         strArgument = getSlashSubstitutedString(strArgument);

         // Copy file to target directory (if possible)
         String strTempPath = LPLibs.templateGetTargetDirectory();
         String strSource = strInputPath + strArgument;
         String strTarget = strTempPath + strArgument;
         try
         {
            FileUtils.copyFile(strSource, strTarget, null, "Please wait");
            System.out.println(strArgument + " copied.");
            // Remember SCORM
            LPLibs.scormAddResourceFile(strTempPath + strArgument, strArgument);
            strNewWebAction = "layout_onOpenFile(\"" + strArgument + "\"" + str2ndCommand + ")";
         }
         catch (IOException e)
         {
            // File can not be copied.
            // Assuming the file is specified with absolute path: "file://" has to be added
            strNewWebAction = "layout_onOpenFile(\"file://" + strArgument + "\"" + str2ndCommand + ")";
         }
      }
      if (strAction.indexOf("check") == 0)
      {
         strNewWebAction = "layout_onSubmitAnswer(" + nPage + ")";
      }
      if ((strAction.indexOf("reset") == 0) && (strAction.indexOf("reset-all") != 0))
      {
         strNewWebAction = "layout_onResetQuestion(" + nPage + ")";
      }
      if (strAction.indexOf("finish") == 0) // not used
      {
         strNewWebAction = "layout_onSubmitTesting(" + nPage + ")";
      }
      if (strAction.indexOf("reset-all") == 0) // not uses
      {
         strNewWebAction = "layout_onResetQuestionnaire(" + nPage + ")";
      }
      
      // Unknown command or empty string --> do nothing
      if (strNewWebAction.length() == 0)
         strNewWebAction = "layout_onDoNothing()";
      
      System.out.println("Action: " + strAction + " --> " + strNewWebAction);
      
      return strNewWebAction;
   }
   
   private String get2ndCommandFromActionArgument(String strArgument, boolean bAddSeparator)
   {
      String str2ndCommand = "";
      
      int nPosSeparator = strArgument.indexOf(';');
      if (nPosSeparator > -1)
      {
         if (bAddSeparator)
            str2ndCommand = ", ";
            
         String strFlag = strArgument.substring(nPosSeparator+1);
         if (strFlag.indexOf("start") == 0)
            str2ndCommand += "\"play\"";
         else if (strFlag.indexOf("stop") == 0)
            str2ndCommand += "\"pause\"";
      }

      return str2ndCommand;
   }

   private String getSlashSubstitutedString(String strArgument)
   {
      String strSlashSubstituted = new String();
      char[] caArguments = strArgument.toCharArray();
      for (int i = 0; i < caArguments.length; ++i)
      {
         char c = caArguments[i];
         if (c == '\\')
         {
            if (((i+1) < caArguments.length) && (caArguments[i+1] != '\\'))
               strSlashSubstituted += '/';
         }
         else
            strSlashSubstituted += c;
      }
      
      return strSlashSubstituted;
   }
   
   private int getIntegerFromActionArgument(String strArgument)
   {
      String strValue = "";
      boolean bDigitFound = false;
      int i = 0;
      
      while (i < strArgument.length())
      {
         if ((strArgument.charAt(i) > 47) && (strArgument.charAt(i) < 58))
         {
            strValue += strArgument.substring(i, i+1);
            bDigitFound = true;
         }
         else
         {
            // only one Integer per String can be found --> stop after the 1st one.
            if (bDigitFound)
               break;
         }
            
         i++;
      }
      
      int nValue = strValue.length() > 0 ? Integer.parseInt(strValue) : 0;
      return nValue;
   }
   
   private int getPageFromActivityTimes(int[] activityTimes)
   {
      int nPage = -1;
      int i = 0;
      long[] alPagingTimestamps = getPagingTimestamps();
      for (i = 0; i < alPagingTimestamps.length-1; ++i)
      {
         // So far activity times are only 1 time period 
         // --> int[] has only 2 entries: begin and end timestamp
         if (((long)activityTimes[0] >= alPagingTimestamps[i]) 
            && ((long)activityTimes[1] < alPagingTimestamps[i+1]))
         {
            nPage = i;
            break;
         }
      }
      if ((long)activityTimes[0] < alPagingTimestamps[0])
         nPage = 0;
      i = alPagingTimestamps.length-1;
      if ((long)activityTimes[0] >= alPagingTimestamps[i]) 
         nPage = i;
      // Bugfix 2651: Activity time of last question exceeds last paging (or SlideEntry resp.)
      else if ( ((long)activityTimes[0] >= alPagingTimestamps[i-1]) 
         && ((long)activityTimes[1] >= alPagingTimestamps[i]) )
         nPage = i-1;

      if (nPage < 0)
         return nPage;
      else
         return (nPage + 1);
   }
   
   private long[] getPagingTimestamps()
   {
      int i = 0;
      int idx = 0;
      long[] pagingTimes = new long[(m_DocInfo.aSlideEntries.length + 1)];
      
      for (i = 0; i < m_DocInfo.aSlideEntries.length; ++i)
      {
         if (m_DocInfo.aSlideEntries[i].bIsNewPage)
         {
            pagingTimes[idx] = m_DocInfo.aSlideEntries[i].lTimestamp;
            idx++;
         }
      }
      pagingTimes[idx] = m_lDuration;
      idx++;

      long[] alPagingTimestamps = new long[idx];
      for (i = 0; i < idx; ++i)
         alPagingTimestamps[i] = pagingTimes[i];
      
      return alPagingTimestamps;
   }
   
   private void fillTextObjects(StringBuffer buffer, Text[] aTexts, String strId)
   {
      int idx = 0;
      int nPageBefore = 0;
      for (int i = 0; i < aTexts.length; ++i)
      {
         Text txt = aTexts[i];
      	int x = txt.getClip(m_dScaling).x;
      	int y = txt.getClip(m_dScaling).y;
      	x += 2; y += 7; // empirically found
         int nPage = getPageFromActivityTimes(txt.GetVisibilityTimes());
         if (nPage != nPageBefore)
            idx = 0;
         Font fntText = txt.GetFont();
         if (fntText == null)
            fntText = new Font(null, Font.PLAIN, 12);
         String strFontFamily = fntText.getFamily();
         int nFontSize = (int)(0.5 + (m_dScaling * fntText.getSize()));
         String strFontStyle = "normal";
         if (fntText.isBold())
            strFontStyle = "bold";
         if (fntText.isItalic())
            strFontStyle = "italic";
         String strFontWeight = "normal";
         Color clrRgb = txt.GetFontColor();
         if (clrRgb == null)
            clrRgb = new Color(0x00, 0x00, 0x00);
         String strColor = Integer.toHexString(clrRgb.getRGB());
         if (strColor.length() == 8)
            // remove transparency
            strColor = strColor.substring(2);
         strColor.toUpperCase();
         strColor = "#" + strColor;
         String strText = txt.getAllContent();
         // Special case: TEXT_TYPE_CHANGE_NUMBER, e.g. 'Question 2/4'
         // '2/4' has to be generated here
         if ((txt.HasChangeableType()) && (txt.GetTextType() == Text.TEXT_TYPE_CHANGE_NUMBER))
         {
            Question question = getQuestionFromPage(nPage);
            if (question != null)
            {
               Questionnaire qq = question.GetQuestionnaire();
               int qIdx = 0;
               int nQuestions = qq.GetQuestions().length;
               for (int k = 0; k < nQuestions; ++k)
               {
                  if (qq.GetQuestions()[k] == question)
                  {
                     qIdx = k;
                     break;
                  }
               }
               strText = "" + (qIdx+1) + "/" + nQuestions;
            }
         }
         
         buffer.append("<div id=\"" + strId + nPage + "_" + idx + "Layer\" " + 
      	              "style=\"position:absolute;left:" + x + "px;top:" + y + "px;" + 
      	              "font-family:'" + strFontFamily + "';" + 
      	              "font-size:" + nFontSize + "px;" + 
      	              "font-style:" + strFontStyle + ";" + 
      	              "font-weight:" + strFontWeight + ";" + 
      	              "color:" + strColor + ";" + 
      	              "z-index:" + (m_nMaxLayerIndex + 10000) + ";visibility:hidden;\">" + 
      	              strText + 
                       "</div>" + END_LN);
         nPageBefore = nPage;
         idx++;
      }
   }
   
   private void fillFeedbackEntry(StringBuffer buffer, 
                                  int idx, Feedback feedback, int qIdx, int nFeedbackType, 
                                  String strAction, boolean bEnabled, boolean bIsQuestionnaire)
   {
      strAction = reinterpretActionStringForWebTemplates(strAction, -1);
      ArrayList alComponents = feedback.GetComponents();

      int nX = -1; 
      int nY = -1;
      int nWidth = 0; 
      int nHeight = 0;
      int nOkWidth = 0;
      int nOkHeight = 0;
      String strTextPassed = "";
      String strText = "";
      String strColorBorder = "";
      String strColorBackground = "";
      String strColorText = "";
      String strFontFamily = "";
      int nFontSizeHeader = 65536;
      int nFontSize = 65536;
      int nFontSizeTable = 65536;
      boolean bIsFontSizeHeaderSet = false;
      boolean bIsFontSizeSet = false;
      boolean bIsFontSizeTableSet = false;
      String strFontStyle = "";
      String strFontWeight = "";
      String strIsEnabled = bEnabled ? "true" : "false";

      if (!alComponents.isEmpty())
      {
         for (int i = 0; i < alComponents.size(); ++i)
         {
            VisualComponent vc = (VisualComponent)alComponents.get(i);
            // Get the Rectangle of the message box - it is the largest object. 
            Rectangle rect = vc.getClip(m_dScaling);
            if (vc instanceof OutlineRectangle)
            {
               if ((rect.width >= nWidth) && (rect.height >= nHeight))
               {
                  nX = rect.x;
                  nY = rect.y;
                  nWidth = rect.width;
                  nHeight = rect.height;
                  Color clrBorder = ((OutlineRectangle)vc).GetColor();
                  strColorBorder = Integer.toHexString(clrBorder.getRGB());
                  if (strColorBorder.length() == 8)
                     // remove transparency byte
                     strColorBorder = strColorBorder.substring(2);
                  strColorBorder.toUpperCase();
                  strColorBorder = "#" + strColorBorder;
               }
            }
            if (vc instanceof FilledRectangle)
            {
               if ((rect.width >= nWidth) && (rect.height >= nHeight))
               {
                  Color clrBackground = ((FilledRectangle)vc).GetColor();
                  strColorBackground = Integer.toHexString(clrBackground.getRGB());
                  if (strColorBackground.length() == 8)
                     // remove transparency byte
                     strColorBackground = strColorBackground.substring(2);
                  strColorBackground.toUpperCase();
                  strColorBackground = "#" + strColorBackground;
               }
            }
            if (vc instanceof ButtonArea)
            {
               // Get the button size
//               rect = ((ButtonArea)vc).GetActivityArea();
//               nOkWidth = (int)(rect.width * m_dScaling + 0.5);
//               nOkHeight = (int)(rect.height * m_dScaling + 0.5);
               rect = ((ButtonArea)vc).getClip(m_dScaling);
               nOkWidth = rect.width;
               nOkHeight = rect.height;
            }
            if (vc instanceof Text)
            {
               Text txt = (Text)vc;
               // Special case: QQ_EVAL
               if (nFeedbackType == Feedback.FEEDBACK_TYPE_QQ_EVAL)
               {
                  if (txt.GetTextType() == Text.TEXT_TYPE_EVAL_QQ_PASSED)
                     strTextPassed = txt.getAllContent();
                  if (txt.GetTextType() == Text.TEXT_TYPE_EVAL_QQ_FAILED)
                     strText = txt.getAllContent();
               }
               else
                  strText = txt.getAllContent();
               Font fntText = txt.GetFont();
               if (fntText == null)
                  fntText = new Font(null, Font.PLAIN, 12);
               int fntSize = (int)(0.5 + (m_dScaling * fntText.getSize()));
               if ( (!bIsFontSizeHeaderSet) && (fntSize < nFontSizeHeader) )
               {
                  nFontSizeHeader = fntSize;
                  nFontSize = fntSize;
                  nFontSizeTable = fntSize;
                  bIsFontSizeHeaderSet = true;
               }
               if ( (!bIsFontSizeSet) && (fntSize < nFontSize) )
               {
                  nFontSize = fntSize;
                  nFontSizeTable = fntSize;
                  bIsFontSizeSet = true;
               }
               if ( (!bIsFontSizeTableSet) && (fntSize < nFontSizeTable) )
               {
                  nFontSizeTable = fntSize;
                  bIsFontSizeHeaderSet = true;
               }
               if (strFontFamily.length() == 0)
                  strFontFamily = fntText.getFamily();
               if (strFontStyle.length() == 0)
               {
                  strFontStyle = "normal";
                  if (fntText.isBold())
                     strFontStyle = "bold";
                  if (fntText.isItalic())
                     strFontStyle = "italic";
               }
               if (strFontWeight.length() == 0)
                  strFontWeight = "normal";
               if (strColorText.length() == 0)
               {
                  Color clrRgb = txt.GetFontColor();
                  if (clrRgb == null)
                     clrRgb = new Color(0x00, 0x00, 0x00);
                  strColorText = Integer.toHexString(clrRgb.getRGB());
                  if (strColorText.length() == 8)
                     // remove transparency byte
                     strColorText = strColorText.substring(2);
                  strColorText.toUpperCase();
                  strColorText = "#" + strColorText;
               }
            }
         }
      }

      // Use default values if font size detemination fails
      if (nFontSizeHeader > 65535)
         nFontSizeHeader = 16;
      if (nFontSize > 65535)
         nFontSize = 14;
      if (nFontSizeTable > 65535)
         nFontSizeTable = 12;

      buffer.append("g_feedbackEntries[" + idx + "] = new FeedbackEntry(" + 
                    nX + ", " + nY + ", " + nWidth + ", " + nHeight + ", " + 
                    bIsQuestionnaire + ", " + qIdx + ", " + nFeedbackType + ", " + strIsEnabled + ", \"" + 
                    strTextPassed + "\", \"" + strText + "\", '" + strAction + "', \"" + 
                    strColorBorder + "\", \"" + strColorBackground + "\", \"" + 
                    strColorText + "\", \"" + 
                    strFontFamily + "\", " + nFontSizeHeader + ", " +
                    nFontSize + ", " + nFontSizeTable + ", \"" + 
                    strFontStyle + "\", \"" + strFontWeight + "\", " + 
                    nOkWidth + ", " + nOkHeight + 
                    ");" + END_LN);
   }
   
   private Question getQuestionFromPage(int nPage)
   {
      Question question = null;
      
      if (m_DocInfo.aQuestionnaires != null)
      {
         for (int i = 0; i < m_DocInfo.aQuestionnaires.length; ++i)
         {
            Questionnaire qq = m_DocInfo.aQuestionnaires[i];
            Question[] aQuestions = qq.GetQuestions();
            if (aQuestions != null && aQuestions.length > 0)
            {
               for (int k = 0; k < aQuestions.length; ++k)
               {
                  Question q = aQuestions[k];
                  int nStartMs = q.GetStartMs();
                  int nEndMs = q.GetEndMs();
                  int[] anTimesMs = {nStartMs, nEndMs};
                  int qPage = getPageFromActivityTimes(anTimesMs);
                  if (qPage == nPage)
                  {
                     question = q;
                     break;
                  }
               }
            }
         }
      }
      
      return question;
   }
   
   private String getResultFeedbackStringFromIndex(int idx)
   {
      String strFeedback = "";
      
      if (m_DocInfo.aQuestionnaires != null)
      {
         // Assuming only one questionnaire/testing
         Questionnaire qq = m_DocInfo.aQuestionnaires[0];
         if (qq != null)
         {
            // Get the Evaluation Feedback
            Feedback fb = qq.GetFeedback(Feedback.FEEDBACK_TYPE_QQ_EVAL);
            if (fb != null)
            {
               // Get the components from Evaluation Feedback
               ArrayList alComponents = fb.GetComponents();
               int idxTextObject = 0;
               int iNumOfLines = 0;
               for (int i = 0; i < alComponents.size(); ++i)
               {
                  // The required text objects are not marked in the EVQ file 
                  // --> "Hack":
                  // - Feedback Draw objects are always in a well defined order.
                  // - The text objects of the question listing are surrounded by 
                  //   horizontal lines and can be ignored.
                  // - Changeable texts can also be ignored.
                  VisualComponent vc = (VisualComponent)alComponents.get(i);

                  if (vc instanceof Line)
                     iNumOfLines++;

                  if ( (vc instanceof Text) && (iNumOfLines != 1) )
                  {
                     Text txt = (Text)vc;
                     if (!txt.HasChangeableType())
                     {
                        idxTextObject++;

                        if (idx == idxTextObject)
                        {
                           strFeedback = txt.getAllContent();
                           break;
                        }
                     }
                  }
               }
            }
         }
      }

      return strFeedback;
   }
   
   private StringBuffer fillBufferForInteraction(StringBuffer buffer, 
                                                 String strId, 
                                                 String strTitle, 
                                                 String strDoEnable, 
                                                 String strActionMouseClick, 
                                                 String strActionMouseEnter, 
                                                 String strActionMouseLeave, 
                                                 String strActionMouseDown, 
                                                 String strActionMouseUp, 
                                                 String strImgDir, 
                                                 String strImageId)
   {
      String g_imgDir = "images/";
      String imgDir = (strImgDir.length() > 0) ? strImgDir : g_imgDir;
      //buffer.append("<script type=\"text/javascript\" language=\"JavaScript\">" + END_LN);

      buffer.append("  var g_" + strId + "_title = \"" + strTitle + "\";" + END_LN);
      buffer.append("  var g_" + strId + "_isEnabled = " + strDoEnable + ";" + END_LN);
      buffer.append("  var g_" + strId + "_state = 0;" + END_LN);
      buffer.append("  var g_" + strId + "0_imgNormal = new Image();" + END_LN);
      if (strImageId.length() > 0)
         buffer.append("  g_" + strId + "0_imgNormal.src = \"" + imgDir + strImageId + "\" + strNormal;" + END_LN);
      else
         buffer.append("  g_" + strId + "0_imgNormal.src = \"" + g_imgDir + "trans.gif" + "\";" + END_LN);
      buffer.append("  var g_" + strId + "0_imgMouseOver = new Image();" + END_LN);
      if (strImageId.length() > 0)
         buffer.append("  g_" + strId + "0_imgMouseOver.src = \"" + imgDir + strImageId + "\" + strOver;" + END_LN);
      else
         buffer.append("  g_" + strId + "0_imgMouseOver.src = \"" + g_imgDir + "trans.gif" + "\";" + END_LN);
      buffer.append("  var g_" + strId + "0_imgMouseDown = new Image();" + END_LN);
      if (strImageId.length() > 0)
         buffer.append("  g_" + strId + "0_imgMouseDown.src = \"" + imgDir + strImageId + "\" + strPressed;" + END_LN);
      else
         buffer.append("  g_" + strId + "0_imgMouseDown.src = \"" + g_imgDir + "trans.gif" + "\";" + END_LN);
      buffer.append("  var g_" + strId + "0_imgDisabled = new Image();" + END_LN);
      if (strImageId.length() > 0)
         buffer.append("  g_" + strId + "0_imgDisabled.src = \"" + imgDir + strImageId + "\" + strInactive;" + END_LN);
      else
         buffer.append("  g_" + strId + "0_imgDisabled.src = \"" + g_imgDir + "trans.gif" + "\";" + END_LN);
      buffer.append("" + END_LN);

      buffer.append("  function btn_" + strId + "_onClick()" + END_LN);
      buffer.append("  {" + END_LN);
      buffer.append("    if (g_" + strId + "_isEnabled)" + END_LN);
      if (strActionMouseClick.length() > 0)
      {
         String strBrackets = (strActionMouseClick.charAt(strActionMouseClick.length()-1) == ')') ? ";" : "();";
         buffer.append("      " + strActionMouseClick + strBrackets + END_LN);
      }
      else
         buffer.append("      ;" + END_LN);
      buffer.append("  }" + END_LN);

      buffer.append("  function btn_" + strId + "_mouseOver()" + END_LN);
      buffer.append("  {" + END_LN);
      buffer.append("    if (g_" + strId + "_isEnabled)" + END_LN);
      buffer.append("    {" + END_LN);
      buffer.append("      var bt = button_getButton(\"" + strId + "\");" + END_LN);
      buffer.append("      if (bt.src == eval(\"g_" + strId + "\" + g_" + strId + "_state + \"_imgNormal.src\"))" + END_LN);
      buffer.append("        bt.src = eval(\"g_" + strId + "\" + g_" + strId + "_state + \"_imgMouseOver.src\");" + END_LN);
      if (strActionMouseEnter.length() > 0)
      {
         String strBrackets = (strActionMouseEnter.charAt(strActionMouseEnter.length()-1) == ')') ? ";" : "();";
         buffer.append("      " + strActionMouseEnter + strBrackets + END_LN);
      }
      buffer.append("    }" + END_LN);
      buffer.append("  }" + END_LN);

      buffer.append("  function btn_" + strId + "_mouseOut()" + END_LN);
      buffer.append("  {" + END_LN);
      buffer.append("    if (g_" + strId + "_isEnabled)" + END_LN);
      buffer.append("    {" + END_LN);
      buffer.append("      var bt = button_getButton(\"" + strId + "\");" + END_LN);
      buffer.append("      bt.src = eval(\"g_" + strId + "\" + g_" + strId + "_state + \"_imgNormal.src\");" + END_LN);
      if (strActionMouseLeave.length() > 0)
      {
         String strBrackets = (strActionMouseLeave.charAt(strActionMouseLeave.length()-1) == ')') ? ";" : "();";
         buffer.append("      " + strActionMouseLeave + strBrackets + END_LN);
      }
      buffer.append("    }" + END_LN);
      buffer.append("  }" + END_LN);

      buffer.append("  function btn_" + strId + "_mouseDown()" + END_LN);
      buffer.append("  {" + END_LN);
      buffer.append("    if (g_" + strId + "_isEnabled)" + END_LN);
      buffer.append("    {" + END_LN);
      buffer.append("      var bt = button_getButton(\"" + strId + "\");" + END_LN);
      buffer.append("      bt.src = eval(\"g_" + strId + "\" + g_" + strId + "_state + \"_imgMouseDown.src\");" + END_LN);
      if (strActionMouseDown.length() > 0)
      {
         String strBrackets = (strActionMouseDown.charAt(strActionMouseDown.length()-1) == ')') ? ";" : "();";
         buffer.append("      " + strActionMouseDown + strBrackets + END_LN);
      }
      buffer.append("    }" + END_LN);
      buffer.append("  }" + END_LN);

      buffer.append("  function btn_" + strId + "_mouseUp()" + END_LN);
      buffer.append("  {" + END_LN);
      buffer.append("    if (g_" + strId + "_isEnabled)" + END_LN);
      buffer.append("    {" + END_LN);
      buffer.append("      var bt = button_getButton(\"" + strId + "\");" + END_LN);
      buffer.append("      bt.src = eval(\"g_" + strId + "\" + g_" + strId + "_state + \"_imgMouseOver.src\");" + END_LN);
      if (strActionMouseUp.length() > 0)
      {
         String strBrackets = (strActionMouseUp.charAt(strActionMouseUp.length()-1) == ')') ? ";" : "();";
         buffer.append("      " + strActionMouseUp + strBrackets + END_LN);
      }
      buffer.append("    }" + END_LN);
      buffer.append("  }" + END_LN);

      //buffer.append("</script>" + END_LN);
      buffer.append("" + END_LN);

      return buffer;
   }
}
