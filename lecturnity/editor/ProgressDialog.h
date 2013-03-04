#if !defined(AFX_PROGRESSDIALOGE_H__0A88752B_5F02_4046_A088_74162EDE9DC2__INCLUDED_)
#define AFX_PROGRESSDIALOGE_H__0A88752B_5F02_4046_A088_74162EDE9DC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressDialog.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CProgressDialogE 

#include "..\Studio\resource.h"

//#include "editorDoc.h"
// A simple #include of editorDoc.h would not work.
// I have no idea whatsoever why not.
// 7.7.09: circular includes?
class CEditorProject;

class CProgressDialogE : public CDialog
{
// Konstruktion
public:
	CProgressDialogE(CWnd* pParent = NULL, CEditorProject *pProject = NULL);  // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CProgressDialogE)
	enum { IDD = IDD_PROGRESS_DIALOG_E };
	CStatic	m_progressLabel;
	CProgressCtrl	m_progress;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CProgressDialogE)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CProgressDialogE)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   enum
   {
      END_DIALOG = 99
   };

private:
   CString     m_labelString;
   CEditorProject *m_pProject;

public:
   void SetLabel(LPCTSTR lpText);
   void SetLabel(UINT nResourceID);
   void SetRange(int lower, int upper);
   void SetStep(int step);
   void SetPos(int pos);
   void OffsetPos(int offset);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PROGRESSDIALOGE_H__0A88752B_5F02_4046_A088_74162EDE9DC2__INCLUDED_
