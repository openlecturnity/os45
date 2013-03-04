package imc.lecturnity.converter.wizard;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.ProfiledSettings;

class RealScormTemplateWizardPanel extends ScormTemplateWizardPanel
{
   private WizardPanel nextWizard_;

   public RealScormTemplateWizardPanel(PublisherWizardData pwData)
   {
      super(pwData, false);
   }

   protected String getHeaderImage()
   {
      return "/imc/lecturnity/converter/images/real_networks_logo.gif";
   }

   public PublisherWizardData getWizardData()
   {
      PublisherWizardData pwData = super.getWizardData();
      
      if (nextWizard_ == null)
         nextWizard_ = new RealBandwidthWizardPanel(pwData);

      pwData.nextWizardPanel = nextWizard_;

      ProfiledSettings ps = pwData.GetProfiledSettings();

      // with strict SCORM support clips must be combined with the video
      // (this is the "reverse" of the action taken in SelectActionWizardPanel)
      if (ps.GetBoolValue("bScormEnabled") && ps.GetBoolValue("bScormStrict"))
         ps.SetBoolValue("bShowClipsOnSlides", false);

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