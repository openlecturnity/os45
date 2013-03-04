package imc.lecturnity.converter.wizard;

import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.io.IOException;
import javax.swing.*;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import javax.swing.filechooser.FileFilter;

import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.util.ImagePanel;
import imc.lecturnity.util.ui.LayoutUtils;
import imc.lecturnity.util.wizards.WizardTextArea;



/**
 * Flash movie size dialog
 */
public class FlashSizeDialog extends FlashAbstractExtendedDialog implements ActionListener, ChangeListener
{
   private JRadioButton m_btnFlexible;
   private JRadioButton m_btnOptimizedRes;
   private JRadioButton m_btnOptimizedCust;
   private CustomSizePanel m_pnlCustomSize;
   private JComboBox m_boxResolutions;
   private JPanel m_pnlWarningArea;
   private WizardTextArea m_txtPageSize;
   
   
   public FlashSizeDialog(Frame parent, FlashConvertWizardData data)
   {
      super(parent, data, g_Localizer.getLocalized("DIALOG_TITLE_SIZE"));
   }
   
   public void actionPerformed(ActionEvent evt)
   {
      super.actionPerformed(evt); // handle ok and cancel
      
      if (evt.getSource() == m_btnFlexible || evt.getSource() == m_btnOptimizedRes || evt.getSource() == m_btnOptimizedCust)
         updateGui();
   }

   public void stateChanged(ChangeEvent evt) 
   {
      if (evt.getSource() == m_pnlCustomSize)
         updateGui();
   }

   /**
    * Does not change members in m_WizardData if an error is displayed.
    */
   protected boolean executeOk()
   {
      FlashConvertWizardData data = m_WizardData;
            
      if (!m_btnFlexible.isSelected())
      {
         if (m_btnOptimizedRes.isSelected())
         {
            String strSize = (String)m_boxResolutions.getSelectedItem();
            int iWidth = Integer.parseInt(strSize.substring(0, strSize.indexOf("x")));
            int iHeight = Integer.parseInt(strSize.substring(strSize.indexOf("x")+1));
                  
            data.m_OutputMovieSize = new Dimension(iWidth, iHeight);
            
            data.m_bOptimizeForScreen = true;
            data.m_bOptimizeForUserSize = false;
         }
         else
         {
            Dimension d = m_pnlCustomSize.getActualSize();
            if (d.width < 1 || d.height < 1)
            {
               JOptionPane.showMessageDialog(this, g_Localizer.getLocalized("SPECIFY_SIZE"),
                                             g_Localizer.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
               return false;
            }
                  
            Dimension dimSmaller = data.subtractScaledFlashBorders(d);
            double scaleX = (float)dimSmaller.width / (float)data.whiteboardSize.width;
            double scaleY = (float)dimSmaller.height / (float)data.whiteboardSize.height;
            data.m_dWbScale = (scaleX < scaleY) ? scaleX : scaleY;
            
            data.m_OutputMovieSize = d;
            
            data.m_bOptimizeForScreen = false;
            data.m_bOptimizeForUserSize = true;
         }
      }
      else
      {
         data.m_bOptimizeForScreen = false;
         data.m_bOptimizeForUserSize = false;
      }
    
      return true;
   }
   
   protected String getTabTitle()
   {
      return g_Localizer.getLocalized("RESOLUTION");
   }
 
   protected JPanel createContentPanel()
   {
      String[] arrResolutions = new String[] { "640x480", "800x600", "1024x768", 
         "1152x864", "1280x960", "1280x1024", "1400x1050", "1600x1200" };
      int iSelectedIndex = 2;
       
      FlashConvertWizardData data = m_WizardData;
      
      
      boolean bOptimized = data.m_bOptimizeForScreen || data.m_bOptimizeForUserSize;
      boolean bResolutionMatch = data.m_bOptimizeForScreen;
      
      if (bResolutionMatch && data.m_OutputMovieSize != null)
      {
         String strFind = data.m_OutputMovieSize.width+"x"+data.m_OutputMovieSize.height;
         
         for (int i=0; i<arrResolutions.length; ++i)
         {
            if (arrResolutions[i].equals(strFind))
            {
               iSelectedIndex = i;
               break;
            }
         }
      }
      
      JPanel pnlInner = new JPanel();
      pnlInner.setLayout(null);
      
    
      int x = 20;
      int y = 20;
      int w = 440;
      int h = 20;
      
      
      m_btnFlexible = new JRadioButton(g_Localizer.getLocalized("VIDEO_FLEXIBLE"));
      m_btnFlexible.setSize(w, h);
      m_btnFlexible.setLocation(x, y);
      m_btnFlexible.addActionListener(this);
      pnlInner.add(m_btnFlexible);
      
      y += 70;
      
      m_btnOptimizedRes = new JRadioButton(g_Localizer.getLocalized("VIDEO_OPTIMIZE_SCREEN"));
      m_btnOptimizedRes.setSize(w, h);
      m_btnOptimizedRes.setLocation(x, y);
      m_btnOptimizedRes.addActionListener(this);
      pnlInner.add(m_btnOptimizedRes);
      
      y += 30;
      
      JLabel lblScreen = new JLabel(g_Localizer.getLocalized("SCREEN"));
      lblScreen.setSize(80, h);
      lblScreen.setLocation(x+20, y);
      pnlInner.add(lblScreen);
      
     
      m_boxResolutions = new JComboBox(arrResolutions);
      m_boxResolutions.setSize(100, h);
      m_boxResolutions.setLocation(x+80+10, y);
      m_boxResolutions.setSelectedIndex(iSelectedIndex);
      pnlInner.add(m_boxResolutions);
      
      y += 40;
      
      m_btnOptimizedCust = new JRadioButton(g_Localizer.getLocalized("VIDEO_OPTIMIZE_SIZE"));
      m_btnOptimizedCust.setSize(w, h);
      m_btnOptimizedCust.setLocation(x, y);
      m_btnOptimizedCust.addActionListener(this);
      pnlInner.add(m_btnOptimizedCust);
      
      y += 30;
      
      
      // use the whiteboard size plus the flash borders for the aspect ratio:
      Dimension dimGreater = data.getWbSizeWithBorders();
     
      int displayW = dimGreater.width;
      int displayH = dimGreater.height;
      if (bOptimized && !bResolutionMatch && data.m_OutputMovieSize != null)
      {
         displayW = data.m_OutputMovieSize.width;
         displayH = data.m_OutputMovieSize.height;
      }
      
      // first two parameters are the size of the panel
      // the third parameter is/are the values used for the aspect ratio
      m_pnlCustomSize = new CustomSizePanel(320, h, new Dimension(dimGreater.width, dimGreater.height));
      m_pnlCustomSize.setLocation(x+15, y);
      m_pnlCustomSize.setActualSize(displayW, displayH); // these are the displayed values
      m_pnlCustomSize.configureAspectRatioBoxVisibility(false, true);
      m_pnlCustomSize.addChangeListener(this);
      pnlInner.add(m_pnlCustomSize);
      
      y += 30;

      JPanel pnlPageSizeArea = new JPanel();
      pnlPageSizeArea.setLayout(null);
      pnlPageSizeArea.setLocation(x, y);
      pnlPageSizeArea.setSize(w, 35);

      Dimension dimSmaller = data.subtractScaledFlashBorders(dimGreater);
      m_txtPageSize = new WizardTextArea(
         "(" + g_Localizer.getLocalized("ACCORDING_PAGE_SIZE") + " " + dimSmaller.width + "x" + dimSmaller.height + ")",
         new Point(15, 0), new Dimension(w, 35),
         imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      pnlPageSizeArea.add(m_txtPageSize);
      
      pnlInner.add(pnlPageSizeArea);

      y += 35;

      m_pnlWarningArea = new JPanel();
      m_pnlWarningArea.setLayout(null);
      m_pnlWarningArea.setLocation(x, y);
      m_pnlWarningArea.setSize(w, 70);
      
      JPanel pnlWarningIcon = new ImagePanel("/imc/lecturnity/converter/images/information.gif");
      pnlWarningIcon.setSize(32, 32);
      pnlWarningIcon.setLocation(0, 0);
      m_pnlWarningArea.add(pnlWarningIcon);
      
      
      WizardTextArea txtBorder = new WizardTextArea(
         g_Localizer.getLocalized("NOTE_OTHER_VALUE"),
         new Point(35+10, 0), new Dimension(w-75, 70),
         imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      m_pnlWarningArea.add(txtBorder);
      
      pnlInner.add(m_pnlWarningArea);
      
      
      m_btnFlexible.setSelected(!bOptimized);
      m_btnOptimizedRes.setSelected(bOptimized && bResolutionMatch);
      m_btnOptimizedCust.setSelected(bOptimized && !bResolutionMatch);
      
      updateGui();
      

      
      ButtonGroup bg = new ButtonGroup();
      bg.add(m_btnFlexible);
      bg.add(m_btnOptimizedRes);
      bg.add(m_btnOptimizedCust);
      
 
      
      return pnlInner;
   }
   
   private void updateGui()
   {
      boolean bOptimized = !m_btnFlexible.isSelected();
      boolean bResolutionMatch = m_btnOptimizedRes.isSelected();

      m_boxResolutions.setEnabled(bOptimized && bResolutionMatch);
      LayoutUtils.enableContainer(m_pnlCustomSize, bOptimized && !bResolutionMatch);
      m_pnlCustomSize.configureAspectRatioBoxVisibility(false, true);
      m_pnlWarningArea.setVisible(bOptimized && !bResolutionMatch);

      if (bOptimized && !bResolutionMatch)
         m_txtPageSize.enable();
      else
         m_txtPageSize.disable();
         
      Dimension dimSmaller = m_WizardData.subtractScaledFlashBorders(m_pnlCustomSize.getActualSize());
      m_txtPageSize.setText("(" + g_Localizer.getLocalized("ACCORDING_PAGE_SIZE") + " " + dimSmaller.width + "x" + dimSmaller.height + ")");
      m_txtPageSize.setVisible(true);
   }
}