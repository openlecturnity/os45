package imc.lecturnity.converter;

import imc.epresenter.player.whiteboard.Text;

public class ThumbnailEntry
{
   public long       lBeginTimestamp;
   public long       lEndTimestamp;
   public String     strImageName;
   public String     strTitle;      // title of chapter
   public String     strCaption;    // title of slide
   public String     strClipFileName;
   
   // PZI:
   public Text[]   aSearchableText;
   
  /**
    * Empty Constructor
    */
   public ThumbnailEntry()
   {
      lBeginTimestamp   = 0;
      lEndTimestamp     = 0;
      strImageName      = "";
      strTitle          = "";
      strCaption        = "";
      strClipFileName   = "";
   }
   
   /**
    * Constructor
    */
   public ThumbnailEntry(long    beginTimestamp,
                         long    endTimestamp,
                         String  imageName, 
                         String  title, 
                         String  caption, 
                         String  clipFileName) 
   {
      this.lBeginTimestamp    = beginTimestamp;
      this.lEndTimestamp      = endTimestamp;
      this.strImageName       = new String(imageName);
      this.strTitle           = new String(title);
      this.strCaption         = new String(caption);
      this.strClipFileName    = new String(clipFileName);
   }

   /**
    * Copy Constructor
    */
   public ThumbnailEntry(ThumbnailEntry te)
   {
      this.lBeginTimestamp    = te.lBeginTimestamp;
      this.lEndTimestamp      = te.lEndTimestamp;
      this.strImageName       = new String(te.strImageName);
      this.strTitle           = new String(te.strTitle);
      this.strCaption         = new String(te.strCaption);
      this.strClipFileName    = new String(te.strClipFileName);

      // PZI: maybe each string should be copied to a new array; 
      this.aSearchableText    = te.aSearchableText;
   }
}
