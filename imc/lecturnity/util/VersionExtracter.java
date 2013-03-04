package imc.lecturnity.util;

/*
 * File:             VersionExtracter.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: VersionExtracter.java,v 1.1 2002-02-06 08:32:24 danielss Exp $
 */

import java.io.InputStream;
import javax.swing.JOptionPane;

/**
 * An instance of this class may be used to extract version
 * information from a resource identifier. This class cannot throw any
 * exception, but it may display an error message if reading the
 * resource fails for any reason. This is a convenience class to
 * enable having the version information in a separate file and not
 * having to change Makefiles or any classes whenever just changing
 * version information.
 *
 * <p>Please note that the resource file should not have any EOL (end
 * of line) character at the end of the version information, as all
 * characters will be returned as a <tt>String</tt>, including the EOL
 * characters.
 *
 * @author Martin Danielsson
 */
public class VersionExtracter
{
   private String versionInfo_ = "0.0.0";

   /**
    * Constructs a new <tt>VersionExtracter</tt> instance which
    * contains the version information from <tt>versionResource</tt>.
    */
   public VersionExtracter(String versionResource)
   {
      try
      {
         InputStream versionStream = getClass().getResourceAsStream(versionResource);
         
         int bufSize = 128; // should definitely be enough
         byte[] tmpBuffer = new byte[bufSize]; 
         int readBytes = 0;
         int totalRead = 0;
         while (readBytes != -1)
         {
            readBytes = versionStream.read(tmpBuffer, totalRead,
                                           bufSize - totalRead);
            if (readBytes != -1)
               totalRead += readBytes;
         }
      
         versionInfo_ = new String(tmpBuffer, 0, totalRead);
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Version information extraction failed!\n " +
                                       "Please report this error!", "Error",
                                       JOptionPane.ERROR_MESSAGE);
         versionInfo_ = "0.0.0";
      }
   }

   /**
    * Returns a <tt>String</tt> containing the version information
    * contained in the resource given in the constructor.
    * @return a <tt>String</tt> containing the version information
    * contained in the resource given in the constructor.
    */
   public String getVersion()
   {
      return versionInfo_;
   }
}
