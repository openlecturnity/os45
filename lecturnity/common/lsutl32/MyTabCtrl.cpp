// MyTabCtrl.cpp: Implementierung der Klasse CMyTabCtrl.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyTabCtrl.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CMyTabCtrl::CMyTabCtrl() : CTabCtrl()
{
   // Bla
}

CMyTabCtrl::~CMyTabCtrl()
{
}

BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CCustomPropPage)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HBRUSH CMyTabCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
   MessageBox(_T("Hallalalala"));
   return CTabCtrl::OnCtlColor(pDC, pWnd, nCtlColor);
}

