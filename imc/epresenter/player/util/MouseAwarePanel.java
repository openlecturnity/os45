package imc.epresenter.player.util;

import java.awt.*;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import javax.swing.*;

public class MouseAwarePanel extends JPanel implements MouseListener
{
   private Color standard_;
   private Color highlightFocus_;
   private Color highlightSelection_;
   private boolean mouseOver_;
   private boolean mousePressed_;

   public MouseAwarePanel (Color standard, Color focus, Color selection)
   {
      standard_ = standard;
      setBackground(standard_);
      highlightFocus_ = focus;
      highlightSelection_ = selection;

      setOpaque(true);

      addMouseListener(this);
   }

   
   public void mouseEntered(MouseEvent e)
   {
      mouseOver_ = true;
      if (!mousePressed_)
      {
         setBackground(highlightFocus_);
         repaint();
      }
   }

   public void mouseExited(MouseEvent e)
   {
      mouseOver_ = false;
      if (!mousePressed_)
      {
         setBackground(standard_);
         repaint();
      }
   }

   public void mousePressed(MouseEvent e) 
   {
      mousePressed_ = true;
      setBackground(highlightSelection_);
      repaint();
   }

   public void mouseReleased(MouseEvent e)
   {
      mousePressed_ = false;
      if (mouseOver_)
         setBackground(highlightFocus_);
      else
         setBackground(standard_);
      repaint();
   }
   
   
   public void mouseClicked(MouseEvent e) {}
   
}