// IAreaDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "Studio.h"
#include "IAreaDialog.h"
#include "MfcUtils.h"


// CIAreaDialog dialog

IMPLEMENT_DYNAMIC(CIAreaDialog, CDialog)

CIAreaDialog::CIAreaDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CIAreaDialog::IDD, pParent)
{

    m_bIsInitialized = false;
}

CIAreaDialog::~CIAreaDialog()
{
}

void CIAreaDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDOK, m_btnOK);
    DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_DIVIDER, m_Divider);
}


BEGIN_MESSAGE_MAP(CIAreaDialog, CDialog)
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_BN_CLICKED(IDOK, &CIAreaDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CIAreaDialog message handlers

BOOL CIAreaDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    CString csTitle;
    csTitle.LoadString(CInteractionAreaDialog::IDD);
    SetWindowText(csTitle);

    DWORD dwIds[] = {IDOK, IDCANCEL, -1};

    MfcUtils::Localize(this, dwIds);


    InitChildWin(&m_dlgInside, IDD_INTERACTION_AREA);
    m_dlgInside.ShowWindow( SW_SHOW );


    CRect rcOK;
    m_btnOK.GetWindowRect(&rcOK);


    int screenY = GetSystemMetrics(SM_CYMAXIMIZED);

    CRect rcWin;
    GetWindowRect(&rcWin);
    //m_iButtonVOffset = rcWin.bottom - rcOK.bottom;
	m_iButtonVOffset = 15;// * screenY / 1000;
    m_szInitial = rcWin.Size();

    if( screenY < rcWin.Height() + 40) {
        rcWin.top = 20;
        rcWin.bottom = screenY - 20;
    }
    MoveWindow(&rcWin, TRUE);
    CenterWindow();   

    m_bIsInitialized = true;

	RepositionControls();
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CIAreaDialog::InitChildWin(CDialog* pDlg, UINT iWinID){
    CWnd* pWnd = GetDlgItem( IDC_PIC_MAIN );
    CRect rcValue;

    if (pWnd && pDlg) {
        pWnd->GetWindowRect( &rcValue ); // Use picture box position.
        ScreenToClient( &rcValue );
        pDlg->Create( iWinID, this );
        pDlg->SetWindowPos( pWnd, rcValue.left, rcValue.top,
            rcValue.Width(), rcValue.Height(), SWP_HIDEWINDOW );
    }
}

void CIAreaDialog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if(m_dlgInside.GetSafeHwnd()) {
        CRect rcWin;
        GetClientRect(&rcWin);

        CRect rcIn;
        m_dlgInside.GetWindowRect(&rcIn);
        rcIn.bottom = rcIn.top + rcWin.Height() - 40;
        ScreenToClient(&rcIn);
        
        m_dlgInside.SetWindowPos(NULL, rcIn.left, rcIn.top, rcIn.Width(), rcIn.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

        RepositionControls();
    }
}

BOOL CIAreaDialog::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if(pMsg->message == WM_MOUSEWHEEL) {
        if(m_dlgInside.GetSafeHwnd()){
            m_dlgInside.SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CIAreaDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: Add your message handler code here and/or call default

    if (m_bIsInitialized) {
        lpMMI->ptMinTrackSize.x = m_szInitial.cx;
        lpMMI->ptMinTrackSize.y = 300;
        lpMMI->ptMaxTrackSize.x = m_szInitial.cx;
        lpMMI->ptMaxTrackSize.y = m_szInitial.cy;
    }
    CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CIAreaDialog::ContinueModal()
{
    // TODO: Add your specialized code here and/or call the base class
    if(m_dlgInside.GetSafeHwnd())
    {
        m_dlgInside.ContinueModal();
    }

    return CDialog::ContinueModal();
}

void CIAreaDialog::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    if (m_dlgInside.GetSafeHwnd())
    {
        m_dlgInside.OnBtnClickOK();
    }
    OnOK();
}

void CIAreaDialog::RepositionControls() 
{
        CRect rcWin;
        GetWindowRect(&rcWin);

        CRect rcOK;
        m_btnOK.GetWindowRect(&rcOK);
        CRect rcCancel;
        m_btnCancel.GetWindowRect(&rcCancel);
       /* CRect rcBack;
        m_stBottom.GetWindowRect(&rcBack);*/
        

        CRect rcControl;
        rcControl = rcOK;
        rcControl.top = rcWin.bottom - m_iButtonVOffset - rcOK.Height();
        rcControl.bottom = rcWin.bottom - m_iButtonVOffset;
        
        rcControl.left = rcOK.left;
        rcControl.right = rcOK.right;
        ScreenToClient(&rcControl);
        //m_btnOK.MoveWindow(&rcControl, TRUE);

        CRect rcControl1;
        rcControl1 = rcCancel;
        rcControl1.top = rcWin.bottom - m_iButtonVOffset - rcCancel.Height();
        rcControl1.bottom = rcWin.bottom - m_iButtonVOffset;
        
        rcControl1.left = rcCancel.left;
        rcControl1.right = rcCancel.right;
        ScreenToClient(&rcControl1);
        //m_btnCancel.MoveWindow(&rcControl, TRUE);

		CRect rcDiv;
        m_Divider.GetWindowRect(&rcDiv);        

        CRect rcControl2;
        rcControl2 = rcDiv;
        rcControl2.top = rcWin.bottom - m_iButtonVOffset - rcOK.Height() - 6;
        rcControl2.bottom = rcWin.bottom - m_iButtonVOffset - rcOK.Height() - 5;
        
        //rcControl.left = rcOK.left;
        //rcControl.right = rcOK.right;
        ScreenToClient(&rcControl2);

        /*CRect rcControl2;
        rcControl2 = rcBack;
        rcControl2.top = rcWin.bottom - m_iBackVOffset - rcBack.Height();
        rcControl2.bottom = rcWin.bottom - m_iBackVOffset;
        
        rcControl2.left = rcBack.left;
        rcControl2.right = rcBack.right;
        ScreenToClient(&rcControl2);*/
        //m_stBottom.MoveWindow(&rcControl, TRUE);
         

        HDWP hDefer = BeginDeferWindowPos(3);
        UINT uFlags = SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER;
        if(hDefer)
            DeferWindowPos(hDefer, m_btnOK.m_hWnd, NULL, rcControl.left, rcControl.top, rcControl.Width(), rcControl.Height(), uFlags);
        if(hDefer)
            DeferWindowPos(hDefer, m_btnCancel.m_hWnd, NULL, rcControl1.left, rcControl1.top, rcControl1.Width(), rcControl1.Height(), uFlags);
		if(hDefer)
            DeferWindowPos(hDefer, m_Divider.m_hWnd, NULL, rcControl2.left, rcControl2.top, rcControl2.Width(), rcControl2.Height(), uFlags);
       /* if(hDefer)
            DeferWindowPos(hDefer, m_stBottom.m_hWnd, NULL, rcControl2.left, rcControl2.top, rcControl2.Width(), rcControl2.Height(), uFlags);*/
        

        EndDeferWindowPos(hDefer);
        //InvalidateRect(&rcControl2);
}

void CIAreaDialog::Init(CInteractionAreaEx *pInteractionButton, CEditorDoc *pEditorDoc) {
    m_dlgInside.Init(pInteractionButton, pEditorDoc);
}

void CIAreaDialog::SetUserEntries(CRect &rcArea, CInteractionAreaEx *pInteraction) {
    m_dlgInside.SetUserEntries(rcArea, pInteraction);
}
