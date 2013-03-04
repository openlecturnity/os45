// TransferProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Uploader.h"
#include "TransferProgressDlg.h"
#include "TransferingContent.h"
#include "MfcUtils.h"


// CTransferProgressDlg dialog

IMPLEMENT_DYNAMIC(CTransferProgressDlg, CDialog)

CTransferProgressDlg::CTransferProgressDlg(double* pdProgress, CString csFrom, CString csTo, CWnd* pParent /*=NULL*/)
: CDialog(CTransferProgressDlg::IDD, pParent) {
    m_PcProgress.SetRange(0,100);
    m_csFrom = csFrom;
    m_csTo = csTo;
    m_pdProgress = pdProgress;
}

CTransferProgressDlg::~CTransferProgressDlg() {
}

void CTransferProgressDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS, m_PcProgress);
    DDX_Control(pDX, IDL_FROM, m_lblFrom);
    DDX_Control(pDX, IDL_TO, m_lblTo);
}


BEGIN_MESSAGE_MAP(CTransferProgressDlg, CDialog)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CTransferProgressDlg message handlers

BOOL CTransferProgressDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    DWORD dwID[] = {
        IDOK,
        IDCANCEL,
        -1 
    };
    MfcUtils::Localize(this, dwID);

    SetIcon(AfxGetApp()->LoadIcon(IDI_UPLOADER), TRUE);

    m_lblFrom.SetWindowText(m_csFrom);
    m_lblTo.SetWindowText(m_csTo);
    m_PcProgress.SetPos((int)(*m_pdProgress * 100.0));
    SetTimer(1,500,NULL);
    CTransferingContent& rTContent = CTransferingContent::GetInstance();
    rTContent.SetParentHwnd(GetSafeHwnd());
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


void CTransferProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default

    //set progress control position
    if (*m_pdProgress >= 1) {
        KillTimer(1);
        EndDialog(IDOK);
    }
    CTransferingContent& rTContent = CTransferingContent::GetInstance();
    if (!rTContent.GetErrStr().IsEmpty()) {
        KillTimer(1);
        EndDialog(IDCANCEL);
    }
    m_PcProgress.SetPos((int)(*m_pdProgress * 100));
    CDialog::OnTimer(nIDEvent);
}

void CTransferProgressDlg::SetFromText(CString csFrom) {
    m_csFrom = csFrom;
}

void CTransferProgressDlg::SetToText(CString csTo) {
    m_csTo = csTo;
}
