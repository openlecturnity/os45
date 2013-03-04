package imc.lecturnity.util;

/*
 * File:             NativeUtils.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: NativeUtils.java,v 1.56 2010-04-21 07:58:17 kuhn Exp $
 */

import imc.epresenter.filesdk.VideoClipInfo;
import imc.epresenter.filesdk.util.Localizer;
import java.io.File;
import javax.swing.JOptionPane;

/**
 * This class contains some useful methods which cannot be implemented
 * in Java. This class makes use of an external library (DLL).
 */
public class NativeUtils
{
   /** 
    * Constant returned by {@link #getVersionType() getVersionType()}
    * if the Lecturnity version is a full version.
    */
   public static final int FULL_VERSION                  = 0;

   /** 
    * Constant returned by {@link #getVersionType() getVersionType()}
    * if the Lecturnity version is an evaluation version.
    */
   public static final int EVALUATION_VERSION            = 1;

   /** 
    * Constant returned by {@link #getVersionType() getVersionType()}
    * if the Lecturnity version is a university version.
    */
   public static final int UNIVERSITY_VERSION            = 2;

   /** 
    * Constant returned by {@link #getVersionType() getVersionType()}
    * if the Lecturnity version is a university evaluation version.
    */
   public static final int UNIVERSITY_EVALUATION_VERSION = 3;

   /** 
    * Constant returned by {@link #getVersionType() getVersionType()}
    * if the Lecturnity version could not be read.
    */
   public static final int VERSION_ERROR = -1;
   

   /**
    *   WM-Redistributable-Install: 0 = Success, everything was well done.
    */
   public static final int SUCCESS              = 0;
      
   /**
    * WM-Redistributable-Install: -21 = Unable to execute Redistributable Package.
    */
   public static final int NO_REDIST_PACKAGE    = -21;

   /**
    * WM-Redistributable-Install: -22 = The system need a reboot.
    */
   public static final int SYSTEM_NEEDS_REBOOT  = -22;
   
   
   

   private static boolean isLoaded_;
   private static final String DLL_REVISION = "$" + "Revision: 1.55 " + "$";

   static
   {
      try 
      {
         System.loadLibrary("nativeutils");
         isLoaded_ = true;

         String dllRevision = getDllRevision();
         if (!dllRevision.equals(DLL_REVISION))
         {
            javax.swing.JOptionPane.showMessageDialog
               (null,
                "nativeutils.dll does not match NativeUtils.\n" +
                "\"" + dllRevision + "\" (DLL) vs. \"" + DLL_REVISION + "\" (NativeUtils).",
                "Warning",
                javax.swing.JOptionPane.WARNING_MESSAGE);
         }
      }
      catch (UnsatisfiedLinkError e)
      {
         isLoaded_ = false;
      }
   }

   private static final void initStrings()
   {
      // nothing to be done (since 2.0.p3): Recordings and
      // cd projects dir is no longer language dependent

   }

   private static final native String getDllRevision();
   
   /**
    * Returns the environment variable content of the given
    * <tt>variable</tt>.
    */
   public static final native String getEnvVariable(String variable);
   
   /**
    * @returns the number of free bytes on the device "hosting" the given path
    */
   public static final native long getSpaceLeftOnDevice(String path);

   /**
    * Starts a file with its appropriate program (shellExec).
    * @returns the success of this starting operation
    */
   public static final native boolean startFile(String file);

   /**
    * As the name suggests: Queries operating system and some 
    * privileges if the user may modify system files and the like.
    *
    * @returns true if the current user may reign as administrator
    */
   public static final native boolean isUserAdmin();
   
   /**
    * Returns the date the Lecturnity Suite was installed, in the
    * following form: YYMMDD (year, month, date).
    */
   public static final native String getInstallationDate();
   
   /**
    * @returns the number of days the evaluation version is bound to run
    *          after the day of installation
    */
   public static final native int getEvaluationPeriod();

   /**
    * Returns the type of version installed. See constants for an
    * explanation. If the installed version does not match the
    * own version, VERSION_ERROR is returned. The program should
    * not start.
    * @see #FULL_VERSION
    * @see #EVALUATION_VERSION
    * @see #UNIVERSITY_VERSION
    */
   public static final native int getVersionType(String ownVersion);

   public static final int getVersionType()
   {
      throw new Error("Nicht mehr diese Funktion benutzen!");
   }

   /**
    * Creates a version string according to the current registry
    * settings, e.g. "1.6.0.p3.b1 Campus".
    * @param betaRelease the beta version of the calling program
    * @see #getVersionStringShort()
    */
   public static final native String getVersionString(String betaRelease);

   /**
    * Creates a short version string according to the current registry
    * settings, e.g. "1.6.0.p3". Does not contain beta versions or
    * Campus/Enterprise.
    * @see #getVersionString(String)
    */
   public static final native String getVersionStringShort();
   
   /**
    * Creates a short version string according to the current registry
    * settings, e.g. "4.0.p3". Does not contain beta versions or
    * Campus/Enterprise.
    * @see #getVersionString(String)
    */
   public static final native String getVersionStringShortShort();
   
   /**
    * @returns something like "184". The current internal release number.
    */
   public static final native String getVersionRevision();

   /**
    * @returns the expiration date of a full version installation, if
    *    applicable, "" if unbound and <tt>null</tt> if an error
    *    occurred. The format of the date is "YYYYMMDD".
    */
   public static final native String getExpirationDate();

   /**
    * @returns <code>true</code> if the given document is an unlocked cd-file
    *          (meaning an appropriate lecturnity.dat is in the same directory)
    *          and should be replayed without further version and type checks
    */
   public static final native boolean isProtectedDocument(String fullPath);

   /**
    * @returns the two character language code that should be used for all 
    *          labels, messages etc. For use in conjunction e.g. with the
    *          {@link imc.epresenter.filesdk.util.Localizer Localizer}.
    */
   public static final native String getLanguageCode();


   /**
    *  Retrieves the property (value) of the given key from the Registry.
    */
   public static final native String getPlayerProperty(String key);

   /**
    * Puts this property (value) under the given key into the Registry.
    */
   public static final native void setPlayerProperty(String key, String value);

   public static final native String getPublisherProperty(String key);

   public static final native void setPublisherProperty(String key, String value);
   
   
   /**
    * @returns the application data directory. This must (and can) be 
    *          obtained from the Registry (especially on Win98).
    */
   public static final native String getApplicationDataDirectory();

   /**
    * @returns the value from the registry that is found under
    *          subSet\path\key; return value is always String regardless of the
    *          value type (e.g. binary data is also packed into that String);
    *          if path or key does not exist <code>null</code> is returned
    */
   public static final native String getRegistryValue(String subSet, String path, String key);

   /**
    * @returns the parent directory from user home value in the Registry. 
    *          For example "c:\Documents and Settings\<username>\.
    *          Returns the user home itself if there is no parent directory.
    */
   public static final String getUserDir()
   {
      String strUserHome = getUserHome();

      if (strUserHome != null && strUserHome.length() > 0)
      {
         int idx = strUserHome.lastIndexOf('\\');
         
         if (idx > 0)
         {
            return strUserHome.substring(0, idx);
         }
         else
            return strUserHome;
      }
      else
         return null;
   }
   
   /**
    * @returns the user home value from the Registry ("Eigene Dateien" or
    *          "My Documents"). Should work on all Windows version from
    *          Windows 98 and upwards (verified for Windows XP)
    */
   public static final String getUserHome()
   {
      String userHome = getRegistryValue
         ("HKCU", 
          "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
          "Personal");

      return userHome;
   }

   /**
    * @returns (from the registry) the value of the Lecturnity Home directory;
    *          it contains subdirectories for source files and
    *          recordings. If it has not yet been defined, this method
    *          returns the user home path. If that fails, too, the
    *          method returns <tt>null</tt>.
    */
   public static final String getLecturnityHome()
   {
      String lecturnityHome = getRegistryValue("HKCU", 
                                               "Software\\imc AG\\LECTURNITY", 
                                               "LecturnityHome");
      if (lecturnityHome == null)
         lecturnityHome = getUserHome();

      return lecturnityHome;
   }

   /**
    * @returns the path which is assumed to be the recording path of
    *          the Lecturnity Suite. May be <tt>null</tt>.
    */
   public static final String getRecordingsPath()
   {
      initStrings();
      String recordingPath = getLecturnityHome();
      if (recordingPath == null)
         return null;

      recordingPath += File.separator + "Recordings";

      if (!new File(recordingPath).exists())
         recordingPath = getLecturnityHome(); // fallback
      
      return recordingPath;
   }

   /**
    * @returns the path which is assumed to be the cd project path of
    *          the Lecturnity Suite. May be <tt>null</tt>.
    */
   public static final String getCdProjectsPath()
   {
      initStrings();
      String cdProjectsPath = getLecturnityHome();
      if (cdProjectsPath == null)
         return null;

      cdProjectsPath += File.separator + "CD Projects";

      if (!new File(cdProjectsPath).exists())
         cdProjectsPath = getLecturnityHome(); // fallback

      return cdProjectsPath;
   }
   
   
   /**
    * @returns the current number of microseconds since unix epoche;
    *          usefull because System.currentTimeMillis() is way too
    *          unprecise for certain tests
    */
   public static final native long getCurrentMicroseconds();

   /**
    * Fills the videoClipInfo structure with valid video information.
    * @return <i>true</i> if successful, <i>false</i> otherwise.
    */
   public static final native boolean retrieveVideoInfo(String path, VideoClipInfo videoClipInfo);

   /**
    * Retrieves the byte data in 24 Bit BGR format of the first frame in the
    * AVI file.
    * @return a byte array of the pixel data of the first frame
    */
   public static final native byte[] retrieveFirstVideoFrame(String videoFile);

   /**
    * Installs the TWLC codec from the given file location. Note: If TWLC is
    * already installed on the system, calling this method is perhaps not such
    * a good idea: It will change to path to the TWLC DLL in the registry so
    * that any program will find the codec.
    *
    * @return <i>true</i> if successful, <i>false</i> otherwise.
    */
   public static final native boolean installTwlcCodec(String twlcDllFileName);

   /**
    * Unregisters the TWLC codec from the system. Note: If the TWLC codec was
    * installed before, it will not be usable anymore after a call to this
    * method. It removes the registry entry which points to the TWLC DLL.
    *
    * @return <i>true</i> if successful, <i>false</i> otherwise.
    */
   public static final native boolean deinstallTwlcCodec();

   /**
    * Calls the Auto-Update Check of LECTURNITY, if available. If not available,
    * does nothing. Returns 0 if no action has been taken, and a value unequal
    * to 0 if the calling program should exit as soon as possible, probably
    * because an update has been issued.
    */
   public static final native int lecturnityUpdateCheck();

   /**
    * Calls the Auto-Update Configuration of LECTURNITY, if available. 
    * If not available, does nothing. Returns 0 if no action has been
    * taken, and a value unequal to 0 if the calling program should 
    * exit as soon as possible, probably because an update has been issued.
    */
   public static final native int lecturnityUpdateConfigure();

   
    /**
    * 
    */
   public static native int doInstallWmEncoderRedistribution(String sInstallPath, 
                                                             boolean bWaitForCompletion, 
                                                             boolean bIsQuiet);
   /**
    * 
    */
   public static native int doInstallWmFormatRedistribution(String sInstallPath, 
                                                            boolean bWaitForCompletion, 
                                                            boolean bIsQuiet);
   
   /**
    */
   public static native boolean systemNeedsReboot();
   
   /**
    * @return the current system codepage as an integer. E.g., 1252 for Latin-1,
    *         1250 for eastern europe (Latin-2)
    */
   public static native int getSystemCodepage();

   /**
    * @return <i>true</i> if the specified file has a size larger than 2 GB.
    */
   public static native boolean isFileLarger2GB(String fileName);
   
   /**
    * @return <i>true</i> if the specified file has an LEP file and is not exported/finalized.
    */
   public static native boolean checkNeedsFinalization(String strLrdPathName);
   
   /**
    * @return the path of the exported/finalized LRD. Can return the given parameter
    *         (= finalization not needed) or null when encountering a problem.
    */
   public static native String getFinalizedPathName(String strLrdPathName);
   
   /**
    * Sends the given string into a named pipe which is created/read by Powertrainer.
    */
   public static native boolean sendDptMessage(String strMessage);
   
   /**
    * Sends the given string into a named pipe which is created/read by Studio.
    */
   public static native boolean sendPublishMessage(String strMessage);
   
   /**
    * @return true if it is a LIVECONTEXT installation of LECTURNITY.
    *         In this case an error was dislayed and the program should/can be quit.
    */
   public static native boolean failWithLiveContextInstallation(String strFailureText);
   
   /**
    * This method returns <i>true</i> if the native library has been
    * successfully loaded. You <b>must not</b> use the other methods
    * if this method returns <i>false</i>.
    *
    * @return <i>true</i> if the native library is loaded.
    */
   public static final boolean isLibraryLoaded()
   {
      return isLoaded_;
   }
}
