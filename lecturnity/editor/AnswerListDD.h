#if !defined(AFX_ANSWERLISTDD_H__2A7BAF59_3B6B_4106_961B_AB6DD3BD23D3__INCLUDED_)
#define AFX_ANSWERLISTDD_H__2A7BAF59_3B6B_4106_961B_AB6DD3BD23D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnswerListDD.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CAnswerListDD 

class CAnswerListDD : public CListCtrl
{
// Konstruktion
public:
	CAnswerListDD();

// Attribute


// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CAnswerListDD)
	public:
	//}}AFX_VIRTUAL

// Implementierung
public:
	//int GetSelectedItem();
	virtual ~CAnswerListDD();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CAnswerListDD)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
	int GetSelectedItem();
   // Loads the image and returns the image index or -1 if fails
   int AddImage(CString &csFilename);
   HRESULT GetImageName(int iImageIndex, CString &csImageName);
   void MoveSelectedUp();
   void MoveSelectedDown();

private:
   CArray<Gdiplus::Image *, Gdiplus::Image *> m_aImages;
   CStringArray m_aImageNames;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_ANSWERLISTDD_H__2A7BAF59_3B6B_4106_961B_AB6DD3BD23D3__INCLUDED_
