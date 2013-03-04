// MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Uploader.h"
#include "MainDlg.h"
#include "GeneralSettings.h"
#include "YtMainDialog.h"
#include "TransferProgressDlg.h"
#include "TransferingContent.h"
#include "ParseLrd.h"
#include "MfcUtils.h"
#include "WaitDlg.h"
#include "PasswordDlg.h"


// CMainDlg dialog

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/) : CDialog(CMainDlg::IDD, pParent) {
    m_csDocFile.Empty();
    m_csLrdFile.Empty();
    m_bIsContextMenu = false;
    m_csAuthor = _T("");
    m_csTitle = _T("");
}

CMainDlg::~CMainDlg() {
}

void CMainDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_GROUP_TRANSFER, m_GbBackground);
    DDX_Control(pDX, IDE_PODCAST_DOC, m_CbDocFiles);
    DDX_Control(pDX, IDB_BROWSE_LRD, m_btnBrowseLrd);
    DDX_Control(pDX, IDE_LRD_FILE, m_edtLrdFile);
    DDX_Control(pDX, IDE_TITLE, m_edtTitle);
    DDX_Control(pDX, IDE_AUTHOR, m_edtAuthor);
    DDX_Control(pDX, IDR_LRD_METADATA, m_RLrdMetadata);
    DDX_Control(pDX, IDB_PODCAST, m_btnPodcast);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDB_SETTINGS, &CMainDlg::OnBnClickedSettings)
    ON_BN_CLICKED(IDB_PODCAST, &CMainDlg::OnBnClickedPodcast)
    ON_CBN_EDITCHANGE(IDE_PODCAST_DOC, &CMainDlg::OnCbnEditchangePodcastDoc)
    ON_CBN_SELCHANGE(IDE_PODCAST_DOC, &CMainDlg::OnCbnSelchangePodcastDoc)
    ON_BN_CLICKED(IDB_BROWSE_DOC, &CMainDlg::OnBnClickedBrowseDoc)
    ON_BN_CLICKED(IDR_LRD_METADATA, &CMainDlg::OnBnClickedLrdMetadata)
    ON_BN_CLICKED(IDB_BROWSE_LRD, &CMainDlg::OnBnClickedBrowseLrd)
    ON_BN_CLICKED(IDR_EXPLICIT_METADATA, &CMainDlg::OnBnClickedExplicitMetadata)
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CMainDlg message handlers

BOOL CMainDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    // from command line ?
    if (!m_csDocFile.IsEmpty()) {
        if (!CheckUrlFileName(m_csDocFile)) {
            CString csError;
            csError.LoadString(IDS_ERR_MP4URL);
            AfxMessageBox(csError, MB_ICONSTOP);
            m_csDocFile.Empty();
        }
    }


    DWORD dwID[] = {
        IDL_PODCAST_DOC,
        IDL_LRD_METADATA,
        IDL_EXPLICIT_METADATA,
        IDL_TITLE,
        IDL_AUTHOR,
        IDB_PODCAST,
        IDB_SETTINGS,
        -1 
    };
    MfcUtils::Localize(this, dwID);

    SetIcon(AfxGetApp()->LoadIcon(IDI_UPLOADER), TRUE);

    m_CbDocFiles.SetWindowText(m_csDocFile);
    m_edtLrdFile.SetWindowText(m_csLrdFile);
    m_RLrdMetadata.SetCheck(BST_CHECKED);
    m_btnPodcast.EnableWindow(FALSE);

    CString csText;
    csText.LoadString(IDB_BROWSE);
    SetDlgItemText(IDB_BROWSE_DOC, csText);
    m_btnBrowseLrd.SetWindowText(csText);

    csText.LoadString(IDS_PODCASTER);
    SetWindowText(csText);

    ::SendMessage(::GetDlgItem(m_hWnd, IDE_PASSWORD), EM_SETPASSWORDCHAR, _T('*'), 0);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CMainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
    HBRUSH hbr = NULL;

    if (nCtlColor == CTLCOLOR_STATIC && pWnd->GetDlgCtrlID() != IDL_PODCAST_DOC) {
        pDC->SetBkColor(RGB(255,255, 255));
        hbr = CreateSolidBrush(RGB(255, 255, 255));
    } else {
        hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);  
    }

    return hbr;
}

void CMainDlg::ShowPodcastSettings() {
    CGeneralSettings sett;
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    sett.DoModal();
}

void CMainDlg::Podcast(CString csMp4File, bool bIsContextMenu /*= false*/) {
    m_bIsContextMenu = bIsContextMenu;
    if(m_bIsContextMenu == true) {
        if (!CheckUrlFileName(csMp4File)) {
            CString csError;
            csError.LoadString(IDS_ERR_MP4URL);
            AfxMessageBox(csError, MB_ICONSTOP);
            return;
        }
        SearchForLrd(csMp4File);
        //m_edtTitle.GetWindowText(m_csTitle);
        //m_edtTitle.GetWindowText(m_csAuthor);
    } else {
        if (!CheckUrlFileName(csMp4File)) {
            CString csError;
            csError.LoadString(IDS_ERR_MP4URL);
            AfxMessageBox(csError, MB_ICONSTOP);
            return;
        }
        DWORD dwFileAtributes = GetFileAttributes(m_csDocFile);
        if (!(!m_csDocFile.IsEmpty() && dwFileAtributes != INVALID_FILE_ATTRIBUTES && dwFileAtributes != FILE_ATTRIBUTE_DIRECTORY)) {
            CString csError;
            csError.Format(IDS_ERROPENFILE,csMp4File);
            AfxMessageBox(csError, MB_ICONSTOP);
            return;
        }
    }
    TransferParam ChannelTp;
    TransferParam MediaTp;
    CGeneralSettings sett;
    ChannelTp = sett.GetActiveChannelData();
    MediaTp = sett.GetActiveMediaData();
    bool bMedia = true;
    CPasswordDlg dlgPsw;

    if (!ChannelTp.bSavePassword || ChannelTp.csPassword.IsEmpty()) {  
        dlgPsw.SetUsername(ChannelTp.csUsername);
        if (dlgPsw.DoModal() == IDOK) {
            ChannelTp.csPassword = dlgPsw.GetPassword();
            ChannelTp.csUsername = dlgPsw.GetUsername();
        }
        if ((!MediaTp.bSavePassword || MediaTp.csPassword.IsEmpty()) && MediaTp.csUsername == ChannelTp.csUsername && MediaTp.csServer == ChannelTp.csServer) {
            MediaTp.csPassword = ChannelTp.csPassword;
            bMedia = false;
        }
    }
    if ((!MediaTp.bSavePassword || MediaTp.csPassword.IsEmpty()) && bMedia)
    {
        dlgPsw.SetUsername(MediaTp.csUsername);
        if (dlgPsw.DoModal() == IDOK) {
            MediaTp.csPassword = dlgPsw.GetPassword();
            MediaTp.csUsername = dlgPsw.GetUsername();
        }
    }
    CArray<CString,CString> aFiles;
    aFiles.Add(csMp4File);
    int piCurrFile = 0;
    double dProgress;
    bool *pbIsCancel = new bool();
    *pbIsCancel = false;
    bool bIsThreadActive = false;
    CTransferingContent& rTContent = CTransferingContent::GetInstance();
    rTContent.MyStartPodcast(MediaTp, ChannelTp, aFiles, m_csTitle,m_csAuthor, &dProgress, &piCurrFile, pbIsCancel, &bIsThreadActive); //last param to be changed
    //for test upload
    //TransferParam utp;
    //utp.bSavePassword=false;
    //utp.csDirectory=_T("/Temp/");
    //utp.csPassword=utp.csUrl=utp.csUsername=_T("");
    //utp.csServer=_T("localhost");
    //utp.iService=0;
    //utp.nPort=21;
    //rTContent.MyStartUpload(utp, aFiles, &dProgress, &piCurrFile, pbIsCancel, &bIsThreadActive); //last param to be changed
    //show progress dialog
    CString csFrom, csTo;
    csFrom.Format(IDL_FROM, aFiles.GetAt(0));
    csTo.Format(IDL_TO, ChannelTp.csUrl, ChannelTp.csServer);
    CTransferProgressDlg tp(&dProgress, csFrom, csTo);
    if (tp.DoModal() == IDCANCEL) {
        *pbIsCancel = true;
        while (bIsThreadActive) {
            Sleep(400);
        }
        rTContent.Cleanup();
    } else {
        CString csMess;
        csMess.LoadString(IDS_SUCCESS);
        AfxMessageBox(csMess);
    }

    //for test errors
    //int ec=rTContent.GetErrCode();
    //CString es=rTContent.GetErrStr();

    aFiles.RemoveAll();
}

void CMainDlg::OnBnClickedSettings() {
    // TODO: Add your control notification handler code here
    ShowPodcastSettings();
}

void CMainDlg::OnBnClickedPodcast() {
    // TODO: Add your control notification handler code here
    //start transfer
    m_edtAuthor.GetWindowText(m_csAuthor);
    m_edtTitle.GetWindowText(m_csTitle);
    m_CbDocFiles.GetWindowText(m_csDocFile);
    Podcast(m_csDocFile);
}

void CMainDlg::OnCbnEditchangePodcastDoc() {
    // TODO: Add your control notification handler code here
    UpdateData();
    CString csFileName;
    m_CbDocFiles.GetWindowText(csFileName);
    m_btnPodcast.EnableWindow(!csFileName.IsEmpty());

    // only allow valid file name characters
    // CString csDocFile(_T(""));
    // int nPos = HIWORD(m_CbDocFiles.GetEditSel());
    // m_CbDocFiles.GetWindowText(csDocFile);
    // if (CheckUrlFileName(csDocFile))
    // {
    //  m_csDocFile = csDocFile;
    // }

    // activate the Podcast button if the file exists
    // DWORD dwFileAtributes = GetFileAttributes(m_csDocFile);
    // BOOL bValid = !m_csDocFile.IsEmpty() && dwFileAtributes != INVALID_FILE_ATTRIBUTES && dwFileAtributes != FILE_ATTRIBUTE_DIRECTORY;
    // 
    // if (bValid)
    // {
    //SearchForLrd(m_csDocFile);
    //m_edtLrdFile.SetWindowText(m_csLrdFile); 
    // }
    // m_CbDocFiles.SetWindowText(m_csDocFile);
    // m_CbDocFiles.SetEditSel(nPos,nPos);

    // //m_CbDocFiles.GetWindowText(m_csDocFile);
    // //BOOL bValid = !m_csDocFile.IsEmpty();
    // //BOOL bValid = !m_csDocFile.IsEmpty() && GetFileAttributes(m_csDocFile) != -1;
    // m_btnPodcast.EnableWindow(bValid);
}

void CMainDlg::OnCbnSelchangePodcastDoc() {
    // TODO: Add your control notification handler code here
    BOOL bValid = m_CbDocFiles.GetCount();
    m_CbDocFiles.GetWindowText(m_csDocFile);
    m_btnPodcast.EnableWindow(bValid);
}

void CMainDlg::OnBnClickedBrowseDoc() {
    // TODO: Add your control notification handler code here
    CString csFilter = _T("Podcast Files (*.mp4)|*.mp4|All Files (*.*)|*.*||");
    CFileDialog dlgOpen(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, csFilter, this);

    if (dlgOpen.DoModal() == IDOK) {
        UpdateData();
        // Does the path / file name contain special characters?
        if (!CheckUrlFileName(dlgOpen.GetPathName())) {
            CString csError;
            csError.LoadString(IDS_ERR_MP4URL);
            AfxMessageBox(csError, MB_ICONSTOP);
        } else {
            // get the item
            m_csDocFile = dlgOpen.GetPathName();
            m_CbDocFiles.SetWindowText(m_csDocFile);
            SearchForLrd(m_csDocFile);
            m_edtLrdFile.SetWindowText(m_csLrdFile);

            UpdateData(FALSE);
            BOOL bValid = !m_csDocFile.IsEmpty();
            m_btnPodcast.EnableWindow(bValid);
        }
    }
}

void CMainDlg::OnBnClickedLrdMetadata() {
    // TODO: Add your control notification handler code here
    UpdateData();

    m_btnBrowseLrd.EnableWindow(TRUE);
    m_edtLrdFile.EnableWindow(TRUE);

    m_edtTitle.EnableWindow(FALSE);
    m_edtAuthor.EnableWindow(FALSE);

    ParseLrd(m_bIsContextMenu);

    UpdateData(FALSE);
}

void CMainDlg::OnBnClickedBrowseLrd() {
    // TODO: Add your control notification handler code here
    CString csFilter = _T("LRD Files (*.lrd)|*.lrd||");
    CFileDialog dlgOpen(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, csFilter, this);
    // allways start in current directory
    if (dlgOpen.DoModal () == IDOK) {
        UpdateData();
        m_csLrdFile = dlgOpen.GetPathName();
        m_edtLrdFile.SetWindowText(dlgOpen.GetPathName());
        ParseLrd(false);
        UpdateData(FALSE);
    } 
}

void CMainDlg::OnBnClickedExplicitMetadata() {
    // TODO: Add your control notification handler code here
    UpdateData();

    m_btnBrowseLrd.EnableWindow(FALSE);
    m_edtLrdFile.EnableWindow(FALSE);

    m_edtTitle.EnableWindow(TRUE);
    m_edtAuthor.EnableWindow(TRUE);

    UpdateData(FALSE);
}

BOOL CMainDlg::CheckUrlFileName(CString csUrl, int iServiceType) {
    BOOL bAllOk = TRUE;

    _TCHAR tsz;

    for (int i = 0; i < csUrl.GetLength(); ++i) {
        tsz = csUrl.GetAt(i);
        bAllOk = bAllOk && ((tsz >= _T('a') && tsz <= _T('z')) || (tsz >= _T('A') && tsz <= _T('Z')) ||
            (tsz >= _T('0') && tsz <= _T('9')) || (iServiceType == UPLOAD_SCP && tsz == _T(' '))||
            (tsz == _T('-')) || (tsz == _T('.')) || (tsz == _T('_')) || (tsz == _T('~')) ||
            (tsz == _T(':')) || (tsz == _T('@')) || 
            (tsz == _T('!')) || (tsz == _T('$')) || (tsz == _T('\'')) || 
            (tsz == _T('*')) || (tsz == _T('+')) || 
            (tsz == _T(',')) || (tsz == _T(';')) || (tsz == _T('=')) || 
            (tsz == _T('%')) || (tsz == _T('/')) || (tsz == _T('\\')));// || (tsz == _T(' ')));

        // These 'allowed' characters seem not to work:
        // (tsz == _T('&')) || (tsz == _T('(')) || (tsz == _T(')')) || (tsz == _T(' ')) || 

        // "escape" sequence: "%" hex hex
        if (tsz == _T('%')) {
            if (i < (csUrl.GetLength()-2)) {
                i++;
                _TCHAR h1 = csUrl.GetAt(i);
                i++;
                _TCHAR h2 = csUrl.GetAt(i);
                bAllOk = bAllOk && ((h1 >= _T('0') && h1 <= _T('9')) || 
                    (h1 >= _T('a') && h1 <= _T('f')) || (h1 >= _T('A') && h1 <= _T('F')));
                bAllOk = bAllOk && ((h2 >= _T('0') && h2 <= _T('9')) || 
                    (h2 >= _T('a') && h2 <= _T('f')) || (h2 >= _T('A') && h2 <= _T('F')));
            } else {
                bAllOk = bAllOk && FALSE;
            }
        }
    }

    return bAllOk;
}

void CMainDlg::SearchForLrd(CString csMediaFile) {
    CString strDirectory;
    int iPos = csMediaFile.ReverseFind(_T('\\'));
    if (iPos != -1) {
        strDirectory = csMediaFile.Left(iPos);
    }
    // set as current directory   
    ::SetCurrentDirectory(strDirectory);
    // update 
    if (!m_bIsContextMenu) {
        UpdateData();
    }
    m_csLrdFile = FindFirstLrdFile();
    ParseLrd(m_bIsContextMenu);
    if (!m_bIsContextMenu) {
        UpdateData(FALSE);
    }
}

void CMainDlg::ParseLrd(bool bIsContextMenu) {
    CParseLrd lrd;
    lrd.Parse(m_csLrdFile);
    if (!m_bIsContextMenu && m_RLrdMetadata.GetCheck()) {
        m_csTitle = lrd.GetTitle();
        m_csAuthor = lrd.GetAuthor();
        m_edtAuthor.SetWindowText(m_csAuthor/*lrd.GetAuthor()*/);
        m_edtTitle.SetWindowText(m_csTitle/*lrd.GetTitle()*/);
    }
    if (m_bIsContextMenu) {
        m_csTitle = lrd.GetTitle();
        m_csAuthor = lrd.GetAuthor();
    }
}

CString CMainDlg::FindFirstLrdFile() {
    TCHAR tszCurrentDirectory[MAX_PATH];
    CString csLrdFile = _T("");

    WIN32_FIND_DATA w32fd;
    ZeroMemory(&w32fd, sizeof(w32fd));
    w32fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

    CString csDir;
    ::GetCurrentDirectory(MAX_PATH, tszCurrentDirectory);
    csDir.SetString(tszCurrentDirectory);
    csDir = csDir.Left(csDir.ReverseFind(_T('\\')));
    HANDLE hFind = ::FindFirstFile(csDir + _T("\\*.lrd"), &w32fd);

    if (hFind != INVALID_HANDLE_VALUE) {
        ::FindClose(hFind);
        CString csFile;
        csFile.SetString(w32fd.cFileName);
        csLrdFile = csDir + _T("\\") + csFile; 
    }
    return csLrdFile;
}

void CMainDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) {
    CDialog::OnActivate(nState, pWndOther, bMinimized);

    // TODO: Add your message handler code here
}


void CMainDlg::ShowYtMainDlg(void) {
    CYtMainDialog ytDlg;
    //CYtDialog ytDlg;
    ytDlg.DoModal();
}
