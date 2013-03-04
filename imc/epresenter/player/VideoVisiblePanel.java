package imc.epresenter.player;

import java.awt.*;
import java.awt.event.*;
import javax.swing.JPanel;
import javax.swing.JLayeredPane;
import javax.swing.border.Border;

import imc.lecturnity.util.ui.LayoutUtils;

public class VideoVisiblePanel extends VideoPanel
{
   private Component[]         components_;
   private Component           visibleComponent_;
   private boolean             componentHidden_ = false;
   private boolean             paintBorder_ = false;
   
   // if this is true: only deactivate (hide) a component if another
   // one can be shown
   private boolean             exorActivate_ = false;
      
   public VideoVisiblePanel(Component[] components)
   {
      this (components, false, false);
   }
   
   public VideoVisiblePanel(Component[] components, boolean paintBorder, boolean exorActivate)
   {
      paintBorder_ = paintBorder;
      exorActivate_ = exorActivate;

      components_ = components;
      for (int i=0; i<components_.length; ++i)
      {
         components_[i].setVisible(false);
         add(components_[i]);
      }

      super.setLayout(null);
      super.setForeground(new Color(0x40,0x40,0x40,0x80)); // shadow color

      super.setOpaque(false);
      super.setBackground(Color.black);
   }

   public void activate(Component c)
   {
      boolean isContained = false;
      for (int i=0; i<components_.length; ++i)
         if (components_[i] == c)
            isContained = true;


      if (c != visibleComponent_)
      {
         visibleComponent_ = null;
         for (int i=0; i<components_.length; ++i)
         {
            if (components_[i] != c)
            {
               if (components_[i].isVisible())
               {
                  
                  if (isContained || !exorActivate_)
                  {

                     // doesn't need to be sychronized with the Toolkit
                     // as setVisible (show) properly uses 
                     // Toolkit.getEventQueue().postEvent(e);
                 
                     components_[i].setVisible(false);
                     //System.out.println("-"+components_[i]);
      
                     Rectangle r = components_[i].getBounds();
                     r.x -= 2;
                     r.y -= 2;
                     r.width += 10;
                     r.height += 10;
                     repaint(r);

                     //System.out.println("videoRepaintRequest="+r);

                     //System.out.println("--- Hidden: "+components_[i]);
                  }
               }
            }
            else
            {

               components_[i].setVisible(true);
               //System.out.println("+"+components_[i]);
               visibleComponent_ = components_[i];
               
               Rectangle r = components_[i].getBounds();
               r.x -= 2;
               r.y -= 2;
               r.width += 10;
               r.height += 10;
               repaint(r);

               //System.out.println("+++ Shown: "+components_[i]);
               
            }
         }
      }
   }

   public void setBounds(int x, int y, int width, int height)
   {
      //System.out.println(x+","+y+" "+width+"x"+height);
      boolean resized = getWidth() != width || getHeight() != height;

      super.setBounds(x, y, width, height);

      if (resized)
      {
         for (int i=0; i<components_.length; ++i)
            positionComponent(components_[i]);
      }
   }


   /*
   public void validateChange()
   {
      if (componentAdded_ || componentRemoved_)
         super.revalidate();
      
      if (componentAdded_)
         positionComponent();
      
      if (componentAdded_ || componentRemoved_)
      {
         // full repaint needed to also redraw the background of the layered pane
         if (LayoutUtils.isComponentFullScreen(this))
         {
            // very dodgy workaround for being in a layered pane
            LayoutUtils.invokeRepaintOnLayeredRoot(this, true);
         }
      }


      componentAdded_ = false;
      componentRemoved_ = false;

         
   }
   //*/

   //
   // three methods for hiding the video during user scaling
   //
   public void hideVideo()
   {
      componentHidden_ = true;

      if (visibleComponent_ != null)
      {
         visibleComponent_.setVisible(false);
      }
   }

   public boolean isVideoVisible()
   {
      return (visibleComponent_ != null && visibleComponent_.isVisible());
   }

   public void showVideo()
   {
      componentHidden_ = false;
         
      if (visibleComponent_ != null && !isVideoVisible())
      {
         visibleComponent_.setVisible(true);
      }
   }

   /*
   public void setCursor(Cursor cursor)
   {
      for (int i=0; i<components_.length; ++i)
         components_[i].setCursor(cursor);
      
      super.setCursor(cursor);
   }
   */

   public void addMouseListener(MouseListener ml)
   {
      for (int i=0; i<components_.length; ++i)
         components_[i].addMouseListener(ml);
      
      super.addMouseListener(ml);
  }

   public void removeMouseListener(MouseListener ml)
   {
      for (int i=0; i<components_.length; ++i)
         components_[i].removeMouseListener(ml);
      
      super.removeMouseListener(ml);
   }

   public void addMouseMotionListener(MouseMotionListener mml)
   {
      for (int i=0; i<components_.length; ++i)
         components_[i].addMouseMotionListener(mml);
      
      super.addMouseMotionListener(mml);
   }

   public void removeMouseMotionListener(MouseMotionListener mml)
   {
      for (int i=0; i<components_.length; ++i)
         components_[i].removeMouseMotionListener(mml);
      
      super.removeMouseMotionListener(mml);
   }
      
   public void setBorder(Border b)
   {
      //throw new UnsupportedOperationException("Borders are not allowed "
      //                                        +"for this component.");

      // do nothing, especially do not add it
   }

   protected void paintChildren(Graphics g)
   {
      if (visibleComponent_ != null)
         visibleComponent_.paint(g);

   }


   //*
   protected void paintComponent(Graphics g)
   {
      // mustn't be sychronized with remove/add/layout-operations
      // as the AWTEventQueue first obtains a lock on the TreeLock 
      // (for the Repaint-Event) before invoking paint
      // (and paintComponent())
     
      super.paintComponent(g);
     
      if (super.isOpaque() != true) // do overlay paint
      {
         Graphics2D g2d = (Graphics2D)g;
     
         Dimension dimScreen = Toolkit.getDefaultToolkit().getScreenSize();
         Dimension d = super.getSize();

         // not fullscreen
         if (!paintBorder_ && (d.width < dimScreen.width || d.height < dimScreen.height))
         {
            return;
         }

         
         Component component = visibleComponent_;

         if (component != null)
         {
            Rectangle videoRect = new Rectangle(0,0,0,0);
            videoRect = component.getBounds();

            // it is fullscreen, now if there is a video fill the area with a
            // semi-transparent color to blur the underlying layer
            // 
            // -> not anymore
            //g.setColor(getBackground());
            //g.fillRect(0,0,dimScreen.width,dimScreen.height);
         
            //
            // if enough space draw a shadow and a border
            // around the video surface
            //
            int shadow = 6;

            if (g.getClip().intersects(videoRect.x-shadow, videoRect.y-shadow,
                                       videoRect.width+shadow*2, videoRect.height+shadow*2))
            {

               if (videoRect.width < dimScreen.width-shadow*2 &&
                   videoRect.height < dimScreen.height-shadow*2)
               {
                  long seconds = (System.currentTimeMillis()/1000)%60;

                  int shadow2 = shadow;
                  //if ((seconds/10)%2==0) 
                  //   shadow2 = shadow/2; // härterer Verlauf

                  setForeground(new Color(0x20,0x20,0x20,0x80/shadow2));

                  g.setColor(getForeground());

                  Rectangle rH = new Rectangle(videoRect.x+shadow, 
                                               videoRect.y+videoRect.height, 
                                               videoRect.width-shadow, shadow);
                  Rectangle rV = new Rectangle(videoRect.x+videoRect.width, 
                                               videoRect.y+shadow, 
                                               shadow, videoRect.height);

                  for (int i=0; i<shadow2; i++)
                  {
                     g2d.fill(rH);
                     g2d.fill(rV);

                     rH.x += 1; rH.height -= 1; rH.width -= 1;
                     rV.y += 1; rV.height -= 2; rV.width -= 1;
                  }

                  for (int i=0; i<2; ++i)
                     g2d.drawRect(videoRect.x-1, videoRect.y-1, 
                                  videoRect.width+1, videoRect.height+1);
               }
            }
         }  // if component_ != null
      } // if do overlay paint

   }
   
   void revalidateVideo()
   {
      if (visibleComponent_ != null)
      {
         visibleComponent_.invalidate();
      }
      this.validate();
      System.out.println(visibleComponent_);
   }

   void positionComponent()
   {
      positionComponent(visibleComponent_);
      System.out.println(visibleComponent_);
   }

   private void positionComponent(Component c)
   {
      if (c != null)
      {
         // position the component with respect to the available size
         Dimension preferredComponentSize = c.getPreferredSize();
            
         int videoWidth = preferredComponentSize.width;
         int videoHeight = preferredComponentSize.height;

         Dimension d = super.getSize();
         Dimension p = new Dimension(videoWidth, videoHeight);

         LayoutUtils.scale(p, d, 1.0f);
         // "-1.0f": Bug 1482: allow videos to be scaled greater than 100%
         // well: unfortunately that leads to the clips on slides being also 
         // scaled (and can get very big); thus it is now again deactivated
    
         Rectangle newBounds = new Rectangle(
            (d.width-p.width)/2, (d.height-p.height)/2, p.width, p.height);

         if (!newBounds.equals(c.getBounds()))
         {
            c.setBounds(newBounds);
             
         }

         //if (p.width > 0 && p.height > 0)
         //   timeChangeProhibited_ = true;

      }
   }

}