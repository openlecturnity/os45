// NewsAvailableDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lsupdt32.h"
#include "NewsAvailableDialog.h"

#include "MfcUtils.h"     // MftUtils

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CNewsAvailableDialog 


CNewsAvailableDialog::CNewsAvailableDialog(int nAnnouncements, CString csAnnTitle, CWnd* pParent /*=NULL*/)
	: CDialog(CNewsAvailableDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewsAvailableDialog)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_nAnnouncements = nAnnouncements;
   m_csAnnouncementTitle = csAnnTitle;
}


void CNewsAvailableDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewsAvailableDialog)
	DDX_Control(pDX, IDC_STATIC_NEWS, m_lbAnnouncements);
	DDX_Control(pDX, IDC_DISABLENEWS, m_cbDisableAnnouncements);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewsAvailableDialog, CDialog)
	//{{AFX_MSG_MAP(CNewsAvailableDialog)
	ON_BN_CLICKED(IDC_DISABLENEWS, OnChangeCheckbox)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDYES, &CNewsAvailableDialog::OnBnClickedYes)
   ON_BN_CLICKED(IDNO, &CNewsAvailableDialog::OnBnClickedNo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CNewsAvailableDialog 

BOOL CNewsAvailableDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   DWORD dwIds[] = {IDC_STATIC_NEWS,
                    IDC_DISABLENEWS,
                    IDYES,
                    IDNO,
                    -1};

   MfcUtils::Localize(this, dwIds);

   CString csTitle;
   csTitle.LoadString(IDD_NEWSAVAILABLE);
   SetWindowText(csTitle);

	// TODO: Zusätzliche Initialisierung hier einfügen
   CString csLabel;
   if (m_nAnnouncements > 1) // more than one announcement
      csLabel.Format(IDS_MSG_NEWS_MORE, m_nAnnouncements);
   else
      csLabel.Format(IDS_MSG_NEWS_ONE, m_csAnnouncementTitle);
   m_lbAnnouncements.SetWindowText(csLabel);

   m_cbDisableAnnouncements.SetCheck(BST_UNCHECKED);
   m_bIsDisableAnnouncementsChecked = false;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CNewsAvailableDialog::OnChangeCheckbox() 
{
	m_bIsDisableAnnouncementsChecked = (m_cbDisableAnnouncements.GetCheck() == BST_CHECKED);
}

void CNewsAvailableDialog::OnBnClickedYes()
{
   CDialog::OnOK();
}

void CNewsAvailableDialog::OnBnClickedNo()
{
   CDialog::OnCancel();
}
