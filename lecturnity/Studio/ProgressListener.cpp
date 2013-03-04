#include "stdafx.h"
#include "Studio.h"
#include "ProgressListener.h"

IProgressListener::IProgressListener()
{
}

IProgressListener::~IProgressListener()
{
}

void IProgressListener::SetProgressLabel(UINT nResourceId) {
    CString csMessage;
    BOOL bSuccess = csMessage.LoadString(nResourceId);
    SetProgressLabel(csMessage);
}

void IProgressListener::ShowMessage(ProgressMessageType nType, UINT nResourceId, CWnd *pWndParent) {
    CString csMessage;
    BOOL bSuccess = csMessage.LoadString(nResourceId);
    ShowMessage(nType, csMessage, pWndParent);
}