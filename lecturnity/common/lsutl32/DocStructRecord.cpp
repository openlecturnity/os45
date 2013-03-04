#include "stdafx.h"
#include "DocStructRecord.h"
#include "..\..\Assistant2_0\backend\mlb_generic.h"
#include "..\..\Assistant2_0\backend\la_document.h"
#include "DlgKeywords.h"
#include "languagesupport.h"
#include "DocumentStructure.h"

#ifdef _STUDIO
#include "..\Studio\Studio.h"
#endif

#define TREE_DEPTH_OFFSET 8
#define RECORD_TEXT_OFFSET 20
#define RECORD_ITEM_OFFSET 10

int CDocStructRecord::s_PAGE = 0;
int CDocStructRecord::s_CHAPTER = 1;
int CDocStructRecord::s_DOCUMENT = 2;

///////////////////////////////////////////////////////////////////////////////////
// class CDocStructRecord implementation
///////////////////////////////////////////////////////////////////////////////////
CDocStructRecord::CDocStructRecord(CDocumentStructure* pContainer, void* pvData, bool bIsExpanded, int nCurrentLayout, UINT nImageIndex, bool bEdtiOnDblClick) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    m_pContainer = pContainer;
    m_pvData = pvData;
    m_bIsExpanded = bIsExpanded;
    m_nCurrentLayout = nCurrentLayout;

    m_pItemText = (CDocStructRecordItem*)AddItem(new CDocStructRecordItem(pContainer->GetName(m_pvData), pContainer->GetType(m_pvData), bEdtiOnDblClick));
    UpdateTooltip();
    m_pItemText->m_nImageIndex = nImageIndex;
}

CDocStructRecord::~CDocStructRecord() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

void CDocStructRecord::DoPropExchange(CXTPPropExchange* pPX) {
    CXTPReportRecord::DoPropExchange(pPX);
}

void CDocStructRecord::UpdateTooltip() {
    AFX_MANAGE_STATE(AfxGetAppModuleState());  
    if (IsDocument()) {
        ASSISTANT::Document *pDocument = (ASSISTANT::Document *)m_pvData;
        CString csDefaultName;
        csDefaultName.LoadString(IDS_DEFAULT_DOCUMENT_NAME);
        if(pDocument->GetDocumentName() == csDefaultName) {
            csDefaultName.LoadString(IDS_NEW_LSD);
            m_pItemText->SetTooltip(csDefaultName);
        } else {
            m_pItemText->SetTooltip(pDocument->GetDocumentPath() + _T("\\") + pDocument->GetDocumentName());
        }
    } else {
        m_pItemText->SetTooltip(m_pContainer->GetName(m_pvData));
    }
}

bool CDocStructRecord::IsPage() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return m_pContainer->GetType(m_pvData) == s_PAGE/*ASSISTANT::GenericContainer::PAGE*/;
}

bool CDocStructRecord::IsChapter() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return m_pContainer->GetType(m_pvData) == s_CHAPTER/*ASSISTANT::GenericContainer::PAGE*/;
}
bool CDocStructRecord::IsDocument() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return m_pContainer->GetType(m_pvData) == s_DOCUMENT/*ASSISTANT::GenericContainer::DOCUMENT*/;
}

bool CDocStructRecord::IsLoadedDocument() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    bool bIsLoadedDocument = false;

    if (m_pContainer->GetType(m_pvData) == s_DOCUMENT/*ASSISTANT::GenericContainer::DOCUMENT*/) {
        ASSISTANT::Document *pDocument = (ASSISTANT::Document *)m_pvData;
        if (pDocument->IsLoaded()) {
            bIsLoadedDocument = true;
        }
    }

    return bIsLoadedDocument;
}

bool CDocStructRecord::IsEmptyDocument() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    bool bIsEmptyDocument = false;

    if (m_pContainer->GetType(m_pvData) == s_DOCUMENT/*ASSISTANT::GenericContainer::DOCUMENT*/) {
        ASSISTANT::Document *pDocument = (ASSISTANT::Document *)m_pvData;
        if (pDocument->IsEmpty()) {
            bIsEmptyDocument = true;
        }
    }

    return bIsEmptyDocument;
}

///////////////////////////////////////////////////////////////////////////////////
// class CXTPDocumentStructurePaintManager implementation
///////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPDocumentStructurePaintManager, CXTPReportPaintManager);

CXTPDocumentStructurePaintManager::CXTPDocumentStructurePaintManager() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (m_pGlyphs != NULL) {
        CMDTARGET_RELEASE(m_pGlyphs);
    }

    CBitmap bmp;
    CXTPResourceManager::AssertValid(XTPResourceManager()->LoadBitmap(&bmp, XTP_IDB_REPORT_GLYPHS_INTERACTION));

    m_pGlyphs = new CXTPImageManager();
    CXTPTempColorMask mask(m_pGlyphs, RGB(255, 0, 255));
    m_pGlyphs->SetIcons(bmp, 0, 8, CSize(0, 0));
}

CXTPDocumentStructurePaintManager::~CXTPDocumentStructurePaintManager() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

int CXTPDocumentStructurePaintManager::GetRowHeight(CDC* pDC, CXTPReportRow* pRow) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (pRow == NULL) {
        ASSERT(false);
        return CXTPReportPaintManager::GetRowHeight(pDC, pRow);;
    }

    if (pRow->GetControl() != NULL
        && pRow->GetRecord() != NULL
        && pRow->GetRecord()->GetRuntimeClass() == RUNTIME_CLASS(CDocStructRecord)) {
        CRect rcControl;
        pRow->GetControl()->GetClientRect(rcControl);

        CDocStructRecord *pDocStrRecord = (CDocStructRecord*) pRow->GetRecord();
        if (pDocStrRecord != NULL && pDocStrRecord->m_nCurrentLayout == DocumentStructureLayout(Compact)) {
            return RECORD_TEXT_OFFSET;
        }
        if (pDocStrRecord && pDocStrRecord->IsPage()) {
            return (rcControl.Width() - pRow->GetTreeDepth()*TREE_DEPTH_OFFSET*2)* 3/4 + RECORD_TEXT_OFFSET;//115;
        } else {// is chapter
            return 40;
        }
    }

    ASSERT(FALSE);
    return CXTPReportPaintManager::GetRowHeight(pDC, pRow);
}

CSize CXTPDocumentStructurePaintManager::DrawCollapsedBitmap(CDC* pDC, const CXTPReportRow* pRow, CRect& rcBitmap) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    rcBitmap.left += 2;
    CSize sizeGlyph = DrawGlyph(pDC, rcBitmap, pRow->IsExpanded() ? 6 : 7);
    if (sizeGlyph.cx != 0 && pDC) {
        rcBitmap.right = rcBitmap.left + sizeGlyph.cx;
    }
    return CSize(2 + sizeGlyph.cx, 2 + sizeGlyph.cy);
}

CSize CXTPDocumentStructurePaintManager::DrawCustomBitmap(CDC* pDC, const CXTPReportRow* pRow, CRect& rcBitmap, UINT nImageIndex) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    rcBitmap.left += 2;
    CSize sizeBitmap = DrawBitmap(pDC, pRow->GetControl(), rcBitmap, nImageIndex);
    if (sizeBitmap.cx != 0 && pDC) {
        rcBitmap.right = rcBitmap.left + sizeBitmap.cx;
    }
    return CSize(2 + sizeBitmap.cx, 2 + sizeBitmap.cy);
}

///////////////////////////////////////////////////////////////////////////////////
// class CDocStructRecordItem implementation
///////////////////////////////////////////////////////////////////////////////////
CDocStructRecordItem::CDocStructRecordItem(CString csCaption, UINT nType, bool bEditOnDblClick) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    //bool bIsBold = true;
    //if(nType == CDocStructRecord::s_PAGE/*ASSISTANT::GenericContainer::PAGE*/)
    //   bIsBold = false;
    SetTextColor(RGB(155, 173, 198));
    SetBold(false);
    SetCaption(csCaption);
    m_csInitialCaption = csCaption;
    m_bEditOnDblClick = bEditOnDblClick;
}

CDocStructRecordItem::~CDocStructRecordItem() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

void CDocStructRecordItem::DoPropExchange(CXTPPropExchange* pPX) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CXTPReportRecordItem::DoPropExchange(pPX);
}

void CDocStructRecordItem::OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    UNREFERENCED_PARAMETER(pItemArgs);
    UNREFERENCED_PARAMETER(szText);
    CString csText(szText);
    if (csText.IsEmpty()) {
        csText = m_csInitialCaption;
    }
    SetCaption(csText);
    CDocStructRecord* pRecord = (CDocStructRecord*)GetRecord();
    if (pRecord) {
        pRecord->m_pContainer->SetName(pRecord->m_pvData, csText);
        m_csInitialCaption = csText;
        pRecord->UpdateTooltip();
    }
    int x = 0;
}

int CDocStructRecordItem::GetExpandedChildsHeight(CXTPReportControl *pReportControl, CXTPReportRecord *pRecord) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    int nHeight = 0;
    if ( pRecord->GetChilds() > 0 && pRecord->IsExpanded()) {
        for ( int i = 0; i < pRecord->GetChilds()->GetCount(); i++ ) {
            CXTPReportRow *pRow = pReportControl->GetRows()->FindInTree(pRecord->GetChilds()->GetAt(i));
            nHeight += pReportControl->GetPaintManager()->GetRowHeight(NULL, pRow );
            nHeight += GetExpandedChildsHeight(pReportControl, pRecord->GetChilds()->GetAt(i) );
        }
    }
    return nHeight;
}

int CDocStructRecordItem::GetReportAreaRows(CXTPReportControl *pReportControl, int nStartRow, BOOL bMoveDown) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    int nDirection = bMoveDown ? +1 : -1;
    int top = pReportControl->GetReportRectangle().top;
    CClientDC dc(pReportControl);
    int nHeaderWidth = pReportControl->GetReportHeader()->GetWidth();

    for (int i = nStartRow; (i < pReportControl->GetRows()->GetCount() && i >= 0); i += nDirection) {
        CXTPReportRow* pRow = pReportControl->GetRows()->GetAt(i);
        ASSERT(pRow);
        if (!pRow) {
            continue;
        }
        int rowHeight = pRow->GetHeight(&dc, nHeaderWidth);
        if (top + rowHeight > pReportControl->GetReportRectangle().bottom 
            && i + 1 <= pReportControl->GetRows()->GetCount()) {
            return bMoveDown ? i - nStartRow  + 1 : nStartRow - i + 1;
        } else if (top + rowHeight > pReportControl->GetReportRectangle().bottom) {
            return bMoveDown ? i - nStartRow : nStartRow - i;
        }
        top += rowHeight;
    }

    return bMoveDown ? pReportControl->GetRows()->GetCount() - nStartRow : nStartRow;
}

int CDocStructRecordItem::Draw(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (pDrawArgs->pDC == NULL || pDrawArgs->pControl == NULL || pDrawArgs->pRow == NULL || pDrawArgs->pItem == NULL) {
        ASSERT(FALSE);
        return 0;
    }
    int offSet = 0;
    CDC* pDC = pDrawArgs->pDC;
    int nTreeDepth = pDrawArgs->pRow->GetTreeDepth();
    int nLeftIndent = pDrawArgs->pControl->GetIndent(nTreeDepth);
    CRect rcRow = pDrawArgs->pRow->GetRect();
    nTreeDepth == 0 ? offSet = 0 : offSet = 0;
    CXTPOffice2007Image *pImage = XTPOffice2007Images()->LoadFile(_T("PUSHBUTTON"));
    ASSERT(pImage);
    int nState = 0;

    if (pDrawArgs->pRow->IsSelected() == TRUE) {
        nState = 2;
    }
    CDocStructRecord* pRecord = (CDocStructRecord*)GetRecord();
    if (pRecord) {
        int nCurrentLayout = pRecord->m_nCurrentLayout;

        if (nCurrentLayout == DocumentStructureLayout(Thumbnail)) {
            rcRow.left += nTreeDepth * TREE_DEPTH_OFFSET;
            rcRow.top += 2;
            rcRow.bottom -= 2;
            rcRow.right -= nTreeDepth * TREE_DEPTH_OFFSET;
        } else {
            rcRow.left += 0; //nLeftIndent + offSet;
        }
        if (pImage) {
            // Draw a blue rectangle that surrounds all child items to show that an item is part of a group.
            if (nCurrentLayout == DocumentStructureLayout(Thumbnail) && pDrawArgs->pRow->GetParentRow() != NULL) {
                int nTopRow = pDrawArgs->pControl->GetTopRowIndex();
                int nVisibleRows = GetReportAreaRows(pDrawArgs->pControl, pDrawArgs->pControl->GetTopRowIndex(), TRUE);
                CXTPReportRow *pTopRow = pDrawArgs->pControl->GetRows()->GetAt(nTopRow);

                if (pTopRow != NULL) {
                    if (pTopRow->GetTreeDepth() > 1 && pTopRow == pDrawArgs->pRow) {
                        for (int i = 1; i < pTopRow->GetTreeDepth(); i++) {
                            int iExpandedChildsHeight = GetExpandedChildsHeight(pDrawArgs->pControl, pDrawArgs->pRow->GetParentRow()->GetRecord());
                            CRect rcParentRect(pTopRow->GetRect());
                            bool bReset = false;
                            for (int j = nTopRow; j < nTopRow + nVisibleRows ; j++) {
                                int iRowLevel = pDrawArgs->pControl->GetRows()->GetAt(j)->GetTreeDepth();
                                if (iRowLevel >= i+1) {
                                    if (bReset == false) {
                                        iExpandedChildsHeight = 0;
                                        bReset = true;
                                    }
                                    iExpandedChildsHeight += pDrawArgs->pControl->GetPaintManager()->GetRowHeight(NULL, pDrawArgs->pControl->GetRows()->GetAt(j));
                                } else {
                                    break;
                                }
                            }
                            rcParentRect.bottom = iExpandedChildsHeight < pDrawArgs->pControl->GetReportRectangle().bottom ? iExpandedChildsHeight : pDrawArgs->pControl->GetReportRectangle().bottom + 2;
                            rcParentRect.left += i * TREE_DEPTH_OFFSET;
                            rcParentRect.right -= i * TREE_DEPTH_OFFSET;
                            pImage->DrawImage(pDC, rcParentRect, pImage->GetSource(0, 5), CRect(4, 4, 4, 4), COLORREF_NULL);
                        }
                    }
                }
            }
            // Draw an orange/blue image over item.
            if ((nCurrentLayout == DocumentStructureLayout(Thumbnail)) ||((nCurrentLayout == DocumentStructureLayout(Compact)) && nState == 2)) {
                if (pRecord->IsPage()) {
                    pImage->DrawImage(pDC, rcRow, pImage->GetSource(nState, 5), CRect(4, 4, 4, 4), COLORREF_NULL);
                } else {
                    // take tree depth and draw button images of tree depth size.
                    CRect newRowOffset(rcRow);
                    if (pDrawArgs->pRow->GetTreeDepth() != 0 && nCurrentLayout == DocumentStructureLayout(Thumbnail)) {
                        newRowOffset.bottom += GetExpandedChildsHeight(pDrawArgs->pControl, pDrawArgs->pRow->GetRecord());
                        if ( pDrawArgs->pRow->IsExpanded() == TRUE ) {
                            newRowOffset.bottom += 1;
                        }
                        pImage->DrawImage(pDC, newRowOffset, pImage->GetSource(0, 5), CRect(4, 4, 4, 4), COLORREF_NULL);
                    }

                    if (pDrawArgs->pRow->GetTreeDepth() == 0) {
                        pImage->DrawImage(pDC, rcRow, pImage->GetSource(nState, 5), CRect(4, 4, 4, 4), COLORREF_NULL);
                    }

                    if (nState == 2) {
                        pImage->DrawImage(pDC, rcRow, pImage->GetSource(nState, 5), CRect(4, 4, 4, 4), COLORREF_NULL);
                    }
                }
            }
        }
        pDC->SetBkMode(TRANSPARENT);
        if (nCurrentLayout == DocumentStructureLayout(Thumbnail) && pRecord->IsPage()) {
            int x = rcRow.left + RECORD_ITEM_OFFSET;
            int y = rcRow.top + RECORD_ITEM_OFFSET;

            int nWidth = rcRow.Width() - 2*RECORD_ITEM_OFFSET;
            int nHeight = nWidth * 3/4;

            if (nWidth > rcRow.Width()) {
                nWidth = rcRow.Width();
            }
            CRect rcThumb;
            rcThumb.SetRect(x, y, x + nWidth, y + nHeight);
            if (pRecord->m_pContainer != NULL) {
                pRecord->m_pContainer->DrawThumbWhiteboard(pRecord->m_pvData, rcThumb, pDC);
            }
        }
    }

    // Code taken from CXTPReportControlItem::Draw - but a bit modified
    m_pControl = pDrawArgs->pControl;

    CRect& rcItem = pDrawArgs->rcItem;
    CXTPReportPaintManager* pPaintManager = pDrawArgs->pControl->GetPaintManager();

    CRgn rgn;
    rgn.CreateRectRgn(rcItem.left, rcItem.top - 1, rcItem.right, rcItem.bottom);

    if (!pDC->IsPrinting()) {
        pDC->SelectClipRgn(&rgn);
    }

    XTP_REPORTRECORDITEM_METRICS* pMetrics = new XTP_REPORTRECORDITEM_METRICS();
    pMetrics->strText = GetCaption(pDrawArgs->pColumn);
    pDrawArgs->pRow->GetItemMetrics(pDrawArgs, pMetrics);

    ASSERT(pMetrics->pFont);
    ASSERT(pMetrics->clrForeground != XTP_REPORT_COLOR_DEFAULT);

    int nItemGlyphs = rcItem.left;

    // draw tree inside item rect (see also HitTest function)
    if (pDrawArgs->pColumn && pDrawArgs->pColumn->IsTreeColumn()) {
        int nTreeDepth = pDrawArgs->pRow->GetTreeDepth() - pDrawArgs->pRow->GetGroupLevel();
        if (nTreeDepth > 0) {
            nTreeDepth++;
        }
        rcItem.left += pDrawArgs->pControl->GetIndent(nTreeDepth);

        BOOL bHasChildren = pDrawArgs->pControl->IsVirtualMode() ?
            pDrawArgs->pRow->GetTreeDepth() == 0 && (pMetrics->nVirtRowFlags & xtpVirtRowHasChildren) :
        pDrawArgs->pRow->HasChildren();

        CRect rcBitmap(rcItem);
        CSize sizeBitmap = pPaintManager->DrawCollapsedBitmap(NULL, pDrawArgs->pRow, rcBitmap);

        int nIconAlign = pDrawArgs->nTextAlign & xtpColumnIconMask;

        // horizontal alignment
        switch (nIconAlign) {
        case xtpColumnIconRight:
            rcBitmap.left = rcBitmap.right - sizeBitmap.cx - 2;
            break;
        case xtpColumnIconLeft:
            rcBitmap.right = rcBitmap.left + sizeBitmap.cx + 2;
            break;
        }
        // vertical alignment
        switch (nIconAlign) {
        case xtpColumnIconTop:
            rcBitmap.bottom = rcBitmap.top + sizeBitmap.cy + 2;
            break;
        case xtpColumnIconBottom:
            rcBitmap.top = rcBitmap.bottom - sizeBitmap.cy - 2;
            break;
        }
        bool bDrawArrow = false;
        if (bHasChildren || (pRecord->IsDocument() && !pRecord->IsEmptyDocument())) {
            bDrawArrow = true;
        }

        if (pRecord->IsDocument() && !pRecord->IsLoadedDocument()) {
            pDrawArgs->pRow->SetExpanded(FALSE);
        }
        sizeBitmap = pPaintManager->DrawCollapsedBitmap(bDrawArrow ? pDC : NULL, pDrawArgs->pRow, rcBitmap);

        if (!pDC->IsPrinting() && bHasChildren) {
            pDrawArgs->pRow->SetCollapseRect(rcBitmap);
        }

        // Draw a custom image at the left side of the record item
        if (pPaintManager->GetRuntimeClass() == RUNTIME_CLASS(CXTPDocumentStructurePaintManager)) {
            if (pRecord->IsChapter() || pRecord->IsDocument()) {
                CRect rcBitmap(rcItem);
                if (((CDocStructRecord*) pDrawArgs->pRow->GetRecord())->m_nCurrentLayout == DocumentStructureLayout(Thumbnail)) {
                    rcBitmap.left += 15;
                    rcBitmap.top -= 5;
                    CSize csBitmap = ((CXTPDocumentStructurePaintManager*)pPaintManager)->DrawCustomBitmap(pDC, pDrawArgs->pRow, rcBitmap, m_nImageIndex);
                    rcItem.left += sizeBitmap.cx + 35;
                } else {
                    rcBitmap.left += 18;
                    CSize csBitmap = ((CXTPDocumentStructurePaintManager*)pPaintManager)->DrawCustomBitmap(pDC, pDrawArgs->pRow, rcBitmap, m_nImageIndex);
                    rcItem.left += sizeBitmap.cx + 25;
                }
            } else {
                if (((CDocStructRecord*) pDrawArgs->pRow->GetRecord())->m_nCurrentLayout == DocumentStructureLayout(Compact)) {
                    rcBitmap.left += 15;
                    CSize csBitmap = ((CXTPDocumentStructurePaintManager*)pPaintManager)->DrawCustomBitmap(pDC, pDrawArgs->pRow, rcBitmap, m_nImageIndex);
                    rcItem.left += sizeBitmap.cx + 10;
                }
                rcItem.left += sizeBitmap.cx + 2;
            }
        }
    }

    COLORREF clrText = XTPOffice2007Images()->GetImageColor(_T("LISTBOX"), _T("NormalText"));
    pDC->SetTextColor(clrText);

    CFont* pOldFont = (CFont*)pDC->SelectObject(pMetrics->pFont);

    if (pMetrics->clrBackground != XTP_REPORT_COLOR_DEFAULT) {
        pDC->SetBkColor(pMetrics->clrBackground);
    } else {
        pDC->SetBkColor(pPaintManager->m_clrControlBack);
    }

    if (m_bHasCheckbox) {
        DrawCheckBox(pDrawArgs, rcItem);
    }

    // Do the draw bitmap pDC, rcItem, GetIconIndex()
    if (pMetrics->nItemIcon != XTP_REPORT_NOICON || GetIconIndex() != XTP_REPORT_NOICON) {
        pPaintManager->DrawItemBitmap(pDrawArgs, rcItem, pMetrics->nItemIcon != XTP_REPORT_NOICON ? pMetrics->nItemIcon : GetIconIndex());
    }
    nItemGlyphs = (rcItem.left - nItemGlyphs);

    OnDrawControls(pDrawArgs, rcItem);
    if (rcItem.Width()) {
        OnDrawCaption(pDrawArgs, pMetrics);
    }

    int nItemTextWidth = nItemGlyphs + 7;
    if (m_pMarkupUIElement) {
        nItemTextWidth += XTPMarkupMeasureElement(m_pMarkupUIElement, rcItem.Width(), INT_MAX).cx;
    } else {
        nItemTextWidth += pDC->GetTextExtent(pMetrics->strText).cx;
    }

    pDC->SelectObject(pOldFont);
    pMetrics->InternalRelease();
    if (!pDC->IsPrinting()) {
        pDC->SelectClipRgn(NULL);
    }

    return nItemTextWidth;
}

void CDocStructRecordItem::OnBeginEdit(XTP_REPORTRECORDITEM_ARGS* pItemArgs) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CDocStructRecord* pRecord = (CDocStructRecord*)GetRecord();
    ASSERT(pRecord);

    if (pRecord->m_nCurrentLayout == DocumentStructureLayout(Thumbnail)) {
        pItemArgs->rcItem.right = pItemArgs->pRow->GetRect().right - pItemArgs->pRow->GetTreeDepth() * TREE_DEPTH_OFFSET;
        if (pRecord->IsPage()) {
            pItemArgs->rcItem.left -= RECORD_ITEM_OFFSET;
            pItemArgs->rcItem.top = pItemArgs->pRow->GetRect().bottom - RECORD_TEXT_OFFSET;
            pItemArgs->rcItem.bottom = pItemArgs->pRow->GetRect().bottom - 2;
        } else {
            pItemArgs->rcItem.left += 45;
            pItemArgs->rcItem.bottom = pItemArgs->pRow->GetRect().bottom - 8;
            pItemArgs->rcItem.top = pItemArgs->pRow->GetRect().top + 14;
        }
    } else { // DocumentStructureLayout(Compact) view
        pItemArgs->rcItem.left += 25;
        pItemArgs->rcItem.top += 3;
    }

    CXTPReportRecordItem::OnBeginEdit(pItemArgs);
}

void CDocStructRecordItem::OnDrawCaption(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pMetrics) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CDocStructRecord* pRecord = (CDocStructRecord*)GetRecord();
    if (pRecord == NULL) {
        return;
    }
    pDrawArgs->nTextAlign = DT_VCENTER;
    if (pRecord->m_nCurrentLayout == DocumentStructureLayout(Thumbnail)) {
        if (pRecord->IsPage()) {
            CRect rcCaption;
            rcCaption.SetRect(pDrawArgs->rcItem.left - RECORD_ITEM_OFFSET + 1, pDrawArgs->rcItem.bottom - RECORD_TEXT_OFFSET - 5, pDrawArgs->rcItem.right - pDrawArgs->pRow->GetTreeDepth() * RECORD_ITEM_OFFSET, pDrawArgs->rcItem.bottom);
            pDrawArgs->rcItem.SetRect(rcCaption.TopLeft(), rcCaption.BottomRight());
        } else {
            CRect rcCaption (pDrawArgs->rcItem);
            rcCaption.left += 3;
            rcCaption.right -= pDrawArgs->pRow->GetTreeDepth() * RECORD_ITEM_OFFSET;
            pDrawArgs->rcItem.SetRect(rcCaption.TopLeft(), rcCaption.BottomRight());
        }
    }

    // If this is a "click" page, draw icon in front of page title
    bool bDrawClickIcon = pRecord->IsPage() && pRecord->m_pContainer != NULL && pRecord->m_pContainer->ShowClickIcon(pRecord->m_pvData);
    if (bDrawClickIcon) { 
        CXTPReportPaintManager* pPaintManager = pDrawArgs->pControl->GetPaintManager();
        if (pPaintManager) {
            CRect rcCaption(pDrawArgs->rcItem);
            CRect rcBitmap(rcCaption.right-22, rcCaption.top, rcCaption.right-2, rcCaption.bottom);
            CSize csBitmap = ((CXTPDocumentStructurePaintManager*)pPaintManager)->DrawCustomBitmap(pDrawArgs->pDC, pDrawArgs->pRow, rcBitmap, 5);
            pDrawArgs->rcItem.right = rcBitmap.left;
        }
    }

    CXTPReportRecordItem::OnDrawCaption(pDrawArgs, pMetrics);
}

void CDocStructRecordItem::OnDblClick(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    // Edit item on double click.
    if (!m_bEditOnDblClick) {
        return;
    }
    if (pClickArgs != NULL && pClickArgs->pControl != NULL) {
        //pClickArgs->pControl->EditItem(pClickArgs);
        //pClickArgs->pControl->GetInplaceEdit()->SetSel(0, -1);
        if (pClickArgs->pRow->HasChildren()) {
            pClickArgs->pRow->SetExpanded(!pClickArgs->pRow->IsExpanded());
        } else {
            CDocStructRecord* pRecord = (CDocStructRecord*)GetRecord();
            if(pRecord->IsDocument() && !pRecord->IsLoadedDocument()) {
                pRecord->m_pContainer->LoadDocument(pRecord->m_pvData);
            }
        }
    }
    //CXTPReportRecordItem::OnDblClick(pClickArgs);
}