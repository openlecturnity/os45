package imc.epresenter.filesdk;

import imc.epresenter.player.whiteboard.*;
import imc.epresenter.player.util.RangeGroup;

import imc.epresenter.filesdk.util.Localizer;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Vector;

import java.io.IOException;
import java.io.StreamTokenizer;
import java.io.StringReader;

import javax.swing.JOptionPane;

/**
 * <p>An instance of this class can be used for searching in Lecturnity
 * Presentations. In order to use a <tt>SearchEngine</tt>, you should
 * instanciate the class with the object and the event queue. If the
 * presentation has a {@link Metadata Metadata} object, pass that one,
 * too. Searching will work even without the meta data, but is nicer
 * with (slide titles will be extracted).</p>
 *
 * <p>If you want to permit multiple searching within a document, save
 * you create <tt>SearchEngine</tt> instance, and following search
 * requests will be faster, as the engine is initialized upon first
 * use (instanciation).</p>
 *
 * @see #search
 * @see SearchConstants
 * @see SearchResult
 */
public class SearchEngine implements SearchConstants
{
   private static String PAGE = "[!]";
   private static String ILLEGAL_SEARCH_TERM = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/epresenter/filesdk/SearchEngine_",
             "en");

         PAGE = l.getLocalized("PAGE");
         ILLEGAL_SEARCH_TERM = l.getLocalized("ILLEGAL_SEARCH_TERM");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "SearchEngine\n" +
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   public static int TITLE_HIT = 5;
   public static int KEYWORD_HIT = 3;
   public static int FULLTEXT_HIT = 1;

   private Metadata metadata_;
   private ObjectQueue objectQueue_;
   private EventQueue eventQueue_;

   private VisualComponent[] objects_;

   private Slide[] slides_;

   /**
    * Creates a new <tt>SearchEngine</tt> instance with the given
    * parameters.
    *
    * @param metadata the {@link Metadata Metadata} object for this
    * presentation, or <tt>null</tt>, if it does not exists (old-style
    * documents)
    * @param objectQueue an initialized {@link ObjectQueue
    * ObjectQueue} instance for this presentation
    * @param eventQueue an initialized {@link EventQueue
    * EventQueue} instance for this presentation
    * @param bStoreFullInfo tells the engine to store the complete text object
    *        during parsing, needed for full text (and style) extraction
    */
   public SearchEngine(Metadata metadata, ObjectQueue objectQueue,
                       EventQueue eventQueue, boolean bStoreFullInfo)
   {
      super();
      
      metadata_ = metadata;
//       metadata_ = null;
      objectQueue_ = objectQueue;
      objects_ = objectQueue.getVisualComponents();
      eventQueue_ = eventQueue;
      
      initEngine(bStoreFullInfo);

//       for (int i=0; i<slides_.length; i++)
//       {
//          System.out.println("Slide #" + i);
//          System.out.println("  Title: " + slides_[i].title);
//          System.out.println("  Content: " + slides_[i].content);
//       }
   }

   // initialized the search engine. this method is called from the
   // constructor automatically. it builds up an index of the slides
   // and the content of the slides.
   private void initEngine(boolean bStoreFullInfo)
   {
      if (metadata_ == null)
      {
         // old-style document, no LMD file

         // retrieve the page objects from the event queue
         EventQueue.PageObject[] pages = eventQueue_.getPageObjects();
         
         // retrieve all events in the event queue
         EventQueue.Event[] events = eventQueue_.getEvents();

         slides_ = new Slide[pages.length];

         int currentEventPos = 0;
         int currentTime = events[currentEventPos].getTimestamp();
         int nextTime;

         for (int i=0; i<pages.length; i++)
         {
            if (i < pages.length - 1)
               nextTime = pages[i+1].getTimeBegin();
            else
               nextTime = pages[i].getTimeEnd();

            boolean doOnce = (nextTime == currentTime);

//             System.out.println("checking from " + currentTime + " to " + nextTime + ".");

            slides_[i] = new Slide();
            Slide s = slides_[i];

            s.begin = pages[i].getTimeBegin();

            // use a standard title "Page <index>"
            s.title = PAGE + pages[i].getPageIndex();
            s.keywords = new String[] {};
            // s.content = "";

            IntSet ints = new IntSet();
            while (currentTime < nextTime || doOnce)
            {
               doOnce = false;

//                System.out.print(currentTime + ", ");
               RangeGroup rg = events[currentEventPos].content();
               rg.initIterator();
               
               while (rg.hasNextValue())
               {
                  int o = rg.nextValue();
                  if (!ints.contains(o))
                  {
                     ints.add(o);
                     if (objects_[o] instanceof Text)
                     {
                        Text text = (Text) objects_[o];
//                         s.content += text.getAllContent().toUpperCase() + " ";
                        s.content.append(text.getAllContent()); //.toUpperCase());
                        s.content.append(' ');
                        
                        if (bStoreFullInfo)
                           s.fullContent.add(text);
                     }
                  }
               } // end RangeGroup content while

               currentEventPos++;
               if (currentEventPos < events.length)
                  currentTime = events[currentEventPos].getTimestamp();
               else
                  currentTime = nextTime;
            } // end of slide event time while
//             System.out.println(".");
            s.content = new StringBuffer(s.content.toString().toUpperCase());
         }
      }
      else
      {
         // we have metadata information from the LMD (lecturnity
         // metadata document) file.
         slides_ = new Slide[metadata_.thumbnails.length];

         EventQueue.Event[] events = eventQueue_.getEvents();

//          System.out.println("events.length == " + events.length);
         
         int currentEventPos = 0;
         int currentTime = events[currentEventPos].getTimestamp();
         int nextTime;
         
         for (int i=0; i<metadata_.thumbnails.length; i++)
         {
            ThumbnailData td = metadata_.thumbnails[i];
            //System.out.println("Thumbnail Title: " + td.title);

            if (i < metadata_.thumbnails.length - 1)
               nextTime = (int) metadata_.thumbnails[i+1].beginTimestamp;
            else
               nextTime = (int) td.endTimestamp;

            boolean doOnce = (nextTime == currentTime);

//             System.out.println("td == " + td);

//             System.out.println("checking from " + currentTime + " to " + nextTime + ".");

            slides_[i] = new Slide();
            Slide s = slides_[i];

            s.begin = (int) td.beginTimestamp;
            s.title = td.title;
            s.keywords = td.keywords;
//             s.content = "";

//            System.out.println("events.length == " + events.length);

            IntSet ints = new IntSet();
            while ((currentTime < nextTime || doOnce) && (currentEventPos < events.length))
            {
//                System.out.print(currentTime + ", ");
               doOnce = false;
               RangeGroup rg = null;
               try
               {
                  rg = events[currentEventPos].content();
               }
               catch (Exception e)
               {
                  System.out.println("Illegal event position: " + currentEventPos);
                  System.out.println("  currentTime == " + currentTime);
                  System.out.println("  nextTime == " + nextTime);
                  System.out.println("  doOnce == " + doOnce);
                  rg = null;
               }

               // PZI: bugfix #4734
               // if there are two events with the same timestamp, skip the first one, because it belongs to the previous page
               boolean bDoubledTimestamp = currentEventPos+1 < events.length && events[currentEventPos].getTimestamp() == events[currentEventPos+1].getTimestamp();

               if (rg != null && !bDoubledTimestamp)
               {                  
                  rg.initIterator();
                  
                  while (rg.hasNextValue())
                  {
                     int o = rg.nextValue();
                     if (o < 0 || o >= objects_.length)
                     {
                        System.out.println("Illegal object index: " + o + ", at time " + currentTime + "ms.");
                        continue;
                     }
                     
                     if (!ints.contains(o))
                     {
                        ints.add(o);
                        if (objects_[o] instanceof Text)
                        {
                           Text text = (Text) objects_[o];
                           //                         s.content += text.getAllContent().toUpperCase() + " ";
                           s.content.append(text.getAllContent().toUpperCase());
                           s.content.append(' ');
                           
                           // PZI: used to enable accessing search matches by event time
                           text.setTimestamp(currentTime);
                           
                           if (bStoreFullInfo)
                              s.fullContent.add(text);
                     
                        }
                     }
                  } // end RangeGroup content while
               }

               currentEventPos++;
               if (currentEventPos < events.length)
                  currentTime = events[currentEventPos].getTimestamp();
               else
                  currentTime = nextTime;
            } // end of slide event time while
         } // end of metadata thumbnails array while
      } // end of if metata exists
   }

   /**
    * <p>This method performs a search on the presentation given in the
    * constructor. The <tt>keywords</tt> parameter is a string
    * containing the keywords to search for, separated by blanks. If
    * you want to search for sentences, enclose the words by
    * quotes, either &quot; or ' marks, e.g. &quot;lecturnity
    * player&quot; or 'market sales'.</p>
    *
    * <p>The <tt>mode</tt> parameter determines the search
    * mode. Supported modes are {@link #MODE_AND MODE_AND} and {@link
    * #MODE_OR}. In MODE_AND mode, all keywords must be matched within
    * a single slide, and in MODE_OR, any keyword has to be matched
    * within the slide.</p>
    *
    * <p>The parameter <tt>where</tt> determines where to look for the
    * requested keywords. It may be a sum of any of the constants
    * {@link #TITLES TITLES}, {@link #KEYWORDS KEYWORDS} and {@link
    * #FULLTEXT FULLTEXT}. If <tt>TITLES</tt> is set, the search will
    * look in the titles, if <tt>KEYWORDS</tt> is set, the search will
    * look in the keywords, and if <tt>FULLTEXT</tt> is set, the
    * search will look in all {@link #Text Text} objects contained in
    * the presentation. These constants can be used in arbitrary
    * combinations.</p>
    *
    * @param keywords the search string to look for, words separated
    * by blanks, quotes allowed.
    * @param mode the search mode
    * @param where where to look for the keywords
    */
   public SearchResult[] search(String keywords, int mode, int where)
      throws SearchException
   {
      // Vector of SearchResult
      Vector hitlist = new Vector();

      boolean keywordSearch = (where & KEYWORDS) > 0;
      boolean titleSearch = (where & TITLES) > 0;
      boolean fullTextSearch = (where & FULLTEXT) > 0;

      // reset hit counters
      for (int i=0; i<slides_.length; i++)
         slides_[i].hits = 0;

      String[] words = split(keywords);

//       for (int i=0; i<words.length; i++)
//          System.out.println("word #" + i + ": '" + words[i] + "'.");

//       System.out.println("keywordSearch: " + keywordSearch);
//       System.out.println("titleSearch: " + titleSearch);
//       System.out.println("fulltextSearch: " + fullTextSearch);
      
      for (int i=0; i<slides_.length; i++)
      {
         Slide slide = slides_[i];

//          System.out.println("Processing Slide '" + slide.title + "'.");
         
         // PZI: disable highlighting of previous search matches  
         for (int index = 0; index < slide.fullContent.size(); index++) {
            Text text = (Text) (slide.fullContent.get(index));
            text.setSearchWords(null);
         }

         boolean hit;

         boolean titleHit = false;
         boolean keywordHit = false;
         boolean fulltextHit = false;

         if (mode == MODE_AND)
            hit = true;
         else // mode == MODE_OR
            hit = false;

         for (int j=0; j<words.length; j++)
         {
            String word = words[j];

//             System.out.println("  Processing '" + word + "'.");

            boolean wordFound = false;

            if (metadata_ != null)
            {
               // we have a metadata structure to search in
               // only in this case we have titles and keywords containing
               // any information.
               if (titleSearch)
               {
                  if (slide.title.toUpperCase().indexOf(word) >= 0)
                  {
                     wordFound = true;
                     titleHit = true;
                     slide.hits += TITLE_HIT;
//                      System.out.println("hit in title on '" + word + "'.");
                  }
               }
               
               if (keywordSearch)
               {
                  for (int k=0; k<slide.keywords.length; k++)
                  {
                     if (slide.keywords[k].toUpperCase().indexOf(word) >= 0)
                     {
                        wordFound = true;
                        keywordHit = true;
                        slide.hits += KEYWORD_HIT;
//                         System.out.println("hit in keyword on '" + word + "'.");
                     }
                  }
               }
            }

            if (fullTextSearch)
            {
//                System.out.println("slide.content == \"" + slide.content + "\"");
               String content = slide.content.toString();

               int hitPos = content.indexOf(word);
//                System.out.println("    hitPos == " + hitPos);
               while (hitPos >= 0)
               {
                  wordFound = true;
                  fulltextHit = true;
                  slide.hits += FULLTEXT_HIT;
                  hitPos = content.indexOf(word, hitPos + 1);
//                   System.out.println("hit in fulltext on '" + word + "'.");
               }
            }

            if (mode == MODE_AND)
               hit = hit && wordFound;
            else
               hit = hit || wordFound;
         } // words for

         if (hit)
         {
            SearchResult sr = new SearchResult();
            sr.title = slide.title;
            sr.timeStamp = slide.begin;
            sr.where = 0;
            if (titleHit)
               sr.where += TITLES;
            if (keywordHit)
               sr.where += KEYWORDS;
            if (fulltextHit)
               sr.where += FULLTEXT;
            sr.relevancy = slide.hits;

            // PZI: enable highlighting
            // get matching text (lines or phrases)
            // note: not using slide.content because lines should be distinguished
            
            // clear previous matches
            sr.clearMatches();

            // TODO: maybe this should be static and global constants
            final String highlightOn = "<font color=red>";
            final int highlightOnLength = highlightOn.length();
            final String highlightOff = "</font>";
            final int highlightOffLength = highlightOff.length();

            if (fulltextHit)
               for (int index = 0; index < slide.fullContent.size(); index++) {
                  Text text = (Text) (slide.fullContent.get(index));
                  StringBuffer buffer = new StringBuffer(text.getAllContent());
                  boolean found = false;

                  // String content = text.getAllContent();
                  // for (String word : words) {
                  // if(content.toUpperCase().indexOf(word) >= 0) {
                  // // System.out.println("  "+index+": "+content);
                  // sr.matches.add("<html><font color=red>"+content+"</font><html>");
                  // break;
                  // }
                  // }

                  for (String word : words) {
                     // quickfix hack to avoid replacement in html tags
                     if (found
                           && (highlightOn.toUpperCase().indexOf(word) >= 0 || highlightOff
                                 .toUpperCase().indexOf(word) >= 0))
                        continue;

                     int fromIndex = buffer.toString().toUpperCase().indexOf(word);
                     while (fromIndex >= 0) {
                        // TODO: cut string to fit into cell - but code in comment is not ok
//                        int maxLength = 30;
//
//                        // limit result length
//                        if (!found && buffer.length() > maxLength) {
//                           // cut at beginning
//                           boolean beginCut = false;
//                           if (fromIndex + word.length() + 5 > maxLength) {
//                              buffer.delete(0, fromIndex + word.length() - maxLength + 3).insert(0,
//                                    "...");
//                              fromIndex = buffer.toString().toUpperCase().indexOf(word);
//                              beginCut = true;
//                           }
//
//                           // cut at end
//                           boolean endCut = false;
//                           if (buffer.length() > maxLength) {
//                              buffer.delete(maxLength, buffer.length()).append("...");
//                              fromIndex = buffer.toString().toUpperCase().indexOf(word);
//                              endCut = true;
//                           }
//                           // JLabel label = new JLabel();
//                           // FontMetrics fontMetrics = label.getFontMetrics(label.getFont());
//                           // System.out.println(fontMetrics.stringWidth(buffer.toString())+" - "+buffer);
//                           // int maxWidth = 80;
//                           // // limit fine tuning
//                           // while(fontMetrics.stringWidth(buffer.toString()) > maxWidth) {
//                           // buffer.delete(buffer.length() - (endCut ? 4 : 1), buffer.length()).append("...");
//                           // System.out.println("< "+buffer);
//                           // endCut = true;
//                           // }
//                           fromIndex = buffer.toString().toUpperCase().indexOf(word);
//
//                           if (fromIndex < 0) {
//                              // cut too much: set searched word as text
//                              buffer.replace(0, buffer.length(), word);
//                              fromIndex = 0;
//                           }
//                        }

                        buffer.insert(fromIndex, highlightOn);
                        fromIndex += highlightOnLength + word.length();
                        buffer.insert(fromIndex, highlightOff);
                        fromIndex += highlightOffLength;
                        fromIndex = buffer.indexOf(word, fromIndex);
                        found = true;
                     }

                  }
                  if (found) {
                     // set words to highlight matches on Whiteboard
                     text.setSearchWords(words);
                     // System.out.println("FOUND: "+buffer);
                     sr.addMatch("<html>" + buffer.toString() + "</html>", text.getTimestamp());
                  }               

            }
            // end PZI
            
            hitlist.addElement(sr);
         }
      } // slides_ for
      
      SearchResult[] srs = new SearchResult[hitlist.size()];
      hitlist.copyInto(srs);

      normalizeRelevancies(srs);

//       for (int i=0; i<srs.length; i++)
//       {
//          System.out.println("Hit #" + i);
//          System.out.println("  Title: " + srs[i].title);
//          System.out.println("  Begin: " + srs[i].timeStamp);
//          System.out.println("  Relevancy: " + srs[i].relevancy);
//          System.out.println("  Where: " + srs[i].where);
//          for (String string : srs[i].matches) {
//            System.out.println("    "+string);
//        }
//       }

      return srs;
   }

   private String[] split(String words)
      throws SearchException
   {
		StreamTokenizer tokenizer =
         new StreamTokenizer(new StringReader(words));

      tokenizer.resetSyntax();
      tokenizer.whitespaceChars(0, 32);
		tokenizer.wordChars( 33, 255 );
		tokenizer.quoteChar( '"' );
		tokenizer.quoteChar( '\'' );
 
		Vector tmp = new Vector();
		
		int ttype = tokenizer.ttype;
		while ( ttype != StreamTokenizer.TT_EOF ) 
      {
         try
         {
            ttype = tokenizer.nextToken();
         }
         catch (IOException e)
         {
            // this should not occur as we read from a String object,
            // but if it does, we will propagate a SearchException
            // instead of an IOException
            throw new SearchException(e.getMessage());
         }

         switch (ttype) 
         {
         case '\'':
         case '"':
         case StreamTokenizer.TT_WORD:
            tmp.addElement(tokenizer.sval.toUpperCase());
            break;
            
         case StreamTokenizer.TT_EOF:
            break;
            
         default:
            throw new SearchException(ILLEGAL_SEARCH_TERM);
         }
		}
		
		String[] tmp2 = new String[tmp.size()];
      tmp.copyInto(tmp2);
      
      return tmp2;
   }

   private void normalizeRelevancies(SearchResult[] searchResults)
   {
      int max = 0;
      for (int i=0; i<searchResults.length; i++)
      {
         if (searchResults[i].relevancy > max)
            max = searchResults[i].relevancy;
      }

      double scaleFactor = 100.0/((double) max);
      
      for (int i=0; i<searchResults.length; i++)
      {
         int r = searchResults[i].relevancy;
         searchResults[i].relevancy = (int) (scaleFactor*r);
      }
   }
   
   
   public Slide[] getSlideInformation()
   {
      return slides_;
   }
   

   public class Slide
   {
      public Slide()
      {
         content = new StringBuffer(1024);
         fullContent = new ArrayList();
      }

      public int begin;
      public String title;
      public String[] keywords;
      public StringBuffer content;
      public ArrayList fullContent;
      public int hits;
   }

   private class IntSet
   {
      private HashSet intSet_;

      public IntSet()
      {
         intSet_ = new HashSet(100);
      }

      public void add(int n)
      {
         intSet_.add(new Integer(n));
      }

      public boolean contains(int n)
      {
         return intSet_.contains(new Integer(n));
      }

      public boolean remove(int n)
      {
         return intSet_.remove(new Integer(n));
      }

      public void clear()
      {
         intSet_.clear();
      }
   }
}
