package imc.epresenter.filesdk;

import java.awt.Point;
import java.io.BufferedReader;
import java.io.IOException;
import java.util.HashMap;

import imc.epresenter.player.util.SGMLParser;
import imc.epresenter.player.whiteboard.ObjectQueue;

public class Question
{
   private Questionnaire m_Questionnaire = null;
   private String m_strScormId = null;
   private int m_iAchievablePoints = 0;
   private int m_iPageStartMs = Integer.MAX_VALUE;
   private int m_iPageEndMs = Integer.MIN_VALUE;
   private boolean m_bSuccessfullyAnswered = false;
   private boolean m_bShownTimer = false;
   private int m_iMaximumAttempts = 0;
   private int m_iTakenAttempts = 0;
   private int m_iMaximumTimeMs = 0;
   private long m_lAccessTimeMs = 0;
   private int m_iTimeElapsedMs = 0;
   private String m_strSuccessAction = null;
   private String m_strFailureAction = null;
   private HashMap m_mapFeedback = new HashMap(19);
   
   public Question(Questionnaire qq)
   {
      m_Questionnaire = qq;
   }
   
   public Questionnaire GetQuestionnaire()
   {
      return m_Questionnaire;
   }
   
   boolean ParseDefinition(String strLine, BufferedReader reader, ObjectQueue mother) throws IOException
   {
      boolean bSuccess = true;
      
      SGMLParser parser = new SGMLParser(strLine, true);
      
      m_strScormId = parser.getValue("id");
      
      String strPoints = parser.getValue("points");
      m_iAchievablePoints = ExtractInt(strPoints);
      
      if (m_iAchievablePoints < 0)
         return false;
      
      String strAttempts = parser.getValue("attempts");
      if (strAttempts != null)
         m_iMaximumAttempts = ExtractInt(strAttempts);
      
      String strMaxTime = parser.getValue("time");
      if (strMaxTime != null)
         m_iMaximumTimeMs = 1000 * ExtractInt(strMaxTime); // specified in seconds
      
      String strPage = parser.getValue("page");
      if (strPage != null)
      {
         Point ptTimes = new Point();
         bSuccess = ExtractTimes(strPage, ptTimes);
         
         if (bSuccess)
         {
            m_iPageStartMs = ptTimes.x;
            m_iPageEndMs = ptTimes.y;
         }
      }
      
      m_strSuccessAction = parser.getValue("onSuccess");
      m_strFailureAction = parser.getValue("onFailure");
      
      // these actions should remain null if they are undefined or empty
      if (m_strSuccessAction != null && m_strSuccessAction.length() == 0)
         m_strSuccessAction = null;
      if (m_strFailureAction != null && m_strFailureAction.length() == 0)
         m_strFailureAction = null;
     
      
      if (!strLine.endsWith("</QUESTION>"))
      {
         strLine = reader.readLine();
         while (!strLine.startsWith("</QUESTION>") && bSuccess)
         {
            if (strLine.startsWith("<feedback"))
               bSuccess = ParseFeedback(strLine, reader, mother);
            else // ignore
               System.out.println("Subtag of question not recognized: "+strLine);
         
            strLine = reader.readLine();
         }
      }
      
      return bSuccess;
   }
   
   public String GetScormId()
   {
      return m_strScormId;
   }
   
   public boolean ContainsMs(int iTimeMs)
   {
      return iTimeMs >= m_iPageStartMs && iTimeMs <= m_iPageEndMs;
   }
   
   public int GetStartMs()
   {
      return m_iPageStartMs;
   }
   
   public int GetEndMs()
   {
      return m_iPageEndMs;
   }
   
   public boolean AreAttemptsLeft()
   {
      if (m_iMaximumAttempts > 0)
         return m_iTakenAttempts < m_iMaximumAttempts;
      else // undefined
         return true;
   }
   
   public void UpdateAnswered(boolean bSuccess)
   {
      if (AreAttemptsLeft())
      {
         if (bSuccess)
            m_bSuccessfullyAnswered = true;
         else
            m_bSuccessfullyAnswered = false;
      }
      
      ++m_iTakenAttempts;
   }
   
   public boolean IsAnswerCorrect()
   {
      return m_bSuccessfullyAnswered;
   }
   
   public boolean IsUnanswered()
   {
      return m_iTakenAttempts == 0;
   }
   
   public int GetAnswerAttempts()
   {
      return m_iTakenAttempts;
   }
   
   /**
    * @returns the number of points for a correct answer of this question.
    */ 
   public int GetPoints()
   {
      return m_iAchievablePoints;
   }
   
   /**
    * @returns the number of attempts the user can use for this question.
    *          A value <= 0 means there is no limit.
    */
   public int GetMaximumAttempts()
   {
      return m_iMaximumAttempts;
   }
   
   /**
    * @returns the number of seconds the user has for answering this
    *          question. A value <= 0 means there is no limit.
    */
   public int GetMaximumTime()
   {
      return m_iMaximumTimeMs;
   }
   
   public String GetActionString(boolean bSuccess)
   {
      if (bSuccess)
         return m_strSuccessAction;
      else
         return m_strFailureAction;
   }
   
   public void SetActionString(String strAction)
   {
   		m_strSuccessAction = strAction;
   		m_strFailureAction = strAction;
   }
   
   public Feedback GetFeedback(int iType)
   {
      return GetFeedback(iType, false);
   }
   
   public Feedback GetFeedback(int iType, boolean bGetSuper)
   {
      Integer key = new Integer(iType);
      
      if (m_mapFeedback.containsKey(key))
      {
         Feedback f = (Feedback)m_mapFeedback.get(key);
         if (f.IsInherited() && bGetSuper)
            return m_Questionnaire.GetFeedback(iType); // TODO regard changed text
         else
            return f;
      }
      else
      {
         if (bGetSuper)
            return m_Questionnaire.GetFeedback(iType);
         else
            return null;
         // normally a feedback does not need to be defined for a question;
         // thus you should normally call this with "bGetSuper"
      }
   }
 

   public boolean IsTimeable()
   {
      return m_iMaximumTimeMs > 0;
   }
   
   public long QueryTimer()
   {
      if (IsTimeable())
      {
         if (IsAnswerCorrect())
            return 0;
         
         if (m_lAccessTimeMs == 0)
            return m_iMaximumTimeMs - m_iTimeElapsedMs; // timer not running
         else
            return m_iMaximumTimeMs - (System.currentTimeMillis()-m_lAccessTimeMs) - m_iTimeElapsedMs;
      }
      else
         return -1;
   }
   
   public long UpdateTimer()
   {
      if (IsTimeable())
      {
         if (IsAnswerCorrect())
            return 0;
             
         if (m_lAccessTimeMs == 0)
         {
            // (re-) start of timer
            m_lAccessTimeMs = System.currentTimeMillis();
         }
         
         long lTimeLeftMs = 
            m_iMaximumTimeMs - (System.currentTimeMillis()-m_lAccessTimeMs) - m_iTimeElapsedMs;
         
         if (lTimeLeftMs > 0)
            return lTimeLeftMs;
         else
            return -1;
      }
      else
         return -1;
   }
   
   public long StopTimer()
   {
      long lTimeLeftMs = QueryTimer();
      
      if (lTimeLeftMs > -1)
      {
         m_iTimeElapsedMs = (int)(m_iMaximumTimeMs - lTimeLeftMs);
         m_lAccessTimeMs = 0; // restart on next UpdateTimer()
      }
      // else nothing to be done: time is up or not timeable
      
      return lTimeLeftMs;
   }
   
   public boolean IsTimerMessageShown()
   {
      return m_bShownTimer;
   }
   
   public void InformShownTimerMessage()
   {
      m_bShownTimer = true;
   }
   
   
   private boolean ParseFeedback(String strLine, BufferedReader reader, ObjectQueue mother) throws IOException
   {
      Feedback f = new Feedback();
      boolean bSuccess = f.Parse(strLine, reader, mother);
      
      if (bSuccess)
         m_mapFeedback.put(new Integer(f.GetType()), f);
      
      return bSuccess;
   }
   
   private int ExtractInt(String strDef)
   {
      int iValue = 0;
      
      if (strDef != null)
      {
         try
         {
            iValue = Integer.parseInt(strDef);
         }
         catch (NumberFormatException exc)
         {
            // ignore: 0 will be returned
         }
      }
  
      return iValue;
   }
   
   public static boolean ExtractTimes(String strToken, Point ptRangeTarget)
   {
      if (strToken.indexOf(' ') > -1) // there is the description text at the start
         strToken = strToken.substring(strToken.indexOf(' ')+1);
      
      String strFrom = strToken;
      String strTo = strToken;
            
      int idx = strToken.indexOf('-');
      if (idx > -1)
      {
         strFrom = strToken.substring(0, idx);
         strTo = strToken.substring(idx+1);
      }
        
      int iStart = -1;
      int iEnd = -1;
      try
      {
         // strFrom and strTo can be the same string
         // this is a single moment (one ms) then
               
         iStart = Integer.parseInt(strFrom);
         iEnd = Integer.parseInt(strTo);
      }
      catch (NumberFormatException exc)
      {
         System.err.println("Time region not recognized: "+strToken);
               
         // TODO there is no integrity check (i.e. iEnd >= iStart)
         // apart from the number format check
      }
      
      if (iStart > -1 && iEnd > -1)
      {
         ptRangeTarget.x = iStart;
         ptRangeTarget.y = iEnd;
         
         return true;
      }
      else
         return false; // error occured
   }
}