package imc.lecturnity.converter.wizard;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Insets;
import java.io.IOException;
import javax.swing.ImageIcon;
import javax.swing.JOptionPane;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.converter.LPLibs;

public class FlashConvertWizardData extends StreamingConvertWizardData
{
   private static String PUBLISH_FLASH = "[!]";
   private static String SERVER_DIR = "[!]";
   private static String SERVER_URL = "[!]";

   private static Localizer localizer = null;
   static 
   {
      try
      {
         localizer = new Localizer(
            "/imc/lecturnity/converter/wizard/FlashWriterWizardPanel_", "en");

         PUBLISH_FLASH = localizer.getLocalized("PUBLISH_FLASH");
         SERVER_DIR = localizer.getLocalized("SERVER_DIR");
         SERVER_URL = localizer.getLocalized("SERVER_URL");
      }
      catch (IOException exc)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }
   
   
   public int       m_iAudioBitrate = 32;
   public int       m_iFlashVersion = 7;
   public boolean   m_bCreateWebsite = true;
	public boolean   m_bSlidestarExport = false;
	public boolean   m_bIsPspDesign = false;
   public boolean   m_bOptimizeForScreen = false;
   public boolean   m_bOptimizeForUserSize = true; //false;
   public boolean   m_bDisplayTitle = true;
   public boolean   m_bDisplayLogo = false;
   public Dimension m_OutputMovieSize = null; // either the target screen dimensions or the user given size or nothing
   public Dimension m_PreferredMovieSize = null;
   public double    m_dWbScale = 1.0d;
   public int       m_iScreenIndex = 2; // 1024x768
   public String    m_LogoImageName = null;
   public String    m_LogoImageUrl = null;
   public Color     m_ColorBackground = new Color(0x2D, 0x33, 0x51);
   public Color     m_ColorText = new Color(0xEA, 0xE7, 0xD7);
   public boolean   m_bOptimizeClipSync = false;
   public boolean   m_bDoNotCutVideos = false;

   private int      m_nLogoWidth;
   private int      m_nLogoHeight;
   private String   m_LogoNameKey = null;

   public void reset()
   {
     // m_iAudioBitrate = 32;
//       m_bEmbedFonts = true;
//       m_bCreateWebsite = true;
//      m_bFixedMovieSize = false;
     // m_OutputMovieSize = null;
      
      super.targetFileName = null;
   }

   public String getSummary()
   {
      String summary ="";

      summary = SOURCE + presentationFileName + '\n';
      summary += TARGET + targetDocName + "\n\n";
      
      switch (replayType)
      
      {
      case REPLAY_LOCAL:
         summary += PUBLISH + PUBLISH_LOCAL + '\n';
         summary += LOCAL_DIR + targetDirHttpServer + '\n';
         break;

      case REPLAY_HTTP:
         summary += PUBLISH + PUBLISH_HTTP + '\n';
         summary += HTTP_DIR + targetDirHttpServer + '\n';
         if ((targetUrlHttpServer != null) && (!targetUrlHttpServer.equals("")))
           summary += HTTP_URL + targetUrlHttpServer + '\n';
         break;

      case REPLAY_FLASHSRV:
         summary += PUBLISH + PUBLISH_FLASH + '\n';
         summary += HTTP_DIR + targetDirHttpServer + '\n';
         summary += SERVER_DIR + targetDirStreamingServer + '\n';
         summary += SERVER_URL + targetUrlStreamingServer + '\n';
         break;
      }

      return summary;
   }
   
   
   public void fillOtherWizardData(ConvertWizardData otherData)
   {
      if (otherData instanceof FlashConvertWizardData)
      {
         FlashConvertWizardData that = (FlashConvertWizardData)otherData;
         
         that.m_iAudioBitrate = m_iAudioBitrate;
         that.m_iFlashVersion = m_iFlashVersion;
         that.m_bCreateWebsite = m_bCreateWebsite;
         that.m_bSlidestarExport = m_bSlidestarExport;
         that.m_bIsPspDesign = m_bIsPspDesign;
         that.m_bOptimizeForScreen = m_bOptimizeForScreen;
         that.m_bOptimizeForUserSize = m_bOptimizeForUserSize;
         that.m_bDisplayTitle = m_bDisplayTitle;
         that.m_bDisplayLogo = m_bDisplayLogo;
         that.m_OutputMovieSize = (m_OutputMovieSize != null) ? new Dimension(m_OutputMovieSize.width, m_OutputMovieSize.height) : null;
         that.m_PreferredMovieSize = (m_PreferredMovieSize != null) ? new Dimension(m_PreferredMovieSize.width, m_PreferredMovieSize.height) : null;
         that.m_dWbScale = m_dWbScale;
         that.m_iScreenIndex = m_iScreenIndex;
         that.m_LogoImageName = (m_LogoImageName != null) ? new String(m_LogoImageName) : null;
         that.m_LogoImageUrl = (m_LogoImageUrl != null) ? new String(m_LogoImageUrl) : null;
         that.m_ColorBackground = new Color(m_ColorBackground.getRed(), m_ColorBackground.getGreen(), m_ColorBackground.getBlue());
         that.m_ColorText = new Color(m_ColorText.getRed(), m_ColorText.getGreen(), m_ColorText.getBlue());
         that.m_bOptimizeClipSync = m_bOptimizeClipSync;
         that.m_bDoNotCutVideos = m_bDoNotCutVideos;
      }

      super.fillOtherWizardData(otherData);
   }
   
   
   public Dimension calculatePageSizeFromMovieSize(Dimension sizeMovie)
   {
      return calculateContentSizeFromMovieSize(sizeMovie, false);
   }

   public Dimension calculateVideoSizeFromMovieSize(Dimension sizeMovie)
   {
      return calculateContentSizeFromMovieSize(sizeMovie, true);
   }

   private Dimension calculateContentSizeFromMovieSize(Dimension sizeMovie, boolean bContentIsVideo)
   {
      // content size is either page size or video size
      Dimension sizePagesOut = new Dimension(0, 0);
      Dimension sizeVideoOut = new Dimension(-1, -1);
      
      Dimension sizeVideoIn = getAccVideoOutputSize(); // Accompanying video
      if (isSgStandAloneMode)
      {
         sizeVideoIn = new Dimension(super.outputVideoWidth, super.outputVideoHeight);
         if (super.videoSizeType == super.VIDEOSIZE_CUSTOM)
            sizeVideoIn = new Dimension(super.customVideoWidth, super.customVideoHeight);
      }
      Dimension sizePagesIn = super.whiteboardSize;
      Dimension sizeLogo = getLogoSize();
      boolean bIsControlbarVisible = (super.iNavigationControlBar == 2) ? false : true;
      boolean bShowVideoOnPages = false; // just for compatibility (this class is not used any more in LEC 4)

      // Before calculating movie insets: Set PSP design flag
      LPLibs.flashSetPspDesign(m_bIsPspDesign);

      LPLibs.flashGetContentDimensions(sizeMovie, sizeVideoIn, sizePagesIn, sizeLogo, 
                                       m_bDisplayTitle, bIsControlbarVisible, bShowVideoOnPages, 
                                       sizeVideoOut, sizePagesOut);

      if (bContentIsVideo)
         return sizeVideoOut;
      else
         return sizePagesOut;
   }

   public Dimension getWbSizeWithBorders()
   {
      Dimension dimSmaller;
      Dimension dimGreater;
      Insets insets = new Insets(0,0,0,0);
      Dimension dimLogoSize = getLogoSize();
      Dimension dimVideoSize = getAccVideoOutputSize(); // Accompanying video
      if (super.containsMultipleVideos && !super.separateClipsFromVideo 
         && dimVideoSize.width <= 0 && dimVideoSize.height <= 0)
      {
         dimVideoSize = new Dimension(super.outputVideoWidth, super.outputVideoHeight);
      }
      boolean bIsControlbarVisible = (super.iNavigationControlBar == 2) ? false : true;

      if (isSgStandAloneMode) // Denver document
      {
         LPLibs.flashSetSgStandAloneMode(true);
         dimSmaller = new Dimension(0, 0);
         dimVideoSize = new Dimension(super.outputVideoWidth, super.outputVideoHeight);
         if (super.videoSizeType == super.VIDEOSIZE_CUSTOM)
            dimVideoSize = new Dimension(super.customVideoWidth, super.customVideoHeight);
      }
      else 
      {
         LPLibs.flashSetSgStandAloneMode(false);
         dimSmaller = super.whiteboardSize;
      }
      
      // Before calculating movie insets: Set PSP design flag
      LPLibs.flashSetPspDesign(m_bIsPspDesign);

///      int nResult = LPLibs.flashObtainMovieInsets(insets, super.whiteboardSize, m_bDisplayTitle, dimLogoSize, 
///                                                  dimVideoSize, bIsControlbarVisible);
      int nResult = LPLibs.flashObtainMovieInsets(insets, dimSmaller, m_bDisplayTitle, dimLogoSize, 
                                                  dimVideoSize, bIsControlbarVisible);
      // TODO: can this error value be ignored?
      
      
      if (isSgStandAloneMode) // Denver document
         dimSmaller = new Dimension(dimVideoSize);

      dimGreater = new Dimension(
         dimSmaller.width + insets.left + insets.right, dimSmaller.height + insets.top + insets.bottom);
      
      return dimGreater;
   }
   
   public Dimension subtractScaledFlashBorders(Dimension dimGreater)
   {
      Dimension dimContent;
      Dimension dimSmaller;
      Insets insets = new Insets(0,0,0,0);
      Dimension dimLogoSize = getLogoSize();
      Dimension dimVideoSize = getAccVideoOutputSize(); // Accompanying video
      boolean bIsControlbarVisible = (super.iNavigationControlBar == 2) ? false : true;

      if (isSgStandAloneMode) // Denver document
      {
         LPLibs.flashSetSgStandAloneMode(true);
         dimContent = new Dimension(0, 0);
         dimVideoSize = new Dimension(super.outputVideoWidth, super.outputVideoHeight);
         if (super.videoSizeType == super.VIDEOSIZE_CUSTOM)
            dimVideoSize = new Dimension(super.customVideoWidth, super.customVideoHeight);
      }
      else
      {
         LPLibs.flashSetSgStandAloneMode(false);
         dimContent = super.whiteboardSize;
      }

      // Before calculating movie insets: Set PSP design flag
      LPLibs.flashSetPspDesign(m_bIsPspDesign);

///      int nResult = LPLibs.flashObtainMovieInsets(insets, super.whiteboardSize, m_bDisplayTitle, dimLogoSize, 
///                                                  dimVideoSize, bIsControlbarVisible);
      int nResult = LPLibs.flashObtainMovieInsets(insets, dimContent, m_bDisplayTitle, dimLogoSize, 
                                                  dimVideoSize, bIsControlbarVisible);
      // TODO: can this error value be ignored?
      
      if (isSgStandAloneMode) // Denver document
      {
         dimSmaller = new Dimension((insets.left + insets.right), (insets.top + insets.bottom));
      }
      else
      {
         int nNormalWidth = dimContent.width + insets.left + insets.right;
         float fEnlargementFactor = dimGreater.width/(float)nNormalWidth;
         
         dimSmaller = new Dimension(
            dimGreater.width - Math.round(fEnlargementFactor * (insets.left + insets.right)), 
            dimGreater.height - Math.round(fEnlargementFactor * (insets.top + insets.bottom)));
      }
      
      return dimSmaller;
   }
   
   // use a some private variables in order to only have to
   // determine the logo image size once (per change)
   private Dimension getLogoSize()
   {
      if (m_bDisplayLogo && m_LogoImageName != null)
      {
         if (!m_LogoImageName.equals(m_LogoNameKey))
         {
            ImageIcon icon = new ImageIcon(m_LogoImageName);
            m_nLogoWidth = icon.getIconWidth();
            m_nLogoHeight = icon.getIconHeight();
            
            m_LogoNameKey = m_LogoImageName;
         }
         else
         {
            // image already known; use the saved width and height values
         }
         
         return new Dimension(m_nLogoWidth, m_nLogoHeight);
      }
      
      //return null;
      return new Dimension(0, 0);
   }

   // determine the size of the accompanying video (if any - else [0, 0] is returned)
   private Dimension getAccVideoOutputSize()
   {
   	if (!videoIsStillImage && (outputVideoWidth > 0) && (outputVideoHeight > 0))
      	return new Dimension(outputVideoWidth, outputVideoHeight);
      else
      	return new Dimension(0, 0);
   }
}
