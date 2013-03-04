// MyTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MyTabCtrl.h"
#include "Resource.h"
#include "MetadataPage.h"
#include "TransferPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MyTabCtrl

MyTabCtrl::MyTabCtrl() {
    m_DialogID[0] = IDD_METADATA_PAGE;
    m_DialogID[1] = IDD_TRANSFER_PAGE;

    m_Dialog[0] = new CMetadataPage();
    m_Dialog[1] = new CTransferPage();

    m_nPageCount = 2;
}

MyTabCtrl::~MyTabCtrl() {
    delete m_Dialog[0];
    delete m_Dialog[1];
}

void MyTabCtrl::InitDialogs() {
    m_Dialog[0]->Create(m_DialogID[0], GetParent());
    m_Dialog[1]->Create(m_DialogID[1], GetParent());
}

BEGIN_MESSAGE_MAP(MyTabCtrl, CTabCtrl)
    //{{AFX_MSG_MAP(MyTabCtrl)
    ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MyTabCtrl message handlers

void MyTabCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) {
    // TODO: Add your control notification handler code here
    ActivateTabDialogs();
    *pResult = 0;
}

void MyTabCtrl::ActivateTabDialogs() {
    int nSel = GetCurSel();
    if (m_Dialog[nSel]->m_hWnd) {
        m_Dialog[nSel]->ShowWindow(SW_HIDE);
    }

    CRect l_rectClient;
    CRect l_rectWnd;

    GetClientRect(l_rectClient);
    AdjustRect(FALSE,l_rectClient);
    GetWindowRect(l_rectWnd);
    GetParent()->ScreenToClient(l_rectWnd);
    l_rectClient.OffsetRect(l_rectWnd.left,l_rectWnd.top);

    for (int nCount=0; nCount < m_nPageCount; nCount++) {
        m_Dialog[nCount]->SetWindowPos(&wndTop,
            l_rectClient.left,
            l_rectClient.top,
            l_rectClient.Width(),
            l_rectClient.Height(),
            SWP_HIDEWINDOW);
    }

    m_Dialog[nSel]->SetWindowPos(&wndTop,
        l_rectClient.left,
        l_rectClient.top,
        l_rectClient.Width(),
        l_rectClient.Height(),
        SWP_SHOWWINDOW);

    m_Dialog[nSel]->ShowWindow(SW_SHOW);
}

void MyTabCtrl::GetTransferSettings(CString csChannelName) {
    CTransferPage* dlgTransferPage = (CTransferPage*) m_Dialog[1];
    if (dlgTransferPage != NULL) {
        if (csChannelName != _T("")) {
            dlgTransferPage->SetChannelName(csChannelName);
            dlgTransferPage->LoadFromRegistry();
        } else {
            dlgTransferPage->ClearPage();
        }
    }
}
