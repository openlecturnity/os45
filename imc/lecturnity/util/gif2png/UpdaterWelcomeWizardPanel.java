package imc.lecturnity.util.gif2png;

import java.awt.*;
import java.io.IOException;

import javax.swing.JLabel;
import javax.swing.JOptionPane;

import imc.lecturnity.util.ImagePanel;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.Wizard;
import imc.lecturnity.util.wizards.WizardData;
import imc.lecturnity.util.wizards.WizardTextArea;
import imc.epresenter.converter.PresentationConverter;
import imc.epresenter.filesdk.util.Localizer;

class UpdaterWelcomeWizardPanel extends WizardPanel
{
   private static String CAPTION = "[!]";
   private static String HEADLINE = "[!]";
   private static String CONTENT = "[!]";
   private static String COPYRIGHT = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/util/gif2png/UpdaterWelcomeWizardPanel_", "en");

         CAPTION = l.getLocalized("CAPTION");
         HEADLINE = l.getLocalized("HEADLINE");
         CONTENT = l.getLocalized("CONTENT");
         COPYRIGHT = l.getLocalized("COPYRIGHT");
      }
      catch (Exception e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog
            (null, "Localizer error in imc.lecturnity.util.gif2png.UpdaterWelcomeWizardPanel:\n" +
             e.toString(), "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(0);
      }
   }

   private UpdaterData data_;
   private WizardPanel nextWizard_;

   public UpdaterWelcomeWizardPanel(UpdaterData data)
   {
      super();

      data_ = data;

      setPreferredSize(new Dimension(500, 400));
      addButtons(NEXT_BACK_CANCEL);
      setEnableButton(BUTTON_BACK, false);
//       useHeaderPanel("Flupp", "Stupp", null);

      Container r = getContentPanel();
      r.setLayout(null);
      r.setBackground(Color.white);

      ImagePanel imagePanel = new ImagePanel
         ("/imc/lecturnity/converter/images/wizard_left_side.gif");
      imagePanel.setLocation(10, 10);
      imagePanel.setSize(imagePanel.getPreferredSize());

      r.add(imagePanel);

      int textWidth = 500 - 60 - imagePanel.getPreferredSize().width;
      int textX     = 20 + imagePanel.getPreferredSize().width;

      JLabel headLabel = new JLabel(HEADLINE);
      headLabel.setFont(new Font("SansSerif", Font.BOLD, 14));
      headLabel.setSize(textWidth, 22);
      headLabel.setLocation(textX, 20);
      r.add(headLabel);

      WizardTextArea content = new WizardTextArea(CONTENT, new Point(textX, 60),
                                                  new Dimension(textWidth, 200),
                                                  NORMAL_FONT);
      r.add(content);

      WizardTextArea copyright = new WizardTextArea(COPYRIGHT, new Point(textX, 289),
                                                    new Dimension(textWidth + 30, 43),
                                                    new Font("SansSerif", Font.PLAIN, 9));
      copyright.setForeground(Color.lightGray);
      r.add(copyright);
   }

   public String getDescription()
   {
      return "Document Optimizer " + DocumentUpdater.VERSION;
   }

   public WizardData getWizardData()
   {
      if (nextWizard_ == null)
         nextWizard_ = new UpdaterSelectWizardPanel(data_);

      data_.nextWizardPanel = nextWizard_;
      nextWizard_.setEnableButton(BUTTON_BACK, true);

      return data_;
   }
}
