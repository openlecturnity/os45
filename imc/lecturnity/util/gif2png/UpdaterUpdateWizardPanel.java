package imc.lecturnity.util.gif2png;

import java.awt.Dimension;
import java.awt.Container;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.io.File;
import java.io.IOException;

import javax.swing.*;

import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.WizardData;
import imc.lecturnity.util.wizards.Wizard;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.util.LogProgressListener;
import imc.lecturnity.util.CancelException;

class UpdaterUpdateWizardPanel extends WizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";

   private static String PROGRESS = "[!]";
   private static String ERROR = "[!]";
   private static String INFORMATION = "[!]";
   private static String CANCEL = "[!]";
   private static char   MNEM_CANCEL = '?';

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/util/gif2png/UpdaterUpdateWizardPanel_", "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");

         PROGRESS = l.getLocalized("PROGRESS");
         ERROR = l.getLocalized("ERROR");
         INFORMATION = l.getLocalized("INFORMATION");
         CANCEL = l.getLocalized("CANCEL");
         MNEM_CANCEL = l.getLocalized("MNEM_CANCEL").charAt(0);
      }
      catch (Exception e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog
            (null, "Localizer error in imc.lecturnity.util.gif2png.UpdaterUpdateWizardPanel:\n" +
             e.toString(), "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(0);
      }
   }

   private UpdaterData data_ = null;

   private JLabel       overallLabel_;
   private JProgressBar overallProgress_;
   private JLabel       detailLabel_;
   private JProgressBar detailProgress_;

   private JButton      cancelButton_;

   private Gif2PngEngine gif2PngEngine_;

   public UpdaterUpdateWizardPanel(UpdaterData data)
   {
      super();

      data_ = data;

      setPreferredSize(new Dimension(500, 400));

      addButtons(BUTTON_BACK | BUTTON_QUIT | BUTTON_CANCEL);
      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");
      
      Container r = getContentPanel();
      r.setLayout(null);

      overallLabel_ = new JLabel(PROGRESS);
      overallLabel_.setSize(440, 16);
      overallLabel_.setLocation(30, 50);
      r.add(overallLabel_);

      overallProgress_ = new JProgressBar(JProgressBar.HORIZONTAL, 0, 1);
      overallProgress_.setSize(440, 20);
      overallProgress_.setLocation(30, 70);
      r.add(overallProgress_);

      detailLabel_ = new JLabel(PROGRESS);
      detailLabel_.setSize(440, 16);
      detailLabel_.setLocation(30, 130);
      r.add(detailLabel_);

      detailProgress_ = new JProgressBar(JProgressBar.HORIZONTAL, 0, 1);
      detailProgress_.setSize(440, 20);
      detailProgress_.setLocation(30, 150);
      r.add(detailProgress_);
      
      cancelButton_ = new JButton(CANCEL);
      cancelButton_.setSize(100, 40);
      cancelButton_.setLocation(200, 200);
      cancelButton_.setMnemonic(MNEM_CANCEL);
      cancelButton_.addActionListener(new CancelButtonListener());
      r.add(cancelButton_);
   }

   public String getDescription()
   {
      return "";
   }
   
   public WizardData getWizardData()
   {
      return data_;
   }

   public int displayWizard()
   {
      setEnableButton(BUTTON_BACK | BUTTON_QUIT | BUTTON_CANCEL, false);
      cancelButton_.setEnabled(true);

      try
      {
         gif2PngEngine_ = new Gif2PngEngine(data_.fileName,
                                            data_.targetFileName,
                                            new OverallProgressListener(),
                                            new DetailProgressListener());
         gif2PngEngine_.update();

         setEnableButton(BUTTON_QUIT, true);
         setEnableButton(BUTTON_BACK | BUTTON_CANCEL, false);
      }
      catch (CancelException e)
      {
         JOptionPane.showMessageDialog(this, e.getMessage(), INFORMATION,
                                       JOptionPane.INFORMATION_MESSAGE);
         setEnableButton(BUTTON_QUIT, false);
         setEnableButton(BUTTON_BACK | BUTTON_CANCEL, true);
      }
      catch (IOException e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(this, e.getMessage(), ERROR,
                                       JOptionPane.ERROR_MESSAGE);
         setEnableButton(BUTTON_QUIT, false);
         setEnableButton(BUTTON_BACK | BUTTON_CANCEL, true);
      }
      finally
      {
         gif2PngEngine_ = null;
         cancelButton_.setEnabled(false);
      }

      return super.displayWizard();
   }

   private void requestCancel()
   {
      if (gif2PngEngine_ != null)
      {
         cancelButton_.setEnabled(false);
         gif2PngEngine_.requestCancel();
      }
   }

   private class CancelButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         requestCancel();
      }
   }

   private class OverallProgressListener implements LogProgressListener
   {
      public void setMaximum(int max) { overallProgress_.setMaximum(max); }
      public void setValue(int value) { overallProgress_.setValue(value); }
      public void logString(String msg) { overallLabel_.setText(msg); }
   }

   private class DetailProgressListener implements LogProgressListener
   {
      public void setMaximum(int max) { detailProgress_.setMaximum(max); }
      public void setValue(int value) { detailProgress_.setValue(value); }
      public void logString(String msg) { detailLabel_.setText(msg); }
   }
}
