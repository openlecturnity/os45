package imc.epresenter.player.master;

import java.awt.Component;
import java.awt.Frame;
import java.awt.Window;
import javax.swing.SwingUtilities;
import java.util.ArrayList;
import java.util.Collections;

import imc.epresenter.filesdk.Feedback;
import imc.epresenter.filesdk.Questionnaire;
import imc.epresenter.filesdk.Question;
import imc.epresenter.player.Coordinator;
import imc.epresenter.player.Manager;
import imc.epresenter.player.whiteboard.ObjectQueue;
import imc.epresenter.player.whiteboard.WhiteBoardPlayer;

public class TestEngine implements Runnable
{
   private Questionnaire[] m_aQuestionnaires;
   private WhiteBoardPlayer m_Whiteboard;
   //private Component m_CompParent;
   private Question m_LastTimered = null;
   private boolean m_bFeedbackActive = false;
 
   private boolean m_bIsNormalRun = true;
   private int m_iAnswerTimeMs = 0;
   private boolean m_bQuestionSuccess = true;
   private boolean m_bIsTestRandom = false;
   private int m_nNumberOfRandomQuestions = 0;
   private Question [] m_aRandomQuestions;
   private int m_nBeforeFirstQuestionTimestamp = -1;
   private int m_nFirstRandomQuestionStartMs = -1;    
   private boolean m_bIsQuestionOnFirstPage = false;
   
   public TestEngine(Questionnaire[] aQuestionnaires, Coordinator c, WhiteBoardPlayer wb)
   {
      m_aQuestionnaires = aQuestionnaires;
      m_Whiteboard = wb;
      CheckRandomTest();
      //m_CompParent = m_Whiteboard.getVisualComponents()[0];
   }
   
   public void QuestionAnsweredAt(int iTimeMs, boolean bQuestionSuccess)
   {
      if (!m_bFeedbackActive)
      {
         m_bFeedbackActive = true;
         
         m_iAnswerTimeMs = iTimeMs;
         m_bQuestionSuccess = bQuestionSuccess;
         
         m_bIsNormalRun = true;
         new Thread(this).start();
         // m_bFeedbackActive is reset at the end of run()
         
         // Note: Showing feedback objects should be blocking.
         // But as this is called for the ui thread it must be a thread
         // from here on.
         // And other interaction on the slides must be prohibited while
         // feedback is active.
      }
   }
   
   public boolean IsFeedbackActive()
   {
      return m_bFeedbackActive;
   }
   
   public void run()
   {
      int iTimeMs = m_iAnswerTimeMs;
      boolean bQuestionSuccess = m_bQuestionSuccess;
      
      Question q = FindQuestionAt(iTimeMs);
      
      if (q != null)
      {
         if (m_bIsNormalRun)
         {
            // increment attempts counter and store success
            q.UpdateAnswered(bQuestionSuccess);
         
            if (bQuestionSuccess)
               TestFeedback.Show(Feedback.FEEDBACK_TYPE_Q_CORRECT, q, m_Whiteboard);
            else
            {
               if (!q.IsTimeable() || q.UpdateTimer() > 0)
               {
                  if (q.AreAttemptsLeft())
                  {
                     TestFeedback.Show(Feedback.FEEDBACK_TYPE_Q_REPEAT, q, m_Whiteboard);
               
                     m_bFeedbackActive = false;
                     return; // wait for the next try
                  }
                  else
                     TestFeedback.Show(Feedback.FEEDBACK_TYPE_Q_WRONG, q, m_Whiteboard);
               }
               // else already a message "time is up" was shown
            
            }
         
            m_Whiteboard.DeactivateVisibleQuestionDynamics(iTimeMs);
             
            CheckQuestionnaireFinished(q, bQuestionSuccess);
         }
         else
         {
            q.InformShownTimerMessage(); // next call (WhiteBoardPlayer.tickTime()) should skip this
            m_Whiteboard.DeactivateVisibleQuestionDynamics(iTimeMs);
            TestFeedback.Show(Feedback.FEEDBACK_TYPE_Q_TIME, q, m_Whiteboard);
            CheckQuestionnaireFinished(q, false); // a timer "interrupt" means question answer wrong
         }
      }
      else
         System.err.println("TestEngine: No question found for time: "+iTimeMs);
      // TODO this is some kind of major error?
      
      
      m_bFeedbackActive = false;
   }
         
   /**
    * @returns the time left for this question in milliseconds or -1
    *          if no question with time constraint was found.
    */ 
   public long UpdateQuestionTimerAt(int iTimeMs)
   {
      Question q = FindQuestionAt(iTimeMs);
      if (!q.IsTimeable() || !q.AreAttemptsLeft() || q.IsAnswerCorrect())
         q = null;
      
      if (m_LastTimered != null && m_LastTimered != q) // q can be null
      {
         m_LastTimered.StopTimer();
         m_LastTimered = null;
      }
      
      if (q != null && q.IsTimeable() && q.AreAttemptsLeft() && !q.IsAnswerCorrect())
      {
         m_LastTimered = q;
         
         long lTimeLeftMs = q.UpdateTimer();
         
         if (lTimeLeftMs < 0 && !q.IsTimerMessageShown())
         {
            m_bFeedbackActive = true;
            m_iAnswerTimeMs = iTimeMs;
            m_bIsNormalRun = false;
            new Thread(this).start();
            // m_bFeedbackActive is reset at the end of run()
         }
         
         return lTimeLeftMs;
      }
      else
      {       
         return -1;
      }
   }
   /*
   public int GetQuestionTriesLeftAt(int iTimeMs)
   {
      Question q = FindQuestionAt(iTimeMs);
      if (q != null && q.GetMaximumAttempts() > 0)
         return q.GetAnswerAttempts();
      else
         return -1;
   }
   */
   
   private void CheckQuestionnaireFinished(Question q, boolean bQuestionSuccess)
   {
      // TODO make this (finding the Questionnaire) not indirect.
      Questionnaire qq = q.GetQuestionnaire();
      
      if (qq != null)
      {
         if (qq.AnyQuestionLeft())
         {
            int iTimeOne = m_Whiteboard.getMediaTime();
            while(m_Whiteboard.IsFeedbackActive())
               try { Thread.sleep(50); } catch (InterruptedException exc) { }
            boolean bJumpDone = m_Whiteboard.getMediaTime() != iTimeOne;
            
            if (!bJumpDone)
               m_Whiteboard.ExecuteAction(q, bQuestionSuccess);
            return; // wait for other questions
         }
         
         
         int iPoints = qq.CountAnswerPoints();
            
         boolean bQuestionnairePassed = iPoints >= qq.GetPassPoints();
            
         if (bQuestionnairePassed)
            TestFeedback.Show(Feedback.FEEDBACK_TYPE_QQ_PASSED, q, m_Whiteboard);
         else
            TestFeedback.Show(Feedback.FEEDBACK_TYPE_QQ_FAILED, q, m_Whiteboard);
            
         if (qq.IsShowEvaluation())
            TestFeedback.ShowEvaluation(qq, m_Whiteboard);
        
         int iTimeOne = m_Whiteboard.getMediaTime();
         while(m_Whiteboard.IsFeedbackActive())
            try { Thread.sleep(50); } catch (InterruptedException exc) { }
         boolean bJumpDone = m_Whiteboard.getMediaTime() != iTimeOne;
           
         if (!bJumpDone)
            m_Whiteboard.ExecuteAction(qq, bQuestionnairePassed);
      }
      else
         System.err.println("TestEngine: No questionnaire found for question: "+q.GetStartMs());
         // TODO this is some kind of major error?
   }
   
   public Question FindQuestionAt(int iTimeMs)
   {
      if (m_aQuestionnaires != null)
      {
         for (int i=0; i<m_aQuestionnaires.length; ++i)
         {
            if (m_aQuestionnaires[i].ContainsMs(iTimeMs))
            {
               Question q = m_aQuestionnaires[i].FindQuestionAt(iTimeMs);
               if (q != null)
                  return q;
               // else continue search in other questionnaires
            }
         }
      }
      
      return null;
   }
   
   //Randomized Tests code
   public boolean IsTestRandom()
   {
   		return m_bIsTestRandom;
   }
   
   
   public int GetNumberOfRandomQuestions()
   {
   		return m_nNumberOfRandomQuestions;
   }
   
   public boolean AreAnyQuestionLeft()
   {
	   return m_aQuestionnaires[0].AnyQuestionLeft();
   }
   private void CheckRandomTest()
   {
   		for (int i = 0; i < m_aQuestionnaires.length; ++i)
   		{
   			if (m_aQuestionnaires[i].IsTestRandomized())
   			{
   				m_bIsTestRandom = true;
   				m_nNumberOfRandomQuestions = m_aQuestionnaires[i].GetNumberOfRandomQuestions();
   				break;
   			}
   		}
   		
   		if (IsTestRandom())
   			InitRandomizedTest();
   }
   
   private void InitRandomizedTest()
   {
   		int nTotalNumberOfQuestions = 0;
   		
   		m_aRandomQuestions = new Question[m_nNumberOfRandomQuestions];
   		
		ArrayList aQuestions = new ArrayList();
   		for (int i = 0 ; i < m_aQuestionnaires.length; ++ i)
   		{
   			Question[]qq = m_aQuestionnaires[i].GetQuestions();
   			nTotalNumberOfQuestions += qq.length;

   			for (int j = 0; j < qq.length; ++j)
	   		{
	   			aQuestions.add(qq[j]);
	   		}
   		}
   		
   		if (aQuestions.size() < 1)
   			return;
   			
   		Question[] qs = new Question[aQuestions.size()];
   		aQuestions.toArray(qs);
   		
   		m_nBeforeFirstQuestionTimestamp = qs[0].GetStartMs() - 1;
   		if(m_nBeforeFirstQuestionTimestamp < 1)
   		{
   			m_nBeforeFirstQuestionTimestamp = 1;
   			m_bIsQuestionOnFirstPage = true;
   		}
   		
   		ArrayList aQIndexes = new ArrayList();
   		for(int k = 0; k < qs.length; ++k)
   		{
   			aQIndexes.add(k);
   		}
   		
   		Collections.shuffle(aQIndexes);
   		
   		ArrayList randomIndexes = new ArrayList(m_nNumberOfRandomQuestions);
   		for (int l = 0; l < m_nNumberOfRandomQuestions; ++l)
   		{
   			randomIndexes.add(aQIndexes.get(l));
   		}
   		
   		Integer aRandomIndexes[] = new Integer[randomIndexes.size()];
   		randomIndexes.toArray(aRandomIndexes);
   		
   		String strRandIndx = "";
   		for(int ii = 0; ii < aRandomIndexes.length; ++ii)
   		{
   			strRandIndx += aRandomIndexes[ii] + ",";
   		}
   		
   		ArrayList alRandomQuestions = new ArrayList();

   		for(int jj = 0; jj < m_nNumberOfRandomQuestions; ++jj)
   		{
   			m_aRandomQuestions[jj] = qs[aRandomIndexes[jj]];
   			alRandomQuestions.add(m_aRandomQuestions[jj]);
   		}
   		
   		
   		m_nFirstRandomQuestionStartMs = m_aRandomQuestions[0].GetStartMs();
   		for(int kk = 0; kk < m_aRandomQuestions.length - 1; ++kk)
   		{
   			String strAction = "Rjump page "+ m_aRandomQuestions[kk+1].GetStartMs() + ";start";
   			m_aRandomQuestions[kk].SetActionString(strAction);
   		}
   		
   		/*String strQQAction = "jump page ";
   		int iLastQMs = qs[qs.length - 1].GetEndMs();
   		if(iLastQMs < m_Whiteboard.getDuration() - 2)
   		{
   			strQQAction += iLastQMs + 1;
   		}
   		else
   		{
   			strQQAction += m_aRandomQuestions[m_aRandomQuestions.length - 1].GetEndMs() - 1;
   		}*/
   		
   		int iLastQMs = qs[qs.length - 1].GetEndMs();
   		int iFirstQMs = qs[0].GetStartMs();
   		for(int kkk = 0; kkk < m_aQuestionnaires.length; ++kkk)
   		{
   			String strActionC = m_aQuestionnaires[kkk].GetActionString(true);
   			String strActionF = m_aQuestionnaires[kkk].GetActionString(false);
   			m_aQuestionnaires[kkk].SetRandomQuestions(alRandomQuestions);
   			
   			if(strActionC != null)
   			{
   				strActionC = ProcessQuestionnaireAction(iFirstQMs, iLastQMs, strActionC);
   				m_aQuestionnaires[kkk].SetActionString(strActionC, true);
   			}
   			if(strActionF != null)
   			{
   				strActionF = ProcessQuestionnaireAction(iFirstQMs, iLastQMs, strActionF);
   				m_aQuestionnaires[kkk].SetActionString(strActionF, false);
   			}
   		}
   		String str = "Randomized - nr of q = " + nTotalNumberOfQuestions + " - nr of rans q = " + m_nNumberOfRandomQuestions + " -rand: " + strRandIndx; 
   		System.out.println(str);
   }
   
   public int GetBeforeFirstQuestionTimestamp()
   {
   		return m_nBeforeFirstQuestionTimestamp;
   }
   
   public void ResetBeforeFirstQuestionTimestamp()
   {
   		m_nBeforeFirstQuestionTimestamp = -1000;
   }
   
   public int GetFirstRandomQuestionMs()
   {
   		return m_nFirstRandomQuestionStartMs;
   }
   
   public boolean IsQuestionOnFirstPage()
   {
   		return m_bIsQuestionOnFirstPage;
   }
   
   public void ResetIsQuestionOnFirstPage()
   {
   		m_bIsQuestionOnFirstPage = false;
   }
    
   private String ProcessQuestionnaireAction(int iFirstQMs, int iLastQMs, String strAction)
   {
   		String strNewAction = strAction;

   		if(strAction.indexOf("jump next") != -1 || strAction.indexOf("start") != -1)
   		{
   			if(iLastQMs < m_Whiteboard.getDuration() - 2)
   			{
   				iLastQMs++;
   				strNewAction = "jump page " + iLastQMs + ";start";
   			}
   			else
   				strNewAction = "stop";
   		}
   		else if(strAction.indexOf("prev") != -1)
   		{
   			if(iFirstQMs <= 1)
   				strNewAction = "stop";
   			else
   				strNewAction = "Rprev" + iFirstQMs;
   		}
   		return strNewAction;
   } 
}