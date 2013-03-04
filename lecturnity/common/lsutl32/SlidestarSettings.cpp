// DlgKeywords.cpp : implementation file
//

#include "stdafx.h"
#include "SlidestarSettings.h"
#include "MfcUtils.h"
#include "Crypto.h"

#include "lutils.h"     // lutils

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlidestarSettings dialog


BEGIN_MESSAGE_MAP(CSlidestarSettings, CDialog)
	//{{AFX_MSG_MAP(CSlidestarSettings)
    ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, OnOK)
END_MESSAGE_MAP()


CSlidestarSettings::CSlidestarSettings(CWnd* pParent) : CDialog(CSlidestarSettings::IDD, pParent) {
    //{{AFX_DATA_INIT(CSlidestarSettings)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_bDefaultProfile = true;
    m_hbrBg = NULL;
}

CSlidestarSettings::CSlidestarSettings(SlidestarTransferSettings sts, CWnd* pParent) : CDialog(CSlidestarSettings::IDD, pParent) {
    m_bDefaultProfile = false;
    m_sts = sts;
    m_hbrBg = NULL;
}

CSlidestarSettings::~CSlidestarSettings() {
    if (m_hbrBg)
        ::DeleteObject(m_hbrBg);
}

void CSlidestarSettings::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_EDIT_HOSTNAME, m_edtHostName);
    DDX_Control(pDX, IDC_EDIT_USERNAME, m_edtUserName);
    DDX_Control(pDX, IDC_EDIT_PASSWORD, m_edtPassword);
}


BOOL CSlidestarSettings::OnInitDialog() { 
    CDialog::OnInitDialog();

    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

    DWORD dwIds[] = {
        IDC_HOSTNAME,
        IDC_USERNAME,
        IDC_PASSWORD,
        IDOK,
        IDCANCEL,
        -1
    };

    MfcUtils::Localize(this, dwIds);

    CString csTitle;
    csTitle.LoadString(IDS_SLIDESTAR_DIALOG);
    SetWindowText(csTitle);

    m_hbrBg = ::CreateSolidBrush(RGB(255, 255, 255));

    if(m_bDefaultProfile)
        ReadDefaultSettings();
    else
        SetDialogValues();

    return TRUE; 
}

HBRUSH CSlidestarSettings::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if (nCtlColor == CTLCOLOR_BTN ||
        nCtlColor == CTLCOLOR_DLG  ||
        nCtlColor == CTLCOLOR_STATIC || true) {
            SetBkColor(pDC->GetSafeHdc(), RGB(255, 255, 255));
            SetBkMode(pDC->GetSafeHdc(), OPAQUE);
            return m_hbrBg;
    }

    return hbr;
}

void CSlidestarSettings::OnOK() {

    if(m_bDefaultProfile)
        WriteDefaultSettings();
    else {
        m_edtHostName.GetWindowText(m_sts.csServer);
        m_edtUserName.GetWindowText(m_sts.csUserName);
        m_edtPassword.GetWindowText(m_sts.csPassword);
    }
    CDialog::OnOK();
}

HRESULT CSlidestarSettings::ReadDefaultSettings() {
    HRESULT hr = S_OK;

    bool bSuccess = true;

    _TCHAR tszRegistryEntry[MAX_PATH];
    unsigned long lLength = MAX_PATH;

    // Host name
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Slidestar_Default"), 
        _T("HostName"), tszRegistryEntry, &lLength);
    if (bSuccess)
        m_edtHostName.SetWindowText(tszRegistryEntry);
    else
        m_edtHostName.SetWindowText(_T(""));

    // User name
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Slidestar_Default"), 
        _T("UserName"), tszRegistryEntry, &lLength);
    if (bSuccess)
        m_edtUserName.SetWindowText(tszRegistryEntry);
    else
        m_edtUserName.SetWindowText(_T(""));
    
    // Password
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Slidestar_Default"), 
        _T("Password"), tszRegistryEntry, &lLength);
    if (bSuccess) {
        CCrypto crypt;
        CString csDecodedPassword = crypt.Decode(tszRegistryEntry);
        m_edtPassword.SetWindowText(csDecodedPassword);
    } else
        m_edtPassword.SetWindowText(_T(""));
    
    return hr;
}

HRESULT CSlidestarSettings::WriteDefaultSettings() {
    HRESULT hr = S_OK;

    bool bSuccess = true;
    
    CString csHostName;
    m_edtHostName.GetWindowText(csHostName);
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Slidestar_Default"),
        _T("HostName"), csHostName);
    
    CString csUserName;
    m_edtUserName.GetWindowText(csUserName);
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Slidestar_Default"),
        _T("UserName"), csUserName);
    
    CString csPassword;
    m_edtPassword.GetWindowText(csPassword);
    CCrypto crypt;
    CString csEncodedPassword = crypt.Encode(csPassword);
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\Slidestar_Default"),
        _T("Password"), csEncodedPassword);
    
    return hr;
}

void CSlidestarSettings::SetDialogValues(void) {
    m_edtHostName.SetWindowText(m_sts.csServer);
    m_edtUserName.SetWindowText(m_sts.csUserName);
    m_edtPassword.SetWindowText(m_sts.csPassword);
}
