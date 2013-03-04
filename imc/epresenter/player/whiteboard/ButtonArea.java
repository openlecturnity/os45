package imc.epresenter.player.whiteboard;

import java.awt.Graphics;
import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.player.util.SGMLParser;

public class ButtonArea extends InteractionArea
{
   private VisualComponent[] m_aNormalComponents = null;
   private VisualComponent[] m_aOverComponents = null;
   private VisualComponent[] m_aPressedComponents = null;
   private VisualComponent[] m_aInactiveComponents = null;
   
   private boolean m_bMouseOver = false;
   private boolean m_bMousePressed = false;
   
   public ButtonArea(String strLine, BufferedReader in, 
                     ObjectQueue mother, ArrayList alPrevious)
   throws IOException
   {
      super(strLine, in);
      
      strLine = in.readLine();
      while (strLine != null && !strLine.startsWith("</"))
      {
         ParseRegion(strLine, in, mother, alPrevious);
         strLine = in.readLine();
      }
   }
   
   public boolean IsQuestionObject()
   {
      String strClickAction = GetActionString(InteractionArea.ACTION_MOUSE_CLICK);
                        
      if (strClickAction != null)
      {
         if (strClickAction.equals("check") || strClickAction.equals("reset"))
            return true;
      }
                        
      return false;
   }
   
   private void ParseRegion(String strLine, BufferedReader in, 
                            ObjectQueue mother, ArrayList alPrevious)
   throws IOException
   {
      String strState = strLine;
      
      ArrayList alComponentCollector = new ArrayList(5);
      
      strLine = in.readLine();
      while (strLine != null && !strLine.startsWith("</"))
      {
         VisualComponent vc = VisualComponent.CreateComponent(strLine, in, mother, alPrevious);
         alComponentCollector.add(vc);
         strLine = in.readLine();
      }
      
      VisualComponent[] aSomeComponents = new VisualComponent[alComponentCollector.size()];
      alComponentCollector.toArray(aSomeComponents);
      
      if (strState.equals("<normal>"))
         m_aNormalComponents = aSomeComponents;
      else if (strState.equals("<over>"))
         m_aOverComponents = aSomeComponents;
      else if (strState.equals("<pressed>"))
         m_aPressedComponents = aSomeComponents;
      else if (strState.equals("<inactive>"))
         m_aInactiveComponents = aSomeComponents;
   }
  
   public void paint(Graphics g, int cwidth, int cheight, double dScale)
   {
      // paint the appropriate components
      
      VisualComponent[] aSomeComponents = m_aNormalComponents;
      if (!m_bIsActivated)
      {
         if (m_aInactiveComponents != null)
            aSomeComponents = m_aInactiveComponents;
      }
      else if (m_bMousePressed && m_bMouseOver)
      {
         if (m_aPressedComponents != null)
            aSomeComponents = m_aPressedComponents;
      }
      else if (m_bMouseOver)
      {
         if (m_aOverComponents != null)
            aSomeComponents = m_aOverComponents;
      }
      
      if (aSomeComponents != null)
      {
         for (int i=0; i<aSomeComponents.length; ++i)
         {
            aSomeComponents[i].paint(g, cwidth, cheight, dScale);
         }
      }
   }
   
   public void Visualize(boolean bHasFocus, boolean bMousePressed)
   {
      m_bMouseOver = bHasFocus;
      m_bMousePressed = bMousePressed;
   }
   
   public VisualComponent[] getNormalComponents()
   {
      return m_aNormalComponents;
   }
   
   public VisualComponent[] getOverComponents()
   {
      return m_aOverComponents;
   }
   
   public VisualComponent[] getPressedComponents()
   {
      return m_aPressedComponents;
   }
   
   public VisualComponent[] getInactiveComponents()
   {
      return m_aInactiveComponents;
   }
}