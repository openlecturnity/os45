// FileSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pptresources.h"
#include "FileSelectDlg.h"
#include "securityutils.h"
#include "MfcUtils.h"


// CFileSelectDlg dialog

IMPLEMENT_DYNAMIC(CFileSelectDlg, CPropertyPage)

CFileSelectDlg::CFileSelectDlg()
	: CPropertyPage(CFileSelectDlg::IDD)
{

}

CFileSelectDlg::~CFileSelectDlg()
{
}

void CFileSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILENAME_SELECTION, m_edtFileName);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_btnBrowse);
}


BEGIN_MESSAGE_MAP(CFileSelectDlg, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CFileSelectDlg::OnBnClickedButtonBrowse)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CFileSelectDlg message handlers

BOOL CFileSelectDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	DWORD dwIds [] = {IDC_FILE_TITLE_LABEL,
		IDC_FILENAME_SD_LABEL,
		IDC_BUTTON_BROWSE,
					-1
					};

	MfcUtils::Localize(this, dwIds);

	CSecurityUtils su;
	CString csLecHome;
	su.GetLecturnityHome(csLecHome);
	CString csPPTName;

	if(m_csPPTName.Find(_T(".pptx")) != -1)
	{
		csPPTName = m_csPPTName.Left(m_csPPTName.GetLength() - 5);
	}
	else if(m_csPPTName.Find(_T(".ppt")) != -1)
	{
		csPPTName = m_csPPTName.Left(m_csPPTName.GetLength() - 4);
	}
	else
	{
		csPPTName = m_csPPTName;
	}
	CString csLDSDefault = csLecHome + _T("\\Source Documents\\") + /*m_csPPTName.Left(m_csPPTName.GetLength() - 3)*/ csPPTName + _T(".lsd");

	m_edtFileName.SetWindowText(csLDSDefault);

	CString csButtText;
	csButtText.LoadString(IDC_BUTTON_BROWSE);
	m_btnBrowse.SetWindowText(csButtText);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFileSelectDlg::Init(CString pPPTName, WCHAR *pLSDName)
{
	int namePos = pPPTName.ReverseFind(_T('\\'));
	if(namePos >= 0)
	{
		m_csPPTName = pPPTName.Right(pPPTName.GetLength() - namePos - 1);
	}
	else
	{
		m_csPPTName = pPPTName;
	}
	m_pLSDName = pLSDName;
}

BOOL CFileSelectDlg::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class

	CPropertySheet *parent = (CPropertySheet*)GetParent();
	parent->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

	CString csFinishStr;
	csFinishStr.LoadString(IDS_FINISH_STRING);

	CWnd* finishButton = parent->GetDlgItem(ID_WIZFINISH);
	finishButton->SetWindowText(csFinishStr);

	return CPropertyPage::OnSetActive();
}

void CFileSelectDlg::OnBnClickedButtonBrowse()
{
	// TODO: Add your control notification handler code here
	CString csEditText;
	m_edtFileName.GetWindowText(csEditText);

	int pos = csEditText.ReverseFind(_T('\\'));
	CString csDefName = _T("");
	if(pos != -1)
	{
		csDefName = csEditText.Right(csEditText.GetLength() - pos - 1);
		csDefName.Left(csDefName.GetLength() - 4);
	}
	static TCHAR szFilter[] = _T("LECTURNITY Source Documents (*.lsd)|*.lsd|All Files (*.*)|*.*||");
    CFileDialog fDlg(false, _T(".lsd"), /*NULL*/csDefName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

	CString csLsdName;
	if(fDlg.DoModal() == IDOK)
	{
		m_edtFileName.SetWindowText(fDlg.GetPathName());
	}


}

BOOL CFileSelectDlg::OnWizardFinish()
{
	// TODO: Add your specialized code here and/or call the base class

	CString csLsdName;
	m_edtFileName.GetWindowText(csLsdName);
	CFile file;

	if(file.Open(csLsdName, CFile::modeRead))
	{
		file.Close();
		CString msg1;
		msg1.LoadString(ERR_FILE);
		CString msg2;
		msg2.LoadString(ERR_EXISTS);
		CString csCaption;
		csCaption.LoadString(IDS_DIALOG_TITLE);

		CString msg = msg1 + csLsdName + _T("\n") + msg2;

		if(MessageBox(msg, csCaption, MB_YESNO | MB_ICONINFORMATION) == IDNO)
		{
			return 0;
		}
	}
	wcscpy(m_pLSDName, csLsdName.GetBuffer());
	return CPropertyPage::OnWizardFinish();
}

HBRUSH CFileSelectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if(pWnd->GetDlgCtrlID() == IDC_FILE_TITLE_LABEL)
	{
		pDC->SetBkColor(RGB(0xFF, 0xFF, 0xFF));
		hbr = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
	}
	else
		hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
