package imc.epresenter.filesdk.util;

/*
 * File:             ByteCounter.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: ByteCounter.java,v 1.2 2001-10-12 07:43:45 danielss Exp $
 */

/**
 * If you want to make use of the {@link CountInputStream
 * CountInputStream} class, you will have to implement this
 * interface. After a read action in a {@link CountInputStream
 * CountInputStream}, the {@link #displayCountedBytes
 * displayCountedBytes} method of the <tt>ByteCounter</tt> will be
 * called, provided the last call to {@link #displayCountedBytes
 * displayCountedBytes} has returned to the dispatcher thread in the
 * {@link CountInputStream CountInputStream} class.
 *
 * @see CountInputStream
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public interface ByteCounter
{
   /**
    * Display the number of bytes counted by the corresponding {@link
    * CountInputStream CountInputStream} instance.
    *
    * @see CountInputStream
    */
   public void displayCountedBytes(int bytes);
}
