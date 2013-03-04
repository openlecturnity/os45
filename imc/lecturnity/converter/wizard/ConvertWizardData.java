package imc.lecturnity.converter.wizard;

import imc.epresenter.filesdk.Metadata;
import imc.epresenter.filesdk.VideoClipInfo;
import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.util.wizards.WizardData;

import java.awt.Dimension;

import javax.swing.JOptionPane;

import java.io.IOException;
import java.io.File;

public class ConvertWizardData extends WizardData
{
   protected static String YES = "[Yes]";
   protected static String NO = "[No]";

   public static String  INSTALL_DIR           = null;
   public static boolean INSTALL_DIR_AVAILABLE = false;
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/ConvertWizardData_",
             "en");
         
         YES = l.getLocalized("YES");
         NO = l.getLocalized("NO");

         String instDir = System.getProperty("install.path");
         if (instDir == null)
         {
            INSTALL_DIR_AVAILABLE = false;
            INSTALL_DIR = null;
         }
         else
         {
            INSTALL_DIR_AVAILABLE = true;
            if (!instDir.endsWith(File.separator))
               instDir += File.separatorChar;
            INSTALL_DIR = instDir;
         }
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "ConvertWizardData",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         System.exit(1);
      }
   }

   // The export types for the presentation
   public static final int EXPORT_TYPE_LECTURNITY    = 0;
   public static final int EXPORT_TYPE_REAL          = 1;
   public static final int EXPORT_TYPE_WMT           = 2;
   public static final int EXPORT_TYPE_FLASH         = 3;
   public static final int EXPORT_TYPE_VIDEO         = 4;

   /**
    * The presentation type is one of export types defined above:
    * EXPORT_TYPE_LECTURNITY, EXPORT_TYPE_REAL, EXPORT_TYPE_FLASH and EXPORT_WMT.
    */
   public int presentationType = EXPORT_TYPE_LECTURNITY;

   public static final int PANEL_VIDEO_SELECT = 0;
   public static final int PANEL_VIDEO_SYNC   = 1;
   
   /** One of the PANEL_VIDEO_* constants defined above: 
   * PANEL_VIDEO_SELECT and PANEL_VIDEO_SYNC */
   public int panelVideoType = PANEL_VIDEO_SELECT;
   
   /**
    * The file name of the recording document to convert (the LRD file
    * name), including the path and the extension.
    */
   public String presentationFileName;

   /**
    * NOTE: Only Lecturnity/Flash Presentation Documents. For target file
    * name specifications of the Streaming Export parts, see {@link
    * StreamingConvertWizardData StreamingConvertWizardData}.
    *
    * The target presentation document file name, without any
    * extensions.
    */
   public String targetFileName = null;

   /**
    * The file name of the audio file used in the presentation.
    */
   public String audioFileName = null;

   /**
    * The dimension of the whiteboard used in the presentation, in
    * pixels.
    */
   public Dimension whiteboardSize = null;

   /**
    * The file name of the object file (.OBJ) used in the
    * presentation.
    */
   public String objectFileName = null;

   /**
    * The file name of the Event list (.EVQ file) used in the
    * presentation.
    */
   public String eventFileName = null;

   /**
    * This variable is set to <i>true</i> if the presentation has any kind
    * of video file, <i>false</i> otherwise. This variable is also true,
    * if the presentation has a still image as a video file or if
    * it contains multiple videos, or a combination of any of these.
    *
    * @see #videoIsStillImage
    */
   public boolean hasVideoFile = false;

   /**
    * <i>true</i> if the video is not a video but a still image.
    * If <i>true</i>, {@link #containsNormalVideo containsNormalVideo}
    * has to be <i>false</i>.
    */
   public boolean videoIsStillImage = false;

   /**
    * <i>true</i> if the presentation contains a normal video. The
    * {@link #hasVideoFile hasVideoFile} member must also be <i>true</i>
    * for this to be true. In this case, the {@link #videoFileName videoFileName}
    * variable contains the AVI file name of the video.
    *
    * If this member is <i>true</i>, {@link #videoIsStillImage videoIsStillImage}
    * must be <i>false</i>.
    *
    * @see #hasVideoFile
    * @see #videoIsStillImage
    */
   public boolean containsNormalVideo = false;

   /**
    * <i>true</i> if the presentation contains multiple videos.
    * If <i>true</i>, the field {@link #multiVideoClips multiVideoClips}
    * contains valid data, otherwise it is <tt>null</tt>.
    */
   public boolean containsMultipleVideos = false;

   /**
    * If <i>true</i>, the document is a screen grabbing stand-alone
    * document. This means it does not contain any event queue or
    * object file. Neither is it probable that a <tt>Metadata</tt>
    * object exists.
    */
   public boolean isSgStandAloneMode = false;

   /**
    * The file name of the video file used in the presentation, or, if
    * {@link #videoIsStillImage videoIsStillImage} is set to
    * <i>true</i>, the file name of the image used.
    *
    * @see #containsNormalVideo
    * @see #videoIsStillImage
    */
   public String videoFileName = null;

   /**
    * If {@link #hasVideoFile hasVideoFile} and {@link containsNormalVideo
    * containsNormalVideo} are set to <i>true</i>, this member contains
    * is a {@link imc.epresenter.filesdk.VideoClipInfo VideoClipInfo}
    * instance describing the video used in this presentation.
    * Several other fields here contain some of the same information 
    * (but not all): e.g. videoWidth and videoHeight
    *
    * @see #hasVideoFile
    * @see #containsNormalVideo
    */
   public VideoClipInfo normalVideoInfo = null;
   
   /**
    * The width of the video file, if applicable. This member is only valid
    * if {@link #hasVideoFile hasVideoFile} is <i>true</i>, and both 
    * {@link #videoIsStillImage videoIsStillImage} and
    * {@link #containsMultipleVideos containsMultipleVideos} are
    * <i>false</i>.
    *
    * @see #hasVideoFile
    * @see #videoIsStillImage
    * @see #containsMultipleVideos
    */
   public int videoWidth = -1;

   /**
    * The height of the video file, if applicable. This member is only valid
    * if {@link #hasVideoFile hasVideoFile} is <i>true</i>, and both 
    * {@link #videoIsStillImage videoIsStillImage} and
    * {@link #containsMultipleVideos containsMultipleVideos} are
    * <i>false</i>.
    *
    * @see #hasVideoFile
    * @see #videoIsStillImage
    * @see #containsMultipleVideos
    */
   public int videoHeight = -1;

   /**
    * If {@link #hasVideoFile hasVideoFile} and {@link containsMultipleVideos
    * containsMultipleVideos} are set to <i>true</i>, this member contains
    * a list of {@link imc.epresenter.filesdk.VideoClipInfo VideoClipInfo}
    * instances describing the clips used in this presentation.
    *
    * @see #hasVideoFile
    * @see #containsMultipleVideos
    */
   public VideoClipInfo[] multiVideoClips = null;

   /**
    * If {@link #hasVideoFile hasVideoFile} is set to <i>true</i>,
    * this variable contains the index of the video helper in the
    * configuration (LRD) file.
    */
   public int videoHelperIndex = -1;

   /**
    * If the presentation has a video, this variable contains
    * information on the existance of a video/audio offset in the
    * presentation. If set to <i>true</i>, the video offset can be
    * read out in the variable {@link #videoOffsetMs videoOffsetMs}.
    */
   public boolean hasVideoOffset = false;

   /**
    * @see #hasVideoOffset
    */
   public int videoOffsetMs = 0;

   /**
    * The name of the metadata (LMD) file of the presentation, or
    * <tt>null</tt> if a metadata file does not exist (old-style
    * documents, AOF documents).
    */
   public String metadataFileName = null;

   /**
    * A {@link imc.epresenter.filesdk.Metadata Metadata} object for
    * the presentation, if such exists. May be <tt>null</tt>.
    */
   public Metadata metadata = null;

   /**
    * <b>Not used.</b> Has no effect at the moment.
    */
   public int masterOffset = 0;

   /**
    * This variable is <i>true</i> if the currect user is allowed to
    * write into the source directory of the presentation (LRD
    * file). This does not always have to be the case!
    */
   public boolean isSourceDirWritable = false;

   /**
    * The code page of the recording in such a form that it may
    * be used directly by the InputStreamReader.
    */
   public String codepage = "cp1252";
   
   protected String getYesNo(boolean b)
   {
      if (b)
         return YES;
      else
         return NO;
   }

   /**
    * States of 'standard navigation':
    * 0: enabled; 1: disabled; 2: hidden
    */
   public int       iNavigationControlBar          = 0;
   public int       iNavigationStandardButtons     = 0;
   public int       iNavigationTimeLine            = 0;
   public int       iNavigationTimeDisplay         = 0;
   public int       iNavigationDocumentStructure   = 0;
   public int       iNavigationPlayerSearchField   = 0;
   public int       iNavigationPlayerConfigButtons = 0;
   public int       iNavigationPluginContextMenu   = 0;

   /** A flag for the autostart mode. */
   public boolean   isAutostartMode  = false;


   public String getSummary()
   {
      return "";
   }

   public void fillOtherWizardData(ConvertWizardData otherData)
   {
      otherData.presentationType = presentationType;
      otherData.presentationFileName = presentationFileName;
      otherData.targetFileName = targetFileName;
      otherData.audioFileName = audioFileName;
      otherData.whiteboardSize = whiteboardSize;
      otherData.objectFileName = objectFileName;
      otherData.eventFileName = eventFileName;
      otherData.hasVideoFile = hasVideoFile;
      otherData.videoIsStillImage = videoIsStillImage;
      otherData.containsNormalVideo = containsNormalVideo;
      otherData.videoFileName = videoFileName;
      otherData.normalVideoInfo = normalVideoInfo;
      otherData.videoWidth = videoWidth;
      otherData.videoHeight = videoHeight;
      otherData.videoHelperIndex = videoHelperIndex;
      otherData.hasVideoOffset = hasVideoOffset;
      otherData.videoOffsetMs = videoOffsetMs;
      otherData.masterOffset = masterOffset;
      otherData.metadataFileName = metadataFileName;
      otherData.metadata = metadata;
      otherData.isSourceDirWritable = isSourceDirWritable;

      otherData.containsMultipleVideos = containsMultipleVideos;
      if (multiVideoClips != null)
         otherData.multiVideoClips = (VideoClipInfo[]) multiVideoClips.clone();
      else
         otherData.multiVideoClips = null;
      otherData.isSgStandAloneMode = isSgStandAloneMode;

      otherData.codepage = codepage;
      otherData.iNavigationControlBar = iNavigationControlBar;
      otherData.iNavigationStandardButtons = iNavigationStandardButtons;
      otherData.iNavigationTimeLine = iNavigationTimeLine;
      otherData.iNavigationTimeDisplay = iNavigationTimeDisplay;
      otherData.iNavigationDocumentStructure = iNavigationDocumentStructure;
      otherData.iNavigationPlayerSearchField = iNavigationPlayerSearchField;
      otherData.iNavigationPlayerConfigButtons = iNavigationPlayerConfigButtons;
      otherData.iNavigationPluginContextMenu = iNavigationPluginContextMenu;
      otherData.isAutostartMode = isAutostartMode;
      
      super.fillOtherWizardData(otherData);
   }
}
