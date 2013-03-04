package imc.lecturnity.converter.wizard;

import javax.swing.filechooser.FileSystemView;

import java.util.Vector;

import javax.swing.JOptionPane;

import java.io.*;

import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.WizardData;

import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;

import imc.lecturnity.util.ui.IconListObject;

import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.filesdk.util.XmlParser;
import imc.epresenter.filesdk.util.XmlFormatException;

abstract class AbstractCdProjectWizardPanel extends WizardPanel
{
   /** The file extension of CD projects (.lcp) */
   protected static final String EXTENSION = ".lcp";
   /** The default (localized) caption of the wizard dialog */
   protected static String CAPTION = "[!]";
   /** ´The default (localized) description for CD Project files */
   protected static String LCP_DESC = "[!]";

   /** The default (localized) tooltip text for the "Next" button */
   protected static String TT_NEXT = "[!]";
   /** The default (localized) tooltip text for the "Quit" button */
   protected static String TT_QUIT = "[!]";
   /** The default (localized) tooltip text for the "Cancel" button */
   protected static String TT_CANCEL = "[!]";

   protected static String FILE_NOT_FOUND = "[!]";
   protected static String ERROR = "[!]";
   private static String ERR_WRITE_SETTINGS = "[!]";
   private static String ERR_READ_SETTINGS = "[!]";
   private static String ERR_WRITE_PROJECT = "[!]";
   private static String ERR_READ_PROJECT = "[!]";

   private static String installDir_ = null;

   public static int RECENT_HISTORY = 8;
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/AbstractCdProjectWizardPanel_",
             "en");

         LCP_DESC = l.getLocalized("LCP_DESC");
         CAPTION = l.getLocalized("CAPTION");
         
         FILE_NOT_FOUND = l.getLocalized("FILE_NOT_FOUND");
         ERROR = l.getLocalized("ERROR");
         ERR_WRITE_SETTINGS = l.getLocalized("ERR_WRITE_SETTINGS");
         ERR_READ_SETTINGS = l.getLocalized("ERR_READ_SETTINGS");
         ERR_WRITE_PROJECT = l.getLocalized("ERR_WRITE_PROJECT");
         ERR_READ_PROJECT = l.getLocalized("ERR_READ_PROJECT");
         TT_NEXT = l.getLocalized("TT_NEXT");
         TT_QUIT = l.getLocalized("TT_QUIT");
         TT_CANCEL = l.getLocalized("TT_CANCEL");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   protected CdProjectWizardData cdData;

   public AbstractCdProjectWizardPanel(WizardData data)
   {
      cdData = (CdProjectWizardData) data;
   }

   protected void readSettings()
   {
      //FileSystemView fsv = FileSystemView.getFileSystemView();
      String strHomeDir = NativeUtils.getUserDir();
      File settingsFile = new File
      (strHomeDir + File.separatorChar + "lecturnity_converter.xml");
      
      if (!settingsFile.exists())
         return;

      try
      {
         // will auto-detect and handle utf-8
         XmlParser parser = new XmlParser(new FileInputStream(settingsFile));

         boolean inSettings = false;
         boolean inRecent   = false;
         boolean inProject  = false;
         boolean inPlayerSetup = false;
         boolean inTemplateDir = false;
         boolean inCache    = false;
         boolean inSettingsLocation = false;
         boolean inWhere    = false;
         boolean inWhen     = false;
         boolean inCacheDir = false;
         boolean inCacheSize= false;

         String playerSetup = "";
         String templateDir = "";
         Vector recentFiles = new Vector();
         
         int cacheSettings  = CdProjectWizardData.CACHE_SETTINGS_TEMP;
         int cacheWhen      = CdProjectWizardData.CACHE_WHEN_CD;
         int cacheWhere     = CdProjectWizardData.CACHE_WHERE_TEMP;
         String cacheDir    = "";
         int cacheSize      = 512;

         int ttype = XmlParser.TAG_START;

         while (ttype != XmlParser.EOF)
         {
            ttype = parser.getNextTag();

            switch (ttype)
            {
            case XmlParser.TAG_START: {
               String tag = parser.getToken();

               if (inSettings)
               {
                  if (inRecent)
                  {
                     if (tag.equalsIgnoreCase("project"))
                        inProject = true;
                     else
                        throw new XmlFormatException
                           ("unknown tag in <RECENT>: <" + tag + ">");
                  }
                  else if (inCache)
                  {
                     if (tag.equalsIgnoreCase("settingslocation"))
                        inSettingsLocation = true;
                     else if (tag.equalsIgnoreCase("where"))
                        inWhere = true;
                     else if (tag.equalsIgnoreCase("when"))
                        inWhen = true;
                     else if (tag.equalsIgnoreCase("cachedir"))
                        inCacheDir = true;
                     else if (tag.equalsIgnoreCase("cachesize"))
                        inCacheSize = true;
                  }
                  else if (tag.equalsIgnoreCase("recent"))
                  {
                     inRecent = true;
                  }
                  else if (tag.equalsIgnoreCase("playersetup"))
                  {
                     inPlayerSetup = true;
                  }
                  else if (tag.equalsIgnoreCase("templatedir"))
                  {
                     inTemplateDir = true;
                  }
                  else if (tag.equalsIgnoreCase("cache"))
                  {
                     inCache = true;
                  }
                  else
                     throw new XmlFormatException
                        ("unknown tag in <SETTINGS>: <" + tag + ">");
               }
               else if (tag.equalsIgnoreCase("settings"))
               {
                  inSettings = true;
               }
               else
                  throw new XmlFormatException
                     ("unknown tag: <" + tag + ">");

               break;
            }
            
            case XmlParser.TAG_END: {
               String tag = parser.getToken();

               if (inSettings)
               {
                  if (inPlayerSetup)
                  {
                     if (tag.equalsIgnoreCase("playersetup"))
                        inPlayerSetup = false;
                     else
                        throw new XmlFormatException
                           ("illegal end tag in <PLAYERSETUP>: </" + tag + ">");
                  }
                  else if (inRecent)
                  {
                     if (inProject)
                     {
                        if (tag.equalsIgnoreCase("project"))
                           inProject = false;
                        else
                           throw new XmlFormatException
                              ("illegal end tag in <PROJECT>: </" + tag + ">");
                     }
                     else if (tag.equalsIgnoreCase("recent"))
                     {
                        inRecent = false;
                     }
                     else
                        throw new XmlFormatException
                           ("illegal end tag in <RECENT>: </" + tag + ">");
                  }
                  else if (inCache)
                  {
                     if (inSettingsLocation)
                     {
                        if (tag.equalsIgnoreCase("settingslocation"))
                           inSettingsLocation = false;
                        else
                           throw new XmlFormatException
                              ("illegal end tag in <SETTINGSLOCATION>: </" + tag + ">");
                     }
                     else if (inWhen)
                     {
                        if (tag.equalsIgnoreCase("when"))
                           inWhen = false;
                        else
                           throw new XmlFormatException
                              ("illegal end tag in <WHEN>: </" + tag + ">");
                     }
                     else if (inWhere)
                     {
                        if (tag.equalsIgnoreCase("where"))
                           inWhere = false;
                        else
                           throw new XmlFormatException
                              ("illegal end tag in <WHERE>: </" + tag + ">");
                     }
                     else if (inCacheDir)
                     {
                        if (tag.equalsIgnoreCase("cachedir"))
                           inCacheDir = false;
                        else
                           throw new XmlFormatException
                              ("illegal end tag in <CACHEDIR>: </" + tag + ">");
                     }
                     else if (inCacheSize)
                     {
                        if (tag.equalsIgnoreCase("cachesize"))
                           inCacheSize = false;
                        else
                           throw new XmlFormatException
                              ("illegal end tag in <CACHESIZE>: </" + tag + ">");
                     }
                     else if (tag.equalsIgnoreCase("cache"))
                     {
                        inCache = false;
                     }
                     else
                        throw new XmlFormatException
                           ("illegal end tag in <CACHE>: </" + tag + ">");
                  }
                  else if (inTemplateDir)
                  {
                     if (tag.equalsIgnoreCase("templatedir"))
                        inTemplateDir = false;
                     else
                        throw new XmlFormatException
                           ("illegal end tag in <TEMPLATEDIR>: </" + tag + ">");
                  }
                  else if (tag.equalsIgnoreCase("settings"))
                  {
                     inSettings = false;
                  }
                  else
                     throw new XmlFormatException
                        ("illegal end tag in <SETTINGS>: </" + tag + ">");
               }
               else
                  throw new XmlFormatException
                     ("illegal end tag in <SETTINGS>: </" + tag + ">");

               break;
            }
            
            case XmlParser.CONTENT: {
               String content = parser.getToken();

               if (inSettings)
               {
                  if (inPlayerSetup)
                     playerSetup = content;
                  else if (inRecent)
                  {
                     if (inProject)
                     {
                        recentFiles.addElement(content);
                     }
                     // else ignore
                  }
                  else if (inTemplateDir)
                     templateDir = content;
                  else if (inCache)
                  {
                     if (inSettingsLocation)
                        cacheSettings = Integer.parseInt(content);
                     else if (inWhere)
                        cacheWhere    = Integer.parseInt(content);
                     else if (inWhen)
                        cacheWhen     = Integer.parseInt(content);
                     else if (inCacheDir)
                        cacheDir      = content;
                     else if (inCacheSize)
                        cacheSize     = Integer.parseInt(content);
                  }
                  // else ignore
               }
               // else ignore

               break;
            }

            default:
               break;
               
            }
         }
         
         cdData.recentFiles.removeAllElements();
         for (int i=0; i<recentFiles.size(); i++)
            cdData.recentFiles.addElement(recentFiles.elementAt(i));
         cdData.playerSetup = playerSetup;
         cdData.lastTemplateDir = templateDir;

         if (cacheSize < 0)
            cacheSize = 0;
         if (cacheSize > 2048)
            cacheSize = 2048;

         cdData.cacheSettingsDefault  = cacheSettings;
         cdData.cacheWhereDefault     = cacheWhere;
         cdData.cacheWhenDefault      = cacheWhen;
         cdData.cacheDirectoryDefault = cacheDir;
         cdData.cacheSizeDefault      = cacheSize;
      }
      catch (IOException e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(this, ERR_READ_SETTINGS + e.getMessage(),
                                       ERROR, JOptionPane.ERROR_MESSAGE);
      }
   }

   protected void writeSettings()
   {
      //FileSystemView fsv = FileSystemView.getFileSystemView();
      String strHomeDir = NativeUtils.getUserDir();//fsv.getHomeDirectory();
      File settingsFile = new File
         (strHomeDir + File.separatorChar + "lecturnity_converter.xml");

      try
      {
         PrintWriter out = FileUtils.CreatePrintWriter(settingsFile.getPath());
         
         out.println("<settings>");
         out.println("  <playersetup>" + slashify(cdData.playerSetup) + "</playersetup>");
         out.println("  <templatedir>" + slashify(cdData.lastTemplateDir) + "</templatedir>");
         out.println("  <recent>");
         for (int i=0; i<cdData.recentFiles.size(); i++)
            out.println("    <project>" + 
                        slashify((String) (cdData.recentFiles.elementAt(i))) +
                        "</project>");
         out.println("  </recent>");
         out.println("  <cache>");
         out.println("    <settingslocation>" + cdData.cacheSettings + "</settingslocation>");
         out.println("    <where>" + cdData.cacheWhere + "</where>");
         out.println("    <when>" + cdData.cacheWhen + "</when>");
         out.println("    <cachedir>" + slashify(cdData.cacheDirectory) + "</cachedir>");
         out.println("    <cachesize>" + cdData.cacheSize + "</cachesize>");
         out.println("  </cache>");
         out.println("</settings>");

         out.flush();
         out.close();
      }
      catch (IOException e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(this, ERR_WRITE_SETTINGS + e.getMessage(),
                                       ERROR, JOptionPane.ERROR_MESSAGE);
      }
   }

   protected boolean readProject()
   {
      try
      {
         File projectFile = new File(cdData.projectFileName);
         if (!projectFile.exists())
            throw new FileNotFoundException
               (FILE_NOT_FOUND + " " + cdData.projectFileName);

         // auto-detects utf-8
         XmlParser parser = new XmlParser(new FileInputStream(projectFile));

         int ttype = XmlParser.TAG_START;
         //       cdData.projectDocs.removeAllElements();
         Vector projectDocs = new Vector();
         String targetDir = null;
         String templateDir = "";
         boolean standalonePlayer = true;
         boolean copyPlayerSetup = false;
         String playerSetup = null;

         boolean inCdProject = false;
         boolean inTargetDir = false;
         boolean inTemplateDir = false;
         boolean inStandalonePlayer = false;
         boolean inPlayerSetup = false;
         boolean inCopyPlayerSetup = false;
         boolean inDocs = false;
         boolean inFile = false;

         // caching
         boolean inCache = false;
         boolean inSettingsLocation = false;
         boolean inWhere = false;
         boolean inWhen = false;
         boolean inCacheDir = false;
         boolean inCacheSize = false;

         int cacheSettings  = cdData.cacheSettingsDefault;
         int cacheWhen      = cdData.cacheWhenDefault;
         int cacheWhere     = cdData.cacheWhereDefault;
         String cacheDir    = cdData.cacheDirectoryDefault;
         int cacheSize      = cdData.cacheSizeDefault;
      
         while (ttype != XmlParser.EOF)
         {
            ttype = parser.getNextTag();
            switch (ttype)
            {
            case XmlParser.TAG_START: {
               String tag = parser.getToken();
               if (inCdProject)
               {
                  if (inDocs)
                  {
                     if (tag.equalsIgnoreCase("FILE"))
                     {
                        inFile = true;
                     }
                     else
                        throw new XmlFormatException
                           ("unknown tag in <DOCS>: <" + tag + ">");
                  }
                  else if (inCache)
                  {
                     if (tag.equalsIgnoreCase("settingslocation"))
                        inSettingsLocation = true;
                     else if (tag.equalsIgnoreCase("where"))
                        inWhere = true;
                     else if (tag.equalsIgnoreCase("when"))
                        inWhen = true;
                     else if (tag.equalsIgnoreCase("cachedir"))
                        inCacheDir = true;
                     else if (tag.equalsIgnoreCase("cachesize"))
                        inCacheSize = true;
                  }
                  else if (tag.equalsIgnoreCase("DOCS"))
                  {
                     inDocs = true;
                  }
                  else if (tag.equalsIgnoreCase("TARGETDIR"))
                  {
                     inTargetDir = true;
                  }
                  else if (tag.equalsIgnoreCase("TEMPLATEDIR"))
                  {
                     inTemplateDir = true;
                  }
                  else if (tag.equalsIgnoreCase("STANDALONEPLAYER"))
                  {
                     inStandalonePlayer = true;
                  }
                  else if (tag.equalsIgnoreCase("PLAYERSETUP"))
                  {
                     inPlayerSetup = true;
                  }
                  else if (tag.equalsIgnoreCase("COPYPLAYERSETUP"))
                  {
                     inCopyPlayerSetup = true;
                  }
                  else if (tag.equalsIgnoreCase("cache"))
                  {
                     inCache = true;
                  }
                  else
                     throw new XmlFormatException
                        ("unknown tag in <CDPROJECT>: <" + tag + ">");
               }
               else if (tag.equalsIgnoreCase("CDPROJECT"))
               {
                  inCdProject = true;
               }
               else
                  throw new XmlFormatException("unknown tag: <" + tag + ">");
               break;
            }

            case XmlParser.TAG_END: {
               String tag = parser.getToken();
               if (inCdProject)
               {
                  if (inDocs)
                  {
                     if (inFile)
                     {
                        if (tag.equalsIgnoreCase("FILE"))
                        {
                           inFile = false;
                        }
                        else
                           throw new XmlFormatException
                              ("illegal end tag in <FILE>: </" + tag + ">");
                     }
                     else if (tag.equalsIgnoreCase("DOCS"))
                     {
                        inDocs = false;
                     }
                     else
                        throw new XmlFormatException
                           ("illegal end tag in <DOCS>: </" + tag + ">");
                  }
                  else if (inTargetDir)
                  {
                     if (tag.equalsIgnoreCase("TARGETDIR"))
                     {
                        inTargetDir = false;
                     }
                     else
                        throw new XmlFormatException
                           ("illegal end tag in <TARGETDIR>: </" + tag + ">");
                  }
                  else if (inTemplateDir)
                  {
                     if (tag.equalsIgnoreCase("TEMPLATEDIR"))
                     {
                        inTemplateDir = false;
                     }
                     else
                        throw new XmlFormatException
                           ("illegal end tag in <TEMPLATEDIR>: </" + tag + ">");
                  }
                  else if (inStandalonePlayer)
                  {
                     if (tag.equalsIgnoreCase("STANDALONEPLAYER"))
                     {
                        inStandalonePlayer = false;
                     }
                     else
                        throw new XmlFormatException
                           ("illegal end tag in <STANDALONEPLAYER>: </" + tag + ">");
                  }
                  else if (inPlayerSetup)
                  {
                     if (tag.equalsIgnoreCase("PLAYERSETUP"))
                     {
                        inPlayerSetup = false;
                     }
                     else
                        throw new XmlFormatException
                           ("illegal end tag in <PLAYERSETUP>: </" + tag + ">");
                  }
                  else if (inCopyPlayerSetup)
                  {
                     if (tag.equalsIgnoreCase("COPYPLAYERSETUP"))
                     {
                        inCopyPlayerSetup = false;
                     }
                     else
                        throw new XmlFormatException
                           ("illegal end tag in <COPYPLAYERSETUP>: </" + tag + ">");
                  }
                  else if (inCache)
                  {
                     if (inSettingsLocation)
                     {
                        if (tag.equalsIgnoreCase("settingslocation"))
                           inSettingsLocation = false;
                        else
                           throw new XmlFormatException
                              ("illegal end tag in <SETTINGSLOCATION>: </" + tag + ">");
                     }
                     else if (inWhen)
                     {
                        if (tag.equalsIgnoreCase("when"))
                           inWhen = false;
                        else
                           throw new XmlFormatException
                              ("illegal end tag in <WHEN>: </" + tag + ">");
                     }
                     else if (inWhere)
                     {
                        if (tag.equalsIgnoreCase("where"))
                           inWhere = false;
                        else
                           throw new XmlFormatException
                              ("illegal end tag in <WHERE>: </" + tag + ">");
                     }
                     else if (inCacheDir)
                     {
                        if (tag.equalsIgnoreCase("cachedir"))
                           inCacheDir = false;
                        else
                           throw new XmlFormatException
                              ("illegal end tag in <CACHEDIR>: </" + tag + ">");
                     }
                     else if (inCacheSize)
                     {
                        if (tag.equalsIgnoreCase("cachesize"))
                           inCacheSize = false;
                        else
                           throw new XmlFormatException
                              ("illegal end tag in <CACHESIZE>: </" + tag + ">");
                     }
                     else if (tag.equalsIgnoreCase("cache"))
                     {
                        inCache = false;
                     }
                     else
                        throw new XmlFormatException
                           ("illegal end tag in <CACHE>: </" + tag + ">");
                  }
                  else if (tag.equalsIgnoreCase("CDPROJECT"))
                  {
                     inCdProject = false;
                  }
                  else
                     throw new XmlFormatException
                        ("illegal end tag in <CDPROJECT>: </" + tag + ">");
               }
               else
                  throw new XmlFormatException
                     ("illegal end tag: </" + tag + ">");
               break;
            }

            case XmlParser.CONTENT: {
               String content = parser.getToken();

               if (inCdProject)
               {
                  if (inDocs)
                  {
                     if (inFile)
                     {
                        projectDocs.addElement(new FileListObject(content));
                     }
                  }
                  else if (inTargetDir)
                  {
                     targetDir = content;
                  }
                  else if (inTemplateDir)
                  {
                     templateDir = content;
                  }
                  else if (inPlayerSetup)
                  {
                     playerSetup = content;
                  }
                  else if (inCopyPlayerSetup)
                  {
                     copyPlayerSetup = (new Boolean(content)).booleanValue();
                  }
                  else if (inStandalonePlayer)
                  {
                     standalonePlayer = (new Boolean(content)).booleanValue();
                  }
               }
               break;
            }
         
            default:
               // ignore
               break;
            }
         }

         cdData.targetDir = (targetDir == null) ? "" : targetDir;
         cdData.templateDir = (templateDir == null) ? "" : templateDir;
         cdData.standalonePlayer = standalonePlayer;
         cdData.copyPlayerSetup = copyPlayerSetup;
         cdData.playerSetupProject = (playerSetup == null) ? "" : playerSetup;
         cdData.projectDocs = projectDocs;

         if (cacheSize < 0)
            cacheSize = 0;
         if (cacheSize > 2048)
            cacheSize = 2048;

         cdData.cacheSettings  = cacheSettings;
         cdData.cacheWhere     = cacheWhere;
         cdData.cacheWhen      = cacheWhen;
         cdData.cacheDirectory = cacheDir;
         cdData.cacheSize      = cacheSize;
         cdData.cacheSettingsDefault  = cacheSettings;
         cdData.cacheWhereDefault     = cacheWhere;
         cdData.cacheWhenDefault      = cacheWhen;
         cdData.cacheDirectoryDefault = cacheDir;
         cdData.cacheSizeDefault      = cacheSize;

         return true;
      }
      catch (IOException e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog
            (this, ERR_READ_PROJECT + e.getMessage(), ERROR, 
             JOptionPane.ERROR_MESSAGE);
      }

      return false;
   }

   protected boolean writeProject()
   {
      try
      {
         PrintWriter out = FileUtils.CreatePrintWriter(cdData.projectFileName);
         
         out.println("<cdproject>");
         out.println("  <targetdir>" + slashify(cdData.targetDir) + "</targetdir>");
         out.println("  <templatedir>" + slashify(cdData.templateDir) + "</templatedir>");
         out.println("  <standaloneplayer>" + cdData.standalonePlayer + "</standaloneplayer>");
         out.println("  <playersetup>" + slashify(cdData.playerSetupProject) + "</playersetup>");
         out.println("  <copyplayersetup>"  + cdData.copyPlayerSetup + "</copyplayersetup>");
         out.println("  <docs>");
         for (int i=0; i<cdData.projectDocs.size(); i++)
            out.println("    <file>" + slashify(cdData.projectDocs.elementAt(i).toString()) + 
                        "</file>");
         out.println("  </docs>");
         out.println("  <cache>");
         out.println("    <settingslocation>" + cdData.cacheSettings + "</settingslocation>");
         out.println("    <where>" + cdData.cacheWhere + "</where>");
         out.println("    <when>" + cdData.cacheWhen + "</when>");
         out.println("    <cachedir>" + slashify(cdData.cacheDirectory) + "</cachedir>");
         out.println("    <cachesize>" + cdData.cacheSize + "</cachesize>");
         out.println("  </cache>");
         out.println("</cdproject>");
         out.flush();
         out.close();
         
         return true;
      }
      catch (IOException e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog
            (this, ERR_WRITE_PROJECT + e.getMessage(), ERROR, 
             JOptionPane.ERROR_MESSAGE);
      }

      return false;
   }

   protected void addToRecentFiles(String projectFile)
   {
      Vector recentFiles = new Vector();
      recentFiles.addElement(projectFile);
      for (int i=0; i<cdData.recentFiles.size(); i++)
      {
         if (!((String) (cdData.recentFiles.elementAt(i))).equals(projectFile))
            recentFiles.addElement(cdData.recentFiles.elementAt(i));
      }

      while (recentFiles.size() > RECENT_HISTORY)
         recentFiles.removeElementAt(recentFiles.size() - 1);
      
      cdData.recentFiles = recentFiles;
   }

   protected String slashify(String string)
   {
      if (string == null)
         return "";
      StringBuffer temp = new StringBuffer(string.length() * 2);
      for (int i=0; i<string.length(); i++)
      {
         char c = string.charAt(i);
         switch (c)
         {
         case '\\':
            temp.append("\\\\");
            break;
            
         case '"':
            temp.append("\\\"");
            break;
            
         case '\'':
            temp.append("\\\'");
            break;

         case 13: // throw away
            break;

         case 10:
            temp.append("\\n");
            break;
            
         default:
            temp.append(c);
            break;
         }
      }
      return temp.toString();
   }

   protected long getFileListObjectListSize(Object[] fileList)
   {
      long totalSize = 0;
      for (int i=0; i<fileList.length; i++)
         totalSize += ((FileListObject) fileList[i]).getFileSize();

      return totalSize;
   }

   protected void setInstallDir(String dir)
   {
      if (dir.endsWith(File.separator))
         installDir_ = dir;
      else
         installDir_ = dir + File.separatorChar;
   }

   protected String getInstallDir()
   {
      return installDir_;
   }

   protected class FileListObject implements IconListObject
   {
      private static final String docIconResource = 
         "/imc/lecturnity/converter/images/icon_dok.gif";
      private static final String noDocIconResource = 
         "/imc/lecturnity/converter/images/icon_dok_missing.gif";
      
      private String docName_;
      private long fileSize_;
      private boolean exists_;

      public FileListObject(String docName)
      {
         docName_ = docName;
         refresh();
      }

      public void refresh()
      {
         File file = new File(docName_);
         exists_ = file.exists();
         if (exists_)
            fileSize_ = file.length();
         else
            fileSize_ = 0;
      }

      public long getFileSize()
      {
         return fileSize_;
      }

      public String toString()
      {
         return docName_;
      }

      public String getIconResourceString()
      {
         return (exists_ ? docIconResource : noDocIconResource);
      }
   }
}
