package imc.epresenter.converter;

/*
 * File:             DefaultConversionDisplay.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: DefaultConversionDisplay.java,v 1.4 2010-02-19 12:27:06 zoen Exp $
 */

/**
 * A default implementation of the {@link ConversionDisplay
 * ConversionDisplay} interface. Instances of this class simply prints
 * to <tt>stdout</tt> using the <tt>System.out.println()</tt> method. It
 * will output the <tt>action</tt>, the file and the file size on {@link
 * #displayCurrentFile displayCurrentFile} events, but it will ignore
 * any {@link #displayCurrentProgress displayCurrentProgress}
 * calls. An instance of this class will be created if a {@link
 * PresentationConverter PresentationConverter} is instanciated with the
 * {@link ConversionDisplay ConversionDisplay} parameter set to
 * <tt>null</tt>.
 *
 * @see ConversionDisplay
 * @see PresentationConverter
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class DefaultConversionDisplay implements ConversionDisplay
{
   /**
    * @see ConversionDisplay#logMessage
    */
   public void logMessage(String message)
   {
      System.out.println(message);
   }

   /**
    * @see ConversionDisplay#displayCurrentFile
    */
   public void displayCurrentFile(String newFile, int size, String action)
   {
      System.out.println(action + " " + newFile + " (" + size + " bytes)");
   }

   /**
    * @see ConversionDisplay#displayCurrentFileProgress
    */
   public void displayCurrentFileProgress(int bytesDone)
   {
   }

   /**
    * @see ConversionDisplay#displayCurrentProgress
    */
   public void displayCurrentProgress(float fProgress)
   {
   }

   /**
    * @see ConversionDisplay#displayCurrentProgressStep
    */
   public void displayCurrentProgressStep(String strStepId, String strLabel)
   {
   }
}
