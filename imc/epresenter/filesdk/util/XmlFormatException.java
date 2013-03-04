package imc.epresenter.filesdk.util;

/*
 * File:             XmlFormatException.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: XmlFormatException.java,v 1.1 2001-10-05 12:10:33 danielss Exp $
 */
import java.io.IOException;

/**
 * An exception of this class is thrown if the xml file read by {@link
 * XmlParser XmlParser} is malformed in any way. The message passed with
 * the exception tries to give a reason why the parsing failed.
 *
 * @see XmlParser
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class XmlFormatException extends IOException 
{
   public XmlFormatException(String message)
   {
      super(message);
   }
}
