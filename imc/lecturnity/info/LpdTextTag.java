package imc.lecturnity.info;

import java.util.ArrayList;

/**
 * Class representing a TEXT tag inside the object queue of a presentation.
 */
public class LpdTextTag
{
   // the TEXTITEM objects contained in this TEXT object
   private ArrayList m_aTextItems;
   
	private int m_iPosX;
	private int m_iPosY;
	private int m_iWidth;
	private int m_iHeight;

	// every text corresponds to the slide it is on
   private int m_iPageNr;
  
   /** Internal building method. */
   public LpdTextTag(int x, int y, int w, int h, int page) 
   {
      m_aTextItems = new ArrayList(1);
      
      m_iPosX = x;
      m_iPosY = y;
      m_iWidth = w;
      m_iHeight = h;
      m_iPageNr = page;
   }

   /** Internal building method. */
   public void addTextItemTag(LpdTextItemTag tag)
   {
      m_aTextItems.add(tag);
   }
   
   
   /**
    * Returns an array of <code>LpdTextItemTag</code> objects.
    *
    * @return an array of <code>LpdTextItemTag</code> objects. Normally there
    *          is only one TEXTITEM tag inside a TEXT tag so the array lenght
    *          is 1.
    */
   public LpdTextItemTag[] getTextItemTags() 
   {
      LpdTextItemTag[] textItemTags = new LpdTextItemTag[m_aTextItems.size()];
      m_aTextItems.toArray(textItemTags);
      return textItemTags;
   }
   
   /**
    * Returns the number of the slide containing this TEXT tag.
    *
    * @return the number of the slide containing this TEXT tag. The numbering
    *          corresponds to the numbering of slides in the <code>Metadata</code>
    *          object.
    * @see     imc.epresenter.filesdk.Metadata
    */
   public int getPageNr() 
   {
      return m_iPageNr;
   }

   /**
    * Returns the width of this text block.
    *
    * @return an <code>int</code> denoting the width
    */
   public int getWidth() 
   {
      return m_iWidth;
   }

   /**
    * Returns the height of this text block.
    *
    * @return an <code>int</code> denoting the height
    */
   public int getHeight() 
   {
      return m_iHeight;
   }

   /**
    * Returns the x position of this text block.
    *
    * @return an <code>int</code> denoting the x position
    */
   public int getX() 
   {
      return m_iPosX;
   }

   /**
    * Returns the y position of the baseline of the first TEXTITEM.
    *
    * @return an <code>int</code> denoting the y position of the first baseline
    */
   public int getY() 
   {
      return m_iPosY;
   }
   
}