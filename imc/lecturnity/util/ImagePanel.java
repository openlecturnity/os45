package imc.lecturnity.util;

/*
 * File:             Wizard.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: ImagePanel.java,v 1.4 2004-05-11 12:52:21 danielss Exp $
 */

import java.awt.Image;
import java.awt.Dimension;
import java.awt.Graphics;

import javax.swing.JPanel;
import javax.swing.ImageIcon;

/**
 * Instances of this class can be used to display an image in a
 * panel. The panel resizes itself automatically to the size of the
 * image (i.e. the preferred size of the panel is set to the image
 * size).
 */
public class ImagePanel extends JPanel
{
   private Image image_;
   private int width_, height_;

   /**
    * Creates an <tt>ImagePanel</tt> containing the image given in the
    * resource. The resource must be a path to a resource within the
    * class path (uses <tt>getClass().getResource()</tt>).
    */
   public ImagePanel(String resourceName)
   {
      super();
      
      ImageIcon imageIcon = new ImageIcon
         (getClass().getResource(resourceName));
      image_ = imageIcon.getImage();

      width_ = image_.getWidth(this);
      height_ = image_.getHeight(this);

      setOpaque(false);
   }

   public Dimension getPreferredSize()
   {
      width_ = image_.getWidth(this);
      height_ = image_.getHeight(this);

      return new Dimension(width_, height_);
   }

   public void paint(Graphics g)
   {
      super.paint(g);
      g.drawImage(image_, 0, 0, this);
   }
}
