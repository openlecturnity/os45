#if !defined(AFX_GENERALSETTINGS_H__3A7D2DA4_D54D_4529_BC20_8BFB79A846C6__INCLUDED_)
#define AFX_GENERALSETTINGS_H__3A7D2DA4_D54D_4529_BC20_8BFB79A846C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GeneralSettings.h : header file
//
#include "MyTabCtrl.h"
#include "Registry.h"
#include "TransferSettings.h"
#include "ListCtrlRadio.h"

/////////////////////////////////////////////////////////////////////////////
// CGeneralSettings dialog

class CGeneralSettings : public CDialog
{
   CImageList m_ilRadioButtons;

   CWinThread *m_Thread;
   void DrawLogo();
   int iLastChannel;
   int iLastChannelTmp;
   // registry
   int       m_iService;
   CString   m_csChannel;
   CString   m_csServer;
   UINT      m_nPort;
   CString   m_csUsername;
   CString   m_csPassword;
   CString   m_csMediaFileUrl;
   CString   m_csMediaDirectory;
   CString   m_csChannelDirectory;
   CString   m_csPrivateKeyFile;
   BOOL      m_bSavePassword;
	int		 m_iSSHVersion;
	int		 m_iIPVersion;
	BOOL	    m_bCompression;
	BOOL	    m_bPrivateKey;
   // methods
   void LoadFromRegistry(CString pszKey);
   BOOL DeleteChannel(CString pszKey);
   BOOL AskForPassword();
   CString GetFileName(CString csFilePath);
   //SSH library
   int iArgc;
   LPCSTR cArgc[32];
   CString csArgc[32];

   BOOL SshUpload(CString csSrcFileName, CString csDstFileName, BOOL bQuit);
   BOOL SshDelete(CString csSrcFileName, BOOL bQuit);
   int CallSsh(int argc, WCHAR** argv, HWND handle, int iDelete);
   int CallSsh(int argc, char** argv, HWND handle, int iDelete);
   CPodcasterApp *pApp;
   // Construction
public:
	CGeneralSettings(CWnd* pParent = NULL);   // standard constructor
   BOOL UploadChannel(CString pszKey, CString csFile);
   void Refresh();

   HWND m_HWaitDlg;

// Dialog Data
	//{{AFX_DATA(CGeneralSettings)
	enum { IDD = IDD_MAINSETTINGS };
	CButton	m_btnRemove;
	CButton	m_btnDelete;
	MyTabCtrl	m_TabControl;
	CListCtrlRadio	m_ListControl;
	CStatic	m_stcBarBottom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeneralSettings)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   CString GetActiveChannel();
	void GetChannels();
   void GetLastChannel();
   BOOL RemoveChannel(int iChannel);
   LONG DeleteKey(CString csKey);

	// Generated message map functions
	//{{AFX_MSG(CGeneralSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnRemove();
	afx_msg void OnDelete();
	afx_msg void OnImport();
	afx_msg void OnNew();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void EnableControls();
   CRegistry reg;

   void SelectItem(int iItem);
   void ClearPages();
   void SelectChannel(CString &csChannel);
	void SetAsActiveChannel();
   BOOL CompareFiles(CString csSrcFile, CString csDstFile);

   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
//   afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERALSETTINGS_H__3A7D2DA4_D54D_4529_BC20_8BFB79A846C6__INCLUDED_)
