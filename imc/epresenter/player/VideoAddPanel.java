package imc.epresenter.player;

import java.awt.*;
import java.awt.event.*;
import javax.swing.JPanel;
import javax.swing.JLayeredPane;
import javax.swing.border.Border;

import imc.lecturnity.util.ui.LayoutUtils;

public class VideoAddPanel extends VideoPanel
{
   private Component           component_ = null;
   private MouseListener       mouseListener_ = null;
   private MouseMotionListener motionListener_ = null;
   private Dimension           preferredComponentSize_ = null;
   private boolean             componentAdded_ = false;
   private boolean             componentRemoved_ = false;
   private boolean             componentHidden_ = false;
      
   public VideoAddPanel()
   {
      super.setLayout(null);
      super.setForeground(new Color(0x40,0x40,0x40,0x80)); // shadow color

      // initially do non-overlay paint; component is opaque
      super.setOpaque(true);
      super.setBackground(Color.black);

      Thread t = new Thread("VideoPanel: Location Check") {
         public void run()
         {
            while(true)
            {
               if (component_ != null)
               {
                  System.out.println("VideoPanelLoop(): "+component_.getBounds());
               }

               try { sleep(10000); } catch (InterruptedException exc) {}
            }
         }
      };
      t.start();
   }

   public void setOverlayPaint(boolean b)
   {
      if (b != super.isOpaque())
         return;

      if (b)
      {
         super.setOpaque(false);
         super.setBackground(new Color(0x80,0x80,0x80,0x80)); 
      }
      else
      {
         super.setOpaque(true);
         super.setBackground(Color.black); 
      }
   }



   public Component add(Component c)
   {
      if (component_ != null)
         throw new IllegalStateException("More than one component not "
                                         +"supported for this container.");

      component_ = c;
      preferredComponentSize_ = component_.getPreferredSize();
      if (mouseListener_ != null)
         component_.addMouseListener(mouseListener_);
      if (motionListener_ != null)
         component_.addMouseMotionListener(motionListener_);

      component_.addComponentListener(new ComponentAdapter() {
         public void componentResized(ComponentEvent evt)
         {
            System.out.println("componentResized(): "+component_.getBounds());
         }
      });

      // component_ is most likely an overlay
      // so it is instantly visible after add() but initially
      // has the wrong position
      component_.setSize(0,0);
      System.out.println("add(): "+component_.getBounds());

      Component rc = super.add(component_);

      componentAdded_ = true;
         
      return rc;
   }


   public void remove(Component c)
   {
      //System.out.print("r");
     
      if (component_ == c)
      {
         component_.removeMouseListener(mouseListener_);
         component_.removeMouseMotionListener(motionListener_);
         component_ = null;

         System.out.println("remove(): ");
      }


      //try { Thread.sleep(1); } catch (InterruptedException exc) {}

      super.remove(c);

      componentRemoved_ = true;
          
      // validate
      // must be done from outside (with revalidate)

      //System.out.print("R");
   }

   public void setBounds(int x, int y, int width, int height)
   {
      //System.out.println(x+","+y+" "+width+"x"+height);
      boolean resized = getWidth() != width || getHeight() != height;

      super.setBounds(x, y, width, height);

      if (resized)
         positionComponent();
   }



   //*
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
      if (component_ != null)
      {
         componentHidden_ = true;
         component_.setSize(0,0);
         componentRemoved_ = true;
         validateChange();
      }
   }

   public boolean isVideoVisible()
   {
      return (component_ != null && component_.getSize().width > 0);
   }

   public void showVideo()
   {
      if (!isVideoVisible())
      {
         positionComponent();
         componentHidden_ = false;
         componentAdded_ = true;
         validateChange();
         
      }
   }

   public void addMouseListener(MouseListener ml)
   {
      if (component_ != null)
         component_.addMouseListener(ml);

      super.addMouseListener(ml);

      mouseListener_ = ml;
   }

   public void removeMouseListener(MouseListener ml)
   {
      if (ml == mouseListener_)
         mouseListener_ = null;

      if (component_ != null)
         component_.removeMouseListener(ml);

      super.removeMouseListener(ml);
   }

   public void addMouseMotionListener(MouseMotionListener mml)
   {
      if (component_ != null)
         component_.addMouseMotionListener(mml);

      super.addMouseMotionListener(mml);

      motionListener_ = mml;
   }

   public void removeMouseMotionListener(MouseMotionListener mml)
   {
      if (mml == motionListener_)
         motionListener_ = null;

      if (component_ != null)
         component_.removeMouseMotionListener(mml);

      super.removeMouseMotionListener(mml);
   }
      
   public void setBorder(Border b)
   {
      if (component_ != null)
         throw new UnsupportedOperationException("Borders are not allowed "
                                                 +"for this component.");

   }

   /*
   public void componentResized(ComponentEvent evt)
   {
      positionComponent();
      //System.out.println("resized");

         

   }

   public void componentShown(ComponentEvent evt) { }
   public void componentHidden(ComponentEvent evt) { }
   public void componentMoved(ComponentEvent evt) { }
   //*/

   //*
   protected void paintComponent(Graphics g)
   {
      // mustn't be sychronized with remove/add/layout-operations
      // as the AWTEventQueue first obtains a lock on the TreeLock 
      // (for the Repaint-Event) before invoking paint and the like
     
      super.paintComponent(g);
     
      if (super.isOpaque() != true) // do overlay paint
      {
         Graphics2D g2d = (Graphics2D)g;
     
         Dimension dimScreen = Toolkit.getDefaultToolkit().getScreenSize();
         Dimension d = super.getSize();

         // not fullscreen
         if (d.width < dimScreen.width || d.height < dimScreen.height)
         {
            return;
         }

         // it is fullscreen, now if there is a video fill the area with a
         // semi-transparent color to blur the underlying layer
         
         //if (component_ != null)
         //{
                   
         if (component_ != null)
         {
            Rectangle videoRect = new Rectangle(0,0,0,0);
            videoRect = component_.getBounds();

            g.setColor(getBackground());
            g.fillRect(0,0,dimScreen.width,dimScreen.height);
         
            //
            // if enough space draw a shadow and a border
            // around the video surface
            //
            int shadow = 6;

            if (g.getClip().intersects(videoRect.x, videoRect.y,
                                       videoRect.width+shadow, videoRect.height+shadow))
            {

               if (videoRect.width < dimScreen.width-shadow*2 &&
                   videoRect.height < dimScreen.height-shadow*2)
               {
                  long seconds = (System.currentTimeMillis()/1000)%60;

                  int shadow2 = shadow;
                  //if ((seconds/10)%2==0) // härterer Verlauf
                  //   shadow2 = shadow/2;

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

               }
            }
         }  // if component_ != null
      } // if do overlay paint

   }
   
   private void positionComponent()
   {
      if (component_ != null && !componentHidden_)
      {
            
         // position the component with respect to the available size
         int videoWidth = preferredComponentSize_.width;
         int videoHeight = preferredComponentSize_.height;

         Dimension d = super.getSize();
         Dimension p = new Dimension(videoWidth, videoHeight);

         LayoutUtils.scale(p, d, 1.0f);
    
         Rectangle newBounds = new Rectangle(
            (d.width-p.width)/2, (d.height-p.height)/2, p.width, p.height);

         if (!newBounds.equals(component_.getBounds()))
         {
            component_.setBounds(newBounds);
            System.out.println("positionComponent(): "+component_.getBounds());
         }

         //if (p.width > 0 && p.height > 0)
         //   timeChangeProhibited_ = true;

      }
   }

}