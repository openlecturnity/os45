package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.font.TextAttribute;
import java.awt.geom.AffineTransform;
import java.awt.geom.GeneralPath;
import java.awt.geom.PathIterator;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.io.*;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.StringTokenizer;
import sun.java2d.SunGraphicsEnvironment; // my, my

import imc.epresenter.player.Manager;
import imc.epresenter.player.util.SGMLParser;
import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.Question;
import imc.lecturnity.util.FileUtils;

public class VisualComponent {
	private static HashMap fontMap = new HashMap();
	private static HashMap colorMap;

   private static HashMap fontFailure = new HashMap();

	private static Color oldColors[] = {
		// standard
		Color.black, Color.red, Color.green, Color.yellow, 
		Color.blue, Color.magenta, Color.cyan, Color.white,
		// grey
		new Color(214,214,214), new Color(191,191,191), new Color(124,124,124),
		new Color(103,103,103), new Color(85,85,85), new Color(51,51,51),
		new Color(28,28,28),
		// red
		new Color(255,204,204), new Color(255,154,154), new Color(255,103,103),
		new Color(255,51,51), new Color(204,0,0), new Color(154,0,0),
		new Color(103,0,0), new Color(51,0,0),
		// green
		new Color(204,255,204),new Color(154,255,154), new Color(103,255,103),
		new Color(51,255,51), new Color(0,204,0), new Color(0,154,0),
		new Color(0,103,0), new Color(0,51,0),
		// yellow
		new Color(255,255,204), new Color(255,255,154), new Color(255,255,103),
		new Color(255,255,51), new Color(204,204,0), new Color(154,154,0),
		new Color(103,103,0), new Color(51,51,0),
		// blue
		new Color(204,204,255), new Color(154,154,255), new Color(103,103,255),
		new Color(51,51,255), new Color(0,0,204), new Color(0,0,154),
		new Color(0,0,103),new Color(0,0,51),
		// pink
		new Color(255,204,255), new Color(255,154,255), new Color(255,103,255),
		new Color(255,51,255), new Color(204,0,204), new Color(154,0,154),
		new Color(103,0,103),new Color(51,0,51),
		// turquoise
		new Color(204,255,255), new Color(154,255,255), new Color(103,255,255),
		new Color(51,255,255), new Color(0,204,204), new Color(0,154,154),
		new Color(0,103,103),new Color(0,51,51)
	};
  		
	protected Rectangle clip = new Rectangle();
   protected Rectangle2D.Float m_rcSize = new Rectangle2D.Float();
   
   private boolean m_bIsVisible = false;
   protected int[] m_aiVisibilityTimes = null;
   protected int[] m_aiActivityTimes = null;
   protected boolean m_bMoveable = false;
   private boolean m_bIsMovingDisabled = false;
   
	
	VisualComponent() { }
	
	void paint(Graphics g, int width, int height, double scale) { }
	
	public Rectangle getClip(double dScale)
   {
      return getClip(dScale, true);
   }
      
	public Rectangle getClip(double dScale, boolean bWithBorder) 
   {
      int iMinus = bWithBorder ? 2 : 0;
      int iPlus = bWithBorder ? 5 : 0;
      // borders for the propper anti aliased painting (and deleting)
      
		if (dScale != 1.0)
			return new Rectangle(
				(int)(dScale*clip.x)-iMinus,(int)(dScale*clip.y)-iMinus,
				(int)(dScale*clip.width)+iPlus,(int)(dScale*clip.height)+iPlus);
		else
			return new Rectangle(clip.x-iMinus,clip.y-iMinus,clip.width+iPlus,clip.height+iPlus);
   }
   
   /**
    * Currently only used for moveable Pictures (which are not in the EventQueue).
    *
    * @returns true if the state was (had to be) changed.
    */
   public boolean SetVisible(boolean bYes)
   {
      if (m_bIsVisible != bYes)
      {
         m_bIsVisible = bYes;
         return true;
      }
      else
         return false;
   }
   
   protected boolean ParseActivityVisibility(SGMLParser parser)
   {
      return ParseActivityVisibility(parser, false);
   }
   
   /**
    * @returns false upon a parsing error. true in all other cases:
    *          especially when there was nothing to parse.
    */
   protected boolean ParseActivityVisibility(SGMLParser parser, boolean bSetActivity)
   {
      boolean bTotalSuccess = true;
      
      // an object for storing two time values
      Point ptTimes = new Point(0, 0);
      
      String strVisibility = parser.getValue("visible");
      if (strVisibility != null)
      {
         boolean bSuccess = Question.ExtractTimes(strVisibility, ptTimes);
         
         if (bSuccess)
         {
            m_aiVisibilityTimes = new int[2];
            
            m_aiVisibilityTimes[0] = ptTimes.x;
            m_aiVisibilityTimes[1] = ptTimes.y;
         }
         else
            bTotalSuccess = false;
      }
      
      // read and parse (timely) activitiy regions
      String strActiveRegions = parser.getValue("active");
      if (strActiveRegions != null)
      {
         ArrayList alRegionCollector = new ArrayList(4);
         
         StringTokenizer tokenizer = new StringTokenizer(strActiveRegions, ";");
         
         while (tokenizer.hasMoreTokens())
         {
            String strToken = tokenizer.nextToken();
            
            boolean bSuccess = Question.ExtractTimes(strToken, ptTimes);
            
            if (bSuccess)
            {
               alRegionCollector.add(new Integer(ptTimes.x)); // start time
               alRegionCollector.add(new Integer(ptTimes.y)); // end time
            }
            else
               bTotalSuccess = false;
         }
         
         if (alRegionCollector.size() > 0)
         {
            m_aiActivityTimes = new int[alRegionCollector.size()];
            
            for (int i=0; i<m_aiActivityTimes.length; ++i)
               m_aiActivityTimes[i] = ((Integer)alRegionCollector.get(i)).intValue();
         }
      }
      
      if (bTotalSuccess && bSetActivity)
      {
         if (m_aiActivityTimes == null && m_aiVisibilityTimes != null)
            m_aiActivityTimes = m_aiVisibilityTimes;
         // activity is normally unspecified but it should match the visibility (then)
      
      }
      
      return bTotalSuccess;
   }

  
   public int[] GetVisibilityTimes()
   {
      return m_aiVisibilityTimes;
   }
   
   public int[] GetActivityTimes()
   {
      return m_aiActivityTimes;
   }
   
   public boolean EqualsActivity(VisualComponent other)
   {
      if (m_aiActivityTimes == null || other.m_aiActivityTimes == null)
         return false;
      
      if (m_aiActivityTimes.length != other.m_aiActivityTimes.length)
         return false;
      
      for (int i=0; i<m_aiActivityTimes.length; ++i)
      {
         if (m_aiActivityTimes[i] != other.m_aiActivityTimes[i])
            return false;
      }
      
      return true;
   }
   
   boolean IsActiveAt(int iTime)
   {
      if (m_aiActivityTimes != null)
      {
         boolean bFound = false;
         for (int i=0; i<m_aiActivityTimes.length; i += 2)
         {
            if (iTime >= m_aiActivityTimes[i] && iTime <= m_aiActivityTimes[i+1])
            {
               bFound = true;
               break;
            }
         }
         return bFound;
      }
      else
      {
         // no activity definition means always active
         return true;
      }
   }
   
   void PrintActivity()
   {
      if (m_aiActivityTimes != null)
      {
         for (int i=0; i<m_aiActivityTimes.length; ++i)
            System.out.print(m_aiActivityTimes[i]+" ");
         System.out.println();
      }
      else
         System.out.println(m_aiActivityTimes);
   }
   
   boolean IsVisibleAt(int iTime)
   {
      if (m_aiVisibilityTimes != null)
      {
         boolean bFound = false;
         for (int i=0; i<m_aiVisibilityTimes.length; i += 2)
         {
            if (iTime >= m_aiVisibilityTimes[i] && iTime <= m_aiVisibilityTimes[i+1])
            {
               bFound = true;
               break;
            }
         }
         
         return bFound;
      }
      else
      {
         // no visibility definition means never visible
         return false;
      }
   }
   
   
   public boolean IsMoveable()
   {
      return m_bMoveable;
   }
   
      
   public boolean DisableMoving()
   {
      boolean bChange = !m_bIsMovingDisabled;
      m_bIsMovingDisabled = true;
      return bChange;
   }
   
   public boolean IsMovingDisabled()
   {
      return m_bIsMovingDisabled;
   }
	
 
   public void MovePosition(float fDiffX, float fDiffY) { }
   
   public void ResetPosition() { }

   public boolean HitTest(int x, int y)
   {
      // only implemented for certain components: InteractionArea, Picture
      return false;
   }
   
	protected String extractString(SGMLParser p, String key) {
		String s = p.getValue(key);
		return s != null ? s : "";
	}
   
	protected int extractInteger(SGMLParser p, String key) {
		String s = p.getValue(key);
		return s != null ? Integer.parseInt(s) : 0;
	}
   
	protected short extractShort(SGMLParser p, String key) {
		String s = p.getValue(key);
		return s != null ? Short.parseShort(s) : (short)0;
	}
   
	protected float extractFloat(SGMLParser p, String key) {
		String s = p.getValue(key);
		if (s != null && s.indexOf(',') != -1) s = s.replace(',', '.');
		return s != null ? Float.parseFloat(s) : 0;
	}
   
   protected float extractFallbackFloat(SGMLParser p, String key)
   {
      String s = p.getValue(key);
      if (s == null && key.length() > 1 && key.endsWith("f"))
      {
         key = key.substring(0, key.length()-1);
         s = p.getValue(key); // cut off the trailing f; use old definition
      }
      if (s != null && s.indexOf(',') != -1) s = s.replace(',', '.');
      return s != null ? Float.parseFloat(s) : 0;
   }
   
   protected void fillSize(SGMLParser p)
   {
      // uses old values x, y, width, height if the new ones are not present
      
      m_rcSize.x = extractFallbackFloat(p, "xf");
      m_rcSize.y = extractFallbackFloat(p, "yf");
      m_rcSize.width = extractFallbackFloat(p, "widthf");
      m_rcSize.height = extractFallbackFloat(p, "heightf");
   }
   
   protected void fillClip(SGMLParser p)
   {
      clip.x = extractShort(p, "x");
      clip.y = extractShort(p, "y");
      clip.width = extractShort(p, "width");
      clip.height = extractShort(p, "height");
   }
   
   protected String extractPath(SGMLParser p, String key, String strInputCodepage)
   {
   	String strValue = p.getValue(key);
		
      if (strValue != null)
      {
         if (FileUtils.isCodepageMismatch(strValue))
         {
            // Bug #1933
            // two wrongs make a right:
            // the filename is probably encoded in the document (and system) codepage
            // but as "file.encoding" is differing from the system codepage
            // java needs a wrong filename
            strValue = FileUtils.makeBuggedFilename(strValue, strInputCodepage);
         }
         
         return strValue;
      }
      else
         return "";
	}
	
   public static Color createBackroundColor(int fcolor)
   {
      return new VisualComponent().createColor(fcolor, null);
   }

	protected Color createColor(int fcolor, String rgb)
	{
		
		if (rgb != null && rgb.length() >= 8) // 0xff00ff
		{
			if (colorMap == null)
				colorMap = new HashMap();

         Integer key = null;
         if (rgb.length() == 10 && rgb.charAt(2) > '7')
         {
            // a value with a high transparency value
            // which makes Integer.decode(rgb) fail (NumberFormatException)
            
            int value = Integer.parseInt(rgb.substring(4), 16);
            value |= (Integer.parseInt(rgb.substring(2,4), 16) << 24);
            
            key = new Integer(value);
         }
         else
            key = Integer.decode(rgb);
         
         /*
         if (rgb.length() == 8)
         {
            // Does not work (see special case above) and is bad performance code anyway.
            rgb = "0xff" + rgb.substring(2, 8);
            key = Integer.decode(rgb);
         }
         */
         
         
         // Error here: 0x00000000 and 0x000000 (which corresponds to 0xff000000)
         // are the same value!
         
         if (!colorMap.containsKey(key))
         {
            int color = key.intValue();
            if (rgb.length() < 10)
               colorMap.put(key, new Color(key.intValue()));
            else
               colorMap.put(key, new Color(key.intValue(), true));
         }
					
         return (Color)colorMap.get(key);
		}

		return oldColors[fcolor];
	}
   
   protected Stroke createStroke(short linestyle, float linewidth, boolean endsRound, boolean joinsRound) {
      return createStroke(linestyle, linewidth, endsRound, joinsRound, false);
   }
   
	protected Stroke createStroke(short linestyle, float linewidth, boolean endsRound, boolean joinsRound, boolean lineWithArrow) {
		Stroke stroke = null;
      
      if (linewidth < 1.0f)
         linewidth = 1.0f;
      
      int endStyle = endsRound ? BasicStroke.CAP_ROUND : BasicStroke.CAP_SQUARE;
      if (lineWithArrow)
         endStyle = BasicStroke.CAP_BUTT; // otherwise arrow head placement is wrong (too)
      int joinStyle = joinsRound ? BasicStroke.JOIN_ROUND : BasicStroke.JOIN_MITER;
      
      if (linestyle != 0) {
         // dashing: an array of line and pause lengths;
         // note that there is always the additional linewidth/2 prolonging
         // on each segments end
         
			float dashing[] = new float[2];
         
         if (linestyle != 1) // not dotted
         {
            // default and old case (prior to Bug #3314)
            
            dashing[0] = linestyle+4+linewidth; // line
            dashing[1] = linestyle+1+linewidth*2; // pause
         }
         else
         {
            float fPauseLength = linewidth;
            dashing[0] = 0;
            dashing[1] = linewidth*2;
         }
         
			float dashstart = (float)(dashing[0]*0.2);
			stroke = new BasicStroke(linewidth, endStyle, joinStyle, 10.0f, dashing, dashstart); 
		} else {
			stroke = new BasicStroke(linewidth, endStyle, joinStyle, 10.0f);
		}
      
		return stroke;
	}
	
	protected Polygon scalePolygon(Polygon p, double scale) {
		if (scale != 1.0) {
			int[] xP = new int[p.npoints];
			int[] yP = new int[p.npoints];
			for (int i=0; i<p.npoints; i++) {
				xP[i] = (int)(scale*p.xpoints[i]);
				yP[i] = (int)(scale*p.ypoints[i]);
			}
			return new Polygon(xP, yP, p.npoints);
		} else {
			return p;
		}
	}
   
   // used be Line
   protected GeneralPath scaleGeneralPath(GeneralPath gp, double scale)
   {
      return modifyGeneralPath(gp, scale, 0.0, 0.0);
   }
   
   // used by Line
   protected GeneralPath translateGeneralPath(GeneralPath gp, double dX, double dY)
   {
      return modifyGeneralPath(gp, 1.0, dX, dY);
   }
   
   /**
    * If both (scaling and translation) are specified it will first scale and 
    * then translate.
    */
   protected GeneralPath modifyGeneralPath(GeneralPath gp, double scale, double dX, double dY)
   {
      if (scale != 1.0 || dX != 0 || dY != 0)
      {
         GeneralPath gpNew = new GeneralPath(gp);
         if (scale != 1.0)
            gpNew.transform(AffineTransform.getScaleInstance(scale, scale));
         if (dX != 0 || dY != 0)
            gpNew.transform(AffineTransform.getTranslateInstance(dX, dY));
         
         return gpNew;
        
      }
      else
      {
         return gp;
      }    
   }
   
   // used by FilledPolygon and Freehand
   protected void paintGeneralPath(Graphics g, double dScale, GeneralPath gp, boolean bFill)
   {
      Graphics2D g2 = (Graphics2D)g;
   
      AffineTransform at = g2.getTransform();
      if (dScale != 1.0)
         g2.scale(dScale, dScale);
      
      if (bFill)
         g2.fill(gp);
      else
         g2.draw(gp);
      
      if (dScale != 1.0)
         g2.setTransform(at); // reset old non-scaled transform
   }
   
   protected GeneralPath createArrowShape(float baseX, float baseY, float dX, float dY, 
                                          float linewidth, int type, int size, double scale,
                                          Point2D.Double ptBase2)
   {
      // default and minimum values
      if (type < 1 || type > 5)
         type = 1;
      if (size < 1 || size > 9)
         size = 5;
      if (linewidth <= 2)
      {
         if (type != 2)
            linewidth = 2;
         else
            linewidth = 1;
      }
      
         
      double dShortenF = 1/Math.sqrt(dX*dX+dY*dY);
      
      double dXn = dShortenF * dX;
      double dYn = dShortenF * dY;
      
      // (dXn, dYn) sind jetzt der normalisierte Richtungsvektor
      // der Richtungsvektor zeigt von der Linienspitze weg
      // also in die "Richtung" in der sich die anderen Punkte der Pfeilspitze
      // befinden
      
      // adapt length and width according to the desired size (and type)
      double dLength = 3.0;
      if (1 == (size % 3)) // 1, 4, 7
         dLength = 2.0;
      else if (0 == (size % 3)) // 3, 6, 9
         dLength = 5.0;
      
      double dWidth  = 1.5;
      if (4 > size) // 1, 2, 3
         dWidth = 1.0;
      else if (6 < size) // 7, 8, 9
         dWidth = 2.5;
      
      if (2 == type)
      {
         dLength *= 5.0/3.0;
         dWidth *= 5.0/3.0;
      }
       
      
      // now determine some points and create a polygon from them
      GeneralPath path = new GeneralPath();
      if (4 > type) // type 1, 2, 3
      {
         // type 1 is the easiest: just paint a triangle with 
         // the three points base, left and right
         // type 2 and 3 use additional points and maybe some curving
         
         double dLeftX = baseX - dLength*linewidth*dXn + dWidth*linewidth*-dYn;
         double dLeftY = baseY - dLength*linewidth*dYn + dWidth*linewidth*+dXn;
      
         double dRightX = baseX - dLength*linewidth*dXn + dWidth*linewidth*+dYn;
         double dRightY = baseY - dLength*linewidth*dYn + dWidth*linewidth*-dXn;
      
         // type 2 and 3 need an additinal point (middle) on the vector behind the end
         double dSubtract = 1.0;
         if (1 == (size % 3)) // 1, 4, 7
            dSubtract = 3.0/5.0;
         else if (0 == (size % 3)) // 3, 6, 9
            dSubtract = 5.0/3.0;
         double dMiddleDistance = (dLength-dSubtract)*linewidth; // for type 3
         if (2 == type)
         {
            double dXh = dLeftX - baseX, dYh = dLeftY - baseY;
            double dOuterLength = Math.sqrt(dXh*dXh + dYh*dYh);
            dMiddleDistance = dOuterLength/dWidth;
         }
         double dMiddleX = baseX - dMiddleDistance*dXn;
         double dMiddleY = baseY - dMiddleDistance*dYn;
     
         
         path.moveTo((float)(scale*baseX), (float)(scale*baseY));
         path.lineTo((float)(scale*dLeftX), (float)(scale*dLeftY));
         
         if (2 == type)
         {
            double dX2 = dLeftX - baseX;
            double dY2 = dLeftY - baseY;
            
            double dShortenF2 = 1/Math.sqrt(dX2*dX2+dY2*dY2);
      
            double dX2n = dShortenF2 * dX2;
            double dY2n = dShortenF2 * dY2;
            
            // some curving
            path.quadTo((float)(dLeftX - (linewidth/2.0)*dY2n + (linewidth/1.4)*dX2n), 
                        (float)(dLeftY + (linewidth/2.0)*dX2n + (linewidth/1.4)*dY2n), 
                        (float)(dLeftX - linewidth*dY2n), (float)(dLeftY + linewidth*dX2n));

         }
        
         if (3 == type || 2 == type)
         {
            path.lineTo((float)(scale*dMiddleX), (float)(scale*dMiddleY));
         }
         
         if (2 == type)
         {
            double dX3 = dRightX - baseX;
            double dY3 = dRightY - baseY;
            
            double dShortenF3 = 1/Math.sqrt(dX3*dX3+dY3*dY3);
      
            double dX3n = dShortenF3 * dX3;
            double dY3n = dShortenF3 * dY3;
            
            path.lineTo((float)(dRightX + linewidth*dY3n), (float)(dRightY - linewidth*dX3n));
           
            // some curving
            path.quadTo((float)(dRightX + (linewidth/2.0)*dY3n + (linewidth/1.4)*dX3n), 
                        (float)(dRightY - (linewidth/2.0)*dX3n + (linewidth/1.4)*dY3n), 
                        (float)(scale*dRightX), (float)(scale*dRightY));
         }
         else
            path.lineTo((float)(scale*dRightX), (float)(scale*dRightY));
         
         path.closePath();
         
         
         if (1 == type)
         {
            ptBase2.x = dLeftX + 0.5*(dRightX - dLeftX);
            ptBase2.y = dLeftY + 0.5*(dRightY - dLeftY);
         }
         else
         {
            ptBase2.x = dMiddleX;
            ptBase2.y = dMiddleY;
         }
      }
      else // type 4,5
      {
         dLength = dLength/2;
         
         double dPoint1X = baseX + dLength*linewidth*dXn; // point in front of base
         double dPoint1Y = baseY + dLength*linewidth*dYn;
         double dPoint2X = baseX + dWidth*linewidth*dYn; // point left of base
         double dPoint2Y = baseY - dWidth*linewidth*dXn;
         double dPoint3X = baseX - dLength*linewidth*dXn; // point behind base
         double dPoint3Y = baseY - dLength*linewidth*dYn;
         double dPoint4X = baseX - dWidth*linewidth*dYn; // point right of base
         double dPoint4Y = baseY + dWidth*linewidth*dXn;
         double[] dPointsXY = { dPoint1X, dPoint1Y, dPoint2X, dPoint2Y, 
            dPoint3X, dPoint3Y, dPoint4X, dPoint4Y };
         
         if (4 == type)
         {
            path.moveTo((float)(scale*(dPoint1X)), (float)(scale*(dPoint1Y)));
            path.lineTo((float)(scale*(dPoint2X)), (float)(scale*(dPoint2Y)));
            path.lineTo((float)(scale*(dPoint3X)), (float)(scale*(dPoint3Y)));
            path.lineTo((float)(scale*(dPoint4X)), (float)(scale*(dPoint4Y)));
                     
            path.closePath();
         }
         else // type 5
         {
            path.moveTo((float)(scale*(dPoint1X)), (float)(scale*(dPoint1Y)));
               
            // make a circle
            // this is not precisely a circle but it comes close enough
            // for an arrow head
            for (int i=0; i<4; ++i)
            {
               int j = i < 3 ? i+1 : 0;
               
               double dMiddle1X = dPointsXY[2*i] + 0.5*(dPointsXY[2*j]-dPointsXY[2*i]);
               double dMiddle1Y = dPointsXY[2*i+1] + 0.5*(dPointsXY[2*j+1]-dPointsXY[2*i+1]);
            
               path.quadTo((float)(scale*(baseX + 1.87*(dMiddle1X-baseX))),
                           (float)(scale*(baseY + 1.87*(dMiddle1Y-baseY))),
                           (float)(scale*(dPointsXY[2*j])), (float)(scale*(dPointsXY[2*j+1])));
            }

         }
         
         ptBase2.x = baseX;
         ptBase2.y = baseY;
      } // create type 4,5

      return path;
   }
	
	protected Font matchFont(String familyName, String ttfName, 
                            String slantText, String weightText, 
                            FileResources resources) {
      
      String fontIdentifier = familyName +
         ((ttfName != null && ttfName.length() > 0) ? " ("+ttfName+")" : "");
      // used for both: key in the fontMap and display string for message dialogs
         
      // try to find it already loaded (important for embedded fonts)
      Font font = (Font)fontMap.get(fontIdentifier);
       
      boolean alreadyFailed = fontFailure.containsKey(fontIdentifier);
     
      HashMap attributeMap = new HashMap();
      attributeMap.put(TextAttribute.FAMILY, familyName);
      if (slantText.equals("italic"))
         attributeMap.put(TextAttribute.POSTURE, TextAttribute.POSTURE_OBLIQUE);
      else
         attributeMap.put(TextAttribute.POSTURE, TextAttribute.POSTURE_REGULAR);
      if (weightText.equals("bold"))
         attributeMap.put(TextAttribute.WEIGHT, TextAttribute.WEIGHT_BOLD);
      else
         attributeMap.put(TextAttribute.WEIGHT, TextAttribute.WEIGHT_REGULAR);
       
      // just to make sure slant and weight are set shouldn't do much harm if
      // they are already set
      if (font != null)
      {
         font = font.deriveFont(attributeMap); 
      }
     
      if (font == null && !alreadyFailed)
      {
         // try to find an exact match
               
         font = new Font(attributeMap);
         if (familyName.equals(font.getFamily()))
         {
            // everything alright, font is installed and usable
         }
         else
         {
            font = null; // not yet found
            
            /* does not work very well with Arial either:
            // yet again a special hack for frutiger:
            // as frutiger (loaded from a ttf file (below))
            // looks broken
            if (familyName.startsWith("Frutiger"))
            {
               if (familyName.equals("Frutiger 57Cn"))
                  familyName = "Arial";
               else if (familyName.equals("Frutiger 47LightCn"))
                  familyName = "Arial Narrow";
      
               attributeMap.put(TextAttribute.FAMILY, familyName);
      
               font = new Font(attributeMap);
               if (familyName.equals(font.getFamily()))
               {
                  // everything alright, font is installed and usable
               }
               else
               {
                  font = null; // still not found
               }
            }
            */
         }
      }
      
      if (font == null && !alreadyFailed)
      {
         InputStream fontInputStream = null;
         if (ttfName != null && ttfName.length() > 0)
            fontInputStream = resources.createFontStream(ttfName);
         if (fontInputStream == null)
         {
            // this is the normal error case:
            // font is not installed and not embedded
            
            if (!fontFailure.containsKey(fontIdentifier))
            {
               Manager.showError(Manager.getLocalized("fontNotFound")+" "+fontIdentifier
                                 +"\n"+Manager.getLocalized("displayInvalid"), Manager.WARNING, null);
               fontFailure.put(fontIdentifier, null);
            }
         }
         else
         {
            try {
               // this doesn't delete the created temporary font file: 
               //font = Font.createFont(Font.TRUETYPE_FONT, fontInputStream);
               // but hopefully this does:
               //System.out.println("Trying to load "+familyName+" "+weightText+" "+slantText);
               font = createTemporaryFont(fontInputStream, familyName, attributeMap);
               
               // font == null is possible
            } catch (Exception e) {
               if (!fontFailure.containsKey(ttfName))
               {
                  Manager.showError(Manager.getLocalized("fontNotValid")+" "+fontIdentifier
                                    +"\n"+Manager.getLocalized("displayInvalid"), Manager.WARNING, e);
                  fontFailure.put(fontIdentifier, null);
               }
            }

            try
            {
               fontInputStream.close();
            }
            catch (IOException exc)
            {
            }
         }
      }
      
      if (font == null)
      {
         // this is the default fallback:
         // font is not installed or embedded, an error occured or it is an old document
         
         String fallbackFamily = "Arial";
         
         familyName = familyName.toLowerCase();
     
         if (familyName.indexOf("symbol") != -1)
            fallbackFamily = "Symbol";
         else if (familyName.indexOf("times") != -1)
            fallbackFamily = "Times New Roman";
         else if (familyName.indexOf("courier") != -1)
            fallbackFamily = "Courier New";
      
         attributeMap.put(TextAttribute.FAMILY, fallbackFamily);
         
         font = new Font(attributeMap);
      }
      
      if (fontMap.get(fontIdentifier) == null)
      {
         fontMap.put(fontIdentifier, font);
      }      
      
      return font;
	}
	
   /* outdated and replaced by the above method, which was previously the "old" one
	// new style with ttf fonts
	protected Font matchFont(String ttfName, String ttfSlant, String ttfWeight, FileResources resources) {
		Font font = (Font)fontMap.get(ttfName);

      if (font == null)
      {
         InputStream fontInputStream = resources.createFontStream(ttfName);
         if (fontInputStream == null)
         {
            if (!fontFailure.containsKey(ttfName))
            {
               Manager.showError(Manager.getLocalized("fontNotFound")+" "+ttfName
                                 +"\n"+Manager.getLocalized("displayInvalid"), Manager.WARNING, null);
               fontFailure.put(ttfName, null);
            }
         }
         else
         {
            try {
               font = Font.createFont(Font.TRUETYPE_FONT, fontInputStream);
            } catch (Exception e) {
               if (!fontFailure.containsKey(ttfName))
               {
                  Manager.showError(Manager.getLocalized("fontNotValid")+" "+ttfName
                                    +"\n"+Manager.getLocalized("displayInvalid"), Manager.WARNING, e);
                  fontFailure.put(ttfName, null);
               }
            }

            try
            {
               fontInputStream.close();
            }
            catch (IOException exc)
            {
            }
         }
         if (font == null)
         {
            ttfName = ttfName.toLowerCase();
            font = new Font(ttfName.substring(0, ttfName.lastIndexOf(".ttf")), Font.PLAIN, 1);
         }

         fontMap.put(ttfName, font);
      }

		int style = 0;
		if (ttfSlant.equals("italic")) style |= Font.ITALIC; 
		if (ttfWeight.equals("bold")) style |= Font.BOLD;
		if (style != 0) font = font.deriveFont(style);
		
		return font;
   }
   */
	
   // TODO change parameter names (strLine, reader); also in ButtonArea
   public static VisualComponent CreateComponent(
      String line, BufferedReader in, ObjectQueue mother, ArrayList alPrevious)
   throws IOException
   {
      VisualComponent vc = null;
      
      if      (line.startsWith("<POINTER"))
         vc = new Pointer(line, in);
      else if (line.startsWith("<LINE"))
         vc = new Line(line, in);
      else if (line.startsWith("<TEXT"))
         vc = new Text(line, in, mother.GetResources(),
                       mother.GetCodepage(), mother.IsConsoleOperation());
      else if (line.startsWith("<OUTLINERECT"))
         vc = new OutlineRectangle(line, in);
      else if (line.startsWith("<FREEHAND"))
         vc = new Freehand(line, in, true, null);
      else if (line.startsWith("<POLYLINE"))
      {
         // HACK:
         // The Assistant writes two lines and a polyline object
         // if the polyline has two arrow heads. The normal lines
         // have/show the arrow heads.
         // The polyline now has to know about these and what to do
         // about it (shorten itself a bit maybe).
                  
         int polyArrowStyle = 0;
         float[] shortenValues = null;
         int idx = line.indexOf("polyArrowStyle=");
         if (idx > -1)
         {
            try
            {
               polyArrowStyle = Integer.parseInt(line.substring(idx+15, idx+16));
            }
            catch (Exception exc)
            {
               // ignore for now
            }
                     
            if (polyArrowStyle > 0)
            {
               shortenValues = new float[(polyArrowStyle < 3) ? 1 : 2];
                        
               if (polyArrowStyle > 2 && alPrevious.size() > 1)
               {
                  shortenValues[0] = ((Line)alPrevious.get(alPrevious.size()-2)).getArrowShorteningForPoly();
                  shortenValues[1] = ((Line)alPrevious.get(alPrevious.size()-1)).getArrowShorteningForPoly();
               }
               else if (alPrevious.size() > 0)
                  shortenValues[0] = ((Line)alPrevious.get(alPrevious.size()-1)).getArrowShorteningForPoly();
            }
         }
                  
         vc = new Freehand(line, in, false, shortenValues);
      }
      else if (line.startsWith("<OUTLINECIRC"))
         vc = new OutlineCircle(line, in);
      else if (line.startsWith("<IMAGE"))
         vc = new Picture(line, in, mother.GetResources(), mother.GetImageVault(),
                          mother.GetImageIds(), mother.GetCodepage());
      else if (line.startsWith("<CHAR"))
         vc = new Character(line, in, mother.GetResources());
      else if (line.startsWith("<FILLEDPOLY"))
         vc = new FilledPolygon(line, in);
      else if (line.startsWith("<FILLEDRECT"))
         vc = new FilledRectangle(line, in);
      else if (line.startsWith("<FILLEDCIRC"))
         vc = new FilledCircle(line, in);
      else if (line.startsWith("<AREA"))
         vc = new InteractionArea(line, in);
      else if (line.startsWith("<BUTTON"))
         vc = new ButtonArea(line, in, mother, alPrevious);
	 else if (line.startsWith("<CALLOUT"))
		 vc = new ButtonArea(line, in, mother, alPrevious);
      else if (line.startsWith("<RADIO"))
         vc = new RadioDynamic(line, in, false);
      else if (line.startsWith("<CHECK"))
         vc = new RadioDynamic(line, in, true);
      else if (line.startsWith("<FIELD"))
         vc = new TextFieldDynamic(line, in);
      else if (line.startsWith("<TARGET"))
         vc = new TargetPoint(line, in);
      
      return vc;
   }
               
               
   public static Font createTemporaryFont(InputStream inputStream, 
                                          String familyName, Map attributes)
   throws java.awt.FontFormatException, java.io.IOException
   {
      String sJavaVersion = System.getProperty("java.vm.version");
      //System.out.println(sJavaVersion);
      //if (sJavaVersion != null && sJavaVersion.length() >= 3 && sJavaVersion.charAt(2) >= '5')
      {
         // Java 1.5 doesn't have SunGraphicsEnvironment.crateFont() anymore.
         // It does however have a Font.createFont(..., File), but this is 
         // new to 1.5 and we still use 1.3 or 1.4. So use the old (and buggy) way.
         // ?
         
         Font font = Font.createFont(Font.TRUETYPE_FONT, inputStream);
         if (font == null)
            return new Font(attributes);
         else
            return font.deriveFont(attributes);
      }
      //else
      //{
         /* Maybe no more with Java 1.5
         // Java has a bug in Font.createFont(..., InputStream): The temporary
         // file doesn't get deleted.
         
         // copied from java.awt.Font.java
         // with some subtle changes concerning the deletion of the created file
      
         // first copy the font file to the temporary directory
         // then create it with SunGraphicsEnvironment.createFont()
         // that will register it under it's family name
         // then try to construct it with normal means
      
         File tempFontFile = File.createTempFile("font", ".ttf", null);
         //System.out.println("Created: "+tempFontFile+" for "+familyName);
         tempFontFile.deleteOnExit();

         FileOutputStream theOutputStream = new FileOutputStream(tempFontFile);

         int bytesRead = 0;
         byte buf[];
         int bufSize = 8192;
         buf = new byte[bufSize]; 
         while (bytesRead != -1) {
            bytesRead = inputStream.read(buf, 0, bufSize);
            if (bytesRead != -1)
               theOutputStream.write(buf, 0, bytesRead);
         }
         theOutputStream.close();
      
         SunGraphicsEnvironment env =
            (SunGraphicsEnvironment)GraphicsEnvironment.getLocalGraphicsEnvironment();
         String createName = env.createFont(tempFontFile);
         if (createName == null) {
            throw new FontFormatException("Unable to create font - bad font data");
         }
      
         if (createName.startsWith(familyName))
            return new Font(attributes);
         else
            return null;
            */
      //}
   }
}