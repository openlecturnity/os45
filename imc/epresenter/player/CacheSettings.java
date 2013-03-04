package imc.epresenter.player;

import java.io.File;
import java.io.IOException;
import java.util.StringTokenizer;

import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.PropertyHandler;


public class CacheSettings
{
   private File cacheDir_;
   
   private PropertyHandler properties_;


   public CacheSettings(File settingsFile) throws IOException
   {
      if (settingsFile != null)
      {
         properties_ = new PropertyHandler(settingsFile);
      } // else use the registry



      //
      // setup some defaults if they don't exist
      //
      String[] keys = { 
         "notAskOnTooSlow", "notAskOnLow", "cacheMode", "deleteMode",
            "maxCacheSize"
      };
      String [] values = {
         "False", "False", "Selective", "Ask", "512" 
      };
      for (int i=0; i<keys.length; i++)
      {
         if (getProperty(keys[i]) == null)
            setProperty(keys[i], values[i]);
      }

   }

   public void prepareCacheDirectory() throws IOException
   {
      checkAndCreateDirectory();
   }

   public void sync()
   {
      if (properties_ != null)
      {
         try
         {
            properties_.syncFile();
         }
         catch (IOException exc)
         {
         }
      }
   }


   public String getLocation()
   {
      return cacheDir_.toString();
   }

   public File getLocationFile()
   {
      return cacheDir_;
   }

   /**
    * @returns <i>true</i> if the given location denoted another directory
    *          than the previous one
    */
   public boolean setLocation(String location) throws IOException
   {
      return checkAndCreateDirectory(createDirectoryHandle(location)+"");
   }

   public int getMaxSize()
   {
      String sizeString = getProperty("maxCacheSize");
      return Integer.parseInt(sizeString);
   }

   public void setMaxSize(int mbytes) throws IOException
   {
      if (mbytes < 0)
         throw new IllegalArgumentException("Size ("+mbytes+") must be > 0.");
      
      setProperty("maxCacheSize", ""+mbytes);
   }

   public String getProperty(String name)
   {
      if (properties_ != null)
         return properties_.getProperty(name);
      else
         return NativeUtils.getPlayerProperty(name);
   }

   public void setProperty(String name, String value)
   {
      if (properties_ != null)
         properties_.setProperty(name, value);
      else
         NativeUtils.setPlayerProperty(name, value);
   }
   
   public String[] getProperties(String name)
   {
      if (properties_ != null)
         return properties_.getMultiProperty(name);
      else
      {
         // double code (PropertyHandler)
         String valueString = getProperty(name);
         if (valueString != null && valueString.length() > 0)
         {
            StringTokenizer tokenizer = new StringTokenizer(valueString, "\t");

            String[] values = new String[tokenizer.countTokens()];

            for (int i=0; i<values.length; i++)
               values[i] = tokenizer.nextToken();

            return values;
         }
         else
            return new String[0];
      }
   }

   public void setProperties(String name, String values[])
   {
      if (properties_ != null)
         properties_.setMultiProperty(name, values);
      else
      {
         // double code (PropertyHandler)
         if (values.length > 0)
         {
            for (int i=0; i<values.length; i++)
               if (values[i].indexOf("\t") > -1)
                  throw new IllegalArgumentException(
                     "Tabulator (\\t) not allowed as part of one of the values.");

            StringBuffer totalValue = new StringBuffer(values[0]);
            for (int i=1; i<values.length; i++)
            {
               totalValue.append("\t");
               totalValue.append(values[i]);
            }

            setProperty(name, totalValue.toString());
         }
      }
   }



   private boolean checkAndCreateDirectory() throws IOException
   {
      String cacheLocation = getProperty("cacheLocation");
      if (cacheLocation == null || cacheLocation.length() == 0)
      {
         cacheLocation = "TEMP";
         setProperty("cacheLocation", cacheLocation);
      }

      return checkAndCreateDirectory(cacheLocation);
   }

   /**
    * @returns <i>true</i> if the given location denoted another directory
    *          than the previous one
    */
   private boolean checkAndCreateDirectory(String cacheLocation) throws IOException
   {
      cacheDir_ = createDirectoryHandle(cacheLocation);
      
      if (!cacheDir_.exists())
      {
         if (!cacheDir_.mkdirs())
            throw new IOException("Could not create cache directory ("
                                  +cacheDir_+").");
      }

      if (cacheDir_.isDirectory())
      {
         /* only works on files...

         if (!cacheDir_.canWrite())
            throw new IOException("Cannot write to cache directory ("
                                  +cacheDir_+").");
                                  */
      }
      else
         throw new IOException("Configured cache location is not a directory ("
                               +cacheDir_+").");

      if (isDirectoryChange(getProperty("cacheLocation"), cacheLocation))
      {
         File oldLocation = createDirectoryHandle(getProperty("cacheLocation"));
         setProperty("cacheLocation", cacheLocation);

         if (oldLocation != null && oldLocation.exists() && oldLocation.isDirectory() && oldLocation.list().length == 0)
            oldLocation.delete();

         return true;
      }

      return false;
   }

   private File createDirectoryHandle(String location)
   {
      if (location.equals("TEMP"))
         location = System.getProperty("java.io.tmpdir")+File.separator+"_Lecturnity-Cache";

      return new File(location);
   }

   private boolean isDirectoryChange(String oldLocation, String newLocation)
   {
      return isDirectoryChange(createDirectoryHandle(oldLocation), 
                               createDirectoryHandle(newLocation));
   }

   private boolean isDirectoryChange(File oldLocation, File newLocation)
   {
      return !oldLocation.equals(newLocation);
   }
}