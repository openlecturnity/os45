package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.geom.AffineTransform;
import java.awt.geom.GeneralPath;
import java.io.*;

import imc.epresenter.player.util.SGMLParser;

class FilledPolygon extends VisualComponent {
	private float linewidth;
	private Color color;
	private GeneralPath m_Polygon;
	
	FilledPolygon(String line, BufferedReader in) {
		SGMLParser parser = new SGMLParser(line);
		
      fillClip(parser);
		color = createColor(extractShort(parser, "fcolor"),
								  extractString(parser, "rgb"));
		linewidth = extractFallbackFloat(parser, "linewidthf");
		
		short count = extractShort(parser, "count");

      m_Polygon = new GeneralPath(GeneralPath.WIND_NON_ZERO, count);
		
		String input = null;
		for(int i=0; i<count; i++) {
			try { input = in.readLine(); } catch (IOException e) {}
			SGMLParser parser2 = new SGMLParser(input);
			float x = extractFallbackFloat(parser2, "xf");
			float y = extractFallbackFloat(parser2, "yf");
         
         if (i == 0)
            m_Polygon.moveTo(x, y);
         else
            m_Polygon.lineTo(x, y);
		}
      m_Polygon.closePath();
			
		// read </FILLEDPOLY>
		try { in.readLine(); } catch (IOException e) {}
	}
	
	void paint(Graphics g, int cwidth, int cheight, double scale) 
   {
      g.setColor(color);
      
      paintGeneralPath(g, scale, m_Polygon, true);
	}
}
