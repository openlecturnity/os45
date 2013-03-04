package imc.lecturnity.converter;

import java.awt.Dimension;
import java.io.File;
import java.util.*;
import java.lang.*;

import imc.lecturnity.converter.StreamingMediaConverter;
import imc.lecturnity.converter.wizard.PublisherWizardData;
import imc.lecturnity.util.NativeUtils;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.VideoClipInfo;
import imc.epresenter.player.audio.Audio2Wav;
import imc.epresenter.converter.ConversionDisplay;


/******************************************************************************/

public class RealMediaConverter extends StreamingMediaConverter
{
   // Are the used DLLs available?
   private static boolean engineLoaded_ = false;

   static
   {
      engineLoaded_ = true;

      try
      {
         System.loadLibrary("Avcnvt");
      }
      catch (UnsatisfiedLinkError e)
      {
         System.out.println("!!! Avcnvt.dll not found !!!\n");
         engineLoaded_ = false;
      }
   }


   // Native functions for the JNI
   public static native long getFccHandler();
   public native float getAudioProgress();
   public native float getVideoProgress();
   private native int convertToRealMedia(int argc, String[] argv);
   private native int getVideoWidth();
   private native int getVideoHeight();
   private native void setVideoOutputSize(int width, int height);
   private native long getTotalPresentationTime();
   private native void setAudioOffset(long audioOffset);
   private native void setVideoOffset(long videoOffset);
   private native int setClipFileNames(String[] clipFileNames);
   private native int setClipTimestamps(long[] clipTimestamps);
   private native int setStillImageName(String stillImageName);
   private native void requestCancel();

   // Private Members
   private PublisherWizardData m_pwData = null;
   
   private ArrayList m_aRmFilenames = new ArrayList(10);
   private String m_strStreamingTargetPath = null;

   /***************************************************************************/

   /**
    * Constructor
    *
    *    @param: PublisherWizardData pwData: the data from the Publisher Wizard.
    *    @param: FileResources resources: the file resourses. 
    *    @param: ConversionDisplay display: the interaction with a progress bar. 
    */
   public RealMediaConverter(PublisherWizardData pwData,
                             FileResources resources,
                             ConversionDisplay display)
   {
      super(pwData, resources, display);

      m_pwData = pwData;
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

   /**
    * Converts Audio and/or Video into RealMedia.
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int convertAudioAndVideo()
   {
      return convertAudioAndVideo(false);
   }
   
   protected int convertAudioAndVideo(boolean bDoClipsOnly)
   {
      int res = SUCCESS;
      
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      DocumentData dd = m_pwData.GetDocumentData();
      
      boolean bHasNormalVideo = dd.HasNormalVideo();
      boolean bHasClips = dd.HasClips();
      boolean bSeparateClips = ps.GetBoolValue("bShowClipsOnSlides");;
      boolean bHasStillimage = dd.HasStillImage();
      boolean bIsDenver = dd.IsDenver();
      int iVideoQuality = ps.GetIntValue("iRealVideoQuality");

      if (ps.GetBoolValue("bRealAutoVideoQuality")) {
         // adjust video quality settings
         if (m_pwData.HasNormalVideo() || m_pwData.HasClips())
         {
            if (m_pwData.HasNormalVideo())
               ps.SetIntValue("iRealVideoQuality", 1); // smoothest motion
            else
               ps.SetIntValue("iRealVideoQuality", 2); // sharpest image
         }
         else
            ps.SetIntValue("iRealVideoQuality", 0); // normal
      }
      
      int iMinRealVersion = m_pwData.m_iMinRealVersion;
      long lVideoOffset = -dd.GetVideoOffsetMs(); // ???
      
      int iVideoOutputWidth = ps.GetIntValue("iVideoWidth");
      int iVideoOutputHeight = ps.GetIntValue("iVideoHeight");
      
      int iClipSizeType = ps.GetIntValue("iClipSizeType");
      boolean bClipSizeLimited = iClipSizeType == PublisherWizardData.CLIPSIZE_MAXSLIDE
         || iClipSizeType == PublisherWizardData.CLIPSIZE_CUSTOM;
      int iMaxClipWidth = ps.GetIntValue("iMaxCustomClipWidth");
      int iMaxClipHeight = ps.GetIntValue("iMaxCustomClipHeight");
      // ??? note that maybe different values are needed
      // after StreamingMediaConverter.initSlideSizes() (data_.maxOutputClipsWidth)
      // (see above)

      boolean[] abAudioBitrates = new boolean [] { ps.GetBoolValue("bRealBitrate0"),
         ps.GetBoolValue("bRealBitrate1"),
         ps.GetBoolValue("bRealBitrate2"), ps.GetBoolValue("bRealBitrate3"),
         ps.GetBoolValue("bRealBitrate4"), ps.GetBoolValue("bRealBitrate5"),
         ps.GetBoolValue("bRealBitrate6"), ps.GetBoolValue("bRealBitrate7"),
         ps.GetBoolValue("bRealBitrate8")
      };
      
      String strInputPath = dd.GetPresentationPath();
      String strVideoFilename = dd.GetVideoFileName(true);;
      String strStillimageName = dd.GetStillImageName(true);;
      String strPrefix = GetOutputPrefix();
      String strTempAudioName = strPrefix + "_aif.tmp";
      m_strStreamingTargetPath = ps.GetPathValue("strTargetDirStreamingServer");
      
      VideoClipInfo[] aClipInfos = dd.GetVideoClipInfos();        
      
      
      // Check for AIF audio file
      res = checkAudioFile();
      if (res != SUCCESS)
         return res;

      // Build a flag like "-t0,2,5"
      String strBandwidthsFlag = "-t";
      for (int b = 0; b < abAudioBitrates.length; ++b)
      {
         if (abAudioBitrates[b])
         {
            if (!strBandwidthsFlag.endsWith("t"))
               strBandwidthsFlag += ",";

            strBandwidthsFlag += b;
         }
      }
         
      String[] aInputVideos = new String[] { strVideoFilename };
      
      if (bDoClipsOnly)
      {
         aInputVideos = new String[aClipInfos.length];
         for (int i = 0; i < aInputVideos.length; ++i)
            aInputVideos[i] = strInputPath + aClipInfos[i].clipFileName;
      }
      if (bIsDenver)
         aInputVideos = new String[] { strInputPath + aClipInfos[0].clipFileName };
      
      for (int v = 0; v < aInputVideos.length; ++v)
      {
         // Build the argument list
         ArrayList aParameters = new ArrayList(20);
   
         aParameters.add("avcnvt");                                      // Ignored / Not read by Parser
         
         String strOutputFile = "";
         
         if (!bDoClipsOnly || bIsDenver)
         {
            aParameters.add("-i" + m_strAudioFileName);                       // Audio Input File
      
            // Video input file?
            if (bHasNormalVideo || bIsDenver)
            {
               aParameters.add("-j" + aInputVideos[v]);                    // Video Input File
            }
            else if (bHasClips) // also means && !bHasNormalVideo
            {
               if (bSeparateClips)
               {
                  // We have to convert one clip after another and an audio file
                  aParameters.add("-y");                                    // no Video Input File
               }
            }
            else
            {
               aParameters.add("-y");                                       // no Video Input File
            }
            
            if (bHasClips && !bSeparateClips)
            {
               aParameters.add("-s");                                    // SG mode
           
               // Put the video clip list to the Converter
               res = HandleVideoClipsSeparated(aClipInfos, strInputPath);
               if (res != SUCCESS)
                  return res;
            }

            strOutputFile = strPrefix + ".rm";
            
            aParameters.add("-a2");                                         // Audio quality  
            // Video quality:
            // 0: normal
            // 1: smoothest motion  (is best for normal videos)
            // 2: sharpest image
            // 3: slideshow
            aParameters.add("-v" + iVideoQuality);                 // Video quality
      
         }
         else
         {
            strOutputFile = strPrefix + "_clip" + v + ".rm";
               
            aParameters.add("-i" + aInputVideos[v]);
            aParameters.add("-x");                                       // No audio       
            aParameters.add("-v2");                                      // Video quality: sharpest image

            // Special case: Evalution and Structured Screengrabbing document
            int nVersion = ConverterWizard.getVersionType();
            if ((nVersion == NativeUtils.EVALUATION_VERSION) && dd.isClipStructured()) {
               System.out.println("*** Eval Note on Structured SG clip");
               aParameters.add("-n");
            }
         }
         
         aParameters.add("-o" + strOutputFile);           // Output File
            
         m_aRmFilenames.add(strOutputFile); // for CopyStreamingServerFiles()
                
         // Add the output file name to the File Container
         m_aStreamingFiles.add(strOutputFile);      
         
      
         aParameters.add("-e");                                          // No events
         aParameters.add("-m");                                          // No image maps

         // Something like "-t0,2,5"
         aParameters.add(strBandwidthsFlag);                                          // Bandwidths
    
         // Optional Real6/G2 compatibility
         if (iMinRealVersion < 8)
            aParameters.add("-g");                                       // G2 compatibility

         if (!bDoClipsOnly)
         {
            // Check for video offset
            System.out.println("*** Video Offset: " + lVideoOffset + "ms");
            setVideoOffset(lVideoOffset);
                     
            // Set the Still Image Name, if any
            if (bHasStillimage)
               setStillImageName(strStillimageName);
            else
               setStillImageName("");
         }
         else
         {
            setVideoOffset(0);
            setStillImageName("");
         }

         int iOutputWidth = iVideoOutputWidth;
         int iOutputHeight = iVideoOutputHeight;
         
         if (bDoClipsOnly)
         { 
            // Read the clip output size:
            int iClipWidth = aClipInfos[v].videoWidth;
            int iClipHeight = aClipInfos[v].videoHeight;

            // Do we have to resize very large clips to a maximum resolution?
            if (bClipSizeLimited)
            {
               double scaleX = (double)iMaxClipWidth / (double)iClipWidth;
               double scaleY = (double)iMaxClipHeight / (double)iClipHeight;
               double scaleClips = (scaleX < scaleY) ? scaleX : scaleY;
               if (scaleClips > (double)1.0)
                  scaleClips = (double)1.0;

               iClipWidth = (int)( (double)iClipWidth * scaleClips + 0.5 );
               iClipHeight = (int)( (double)iClipHeight * scaleClips + 0.5 );
               
               // Rounding problem for small sizes (see "Check video output size" below)
               if ((iMaxClipWidth == 32) && (iClipWidth == 31))
                  iClipWidth = 32;
               if ((iMaxClipHeight == 32) && (iClipHeight == 31))
                  iClipHeight = 32;
            }

            // ??? side effect?
            aClipInfos[v].videoWidth = iClipWidth;
            aClipInfos[v].videoHeight = iClipHeight;
            
            System.out.println("Clip[" + v + "] size: " + iClipWidth + "x" + iClipHeight);
            
            iOutputWidth = iClipWidth;
            iOutputHeight = iClipHeight;
         }
         
         
         // Check video output size: it must have a minimum of 32x32 px.
         if ((iOutputWidth < 32) || (iOutputHeight < 32))
            res = INVALID_VIDEO_SIZE;
         if (res != SUCCESS)
            return res;

         // Set the video output size:
         setVideoOutputSize(iOutputWidth, iOutputHeight);


         // Copy the argument list to an array with the 
         // correct number of arguments
         String[] args = new String[aParameters.size()];
         aParameters.toArray(args);
      
         // --------------------------
         // Now convert into RealMedia
         // --------------------------
         res = convertToRealMedia(aParameters.size(), args);
      
         // Check, if the conversion of this video/clip was successfull
         // If not, return error code 
         if (res != SUCCESS && bDoClipsOnly)
            return res;
      }
      
      // Get the used Codec
      m_pwData.m_strFccCodec = getFccCodec(); // ??? side effect

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
         
         
         m_dimVideo.width = getVideoWidth();
         m_dimVideo.height = getVideoHeight();
         // ??? side effects?        
         System.out.println("Video size: " + m_dimVideo.width + " x " + m_dimVideo.height);
      }

      return res;
   }
   
   /***************************************************************************/

   /**
    * Converts Clips into RealMedia.
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

         ArrayList aParameters = new ArrayList(20);
         
         aParameters.add("avcnvt");                                   // Ignored / Not read by Parser
         aParameters.add("-i" + clipFilenames[i]);                    // Clip Input File
         aParameters.add("-o" + m_strPathFileName 
                           + "_clip" + i + ".rm");                 // Output File
         
          m_aRmFilenames.add(m_strPathFileName 
                             + "_clip" + i + ".rm"); // for CopyStreamingServerFiles()
         
         aParameters.add("-e");                                       // No events
         aParameters.add("-m");                                       // No image maps
         aParameters.add("-x");                                       // No audio
         aParameters.add("-v2");                                      // Video quality: sharpest image
                         
         // Build a flag like "-t0,2,5"
         String strBandwidths = "-t";
         for (int b = 0; b < data_.audioBitrates.length; ++b)
         {
            if (data_.audioBitrates[b])
            {
               if (!strBandwidths.endsWith("t"))
                  strBandwidths += ",";

               strBandwidths += b;
            }
         }
         aParameters.add(strBandwidths);                                          // Bandwidths
         
         // Optional Real6/G2 compatibility
         if (data_.minRealVersion < 8)
         {
            // Real6/G2 compatibility
            aParameters.add("-g");                                    // G2 compatibility
         }

         // Copy the argument list to an array with the 
         // correct number of arguments
         String[] args = new String[aParameters.size()];
         aParameters.toArray(args);

         setVideoOffset(0);
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

         // --------------------------
         // Now convert into RealMedia
         // --------------------------
         res = convertToRealMedia(aParameters.size(), args);

         // Get the used Codec
         data_.fccCodec = getFccCodec();

         // Add the output file name to the File Container
         streamingFiles.add(m_strPathName + m_strFileName + "_clip" + i + ".rm");      

         // Check, if the conversion of the clip was successfull
         // If not, return error code 
         if (res != SUCCESS)
            return res;
      }
      
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
      super.setKeywordsForParser();
      
///      // Get the Player size
///      if (data_.hasVideoFile && !data_.videoIsStillImage )
///      {
///         // Video
///         //tpcAll.nRealPlayerWidth = ( (tpcAll.nVideoWidth<nMinVideoWidth) ? (nMinVideoWidth) : (tpcAll.nVideoWidth) );
///         tpcAll.nRealPlayerWidth = tpcAll.nVideoWidth;
///         tpcAll.nRealPlayerHeight = 30;
///      }
///      else
///      {
///         // No video (but maybe still image)
///         tpcAll.nRealPlayerWidth = ( (tpcAll.nVideoWidth<nMinVideoWidth) ? (nMinVideoWidth) : (tpcAll.nVideoWidth) );
///         tpcAll.nRealPlayerHeight = 30;
///      }
///      
///      tpcAll.nPlayerShadowWidth = tpcAll.nRealPlayerWidth;
///      tpcAll.nPlayerShadowHeight = tpcAll.nVideoHeight 
///                                 + tpcAll.nRealPlayerHeight 
///                                 + tpcAll.nRealPlayerHeight;

      return SUCCESS;
   }
   */
   
   /***************************************************************************/

   /**
    * This routine writes the Stand-Alone Player to a HTML page
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   /*
   protected int createStandAlonePlayer()
   {
      int res = SUCCESS;
      
      // Write RealMedia-independent files
      res = super.createStandAlonePlayer();
      if (res != SUCCESS)
         return HTML_FAILED;
      
      // Write '<name>.html'
      res = templateParser.parse("standalone_rm.tmpl", m_strPathFileName + ".html");
      if (res == SUCCESS)
         System.out.println(m_strFileName + ".html written.");
      else
      {
         System.out.println("Unable to write " + m_strFileName + ".html !");
         return HTML_FAILED;
      }

      // Add the output file name to the File Container
      fileContainer.add(m_strPathName + m_strFileName + ".html");      
      
      return res;
   }
   */
   
   /***************************************************************************/

   /**
    * This routine writes the HTML Files 
    * control.js, <name>.html, <name>_embed.html and <name>_indexframe.html.
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   /*
   protected int writeHtmlFiles()
   {
      int res = SUCCESS;
      
      // Write RealMedia-independent files
      res = super.writeHtmlFiles();
      if (res != SUCCESS)
         return HTML_FAILED;
      

      // Write 'control_rm.js'
      res = templateParser.parse("control_rm.tmpl", m_strPathName + "control_rm.js");
      if (res == SUCCESS)
         System.out.println("control_rm.js written.");
      else
      {
         System.out.println("Unable to write control_rm.js !");
         return HTML_FAILED;
      }
         
      // Add the output file name to the File Container
      fileContainer.add(m_strPathName + "control_rm.js");      
      

      // Write '<name>_videoframe.html'
      res = templateParser.parse("videoframe_rm.tmpl", m_strPathFileName + "_videoframe.html");
      if (res == SUCCESS)
         System.out.println(m_strFileName + "_videoframe.html written.");
      else
      {
         System.out.println("Unable to write " + m_strFileName + "_videoframe.html !");
         return HTML_FAILED;
      }

      // Add the output file name to the File Container
      fileContainer.add(m_strPathName + m_strFileName + "_videoframe.html");      


      // New in 1.6.0:
      // Write '<name>.smi', if clips are separated from video
      if (data_.separateClipsFromVideo)
      {
         res = templateParser.parse("smil.tmpl", m_strPathFileName + ".smi");
         if (res == SUCCESS)
            System.out.println(m_strFileName + ".smi written.");
         else
         {
            System.out.println("Unable to write " + m_strFileName + ".smi !");
            return HTML_FAILED;
         }
   
         // Add the output file name to the File Container
         fileContainer.add(m_strPathName + m_strFileName + ".smi");      
      }

      return res;
   }
   */
   
   /***************************************************************************/

   /**
    * This routine writes the Meta File (RPM)
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   /*
   protected int writeMetaFile()
   {
      int res = SUCCESS;

      // Write the RPM-File
      res = templateParser.parse("rpm.tmpl", m_strPathFileName + ".rpm");
      if (res == SUCCESS)
         System.out.println(m_strFileName + ".rpm written.");
      else
      {
         System.out.println("Unable to write " + m_strFileName + ".rpm !");
         return RPM_FAILED;
      }

      // Add the output file name to the File Container
      fileContainer.add(m_strPathName + m_strFileName + ".rpm");
      
      return res;
   }
   */
   
   /***************************************************************************/

   /**
    * This routine checks if the RealMedia file can be written
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   protected int checkStreamingFileWritable()
   {
      String outFile = m_strPathFileName + ".rm";
      
      return CheckStreamingFileWriteable(outFile);
      
      /*
      int res = SUCCESS;

      PrintWriter fileOut = null;
      
      try
      {
         // Open Output File
         fileOut = new PrintWriter(
                   new BufferedWriter(
                   new FileWriter(outFile)));

         // Close Output File
         fileOut.flush();
         fileOut.close();
      }
      catch (IOException e)
      {
         e.printStackTrace();
         System.out.println("Unable to open " + outFile);
         return RPM_FAILED;
      }

      return res;
      */
   }
   
   /***************************************************************************/

   /**
    * This routine copies the Files to the Real Server directory. 
    *
    *    @return int: 0 (SUCCESS), if successful - or error code otherwise.
    */
   
   protected int copyStreamingServerFiles()
   {
      // called "externally" from StreamingMediaConverter
      if (m_aRmFilenames != null && m_strStreamingTargetPath != null)
         return CopyStreamingServerFiles(m_aRmFilenames, m_strStreamingTargetPath);
      else
         return NO_WMSERVER_COPY;
   }
   
   protected ArrayList getStreamingServerFileNames()
   {
      return m_aRmFilenames;
   }

   /*
   protected int copyStreamingServerFiles()
   {
      int res = SUCCESS;
      
      String fileIn, fileOut;
      
      // Reset progress bar
      int currentProgress = 0;
      display.displayCurrentFileProgress(currentProgress);

      // Copy the RealMedia file
      fileIn = m_strPathName + m_strFileName + ".rm";
      fileOut = m_strStreamingTargetPath + m_strFileName + ".rm";

      ///res = copyFile(fileIn, fileOut);
      res = moveFile(fileIn, fileOut);

      // Update progress bar
      currentProgress++;
      display.displayCurrentFileProgress(currentProgress);
      

      // Do we have to copy a SMIL file and some clips?
      if ( (res == SUCCESS) && (data_.separateClipsFromVideo) )
      {
///         ///This not necessary, because the SMIL file must be referenced on the HTTP server 
///         ///(New in 1.6.1)
///         // Copy the SMIL file
///         fileIn = m_strPathName + m_strFileName + ".smi";
///         fileOut = m_strStreamingTargetPath + m_strFileName + ".smi";
///
///         res = copyFile(fileIn, fileOut);
         fileIn = m_strPathName + m_strFileName + ".smi";
         fileOut = m_strStreamingTargetPath + m_strFileName + ".smi";
         res = moveFile(fileIn, fileOut);
      
         if (res == SUCCESS)
         {
            int size = data_.multiVideoClips.length;

            // Copy the clip files
            for (int i = 0; i < size; i++)
            {
               fileIn = m_strPathName + m_strFileName + "_clip" + i + ".rm";
               fileOut = m_strStreamingTargetPath + m_strFileName + "_clip" + i + ".rm";

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
      }

      if (res == SUCCESS)
         return SUCCESS;
      else
         return NO_REALSERVER_COPY;
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
      int res = SUCCESS;
      
      /*
      int size = data_.multiVideoClips.length;
      String[] clipFilenames = new String[size];
      long[] clipTimestamps = new long[size];
      
      // Fill the arrays
      for (int i = 0; i < size; ++i)
      {
         // Add the input path to the filenames
         clipFilenames[i] = m_strInputPathName + data_.multiVideoClips[i].clipFileName;
         clipTimestamps[i] = data_.multiVideoClips[i].startTimeMs;
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

}
