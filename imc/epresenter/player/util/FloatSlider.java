package imc.epresenter.player.util;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

public class FloatSlider extends JPanel
implements MouseListener, MouseMotionListener, ComponentListener
{
   private Image knob_;
   private Image over_;
   private Image pressed_;
   private Image disabled_; // can be null
   private Rectangle knobBounds_;
   private float value_;
   private ChangeListener listener_;
   private boolean valueAdjusting_;
   private boolean mouseOver_;

   public FloatSlider(Image knob)
   {
      this(knob, knob, knob, null);
   }

   public FloatSlider(Image knob, Image over, Image pressed, Image disabled)
   {
      knob_ = knob;
      over_ = over;
      pressed_ = pressed;
      disabled_ = disabled;
      knobBounds_ = new Rectangle(0,0, knob_.getWidth(null), knob_.getHeight(null));
      if (over_.getWidth(null) != knobBounds_.width ||
          over_.getHeight(null) != knobBounds_.height ||
          pressed_.getWidth(null) != knobBounds_.width ||
          pressed_.getHeight(null) != knobBounds_.height)
         throw new IllegalArgumentException("Knob (image) sizes do not match: "
                                            +knobBounds_.width+"x"+knobBounds_.height+" "
                                            +over_.getWidth(null)+"x"+over_.getHeight(null)+" "
                                            +pressed_.getWidth(null)+"x"+pressed_.getHeight(null));
      if (disabled_ != null)
      {
         if (disabled_.getWidth(null) != knobBounds_.width ||
             disabled_.getHeight(null) != knobBounds_.height)
            throw new IllegalArgumentException("Knob (image) sizes do not match: "
                                               +knobBounds_.width+"x"+knobBounds_.height+" "
                                               +disabled_.getWidth(null)+"x"+disabled_.getHeight(null));
      }
      
      setMinimumSize(new Dimension(knobBounds_.width, knobBounds_.height));
      setPreferredSize(new Dimension(4*getMinimumSize().width, getMinimumSize().height));

      setOpaque(false);

      if (null == disabled_)
      {
         addMouseListener(this);
         addMouseMotionListener(this);
      }
      addComponentListener(this);
   }

   public Dimension getKnobSize()
   {
      return new Dimension(knobBounds_.width, knobBounds_.height);
   }

   public void setExternalValue(float v)
   {
      if (v != value_)
      {
         setValue(v);
         if (listener_ != null) listener_.stateChanged(new ChangeEvent(this));
      }
   }

   public void setValue(float v)
   {
      if (v < 0 || v > 1)
      {
         System.err.println("Value ("+v+") is out of bounds [0,1].");
         
         if (v < 0)
            v = 0;
         if (v > 1)
            v = 1;
         
         //throw new IllegalArgumentException("Value ("+v+") is out of bounds [0,1].");
      }

      float oldValue = value_;
      value_ = v;
      makeRepaint(oldValue, v);
   }

   public float getValue()
   {
      return value_;
   }

   public boolean isValueAdjusting()
   {
      return valueAdjusting_;
   }

   public void setChangeListener(ChangeListener l)
   {
      listener_ = l;
   }



   protected void paintComponent(Graphics g)
   {
      //super.paintComponent(g); // does anything while being transparent?

      /*
      g.setColor(Color.magenta);
      Rectangle r = new Rectangle(0,0,getSize().width, getSize().height);//g.getClip().getBounds();
      r.width -= 1; r.height -= 1;
      ((Graphics2D)g).draw(r);
      //*/

      Image i = knob_;
      if (disabled_ != null)
      {
         i = disabled_;
      }
      else
      {
         if (valueAdjusting_)
            i = pressed_;
         else if (mouseOver_)
            i = over_;
      }

      int innerWidth = getWidth()-knobBounds_.width;
      int knobX = (int)(value_*innerWidth);
      g.drawImage(i, knobX, 0, this);
      knobBounds_.x = knobX;
   }




   public void mousePressed(MouseEvent e)
   {
      valueAdjusting_ = true;
      
      /*
      if (!knobBounds_.contains(new Point(e.getX(), e.getY())))
      {
         mouseDragged(e);
      }
      else
         repaint();
         */
      mouseDragged(e);
   }

   public void mouseReleased(MouseEvent e)
   {
      valueAdjusting_ = false;
      
      float newValue = calculateNewValue(e.getX());
      float oldValue = value_;
      value_ = newValue;
      makeRepaint(oldValue, newValue, true);

      if (listener_ != null) listener_.stateChanged(new ChangeEvent(this));
   }

   public void mouseDragged(MouseEvent e)
   {
      if (valueAdjusting_)
      {
         float newValue = calculateNewValue(e.getX());
         float oldValue = value_;
         value_ = newValue;
         makeRepaint(oldValue, newValue);

         if (listener_ != null) listener_.stateChanged(new ChangeEvent(this));
      }
   }

   public void componentResized(ComponentEvent e)
   {
      //System.out.println("S:"+getSize());
   }
   
   public void mouseEntered(MouseEvent e)
   {
      mouseOver_ = true;
      makeRepaint(value_, value_, true);
   }

   public void mouseExited(MouseEvent e)
   {
      mouseOver_ = false;
      makeRepaint(value_, value_, true);
   }
   
   public void mouseClicked(MouseEvent e) {}
   public void mouseMoved(MouseEvent e) {}
   public void componentHidden(ComponentEvent e) {}
   public void componentShown(ComponentEvent e) {}
   public void componentMoved(ComponentEvent e) {}
   
   private float calculateNewValue(int mouseX)
   {
      int validStartX = knobBounds_.width/2;
      int validEndX = validStartX+getWidth()-knobBounds_.width-1;
      if (mouseX < validStartX) mouseX = validStartX;
      else if (mouseX > validEndX) mouseX = validEndX;
      
      return (mouseX-validStartX)/(float)(validEndX-validStartX);
   }

   private void makeRepaint(float oldValue, float newValue)
   {
      makeRepaint(oldValue, newValue, false);
   }
   
   private void makeRepaint(float oldValue, float newValue, boolean nocheck)
   {
      int innerWidth = getWidth()-knobBounds_.width;
      int oldX = (int)(oldValue*innerWidth);
      int newX = (int)(newValue*innerWidth);

      if (oldX != newX || nocheck)
      {
         int minX = knobBounds_.width/2+Math.min(oldX, newX);
         int maxX = knobBounds_.width+Math.max(oldX, newX);
         
         repaint(minX-knobBounds_.width/2, 0,
                 maxX-minX+knobBounds_.width/2, knobBounds_.height);
      }
   }
}