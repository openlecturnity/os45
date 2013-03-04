// DocumentStructureTree.cpp : implementation file
//

#include "stdafx.h"
#include "Assistant.h"
#include "DocumentStructureTree.h"
#include "backend/mlb_generic.h"
#include "DlgKeywords.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDocumentStructureTree

CDocumentStructureTree::CDocumentStructureTree()
{
   m_hItemDrag = NULL;
   m_bAfterTree = false;
   m_bBeforeTree = false;
   m_bFileNameExists = false;
   m_nLastId=0;
   m_hItemId = NULL;
   bInsertInChapter = false;
   bInsertFirst = false;
   m_bEndDrag = false;
}

CDocumentStructureTree::~CDocumentStructureTree()
{
   m_hItemDrag = NULL;
   m_hItemDrop = NULL;
   m_hItemEdit = NULL;
}


BEGIN_MESSAGE_MAP(CDocumentStructureTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CDocumentStructureTree)
   
   ON_WM_CONTEXTMENU()
   ON_WM_KEYDOWN()
   ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
   ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
   ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
   ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
   ON_COMMAND(ID_RENAME, OnRenameItem)
   ON_COMMAND(ID_DELETE, OnDeleteItem)
   ON_COMMAND(ID_INSERT_PAGE, OnInsertPage)
   ON_COMMAND(ID_INSERT_CHAPTER, OnInsertChapter)
   ON_COMMAND(ID_SAVE_PAGE_CHAPTER, OnSavePageChapter)
   ON_COMMAND(ID_INSERT_KEYWORDS, OnInsertKeywords)
   ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelectionChanged)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocumentStructureTree message handlers

BOOL CDocumentStructureTree::PreTranslateMessage(MSG* pMsg)
{
      
     CEdit *edit = GetEditControl();
        
     if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT || 
		 pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN || pMsg->wParam == VK_SPACE || 
		 pMsg->wParam == VK_BACK || pMsg->wParam == VK_DELETE) && edit) /*|| 
        pMsg->wParam == VK_NEXT  || pMsg->wParam == VK_PRIOR */
      {
		SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam); 
        return TRUE;
      }

     if(pMsg->message == WM_RBUTTONDOWN)  
      {
        SendMessage(WM_CONTEXTMENU, pMsg->wParam, pMsg->lParam);
        return TRUE;  
      }

     return CTreeCtrl::PreTranslateMessage(pMsg);


}

void CDocumentStructureTree::DoDataExchange(CDataExchange* pDX)
{
   CTreeCtrl::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocumentStructureTree)

	//}}AFX_DATA_MAP
}


void CDocumentStructureTree::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
   
   LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW)pNMHDR;
	*pResult = 0;	
	
	CImageList* imageList = NULL;   
	POINT ptOffset;
	RECT rcItem;
	
	if ((imageList = CreateDragImage(lpnmtv->itemNew.hItem)) == NULL)
		return;
	if (GetItemRect(lpnmtv->itemNew.hItem, &rcItem, TRUE))
	{
		CPoint ptDragBegin;
		int nX, nY;
		ptDragBegin = lpnmtv->ptDrag;
		ImageList_GetIconSize(imageList->GetSafeHandle(), &nX, &nY);
		ptOffset.x = (ptDragBegin.x - rcItem.left) + (nX - (rcItem.right - rcItem.left));
		ptOffset.y = (ptDragBegin.y - rcItem.top) + (nY - (rcItem.bottom - rcItem.top));
		MapWindowPoints(NULL, &rcItem);
	}
	else
		GetWindowRect(&rcItem);		
	
	
	BOOL bDragBegun = imageList->BeginDrag(0, CPoint(-15,-15));
		
	if (! bDragBegun)
	{
		
      delete imageList;
		return;
	}
	
	CPoint ptDragEnter = lpnmtv->ptDrag;
	ClientToScreen(&ptDragEnter);
	if (!imageList->DragEnter(NULL, ptDragEnter))
	{      
		delete imageList;
		return;
	}
	
	delete imageList;

	InvalidateRect(&rcItem, TRUE);
	UpdateWindow();
	
	SetCapture(); 
	m_hItemDrag = lpnmtv->itemNew.hItem;  
}


void CDocumentStructureTree::OnMouseMove(UINT nFlags, CPoint point) 
{
	
	UINT		flags;
	HTREEITEM	hitem= HitTest(point, &flags);
	
	
	if (m_hItemDrag != NULL)
	{
		CPoint pt;
		
		// drag the item to the current position 
		pt = point;
		ClientToScreen(&pt);
		
		CImageList::DragMove(pt);
		CImageList::DragEnter(this, point);
		
		CImageList::DragShowNolock(FALSE);
		
		TVHITTESTINFO tvhti;
		tvhti.pt = pt;
		ScreenToClient(&tvhti.pt);
		HTREEITEM hItemSel = HitTest(&tvhti);
		
		int image,selected;
		GetItemImage(m_hItemDrag,image,selected);

		int imagePage,selectedPage;
		GetItemImage(hitem,imagePage,selectedPage);
		
		HTREEITEM hItemChild = GetParentItem(hitem);
		HTREEITEM hItemNextChild;
		bool bSelectChapter = true;
		//bool bCursorArrow = true;
		
		if(GetChildItem(m_hItemDrag) != NULL)
		{	 
			while(hItemChild != m_hItemDrag /*&& hItemChild!=GetRootItem()*/ && hItemChild!=NULL)
			{   				
				hItemNextChild = GetParentItem(hItemChild);	
				hItemChild = hItemNextChild;
			}
		}	
		
        bool bIsDropChildDrag = false;

		HTREEITEM hParentItem = GetParentItem(hitem);
		while(hParentItem != GetRootItem() && hParentItem != NULL)
		{
			if(hParentItem == m_hItemDrag)
			{
				bIsDropChildDrag = true;
				break;
			}
			hParentItem = GetParentItem(hParentItem);
		}
		
        if (hitem == m_hItemDrag || (GetParentItem(GetNextItem(hitem,TVGN_PARENT))!=NULL && GetParentItem(m_hItemDrag)==NULL)
			|| (GetParentItem(GetParentItem(hitem))==NULL && GetParentItem(hitem)!=NULL  && GetParentItem(m_hItemDrag)==NULL)
			|| (GetParentItem(m_hItemDrag) && hitem == GetRootItem()/*!GetParentItem(hitem)*/)
            ||  bIsDropChildDrag)
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
			SelectDropTarget(NULL);
			SetInsertMark(NULL, true);
			//bCursorArrow = false;
			
		}
		else
		{
			if (CWnd::WindowFromPoint(pt) != this /*|| GetRootItem() == m_hItemDrag || (GetRootItem() == hitem && GetChildItem(m_hItemDrag) == NULL && image == 1)*/)
			{	
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
				SelectDropTarget(NULL);
				SetInsertMark(NULL, true);
				//bCursorArrow = false;
			} 
			else
			{
				if(/*GetParentItem(hitem)==NULL &&*/ hitem == GetRootItem())
				{
					bSelectChapter = false;
				}
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
				
				GetItemImage(tvhti.hItem,image,selected);
				
				// setting the insert mark if the item is a directory  
				
				CRect rect;
				GetItemRect(hitem,rect,false);
				CPoint pointCenter=rect.CenterPoint();
				CPoint pointTop=rect.TopLeft();
				CPoint pointBottom=rect.BottomRight();
				
				//select item if is directory
				
				if(((GetChildItem(tvhti.hItem)!=NULL || image == 0))/* && bCursorArrow*//*&& GetRootItem()!=hitem*/ && ((pointBottom.y-6>point.y) && (pointTop.y+6<point.y))) 
				{
					m_hItemDrop=tvhti.hItem;   
					m_bAfterTree=false;
					m_bBeforeTree=false;
					if(bSelectChapter)
						SelectDropTarget(tvhti.hItem);
					else
						SelectDropTarget(NULL);	
					SetInsertMark(NULL,true);
					
				}
				else 
					
					//set insert mark after directory/child item
					
					if(/*(GetChildItem(tvhti.hItem) != NULL || image == 0) && GetParentItem(tvhti.hItem) &&*/ /*bCursorArrow &&*/ ((pointBottom.y-6) <= point.y && (pointBottom.y+6 >= point.y)))
					{
						
						m_hItemDrop=tvhti.hItem;   
						m_bAfterTree=true;
						m_bBeforeTree=false;
						SetInsertMark(tvhti.hItem,true);
						SelectDropTarget(NULL);	
						
					}
					
					//set insert mark before directory/child item
					
					else
					{
						if((/*GetChildItem(hitem) != NULL*/ image==1 || image==0) /*&& bCursorArrow*/ )
						{
							m_bAfterTree=false;
							m_bBeforeTree=true;

							SetInsertMark(tvhti.hItem,false);
							SelectDropTarget(NULL);
						}
						m_hItemDrop=tvhti.hItem;
						
						//if(/*GetNextItem(tvhti.hItem,TVGN_PREVIOUS)!=NULL*/tvhti.hItem!=GetRootItem())
						//{
							
							/*m_bAfterTree=false;
							m_bBeforeTree=true;
							SetInsertMark(tvhti.hItem,false);
							SelectDropTarget(NULL);	*/
						//}
                  
					}     
			}
		}
   }
   
   CImageList::DragShowNolock(TRUE);   
   CTreeCtrl::OnMouseMove(nFlags, point);  
}


void CDocumentStructureTree::OnLButtonUp(UINT nFlags, CPoint point) 
{  

   UINT flags;
   HTREEITEM hitem= HitTest(point, &flags);

   if(GetItemState(hitem,TVE_EXPAND) && m_hItemDrag == NULL)
   {
      ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(hitem);
      int nId = pContainer->GetID();

      for(int i=0; i<m_folderState.anId.size(); i++)
      {
         if(m_folderState.anId.at(i) == nId)
         {
            if(m_folderState.anExpanded.at(i) == 1)
               m_folderState.anExpanded.at(i) = 0;
            else
               if(m_folderState.anExpanded.at(i) == 0)
                  m_folderState.anExpanded.at(i) = 1;
            break;
         }
      }

   }

   if (m_hItemDrag != NULL)
   {
      SelectDropTarget(NULL);
      SetInsertMark(NULL,false);
      OnEndDrag(nFlags, point);
      m_hItemDrag = NULL;

   }
   else
      CTreeCtrl::OnLButtonUp(nFlags, point);    
}


void CDocumentStructureTree::OnEndDrag(UINT nFlags, CPoint point)
{
	if (m_hItemDrag == NULL || m_hItemDrop == NULL || GetCursor() == AfxGetApp()->LoadStandardCursor(IDC_NO))
	{
	FinishDragging(TRUE);
	RedrawWindow();
	return;
	}	
	
	HTREEITEM hItemDrag = m_hItemDrag;
	HTREEITEM hItemDrop = m_hItemDrop;
	
	int imageDrag,imageDrop,selected;
	
	GetItemImage(hItemDrop,imageDrop,selected);
	GetItemImage(hItemDrag,imageDrag,selected);
	
	//drag item is a slide
	
	if(imageDrag == 1)
	{
		if(m_bAfterTree == true && imageDrag == 1 && imageDrop == 1)
         SendMoveMessage(hItemDrag, hItemDrop);
		else
			if(m_bBeforeTree == true && imageDrag == 1 && imageDrop == 1)
			{
				if(GetPrevVisibleItem(hItemDrop) == GetParentItem(hItemDrop))  
				{  
					bInsertInChapter = true;
					SendMoveMessage(hItemDrag, GetParentItem(hItemDrop)); 
				}
				else
					SendMoveMessage(hItemDrag, GetNextItem(hItemDrop,TVGN_PREVIOUS));
			}
			else   
				if(m_bAfterTree == true && imageDrag == 1 && imageDrop == 0/* && GetChildItem(hItemDrop) != NULL*/)
				{
					//bInsertInChapter = true;
					SendMoveMessage(hItemDrag, hItemDrop);	
					
				}
				else   
					if(m_bBeforeTree == true && imageDrag == 1 && imageDrop == 0)
						if(!GetParentItem(hItemDrop))
						{
							bInsertInChapter = true;
							SendMoveMessage(hItemDrag, GetPrevVisibleItem(hItemDrop));
						}
						else
							if(!GetParentItem(GetPrevVisibleItem(hItemDrop)))
							{
								bInsertInChapter = true;
								SendMoveMessage(hItemDrag, GetParentItem(hItemDrop));
							}
							else   
								SendMoveMessage(hItemDrag, GetPrevVisibleItem(hItemDrop));
					 else   
						if(m_bBeforeTree == false && m_bAfterTree == false && imageDrag == 1 && imageDrop == 0)
						{
							bInsertInChapter = true;
							SendMoveMessage(hItemDrag, hItemDrop);
						}
      					
	}				
	//drag item is an empty directory
	
	if(imageDrag == 0 && GetChildItem(hItemDrag) == NULL)
	{
		if(m_bAfterTree == true)
		{
		    if(imageDrag == 0 && imageDrop == 0)
			if(!GetParentItem(hItemDrop) && GetParentItem(hItemDrag))
				bInsertInChapter = true;
			SendMoveMessage(hItemDrag, hItemDrop);
		}
		else	
			if(m_bBeforeTree == true && imageDrag == 0 && imageDrop == 1)
			{
				if(GetPrevVisibleItem(hItemDrop) == GetParentItem(hItemDrop))
				{
					bInsertInChapter = true;
					SendMoveMessage(hItemDrag, GetParentItem(hItemDrop));
				}
				else
					SendMoveMessage(hItemDrag, GetNextItem(hItemDrop,TVGN_PREVIOUS));
			}
			else  
				if(m_bBeforeTree == true && imageDrag == 0 && imageDrop == 0)
				{
                  if((!GetParentItem(hItemDrag) && !GetParentItem(hItemDrop)) ||  GetRootItem() == hItemDrop/*&& GetRootItem() != hItemDrop*/)
                  {
                     if(GetRootItem() == hItemDrop)
					 {
						bInsertFirst = true;
						SendMoveMessage(hItemDrag, hItemDrop); 
					 }
					  //bInsertInChapter = true;
                     //SendMoveMessage(hItemDrag, hItemDrop);
					 else
					  SendMoveMessage(hItemDrag, GetNextItem(hItemDrop,TVGN_PREVIOUS)); 
                  }
                  else
                     if(!GetParentItem(GetPrevVisibleItem(hItemDrop)) && GetRootItem() != hItemDrop)
                     {
                        bInsertInChapter = true;
                        SendMoveMessage(hItemDrag, GetParentItem(hItemDrop));
                     }
					 else
						if(GetRootItem() != hItemDrop)
							SendMoveMessage(hItemDrag, GetPrevVisibleItem(hItemDrop));
				}
				  else
					 if(m_bBeforeTree == false && m_bAfterTree == false && imageDrag == 0 && imageDrop == 0)
					 {
						if(GetParentItem(hItemDrag))
							bInsertInChapter = true;
						SendMoveMessage(hItemDrag, hItemDrop);
					 }      
	}			
				
	// drag item is a directory with children
	
   if(imageDrag==0 && GetChildItem(hItemDrag) != NULL/* && GetParentItem(hItemDrag) != NULL*/)
	{
      if(m_bAfterTree == true && imageDrag == 0)
	  {
	     if(imageDrag == 0 && imageDrop == 0)
			if(!GetParentItem(hItemDrop) && GetParentItem(hItemDrag))
				bInsertInChapter = true;
		 SendMoveMessage(hItemDrag, hItemDrop);     
	  }
		else
			if(m_bBeforeTree == true && imageDrag == 0 && imageDrop == 1)
			{
				if(GetPrevVisibleItem(hItemDrop) == GetParentItem(hItemDrop))
				{
					bInsertInChapter = true;
					SendMoveMessage(hItemDrag, GetParentItem(hItemDrop));
				}
				else
					SendMoveMessage(hItemDrag, GetNextItem(hItemDrop,TVGN_PREVIOUS));
			}
			else
				if(m_bBeforeTree == true && imageDrag == 0 && imageDrop == 0) 
				{
					if((!GetParentItem(hItemDrag) && !GetParentItem(hItemDrop))  ||  GetRootItem() == hItemDrop/* && GetRootItem() != hItemDrop*/)
					{
						if(GetRootItem() == hItemDrop)
						{
							bInsertFirst = true;
							SendMoveMessage(hItemDrag, hItemDrop); 
						}
						else
						//bInsertInChapter = true;
						//SendMoveMessage(hItemDrag, hItemDrop);
						SendMoveMessage(hItemDrag, GetNextItem(hItemDrop,TVGN_PREVIOUS)); 
					}
					else
						if(!GetParentItem(GetPrevVisibleItem(hItemDrop)) && GetRootItem() != hItemDrop)
						{
							bInsertInChapter = true;
							SendMoveMessage(hItemDrag, GetParentItem(hItemDrop));
						}
						else 
							if(GetRootItem() != hItemDrop)
								SendMoveMessage(hItemDrag, GetPrevVisibleItem(hItemDrop));
				}
        		else
					if(m_bBeforeTree == false && m_bAfterTree == false && imageDrag == 0 && imageDrop == 0)
					{
						if(GetParentItem(hItemDrag))
							bInsertInChapter = true;
						SendMoveMessage(hItemDrag, hItemDrop);
					}        						
	}

	bInsertInChapter = false;
	bInsertFirst = false;
	FinishDragging(TRUE);
	RedrawWindow();
	
	
	
   return;									
}

void CDocumentStructureTree::SendMoveMessage(HTREEITEM hItemDrag, HTREEITEM hItemDrop)
{
   ASSISTANT::GenericContainer *pContainerDrag = (ASSISTANT::GenericContainer *)GetItemData(hItemDrag);
   UINT nIdDrag = pContainerDrag->GetID();
   ASSISTANT::GenericContainer *pContainerDrop = (ASSISTANT::GenericContainer *)GetItemData(hItemDrop);
   UINT nIdDrop = pContainerDrop->GetID();
   CWnd *pParent = GetParent();
   if (pParent)
      pParent->SendMessage(WM_MOVE_ITEM, (WPARAM)nIdDrag, (LPARAM)nIdDrop);  
}


void CDocumentStructureTree::FinishDragging(BOOL bDraggingImageList)
{
	if (m_hItemDrag != NULL)
	{
		if (bDraggingImageList)
		{
			CImageList::DragLeave(NULL);
			CImageList::EndDrag();
		}
		ReleaseCapture();
		ShowCursor(TRUE);
		
		SelectItem(NULL);            
		m_hItemDrag = NULL;		
	}
    m_bEndDrag = true;
}


void CDocumentStructureTree::OnContextMenu(CWnd* pWnd, CPoint point) 
{

   UINT		flags;
	HTREEITEM	hitem= HitTest(point, &flags);
   
	if(hitem != NULL )
	{  
      CMenu menu;
		CMenu *submenu;
      SelectItem(hitem);
      SetFocus();
	   int image,selected;
	   
      GetItemImage(hitem, image, selected);
		
		menu.LoadMenu(IDR_DOCUMENT_STRUCTURE);  
		submenu = menu.GetSubMenu(0);
		if(GetChildItem(hitem)!=0 || image==0) menu.EnableMenuItem(ID_INSERT_KEYWORDS,true);
		ClientToScreen(&point);   
		if(submenu)
         submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	
}


void CDocumentStructureTree::OnRenameItem()
{

   CEdit* editRename = EditLabel(GetSelectedItem());

}


void CDocumentStructureTree::OnDeleteItem()
{
   
   HTREEITEM hSelectedItem;

   hSelectedItem = GetSelectedItem();

   ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(hSelectedItem);
   UINT nId = pContainer->GetID();
   
   CWnd *pParent = GetParent();
   if (pParent)
      pParent->SendMessage(WM_SELECTION_DELETE, (WPARAM)nId, 0);
   
}

void CDocumentStructureTree::OnSavePageChapter()
{
   HTREEITEM	hitem;
   
   hitem = GetSelectedItem();
   
   CString csFilter;
	csFilter.LoadString(IDS_LSD_FILTER);
   
   CFileDialog fileDlg (FALSE, _T("LSD"), GetItemText(hitem), OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, csFilter, this);   
        
   UINT nReturn = fileDlg.DoModal();

   if (nReturn != IDCANCEL)
   {
      HTREEITEM hSelectedItem = GetSelectedItem();
      ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(hSelectedItem);
      UINT nId = pContainer->GetID();
      CWnd *pParent = GetParent();
      if (pParent)
         pParent->SendMessage(WM_SELECTION_SAVE, (WPARAM)nId, (LPARAM)((LPCTSTR)fileDlg.GetPathName()));
   }
   
}

void CDocumentStructureTree::OnInsertKeywords()
{
   HTREEITEM hSelectedItem = GetSelectedItem();
   

   ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(hSelectedItem);

	CDlgKeywords dlgKeywords(this);
   CString csName = pContainer->GetName();
	dlgKeywords.SetTitle(csName);
   CString csKeywords;
   pContainer->GetKeywords(csKeywords);
   dlgKeywords.SetKeywords(csKeywords);
   
	int rc = dlgKeywords.DoModal();

	if(rc == IDOK)
   {
      CString csName = dlgKeywords.GetTitle();
      pContainer->SetName(csName);
      CString csKeywords = dlgKeywords.GetKeywords();
      pContainer->SetKeywords(csKeywords);
	}
}

void CDocumentStructureTree::OnInsertPage()
{  
   HTREEITEM hSelectedItem;

   hSelectedItem = GetSelectedItem();
   //AfxMessageBox(GetItemText(GetSelectedItem()));
   ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(hSelectedItem);
   UINT nId = pContainer->GetID();
   CWnd *pParent = GetParent();
   if (pParent)
      pParent->SendMessage(WM_INSERT_PAGE, (WPARAM)nId, 0);
}

void CDocumentStructureTree::OnInsertChapter()
{
   HTREEITEM hSelectedItem;

   hSelectedItem = GetSelectedItem();
   
   ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(hSelectedItem);
   UINT nId = pContainer->GetID();
   CWnd *pParent = GetParent();
   if (pParent)
      pParent->SendMessage(WM_INSERT_CHAPTER, (WPARAM)nId, 0);
   
}

void CDocumentStructureTree::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
   HWND hWnd = (HWND)SendMessage (LVM_GETEDITCONTROL);
   
   TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

   m_hItemEdit = GetSelectedItem();
	m_csItemText = GetItemText(m_hItemEdit);
   
   *pResult = 0; 
}

void CDocumentStructureTree::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
   TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
   
   CEdit *editLabel = GetEditControl();
   CString csName;
   editLabel->GetWindowText(csName);
   
   SetItemText(m_hItemEdit, csName);
   ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(m_hItemEdit);
   pContainer->SetName(csName);

   *pResult = 0;
}

void CDocumentStructureTree::FindName(CTreeCtrl& tree, CString name, HTREEITEM hItem)
{
	
	HTREEITEM hItemChild = tree.GetChildItem(hItem);
	
	if(name==tree.GetItemText(hItem))
	{
		m_bFileNameExists = true;
		return;
	}
	
	while (hItemChild != NULL)
	{
		if(name==tree.GetItemText(hItemChild)) 
		{
			m_bFileNameExists = true;
			return;
		}
		else
		{
			HTREEITEM hItemNextChild = tree.GetNextSiblingItem(hItemChild);
			FindName(tree,name,hItemChild);
			hItemChild = hItemNextChild;
		}      
	}   
}

void CDocumentStructureTree::SetLastId(UINT nID)
{
   m_nLastId = nID;   
}

UINT CDocumentStructureTree::GetLastId()
{
   return m_nLastId; 
}


void CDocumentStructureTree::OnSelectionChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
   
   HTREEITEM hItem = pNMTreeView->itemOld.hItem;
   HTREEITEM hItemNew = pNMTreeView->itemNew.hItem; 

   if(m_bEndDrag)
   {
	  hItem = pNMTreeView->itemNew.hItem;
	  m_bEndDrag = false;
   }

   if(hItem != NULL && m_hItemDrag == NULL)
   {   
      HTREEITEM hSelectedItem = hItemNew;
      ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(hSelectedItem);
      UINT nId = pContainer->GetID();
      CWnd *pParent = GetParent();
      if (pParent)
         pParent->SendMessage(WM_SELECTION_CHANGED, (WPARAM)nId, 0);
   }
   
    
   /*if(hItemNew != NULL && GetChildItem(hItemNew) != NULL)
	   {  
//         m_hItemChapter = hItemNew;
         HTREEITEM hitemSelected = GetChildItem(hItemNew);
         
         ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(hitemSelected);
         UINT nId = pContainer->GetID();
         CWnd *pParent = GetParent();
         if (pParent)
            pParent->SendMessage(WM_SELECTION_CHANGED, (WPARAM)nId, 0);
         
         //SelectItem(hitemSelected);
         //SetFocus();
      }*/
   *pResult = 0;
}

void CDocumentStructureTree::SetSelection(UINT nID)
{
   HTREEITEM hItem = GetChildItem(GetRootItem()); 
   
   ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(GetRootItem());
   UINT nRootId = pContainer->GetID();

   if(nID == nRootId) 
      m_hItemId = GetRootItem(); 
   
   bool bFound = false;
   while(!bFound && hItem != NULL && m_hItemId==NULL)
   {
      bFound = FindItemId(nID, hItem);
      if(!bFound)
         hItem = GetNextItem(hItem, TVGN_NEXT);
   }
   
   if (m_hItemId != NULL) 
      SelectItem(m_hItemId);
   m_hItemId=NULL;  
}

UINT CDocumentStructureTree::GetSelectionId()
{
   HTREEITEM hSelectedItem = GetSelectedItem();
   ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(hSelectedItem);
   UINT nId = pContainer->GetID();

   return nId;
}

HTREEITEM CDocumentStructureTree::FindItem(UINT nID, HTREEITEM hParentItem)
{
   
   HTREEITEM hItem = NULL;
   
   if (hParentItem == NULL)
      hItem = GetRootItem(); 
   else
      hItem = GetChildItem(hParentItem);
   
   while (hItem != NULL)
   {
      ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(hItem);
      UINT nItemId = pContainer->GetID();
      if (nItemId == nID)
         return hItem;
      
      HTREEITEM hFound = FindItem(nID, hItem);

      if (hFound)
         return hFound;

      hItem = GetNextItem(hItem, TVGN_NEXT);
   }
   
   return NULL;
}

bool CDocumentStructureTree::FindItemId(UINT nID, HTREEITEM hItem)
{
   
   HTREEITEM hItemChild = GetChildItem(hItem);
   
   ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(hItem);
   UINT nItemId = pContainer->GetID();

   if (nID == nItemId)
   {
      m_hItemId = hItem; 
      return true;
   }
    
   bool bFound = false;
   while (!bFound && hItemChild != NULL)
   {
      ASSISTANT::GenericContainer *pContainer = (ASSISTANT::GenericContainer *)GetItemData(hItemChild);
      UINT nChildId = pContainer->GetID();
      if (nID == nChildId) 
      {	
         m_hItemId = hItemChild; 
         bFound = true;
      }
      else
      {
         HTREEITEM hItemNextChild = GetNextSiblingItem(hItemChild);
         bFound = FindItemId(nID,hItemChild);
         hItemChild = hItemNextChild;       
      }      
   }
   return bFound;
}

void CDocumentStructureTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   
   switch (nChar)
	{
		
		case VK_LEFT :
		{
         CEdit *edit = GetEditControl();
         if(edit)
            edit->SendMessage(WM_KEYDOWN, VK_LEFT); 
         break;
		}

      case VK_RIGHT :
		{
         CEdit *edit = GetEditControl();
         if(edit)
            edit->SendMessage(WM_KEYDOWN, VK_RIGHT);
			break;
		}
      case VK_UP :
		{
         CEdit *edit = GetEditControl();
         if(edit)
            break;
		}

	  case VK_DOWN:
		  {
			  CEdit *edit = GetEditControl();
			  if(edit)
				  break;
		  }

	  case VK_SPACE:
		  {
			  CEdit *edit = GetEditControl();
			  if(edit)
				  edit->SendMessage(WM_CHAR, VK_SPACE);
			  break;
		  }

	  case VK_BACK :
		  {
			  CEdit *edit = GetEditControl();
			  if(edit)
				  edit->SendMessage(WM_CHAR, VK_BACK); 
			  break;
		  }
	  case VK_DELETE :
		  {
			  CEdit *edit = GetEditControl();
			  if(edit)
				  edit->SendMessage(WM_KEYDOWN, VK_DELETE); 
			  break;
		  }

/*
      case VK_NEXT :
		{
         SendMessage(WM_KEYDOWN, VK_DOWN);
			break;
		}

      case VK_PRIOR :
		{
         SendMessage(WM_KEYDOWN, VK_UP);
			break;
		}*/

		default :
		{
			CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
    }	
}

