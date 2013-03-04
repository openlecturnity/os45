#if !defined(AFX_PASSWORDDLG_H__00B39458_AA6A_48B8_8E25_92D00F21DF7E__INCLUDED_)
#define AFX_PASSWORDDLG_H__00B39458_AA6A_48B8_8E25_92D00F21DF7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PasswordDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog

class CPasswordDlg : public CDialog
{
// Construction
public:
   CPasswordDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CPasswordDlg)
   enum { IDD = IDD_PASSWORD_DIALOG };
   CString   m_csUsername;
   CString   m_csPassword;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CPasswordDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CPasswordDlg)
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDDLG_H__00B39458_AA6A_48B8_8E25_92D00F21DF7E__INCLUDED_)
