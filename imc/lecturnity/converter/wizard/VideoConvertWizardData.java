package imc.lecturnity.converter.wizard;

import java.awt.Dimension;

public class VideoConvertWizardData extends ConvertWizardData
{
   public Dimension m_dimOutputSize = new Dimension(320, 240);
   public int m_iVideoQFactor = 2;
   public int m_iAudioRate = 48;
   public int m_iFrameRate = 5;
   public boolean m_bEmbedClips = true;
   public boolean m_bVideoInsteadOfPages = false;
   //public String m_strTargetFile = null;

   public String getSummary()
   {
      String summary = "";
      
      return summary;
   }
   
   
   public void fillOtherWizardData(ConvertWizardData otherData)
   {
      if (otherData instanceof VideoConvertWizardData)
      {
         VideoConvertWizardData that = (VideoConvertWizardData)otherData;
         
         
         that.m_dimOutputSize = new Dimension(m_dimOutputSize.width, m_dimOutputSize.height);
         that.m_iVideoQFactor = m_iVideoQFactor;
         that.m_iAudioRate = m_iAudioRate;
         that.m_iFrameRate = m_iFrameRate; 
         that.m_bVideoInsteadOfPages = m_bVideoInsteadOfPages;
      }

      super.fillOtherWizardData(otherData);
   }
   
}
