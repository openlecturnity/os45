package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.geom.*;
import java.io.*;

import imc.epresenter.player.util.SGMLParser;

//
// Diese Klasse dient auch gleichzeitig als Polyline, der einzige Unterschied
// ist evtl. der (bei Polyline vorhandene) linestyle. -> aktuell: hm!
public class Freehand extends VisualComponent {
   private short linestyle, subtype, arrowStyle = 0;
   private float linewidth;
   private float[] xpoints;
	private float[] ypoints;
   private GeneralPath m_Polygon;
	private Color color;
	private Stroke stroke;
   private boolean isFreehand, joinsRound, isEmpty;
	
	Freehand(String line, BufferedReader in, boolean isFreehand, float[] arrowShortenings) {
		SGMLParser parser = new SGMLParser(line);
		
      this.isFreehand = isFreehand;

		linewidth = extractFallbackFloat(parser, "linewidthf");
      
		clip.x = (int)(extractShort(parser, "x")-linewidth/2-1);
      clip.y = (int)(extractShort(parser, "y")-linewidth/2);
		clip.width = (int)(extractShort(parser, "width")+linewidth+3);
		clip.height = (int)(extractShort(parser, "height")+linewidth+3);
      
		color = createColor(extractShort(parser, "fcolor"),
								  extractString(parser, "rgb"));
		linestyle = extractShort(parser, "linestyle");
      

		short sPointCount = extractShort(parser, "count");
		xpoints = new float[sPointCount > 1 ? sPointCount : 2];
		ypoints = new float[sPointCount > 1 ? sPointCount : 2];
      // have room for a possible second point
		
		String input = null;
		for(int i=0; i<sPointCount; i++) {
			try { input = in.readLine(); } catch (IOException e) {}
			SGMLParser parser2 = new SGMLParser(input);
			xpoints[i] = extractFallbackFloat(parser2, "xf");
			ypoints[i] = extractFallbackFloat(parser2, "yf");
		}
      
      joinsRound = true; // marker and FREEHAND have round joins
      
      if (sPointCount <= 0)
         isEmpty = true;
      
      if (sPointCount == 1)
      {
         xpoints[1] = xpoints[0];
         ypoints[1] = ypoints[0];
         // otherwise a marker (ends CAP_SQUARE) doesn't get painted
      }
      else if (sPointCount > 1)
      {
         boolean firstMatchesLast =
            xpoints[0] == xpoints[sPointCount-1] &&
            ypoints[0] == ypoints[sPointCount-1];
         
         if (firstMatchesLast)
         {
            // it is most likely a rectangle (or similar) from PPT
            // or a closed polygon from assistant or PPT
            joinsRound = false;
         }
         
         if (sPointCount > 2)
         {
            // BUGFIX #2094 (part of it): avoid having the line end in two times
            // the same point, which leads to wrong painting
            // and will the below algorithms about the arrow heads make
            // more difficult.
            if (xpoints[sPointCount-1] == xpoints[sPointCount-2] &&
                ypoints[sPointCount-1] == ypoints[sPointCount-2])
            {
               removeSegments(0.0, true);
               sPointCount--;
            }
         }
        
               
         // A POLYLINE may have arrow heads.
         // Currently this is done with a hack in the Assistant/ObjectQueue:
         // for every arrow head there is an additional simple LINE having
         // and displaying that arrow head.
         
         // However since 1.7.0.p6 arrow head support and placement
         // is heavily improved and for that the length of this POLYLINE
         // maybe has to be shortened on one or both ends:
         
         arrowStyle = extractShort(parser, "polyArrowStyle");
         if (arrowStyle != 0 && arrowShortenings != null)
         {
            int idxShortens = 0;
 
            if (xpoints.length > 1 && (arrowStyle == 2 || arrowStyle == 3))
            {
               // Pfeilspitze am Anfang
               
               if (idxShortens < arrowShortenings.length && arrowShortenings[idxShortens] > 0.0f)
               {
                  removeSegments(arrowShortenings[idxShortens++], false);
                  sPointCount = (short)xpoints.length;
               }
            }
            
            if (xpoints.length > 1 && (arrowStyle == 1 || arrowStyle == 3))
            {
               // Pfeilspitze am Ende
               
               if (idxShortens < arrowShortenings.length && arrowShortenings[idxShortens] > 0.0f)
               {
                  removeSegments(arrowShortenings[idxShortens++], true);
                  sPointCount = (short)xpoints.length;
               }
            }
      
            
         } // arrowStyle != 0
      
      } // sPointCount > 1
      
      if (!joinsRound)
      {
         clip.x -= 10; clip.y -= 10;
         clip.width += 20; clip.height += 20;
      }
      
      stroke = createStroke(linestyle, linewidth, isFreehand, joinsRound, arrowStyle != 0);
		
      
      m_Polygon = new GeneralPath(GeneralPath.WIND_NON_ZERO, xpoints.length);
      for (int i=0; i<xpoints.length; ++i)
      {
         if (i == 0)
            m_Polygon.moveTo(xpoints[i], ypoints[i]);
         else
            m_Polygon.lineTo(xpoints[i], ypoints[i]);
      }
      
      if (!joinsRound)
         m_Polygon.closePath();
		
		// read </FREEHAND>
		try { in.readLine(); } catch (IOException e) {}
	}
	
	void paint(Graphics g, int width, int height, double scale) {
      if (isEmpty)
         return;
      
      Graphics2D g2 = (Graphics2D)g;
      Stroke strokeRestore = g2.getStroke();
      
		if (scale != 1.0) {
         Stroke stroke2 = createStroke(linestyle, (float)(scale*linewidth), isFreehand, joinsRound, arrowStyle != 0);
			g2.setStroke(stroke2);
		} else {
			g2.setStroke(stroke);
		}
		g.setColor(color);
         
      paintGeneralPath(g, scale, m_Polygon, false);
      
      /*
      // old style; simply draw a Polyline or Polygon
      if (scale != 1.0) {
         Polygon p = new Polygon(xpoints, ypoints, xpoints.length);
         p = scalePolygon(p, scale);
            
         if (joinsRound)
            g.drawPolyline(p.xpoints, p.ypoints, nCount);
         else
            g.drawPolygon(p.xpoints, p.ypoints, nCount);
      } else {
         if (joinsRound)
            g.drawPolyline(xpoints, ypoints, nCount);
         else
            g.drawPolygon(xpoints, ypoints, nCount);
      }
      */
      
      g2.setStroke(strokeRestore);
	}
   
   /**
    * @param dShortenLength specifies the length of how many 
    *        segments to remove; if the length is 0.0 only one segment is
    *        removed.
    */
   private void removeSegments(double dShortenLength, boolean bAtTheEnd)
   {
      if (xpoints != null && xpoints.length > 0)
      {
         if (xpoints.length > 2)
         {
            int nRemoveCount = 0;
            if (0.0 == dShortenLength) 
            {
               // remove just one element
               nRemoveCount = 1;
            }
            else
            {
               // remove as many elements as necessary to 
               // (nearly) reach the given shortening length
               
               double dRemovedLength = 0.0;
               int nStartIdx = xpoints.length-1;
               int iLoopChange = -1;
               if (!bAtTheEnd)
               {
                 nStartIdx = 1;
                 iLoopChange = 1;
               }
               
               for (int i=nStartIdx; i>0 && i<xpoints.length && dRemovedLength < dShortenLength; i += iLoopChange)
               {
                  double dX = xpoints[i] - xpoints[i-1];
                  double dY = ypoints[i] - ypoints[i-1];
                  double dSegmentLength = Math.sqrt(dX*dX+dY*dY);
                     
                  if (dRemovedLength + dSegmentLength < dShortenLength)
                  {
                     nRemoveCount++;
                     dRemovedLength += dSegmentLength;
                  }
                  else
                     break;
               }
            }
            
            int nNewLength = xpoints.length-nRemoveCount;
            if (nNewLength > 1)
            {
               float[] xpointsNew = new float[nNewLength];
               float[] ypointsNew = new float[nNewLength];
               int nStart = bAtTheEnd ? 0 : nRemoveCount;
               int nLength = xpointsNew.length;
            
               System.arraycopy(xpoints, nStart, xpointsNew, 0, nLength);
               xpoints = xpointsNew;
               System.arraycopy(ypoints, nStart, ypointsNew, 0, nLength);
               ypoints = ypointsNew;
               
               //System.out.println("Removed "+nRemoveCount+(bAtTheEnd ? " at the end." : " at the start."));
            }
            else
            {
               xpoints = new float[0];
               isEmpty = true;
            }
         }
         else
         {
            xpoints = new float[0];
            isEmpty = true;
         }
      }
   }
}