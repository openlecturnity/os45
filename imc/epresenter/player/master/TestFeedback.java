package imc.epresenter.player.master;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import javax.swing.*;
import javax.swing.border.Border;

import imc.epresenter.filesdk.Feedback;
import imc.epresenter.filesdk.Questionnaire;
import imc.epresenter.filesdk.Question;
import imc.epresenter.player.Manager;
import imc.epresenter.player.whiteboard.Text;
import imc.epresenter.player.whiteboard.VisualComponent;
import imc.epresenter.player.whiteboard.WhiteBoardPlayer;
import imc.lecturnity.util.ui.LayoutUtils;

public class TestFeedback extends JWindow
{
   public static void Show(int iType, Question q, final WhiteBoardPlayer wbp)
   {
      final Feedback feedback = q.GetFeedback(iType, true);
      
      if (feedback != null && feedback.IsEnabled())
         wbp.ShowFeedback(feedback);
   }

   public static void ShowEvaluation(Questionnaire qq, WhiteBoardPlayer wbp)
   {
      if (qq != null && wbp != null && qq.IsShowEvaluation())
      {
         Feedback feedback = qq.GetFeedback(Feedback.FEEDBACK_TYPE_QQ_EVAL);
      
         if (feedback != null && feedback.IsEnabled())
         {
            ArrayList alComps = feedback.GetComponents();
            
            if (alComps != null && alComps.size() > 0)
            {
               int iAchievedPoints = qq.CountAnswerPoints();
               int iTotalPoints = qq.GetTotalPoints();
               int iPassPoints = qq.GetPassPoints();
               int iAchievedPercent = 100;
               if (iTotalPoints > 0)
                  iAchievedPercent = (iAchievedPoints * 100) / iTotalPoints;
               int iPassPercent = 100;
			   if (iTotalPoints > 0)
				   iPassPercent = qq.GetRandomPassPercentage();//(iPassPoints * 100) / iTotalPoints;
               
               boolean bSuccess = iAchievedPoints >= iPassPoints;
               
               Question[] qs = qq.GetQuestions();
               int iCorrectCounter = 0;
               int iPointsCounter = 0;
               for (int i=0; i<alComps.size(); ++i)
               {
                  VisualComponent comp = (VisualComponent)alComps.get(i);
                  if (comp instanceof Text)
                  {
                     Text text = (Text)comp;
                     if (text.HasChangeableType())
                     {
                        if (!bSuccess && text.GetTextType() == Text.TEXT_TYPE_EVAL_QQ_PASSED)
                           text.ChangeText("");
                        else if (bSuccess && text.GetTextType() == Text.TEXT_TYPE_EVAL_QQ_FAILED)
                           text.ChangeText("");
                        else if (text.GetTextType() == Text.TEXT_TYPE_EVAL_QQ_TOTAL)
                           text.ChangeText(""+iTotalPoints);
                        else if (text.GetTextType() == Text.TEXT_TYPE_EVAL_QQ_ACHIEVED)
                           text.ChangeText(""+iAchievedPoints);
                        else if (text.GetTextType() == Text.TEXT_TYPE_EVAL_QQ_ACHIEVED_PERCENT)
                           text.ChangeText(""+iAchievedPercent+"%");
                        else if (text.GetTextType() == Text.TEXT_TYPE_EVAL_QQ_REQUIRED)
                           text.ChangeText(""+iPassPoints);
                        else if (text.GetTextType() == Text.TEXT_TYPE_EVAL_QQ_REQUIRED_PERCENT)
                           text.ChangeText(""+iPassPercent+"%");
                        else if (text.GetTextType() == Text.TEXT_TYPE_EVAL_Q_CORRECT)
                        {
                           if (iCorrectCounter < qs.length)
                           {
                              if (qs[iCorrectCounter].IsAnswerCorrect())
                                 text.ChangeText(Manager.getLocalized("correct"));
                              else
                                 text.ChangeText(Manager.getLocalized("wrong"));
                              ++iCorrectCounter;
                           }
                        }
                        else if (text.GetTextType() == Text.TEXT_TYPE_EVAL_Q_POINTS)
                        {
                           if (iPointsCounter < qs.length)
                           {
                              if (qs[iPointsCounter].IsAnswerCorrect())
                                 text.ChangeText(""+qs[iPointsCounter].GetPoints());
                              else
                                 text.ChangeText("0");
                              ++iPointsCounter;
                           }
                        }
                     }
                  }
               }
            }
            
            
            wbp.ShowFeedback(feedback);
         }
      }
   }
}