// ManualConfigurationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "lsutl32.h"
#include "ManualConfigurationDialog.h"
#include "ManualConfiguration.h"
#include "MfcUtils.h"

// CManualConfigurationDialog dialog

// static
UINT CManualConfigurationDialog::ShowItModal(CManualConfiguration* pManualConfiguration) {
    //
    // This method is necessary to create with AFX_MANAGE_STATE
    // the right context to find the right (local) resources.
    //
    // It is especially important that creation and DoModal are
    // in the same context/method.
    //
    if (!pManualConfiguration) {
        ASSERT(false); // Please provide valid input
        return IDABORT;
    }
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CManualConfigurationDialog dlg(pManualConfiguration);
    return dlg.DoModal();
}

IMPLEMENT_DYNAMIC(CManualConfigurationDialog, CDialog)

CManualConfigurationDialog::CManualConfigurationDialog(CManualConfiguration* pManualConfiguration)
: CDialog(CManualConfigurationDialog::IDD) {
    m_pManualConfiguration = pManualConfiguration;
}

CManualConfigurationDialog::~CManualConfigurationDialog() {
}

void CManualConfigurationDialog::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_MC_SLIDES_NO, m_ceSlidesNumber);
    DDX_Control(pDX, IDC_SPIN_MC_SLIDES_NO, m_sbSlidesNumber);
    DDX_Control(pDX, IDC_STATIC_MC_SLIDES_NO, m_stSlidesNumber);
    DDX_Control(pDX, IDC_CHECK_MC_SHOW_HEADER, m_chkHeader);
    DDX_Control(pDX, IDC_CHECK_MC_SHOW_FOOTER, m_chkFooter);
    DDX_Control(pDX, IDC_CHECK_MC_SHOW_COVER_SHEET, m_chkCoverSheet);
    DDX_Control(pDX, IDC_CHECK_MC_SHOW_TABLE_OF_CONTENT, m_chkTableOfContent);
}

BEGIN_MESSAGE_MAP(CManualConfigurationDialog, CDialog)
    ON_EN_UPDATE(IDC_EDIT_MC_SLIDES_NO, &CManualConfigurationDialog::OnUpdateEditSlidesNumber)
END_MESSAGE_MAP()

// CManualConfigurationDialog message handlers

BOOL CManualConfigurationDialog::OnInitDialog() {
    CDialog::OnInitDialog();

    CString csCaption;
    csCaption.LoadString(IDS_MANUAL_CONFIGURATION);
    SetWindowText(csCaption);

    DWORD dwIds[] = {IDC_STATIC_MC_SLIDES_NO,
        IDC_CHECK_MC_SHOW_HEADER, 
        IDC_CHECK_MC_SHOW_FOOTER, 
        IDC_CHECK_MC_SHOW_COVER_SHEET, 
        IDC_CHECK_MC_SHOW_TABLE_OF_CONTENT, 
        IDOK,
        IDCANCEL,
        -1};

    MfcUtils::Localize(this, dwIds);

    m_sbSlidesNumber.SetRange(1, 3);
    m_sbSlidesNumber.SetBuddy(&m_ceSlidesNumber);
    m_sbSlidesNumber.SetPos(m_pManualConfiguration->SlidesNumberPerPage);

    m_ceSlidesNumber.LimitText(1);

    m_chkHeader.SetCheck(m_pManualConfiguration->ShowHeader);
    m_chkFooter.SetCheck(m_pManualConfiguration->ShowFooter);
    m_chkCoverSheet.SetCheck(m_pManualConfiguration->ShowCoverSheet);
    m_chkTableOfContent.SetCheck(m_pManualConfiguration->ShowTableOfContent);

    return TRUE;
}

void CManualConfigurationDialog::OnUpdateEditSlidesNumber() {
    CString csText;
    m_ceSlidesNumber.GetWindowText(csText);
    int nVal = _ttoi(csText);
    if (nVal > 3 || nVal < 1) {
        nVal = nVal > 3 ? 3: 1;
        csText.Format(_T("%d"), nVal);
        m_ceSlidesNumber.SetWindowText(csText);
    }
    int iLen = csText.GetLength();
    m_ceSlidesNumber.SetSel(csText.GetLength(), -1);
}

void CManualConfigurationDialog::OnOK() {
    m_pManualConfiguration->SlidesNumberPerPage = m_sbSlidesNumber.GetPos();
    m_pManualConfiguration->ShowHeader = m_chkHeader.GetCheck() == 0 ? false: true;
    m_pManualConfiguration->ShowFooter = m_chkFooter.GetCheck() == 0 ? false: true;
    m_pManualConfiguration->ShowCoverSheet = m_chkCoverSheet.GetCheck() == 0 ? false: true;
    m_pManualConfiguration->ShowTableOfContent = m_chkTableOfContent.GetCheck() == 0 ? false: true;

    CDialog::OnOK();
}