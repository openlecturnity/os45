package imc.epresenter.filesdk;

/*
 * File:             SimpleFileResources.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: SimpleFileResources.java,v 1.19 2005-02-01 13:07:06 kuhn Exp $
 */

import java.io.*;

import imc.lecturnity.util.NativeUtils;

/**
 * This class implements the abstract methods in the {@link
 * FileResources FileResources} class in a simple manner: it simply
 * delegates the creation of the {@link java.io.InputStream
 * InputStream} to a {@link java.io.FileInputStream FileInputStream}
 * and converts eventual {@link java.io.FileNotFoundException
 * FileNotFoundException}s to {@link ResourceNotFoundException
 * ResourceNotFoundException} instances.
 *
 * <p>Instances of this class are created automatically by the {@link
 * FileResources FileResources} class in its {@link
 * FileResources#createFileResources createFileResources} method, if
 * the provided file name names an old-style (aof style) ePresentation
 * document.</p>
 *
 * @see FileResources
 * @see FileResources#createFileResources
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
class SimpleFileResources extends FileResources
{
   private String fileName_ = null;
   private String dirName_  = null;
   private PresentationData presentationData_ = null;
   private int documentType_      = NativeUtils.UNIVERSITY_VERSION; // default
   private boolean hasMarkString_ = false;
   private String markString_     = null;

   /**
    * Creates a <tt>SimpleFileResources</tt> instance. See above
    * (class description) for a more detailed description.
    *
    * @see FileResources#createFileResources
    */
   public SimpleFileResources(String fileName)
      throws ResourceNotFoundException, IOException
   {
      super();

      fileName_ = fileName;
      try
      {
         File file = new File(fileName_);
         // extract path to the file
         String completePath = file.getCanonicalPath();
         int lastSlash = completePath.lastIndexOf(File.separatorChar);
         dirName_ = completePath.substring(0, lastSlash+1);
      }
      catch (IOException e)
      {
         dirName_ = "";
      }
      
      if (ConsistencyChecker.checkForConsistency(fileName_))
      {
         try
         {
            presentationData_ = ConsistencyChecker.getLastPresentationData();
            m_strCodepage = presentationData_.strCodepage;
            if (audioHasLadFormat())
               retrieveDocumentMark();
            else 
            {
              
               // small hack for decompressed lpds
               File possibleFile = new File(fileName_.substring(0, fileName_.length()-4)+".aqs");
               if (possibleFile.exists())
                  markString_ = "dummy";
            }
         }
         catch (IOException e)
         {
            markString_ = null;
            documentType_ = -1;
            throw e;
         }
      }
      else
         throw new ResourceNotFoundException
            (ConsistencyChecker.getLastConsistencyError());
   }

   /**
    * @see FileResources#createConfigFileInputStream
    */    
   public InputStream createConfigFileInputStream() 
      throws ResourceNotFoundException
   {
      return createInputStream(fileName_);
   }

   /**
    * @see FileResources#createInputStream
    */
   public InputStream createInputStream(String resourceName)
      throws ResourceNotFoundException
   {
      try
      {
         FileInputStream fileIn = new FileInputStream(resolveFile(resourceName));
         return fileIn;
      }
      catch (FileNotFoundException e)
      {
         throw new ResourceNotFoundException(e.getMessage());
      }
   }

   /**
    * @see FileResources#createBufferedInputStream
    */
   public InputStream createBufferedInputStream
      (String resourceName, int bufferSize)
      throws ResourceNotFoundException
   {
      try
      {
         InputStream fileIn = 
            new BufferedInputStream(new FileInputStream(resolveFile(resourceName)),
                                    bufferSize);
         return fileIn;
      }
      catch (FileNotFoundException e)
      {
         throw new ResourceNotFoundException(e.getMessage());
      }
   }

   public long getFileSize(String resourceName)
   {
      File file = resolveFile(resourceName);
      if (file.exists())
         return file.length();
      else
         return -1L;
   }

   private File resolveFile(String resourceName)
   {
      String strippedResourceName = (new File(resourceName)).getName();

      File file = new File(dirName_ + strippedResourceName);
      if (file.exists())
         return file;

      // note: it may be here that the file does not exist! in that
      // case, it is up to the calling method to throw an appropriate
      // exception or to handle this in a suitable way.
      file = new File(resourceName);
      return file;
   }

   protected String getCurrentDirectory()
   {
      return "" + dirName_;
   }

   /**
    * Not supported by this class.
    *
    * @return <tt>null</tt>
    */
   public java.util.Hashtable getArchivedFiles()
   {
      return null;
   }

   /**
    * Returns a fitting URL for the Java Media Framework. This will
    * always be a file://<i>resource</i> style URL. If the file is
    * found in the current directory, this path is used. Otherwise,
    * the file is looked for as a global file.  If the file cannot be
    * located, this method will return <tt>null</tt>.  
    */
   public String getResourceURL(String resource)
   {
//       File file = resolveFile(resource);
//       if (!file.exists())
//          return null;
//       return "file:/" + (file.getAbsolutePath());
      String lrdFileName = dirName_ + (new File(fileName_)).getName();

      return "fastfile://" + lrdFileName + "/" + resource;
   }

   /**
    * @see FileResources#getDocumentType()
    */
   public int getDocumentType()
   {
      return documentType_;
   }

   /**
    * @see FileResources#getMarkString()
    */
   public String getMarkString()
   {
      return markString_;
   }

   private boolean audioHasLadFormat()
      throws IOException
   {
      InputStream in = new BufferedInputStream
         (createInputStream(presentationData_.audioFileName));
      DataInputStream dataIn = new DataInputStream(in);

      byte[] ladFcc = new byte[4];
      dataIn.readFully(ladFcc);

      dataIn.close();

      return (ladFcc[0] == 'L' &&
              ladFcc[1] == 'A' &&
              ladFcc[2] == 'D' &&
              ladFcc[3] == ' ');
   }

   private void retrieveDocumentMark()
      throws IOException
   {
      // Presentation document does definitely have audio of the 
      // Lecturnity Audio Document format
      InputStream in = new BufferedInputStream(
         createInputStream(presentationData_.audioFileName));
      DataInputStream dataIn = new DataInputStream(in);
      
      skipFully(dataIn, 4);
      byte[] key = new byte[892];
      dataIn.readFully(key);
      
      dataIn = new DataInputStream(new XorInputStream(in, key));
      byte[] mark = new byte[128];
      dataIn.readFully(mark);
     
      dataIn.close();
      
      int nullBytePos = 128;
      for (int i=0; i<mark.length; i++)
      {
         if (mark[i] == 0)
         {
            nullBytePos = i;
            break;
         }
      }

      markString_ = new String(mark, 0, nullBytePos);
     
      int semicPos = markString_.indexOf(';');
      if (semicPos > 0)
      {
         String type = markString_.substring(0, semicPos);
         
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
         throw new IOException("lad header corrupt, marker not found");
   }
}
