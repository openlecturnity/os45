package imc.lecturnity.converter;

import imc.lecturnity.util.NativeUtils;

public class DptCommunicator {
    private float m_fProgressOffset = 0.0f;
    
    /**
     * It is possible (likely) that there were already other progresses:
     * 2x: Open and Export in the Studio.
     * In that case only the remaining "progress space" 0.66 .. 1.0 should be used.
     */
    public void ActivateProgressOffset(float fOffset) {
        if (fOffset > 0 && fOffset < 1)
            m_fProgressOffset = fOffset;
    }
    
    /**
      * @param f - The progress value ranging from 0.0 to 1.0.
      */
    public void ShowProgress(float f) {
        if (m_fProgressOffset != 0) {
            float fRemainder = 1.0f - m_fProgressOffset;
            f = m_fProgressOffset + f/fRemainder;
        }
        
        NativeUtils.sendDptMessage("[DPT Progress] " + f);
    }
    
    public void ShowProgressLabel(String strLabel) {
        NativeUtils.sendDptMessage("[DPT Label] " + strLabel);
    }
    
    public void ShowWarningMessage(String strMessage) {
        NativeUtils.sendDptMessage("[DPT Warning] " + strMessage);
    }
    
    public void ShowErrorMessage(String strMessage) {
        NativeUtils.sendDptMessage("[DPT Error] " + strMessage);
    }
    
    public void SendArbitraryMessage(String strId, String strMessage) {
        NativeUtils.sendDptMessage("[DPT "+strId+"] " + strMessage);
    }
}