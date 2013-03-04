// FixObjDlg.h : Header-Datei
//

#if !defined(AFX_FIXOBJDLG_H__B1A549C1_5D0E_4BB8_8D0B_A3C45766E81A__INCLUDED_)
#define AFX_FIXOBJDLG_H__B1A549C1_5D0E_4BB8_8D0B_A3C45766E81A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dropit.h"

/////////////////////////////////////////////////////////////////////////////
// CFixObjDlg Dialogfeld

class CFixObjDlg : public CDialog
{
// Konstruktion
public:
	CFixObjDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CFixObjDlg)
	enum { IDD = IDD_FIXOBJ_DIALOG };
	CDropit	m_ctrlDrop;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CFixObjDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CFixObjDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   HRESULT FixIt(CString &csFilename);
   HRESULT ProcessObjFile(CFile &fileInput, CFile &fileOutput);
   HRESULT ProcessEvqFile(CFile &fileInput, CFile &fileOutput);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_FIXOBJDLG_H__B1A549C1_5D0E_4BB8_8D0B_A3C45766E81A__INCLUDED_)
