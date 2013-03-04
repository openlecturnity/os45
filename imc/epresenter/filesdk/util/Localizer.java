package imc.epresenter.filesdk.util;

/*
 * File:             Localizer.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: Localizer.java,v 1.7 2007-11-29 13:26:53 zoen Exp $
 */

import java.util.Properties;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.IOException;

import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;

/**
 * This class helps to write localized/multilingual applications in
 * the ePresenter Suite.
 *
 * <p><b>Usage:</b> For each language and class/application (depends
 * on how you wish to use it: very class-internal or rather
 * application-wide) you will need an instance of this class:
 * <tt>Localizer</tt>. If you then need a text string in the current
 * language (which you need not know explicitly), you just call the
 * {@link #getLocalized() getLocalized()} method of this class at the
 * appropriate position. It is useful to instanciate the
 * <tt>Localizer</tt> instance once, e.g. in the constructor of the
 * class using the <tt>Localizer</tt> class.</p>
 *
 * <p>The <tt>Localizer</tt> instance will try to read a text file
 * with a given prefix (e.g. the class or application name) followed
 * by the language abbrevation in the <tt>user.language</tt> system
 * property. Then, the suffix <tt>.properties</tt> is added. In each
 * text file, you should put the localized strings in a key=text
 * notation:
 * <pre>
 * OPEN=Ouvrir le fichier
 * CLOSE=Fermer la fenetre
 * ...
 * </pre>
 * <b>Note:</b> The keys are <i>case-sensitive</i>.</p>
 *
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class Localizer 
{
   private static String languageCode_ = "en";

   static
   {
      // read the default language from the Registry through the 
      // nativeutils.dll

      try
      {
         if (NativeUtils.isLibraryLoaded())
         {
            languageCode_ = NativeUtils.getLanguageCode();
         }
         else
         {
            //System.err.println
            //   ("Could not read out Lecturnity Suite language. Assuming \"en\".");
         }
      }
      catch (UnsatisfiedLinkError e)
      {
         //System.err.println
         //   ("Could not read out Lecturnity Suite language. Assuming \"en\".");
      }
   }

   private Properties resources_;

   /**
    * Constructs a new <tt>Localizer</tt> instance with the given
    * parameters.
    * @param fileNamePrefix the file name prefix of the properties
    * text file (see above)
    * @param the default language to choose if the local language is
    * not available (these language properties <b>must</b> be
    * available!)
    */
   public Localizer(String fileNamePrefix, String defaultLanguage)
      throws IOException
   {
      super();
      
      InputStream textResource = null;

      textResource = getClass().getResourceAsStream
         (fileNamePrefix + languageCode_ + ".properties");

      if (textResource == null)
         textResource = getClass().getResourceAsStream
            (fileNamePrefix + defaultLanguage + ".properties");
      
      if (textResource == null)
         throw new FileNotFoundException(fileNamePrefix + languageCode_ + ".properties");
      
      resources_ = FileUtils.CreateProperties(textResource);
      
      if (textResource != null)
         textResource.close();
   }

   /**
    * Returns the translated string for the given key using the
    * current <tt>Localizer</tt> instance context.
    *
    * @param key the key for the translation to look for in this
    * <tt>Localizer</tt> instance
    * @return a translation for the <tt>key</tt>
    */
   public String getLocalized(String key)
   {
      return resources_.getProperty(key);
   }
   
   /**
    * Returns the 2 character language code
    * @return a String with the language code</tt>
    */
   public String getLanguageCode()
   {
      return languageCode_;
   }
}
