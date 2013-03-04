package imc.epresenter.filesdk;

/*
 * File:             ResourceNotFoundException.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: ResourceNotFoundException.java,v 1.2 2001-10-12 07:45:57 danielss Exp $
 */

import java.io.FileNotFoundException;

/**
 * Instances of this class may be thrown if a {@link FileResources
 * FileResources} object does not find a resource that is being
 * requested.
 *
 * @see FileResources
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class ResourceNotFoundException extends FileNotFoundException
{
    public ResourceNotFoundException(String message)
    {
	super(message);
    }
}
