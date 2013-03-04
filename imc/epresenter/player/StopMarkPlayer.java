package imc.epresenter.player;

import java.awt.Component;
import java.util.ArrayList;
import java.util.Arrays;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.player.*;
import imc.epresenter.player.util.SGMLParser;

public class StopMarkPlayer implements SRStreamPlayer
{
	private Coordinator m_Coordinator;
	private boolean     m_bStarted = false;
   private int[]       m_aiStopPositions;
   private int         m_iLastTimeMs = 0;

   
   public StopMarkPlayer(int[] aiStopTimes)
   {
      m_aiStopPositions = aiStopTimes;
   }

	public void init(FileResources resources, String[] args, Coordinator c)
   {
		m_Coordinator = c;
   }
	
	public void enableVisualComponents(boolean bEnable)
   {
      // not implemented here
   }

	public Component[] getVisualComponents()
   {
		return null;
	}

	public String getTitle()
   {
		return "StopMarkPlayer";
	}	

	public String getDescription()
   {
		return "Stops at stop mark positions.";
	}
   	
	public void pause(EventInfo info)
   {
      // the mode pause signifies (user) scrolling
	
      m_bStarted = false;
   }
   
	public void start(EventInfo info)
   {
      m_bStarted = true;
   }
   
	public void stop(EventInfo info)
   {
      m_bStarted = false;
   }
	
	public synchronized void setMediaTime(int iTimeMs, EventInfo info)
   {
      if (m_bStarted && info != null)
      {
         // it is a regular time event during normal replay
         
         if (info.source == EventInfo.MASTER && iTimeMs > m_iLastTimeMs) // || info.source == EventInfo.HELPER)
         {
            // Note: do not allow HELPER above: this leads to untrackable wrong behavior
            // with mouse over jumps during replay (often stops at stopmark before)
            
            int idx = Arrays.binarySearch(m_aiStopPositions, iTimeMs);
         
            if (idx >= 0)
            {
               // direct match for this replay time (seldom but possible)
               boolean bSpecial = true;
               m_Coordinator.requestStop(bSpecial, this);
               
               // nevertheless make sure the time is right
               m_Coordinator.requestTime(iTimeMs, this);
            }
            else
            {
               idx = Math.abs(idx) - 1;
               // (insert) position with a value greater than iTimeMs
               
               if (idx > 0)
               {
                  if (iTimeMs > m_aiStopPositions[idx-1] && m_iLastTimeMs < m_aiStopPositions[idx-1])
                  {
                     boolean bSpecial = true;
                     m_Coordinator.requestStop(bSpecial, this);
                     m_Coordinator.requestTime(m_aiStopPositions[idx-1], this);
                  }
               }
               // else interesting only after the first time
            }
         }
         // else ignore event (backwards or same time again)
         
		}
      
      m_iLastTimeMs = iTimeMs;
	}
   
   public boolean isExactStopAt(int iTimeMs)
   {
      int idx = Arrays.binarySearch(m_aiStopPositions, iTimeMs);
      
      return idx >= 0;
   }

   public void setStartOffset(int millis)
   {
      throw new UnsupportedOperationException("Currently not implemented for this component.");
   }

	public int getMediaTime()
   { 
      throw new UnsupportedOperationException("Currently not implemented for this component.");
   }
   
	public int getDuration()
   {
      throw new UnsupportedOperationException("Currently not implemented for this component.");
   }
   
	public void setDuration(int millis)
   {
   }
   
	public void setInfo(String key, String value)
   {
   }

	public void close(EventInfo info)
   { 
	}
}