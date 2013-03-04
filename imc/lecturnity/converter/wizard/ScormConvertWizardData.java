package imc.lecturnity.converter.wizard;

class ScormConvertWizardData extends ConvertWizardData
{
   /** SCORM Variables */
   /** true if SCORM is enabled */
   public boolean scormEnabled = false;
   /** true if strict SCORM is enabled (media data in package) */
   public boolean scormStrict = false;

   public void fillOtherWizardData(ConvertWizardData otherData)
   {
      if (otherData instanceof ScormConvertWizardData)
      {
         ScormConvertWizardData that = (ScormConvertWizardData) otherData;
         
         that.scormEnabled = this.scormEnabled;
         that.scormStrict = this.scormStrict;
      }

      super.fillOtherWizardData(otherData);
   }
}