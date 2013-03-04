package imc.epresenter.filesdk.util;

/*
 * File:             Logger.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: Logger.java,v 1.4 2001-10-12 08:22:49 danielss Exp $
 */

import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.IOException;

import java.text.DateFormat;
import java.util.Date;

/**
 * This is a conveniance class for logging purposes. Create a
 * <tt>Logger</tt> instance with the constructor and ive a file name
 * of the file to use as a log file. The <tt>Logger</tt> will append
 * to already existing files. Then use the {@link #log log} method in
 * order to write into the log file. The <tt>Logger</tt> will
 * automatically add a date and time stamp in front of the logged
 * message. If the logging should be stopped, call the {@link #close
 * close} method. The <tt>Logger</tt> will then write a <tt>&quot;Log
 * closed&quot;</tt> message and then close the output
 * stream to the log file.
 *
 * <p><b>Note:</b> The <tt>Logger</tt> instances will under no
 * circumstances throw any exceptions. In case of problems, the file
 * output stream to the log file will be closed and any messages will
 * be output to <tt>stderr</tt> (using {@link
 * java.lang.System.err#println System.err.println}.
 *
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class Logger
{
   private PrintWriter out_ = null;

   /**
    * Creates a new <tt>Logger</tt> instance for the file with the
    * given file name. If the creation of this file fails, the logged
    * messages will be output to <tt>stderr</tt>.
    *
    * @param logFileName the name of the file to write to
    */
   public Logger(String logFileName)
   {
      try
      {
	 out_ = new PrintWriter(new FileWriter(logFileName, true), true);
	 log("Opening log `" + logFileName + "'.");
      }
      catch (IOException e)
      {
	 System.err.println("Severe Error in Logger class:");
	 e.printStackTrace();
	 out_ = null;
      }
   }

   /**
    * Log a message to the log file. The <tt>Logger</tt> instance will
    * append a date and time stamp in front of the message.
    */
   public void log(String message)
   {
      String logMsg = appendTime(message);
      if (out_ != null)
	 out_.println(logMsg);
      else
	 System.err.println("NOT LOGGED: " + logMsg);
   }

   /**
    * Write a <tt>&quot;Log closed.&quot;</tt> message to the log file
    * and then close the file output stream.
    */
   public void close()
   {
      log("Log closed.");
      out_.flush();
      out_.close();
      out_ = null;
   }

   /**
    * Return the {@link java.io.PrintWriter PrintWriter} instance used
    * in this <tt>Logger</tt> instance. This may come handy if you
    * want to redirect a stack trace to the log file:
    * <pre>
    * try
    * {
    *    [ do something dangerous ... ]
    * }
    * catch (Exception e)
    * {
    *    e.printStackTrace(logger.getPrintWriter());
    * }
    * </pre>
    *
    * @return the {@link java.io.PrintWriter PrintWriter} instance
    * used by this <tt>Logger</tt> instance.
    */
   public PrintWriter getPrintWriter()
   {
      return out_;
   }

   private String appendTime(String message)
   {
      return ("[" + (new Date()).toString() + "] " + message);
   }
}
