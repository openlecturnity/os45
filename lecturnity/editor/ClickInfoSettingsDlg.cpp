// C:\Sandbox\lecturnity\editor\ClickInfoSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\Studio\Studio.h"
#include "ClickInfoSettingsDlg.h"
#include "MfcUtils.h"           // common/lsutl32/
#include "MiscFunctions.h"      // common/filesdk/
#include "InteractionArea.h"    // common/filesdk/


// CClickInfoSettingsDlg dialog

IMPLEMENT_DYNAMIC(CClickInfoSettingsDlg, CDialog)

CClickInfoSettingsDlg::CClickInfoSettingsDlg(CLcElementInfo *pLcElementInfo, CWnd* pParent /*=NULL*/)
: CDialog(CClickInfoSettingsDlg::IDD, pParent) {
    ASSERT(pLcElementInfo);

    m_pLcElementInfo = pLcElementInfo;
    // Further on, dialog will work with the clone
    m_pLcElementInfoCopy = m_pLcElementInfo->Copy();
}

CClickInfoSettingsDlg::~CClickInfoSettingsDlg() {
    m_acsControlTypes.RemoveAll();
    SAFE_DELETE(m_pLcElementInfoCopy);
}

void CClickInfoSettingsDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CALL_EDT_CLICK_APP_NAME, m_edtAppName);
    DDX_Control(pDX, IDC_CALL_EDT_CLICK_CONTROL_NAME, m_edtCtrlName);
    DDX_Control(pDX, IDC_CALL_CMB_CLICK_CONTROL_TYPE, m_cmbCtrlType);
    DDX_Control(pDX, IDC_CALL_EDT_X, m_edtX);
    DDX_Control(pDX, IDC_CALL_EDT_Y, m_edtY);
    DDX_Control(pDX, IDC_CALL_EDT_WIDTH, m_edtWidth);
    DDX_Control(pDX, IDC_CALL_EDT_HEIGHT, m_edtHeight);
}


BEGIN_MESSAGE_MAP(CClickInfoSettingsDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CClickInfoSettingsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CClickInfoSettingsDlg message handlers

void CClickInfoSettingsDlg::OnBnClickedOk() {
    CString csAppName, csControlName;
    m_edtAppName.GetWindowText(csAppName);
    m_pLcElementInfoCopy->SetProcessName(csAppName);

    m_edtCtrlName.GetWindowText(csControlName);
    StringManipulation::EncodeXmlSpecialCharacters(csControlName);
    m_pLcElementInfoCopy->SetName(csControlName);

    CString csSelText;
    m_cmbCtrlType.GetWindowText(csSelText);
    for (int i=0; i<m_acsControlTypes.GetCount(); i++) {
        if (m_acsControlTypes.GetAt(i).Compare(csSelText) == 0) {
            CString csControlType;
            csControlType.Format(_T("%d"), i);
            m_pLcElementInfoCopy->SetAccType(csControlType);
            break;
        }
    }
    CString csText;
    m_edtX.GetWindowText(csText);
    int x = _wtoi(csText);
    m_edtY.GetWindowText(csText);
    int y = _wtoi(csText);
    m_edtWidth.GetWindowText(csText);
    int w = _wtoi(csText);
    m_edtHeight.GetWindowText(csText);
    int h = _wtoi(csText);

    m_pLcElementInfoCopy->SetPtLocation(CPoint(x,y));
    m_pLcElementInfoCopy->SetPtSize(CPoint(w,h));

    // Be aware that will call overloaded != operator
    if (*m_pLcElementInfo != *m_pLcElementInfoCopy) {
        m_pLcElementInfo->SetElementInfo(m_pLcElementInfoCopy);
        OnOK();
    } else {
        // No changes
        OnCancel();
    }
}

BOOL CClickInfoSettingsDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    CString csTitle;
    csTitle.LoadString(IDD_DIALOG_CLICK_INFO_SETTINGS);
    SetWindowText(csTitle);
    DWORD dwIds []= {
        IDC_CALL_LBL_CLICK_APP_NAME,
        IDC_CALL_LBL_CLICK_CONTROL_NAME,
        IDC_CALL_LBL_CLICK_CONTROL_TYPE,
        IDC_CALL_LBL_POSITION,
        IDC_CALL_LBL_SIZE,
        IDC_CALL_LBL_X,
        IDC_CALL_LBL_Y,
        IDC_CALL_LBL_WIDTH,
        IDC_CALL_LBL_HEIGHT,
        IDOK,
        IDCANCEL,
        -1
    };
    MfcUtils::Localize(this, dwIds);
    CString csAppName = m_pLcElementInfoCopy->GetProcessName();
    m_edtAppName.SetWindowText(csAppName);

    CString csControlName = m_pLcElementInfoCopy->GetName();
    StringManipulation::DecodeXmlSpecialCharacters(csControlName);
    m_edtCtrlName.SetWindowText(csControlName);

    SetControlType(m_pLcElementInfoCopy->GetAccType());
    CString csText;
    csText.Format(_T("%d"), m_pLcElementInfoCopy->GetPtLocation().x);
    m_edtX.SetWindowText(csText);
    csText.Format(_T("%d"), m_pLcElementInfoCopy->GetPtLocation().y);
    m_edtY.SetWindowText(csText);
    csText.Format(_T("%d"), m_pLcElementInfoCopy->GetPtSize().x);
    m_edtWidth.SetWindowText(csText);
    csText.Format(_T("%d"), m_pLcElementInfoCopy->GetPtSize().y);
    m_edtHeight.SetWindowText(csText);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

//Private
void CClickInfoSettingsDlg::SetControlType(CString csControlType) {
    CString csComboEntry;
    CString csType;

    int iTypeId = _wtoi(csControlType);
    int iPos = 0;

    CString csCurSel = _T("");
    for (UINT nId = IDS_ROLE_SYSTEM_UNKNOWN; nId <= IDS_ROLE_SYSTEM_DIALOGSHELL; nId++) {
        csType.LoadString(nId);
        int iCurPos = 0;
        CString csComponentType = csType.Tokenize(_T("_"), iCurPos);
        m_acsControlTypes.Add(csComponentType);
        m_cmbCtrlType.AddString(csComponentType);
        if (iTypeId == iPos) {
            csCurSel = csComponentType;
        }
        iPos++;
        csType.Empty();
    }
    int iCurSel = m_cmbCtrlType.FindString(0, csCurSel);
    m_cmbCtrlType.SetCurSel(iCurSel);
}
