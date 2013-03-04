package imc.lecturnity.converter.wizard;

import java.awt.event.*;
import javax.swing.*;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.converter.ProfiledSettings;

/**
 * A dialog class for setting 'standard navigation' states
 */
public class FlashStandardNavigationDialog extends JDialog implements ActionListener
{
   protected JCheckBox    m_boxControlBar;
   protected JCheckBox    m_boxStandardButtons;
   protected JCheckBox    m_boxTimeLine;
   protected JCheckBox    m_boxTimeDisplay;
   protected JCheckBox    m_boxDocumentStructure;
   protected JCheckBox    m_boxPluginContextMenu;
   
   protected JRadioButton m_btnControlBarEnabled;
   protected JRadioButton m_btnControlBarDisabled;
   protected JRadioButton m_btnStandardButtonsEnabled;
   protected JRadioButton m_btnStandardButtonsDisabled;
   protected JRadioButton m_btnTimeLineEnabled;
   protected JRadioButton m_btnTimeLineDisabled;
   protected JRadioButton m_btnTimeDisplayEnabled;
   protected JRadioButton m_btnTimeDisplayDisabled;
   protected JRadioButton m_btnDocumentStructureEnabled;
   protected JRadioButton m_btnDocumentStructureDisabled;

   protected PublisherWizardData m_pwData;
   protected static Localizer m_Localizer;
   
 
   public FlashStandardNavigationDialog(PublisherWizardData pwData, Localizer localizer)
   {
      m_pwData = pwData;
      m_Localizer = localizer;
   }
   
   public void actionPerformed(ActionEvent evt)
   {
      // Checkbox 'control bar' 
      // this state affects the states of 'standard buttons', 'time line' and 'time display'
      if ((evt.getSource() == m_boxControlBar) 
         || (evt.getSource() == m_btnControlBarEnabled) 
         || (evt.getSource() == m_btnControlBarDisabled))
      {
         checkControlBar();
      }

      // Checkbox 'standard buttons' 
      // this state affects the state of 'control bar' --> set to 'enabled'
      if ((evt.getSource() == m_boxStandardButtons) 
         || (evt.getSource() == m_btnStandardButtonsEnabled) 
         || (evt.getSource() == m_btnStandardButtonsDisabled))
      {
         enableControlBar();
         checkStandardButtons();         
      }

      // Checkbox 'time line' 
      // this state affects the state of 'control bar' --> set to 'enabled'
      if ((evt.getSource() == m_boxTimeLine) 
         || (evt.getSource() == m_btnTimeLineEnabled) 
         || (evt.getSource() == m_btnTimeLineDisabled))
      {
         enableControlBar();
         checkTimeLine();
      }

      // Checkbox 'time display' 
      // this state affects the state of 'control bar' --> set to 'enabled'
      if ((evt.getSource() == m_boxTimeDisplay) 
         || (evt.getSource() == m_btnTimeDisplayEnabled) 
         || (evt.getSource() == m_btnTimeDisplayDisabled))
      {
         enableControlBar();
         checkTimeDisplay();
      }

      // Checkbox 'document structure' 
      if ((evt.getSource() == m_boxDocumentStructure) 
         || (evt.getSource() == m_btnDocumentStructureEnabled) 
         || (evt.getSource() == m_btnDocumentStructureDisabled))
      {
         checkDocumentStructure();
      }

      // Checkbox 'context menu of plugin' 
      if (evt.getSource() == m_boxPluginContextMenu) 
      {
         checkPluginContextMenu();
      }
   }
   
   /**
    * Does not change members of m_pwData if there is any error.
    */
   protected boolean executeOk()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      ps.SetIntValue("iControlBarVisibility", getStateOfControlBar());
      ps.SetIntValue("iNavigationButtonsVisibility", getStateOfStandardButtons());
      ps.SetIntValue("iTimeLineVisibility", getStateOfTimeLine());
      ps.SetIntValue("iTimeDisplayVisibility", getStateOfTimeDisplay());
      ps.SetIntValue("iDocumentStructureVisibility", getStateOfDocumentStructure());
      ps.SetIntValue("iPluginContextMenuVisibility", getStateOfPluginContextMenu());
      
      return true;
   }
   
   protected String getTabTitle()
   {
      return m_Localizer.getLocalized("STANDARD_NAVIGATION");
   }

   protected JPanel createContentPanel()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      JPanel pnlInner = new JPanel();
      pnlInner.setLayout(null);
      
    
      int x = 20;
      int y = 20;
      int w = 440;
      int h = 20;
      
      boolean bIsVisible = true;
      boolean bIsEnabled = true;
      
      JLabel lblQuestion = new JLabel(m_Localizer.getLocalized("STANDARD_NAVIGATION_LABEL"));
      lblQuestion.setLocation(x, y);
      lblQuestion.setSize(w, h);
      pnlInner.add(lblQuestion);
      
      y += 50;
      
      m_boxControlBar = new JCheckBox(m_Localizer.getLocalized("STANDARD_NAVIGATION_CONTROLBAR"));
      m_boxControlBar.setLocation(x, y);
      m_boxControlBar.setSize(220, h);
      bIsVisible = (ps.GetIntValue("iControlBarVisibility") < 2);
      bIsEnabled = (ps.GetIntValue("iControlBarVisibility") == 0);
      m_boxControlBar.setSelected(bIsVisible);
      m_boxControlBar.addActionListener(this);
      pnlInner.add(m_boxControlBar);

      ButtonGroup bgControlBar = new ButtonGroup();

      m_btnControlBarEnabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_ENABLED"), bIsEnabled);
      m_btnControlBarEnabled.setLocation(x+240, y);
      m_btnControlBarEnabled.setSize(85, h);
      m_btnControlBarEnabled.addActionListener(this);
      if (!bIsVisible)
         m_btnControlBarEnabled.setEnabled(false);
      bgControlBar.add(m_btnControlBarEnabled);
      pnlInner.add(m_btnControlBarEnabled);

      m_btnControlBarDisabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_DISABLED"), !bIsEnabled);
      m_btnControlBarDisabled.setLocation(x+325, y);
      m_btnControlBarDisabled.setSize(120, h);
      m_btnControlBarDisabled.addActionListener(this);
      if (!bIsVisible)
         m_btnControlBarDisabled.setEnabled(false);
      bgControlBar.add(m_btnControlBarDisabled);
      pnlInner.add(m_btnControlBarDisabled);

      y += 30;

      m_boxStandardButtons = new JCheckBox(m_Localizer.getLocalized("STANDARD_NAVIGATION_STANDARD_BUTTONS"));
      m_boxStandardButtons.setLocation(x+20, y);
      m_boxStandardButtons.setSize(200, h);
      bIsVisible = (ps.GetIntValue("iNavigationButtonsVisibility") < 2);
      bIsEnabled = (ps.GetIntValue("iNavigationButtonsVisibility") == 0);
      m_boxStandardButtons.setSelected(bIsVisible);
      m_boxStandardButtons.addActionListener(this);
      pnlInner.add(m_boxStandardButtons);

      ButtonGroup bgStandardButtons = new ButtonGroup();

      m_btnStandardButtonsEnabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_ENABLED"), bIsEnabled);
      m_btnStandardButtonsEnabled.setLocation(x+240, y);
      m_btnStandardButtonsEnabled.setSize(85, h);
      m_btnStandardButtonsEnabled.addActionListener(this);
      if (!bIsVisible)
         m_btnStandardButtonsEnabled.setEnabled(false);
      bgStandardButtons.add(m_btnStandardButtonsEnabled);
      pnlInner.add(m_btnStandardButtonsEnabled);

      m_btnStandardButtonsDisabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_DISABLED"), !bIsEnabled);
      m_btnStandardButtonsDisabled.setLocation(x+325, y);
      m_btnStandardButtonsDisabled.setSize(120, h);
      m_btnStandardButtonsDisabled.addActionListener(this);
      if (!bIsVisible)
         m_btnStandardButtonsDisabled.setEnabled(false);
      bgStandardButtons.add(m_btnStandardButtonsDisabled);
      pnlInner.add(m_btnStandardButtonsDisabled);

      y += 25;

      m_boxTimeLine = new JCheckBox(m_Localizer.getLocalized("STANDARD_NAVIGATION_TIME_LINE"));
      m_boxTimeLine.setLocation(x+20, y);
      m_boxTimeLine.setSize(200, h);
      bIsVisible = (ps.GetIntValue("iTimeLineVisibility") < 2);
      bIsEnabled = (ps.GetIntValue("iTimeLineVisibility") == 0);
      m_boxTimeLine.setSelected(bIsVisible);
      m_boxTimeLine.addActionListener(this);
      pnlInner.add(m_boxTimeLine);

      ButtonGroup bgTimeLine = new ButtonGroup();

      m_btnTimeLineEnabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_ENABLED"), bIsEnabled);
      m_btnTimeLineEnabled.setLocation(x+240, y);
      m_btnTimeLineEnabled.setSize(85, h);
      m_btnTimeLineEnabled.addActionListener(this);
      if (!bIsVisible)
         m_btnTimeLineEnabled.setEnabled(false);
      bgTimeLine.add(m_btnTimeLineEnabled);
      pnlInner.add(m_btnTimeLineEnabled);

      m_btnTimeLineDisabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_DISABLED"), !bIsEnabled);
      m_btnTimeLineDisabled.setLocation(x+325, y);
      m_btnTimeLineDisabled.setSize(120, h);
      m_btnTimeLineDisabled.addActionListener(this);
      if (!bIsVisible)
         m_btnTimeLineDisabled.setEnabled(false);
      bgTimeLine.add(m_btnTimeLineDisabled);
      pnlInner.add(m_btnTimeLineDisabled);

      y += 25;

      m_boxTimeDisplay = new JCheckBox(m_Localizer.getLocalized("STANDARD_NAVIGATION_TIME_DISPLAY"));
      m_boxTimeDisplay.setLocation(x+20, y);
      m_boxTimeDisplay.setSize(200, h);
      bIsVisible = (ps.GetIntValue("iTimeDisplayVisibility") < 2);
      bIsEnabled = (ps.GetIntValue("iTimeDisplayVisibility") == 0);
      m_boxTimeDisplay.setSelected(bIsVisible);
      m_boxTimeDisplay.addActionListener(this);
      pnlInner.add(m_boxTimeDisplay);

      ButtonGroup bgTimeDisplay = new ButtonGroup();

      m_btnTimeDisplayEnabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_ENABLED"), bIsEnabled);
      m_btnTimeDisplayEnabled.setLocation(x+240, y);
      m_btnTimeDisplayEnabled.setSize(85, h);
      m_btnTimeDisplayEnabled.addActionListener(this);
      if (!bIsVisible)
         m_btnTimeDisplayEnabled.setEnabled(false);
      bgTimeDisplay.add(m_btnTimeDisplayEnabled);
      pnlInner.add(m_btnTimeDisplayEnabled);

      m_btnTimeDisplayDisabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_DISABLED"), !bIsEnabled);
      m_btnTimeDisplayDisabled.setLocation(x+325, y);
      m_btnTimeDisplayDisabled.setSize(120, h);
      m_btnTimeDisplayDisabled.addActionListener(this);
      if (!bIsVisible)
         m_btnTimeDisplayDisabled.setEnabled(false);
      bgTimeDisplay.add(m_btnTimeDisplayDisabled);
      pnlInner.add(m_btnTimeDisplayDisabled);
      
      y += 50;
      
      m_boxDocumentStructure = new JCheckBox(m_Localizer.getLocalized("STANDARD_NAVIGATION_DOCUMENT_STRUCTURE"));
      m_boxDocumentStructure.setLocation(x, y);
      m_boxDocumentStructure.setSize(220, h);
      bIsVisible = (ps.GetIntValue("iDocumentStructureVisibility") < 2);
      bIsEnabled = (ps.GetIntValue("iDocumentStructureVisibility") == 0);
      m_boxDocumentStructure.setSelected(bIsVisible);
      m_boxDocumentStructure.addActionListener(this);
      pnlInner.add(m_boxDocumentStructure);

      ButtonGroup bgDocumentStructure = new ButtonGroup();

      m_btnDocumentStructureEnabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_ENABLED"), bIsEnabled);
      m_btnDocumentStructureEnabled.setLocation(x+240, y);
      m_btnDocumentStructureEnabled.setSize(85, h);
      m_btnDocumentStructureEnabled.addActionListener(this);
      if (!bIsVisible)
         m_btnDocumentStructureEnabled.setEnabled(false);
      bgDocumentStructure.add(m_btnDocumentStructureEnabled);
      pnlInner.add(m_btnDocumentStructureEnabled);

      m_btnDocumentStructureDisabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_DISABLED"), !bIsEnabled);
      m_btnDocumentStructureDisabled.setLocation(x+325, y);
      m_btnDocumentStructureDisabled.setSize(120, h);
      m_btnDocumentStructureDisabled.addActionListener(this);
      if (!bIsVisible)
         m_btnDocumentStructureDisabled.setEnabled(false);
      bgDocumentStructure.add(m_btnDocumentStructureDisabled);
      pnlInner.add(m_btnDocumentStructureDisabled);
      
      y += 30;
      
      m_boxPluginContextMenu = new JCheckBox(m_Localizer.getLocalized("STANDARD_NAVIGATION_PLUGIN_CONTEXTMENU"));
      m_boxPluginContextMenu.setLocation(x, y);
      m_boxPluginContextMenu.setSize(220, h);
      bIsVisible = (ps.GetIntValue("iPluginContextMenuVisibility") < 2);
      m_boxPluginContextMenu.setSelected(bIsVisible);
      m_boxPluginContextMenu.addActionListener(this);
      pnlInner.add(m_boxPluginContextMenu);


      // Only "New Flash layer" documents have a 'Document structure'
      if (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX)
      {
         showDocumentStructure(true);
         // Disable "PluginContextMenuVisibility", because this Feature is not available 
         // any more in Flash Player 9 
         showPluginContextMenu(false);
      }
      else
      {
         showDocumentStructure(false);
      }
      
      
      return pnlInner;
   }

   protected void showDocumentStructure(boolean bEnabled)
   {
      m_boxDocumentStructure.setVisible(bEnabled);
      m_btnDocumentStructureEnabled.setVisible(bEnabled);
      m_btnDocumentStructureDisabled.setVisible(bEnabled);
   }
   
   protected void showPluginContextMenu(boolean bEnabled)
   {
      m_boxPluginContextMenu.setVisible(bEnabled);
   }
   
   protected void checkControlBar()
   {
      // state of 'control bar'
      int iControlBar = getStateOfControlBar();

      switch (iControlBar)
      {
         case 0:  // enabled
                  setStateOfControlBarDependentStuff(true);
                  m_boxStandardButtons.setSelected(true);
                  m_btnStandardButtonsEnabled.setSelected(true);
                  m_boxTimeLine.setSelected(true);
                  m_btnTimeLineEnabled.setSelected(true);
                  m_boxTimeDisplay.setSelected(true);
                  m_btnTimeDisplayEnabled.setSelected(true);
                  break;
         case 1:  // disabled
                  setStateOfControlBarDependentStuff(true);
                  m_boxStandardButtons.setSelected(true);
                  m_btnStandardButtonsDisabled.setSelected(true);
                  m_boxTimeLine.setSelected(true);
                  m_btnTimeLineDisabled.setSelected(true);
                  m_boxTimeDisplay.setSelected(true);
                  m_btnTimeDisplayDisabled.setSelected(true);
                  break;
         case 2:  // hidden
                  setStateOfControlBarDependentStuff(false);
                  m_boxStandardButtons.setSelected(false);
                  m_boxTimeLine.setSelected(false);
                  m_boxTimeDisplay.setSelected(false);
                  break;
      }
   }

   protected void checkStandardButtons()
   {
      // state of 'standard buttons'
      int iStandardButtons = getStateOfStandardButtons();

      switch (iStandardButtons)
      {
         case 0:  // enabled
                  m_btnStandardButtonsEnabled.setEnabled(true);
                  m_btnStandardButtonsDisabled.setEnabled(true);
                  break;
         case 1:  // disabled
                  m_btnStandardButtonsEnabled.setEnabled(true);
                  m_btnStandardButtonsDisabled.setEnabled(true);
                  break;
         case 2:  // hidden
                  m_btnStandardButtonsEnabled.setEnabled(false);
                  m_btnStandardButtonsDisabled.setEnabled(false);
                  break;
      }
   }

   protected void checkTimeLine()
   {
      // state of 'time line'
      int iTimeLine = getStateOfTimeLine();

      switch (iTimeLine)
      {
         case 0:  // enabled
                  m_btnTimeLineEnabled.setEnabled(true);
                  m_btnTimeLineDisabled.setEnabled(true);
                  break;
         case 1:  // disabled
                  m_btnTimeLineEnabled.setEnabled(true);
                  m_btnTimeLineDisabled.setEnabled(true);
                  break;
         case 2:  // hidden
                  m_btnTimeLineEnabled.setEnabled(false);
                  m_btnTimeLineDisabled.setEnabled(false);
                  break;
      }
   }

   protected void checkTimeDisplay()
   {
      // state of 'time display'
      int iTimeDisplay = getStateOfTimeDisplay();

      switch (iTimeDisplay)
      {
         case 0:  // enabled
                  m_btnTimeDisplayEnabled.setEnabled(true);
                  m_btnTimeDisplayDisabled.setEnabled(true);
                  break;
         case 1:  // disabled
                  m_btnTimeDisplayEnabled.setEnabled(true);
                  m_btnTimeDisplayDisabled.setEnabled(true);
                  break;
         case 2:  // hidden
                  m_btnTimeDisplayEnabled.setEnabled(false);
                  m_btnTimeDisplayDisabled.setEnabled(false);
                  break;
      }
   }

   protected void checkDocumentStructure()
   {
      // state of 'document structure'
      int iDocumentStructure = getStateOfDocumentStructure();

      switch (iDocumentStructure)
      {
         case 0:  // enabled
                  m_btnDocumentStructureEnabled.setEnabled(true);
                  m_btnDocumentStructureDisabled.setEnabled(true);
                  break;
         case 1:  // disabled
                  m_btnDocumentStructureEnabled.setEnabled(true);
                  m_btnDocumentStructureDisabled.setEnabled(true);
                  break;
         case 2:  // hidden
                  m_btnDocumentStructureEnabled.setEnabled(false);
                  m_btnDocumentStructureDisabled.setEnabled(false);
                  break;
      }
   }

   protected void checkPluginContextMenu()
   {
      // state of 'context menu of plugin'
      // (Nothing to do here)
   }

   protected int getStateOfControlBar()
   {
      int iControlBar = 0;
      if (m_boxControlBar.isSelected())
      {
         if (m_btnControlBarEnabled.isSelected())
            iControlBar = 0;
         else
            iControlBar = 1;
      }
      else
         iControlBar = 2;
         
      return iControlBar;
   }

   protected int getStateOfStandardButtons()
   {
      int iStandardButtons = 0;
      if (m_boxStandardButtons.isSelected())
      {
         if (m_btnStandardButtonsEnabled.isSelected())
            iStandardButtons = 0;
         else
            iStandardButtons = 1;
      }
      else
         iStandardButtons = 2;
         
      return iStandardButtons;
   }

   protected int getStateOfTimeLine()
   {
      int iTimeLine = 0;
      if (m_boxTimeLine.isSelected())
      {
         if (m_btnTimeLineEnabled.isSelected())
            iTimeLine = 0;
         else
            iTimeLine = 1;
      }
      else
         iTimeLine = 2;
         
      return iTimeLine;
   }

   protected int getStateOfTimeDisplay()
   {
      int iTimeDisplay = 0;
      if (m_boxTimeDisplay.isSelected())
      {
         if (m_btnTimeDisplayEnabled.isSelected())
            iTimeDisplay = 0;
         else
            iTimeDisplay = 1;
      }
      else
         iTimeDisplay = 2;
         
      return iTimeDisplay;
   }

   protected int getStateOfDocumentStructure()
   {
      int iDocumentStructure = 0;
      if (m_boxDocumentStructure.isSelected())
      {
         if (m_btnDocumentStructureEnabled.isSelected())
            iDocumentStructure = 0;
         else
            iDocumentStructure = 1;
      }
      else
         iDocumentStructure = 2;
         
      return iDocumentStructure;
   }

   protected int getStateOfPluginContextMenu()
   {
      int iPluginContextMenu = 0;
      if (!m_boxPluginContextMenu.isSelected())
         iPluginContextMenu = 2;
         
      return iPluginContextMenu;
   }

   protected void setStateOfControlBarDependentStuff(boolean bEnabled)
   {
      m_btnControlBarEnabled.setEnabled(bEnabled);
      m_btnControlBarDisabled.setEnabled(bEnabled);
      m_boxStandardButtons.setEnabled(bEnabled);
      m_btnStandardButtonsEnabled.setEnabled(bEnabled);
      m_btnStandardButtonsDisabled.setEnabled(bEnabled);
      m_boxTimeLine.setEnabled(bEnabled);
      m_btnTimeLineEnabled.setEnabled(bEnabled);
      m_btnTimeLineDisabled.setEnabled(bEnabled);
      m_boxTimeDisplay.setEnabled(bEnabled);
      m_btnTimeDisplayEnabled.setEnabled(bEnabled);
      m_btnTimeDisplayDisabled.setEnabled(bEnabled);
   }

   protected void enableControlBar()
   {
      m_btnControlBarEnabled.setEnabled(true);
      m_btnControlBarDisabled.setEnabled(true);
      m_boxControlBar.setSelected(true);
      m_btnControlBarEnabled.setSelected(true);
   }
}
