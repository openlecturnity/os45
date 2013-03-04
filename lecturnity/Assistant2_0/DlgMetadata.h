#if !defined(AFX_DLGMETADATA_H__599CF7C7_2ABA_459A_9F6E_3EE128E9E699__INCLUDED_)
#define AFX_DLGMETADATA_H__599CF7C7_2ABA_459A_9F6E_3EE128E9E699__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMetadata.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMetadata dialog

#include "..\Studio\Resource.h"

//#include "XPGroupBox.h"
class CXPGroupBox;

class CDlgMetadata : public CDialog
{
   // Construction
public:
   CDlgMetadata(CWnd* pParent = NULL);   // standard constructor
   virtual ~CDlgMetadata();
   CString GetTitle(){return m_csTitle;}
   CString GetAuthor(){return m_csAuthor;}
   CString GetProducer(){return m_csProducer;}
   CString GetKeywords(){return m_csKeywords;}
   CString GetRecordDate(){return m_csRecordDate;}
   CString GetRecordTime(){return m_csRecordTime;}
   CString GetRecordLength(){return m_csRecordLength;}
   CString GetRecordPath(){return m_csRecordPath;}

   void Init(CString &csTitle, CString &csAuthor, 
      CString &csProducer, CString &csKeywords);
   void SetRecordInfo(CString &csRecordDate, CString &csRecordTime, 
      CString &csRecordLength, CString &csRecordPath);
   void SetTitleVisibility(bool bVisibility);

private:

   CString	m_csTitle;
   CString	m_csAuthor;
   CString	m_csProducer;
   CString	m_csKeywords;

   CString	m_csRecordDate;
   CString	m_csRecordTime;
   CString	m_csRecordLength;
   CString	m_csRecordPath;

   bool	m_bTitleVisibility;
   CXPGroupBox	*m_gbCustomGroupBox;

   // Dialog Data
   //{{AFX_DATA(CDlgMetadata)
   enum { IDD = IDD_METADATA };

   //}}AFX_DATA

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CDlgMetadata)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL
   void HideTitle();
   void SetNewPosition();
   void HideLabel(UINT nID);
   void HideEditBox(UINT nID);
   void ProcessRecordPath(CString& csRecordPath);
   // Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CDlgMetadata)
   virtual BOOL OnInitDialog();
   afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMETADATA_H__599CF7C7_2ABA_459A_9F6E_3EE128E9E699__INCLUDED_)
