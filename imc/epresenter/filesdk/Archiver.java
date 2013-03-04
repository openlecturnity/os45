package imc.epresenter.filesdk;

/*
 * File:             Archiver.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: Archiver.java,v 1.21 2010-02-19 12:29:04 zoen Exp $
 */

import java.util.Vector;

import java.io.*;

import java.util.zip.Deflater;
import java.util.zip.DeflaterOutputStream;

import imc.epresenter.converter.ConversionDisplay;
import imc.epresenter.converter.DefaultConversionDisplay;

import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.filesdk.util.ByteCounter;
import imc.epresenter.filesdk.util.CountInputStream;

import javax.swing.JOptionPane;

/**
 * This class is used for creating ePresentation Archives
 * (<tt>.lpd</tt> files). A new <tt>Archiver</tt> instance is created
 * by invoking the constructor with the new archive file name. Then,
 * the files that should be added to the presentation archive are
 * added by using the {@link #addFile(String, boolean) addFile} and
 * {@link #addConfigFile(String) addConfigFile} methods. Note that is
 * recommended to archive the audio file <i>uncompressed</i> and that
 * it may lead to undefined behaviour if more than one configuration
 * file is added to the archive.
 *
 * <p>Having added all the desired files, the new ePresentation
 * archive can be written to disk by using the {@link #writeArchive
 * writeArchive} method.</p>
 *
 * <p><b>Note:</b> The <tt>Archiver</tt> instances use temporary files
 * with the file name <tt>&lt;archive name&gt;.tmp</tt>, so be careful
 * with important files carrying a <tt>.tmp</tt> extension
 * (contradicts itself, though!).</p>
 *
 * @see FileResources
 * @see ArchivedFileResources
 * @see SimpleFileResources
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class Archiver 
{
   protected final static int KEY_LENGTH  = 2048;
   protected final static int BUFFER_SIZE = 16384;
   protected final static int READ_BUFFER  = 65536;
   protected final static int WRITE_BUFFER = 65536;

   public final static String ARCHIVE_EXTENSION = ".lpd";

   private String  archiveName_ = null;
   private String  currentDir_  = null;
   private byte[]  inputKey_    = null;
   private Vector  archivedFilesVector_ = null;
   private boolean isArchiveWritten_ = false;

   private boolean cancelRequest_ = false;

   private ConversionDisplay display_ = null;

   private Localizer localizer_ = null;

   private boolean createHelpDocument_ = false;
   private boolean startInFullScreen_ = false;
   private boolean showClipsOnSlides_ = false;

   /**
    * Creates a new <tt>Archiver</tt> for an archive with the given
    * archive file name.
    *
    * @param archiveName the file name of the archive that will be created
    * @param currentDir the directory in which the presentation to
    * convert may be found.
    * @param display the conversion display to output comments on, or
    * <tt>null</tt>: the the comments are output to <tt>stdout</tt>.
    */
   public Archiver(String archiveName, 
                   String currentDir, 
                   ConversionDisplay display)
   {
      this(archiveName, currentDir, display, null);
   }

   /**
    * Creates a new <tt>Archiver</tt> for an archive with the given
    * archive file name. By supplying a byte array key, the <tt>Archiver</tt>
    * assumes the input files to be XOR encrypted and decrypts them before
    * writing the archive to disk.
    *
    * @param archiveName the file name of the archive that will be created
    * @param currentDir the directory in which the presentation to
    * convert may be found.
    * @param display the conversion display to output comments on, or
    * <tt>null</tt>: the the comments are output to <tt>stdout</tt>.
    * @param inputKey the key with which the input files are xor encrypted,
    *   can be <tt>null</tt> if the files or non-encrypted.
    */
   public Archiver(String archiveName, 
                   String currentDir, 
                   ConversionDisplay display,
                   byte[] inputKey)
   {
      try
      {
         localizer_ = new Localizer("/imc/epresenter/filesdk/Archiver_",
                                    "en");
      }
      catch (IOException e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(null,
                                       "Language properties not found!",
                                       "imc.epresenter.filesdk.Archiver",
                                       JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }

      archiveName_ = archiveName;
      currentDir_  = currentDir;

      if (inputKey == null)
      {
         inputKey_ = new byte[512];
         for (int i=0; i<inputKey_.length; ++i)
            inputKey_[i] = 0;
      }
      else
      {
         inputKey_ = inputKey;
      }

      archivedFilesVector_ = new Vector();
      isArchiveWritten_ = false;
      
      cancelRequest_ = false;

      if (display != null)
         display_ = display;
      else
         display_ = new DefaultConversionDisplay();
   }

   /**
    * Use this method to tell an existing <tt>Archiver</tt> instance
    * (before writing the archive!) to create an archive for online
    * help documents. It will write the appropriate header, so that
    * the {@link FileResources#isHelpDocument(java.lang.String) isHelpDocument}
    * method of <tt>FileResources</tt> returns <i>true</i>.
    *
    * @see FileResources
    * @see FileResources#isHelpDocument(java.lang.String)
    */
   public void setCreateHelpDocument(boolean b)
   {
      createHelpDocument_ = b;
   }

   /**
    * @see imc.epresenter.converter.PresentationConverter#setStartInFullScreen(boolean)
    */
   public void setStartInFullScreen(boolean b)
   {
      startInFullScreen_ = b;
   }

   /**
    * @see imc.epresenter.converter.PresentationConverter#setShowClipsOnSlides(boolean)
    */
   public void setShowClipsOnSlides(boolean b)
   {
      showClipsOnSlides_ = b;
   }

   /**
    * Adds the file with the given file name as a configuration file
    * to this ePresentation archive. The path specified in the
    * constructor is appended to the file name.
    *
    * @param fileName the file name of the configuration file
    * @throws java.io.IOException if the file cannot be found
    */
   public void addConfigFile(String fileName)
      throws IOException
   {
      File configFile = new File(currentDir_ + fileName);
      FileStruct configFileStruct = new FileStruct(fileName, -1, -1, 
                                                   (int) configFile.length(),
                                                   true, true, null);
      archivedFilesVector_.addElement(configFileStruct);
   }

   /**
    * Adds the file specified in <tt>configFile</tt> as the file given
    * in <tt>addAs</tt> as a configuration file.
    *
    * @param configFile the actual file to add to the archive, <b>with
    * full path</b>!
    * @param addAs the name of <tt>configFile</tt> in the archive
    */
   public void addConfigFileAs(String configFileName, String addAs)
      throws IOException
   {
      File configFile = new File(configFileName);
      FileStruct configFileStruct = new FileStruct(addAs, -1, -1,
                                                   (int)
                                                   configFile.length(),
                                                   true, true, configFileName);
      archivedFilesVector_.addElement(configFileStruct);
   }

   /**
    * Adds the file with the given file name to the ePresentation
    * archive. You may choose if the file should be compressed or
    * not. The path specified in the constructor is appended to the
    * file name.
    *
    * @param fileName the file name of the file to be added to the archive
    * @param compress <i>true</i> if the file should be compressed, otherwise
    * <i>false</i>
    * @throws java.io.IOException if the file cannot be found
    */
   public void addFile(String fileName, boolean compress)
      throws IOException
   {
      File file = new File(currentDir_ + fileName);
      FileStruct fileStruct = new FileStruct(fileName, -1, -1, (int) file.length(), 
                                             compress, false, null);
      archivedFilesVector_.addElement(fileStruct);
   }

   /**
    * Adds the file with the given file name to the ePresentation
    * archive under another name. You may choose if the file should be
    * compressed or not.
    *
    * @param fileName the file name of the file to be added to the archive
    * @param addAs the name of the added file in the archive
    * @param compress <i>true</i> if the file should be compressed, otherwise
    * <i>false</i>
    * @throws java.io.IOException if the file cannot be found */
   public void addFileAs(String fileName, String addAs, 
                         boolean compress)
      throws IOException
   {
      File file = new File(fileName);
      FileStruct fileStruct = new FileStruct(addAs, -1, -1, (int)
                                             file.length(), compress,
                                             false, fileName);
      archivedFilesVector_.addElement(fileStruct);
   }

   /**
    * Invoke this method after having added all files in order to
    * write the archive to disk.
    *
    * @see #addFile(String, boolean)
    * @see #addConfigFile(String)
    * @throws java.io.IOException if any of the files added to the archives
    * cannot be found, the disk is full, ...
    */
   public void writeArchive() throws IOException
   {
      byte[] key = createRandomKey();

      {
         // Next line: "writeFiles" is defined in LecturnityWriterWizardPanel::defineProgressBarSteps
         display_.displayCurrentProgressStep("writeFiles", localizer_.getLocalized("Adding"));
         display_.displayCurrentProgress(0.0f);
         
         // To be able to display a continuous progress only the number 
         // of files used for "writeFileToStream()" are remembered. 
         // TODO: Remember progress for each file in "writeFileToStream()" again.
         display_.displayCurrentFile("File",
                 archivedFilesVector_.size(),
                 localizer_.getLocalized("Adding"));

         XorOutputStream tmpOut =
            new XorOutputStream
            (new BufferedOutputStream(new FileOutputStream(archiveName_ +".tmp"), 
                                      WRITE_BUFFER),
             key);

         int writtenBytes = 0;
         for (int i=0; i<archivedFilesVector_.size(); i++)
         {
            FileStruct fileStruct = 
               (FileStruct) archivedFilesVector_.elementAt(i);

            // Outcommented to be able to display a continuous progress
            //display_.displayCurrentFile(fileStruct.name,
            //                            fileStruct.realSize,
            //                            localizer_.getLocalized("Adding"));

            int offset = writtenBytes;
            int size;
	    
            if (fileStruct.realName == null)
               size = writeFileToStream(tmpOut, currentDir_ + fileStruct.name,
                                        fileStruct.isCompressed);
            else
               size = writeFileToStream(tmpOut, fileStruct.realName,
                                        fileStruct.isCompressed);

            fileStruct.offset = offset;
            fileStruct.size   = size;

            writtenBytes += size;

            // To be able to display a continuous progress
            display_.displayCurrentFileProgress(i+1);

            if (cancelRequest_)
               break;
         }
	    
         tmpOut.close();

         if (cancelRequest_)
         {
            // clean up
            deleteFile(archiveName_ + ".tmp");
            return;
         }
         // 	 display_.logMessage(localizer_.getLocalized("Written_bytes") + writtenBytes);
      }

      {
         if (cancelRequest_)
            return;

         OutputStream fileOut = new BufferedOutputStream
            (new FileOutputStream(archiveName_ + ARCHIVE_EXTENSION), WRITE_BUFFER);
         
         byte[] epfHeader = new byte[] {'E', 'P', 'F', 0};
         /*
           if (createHelpDocument_)
           epfHeader = new byte[] {'E', 'P', 'F', 'H'};
           else
           epfHeader = new byte[] {'E', 'P', 'F', ' '};
         */

         if (!createHelpDocument_ &&
             !startInFullScreen_ &&
             !showClipsOnSlides_)
            epfHeader[3] = (byte) FileResources.BITMASK_NORMAL_DOCUMENT;
         else
         {
            if (createHelpDocument_)
               epfHeader[3] = (byte) (FileResources.BITMASK_HELP_DOCUMENT);
            if (startInFullScreen_)
               epfHeader[3] = (byte) (epfHeader[3] | FileResources.BITMASK_START_FULLSCREEN);
            if (showClipsOnSlides_)
               epfHeader[3] = (byte) (epfHeader[3] | FileResources.BITMASK_CLIPS_ON_SLIDES);
         }
         
         fileOut.write(epfHeader);
	 
         display_.logMessage(localizer_.getLocalized("Write_header"));
         writeKey(fileOut, key);
	 
         XorOutputStream out = new XorOutputStream(fileOut, key);
         writeHeader(out);
         out.close();
	 
         display_.logMessage(localizer_.getLocalized("Appending_data"));
         fileOut = new BufferedOutputStream
            (new FileOutputStream(archiveName_ + ARCHIVE_EXTENSION, true), WRITE_BUFFER);
         appendFileToStream(fileOut, archiveName_ + ".tmp" );
         fileOut.close();
 
         deleteFile(archiveName_ + ".tmp");

         if (cancelRequest_)
         {
            // delete unfinished archive
            deleteFile(archiveName_ + ARCHIVE_EXTENSION);
         }
         else
         {
            display_.logMessage(localizer_.getLocalized("Archivation_done"));
         }
      }
   }

   public void cancelOperation()
   {
      cancelRequest_ = true;
   }
    
   /**
    * Writes the given file to a given stream, compressed, if
    * desired. The method returns the number of bytes written through
    * the given <tt>XorOutputStream</tt>.
    *
    * @param out the <tt>XorOutputStream</tt> to write to
    * @param fileName the file name of the file to write to
    * <tt>out</tt>
    * @param compressed <i>true</i> if the file should be filtered
    * through a {@link java.util.zip.DeflaterOutputStream
    * DeflaterOutputStream}
    * @return the number of bytes written to <tt>out</tt>
    */
   private int writeFileToStream(XorOutputStream out, String fileName,
                                 boolean compressed) throws IOException 
   {
      //       if (compressed)
      // 	 display_.logMessage("Deflating " + fileName + ".");
      //       else
      // 	 display_.logMessage("Writing uncompressed: " + fileName + " ...");

      int totalWrittenPre = out.getTotalWrittenBytes();

      InputStream in = new BufferedInputStream
         (new XorInputStream(new FileInputStream(fileName), inputKey_), READ_BUFFER);

      OutputStream fileOut = null;
      Deflater deflater = null;
      if (compressed)
      {
         deflater = new Deflater();
         fileOut = new DeflaterOutputStream(out, deflater);
      }
      else
         fileOut = out;
      
      byte[] buf = new byte[BUFFER_SIZE];
      
      int readBytes = 1024;
      int totalRead = 0;
      while (readBytes > 0 && !cancelRequest_) 
      {
         readBytes = in.read(buf);
         if (readBytes > 0)
            fileOut.write(buf, 0, readBytes);
         totalRead += readBytes;
         // Outcommented to be able to display a continuous progress
         //display_.displayCurrentFileProgress(totalRead);
      }

      if (compressed)
         ((DeflaterOutputStream) fileOut).finish();
      else
         fileOut.flush();

      in.close();

      return out.getTotalWrittenBytes() - totalWrittenPre;
   }

   /**
    * Writes the key given as a byte array to the given stream.
    * 
    * @param out the output stream to write to.
    * @param key the key to write to <tt>out</tt>
    */
   private void writeKey(OutputStream out, byte[] key)
      throws IOException
   {
      out.write(key);
      out.flush();
   }

   /**
    * Writes the archive header to the given output stream. This
    * consists of an array of {@link FileStruct FileStruct} instances
    * written through an {@link java.io.ObjectOutputStream
    * ObjectOutputStream}.
    *
    * @param out the output stream to write the header to
    * @throws java.io.IOException if something goes wrong while writing the header
    */
   private void writeHeader(OutputStream out)
      throws IOException
   {
      FileStruct[] files = new FileStruct[archivedFilesVector_.size()];
      archivedFilesVector_.copyInto(files);
      
      ObjectOutputStream objectOut = new ObjectOutputStream(out);

      objectOut.writeObject(files);
      objectOut.flush();
      objectOut.close(); // needed? does it work without?
   }

   /**
    * This method writes the content of the file with the given file
    * name to the given output stream without changing it.
    *
    * @param out the output stream to write to
    * @param fileName the file name of the file to write to
    * <tt>out</tt>
    * @throws java.io.IOException e.g. if the input file was not found or if
    * something else goes wrong.
    */
   private void appendFileToStream(OutputStream out, String fileName)
      throws IOException
   {
      File file = new File(fileName);
      int fileSize = (int) (file.length());
      
      // Next line: "writeArchive" is defined in LecturnityWriterWizardPanel::defineProgressBarSteps
      display_.displayCurrentProgressStep("writeArchive", localizer_.getLocalized("Appending_data"));
      display_.displayCurrentProgress(0.0f);

      display_.displayCurrentFile(localizer_.getLocalized("TEMP_FILE"), fileSize,
                                  localizer_.getLocalized("COPY_TEMP"));

      InputStream in = new CountInputStream
         (new BufferedInputStream(new FileInputStream(fileName), READ_BUFFER),
          new AppendByteCounter());
      
      byte[] buf = new byte[BUFFER_SIZE];

      int readBytes = 1024;
      while (readBytes > 0 && !cancelRequest_)
      {
         readBytes = in.read(buf);
         if (readBytes > 0)
            out.write(buf, 0, readBytes);
      }
      in.close();
      out.flush();

      display_.displayCurrentFileProgress(fileSize);
   }

   private class AppendByteCounter implements ByteCounter
   {
      public void displayCountedBytes(int count)
      {
         display_.displayCurrentFileProgress(count);
      }
   }

   /**
    * Deletes the file with the specified file name.
    * @param fileName the file name (uncluding the path) of the file to delete
    * @throws java.io.IOException if the deletion was not successful
    */
   private void deleteFile(String fileName) throws IOException
   {
      File file = new File(fileName);
      boolean success = file.delete();
      if (!success)
         throw new IOException(localizer_.getLocalized("ERR_Couldnt_delete_file") +
                               "\"" + fileName + "\"!");
   }
    
   /**
    * Returns a random byte array of {@link #KEY_LENGTH KEY_LENGTH} length.
    *
    * @return a random byte array of {@link #KEY_LENGTH KEY_LENGTH} length.
    */
   private byte[] createRandomKey()
   {
      byte[] key = new byte[KEY_LENGTH];
      for (int i=0; i<KEY_LENGTH; i++)
         key[i] = (byte) (Math.random()*256d);
      
      return key;
   }
}
