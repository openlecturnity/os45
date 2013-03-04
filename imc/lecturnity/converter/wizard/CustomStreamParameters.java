package imc.lecturnity.converter.wizard;

import java.awt.*;

import javax.swing.*;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.converter.ConverterWizard;
import imc.lecturnity.converter.DocumentData;
import imc.lecturnity.converter.ProfiledSettings;
import imc.lecturnity.converter.StreamInformation;
import imc.lecturnity.util.ui.LayoutUtils;
import imc.lecturnity.util.wizards.WizardPanel;

public class CustomStreamParameters extends WizardPanel
{
   private static String BITRATE = "[!]";
   private static String FRAMERATE = "[!]";
   private static String FPS = "[!]";
   private static String KBPS = "[!]";
   private static String SETTINGS = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/CustomStreamParameters_", "en");
         BITRATE = l.getLocalized("BITRATE");
         FRAMERATE = l.getLocalized("FRAMERATE");
         FPS = l.getLocalized("FPS");
         KBPS = l.getLocalized("KBPS");
         SETTINGS = l.getLocalized("SETTINGS");
         //MNEM_VIDEOSIZE = l.getLocalized("MNEM_VIDEOSIZE").charAt(0);
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open CustomStreamParameters locale database!\n"+
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }
      
   private PublisherWizardData pwData_ = null;
   
   private JComboBox boxAudioBitrates_ = null;
   private JComboBox boxVideoBitrates_ = null;
   private JComboBox boxVideoFramerates_ = null;
   private JComboBox boxClipBitrates_ = null;
   private JComboBox boxClipFramerates_ = null;
   
   // Flash Videos have a fix frame rate so far --> hide the corresponding ComboBoxes
   private boolean bForFlash_ = false;
   
   public CustomStreamParameters(ExtendedSettingsDialog parent, PublisherWizardData pwData)
   {
      pwData_ = pwData;
      
      if (pwData_.GetProfiledSettings().GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH)
         bForFlash_ = true;
      
      initGui();
   }
   
   public PublisherWizardData getWizardData()
   {
      //
      // update the values in data_ according to the texts/user input
      // in the gui components
      // if an error occurs that value stays the same
      //
      
      DocumentData dd = pwData_.GetDocumentData();
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      StreamInformation siAudioStreamInfo = dd.GetAudioStreamInfo();
      StreamInformation siVideoStreamInfo = dd.GetVideoStreamInfo();
      StreamInformation siClipsStreamInfo = dd.GetClipsStreamInfo();
      
      // This dialog is used for WM (default) and Flash export 
      // --> both formats can have different bitrates/framerates.
      boolean bForFlash = (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH);
      boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && ps.GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;
            
      if (bForFlash && bIsGenericProfile) {
          // set dummy streams for Generic Profiles if required (Flash only)
          // WM case is handled in WmEncoderWizardPanel
          if (siVideoStreamInfo == null) {
              siVideoStreamInfo = new StreamInformation(StreamInformation.VIDEO_STREAM);
              siVideoStreamInfo.m_nBitrate = ps.GetIntValue("iFlashVideoBitrate");
          }
          if (siClipsStreamInfo == null) {
              siClipsStreamInfo = new StreamInformation(StreamInformation.VIDEO_STREAM);
              siClipsStreamInfo.m_nBitrate = ps.GetIntValue("iFlashClipBitrate");
          }
      }

      int nNewBitrate = siAudioStreamInfo.m_nBitrate;
      try {
         nNewBitrate = Integer.parseInt((String)boxAudioBitrates_.getSelectedItem())*1000;
      } catch (Exception exc) {}
            
      if (nNewBitrate > 0)
      {
         siAudioStreamInfo.m_nBitrate = nNewBitrate;

         if (bForFlash)
            ps.SetIntValue("iFlashAudioBitrate", nNewBitrate);
         else
            ps.SetIntValue("iWmAudioBitrate", nNewBitrate);
      }
         
      
      if (siVideoStreamInfo != null)
      {
         int nNewBitrate2 = siVideoStreamInfo.m_nBitrate;
         
         try {
            // Bugfix 5577: automatic bitrate feature
            if (bForFlash && "AUTO".equals(boxVideoBitrates_.getSelectedItem()))
               // Flash automatic mode
               nNewBitrate2 = -1;
            else
               nNewBitrate2 = Integer.parseInt((String)boxVideoBitrates_.getSelectedItem())*1000;;
         } catch (Exception exc) {}
         
         if (nNewBitrate2 != 0)
         {
            siVideoStreamInfo.m_nBitrate = nNewBitrate2;

            if (bForFlash)
               ps.SetIntValue("iFlashVideoBitrate", nNewBitrate2);
            else
               ps.SetIntValue("iWmVideoBitrate", nNewBitrate2);
         }
         
         if (!bForFlash) {
             int nNewFramerateTenths2 = siVideoStreamInfo.m_nFrequency;
           
             try {
                String text = (String)boxVideoFramerates_.getSelectedItem();
                text = text.replace(',', '.');
                nNewFramerateTenths2 = (int)(Double.parseDouble(text)*10);
             } catch (Exception exc) {}
             
             if (nNewFramerateTenths2 > 0)
             {
                siVideoStreamInfo.m_nFrequency = nNewFramerateTenths2;
    
                ps.SetIntValue("iVideoFramerateTenths", nNewFramerateTenths2);
             }
         }
      }
      
      if (siClipsStreamInfo != null)
      {
         int nNewBitrate3 = siClipsStreamInfo.m_nBitrate;
         
         try { 
            // Bugfix 5577: automatic bitrate feature
            if (bForFlash && "AUTO".equals(boxClipBitrates_.getSelectedItem()))
               // Flash automatic mode
               nNewBitrate3 = -1;
            else
               nNewBitrate3 = Integer.parseInt((String)boxClipBitrates_.getSelectedItem())*1000;
         } catch (Exception exc) {}
         
         if (nNewBitrate3 != 0)
         {
            siClipsStreamInfo.m_nBitrate = nNewBitrate3;

            if (bForFlash)
               ps.SetIntValue("iFlashClipBitrate", nNewBitrate3);
            else
               ps.SetIntValue("iWmClipBitrate", nNewBitrate3);
         }
      
      
         if (!bForFlash) {
             int nNewFramerateTenths3 = siClipsStreamInfo.m_nFrequency;
             
             try {
                String text = (String)boxClipFramerates_.getSelectedItem();
                text = text.replace(',', '.');
                nNewFramerateTenths3 = (int)(Double.parseDouble(text)*10);
             } catch (Exception exc) {}
             
             if (nNewFramerateTenths3 > 0)
             {
                siClipsStreamInfo.m_nFrequency = nNewFramerateTenths3;
    
                ps.SetIntValue("iClipFramerateTenths", nNewFramerateTenths3);
             }
         }
      }
      
      return pwData_;
   }
   
   public String getDescription()
   {
      return "Adjust bitrates and framerates of streams.";
   }
   
   private void initGui()
   {
      Font boldFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font normalFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);
      
      DocumentData dd = pwData_.GetDocumentData();
      ProfiledSettings ps = pwData_.GetProfiledSettings();

      StreamInformation siAudioStreamInfo = dd.GetAudioStreamInfo();
      StreamInformation siVideoStreamInfo = dd.GetVideoStreamInfo();
      StreamInformation siClipsStreamInfo = dd.GetClipsStreamInfo();
      boolean isFlashExport = (ps.GetIntValue("iExportType") == PublisherWizardData.EXPORT_TYPE_FLASH);

      
      this.setBorder(BorderFactory.createEmptyBorder(20, 15, 15, 20));
      this.setLayout(new BorderLayout());
      JPanel pnlMajor = new JPanel();
      pnlMajor.setLayout(new BorderLayout());
      this.add("Center", pnlMajor);
      JPanel pnlUpper = new JPanel();//new GridLayout(0,1,0,10));
      BoxLayout b3 = new BoxLayout(pnlUpper, BoxLayout.Y_AXIS);
      pnlUpper.setLayout(b3);
      pnlMajor.add("North", pnlUpper);
      // space below is free
      
      
      JPanel pnlAudio = new JPanel(new FlowLayout(FlowLayout.LEFT));
      pnlAudio.setFont(normalFont);
      pnlAudio.setBorder(BorderFactory.createTitledBorder("Audio "+SETTINGS));
      JPanel pnlVideo = new JPanel(new FlowLayout(FlowLayout.LEFT));
      pnlVideo.setFont(normalFont);
      pnlVideo.setBorder(BorderFactory.createTitledBorder("Video "+SETTINGS));
      JPanel pnlClips = new JPanel(new FlowLayout(FlowLayout.LEFT));
      pnlClips.setFont(normalFont);
      pnlClips.setBorder(BorderFactory.createTitledBorder("Clip "+SETTINGS));
      
      
      ////////
      // AUDIO
      ////////
      int nAudioBitrateK = siAudioStreamInfo.m_nBitrate/1000;

      if (pwData_.m_bIsProfileMode)
      {
         if (isFlashExport)
            nAudioBitrateK = ps.GetIntValue("iFlashAudioBitrate")/1000;
         else
            nAudioBitrateK = ps.GetIntValue("iWmAudioBitrate")/1000;
      }
     
      String[] aAudioBitrates;
      if (isFlashExport)
         aAudioBitrates = new String[] { "8", "16", "24", "32", "40", "48", "56", "64" };
      else // must be WM export
         aAudioBitrates = new String[] { "6", "10", "16", "20", "32", "48" };
      
      int idx = findArrayIndex(aAudioBitrates, nAudioBitrateK+"");
      /* should not be possible for audio
      if (idx < 0)
      {
         idx = (idx+1)*(-1);
         aAudioBitrates = enlargeAndInsert(aAudioBitrates, nAudioBitrateK+"", idx);
      }
      */
      boxAudioBitrates_ = new JComboBox(aAudioBitrates);
      boxAudioBitrates_.setPreferredSize(
         new Dimension(50, boxAudioBitrates_.getPreferredSize().height));
      boxAudioBitrates_.setSelectedIndex(idx);
      
      pnlAudio.add(new JLabel(BITRATE+": "));
      pnlAudio.add(boxAudioBitrates_);
      pnlAudio.add(new JLabel(KBPS));
      
      pnlUpper.add(LayoutUtils.makeBoxable(pnlAudio, true));
      
      
      ////////
      // VIDEO
      ////////
      boolean bIsGenericProfile = ConverterWizard.m_bGenericProfilesEnabled && ps.GetType() == PublisherWizardData.DOCUMENT_TYPE_UNKNOWN;

      if (siVideoStreamInfo != null || bIsGenericProfile)
      {  // stream available or generic profile
         int nVideoBitrateK = 0;
         double dVideoFramerate = 0;

         if (siVideoStreamInfo != null) {
             nVideoBitrateK = siVideoStreamInfo.m_nBitrate/1000;
             dVideoFramerate = siVideoStreamInfo.m_nFrequency/10.0;             
         }
         if (!bIsGenericProfile )
            if (isFlashExport && ps.GetIntValue("iFlashVideoBitrate") < 0) {
               // Bugfix 4945: optimize bitrates
               nVideoBitrateK = FlashSettingsWizardPanel.GetFlashOptimizedVideoBitrate(siVideoStreamInfo.m_nWidth, 
                                                                                       siVideoStreamInfo.m_nHeight, 
                                                                                       (siVideoStreamInfo.m_nFrequency/10))/1000;
         }
         
         if (pwData_.m_bIsProfileMode)
         {
            if (isFlashExport) {
               // Bugfix 5575: bitrate not initialized
               // nVideoBitrateK = ps.GetIntValue("iFlashVideoBitrate")/1000;
               nVideoBitrateK = ps.GetIntValue("iFlashVideoBitrate");
               if ( nVideoBitrateK > 0 )
                   nVideoBitrateK /= 1000;
            }
            else
               nVideoBitrateK = ps.GetIntValue("iWmVideoBitrate")/1000;

            dVideoFramerate = ps.GetIntValue("iVideoFramerateTenths")/10.0;
         }
         
         String[] aVideoBitrates;
         if (isFlashExport)
            aVideoBitrates = new String[] { "40", "60", "100", "150", 
               "200", "300", "400", "500", "600", "700", "800", "900", "1000", "1200", "1400", "1600", 
               "2000", "2500", "3000", "3500", "4000", "4500", "5000", "6000", "7000", "8000", "9000", "10000" };
         else // must be WM export
            aVideoBitrates = new String[] { "10", "15", "20", "25",
               "40", "60", "100", "150", "200", "300", "400", "500", "600" };

         int idx2 = findArrayIndex(aVideoBitrates, nVideoBitrateK+"");
         if (idx2 < 0 && nVideoBitrateK > 0)
         {
            idx2 = (idx2+1)*(-1);
            aVideoBitrates = enlargeAndInsert(aVideoBitrates, nVideoBitrateK+"", idx2);
         }

         if (isFlashExport) {
             // Bugfix 5575: bitrate not initialized
             // Bugfix 5577: automatic bitrate feature
             // add automatic mode for Flash
             // Note: must be done after findArrayIndex()
             aVideoBitrates = enlargeAndInsert(aVideoBitrates, "AUTO", 0);
             idx2++;
             if ( nVideoBitrateK < 0 ) 
                 // index of AUTO
                 idx2 = 0;
         }

         boxVideoBitrates_ = new JComboBox(aVideoBitrates);
         boxVideoBitrates_.setPreferredSize(
            new Dimension(70, boxVideoBitrates_.getPreferredSize().height));
         boxVideoBitrates_.setEditable(true);
         boxVideoBitrates_.setSelectedIndex(idx2);
      
         pnlVideo.add(new JLabel(BITRATE+": "));
         pnlVideo.add(boxVideoBitrates_);
         pnlVideo.add(new JLabel(KBPS));

         
         
         JLabel lblSpacer = new JLabel();
         lblSpacer.setPreferredSize(new Dimension(15, lblSpacer.getPreferredSize().height));
     
         pnlVideo.add(lblSpacer);
      
         
         String[] aVideoFramerates = new String[] { 
            "1", "2", "5", "10", "15", "20", "25", "30" };
         int idx3 = findArrayIndex(aVideoFramerates, dVideoFramerate+"");
         if (idx3 < 0)
         {
            idx3 = (idx3+1)*(-1);
            aVideoFramerates = enlargeAndInsert(aVideoFramerates, dVideoFramerate+"", idx3);
         }
         boxVideoFramerates_ = new JComboBox(aVideoFramerates);
         boxVideoFramerates_.setPreferredSize(
            new Dimension(50, boxVideoFramerates_.getPreferredSize().height));
         boxVideoFramerates_.setEditable(true);
         boxVideoFramerates_.setSelectedIndex(idx3);
         
         JPanel pnlHelper1 = new JPanel(new GridLayout(0,1));
         JPanel pnlHelper2 = new JPanel();
         
         pnlHelper2.add(boxVideoFramerates_);
         pnlHelper1.add(new JCheckBox("Wie Original"));
         pnlHelper1.add(pnlHelper2);
         
         // Flash Videos have a fix frame rate so far 
         //--> do not show the corresponding ComboBoxes
         if (!bForFlash_) 
         {
            pnlVideo.add(new JLabel(FRAMERATE+": "));
            pnlVideo.add(boxVideoFramerates_);//pnlVideo.add(pnlHelper1);
            pnlVideo.add(new JLabel(FPS));
         }
      
         pnlUpper.add(LayoutUtils.makeBoxable(pnlVideo, true));
      }
      
      ////////
      // CLIPS
      ////////
      if (siClipsStreamInfo != null || bIsGenericProfile)
      {  // stream available or generic profile
         int nClipsBitrateK = 0;
         double dClipsFramerate = 0;
         
         if (siClipsStreamInfo != null) {
             nClipsBitrateK = siClipsStreamInfo.m_nBitrate/1000;
             dClipsFramerate = siClipsStreamInfo.m_nFrequency/10.0;             
         }
         
         if (!bIsGenericProfile)
            if (isFlashExport && ps.GetIntValue("iFlashClipBitrate") < 0) {
               // Bugfix 4945: optimize bitrates
               nClipsBitrateK = FlashSettingsWizardPanel.GetFlashOptimizedVideoBitrate(siClipsStreamInfo.m_nWidth, 
                                                                                       siClipsStreamInfo.m_nHeight, 
                                                                                       (siClipsStreamInfo.m_nFrequency/10))/1000;
         }
         
         if (pwData_.m_bIsProfileMode)
         {
            if (isFlashExport) {
               // Bugfix 5575: bitrate not initialized
               // nClipsBitrateK = ps.GetIntValue("iFlashClipBitrate")/1000;
               nClipsBitrateK = ps.GetIntValue("iFlashClipBitrate");
               if ( nClipsBitrateK > 0 )
                   nClipsBitrateK /= 1000;
            }
            else
               nClipsBitrateK = ps.GetIntValue("iWmClipBitrate")/1000;

            dClipsFramerate = ps.GetIntValue("iClipFramerateTenths")/10.0;
         }

         String[] aClipBitrates;
         if (isFlashExport)
            aClipBitrates = new String[] { "40", "60", "100", "150", 
               "200", "300", "400", "500", "600", "700", "800", "900", "1000", "1200", "1400", "1600", 
               "2000", "2500", "3000", "3500", "4000", "4500", "5000", "6000", "7000", "8000", "9000", "10000" };
         else // must be WM export
            aClipBitrates = new String[] { "10", "15", "20", "25",
            "40", "60", "100", "150", "200", "350", "500", "1000" };
         int idx2 = findArrayIndex(aClipBitrates, nClipsBitrateK+"");
         if (idx2 < 0 && nClipsBitrateK > 0)
         {
            idx2 = (idx2+1)*(-1);
            aClipBitrates = enlargeAndInsert(aClipBitrates, nClipsBitrateK+"", idx2);
         }
         
         if (isFlashExport) {
             // Bugfix 5575: bitrate not initialized
             // Bugfix 5577: automatic bitrate feature
             // add automatic mode for Flash
             // Note: must be done after findArrayIndex()
             aClipBitrates = enlargeAndInsert(aClipBitrates, "AUTO", 0);
             idx2++;
             if ( nClipsBitrateK < 0 ) 
                 // index of AUTO
                 idx2 = 0;
         }
         
         boxClipBitrates_ = new JComboBox(aClipBitrates);
         boxClipBitrates_.setPreferredSize(
            new Dimension(70, boxClipBitrates_.getPreferredSize().height));
         boxClipBitrates_.setEditable(true);
         boxClipBitrates_.setSelectedIndex(idx2);
      
         pnlClips.add(new JLabel(BITRATE+": "));
         pnlClips.add(boxClipBitrates_);
         pnlClips.add(new JLabel(KBPS));

         
         JLabel lblSpacer = new JLabel();
         lblSpacer.setPreferredSize(new Dimension(15, lblSpacer.getPreferredSize().height));
     
         pnlClips.add(lblSpacer);
      
      
         String[] aClipFramerates = new String[] { "1", "2", "5", "10" };
         int idx3 = findArrayIndex(aClipFramerates, dClipsFramerate+"");
         if (idx3 < 0)
         {
            idx3 = (idx3+1)*(-1);
            aClipFramerates = enlargeAndInsert(aClipFramerates, dClipsFramerate+"", idx3);
         }
         boxClipFramerates_ = new JComboBox(aClipFramerates);
         boxClipFramerates_.setPreferredSize(
            new Dimension(50, boxClipFramerates_.getPreferredSize().height));
         boxClipFramerates_.setEditable(true);
         boxClipFramerates_.setSelectedIndex(idx3);
      
         // Flash Videos have a fix frame rate so far 
         //--> do not show the corresponding ComboBoxes
         if (!bForFlash_)
         {
            pnlClips.add(new JLabel(FRAMERATE+": "));
            pnlClips.add(boxClipFramerates_);
            pnlClips.add(new JLabel(FPS));
         }
   
         pnlUpper.add(LayoutUtils.makeBoxable(pnlClips, true));
      }
   }
   

   
   /**
    * Inserts the given String in an array at the desired position.
    *
    * @returns the new enlarged array
    */
   private String[] enlargeAndInsert(String[] aCurrent, String toInsert, int nInsertPosition)
   {
      String[] newArray = new String[aCurrent.length+1];
      if (nInsertPosition > 0) // copy beginning
         System.arraycopy(aCurrent, 0, newArray, 0, nInsertPosition);
      newArray[nInsertPosition] = toInsert;
      if (nInsertPosition < aCurrent.length) // copy remainder
         System.arraycopy(
            aCurrent, nInsertPosition, newArray, nInsertPosition+1, aCurrent.length-nInsertPosition);
      
      return newArray;
   }
   
//   private String makeDoubleString(double dValue, int nDecimalPlaces)
//   {
//      int nMultiplier = 10*nDecimalPlaces;
//      
//      double dTemp = dValue*nMultiplier;
//      long lTemp = (long)dTemp;
//      
//      return (lTemp/nMultiplier)+"."+(lTemp%nMultiplier);
//   }
}