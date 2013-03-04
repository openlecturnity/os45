package imc.epresenter.player;

import java.awt.*;
import java.awt.event.*;
import java.awt.print.*;
import java.io.*;
import java.net.URL;
import java.text.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

import imc.epresenter.player.Document;
import imc.epresenter.player.util.TimeFormat;
import imc.epresenter.player.util.FloatNativeComponent;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.SecurityUtils;
import imc.lecturnity.util.ui.LayoutUtils;
import imc.lecturnity.util.ui.SplashScreen;


public class GlobalMenuBar extends JMenuBar
{
   static
   {
      JPopupMenu.setDefaultLightWeightPopupEnabled(false);
   }


   private GlobalMasterAndUI master_;
   private Document document_;

   private JMenu     viewMenu_;
   private JMenu     controlMenu_;
   private JMenuItem metaItem_;
   private JMenuItem cacheItem_;
   private JCheckBoxMenuItem loopItem_;
   
   private int iNavigationState_;

   private Color standardBackground_ = new Color(0xd5d5d5);
   



   public GlobalMenuBar(GlobalMasterAndUI master, Document document)
   {
      master_ = master;
      document_ = document;
      
      iNavigationState_ = master_.getStandardNavigationState();


      //
      // File Menu
      // 

      final Image frameIconImage = createIcon("/imc/epresenter/player/icon_player.gif").getImage();
      
      JMenu fileMenu = new JMenu(Manager.getLocalized("file"));
      JMenuItem item1 = new JMenuItem(new AbstractAction(Manager.getLocalized("open")+"...") {
         public void actionPerformed(ActionEvent e)
         {
            //Daemon.stopSplashJostling();
            String file = Manager.showFileDialog(
               SwingUtilities.windowForComponent(GlobalMenuBar.this));
            if (file != null)
            {
               Manager.putLoadWindow(
                  SwingUtilities.windowForComponent(GlobalMenuBar.this));
               master_.loadDocument(file);
            }
         }
      });
      item1.setAccelerator(
         KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_O, 
                                java.awt.event.InputEvent.CTRL_MASK));
      fileMenu.add(item1);
		JMenuItem item1komma2 = new JMenuItem(new AbstractAction(Manager.getLocalized("openURL")+"...") {
         public void actionPerformed(ActionEvent e)
         {
            //Daemon.stopSplashJostling();
            String url = Manager.showURLDialog(
               SwingUtilities.windowForComponent(GlobalMenuBar.this));
            if (url != null)
            {
               // must start with "http" in order to be handled correctly 
               // by GlobalMasterAndUI.loadDocument
               if (!url.startsWith("http"))
                  url = "http://"+url;

               Manager.putLoadWindow(
                  SwingUtilities.windowForComponent(GlobalMenuBar.this));
               

               master_.loadDocument(url);
            }
         }
      });
      fileMenu.add(item1komma2);
      /*
      JMenuItem printItem = createMenuItem(new AbstractAction("Print XXX") {
         public void actionPerformed(ActionEvent e)
         {
            PrinterJob job = PrinterJob.getPrinterJob();

            job.setPageable(document_);
            
            // TODO make object orientated??
            String strPrintTitle = document_.GetWindowTitle(null);
            if (strPrintTitle == null || strPrintTitle.length() == 0)
               strPrintTitle = "LECTURNITY Player Printing";

            job.setJobName(strPrintTitle);

            boolean bDoPrint = job.printDialog();

            if (bDoPrint) {
               try {
                  job.print();
               } catch (PrinterException exc) {
                  exc.printStackTrace();
               }
            }
            
         }
      });
      fileMenu.add(printItem);
      */
		metaItem_ = createMenuItem(new AbstractAction(Manager.getLocalized("properties")) {
         public void actionPerformed(ActionEvent e)
         {
            final JFrame metaFrame = Manager.requestFrame();
            metaFrame.setTitle(Manager.getLocalized("properties"));
            //metaFrame.setIconImage(frameIconImage);
            Border labelBorderKey = BorderFactory.createEmptyBorder(2,3,2,2);
            Border labelBorderValue = BorderFactory. createEmptyBorder(2,2,2,3);

            // meat line:
            JPanel meta = createTableLayout(document_.getMetaInfo(), labelBorderKey, labelBorderValue);
            
            meta.setOpaque(true);
            Border b27 = BorderFactory.createEmptyBorder(10,10,10,10);
            meta.setBorder(
               BorderFactory.createCompoundBorder(BorderFactory.createBevelBorder(BevelBorder.LOWERED), b27));
            JPanel total = new JPanel(new BorderLayout(0,3));
            total.setOpaque(true);
            total.setBorder(BorderFactory.createEmptyBorder(4,4,4,4));
            total.add("Center", meta);
            JPanel op = new JPanel(new FlowLayout());
            op.setOpaque(true);
            JButton button = new JButton("Ok");
            button.addActionListener(new ActionListener() {
               public void actionPerformed(ActionEvent e)
               {
                  metaFrame.dispose();
                  metaFrame.setContentPane(new JPanel());
               }
            });
            op.add(button);
            total.add("South", op);
            metaFrame.setContentPane(total);
            metaFrame.pack();
           Window w = SwingUtilities.windowForComponent(GlobalMenuBar.this);
            if (w != null)
               LayoutUtils.centerWindow(metaFrame, w.getBounds());
            else
               LayoutUtils.centerWindow(metaFrame);
            metaFrame.show();
            metaFrame.addWindowListener(new WindowAdapter() {
               public void windowClosing(WindowEvent e) {
                  metaFrame.dispose();
                  metaFrame.setContentPane(new JPanel());
               }
            });
         }
      });
      fileMenu.add(metaItem_);
      if (document_.getLocation() == null)
         metaItem_.setEnabled(false);
		JMenuItem fileExitItem = createMenuItem(new AbstractAction(Manager.getLocalized("exit")) {
         public void actionPerformed(ActionEvent e)
         {
            master_.closeDocument(document_);
         }
      });
      fileExitItem.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F4,
                                 java.awt.event.InputEvent.ALT_MASK));
      fileMenu.add(fileExitItem);
      


      //
      // Help Menu
      //

      final String helpPath = System.getProperty("help.path");
      
      JMenu helpMenu = new JMenu("?");
      JMenuItem item3 = new JMenuItem(new AbstractAction(Manager.getLocalized("help")) {
         public void actionPerformed(ActionEvent e)
         {
            Window w = SwingUtilities.windowForComponent(GlobalMenuBar.this);
            
            //Daemon.stopSplashJostling();
            if (helpPath != null && !helpPath.endsWith(".html"))
            {
               boolean bHelpFound = false;
               if (new File(helpPath).exists())
               {
                  if (NativeUtils.isLibraryLoaded())
                  {
                     if (NativeUtils.startFile(helpPath)) // false means error
                        bHelpFound = true;
                  }
               }
              
               if (!bHelpFound)
                  Manager.showError(w, Manager.getLocalized("helpError2")
                                    +"\n("+helpPath+")", Manager.WARNING, null);
            }
            else // helpPath unspecified or old style
            {
               // should only happen on manual invocation
               // or some strange error
               
               boolean bHelpFound = false;
               if (NativeUtils.isLibraryLoaded())
               {
                  // try and read it's location from the registry
                  String helpPathRegistry = NativeUtils.getRegistryValue(
                     "HKLM", "SOFTWARE\\imc AG\\LECTURNITY", "InstallDir");
                 
                  if (helpPathRegistry != null)
                  {
                     //String language = NativeUtils.getLanguageCode();
                     helpPathRegistry += File.separator;
                     helpPathRegistry += "Player";
                     helpPathRegistry += File.separator;
                     helpPathRegistry += "player.pdf";
                     
                     if (new File(helpPathRegistry).exists())
                     {
                        if (NativeUtils.startFile(helpPathRegistry)) // false means error
                           bHelpFound = true;
                     }
               
                  }
               }
               
               
               
               if (!bHelpFound)
                  Manager.showError(w, Manager.getLocalized("helpError2"), Manager.WARNING, null);
            }
            
               // deprecated as of 1.6.1; online-help now one pdf file
               //Manager.showHtmlFrame(master_, Manager.getLocalized("onlineHelp"), helpPath); 
            
         }
      });
      //if (helpPath == null || !(new File(helpPath).exists()) )
      //   item3.setEnabled(false);
      helpMenu.add(item3);
      item3.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F1, 0));
       

      JMenuItem item4 = new JMenuItem(new AbstractAction(Manager.getLocalized("about")) {
         public void actionPerformed(ActionEvent e)
         {
             Window w = SwingUtilities.windowForComponent(GlobalMenuBar.this);
             SplashScreen s = new SplashScreen(w);
             s.exhibit();
         }
      });
      helpMenu.add(item4);


      //
      // Extras Menu
      //

      JMenu cacheMenu = new JMenu(Manager.getLocalized("tools"));
      loopItem_ = new JCheckBoxMenuItem(new AbstractAction(Manager.getLocalized("autoLoop")) {
         public void actionPerformed(ActionEvent e)
         {
            JCheckBoxMenuItem item498 = (JCheckBoxMenuItem)e.getSource();
            document_.activateLooping(item498.getState());
         }
      });
      cacheMenu.add(loopItem_);
       if (Manager.isWindows())
       {
          cacheItem_ = new JMenuItem(new AbstractAction(Manager.getLocalized("cacheManager")+"...") {
             public void actionPerformed(ActionEvent e)
             {
                master_.showCacheDialog(document_.getLocation());
             }
          });
          cacheMenu.add(cacheItem_);
          if (document_.getLocation() == null)
             cacheItem_.setEnabled(false);
       }



       //
       // View Menu
       //

       viewMenu_ = new JMenu(Manager.getLocalized("view"));
       
       JMenuItem viewFullItem =
          new JMenuItem(new AbstractAction(Manager.getLocalized("fullScreenMode")) {
          public void actionPerformed(ActionEvent e)
          {
             
             if (document_.getLocation() != null)
                document_.switchToFullScreen(false);
          }
       });
       viewFullItem.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_ENTER,
                                 java.awt.event.InputEvent.ALT_MASK));
       viewMenu_.add(viewFullItem);

       final JMenuItem viewRestoreItem = 
          new JMenuItem(new AbstractAction(Manager.getLocalized("restoreView")) {
          public void actionPerformed(ActionEvent e)
          {
             /*
             KeyEvent event = new KeyEvent(GlobalMenuBar.this, 
                                           java.awt.event.KeyEvent.KEY_RELEASED,
                                           System.currentTimeMillis(),
                                           java.awt.event.InputEvent.SHIFT_MASK,
                                           java.awt.event.KeyEvent.VK_COLON);
             master_.eventDispatched(event);
             */
             
             master_.restoreLayout(document_);
          }
       });
       viewRestoreItem.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_SPACE,
                                 java.awt.event.InputEvent.ALT_MASK));
       viewMenu_.add(viewRestoreItem);

       

       if (document_.getLocation() == null)
          viewMenu_.setEnabled(false);
       

       //
       // Control menu
       // 
       controlMenu_ = new JMenu(Manager.getLocalized("control"));
       

       JMenuItem startStopItem =
          new JMenuItem(new AbstractAction(Manager.getLocalized("startStopReplay")) {
             public void actionPerformed(ActionEvent e)
             {
                //document_.doStartOrStop();
             }
          });
       if (iNavigationState_ == Document.NAV_STATE_ENABLED)
         startStopItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_SPACE, 0));
       else
         startStopItem.setEnabled(false);
       if (iNavigationState_ != Document.NAV_STATE_HIDDEN)
         controlMenu_.add(startStopItem);

       controlMenu_.addSeparator();

       JMenuItem nextSlideItem =
          new JMenuItem(new AbstractAction(Manager.getLocalized("nextSlide")) {
             public void actionPerformed(ActionEvent e)
             {
                document_.requestNextSlide(null);
             }
          });
       if (iNavigationState_ == Document.NAV_STATE_ENABLED)
         nextSlideItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_PAGE_DOWN, 0));
       else
         nextSlideItem.setEnabled(false);
       if (iNavigationState_ != Document.NAV_STATE_HIDDEN)
         controlMenu_.add(nextSlideItem);

       JMenuItem prevSlideItem =
          new JMenuItem(new AbstractAction(Manager.getLocalized("prevSlide")) {
             public void actionPerformed(ActionEvent e)
             {
                document_.requestPreviousSlide(false, null);
             }
          });
       if (iNavigationState_ == Document.NAV_STATE_ENABLED)
         prevSlideItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_PAGE_UP, 0));
       else
         prevSlideItem.setEnabled(false);
       if (iNavigationState_ != Document.NAV_STATE_HIDDEN)
          controlMenu_.add(prevSlideItem);

       JMenuItem forwardItem =
          new JMenuItem(new AbstractAction(Manager.getLocalized("secondsForward")) {
             public void actionPerformed(ActionEvent e)
             {
                document_.jumpForward(10000);
             }
          });
       if (iNavigationState_ == Document.NAV_STATE_ENABLED)
         forwardItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_RIGHT, 0));
       else
         forwardItem.setEnabled(false);
       if (iNavigationState_ != Document.NAV_STATE_HIDDEN)
          controlMenu_.add(forwardItem);

       JMenuItem backwardItem =
          new JMenuItem(new AbstractAction(Manager.getLocalized("secondsBackward")) {
             public void actionPerformed(ActionEvent e)
             {
                document_.jumpBackward(10000);
             }
          });
       if (iNavigationState_ == Document.NAV_STATE_ENABLED)
         backwardItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_LEFT, 0));
       else
         backwardItem.setEnabled(false);
       if (iNavigationState_ != Document.NAV_STATE_HIDDEN)
          controlMenu_.add(backwardItem);

       JMenuItem toBeginningItem =
          new JMenuItem(new AbstractAction(Manager.getLocalized("toBeginning")) {
             public void actionPerformed(ActionEvent e)
             {
                document_.jumpForward(-1);
             }
          });
       if (iNavigationState_ == Document.NAV_STATE_ENABLED)
         toBeginningItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_HOME, 0));
       else
         toBeginningItem.setEnabled(false);
       if (iNavigationState_ != Document.NAV_STATE_HIDDEN)
          controlMenu_.add(toBeginningItem);

       JMenuItem toEndItem =
          new JMenuItem(new AbstractAction(Manager.getLocalized("toEnd")) {
             public void actionPerformed(ActionEvent e)
             {
                document_.jumpBackward(-1);
             }
          });
       if (iNavigationState_ == Document.NAV_STATE_ENABLED)
         toEndItem.setAccelerator(
            KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_END, 0));
       else
         toEndItem.setEnabled(false);
       if (iNavigationState_ != Document.NAV_STATE_HIDDEN)
          controlMenu_.add(toEndItem);

       if (iNavigationState_ != Document.NAV_STATE_HIDDEN)
          controlMenu_.addSeparator();

       JMenuItem increaseVolumeItem =
          new JMenuItem(new AbstractAction(Manager.getLocalized("increaseVolume")) {
             public void actionPerformed(ActionEvent e)
             {
                document_.increaseVolume();
             }
          });
       increaseVolumeItem.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F10, 0));
       controlMenu_.add(increaseVolumeItem);

       JMenuItem decreaseVolumeItem =
          new JMenuItem(new AbstractAction(Manager.getLocalized("decreaseVolume")) {
             public void actionPerformed(ActionEvent e)
             {
                document_.decreaseVolume();
             }
          });
       decreaseVolumeItem.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F9, 0));
       controlMenu_.add(decreaseVolumeItem);

       JMenuItem muteOnOffItem =
          new JMenuItem(new AbstractAction(Manager.getLocalized("muteOnOff")) {
             public void actionPerformed(ActionEvent e)
             {
                document_.muteOnOff();
             }
          });
       muteOnOffItem.setAccelerator(
          KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_M, 
                                 java.awt.event.InputEvent.CTRL_MASK));
       controlMenu_.add(muteOnOffItem);


       if (document_.getLocation() == null)
          controlMenu_.setEnabled(false);



       this.add(fileMenu);
       this.add(viewMenu_);
       this.add(controlMenu_);
       if (Manager.isWindows())
          this.add(cacheMenu);
       this.add(helpMenu);
      
   }

   public void setDocument(Document d)
   {
      if (d != null && d.getLocation() != null)
      {
         metaItem_.setEnabled(true);
         if (Manager.isWindows())
            cacheItem_.setEnabled(true);
         viewMenu_.setEnabled(true);
         controlMenu_.setEnabled(true);
      }

      loopItem_.setState(false);

      document_ = d;
      // all other is currently done "on demand"
      // meaning things are constructed/instantiated when the item is selected
   }

   // most dirties hack to asses the right menu bar for a given document
   public Document getDocument()
   {
      return document_;
   }

   public void actionPerformed(ActionEvent e)
   {
   }

   private ImageIcon createIcon(String location)
   {
		URL loc = getClass().getResource(location);
      if (loc != null)
         return new ImageIcon(loc);
      else
         return null;
	}
 
   private JMenuItem createMenuItem(Action a)
   {
      JMenuItem item = new JMenuItem(a);
      //item.setBackground(standardBackground_);
      return item;
   }

   private JPanel createTableLayout(HashMap values, Border keyB, Border valueB)
   {
      JPanel total = new JPanel(new BorderLayout());
      total.setOpaque(false);

      JPanel keyPanel = new JPanel(new GridLayout(0,1));
      JPanel valuePanel = new JPanel(new GridLayout(0,1));

      Iterator iter = values.keySet().iterator();
      while(iter.hasNext())
      {
         String key = (String)iter.next();
         String value = (String)values.get(key);
         JLabel keyL = createLabel(key+": ", Font.PLAIN);
         keyL.setBorder(keyB);
         JLabel valueL = createLabel(value);
         valueL.setToolTipText(value);
         valueL.setBorder(valueB);

         keyPanel.add(keyL);
         valuePanel.add(valueL);
      }

      total.add("West", keyPanel);
      total.add("Center", valuePanel);

      return total;
   }

   private JLabel createLabel(String text)
   {
      return createLabel(text, Font.PLAIN);
   }

   private JLabel createLabel(String text, int style)
   {
      JLabel l = new JLabel(text, JLabel.LEFT);
      if (style != Font.PLAIN)
         l.setFont(l.getFont().deriveFont(Font.BOLD));
      return l;
   }

}