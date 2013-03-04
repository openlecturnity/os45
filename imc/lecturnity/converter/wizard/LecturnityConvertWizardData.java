package imc.lecturnity.converter.wizard;

import java.awt.Dimension;

import java.io.IOException;

import javax.swing.JOptionPane;

import imc.epresenter.filesdk.util.Localizer;

public class LecturnityConvertWizardData extends ConvertWizardData
{
   private static String SOURCE = "[!]";
   private static String TARGET = "[!]";
   private static String TARGETFORMAT = "[!]";
   private static String AUDIO = "[!]";
   private static String THUMBS = "[!]";
   private static String EMBED = "[!]";
   private static String HELPDOC = "[!]";
   private static String VIDEO = "[!]";
   private static String VIDEO_FILE = "[!]";
   private static String VIDEOOFFSET = "[!]";
   private static String E_CDN = "[!]";
   private static String MS = "[!]";
   private static String FULLSCREEN = "[!]";
   private static String CLIPSONSLIDES = "[!]";

   private static String BW[] = {"[!]", "[!]", "<unknown>", "<unknown>"};

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/LecturnityConvertWizardData_",
             "en");
         
         SOURCE = l.getLocalized("SOURCE");
         TARGET = l.getLocalized("TARGET");
         TARGETFORMAT = l.getLocalized("TARGETFORMAT");
         AUDIO = l.getLocalized("AUDIO");
         THUMBS = l.getLocalized("THUMBS");
         EMBED = l.getLocalized("EMBED");
         HELPDOC = l.getLocalized("HELPDOC");
         VIDEO = l.getLocalized("VIDEO");
         VIDEO_FILE = l.getLocalized("VIDEO_FILE");
         VIDEOOFFSET = l.getLocalized("VIDEOOFFSET");
         E_CDN = l.getLocalized("E_CDN");
         MS = l.getLocalized("MS");
         FULLSCREEN = l.getLocalized("FULLSCREEN");
         CLIPSONSLIDES = l.getLocalized("CLIPSONSLIDES");

         // get the correct names for the bandwidth selection from the
         // bandwidth wizard panel:
         l = new Localizer
            ("/imc/lecturnity/converter/wizard/LecturnityBandwidthWizardPanel_",
             "en");

         BW[0] = l.getLocalized("BW_256");
         BW[1] = l.getLocalized("BW_048");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "LecturnityConvertWizardData",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         System.exit(1);
      }
   }

   public static final int AUDIO_WAV_256 = 0;
   public static final int AUDIO_AQS_48  = 1;
   public static final int AUDIO_MP3     = 2;
   public static final int AUDIO_REAL    = 3;

   public int targetAudioFormat      = 1;
   public boolean useThumbnails      = true;
//    public Dimension thumbnailsSize   = new Dimension(100, 100);
   public boolean[] thumbnailsSizes  = {true, true, true};
   public boolean embedFonts         = false;
   public boolean createHelpDocument = false;
   public boolean startInFullScreen  = false;
   public boolean showClipsOnSlides  = true;
   public boolean addToCdProject     = false;

   public String getSummary()
   {
      String summary = SOURCE + presentationFileName + '\n';
      summary += TARGET + targetFileName + "\n";
      summary += TARGETFORMAT + "\n\n";

      summary += AUDIO + BW[targetAudioFormat] + '\n';
      summary += VIDEO + getYesNo(hasVideoFile) + '\n';
      if (hasVideoFile)
      {
         if (!containsMultipleVideos)
         {
            summary += VIDEO_FILE + videoFileName + '\n';
            summary += VIDEOOFFSET + videoOffsetMs + " " + MS + '\n';
         }
         else
         {
            summary += VIDEO_FILE;
            for (int i=0; i<multiVideoClips.length; ++i)
            {
               summary += multiVideoClips[i].clipFileName;
               if (i < multiVideoClips.length - 1)
                  summary += "; ";
            }
            summary += "\n\n";
         }
      }
      summary += THUMBS + getYesNo(useThumbnails) + '\n';
      summary += EMBED + getYesNo(embedFonts) + '\n';
      summary += HELPDOC + getYesNo(createHelpDocument) + "\n";
      summary += FULLSCREEN + getYesNo(startInFullScreen) + "\n";
      summary += CLIPSONSLIDES + getYesNo(showClipsOnSlides) + "\n\n";
      
      return summary;
   }
}
