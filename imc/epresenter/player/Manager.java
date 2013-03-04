package imc.epresenter.player;

import java.awt.*;
import java.awt.event.*;
import java.awt.font.TextAttribute;
import java.io.*;
import java.net.URL;
import java.util.*;
import java.text.SimpleDateFormat;
import java.text.ParseException;
import javax.swing.*;
import javax.swing.filechooser.FileFilter;
import javax.swing.plaf.ColorUIResource;


import imc.epresenter.player.util.MemoryMeter;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.ui.LayoutUtils;

/**
 */
public class Manager {
   private static PrintStream m_debugFile;
   public static void DebugMsg(String strMessage)
   {
      /*
      try{
         if (null == m_debugFile)
            m_debugFile = new PrintStream(new FileOutputStream("c:\\autorun_p_debug.txt"));
         m_debugFile.println(strMessage);
      } catch (IOException exc) { exc.printStackTrace(); }
      //*/
      
      //System.out.println(strMessage);
   }
   
   public static String versionNumber = "4.5.0";
   // NOTE: The "gui version" here should not reflect the patch level!
   //       This is what is checked against the registry.
   
   public static String internalBeta = "1";
   public static String versionString = "LECTURNITY Player "+versionNumber;
   static
   {
      if (NativeUtils.isLibraryLoaded())
      {
         String registryVersionString = NativeUtils.getVersionString(internalBeta);
         if (!(registryVersionString.indexOf("?") > -1)
             && registryVersionString.startsWith(versionNumber))
            versionString = "LECTURNITY Player "+registryVersionString;
         // else: could be there is nothing in the registry; ie if cd start
         // or: the installed version is older; then also the one on the cd is used
      }
      
      System.setProperty("nyckel.musik", "forte");
   }
   public static String productString = "Player";
	
	public static final int WARNING = JOptionPane.WARNING_MESSAGE;
	public static final int ERROR = JOptionPane.ERROR_MESSAGE;
	
   private static ArrayList framePool = new ArrayList(4);
   private static HashMap dialogHash = new HashMap(6);
   private static Image frameIcon;
   private static HashMap htmlFrames_ = new HashMap();
   private static HashMap locationTitleMap_ = new HashMap();
   private static Properties m_UiTexts;
   private static JFileChooser chooser;
   private static File lastLoadDir;
   private static boolean lookAndFeelSet;
   private static Font commonFont;

	
   static
   {
      //UIManager.put("Button.font", Manager.getCommonFont());
      //UIManager.put("Label.font", Manager.getCommonFont());

      //imc.lecturnity.util.ColorManager.setImcColors();
      ColorUIResource orangeResource = new ColorUIResource(new Color(0xf7c9a7));
      Color scrollBC = new JButton().getBackground();
      ColorUIResource whiteResource = new ColorUIResource(Color.white);
      ColorUIResource scrollResource = new ColorUIResource(
         new Color((scrollBC.getRed()+255)/2, 
                   (scrollBC.getGreen()+255)/2, 
                   (scrollBC.getBlue()+255)/2));
      UIManager.put("ScrollBar.background", scrollResource); // funktioniert bei Metal
      if (isWindows())
         UIManager.put("ScrollBar.track", scrollResource); // funktioniert bei Win L&F
      UIManager.put("Table.background", whiteResource); // funktioniert bei Metal
      //UIManager.put("ScrollPane.background", whiteResource); // funktioniert bei Metal
      

      /* bunny's present */
      GregorianCalendar cal = new GregorianCalendar();
      int hour = cal.get(Calendar.HOUR_OF_DAY);
      if (hour > 1 && hour < 4)
         UIManager.put("control", orangeResource);
      
      URL loc = "".getClass().getResource("/imc/epresenter/player/icon_player.gif");
      if (null != loc)
         frameIcon = new ImageIcon(loc).getImage();
      

   }

   


	
	public static void main(String[] args) {
      try {
         if (!lookAndFeelSet)
         {
            if (isWindows())
               UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            lookAndFeelSet = true;
         }
      } catch (Exception various) {
         various.printStackTrace();
      }
      
      //MemoryMeter mm = new MemoryMeter(true);

      /*
      String message = "";
      for (int i=0; i<args.length; ++i)
         message += args[i]+"\n";
      JOptionPane.showConfirmDialog(null, message);

      if (args.length > 1)
      {
         String strMessage = "";
         strMessage += args[1];
         strMessage += "\n";
         for (int i=0; i<args[1].length(); ++i)
            strMessage += ((int)args[1].charAt(i))+" ";
         
         System.out.println(strMessage);
         JOptionPane.showConfirmDialog(null, strMessage);
      }
      */
      
      String strEnvParameters = System.getenv("LECTURNITY_PARAMETERS");
      if (strEnvParameters != null && strEnvParameters.length() > 1)
      {
         // javaw.exe cannot handle unicode characters: use the environment
         // variable instead as args
         
         args = FileUtils.SplitCommandLine(strEnvParameters);
      }
      
      if (NativeUtils.failWithLiveContextInstallation(getLocalized("liveContextDirect")))
          System.exit(-1);
      // If it is a LIVECONTEXT installation the Player (or any direct start) will not start
      
      
      //if (args.length > 0) System.out.println("found first option _"+args[0]+"_");
		if (args.length == 1 && args[0].equals("-daemon")) {
			// special case: start as background process
			Daemon d = new Daemon();
			d.start();
			try { d.join(); } catch (InterruptedException e) {}
		} else if (args.length == 1 && args[0].equals("-help")) {
         // wird nur in Daemon und nur beim Start überprüft
         System.setProperty("html.display", "true");
         
         
         Daemon d = new Daemon(false);
			d.start();
         String helpPath = System.getProperty("help.path");
			showHtmlFrame(d.getMaster(), Manager.getLocalized("onlineHelp"), helpPath); 
			try { d.join(); } catch (InterruptedException e) {}
		} else if (args.length == 1 && args[0].toLowerCase().endsWith(".lpl")) {
			// special case for lpl files
         // double code (GlobalMasterAndUI.checkLocation())
			try {
            InputStream in = null;
            if (args[0].toLowerCase().startsWith("http"))
               in = new BufferedInputStream(new URL(args[0]).openStream()); 
            else
               in = new BufferedInputStream(new FileInputStream(args[0]));

            Properties props = new Properties();
            props.load(in);


            String offset = props.getProperty("startMillis", "");
            String lecture = props.getProperty("url");
            if (lecture == null)
            {
               System.err.println("No document specified in the lpl file.");
               System.exit(1);
            }

            // to do: use this later on
            String length = props.getProperty("docSizeBytes", "0");

            try
            {
               if (in != null)
                  in.close();
            }
            catch (IOException exc)
            {
            }
         

				main(new String[] { "-time", offset.trim(), lecture });
			} catch (IOException e) {
				e.printStackTrace();
				System.exit(1);
			}
		} else if (args.length == 1 && args[0].toLowerCase().endsWith(".html")
                 || args.length == 2 && args[0].toLowerCase().equals("-load") && args[1].toLowerCase().endsWith(".html")) {
         // gets only checked in Daemon during start:
         System.setProperty("html.display", "true");
         
         DebugMsg("Staring Daemon with html argument...");
         
         String strHtmlName = args[0];
         if (args.length == 2)
            strHtmlName = args[1];
         
         Daemon d = new Daemon(false, strHtmlName);
			d.start();
         // show start screen of cd export
         String title = Manager.getLocalized("lecturnityCD");
         
         if (strHtmlName.toLowerCase().indexOf("player") != -1 &&
             strHtmlName.toLowerCase().indexOf("help") != -1)
            title =  Manager.getLocalized("onlineHelp");
         
         try
         {
            // try to find the title in the html file; only accept files with
            // utf-8 encoding (as "Content Type")
            //
            
            FileInputStream fis = new FileInputStream(strHtmlName);
            BufferedReader in = FileUtils.createBufferedReader(fis, "utf-8", false, null);
            
            boolean bContentTypeFound = false;
            String strLine = in.readLine();
            while (strLine != null)
            {
               strLine = strLine.trim();
               String strLineLow = strLine.toLowerCase();
               
               if (strLineLow.startsWith("<meta http-equiv=\"content-type\""))
               {
                  int idx = strLineLow.indexOf("charset=");
                  if (idx > -1)
                  {
                     int idx2 = strLineLow.indexOf("\"", idx + 9); // after charset=
                     if (idx2 > -1)
                     {
                        String strEncoding = strLineLow.substring(idx + 8, idx2);
                        if (strEncoding.equals("utf-8"))
                           bContentTypeFound = true;
                        else
                           break; // another encoding not supported
                     }
                  }
               }
               
               if (strLineLow.startsWith("<title>"))
               {
                  if (bContentTypeFound)
                  {
                     int idx2 = strLineLow.indexOf("<", 7);
                     if (idx2 > -1)
                        title = strLine.substring(7, idx2);
                  }
                  // else ignore title before or without content type
                  
                  break; // only look for title
               }
               
               strLine = in.readLine();
            }
            
            
            in.close();
            fis.close();
         }
         catch (IOException exc)
         {
            // ignore; keep standard title
         }
         
         DebugMsg("Showing html argument...");
         
			if (showHtmlFrame(d.getMaster(), title, strHtmlName))
         {
            // after loading is finished (only does something on first loading
            // and if it is no eval/expired version):
            //Daemon.hideSplashScreen();

            try { d.join(); } catch (InterruptedException e) {}

         }
         else
            System.exit(1);

      } else if (args.length == 2 && args[0].toLowerCase().endsWith(".html") && args[1].toLowerCase().endsWith(".lpd")) {
         // tutorial mode ....
         // double display: html AND lpd
        
         // wird nur in Daemon und nur beim Start überprüft
         System.setProperty("html.display", "true");
         
         // double code (below)
         String location = args[1];
         File f = new File(location);
         if (!location.toLowerCase().startsWith("http"))
         {
            location = f.getAbsolutePath();
            try { lastLoadDir = new File(location).getCanonicalFile(); } catch (IOException ioe) {}
         }

         
         // used for automatic highlight of html frame after presentation end
         // and for the decision whether a document is to be shown in full screen or not
         //System.setProperty("tutorial.mode", args[1]);
         
         Manager.mapLocationToTitle(args[1], Manager.getLocalized("tutorial"));
         // TODO: mark tutorial.lpd to start in full screen??
         
         Daemon d = new Daemon(location, args[0], 0, false);
         
			d.start();
         
         showHtmlFrame(d.getMaster(), Manager.getLocalized("tutorial"), args[0]);
         
         d.getMaster().activateDocument(args[1]);

         //try { d.join(); } catch (InterruptedException e) {}
		
      } else {

			String location = null;
			int time = 0;
			boolean autoStart = false;
			

			if (args.length > 0 && !(new File(args[0]).isDirectory())) { // start with parameters
				if (args[0].equals("-h")) {
					printUsageAndExit(0);
				} else if (new File(args[args.length-1]).isFile()) { // normal case
               // double code (below)
               for (int i=0; i<args.length-1; i++) {
						if (args[i].equals("-time")) {
							time = Integer.parseInt(args[++i]);
						} else if (args[i].equals("-start")) {
							autoStart = true;
						}
					}
					location = args[args.length-1];
            } else if (args[args.length-1].toLowerCase().startsWith("http"))
            {
               // double code (above)
               for (int i=0; i<args.length-1; i++) {
						if (args[i].equals("-time")) {
							time = Integer.parseInt(args[++i]);
						} else if (args[i].equals("-start")) {
							autoStart = true;
						}
					}
               location = args[args.length-1];
            } else {
               showError(Manager.getLocalized("fileNotFound1")
                         +" \""+args[args.length-1]+"\" "
                         +Manager.getLocalized("fileNotFound2"), Manager.ERROR, null);
               /*
					if (!new File(args[args.length-1]).exists()) {
						System.err.println(args[args.length-1]+" was not found.");
						printUsageAndExit(1);
					} else {
						System.err.println(args[args.length-1]+" is directory; no file.");
						printUsageAndExit(1);
					}
               */
				}
			} else { // start with FileChooser
            /*
				File startDir = lastLoadDir;
            if (args.length > 0 && new File(args[0]).isDirectory())
					startDir = new File(args[0]);
				location = showFileDialog(startDir);
            if (location == null) System.exit(0);
            */
			}

         if (location != null)
         {
            
            // double code (above)
            File f = new File(location);
            if (!location.toLowerCase().startsWith("http"))
            {
               location = f.getAbsolutePath();
               try { lastLoadDir = new File(location).getCanonicalFile(); } catch (IOException ioe) {}
            }
         }
         
			Daemon d = new Daemon(location, time, false);
			//if (autoStart) d.startReplay(); 
			d.start(); // make Daemon waiting for incoming commands
		} // no-ap (normal case)
	}

	private static void printUsageAndExit(int status) {
		System.out.println("usage: invoke with java"
						   +" imc.epresenter.player.Manager [-time <offset>] [-start] <config document> ");
		System.exit(status);
	}

   public static String mapLocationToTitle(String location)
   {
      Object o = locationTitleMap_.get(location);
      if (o != null)
         return (String)o;
      else
         return null;

   }

   public static void mapLocationToTitle(String location, String title)
   {
      locationTitleMap_.put(location, title);
   }

   public static void activateHtmlFrame(String title)
   {
      if (htmlFrames_.containsKey(title))
      {
         HelpDisplay frame = (HelpDisplay)htmlFrames_.get(title);
         
         if (!frame.isShowing())
         {
            Document.incrementCounter();

            frame.setVisible(true);
         }
         else
            frame.toFront();
      }
   }

   public static boolean showHtmlFrame(GlobalMasterAndUI master, String title, String htmlPath)
   {
      if (htmlFrames_.containsKey(title))
      {
         HelpDisplay frame = (HelpDisplay)htmlFrames_.get(title);
         try
         {
            frame.loadPage(new File(htmlPath).toURL());
         }
         catch (Exception e)
         {
            e.printStackTrace();
         }


         if (!frame.isShowing())
         {
            Document.incrementCounter();

            frame.setVisible(true);
         }
         else
            frame.toFront();
      }
      else
      {
         if (htmlPath == null || !new File(htmlPath).exists())
         {
            Manager.showError(Manager.getLocalized("htmlPathError1")
                              +" ("+htmlPath+") "+Manager.getLocalized("htmlPathError2"),
                              Manager.WARNING, null);
            return false;
         }
         
         
         final HelpDisplay frame = new HelpDisplay(htmlPath, master, title);
         Manager.putLoadWindow(frame);

         frame.setIconImage(frameIcon);
         
         // special case for FEST-AmI-DVD, in order to have better object placement
         File amiFile1 = new File(new File(htmlPath).getParentFile(), "8_Ostermann.lpd");
         File amiFile2 = new File(new File(htmlPath).getParentFile(), "6_Ludewig.lpd");
         if (amiFile1.exists() && amiFile2.exists())
            frame.setSize(frame.getSize().width+100, frame.getSize().height);
         
         frame.setVisible(true);

         Document.incrementCounter();

         frame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e)
            {
               Document.decrementCounter("windowClosing on htmlFrame");
               if (Document.getInstanceCount() == 0)
                  System.exit(0); // hm
            }
         });
     
         htmlFrames_.put(title, frame);

         frame.toFront();
      }

      return true;


   }

   static String showFileDialog()
   {
      return showFileDialog(null);
   }

   static String showFileDialog(Window parent)
   {
      String startDirString = ".";
      if (NativeUtils.isLibraryLoaded() && NativeUtils.getLecturnityHome() != null)
         startDirString = NativeUtils.getRecordingsPath();
      File startDir = new File(startDirString);
      try { startDir = startDir.getCanonicalFile(); } catch (IOException ioe) {}
      if (lastLoadDir != null)
         startDir = lastLoadDir;
      return showFileDialog(parent, startDir);
   }

   static String showFileDialog(Window parent, File startDir) {
	 	File choosen = null;
      if (chooser == null)
      {
         chooser = new JFileChooser(startDir);
         chooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
         chooser.setDialogTitle(getLocalized("open"));
         chooser.addChoosableFileFilter(new FileFilter() {
            public boolean accept(File f) {
               return f.isDirectory() || 
               f.getPath().toLowerCase().endsWith(".lpd") ||
               f.getPath().toLowerCase().endsWith(".lrd") ||
               f.getPath().toLowerCase().endsWith(".epf") ||
               f.getPath().toLowerCase().endsWith(".aof");
            }
            public String getDescription() {
               return getLocalized("filterDescription");
            }
         });

         Dimension d = chooser.getPreferredSize();
         d.width += 100;
         d.height += 50;
         chooser.setPreferredSize(d);
      }
      else
         chooser.setCurrentDirectory(startDir);
      
      while(choosen == null) {
         int val = chooser.showOpenDialog(parent);
			if (val == JFileChooser.APPROVE_OPTION) {
				choosen = chooser.getSelectedFile();
				if (choosen.isDirectory()) 
               return showFileDialog(parent, choosen);
			} else {
            return null;
			}
		}

      try { lastLoadDir = choosen.getParentFile().getCanonicalFile(); } catch (IOException e){}

		return choosen.getAbsolutePath();
	}

   static String showURLDialog(Window parent)
   {
      final JTextField inputField = new JTextField();
      inputField.setPreferredSize(
         new Dimension(443, inputField.getPreferredSize().height));
      JLabel comment = new JLabel(getLocalized("choseURL"));
      comment.setBorder(BorderFactory.createEmptyBorder(0,0,6,0));
      JButton commit = new JButton(getLocalized("load"));

      
      final JDialog dialog = requestDialog((Frame)parent, 
                                           Manager.getLocalized("openURL"), true);
      
      final Window parentI = parent;

      ActionListener al = new ActionListener() {
         public void actionPerformed(ActionEvent evt)
         {
            String url = inputField.getText().toLowerCase();
            if (url.endsWith(".lpd") || url.endsWith(".lpl"))
               dialog.dispose();
            else
               Manager.showError((Frame)parentI, 
                                 Manager.getLocalized("specifyLPD"), 
                                 Manager.WARNING, null);
         }
      };

      inputField.addActionListener(al);
      commit.addActionListener(al);

      JPanel inner = new JPanel(new GridLayout(1,1));
      inner.add(inputField);
      inner.setBorder(BorderFactory.createEmptyBorder(2,0,2,6));

      JPanel content = new JPanel(new BorderLayout());
      content.add("North", comment);
      content.add("Center", inner);
      content.add("East", commit);
      LayoutUtils.addBorder(content, BorderFactory.createEmptyBorder(6,6,6,6));

      dialog.setContentPane(content);
      dialog.pack();
      LayoutUtils.centerWindow(dialog);

      dialog.show(); //is blocking until closed

     
      dialog.setContentPane(new JPanel());
      
      dialog.dispose();
      
      //dialog.finalize();

      String url = inputField.getText().toLowerCase();
      boolean isLPD = url.endsWith(".lpd") || url.endsWith(".lpl");

      if (url.length() > 0 && isLPD)
         return inputField.getText();
      else
         return null;
         
   }

	public static String getLocalized(String key) {
      if (m_UiTexts == null) {
			try {
            String language = null;
            if (NativeUtils.isLibraryLoaded())
               language = NativeUtils.getLanguageCode();
            else
                language = System.getProperty("user.language", "en");
            
				InputStream textResource = key.getClass().getResourceAsStream(
					"/imc/epresenter/player/texts_"+language+".properties");
				if (textResource == null)
					textResource = key.getClass().getResourceAsStream(
						"/imc/epresenter/player/texts_en.properties");
				//texts = new PropertyResourceBundle(textResource);
            
            m_UiTexts = FileUtils.CreateProperties(textResource);

            if (textResource != null)
               textResource.close();
         
			} catch (IOException e) {
				throw new RuntimeException("Cannot load localization resource bundle ("+e+").");
			}
		}

		return m_UiTexts.getProperty(key);
	}
   
   public static void showError(String message, int level, Throwable e) {
      showError(null, message, level, e);
   }

	public static void showError(Window parent, String message, int level, Throwable e) {
      boolean bConsoleOperation = System.getProperty("mode.consoleoperation", "false").equals("true");
      
      if (!bConsoleOperation)
      {
         String title = getLocalized("warning");
         if (level == ERROR) {
            title = getLocalized("error");

            /* bunny's present */
            GregorianCalendar cal = new GregorianCalendar();
            int hour = cal.get(Calendar.HOUR_OF_DAY);
            if (hour > 2 && hour < 4)
               title = "Big badaboom!";
      
         }
		
         if (e != null) 
         {
            String exceptionClass = e.getClass().getName();
            if (exceptionClass.lastIndexOf(".") > -1)
               exceptionClass = exceptionClass.substring(exceptionClass.lastIndexOf(".")+1);
            String errorMessage = exceptionClass;
            if (e.getMessage() != null)
               errorMessage = exceptionClass+": "+e.getMessage();
         
            if (message.length() != 0)
               message += "\n"+errorMessage;
            else
               message = errorMessage;
         
            e.printStackTrace();
         }
		
         //Daemon.stopSplashJostling(); // just in case that there is a splash screen "on top"
         if (parent != null)
            parent.toFront();
         JOptionPane.showMessageDialog(parent, message, title, level);

         if (level == ERROR && Document.getInstanceCount() == 0)
            System.exit(1);


         /*
         PrintWriter writer = new PrintWriter(new StringWriter());
         e.printStackTrace(writer);
         String errorString = writer.toString();
         */
      }
      else // bConsoleOperation
      {
         //
         // this is unnecessary and will normally not be reached:
         //
         // Before executing this static method the static constructor will
         // be run. This constructor will execute the static block at
         // the beginning of this class. And this static block in turn 
         // invokes methods (e.g. "new JButton()") which are not 
         // available when there is no graphics display ( == bConsoleOperation).
         //
         
         System.err.println(message);
         if (e != null)
            e.printStackTrace();
      }
	}

   public static Font getCommonFont()
   {
      if (commonFont != null)
         return commonFont;

      boolean bConsoleOperation = System.getProperty("mode.consoleoperation", "false").equals("true");
      
      Font font = null;
		
      String ttfName = "arial.ttf";
      URL fontInput = ttfName.getClass().getResource(
         "/imc/epresenter/filesdk/fonts/"+ttfName);
		if (fontInput == null) {
			if (!bConsoleOperation) // retrieval does not need fonts...
            System.err.println("M: could not find desired resource ("+ttfName+").");
		} else {
			try {
            // doesn't delete the temporarily created font file
				//font = Font.createFont(Font.TRUETYPE_FONT, fontInput.openStream());
            
            String familyName = "Arial";
            HashMap attributeMap = new HashMap();
            attributeMap.put(TextAttribute.FAMILY, familyName);
            InputStream stream =  fontInput.openStream();
            font = imc.epresenter.player.whiteboard.VisualComponent.createTemporaryFont(
               stream, familyName, attributeMap);
            stream.close();
            if (font != null)
               font = font.deriveFont(12.0f);
			} catch (FontFormatException ffe) {
				System.err.println("M: not a valid truetype font ("+ttfName+").");
				ffe.printStackTrace();
			} catch (IOException ioe) {
				System.err.println("M: cannot load desired font ("+ttfName+").");
				ioe.printStackTrace();
			}
		}
      
      if (font == null && !bConsoleOperation) font = new Font("Arial", Font.PLAIN, 12);
		
      commonFont = font;

      return font;
   }

   public static boolean isWindows()
   {
      return File.separator.equals("\\");
   }

   public static JFrame requestFrame()
   {
      
      if (framePool.isEmpty())
      {
         URL loc = "".getClass().getResource("/imc/epresenter/player/icon_player.gif");
         if (null != loc)
            frameIcon = new ImageIcon(loc).getImage();
      

         JFrame f = new JFrame();
         f.setIconImage(frameIcon);
         f.addWindowListener(new MyWindowAdapter(framePool));
         framePool.add(f);
      }

      JFrame frame = (JFrame)framePool.remove(framePool.size()-1);
		
      return frame;
   }

   public static JDialog requestDialog(Window parent, String title, boolean modal)
   {
      if (dialogHash.get(parent) == null)
      {
         JDialog d = null;
         if (parent == null)
            d = new JDialog();
         else
         {
            if (parent instanceof Frame)
               d = new JDialog((Frame)parent);
            else 
               d = new JDialog((Dialog)parent);
         }
         d.addWindowListener(new MyWindowAdapter(dialogHash));
         dialogHash.put(parent, d);
      }

      
      JDialog d = (JDialog)dialogHash.remove(parent);
      d.setTitle(title);
      d.setModal(modal);

      return d;
      
      /*
      if (parent instanceof Frame)
         return new JDialog((Frame)parent, title, modal);
      else
         return new JDialog((Dialog)parent, title, modal);
         */
   }


   static Window handle_;
   static Window[] alternateHandles_ = new Window[2];
   
   public static void registerWindow(Window w)
   {
      for (int i=0; i<alternateHandles_.length; ++i)
      {
         if (alternateHandles_[i] == null || alternateHandles_[i] == w)
         {
            alternateHandles_[i] = w;
            break;
         }
      }
   }
   
   public static void putLoadWindow(Window w)
   {
      handle_ = w;
   }

   public static Window getLoadWindow()
   {
      if (handle_ == null)
      {
         // it would be nice to return a window anyway if there is one
         // so that modal dialogs have a valid parent!
         for (int i=0; i<alternateHandles_.length; ++i)
         {
            if (alternateHandles_[i] != null && alternateHandles_[i].isVisible())
            {
               handle_ = alternateHandles_[i];
               break;
            }
         }
      }
      
      return handle_;
   }


   private static class MyWindowAdapter extends WindowAdapter
   {
      private Object barn;

      MyWindowAdapter(Object source)
      {
         barn = source;
      }

      public void windowClosed(WindowEvent e)
      {
         Window w = (Window)e.getSource();
         if (w instanceof JFrame)
            ((JFrame)w).setContentPane(new JPanel());
         else if (w instanceof JDialog)
            ((JDialog)w).setContentPane(new JPanel());
         w.dispose();
         w.removeWindowListener(this);
         if (barn instanceof ArrayList)
            ((ArrayList)barn).add(w);
         else if (barn instanceof HashMap)
            ((HashMap)barn).put(((JDialog)w).getOwner(), w);
      }
   }
}