// AnswerListDD.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "AnswerListDD.h"

//---------------------------------------------------------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnswerListDD

#define ANSWER_WIDTH 100
#define ANSWER_HEIGHT 100

//---------------------------------------------------------------------------------------------------------------------
CAnswerListDD::CAnswerListDD()
{
   //	m_bComboCreated = false;  
}

//---------------------------------------------------------------------------------------------------------------------
CAnswerListDD::~CAnswerListDD()
{
   for (int i = 0; i < m_aImages.GetSize(); ++i)
   {
      Gdiplus::Image *pImage = m_aImages[i];
      if (pImage)
         delete pImage;
   }
   m_aImages.RemoveAll();
}

//---------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CAnswerListDD, CListCtrl)
//{{AFX_MSG_MAP(CAnswerListDD)
ON_WM_VSCROLL()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CAnswerListDD 

void CAnswerListDD::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
   // TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
   HDC hDC = lpDrawItemStruct->hDC;
   
   CDC* pDC = CDC::FromHandle(hDC);

   CRect rcItem(lpDrawItemStruct->rcItem);

   UINT uiFlags = ILD_TRANSPARENT;

   int nItem = lpDrawItemStruct->itemID;

   BOOL bFocus = (GetFocus() == this);

   COLORREF clrBkSave = 0;
   COLORREF clrImage = RGB(0, 255, 0);
   
   // get item data
   LV_ITEM lvi;
   lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
   lvi.iItem = nItem;
   lvi.iSubItem = 0;
   lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
   lvi.state = LVIS_FOCUSED | LVIS_SELECTED;
   
   GetItem(&lvi);
   
   BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
   bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);
   
   // set colors if item is selected
   CRect rcAllLabels;
   GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);
   
   if (bSelected)
   {
      clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
      
      CBrush cbr(::GetSysColor(COLOR_HIGHLIGHT));
      pDC->FillRect(rcAllLabels, &cbr);
   }
   else
   {
      CBrush cbr(RGB(255, 255, 255));
      pDC->FillRect(rcAllLabels, &cbr);
   }
   
   // draw focus rectangle if item has focus
   if (lvi.state & LVIS_FOCUSED && bFocus)
      pDC->DrawFocusRect(rcAllLabels);
   
   // set original colors if item was selected
   
   if (bSelected)
   {
      pDC->SetBkColor(clrBkSave);
   }
   
   CImageList *pImageList = GetImageList(LVSIL_STATE);
   int iImageIndex = lvi.iImage;
   if (iImageIndex >= 0 && iImageIndex < m_aImages.GetSize())
   {
      Gdiplus::Graphics graphics(hDC);
      Gdiplus::Image *pImage = m_aImages[lvi.iImage];
      graphics.DrawImage(pImage, rcItem.left + 5, rcItem.top + 5, pImage->GetWidth(), pImage->GetHeight());

   }
   
   
}

//---------------------------------------------------------------------------------------------------------------------
void CAnswerListDD::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   // TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
   
   CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
   // Redraw items. Needet for ComboBox
   RedrawWindow();
   
}

//---------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------

int CAnswerListDD::GetSelectedItem()
{
   // check new item and set its icon as the app icon
   POSITION p = GetFirstSelectedItemPosition();
   int nSelected = GetNextSelectedItem(p);	
   return nSelected;
}


int CAnswerListDD::AddImage(CString &csFilename)
{

   int iStringLength = csFilename.GetLength();
   WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR)); // +1 for null termination
#ifdef _UNICODE
   wcscpy(wcString, csFilename);
#else
   MultiByteToWideChar(CP_ACP, 0, csFilename, iStringLength+1, 
      wcString, iStringLength+1);
#endif
      
   Gdiplus::Image *pImage = Gdiplus::Image::FromFile(wcString);
   
   CRect rcItem;
   GetItemRect(0, &rcItem, LVIR_BOUNDS);
   rcItem.DeflateRect(5, 5, 5, 5);
   int iImageWidth = pImage->GetWidth();
   int iImageHeight = pImage->GetHeight();
   double dXZoom = (double)rcItem.Width() / iImageWidth;
   double dYZoom = (double)rcItem.Height() / iImageHeight;
   double dZoom = dXZoom < dYZoom ? dXZoom : dYZoom;
   
   int iNewImageWidth = (int)(iImageWidth * dZoom);
   int iNewImageHeight = (int)(iImageHeight * dZoom);

   Gdiplus::Image *pZoomedImage = pImage->GetThumbnailImage(iNewImageWidth, iNewImageHeight);

   m_aImages.Add(pZoomedImage);
   m_aImageNames.Add(csFilename);

   int iThumbWidth = pZoomedImage->GetWidth();

   delete pImage;

   if (wcString)
      free(wcString);

   return m_aImages.GetSize() - 1;
}


HRESULT CAnswerListDD::GetImageName(int iImageIndex, CString &csImageName)
{
   HRESULT hr = S_OK;

   if (iImageIndex < 0 || iImageIndex >= m_aImageNames.GetSize())
      hr = E_INVALIDARG;

   Gdiplus::Image *pImage = NULL;
   if (SUCCEEDED(hr))
   {
      csImageName = m_aImageNames[iImageIndex];
   }

   return hr;
}

void CAnswerListDD::MoveSelectedUp()
{
   int iSelItemIndex = GetSelectedItem();
   
   LV_ITEM lvi;
   lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
   lvi.iItem = iSelItemIndex;
   lvi.iSubItem = 0;
   lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
   lvi.state = LVIS_FOCUSED | LVIS_SELECTED;
   GetItem(&lvi);
   
   DeleteItem(iSelItemIndex);
   
   lvi.iItem = iSelItemIndex - 1;
   InsertItem(&lvi);
   
   RedrawItems(iSelItemIndex,iSelItemIndex-1);
   
   SetSelectionMark(iSelItemIndex+1);
   SetFocus();

}

void CAnswerListDD::MoveSelectedDown()
{
	int iSelItemIndex = GetSelectedItem();

	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.iItem = iSelItemIndex;
	lvi.iSubItem = 0;
	lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	lvi.state = LVIS_FOCUSED | LVIS_SELECTED;
	GetItem(&lvi);

	DeleteItem(iSelItemIndex);

	lvi.iItem = iSelItemIndex + 1;
	InsertItem(&lvi);

	RedrawItems(iSelItemIndex, iSelItemIndex+1);

	SetSelectionMark(iSelItemIndex + 1);
	SetFocus();
}