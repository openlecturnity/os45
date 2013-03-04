// ExtHeaderCtrl.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "ExtHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtHeaderCtrl

CExtHeaderCtrl::CExtHeaderCtrl()
{
	m_bBeginTrack = false;
}

CExtHeaderCtrl::~CExtHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CExtHeaderCtrl, CHeaderCtrl)
//{{AFX_MSG_MAP(CExtHeaderCtrl)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
ON_NOTIFY_REFLECT(HDN_BEGINTRACKW, OnBeginTrack) 
ON_NOTIFY_REFLECT(HDN_ENDTRACKA, OnEndTrack) 
ON_NOTIFY_REFLECT(HDN_BEGINTRACKA, OnBeginTrack) 
ON_NOTIFY_REFLECT(HDN_TRACKA, OnTrack) 
ON_NOTIFY_REFLECT(HDN_ITEMCHANGINGA, OnChangeItem) 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CExtHeaderCtrl 

BOOL CExtHeaderCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{  
	return CHeaderCtrl::OnNotify(wParam, lParam, pResult);
}

void CExtHeaderCtrl::OnEndTrack(NMHDR * pNMHDR, LRESULT* pResult)
{

	NMHEADER *pHdr = (NMHEADER*)pNMHDR;
	if (pHdr && pHdr->iItem == 0)
	{
		HDITEM *pItem = pHdr->pitem;
		if (pItem->mask & HDI_WIDTH)
		{
			pItem->cxy = 18;  
			this->SetItem(0, pItem);
			m_bBeginTrack = false;
		}
	
	}
	
	*pResult = 0;
}

void CExtHeaderCtrl::OnBeginTrack(NMHDR * pNMHDR, LRESULT* pResult)
{
	NMHEADER *pHdr = (NMHEADER*)pNMHDR;
	
	if (pHdr && pHdr->iItem == 0)
	{
		HDITEM *pItem = pHdr->pitem;
		if (pItem->mask & HDI_WIDTH)
		{
			pItem->cxy = 18;  
			m_bBeginTrack = true;
		}
	
	}
	
	*pResult = 0;
}

void CExtHeaderCtrl::OnTrack(NMHDR * pNMHDR, LRESULT* pResult)
{
	NMHEADER *pHdr = (NMHEADER*)pNMHDR;
	
	if (pHdr && pHdr->iItem == 0 && m_bBeginTrack)
	{
		HDITEM *pItem = pHdr->pitem;
		if (pItem->mask & HDI_WIDTH)
		{
			pItem->cxy = 18;  
		}
	
	}
	
	*pResult = 0;
}

void CExtHeaderCtrl::OnChangeItem(NMHDR * pNMHDR, LRESULT* pResult)
{
	
	NMHEADER *pHdr = (NMHEADER*)pNMHDR;
	

	if (pHdr && pHdr->iItem == 0 && m_bBeginTrack)
	{
		HDITEM *pItem = pHdr->pitem;
		if (pItem->mask & HDI_WIDTH)
		{
			pItem->cxy = 18;  
		}
	
	}

	*pResult = 0;
	
}

BOOL CExtHeaderCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest != HTCLIENT)
		return CHeaderCtrl::OnSetCursor(pWnd, nHitTest, message);
		
	
	// position of cursor in screen coordinates
	CPoint ptPoint;
	BOOL bResult = ::GetCursorPos(&ptPoint);

	// position of mouse in window
	ScreenToClient(&ptPoint);
 
	CRect rcItem0;
	GetItemRect(0, &rcItem0);
	
	CRect rcItem1;
	GetItemRect(1, &rcItem1);

	if (rcItem0.PtInRect(ptPoint) || (ptPoint.x <= (rcItem0.right + rcItem1.Width()/2)))
		return TRUE;


	return CHeaderCtrl::OnSetCursor(pWnd, nHitTest, message);
}
