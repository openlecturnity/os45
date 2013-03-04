#pragma once

#include "XPGroupBox.h"
#include "Crypto.h"
#include "afxwin.h"
#include "Uploader.h"

#define FTP_SERVICE 0
#define HTTP_SERVICE 0

// CTransferSettings dialog

class CTransferSettings : public CDialog
{
   DECLARE_DYNAMIC(CTransferSettings)

public:
   CTransferSettings(CWnd* pParent = NULL);   // standard constructor
   virtual ~CTransferSettings();

   // Dialog Data
   enum { IDD = IDD_SETTINGS_DIALOG };
private:
   CEdit m_edtChannel;
   CEdit m_edtMediaFileUrl;
   CEdit m_edtServer;
   CEdit m_edtUsername;
   CEdit m_edtPassword;
   CEdit m_edtChannelDirectory;
   CEdit m_edtMediaDirectory;
   CComboBox m_cboService;
   CEdit m_edtPort;
   CButton m_ckbtnSavePassword;
   /*CComboBox m_cboSSHVersion;
   CComboBox m_cboIPVersion;
   CButton m_ckbtnCompression;
   CButton m_ckbtnPrivateKey;*/
   CXPGroupBox m_gbWhiteRectangle;

   //Message map functions
public:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
   afx_msg void OnSelchangeService();
   afx_msg void OnPrivateKeyFile();
   afx_msg void OnChangeXmlurl();
   afx_msg void OnChangeMp4url();
   afx_msg void OnChangeHost();
   afx_msg void OnChangeChanneldir();
   afx_msg void OnChangeMp4dir();

   DECLARE_MESSAGE_MAP()

   //Members
private:
   void LoadFromRegistry(CString pszKey);
   void SaveToRegistry(CString pszKey);
   void CheckInputTextForBlanks(CString &csInputText);

public:
   void SetChannelName(CString csChannelName);
   CString GetChannelName();
   CString GetChannelURL();
   void SetTempChannel(bool bTempChannel){m_bTemporaryChannel = bTempChannel;}
   void GetTempChannelSettings(TransferParam &tcTempChannel);
   void SetTempChannelSettings(TransferParam tcTempChannel);

   //Attributes
private:
   bool m_isKeyPressed;
   CString m_csPrivateKeyFile;
   CString m_csChannelName;
   CCrypto m_Crypto;
   bool m_bTemporaryChannel;
   TransferParam m_tcTempChannel;
};
