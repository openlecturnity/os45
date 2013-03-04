package imc.lecturnity.converter.wizard;

import java.io.IOException;

import javax.swing.JOptionPane;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.converter.StreamInformation;

public class StreamingConvertWizardData extends ScormConvertWizardData
{
   protected static String SOURCE = "[!]";
   protected static String TARGET = "[!]";
   protected static String PUBLISH = "[!]";
   protected static String PUBLISH_LOCAL = "[!]";
   protected static String PUBLISH_HTTP = "[!]";
   protected static String HTTP_URL = "[!]";
   protected static String HTTP_DIR = "[!]";
   protected static String LOCAL_DIR = "[!]";
   protected static String NA = "[!]";
   protected static String TARGET_VER = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/StreamingConvertWizardData_",
             "en");
         
         SOURCE = l.getLocalized("SOURCE");
         TARGET = l.getLocalized("TARGET");
         NA = l.getLocalized("NA");

         PUBLISH = l.getLocalized("PUBLISH");
         PUBLISH_LOCAL = l.getLocalized("PUBLISH_LOCAL");
         PUBLISH_HTTP = l.getLocalized("PUBLISH_HTTP");

         HTTP_DIR = l.getLocalized("HTTP_DIR");
         HTTP_URL = l.getLocalized("HTTP_URL");
         LOCAL_DIR = l.getLocalized("LOCAL_DIR");

         TARGET_VER = l.getLocalized("TARGET_VER");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "StreamingConvertWizardData\n" + e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         System.exit(1);
      }
   }
   

   public static final int REPLAY_LOCAL     = 0;
   public static final int REPLAY_HTTP      = 1;
   public static final int REPLAY_WMSRV     = 2;
   public static final int REPLAY_RMSRV     = 2;
   public static final int REPLAY_FLASHSRV  = 2;

   /** One of the REPLAY_* constants. */
   public int replayType = REPLAY_LOCAL;

   public static final int PANEL_EXTD_EXTENT    = 0;
   public static final int PANEL_EXTD_SLIDES    = 1;
   public static final int PANEL_EXTD_SIZES     = 2;
   public static final int PANEL_EXTD_RATES     = 3;
///   public static final int PANEL_EXTD_LAYOUT    = 1; //not used any more since 1.6.1.p1

   
   /** One of the PANEL_EXTD_* constants. */
   public int panelExtdType = PANEL_EXTD_EXTENT;
   
   public static final int VIDEOSIZE_STANDARD = 0;
   public static final int VIDEOSIZE_ORIGINAL = 1;
   public static final int VIDEOSIZE_MAXCLIP  = 2;
   public static final int VIDEOSIZE_CUSTOM   = 3;

   /** One of the VIDEOSIZE_* constants. */
   public int videoSizeType = VIDEOSIZE_STANDARD;

   public static final int CLIPSIZE_STANDARD = 0;
   public static final int CLIPSIZE_MAXSLIDE = 1;
   public static final int CLIPSIZE_CUSTOM   = 2;

   /** One of the CLIPSIZE_* constants. */
   public int clipSizeType = CLIPSIZE_STANDARD;

   public static final int SLIDESIZE_STANDARD   = 0;
   public static final int SLIDESIZE_ORIGINAL   = 1;
   public static final int SLIDESIZE_SCREEN_RES = 2;
   public static final int SLIDESIZE_CUSTOM     = 3;

   /** One of the SLIDESIZE_* constants. */
   public int slideSizeType = SLIDESIZE_STANDARD;

   /** The selected index of the ComboBox for slide resolution 
   * depending on screen resolution. */
   public int slideScreenResSelectedIndex = 0;
   
   /** Flag, if the aspect ratio for the slides should be considered. 
   * (Only necessary for custom slide size.) */
   public boolean useSlideAspectRatio = true;
   
   /** The target directory of the Streaming Server. */
   public String targetDirStreamingServer = "";
   /** The target directory of the HTTP-Server. */
   public String targetDirHttpServer = "";
   /** The target URL of the Streaming Server. */
   public String targetUrlStreamingServer = "";
   /** The target URL of the HTTP-Server. */
   public String targetUrlHttpServer = "";
   /** The target document name (without path). */
   public String targetDocName = "";

   /** The four character code of the used codec. */
   public String fccCodec = "";

   /** The preferred width of the slides, optimized for a 1024x768 screen; 
   * may be -1 if not yet initialized. */
   public int    preferredSlideWidth = -1;
   /** The preferred height of the slides, optimized for a 1024x768 screen; 
   * may be -1 if not yet initialized. */
   public int    preferredSlideHeight = -1;

   /** The output width of the slides; may be -1 if not yet initialized. */
   public int    outputSlideWidth = -1;
   /** The output height of the slides; may be -1 if not yet initialized. */
   public int    outputSlideHeight = -1;

   /** The user defined custom width of the slides; may be -1 if not yet initialized. */
   public int    customSlideWidth = -1;
   /** The user defined custom height of the slides; may be -1 if not yet initialized. */
   public int    customSlideHeight = -1;

   /** The output width of the video; may be -1 if not yet initialized. */
   public int    outputVideoWidth = -1;
   /** The output height of the video; may be -1 if not yet initialized. */
   public int    outputVideoHeight = -1;

   /** The maximum output width of the clips; may be -1 if not yet initialized. */
   public int    maxOutputClipsWidth = -1;
   /** The maximum output height of the clips; may be -1 if not yet initialized. */
   public int    maxOutputClipsHeight = -1;

   /** The user defined custom width of the video; may be -1 if not yet initialized. */
   public int    customVideoWidth = -1;
   /** The user defined custom height of the video; may be -1 if not yet initialized. */
   public int    customVideoHeight = -1;

   /** The user defined custom width of the clips; may be -1 if not yet initialized. */
   public int    customClipsWidth = -1;
   /** The user defined custom height of the clips; may be -1 if not yet initialized. */
   public int    customClipsHeight = -1;

   /** The preferred width of the screen; may be -1 if not yet initialized. */
   public int    preferredScreenWidth = -1;
   /** The preferred height of the screen; may be -1 if not yet initialized. */
   public int    preferredScreenHeight = -1;

   /** The screen width to match the slides to if slideSizeType is SLIDES_SCREEN_RES, or -1 if not initialized */
   public int    matchScreenWidth = -1;
   /** The screen height to match the slides to if slideSizeType is SLIDES_SCREEN_RES, or -1 if not initialized */
   public int    matchScreenHeight = -1;

   /** Flag, if Telepointer is visible on the slides. */
   public boolean usePointer = true;
   /** Flag, if dynamic events are shown on the slides. */
   public boolean useDynamic = true;
   /** Flag, if partial images are created instead of full images. */
   public boolean useDataReduction = true;
   /** Flag, if dynamic scaling is enabled by moving the frame borders. */
   public boolean documentIsScalable = false;
   /** Flag, if the video frame is resized to the chosen video size. */
   public boolean doConsiderVideosize = false;
   /** Flag, if clips should be separated from the video and have to be shown 
   * in the slide area. Uses SMIL (RealMedia) or ? (Windows Media), resp. */
   public boolean separateClipsFromVideo = false;

   /*
    * Three stream information objects. Containing all the necessary
    * information for profile creation and thus conversion.
    * They replace the above information.
    */
   public StreamInformation siAudioStreamInfo = null;
   public StreamInformation siVideoStreamInfo = null;
   public StreamInformation siClipsStreamInfo = null;


   public void fillOtherWizardData(ConvertWizardData otherData)
   {
      if (otherData instanceof StreamingConvertWizardData)
      {
         StreamingConvertWizardData that = (StreamingConvertWizardData)otherData;

         that.replayType = replayType;
         that.panelExtdType = panelExtdType;
         that.videoSizeType = videoSizeType;
         that.clipSizeType = clipSizeType;
         that.slideSizeType = slideSizeType;
         that.slideScreenResSelectedIndex = slideScreenResSelectedIndex;
         that.useSlideAspectRatio = useSlideAspectRatio;
         that.targetDirStreamingServer = (targetDirStreamingServer != null) ? new String(targetDirStreamingServer) : null;
         that.targetDirHttpServer = (targetDirHttpServer != null) ? new String(targetDirHttpServer) : null;
         that.targetUrlStreamingServer = (targetUrlStreamingServer != null) ? new String(targetUrlStreamingServer) : null;
         that.targetUrlHttpServer = (targetUrlHttpServer != null) ? new String(targetUrlHttpServer) : null;
         that.targetDocName = (targetDocName != null) ? new String(targetDocName) : null;
         that.fccCodec = (fccCodec != null) ? new String(fccCodec) : null;
         that.preferredSlideWidth = preferredSlideWidth;
         that.preferredSlideHeight = preferredSlideHeight;
         that.outputSlideWidth = outputSlideWidth;
         that.outputSlideHeight = outputSlideHeight;
         that.customSlideWidth = customSlideWidth;
         that.customSlideHeight = customSlideHeight;
         that.outputVideoWidth = outputVideoWidth;
         that.outputVideoHeight = outputVideoHeight;
         that.maxOutputClipsWidth = maxOutputClipsWidth;
         that.maxOutputClipsHeight = maxOutputClipsHeight;
         that.customVideoWidth = customVideoWidth;
         that.customVideoHeight = customVideoHeight;
         that.customClipsWidth = customClipsWidth;
         that.customClipsHeight = customClipsHeight;
         that.preferredScreenWidth = preferredScreenWidth;
         that.preferredScreenHeight = preferredScreenHeight;
         that.matchScreenWidth = matchScreenWidth;
         that.matchScreenHeight = matchScreenHeight;
         that.usePointer = usePointer;
         that.useDynamic = useDynamic;
         that.useDataReduction = useDataReduction;
         that.documentIsScalable = documentIsScalable;
         that.doConsiderVideosize = doConsiderVideosize;
         that.separateClipsFromVideo = separateClipsFromVideo;
         that.siAudioStreamInfo = (siAudioStreamInfo != null) ? new StreamInformation(siAudioStreamInfo) : null;
         that.siVideoStreamInfo = (siVideoStreamInfo != null) ? new StreamInformation(siVideoStreamInfo) : null;
         that.siClipsStreamInfo = (siClipsStreamInfo != null) ? new StreamInformation(siClipsStreamInfo) : null;
      }

      super.fillOtherWizardData(otherData);
   }

}
