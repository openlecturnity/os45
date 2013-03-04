package imc.lecturnity.converter.wizard;

import java.awt.*;
import javax.swing.*;
import java.awt.Font;

import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.converter.StreamInformation;
import imc.lecturnity.util.ui.LayoutUtils;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.WizardData;

import imc.epresenter.filesdk.util.Localizer;


public class WmpFileExtensionPanel extends WizardPanel
{
   private static String USE_WM_SUFFIX = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/WmpFileExtensionPanel_",
             "en");
         
         USE_WM_SUFFIX = l.getLocalized("USE_WM_SUFFIX");

      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database: WmpFileExtensionPanel\n" +
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private ExtendedSettingsDialog m_parent = null;
   private PublisherWizardData m_pwData = null;

   private JCheckBox m_WmSuffixBox;

   public WmpFileExtensionPanel(ExtendedSettingsDialog parent, PublisherWizardData pwData)
   {
      m_parent = parent;
      m_pwData = pwData;

      Font boldFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font normalFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);
      
      //DocumentData dd = m_pwData.GetDocumentData();
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      this.setBorder(BorderFactory.createEmptyBorder(20, 15, 15, 20));
      this.setLayout(new BorderLayout());
      JPanel pnlMajor = new JPanel();
      pnlMajor.setLayout(new BorderLayout());
      this.add("North", pnlMajor);

      m_WmSuffixBox = new JCheckBox(USE_WM_SUFFIX);
      m_WmSuffixBox.setSize(380, 22);
      m_WmSuffixBox.setLocation(30, 250);
      boolean bUseAdvancedWmSuffix = ps.GetBoolValue("bWmUseAdvancedSuffix");
      m_WmSuffixBox.setSelected(bUseAdvancedWmSuffix);
      pnlMajor.add(m_WmSuffixBox);

   }

   public PublisherWizardData getWizardData()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      boolean bWmUseAdvancedSuffix = m_WmSuffixBox.isSelected();
      ps.SetBoolValue("bWmUseAdvancedSuffix", bWmUseAdvancedSuffix);
      m_pwData.SetWmSuffix(bWmUseAdvancedSuffix);

      return m_pwData;
   }

   public String getDescription()
   {
      return "Use extended Windows Media suffix.";
   }
}
