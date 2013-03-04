package imc.epresenter.filesdk;

/**
 * This class contains the data about a thumbnail/page object
 *
 */
public class ThumbnailData
{
   /**
    * An exclusive identifier of this thumbnail/page
    */
   public int number;

   /**
    * An exclusive identifier of the chapter of this page 
    */
   public int chapterNumber;

   /**
    * The title of the page represented by this thumbnail
    */
   public String title;

   /**
    * The names of the chapters this page is contained within. This
    * field at least contains the name of the document, which always
    * is the top level chapter.
    */
   public String[] chapterTitles;

   /**
    * This field is <i>true</i> iff a title should be displayed at all.
    */
   public boolean displayTitle;

   /**
    * If {@link #displayTitle displayTitle} is <i>true</i>, the
    * chapter titles beginning with this offset in the {@link
    * #chapterTitle chapterTitle} variable should be displayed.
    */
   public int titleOffset;

   /**
    * The name of the resource containing the (png) bitmap of this
    * thumbnail
    */
   public String thumbnailResource;

   /**
    * The level in the chapter overview of this page, 0 means
    * top-level, i.e. this page does not belong to a chapter.
    */
   public int chapterLevel;

   /**
    * The start time stamp when this page should be displayed
    */
   public long beginTimestamp;

   /**
    * The time stamp of the next thumbnail, i.e. the end time+1 of
    * this page 
    */
   public long endTimestamp;

   /**
    * This member is <i>true</i> if this thumbnail represents a video
    * clip. If it is <i>false</i>, a normal page is represented.
    */
   public boolean isVideoClip;

   /**
    * An array containing metadata keywords for this
    * thumbnail/page. If there are no keywords for this page object,
    * this variable will be an array of length 0.
    */
   public String[] keywords;
   
   /**
    * type of page may be empty for normal pages or specified as follows:
    *   "analysis"  : page inserted by structuring screenrecording clip
    *   "click"     : page inserted as result of a mouse click
    */
   public String pageType;

   public String toString()
   {
      return "ThumbnailData [" + beginTimestamp + ", " + endTimestamp + "): title=" + title;
   }
}
