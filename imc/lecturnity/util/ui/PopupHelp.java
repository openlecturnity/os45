package imc.lecturnity.util.ui;

import javax.swing.*;
import java.awt.event.*;
import javax.swing.border.*;

import java.awt.*;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.converter.ConverterWizard;

public class PopupHelp
{
   private static String DIRECT_HELP = "[!]";
   
   private static boolean USE_CUSTOM_COLORS = false;

   static
   {
      try
      {
         Localizer l = new Localizer("/imc/lecturnity/util/ui/PopupHelp_", "en");

         DIRECT_HELP = l.getLocalized("DIRECT_HELP");
      }
      catch (Exception e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(null, "Language resource database could not\n" +
                                       "be opened: PopupHelp\n" + e.toString(), "Error",
                                       JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private JComponent component_;
   private String helpText_;

   private static JWindow helpWindow_      = null;
   private static JLabel textLabel_   = null;

   private JPopupMenu popupMenu_;
  
   public PopupHelp(JComponent c, String t)
   {
      component_ = c;
      helpText_ = t;

      if (helpWindow_ == null)
      {
         helpWindow_ = new JWindow();

         Container p = helpWindow_.getContentPane();
         //if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         if (USE_CUSTOM_COLORS)
            p.setBackground(imc.lecturnity.util.ColorManager.orangeResource);
         else
            p.setBackground(javax.swing.UIManager.getColor("ToolTip.background"));
         p.setLayout(new BorderLayout());
         JPanel r = new JPanel() {
               public Insets getInsets()
               {
                  return new Insets(5, 8, 5, 8);
               }
            };
         //if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         if (USE_CUSTOM_COLORS)
            r.setBackground(imc.lecturnity.util.ColorManager.orangeResource);
         else
            r.setBackground(javax.swing.UIManager.getColor("ToolTip.background"));

         p.add(r, BorderLayout.CENTER);
         r.setBorder(new LineBorder(Color.black, 1));
         r.setLayout(new BorderLayout());
         textLabel_ = new JLabel("dummi");
         textLabel_.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
         r.add(textLabel_, BorderLayout.CENTER);         
         helpWindow_.pack();
      }

      WindowCloser windowCloser = new WindowCloser();
      helpWindow_.addMouseListener(windowCloser);

      // popup menu part
      popupMenu_ = new JPopupMenu();
      JMenuItem menuItem = popupMenu_.add(DIRECT_HELP);
      menuItem.addActionListener(new HelpActionListener());

      // add mouse listener to component:
      component_.addMouseListener(new MouseChecker());
      // and a keyboard listener:
      component_.addKeyListener(new KeyChecker());
   }

   /**
    * Sets the help text of this <tt>PopupHelp</tt> instance
    * @param helpText the new help text
    */
   public void setText(String helpText)
   {
      helpText_ = helpText;
   }

   /**
    * Returns the current help text for this <tt>PopupHelp</tt> instance
    * @return the current help text for this <tt>PopupHelp</tt> instance
    */
   public String getText()
   {
      return helpText_;
   }

   public void popupHelp()
   {
      performHelpAction();
   }

   /**
    * Call this method to close all currently visible help windows.
    */
   public static void closeHelpWindows()
   {
      if (helpWindow_ != null)
         helpWindow_.setVisible(false);
   }

   /**
    * Returns <i>true</i> if the help window is visible
    * @return <i>true</i> if the help window is visible
    */
   public static boolean isHelpVisible()
   {
      if (helpWindow_ != null)
         return helpWindow_.isVisible();
      return false;
   }

   private void checkEvent(MouseEvent e)
   {
//       if (helpWindow_.isVisible())
//          helpWindow_.setVisible(false);

      if (e.isPopupTrigger())
      {
         popupMenu_.show(component_, e.getX(), e.getY());
      }
   }

   private void performHelpAction()
   {
      // first, set correct text:
      textLabel_.setText(helpText_);
      // then, pack the window:
      helpWindow_.pack();

      Dimension compDim = component_.getSize();
      Point upLeft = new Point(0, 0);
      SwingUtilities.convertPointToScreen(upLeft, component_);

      Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

      Dimension helpDim = helpWindow_.getSize();

         // default position: left side, atop, 5 pixel above component:
      int deltaX = 0;
      int deltaY = -helpDim.height - 5;

         // is the help window too large to be displayed above component?
      if (helpDim.height > upLeft.y - 10)
      {
         // yes, display below?
         deltaY = compDim.height + 5;

         // too large for below?
         if (upLeft.y + deltaY + helpDim.height > screenSize.height)
         {
            // display top-aligned with 10 px space:
            deltaY = -upLeft.y + 10;
         }
      }

      // is the help window too large to be displayed left of component?
      if (screenSize.width - upLeft.x - 5 < helpDim.width)
      {
         // yes, move to the right a little bit
         deltaX = - (helpDim.width - (screenSize.width - upLeft.x) + 5);
      }
      else if (upLeft.x < 0) // help window out of screen bounds
      {
         deltaX = -upLeft.x + 5;
      }

      Point p = new Point(deltaX, deltaY);
      SwingUtilities.convertPointToScreen(p, component_);
      helpWindow_.setLocation(p);
      helpWindow_.setVisible(true);
   }

   private class MouseChecker extends MouseAdapter 
   {
      public void mouseClicked(MouseEvent e)
      {
         checkEvent(e);
      }

      public void mousePressed(MouseEvent e)
      {
         checkEvent(e);
      }
      
      public void mouseReleased(MouseEvent e)
      {
         checkEvent(e);
      }
   }

   private class KeyChecker extends KeyAdapter
   {
      public void keyReleased(KeyEvent e)
      {
         if (e.getKeyCode() == KeyEvent.VK_F1)
            performHelpAction();
         else if (helpWindow_.isVisible())
            helpWindow_.setVisible(false);
      }
   }

   private class HelpActionListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         performHelpAction();
      }
   }

   private class WindowCloser extends MouseAdapter
   {
      public void mousePressed(MouseEvent e)
      {
         helpWindow_.setVisible(false);
      }
   }
}
