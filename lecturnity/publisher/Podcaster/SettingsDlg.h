#if !defined(AFX_SETTINGSDLG_H__D20CC7E6_9784_481F_891B_5286CD087F9E__INCLUDED_)
#define AFX_SETTINGSDLG_H__D20CC7E6_9784_481F_891B_5286CD087F9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog
#include "Crypto.h"


#define FTP_SERVICE 0
#define HTTP_SERVICE 0

class CSettingsDlg : public CDialog
{
   COLORREF color;
   CBrush brush;
// Construction
public:
   HICON m_hIcon;
   CSettingsDlg(CWnd* pParent = NULL);   // standard constructor
   void LoadFromRegistry(CString pszKey);
   void SaveToRegistry(CString pszKey);


   LPTSTR strChannel; 

   CString   m_csPrivateKeyFile;

// Dialog Data
   //{{AFX_DATA(CSettingsDlg)
	enum { IDD = IDD_SETTINGS_DIALOG };
	CButton	 m_btnPrivateKey;
	CButton	 m_btnCompression;
	CComboBox m_boxIPVersion;
	CComboBox m_boxSSHVersion;
   CStatic   m_stcBarBottom;
   int       m_iService;
   CString   m_csChannel;
   CString   m_csServer;
   UINT      m_nPort;
   CString   m_csUsername;
   CString   m_csPassword;
   CString   m_csMediaFileUrl;
   CString   m_csMediaDirectory;
   CString   m_csChannelDirectory;
   BOOL      m_bSavePassword;
	int		 m_iSSHVersion;
	int		 m_iIPVersion;
	BOOL	    m_bCompression;
	BOOL	    m_bPrivateKey;
	//}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CSettingsDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CSettingsDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnSelchangeService();
   afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	afx_msg void OnDestroy();
	virtual void OnCancel();
	afx_msg void OnPrivateKeyFile();
	afx_msg void OnChangeXmlurl();
	afx_msg void OnChangeMp4url();
	afx_msg void OnChangeHost();
	afx_msg void OnChangeChanneldir();
	afx_msg void OnChangeMp4dir();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
private:
   void CheckInputTextForBlanks(CString &csInputText);

private:
   CCrypto m_Crypto;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSDLG_H__D20CC7E6_9784_481F_891B_5286CD087F9E__INCLUDED_)
