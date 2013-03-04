#pragma once

class IProgressListener : public CObject
{
public:
    enum ProgressMessageType {
        PROGRESS_MSG_ERROR = MB_ICONERROR,
        PROGRESS_MSG_WARNING = MB_ICONWARNING
    };


	IProgressListener();
	virtual ~IProgressListener();

    virtual void SetProgress(float f) = 0;
    virtual void SetProgressLabel(CString& csLabel) = 0;
    virtual void SetProgressLabel(UINT nResourceId);
    virtual void ShowMessage(ProgressMessageType nType, CString& csMessage, CWnd *pWndParent=NULL) = 0;
    virtual void ShowMessage(ProgressMessageType nType, UINT nResourceId, CWnd *pWndParent=NULL);

    // So this needs to be implemented in a class:
    //virtual void SetProgress(float f);
    //virtual void SetProgressLabel(CString& csLabel);
    //virtual void ShowMessage(ProgressMessageType nType, CString& csMessage, CWnd *pWndParent=NULL);
};


