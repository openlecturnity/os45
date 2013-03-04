package imc.lecturnity.converter;

import java.awt.*;
import java.io.File;
import java.io.FilenameFilter;
import java.util.*;

import imc.lecturnity.converter.StreamInformation;
import imc.lecturnity.converter.StreamingMediaConverter;
import imc.lecturnity.converter.wizard.PublisherWizardData;
import imc.lecturnity.util.NativeUtils;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.VideoClipInfo;
import imc.epresenter.player.audio.Audio2Wav;
import imc.epresenter.converter.ConversionDisplay;


/******************************************************************************/

public class WindowsMediaConverter extends StreamingMediaConverter
{
   // Are the used DLLs available?
   private static boolean engineLoaded_ = false;

   static
   {
      engineLoaded_ = true;

      try
      {
         System.loadLibrary("avitowmv");
      }
      catch (UnsatisfiedLinkError e)
      {
         // this may also fail for example if there is no wmvcore.dll
         // which is used by avitowmv.dll
         
         System.out.println("!!! avitowmv.dll not found or not loadable (dependencies?) !!!\n");
         engineLoaded_ = false;
      }
   }


   // Native functions for the JNI
   public static native WmpProfileInfo[] getWmp64Profiles();
   public static native WmpProfileInfo[] getWmp71Profiles();
   public static native WmpProfileInfo[] getWmp80Profiles();
   public static native WmpProfileInfo getProfileInfoFromFile(String sFileName);
   public static native boolean is7erCodecsInstalled();
   public static native boolean is9erCodecsInstalled();
   public static native float getCurrentAVProgress();
   public static native long getFccHandler();
   public static native String getProfileDirectory();
   private native int convertToWindowsMedia(int argc, String[] argv);
   private native void setVideoOutputSize(int width, int height);
   //private native int getVideoWidth(); //obsolete
   //private native int getVideoHeight(); //obsolete
   private native long getTotalPresentationTime();
   //private native int setVideoClips(VideoClipInfo[] multiVideoClips); //obsolete
   private native int setClipFileNames(String[] clipFileNames);
   private native int setClipTimestamps(long[] clipTimestamps);
   private native int setStillImageName(String stillImageName);
   private native void requestCancel();
   private native int createProfile(String sProfileFileName,
                                    StreamInformation siAudioInfo, 
                                    StreamInformation siVideoInfo);


   // Private Members
   private PublisherWizardData m_pwData = null;
   private boolean m_bUseAdvancedAudioSuffix = false;
   
   private ArrayList m_aWmFilenames = new ArrayList(10);
   private String m_strStreamingTargetPath = null;
   

   /***************************************************************************/

   /**
    * Constructor
    *
    *    @param: PublisherWizardData data: the data from the Publisher Wizard.
    *    @param: FileResources resources: the file resourses. 
    *    @param: ConversionDisplay display: the interaction with a progress bar. 
    */
   public WindowsMediaConverter(PublisherWizardData pwData,
                                FileResources resources,
                                ConversionDisplay display)
   {
      super(pwData, resources, display);

      m_pwData = pwData;
      
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      DocumentData dd = m_pwData.GetDocumentData();
     
      m_bUseAdvancedAudioSuffix = 
         ps.GetBoolValue("bWmUseAdvancedSuffix") && dd.HasClips() 
         && ps.GetBoolValue("bShowClipsOnSlides") && !dd.HasNormalVideo();
   }

   /***************************************************************************/

   /**
    * Returns <i>true</i> if the native DLLs could be loaded.
    */
   public static boolean isEngineLoaded()
   {
      try
      {
         // give it a second chance; maybe a redistribution package was installed
         
         System.loadLibrary("avitowmv");
         
         engineLoaded_ = true;
      }
      catch (UnsatisfiedLinkError e) {}
      
      return engineLoaded_;
   }

   /***************************************************************************/

   /**
    * Gets the custom profile file names (including extension ".prx")
    * 
    *    @return String[]: String array with the file names of the custom profiles.
    */
   public static String[] getCustomProfileNames()
   {
      // Get the WM Profile Directory
      String strProfileDirectory = getProfileDirectory();
      File wmFile = new File(strProfileDirectory);
      // Profiles have the extension ".prx"
      String[] wmProfiles = wmFile.list(new ExtensionFilenameFilter("prx"));

      // Debug Info
      //System.out.println("WM Profile Directory: " + strProfileDirectory);
      //for (int i = 0; i < wmProfiles.length; ++i)
      //   System.out.println("- " + wmProfiles[i]);
      //System.out.println("");
         
      return wmProfiles;
   }

   /***************************************************************************/

   /**
    * Starts the Converting routine.
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */

   public int startConvert()
   {
      int res = SUCCESS;

      if (!engineLoaded_)
      {
         System.out.println("!!! DLL NOT FOUND !!!\n");
         return DLL_NOT_FOUND;
      }

      res = super.startConvert();

      
      return res;
   }

   /***************************************************************************/

   /**
    * Cancels the Converting process
    */
   public void cancelConvert()
   {
      //m_nConversionStatus = CANCELLED;
      super.cancelConvert();
      requestCancel();
   }

   /***************************************************************************/

   protected int convertAudioAndVideo()
   {
      return convertAudioAndVideo(false);
   }
   
   /**
    * Converts Audio and/or Video into WindowsMedia.
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int convertAudioAndVideo(boolean bDoClipsOnly)
   {
      int res = SUCCESS;
      int iPossibleWarning = SUCCESS;
      
      // Check for AIF audio file
      if (!bDoClipsOnly)
         res = checkAudioFile(); // possibly creates temporary wav file
      if (res != SUCCESS)
         return res;


      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      DocumentData dd = m_pwData.GetDocumentData();
      
      StreamInformation siStreamAudio = dd.GetAudioStreamInfo();      
      StreamInformation siStreamVideo = dd.GetVideoStreamInfo();
      StreamInformation siStreamClip = dd.GetClipsStreamInfo();
      
      // if there are clips but no real video: use an sg codec for the "video"
      // there could be a still image of course, but that is no problem for an sg codec
      boolean bMakeSgProfile = siStreamClip != null && !ps.GetBoolValue("bShowClipsOnSlides");      
   
      String strOutputPrefix = GetOutputPrefix(); // ??? see above
      String strOutputSuffix = m_pwData.m_strWmSuffix;
      m_strStreamingTargetPath = ps.GetPathValue("strTargetDirStreamingServer");
      boolean bHasNormalVideo = dd.HasNormalVideo();
      boolean bIsDenver = dd.IsDenver();
      long lVideoOffset = -dd.GetVideoOffsetMs(); // ???
      VideoClipInfo[] aClipInfos = dd.GetVideoClipInfos();
      int iClipCount = aClipInfos != null ? aClipInfos.length : 0;
      String strClipPrefix = dd.GetPresentationPath();
    
      int iProfileType = ps.GetIntValue("iWmVideoProfileType");
      if (bDoClipsOnly || bIsDenver)
         iProfileType = ps.GetIntValue("iWmClipProfileType");
      String strProfileTypeSwitch = null;
      if (iProfileType == PublisherWizardData.PLAYER_64)
         strProfileTypeSwitch = "-wmp64";
      else if (iProfileType == PublisherWizardData.PLAYER_70)
         strProfileTypeSwitch = "-wmp71";
      else if (iProfileType == PublisherWizardData.PLAYER_80)
         strProfileTypeSwitch = "-wmp80";
   
      String strVideoFilename = dd.GetVideoFileName(true);;
      int iVideoOutputWidth = ps.GetIntValue("iVideoWidth");
      int iVideoOutputHeight = ps.GetIntValue("iVideoHeight");
      boolean bHasStillImage = dd.HasStillImage();
      String strStillimageName = dd.GetStillImageName(true);;
      String strTempAudioName = strOutputPrefix + "_aif.tmp";

      int iClipSizeType = ps.GetIntValue("iClipSizeType");
      boolean bClipSizeLimited = iClipSizeType == PublisherWizardData.CLIPSIZE_MAXSLIDE
         || iClipSizeType == PublisherWizardData.CLIPSIZE_CUSTOM;
      int iMaxClipWidth = ps.GetIntValue("iMaxCustomClipWidth");
      int iMaxClipHeight = ps.GetIntValue("iMaxCustomClipHeight");
      // ??? note that maybe different values are needed
      // after StreamingMediaConverter.initSlideSizes() (data_.maxOutputClipsWidth)
      // (see above)
      
      String strProfilePath = null;
      if (!bDoClipsOnly)
         strProfilePath = GetOutputPath() + "main"+System.currentTimeMillis()+".prx";
      else
         strProfilePath = GetOutputPath() + "clips"+System.currentTimeMillis()+".prx";
 
      StreamInformation siStreamOne = siStreamAudio;      
      StreamInformation siStreamTwo = siStreamVideo;
          
      if (bDoClipsOnly)
      {
         // use dummy audio stream for clips
         siStreamOne = new StreamInformation(-1, 6000, 8000, 1);
         
         // consider the dummy audio when specifying the video bitrate:
         siStreamClip.m_nBitrate -= siStreamOne.m_nBitrate;
         
         bMakeSgProfile = true;
      }
  
      // Create a profile for this data
      File profileFile = new File(strProfilePath);
      if (siStreamVideo == null && bMakeSgProfile) // no video and clips mixed; or clips only
            siStreamTwo = siStreamClip; // could still be null
      if (bDoClipsOnly || bIsDenver)
         siStreamTwo = siStreamClip;
      res = createProfile(profileFile.getAbsolutePath(), siStreamOne, siStreamTwo);
      profileFile.deleteOnExit();
      if (res != SUCCESS)
         return res;
      
      String[] aInputVideoNames = new String[] { strVideoFilename };
      
      if (bDoClipsOnly)
      {
         aInputVideoNames = new String[iClipCount];
         for (int i=0; i<iClipCount; ++i)
         {
            aInputVideoNames[i] = strClipPrefix + aClipInfos[i].clipFileName;
         }
      }
      if (bIsDenver)
         aInputVideoNames[0] = strClipPrefix + aClipInfos[0].clipFileName;
      
      for (int v=0; v<aInputVideoNames.length; ++v)
      {
         // Check, if process is cancelled
         if (m_nConversionStatus == CANCELLED)
         {
            res = CANCELLED;
            return res;
         }
         
         // Build the argument list
         ArrayList aParameters = new ArrayList(20);
      
         // Executable - Ignored / Not read by Parser
         aParameters.add("avitowmv");
      
         // Profile Index
         //if (strProfilePath.equals(""))
         //{
         //   // System Profile
         //   int nProfileIndex = data_.systemProfile;
         //   argv[argc] = "-p";
         //   argc++;
         //   argv[argc] = (new Integer(nProfileIndex)).toString();
         //   argc++;
         //}
         //else
         //{
         // Custom Profile
         aParameters.add("-pf");
         aParameters.add(strProfilePath);
         //}
      
      
         // Output File
         String strStreamingFile = null;
         aParameters.add("-o");
         if (m_bUseAdvancedAudioSuffix && !bDoClipsOnly)
            strStreamingFile = strOutputPrefix + ".wma";
         else
         {
            if (!bDoClipsOnly)
               strStreamingFile = strOutputPrefix + strOutputSuffix;
            else
               strStreamingFile = strOutputPrefix + "_clip" + v + strOutputSuffix;
         }
         aParameters.add(strStreamingFile);
         
         m_aWmFilenames.add(strStreamingFile); // for CopyStreamingServerFiles()

         // Audio Input File
         aParameters.add("-i");
         if (!bDoClipsOnly || bIsDenver)
            aParameters.add(super.m_strAudioFileName); // ???
         else
            aParameters.add(aInputVideoNames[v]);
            
         
         // Video Input File (optional)
         if ((bHasNormalVideo && !bDoClipsOnly) || bIsDenver)
         {
            aParameters.add("-j");
            aParameters.add(aInputVideoNames[0]);
         }

         // Video Offset (optional)
         if (bHasNormalVideo && (lVideoOffset != 0) && !bDoClipsOnly) // ??? && !bDoClipsOnly
         {
            System.out.println("*** Video Offset: " + lVideoOffset + "ms");
            aParameters.add("-t");
            aParameters.add((new Long(lVideoOffset)).toString());
         }
      
         // Setting the Profile Version
         if (strProfileTypeSwitch != null)
         {
            aParameters.add(strProfileTypeSwitch);
         }
         else
         {
            // do nothing
         }

         // Screen Grabbing Mode (optional)
         if (bMakeSgProfile && !bDoClipsOnly)
         {
            aParameters.add("-sgm"); // ??? why not in clips only mode?
         
            // Put the video clip list to the Converter
            res = HandleVideoClipsSeparated(aClipInfos, strClipPrefix);
            if (res != SUCCESS)
               return res;
         }
         
         // Special case: Evalution and Structured Screengrabbing document
         int nVersion = ConverterWizard.getVersionType();
         if ((nVersion == NativeUtils.EVALUATION_VERSION) && dd.isClipStructured()) {
            System.out.println("*** Eval Note on Structured SG clip");
            aParameters.add("-eval");
         }
         
         if (bDoClipsOnly)
         {
            // resize large clips to a maximum size?
            
            int iClipWidth = aClipInfos[v].videoWidth;
            int iClipHeight = aClipInfos[v].videoHeight;

            if (bClipSizeLimited)
            {
               double scaleX = (double)iMaxClipWidth / (double)iClipWidth;
               double scaleY = (double)iMaxClipHeight / (double)iClipHeight;
               double scaleClips = (scaleX < scaleY) ? scaleX : scaleY;
               if (scaleClips > (double)1.0)
                  scaleClips = (double)1.0;

               iClipWidth = (int)( (double)iClipWidth * scaleClips + 0.5 );
               iClipHeight = (int)( (double)iClipHeight * scaleClips + 0.5 );
            }

            // why side effect?
            aClipInfos[v].videoWidth = iClipWidth;
            aClipInfos[v].videoHeight = iClipHeight;
            System.out.println("Clip[" + v + "] size: " + iClipWidth + "x" + iClipHeight);
            
            iVideoOutputWidth = iClipWidth;
            iVideoOutputHeight = iClipHeight;
         }

         // Set the video output size:
         setVideoOutputSize(iVideoOutputWidth, iVideoOutputHeight);

         // Set the Still Image Name, if any
         if (bHasStillImage && !bDoClipsOnly)
            setStillImageName(strStillimageName);
         else
            setStillImageName("");
         
      
         // Copy the argument list to an array with the 
         // correct number of arguments
         String[] args = new String[aParameters.size()];
         aParameters.toArray(args);
      
         // Debug Info
         String strCommand = "";
         for (int i = 0; i < aParameters.size(); ++i)
         {
            strCommand += args[i];
            strCommand += " ";  
         }
         System.out.println("Command:\n" + strCommand + "\n");

         // -----------------------------
         // Now convert into WindowsMedia
         // -----------------------------

         res = convertToWindowsMedia(aParameters.size(), args);
         
         // Add the output file name to the File Container
         m_aStreamingFiles.add(strStreamingFile);
         
         // remember warning code if any
         if (res != SUCCESS)
         {
            if (res < 0)
               return res;
            else
            {
               // there was a warning message store that
               iPossibleWarning = res;
               res = SUCCESS;
            }
         }         
      } // for all video files

      // Get the used Codec
      m_pwData.m_strFccCodec = getFccCodec(); // ??? why side effect? why here?
         

      // Remove temporary audio file, if any
      File file = new File(strTempAudioName);
      if (file.exists())
         file.delete();
     
      if (!bDoClipsOnly)
      {
         // Get some other stuff from Audio/Video
         
         m_lDuration = getTotalPresentationTime();
         // Note: side effect; variable from StreamingMediaConverter

         System.out.println("Duration = " + m_lDuration + "ms");
         m_dimVideo.width = m_pwData.GetProfiledSettings().GetIntValue("iVideoWidth"); //getVideoWidth();
         m_dimVideo.height = m_pwData.GetProfiledSettings().GetIntValue("iVideoHeight"); //getVideoHeight();
         // ??? why side effect?
         System.out.println("Video size: " + m_dimVideo.width + " x " + m_dimVideo.height);
      }

      if (res == SUCCESS && iPossibleWarning != SUCCESS)
         return iPossibleWarning;
      else
         return res;
   }
   
   /***************************************************************************/

   /**
    * Converts Clips into Windows Media.
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int convertClipsOnly()
   {
      System.out.println("");
      System.out.println("- In convertClipsOnly():");
      
      return convertAudioAndVideo(true);

      /*
      int res = SUCCESS;
      int possibleWarning = res;
      
      
      // Create a profile for all clips
      File profileFile = new File(m_strPathName + "clips"+System.currentTimeMillis()+".prx");
      StreamInformation dummyAudioInfo = new StreamInformation(-1, 6000, 8000, 1);
      
      // consider the dummy audio when specifying the video bitrate:
      data_.siClipsStreamInfo.m_nBitrate -= dummyAudioInfo.m_nBitrate;
      
      res = createProfile(profileFile.getAbsolutePath(), dummyAudioInfo, data_.siClipsStreamInfo);
      profileFile.deleteOnExit();
      if (res != SUCCESS)
         return res;
      data_.clipCustomProfileFile = profileFile.getAbsolutePath();
      
      
      int size = data_.multiVideoClips.length;
      String[] clipFilenames = new String[size];

      for (int i = 0; i < size; i++)
      {
         // Check, if process is cancelled
         if (nProgressStatus == CANCELLED)
         {
            res = CANCELLED;
            return res;
         }

         // Add the input path to the filenames
         clipFilenames[i] = m_strInputPathName + data_.multiVideoClips[i].clipFileName;

         // Build the argument list
         int max_argc = 25;   // A maximum of 25 arguments
         String[] argv = new String[max_argc];
   
         int argc = 0;        // Actual number of arguments
         
         // Executable - Ignored / Not read by Parser
         argv[argc] = "avitowmv";
         argc++;
         
         // Profile Index
         if (data_.clipCustomProfileFile.equals(""))
         {
            // System Profile
            int nProfileIndex = data_.clipSystemProfile;
            argv[argc] = "-p";
            argc++;
            argv[argc] = (new Integer(nProfileIndex)).toString();
            argc++;
         }
         else
         {
            // Custom Profile
            argv[argc] = "-pf";
            argc++;
            argv[argc] = data_.clipCustomProfileFile;
            argc++;
         }
         
         
         // Output File
         argv[argc] = "-o";
         argc++;
         argv[argc] = m_strPathFileName 
                      + "_clip" + i + tpcAll.strWmSuffix;
         argc++;
   
         // Clip Input File 
         argv[argc] = "-i";
         argc++;
         argv[argc] = clipFilenames[i];
         argc++;
   
         // Setting the Profile Version
         if (data_.clipSystemProfileType == WmpConvertWizardData.PLAYER_64)
         {
            argv[argc] = "-wmp64";
            argc++;
         }
         else if (data_.clipSystemProfileType == WmpConvertWizardData.PLAYER_70)
         {
            argv[argc] = "-wmp71";
            argc++;
         }
         else if (data_.clipSystemProfileType == WmpConvertWizardData.PLAYER_80)
         {
            argv[argc] = "-wmp80";
            argc++;
         }
         else
         {
            // do nothing
         }

         // Copy the argument list to an array with the 
         // correct number of arguments
         String[] args = new String[argc];
         for (int j = 0; j < argc; ++j)
            args[j] = argv[j];
         
         // Debug Info
         String strCommand = "";
         for (int j = 0; j < argc; ++j)
         {
            strCommand += args[j];
            strCommand += " ";  
         }
         System.out.println("Command:\n" + strCommand + "\n");
   
         setStillImageName("");
   
         // Read the clip output size:
         int clipWidth = data_.multiVideoClips[i].videoWidth;
         int clipHeight = data_.multiVideoClips[i].videoHeight;

         // Do we have to resize very large clips to a maximum resolution?
         if ( (data_.clipSizeType == data_.CLIPSIZE_MAXSLIDE)
            || ((data_.clipSizeType == data_.CLIPSIZE_CUSTOM)) )
         {
            double scaleX = (double)data_.maxOutputClipsWidth / (double)clipWidth;
            double scaleY = (double)data_.maxOutputClipsHeight / (double)clipHeight;
            double scaleClips = (scaleX < scaleY) ? scaleX : scaleY;
            if (scaleClips > (double)1.0)
               scaleClips = (double)1.0;

            clipWidth = (int)( (double)clipWidth * scaleClips + 0.5 );
            clipHeight = (int)( (double)clipHeight * scaleClips + 0.5 );
         }

         data_.multiVideoClips[i].videoWidth = clipWidth;
         data_.multiVideoClips[i].videoHeight = clipHeight;
         System.out.println("Clip[" + i + "] size: " + clipWidth + "x" + clipHeight);
         
         // Set the clip output size:
         setVideoOutputSize(clipWidth, clipHeight);

         // -----------------------------
         // Now convert into WindowsMedia
         // -----------------------------

         res = convertToWindowsMedia(argc, args);
   
         // Get the used Codec
         data_.fccCodec = getFccCodec();

         // Add the output file name to the File Container
         streamingFiles.add(m_strPathName + m_strFileName + "_clip" + i + tpcAll.strWmSuffix); 
         
         // Check, if the conversion of the clip was successfull
         // If not, return error code 
         if (res != SUCCESS)
         {
            if (res < 0)
               return res;
            else
            {
               // there was a warning message store that
               possibleWarning = res;
               res = SUCCESS;
            }
         }

         // Write the corresponding metafile (ASX) for the clip
         writeMetaFileForClipIndex(i);
         
      } // for all clips

      if (res == SUCCESS && possibleWarning != SUCCESS)
         return possibleWarning;
      else
         return res;
         */
   }
   
   /***************************************************************************/

   /**
    * This routine sets the keywords for the (HTML-)Parser
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   /*
   protected int setKeywordsForParser()
   {
      return super.setKeywordsForParser(); // returns always SUCCESS?
   }
   */
   
   /***************************************************************************/

   /**
    * This routine checks if the Windows Media file can be written
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int checkStreamingFileWritable()
   {
      int res = SUCCESS;
      
      String strOutputPrefix = GetOutputPrefix();
      String strOutputSuffix = m_pwData.m_strWmSuffix; // can be ".wm" or ".wmv"

      String strOutFile = strOutputPrefix + strOutputSuffix;
      if (m_bUseAdvancedAudioSuffix)
         strOutFile = strOutputPrefix + ".wma";
      
      return CheckStreamingFileWriteable(strOutFile);
   }
     
   /***************************************************************************/

   /**
    * This routine copies the Files to the WindowsMedia Server directory. 
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int copyStreamingServerFiles()
   {
      // called "externally" from StreamingMediaConverter
      if (m_aWmFilenames != null && m_strStreamingTargetPath != null)
         return CopyStreamingServerFiles(m_aWmFilenames, m_strStreamingTargetPath);
      else
         return NO_WMSERVER_COPY;
   }
   
   protected ArrayList getStreamingServerFileNames()
   {
      return m_aWmFilenames;
   }

   /*
   private int copyStreamingServerFiles(ArrayList aFilenames, String strTargetPath)
   {
      int res = SUCCESS;
      
      // Reset progress bar
      int currentProgress = 0;
      display.displayCurrentFileProgress(currentProgress);

      for (int i=0; i<aFilenames.size() && res == SUCCESS; ++i)
      {
         String strInputFile = (String)aFilenames.get(i);
         String strFilename = new File(strInputFile).getName();
         String strOutputFile = strTargetPath + strFilename;
         
         // Copy the WindowsMedia file to the "server" directory
         res = moveFile(strInputFile, strOutputFile);
         
         // Update progress bar
         currentProgress++;
         display.displayCurrentFileProgress(currentProgress);
      }
      */
      /*     
      //String fileIn, fileOut;
       // Copy the WindowsMedia file
      fileIn = m_strPathName + m_strFileName + tpcAll.strWmSuffix;
      fileOut = m_strStreamingTargetPath + m_strFileName + tpcAll.strWmSuffix;

      ///res = copyFile(fileIn, fileOut);
      res = moveFile(fileIn, fileOut);
      
      // Update progress bar
      currentProgress++;
      display.displayCurrentFileProgress(currentProgress);


      // Do we have to copy some clips?
      if ( (res == SUCCESS) && (data_.separateClipsFromVideo) )
      {
         int size = data_.multiVideoClips.length;

         // Copy the clip files
         for (int i = 0; i < size; i++)
         {
            fileIn = m_strPathName + m_strFileName + "_clip" + i + tpcAll.strWmSuffix;
            fileOut = m_strStreamingTargetPath + m_strFileName + "_clip" + i + tpcAll.strWmSuffix;

            if (res == SUCCESS)
            {
               ///res = copyFile(fileIn, fileOut);
               res = moveFile(fileIn, fileOut);

                  // Update progress bar
                  currentProgress++;
                  display.displayCurrentFileProgress(currentProgress);
            }
         }
      }
      */
/*
      if (res == SUCCESS)
         return SUCCESS;
      else
         return NO_WMSERVER_COPY;
   }
   */

   /***************************************************************************/

   /**
    * Get the used/desired fourcc codec
    */
   protected String getFccCodec()
   {
      long fccHandler = getFccHandler();
      return super.GetFccCodecString((int)fccHandler);
      
      /*
      char[] fcc = new char[4];
      fcc[0] = (char)( (fccHandler)     &~(~0<<8) );
      fcc[1] = (char)( (fccHandler>>8)  &~(~0<<8) );
      fcc[2] = (char)( (fccHandler>>16) &~(~0<<8) );
      fcc[3] = (char)( (fccHandler>>24) &~(~0<<8) );
      
      String fccCodec = "";
      for (int i = 0; i < 4; ++i)
         fccCodec += fcc[i];
      System.out.println("fccCodec: " + fccCodec + "\n");
      
      return fccCodec;
      */
   }
   
   /***************************************************************************/

   /**
    * Read the video clip info and set filenames and timestamps to the Converter
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int ReadAndSetVideoClips(String[] aClipFilenames, long[] aClipTimestamps)
   {
      // is called from StreamingMediaConverter.HandleVideoClipsSeparated()
      
      int res = SUCCESS;
      
      /*
      int size = aClips.length;
      String[] clipFilenames = new String[size];
      long[] clipTimestamps = new long[size];
      
      // Fill the arrays
      for (int i = 0; i < size; ++i)
      {
         // Add the input path to the filenames
         clipFilenames[i] = strSourcePath + aClips[i].clipFileName;
         clipTimestamps[i] = aClips[i].startTimeMs;
      }
      */
      
      // Call 2 JNI functions to put the data to the Converter
      if (res == SUCCESS)
         res = setClipFileNames(aClipFilenames);
      if (res == SUCCESS)
         res = setClipTimestamps(aClipTimestamps);
     
      return res;
   }
  
   /***************************************************************************/

   public static class ExtensionFilenameFilter implements FilenameFilter
   {
      private String extension;

      public ExtensionFilenameFilter(String extension)
      {
         this.extension = extension.toLowerCase();
      }

      public boolean accept(File file, String name)
      {
         return name.toLowerCase().endsWith("." + extension);
      }
   }

}
