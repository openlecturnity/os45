package imc.lecturnity.util.wizards;

public class WizardData
{
   public WizardPanel nextWizardPanel;
   public WizardPanel finishWizardPanel;

   public void fillOtherWizardData(WizardData otherData)
   {
      otherData.nextWizardPanel = nextWizardPanel;
      otherData.finishWizardPanel = finishWizardPanel;
   }
}
