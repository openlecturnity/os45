// GeneralSettings.cpp : implementation file
//

#include "stdafx.h"
#include "Uploader.h"
#include "GeneralSettings.h"
#include "MfcUtils.h"
#include "Resource.h"
#include "MetadataPage.h"
#include "TransferPage.h"
#include "TransferConfigDlg.h"
#include "XmlValidator.h"
#include "ChannelMetadata.h"
#include "Ftp.h"
#include "Ssh.h"
#include "PasswordDlg.h"
#include "WaitDlg.h"
#include "Crypto.h"
#include <string>


// CGeneralSettings dialog

IMPLEMENT_DYNAMIC(CGeneralSettings, CDialog)

CGeneralSettings::CGeneralSettings(CWnd* pParent /*=NULL*/)
: CDialog(CGeneralSettings::IDD, pParent) {
    m_HWaitDlg = NULL;
    m_bTemporary = false;
    m_bTempSel = false;
}

CGeneralSettings::CGeneralSettings(CString csChannelName, 
                                   TransferParam tcTempChannel,
                                   TransferParam tcTempMedia,
                                   TransferMetadata tmTempMetadata, CWnd* pParent /*=NULL*/)
                                   : CDialog(CGeneralSettings::IDD, pParent) {
    m_HWaitDlg = NULL;
    m_bTemporary = true;
    m_bTempSel = true;
    m_tcTempChannel = tcTempChannel;
    m_tcTempMedia = tcTempMedia;
    m_tmTempMetadata = tmTempMetadata;
    m_csTempChannelName = csChannelName;
}

CGeneralSettings::~CGeneralSettings() {
}

void CGeneralSettings::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CGeneralSettings)
    DDX_Control(pDX, IDB_REMOVE, m_btnRemove);
    DDX_Control(pDX, IDB_DELETE, m_btnDelete);
    DDX_Control(pDX, IDC_PAGES, m_TabControl);
    DDX_Control(pDX, IDC_CHANNELLIST, m_ListControl);
    DDX_Control(pDX, IDC_BARBOTTOM, m_stcBarBottom);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGeneralSettings, CDialog)
    //{{AFX_MSG_MAP(CGeneralSettings)
    ON_BN_CLICKED(IDB_REMOVE, OnRemove)
    ON_BN_CLICKED(IDB_DELETE, OnDelete)
    ON_BN_CLICKED(IDB_NEW, OnNew)
    ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDB_IMPORT, OnImport)
    ON_NOTIFY(NM_CLICK, IDC_CHANNELLIST, OnClickList)
    ON_WM_ACTIVATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CGeneralSettings message handlers

BOOL CGeneralSettings::OnInitDialog()  {
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    pApp = (CUploaderApp *)AfxGetApp();
    SetIcon(AfxGetApp()->LoadIcon(IDI_UPLOADER), TRUE); 

    DWORD dwID[] = {
        IDL_CHANNELS,
        IDCANCEL,
        IDB_NEW,
        IDB_IMPORT,
        IDB_REMOVE,
        IDB_DELETE,
        -1 };

        MfcUtils::Localize(this, dwID);

        CString csCaption;
        csCaption.LoadString(IDD_MAINSETTINGS);
        SetWindowText(csCaption);

        m_TabControl.InitDialogs();
        csCaption.LoadString(IDT_METADATA);
        m_TabControl.InsertItem(0, csCaption);

        csCaption.LoadString(IDT_TRANSFER);
        m_TabControl.InsertItem(1, csCaption);

        m_TabControl.ActivateTabDialogs();


        m_ListControl.SetExtendedStyle(LVS_EX_FULLROWSELECT);

        csCaption.LoadString(IDL_TITLE2);
        m_ListControl.InsertColumn(0, csCaption, LVCFMT_LEFT, 148, -1);

        csCaption.LoadString(IDS_TEMPORARY);
        m_ListControl.InsertColumn(1, csCaption, LVCFMT_LEFT, 70, -1);

        csCaption.LoadString(IDL_STATUS);
        m_ListControl.InsertColumn(2, csCaption, LVCFMT_LEFT, 50, -1);

        GetChannels();

        EnableControls();

        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
}

CString CGeneralSettings::GetActiveChannel() {
    HKEY hKey = NULL;
    TCHAR tstrSubKey[MAX_PATH];
    DWORD dwSubKey = MAX_PATH - 1;
    int iActive = 0;
    FILETIME ft;

    CString csResult;
    csResult.Empty();

    LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER,
        UPLOADER_KEY, 0, KEY_ENUMERATE_SUB_KEYS, &hKey);

    if (lResult != ERROR_SUCCESS) {
        csResult;
    }

    DWORD i = 0;
    while (TRUE) {
        csResult.Empty();
        dwSubKey = MAX_PATH - 1;
        lResult = RegEnumKeyEx(hKey, i++, tstrSubKey,
            &dwSubKey, NULL, NULL, NULL, &ft);
        if (lResult == ERROR_NO_MORE_ITEMS) {
            break;
        }
        if (lResult != ERROR_SUCCESS) {
            break;
        }
        csResult = CString(tstrSubKey, dwSubKey);
        // get active value
        if (m_regEntry.Open(csResult) == ERROR_SUCCESS) {
            iActive = m_regEntry.GetProfileInt(_T("Active"), 0);   
            m_regEntry.Close();
        }
        if (iActive != 0) {
            // found
            break;
        }
    }
    RegCloseKey(hKey);
    return csResult;
}

void CGeneralSettings::GetLastChannel() {
    HKEY hKey = NULL;
    TCHAR tstrSubKey[MAX_PATH];
    DWORD dwSubKey = MAX_PATH - 1;
    FILETIME ft;

    LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER,
        UPLOADER_KEY, 0, KEY_ENUMERATE_SUB_KEYS, &hKey);

    if (lResult != ERROR_SUCCESS) {
        return;
    }

    DWORD i = 0;
    m_iLastChannel = -1;
    while (TRUE) {
        dwSubKey = MAX_PATH - 1;
        lResult = RegEnumKeyEx(hKey, i++, tstrSubKey, &dwSubKey, NULL, NULL, NULL, &ft);
        if (lResult == ERROR_NO_MORE_ITEMS) {
            break;
        }
        if (lResult != ERROR_SUCCESS) {
            break;
        }
        // calculate last channel
        swscanf_s(tstrSubKey, _T("Channel %d"), &m_iLastChannelTmp);
        if (m_iLastChannelTmp > m_iLastChannel) {
            m_iLastChannel = m_iLastChannelTmp;
        }
    }
    RegCloseKey(hKey);
}

void CGeneralSettings::GetChannels() {
    HKEY hKey = NULL;
    TCHAR tstrSubKey[MAX_PATH];
    DWORD dwSubKey = MAX_PATH - 1;
    FILETIME ft;

    LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER,
        UPLOADER_KEY, 0, KEY_ENUMERATE_SUB_KEYS, &hKey);

    if (lResult != ERROR_SUCCESS) {
        if (m_bTemporary) {
            InsertTemporaryChannel();
        }
        return;
    }

    DWORD i = 0;
    m_iLastChannel = -1;
    m_ListControl.DeleteAllItems();
    while (TRUE) {
        dwSubKey = MAX_PATH - 1;
        lResult = RegEnumKeyEx(hKey, i, tstrSubKey, &dwSubKey, NULL, NULL, NULL, &ft);
        if (lResult == ERROR_NO_MORE_ITEMS) {
            break;
        }
        if (lResult != ERROR_SUCCESS) {
            break;
        }
        // insert item
        CString csItem = _T("");
        m_ListControl.InsertItem(i, csItem);

        // use item data to store the channel name
        CString csText;
        int curPos= 0;
        CString csResToken;

        csText = tstrSubKey;

        while (csText.Tokenize(_T(" "),curPos) != _T("")) {
            csResToken = csText.Tokenize(_T(" "),curPos);
        }

        DWORD dwChannelIndex = ::_ttoi(csResToken);
        m_ListControl.SetItemData(i, dwChannelIndex);

        i++;  // next channel
        // calculate last channel
        swscanf_s(tstrSubKey, _T("Channel %d"), &m_iLastChannelTmp);
        if (m_iLastChannelTmp > m_iLastChannel) {
            m_iLastChannel = m_iLastChannelTmp;
        }
    }
    RegCloseKey(hKey);
    //show wait dialog

    CWaitDlg waitDlg;
    waitDlg.Create(IDD_WAIT_DIALOG);
    m_HWaitDlg = waitDlg.GetSafeHwnd();

    CWnd *wait = FromHandle(m_HWaitDlg);
    wait->ShowWindow(SW_SHOW);
    wait->CenterWindow(this);
    wait->BringWindowToTop();
    wait->UpdateWindow();

    unsigned uCount = m_ListControl.GetItemCount();
    for (unsigned j = 0;j < uCount;j++) {
        CString pszKey = GetListItemData(j);
        if (m_regEntry.Open(pszKey) == ERROR_SUCCESS) {
            int iActive = m_regEntry.GetProfileInt(_T("Active"), 0); 
            m_ListControl.SetState(j, 2, iActive);

            if (m_bTemporary && iActive) {
                m_ListControl.SetState(j, 2, 0);
            }
            m_regEntry.Close();
        }

        // set channel name to channel title
        m_TabControl.GetTransferSettings((LPCTSTR)pszKey);
        // metadata
        CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
        CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];
        meta->GetMetadata(trsp->GetChannelURL(), FALSE);
        m_ListControl.SetItemText(j, 0, meta->GetTitle());    
    }
    if (uCount > 0) {
        CString csKey = GetActiveChannel();
        m_TabControl.GetTransferSettings((LPCTSTR)csKey);
    }
    if (m_bTemporary) {
        InsertTemporaryChannel();
    }
    wait->DestroyWindow();
}

void CGeneralSettings::InsertTemporaryChannel() {
    unsigned uCount = m_ListControl.GetItemCount();

    m_ListControl.InsertItem(uCount, m_csTempChannelName);

    CString csText;
    csText.LoadString(IDS_YES);
    m_ListControl.SetItemText(uCount, 1, csText);
    m_ListControl.SetItemData(uCount, -1);
    m_ListControl.SetFocus();
    m_ListControl.SetItemState(uCount, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
    m_ListControl.SetState(uCount, 2, 1);

    CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
    CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];

    CChannelMetadata dlgMeta;
    CString csTempChannel = pApp->GetTempDirectory() + _T("\\TempChannel.xml");
    dlgMeta.CreateTempChannel(csTempChannel, m_tmTempMetadata);

    meta->SetTempChannel(true);
    meta->SetTempMetadata(m_tmTempMetadata);
    meta->GetMetadata(csTempChannel, FALSE);

    trsp->SetTempChannel(true);
    trsp->SetTemporaryChannel(m_tcTempChannel);
    trsp->SetTemporaryMedia(m_tcTempMedia);
}

LONG CGeneralSettings::DeleteKey(CString csKey) {
    HKEY hKey = NULL;
    LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER,
        UPLOADER_KEY, 0, KEY_ALL_ACCESS, &hKey);  
    if (lResult != ERROR_SUCCESS) {
        return lResult;
    }
    lResult = RegDeleteKey(hKey, csKey);
    RegCloseKey(hKey);  
    return lResult;
}

BOOL CGeneralSettings::RemoveChannel(int iChannel) {
    BOOL bResult = FALSE;

    if (iChannel < 0) {
        return bResult;
    }

    CString csKey = GetListItemData(iChannel);

    LONG lResult = DeleteKey(csKey);
    bResult = (lResult == ERROR_SUCCESS);

    if (bResult) {
        // delete the channel name stored from list
        m_ListControl.DeleteItem(iChannel); // delete from list
        SelectItem(iChannel - 1);
        ClearPages();
    }
    return bResult;
}

void CGeneralSettings::ClearPages() {
    if (m_ListControl.GetItemCount() == 0) {
        CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
        meta->GetMetadata(_T(""), FALSE);
        CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];
        trsp->ClearPage();
    }
}

void CGeneralSettings::OnRemove() {
    CString csQuestion;
    csQuestion.LoadString(IDM_CONFIRMREMOVE);

    if (AfxMessageBox(csQuestion, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES) {
        return;
    }

    int iSelected = m_ListControl.GetNextItem(-1, LVNI_SELECTED);
    if (iSelected != -1) {
        RemoveChannel(iSelected);
    }
    EnableControls();
}

void CGeneralSettings::OnDelete() {
    // TODO: Add your control notification handler code here
    CString csQuestion;
    csQuestion.LoadString(IDM_CONFIRMDELETE);
    if (AfxMessageBox(csQuestion ,MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES) {
        return;
    }

    int iSelected = m_ListControl.GetNextItem(-1, LVNI_SELECTED);

    if (iSelected != -1) {
        // TODO: remove from remote server
        CString csKey = GetListItemData(iSelected);
        if (DeleteChannel(csKey) == TRUE) {
            RemoveChannel(iSelected);
        }
    }
    EnableControls();
}

void CGeneralSettings::SelectChannel(CString &csChannel) {
    unsigned uCount = m_ListControl.GetItemCount();
    if (uCount > 0) {
        for (unsigned i = 0;i < uCount;i++) {
            CString pszKey = GetListItemData(i);

            if (csChannel.Compare(pszKey) == 0) {
                m_ListControl.SetState(i, 2, 0);
                SelectItem(i);
                if (m_regEntry.Open(pszKey) == ERROR_SUCCESS) {
                    m_regEntry.WriteProfileInt(_T("Active"), 0);   
                    m_regEntry.Close();
                }
                m_iLastChannel++;
                return;
            }
        }
    }       
}

void CGeneralSettings::Refresh() {
    CWaitDlg waitDlg;
    waitDlg.Create(IDD_WAIT_DIALOG);
    m_HWaitDlg = waitDlg.GetSafeHwnd();

    CWnd *wait = FromHandle(m_HWaitDlg);
    wait->ShowWindow(SW_SHOW);
    wait->CenterWindow(this);
    wait->BringWindowToTop();
    wait->UpdateWindow();
    unsigned uCount = m_ListControl.GetItemCount();
    if (uCount > 0) {
        for (unsigned i = 0;i < uCount;i++) {
            UINT uState = m_ListControl.GetItemState(i, LVIS_SELECTED);
            bool bSelected = (uState == LVIS_SELECTED);
            if (bSelected) {
                // transfer
                CString csKey = GetListItemData(i);
                m_TabControl.GetTransferSettings((LPCTSTR)csKey);

                // metadata
                CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
                CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];
                meta->GetMetadata(trsp->GetChannelURL(), FALSE);
                m_ListControl.SetItemText(i, 0, meta->GetTitle());

                wait->ShowWindow(SW_HIDE);
                return;
            }
        }
    }
    wait->DestroyWindow();
}

void CGeneralSettings::OnImport() {
    // TODO: Add your control notification handler code here

    GetLastChannel();

    CString csNewChannel;
    csNewChannel.Format(_T("Channel %d"), m_iLastChannel + 1);
    int iAppend = m_ListControl.GetItemCount();

    CTransferConfigDlg dlgConfigChannel(1);
    CTransferConfigDlg dlgConfigMedia(2);

    dlgConfigChannel.SetChannelName(csNewChannel);
    dlgConfigMedia.SetChannelName(csNewChannel);

    if (dlgConfigChannel.DoModal() == IDOK) {
        if (dlgConfigMedia.DoModal() == IDOK) {
            CWaitDlg waitDlg;
            waitDlg.Create(IDD_WAIT_DIALOG);
            m_HWaitDlg = waitDlg.GetSafeHwnd();

            CWnd *wait = FromHandle(m_HWaitDlg);
            wait->ShowWindow(SW_SHOW);
            wait->CenterWindow(this);
            wait->BringWindowToTop();
            wait->UpdateWindow();

            CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
            if (meta->GetMetadata(dlgConfigChannel.GetChannelURL()) == TRUE) {
                m_ListControl.InsertItem(iAppend, _T("??"));
                m_ListControl.SetItemText(iAppend, 0, meta->GetTitle());

                // the new channel
                DWORD dwChannelIndex = m_iLastChannel + 1;
                m_ListControl.SetItemData(iAppend, dwChannelIndex);

                SelectChannel(csNewChannel);
                SetAsActiveChannel();
                m_iLastChannel++;
            } else {
                DeleteKey(csNewChannel);
                SelectItem(0);
            }
            wait->DestroyWindow();
        }
    }	
    EnableControls();
}

void CGeneralSettings::OnNew() {
    CXmlValidator valid;
    CString csFile;

    BOOL bDelete = TRUE;
    GetLastChannel();

    CString csNewChannel;
    csNewChannel.Format(_T("Channel %d"), m_iLastChannel + 1);
    int iAppend = m_ListControl.GetItemCount();

    CTransferConfigDlg dlgConfigChannel(1);
    CTransferConfigDlg dlgConfigMedia(2);
    dlgConfigChannel.SetChannelName(csNewChannel);
    dlgConfigChannel.IsNewChannel(true);
    dlgConfigMedia.SetChannelName(csNewChannel);

    CWaitDlg waitDlg;
    waitDlg.Create(IDD_WAIT_DIALOG);
    m_HWaitDlg = waitDlg.GetSafeHwnd();

    CWnd *wait=FromHandle(m_HWaitDlg);
    if (dlgConfigChannel.DoModal() == IDOK) {
        dlgConfigChannel.IsNewChannel(false);

        if (dlgConfigMedia.DoModal() == IDOK) {
            // metadata
            CChannelMetadata dlgMeta;

            if (dlgMeta.DoModal() == IDOK) {     
                wait->ShowWindow(SW_SHOW);
                wait->CenterWindow(this);
                wait->BringWindowToTop();
                wait->UpdateWindow();

                // create new channel
                csFile = pApp->GetTempDirectory() + _T("\\new.xml");
                bDelete = !dlgMeta.CreateNewChannel(csFile);
                if (bDelete == FALSE) {
                    // arrange file
                    valid.Validate(csFile, csFile);
                    // upload to server
                    bDelete = !UploadChannel(csNewChannel, csFile);
                }
            }
        }
    }	

    // test if the key must be deleted
    if (bDelete) {
        DeleteKey(csNewChannel);
        SelectItem(0);
    } else {
        // add new channel
        m_ListControl.InsertItem(iAppend, _T("??"));

        // the new channel
        CString *csChannelName = new CString(csNewChannel);
        DWORD dwChannelIndex = m_iLastChannel + 1;
        m_ListControl.SetItemData(iAppend, dwChannelIndex);

        m_ListControl.SetState(iAppend, 2, 0);
        SelectChannel(csNewChannel);
        SetAsActiveChannel();
        m_iLastChannel++;

        // set channel name to channel title
        m_TabControl.GetTransferSettings(csNewChannel);
        // metadata
        CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
        CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];

        meta->GetMetadata(trsp->GetChannelURL(), FALSE);
        m_ListControl.SetItemText(iAppend, 0, meta->GetTitle());


        CString csFileDownload = pApp->GetTempDirectory() + _T("\\tmp2.xml");

        BOOL bResult = valid.Validate(trsp->GetChannelURL(), csFileDownload);
        wait->DestroyWindow();
        if (bResult) {
            bResult = CompareFiles(csFile, csFileDownload);
            CString csMessage;
            csMessage.LoadString((bResult) ? IDS_SUCCESS : IDS_NOTEQUALS);
            AfxMessageBox(csMessage, MB_ICONINFORMATION); 
        } else {
            CString csMessage;
            csMessage.LoadString(IDS_NOTEQUALS);
            AfxMessageBox(csMessage, MB_ICONINFORMATION); 
        } 
    }
    EnableControls();
}

void CGeneralSettings::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // item
    int iSelected = pNMListView->iItem;
    if (iSelected != -1) {
        CWaitDlg waitDlg;
        waitDlg.Create(IDD_WAIT_DIALOG);
        m_HWaitDlg = waitDlg.GetSafeHwnd();

        CWnd *wait = FromHandle(m_HWaitDlg);
        wait->ShowWindow(SW_SHOW);
        wait->CenterWindow(this);
        wait->BringWindowToTop();
        wait->UpdateWindow();
        // transfer

        if (m_ListControl.GetItemData(iSelected) == -1) {
            CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
            CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];
            trsp->SetTempChannel(true);
            meta->SetTempChannel(true);

            CString csTempChannel = pApp->GetTempDirectory() + _T("\\TempChannel.xml");
            meta->GetMetadata(csTempChannel, FALSE);
            m_ListControl.SetItemText(iSelected, 0, m_csTempChannelName);

            trsp->SetTemporaryChannel(m_tcTempChannel);
            trsp->SetTemporaryMedia(m_tcTempMedia);
            m_bTempSel = true;
        } else {
            CString csKey = GetListItemData(iSelected);
            m_TabControl.GetTransferSettings((LPCTSTR)csKey);

            // metadata
            CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
            CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];
            trsp->SetTempChannel(false);
            meta->SetTempChannel(false);
            meta->GetMetadata(trsp->GetChannelURL(), FALSE);
            m_ListControl.SetItemText(iSelected, 0, meta->GetTitle());
            m_bTempSel = false;
        }
        wait->DestroyWindow();
    }

    // subitem
    if (pNMListView->iSubItem == 2) {
        SetAsActiveChannel();
    }
    EnableControls();
    *pResult = 0;
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CGeneralSettings::SelectItem(int iItem) {
    int iCount = m_ListControl.GetItemCount();
    if (iCount != 0) {
        m_ListControl.SetFocus();
        m_ListControl.SetItemState(iItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
        m_ListControl.RedrawItems(0, iCount - 1);
        Refresh();
    }
}

void CGeneralSettings::OnShowWindow(BOOL bShow, UINT nStatus) {
    CDialog::OnShowWindow(bShow, nStatus);
}

void CGeneralSettings::SetAsActiveChannel() {
    int iSelected = m_ListControl.GetNextItem(-1, LVNI_SELECTED);
    // no selected item
    if (iSelected == -1) {
        return;
    }
    unsigned uCount = m_ListControl.GetItemCount();
    if (uCount > 0) {
        for (unsigned i = 0; i < uCount; i++) {
            UINT uState = m_ListControl.GetItemState(i, LVIS_SELECTED);
            bool bSelected = (uState == LVIS_SELECTED);

            if (m_ListControl.GetItemData(i) != -1) {
                CString pszKey = GetListItemData(i);
                if (m_regEntry.Open(pszKey) == ERROR_SUCCESS) {
                    m_regEntry.WriteProfileInt(_T("Active"), (bSelected) ? 1 :0);   
                    m_regEntry.Close();
                    m_ListControl.SetState(i, 2, bSelected);
                }
            } else if (m_ListControl.GetItemData(iSelected) == -1) {
                m_ListControl.SetState(i, 2, 1);
            } else {
                m_ListControl.SetState(i, 2, 0);
            }
        }
    }
}

//////////////////////////
// NETWORK
/////////////////////
void CGeneralSettings::LoadFromRegistry(CString pszKey) {
    if (m_regEntry.Open(pszKey) == ERROR_SUCCESS) {
        CString csTemp(_T(""));
        //Channel settings
        m_tpChannel.iService = m_regEntry.GetProfileInt(_T("Channel Service"), UPLOAD_FTP);   

        csTemp = m_regEntry.GetProfileString(_T("Channel URL"), _T(""));
        m_tpChannel.csUrl = csTemp.GetString();
        csTemp = m_regEntry.GetProfileString(_T("Channel Server"), _T(""));
        m_tpChannel.csServer = csTemp.GetString();
        m_tpChannel.nPort = m_regEntry.GetProfileInt(_T("Channel Port"), 21);   
        csTemp = m_regEntry.GetProfileString(_T("Channel Username"), _T(""));
        m_tpChannel.csUsername = csTemp.GetString();
        m_tpChannel.bSavePassword = m_regEntry.GetProfileInt(_T("Channel SavePassword"), TRUE);
        if (m_tpChannel.bSavePassword) {
            CCrypto crypt;
            CString csAux = m_regEntry.GetProfileString(_T("Channel Password"), _T(""));
            CString csPassword = csAux.GetString();
            m_tpChannel.csPassword = crypt.Decode(csPassword);
        }
        csTemp = m_regEntry.GetProfileString(_T("Channel Directory"), _T("/"));
        m_tpChannel.csDirectory = csTemp.GetString();

        if (!m_tpChannel.csDirectory.IsEmpty()) {
            if (m_tpChannel.csDirectory.Right(1) != _T("/")) {
                m_tpChannel.csDirectory += _T("/");
            }
        }
        //Media settings
        m_tpMedia.iService = m_regEntry.GetProfileInt(_T("Media Service"), UPLOAD_FTP);   

        csTemp = m_regEntry.GetProfileString(_T("Media URL"), _T(""));
        m_tpMedia.csUrl = csTemp.GetString();
        csTemp = m_regEntry.GetProfileString(_T("Media Server"), _T(""));
        m_tpMedia.csServer = csTemp.GetString();
        m_tpMedia.nPort = m_regEntry.GetProfileInt(_T("Media Port"), 21);   
        csTemp = m_regEntry.GetProfileString(_T("Media Username"), _T(""));
        m_tpMedia.csUsername = csTemp.GetString();
        m_tpMedia.bSavePassword = m_regEntry.GetProfileInt(_T("Media SavePassword"), TRUE);
        if (m_tpMedia.bSavePassword) {
            CCrypto crypt;
            CString csAux = m_regEntry.GetProfileString(_T("Media Password"), _T(""));
            CString csPassword = csAux.GetString();
            m_tpMedia.csPassword = crypt.Decode(csPassword);
        }
        csTemp = m_regEntry.GetProfileString(_T("Media Directory"), _T("/"));
        m_tpMedia.csDirectory = csTemp.GetString();

        if (!m_tpMedia.csDirectory.IsEmpty()) {
            if (m_tpMedia.csDirectory.Right(1) != _T("/")) {
                m_tpMedia.csDirectory += _T("/");
            }
        }
        m_regEntry.Close();
    }
}

BOOL CGeneralSettings::DeleteChannel(CString pszKey) {
    BOOL bResult = FALSE;
    // load settings
    LoadFromRegistry(pszKey);
    // 4. ask for password if necessary
    if (!m_tpChannel.bSavePassword || m_tpChannel.csPassword.IsEmpty()) {
        if (!AskForPassword()) {
            return bResult;
        }
    }
    if (m_tpChannel.iService == UPLOAD_FTP) {
        // FTP
        CFtp ftp;
        bResult = ftp.Open(m_tpChannel.csServer, m_tpChannel.nPort, m_tpChannel.csUsername, m_tpChannel.csPassword);
        if (bResult) {
            CString csXml = GetFileName(m_tpChannel.csUrl);
            bResult = ftp.Delete(csXml, m_tpChannel.csDirectory);
            ftp.Close();
        }
    } else {
        CSsh ssh;
        double dProg;
        bool pbIsCancel;
        bool pbIsThreadActive;
        CString csXml;

        if (m_tpChannel.iService == UPLOAD_SCP) {
            csXml = _T("\'") + m_tpChannel.csDirectory + _T("\'\'") + GetFileName(m_tpChannel.csUrl) + _T("\'");         
        } else {
            csXml = m_tpChannel.csDirectory + GetFileName(m_tpChannel.csUrl);
        }
        bResult = ssh.Delete(csXml, m_tpChannel, &dProg, &pbIsCancel, &pbIsThreadActive);
        if (!bResult) {   
            ssh.GetErrStr();
        }
    }
    return bResult;
}

BOOL CGeneralSettings::AskForPassword() {
    BOOL bResult = FALSE;
#ifdef _UPLOADER_EXE
    CPasswordDlg dlgPsw;
#else
    CPasswordDlg dlgPsw(m_tpChannel.csUsername, m_tpChannel.csServer, this);
#endif

    dlgPsw.SetUsername(m_tpChannel.csUsername);
    if (dlgPsw.DoModal() == IDOK) {
        m_tpChannel.csPassword = dlgPsw.GetPassword();
        m_tpChannel.csUsername = dlgPsw.GetUsername();
        bResult = TRUE;
    }
    return bResult;
}

CString CGeneralSettings::GetFileName(CString csFilePath) {
    CString csFileName;
    csFileName = csFilePath;

    int iSize = csFilePath.GetLength();
    for (int i = iSize - 1;i > 0;i--) {
        if ((csFilePath[i] == _T('\\')) || (csFilePath[i] == _T('/'))) {
            csFileName = csFilePath.Right(iSize - i - 1);
            break;
        }
    }
    return csFileName;
}

BOOL CGeneralSettings::UploadChannel(CString pszKey, CString csFile) {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    BOOL bResult = FALSE;
    // load settings
    LoadFromRegistry(pszKey);

    // 4. ask for password if necessary

    if (!m_tpChannel.bSavePassword || m_tpChannel.csPassword.IsEmpty()) {
        if (!AskForPassword()) {
            return bResult;
        }
    }
    if (m_tpChannel.iService == UPLOAD_FTP) {
        // FTP
        CFtp ftp;
        bResult = ftp.Open(m_tpChannel.csServer, m_tpChannel.nPort, m_tpChannel.csUsername, m_tpChannel.csPassword);
        if (bResult) {
            CString csNewChannel = GetFileName(m_tpChannel.csUrl);
            bResult = ftp.Upload(csFile, csNewChannel, m_tpChannel.csDirectory);
            ftp.Close();
        }
    } else {
        CString csNewChannel = GetFileName(m_tpChannel.csUrl);
        CString csXmlFile;
        if (m_tpChannel.iService == UPLOAD_SCP) {
            csXmlFile = _T("\'") + m_tpChannel.csDirectory + _T("\'\'") + csNewChannel + _T("\'");
        } else {
            csXmlFile = m_tpChannel.csDirectory + csNewChannel;
        }
        CSsh ssh;
        double dProg;
        bool pbIsCancel = false;
        bool pbIsThreadActive = false;

        bResult = ssh.Upload(csFile, csXmlFile, m_tpChannel, &dProg, &pbIsCancel, &pbIsThreadActive);
    }
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
    return bResult;
}

BOOL CGeneralSettings::DestroyWindow() {
    // TODO: Add your specialized code here and/or call the base class
    ::DeleteFile(pApp->GetTempDirectory() + _T("\\tmp2.xml"));
    ::DeleteFile(pApp->GetTempDirectory() + _T("\\new.xml"));
    ::DeleteFile(pApp->GetTempDirectory() + _T("\\mod.xml"));
    ::DeleteFile(pApp->GetTempDirectory() + _T("\\TempChannel.xml"));
    return CDialog::DestroyWindow();
}

void CGeneralSettings::EnableControls() {
    bool bEnable = (m_ListControl.GetItemCount() > 0);
    bool bEnableTemp = true;

    int iSelected = m_ListControl.GetNextItem(-1, LVNI_SELECTED);
    if (iSelected != -1) {
        int data = (int) m_ListControl.GetItemData(iSelected);
        if (data == -1) {
            bEnableTemp = false;
        }
    }
    m_btnDelete.EnableWindow(bEnable && bEnableTemp);
    m_btnRemove.EnableWindow(bEnable && bEnableTemp);

    CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
    CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];
    meta->EnableChangeButton(bEnable);
    trsp->EnableChangeButton(bEnable);
}

BOOL CGeneralSettings::CompareFiles(CString srcFileName, CString dstFileName) {
    BOOL bResult = FALSE;
    BYTE btSrc, btDst;
    CFile src, dst;

    if (src.Open(srcFileName,  CFile::modeRead | CFile::shareDenyWrite, NULL)) {
        if (dst.Open(dstFileName,  CFile::modeRead | CFile::shareDenyWrite, NULL)) {
            DWORD dwSrcLen = (DWORD)src.GetLength();
            DWORD dwDstLen = (DWORD)dst.GetLength();
            if (dwSrcLen == dwDstLen) {
                for (unsigned i=0;i < src.GetLength();i++) {
                    src.Read((void *)&btSrc, 1);
                    dst.Read((void *)&btDst, 1);
                    bResult = (btSrc == btDst);
                    if (!bResult) {
                        break;
                    }               
                }
            }
            dst.Close();
        }
        src.Close();
    }
    return bResult;
}

void CGeneralSettings::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) {
    CDialog::OnActivate(nState, pWndOther, bMinimized);

    // TODO: Add your message handler code here
}

TransferParam CGeneralSettings::GetTemporaryChannel() {
    return m_tcTempChannel;
}

TransferParam CGeneralSettings::GetTemporaryMedia() {
    return m_tcTempMedia;
}

TransferMetadata CGeneralSettings::GetTemporaryMetadata() {
    return m_tmTempMetadata;
}

void CGeneralSettings::SetTemporaryChannel(TransferParam tpTempChannel) {
    m_tcTempChannel = tpTempChannel;
}

void CGeneralSettings::SetTemporaryMedia(TransferParam tpTempMedia) {
    m_tcTempMedia = tpTempMedia;
}

void CGeneralSettings::SetTemporaryMetadata(TransferMetadata tpTempMetadata) {
    m_tmTempMetadata = tpTempMetadata;
}

void CGeneralSettings::ChangeStatus(int nItem, bool bActive) {
    if (m_ListControl.GetItemData(nItem) != -1) {
        if (bActive) {
            CString csText;
            csText.LoadString(IDL_ACTIVE);
            m_ListControl.SetItemText(nItem, 1, csText);
        } else {
            m_ListControl.SetItemText(nItem, 1, _T(""));
        }
    }
}

CString CGeneralSettings::GetListItemData(int nItem) {
    DWORD_PTR dwChannelIndex = m_ListControl.GetItemData(nItem);
    CString csChannelIndex;
    csChannelIndex.Format(_T("%d"), dwChannelIndex);
    CString csKey = _T("Channel ") + csChannelIndex;

    return csKey;
}

TransferParam CGeneralSettings::GetActiveChannelData() {
    CString csChannel = GetActiveChannel();
    LoadFromRegistry(csChannel);
    return m_tpChannel;
}
TransferParam CGeneralSettings::GetActiveMediaData() {
    CString csChannel = GetActiveChannel();
    LoadFromRegistry(csChannel);
    return m_tpMedia;
}

bool CGeneralSettings::IsTempChannelSelected() {
    return m_bTempSel;
}
