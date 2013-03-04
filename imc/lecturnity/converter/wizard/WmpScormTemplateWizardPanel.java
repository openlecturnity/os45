package imc.lecturnity.converter.wizard;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.ProfiledSettings;

class WmpScormTemplateWizardPanel extends ScormTemplateWizardPanel
{
   private WizardPanel nextWizard_;

   public WmpScormTemplateWizardPanel(PublisherWizardData pwData)
   {
      super(pwData);
   }

   protected String getHeaderImage()
   {
      return "/imc/lecturnity/converter/images/windowsmedia_logo.png";
   }

   public PublisherWizardData getWizardData()
   {
      PublisherWizardData pwData = super.getWizardData();

      if (nextWizard_ == null)
         nextWizard_ = new WmpEncoderWizardPanel(pwData);

      pwData.nextWizardPanel = nextWizard_;

      ProfiledSettings ps = pwData.GetProfiledSettings();

      // Get current Template setttings
      String strPath = LPLibs.templateGetActivePath();
      ps.SetStringValue("strRmWmTemplateConfigFilePath", strPath);
      String strSettings = LPLibs.templateGetSettings();
      ps.SetStringValue("strRmWmTemplateConfigParameterList", strSettings);
      // Bugfix 5326: Check for "HideWebPlayerLogo" flag in Template settings
      if (strSettings.indexOf("%LecturnityWebPlayerLogo%:1:1") >= 0) {
         // This flag is set to "hidden"
         ps.SetBoolValue("bHideLecturnityIdentity", true);
      }

      return pwData;
   }
}
