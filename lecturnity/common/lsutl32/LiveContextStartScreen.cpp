// LiveContextStartScreen.cpp : implementation file
//

#include "stdafx.h"
#include "lsutl32.h"
#include "LiveContextStartScreen.h"


// CLiveContextStartScreen dialog

IMPLEMENT_DYNAMIC(CLiveContextStartScreen, CDialog)

CLiveContextStartScreen::CLiveContextStartScreen(CWnd* pParent /*=NULL*/)
	: CDialog(CLiveContextStartScreen::IDD, pParent) {
}

CLiveContextStartScreen::~CLiveContextStartScreen() {
}

void CLiveContextStartScreen::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS, m_progress);
    DDX_Control(pDX, IDC_STATUS, m_stStatus);   
}


BEGIN_MESSAGE_MAP(CLiveContextStartScreen, CDialog)
END_MESSAGE_MAP()


// CLiveContextStartScreen message handlers

BOOL CLiveContextStartScreen::OnInitDialog() {
    CDialog::OnInitDialog();

    m_progress.SetRange(0,100);
    m_progress.SetPos(0);

    return TRUE;
}

void CLiveContextStartScreen::Show(CString csState, bool bIsRecordMode, bool bIsEditMode) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Create(CLiveContextStartScreen::IDD);
    ShowWindow(SW_SHOW);
    m_stStatus.SetWindowText(csState);

    CString csLiveContextMode;
    if (bIsRecordMode) {
        csLiveContextMode.LoadString(IDS_LC_RECORD_MODE);    
    } else if (bIsEditMode) {
        csLiveContextMode.LoadString(IDS_LC_EDIT_MODE);  
    } else {
        csLiveContextMode.LoadString(IDS_LC_PREVIEW_MODE);  
    }
    SetWindowText(csLiveContextMode);
}

void CLiveContextStartScreen::UpdateProgress(UINT nStep, UINT nTotalSteps) {
    int nPos = (nStep*100)/nTotalSteps;
    m_progress.SetPos(nPos);
}

void CLiveContextStartScreen::Close() {
    // Use the CWnd::DestroyWindow function to destroy a dialog box created by the Create function
    DestroyWindow();
}