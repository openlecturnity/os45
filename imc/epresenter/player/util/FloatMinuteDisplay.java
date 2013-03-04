package imc.epresenter.player.util;


import java.awt.*;
import java.awt.font.*;
import java.awt.geom.Rectangle2D;
import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.BevelBorder;

import imc.epresenter.player.Manager;

public class FloatMinuteDisplay extends JPanel
{
   private int millis_;
   private Font displayFont_;
   private Rectangle2D[] labelBounds_;
   

	public FloatMinuteDisplay(int millis)
	{
      millis_ = millis;

      setForeground(Color.white);
      setOpaque(false);
		
      displayFont_ = getFont();
	}

	protected void paintComponent(Graphics g)
	{
      int width = getWidth();
      int height = getHeight();
      
      //super.paintComponent(g); // background color
		Graphics2D g2D = (Graphics2D)g;
      
      /*
      g2D.setRenderingHints(new RenderingHints(
			RenderingHints.KEY_TEXT_ANTIALIASING,
			RenderingHints.VALUE_TEXT_ANTIALIAS_ON));
         */

      /*
      g.setColor(Color.blue);
      Rectangle r = new Rectangle(0,0,getSize().width, getSize().height);//g.getClip().getBounds();
      r.width -= 1; r.height -= 1;
      ((Graphics2D)g).draw(r);
      //*/


      g.setFont(displayFont_);

      FontMetrics fm = g.getFontMetrics();
      if (fm.getAscent() > height)
      {
         displayFont_ = displayFont_.deriveFont(height/(float)fm.getAscent());
         g.setFont(displayFont_);
      }
		
      
      FontRenderContext frc = g2D.getFontRenderContext();

		Rectangle2D maxLabelSize = displayFont_.createGlyphVector(frc, "888").getVisualBounds();

		int numLabels = (int)(width / (maxLabelSize.getWidth()*2.2));
		// 2.2 = at least 1 label-widths spacing between 2 labels
		int millisPerLabel = 0; // dummy for later code

		if (numLabels > 0)
		{
			millisPerLabel = millis_ / numLabels;
			if (millisPerLabel > 60*60*1000) millisPerLabel = 2*60*60*1000;
			else if (millisPerLabel > 30*60*1000) millisPerLabel = 60*60*1000;
			else if (millisPerLabel > 20*60*1000) millisPerLabel = 30*60*1000;
			else if (millisPerLabel > 15*60*1000) millisPerLabel = 20*60*1000;
			else if (millisPerLabel > 10*60*1000) millisPerLabel = 15*60*1000;
			else if (millisPerLabel > 5*60*1000) millisPerLabel = 10*60*1000;
			else if (millisPerLabel > 2*60*1000) millisPerLabel = 5*60*1000;
			else if (millisPerLabel > 60*1000) millisPerLabel = 2*60*1000;
			else if (millisPerLabel > 30*1000) millisPerLabel = 60*1000;
         else if (millisPerLabel > 20*1000) millisPerLabel = 30*1000;
         else if (millisPerLabel > 10*1000) millisPerLabel = 20*1000;
         else if (millisPerLabel > 5*1000) millisPerLabel = 10*1000;
         else millisPerLabel = 5*1000;

         if (millis_ > 1000*60*4 && millisPerLabel < 60*1000)
            millisPerLabel = 60*1000; // no "seconds" for long documents

			numLabels = millis_ / millisPerLabel;
		}

		if (labelBounds_ == null || numLabels != labelBounds_.length)
		{
			labelBounds_ = new Rectangle2D[numLabels];
		
         // double code (below)
			for (int i=0; i<numLabels; i++)
			{
				int labelMillis = i*millisPerLabel;
				String labelText = null;
            if (millisPerLabel >= 60*1000)
               labelText = ((labelMillis)/(60*1000))+"";
            else
               labelText = (labelMillis/1000)+"s";

				labelBounds_[i] = displayFont_.createGlyphVector(frc, labelText).getVisualBounds();
			}
		}

      // double code (above)
		for (int i=0; i<numLabels; i++)
		{
			Rectangle2D labelSize = labelBounds_[i];
			int labelMillis = (i+1)*millisPerLabel;
			String labelText = null;
         if (millisPerLabel >= 60*1000)
            labelText = ((labelMillis)/(60*1000))+"";
         else
            labelText = (labelMillis/1000)+"s";
         int position = (int)((labelMillis/(float)millis_)*width);
			float x = position-(float)labelSize.getWidth()/2;
			float y = (height-(float)labelSize.getHeight())/2+(float)labelSize.getHeight()-1;
         if (x+labelSize.getWidth()+1 < width)
         {
            g.setColor(Color.black);
            g2D.drawString(labelText, x+1, y+1);
            g.setColor(getForeground());
            g2D.drawString(labelText, x, y);
         }
      }

      if (millisPerLabel > 0)
      {
         int strokeMillis = (int)(0.5*millisPerLabel);
         while(strokeMillis < millis_)
         {
            int x = (int)((strokeMillis/(float)millis_)*width)-2;
            if (x+14 < width)
            {
               g.setColor(Color.black);
               g.drawLine(x+1,height/2+1,x+5,height/2+1);
               g.setColor(getForeground());
               g.drawLine(x,height/2,x+4,height/2);
            }
            strokeMillis += millisPerLabel;
         }
      }
		

	}


}