package imc.lecturnity.converter.wizard;

import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.io.IOException;
import javax.swing.*;
import javax.swing.filechooser.FileFilter;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.ui.DirectoryChooser;
import imc.lecturnity.util.ui.LayoutUtils;
import imc.lecturnity.util.wizards.WizardTextArea;


/**
 * A dialog class for setting 'standard navigation' states
 * It is derived from FlashStandardNavigationDialog and has 2 additionally settings
 */
public class LecturnityStandardNavigationDialog extends FlashStandardNavigationDialog implements ActionListener
{
   private JCheckBox    m_boxPlayerSearchField;
   private JCheckBox    m_boxPlayerConfigButtons;
   
   private JRadioButton m_btnPlayerSearchFieldEnabled;
   private JRadioButton m_btnPlayerSearchFieldDisabled;
   private JRadioButton m_btnPlayerConfigButtonsEnabled;
   private JRadioButton m_btnPlayerConfigButtonsDisabled;
   private JLabel       m_lblPlayerConfigButtons;


   public LecturnityStandardNavigationDialog(PublisherWizardData pwData, Localizer localizer)
   {
      super(pwData, localizer);
   }
   
   public void actionPerformed(ActionEvent evt)
   {
      super.actionPerformed(evt);

      // Checkbox 'control bar' 
      // this state affects also the state of 'Player Configuration Buttons'
      if ((evt.getSource() == m_boxControlBar) 
         || (evt.getSource() == m_btnControlBarEnabled) 
         || (evt.getSource() == m_btnControlBarDisabled))
      {
         checkMyControlBar();
      }
      
      // Checkbox 'Player Search field' 
      if ((evt.getSource() == m_boxPlayerSearchField) 
         || (evt.getSource() == m_btnPlayerSearchFieldEnabled) 
         || (evt.getSource() == m_btnPlayerSearchFieldDisabled))
      {
         checkPlayerSearchField();
      }

      // Checkbox 'Player Configuration buttons' 
      // this state affects the state of 'control bar' --> set to 'enabled'
      if ((evt.getSource() == m_boxPlayerConfigButtons) 
         || (evt.getSource() == m_btnPlayerConfigButtonsEnabled) 
         || (evt.getSource() == m_btnPlayerConfigButtonsDisabled))
      {
         enableControlBar();
         checkPlayerConfigButtons();
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
      ps.SetIntValue("iPlayerSearchFieldVisibility", getStateOfPlayerSearchField());
      ps.SetIntValue("iPlayerConfigButtonsVisibility", getStateOfPlayerConfigButtons());

      return true;
   }
   
   protected JPanel createContentPanel()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      JPanel pnlInner = super.createContentPanel();

      int x = 20;
      int y = 20 + 50 + 30 + 25 + 25 + 25 + 50;
      int w = 440;
      int h = 20;
      
      boolean bIsVisible;
      boolean bIsEnabled;
      
      // 'Document structure' is hidden in Flash:
      // --> show it.
      m_boxDocumentStructure.setLocation(x, y+20);
      m_btnDocumentStructureEnabled.setLocation(x+240, y+20);
      m_btnDocumentStructureDisabled.setLocation(x+325, y+20);
      showDocumentStructure(true);
      
      // 'Plugin Context Menu' is not part of LPDs --> hide it
      showPluginContextMenu(false);
      
      y += 55;

      // Player search field
      m_boxPlayerSearchField = new JCheckBox(m_Localizer.getLocalized("STANDARD_NAVIGATION_PLAYER_SEARCH_FIELD"));
      m_boxPlayerSearchField.setLocation(x, y);
      m_boxPlayerSearchField.setSize(230, h);
      bIsVisible = (ps.GetIntValue("iPlayerSearchFieldVisibility") < 2);
      bIsEnabled = (ps.GetIntValue("iPlayerSearchFieldVisibility") == 0);
      m_boxPlayerSearchField.setSelected(bIsVisible);
      m_boxPlayerSearchField.addActionListener(this);
      pnlInner.add(m_boxPlayerSearchField);

      ButtonGroup bgPlayerSearchField = new ButtonGroup();

      m_btnPlayerSearchFieldEnabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_ENABLED"), bIsEnabled);
      m_btnPlayerSearchFieldEnabled.setLocation(x+240, y);
      m_btnPlayerSearchFieldEnabled.setSize(85, h);
      m_btnPlayerSearchFieldEnabled.addActionListener(this);
      if (!bIsVisible)
         m_btnPlayerSearchFieldEnabled.setEnabled(false);
      bgPlayerSearchField.add(m_btnPlayerSearchFieldEnabled);
      pnlInner.add(m_btnPlayerSearchFieldEnabled);

      m_btnPlayerSearchFieldDisabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_DISABLED"), !bIsEnabled);
      m_btnPlayerSearchFieldDisabled.setLocation(x+325, y);
      m_btnPlayerSearchFieldDisabled.setSize(120, h);
      m_btnPlayerSearchFieldDisabled.addActionListener(this);
      if (!bIsVisible)
         m_btnPlayerSearchFieldDisabled.setEnabled(false);
      bgPlayerSearchField.add(m_btnPlayerSearchFieldDisabled);
      pnlInner.add(m_btnPlayerSearchFieldDisabled);

      y += 40;

      // Player Configuration buttons
      m_boxPlayerConfigButtons = new JCheckBox(m_Localizer.getLocalized("STANDARD_NAVIGATION_PLAYER_CONFIG_BUTTONS1"));
      m_boxPlayerConfigButtons.setLocation(x+20, y-145);
      m_boxPlayerConfigButtons.setSize(230-20, h-2);
      bIsVisible = (ps.GetIntValue("iPlayerConfigButtonsVisibility") < 2);
      bIsEnabled = (ps.GetIntValue("iPlayerConfigButtonsVisibility") == 0);
      m_boxPlayerConfigButtons.setSelected(bIsVisible);
      m_boxPlayerConfigButtons.addActionListener(this);
      pnlInner.add(m_boxPlayerConfigButtons);
      // The text of STANDARD_NAVIGATION_PLAYER_CONFIG_BUTTONS takes 2 lines
      m_lblPlayerConfigButtons = new JLabel(m_Localizer.getLocalized("STANDARD_NAVIGATION_PLAYER_CONFIG_BUTTONS2"));
      m_lblPlayerConfigButtons.setLocation(x+18+20, y+h-2-145);
      m_lblPlayerConfigButtons.setSize(230-20, h);
      pnlInner.add(m_lblPlayerConfigButtons);

      ButtonGroup bgPlayerConfigButtons = new ButtonGroup();

      m_btnPlayerConfigButtonsEnabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_ENABLED"), bIsEnabled);
      m_btnPlayerConfigButtonsEnabled.setLocation(x+240, y-145);
      m_btnPlayerConfigButtonsEnabled.setSize(85, h);
      m_btnPlayerConfigButtonsEnabled.addActionListener(this);
      if (!bIsVisible)
         m_btnPlayerConfigButtonsEnabled.setEnabled(false);
      bgPlayerConfigButtons.add(m_btnPlayerConfigButtonsEnabled);
      pnlInner.add(m_btnPlayerConfigButtonsEnabled);

      m_btnPlayerConfigButtonsDisabled = new JRadioButton(m_Localizer.getLocalized("STANDARD_NAVIGATION_DISABLED"), !bIsEnabled);
      m_btnPlayerConfigButtonsDisabled.setLocation(x+325, y-145);
      m_btnPlayerConfigButtonsDisabled.setSize(120, h);
      m_btnPlayerConfigButtonsDisabled.addActionListener(this);
      if (!bIsVisible)
         m_btnPlayerConfigButtonsDisabled.setEnabled(false);
      bgPlayerConfigButtons.add(m_btnPlayerConfigButtonsDisabled);
      pnlInner.add(m_btnPlayerConfigButtonsDisabled);

      return pnlInner;
   }


   private void checkPlayerSearchField()
   {
      // state of 'player search field'
      int iPlayerSearchField = getStateOfPlayerSearchField();

      switch (iPlayerSearchField)
      {
         case 0:  // enabled
                  m_btnPlayerSearchFieldEnabled.setEnabled(true);
                  m_btnPlayerSearchFieldDisabled.setEnabled(true);
                  break;
         case 1:  // disabled
                  m_btnPlayerSearchFieldEnabled.setEnabled(true);
                  m_btnPlayerSearchFieldDisabled.setEnabled(true);
                  break;
         case 2:  // hidden
                  m_btnPlayerSearchFieldEnabled.setEnabled(false);
                  m_btnPlayerSearchFieldDisabled.setEnabled(false);
                  break;
      }
   }

   private void checkPlayerConfigButtons()
   {
      // state of 'player configuration buttons'
      int iPlayerConfigButtons = getStateOfPlayerConfigButtons();

      switch (iPlayerConfigButtons)
      {
         case 0:  // enabled
                  m_btnPlayerConfigButtonsEnabled.setEnabled(true);
                  m_btnPlayerConfigButtonsDisabled.setEnabled(true);
                  break;
         case 1:  // disabled
                  m_btnPlayerConfigButtonsEnabled.setEnabled(true);
                  m_btnPlayerConfigButtonsDisabled.setEnabled(true);
                  break;
         case 2:  // hidden
                  m_btnPlayerConfigButtonsEnabled.setEnabled(false);
                  m_btnPlayerConfigButtonsDisabled.setEnabled(false);
                  break;
      }
   }

   private int getStateOfPlayerSearchField()
   {
      int iPlayerSearchField = 0;
      if (m_boxPlayerSearchField.isSelected())
      {
         if (m_btnPlayerSearchFieldEnabled.isSelected())
            iPlayerSearchField = 0;
         else
            iPlayerSearchField = 1;
      }
      else
         iPlayerSearchField = 2;
         
      return iPlayerSearchField;
   }

   private int getStateOfPlayerConfigButtons()
   {
      int iPlayerConfigButtons = 0;
      if (m_boxPlayerConfigButtons.isSelected())
      {
         if (m_btnPlayerConfigButtonsEnabled.isSelected())
            iPlayerConfigButtons = 0;
         else
            iPlayerConfigButtons = 1;
      }
      else
         iPlayerConfigButtons = 2;
         
      return iPlayerConfigButtons;
   }

   private void checkMyControlBar()
   {
      // state of 'control bar'
      int iControlBar = getStateOfControlBar();

      switch (iControlBar)
      {
         case 0:  // enabled
                  setMyStateOfControlBarDependentStuff(true);
                  m_boxPlayerConfigButtons.setSelected(true);
                  m_btnPlayerConfigButtonsEnabled.setSelected(true);
                  break;
         case 1:  // disabled
                  setMyStateOfControlBarDependentStuff(true);
                  m_boxPlayerConfigButtons.setSelected(true);
                  m_btnPlayerConfigButtonsDisabled.setSelected(true);
                  break;
         case 2:  // hidden
                  setMyStateOfControlBarDependentStuff(false);
                  m_boxPlayerConfigButtons.setSelected(false);
                  break;
      }
   }

   private void setMyStateOfControlBarDependentStuff(boolean bEnabled)
   {
      m_boxPlayerConfigButtons.setEnabled(bEnabled);
      m_btnPlayerConfigButtonsEnabled.setEnabled(bEnabled);
      m_btnPlayerConfigButtonsDisabled.setEnabled(bEnabled);
      m_lblPlayerConfigButtons.setEnabled(bEnabled);
   }

}
