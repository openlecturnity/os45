package imc.lecturnity.converter;

import java.awt.Dimension;
import java.io.*;
import java.util.StringTokenizer;
import javax.swing.JOptionPane;

import imc.epresenter.filesdk.Metadata;
import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.wizard.PublisherWizardData;
import imc.lecturnity.util.NativeUtils;

public class FlashSnippetCreator extends SnippetCreator
{
   public static Localizer localizer;
   private static String URL_HAS_ILLEGAL_CHARACTERS = "[!]";
   private static String URL_NO_REPLAY_POSSIBLE = "[!]";
   private static String LMS_NO_COURSE_RESUMING_ALERT = "[!]";
   private static String NO_APPROPRIATE_FLASH_PLAYER = "[!]";
   private static String POPUP_BLOCKER_ALERT = "[!]";
   private static String POPUP_BLOCKER_TEXT1 = "[!]";
   private static String POPUP_BLOCKER_TEXT2 = "[!]";
   private static String POPUP_BLOCKER_TEXT3 = "[!]";
   private static String POPUP_BLOCKER_TEXT4 = "[!]";
   private static String POPUP_BLOCKER_TEXT5 = "[!]";
   private static String NO_JAVASCRIPT_HEADER = "[!]";
   private static String NO_JAVASCRIPT_REASON = "[!]";
   private static String NO_JAVASCRIPT_CAUSE1 = "[!]";
   private static String NO_JAVASCRIPT_SPECIAL = "[!]";
   private static String NO_JAVASCRIPT_CAUSE2 = "[!]";
   private static String NO_JAVASCRIPT_IMAGE = "[!]";

   static
   {
      try
      {
         Localizer l 
            = new Localizer("/imc/lecturnity/converter/TemplateSnippetCreator_",
                            "en");

         URL_HAS_ILLEGAL_CHARACTERS = l.getLocalized("URL_HAS_ILLEGAL_CHARACTERS");
         URL_NO_REPLAY_POSSIBLE = l.getLocalized("URL_NO_REPLAY_POSSIBLE");
         LMS_NO_COURSE_RESUMING_ALERT = l.getLocalized("LMS_NO_COURSE_RESUMING_ALERT");
         NO_APPROPRIATE_FLASH_PLAYER = l.getLocalized("NO_APPROPRIATE_FLASH_PLAYER");
         POPUP_BLOCKER_ALERT = l.getLocalized("POPUP_BLOCKER_ALERT");
         POPUP_BLOCKER_TEXT1 = l.getLocalized("POPUP_BLOCKER_TEXT1");
         POPUP_BLOCKER_TEXT2 = l.getLocalized("POPUP_BLOCKER_TEXT2");
         POPUP_BLOCKER_TEXT3 = l.getLocalized("POPUP_BLOCKER_TEXT3");
         POPUP_BLOCKER_TEXT4 = l.getLocalized("POPUP_BLOCKER_TEXT4");
         POPUP_BLOCKER_TEXT5 = l.getLocalized("POPUP_BLOCKER_TEXT5");
         NO_JAVASCRIPT_HEADER = l.getLocalized("NO_JAVASCRIPT_HEADER");
         NO_JAVASCRIPT_REASON = l.getLocalized("NO_JAVASCRIPT_REASON");
         NO_JAVASCRIPT_CAUSE1 = l.getLocalized("NO_JAVASCRIPT_CAUSE1");
         NO_JAVASCRIPT_SPECIAL = l.getLocalized("NO_JAVASCRIPT_SPECIAL");
         NO_JAVASCRIPT_CAUSE2 = l.getLocalized("NO_JAVASCRIPT_CAUSE2");
         NO_JAVASCRIPT_IMAGE = l.getLocalized("NO_JAVASCRIPT_IMAGE");

         localizer = l;
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

   public FlashSnippetCreator()
   {
      super();
   }

   public static String slashEndln = "\r\n";

   public void createSnippets(PublisherWizardData pwData, boolean bFixedSize, 
                              boolean bMenuEnabled, boolean bScormEnabled, boolean bLecIdentityHidden, 
                              String strBackgroundHex, String strTargetFilename,
                              Dimension dimOuterFlashSize)
   {
      ProfiledSettings ps = pwData.GetProfiledSettings();
      DocumentData dd = pwData.GetDocumentData();
      Metadata metadata = dd.GetMetadata();
      
      // For new snippets for the FlexPlayer in popup window
      int nVideoWidth = (dd.HasNormalVideo()) ? ps.GetIntValue("iVideoWidth") : 0; 
      int nVideoHeight = (dd.HasNormalVideo()) ? ps.GetIntValue("iVideoHeight") : 0; 

      String author = "";
      String title = "";
      String date = "";
      String length = "";
      String thumbs = "";
      String scormIncludes= "";
      if (metadata != null)
      {
         author = metadata.author;
         title  = metadata.title;
         date   = metadata.recordDate;
         length = metadata.recordLength;
         thumbs = "" + metadata.thumbnails.length;
      }
      addSnippet("Author", checkHtmlString(author));
      addSnippet("Title", checkHtmlString(title));
      addSnippet("RecordDate", checkHtmlString(date));
      addSnippet("NumOfThumbnails", thumbs);
      
      if (bLecIdentityHidden) {
         addSnippet("LecturnityWebPlayer", "");
         addSnippet("Version", "");
      } else {
         addSnippet("LecturnityWebPlayer", "- LECTURNITY WebPlayer");
         addSnippet("Version", NativeUtils.getVersionStringShortShort());
      }
      
      if(bScormEnabled == true)
      {
         String f1 = "<script src=\"js/APIWrapper.js\" type=\"text/javascript\"></script>";
         String f2 = "<script src=\"js/variables.js\" type=\"text/javascript\"></script>";
         String f3 = "<script src=\"js/scorm.js\" type=\"text/javascript\"></script>";
         String f4 = "<script src=\"js/scorm_utils.js\" type=\"text/javascript\"></script>";
         scormIncludes = "\t" + f1 + "\n" + "\t" + f2 + "\n" + "\t" + f3 + "\n" + "\t" + f4;
      }
      addSnippet("ScormFiles", scormIncludes);
      StringTokenizer st = new StringTokenizer(checkHtmlString(length), ":");
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
//      addSnippet(new Snippet("RecordLength", tpc.strRecordLength));
      System.out.println("RecordLengthNew:" + strMyRecLen);
      addSnippet(new Snippet("RecordLengthNew", strMyRecLen));
      addSnippet("RecordLength", checkHtmlString(length));
      addSnippet("ScormEnabled", "" + bScormEnabled);
      addSnippet("FlashWidth", "" + dimOuterFlashSize.width);
      addSnippet("FlashHeight", "" + dimOuterFlashSize.height);
      
      addSnippet("UrlHasIllegalCharacters", "" + checkHtmlString(URL_HAS_ILLEGAL_CHARACTERS));
      addSnippet("UrlNoReplayPossible", "" + checkHtmlString(URL_NO_REPLAY_POSSIBLE));

      addSnippet("LmsNoCourseResumingAlert", "" + checkHtmlString(LMS_NO_COURSE_RESUMING_ALERT));
      

      // New snippets for the FlexPlayer in popup window
      addSnippet(new Snippet("IsStandalone", "" + dd.IsDenver()));
      addSnippet(new Snippet("SlidesWidth", "" + ps.GetIntValue("iSlideWidth")));
      addSnippet(new Snippet("SlidesHeight", "" + ps.GetIntValue("iSlideHeight")));
      addSnippet(new Snippet("VideoWidth", "" + nVideoWidth));
      addSnippet(new Snippet("VideoHeight", "" + nVideoHeight));
      addSnippet(new Snippet("NavigationControlBarState", ("" + ps.GetIntValue("iControlBarVisibility"))));
      addSnippet(new Snippet("NavigationDocumentStructureState", ("" + ps.GetIntValue("iDocumentStructureVisibility"))));
      addSnippet(new Snippet("BaseDocumentName", ps.GetStringValue("strTargetPathlessDocName")));
      addSnippet(new Snippet("PopupBlockerAlert", POPUP_BLOCKER_ALERT));
      addSnippet(new Snippet("PopupBlockerText1", POPUP_BLOCKER_TEXT1));
      addSnippet(new Snippet("PopupBlockerText2", POPUP_BLOCKER_TEXT2));
      addSnippet(new Snippet("PopupBlockerText3", POPUP_BLOCKER_TEXT3));
      addSnippet(new Snippet("PopupBlockerText4", POPUP_BLOCKER_TEXT4));
      addSnippet(new Snippet("PopupBlockerText5", POPUP_BLOCKER_TEXT5));
      addSnippet(new Snippet("NoJavascriptHeader", checkHtmlString(NO_JAVASCRIPT_HEADER)));
      addSnippet(new Snippet("NoJavascriptReason", checkHtmlString(NO_JAVASCRIPT_REASON)));
      addSnippet(new Snippet("NoJavascriptCause1", checkHtmlString(NO_JAVASCRIPT_CAUSE1)));
      addSnippet(new Snippet("NoJavascriptSpecial", checkHtmlString(NO_JAVASCRIPT_SPECIAL)));
      addSnippet(new Snippet("NoJavascriptCause2", checkHtmlString(NO_JAVASCRIPT_CAUSE2)));
      addSnippet(new Snippet("NoJavascriptImage", checkHtmlString(NO_JAVASCRIPT_IMAGE)));
      

      // Background color
      addSnippet("BackgroundColor", "#" + strBackgroundHex);


      // Metadata
      addSnippet(new Snippet("MetadataKeywords", getMetadataKeywords(metadata)));
      
      File targetFile = new File(strTargetFilename);
      String targetFileName = targetFile.getName();

      String flashWidth = "100%";
      String flashHeight = "100%";
      
      if (bFixedSize)
      {
         flashWidth = "" + dimOuterFlashSize.width;
         flashHeight = "" + dimOuterFlashSize.height;
      }

      // FlashVars (Bugfix #3710/#4315)
      // Any Video: Accompanying video / video combined with clips / "Denver" video
      boolean bHasAnyVideo = dd.HasNormalVideo() 
                             || (dd.HasClips() && !ps.GetBoolValue("bShowClipsOnSlides")) 
                             || dd.IsDenver();
      // Clips: must be separated from video
      boolean bHasClips = dd.HasClips() && ps.GetBoolValue("bShowClipsOnSlides");

      String strFlashVars = "";
      String strBaseUrl = ps.GetStringValue("strTargetUrlHttpServer");
      // Bugfix 4596: Replace a "/" by ""
      if (strBaseUrl.equals("/"))
         strBaseUrl = "";
      strFlashVars += "baseUrl=";
      strFlashVars += strBaseUrl;
      if (bHasAnyVideo || bHasClips)
      {
         String strStreamUrl = ps.GetStringValue("strTargetUrlStreamingServer");
         // Bugfix 4596: Replace a "/" by ""
         if (strStreamUrl.equals("/"))
            strStreamUrl = "";
         strFlashVars += "&streamUrl=";
         strFlashVars += strStreamUrl;
         
         String strDocName = ps.GetStringValue("strTargetPathlessDocName");
         if (bHasAnyVideo)
         {
            strFlashVars += "&videoFlvName=";
            strFlashVars += strDocName;
            if (ps.GetIntValue("iReplayType") != PublisherWizardData.REPLAY_SERVER)
               strFlashVars += ".flv";
         }
         
         if (bHasClips && (dd.GetVideoClipInfos().length > 0))
         {
            for (int i = 0; i < dd.GetVideoClipInfos().length; ++i)
            {
               strFlashVars += ("&clip" + i + "FlvName=");
               strFlashVars += (strDocName + "_clip" + i);
               if (ps.GetIntValue("iReplayType") != PublisherWizardData.REPLAY_SERVER)
                  strFlashVars += ".flv";
            }
         }
         
         strFlashVars += "&streamStripFlvSuffix=true";
      }
      String strAutostart = "&autoStart=";
      if (ps.GetBoolValue("bDoAutostart"))
         strAutostart += "true";
      else
         strAutostart += "false";
      strFlashVars += strAutostart;


      // "FlashVars"
      addSnippet("FlashVars", "&"+strFlashVars);

      // "FlashPlayer"
      StringBuffer flashObject = new StringBuffer(1024);
      flashObject.append("document.writeln('<object id=\"FlashPlayer\" ');" + slashEndln);
      flashObject.append("document.writeln('  classid=\"CLSID:D27CDB6E-AE6D-11cf-96B8-444553540000\" ');" + slashEndln);
      flashObject.append("document.writeln('  codebase=\"https://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,40,0\" ');" + slashEndln);
      flashObject.append("document.writeln('  width=\"" + flashWidth + "\" ');" + slashEndln);
      flashObject.append("document.writeln('  height=\"" + flashHeight + "\">');" + slashEndln);
      flashObject.append("document.writeln('  <param name=\"allowScriptAccess\" value=\"always\" />');" + slashEndln);
      //flashObject.append("document.writeln('  <param name=\"movie\" value=\"" + targetFileName + "\">');" + slashEndln);
      flashObject.append("document.writeln('  <param name=\"movie\" value=\"" + LPLibs.flashGetPreloaderFilename() + "\">');" + slashEndln);
      flashObject.append("document.writeln('  <param name=\"FlashVars\" value=\"" + strFlashVars + "\">');" + slashEndln);
      flashObject.append("document.writeln('  <param name=\"quality\" value=\"high\">');" + slashEndln);
      flashObject.append("document.writeln('  <param name=\"loop\" value=\"false\"');" + slashEndln);
      flashObject.append("document.writeln('  <param name=\"scale\" value=\"showall\">');" + slashEndln);
      if (!bMenuEnabled)
         flashObject.append("document.writeln('  <param name=\"menu\" value=\"false\">');" + slashEndln);
      else
         flashObject.append("document.writeln('  <param name=\"menu\" value=\"true\">');" + slashEndln);
      flashObject.append("document.writeln('  <param name=\"swLiveConnect\" value=\"true\">');" + slashEndln);
      flashObject.append("document.writeln('  <embed name=\"FlashPlayer\"');" + slashEndln);
      flashObject.append("document.writeln('    type=\"application/x-shockwave-flash\" ');" + slashEndln);
      flashObject.append("document.writeln('    pluginspage=\"http://www.macromedia.com/go/getflashplayer\" ');" + slashEndln);
      //flashObject.append("document.writeln('    src=\"" + targetFileName + "\" ');" + slashEndln);
      flashObject.append("document.writeln('    src=\"" + LPLibs.flashGetPreloaderFilename() + "\" ');" + slashEndln);
      flashObject.append("document.writeln('    FlashVars=\"" + strFlashVars + "\" ');" + slashEndln);
      flashObject.append("document.writeln('    width=\"" + flashWidth + "\" ');" + slashEndln);
      flashObject.append("document.writeln('    height=\"" + flashHeight + "\" ');" + slashEndln);
      flashObject.append("document.writeln('    quality=\"high\" ');" + slashEndln);
      flashObject.append("document.writeln('    loop=\"false\"');" + slashEndln);
      flashObject.append("document.writeln('    scale=\"showall\" ');" + slashEndln);
      if (!bMenuEnabled)
         flashObject.append("document.writeln('    menu=\"false\" ');" + slashEndln);
      else
         flashObject.append("document.writeln('    menu=\"true\" ');" + slashEndln);
      flashObject.append("document.writeln('    swLiveConnect=\"true\">');" + slashEndln);
//      flashObject.append("document.writeln('  </embed>');" + slashEndln); // </embed> can be omitted
      flashObject.append("document.writeln('</object>');" + slashEndln);
      addSnippet("FlashPlayer", flashObject.toString());

      // "LecFlashPlayer"
      String strLecFlashPlayer;
      if (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX)
         strLecFlashPlayer = "LecturnityFlashPlayer";
      else
         strLecFlashPlayer = "FlashPlayer";
      addSnippet("LecFlashPlayer", strLecFlashPlayer);

      // "LecMovie"
      String strLecMovie;
      if (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX)
         strLecMovie = "";
      else
         strLecMovie = "sprLecMovie.";
      addSnippet("LecMovie", strLecMovie);

      // "GetScormVariable"
      String strGetScormVariable;
      if (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX)
         strGetScormVariable = "GetScormVariable";
      else
         strGetScormVariable = "GetVariable";
      addSnippet("GetScormVariable", strGetScormVariable);

      // "SetScormVariable"
      String strSetScormVariable;
      if (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX)
         strSetScormVariable = "SetScormVariable";
      else
         strSetScormVariable = "SetVariable";
      addSnippet("SetScormVariable", strSetScormVariable);

      // "FixedFlashSize"
      addSnippet("FixedFlashSize", "" + bFixedSize);

      // "NoAppropriateFlashPlayer"
      addSnippet("NoAppropriateFlashPlayer", NO_APPROPRIATE_FLASH_PLAYER);
   }
}
