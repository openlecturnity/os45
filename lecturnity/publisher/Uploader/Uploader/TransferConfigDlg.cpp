// TranferConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Uploader.h"
#include "TransferConfigDlg.h"
#include "Registry.h"
#include "MfcUtils.h"
#include "FileExists.h"
#include "GeneralSettings.h"
#include "TransferPage.h"
#include "Ssh.h"
#include "Ftp.h"
#include "PasswordDlg.h"
#include "WaitDlg.h"


// CTranferConfigDlg dialog

IMPLEMENT_DYNAMIC(CTransferConfigDlg, CDialog)

CTransferConfigDlg::CTransferConfigDlg(int Type, CWnd* pParent /*=NULL*/)
: CDialog(CTransferConfigDlg::IDD, pParent) {
    m_bTemporaryChannel = false;
    m_isKeyPressed = false;
    m_isFocused = true;
    m_bIsNewChannel = false;
    InitTransferParam();
    m_iType = Type;
    m_bIsSlidestar = false;
}

CTransferConfigDlg::CTransferConfigDlg(int Type, TransferParam tp, CWnd* pParent /*=NULL*/, bool bIsSlidestar /*= false*/)
: CDialog(CTransferConfigDlg::IDD, pParent) {
    m_bTemporaryChannel = false;
    m_isKeyPressed = false;
    m_isFocused = true;
    m_bIsNewChannel = false;
    m_tp = tp;
    m_iType = Type;
    m_bIsSlidestar = bIsSlidestar;
}

CTransferConfigDlg::CTransferConfigDlg(int Type, bool bFromReg, CWnd* pParent /*=NULL*/)
: CDialog(CTransferConfigDlg::IDD, pParent) {
    m_bTemporaryChannel = false;
    m_isKeyPressed = false;
    m_isFocused = true;
    m_bIsNewChannel = false;
    CRegistry reg;
    if (bFromReg) {
        reg.LoadTransferParam(m_tp);
    } else {
        InitTransferParam();
    }
    m_iType = Type;
    m_bIsSlidestar = false;
}

CTransferConfigDlg::~CTransferConfigDlg() {
}

void CTransferConfigDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_GROUP_TRANSFER, m_CbBackground);
    DDX_Control(pDX, IDE_HOST, m_edtHost);
    DDX_Control(pDX, IDE_DIRECTORY, m_edtDirectory);
    DDX_Control(pDX, IDE_USERNAME, m_edtUsername);
    DDX_Control(pDX, IDE_PASSWORD, m_edtPassword);
    DDX_Control(pDX, IDL_SAVEPSWD, m_CSavePassword);
    DDX_Control(pDX, IDE_PORT, m_edtPort);
    DDX_Control(pDX, IDC_SERVICE, m_CbService);
    DDX_Control(pDX, IDE_URL, m_edtUrl);
    DDX_Control(pDX, IDL_URL, m_lblUrl);
    DDX_Control(pDX, IDL_DIRECTORY, m_lblDirectory);
    DDX_Control(pDX, IDG_SERVER, m_GbServer);
    DDX_Control(pDX, IDL_HOST, m_lblHost);
    DDX_Control(pDX, IDL_USERNAME, m_lblUserName);
    DDX_Control(pDX, IDL_PASSWORD, m_lblPassword);
    DDX_Control(pDX, IDL_SERVICE, m_lblService);
    DDX_Control(pDX, IDL_PORT, m_lblPort);
    DDX_Control(pDX, IDC_BARBOTTOM, m_IBar);
    DDX_Control(pDX, IDOK, m_btnOk);
    DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CTransferConfigDlg, CDialog)
    ON_CBN_SELCHANGE(IDC_SERVICE, &CTransferConfigDlg::OnCbnSelchangeService)
    ON_EN_KILLFOCUS(IDE_HOST, &CTransferConfigDlg::OnEnKillfocusHost)
    ON_EN_KILLFOCUS(IDE_DIRECTORY, &CTransferConfigDlg::OnEnKillfocusDirectory)
    ON_BN_CLICKED(IDOK, &CTransferConfigDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDL_SAVEPSWD, &CTransferConfigDlg::OnSavePassword)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CTranferConfigDlg message handlers

void CTransferConfigDlg::CheckInputTextForBlanks(CString &csInputText) {
    CString csTrimLeft = csInputText;
    csTrimLeft.TrimLeft();
    CString csTrimLeftRight = csTrimLeft;
    csTrimLeftRight.TrimRight();

    if (csInputText.Compare(csTrimLeftRight) != 0) {
        CString csMessage;
        csMessage.LoadString(IDS_WARNING_WHITESPACE);
        int nFeedback = AfxMessageBox(csMessage, MB_OK, MB_ICONINFORMATION); 
        csInputText.Empty();
        csInputText.Format(csTrimLeftRight);
    }
}

void CTransferConfigDlg::OnCbnSelchangeService() {
    // TODO: Add your control notification handler code here
    UpdateData();

    BOOL bActivate = (m_CbService.GetCurSel() != 0);

    if (bActivate) {
        m_edtPort.SetWindowText(_T("22"));
    } else {
        m_edtPort.SetWindowText(_T("21"));
    }
    UpdateData(FALSE);
}

void CTransferConfigDlg::OnEnKillfocusHost() {
    // TODO: Add your control notification handler code here
    m_edtHost.GetWindowText(m_tp.csServer);
    CheckInputTextForBlanks(m_tp.csServer);
    m_edtHost.SetWindowText(m_tp.csServer);
}

void CTransferConfigDlg::OnEnKillfocusDirectory() {
    // TODO: Add your control notification handler code here
    m_edtDirectory.GetWindowText(m_tp.csDirectory);
    CheckInputTextForBlanks(m_tp.csDirectory);
    m_edtDirectory.SetWindowText(m_tp.csDirectory);
}

void CTransferConfigDlg::OnBnClickedOk() {
    // TODO: Add your control notification handler code here
    CRegistry reg;
    BOOL bResult = FALSE;
    SaveTransferParam();

#ifdef _UPLOADER_EXE
    if (m_iType == T_CHANNEL && !m_bIsNewChannel) {
        bResult = CheckTransferParam(); 
    } else {
        bResult = TRUE;
    }
#else
    bResult = TRUE;
#endif

    if (bResult) {
        if (m_iType == T_TRANSFER) { 
            if (!m_bTemporaryChannel) {
                reg.SaveTransferParam(m_tp, m_csChannelName);
            }
        } else if (m_iType == T_CHANNEL) {
            if (!m_bTemporaryChannel) {
                    reg.SaveChannelTransferParam(m_tp, m_csChannelName);
            }
        } else if (m_iType == T_MEDIA) {
            if (!m_bTemporaryChannel) {
                        reg.SaveMediaTransferParam(m_tp, m_csChannelName);
            }
        }

        CString csText;

        m_edtUrl.GetWindowText(csText);
        csText.ReleaseBuffer();
        m_acUrl->AddItem(csText);

        m_edtHost.GetWindowText(csText);
        csText.ReleaseBuffer();
        m_acHost->AddItem(csText);

        m_edtUsername.GetWindowText(csText);
        csText.ReleaseBuffer();
        m_acUser->AddItem(csText);

        m_edtDirectory.GetWindowText(csText);
        csText.ReleaseBuffer();
        m_acDir->AddItem(csText);

        m_edtPort.GetWindowText(csText);
        csText.ReleaseBuffer();
        m_acPort->AddItem(csText);


        if (m_iType == T_CHANNEL && m_bIsNewChannel) {
            CSsh ssh;
            bool bExists;
            bool pbIsDir = false;
            bool pbIsEmpty = false;
            CString csFileName;
            csFileName = m_tp.csDirectory + GetFileName(m_tp.csUrl);
            if (m_tp.iService != UPLOAD_FTP) {    
                ssh.CheckTarget(m_tp.csServer, m_tp.iService, m_tp.nPort, m_tp.csUsername, m_tp.csPassword, csFileName, bExists, &pbIsDir, &pbIsEmpty);

                if (bExists) {
                    AfxMessageBox(IDS_CHANNEL_EXISTS, MB_OK | MB_TOPMOST);
                } else {
                    OnOK();
                }

            } else {
                CFtp ftp;
                bResult = ftp.Open(m_tp.csServer, m_tp.nPort, m_tp.csUsername, m_tp.csPassword);
                if (bResult) {
                    ftp.CheckTarget(csFileName, bExists, &pbIsDir, &pbIsEmpty);
                    if (bExists) {
                        AfxMessageBox(IDS_CHANNEL_EXISTS, MB_OK | MB_TOPMOST);
                    } else {
                        OnOK();
                    }
                }
            }
        } else {
            OnOK();
        }
    }
}

BOOL CTransferConfigDlg::CheckTransferParam() {
    BOOL bResult = FALSE;
    int iResult = 1;
    CUploaderApp *pApp;
    CFtp ftp;
    pApp = (CUploaderApp *)AfxGetApp();

    if (m_tp.csPassword.IsEmpty() || !m_tp.bSavePassword) {
        CPasswordDlg dlgPsw;
        dlgPsw.SetUsername(m_tp.csUsername);
        if (dlgPsw.DoModal() == IDOK) {
            m_tp.csPassword = dlgPsw.GetPassword();
            m_tp.csUsername = dlgPsw.GetUsername();
        }
    }

    CWaitDlg waitDlg;
    waitDlg.Create(IDD_WAIT_DIALOG);
    HWND hWaitDlg = waitDlg.GetSafeHwnd();
    CWnd *wait = FromHandle(hWaitDlg);
    wait->ShowWindow(SW_SHOW);
    wait->CenterWindow(this);
    wait->BringWindowToTop();
    wait->UpdateWindow();

    if (m_iType == T_CHANNEL && m_tp.iService == UPLOAD_FTP) {
        CFileExists feXml;
        feXml.SetFilename(GetFileName(m_tp.csUrl));
        CString csUrl = m_tp.csUrl;
        int iFileSize = GetFileName(m_tp.csUrl).GetLength();
        int iUrlSize = csUrl.GetLength() - iFileSize;
        csUrl.Delete(iUrlSize, iFileSize);
        feXml.SetUrl(csUrl);
        iResult = feXml.RemoteFileExists();
    }

    if (iResult == 0) {
        wait->DestroyWindow();
        CString csErrorMessage;
        csErrorMessage.LoadString(IDS_ERRPARSE);
        AfxMessageBox(csErrorMessage, MB_ICONSTOP);
    }

    CString csFile = pApp->GetTempDirectory() + _T("\\mod.xml"); 

    if (m_tp.iService == UPLOAD_FTP) { 
        if (iResult) {
            bResult = ftp.Open(m_tp.csServer, m_tp.nPort, m_tp.csUsername, m_tp.csPassword);
            if (bResult) {
                CString csNewChannel = GetFileName(m_tp.csUrl);
                bResult = ftp.Download(csNewChannel, csFile, m_tp.csDirectory);
                ftp.Close();
            } 
        }
    } else {
        if (iResult) {
            CString csNewChannel = GetFileName(m_tp.csUrl);
            CString csXmlFile;
            if (m_tp.iService == UPLOAD_SCP) {
                csXmlFile = _T("\'") + m_tp.csDirectory + _T("\'\'") + csNewChannel + _T("\'");            
            } else {
                csXmlFile = m_tp.csDirectory + csNewChannel;
            }

            CSsh ssh;
            double dProg;
            bool bIsCancel;
            bool bIsThreadActive;

            bResult = ssh.Download(csXmlFile, csFile, m_tp, &dProg, &bIsCancel, &bIsThreadActive);
            wait->DestroyWindow();

            if (!bResult) {
                wait->DestroyWindow();
                CString csErrStr = ssh.GetErrStr();
            }
        }
    }

    return bResult;
}

void CTransferConfigDlg::IsNewChannel(bool bIsNewChannel) {
    m_bIsNewChannel = bIsNewChannel;
}


TransferParam CTransferConfigDlg::GetTransferParam() {
    return m_tp;
}

void CTransferConfigDlg::SetTransferParam(TransferParam tpTransfer) {
    m_tp = tpTransfer;
}

HBRUSH CTransferConfigDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
    HBRUSH hbr = NULL;

    if (nCtlColor == CTLCOLOR_STATIC ) {
        pDC->SetBkColor(RGB(255,255, 255));
        hbr = CreateSolidBrush(RGB(255, 255, 255));
    } else {
        hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);  
    }

    return hbr;
}

void CTransferConfigDlg::SaveTransferParam() {
    int iCheckStatus;
    CString csTemp;
    iCheckStatus = m_CSavePassword.GetCheck();
    if (iCheckStatus == BST_CHECKED) {
        m_tp.bSavePassword = TRUE;
    } else {
        m_tp.bSavePassword = FALSE;
    }
    m_edtDirectory.GetWindowText(m_tp.csDirectory);
    CheckDirectoryFormat(m_tp.csDirectory);
    m_edtDirectory.SetWindowText(m_tp.csDirectory);
    m_edtPassword.GetWindowText(m_tp.csPassword);
    m_edtHost.GetWindowText(m_tp.csServer);
    m_edtUsername.GetWindowText(m_tp.csUsername);
    switch (m_CbService.GetCurSel()) {
    case 0:  m_tp.iService = UPLOAD_FTP;
        break;
    case 1:  m_tp.iService = UPLOAD_SCP;
        break;
    case 2:  m_tp.iService = UPLOAD_SFTP;
        break;
    }
    m_edtPort.GetWindowText(csTemp);
    m_tp.nPort = _ttoi(csTemp);
    m_edtUrl.GetWindowText(m_tp.csUrl);
    if (m_iType == T_MEDIA) {
        CheckUrlDirectoryFormat(m_tp.csUrl);
    }
}

void CTransferConfigDlg::LoadTransferParam() {
    CString csTemp;
    if (m_tp.bSavePassword == TRUE) {
        m_CSavePassword.SetCheck(BST_CHECKED);
    } else {
        m_CSavePassword.SetCheck(BST_UNCHECKED);
        m_edtPassword.EnableWindow(false);
    }
    CheckDirectoryFormat(m_tp.csDirectory);
    m_edtDirectory.SetWindowText(m_tp.csDirectory);
    if (m_tp.bSavePassword == TRUE) {
        m_edtPassword.SetWindowText(m_tp.csPassword);
    }
    m_edtHost.SetWindowText(m_tp.csServer);
    m_edtUsername.SetWindowText(m_tp.csUsername);
    m_CbService.SetCurSel(m_tp.iService);
    csTemp.Format(_T("%d"),m_tp.nPort);
    m_edtPort.SetWindowText(csTemp);
    m_edtUrl.SetWindowText(m_tp.csUrl);
    if (m_iType == T_MEDIA) {
        CheckUrlDirectoryFormat(m_tp.csUrl);
    }
}

BOOL CTransferConfigDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    m_acUrl = new CCustomAutoComplete(HKEY_CURRENT_USER, CString(RECENT_KEY) + CString(_T("Url")));	
    m_acUrl->Bind(m_edtUrl.m_hWnd, ACO_UPDOWNKEYDROPSLIST | ACO_AUTOSUGGEST | ACO_AUTOAPPEND, _T(""));

    m_acHost = new CCustomAutoComplete(HKEY_CURRENT_USER, CString(RECENT_KEY) + CString(_T("Host")));	
    m_acHost->Bind(m_edtHost.m_hWnd, ACO_UPDOWNKEYDROPSLIST | ACO_AUTOSUGGEST | ACO_AUTOAPPEND, _T(""));

    m_acUser = new CCustomAutoComplete(HKEY_CURRENT_USER, CString(RECENT_KEY) + CString(_T("User")));	
    m_acUser->Bind(m_edtUsername.m_hWnd, ACO_UPDOWNKEYDROPSLIST | ACO_AUTOSUGGEST | ACO_AUTOAPPEND, _T(""));

    m_acDir = new CCustomAutoComplete(HKEY_CURRENT_USER, CString(RECENT_KEY) + CString(_T("Directory")));	
    m_acDir->Bind(m_edtDirectory.m_hWnd, ACO_UPDOWNKEYDROPSLIST | ACO_AUTOSUGGEST | ACO_AUTOAPPEND, _T(""));

    m_acPort = new CCustomAutoComplete(HKEY_CURRENT_USER, CString(RECENT_KEY) + CString(_T("Port")));	
    m_acPort->Bind(m_edtPort.m_hWnd, ACO_UPDOWNKEYDROPSLIST | ACO_AUTOSUGGEST | ACO_AUTOAPPEND, _T(""));


    LoadTransferParam();
    DWORD dwID[] = {
        IDL_SERVICE,
        IDL_HOST,
        IDL_PORT,
        IDL_USERNAME,
        IDL_PASSWORD,
        IDL_SAVEPSWD,
        IDL_DIRECTORY,
        IDG_SERVER,
        IDOK,
        IDCANCEL,
        -1 
    };
    MfcUtils::Localize(this, dwID);

    CString csText;
    csText.LoadString(IDD_TRANSFER_CONFIG_DIALOG);
    SetWindowText(csText);

    SetIcon(AfxGetApp()->LoadIcon(IDI_UPLOADER), TRUE);

    if (m_iType == T_TRANSFER) {
        m_edtUrl.ShowWindow(SW_HIDE);
        m_lblUrl.ShowWindow(SW_HIDE);
        LPRECT lpRect = new RECT();
        LPRECT lpDialogRect = new RECT();
        this->GetWindowRect(lpDialogRect);
        lpDialogRect->bottom -= 50;
        this->SetWindowPos(NULL,0,0,lpDialogRect->right-lpDialogRect->left,lpDialogRect->bottom-lpDialogRect->top,SWP_NOZORDER|SWP_NOMOVE);
        m_CbBackground.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 35;
        lpRect->left -= lpDialogRect->left;
        lpRect->bottom -= lpDialogRect->top + 70;
        lpRect->right -= lpDialogRect->left;
        m_CbBackground.SetWindowPos(NULL,lpRect->left,lpRect->top,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top,SWP_NOZORDER);
        m_GbServer.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_GbServer.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_lblHost.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_lblHost.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_edtHost.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_edtHost.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_lblService.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_lblService.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_CbService.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_CbService.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_lblUserName.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_lblUserName.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_edtUsername.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_edtUsername.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_lblPassword.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_lblPassword.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_edtPassword.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_edtPassword.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_CSavePassword.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_CSavePassword.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_lblDirectory.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_lblDirectory.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_edtDirectory.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_edtDirectory.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_lblPort.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_lblPort.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_edtPort.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_edtPort.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_IBar.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_IBar.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_btnOk.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_btnOk.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        m_btnCancel.GetWindowRect(lpRect);
        lpRect->top -= lpDialogRect->top + 70;
        lpRect->left -= lpDialogRect->left;
        m_btnCancel.SetWindowPos(NULL,lpRect->left,lpRect->top,0,0,SWP_NOZORDER|SWP_NOSIZE);
        delete lpRect;
        delete lpDialogRect;
        if (m_bIsSlidestar) {
            m_edtDirectory.EnableWindow(false);
        }
    } else {
        CString csTemp;
        if (m_iType == T_MEDIA) {
            csTemp.LoadString(IDL_MP4URL);
        } else {
            csTemp.LoadString(IDL_CHANNEL);
        }
        m_lblUrl.SetWindowText(csTemp);
        m_edtUrl.ShowWindow(SW_SHOW);
        m_lblUrl.ShowWindow(SW_SHOW);
    }

    CenterWindow();
    ::SendMessage(::GetDlgItem(m_hWnd, IDE_PASSWORD), EM_SETPASSWORDCHAR, _T('*'), 0);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CTransferConfigDlg::CheckDirectoryFormat(CString &csDir) {
    if (csDir.GetLength() > 0) {
        if (csDir.GetAt(0) != _T('/')) {
            csDir = _T("/") + csDir;
        }
        if (csDir.GetAt(csDir.GetLength() - 1) != _T('/')) {
            csDir = csDir + _T("/");
        }
    } else {
        csDir = _T("/");
    }
}

void CTransferConfigDlg::CheckUrlDirectoryFormat(CString &csDir) {
    if(csDir.GetLength() > 0) {
        if (csDir.GetAt(csDir.GetLength() - 1) != _T('/')) {
            csDir = csDir + _T("/");
        }
    }
}

void CTransferConfigDlg::InitTransferParam() {
    m_tp.bSavePassword = TRUE;
    m_tp.csDirectory = _T("");
    m_tp.csPassword = _T("");
    m_tp.csServer = _T("");
    m_tp.csUrl = _T("");
    m_tp.csUsername = _T("");
    m_tp.iService = UPLOAD_FTP;
    m_tp.nPort = DEFAULT_FTP_PORT;
}

void CTransferConfigDlg::SetChannelName(CString csChannelName) {
    m_csChannelName = csChannelName;
}

CString CTransferConfigDlg::GetChannelURL() {
    return m_tp.csUrl;
}

void CTransferConfigDlg::SetTempChannel(bool bTempChannel) {
    m_bTemporaryChannel = bTempChannel;
}

BOOL CTransferConfigDlg::PreTranslateMessage(MSG* pMsg) {

    if (!::GetFocus()) {
        m_isFocused = false;
    }

    if (::GetFocus() && !m_isFocused) {
        RedrawWindow();
        m_isFocused = true;
    }

    if (m_isKeyPressed) {
        this->RedrawWindow();
    }

    if((::GetFocus() ==  m_edtUrl.m_hWnd || ::GetFocus() ==  m_edtHost.m_hWnd || ::GetFocus() ==  m_edtUsername.m_hWnd 
        || ::GetFocus() ==  m_edtDirectory.m_hWnd || ::GetFocus() ==  m_edtPort.m_hWnd) 
        && (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)) {
        CWnd *wndEdit = NULL;

        if (::GetFocus() ==  m_edtUrl.m_hWnd) {
            wndEdit = CWnd::FromHandle(m_edtUrl.GetSafeHwnd());
        } else {
            if (::GetFocus() ==  m_edtHost.m_hWnd) {
                wndEdit = CWnd::FromHandle(m_edtHost.GetSafeHwnd());
            } else {
                if (::GetFocus() ==  m_edtUsername.m_hWnd) {
                    wndEdit = CWnd::FromHandle(m_edtUsername.GetSafeHwnd());
                } else {
                    if (::GetFocus() ==  m_edtDirectory.m_hWnd) {
                        wndEdit = CWnd::FromHandle(m_edtDirectory.GetSafeHwnd());
                    } else {
                        if (::GetFocus() ==  m_edtPort.m_hWnd) {
                            wndEdit = CWnd::FromHandle(m_edtPort.GetSafeHwnd());
                        }
                    }
                }
            }
        }
        CEdit *edtCtrl((CEdit*)wndEdit);

        edtCtrl->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
        return TRUE;

    } else {
        if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB) {
            m_isKeyPressed = true;
            SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam); 
        } else {
            if (pMsg->message == WM_SYSKEYDOWN) {
                m_isKeyPressed = true;
                SendMessage(WM_SYSKEYDOWN, pMsg->wParam, pMsg->lParam); 
            } else {
                m_isKeyPressed = false;
            }
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CTransferConfigDlg::DestroyWindow() {
    // TODO: Add your specialized code here and/or call the base class

    m_acUrl->Unbind();
    m_acHost->Unbind();
    m_acUser->Unbind();
    m_acDir->Unbind();
    m_acPort->Unbind();


    return CDialog::DestroyWindow();
}

void CTransferConfigDlg::OnSavePassword() {
    bool bEnable = (m_CSavePassword.GetCheck() == BST_CHECKED) ? true : false;
    m_edtPassword.EnableWindow(bEnable);
}

CString CTransferConfigDlg::GetFileName(CString csFilePath) {
    CString csFileName;
    csFileName = csFilePath;

    int iSize = csFilePath.GetLength();
    for(int i = iSize - 1; i > 0; i--) {
        if ((csFilePath[i] == _T('\\')) || (csFilePath[i] == _T('/'))) {
            csFileName = csFilePath.Right(iSize - i - 1);
            break;
        }
    }

    return csFileName;
}
