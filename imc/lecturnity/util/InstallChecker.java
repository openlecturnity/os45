package imc.lecturnity.util;

public class InstallChecker
{
   public static final int JAVA_VERSION_MISMATCH = 1;
   public static final int JMF_MISSING = 2;
   public static final int JAI_MISSING = 4;

   public static void main(String[] args)
   {
      System.exit(checkInstallation());
   }

   /**
    * Use this method in order to check for some classes that are only
    * available in the JMF respectively the JAI. The integer returned
    * will be a combination of the constants
    * <tt>JAVA_VERSION_MISMATCH</tt> (if the Java version required
    * does not match the current Java version), <tt>JMF_MISSING</tt>
    * (if the Java Media Framework is missing) and
    * <tt>JAI_MISSING</tt> (if the Java Advanced Imaging API is
    * missing). You can check for the e.g. JMF by ANDing the result
    * with the constant above:
    * <CODE>
    *   int result = InstallChecker.checkInstallation();
    *   boolean noJmfInstalled = (result & InstallChecker.JMF_MISSING) > 0;
    * </CODE>
    */
   public static int checkInstallation()
   {
      int ret = 0;

      try
      {
         Class clock = Class.forName("javax.media.Clock");
      }
      catch (ClassNotFoundException e)
      {
         ret += JMF_MISSING;
      }

      try
      {
         Class jai = Class.forName("javax.media.jai.JAI");
      }
      catch (ClassNotFoundException e)
      {
         ret += JAI_MISSING;
      }

      return ret;
   }
}
