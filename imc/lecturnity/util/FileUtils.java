package imc.lecturnity.util;

import java.io.*;
import javax.swing.ProgressMonitorInputStream;

import javax.swing.JOptionPane;
import javax.swing.JDialog;
import javax.swing.filechooser.FileFilter;

import java.awt.Component;

import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Properties;
import java.util.StringTokenizer;

import imc.epresenter.filesdk.util.Localizer;

public class FileUtils
{
   private static String CONFIRM = "[!]";
   private static String WARNING = "[!]";
   private static String ERROR = "[!]";
   private static String EXISTS = "[!]";
   private static String EXISTS2 = "[!]";
   private static String WRITEFAIL = "[!]";
   
   private static String YES = "[!]";
   private static String YESALL = "[!]";
   private static String NO = "[!]";
   private static String CANCEL = "[!]";

   private static String REPLACE_CAP = "[!]";
   private static String REPLACE = "[!]";
   private static String REPLACE2 = "[!]";
   private static String REPLACE3 = "[!]";
   private static String CHANGED = "[!]";

   private static String WARN_ENCODING = "[!]";
   private static String ERR_NO_DIR = "[!]";
   private static String ERR_NOT_DIR = "[!]";
   private static String ERR_COPY = "[!]";

   private static String DIR_NOEXIST1 = "[!]";
   private static String DIR_NOEXIST2 = "[!]";
   private static String DIR_NODIR = "[!]";
   private static String DIR_CREATEERR = "[!]";

   private static String CODEPAGE_MISMATCH = "[!]";
   private static String CODEPAGE_SYSTEM = "[!]";
   private static String CODEPAGE_STANDARDS = "[!]";
   private static String CODEPAGE_QUESTION = "[!]";
   private static String CODEPAGE_INTL_ERROR = "[!]";

   static
   {
      // initialize some constants...
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/util/FileUtils_",
             "en");

         CONFIRM = l.getLocalized("CONFIRM");
         WARNING = l.getLocalized("WARNING");
         ERROR = l.getLocalized("ERROR");
         EXISTS = l.getLocalized("EXISTS");
         EXISTS2 = l.getLocalized("EXISTS2");
         WRITEFAIL = l.getLocalized("WRITEFAIL");

         YES = l.getLocalized("YES");
         YESALL = l.getLocalized("YESALL");
         NO = l.getLocalized("NO");
         CANCEL = l.getLocalized("CANCEL");

         REPLACE_CAP = l.getLocalized("REPLACE_CAP");
         REPLACE = l.getLocalized("REPLACE");
         REPLACE2 = l.getLocalized("REPLACE2");
         REPLACE3 = l.getLocalized("REPLACE3");
         CHANGED = l.getLocalized("CHANGED");

         WARN_ENCODING = l.getLocalized("WARN_ENCODING");
         ERR_NO_DIR = l.getLocalized("ERR_NO_DIR");
         ERR_NOT_DIR = l.getLocalized("ERR_NOT_DIR");
         ERR_COPY = l.getLocalized("ERR_COPY");

         DIR_NOEXIST1 = l.getLocalized("DIR_NOEXIST1");
         DIR_NOEXIST2 = l.getLocalized("DIR_NOEXIST2");
         DIR_NODIR = l.getLocalized("DIR_NODIR");
         DIR_CREATEERR = l.getLocalized("DIR_CREATEERR");

         CODEPAGE_MISMATCH = l.getLocalized("CODEPAGE_MISMATCH");
         CODEPAGE_SYSTEM = l.getLocalized("CODEPAGE_SYSTEM");
         CODEPAGE_STANDARDS = l.getLocalized("CODEPAGE_STANDARDS");
         CODEPAGE_QUESTION = l.getLocalized("CODEPAGE_QUESTION");
         CODEPAGE_INTL_ERROR = l.getLocalized("CODEPAGE_INTL_ERROR");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "FileUtils\n" + 
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         System.exit(1);
      }
   }

   /**
    * The buffer size in used for the copy actions. Defaults to
    * <tt>65536</tt> bytes (64 kB).
    */
   public static int BUFFER_SIZE = 65536;
   private static boolean g_bEncodingMismatch = false;
   static
   {
      // truly static setting, neither of the two variables changes 
      // between different documents
      
      String strEncoding = System.getProperty("file.encoding", "Cp1252");
      String strCodepage = "Cp1252";
      if (NativeUtils.isLibraryLoaded())
         strCodepage = "Cp"+NativeUtils.getSystemCodepage();
   
      g_bEncodingMismatch = !strEncoding.equals(strCodepage);
   }
   
   private static boolean s_bWasUtf8Reader = false;

   private Component parentComponent_;
   private boolean cancelRequest_ = false;
   private boolean copyAll_ = false;
   private int countCopiedFiles_ = 0;

   public FileUtils(Component parent)
   {
      parentComponent_ = parent;
      cancelRequest_ = false;
   }

   public static void copyFile(File src, File dest,
                               Component parentComponent, String message)
      throws IOException
   {
      InputStream in = new FileInputStream(src);
      OutputStream out = new FileOutputStream(dest);
      copyFile(in, out, parentComponent, message);
      out.flush();
      out.close();
      in.close();
      dest.setLastModified(src.lastModified());
   }

   public static void copyFile(String src, String dest, 
                               Component parentComponent, String message)
      throws IOException
   {
      copyFile(new File(src), new File(dest), parentComponent, message);
   }
   
   /**
    * Copies the whole <tt>InputStream</tt> from <tt>src</tt> to the
    * <tt>OutputStream</tt> in <tt>out</tt>. The streams will be
    * buffered, and a progress monitor with <tt>message</tt> will be
    * displayed if the action takes a lot of time.
    *
    * <p>The <tt>OutputStream</tt> and the <tt>InputStream</tt> are
    * <i>not</i> closed and flushed after copying has finished.</p>
    *
    * @param parentComponent the parent component the progress dialog
    * will belong to (e.g. a <tt>Frame</tt> object).
    */
   public static void copyFile(InputStream src, OutputStream dest,
                               Component parentComponent, String message)
      throws IOException
   {
      InputStream in = new BufferedInputStream
         (new ProgressMonitorInputStream(parentComponent, message,
                                         src));
      BufferedOutputStream out = new BufferedOutputStream(dest);

      byte[] buffer = new byte[BUFFER_SIZE];

      int bytesRead = 0;
      while (bytesRead != -1)
      {
         bytesRead = in.read(buffer);
         if (bytesRead != -1)
         {
            out.write(buffer, 0, bytesRead);
         }
      }

      out.flush();
   }

   /**
    * This method creates a custom <tt>FileFilter</tt> with the given
    * description string which accepts files with the extension
    * <tt>extension</tt> or directories.
    */
   public static FileFilter createFileFilter(String description, String extension)
   {
      return new CustomFileFilter(description, extension);
   }

   /**
    * Returns <i>true</i> if it is allowed to write to this file name,
    * <i>false</i> otherwise. If the file under this file name exists,
    * a dialog is popped up, asking if it should be overwritten.
    * <p>If this method returns <i>true</i>, you can be <i>sure</i>
    * that it is ok just to write to the file. The user has eventually
    * confirmed that he wants to overwrite the file. Additionally, you
    * can be sure that you are <i>allowed</i> to write to the file name
    * specified.
    *
    * @param parent the parent AWT component the popup dialogs
    * eventually belong to
    * @param fileName the name of the file to check if it's writable
    */
   public static boolean isWritable(Component parent, String fileName)
   {
      boolean isVerified = false;
      
      File fileNameFile = new File(fileName);
      if (fileNameFile.exists())
      {
         int option = JOptionPane.showConfirmDialog
            (parent, EXISTS + fileName + EXISTS2, CONFIRM, 
             JOptionPane.YES_NO_OPTION);

         if (option == JOptionPane.YES_OPTION)
         {
            if (testWrite(fileName))
            {
               isVerified = true;
            }
            else
            {
               JOptionPane.showMessageDialog(parent, WRITEFAIL, ERROR, 
                                             JOptionPane.ERROR_MESSAGE);
               isVerified = false;
            }
         }
      }
      else
      {
         if (testWrite(fileName))
         {
            isVerified = true;
         }
         else
         {
            JOptionPane.showMessageDialog(parent, WRITEFAIL, ERROR,
                                          JOptionPane.ERROR_MESSAGE);
            isVerified = false;
         }
      }
      
      return isVerified;
   }

   public static boolean doesTargetFileExist(String pathFileName)
   {
      File fileNameFile = new File(pathFileName);
      if (fileNameFile.exists())
         return true;
      else
         return false;
   }

   private static boolean testWrite(String targetFileName)
   {
      try
      {
         String tempTarget = targetFileName + ".temp~";

         PrintWriter out = new PrintWriter(new FileWriter(tempTarget));
         out.println("test");
         out.flush();
         out.close();
         
         File tempTargetFile = new File(tempTarget);
         tempTargetFile.delete();

         return true;
      }
      catch (Exception e)
      {
         return false;
      }
   }

   /**
    * Use this method if you want to check a directory for
    * existance. This method will perform the following actions: If
    * the directory exists and is truly a directory, the method will
    * return <i>true</i>. If the path identifier does not exist, the
    * user is asked if he wants to create the directory. If the user
    * answers "no", the method returns <i>false</i>. Otherwise, and if
    * the creation of the directory(ies) is successful, it returns
    * <i>true</i>. In all other cases (error,...), <i>false</i> is
    * returned.
    */
   public static boolean checkDirectory(Component parent, String dirName)
   {
      File dir = new File(dirName);
      if (dir.exists())
      {
         if (dir.isDirectory())
            return true;

         JOptionPane.showMessageDialog(parent, DIR_NODIR, ERROR,
                                       JOptionPane.ERROR_MESSAGE);
         return false;
      }
      else
      {
         int result = JOptionPane.showConfirmDialog
            (parent, DIR_NOEXIST1 + dirName + DIR_NOEXIST2, CONFIRM,
             JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);

         if (result == JOptionPane.YES_OPTION)
         {
            boolean mkdirOk = false;
            try
            {
               mkdirOk = dir.mkdirs();
            }
            catch (Exception e)
            {
               mkdirOk = false;
            }

            if (!mkdirOk)
               JOptionPane.showMessageDialog
                  (parent, DIR_CREATEERR + dirName, ERROR,
                   JOptionPane.ERROR_MESSAGE);

            return mkdirOk;
         }
         else // NO_OPTION or CLOSED_OPTION
         {
            return false;
         }
      }
   }

   public static boolean createDir(String dirName)
   {
      File dir = new File(dirName);
      if (dir.exists())
      {
         if (dir.isDirectory())
            return true;
         return false;
      }
      else
      {
         return dir.mkdirs();
      }
   }

   /**
    * This method returns true if the selected directory is not empty.
    * In all other cases this method returns false. 
    * (Returns false even in the case that the directory does not exist 
    * or dirName is not a directory.)
    *
    * @param dirName the directory name to be checked.
    */
   public static boolean isNotEmptyDir(String dirName)
   {
      File dir = new File(dirName);
      if (dir.exists() && dir.isDirectory())
      {
         String [] astrList = dir.list();
         if (astrList.length > 0)
            return true;
         else
            return false;
      }
      else
         return false;
   }

   /**
    * This method returns true if a direcory or file exists with the selected filename, 
    * otherwise false is returned.
    *
    * @param strDirOrFileName the directory or file name to be checked.
    */
   public static boolean checkTargetExists(String strDirOrFileName)
   {
      File filename = new File(strDirOrFileName);
      if (filename.exists())
         return true;
      else
         return false;
   }

   /**
    * This method returns 1 if the selected direcory or file does not exist.
    * If the selected directory or file name exists then an underscore and a 
    * consecutive number (beginning with 2) is added to the directory or file 
    * name and rechecked again. 
    * The index of the next non-existing directory or file name is then returned.
    *
    * @param strDirOrFileName the directory or file name to be checked. 
    *    If strDirOrFileName is a file then it must be specified without suffix!
    * @param strSuffix the suffix (incl. the dot, e.g. ".xyz") of the filename to be checked.
    *    If strSuffix is not specified or an empty String then it is assumed that 
    *    strDirOrFileName is a directory name.
    * @param strExtension the extionsion which should be added to the directory/file name (e.g. "_l").
    */
   public static int getIndexOfNextEmptyDirOrFileName(String strDirOrFileName, String strSuffix, 
                                                      String strExtension)
   {
      int idx = 1;
      int idxMax = 500;
      String strNextDirOrFileName = strDirOrFileName + strSuffix;
      
      if (!checkTargetExists(strNextDirOrFileName))
         return idx;

      boolean bIsDirectory = strSuffix.equals("");

      if (bIsDirectory)
      {
         // Check for Separator at the and of the String and remove it
         int nLen = strDirOrFileName.length();
         if (strDirOrFileName.endsWith(File.separator))
            strDirOrFileName = strDirOrFileName.substring(0, nLen-1);
      }

      boolean bAlreadyExists = true;
      do
      {
         idx++;
         strNextDirOrFileName = strDirOrFileName + strExtension + idx + strSuffix;
         bAlreadyExists = checkTargetExists(strNextDirOrFileName);
      }
      while(bAlreadyExists && (idx <= idxMax));

      if (idx >= idxMax)
      {
         // There must be an error: the above loop seems to be endless
         return -1;
      }

      return idx;
   }

   public static int getIndexOfNextEmptyDirName(String strDirName, String strExtension)
   {
      return getIndexOfNextEmptyDirOrFileName(strDirName, "", strExtension);
   }

   public static int getIndexOfNextEmptyFileName(String strFileName, String strExtension)
   {
      String strSuffix = "";
      int nLastDot = strFileName.lastIndexOf(".");
      if (nLastDot > 0)
      {
         strSuffix = strFileName.substring(nLastDot);
         strFileName = strFileName.substring(0, nLastDot);
      }

      return getIndexOfNextEmptyDirOrFileName(strFileName, strSuffix, strExtension);
   }

   /**
    * This method returns the file name without path and without any extension
    *
    * @param strPathFile the file name including path, file name and file extension
    */
   public static String extractNameFromPathFile(String strPathFile)
   {
      File fileName = new File (strPathFile);
      String strippedName = fileName.getName();
      
      int lastDot = strippedName.lastIndexOf('.');
      if (lastDot > 0)
         strippedName = strippedName.substring(0, lastDot);
      
      return strippedName;
   }

   /**
    * This method returns the path from a given String with path and file name.
    * The returned path ends with a Separator.
    *
    * @param strPathFile the file name including path, file name and file extension
    */
   public static String extractPathFromPathFile(String strPathFile)
   {
      String strPath = "";
      String fileName = new File(strPathFile) + "";
      int lastSeparator = fileName.lastIndexOf(File.separatorChar);
      if (lastSeparator >= 0)
         strPath= fileName.substring(0, lastSeparator + 1);
      else
         strPath = "." + File.separatorChar;

      return strPath;
   }

   /**
    * This method returns the size in Kilobytes (KB) of a given file list.
    *
    * @param aFileList a String Array with the file list
    */
   public static int getSizeKbOfFileList(String[] aFileList)
   {
      int iSumSizeKb = 0;

      for (int i = 0; i < aFileList.length; ++i)
      {
         File f = new File(aFileList[i]);
         if (f.exists())
         {
            int iSizeKb = (int)(f.length() / 1024);
            if (iSizeKb < 1)
               iSizeKb = 1;
            iSumSizeKb += iSizeKb;
         }
      }

      return iSumSizeKb;
   }
   
   /**
    * This method returns true if the file name (the argument has to
    * be the file name only!) is URL-compliant, i.e. it contains only
    * alphanumerical and numerical characters (a-z, A-Z, 0-9),
    * underscores, dashes and dots (_, -, .).
    *
    * @param urlName the file name of the file to check for URL
    * compliance
    */
   public static boolean checkUrlFileName(String urlName)
   {
      char c;
      
      boolean allOk = true;

      for (int i=0; i<urlName.length(); ++i)
      {
         c = urlName.charAt(i);

         allOk = allOk && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                           (c >= '0' && c <= '9') || 
                           (c == '_') || (c == '-') || (c == '.'));
      }

      return allOk;
   }
   
   /**
    * This method returns true if the file name (the argument has to
    * be the file name only!) is URL-compliant, i.e. it contains only
    * alphanumerical and numerical characters (a-z, A-Z, 0-9),
    * underscores, dashes and dots (_, -, .). 
    * Additionally blank (or whitespace) is allowed.
    *
    * @param urlName the file name of the file to check for URL
    * compliance
    */
   public static boolean checkUrlFileNameAndAllowBlanks(String urlName)
   {
      char c;
      
      boolean allOk = true;

      for (int i=0; i<urlName.length(); ++i)
      {
         c = urlName.charAt(i);

         allOk = allOk && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                           (c >= '0' && c <= '9') || (c == ' ') ||
                           (c == '_') || (c == '-') || (c == '.'));
      }

      return allOk;
   }
   
   /**
    * This method returns true if the path (with or without the file name) is 
    * URL-compliant (RFC 1630, RFC 3986), i.e. it contains only alphanumerical 
    * and numerical characters ("alpha": a-z, A-Z; "digit": 0-9), 
    * "unreserved" ("-" / "." / "_" / "~" and ":" / "@"), 
    * "sub-delims" ("!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="), 
    * "escape" characters ("%" hex hex) - where "hex" is 0-9, a-f, A-F.
    * For paths also Slash "/" and Backslash "\" should be allowed.
    *
    * @param urlPath the path to check for URL compliance
    */
   public static boolean checkUrlPathName(String urlPath)
   {
      char c;
      
      boolean allOk = true;

      for (int i=0; i<urlPath.length(); ++i)
      {
         c = urlPath.charAt(i);

         allOk = allOk && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                           (c >= '0' && c <= '9') || 
                           (c == '-') || (c == '.') || (c == '_') || (c == '~') ||
                           (c == ':') || (c == '@') || 
                           (c == '!') || (c == '$') || (c == '&') || (c == '\'') || 
                           (c == '(') || (c == ')') || (c == '*') || (c == '+') || 
                           (c == ',') || (c == ';') || (c == '=') || (c == ' ') ||
                           (c == '%') || (c == '/') || (c == '\\'));

         // "escape" sequence: "%" hex hex
         if (c == '%')
         {
            if (i < (urlPath.length()-2))
            {
               i++;
               char h1 = urlPath.charAt(i);
               i++;
               char h2 = urlPath.charAt(i);
               allOk = allOk && ((h1 >= '0' && h1 <= '9') || 
                                 (h1 >= 'a' && h1 <= 'f') || (h1 >= 'A' && h1 <= 'F'));
               allOk = allOk && ((h2 >= '0' && h2 <= '9') || 
                                 (h2 >= 'a' && h2 <= 'f') || (h2 >= 'A' && h2 <= 'F'));
            }
            else
               allOk = allOk && false;
         }
      }

      return allOk;
   }
   
   private static boolean EqualsEncodings(String strEncoding1, String strEncoding2)
   {
      strEncoding1 = strEncoding1.toLowerCase();
      strEncoding2 = strEncoding2.toLowerCase();
      
      if (strEncoding1.equals(strEncoding2))
         return true;
      else
      {
         if (strEncoding1.startsWith("utf") && strEncoding2.startsWith("utf"))
         {
            // remove the first "-" in the strings
            
            if (strEncoding1.indexOf('-') > -1)
               strEncoding1 = strEncoding1.substring(0, strEncoding1.indexOf('-')) + strEncoding1.substring(strEncoding2.indexOf('-') + 1);
            if (strEncoding2.indexOf('-') > -1)
               strEncoding2 = strEncoding2.substring(0, strEncoding2.indexOf('-')) + strEncoding2.substring(strEncoding2.indexOf('-') + 1);
            
            if (strEncoding1.equals(strEncoding2))
               return true;
         }
      }
      
      return false;
   }
   
   private static boolean IsUtf8Encoding(String strEncoding)
   {
      strEncoding = strEncoding.toLowerCase();
      
      if (strEncoding.equals("utf8") || strEncoding.equals("utf-8"))
         return true;
      else
         return false;
   }
   
   public static BufferedReader createBufferedReader(InputStream inputStream, String strCodepage)
      throws UnsupportedEncodingException
   {
      return createBufferedReader(inputStream, strCodepage, false, null);
   }
   
   /**
    * @returns a BufferedReader from the given InputStream. Additionally if
    *          a codepage string is given the Reader does the corresponding conversion;
    *          if no codepage is given the default one (Windows) or 1252 (Linux)
    *          is used.
    *          If the given encoding is not supported and a parent component is given
    *          a warning message will be displayed.
    */ 
   public static BufferedReader createBufferedReader(
      InputStream inputStream, String strCodepage, boolean bShowWarning, Component parentComponent)
   throws UnsupportedEncodingException
   {
      s_bWasUtf8Reader = false;
      
      InputStream inInternal = inputStream;
      try
      {
         // if the file has an utf-8 bom then strCodepage is overwritten
         //
         BufferedInputStream bis = new BufferedInputStream(inputStream);
         bis.mark(3);
         
         byte[] aBom = new byte[3];
         int iRead = bis.read(aBom);
         
         if (iRead > 0)
         {
            if (iRead == 3 && (aBom[0] & 0xff) == 0xef && (aBom[1] & 0xff) == 0xbb && (aBom[2] & 0xff) == 0xbf)
               strCodepage = "utf-8";
            else
               bis.reset();
         }
         
         inInternal = bis;
      }
      catch (IOException exc)
      {
         exc.printStackTrace();
         // severe error, but it will also happen elsewhere and be handled there
      }
     
    
      InputStreamReader isr = null;
      try
      {
         if (strCodepage != null && (strCodepage.startsWith("Cp") || strCodepage.startsWith("utf") || strCodepage.startsWith("UTF")))
         {
            isr = new InputStreamReader(inInternal, strCodepage);
         }
         else
         {
            // prior to 1.7.0.p3 there is no CODEPAGE tag
            boolean bIsWindows = File.separator.equals("\\");
            
            if (bIsWindows)
               isr = new InputStreamReader(inInternal); 
            else
               isr = new InputStreamReader(inInternal, "Cp1252"); // otherwise defaults to "utf-8" with linux
         }
      }
      catch (UnsupportedEncodingException exc)
      {
         exc.printStackTrace();
         isr = new InputStreamReader(inInternal, "ISO8859_1");
      }
      
      if (strCodepage != null && bShowWarning)
      {
         String strEncoding = isr.getEncoding();
         if (!EqualsEncodings(strEncoding, strCodepage))
         {
            JOptionPane.showMessageDialog(
               parentComponent, WARN_ENCODING,
               WARNING, JOptionPane.WARNING_MESSAGE);
         }
      }
      
      if (IsUtf8Encoding(isr.getEncoding()))
         s_bWasUtf8Reader = true;
      else
         s_bWasUtf8Reader = false;
      
      return new BufferedReader(isr);
   }
   
   public static boolean WasUtf8Reader()
   {
      return s_bWasUtf8Reader;
   }
   
   public static Properties CreateProperties(String strFileName)
   {
      File fileProperties = new File(strFileName);
         
      if (fileProperties.exists() && fileProperties.isFile())
      {
         try
         {
            FileInputStream fis = new FileInputStream(fileProperties);
            Properties props = CreateProperties(fis);
            fis.close();
            return props;
         }
         catch (IOException exc)
         {
            exc.printStackTrace();
         }
      }
      
      return new Properties();
   }
   
   public static Properties CreateProperties(InputStream inStream)
   {
      Properties props = new Properties();
      
      if (inStream != null)
      {
         try
         {
            // auto-detects utf-8
            BufferedReader reader = createBufferedReader(inStream, null);
            
            String strLine = reader.readLine();
            while (strLine != null)
            {
               strLine = strLine.trim();
               if (strLine.length() > 0 && !strLine.startsWith("#"))
               {
                  int idx = strLine.indexOf('=');
                  if (idx > 0)
                  {
                     String strIdent = strLine.substring(0, idx);
                     String strValue = strLine.substring(idx+1);
                  
                     strIdent = strIdent.trim();
                     strValue = strValue.trim();
                     
                     if (strValue.indexOf('\\') > -1)
                     {
                        // filter out escape characters and replace \ and n with a true \n
                        
                        int iLength = strValue.length();
                        StringBuffer buffer = new StringBuffer(iLength);
                        boolean bEscapeActive = false;
                        for (int i=0; i<iLength; ++i)
                        {
                           char c = strValue.charAt(i);
                           if (c == '\\' && !bEscapeActive)
                           {
                              if (i+1 < iLength && strValue.charAt(i+1) == 'n')
                              {
                                 buffer.append('\n');
                                 ++i; // two characters processed
                              }
                              else
                                 bEscapeActive = true;
                           }
                           else
                           {
                              buffer.append(c);
                              bEscapeActive = false;
                           }
                        }
                        strValue = buffer.toString();
                     }
                           
                     props.put(strIdent, strValue);
                  }
               }
               // else a comment or empty line
               
               strLine = reader.readLine();
            }
         }
         catch (Exception exc)
         {
            exc.printStackTrace();
         }
      }
      
      return props;
   }
   
   public static PrintWriter CreatePrintWriter(String strFileName) throws IOException
   {
      OutputStream outStream = new BufferedOutputStream(new FileOutputStream(strFileName));
      outStream.write(new byte[] { (byte)0xef, (byte)0xbb, (byte)0xbf }); // utf-8 BOM
         
      PrintWriter out = new PrintWriter(new OutputStreamWriter(outStream, "utf-8"));
      
      return out;
   }
   
   /**
    * @returns true if all of the characters of the string are digits
    */
   public static boolean isInteger(String str)
   {
      boolean bIsNumber = true;
      
      for (int i=0; bIsNumber && i<str.length(); ++i)
         bIsNumber = Character.isDigit(str.charAt(i));
      
      return bIsNumber;
   }
   
   public static boolean isCodepageMismatch()
   {
      return isCodepageMismatch(null);
   }

   /**
    * Checks if the current system has a codepage mismatch:
    * the system codepage does not match the setting for
    * standards and formats in the Windows 'intl.cpl' settings dialog.
    *
    * Addition 2.0.p2: This is only interesting when Readers are opened
    * with the standard encoding (no encoding given in constructor).
    * It also affects in some way the file access. But only on
    * older java versions (<1.3.1_07?).
    * Both are not valid anymore with 2.0.p2: So don't use.
    *
    * @returns true if the 'intl.cpl' dialog was called or if 'cancel'
    *          was pressed.
    */
   public static boolean checkCodepageAndStandards(Component parentComponent)
   {
      if (g_bEncodingMismatch && NativeUtils.isLibraryLoaded())
      {
         String systemCp = "Cp" + NativeUtils.getSystemCodepage();
         String javaCp = System.getProperty("file.encoding", "Cp1252");
         int res = JOptionPane.showConfirmDialog(parentComponent,
                                                 CODEPAGE_MISMATCH + 
                                                 CODEPAGE_SYSTEM + systemCp + "\n" + 
                                                 CODEPAGE_STANDARDS + javaCp + "\n\n" +
                                                 CODEPAGE_QUESTION,
                                                 WARNING,
                                                 JOptionPane.YES_NO_CANCEL_OPTION,
                                                 JOptionPane.WARNING_MESSAGE);
         if (res == JOptionPane.YES_OPTION)
         {
            try
            {
               Runtime.getRuntime().exec(new String[] {"rundll32.exe", 
                                                       "shell32.dll,Control_RunDLL", 
                                                       "intl.cpl,,0"});
               return true;
            }
            catch (IOException e)
            {
               JOptionPane.showMessageDialog(parentComponent,
                                             CODEPAGE_INTL_ERROR,
                                             ERROR,
                                             JOptionPane.ERROR_MESSAGE);
               e.printStackTrace();
            }
         }
         else if (res == JOptionPane.CANCEL_OPTION)
         {
            return true;
         }
      }

      return false;
   }


   /**
    * @returns true if the system codepage does not match the file.encoding
    *          and the given file name contains question marks ('?') or
    *          characters with a value > 127. If no file name is given
    *          the latter check is omitted.
    */
   public static boolean isCodepageMismatch(String strFilename)
   {
      // g_bEncodingMismatch defined statically above
      
      if (strFilename != null)
      {
         boolean bContainsNoneUsCharacters = false;
         for (int i=0; i<strFilename.length(); ++i)
         {
            if (strFilename.charAt(i) > 127 || strFilename.charAt(i) == '?')
            {
               bContainsNoneUsCharacters = true;
               break;
            }
         }
         
         return g_bEncodingMismatch && bContainsNoneUsCharacters;
      }
      else
      {
         return g_bEncodingMismatch;
      }
   }
   
   public static String makeBuggedFilename(String strFilename, String strInputEncoding)
   {
      String strOutputEncoding = System.getProperty("file.encoding", "Cp1252");
      
      
      return translateCodepageString(strFilename, strInputEncoding, strOutputEncoding);
   }
   
   public static String makeUnbuggedFilename(String strFilename, String strOutputEncoding)
   {
      String strInputEncoding = System.getProperty("file.encoding", "Cp1252");
      
      return translateCodepageString(strFilename, strInputEncoding, strOutputEncoding);
   }
   
   /**
    * @returns true if all the characters in the path are contained in 
    *          the current system codepage.
    */
   public static boolean IsCodepageFriendlyPath(String strPath)
   {
      // slightly double code (below)
      
      //String strEncoding = System.getProperty("file.encoding", "Cp1252");
      String strCodepage = "Cp1252";
      if (NativeUtils.isLibraryLoaded())
         strCodepage = "Cp"+NativeUtils.getSystemCodepage();
 
      String strCodepagePath = strPath;
      
      try
      {
         byte[] aCodepageChars = strPath.getBytes(strCodepage);
         strCodepagePath = new String(aCodepageChars, strCodepage);
      }
      catch (UnsupportedEncodingException exc)
      {
         exc.printStackTrace();
         // shouldn't happen; do nothing more
      }
      
      return strPath.equals(strCodepagePath);
   }
   
   /**
    * @returns the characters which are not in the current system codepage
    *          or otherwise problematic (ie. vietnamese combining characters).
    */
   public static String GetCodepageProblemCharacters(String strPath)
   {
      String strCodepage = "Cp1252";
      if (NativeUtils.isLibraryLoaded())
         strCodepage = "Cp"+NativeUtils.getSystemCodepage();
 
      String strCodepagePath = strPath;
      
      try
      {
         byte[] aCodepageChars = strPath.getBytes(strCodepage);
         strCodepagePath = new String(aCodepageChars, strCodepage);
      }
      catch (UnsupportedEncodingException exc)
      {
         exc.printStackTrace();
         // shouldn't happen; do nothing more
      }
      
      String strProblems = "";
      for (int i=0; i<strPath.length(); ++i)
      {
         if (strCodepagePath.charAt(i) != strPath.charAt(i))
            strProblems += strPath.charAt(i) + " ";
      }
      
      return strProblems;
   }
   
   /**
    * Makes two conversions: 1. 16 -> 8 bit characters using the input encoding.
    * 2. 8 -> 16 Bit using the output encoding.
    *
    * @returns the given string encoded with the output encoding. 
    */
   public static String translateCodepageString(String strInput, String strInputEncoding, String strOutputEncoding)
   {
      try
      {
          return new String(strInput.getBytes(strInputEncoding), strOutputEncoding);
      }
      catch (UnsupportedEncodingException exc)
      {
         return strInput;
      }
   }
   
   public static String[] SplitCommandLine(String strCommandLine)
   {
      if (strCommandLine == null || strCommandLine.length() == 0)
         return new String[0];
      
      ArrayList aParameters = new ArrayList(5);
         
      StringTokenizer tokenizer = new StringTokenizer(strCommandLine, " \"", true);
      String strOneParameter = "";
    
      boolean bQuotationActive = false;
      while (tokenizer.hasMoreTokens())
      {
         String strToken = tokenizer.nextToken();
            
         if (strToken.equals(" "))
         {
            if (!bQuotationActive && strOneParameter.length() > 0)
            {
               aParameters.add(strOneParameter);
               strOneParameter = "";
            }
            else
               strOneParameter += strToken;
         }
         else if (strToken.equals("\""))
         {
            bQuotationActive = !bQuotationActive;
         }
         else
         {
            strOneParameter += strToken;
         }
      }
         
      if (strOneParameter.length() > 0)
      {
         aParameters.add(strOneParameter);
         strOneParameter = "";
      }
           
      String[] aSplitted = new String[aParameters.size()];
      for (int i=0; i<aParameters.size(); ++i)
         aSplitted[i] = (String)aParameters.get(i);
         
      return aSplitted;
   }
   

   ///////////////////////////////////////////////////
   //
   //
   //     N O N - S T A T I C   M E T H O D S
   //
   //
   ///////////////////////////////////////////////////

   public void requestCancel()
   {
      cancelRequest_ = true;
   }

   public boolean copyRecursive(File sourceDir, File targetDir,
                                java.io.FileFilter fileFilter,
                                ProgressListener progressListener)
   {
      return copyRecursive(sourceDir, targetDir, fileFilter, 
                           progressListener, false);
   }

   public boolean copyRecursive(File sourceDir, File targetDir,
                                java.io.FileFilter fileFilter,
                                ProgressListener progressListener, 
                                boolean bCopyAll)
   {
      // this is only a wrapper for the recursive method below
      cancelRequest_ = false;
      copyAll_ = bCopyAll;
      if (progressListener != null)
      {
         progressListener.setValue(0);
         int numFiles = countFiles(sourceDir, fileFilter, false);
         System.out.println("copying " + numFiles + " files...");
         progressListener.setMaximum(numFiles);
         countCopiedFiles_ = 0;
      }
      boolean success = copyRecursive0(sourceDir, targetDir, fileFilter,
                                       progressListener);
      return success;
   }

   private boolean copyRecursive0(File sourceDir, File targetDir,
                                  java.io.FileFilter fileFilter,
                                  ProgressListener progressListener)
   {
      if (cancelRequest_)
         return false;
      
      if (!targetDir.exists())
      {
         boolean success = targetDir.mkdirs();
         if (!success)
         {
            JOptionPane.showMessageDialog
               (parentComponent_, ERR_NO_DIR + targetDir, ERROR,
                JOptionPane.ERROR_MESSAGE);
            return false;
         }
      }
      else
      {
         if (!targetDir.isDirectory())
         {
            JOptionPane.showMessageDialog
               (parentComponent_, ERR_NOT_DIR + targetDir, ERROR,
                JOptionPane.ERROR_MESSAGE);
            return false;
         }
      }
      
      File[] files = sourceDir.listFiles(fileFilter);
      if (files != null)
      {
         for (int i=0; i<files.length && !cancelRequest_; i++)
         {
            if (fileFilter.accept(files[i]))
            {
               if (files[i].isDirectory())
               {
                  File newDir = new File(targetDir.getPath() + File.separatorChar + files[i].getName());
               
                  boolean success = copyRecursive0(files[i], newDir, fileFilter,
                                                   progressListener);
                  if (!success) // abort copy!
                     return false;
               }
               else if (files[i].isFile())
               {
                  boolean success = copyFile2
                  (files[i], new File
                   (targetDir.getPath() + File.separatorChar + files[i].getName()),
                   null);
                  if (!success) // abort copy!
                     return false;
               
                  if (progressListener != null)
                     progressListener.setValue(++countCopiedFiles_);
               }
            }
            else
            {
               System.out.println("note: unknown file type of: " + files[i]);
            }
         }
      }

      return !cancelRequest_;
   }

   public boolean copyFile2(File source, File target, 
                            ProgressListener progressListener)
   {
      try
      {
         if (target.exists() && !copyAll_)
         {
            if (source.length() == target.length() &&
                source.lastModified() == target.lastModified())
               return true;
            else
            {
               // file exists and is different! overwrite?
               DateFormat dateFormat = DateFormat.getDateTimeInstance();
               String srcDate = dateFormat.format(new Date(source.lastModified()));
               String trgDate = dateFormat.format(new Date(target.lastModified()));

               System.out.println("source: " + source.lastModified() + " vs. target: " + target.lastModified());
               
               String msg = REPLACE + " \"" + source.getName() + "\"." +
                  REPLACE2 + target.length() + " Bytes\n" +
                  trgDate + REPLACE3 +
                  source.length() + " Bytes\n" +
                  srcDate;

               String options[] = {YES, YESALL, NO, CANCEL};
               JOptionPane optionPane = new JOptionPane(msg, JOptionPane.QUESTION_MESSAGE,
                                                        0, // type, not important
                                                        null, // Icon
                                                        options,
                                                        options[0]);
               JDialog dlg = optionPane.createDialog(parentComponent_, REPLACE_CAP);
               dlg.show();

               String selectedOption = (String) optionPane.getValue();
               if (selectedOption.equals(NO))
                  return true;
               else if (selectedOption.equals(CANCEL))
               {
                  cancelRequest_ = true;
                  return false;
               }
               else if (selectedOption.equals(YESALL))
                  copyAll_ = true;
               
               // else: overwrite
            }
         }

         // ok, let's do it.
         if (progressListener != null)
         {
            progressListener.setValue(0);
            progressListener.setMaximum((int) source.length());
         }

         InputStream in = new BufferedInputStream(new FileInputStream(source));
         OutputStream out = new BufferedOutputStream(new FileOutputStream(target));

         byte[] buffer = new byte[BUFFER_SIZE];

         long lastTime = System.currentTimeMillis();

         int bytesRead = 0;
         int totalRead = 0;
         while (bytesRead != -1 && !cancelRequest_)
         {
            bytesRead = in.read(buffer);
            if (bytesRead != -1)
            {
               out.write(buffer, 0, bytesRead);
               totalRead += bytesRead;
               if (progressListener != null)
               {
                  if (System.currentTimeMillis() - lastTime > 500)
                  {
                     lastTime = System.currentTimeMillis();
                     progressListener.setValue(totalRead);
                  }
               }
            }
         }

         out.flush();
         out.close();
         in.close();

         if (cancelRequest_)
            target.delete();
         else
         {
            if (progressListener != null)
               progressListener.setValue((int) source.length());
         }

         // ensure dates and time are equal:
         long dateTime = source.lastModified();
         try
         {
            target.setLastModified(dateTime);
            source.setLastModified(dateTime);
         }
         catch (Exception e)
         {
            System.err.println("could not modify date time of '" + source + "'.");
         }
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog
            (parentComponent_, ERR_COPY + source + "\n\n" + e.getMessage(),
             ERROR, JOptionPane.ERROR_MESSAGE);
         return false;
      }

      return !cancelRequest_;
   }

   public int countFiles(File directory, java.io.FileFilter fileFilter, 
                         boolean countDirs)
   {
      int count = 0;
      
      if (directory != null && directory.isDirectory())
      {

         File[] files = directory.listFiles(fileFilter);
         for (int i=0; i<files.length; i++)
         {
            if (fileFilter.accept(files[i]))
            {
               if (files[i].isDirectory())
               {
                  if (countDirs)
                     count++;
                  count += countFiles(files[i], fileFilter, countDirs);
               }
               else
                  count++;
            }
         }
      }

      return count;
   }
   
   

   private static class CustomFileFilter extends FileFilter
   {
      private String description_;
      private String extension_;

      public CustomFileFilter(String description, String extension)
      {
         super();

         description_ = description;
         extension_ = extension.toUpperCase();
      }

      public boolean accept(File file)
      {
         if (file.isDirectory())
            return true;

         String fileName = file.getName().toUpperCase();

         return (fileName.endsWith(extension_));
      }

      public String getDescription()
      {
         return description_;
      }
   }
}
