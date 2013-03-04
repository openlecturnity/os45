package imc.epresenter.filesdk;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.player.util.SGMLParser;
import imc.epresenter.player.whiteboard.ObjectQueue;


//
// Note: All the information belonging to a questionnaire or question
// is stored in two locations here and in ObjectQueue:
// There are a number of objects in the ObjectQueue: InteractionAreas, 
// moveable objects (VisualComponent), changeable texts (Text) and TargetPoints.
// All of these are only coupled with the questions here with their visibility
// or activity time.
//

public class Questionnaire
{
   private static ArrayList s_liQuestionnaires = new ArrayList(5);
   
   public static Questionnaire ParseQuestionnaire(String strLine, BufferedReader reader, ObjectQueue mother)
      throws IOException
   {
      Questionnaire qq = new Questionnaire();
      boolean bSuccess = qq.ParseDefinition(strLine, reader, mother);
      
      if (bSuccess)
      {
         s_liQuestionnaires.add(qq);
         return qq;
      }
      else
         return null;
   }
   
   public static Questionnaire[] GetAllQuestionnaires(boolean bClear)
   {
      if (s_liQuestionnaires.size() <= 0)
         return null;
      
      Questionnaire[] aQuestions = new Questionnaire[s_liQuestionnaires.size()];
      s_liQuestionnaires.toArray(aQuestions);
      
      if (bClear)
         s_liQuestionnaires.clear();
      
      return aQuestions;
   }
   
   ////////////////////////// non-static ////////////////////////////////////
   
   private int m_iPassPoints = 0;
   private boolean m_bShowEvaluation = false;
   private ArrayList m_alQuestions = new ArrayList(10);
   private int m_iTotalStartMs = Integer.MAX_VALUE;
   private int m_iTotalEndMs = Integer.MIN_VALUE;
   private int m_iTotalPoints = 0;
   private boolean m_bIsFinished = false;
   private String m_strSuccessAction = null;
   private String m_strFailureAction = null;
   private HashMap m_mapFeedback = new HashMap(17);
	private boolean m_bIsRandomized = false;
	private int m_nNumberOfRandomQuestions = -1;
	private int m_nRandomRelativePassPercentage = -1;
	private ArrayList m_alRandomQuestions = new ArrayList();
   
   private boolean ParseDefinition(String strLine, BufferedReader reader, ObjectQueue mother) throws IOException
   {
      if (!strLine.startsWith("<QUESTIONNAIRE"))
         return false;
      
      SGMLParser parser = new SGMLParser(strLine, true);
      
      String strEvaluation = parser.getValue("eval");
      if (strEvaluation != null && strEvaluation.equals("true"))
         m_bShowEvaluation = true;
     
      m_strSuccessAction = parser.getValue("onSuccess");
      m_strFailureAction = parser.getValue("onFailure");

	  String sRandomized = null;
	  sRandomized = parser.getValue("randomizedQuestions");
	  
	  if(sRandomized != null && sRandomized.length() !=0 && sRandomized.equalsIgnoreCase("true"))
		   m_bIsRandomized = true;
		   
	   String sNumberOfQuestions = null;
	   sNumberOfQuestions = parser.getValue("randomizedQuestionsNumber");
	   if(sNumberOfQuestions != null && sNumberOfQuestions.length() != 0)
	   		m_nNumberOfRandomQuestions = Integer.parseInt(sNumberOfQuestions);

	   System.out.println("!!!!Soare is Randomized = " + m_bIsRandomized + " - nr of Q = " + m_nNumberOfRandomQuestions);
      
      // these actions should remain null if they are undefined or empty
      if (m_strSuccessAction != null && m_strSuccessAction.length() == 0)
         m_strSuccessAction = null;
      if (m_strFailureAction != null && m_strFailureAction.length() == 0)
         m_strFailureAction = null;
            
      boolean bSuccess = true;
      strLine = reader.readLine();
      
      while (bSuccess && strLine != null && !strLine.startsWith("</QUESTIONNAIRE>"))
      {
         if (strLine.startsWith("<QUESTION"))
         {
            Question q = new Question(this);
            bSuccess = q.ParseDefinition(strLine, reader, mother);
            
            if (bSuccess)
            {
               m_alQuestions.add(q);
               
               if (q.GetStartMs() < m_iTotalStartMs)
                  m_iTotalStartMs = q.GetStartMs();
               
               if (q.GetEndMs() > m_iTotalEndMs)
                  m_iTotalEndMs = q.GetEndMs();
               
               m_iTotalPoints += q.GetPoints();
            }
         }
         else if (strLine.startsWith("<feedback"))
            bSuccess = ParseFeedback(strLine, reader, mother);
         else if (strLine.startsWith("<pass"))
            bSuccess = ParsePassCriteria(strLine);
         else if (strLine.startsWith("<default"))
            DiscardDefaultSection(strLine, reader);
         else if (strLine.trim().length() > 0)
            System.out.println("Subtag of questionnaire not recognized: "+strLine);
         // ignore unknown and empty lines
         
         if (bSuccess)
            strLine = reader.readLine();
      }
      
      return bSuccess;
   }
   
   public boolean ContainsMs(int iTimeMs)
   {
      return iTimeMs >= m_iTotalStartMs && iTimeMs <= m_iTotalEndMs;
   }
      
   public int GetStartMs()
   {
      return m_iTotalStartMs;
   }
   
   public int GetEndMs()
   {
      return m_iTotalEndMs;
   }
  
   /**
    * @returns the maximum number of points for all the questions.
    *          Determined by adding up all single point values.
    */
   public int GetTotalPoints()
   {
      return m_iTotalPoints;
   }
   
   /**
    * @returns always an absolute point value to be reached.
    */
   public int GetPassPoints()
   {
      return m_iPassPoints;
   }
   
   public int GetRandomPassPercentage() //TODO Change name of method
   {
   		return m_nRandomRelativePassPercentage;
   }
   
   public boolean IsShowEvaluation()
   {
      return m_bShowEvaluation;
   }
   
   public String GetActionString(boolean bSuccess)
   {
      if (bSuccess)
         return m_strSuccessAction;
      else
         return m_strFailureAction;
   }
   
   public void SetActionString(String strAction, boolean bSuccess)
   {
   		if(bSuccess)
   			m_strSuccessAction = strAction;
   		else
   			m_strFailureAction = strAction;
   }
   
   public Question FindQuestionAt(int iTimeMs)
   {
      if (ContainsMs(iTimeMs))
      {
         for (int i=0; i<m_alQuestions.size(); ++i)
         {
            Question q = (Question)m_alQuestions.get(i);
         
            if (q.ContainsMs(iTimeMs))
               return q;
         }
      }
      
      return null;
   }
   
   public int CountAnswerPoints()
   {
      int iCollectedPoints = 0;
      
      for (int i=0; i<m_alQuestions.size(); ++i)
      {
         Question q = (Question)m_alQuestions.get(i);
         
         if (q.IsAnswerCorrect())
            iCollectedPoints += q.GetPoints();
      }
      
      return iCollectedPoints;
   }
   
   public boolean AnyQuestionLeft()
   {
      for (int i=0; i<m_alQuestions.size(); ++i)
      {
         Question q = (Question)m_alQuestions.get(i);
         
         boolean bQuestionActive = true;
         
         if (q.IsAnswerCorrect())
            bQuestionActive = false; // right answer given
         else
         {
            if (q.IsTimeable() && q.QueryTimer() < 0)
               bQuestionActive = false; // timer's up
            else
            {
               if (q.GetMaximumAttempts() > 0)
               {
                  if (!q.AreAttemptsLeft())
                     bQuestionActive = false; // all attempts used
               }
               else
               {
                  if (!q.IsUnanswered())
                     bQuestionActive = false; // no attempt limit but tried once
               }
            }
         }
                  
         if (bQuestionActive)
            return true;
      }
      
      return false;
   }
   //Soare
   public void SetRandomQuestions(ArrayList alRandomQuestions)
   {
   		m_alQuestions.clear();
   		int iTotalPoints = 0;
   		for(int i = 0; i < alRandomQuestions.size(); ++i)
   		{
   			Question q = (Question)alRandomQuestions.get(i);
   			m_alQuestions.add(q);
   			iTotalPoints += q.GetPoints();
   		}
   		
   		m_iTotalPoints = iTotalPoints;
   		if (m_nRandomRelativePassPercentage != -1)
   			m_iPassPoints = (int)Math.ceil(((m_nRandomRelativePassPercentage/(double)100) * iTotalPoints));
   }
   public Feedback GetFeedback(int iType)
   {
      Integer key = new Integer(iType);
      
      if (m_mapFeedback.containsKey(key))
         return (Feedback)m_mapFeedback.get(key);
      else
      {
         // normally a feedback should be defined for a questionnaire;
         // at least as "disabled"
         System.err.println("Questionnaire: Feedback "+iType+" is undefined or not set.");
         return null; 
      }
   }
   
   // easy access for Publisher:
   public Question[] GetQuestions()
   {
      Question[] qs = new Question[m_alQuestions.size()];
      m_alQuestions.toArray(qs);
      
      return qs;
   }

	public boolean IsTestRandomized()
	{
		return m_bIsRandomized;
	}

	public int GetNumberOfRandomQuestions()
	{
		return m_nNumberOfRandomQuestions;
	}

   private boolean ParsePassCriteria(String strLine)
   {
      SGMLParser parser = new SGMLParser(strLine);
      
      String strType = parser.getValue("type");
      // "percentage", "absolute"
      String strMaximum = parser.getValue("max");
      String strValue = parser.getValue("value");
      
      if (strType == null || strValue == null)
         return false;
      
      int iValue = ExtractInt(strValue, -1);
      
      if (iValue < 0)
         return false;
      
      if (strType.equals("relative"))
      {
         if (iValue > 100)
            return false;
         
         int iValueMax = ExtractInt(strMaximum, 0);
         if (iValueMax < 0)
            return false;
         m_nRandomRelativePassPercentage = iValue;
         m_iPassPoints = (int)Math.ceil(((iValue/(double)100) * iValueMax));
      }
      else if (strType.equals("absolute"))
         m_iPassPoints = iValue;
      
      // TODO this is a little bit inconsistent:
      // percentage requires a maximum value, however this could 
      // (and maybe should) also be generated from the contained 
      // questions.
      
      return true;
   }
   
   private boolean ParseFeedback(String strLine, BufferedReader reader, ObjectQueue mother) throws IOException
   {
      Feedback f = new Feedback();
      boolean bSuccess = f.Parse(strLine, reader, mother);
      
      if (bSuccess)
         m_mapFeedback.put(new Integer(f.GetType()), f);
      
      return bSuccess;
   }
   
   private void DiscardDefaultSection(String strLine, BufferedReader reader) throws IOException
   {
      while (!strLine.startsWith("</default"))
         strLine = reader.readLine();
   }
   
   private int ExtractInt(String strDef, int iDefault)
   {
      int iValue = iDefault;
      
      if (strDef != null)
      {
         try
         {
            iValue = Integer.parseInt(strDef);
         }
         catch (NumberFormatException exc)
         {
            // ignore: iDefault will be returned
         }
      }
  
      return iValue;
   }
   
}