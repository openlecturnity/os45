package imc.epresenter.filesdk.util;

/*
 * File:             CustomButton.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: CustomButton.java,v 1.1 2001-11-06 15:43:27 danielss Exp $
 */

import java.awt.Image;
import java.awt.Dimension;
import java.awt.Graphics;
import javax.swing.ImageIcon;
import javax.swing.JPanel;

import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import java.util.Vector;

/**
 * Custom implementation of a button using a <tt>JPanel</tt> and four
 * different image files carrying the same base names. For a single
 * button, you will need four images: one normal image, one for
 * hovering (mouse over button), one for when the button has been
 * pressed, and one if it is disabled.
 *
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class CustomButton extends JPanel 
{
   private static final int STATE_NORMAL   = 0;
   private static final int STATE_HOVER    = 1;
   private static final int STATE_PRESSED  = 2;
   private static final int STATE_DISABLED = 3;

   private Image[] image_ = new Image[4];
   private int state_;

   private boolean isInButton_ = false;
   private boolean isPressed_  = false;

   private Dimension size_;
   private String    actionCommand_;
   private Vector    actionListeners_;

   /**
    * Create a new <tt>CustomButton</tt> with the given
    * parameters. The <tt>resource</tt> parameters takes the path to
    * the image resources (as gif or jpg files). The constructor
    * supposes that there are four image files in the following way:
    * the resource string contains the base name of the button images,
    * i.e. <tt>&quot/imc/epresenter/converter/images/plus&quot&</tt>. 
    * Then the constructor will add the following suffixes plus the
    * suffix given in <tt>suffix</tt>:
    * <ul><li><tt>_n</tt> for the mouseOver event button image</li>
    *     <li><tt>_p</tt> for the mousePressed event button image</li>
    *     <li><tt>_d</tt> for the disable button image</li>
    *     <li> nothing for the normal button image</li>
    * </ul>
    * In the above example, <tt>CustomButton</tt> would look the
    * images <tt>plus.gif</tt>, <tt>plus_n.gif</tt>,
    * <tt>plus_d.gif</tt> and <tt>plus_p.gif</tt>, if <tt>suffix</tt>
    * equalled <tt>.gif</tt>.
    *
    * @param resource the base name of the button images
    * @param suffix the suffix of the images (e.g. <tt>.gif</tt>)
    * @param actionCommand the action command used for the action
    * events
    */
   public CustomButton(String resource, String suffix, 
                       String actionCommand)
   {
      super();
      state_         = STATE_NORMAL;
      actionCommand_ = "" + actionCommand;

      actionListeners_ = new Vector();
      loadImages(resource, suffix);

      addMouseListener(new HoverChecker());
   }

   private void loadImages(String resource, String suffix)
   {
      ImageIcon icon = new ImageIcon
	 (getClass().getResource(resource + suffix));
      image_[STATE_NORMAL] = icon.getImage();

      icon = new ImageIcon
	 (getClass().getResource(resource + "_n" + suffix));
      image_[STATE_HOVER] = icon.getImage();

      icon = new ImageIcon
	 (getClass().getResource(resource + "_p" + suffix));
      image_[STATE_PRESSED] = icon.getImage();

      icon = new ImageIcon
	 (getClass().getResource(resource + "_d" + suffix));
      image_[STATE_DISABLED] = icon.getImage();

      size_ = new Dimension(image_[STATE_NORMAL].getWidth(this),
                            image_[STATE_NORMAL].getHeight(this));
   }

   public void addActionListener(ActionListener actionListener)
   {
      synchronized(actionListeners_)
      {
         actionListeners_.addElement(actionListener);
      }
   }

   public void removeActionListener(ActionListener actionListener)
   {
      synchronized(actionListeners_)
      {
         actionListeners_.removeElement(actionListener);
      }
   }

   public void setActionCommand(String actionCommand)
   {
      actionCommand_ = actionCommand;
   }

   public String getActionCommand()
   {
      return "" + actionCommand_;
   }

   private void postActionEvents()
   {
      ActionEvent actionEvent = new ActionEvent(this, ActionEvent.ACTION_PERFORMED,
                                                "" + actionCommand_);
      synchronized(actionListeners_)
      {
         for (int i=0; i<actionListeners_.size(); i++)
            ((ActionListener) 
             actionListeners_.elementAt(i)).actionPerformed(actionEvent);
      }
   }

   /**
    * Returns the size of the images used.
    */
   public Dimension getPreferredSize()
   {
      return size_;
   }

   /**
    * Returns the size of the images used.
    */
   public Dimension getMaximumSize()
   {
      return getPreferredSize();
   }

   /**
    * Returns the size of the images used.
    */
   public Dimension getMinimumSize()
   {
      return getPreferredSize();
   }

   /**
    * Dis-/Enables the <tt>CustomButton</tt> instance. The image used
    * in the button is set appropriately.
    */
   public void setEnabled(boolean state)
   {
      if (!state)
         state_ = STATE_DISABLED;
      else
         state_ = STATE_NORMAL;
      super.setEnabled(state);
   }

   public void paint(Graphics g)
   {
      if (image_[state_] != null)
         g.drawImage(image_[state_], 0, 0, this);
   }

   private class HoverChecker implements MouseListener
   {
      public void mouseClicked(MouseEvent e)
      {
      }

      public void mousePressed(MouseEvent e)
      {
         if (!isEnabled())
            return;
         isPressed_ = true;
         if (isInButton_)
            state_ = STATE_PRESSED;
         else
            state_ = STATE_HOVER;
         repaint();
      }

      public void mouseReleased(MouseEvent e)
      {
         if (!isEnabled())
            return;
         isPressed_ = false;
         if (isInButton_)
         {
            state_ = STATE_HOVER;
            postActionEvents();
         }
         else
            state_ = STATE_NORMAL;
         repaint();
      }

      public void mouseEntered(MouseEvent e)
      {
         if (!isEnabled())
            return;
         isInButton_ = true;
         if (isPressed_)
            state_ = STATE_PRESSED;
         else
            state_ = STATE_HOVER;
         repaint();
      }

      public void mouseExited(MouseEvent e)
      {
         if (!isEnabled())
            return;
         isInButton_ = false;
         if (isPressed_)
            state_ = STATE_HOVER;
         else
            state_ = STATE_NORMAL;
         repaint();
      }
   }
}
