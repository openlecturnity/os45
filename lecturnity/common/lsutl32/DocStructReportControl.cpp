#include "stdafx.h"
#include "resource.h"
#include "DocStructReportControl.h"
#include "DocStructRecord.h"
#include "..\..\Assistant2_0\backend\mlb_generic.h"
#include "DlgKeywords.h"
#include "DocumentStructure.h"
#include "MiscFunctions.h"

CDocStructReportControl::CDocStructReportControl() {
    m_pDragRow = NULL;
    m_pDropRow = NULL;
    m_pMouseOverRow = NULL;
    m_bDroppedInFolder = false;
    m_bFolderSelected = false;
    m_nIdDrag = -1;
}

CDocStructReportControl::~CDocStructReportControl() {
    m_pDragRow = NULL;
    m_pDropRow = NULL;
    m_pMouseOverRow = NULL;
}

BEGIN_MESSAGE_MAP(CDocStructReportControl, CXTPReportControl)
    ON_NOTIFY_REFLECT(XTP_NM_REPORT_DRAGDROP_COMPLETED, OnDragDropCompleted)
    ON_NOTIFY_REFLECT(XTP_NM_REPORT_SELCHANGED, OnSelectionChanged)
    ON_WM_CONTEXTMENU()
    ON_WM_LBUTTONDOWN()
    ON_COMMAND(ID_LOAD_LSD, OnLoadItem)
    ON_COMMAND(ID_LSD_CLOSE, OnCloseItem)
    ON_COMMAND(ID_REMOVE_ENTRY, OnRemoveItem)
    ON_COMMAND(ID_RENAME, OnRenameItem)
    ON_COMMAND(ID_DELETE, OnDeleteItem)
    ON_COMMAND(ID_SAVE_PAGE_CHAPTER, OnSavePageChapter)
    ON_COMMAND(ID_INSERT_KEYWORDS, OnInsertKeywords)
    ON_COMMAND(ID_INSERT_PAGE, OnInsertPage)
    ON_COMMAND(ID_INSERT_CHAPTER, OnInsertChapter)
END_MESSAGE_MAP()

DROPEFFECT CDocStructReportControl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point, int nState) {
    DROPEFFECT dropEffect = CXTPReportControl::OnDragOver(pDataObject, dwKeyState, point, nState);
    CXTPReportRow* pRow = HitTest(point);

    if (pRow && m_pDragRow) {
        bool bIsDropChildDrag = false;

        CXTPReportRow* pParentRow = pRow->GetParentRow();
        while (pParentRow != NULL) {
            if (pParentRow == m_pDragRow) {
                bIsDropChildDrag = true;
                break;
            }
            pParentRow = pParentRow->GetParentRow();
        }

        bool bFolderSelected = (pRow->GetRect().bottom-6>point.y) && (pRow->GetRect().top+6<point.y);

        if (pRow == m_pDragRow || (pRow->GetParentRow()!=NULL && m_pDragRow->GetParentRow()==NULL)
            || (pRow->GetParentRow()==NULL && m_pDragRow->GetParentRow()==NULL && bFolderSelected)
            || (m_pDragRow->GetParentRow() && pRow->GetParentRow()==NULL && !bFolderSelected)
            || bIsDropChildDrag) {
                dropEffect = DROPEFFECT_NONE;
        }
    }

    if (dropEffect != DROPEFFECT_NONE) {
        if (pRow && pRow->GetRecord()) {
            int nTypeDrop = ((CDocStructRecord*)pRow->GetRecord())->m_pContainer->GetType(((CDocStructRecord*)pRow->GetRecord())->m_pvData);

            if (nTypeDrop == ASSISTANT::GenericContainer::DOCUMENT || nTypeDrop == ASSISTANT::GenericContainer::FOLDER) {
                if ((pRow->GetRect().bottom-6>point.y) && (pRow->GetRect().top+6<point.y)) {
                    m_nDropPos = -1;
                    m_nFocusedRow = pRow->GetIndex();
                    pRow->SetSelected(TRUE);
                    m_bFolderSelected = true;
                } else if (m_bFolderSelected) {
                    pRow->SetSelected(FALSE);
                    m_nFocusedRow = -1;
                    m_bFolderSelected = false;
                }
            }
            m_pMouseOverRow = pRow;
            RedrawControl();
        } else {
            m_pMouseOverRow = NULL;
        }
    } else {
        m_nDropPos = -1;
    }

    return dropEffect;
}

void CDocStructReportControl::DrawDropMarker(CDC* pDC) {
    if (m_pMouseOverRow) {
        int offSet = 0;
        int nTreeDepth = m_pMouseOverRow->GetTreeDepth();
        int nLeftIndent = GetIndent(nTreeDepth);

        CRect rcRow(m_rcReportArea.left, m_nDropPos, m_rcReportArea.right, m_nDropPos + 1);

        if (((CDocStructRecord*)m_pMouseOverRow->GetRecord())->m_nCurrentLayout == DocumentStructureLayout(Compact)) {
            nTreeDepth == 0 ? offSet = 15 : offSet = 37;
        } else {
            offSet = 0;
        }

        rcRow.left += nLeftIndent + offSet;

        pDC->FillSolidRect(rcRow, GetPaintManager()->m_clrHotDivider);

        CXTPPenDC pen (*pDC, GetPaintManager()->m_clrHotDivider);
        CXTPBrushDC brush (*pDC, GetPaintManager()->m_clrHotDivider);

        int x = rcRow.left;
        int y = m_nDropPos;

        POINT ptsLeftArrow[] =
        {
            {x, y - 2}, {x + 2, y - 2}, {x + 2, y - 5}, {x + 7, y},
            {x + 2, y + 5}, {x + 2, y + 2}, {x, y + 2}
        };
        pDC->Polygon(ptsLeftArrow, 7);

        x = rcRow.right - 1;

        POINT ptsRightArrow[] =
        {
            {x, y - 2}, {x - 2, y - 2}, {x - 2, y - 5}, {x - 7, y},
            {x - 2, y + 5}, {x - 2, y + 2}, {x, y + 2}
        };
        pDC->Polygon(ptsRightArrow, 7);
    }
}

void CDocStructReportControl::OnBeginDrag(CPoint point) {
    m_pDragRow = HitTest(point);
    CXTPReportControl::OnBeginDrag(point);
}

BOOL CDocStructReportControl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) {
    m_nDropPos = -1;
    m_pDropRow = HitTest(point);

    if (!m_pDragRow || !m_pDropRow || dropEffect == DROPEFFECT_NONE) {
        return FALSE;
    }

    UINT nIdDrag = ((CDocStructRecord*)m_pDragRow->GetRecord())->m_pContainer->GetID(((CDocStructRecord*)m_pDragRow->GetRecord())->m_pvData);
    m_nIdDrag = nIdDrag;
    UINT nIdDrop = ((CDocStructRecord*)m_pDropRow->GetRecord())->m_pContainer->GetID(((CDocStructRecord*)m_pDropRow->GetRecord())->m_pvData);

    BOOL bAbove = TRUE;
    bAbove = m_pDropRow->GetRect().CenterPoint().y < point.y ? FALSE : TRUE;

    CWnd *pWnd = GetParent();

    if (!bAbove && !m_bFolderSelected) {
        pWnd->SendMessage(WM_MOVE_ITEM, (WPARAM)nIdDrag, (LPARAM)nIdDrop);
    } else if (m_bFolderSelected) {
        m_bDroppedInFolder = true;
        pWnd->SendMessage(WM_MOVE_ITEM, (WPARAM)nIdDrag, (LPARAM)nIdDrop);
    } else {
        CXTPReportRow* pPrevDropRow = GetRows()->GetPrev((CXTPReportRow *)m_pDropRow, true); 
        CDocStructRecord* pCurrRecord;

        if (pPrevDropRow == m_pDropRow->GetParentRow() || pPrevDropRow->GetParentRow() == NULL) {
            m_bDroppedInFolder = true;
            pCurrRecord = (CDocStructRecord*)pPrevDropRow->GetRecord();
        } else if (pPrevDropRow->GetParentRow() != m_pDropRow->GetParentRow()) {
            pCurrRecord = (CDocStructRecord*)pPrevDropRow->GetParentRow()->GetRecord();
        } else {
            pCurrRecord = (CDocStructRecord*)pPrevDropRow->GetRecord();
        }

        UINT nIdDropPreview = pCurrRecord->m_pContainer->GetID(pCurrRecord->m_pvData);

        if (nIdDropPreview != nIdDrag) {
            pWnd->SendMessage(WM_MOVE_ITEM, (WPARAM)nIdDrag, (LPARAM)nIdDropPreview);
        }
    }

    m_bDroppedInFolder = false;

    return FALSE;
}

bool CDocStructReportControl::IsDroppedInFolder() {
    return m_bDroppedInFolder;
}

void CDocStructReportControl::OnLButtonDown(UINT nFlags, CPoint point) {
    CXTPReportControl::OnLButtonDown(nFlags, point);

    CXTPReportRow* pRow = HitTest(point);
    if (pRow) {
        CDocStructRecord* pRecord = (CDocStructRecord*)pRow->GetRecord();
        if (pRecord->IsDocument() && !pRecord->IsLoadedDocument()) {
            CXTPReportPaintManager* pPaintManager = GetPaintManager();
            CRect rcRow;
            rcRow = pRow->GetRect();
            CSize sizeBitmap = pPaintManager->DrawCollapsedBitmap(NULL, pRow, rcRow);
            rcRow.right = rcRow.left + sizeBitmap.cx;
            if (rcRow.PtInRect(point)) {
                pRecord->m_pContainer->LoadDocument(pRecord->m_pvData);
            }
        }
    }
}

void CDocStructReportControl::OnDragDropCompleted(NMHDR* pNMHDR, LRESULT* pResult) {
    XTP_NM_REPORTDRAGDROP *nmData =( XTP_NM_REPORTDRAGDROP *)pNMHDR;

    CXTPReportRow* pTargetRow = NULL;

    for (int i = 0; i < GetRows()->GetCount(); i++) {
        CDocStructRecord* pTargetRecord = (CDocStructRecord*)GetRows()->GetAt(i)->GetRecord();
        if (pTargetRecord->m_pContainer->GetID(pTargetRecord->m_pvData) == m_nIdDrag) {
            pTargetRow = GetRows()->GetAt(i);
            break;
        }
    }
    if (pTargetRow != NULL) {
        SetFocusedRow(pTargetRow);
    }
    m_pDragRow = NULL;
    m_pDropRow = NULL;
}

void CDocStructReportControl::OnSelectionChanged(NMHDR* pNMHDR, LRESULT* pResult) {
    if (GetSelectedRows()->GetCount() != 1) {
        return;
    }

    CDocStructRecord* pRecord = (CDocStructRecord*)GetSelectedRows()->GetAt(0)->GetRecord();

    if (GetSelectedRows()->GetAt(0)->HasChildren()) {
        CXTPReportRow * pRow = GetRows()->GetNext(GetSelectedRows()->GetAt(0), true);
        pRecord = (CDocStructRecord*) pRow->GetRecord();
    }

    if (pRecord == NULL) {
        return;
    }

    UINT nId = pRecord->m_pContainer->GetID(pRecord->m_pvData);
    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(WM_SELECTION_CHANGED, (WPARAM)nId, 0);
    }

    *pResult = 0;
}

void CDocStructReportControl::OnContextMenu(CWnd* pWnd, CPoint point) {
    ScreenToClient(&point);
    CXTPReportRow* pRow = HitTest(point);
    if (pRow == NULL) {
        return;
    }

    SetFocusedRow(pRow);

    CMenu menu;
    menu.LoadMenu(IDR_DOCUMENT_STRUCTURE); 

    CMenu *submenu;
    submenu = menu.GetSubMenu(0);

    CDocStructRecord* pRecord = (CDocStructRecord*)pRow->GetRecord();
    if (pRecord == NULL) {
        return;
    }

    if (pRecord->IsDocument()) {
        // Delete menu entries belongs only to page/chapter
        menu.RemoveMenu(ID_DELETE, MF_BYCOMMAND);
        menu.RemoveMenu(ID_SAVE_PAGE_CHAPTER, MF_BYCOMMAND);
        menu.RemoveMenu(ID_INSERT_KEYWORDS, MF_BYCOMMAND);
    } else {
        // Delete menu entries belongs only to document
        menu.RemoveMenu(ID_REMOVE_ENTRY, MF_BYCOMMAND);
    }

    ClientToScreen(&point);   
    if (submenu) {
        submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
    }
}

void CDocStructReportControl::OnLoadItem() {
    CXTPReportRow* pRow = GetFocusedRow();
    if (pRow) {
        CDocStructRecord* pRecord = (CDocStructRecord*)pRow->GetRecord();

        UINT nId = pRecord->m_pContainer->GetID(pRecord->m_pvData);

        CWnd *pParent = GetParent();
        if (pParent) {
            pParent->SendMessage(WM_SELECTION_LOAD, (WPARAM)nId, 0);
        }
    }
}

void CDocStructReportControl::OnCloseItem() {
    CXTPReportRow* pRow = GetFocusedRow();
    if (pRow) {
        CDocStructRecord* pRecord = (CDocStructRecord*)pRow->GetRecord();

        UINT nId = pRecord->m_pContainer->GetID(pRecord->m_pvData);

        CWnd *pParent = GetParent();
        if (pParent) {
            pParent->SendMessage(WM_SELECTION_CLOSE, (WPARAM)nId, 0);
        }
    }
}

void CDocStructReportControl::OnRemoveItem() {
    CXTPReportRow* pRow = GetFocusedRow();
    if (pRow) {
        CDocStructRecord* pRecord = (CDocStructRecord*)pRow->GetRecord();

        UINT nId = pRecord->m_pContainer->GetID( pRecord->m_pvData);

        CWnd *pParent = GetParent();
        if (pParent) {
            pParent->SendMessage(WM_SELECTION_REMOVE, (WPARAM)nId, 0);
        }
    }
}

void CDocStructReportControl::OnRenameItem() {
    CXTPReportRow* pRow = GetFocusedRow();
    if (pRow == NULL) {
        return;
    }
    XTP_REPORTRECORDITEM_ARGS itemArgs(this, pRow, GetColumns()->GetAt(0)) ;
    EditItem(&itemArgs);
    GetInplaceEdit()->SetSel(0, -1);
}

void CDocStructReportControl::OnDeleteItem() { 
    CXTPReportRow* pRow = GetFocusedRow();
    if (pRow == NULL) {
        return;
    }
    CDocStructRecord* pRecord = (CDocStructRecord*)pRow->GetRecord();
    if (pRecord == NULL) {
        return;
    }
    UINT nId = pRecord->m_pContainer->GetID(pRecord->m_pvData);
    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(WM_SELECTION_DELETE, (WPARAM)nId, 0);
    }
}

void CDocStructReportControl::OnSavePageChapter() {
    CXTPReportRow* pRow = GetFocusedRow();
    if (pRow == NULL) {
        return;
    }
    CString csFilter;
    csFilter.LoadString(IDS_LSD_FILTER);

    CDocStructRecord* pRecord = (CDocStructRecord*)pRow->GetRecord();
    if (pRecord == NULL) {
        return;
    }
    CString csCaption;
    csCaption = pRecord->GetItem(0)->GetCaption(GetColumns()->GetAt(0));
    // remove any filename reserved characters from the new recording file
    StringManipulation::RemoveReservedFilenameChars(csCaption);
    CFileDialog fileDlg (FALSE, _T("LSD"), csCaption, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, csFilter, this);   

    UINT nReturn = fileDlg.DoModal();
    if (nReturn != IDCANCEL) {
        UINT nId = pRecord->m_pContainer->GetID(pRecord->m_pvData);
        CWnd *pParent = GetParent();
        if (pParent) {
            pParent->SendMessage(WM_SELECTION_SAVE, (WPARAM)nId, (LPARAM)((LPCTSTR)fileDlg.GetPathName()));
        }
    }
}

void CDocStructReportControl::OnInsertKeywords() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CXTPReportRow* pRow = GetFocusedRow();
    if (pRow) {
        CDocStructRecord* pRecord = (CDocStructRecord*)pRow->GetRecord();
        CString csPageName = pRecord->m_pContainer->GetName(pRecord->m_pvData);

        CDlgKeywords dlgKeywords(this);
        CString csName = pRecord->m_pContainer->GetName(pRecord->m_pvData);
        dlgKeywords.SetTitle(csName);
        CString csKeywords;
        pRecord->m_pContainer->GetKeywords(pRecord->m_pvData, csKeywords);
        dlgKeywords.SetKeywords(csKeywords);

        int rc = dlgKeywords.DoModal();

        if (rc == IDOK) {
            CString csName = dlgKeywords.GetTitle();
            pRecord->m_pContainer->SetName(pRecord->m_pvData, csName);
            CString csKeywords = dlgKeywords.GetKeywords();
            pRecord->m_pContainer->SetKeywords(pRecord->m_pvData, csKeywords);

            UINT nId = pRecord->m_pContainer->GetID(pRecord->m_pvData);

            CWnd *pParent = GetParent();
            if (pParent) {
                pParent->SendMessage(WM_SELECTION_CHANGED, (WPARAM)nId, (LPARAM)true);
            }
        }
    }
}

void CDocStructReportControl::OnInsertPage() {  
    CXTPReportRow* pRow = GetFocusedRow();
    if (pRow) {
        CDocStructRecord* pRecord = (CDocStructRecord*)pRow->GetRecord();

        UINT nId = pRecord->m_pContainer->GetID(pRecord->m_pvData);

        CWnd *pParent = GetParent();
        if (pParent) {
            pParent->SendMessage(WM_INSERT_PAGE, (WPARAM)nId, 0);
        }
    }
}

void CDocStructReportControl::OnInsertChapter() {
    CXTPReportRow* pRow = GetFocusedRow();
    if (pRow) {
        CDocStructRecord* pRecord = (CDocStructRecord*)pRow->GetRecord();

        UINT nId = pRecord->m_pContainer->GetID(pRecord->m_pvData);

        CWnd *pParent = GetParent();
        if (pParent) {
            pParent->SendMessage(WM_INSERT_CHAPTER, (WPARAM)nId, 0);
        }
    }
}