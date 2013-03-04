package imc.epresenter.player.util;

import java.awt.*;
import javax.swing.*;

public class FixedPicturedPanel extends JPanel
{
   private Image _background;
   
   
   public FixedPicturedPanel(Image back)
   {
      _background = back;

      Dimension minPicDim = new Dimension(back.getWidth(null), back.getHeight(null));

      setMinimumSize(minPicDim);
      setPreferredSize(minPicDim);
   }

   protected void paintComponent(Graphics g)
   {
      super.paintComponent(g);

      g.drawImage(_background, 0, 0, this);
   }

}