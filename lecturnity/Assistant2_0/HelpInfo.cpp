// HelpInfo.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "assistant.h"
#include "HelpInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CHelpInfo 

#include "MfcUtils.h"

CHelpInfo::CHelpInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHelpInfo)
	//}}AFX_DATA_INIT
}


void CHelpInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelpInfo)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelpInfo, CDialog)
	//{{AFX_MSG_MAP(CHelpInfo)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CHelpInfo 

int CHelpInfo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
   m_brushYellow.CreateSolidBrush(RGB(255, 255, 226));
	return 0;
}

BOOL CHelpInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   DWORD dwIds[] = {IDC_HELP_TEXT,
                    -1};

   MfcUtils::Localize(this, dwIds);

   CString csTitle;
   csTitle.LoadString(IDD_QUICK_INFO);
   SetWindowText(csTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

HBRUSH CHelpInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
   HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   
   switch (nCtlColor)
   {
      // process my edit controls by ID.
   case CTLCOLOR_STATIC:
      {
         switch (pWnd->GetDlgCtrlID())
         {
         case IDC_HELP_TEXT:  
            pDC->SetBkColor(RGB(255, 255, 226));
            hbr = (HBRUSH) m_brushYellow;
            break;
         }
      }
   case CTLCOLOR_DLG: 
      pDC->SetBkColor(RGB(255, 255, 226));
      hbr = (HBRUSH) m_brushYellow;
      break;
   }
   return hbr;
}
