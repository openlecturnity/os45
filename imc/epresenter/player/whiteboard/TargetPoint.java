package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.geom.Rectangle2D;
import java.io.BufferedReader;
import java.io.IOException;

import imc.epresenter.player.util.SGMLParser;


public class TargetPoint extends VisualComponent
{
   private VisualComponent m_LastSnappedComponent = null;
   private Point m_ptCenter = null;
   private short m_sBelongsKey = -1;
   
   public TargetPoint(String strLine, BufferedReader reader)
   {
      SGMLParser parser = new SGMLParser(strLine);
      
      fillClip(parser);
      fillSize(parser);
      
      m_sBelongsKey = extractShort(parser, "belongs");

      m_ptCenter = new Point(clip.x + (clip.width - 1) / 2, clip.y + (clip.height - 1) / 2);
      
      // has an activity range
      super.ParseActivityVisibility(parser, true);
   }
     
   public short GetBelongsKey()
   {
      return m_sBelongsKey;
   }
   
   public boolean IsAttractive(Rectangle rcSource, VisualComponent comp)
   {
      if (m_LastSnappedComponent == null || m_LastSnappedComponent == comp)
      {
         // center of moved object inside of the boundaries
         
         int iSourceMidX = rcSource.x + rcSource.width/2;
         int iSourceMidY = rcSource.y + rcSource.height/2;
         
         boolean bHorizontalMatch = 
            iSourceMidX >= m_rcSize.x && iSourceMidX <= m_rcSize.x + m_rcSize.width;
         boolean bVerticalMatch = 
            iSourceMidY >= m_rcSize.y && iSourceMidY <= m_rcSize.y + m_rcSize.height;

         if (bHorizontalMatch && bVerticalMatch)
            return true;
         else
            return false;
      }
      else
         return false; // no second snap-in
   }
   
   public void SnapIn(VisualComponent vc, Rectangle rcSource)
   {
      int iSourceX = rcSource.x + rcSource.width/2;
      int iSourceY = rcSource.y + rcSource.height/2;

      rcSource.x -= (iSourceX-m_ptCenter.x);
      rcSource.y -= (iSourceY-m_ptCenter.y);
      
      m_LastSnappedComponent = vc;
   }
   
   public void SnapOut(VisualComponent vc)
   {
      if (m_LastSnappedComponent == vc || vc == null)
      {
         m_LastSnappedComponent = null;
      }
   }
   
   public boolean CheckAnswer()
   {
      // TODO maybe (also) check distance here (and not in ObjectQueue).
      
      if (m_sBelongsKey >= 0xfff) // no source image specified
         return true;
      
      if (m_LastSnappedComponent != null && m_LastSnappedComponent instanceof Picture)
         return ((Picture)m_LastSnappedComponent).GetBelongsKey() == m_sBelongsKey;
      else
         return false; // if undefined nothing matches
   }
   
   
   // interface for Publisher
   public Point GetCenterPoint()
   {
      return m_ptCenter;
   }
   
   public Rectangle GetArea()
   {
      return clip;
   }

   void paint(Graphics g, int width, int height, double scale)
   {
      Graphics2D g2 = (Graphics2D)g;
      Stroke strokeRestore = g2.getStroke();
      
		Stroke stroke2 = createStroke((short)0, (float)(scale*1), false, false);
	   g2.setStroke(stroke2);
		g.setColor(Color.black);
		
      Rectangle2D.Float rf = new Rectangle2D.Float(
         (float)(scale*m_rcSize.x), (float)(scale*m_rcSize.y),
         (float)(scale*(m_rcSize.width-1)), (float)(scale*(m_rcSize.height-1)));
      // "-1": otherwise the rectangle is one pixel too wide/high
      // in comparison to the image
    
      g2.draw(rf);
     
      g2.setStroke(strokeRestore);
   }
}