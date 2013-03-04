// MyListBox.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "MyListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MyListBox

MyListBox::MyListBox()
{
}

MyListBox::~MyListBox()
{
}


BEGIN_MESSAGE_MAP(MyListBox, CListBox)
	//{{AFX_MSG_MAP(MyListBox)
		// HINWEIS - Der Klassen-Assistent fügt hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten MyListBox 

void MyListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   //CRect members to store the position of the items

   CRect rItem;
	CRect rText;

   CDC* dc = CDC::FromHandle(lpDrawItemStruct->hDC);

	if ((int)lpDrawItemStruct->itemID < 0)
	{
		// If there are no elements in the List Box 
		// based on whether the list box has Focus or not 
		// draw the Focus Rect or Erase it,
		if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && 
			(lpDrawItemStruct->itemState & ODS_FOCUS))
		{
			dc->DrawFocusRect(&lpDrawItemStruct->rcItem);
		}
		else if ((lpDrawItemStruct->itemAction & ODA_FOCUS) &&	
			!(lpDrawItemStruct->itemState & ODS_FOCUS)) 
		{
			dc->DrawFocusRect(&lpDrawItemStruct->rcItem); 
		}
		return;
	}

	if ((int)lpDrawItemStruct->itemID == 0)
	{
      // Disable the first entry (header)
      lpDrawItemStruct->itemState |= ODS_DISABLED;
   }


   // String to store the text
	CString csText;

	// Get the item text.
	GetText(lpDrawItemStruct->itemID, csText);


   //Initialize the Item's row
	rItem = lpDrawItemStruct->rcItem;
   rText = rItem;


   //Start drawing the text 2 pixels away from border
   rText.left = rItem.left + 2;
   rText.top  = rItem.top;


///	UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_EXPANDTABS | DT_TABSTOP;
	UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
	if (GetStyle() & LBS_USETABSTOPS)
		nFormat |= DT_EXPANDTABS;


	// If item selected, draw the highlight rectangle.
	// Or if item deselected, draw the rectangle using the window color.
	if ((lpDrawItemStruct->itemState & ODS_SELECTED) &&
		 (lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		CBrush br(::GetSysColor(COLOR_HIGHLIGHT));
		dc->FillRect(&rItem, &br);
	}
	else if (!(lpDrawItemStruct->itemState & ODS_SELECTED) && 
		(lpDrawItemStruct->itemAction & ODA_SELECT)) 
	{
		CBrush br(::GetSysColor(COLOR_WINDOW));
		dc->FillRect(&rItem, &br);
	}

	// If the item has focus, draw the focus rect.
	// If the item does not have focus, erase the focus rect.
	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && 
		(lpDrawItemStruct->itemState & ODS_FOCUS))
	{
		dc->DrawFocusRect(&rItem); 
	}
	else if ((lpDrawItemStruct->itemAction & ODA_FOCUS) &&	
		!(lpDrawItemStruct->itemState & ODS_FOCUS))
	{
		dc->DrawFocusRect(&rItem); 
	}


	// To draw the Text set the background mode to transparent.
	int iBkMode = dc->SetBkMode(TRANSPARENT);

	COLORREF crText;

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		crText = dc->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	else if (lpDrawItemStruct->itemState & ODS_DISABLED)
		crText = dc->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
	else
		crText = dc->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

	// Header line: grey text (see above), bold text style
   CFont *pOldFont;
   if (lpDrawItemStruct->itemState & ODS_DISABLED)
   {
      CFont font;
      // Create a 'dummy' font - which is overwritten by a modified 'original' font
      font.CreatePointFont(12, _T("Tahoma"));
      //font.CreateFont(12, 12, 0, 0, FW_SEMIBOLD, 0, 0, 0, 1, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
      pOldFont = dc->SelectObject(&font);
      LOGFONT logfont;
      pOldFont->GetLogFont(&logfont);
      logfont.lfWeight = FW_SEMIBOLD;
      font.CreateFontIndirect(&logfont);
      dc->SelectObject(&font);
   }


   // Look for color code (e.g. #ff0000) in the item text
   int nPosColor = csText.Find(_T("#"));
   bool bIsColorKeyword = (nPosColor > -1);

   if (bIsColorKeyword)
   {
      // Replace the color code by a colored rectangle
      COLORREF clrRect;
      clrRect = GetColorFromString(csText);
      csText = csText.Left(nPosColor);
      CRect rColor(280, rItem.top, 280+24, rItem.top+12);
		CBrush brush(clrRect);
      CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
      CPen *pOldPen = dc->SelectObject(&pen);
      CBrush *pOldBrush = dc->SelectObject(&brush);
      dc->SetBkColor(clrRect);
      dc->Rectangle(&rColor);
      // Reset old Values
      dc->SelectObject(pOldPen);
      dc->SelectObject(pOldBrush);
   }


	// Draw the Text using tabs
   int nTabs[] = {210, 280};
   dc->TabbedTextOut(rText.left, rText.top, csText, 2, nTabs, 0);

   // Reset old font (changed for the 'header line' above)
   if (lpDrawItemStruct->itemState & ODS_DISABLED)
      dc->SelectObject(pOldFont);

   // Detach the CDC
   dc->Detach();
}

void MyListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
   lpMeasureItemStruct->itemHeight = 12;
}

COLORREF MyListBox::GetColorFromString(CString csText)
{
   int nColorPos = csText.Find(_T("#"));
   CString csColor = csText.Mid(nColorPos + 1, 6);

   // COLORREF is BGR, but we put RGB data to it
   COLORREF crColorRev = _tcstoul(csColor, NULL, 16);
   COLORREF crColor = ((crColorRev & 0x00ff0000) >> 16) | (crColorRev & 0x0000ff00) | ((crColorRev & 0x000000ff) << 16);
   
   return crColor;
}