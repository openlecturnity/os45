// LmdManipDlg.h : Header-Datei
//

#if !defined(AFX_LMDMANIPDLG_H__6807EEF3_B698_4053_A812_4158386B03B9__INCLUDED_)
#define AFX_LMDMANIPDLG_H__6807EEF3_B698_4053_A812_4158386B03B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLmdManipDlg Dialogfeld

#include "LmdTree.h"

#include "SgmlParser.h"

class CLmdManipDlg : public CDialog
{
// Konstruktion
public:
	CLmdManipDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CLmdManipDlg)
	enum { IDD = IDD_LMDMANIP_DIALOG };
	CButton	m_btOk;
	CLmdTree	m_lmdTree;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CLmdManipDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CLmdManipDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   afx_msg void RemoveFromStructure();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   void LoadLmdFile(const _TCHAR *tszFile);

private:
   SgmlFile *m_pLmdFile;
   CString   m_csLmdFileName;
   CMenu    m_pagePopup;
   bool     m_bHasChanged;

   bool BuildTree(SgmlElement *pRoot);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_LMDMANIPDLG_H__6807EEF3_B698_4053_A812_4158386B03B9__INCLUDED_)
