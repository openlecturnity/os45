#if !defined(AFX_MISSINGFONTSDLG_H__E3B89E06_E58E_4D1E_9CCA_802B54B512F4__INCLUDED_)
#define AFX_MISSINGFONTSDLG_H__E3B89E06_E58E_4D1E_9CCA_802B54B512F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MissingFontsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMissingFontsDlg dialog

#include "..\Studio\Resource.h"

class CMissingFontsDlg : public CDialog
{
// Construction
public:
	void AddMissingFont(CString &csFontName, CString &csPages, CString &csType);
	CMissingFontsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMissingFontsDlg)
	enum { IDD = IDD_MISSING_FONTS };
	CListCtrl	m_MissingFontsList;
   CButton	m_cbbDoNotDisplayAgain;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMissingFontsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMissingFontsDlg)
   virtual BOOL OnInitDialog();
	afx_msg void OnDialogMissingFontsOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
  typedef struct MissingFontInfoStruct
      {
      CString csFontName; 
      CString csPages; 
      CString csType;
   } sMissingFontInfo, *psMissingFontInfo;
   CArray<psMissingFontInfo,psMissingFontInfo> m_MissingFontsArray;
   void PopulateMissingFontsList();
   void CheckCheckBox();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MISSINGFONTSDLG_H__E3B89E06_E58E_4D1E_9CCA_802B54B512F4__INCLUDED_)
