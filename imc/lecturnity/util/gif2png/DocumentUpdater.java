package imc.lecturnity.util.gif2png;

import imc.lecturnity.util.VersionExtracter;

import imc.lecturnity.util.wizards.WizardListener;
import imc.lecturnity.util.wizards.WizardEvent;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.Wizard;

public class DocumentUpdater implements WizardListener
{
   public static String VERSION = "?.?.?";

   public static void main(String[] args)
   {
      // retrieve version:
      VersionExtracter ve = new VersionExtracter
         ("/imc/lecturnity/util/gif2png/VERSION");
      VERSION = ve.getVersion();

      if (args.length > 0)
      {
         new DocumentUpdater(args[0]);
      }
      else
      {
         new DocumentUpdater();
      }
   }

   private DocumentUpdater()
   {
      this(null);
   }

   private DocumentUpdater(String arg)
   {
      try
      {
         javax.swing.UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
      }
      catch (Exception e)
      {
         // if we're not on windows, the above code will not work.
      }

      UpdaterData data = new UpdaterData();
      data.fileName = arg;

      WizardPanel wizardPanel;
      if (arg != null)
         wizardPanel = new UpdaterSelectWizardPanel(data);
      else
         wizardPanel = new UpdaterWelcomeWizardPanel(data);
      new Wizard(wizardPanel, this, null, wizardPanel.getFrameIcon());
   }

   public void wizardFinished(WizardEvent e)
   {
      System.exit(0);
   }
}
