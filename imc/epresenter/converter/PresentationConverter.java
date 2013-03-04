package imc.epresenter.converter;

import imc.epresenter.filesdk.Archiver;
import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.Metadata;
import imc.epresenter.filesdk.ThumbnailData;

import imc.epresenter.filesdk.util.XmlParser;
import imc.epresenter.filesdk.util.XmlFormatException;
import imc.epresenter.filesdk.util.CountInputStream;
import imc.epresenter.filesdk.util.ByteCounter;
import imc.epresenter.filesdk.util.Localizer;

import imc.epresenter.player.whiteboard.EventQueue;
import imc.epresenter.player.audio.AiffFileReader;
import imc.epresenter.player.audio.AQSFileWriter;

import imc.lecturnity.converter.LPLibs;

import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;

import java.io.*;

import java.util.HashSet;
import java.util.Vector;
import java.util.Iterator;
import java.util.Date;

import java.text.SimpleDateFormat;

import java.awt.Dimension;
import java.awt.Color;

import javax.swing.JOptionPane;

import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.converter.wizard.PublisherWizardData;
import imc.lecturnity.converter.PictureExtracter;

import imc.lecturnity.util.VersionExtracter;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.FileUtils;

/**
 * This class implements some methods which make it quite easy to
 * convert AOF documents in the old format to the new one-file EPF
 * format. This class is used by the front-end tools {@link
 * BatchConverter BatchConverter} and {@link ConverterGui
 * ConverterGui}.
 * 
 * <p>Instances of this class are able to parse AOF configuration
 * files (having the suffix <tt>.aof</tt>) and find out the most of
 * the files used in the presentation by itself. A front-end, like the
 * {@link ConverterGui ConverterGui} may then allow a user to add
 * further files to the ePresentation using e.g. the {@link #addFile
 * addFile} method of this class.</p>
 *
 * <p><b>Usage:</b> Instanciate a <tt>PresentationConverter</tt> with
 * an AOF configuration file and an instance of a class implementing
 * the {@link ConversionDisplay ConversionDisplay} interface. Comments
 * and progress messages will be output to this display. Then, in a
 * second step, add further files to the ePresentation (removing files
 * is normally not needed and/or recommended; found files are being
 * referred to in the AOF configuration file and are thus probably
 * needed), if needed. In a third step you will need to provide an
 * archive name using the {@link #setArchiveName setArchiveName}
 * method (if not already done in the constructor). Then, call {@link
 * #performConvert performConvert} to write the new archive. Then you
 * may release the <tt>PresentationConverter</tt> instance.</p>
 *
 * @see ConverterGui
 * @see BatchConverter
 * @see ConversionDisplay
 * @see imc.epresenter.filesdk.Archiver
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class PresentationConverter implements ByteCounter
{
   /** 
    * New since version 1.8:
    * The library LPLibs is used to open the config file (*.lrd), 
    * add the tags for 'standard navigation' and write a modified 
    * new config file.
    */
   static
   {
      try
      {
         System.loadLibrary("LPLibs");
      }
      catch (UnsatisfiedLinkError e)
      {
         javax.swing.JOptionPane.showMessageDialog
            (null,
             "The dynamic link library 'LPLibs.dll' was not found in the path. Reinstalling the application may help.",
             "Error",
             javax.swing.JOptionPane.ERROR_MESSAGE);
         System.exit(0);
      }
   }
   
   /** 
    * The main version of this tool. This is read out from this resource:
    * <tt>/imc/lecturnity/converter/VERSION</tt>.
    * Note that this does not contain the patch level! If you need
    * the patch level, too, use SHORT_VERSION or PRINTABLE_VERSION.
    * This variable should normally not be used, as it is too imprecise.
    * 
    * @see #SHORT_VERSION
    * @see #PRINTABLE_VERSION
    */
   public static String MAIN_VERSION = "0.0.0";

   /**
    * The internal release, is read out from
    * <tt>/imc/lecturnity/converter/INTERNAL_RELEASE</tt>
    */
   public static String INTERNAL_RELEASE = "0";

   /**
    * A printable version, like "1.6.0.p3.b12 Campus"
    */
   public static String PRINTABLE_VERSION = "0.0.0";

   /**
    * A short version string, including the patch level,
    * e.g. "1.6.0.p3".
    */
   public static String SHORT_VERSION = "0.0.0";

   public static final int SUCCESS = 0;
   public static final int CANCELLED = -1;
   public static final int NO_PIC_EXTRACT = -2;

   private static String ERR_THUMBS = "[!]";

   private static final Object[][] SUFFIX_WEIGHTS = {{".jpg", new Double(1d)},
                                                     {".gif", new Double(1d)},
                                                     {".png", new Double(1d)},
                                                     {".obj", new Double(.1d)},
                                                     {".evq", new Double(.05d)},
                                                     {".mov", new Double(1d)},
                                                     {".avi", new Double(1d)},
                                                     {".mp3", new Double(1d)}};
   
   private static final double DEFAULT_WEIGHT = .5d;
   private static final double AQS_TO_AIF_WEIGHT = 3.0/16.0;

   /**
    * The thumbnails sizes used for creation of thumbnails. 100, 150
    * and 200 pixels.
    */
   public static final int THUMB_SIZES[] = {100, 150, 200};

   // the size of the audio file inputstream buffer
   private static final int AUDIOCONVERT_BUFFER = 65536;

   private static Localizer localizer_ = null;
                                                     
   static
   {
      try
      {
         localizer_ = new Localizer("/imc/epresenter/converter/PresentationConverter_",
                                    "en");
         VersionExtracter ve = new VersionExtracter("/imc/lecturnity/converter/VERSION");
         MAIN_VERSION = ve.getVersion();
         ve = new VersionExtracter("/imc/lecturnity/converter/INTERNAL_RELEASE");
         INTERNAL_RELEASE = ve.getVersion();
         PRINTABLE_VERSION = NativeUtils.getVersionString(INTERNAL_RELEASE);
         SHORT_VERSION = NativeUtils.getVersionStringShort();

         ERR_THUMBS = localizer_.getLocalized("ERR_THUMBS");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open the Locale database!",
                                       "Severe Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   /**
    * Calling this class from the command prompt will result in the
    * {@link ConverterGui ConverterGui} to be started.
    */
   public static void main(String[] args)
   {
      System.out.println("Not startable. Starting Converter Wizard...");
      imc.lecturnity.converter.ConverterWizard.main(args);
   }

   private Archiver archiver_ = null;
   private ConversionDisplay conversionDisplay_ = null;
   private boolean convertAudio_ = false;
   private boolean inProgress_ = false;
   private boolean cancelRequest_ = false;

   private AQSFileWriter aqsWriter_ = null;

   // full path to the archive, without suffix .epf
   private String archiveName_ = null;
   // path with trailing File.separator to the directory of the
   // original presentation
   private String currentDir_ = null;

   // the name of the configuration file (i.e., aof or epf file). only
   // the file name (without path, see currentDir_)
   private String configFile_ = null;
   // the name of the .obj file, without path (see currentDir_)
   private String objFile_    = null;

   // the set of files that are used in the presentation, NOT INCLUDED
   // are the audio files and the configuration file.
   private HashSet fileSet_   = null;

   private HashSet externalFileSet_ = null;

   // a list of the audio files in this archive. these are handled
   // separately as they must not be compressed on archivation.
   private Vector  audioVector_ = null;

   // true if the created acrhived is intended to be used as a help
   // file (header is "EPFH" instead of "EPF ").
   private boolean isHelpDocument_ = false;

   // the set of used ttf fonts
   private HashSet usedFontsSet_ = null;
   // true if fonts should be embedded, false default
   private boolean embedFonts_ = false;

   // true if full screen mode startup is activated, false is default
   private boolean startInFullScreen_ = false;
   // true if video clips should be displayed on top of slides, false 
   // is default
   private boolean showClipsOnSlides_ = false;

   private boolean createThumbnails_ = false;
   private boolean addMetadataTag_ = false;
   private String metadataFileName_ = null;

   // if set to true, an "_eval" resource is added to the presentation document
   private boolean isEvalVersion_ = false;

   private PublisherWizardData pwData_;
   

   /**
    * Constructs a new <tt>PresentationConverter</tt> instance with
    * the given parameters. The configuration file will be parsed for
    * other files lying in the directory of the config file. If an
    * AOF object file (having the extension <tt>.obj</tt>) is found,
    * this file will also parsed for image files belonging to the
    * presentation. In order to retrieve the names of the files found,
    * use the {@link #getAudioFiles getAudioFiles} and {@link #getFiles
    * getFiles} methods.
    * 
    * @param configFile the full path to the configuration
    * (<tt>.epf</tt> or <tt>.aof</tt>) file of the presentation to
    * convert
    * @param archiveName the full path to the archive to
    * create. <b>Note</b>: the suffix <tt>.epf</tt> will be appended
    * to this name
    * @param conversionDisplay the {@link ConversionDisplay
    * ConversionDisplay} instance to output comments to, or
    * <tt>null</tt> to use the {@link DefaultConversionDisplay
    * DefaultConversionDisplay}.
    * @param convertAudio <i>true</i> if the audio should be converted
    * into the AQS format.
    */
   public PresentationConverter(String configFile, 
                                String archiveName,
                                ConversionDisplay conversionDisplay,
                                PublisherWizardData pwData, 
                                boolean convertAudio) 
      throws IOException
   {
//       localizer_ = new Localizer("/imc/epresenter/converter/PresentationConverter_",
//                                  "en");

      archiveName_       = archiveName;
      conversionDisplay_ = conversionDisplay;
      // audioConverter_    = audioConverter;
      convertAudio_      = convertAudio;

      pwData_ = pwData;

      initPath(configFile);

      fileSet_           = new HashSet();
      externalFileSet_   = new HashSet();
      usedFontsSet_      = new HashSet();
      audioVector_       = new Vector();

      if (!hasCorrectHeader())
         throw new XmlFormatException
            (localizer_.getLocalized("ERR_Malformed_AOF_header"));
      
      parseConfigFile();
      if (objFile_ != null)
         parseObjFile();
   }

   /**
    * Sets the archive name to use. This method has only impact if
    * called before calling the {@link #performConvert performConvert}
    * method. This is method is specifically useful if you want to
    * supply an archive file name later than retrieving the files from
    * the config file.
    * 
    */
   public void setArchiveName(String archiveName)
   {
      archiveName_ = archiveName;
   }

   /**
    * Use this method and give <i>true</i> as an argument if you want
    * the <tt>PresentationConverter</tt> to create a help document
    * (carrying the header <tt>EPFH</tt> instead of <tt>EPF</tt>).
    *
    * @param b <i>true</i> if a help document should be created
    */
   public void setCreateHelpDocument(boolean b)
   {
      isHelpDocument_ = b;
   }

   /**
    * @return <i>true</i> if {@link #setCreateHelpDocument
    * shouldCreateHelpDocument} has been called with a <i>true</i>
    * argument.
    * @see #setCreateHelpDocument(boolean)
    */
   public boolean shouldCreateHelpDocument()
   {
      return isHelpDocument_;
   }

   /**
    * By using this method, you can have the Player start in its full
    * screen mode at startup with the created document. Default is
    * <i>false</i>.
    *
    * @param b <i>true</i> if full screen mode startup should be
    * activated.
    */
   public void setStartInFullScreen(boolean b)
   {
      startInFullScreen_ = b;
   }

   /**
    * Use this method in order to create a document in which video
    * clips are displayed on top of the slides instead of in the video
    * window. Defaults to <i>false</i>.
    *
    * @param b <i>true</i> if clips should be display on top of the
    * slides instead of in the video window.
    */
   public void setShowClipsOnSlides(boolean b)
   {
      showClipsOnSlides_ = b;
   }

   /**
    *
    */
   public void setEmbedFonts(boolean b)
   {
//       System.out.println("setEmbedFonts(" + b + ")");

      embedFonts_ = b;
   }

   /**
    *
    */
   public boolean shouldEmbedFonts()
   {
//       System.out.println("shouldEmbedFonts() == " + embedFonts_);
      
      return embedFonts_;
   }

   public void setCreateThumbnails(boolean b)
   {
      createThumbnails_ = b;
   }

   public void setIsEvalVersion(boolean b)
   {
      isEvalVersion_ = b;
   }

   private void initPath(String fileName)
   {
      File configFile = new File(fileName);
      configFile_ = configFile.getName();
      currentDir_ = configFile.getAbsolutePath();
      currentDir_ = currentDir_.substring(0, currentDir_.length() - configFile_.length());
      
      conversionDisplay_.logMessage(localizer_.getLocalized("Presentation_dir") + currentDir_);
      // System.out.println("config file dir: " + currentDir_);
   }

   private boolean hasCorrectHeader()
   {
      File configFile = new File(currentDir_ + configFile_);
      try
      {
         FileInputStream fis = new FileInputStream(configFile);
         BufferedReader in = FileUtils.createBufferedReader(fis, null);

         String firstLine = in.readLine().trim();
         
         in.close();
         
         return (firstLine.toUpperCase().startsWith("<DOCUMENT>"));
      }
      catch (Exception e)
      {
         e.printStackTrace();
         return false;
      }
   }

   private void parseConfigFile()
   {
      conversionDisplay_.logMessage(localizer_.getLocalized("Parsing_config_file"));
      // Next line: "scanInput" is defined in LecturnityWriterWizardPanel::defineProgressBarSteps
      conversionDisplay_.displayCurrentProgressStep("scanInput", localizer_.getLocalized("Parsing_config_file"));
      conversionDisplay_.displayCurrentProgress(0.0f);
      try
      {
         InputStream in = new FileInputStream(currentDir_ + configFile_);
         XmlParser parser = new XmlParser(in);

         String thisTag = "";
         boolean ignore = true;

         int tokenType =  XmlParser.TAG_START;
         while (tokenType != XmlParser.EOF)
         {
            tokenType = parser.getNextTag();
            switch (tokenType)
            {
            case XmlParser.TAG_START:
               String tag = parser.getToken();
               if (tag.equals("AUDIO") ||
                   tag.equals("OPT") ||
                   tag.equals("METADATA"))
                  ignore = false;
               thisTag = tag.toUpperCase();
               break;

            case XmlParser.TAG_END:
               ignore = true;
               break;

            case XmlParser.CONTENT:
               if (!ignore)
                  checkToken(parser.getToken(), thisTag);
               break;

            default:
               break;
            }
         }
         
         in.close();
      }
      catch (XmlFormatException e)
      {
         e.printStackTrace();
      }
      catch (IOException e)
      {
         e.printStackTrace();
      }
      conversionDisplay_.displayCurrentProgress(1.0f);
      conversionDisplay_.logMessage(localizer_.getLocalized("Parsing_config_done"));
   }

   private void addStandardNavigationToConfigFile(String configFileName)
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      int[] anNavigationStates = new int[7];
      anNavigationStates[0] = ps.GetIntValue("iControlBarVisibility");
      anNavigationStates[1] = ps.GetIntValue("iNavigationButtonsVisibility");
      anNavigationStates[2] = ps.GetIntValue("iTimeLineVisibility");
      anNavigationStates[3] = ps.GetIntValue("iTimeDisplayVisibility");
      anNavigationStates[4] = ps.GetIntValue("iDocumentStructureVisibility");
      anNavigationStates[5] = ps.GetIntValue("iPlayerSearchFieldVisibility");
      anNavigationStates[6] = ps.GetIntValue("iPlayerConfigButtonsVisibility");

      LPLibs.templatePutStandardNavigationStatesToLrd(configFileName, anNavigationStates);
   }

   private void checkToken(String token, String inTag)
   {
      File file = new File(currentDir_ + token);
      if (file.exists())
      {

         // we now add avis, too.
//          if (!token.toLowerCase().endsWith(".avi"))
//          {

         // workaround: in some aof files, the aof file itself is
         // mentioned in the helper <OPT> tags. in that case, the
         // file must not be written once more into the archive.
         if (token.equals(configFile_))
            return;
         
         String lowerCaseToken = token.toLowerCase();
         if (lowerCaseToken.endsWith(".obj"))
            objFile_ = token;
         
         if (inTag.equals("AUDIO"))
            audioVector_.addElement(token);
         else {
            // Bugfix 5705 (4.0.p6): 
            // Ignore clip in Demo Documents
            boolean bIsClipInDemoDocument = false;
            if ( (lowerCaseToken.indexOf("_clip", 0) >= 0) && (lowerCaseToken.endsWith(".avi"))) {
                DocumentData dd = pwData_.GetDocumentData();
                if (!dd.HasMultipleVideos()) 
                    bIsClipInDemoDocument = true;
            }
            
            if (!bIsClipInDemoDocument)
               fileSet_.add(token);
         }
         
         if (lowerCaseToken.endsWith(".appevq") &&
             file.length() > 0)
         {
            String extApps = retrieveExtAppNames(token);
            conversionDisplay_.logMessage
               (localizer_.getLocalized("WARN_External_scripts") + " (" + 
                token + ").\n" +
                localizer_.getLocalized("WARN_External_scripts2") + "\n" +
                extApps);
         }
         
         if (lowerCaseToken.endsWith("_jedas.ini"))
         {
            conversionDisplay_.logMessage
               (localizer_.getLocalized("WARN_Jedas") + " '" + 
                token +"').\n" + 
                localizer_.getLocalized("WARN_Jedas2"));
         }
         
         // we now add avi's, too.
//          }
//          else
//             conversionDisplay_.logMessage
//                ("WARNING: Ignored '" + token + "' (AVI video).");

      }
   }

   private String retrieveExtAppNames(String fileName)
   {
      String stripAppEvq = fileName.substring(0, fileName.length()-7);

      File file = new File(currentDir_ + stripAppEvq + ".appobj");
      try
      {
         String scripts = "";
         int nr = 1;
         InputStream in = new FileInputStream(file);
         XmlParser parser = new XmlParser(in);

         int tokenType = XmlParser.TAG_START;
         boolean ignore = true;
         while (tokenType != XmlParser.EOF)
         {
            tokenType = parser.getNextTag();
            switch (tokenType)
            {
            case XmlParser.TAG_START:
               if (parser.getToken().toUpperCase().equals("APP"))
                  ignore = false;
               break;

            case XmlParser.TAG_END:
               ignore = true;
               break;

            case XmlParser.PROPERTY:
               if (!ignore)
               {
                  String propertyName = parser.getToken();
                  if (propertyName.toLowerCase().equals("filename"))
                     scripts += (nr++) + ".) " + parser.getPropertyOfToken() + '\n';
               }
               break;
               
            default:
               // ignore all other things
               break;
            }
         }

         in.close();
         
         return scripts;
      }
      catch (IOException e)
      {
         return "<error reading \"" + stripAppEvq + ".appobj\">";
      }
   }

   private void parseObjFile() throws IOException
   {
      File objFile = new File(currentDir_ + objFile_);
      conversionDisplay_.displayCurrentFile(objFile_, (int) objFile.length(), 
                                            localizer_.getLocalized("Parsing"));
      conversionDisplay_.logMessage(localizer_.getLocalized("PARSE_OBJ_FILE"));
      
      CountInputStream tmpIn = new CountInputStream
         (new FileInputStream(currentDir_ + objFile_),
          this);
                                                    
      XmlParser objParser = new XmlParser(tmpIn);

      int tokenType = XmlParser.TAG_START;
      boolean inTextItem = false;
      boolean ignore = true;
      while (tokenType != XmlParser.EOF)
      {
         tokenType = objParser.getNextTag();
         switch (tokenType)
         {
         case XmlParser.TAG_START:
            if (objParser.getToken().toUpperCase().equals("IMAGE"))
                ignore = false;
            if (objParser.getToken().equalsIgnoreCase("TEXTITEM"))
               inTextItem = true;
            break;

         case XmlParser.TAG_END:
            ignore = true;
            inTextItem = false;
            break;

         case XmlParser.PROPERTY:
            if (!ignore)
            {
               String key = objParser.getToken();
               String value = objParser.getPropertyOfToken();
               if (key.equalsIgnoreCase("fname"))
                  checkToken(value, "IMAGE");
            }
            if (inTextItem)
            {
               String key = objParser.getToken().toLowerCase();
               String value = objParser.getPropertyOfToken();
               if (key.equalsIgnoreCase("ttf"))
                  usedFontsSet_.add(value);
            }
            break;
         }
      }
      
      tmpIn.close();
      
      conversionDisplay_.logMessage(localizer_.getLocalized("PARSE_OBJ_DONE"));
   }

   /**
    * Returns the current directory of this instance.
    * @return the current directory of this instance.
    */
   public String getCurrentDir()
   {
      return "" + currentDir_;
   }
   
   /**
    * Returns the name of the AOF config fíle. <b>Note:</b> This file
    * name does not contain the presentation directory this files lies
    * in! It is just the file name.
    * @return the name of the AOF config fíle.
    */
   public String getConfigFileName()
   {
      return configFile_;
   }

   /**
    * Returns an array of the audio files used in this
    * presentation. Like in the {@link #getConfigFileName
    * getConfigFileName} method, this method only returns the file
    * names, and not the full paths.
    *
    * @return an array of the audio files used in this
    * presentation
    */
   public String[] getAudioFiles()
   {
      String[] audioFiles = new String[audioVector_.size()];
      audioVector_.copyInto(audioFiles);
      return audioFiles;
   }

   /**
    * Returns all other files contained in the presentation which were
    * found during instanciation or that have been added using {@link
    * #addFile addFile}. Once more, this method only returns the file
    * names, without the paths.
    *
    * @return an array of other files in the presentation */
   public String[] getFiles()
   {
      String[] files = new String[fileSet_.size()];
      Iterator fileIterator = fileSet_.iterator();
      
      for (int i=0; fileIterator.hasNext(); i++)
         files[i] = (String) fileIterator.next();

      return files;
   }

   /**
    * Returns an array of the external files added to this archive
    * using the {@link addExternalFile addExternalFile} method.
    * @retun an array of external files.
    * @see #addExternalFile(String)
    */
   public String[] getExternalFiles()
   {
      String[] files = new String[externalFileSet_.size()];
      Iterator fileIterator = externalFileSet_.iterator();
      
      for (int i=0; fileIterator.hasNext(); i++)
         files[i] = (String) fileIterator.next();

      return files;
   }

   /**
    * Returns an array of the used TrueType fonts of the presentation.
    * @return an array of the used TrueType fonts of the presentation.
    */
   public String[] getUsedTTFonts()
   {
      String[] fontList = new String[usedFontsSet_.size()];
      Iterator iter = usedFontsSet_.iterator();
      int i=0;
      while (iter.hasNext())
         fontList[i++] = (String) (iter.next());
      return fontList;
   }

   /**
    * Add a file as it is from the AOF config file directory to this
    * ePresentation archive. The file to add has to exist and it must
    * not contain any paths.
    *
    * @param fileName the name of the file to add to the archive
    * @return <i>true</i> if the adding of the file was successful,
    * <i>false</i> otherwise. Error messages are output to the {@link
    * ConversionDisplay ConversionDisplay} instance.
    */
   public boolean addFile(String fileName)
   {
      if (fileName.equals(configFile_))
      {
         conversionDisplay_.logMessage
            (localizer_.getLocalized("ERR_Cannot_add_config") + fileName);
         return false;
      }

      // check if already exists (possibly as external file)
      boolean externalFileSetContainsFile = false;
      Iterator iter = externalFileSet_.iterator();
      while (iter.hasNext())
      {
         String fn = (String) iter.next();
         File ff = new File(fn);
         if (ff.getName().equals(fileName))
            externalFileSetContainsFile = true;
      }

      if (externalFileSetContainsFile)
      {
         conversionDisplay_.logMessage
            (localizer_.getLocalized("ERR_Add_res_exists1") + "'" + fileName +
             "': " + localizer_.getLocalized("ERR_Add_res_exists2"));
         return false;
      }

      File file = new File(currentDir_ + fileName);
      if (file.exists())
      {
         boolean success = fileSet_.add(fileName);
         if (success)
            conversionDisplay_.logMessage
               (localizer_.getLocalized("Added_file") + fileName);
         else
            conversionDisplay_.logMessage
               (localizer_.getLocalized("ERR_File_in_archive") + fileName);
         return true;
      } 
      else
      {
         conversionDisplay_.logMessage
            (localizer_.getLocalized("ERR_File_not_found") +
             fileName + " (" + currentDir_ + ").");
         return false;
      }
   }

   /**
    * Removes the given file (without path!) from the archive.
    *
    * @param file the file to remove from the archive
    * @return <i>true</i> if the removal was successful, <i>false</i> otherwise.
    */
   public boolean removeFile(String file)
   {
      return (fileSet_.remove(file));
   }

   /**
    * Adds a file to the archive which is not in the current
    * directory. The given <tt>fileName</tt> has to contain the full
    * path to the file to include. The alias of the resource in the
    * archive is the file name only, not including the
    * path. E.g. adding <tt>C:\WINNT\Fonts\ARIAL.TTF</tt> will result
    * in a resource <tt>ARIAL.TTF</tt> in the {@link
    * imc.epresenter.filesdk.FileResources FileResources} object.
    *
    * @param fileName the file name of the external file to add to the
    *   archive.
    * @return <i>true</i> if the add operation was successful. Error
    * messages are output to the {@link ConversionDisplay
    * ConversionDisplay} instance of this
    * <tt>PresentationConverter</tt>.
    * @see #removeExternalFile(String);
    */
   public boolean addExternalFile(String fileName)
   {
      // check if exists (as normal file)
      if (fileSet_.contains((new File(fileName)).getName()))
      {
         conversionDisplay_.logMessage
            (localizer_.getLocalized("ERR_Add_res_exists1") + "'" + fileName +
             "': " + localizer_.getLocalized("ERR_Add_res_exists2"));
         return false;
      }

      File file = new File(fileName);
      if (file.exists())
      {
         boolean success = externalFileSet_.add(fileName);
         if (success)
            conversionDisplay_.logMessage
               (localizer_.getLocalized("Added_external") + fileName);
         else
            conversionDisplay_.logMessage
               (localizer_.getLocalized("ERR_Ext_exists") + fileName);
         return success;
      }
      else
      {
         conversionDisplay_.logMessage
            (localizer_.getLocalized("ERR_File_not_found") + fileName);
         return false;
      }
   }

   /**
    * Removes the given external file from the archive.
    * @param fileName the external file to remove
    * @return <i>true</i> if the operation was successful
    * @see #addExternalFile(String)
    */
   public boolean removeExternalFile(String fileName)
   {
      return (externalFileSet_.remove(fileName));
   }

   /**
    * Calculate and return an estimated size in bytes of the current
    * archive. You may choose if audio conversion should be taken into
    * account or not.
    */
   public long getEstimatedArchiveSize(boolean convertAudio)
   {
      long estimatedSize = 0;
      
      for (int i=0; i<audioVector_.size(); i++)
      {
         File file = new File(currentDir_ + ((String) audioVector_.elementAt(i)));
         if (convertAudio)
            estimatedSize += (long) (AQS_TO_AIF_WEIGHT * file.length());
         else
            estimatedSize += file.length();
      }

      String[] files = getFiles();
      for (int i=0; i<files.length; i++)
         estimatedSize += getWeightedFileSize(currentDir_ + files[i]);

      String[] externalFiles = getExternalFiles();
      for (int i=0; i<externalFiles.length; i++)
         estimatedSize += getWeightedFileSize(externalFiles[i]);

      return estimatedSize;
   }

   private long getWeightedFileSize(String fileName)
   {
      File file = new File(fileName);
      long size = file.length();
      double weight = DEFAULT_WEIGHT;
      for (int i=0; i<SUFFIX_WEIGHTS.length; i++)
      {
         String suffix = (String) SUFFIX_WEIGHTS[i][0];
         if (fileName.endsWith(suffix))
            weight = ((Double) SUFFIX_WEIGHTS[i][1]).doubleValue();
      }

      return (long) (weight * size);
   }

   /**
    * Performs the conversion of the presentation. Using this method
    * you ca supply a flag which tells the instance to convert audio
    * or not (in case you have changed your mind).
    *
    * @param convertAudio <i>true</i> if audio files should be
    * converted to the AQS format
    */
   public int performConvert(boolean convertAudio) throws IOException,
      UnsupportedAudioFileException
   {
      convertAudio_ = convertAudio;
      return performConvert();
   }

   /**
    * Performs the conversion of the presentation using the current
    * settings. The progress of the conversion will be reported to the
    * given {@link ConversionDisplay ConversionDisplay} instance (see
    * constructor).
    * 
    * @throws UnsupportedAudioFileException if the audio converter
    * cannot handle the audio format in the original presentation
    * (only in case audio conversion is enabled).
    */
   public int performConvert() throws IOException,
      UnsupportedAudioFileException
   {
      DocumentData dd = pwData_.GetDocumentData();
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      inProgress_ = true;
      cancelRequest_ = false;
      addMetadataTag_ = false;

      // here we store all temporary files. 
      // these will be deleted after conversion has finished.
      Vector tempFiles = new Vector();

      // introduced a try... finally block on order to
      //  (a) reset the archiver_ variable in case of exceptions,
      //  (b) definitely delete all temporary files in case of exceptions.
      try
      {
         if (archiveName_ == null)
            throw new IOException(localizer_.getLocalized("ERR_No_archive_name"));

         if (createThumbnails_ && dd.GetMetadata() == null)
         {
            // LMD file name must not be determined, it is already known in DocumentData.
//            int dotPos = configFile_.lastIndexOf('.');
//            dd.SetMetadataFileName(configFile_.substring(0, dotPos) + ".lmd");
            addMetadataTag_ = true;
         }
         
         //
         // AUDIO (CONVERSION) PART
         //
         String[] realAudioFiles = null;
         
///         writeTemporaryEpfFile(archiveName_ + ".epf.tmp");
///         tempFiles.addElement(archiveName_ + ".epf.tmp");
         
         if (convertAudio_)
         {
            conversionDisplay_.logMessage(localizer_.getLocalized("CONVERT_AUDIO"));
            // Next line: "convertAudio" is defined in LecturnityWriterWizardPanel::defineProgressBarSteps
            conversionDisplay_.displayCurrentProgressStep("convertAudio", localizer_.getLocalized("CONVERT_AUDIO"));
            conversionDisplay_.displayCurrentProgress(0.0f);
            realAudioFiles = convertAudioFiles();
            for (int i=0; i<realAudioFiles.length; i++)
               tempFiles.addElement(realAudioFiles[i]);
            
            conversionDisplay_.displayCurrentProgress(1.0f);
            conversionDisplay_.logMessage(localizer_.getLocalized("CONVERT_AUDIO_DONE"));
         }
         
         if (cancelRequest_)
            return CANCELLED;
         
         conversionDisplay_.logMessage(localizer_.getLocalized("Writing_archive") + 
                                       " '" + archiveName_ + "'.");

         archiver_ = new Archiver(archiveName_, currentDir_, 
                                  conversionDisplay_);

         archiver_.setCreateHelpDocument(isHelpDocument_);
         archiver_.setStartInFullScreen(startInFullScreen_);
         archiver_.setShowClipsOnSlides(showClipsOnSlides_);

         //
         // CONFIG FILE (EVENTUALLY MODIFIED) AND AUDIO PART
         //

         // config file is always written out to a temporary file,
         // even if it is not altered.
         writeTemporaryEpfFile(archiveName_ + ".epf.tmp");
         tempFiles.addElement(archiveName_ + ".epf.tmp");
         // New in 1.8.0:
         addStandardNavigationToConfigFile(archiveName_ + ".epf.tmp");
         archiver_.addConfigFileAs(archiveName_ + ".epf.tmp", configFile_);

         if (convertAudio_)
         {
            String[] audioFiles = getAudioFiles();
            // do not compress audio
            for (int i=0; i<audioFiles.length; i++)
               archiver_.addFileAs(realAudioFiles[i], audioFiles[i], false); 
         }
         else
         {
            // archiver_.addConfigFile(configFile_);
            String[] audioFiles = getAudioFiles();
            for (int i=0; i<audioFiles.length; i++)
               archiver_.addFile(audioFiles[i], false); // do not compress audio
         }

         if (cancelRequest_)
            return CANCELLED;
         
         //
         // OTHER FILES PART, LIKE EVQ AND OBJ
         //
         String[] otherFiles = getFiles();
         for (int i=0; i<otherFiles.length; i++)
         {
            // do not compress mp3 or avi/mov files (you can't)
            boolean compress = !(otherFiles[i].toLowerCase().endsWith(".mp3") ||
                                 otherFiles[i].toLowerCase().endsWith(".avi") ||
                                 otherFiles[i].toLowerCase().endsWith(".mpeg") ||
                                 otherFiles[i].toLowerCase().endsWith(".mov"));
            archiver_.addFile(otherFiles[i], compress); // compress all other files
         }

         if (cancelRequest_)
            return CANCELLED;
         
         //
         // EXTERNAL FILES PART
         //
         String[] externalFiles = getExternalFiles();
         for (int i=0; i<externalFiles.length; i++)
         {
            File t = new File(externalFiles[i]);
            
            // do not compress mp3 or avi/mov files (you can't)
            boolean compress = !(externalFiles[i].toLowerCase().endsWith(".mp3") ||
                                 externalFiles[i].toLowerCase().endsWith(".avi") ||
                                 externalFiles[i].toLowerCase().endsWith(".mpeg") ||
                                 externalFiles[i].toLowerCase().endsWith(".mov"));
            // use only file name as alias in archive:
            archiver_.addFileAs(externalFiles[i], t.getName(), compress);
         }
         
         if (cancelRequest_)
            return CANCELLED;
         
         //
         //  EMBED FONTS PART
         //
         if (shouldEmbedFonts())
         {
            String fontNames[] = getUsedTTFonts();
            
            String fontsDir = "";
            
            // after the NativeUtils getEnv function works:
            String windowsDir =
               imc.lecturnity.util.NativeUtils.getEnvVariable("WINDIR");

            if (windowsDir == null)
            {
               conversionDisplay_.logMessage
                  (localizer_.getLocalized("WARN_NO_WINDIR"));
            }
            else
            {
               if (!windowsDir.equals(""))
                  fontsDir = windowsDir + File.separatorChar + "Fonts" +
                     File.separatorChar;
               
               //             fontsDir = "C:\\WINNT\\Fonts\\";
               
               if (!fontsDir.equals(""))
               {
                  for (int i=0; i<fontNames.length; i++)
                  {
                     boolean isEmbedded = false;
                     for (int j=0; j<FileResources.EMBEDDED_FONTS.length; j++)
                     {
                        if (fontNames[i].equals(FileResources.EMBEDDED_FONTS[j]))
                           isEmbedded = true;
                     }
                     
                     // we do not want to embed already embedded fonts!
                     if (!isEmbedded)
                     {
                        // First try without Fonts Dir path (full path fonts)
                        boolean fontFound = false;
                        File fontFile = new File(fontNames[i]);
                        if (fontFile.exists())
                        {
                           archiver_.addFileAs(fontNames[i],
                                               fontNames[i], true);
                           fontFound = true;
                        }

                        // If not found, try the presentation directory (currentDir_)
                        if (!fontFound)
                        {
                           fontFile = new File(currentDir_ + fontNames[i]);
                           if (fontFile.exists())
                           {
                              // add and compress ttf file
                              archiver_.addFileAs(currentDir_ + fontNames[i],
                                                  fontNames[i], true);
                              fontFound = true;
                           }
                        }
                        
                        // If still not found, try the Windows Fonts dir
                        if (!fontFound)
                        {
                           fontFile = new File(fontsDir + fontNames[i]);
                           if (fontFile.exists())
                           {
                              // add and compress ttf file
                              archiver_.addFileAs(fontsDir + fontNames[i],
                                                  fontNames[i], true);
                              fontFound = true;
                           }
                        }

                        if (!fontFound)
                        {
                           conversionDisplay_.logMessage
                              (localizer_.getLocalized("WARN_FONT_NOT_FOUND") +
                               fontNames[i]);
                        }
                     }
                  }
               }
            }
         }

         if (cancelRequest_)
            return CANCELLED;

         // we will need a FileResources object for the further
         // progress of the conversion... let's create one here!
         FileResources resources = 
            FileResources.createFileResources(dd.GetPresentationFileName());

         //
         // THUMBNAILS PART
         //
         if (createThumbnails_)
         {
            // ok, we want to create thumbnails for this
            // presentation. for that we need a FileResources instance
            // (for reading event and object queue)

            // tell the user what we're doing: preparing write of thumbnails
            conversionDisplay_.logMessage(localizer_.getLocalized("PREP_THUMBS"));

            // the picture extracter extracts png pictures of the
            // presentation of an arbitrary size. it makes use of
            // the EventQueue and ObjectQueue classes of the
            // whiteboard player of the Player.
            PictureExtracter pe; 
            try
            {
                pe = new PictureExtracter(pwData_, resources, new Dimension(100, 100), 
                                          conversionDisplay_);
            }
            catch (Exception e)
            {
               e.printStackTrace();
               System.out.println("Unable to create PictureExtracter.");
               
               throw new IOException(ERR_THUMBS + e.getMessage());
            }

            // for the thumbnails, make the picture extracter draw a
            // black border around them
            pe.setDrawFrame(false); // no 3d frame
            pe.setDrawBorderLine(true);
            pe.setBorderLineColor(Color.black);

            // create a thumbnail resource information file
            String thumbInfoFile = archiveName_ + "thumbinfo.tmp";
            writeThumbInfoFile(thumbInfoFile);
            archiver_.addFileAs(thumbInfoFile, "_internal_thumbinfo", true);
            tempFiles.addElement(thumbInfoFile);

            boolean[] bThumbnailsSizes = { ps.GetBoolValue("bLpdThumbnailsSizeSmall"), 
                                           ps.GetBoolValue("bLpdThumbnailsSizeMedium"), 
                                           ps.GetBoolValue("bLpdThumbnailsSizeLarge") };

            if (dd.GetMetadata() != null)
            {
               // if there is a imc.epresenter.filesdk.Metadata
               // structure given for this presentation, then it was
               // written with a new (>=1.10) version of the
               // Lecturnity Assistant

               conversionDisplay_.logMessage(localizer_.getLocalized("CREATE_THUMBS"));
               // Next line: "createThumbnails" is defined in LecturnityWriterWizardPanel::defineProgressBarSteps
               conversionDisplay_.displayCurrentProgressStep("createThumbnails", localizer_.getLocalized("CREATE_THUMBS"));
               conversionDisplay_.displayCurrentProgress(0.0f);

               ThumbnailData[] t = dd.GetMetadata().thumbnails;

               conversionDisplay_.displayCurrentFile(localizer_.getLocalized("THUMB_THUMB"),
                       THUMB_SIZES.length * t.length,
                       localizer_.getLocalized("THUMB_CREATE"));

               for (int thumbs=0; thumbs<THUMB_SIZES.length; thumbs++)
               {
                  if (bThumbnailsSizes[thumbs])
                  {
                     //conversionDisplay_.displayCurrentFile(localizer_.getLocalized("THUMB_THUMB"),
                     //                                      t.length,
                     //                                      localizer_.getLocalized("THUMB_CREATE"));

                     int thisSize = THUMB_SIZES[thumbs];
                     pe.setPictureSize(new Dimension(thisSize, thisSize));

                     // we only need to write a screengrabbing clip image
                     // once for each thumbnail size:
                     boolean hasWrittenClipImage = false;
                     
                     for (int i=0; i<t.length; i++)
                     {
                        String tmpFileName = archiveName_ + "_" + i + "_" + 
                           thisSize + ".tmp";
                        boolean success = false;

                        if (t[i].isVideoClip)
                        {
                           // first, try to extract the first video frame.
                           try
                           {
                              String fileName = getClipFileNameFromTime(t[i].beginTimestamp);
                              if (fileName == null)
                                 throw new Exception("Could not retrieve file name for clip '" + 
                                                     t[i].title + "'.");
                              pe.writeAviFramePngPicture(currentDir_ + fileName, tmpFileName);

                              success = true;
                           }
                           catch (Exception ex)
                           {
                              ex.printStackTrace();

                              // oops. notify the user:
                              JOptionPane.showMessageDialog(null, "Could not extract thumbnail for video clip.",
                                                            "Warning", JOptionPane.WARNING_MESSAGE);
                              // that did not work out; write standard image
                              
                              if (!hasWrittenClipImage)
                              {
                                 InputStream clipImageStream = getClass().getResourceAsStream
                                    ("/imc/lecturnity/converter/images/" + thisSize + "_clip.png");
                                 if (clipImageStream != null)
                                 {
                                    try
                                    {
                                       FileOutputStream fileOut = new FileOutputStream(tmpFileName);
                                       FileUtils.copyFile(clipImageStream,
                                                          fileOut,
                                                          null,
                                                          "Please wait");
                                       fileOut.flush();
                                       fileOut.close();
                                       success = true;
                                    }
                                    catch (IOException e)
                                    {
                                    }
                              
                                    clipImageStream.close();
                                    // if writing the clip thumbnail image failed, still
                                    // do not try again:
                                    hasWrittenClipImage = true;
                                 }
                              }
                           }
                        }
                        else
                        {
                           // (The last parametet 'true' includes the drawing of interactive parts)
                           pe.writePngPicture(((int) t[i].endTimestamp)-1, 
                                              tmpFileName, true);
                           success = true;
                        }
                        
                        if (success)
                        {
                           tempFiles.addElement(tmpFileName);
                           archiver_.addFileAs(tmpFileName, 
                                               thisSize + "_"  + 
                                               t[i].thumbnailResource, true);
                           
                           //conversionDisplay_.displayCurrentFileProgress(i+1);
                           conversionDisplay_.displayCurrentFileProgress(thumbs*t.length + i+1);
                        }
                        
                        if (cancelRequest_)
                           break;
                     }
                  }

                  if (cancelRequest_)
                     break;
               }
               conversionDisplay_.displayCurrentProgress(1.0f);
            }
            else
            {
               EventQueue.PageObject[] pages = pe.getEventQueue().getPageObjects();

               conversionDisplay_.logMessage(localizer_.getLocalized("CREATE_THUMBS"));
               // Next line: "createThumbnails" is defined in LecturnityWriterWizardPanel::defineProgressBarSteps
               conversionDisplay_.displayCurrentProgressStep("createThumbnails", localizer_.getLocalized("CREATE_THUMBS"));
               conversionDisplay_.displayCurrentProgress(0.0f);

               String tempLmdFileName = archiveName_ + ".lmd.tmp";

               int dotPos = configFile_.lastIndexOf('.');
               String baseName = configFile_.substring(0, dotPos);

               writeMetadata(pages, tempLmdFileName, baseName);
               archiver_.addFileAs(tempLmdFileName,
                                   dd.GetMetadataFileName(), true);

               // mark this temporary file to be deleted:
               tempFiles.addElement(tempLmdFileName);

               for (int thumbs=0; thumbs<THUMB_SIZES.length; thumbs++)
               {
                  if (bThumbnailsSizes[thumbs])
                  {
                     conversionDisplay_.displayCurrentFile(localizer_.getLocalized("THUMB_THUMB"),
                                                           pages.length,
                                                           localizer_.getLocalized("THUMB_CREATE"));

                     int thisSize = THUMB_SIZES[thumbs];
                     pe.setPictureSize(new Dimension(thisSize, thisSize));

                     for (int i=0; i<pages.length; i++)
                     {
                        String tmpFileName = archiveName_ + "_" + i + "_" + thisSize + ".tmp";
                        int startTime = pages[i].getTimeBegin();
                        int endTime   = pages[i].getTimeEnd();
                        pe.writePngPicture(endTime, tmpFileName);
                        tempFiles.addElement(tmpFileName);
                        archiver_.addFileAs(tmpFileName, 
                                            thisSize + "_" + baseName + "_" + startTime + "_thumb.png", 
                                            true);

                        //conversionDisplay_.displayCurrentFileProgress(i+1);
                        conversionDisplay_.displayCurrentFileProgress(thumbs*pages.length + i+1);

                        if (cancelRequest_)
                           break;
                     }
                  }

                  if (cancelRequest_)
                     break;
               }
            }
         }
         
         if (cancelRequest_)
            return CANCELLED;

         //
         // AUDIO MARKING PART
         //
         String audioMarkString = resources.getMarkString();
         String tempMarkFileName = archiveName_ + "_typeinfo.tmp";

         if (audioMarkString == null)
         {
            // no marking exists, let's create one!
            // defaults to university version...int
            audioMarkString = "u;" + SHORT_VERSION + ";";
            SimpleDateFormat sdf = new
               SimpleDateFormat("HH:mm:ss;yyyy.MM.dd");
            audioMarkString += sdf.format(new Date());
         }
         writeMarkInfoFile(tempMarkFileName, audioMarkString);
         // add it to the archive:
         archiver_.addFileAs(tempMarkFileName, "_internal_versiontypeinfo", true);
         tempFiles.addElement(tempMarkFileName); // delete afterwards

         //
         // ACTUAL ARCHIVE WRITE PART
         //
         conversionDisplay_.logMessage(localizer_.getLocalized("WRITE_DATA"));
            archiver_.writeArchive();

         if (!cancelRequest_) {
            conversionDisplay_.logMessage(localizer_.getLocalized("WRITE_DATA_DONE"));
            conversionDisplay_.displayCurrentProgress(1.0f);
         }

         //
         // DELETE TEMPORARY FILES PART
         //
         // display small message that temp files are deleted
         conversionDisplay_.logMessage(localizer_.getLocalized("DELETE_TEMPS"));
         // Next line: "cleaningUp" is defined in LecturnityWriterWizardPanel::defineProgressBarSteps
         conversionDisplay_.displayCurrentProgressStep("cleaningUp", localizer_.getLocalized("DELETE_TEMPS"));
         conversionDisplay_.displayCurrentProgress(0.0f);

         for (int i=0; i<tempFiles.size(); i++)
         {
            String tempFile = (String) (tempFiles.elementAt(i));
            deleteFile(tempFile);
         }

         if (cancelRequest_)
            return CANCELLED;

         //
         // INTERACTION PART
         //
         // Interaction:
         // We have to look, if one (or more) files have to be copied into 
         // the target directory which should be accessible by "open-file"/"open-url"
         // --> using methods from "LPLibs"

         // Get paths and names
         File destFile = new File(archiveName_);
         String destName = destFile.getName();
         String destDir = destFile.getAbsolutePath();
         destDir = destDir.substring(0, destDir.length() - destName.length());
         String srcName = configFile_.substring(0, configFile_.length()-4);
         // Set OBJ/EVQ input files
         String strEvqFileName = dd.GetEvqFileName();
         String strObjFileName = dd.GetObjFileName();
         String evqFile = currentDir_ + strEvqFileName;
         String objFile = currentDir_ + strObjFileName;
         LPLibs.lpdPrepareCopiesOfInteractiveOpenedFiles(new File(currentDir_, strEvqFileName)+"", 
                                                         new File(currentDir_, strObjFileName)+"", 
                                                         new File(destDir + destName + ".lpd")+"");


         conversionDisplay_.logMessage(localizer_.getLocalized("Conversion_finished"));
         conversionDisplay_.displayCurrentProgress(1.0f);

         return SUCCESS;
      }
      finally
      {
         // System.out.println("hfkjdhfkljdshflskjdhflvkcdhfvjksahdnfkvjhndsafvklnahdfklvhnk");
         archiver_ = null;
         inProgress_ = false;

         // let's check those temp files again...
         for (int i=0; i<tempFiles.size(); i++)
         {
            String tempFileName = (String) (tempFiles.elementAt(i));
            File tempFile = new File(tempFileName);
            if (tempFile.exists())
               deleteFile(tempFileName);
         }
      }
   }

   private String getClipFileNameFromTime(long startTime)
   {
      DocumentData dd = pwData_.GetDocumentData();

      String fileName = null;

      if (dd.GetVideoClipInfos() != null)
      {
         for (int i = 0; i < dd.GetVideoClipInfos().length; ++i)
         {
            if (startTime >= dd.GetVideoClipInfos()[i].startTimeMs)
               fileName = dd.GetVideoClipInfos()[i].clipFileName;
         }
      }

      return fileName;
   }

   public boolean isInProgress()
   {
      return inProgress_;
   }

   public void cancelConversion()
   {
      cancelRequest_ = true;
      if (archiver_ != null)
      {
//          System.out.println("Archiver: heeer uff");
         archiver_.cancelOperation();
      }
      if (aqsWriter_ != null)
         aqsWriter_.cancelWrite();
   }

   private void writeTemporaryEpfFile(String fileName)
      throws IOException
   {
      FileInputStream fis = new FileInputStream(currentDir_ + configFile_);
      BufferedReader in = FileUtils.createBufferedReader(fis, null);
      
      PrintWriter out = FileUtils.CreatePrintWriter(fileName);
      
      int audioCount = 0;
      String readLine = "";
      while (readLine != null)
      {
         readLine = in.readLine();
         if (readLine != null)
         {
            int audioPos = readLine.toUpperCase().indexOf("<AUDIO>");
            int filesPos = readLine.toUpperCase().indexOf("</FILES>");

            if (convertAudio_ && audioPos != -1)
            {
               // next occurance of '<'
               int audioEndPos = readLine.indexOf('<', audioPos + 1);
               String beginning = readLine.substring(0, audioPos + 7);
               String end       = readLine.substring(audioEndPos,
                                                     readLine.length());
               String audioFile = (String) audioVector_.elementAt(audioCount);
               int dotPos = audioFile.lastIndexOf('.');
               audioFile = audioFile.substring(0, dotPos);
               
               out.println(beginning + audioFile + ".aqs" + end);
               audioCount++;
            } 
            else if (addMetadataTag_ && filesPos != -1)
            {
               out.println("    <METADATA>" + pwData_.GetDocumentData().GetMetadataFileName() +
                           "</METADATA>");
               out.println(readLine);
            } 
            else
               out.println(readLine);
         }
      }

      out.flush();
      out.close();
      in.close();
   }

   private void writeMetadata(EventQueue.PageObject[] pages, String tempFileName, 
                              String baseName)
      throws IOException
   {
      PrintWriter out = FileUtils.CreatePrintWriter(tempFileName);
      
      out.println("<docinfo>");
      out.println("<metainfo>");
      out.println("</metainfo>");
      out.println("<structure>");
      out.println("<chapter title=\"" + localizer_.getLocalized("DOCUMENT") +
                  "\" nr=\"1\">");
      
      String pageLocalized = localizer_.getLocalized("PAGE");

      for (int i=0; i<pages.length; i++)
      {
         int time = pages[i].getTimeBegin();
         out.println("  <page nr=\"" + (i+2) + "\" begin=\"" + time +
                     "\" end=\"" + pages[i].getTimeEnd() + "\" " +
                     "thumb=\"" + baseName + "_" + time + "_thumb.png" + "\">" +
                     pageLocalized + pages[i].getPageIndex() +
                     "</page>");
      }

      out.println("</chapter>");
      out.println("</structure>");
      out.println("</docinfo>");
      out.flush();
      out.close();
   }

   private void writeThumbInfoFile(String fileName)
      throws IOException
   {
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      // do not use utf-8 here; no non-Ascii content anyway
      PrintWriter out = new PrintWriter(new FileWriter(fileName));

      boolean[] bThumbnailsSizes = { ps.GetBoolValue("bLpdThumbnailsSizeSmall"), 
                                     ps.GetBoolValue("bLpdThumbnailsSizeMedium"), 
                                     ps.GetBoolValue("bLpdThumbnailsSizeLarge") };

      out.println("<thumbs>");
      for (int i=0; i<THUMB_SIZES.length; i++)
      {
         if (bThumbnailsSizes[i])
            out.println("  <size>" + THUMB_SIZES[i] + "</size>");
      }
      out.println("</thumbs>");

      out.flush();
      out.close();
   }

   private void writeEvalInfoFile(String fileName)
      throws IOException
   {
      // do not use utf-8 here; no non-Ascii content anyway
      PrintWriter out = new PrintWriter(new FileWriter(fileName));
      // just print the PresentationConverter version
      out.println(SHORT_VERSION);
      out.flush();
      out.close();
   }

   private void writeMarkInfoFile(String fileName, String content)
      throws IOException
   {
//       System.out.println("writing '" + content + "' to mark info file.");

      // do not use utf-8 here; it is used for audio mark string which is Ascii
      PrintWriter out = new PrintWriter(new FileWriter(fileName));
      out.print(content); // _not_ println!
      out.flush();
      out.close();
   }

   private String[] convertAudioFiles()
      throws IOException, UnsupportedAudioFileException
   {
      // AiffFileReader aiffReader = new AiffFileReader();
      try
      {
         aqsWriter_ = new AQSFileWriter();
         
         String[] convertedAudioFiles = new String[audioVector_.size()];
         
         for (int i=0; i<audioVector_.size() && !cancelRequest_; i++)
         {
            String audioFile = (String) audioVector_.elementAt(i);
         
            int dotPos = audioFile.lastIndexOf('.');
            String aqsAudioFile = audioFile.substring(0, dotPos) + ".aqs";
            
            convertedAudioFiles[i] = archiveName_ + "_" + aqsAudioFile +
               ".tmp";
            
            File tmpFile = new File(currentDir_ + audioFile);
            conversionDisplay_.displayCurrentFile(audioFile, 
                                                  (int) tmpFile.length(),
                                                  localizer_.getLocalized("Converting_to_AQS"));
            
            CountInputStream countIn = new CountInputStream
               (new BufferedInputStream(new FileInputStream(tmpFile), AUDIOCONVERT_BUFFER), this);
            
//             OutputStream fileOut = new BufferedOutputStream
//                (new FileOutputStream(convertedAudioFiles[i]), AUDIOCONVERT_BUFFER);
            
            try 
            {
               aqsWriter_.write(AudioSystem.getAudioInputStream(countIn),
                                new File(convertedAudioFiles[i]), 3f, true);
            } 
            catch (UnsupportedAudioFileException e) 
            {
               if (audioFile.toLowerCase().endsWith(".aif")) 
               {
                  // we'll try the AiffFileReader first, before bailing
                  // out
                  countIn = new CountInputStream
                     (new BufferedInputStream(new FileInputStream(tmpFile), AUDIOCONVERT_BUFFER), this);
                  
//                   fileOut = new BufferedOutputStream
//                      (new FileOutputStream(convertedAudioFiles[i]), AUDIOCONVERT_BUFFER);
                  
                  AiffFileReader aiffReader = new AiffFileReader();
                  aqsWriter_.write(aiffReader.getAudioInputStream(countIn),
                                   new File(convertedAudioFiles[i]), 3f, true);
               }
               else
                  throw e;
            }
            //          aqsWriter_.write(aiffReader.getAudioInputStream(countIn),
            //                          fileOut, 3f);
            
//             fileOut.flush();
//             fileOut.close();
         }


         // at last, convert the names
         Vector tmpVector = new Vector();
         for (int i=0; i<audioVector_.size(); i++)
         {
            String aifFile = (String) audioVector_.elementAt(i);
            int dotPos = aifFile.lastIndexOf('.');
            String aqsFile = aifFile.substring(0, dotPos) + ".aqs";
            tmpVector.addElement(aqsFile);
         }
         audioVector_ = tmpVector;

         return convertedAudioFiles;
      }
      finally
      {
         aqsWriter_ = null;
      }
   }

   private void deleteFile(String file)
   {
      File fileToDelete = new File(file);
      if (fileToDelete.exists())
      {
         boolean success = false;
         for (int i=0; i<3; i++)
         {
            success = fileToDelete.delete();
            if (!success)
            {
               System.out.println("Delete failed, retry #" + (i+1) + ".");
               fileToDelete = new File(file);
               try
               {
                  Thread.currentThread().sleep(100);               
               }
               catch (InterruptedException e)
               {
                  break;
               }
            }
            else
               break;
         }

         if (!success)
            conversionDisplay_.logMessage
               (localizer_.getLocalized("ERR_Delete_temp") +
                " '" + file + "'.");
      }
      else
         conversionDisplay_.logMessage
            (localizer_.getLocalized("ERR_Delete_Fnf") + file);
   }

   /**
    * This method is for internal purposes only. Do not call manually!
    * It is only used in conjunction with a {@link
    * imc.epresenter.filesdk.util.CountInputStream CountInputStream}
    * in order to keep track of how many bytes of a file have been
    * read.
    *
    * @see imc.epresenter.filesdk.util.CountInputStream
    */
   public void displayCountedBytes(int bytesCounted)
   {
      conversionDisplay_.displayCurrentFileProgress(bytesCounted);
   }
}
