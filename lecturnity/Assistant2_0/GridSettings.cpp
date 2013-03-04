// GridSettings.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#else
#include "..\Studio\Resource.h"
#endif
#include "GridSettings.h"
#include "MfcUtils.h"
#include "AssistantDoc.h"


// CGridSettings dialog

IMPLEMENT_DYNAMIC(CGridSettings, CDialog)

CGridSettings::CGridSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CGridSettings::IDD, pParent)
{
	
}

CGridSettings::~CGridSettings()
{
}

void CGridSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SPACING, m_comboSpacing);
	DDX_Control(pDX, IDC_CHECK_SNAP, m_checkSnap);
	DDX_Control(pDX, IDC_CHECK_DISPLAY_GRID, m_checkDisplay);
}


BEGIN_MESSAGE_MAP(CGridSettings, CDialog)
	ON_BN_CLICKED(IDC_CHECK_SNAP, &CGridSettings::OnBnClickedCheckSnap)
	ON_CBN_CLOSEUP(IDC_COMBO_SPACING, &CGridSettings::OnCbnCloseupComboSpacing)
	ON_BN_CLICKED(IDC_CHECK_DISPLAY_GRID, &CGridSettings::OnBnClickedCheckDisplayGrid)
	ON_BN_CLICKED(IDOK, &CGridSettings::OnOk)
END_MESSAGE_MAP()


// CGridSettings message handlers

BOOL CGridSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwIds[] = {IDC_STATIC_SNAP,
		IDC_CHECK_SNAP,
		IDC_STATIC_GRID_SETTINGS,
		IDC_STATIC_SPACING,
		IDC_STATIC_PIXELS,
		IDC_CHECK_DISPLAY_GRID,
		IDOK,
		IDCANCEL,
		-1};

	MfcUtils::Localize(this, dwIds);
	CString csTitle;
	csTitle.LoadString(IDS_GRID_TITLE);
	SetWindowText(csTitle);

	if(m_nSnap)
		m_checkSnap.SetCheck(true);
	else
		m_checkSnap.SetCheck(false);
	if(m_nDisplay)
		m_checkDisplay.SetCheck(true);
	else
		m_checkDisplay.SetCheck(false);	
	m_comboSpacing.SetCurSel((m_nSpacing/5)-1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CGridSettings::OnBnClickedCheckSnap()
{
	m_nSnap = m_checkSnap.GetCheck();
}

void CGridSettings::OnCbnCloseupComboSpacing()
{
	m_nSpacing = (m_comboSpacing.GetCurSel()+1)*5;
}

void CGridSettings::OnBnClickedCheckDisplayGrid()
{
	m_nDisplay = m_checkDisplay.GetCheck();	
}

void CGridSettings::Init(CAssistantDoc *pAssistantDoc)
{
    m_pAssistantDoc = pAssistantDoc;
	if (m_pAssistantDoc != NULL) 
		m_pAssistantDoc->GetGridSettings(m_nSnap, m_nSpacing, m_nDisplay);  
}

void CGridSettings::OnOk()
{
	// TODO: Add your control notification handler code here
	if (m_pAssistantDoc != NULL) 
		m_pAssistantDoc->SetGridSettings(m_nSnap, m_nSpacing, m_nDisplay);
	OnOK();
}
