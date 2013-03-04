package imc.epresenter.filesdk;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;

import imc.epresenter.player.util.SGMLParser;
import imc.epresenter.player.whiteboard.ButtonArea;
import imc.epresenter.player.whiteboard.ObjectQueue;
import imc.epresenter.player.whiteboard.VisualComponent;

public class Feedback
{
   public static final int FEEDBACK_TYPE_UNDEFINED = 0;
   public static final int FEEDBACK_TYPE_Q_CORRECT = 1;
   public static final int FEEDBACK_TYPE_Q_WRONG = 2;
   public static final int FEEDBACK_TYPE_Q_REPEAT = 3;
   public static final int FEEDBACK_TYPE_Q_TRIES = 4;
   public static final int FEEDBACK_TYPE_Q_TIME = 5;
   public static final int FEEDBACK_TYPE_QQ_PASSED = 6;
   public static final int FEEDBACK_TYPE_QQ_FAILED = 7;
   public static final int FEEDBACK_TYPE_QQ_EVAL = 8;
   
   private int m_iType = FEEDBACK_TYPE_UNDEFINED;
   private boolean m_bIsDisabled = false;
   private boolean m_bIsInherited = false;
   private String m_strChangeText = null;
   private ArrayList m_alComponents = new ArrayList(10);

   public boolean Parse(String strLine, BufferedReader reader, ObjectQueue mother) throws IOException
   {
      if (!strLine.startsWith("<feedback"))
         return false;
      
      SGMLParser parser = new SGMLParser(strLine, true);
      
      String strType = parser.getValue("type");
      
      if (strType != null && strType.length() > 0)
      {
         if (strType.equals("q-correct"))
            m_iType = FEEDBACK_TYPE_Q_CORRECT;
         else if (strType.equals("q-wrong"))
            m_iType = FEEDBACK_TYPE_Q_WRONG;
         else if (strType.equals("q-repeat"))
            m_iType = FEEDBACK_TYPE_Q_REPEAT;
         else if (strType.equals("q-tries"))
            m_iType = FEEDBACK_TYPE_Q_TRIES;
         else if (strType.equals("q-time"))
            m_iType = FEEDBACK_TYPE_Q_TIME;
         else if (strType.equals("qq-passed"))
            m_iType = FEEDBACK_TYPE_QQ_PASSED;
         else if (strType.equals("qq-failed"))
            m_iType = FEEDBACK_TYPE_QQ_FAILED;
         else if (strType.equals("qq-eval"))
            m_iType = FEEDBACK_TYPE_QQ_EVAL;
      }
      else
         return false; // it needs a type
      
      String strValue = parser.getValue("value");
      
      if (strValue != null)
      {
         if (strValue.equals("disable"))
         {
            m_bIsDisabled = true;
         }
         else if (strValue.equals("inherit"))
         {
            m_bIsInherited = true;
         }
         else if (strValue.equals("change-text"))
         {
            m_bIsInherited = true;
            m_strChangeText = parser.getValue("text");
         }
         else if (strValue.equals("enable"))
         {
            // nothing done here, objects are parsed below
         }
         else
         {
            System.err.println("Feedback: Did not recognize value: "+strValue);
            return false;
         }
      }
      else
      {
         System.err.println("Feedback: No value specified.");
         return false;
      }
      
 
      if (!strLine.endsWith("</feedback>"))
      {
         strLine = reader.readLine();
         while(!strLine.endsWith("</feedback>"))
         {
            if (!m_bIsDisabled && !m_bIsInherited)
            {
               // regular definition: parse the objects
            
               VisualComponent vc = null;
               vc = VisualComponent.CreateComponent(strLine, reader, mother, m_alComponents);
         
               if (vc != null)
               {
                  if (vc instanceof ButtonArea)
                     ((ButtonArea)vc).SetActive(true);
               
                  m_alComponents.add(vc);
               }
            }
         
            strLine = reader.readLine();
         }
      }
       
      return true;
   }
  
   
   public int GetType()
   {
      return m_iType;
   }
   
   public boolean IsEnabled()
   {
      return !m_bIsDisabled;
   }
   
   public boolean IsInherited()
   {
      return m_bIsInherited;
   }
   
   /**
    * Only if IsInherited() returns true this _can_ be different from null.
    */
   public String GetChangeText()
   {
      return m_strChangeText;
   }
   
   public ArrayList GetComponents()
   {
      return m_alComponents;
   }
}