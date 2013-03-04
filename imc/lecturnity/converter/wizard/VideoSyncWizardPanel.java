package imc.lecturnity.converter.wizard;

import java.awt.*;
import java.awt.event.*;

import java.io.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.filechooser.FileFilter;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.DocumentData;

import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.wizards.Wizard;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.WizardData;
import imc.lecturnity.util.wizards.WizardTextArea;
import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.filesdk.FileResources;

import imc.epresenter.player.Document;
import imc.epresenter.player.DocumentController;

import java.util.Hashtable;

public class VideoSyncWizardPanel extends WizardPanel implements DocumentController
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String ICON_RESOURCE = 
      "/imc/lecturnity/converter/images/lecturnity_logo.gif";
   private static String START_PLAYER = "[!]";
   private static char   MNEM_START_PLAYER = '?';
   private static String DESC = "[!]";

   private static String LEFT_LABEL = "[!]";
   private static String LEFT_TOOLTIP = "[!]";
   private static char   MNEM_LEFT = '?';
   private static String RIGHT_LABEL = "[!]";
   private static String RIGHT_TOOLTIP = "[!]";
   private static char   MNEM_RIGHT = '?';
   private static String LARGER_LABEL = "[!]";
   private static String LARGER_TOOLTIP = "[!]";
   private static char   MNEM_LARGER = '?';
   private static String SMALLER_LABEL = "[!]";
   private static String SMALLER_TOOLTIP = "[!]";
   private static char   MNEM_SMALLER = '?';
   private static String SLIDER_TOOLTIP = "[!]";

   private static String ERROR = "[!]";
   private static String REMOVE_OFFSET_ERROR = "[!]";
   private static String ADD_OFFSET_ERROR = "[!]";
   private static String COPY_ERROR = "[!]";
   
   private static String DEL_TEMP_ERROR = "[!]";
   private static String INFORMATION = "[!]";
   private static String WARNING = "[!]";

   private static String ERR_PLAYERSTART = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/VideoSyncWizardPanel_",
             "en");
         
         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");

         START_PLAYER = l.getLocalized("START_PLAYER");
         MNEM_START_PLAYER = l.getLocalized("MNEM_START_PLAYER").charAt(0);
         DESC = l.getLocalized("DESC");

         LEFT_LABEL = l.getLocalized("LEFT_LABEL");
         LEFT_TOOLTIP = l.getLocalized("LEFT_TOOLTIP");
         MNEM_LEFT = l.getLocalized("MNEM_LEFT").charAt(0);
         RIGHT_LABEL = l.getLocalized("RIGHT_LABEL");
         RIGHT_TOOLTIP = l.getLocalized("RIGHT_TOOLTIP");
         MNEM_RIGHT = l.getLocalized("MNEM_RIGHT").charAt(0);
         LARGER_LABEL = l.getLocalized("LARGER_LABEL");
         LARGER_TOOLTIP = l.getLocalized("LARGER_TOOLTIP");
         MNEM_LARGER = l.getLocalized("MNEM_LARGER").charAt(0);
         SMALLER_LABEL = l.getLocalized("SMALLER_LABEL");
         SMALLER_TOOLTIP = l.getLocalized("SMALLER_TOOLTIP");
         MNEM_SMALLER = l.getLocalized("MNEM_SMALLER").charAt(0);
         SLIDER_TOOLTIP = l.getLocalized("SLIDER_TOOLTIP");

         INFORMATION = l.getLocalized("INFORMATION");
         WARNING = l.getLocalized("WARNING");

         ERROR = l.getLocalized("ERROR");
         REMOVE_OFFSET_ERROR = l.getLocalized("REMOVE_OFFSET_ERROR");
         ADD_OFFSET_ERROR = l.getLocalized("ADD_OFFSET_ERROR");
         DEL_TEMP_ERROR = l.getLocalized("DEL_TEMP_ERROR");
         COPY_ERROR = l.getLocalized("COPY_ERROR");

         ERR_PLAYERSTART = l.getLocalized("ERR_PLAYERSTART");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database: VideoSyncWizardPanel\n" +
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private WizardPanel nextWizard_ = null;

   private PublisherWizardData pwData_;

   private JFrame   documentFrame_;
   private Document document_;

   private JButton playerButton_;
   private JSlider slider_;
   private JLabel  msLabel_;

   private double zoomFactor = 1.0;

   private ExtendedVideoDialog parent_ = null;
   
   public VideoSyncWizardPanel(ExtendedVideoDialog parent, PublisherWizardData pwData)
   {
      super();

      parent_ = parent;

      pwData_ = pwData;

      setPreferredSize(new Dimension(500, 400)); //(500, 400)

///      addButtons(NEXT_BACK_CANCEL);
      
///      useHeaderPanel(HEADER, SUBHEADER,
///                     ICON_RESOURCE);
      buttonPanel_.showBreakLine(false);

      Container r = getContentPanel();
      r.setLayout(null);

      WizardTextArea desc = new WizardTextArea(DESC, new Point(20, 30), /*(30, 20)*/
                                               new Dimension(420, 80), /*(440, 80)*/
                                               imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      r.add(desc);

      JPanel sliderPanel = new JPanel();
      r.add(sliderPanel);
      sliderPanel.setBorder(new EtchedBorder());
      sliderPanel.setSize(440, 160); //(460, 160);
      sliderPanel.setLocation(10, 110); //(20, 100);
      sliderPanel.setLayout(null);

      slider_ = new JSlider(JSlider.HORIZONTAL, -10000, 6000, 0);
      slider_.setSize(420, 50); //(440, 50);
      slider_.setLocation(10, 65);

      Hashtable labels = slider_.createStandardLabels(2000);
      slider_.setLabelTable(labels);
      slider_.setPaintLabels(true);

      slider_.setMajorTickSpacing(2000);
      slider_.setMinorTickSpacing(200);
      slider_.setPaintTicks(true);
      slider_.addChangeListener(new SliderListener());
      slider_.setToolTipText(SLIDER_TOOLTIP);
      sliderPanel.add(slider_);

      playerButton_ = new JButton(START_PLAYER);
      playerButton_.setSize(150, 30);
      playerButton_.setLocation(145, 20); //(155, 20);
      playerButton_.setMnemonic(MNEM_START_PLAYER);
      playerButton_.addActionListener(new StartPlayerListener());
      sliderPanel.add(playerButton_);

      msLabel_ = new JLabel("" + pwData_.GetDocumentData().GetVideoOffsetMs() + " ms");
      msLabel_.setSize(100, 15);
      msLabel_.setLocation(30, 30);
      sliderPanel.add(msLabel_);

      ActionListener buttonListener = new ButtonListener();
      
      int bHeight = 24;
      int bWidth  = 95; //100;
      int bSpace  = 7;
      int bOffset = 20;
      
      JButton leftButton = new JButton(LEFT_LABEL);
      leftButton.setActionCommand("LEFT");
      leftButton.setSize(bWidth, bHeight);
      leftButton.setLocation(bOffset + 0, 120);
      leftButton.setToolTipText(LEFT_TOOLTIP);
      leftButton.setMnemonic(MNEM_LEFT);
      leftButton.addActionListener(buttonListener);
      sliderPanel.add(leftButton);

      JButton largerButton = new JButton(LARGER_LABEL);
      largerButton.setActionCommand("LARGER");
      largerButton.setSize(bWidth, bHeight);
      largerButton.setLocation(bOffset + (bWidth + bSpace), 120);
      largerButton.setToolTipText(LARGER_TOOLTIP);
      largerButton.setMnemonic(MNEM_LARGER);
      largerButton.addActionListener(buttonListener);
      sliderPanel.add(largerButton);

      JButton smallerButton = new JButton(SMALLER_LABEL);
      smallerButton.setActionCommand("SMALLER");
      smallerButton.setSize(bWidth, bHeight);
      smallerButton.setLocation(bOffset + 2*(bWidth + bSpace), 120);
      smallerButton.setToolTipText(SMALLER_TOOLTIP);
      smallerButton.setMnemonic(MNEM_SMALLER);
      smallerButton.addActionListener(buttonListener);
      sliderPanel.add(smallerButton);

      JButton rightButton = new JButton(RIGHT_LABEL);
      rightButton.setActionCommand("RIGHT");
      rightButton.setSize(bWidth - 1, bHeight);
      rightButton.setLocation(bOffset + 3*(bWidth + bSpace), 120);
      rightButton.setToolTipText(RIGHT_TOOLTIP);
      rightButton.setMnemonic(MNEM_RIGHT);
      rightButton.addActionListener(buttonListener);
      sliderPanel.add(rightButton);

      refreshDataDisplay();
   }

   public String getDescription()
   {
      return "";
   }

   protected WizardPanel getNextWizardPanel()
   {
      return null;
   }

   public PublisherWizardData getWizardData()
   {
      if (nextWizard_ == null)
         nextWizard_ = new SelectActionWizardPanel(pwData_);
      
      pwData_.nextWizardPanel = nextWizard_;

      if (document_ != null)
      {
         document_.close();
         document_ = null;
         if (documentFrame_ != null)
         {
            documentFrame_.dispose();
            documentFrame_ = null;
         }
         playerButton_.setEnabled(true);
      }

      return pwData_;
   }

   public boolean verifyNext()
   {
      if (slider_.getValue() == pwData_.GetDocumentData().GetVideoOffsetMs())
         return true;
      
      if (!removeOffsets())
         return false;
      
      if (!addOffsets())
         return false;

      return true;
   }

   // remove any <OFFSETS> tags in document
   private boolean removeOffsets()
   {
      try
      {
         DocumentData dd = pwData_.GetDocumentData();

         FileInputStream fis = new FileInputStream(dd.GetPresentationFileName());
         BufferedReader in = FileUtils.createBufferedReader(fis, dd.GetCodepage());
         PrintWriter out = FileUtils.CreatePrintWriter(dd.GetPresentationFileName() + ".tmp");
    
         String line;
         boolean inOffsets = false;
         while ((line = in.readLine()) != null)
         {
            if (line.trim().equals("<OFFSETS>"))
            {
               inOffsets = true;
            }
            else if (inOffsets)
            {
               if (line.trim().equals("</OFFSETS>"))
                  inOffsets = false;
            }
            else
               out.println(line);
         }

         in.close();
         out.flush();
         out.close();

         // copy temporary file back to real LRD file:
         boolean success = copyFile(new File(dd.GetPresentationFileName() + ".tmp"),
                                    new File(dd.GetPresentationFileName()), "");
         // delete temporary file:
         File tempFile = new File(dd.GetPresentationFileName() + ".tmp");
         if (!tempFile.delete())
            JOptionPane.showMessageDialog
               (this, DEL_TEMP_ERROR + dd.GetPresentationFileName() + ".tmp",
                WARNING, JOptionPane.WARNING_MESSAGE);

         
         dd.SetVideoOffsetMs(0);

         return true;
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(this, REMOVE_OFFSET_ERROR + e.getMessage(), 
                                       ERROR, JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         return false;
      }
   }

   private boolean addOffsets()
   {
      try
      {
         DocumentData dd = pwData_.GetDocumentData();

         FileInputStream fis = new FileInputStream(dd.GetPresentationFileName());
         BufferedReader in = FileUtils.createBufferedReader(fis, dd.GetCodepage());
         PrintWriter out = FileUtils.CreatePrintWriter(dd.GetPresentationFileName() + ".tmp");
                  
         String line;
         boolean inOffsets = false;
         while ((line = in.readLine()) != null)
         {
            out.println(line);

            if (line.trim().equals("</HELPERS>"))
            {
               // we'll add the offsets here
               out.println("    <OFFSETS>");
               for (int i = 0; i < dd.GetVideoHelperIndex(); i++)
                  out.println("      <O>0</O>");
               out.println("      <O>" + slider_.getValue() + "</O>");
               out.println("    </OFFSETS>");
            }
         }

         in.close();
         out.flush();
         out.close();

         // copy temporary file back to real LRD file:
         boolean success = copyFile(new File(dd.GetPresentationFileName() + ".tmp"),
                                    new File(dd.GetPresentationFileName()), "");
         // delete temporary file:
         File tempFile = new File(dd.GetPresentationFileName() + ".tmp");
         if (!tempFile.delete())
            JOptionPane.showMessageDialog
               (this, DEL_TEMP_ERROR + dd.GetPresentationFileName() + ".tmp",
                WARNING, JOptionPane.WARNING_MESSAGE);

         dd.SetVideoOffsetMs(slider_.getValue());

         return true;
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(this, REMOVE_OFFSET_ERROR + e.getMessage(), 
                                       ERROR, JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         return false;
      }
   }

   public void closePerformed()
   {
      // release document:
      document_ = null;
      if (documentFrame_ != null)
      {
         documentFrame_.dispose();
         documentFrame_ = null;
      }
      playerButton_.setEnabled(true);
   }


   public void loadDocument(String location)
   {
      // not implemented here
   }

   public void loadDocument(String location, int time)
   {
      // not implemented here
   }

   public void loadDocument(String location, int time, 
                            boolean startup, ActionListener finishListener)
   {
      // not implemented here
   }
   
   public void closeDocument(Document d)
   {
      // not implemented here
   }

   private void startPlayer()
   {
      playerButton_.setEnabled(false);
      Thread t = new Thread() 
         {
            public void run()
            {
//                setEnableButton(NEXT_BACK_CANCEL, false);
               try
               {
                  DocumentData dd = pwData_.GetDocumentData();

                  document_ = new Document(dd.GetPresentationFileName(), 0, 
                                           VideoSyncWizardPanel.this);
                  document_.setHelperOffset(dd.GetVideoHelperIndex(),
                                            slider_.getValue());
                  documentFrame_ = new JFrame
                     ("Publisher " + 
                      imc.epresenter.converter.PresentationConverter.PRINTABLE_VERSION);
                  Container content = documentFrame_.getContentPane();
                  content.setLayout(new BorderLayout());
                  content.add(document_.getVisualComponent(), BorderLayout.CENTER);
                  documentFrame_.addWindowListener(new WindowAdapter() 
                     {
                        public void windowClosing(WindowEvent e)
                        {
                           if (document_ != null)
                           {
                              document_.close();
                              document_ = null;
                           }
                           if (documentFrame_ != null)
                           {
                              documentFrame_.dispose();
                              documentFrame_ = null;
                           }
                           playerButton_.setEnabled(true);
                        }
                     });
                  documentFrame_.setIconImage(DIALOG_ICON.getImage());
                  documentFrame_.pack();
                  documentFrame_.setVisible(true);
               }
               catch (IllegalAccessException e) 
               { 
                  JOptionPane.showMessageDialog(VideoSyncWizardPanel.this, ERR_PLAYERSTART + e.getMessage(),
                                                ERROR, JOptionPane.ERROR_MESSAGE);
                  if (document_ != null)
                  {
                     document_.close();
                     document_ = null;
                  }

                  if (documentFrame_ != null)
                  {
                     documentFrame_.dispose();
                     documentFrame_ = null;
                  }
                  playerButton_.setEnabled(true);
                  e.printStackTrace();
               }
               catch (InstantiationException e)
               { 
                  JOptionPane.showMessageDialog(VideoSyncWizardPanel.this, ERR_PLAYERSTART + e.getMessage(),
                                                ERROR, JOptionPane.ERROR_MESSAGE);
                  if (document_ != null)
                  {
                     document_.close();
                     document_ = null;
                  }

                  if (documentFrame_ != null)
                  {
                     documentFrame_.dispose();
                     documentFrame_ = null;
                  }
                  playerButton_.setEnabled(true);
                  e.printStackTrace();
               }

//                setEnableButton(NEXT_BACK_CANCEL, true);
            }
         };
      t.start();
   }

   public void refreshDataDisplay()
   {
      int min = slider_.getMinimum();
      int max = slider_.getMaximum();

      int range = max - min;
      int val = pwData_.GetDocumentData().GetVideoOffsetMs();

      if (val < min || val > max)
      {
         // Eugh. We have to do something about the range
         slider_.setMinimum(val - 1000);
         slider_.setMaximum(val + 1000);
         slider_.setValue(val);

         makeSliderLarger();
         makeSliderSmaller();
      }
      else
      {
         slider_.setValue(val);
      }

      super.refreshDataDisplay();
   }

   private void updateOffset()
   {
      msLabel_.setText("" + slider_.getValue() + " ms");
      if (document_ != null)
      {
         document_.setHelperOffset(pwData_.GetDocumentData().GetVideoHelperIndex(), slider_.getValue());
      }
   }

   private void moveSliderLeft()
   {
      int min = slider_.getMinimum() - (int) (zoomFactor*2000);
      int max = slider_.getMaximum() - (int) (zoomFactor*2000);
      int val = slider_.getValue();

      if (val < min) 
         val = min;
      if (val > max)
         val = max;

      slider_.setMinimum(min);
      slider_.setMaximum(max);
      slider_.setValue(val);

      updateTicks();
   }

   private void moveSliderRight()
   {
      int min = slider_.getMinimum() + (int) (zoomFactor*2000);
      int max = slider_.getMaximum() + (int) (zoomFactor*2000);
      int val = slider_.getValue();

      if (val < min) 
         val = min;
      if (val > max)
         val = max;

      slider_.setMinimum(min);
      slider_.setMaximum(max);
      slider_.setValue(val);

      updateTicks();
   }

   private void makeSliderLarger()
   {
      if (zoomFactor > 1000)
         return;

      zoomFactor = zoomFactor * 2.0;
//      System.out.println("zoomFactor: " + zoomFactor);

      int val = slider_.getValue();

      int extent = (int) (zoomFactor * 16000) / 2;
      
      // center around val
      int min = val - extent;
      int max = val + extent;

      // check so that ticks still are ok:
      int majorTick = (int) (zoomFactor*2000);
      if (min < 0)
      {
         int dev = Math.abs(min) % majorTick;
         if (dev > majorTick/2)
         {
            min -= (majorTick - dev);
            max -= (majorTick - dev);
         }
         else
         {
            min += dev;
            max += dev;
         }
      }
      else
      {
         int dev = Math.abs(min) % majorTick;
         if (dev > majorTick/2)
         {
            min += (majorTick - dev);
            max += (majorTick - dev);
         }
         else
         {
            min -= dev;
            max -= dev;
         }
      }

      slider_.setMinimum(min);
      slider_.setMaximum(max);
      slider_.setValue(val);

      updateTicks();
   }

   private void makeSliderSmaller()
   {
      if (zoomFactor < 0.001)
         return;

      zoomFactor = zoomFactor / 2.0;
//      System.out.println("zoomFactor: " + zoomFactor);
      
      int val = slider_.getValue();

      int extent = (int) (zoomFactor * 16000) / 2;
      
      // center around val
      int min = val - extent;
      int max = val + extent;

      // check so that ticks still are ok:
      int majorTick = (int) (zoomFactor*2000);
      if (min < 0)
      {
         int dev = Math.abs(min) % majorTick;
         if (dev > majorTick/2)
         {
            min -= (majorTick - dev);
            max -= (majorTick - dev);
         }
         else
         {
            min += dev;
            max += dev;
         }
      }
      else
      {
         int dev = Math.abs(min) % majorTick;
         if (dev > majorTick/2)
         {
            min += (majorTick - dev);
            max += (majorTick - dev);
         }
         else
         {
            min -= dev;
            max -= dev;
         }
      }

      slider_.setMinimum(min);
      slider_.setMaximum(max);
      slider_.setValue(val);

      updateTicks();
   }

   private void updateTicks()
   {
      int majorTickSpacing = (int) (zoomFactor*2000);
      int minorTickSpacing = (int) (zoomFactor*200);

      slider_.setLabelTable(slider_.createStandardLabels(majorTickSpacing));
      slider_.setMinorTickSpacing(minorTickSpacing);
      slider_.setMajorTickSpacing(majorTickSpacing);
   }

   private boolean copyFile(File src, File dest, String message)
   {
      try
      {
         FileUtils.copyFile(src, dest, this, message);
         return true;
      }
      catch (IOException e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(this, COPY_ERROR + e.getMessage(),
                                       ERROR, JOptionPane.ERROR_MESSAGE);
         return false;
      }
   }

   private class StartPlayerListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         startPlayer();
      }
   }

   private class SliderListener implements ChangeListener
   {
      public void stateChanged(ChangeEvent e)
      {
         updateOffset();
      }
   }

   private class ButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         String command = e.getActionCommand();

         if (command.equals("LEFT"))
            moveSliderLeft();
         else if (command.equals("RIGHT"))
            moveSliderRight();
         else if (command.equals("LARGER"))
            makeSliderLarger();
         else if (command.equals("SMALLER"))
            makeSliderSmaller();
      }
   }
}
