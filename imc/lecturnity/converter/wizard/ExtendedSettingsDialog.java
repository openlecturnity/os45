package imc.lecturnity.converter.wizard;

import javax.swing.*;

import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

import java.awt.event.*;

import java.awt.Frame;
import java.awt.Insets;
import java.awt.Container;
import java.awt.Component;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Point;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.util.wizards.ButtonPanel;
import imc.lecturnity.util.wizards.WizardButton;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.StreamingMediaConverter;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.converter.wizard.FlashConvertWizardData;

class ExtendedSettingsDialog extends JFrame
{
   public static final int ACTION_CANCEL = 0;
   public static final int ACTION_OK     = 1;

   private static String CAPTION = "[!]";
   private static String EXTENT_TAB = "[!]";
   private static String SLIDESIZE_TAB = "[!]";
   private static String VIDEOSIZE_TAB = "[!]";
   private static String FLASHSIZE_TAB = "[!]";
   private static String STREAMCUSTOM_TAB = "[!]";
   private static String BITRATE_TAB = "[!]";
   private static String FILE_EXTENSION_TAB = "[!]";
   private static String ERROR = "[!]";
   private static String WARNING = "[!]";
   private static String CHANGE = "[!]";
   private static String CANCEL = "[!]";
   private static String CONTINUE = "[!]";
   private static String VIDEO_SIZE_TOO_BIG = "[!]";
   private static String CLIP_SIZE_TOO_BIG = "[!]";
   private static String VIDEO_SIZE_CRITICAL = "[!]";
   private static String CLIP_SIZE_CRITICAL = "[!]";
   private static String CHANGE_HINT = "[!]";
   private static String SPECIFY_SIZE = "[!]";
//   private static char   MNEM_EXTENT = '?';
//   private static char   MNEM_SLIDESIZE = '?';
//   private static char   MNEM_VIDEOSIZE = '?';

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/ExtendedSettingsDialog_",
             "en");
         CAPTION = l.getLocalized("CAPTION");
         EXTENT_TAB = l.getLocalized("EXTENT_TAB");
         SLIDESIZE_TAB = l.getLocalized("SLIDESIZE_TAB");
         VIDEOSIZE_TAB = l.getLocalized("VIDEOSIZE_TAB");
         FLASHSIZE_TAB = l.getLocalized("FLASHSIZE_TAB");
         STREAMCUSTOM_TAB = l.getLocalized("STREAMCUSTOM_TAB");
         BITRATE_TAB = l.getLocalized("BITRATE_TAB");
         FILE_EXTENSION_TAB = l.getLocalized("FILE_EXTENSION_TAB");
         ERROR = l.getLocalized("ERROR");
         WARNING = l.getLocalized("WARNING");
         CHANGE = l.getLocalized("CHANGE");
         CANCEL = l.getLocalized("CANCEL");
         CONTINUE = l.getLocalized("CONTINUE");
         VIDEO_SIZE_TOO_BIG = l.getLocalized("VIDEO_SIZE_TOO_BIG");
         CLIP_SIZE_TOO_BIG = l.getLocalized("CLIP_SIZE_TOO_BIG");
         VIDEO_SIZE_CRITICAL = l.getLocalized("VIDEO_SIZE_CRITICAL");
         CLIP_SIZE_CRITICAL = l.getLocalized("CLIP_SIZE_CRITICAL");
         CHANGE_HINT = l.getLocalized("CHANGE_HINT");
         SPECIFY_SIZE = l.getLocalized("SPECIFY_SIZE");
//         MNEM_EXTENT = l.getLocalized("MNEM_EXTENT").charAt(0);
//         MNEM_SLIDESIZE = l.getLocalized("MNEM_SLIDESIZE").charAt(0);
//         MNEM_VIDEOSIZE = l.getLocalized("MNEM_VIDEOSIZE").charAt(0);
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open ExtendedVideoDialog locale database!\n"+
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private PublisherWizardData pwData_;

   private JTabbedPane tabbedPane_;

   private ExtentOfConversionPanel panelExtentOfConversion_ ;
   private SlideSizePanel          panelSlideSize_;
   private FlashSizePanel          panelFlashSize_;
   private StreamingVideoSizePanel panelVideoSize_ ;
   private CustomStreamParameters  panelCustomParameters_;
   private WmpFileExtensionPanel   panelWmpFileExtension_;

//   private int actionTaken_ = ACTION_CANCEL;

//   private Frame       parent_      = null;
   private WizardPanel parentPanel_ = null;

   private int         enabledParentButtons_ = 0;

   public ExtendedSettingsDialog(Frame parent, 
                                 WizardPanel parentPanel,
                                 PublisherWizardData pwData) 
   {
//       super(parent, CAPTION, true);
      super(CAPTION);

//      parent_        = parent;
      parentPanel_   = parentPanel;
      pwData_        = pwData;

//       setDefaultCloseOperation(DISPOSE_ON_CLOSE);
      
      initGui();

      centerDialog(parent);

   }

   public void setTabIndex(int tabIndex)
   {
      tabbedPane_.setSelectedIndex(tabIndex);
   }
   
   public void checkEnabled()
   {
      boolean enableExtendedVideoSettings = true;
      boolean isDenverMode = false;
      boolean isPspDesign = false;
      boolean isNewFlashPlayer = false;
      boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && pwData_.GetProfiledSettings().GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
      
      enableExtendedVideoSettings = pwData_.HasNormalVideo() || pwData_.HasClips() || bIsGenericProfile;
      isDenverMode = pwData_.IsDenverDocument();
      isPspDesign = pwData_.GetProfiledSettings().GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_PSP;

      if (parentPanel_ instanceof FlashSettingsWizardPanel)
      {
         // Enable/Disable Flash Size tab depending on the Flash Player type
         if (pwData_.GetProfiledSettings().GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX)
            isNewFlashPlayer = true;
         setFlashSizeTabEnabled(!isNewFlashPlayer);
         if (isNewFlashPlayer && (tabbedPane_.getSelectedIndex() == 0))
         {
            if (enableExtendedVideoSettings)
               setTabIndex(1);
            else
               setTabIndex(2);
         }
      }

      // Consider "Denver" mode:
      // If we have a stand alone clip, we have to disable every tab 
      // except the video tab and stream config tab
      if (isDenverMode)
      {
         //Exception: "Denver" mode and Sony PSP Design
         if (isPspDesign)
         {
            // Disable video size tab
            setVideoTabEnabled(false);
         }
         else
         {
            enableOnlyVideoTab();
         
            int idxCustom = -1;
            if (parentPanel_ instanceof FlashSettingsWizardPanel)
               idxCustom = tabbedPane_.indexOfTab(BITRATE_TAB);
            else
               idxCustom = tabbedPane_.indexOfTab(STREAMCUSTOM_TAB);

            if (idxCustom >= 0)
               tabbedPane_.setEnabledAt(idxCustom, true);

            setVideoTabEnabled(enableExtendedVideoSettings);
         }
      }
      else
         setVideoTabEnabled(enableExtendedVideoSettings);
   }
   
   public void setFlashSizeTabEnabled(boolean isEnabled)
   {
      int idxFlashSize = tabbedPane_.indexOfTab(FLASHSIZE_TAB);
      if (idxFlashSize >= 0)
         tabbedPane_.setEnabledAt(idxFlashSize, isEnabled);
   }
   
   public void setVideoTabEnabled(boolean isEnabled)
   {
      int idxVideo = tabbedPane_.indexOfTab(VIDEOSIZE_TAB);
      if (idxVideo >= 0)
         tabbedPane_.setEnabledAt(idxVideo, isEnabled);
   }
   
   // disable all tabs except video
   public void enableOnlyVideoTab()
   {
      int idxVideo = tabbedPane_.indexOfTab(VIDEOSIZE_TAB);
      if (idxVideo >= 0)
      {
         for (int i = 0; i < (tabbedPane_.getTabCount()); ++i)
         {
            if (i != idxVideo)
               tabbedPane_.setEnabledAt(i, false);
         }
      }
   }
   
   private void initGui()
   {
      addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e)
            {
               doCancel();
            }
         });

      if (parentPanel_ != null)
      {
         setIconImage(parentPanel_.getFrameIcon());
      
         enabledParentButtons_ = parentPanel_.getEnabledButtons();
         parentPanel_.setEnableButton(enabledParentButtons_, false);
      }
      
      Container r = getContentPane();
      r.setLayout(new BorderLayout());

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

      r.add(bp, BorderLayout.SOUTH);

      // The center panel with the tabbed pane
      JPanel center = new JPanel() { 
            public Dimension getPreferredSize() 
            { 
               int h = 360;
               if (pwData_.GetProfiledSettings().GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH)
                  h += 70; // needs more space for check boxes
               
               return new Dimension(480, h); 
            } 
            
            public Insets getInsets()
            {
               return new Insets(7, 7, 0, 7);
            }
         };
      center.setLayout(new BorderLayout());
      r.add(center, BorderLayout.CENTER);

      // Create a tabbed pane to hold the other panels
      tabbedPane_ = new JTabbedPane(JTabbedPane.TOP);
      tabbedPane_.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);
      
      // Add a ChangeListener to the tabbed pane
      MyChangeListener mcl = new MyChangeListener();
      tabbedPane_.addChangeListener(mcl);

      // Add the panel with the extent of conversion
      // (not used for Flash)
      if (!(parentPanel_ instanceof FlashSettingsWizardPanel))
      {
         panelExtentOfConversion_ = new ExtentOfConversionPanel(this, pwData_);
         tabbedPane_.addTab(EXTENT_TAB, panelExtentOfConversion_);
         //tabbedPane_.setMnemonicAt(0, MNEM_EXTENT);
      }

      // Add the panel with the slide size
      // (different for Flash --> movie size)
      if (parentPanel_ instanceof FlashSettingsWizardPanel)
      {
         panelFlashSize_ = new FlashSizePanel(this, pwData_);
         tabbedPane_.addTab(FLASHSIZE_TAB, panelFlashSize_);
      }
      else
      {
         panelSlideSize_ = new SlideSizePanel(this, pwData_);
         tabbedPane_.addTab(SLIDESIZE_TAB, panelSlideSize_);
      }
      //tabbedPane_.setMnemonicAt(2, MNEM_SLIDESIZE);
      
      // Add the panel with the video size
      panelVideoSize_ = new StreamingVideoSizePanel(this, pwData_);
      tabbedPane_.addTab(VIDEOSIZE_TAB, panelVideoSize_);
      //tabbedPane_.setMnemonicAt(3, MNEM_VIDEOSIZE);
      
      if (parentPanel_ instanceof WmpEncoderWizardPanel)
      {
         // Add the panel for custom stream parameters
         panelCustomParameters_ = new CustomStreamParameters(this, pwData_);
         tabbedPane_.addTab(STREAMCUSTOM_TAB, panelCustomParameters_);
         
         // Add the panel for WM file extension
         panelWmpFileExtension_ = new WmpFileExtensionPanel(this, pwData_);
         tabbedPane_.addTab(FILE_EXTENSION_TAB, panelWmpFileExtension_);
      }

      if (parentPanel_ instanceof FlashSettingsWizardPanel)
      {
         // Add the panel for custom stream parameters
         panelCustomParameters_ = new CustomStreamParameters(this, pwData_);
         tabbedPane_.addTab(BITRATE_TAB, panelCustomParameters_);
      }


      // Select the actual panel
      // This numbering is defined in PublisherWizardData
      // changed in SelectActionWizardPanel and here.
      // And it must be taken into account in the above tab adding...
      tabbedPane_.setSelectedIndex(pwData_.m_iVideoExtendedType);

      center.add(tabbedPane_, BorderLayout.CENTER);


      // Enable/Disable the panel with the video size
      checkEnabled();
      
      setResizable(false);
      pack();
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

   private void doOk()
   {
      PublisherWizardData origPublisherData = null;

      // Make a copy of the original data
      origPublisherData = new PublisherWizardData();
      pwData_.fillOtherWizardData(origPublisherData);

//      actionTaken_ = ACTION_OK;

      pwData_.m_iVideoExtendedType = tabbedPane_.getSelectedIndex();

      Dimension slideSize = null;

      if (panelExtentOfConversion_ != null)
         pwData_ = panelExtentOfConversion_.getWizardData();
///      if (panelLayout_ != null)
///         pwData_ = (PublisherWizardData)panelLayout_.getWizardData();
      if (panelSlideSize_ != null)
      {
         pwData_ = panelSlideSize_.getWizardData();
         slideSize = panelSlideSize_.getActualSlideSize();
      }
      if (panelVideoSize_ != null)
      {
         if (panelSlideSize_ != null)
            panelVideoSize_.updateSlideSize(slideSize);
         pwData_ = panelVideoSize_.getWizardData();
      }
      if (panelFlashSize_ != null)
      {
         panelFlashSize_.refreshGui();
         pwData_ = panelFlashSize_.getWizardData();
      }
      if (panelCustomParameters_ != null)
         pwData_ = panelCustomParameters_.getWizardData();
      if (panelWmpFileExtension_ != null)
         pwData_ = panelWmpFileExtension_.getWizardData();
      // Re-check Flash size in Denver mode (if the video size maybe has been changed)
      if (pwData_.IsDenverDocument() && panelFlashSize_ != null)
         pwData_ = panelFlashSize_.getWizardData();

      int hr = 0; // SUCCESS
      if (parentPanel_ instanceof FlashSettingsWizardPanel)
         hr = ((FlashSettingsWizardPanel)parentPanel_).checkVideoAndPageSizes((Component)this, true);
      else
         hr = checkVideoSizes((Component)this, pwData_, false, true);
      if (hr < 0)
      {
         // Reset original data
         origPublisherData.fillOtherWizardData(pwData_);
         return;
      }

      dispose();
   }

   public static int checkVideoSizes(Component myPanel, PublisherWizardData pwData) {
       return checkVideoSizes(myPanel, pwData, true);
   }

   public static int checkVideoSizes(Component myPanel, PublisherWizardData pwData, 
                                     boolean bShowWarning) {
       return checkVideoSizes(myPanel, pwData, bShowWarning, false);
   }

   public static int checkVideoSizes(Component myPanel, PublisherWizardData pwData, 
                                     boolean bShowWarning, boolean bIsInternal)
   {
      int hr = 0; // SUCCESS

      ProfiledSettings ps = pwData.GetProfiledSettings();

      // Bugfix 4507 (also 3863): Special check for video and clips size which should not exceed 3 MegaPixel
      // Bugfix 5731: video and clips size should not exceed 1280*1024.
      boolean bExceedsLimit = false;
      boolean bCriticalSizeReached = false;
      int nMaxPixelSize = 3000000;
      int nMaxVideoOrClipWidth = 1280;
      int nMaxVideoOrClipHeight = 1024;

      // Check video
      if (ps.GetIntValue("iVideoWidth") * ps.GetIntValue("iVideoHeight") > nMaxPixelSize)
         bExceedsLimit = true;
      if (bExceedsLimit) {
         String strMsg = VIDEO_SIZE_TOO_BIG;
         if (!bIsInternal)
             strMsg += CHANGE_HINT;
         JOptionPane.showMessageDialog(myPanel, strMsg + "\n",
                                       ERROR, JOptionPane.ERROR_MESSAGE);
         hr = -1;
         return hr;
      }

      if ( (ps.GetIntValue("iVideoWidth") > nMaxVideoOrClipWidth) 
           && (ps.GetIntValue("iVideoHeight") > nMaxVideoOrClipHeight) ) 
          bCriticalSizeReached = true;
      if (bCriticalSizeReached && bShowWarning) {
         if (bIsInternal) {
            JOptionPane.showMessageDialog(myPanel, VIDEO_SIZE_CRITICAL + "\n",
                                          WARNING, JOptionPane.WARNING_MESSAGE);
         } else {
            String[] opt = {CHANGE, CONTINUE};
            int jres = JOptionPane.showOptionDialog(myPanel, VIDEO_SIZE_CRITICAL + CHANGE_HINT + "\n", 
                                                    WARNING, JOptionPane.DEFAULT_OPTION, 
                                                    JOptionPane.WARNING_MESSAGE, 
                                                    null, opt, opt[0]);
            if (0 == jres) {
               // CHANGE was selected
               hr = -1;
               return hr;
            }
         }
      }

      // Check clips
      if (pwData.HasClips() && ps.GetBoolValue("bShowClipsOnSlides")) {
         Dimension maxClipSize = SelectPresentationWizardPanel.getMaximumVideoSize(pwData, true);

         // Which clip size option is selected?
         if (ps.GetIntValue("iClipSizeType") == PublisherWizardData.CLIPSIZE_STANDARD) {
            if (maxClipSize.width * maxClipSize.height > nMaxPixelSize)
               bExceedsLimit = true;
            if ( (maxClipSize.width > nMaxVideoOrClipWidth)
                 && (maxClipSize.height > nMaxVideoOrClipHeight) )
               bCriticalSizeReached = true;
         } else if (ps.GetIntValue("iClipSizeType") == PublisherWizardData.CLIPSIZE_MAXSLIDE) {
             if (maxClipSize.width * maxClipSize.height > nMaxPixelSize)
               bExceedsLimit = true;
             if ( (maxClipSize.width > nMaxVideoOrClipWidth)
                  && (maxClipSize.height > nMaxVideoOrClipHeight) )
               bCriticalSizeReached = true;
         } else {
            // PublisherWizardData.CLIPSIZE_CUSTOM
            if (ps.GetIntValue("iMaxCustomClipWidth") * ps.GetIntValue("iMaxCustomClipHeight") > nMaxPixelSize) {
                if (maxClipSize.width * maxClipSize.height > nMaxPixelSize)
                    bExceedsLimit = true;
            } else if ( (ps.GetIntValue("iMaxCustomClipWidth") > nMaxVideoOrClipWidth) 
                        && (ps.GetIntValue("iMaxCustomClipHeight") > nMaxVideoOrClipHeight) ) {
                if ( (maxClipSize.width > nMaxVideoOrClipWidth)
                     && (maxClipSize.height > nMaxVideoOrClipHeight) )
               bCriticalSizeReached = true;
            }
         }

         if (bExceedsLimit)
         {
            String strMsg = CLIP_SIZE_TOO_BIG;
            if (!bIsInternal)
               strMsg += CHANGE_HINT;
            JOptionPane.showMessageDialog(myPanel, strMsg + "\n",
                                          ERROR, JOptionPane.ERROR_MESSAGE);
            hr = -1;
            return hr;
         }

         // Critical size: only remarkable for RealMedia export  
         boolean bIsRealMediaExport = (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_REAL);
         if (bCriticalSizeReached && bShowWarning && bIsRealMediaExport)
         {
            if (bIsInternal) {
               JOptionPane.showMessageDialog(myPanel, CLIP_SIZE_CRITICAL + "\n",
                                             WARNING, JOptionPane.WARNING_MESSAGE);
            } else {
               String[] opt = {CHANGE, CONTINUE};
               int jres = JOptionPane.showOptionDialog(myPanel, CLIP_SIZE_CRITICAL + CHANGE_HINT + "\n", 
                                                       WARNING, JOptionPane.DEFAULT_OPTION, 
                                                       JOptionPane.WARNING_MESSAGE, 
                                                       null, opt, opt[0]);
               if (0 == jres) {
                  // CHANGE was selected
                  hr = -1;
                  return hr;
               }
            }
         }
      }

      return hr;
   }

   private void doCancel()
   {
//      actionTaken_ = ACTION_CANCEL;
      dispose();
   }

   public void dispose()
   {
      if (parentPanel_ != null)
      {
         parentPanel_.setEnableButton(enabledParentButtons_, true);
         parentPanel_.refreshDataDisplay();
      }
      super.dispose();
   }

   private class ButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         String cmd = e.getActionCommand();
         if (cmd.equals(WizardPanel.OK))
         {
		      if (panelFlashSize_ != null)
		      {
		         Dimension dim = panelFlashSize_.getCustomSize();
   		      if (dim.width < 1 || dim.height < 1)
      		   {
         		   JOptionPane.showMessageDialog(panelFlashSize_, SPECIFY_SIZE,
            		                              ERROR, JOptionPane.ERROR_MESSAGE);
            		return;
         		}
            }
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

   private class MyChangeListener implements ChangeListener
   {
      public void stateChanged(ChangeEvent e)
      {
         //System.out.println("Tabbed Pane: " + tabbedPane_.getSelectedIndex());

         // We have to update the slide dimensions in the panel "Video and Clips" 
         if ( (panelSlideSize_ != null) && (panelVideoSize_ != null) )
         {
            Dimension slideSize = panelSlideSize_.getActualSlideSize();
            panelVideoSize_.updateSlideSize(slideSize);
         }
         if ( (panelFlashSize_ != null) && (panelVideoSize_ != null) )
         {
            Dimension slideSize = panelFlashSize_.getActualSlideSize();
            panelVideoSize_.updateSlideSize(slideSize);
         }
      }
   }
}
