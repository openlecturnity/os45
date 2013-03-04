package imc.epresenter.filesdk;

/*
 * File:             ConsistencyChecker.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: ConsistencyChecker.java,v 1.33 2010-03-26 11:01:51 zoen Exp $
 */

import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.filesdk.util.XmlParser;
import imc.epresenter.filesdk.util.XmlFormatException;

import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;

import javax.swing.JOptionPane;

import java.awt.Dimension;

import java.io.*;

import java.util.Vector;

/**
 * This class may be used in order to retrieve information on
 * Lecturnity Recording Documents. These data are returned by the
 * {@link #getLastPresentationData() getPresentationData()} method.
 *
 * @see PresentationData
 * @author Martin Danielsson
 */
public class ConsistencyChecker
{
   private static Localizer localizer_;

   // eventual error message of the consistency check is kept here
   private static String consistencyErrorMessage_ = null;

   private static boolean presentationDataExists_ = false;

   private static String presentationFileName_ = null;
   private static String presentationPath_   = null;
   private static String audioFileName_      = null;
   private static String eventFileName_      = null;
   private static String objectFileName_     = null;
   private static Dimension whiteboardSize_  = null;
   private static boolean usesVideo_         = false;
   private static int videoHelperIndex_      = -1;
   private static boolean hasVideoOffset_    = false;
   private static boolean videoIsStillImage_ = false;

   private static boolean containsNormalVideo_    = false;
   private static VideoClipInfo normalVideoInfo_  = null;
   private static boolean containsMultipleVideos_ = false;
   private static Vector multiVideoClipsVector_   = null;
   private static boolean isSgStandAloneMode_     = false;

    // PZI: true if a clip is structured (used to fix inconsistency warning message)
   private static boolean hasStructuredClips_ = false;
   // PZI: true if all clips are fully structured (incl. images and fulltext)
   // TODO: maybe not necessary
   private static boolean hasStructuredClipsCompleted_ = true;
   // PZ/PZI: true in case of a demo document ("simulation mode")
   private static boolean bIsSimulationMode = false;

   private static int masterOffset_          = 0;
   private static String videoFileName_      = null;
   private static int videoWidth_            = 0;
   private static int videoHeight_           = 0;
   private static int videoOffsetMs_         = 0;
   
   private static String strCodepage_               = null;
   private static String strLength_                 = null;
   
   private static String metadataFileName_   = null;
   private static Metadata metadata_         = null;

   static
   {
      try
      {
         localizer_ = new Localizer("/imc/epresenter/filesdk/ConsistencyChecker_",
                                    "en");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open the Locale database!",
                                       "Severe Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   /**
    * This method performs a consistency check of the given
    * presentation. It returns <i>true</i> if all files that are
    * definitely needed for a presentation are available and non null
    * (i.e., the audio file, the event queue and the object queue). If
    * this method returns <i>false</i>, the error message may be
    * retrieved using the {@link #getLastConsistencyError()
    * getLastConsistencyError} method.
    */
   public static boolean checkForConsistency(String aofConfigFileName)
   {
      return checkForConsistency(aofConfigFileName, false);
   }

   public static boolean checkForConsistency(String aofConfigFileName,
                                             boolean retrieveVideoSizes)
   {
      presentationFileName_ = null;
      presentationPath_     = null;

      try
      {
         File file = new File(aofConfigFileName);
         String fileName = "";
         try
         {
            fileName = file.getCanonicalPath();
         }
         catch (IOException e)
         {
            throw new IOException(localizer_.getLocalized("ERR_RESOLVE"));
         }
         
         String dirName = fileName.substring
            (0, fileName.lastIndexOf(File.separatorChar) + 1);

         presentationFileName_ = fileName;
         presentationPath_     = dirName;

         InputStream stream = new BufferedInputStream(new FileInputStream(fileName));
         boolean bResult = checkForConsistency(stream, dirName, false, retrieveVideoSizes);
         stream.close();
         return bResult;
      }
      catch (Exception e)
      {
         e.printStackTrace();
         consistencyErrorMessage_ = e.getMessage();
         return false;
      }
   }

   /**
    * Backend method for the above checkForConsistency method. Here
    * you may supply a plain <tt>InputStream</tt> instead of a file
    * name of an LRD file. This makes it possible to scan for the file
    * names of the other parts of a presentation, even if the config
    * file input stream comes from an archived file resources
    * instance. If that is the case, you may supply <tt>null</tt> as
    * <tt>dirName</tt>, and <tt>ignoreFiles</tt> has to be set to
    * true. In that case, the {@link PresentationData
    * PresentationData} instance returned by {@link
    * #getLastPresentationData() getLastPresentationData()} does
    * <b>not</b> contain any {@link Metadata Metadata} instance!
    *
    * @param configFileStream a config file input stream instance
    * @param dirName the directory of the LRD file, or <tt>null</tt>
    * if the config file input stream comes from an LPD file
    * @param ignoreFiles set this variable to <i>true</i> if you do
    * not want to check the files in the config file for existance
    * (will fluke if the config file comes from an LPD file).
    * @see FileResources
    * @since 1.4.1
    */
   public static boolean checkForConsistency(InputStream configFileStream, String dirName, boolean ignoreFiles)
   {
      return checkForConsistency(configFileStream, dirName, ignoreFiles, false);
   }

   public static boolean checkForConsistency(InputStream configFileStream, String dirName, 
                                             boolean ignoreFiles, boolean retrieveVideoSizes)
   {
      consistencyErrorMessage_ = null;

      presentationDataExists_ = false;
      
      audioFileName_        = null;
      eventFileName_        = null;
      objectFileName_       = null;
      whiteboardSize_       = null;
      usesVideo_            = false;
      videoFileName_        = null;
      videoWidth_           = -1;
      videoHeight_          = -1;
      videoIsStillImage_    = false;
      
      strCodepage_          = null;
      strLength_            = null;
      
      metadataFileName_     = null;
      metadata_             = null;

      containsNormalVideo_    = false;
      containsMultipleVideos_ = false;
      multiVideoClipsVector_  = new Vector();
      isSgStandAloneMode_     = false;
      
      // PZI: reset flags
      hasStructuredClips_           = false;
      hasStructuredClipsCompleted_  = true;
      
      boolean isOk = true;

      try
      {
//          XmlParser parser = new XmlParser(fileName);
         XmlParser parser = new XmlParser(configFileStream);

         if (parser.IsUtf8())
            strCodepage_ = "utf-8"; // Bom overrides CODEPAGE
         
         int ttype = XmlParser.TAG_START;
         int nextContentHasToExist = 0;
         String triggerEndTag = "";
         String currentTag = "";
         String sectionDescription = "";
         String possiblyMissingFiles = "";
         String currentVideoFileName = "";
         int multiVideoCount = 0;
         boolean nextContentIsMultiVideoStartTime = false;
         
         boolean inInfoTag = false;
         boolean inInfoTypeTag = false;
         boolean inInfoCodepageTag = false;
         boolean inInfoLengthTag = false;
         boolean inInfoViewTypeTag = false;
         boolean inWbPlayOptions = false;
         boolean nextIsWbPlayDimension = false;
         boolean inVideoOptions  = false;
         boolean inAudioTag = false;
         boolean inMetadataTag = false;

         boolean inVideoMultiOpt = false;
         boolean reallyMissingVideoFile = false;

         int currentHelperNr = -1;
         int currentOffsetNr = -1;
         int videoPlayerHelperNr = -1;

         boolean nextIsFirstVideoOption = false;

         boolean inHelpers = false;
         boolean inOffsets = false;
         boolean inMasterOffset = false;
         boolean inHelperOffset = false;
         
         boolean firstTag = true;

         while (ttype != XmlParser.EOF)
         {
            ttype = parser.getNextTag();

            if (firstTag)
            {
               if (ttype == XmlParser.TAG_START)
               {
                  if (!parser.getToken().equalsIgnoreCase("DOCUMENT"))
                     throw new IOException(localizer_.getLocalized("ERR_INVALID"));
               }
               else
                  throw new IOException(localizer_.getLocalized("ERR_INVALID"));

               firstTag = false;
            }

            switch (ttype)
            {
            case XmlParser.TAG_START: {
               String tag = parser.getToken();
               if (tag.equalsIgnoreCase("HELPERS"))
               {
                  inHelpers = true;
               }
               else if (tag.equalsIgnoreCase("INFO"))
               {
                  inInfoTag = true;
               }
               else if (inInfoTag)
               {
                  if (tag.equalsIgnoreCase("TYPE"))
                     inInfoTypeTag = true;
                  else if (tag.equalsIgnoreCase("CODEPAGE"))
                     inInfoCodepageTag = true;
                  else if (tag.equalsIgnoreCase("LENGTH"))
                     inInfoLengthTag = true;
                  else if (tag.equalsIgnoreCase("VIEWTYPE"))
                     inInfoViewTypeTag = true;
               }
               else if (tag.equalsIgnoreCase("OFFSETS"))
               {
                  inOffsets = true;
               }
               else if (tag.equalsIgnoreCase("STARTOFFSET"))
               {
                  inMasterOffset = true;
               }
               else if (inHelpers)
               { 
                  if (tag.equalsIgnoreCase("H"))
                     currentHelperNr++;
               }
               else if (inOffsets)
               {
                  if (tag.equalsIgnoreCase("O"))
                  {
                     currentOffsetNr++;
                     inHelperOffset = true;
                  }
               }
               else if (inMasterOffset)
               {
                  throw new XmlFormatException
                     ("illegal start tag in <STARTOFFSET> tag: <" + tag + ">");
               }
               else if (tag.equalsIgnoreCase("AUDIO"))
               {
                  triggerEndTag = "AUDIO";
                  nextContentHasToExist = 1;
                  sectionDescription =
                     localizer_.getLocalized("DESC_AUDIO");
                  inAudioTag = true;
//                   System.out.println("Waiting for files in: " + sectionDescription);
               }
               else if (tag.equalsIgnoreCase("METADATA"))
               {
                  triggerEndTag = "METADATA";
                  nextContentHasToExist = 1;
                  sectionDescription =
                     localizer_.getLocalized("DESC_METADATA");
                  inMetadataTag = true;
               }
               // PZI:
               else if (tag.equalsIgnoreCase("CLIPSTRUCTURE")) {
                  // read everything here
                  try {
                     // System.out.println(tag);
                     // skip empty content
                     parser.getNextTag();
                     // read structuring information for each clip
                     int clipCount = 0;
                     while (parser.getNextTag() != XmlParser.EOF
                           && parser.getToken().equalsIgnoreCase("CLIP")) {
                        // System.out.println("  Clip Nr. " + (clipCount + 1));
                        VideoClipInfo videoClipInfo = (VideoClipInfo) multiVideoClipsVector_
                              .get(clipCount++);

                        // read until end of clip
                        int type = parser.getNextTag();
                        while (type != XmlParser.EOF
                              && !parser.getToken().equalsIgnoreCase("CLIP")) {
                           String availableTag = parser.getToken();
                           if (type == XmlParser.TAG_START
                                 && availableTag.equalsIgnoreCase("STRUCTURE")) {
                              // flag
                              parser.getNextTag();
                              String availableFlag = parser.getToken();
                              boolean available = availableFlag.equalsIgnoreCase("available");
                              hasStructuredClipsCompleted_ &= available;
                              videoClipInfo.structureAvailable = available;                              
                              if (videoClipInfo.structureAvailable)
                                 hasStructuredClips_ = true;
                               
                           } else if (type == XmlParser.TAG_START
                                 && availableTag.equalsIgnoreCase("IMAGES")) {
                              // flag
                              parser.getNextTag();
                              String availableFlag = parser.getToken();
                              boolean available = availableFlag.equalsIgnoreCase("available");
                              hasStructuredClipsCompleted_ &= available;
                              videoClipInfo.imagesAvailable = available;
                              
                           } else if (type == XmlParser.TAG_START
                                 && availableTag.equalsIgnoreCase("FULLTEXT")) {
                              // flag
                              parser.getNextTag();
                              String availableFlag = parser.getToken();
                              boolean available = availableFlag.equalsIgnoreCase("available");
                              hasStructuredClipsCompleted_ &= available;
                              videoClipInfo.fulltextAvailable = available;
                           }
                           // read next tag for loop iteration
                           type = parser.getNextTag();
                        }
                        // skip empty content
                        parser.getNextTag();
                     }
                     // System.out.println(parser.getToken());
                  } catch (Exception e) {
                     // TODO: error handling
                  }

               }
               currentTag = "" + tag;
               break;
            }

            case XmlParser.TAG_END: {
               String tag = parser.getToken();
               if (nextContentHasToExist > 0 && !ignoreFiles)
               {
                  if (tag.equalsIgnoreCase(triggerEndTag))
                  {
                     if (consistencyErrorMessage_ == null)
                        consistencyErrorMessage_ = "";

                     // Logic of the next two if statements:
                     // If the presentation contains multiple video
                     // clips, then the counter nextContentHasToExists
                     // is incremented each time a video file is
                     // found, so that multiple video files are
                     // recognized. Still, if we have multiple videos
                     // files but have not found any, we still want to
                     // output an error message.
                     if (inVideoOptions && containsMultipleVideos_)
                     {
                        if (reallyMissingVideoFile)
                        {
                           consistencyErrorMessage_ += "\n" + nextContentHasToExist +
                              " " + localizer_.getLocalized("ERR_MISSING") +
                              sectionDescription +
                              localizer_.getLocalized("ERR_MISSING2") +
                              possiblyMissingFiles;
                        }
                     }
                     else
                     {
                        consistencyErrorMessage_ += "\n" + nextContentHasToExist +
                           " " + localizer_.getLocalized("ERR_MISSING") +
                           sectionDescription +
                           localizer_.getLocalized("ERR_MISSING2") +
                           possiblyMissingFiles;
                     }
                     
//                      System.out.println(consistencyErrorMessage_);

                  }
               }

               if (tag.equalsIgnoreCase(triggerEndTag))
               {
                  triggerEndTag = "";
                  possiblyMissingFiles = "";
                  nextContentHasToExist = 0;
                  
                  inWbPlayOptions = false;
                  inVideoOptions = false;
                  inVideoMultiOpt = false;
                  inAudioTag = false;
                  inMetadataTag = false;
               }

               if (inHelpers)
               {
                  if (tag.equalsIgnoreCase("HELPERS"))
                     inHelpers = false;
               }
               else if (inOffsets)
               {
                  if (inHelperOffset)
                  {
                     if (tag.equalsIgnoreCase("O"))
                        inHelperOffset = false;
                  }
                  else if (tag.equalsIgnoreCase("OFFSETS"))
                     inOffsets = false;
               }
               else if (inMasterOffset)
               {
                  if (tag.equalsIgnoreCase("STARTOFFSET"))
                     inMasterOffset = false;
                  else
                     throw new XmlFormatException
                        ("invalid end tag in <STARTOFFSET>: <" + tag + ">");
               }
               else if (inInfoTag)
               {
                  if (inInfoTypeTag)
                  {
                     if (tag.equalsIgnoreCase("TYPE"))
                        inInfoTypeTag = false;
                     else
                        throw new XmlFormatException
                           ("invalid end tag in <TYPE>: <" + tag + ">");
                  }
                  else if (inInfoCodepageTag)
                  {
                     if (tag.equalsIgnoreCase("CODEPAGE"))
                        inInfoCodepageTag = false;
                     else
                        throw new XmlFormatException
                           ("invalid end tag in <CODEPAGE>: <" + tag + ">");
                  }
                  else if (inInfoLengthTag)
                  {
                     if (tag.equalsIgnoreCase("LENGTH"))
                        inInfoLengthTag = false;
                     else
                        throw new XmlFormatException
                           ("invalid end tag in <LENGTH>: <" + tag + ">");
                  }
                  else if (inInfoViewTypeTag)
                  {
                     if (tag.equalsIgnoreCase("VIEWTYPE"))
                         inInfoViewTypeTag = false;
                     else
                        throw new XmlFormatException
                           ("invalid end tag in <VIEWTYPE>: <" + tag + ">");
                  }
                  else if (tag.equalsIgnoreCase("INFO"))
                  {
                     inInfoTag = false;
                  }
               }

               break;
            }

            case XmlParser.PROPERTY: {
               // we may ignore these
               break;
            }

            case XmlParser.BOOL_PROPERTY: {
               // we may ignore these
               break;
            }

            case XmlParser.CONTENT: {
               String content = parser.getToken().trim();

               if (content.equals(""))
                  break;

               if (inWbPlayOptions)
               {
                  if (content.equalsIgnoreCase("-G"))
                  {
                     nextIsWbPlayDimension = true;
                     break;
                  }

                  if (nextIsWbPlayDimension)
                  {
                     whiteboardSize_ = parseWbSize(content);
                     nextIsWbPlayDimension = false;
                     break;
                  }
               }

               if (inInfoTag)
               {
                  if (inInfoTypeTag)
                  {
                     if (content.equalsIgnoreCase("denver"))
                     {
                        //System.out.println("TYPE is 'denver'");
                        isSgStandAloneMode_ = true;
                        break;
                     }
                  }
                  if (inInfoCodepageTag)
                  {
                     if (FileUtils.isInteger(content))
                        strCodepage_ = "Cp"+content;
                     else
                        strCodepage_ = content;
                     
                     //if (parser.IsUtf8())
                     //   strCodepage_ = "utf-8"; // Bom overrides CODEPAGE
                     // No: every text file (eg obj, lmd) automatically checks
                     // for being "utf-8" encoded. If not the specified codepage should be used.
                     
                     break;
                  }
                  if (inInfoLengthTag)
                  {
                     strLength_ = content;
                     break;
                  }
                  if (inInfoViewTypeTag) {
                      if (content.equalsIgnoreCase("simulation"))
                          bIsSimulationMode = true;
                      else
                          bIsSimulationMode = false;
                  }
               }

               if (inVideoOptions)
               {
                  if (content.equalsIgnoreCase("-still")) // still image
                  {
                     // next content line is the still image to use
                     videoIsStillImage_ = true;
                     nextContentHasToExist = 1;
                     nextIsFirstVideoOption = false;
                     inVideoMultiOpt = false;
                     break;
                  }
                  else if (content.equalsIgnoreCase("-video"))
                  {
                     // next content line is the video file to use
                     containsNormalVideo_ = true;
                     nextContentHasToExist = 1;
                     nextIsFirstVideoOption = false;
                     inVideoMultiOpt = false;
                     break;
                  }
                  else if (content.equalsIgnoreCase("-multi")) // multiple videos
                  {
                     // next contents are multiple videos, next line
                     // is a video clip, then follows an offset for that clip.
//                      System.out.println("-multi");
                     containsMultipleVideos_ = true;
                     inVideoMultiOpt = true;
                     nextContentHasToExist = 1; // next line has to exist

                     nextIsFirstVideoOption = false;
                     break;
                  }
                  else if (nextContentIsMultiVideoStartTime)
                  {
                     // we have just read a video clip file name; now a
                     // start time for that clip:
                     long videoStartTime = Long.parseLong(content);
                     multiVideoClipsVector_.addElement
                        (new VideoClipInfo(currentVideoFileName, videoStartTime));
                     nextContentIsMultiVideoStartTime = false;

//                      System.out.println("Clip: " + currentVideoFileName + ", time: " + videoStartTime);
                     multiVideoCount++;

                     // next line, if exists, is next video
                     nextContentHasToExist = 1;

                     nextIsFirstVideoOption = false;
                     break;
                  }
                  else if (nextIsFirstVideoOption)
                  {
                     // we have not gotten any option of the type "-option"
                     // then assume option "-video"
                     containsNormalVideo_ = true;
                     nextContentHasToExist = 1;
                     nextIsFirstVideoOption = false;
                     inVideoMultiOpt = false;

                     // NOTE: Do not "break;" here! Continue!
                  }
               }

               if (nextContentHasToExist > 0)
               {
                  File testFile = new File(dirName + content);
                 
                  if (ignoreFiles || testFile.exists())
                  {
                     if (ignoreFiles || testFile.length() > 0)
                     {
                        if (!ignoreFiles)
                           nextContentHasToExist--;
                        if (inVideoOptions)
                        {
//                            if (content.toUpperCase().endsWith(".AVI"))
                           if (containsMultipleVideos_ && inVideoMultiOpt)
                           {
                              // we're after a "-multi" opt:
                              currentVideoFileName = content;
                              nextContentIsMultiVideoStartTime = true;
                           }
                           else
                           {
                              videoFileName_ = content;
                           }
                        }
                        if (inAudioTag)
                        {
                           audioFileName_ = content;
                        }
                        if (inMetadataTag)
                        {
                           metadataFileName_ = content;
                        }
                        if (inWbPlayOptions)
                        {
                           String tContent = content.toUpperCase();
                           if (tContent.endsWith(".EVQ"))
                              eventFileName_ = content;
                           else if (tContent.endsWith(".OBJ"))
                              objectFileName_ = content;
                        }

//                         System.out.println("  Found file: '" + content
//                                            + "'");
                     }
                     else
                     {
                        possiblyMissingFiles += content + " (0 Bytes)\n";
                        // double code (see below)
                        if (inVideoOptions && containsMultipleVideos_ && inVideoMultiOpt)
                           reallyMissingVideoFile = true;
                     }
                  }
                  else
                  {
                     possiblyMissingFiles += content + '\n';
                     // double code (see above):
                     if (inVideoOptions && containsMultipleVideos_ && inVideoMultiOpt)
                        reallyMissingVideoFile = true;
                  }
               }
               else if (currentTag.equalsIgnoreCase("OPT"))
               {
                  if (content.equalsIgnoreCase("WBPLAY"))
                  {
                     nextContentHasToExist = 2;
                     sectionDescription =
                        localizer_.getLocalized("DESC_WBPLAY");
                     triggerEndTag = "H";
                     inWbPlayOptions = true;
//                      System.out.println("Waiting for files in: " + sectionDescription);
                  }
                  else if (content.indexOf("JMFVideoPlayer") >= 0)
                  {
//                      nextContentHasToExist = 1;
                     sectionDescription = localizer_.getLocalized("DESC_VIDEO");
                     triggerEndTag = "H";
                     inVideoOptions = true;
                     usesVideo_ = true;
                     videoPlayerHelperNr = currentHelperNr;
                     videoHelperIndex_ = currentHelperNr;
                     nextIsFirstVideoOption = true;

//                      System.out.println("video helper has nr #" + videoPlayerHelperNr);

//                      System.out.println("Waiting for files in: " + sectionDescription);
                  }
                  // insert other known helpers here
               }
               else if (inOffsets)
               {
                  if (inHelperOffset)
                  {
//                      System.out.println("found offset #" + currentOffsetNr + ": " + content);

                     if (currentOffsetNr == videoPlayerHelperNr)
                     {
                        videoOffsetMs_ = Integer.parseInt(content);
                        hasVideoOffset_ = true;
//                         System.out.println("video helper has offset " + videoOffsetMs_ + " ms");
                     }
                  }
               }
               else if (inMasterOffset)
               {
                  masterOffset_ = Integer.parseInt(content);
               }
            }
            
            case XmlParser.EOF:
               break;
            }
         } // end while (ttype != XmlParser.EOF)
      }
      catch (IOException e)
      {
         consistencyErrorMessage_ = e.getMessage();
         isOk = false;
      }

      isOk = (consistencyErrorMessage_ == null ||
              consistencyErrorMessage_.equals(""));
      
      if (isOk)
      {
         // if in ignoreFiles mode, do not read Metadata!!!
         if (metadataFileName_ != null && !ignoreFiles)
         {
            try
            {
               InputStream metadataStream = new BufferedInputStream
                  (new FileInputStream(presentationPath_ + metadataFileName_));
               
               BufferedReader reader = FileUtils.createBufferedReader(metadataStream, strCodepage_);
               
               metadata_ = FileResources.createMetadata(reader);
               metadataStream.close();
            }
            catch (IOException e)
            {
               e.printStackTrace();
               consistencyErrorMessage_ = "" +
                  localizer_.getLocalized("ERR_METADATA") + metadataFileName_;
            }
         }
      }
      else
      {
         // add error message in front of real messages:
         consistencyErrorMessage_ = localizer_.getLocalized("ERR_HEADER") + consistencyErrorMessage_;
      }

      isOk = (consistencyErrorMessage_ == null ||
              consistencyErrorMessage_.equals(""));
      
      if (retrieveVideoSizes)
      {
         if (containsMultipleVideos_)
         {
            for (int i=0; i<multiVideoClipsVector_.size(); ++i)
            {
               VideoClipInfo vci = (VideoClipInfo) (multiVideoClipsVector_.elementAt(i));
               if (NativeUtils.retrieveVideoInfo(dirName, vci))
               {
                  //System.out.println("Video '" + vci.clipFileName + "': " + vci.videoWidth + "x" + vci.videoHeight + ", length: " + vci.videoDurationMs + "ms");
               }
               else
               {
                  System.err.println("retrieveVideoInfo failed");
               }
            }
         }

         if (containsNormalVideo_)
         {
            normalVideoInfo_ = new VideoClipInfo(videoFileName_, 0);
            if (NativeUtils.retrieveVideoInfo(dirName, normalVideoInfo_))
            {
               //System.out.println("Video '" + normalVideoInfo_.clipFileName + "': " 
               //                   + normalVideoInfo_.videoWidth + "x" + normalVideoInfo_.videoHeight);
               videoWidth_ = normalVideoInfo_.videoWidth;
               videoHeight_ = normalVideoInfo_.videoHeight;
            }
         }
      }

      presentationDataExists_ = isOk;

      return isOk;
   }
   
   private static Dimension parseWbSize(String wbSize)
   {
      int xPos = wbSize.indexOf('x');
      int width = Integer.parseInt(wbSize.substring(0, xPos));
      int height = Integer.parseInt(wbSize.substring(xPos + 1, wbSize.length()));
      return new Dimension(width, height);
   }
   
   /**
    * This method returns the last error message output by the {@link
    * #checkForConsistency() checkForConsistency} method. If that
    * method was successful, this method will return <tt>null</tt>.
    */
   public static String getLastConsistencyError()
   {
      return consistencyErrorMessage_;
   }

   /**
    * Returns a {@link PresentationData PresentationData} object
    * describing the last successfully consistency-checked
    * presentation configuration file (using the {@link
    * #checkForConsistency() checkForConsistency()} method).
    *
    * @return a {@link PresentationData PresentationData} object for
    * the lastly checked presentation, or <tt>null</tt> if that check
    * was unsuccessful.
    * @see PresentationData
    * @see #checkForConsistency()
    * @see #getLastConsistencyError()
    */
   public static PresentationData getLastPresentationData()
   {
      if (!presentationDataExists_)
         return null;

      PresentationData pd = new PresentationData();

      pd.presentationFileName = "" + presentationFileName_;
      pd.presentationPath = "" + presentationPath_;
      pd.audioFileName = "" + audioFileName_;
      if (eventFileName_ != null)
         pd.eventFileName = "" + eventFileName_;
      else
         pd.eventFileName = null;
      if (objectFileName_ != null)
         pd.objectFileName = "" + objectFileName_;
      else
         pd.objectFileName = null;
      if (whiteboardSize_ != null)
         pd.whiteboardSize = (Dimension) whiteboardSize_.clone();
      else 
         pd.whiteboardSize = null;
      pd.usesVideo = usesVideo_;
      pd.videoHelperIndex = videoHelperIndex_;
      if (videoFileName_ != null)
         pd.videoFileName = "" + videoFileName_;
      else
         pd.videoFileName = "";
      pd.normalVideoInfo = normalVideoInfo_;
         
      pd.videoWidth = videoWidth_;
      pd.videoHeight = videoHeight_;
      pd.hasVideoOffset = hasVideoOffset_;
      pd.videoOffsetMs = videoOffsetMs_;
      pd.videoIsStillImage = videoIsStillImage_;
      pd.containsNormalVideo = containsNormalVideo_;


      // PZI
      pd.hasStructuredClips = hasStructuredClips_;

      pd.containsMultipleVideos = containsMultipleVideos_;
      if (containsMultipleVideos_)
      {
         pd.multiVideoClips = new VideoClipInfo[multiVideoClipsVector_.size()];
         multiVideoClipsVector_.copyInto(pd.multiVideoClips);
      }
      else
         pd.multiVideoClips = null;
      pd.isSgStandAloneMode = isSgStandAloneMode_;

      // PZ/PZI
      if (bIsSimulationMode) {
         pd.containsMultipleVideos = false;
         pd.multiVideoClips = null;
         pd.hasStructuredClips = false;
      }
      
      pd.masterOffset = masterOffset_;
      
      if (strCodepage_ != null)
         pd.strCodepage = strCodepage_;
      else
         pd.strCodepage = null;

      if (metadataFileName_ != null)
         pd.metadataFileName = "" + metadataFileName_;
      else
         pd.metadataFileName = null;
      pd.metadata = metadata_;
      
      // try to take the record length from the lmd file but if not existant
      // then that of the lrd file
      if (strLength_ != null && pd.metadata != null && (pd.metadata.recordLength == null || pd.metadata.recordLength.equals("")))
         pd.metadata.recordLength = strLength_;

      return pd;
   }
}
