package imc.epresenter.filesdk;

/*
 * File:             FileFormatException.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: FileFormatException.java,v 1.2 2001-10-12 07:45:35 danielss Exp $
 */

import java.io.IOException;

/**
 * An instance of this exception class is thrown if an ePresentation
 * archive is read and it has the wrong format.
 *
 * @see ArchivedFileResources
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class FileFormatException extends IOException
{
   public FileFormatException(String message)
   {
      super(message);
   }
}
