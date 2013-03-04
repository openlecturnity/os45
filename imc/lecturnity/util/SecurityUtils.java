package imc.lecturnity.util;

import java.io.*;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;

import imc.epresenter.player.Manager;


public class SecurityUtils
{
   public static boolean isEvaluationConflict(int documentType)
   {

      // document type vs. installation type
      int versionType = getInstallationType(Manager.versionNumber);

      boolean documentIsEval = (documentType == NativeUtils.EVALUATION_VERSION 
                                || documentType == NativeUtils.UNIVERSITY_EVALUATION_VERSION);
      boolean playerIsFull = (versionType == NativeUtils.FULL_VERSION 
                              || versionType == NativeUtils.UNIVERSITY_VERSION);
     
      return (playerIsFull && documentIsEval);
   }

   public static boolean isCampusConflict(int documentType)
   {

      int versionType = getInstallationType(Manager.versionNumber);
   
      return (versionType == NativeUtils.UNIVERSITY_VERSION && documentType == NativeUtils.FULL_VERSION);

   }

   public static boolean isDocumentCleared(String location)
   {
      return NativeUtils.isLibraryLoaded() && NativeUtils.isProtectedDocument(location);
   }

   public static int getInstallationType(String ownVersion)
   {
      if (Manager.isWindows())
      {
         int versionType = NativeUtils.VERSION_ERROR;

         if (NativeUtils.isLibraryLoaded())
            versionType = NativeUtils.getVersionType(ownVersion);

         // TODO:  ???
         //Manager.showError(Manager.getLocalized("neededFileMissing"), Manager.ERROR, null);

         return versionType;
      }
      else
         return NativeUtils.UNIVERSITY_EVALUATION_VERSION; // default  (see #754)
   }

   /* Not here, because it needs working FileResources.
   public static int getDocumentType(String location)
   {

   }
   */

   /**
    * @returns null, if no end date is set; the actual date object if a date is set
    *          and a date today minus one day in case of an error
    */
   public static Date getEndDate(boolean ignoreFailure)
   {
      Date endDate = null;
      String endString = null; 
      if (NativeUtils.isLibraryLoaded())
         endString = NativeUtils.getExpirationDate();
      if (endString != null)
      {
         if (endString.length() != 0)
         {
            SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd");

            try
            {
               endDate = sdf.parse(endString);
               // make it end not today 0:00 but 23:59
               GregorianCalendar gcal = new GregorianCalendar();
               gcal.setTime(endDate);
               gcal.add(Calendar.DAY_OF_MONTH, +1);
               gcal.add(Calendar.SECOND, -1);
               endDate = gcal.getTime();
            }
            catch (ParseException pe)
            {
               if (!ignoreFailure)
               {
                  GregorianCalendar gcal = new GregorianCalendar();
                  gcal.add(Calendar.DAY_OF_MONTH, -1);
                  endDate = gcal.getTime();
               }
            }

         }
         else
         {
            // everything is alright, unlimited version
            // endDate remains null
         }
      }
      else
      {
         if (!ignoreFailure)
         {
            // endDate == null means error (registry and/or date)
            // make the program not work
            GregorianCalendar gcal = new GregorianCalendar();
            gcal.add(Calendar.DAY_OF_MONTH, -1);
            endDate = gcal.getTime();
         }
      }

      return endDate;
   }

   /**
    * @returns if it is an evaluation version:
    *          the difference between the evaluation period and the
    *          duration of the installation
    *          in any other case (eval expired, no eval version, error): -1
    */
   public static int getRemainingEvaluationDays()
   {

      Date install = getInstallationDate();
      Date today = new Date();

      long diffMillis = today.getTime()-install.getTime();
      int days = (int)(diffMillis/(1000*60*60*24));

      return days >= 0 ? NativeUtils.getEvaluationPeriod()-days : -1;
      //return days >= 0 ? 32-days : -1;
   }

   private static Date getInstallationDate()
   {
      SimpleDateFormat sdf = new SimpleDateFormat("yyMMdd");
      Date d = null;
      try
      {
         d = sdf.parse(NativeUtils.getInstallationDate());
      }
      catch (ParseException e)
      {
         Manager.showError("Holla! Kaputt!", Manager.ERROR, e);
      }

      return d;
   }

}
