// ConvWizard.cpp : implementation file
//

#include "stdafx.h"
#include "pptresources.h"
#include "ConvWizard.h"
#include "MfcUtils.h"


// CConvWizard

IMPLEMENT_DYNAMIC(CConvWizard, CPropertySheet)

CConvWizard::CConvWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	, m_bIsActive(false)
{
	InitWiz();
}

CConvWizard::CConvWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	InitWiz();
}

CConvWizard::~CConvWizard()
{
	m_bIsActive = false;
	if(m_pStartDlg)
		delete(m_pStartDlg);
	if(m_pFileSelectDlg)
		delete(m_pFileSelectDlg);
	if(m_pcsDialogTitle)
		delete(m_pcsDialogTitle);
}


BEGIN_MESSAGE_MAP(CConvWizard, CPropertySheet)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CConvWizard message handlers

void CConvWizard::InitWiz(void)
{
	m_psh.dwFlags |= PSH_WIZARD_LITE;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_pStartDlg			= new CStartDlg();
	m_pFileSelectDlg	= new CFileSelectDlg();

	m_pcsDialogTitle	= new CString();
	m_pcsDialogTitle->LoadString(IDS_DIALOG_TITLE);

	LPTSTR pstTitle = m_pcsDialogTitle->GetBuffer(m_pcsDialogTitle->GetLength());

	m_pStartDlg->m_psp.dwFlags		&= ~PSH_HASHELP;
	m_pStartDlg->m_psp.dwFlags		|= PSP_USETITLE;
	m_pStartDlg->m_psp.pszTitle		= pstTitle;

	m_pFileSelectDlg->m_psp.dwFlags &= ~PSH_HASHELP;
	m_pFileSelectDlg->m_psp.dwFlags	|= PSP_USETITLE;
	m_pFileSelectDlg->m_psp.pszTitle = pstTitle;

	AddPage(m_pStartDlg);
	AddPage(m_pFileSelectDlg);

	SetWizardMode();
	m_bProgress = false;
	m_bIsActive = true;
}

void CConvWizard::Init(CString wszPPTName, int iSlideCount, int iCurrentSlide, WCHAR *wszSlideSelection, WCHAR *wszLSDName, HWND *pHWnd)
{
	m_pStartDlg->Init(iSlideCount, iCurrentSlide, wszSlideSelection);
	m_pFileSelectDlg->Init(wszPPTName, wszLSDName);
}

void CConvWizard::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_bIsActive = false;
	CPropertySheet::OnClose();
}

void CConvWizard::OnDestroy()
{
	CPropertySheet::OnDestroy();

	// TODO: Add your message handler code here
	m_bIsActive = false;
}

BOOL CConvWizard::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// TODO:  Add your specialized code here
	SetIcon(m_hIcon, false);
	DWORD dwIds [] = {ID_WIZBACK,
					ID_WIZNEXT,
					IDCANCEL,
					-1
					};
	MfcUtils::Localize(this, dwIds);

	return bResult;
}

BOOL CConvWizard::ContinueModal()
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertySheet::ContinueModal();
}
