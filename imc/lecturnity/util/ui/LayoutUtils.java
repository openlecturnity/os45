package imc.lecturnity.util.ui;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.Border;

public class LayoutUtils
{

   public static JPanel createBoxLayoutPanel(boolean vertical)
   {
      JPanel panel = new JPanel();
      BoxLayout layout = new BoxLayout(panel, vertical ? BoxLayout.Y_AXIS : BoxLayout.X_AXIS);
      panel.setLayout(layout);

      return panel;
   }

   public static JTextArea createMultilineLabel(String text)
   {
      JTextArea message = new JTextArea(text);
      message.setEditable(false);
      message.setLineWrap(true);
      message.setWrapStyleWord(true);
      message.setFont(new JLabel().getFont());
      message.setOpaque(false);

      return message;
   }

   /**
    * Makes all sizes (min, pref, max) the same to the respective maximum of
    * all given components.
    * Helpfull if you want to have several components (eg buttons) of the same
    * size regardless of their content.
    */
   public static void evenSizes(JComponent[] components)
   {
      Dimension minSize = new Dimension(Integer.MIN_VALUE, Integer.MIN_VALUE);
      Dimension prefSize = new Dimension(Integer.MIN_VALUE, Integer.MIN_VALUE);
      Dimension maxSize = new Dimension(Integer.MIN_VALUE, Integer.MIN_VALUE);

      for (int i=0; i<components.length; i++)
      {
         Dimension min = components[i].getMinimumSize();
         if (min.width > minSize.width)
            minSize.width = min.width;
         if (min.height > minSize.height)
            minSize.height = min.height;

         Dimension pref = components[i].getPreferredSize();
         if (pref.width > prefSize.width)
            prefSize.width = pref.width;
         if (pref.height > prefSize.height)
            prefSize.height = pref.height;

         Dimension max = components[i].getMaximumSize();
         if (max.width > maxSize.width)
            maxSize.width = max.width;
         if (max.height > maxSize.height)
            maxSize.height = max.height;
      }

      for (int i=0; i<components.length; i++)
      {
         components[i].setMinimumSize(minSize);
         components[i].setPreferredSize(prefSize);
         components[i].setMaximumSize(maxSize);
      }
   }

   /**
    * Returns a component that takes the desired space in one dimension and all
    * available space in the other dimension and is thus suited for usage in a BoxLayout.
    */
   public static JComponent makeBoxable(JComponent comp, boolean horizontal)
   {
      if (!(comp instanceof JPanel)) // JPanel (and not JComponen) is important
      {
         JPanel helpP = new JPanel(new GridLayout(1,1));
         helpP.add(comp);
         comp = helpP;
      }

      if (horizontal)
         comp.setMaximumSize(new Dimension(Short.MAX_VALUE,
                                           comp.getPreferredSize().height));
      else
         comp.setMaximumSize(new Dimension(comp.getPreferredSize().width,
                                           Short.MAX_VALUE));
      

      return comp;
   }

   public static void addBorder(JComponent target, Border add)
   {
      addBorder(target, add, false);
   }
   
   public static void addBorder(JComponent target, Border add, boolean bInsert)
   {
      if (target.getBorder() != null)
      {
         if (!bInsert)
            target.setBorder(BorderFactory.createCompoundBorder(add, target.getBorder()));
         else
            target.setBorder(BorderFactory.createCompoundBorder(target.getBorder(), add));
      }
      else
         target.setBorder(add);

   
   }

   public static void addBorder(JComponent target, Insets insets)
   {
      Border add = BorderFactory.createEmptyBorder(insets.top, insets.left,
                                                   insets.bottom, insets.right);
      addBorder(target, add);
   }
   
   
   public static void enableContainer(Container container, boolean bEnable)
   {
      Component[] comps = container.getComponents();
      for (int i=0; i<comps.length; ++i)
      {
         if (comps[i] instanceof Container)
            enableContainer((Container)comps[i], bEnable);
       
         comps[i].setEnabled(bEnable);
      }
   }

   public static void centerWindow(Window target)
   {
      Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
      centerWindow(target, new Rectangle(0,0, d.width, d.height));
   }
   
   public static void centerWindow(Window target, Window source)
   {
      Rectangle sourceR = new Rectangle(source.getLocation().x, source.getLocation().y,
                                        source.getSize().width, source.getSize().height);
      
      centerWindow(target, sourceR);
   }

   public static void centerWindow(Window target, Rectangle source)
   {
      Dimension d = target.getSize();
      int diffX = source.width-d.width;
      int diffY = source.height-d.height;
      target.setLocation(source.x+diffX/2, source.y+diffY/2);
   }

   public static void clipWindowToScreen(Window target)
   {
      Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
      Dimension frameSize = target.getSize();

      if (frameSize.width > screenSize.width
          || frameSize.height > screenSize.height)
      {

         int correctedWidth = frameSize.width;
         int correctedHeight = frameSize.height;

         if (correctedWidth > screenSize.width)
            correctedWidth = screenSize.width;
         if (correctedHeight > screenSize.height)
            correctedHeight = screenSize.height;

         Point p = target.getLocation();

         int correctedX = p.x;
         int correctedY = p.y;

         if (correctedX < 0)
            correctedX = 0;
         if (correctedY < 0)
            correctedY = 0;

         target.setBounds(correctedX, correctedY, 
                         correctedWidth, correctedHeight);
      }
   }

   public static boolean isComponentFullScreen(Component c)
   {
      Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
      return (new Rectangle(0,0,screenSize.width,screenSize.height).equals(c.getBounds()));
   }



   /**
    * @returns the scaling factor administered to <code>source</code> 
    * to fit it to <code>target</code>
    */
   public static float scale(Dimension source, Dimension target, float maxScale)
   {
      if (source.width != target.width || source.height != target.height)
      {
         float scale = Math.min(target.width/(float)source.width,
                                target.height/(float)source.height);

         if (scale < maxScale || maxScale < 0.0f)
         {
            source.width = (int)(scale*source.width);
            source.height = (int)(scale*source.height);

            return scale;
         }
         else
         {
            source.width = (int)(maxScale*source.width);
            source.height = (int)(maxScale*source.height);

            return maxScale;
         }
      }
      else
         return 1.0f;
   }

   /**
    * If the given component is part of a JLayeredPane the lowest layer
    * of that JLayeredPane is used for the repaint; otherwise the component itself.
   * The repaint area is restricted to the leaf area; exception: flag everything is true.
    */
   public static void invokeRepaintOnLayeredRoot(Component leaf, boolean everything)
   {
      // very dodgy workaround for being in a layered pane
      Component con = leaf.getParent();
      if (con != null && con instanceof JLayeredPane)
      {
         JLayeredPane lp = (JLayeredPane)con;
         Component[] cs = null;
         synchronized(leaf.getTreeLock())
         {
            cs = lp.getComponentsInLayer(lp.lowestLayer());
         }
         if (cs != null && cs.length > 0)
            con = cs[0];
         else
            con = leaf;

         // otherwise, when repaint is invoked normally
         // while we are in a higher layer of a layered pane
         // display flickers sometime

      }
      else
      {
         con = leaf;
      }

      if (everything || con == leaf)
         con.repaint();
      else
      {
         Rectangle r = leaf.getBounds();
         con.repaint(r.x, r.y, r.width, r.height);
      }
   }

   /**
    * The given component is the glass pane of a window: 
    * The content pane of the window is used for the repaint.
    */
   public static void invokeRepaintOnContentPane(Component glassPane, Rectangle restrictedArea)
   {
      Component con = glassPane;
      Rectangle r = restrictedArea;
      
      if (r == null)
         r = con.getBounds();

      Window w = SwingUtilities.windowForComponent(glassPane);
      if (w != null && w instanceof JWindow && ((JWindow)w).getGlassPane() == glassPane)
         con = ((JWindow)w).getContentPane();

      con.repaint(r.x, r.y, r.width, r.height);
   }

   public static boolean Contains(Container container, Component component)
   {
      if (container != null && component != null)
      {
         Component[] comps = container.getComponents();
         for (int i=0; i<comps.length; ++i)
         {
            if (comps[i] == component)
               return true;
         }
      }
      
      return false;
   }
}