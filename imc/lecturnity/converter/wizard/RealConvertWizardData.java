package imc.lecturnity.converter.wizard;

import java.io.IOException;

import javax.swing.JOptionPane;

import imc.epresenter.filesdk.util.Localizer;

public class RealConvertWizardData extends StreamingConvertWizardData
{
   private static String TARGETFORMAT = "[!]";
   private static String AUDIO = "[!]";
   private static String VIDEO = "[!]";
   private static String VIDEO_QUALITY = "[!]";
   private static String NORMAL = "[!]";
   private static String SMOOTHEST_MOTION = "[!]";
   private static String SHARPEST_IMAGE = "[!]";
   private static String SLIDESHOW = "[!]";
   //private static String NA = "[N/A]";
   private static String PUBLISH_REAL = "[!]";

   private static String SERVER_URL = "[!]";
   private static String SERVER_DIR = "[!]";

   //private static String TARGET_VER = "[!]";

   private static String AUDIO_BW[] 
      = {"<0>", "<1>", "<2>", "<3>", "<4>", "<5>", "<6>", "<7>", "<8>"};
   private static String VIDEO_BW[] 
      = {"<0>", "<1>", "<2>", "<3>", "<4>", "<5>", "<6>", "<7>", "<8>"};

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/RealConvertWizardData_",
             "en");
         
         //SOURCE = l.getLocalized("SOURCE");
         //TARGET = l.getLocalized("TARGET");
         TARGETFORMAT = l.getLocalized("TARGETFORMAT");
         AUDIO = l.getLocalized("AUDIO");
         VIDEO = l.getLocalized("VIDEO");
         VIDEO_QUALITY = l.getLocalized("VIDEO_QUALITY");
         NORMAL = l.getLocalized("NORMAL");
         SMOOTHEST_MOTION  = l.getLocalized("SMOOTHEST_MOTION");
         SHARPEST_IMAGE = l.getLocalized("SHARPEST_IMAGE");
         SLIDESHOW = l.getLocalized("SLIDESHOW");
         //NA = l.getLocalized("NA");

         //PUBLISH = l.getLocalized("PUBLISH");
         //PUBLISH_LOCAL = l.getLocalized("PUBLISH_LOCAL");
         //PUBLISH_HTTP = l.getLocalized("PUBLISH_HTTP");
         PUBLISH_REAL = l.getLocalized("PUBLISH_REAL");

         //HTTP_DIR = l.getLocalized("HTTP_DIR");
         //HTTP_URL = l.getLocalized("HTTP_URL");
         SERVER_DIR = l.getLocalized("SERVER_DIR");
         SERVER_URL = l.getLocalized("SERVER_URL");
         //LOCAL_DIR = l.getLocalized("LOCAL_DIR");

         //TARGET_VER = l.getLocalized("TARGET_VER");

         // get the correct names for the bandwidth selection from the
         // bandwidth wizard panel:
         l = new Localizer
            ("/imc/lecturnity/converter/wizard/RealBandwidthWizardPanel_",
             "en");

         for (int i=0; i<AUDIO_BW.length; i++)
            AUDIO_BW[i] = l.getLocalized("BITRATE" + i);
         for (int i=0; i<VIDEO_BW.length; i++)
            VIDEO_BW[i] = l.getLocalized("BITRATE" + i);

      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "RealConvertWizardData\n" + e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         System.exit(1);
      }
   }
   
   public boolean[] audioBitrates = {false, false, true, false, false, true, false, false, true};
   public boolean[] videoBitrates = {false, false, true, false, false, true, false, false, true};

//    public int replayType = REPLAY_LOCAL;

//    public String httpTarget = "rtsp://";
//    public String realTarget = "";

   public int minRealVersion = 8;
   public int realpixBitrate = 12000;
   
   public int realVideoQuality   = 0;

   public String getSummary()
   {
      String summary = SOURCE + presentationFileName + '\n';
      summary += TARGET + targetDocName + "\n\n";

      summary += TARGET_VER + "RealMedia " + minRealVersion + "\n\n";
      
      summary += AUDIO;
      boolean firstBr = true;
      for (int i=0; i<AUDIO_BW.length; i++)
      {
         if (audioBitrates[i])
         {
            if (firstBr)
            {
               summary += AUDIO_BW[i];
               firstBr = false;
            }
            else
               summary += ", " + AUDIO_BW[i];
         }
      }
      summary += ".\n";
      
      summary += VIDEO_QUALITY;
      switch(realVideoQuality)
      {
      case 0:
         summary += NORMAL + '\n';
         break;
      case 1:
         summary += SMOOTHEST_MOTION + '\n';
         break;
      case 2:
         summary += SHARPEST_IMAGE + '\n';
         break;
      case 3:
         summary += SLIDESHOW + '\n';
         break;
      }
      summary += "\n";

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

      case REPLAY_RMSRV:
         summary += PUBLISH + PUBLISH_REAL + '\n';
         summary += HTTP_DIR + targetDirHttpServer + '\n';
         summary += SERVER_DIR + targetDirStreamingServer + '\n';
         if ((targetUrlHttpServer != null) && (!targetUrlHttpServer.equals("")))
           summary += HTTP_URL + targetUrlHttpServer + '\n';
         summary += SERVER_URL + targetUrlStreamingServer + '\n';
         break;
      }

      return summary;
   }
}
