package imc.lecturnity.converter;

import java.awt.Dimension;
import java.awt.Insets;

import imc.epresenter.filesdk.Metadata;


public class LPLibs
{
   static
   {
      try
      {
         System.loadLibrary("LPLibs");
         init();
      }
      catch (UnsatisfiedLinkError e)
      {
         javax.swing.JOptionPane.showMessageDialog
            (null,
             "The dynamic link library 'LPLibs.dll' was not found in the path. Reinstalling the application may help.",
             "Error",
             javax.swing.JOptionPane.ERROR_MESSAGE);
         System.exit(-1);
      }
   }
   
   
   /**
    * Warning and error codes:   
    * >0: Warning (Note: multiple warning codes are possible --> use 1, 2, 4, 8, 16, ... as flags).
    *  0: Success, everything was well done.
    * <0: Error.
    */
   public static final int WARN_GLYPH_NOT_FOUND     = 16;
   public static final int WARN_PAGE_TOO_SHORT      =  8;
   public static final int WARN_FONTSTYLES_IN_TTF   =  4;
   public static final int WARN_STOPMARK_LOSS       =  2;
   public static final int WARN_FONT_NOT_FOUND      =  1;
   public static final int SUCCESS                  =  0;
   public static final int UNSPECIFIC_ERROR         = -1;
   public static final int INPUT_FILE_MISSING       = -2;
   public static final int EMBED_FILE_MISSING       = -3;
   public static final int OPEN_FILE_ERROR          = -4;
   public static final int WHITEBOARD_CORRUPT       = -5;
   public static final int COMMAND_START_FAILED     = -6;
   public static final int COMMAND_EXECUTION_FAILED = -7;
   public static final int AUDIO_CONVERSION_FAILED  = -8;
   public static final int FONT_CONVERSION_FAILED   = -9;
   public static final int IMAGE_CONVERSION_FAILED  = -10;
   public static final int DOCUMENT_TOO_COMPLEX     = -11;
   public static final int VIDEO_FILTERS_MISSING    = -12;
   public static final int VIDEO_DECODING_FAILED    = -13;
   public static final int VIDEO_CONVERSION_FAILED  = -14;
   public static final int CLIP_CONVERSION_FAILED   = -15;
   public static final int PAGE_TOO_SHORT           = -16;
   public static final int AUDIO_8_16_KHZ_NOT_SUPPORTED = -17;  
   public static final int FLV_NO_OVERWRITE         = -18;
   public static final int TOO_MANY_OPEN_FILES      = -19;
   public static final int INTERACTIVE_PAGE_TOO_SHORT = -20;
   public static final int NO_MEMORY                = -21;
   public static final int NO_SWF_CREATED           = -22;
   public static final int NOT_ENOUGH_DISK_SPACE    = -23; 
   public static final int CANCELLED                = -99;
   
   
   // Flash Output Format
   public static final int FLASH_FORMAT_NORMAL     = 0;
   public static final int FLASH_FORMAT_SLIDESTAR  = 1;
   public static final int FLASH_FORMAT_PSP        = 2;
   public static final int FLASH_FORMAT_FLEX       = 3;
   
   // Standard Sony PSP Resolution
   public static final int PSP_STANDARD_WIDTH       = 480;
   public static final int PSP_STANDARD_HEIGHT      = 272;
   public static final int PSP_CONTENT_WIDTH        = 363;
   public static final int PSP_CONTENT_HEIGHT       = 272;

   // Telepointer types
   public static final int TELEPOINTER_TYPE_POINTER = 0;
   public static final int TELEPOINTER_TYPE_CURSOR  = 1;
   
   // 'Standard Navigation'
   public static final int NUMBER_OF_NAVIGATION_STATES = 8;
   // The following Strings are also defined in
   // - "TemplateKeywords.h" (LPLibs library)
   // - "LrdFile.h" (FileSdk library)
   // - "Document.java" (imc.epresenter.player)
   public static final String STR_NAVIGATION_NODE = "NAVIGATION";
   public static final String[] STR_NAVIGATION_SUBNODES = {
      "CONTROLBAR",
      "STANDARDBUTTONS",
      "TIMELINE",
      "TIMEDISPLAY",
      "DOCUMENTSTRUCTURE",
      "PLAYERSEARCHFIELD",
      "PLAYERCONFIGBUTTONS",
      "PLUGINCONTEXTMENU"
   };
   public static final String[] STR_NAVIGATION_STATES = {
      "enabled",  /* 0 */
      "disabled", /* 1 */
      "hidden"    /* 2 */
   };
   public static final String[] STR_NAVIGATION_KEYWORDS = {
      "NavigationControlBar", 
      "NavigationStandardButtons",
      "NavigationTimeLine",
      "NavigationTimeDisplay",
      "NavigationDocumentStructure",
      "NavigationPlayerSearchField",
      "NavigationPlayerConfigButtons",
      "NavigationPluginContextMenu"
   };


   public static native int init();
   public static native int cleanUp();
 
   public static native String[] getListOfInteractiveOpenedFiles(String evqFile, String objFile);
   public static native int lpdPrepareCopiesOfInteractiveOpenedFiles(
      String evqFile, String objFile, String outFile);

   public static native int scormInit();
   public static native int scormCleanUp();
   public static native String scormGetSettings();
   public static native int scormUseSettings(String strScormSettings);
   public static native int scormConfigure(java.awt.Window parent, boolean bDoNotSave);
   public static native int scormSetTargetFileName(String targetFile);
   public static native String scormGetTargetFilePath();
   public static native int scormSetIndexFileName(String realIndexFile, String indexFile);
   public static native int scormAddResourceFile(String realFileName, String fileName);
   public static native int scormAddMetadataKeyword(String keyword);
   public static native int scormAddGeneralKeyword(String keyword, String value);
   public static native int scormAddFullTextString(String string);
   public static native int scormCreatePackage(java.awt.Window parent);
   public static native int scormGetProgress();
   public static native int scormCancel();
   public static native String scormGetLanguage();
   public static native String scormGetVersion();
   public static native String scormGetStatus();
   public static native boolean scormGetFullTextAsKeywords();

   public static native int templateInit();
	public static native int templateCleanUp();
   public static native int templateReadSettings();
   public static native int templateReadSettingsFromFile(String strTemplatePath);
   public static native int templateUseSettings(String strProfileSettings);
   public static native String templateGetSettings();
   public static native String templateGetActivePath();
   public static native int templateConfigure(java.awt.Window parent, boolean bDoNotSave);
	public static native int templateAddKeyword(String keyword, String value);
	public static native String templateGetTitle();
	public static native String templateGetDescription();
   public static native String templateGetAuthor();
   public static native String templateGetCopyright();
	public static native boolean templateIsScormCompliant();
   public static native boolean templateIsUtfTemplate();
	public static native int templateDoParse(java.awt.Window parent);
	public static native int templateSetTargetDirectory(String targetDirectory);
	public static native String templateGetTargetDirectory();
   // Returns file count!
	public static native int templateGetFileCount();
	public static native String templateGetFile(int nIndex);
   public static native String templateGetIndexFile();
	public static native int templateSetDocumentType(String docType);
	public static native int templateSetScormEnabled(boolean bScormEnabled);
	public static native int flashSetScormEnabled(boolean bScormEnabled);
	public static native int templateSetTargetFormat(String targetFormat);
   public static native int templateGetMaxSlidesWidth(int screenWidth, int videoWidth, int stillImageWidth);
   public static native int templateGetMaxSlidesHeight(int screenHeight, int videoHeight, int stillImageHeight);
   public static native String templateGetGlobalSetting(String settingsName);
   public static native int[] templateGetStandardNavigationStates();
   public static native int templatePutStandardNavigationStatesToLrd(String lrdFile, int[] anNavigationStates);
	
   
   public static native int flashSetInputFiles(String lrdFile, String evqFile, 
                                               String objFile, String ladFile);
   public static native int flashSetInputAudioIsFromFlv(boolean bIsAudioFlv, String audioFlvFile);
//   public static native int flashSetInputAudioFile(String audioFile, String audioFlvFile);
   public static native int flashSetInputVideoFile(String videoFile, String videoFlvFile, 
                                                   int nVideoWidth, int nVideoHeight, boolean bDoPadding);
   public static native int flashSetNumberOfClips(int numOfClips);
   public static native int flashSetInputClipFile(int index, String clipFile, String clipFlvFile, 
                                                  int nClipWidth, int nClipHeight, boolean bDoPadding, 
                                                  long clipBeginMs, long clipEndMs);
   public static native int flashSetInputStillImageFile(String stillImageFile, int nWidth, int nHeight);
   public static native int flashSetOutputFile(String swfFile);
   public static native int flashSetRtmpPath(String rtmpPath);
   public static native int flashSetWbInputDimension(int nWidth, int nHeight);
   public static native int flashSetWbOutputDimension(int nWidth, int nHeight);
   public static native int flashSetMovieDimension(int nWidth, int nHeight);
   public static native int flashSetFlashVersion(int nFlashVersion);
   public static native int flashSetAutostartMode(boolean bAutostartMode);
   public static native int flashSetSgStandAloneMode(boolean bSgStandAloneMode);
   public static native int flashSetDrawEvalNote(boolean bDrawEvalNote);
   public static native int flashSetPspDesign(boolean bPspDesign);
   public static native int flashSetShowClipsInVideoArea(boolean bShowClipsInVideoArea);
   public static native int flashHideLecturnityIdentity(boolean bLecIdentityHidden);
   public static native int flashEnableOutputDir(String outputDir);
   public static native int flashResetAllDimensions();
   public static native int flashGetOutputWidth();
   public static native int flashGetOutputHeight();
   public static native int flashGetContentDimensions(Dimension dimMovieSize, Dimension dimVideoSizeIn, 
                                                      Dimension dimPageSizeIn, Dimension dimLogoSize, 
                                                      boolean bDisplayTitle, boolean bIsControlbarVisible, 
                                                      boolean bShowVideoOnPages, 
                                                      Dimension dimVideoSizeOut, Dimension dimPageSizeOut);
   public static native int flashObtainMovieInsets(Insets insets, Dimension dimWbSize, 
                                                   boolean bDisplayTitle, Dimension dimLogoSize, 
                                                   Dimension dimVideoSize, boolean bIsControlbarVisible);
   public static native int flashConvertImages();
   public static native int flashConvertFonts();
   public static native int flashConvertAudioToMp3(String strMp3File, int iAudioBitrate);
   public static native int flashSetStandardNavigationStates(int nNavigationControlBar, 
                                                             int nNavigationStandardButtons, 
                                                             int nNavigationTimeLine, 
                                                             int nNavigationTimeDisplay, 
                                                             int nNavigationDocumentStructure, 
                                                             int nNavigationPluginContextMenu);
   public static native int flashBuildMovie(int iBasicColor, int iBackColor, String sAuthor, String sTitle,
                                            int iTextColor, String sLogoFile, String sLogoUrl, 
                                            int iFlashFormatType, boolean bShowVideoOnPages);
   public static native int flashCreateSlidestar(String sThumbFiles);
   public static native int flashCreateFlexFiles(String strBaseUrl, String strStreamUrl, 
                                                 boolean bShowVideoOnPages, boolean bShowClipsInVideoArea, 
                                                 boolean bStripFlvSuffix, boolean bDoAutoStart, 
                                                 boolean bScormEnabled, 
                                                 boolean bUseOriginalSlideSize);
   public static native int flashPrepareScorm();
   public static native int flashCancel();
   public static native int flashCleanup();
   public static native String flashLastErrorDetail();
   public static native String flashGetFontListFontNotFound();
   public static native String flashGetFontListGlyphNotFound();
   public static native String flashGetFontListStylesNotSupported();
   public static native String flashGetPreloaderFilename();
   public static native String flashGetTooShortPagesTimes();
   public static native int flashShowColorDialog(int iCurrentColor);
   
   public static native int videoPrepare(String strInputLrd, String strOutputMp4,
                                         int iFrameRateTenth, int iVideoRateKbit, int iAudioRateKbit,
                                         int iOutputWidth, int iOutputHeight, boolean bEmbedClips,
                                         boolean bVideoInsteadOfPages, boolean bTargetIPod);
   public static native int videoConvertStart();
   public static native int videoConvertCancel();
   public static native int videoGetProgress();
   public static native int videoCleanup();
   
   /**
    * Performs the actual preparation only if bCombineWithClips is false.
    * 
    * @returns a negative value if an error occurs; can return VIDEO_FILTERS_MISSING
    */
   public static native int flvPrepare(String strInputAvi, int iVideoOffsetMs, 
                                       String strInputLad, String strOutputFlv,
                                       int iVideoRateKbit, int iAudioRateKbit,
                                       int iOutputWidth, int iOutputHeight, 
                                       boolean bCombineWithClips, boolean bIncreaseKeyframes,
                                       boolean bDoPadding, boolean bForVersion9);
   
   /**
    * Performs the complete preparation if bIsLastClip is true.
    *
    * @returns a negative value if an error occurs; can return VIDEO_FILTERS_MISSING
    */
   public static native int flvAddClip(String strInputAvi, int iClipOffsetMs, boolean bIsLastClip);
   public static native int flvConvertStart();
   public static native int flvConvertCancel();
   public static native boolean flvCheckOverwrite(String strOutputFlv);  
   
   /**
    * @returns a value between 0 .. 10000; can return a negative value when encountering an error
    */
   public static native int flvGetProgress();
   public static native int flvCleanup();   
}      
