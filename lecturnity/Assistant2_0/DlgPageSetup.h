#if !defined(AFX_DLGPAGESETUP_H__A2EA7A4B_AFB2_46AE_AF90_67AAA15A8454__INCLUDED_)
#define AFX_DLGPAGESETUP_H__A2EA7A4B_AFB2_46AE_AF90_67AAA15A8454__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPageSetup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPageSetup dialog

class CDlgPageSetup : public CDialog
{
// Construction
public:
	
   CDlgPageSetup(CWnd* pParent = NULL,UINT nPageWidth=0, UINT nPageHeight=0, COLORREF clrBackground=RGB(255,255,255));

private:
   CComboBox	m_comboSlide;
   CButton	m_radioSlidesPortrait;
   CButton	m_radioSlidesLandscape;
	CString     m_csWidth;
   CString     m_csHeight;
   int		   m_iSlidesPortrait;
   int		   m_iSlidesLandscape;
	CSpinButtonCtrl	m_spinWidth;
	CEdit	      m_edtWidth;
   CSpinButtonCtrl	m_spinHeight;
   CEdit	      m_edtHeight;
   CXTColorPicker	m_cpBackColor;
   
   UINT m_nPageWidth;
   UINT m_nPageHeight;
   UINT m_clrBackground;

public:
   UINT GetHeight();
   UINT GetWidth();
   COLORREF GetBackgroundColor();
  
   // Dialog Data
	//{{AFX_DATA(CDlgPageSetup)
	enum { IDD = IDD_PAGE_SETUP };
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPageSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPageSetup)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioSlidesPortrait();
	afx_msg void OnRadioSlidesLandscape();
	afx_msg void OnCloseupComboSlide();
	afx_msg void OnChangeWidth();
	afx_msg void OnChangeHeight();
	virtual void OnOK();
  
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPAGESETUP_H__A2EA7A4B_AFB2_46AE_AF90_67AAA15A8454__INCLUDED_)
