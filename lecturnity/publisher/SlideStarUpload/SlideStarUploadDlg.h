// SlideStarUploadDlg.h : header file
//

#if !defined(AFX_SLIDESTARUPLOADDLG_H__612E4EFB_2955_4E26_9FBA_C56014D26772__INCLUDED_)
#define AFX_SLIDESTARUPLOADDLG_H__612E4EFB_2955_4E26_9FBA_C56014D26772__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSlideStarUploadDlg dialog

#include "afxinet.h"
#include "Crypto.h"
#include "Ftp.h"

#define BLOCKSIZE 4096
#define WM_STARTSESSION WM_USER+1


class CSlideStarUploadDlg : public CDialog
{
// Construction
   // version 2
   // channel
   CString csActiveChannel;
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
   // remote file
   CString   m_csRemoteFileName;
   // methods
   void LoadFromRegistry(CString pszKey = "");
   //BOOL AddItem(LPCTSTR srcFileName, LPCTSTR dstFileName);

   CFtp ftp;
   BOOL bDoDelete;
   HMODULE hModule;
   //SSH library
   int iArgc;
   LPCTSTR cArgc[32];
   CString csArgc[32];
   BOOL SshUpload(CString csSrcFileName, CString csDstFileName, BOOL bQuit);
   BOOL SshDownload(CString csSrcFileName, CString csDstFileName, BOOL bQuit);
   BOOL SshDelete(CString csSrcFileName, BOOL bQuit = FALSE);
   CSlideStarUploadApp *pApp;

public:
   CSlideStarUploadDlg(CWnd* pParent = NULL);   // standard constructor
   BOOL DoSession();
   BOOL DoFtpSession();
   BOOL DoSshSession();
   CString GetActiveChannel();
   bool ExistSettings();

   // public
   CString csAuthor;
   CString csTitle;
   CString csMediaFile;

   // Dialog Data
   //{{AFX_DATA(CSlideStarUploadDlg)
	enum { IDD = IDD_SLIDESTARUPLOAD_DIALOG };
   CStatic   m_stcBarBottom;
   CProgressCtrl   m_prgProgress;
	CString	m_csFrom;
	CString	m_csTo;
	//}}AFX_DATA

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CSlideStarUploadDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support
   virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
   //}}AFX_VIRTUAL

// Implementation
protected:
   HICON m_hIcon;

   // Generated message map functions
   //{{AFX_MSG(CSlideStarUploadDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
   afx_msg void OnPaint();
   afx_msg HCURSOR OnQueryDragIcon();
   virtual void OnCancel();
	afx_msg void OnClose();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
private:
   _TCHAR m_tszCurrentDir[MAX_PATH];
   CWinThread *m_Thread;
   DWORD m_dwMediaFileLength;

   CString GetFileName(CString csFilePath);
   CString GetHostName();
   BOOL AskForPassword();
   BOOL CompareFiles(CString csSrcFile, CString csDstFile);
   BOOL IsMediaUrlOk(CString &csURL);
   void DrawLogo();
   DWORD GetFileLength(CString csFile);
   int CallSsh(int argc, _TCHAR** argv, HWND handle, int iDelete);
   void SetInfoText(CString csFrom, CString csTo);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLIDESTARUPLOADDLG_H__612E4EFB_2955_4E26_9FBA_C56014D26772__INCLUDED_)
