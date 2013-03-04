#if !defined(AFX_EXPORTPROGRESSDIALOG_H__7DFC2711_0931_4C49_8ABE_B2818901B4A5__INCLUDED_)
#define AFX_EXPORTPROGRESSDIALOG_H__7DFC2711_0931_4C49_8ABE_B2818901B4A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "..\Studio\resource.h"
#include "lutils.h"

//#include "editorDoc.h"
class CEditorProject; // to avoid circular includes

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CExportProgressDialog 

class CExportProgressDialog : public CDialog, public ExportProgressListener
{
// Konstruktion
public:
    //CExportProgressDialog(CWnd* pParent = NULL);   // Standardkonstruktor
	CExportProgressDialog(CEditorProject *pProject, CWnd* pParent = NULL);

// Dialogfelddaten
	//{{AFX_DATA(CExportProgressDialog)
	enum { IDD = IDD_EXPORT_PROGRESS };
	CStatic	m_particularLabel;
	CStatic	m_overallLabel;
	CProgressCtrl	m_particularProgress;
	CProgressCtrl	m_overallProgress;
	//}}AFX_DATA

   virtual void SetOverallProgress(int percent);
   virtual void SetOverallSteps(int steps);
   virtual void SetOverallLabel(LPCTSTR lpText);
   virtual void SetOverallLabel(UINT nResourceID);
   virtual void SetParticularProgress(int percent);
   virtual void SetParticularLabel(LPCTSTR lpText);
   virtual void SetParticularLabel(UINT nResourceID);
   void SetTotalVideoCount(int nCount);
   void SetHasVideo(bool bHasVideo);

   virtual bool ExportWasCancelled() {return m_bExportCancelled;}

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CExportProgressDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

   
public:
   enum
   {
      END_DIALOG = 99
   };

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CExportProgressDialog)
	virtual BOOL OnInitDialog();
   afx_msg LRESULT OnCheckEndDialog(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnExternalUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    CEditorProject *m_pProject;
    CString m_overallString;
    CString m_particularString;
    int     m_totalVideoCount;
    int     m_currentVideoCount;
    // True if export has a normal video and not only clips
    bool    m_bHasVideo;
    bool    m_bExportCancelled;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EXPORTPROGRESSDIALOG_H__7DFC2711_0931_4C49_8ABE_B2818901B4A5__INCLUDED_
