package imc.lecturnity.converter;

import imc.epresenter.filesdk.ConsistencyChecker;
import imc.epresenter.filesdk.PresentationData;
import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.Metadata;
import imc.epresenter.filesdk.VideoClipInfo;
import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.converter.wizard.PublisherWizardData;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.wizards.WizardPanel;

import java.awt.Dimension;
import java.io.File;
import java.io.IOException;
import javax.swing.*;

public class DocumentData
{
   private boolean         m_bIsPresentationDataRead  = false;

   private String          m_strPresentationFileName  = "";
   private String          m_strAudioFileName         = "";
   private String          m_strCodepage              = "cp1252";

   private String          m_strEvqFileName           = "";
   private String          m_strObjFileName           = "";
   private Dimension       m_WhiteboardDimension      = null;

   private boolean         m_bContainsNormalVideo     = false;
   private String          m_strVideoFileName         = "";
   private VideoClipInfo   m_VideoInfo                = null;
   private Dimension       m_VideoDimension           = null;
   private int             m_iVideoOffsetMs           = 0;
   private int             m_iVideoHelperIndex        = 0;

   private boolean         m_bContainsMultipleVideos  = false;
   private VideoClipInfo[] m_aVideoClipInfos          = null;

   private boolean         m_bContainsStillImage      = false;
   private String          m_strStillImageFileName    = "";

   private Metadata        m_Metadata                 = null;
   private String          m_strMetadataFileName      = "";

   private StreamInformation m_AudioStreamInfo        = null;
   private StreamInformation m_VideoStreamInfo        = null;
   private StreamInformation m_ClipsStreamInfo        = null;

   private int             m_iDocumentType            = PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;

   /** Parent Panel (necessary for error messages, etc.) */
   private WizardPanel     m_parentPanel              = null;

   // PZI: support structured clips
   private boolean m_bIsClipStructured = false;
   public boolean isClipStructured() {
      return m_bIsClipStructured;
   }
   // PZI: handle automated structuring and reset
   private boolean m_bIsClipStructuredSimulated = false;
   
   private boolean m_bAddClipStructure = false;
   public boolean isAddClipStructure() {
       return m_bAddClipStructure;
   }
   public void setAddClipStructure(boolean bAddClipStructure) {
       m_bAddClipStructure = bAddClipStructure;
   }

   private static String   ERROR_MSG = "[!]";
   private static String   ERR_NOMARK = "[!]";
   private static String   DT_ENTERPRISE = "[!]";
   private static String   DT_EVAL = "[!]";
   private static String   DT_CAMPUS = "[!]";
   private static String   WRONG_DOCTYPE1 = "[!]";
   private static String   WRONG_DOCTYPE2 = "[!]";
   private static String   VIDEO_2GB = "[!]";

   static 
   {
      try
      {
         Localizer l = new Localizer(
            "/imc/lecturnity/converter/DocumentData_", "en");

         ERROR_MSG                     = l.getLocalized("ERROR_MSG");
         ERR_NOMARK                    = l.getLocalized("ERR_NOMARK");
         DT_ENTERPRISE                 = l.getLocalized("DT_ENTERPRISE");
         DT_EVAL                       = l.getLocalized("DT_EVAL");
         DT_CAMPUS                     = l.getLocalized("DT_CAMPUS");
         WRONG_DOCTYPE1                = l.getLocalized("WRONG_DOCTYPE1");
         WRONG_DOCTYPE2                = l.getLocalized("WRONG_DOCTYPE2");
         VIDEO_2GB                     = l.getLocalized("VIDEO_2GB");
      }
      catch (IOException exc)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }


   // Constructor
   public DocumentData()
   {
      this(null);
   }

   public DocumentData(WizardPanel parent)
   {
      m_parentPanel = parent;
      // ??? WizardPanels change often, so a specific one makes no sense in the constructor.
   }
   

   /** Read Document Data from Presentation file name. Returns 'true' if successful, otherwise 'false'. */
   public boolean ReadDocumentDataFromPresentationFile(String strPresentationFileName)
   {
     return ReadDataFromPresentationFile(strPresentationFileName);
   }

   // SET methods
   public void SetPresentationFileName(String strPresentationFileName)
   {
      m_strPresentationFileName = strPresentationFileName;
   }

   public void SetVideoOffsetMs(int iVideoOffsetMs)
   {
      m_iVideoOffsetMs = iVideoOffsetMs;
   }

   public void SetVideoDimension(int iVideoWidth, int iVideoHeight)
   {
      m_VideoDimension = new Dimension(iVideoWidth, iVideoHeight);
   }

   public void SetVideoHelperIndex(int iVideoHelperIndex)
   {
      m_iVideoHelperIndex = iVideoHelperIndex;
   }

   public void SetVideoFileName(String strVideoFileName)
   {
      m_bContainsNormalVideo = true;
      m_strVideoFileName = strVideoFileName;
      m_bContainsStillImage = false;
      m_strStillImageFileName = "";
   }

   public void SetStillImageName(String strStillImageFileName)
   {
      m_bContainsNormalVideo = false;
      m_strVideoFileName = "";
      m_bContainsStillImage = true;
      m_strStillImageFileName = strStillImageFileName;
      m_VideoDimension = new Dimension(-1, -1);
   }

   public void ClearVideoAndStillImage()
   {
      m_bContainsNormalVideo = false;
      m_strVideoFileName = "";
      m_bContainsStillImage = false;
      m_strStillImageFileName = "";
      m_VideoDimension = new Dimension(-1, -1);
   }

   public void SetMetadataFileName(String strMetadataFileName)
   {
      m_strMetadataFileName = strMetadataFileName;
   }

   public void SetAudioStreamInfo(StreamInformation audioStreamInfo)
   {
      m_AudioStreamInfo = new StreamInformation(audioStreamInfo);
   }

   public void SetVideoStreamInfo(StreamInformation videoStreamInfo)
   {
      m_VideoStreamInfo = new StreamInformation(videoStreamInfo);
   }

   public void SetClipsStreamInfo(StreamInformation clipsStreamInfo)
   {
      m_ClipsStreamInfo = new StreamInformation(clipsStreamInfo);
   }

   // GET methods
   public boolean GetIsPresentationDataRead()
   {
      return m_bIsPresentationDataRead;
   }

   public String GetPresentationFileName()
   {
      return m_strPresentationFileName;
   }
   
   public String GetPresentationPath()
   {
      String strPath = new File(m_strPresentationFileName).getParentFile().getPath();
      if (!strPath.endsWith(File.separator))
         strPath += File.separator;
      return strPath;
   }

   public String GetAudioFileName()
   {
      return GetAudioFileName(false);
   }
   
   public String GetAudioFileName(boolean bFullPath)
   {
      String strAudio = m_strAudioFileName;
      if (bFullPath)
         strAudio = new File(new File(m_strPresentationFileName).getParentFile(), m_strAudioFileName)+"";

      return strAudio;
   }

   public String GetCodepage()
   {
      return m_strCodepage;
   }

   public int GetDocumentType()
   {
      return m_iDocumentType;
   }
   
   public boolean IsDenver()
   {
      return m_iDocumentType == PublisherWizardData.DOCUMENT_TYPE_DENVER;
   }

   public String GetEvqFileName()
   {
      return GetEvqFileName(false);
   }
   
   public String GetEvqFileName(boolean bFullPath)
   {
      String strEvq = m_strEvqFileName;
      if (bFullPath)
         strEvq = new File(new File(m_strPresentationFileName).getParentFile(), m_strEvqFileName)+"";

      return strEvq;
   }

   public String GetObjFileName()
   {
      return GetObjFileName(false);
   }
   
   public String GetObjFileName(boolean bFullPath)
   {
      String strObj = m_strObjFileName;
      if (bFullPath)
         strObj = new File(new File(m_strPresentationFileName).getParentFile(), m_strObjFileName)+"";

      return strObj;
   }

   public Dimension GetWhiteboardDimension()
   {
      return m_WhiteboardDimension;
   }

   public boolean HasNormalVideo()
   {
      return m_bContainsNormalVideo;
   }

   public String GetVideoFileName()
   {
      return GetVideoFileName(false);
   }
   
   public String GetVideoFileName(boolean bFullPath)
   {
      String strVideo = m_strVideoFileName;
      if (bFullPath)
         strVideo = new File(new File(m_strPresentationFileName).getParentFile(), m_strVideoFileName)+"";

      return strVideo;
   }

   public VideoClipInfo GetVideoInfo()
   {
      return m_VideoInfo;
   }

   public Dimension GetVideoDimension()
   {
      return m_VideoDimension;
   }

   public int GetVideoOffsetMs()
   {
      return m_iVideoOffsetMs;
   }

   public int GetVideoHelperIndex()
   {
      return m_iVideoHelperIndex;
   }

   public boolean HasMultipleVideos()
   {
      return m_bContainsMultipleVideos;
   }
   
   public boolean HasClips()
   {
      return m_bContainsMultipleVideos;
   }

   public VideoClipInfo[] GetVideoClipInfos()
   {
      return m_aVideoClipInfos;
   }

   public boolean HasStillImage()
   {
      return m_bContainsStillImage;
   }

   public String GetStillImageName()
   {
      return GetStillImageName(false);
   }
   
   public String GetStillImageName(boolean bFullPath)
   {
      String strStill = m_strStillImageFileName;
      if (bFullPath)
         strStill = new File(new File(m_strPresentationFileName).getParentFile(), m_strStillImageFileName)+"";

      return strStill;
   }

   public Metadata GetMetadata()
   {
      return m_Metadata;
   }

   public String GetMetadataFileName()
   {
      return m_strMetadataFileName;
   }

   public StreamInformation GetAudioStreamInfo()
   {
      return m_AudioStreamInfo;
   }

   public StreamInformation GetVideoStreamInfo()
   {
      return m_VideoStreamInfo;
   }

   public StreamInformation GetClipsStreamInfo()
   {
      return m_ClipsStreamInfo;
   }
   
   public void updateForStructuredScreengrabbing(ProfiledSettings ps) {
//       System.out.println("--------------- WAS -----------------");
//       System.out.println("m_strEvqFileName: "+m_strEvqFileName);
//       System.out.println("m_strObjFileName: "+m_strObjFileName);
//       System.out.println("m_WhiteboardDimension: "+m_WhiteboardDimension);
//       System.out.println("iClipSizeType: "+ps.GetIntValue("iClipSizeType"));
//       System.out.println("m_iDocumentType: "+m_iDocumentType);
//       System.out.println();
//       System.out.println("m_overrideDenver: "+m_bAddClipStructure);
//       System.out.println("IsDenver        : "+IsDenver());
//       System.out.println("hasStructuredClips: "+isClipStructured());
//       System.out.println("hasStructuredClips: "+m_bIsClipStructuredSimulated);
//       System.out.println("-------------------------------------");
      
      if(m_bAddClipStructure && IsDenver()) {
         // override Denver document type 
         // prepare structuring which will be performed on export
          
//         System.out.println("SET");

         // set flag as reminder on undo
         m_bIsClipStructuredSimulated = true;
          
         int iExportType = ps.GetIntValue("iExportType");
         switch (iExportType) {
           // only the following formats are supported
           case PublisherWizardData.EXPORT_TYPE_LECTURNITY:
           case PublisherWizardData.EXPORT_TYPE_WMT:
           case PublisherWizardData.EXPORT_TYPE_REAL:
           case PublisherWizardData.EXPORT_TYPE_FLASH:
               // set variables to simulate structured recording for publisher dialog
               // the structure will be computed AFTER the dialog and BEFORE the export
               
               // add EVQ and OBJ filenames
               m_strEvqFileName = m_strPresentationFileName.substring(0, m_strPresentationFileName.lastIndexOf('.')) + ".evq";
               m_strObjFileName = m_strPresentationFileName.substring(0, m_strPresentationFileName.lastIndexOf('.')) + ".obj";
   
               // adjust whiteboard size
               if (m_aVideoClipInfos != null && m_aVideoClipInfos.length > 0) {
                  // determine clip size
                  // size of first clip because currently only Denver docs with a single clip are supported
                  m_WhiteboardDimension = new Dimension(m_aVideoClipInfos[0].videoWidth, m_aVideoClipInfos[0].videoHeight);
               } else {
                  // set default size - should never happen
                  // TODO: what size???
                  m_WhiteboardDimension = new Dimension(1024, 768);
               }
               // whiteboard may be downscaled during export - adjust clip accordingly
               if(ps != null) 
                  ps.SetIntValue("iClipSizeType", PublisherWizardData.CLIPSIZE_MAXSLIDE);
               
               // determine document type
               m_iDocumentType = DetermineDocumentType();
               
               // set LPLibs status (not only used for Flash despite of name)
               LPLibs.flashSetSgStandAloneMode(false);

           default:
               // not supported format: keep denver document
               // TODO: may be called for all formats
         }
         
      } else if (!m_bAddClipStructure
                && (GetDocumentType() == PublisherWizardData.DOCUMENT_TYPE_AUDIO_CLIPS_PAGES)
                && (m_bIsClipStructured || m_bIsClipStructuredSimulated)) {
          // reset Denver document type 
          // prepare unstructuring which will be performed on export

//          System.out.println("UNSET");

          // clear flag used reminder on undo
          m_bIsClipStructuredSimulated = false;

          // add EVQ and OBJ filenames
          m_strEvqFileName = "";
          m_strObjFileName = "";
          
          // set empty whiteboard size
          m_WhiteboardDimension = new Dimension(0, 0);
          
          // whiteboard may be downscaled during export - adjust clip accordingly
          if(ps != null) 
             ps.SetIntValue("iClipSizeType", PublisherWizardData.CLIPSIZE_STANDARD);

          // determine document type
          m_iDocumentType = DetermineDocumentType();
          
          // set LPLibs status (not only used for Flash despite of name) 
          LPLibs.flashSetSgStandAloneMode(true);
      }

//      System.out.println("--------------- SET -----------------");
//      System.out.println("m_strEvqFileName: "+m_strEvqFileName);
//      System.out.println("m_strObjFileName: "+m_strObjFileName);
//      System.out.println("m_WhiteboardDimension: "+m_WhiteboardDimension);
//      System.out.println("iClipSizeType: "+ps.GetIntValue("iClipSizeType"));
//      System.out.println("m_iDocumentType: "+m_iDocumentType);
//      System.out.println();
//      System.out.println("m_overrideDenver: "+m_bAddClipStructure);
//      System.out.println("IsDenver        : "+IsDenver());
//      System.out.println("hasStructuredClips: "+isClipStructured());
//      System.out.println("hasStructuredClips: "+m_bIsClipStructuredSimulated);
//      System.out.println("-------------------------------------");
   }
 
   // Read Document Data from Presentation file name. Returns 'true' if successful, otherwise 'false'.
   private boolean ReadDataFromPresentationFile(String strPresentationFileName)
   {
      PresentationData pd = CheckForConsistency(strPresentationFileName);
      if (pd == null)
         return false;
      else
      {
         // PZI: clip structure flag
         m_bIsClipStructured = pd.hasStructuredClips;
                
         m_strPresentationFileName        = new File(strPresentationFileName)+"";
         m_strAudioFileName               = pd.audioFileName;
         m_strEvqFileName                 = (pd.eventFileName != null) ?
                                               pd.eventFileName : "";
         m_strObjFileName                 = (pd.objectFileName != null) ?
                                               pd.objectFileName : "";
         m_WhiteboardDimension            = (pd.whiteboardSize != null) ? 
                                               new Dimension(pd.whiteboardSize) : 
                                               new Dimension(0, 0);
         m_bContainsNormalVideo           = pd.containsNormalVideo;
         m_strVideoFileName               = pd.videoFileName;
         m_VideoInfo                      = pd.normalVideoInfo;
         m_VideoDimension                 = new Dimension(pd.videoWidth, pd.videoHeight);
         m_iVideoOffsetMs                 = pd.videoOffsetMs;
         m_iVideoHelperIndex              = pd.videoHelperIndex;
         m_bContainsMultipleVideos        = pd.containsMultipleVideos;
         m_aVideoClipInfos                = pd.multiVideoClips;
         if (pd.videoIsStillImage)
         {
            m_bContainsStillImage            = true;
            m_bContainsNormalVideo           = false;
            m_strStillImageFileName          = new String(m_strVideoFileName);
            m_strVideoFileName               = "";
         }
         m_Metadata                       = pd.metadata;
         m_strMetadataFileName            = pd.metadataFileName;

         
         m_bIsPresentationDataRead = true;
         // ??? already needed in DetermineDocumentType()
         m_iDocumentType                  = DetermineDocumentType();    


//         // Not used PresentationData variables:
//         hasVideoFile = pd.usesVideo;
//         hasVideoOffset = pd.hasVideoOffset;
//         masterOffset = pd.masterOffset;
//         isSgStandAloneMode = pd.isSgStandAloneMode;
         
      
         
         // Update the StreamInformation objects (only used by Flash and WM)
         // ??? Note: WM still overwrites this upon constructor of some dialogs.
         //
         
         // AUDIO
         if (GetAudioStreamInfo() == null)
         {
            StreamInformation siAudio = new StreamInformation(StreamInformation.AUDIO_STREAM);
            siAudio.m_nBitrate = 32000; // default
            SetAudioStreamInfo(siAudio);
         }
         // else no resetting of parameters needed

         // VIDEO
         if (HasNormalVideo() && GetVideoInfo() != null)
         {
            StreamInformation siVideo = new StreamInformation(
               -1, -1, GetVideoInfo().videoFramerateTenths, GetVideoDimension().width, GetVideoDimension().height);
            if (GetVideoStreamInfo() != null)
               siVideo.m_nBitrate = GetVideoStreamInfo().m_nBitrate;
            else
               siVideo.m_nBitrate = 800 * 1000; // default
            SetVideoStreamInfo(siVideo);
         }
            /* // ???
            else // ! GetVideoInfo() != null
            {
               // Known Bug: dd.GetVideoInfo() is created/copied in verifyNext()
               // of SelectPresentationWizardPanel.java, 
               // adding a video with SelectActionWizardPanel.java doesn't create it
            
               JOptionPane.showMessageDialog(this, g_Localizer.getLocalized("RELOAD_CONFIG_CHANGED"), 
                                             g_Localizer.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
               System.exit(1);
            }
            */


         // CLIPS
         if (HasClips())
         {
            int iFramerateTenths = 0;
            int iMaxClipWidth = 0, iMaxClipHeight = 0;
            boolean bClipsAreScreengrabbing = true;

            for (int i = 0; i < GetVideoClipInfos().length; ++i)
            {
               if (GetVideoClipInfos()[i].videoWidth > iMaxClipWidth)
                  iMaxClipWidth = GetVideoClipInfos()[i].videoWidth;
               if (GetVideoClipInfos()[i].videoHeight > iMaxClipHeight)
                  iMaxClipHeight = GetVideoClipInfos()[i].videoHeight;
               if (GetVideoClipInfos()[i].videoFramerateTenths > iFramerateTenths)
                  iFramerateTenths = GetVideoClipInfos()[i].videoFramerateTenths;
            }

            StreamInformation siClips = new StreamInformation(
               -1, -1, iFramerateTenths, iMaxClipWidth, iMaxClipHeight);
            if (GetClipsStreamInfo() != null)
               siClips.m_nBitrate = GetClipsStreamInfo().m_nBitrate;
            else
               siClips.m_nBitrate = 5000 * 1000; // default
            SetClipsStreamInfo(siClips);
         }
         


         // 2 GB check for video files
         if (m_bContainsNormalVideo || m_bContainsMultipleVideos)
         {
            File lrdFile = new File(m_strPresentationFileName);
            String lrdPath = lrdFile.getParentFile().toString();

            boolean bContainsTooLargeVideos = false;
            if (m_bContainsNormalVideo)
            {
               bContainsTooLargeVideos = bContainsTooLargeVideos || NativeUtils.isFileLarger2GB(lrdPath + File.separator + m_strVideoFileName);
            }
            else if (m_bContainsMultipleVideos && m_aVideoClipInfos != null)
            {
               for (int i = 0; i < m_aVideoClipInfos.length; ++i)
               {
                  imc.epresenter.filesdk.VideoClipInfo vci = m_aVideoClipInfos[i];
                  if (null != vci && null != vci.clipFileName)
                     bContainsTooLargeVideos = bContainsTooLargeVideos || NativeUtils.isFileLarger2GB(lrdPath + File.separator + vci.clipFileName);
               }
            }

            if (bContainsTooLargeVideos)
            {
               JOptionPane.showMessageDialog(m_parentPanel, VIDEO_2GB, ERROR_MSG, 
                                             JOptionPane.ERROR_MESSAGE);
               return false;
            }
         }
      }
      
      return true;
   }

   private PresentationData CheckForConsistency(String strLrdFile)
   {
      // Try to retrieve video file sizes for SG clips, too:
      boolean bSuccess = ConsistencyChecker.checkForConsistency(strLrdFile, true);
      PresentationData pd = null;

      if (bSuccess)
      {
         pd = ConsistencyChecker.getLastPresentationData();
         
         // this has to work:
         try
         {
            FileResources fileResources = FileResources.createFileResources(strLrdFile);
            int iDocType = fileResources.getDocumentType();
            String markString = fileResources.getMarkString();
			
            // As of version 1.6.0, the Lecturnity Publisher only supports
            // documents which have been marked (i.e. which have LAD audio)
            if (markString == null)
            {
               String strErrorMsg = ERR_NOMARK;
               JOptionPane.showMessageDialog(m_parentPanel, strErrorMsg, ERROR_MSG,
                                             JOptionPane.ERROR_MESSAGE);
               return null;
            }

            int iConverterType = ConverterWizard.getVersionType();

            boolean bIsCampusVersion = (iConverterType & NativeUtils.UNIVERSITY_VERSION) > 0;
            boolean bIsDocCampusVersion = (iDocType & NativeUtils.UNIVERSITY_VERSION) > 0;
            boolean bIsEvalVersion = (iConverterType & NativeUtils.EVALUATION_VERSION) > 0;
            boolean bIsDocEvalVersion = (iDocType & NativeUtils.EVALUATION_VERSION) > 0;

            boolean bDoPermitConvert = false;

            if (bIsEvalVersion)
               bDoPermitConvert = bIsDocEvalVersion;
            else
            {
               bDoPermitConvert = !bIsDocEvalVersion && 
                                   (!bIsCampusVersion ||
                                    (bIsCampusVersion && bIsDocCampusVersion));
            }

            if (!bDoPermitConvert)
            {
               String strDocVersion;
               if (bIsDocEvalVersion)
                  strDocVersion = DT_EVAL;
               else if (bIsDocCampusVersion)
                  strDocVersion = DT_CAMPUS;
               else
                  strDocVersion = DT_ENTERPRISE;

               String strErrorMsg = WRONG_DOCTYPE1 + " " + strDocVersion + " " + WRONG_DOCTYPE2;
               JOptionPane.showMessageDialog(m_parentPanel, strErrorMsg, ERROR_MSG, 
                                             JOptionPane.ERROR_MESSAGE);
               return null;
            }
         }
         catch (IOException e)
         {
            String strErrorMsg = e.toString();
            JOptionPane.showMessageDialog(m_parentPanel, strErrorMsg, ERROR_MSG, 
                                          JOptionPane.ERROR_MESSAGE);
            return null;
         }
      }
      else
      {
         String strErrorMsg = ConsistencyChecker.getLastConsistencyError();
         JOptionPane.showMessageDialog(m_parentPanel, strErrorMsg, ERROR_MSG, 
                                       JOptionPane.ERROR_MESSAGE);
         return null;
      }

      return pd;
   }

   private int DetermineDocumentType()
   {
      int iDocumentType = PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;

      // Document type can not be determined if Presentation file is not read already
      if (m_bIsPresentationDataRead)
      {
         boolean bHasPages = ( (m_WhiteboardDimension != null) 
                               && (m_WhiteboardDimension.width > 0) 
                               && (m_WhiteboardDimension.height > 0) );
         boolean bHasVideo = ( m_bContainsNormalVideo
                               && (m_strVideoFileName.length() > 0)
                               && (m_VideoDimension != null) 
                               && (m_VideoDimension.width > 0) 
                               && (m_VideoDimension.height > 0) );
         boolean bHasClips = m_bContainsMultipleVideos;
         boolean bHasStillImage = ( m_bContainsStillImage 
                                    && (m_strStillImageFileName.length() > 0) );

         if (bHasPages)
         {
            if (bHasVideo)
            {
               if (bHasClips)
                  iDocumentType = PublisherWizardData.DOCUMENT_TYPE_AUDIO_VIDEO_CLIPS_PAGES;
               else
                  iDocumentType = PublisherWizardData.DOCUMENT_TYPE_AUDIO_VIDEO_PAGES;
            }
            else if (bHasStillImage)
            {
               if (bHasClips)
                  iDocumentType = PublisherWizardData.DOCUMENT_TYPE_AUDIO_STILLIMAGE_CLIPS_PAGES;
               else
                  iDocumentType = PublisherWizardData.DOCUMENT_TYPE_AUDIO_STILLIMAGE_PAGES;
            }
            else
            {
               // No video, no still image
               if (bHasClips)
                  iDocumentType = PublisherWizardData.DOCUMENT_TYPE_AUDIO_CLIPS_PAGES;
               else
                  iDocumentType = PublisherWizardData.DOCUMENT_TYPE_AUDIO_PAGES;
            }
         }
         else
         {
            // Only "Denver" documents have no pages
            iDocumentType = PublisherWizardData.DOCUMENT_TYPE_DENVER;
         }
      }
      
      return iDocumentType;
   }
}