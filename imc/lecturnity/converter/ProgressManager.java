package imc.lecturnity.converter;

import java.util.List;
import java.util.ArrayList;
import javax.swing.JOptionPane;

/* REVIEW UK
 * Use a HashMap for m_listProgressSteps: makes loops/searching superfluous
 *  -> for example IsUniqueStepId() is not used anymore (instead: map.contains())
 * Why are the five public Get methods necessary?
 */

public class ProgressManager {

    //Members
    private ProgressCommunicator m_progressCommunicator;
    private List<ProgressStep> m_listProgressSteps;
    private int m_iCurrentStep;
    private int m_iTotalSteps;
    private int m_iTotalSubSteps;
    private float m_fCurrentSubProgress; // Progress is in the range [0.0..1.0]
    private String m_strCurrentLabel;
//    private float m_fProgressOffset;


    // Constructor
    public ProgressManager() {
        m_progressCommunicator = new ProgressCommunicator();
        m_listProgressSteps = new ArrayList<ProgressStep>();
        Clear();
    }


    // Set methods
    public void Clear() {
        m_iCurrentStep = 0;
        m_iTotalSteps = 0;
        m_fCurrentSubProgress = 0.0f;
        m_strCurrentLabel = "";
//        m_fProgressOffset = 0.0f;
    }


    /**
     * @param strId - the (unique) ID of the progress step
     * @param nWeighting - the weighting of progress step in the range [1..10]
     */
    public void AddProgressStep(String strStepId, int iWeighting) {
        boolean bIsUniqueId = IsUniqueStepId(strStepId);
        if (bIsUniqueId) {
            m_iTotalSubSteps += iWeighting;

            ProgressStep progressStep = new ProgressStep();
            progressStep.strId = strStepId;
            progressStep.iWeighting = iWeighting;
            m_listProgressSteps.add(progressStep);
            m_iTotalSteps = m_listProgressSteps.size();
        } else {
            // Progress step ID already exists --> Throw error message
            String strMsg = "In ProgressManager::AddProgressStep(): \nProgress step ID \"" 
                            + strStepId + "\" already exists!";
            System.out.println("!!! Error: " + strMsg);
            //JOptionPane.showMessageDialog(null, strMsg, "Error", JOptionPane.ERROR_MESSAGE);
        }
    }
    
    /**
     * @param strId - the (unique) ID of the progress step
     * @return the current step (a value >= 0); -1 in case of undefined progress step ID
     */
    public int SetProgressStep(String strStepId) {
        int res = -1;
        
        for (int i = 0; i < m_iTotalSteps; ++i) {
            if (m_listProgressSteps.get(i).strId.equalsIgnoreCase(strStepId)) {
                res = i;
                // Update the current step
                m_iCurrentStep = i;
                m_fCurrentSubProgress = 0.0f;
            }
        }
        
        if (res < 0) {
            // Undefined progress step ID --> Throw error message
            String strMsg = "In ProgressManager::SetProgressStep(): \nProgress step ID \"" 
                            + strStepId + "\" not found!";
            System.out.println("!!! Error: " + strMsg);
            //JOptionPane.showMessageDialog(null, strMsg, "Error", JOptionPane.ERROR_MESSAGE);
        }
        
        
        return res;
    }

    /**
     * @param fProgress - The progress value in the range [0.0..1.0]
     */
    public void SetCurrentSubProgress(float fProgress) {
        if ( (fProgress < 0.0f) || (fProgress > 1.0f) ) { 
            System.out.println("! ProgressManager::SetCurrentSubProgress(" + fProgress + "): \n  Error - Progress outside accepted range [0.0..1.0].");
            return;
        }
        m_fCurrentSubProgress = fProgress;
        // Something has changed --> Call the ProgressCommunicator
        m_progressCommunicator.SendProgress(GetTotalProgress());
    }
    
    /**
     * @param strLabel - the Progress Label
     */
    public void SetProgressLabel(String strLabel) {
        m_strCurrentLabel = strLabel;
        // Something has changed --> Call the ProgressCommunicator
        m_progressCommunicator.SendProgressLabel(m_strCurrentLabel);
        m_progressCommunicator.SendProgress(GetTotalProgress());
    }

//    /**
//     * It is possible (likely) that there were already other progresses:
//     * 2x: Open and Export in the Studio.
//     * In that case only the remaining "progress space" 0.66 .. 1.0 should be used.
//     */
//    public void ActivateProgressOffset(float fOffset) {
//        if ( (fOffset > 0.0f) && (fOffset < 1.0f) )
//            m_fProgressOffset = fOffset;
//    }
    

    // Get methods
    /**
     * @return the current step number (which must be lower than the total number of steps)
     */
    public int GetCurrentStep() {
        return m_iCurrentStep;
    }
    
    /**
     * @return the total number of steps
     */
    public int GetTotalSteps() {
        return m_iTotalSteps;
    }
    
    /**
     * @return the current progress value in the range [0.0..1.0]
     */
    public float GetCurrentSubProgress() {
        return m_fCurrentSubProgress;
    }
    
    /**
     * @return the total progress in the range [0%..100%]
     */
    public int GetTotalProgressPercentage() {
        return Math.round(100.0f * GetTotalProgress());
    }

    /**
     * @return the total progress value in the range [0.0..1.0]
     */
    public float GetTotalProgress() {
        float fProgressOffset = GetProgressStepOffset();
        int iWeighting = m_listProgressSteps.get(m_iCurrentStep).iWeighting;
        float fCurrentSubProgress = m_fCurrentSubProgress * iWeighting / m_iTotalSubSteps;
        float fTotalProgress = fProgressOffset + fCurrentSubProgress;

//        // Is there an "active progress offset" to consider?
//        if (m_fProgressOffset != 0.0f) {
//            float fRemainder = 1.0f - m_fProgressOffset;
//            fTotalProgress = m_fProgressOffset + (fRemainder * fTotalProgress);
//        }

        if (fTotalProgress > 1.0f) { 
            // This should only be possible by rounding errors 
            // but has to be checked if the value is much greater than 1.0
            System.out.println("! ProgressManager::GetTotalProgress(): \n  Warning - Progress exceeds 1.0 (" + fTotalProgress + ").");
            fTotalProgress = 1.0f;
        }

        return fTotalProgress;
    }
    
    /**
     * @return a String containing the current progress label
     */
    public String GetCurrentProgressLabel() {
        return m_strCurrentLabel;
    }

    /**
     * @return a String containing the current progress ID
     */
    public String GetCurrentProgressId() {
        return m_listProgressSteps.get(m_iCurrentStep).strId;
    }

    
    // Message interfaces for ProgressCommunicator 
    public void SendWarningMessage(String strMessage) {
        m_progressCommunicator.SendWarningMessage(strMessage);
    }
    
    public void SendErrorMessage(String strMessage) {
        m_progressCommunicator.SendErrorMessage(strMessage);
    }
    
    public void SendArbitraryMessage(String strId, String strMessage) {
        m_progressCommunicator.SendArbitraryMessage(strId, strMessage);
    }


    // Private methods
    /**
     * @return a progess offset in the range [0.0..1.0]
     */
    private float GetProgressStepOffset() {
        float fProgressOffset = 0.0f;
        
        if (m_iCurrentStep >= 0) {
            int iCurrentSubSteps = 0;
            for (int i = 0 ; i < m_iCurrentStep; ++i)
                iCurrentSubSteps += m_listProgressSteps.get(i).iWeighting;
            fProgressOffset = 1.0f * iCurrentSubSteps / m_iTotalSubSteps;
        }
        
        return fProgressOffset;
    }
    
    private boolean IsUniqueStepId(String strStepId) {
        for (int i = 0 ; i < m_iTotalSteps; ++i) {
            if (m_listProgressSteps.get(i).strId.equalsIgnoreCase(strStepId)) {
                // Progress step ID already exists
                return false;
            }
        }
        return true;
    }


    // Inner class to handle progess steps 
    private class ProgressStep {
        public String strId;
        public int iWeighting;
    }
}