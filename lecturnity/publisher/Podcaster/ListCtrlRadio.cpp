// ListCtrlRadio.cpp : implementation file
//

#include "stdafx.h"
#include "podcaster.h"
#include "ListCtrlRadio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlRadio

CListCtrlRadio::CListCtrlRadio()
{
   m_ilRadioButtons.Create(IDB_RADIOBUTTON, 16, 2, RGB(255, 255, 255));
}

CListCtrlRadio::~CListCtrlRadio()
{
}


BEGIN_MESSAGE_MAP(CListCtrlRadio, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlRadio)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlRadio message handlers
BOOL CListCtrlRadio::Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	BOOL bCreated = CListCtrl::Create(dwStyle, rect, pParentWnd,nID );
   SetExtendedStyle(LVS_EX_SUBITEMIMAGES);
   SetImageList(&m_ilRadioButtons, LVSIL_STATE);
	
   return bCreated;
}

void CListCtrlRadio::SetState(int iItem, int iSubItem, int iState)
{
   lvi.mask = LVIF_IMAGE | LVIF_STATE;
   lvi.iItem = iItem;
   lvi.iSubItem = 1;
   lvi.iImage = iState;
   lvi.iIndent = 0;
   lvi.stateMask = LVIS_STATEIMAGEMASK;
   lvi.state = INDEXTOSTATEIMAGEMASK(iState + 1);
   this->SetItem(&lvi);
}

void CListCtrlRadio::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
   CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

   COLORREF clrTextSave = 0;
   COLORREF clrBkSave   = 0;
   
   // get item data 
   int nItem = lpDrawItemStruct->itemID;
    
   // text
   lvi.mask = LVIF_STATE;
   lvi.iItem = nItem;
   lvi.iSubItem = 0;
   lvi.pszText = szBuff;
   lvi.cchTextMax = sizeof(szBuff);
   lvi.stateMask = 0xFFFF;     // get all state flags  
   GetItem(&lvi);
   
   BOOL bFocus = (GetFocus() == this);
   BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
   bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);
    
   // set colors if item is selected 
   CRect rcLabel;
   this->GetSubItemRect(nItem, 0, LVIR_LABEL, rcLabel);
   //GetItemRect(nItem, rcLabel, LVIR_LABEL);
   
   if (bSelected)
   {
      clrBkSave   = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
      clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
      CBrush cbr(::GetSysColor(COLOR_HIGHLIGHT));
      pDC->FillRect(rcLabel, &cbr);
   }
   else
   {
      CBrush cbr(RGB(255, 255, 255));
      pDC->FillRect(rcLabel, &cbr);
   }
    
   szBuff[0] = 0;
   GetItemText(nItem, 0, szBuff, sizeof(szBuff));

   DWORD dwFlags =  DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER;
   pDC->DrawText(szBuff, -1, rcLabel, dwFlags);

   // draw focus rectangle if item has focus  
   if (lvi.state & LVIS_FOCUSED && bFocus)
   {
      pDC->DrawFocusRect(rcLabel);
   }

   // set original colors if item was selected   
   if (bSelected)
   {
      pDC->SetBkColor(clrBkSave);
      pDC->SetTextColor(clrTextSave);
   }

   // draw state icon
   lvi.mask = LVIF_STATE;
   lvi.iItem = nItem;
   lvi.iSubItem = 1;
   lvi.pszText = szBuff;
   lvi.cchTextMax = sizeof(szBuff);
   lvi.stateMask = 0xFFFF;     // get all state flags
   GetItem(&lvi);
 
   GetSubItemRect(nItem, 1, LVIR_BOUNDS, rcLabel);
   // clear subitem
   CBrush cbr(RGB(255, 255, 255));
   pDC->FillRect(rcLabel, &cbr);

   UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
   if(nStateImageMask)
   {
      int nImage = (nStateImageMask >> 12) - 1;     
      m_ilRadioButtons.Draw(pDC, nImage, CPoint(rcLabel.left + 8, rcLabel.top), ILD_TRANSPARENT);
   }
     
}
