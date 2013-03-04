package imc.epresenter.player;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

import imc.lecturnity.util.DelayedTimer;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.ui.LayoutUtils;

public class CacheDialogs
{
   private static ImageIcon dialogIcon = new ImageIcon(
      "".getClass().getResource("/imc/epresenter/player/icon_player.gif"));
      

   private static Border emptyBorder6_ = BorderFactory.createEmptyBorder(6,6,6,6);
   private static Border emptyBorder2_ = BorderFactory.createEmptyBorder(2,0,2,6);

   
   public static int showCacheNowDialog(
      final CacheBackend backend, final String location)
   {
      //
      // check if there is a setting affecting this dialog
      // which then might stay hidden and return a selection directly
      //
      CacheSettings settings = backend.getSettings();
      String cacheMode = settings.getProperty("cacheMode");

      if (cacheMode.equals("Always"))
         return JOptionPane.YES_OPTION;
      if (cacheMode.equals("Never"))
         return JOptionPane.NO_OPTION;


      String[] paths = settings.getProperties("selectiveCachePaths");
      String[] values = settings.getProperties("selectiveCacheValues");
      if (paths != null && paths.length > 0)
      {
         for (int i=0; i<paths.length; i++)
         {
            if (pathMatches(location, paths[i]))
            {
               if (values[i].equals("Yes"))
                  return JOptionPane.YES_OPTION;
               else
                  return JOptionPane.NO_OPTION;
            }
         }
      }


      //
      // build and display the dialog
      //
      JTextArea messageA = LayoutUtils.createMultilineLabel(
         Manager.getLocalized("cacheNowMessage"));
      messageA.setRows(5);
      messageA.setPreferredSize(
         new Dimension(400, messageA.getPreferredSize().height));
      
      JButton settingsB = new JButton(Manager.getLocalized("settings"));
      settingsB.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e)
         {
            CacheControl cc = new CacheControl(backend, location);
            cc.showSettingsOnly();
            //cc.waitForFinish();
         }
      });
      JRadioButton cacheYesB = new JRadioButton(
         Manager.getLocalized("yesCache"), true);
      cacheYesB.setFont(cacheYesB.getFont().deriveFont(Font.BOLD));
      JRadioButton cacheNoB = new JRadioButton(Manager.getLocalized("noCache"));
      ButtonGroup groupOne = new ButtonGroup();
      groupOne.add(cacheYesB);
      groupOne.add(cacheNoB);

      final JCheckBox futureC = new JCheckBox(Manager.getLocalized("notAskAgainAnd"));

      final JRadioButton futurePathB = new JRadioButton(
         Manager.getLocalized("saveForPath"), true);
      futurePathB.setEnabled(false);
      final JRadioButton futureAllB = new JRadioButton(
         Manager.getLocalized("saveForAll"));
      futureAllB.setEnabled(false);
      ButtonGroup groupTwo = new ButtonGroup();
      groupTwo.add(futurePathB);
      groupTwo.add(futureAllB);

      futureC.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e)
         {
            if (futureC.isSelected())
            {
               futurePathB.setEnabled(true);
               futureAllB.setEnabled(true);
            }
            else
            {
               futurePathB.setEnabled(false);
               futureAllB.setEnabled(false);
            }
         }
      });


      JPanel inner1 = new JPanel(new GridLayout(0,1));
      inner1.add(cacheYesB);
      inner1.add(cacheNoB);

      JPanel inner2 = new JPanel(new GridLayout(0,1));
      inner2.add(futurePathB);
      inner2.add(futureAllB);

      JPanel dummy1 = new JPanel();
      dummy1.add(settingsB);

      JPanel outer1 = new JPanel(new BorderLayout(0,4));
      outer1.add("North", messageA);
      outer1.add("West", Box.createHorizontalStrut(30));
      outer1.add("Center", inner1);
      outer1.add("East", dummy1);

      JPanel outer2 = new JPanel(new BorderLayout());
      outer2.add("North", futureC);
      outer2.add("West", Box.createHorizontalStrut(30));
      outer2.add("Center", inner2);


      //
      // prepare for box layout (resize horizontally)
      //
      

      JPanel total = LayoutUtils.createBoxLayoutPanel(true);
      
      total.add(LayoutUtils.makeBoxable(outer1, true));
      total.add(Box.createVerticalStrut(10));
      total.add(LayoutUtils.makeBoxable(outer2, true));


      int result = JOptionPane.showOptionDialog(
         Manager.getLoadWindow(), total, Manager.getLocalized("caching")+"?",
         JOptionPane.YES_NO_CANCEL_OPTION,
         JOptionPane.WARNING_MESSAGE,
         null, 
         new String[] { Manager.getLocalized("help"), "Ok", Manager.getLocalized("cancel") },
         "Ok");

      // YES = HELP
      // NO = CACHE | PLAY
      // CANCEL = QUIT
      
      if (result == JOptionPane.YES_OPTION)
         result = JOptionPane.CLOSED_OPTION;
      else if (result == JOptionPane.CLOSED_OPTION)
         result = JOptionPane.CANCEL_OPTION;
      
      if (result == JOptionPane.NO_OPTION && !cacheNoB.isSelected())
         result = JOptionPane.YES_OPTION;
      
      
      // YES = CACHE
      // NO = PLAY
      // CANCEL = QUIT
      // CLOSED = HELP

      if (result == JOptionPane.YES_OPTION || result == JOptionPane.NO_OPTION)
      {
         // save settings if activated
         if (futureC.isSelected())
         {
            // futurePathB, futureAllB
            if (futurePathB.isSelected())
            {
               if (paths == null)
               {
                  paths = new String[0];
                  values = new String[0];
               }

               String[] newPaths = new String[paths.length+1];
               String[] newValues = new String[paths.length+1];

               System.arraycopy(paths, 0, newPaths, 0, paths.length); 
               System.arraycopy(values, 0, newValues, 0, paths.length);

               newPaths[paths.length] = stripPath(location);
               newValues[paths.length] = cacheNoB.isSelected() ? "No" : "Yes";

               settings.setProperties("selectiveCachePaths", newPaths);
               settings.setProperties("selectiveCacheValues", newValues);
            }
            else
            {
               settings.setProperty("cacheMode", "Always");
            }

            settings.sync();
         }
      }


      return result;
   }

   public static int showCacheTooSlowDialog(CacheBackend backend)
   {

      JTextArea messageA = LayoutUtils.createMultilineLabel(
        Manager.getLocalized("cacheTooSlow"));
      messageA.setRows(3);
      messageA.setPreferredSize(new Dimension(400, messageA.getPreferredSize().height));
      
      JCheckBox awayWithTheeB = new JCheckBox("Diesen Dialog nicht mehr anzeigen.");

      JPanel total = new JPanel(new BorderLayout());
      total.add("Center", messageA);
      //total.add("South", awayWithTheeB);

      int result = JOptionPane.showOptionDialog(
         Manager.getLoadWindow(), total, Manager.getLocalized("slowCache"), 
         JOptionPane.YES_NO_CANCEL_OPTION,
         JOptionPane.WARNING_MESSAGE,
         null, new String[] { 
            Manager.getLocalized("changeSettings"), 
               Manager.getLocalized("replay"), 
               Manager.getLocalized("cancel") },
               "Ok");

      
      if (result == JOptionPane.CLOSED_OPTION)
         result = JOptionPane.CANCEL_OPTION;
      

      return result;
   }
   
   public static int showCacheTooSmallDialog(CacheBackend backend)
   {
      return showCacheTooSmallDialog(backend, false);
   }

   public static int showCacheTooSmallDialog(CacheBackend backend, boolean noNo)
   {
      JTextArea messageA = LayoutUtils.createMultilineLabel(
         Manager.getLocalized("cacheTooSmall"));
      messageA.setRows(3);
      messageA.setPreferredSize(new Dimension(400, messageA.getPreferredSize().height));
      
      JCheckBox awayWithTheeB = new JCheckBox("???? Diesen Dialog nicht mehr anzeigen.");

      JPanel total = new JPanel(new BorderLayout());
      total.add("Center", messageA);
      //total.add("South", awayWithTheeB);
      
      String[] options = new String[] { Manager.getLocalized("changeSettings"), 
         Manager.getLocalized("replay"), Manager.getLocalized("cancel") };
      String[] nonoOptions = new String[] { Manager.getLocalized("changeSettings"), 
         Manager.getLocalized("cancel") };

      int result = JOptionPane.showOptionDialog(
         Manager.getLoadWindow(), total, Manager.getLocalized("smallCache"),
         noNo ? JOptionPane.YES_NO_OPTION : JOptionPane.YES_NO_CANCEL_OPTION,
         JOptionPane.WARNING_MESSAGE,
         null, noNo ? nonoOptions : options, "Ok");

      if (result == JOptionPane.CLOSED_OPTION)
         result = JOptionPane.CANCEL_OPTION;

      if (noNo && result == JOptionPane.NO_OPTION)
         result = JOptionPane.CANCEL_OPTION;
      
      return result;
   }
   
   public static int showCacheTooFullDialog(CacheBackend backend)
   {
      return showCacheTooFullDialog(backend, false);
   }

   public static int showCacheTooFullDialog(CacheBackend backend, boolean noNo)
   {

      JTextArea messageA = LayoutUtils.createMultilineLabel(
         Manager.getLocalized("cacheTooFull"));
      messageA.setRows(3);
      messageA.setPreferredSize(new Dimension(400, messageA.getPreferredSize().height));
      
      JCheckBox awayWithTheeB = new JCheckBox("Diesen Dialog nicht mehr anzeigen.");

      JPanel total = new JPanel(new BorderLayout());
      total.add("Center", messageA);
      //total.add("South", awayWithTheeB);
      
      String[] options = new String[] { Manager.getLocalized("changeSettings"), 
         Manager.getLocalized("replay"), Manager.getLocalized("cancel") };
      String[] nonoOptions = new String[] { Manager.getLocalized("changeSettings"), 
         Manager.getLocalized("cancel") };


      int result = JOptionPane.showOptionDialog(
         Manager.getLoadWindow(), total, Manager.getLocalized("fullCache"),
         noNo ? JOptionPane.YES_NO_OPTION : JOptionPane.YES_NO_CANCEL_OPTION,
         JOptionPane.WARNING_MESSAGE,
         null, noNo ? nonoOptions : options, "Ok");

      if (result == JOptionPane.CLOSED_OPTION)
         result = JOptionPane.CANCEL_OPTION;
      
      if (noNo && result == JOptionPane.NO_OPTION)
         result = JOptionPane.CANCEL_OPTION;
      

      return result;
   }

   private static String stripPath(String location)
   {
      return new File(location).getParent();
   }

   private static boolean pathMatches(String file, String path)
   {
      return new File(file).getParentFile().equals(new File(path));
   }


   public static class SizeStub extends JPanel implements ChangeListener,
      KeyListener, ActionListener
   {
      private ActionListener listener_;
      private JSlider slider_;
      private JTextField field_;
      private DelayedTimer timer_;


      public SizeStub(ActionListener listener, int value)
      {
         listener_ = listener;

         slider_ = new JSlider(0,2048);
         slider_.setMajorTickSpacing(1024);
         slider_.setMinorTickSpacing(100);
         slider_.setPaintTicks(true);
         slider_.setPreferredSize(new Dimension(200, slider_.getPreferredSize().height));
         slider_.addChangeListener(this);

         field_ = new JTextField("");
         field_.setHorizontalAlignment(JLabel.RIGHT);
         field_.setPreferredSize(new Dimension(70, field_.getPreferredSize().height));
         field_.addKeyListener(this);

         slider_.setValue(value);
         
         add(slider_);
         add(field_);
         add(new JLabel(" MB"));
         
         timer_ = new DelayedTimer(this, 1000);
      }

      public int getValue()
      {
         return slider_.getValue();
      }


      public void stateChanged(ChangeEvent e)
      {
         int value = slider_.getValue();
         if (!(""+value).equals(field_.getText()))
            field_.setText(value+"");
         if (timer_ != null)
            timer_.delayFurther();
      }


      public void keyPressed(KeyEvent e)
      {
      }

      public void keyReleased(KeyEvent e)
      {
         checkForNumber(e);

         timer_.delayFurther();

         if (!e.isConsumed())
         {
            try
            {
               int value = Integer.parseInt(field_.getText());
               if (value != slider_.getValue())
               {
                  if (value <= slider_.getMaximum())
                     slider_.setValue(value);
                  //else
                     //field_.setText(""+slider_.getMaximum());
               }
            }
            catch (NumberFormatException exc)
            {
            }
         }
        
      }
      
      public void keyTyped(KeyEvent e)
      {
      }

      public void actionPerformed(ActionEvent e)
      {
         if (listener_ != null)
         {
            //System.out.println("event dispatching");
            listener_.actionPerformed(new ActionEvent(this, 0, ""));
         }
      }


      private void checkForNumber(KeyEvent e)
      {
         char c = e.getKeyChar();
         int code = e.getKeyCode();
         boolean valid = false;
         if (c >= '0' && c <= '9')
            valid = true;
         if (code == e.VK_LEFT || code == e.VK_RIGHT)
            valid = true;
         if (code == e.VK_BACK_SPACE || code == e.VK_DELETE)
            valid = true;
         
         if (!valid)
            e.consume(); // no valid input
      }
   }



   public static class CopyStub extends JPanel implements Runnable, ActionListener
   {
      private CacheBackend backend_;
      private String location_;
      private ActionListener listener_;
      private JProgressBar progressBar_;
      private JButton copyB_;
      private boolean cancelled_;
      private boolean isActive_;

      
      public CopyStub(CacheBackend backend, String location,
                      ActionListener listener, boolean autoStart)
      {
         backend_ = backend;
         location_ = location;
         listener_ = listener; // for copy finished/canceled

         progressBar_ = new JProgressBar();
         progressBar_.setPreferredSize(
            new Dimension(400, progressBar_.getPreferredSize().height));
         
         copyB_ = new JButton(Manager.getLocalized("copy"));
         copyB_.addActionListener(this);
         if (backend_.contains(location_))
            copyB_.setEnabled(false);
         
         LayoutUtils.addBorder(progressBar_, emptyBorder2_);

         setLayout(new BorderLayout());
         add("Center", progressBar_);
         add("East", copyB_);


         if (autoStart)
            start();
      }

      public void setEnabled(boolean b)
      {
         copyB_.setEnabled(b);
      }

      public void actionPerformed(ActionEvent evt)
      {
         if (((JButton)evt.getSource()).getText().equals(Manager.getLocalized("copy")))
            start();
         else if (((JButton)evt.getSource()).getText().equals(Manager.getLocalized("cancel")))
            cancel();
      }

      public void start()
      {
         // todo
         // check sizes (cache and device)



         new Thread(this).start();
         copyB_.setText(Manager.getLocalized("cancel"));
      }

      public void run()
      {
         isActive_ = true;

         Window parent = Manager.getLoadWindow();

         String newFile = null;
         try
         {
            newFile = backend_.copyFile(location_, progressBar_);
         }
         catch (FileNotFoundException fex)
         {
            Manager.showError(parent, Manager.getLocalized("fileNotFound1")
                              +" ("+location_+") "
                              +Manager.getLocalized("fileNotFound2"),
                              Manager.WARNING, fex);

         }
         catch (IOException iex)
         {
            Manager.showError(parent, 
                              Manager.getLocalized("copyError"), Manager.WARNING, iex);
         }
         
         if (!cancelled_ && newFile != null)
         {
            copyB_.setText(Manager.getLocalized("copy"));
            copyB_.setEnabled(false);
            progressBar_.setForeground(progressBar_.getBackground().darker());
         
         }
         
         if (listener_ != null)
         {
            String result = "ok";
            if (cancelled_)
               result = "cancel";
            else if (newFile == null)
               result = "error";
            listener_.actionPerformed(new ActionEvent(this, 0, result));
         }

         isActive_ = false;
      }

      public boolean isActive()
      {
         return isActive_;
      }

      public boolean isCanceled()
      {
         return cancelled_;
      }

      public void cancel()
      {
         cancelled_ = true;
         backend_.cancelCopy(location_);
         progressBar_.setValue(progressBar_.getMinimum());
         copyB_.setText(Manager.getLocalized("copy"));

         // just in case the copy was finished
         try
         {
            backend_.deleteInCache(location_);
         }
         catch (IOException exc)
         {
            // hm
         }
      }

      public void reinit()
      {
         progressBar_.setValue(progressBar_.getMinimum());
         copyB_.setEnabled(true);
         progressBar_.setForeground(new JProgressBar().getForeground());
      }

   }
      
}