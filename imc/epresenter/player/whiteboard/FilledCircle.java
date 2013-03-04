package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.geom.Ellipse2D;
import java.io.*;

import imc.epresenter.player.util.SGMLParser;

class FilledCircle extends VisualComponent {
	private Color color;
	
	FilledCircle(String line, BufferedReader in) {
		SGMLParser parser = new SGMLParser(line);
		
		fillClip(parser);
      fillSize(parser);
		color = createColor(extractShort(parser, "fcolor"),
								  extractString(parser, "rgb"));
	}
	
	void paint(Graphics g, int cwidth, int cheight, double scale) {
		g.setColor(color);
		Ellipse2D.Double ed = new Ellipse2D.Double(
         (scale * m_rcSize.x), (scale * m_rcSize.y),
         (scale * m_rcSize.width), (scale * m_rcSize.height));
      ((Graphics2D)g).fill(ed);
   }
}