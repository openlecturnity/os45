// TargetListDD.cpp : implementation file
//

#include "stdafx.h"  
#include "TargetListDD.h"

//---------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CTargetListDD

CTargetListDD::CTargetListDD()
{
}

//---------------------------------------------------------------------------------------------------------------------
CTargetListDD::~CTargetListDD()
{
}

//---------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTargetListDD, CListCtrl)
//{{AFX_MSG_MAP(CTargetListDD)
// NOTE - the ClassWizard will add and remove mapping macros here.
ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
ON_NOTIFY_REFLECT(HDN_TRACK, OnTrack)
ON_NOTIFY_REFLECT(HDN_BEGINTRACK, OnBegintrack)
ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)

ON_WM_CREATE()
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CTargetListDD message handlers
void CTargetListDD::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
   // TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
   
   CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
   CRect rcItem(lpDrawItemStruct->rcItem);
   UINT uiFlags = ILD_TRANSPARENT;
   int nItem = lpDrawItemStruct->itemID;
   BOOL bFocus = (GetFocus() == this);
   COLORREF clrTextSave = 0;
   COLORREF clrBkSave = 0;
   COLORREF clrImage = RGB(0, 255, 0);
   static _TCHAR szBuff[MAX_PATH];
   //	LPCTSTR pszText;
   
   // get item data
   
   LV_ITEM lvi;
   lvi.mask = LVIF_TEXT | LVIF_STATE;
   lvi.iItem = nItem;
   lvi.iSubItem = 0;
   lvi.pszText = szBuff;
   lvi.cchTextMax = sizeof(szBuff);
   lvi.stateMask = 0xFFFF;     // get all state flags
   GetItem(&lvi);
   
   BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
   bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);
   
   // set colors if item is selected
   LV_COLUMN lvc;
   
   
   GetColumn(0, &lvc);
   
   CRect rcAllLabels;
   GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);
   
   if (bSelected)
   {
      clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
      clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
      
      CBrush cbr(::GetSysColor(COLOR_HIGHLIGHT));
      pDC->FillRect(rcAllLabels, &cbr);
   }
   else
   {
      CBrush cbr(RGB(255, 255, 255));
      pDC->FillRect(rcAllLabels, &cbr);
   }
   
   if (lvi.state & LVIS_CUT)
   {
      clrImage = RGB(255, 255, 255);
      uiFlags |= ILD_BLEND50;
   }
   else if (bSelected)
   {
      clrImage = ::GetSysColor(COLOR_HIGHLIGHT);
      uiFlags |= ILD_BLEND50;
   }
   // draw labels for columns
   lvc.mask = LVCF_FMT | LVCF_WIDTH;
   
   int nColumn = 0; 
   GetColumn(nColumn, &lvc);
   
   GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);
   
   int nRetLen = GetItemText(nItem, nColumn,
      szBuff, sizeof(szBuff));
   if (nRetLen == 0)
      return;
   
   
   UINT nJustify = DT_LEFT;
   
   
   switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
   {
   case LVCFMT_RIGHT:
      nJustify = DT_RIGHT;
      break;
   case LVCFMT_CENTER:
      nJustify = DT_CENTER;
      break;
   default:
      break;
      
   }
   
   CRect rcText;
   rcText = rcAllLabels;
   
   
   pDC->DrawText(szBuff, -1, rcText,
      nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
   
   // draw focus rectangle if item has focus
   
   if (lvi.state & LVIS_FOCUSED && bFocus)
      pDC->DrawFocusRect(rcAllLabels);
   
   // set original colors if item was selected
   
   if (bSelected)
   {
      pDC->SetTextColor(clrTextSave);
      pDC->SetBkColor(clrBkSave);
   }
   
   
}
//---------------------------------------------------------------------------------------------------------------------
BOOL CTargetListDD::PreCreateWindow(CREATESTRUCT& cs) 
{
   // TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
   
   cs.style |= LVS_OWNERDRAWFIXED;
   //m_bFullRowSel = TRUE;
   
   return CListCtrl::PreCreateWindow(cs);
}
//---------------------------------------------------------------------------------------------------------------------
int CTargetListDD::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   return 0;
}
//---------------------------------------------------------------------------------------------------------------------
void CTargetListDD::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   // TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
   
   *pResult = 0;
}
//---------------------------------------------------------------------------------------------------------------------

void CTargetListDD::OnTrack(NMHDR* pNMHDR, LRESULT* pResult) 
{
   HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
   // TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
   
   *pResult = 0;
}

//---------------------------------------------------------------------------------------------------------------------
void CTargetListDD::OnBegintrack(NMHDR* pNMHDR, LRESULT* pResult) 
{
   HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
   // TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
   
   *pResult = 0;
}

//---------------------------------------------------------------------------------------------------------------------
void CTargetListDD::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
   LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
   LV_ITEM	*plvItem = &plvDispInfo->item;
   
   if (plvItem->pszText != NULL)
   {
      CString csText=plvItem->pszText;
      if(csText.IsEmpty())
         return;
      SetItemText(plvItem->iItem, plvItem->iSubItem, csText);
   }
   
   
   *pResult = FALSE;
}
//---------------------------------------------------------------------------------------------------------------------

void CTargetListDD::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
   
   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   CEdit *pEdit = GetEditControl();
   
   if (pEdit && pEdit->GetSafeHwnd())
   { 
      int nSelectedItem = GetSelectedItem();
      
      CString csText = GetItemText(nSelectedItem, 0);
      csOldText = csText;
      pEdit->SetWindowText(csText);
      // nötig um Acceleratoren zu umgehen
   }
   
   *pResult = 0;
}

//---------------------------------------------------------------------------------------------------------------------
int CTargetListDD::GetSelectedItem()
{
   // check new item and set its icon as the app icon
   POSITION p = GetFirstSelectedItemPosition();
   int nSelected = GetNextSelectedItem(p);	
   return nSelected;
   
}

