// MetadataPage.cpp : implementation file
//

#include "stdafx.h"
#include "Uploader.h"
#include "MfcUtils.h"
#include "ChannelMetadata.h"
#include "SetMetadata.h"
#include "GetMetadata.h"
#include "TransferPage.h"
#include "MetadataPage.h"
#include "GeneralSettings.h"


// CMetadataPage dialog

IMPLEMENT_DYNAMIC(CMetadataPage, CDialog)

CMetadataPage::CMetadataPage(CWnd* pParent /*=NULL*/): CDialog(CMetadataPage::IDD, pParent) {
    m_bTemporaryChannel = false;
}

CMetadataPage::~CMetadataPage() {
}

void CMetadataPage::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TITLE, m_stcTitle);
    DDX_Control(pDX, IDC_SUBTITLE, m_stcSubtitle);
    DDX_Control(pDX, IDC_AUTHOR, m_stcAuthor);
    DDX_Control(pDX, IDC_DESCRIPTION, m_stcDescription);
    DDX_Control(pDX, IDC_SUMMARY, m_stcSummary);
    DDX_Control(pDX, IDC_LANGUAGE, m_stcLanguage);
    DDX_Control(pDX, IDC_COPYRIGHT, m_stcCopyright);
    DDX_Control(pDX, IDC_OWNER, m_stcOwner);
    DDX_Control(pDX, IDC_EMAIL, m_stcEmail);
    DDX_Control(pDX, IDC_LINK, m_stcLink);
    DDX_Control(pDX, IDC_WEBMASTER, m_stcWebmaster);
    DDX_Control(pDX, IDC_EXPLICIT, m_stcExplicit);
    DDX_Control(pDX, IDC_KEYWORDS, m_stcKeywords);
    DDX_Control(pDX, IDB_CHANGE, m_btnChange);
}


BEGIN_MESSAGE_MAP(CMetadataPage, CDialog)
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDB_CHANGE, OnChange)
END_MESSAGE_MAP()


// CMetadataPage message handlers
BOOL CMetadataPage::OnInitDialog() {
    CDialog::OnInitDialog();

    pApp = (CUploaderApp *)AfxGetApp();

    DWORD dwID[] = {
        IDL_TITLE,
        IDL_SUBTITLE,
        IDL_AUTHOR,
        IDL_DESCRIPTION,
        IDL_SUMMARY,
        IDL_LANGUAGE,
        IDL_COPYRIGHT,
        IDL_OWNER,
        IDL_MAIL,
        IDL_LINK,
        IDL_WEBMASTER,
        IDL_EXPLICIT,
        IDL_KEYWORDS,
        IDB_CHANGE,
        -1 
    };
    MfcUtils::Localize(this, dwID);

    m_stcTitle.SetWindowText(_T(""));
    m_stcSubtitle.SetWindowText(_T(""));
    m_stcAuthor.SetWindowText(_T(""));
    m_stcDescription.SetWindowText(_T(""));
    m_stcSummary.SetWindowText(_T(""));
    m_stcLanguage.SetWindowText(_T(""));
    m_stcCopyright.SetWindowText(_T(""));
    m_stcOwner.SetWindowText(_T(""));
    m_stcEmail.SetWindowText(_T(""));
    m_stcLink.SetWindowText(_T(""));
    m_stcWebmaster.SetWindowText(_T(""));
    m_stcExplicit.SetWindowText(_T(""));
    m_stcKeywords.SetWindowText(_T(""));

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CMetadataPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    pDC->SetBkColor(RGB(255, 255, 255));
    hbr = CreateSolidBrush(RGB(255, 255, 255));

    return hbr;
}

void CMetadataPage::OnChange() {
    // TODO: Add your control notification handler code here
    CChannelMetadata dlgChannelMeta;
    UpdateData();

    TransferMetadata tmMetadata;

    m_stcTitle.GetWindowText(tmMetadata.csTitle);
    m_stcSubtitle.GetWindowText(tmMetadata.csSubtitle);
    m_stcAuthor.GetWindowText(tmMetadata.csAuthor);
    m_stcDescription.GetWindowText(tmMetadata.csDescription);
    m_stcSummary.GetWindowText(tmMetadata.csSummary);
    m_stcLanguage.GetWindowText(tmMetadata.csLanguage);
    m_stcCopyright.GetWindowText(tmMetadata.csCopyright);
    m_stcOwner.GetWindowText(tmMetadata.csOwner);
    m_stcEmail.GetWindowText(tmMetadata.csEmail);
    m_stcLink.GetWindowText(tmMetadata.csLink);
    m_stcWebmaster.GetWindowText(tmMetadata.csWebmaster);
    m_stcExplicit.GetWindowText(tmMetadata.csExplicit);
    m_stcKeywords.GetWindowText(tmMetadata.csKeywords);

    dlgChannelMeta.SetMetadata(tmMetadata);
    dlgChannelMeta.SetTempChannel(m_bTemporaryChannel);


    if (dlgChannelMeta.DoModal() == IDOK) {
        CSetMetadata smd;

        tmMetadata = dlgChannelMeta.GetMetadata();
        smd.SaveMetadata(tmMetadata);

        CGeneralSettings *tab = (CGeneralSettings *)this->GetParent();

        if (m_bTemporaryChannel) {
            CString csTempChannel = pApp->GetTempDirectory() + _T("\\TempChannel.xml");
            GetMetadata(csTempChannel);
            m_tmTempMetadata = dlgChannelMeta.GetTempMetadata();
            tab->SetTemporaryMetadata(m_tmTempMetadata);
        } else {
            CTransferPage *tpag = (CTransferPage *)tab->m_TabControl.m_Dialog[1];

            // use temporary storage
            CString csFile = pApp->GetTempDirectory() + _T("\\mod.xml");
            if(smd.Parse(tpag->GetChannelURL(), csFile) == TRUE) {
                tab->UploadChannel(tpag->GetChannelName(), csFile);
                GetMetadata(tpag->GetChannelURL());
            }
            tab->Refresh();
        }
    }
}

BOOL CMetadataPage::GetMetadata(CString csUrl, BOOL bDisplayError) {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    CGetMetadata meta;
    BOOL bResult = meta.Parse(csUrl, bDisplayError);

    UpdateData();
    TransferMetadata tmMetadata;
    tmMetadata = meta.GetMetadata();

    m_stcTitle.SetWindowText(tmMetadata.csTitle);
    m_stcSubtitle.SetWindowText(tmMetadata.csSubtitle);
    m_stcAuthor.SetWindowText(tmMetadata.csAuthor);
    m_stcDescription.SetWindowText(tmMetadata.csDescription);
    m_stcSummary.SetWindowText(tmMetadata.csSummary);
    m_stcLanguage.SetWindowText(tmMetadata.csLanguage);
    m_stcCopyright.SetWindowText(tmMetadata.csCopyright);
    m_stcOwner.SetWindowText(tmMetadata.csOwner);
    m_stcEmail.SetWindowText(tmMetadata.csEmail);
    m_stcLink.SetWindowText(tmMetadata.csLink);
    m_stcWebmaster.SetWindowText(tmMetadata.csWebmaster);
    m_stcExplicit.SetWindowText(tmMetadata.csExplicit);
    m_stcKeywords.SetWindowText(tmMetadata.csKeywords);

    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
    return true;
}

CString CMetadataPage::GetTitle() {
    CString csText;
    m_stcTitle.GetWindowText(csText);
    return csText;
}

void CMetadataPage::EnableChangeButton(bool bEnable) {
    m_btnChange.EnableWindow(bEnable);
}

void CMetadataPage::SetTempChannel(bool bTempChannel) {
    m_bTemporaryChannel = bTempChannel;
}

void CMetadataPage::SetTempMetadata(TransferMetadata tmTempMetadata) {
    m_tmTempMetadata = tmTempMetadata;
}