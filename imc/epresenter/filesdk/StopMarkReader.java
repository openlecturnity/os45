package imc.epresenter.filesdk;

import java.util.ArrayList;
import java.util.Arrays;

import imc.epresenter.player.util.SGMLParser;

public class StopMarkReader
{
   private static ArrayList s_liStopTimes = new ArrayList(40);
   
   public static int parseMark(String strLine)
   {
      int iTimeMs = Integer.MIN_VALUE;
         
      SGMLParser parser = new SGMLParser(strLine);
      
      String strType = parser.getValue("type");
      if (strType != null && strType.equals("stop"))
      {
         try
         {
            iTimeMs = Integer.parseInt(parser.getValue("time"));
         }
         catch (NumberFormatException exc)
         {
            // ignore (see below)
         }
         
         if (iTimeMs > 0)
         {
            s_liStopTimes.add(new Integer(iTimeMs));
         }
         // else
         // stop time "0" is rather dull and needn't be regarded
         // AND "0" could also mean the time was not specified (an error)
            
         
      }
      
      return iTimeMs;
   }
   
   public static boolean stopsPresent()
   {
      return s_liStopTimes.size() > 0;
   }

   public static int[] getStopTimes(boolean bClear)
   {
      int[] aiStopPositions = new int[s_liStopTimes.size()];
      for (int i=0; i<aiStopPositions.length; ++i)
         aiStopPositions[i] = ((Integer)s_liStopTimes.get(i)).intValue();
         
      if (bClear)
         s_liStopTimes.clear();
      Arrays.sort(aiStopPositions);
         
      // check for double entries (not allowed)
      for (int i=1; i<aiStopPositions.length; ++i)
         if (aiStopPositions[i] == aiStopPositions[i-1])
            throw new IllegalArgumentException("Double entries not allowed in stop marks.");
      
      return aiStopPositions;
   }
}
