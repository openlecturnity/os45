package imc.lecturnity.util;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.*;
import java.util.Enumeration;
import java.util.Properties;
import java.util.StringTokenizer;


public class PropertyHandler extends Properties implements ActionListener
{
   private File propertyLocation_;
   private long lastFileChange_;
   private DelayedTimer reverseTimer_; 
   
   public PropertyHandler(File propsLocation) throws IOException
   {
      if (propsLocation.exists())
      {
         Properties inProps = FileUtils.CreateProperties(propsLocation.getPath());
         super.putAll(inProps);
         
         if (!propsLocation.canWrite())
            throw new IOException("Unable to write to "+propsLocation);
      }
      else
      {
         if (!propsLocation.getParentFile().exists())
         {
            boolean success = propsLocation.getParentFile().mkdirs();
            if (!success)
              throw new IOException("Unable to create needed directory "+
                                    propsLocation.getParentFile());
         }

         boolean success = propsLocation.createNewFile();
         if (!success)
            throw new IOException("Unable to create "+propsLocation);
      }
       
      propertyLocation_ = propsLocation;
      lastFileChange_ = propsLocation.lastModified();

      /*
      Runtime.getRuntime().addShutdownHook(new Thread() {
         public void run()
         {
            if (reverseTimer_ != null)
            {
               doTheHandling(true);
            }
         }
      });
      */
   }

   public String getProperty(String key)
   {
      checkFile();
      return super.getProperty(key);
   }

   public String getProperty(String key, String defaultValue)
   {
      checkFile();
      return super.getProperty(key, defaultValue);
   }

   public String[] getMultiProperty(String key)
   {
      checkFile();
      // double code (PropertyHandler)
      String valueString = super.getProperty(key);
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

   
   public Object setProperty(String key, String value)
   {
      return setProperty(key, value, false);
   }



   public Object setProperty(String key, String value, boolean writeNow)
   {
      Object old = super.setProperty(key, value);

      doTheHandling(writeNow);

      return old;
   }



   public void setMultiProperty(String key, String[] values)
   {
      setMultiProperty(key, values, false);
   }


   public void setMultiProperty(String key, String[] values, boolean writeNow)
   {
      // double code (CacheSettings)
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

         super.setProperty(key, totalValue.toString());
      }

      doTheHandling(writeNow);

   }


   public void syncFile() throws IOException
   {
      PrintWriter out = FileUtils.CreatePrintWriter(propertyLocation_.getPath());
      
      Enumeration enumeration = super.propertyNames();
      while (enumeration.hasMoreElements())
      {
         String strKey = (String)enumeration.nextElement();
         String strValue = super.getProperty(strKey);
         
         out.println(strKey+"="+strValue);
      }
      
      out.close();
   }


   public void actionPerformed(ActionEvent e)
   {
      doTheHandling(true);
   }

   private void doTheHandling(boolean writeNow)
   {
      if (writeNow)
      {
         try
         {
            syncFile();
         }
         catch (IOException exc)
         {
         }

         if (reverseTimer_ != null)
         {
            reverseTimer_.deactivate();
            reverseTimer_ = null;
         }
      }
      else
      {
         if (reverseTimer_ != null)
            reverseTimer_.delayFurther();
         else
            reverseTimer_ = new DelayedTimer(this, 1000, true);
      }
   }


   private void checkFile()
   {
      if (propertyLocation_.exists())
      {
         long change = propertyLocation_.lastModified();
         if (change != lastFileChange_)
         {
            Properties inProps = FileUtils.CreateProperties(propertyLocation_.getPath());
            super.putAll(inProps);
         
            //InputStream in = new BufferedInputStream(new FileInputStream(propertyLocation_));
            //super.load(in);
            //in.close();
            lastFileChange_ = change;
        
         }
      }
   }
}