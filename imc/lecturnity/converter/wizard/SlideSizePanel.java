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

import javax.swing.*;
import javax.swing.border.BevelBorder;
import javax.swing.border.TitledBorder;
import javax.swing.filechooser.FileFilter;

import javax.swing.JOptionPane;

import java.io.File;

import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.wizards.ButtonPanel;
import imc.lecturnity.util.wizards.WizardButton;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.WizardData;

import imc.lecturnity.util.ui.PopupHelp;
import imc.lecturnity.util.ui.LazyTextArea;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.converter.StreamingMediaConverter;

class SlideSizePanel extends WizardPanel
{
   private static Dimension[] SCREEN_RESOLUTIONS = { new Dimension(640, 480), 
                                                     new Dimension(800, 600), 
                                                     new Dimension(1024, 768), 
                                                     new Dimension(1152, 864), 
                                                     new Dimension(1280, 1024), 
                                                     new Dimension(1600, 1200) };

   private static int NUM_RES = SCREEN_RESOLUTIONS.length;
   
   private static String SLIDESIZE_CAPTION = "[!]";
   private static String STANDARD_SIZE = "[!]";
   private static String ORIGINAL_SIZE = "[!]";
   private static String SCREEN_RES_SIZE = "[!]";
   private static String CUSTOM_SIZE = "[!]";
   private static String STANDARD_SIZE_DESC = "[!]";
   private static String ORIGINAL_SIZE_DESC = "[!]";
   private static String ORIGINAL_SIZE_DESC2 = "[!]";
   private static String SCREEN_RES_SIZE_DESC = "[!]";
   private static char   MNEM_STANDARD = '?';
   private static char   MNEM_ORIGINAL = '?';
   private static char   MNEM_SCREEN = '?';
   private static char   MNEM_CUSTOM = '?';
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/SlideSizePanel_",
             "en");
         SLIDESIZE_CAPTION = l.getLocalized("SLIDESIZE_CAPTION");
         STANDARD_SIZE = l.getLocalized("STANDARD_SIZE");
         ORIGINAL_SIZE = l.getLocalized("ORIGINAL_SIZE");
         SCREEN_RES_SIZE = l.getLocalized("SCREEN_RES_SIZE");
         CUSTOM_SIZE = l.getLocalized("CUSTOM_SIZE");
         STANDARD_SIZE_DESC = l.getLocalized("STANDARD_SIZE_DESC");
         ORIGINAL_SIZE_DESC = l.getLocalized("ORIGINAL_SIZE_DESC");
         ORIGINAL_SIZE_DESC2 = l.getLocalized("ORIGINAL_SIZE_DESC2");
         SCREEN_RES_SIZE_DESC = l.getLocalized("SCREEN_RES_SIZE_DESC");
         MNEM_STANDARD = l.getLocalized("MNEM_STANDARD").charAt(0);
         MNEM_ORIGINAL = l.getLocalized("MNEM_ORIGINAL").charAt(0);
         MNEM_SCREEN = l.getLocalized("MNEM_SCREEN").charAt(0);
         MNEM_CUSTOM = l.getLocalized("MNEM_CUSTOM").charAt(0);
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open SlideSizePanel locale database!\n"+
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private String[] SCREEN_RES_SIZES = null;
   //private Dimension[] screenResSlideSizes_ = null;

   private JRadioButton standardSizeRadio_;
   private JRadioButton originalSizeRadio_;
   private JRadioButton screenResSizeRadio_;
   private JRadioButton customSizeRadio_;
   private JComboBox    screenResSizesBox_;

   private CustomSizePanel customSizePanel_;

   private LazyTextArea standardSizeLabel_;
   private LazyTextArea originalSizeLabel_;
   private LazyTextArea screenResSizeLabel_;

   private PublisherWizardData pwData_;

   private Dimension initSlideSize_;
   private double slideScale_;
   private int standardSlideWidth_;
   private int standardSlideHeight_;
   private int customSlideWidth_;
   private int customSlideHeight_;
   private int deltaWidth_;
   private int deltaHeight_;
   private int minScreenWidth_;
   private int minScreenHeight_;

   private ExtendedSettingsDialog parent_ = null;
   
   public SlideSizePanel(ExtendedSettingsDialog parent, PublisherWizardData pwData)
   {
      super();
      
      parent_ = parent;
      pwData_ = pwData;

      initSlideSizes();
      initGui();
   }

//    public Insets getInsets()
//    { 
//       return new Insets(10, 10, 10, 10);
//    }
   
   public PublisherWizardData getWizardData() 
   { 
      int w = getActualSlideSize().width;
      int h = getActualSlideSize().height;

      ProfiledSettings ps = pwData_.GetProfiledSettings();

      ps.SetIntValue("iSlideWidth", w); 
      ps.SetIntValue("iSlideHeight", h); 
      
      ps.SetIntValue("iRmWmScreenResolutionIndex", screenResSizesBox_.getSelectedIndex());
      pwData_.m_CustomPageDimension.width = customSizePanel_.getActualSize().width;
      pwData_.m_CustomPageDimension.height = customSizePanel_.getActualSize().height;

      if (standardSizeRadio_.isSelected()) 
      { 
         ps.SetIntValue("iSlideSizeType", PublisherWizardData.SLIDESIZE_STANDARD); 
         System.out.println("Standard slide size: " + w + "x" + h);
      }
      else if (originalSizeRadio_.isSelected()) 
      { 
         ps.SetIntValue("iSlideSizeType", PublisherWizardData.SLIDESIZE_ORIGINAL); 
         System.out.println("Original slide size: " + w + "x" + h);
      }
      else if (screenResSizeRadio_.isSelected()) 
      { 
         ps.SetIntValue("iSlideSizeType", PublisherWizardData.SLIDESIZE_SCREEN_RES); 
         System.out.println("ScreenRes slide size: " + w + "x" + h);

         pwData_.m_MatchingScreenDimension.width = SCREEN_RESOLUTIONS[screenResSizesBox_.getSelectedIndex()].width;
         pwData_.m_MatchingScreenDimension.height = SCREEN_RESOLUTIONS[screenResSizesBox_.getSelectedIndex()].height;
      }
      else if (customSizeRadio_.isSelected())
      {
         ps.SetIntValue("iSlideSizeType", PublisherWizardData.SLIDESIZE_CUSTOM);
         System.out.println("Custom slide size: " + w + "x" + h);
      }
      
      // The preferred screen size can be calculated from the slide size 
      // and must not be a size of SCREEN_RESOLUTIONS
      pwData_.m_PreferredScreenDimension.width = ps.GetIntValue("iSlideWidth") 
         + StreamingMediaConverter.DELTA_BROWSER_TO_SLIDE_WIDTH;
      pwData_.m_PreferredScreenDimension.height = ps.GetIntValue("iSlideHeight") 
         + StreamingMediaConverter.DELTA_BROWSER_TO_SLIDE_HEIGHT;
      
      return pwData_;
   }

   public Dimension getActualSlideSize()
   {
      Dimension actualSlideSize = new Dimension(0, 0);

      if (standardSizeRadio_.isSelected()) 
      { 
         actualSlideSize.width = standardSlideWidth_; 
         actualSlideSize.height = standardSlideHeight_; 
      }
      else if (originalSizeRadio_.isSelected()) 
      { 
         actualSlideSize.width = initSlideSize_.width; 
         actualSlideSize.height = initSlideSize_.height; 
      }
      else if (screenResSizeRadio_.isSelected()) 
      { 
         //         String customSize = screenResSizesBox_.getSelectedItem().toString(); 
         //         int xPos = customSize.indexOf('x'); 
         //         int x2Pos = customSize.indexOf(' '); 
         //         actualSlideSize.width = Integer.parseInt(customSize.substring(0, xPos)); 
         //         actualSlideSize.height = Integer.parseInt(customSize.substring(xPos + 1, x2Pos));
//          actualSlideSize.width = SCREEN_RESOLUTIONS[screenResSizesBox_.getSelectedIndex()].width;
//          actualSlideSize.height = SCREEN_RESOLUTIONS[screenResSizesBox_.getSelectedIndex()].height;
         actualSlideSize.width = -1;
         actualSlideSize.height = -1;
      }
      else if (customSizeRadio_.isSelected())
      {
         actualSlideSize.width = customSizePanel_.getActualSize().width;
         actualSlideSize.height = customSizePanel_.getActualSize().height;
      }
      
      return actualSlideSize;
   }
   
   public String getDescription()
   {
      return "";
   }

   private void initGui()
   {
      //Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      ProfiledSettings ps = pwData_.GetProfiledSettings();

      boolean useStandardSize  = (ps.GetIntValue("iSlideSizeType") == PublisherWizardData.SLIDESIZE_STANDARD);
      boolean useOriginalSize  = (ps.GetIntValue("iSlideSizeType") == PublisherWizardData.SLIDESIZE_ORIGINAL);
      boolean useScreenResSize = (ps.GetIntValue("iSlideSizeType") == PublisherWizardData.SLIDESIZE_SCREEN_RES);
      boolean useCustomSize    = (ps.GetIntValue("iSlideSizeType") == PublisherWizardData.SLIDESIZE_CUSTOM);

      this.setLayout(new BorderLayout());
      JPanel panel = new JPanel();
      panel.setLayout(null);
///      panel.setBorder(new TitledBorder(SLIDESIZE_CAPTION));
      this.add(panel, BorderLayout.CENTER);

      int y = 20;
      int s1 = 400;
      int s2 = 260;
      int s3 = 40;
      int x1 = 20;
      int x2 = 40;

      ButtonGroup bg = new ButtonGroup();
      RadioListener rl = new RadioListener();

      standardSizeRadio_ = new JRadioButton(STANDARD_SIZE /* + 
                                           "(" + standardSlideWidth_ + "x" +
                                           standardSlideHeight_ + ")"*/, useStandardSize);
      standardSizeRadio_.setFont(headerFont);
      standardSizeRadio_.setSize(s1, 20);
      standardSizeRadio_.setLocation(x1, y);
      standardSizeRadio_.setMnemonic(MNEM_STANDARD);
      standardSizeRadio_.addActionListener(rl);
      panel.add(standardSizeRadio_);
      bg.add(standardSizeRadio_);

      y += 25;
      standardSizeLabel_ = new LazyTextArea(STANDARD_SIZE_DESC, 
                                           new Point(x1 + 20, y), 
                                           new Dimension(s1 - 20, 40),
                                           descFont );
      panel.add(standardSizeLabel_);
      
      y += 40;
      originalSizeRadio_ = new JRadioButton(ORIGINAL_SIZE + 
                                           "(" + initSlideSize_.width + "x" + 
                                           initSlideSize_.height + ")", useOriginalSize);
      originalSizeRadio_.setFont(headerFont);
      originalSizeRadio_.setSize(s1, 20);
      originalSizeRadio_.setLocation(x1, y);
      originalSizeRadio_.setMnemonic(MNEM_ORIGINAL);
      originalSizeRadio_.addActionListener(rl);
      panel.add(originalSizeRadio_);
      bg.add(originalSizeRadio_);

      y += 25;
      String standardSizeText = ORIGINAL_SIZE_DESC + getMinScreenSizeText(initSlideSize_) 
                                + " " + ORIGINAL_SIZE_DESC2;
      originalSizeLabel_ = new LazyTextArea(standardSizeText, 
                                           new Point(x1 + 20, y), 
                                           new Dimension(s1 - 20, 40),
                                           descFont );
      //panel.add(originalSizeLabel_);
      
      y += 40;
      screenResSizeRadio_ = new JRadioButton(SCREEN_RES_SIZE, useScreenResSize);
      screenResSizeRadio_.setFont(headerFont);
      screenResSizeRadio_.setSize(s1, 20);
      screenResSizeRadio_.setLocation(x1, y);
      screenResSizeRadio_.setMnemonic(MNEM_SCREEN);
      screenResSizeRadio_.addActionListener(rl);
      bg.add(screenResSizeRadio_);
      panel.add(screenResSizeRadio_);

      y += 25;
      screenResSizeLabel_ = new LazyTextArea(SCREEN_RES_SIZE_DESC, 
                                             new Point(x2 + 2, y), 
                                             new Dimension(s2, 22),
                                             descFont );
      panel.add(screenResSizeLabel_);
      
      y += 22;
      screenResSizesBox_ = new JComboBox(SCREEN_RES_SIZES);
      screenResSizesBox_.setSize(120, 22);
      screenResSizesBox_.setLocation(x2, y);
      screenResSizesBox_.setEditable(false);
      screenResSizesBox_.setSelectedIndex(ps.GetIntValue("iRmWmScreenResolutionIndex"));
      panel.add(screenResSizesBox_);
      
      y += 40;
      customSizeRadio_ = new JRadioButton(CUSTOM_SIZE, useCustomSize);
      customSizeRadio_.setFont(headerFont);
      customSizeRadio_.setSize(s1, 20);
      customSizeRadio_.setLocation(x1, y);
      customSizeRadio_.setMnemonic(MNEM_CUSTOM);
      customSizeRadio_.addActionListener(rl);
      bg.add(customSizeRadio_);
      panel.add(customSizeRadio_);

      y += 25;
      customSizePanel_ = new CustomSizePanel(340, 25, initSlideSize_);
      customSizePanel_.setMinimumSize(new Dimension(1, 1));
      customSizePanel_.setMaximumSize(new Dimension(9999, 9999));
      customSizePanel_.setLocation(x2, y);
      customSizePanel_.setActualSize(customSlideWidth_, customSlideHeight_);
      customSizePanel_.setEnabledAspectRatioBox(false);
      panel.add(customSizePanel_);
      //System.out.println("customSlideSizePanel: " + initSlideSize_.width + ", " + initSlideSize_.height + " ==> " + customSlideWidth_ + ", " + customSlideHeight_);

      if (pwData_.m_bIsProfileMode)
      {
         if (ps.GetIntValue("iSlideSizeType") != PublisherWizardData.SLIDESIZE_SCREEN_RES)
            customSizePanel_.setActualSize(ps.GetIntValue("iSlideWidth"), 
                                           ps.GetIntValue("iSlideHeight"));
      }

      checkEnabled();
   }

   private void checkEnabled()
   {
      screenResSizesBox_.setEnabled(screenResSizeRadio_.isSelected());

      customSizePanel_.setEnabled(customSizeRadio_.isSelected());
   }

   private void initSlideSizes()
   {
      //screenResSlideSizes_ = new Dimension[NUM_RES];
      SCREEN_RES_SIZES = new String[NUM_RES];

      standardSlideWidth_ = pwData_.m_PreferredSlideDimension.width;
      standardSlideHeight_ = pwData_.m_PreferredSlideDimension.height;

      // Create init values for the custom slide size
      initSlideSize_ = new Dimension(0, 0);
      if (!pwData_.IsDenverDocument())
      {
         initSlideSize_.width = pwData_.GetDocumentData().GetWhiteboardDimension().width;
         initSlideSize_.height = pwData_.GetDocumentData().GetWhiteboardDimension().height;
      }
      else
      {
         initSlideSize_.width = StreamingMediaConverter.STANDARD_SLIDE_WIDTH;
         initSlideSize_.height = StreamingMediaConverter.STANDARD_SLIDE_HEIGHT;
      }

      customSlideWidth_ = (pwData_.m_CustomPageDimension.width > -1) ? 
         pwData_.m_CustomPageDimension.width : initSlideSize_.width;
      customSlideHeight_ = (pwData_.m_CustomPageDimension.height > -1) ? 
         pwData_.m_CustomPageDimension.height : initSlideSize_.height;

      deltaWidth_ = StreamingMediaConverter.STANDARD_SCREEN_WIDTH - standardSlideWidth_;
      deltaHeight_ = StreamingMediaConverter.STANDARD_SCREEN_HEIGHT - standardSlideHeight_;

      // Calculate slide sizes for different screen resolutions
      for (int i = 0; i < SCREEN_RESOLUTIONS.length; i++)
      {
         //screenResSlideSizes_[i] = new Dimension(SCREEN_RESOLUTIONS[i].width - deltaWidth_, 
         //                                        SCREEN_RESOLUTIONS[i].height - deltaHeight_);

         // Consider special case 1280x1024: this resolution has not 4:3 ratio
         // ==> use 1280x960 (4:3) instead.
//          if (SCREEN_RESOLUTIONS[i].equals(new Dimension(1280, 1024)))
//             screenResSlideSizes_[i].height -= 64;

         SCREEN_RES_SIZES[i] = "" 
                               + SCREEN_RESOLUTIONS[i].width + "x" 
                               + SCREEN_RESOLUTIONS[i].height;
      }
   }

   private Dimension getMinScreenSize(Dimension slideSize)
   {
      int idx = 0;
      int w = slideSize.width + deltaWidth_;
      int h = slideSize.height + deltaHeight_;
      for (int i = 0; i < SCREEN_RESOLUTIONS.length; i++)
      {
         idx = i;
         if ( (SCREEN_RESOLUTIONS[i].width >= w) && (SCREEN_RESOLUTIONS[i].height >= h) )
         break;
      }
      
      return SCREEN_RESOLUTIONS[idx];
   }

   private String getMinScreenSizeText(Dimension slideSize)
   {
      Dimension minScreenSize = getMinScreenSize(slideSize);
      String sizeText = minScreenSize.width + "x" + minScreenSize.height;
      return sizeText;
   }
      
   private class RadioListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         checkEnabled();
      }
   }
}
