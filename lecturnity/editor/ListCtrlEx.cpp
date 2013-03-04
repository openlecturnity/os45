// ListCtrlEx.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ListCtrlEx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CListCtrlEx::CListCtrlEx()
{
   m_pEdit = NULL;
   
   m_iSingleCheckedItem = -1;
   m_iCheckOption = SINGLE_SELECTION;
   m_iMove = -1;
   m_bHasFocus = true;

   m_iColumnCount = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CListCtrlEx::~CListCtrlEx()
{
   if (m_pEdit)
      delete m_pEdit;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
//{{AFX_MSG_MAP(CListCtrlEx)
   ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
   ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
   ON_WM_LBUTTONDOWN()
   ON_WM_SETFOCUS()
   ON_WM_KILLFOCUS()
   ON_WM_PAINT()
   ON_WM_CAPTURECHANGED()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CListCtrlEx 
void CListCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
   CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

   CRect rcItem(lpDrawItemStruct->rcItem);

   BOOL bFocus = (GetFocus() == this);
   COLORREF clrTextSave = 0;
   COLORREF clrBkSave = 0;
   static _TCHAR szBuff[MAX_PATH];
   
   // get item data
   
   int nItem = lpDrawItemStruct->itemID;
   LV_ITEM lvi;
   lvi.mask = LVIF_IMAGE | LVIF_STATE;
   lvi.iItem = nItem;
   lvi.iSubItem = 1;
   lvi.pszText = szBuff;
   lvi.cchTextMax = sizeof(szBuff);
   lvi.stateMask = 0xFFFF;     // get all state flags
   GetItem(&lvi);
   
   BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
   bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);
   
   // set colors if item is selected
   
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
   
   // draw state icon///////////////////////////////////////////////////////////////////////
   LV_COLUMN lvc;
   lvc.mask = LVCF_FMT | LVCF_WIDTH;
   
   GetColumn(0, &lvc);
   
   UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
   if (nStateImageMask)
   {
      int nImage = (nStateImageMask>>12) - 1;

      CImageList* pImageList = GetImageList(LVSIL_STATE);
      if (pImageList)
      {
         rcItem.left += lvc.cx;
         pImageList->Draw(pDC, nImage,
            CPoint(rcItem.left+8, rcItem.top), ILD_TRANSPARENT);
      }
   }
   
   int nColumn = 0; 
   GetColumn(nColumn, &lvc);
   
   GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);
   
   int nRetLen = GetItemText(nItem, nColumn,szBuff, sizeof(szBuff));
   if (nRetLen == 0)
      return;
   
   UINT nJustify = DT_LEFT;
   
   CRect rcText;
   rcText = rcAllLabels;
   rcText.left += OFFSET_OTHER;
   rcText.right -= OFFSET_OTHER;
   
   pDC->DrawText(szBuff, -1, rcText,
      nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
   
   // draw focus rectangle if item has focus
   
   if (lvi.state & LVIS_FOCUSED && bFocus)
   {
      pDC->DrawFocusRect(rcAllLabels);
   }
   
   // set original colors if item was selected
   
   if (bSelected)
   {
      pDC->SetTextColor(clrTextSave);
      pDC->SetBkColor(clrBkSave);
   }
   
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CListCtrlEx::PreCreateWindow(CREATESTRUCT& cs) 
{
   cs.style |= LVS_OWNERDRAWFIXED;
   return CListCtrl::PreCreateWindow(cs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CListCtrlEx::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CString str;
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
   {
      *pResult =0;
      return;
   }
   
   EndEditItem();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CListCtrlEx::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   
   if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
   {
      *pResult =0;
      return;
   }
   
   m_iItem = pNMListView->iItem;
   int col = pNMListView->iSubItem;

   if(m_iItem==(GetItemCount()-1))
   {
      InsertNewItem(GetItemCount() - 1);
   }
   else
   {
      EditItem(m_iItem);
   }
   
   *pResult = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
   UINT uFlags = 0;
   int nHitItem = HitTest(point, &uFlags);
   
   // we need additional checking in owner-draw mode
   // because we only get LVHT_ONITEM
   BOOL bHit = FALSE;
   if (uFlags == LVHT_ONITEM && (GetStyle() & LVS_OWNERDRAWFIXED))
   {
      CRect rect;
      GetItemRect(nHitItem, rect, LVIR_BOUNDS);
      LV_COLUMN lvc;
      lvc.mask = LVCF_FMT | LVCF_WIDTH;
      GetColumn(0, &lvc);
      
      // check if hit was on a state icon
      if (point.x >= (rect.left+lvc.cx))
         bHit = TRUE;
   }
   else if (uFlags & LVHT_ONITEMSTATEICON)
      bHit = TRUE;
   
   if (bHit)
      CheckItem(nHitItem);
   else
   {
      if (m_pEdit)
         EndEditItem();
      CListCtrl::OnLButtonDown(nFlags, point);	
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CListCtrlEx::CheckItem(int iNewCheckedItem)
{
   
   // reset if there is checked item
   
   if (m_iCheckOption == SINGLE_SELECTION)
   {
      if (m_iSingleCheckedItem != -1)
      {
         LV_ITEM lvi2;
         lvi2.mask = LVIF_IMAGE | LVIF_STATE;
         lvi2.iItem = m_iSingleCheckedItem;
         lvi2.iSubItem = 1;
         lvi2.iImage = m_iSingleCheckedItem;
         lvi2.iIndent = 0;
         lvi2.stateMask = LVIS_STATEIMAGEMASK;
         lvi2.state = INDEXTOSTATEIMAGEMASK(1);
         
         SetItem(&lvi2);
      }
      
      if(m_iMove != -1)
      {
         LV_ITEM lvi2;
         lvi2.mask = LVIF_IMAGE | LVIF_STATE;
         lvi2.iItem = m_iMove;
         lvi2.iSubItem = 1;
         lvi2.iImage = m_iMove;
         lvi2.iIndent = 0;
         lvi2.stateMask = LVIS_STATEIMAGEMASK;
         lvi2.state = INDEXTOSTATEIMAGEMASK(1);
         
         SetItem(&lvi2);
         m_iMove = -1;
      }
      
      // check new item and set its icon as the app icon
      
      
      if (m_iSingleCheckedItem == iNewCheckedItem)
      {
         m_iSingleCheckedItem = -1;
      }
      else if ((m_iSingleCheckedItem = iNewCheckedItem) != -1)
      {	
         LV_ITEM lvi2;
         lvi2.mask = LVIF_IMAGE | LVIF_STATE;
         lvi2.iItem = m_iSingleCheckedItem;
         lvi2.iSubItem = 1;
         lvi2.iImage = m_iSingleCheckedItem;
         lvi2.iIndent = 0;
         lvi2.stateMask = LVIS_STATEIMAGEMASK;
         lvi2.state = INDEXTOSTATEIMAGEMASK(2);
         
         SetItem(&lvi2);
         SelectItem(m_iSingleCheckedItem);
         SetFocus();
      }
   }
   else if (m_iCheckOption == MULTIPLE_SELECTION)
   {
      if (iNewCheckedItem != -1)
      {	
         LV_ITEM lvi2;
         lvi2.mask = LVIF_IMAGE | LVIF_STATE;
         lvi2.iItem = iNewCheckedItem;
         lvi2.iSubItem = 1;
         lvi2.iImage = iNewCheckedItem;
         lvi2.iIndent = 0;
         lvi2.stateMask = LVIS_STATEIMAGEMASK;
         lvi2.state = IsChecked(iNewCheckedItem) ? INDEXTOSTATEIMAGEMASK(1) : INDEXTOSTATEIMAGEMASK(2);
         
         
         SetItem(&lvi2);
         SelectItem(iNewCheckedItem);
         SetFocus();
      }
   }
   
   EndEditItem();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CListCtrlEx::ChangeSelectionType(int iType)
{
   m_iCheckOption = iType;
   m_iSingleCheckedItem = -1;

   for (int i = 0; i < GetItemCount()-1; ++i)
   {
      // just reset the state image (and thus the correct state)
      SetItem(i, 1, LVIF_STATE, NULL, 0, INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, NULL);
      
      // SetItem(LV_ITEM) doesn't seem to work in this case.
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CListCtrlEx::GetSelectedItem()
{
   // check new item and set its icon as the app icon
   POSITION p = GetFirstSelectedItemPosition();
   int nSelected = GetNextSelectedItem(p);
   return nSelected;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CListCtrlEx::SelectItem(int nItemID)
{
   
   // check new item and set its icon as the app icon
   POSITION p = GetFirstSelectedItemPosition();
   while (p)
   {
      int nSelected = GetNextSelectedItem(p);
      SetItemState(nSelected, 0, LVIS_SELECTED);
   }

   SetItemState(nItemID, LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED);
   
   EnsureVisible(nItemID, FALSE);
}  

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CListCtrlEx::EditItem(int iItem)
{
   HRESULT hr = S_OK;
   
   EndEditItem();
   
   m_pEdit = new CEdit();
   
   DWORD dwStyle = 
      WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_TABSTOP|ES_LEFT|ES_AUTOHSCROLL; 
   
   CRect rcEdit;
   GetSubItemRect(iItem, 0, LVIR_BOUNDS, rcEdit);
   //rcEdit.left = 0;
   rcEdit.right = GetColumnWidth(0);
   rcEdit.bottom -= 1;
   m_pEdit->Create(dwStyle, rcEdit, this, 1001);
   m_pEdit->ShowWindow(SW_SHOW);

   CFont *pFont;
   pFont = GetFont();	
   m_pEdit->SetFont(pFont, true);   

   CString csItemText = GetItemText(iItem, 0);
   m_pEdit->SetWindowText(csItemText);
   
   if (!csItemText.IsEmpty())
   {
      m_pEdit->SetSel(0,-1);
   }


   m_iItem = iItem;
   m_pEdit->SetFocus();

   return hr;
   
}

HRESULT CListCtrlEx::EndEditItem()
{
   HRESULT hr = S_OK;

   if (m_pEdit)
   {
      CString csTmp;
      
      m_pEdit->GetWindowText(csTmp);
      SetItemText(m_iItem, 0, csTmp);

      m_pEdit->DestroyWindow(); 
      delete m_pEdit;
      m_pEdit = NULL;
      
      CWnd *pParent = GetParent();
      pParent->SendMessage(WM_EDIT_FINISHED);
   }

   return hr;
}

HRESULT CListCtrlEx::InsertNewItem(int iItem, void *pItemData)
{
   HRESULT hr = S_OK;
   
   EndEditItem();

   LV_ITEM lviLeft;
   lviLeft.mask = LVIF_TEXT| LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
   lviLeft.iItem = iItem;
   lviLeft.iSubItem = 0;
   lviLeft.pszText = _T("");
   lviLeft.iImage = 0;
   lviLeft.stateMask = 0;
   lviLeft.state = 0;
   lviLeft.lParam = (long)pItemData;
   InsertItem(&lviLeft);
   
   if (m_iColumnCount > 1)
   {
      LV_ITEM lviRight;
      lviRight.mask = LVIF_IMAGE | LVIF_STATE;
      lviRight.iItem = iItem;
      lviRight.iSubItem = 1;
      lviRight.iImage = 0;
      lviRight.iIndent = 0;
      lviRight.stateMask = LVIS_STATEIMAGEMASK;
      lviRight.state = INDEXTOSTATEIMAGEMASK(1);
      SetItem(&lviRight);
   }

   EditItem(iItem);
   SelectItem(iItem);

   return hr;
   
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CListCtrlEx::OnSetFocus(CWnd* pOldWnd) 
{
   CListCtrl::OnSetFocus(pOldWnd);
   
   m_bHasFocus = true;	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CListCtrlEx::OnKillFocus(CWnd* pNewWnd) 
{
   m_bHasFocus = false;
   
   CString csTmp;
   if (m_pEdit && 
      (pNewWnd != this) &&
      (pNewWnd != m_pEdit))
   {
      EndEditItem();
   }
   
   CListCtrl::OnKillFocus(pNewWnd);	
}

void CListCtrlEx::OnCaptureChanged(CWnd *pWnd) 
{
   if (m_pEdit)
      EndEditItem();

	CListCtrl::OnCaptureChanged(pWnd);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CListCtrlEx::PreTranslateMessage(MSG* pMsg) 
{
   CString str;
   if( pMsg->message == WM_KEYDOWN )
   {
      if(pMsg->wParam == VK_RETURN)
      {
         EndEditItem();
         
         ::TranslateMessage(pMsg);
         ::DispatchMessage(pMsg);
         return TRUE;		    	
      }
      
      if(pMsg->wParam == VK_ESCAPE)
      {
         if (m_pEdit) 
         {
            m_pEdit->DestroyWindow(); 
            delete m_pEdit;
            m_pEdit = NULL;
         }
         
         ::TranslateMessage(pMsg);
         ::DispatchMessage(pMsg);
         return TRUE;		    	
      }
   }	
   return CListCtrl::PreTranslateMessage(pMsg);
}

bool CListCtrlEx::IsItemSelected()
{
   if (GetSelectedCount() > 0)
      return true;
   else
      return false;
}

HRESULT CListCtrlEx::MoveSelectedUp()
{
   HRESULT hr = S_OK;

   EndEditItem();

   int iSelectedItem = GetSelectedItem();
   if (iSelectedItem <= 0)
      return E_FAIL;

   static _TCHAR szBuff[MAX_PATH];
   LV_ITEM lvi_SelectedLeft;
   lvi_SelectedLeft.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
   lvi_SelectedLeft.iItem = iSelectedItem;
   lvi_SelectedLeft.iSubItem = 0;   
   lvi_SelectedLeft.pszText = szBuff;
   lvi_SelectedLeft.cchTextMax = sizeof(szBuff);
   lvi_SelectedLeft.stateMask = LVIS_STATEIMAGEMASK; 
   GetItem(&lvi_SelectedLeft);

   LV_ITEM lvi_SelectedRight;
   
   if (m_iColumnCount > 1)
   {
      lvi_SelectedRight.mask = LVIF_IMAGE | LVIF_STATE;
      lvi_SelectedRight.iItem = iSelectedItem;
      lvi_SelectedRight.iSubItem = 1;
      lvi_SelectedRight.pszText = szBuff;
      lvi_SelectedRight.cchTextMax = sizeof(szBuff);
      lvi_SelectedRight.stateMask = LVIS_STATEIMAGEMASK; 
      GetItem(&lvi_SelectedRight);
   }

   DeleteItem(iSelectedItem);

   lvi_SelectedLeft.iItem = iSelectedItem-1;
   InsertItem(&lvi_SelectedLeft);
   
   if (m_iColumnCount > 1)
   {
      lvi_SelectedRight.iItem = iSelectedItem-1;
      SetItem(&lvi_SelectedRight);

      if (m_iSingleCheckedItem == iSelectedItem)
      {
         m_iSingleCheckedItem = iSelectedItem-1;
         
         LV_ITEM lvi2;
         lvi2.mask = LVIF_IMAGE | LVIF_STATE;
         lvi2.iItem = m_iSingleCheckedItem;
         lvi2.iSubItem = 1;
         lvi2.iImage = m_iSingleCheckedItem;
         lvi2.iIndent = 0;
         lvi2.stateMask = LVIS_STATEIMAGEMASK;
         lvi2.state = INDEXTOSTATEIMAGEMASK(2);
         
         SetItem(&lvi2);
      }
      else
      {
         LV_ITEM lvi2;
         lvi2.mask = LVIF_IMAGE | LVIF_STATE;
         lvi2.iItem = iSelectedItem-1;
         lvi2.iSubItem = 1;
         lvi2.iImage = iSelectedItem-1;
         lvi2.iIndent = 0;
         lvi2.stateMask = LVIS_STATEIMAGEMASK;
         lvi2.state = INDEXTOSTATEIMAGEMASK(1);
         
         SetItem(&lvi2);
      }
   }
   
   if (m_iSingleCheckedItem == iSelectedItem)
      m_iSingleCheckedItem = iSelectedItem-1;

   SelectItem(iSelectedItem-1); 

   return hr;
}

HRESULT CListCtrlEx::MoveSelectedDown()
{
   HRESULT hr = S_OK;

   EndEditItem();

   int iSelectedItem = GetSelectedItem();
   if (iSelectedItem >= GetItemCount() - 1)
      return E_FAIL;

   static _TCHAR szBuff[MAX_PATH];
   LV_ITEM lvi_SelectedLeft;
   lvi_SelectedLeft.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
   lvi_SelectedLeft.iItem = iSelectedItem;
   lvi_SelectedLeft.iSubItem = 0;   
   lvi_SelectedLeft.pszText = szBuff;
   lvi_SelectedLeft.cchTextMax = sizeof(szBuff);
   lvi_SelectedLeft.stateMask = LVIS_STATEIMAGEMASK;
   GetItem(&lvi_SelectedLeft);
   
   LV_ITEM lvi_SelectedRight;
   if (m_iColumnCount > 1)
   {
      lvi_SelectedRight.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
      lvi_SelectedRight.iItem = iSelectedItem;
      lvi_SelectedRight.iSubItem = 1;
      lvi_SelectedRight.pszText = szBuff;
      lvi_SelectedRight.cchTextMax = sizeof(szBuff);
      lvi_SelectedRight.stateMask = LVIS_STATEIMAGEMASK; 
      GetItem(&lvi_SelectedRight);
   }
   
   DeleteItem(iSelectedItem);
   
   lvi_SelectedLeft.iItem = iSelectedItem+1;
   InsertItem(&lvi_SelectedLeft);
   
   if (m_iColumnCount > 1)
   {
      lvi_SelectedRight.iItem = iSelectedItem+1;
      SetItem(&lvi_SelectedRight);
      if (m_iSingleCheckedItem == iSelectedItem)
      {
         m_iSingleCheckedItem = iSelectedItem+1;
         
         LV_ITEM lvi2;
         lvi2.mask = LVIF_IMAGE | LVIF_STATE;
         lvi2.iItem = m_iSingleCheckedItem;
         lvi2.iSubItem = 1;
         lvi2.iImage = m_iSingleCheckedItem;
         lvi2.iIndent = 0;
         lvi2.stateMask = LVIS_STATEIMAGEMASK;
         lvi2.state = INDEXTOSTATEIMAGEMASK(2);
         
         SetItem(&lvi2);
      }
      else
      {
         LV_ITEM lvi2;
         lvi2.mask = LVIF_IMAGE | LVIF_STATE;
         lvi2.iItem = iSelectedItem+1;
         lvi2.iSubItem = 1;
         lvi2.iImage = iSelectedItem+1;
         lvi2.iIndent = 0;
         lvi2.stateMask = LVIS_STATEIMAGEMASK;
         lvi2.state = INDEXTOSTATEIMAGEMASK(1);
         
         SetItem(&lvi2);
      }
   }
    
   if (m_iSingleCheckedItem == iSelectedItem)
      m_iSingleCheckedItem = iSelectedItem+1;

   SelectItem(iSelectedItem+1); 

   return hr;
}

// private
bool CListCtrlEx::IsChecked(int iIndex)
{
   if (iIndex < 0 || iIndex >= GetItemCount())
      return false;

   LV_ITEM lvi;
   lvi.mask = LVIF_IMAGE | LVIF_STATE;
   lvi.stateMask = LVIS_STATEIMAGEMASK;
   lvi.iItem = iIndex;
   lvi.iSubItem = 1;
   lvi.iIndent = 0;

   GetItem(&lvi);
   // GetItemState() alone doesn't seem to return anything?

   UINT nStateImageMask = lvi.state;

   int nImage = (nStateImageMask >> 12);

   return nImage == 2; // otherwise it is 1 for false/unselected image
}