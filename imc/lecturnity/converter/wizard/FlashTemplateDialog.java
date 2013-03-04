package imc.lecturnity.converter.wizard;

import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.io.IOException;
import javax.swing.*;
import javax.swing.filechooser.FileFilter;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.converter.LPLibs;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.ui.DirectoryChooser;
import imc.lecturnity.util.ui.LayoutUtils;
import imc.lecturnity.util.wizards.WizardTextArea;


/**
 * 
 */
///public class FlashTemplateDialog extends FlashExtendedDialog implements ActionListener
public class FlashTemplateDialog extends JDialog implements ActionListener
{
   private File m_LastDirectory;
   
   private JButton      m_btnBasicColor;
   private JButton      m_btnBackColor;
   private JCheckBox    m_boxDisplayTitle;
   private JButton      m_btnTextColor;
   private JCheckBox    m_boxDisplayLogo;
   private JLabel       m_lblFileName;
   private JLabel       m_lblLogoUrl;
   private JTextField   m_txtFileName;
   private JTextField   m_txtLogoUrl;
   private JButton      m_btnBrowse;
   private JCheckBox    m_boxShowWebPlayerLogo;
   private JCheckBox    m_boxUseOriginalslideSize;
   private JCheckBox    m_boxStartInOptimizedSize;

   private PublisherWizardData m_pwData;
   private boolean m_bIsNewFlashPlayer;

   private static Localizer m_Localizer;
   
 
   public FlashTemplateDialog(PublisherWizardData pwData, Localizer localizer)
   {
      m_pwData = pwData;
      m_Localizer = localizer;
      m_bIsNewFlashPlayer = 
         (m_pwData.GetProfiledSettings().GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_FLEX);
   }
   
   public void actionPerformed(ActionEvent evt)
   {
///      super.actionPerformed(evt); // handle ok and cancel

      String sPresentationFileName = m_pwData.GetDocumentData().GetPresentationFileName();
      
      if (evt.getSource() == m_btnBasicColor || evt.getSource() == m_btnBackColor || evt.getSource() == m_btnTextColor)
      {
         JButton b = (JButton)evt.getSource();
         b.setForeground(new Color(LPLibs.flashShowColorDialog(b.getForeground().getRGB())));
      }
      else if (evt.getSource() == m_btnBrowse)
      {
         JFileChooser fileChooser = new JFileChooser();
         Dimension fcSize = fileChooser.getPreferredSize();
         fileChooser.setPreferredSize(new Dimension(fcSize.width + 100, fcSize.height + 50));

         if (m_LastDirectory == null)
         {
            File tmp = new File(sPresentationFileName);
            File parentDir = tmp.getParentFile();
            if (parentDir != null)
               fileChooser.setCurrentDirectory(parentDir);
         }
         else
         {
            fileChooser.setCurrentDirectory(m_LastDirectory);
         }

         FileFilter ff =  new StillImageFilter();

         fileChooser.addChoosableFileFilter(ff);
         fileChooser.addChoosableFileFilter(fileChooser.getAcceptAllFileFilter());
         fileChooser.setFileFilter(ff);
         
         fileChooser.setDialogTitle(m_Localizer.getLocalized("CHOOSER_TITLE"));

         int action = fileChooser.showOpenDialog(this);

         m_LastDirectory = fileChooser.getCurrentDirectory();

         if (action == fileChooser.APPROVE_OPTION)
         {
            File selectedFile = fileChooser.getSelectedFile();
            m_txtFileName.setText(selectedFile.getAbsolutePath());
         }
      }
      else if (evt.getSource() == m_boxDisplayTitle)
      {
         m_btnTextColor.setEnabled(m_boxDisplayTitle.isSelected());
      }
      else if (evt.getSource() == m_boxDisplayLogo)
      {
         m_lblFileName.setEnabled(m_boxDisplayLogo.isSelected());
         m_txtFileName.setEnabled(m_boxDisplayLogo.isSelected());
         m_btnBrowse.setEnabled(m_boxDisplayLogo.isSelected());
         m_lblLogoUrl.setEnabled(m_boxDisplayLogo.isSelected());
         m_txtLogoUrl.setEnabled(m_boxDisplayLogo.isSelected());
      }
            
   }
   
   /**
    * Does not change members of m_pwData if there is any error.
    */
   protected boolean executeOk()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();

      boolean bInsetsChanged = false; // did settings affecting the insets get changed?
      bInsetsChanged = ps.GetBoolValue("bFlashDisplayLogo") != m_boxDisplayLogo.isSelected();
      bInsetsChanged = bInsetsChanged || ps.GetStringValue("strLogoImageName").length() > 0 
         && !m_txtFileName.getText().equals(ps.GetStringValue("strLogoImageName"));
      bInsetsChanged = bInsetsChanged || ps.GetBoolValue("bFlashDisplayTitle") != m_boxDisplayTitle.isSelected();
      bInsetsChanged = bInsetsChanged || ps.GetBoolValue("bHideLecturnityIdentity") != m_boxShowWebPlayerLogo.isSelected();

      if (m_boxDisplayLogo.isSelected())
      {
         if (m_txtFileName.getText().length() > 0)
         {
            File f = new File(m_txtFileName.getText());
            if (!(f.exists() && f.isFile()))
            {
               JOptionPane.showMessageDialog(this, 
                                             m_Localizer.getLocalized("IMAGE_NOT_FOUND"), 
                                             m_Localizer.getLocalized("ERROR"),
                                             JOptionPane.ERROR_MESSAGE);
         
               return false;
            }
            
            ps.SetStringValue("strLogoImageName", m_txtFileName.getText());
         }
         else
         {
            JOptionPane.showMessageDialog(this, 
                                          m_Localizer.getLocalized("IMAGE_NOT_SPECIFIED"), 
                                          m_Localizer.getLocalized("ERROR"),
                                          JOptionPane.ERROR_MESSAGE);
         
            return false;
         }

         if (m_txtLogoUrl.getText().length() > 0)
            ps.SetStringValue("strLogoImageUrl", castSpecialCharactersInUrl(m_txtLogoUrl.getText()));
         else
            ps.SetStringValue("strLogoImageUrl", "");
         
         ps.SetBoolValue("bFlashDisplayLogo", true);
      }
      else
      {
         ps.SetBoolValue("bFlashDisplayLogo", false);
      }

      ps.SetIntValue("iFlashColorBasic", m_btnBasicColor.getForeground().getRGB());
      ps.SetIntValue("iFlashColorBackground", m_btnBackColor.getForeground().getRGB());

      if (m_boxDisplayTitle.isSelected())
      {
         ps.SetBoolValue("bFlashDisplayTitle", true);
         ps.SetIntValue("iFlashColorText", m_btnTextColor.getForeground().getRGB());
      }
      else
      {
         ps.SetBoolValue("bFlashDisplayTitle", false);
      }
      
      // Bugfix 5326
      ps.SetBoolValue("bHideLecturnityIdentity", m_boxShowWebPlayerLogo.isSelected()); 
      
      // Bugfix 5603
      if (m_boxUseOriginalslideSize.isSelected())
          ps.SetIntValue("iSlideSizeType", PublisherWizardData.SLIDESIZE_ORIGINAL);
      else
          ps.SetIntValue("iSlideSizeType", PublisherWizardData.SLIDESIZE_CUSTOM);

      ps.SetBoolValue("bFlashStartInOptimizedSize", m_boxStartInOptimizedSize.isSelected());


      // Adapt movie size, if something has changed (only "normal" Flash Template, not for new Flex FlashPlayer)
      if (bInsetsChanged && ps.GetIntValue("iFlashTemplateType") == PublisherWizardData.TEMPLATE_FLASH_NORMAL)
      {
         Dimension dimMovie = new Dimension(ps.GetIntValue("iFlashMovieWidth"), 
                                            ps.GetIntValue("iFlashMovieHeight"));

         if (ps.GetIntValue("iFlashMovieSizeType") == PublisherWizardData.MOVIESIZE_CUSTOM)
         {
            // Ignore the case that the document contains video and pages 
            if ( (m_pwData.HasNormalVideo() && !m_pwData.IsDenverDocument()) 
               || (m_pwData.HasClips() && !ps.GetBoolValue("bShowClipsOnSlides") && !m_pwData.IsDenverDocument()) )
               return true;
   
            // as the aspect ratio changed we need to adapt the user given movie resolution
            JOptionPane.showMessageDialog(this, 
                                          m_Localizer.getLocalized("NOTE_MOVIE_SIZE"), 
                                          m_Localizer.getLocalized("WARNING"),
                                          JOptionPane.WARNING_MESSAGE);
   
            // Adapt movie size
            dimMovie = m_pwData.FlashGetWbSizeWithBorders();
            ps.SetIntValue("iFlashMovieWidth", (int)(0.5f + (dimMovie.width * m_pwData.m_dWhiteboardScale)));
            ps.SetIntValue("iFlashMovieHeight", (int)(0.5f + (dimMovie.height * m_pwData.m_dWhiteboardScale)));
         }
         else if (ps.GetIntValue("iFlashMovieSizeType") == PublisherWizardData.MOVIESIZE_SCREEN
            && bInsetsChanged)
         {
            // The next line is only called te recalculate Flash movie insets
            Dimension dimGreater = m_pwData.FlashGetWbSizeWithBorders();
         }
         // Recalculate Page size
         Dimension dimWhiteboard = new Dimension(-1, -1);
         dimWhiteboard = m_pwData.FlashCalculatePageSizeFromMovieSize(dimMovie);
         ps.SetIntValue("iSlideWidth", dimWhiteboard.width);
         ps.SetIntValue("iSlideHeight", dimWhiteboard.height);
         System.out.println("+ dimWB: " + dimWhiteboard.width + "x" + dimWhiteboard.height + " (Movie: " + dimMovie.width + "x" + dimMovie.height + ")");
      }

      return true;
   }
   
   protected String getTabTitle()
   {
      return m_Localizer.getLocalized("TEMPLATE");
   }

   protected JPanel createContentPanel()
   {
      ProfiledSettings ps = m_pwData.GetProfiledSettings();
      
      JPanel pnlInner = new JPanel();
      pnlInner.setLayout(null);
      
    
      int x = 20;
      int y = 20;
      int w = 440;
      int h = 20;
      
      
      JLabel lblBasicColor = new JLabel(m_Localizer.getLocalized("BASIC_COLOR")+": ");
      lblBasicColor.setLocation(x, y);
      lblBasicColor.setSize(120, h);
      lblBasicColor.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(lblBasicColor);

      m_btnBasicColor = new ColorButton();
      m_btnBasicColor.setLocation(x+130, y-5);
      m_btnBasicColor.setSize(50, h+10);
      m_btnBasicColor.setForeground(new Color(ps.GetIntValue("iFlashColorBasic")));
      //m_btnBasicColor.setContentAreaFilled(false);
      m_btnBasicColor.addActionListener(this);
      m_btnBasicColor.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(m_btnBasicColor);
      
      y += 35;
      
      JLabel lblBackColor = new JLabel(m_Localizer.getLocalized("BACKGROUND_COLOR")+": ");
      lblBackColor.setLocation(x, y);
      lblBackColor.setSize(120, h);
      lblBackColor.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(lblBackColor);

      m_btnBackColor = new ColorButton();
      m_btnBackColor.setLocation(x+130, y-5);
      m_btnBackColor.setSize(50, h+10);
         m_btnBackColor.setForeground(new Color(ps.GetIntValue("iFlashColorBackground")));
      //m_btnBackColor.setContentAreaFilled(false);
      m_btnBackColor.addActionListener(this);
      m_btnBackColor.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(m_btnBackColor);
      
      y += 35;
      
      m_boxDisplayTitle = new JCheckBox(m_Localizer.getLocalized("DISPLAY_AUTHOR"));
      m_boxDisplayTitle.setLocation(x, y);
      m_boxDisplayTitle.setSize(w, h);
      m_boxDisplayTitle.setSelected(ps.GetBoolValue("bFlashDisplayTitle"));
      m_boxDisplayTitle.addActionListener(this);
      m_boxDisplayTitle.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(m_boxDisplayTitle);
    
      y += 30;
      
      JLabel lblTextColor = new JLabel(m_Localizer.getLocalized("TEXT_COLOR")+": ");
      lblTextColor.setLocation(x+20, y);
      lblTextColor.setSize(100, h);
      lblTextColor.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(lblTextColor);

      m_btnTextColor = new ColorButton();
      m_btnTextColor.setLocation(x+110+20, y-5);
      m_btnTextColor.setSize(50, h+10);
      m_btnTextColor.setForeground(new Color(ps.GetIntValue("iFlashColorText")));
      m_btnTextColor.setEnabled(ps.GetBoolValue("bFlashDisplayTitle"));
      //m_btnTextColor.setContentAreaFilled(false);
      m_btnTextColor.addActionListener(this);
      m_btnTextColor.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(m_btnTextColor);
      
      y += 35;
      
      m_boxDisplayLogo = new JCheckBox(m_Localizer.getLocalized("DISPLAY_LOGO"));
      m_boxDisplayLogo.setLocation(x, y);
      m_boxDisplayLogo.setSize(w, h);
      m_boxDisplayLogo.addActionListener(this);
      m_boxDisplayLogo.setSelected(ps.GetBoolValue("bFlashDisplayLogo"));
      m_boxDisplayLogo.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(m_boxDisplayLogo);
       
      y += 25;
      
      m_lblFileName = new JLabel(m_Localizer.getLocalized("FILE_NAME")+": ");
      m_lblFileName.setLocation(x+20, y);
      m_lblFileName.setSize(120, h);
      m_lblFileName.setEnabled(ps.GetBoolValue("bFlashDisplayLogo"));
      m_lblFileName.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(m_lblFileName);
      
      y += 25;
      
      m_txtFileName = new JTextField();
      m_txtFileName.setLocation(x+20, y);
      m_txtFileName.setSize(w-20-120-10-25, h);
      m_txtFileName.setEnabled(ps.GetBoolValue("bFlashDisplayLogo"));
      m_txtFileName.setText(ps.GetStringValue("strLogoImageName"));
      m_txtFileName.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(m_txtFileName);

      m_btnBrowse = new JButton(m_Localizer.getLocalized("SEARCH")+"...");
      m_btnBrowse.setLocation(x+(w-120)+10-30, y-1);
      m_btnBrowse.setSize(120, h+2);
      m_btnBrowse.addActionListener(this);
      m_btnBrowse.setEnabled(ps.GetBoolValue("bFlashDisplayLogo"));
      m_btnBrowse.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(m_btnBrowse);
      
      y += 30;
      
      m_lblLogoUrl = new JLabel(m_Localizer.getLocalized("LOGO_URL")+": ");
      m_lblLogoUrl.setLocation(x+20, y);
      m_lblLogoUrl.setSize(w-20-120, h);
      m_lblLogoUrl.setEnabled(ps.GetBoolValue("bFlashDisplayLogo"));
      m_lblLogoUrl.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(m_lblLogoUrl);
      
      y += 25;
      
      m_txtLogoUrl = new JTextField();
      m_txtLogoUrl.setLocation(x+20, y);
      m_txtLogoUrl.setSize(w-20-120-10-25, h);
      m_txtLogoUrl.setEnabled(ps.GetBoolValue("bFlashDisplayLogo"));
      m_txtLogoUrl.setText(ps.GetStringValue("strLogoImageUrl"));
      m_txtLogoUrl.setVisible(!m_bIsNewFlashPlayer);
      pnlInner.add(m_txtLogoUrl);
      
      y +=30; 
      if (m_bIsNewFlashPlayer)
         y = 30;
      
      m_boxShowWebPlayerLogo = new JCheckBox(m_Localizer.getLocalized("HIDE_WEBPLAYER_LOGO"));
      m_boxShowWebPlayerLogo.setLocation(x, y);
      m_boxShowWebPlayerLogo.setSize(w, h);
      m_boxShowWebPlayerLogo.addActionListener(this);
      m_boxShowWebPlayerLogo.setSelected(ps.GetBoolValue("bHideLecturnityIdentity"));
      m_boxShowWebPlayerLogo.setVisible(true);
      pnlInner.add(m_boxShowWebPlayerLogo);

      y += 25;

      m_boxUseOriginalslideSize = new JCheckBox(m_Localizer.getLocalized("DEFAULT_CONTENT_SIZE"));
      m_boxUseOriginalslideSize.setLocation(x, y);
      m_boxUseOriginalslideSize.setSize(w, h);
      m_boxUseOriginalslideSize.addActionListener(this);
      m_boxUseOriginalslideSize.setSelected(ps.GetIntValue("iSlideSizeType") == PublisherWizardData.SLIDESIZE_ORIGINAL);
      m_boxUseOriginalslideSize.setVisible(true);
      pnlInner.add(m_boxUseOriginalslideSize);
      if (!m_bIsNewFlashPlayer)
          m_boxUseOriginalslideSize.setVisible(false);
          
      y += 25;

      m_boxStartInOptimizedSize = new JCheckBox(m_Localizer.getLocalized("OPEN_IN_OPTIMIZED_SIZE"));
      m_boxStartInOptimizedSize.setLocation(x, y);
      m_boxStartInOptimizedSize.setSize(w, h);
      m_boxStartInOptimizedSize.addActionListener(this);
      m_boxStartInOptimizedSize.setSelected(ps.GetBoolValue("bFlashStartInOptimizedSize"));
      m_boxStartInOptimizedSize.setVisible(true);
      pnlInner.add(m_boxStartInOptimizedSize);
      if (!m_bIsNewFlashPlayer)
          m_boxStartInOptimizedSize.setVisible(false);

      return pnlInner;
   }

   protected String castSpecialCharactersInUrl(String strUrl)
   {
      // ActionScript Compiler fails with Strings containing uncasted '\' and '"'
      
      StringBuffer strBufUrl = new StringBuffer(strUrl);

      int nLen = strUrl.length();
      for (int i = nLen-1; i >= 0; --i)
      {
         // Look for uncasted Backslashes
         if (strBufUrl.substring(i, i+1).equals("\\"))
         {
            // Do we have even or odd number of '\'?
            int sumBackslashes = 1;
            
            if (i > 0)
            {
               for (int j = i-1; j >= 0; --j)
               {
                  i = j;
                  if (strBufUrl.substring(j, j+1).equals("\\"))
                  {
                     sumBackslashes++;
                  }
                  else
                  {
                     i++; // move to the 1st '\'
                     break;
                  }
               }
            }
            
            if (sumBackslashes%2 != 0)
            {
               // Odd number of backslashes
               if ((i+sumBackslashes+1 < nLen) 
                  && (strBufUrl.substring(i+sumBackslashes, i+sumBackslashes+1).equals("\"")))
               {
                  // Don't cast the '\' of an already casted quotation mark
                  // do nothing here
               }
               else
               {
                  strBufUrl.insert(i, "\\");
               }
            }
            else
            {
               // Even number of backslashes
               if ((i+sumBackslashes+1 < nLen) 
                  && (strBufUrl.substring(i+sumBackslashes, i+sumBackslashes+1).equals("\"")))
               {
                  // We have to cast the quotation mark
                  strBufUrl.insert(i, "\\");
               }
               else
               {
                  // do nothing here
               }
            }
         }

         // Look for uncasted quotation marks
         if (strBufUrl.substring(i, i+1).equals("\""))
         {
            boolean bIsAlreadyCasted 
               = ((i > 0) && (strBufUrl.substring(i-1, i).equals("\\"))) ? true : false;
         
            if (!bIsAlreadyCasted)
            {
               strBufUrl.insert(i, "\\");
            }
         }
      }
      
      return strBufUrl.toString();
   }
   
   static class ColorButton extends JButton
   {
      protected void paintComponent(Graphics g)
      {
         super.paintComponent(g);
            
         Insets ins = getInsets();
         if (isEnabled())
            g.setColor(getForeground());
         else
            g.setColor(Color.gray);
         g.fillRect(6,6,getWidth()-13, getHeight()-13);
      }
      
   }
   
   static class StillImageFilter extends FileFilter
   {
      public boolean accept(File f)
      {
         if (f.isDirectory())
            return true;
         String fileName = f.getName().toUpperCase();
         return (fileName.endsWith(".JPG") ||
                 fileName.endsWith(".JPEG") ||
                 fileName.endsWith(".GIF") || 
                 fileName.endsWith(".PNG")
         );
      }
      
      public String getDescription()
      {
         return m_Localizer.getLocalized("FILTER_DESCRIPTION");
      }
   }
}