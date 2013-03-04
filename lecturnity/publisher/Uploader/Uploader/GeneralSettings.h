//#if !defined(AFX_GENERALSETTINGS_H__3A7D2DA4_D54D_4529_BC20_8BFB79A846C6__INCLUDED_)
//#define AFX_GENERALSETTINGS_H__3A7D2DA4_D54D_4529_BC20_8BFB79A846C6__INCLUDED_

#pragma once

#include "MyTabCtrl.h"
#include "afxwin.h"
#include "Registry.h"
#include "ListCtrlRadio.h"

// CGeneralSettings dialog

class CGeneralSettings : public CDialog {
    DECLARE_DYNAMIC(CGeneralSettings)

public:
    CGeneralSettings(CWnd* pParent = NULL);   // standard constructor
    CGeneralSettings(CString csChannelName, 
        TransferParam tcTempChannel, TransferParam tcTempMedia, 
        TransferMetadata tmTempMetadata, CWnd* pParent = NULL);
    virtual ~CGeneralSettings();

    // Dialog Data
    //{{AFX_DATA(CGeneralSettings)
    enum { IDD = IDD_MAINSETTINGS };
private:
    CListCtrlRadio m_ListControl;
    CButton	m_btnRemove;
    CButton	m_btnDelete;
    CStatic	m_stcBarBottom;
public:
    MyTabCtrl m_TabControl;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGeneralSettings)
public:
    virtual BOOL DestroyWindow();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL

    // Generated message map functions
    //{{AFX_MSG(CGeneralSettings)
    afx_msg void OnRemove();
    afx_msg void OnDelete();
    afx_msg void OnImport();
    afx_msg void OnNew();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

private:
    //Methods
    CString GetActiveChannel();
    void GetLastChannel();
    BOOL RemoveChannel(int iChannel);
    LONG DeleteKey(CString csKey);
    void GetChannels();
    void SelectItem(int iItem);
    void ClearPages();
    void SelectChannel(CString &csChannel);
    void SetAsActiveChannel();
    BOOL CompareFiles(CString csSrcFile, CString csDstFile);
    void LoadFromRegistry(CString pszKey);
    BOOL DeleteChannel(CString pszKey);
    BOOL AskForPassword();
    CString GetFileName(CString csFilePath);
    void EnableControls();
    BOOL LoadStringResource(CString &cszString, UINT nID);
    void ChangeStatus(int nItem, bool bActive);
    CString GetListItemData(int nItem);
    void InsertTemporaryChannel();

    // Channel/Media settings
    TransferParam m_tpChannel;
    TransferParam m_tpMedia;

    CRegistry m_regEntry;
    int m_iLastChannel;
    int m_iLastChannelTmp;
    HWND m_HWaitDlg;
    CUploaderApp *pApp;

    //Temporary channel
    bool m_bTemporary;
    bool m_bTempSel;
    CString m_csTempChannelName;
    TransferParam m_tcTempChannel;
    TransferParam m_tcTempMedia;
    TransferMetadata m_tmTempMetadata;

public:
    BOOL UploadChannel(CString pszKey, CString csFile);
    void Refresh();
    TransferParam GetTemporaryChannel();
    TransferParam GetTemporaryMedia();
    TransferMetadata GetTemporaryMetadata();
    void SetTemporaryChannel(TransferParam tpTempChannel);
    void SetTemporaryMedia(TransferParam tpTempMedia);
    void SetTemporaryMetadata(TransferMetadata tpTempMetadata);
    TransferParam  GetActiveChannelData();
    TransferParam GetActiveMediaData();
    bool IsTempChannelSelected();
};
//#endif