package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Rectangle2D;
import java.io.*;

import imc.epresenter.player.util.SGMLParser;

public class OutlineRectangle extends VisualComponent {
	private float linewidth;
	private short linestyle;
   private Color color;
	private Stroke stroke;
   protected boolean m_bIsActuallyACircle = false;
   
   //private float fRemainderX = 0, fRemainderY = 0;
	
	OutlineRectangle(String line, BufferedReader in) {
		SGMLParser parser = new SGMLParser(line);
		
      fillSize(parser);
      
      color = createColor(extractShort(parser, "fcolor"),
								  extractString(parser, "rgb"));
		linewidth = extractFallbackFloat(parser, "linewidthf");
		linestyle = extractShort(parser, "linestyle");
		stroke = createStroke(linestyle, linewidth, false, false);
      
      float lw2 = linewidth/2.0f;
      
      clip.x = (int)(m_rcSize.x-lw2-1);
      clip.y = (int)(m_rcSize.y-lw2-1);
      clip.width = (int)(m_rcSize.width+linewidth+2);
      clip.height = (int)(m_rcSize.height+linewidth+2);
   }
 
   public Color GetColor()
   {
      return color;
   }
 	
	void paint(Graphics g, int cwidth, int cheight, double scale) 
   {
		Graphics2D g2 = (Graphics2D)g;
      Stroke strokeRestore = g2.getStroke();
      
		if (scale != 1.0) {
         Stroke stroke2 = createStroke(linestyle, (float)(scale*linewidth), false, false);
			g2.setStroke(stroke2);
		} else {
			g2.setStroke(stroke);
		}
		g.setColor(color);
		
      Shape shape = null;
      
      if (!m_bIsActuallyACircle)
      {
         shape = new Rectangle2D.Double(
            (scale * m_rcSize.x), (scale * m_rcSize.y),
            (scale * m_rcSize.width), (scale * m_rcSize.height));
      }
      else
      {
         shape = new Ellipse2D.Double(
            (scale * m_rcSize.x), (scale * m_rcSize.y),
            (scale * m_rcSize.width), (scale * m_rcSize.height));
      }
      
      g2.draw(shape);

      g2.setStroke(strokeRestore);
   }
}