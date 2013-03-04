package imc.epresenter.player.util;

import java.awt.*;
import javax.swing.*;

public class VariablePicturedPanel extends JPanel
{
   private Image _left, _middle, _right;
   
   
   public VariablePicturedPanel(Image left, Image middle, Image right)
   {
      int leftW = left.getWidth(null), leftH = left.getHeight(null);
      int middleW = middle.getWidth(null), middleH = middle.getHeight(null);
      int rightW = right.getWidth(null), rightH = right.getHeight(null);
      if (leftH != middleH || middleH != rightH)
         throw new IllegalArgumentException("Heights of the images ("+leftH
                                            +","+middleH+","+rightH+") differ.");

      Dimension minPicDim = new Dimension(leftW+rightW, middleH);

      setMinimumSize(minPicDim);
      setPreferredSize(minPicDim);

      _left = left;
      _middle = middle;
      _right = right;
   }

   protected void paintComponent(Graphics g)
   {
      super.paintComponent(g);

      Rectangle clip = g.getClip().getBounds();
      
      int w = getWidth();
      int leftW = _left.getWidth(null);
      int rightW = _right.getWidth(null);
      int space = w-leftW-rightW;
      int middleW = _middle.getWidth(null);
         
      if (space > 0)
      {
         int startX = Math.max(leftW, clip.x);
         int endX = Math.min(clip.x+clip.width, leftW+space); 
         for (int x=startX; x<endX; x+=middleW)
            g.drawImage(_middle, x,0, this);
      }

      if (clip.x < leftW)
         g.drawImage(_left, 0,0, this);
      if (clip.x+clip.width > w-rightW)
         g.drawImage(_right, w-rightW,0, this);


   }

}