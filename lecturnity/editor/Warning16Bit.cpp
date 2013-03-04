// Warning16Bit.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "Warning16Bit.h"

#include "MfcUtils.h" // MfcUtils::Localize

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CWarning16Bit 


CWarning16Bit::CWarning16Bit(CWnd* pParent /*=NULL*/)
	: CDialog(CWarning16Bit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWarning16Bit)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CWarning16Bit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWarning16Bit)
	DDX_Control(pDX, IDC_DO_NOT_DISPLAY_AGAIN_E, m_cbbDoNotDisplayAgain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWarning16Bit, CDialog)
	//{{AFX_MSG_MAP(CWarning16Bit)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CWarning16Bit 

BOOL CWarning16Bit::OnInitDialog() 
{
	CDialog::OnInitDialog();

   CString csCap;
   csCap.LoadString(IDS_WARNING_E);
   SetWindowText(csCap);

   DWORD dwIds[] = {IDC_WARNING_16BIT_E,
                    IDC_DO_NOT_DISPLAY_AGAIN_E,
                    IDYES,
                    IDNO,
                    -1};
   MfcUtils::Localize(this, dwIds);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CWarning16Bit::CheckCheckBox()
{
   if (m_cbbDoNotDisplayAgain.GetCheck() == BST_CHECKED)
   {
      AfxGetApp()->WriteProfileInt(_T("Settings"), _T("DoNotDisplay16BitWarning"), 1);
   }
}

void CWarning16Bit::OnYes() 
{
   CheckCheckBox();
   EndDialog(IDYES);	
}

void CWarning16Bit::OnNo() 
{
   CheckCheckBox();
   EndDialog(IDNO);
}
