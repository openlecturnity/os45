package imc.lecturnity.converter.wizard;

import java.io.IOException;

import javax.swing.JOptionPane;

import imc.epresenter.filesdk.util.Localizer;
///import imc.lecturnity.converter.StreamInformation;

public class WmpConvertWizardData extends StreamingConvertWizardData
{
   private static String TARGETFORMAT = "[!]";
   private static String PUBLISH_WMP = "[!]";
   private static String SERVER_DIR = "[!]";
   private static String SERVER_URL = "[!]";
   private static String PROFILE = "[!]";
   private static String SETTINGS = "[!]";
   private static String CLIPVERSION = "[!]";
   private static String CLIPPROFILE = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/WmpConvertWizardData_",
             "en");

         TARGETFORMAT = l.getLocalized("TARGETFORMAT");
         PUBLISH_WMP = l.getLocalized("PUBLISH_WMP");
         SERVER_DIR = l.getLocalized("SERVER_DIR");
         SERVER_URL = l.getLocalized("SERVER_URL");
         PROFILE = l.getLocalized("PROFILE");
         SETTINGS = l.getLocalized("SETTINGS");
         CLIPVERSION = l.getLocalized("CLIPVERSION");
         CLIPPROFILE = l.getLocalized("CLIPPROFILE");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "WmpConvertWizardData\n" + e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         System.exit(1);
      }
   }

   public static final int PLAYER_CP = 0x20000;
   public static final int PLAYER_64 = 0x40000;
   public static final int PLAYER_70 = 0x70000;
   public static final int PLAYER_80 = 0x80000;
   public static final int PLAYER_90 = 0x90000;

   // System Profile stuff
   public int    systemProfileType        = PLAYER_70;
   public int    systemProfile            = 0;
   public String systemProfileDescription = "";
   // The custom profile file includes full path and extension ".prx" 
   public String customProfileFile        = "";

   // System Profile stuff for Clips
   public int    clipSystemProfileType        = PLAYER_70;
   public int    clipSystemProfile            = 0;
   public String clipSystemProfileDescription = "";
   // The custom profile file includes full path and extension ".prx" 
   public String clipCustomProfileFile        = "";
   public boolean bClipsAreScreengrabbing      = true;
   
   // Suffix for WM files: .wm or .wma/.wmv
   public String wmSuffix = ".wm";
   public boolean bUseAdvancedWmSuffix = false;
   
///   /*
///    * Three stream information objects. Containing all the necessary
///    * information for profile creation and thus conversion.
///    * They replace the above information.
///    */
///   public StreamInformation siAudioStreamInfo = null;
///   public StreamInformation siVideoStreamInfo = null;
///   public StreamInformation siClipsStreamInfo = null;
   
   
   /**
    * If this variable is set to <i>true</i> the HTML files
    * created are optimized for Netscape. Defaults to <i>false</i>.
    */
   public boolean optimizeForNetscape     = false;

   public String getSummary()
   {
      String summary = SOURCE + presentationFileName + '\n';
      summary += TARGET + targetDocName + "\n\n";

      
      
      // new summary style for WM export starting with 1.6.1.p1
      String sProfileOne = "Audio";
      if (siVideoStreamInfo != null)
         sProfileOne += " + Video";
      
      summary += PROFILE + "WindowsMedia "+sProfileOne+"\n";
      summary += SETTINGS + " " + systemProfileDescription +"\n\n";
      
      if (siClipsStreamInfo != null && separateClipsFromVideo)
      {
         summary += PROFILE + "WindowsMedia Clips\n";
         summary += SETTINGS + " " + clipSystemProfileDescription +"\n\n";
      }
      
      
      
      /* old style; prior to 1.6.1.p1
      // Disable A/V profile in only one case: 
      // if we have clips on slides without video
      boolean enableVideoProfiles = true;
      if (containsMultipleVideos && !containsNormalVideo && separateClipsFromVideo)
         enableVideoProfiles = false;

      if (enableVideoProfiles)
      {
         if (systemProfileType == PLAYER_CP)
            summary += TARGET_VER + "WindowsMedia (Custom Profile)\n";
         else if (systemProfileType == PLAYER_64)
            summary += TARGET_VER + "WindowsMedia 6.4\n";
         else if (systemProfileType == PLAYER_70)
            summary += TARGET_VER + "WindowsMedia 7.0\n";
         else if (systemProfileType == PLAYER_80)
            summary += TARGET_VER + "WindowsMedia 8.0\n";
         else if (systemProfileType == PLAYER_90)
            summary += TARGET_VER + "WindowsMedia 9.0\n";
      }
      
      summary += PROFILE + systemProfileDescription + "\n\n";
      
      if (separateClipsFromVideo)
      {
         if (clipSystemProfileType == PLAYER_CP)
            summary += CLIPVERSION + "WindowsMedia (Custom Profile)\n";
         else if (clipSystemProfileType == PLAYER_64)
            summary += CLIPVERSION + "WindowsMedia 6.4\n";
         else if (clipSystemProfileType == PLAYER_70)
            summary += CLIPVERSION + "WindowsMedia 7.0\n";
         else if (clipSystemProfileType == PLAYER_80)
            summary += CLIPVERSION + "WindowsMedia 8.0\n";
         else if (clipSystemProfileType == PLAYER_90)
            summary += CLIPVERSION + "WindowsMedia 9.0\n";
         
         summary += CLIPPROFILE + clipSystemProfileDescription + "\n\n";
      }
      */
      
      switch (replayType)
      {
      case REPLAY_LOCAL:
         summary += PUBLISH + PUBLISH_LOCAL + '\n';
         summary += LOCAL_DIR + targetDirHttpServer + '\n';
         break;

      case REPLAY_HTTP:
         summary += PUBLISH + PUBLISH_HTTP + '\n';
         summary += HTTP_DIR + targetDirHttpServer + '\n';
         if ((targetUrlHttpServer != null) && (!targetUrlHttpServer.equals("")))
           summary += HTTP_URL + targetUrlHttpServer + '\n';
         break;

      case REPLAY_WMSRV:
         summary += PUBLISH + PUBLISH_WMP + '\n';
         summary += HTTP_DIR + targetDirHttpServer + '\n';
         summary += SERVER_DIR + targetDirStreamingServer + '\n';
         summary += SERVER_URL + targetUrlStreamingServer + '\n';
         break;
      }
      
      // Windows Media EULA
      summary +="\n\n\n\n";
      summary += "Portions utilize Microsoft Windows Media Technologies. \n";
      summary += "Copyright (c) 1999-2002 Microsoft Corporation. All Rights Reserved. \n";

      return summary;
   }
}
