#if !defined(AFX_EMPTYFIELD_H__DD1A4B0D_83B0_4AE4_882A_6B48CA8FB49D__INCLUDED_)
#define AFX_EMPTYFIELD_H__DD1A4B0D_83B0_4AE4_882A_6B48CA8FB49D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EmptyField.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CEmptyField 

class CEmptyField : public CStatic
{
// Konstruktion
public:
	CEmptyField();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CEmptyField)
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CEmptyField();

   void SetSelected(bool bSelected) {m_bSelected = bSelected;}
   bool IsSelected() {return m_bSelected ;}
   void SetInsertionPos(UINT nInsertPosition) {m_nInsertPosition = nInsertPosition;}
   void IncrementInsertionPos() {++m_nInsertPosition;}
   void DecrementInsertionPos() {--m_nInsertPosition;}
   UINT GetInsertionPos() {return m_nInsertPosition;}
   void UpdateAnswers(CStringArray &aAnswers);
   void GetAnswers(CStringArray &aAnswers);
   void SetAnswers(CStringArray &aAnswers);

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CEmptyField)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

// Attributes
private:
	bool m_bSelected;
	UINT m_nInsertPosition;
   CStringArray m_aPossibleAnswers;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EMPTYFIELD_H__DD1A4B0D_83B0_4AE4_882A_6B48CA8FB49D__INCLUDED_
