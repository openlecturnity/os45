package imc.lecturnity.info;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import javax.imageio.IIOImage;
import javax.imageio.ImageIO;
import javax.imageio.ImageWriter;
import javax.imageio.ImageWriteParam;
import javax.swing.ImageIcon;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.FileStruct;
import imc.epresenter.filesdk.Metadata;
import imc.epresenter.filesdk.ResourceNotFoundException;
import imc.epresenter.filesdk.SearchEngine;
import imc.epresenter.filesdk.ThumbnailData;
import imc.epresenter.filesdk.VideoClipInfo;
import imc.epresenter.player.Document;
import imc.epresenter.player.Manager;
import imc.epresenter.player.audio.SoundPlayer;
import imc.epresenter.player.util.XmlTag;
import imc.epresenter.player.whiteboard.Text;
import imc.epresenter.player.whiteboard.WhiteBoardPlayer;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;


/**
 * General class for accessing all the metadata and full text information
 * and the audio data and slide and clip images of a presentation.
 * 
 * @version 1.20, 24.08.2007; 1.00, 06.09.2005
 */
public class LpdFile
{
   private static final int OK = 0;
   private static final int E_ARGS = -23;
   private static final int E_IO = -25;
   private static final int E_RESOURCE = -27;
   private static final int E_NOOPT = -29;
   private static final int E_SOUND = -37;
   private static final int E_WHITEBOARD = -31;
   private static final int E_IMAGES = -45;

   /**
    * One or more of the parameters were not valid.
    */
   public static final int E_IMAGE_INVALID_PARAMETER = -1;
   
   /**
    * The output directory is not creatable or not writeable.
    */
   public static final int E_IMAGE_NO_OUTPUT_DIR = -2;
   
   /**
    * The image encoding failed. An exception occured.
    */
   public static final int E_IMAGE_ENCODING = -3;
   
   /**
    * The library nativeutils.dll is not present or accessible.
    */
   public static final int E_IMAGE_NO_NATIVEUTILS = -6;
   
   /**
    * The first frame of a video could not be extracted as an image.
    * Probably the codec is missing.
    */
   public static final int E_IMAGE_NO_VIDEO_IMAGE = -7;
   
   /**
    * Some internal failure has occured. Maybe the object is not
    * initialized properly.
    */
   public static final int E_IMAGE_INTERNAL_FAILURE = -8;
   
   
   /**
    * For debugging purposes: Tries to open the first parameter as an
    * input file.
    */
   public static void main(String[] args)
   {
      if (args.length == 0)
      {
         System.err.println("Error: Missing parameter: <lpd|lrd>");
      }
      else
      {
         System.setProperty("debug.output", "true");
         LpdFile lf = open(new File(args[0]));
         
         if (lf == null)
         {
            System.err.println("File could not be opened.");
            return;
         }
         
         if (lf.getMetaData() != null)
            System.out.println("Length: "+lf.getMetaData().recordLength);
         
         if (args.length > 1)
         {
            if (false)
            {
               AudioExtractor ae = lf.getAudioExtractor();
            
               System.out.println("#Channels: "+ae.getChannels());
               System.out.println("SamplingR: "+ae.getSamplingRate());
               System.out.println("#BitsPerS: "+ae.getBitsPerSample());
               System.out.print("Decompressing... ");
               boolean bSuccess = ae.extractToFile(new File(args[1]));
               System.out.println("Done. "+(bSuccess ? "Success." : "Failure."));
            }
            else
            {
               System.out.println("Starting writing images... ");
               int iPageCount = lf.writeImages(540, 400, 95, args[1]);
               System.out.println("Done. "+iPageCount+" images written");
            }
         }
         else
         {
            System.out.println("No second parameter (output file) given. Doing no audio or image extraction.");
         }
      }
      
      
      // in case there was a warning dialog, we need to exit explicitly
      System.exit(0);
   }
   
   
   /**
    * Static factory method for creating an <code>LpdFile</code> object.
    * With the help of this object one can access/get all the full text and
    * slide information and the remaining metadata present in an LPD or LRD file.
    * The given file be can be closed directly after this call.
    * <p>
    * Errors or warnings are only reported as string output on the standard
    * error output.
    * 
    * @return the newly created <code>LpdFile</code> containing (read-only) 
    *         all the information in a given LPD or LRD file
    */
   public static LpdFile open(File file) 
   {
      int iReturn = OK;
      boolean bDebugOutput = System.getProperty("debug.output", "false").equals("true");
      
      // do not display (graphical) warning messages during parsing
      System.setProperty("mode.consoleoperation", "true");

      String strLocation = null;
      if (null != file)
         strLocation = file.getPath();
      else
      {
         iReturn = E_ARGS;
         System.err.println("Error: Missing parameter: <lpd|lrd>");
      }
      
        
      // double code (Document)
      FileResources resources = null;
      InputStream configStream = null;
      
      if (iReturn >= OK && File.separator.equals("\\")) // is Windows
         strLocation = strLocation.replace('/', '\\');
      
      if (iReturn >= OK)
      {
         try 
         {
            resources = FileResources.createFileResources(strLocation);
            configStream = resources.createConfigFileInputStream();
         }
         catch (ResourceNotFoundException re)
         {
            iReturn = E_RESOURCE;
            System.err.println("Error: Configuration not found.");
         }
         catch (IOException ie)
         {
            iReturn = E_IO;
            System.err.println("Error: IO, maybe file not found: "+strLocation);
         }
      }
      XmlTag configTag = null;
      if (iReturn >= OK)
         configTag = (XmlTag.parse(configStream))[0];
      
      if (null != configStream)
      {
         try
         {
            configStream.close();
         }
         catch (IOException exc)
         {
         }
      }
      
      String strCodepage = "Cp1252";
      if (iReturn >= OK)
      {
         Object[] arrCodepages = configTag.getValues("CODEPAGE");
         if (arrCodepages != null && arrCodepages.length > 0)
         {
            strCodepage = (String)arrCodepages[0];
            if (FileUtils.isInteger(strCodepage))
               strCodepage = "Cp"+strCodepage;
         }
      }
      
      // jetzt brauchen wir noch Metadaten
      Metadata metadata = null;
      if (iReturn >= OK)
      {
         Object[] metaA = configTag.getValues("METADATA");
         if (metaA != null && metaA.length > 0)
         {
            String metaFile = (String)metaA[0];
           
            try
            {
               metadata = resources.getMetadata(metaFile);
            }
            catch (IOException ioe)
            {
            }
         }
      }
      if (null == metadata)
         System.err.println("Warning: No metadata found.");
      
      // recordLength field in metadata is not set by the Resources
      // because it is only listed in the LRD and not in the LMD
      String strRecordLength = "0:00";
      if (iReturn >= OK && null != metadata)
      {
         if (metadata.recordLength == null || metadata.recordLength.equals(""))
         {
            Object[] arrLengths = configTag.getValues("LENGTH");
            if (arrLengths != null && arrLengths.length > 0)
            {
               strRecordLength = (String)arrLengths[0];
            }
            metadata.recordLength = strRecordLength;
         }
      }
        
      
      System.setProperty("nyckel.musik", "forte");
      String audioLocation = null;
      if (iReturn >= OK)
      {
         audioLocation = (String)( (configTag.getValues("AUDIO"))[0] );
      }
      
      SoundPlayer sound = null;
      AudioExtractor audioExtractor = null;
      if (iReturn >= OK)
      {
         try
         {
            sound = new SoundPlayer(resources, audioLocation, null);
         }
         catch (Exception exc)
         {
            iReturn = E_SOUND;
            exc.printStackTrace();
            System.err.println("Error: AUDIO, could not open audio input ("+exc+").");
         }
         
         audioExtractor = new AudioExtractor(sound);

      }
      
   
      String strConfigValue = System.getProperty("pixel.limit", "");
      if (strConfigValue == null || strConfigValue.length() == 0)
         System.setProperty("pixel.limit", "10000000"); // make sure he can load some images
      
      String strEvqFile = null;
      String strObjFile = null;
      String strWbSize = "720x540"; // default
      WhiteBoardPlayer whiteboard = null;
      if (iReturn >= OK)
      {
         Object[] arrOpt = configTag.getValues("OPT");
         if (null != arrOpt && 0 < arrOpt.length)
         {
            boolean bSizePending = false;
            
            for (int i=0; i<arrOpt.length; ++i)
            {
               String strOption = (String)arrOpt[i];
               if (strOption.endsWith(".evq"))
                  strEvqFile = strOption;
               else if (strOption.endsWith(".obj"))
                  strObjFile = strOption;
               else if (strOption.equals("-G"))
                  bSizePending = true;
               else if (bSizePending && strOption.indexOf("x") > 0)
               {
                  bSizePending = false;
                  strWbSize = strOption;
                  
                  /* if enabled remove above
                  int iIndex = strOption.indexOf("x");
                  String strWidth = strOption.substring(0, iIndex);
                  String strHeight = strOption.substring(iIndex + 1);
                  
                  try
                  {
                     int iDummy1 = Integer.parseInt(strWidth);
                     int iDummy2 = Integer.parseInt(strHeight);
                     
                     // no number format exception
                     iWbWidth = iDummy1; 
                     iWbHeight = iDummy2;
                     
                     strWbSize = strOption;
                  }
                  catch (NumberFormatException exc)
                  {
                     // ignore; use default
                  }
                  */
               }
            }
         }
         else
         {
            iReturn = E_NOOPT;
            System.err.println("Error: No options found. Maybe configuration file corrupt.");
         }
      }
      
      if (null == strEvqFile || null == strObjFile)
      {
         String strType = "";
         Object[] arrCodepages = configTag.getValues("TYPE");
         if (arrCodepages != null && arrCodepages.length > 0)
            strType = (String)arrCodepages[0];
         
         if (!strType.equals("denver"))
            System.err.println("Warning: No information about whiteboard found.");
      }
      else
      {
         whiteboard = new WhiteBoardPlayer(true);
         try
         {
            String[] args = 
               new String[] { strEvqFile, strObjFile, strWbSize, "-cp", strCodepage };
            whiteboard.init(resources, args, null);
         }
         catch (RuntimeException exc)
         {
            iReturn = E_WHITEBOARD;
            System.err.println("Error: Cannot initialize whiteboard.");
         }
      }
      
      // das SearchEngine-Objekt verknüpft all die bisherigen Informationen
      // und erlaubt eine Text-zu-Slide-Zuornung
      SearchEngine engine = null;
      if (iReturn >= OK && null != whiteboard)
         engine = new SearchEngine(metadata, whiteboard.getObjectQueue(), 
                                   whiteboard.getEventQueue(), true);
      
      SearchEngine.Slide[] slides = null;
      if (iReturn >= OK && null != engine)
      {
         slides = engine.getSlideInformation();
      
         if (null == slides || 0 == slides.length)
            System.err.println("Warning: No slide information found.");
         
         if (bDebugOutput)
            System.out.println("Slide count: "+slides.length);
      }
      
      LpdTextTag[] aTextTags = null;
      if (iReturn >= OK && null != slides && slides.length > 0)
      {
         ArrayList textList = new ArrayList(500);
         for (int i=0; i<slides.length; ++i)
         {
            if (bDebugOutput)
               System.out.println("Slide "+i+": ");
            for (int j=0; j<slides[i].fullContent.size(); ++j)
            {
               Text text = (Text)slides[i].fullContent.get(j);
               if (bDebugOutput)
                  System.out.println(" "+text.getAllContent());
               
               LpdTextTag textTag = text.extractEverything(i);
               textList.add(textTag);
               
               if (bDebugOutput)
               {
                  //System.out.println(" Slant0: "+textTag.getTextItemTags()[0].getSlant());
                  //System.out.println(" Famil0: "+textTag.getTextItemTags()[0].getFont());
               }
               
               //if (bDebugOutput)
                  //System.out.println("pos: "+textTag.getX());
            }
         }
         aTextTags = new LpdTextTag[textList.size()];
         textList.toArray(aTextTags);
      }
   
      // reset "old" setting
      System.setProperty("mode.consoleoperation", "");

      
      if (iReturn >= OK)
      {
         String strFilenamePrefix = "";
         int idx1 = strLocation.lastIndexOf('\\');
         int idx2 = strLocation.lastIndexOf('.');
         if (idx1 > -1 && idx2 > -1 && idx2 > idx1)
            strFilenamePrefix = strLocation.substring(idx1 + 1, idx2);
         
         LpdFile lFile = new LpdFile(metadata, aTextTags,
                                     audioExtractor, whiteboard, 
                                     resources, strFilenamePrefix);
         return lFile;
      }
      else
         return null;
   }
   
   
   //
   // non-static part:
   //
   
   private Metadata m_Metadata;
   private LpdTextTag[] m_aTextTags;
   private AudioExtractor m_AudioExtractor;
   private WhiteBoardPlayer m_Whiteboard;
   private String m_strFilenamePrefix;
   private FileResources m_Resources;
   
   
   LpdFile(Metadata metadata, LpdTextTag[] aTextTags, 
           AudioExtractor audioExtractor, WhiteBoardPlayer whiteboard,
           FileResources resources, String strFilenamePrefix)
   {
      m_Metadata = metadata;
      m_aTextTags = aTextTags;
      m_AudioExtractor = audioExtractor;
      m_Whiteboard = whiteboard;
      m_strFilenamePrefix = strFilenamePrefix;
      if (m_strFilenamePrefix == null)
         m_strFilenamePrefix = "";
      m_Resources = resources;
   }
   
	
   /**
    * Returns the <code>Metadata</code> object containing all metadata 
    *          specified in an LPD or LRD file.
    * 
    * @return a <code>Metadata</code> object containing all metadata 
    *          specified in an LPD or LRD file. Can be null if no
    *          metadata specification could be found.
    * @see     imc.epresenter.filesdk.Metadata
    */
   public Metadata getMetaData() {
      return m_Metadata;
   }
   
   /**
    * Returns an array of <code>LpdTextTag</code> objects containing all
    *          of the full text on the slides.
    *
    * @return an array of <code>LpdTextTag</code> objects containing all
    *          of the full text on the slides. Can be null when no slides
    *          are present in a presentation.
    * @see     imc.lecturnity.info.LpdTextTag
    */
   public LpdTextTag[] getTextTags() {
      return m_aTextTags;
   }
   
   /**
    * Returns an object for extraction the audio file and the audio
    * information of an LPD or LRD file.
    * 
    * @return an object for extracting the audio file and
    * information of an LPD or LRD file.
    */
   public AudioExtractor getAudioExtractor() {
      return m_AudioExtractor;
   } 

   /**
    * Writes images of all pages and clips to the specified output directory.
    * The specified width and height are maximum sizes: The images can be smaller
    * in width or height or both.
    *
    * @return if successful the number of written images otherwise a negative error code.
    */
   public int writeImages(int iOutputWidth, int iOutputHeigth, 
                          int iJpegQuality, String strOutputDirectory)
   {
      if (m_Metadata == null)
         return E_IMAGE_INTERNAL_FAILURE;
      
      if (strOutputDirectory == null)
         return E_IMAGE_INVALID_PARAMETER;
      
      if (iOutputWidth <= 0 || iOutputHeigth <= 0)
         return E_IMAGE_INVALID_PARAMETER;
      
      if (iJpegQuality < 1 || iJpegQuality > 100)
         return E_IMAGE_INVALID_PARAMETER;
      
      Dimension dimMaxImageSize = new Dimension(iOutputWidth, iOutputHeigth);
      float fJpegQuality = iJpegQuality / 100.0f;
      
      boolean bExists = false;
      boolean bIsFile = false;
      
      File outputDirectory = new File(strOutputDirectory);
      
      bExists = outputDirectory.exists();
      if (bExists)
         bIsFile = outputDirectory.isFile();
       
      if (!bExists)
      {
         boolean bSuccess = outputDirectory.mkdirs();
            
         if (!bSuccess)
            return E_IMAGE_NO_OUTPUT_DIR;
      }
      else if (bIsFile)
      {
         return E_IMAGE_NO_OUTPUT_DIR;
      }
      
      if (m_Metadata.thumbnails != null && m_Metadata.thumbnails.length > 0)
      {
         // Slightly double code to imc.lecturnity.converter.PictureExtracter.
                  
         Iterator iterator = ImageIO.getImageWritersByFormatName("jpg");
         ImageWriter writer = (ImageWriter)iterator.next();
         ImageWriteParam qualityParam = writer.getDefaultWriteParam();
         qualityParam.setCompressionMode(ImageWriteParam.MODE_EXPLICIT);
         qualityParam.setCompressionQuality(fJpegQuality);
         
         BufferedImage image = null;
         
         int iPageCount = 0;
         int iClipCount = 0;
         
         ThumbnailData[] aThumbData = m_Metadata.thumbnails;
         for (int i=0; i<aThumbData.length; ++i)
         {
               
            File outputFile = null;
            if (!aThumbData[i].isVideoClip) // normal page
            {
               if (m_Whiteboard == null)
                  return E_IMAGE_INTERNAL_FAILURE;
               
               outputFile = new File(outputDirectory, m_strFilenamePrefix+"_page"+iPageCount+".jpg");
               iPageCount++;
               
               Dimension dimPageSize = m_Whiteboard.GetBoardSize();
               Dimension dimImageSize = fitIn(dimPageSize, dimMaxImageSize);
         
               image = new BufferedImage(dimImageSize.width, 
                                         dimImageSize.height,
                                         BufferedImage.TYPE_INT_RGB);
               
               Graphics2D graphics = createGraphics(image);
                  
               graphics.setColor(Color.white);
               graphics.fillRect(0, 0, dimImageSize.width, dimImageSize.height);
            
               double dZoom = Math.min(dimImageSize.width/(double)dimPageSize.width,
                                       dimImageSize.height/(double)dimPageSize.height);
            
               long lPageEndTime = aThumbData[i].endTimestamp;
               if (i < aThumbData.length - 1)
                  lPageEndTime = aThumbData[i + 1].beginTimestamp - 1;
         
               m_Whiteboard.getEventQueue().paint((Graphics)graphics, (int)lPageEndTime, dZoom, true, true);
            }
            else
            {
               if (!NativeUtils.isLibraryLoaded())
                  return E_IMAGE_NO_NATIVEUTILS;
               
               outputFile = new File(outputDirectory, m_strFilenamePrefix+"_clip"+iClipCount+".jpg");
               iClipCount++;
               
               String strThumbPngName = aThumbData[i].thumbnailResource;
               String strAviName = strThumbPngName;
               int idx = strThumbPngName.lastIndexOf('.');
               if (idx > -1)
                  strAviName = strThumbPngName.substring(0, idx) + ".avi";
               
               File tempAviFile = createTemporaryAviFile(strAviName, outputDirectory);
               
               if (tempAviFile == null)
                  System.err.println("Error: Cannot create temporary avi file for "+strAviName);
              
               BufferedImage aviImage = null;
               if (tempAviFile != null)
                  aviImage = createImageFromAvi(tempAviFile+"");
               
               if (tempAviFile != null)
                  tempAviFile.delete();
               
               if (aviImage == null && tempAviFile != null)
                  System.err.println("Error: Cannot create image for "+strAviName);
               
               if (aviImage == null)
                  return E_IMAGE_NO_VIDEO_IMAGE;
               
               int iAviWidth = aviImage.getWidth();
               int iAviHeight = aviImage.getHeight();
               Dimension dimAviSize = new Dimension(iAviWidth, iAviHeight);
               
               Dimension dimAviImageSize = fitIn(dimAviSize, dimMaxImageSize);
                              
               image = new BufferedImage(dimAviImageSize.width, 
                                         dimAviImageSize.height,
                                         BufferedImage.TYPE_INT_RGB);
               
               Graphics2D graphics = createGraphics(image);
               
               graphics.setColor(Color.black);
               graphics.fillRect(0, 0, dimAviImageSize.width, dimAviImageSize.height);
            
            
               graphics.drawImage(
                  aviImage, 0, 0, dimAviImageSize.width, dimAviImageSize.height, null);
               
               
               // Fallback solution if nativeutils.dll is not present but
               // input is an LPD with clip thumb images.
               // Currently unused.
               if (false)
               {
                  int[] aiThumbnailSizes = null;
                  if (m_Resources != null)
                     aiThumbnailSizes = m_Resources.getThumbnailSizes();

                  String strThumbFileName = aThumbData[i].thumbnailResource;
                  if (aiThumbnailSizes != null && aiThumbnailSizes.length > 0)
                     strThumbFileName = aiThumbnailSizes[aiThumbnailSizes.length - 1]+"_"+strThumbFileName;
                  ImageIcon icon = createIcon(strThumbFileName);
            
                  if (icon != null)
                  {
                     //graphics.drawImage(icon.getImage(), 0, 0, 
                     //                   dimImageSize.width, dimImageSize.height, null);
                  }
               }
            }
            
            if (image == null)
               return E_IMAGE_INTERNAL_FAILURE;
            
            try
            {
               if (outputFile.exists())
                  outputFile.delete();
               
               writer.setOutput(ImageIO.createImageOutputStream(outputFile));
               IIOImage ioImage = new IIOImage(image, null, null);
               writer.write(null, ioImage, qualityParam);
            }
            catch (IOException exc)
            {
               exc.printStackTrace();
               return E_IMAGE_ENCODING;
            }
    
         }
         
         return aThumbData.length;
      }
     
      return 0;
   }

   private BufferedImage createImageFromAvi(String strAviFilename)
   {
      // Copied from PictureExtracter::fillAviImageBuffer()
      
      if (NativeUtils.isLibraryLoaded())
      {
         byte[] pixelData = NativeUtils.retrieveFirstVideoFrame(strAviFilename);
         
         VideoClipInfo vci = new VideoClipInfo(strAviFilename, 0);
         boolean bSuccess = NativeUtils.retrieveVideoInfo("", vci);
         
         if (bSuccess && pixelData != null)
         {
            BufferedImage bufferedImage = new BufferedImage(vci.videoWidth, vci.videoHeight,
                                                            BufferedImage.TYPE_INT_ARGB);
            int [] intPixels = new int[vci.videoWidth*vci.videoHeight];
            int c = 0;
            int p = 0;
            int padding = 4 - (vci.videoWidth * 3) % 4;
            if (padding == 4)
               padding = 0;

            for (int y=0; y<vci.videoHeight; ++y)
            {
               p = (vci.videoHeight - y - 1) * vci.videoWidth;

               for (int x=0; x<vci.videoWidth; ++x)
               {
                  intPixels[p] = (0xff000000 |
                                  ((pixelData[c+2] & 0xff) << 16) | 
                                  ((pixelData[c+1] & 0xff) << 8) | 
                                  (pixelData[c] & 0xff));
                  
                  ++p;
                  c += 3;
               }
               
               c += padding;
            }

            bufferedImage.setRGB(0, 0, vci.videoWidth, vci.videoHeight,
                                 intPixels, 0, vci.videoWidth);
            
            return bufferedImage;
         }
      }
      
      return null;
   }
   
   private File createTemporaryAviFile(String strAviResource, File outputDirectory)
   {
      try
      {
         InputStream in = m_Resources.createInputStream(strAviResource);
         
         if (in != null)
         {
            File tempOutputAvi = new File(outputDirectory, "459834temp_"+strAviResource);
            //tempOutputAvi.deleteOnExit(); // delete on the fly above
            
            FileOutputStream out = new FileOutputStream(tempOutputAvi);
            
            int iRead = 0;
            byte[] aBuffer = new byte[16*1024];
            
            while ((iRead = in.read(aBuffer)) > 0)
               out.write(aBuffer, 0, iRead);
            
            out.close();
            in.close();
            
            return tempOutputAvi;
         }
      }
      catch (IOException exc)
      {
         System.err.println("Error: Cannot create temporary avi file.");
         exc.printStackTrace();
      }
      
      return null;
   }
   
   private ImageIcon createIcon(String strFileName)
   {
      ImageIcon icon = null;
      
      if (m_Resources != null)
      {
         try
         {
            int fileSize = (int)m_Resources.getFileSize(strFileName);
            if (fileSize > 0)
            {
               byte[] imageData = new byte[fileSize];
               InputStream inputStream = m_Resources.createInputStream(strFileName);
               int read = 0;
               while (read < fileSize)
                  read += inputStream.read(imageData, read, fileSize-read);

               inputStream.close();

               icon = new ImageIcon(imageData);
            }
         }
         catch (IOException ioe)
         {
            ioe.printStackTrace();
         }
      }

      return icon;
   }
   
   private Dimension fitIn(Dimension dimSize, Dimension dimMaxSize)
   {
      return fitIn(dimSize, dimMaxSize, true);
   }
   
   private Dimension fitIn(Dimension dimSize, Dimension dimMaxSize, boolean bDoNotEnlarge)
   {
      double dZoom = 1.0;
      if (dimSize.width != dimMaxSize.width || dimSize.height != dimMaxSize.height)
      {
            dZoom = Math.min(dimMaxSize.width / (double)dimSize.width,
                             dimMaxSize.height / (double)dimSize.height);
      }
      
      if (dZoom > 1.0 && bDoNotEnlarge)
         return dimSize;
      
      Dimension dimOutputSize = new Dimension(
         (int)(dimSize.width * dZoom), (int)(dimSize.height * dZoom));
      
      return dimOutputSize;
   }
   
   private Graphics2D createGraphics(BufferedImage image)
   {
      Graphics2D graphics = image.createGraphics();
                
      RenderingHints antialiasRenderingHints = new RenderingHints(
         RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BICUBIC);
      antialiasRenderingHints.put(RenderingHints.KEY_ANTIALIASING,
                                  RenderingHints.VALUE_ANTIALIAS_ON);
      graphics.setRenderingHints(antialiasRenderingHints);
         
      return graphics;
   }
}