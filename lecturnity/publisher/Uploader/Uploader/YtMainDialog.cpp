// YtMainDialog.cpp : implementation file
//

#include "stdafx.h"
#include "afxinet.h"
#include "Uploader.h"
#include "TransferProgressDlg.h"
#include "TransferingContent.h"
#include "YtMainDialog.h"
#include "YouTubeSettings.h"
#include "UploaderUtils.h"


// CYtMainDialog dialog

IMPLEMENT_DYNAMIC(CYtMainDialog, CDialog)

CYtMainDialog::CYtMainDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CYtMainDialog::IDD, pParent) {
    m_csAuthToken = _T("");
    m_acsYtCategories.RemoveAll();
    InitCategories();
}

CYtMainDialog::~CYtMainDialog() {
}

void CYtMainDialog::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_DUMMY, m_edtDummy);
    DDX_Control(pDX, IDC_YT_UPLOAD_BUTTON, m_btnYtUpload);
    DDX_Control(pDX, IDC_EDIT_YT_FILE, m_edtMp4);
    DDX_Control(pDX, IDC_EDIT_DMY2, m_edtDmy2);
}


BEGIN_MESSAGE_MAP(CYtMainDialog, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_YT_AUTH, &CYtMainDialog::OnBnClickedButtonYtAuth)
    ON_BN_CLICKED(IDC_YT_UPLOAD_BUTTON, &CYtMainDialog::OnBnClickedYtUploadButton)
    ON_BN_CLICKED(IDC_YT_BROWSE_BUTTON, &CYtMainDialog::OnBnClickedYtBrowseButton)
    ON_BN_CLICKED(IDC_BUTTON1, &CYtMainDialog::OnBnClickedButton1)
END_MESSAGE_MAP()


// CYtMainDialog message handlers

void CYtMainDialog::OnBnClickedButtonYtAuth() {
    // TODO: Add your control notification handler code here
    m_ytTp.csCategory = _T("Education");
    m_ytTp.csTitle = _T("Un Titlu");
    m_ytTp.csDescription = _T("Film foarte scurt");
    m_ytTp.csKeywords = _T("scurt");
    //CYTTransferDlg dlg(m_ytTp, m_acsYtCategories, this);
    //CYouTubeSettings dlg;
    //ProfileUtils::ShowYouTubeSettings();
    CUploaderUtils uu;
    int iResId = uu.ShowYouTubeSettingsDialog(this, m_ytTp.csUsername, m_ytTp.csPassword, m_ytTp.csTitle,
                    m_ytTp.csDescription, m_ytTp.csKeywords, m_ytTp.csCategory, m_ytTp.iPrivacy);
    //uu.ShowYoutubeSettingsDialog();

    if (iResId == IDOK) {
       // m_ytTp = dlg.GetYtTransferParams();
        m_btnYtUpload.EnableWindow(true);
    } else {
        m_edtDummy.SetWindowText(_T("s-a apasat cancel"));
        return;
    }

    /*CInternetSession session;
     CString csPath = _T("/youtube/accounts/ClientLogin");
     CString csQuerryFormat =  _T("?Email=%s&Passwd=%s&service=youtube&source=Lec.YouTube.Library");
     CString csQuerry;
     csQuerry.Format(csQuerryFormat, m_ytTp.csUsername, m_ytTp.csPassword);
     csPath += csQuerry;
    CHttpConnection* pConnection = session.GetHttpConnection(_T("www.google.com"), INTERNET_DEFAULT_HTTPS_PORT, _T(""), _T(""));
    CHttpFile* pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, csPath,
        NULL, 1, NULL, HTTP_VERSION, 
                             INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID |INTERNET_FLAG_IGNORE_CERT_DATE_INVALID);
    CString csHeaders = _T("Content-Type: application/x-www-form-urlencoded\r\n");
   */

    //pFile->AddRequestHeaders(csHeaders, HTTP_ADDREQ_FLAG_ADD);
    //BOOL result = pFile->SendRequest(csHeaders/*, (LPVOID)(LPCTSTR)csQuerry, csQuerry.GetLength()*/);
    //pFile->WriteString(csQuerry);
    //result = pFile->EndRequest(HSR_SYNC);
    /*if(result != FALSE)
    {
        char aa;
        CString csResponse = _T("");
        csResponse.Format(_T("result = %d\n"), result);
        while(pFile->Read(&aa,1) == 1)
        {
            csResponse += aa;
        }
        m_edtDummy.SetWindowText(csResponse);
        CString csAuthToken, csErrorString;
        if(GetAuthToken(csResponse, csAuthToken, csErrorString))
        {
            m_csAuthToken = csAuthToken;
            m_btnYtUpload.EnableWindow(TRUE);
            //MessageBox(m_csAuthToken);
        }
        
        
    }
    else
    {
        MessageBox(_T("Ceva nu-i bine!!!"));
    }
    pFile->Close();
    session.Close();
*/

}

bool CYtMainDialog::GetAuthToken(CString csResponse, CString &csToken, CString& csError) {
    bool bRes = false;
    if (csResponse.IsEmpty()) {
        return false;
    }
    int iPos = 0;
    CString csSeparator = _T("\n");
    CString csTok = csResponse.Tokenize(csSeparator, iPos);
    while (csTok != "") {
        if (csTok.Find(_T("Auth=")) != -1) {
            csToken = csTok.Right(csTok.GetLength() - 5);
            csError = _T("");
            bRes = true;
            break;
        }
        if (csTok.Find(_T("Error=")) != -1) {
            csError = csTok.Right(csTok.GetLength() - 6);
            csToken = _T("");
            bRes = false;
            break;
        }
        csTok = csResponse.Tokenize(csSeparator, iPos);
    }
    return bRes;
}

void CYtMainDialog::InitCategories() {
    m_acsYtCategories.Add(_T("Film"));
    m_acsYtCategories.Add(_T("Autos"));
    m_acsYtCategories.Add(_T("Music"));
    m_acsYtCategories.Add(_T("Animals"));
    m_acsYtCategories.Add(_T("Sports"));
    m_acsYtCategories.Add(_T("Travel"));
    m_acsYtCategories.Add(_T("Shortmov"));
    m_acsYtCategories.Add(_T("Videoblog"));
    m_acsYtCategories.Add(_T("Games"));
    m_acsYtCategories.Add(_T("Comedy"));
    m_acsYtCategories.Add(_T("People"));
    m_acsYtCategories.Add(_T("News"));
    m_acsYtCategories.Add(_T("Entertainment"));
    m_acsYtCategories.Add(_T("Education"));
    m_acsYtCategories.Add(_T("Howto"));
    m_acsYtCategories.Add(_T("Nonprofit"));
    m_acsYtCategories.Add(_T("Tech"));
}

void CYtMainDialog::OnBnClickedYtUploadButton() {
    // TODO: Add your control notification handler code here
    CString csMp4;
    m_edtMp4.GetWindowText(csMp4);
    if (csMp4.IsEmpty()) {
        m_btnYtUpload.EnableWindow(FALSE);
        MessageBox(_T("Nu e fisier selectat"));
        return;
    }

    int piCurrFile = 0;
    double dProgress;
    bool *pbIsCancel = new bool();
    *pbIsCancel = false;
    bool bIsThreadActive = false;
    CTransferingContent& rTContent = CTransferingContent::GetInstance();
    rTContent.MyStartYtUpload(m_ytTp, csMp4, _T(""), &dProgress, &piCurrFile, pbIsCancel, &bIsThreadActive);

    CTransferProgressDlg dlg(&dProgress, csMp4, _T("youtube"), this);
    if (dlg.DoModal() == IDCANCEL) {
        *pbIsCancel = true;
        while (bIsThreadActive)
            Sleep(400);
    } else {
        CString csMess;
        csMess.LoadString(IDS_SUCCESS);
        AfxMessageBox(csMess);
    }

    MessageBox(_T("am terminat!!!!"));

}

void CYtMainDialog::OnBnClickedYtBrowseButton() {
    // TODO: Add your control notification handler code here
    CString csFilter = _T("Podcast Files (*.mp4)|*.mp4|All Files (*.*)|*.*||");
    CFileDialog dlgOpen(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, csFilter, this);
    if (dlgOpen.DoModal() == IDOK) {
        m_edtMp4.SetWindowText(dlgOpen.GetPathName());
    }
}

void CYtMainDialog::OnBnClickedButton1() {
    // TODO: Add your control notification handler code here
    CString csMp4;
    m_edtMp4.GetWindowText(csMp4);
    if (csMp4.IsEmpty()) {
        m_btnYtUpload.EnableWindow(FALSE);
        MessageBox(_T("Nu e fisier selectat"));
        return;
    }

    CFile file;
    BOOL bFile = file.Open(csMp4, CFile::modeRead);
    if (!bFile) {
        MessageBox(_T("Nu a putut fi deschis fisierul"));
        return;
    }

    int iFileLength = file.GetLength();
    char *fContent = new char[iFileLength];
    BYTE *buf= new BYTE[iFileLength];
    CArray<BYTE, BYTE>aC;
    BYTE aa;
    for (int i = 0; i < iFileLength; i++) {
        file.Read(&aa, 1);
        aC.Add(aa);
    }

  /*  for(int i = 0; i < iFileLength; i ++)
    {
        *buf = aC.GetAt(i);
        buf++;
    }*/
    //file.Read(buf, iFileLength);
    //int nmb = file.Read(fContent, iFileLength);
    //CString csContent;
    /*char aa;
    while(file.Read(&aa, 1) == 1)
    {
        csContent += aa;
    }*/
    file.Close();
   /* CString cs;
    for(int i = 0; i < aC.GetCount(); i++)
    {
        CString s;
        s.Format(_T("%s "), aC.GetAt(i));
        cs+=s;
    }*/

    CString csNewName;
    csNewName = csMp4.Left(csMp4.GetLength() - 4) + _T("1.txt");
    CFile nFile;
    nFile.Open(csNewName, CFile::modeCreate | CFile::modeWrite);
    for (int i = 0; i < aC.GetCount(); i++) {
        char a = aC.GetAt(i);
        nFile.Write(&a,1);
    }
    nFile.Close();
    /*int len = MultiByteToWideChar(CP_UTF8, 0, fContent, iFileLength, NULL, 0);
    LPWSTR lpWStr=new WCHAR[len];
    MultiByteToWideChar(CP_UTF8, 0, fContent, iFileLength, lpWStr, len);


    CString csLangFile = _T("");
    for(int l = 0; l < len; l++)
        csLangFile += lpWStr[l];
    //csContent.Format(_T("%s"), fContent);*/
    //m_edtDmy2.SetWindowTextW(cs);
    MessageBox(_T("Am terminat de citit"));

}
