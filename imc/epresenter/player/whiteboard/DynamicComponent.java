package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.geom.AffineTransform;
import java.io.BufferedReader;
import javax.swing.ButtonGroup;
import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JRadioButton;

public abstract class DynamicComponent extends InteractionArea
{
   protected JComponent m_BoardComponent;
   protected ButtonGroup m_Group = null; // TODO this is not placed very good here
   
   public DynamicComponent(String strLine, BufferedReader in)
   {
      super(strLine, in);
   }
  
   public boolean IsQuestionObject()
   {
      return true;
   }
   
   public boolean ShowHide(int iTimeMs)
   {
      if (m_BoardComponent != null)
      {
         boolean bShouldBeVisible = false;
      
         if (m_aiVisibilityTimes != null && m_aiVisibilityTimes.length > 1)
         {
            for (int i=0; i<m_aiVisibilityTimes.length; i+=2)
            {
               if (iTimeMs >= m_aiVisibilityTimes[i] && iTimeMs <= m_aiVisibilityTimes[i+1])
               {
                  bShouldBeVisible = true;
                  break;
               }
            }
         }
         
         if (bShouldBeVisible != m_BoardComponent.isVisible())
         {
            m_BoardComponent.setVisible(bShouldBeVisible);
            
            return true;
         }
         else
            return false;
      }
      else
         return false; // no component: no visibility change
      
   }
   
   public boolean IsVisible()
   {
      if (m_BoardComponent != null)
         return m_BoardComponent.isVisible();
      else
         return false;
   }
   
   public JComponent GetNative()
   {
      return m_BoardComponent;
   }
   
   public void AddRadioTo(ButtonGroup grpRadios)
   {
      if (grpRadios != null && m_BoardComponent != null && m_BoardComponent instanceof JRadioButton)
      {
         m_Group = grpRadios;
         grpRadios.add((JRadioButton)m_BoardComponent);
      }
   }
  
   public boolean SetActive(boolean bYes)
   {
      boolean bResult = super.SetActive(bYes);
      
      if (m_BoardComponent != null && bYes != m_BoardComponent.isEnabled())
         m_BoardComponent.setEnabled(bYes);
      
      return bResult;
   }
   
      
   void paint(Graphics g, int cwidth, int cheight, double dScale)
   {
      Graphics2D g2 = (Graphics2D)g;
      
      /* debug painting: scaled size of component
      AffineTransform tranf3 = null;
      if (dScale != 1.0)
      {
         tranf3 = g2.getTransform();
         g2.scale(dScale, dScale);
      }
         
      g.setColor(Color.cyan);
      g2.fill(rcObjDefine);
         
      if (dScale != 1.0)
         g2.setTransform(tranf3);
         */
         
      if (!m_BoardComponent.isVisible())
      {
         // feedback visible or direct painting (thumbnails): paint dummy component
  
         // Note: the component paints itself scaled
         
         Dimension dimNow = m_BoardComponent.getSize();
         Dimension dimPref = m_BoardComponent.getPreferredSize();
         Dimension dimScaled = new Dimension((int)(dScale * dimPref.width), 
                                             (int)(dScale * dimPref.height));
         if (dScale != 1.0 && !dimNow.equals(dimScaled))
            m_BoardComponent.setSize(dimScaled);
         // this case: painting of thumbnails: the original component 
         // (on invisible whiteboard) was not resized
         
         Rectangle rcObjDefine = GetActivityArea();
         int iScaledOffsetX = (int)(dScale*rcObjDefine.x);
         int iScaledOffsetY = (int)(dScale*rcObjDefine.y);
          
         g.translate(iScaledOffsetX, iScaledOffsetY);
        
         m_BoardComponent.paint(g);
        
         g.translate(-iScaledOffsetX, -iScaledOffsetY);
      }
   }
   
   public abstract boolean CheckAnswer();
   
   public abstract void Reset();
}