package imc.epresenter.filesdk;

import java.util.ArrayList;

/**
 * Instances of this class are returned by the {@link SearchEngine
 * SearchEngine} class. The represent slides in which the desired
 * keywords were found.
 */
public class SearchResult implements SearchConstants
{
   /**
    * The title of the slide where a hit while searching occurred
    */
   public String title;

   /**
    * The begin time stamp of the slide where a hit was found
    */
   public long timeStamp;

   /**
    * This field indicates where the looked for keywords were
    * found. To retrieve this information, you will need to AND the
    * result of this method with the constants from the {@link
    * SearchConstants SearchConstants} interface.
    *
    * @see SearchConstants
    */
   public int where;

   /**
    * The relevance of this hit. This value is normalized between 0
    * and 100 and may be considered as a hit match percentage or so.
    */
   public int relevancy;
   
   // PZI: extend to enable listing of search results and event-based access
   private ArrayList<MatchWithTimestamp> m_aMatches = new ArrayList<MatchWithTimestamp>();
   
   public void clearMatches() {
      m_aMatches.clear();
   }
   
   public void addMatch(String text, int timestamp) {
       m_aMatches.add(new MatchWithTimestamp(text, timestamp));
   }
   
   public int getNumberOfMatches() {
       if(m_aMatches != null)
           return m_aMatches.size();
       else return 0;
   }
   
   public String getMatchText(int index) {
      if(index < m_aMatches.size())
         return m_aMatches.get(index).text;
      else 
         return "";
   }
   
   public int getMatchTimestamp(int index) {
      if(index < m_aMatches.size())
         return m_aMatches.get(index).timestamp;
      else 
         return (int)timeStamp;
   }
   
   class MatchWithTimestamp {      
      String text;
      int timestamp;
      MatchWithTimestamp(String text, int timestamp) {
         this.text = text;
         this.timestamp = timestamp;
      }
   }
}
