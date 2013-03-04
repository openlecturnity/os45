// MultiBitmapCtrl.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "cdStart_doIt.h"
#include "MultiBitmapCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiBitmapCtrl

CMultiBitmapCtrl::CMultiBitmapCtrl()
{
   m_imageNr = 0;
}

CMultiBitmapCtrl::~CMultiBitmapCtrl()
{
}


BEGIN_MESSAGE_MAP(CMultiBitmapCtrl, CWnd)
	//{{AFX_MSG_MAP(CMultiBitmapCtrl)
	ON_WM_PAINT()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CMultiBitmapCtrl 

BOOL CMultiBitmapCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, int nPicCount, UINT nBitmapID, CCreateContext* pContext)
//BOOL CMultiBitmapCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	
	BOOL res = CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID, pContext);

   int cx = rect.right - rect.left;
   int cy = rect.bottom - rect.top;

   if (FALSE != res)
   {
      m_imageList.Create(cx, cy, ILC_COLOR24, 1, nPicCount);
      CBitmap bitmap;
      bitmap.LoadBitmap(nBitmapID);
      m_imageList.Add(&bitmap, RGB(0, 255, 0));
      bitmap.DeleteObject();
   }

   return res;
}

void CMultiBitmapCtrl::SetImageNr(int nr)
{
   m_imageNr = nr;
   RedrawWindow();
}

void CMultiBitmapCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen
   m_imageList.Draw(&dc, m_imageNr, CPoint(0, 0), ILD_NORMAL);
	
	// Kein Aufruf von CWnd::OnPaint() für Zeichnungsnachrichten
}

void CMultiBitmapCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	

	CWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
