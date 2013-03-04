// Warning16Bit.cpp: Implementierungsdatei
//

#include "stdafx.h"

#include "Warning16Bit.h"

#include "MfcUtils.h" // MfcUtils::Localize

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CWarning16BitA 


CWarning16BitA::CWarning16BitA(CWnd* pParent /*=NULL*/)
	: CDialog(CWarning16BitA::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWarning16BitA)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CWarning16BitA::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWarning16BitA)
	DDX_Control(pDX, IDC_DO_NOT_DISPLAY_AGAIN, m_cbbDoNotDisplayAgain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWarning16BitA, CDialog)
	//{{AFX_MSG_MAP(CWarning16BitA)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CWarning16BitA 

BOOL CWarning16BitA::OnInitDialog() 
{
	CDialog::OnInitDialog();

   CString csCap;
   csCap.LoadString(IDS_WARNING);
   SetWindowText(csCap);

   DWORD dwIds[] = {IDC_WARNING_16BIT,
                    IDC_DO_NOT_DISPLAY_AGAIN,
                    IDYES,
                    IDNO,
                    -1};
   MfcUtils::Localize(this, dwIds);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CWarning16BitA::CheckCheckBox()
{
   if (m_cbbDoNotDisplayAgain.GetCheck() == BST_CHECKED)
   {
      AfxGetApp()->WriteProfileInt(_T("Settings"), _T("DoNotDisplay16BitWarning"), 1);
   }
}

void CWarning16BitA::OnYes() 
{
   CheckCheckBox();
   EndDialog(IDYES);	
}

void CWarning16BitA::OnNo() 
{
   CheckCheckBox();
   EndDialog(IDNO);
}
