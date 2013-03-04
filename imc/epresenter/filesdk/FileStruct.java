package imc.epresenter.filesdk;

/*
 * File:             FileStruct.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: FileStruct.java,v 1.5 2002-10-07 14:30:47 danielss Exp $
 */

/**
 * This class is a data container for files contained in an {@link
 * Archiver Archiver} archive.
 *
 * @see Archiver
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin
 * Danielsson</a>
 */
public class FileStruct implements java.io.Serializable
{
   static final long serialVersionUID = -4323758989860833711L;

   public FileStruct()
   {
      this(null, 0, 0, 0, false, false, null);
   }

   /**
    * Creates a new <tt>FileStruct</tt> instance with the given
    * parameters.
    *
    * @param name the name of the file
    * @param offset the offset of this file in the archive
    * @param size the deflated size of the file in the archive
    * @param realSize the real size of the file in the archive
    * @param compressed <i>true</i> if this is/should be compressed
    * @param isConfigFile <i>true</i> if and only if this file is an
    * ePresentation configuration file
    * @param realName if not <tt>null</tt>, this is the file which is
    * going to be added as <tt>name</tt> into the archive
    */
   public FileStruct(String name, int offset, int size, int realSize,
		     boolean compressed, boolean isConfigFile,
		     String realName)
   {
      this.name         = name;
      this.offset       = offset;
      this.size         = size;
      this.realSize     = realSize;
      this.isCompressed = compressed;
      this.isConfigFile = isConfigFile;
      this.realName     = realName;
   }

   public String  name;
   public int     offset;
   public int     size;
   public int     realSize;
   public boolean isCompressed;
   public boolean isConfigFile;
   public String  realName;
}
