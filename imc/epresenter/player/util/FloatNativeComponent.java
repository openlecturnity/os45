package imc.epresenter.player.util;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import imc.epresenter.player.Manager;

public class FloatNativeComponent extends JPanel implements ComponentListener
{
   private Window thisRoot_;
   private JWindow nativeHolder_;

   public FloatNativeComponent(Component nativeComponent)
   {
      setPreferredSize(nativeComponent.getPreferredSize());
      setMinimumSize(nativeComponent.getMinimumSize());
      addComponentListener(this);

      nativeHolder_ = new JWindow();
      nativeHolder_.getContentPane().setLayout(new GridLayout(1,1));
      nativeHolder_.getContentPane().add("Center", nativeComponent);
      nativeHolder_.pack();
   }

   public void componentMoved(ComponentEvent e)
   {
      Component source = e.getComponent();

      if (source == thisRoot_)
      {
         Point p = new Point(0,0);
         SwingUtilities.convertPointToScreen(p, this);
         nativeHolder_.setLocation(p);
      }
   }
   
   public void componentResized(ComponentEvent e)
   {
      if (thisRoot_ == null)
      {
         thisRoot_ = SwingUtilities.windowForComponent(this);
         thisRoot_.addComponentListener(this);

         Point p = new Point(0,0);
         SwingUtilities.convertPointToScreen(p, this);
         nativeHolder_.setLocation(p);
         nativeHolder_.setVisible(true);
         nativeHolder_.toFront();
      }

      Component source = e.getComponent();
      if (source == this)
      {
         nativeHolder_.setSize(this.getSize());
      }
   }

   public void componentShown(ComponentEvent e)
   {
      Component source = e.getComponent();
      
      if (source == thisRoot_)
      {
         nativeHolder_.setVisible(true);
         nativeHolder_.toFront();
      }
   }

   public void componentHidden(ComponentEvent e)
   {
      Component source = e.getComponent();
      
      if (source == thisRoot_)
      {
         nativeHolder_.setVisible(false);
      }
   }

   
}
