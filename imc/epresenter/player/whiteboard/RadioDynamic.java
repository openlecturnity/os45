package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.util.Enumeration;
import javax.swing.JCheckBox;
import javax.swing.JRadioButton;
import javax.swing.JToggleButton;

import imc.epresenter.player.util.SGMLParser;

public class RadioDynamic extends DynamicComponent
{
   private boolean m_bShouldBeChecked = false;
   private boolean m_bIsCheckBox = false;
   
	public RadioDynamic(String strLine, BufferedReader in, boolean bIsCheckBox)
   {
      super(strLine, in);
      
      SGMLParser parser = new SGMLParser(strLine);
      String strCorrect = parser.getValue("give");
      if (strCorrect != null)
         m_bShouldBeChecked = strCorrect.equals("w");
      m_bIsCheckBox = bIsCheckBox;
      
      if (m_bIsCheckBox)
      {
         m_BoardComponent = new MyCheckBox();
         ((JCheckBox)m_BoardComponent).setHorizontalAlignment(JRadioButton.CENTER);
         ((JCheckBox)m_BoardComponent).setVerticalAlignment(JRadioButton.CENTER);
      }
      else
      {
         m_BoardComponent = new MyRadioButton();
         ((JRadioButton)m_BoardComponent).setHorizontalAlignment(JRadioButton.CENTER);
         ((JRadioButton)m_BoardComponent).setVerticalAlignment(JRadioButton.CENTER);
      }
      Rectangle rcObjDefine = GetActivityArea();
      m_BoardComponent.setSize(rcObjDefine.width, rcObjDefine.height);
      m_BoardComponent.setPreferredSize(new Dimension(rcObjDefine.width, rcObjDefine.height));
      m_BoardComponent.setLocation(rcObjDefine.x, rcObjDefine.y);
      m_BoardComponent.setOpaque(false);
      m_BoardComponent.setVisible(false);
      
      if (m_bIsCheckBox)
         ((MyCheckBox)m_BoardComponent).InitImages();
      else
         ((MyRadioButton)m_BoardComponent).InitImages();
   }
   
   public boolean CheckAnswer()
   {
      JToggleButton tb = (JToggleButton)m_BoardComponent;
         
      return tb.isSelected() == m_bShouldBeChecked;
   }
   
   public boolean IsCorrectAnswer()
   {
      return m_bShouldBeChecked;
   }
   
   public boolean IsCheckBox()
   {
      return m_bIsCheckBox;
   }
   
   public void Reset()
   {
      if (m_Group != null)
      {
         Enumeration enumerator = m_Group.getElements();
         while (enumerator.hasMoreElements())
         {
            JRadioButton btn = (JRadioButton)enumerator.nextElement();
            if (!btn.isVisible())
               btn.setSelected(true); // invisible button, resets on all other buttons
         }
      }
      
      ((JToggleButton)m_BoardComponent).setSelected(false);
   }
   
   // exactly double code (below)
   private static class MyRadioButton extends JRadioButton
   {
      private BufferedImage m_ImageNormal = null;
      private BufferedImage m_ImageSelected = null;
      
      void InitImages()
      {
         Dimension dimPref = getPreferredSize();
         
         m_ImageNormal = new BufferedImage(dimPref.width, dimPref.height,
                                           BufferedImage.TYPE_INT_ARGB);
         m_ImageSelected =  new BufferedImage(dimPref.width, dimPref.height,
                                              BufferedImage.TYPE_INT_ARGB);
        
         boolean bIsSelected = isSelected();
         
         if (getWidth() <= 0)
            setSize(dimPref); // make sure something gets painted
         
         setSelected(true);
         super.paintComponent(m_ImageSelected.createGraphics());
         setSelected(false);
         super.paintComponent(m_ImageNormal.createGraphics());
         
         setSelected(bIsSelected);
      }
      
      protected void paintComponent(Graphics g)
      {
         Graphics2D g2 = (Graphics2D)g;
               
         Dimension dimPref = getPreferredSize();
         Dimension dimNow = getSize();
               
         double dScale = Math.min(dimNow.width/(double)dimPref.width, 
                                  dimNow.height/(double)dimPref.height);
         
         if (dScale != 1.0)
            g2.setRenderingHint(RenderingHints.KEY_INTERPOLATION, 
                                RenderingHints.VALUE_INTERPOLATION_BICUBIC);
                  
         if (!isSelected())
            g.drawImage(m_ImageNormal, 0, 0, dimNow.width, dimNow.height, null);
         else
            g.drawImage(m_ImageSelected, 0, 0, dimNow.width, dimNow.height, null);
          
      }
   }
   
   // exactly double code (above)
   private static class MyCheckBox extends JCheckBox
   {
      private BufferedImage m_ImageNormal = null;
      private BufferedImage m_ImageSelected = null;
      
      void InitImages()
      {
         Dimension dimPref = getPreferredSize();
         
         m_ImageNormal = new BufferedImage(dimPref.width, dimPref.height,
                                           BufferedImage.TYPE_INT_ARGB);
         m_ImageSelected =  new BufferedImage(dimPref.width, dimPref.height,
                                              BufferedImage.TYPE_INT_ARGB);
        
         boolean bIsSelected = isSelected();
         
         if (getWidth() <= 0)
            setSize(dimPref); // make sure something gets painted
         
         setSelected(true);
         super.paintComponent(m_ImageSelected.createGraphics());
         setSelected(false);
         super.paintComponent(m_ImageNormal.createGraphics());
         
         setSelected(bIsSelected);
      }
      
      protected void paintComponent(Graphics g)
      {
         Graphics2D g2 = (Graphics2D)g;
               
         Dimension dimPref = getPreferredSize();
         Dimension dimNow = getSize();
               
         double dScale = Math.min(dimNow.width/(double)dimPref.width, 
                                  dimNow.height/(double)dimPref.height);
         
         if (dScale != 1.0)
            g2.setRenderingHint(RenderingHints.KEY_INTERPOLATION, 
                                RenderingHints.VALUE_INTERPOLATION_BICUBIC);
                  
         if (!isSelected())
            g.drawImage(m_ImageNormal, 0, 0, dimNow.width, dimNow.height, null);
         else
            g.drawImage(m_ImageSelected, 0, 0, dimNow.width, dimNow.height, null);
          
      }
   }
}