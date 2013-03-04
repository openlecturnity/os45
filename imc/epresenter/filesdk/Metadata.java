package imc.epresenter.filesdk;

/**
 * Class which contains metadata of a presentation.
 *
 * @see FileResources#getMetadata(String)
 */
public class Metadata
{
   /**
    * The title of this presentation.
    */
   public String title;

   /**
    * The author of this presentation.
    */
   public String author;

   /**
    * The creator of this presentation.
    */
   public String creator;

   /**
    * The recording date of this presentation
    */
   public String recordDate;

   /**
    * The length of the recording, as a String object
    * of the form minutes:seconds, i.e. &quot;52:31&quot;.
    * This String .equals &quot;&quot; if it is not defined.
    */
   public String recordLength;

   /**
    * An array of keywords that are defined for this presentation. If
    * there are no keywords for this presentation, this field is an
    * array of length 0.
    */
   public String[] keywords;

   /**
    * An array of thumbnail data for this presentation. The
    * <tt>ThumbnailData</tt> instances also contain any possible
    * keywords for the presentation.
    *
    * @see ThumbnailData 
    */
   public ThumbnailData[] thumbnails;
   // removed thumbnails of click pages
   public ThumbnailData[] removedThumbnails;

   
   public String toString()
   {
      String tmp = "";
      
      tmp += "Title  : " + title + '\n';
      tmp += "Author : " + author + '\n';
      tmp += "Creator: " + creator + '\n';
      tmp += "Date   : " + recordDate + '\n';
      tmp += "Length : " + recordLength + '\n';

      for (int i=0; i<keywords.length; i++)
         tmp += "  Keyword #" + i + ": " + keywords[i] + '\n';

      for (int i=0; i<thumbnails.length; i++)
      {
         tmp += ("  Thumbnail at " + thumbnails[i].beginTimestamp + ", " +
                 thumbnails[i].thumbnailResource) + '\n';
         for (int j=0; j<thumbnails[i].keywords.length; j++)
            tmp += "     Keyword #" + j + ": " + thumbnails[i].keywords[j] + '\n';
      }

      return tmp;
   }
}
