package imc.lecturnity.converter.wizard;

import java.awt.Dimension;
import java.awt.Container;
import java.awt.Point;
import java.awt.Font;
import java.awt.Color;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import javax.swing.*;
import javax.swing.border.BevelBorder;

import java.text.DateFormat;

import java.util.Vector;
import java.util.Date;
import java.util.Locale;
import java.util.HashMap;

import java.io.*;
import java.util.Random;
import java.util.zip.CRC32;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.*;

import imc.epresenter.filesdk.Metadata;
import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.XorOutputStream;

import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.filesdk.util.XmlParser;
import imc.epresenter.filesdk.util.XmlFormatException;

import imc.epresenter.player.util.XmlTag;

class CdProjectWriterWizardPanel extends AbstractCdProjectWizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String COPYING = "[!]";
   private static String CREATING = "[!]";
   private static String PROGRESS = "[!]";
   private static String CANCEL = "[!]";
   private static char   MNEM_CANCEL = '?';
   private static String CANCELLED = "[!]";
   private static String SUCCESS = "[!]";
   private static String INFORMATION = "[!]";
   private static String KEY_FAILED = "[!]";
   private static String DONE = "[!]";
   private static String RUNTIME = "[!]";
   private static String WARNING = "[!]";
   private static String ERR_METADATA = "[!]";
   private static String ERR_NO_SUCCESS = "[!]";
   private static String ERR_HTML = "[!]";
   private static String ERR_CACHE_CFG = "[!]";
   private static String START = "[!]";
   private static String ERR_EXE_START_PATH = "[!]";
   
   private static JaiFileFilter jaiFileFilter = new JaiFileFilter();
   private static NoTemplatesFilter noTemplatesFilter = new NoTemplatesFilter();

   // These files are used if there is a stand-alone player installed on the
   // Lecturnity CD.
   private static String[][] FILES = {{"Player/player.jar", "bin/player.jar"},
                                      {"Backend/nativeutils.dll", "bin/nativeutils.dll"},
                                      {"lsgc.dll", "bin/lsgc.dll"},
                                      {"Publisher/CD Templates/kickstart.jar", "bin/kickstart.jar"},
                                      {"Publisher/CD Templates/autorun.exe", "autorun.exe"},
                                      {"Publisher/CD Templates/autorun.ico", "autorun.ico"},
                                      {"Publisher/CD Templates/autorun.inf", "autorun.inf"}};
   // These files are files copied to the Lecturnity CD if there is no
   // Lecturnity Player in stand-alone mode installed.
   private static String[][] FILES2 = {{"Publisher/CD Templates/autorun.exe", "autorun.exe"},
                                       {"Publisher/CD Templates/autorun.ico", "autorun.ico"},
                                       {"Publisher/CD Templates/autorun.inf", "autorun.inf"}};

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/CdProjectWriterWizardPanel_",
             "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         COPYING = l.getLocalized("COPYING");
         CREATING = l.getLocalized("CREATING");
         PROGRESS = l.getLocalized("PROGRESS");
         CANCEL = l.getLocalized("CANCEL");
         MNEM_CANCEL = l.getLocalized("MNEM_CANCEL").charAt(0);
         CANCELLED = l.getLocalized("CANCELLED");
         SUCCESS = l.getLocalized("SUCCESS");
         INFORMATION = l.getLocalized("INFORMATION");
         KEY_FAILED = l.getLocalized("KEY_FAILED");
         DONE = l.getLocalized("DONE");
         RUNTIME = l.getLocalized("RUNTIME");
         WARNING = l.getLocalized("WARNING");
         ERR_METADATA = l.getLocalized("ERR_METADATA");
         ERR_NO_SUCCESS = l.getLocalized("ERR_NO_SUCCESS");
         ERR_HTML = l.getLocalized("ERR_HTML");
         ERR_CACHE_CFG = l.getLocalized("ERR_CACHE_CFG");
         START = l.getLocalized("START");
         ERR_EXE_START_PATH = l.getLocalized("ERR_EXE_START_PATH");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private JProgressBar overallProgress_;
   private JProgressBar fileProgress_;

   private JLabel fileLabel_;
   private JButton cancelButton_;
   
   private boolean cancelRequest_ = false;
   private FileUtils fileUtils_ = null;

   private String docLengthString_ = "0:00";
   
   public CdProjectWriterWizardPanel(WizardData data)
   {
      super(data);

      setPreferredSize(new Dimension(500, 400));

      if (NativeUtils.isLibraryLoaded())
      {
         addButtons(BUTTON_CUSTOM | BUTTON_QUIT | BUTTON_BACK | BUTTON_CANCEL, START, 's');
      }
      else
      {
         addButtons(BACK_QUIT_CANCEL);
      }
      // setEnableButtons(BUTTON_BACK | BUTTON_FINISH | BUTTON_CANCEL, false);

      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");

      Container r = getContentPanel();
      r.setLayout(null);

      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      JLabel progressLabel = new JLabel(PROGRESS, JLabel.CENTER);
      progressLabel.setSize(440, 20);
      progressLabel.setLocation(30, 50);
      progressLabel.setFont(descFont);
      r.add(progressLabel);

      overallProgress_ = new JProgressBar(JProgressBar.HORIZONTAL, 0, 100);
      overallProgress_.setSize(440, 30);
      overallProgress_.setLocation(30, 70);
      r.add(overallProgress_);

      fileLabel_ = new JLabel("", JLabel.CENTER);
      fileLabel_.setSize(440, 20);
      fileLabel_.setLocation(30, 130);
      fileLabel_.setFont(descFont);
      r.add(fileLabel_);
      
      fileProgress_ = new JProgressBar(JProgressBar.HORIZONTAL, 0, 100);
      fileProgress_.setSize(440, 30);
      fileProgress_.setLocation(30, 150);
      r.add(fileProgress_);
      
      cancelButton_ = new JButton(CANCEL);
      cancelButton_.setSize(140, 30);
      cancelButton_.setLocation(180, 200);
      cancelButton_.addActionListener(new ButtonListener());
      cancelButton_.setEnabled(false);
      cancelButton_.setMnemonic(MNEM_CANCEL);
      r.add(cancelButton_);
   }

   public String getDescription()
   {
      return "";
   }

   public WizardData getWizardData()
   {
      return cdData;
   }

   public int displayWizard()
   {
      cancelButton_.setEnabled(true);
      if(NativeUtils.isLibraryLoaded())
         setEnableButton(BUTTON_CUSTOM | BUTTON_BACK | BUTTON_QUIT | BUTTON_CANCEL, false);
      else
         setEnableButton(BUTTON_BACK | BUTTON_QUIT | BUTTON_CANCEL, false);

      cancelRequest_ = false;

      // overall progress displays copy progress of the document
      // files. the last three steps are copying of the jre, creation
      // of the html page and copying the player jars (and the kickstart).
      overallProgress_.setValue(0);
      overallProgress_.setMaximum(cdData.projectDocs.size() + 5);

      fileUtils_ = new FileUtils(this);

      boolean success = copyFiles();
         
      if (cancelRequest_)
         JOptionPane.showMessageDialog(this, CANCELLED, INFORMATION, 
                                       JOptionPane.INFORMATION_MESSAGE);
      
      if (!success)
      {
         JOptionPane.showMessageDialog(this, ERR_NO_SUCCESS, ERROR,
                                       JOptionPane.ERROR_MESSAGE);
         setEnableButton(BUTTON_CANCEL | BUTTON_BACK, true);
         setEnableButton(BUTTON_QUIT, false);
         if (NativeUtils.isLibraryLoaded())
            setEnableButton(BUTTON_CUSTOM, false);
      }
      else
      {
         JOptionPane.showMessageDialog(this, SUCCESS, INFORMATION,
                                       JOptionPane.INFORMATION_MESSAGE);
         setEnableButton(BUTTON_CANCEL | BUTTON_BACK, false);
         setEnableButton(BUTTON_QUIT, true);
         if (NativeUtils.isLibraryLoaded())
            setEnableButton(BUTTON_CUSTOM, true);
      }

      cancelButton_.setEnabled(false);
      return super.displayWizard();
   }

   protected void customCommand(String command)
   {
      if (getWizardData() != null)
      {
         String strCommand = ((CdProjectWizardData)getWizardData()).targetDir;
         File exeFile = new File(new File(strCommand), "autorun.exe");
        
         try
         {
            Runtime.getRuntime().exec(exeFile.toString());
         }
         catch (IOException e)
         {          
            // might be a problem with the path
            // use default/system codepage for both methods:
            byte[] aCodepagePath = exeFile.getPath().getBytes();
            String strCodepagePath = new String(aCodepagePath);
            
            if (!exeFile.getPath().equals(strCodepagePath))
            {
               JOptionPane.showMessageDialog(this, ERR_EXE_START_PATH, ERROR,
                                             JOptionPane.ERROR_MESSAGE);
            }
            else
               e.printStackTrace(); // unknown error
         }
       
         doQuit();
      }
   }
   
   private boolean copyFiles()
   {
      ProgressListener progressListener = new FileProgressListener();

      int numDocs = cdData.projectDocs.size();

      boolean success;

      if (cdData.standalonePlayer)
      {
         fileLabel_.setText(COPYING + RUNTIME);
         success = fileUtils_.copyRecursive
            (new File(getInstallDir() + "jre5"),
             new File(cdData.targetDir + File.separatorChar + "bin" + File.separatorChar + "jre"),
             jaiFileFilter, progressListener);
         
         if (!success || cancelRequest_)
            return false;
         
         overallProgress_.setValue(1);
         
         String strSystemRoot = NativeUtils.getEnvVariable("SYSTEMROOT");
         
         for (int i=0; i<FILES.length; i++)
         {
            File source = new File(getInstallDir() + FILES[i][0]);
            // Bugfix 2701: special case: lsgc.dll is in %SYSTEMROOT%/system32
            if (FILES[i][0].indexOf("lsgc.dll") == 0)
               source = new File(strSystemRoot + "/system32/lsgc.dll");
               
            File target = new File(cdData.targetDir + File.separatorChar + FILES[i][1]);
            
            fileLabel_.setText(COPYING + source.getName() + "...");
            success = fileUtils_.copyFile2(source, target, progressListener);
            
            if (!success || cancelRequest_)
               return false;
         }
      }
      else
      {
         // 1.4.0 bugfix:
         // we still need the autorun.exe
         for (int i=0; i<FILES2.length; i++)
         {
            File source = new File(getInstallDir() + FILES2[i][0]);
            File target = new File(cdData.targetDir + File.separatorChar + FILES2[i][1]);
            
            fileLabel_.setText(COPYING + source.getName() + "...");
            success = fileUtils_.copyFile2(source, target, progressListener);
            
            if (!success || cancelRequest_)
               return false;
         }
      }

      overallProgress_.setValue(2);
      
      File docDir = new File(cdData.targetDir + File.separatorChar + "docs");
      if (!docDir.exists())
         docDir.mkdir();
      
      for (int i=0; i<numDocs; i++)
      {
         File source = new File(cdData.projectDocs.elementAt(i).toString());
//          File target = new File
//             (cdData.targetDir + File.separatorChar + "docs" +
//             File.separatorChar + source.getName());
         File target = new File
            (cdData.targetDir + File.separatorChar + "docs" +
             File.separatorChar + "doc" + (i+1) + ".lpd");

         fileLabel_.setText(COPYING + source.getName() + "...");
         success = fileUtils_.copyFile2(source, target, progressListener);

         if (!success || cancelRequest_)
            return false;
         
         overallProgress_.setValue(i + 3);
      }

      fileLabel_.setText(COPYING + "lecturnity.dat...");
      success = generateDatFiles(progressListener);
      if (!success || cancelRequest_)
         return false;

      fileLabel_.setText(CREATING + "cache.cfg...");
      success = generateCacheConfigFile(progressListener);
      if (!success || cancelRequest_)
         return false;

      fileLabel_.setText(CREATING + "HTML...");
      success = generateHtmlFiles(progressListener);
      if (!success || cancelRequest_)
         return false;
      overallProgress_.setValue(numDocs + 3);
      
      fileLabel_.setText(COPYING + "HTML...");
      success = fileUtils_.copyRecursive
         (new File(cdData.templateDir),
          new File(cdData.targetDir + File.separatorChar + "docs"),
          noTemplatesFilter, progressListener);
      if (!success || cancelRequest_)
         return false;

      overallProgress_.setValue(numDocs + 4);

      if (cdData.copyPlayerSetup)
      {
         fileLabel_.setText(COPYING + "Lecturnity Player setup...");

         File playerSetupFile = new File(cdData.playerSetupProject);

         // rename the player setup file to setup.exe always
         success = fileUtils_.copyFile2
            (playerSetupFile,
             new File(cdData.targetDir + File.separatorChar + "setup.exe"),
             progressListener);

         if (!success || cancelRequest_)
            return false;

         // Look for a jar file with the same name and the ending .jar?
         String jarFileName = "" + playerSetupFile.toString();
         if (jarFileName.toUpperCase().endsWith(".EXE"))
            jarFileName = jarFileName.substring(0, jarFileName.length() - 4);
         jarFileName += ".jar";

         File jarFile = new File(jarFileName);
         if (jarFile.exists())
         { 
            // A jar file exists, copy that one, too.
            success = fileUtils_.copyFile2
               (jarFile,
                new File(cdData.targetDir + File.separatorChar + jarFile.getName()),
                progressListener);
         }

         if (!success || cancelRequest_)
            return false;
      }
      
      overallProgress_.setValue(overallProgress_.getMaximum());

      fileLabel_.setText(DONE);
      return !cancelRequest_;
   }

   /**
    * This methods creates the key file for the security check at the
    * start of the lecturnity player. This method also writes a file
    * "version.dat" with the player version supplied with the
    * cd. perhaps.
    */
   private boolean generateDatFiles(ProgressListener progressListener)
   {
      try
      {
         byte[] key = createRandomKey();

         // new from version 1.3.1.b5 and on: use the first two bytes
         // of the key to store the language. this means that it is
         // not possible to change the language without destroying the
         // key.
         String langCode = NativeUtils.getLanguageCode();
         if (langCode != null && langCode.length() > 1)
         {
            key[0] = (byte) langCode.charAt(0);
            key[1] = (byte) langCode.charAt(1);
         }
         else
         {
            // ignore this for now; use created (random) key
         }
         
         // write version number of dat file
         // used upon NativeUtils::IsProtectedDocument to find out
         // whether file names are utf8
         key[2] = (byte)0x1e; // the identifier
         key[3] = (byte)0xcd;
         key[4] = (byte)0xad;
         key[5] = (byte)2; // the version

         OutputStream out = new BufferedOutputStream
            (new FileOutputStream(cdData.targetDir + File.separatorChar + "docs" +
                                  File.separatorChar + "lecturnity.dat"));
         out.write(key);
         
         out = new XorOutputStream(out, key);

         progressListener.setValue(0);
         progressListener.setMaximum(cdData.projectDocs.size() - 1);
         for (int i=0; i<cdData.projectDocs.size(); i++)
         {
            String fileName = cdData.projectDocs.elementAt(i).toString();
            String crc = createCrc32String(fileName);
            byte[] tempCrc = crc.getBytes();

            // Bugfix: Up to version 1.4.1.b2, the CRC string could
            // accidentally have less than 8 bytes; this resulted in
            // an incorrect CD export (Bug #649).
            byte[] crcBytes = new byte[8];
            java.util.Arrays.fill(crcBytes, (byte) 0);
            for (int j=0; j<tempCrc.length; ++j)
               crcBytes[j] = tempCrc[j];

            String strippedFileName = "doc" + (i+1) + ".lpd" + ((char) 0); //file.getName() + ((char) 0);
            byte[] aFileNameBytesUtf8 = strippedFileName.getBytes("utf-8");
            
            out.write(crcBytes);
            out.write(aFileNameBytesUtf8);
            progressListener.setValue(i);
         }

         out.flush();
         out.close();

         // now write the current version into a separate file:
         // The short version can be found in the PresentationConverter
         // We take the short version: This version contains the patch level
         // of the Installation, but not things like "Campus" or the beta version.
         String playerVersion = imc.epresenter.converter.PresentationConverter.SHORT_VERSION;

         // do not use utf-8 here; plain Ascii content
         PrintWriter verOut = new PrintWriter
            (new FileWriter(cdData.targetDir + File.separatorChar + "version.dat"));
         verOut.println(playerVersion + '.' + ((char) 0));
         verOut.flush();
         verOut.close();

         return true;
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(this, KEY_FAILED + e.getMessage(),
                                       ERROR, JOptionPane.ERROR_MESSAGE);
         return false;
      }
   }

   private byte[] createRandomKey()
   {
      byte[] tmp = new byte[512];
      Random random = new Random();
      random.nextBytes(tmp);
      return tmp;
   }

   private String createCrc32String(String lpdFileName) throws IOException
   {
      DataInputStream in = new DataInputStream
         (new BufferedInputStream(new FileInputStream(lpdFileName)));

      byte[] buf = new byte[4];

      in.readFully(buf);

      if (!(buf[0] == 'E' &&
            buf[1] == 'P' &&
            buf[2] == 'F'))
         throw new IOException("file is not an LPD file: " + lpdFileName);

      buf = new byte[256];
      in.readFully(buf);

      CRC32 crc32 = new CRC32();
      crc32.update(buf);

      return Long.toHexString(crc32.getValue());
   }

   private boolean generateHtmlFiles(ProgressListener progressListener)
   {
      progressListener.setValue(0);
      progressListener.setMaximum(cdData.projectDocs.size());

      File docFile = new File(cdData.templateDir + File.separatorChar + "doc.html.tmpl");
      File indexFile = new File(cdData.templateDir + File.separatorChar + "index.html.tmpl");

      try
      {
//          StringOutputStream docOut = new StringOutputStream();

         StringBuffer docOut = new StringBuffer(4096);
         
         for (int i=0; i<cdData.projectDocs.size(); i++)
         {
            String lpdFileName = cdData.targetDir + File.separatorChar + "docs" + 
               File.separatorChar + "doc" + (i+1) + ".lpd"; //cdData.projectDocs.elementAt(i).toString();

            FileInputStream docFileIn = new FileInputStream(docFile);

            HashMap mapReplacements = getLpdResourceMap(lpdFileName);
            AddLanguageReplacements(mapReplacements);
            
            TemplateParser templateParser =
               new TemplateParser(docFileIn, mapReplacements);
            templateParser.doParse();

            docFileIn.close();

            docOut.append(templateParser.getOutputString());

            progressListener.setValue(i);
         }

         //String docToken = slashify(docOut.toString());
         String docToken = docOut.toString();

         HashMap resourceMap = new HashMap();
         resourceMap.put("DOCS", docToken);
         resourceMap.put("CREATIONDATE", "" + 
             DateFormat.getDateTimeInstance
             (DateFormat.LONG, DateFormat.LONG,
              Locale.getDefault()).format(new Date()));
         
         AddLanguageReplacements(resourceMap);

         File targetFile = new File(cdData.targetDir + File.separatorChar + "docs" +
                                      File.separatorChar + "index.html");
 
         FileInputStream indexFileIn = new FileInputStream(indexFile);
         FileOutputStream indexFileOut = new FileOutputStream(targetFile);

         TemplateParser indexFileParser = new TemplateParser(
            indexFileIn, indexFileOut, resourceMap);
         
         indexFileParser.doParse();

         indexFileOut.flush();
         indexFileOut.close();
         indexFileIn.close();
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(this, ERR_HTML + e.toString(), WARNING,
                                       JOptionPane.WARNING_MESSAGE);
         e.printStackTrace();
         return false;
      }
      return true;
   }

   private HashMap getLpdResourceMap(String lpdFileName)
   {
      Metadata metadata = retrieveMetadata(lpdFileName);

      HashMap map = new HashMap(50);

      // read document length by sideeffect:
      String lenString = docLengthString_;
      
      String resourceString = "";

      if (metadata == null)
      {
         String strippedName = (new File(lpdFileName)).getName();

         map.put("TITLE", "<a href=\"" + strippedName + "\">[" + 
                 htmlify(strippedName) + "]</a>");
//          resourceString += "TITLE=<a href=\\\"" + strippedName + "\\\">[" + 
//             htmlify(strippedName) + "]</a>\n";
         map.put("AUTHOR", "&lt;unknown&gt;");
//          resourceString += "AUTHOR=&lt;unknown&gt;\n";
         map.put("PRODUCER", "&lt;unknown&gt;");
//          resourceString += "PRODUCER=&lt;unknown&gt;\n";
         map.put("LENGTH", "&lt;unknown&gt;");
//          resourceString += "LENGTH=&lt;unknown&gt;\n";
         map.put("DOC_KEYWORDS", "&lt;unknown&gt;");
//          resourceString += "DOC_KEYWORDS=&lt;unknown&gt;\n";
         map.put("DATE", "&lt;unknown&gt;");
//          resourceString += "DATE=&lt;unknown&gt;\n";
         map.put("NUM_SLIDES", "&lt;unknown&gt;");
//          resourceString += "NUM_SLIDES=&lt;unknown&gt;\n";
      }
      else
      {
//          long lastSlideEnd = metadata.thumbnails[metadata.thumbnails.length - 1].endTimestamp;
// //          String lenString = sdf.format(new Date(lastSlideEnd));
//          int totalSecs = ((int) ((lastSlideEnd + 500)/1000));
//          int secs = totalSecs % 60;
//          int mins = totalSecs / 60;
//          String lenString = "" + mins + ":" + ((secs < 10) ? "0" + secs : "" + secs);

         String strippedName = (new File(lpdFileName)).getName();

         if (metadata.title == null)
            metadata.title = "[" + strippedName + "]";
         if (metadata.title.equals(""))
            metadata.title = "[" + strippedName + "]";

         map.put("TITLE", "<a href=\"" + strippedName + "\">" +
                 htmlify(metadata.title) + "</a>");
//          resourceString += "TITLE=<a href=\\\"" + strippedName + "\\\">" +
//             htmlify(metadata.title) + "</a>\n";
         map.put("AUTHOR", htmlify(metadata.author));
//          resourceString += "AUTHOR=" + htmlify(metadata.author) + '\n';
         map.put("PRODUCER", htmlify(metadata.creator));
//          resourceString += "PRODUCER=" + htmlify(metadata.creator) + '\n';
         map.put("LENGTH", htmlify(lenString));
//          resourceString += "LENGTH=" + htmlify(lenString) + '\n';
//          resourceString += "DOC_KEYWORDS=";

         // Now for the keywords.
         StringBuffer keywordBuffer = new StringBuffer(1024);

         for (int i=0; i<metadata.keywords.length; i++)
         {
//             resourceString += htmlify(metadata.keywords[i]);
//             if (i != metadata.keywords.length-1)
//                resourceString += ", ";
            keywordBuffer.append(htmlify(metadata.keywords[i]));
            if (i != metadata.keywords.length - 1)
               keywordBuffer.append(", ");
         }
//          if (metadata.keywords.length == 0)
//             resourceString += "-\n";
//          else
//             resourceString += ".\n";
         if (metadata.keywords.length == 0)
            keywordBuffer.append("-");
         else
            keywordBuffer.append(".");
         map.put("DOC_KEYWORDS", keywordBuffer.toString());

         map.put("DATE", htmlify(metadata.recordDate));
//          resourceString += "DATE=" + htmlify(metadata.recordDate) + '\n';
         map.put("NUM_SLIDES", "" + metadata.thumbnails.length);
//          resourceString += "NUM_SLIDES=" + metadata.thumbnails.length + '\n';

//          System.out.println("  resourceString==" + resourceString);
      }

//       byte[] stringBytes = resourceString.getBytes();

//       return new ByteArrayInputStream(stringBytes);

      return map;
   }

//    private InputStream getLpdResourceStream(String lpdFileName)
//    {
//       Metadata metadata = retrieveMetadata(lpdFileName);

//       // read document length by sideeffect:
//       String lenString = docLengthString_;
      
//       String resourceString = "";

//       if (metadata == null)
//       {
//          String strippedName = (new File(lpdFileName)).getName();

//          resourceString += "TITLE=<a href=\\\"" + strippedName + "\\\">[" + 
//             htmlify(strippedName) + "]</a>\n";
//          resourceString += "AUTHOR=&lt;unknown&gt;\n";
//          resourceString += "PRODUCER=&lt;unknown&gt;\n";
//          resourceString += "LENGTH=&lt;unknown&gt;\n";
//          resourceString += "DOC_KEYWORDS=&lt;unknown&gt;\n";
//          resourceString += "DATE=&lt;unknown&gt;\n";
//          resourceString += "NUM_SLIDES=&lt;unknown&gt;\n";
//       }
//       else
//       {
// //          long lastSlideEnd = metadata.thumbnails[metadata.thumbnails.length - 1].endTimestamp;
// // //          String lenString = sdf.format(new Date(lastSlideEnd));
// //          int totalSecs = ((int) ((lastSlideEnd + 500)/1000));
// //          int secs = totalSecs % 60;
// //          int mins = totalSecs / 60;
// //          String lenString = "" + mins + ":" + ((secs < 10) ? "0" + secs : "" + secs);

//          String strippedName = (new File(lpdFileName)).getName();

//          if (metadata.title == null)
//             metadata.title = "[" + strippedName + "]";
//          if (metadata.title.equals(""))
//             metadata.title = "[" + strippedName + "]";

//          resourceString += "TITLE=<a href=\\\"" + strippedName + "\\\">" +
//             htmlify(metadata.title) + "</a>\n";
//          resourceString += "AUTHOR=" + htmlify(metadata.author) + '\n';
//          resourceString += "PRODUCER=" + htmlify(metadata.creator) + '\n';
//          resourceString += "LENGTH=" + htmlify(lenString) + '\n';
//          resourceString += "DOC_KEYWORDS=";
//          for (int i=0; i<metadata.keywords.length; i++)
//          {
//             resourceString += htmlify(metadata.keywords[i]);
//             if (i != metadata.keywords.length-1)
//                resourceString += ", ";
//          }
//          if (metadata.keywords.length == 0)
//             resourceString += "-\n";
//          else
//             resourceString += ".\n";
//          resourceString += "DATE=" + htmlify(metadata.recordDate) + '\n';
//          resourceString += "NUM_SLIDES=" + metadata.thumbnails.length + '\n';

// //          System.out.println("  resourceString==" + resourceString);
//       }

//       byte[] stringBytes = resourceString.getBytes();

//       return new ByteArrayInputStream(stringBytes);
//    }

   private String htmlify(String string)
   {
      StringBuffer tmp = new StringBuffer();

      for (int i=0; i<string.length(); i++)
      {
         char c = string.charAt(i);
         
         switch (c)
         {
         case '<': tmp.append("&lt;"); break;
         case '>': tmp.append("&gt;"); break;
         case '&': tmp.append("&amp;"); break;
//          case 'ö': tmp.append("&ouml;"); break;
//          case 'ä': tmp.append("&auml;"); break;
//          case 'ü': tmp.append("&uuml;"); break;
//          case 'Ö': tmp.append("&Ouml;"); break;
//          case 'Ä': tmp.append("&Auml;"); break;
//          case 'Ü': tmp.append("&Uuml;"); break;
//          case 'å': tmp.append("&aring;"); break;
//          case 'Å': tmp.append("&Aring;"); break;
//          case 'ß': tmp.append("&szlig;"); break;
         case '"': tmp.append("&quot;"); break;
         default: tmp.append(c);
         }
      }

      return tmp.toString();
   }

   private Metadata retrieveMetadata(String lpdFileName)
   {
      try
      {
         Metadata metadata = null;

         FileResources fileResources = FileResources.createFileResources(lpdFileName);
         String lmdFileName = getMetadataFileName(fileResources);

         metadata = fileResources.getMetadata(lmdFileName);

         return metadata;
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog
            (this, ERR_METADATA + lpdFileName, WARNING,
             JOptionPane.WARNING_MESSAGE);
         
         e.printStackTrace();
         return null;
      }
   }

   private String getMetadataFileName(FileResources fileResources)
      throws IOException
   {
      InputStream configIn = new BufferedInputStream
         (fileResources.createConfigFileInputStream());

      XmlTag configTag = (XmlTag.parse(configIn))[0];
      configIn.close();
   
      Object[] arrLengths = configTag.getValues("LENGTH");
      if (arrLengths != null && arrLengths.length > 0)
         docLengthString_ = (String)arrLengths[0];
      
      Object[] metaA = configTag.getValues("METADATA");
      if (metaA != null && metaA.length > 0)
      {
         String metaFile = (String)metaA[0];
         return metaFile;
      }
      
      return null;
      
      
      /* not needed anymore; replaced by XmlTag (above)
      XmlParser parser = new XmlParser(configIn);

      int ttype = XmlParser.TAG_START;
      String metadataFileName = null;
      boolean found = false;

      boolean inDocument = false;
      boolean inFiles = false;
      boolean inMetadata = false;
      boolean inInfo = false;
      boolean inLength = false;

      while (ttype != XmlParser.EOF && !found)
      {
         ttype = parser.getNextTag();

         switch (ttype)
         {
         case XmlParser.TAG_START: {
            String tag = parser.getToken();
            
            if (inDocument)
            {
               if (inFiles)
               {
                  if (tag.equalsIgnoreCase("METADATA"))
                     inMetadata = true;
                  // else ignore
               }
               else if (tag.equalsIgnoreCase("FILES"))
               {
                  inFiles = true;
               }
               else if (inInfo)
               {
                  if (tag.equalsIgnoreCase("LENGTH"))
                     inLength = true;
                  // else ignore
               }
               else if (tag.equalsIgnoreCase("INFO"))
               {
                  inInfo = true;
               }
               // else ignore
            }
            else if (tag.equalsIgnoreCase("DOCUMENT"))
            {
               inDocument = true;
            }
            // ignore everything else

            break;
         }

         case XmlParser.TAG_END: {
            String tag = parser.getToken();

            if (inDocument)
            {
               if (inFiles)
               {
                  if (inMetadata)
                  {
                     if (tag.equalsIgnoreCase("METADATA"))
                        inMetadata = false;
                  }
                  else if (tag.equalsIgnoreCase("FILES"))
                  {
                     inFiles = false;
                  }
                  // else ignore
               }
               else if (inInfo)
               {
                  if (inLength)
                  {
                     if (tag.equalsIgnoreCase("LENGTH"))
                        inLength = false;
                  }
                  else if (tag.equalsIgnoreCase("INFO"))
                  {
                     inInfo = false;
                  }
                  // else ignore
               }
               else if (tag.equalsIgnoreCase("DOCUMENT"))
               {
                  inDocument = false;
               }
               // else ignore
            }
            // else ignore

            break;
         }

         case XmlParser.CONTENT: {
            String content = parser.getToken();
            if (inDocument)
            {
               if (inFiles)
               {
                  if (inMetadata)
                     metadataFileName = content;
               }
               else if (inInfo)
               {
                  if (inLength)
                     docLengthString_ = content;
               }
            }

            break;
         }

         default:
            break;
         }
      }

      return metadataFileName;
      */
   }

   private boolean generateCacheConfigFile(ProgressListener pl)
   {
      try
      {
         String strConfigFile = cdData.targetDir + File.separatorChar + "docs" +
            File.separatorChar + "cache.cfg";
        
         PrintWriter out = FileUtils.CreatePrintWriter(strConfigFile);
        
         switch (cdData.cacheSettings)
         {
         case CdProjectWizardData.CACHE_SETTINGS_HOME:
            out.println("settingsLocation=APPDATA");
            break;
         case CdProjectWizardData.CACHE_SETTINGS_REGISTRY:
            out.println("settingsLocation=REGISTRY");
            break;
         case CdProjectWizardData.CACHE_SETTINGS_TEMP:
         default:
            out.println("settingsLocation=TEMP");
            break;
         }

         switch (cdData.cacheWhen)
         {
         case CdProjectWizardData.CACHE_WHEN_CD:
            out.println("cacheMode=CD");
            break;
         case CdProjectWizardData.CACHE_WHEN_NEVER:
            out.println("cacheMode=NEVER");
            break;
         case CdProjectWizardData.CACHE_WHEN_ALWAYS:
         default:
            out.println("cacheMode=ALWAYS");
            break;
         }

         switch (cdData.cacheWhere)
         {
         case CdProjectWizardData.CACHE_WHERE_CUSTOM:
            out.println("cacheLocation=" + cdData.cacheDirectory);
            break;
         case CdProjectWizardData.CACHE_WHERE_TEMP:
            out.println("cacheLocation=TEMP");
            break;
         }

         out.println("maxCacheSize=" + cdData.cacheSize);

         out.flush();
         out.close();

         return true;
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(this, ERR_CACHE_CFG + e.getMessage(), ERROR,
                                       JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         return false;
      }
   }

   private void cancelOperation()
   {
      // ...
      cancelButton_.setEnabled(false);
      cancelRequest_ = true;
      if (fileUtils_ != null)
         fileUtils_.requestCancel();
   }
   
   private void AddLanguageReplacements(HashMap map)
   {
      if (map != null)
      {
         Localizer l = null;
         
         try
         {
            l = new Localizer(
               "/imc/lecturnity/converter/wizard/CdProjectWriterWizardPanel_", "en");
         }
         catch (IOException e)
         {
            // ignore
            return;
         }
         
         String[] aIds = new String[] { "LANG_DATE", "LANG_LENGTH",
            "LANG_AUTHOR", "LANG_PRODUCER", "LANG_KEYWORDS", "LANG_PAGECOUNT",
            "LANG_TITLE", "LANG_DATE_S", "LANG_LENGTH_S", "LANG_PAGECOUNT_S" };

         for (int i=0; i<aIds.length; ++i)
         {
            String sReplacement = l.getLocalized(aIds[i]);
            if (sReplacement != null && sReplacement.length() > 0)
               map.put(aIds[i], sReplacement);
            else
            {
               // ignore, should only happen during development
               
               System.err.println("!!! Could not find replacement/translation for "+aIds[i]);
            }
         }
         
         
      }
   }

   private class ButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         cancelOperation();
      }
   }

   private static class JaiFileFilter implements FileFilter
   {
      public boolean accept(File file)
      {
         return (!(file.getName().toUpperCase().indexOf("JAI") >= 0));
      }
   }

   // accepts all files except "index.html.tmpl" and "doc.html.tmpl"
   private static class NoTemplatesFilter implements FileFilter
   {
      public boolean accept(File file)
      {
         String fileName = file.getName();
         return !(fileName.equalsIgnoreCase("index.html.tmpl") ||
                  fileName.equalsIgnoreCase("doc.html.tmpl"));
      }
   }

   private class FileProgressListener implements ProgressListener
   {
      public void setMaximum(int max)
      {
         fileProgress_.setMaximum(max);
      }

      public void setValue(int value)
      {
         fileProgress_.setValue(value);
      }
   }

}
