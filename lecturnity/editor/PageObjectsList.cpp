// PageObjectsList.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "PageObjectsList.h"
#include "ChangeableObject.h"
#include "MainFrm.h"
#include "InteractionStream.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageObjectsList

CPageObjectsList::CPageObjectsList()
{
   m_pEditorDoc = NULL;
}

CPageObjectsList::~CPageObjectsList()
{
}


BEGIN_MESSAGE_MAP(CPageObjectsList, CListCtrl)
	//{{AFX_MSG_MAP(CPageObjectsList)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPageObjectsList 

int CPageObjectsList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Vergrößern / Verkleinern der ersten Spalte verhindern
	CHeaderCtrl *pHeaderCtrl = GetHeaderCtrl();
	if (pHeaderCtrl)
   {
		(m_wndHeaderCtrl.SubclassWindow)(pHeaderCtrl->m_hWnd);
      m_wndHeaderCtrl.FreezeColumn(0);
      m_wndHeaderCtrl.EnableAutoSize(false);
   }

	return 0;
}

void CPageObjectsList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UINT uFlags = 0;
	int nHitItem = HitTest(point, &uFlags);
	
	// we need additional checking in owner-draw mode
	// because we only get LVHT_ONITEM
	BOOL bHit = FALSE;
	if (uFlags != LVHT_ONITEM && uFlags & LVHT_ONITEMSTATEICON)
		bHit = TRUE;
	
	if (bHit)
		CheckItem(nHitItem);
	CListCtrl::OnLButtonDown(nFlags, point);
}

void CPageObjectsList::CheckItem(int nItemID)
{
	// check new item and set its icon as the app icon
	
	//CWnd* pMainWnd = AfxGetMainWnd();
	
	UINT uiItemState = GetItemState(nItemID, LVIS_STATEIMAGEMASK);
	if (uiItemState & INDEXTOSTATEIMAGEMASK(2))
	{
		SetItemState(nItemID,
			INDEXTOSTATEIMAGEMASK(1) | INDEXTOOVERLAYMASK(1),
			LVIS_STATEIMAGEMASK | LVIS_OVERLAYMASK);
      if (nItemID >= 0 && nItemID < m_caInteractions.GetSize())
         m_caInteractions[nItemID]->SetHiddenDuringEdit(false);
	}
	else
	{
		SetItemState(nItemID,
			INDEXTOSTATEIMAGEMASK(2) | INDEXTOOVERLAYMASK(1),
			LVIS_STATEIMAGEMASK | LVIS_OVERLAYMASK);
      if (nItemID >= 0 && nItemID < m_caInteractions.GetSize())
         m_caInteractions[nItemID]->SetHiddenDuringEdit(true);
	}

    CMainFrameE::GetCurrentInstance()->UpdateWhiteboardView();
}

void CPageObjectsList::DeSelectItems()
{
	POSITION p = GetFirstSelectedItemPosition();
	while (p)
	{
		int nSelected = GetNextSelectedItem(p);
		SetItemState(nSelected, 0, LVIS_SELECTED);
	}
}

void CPageObjectsList::SelectItem(int nItemID)
{
	// check new item and set its icon as the app icon
	//POSITION p = GetFirstSelectedItemPosition();
	//while (p)
	//{
	//	int nSelected = GetNextSelectedItem(p);
	//	SetItemState(nSelected, 0, LVIS_SELECTED);
	//}

   if (nItemID != -1)
   {
      SetItemState(nItemID, LVIS_SELECTED, LVIS_SELECTED);
	   EnsureVisible(nItemID, FALSE);
   }
}

int CPageObjectsList::GetSelectedItem()
{
	// check new item and set its icon as the app icon
	POSITION p = GetFirstSelectedItemPosition();
	int nSelected = GetNextSelectedItem(p);
	return nSelected;
}
	
void CPageObjectsList::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	if (pDispInfo->item.pszText != NULL)
   {
      int iItemID = pDispInfo->item.iItem;
      CString csText = pDispInfo->item.pszText;
		SetItemText(iItemID, 1, csText);
      if (iItemID >= 0 && iItemID < m_caInteractions.GetSize())
      {
         if (m_caInteractions[iItemID]->IsInteractionNameText())
         {
            CInteractionAreaEx *pCopy = m_caInteractions[iItemID]->Copy(true);
            pCopy->SetInteractionName(csText, true);

            
            if (m_pEditorDoc)
            {
               CInteractionStream *pInteractions = m_pEditorDoc->project.GetInteractionStream(false);
               if (pInteractions != NULL)
                  pInteractions->ModifyArea(m_caInteractions[iItemID]->GetHashKey(), pCopy, true);
            }
         }
         else
            m_caInteractions[iItemID]->SetInteractionName(csText, true); // TODO also here use ModifyArea()?
      }
   }

	m_wndEdit.UnsubclassWindow();

   CEditorDoc *pEditorDoc = 
       (CEditorDoc *)CMainFrameE::GetEditorDocument();
   pEditorDoc->UpdateAllViews(NULL, HINT_CHANGE_PAGE);

	*pResult = 0;
}

void CPageObjectsList::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	CEdit *pEdit = GetEditControl();
	if (pEdit && pEdit->GetSafeHwnd())
	{
		int nSelectedItem = GetSelectedItem();
		CString csText = GetItemText(nSelectedItem, 1);
		pEdit->SetWindowText(csText);
		
		// nötig um Acceleratoren zu umgehen
		(m_wndEdit.SubclassWindow)(pEdit->GetSafeHwnd());
	}
	
	*pResult = 0;
}

BOOL CPageObjectsList::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	return TRUE;
}

void CPageObjectsList::OnPaint() 
{
   CPaintDC dc(this);
   
   // Exclude the header control from being re-painted.
   CRect rcHeader;
   m_wndHeaderCtrl.GetWindowRect( &rcHeader );
   ScreenToClient( &rcHeader );
   dc.ExcludeClipRect( &rcHeader );
   
   // Get the client rect.
   CRect rcClient;
   GetClientRect(&rcClient);

   // Paint to a memory device context to help
   // eliminate screen flicker.
   CXTMemDC memDC(&dc, rcClient, RGB(255, 255, 255));
   
   // Now let the window do its default painting...
   CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );
   
   // Refresh the header control.
   m_wndHeaderCtrl.Invalidate();
   m_wndHeaderCtrl.UpdateWindow();
}

void CPageObjectsList::RefreshList(CEditorDoc *pDoc)
{
   CInteractionStream *pInteractionStream = 
                              pDoc->project.GetInteractionStream();

   if (pInteractionStream)
   {
      CArray<CInteractionAreaEx *, CInteractionAreaEx *> caInteractionsFind;
      pInteractionStream->FindInteractions(pDoc->m_curPosMs, false, false, &caInteractionsFind);
      bool bRefreshNeeded = false;
      bool bSelectionChanged = false;
      CUIntArray aSelectedItems;
      if (m_caInteractions.GetSize() != caInteractionsFind.GetSize())
      {
         bRefreshNeeded = true;
      }
      else
      {
         int iLastInteraction = m_caInteractions.GetSize()-1;
         for (int i = 0; i < m_caInteractions.GetSize(); ++i)
         {
            int iPosition = iLastInteraction - i;
            if (m_caInteractions[iPosition] != caInteractionsFind[i])
            {
               bRefreshNeeded = true;
               break;
            }

            if (caInteractionsFind[i]->IsSelected())
               aSelectedItems.Add(iPosition);
         }
      }

      if (bRefreshNeeded)
      {
         aSelectedItems.RemoveAll();
         // add new interactions to list
         DeleteAllItems();
         
         // save new interactions
         m_caInteractions.RemoveAll();
         for (int i = 0; i < caInteractionsFind.GetSize(); ++i)
            m_caInteractions.InsertAt(0, caInteractionsFind[i]);
         
         int iRadioButtonCounter = 0;
         int iCheckButtonCounter = 0;
         int iImageCounter = 0;
         int iTargetCounter = 0;
         int iEditCounter = 0;
         for (i = 0; i < m_caInteractions.GetSize(); ++i)
         {
            LV_ITEM lvi;
            
            lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
            lvi.iItem = i;
            lvi.iSubItem = 0;
            lvi.pszText = _T("");
            lvi.iImage = 1;
            lvi.stateMask = LVIS_STATEIMAGEMASK;
            if (m_caInteractions[i]->IsHiddenDuringEdit())
               lvi.state = INDEXTOSTATEIMAGEMASK(2);
            else
               lvi.state = INDEXTOSTATEIMAGEMASK(1);
            
            InsertItem(&lvi);
            
            
            SetItemText(i, 1, m_caInteractions[i]->GetInteractionName());
            SetItemText(i, 2, m_caInteractions[i]->GetInteractionType());
            
            if (m_caInteractions[i]->IsSelected())
               aSelectedItems.Add(i);
         }
         
      }

      DeSelectItems();
      for (int i = 0; i < aSelectedItems.GetSize(); ++i)
         SelectItem(aSelectedItems[i]);

   }
   else
   {
      DeleteAllItems();
   }

   
}

void CPageObjectsList::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
   
  
    CMainFrameE *pMainWnd = CMainFrameE::GetCurrentInstance();
    CWhiteboardView *pWhiteboardView = CMainFrameE::GetEditorView();
    /*
   if (AfxGetMainWnd() && 
       AfxGetMainWnd()->GetRuntimeClass() == RUNTIME_CLASS(CMainFrameE))
   {
      pMainWnd = (CMainFrameE *)AfxGetMainWnd();
      pWhiteboardView = pMainWnd->GetWhiteboardView();
   }
   */

   
   if ((pNMListView->iItem >= 0 && pNMListView->iItem < m_caInteractions.GetSize()))
   {
      UINT nState = GetItemState(pNMListView->iItem, LVIS_SELECTED);
      if (nState == 0)
      {
         m_caInteractions[pNMListView->iItem]->SetSelected(false);
         if (pWhiteboardView)
            pWhiteboardView->UpdateSelectionArray();
      }
      else
      {
         m_caInteractions[pNMListView->iItem]->SetSelected(true);
         if (pWhiteboardView)
            pWhiteboardView->SetActiveInteraction(m_caInteractions[pNMListView->iItem]);
      }
      
      if (pMainWnd)
         pMainWnd->UpdateWhiteboardView();
   }
   
   *pResult = 0;
}

void CPageObjectsList::MoveForward()
{
   int iSelected = GetSelectedItem();
   
   if (iSelected >= 0 && iSelected < m_caInteractions.GetSize())
   {
      
      CEditorDoc *pEditorDoc =  
          (CEditorDoc *)CMainFrameE::GetEditorDocument();
      if (pEditorDoc && pEditorDoc->project.GetInteractionStream())
      {
         UINT nHashKey = m_caInteractions[iSelected]->GetHashKey();
         pEditorDoc->project.GetInteractionStream()->MoveOneUp(nHashKey);
      }
   }
}

void CPageObjectsList::MoveBackward()
{
   int iSelected = GetSelectedItem();
   
   if (iSelected >= 0 && iSelected < m_caInteractions.GetSize())
   {
      
      CEditorDoc *pEditorDoc = 
               (CEditorDoc *)CMainFrameE::GetEditorDocument();;
      if (pEditorDoc && pEditorDoc->project.GetInteractionStream())
      {
         UINT nHashKey = m_caInteractions[iSelected]->GetHashKey();
         pEditorDoc->project.GetInteractionStream()->MoveOneDown(nHashKey);
      }
   }
}


void CPageObjectsList::MoveFront()
{
   int iSelected = GetSelectedItem();
   
   if (iSelected >= 0 && iSelected < m_caInteractions.GetSize())
   {
      
      CEditorDoc *pEditorDoc = 
               (CEditorDoc *)CMainFrameE::GetEditorDocument();;
      if (pEditorDoc && pEditorDoc->project.GetInteractionStream())
      {
         UINT nHashKey = m_caInteractions[iSelected]->GetHashKey();
         pEditorDoc->project.GetInteractionStream()->MoveToFront(nHashKey);
      }
   }
}

void CPageObjectsList::MoveBack()
{
   int iSelected = GetSelectedItem();
   
   if (iSelected >= 0 && iSelected < m_caInteractions.GetSize())
   {
      
      CEditorDoc *pEditorDoc = 
               (CEditorDoc *)CMainFrameE::GetEditorDocument();;
      if (pEditorDoc && pEditorDoc->project.GetInteractionStream())
      {
         UINT nHashKey = m_caInteractions[iSelected]->GetHashKey();
         pEditorDoc->project.GetInteractionStream()->MoveToBack(nHashKey);
      }
   }
}

void CPageObjectsList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   if (nChar == VK_F2)
   {
      int iSelected = GetSelectedItem();
      if (iSelected >= 0)
		   EditLabel(iSelected);
   }

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPageObjectsList::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	UINT uFlags = 0;
	int nHitItem = HitTest(point, &uFlags);
	
	// we need additional checking in owner-draw mode
	// because we only get LVHT_ONITEM
	BOOL bHit = FALSE;
	if (uFlags != LVHT_ONITEM && uFlags & LVHT_ONITEMSTATEICON)
		bHit = TRUE;
	
   if (!bHit)
   {
       CWhiteboardView *pWhiteboardView = CMainFrameE::GetEditorView();
      
      if (pWhiteboardView)
         pWhiteboardView->ShowInteractionProperties();
   }
   
	CListCtrl::OnLButtonDblClk(nFlags, point);

}

