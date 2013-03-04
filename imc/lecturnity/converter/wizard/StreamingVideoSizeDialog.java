package imc.lecturnity.converter.wizard;

import java.awt.Dimension;
import java.awt.Container;
import java.awt.Point;
import java.awt.Font;
import java.awt.Color;
import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.Insets;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

import javax.swing.*;
import javax.swing.border.BevelBorder;
import javax.swing.border.TitledBorder;
import javax.swing.filechooser.FileFilter;

import javax.swing.JOptionPane;

import java.io.File;

import imc.lecturnity.util.wizards.ButtonPanel;
import imc.lecturnity.util.wizards.WizardButton;
import imc.lecturnity.util.wizards.WizardPanel;

import imc.lecturnity.util.ui.PopupHelp;
import imc.lecturnity.util.ui.LazyTextArea;

import imc.epresenter.filesdk.util.Localizer;

class StreamingVideoSizeDialog extends JDialog
{
   public static final int ACTION_CANCEL = 0;
   public static final int ACTION_OK     = 1;

   private static String CAPTION = "[!]";
   private static String VIDEOSIZE_CAPTION = "[!]";
   private static String VIDEOSIZE_TAB = "[!]";
   private static String PROFILE_SIZE = "[!]";
   private static String MAXCLIP_SIZE = "[!]";
   private static String MAXCLIP_WARNING = "[!]";
   private static String ORIGINAL_SIZE = "[!]";
   private static String CUSTOM_SIZE = "[!]";
   private static String CONSIDER_VIDEOSIZE = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/StreamingVideoSizeDialog_",
             "en");
         CAPTION = l.getLocalized("CAPTION");
         VIDEOSIZE_CAPTION = l.getLocalized("VIDEOSIZE_CAPTION");
         VIDEOSIZE_TAB = l.getLocalized("VIDEOSIZE_TAB");
         PROFILE_SIZE = l.getLocalized("PROFILE_SIZE");
         MAXCLIP_SIZE = l.getLocalized("MAXCLIP_SIZE");
         MAXCLIP_WARNING = l.getLocalized("MAXCLIP_WARNING");
         ORIGINAL_SIZE = l.getLocalized("ORIGINAL_SIZE");
         CUSTOM_SIZE = l.getLocalized("CUSTOM_SIZE");
         CONSIDER_VIDEOSIZE = l.getLocalized("CONSIDER_VIDEOSIZE");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open StreamingVideoSizeDialog locale database!\n"+
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private static final String[] CUSTOM_SIZES = {"160x120", "176x144", "240x180", "320x240", "480x360", "640x480", "800x600", "1024x768"};

   private JRadioButton profileSizeRadio_;
   private JRadioButton maxClipSizeRadio_;
   private JRadioButton customSizeRadio_;
   private JComboBox    customSizesBox_;
   private JCheckBox    considerVideosizeBox_;

   private LazyTextArea maxClipSizeLabel_;

   private StreamingConvertWizardData data_;
   private Dimension profileVideoSize_;

   private int       maxVideoWidth_ = -1;
   private int       maxVideoHeight_ = -1;

   private int actionTaken_ = ACTION_CANCEL;

   public StreamingVideoSizeDialog(Frame parent, StreamingConvertWizardData data, Dimension profileVideoSize)
   {
      super(parent, CAPTION, true);
      data_ = data;
      profileVideoSize_ = profileVideoSize;

      setDefaultCloseOperation(DISPOSE_ON_CLOSE);
      
      initGui();

      centerDialog(parent);
   }

   private void initGui()
   {
      Container r = getContentPane();
      r.setLayout(new BorderLayout());

      boolean hasNormalVideo = data_.hasVideoFile && !data_.videoIsStillImage && 
         !data_.containsMultipleVideos;
      boolean hasSgClips     = data_.hasVideoFile && !data_.videoIsStillImage &&
         data_.containsMultipleVideos;

      ButtonPanel bp = new ButtonPanel(false);
      ButtonListener buttonListener = new ButtonListener();
      WizardButton okButton = new WizardButton(WizardPanel.OK);
      okButton.setMnemonic(WizardPanel.MNEM_OK);
      okButton.setDefaultCapable(true);
      okButton.addActionListener(buttonListener);
      bp.addRightButton(okButton);
      bp.addRightButton(Box.createHorizontalStrut(WizardPanel.BUTTON_SPACE_X));
      WizardButton cancelButton = new WizardButton(WizardPanel.CANCEL);
      cancelButton.setMnemonic(WizardPanel.MNEM_CANCEL);
      cancelButton.addActionListener(buttonListener);
      bp.addRightButton(cancelButton);

      boolean useStandardSize = (data_.videoSizeType == StreamingConvertWizardData.VIDEOSIZE_STANDARD);
      boolean useMaxClipSize  = (data_.videoSizeType == StreamingConvertWizardData.VIDEOSIZE_MAXCLIP);
      boolean useCustomSize   = (data_.videoSizeType == StreamingConvertWizardData.VIDEOSIZE_CUSTOM);
      boolean considerVideoSize = data_.doConsiderVideosize;

//       JButton helpButton = new WizardButton(WizardPanel.HELP);
//       helpButton.setMnemonic(WizardPanel.MNEM_HELP);
// //       helpPopup_ = new PopupHelp(helpButton, allHelp);
//       helpButton.addActionListener(new ActionListener() {
//             public void actionPerformed(ActionEvent e) { 
//                if (!PopupHelp.isHelpVisible())
//                   helpPopup_.popupHelp(); 
//                else
//                   PopupHelp.closeHelpWindows();
//             }
//          });
//       bp.addLeftButton(helpButton);

      r.add(bp, BorderLayout.SOUTH);

      // The center panel with the tabbed pane

      JPanel center = new JPanel() { 
            public Dimension getPreferredSize() 
            { 
               return new Dimension(420, 360); 
            } 
            
            public Insets getInsets()
            {
               return new Insets(7, 7, 0, 7);
            }
         };
      center.setLayout(new BorderLayout());
      r.add(center, BorderLayout.CENTER);

      JTabbedPane pane = new JTabbedPane(JTabbedPane.TOP);

      // Save location panel

      JPanel panel0 = new JPanel() 
         {
            public Insets getInsets() { return new Insets(10, 10, 10, 10); }
         };
      panel0.setLayout(new BorderLayout());
      JPanel panel = new JPanel();
      panel.setLayout(null);
      panel.setBorder(new TitledBorder(VIDEOSIZE_CAPTION));
      panel0.add(panel, BorderLayout.CENTER);

      pane.addTab(VIDEOSIZE_TAB, panel0);

      int y = 30;
      int s1 = 340;
      int s2 = 260;
      int x1 = 20;
      int x2 = 100;

      ButtonGroup bg = new ButtonGroup();
      RadioListener rl = new RadioListener();

      profileSizeRadio_ = new JRadioButton(PROFILE_SIZE + 
                                           "(" + profileVideoSize_.width + "x" +
                                           profileVideoSize_.height + ")", useStandardSize);
      profileSizeRadio_.setSize(s1, 20);
      profileSizeRadio_.setLocation(x1, y);
      profileSizeRadio_.addActionListener(rl);
      panel.add(profileSizeRadio_);
      bg.add(profileSizeRadio_);

      boolean hasClipSizes = false;
      maxVideoWidth_ = -1;
      maxVideoHeight_ = -1;

      if (data_.containsMultipleVideos)
      {
         for (int i=0; i<data_.multiVideoClips.length; ++i)
         {
            int w = data_.multiVideoClips[i].videoWidth;
            if (w > maxVideoWidth_)
               maxVideoWidth_ = w;
            int h = data_.multiVideoClips[i].videoHeight;
            if (h > maxVideoHeight_)
               maxVideoHeight_ = h;
         }

         if (maxVideoWidth_ == -1 || maxVideoHeight_ == -1)
            hasClipSizes = false;
         else
            hasClipSizes = true;
      }
      else
      {
         maxVideoWidth_ = data_.videoWidth;
         maxVideoHeight_ = data_.videoHeight;
      }

      String label;
      if (hasNormalVideo)
         label = ORIGINAL_SIZE;
      else
         label = MAXCLIP_SIZE;

      y += 40;
      maxClipSizeRadio_ = new JRadioButton(label + "(" + maxVideoWidth_ + "x" + maxVideoHeight_ + ")", useMaxClipSize);
      maxClipSizeRadio_.setSize(s1, 20);
      maxClipSizeRadio_.setLocation(x1, y);
      maxClipSizeRadio_.addActionListener(rl);
      panel.add(maxClipSizeRadio_);
//       if (!data_.containsMultipleVideos || !hasClipSizes)
//       {
//          maxClipSizeRadio_.setEnabled(false);
//       }
      bg.add(maxClipSizeRadio_);

      y += 25;
      maxClipSizeLabel_ = new LazyTextArea(MAXCLIP_WARNING, 
                                           new Point(x1 + 20, y), 
                                           new Dimension(s1 - 20, 40),
                                           imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12) );
      //maxClipSizeLabel_.setSize(s1 - 20, 40);
      //maxClipSizeLabel_.setLocation(x1 + 20, y);
      panel.add(maxClipSizeLabel_);
      
      y += 40;
      customSizeRadio_ = new JRadioButton(CUSTOM_SIZE, useCustomSize);
      customSizeRadio_.setSize(s1, 20);
      customSizeRadio_.setLocation(x1, y);
      customSizeRadio_.addActionListener(rl);
      bg.add(customSizeRadio_);
      panel.add(customSizeRadio_);

      y += 30;
      customSizesBox_ = new JComboBox(CUSTOM_SIZES);
      customSizesBox_.setSize(150, 22);
      customSizesBox_.setLocation(x2, y);
      panel.add(customSizesBox_);
      if (useCustomSize)
      {
         String customSize = "" + data_.outputVideoWidth + "x" + data_.outputVideoHeight;
         customSizesBox_.setSelectedItem(customSize);
         if (!customSizesBox_.getSelectedItem().equals(customSize))
         {
            customSizesBox_.addItem(customSize);
            customSizesBox_.setSelectedItem(customSize);
         }
      }
      
      y += 90;
      considerVideosizeBox_ = new JCheckBox(CONSIDER_VIDEOSIZE, considerVideoSize);
      considerVideosizeBox_.setSize(s1, 20);
      considerVideosizeBox_.setLocation(x1, y);
      panel.add(considerVideosizeBox_);
      
      center.add(pane, BorderLayout.CENTER);

      setResizable(false);
      pack();

      checkEnabled();
   }

   private void doOk()
   {
      actionTaken_ = ACTION_OK;

      if (profileSizeRadio_.isSelected())
      {
         data_.videoSizeType = StreamingConvertWizardData.VIDEOSIZE_STANDARD;
         data_.outputVideoWidth = profileVideoSize_.width;
         data_.outputVideoHeight = profileVideoSize_.height;
      }
      else if (maxClipSizeRadio_.isSelected())
      {
         data_.videoSizeType = StreamingConvertWizardData.VIDEOSIZE_MAXCLIP;
         data_.outputVideoWidth = maxVideoWidth_;
         data_.outputVideoHeight = maxVideoHeight_;
      }
      else if (customSizeRadio_.isSelected())
      {
         data_.videoSizeType = StreamingConvertWizardData.VIDEOSIZE_CUSTOM;
         String customSize = customSizesBox_.getSelectedItem().toString();
         int xPos = customSize.indexOf('x');
         int w = Integer.parseInt(customSize.substring(0, xPos));
         int h = Integer.parseInt(customSize.substring(xPos + 1));

         System.out.println("Custom size: " + w + "x" + h);

         data_.outputVideoWidth = w;
         data_.outputVideoHeight = h;
      }

      data_.doConsiderVideosize = considerVideosizeBox_.isSelected();
      
      dispose();
   }

   private void doCancel()
   {
      actionTaken_ = ACTION_CANCEL;
      dispose();
   }

   public int getActionTaken()
   {
      return actionTaken_;
   }

   private void checkEnabled()
   {
      customSizesBox_.setEnabled(customSizeRadio_.isSelected());
   }

   private void centerDialog(Frame parent)
   {
      if (parent == null)
         return;

      Point pPos = parent.getLocation();
      Dimension pDim = parent.getSize();

      Dimension myDim = getSize();
      int myX = pPos.x + (pDim.width - myDim.width)/2;
      if (myX < 0) 
         myX = 0;
      int myY = pPos.y + (pDim.height - myDim.height)/2;
      if (myY < 0)
         myY = 0;

      setLocation(new Point(myX, myY));
   }

   private class ButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         String cmd = e.getActionCommand();
         if (cmd.equals(WizardPanel.OK))
         {
            doOk();
         }
         else if (cmd.equals(WizardPanel.CANCEL))
         {
            doCancel();
         }
//          else if (cmd.equals(WizardPanel.HELP))
//          {
//             doHelp();
//          }
      }
   }

   private class RadioListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         checkEnabled();
      }
   }
}
