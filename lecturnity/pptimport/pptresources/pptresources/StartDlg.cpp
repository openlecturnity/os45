// StartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pptresources.h"
#include "StartDlg.h"
#include "MfcUtils.h"


// CStartDlg dialog

IMPLEMENT_DYNAMIC(CStartDlg, CPropertyPage)

CStartDlg::CStartDlg()
	: CPropertyPage(CStartDlg::IDD)
{

}

CStartDlg::~CStartDlg()
{
}

void CStartDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//	DDX_Control(pDX, 65535, m_rbtAll);
	DDX_Control(pDX, IDC_SLIDE_SELECTION, m_edtRange);
}


BEGIN_MESSAGE_MAP(CStartDlg, CPropertyPage)
ON_BN_CLICKED(IDC_RADIO_ALL, &CStartDlg::OnBnClickedRadioAll)
ON_BN_CLICKED(IDC_RADIO_CURRENT, &CStartDlg::OnBnClickedRadioCurrent)
ON_BN_CLICKED(IDC_RADIO_SLIDES, &CStartDlg::OnBnClickedRadioSlides)
ON_WM_CTLCOLOR()
ON_EN_UPDATE(IDC_SLIDE_SELECTION, &CStartDlg::OnEnUpdateSlideSelection)
END_MESSAGE_MAP()


// CStartDlg message handlers

void CStartDlg::InitWiz(void)
{
}

void CStartDlg::Init(int iSlideCount, int iCurrentSlide, WCHAR *wszSlideSelection)
{
	m_iSlideCount = iSlideCount;
	m_iCurrentSlide = iCurrentSlide;
	m_pSlideSelection = wszSlideSelection;

	int x = 0;
}

BOOL CStartDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	DWORD dwIds [] = {IDC_RADIO_ALL,
					IDC_RADIO_CURRENT,
					IDC_RADIO_SLIDES,
					IDC_TITLE_LABEL,
					IDC_USAGE_LABEL,
					-1
					};

	MfcUtils::Localize(this, dwIds);
	aSelected[SELECT_ALL_SLIDES]	= 1;
	aSelected[SELECT_CURRENT_SLIDE]	= 0;
	aSelected[SELECT_SLIDE_RANGE]	= 0;

	CheckDlgButton(IDC_RADIO_ALL, BST_CHECKED);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CStartDlg::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class

	CPropertySheet *parent = (CPropertySheet*)GetParent();
	parent->SetWizardButtons(/*PSWIZB_BACK |*/ PSWIZB_NEXT);

	CWnd* backButton = parent->GetDlgItem(ID_WIZBACK);
	backButton->EnableWindow(false);

	CString csNextstr;

	csNextstr.LoadString(ID_WIZNEXT);
	CWnd* nextButton = parent->GetDlgItem(ID_WIZNEXT);
	nextButton->SetWindowText(csNextstr);

	return CPropertyPage::OnSetActive();
}

void CStartDlg::OnBnClickedRadioAll()
{
	// TODO: Add your control notification handler code here
	aSelected[SELECT_ALL_SLIDES]	= 1;
	aSelected[SELECT_CURRENT_SLIDE]	= 0;
	aSelected[SELECT_SLIDE_RANGE]	= 0;
}

void CStartDlg::OnBnClickedRadioCurrent()
{
	// TODO: Add your control notification handler code here
	aSelected[SELECT_ALL_SLIDES]	= 0;
	aSelected[SELECT_CURRENT_SLIDE]	= 1;
	aSelected[SELECT_SLIDE_RANGE]	= 0;
}

void CStartDlg::OnBnClickedRadioSlides()
{
	// TODO: Add your control notification handler code here
	aSelected[SELECT_ALL_SLIDES]	= 0;
	aSelected[SELECT_CURRENT_SLIDE]	= 0;
	aSelected[SELECT_SLIDE_RANGE]	= 1;
	::SetFocus(m_edtRange.m_hWnd);
}

LRESULT CStartDlg::OnWizardNext()
{
	// TODO: Add your specialized code here and/or call the base class
	CString csSlideSel;
	
	if(aSelected[SELECT_ALL_SLIDES])
	{
		csSlideSel.Format(_T("1-%d"),m_iSlideCount);
	}

	if(aSelected[SELECT_CURRENT_SLIDE])
	{
		csSlideSel.Format(_T("%d"), m_iCurrentSlide);
	}
	
	if(aSelected[SELECT_SLIDE_RANGE])
	{
		CString csRange;
		m_edtRange.GetWindowText(csRange);
		if(!VerifyRange(csRange))
		{
			CString csWarnMessage;
			csWarnMessage.LoadString(ERR_SELECT_SLIDE_RANGE);
			MessageBox(csWarnMessage);
			return -1;
		}
		csSlideSel = csRange;

	}
	wcscpy(m_pSlideSelection, csSlideSel.GetBuffer());
	
	//return -1;

	return CPropertyPage::OnWizardNext();
}

bool CStartDlg::VerifyRange(CString csRange)
{
	CString csCorrectValues = _T("0123456789; -");

	if(csRange == _T(""))
		return false;

	for(int i = 0; i < csRange.GetLength(); i++)
	{
		if(csCorrectValues.Find(csRange.GetAt(i)) == (-1))
		{
			return false;
		}
	}

	CString csToken;
	int iCurrPos = 0;

	csToken = csRange.Tokenize(_T("; -"), iCurrPos);
	
	while(csToken != _T(""))
	{
		int val = _wtoi(csToken);
		if((val < 1) || (val > m_iSlideCount))
			return false;
		csToken = csRange.Tokenize(_T("; -"), iCurrPos);
	}

	return true;
}

HBRUSH CStartDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if(pWnd->GetDlgCtrlID() == IDC_TITLE_LABEL)
	{
		pDC->SetBkColor(RGB(0xFF, 0xFF, 0xFF));
		hbr = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
	}
	else
		hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CStartDlg::OnEnUpdateSlideSelection()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
	if(aSelected[SELECT_SLIDE_RANGE] != 1)
	{
		CButton *rbtSelSlide = (CButton*)GetDlgItem(IDC_RADIO_SLIDES);
		rbtSelSlide->SetCheck(BST_CHECKED);
		CButton *rbtSelCurrent = (CButton*)GetDlgItem(IDC_RADIO_CURRENT);
		rbtSelCurrent->SetCheck(BST_UNCHECKED);
		CButton *rbtSelAll = (CButton*)GetDlgItem(IDC_RADIO_ALL);
		rbtSelAll->SetCheck(BST_UNCHECKED);
		aSelected[SELECT_ALL_SLIDES]	= 0;
		aSelected[SELECT_CURRENT_SLIDE]	= 0;
		aSelected[SELECT_SLIDE_RANGE]	= 1;
	}
}
