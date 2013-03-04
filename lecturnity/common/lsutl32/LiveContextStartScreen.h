#pragma once

#ifdef _LSUTL32_DLL
    #define LSUTL32_EXPORT __declspec(dllexport)
#else 
    #ifdef _LSUTL32_STATIC
        #define LSUTL32_EXPORT 
    #else
        #define LSUTL32_EXPORT __declspec(dllimport)
    #endif
#endif

#include "resource.h"

class LSUTL32_EXPORT CLiveContextStartScreen : public CDialog {
    DECLARE_DYNAMIC(CLiveContextStartScreen)

public:
    CLiveContextStartScreen(CWnd* pParent = NULL);   // standard constructor
    virtual ~CLiveContextStartScreen();
    virtual BOOL OnInitDialog();

    void Show(CString csState, bool bIsRecordMode, bool bIsEditMode);
    void Close();
    void UpdateProgress(UINT nStep, UINT nTotalSteps);

    // Dialog Data
    enum { IDD = IDD_LIVECONTEXT_STARTSCREEN };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
private:
    CProgressCtrl m_progress;
    CStatic	m_stStatus;
};
