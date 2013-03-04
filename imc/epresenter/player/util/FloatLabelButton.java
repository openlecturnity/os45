package imc.epresenter.player.util;



import java.awt.*;

import javax.swing.*;



import imc.epresenter.player.Manager;



public class FloatLabelButton extends JButton

{

   private Insets _textInsets;

   private Image _image;

   private Image _image_pressed;



   public FloatLabelButton(String text, Image image1, Image image2, Insets insets)

   {

      super(text);



      int w1 = image1.getWidth(null), h1 = image1.getHeight(null);

      int w2 = image2.getWidth(null), h2 = image2.getHeight(null);



      if (w1 != w2 || h1 != h2)

         throw new IllegalArgumentException("Picture sizes do not match: "+w1+"x"+h1

                                            +" vs. "+w2+"x"+h2+".");





      setMinimumSize(new Dimension(w1, h1));

      setPreferredSize(getMinimumSize());

      setContentAreaFilled(false);

      

      _textInsets = insets;

      _image = image1;

      _image_pressed = image2;



      setFont(getFont().deriveFont(Font.BOLD).deriveFont(14.0f));

   }







   protected void paintComponent(Graphics g)

   {

      

      g.setFont(getFont());

      

      FontMetrics fm = g.getFontMetrics();

      

      /* does not work very well

      int fontWidth = fm.stringWidth(getText());

      int availableWidth = getWidth()-_textInsets.left-_textInsets.right;

      Font scaledFont = null;

      if (fontWidth > availableWidth)

      {

         scaledFont = getFont().deriveFont(getFont().getSize()*(availableWidth/(float)fontWidth));

         g.setFont(scaledFont);

         fm = g.getFontMetrics();

         setFont(scaledFont);

      }

      */

      

      g.drawImage(_image, 0, 0, this);



      g.setColor(getForeground());

      g.drawString(getText(), _textInsets.left, _textInsets.top+fm.getAscent());

   }



}