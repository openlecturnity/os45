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
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.filechooser.FileFilter;

import javax.swing.JOptionPane;

import java.io.File;

import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.wizards.ButtonPanel;
import imc.lecturnity.util.wizards.WizardButton;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.WizardData;
import imc.lecturnity.util.wizards.WizardTextArea;

import imc.lecturnity.util.ImagePanel;
import imc.lecturnity.util.ui.PopupHelp;
import imc.lecturnity.util.ui.LazyTextArea;
import imc.lecturnity.util.ui.LayoutUtils;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.converter.StreamingMediaConverter;

class FlashSizePanel extends WizardPanel implements ChangeListener
{
   private static String MOVIE_FLEXIBLE = "[!]";
   private static String MOVIE_OPTIMIZE_SCREEN = "[!]";
   private static String MOVIE_OPTIMIZE_SIZE = "[!]";
   private static String MOVIE_OPTIMIZE = "[!]";
   private static String SCREEN = "[!]";
   private static String ACCORDING_PAGE_SIZE = "[!]";
   private static String NOTE_OTHER_VALUE = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/FlashSizePanel_",
             "en");
         MOVIE_FLEXIBLE = l.getLocalized("MOVIE_FLEXIBLE");
         MOVIE_OPTIMIZE_SCREEN = l.getLocalized("MOVIE_OPTIMIZE_SCREEN");
         MOVIE_OPTIMIZE_SIZE = l.getLocalized("MOVIE_OPTIMIZE_SIZE");
         MOVIE_OPTIMIZE = l.getLocalized("MOVIE_FLEXIBLE");
         SCREEN = l.getLocalized("SCREEN");
         ACCORDING_PAGE_SIZE = l.getLocalized("ACCORDING_PAGE_SIZE");
         NOTE_OTHER_VALUE = l.getLocalized("NOTE_OTHER_VALUE");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open FlashSizePanel locale database!\n"+
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private ExtendedSettingsDialog parent_ = null;
   private PublisherWizardData pwData_;

   private boolean bSeparateClipsFromVideo_;

   private JRadioButton m_btnFlexible;
   private JRadioButton m_btnOptimizedRes;
   private JRadioButton m_btnOptimizedCust;
   private CustomSizePanel m_pnlCustomSize;
   private JLabel m_lblScreen;
   private JComboBox m_boxResolutions;
   private JPanel m_pnlPageSizeArea;
   private JPanel m_pnlWarningArea;
   private WizardTextArea m_txtPageSize;
   

   public FlashSizePanel(ExtendedSettingsDialog parent, PublisherWizardData pwData)
   {
      super();
      
      parent_ = parent;
      pwData_ = pwData;

      bSeparateClipsFromVideo_ = pwData_.GetProfiledSettings().GetBoolValue("bShowClipsOnSlides");

      initGui();
   }

   public PublisherWizardData getWizardData() 
   {
      DocumentData dd = pwData_.GetDocumentData();
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      if (!m_btnFlexible.isSelected())
      {
         if (m_btnOptimizedRes.isSelected()) // "Screen Resolution"
         {
            String strSize = (String)m_boxResolutions.getSelectedItem();
            int iWidth = Integer.parseInt(strSize.substring(0, strSize.indexOf("x")));
            int iHeight = Integer.parseInt(strSize.substring(strSize.indexOf("x")+1));
                  
            // subtract size of ui elements of IE from available screen size
            if (iWidth <= 1024)
               iHeight -= 180;
            else
               iHeight -= 240; // larger resolutions may have larger font sizes
            iWidth -= 30;

            ps.SetIntValue("iFlashMovieWidth", iWidth);
            ps.SetIntValue("iFlashMovieHeight", iHeight);
            
            Dimension dim = new Dimension(iWidth, iHeight);
            Dimension dimSmaller = new Dimension(-1, -1);
            dimSmaller = pwData_.FlashCalculatePageSizeFromMovieSize(dim);
            ps.SetIntValue("iSlideWidth", dimSmaller.width);
            ps.SetIntValue("iSlideHeight", dimSmaller.height);

            ps.SetIntValue("iFlashMovieSizeType", PublisherWizardData.MOVIESIZE_SCREEN);
            ps.SetIntValue("iRmWmScreenResolutionIndex", m_boxResolutions.getSelectedIndex());
         }
         else // "User defined"
         {
            Dimension dim = m_pnlCustomSize.getActualSize();

            // "Denver" mode?
            if (pwData_.IsDenverDocument())
            {
               // Recalculate dim (= movie size) - exception: Sony PSP design
               if (ps.GetIntValue("iFlashTemplateType") != PublisherWizardData.TEMPLATE_FLASH_PSP)
                  dim = pwData_.FlashGetWbSizeWithBorders();
            }
            // "Normal" mode
            else
            {
               Dimension dimSmaller = new Dimension(-1, -1);
               dimSmaller = pwData_.FlashCalculatePageSizeFromMovieSize(dim);
               double scaleX = 1.0; double scaleY = 1.0;
               scaleX = (float)dimSmaller.width / (float)dd.GetWhiteboardDimension().width;
               scaleY = (float)dimSmaller.height / (float)dd.GetWhiteboardDimension().height;
               pwData_.m_dWhiteboardScale = 0.5 * (scaleX + scaleY);
               ps.SetIntValue("iSlideWidth", dimSmaller.width);
               ps.SetIntValue("iSlideHeight", dimSmaller.height);
            }

            // If "separateClipsFromVideo" has changed then the movie size has to be recalculated (not for PSP)
            // Bugfix 4152: Also no recalculation if the movie contains a acccompanying video
            boolean bIsPsp = (ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_PSP);
            if (!bIsPsp && !dd.HasNormalVideo() && (bSeparateClipsFromVideo_ != ps.GetBoolValue("bShowClipsOnSlides")))
            {
               dim = pwData_.FlashGetWbSizeWithBorders();
               dim.width = (int)(dim.width * pwData_.m_dWhiteboardScale + 0.5);
               dim.height = (int)(dim.height * pwData_.m_dWhiteboardScale + 0.5);
            }

            ps.SetIntValue("iFlashMovieWidth", dim.width);
            ps.SetIntValue("iFlashMovieHeight", dim.height);
            ps.SetIntValue("iFlashMovieSizeType", PublisherWizardData.MOVIESIZE_CUSTOM);
            pwData_.m_PreferredMovieDimension = dim;
         }
      }
      else // "Flexible"
      {
         Dimension dim = pwData_.FlashGetWbSizeWithBorders();
         ps.SetIntValue("iFlashMovieWidth", dim.width);
         ps.SetIntValue("iFlashMovieHeight", dim.height);

         ps.SetIntValue("iFlashMovieSizeType", PublisherWizardData.MOVIESIZE_FLEXIBLE);
      }

      return pwData_;
   }

   public void refreshGui()
   {
      updateGui();
   }

   public String getDescription()
   {
      return "";
   }

   public Dimension getCustomSize()
   {
   	return m_pnlCustomSize.getActualSize();
   }

   private void initGui()
   {
      //Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      String[] arrResolutions = new String[] { "640x480", "800x600", "1024x768", 
         "1152x864", "1280x960", "1280x1024", "1400x1050", "1600x1200" };

      ProfiledSettings ps = pwData_.GetProfiledSettings();

      boolean bOptimized = (ps.GetIntValue("iFlashMovieSizeType") != PublisherWizardData.MOVIESIZE_FLEXIBLE);
      boolean bResolutionMatch = (ps.GetIntValue("iFlashMovieSizeType") == PublisherWizardData.MOVIESIZE_SCREEN);
      int iSelectedIndex = ps.GetIntValue("iRmWmScreenResolutionIndex");
      boolean bMovieSizeDefined = (ps.GetIntValue("iFlashMovieWidth") > 0 && ps.GetIntValue("iFlashMovieHeight") > 0);
      int iMovieWidth = ps.GetIntValue("iFlashMovieWidth");
      int iMovieHeight = ps.GetIntValue("iFlashMovieHeight");
      boolean bPreferredMovieSizeDefined = (pwData_.m_PreferredMovieDimension != null);
      int iPreferredMovieWidth = bPreferredMovieSizeDefined ? pwData_.m_PreferredMovieDimension.width : -1;
      int iPreferredMovieHeight = bPreferredMovieSizeDefined ? pwData_.m_PreferredMovieDimension.height : -1;
      
      if (bResolutionMatch && bMovieSizeDefined)
      {
         String strFind = iMovieWidth + "x" + iMovieHeight;
         
         for (int i = 0; i < arrResolutions.length; ++i)
         {
            if (arrResolutions[i].equals(strFind))
            {
               iSelectedIndex = i;
               break;
            }
         }
      }
      
      this.setLayout(new BorderLayout());
      JPanel pnlInner = new JPanel();
      pnlInner.setLayout(null);
      this.add(pnlInner, BorderLayout.CENTER);

      ButtonGroup bg = new ButtonGroup();

      RadioListener rl = new RadioListener();

      int x = 20;
      int y = 20;
      int w = 440;
      int h = 20;
      
      
      m_btnFlexible = new JRadioButton(MOVIE_FLEXIBLE);
      m_btnFlexible.setFont(headerFont);
      m_btnFlexible.setSize(w, h);
      m_btnFlexible.setLocation(x, y);
      m_btnFlexible.addActionListener(rl);
      pnlInner.add(m_btnFlexible);
      bg.add(m_btnFlexible);

      y += 25;
      
      m_btnOptimizedRes = new JRadioButton(MOVIE_OPTIMIZE_SCREEN);
      m_btnOptimizedRes.setFont(headerFont);
      m_btnOptimizedRes.setSize(w, h);
      m_btnOptimizedRes.setLocation(x, y);
      m_btnOptimizedRes.addActionListener(rl);
      pnlInner.add(m_btnOptimizedRes);
      bg.add(m_btnOptimizedRes);
      
      y += 25;
      
      m_lblScreen = new JLabel(SCREEN);
      m_lblScreen.setFont(descFont);
      m_lblScreen.setSize(80, h);
      m_lblScreen.setLocation(x+22, y);
      pnlInner.add(m_lblScreen);
      
     
      m_boxResolutions = new JComboBox(arrResolutions);
      m_boxResolutions.setFont(descFont);
      m_boxResolutions.setSize(100, h);
      m_boxResolutions.setLocation(x+80+10, y);
      m_boxResolutions.setSelectedIndex(iSelectedIndex);
      pnlInner.add(m_boxResolutions);
      
      y += 25;

      m_btnOptimizedCust = new JRadioButton(MOVIE_OPTIMIZE_SIZE);
      m_btnOptimizedCust.setFont(descFont);
      m_btnOptimizedCust.setSize(w, h);
      m_btnOptimizedCust.setLocation(x, y);
      m_btnOptimizedCust.addActionListener(rl);
      pnlInner.add(m_btnOptimizedCust);
      bg.add(m_btnOptimizedCust);
      
      y += 30;
      
      // use the whiteboard size plus the Flash borders for the aspect ratio:
      Dimension dimGreater = new Dimension(-1, -1);
      dimGreater = pwData_.FlashGetWbSizeWithBorders();
     
      int displayW = dimGreater.width;
      int displayH = dimGreater.height;
      if (bOptimized && bMovieSizeDefined)
      {
         if (bPreferredMovieSizeDefined)
         {
            displayW = iPreferredMovieWidth;
            displayH = iPreferredMovieHeight;
         }
         else
         {
            displayW = iMovieWidth;
            displayH = iMovieHeight;
         }
         dimGreater.width = displayW;
         dimGreater.height = displayH;
      }
      
      // first two parameters are the size of the panel
      // the third parameter is/are the values used for the aspect ratio
      m_pnlCustomSize = new CustomSizePanel(320, h, new Dimension(dimGreater.width, dimGreater.height), '@', true);
      m_pnlCustomSize.setFont(descFont);
      m_pnlCustomSize.setLocation(x+15, y);
      m_pnlCustomSize.setActualSize(displayW, displayH); // these are the displayed values
      m_pnlCustomSize.configureAspectRatioBoxVisibility(true, true);
      m_pnlCustomSize.addChangeListener(this);
      pnlInner.add(m_pnlCustomSize);
      
      y += 30;

      m_pnlPageSizeArea = new JPanel();
      m_pnlPageSizeArea.setLayout(null);
      m_pnlPageSizeArea.setFont(descFont);
      m_pnlPageSizeArea.setLocation(x, y);
      m_pnlPageSizeArea.setSize(w, h);
 

      m_txtPageSize = new WizardTextArea("",
         new Point(15, 0), new Dimension(w, 35),
         imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      m_pnlPageSizeArea.add(m_txtPageSize);
      
      pnlInner.add(m_pnlPageSizeArea);

      y += 30;

      m_pnlWarningArea = new JPanel();
      m_pnlWarningArea.setLayout(null);
      m_pnlWarningArea.setFont(descFont);
      m_pnlWarningArea.setLocation(x, y);
      m_pnlWarningArea.setSize(w, 50);
      
     
      JPanel pnlWarningIcon = new ImagePanel("/imc/lecturnity/converter/images/information.gif");
      pnlWarningIcon.setSize(32, 32);
      pnlWarningIcon.setLocation(0, 0);
      m_pnlWarningArea.add(pnlWarningIcon);
      
      
      WizardTextArea txtBorder = new WizardTextArea(NOTE_OTHER_VALUE,
         new Point(35+10, 0), new Dimension(w-75, 50),
         imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      m_pnlWarningArea.add(txtBorder);
      
      pnlInner.add(m_pnlWarningArea);
      
      y += 65;
   
      
      m_btnFlexible.setSelected(!bOptimized);
      m_btnOptimizedRes.setSelected(bOptimized && bResolutionMatch);
      m_btnOptimizedCust.setSelected(bOptimized && !bResolutionMatch);
      
      if (pwData_.m_bIsProfileMode)
      {
         if (ps.GetIntValue("iFlashMovieSizeType") != PublisherWizardData.MOVIESIZE_CUSTOM)
            m_pnlCustomSize.setActualSize(ps.GetIntValue("iFlashMovieWidth"), 
                                          ps.GetIntValue("iFlashMovieHeight"));
      }

      updateGui();
   }
   
   private void updateGui()
   {
      boolean bIsPspDesign = (pwData_.GetProfiledSettings().GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_PSP);
      
      if (bIsPspDesign)
      {
         m_btnFlexible.setEnabled(false);
         m_btnOptimizedRes.setEnabled(false);
         m_btnOptimizedCust.setSelected(true);
      }

      m_lblScreen.setEnabled(m_btnOptimizedRes.isSelected());
      m_boxResolutions.setEnabled(m_btnOptimizedRes.isSelected());

      LayoutUtils.enableContainer(m_pnlCustomSize, m_btnOptimizedCust.isSelected());
      m_pnlCustomSize.setEnabled(m_btnOptimizedCust.isSelected());
      m_pnlWarningArea.setVisible(m_btnOptimizedCust.isSelected());
      if (m_btnOptimizedCust.isSelected())
         m_txtPageSize.enable();
      else
         m_txtPageSize.disable();
         
      Dimension dim = m_pnlCustomSize.getActualSize();
      Dimension dimSmaller = new Dimension(-1, -1);
      dimSmaller = pwData_.FlashCalculatePageSizeFromMovieSize(dim);
      m_txtPageSize.setText("(" + ACCORDING_PAGE_SIZE + " " + dimSmaller.width + "x" + dimSmaller.height + ")");
      m_txtPageSize.setVisible(true);
   }
   
   public Dimension getActualSlideSize()
   {
      Dimension dim = m_pnlCustomSize.getActualSize();
      Dimension actualSlideSize = new Dimension(-1, -1);
      actualSlideSize = pwData_.FlashCalculatePageSizeFromMovieSize(dim);

      return actualSlideSize;
   }
   
   public void stateChanged(ChangeEvent evt) 
   {
      if (evt.getSource() == m_pnlCustomSize)
      {
         updateGui();
      }
   }

   private class RadioListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         updateGui();
      }
   }
}
