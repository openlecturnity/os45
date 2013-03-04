#include "StdAfx.h"
#include "DptCommunicator.h"

CDptCommunicator::CDptCommunicator(void)
{
    m_bProgressScaling = false;
    m_fLastProgress = 0.0f;
    m_iAchievedProgresses = 0;
    m_iTotalProgresses = 1;

    m_DptPipe.Init(_T("dpt_msg_pipe"), false);
}

CDptCommunicator::~CDptCommunicator(void)
{
}

void CDptCommunicator::ActivateProgressScaling(int iTotal) {
    if (iTotal <= 0)
        iTotal = 1; // avoid divide by zero

    m_bProgressScaling = true;
    m_iTotalProgresses = iTotal;
}

void CDptCommunicator::SetProgress(float f) {
    if (m_bProgressScaling) {
        if (f < m_fLastProgress) 
            m_iAchievedProgresses += 1;
        // this is more or less a heuristic: negative progress is not allowed
        // if f < m_fLastProgress it is assumed that f was 1.0 earlier and is now 0.0
        // (but both values cannot be guaranteed to be exactly set/reached)

        m_fLastProgress = f;

        f += m_iAchievedProgresses;

        f /= m_iTotalProgresses;
    }   


    CString csProgress;
    csProgress.Format(_T("[DPT Progress] %f"), f);

    m_DptPipe.WriteMessage(csProgress); // may return E_UNEXPECTED if the above Init() failed
}

void CDptCommunicator::SetProgressLabel(CString& csLabel) {
    CString csProgressLabel;
    csProgressLabel.Format(_T("[DPT Label] %s"), csLabel);

    m_DptPipe.WriteMessage(csProgressLabel);
}

void CDptCommunicator::ShowMessage(ProgressMessageType nType, CString& csMessage, CWnd *pWndParent) {
    CString csShowMessage;

    if (nType == PROGRESS_MSG_ERROR)
        csShowMessage.Format(_T("[DPT Error] %s"), csMessage);
    else if (nType == PROGRESS_MSG_WARNING)
        csShowMessage.Format(_T("[DPT Warning] %s"), csMessage);

    m_DptPipe.WriteMessage(csShowMessage);
}