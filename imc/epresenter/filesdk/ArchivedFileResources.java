package imc.epresenter.filesdk;

/*
 * File:             ArchivedFileResources.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: ArchivedFileResources.java,v 1.25 2005-05-31 14:18:27 kuhn Exp $
 */

import java.io.*;

import java.util.Hashtable;

import java.util.zip.Inflater;
import java.util.zip.InflaterInputStream;


import imc.epresenter.player.util.XmlTag;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;

/**
 * This class is an implementation of the abstract class {@link
 * FileResources FileResources} working on an ePresentation Archive
 * with the ending <tt>.epf</tt>. It will create input streams that
 * work as normal {@link java.io.FileInputStream FileInputStream}s.
 *
 * @see FileResources
 * @see SimpleFileResources
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
class ArchivedFileResources extends FileResources
{
   private String    fileName_           = null;
   private String    configResourceName_ = null;
   private String    currentDir_         = null;
   private Hashtable fileTable_          = null;

   private long headerLength_ = -1;
   private byte[] archiveKey_ = null;

   private int documentType_ = NativeUtils.UNIVERSITY_VERSION; // default
   private String markString_ = null;

   /**
    * Creates a new <tt>ArchivedFileResources</tt> instance. Instances
    * of this class are normally created by the {@link FileResources
    * FileResources} class, and not manually.
    *
    * @see FileResources#createFileResources
    */
   public ArchivedFileResources(String fileName)
      throws IOException
   {
      super();

      fileName_ = fileName;

      // try to resolve current path canonically
      try
      {
         File file = new File(fileName_);
         String dir = file.getCanonicalPath();
         int slashPos = dir.lastIndexOf(File.separatorChar);
         currentDir_ = dir.substring(0, slashPos + 1);
      }
      catch (IOException e)
      {
         currentDir_ = "." + File.separatorChar;
      }

      createFileTable();

      
      InputStream configStream = createConfigFileInputStream();
      XmlTag configTag = (XmlTag.parse(configStream))[0];
      configStream.close();
   
      // double code (Document)
      //
      // since 1.7.0.p3 there might be a codepage specification
      // in the lrd file this is relevant for all text files (obj, lmd)
      //
      // Bugfix #2013:
      // and it is relevant for file names in the case of an LPD
      Object[] arrCodepages = configTag.getValues("CODEPAGE");
      if (arrCodepages != null && arrCodepages.length > 0)
      {
         m_strCodepage = (String)arrCodepages[0];
         if (FileUtils.isInteger(m_strCodepage))
            m_strCodepage = "Cp"+m_strCodepage;
      }
      
       
      try
      {
         retrieveDocumentType();
      }
      catch (IOException e)
      {
         markString_ = null;
         documentType_ = -1;
         throw e;
      }
   }

   private void createFileTable() throws IOException
   {
      fileTable_ = new Hashtable();
      
      FileInputStream fileIn = new FileInputStream(fileName_);

      byte[] epfHeader = new byte[4];
      int epfReadBytes = fileIn.read(epfHeader);
      if (epfHeader[0] != 'E' ||
          epfHeader[1] != 'P' ||
          epfHeader[2] != 'F')
      {
         fileIn.close();
         throw new FileFormatException("File '" + fileName_ + "' has " +
                                       "the wrong format!");
      }

      archiveKey_ = new byte[Archiver.KEY_LENGTH];
      
      int readBytes = fileIn.read(archiveKey_);
      if (readBytes != archiveKey_.length)
         throw new IOException("Key size mismatch: " + readBytes +
                               "!=" + archiveKey_.length);

      XorInputStream xorIn = new XorInputStream(fileIn, archiveKey_);
      ObjectInputStream objectIn = new ObjectInputStream(xorIn);

      FileStruct[] files = null;
      try
      {
         files = (FileStruct[]) objectIn.readObject();
      }
      catch (ClassNotFoundException e)
      {
         throw new FileFormatException("Error reading the header: " +
                                       e.getMessage());
      }

      // length of header + archiveKey_.length + "EPF "
      headerLength_ = xorIn.getTotalRead() + Archiver.KEY_LENGTH + 4;
      
      for (int i=0; i<files.length; i++)
      {
         fileTable_.put(files[i].name, files[i]);

         if (files[i].isConfigFile)
         {
            configResourceName_ = files[i].name;
         }
      }

      fileIn.close();
   }

   /**
    * @return an {@link java.io.InputStream InputStream} for the
    * configuration file of the ePresentation.
    * @see #createInputStream
    * @see FileResources#createConfigFileInputStream
    */   
   public InputStream createConfigFileInputStream()
      throws ResourceNotFoundException, IOException 
   {
      return createInputStream(configResourceName_);
   }

   /**
    * Creates an {@link java.io.InputStream InputStream} object which
    * will contain the wanted resource in <tt>resourceName</tt>. If
    * this resource is not found inside the archive, this method will
    * look in the current directory (the directory of the archive) for
    * the wanted resource. If this also fails, a {@link
    * ResourceNotFoundException ResourceNotFoundException} will be
    * thrown. Other {@link java.io.IOException IOException} instances
    * may also be thrown!
    *
    * @return an {@link java.io.InputStream InputStream} for the given
    *    resource name.
    * @see FileResources#createInputStream
    */
   public InputStream createInputStream(String resourceName)
      throws ResourceNotFoundException, IOException
   {
      String strippedName = (new File(resourceName)).getName();
      
      FileStruct fileStruct = (FileStruct) fileTable_.get(strippedName);
      if (fileStruct == null)
      {
         // try with full path in resource (fonts!)
         fileStruct = (FileStruct) fileTable_.get(resourceName);
         
         if (fileStruct == null && resourceName.indexOf("\\\\") > -1)
         {
            // Bugfix Bug 1860
            // remove double-backslashes:
            // ttf= in ObjectQueue has a path with "\\" as delimiter
            // the FileStruct hashtable entry has only single "\"
            // (Bug 1602 and Bug 1706 are indirect reasons for this)
            StringBuffer unslashed = new StringBuffer();
            for (int i=0; i<resourceName.length(); ++i)
            {
               if (resourceName.charAt(i) != '\\')
                  unslashed.append(resourceName.charAt(i));
               else if (i+1 < resourceName.length())
               {
                  unslashed.append(resourceName.charAt(i+1));
                  ++i;
               }
            }
            
            fileStruct = (FileStruct) fileTable_.get(unslashed.toString());
         }
         
         // Bugfix #2013:
         // file names are correct in the LPD (FileStruct[]) and maybe wrongly
         // read from the LRD inside the LPD
         if (fileStruct == null && m_strCodepage != null)
         {
            String correctName = FileUtils.makeUnbuggedFilename(strippedName, m_strCodepage);
            
            fileStruct = (FileStruct) fileTable_.get(correctName);
         }
      }

      if (fileStruct == null)
      {
         // check in the current directory for the resource
         File file = new File(currentDir_ + strippedName);
         if (file.exists())
            return new FileInputStream(file);
         
         // check for global file name?
         file = new File(strippedName);
         if (file.exists())
            return new FileInputStream(file);
         
         throw new ResourceNotFoundException("Resource not found: '" + 
                                             strippedName + "'.");
      }
      
      try
      {
         FileInputStream fileIn = new FileInputStream(fileName_);
         skipFully(fileIn, headerLength_);
         //          fileIn.skip(headerLength_);
         
         XorInputStream xorIn = new XorInputStream(fileIn, archiveKey_);
         skipFully(xorIn, fileStruct.offset);
         //          xorIn.skip(fileStruct.offset);
         xorIn.setMaxBytesRead(fileStruct.size);
         
         if (fileStruct.isCompressed)
            return new InflaterInputStream(xorIn);
         else
            return xorIn;
      }
      catch (FileNotFoundException e)
      {
         throw new ResourceNotFoundException("Resource archive not found! " +
                                             e.getMessage());
      }
   }

   public InputStream createBufferedInputStream
   (String resourceName, int bufferSize)
      throws ResourceNotFoundException, IOException
   {
      String strippedName = (new File(resourceName)).getName();

      // Double code!! See createInputStream
      FileStruct fileStruct = (FileStruct) fileTable_.get(strippedName);
      if (fileStruct == null)
      {
         fileStruct = (FileStruct) fileTable_.get(resourceName);
      }

      if (fileStruct == null)
      {
         // check in the current directory for the resource
         File file = new File(currentDir_ + strippedName);
         if (file.exists())
            return new BufferedInputStream(new FileInputStream(file), bufferSize);
         
         // check for global file name?
         file = new File(strippedName);
         if (file.exists())
            return new BufferedInputStream(new FileInputStream(file), bufferSize);
         
         throw new ResourceNotFoundException("Resource not found: '" + 
                                             strippedName + "'.");
      }
      
      try
      {
         //System.out.println("tüdelidü");
         InputStream fileIn =
            new BufferedInputStream(new FileInputStream(fileName_), bufferSize);
         skipFully(fileIn, headerLength_);
         //          fileIn.skip(headerLength_);

         XorInputStream xorIn = new XorInputStream(fileIn, archiveKey_);
         skipFully(xorIn, fileStruct.offset);
         //          xorIn.skip(fileStruct.offset);
         xorIn.setMaxBytesRead(fileStruct.size);
         
         if (fileStruct.isCompressed)
            return new InflaterInputStream(xorIn);
         else
            return xorIn;
      }
      catch (FileNotFoundException e)
      {
         throw new ResourceNotFoundException("Resource archive not found! " +
                                             e.getMessage());
      }
   }

   public long getFileSize(String resourceName)
   {
      String strippedName = (new File(resourceName)).getName();

      FileStruct fileStruct = (FileStruct) fileTable_.get(strippedName);
      if (fileStruct != null)
         return fileStruct.realSize;

      // check for file in current directory
      File file = new File(currentDir_ + strippedName);
      if (file.exists())
         return file.length();
      
      // check for global file name?
      file = new File(strippedName);
      if (file.exists())
         return file.length();

      return -1;
   }

   protected String getCurrentDirectory()
   {
      return "" + currentDir_;
   }

   public Hashtable getArchivedFiles()
   {
      return fileTable_;
   }

   /**
    * Returns a fitting URL for the Java Media Framework to the
    * requested resource. If the resource is found in the current
    * archive, epf://<i>archive</i>/<i>resource</i> is returned. If
    * the resource cannot be found in the archive, the
    * <tt>ArchivedFileResources</tt> instance checks for the resource
    * as a global file name, and then checks for the file in the
    * current directory (i.e., the directory of the archive). If the
    * resource cannot be found, <tt>null</tt> is returned.
    */
   public String getResourceURL(String resource)
   {
      String strippedName = (new File(resource)).getName();

      // ok, first we check for the resource in the archive
      FileStruct fileStruct = (FileStruct) fileTable_.get(strippedName);
      if (fileStruct != null)
         return "epf://" + fileName_ + "/" + resource;

      File file = new File(currentDir_ + strippedName);
      if (file.exists())
         return "file://" + currentDir_ + strippedName;
      
      file = new File(resource);
      if (file.exists())
         return "file://" + resource;

      return null;
   }

   public int getDocumentType()
   {
      return documentType_;
   }

   public String getMarkString()
   {
      return markString_;
   }

   private void retrieveDocumentType()
      throws IOException
   {
      InputStream markIn = null;
      try
      {
         markIn = new BufferedInputStream(
            createInputStream("_internal_versiontypeinfo"));
      }
      catch (ResourceNotFoundException e)
      {
         markIn = null;
      }
	 
      if (markIn == null)
      {
         // old document, prior to or equal version 1.1.1
         documentType_ = NativeUtils.UNIVERSITY_VERSION; // default
         markString_   = null;
      }
      else
      {
         byte[] mark = new byte[128];
         
         int readBytes = 0;
         int totalRead = 0;
         
         while (readBytes != -1)
         {
            readBytes = markIn.read(mark, totalRead, 128 - totalRead);
            if (readBytes != -1)
               totalRead += readBytes;
         }

         markIn.close();

         markString_ = new String(mark, 0, totalRead);

         int semicPos = markString_.indexOf(';');
         if (semicPos > 0)
         {
            String type = markString_.substring(0, semicPos);
         
            //             System.out.println("document type is '" + type + "'.");
         
            if (type.equals("u"))
               documentType_ = NativeUtils.UNIVERSITY_VERSION;
            else if (type.equals("ue"))
               documentType_ = NativeUtils.UNIVERSITY_EVALUATION_VERSION;
            else if (type.equals("f"))
               documentType_ = NativeUtils.FULL_VERSION;
            else if (type.equals("e"))
               documentType_ = NativeUtils.EVALUATION_VERSION;
            else
               throw new IOException("document type corrupt! (type '" + type + "')");
         }
         else
            throw new IOException("document mark header corrupt!");
      }
   }
}
