package imc.lecturnity.converter.wizard;

import imc.lecturnity.util.wizards.WizardData;

import java.util.Vector;

public class CdProjectWizardData extends WizardData
{
   public static final int CACHE_SETTINGS_TEMP     = 0;
   public static final int CACHE_SETTINGS_HOME     = 1;
   public static final int CACHE_SETTINGS_REGISTRY = 2;
   public static final int CACHE_WHEN_ALWAYS       = 0;
   public static final int CACHE_WHEN_CD           = 1;
   public static final int CACHE_WHEN_NEVER        = 2;
   public static final int CACHE_WHERE_TEMP        = 0;
   public static final int CACHE_WHERE_CUSTOM      = 1;

   // general settings
   /** The most recently used player setup program */
   public String playerSetup = "";
   /** A list of recent project file names */
   public Vector recentFiles = new Vector();
   /** The most recently used HTML template directory */
   public String lastTemplateDir = "";

   // project data
   /** The name of the project file */
   public String projectFileName = "";
   /** The target directory for the CD project */
   public String targetDir = "";
   /** If set to true, the JRE and a player JAR is copied to the CD */
   public boolean standalonePlayer = true;
   /** If set to true, a player setup program is copied to the CD */
   public boolean copyPlayerSetup = false;
   /** The file name of the player setup program used for this project */
   public String playerSetupProject = "";
   /** A vector of documents for this CD project */
   public Vector projectDocs = new Vector();
   /** The directory for the HTML templates */
   public String templateDir = "";

   // eventual file to add
   /** The file name of a newly converted LPD document */
   public String addThisFile = null;
   /** True if the file in <tt>addThisFile</tt> should be added to the doc list */
   public boolean filePending = false;

   // Cache settings
   /** Where to save the cache settings. One of the CACHE_SETTINGS_* values. */
   public int cacheSettings = CACHE_SETTINGS_TEMP;
   public int cacheSettingsDefault = CACHE_SETTINGS_TEMP;
   /** When to use caching; one of then CACHE_WHEN_* values. */
   public int cacheWhen     = CACHE_WHEN_CD;
   public int cacheWhenDefault = CACHE_WHEN_CD;
   /** Where to place the cache directory. One of the CACHE_WHERE_* values. */
   public int cacheWhere    = CACHE_WHERE_TEMP;
   public int cacheWhereDefault = CACHE_WHERE_TEMP;
   /** If <tt>cacheWhere</tt> is CACHE_WHERE_CUSTOM, then this
    * variable contains the custom path of the cache directory. */
   public String cacheDirectory = "";
   public String cacheDirectoryDefault = "";

   /** The maximum size of the cache directory, in MB (megabytes). */
   public int cacheSize = 512;
   public int cacheSizeDefault = 512;

   public void fillOtherWizardData(WizardData data)
   {
      super.fillOtherWizardData(data);
      CdProjectWizardData d = (CdProjectWizardData) data;
      d.playerSetup        = playerSetup;
      d.recentFiles        = (Vector) recentFiles.clone();
      d.lastTemplateDir    = lastTemplateDir;
      d.projectFileName    = projectFileName;
      d.targetDir          = targetDir;
      d.standalonePlayer   = standalonePlayer;
      d.copyPlayerSetup    = copyPlayerSetup;
      d.playerSetupProject = playerSetupProject;
      d.projectDocs        = (Vector) projectDocs.clone();
      d.templateDir        = templateDir;
      d.addThisFile        = addThisFile;
      d.filePending        = filePending;
      d.cacheSettings      = cacheSettings;
      d.cacheWhen          = cacheWhen;
      d.cacheWhere         = cacheWhere;
      d.cacheDirectory     = cacheDirectory;
      d.cacheSize          = cacheSize;

      d.cacheSettingsDefault  = cacheSettingsDefault;
      d.cacheWhenDefault      = cacheWhenDefault;
      d.cacheWhereDefault     = cacheWhereDefault;
      d.cacheDirectoryDefault = cacheDirectoryDefault;
      d.cacheSizeDefault      = cacheSizeDefault;
   }
}
