// FileExists.cpp : implementation file
//

#include "stdafx.h"
#include "MfcUtils.h"
#include <wininet.h>
#include "Uploader.h"
#include "NewFile.h"
#include "FileExists.h"


// CFileExists dialog

IMPLEMENT_DYNAMIC(CFileExists, CDialog)

CFileExists::CFileExists(bool bNoMody /*= false*/, CWnd* pParent /*=NULL*/)
: CDialog(CFileExists::IDD, pParent) {
    m_iAction = 0;
    m_csUrl = _T("");
    m_csFileName = _T("");
    m_bNoMody = bNoMody;
}

CFileExists::~CFileExists() {
}

void CFileExists::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDL_L2, m_cstFileName);
    DDX_Control(pDX, IDL_L6, m_lblMody);
    DDX_Control(pDX, IDMODY, m_btnMody);
}


BEGIN_MESSAGE_MAP(CFileExists, CDialog)
    ON_BN_CLICKED(IDOK, &CFileExists::OnBnClickedOk)
    ON_BN_CLICKED(IDMODY, &CFileExists::OnBnClickedMody)
    ON_BN_CLICKED(IDOVER, &CFileExists::OnBnClickedOver)
    ON_BN_CLICKED(IDCANCEL, &CFileExists::OnBnClickedCancel)
END_MESSAGE_MAP()


// CFileExists message handlers

BOOL CFileExists::OnInitDialog() {
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    SetIcon(AfxGetApp()->LoadIcon(IDI_UPLOADER), true);

    DWORD dwID[] = {
        IDL_L1,
        IDL_L3,
        IDL_L4,
        IDL_L5,
        IDL_L6,
        IDL_L7,
        IDOK,
        IDMODY,
        IDOVER,
        IDCANCEL,
        -1
    };
    MfcUtils::Localize(this, dwID);

    CString csCaption;
    csCaption.LoadString(IDD_MP4EXISTS);
    SetWindowText(csCaption);

    m_cstFileName.SetWindowText(m_csFileName);
    if (m_bNoMody) {
        m_lblMody.ShowWindow(SW_HIDE);
        m_btnMody.ShowWindow(SW_HIDE);
    }
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileExists::OnBnClickedOk() {
    // TODO: Add your control notification handler code here
    OnOK();
}

void CFileExists::OnBnClickedMody() {
    // TODO: Add your control notification handler code here
    CString csExt = _T(".mp4");

    int iPos = m_csFileName.ReverseFind(_T('.'));
    if (iPos != (-1)) {
        csExt = m_csFileName.Right(m_csFileName.GetLength() - iPos);
    }

    Filetype fType = TYPE_MP4;

    if (csExt == _T(".lzp")) {
        fType = TYPE_LZP; 
    }

    CNewFile nfDlg(fType);

    if (nfDlg.DoModal() == IDOK) {
        m_csFileName = nfDlg.GetNewFileName();
        m_cstFileName.SetWindowText(m_csFileName);
        m_iAction = 1;
        OnOK();
    }

}

void CFileExists::OnBnClickedOver() {
    // TODO: Add your control notification handler code here
    m_iAction = 2;
    OnCancel();
}

void CFileExists::OnBnClickedCancel() {
    // TODO: Add your control notification handler code here
    m_iAction = 0;
    OnCancel();
}

int CFileExists::RemoteFileExists(void) {
    int iResult = -1;   // network error
    DWORD dwFilePos = 0;
    HINTERNET hInternet = NULL, hDownload = NULL; 

    CString csUrl = m_csUrl + m_csFileName;

    hInternet = InternetOpen(_T("Internet"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet != NULL) {
        hDownload = InternetOpenUrl(hInternet, csUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (hDownload != NULL) {
            dwFilePos = InternetSetFilePointer(hDownload, 0, NULL, FILE_BEGIN, NULL);
            iResult = (dwFilePos != (DWORD)-1) ? 1 : 0;
            InternetCloseHandle(hDownload);
        } else {
            GetInetError();
        }
        InternetCloseHandle(hInternet);
    } else {
        GetInetError();
    }
    return iResult;
}

void CFileExists::GetInetError() {
    LPVOID lpMsgBuf;
    CString csErrorMessage, csFormat;
    DWORD dwErrorNo = ::GetLastError();

    if (dwErrorNo == 0) {
        return;
    }
    if (::FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwErrorNo,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL ) == 0) {
        // Handle the error.
        csFormat.LoadString(IDS_FTPERROR);
        csErrorMessage.Format(csFormat, dwErrorNo);
        ::MessageBox(NULL, csErrorMessage, _T("Uploader"), MB_OK|MB_ICONSTOP);
        return;
    }
    // Display the string.
    ::MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Uploader"), MB_OK|MB_ICONSTOP);
    // Free the buffer.
    ::LocalFree(lpMsgBuf);
}

void CFileExists::SetUrl(CString csUrl) {
    m_csUrl = csUrl;
}

void CFileExists::SetFilename(CString csFilename) {
    m_csFileName = csFilename;
}

CString CFileExists::GetNewFilename() {
    return m_csFileName;
}

int CFileExists::GetAction() {
    return m_iAction;
}