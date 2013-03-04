// changeurlDlg.h : Header-Datei
//

#if !defined(AFX_CHANGEURLDLG_H__91A4EF19_CF9C_4091_B107_AA1D8793BA74__INCLUDED_)
#define AFX_CHANGEURLDLG_H__91A4EF19_CF9C_4091_B107_AA1D8793BA74__INCLUDED_

#include "changeurlengine.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CChangeurlDlg Dialogfeld

class CChangeurlDlg : public CDialog, public CChangeUrlEngine
{
// Konstruktion
public:
	CChangeurlDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CChangeurlDlg)
	enum { IDD = IDD_CHANGEURL_DIALOG170 };
	CStatic	m_lbDocType;
	CStatic	m_lbScorm;
	CEdit	m_edSmilFile;
	CStatic	m_lbVersion;
	CButton	m_rbStreaming;
	CButton	m_rbLocal;
	CButton	m_rbHttp;
	CStatic	m_lbUrlStreaming;
	CStatic	m_lbUrlHttp;
	CStatic	m_lbUrlHint;
	CStatic	m_lbModify;
	CEdit	m_edUrlStreaming;
	CEdit	m_edUrlHttp;
	CButton	m_btModify;
	CButton	m_frmStep3;
	CButton	m_frmStep2;
	CButton	m_frmStep1;
	CEdit	m_edFilename;
	CButton	m_btBrowse;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CChangeurlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CChangeurlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTypeLocal();
	afx_msg void OnTypeHttp();
	afx_msg void OnTypeStreaming();
	afx_msg void OnBrowse();
	afx_msg void OnModify();
	afx_msg void OnChangeUrl();
	afx_msg void OnCloseApp();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   virtual void DisableControls();
   virtual void EnableStep3(BOOL bEnable=TRUE);
   virtual void EnableStep2(BOOL bEnable=TRUE, UINT nReplayType=REPLAY_TYPE_LOCAL);

   virtual void LogMessage(CString csMsg, UINT nBeep = 0);
   virtual bool QuestionMessage(CString csMsg);

   virtual CString GetDocumentName();
   virtual void SetDocumentName(LPCTSTR tszDocName);
   virtual CString GetUrlHttp();
   virtual void SetUrlHttp(LPCTSTR tszUrlHttp);
   virtual CString GetUrlStreaming();
   virtual void SetUrlStreaming(LPCTSTR tszUrlStreaming);
   virtual void SetDocType(LPCTSTR tszDocType);
   virtual void SetDocVersion(LPCTSTR tszDocVersion);
   virtual void SetScormSupport(LPCTSTR tszScormSupport);
   virtual void SetSmilFileName(LPCTSTR tszSmilFileName);
   virtual bool RequestSmilFileName(CString &csSmilFileName);
   virtual void SetWaitCursor(bool bDisplay);
   virtual CWnd *GetCWnd() { return this; }

   virtual UINT GetTargetType();

   bool GetFileFromFileDialog(CString &csFileName, DWORD dwFilterId);

   // The path to the HTML/SMI/js files, including a backslash
   //CString m_csPath;
   // The base name of the document, excluding any extensions or additions
   //CString m_csBaseName;

   //UINT m_nDocType; // DOCTYPE_WMT/DOCTYPE_REAL
   //bool m_bHasClips; // isClipsSeparated
   //UINT m_nNumberOfClips; // numOfClips
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_CHANGEURLDLG_H__91A4EF19_CF9C_4091_B107_AA1D8793BA74__INCLUDED_)
