package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.io.BufferedReader;
import java.util.ArrayList;
import java.util.StringTokenizer;
import javax.swing.BorderFactory;
import javax.swing.JTextField;
import javax.swing.border.BevelBorder;
import javax.swing.border.SoftBevelBorder;

import imc.epresenter.player.util.SGMLParser;

public class TextFieldDynamic extends DynamicComponent
{
   private ArrayList m_aCorrectTexts = new ArrayList(5);
   
	public TextFieldDynamic(String strLine, BufferedReader in)
   {
      super(strLine, in);
      
      SGMLParser parser = new SGMLParser(strLine);
      String strCorrectTexts = parser.getValue("give");
      if (strCorrectTexts != null)
      {
         String strOneText = "";
         boolean bEscapeActive = false;
         StringTokenizer tokenizer = new StringTokenizer(strCorrectTexts, ";\\", true);
         while (tokenizer.hasMoreTokens())
         {
            String strToken = tokenizer.nextToken();
            
            if (strToken.equals(";") && !bEscapeActive)
            {
               if (strOneText.length() > 0)
                  m_aCorrectTexts.add(strOneText);
               strOneText = "";
            }
            else if (strToken.equals("\\") && !bEscapeActive)
            {
               bEscapeActive = true;
            }
            else 
            {
               strOneText += strToken;
               if (bEscapeActive)
                  bEscapeActive = false;
            }
         }
      }
      
      
      m_BoardComponent = new JTextField();
      m_BoardComponent.setBorder(BorderFactory.createLineBorder(Color.black, 1));
      Rectangle rcObjDefine = GetActivityArea();
      m_BoardComponent.setSize(rcObjDefine.width, rcObjDefine.height);
      m_BoardComponent.setPreferredSize(new Dimension(rcObjDefine.width, rcObjDefine.height));
      m_BoardComponent.setLocation(rcObjDefine.x, rcObjDefine.y);
      m_BoardComponent.setOpaque(true);
      m_BoardComponent.setVisible(false);
   }
   
   public boolean CheckAnswer()
   {
      JTextField tf = (JTextField)m_BoardComponent;
      boolean bAnswer = false;
      for (int i=0; i<m_aCorrectTexts.size(); ++i)
      {
         String strOneText = (String)m_aCorrectTexts.get(i);
         if (strOneText.equals(tf.getText()))
             bAnswer = true;
      }
      return bAnswer;
   }
   
   public ArrayList GetCorrectTexts()
   {
      return m_aCorrectTexts;
   }
   
   public void Reset()
   {
      ((JTextField)m_BoardComponent).setText("");
   }
}