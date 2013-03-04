#if !defined(AFX_INTERACTIONBUTTONPREVIEW_H__002E3BBC_8E79_4A79_836B_2FF6323A4ACF__INCLUDED_)
#define AFX_INTERACTIONBUTTONPREVIEW_H__002E3BBC_8E79_4A79_836B_2FF6323A4ACF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// InteractionButtonPreview.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CInteractionButtonPreview 

class CInteractionButtonPreview : public CStatic
{
// Konstruktion
public:
	CInteractionButtonPreview();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CInteractionButtonPreview)
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CInteractionButtonPreview();

   void SetText(CString &csButtonText) {m_csButtonText = csButtonText;}
	void GetText(CString &csText, CRect &rcText, LOGFONT &logFont);
   
   void SetButtonFont(LOGFONT *pLfButton);
   void SetColors(COLORREF clrFill, COLORREF clrLine, COLORREF clrText);
   void SetTextColor(COLORREF clrText);
   void SetLineColor(COLORREF clrLine);
   void SetFillColor(COLORREF clrFill);
   void SetImage(CString &csImageFileName);
   void GetImageDimension(CSize &siImage);
   void IsImageButton(bool bIsImageButton);
   void SetLineWidth(int iLineWidth);
   void SetLineStyle(int iLineStyle);

private:
   void CalculateImageRect(Gdiplus::RectF gdipRcClient, CRect rcArea, 
                           Gdiplus::RectF &gdipRcImage);
   void CalculateRectangleRect(Gdiplus::RectF gdipRcClient, Gdiplus::RectF gdipRcText, 
                               Gdiplus::RectF &gdipRcRectangle);

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CInteractionButtonPreview)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
   CString m_csButtonText;
	CRect m_rcArea;
	LOGFONT m_lfButton;
   COLORREF m_clrFill;
   COLORREF m_clrLine;
   COLORREF m_clrText;
   Gdiplus::Image *m_pGdipImage;
   int m_iLineWidth;
   int m_iLineStyle;
   CArray<Gdiplus::DashStyle, Gdiplus::DashStyle> m_aLineStyles;

   bool m_bIsImageButton;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_INTERACTIONBUTTONPREVIEW_H__002E3BBC_8E79_4A79_836B_2FF6323A4ACF__INCLUDED_
