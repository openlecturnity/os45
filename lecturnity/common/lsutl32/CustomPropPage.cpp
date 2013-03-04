// CustomPropPage.cpp: Implementierung der Klasse CCustomPropPage.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "CustomPropPage.h"


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////


CCustomPropPage::CCustomPropPage(UINT nID, CWnd *pWndParent /*=NULL*/) :
   CDialog(nID, pWndParent)
{
	//{{AFX_DATA_INIT(CCustomPropPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
   m_bBrushInited = false;
   m_nResID = nID;
}

CCustomPropPage::~CCustomPropPage()
{

}

BEGIN_MESSAGE_MAP(CCustomPropPage, CDialog)
	//{{AFX_MSG_MAP(CCustomPropPage)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCustomPropPage::Create(CWnd *pWndParent)
{
   return CDialog::Create(m_nResID, pWndParent);
}


HBRUSH CCustomPropPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
   /*
   if (!m_bBrushInited)
   {
      m_bgBrush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
      m_bBrushInited = true;
   }
   HBRUSH hbr = (HBRUSH) m_bgBrush.m_hObject;
   pDC->SetBkMode(TRANSPARENT);

	return hbr;
   */
   return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL CCustomPropPage::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen

   //return TRUE;
   return CDialog::OnEraseBkgnd(pDC);
}
