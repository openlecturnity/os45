// ChannelMetadata.cpp : implementation file
//

#include "stdafx.h"
#include "Uploader.h"
#include "ChannelMetadata.h"
#include "MfcUtils.h"
#include "NewChannel.h"


// CChannelMetadata dialog

IMPLEMENT_DYNAMIC(CChannelMetadata, CDialog)

CChannelMetadata::CChannelMetadata(CWnd* pParent /*=NULL*/)
: CDialog(CChannelMetadata::IDD, pParent) {
    m_isKeyPressed = false;
    m_bTemporaryChannel = false;
    m_isFocused = true;
}

CChannelMetadata::~CChannelMetadata() {
}

void CChannelMetadata::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_TITLE, m_edtTitle);
    DDX_Control(pDX, IDE_SUBTITLE, m_edtSubtitle);
    DDX_Control(pDX, IDE_AUTHOR, m_edtAuthor);
    DDX_Control(pDX, IDE_DESCRIPTION, m_edtDescription);
    DDX_Control(pDX, IDE_SUMARRY, m_edtSummary);
    DDX_Control(pDX, IDE_LANGUAGE, m_edtLanguage);
    DDX_Control(pDX, IDE_COPYRIGHT, m_edtCopyright);
    DDX_Control(pDX, IDE_OWNER, m_edtOwner);
    DDX_Control(pDX, IDE_MAIL, m_edtEmail);
    DDX_Control(pDX, IDE_LINK, m_edtLink);
    DDX_Control(pDX, IDE_WEBMASTER, m_edtWebmaster);
    DDX_Control(pDX, IDE_EXPLICIT, m_edtExplicit);
    DDX_Control(pDX, IDE_KEYWORDS, m_edtKeywords);
    DDX_Control(pDX, IDC_GROUP_METADATA, m_gbWhiteRectangle);
}

BEGIN_MESSAGE_MAP(CChannelMetadata, CDialog)
    ON_WM_CTLCOLOR()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// CChannelMetadata message handlers

BOOL CChannelMetadata::OnInitDialog() {
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
        IDCANCEL,
        -1 };

        MfcUtils::Localize(this, dwID);
        CString csCaption;
        csCaption.LoadString(IDD_CHANNELMETADATA);
        SetWindowText(csCaption);

        SetIcon(AfxGetApp()->LoadIcon(IDI_UPLOADER), TRUE);

        InitMetadata(m_tmMetadata);
        return TRUE;
}

HBRUSH CChannelMetadata::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor) {   
    HBRUSH hbr = NULL;

    if (nCtlColor == CTLCOLOR_STATIC ) {
        pDC->SetBkColor(RGB(255,255, 255));
        hbr = CreateSolidBrush(RGB(255, 255, 255));
    } else {
        hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);  
    }
    return hbr;
}

void CChannelMetadata::OnOK() {
    if (m_bTemporaryChannel) {
        CString csTempChannel = pApp->GetTempDirectory() + _T("\\TempChannel.xml");

        m_edtTitle.GetWindowText(m_tmTempMetadata.csTitle);
        m_edtSubtitle.GetWindowText(m_tmTempMetadata.csSubtitle);
        m_edtAuthor.GetWindowText(m_tmTempMetadata.csAuthor);
        m_edtDescription.GetWindowText(m_tmTempMetadata.csDescription);
        m_edtSummary.GetWindowText(m_tmTempMetadata.csSummary);
        m_edtLanguage.GetWindowText(m_tmTempMetadata.csLanguage);
        m_edtCopyright.GetWindowText(m_tmTempMetadata.csCopyright);
        m_edtOwner.GetWindowText(m_tmTempMetadata.csOwner);
        m_edtEmail.GetWindowText(m_tmTempMetadata.csEmail);
        m_edtLink.GetWindowText(m_tmTempMetadata.csLink);
        m_edtWebmaster.GetWindowText(m_tmTempMetadata.csWebmaster);
        m_edtExplicit.GetWindowText(m_tmTempMetadata.csExplicit);
        m_edtKeywords.GetWindowText(m_tmTempMetadata.csKeywords);

        CreateTempChannel(csTempChannel, m_tmTempMetadata);
    } else {
        m_edtTitle.GetWindowText(m_tmMetadata.csTitle);
        m_edtSubtitle.GetWindowText(m_tmMetadata.csSubtitle);
        m_edtAuthor.GetWindowText(m_tmMetadata.csAuthor);
        m_edtDescription.GetWindowText(m_tmMetadata.csDescription);
        m_edtSummary.GetWindowText(m_tmMetadata.csSummary);
        m_edtLanguage.GetWindowText(m_tmMetadata.csLanguage);
        m_edtCopyright.GetWindowText(m_tmMetadata.csCopyright);
        m_edtOwner.GetWindowText(m_tmMetadata.csOwner);
        m_edtEmail.GetWindowText(m_tmMetadata.csEmail);
        m_edtLink.GetWindowText(m_tmMetadata.csLink);
        m_edtWebmaster.GetWindowText(m_tmMetadata.csWebmaster);
        m_edtExplicit.GetWindowText(m_tmMetadata.csExplicit);
        m_edtKeywords.GetWindowText(m_tmMetadata.csKeywords);	
    }
    CDialog::EndDialog(IDOK);
}

BOOL CChannelMetadata::CreateNewChannel(LPCTSTR tstrNewChannel) {
    BOOL bResult = FALSE;

    CNewChannel chn;
    chn.SetMetadata(m_tmMetadata);

    bResult = chn.Create(tstrNewChannel);

    return bResult;
}

BOOL CChannelMetadata::CreateTempChannel(LPCTSTR tstrTempChannel, TransferMetadata tmTempMetadata) {
    BOOL bResult = FALSE;

    CNewChannel chn;
    chn.SetMetadata(tmTempMetadata);

    bResult = chn.Create(tstrTempChannel);

    return bResult;
}

void CChannelMetadata::InitMetadata(TransferMetadata tmMetadata) {
    m_edtTitle.SetWindowText(tmMetadata.csTitle);
    m_edtSubtitle.SetWindowText(tmMetadata.csSubtitle);
    m_edtAuthor.SetWindowText(tmMetadata.csAuthor);
    m_edtDescription.SetWindowText(tmMetadata.csDescription);
    m_edtSummary.SetWindowText(tmMetadata.csSummary);
    m_edtLanguage.SetWindowText(tmMetadata.csLanguage);
    m_edtCopyright.SetWindowText(tmMetadata.csCopyright);
    m_edtOwner.SetWindowText(tmMetadata.csOwner);
    m_edtEmail.SetWindowText(tmMetadata.csEmail);
    m_edtLink.SetWindowText(tmMetadata.csLink);
    m_edtWebmaster.SetWindowText(tmMetadata.csWebmaster);
    m_edtExplicit.SetWindowText(tmMetadata.csExplicit);
    m_edtKeywords.SetWindowText(tmMetadata.csKeywords);
}

void CChannelMetadata::SetMetadata(TransferMetadata tmMetadata) {
    m_tmMetadata = tmMetadata;
}

TransferMetadata CChannelMetadata::GetMetadata() {
    return m_tmMetadata;
}

BOOL CChannelMetadata::PreTranslateMessage(MSG* pMsg) {
    if (!this->GetFocus()) {
        m_isFocused = false;
    }

    if (this->GetFocus() && !m_isFocused) {
        this->RedrawWindow();
        m_isFocused = true;
    }

    if (m_isKeyPressed) {
        this->RedrawWindow();
    }

    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB) {
        m_isKeyPressed = true;
        SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam); 
    } else if (pMsg->message == WM_SYSKEYDOWN) {
        m_isKeyPressed = true;
        SendMessage(WM_SYSKEYDOWN, pMsg->wParam, pMsg->lParam); 
    } else { 
        m_isKeyPressed = false;
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CChannelMetadata::SetTempChannel(bool bTempChannel) {
    m_bTemporaryChannel = bTempChannel;
}

TransferMetadata CChannelMetadata::GetTempMetadata() {
    return m_tmTempMetadata;
}