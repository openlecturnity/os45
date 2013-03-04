package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.font.*;
import java.awt.geom.AffineTransform;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.awt.geom.Line2D;
import java.awt.geom.RoundRectangle2D;
import java.awt.image.BufferedImage;
import java.io.*;
import java.net.URL;
import java.util.*;

import imc.epresenter.player.util.SGMLParser;
import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.SearchEngine;

public class Text extends VisualComponent {
   public static int FONT_BOLD_MASK        =  1;
   public static int FONT_ITALIC_MASK      =  2;
   public static int FONT_EMBOSS_MASK      =  4;
   public static int FONT_SHADOW_MASK      =  8;
   public static int FONT_UNDERLINE_MASK   = 16;
   public static int FONT_SUBSCRIPT_MASK   = 32;
   public static int FONT_SUPERSCRIPT_MASK = 64;
   
   public static int TEXT_TYPE_NOTHING = 0;
   public static int TEXT_TYPE_CHANGE_TIME = 1;
   public static int TEXT_TYPE_CHANGE_TRIES = 2;
   public static int TEXT_TYPE_CHANGE_NUMBER = 3;
   public static int TEXT_TYPE_EVAL_Q_CORRECT = 5;
   public static int TEXT_TYPE_EVAL_Q_POINTS = 6;
   public static int TEXT_TYPE_EVAL_QQ_PASSED = 7;
   public static int TEXT_TYPE_EVAL_QQ_FAILED = 8;
   public static int TEXT_TYPE_EVAL_QQ_ACHIEVED = 9;
   public static int TEXT_TYPE_EVAL_QQ_ACHIEVED_PERCENT = 10;
   public static int TEXT_TYPE_EVAL_QQ_TOTAL = 11;
   public static int TEXT_TYPE_EVAL_QQ_REQUIRED = 12;
   public static int TEXT_TYPE_EVAL_QQ_REQUIRED_PERCENT = 13;
   private static int TEXT_TYPE_LAST_CHANGE = 14;
   public static int TEXT_TYPE_BLANK_SUPPORT = 15;
   public static int TEXT_TYPE_RADIO_SUPPORT = 16;
   public static int TEXT_TYPE_QUESTION_TITLE = 17;
   public static int TEXT_TYPE_QUESTION_TEXT = 18;
   public static int TEXT_TYPE_NUMBER_SUPPORT = 19;
   public static int TEXT_TYPE_TRIES_SUPPORT = 20;
   public static int TEXT_TYPE_TIME_SUPPORT = 21;
   public static int TEXT_TYPE_TARGET_SUPPORT = 22;
  
   private short textX, textY, textWidth, textHeight;
   
	private short[] xvalues;
	private short[] yvalues;
   private short[] allowedWidths;
	private Color[] colors;
   private String[] families;
   private int[] sizesInt;
	private String[] contents;
	private Font[] fonts;
   private GlyphVector[] currentGlyphs;
   private int[] styles;
   private String[] ttfNames;
   private double currentScale = 1.0;
   private boolean doMakeAntialias = true;
   private String m_strInputCodepage;
   private String allContent_ = null;
   private int m_iType = TEXT_TYPE_NOTHING; // 0 means undefined

   // PZI: used to highlight matches of full text search
   private String[] m_astrSearchWords;

   private static Graphics2D graphics2D;
   
   
   public static int GetTextTypeFromString(String strTextType)
   {
      int iType = TEXT_TYPE_NOTHING;
      
      if (strTextType != null && strTextType.length() > 0)
      {
          if (strTextType.equals("time"))
            iType = TEXT_TYPE_CHANGE_TIME;
         else if (strTextType.equals("tries"))
            iType = TEXT_TYPE_CHANGE_TRIES;
         else if (strTextType.equals("number"))
            iType = TEXT_TYPE_CHANGE_NUMBER;
         else if (strTextType.equals("q-correct"))
            iType = TEXT_TYPE_EVAL_Q_CORRECT;
         else if (strTextType.equals("q-points"))
            iType = TEXT_TYPE_EVAL_Q_POINTS;
         else if (strTextType.equals("qq-passed"))
            iType = TEXT_TYPE_EVAL_QQ_PASSED;
         else if (strTextType.equals("qq-failed"))
            iType = TEXT_TYPE_EVAL_QQ_FAILED;
         else if (strTextType.equals("qq-achieved"))
            iType = TEXT_TYPE_EVAL_QQ_ACHIEVED;
         else if (strTextType.equals("qq-achieved-p"))
            iType = TEXT_TYPE_EVAL_QQ_ACHIEVED_PERCENT;
         else if (strTextType.equals("qq-total"))
            iType = TEXT_TYPE_EVAL_QQ_TOTAL;
         else if (strTextType.equals("qq-required"))
            iType = TEXT_TYPE_EVAL_QQ_REQUIRED;
         else if (strTextType.equals("qq-required-p"))
            iType = TEXT_TYPE_EVAL_QQ_REQUIRED_PERCENT;
         else if (strTextType.equals("blank-s"))
            iType = TEXT_TYPE_BLANK_SUPPORT;
         else if (strTextType.equals("radio-s"))
            iType = TEXT_TYPE_RADIO_SUPPORT;
         else if (strTextType.equals("title"))
            iType = TEXT_TYPE_QUESTION_TITLE;
         else if (strTextType.equals("question"))
            iType = TEXT_TYPE_QUESTION_TEXT;
         else if (strTextType.equals("number-s"))
            iType = TEXT_TYPE_NUMBER_SUPPORT;
         else if (strTextType.equals("tries-s"))
            iType = TEXT_TYPE_TRIES_SUPPORT;
         else if (strTextType.equals("time-s"))
            iType = TEXT_TYPE_TIME_SUPPORT;
         else if (strTextType.equals("target-s"))
            iType = TEXT_TYPE_TARGET_SUPPORT;
         //else
         //   System.err.println("Text: Didn't recognize type: "+strTextType);
      }
      
      return iType;
   }
   

   Text(String line, BufferedReader in, FileResources resources, String strInputCodepage) {
      this(line, in, resources, strInputCodepage, false);
   }
   
	Text(String line, BufferedReader in, FileResources resources, String strInputCodepage, boolean bConsoleOperation) {
		
		SGMLParser parser = new SGMLParser(line);
		int blockStart = extractInteger(parser, "x");
		int blockWidth = extractInteger(parser, "width");
		
      int x = blockStart;
      int baseLineFirstItem = extractInteger(parser, "y");
      int width = blockWidth;
      int height = extractInteger(parser, "height");
      
      String strTextType = extractString(parser, "type");
      if (strTextType != null && strTextType.length() > 0)
         m_iType = GetTextTypeFromString(strTextType);
   
      if (m_iType != 0)
         super.ParseActivityVisibility(parser);
      
      textX = (short)x; textY = (short)baseLineFirstItem; 
      textWidth = (short)width; textHeight = (short)height;
      
      m_strInputCodepage = strInputCodepage;

		// Sammle alle Texte und deren Attribute ein
		ArrayList itemCollector = new ArrayList(10);
		String input = null;
		try {
			while (!(input = in.readLine()).startsWith("</TEXT>")) {
				itemCollector.add(input);
				itemCollector.add(in.readLine());
				String l = in.readLine();
            while (!l.startsWith("</TEXTI")) l = in.readLine();  // jump over empty lines
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		String[] items = new String[itemCollector.size()];
		itemCollector.toArray(items);
		
		int numItems = items.length/2;
		xvalues = new short[numItems];
		yvalues = new short[numItems];
      allowedWidths = new short[numItems];
		colors = new Color[numItems];
      families = new String[numItems];
      sizesInt = new int[numItems];
		contents = new String[numItems];
		fonts = new Font[numItems];
      currentGlyphs = new GlyphVector[numItems];
      styles = new int[numItems];
      ttfNames = new String[numItems];
      //String[] fontNames = new String[numItems];
		short[] xoffs = new short[numItems];
		short[] yoffs = new short[numItems];
		short[] lines = new short[numItems];
		float[] sizes = new float[numItems];

      for (int i=0; i<numItems; i++) {
			SGMLParser parser2 = new SGMLParser(items[2*i]);
			xvalues[i] = extractShort(parser2, "x");
			yvalues[i] = extractShort(parser2, "y");
			colors[i] = createColor(extractInteger(parser2, "ccolor"),
									extractString(parser2, "rgb"));
        
			contents[i] = removeEscapes(items[2*i+1]);
         
         
         xoffs[i] = extractShort(parser2, "offx");
         
         // dodgy workaround
         //if (xoffs[i] > width) width += xoffs[i];
         
         // better workaround ?
         if (xoffs[i] > 0 && numItems == 1) width += xoffs[i];
			
         yoffs[i] = extractShort(parser2, "offy");
         lines[i] = extractShort(parser2, "line");
			sizes[i] = extractFloat(parser2, "size");
         sizesInt[i] = Math.round(sizes[i]);
			ttfNames[i] = extractPath(parser2, "ttf", strInputCodepage);
			String familyName = extractString(parser2, "font");
         families[i] = familyName;
         String slant = extractString(parser2, "slant");
         String weight = extractString(parser2, "weight");
         styles[i] = extractShort(parser2, "style");
         //fontNames[i] = ttfNames[i]+oldFontName+slant+weight;
   
         String slantText = "regular";
         String weightText = "normal";
         if (slant != null && slant.length() > 0)
            slantText = slant;
         if (weight != null && weight.length() > 0)
            weightText = weight;
         
         //
         // just make sure bold and italic ARE reflected in the style value
         //
         if (weightText.equals("bold"))
            styles[i] |= FONT_BOLD_MASK;

         if (slantText.equals("italic"))
            styles[i] |= FONT_ITALIC_MASK;
         
        
         //if (familyName.toLowerCase().indexOf("frutiger") != -1)
         //   doMakeAntialias = false; // special case for Frutiger
         // Seems to work better with Java 1.5 (since 2.0.p2)
	         
         // java seems to be unable to load "Helvetica", so in order
         // to avoid "sansserif" we use "Arial"
         // slant and weight are handled by matchFont (and its attribute map)
         if (familyName.toLowerCase().indexOf("helvetica") != -1)
            familyName = "Arial";
         
         // java does not support kerning, the T in the TeleLogo font
         // is thus placed wrongly (too far to the right)
         // Note: this is only _one_ (out of many) special cases
         if (familyName.toLowerCase().indexOf("telelogo") != -1 
             && contents[i].length() == 1 && contents[i].charAt(0) == '"')
            xvalues[i] -= sizes[i]/8;
         
         //if (!bConsoleOperation)
         {
            fonts[i] = matchFont(familyName, ttfNames[i], slantText, weightText, resources);
            
            fonts[i] = fonts[i].deriveFont(sizes[i]);
         }
		}
		
      //if (!bConsoleOperation)
      // Seems to be working with Java 1.5 even if headless; it is needed to produce page images.
      {
         //
         // Textbreitenkorrektur:
         // Fuer manche Textitems ergibt sich die Breite
         // aus der x-Wert des in der Zeile nachfolgenden.
         // Fuer andere aus einer Ueberschreitung der Gesamtbreite.
         //
         if (graphics2D == null) 
         {
            graphics2D = new BufferedImage(
               1, 1, BufferedImage.TYPE_INT_RGB).createGraphics();
            RenderingHints renderHints = new RenderingHints(null);
            //renderHints.put(RenderingHints.KEY_FRACTIONALMETRICS,
            //                RenderingHints.VALUE_FRACTIONALMETRICS_ON);
            //renderHints.put(RenderingHints.KEY_TEXT_ANTIALIASING,
            //             RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
            if (doMakeAntialias)
            {
               renderHints.put(RenderingHints.KEY_ANTIALIASING,
                               RenderingHints.VALUE_ANTIALIAS_ON);
            }
            graphics2D.setRenderingHints(renderHints);
         }

         //
         // find allowed widths for later width adjustments
         //
         for (int i=0; i<contents.length; i++) {
            graphics2D.setFont(fonts[i]);
            int fontWidth = graphics2D.getFontMetrics().stringWidth(contents[i]);

            int targetWidth = fontWidth;
            if (i+1 < contents.length && lines[i+1] == lines[i]) {
               // in the same line: a TEXTITEM right of this one
               targetWidth = xvalues[i+1]-(xvalues[i]+xoffs[i]);
            } else if (blockWidth > 0 && xvalues[i]+xoffs[i]+fontWidth > blockStart+blockWidth) {
               // this line exceeds the width of the text block
               targetWidth = blockStart+blockWidth-(xvalues[i]+xoffs[i]);
            }
            allowedWidths[i] = (short)targetWidth;
         }

         // Korrektur von xvalue und yvalue
         for (int i=0; i<xvalues.length; i++) {
            xvalues[i] += xoffs[i];
            yvalues[i] += yoffs[i];
         }
		
         //
         // compute an upper bound for the width of this text object
         // (hopefully!)
         //
         int maxWidth = 0;
         for (int i=0; i<contents.length; i++)
         {
            graphics2D.setFont(fonts[i]);
            FontMetrics fm = graphics2D.getFontMetrics();
            int fontWidth = fm.stringWidth(contents[i]);
            int totalWidth = xoffs[i]+fontWidth;
            if (totalWidth > maxWidth) maxWidth = totalWidth;
         }
         if (maxWidth > width) width = maxWidth;
      

         clip = new Rectangle(x, baseLineFirstItem-(int)(height/(float)contents.length),
                              width, (int)(1.4f*height));
      
      } // !bConsoleOperation; Textbreiten-Korrektur
	}
   
   public boolean HasTextType()
   {
      return m_iType != TEXT_TYPE_NOTHING;
   }
   
   public boolean HasChangeableType()
   {
      return m_iType > TEXT_TYPE_NOTHING && m_iType < TEXT_TYPE_LAST_CHANGE;
   }
   
   public int GetTextType()
   {
      return m_iType;
   }
   
   public boolean ChangeText(String strNewText)
   {
      if (contents.length > 0 && !contents[0].equals(strNewText))
      {
         contents[0] = strNewText;
         currentScale = -1.0; // force recreation of glyphs
         return true;
      }
      else
         return false;
   }
	
	// Used by the Publisher to get font infos 
	public Font GetFont()
	{
	   if (contents.length > 0)
	      return fonts[0];
	   else
	      return null;
	}

	// Used by the Publisher to get font color 
	public Color GetFontColor()
	{
	   if (contents.length > 0)
	      return colors[0];
	   else
	      return null;
	}

	void paint(Graphics g, int width, int height, double scale) {
      Graphics2D g2D = (Graphics2D)g;
      Stroke strokeRestore = g2D.getStroke();      
      RenderingHints renderRestore = g2D.getRenderingHints();
      Font fontRestore = g.getFont();
      
      if (!doMakeAntialias)
         g2D.setRenderingHints(new RenderingHints(null)); 
      
      
      // using a scaling AffineTransform (disabled) 
      double realScale = 1.0;//scale;
      //scale = 1.0;
    
      boolean bDoWidthAdaption = !HasChangeableType();
      
      
      for (int i=0; i<contents.length; i++) {
         Font currentFont = fonts[i];

         // have the size be divideable by 0.5
         if (scale > 1.01 || scale < 0.99)
				currentFont = fonts[i].deriveFont(
               fontSizeRound((float)(scale*fonts[i].getSize2D())));
         
         g.setFont(currentFont);

         if (g.getFont().getSize2D() > 0.8)
         {
            g.setColor(colors[i]);
          
            if (scale != currentScale || currentGlyphs[i] == null)
            {
               FontMetrics fm = g2D.getFontMetrics();
               int fontWidth = fm.stringWidth(contents[i]);
               FontRenderContext frc = g2D.getFontRenderContext();

               float allowedWidth = (float)(scale*allowedWidths[i]);
               
               if (Math.abs(fontWidth-allowedWidth) > 0 && bDoWidthAdaption) // text too wide
               {
                  /*
                  //
                  // first correction (font size)
                  //
                  float factor = allowedWidth/fontWidth;
                  currentFont = currentFont.deriveFont(factor*currentFont.getSize2D());
            
                  g2D.setFont(currentFont);
                  fm = g2D.getFontMetrics();
                  fontWidth = fm.stringWidth(contents[i]);
                  frc = g2D.getFontRenderContext();
                  */

                  currentGlyphs[i] = createGlyphVector(frc, contents[i], currentFont);
                  
                  if (contents[i].length() > 1)
                  {
                     //
                     // second correction (glyph spacing)
                     //
                     float factor2 = allowedWidth/fontWidth;
                     // Note: factor2 may also be > 1 but is never;
                     // text is never made wider than necessary
                     // see: calculation of targetwidth/allowedwidth
                     //
                     // 7.12.03: not true!
                     scaleGlyphVector(currentGlyphs[i], factor2);
                     //if (fonts[i].getFamily().startsWith("Frutiger"))
                     //   System.out.println("Scale: "+factor2);
         
                  }

               }
               else // no correction needed
                  currentGlyphs[i] = createGlyphVector(frc, contents[i], currentFont);

            }

            AffineTransform oldT = g2D.getTransform();
               
            if (realScale != 1.0)
            {
               AffineTransform scaleT = AffineTransform.getScaleInstance(realScale, realScale);
               
               // this affects the shape itself (the size) but not the placement
               // with drawString(), it does however affect draw(Line).
               g2D.setTransform(scaleT);
            }
            
            float scaledX = (float)(scale*xvalues[i]);
            float scaledY = (float)(scale*yvalues[i]);
           
            // PZI: highlight search matches
            // TODO: rework access to words
            if (m_astrSearchWords != null) {
               // check all search words
               for (String word : m_astrSearchWords) {
                  // highlight all matches
                  int pos = contents[i].toUpperCase().indexOf(word);
                  while (pos >= 0) {
                     // determine bounds of matched text
                     Rectangle2D rect = currentGlyphs[i].getGlyphOutline(pos, scaledX, scaledY)
                           .getBounds2D();
                     for (int index = 0; index < word.length(); index++)
                        rect = rect.createUnion(currentGlyphs[i].getGlyphOutline(pos + index,
                              scaledX, scaledY).getBounds2D());
                     // create rounded rectangle for highlighting
                     double spacingX = 5d;
                     double spacingY = 3d;
                     RoundRectangle2D roundRectangle2D = new RoundRectangle2D.Double(rect.getX()
                           - spacingX, rect.getY() - spacingY, rect.getWidth() + 2 * spacingX, rect
                           .getHeight()
                           + 2 * spacingY, 10, 10);

                     // draw highlighting
                     g2D.setColor(new Color(255, 255, 0, 200));
                     g2D.fill(roundRectangle2D);
                     g2D.setColor(new Color(200, 200, 0));
                     g2D.draw(roundRectangle2D);

                     // get next match
                     pos = contents[i].toUpperCase().indexOf(word, pos + 1);
                  }
               }
            }

            g.setColor(colors[i]);
            g2D.drawGlyphVector(currentGlyphs[i], scaledX, scaledY);
            
            
            /*
            // debugging purposes: mark baseline start
            g2D.setStroke(new BasicStroke(1.0f));
            g2D.setColor(Color.red);
            Line2D line1 = new Line2D.Float(scaledX, scaledY, scaledX+2, scaledY-2);
            g2D.draw(line1);
            Line2D line2 = new Line2D.Float(scaledX, scaledY, scaledX+2, scaledY+2);
            g2D.draw(line2);
            */
            
            
            // paint an underline if necessary
            if ((styles[i] & FONT_UNDERLINE_MASK) != 0)
            {
               // we could of course use java.awt.font.TextLayout (normal 
               // Graphics.drawString() doesn't do/support underlined text)
               // but that doesn't look like windows, so we just do it manually
               
               Rectangle2D rect = currentGlyphs[i].getOutline(scaledX, scaledY).getBounds2D();
               float lineWidth = (float)rect.getBounds().width;
                         
               float underlineSize = g.getFont().getSize2D()/10;
               if (underlineSize < 1)
                  underlineSize = 1.0f;
               BasicStroke stroke = new BasicStroke(underlineSize);
               g2D.setStroke(stroke);
               
               // the x-placement of the text differs slightly from scaledX and scaledY
               Line2D line = new Line2D.Float(
                  rect.getBounds().x, scaledY+underlineSize*1.5f,
                  rect.getBounds().x+lineWidth, scaledY+underlineSize*1.5f);
               
               g2D.draw(line);
            }
            
            
            
            if (realScale != 1.0)
            {
               g2D.setTransform(oldT);
            }
            
         } // if size > 0.8
           
      } // for contents
      
      currentScale = scale;
      
      if (!doMakeAntialias)
         g2D.setRenderingHints(renderRestore);
      
      /* draw bounding rectangle
      BasicStroke stroke = new BasicStroke(1.0f);
      g2D.setStroke(stroke);
               
      Rectangle bounds = g.getClipBounds();
      g.setColor(Color.red);
      bounds.x += 1; bounds.y += 1; bounds.width -= 2; bounds.height -= 2;
      g2D.draw(bounds);
      */
      
      g.setFont(fontRestore);
      g2D.setStroke(strokeRestore);
   }
	
	private static String removeEscapes(String input) {
		StringBuffer output = new StringBuffer();
		int oldIndex = -1;
		int index = input.indexOf("\\");
		while(index != -1) {
			output.append(input.substring(oldIndex+1, index));
			oldIndex = index;
			index = input.indexOf("\\", index+2);
		}
		output.append(input.substring(oldIndex+1, input.length()));
		
		return output.toString();
	}
	
	
	// sometimes lines are empty
	private String nextLine(BufferedReader br) throws IOException {
		String line = null;
		while((line = br.readLine()).length() == 0);
		return line;
	}


   private GlyphVector createGlyphVector(FontRenderContext frc, String content, Font font)
   {
      // Characters from "symbol" fonts might be specified from the 
      // "private use area" of the font. Or they can be specified between
      // 0-255. For the latter case a correction needs to be made here.
      //
      boolean bCanDisplay = true;
      boolean bAllBelow256 = true;
      
      int iStrLength = content.length();
      for (int i=0; i<iStrLength; ++i)
      {
         char c = content.charAt(i);
         
         if (!font.canDisplay(c))
            bCanDisplay = false;
         
         if (c > 255)
            bAllBelow256 = false;
      }
  
      if (!bCanDisplay && bAllBelow256)
      {
         StringBuffer strContentNew = new StringBuffer();
         
         boolean bIsPrivateArea = true;
         for (int i=0; i<iStrLength; ++i)
         {
            char c = content.charAt(i);
            c |= 0xf000;
            
            strContentNew.append(c);
            
            if (!font.canDisplay(c))            
            {
               bIsPrivateArea = false;
               break;
            }
         }
         
         if (bIsPrivateArea)
            content = strContentNew.toString();
      }
      
      
      GlyphVector gv = font.createGlyphVector(frc, content);
      
      // Special Bugfix #3358
      if (font.getFamily().equals("WingDings3"));
      {   
         Point2D p2prev = null;
         for (int i=0; i<iStrLength; ++i)
         {
            Point2D p2 = gv.getGlyphPosition(i);
            if (p2prev != null)
            {
               if (p2prev.getY() != p2.getY())
               {
                  p2.setLocation(p2.getX(), p2prev.getY());
                  gv.setGlyphPosition(i, p2);
               }
            }
            p2prev = p2;
         }
      }
     
      return gv;
      
      /* Was used (necessary?) for Java 1.3; deactivated for 2.0.p3
         Now characters of symbol fonts have a 0xf0 as first byte: Unicode "private range";
         this works automatically at least with Java 1.5
         
      boolean bSymbolTreatment = false;
      
      String lowerFontName = font.getFontName().toLowerCase();
      
      boolean bIsSymbolFont = lowerFontName.indexOf("symbol") != -1 || 
         lowerFontName.indexOf("marlett") != -1 ||
         lowerFontName.indexOf("dings") != -1 ||
         lowerFontName.indexOf("monotype") != -1;
      
      System.out.println(font.getFamily());
      for (int i=0; i<content.length(); ++i)
         System.out.print((int)content.charAt(i)+" ");
      System.out.println();
            
      
      if (bIsSymbolFont)
      {
         // Note
         // There can be two styles of accessing a font object: codepage (?)
         // and Unicode. The old style for symbol fonts was codepage. Using
         // that access style requires the below processing/translation of characters.
         
         bSymbolTreatment = true;
         
         for (int i=0; i<content.length(); ++i)
         {
            // If there is any character not in the 0-255 range
            // it is probably a string with Unicode access.
            
            if (content.charAt(i) > 0xff)
               bSymbolTreatment = false;
         }
      }
      
      if (bSymbolTreatment)
      {
         int[] glyphCodes = new int[content.length()];
         
         int fontIndex = 99;
         if (lowerFontName.indexOf("symbol") != -1)
            fontIndex = 0;
         if (lowerFontName.indexOf("marlett") != -1)
            fontIndex = 1;
         if (lowerFontName.indexOf("webdings") != -1)
            fontIndex = 2;
         if (lowerFontName.indexOf("wingdings") != -1)
         {
            fontIndex = 3;
            if (lowerFontName.indexOf("2") != -1)
               fontIndex = 4;
            else if (lowerFontName.indexOf("3") != -1)
               fontIndex = 5;
         }
         if (lowerFontName.indexOf("monotype sorts") != -1)
            fontIndex = 6;
         
         // the fonts with special symbols only use 256 characters at most
         byte[] abtCharacters = null;
         try
         {
            abtCharacters = content.getBytes("iso8859-1");
            // the below translation uses index values in this range
         }
         catch (UnsupportedEncodingException exc)
         {
            // shouldn't happen
            abtCharacters = content.getBytes();
         } 
         
         for (int j=0; j<glyphCodes.length; j++)
            glyphCodes[j] = translateSpecialChar(abtCharacters[j] & 0xff, fontIndex);
         
         return font.createGlyphVector(frc, glyphCodes);
      }
      else
      {     
         return font.createGlyphVector(frc, content);
      }
      */
   }

   /* Was used (necessary?) for Java 1.3; deactivated for 2.0.p3
   private int translateSpecialChar(int character, int fontIndex)
   {
      int result = character;
      //if (character == ' ')
      //   return 2;
      switch(fontIndex) 
      {
         case 0: // Symbol
            // between "glyph code" access and ascii access to the symbol font
            // are 3 gaps: 29 at the beginning, 34 in the middle and 1 near the end
            if (character == ' ')
               result = 3;
            else if (character > 0xef) 
               result -= 29+34+1;
            else if (character > 0x7f) 
               result -= 29+34;
            else 
               result -= 29;
            
            break;
         case 1: // Marlett
            if (character > 0x60)
               result -= 0x55;
            else if (character > 0x2f)
               result -= 0x2e;
            break;
         case 2:  // nearly all dings
         case 4:
         case 5:
            if (character == ' ')
               result = 2;
            else if (character > 0x7f)
               result -= 0x1e;
            else if (character > 0x20)
               result -= 0x1d;
            break;
         case 3:  // wingdings need very special treatment
            if (character == ' ')
               result = 2;
            else if (character >= 0xfb)
               result -= 0xfb-0x38;
            else if (character >= 0xb4)
               result -= 0xb4-0x9a;
            else if (character >= 0xa9)
               result -= 0xa9-0x8e;
            else if (character == 0xa8)
               result = 0x86;
            else if (character == 0xa7)
               result = 0x83;
            else if (character >= 0xa1)
               result -= 0xa1-0x7b;
            else if (character == 0xa0)
               result = 0x78;
            else if (character == 0x9f)
               result = 0x79;
            else if (character == 0x9e)
               result = 0x82;
            else if (character >= 0x96)
               result -= 0x96-0x57;
            else if (character >= 0x80)
               result -= 0x80-0x62;
            else if (character >= 0x7d)
               result -= 0x7d-0x53;
            else if (character >= 0x7b)
               result -= 0x7b-0x55;
            else if (character >= 0x78)
               result -= 0x78-0x5f;
            else if (character == 0x77)
               result = 0x8a;
            else if (character == 0x76)
               result = 0x99;
            else if (character == 0x75)
               result = 0x8b;
            else if (character >= 0x73)
               result -= 0x73-0x8c;
            else if (character == 0x70)
               result = 0x87;
            else if (character == 0x71)
               result = 0x89;
            else if (character == 0x72)
               result = 0x88;
            else if (character >= 0x6e)
               result -= 0x6e-0x84;
            else if (character == 0x6d)
               result = 0x81;
            else if (character == 0x6c)
               result = 0x7a;
            else if (character >= 0x55)
               result -= 0x55-0x3c;
            else if (character >= 0x21)
               result -= 0x21-0x04;
            break;
         case 6: // Monotype Sorts
            if (character == ' ') // otherwise it gets converted
               result = 2;
            else if (character >= 0xf1)
               result -= 0x32;
            else if (character >= 0xa1)
               result -= 0x31;
            else if (character >= 0x80)
               result -= 0x1e;
            else if (character >= 0x21)
               result -= 0x1d;
            
            break;
      }
      return result;
   }
   */

   private void scaleGlyphVector(GlyphVector gv, double factor)
   {
      int count = gv.getNumGlyphs();
      for (int i=0; i<count; i++)
      {
         Point2D p2D = gv.getGlyphPosition(i);
         p2D.setLocation(factor*p2D.getX(), p2D.getY());
         gv.setGlyphPosition(i, p2D);
      }

   }
   
   private float fontSizeRound(float x)
   {
      // maybe it's a good idea to have the font sizes 
      // be divideable by 0.5?
      return ((int)(x*2))/2.0f;
   }

   /**
    * This method returns <i>true</i> if the string given in
    * <tt>substring</tt> is contained within this <tt>Text</tt>
    * instance, otherwise <i>false</i>.
    */
   public boolean contains(String substring)
   {
      if (allContent_ == null)
         createAllContent();

      return (allContent_.indexOf(substring) >= 0);
   }

   public String getAllContent()
   {
      if (allContent_ == null)
         createAllContent();

      return allContent_;
   }

   private void createAllContent()
   {
      allContent_ = "";
      for (int i=0; i<contents.length; i++)
         allContent_ += contents[i];
   }
   
   // PZI: set words specified by full text search: used to highlight matches
   // null for no words
   public void setSearchWords(String[] astrSearchWords) {
      m_astrSearchWords = astrSearchWords;
   }
   
   public imc.lecturnity.info.LpdTextTag extractEverything(int pageNr)
   {
      imc.lecturnity.info.LpdTextTag textTag = 
         new imc.lecturnity.info.LpdTextTag(textX, textY, textWidth, textHeight, pageNr);
  
      /*
      imc.lecturnity.info.LpdTextItemTag[] itemTags =
         new imc.lecturnity.info.LpdTextItemTag[contents.length];
         */
      
      for (int i=0; i<contents.length; ++i)
      {
         imc.lecturnity.info.LpdTextItemTag item =
            new imc.lecturnity.info.LpdTextItemTag(
               (int)xvalues[i], (int)yvalues[i], families[i], 
               ttfNames[i], colors[i], styles[i], sizesInt[i], contents[i]);
         
         textTag.addTextItemTag(item);
      }
    
      return textTag;
   }
   
   // PZI: used to enable accessing search matches by event time
   private int m_iTimestamp;

   public int getTimestamp() {
      return m_iTimestamp;
   }

   public void setTimestamp(int timestamp) {
      m_iTimestamp = timestamp;
   }
   
}