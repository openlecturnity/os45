#if !defined(AFX_PROGRESSDIALOG_H__0A88752B_5F02_4046_A088_74162EDE9DC2__INCLUDED_)
#define AFX_PROGRESSDIALOG_H__0A88752B_5F02_4046_A088_74162EDE9DC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressDialog.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CProgressDialogA 

#include "..\Studio\Resource.h"
//#include "editorDoc.h"
// A simple #include of AssistantDoc.h would not work.
// I have no idea whatsoever why not.
class CAssistantDoc;

class CProgressDialogA : public CDialog
{
// Konstruktion
public:
	CProgressDialogA(CWnd* pParent, HANDLE hDialogInitEvent);  // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CProgressDialogA)
	enum { IDD = IDD_PROGRESS_DIALOG };
	CStatic	m_progressLabel;
	CProgressCtrl	m_progress;
	//}}AFX_DATA

public:
   void SetLabel(LPCTSTR lpText);
   void SetLabel(UINT nResourceID);
   void SetRange(int lower, int upper);
   void SetStep(int step);
   void SetPos(int pos);
   void OffsetPos(int offset);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CProgressDialogA)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CProgressDialogA)
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
   HANDLE m_hDialogInitEvent;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PROGRESSDIALOG_H__0A88752B_5F02_4046_A088_74162EDE9DC2__INCLUDED_
