package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.io.*;
import java.util.*;
import java.net.URL;

import imc.epresenter.filesdk.Metadata;
import imc.epresenter.player.util.*;

/** (Reads and) stores for every line in the evq file an Event object. */
public class EventQueue {
	private int[] lookup;
	private Event[] content;
	//private URL eventURL;
	private InputStream inputStream;
	private ObjectQueue objectQueue;
	private Event lastPaintedEvent_;
	private int lastArrayIndex = 0;
	private int[] timeOfPageChange_;
	private int[] timeOfNewPage_;
	private int numOfPageChanges_;
	private PageObject[] pageObject_;

   private Event lastGrabbedEvent_;
   private int lastGrabbedTime_          = -1;
   
   public static Metadata metadata = null;
   
   /*
   private Event lastMouseCheckEvent_;
   private boolean lastMouseCheckAnswer_ = false;
   private int lastCheckedMouseX_        = -1;
   private int lastCheckedMouseY_        = -1;
   */
   
	public EventQueue(InputStream in, ObjectQueue oq) {
		objectQueue = oq;
		inputStream = in;
		lastPaintedEvent_ = new Event(-1,-1, 0);
	}
	
   public void startParsing(ProgressListener listener) throws IOException 
   {
      startParsing(listener, false);
   }
      
	public void startParsing(ProgressListener listener, boolean bCorrectRectangles) throws IOException 
   {
		//System.gc();
		//long mem1 = Runtime.getRuntime().totalMemory()-Runtime.getRuntime().freeMemory();
		parse(listener, bCorrectRectangles);
		//System.gc();
		//long mem2 = Runtime.getRuntime().totalMemory()-Runtime.getRuntime().freeMemory();
		//System.out.println("EQ: EventQueue occupies "+((mem2-mem1)/(float)1000)+" KB");
		
	}


	// Read in the events and collect information about paging events
	private void parse(ProgressListener listener, boolean bCorrectRectangles) throws IOException {
		//BufferedInputStream bin = new BufferedInputStream(eventURL.openStream());
		CounterInputStream cis = new CounterInputStream(inputStream);
		
		ArrayList events = new ArrayList(2000);
		ArrayList timeOfNewPaging = new ArrayList(40);
		ArrayList pageIndex = new ArrayList(40);
		ArrayList timeOfPaging = new ArrayList(40);
		ArrayList timeBeforePaging = new ArrayList(40);
		HashMap shownPages = new HashMap();
		int lastTime = -20;
		int nPage, nBegin, nEnd;
		Event lastEvent = null; // Save the last event of one
		                        // to high precision run!
      int lastPage = -1;
		                        
      timeOfPaging.add(new Integer(0));
      
      int iPageToBeModified = -1;
      
      int iLastPage = -1;
      int iLastBackgroundIdx = -1;
		while(true) {
         // not-trimmed range groups in this event: maybe there is interaction to add to it
			Event e = nextEvent(cis, false, iLastPage, iLastBackgroundIdx); 
			if (e == null) break;
			
			// remove pages according to LMD
			boolean bNewPage = iPageToBeModified<0 ? e.getPage() != lastPage : e.getPage() != iPageToBeModified;
			if ( bNewPage && metadata!=null ) {
			    // next page found
			    iPageToBeModified = -1;
			    for ( int i=0; iPageToBeModified<0 && i<metadata.removedThumbnails.length; i++)
			        if ( e.getTimestamp() == metadata.removedThumbnails[i].beginTimestamp)
                        iPageToBeModified = e.getPage();
			}
			if ( e.getPage() == iPageToBeModified )
			    // remove page, i.e. use same page number as previous page
			    e.page = lastPage;
			
         
         // Bugfix #3860 and #3848 (see below)
         if (bCorrectRectangles && e.getPage() != iLastPage)
         {
            iLastPage = e.getPage();
            RangeGroup rg = e.content();
            if (rg != null)
            {
               int iFirstIdx = rg.firstValue();
               if (iFirstIdx > -1 && objectQueue.IsFilledRectangle(iFirstIdx))
                  iLastBackgroundIdx = iFirstIdx;
            }
         }
              			
         // Note: do not try to drop events that might appear to 
         // be not needed: they likely will be needed some way or the other.
         // In olden days we used to drop events here that were very close together
         // and that caused more harm than use.
         
         if (lastEvent != null) {
            events.add(lastEvent);
					
            if (!shownPages.containsKey(new Integer(lastEvent.page))) {
               shownPages.put(new Integer(lastEvent.page),null);
               timeOfNewPaging.add(new Integer(lastEvent.time));
            }
					
            if (lastEvent.page != e.page) {
               pageIndex.add(new Integer(lastEvent.page));
               timeBeforePaging.add(new Integer(lastEvent.time));
               timeOfPaging.add(new Integer(e.time));
            }
         }
				
         lastTime = e.time;
         lastEvent = e;
         lastPage = e.page;
			
         if (listener != null)
            listener.progressAchieved(cis.readBytes());
		}

		events.add(lastEvent);
		
		if (!shownPages.containsKey(new Integer(lastEvent.page))) {
			shownPages.put(new Integer(lastEvent.page),null);
			timeOfNewPaging.add(new Integer(lastEvent.time));
		}
		
		pageIndex.add(new Integer(lastEvent.page));
		timeBeforePaging.add(new Integer(lastEvent.time));

		lookup = new int[events.size()+1];
		content = new Event[events.size()+1];
		events.toArray(content);
		for (int i=0; i<events.size(); i++) 
      {
         content[i].trim(); // event (and range group) will be finished here
         
		   lookup[i] = content[i].time;
      }
		   
		// stop elements for binarySearch to work properly
		content[events.size()] = content[events.size()-1];
		lookup[events.size()] = Integer.MAX_VALUE;
         
		timeOfNewPage_  = new int[timeOfNewPaging.size()];
		for (int i=0; i<timeOfNewPage_.length; i++) {
			timeOfNewPage_[i] = ((Integer)timeOfNewPaging.get(i)).intValue();
		}
		
		timeOfPageChange_  = new int[timeOfPaging.size()];
		for (int i=0; i<timeOfPageChange_.length; i++) {
			timeOfPageChange_[i] = ((Integer)timeOfPaging.get(i)).intValue();
		}
		
		// Create the Page Objects
		numOfPageChanges_ = pageIndex.size();
		pageObject_ = new PageObject[numOfPageChanges_];
		
		for (int i=0; i<numOfPageChanges_; i++)
		{
		   nPage  = ((Integer)pageIndex.get(i)).intValue();
		   nBegin = ((Integer)timeOfPaging.get(i)).intValue();
		   nEnd   = ((Integer)timeBeforePaging.get(i)).intValue();
		   
		   pageObject_[i] = new PageObject(nPage, nBegin, nEnd);
		   
		}
	}
   
	/** 
    * Returns the difference area between the specified time 
    * and the last painted event time.
    * Internally this is used to determine the repaint area for each
    * timestamp during replay or scrolling.
    */
   public Rectangle getClip(int time) {
		return getClip(time, 1.0);
	}
	
	public Rectangle getClip(int time, double scale) {
		Event now = grabEvent(time);
		
		if (now.time != lastPaintedEvent_.time) {
         RangeGroup diff = lastPaintedEvent_.compare(now);
         Rectangle r = objectQueue.getClip(diff, scale);
         return r;
		} else {
			return new Rectangle(0,0,0,0);
		}
	}
	
	//
	// Untersucht wird alles, aber nur das was den Clipping-Bereich 
	// schneidet wird (neu) gezeichnet.
	//
	// All stuff is analyzed - but only these parts are (re-)drawn which subtend the clipping area
	public void paint(Graphics g, int iTimeMs, double dScale) 
   {
      paint(g, iTimeMs, dScale, false, false);
	}
   
   public void paint(Graphics g, int iTimeMs, double dScale, boolean bIgnorePointer) 
   {
      paint(g, iTimeMs, dScale, bIgnorePointer, false);
	}
   
   public void paint(Graphics g, int iTimeMs, double dScale, boolean bIgnorePointer, boolean bIncludeInteractives) 
   {
		Event now = grabEvent(iTimeMs);
      
      objectQueue.paint(g, now.content(), dScale, bIgnorePointer);
      
      if (bIncludeInteractives)
         objectQueue.PaintInteractives(g, iTimeMs, dScale);
      
      lastPaintedEvent_ = now;
	}
	
   public Color getFirstBackgroundColor()
   {
      return VisualComponent.createBackroundColor(content[0].color);
   }



   /**
    * Get the timestamps, when a   n e w   page appears. 
    */
	public int[] pageChangeTimes() {
		return timeOfNewPage_;
	}
	
   /**
    * Get the timestamps on   e v e r y   page change. 
    */
	public int[] getAllPageChangeTimes() {
		return timeOfPageChange_;
	}
	
   /**
    * Get the PageObjects with Begin/End time and page index. 
    */
	public PageObject[] getPageObjects() {
		return pageObject_;
	}
	
   /**
    * Get the Event Objects. 
    */
	public Event[] getEvents() {
		return content;
	}
   
	int getDuration() {
		Event e = grabEvent(Integer.MAX_VALUE);
      return e.time;
   }
	
   private Event nextEvent(InputStream input, boolean bFinish, int iLastPage, int iLastRectangleIdx) throws IOException {
		// Suche alle relevanten Daten fuer ein Event
		int time = nextNumber(input);
		if (time == -1) return null;
		int page = nextNumber(input);
		int color = nextNumber(input); // old background color
		int objectCount = nextNumber(input);
		
		Event e = new Event(time, page, color);
		if (objectCount > 0) {

			for (int i=0; i<objectCount; i++)
			{
            int n = nextNumber(input);
            
            // Workaround for Bug #3860 (Bugfix #3848):
            // Replace superfluous background rectangles with the right (earlier) occurance.
            //
            
            if (i == 0)
            {
               if (iLastRectangleIdx > -1 && iLastPage > -1)
               {
                  if (page == iLastPage && n != iLastRectangleIdx)
                  {
                     if (objectQueue.IsSameFilledRectangle(iLastRectangleIdx, n))
                     {
                        n = iLastRectangleIdx;
                     }
                  }
               }
            }
            
            
            e.lastNumber = n;
            e.add(n, 1);
			}
      }
		
      if (bFinish)
         e.trim();
		
		return e;
	}
	
	private int nextNumber(InputStream input) throws IOException {
		int num = 0;
		int in = input.read();
		while(in < 48 || in > 57) { // Suche nach Anfang einer Zahl
			if (in < 0) return -1;
			in = input.read();
		}
		while(in > 47 && in < 58) { // Lese Zahl bis zum Ende ein
			num = num*10 + (in-48);
			in = input.read();
		}
		return num;
	}
	
	
	private Event grabEvent(int time) 
   {
      if (time == lastGrabbedTime_)
      {
         return lastGrabbedEvent_;
      }
      else
      {
         int index = Arrays.binarySearch(lookup, time);
         if (index < 0) index = index*(-1)-2;
         if (index == -1) index = 0;

         if (index < content.length - 1 && content[index].time == content[index + 1].time)
            index++; // two events with the same time, take the latter one
         
         lastGrabbedEvent_ = content[index];
         lastGrabbedTime_ = time;


         return lastGrabbedEvent_;
      }
   }
	
	/**
    * The events of the event queue.
    *
    * <p>Used by <tt>SearchEngine</tt>, too!</p>
    */
	public class Event
   {
		int time;
		int page;
		int lastNumber;
      int color;
      
      private RangeGroup m_IndexRanges;
		
		Event(int t, int p, int c)
      {
			time = t; 
			page = p;
			lastNumber = 0;
         color = c;
         m_IndexRanges = new RangeGroup();
         //m_bContainsInteraction = false;
		}
      
      Event Copy()
      {
         Event evtCopy = new Event(time, page, color);
         
         m_IndexRanges.initIterator();
         while (m_IndexRanges.hasNextValue())
         {
            evtCopy.m_IndexRanges.add(m_IndexRanges.nextValue(), 1);
         }
                 
         return evtCopy;
      }
		
		String summary()
      {
			return m_IndexRanges.summary();
		}
      
      public String toString()
      {
         return "Event t"+time+" p"+page+": "+m_IndexRanges;
      }
		
		void add(int value, int length)
      {
			m_IndexRanges.add(value, length);
		}
		
		void trim()
      {
			m_IndexRanges.trim();
		}
		
		public RangeGroup content()
      {
			return m_IndexRanges;
		}
	
		RangeGroup compare(Event other)
      {
			return m_IndexRanges.diff(other.m_IndexRanges);
		}

      public int getTimestamp()
      {
         return time;
      }

      public int getPage()
      {
         return page;
      }

      public int getLastNumber()
      {
         return lastNumber;
      }
	}

   //**************************************************
   
   public class PageObject
   {
      private int nPageIndex_;
      private int nTimeBegin_;
      private int nTimeEnd_;
      
      PageObject(int pageIndex, int timeBegin, int timeEnd)
      {
         nPageIndex_    = pageIndex;
         nTimeBegin_    = timeBegin;
         nTimeEnd_      = timeEnd;
      }

      public int getPageIndex()
      {
         return nPageIndex_;
      }
      
      public int getTimeBegin()
      {
         return nTimeBegin_;
      }
      
      public int getTimeEnd()
      {
         return nTimeEnd_;
      }
      
      public void setPageIndex(int pageIndex)
      {
         nPageIndex_ = pageIndex;
      }
      
      public void setTimeBegin(int timeBegin)
      {
         nTimeBegin_ = timeBegin;
      }
      
      public void setTimeEnd(int timeEnd)
      {
         nTimeEnd_ = timeEnd;
      }
      
   }
   
}
