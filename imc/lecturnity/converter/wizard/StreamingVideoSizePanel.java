package imc.lecturnity.converter.wizard;

import java.awt.*;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

import javax.swing.*;
import javax.swing.border.BevelBorder;
import javax.swing.border.TitledBorder;
import javax.swing.border.LineBorder;
import javax.swing.filechooser.FileFilter;

import javax.swing.JOptionPane;

import java.io.File;

import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.converter.StreamingMediaConverter;
import imc.lecturnity.util.wizards.ButtonPanel;
import imc.lecturnity.util.wizards.WizardButton;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.WizardData;

import imc.lecturnity.util.ui.PopupHelp;
import imc.lecturnity.util.ui.LazyTextArea;

import imc.epresenter.filesdk.util.Localizer;

class StreamingVideoSizePanel extends WizardPanel
{
//   private static String VIDEOSIZE_CAPTION = "[!]";
   private static String CLIPS_ON_VIDEO = "[!]";
   private static String SIZE_VIDEO_AREA = "[!]";
   private static String SIZE_CLIPS_AREA = "[!]";
   private static String CLIPS_SEPARATED = "[!]";
   private static String CLIPS_ON_SLIDES = "[!]";
//   private static String CLIPS_INCLUDED = "[!]";
   private static String PROFILE_SIZE = "[!]";
   private static String MAXCLIP_SIZE = "[!]";
//   private static String WARNING = "[!]";
//   private static String MAXCLIP_WARNING = "[!]";
   private static String ORIGINAL_SIZE = "[!]";
   private static String CUSTOM_SIZE_VIDEO = "[!]";
//   private static String CLIPS_EXCLUDED = "[!]";
   private static String CLIPS_FULL_SIZE = "[!]";
   private static String CLIPS_MAX_SLIDE_SIZE = "[!]";
   private static String CUSTOM_SIZE_CLIPS = "[!]";
   private static String MAXIMUM = "[!]";
   private static String OPTIMIZE_CLIP_SYNC = "[!]";
   private static String DO_NOT_CUT_VIDEOS = "[!]";
   
   private static char   MNEM_CLIPS_ON_VIDEO = '?';
   private static char   MNEM_CLIPS_SEPARATED = '?';
   private static char   MNEM_CLIPS_ON_SLIDES = '?';
   private static char   MNEM_PROFILE_SIZE = '?';
   private static char   MNEM_MAXCLIP_SIZE = '?';
   private static char   MNEM_ORIGINAL_SIZE = '?';
   private static char   MNEM_CUSTOM_SIZE_VIDEO = '?';
   private static char   MNEM_CLIPS_FULL_SIZE = '?';
   private static char   MNEM_CLIPS_MAX_SLIDE_SIZE = '?';
   private static char   MNEM_CUSTOM_SIZE_CLIPS = '?';
   private static char   MNEM_CUSTOM_ASPECT_RATIO_CLIPS = '?';

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/StreamingVideoSizePanel_",
             "en");
//         VIDEOSIZE_CAPTION = l.getLocalized("VIDEOSIZE_CAPTION");
         CLIPS_ON_VIDEO = l.getLocalized("CLIPS_ON_VIDEO");
         SIZE_VIDEO_AREA = l.getLocalized("SIZE_VIDEO_AREA");
         SIZE_CLIPS_AREA = l.getLocalized("SIZE_CLIPS_AREA");
         CLIPS_SEPARATED = l.getLocalized("CLIPS_SEPARATED");
         CLIPS_ON_SLIDES = l.getLocalized("CLIPS_ON_SLIDES");
//         CLIPS_INCLUDED = l.getLocalized("CLIPS_INCLUDED");
         PROFILE_SIZE = l.getLocalized("PROFILE_SIZE");
         MAXCLIP_SIZE = l.getLocalized("MAXCLIP_SIZE");
//         WARNING = l.getLocalized("WARNING");
//         MAXCLIP_WARNING = l.getLocalized("MAXCLIP_WARNING");
         ORIGINAL_SIZE = l.getLocalized("ORIGINAL_SIZE");
         CUSTOM_SIZE_VIDEO = l.getLocalized("CUSTOM_SIZE_VIDEO");
//         CLIPS_EXCLUDED = l.getLocalized("CLIPS_EXCLUDED");
         CLIPS_FULL_SIZE = l.getLocalized("CLIPS_FULL_SIZE");
         CLIPS_MAX_SLIDE_SIZE = l.getLocalized("CLIPS_MAX_SLIDE_SIZE");
         CUSTOM_SIZE_CLIPS = l.getLocalized("CUSTOM_SIZE_CLIPS");
         MAXIMUM = l.getLocalized("MAXIMUM");
         OPTIMIZE_CLIP_SYNC = l.getLocalized("OPTIMIZE_CLIP_SYNC");
         DO_NOT_CUT_VIDEOS = l.getLocalized("DO_NOT_CUT_VIDEOS");

         MNEM_CLIPS_ON_VIDEO=l.getLocalized("MNEM_CLIPS_ON_VIDEO").charAt(0);
         MNEM_CLIPS_SEPARATED=l.getLocalized("MNEM_CLIPS_SEPARATED").charAt(0);
         MNEM_CLIPS_ON_SLIDES=l.getLocalized("MNEM_CLIPS_ON_SLIDES").charAt(0);
         MNEM_PROFILE_SIZE=l.getLocalized("MNEM_PROFILE_SIZE").charAt(0);
         MNEM_MAXCLIP_SIZE=l.getLocalized("MNEM_MAXCLIP_SIZE").charAt(0);
         MNEM_ORIGINAL_SIZE=l.getLocalized("MNEM_ORIGINAL_SIZE").charAt(0);
         MNEM_CUSTOM_SIZE_VIDEO=l.getLocalized("MNEM_CUSTOM_SIZE_VIDEO").charAt(0);
         MNEM_CLIPS_FULL_SIZE=l.getLocalized("MNEM_CLIPS_FULL_SIZE").charAt(0);
         MNEM_CLIPS_MAX_SLIDE_SIZE=l.getLocalized("MNEM_CLIPS_MAX_SLIDE_SIZE").charAt(0);
         MNEM_CUSTOM_SIZE_CLIPS=l.getLocalized("MNEM_CUSTOM_SIZE_CLIPS").charAt(0);
         MNEM_CUSTOM_ASPECT_RATIO_CLIPS=l.getLocalized("MNEM_CUSTOM_ASPECT_RATIO_CLIPS").charAt(0);
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open StreamingVideoSizePanel locale database!\n"+
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

//   private static final String[] CUSTOM_SIZES = {"160x120", "176x144", "240x180", "320x240", "480x360", "640x480", "800x600", "1024x768"};

   private JRadioButton clipsOnVideoRadio_;
   private JRadioButton clipsOnSlidesRadio_;
   private JCheckBox clipsSeparatedBox_;
   private JRadioButton videoStandardSizeRadio_;
   private JRadioButton videoOriginalSizeRadio_;
   private JRadioButton videoMaxClipSizeRadio_;
   private JRadioButton videoCustomSizeRadio_;
   private JRadioButton clipsFullSizeRadio_;
   private JRadioButton clipsMaxSlideSizeRadio_;
   private JRadioButton clipsCustomSizeRadio_;
   private JCheckBox m_boxClipSync;
   private JCheckBox m_boxVideoCut;

   private CustomSizePanel customVideoSizePanel_;
   private CustomSizePanel customClipSizePanel_;

   private JPanel videoAreaSizePanel_; 
   private JPanel clipsAreaSizePanel_; 

//   private LazyTextArea maxClipSizeLabel_;

   private PublisherWizardData pwData_;

   private Dimension profileVideoSize_;

   private int maxVideoWidth_     = -1;
   private int maxVideoHeight_    = -1;
   private int customVideoWidth_  = -1;
   private int customVideoHeight_ = -1;
   private int customClipsWidth_  = -1;
   private int customClipsHeight_ = -1;
   private int actualSlideWidth_  = -1;
   private int actualSlideHeight_ = -1;
   
//   private boolean hasNormalVideoOnly_ = false;
   private boolean hasVideo_ = false;
   private boolean hasClips_ = false;

//   private ExtendedSettingsDialog parent_ = null;
   
   public StreamingVideoSizePanel(ExtendedSettingsDialog parent, PublisherWizardData pwData)
   {
      super();
      
//      parent_ = parent;
      pwData_ = pwData;
      profileVideoSize_ = new Dimension(StreamingMediaConverter.STANDARD_VIDEO_WIDTH, 
                                        StreamingMediaConverter.STANDARD_VIDEO_HEIGHT);

      initVideoSizes();
      initGui();
   }

//    public Insets getInsets()
//    { 
//       return new Insets(10, 10, 10, 10);
//    }
   
   public PublisherWizardData getWizardData()
   {
      DocumentData dd = pwData_.GetDocumentData();
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      // Check, if clips are separated from video
      if (hasClips_ && !pwData_.IsDenverDocument())
      {
         if (hasVideo_ && clipsSeparatedBox_.isSelected())
         {
            // Clips separated from video
            ps.SetBoolValue("bShowClipsOnSlides", true);
         }
         else if (!hasVideo_ && clipsOnSlidesRadio_.isSelected())
         {
            // Clips separated from video
            ps.SetBoolValue("bShowClipsOnSlides", true);
         }
         else
         {
            // Clips combined with video / video only
            ps.SetBoolValue("bShowClipsOnSlides", false);
         }
      }
      else
      {
         // Clips combined with video / video only
//         ps.SetBoolValue("bShowClipsOnSlides", false);
         
         // Note: Having this enabled actually changes the value.
         // This interferes with FlashSizePanel and the calculation
         // in the "User defined" case: movie size made smaller.
      }

      // Check the radio button group "video" 
      if (hasVideo_ || hasClips_)
      {
         pwData_.m_CustomVideoDimension.width = customVideoSizePanel_.getActualSize().width;
         pwData_.m_CustomVideoDimension.height = customVideoSizePanel_.getActualSize().height;
   
         if (videoStandardSizeRadio_.isEnabled() && videoStandardSizeRadio_.isSelected())
         {
            ps.SetIntValue("iVideoSizeType", PublisherWizardData.VIDEOSIZE_STANDARD);
   
            ps.SetIntValue("iVideoWidth", profileVideoSize_.width);
            ps.SetIntValue("iVideoHeight", profileVideoSize_.height);
         }
         else if (hasVideo_ && videoOriginalSizeRadio_.isEnabled() && videoOriginalSizeRadio_.isSelected())
         {
            ps.SetIntValue("iVideoSizeType", PublisherWizardData.VIDEOSIZE_ORIGINAL);
   
            ps.SetIntValue("iVideoWidth", dd.GetVideoDimension().width);
            ps.SetIntValue("iVideoHeight", dd.GetVideoDimension().height);
         }
         else if (hasClips_ && videoMaxClipSizeRadio_.isEnabled() && videoMaxClipSizeRadio_.isSelected())
         {
            ps.SetIntValue("iVideoSizeType", PublisherWizardData.VIDEOSIZE_MAXCLIP);
   
            ps.SetIntValue("iVideoWidth", maxVideoWidth_);
            ps.SetIntValue("iVideoHeight", maxVideoHeight_);
         }
         else if (videoCustomSizeRadio_.isEnabled() && videoCustomSizeRadio_.isSelected())
         {
            ps.SetIntValue("iVideoSizeType", PublisherWizardData.VIDEOSIZE_CUSTOM);
   
            Dimension customSize = customVideoSizePanel_.getActualSize();
            int w = customSize.width;
            int h = customSize.height;
   
            ps.SetIntValue("iVideoWidth", w);
            ps.SetIntValue("iVideoHeight", h);
         }
      }

      // Check the radio button group "clips" 
      if (hasClips_ && !pwData_.IsDenverDocument())
      {
         pwData_.m_CustomClipsDimension.width = customClipSizePanel_.getActualSize().width;
         pwData_.m_CustomClipsDimension.height = customClipSizePanel_.getActualSize().height;

         if (clipsFullSizeRadio_.isSelected())
         {
            ps.SetIntValue("iClipSizeType", PublisherWizardData.CLIPSIZE_STANDARD);
   
            ps.SetIntValue("iMaxCustomClipWidth", maxVideoWidth_);
            ps.SetIntValue("iMaxCustomClipHeight", maxVideoHeight_);
         }
         else if (clipsMaxSlideSizeRadio_.isSelected())
         {
            ps.SetIntValue("iClipSizeType", PublisherWizardData.CLIPSIZE_MAXSLIDE);
   
            int w = actualSlideWidth_;
            int h = actualSlideHeight_;
            
            System.out.println("MaxSlide clips size: " + w + "x" + h);
   
            ps.SetIntValue("iMaxCustomClipWidth", w);
            ps.SetIntValue("iMaxCustomClipHeight", h);
         }
         else if (clipsCustomSizeRadio_.isSelected())
         {
            ps.SetIntValue("iClipSizeType", PublisherWizardData.CLIPSIZE_CUSTOM);
   
            Dimension customSize = customClipSizePanel_.getActualSize();
            int w = customSize.width;
            int h = customSize.height;
            System.out.println("Custom clips size: " + w + "x" + h);
   
            ps.SetIntValue("iMaxCustomClipWidth", w);
            ps.SetIntValue("iMaxCustomClipHeight", h);
         }
      }

      if (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH)
      {
         if (m_boxClipSync != null)
            ps.SetBoolValue("bFlashOptimizeClipSync", m_boxClipSync.isSelected());
         if (m_boxVideoCut != null)
            ps.SetBoolValue("bFlashDoNotCutVideos", m_boxVideoCut.isSelected());
      }
   
      return pwData_;
   }

   public String getDescription()
   {
      return "";
   }

   public void updateSlideSize(Dimension slideSize)
   {
      updateSlideSize(slideSize.width, slideSize.height);
   }
   
   public void updateSlideSize(int slideWidth, int slideHeight)
   {
      actualSlideWidth_ = slideWidth; 
      actualSlideHeight_ = slideHeight;

      if (clipsMaxSlideSizeRadio_ != null)
      {
         String strClipsMaxSlideSize = CLIPS_MAX_SLIDE_SIZE; // + " (" + slideWidth 
//                                        + "x" + slideHeight + ")";
         clipsMaxSlideSizeRadio_.setText(strClipsMaxSlideSize);

         checkEnabled();
      }
   }

   private void initGui()
   {
      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      DocumentData dd = pwData_.GetDocumentData();
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      boolean isDenverMode = pwData_.IsDenverDocument();
      boolean isRealExport = (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_REAL);
      boolean isFlashExport = (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH);
      boolean isScormEnabled = ps.GetBoolValue("bScormEnabled");
      boolean isScormStrict = ps.GetBoolValue("bScormStrict");
      boolean separateClipsFromVideo = ps.GetBoolValue("bShowClipsOnSlides");
      boolean useVideoSizeStandard = (ps.GetIntValue("iVideoSizeType") == PublisherWizardData.VIDEOSIZE_STANDARD);
      boolean useVideoSizeOriginal = (ps.GetIntValue("iVideoSizeType") == PublisherWizardData.VIDEOSIZE_ORIGINAL);
      boolean useVideoSizeMaxClip  = (ps.GetIntValue("iVideoSizeType") == PublisherWizardData.VIDEOSIZE_MAXCLIP);
      boolean useVideoSizeCustom   = (ps.GetIntValue("iVideoSizeType") == PublisherWizardData.VIDEOSIZE_CUSTOM);
      boolean useClipSizeStandard  = (ps.GetIntValue("iClipSizeType") == PublisherWizardData.CLIPSIZE_STANDARD);
      boolean useClipSizeSlide     = (ps.GetIntValue("iClipSizeType") == PublisherWizardData.CLIPSIZE_MAXSLIDE);
      boolean useClipSizeCustom    = (ps.GetIntValue("iClipSizeType") == PublisherWizardData.CLIPSIZE_CUSTOM);
      boolean optimizeClipSync = false;
      boolean doNotCutVideos = false;
      boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && ps.GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;

      if (isFlashExport)
      {
         optimizeClipSync = ps.GetBoolValue("bFlashOptimizeClipSync");
         doNotCutVideos = ps.GetBoolValue("bFlashDoNotCutVideos");
      }
      int origVideoWidth  = dd.GetVideoDimension().width;
      int origVideoHeight = dd.GetVideoDimension().height;

//      hasNormalVideoOnly_ = pwData_.HasNormalVideo() && !pwData_.HasClips();
      hasVideo_ = pwData_.HasNormalVideo(); 
      hasClips_ = pwData_.HasClips();
	  
      if ( bIsGenericProfile ) {
         // enable all media types
         hasVideo_ = true;
		 hasClips_ = true;
      }

      this.setLayout(new BorderLayout());
      JPanel panel = new JPanel();
      panel.setLayout(null);
///      panel.setBorder(new TitledBorder(VIDEOSIZE_CAPTION));
      this.add(panel, BorderLayout.CENTER);

      int y = 20;
      int y1 = 0;
      int y2 = 0;
      int s0 = 280;
      int s1 = 380;
      int x0 = 20;
      int x1 = 40;
      int w = 420;

      ButtonGroup bgMaster = new ButtonGroup();
      ButtonGroup bgSlave1 = new ButtonGroup();
      ButtonGroup bgSlave2 = new ButtonGroup();

      RadioListener rl = new RadioListener();
      CheckBoxListener cbl = new CheckBoxListener();


      if (hasVideo_ || hasClips_)
      {
         if (!hasVideo_ && !isDenverMode) // clips only
         {
            clipsOnVideoRadio_ = new JRadioButton(CLIPS_ON_VIDEO, !separateClipsFromVideo);
            clipsOnVideoRadio_.setFont(headerFont);
            clipsOnVideoRadio_.setSize(s0, 20);
            clipsOnVideoRadio_.setLocation(x0, y);
            clipsOnVideoRadio_.addActionListener(rl);
            clipsOnVideoRadio_.setMnemonic(MNEM_CLIPS_ON_VIDEO);
            panel.add(clipsOnVideoRadio_);
            bgMaster.add(clipsOnVideoRadio_);
            y += 25;
         }

         videoAreaSizePanel_ = new JPanel();
         videoAreaSizePanel_.setLayout(null);
         String BorderTitle = (hasVideo_) ? SIZE_VIDEO_AREA : SIZE_CLIPS_AREA;
         videoAreaSizePanel_.setBorder(new TitledBorder(" " + BorderTitle + " "));
         videoAreaSizePanel_.setLocation(x0, y);
         panel.add(videoAreaSizePanel_);
         
         y1 += 20;
         videoStandardSizeRadio_ = new JRadioButton(PROFILE_SIZE + 
                                           "(" + profileVideoSize_.width + "x" +
                                           profileVideoSize_.height + ")", useVideoSizeStandard);
         videoStandardSizeRadio_.setSize(s1, 20);
         videoStandardSizeRadio_.setLocation(x0, y1);
         videoStandardSizeRadio_.addActionListener(rl);
         videoStandardSizeRadio_.setMnemonic(MNEM_PROFILE_SIZE);
         videoAreaSizePanel_.add(videoStandardSizeRadio_);
         bgSlave1.add(videoStandardSizeRadio_);

         if (hasVideo_)
         {
            y1 += 22;
            String sizeText = bIsGenericProfile ? "" : " (" + origVideoWidth + "x" + origVideoHeight + ")";             
            videoOriginalSizeRadio_ = new JRadioButton(ORIGINAL_SIZE + sizeText, 
                                                      useVideoSizeOriginal);
            videoOriginalSizeRadio_.setSize(s1, 20);
            videoOriginalSizeRadio_.setLocation(x0, y1);
            videoOriginalSizeRadio_.addActionListener(rl);
            videoOriginalSizeRadio_.setMnemonic(MNEM_ORIGINAL_SIZE);
            videoAreaSizePanel_.add(videoOriginalSizeRadio_);
            bgSlave1.add(videoOriginalSizeRadio_);
         }

         if (hasClips_)
         {
            y1 += 22;
            String sizeText = bIsGenericProfile ? "" : " (" + maxVideoWidth_ + "x" + maxVideoHeight_ + ")";             
            videoMaxClipSizeRadio_ = new JRadioButton(MAXCLIP_SIZE + sizeText, 
                                                      useVideoSizeMaxClip);
            videoMaxClipSizeRadio_.setSize(s1, 20);
            videoMaxClipSizeRadio_.setLocation(x0, y1);
            videoMaxClipSizeRadio_.addActionListener(rl);
            videoMaxClipSizeRadio_.setMnemonic(MNEM_MAXCLIP_SIZE);
            videoAreaSizePanel_.add(videoMaxClipSizeRadio_);
            bgSlave1.add(videoMaxClipSizeRadio_);
         }

         y1 += 22;
         videoCustomSizeRadio_ = new JRadioButton(CUSTOM_SIZE_VIDEO, useVideoSizeCustom);
         videoCustomSizeRadio_.setSize(s1, 20);
         videoCustomSizeRadio_.setLocation(x0, y1);
         videoCustomSizeRadio_.addActionListener(rl);
         videoCustomSizeRadio_.setMnemonic(MNEM_CUSTOM_SIZE_VIDEO);
         bgSlave1.add(videoCustomSizeRadio_);
         videoAreaSizePanel_.add(videoCustomSizeRadio_);

         y1 += 20;
         Dimension defaultSize;
         if (hasVideo_)
            defaultSize = new Dimension(customVideoWidth_, customVideoHeight_);
         else
            defaultSize = new Dimension(maxVideoWidth_, maxVideoHeight_);
         
         if (defaultSize.width <= 0 || defaultSize.height <= 0)
             defaultSize = new Dimension(StreamingMediaConverter.STANDARD_VIDEO_WIDTH, 
                                         StreamingMediaConverter.STANDARD_VIDEO_HEIGHT);

         customVideoSizePanel_ = new CustomSizePanel(340, 25, defaultSize);
         customVideoSizePanel_.setMinimumSize(new Dimension(1, 1));
         customVideoSizePanel_.setMaximumSize(new Dimension(9999, 9999));
         customVideoSizePanel_.setLocation(x1, y1);
//         customVideoSizePanel_.setActualSize(customVideoWidth_, customVideoHeight_);
         videoAreaSizePanel_.add(customVideoSizePanel_);
         //System.out.println("customVideoSizePanel: " + defaultSize.width + ", " + defaultSize.height + " ==> " + customVideoWidth_ + ", " + customVideoHeight_);

         y1 += 30;
         videoAreaSizePanel_.setSize(w, y1);

         y += y1;
         y += 20;
      }

      if (hasClips_ && !isDenverMode)      
      {
         if (hasVideo_)
         {
            clipsSeparatedBox_ = new JCheckBox(CLIPS_SEPARATED, separateClipsFromVideo);
            clipsSeparatedBox_.setSize(s1, 20);
            clipsSeparatedBox_.setLocation(x0, y);
            clipsSeparatedBox_.addActionListener(cbl);
            clipsSeparatedBox_.setMnemonic(MNEM_CLIPS_SEPARATED);

            // with strict SCORM support separation is not allowed
            if (isRealExport && isScormEnabled && isScormStrict)
               clipsSeparatedBox_.setEnabled(false);
 
            panel.add(clipsSeparatedBox_);
         }
         else
         {
            clipsOnSlidesRadio_ = new JRadioButton(CLIPS_ON_SLIDES, separateClipsFromVideo);
            clipsOnSlidesRadio_.setFont(headerFont);
            clipsOnSlidesRadio_.setSize(s0, 20);
            clipsOnSlidesRadio_.setLocation(x0, y);
            clipsOnSlidesRadio_.addActionListener(rl);
            clipsOnSlidesRadio_.setMnemonic(MNEM_CLIPS_ON_SLIDES);

            // with strict SCORM support separation is not allowed
            if (isRealExport && isScormEnabled && isScormStrict)
               clipsOnSlidesRadio_.setEnabled(false);
 
            panel.add(clipsOnSlidesRadio_);
            bgMaster.add(clipsOnSlidesRadio_);
         }
         y += 25;
         
         clipsAreaSizePanel_ = new JPanel();
         clipsAreaSizePanel_.setLayout(null);
         clipsAreaSizePanel_.setBorder(new TitledBorder(" " + SIZE_CLIPS_AREA + " "));
         clipsAreaSizePanel_.setLocation(x0, y);
         panel.add(clipsAreaSizePanel_);
         
         y2 += 20;
         String sizeText = bIsGenericProfile ? "" : " (" + MAXIMUM + " " + maxVideoWidth_ + "x" + maxVideoHeight_ + ")";             
         clipsFullSizeRadio_ = new JRadioButton(CLIPS_FULL_SIZE + sizeText, 
                                                useClipSizeStandard);
         clipsFullSizeRadio_.setSize(s1, 20);
         clipsFullSizeRadio_.setLocation(x0, y2);
         clipsFullSizeRadio_.addActionListener(rl);
         clipsFullSizeRadio_.setMnemonic(MNEM_CLIPS_FULL_SIZE);
         clipsAreaSizePanel_.add(clipsFullSizeRadio_);
         bgSlave2.add(clipsFullSizeRadio_);
   
         y2 += 22;
         clipsMaxSlideSizeRadio_ = new JRadioButton(CLIPS_MAX_SLIDE_SIZE /* + " (" + actualSlideWidth_ + "x" + actualSlideHeight_ + ")" */,
                                                    useClipSizeSlide);
         clipsMaxSlideSizeRadio_.setSize(s1, 20);
         clipsMaxSlideSizeRadio_.setLocation(x0, y2);
         clipsMaxSlideSizeRadio_.addActionListener(rl);
         clipsMaxSlideSizeRadio_.setMnemonic(MNEM_CLIPS_MAX_SLIDE_SIZE);
         clipsAreaSizePanel_.add(clipsMaxSlideSizeRadio_);
         bgSlave2.add(clipsMaxSlideSizeRadio_);

         y2 += 22;
         clipsCustomSizeRadio_ = new JRadioButton(CUSTOM_SIZE_CLIPS, useClipSizeCustom);
         clipsCustomSizeRadio_.setSize(s1, 20);
         clipsCustomSizeRadio_.setLocation(x0, y2);
         clipsCustomSizeRadio_.addActionListener(rl);
         clipsCustomSizeRadio_.setMnemonic(MNEM_CUSTOM_SIZE_CLIPS);
         bgSlave2.add(clipsCustomSizeRadio_);
         clipsAreaSizePanel_.add(clipsCustomSizeRadio_);

         y2 += 20;
         Dimension defaultSize = new Dimension(640, 480);
         customClipSizePanel_ = new CustomSizePanel(340, 25, defaultSize, MNEM_CUSTOM_ASPECT_RATIO_CLIPS);
         customClipSizePanel_.setMinimumSize(new Dimension(1, 1));
         customClipSizePanel_.setMaximumSize(new Dimension(9999, 9999));
         customClipSizePanel_.setLocation(x1, y2);
         customClipSizePanel_.setActualSize(customClipsWidth_, customClipsHeight_);
         clipsAreaSizePanel_.add(customClipSizePanel_);

         y2 += 30;
         clipsAreaSizePanel_.setSize(w, y2);
         
         y += y2;
      }
      
      y += 15;
    
      if (isFlashExport)
      {
         int h = 20;
         
         if (hasClips_ || bIsGenericProfile)
         {
            m_boxClipSync = new JCheckBox(OPTIMIZE_CLIP_SYNC);
            m_boxClipSync.setSize(w, h);
            m_boxClipSync.setLocation(x0, y);
            panel.add(m_boxClipSync);
         
            m_boxClipSync.setSelected(optimizeClipSync);
      
            y += 25;
         }
   
         if (hasVideo_ || hasClips_ || bIsGenericProfile)
         {
            m_boxVideoCut = new JCheckBox(DO_NOT_CUT_VIDEOS);
            m_boxVideoCut.setSize(w, h);
            m_boxVideoCut.setLocation(x0, y);
            panel.add(m_boxVideoCut);
         
            m_boxVideoCut.setSelected(doNotCutVideos);
         }
      }
      
      // Check which selections are available or not
      checkEnabled();
   }

   
   private void checkEnabled()
   {
      boolean isDenverMode = false;
      boolean isFlashExport = false;
      boolean isPspDesign = false;
      int origVideoWidth  = -1;
      int origVideoHeight = -1;

      DocumentData dd = pwData_.GetDocumentData();
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      isDenverMode = pwData_.IsDenverDocument();
      isFlashExport = (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH);
      if (isFlashExport)
         isPspDesign = (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_PSP);
      origVideoWidth  = dd.GetVideoDimension().width;
      origVideoHeight = dd.GetVideoDimension().height;
      
      if (hasClips_ && !isDenverMode)
      {
         // Check the 'Master' checkbox resp. button group (Video -- Clips)
         if (hasVideo_)
         {
            doEnablePanel(clipsAreaSizePanel_, clipsSeparatedBox_.isSelected());
         }
         else
         {
            doEnablePanel(clipsAreaSizePanel_, clipsOnSlidesRadio_.isSelected());
            doEnablePanel(videoAreaSizePanel_, !clipsOnSlidesRadio_.isSelected());
         }

         customClipSizePanel_.setEnabled(clipsCustomSizeRadio_.isEnabled()
                                         && clipsCustomSizeRadio_.isSelected());
         
//          if ( (maxVideoWidth_ <= actualSlideWidth_) 
//             && (maxVideoHeight_ <= actualSlideHeight_) )
//          {
//             // no clip is larger than slide size
//             clipsMaxSlideSizeRadio_.setEnabled(false);
//
//             if (clipsMaxSlideSizeRadio_.isSelected())
//             {
//                // In this case we have to change the selection
//                clipsFullSizeRadio_.setSelected(true);
//             }
//          }
      }

      boolean bIsSmallOrigVideo = ((origVideoWidth <= profileVideoSize_.width)
                                   && (origVideoHeight <= profileVideoSize_.height));
      boolean bIsSmallMaxVideo = ((maxVideoWidth_ <= profileVideoSize_.width)
                                  && (maxVideoWidth_ <= profileVideoSize_.height));
      
      boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && ps.GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
      if ( bIsGenericProfile ) {
          // BUG 5596: Standard video resolution disabled
          // override to be document independent
          // TODO: check should be performed in writer classes
          bIsSmallOrigVideo = false;
          bIsSmallMaxVideo = false;          
      }          
         
      if (hasVideo_)
      {
         customVideoSizePanel_.setEnabled(videoCustomSizeRadio_.isEnabled()
                                          && videoCustomSizeRadio_.isSelected());
   
         // Special case I: 
         // No small video should be scaled up to default size (320x240)
         if (bIsSmallOrigVideo || bIsSmallMaxVideo)
         {
            // if it has clips their dimensions are reflected in maxVideoWidth_
            // and maxVideoHeight_ otherwise it's the size of the video itself

            if (bIsSmallOrigVideo)
               videoStandardSizeRadio_.setEnabled(false);
            if (bIsSmallMaxVideo && hasClips_ && !clipsSeparatedBox_.isSelected())
               videoStandardSizeRadio_.setEnabled(false);

            if (!videoStandardSizeRadio_.isEnabled() && videoStandardSizeRadio_.isSelected())
            {
               // In this case we have to change the selection
               if (hasClips_)
               {
                  // this selection is checked once again in 'Special case II' below
                  // because of 'if (hasClips_)' above we are sure we have this radio button
                  videoMaxClipSizeRadio_.setSelected(true);
               }
               else
               {
                  // because of 'if (hasVideo_)' in this branch we are sure we have this radio button
                  videoOriginalSizeRadio_.setSelected(true);
               }
            }
         }
         
         // Special case II: 
         // "Max. clip size" in the videoAreaSizePanel should be disabled if 
         // we have video & clips and clips are separated from video 
         if (hasClips_)
         {
            // because of 'if (hasClips_)' above we are sure we have this radio button
            videoMaxClipSizeRadio_.setEnabled(!clipsSeparatedBox_.isSelected());
            
            if (!videoMaxClipSizeRadio_.isEnabled() && videoMaxClipSizeRadio_.isSelected())
            {
               // In this case we have to change the selection
               if ( bIsSmallOrigVideo || bIsSmallMaxVideo
                  || !videoStandardSizeRadio_.isEnabled() )
               {
                  // because of 'if (hasVideo_)' in this branch we are sure we have this radio button
                  videoOriginalSizeRadio_.setSelected(true);
               }
               else
               {
                  videoStandardSizeRadio_.setSelected(true);
               }
            }
         }
      }

      // Special case III: 'Denver' documents
      // 
      if (hasClips_ && isDenverMode)
      {
         customVideoSizePanel_.setEnabled(videoCustomSizeRadio_.isEnabled()
                                          && videoCustomSizeRadio_.isSelected());

         // It makes no sense to rescale a denver document to standard size
         videoStandardSizeRadio_.setEnabled(false);
         
         // So there are two choices left: videoCustomSize and videoMaxClipSize
         // if videoStandardSize was selected, it is deselected in the next step
         if (!videoCustomSizeRadio_.isSelected())
         {
            // In this case we have to change the selection
            // because of 'if (hasClips_)' above we are sure we have this radio button
            videoMaxClipSizeRadio_.setSelected(true);
         }
         
         // Another special case: Sony PSP design: 
         if (isFlashExport && isPspDesign)
         {
            // Not necessary any more, because this dialog is not visible
            //videoCustomSizeRadio_.setEnabled(false);
            //customVideoSizePanel_.setEnabled(false);
            //videoMaxClipSizeRadio_.setSelected(true);
         }
      }
   }

   private void initVideoSizes()
   {
      DocumentData dd = pwData_.GetDocumentData();
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      Dimension origVideoSize = new Dimension(dd.GetVideoDimension().width, 
                                              dd.GetVideoDimension().height);
      // Calculate the maximum size of the video
      Dimension maxVideoSize = SelectPresentationWizardPanel.getMaximumVideoSize(pwData_, true);
      Dimension customVideoSize = new Dimension(pwData_.m_CustomVideoDimension.width, 
                                                pwData_.m_CustomVideoDimension.height);
      Dimension customClipDimension = new Dimension(pwData_.m_CustomClipsDimension.width, 
                                                    pwData_.m_CustomClipsDimension.height);
      boolean hasVideo = pwData_.HasNormalVideo();

      actualSlideWidth_ = ps.GetIntValue("iSlideWidth");
      actualSlideHeight_ = ps.GetIntValue("iSlideHeight");

      maxVideoWidth_ = maxVideoSize.width;
      maxVideoHeight_ = maxVideoSize.height;

      if (hasVideo)
      {
         customVideoWidth_ = (customVideoSize.width > -1) ? customVideoSize.width : origVideoSize.width;
         customVideoHeight_ = (customVideoSize.height > -1) ? customVideoSize.height : origVideoSize.height;
      }
      else
      {
         customVideoWidth_ = (customVideoSize.width > -1) ? customVideoSize.width : maxVideoSize.width;
         customVideoHeight_ = (customVideoSize.height > -1) ? customVideoSize.height : maxVideoSize.height;
      }

//       customClipsWidth_ = (customClipDimension.width > -1) ? customClipDimension.width : actualSlideWidth_;
//       customClipsHeight_ = (customClipDimension.height > -1) ? customClipDimension.height : actualSlideHeight_;
      customClipsWidth_ = (customClipDimension.width > -1) ? customClipDimension.width : 640;
      customClipsHeight_ = (customClipDimension.height > -1) ? customClipDimension.height : 480;
   }
   
   private void doEnablePanel(JPanel p, boolean flag)
   {
      Component[] components = p.getComponents();
      for (int i=0; i<components.length; ++i)
      {
         if (components[i] instanceof JComponent)
            ((JComponent)components[i]).setEnabled(flag);
      }
   }

   private class RadioListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         checkEnabled();
      }
   }

   private class CheckBoxListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         checkEnabled();
      }
   }
}
