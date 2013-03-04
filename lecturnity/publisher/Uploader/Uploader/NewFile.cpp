// NewFile.cpp : implementation file
//

#include "stdafx.h"
#include "MfcUtils.h"
#include "Uploader.h"
#include "NewFile.h"


// CNewFile dialog

IMPLEMENT_DYNAMIC(CNewFile, CDialog)

CNewFile::CNewFile(CWnd* pParent /*=NULL*/): CDialog(CNewFile::IDD, pParent) {
    m_bFileType = (bool) (!TYPE_MP4);
    m_csNewFileName = _T("");
}

CNewFile::CNewFile(Filetype eType, CWnd* pParent /*=NULL*/): CDialog(CNewFile::IDD, pParent) {
    m_bFileType = (bool) !eType;
    m_csNewFileName = _T("");
}

CNewFile::~CNewFile() {
}

void CNewFile::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ST_LZP, m_csLzp);
    DDX_Control(pDX, IDC_ST_MP4, m_csMp4);
    DDX_Control(pDX, IDC_EDIT_FILE, m_edtFile);
    DDX_Control(pDX, IDOK, m_btnOK);
}


BEGIN_MESSAGE_MAP(CNewFile, CDialog)
    ON_EN_CHANGE(IDC_EDIT_FILE, &CNewFile::OnEnChangeEdit1)
    ON_BN_CLICKED(IDOK, &CNewFile::OnBnClickedOk)
END_MESSAGE_MAP()


// CNewFile message handlers

BOOL CNewFile::OnInitDialog() {
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    SetIcon(AfxGetApp()->LoadIcon(IDI_UPLOADER), true);

    DWORD dwID[] = {
        IDL_NF1,
        IDL_NF2,
        IDOK,
        IDCANCEL,
        -1
    };

    MfcUtils::Localize(this, dwID);
    CString csCaption;
    csCaption.LoadString(IDD_NEWMP4);
    SetWindowText(csCaption);
    m_csMp4.ShowWindow(m_bFileType);
    m_csLzp.ShowWindow(!m_bFileType);

    m_edtFile.SetFocus();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewFile::OnEnChangeEdit1() {
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    CString csText;
    m_edtFile.GetWindowText(csText);

    m_btnOK.EnableWindow(!csText.IsEmpty());
}

CString CNewFile::GetNewFileName() {
    CString csExt = _T(".mp4");

    if (m_bFileType == false) {
        csExt = _T(".lzp");
    }
    return (m_csNewFileName + csExt);
}
void CNewFile::OnBnClickedOk() {
    // TODO: Add your control notification handler code here
    m_edtFile.GetWindowText(m_csNewFileName);
    OnOK();
}
