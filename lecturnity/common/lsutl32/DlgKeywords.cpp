// DlgKeywords.cpp : implementation file
//

#include "stdafx.h"
//#include "..\..\Studio\Resource.h"
#include "lsutl32.h"
#include "DlgKeywords.h"
#include "MfcUtils.h"
#include "XPGroupBox.h"


#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgKeywords dialog


CDlgKeywords::CDlgKeywords(CWnd* pParent): CDialog(CDlgKeywords::IDD, pParent) {
    pParent->EnableWindow(false);
    pParent->EnableWindow(true);
    m_csKeywords = _T("");
    m_gbCustomGroupBox = new CXPGroupBox();
    //{{AFX_DATA_INIT(CDlgKeywords)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

CDlgKeywords::~CDlgKeywords() {
    SAFE_DELETE(m_gbCustomGroupBox);
}

void CDlgKeywords::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgKeywords)
    DDX_Text(pDX, IDC_KEYWORDS_EDIT_TITLE, m_csTitle);
    DDX_Text(pDX, IDC_KEYWORDS_EDIT_KEYWORDS, m_csKeywords);
    DDX_Control(pDX, IDC_KEYWORDS_STATIC_GROUP, *m_gbCustomGroupBox);
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgKeywords, CDialog)
    //{{AFX_MSG_MAP(CDlgKeywords)
    ON_WM_CTLCOLOR()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgKeywords message handlers

BOOL CDlgKeywords::OnInitDialog() {
    CDialog::OnInitDialog();

    DWORD dwIds[] = {IDC_KEYWORDS_STATIC_TITLE,
        IDC_KEYWORDS_STATIC_KEYWORDS,
        IDOK,
        IDCANCEL,
        -1};

    MfcUtils::Localize(this, dwIds);

    CString csTitle;
    csTitle.LoadString(IDS_KEYWORDS_DIALOG_TITLE);
    SetWindowText(csTitle);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgKeywords::SetTitle(CString &csTitle) {
    m_csTitle = csTitle;
}

void CDlgKeywords::SetKeywords(CString &csKeywords) {
    m_csKeywords = csKeywords;
}

HBRUSH CDlgKeywords::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor) {

    HBRUSH hbr = NULL;

    if (nCtlColor == CTLCOLOR_STATIC) {
        pDC->SetBkColor(RGB(255,255, 255));
        hbr = CreateSolidBrush(RGB(255, 255, 255));
        //pDC->SetBkColor(RGB(249, 240, 255));
        //hbr = CreateSolidBrush(RGB(249, 240, 255));
    } else {
        hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);  
    }

    return hbr;
}
