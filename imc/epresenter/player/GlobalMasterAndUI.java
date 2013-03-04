package imc.epresenter.player;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.URL;
import java.net.MalformedURLException;
import java.text.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.player.Document;
import imc.epresenter.player.util.FloatNativeComponent;
import imc.epresenter.player.util.TimeFormat;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.ui.LayoutUtils;


public class GlobalMasterAndUI implements DocumentController, AWTEventListener
{
   private Daemon daemon_;

   private Thread loadThread_;

   private HashMap currentDocuments_;  // location is the key
   private HashMap displayedCacheControls_;

   private String normalLocation_;
   private String quickLocation_;
   
   private JFrame mainFrame_;
   private JFrame helpFrame_;
   private JFrame cacheFrame_;
   private GlobalMenuBar mainMenuBar_;
   private GlobalMenuBar helpMenuBar_;

   private boolean alreadyWarned_;
   private boolean dummyVisible_;
   private boolean loadingDocument_;
   private int iNavigationState_;

   private JLabel copyLabel_;
   private JWindow copyWindow_;
   


   public GlobalMasterAndUI(Daemon daemon)
   {
      this(daemon, null, -1);
   }

   public GlobalMasterAndUI(Daemon daemon, String location, int time)
   {
      daemon_ = daemon;
      currentDocuments_ = new HashMap();
      
      iNavigationState_ = Document.NAV_STATE_ENABLED;

      //
      // install a global key listener in order to avoid focus "problems"
      // (e.g. button has the focus and is consuming a SPACE and thus
      // replay is not started/stopped)
      //
      Toolkit.getDefaultToolkit().addAWTEventListener(this, AWTEvent.KEY_EVENT_MASK);

      
      if (location != null)
      {
         loadDocument(location, time, true);
      }
      else if (System.getProperty("html.display") == null)
      {
         putInFrameAndShow(new Document(this), false); // this will be a dummy document
                                                       // showing only an image of the interface
         Manager.putLoadWindow(mainFrame_);
         daemon.toFrontSplashScreen();
      }
   }

   
   public void eventDispatched(AWTEvent evt)
   {
      if (evt.getID() == KeyEvent.KEY_RELEASED)
      {
         KeyEvent kevt = (KeyEvent)evt;
  
         long seconds = (System.currentTimeMillis()/1000)%60;
               
         //System.out.println(seconds+": "+kevt.getKeyCode()+" on "+kevt.getSource().getClass());

         Document document = null;
         JFrame frame = null;
               
         Component focusOwner = 
            SwingUtilities.findFocusOwner((Component)evt.getSource());
         Window originWindow = 
            SwingUtilities.windowForComponent((Component)evt.getSource());

         /*
         if (focusOwner != null)
         System.out.print(" "+"focus="+focusOwner.getClass());
         else
         System.out.print(" "+"focus="+focusOwner);
         if (originWindow != null)
         System.out.println(", origin="+originWindow.getClass());
         else
         System.out.println(", origin="+originWindow);
         //*/

         if (originWindow == null)   
         {
            if (focusOwner instanceof Window)
               originWindow = (Window)focusOwner;
            else if (evt.getSource() instanceof Window)
               originWindow = (Window)evt.getSource();
         }
          
         //System.out.println("originWindow="+originWindow);

         if (helpFrame_ != null && originWindow == helpFrame_)
         {
            document = (Document)currentDocuments_.get(quickLocation_);
            frame = helpFrame_;
         }
         else if (mainFrame_ != null && originWindow == mainFrame_)
         {
            document = (Document)currentDocuments_.get(normalLocation_);
            frame = mainFrame_;
         }

         if (document == null || frame == null)
         {
            //System.out.println(" document or frame == null");
            return;
         }

         final Document finalDoc = document;
               
         boolean bTextInput = focusOwner instanceof JTextField;

         /* Does not work as it is processed AFTER any menu accelerators.
         if (textInput)
         {
            System.out.println("textInput");
            kevt.consume();
         }
         */
         
         if (!bTextInput || (kevt.getModifiers() & kevt.CTRL_MASK) != 0)
         {
            if (kevt.getKeyCode() == kevt.VK_SPACE && (kevt.getModifiers() & kevt.ALT_MASK) == 0)
            {
               //if (kevt.getSource() instanceof AbstractButton) // sometimes it's a JPanel?
               document.doStartOrStop();
               kevt.consume();
            }
            else if (kevt.getKeyCode() == kevt.VK_F10)
            {
               // in this (special) case the Accelerators don't work
               // so we need to do it manually
               document.increaseVolume();
               kevt.consume();
            }
            else if (kevt.getKeyChar() == ':')
            {
               // in this (special) case the Accelerators don't work
               // so we need to do it manually
               document.restoreVideoSize();
               kevt.consume();
            }
            
            

            Container parent = SwingUtilities.getAncestorOfClass(
               JScrollPane.class, (Component)evt.getSource());
            boolean parentIsScrollPane = parent != null;
            
            if (parentIsScrollPane)
            {
               if (kevt.getKeyCode() == kevt.VK_PAGE_DOWN)
               {
                  document.requestNextSlide(null);
                  kevt.consume();
               }
               else if (kevt.getKeyCode() == kevt.VK_PAGE_UP)
               {
                  document.requestPreviousSlide(false, null);
                  kevt.consume();
               }
               else if (kevt.getKeyCode() == kevt.VK_RIGHT)
               {
                  document.jumpForward(10000);
                  kevt.consume();
               }
               else if (kevt.getKeyCode() == kevt.VK_LEFT)
               {
                  document.jumpBackward(10000);
                  kevt.consume();
               }
            }
            
            /*
            
            else if (kevt.getKeyCode() == kevt.VK_HOME)
            {
               document.jumpForward(-1);
               kevt.consume();
            }
            else if (kevt.getKeyCode() == kevt.VK_END)
            {
               document.jumpBackward(-1);
               kevt.consume();
            }
            else if (kevt.getKeyCode() == kevt.VK_F9)
            {
               document.decreaseVolume();
               kevt.consume();
            }
            //*/

         } // if (!textInput || ...)

         /*
         if ((kevt.getModifiers() & kevt.ALT_MASK) != 0)
         {
            if (kevt.getKeyCode() == kevt.VK_ENTER)
            {
                     
               //document.switchToFullScreen(false);

               kevt.consume();
            }
         }
         //*/
      
         if (kevt.getKeyCode() == kevt.VK_ESCAPE)
         {
            document.switchToFullScreen(true);

            kevt.consume();
         }

      } // KEY_RELEASED
   } // eventDispatched()
      


   public void loadDocument(String location)
   {
      loadDocument(location, 0);
   }

   public void loadDocument(String location, int time)
   {
      loadDocument(location, time, false);
   }
   public void loadDocument(String location, int time, boolean startup)
   {
      loadDocument(location, time, startup, null);
   }

   public void loadDocument(
      String location, int time, boolean startup, ActionListener finishListener)
   {
      loadDocument(location, time, startup, finishListener, null);
   }

   public void loadDocument(
      final String location, final int time, 
      final boolean startup, final ActionListener finishListener,
      final String parameters)
   {
      
      loadThread_ = new Thread("GlobalMaster: Document loading")
      {
         public void run()
         {
            if (loadingDocument_)
               return; // another loading is active

            loadingDocument_ = true;

            Window parent = Manager.getLoadWindow();
      
            String location2 = null;
            try
            {
               if (Manager.isWindows())
               {
                  // translates from lpl to lpd (if lpl is given)
                  // and after that if the lpd is contained
                  // in the cache it translates to that cache location
                  location2 = checkLocation(location);
               }
               else
                  location2 = location;
                  
            }
            catch (Throwable t)
            {
               Manager.showError(
                  parent, Manager.getLocalized("unexpectedError"), Manager.ERROR, t);
               Manager.putLoadWindow(null);
            }
            
            if (location2 == null)
            {
               if (startup)
                  System.exit(0);
               else
               {
                  // double code (below)
                  if (finishListener != null)
                     finishListener.actionPerformed(new ActionEvent(this, 0, "fehler"));
                  loadingDocument_ = false;
                  return;
               }
            }

            // often checkLocation() (above) makes a translation
            // from location (original) to a file in the cache (location2)
            // if the associated file comes from a cd it is 
            // protected (lecturnity.dat) but lecturnity.dat is 
            // not found in the cache
            // we need therefore the originalLocation to do the 
            // protection check (in Document) on it
            String originalLocation = null;
            if (!location.equals(location2))
               originalLocation = location;

            boolean wasFullScreen = LayoutNov2006.fullScreenActiveStatic_;
           

            boolean urlSaidFullScreen = false;

            if (parameters != null)
            {                                      
               String fs = "fullScreen=";
               if (parameters.indexOf(fs) > -1)
               {
                  if (parameters.substring(parameters.indexOf(fs)+fs.length()).startsWith("true"))
                     urlSaidFullScreen = true;
               }
            }


            //
            // Falls es noch nicht geladen, lade nun das Dokument, 
            // ansonsten setze nur den Zeitpunkt (und bring es in den Vordergrund).
            // Für den Fall, dass es (neu) geladen wird:
            // Überprüfe, ob es ein Schnellstartdokument ist oder eines, dass
            // automatisch im Vollbildmodus gestartet werden soll (3 Varianten).
            //
            Document oldDocument = (Document)currentDocuments_.get(location2);
            
            if (oldDocument != null && !oldDocument.isClosed())
            {
               oldDocument.setTime(time);
            }
            else // load or reload the document
            {
               boolean isHelpDocument = isHelpDocument(location2);
               String deactivateLocation = 
                  isHelpDocument ? quickLocation_ : normalLocation_;
               
               Document d = startupAndRemove(
                  location2, originalLocation, time, deactivateLocation);
               if (d != null)
               {
                  // Note for 'standard navigation':
                  // The document created with "startupAndRemove(...)" (see above) 
                  // sets the 'bIsNavigationEnabled_' flag.
                  // "putInFrameAndShow(...)" creates the BlobalMenuBar which 
                  // reads this flag. So don't change the order here!
                  putInFrameAndShow(d, isHelpDocument);

                  /*
                  if (System.getProperty("tutorial.mode") != null)
                  {
                     if (wasFullScreen || location.equals(System.getProperty("tutorial.mode")))
                        d.switchToFullScreen(false);
                  }
                  */

                  boolean isStartInFullscreen = FileResources.isStartInFullscreenDocument(location2);
                  //System.out.println("startInFullscreen="+isStartInFullscreen);
                  if (urlSaidFullScreen || FileResources.isStartInFullscreenDocument(location2))
                     d.switchToFullScreenAuto();

                  if (isHelpDocument)
                  {
                     d.start();
                     quickLocation_ = location2;
                  }
                  else
                  {
                     normalLocation_ = location2;
                  }
               }
               else if (startup) // document loading failed and is first one
                  System.exit(1);
            }
          
            // after loading is finished (only does something on first loading):
            Daemon.hideSplashScreen();


            // hm: if help is closed cache frame of normal is closed!
            if (cacheFrame_ != null && cacheFrame_.isVisible())
            {
               cacheFrame_.setVisible(false);
               cacheFrame_.setContentPane(new JPanel());
            }

            // double code (above)
            if (finishListener != null)
               finishListener.actionPerformed(
                  new ActionEvent(GlobalMasterAndUI.this, 0, "fertig"));
            loadingDocument_ = false;
         } // run
      };
      loadThread_.start();
   }

   public void activateDocument(String location)
   {
      Frame frame = null;
      if (mainMenuBar_ != null && mainMenuBar_.getDocument() != null)
      {
         String mainLocation = mainMenuBar_.getDocument().getLocation();
         if (mainLocation.equals(location))
         {
            frame = mainFrame_;
         }
      }
      else if (helpMenuBar_ != null && helpMenuBar_.getDocument() != null)
      {
         String helpLocation = helpMenuBar_.getDocument().getLocation();
         if (helpLocation.equals(location))
         {
            frame = helpFrame_;
         }
      }

      if (frame != null)
      {
         frame.toFront();
      }

   }

   public void closePerformed()
   {
   }

   public Document getDocumentIfLoaded(String location)
   {
      // check if this file is maybe located in the cache
      CacheBackend cache = createCacheBackend(location, null, false, false);
      if (cache != null && cache.contains(location))
         location = cache.getFileInCache(location)+"";

      Document loadedDocument = (Document)currentDocuments_.get(location);
      return loadedDocument;
   }


   // invoked only by MyWindowListener.windowClosing() && GlobalMenuBar."exit"
   // && PopupMenu."close" && Document.requestClose()
   // so closing without another document being loaded...
   public void closeDocument(Document document)
   {
      if (document != null)
      {
          if (!document.isClosed())
             document.close();

          if (mainMenuBar_ != null && mainMenuBar_.getDocument() == document)
          {
             mainMenuBar_.setDocument(null);
             mainFrame_.setVisible(false);
             if (mainFrame_.getContentPane().getComponentCount() > 0)
                mainFrame_.getContentPane().removeAll();
          }
          else if (helpMenuBar_ != null && helpMenuBar_.getDocument() == document)
          {
             helpMenuBar_.setDocument(null);
             helpFrame_.setVisible(false);
             if (helpFrame_.getContentPane().getComponentCount() > 0)
                helpFrame_.getContentPane().removeAll();
             
          }

          
          currentDocuments_.remove(document.getLocation());
          

          closeCacheControl(document);
      }
   	if (daemon_ != null && Document.getInstanceCount() == 0) daemon_.shutDown();
      if (daemon_ != null && dummyVisible_ && Document.getInstanceCount() == 1) daemon_.shutDown();
   }


   public void showCacheDialog(String location)
   {
      showCacheDialog(location, false);
   }

   public void showCacheDialog(String location, boolean settingsOnly)
   {
      Document d = (Document)currentDocuments_.get(location);
      Window w = mainFrame_;
      if (d != null)
         w = SwingUtilities.windowForComponent(d.getVisualComponent());
            

      CacheBackend cache = createCacheBackend(location, w, true, false);
      if (cache == null)
         return;


      if (displayedCacheControls_ == null)
         displayedCacheControls_ = new HashMap();

      

      CacheControl cc = new CacheControl(cache, location);
     
      
      
      if (settingsOnly)
         cc.showSettingsOnly();
      else
      {
         // this gets either replaced by a new instance (here)
         // or deleted when the appropriate document is closed (closeDocument())
         displayedCacheControls_.put(location, cc);
      
         cc.showYourself();
      }
   }

   public void restoreLayout(Document document)
   {
      long seconds = (System.currentTimeMillis()/1000)%60;
      //System.out.println(seconds+" restoreLayout()");

      // BUGFIX #2678
      // Also fullscreen mode should be ended.
      document.switchToFullScreen(true, true); // end it if active
      
      document.restoreVideoSize(); // if it has one


      JFrame frame = null;
      if (helpFrame_ != null && quickLocation_ != null 
          && quickLocation_.equals(document.getLocation()))
      {
         frame = helpFrame_;
      }
      else if (mainFrame_ != null && normalLocation_ != null 
               && normalLocation_.equals(document.getLocation()))
      {
         frame = mainFrame_;
      }
      // Gibt es hier ein Problem mit dem Dummy-Fenster?
      // Nein 1, denn beim Menü ist dieser Punkt dann deaktiviert
      // Nein 2, denn bei einem Tastatur-Event ist es nicht interessant.

      if (frame != null)
      {
         frame.pack();
               
         LayoutUtils.clipWindowToScreen(frame);
      }
   }
   

   public int getStandardNavigationState()
   {
      return iNavigationState_;
   }
   
   public void setStandardNavigationState(int iState)
   {
      iNavigationState_ = iState;
   }
   
   private boolean isHelpDocument(String location)
   {
      boolean isHelp = false;
      try {
         isHelp = FileResources.isHelpDocument(location);
      } catch (IOException e) { }

      return isHelp;
   }

   // 
   // starts a new Document (and returns that)
   // closes the old document
   // and updates status structure currentDocuments_
   // 
   private synchronized Document startupAndRemove(
      String newLocation, String originalLocation, int time, String oldLocation)
   {
      Document d = null;
      try
      {
         d = new Document(newLocation, originalLocation, time, this);
      }
      catch (IllegalAccessException e)
      {
         return null; // security check failed
      }
      catch (InstantiationException exc)
      {
         return null; // helper or similar failed
      }
      /* others are not thrown
      catch (Throwable e)
      {
         Window w = Manager.getLoadWindow();
         if (w != null)
            Manager.putLoadWindow(null);

         Manager.showError(w, Manager.getLocalized("unexpectedError"), Manager.ERROR, e);
         return null;
      }
      */
      

      if (oldLocation != null)
      {
         Document d2 = (Document)currentDocuments_.remove(oldLocation);
         if (d2 != null)
         {
            //System.out.println("closing Document "+d2);
            d2.close();

            closeCacheControl(d2);
         }
      }

     currentDocuments_.put(newLocation, d);

      return d;
   }

   private synchronized void putInFrameAndShow(Document document, boolean isQuick)
   {
      String frameTitle = Manager.productString;
      String strDocumentTitle = document.GetWindowTitle(frameTitle);
      if (strDocumentTitle != null && strDocumentTitle.length() > 0)
         frameTitle = strDocumentTitle;

      URL loc = "".getClass().getResource("/imc/epresenter/player/icons2/LECTURNITY_Player.png");
       ImageIcon ii = new ImageIcon(loc);
       Image frameIcon = null;
       if (ii != null)
           frameIcon = ii.getImage();
       else
           System.err.println("GlobalMasterAndUI: ImageIcon is null.");
      

      // special case for dummy window visible or quick document to load
      if (currentDocuments_.size() == 0)
         dummyVisible_ = true;
      else if (dummyVisible_)
      {
         Document.decrementCounter("special case for dummy close");
         dummyVisible_ = false;

         if (isQuick)
            mainFrame_.dispose();
      }

      

      JFrame frame;

      if (!isQuick)
      {
         if (mainFrame_ == null)
         {
            mainFrame_ = new JFrame();
            mainFrame_.setIconImage(frameIcon);
            if (iNavigationState_ != Document.NAV_STATE_HIDDEN)
            {
               mainMenuBar_ = new GlobalMenuBar(this, document);
               mainFrame_.setJMenuBar(mainMenuBar_);
            }
            //mainFrame_.addKeyListener(new MyKeyListener("main"));
            mainFrame_.addWindowListener(new MyWindowListener("main"));
            
            Manager.registerWindow(mainFrame_);
         }

         frame = mainFrame_;
         if (iNavigationState_ != Document.NAV_STATE_HIDDEN)
            mainMenuBar_.setDocument(document);
      }
      else
      {
         if (helpFrame_ == null)
         {
            helpFrame_ = new JFrame();
            helpFrame_.setIconImage(frameIcon);
            if (iNavigationState_ != Document.NAV_STATE_HIDDEN)
            {
               helpMenuBar_ = new GlobalMenuBar(this, document);
               helpFrame_.setJMenuBar(helpMenuBar_);
            }
            //helpFrame_.addKeyListener(new MyKeyListener("help"));
            helpFrame_.addWindowListener(new MyWindowListener("help"));
            
            Manager.registerWindow(helpFrame_);
         }

         frame = helpFrame_;
         if (iNavigationState_ != Document.NAV_STATE_HIDDEN)
            helpMenuBar_.setDocument(document);
      }
         
      if (frame.getContentPane().getComponentCount() > 0)
         frame.getContentPane().removeAll();

      frame.getContentPane().add("Center", document.getVisualComponent());

      //frame.setContentPane(document.getVisualComponent());
      frame.setTitle(frameTitle);
      frame.pack();
      LayoutUtils.clipWindowToScreen(frame);

      if (frame.isVisible())
      {
         centerFrame(frame);
         frame.toFront();
      }
      else
      {
         centerFrame(frame);
         frame.setVisible(true);
      }

      // make something "innocent" have the focus after (re-)load
      document.getVisualComponent().requestFocus();

      // for video display:
      document.resetTime();
      
   }

   // invoked by startupAndRemove()
   // and closeDocument() to close a cache control window
   private void closeCacheControl(Document d)
   {
      if (displayedCacheControls_ != null)
      {
         String location = d.getLocation();
         CacheControl cc = (CacheControl)displayedCacheControls_.get(location);
         if (cc != null)
         {
            displayedCacheControls_.remove(location);
            Window w = SwingUtilities.windowForComponent(cc);
            if (w != null)
               w.dispose();
         }
      }
   }
   
   private void centerFrame(Window target)
   {
      Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
      centerFrame(target, new Rectangle(0,0, d.width, d.height));
   }

   private void centerFrame(Window target, Rectangle source)
   {
      Dimension d = target.getSize();
      int diffX = source.width-d.width;
      int diffY = source.height-d.height;
      target.setLocation(source.x+diffX/2, source.y+diffY/2);
   }


   private String checkLocation(String location)
   {
      if (!(location.endsWith("lpl") || location.endsWith("lpd")))
         return location; // eg for lrds

      // done on behalf of this parent window
      Window parent = Manager.getLoadWindow();
      

      //
      // the default here is always the original location
      // except for urls, there the (error) default is null 
      //
      if (location.endsWith(".lpl"))
      {
         // double code (Manager.main())
         InputStream in = null;
         try {
            if (location.toLowerCase().startsWith("http"))
               in = new BufferedInputStream(new URL(location).openStream()); 
            else
               in = new BufferedInputStream(new FileInputStream(location));

            Properties props = new Properties();
            props.load(in);


            String offset = props.getProperty("startMillis", "");
            String lecture = props.getProperty("url");
            if (lecture == null)
            {
               throw new IOException("No document specified in the lpl file.");
            }

            // to do: use this later on
            String length = props.getProperty("docSizeBytes", "-1");
            System.setProperty(lecture, length);

            location = lecture;
         }
         catch (IOException e)
         {
            String message = "";
            if (e instanceof FileNotFoundException)
               message = Manager.getLocalized("fileNotFound1")+" "+Manager.getLocalized("fileNotFound2");

            Manager.showError(parent, message, Manager.WARNING, e);
            // to do:
            // useful error handling... ?
            return null;
         }
         finally
         {
            try
            {
               if (in != null)
                  in.close();
            }
            catch (IOException exc)
            {
            }
         }

      }




      CacheBackend cache = createCacheBackend(location, parent, true, true);
      if (cache == null)
         return location;

      
      if (location.toLowerCase().startsWith("http"))
      {
         URL url = null;
         try
         {
            url = new URL(location);
            //System.out.println(url.getHost());
            url.openConnection();
         }
         catch (MalformedURLException exc)
         {
            Manager.showError(parent, Manager.getLocalized("badUrl")+"\n("+location+")",
                              Manager.WARNING, exc);
            return null;
         }
         catch (IOException exc)
         {
            Manager.showError(parent, Manager.getLocalized("urlDownloadFail")
                              +"\n("+location+")",
                              Manager.WARNING, exc);
            return null;
         }

         if (!cache.contains(location))
         {
            return assesSpaceSufficientAndCopy(cache, location);
         }
         else // in cache
         {
            return cache.getFileInCache(location)+"";
         }
      }








      int millis = Document.determineLength(location, parent);
      if (millis == -1)
         // input file (location) corrupt, error already displayed ?
         return null;

      File outCache = new File(location);
      File inCache = cache.getFileInCache(location);
      
      long neededBytes = (outCache.length()*1000)/millis; // bytes per second

      Daemon.setSplashText(Manager.getLocalized("speedTest"));

      //long outSpeed = neededBytes/5+(long)(Math.random()*neededBytes);
      long outSpeed = SpeedCheck.testSpeedOf(outCache+"");
      long inSpeed = -1;
      
      
      if (inCache != null && inCache.exists()) // try to use that
      {
         //inSpeed = neededBytes/5+(long)(Math.random()*2*neededBytes);
         inSpeed = SpeedCheck.testSpeedOf(inCache+"");

         Daemon.resetSplashText();

         if (inSpeed >= neededBytes)
            return inCache+"";
         else if (outSpeed < neededBytes)
         {
            //Daemon.stopSplashJostling();
            // just in case that there is a splash screen "on top"

            // show and handle cache too slow dialog
            int result = CacheDialogs.showCacheTooSlowDialog(cache);
            
            if (result == JOptionPane.YES_OPTION)
            {
               CacheControl cc = new CacheControl(cache, location);
               cc.showSettingsOnly();
               // cacheChanged = true ???
            }
            else if (result == JOptionPane.NO_OPTION)
               return location;
            else if (result == JOptionPane.CANCEL_OPTION)
               return null;

         }
         //else the original location is returned (see bottom)
      }
      else
         Daemon.resetSplashText();

      
      
      if (outSpeed < neededBytes) // || cacheChanged)
      {
         
         //Daemon.stopSplashJostling();
         // just in case that there is a splash screen "on top"
         

         // show do you want to cache dialog
         int result1 = JOptionPane.OK_OPTION;
         result1 = CacheDialogs.showCacheNowDialog(cache, location);
         while (result1 == JOptionPane.OK_OPTION || result1 == JOptionPane.CLOSED_OPTION)
         {
            if (result1 == JOptionPane.OK_OPTION)
            {
               String result =  assesSpaceSufficientAndCopy(cache, location);
               if (result != null)
                  return result;
            }
            else // CLOSED_OPTION means show Help
            {
               final Object notifyTarget = new Object();

               final JFrame frame = Manager.requestFrame();
               frame.setTitle(Manager.getLocalized("cacheHelp"));

               WindowListener wl = new WindowAdapter() {
                  public void windowClosing(WindowEvent evt)
                  {
                     ((JFrame)evt.getSource()).removeWindowListener(this);

                     synchronized (notifyTarget)
                     {
                        notifyTarget.notify();
                     }
                  }
               };

               frame.addWindowListener(wl);


               final JButton button = new JButton();
               //button.setFocusPainted(false);
               button.setText("      OK      ");
               //button.setBorder(BorderFactory.createEmptyBorder(6,6,6,6));
               //button.setMaximumSize(new Dimension(500, Integer.MAX_VALUE));

               JLabel label = new JLabel(Manager.getLocalized("cacheHelpText"));
               label.setBorder(BorderFactory.createEmptyBorder(6,6,6,6));
               //label.setFont(label.getFont().deriveFont(12.0f));

               JPanel p = new JPanel(new BorderLayout());
               p.add("Center", label);
               JPanel p2 = new JPanel(new FlowLayout());
               p2.add(button);
               p.add("South", p2);
               //p.setMaximumSize(new Dimension(500, Integer.MAX_VALUE));
               
               //System.out.println(p.getPreferredSize().width);

               ActionListener al2 = new ActionListener() {
                  public void actionPerformed(ActionEvent evt)
                  {
                     frame.dispose();
                     
                     synchronized (notifyTarget)
                     {
                        notifyTarget.notify();
                     }
                  
                  }
               };

               button.addActionListener(al2);


               frame.setContentPane(p);
               //frame.pack();
               frame.setSize(new Dimension(500,220));




               LayoutUtils.centerWindow(frame);
               frame.setVisible(true);





              synchronized(notifyTarget)
              {
                 try { notifyTarget.wait(); } catch (InterruptedException e) {}
              }
            }

            //System.out.println("showDialog");

            result1 = CacheDialogs.showCacheNowDialog(cache, location);
         }
         
         if (result1 == JOptionPane.CANCEL_OPTION)
         {
            return null;
         }
         // else (NO) the original location is returned (see below)

      }


      return location;
   }


   private CacheBackend createCacheBackend(String location, Window parent, 
                                           boolean displayErrors, 
                                           boolean checkAlreadyWarned)
   {
   
      CacheBackend cache = null;
      try
      {
         cache = new CacheBackend(this, location);
      }
      catch (IOException exc)
      {
         // this happens due to inability to use the found config file
         // eg not write or read failure
         
         if (displayErrors)
         {
            if (checkAlreadyWarned && !alreadyWarned_)
            {
               Manager.showError(parent, Manager.getLocalized("settingsFail"),
                                 Manager.WARNING, exc);
               alreadyWarned_ = true;
            }
         }
      }

      if (cache != null)
      {
         try
         {
            cache.prepareCacheDirectory();
         }
         catch (IOException e)
         {
            if (displayErrors)
            {
               Manager.showError(parent, Manager.getLocalized("cacheDirFail"),
                                 Manager.WARNING, e);
            }

            try
            {
               cache.getSettings().setLocation("TEMP");
            }
            catch (IOException exc)
            {
            }
         }
      }

      return cache;
   }


   private String assesSpaceSufficientAndCopy(CacheBackend cache, String location)
   {
      ActionListener al = new ActionListener() {
         public void actionPerformed(ActionEvent e)
         {
            //System.out.println("notify");
            synchronized(GlobalMasterAndUI.this) { GlobalMasterAndUI.this.notify(); }
         }
      };
         
      int result2 = JOptionPane.OK_OPTION;
      if (!cache.cacheAndDeviceSufficient(location))
      {
         result2 = CacheDialogs.showCacheTooSmallDialog(cache, true);
         while (result2 == JOptionPane.OK_OPTION)
         {
            CacheControl cc = new CacheControl(cache, location);
            cc.showSettingsOnly();

            if (cache.cacheAndDeviceSufficient(location))
               break;

            result2 = CacheDialogs.showCacheTooSmallDialog(cache, true);
         }

         if (result2 == JOptionPane.CANCEL_OPTION)
            return null;

         // else (NO) the original location is returned
         if (result2 == JOptionPane.NO_OPTION)
            return location;
      }

      int result3 = JOptionPane.OK_OPTION;
      if (!cache.canTake(location))
      {
         result3 = CacheDialogs.showCacheTooFullDialog(cache, true);
         while (result3 == JOptionPane.OK_OPTION)
         {
            CacheControl cc = new CacheControl(cache, location);
            cc.showSettingsOnly();

            if (cache.canTake(location))
               break;

            result3 = CacheDialogs.showCacheTooFullDialog(cache, true);
         }

         if (result3 == JOptionPane.CANCEL_OPTION)
            return null;

         // else (NO) the original location is returned
         if (result2 == JOptionPane.NO_OPTION)
            return location;
      }

      // double code (above)
      CacheDialogs.CopyStub cs = new CacheDialogs.CopyStub(
         cache, location, al, false);

      if (copyWindow_ == null)
      {
         copyLabel_ = new JLabel(
            Manager.getLocalized("copying")+" "
            +location+" "
            +Manager.getLocalized("toCache")+"...");

         JPanel inner = new JPanel(new BorderLayout());
         inner.setBorder(BorderFactory.createEmptyBorder(6,6,6,6));
         LayoutUtils.addBorder(inner, 
                               BorderFactory.createLineBorder(Color.black, 1));

         inner.add("North", copyLabel_);
         inner.add("South", cs);

         copyWindow_ = new JWindow();
         copyWindow_.setContentPane(inner);
         copyWindow_.pack();
         centerFrame(copyWindow_);
      }
      else
      {
         copyLabel_.setText(
            Manager.getLocalized("copying")+" "
            +location+" "
            +Manager.getLocalized("toCache")+"...");
         copyWindow_.pack();
         centerFrame(copyWindow_);
      }
      
      copyWindow_.setVisible(true);

      cs.start();

      synchronized(this) { try { wait(); } catch (InterruptedException e) {} }

      //System.out.println("wait finished");

      copyWindow_.setContentPane(new JPanel());
      copyWindow_.setVisible(false);
      //System.out.println("Wumpfy");
      copyWindow_.dispose();

      // test if copy is ok or canceled
      if (cache.contains(location))
         return cache.getFileInCache(location)+"";
      else
         return null;
      //if (!cs.isCanceled()) // there is a difference between cancel and error!
      // else continue loop
   }



   private class MyWindowListener extends WindowAdapter
   {
      private String identifier_;

      public MyWindowListener(String identifier)
      {
         identifier_ = identifier;
      }

      public void windowClosing(WindowEvent e)
      {
         Document document = null;
         if (identifier_.equals("help"))
         {
            document = (Document)currentDocuments_.get(quickLocation_);
            // frame references are kept, thus commented:
            //helpFrame_.removeWindowListener(this); 
         }
         else
         {
            document = (Document)currentDocuments_.get(normalLocation_);
            //mainFrame_.removeWindowListener(this);
         }


         closeDocument(document);

         // closeDocument does nothing if only the dummy window is visible
         if (dummyVisible_)
            System.exit(0);

      }
   }
}