// TransferPage.cpp : implementation file
//

#include "stdafx.h"
#include "Uploader.h"
#include "TransferPage.h"
#include "MfcUtils.h"
#include "TransferConfigDlg.h"
#include "GeneralSettings.h"
#include "Registry.h"
#include "Crypto.h"


// CTransferPage dialog

IMPLEMENT_DYNAMIC(CTransferPage, CDialog)

CTransferPage::CTransferPage(CWnd* pParent /*=NULL*/): CDialog(CTransferPage::IDD, pParent) {
    m_bTemporaryChannel = false;
}

CTransferPage::~CTransferPage() {
}

void CTransferPage::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SERVICE_CHANNEL, m_stcChannelService);
    DDX_Control(pDX, IDC_CHANNEL, m_stcChannelUrl);
    DDX_Control(pDX, IDC_HOSTNAME_CHANNEL, m_stcChannelServer);
    DDX_Control(pDX, IDC_PORT_CHANNEL, m_stcChannelPort);
    DDX_Control(pDX, IDC_USERNAME_CHANNEL, m_stcChannelUsername);
    DDX_Control(pDX, IDC_PASSWORD_CHANNEL, m_stcChannelPassword);
    DDX_Control(pDX, IDC_SAVE_CHANNEL, m_stcChannelSave);
    DDX_Control(pDX, IDC_XMLDIR, m_stcChannelDir);
    DDX_Control(pDX, IDB_CHANGE, m_btnChannelChange);

    DDX_Control(pDX, IDC_SERVICE_MEDIA, m_stcMediaService);
    DDX_Control(pDX, IDC_WEBURL, m_stcMediaUrl);
    DDX_Control(pDX, IDC_HOSTNAME_MEDIA, m_stcMediaServer);
    DDX_Control(pDX, IDC_PORT_MEDIA, m_stcMediaPort);
    DDX_Control(pDX, IDC_USERNAME_MEDIA, m_stcMediaUsername);
    DDX_Control(pDX, IDC_PASSWORD_MEDIA, m_stcMediaPassword);
    DDX_Control(pDX, IDC_SAVE_MEDIA, m_stcMediaSave);
    DDX_Control(pDX, IDC_MP4DIR, m_stcMediaDir);
    DDX_Control(pDX, IDB_CHANGE_MEDIA, m_btnMediaChange);
}


BEGIN_MESSAGE_MAP(CTransferPage, CDialog)
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDB_CHANGE, OnChangeChannel)
    ON_BN_CLICKED(IDB_CHANGE_MEDIA, OnChangeMedia)
END_MESSAGE_MAP()

// CTransferPage message handlers
BOOL CTransferPage::OnInitDialog() {
    CDialog::OnInitDialog();

    DWORD dwID[] = {
        IDL_CHANNEL,
        IDL_SERVICE,
        IDL_HOST,
        IDL_PORT,
        IDL_USERNAME,
        IDL_PASSWORD,
        IDL_SAVEPSWD,
        IDB_CHANGE,
        IDL_MP4URL,
        IDL_PODCAST_CHANNEL,
        IDL_PODCAST_MEDIA_SERVER,
        -1 
    };
    MfcUtils::Localize(this, dwID);

    CString csCaption;

    csCaption.LoadString(IDL_SERVICE);
    SetDlgItemText(IDL_SERVICE_MEDIA, csCaption);

    csCaption.LoadString(IDL_HOST);
    SetDlgItemText(IDL_HOST_MEDIA, csCaption);

    csCaption.LoadString(IDL_PORT);
    SetDlgItemText(IDL_PORT_MEDIA, csCaption);

    csCaption.LoadString(IDL_USERNAME);
    SetDlgItemText(IDL_USERNAME_MEDIA, csCaption);

    csCaption.LoadString(IDL_PASSWORD);
    SetDlgItemText(IDL_PASSWORD_MEDIA, csCaption);

    csCaption.LoadString(IDL_SAVEPSWD);
    SetDlgItemText(IDL_SAVEPSWD_MEDIA, csCaption);

    csCaption.LoadString(IDB_CHANGE);
    SetDlgItemText(IDB_CHANGE_MEDIA, csCaption);

    csCaption.LoadString(IDL_DIRECTORY);
    SetDlgItemText(IDL_MP4DIR, csCaption);
    SetDlgItemText(IDL_CHANNELDIR, csCaption);

    m_stcChannelService.SetWindowText(_T(""));
    m_stcChannelUrl.SetWindowText(_T(""));
    m_stcChannelServer.SetWindowText(_T(""));
    m_stcChannelPort.SetWindowText(_T(""));
    m_stcChannelUsername.SetWindowText(_T(""));
    m_stcChannelPassword.SetWindowText(_T(""));
    m_stcChannelSave.SetWindowText(_T(""));
    m_stcChannelDir.SetWindowText(_T(""));

    m_stcMediaService.SetWindowText(_T(""));
    m_stcMediaUrl.SetWindowText(_T(""));
    m_stcMediaServer.SetWindowText(_T(""));
    m_stcMediaPort.SetWindowText(_T(""));
    m_stcMediaUsername.SetWindowText(_T(""));
    m_stcMediaPassword.SetWindowText(_T(""));
    m_stcMediaSave.SetWindowText(_T(""));
    m_stcMediaDir.SetWindowText(_T(""));

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CTransferPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    pDC->SetBkColor(RGB(255, 255, 255));
    hbr = CreateSolidBrush(RGB(255, 255, 255));

    return hbr;
}


void CTransferPage::LoadFromRegistry() {

    CRegistry reg;

    if (reg.Open(m_csChannelName) == ERROR_SUCCESS) {
        UpdateData();

        CString csText;
        int nProfile;

        //Channel data

        int iService = reg.GetProfileInt(_T("Channel Service"), UPLOAD_FTP);
        switch (iService) {
        case UPLOAD_FTP:
            m_stcChannelService.SetWindowText(_T("FTP"));
            break;
        case UPLOAD_SCP:
            m_stcChannelService.SetWindowText(_T("SCP"));
            break;
        case UPLOAD_SFTP:
            m_stcChannelService.SetWindowText(_T("SFTP"));
            break;
        }

        m_stcChannelUrl.SetWindowText(reg.GetProfileString(_T("Channel URL"), _T("")));
        m_stcChannelServer.SetWindowText(reg.GetProfileString(_T("Channel Server"), _T("")));

        nProfile = reg.GetProfileInt(_T("Channel Port"), DEFAULT_FTP_PORT);
        csText.Format(_T("%d"), nProfile);
        m_stcChannelPort.SetWindowText(csText);

        m_stcChannelUsername.SetWindowText(reg.GetProfileString(_T("Channel Username"), _T("")));

        BOOL bSavePassword = reg.GetProfileInt(_T("Channel SavePassword"), TRUE);  
        if (bSavePassword) {
            CString csAux = reg.GetProfileString(_T("Channel Password"), _T(""));
            CCrypto crypto;
            CString csPassword = csAux.GetString();
            csPassword = crypto.Decode(csPassword);
            CString csText;
            for(int i=0; i<csPassword.GetLength();i++)
                csText += _T("*");
            m_stcChannelPassword.SetWindowText(csText);
        } else {
            m_stcChannelPassword.SetWindowText(_T(""));
        }

        CString csSavePassword;
        if (bSavePassword) {
            csSavePassword.LoadString(IDS_YES);
        } else {
            csSavePassword.LoadString(IDS_NO);
        }

        m_stcChannelSave.SetWindowText(csSavePassword);
        m_stcChannelDir.SetWindowText(reg.GetProfileString(_T("Channel Directory"), _T("/")));

        //Media data

        iService = reg.GetProfileInt(_T("Media Service"), UPLOAD_FTP);
        switch (iService){
        case UPLOAD_FTP:
            m_stcMediaService.SetWindowText(_T("FTP"));
            break;
        case UPLOAD_SCP:
            m_stcMediaService.SetWindowText(_T("SCP"));
            break;
        case UPLOAD_SFTP:
            m_stcMediaService.SetWindowText(_T("SFTP"));
            break;
        }

        m_stcMediaUrl.SetWindowText(reg.GetProfileString(_T("Media URL"), _T("")));
        m_stcMediaServer.SetWindowText(reg.GetProfileString(_T("Media Server"), _T("")));

        nProfile = reg.GetProfileInt(_T("Media Port"), DEFAULT_FTP_PORT);
        csText.Format(_T("%d"), nProfile);
        m_stcMediaPort.SetWindowText(csText);

        m_stcMediaUsername.SetWindowText(reg.GetProfileString(_T("Media Username"), _T("")));

        bSavePassword = reg.GetProfileInt(_T("Media SavePassword"), TRUE);  
        if (bSavePassword) {
            CString csAux = reg.GetProfileString(_T("Media Password"), _T(""));
            CCrypto crypto;
            CString csPassword = csAux.GetString();
            csPassword = crypto.Decode(csPassword);
            CString csText;
            for (int i=0; i<csPassword.GetLength();i++) {
                csText += _T("*");
            }
            m_stcMediaPassword.SetWindowText(csText);
        } else {
            m_stcMediaPassword.SetWindowText(_T(""));
        }

        if (bSavePassword) {
            csSavePassword.LoadString(IDS_YES);
        } else {
            csSavePassword.LoadString(IDS_NO);
        }

        m_stcMediaSave.SetWindowText(csSavePassword);
        m_stcMediaDir.SetWindowText(reg.GetProfileString(_T("Media Directory"), _T("/")));

        UpdateData(FALSE);
        reg.Close();
    }
}

void CTransferPage::ClearPage() {
    UpdateData();

    m_stcChannelService.SetWindowText(_T(""));
    m_stcChannelUrl.SetWindowText(_T(""));
    m_stcChannelServer.SetWindowText(_T(""));
    m_stcChannelPort.SetWindowText(_T(""));
    m_stcChannelUsername.SetWindowText(_T(""));
    m_stcChannelPassword.SetWindowText(_T(""));
    m_stcChannelSave.SetWindowText(_T(""));
    m_stcChannelDir.SetWindowText(_T(""));

    m_stcMediaService.SetWindowText(_T(""));
    m_stcMediaUrl.SetWindowText(_T(""));
    m_stcMediaServer.SetWindowText(_T(""));
    m_stcMediaPort.SetWindowText(_T(""));
    m_stcMediaUsername.SetWindowText(_T(""));
    m_stcMediaPassword.SetWindowText(_T(""));
    m_stcMediaSave.SetWindowText(_T(""));
    m_stcMediaDir.SetWindowText(_T(""));

    UpdateData(FALSE);
}



BOOL CTransferPage::DestroyWindow() {
    // TODO: Add your specialized code here and/or call the base class
    UpdateData();
    UpdateData(FALSE);

    return CDialog::DestroyWindow();
}

void CTransferPage::OnChangeChannel() {
    // TODO: Add your control notification handler code here


    CGeneralSettings *tab = (CGeneralSettings *)this->GetParent();

    if (m_bTemporaryChannel) {
        CTransferConfigDlg dlgTransferConfig(1, m_tcTempChannel);
        dlgTransferConfig.SetTempChannel(true);
        if (dlgTransferConfig.DoModal() == IDOK) {
            m_tcTempChannel = dlgTransferConfig.GetTransferParam();
            SetTemporaryChannel(m_tcTempChannel);
            tab->SetTemporaryChannel(m_tcTempChannel);
        }
    } else {
        CRegistry reg;
        TransferParam tpChannel;
        reg.LoadChannelTransferParam(tpChannel, m_csChannelName);

        CTransferConfigDlg dlgTransferConfig(1, tpChannel);
        dlgTransferConfig.SetChannelName(m_csChannelName);

        if (dlgTransferConfig.DoModal() == IDOK) {
            tab->Refresh();
            LoadFromRegistry();
        }
    }
}

void CTransferPage::OnChangeMedia() {
    // TODO: Add your control notification handler code here

    CGeneralSettings *tab = (CGeneralSettings *)this->GetParent();

    if (m_bTemporaryChannel) {
        CTransferConfigDlg dlgTransferConfig(2, m_tcTempMedia);
        dlgTransferConfig.SetTempChannel(true);
        if (dlgTransferConfig.DoModal() == IDOK) {
            m_tcTempMedia = dlgTransferConfig.GetTransferParam();
            SetTemporaryMedia(m_tcTempMedia);
            tab->SetTemporaryMedia(m_tcTempMedia);
        }
    } else {
        CRegistry reg;
        TransferParam tpMedia;
        reg.LoadMediaTransferParam(tpMedia, m_csChannelName);

        CTransferConfigDlg dlgTransferConfig(2, tpMedia);
        dlgTransferConfig.SetChannelName(m_csChannelName);

        if (dlgTransferConfig.DoModal() == IDOK) {
            LoadFromRegistry();
        }
    }
}

void CTransferPage::SetChannelName(CString csChannelName) {
    m_csChannelName = csChannelName;
}

CString CTransferPage::GetChannelName() {
    return m_csChannelName;
}

CString CTransferPage::GetChannelURL() {
    CString csText;
    m_stcChannelUrl.GetWindowText(csText);
    return csText;
}

void CTransferPage::SetTempChannel(bool bTempChannel) {
    m_bTemporaryChannel = bTempChannel;
}

void CTransferPage::SetTemporaryChannel(TransferParam tpTempChannel) {
    m_tcTempChannel = tpTempChannel;

    switch (m_tcTempChannel.iService) {
    case UPLOAD_FTP:
        m_stcChannelService.SetWindowText(_T("FTP"));
        break;
    case UPLOAD_SCP:
        m_stcChannelService.SetWindowText(_T("SCP"));
        break;
    case UPLOAD_SFTP:
        m_stcChannelService.SetWindowText(_T("SFTP"));
        break;
    }

    m_stcChannelUrl.SetWindowText(m_tcTempChannel.csUrl);
    m_stcChannelServer.SetWindowText(m_tcTempChannel.csServer);

    CString csText;
    csText.Format(_T("%d"), m_tcTempChannel.nPort);
    m_stcChannelPort.SetWindowText(csText);

    m_stcChannelUsername.SetWindowText(m_tcTempChannel.csUsername);

    if (m_tcTempChannel.bSavePassword) {
        csText.Empty();
        for (int i=0; i<m_tcTempChannel.csPassword.GetLength();i++) {
            csText += _T("*");
        }
        m_stcChannelPassword.SetWindowText(csText);
    } else {
        m_stcChannelPassword.SetWindowText(_T(""));
    }

    m_stcChannelDir.SetWindowText(m_tcTempChannel.csDirectory);


    CString csSavePassword;
    if (m_tcTempChannel.bSavePassword) {
        csSavePassword.LoadString(IDS_YES);
    } else {
        csSavePassword.LoadString(IDS_NO);
    }

    m_stcChannelSave.SetWindowText(csSavePassword);
}

void CTransferPage::SetTemporaryMedia(TransferParam tpTempMedia) {
    m_tcTempMedia = tpTempMedia;
    switch (m_tcTempMedia.iService) {
    case UPLOAD_FTP:
        m_stcMediaService.SetWindowText(_T("FTP"));
        break;
    case UPLOAD_SCP:
        m_stcMediaService.SetWindowText(_T("SCP"));
        break;
    case UPLOAD_SFTP:
        m_stcMediaService.SetWindowText(_T("SFTP"));
        break;
    }

    m_stcMediaUrl.SetWindowText(m_tcTempMedia.csUrl);
    m_stcMediaServer.SetWindowText(m_tcTempMedia.csServer);

    CString csText;
    csText.Format(_T("%d"), m_tcTempMedia.nPort);
    m_stcMediaPort.SetWindowText(csText);

    m_stcMediaUsername.SetWindowText(m_tcTempMedia.csUsername);

    if (m_tcTempMedia.bSavePassword) {
        csText.Empty();
        for (int i=0; i<m_tcTempMedia.csPassword.GetLength();i++) {
            csText += _T("*");
        }
        m_stcMediaPassword.SetWindowText(csText);
    } else {
        m_stcMediaPassword.SetWindowText(_T(""));
    }

    m_stcMediaDir.SetWindowText(m_tcTempMedia.csDirectory);

    CString csSavePassword;
    if (m_tcTempMedia.bSavePassword) {
        csSavePassword.LoadString(IDS_YES);
    } else {
        csSavePassword.LoadString(IDS_NO);
    }

    m_stcMediaSave.SetWindowText(csSavePassword);
}

TransferParam CTransferPage::GetTemporaryChannel() {
    return m_tcTempChannel;
}
void CTransferPage::EnableChangeButton(bool bEnable) {
    m_btnChannelChange.EnableWindow(bEnable);
    m_btnMediaChange.EnableWindow(bEnable);
}
