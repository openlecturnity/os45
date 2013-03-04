#if !defined(AFX_LRDDIALOG_H__BCFB82CE_D7A6_40A6_B279_7C10D29F0300__INCLUDED_)
#define AFX_LRDDIALOG_H__BCFB82CE_D7A6_40A6_B279_7C10D29F0300__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LrdDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLrdDialog dialog
#include "ReadOnlyEdit.h"

class CLrdDialog : public CDialog
{
	COLORREF color;
	CBrush brush;
// Construction
public:
   HICON m_hIcon;
   CLrdDialog(CWnd* pParent = NULL);   // standard constructor
   LPTSTR FindFirstLrdFile();
   void ParseLrd();

// Dialog Data
   //{{AFX_DATA(CLrdDialog)
	enum { IDD = IDD_LRD_DIALOG };
   CReadOnlyEdit   m_edtAuthor2;
   CReadOnlyEdit   m_edtTitle2;
   CReadOnlyEdit   m_edtLRDocument2;
   CButton   m_btnBrowseButton;
   CString   m_csLRDocument;
   CString   m_csTitle;
   CString   m_csAuthor;
   int      m_iOption;
	//}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CLrdDialog)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CLrdDialog)
   afx_msg void OnBrowse();
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   virtual void OnCancel();
   afx_msg void OnOption();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
private:
   TCHAR m_tszCurrentDirectory[MAX_PATH];
   TCHAR m_tszLrdFile[MAX_PATH];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LRDDIALOG_H__BCFB82CE_D7A6_40A6_B279_7C10D29F0300__INCLUDED_)
