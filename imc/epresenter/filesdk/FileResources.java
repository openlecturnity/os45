package imc.epresenter.filesdk;

/*
 * File:             FileResources.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: FileResources.java,v 1.55 2010-03-22 13:03:50 ziewer Exp $
 */

import java.io.*;

import java.util.Vector;

import imc.epresenter.filesdk.util.XmlParser;
import imc.epresenter.filesdk.util.XmlFormatException;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;

/**
 * This class is used by applications wanting to gain access to
 * resources in ePresentation archives, e.g. the ePresentation
 * Player. An application will use the name of a configuration file
 * (<tt>.epf</tt> file) or an ePresentation archive (also
 * <tt>.epf</tt> suffix) with the {@link #createFileResources
 * createFileResources} method in order to get a fitting instance of
 * this class. That may either be a {@link SimpleFileResources
 * SimpleFileResources} instance (for a traditional AOF style
 * <tt>.epf</tt> file) or an {@link ArchivedFileResources
 * ArchivedFileResources} instance. Ths application does not need to
 * bother which kind of instance it gets, it will simply make use of
 * the two methods {@link #createConfigFileInputStream
 * createConfigFileInputStream} and {@link #createInputStream
 * createInputStream} methods in order to create {@link
 * java.io.InputStream InputStream}s for the given resources.
 *
 * @see ArchivedFileResources
 * @see SimpleFileResources
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public abstract class FileResources 
{
   // old style documents had either 0x48 (=H) or this (SPACE)
   protected static int BITMASK_NORMAL_DOCUMENT = 0x20;
                                     
   protected static int BITMASK_HELP_DOCUMENT = 0x48;
   protected static int BITMASK_CLIPS_ON_SLIDES = 0x80;
   protected static int BITMASK_START_FULLSCREEN = 0x04;

   /**
    * The embedded fonts (fonts contained within the Lecturnity Suite).
    */
   public static final String[] EMBEDDED_FONTS = {"arial.ttf", "cour.ttf",
                                                  "times.ttf", "symbol.ttf"};
   
   
   protected String m_strCodepage;

   /**
    * Creates a new <tt>FileResources</tt> instance. The actual class
    * of the created object depends of the type of ePresentation file
    * the <tt>fileName</tt> parameter points to. See class
    * introduction.
    *
    * @param fileName the file name (full path) to the ePresentation
    * to create a <tt>FileResources</tt> object for.
    */
   public static final FileResources createFileResources(String fileName)
      throws IOException
   {
      if (hasNewFormat(fileName))
         return new ArchivedFileResources(fileName);
      else
         return new SimpleFileResources(fileName);
   }

   /**
    * Returns <i>true</i> if the {@link java.io.File File} has the new
    * ePresentation format, <i>false</i> otherwise. <b>Note:</b> It is
    * not said that if this method returns <i>false</i>, the file has
    * the <i>old</i> format. It may be illegal, too. This method will
    * return <i>true</i> for ePresentation Help files, too.
    */
   public static boolean hasNewFormat(File file)
      throws IOException
   {
      /*
      FileInputStream in = new FileInputStream(file);
      byte[] epfHeader = new byte[4];
      int readBytes = in.read(epfHeader);
      in.close();
      
      boolean headerCorrect = (epfHeader[0] == 'E' &&
                               epfHeader[1] == 'P' &&
                               epfHeader[2] == 'F');
                               */
      return readAndCheckLpdHeader(file+"", new byte[4]);
   }

   /**
    * @see #hasNewFormat(java.io.File)
    */
   public static boolean hasNewFormat(String fileName)
      throws IOException
   {
      return hasNewFormat(new File(fileName));
   }

   /**
    * Returns <i>true</i> if the {@link java.io.File File} has the new
    * ePresentation format and is a help file, <i>false</i>
    * otherwise. <b>Note:</b> It is not said that if this method
    * returns <i>false</i>, the file has the <i>old</i> format. It may
    * be illegal, too.  
    */
   public static boolean isHelpDocument(File file)
      throws IOException 
   {
      byte[] header = new byte[4];
      boolean headerCorrect = readAndCheckLpdHeader(file+"", header);
    
      return (headerCorrect && ((header[3] & BITMASK_HELP_DOCUMENT) != 0));
   }

   /**
    * @see #isHelpDocument(java.io.File)
    */
   public static boolean isHelpDocument(String fileName)
      throws IOException
   {
      return isHelpDocument(new File(fileName));
   }


   /**
    * Checks if the given document shall display clips on slides.
    * As this is a purely informative method it throws not Exceptions
    * but catches them and returns false.
    */
   public static boolean isClipsOnSlidesDocument(String fileName)
   {
      try
      {
         byte[] header = new byte[4];
         boolean headerCorrect = readAndCheckLpdHeader(fileName, header);
         
         return (headerCorrect && ((header[3] & BITMASK_CLIPS_ON_SLIDES) != 0));
      }
      catch (IOException exc)
      {
      }
      
      return false;
      
   }

   /**
    * Checks if the given document shall automatically switch to 
    * fullscreen display after loading.
    * As this is a purely informative method it throws not Exceptions
    * but catches them and returns false.
    */
   public static boolean isStartInFullscreenDocument(String fileName)
   {
      try
      {
         byte[] header = new byte[4];
         boolean headerCorrect = readAndCheckLpdHeader(fileName, header);
         
         return (headerCorrect && ((header[3] & BITMASK_START_FULLSCREEN) != 0));
      }
      catch (IOException exc)
      {
      }
      
      return false;
      
   }

   private static boolean readAndCheckLpdHeader(String fileName, byte[] header)
   throws IOException
   {
      FileInputStream in = new FileInputStream(new File(fileName));
      int readBytes = in.read(header);
      in.close();
      
      boolean headerCorrect = (header[0] == 'E' &&
                               header[1] == 'P' &&
                               header[2] == 'F');
      
      return (readBytes == 4 && headerCorrect);
   }


   /**
    * Creates an {@link java.io.InputStream InputStream} for the
    * configuration file of the current ePresentation which is
    * represented by the current instance of <tt>FileResources</tt>.
    *
    * @throws ResourceNotFoundException if the resource of the
    * configuration file could not be found
    * @throws IOException of some kind, of any other I/O error has
    * occurred.
    */
   public abstract InputStream createConfigFileInputStream()
      throws ResourceNotFoundException, IOException;

   /**
    * Creates an {@link java.io.InputStream} for the given
    * <tt>resourceName</tt> in the ePresentation represented by the
    * current <tt>FileResources</tt> instance. This
    * <tt>InputStream</tt> is generally not buffered, so if you need a
    buffer, wrap it around the returned input stream.
    *
    * @param resourceName the name of the resource to create an {@link
    * java.io.InputStream InputStream} of.
    * @throws ResourceNotFoundException if the resource could not be
    * found 
    * @throws IOException of some kind, if any other I/O error
    * has occurred.  
    */
   public abstract InputStream createInputStream(String resourceName) 
      throws ResourceNotFoundException, IOException;

   /**
    * Creates an {@link java.io.InputStream} for the given
    * <tt>resourceName</tt> in the ePresentation represented by the
    * current <tt>FileResources</tt> instance (in case of an
    * <ttArchivedFileResources</tt> instance of <tt>FileResources</tt>
    * this will be an <tt>XorInputStream</tt>, and <b>not</b> a
    * <tt>BufferedInputStream</tt>!). This <tt>InputStream</tt> is
    * buffered with a buffer of the size
    * <tt>bufferSize</tt>. <b>Note for archives:</b> It is the file
    * itself (the xor'ed data) which is buffered in this
    * case. Normally you should create an <tt>InputStream</tt> using
    * the {@link #createInputStream(String) createInputStream} method
    * and then wrapping a <tt>BufferedInputStream</tt> around it. This
    * method is only intended for those case where that is not possible
    * (e.g. if you need the functionality of the {@link XorInputStream
    * XorInputStream}).
    *
    * @param resourceName the name of the resource to create an {@link
    * java.io.InputStream InputStream} of.
    * @param bufferSize the size of the read buffer to use.
    * @throws ResourceNotFoundException if the resource could not be
    * found 
    * @throws IOException of some kind, if any other I/O error
    * has occurred.  
    */
   public abstract InputStream createBufferedInputStream
      (String resourceName, int bufferSize)
      throws ResourceNotFoundException, IOException;
   
   /**
    * This method returns an <tt>InputStream</tt> to the TrueType file
    * specified in the parameter <tt>ttfName</tt>. This method will
    * first look in the LPD archive (or in the current directory if
    * this object represents an LRD document). Secondly, this method
    * will look for the font object in the current resources (jar
    * file). If the font file still has not been found, the font will
    * be looked for in the Windows Fonts directory
    * (<tt>$WINDIR$/Fonts</tt>). If everything fails, <tt>null</tt> is
    * returned.
    */
   public InputStream createFontStream(String ttfName)
   {
      InputStream fontStream = null;

      // first look in the archive/current directory
      try
      {
         fontStream = createInputStream(ttfName);
      }
      catch (Exception e)
      {
         fontStream = null;
      }

      // second, look in the jar file
      if (fontStream == null)
      {
         fontStream = getClass().getResourceAsStream
            ("/imc/epresenter/filesdk/fonts/" + ttfName);
      }

      // if still not found, look in the windows directory
      if (fontStream == null)
      {
         if (NativeUtils.isLibraryLoaded())
         {
            String fontsDir = "";
            String winDir = imc.lecturnity.util.NativeUtils.getEnvVariable("WINDIR");
            if (winDir != null)
            {
               if (!winDir.equals(""))
                  fontsDir = winDir + File.separatorChar + "Fonts" +
                     File.separatorChar;
               
               File fontFile = new File(fontsDir + ttfName);
               if (fontFile.exists())
               {
                  try
                  {
                     fontStream = new FileInputStream(fontFile);
                  }
                  catch (FileNotFoundException e)
                  {
                     fontStream = null;
                  }
               }
            }
         }
      }

      return fontStream;
   }

   /**
    * Returns the file size of the given resource, or <tt>-1</tt> if
    * the resource was not found.
    *
    * @return the size of the resource, or <tt>-1</tt> if not found
    */
   public abstract long getFileSize(String resourceName);

   /**
    * This method returns a <tt>File</tt> instance pointing to the
    * resource given in <tt>resourceName</tt>. This method will only
    * return <i>real</i> <tt>File</tt> instances, that is, you cannot
    * retrieve a <tt>File</tt> instance for a resources bundled in an
    * archive.
    *
    * <p>This method will firstly concatenate the name with the
    * current path and check if that file exists. Otherwise, it will
    * check whether the string in <tt>resourceName</tt> is a complete
    * path, and take that file if it exists.
    *
    * @param resourceName the resource to retrieve a <tt>File</tt>
    * instance for.
    * @return a <tt>File</tt> instance for the given resource, or
    * <tt>null</tt> if the file does not exists.  
    */
   public File getFile(String resourceName)
   {
      String strippedResourceName = (new File(resourceName)).getName();

      File file = new File(getCurrentDirectory() + strippedResourceName);
      if (file.exists())
         return file;

      file = new File(resourceName);
      if (file.exists())
         return file;

      return null;
   }

   public boolean StartFileOrUrl(String strTodo) throws IOException
   {
      if (!NativeUtils.isLibraryLoaded())
         return false;
      
      if (strTodo.indexOf(':') > -1)
      {
         // parameter is either an URL or an absolute path
         return  NativeUtils.startFile(strTodo);
      }
      else
      {
         // parameter is a local or archived file
         
         // You needn't do security checks here (i.e. for 
         // relative addressing with "..\.." or other strang things
         // as both cases below are very special in how the file is
         // found.
         // However just to make sure:
         
         if (strTodo.indexOf('\\') > -1 || strTodo.indexOf('/') > -1)
         {
            System.err.println("File path has wrong format to be executed: "+strTodo);
            // TODO proper error dialog??
            return false;
         }
         
         
         File fileTodo = getFile(strTodo);
         if (fileTodo != null)
         {
            // parameter is an existing local file
            return NativeUtils.startFile(fileTodo.getPath());
         }
         else
         {
            // probably file not found
            return false;
               
         }
         
         /* case for archived files; currently not supported/used
         else
         {
            // parameter might be archived, if so copy it to a 
            // temporary directory and start from there
            
            int idxDot = strTodo.lastIndexOf('.');
            if (idxDot > -1 && idxDot > 0 && idxDot < strTodo.length() - 1)
            {
               String strSuffix = strTodo.substring(idxDot + 1);
               String strPrefix = strTodo.substring(0, idxDot);
               
               InputStream archivedStream = createInputStream(strTodo);
               
               if (archivedStream != null)
               {
                  File fileTemp = File.createTempFile(strPrefix, strSuffix, null);
                  fileTemp.deleteOnExit();

                  FileOutputStream theOutputStream = new FileOutputStream(fileTemp);

                  int bytesRead = 0;
                  byte buf[];
                  int bufSize = 8192;
                  buf = new byte[bufSize]; 
                  while (bytesRead != -1)
                  {
                     bytesRead = archivedStream.read(buf, 0, bufSize);
                     if (bytesRead != -1)
                        theOutputStream.write(buf, 0, bytesRead);
                  }
                  theOutputStream.close();
                  archivedStream.close();
                  
                  return NativeUtils.startFile(fileTemp.getPath());;
               }
               else
               {
                  System.err.println("File for execution not found in archive: "+strTodo);
                  // TODO proper error dialog?
                  return false;
               }
            }
            else
            {
               System.err.println("File name has wrong format to be executed: "+strTodo);
               // TODO proper error dialog??
               return false;
            }
         }
         */
      }
   }
   
   /**
    * Returns an URL to the given resource for use with the Java Media
    * Framework. If the given resource cannot be located (e.g. the
    * <tt>ArchivedFileResources</tt> instance looks for the resource
    * first in the archive, then as a regular file), this method will
    * return <tt>null</tt>.
    */
   public abstract String getResourceURL(String resource);

   /**
    * This method returns the current directory (the main directory of
    * the ePresentation) as string, including the last
    * <tt>File.separatorChar</tt> character.
    */
   protected abstract String getCurrentDirectory();

   /**
    * Returns a <tt>java.util.Hashtable</tt> containing {@link
    * FileStruct FileStruct} instances of files in the archive, if
    * this feature is available. Otherwise, <tt>null</tt> is returned.
    *
    * @return a <tt>java.util.Hashtable</tt> of <tt>FileStruct</tt>
    * instances.
    */
   public abstract java.util.Hashtable getArchivedFiles();

   /**
    * Returns the document type. If the document type is invalid, -1
    * is returned.
    * @see imc.lecturnity.util.NativeUtils
    */
   public abstract int getDocumentType() throws IOException;

   /**
    * Returns the eventual mark string of this document. If the
    * document is not marked, this method will return <tt>null</tt>.
    *
    * @since 1.1.2
    */
   public abstract String getMarkString();

   /**
    * Returns the document version, i.e. the version of the Lecturnity
    * Suite which was used to record this document. This method will
    * return "1.0.0" if the document is not marked.
    *
    * @return the version of the document, in "x.y.z" style.
    * @see #getMarkString()
    * @since 1.4.0
    */
   public String getDocumentVersion()
   {
      String mark = getMarkString();
      if (mark == null)
         return "1.0.0";

      int semi1 = mark.indexOf(";");
      if (semi1 < 0)
         return "1.0.0";
      int semi2 = mark.indexOf(";", semi1 + 1);
      if (semi2 < 0)
         return "1.0.0";

      return mark.substring(semi1 + 1, semi2);
   }

   /**
    * This method skips exactly <tt>skipBytes</tt> if there exist that
    * many bytes in the stream. If not, an <tt>EOFException</tt> will
    * be thrown.
    *
    * @param in the <tt>InputStream</tt> to skip within
    * @param skipBytes the number of bytes to skip
    * @return the number of bytes skipped, i.e. always
    * <tt>skipBytes</tt> if successful
    */
   protected long skipFully(InputStream in, long skipBytes)
      throws IOException
   {
      long skipped = 0;
      long totalSkip = 0;
      while (totalSkip < skipBytes)
      {
         skipped = in.skip(skipBytes - totalSkip);
         totalSkip += skipped;
      }

      return totalSkip;
   }

   /**
    * Creates a {@link Metadata Metadata} structure of the LMD file
    * given in the parameter <tt>metadataResource</tt>.
    */
   public Metadata getMetadata(String metadataResource)
      throws IOException
   {
      return getMetadata(createInputStream(metadataResource));
   }

   /**
    * Creates a {@link Metadata Metadata} structure of the LMD file
    * in the given <tt>InputStream</tt>. The thumbnail file names
    * given in the {@link ThumbnailData ThumbnailData} field in the
    * returned structure are not the real file names of the thumbnail
    * resources. You will have to use the {@link #getThumbnailSizes()
    * getThumbnailSizes} method to retrieve an array of thumbnail
    * sizes and then insert that size, followed by an underscore, in front of
    * the file name of the thumbnail. E.g., a thumbnail is called
    * <tt>presentation_12345_thumb.png</tt>, and there are thumbnails
    * in the sizes 100, 150 and 200. Then the corresponding file names
    * are <tt>100_presentation_12345_thumb.png</tt> and so on.
    *
    * @see #getThumbnailSizes()
    */
   public Metadata getMetadata(InputStream metadataStream)
      throws IOException
   {
      BufferedReader reader = FileUtils.createBufferedReader(metadataStream, m_strCodepage);
      
      Metadata md = FileResources.createMetadata(reader);
      metadataStream.close();
      return md;
   }

  
   protected static Metadata createMetadata(Reader reader)
      throws IOException
   {
      XmlParser parser = new XmlParser(reader);

      int ttype = XmlParser.TAG_START;

      Vector thumbs = new Vector();
      Vector thumbsRemoved = new Vector();

      int number = -1;
      int chapterNumber = -1;
      String chapterTitle = null;
      String title = null;
      String thumbnailResource = null;
      int chapterLevel = 0;
      long beginTimestamp = -1;
      long endTimestamp = -1;
      String pageType = null;

      boolean inPage = false;
      // <CLIP> and <PAGE> tags are treated equally
      boolean isPageClip = false;
      boolean inStructure = false;
      boolean firstInChapter = false;

      boolean chapterPending = false;

      Vector chapterStack = new Vector();

      boolean inMetainfo = false;
      boolean inTitle = false;
      boolean inAuthor = false;
      boolean inCreator = false;
      boolean inRecordDate = false;
      boolean inRecordLength = false;
      boolean inKeyword = false;
      boolean inNr = false;
      boolean inBegin = false;
      boolean inEnd = false;
      boolean inThumb = false;
      boolean inType = false;
      boolean inFileInfo = false;
      boolean inShowClickPages = false;

      String mTitle = "";
      String mAuthor = "";
      String mCreator = "";
      String mRecordDate = "";
      String mRecordLength = "";

      Vector mKeywords = new Vector();
      Vector pageKeywords = null;
      
      String[] lastChapters = {};

      boolean hasNewStyleTitle = false;

      //       try {  // this was for debugging

      while (ttype != XmlParser.EOF)
      {
         ttype = parser.getNextTag();
         
         switch (ttype)
         {
         case XmlParser.TAG_START: {
            String tag = parser.getToken().toUpperCase();

            //             System.out.println("TAG_START: <" + tag + ">");
            
            if (tag.equals("STRUCTURE"))
               inStructure = true;
            else if (tag.equals("METAINFO"))
               inMetainfo = true;
            else if (tag.equals("FILEINFO"))
                inFileInfo = true;
            else if (inStructure)
            {
               if (inPage)
               {
                  if (tag.equals("TITLE"))
                     inTitle = true;
                  else if (tag.equals("NR"))
                     inNr = true;
                  else if (tag.equals("BEGIN"))
                     inBegin = true;
                  else if (tag.equals("END"))
                     inEnd = true;
                  else if (tag.equals("THUMB"))
                     inThumb = true;
                  else if (tag.equals("KEYWORD"))
                     inKeyword = true;
                  else if (tag.equals("TYPE"))
                     inType = true;
                  //                   else
                  //                      throw new XmlFormatException
                  //                         ("unknown tag in <PAGE> or <CLIP> object: <" + tag + ">");
               }
               else if (tag.equals("CHAPTER"))
               {
                  if (chapterPending)
                  {
                     Chapter c = new Chapter(chapterTitle, chapterNumber);
                     chapterStack.addElement(c);
                  }
                  chapterPending = true;
                  firstInChapter = true;
                  chapterLevel++;
               }
               else if (tag.equals("PAGE") || tag.equals("CLIP"))
               {
                  if (chapterPending)
                  {
                     Chapter c = new Chapter(chapterTitle, chapterNumber);
                     chapterStack.addElement(c);
                     chapterPending = false;
                  }
                  inPage = true;
                  hasNewStyleTitle = false;
                  if (tag.equals("CLIP"))
                     isPageClip = true;
                  else
                     isPageClip = false;
                  
                  // we'll store keywords here:
                  pageKeywords = new Vector();
                  
                  // reset page type
                  pageType = null;
               }
            }
            else if (inMetainfo)
            {
               if (tag.equals("TITLE"))
                  inTitle = true;
               else if (tag.equals("AUTHOR"))
                  inAuthor = true;
               else if (tag.equals("CREATOR"))
                  inCreator = true;
               else if (tag.equals("RECORDDATE"))
                  inRecordDate = true;
               else if (tag.equals("RECORDLENGTH"))
                  inRecordLength = true;
               else if (tag.equals("KEYWORD"))
                  inKeyword = true;
               //                else
               //                   throw new XmlFormatException
               //                      ("unknown start tag in <METAINFO>: <" + tag + ">");
            }
            else if (inFileInfo)
            {
               if (tag.equals("SHOWCLICKPAGES"))
                  inShowClickPages = true;
            }

            break;
         }

         case XmlParser.TAG_END: {
            String tag = parser.getToken().toUpperCase();

            //             System.out.println("TAG_END: </" + tag + ">");

            if (inStructure)
            {
               if (tag.equals("STRUCTURE"))
                  inStructure = false;
               else if (tag.equals("CHAPTER"))
               {
                  if (chapterPending)
                  {
                     Chapter c = new Chapter(chapterTitle, chapterNumber);
                     chapterStack.addElement(c);
                     chapterPending = false;
                  }

                  chapterStack.removeElementAt(chapterStack.size() - 1);
                  if (chapterStack.size() > 0)
                  {
                     Chapter c = (Chapter) (chapterStack.lastElement());
                     chapterTitle = c.title;
                     chapterNumber = c.number;
                     chapterLevel--;
                     firstInChapter = true;
                  }
                  else
                  {
                     chapterTitle = "";
                     chapterNumber = -1;
                     chapterLevel = 0;
                     firstInChapter = false;
                  }
               }
               else if (inPage)
               {
                  if (tag.equals("PAGE") || tag.equals("CLIP"))
                  {
                     ThumbnailData t = new ThumbnailData();
                     t.number = number;
                     if (title != null)
                        t.title = title;
                     else
                        t.title = "";

                     t.isVideoClip = isPageClip;
                     
                     t.chapterTitles = new String[chapterStack.size()];
                     for (int j=0; j<chapterStack.size(); j++)
                        t.chapterTitles[j] = ((Chapter) (chapterStack.elementAt(j))).title;
                     
                     int matchIndex = getMatchIndex(t.chapterTitles, lastChapters);
                     
                     if (matchIndex == t.chapterTitles.length &&
                         t.chapterTitles.length == lastChapters.length)
                     {
                        t.displayTitle = false;
                     }
                     else
                     {
                        t.displayTitle = true;
                        
                        t.titleOffset = matchIndex;
                        
                        if (t.chapterTitles.length == matchIndex &&
                            lastChapters.length > t.chapterTitles.length &&
                            t.titleOffset > 0)
                           t.titleOffset--;
                     }
                     
                     lastChapters = t.chapterTitles;
                     
                     t.chapterNumber = chapterNumber;
                     t.chapterLevel = chapterLevel;
                     t.thumbnailResource = thumbnailResource;
                     t.beginTimestamp = beginTimestamp;
                     t.endTimestamp = endTimestamp;
                     t.pageType = pageType;

                     // copy keywords...
                     t.keywords = new String[pageKeywords.size()]; // may be 0
                     pageKeywords.copyInto(t.keywords);
                     
                     // reset keyword counter
                     pageKeywords = null;
                     
                     thumbs.addElement(t);

                     inPage = false;
                  }
                  else if (inTitle)
                  {
                     if (tag.equals("TITLE"))
                        inTitle = false;
                     else
                        throw new XmlFormatException("illegal end tag in <TITLE> tag: <" + tag + ">");
                  }
                  else if (inNr)
                  {
                     if (tag.equals("NR"))
                        inNr = false;
                     else
                        throw new XmlFormatException("illegal end tag in <NR> tag: <" + tag + ">");
                  }
                  else if (inBegin)
                  {
                     if (tag.equals("BEGIN"))
                        inBegin = false;
                     else
                        throw new XmlFormatException("illegal end tag in <BEGIN> tag: <" + tag + ">");
                  }
                  else if (inEnd)
                  {
                     if (tag.equals("END"))
                        inEnd = false;
                     else
                        throw new XmlFormatException("illegal end tag in <END> tag: <" + tag + ">");
                  }
                  else if (inThumb)
                  {
                     if (tag.equals("THUMB"))
                        inThumb = false;
                     else
                        throw new XmlFormatException("illegal end tag in <THUMB> tag: <" + tag + ">");
                  }
                  else if (inKeyword)
                  {
                     if (tag.equals("KEYWORD"))
                        inKeyword = false;
                     else
                        throw new XmlFormatException("illegal end tag in <KEYWORD> tag: <" + tag + ">");
                  }
                  else if (inType)
                  {
                     if (tag.equals("TYPE"))
                        inType = false;
                     else
                        throw new XmlFormatException("illegal end tag in <TYPE> tag: <" + tag + ">");
                  }
               }
            }
            else if (inMetainfo)
            {
               if (tag.equals("METAINFO"))
                  inMetainfo = false;
               else if (tag.equals("AUTHOR"))
                  inAuthor = false;
               else if (tag.equals("CREATOR"))
                  inCreator = false;
               else if (tag.equals("RECORDDATE"))
                  inRecordDate = false;
               else if (tag.equals("RECORDLENGTH"))
                  inRecordLength = false;
               else if (tag.equals("KEYWORD"))
                  inKeyword = false;
               else if (tag.equals("TITLE"))
                  inTitle = false;
               //                else
               //                   System.err.println("Unknown end tag in <metainfo>: " + tag);
            }
            else if (inFileInfo)
            {
               if (tag.equals("FILEINFO"))
                  inFileInfo = false;
               else if (tag.equals("SHOWCLICKPAGES"))
                   inShowClickPages = false;
            }
            break;
         }

         case XmlParser.PROPERTY: {
            if (!inStructure)
               break;

            String key = parser.getToken().toUpperCase();
            String value = parser.getPropertyOfToken();

            //             System.out.println("PROPERTY: " + key + "=" + value);

            if (chapterPending)
            {
               if (key.equals("TITLE"))
                  chapterTitle = value;
               else if (key.equals("NR"))
                  chapterNumber = Integer.parseInt(value);
               else
                  System.err.println("FileResources::getMetadata() Chapter: Unknown property '" +
                                     key + "=" + value + "'.");
            }
            else if (inPage)
            {
               if (key.equals("NR"))
                  number = Integer.parseInt(value);
               else if (key.equals("BEGIN"))
                  beginTimestamp = Long.parseLong(value);
               else if (key.equals("END"))
                  endTimestamp = Long.parseLong(value);
               else if (key.equals("THUMB"))
                  thumbnailResource = value;
               //                else
               //                   throw new XmlFormatException("FileResources::getMetadata() Chapter: Unknown property '" +
               //                                                key + "=" + value + "'.");
            }
            
            break;
         }

         case XmlParser.CONTENT: {

            String content = parser.getToken();

            // HOW TO READ UTF8 STRINGS:
            //             byte[] tmp = content.getBytes();
            //             String testContent = new String(tmp, "UTF8");
            //             System.out.println("Test Content (UTF8): '" + testContent + "'.");
            //             content = testContent;

            //             System.out.println("CONTENT: '" + content + "'");

            if (inStructure)
            {
               if (inPage)
               {
                  if (inTitle)
                  {
                     //                      System.out.println("<STRU><PAGE> title = " + content);
                     title = content;
                     hasNewStyleTitle = true;
                  }
                  else if (inNr)
                  {
                     //                      System.out.println("<STRU><PAGE> number = " + content);
                     number = Integer.parseInt(content);
                  }
                  else if (inBegin)
                  {
                     //                      System.out.println("<STRU><PAGE> beginTimestamp = " + content);
                     beginTimestamp = Long.parseLong(content);
                  }
                  else if (inEnd)
                  {
                     //                      System.out.println("<STRU><PAGE> endTimestamp = " + content);
                     endTimestamp = Long.parseLong(content);
                  }
                  else if (inThumb)
                  {
                     //                      System.out.println("<STRU><PAGE> thumbnailResource = " + content);
                     thumbnailResource = content;
                  }
                  else if (inKeyword)
                  {
                     //                      System.out.println("<STRU><PAGE> add keyword = " + content);
                     pageKeywords.addElement(content);
                  }
                  else if (inType)
                  {
                     //                      System.out.println("<STRU><PAGE> type = " + content);
                     pageType = content;
                     
                  }
                  else // we'll take that as a title
                  {
                     if (!hasNewStyleTitle)
                     {
                        //                         System.out.println("<STRU><PAGE> title [oldstyle] = " + content);
                        title = content;
                     }
                  }
               }
            }
            else if (inMetainfo)
            {
               if (inTitle)
                  mTitle = content;
               else if (inAuthor)
                  mAuthor = content;
               else if (inCreator)
                  mCreator = content;
               else if (inRecordDate)
                  mRecordDate = content;
               else if (inRecordLength)
                  mRecordLength = content;
               else if (inKeyword)
                  mKeywords.addElement(content);
            }
            else if (inFileInfo)
            {
                if (inShowClickPages) {
                    if ("false".equalsIgnoreCase(content)) {
                        // remove all click pages
                        for (int i=0; i<thumbs.size(); i++) {
                            ThumbnailData thumbnailData = (ThumbnailData)thumbs.get(i);

                            if ("CLICK".equalsIgnoreCase(thumbnailData.pageType)) {
                                // ignore click page
                                if (i>0)
                                    // adjust end time of previous entry
                                    ((ThumbnailData)thumbs.get(i-1)).endTimestamp = thumbnailData.endTimestamp;
                                // remove page from main list
                                thumbs.remove(i--);
                                // remember in "removed" list (required to adjust event queue)
                                thumbsRemoved.add(thumbnailData);
                            }                           
                        }
                    }
                }                
            }

            break;
         }

         case XmlParser.EOF: 
            //             System.out.println("EOF");
            break;

         default:
            System.err.println("Some error in metadata structure found.");
         }
      }  // end while (ttype != XmlParser.EOF)

      Metadata metadata = new Metadata();
      metadata.thumbnails = new ThumbnailData[thumbs.size()];
      thumbs.copyInto(metadata.thumbnails);
      metadata.removedThumbnails = new ThumbnailData[thumbsRemoved.size()];
      thumbsRemoved.copyInto(metadata.removedThumbnails);
      metadata.keywords = new String[mKeywords.size()];
      mKeywords.copyInto(metadata.keywords);
      metadata.title = mTitle;
      metadata.author = mAuthor;
      metadata.creator = mCreator;
      metadata.recordDate = mRecordDate;
      metadata.recordLength = mRecordLength;

      return metadata;

      //       }      
      //       catch (IOException e)
      //          {
      //             System.out.println("===== DEBUG");
      //             e.printStackTrace();
      //             System.out.println("===== DEBUG");
      //             throw e;
      //          }

      //       System.out.println("================ Thumbnails data");
      //       for (int i=0; i<metadata.thumbnails.length; i++)
      //          System.out.println("#" + i + ": " + metadata.thumbnails[i]);
      
   }

   private static int getMatchIndex(String[] list1, String[] list2)
   {
      int maxMatch = Math.min(list1.length, list2.length);

      int matchCount = 0;

      for (int i=0; i<maxMatch; i++)
      {

         if (list1[i].equals(list2[i]))
            matchCount++;
         else
            break;
      }

      return matchCount;
   }

   /**
    * Returns an array of thumbnail sizes embedded in this
    * presentation. This method will return <tt>null</tt> with an
    * unconverted document or if no thumbnails were created at convert
    * time.
    *
    * @see #getMetadata(InputStream)
    */
   public int[] getThumbnailSizes()
   {
      int[] sizes;

      try
      {
         Vector sizeVector = new Vector();

         InputStream in = createInputStream("_internal_thumbinfo");
         XmlParser parser = new XmlParser(in);

         boolean inSizeTag = false;

         int ttype = XmlParser.TAG_START;
         while (ttype != XmlParser.EOF)
         {
            ttype = parser.getNextTag();
            
            switch (ttype)
            {
            case XmlParser.TAG_START: {
               String tag = parser.getToken();
               if (tag.equalsIgnoreCase("size"))
               {
                  //                   System.out.println("inSizeTag = true");
                  inSizeTag = true;
               }
               break;
            }

            case XmlParser.TAG_END: {
               String tag = parser.getToken();
               if (tag.equalsIgnoreCase("size"))
               {
                  //                   System.out.println("inSizeTag = false");
                  inSizeTag = false;
               }
               break;
            }

            case XmlParser.CONTENT: {
               //                System.out.println("CONTENT == " + parser.getToken() + "(" + inSizeTag + ")");
               if (inSizeTag)
               {
                  int thisSize = Integer.parseInt(parser.getToken());
                  sizeVector.addElement(new Integer(thisSize));
               }
               break;
            }
            
            default:
               // do nothing
               break;
            }
         }

         sizes = new int[sizeVector.size()];
         for (int i=0; i<sizeVector.size(); i++)
            sizes[i] = ((Integer) (sizeVector.elementAt(i))).intValue();

         in.close();
      }
      catch (Exception e)
      {
         sizes = null;
      }

      return sizes;
   }
}

class Chapter
{
   public Chapter(String title, int number)
   {
      this.title  = title;
      this.number = number;
   }
   
   public String title;
   public int number;
}
