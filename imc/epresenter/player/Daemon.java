package imc.epresenter.player;

import java.awt.*;
import java.io.*;
import java.net.*;
import java.text.*;
import java.util.*;
import javax.swing.JOptionPane; // for debug output

import imc.epresenter.filesdk.FileResources;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.SecurityUtils;
import imc.lecturnity.util.ui.SplashScreen;

public class Daemon extends Thread {
   private static final int SERVERPORT = 21777;
   private static SplashScreen splashScreen_;
   private static Thread loadThread_;
   private static int remainingDays_;
   private static Date endDate_;

   public static void main(String[] args) {
      // start as background process (no args)
      Thread t = new Daemon();
      t.start();
      try { t.join(); } catch (InterruptedException e) {}
   }


   private boolean loadedAsDaemon_;
   private GlobalMasterAndUI master_;

   public Daemon() {
      this (null, -1, true);
   }

   public Daemon(boolean daemonMode)
   {
      this (null, -1, daemonMode);
   }

   public Daemon(boolean daemonMode, String htmlLocation)
   {
      this (null, htmlLocation, 0, daemonMode);
   }

   public Daemon(String location, int time, boolean daemonMode) {
      this (location, null, time, daemonMode);
   }

   public Daemon(String location, String htmlLocation, int time, boolean daemonMode) {
      //super("Daemon: External command interface");
      setName("Daemon: External command interface");
      
      Manager.DebugMsg("Checking for player...");
      
      if (serverExists(location, htmlLocation, time)) 
         System.exit(0); // server exists and is informed
      
      loadedAsDaemon_ = daemonMode;
      
      Manager.DebugMsg("No player detected.");
      
      if (Manager.isWindows() && !NativeUtils.isLibraryLoaded())
      {
         Manager.DebugMsg("No native library.");
         
         Manager.showError(Manager.getLocalized("neededFileMissing"), Manager.ERROR, null);
         System.exit(1); // this is turned off in showError
      }
     
      // double code (Document)
      int versionType = SecurityUtils.getInstallationType(Manager.versionNumber);
      // can also be VERSION_ERROR; ie if cd start
              
      boolean isEvaluationVersion = 
         versionType == NativeUtils.EVALUATION_VERSION || 
         versionType == NativeUtils.UNIVERSITY_EVALUATION_VERSION;

      Manager.DebugMsg("Eval version? "+isEvaluationVersion);

      // double code (Document)
      boolean isProtected = false;
      if (location != null)
         isProtected = SecurityUtils.isDocumentCleared(location);

      remainingDays_ = 1;
      endDate_ = null;
      if (isEvaluationVersion)
      {
         if (Manager.isWindows())
            remainingDays_ = SecurityUtils.getRemainingEvaluationDays();
         else
            isEvaluationVersion = false; // special case for linux
      }
      else
      {
         if (!isProtected)
            endDate_ = SecurityUtils.getEndDate(false);
         // else it stays null and is therefore unlimited
      }

      //remainingDays_ = -1;
      /*
      GregorianCalendar gcal = new GregorianCalendar();
      gcal.add(Calendar.DAY_OF_MONTH, +1);
      endDate_ = gcal.getTime();
      //*/

      Manager.DebugMsg("Showing splash screen...");
      
      // amongst others: in order to have getSplashBounds() working
      prepareSplashScreen(isEvaluationVersion, remainingDays_, endDate_);
      
      // double code (Document) continued
      if (Manager.isWindows())
      {
         if (System.getProperty("html.display") == null && !isProtected)
         {
            if (SecurityUtils.getInstallationType(Manager.versionNumber) == NativeUtils.VERSION_ERROR)
            {
               Manager.showError(Manager.getLocalized("versionConflict"), Manager.ERROR, null);
               System.exit(1); // this is turned off in showError
            }
         }
      }
      
      if (!Manager.isWindows() || System.getProperty("html.display") != null || 
          remainingDays_ > -1 && (endDate_ == null || endDate_.after(new Date()))) {
         
         // nothing wrong found start normally
         
         if (NativeUtils.isLibraryLoaded() && System.getProperty("html.display") == null)
         {
            // every start (normal and not hacked) should pass
            // here except it is started from cd (export, eval) or the tutorial;
            // check if an update is needed and if so quit
            int nStop = NativeUtils.lecturnityUpdateCheck();
            if (nStop != 0)
               System.exit(0);
         }
         
         // if the user during this check says he wants to change
         // the setting true ist returnd and thus this program is left
         //if (FileUtils.checkCodepageAndStandards(null))
         //   System.exit(0);
         // Check deactivated as of 2.0.p2.
         
                          
         master_ = new GlobalMasterAndUI(this, location, time);
         
         Manager.DebugMsg("GlobalMasterAndUI created.");
      }
      
      // don't show SplashScreen if it is a website (only)
      if (location != null || htmlLocation == null)
         showSplashScreen();


      if (Manager.isWindows() && System.getProperty("html.display") == null &&
          (remainingDays_ < 0 || endDate_ != null && endDate_.before(new Date())))
      {
         splashScreen_.waitForDispose(); 
         System.exit(0);
      }

      /*
      Thread javaAliveThread = new Thread() {
         public void run()
         {
            while(true)
            {
               try { sleep(1000); } catch (InterruptedException exc) {}
               System.out.print(".");
            }
         }
      };
      javaAliveThread.start();
      //*/

      //if (System.getProperty("html.display") != null)
      //   System.setProperty("html.display", null);
   }

   private boolean serverExists(String location, int time) {
      return serverExists(location, null, time);
   }

   private boolean serverExists(String location, String htmlLocation, int time) {
    	try {
         Manager.DebugMsg("Trying to connect...");
         
         // contact the currently running instance
         Socket s = new Socket("localhost", SERVERPORT);
         
         Manager.DebugMsg("Connection made.");
         
         BufferedReader in = new BufferedReader(
            new InputStreamReader(s.getInputStream()));
         PrintStream out = new PrintStream(s.getOutputStream(), true);
         
         // Bugfix #2037: make an authentication step to avoid problems with proxies
         String strAuth = in.readLine();
         
         Manager.DebugMsg("Got auth string: "+strAuth);
         
         if (!strAuth.equals("LECTURNITY Player"))
         {
            Manager.DebugMsg("Not the player listening!");
            
            return false;
         }
         else
            Manager.DebugMsg("Player is listening.");


         boolean somethingWritten = false;
         if (location != null) {
            out.println("load \""+location+"\" "+time); 
            
            somethingWritten = true;
         } 

         if (htmlLocation != null)
         {
            out.println("load "+htmlLocation);
            
            somethingWritten = true;
         }
         
         if (!somethingWritten) {
            out.println("nop");
         }

         s.close();

         if (location != null && htmlLocation != null)
         {
            // second load was ignored (connection closed before it could be read)
            return serverExists(null, htmlLocation, 0);
         }

         return true;
      } catch (IOException ioe) {
         return false;
      }
   }

   public GlobalMasterAndUI getMaster()
   {
      return master_;
   }


   // waiting for external commands
   public void run() {
      boolean DEBUG = System.getProperty("app.netdebug") != null;
      
      ServerSocket server = null;
      try {
         server = new ServerSocket(SERVERPORT);
      } catch (IOException ioe) {
         ioe.printStackTrace();
         return;
      }
      while (true) {
         try {
            if (DEBUG) System.out.println("Wating for new connection...");

            Socket client = server.accept();

            if (DEBUG) System.out.println("Connection established.");
            if (DEBUG) System.out.println("My address is: "+InetAddress.getByName("localhost"));
            if (DEBUG) System.out.println("Remote adr is: "+client.getInetAddress());

            // only local connections are allowed
            if (client.getInetAddress().equals(InetAddress.getByName("localhost")))
            {

               BufferedReader in = new BufferedReader(
                  new InputStreamReader(client.getInputStream(), "utf-8"));
               PrintStream out = new PrintStream(client.getOutputStream(), true);
               
               if (DEBUG) System.out.println("Writing auth string...");
               
               out.println("LECTURNITY Player");

               if (DEBUG) System.out.println("Trying to read command...");

               String commandLine = in.readLine();

               if (DEBUG) System.out.println("Got command: "+commandLine);
                 
               if (commandLine == null)
                  commandLine = "";
               
               //
               // split input command line (with a bit of parsing),
               // because paths may contain spaces
               //
               StringTokenizer st = new StringTokenizer(commandLine, " ");
               ArrayList commandCollector = new ArrayList(3);
               
               String lastString = null;
               boolean quoteActive = false;
               while (st.hasMoreTokens())
               {
                  String nextString = st.nextToken();
                  
                  if (quoteActive)
                  {
                     if (nextString.endsWith("\""))
                     {
                        lastString += nextString.substring(0,nextString.length()-1);
                        commandCollector.add(lastString);
                        lastString = null;
                        quoteActive = false;
                     }
                     else
                     {
                        lastString += nextString+" ";
                     }
                  }
                  else
                  {
                     if (nextString.startsWith("\""))
                     {
                        if (nextString.endsWith("\""))
                        {
                           if (nextString.length() > 1)
                              commandCollector.add(nextString.substring(1,nextString.length()-1));
                        }
                        else
                        {
                           quoteActive = true;
                           lastString = nextString.substring(1)+" ";
                        }
                     }
                     else
                     {
                        commandCollector.add(nextString);
                     }
                  }
               }

               String[] commands = new String[commandCollector.size()];
               commandCollector.toArray(commands);


               //
               // now do what is to be done
               //
               if (commands.length == 0) {
                  // protection against flooding:
                  try { Thread.sleep(1000); } catch (InterruptedException e) {}
               }
               else
               {   
                  String givenLocation = null; 
                  if (commands.length > 1)
                     givenLocation = commands[1];
                  Document appropriateDocument = null;

                  if (givenLocation != null) // find appropriate Document, if any
                     appropriateDocument = master_.getDocumentIfLoaded(givenLocation);
                  boolean thatDocumentLoaded = appropriateDocument != null;
                   
                  if (DEBUG) {
                      if (thatDocumentLoaded)
                          System.out.println("That document is already loaded.");
                      else
                          System.out.println("That document is not loaded.");
                  }

                  int millis = -1;
                  boolean propperTimeGiven = false; 
                  if (commands.length > 2) 
                  {
                     try
                     {
                        millis = Integer.parseInt(commands[2]);
                        if (millis >= 0)
                           propperTimeGiven = true;
                     }
                     catch (NumberFormatException e)
                     {
                     }
                  }

                  if (commands[0].equals("time")) {
                     if (thatDocumentLoaded) {
                        if (propperTimeGiven) {
                           appropriateDocument.setTime(millis);
                        } else {
                           out.println("Command had no or no propper time value.");
                           try { Thread.sleep(1000); } catch (InterruptedException e) {}
                        }
                     } else {
                        out.println("That document is not loaded.");
                        try { Thread.sleep(1000); } catch (InterruptedException e) {}
                     }
                  } else if (commands[0].equals("load")) {
                     if (thatDocumentLoaded)
                     {
                        out.println("That document is already loaded.");
                        try { Thread.sleep(1000); } catch (InterruptedException e) {}
                     }
                     else
                     {
                         if (DEBUG) System.out.println("Should be loading this: " + givenLocation);
                        if (givenLocation != null)
                        {
                           if (commands.length > 2 && !propperTimeGiven) {
                              out.println("Command had no propper time value.");
                              try { Thread.sleep(1000); } catch (InterruptedException e) {}
                           } else if (!new File(givenLocation).exists()) {
                              out.println("Given file ("+givenLocation+") not found.");
                              try { Thread.sleep(1000); } catch (InterruptedException e) {}
                           } else {
                              if (!givenLocation.toLowerCase().endsWith(".html"))
                              {
                                 int idx1 = givenLocation.toLowerCase().indexOf("tutorial");
                                 if (idx1 > -1 && 
                                     givenLocation.toLowerCase().indexOf("tutorial.lpd", idx1+1) > -1)
                                    Manager.mapLocationToTitle(givenLocation, Manager.getLocalized("tutorial"));
         
                                    //System.setProperty("tutorial.mode", givenLocation);
                                 
                                  if (DEBUG) System.out.println("Loading this document...");
                                 if (propperTimeGiven)
                                    master_.loadDocument(givenLocation, millis);
                                 else
                                    master_.loadDocument(givenLocation);
                              }
                              else
                              {
                                 String title = Manager.getLocalized("showHtml");
                                 if (givenLocation.toLowerCase().indexOf("player") != -1 &&
                                     givenLocation.toLowerCase().indexOf("help") != -1)
                                    title =  Manager.getLocalized("onlineHelp");
                                 if (givenLocation.toLowerCase().indexOf("tutorial") != -1 &&
                                     givenLocation.toLowerCase().indexOf("600x550") != -1)
                                    title = Manager.getLocalized("tutorial");
                                 
                                 Manager.showHtmlFrame(
                                    master_, title, givenLocation); 
                              }
                           }
                        } else {
                           out.println("No file argument given.");
                           try { Thread.sleep(1000); } catch (InterruptedException e) {}
                        }
                     }
                  } else if (commands[0].equals("help")) {

                     String helpPath = System.getProperty("help.path");
                     Manager.showHtmlFrame(master_, Manager.getLocalized("onlineHelp"), helpPath); 

                  } else if (commands[0].equals("start")) {
                     if (thatDocumentLoaded) {
                        appropriateDocument.start();
                     } else {
                        out.println("That document is not loaded.");
                        try { Thread.sleep(1000); } catch (InterruptedException e) {}
                     }
                  } else if (commands[0].equals("stop")) {
                     if (thatDocumentLoaded) {
                        appropriateDocument.stop();
                     } else {
                        out.println("That document is not loaded.");
                        try { Thread.sleep(1000); } catch (InterruptedException e) {}
                     }
                  } else if (commands[0].equals("close")) {
                     if (thatDocumentLoaded) {
                        master_.closeDocument(appropriateDocument);
                     } else {
                        out.println("That document is not loaded.");
                        try { Thread.sleep(1000); } catch (InterruptedException e) {}
                     }
                  } else if (commands[0].startsWith("nop")) {
                     try { Thread.sleep(1000); } catch (InterruptedException e) {}
                  } else {
                     // command not known
                     out.println("Command unknown.");
                     try { Thread.sleep(1000); } catch (InterruptedException e) {}
                  }
               }
            } else { // not allowed
               if (DEBUG) System.out.println("Connection from "+client.getInetAddress()+" refused.");
               try { Thread.sleep(1000); } catch (InterruptedException e) {}
            }

            client.close();

         } catch (IOException ioe) {
            ioe.printStackTrace();
            if (!(ioe instanceof SocketException)) {
               if (DEBUG) System.out.println("Other exception than "
                                             +"SocketException. Quitting Daemon.");
               return;
            }
         }

         if (DEBUG) System.out.println("End of loop (communication).");

      }
   }

   // last document closed => close program (if not daemon mode)
   public void shutDown() {
      // hm: and Window references?
      if (!loadedAsDaemon_) System.exit(0);
   }
   



   //
   // static methods for splash screen
   //
   public static Rectangle getSplashBounds()
   {
      if (splashScreen_ != null)
         return splashScreen_.getBounds();
      else
      {
         Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
         return new Rectangle(screen.width/2-125, screen.height/2, 250, 0);
         //return new Rectangle(0,0,250,0); // hm
      }
   }

   public static void setSplashText(String message)
   {
      if (splashScreen_ != null)
         splashScreen_.replaceMessage(message);
   }

   public static void resetSplashText()
   {
      if (splashScreen_ != null)
         splashScreen_.resetMessage();
   }
/*
   public static void stopSplashJostling()
   {
      if (splashScreen_ != null)
         splashScreen_.stopJostling();

   }
   */
   public static void toFrontSplashScreen()
   {
      if (splashScreen_ != null)
         splashScreen_.toFront();
   }
   
   public static void hideSplashScreen()
   {
      if (splashScreen_ != null)
         splashScreen_.duckAndCover(true);
   }


   private static void prepareSplashScreen(
      boolean isEvaluationVersion, int remainingDays, Date endDate)
   {
      if (splashScreen_ == null)
      {
         boolean checkEnddate = false;
         checkEnddate = Manager.isWindows() &&
            System.getProperty("html.display") == null;
  
         splashScreen_ = new SplashScreen(
            isEvaluationVersion, remainingDays, checkEnddate ? endDate : null);
         Point p = splashScreen_.getLocation();
         p.y -= 50;
         splashScreen_.setLocation(p);
      }
   }

   private static void showSplashScreen()
   {   
      splashScreen_.exhibit();

      if (loadThread_ == null || !loadThread_.isAlive()) // no document is being loaded
                                                         // that means html page is displayed
                                                         // or called without document
                                                         // then hide it after 5 seconds
      {
          splashScreen_.closeSoon();
      }
   }

   /*
   public static boolean isProgramWornOut(int versionType)
   {
   if (versionType == NativeUtils.EVALUATION_VERSION
   || versionType == NativeUtils.UNIVERSITY_EVALUATION_VERSION)
   {
   int days = getRemainingEvaluationDays();
   if (days >= 0)
   return false;
   else
   return true;
   }

   return false;
   }
   */

   /* Jetzt in SecurityUtils.java
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
   */

}	