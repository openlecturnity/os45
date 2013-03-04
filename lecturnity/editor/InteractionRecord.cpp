#include "stdafx.h"
#include "InteractionRecord.h"
#include "InteractionStructureView.h"
#include "MainFrm.h"

// ********************************************CInteractionRecordItemIcon***************************
CInteractionRecordItemIcon::CInteractionRecordItemIcon(BOOL bVisible): m_bVisible(bVisible) {
    UpdateVisibleIcon();
}

void CInteractionRecordItemIcon::UpdateVisibleIcon() {
    SetIconIndex(m_bVisible ? 0 : 1);
}

void CInteractionRecordItemIcon::DoPropExchange(CXTPPropExchange* pPX) {
    CXTPReportRecordItem::DoPropExchange(pPX);
    PX_Bool(pPX, _T("Visible"), m_bVisible);
}

int CInteractionRecordItemIcon::Draw(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs) {
    if ( pDrawArgs != NULL 
        && pDrawArgs->pControl != NULL 
        && pDrawArgs->pControl->GetPaintManager() != NULL 
        && pDrawArgs->pControl->GetPaintManager()->GetRuntimeClass() != RUNTIME_CLASS(CXTPReportInteractionStructurePaintManager) ){
        return CXTPReportRecordItem::Draw(pDrawArgs);
    }

    int offSet = 0;
    CDC* pDC = pDrawArgs->pDC;
    int nTreeDepth = pDrawArgs->pRow->GetTreeDepth();
    int nLeftIndent = pDrawArgs->pControl->GetIndent(nTreeDepth);
    CRect rcRow = pDrawArgs->pRow->GetRect();

    nTreeDepth == 0 ? offSet = 18 : offSet = 25;
    rcRow.left += nLeftIndent + offSet;

    if (pDrawArgs->pRow->IsSelected() == TRUE) {
        CXTPOffice2007Image* pImage = XTPOffice2007Images()->LoadFile(_T("LISTBOX"));
        if (pImage) {
            pImage->DrawImage(pDC, rcRow, pImage->GetSource(2, 4), CRect(4, 4, 4, 4), COLORREF_NULL);
        }
        pDC->SetBkMode(TRANSPARENT);
    } else {
        // If the row is root draw gray rectangle.
        if (pDrawArgs->pRow->GetTreeDepth() == 0) {
            CXTPOffice2007Image* pImage = XTPOffice2007Images()->LoadFile(_T("PUSHBUTTON"));
            if (pImage) {
                pImage->DrawImage(pDC, rcRow, pImage->GetSource(3, 5), CRect(4, 4, 4, 4), COLORREF_NULL);
            }
            pDC->SetBkMode(TRANSPARENT);

            COLORREF clrBorder = RGB(101, 147, 207);
            pDC->Draw3dRect(rcRow, clrBorder, clrBorder);
        }
    }

    // Code taken from CXTPReportRecordItem
    ASSERT(pDrawArgs->pControl);
    if (!pDrawArgs->pControl) {
        return 0;
    } else {
        m_pControl = pDrawArgs->pControl;
    }

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
    pDC->SetTextColor(pMetrics->clrForeground);

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
    if (rcItem.Width())
        OnDrawCaption(pDrawArgs, pMetrics);

    int nItemTextWidth = nItemGlyphs + 7;
    if (m_pMarkupUIElement) {
        nItemTextWidth += XTPMarkupMeasureElement(m_pMarkupUIElement, rcItem.Width(), INT_MAX).cx;
    } else {
        nItemTextWidth += pDC->GetTextExtent(pMetrics->strText).cx;
    }

    pDC->SelectObject(pOldFont);
    pMetrics->InternalRelease();
    if (!pDC->IsPrinting())
        pDC->SelectClipRgn(NULL);
    return nItemTextWidth;
}
// end CInteractionRecordItemIcon

// ********************************************CInteractionRecordItemInteractionText***************************
CInteractionRecordItemInteractionText::CInteractionRecordItemInteractionText(UINT nPosition, CString csText) {
    m_strText = csText;
    m_nPosition = nPosition;
    SetTooltip(csText);
}

int CInteractionRecordItemInteractionText::Compare(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem) {
    return int(m_nPosition) - int(((CInteractionRecordItemInteractionText*)pItem)->m_nPosition);
}

int CInteractionRecordItemInteractionText::Draw(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs) {
    if (pDrawArgs != NULL 
        && pDrawArgs->pControl != NULL 
        && pDrawArgs->pControl->GetPaintManager() != NULL 
        && pDrawArgs->pControl->GetPaintManager()->GetRuntimeClass() != RUNTIME_CLASS(CXTPReportInteractionStructurePaintManager)) {
        return CXTPReportRecordItem::Draw(pDrawArgs);
    }

    ASSERT(pDrawArgs->pControl);
    if (!pDrawArgs->pControl) {
        return 0;
    } else {
        m_pControl = pDrawArgs->pControl;
    }

    CDC* pDC = pDrawArgs->pDC;
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

    //	if (pMetrics->clrBackground != XTP_REPORT_COLOR_DEFAULT && pDrawArgs->bDrawFocusRect == TRUE)
    //		pDC->FillSolidRect(rcItem, pMetrics->clrBackground);

    int nItemGlyphs = rcItem.left;

    // draw tree inside item rect (see also HitTest function)
    if (pDrawArgs->pColumn && pDrawArgs->pColumn->IsTreeColumn()) {
        //if (pDrawArgs->pControl->IsVirtualMode())
        //{
        //	pDrawArgs->pRow->m_nRowLevel = pMetrics->nVirtRowLevel ? 1 : 0; // only 1 level of children is supported at the moment
        //	if (pDrawArgs->pRow->m_nRowLevel)
        //      pDrawArgs->pRow->SetCollapseRect(CRect(0));
        //		//pDrawArgs->pRow->m_rcCollapse = CRect(0);
        //}
        int nTreeDepth = pDrawArgs->pRow->GetTreeDepth() - pDrawArgs->pRow->GetGroupLevel();
        if (nTreeDepth > 0) {
            nTreeDepth++;
        }
        rcItem.left += pDrawArgs->pControl->GetIndent(nTreeDepth);

        BOOL bHasChildren = pDrawArgs->pControl->IsVirtualMode() ?
            pDrawArgs->pRow->GetTreeDepth() == 0 && (pMetrics->nVirtRowFlags & xtpVirtRowHasChildren) : pDrawArgs->pRow->HasChildren();

        CRect rcBitmap(rcItem);
        CSize sizeBitmap = pPaintManager->DrawCollapsedBitmap(NULL, pDrawArgs->pRow, rcBitmap);

        //      if ( pDrawArgs->bDrawTreeStructure == TRUE ) 
        //		   pPaintManager->DrawTreeStructure(pDrawArgs, pMetrics, rcItem, sizeBitmap);

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
        sizeBitmap = pPaintManager->DrawCollapsedBitmap(bHasChildren ? pDC : NULL, pDrawArgs->pRow, rcBitmap);

        if (!pDC->IsPrinting() && bHasChildren) {
            pDrawArgs->pRow->SetCollapseRect(rcBitmap);
        }

        if (pPaintManager->GetRuntimeClass() == RUNTIME_CLASS(CXTPReportInteractionStructurePaintManager)) {
            rcBitmap.right += 20;
            rcBitmap.left += 14;
            CSize csBitmap = ((CXTPReportInteractionStructurePaintManager*)pPaintManager)->DrawCustomBitmap(pDC, pDrawArgs->pRow, rcBitmap, m_nImageIndex);
            rcItem.left += sizeBitmap.cx + 24; // Change from 2 to 20 - I need extra space for another image.
        } else {
            rcItem.left += sizeBitmap.cx + 2;
        }
    }

    pDC->SetTextColor(pMetrics->clrForeground);

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
    if (rcItem.Width())
        OnDrawCaption(pDrawArgs, pMetrics);

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
// end CInteractionRecordItemInteractionText

// ******************************************** CInteractionRecord ******************************
CInteractionRecord::CInteractionRecord(CInteractionAreaEx *pInteractionArea, UINT nPos, CString csText, UINT nImageIndex) {
    CString csRecordText = csText + pInteractionArea->GetInteractionName();
    if (csRecordText.GetLength() == 0) {
        csRecordText = pInteractionArea->GetInteractionType();
    }
    m_pItemText = (CInteractionRecordItemInteractionText*) AddItem(new CInteractionRecordItemInteractionText(nPos, csRecordText));
	m_pItemIcon = (CInteractionRecordItemIcon*) AddItem(new CInteractionRecordItemIcon(pInteractionArea->IsTimelineDisplay()||pInteractionArea->IsSelected() == TRUE ? 1 : 0));
    m_pInteractionArea = pInteractionArea;
    m_pItemText->m_nImageIndex = nImageIndex;
}

CInteractionRecord::~CInteractionRecord() {
    m_pInteractionArea = NULL;
    m_pItemIcon = NULL;
}

BOOL CInteractionRecord::SetVisible() {
    ASSERT(m_pItemIcon);
    if (m_pItemIcon->m_bVisible){
        return FALSE;
    }
    m_pItemIcon->m_bVisible = TRUE;
    m_pItemIcon->UpdateVisibleIcon();
    return TRUE;
}

void CInteractionRecord::DoPropExchange(CXTPPropExchange* pPX) {
    CXTPReportRecord::DoPropExchange(pPX);

    if (pPX->IsLoading()) {
        // 0 - m_pItemText
        ASSERT_KINDOF(CInteractionRecordItemInteractionText, GetItem(0));
        m_pItemText = DYNAMIC_DOWNCAST(CInteractionRecordItemInteractionText, GetItem(0));
        ASSERT(m_pItemText);

        // 1 - m_pItemIcon
        ASSERT_KINDOF(CInteractionRecordItemIcon, GetItem(ICON_COLUMN_NO));
        m_pItemIcon = DYNAMIC_DOWNCAST(CInteractionRecordItemIcon, GetItem(ICON_COLUMN_NO));
        ASSERT(m_pItemIcon);
    }
}
// *********end CInteractionRecord