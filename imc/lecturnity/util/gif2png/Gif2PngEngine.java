package imc.lecturnity.util.gif2png;

import com.sun.media.jai.codec.ImageEncoder;
import com.sun.media.jai.codec.ImageCodec;
import com.sun.media.jai.codec.PNGEncodeParam;
// import com.sun.media.jai.codec.PNGEncodeParam;
import com.sun.media.jai.codec.SeekableStream;

// import javax.media.jai.JAI;
// import javax.media.jai.RenderedOp;
// import javax.media.jai.RenderableOp;
import javax.media.jai.*;

import java.awt.*;
import java.awt.image.*;

import javax.media.jai.operator.CropDescriptor;

import java.awt.Button;
import java.awt.MediaTracker;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.image.renderable.ParameterBlock;

import javax.swing.JOptionPane;

import java.io.*;

import java.util.HashSet;
import java.util.Hashtable;
import java.util.List;
import java.util.Arrays;

import imc.epresenter.filesdk.*;

import imc.epresenter.filesdk.util.Localizer;

import imc.epresenter.converter.ConversionDisplay;

import imc.lecturnity.util.LogProgressListener;
import imc.lecturnity.util.CancelException;

class Gif2PngEngine
{
   private static String ANALYZING = "[!]";
   private static String UPDATING = "[!]";
   private static String DONE = "[!]";
   private static String RECONSTRUCTING = "[!]";
   private static String CANCELLED = "[!]";
   private static String OPENING = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/util/gif2png/Gif2PngEngine_", "en");

         ANALYZING = l.getLocalized("ANALYZING");
         UPDATING = l.getLocalized("UPDATING");
         DONE = l.getLocalized("DONE");
         RECONSTRUCTING = l.getLocalized("RECONSTRUCTING");
         CANCELLED = l.getLocalized("CANCELLED");
         OPENING = l.getLocalized("OPENING");
      }
      catch (Exception e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog
            (null, "Localizer error in imc.lecturnity.util.gif2png.Gif2PngEngine:\n" +
             e.toString(), "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(0);
      }
   }

   private String fileName_    = null;
   private String outFileName_ = null;

   private Archiver archiver_  = null;

   private LogProgressListener overallProgress_ = null;
   private LogProgressListener detailProgress_  = null;

   private boolean cancelRequested_ = false;

   public Gif2PngEngine(String fileName,
                        String outFileName,
                        LogProgressListener overallProgress,
                        LogProgressListener detailProgress)
   {
      fileName_ = "" + fileName;
      outFileName_ = "" + outFileName;

      if (overallProgress == null)
         overallProgress_ = new DefaultLogProgressListener();
      else
         overallProgress_ = overallProgress;

      if (detailProgress == null)
         detailProgress_ = new DefaultLogProgressListener();
      else
         detailProgress_ = detailProgress;
   }

   public void update()
      throws CancelException, IOException
   {
      cancelRequested_ = false;

      detailProgress_.logString(OPENING + "\"" + fileName_ + "\"...");

      boolean isLpd = FileResources.hasNewFormat(fileName_);
      if (!isLpd)
      {
         handleLrdDocument();
      }
      else
      {
         handleLpdDocument();
      }
   }

   private void handleLrdDocument() 
      throws CancelException, IOException
   {
      System.err.println("Do not use anymore. Does not hanle encodings correctly.");
      
      overallProgress_.setValue(0);
      overallProgress_.setMaximum(3);

      detailProgress_.setValue(0);
      detailProgress_.setMaximum(1);
      detailProgress_.logString(ANALYZING);

      FileResources resources = FileResources.createFileResources(fileName_);
      boolean success = ConsistencyChecker.checkForConsistency(fileName_);
      PresentationData data = ConsistencyChecker.getLastPresentationData();

      String dir = data.presentationPath + File.separator;

      overallProgress_.setValue(1);
      checkCancel();

      PrintWriter objOut = new PrintWriter
         (new BufferedWriter(new FileWriter
            (dir + data.objectFileName + ".new")));
      
      BufferedReader objIn = new BufferedReader
         (new InputStreamReader(resources.createInputStream(data.objectFileName)));

      String[] gifFiles = getGifFiles(objIn, objOut);

      objIn.close();
      objOut.flush();
      objOut.close();

      overallProgress_.setValue(2);
      checkCancel();

      detailProgress_.logString(UPDATING);
      detailProgress_.setMaximum(gifFiles.length);
      for (int i=0; i<gifFiles.length; ++i)
      {
         detailProgress_.setValue(i);
         String pngFileName = gifFiles[i].substring(0, gifFiles[i].length() - 4) + ".png";
         convertImageToPng(dir + gifFiles[i], dir + pngFileName);
         if (cancelRequested_)
            break;
      }
      detailProgress_.setValue(gifFiles.length);
      checkCancel();

      File objFileNew = new File(dir + data.objectFileName + ".new");
      File objFileOld = new File(dir + data.objectFileName);
      objFileOld.renameTo(new File(dir + data.objectFileName + ".bak"));
      objFileNew.renameTo(new File(dir + data.objectFileName));

      overallProgress_.setValue(3);
      detailProgress_.setValue(1);
      detailProgress_.setMaximum(1);
      detailProgress_.logString(DONE);
   }

   private void handleLpdDocument() throws IOException, CancelException
   {
      System.err.println("Do not use anymore. Does not hanle encodings correctly.");
      
      FileStruct[] newFiles = null;

      try
      {
         overallProgress_.setValue(0);
         overallProgress_.setMaximum(4);
         detailProgress_.setValue(0);
         detailProgress_.setMaximum(1);
         detailProgress_.logString(ANALYZING);

         FileResources resources = FileResources.createFileResources(fileName_);

         int fileCounter = -1;
      
         boolean success = ConsistencyChecker.checkForConsistency
            (resources.createConfigFileInputStream(), null, true);
         PresentationData data = ConsistencyChecker.getLastPresentationData();

         java.util.Random random = new java.util.Random();
         byte[] key = new byte[512];
         random.nextBytes(key);
//          for (int i=0; i<key.length; ++i)
//             key[i] = 0;

         Hashtable fileTab = resources.getArchivedFiles();
         FileStruct[] files = new FileStruct[fileTab.size()];
         files = (FileStruct[]) (fileTab.values().toArray(files));

         newFiles = new FileStruct[files.length];
         for (int i=0; i<newFiles.length; ++i)
            newFiles[i] = new FileStruct();

         detailProgress_.logString(UPDATING);
         overallProgress_.setValue(1);
         checkCancel();
      
         String tempObjName = outFileName_ + "_" + (++fileCounter) + ".tmp";
         PrintWriter objOut = new PrintWriter
            (new BufferedWriter(new OutputStreamWriter
               (new XorOutputStream(new FileOutputStream
                  (tempObjName), key))));
             
         newFiles[fileCounter].name         = data.objectFileName;
         newFiles[fileCounter].realName     = tempObjName;
         newFiles[fileCounter].isCompressed = true;
         newFiles[fileCounter].isConfigFile = false;

         BufferedReader objIn = new BufferedReader
            (new InputStreamReader(resources.createInputStream(data.objectFileName)));

         String[] gifFiles = getGifFiles(objIn, objOut);

         objIn.close();
         objOut.flush();
         objOut.close();

         checkCancel();
         overallProgress_.setValue(2);
         detailProgress_.setValue(0);
         detailProgress_.setMaximum(gifFiles.length);

         for (int i=0; i<gifFiles.length; ++i)
         {
            detailProgress_.setValue(i);

            String pngFileName = gifFiles[i].substring(0, gifFiles[i].length() - 4) + ".png";
            String tempName    = outFileName_ + "_" + (++fileCounter) + ".tmp";

            int len = (int) resources.getFileSize(gifFiles[i]);

            // Fscking java: It does not work to write directly to the XorOutputStream;
            // I don't know why, but writing into the byte array stream, and then
            // writing the byte array to a file works. What the heck...
            ByteArrayOutputStream imgOut = new ByteArrayOutputStream(65536);
            InputStream           imgIn  = resources.createInputStream(gifFiles[i]);

            convertImageToPng(imgIn, len, imgOut);
            
            imgIn.close();
            imgOut.flush();
            imgOut.close();

            XorOutputStream xorOut = new XorOutputStream(new FileOutputStream(tempName), key);
            xorOut.write(imgOut.toByteArray());
            xorOut.flush();
            xorOut.close();

            newFiles[fileCounter].name         = pngFileName;
            newFiles[fileCounter].realName     = tempName;
            newFiles[fileCounter].isCompressed = true;
            newFiles[fileCounter].isConfigFile = false;
            if (cancelRequested_)
               break;
         }

         checkCancel();
         detailProgress_.setValue(gifFiles.length);

         List fileList = Arrays.asList(gifFiles);
         HashSet gifSet = new HashSet(fileList);
         gifSet.add(data.objectFileName);

         overallProgress_.setValue(3);
         detailProgress_.logString(RECONSTRUCTING);
         detailProgress_.setValue(0);
         detailProgress_.setMaximum(files.length);
      
         for (int i=0; i<files.length; ++i)
         {
            detailProgress_.setValue(i);
            if (!gifSet.contains(files[i].name))
            {
               String tempName = outFileName_ + "_" + (++fileCounter) + ".tmp";
               extractResource(resources, files[i].name, tempName, key);
               newFiles[fileCounter].name         = files[i].name;
               newFiles[fileCounter].realName     = tempName;
               newFiles[fileCounter].isCompressed = files[i].isCompressed;
               newFiles[fileCounter].isConfigFile = files[i].isConfigFile;
            }
         }

         checkCancel();
         detailProgress_.setValue(files.length);

         archiver_ = new Archiver(outFileName_, null, 
                                  new DummyConversionDisplay(), key);
         for (int i=0; i<newFiles.length; ++i)
         {
            if (newFiles[i].isConfigFile)
               archiver_.addConfigFileAs(newFiles[i].realName, newFiles[i].name);
            else
               archiver_.addFileAs
                  (newFiles[i].realName, newFiles[i].name, newFiles[i].isCompressed);
         }

         // is it a help document? (aka quick start document)
         archiver_.setCreateHelpDocument(FileResources.isHelpDocument(fileName_));

         archiver_.writeArchive();
         checkCancel();

         overallProgress_.setValue(4);
         detailProgress_.setValue(1);
         detailProgress_.setMaximum(1);
         detailProgress_.logString(DONE);
      }
      finally
      {
         if (newFiles != null)
         {
            for (int i=0; i<newFiles.length; ++i)
            {
               if (newFiles[i].realName != null)
               {
                  File file = new File(newFiles[i].realName);
                  file.delete();
               }
            }
         }
      }
   }

   private void extractResource(FileResources res, String resName, String outName, byte[] outputKey)
      throws IOException
   {
      InputStream in = res.createInputStream(resName);
      byte[] buf = new byte[16384];
      
      OutputStream out = new BufferedOutputStream
         (new XorOutputStream(new FileOutputStream(outName), outputKey));

      int readBytes = 0;
      while (readBytes != -1)
      {
         readBytes = in.read(buf);
         if (readBytes != -1)
         {
            out.write(buf, 0, readBytes);
         }
      }

      out.flush();
      out.close();
      in.close();
   }

   private String[] getGifFiles(BufferedReader objIn, PrintWriter objOut) 
      throws IOException
   {
      String  line        = objIn.readLine();
      boolean lineWritten = false;

      HashSet set = new HashSet();

      while (line != null)
      {
         lineWritten = false;

         String tmp = line.trim().toUpperCase();
         if (tmp.startsWith("<IMAGE"))
         {
            int fnamePos = line.toUpperCase().indexOf("FNAME=");
            if (fnamePos >= 0)
            {
               int quotePos = line.indexOf("\"", fnamePos);
               int quote2Pos = line.indexOf("\"", quotePos + 1);

               String gifName = "";
               if (quotePos >= 0 && quote2Pos > 0)
               {
                  // <IMAGE> tag with quotes around the image file name, good!
                  gifName = line.substring(quotePos + 1, quote2Pos);
               }
               else
               {
                  // no quotes around the file name... hmm.
                  int bracketPos = line.indexOf(">", fnamePos);
                  int spacePos   = line.indexOf(" ", fnamePos);

                  if (bracketPos >= 0 || spacePos >= 0)
                  {
                     if (bracketPos < 0)
                        bracketPos = Integer.MAX_VALUE;
                     if (spacePos < 0)
                        spacePos = Integer.MAX_VALUE;

                     int delimPos = Math.min(bracketPos, spacePos);
                     gifName = line.substring(fnamePos + 6, delimPos);

                     quotePos = fnamePos + 5;
                     quote2Pos = delimPos;
                  }
               }
               
               if (gifName.toUpperCase().endsWith(".GIF"))
               {
                  // found an <IMAGE> GIF
                  if (!set.contains(gifName))
                     set.add(gifName);

                  objOut.println(line.substring(0, quotePos + 1) + 
                                 gifName.substring(0, gifName.length() - 4) + ".png" +
                                 line.substring(quote2Pos));
                  lineWritten = true;
               }
            }
         }

         if (!lineWritten)
            objOut.println(line);

         line = objIn.readLine();
      }

      String[] ret = new String[set.size()];
      ret = (String[]) set.toArray(ret);

      return ret;
   }

   public void requestCancel()
   {
      cancelRequested_ = true;
      if (archiver_ != null)
         archiver_.cancelOperation();
   }

   private void checkCancel() throws CancelException
   {
      if (cancelRequested_)
         throw new CancelException(CANCELLED);
   }

   /**
    * This static methods takes an arbitrary file name of an image readable by 
    * the default toolkit of this JVM (at least GIF and JPEG) and writes a
    * PNG image of it to the file name given in <tt>imgOutName</tt>.
    */
   public static void convertImageToPng(String imgInName, String imgOutName)
      throws IOException
   {
      InputStream imgIn = new BufferedInputStream(new FileInputStream(imgInName));
      OutputStream imgOut = new BufferedOutputStream(new FileOutputStream(imgOutName));

      convertImageToPng(imgIn, (int) (new File(imgInName)).length(), imgOut);
      
      imgIn.close();
      imgOut.flush();
      imgOut.close();
   }

   /**
    * This static method writes a PNG image into the given output stream 
    * (<tt>imgOut</tt>). The input stream must point to image data in a format
    * which is handled by the default <tt>Toolkit</tt> (i.e. GIF or JPEG image
    * data).
    */
   public static void convertImageToPng(InputStream imgIn, int imgInSize, 
                                        OutputStream imgOut)
      throws IOException
   {
      byte[] imgData = new byte[(int) imgInSize];
      int readData = 0;
      int readBytes = 0;
      int bytesLeft = (int) imgInSize;
      while (readBytes != -1 && bytesLeft > 0)
      {
         readBytes = imgIn.read(imgData, readData, bytesLeft);
         if (readBytes != -1)
         {
            bytesLeft -= readBytes;
            readData  += readBytes;
         }
      }

      if (bytesLeft > 0)
         throw new IOException("Could not read image completely!");

      Toolkit tk = Toolkit.getDefaultToolkit();
      Image awtImage = tk.createImage(imgData);

      Button button = new Button("blubb");
      
      MediaTracker mt = new MediaTracker(button);
      mt.addImage(awtImage, 0);
      try
      {
         mt.waitForAll();
      }
      catch (InterruptedException e)
      {
      }

      BufferedImage tmpImg = new BufferedImage(awtImage.getWidth(button), awtImage.getHeight(button),
                                               BufferedImage.TYPE_INT_ARGB);
      Graphics g = tmpImg.getGraphics();
      g.drawImage(awtImage, 0, 0, button);

      PNGEncodeParam pngParam = PNGEncodeParam.getDefaultEncodeParam(tmpImg);
      ImageEncoder pngWriter = 
         ImageCodec.createImageEncoder("PNG", imgOut,
                                       pngParam);
      pngWriter.encode(tmpImg);
   }
}

class DefaultLogProgressListener implements LogProgressListener
{
   int curMax_ = 1;
   PrintStream out_;

   public DefaultLogProgressListener()
   {
      out_ = System.out;
   }

   public void setMaximum(int max)
   {
      curMax_ = max;
   }

   public void setValue(int value)
   {
      out_.print(".");
      if (value == curMax_)
         out_.println();
   }

   public void logString(String msg)
   {
      out_.println();
      out_.println(msg);
   }
}

class DummyConversionDisplay implements ConversionDisplay
{
   public void displayCurrentFile(String newFile, int fileSize, String action)
   {
   }

   public void displayCurrentFileProgress(int bytesDone)
   {
   }

   public void displayCurrentProgress(float fProgress)
   {
   }

   public void logMessage(String message)
   {
   }

   public void displayCurrentProgressStep(String strStepId, String strLabel)
   {
   }
}
