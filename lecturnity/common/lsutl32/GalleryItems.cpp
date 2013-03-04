// GalleryItems.cpp: implementation of the CGalleryItemStyleSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GalleryItems.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGalleryItemFontFace::CGalleryItemFontFace(LPCTSTR lpszCaption)
{
	m_strCaption = lpszCaption;
}

void CGalleryItemFontFace::Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL /*bPressed*/, BOOL bChecked)
{
	CXTPPaintManager* pPaintManager = pGallery->GetPaintManager();

	pPaintManager->DrawRectangle(pDC, rcItem, bSelected, FALSE, bEnabled, bChecked, FALSE, xtpBarTypePopup, xtpBarPopup);

	COLORREF clrTextColor = pPaintManager->GetRectangleTextColor(bSelected, FALSE, bEnabled, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);
	pDC->SetTextColor(clrTextColor);

	CFont fnt;
	fnt.CreatePointFont(150, m_strCaption);

	CXTPFontDC font(pDC, &fnt);

	CRect rcText(rcItem);
	rcText.left += 4;

	pDC->DrawText(m_strCaption, rcText, DT_END_ELLIPSIS|DT_SINGLELINE|DT_VCENTER);
}

void CGalleryItemFontFace::AddFontItems(CXTPControlGalleryItems* pItems)
{
	static CXTPControlFontComboBoxList::CFontDescHolder s_fontHolder;
	s_fontHolder.EnumFontFamilies();

	// now walk through the fonts and remove (charset) from fonts with only one
	CArray<CXTPControlFontComboBoxList::CFontDesc*, CXTPControlFontComboBoxList::CFontDesc*>& arrFontDesc = 
	s_fontHolder.m_arrayFontDesc;

	int nCount = (int)arrFontDesc.GetSize();
	int i;
	for (i = 0; i < nCount; i++)
	{
		CXTPControlFontComboBoxList::CFontDesc* pDesc = arrFontDesc[i];
		CString str = pDesc->m_strName;
		CXTPControlGalleryItem* pItem = pItems->AddItem(new CGalleryItemFontFace(str));
		pItem->SetData((DWORD_PTR)pDesc);
	}
}


