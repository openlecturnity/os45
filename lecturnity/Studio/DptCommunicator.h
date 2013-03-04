#pragma once
#include "ProgressListener.h"
#include "lutils.h" // LIo::MessagePipe

class CDptCommunicator : public IProgressListener {
public:
    CDptCommunicator(void);
    ~CDptCommunicator(void);

    /** There can be several times the progress 0 - 1.0; it is translated in only one. */
    void ActivateProgressScaling(int iTotal);

    virtual void SetProgress(float f);
    virtual void SetProgressLabel(CString& csLabel);
    virtual void ShowMessage(ProgressMessageType nType, CString& csMessage, CWnd *pWndParent=NULL);

private:
    bool m_bProgressScaling;
    float m_fLastProgress;
    int m_iAchievedProgresses;
    int m_iTotalProgresses;
    LIo::MessagePipe m_DptPipe;
};
