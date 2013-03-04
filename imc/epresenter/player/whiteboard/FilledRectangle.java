package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.geom.Rectangle2D;
import java.io.*;

import imc.epresenter.player.util.SGMLParser;

public class FilledRectangle extends VisualComponent {
	private Color color;
	
	FilledRectangle(String line, BufferedReader in) {
		SGMLParser parser = new SGMLParser(line);
		
      fillClip(parser);
      fillSize(parser);
		color = createColor(extractShort(parser, "fcolor"),
								  extractString(parser, "rgb"));
	}
	
	void paint(Graphics g, int cwidth, int cheight, double scale) {
		g.setColor(color);
      Rectangle2D.Double rd = new Rectangle2D.Double(
         (scale * m_rcSize.x), (scale * m_rcSize.y),
         (scale * m_rcSize.width), (scale * m_rcSize.height));
      ((Graphics2D)g).fill(rd);
      
		//Rectangle clip = getClip(scale);
		//g.fillRect(clip.x+1,clip.y+1,clip.width-2,clip.height-2);
		//System.out.println("FREC"+clip.x+","+clip.y+","+clip.width+","+clip.height);
	}

   public Color GetColor()
   {
      return color;
   }
   
   public boolean HasSameValues(FilledRectangle other)
   {
      return other.clip.equals(clip) && other.color.equals(color);
   }
}