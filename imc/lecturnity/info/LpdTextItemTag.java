package imc.lecturnity.info;

import java.awt.Color;

import imc.epresenter.player.whiteboard.Text;

/**
 * Specific class representing a TEXTITEM inside a TEXT tag.
 */
public class LpdTextItemTag
{

   /**
    * STYLE_NONE equals 0 and specifies that it is plain text.
    */
   public static final int STYLE_NONE = 0;
   /** Bit mask for the text attribute BOLD. */
   public static final int STYLE_BOLD = Text.FONT_BOLD_MASK;
   /** Bit mask for the text attribute ITALIC. */
   public static final int STYLE_ITALIC = Text.FONT_ITALIC_MASK;
   /** Bit mask for the text attribute EMBOSSED. */
   public static final int STYLE_EMBOSS = Text.FONT_EMBOSS_MASK;
   /** Bit mask for the text attribute SHADOWED. */
   public static final int STYLE_SHADOW = Text.FONT_SHADOW_MASK;
   /** Bit mask for the text attribute UNDERLINED. */
   public static final int STYLE_UNDERLINE = Text.FONT_UNDERLINE_MASK;
   /** Bit mask for the text attribute SUBSCRIPT. This bit being set
    has however no influence on the text position. */
   public static final int STYLE_SUBSCRIPT = Text.FONT_SUBSCRIPT_MASK;
   /** Bit mask for the text attribute SUPERSCRIPT. This bit being set
    has however no influence on the text position. */
   public static final int STYLE_SUPERSCRIPT = Text.FONT_SUPERSCRIPT_MASK;
   
   /** Constant returned by <code>getWeight()</code>. */
   public static final int WEIGHT_NORMAL = 0;
	/** Constant returned by <code>getWeight()</code>. */
   public static final int WEIGHT_BOLD = 1;


   private int m_iPosX;
   private int m_iPosY;
   private String m_strFontFamily;
   private String m_strTtfName;
   private Color m_Color;
   private int m_iStyle;
   private int m_iSize;
   private String m_strFullText;
   
   /** Internal building method. */
   public LpdTextItemTag(int x, int y,
                         String family, String ttf, Color c, int style,
                         int size, String fullText)
   {
      m_iPosX = x;
      m_iPosY = y;
      
      m_strFontFamily = family;
      m_strTtfName = ttf;
      m_Color = c;
      m_iStyle = style;
      m_iSize = size;
      
      m_strFullText = fullText;      
   }

   /**
    * Returns the full text specified by this TEXTITEM.
    *
    * @return the full text content
    */
   public String getText() 
   {
      return m_strFullText;
   }

   /**
    * Returns the color of this text. Same as <code>getRGB()</code>.
    *
    * @return the color of this text
    */
   public Color getCColor() 
   {
      return m_Color;
   }

   /**
    * Returns the color of this text. Contains normally the information
    *          specified by the <code>rgb</code> attribute and if that is
    *          missing the one of <code>ccolor</code>.
    *
    * @return the color of this text
    */
   public Color getRGB() 
   {
      return m_Color;
   }

   /**
    * Returns the font family.
    *
    * @return the font family
    */
   public String getFont() 
   {
      return m_strFontFamily;
   }

   /**
    * Returns the file name of the used TTF font. This can be a full path
    *          but normally it is only the file name itself. The fonts
    *          specified by a simple file name without path
    *          are all found in the Windows font directory.
    *
    * @return the TTF file name
    */
   public String getTTF() 
   {
      return m_strTtfName;
   }

   /**
    * Returns the x position of this TEXTITEM.
    *
    * @return the x position as an <code>int</code>
    */
   public int getX() 
   {
      return m_iPosX;
   }

   /**
    * Returns the y position of the baseline of this TEXTITEM.
    *
    * @return the y position as an <code>int</code>
    */
   public int getY() 
   {
      return m_iPosY;
   }

   /**
    * The attributes <code>offx</code> and <code>offy</code> of a TEXTITEM
    * are always 0. 
    * <p>
    * These fields were used of super or subscripted text. But currently 
    * these offsets are represented directly by the chanbed x and y position.
    *
    * @return always 0
    */
   public int getOffX() 
   {
      return 0;
   }

   /**
    * The attributes <code>offx</code> and <code>offy</code> of a TEXTITEM
    * are always 0. 
    * <p>
    * These fields were used of super or subscripted text. But currently 
    * these offsets are represented directly by the chanbed x and y position.
    *
    * @return always 0
    */
   public int getOffY() 
   {
      return 0;
   }

   /**
    * The STYLE_ constants and the <code>style</code> variable are used
    * for an easy specification of the font configuration. E.g. it contains
    * information wether the font/text is bold or not.
    * <p>
    * This information is combined bitwise in on <code>int</code> variable.
    * So in order to determine if a font/text is underlined you would check
    * if <code>getStyle() & STYLE_UNDERLINE</code> differs from 0.
    * <p>
    * 
    *
    * @return an <code>int</code> specifying style information about the text
    */
   public int getStyle() 
   {
      return m_iStyle;
   }
   
   /**
    * Returns the font height in pt values.
    *
    * @return the font height as an <code>int</code>
    */
   public int getSize() 
   {
      return m_iSize;
   }
   

   /**
    * TEXTITEMs have no width or height. Is always 0. Width and height are 
    * only specified for a TEXT object.
    *
    * @return always 0
    */
   public int getWidth() 
   {
      return 0;
   }

   /**
    * TEXTITEMs have no width or height. Is always 0. Width and height are 
    * only specified for a TEXT object.
    *
    * @return always 0
    */
   public int getHeight()
   {
      return 0;
   }

   /**
    * Returns the texts boldness as one of two constants.
    *
    * @return <code>WEIGHT_BOLD</code> if the text is bold and 
    * <code>WHEIGHT_NORMAL</code> otherwise
    */
   public int getWeight() 
   {
      if (0 != (m_iStyle & Text.FONT_ITALIC_MASK))
         return WEIGHT_BOLD;
      else
         return WEIGHT_NORMAL;
   }

   /**
    * Returns the italic attribute as a <code>String</code>.
    *
    * @return the string "italic" if the text is italic, "roman" otherwise
    */
   public String getSlant() 
   {
      if (0 != (m_iStyle & Text.FONT_ITALIC_MASK))
         return "italic";
      else
         return "roman";
   }
   
}