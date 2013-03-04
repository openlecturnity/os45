#if !defined(AFX_TEMPLATEVARINT_H__46665A49_38FA_438A_81DE_FD943D7F96E9__INCLUDED_)
#define AFX_TEMPLATEVARINT_H__46665A49_38FA_438A_81DE_FD943D7F96E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateVarInt.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateVarInt 

#include "TemplateEngine.h"

class CTemplateVarInt : public CDialog
{
// Konstruktion
public:
	CTemplateVarInt(CTemplateKeyword *pKeyword);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CTemplateVarInt)
	enum { IDD = IDD_TEMPLATE_VAR_INT };
	CSpinButtonCtrl	m_sbcValue;
	CEdit	m_edValue;
	CEdit	m_edVarName;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTemplateVarInt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTemplateVarInt)
	virtual BOOL OnInitDialog();
	afx_msg void OnSpinButtonClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLeaveValue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CTemplateIntegerKeyword *m_pKeyword;

   int GetValue();
   void SetValue(int value, bool bIsChange=true);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TEMPLATEVARINT_H__46665A49_38FA_438A_81DE_FD943D7F96E9__INCLUDED_
