package imc.epresenter.converter;

/*
 * File:             ConversionDisplay.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: ConversionDisplay.java,v 1.4 2010-02-19 12:26:54 zoen Exp $
 */

/**
 * Classes implementing this interface may be used by the {@link
 * PresentationConverter PresentationConverter} in order to display
 * messages that occur during the conversion of an AOF document. The
 * {@link #logMessage logMessage} method is used by the {@link
 * PresentationConverter PresentationConverter} to display more
 * general messages. The two methods {@link #displayCurrentFile
 * displayCurrentFile} and {@link #displayCurrentProgress
 * displayCurrentProgress} belong together: The {@link
 * #displayCurrentFile displayCurrentFile} method is called before
 * work (of some kind, given as <tt>action</tt>) on a specific file
 * has started. Then the {@link #displayCurrentProgress
 * displayCurrentProgress} method is called in order to notify the
 * <tt>ConversionDisplay</tt> on the progress (measured in bytes of
 * the file) on the work.
 *
 * @see ConverterGui
 * @see BatchConverter
 * @see PresentationConverter
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public interface ConversionDisplay
{
   /**
    * Display a general message.
    * @param message the message to display
    */
   public void logMessage(String message);

   /**
    * Display a new file that is being worked on. The
    * <tt>fileSize</tt> parameter may be used in order to initialize
    * e.g. progress bars. The <tt>action</tt> parameter tells the
    * <tt>ConversionDisplay</tt> what is being done to the file.
    *
    * @param newFile the file that will be processed
    * @param fileSize the size of <tt>newFile</tt> in bytes
    * @param action a string describing what is being done to
    * <tt>newFile</tt>
    * @see #displayCurrentProgress
    */
   public void displayCurrentFile(String newFile, int fileSize, String
				  action);

   /**
    * Display the current progress on the active file. This method is
    * called only after the {@link #displayCurrentFile
    * displayCurrentFile} method has been called. This method may be
    * used to update e.g. a progress bar.
    *
    * @param bytesDone the number of bytes processed up to now
    * @see #displayCurrentFile
    */
   public void displayCurrentFileProgress(int bytesDone);


   /**
    * Display the current progress. This method may be
    * used to update e.g. a progress bar.
    *
    * @param fProgress the progress in the rang [0.0..1.0]
    * @see #displayCurrentFile
    */
   public void displayCurrentProgress(float fProgress);

   /**
    * Display the current progress step. This method is
    * used by the ProgressManager for "silent publishing". 
    * See imc.lecturnity.converter.ProgressManager for more details. 
    *
    * @param strStepId a String containing a unique progress step ID
    * @param strLabel a String containing a label for the progress step
    */
   public void displayCurrentProgressStep(String strStepId, String strLabel);
}
