package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.geom.Line2D;
import java.awt.geom.GeneralPath;
import java.awt.geom.Point2D;
import java.io.*;

import imc.epresenter.player.util.SGMLParser;

public class Line extends VisualComponent
{
   private float startX,startY,endX,endY;
   private float linewidth;
	private short linestyle, arrowConfig = 0;
   private float polyLineShortening = 0.0f;
   private Stroke stroke;
	private GeneralPath arrowShape1, arrowShape2;
	private Color color;
   private boolean bLineEmpty;
	
	Line(String line, BufferedReader in)
   {
		SGMLParser parser = new SGMLParser(line);
		
      fillSize(parser);
      
      bLineEmpty = false;
		
		boolean ori = extractShort(parser, "ori") != 0;
		boolean direction = extractShort(parser, "direction") != 0;
		
      if(ori)
      {
         if(!direction)
         {
            startX = m_rcSize.x;
            startY = m_rcSize.y;
            endX = m_rcSize.x + m_rcSize.width;
            endY = m_rcSize.y + m_rcSize.height;
         } 
         else
         {
            startX = m_rcSize.x + m_rcSize.width;
            startY = m_rcSize.y + m_rcSize.height;
            endX = m_rcSize.x;
            endY = m_rcSize.y;
         }
      }
      else
      { 
         if(!direction)
         {
            startX = m_rcSize.x;
            startY = m_rcSize.y + m_rcSize.height;
            endX = m_rcSize.x + m_rcSize.width;
            endY = m_rcSize.y;
         }
         else
         {
            startX = m_rcSize.x + m_rcSize.width;
            startY = m_rcSize.y; 
            endX = m_rcSize.x;
            endY = m_rcSize.y + m_rcSize.height; 
         }
      }
		
      int minX = (int)Math.min(startX, endX);
      int minY = (int)Math.min(startY, endY);
      int maxX = (int)Math.max(startX, endX)+1;
      int maxY = (int)Math.max(startY, endY)+1;
      
      linewidth = extractFallbackFloat(parser, "linewidthf");

      clip.x = (int)(minX-linewidth);
      clip.y = (int)(minY-linewidth);
      clip.width = (int)(maxX-minX+2*linewidth);
      clip.height = (int)(maxY-minY+2*linewidth);

		int fcolor = extractShort(parser, "fcolor");
		color = createColor(fcolor, extractString(parser, "rgb"));
		
		short arrowStyle = extractShort(parser, "arrowstyle");
		if (arrowStyle != 0)
      {
         arrowConfig = extractShort(parser, "arrowconfig");
         if (0 == arrowConfig) // not specified; use standard values
            arrowConfig = 1515;
        
         int arrowSize1 = arrowConfig % 10;
         int arrowType1 = (arrowConfig / 10) % 10;
         int arrowSize2 = (arrowConfig / 100) % 10;
         int arrowType2 = (arrowConfig / 1000);
       
         
         float dX = endX-startX; // Richtungsvektor
		   float dY = endY-startY;
         
         double dLength = Math.sqrt(dX*dX+dY*dY);
         double dShortenF = 1/dLength;
         double dXn = dShortenF * dX; // normalisierter Richtungsvektor
         double dYn = dShortenF * dY;

           
         // now create the arrows
			Point2D.Double ptBase2 = new Point2D.Double();
         if (arrowStyle == 1 || arrowStyle == 3)
         {
            // Spitze am Ende der Linie
				arrowShape1 = createArrowShape(endX, endY, dX, dY, 
                                           linewidth, arrowType1, arrowSize1, 1.0, ptBase2);
            clip.add(arrowShape1.getBounds2D());
            
            // Linie am Ende verkürzen für die Pfeilspitze
            if (4 > arrowType1) // 1, 2, 3
            {
               endX = (float)ptBase2.x;
               endY = (float)ptBase2.y;
            }
         }
			if (arrowStyle == 2 || arrowStyle == 3)
         {
				// Spitze am Anfang der Linie
				arrowShape2 = createArrowShape(startX, startY, -dX, -dY, 
                                           linewidth, arrowType2, arrowSize2, 1.0, ptBase2);
				clip.add(arrowShape2.getBounds2D());
			
            // Linie am Anfang verkürzen für die Pfeilspitze
            if (4 > arrowType2) // 1, 2, 3
            {
               startX = (float)ptBase2.x;
               startY = (float)ptBase2.y;
            }
         }
         
            
         // Maybe the position of the start OR the end point was
         // corrected due to the presence of an arrow head.
         // For very short lines (i.e. for POLYLINES with arrow heads) 
         // this algorithm may result in significantly
         // extending the line which is undesired.
         // In that case do not paint the line itself at all.
         float dXc = endX-startX, dYc = endY-startY;
         double dNewLength = Math.sqrt(dXc*dXc+dYc*dYc);
         if (dNewLength > dLength)
         {
            bLineEmpty = true;
            
            // It could be that we are the arrow head of a polyline.
            // AND if this line is very short that polyline also has to
            // be shortened in order for a proper display.
            //
            // Note: This method is not entirely precise
            // as it assumes that if the line is short it is
            // stretched over its original length in the _opposite_ direction
            // by the above algorithm. But this of course has not to be 
            // always true.
            polyLineShortening = (float)dNewLength;
         }
         else
         {
            polyLineShortening = 0;
         }
            
		}
		
		linestyle = extractShort(parser, "linestyle");
      
		stroke = createStroke(linestyle, linewidth, false, false, arrowConfig != 0);
	}
	
	void paint(Graphics g, int cwidth, int cheight, double scale)
   {
		Graphics2D g2 = (Graphics2D)g;
      Stroke strokeRestore = g2.getStroke();      
      
		if (scale != 1.0)
      {
         Stroke stroke2 = createStroke(linestyle, (float)(scale*linewidth), false, false, arrowConfig != 0);
			g2.setStroke(stroke2);
		}
      else
      {
			g2.setStroke(stroke);
		}
		g.setColor(color);
      
      
      Line2D.Float line = new Line2D.Float((float)(scale*startX), (float)(scale*startY),
                                           (float)(scale*endX),(float)(scale*endY));
         
      if (!bLineEmpty) 
         g2.draw(line);
      // else maybe it was shortened so much (arrow heads)
      // that it shouldn't be painted at all
      
                                        
		if (scale != 1.0)
      {
         if (arrowShape1 != null) 
            g2.fill(scaleGeneralPath(arrowShape1, scale));
         if (arrowShape2 != null) 
            g2.fill(scaleGeneralPath(arrowShape2, scale));
      }
      else
      {
			if (arrowShape1 != null) g2.fill(arrowShape1);
         if (arrowShape2 != null) g2.fill(arrowShape2);
      }

      g2.setStroke(strokeRestore);
	}
   
   /**
    * @return
    */
   float getArrowShorteningForPoly()
   {
      return polyLineShortening;
   }
}
