package imc.epresenter.player;
                                                        
import java.awt.Window;
import java.io.*;
import java.net.Authenticator;
import java.net.URL;
import java.net.MalformedURLException;
import java.net.HttpURLConnection;
import java.util.*;
import javax.swing.JComponent;
import javax.swing.JProgressBar;

import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.net.AuthenticationManager;


public class CacheBackend
{
   static boolean DEBUG = false;
   static
   {
      if (System.getProperty("app.configdebug") != null)
         DEBUG = true;
   }

   private GlobalMasterAndUI master_;
   private CacheSettings settings_;
   private Map activeCopys_ = Collections.synchronizedMap(new HashMap());
   private AuthenticationManager authenticator_;

   
   public CacheBackend(GlobalMasterAndUI master) throws IOException
   {
      this(master, null);
   }

   public CacheBackend(GlobalMasterAndUI master, String location) throws IOException
   {
      master_ = master;
      
      String configLocation = null; // default is the registry (null)

      // check the registry for properties
      String cacheLocation = NativeUtils.getPlayerProperty("cacheLocation");

      Properties cdProps = null;

      if (cacheLocation == null || cacheLocation.length() == 0)
      { 
         if (DEBUG)
            System.out.println("cacheLocation not in registry.");

         // no settings in the registry
         // check app data dir and tmp dir for a config file
         String fileName = "_LecturnityPlayer.cfg";

         String appDataDir = NativeUtils.getApplicationDataDirectory();

         File appFile = new File(new File(appDataDir), fileName);
         if (appFile.exists())
         {
            configLocation = appFile+"";
            //System.out.println("app:"+configLocation);
         }
         else
         { 
            String tmpDir = System.getProperty("java.io.tmpdir");
            File tmpFile = new File(new File(tmpDir), fileName);
            if (tmpFile.exists())
               configLocation = tmpFile+"";
            else
            {
               // cache.cfg on a CD ?
               if (location != null)
               {
                  File cfgFile = new File(new File(location).getParentFile(), "cache.cfg");
                  if (cfgFile.exists())
                  {
                     cdProps = FileUtils.CreateProperties(cfgFile.getPath());
                     
                     if (DEBUG && cdProps.size() > 0)
                        System.out.println("Found Cd properties.");

                     String settingsLocation = 
                        cdProps.getProperty("settingsLocation");
                     if (settingsLocation != null)
                     {
                        if (settingsLocation.equals("TEMP"))
                           configLocation = tmpFile+"";
                        else if (settingsLocation.equals("APPDATA"))
                           configLocation = appFile+"";
                     }

                     //in.close();
                  }
               }
            }
         }
      } // cacheLocation not in registry

      if (DEBUG)
      {
         if (configLocation != null && new File(configLocation).exists())
            System.out.println("Found config data in a file = "+configLocation);
         else if (configLocation != null)
            System.out.println("Wanna be config data in a file = "+configLocation);
         else
            System.out.println("Found no config data in a file.");
      }

      settings_ = new CacheSettings(
         configLocation != null ? new File(configLocation) : null);

      if (cdProps != null) // only true when there are no other settings, aka new settings
      {
         if (DEBUG)
            System.out.println("Cd-Properties active.");

         String cacheMode = cdProps.getProperty("cacheMode");
         if (cacheMode != null)
         {
            if (cacheMode.equals("CD"))
            {
               cacheMode = "Selective";
               settings_.setProperty("selectiveCachePaths", 
                                     new File(location).getParent());
               settings_.setProperty("selectiveCacheValues", "Yes"); 

            }
            settings_.setProperty("cacheMode", cacheMode);
         }
         String cacheLocation2 = cdProps.getProperty("cacheLocation");
         if (cacheLocation2 != null 
             && (cacheLocation2.equals("TEMP") || checkAndCreateDirectory(cacheLocation2)))
            settings_.setProperty("cacheLocation", cacheLocation2);
         String maxCacheSize = cdProps.getProperty("maxCacheSize");
         if (maxCacheSize != null)
            settings_.setProperty("maxCacheSize", maxCacheSize);

      }

      if (DEBUG)
         System.out.println("Current cacheLocation="+settings_.getProperty("cacheLocation"));


      authenticator_ = new AuthenticationManager(null);
      Authenticator.setDefault(authenticator_);
   }

   public void prepareCacheDirectory() throws IOException
   {
      settings_.prepareCacheDirectory();
   }

   public CacheSettings getSettings()
   {
      return settings_;
   }

   public GlobalMasterAndUI getMaster()
   {
      return master_;
   }


   public File[] listFiles()
   {
      return settings_.getLocationFile().listFiles(new FileFilter() {
         public boolean accept(File name)
         {
            return name.toString().toLowerCase().endsWith(".lpd");
         }
      });
   }

   /**
    * @returns the new location of the file
    */
   public String copyFile(String sourceLocation, JProgressBar progress) throws IOException
   {
      Window parent = Manager.getLoadWindow();
       
      if (contains(sourceLocation))
         return getFileInCache(sourceLocation).toString();

      InputStream in = null;
      long length = 0;
      String name = null;

      // double code (contains(), getFileInCache())
      if (sourceLocation.toLowerCase().startsWith("http"))
      {
         HttpURLConnection con = null;
         try
         {
            URL url = new URL(sourceLocation);
            name = url.getFile();
            name = name.replace('/', '_');
            name = name.replace('\\', '_');
            authenticator_.reset();
            con = (HttpURLConnection)url.openConnection();

            
            in = con.getInputStream();


            // bug around (CDN always sends always 200 as response code)
            byte[] header = new byte[4];
            int readBytes = in.read(header);
            in.close();
      
            boolean headerCorrect = (header[0] == 'E' &&
                                     header[1] == 'P' &&
                                     header[2] == 'F');

            if (readBytes == 4 && headerCorrect)
            {
               con = (HttpURLConnection)url.openConnection();
               in = con.getInputStream(); // everything seems alright
            }
            else
            {
               // does not exist or something similar
               Manager.showError(parent, Manager.getLocalized("urlNotEpf"),
                                 Manager.ERROR, null);
               return null;
            }
   

         }
         catch (MalformedURLException exc)
         {
            // should not happen (checked before)
         }

         length = con.getContentLength();
         if (length <= 0)
         {
            String lengthS = System.getProperty(sourceLocation);
            if (lengthS != null)
            {
               try
               {
                  length = Long.parseLong(lengthS);
               }
               catch (NumberFormatException exc)
               {
               }
               System.setProperty(sourceLocation, null);
            }
         }


      }
      else
      {
         File sourceFile = new File(sourceLocation);
         in = new FileInputStream(sourceFile);
         length = sourceFile.length();
         name = sourceFile.getName();
         
         if (length > 0)
            name = length+"_"+name;

      }

      
      File targetFile = new File(settings_.getLocationFile(), name);
     
      
      int range = progress.getMaximum()-progress.getMinimum();

      FileOutputStream out = null;
      try
      {
         byte[] buffer = new byte[4096];
         int r = 0;
         activeCopys_.put(sourceLocation, in);
         out = new FileOutputStream(targetFile);

         if (length <= 0)
            progress.setString(Manager.getLocalized("lengthUnknown"));

         long written = 0;
         while((r = in.read(buffer)) > -1)
         {
            //System.out.print(r+" ");

            out.write(buffer, 0, r);
            written += r;
            if (length > 0)
               progress.setValue(progress.getMinimum()+(int)((written/(float)length)*range));
         }
      }
      catch (IOException exc)
      {
         //System.out.println("Exception de IO");
         if (out != null)
            out.close(); // hm! hm!
         targetFile.delete(); // but close() is needed for this to work
         if (activeCopys_.containsKey(sourceLocation))
         {
            activeCopys_.remove(sourceLocation);
            throw exc;
         }
         // else copy was cancelled
      }
      finally
      {
         if (out != null)
            out.close();
         if (in != null)
            in.close();
         if (targetFile.exists() && targetFile.length() == 0)
            targetFile.delete();
         activeCopys_.remove(sourceLocation);
      }

      if (length > 0 && targetFile.exists() && targetFile.length() == length)
         return targetFile.toString();
      else if (length <= 0 && targetFile.exists() && targetFile.length() > 0)
         return targetFile.toString();
      else
         return null;
   }

   public void cancelCopy(String sourceLocation)
   {
      InputStream in = (InputStream)activeCopys_.remove(sourceLocation);
      if (in != null)
      {
         try
         {
            in.close();
         }
         catch (IOException exc)
         {
            // hm, unhandled...
         }
      }
   }



   public void loadFile(String location)
   {
      master_.loadDocument(location);
   }

   public boolean contains(String location)
   {
      // double code (getFileInCache())
      if (location.toLowerCase().startsWith("http"))
      {
         try
         {
            location = new URL(location).getFile();
            location = location.replace('/', '_');
            location = location.replace('\\', '_');
            
         }
         catch (MalformedURLException exc)
         {
            // should not happen (checked before invokation)
         }

         if (location == null || location.length() == 0)
            return false;
         else
            return getFileInCache(location).exists();
      }

      File sourceFile = new File(location);
      if (sourceFile.exists())
      {
         long length = sourceFile.length();
         if (length > 0 && !sourceFile.getName().startsWith(length+""))
            location = new File(sourceFile.getParentFile(), length+"_"+sourceFile.getName())+"";
      }
      File cacheFile = getFileInCache(location);
      //boolean sizeIsAppropriate = ignoreSize || sourceFile.length() == cacheFile.length();
      return cacheFile.exists();
      // hm: different sizes and same name could mean different documents!
   }

   public boolean cacheAndDeviceSufficient(String location)
   {
      long maxCacheSize = 1024L*1024L*settings_.getMaxSize();

      return cacheAndDeviceSufficient(location, maxCacheSize);
   }    

   public boolean cacheAndDeviceSufficient(String location, long maxCacheSize)
   {
      long fileSize = new File(location).length();
      if (maxCacheSize == -1)
         maxCacheSize = 1024L*1024L*settings_.getMaxSize();
      long bytesFreeOnDevice = NativeUtils.getSpaceLeftOnDevice(settings_.getLocation());
      
      return fileSize <= Math.min(bytesFreeOnDevice, maxCacheSize);
   }
   
   public boolean canTake(String location)
   {
      long maxCacheSize = 1024L*1024L*settings_.getMaxSize();
      
      return canTake(location, maxCacheSize);
   }
   
   public boolean canTake(String location, long maxCacheSize)
   {
      if (maxCacheSize == -1)
         maxCacheSize = 1024L*1024L*settings_.getMaxSize();
      
      long bytesInCache = 0;
      File[] list = listFiles();
      for (int i=0; i<list.length; i++)
         bytesInCache += list[i].length();

      
      long bytesFreeInCache = maxCacheSize-bytesInCache;
      
      long fileSize = new File(location).length();

      return fileSize <= bytesFreeInCache;
   }



   public void deleteInCache(String location) throws IOException 
   {
      File f = getFileInCache(location);

      
      int counter = 0;
      do
      {
         f.delete();
         if (f.exists())
            try { Thread.sleep(120); } catch (InterruptedException exc) {}
         // out.close() might not be invoked yet
         else
            break;

         if (counter % 3 == 0)
            System.gc(); // if a (not yet freed) FileInputStream is still open
      }
      while (counter++ < 10);

      
   }


   public File getFileInCache(String location)
   {
      // double code (contains())
      if (location.toLowerCase().startsWith("http"))
      {
         try
         {
            location = new URL(location).getFile();
            location = location.replace('/', '_');
            location = location.replace('\\', '_');
         }
         catch (MalformedURLException exc)
         {
            // should not happen (checked before invokation)
         }
      }

      File localFile = new File(location);
      if (localFile.exists())
      {
         if (!localFile.getParentFile().equals(settings_.getLocationFile()))
         {
            long length = localFile.length();
            if (!localFile.getName().startsWith(length+""))
               location = new File(localFile.getParentFile(), length+"_"+localFile.getName())+"";
         }
         // else file is contained in cache under exactly the give name
         // so no size must be prepended
      }

      File returnFile = null;

      if (location.indexOf(File.separator) > -1)
          returnFile =  new File(settings_.getLocationFile(), new File(location).getName());
      else
          returnFile =  new File(settings_.getLocationFile(), location);

      try { returnFile = returnFile.getCanonicalFile(); } catch (Exception e) {}

      return returnFile;
   }


   private boolean checkAndCreateDirectory(String path)
   {
      File d = new File(path);
      if (d.exists())
         return d.isDirectory();
      else
         return d.mkdirs();
   }

}