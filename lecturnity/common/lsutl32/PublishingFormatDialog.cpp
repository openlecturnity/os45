// PublishingFormatDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "resource.h"
#include "lsutl32.h"
#include "PublishingFormatDialog.h"
#include "YouTubeSettings.h"
#include "SlidestarSettings.h"
#include "ProfileInfo.h"
#include "ProfileManager.h"
#include "MfcUtils.h"
#include "ProfileUtils.h"
#include "lutils.h"
#include "LanguageSupport.h"

// CPublishingFormatDialog-Dialogfeld

IMPLEMENT_DYNAMIC(CPublishingFormatDialog, CDialog)

CPublishingFormatDialog::CPublishingFormatDialog(CWnd* pParent, CString csLrdName)
: CDialog(CPublishingFormatDialog::IDD, pParent) {
    m_csLrdName = csLrdName;

    m_iCurrentSelection = PublishingFormat::TYPE_NOTHING;
    m_hbrBg = NULL;

    m_hPipe = 0;

    m_hResource = NULL;
    m_bFromContextMenu = false;
    XTPPaintTheme xtpCurrentTheme = XTPPaintManager()->GetCurrentTheme();

    if (xtpCurrentTheme != xtpThemeRibbon) {
        m_hResource = LoadLibrary(_T("../Studio/Studio.exe"));
        XTPOffice2007Images()->SetHandle(m_hResource);  
        m_bFromContextMenu = true;
    }
}

CPublishingFormatDialog::~CPublishingFormatDialog() {
    if (m_hbrBg) {
        ::DeleteObject(m_hbrBg);
    }
    if (m_hPipe) {
        CloseHandle(m_hPipe);
    }
    if (m_bFromContextMenu) {
        if (m_hResource != NULL) {
            FreeLibrary(m_hResource);
            m_hResource = NULL;
        }      
    }
}

void CPublishingFormatDialog::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_FORMAT_YOUTUBE, m_btnFormatYouTube);
    DDX_Control(pDX, IDC_FORMAT_SLIDESTAR, m_btnFormatSlidestar);
    DDX_Control(pDX, IDC_FORMAT_FLASH, m_btnFormatFlash);
    DDX_Control(pDX, IDC_FORMAT_WM, m_btnFormatWM);
    DDX_Control(pDX, IDC_FORMAT_RM, m_btnFormatRM);
    DDX_Control(pDX, IDC_FORMAT_IPOD, m_btnFormatIPOD);
    DDX_Control(pDX, IDC_FORMAT_LPD, m_btnFormatLPD);
    DDX_Control(pDX, IDC_FORMAT_CUSTOM, m_btnFormatCustom);
}


BEGIN_MESSAGE_MAP(CPublishingFormatDialog, CDialog)
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateUIMessage)
    ON_WM_PAINT()
    ON_WM_CTLCOLOR()
    ON_COMMAND(IDC_FORMAT_YOUTUBE, OnFormatYouTube)
    ON_UPDATE_COMMAND_UI(IDC_FORMAT_YOUTUBE, OnUpdateFormatYouTube)
    ON_COMMAND(IDC_EDIT_YOUTUBE, OnEditYouTube)
    ON_COMMAND(IDC_FORMAT_SLIDESTAR, OnFormatSlidestar)
    ON_UPDATE_COMMAND_UI(IDC_FORMAT_SLIDESTAR, OnUpdateFormatSlidestar)
    ON_COMMAND(IDC_EDIT_SLIDESTAR, OnEditSlidestar)
    ON_COMMAND(IDC_FORMAT_FLASH, OnFormatFlash)
    ON_UPDATE_COMMAND_UI(IDC_FORMAT_FLASH, OnUpdateFormatFlash)
    ON_COMMAND(IDC_FORMAT_WM, OnFormatWindowsMedia)
    ON_UPDATE_COMMAND_UI(IDC_FORMAT_WM, OnUpdateFormatWindowsMedia)
    ON_COMMAND(IDC_FORMAT_RM, OnFormatRealMedia)
    ON_UPDATE_COMMAND_UI(IDC_FORMAT_RM, OnUpdateFormatRealMedia)
    ON_COMMAND(IDC_FORMAT_IPOD, OnFormatIPod)
    ON_UPDATE_COMMAND_UI(IDC_FORMAT_IPOD, OnUpdateFormatIPod)
    ON_COMMAND(IDC_FORMAT_LPD, OnFormatLPD)
    ON_UPDATE_COMMAND_UI(IDC_FORMAT_LPD, OnUpdateFormatLPD)
    ON_COMMAND(IDC_FORMAT_CUSTOM, OnFormatCustom)
    ON_UPDATE_COMMAND_UI(IDC_FORMAT_CUSTOM, OnUpdateFormatCustom)
    ON_COMMAND(IDC_EDIT_CUSTOM, OnEditCustom)
    ON_COMMAND(IDC_PROFILE_MANAGER, OnProfileManager)
    ON_COMMAND(IDC_CANCEL, OnCancel)
END_MESSAGE_MAP()


// CPublishingFormatDialog-Meldungshandler

BOOL CPublishingFormatDialog::OnInitDialog() { 
    CDialog::OnInitDialog();

    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

    DWORD dwIds[] = {
        IDC_DEFAULT_TITLE,
        IDC_CUSTOM_TITLE,
        IDC_FORMAT_RM,
        IDC_FORMAT_WM,
        IDC_FORMAT_FLASH,
        IDC_FORMAT_IPOD,
        IDC_FORMAT_LPD,
        IDC_CANCEL,
        -1
    };

    MfcUtils::Localize(this, dwIds);

    CString csProfileManager;
    csProfileManager.LoadString(IDS_PROFILE_MANAGER);
    GetDlgItem(IDC_PROFILE_MANAGER)->SetWindowText(csProfileManager);

    CString csTitle;
    csTitle.LoadString(IDS_PUBLISHING_WINDOW_TITLE);
    SetWindowText(csTitle);

    HRESULT hr = ProfileUtils::GetDirectPublishProfile(m_iCurrentSelection);
    if (FAILED(hr))
        m_iCurrentSelection = PublishingFormat::TYPE_LECTURNITY;

    CXTPButton *pBtn = NULL;

    CFont *pFont = XTPPaintManager()->GetRegularFont();
    pBtn = (CXTPButton *)GetDlgItem(IDC_EDIT_YOUTUBE);
    if (pBtn)
        pBtn->SetFont(pFont);
    pBtn = (CXTPButton *)GetDlgItem(IDC_EDIT_SLIDESTAR);
    if (pBtn)
        pBtn->SetFont(pFont);

    m_hbrBg = ::CreateSolidBrush(RGB(255, 255, 255));

    HWND hwndChild = ::GetWindow(GetSafeHwnd(), GW_CHILD);  
    while (hwndChild) {
        CWnd *pWnd = CWnd::FromHandle(hwndChild);
        int nID = pWnd->GetDlgCtrlID();

        UINT nImageId;
        CString csDescription = _T("");
        CString csTitle = _T("");

        if (nID == IDC_FORMAT_YOUTUBE || nID == IDC_FORMAT_SLIDESTAR || 
            nID == IDC_FORMAT_FLASH || nID == IDC_FORMAT_WM || nID == IDC_FORMAT_RM || 
            nID == IDC_FORMAT_IPOD || nID == IDC_FORMAT_LPD) {

            CXTPButton *pButton = (CXTPButton *)GetDlgItem(pWnd->GetDlgCtrlID());
            if (!pButton) 
                continue;

            switch(nID) {
                case IDC_FORMAT_YOUTUBE:
                    nImageId = IDB_YOUTUBE;
                    csDescription.LoadString(IDS_FORMAT_YOUTUBE);
                    ((CProfileButton *)pButton)->CreateEditButton(IDC_EDIT_YOUTUBE);
                    break;
                case IDC_FORMAT_SLIDESTAR:
                    nImageId = IDB_SLIDESTAR;
                    csDescription.LoadString(IDS_FORMAT_SLIDESTAR);
                    ((CProfileButton *)pButton)->CreateEditButton(IDC_EDIT_SLIDESTAR);
                    break;
                case IDC_FORMAT_FLASH:
                    nImageId = IDB_FLASH;
                    csDescription.LoadString(IDS_FORMAT_FLASH);
                    break;
                case IDC_FORMAT_WM:
                    nImageId = IDB_WINDOWSMEDIA;
                    csDescription.LoadString(IDS_FORMAT_WM);
                    break;
                case IDC_FORMAT_RM:
                    nImageId = IDB_REALMEDIA;
                    csDescription.LoadString(IDS_FORMAT_RM);
                    break;
                case IDC_FORMAT_IPOD:
                    nImageId = IDB_IPOD;
                    csDescription.LoadString(IDS_FORMAT_IPOD);
                    break;
                case IDC_FORMAT_LPD:
                    nImageId = IDB_LPD;
                    csDescription.LoadString(IDS_FORMAT_LPD);
                    break;
            }

            pButton->SetFont(pFont);

            pButton->SetBitmap(CSize(48,48), nImageId);
            pButton->SetTheme(xtpButtonThemeOffice2007);
            pButton->SetShowFocus(FALSE);
            pButton->SetFlatStyle(TRUE);
            pButton->SetTransparent(TRUE);

            pButton->SetTextAlignment(BS_VCENTER | BS_LEFT);
            pButton->SetImageAlignment(BS_VCENTER | BS_LEFT);
            pButton->EnableMarkup();

            csTitle.LoadString(nID);
            pButton->SetWindowText(
                _T("<StackPanel Margin='5, 0, 0, 0'>")
                _T("<TextBlock Foreground='#003399'><Bold>")+csTitle+_T("</Bold></TextBlock>")
                _T("<TextBlock TextWrapping='Wrap'>")+csDescription+_T("</TextBlock>")
                _T("</StackPanel>"));
        } else if (nID == IDC_FORMAT_CUSTOM) {
            nImageId = IDB_CUSTOM;
            CProfileButton *pButton = (CProfileButton *)GetDlgItem(pWnd->GetDlgCtrlID());
            pButton->SetFont(pFont);

            pButton->SetBitmap(CSize(48,48), nImageId);
            pButton->SetTheme(xtpButtonThemeOffice2007);
            pButton->SetShowFocus(FALSE);
            pButton->SetFlatStyle(TRUE);
            pButton->SetTransparent(TRUE);

            pButton->SetTextAlignment(BS_VCENTER | BS_LEFT);
            pButton->SetImageAlignment(BS_VCENTER | BS_LEFT);
            pButton->EnableMarkup();

            pButton->SetWindowText(_T(""));

            pButton->CreateProfileCombo();
            pButton->CreateEditButton(IDC_EDIT_CUSTOM);
        }

        hwndChild = ::GetWindow( hwndChild, GW_HWNDNEXT);   
    }

    m_captionDefault.SetTitleAlign(xtpTitleAlignLeft);
    m_captionDefault.SetCaptionType(xtpCaptionNormal);
    m_captionDefault.SubclassDlgItem(IDC_DEFAULT_TITLE, this);

    m_captionCustom.SetTitleAlign(xtpTitleAlignLeft);
    m_captionCustom.SetCaptionType(xtpCaptionNormal);
    m_captionCustom.SubclassDlgItem(IDC_CUSTOM_TITLE, this);

    return TRUE; 
}

void CPublishingFormatDialog::OnIdleUpdateUIMessage() {
	UpdateDialogControls( this, TRUE );
}

BOOL CPublishingFormatDialog::ContinueModal() {
    SendMessage(WM_IDLEUPDATECMDUI, WPARAM(TRUE), 0);

    return CDialog::ContinueModal();
}

void CPublishingFormatDialog::OnPaint() {
    CPaintDC dc(this); 

    CRect rcDialog;
    GetClientRect(&rcDialog);

    CRect rcTitleBg(0, 0, rcDialog.Width(), 50);
    CBrush whiteBrush;
    whiteBrush.CreateSolidBrush(RGB(255, 255, 255));
    dc.FillRect(rcTitleBg, &whiteBrush);
    whiteBrush.DeleteObject();

    CFont *pWindowFont = XTPPaintManager()->GetRegularFont();
    if (pWindowFont != NULL) {
        LOGFONT lf;
        pWindowFont->GetLogFont(&lf);
        HFONT hFontNew, hFontOld;
        lf.lfHeight = 18;
        hFontNew = CreateFontIndirect(&lf);
        if (hFontNew != NULL ) {
            hFontOld = (HFONT) SelectObject(dc.m_hDC, hFontNew);
            CRect rcTitle(8, 8, rcDialog.Width(), 48);
            CString csTitle;
            csTitle.LoadString(IDS_PUBLISHINGTITLE);

            COLORREF oldColor = dc.SetTextColor(RGB(0, 51, 153));
            dc.DrawText(csTitle, csTitle.GetLength(), rcTitle, DT_LEFT);

            if (hFontOld != NULL) {
                SelectObject(dc.m_hDC, hFontOld);
                DeleteObject(hFontNew);
            }
            dc.SetTextColor(oldColor);
        }

        lf.lfHeight = 14;
        hFontNew = CreateFontIndirect(&lf);
        if (hFontNew != NULL ) {
            hFontOld = (HFONT) SelectObject(dc.m_hDC, hFontNew);

            CRect rcSubTitle(8, 45, rcDialog.Width(), 60);
            CString csSubTitle;
            csSubTitle.LoadString(IDS_PUBLISHINGSUBTITLE);
            COLORREF oldColor = dc.SetTextColor(RGB(0, 0, 0));
            dc.DrawText(csSubTitle, csSubTitle.GetLength(), rcSubTitle, DT_LEFT);

            if (hFontOld != NULL) {
                SelectObject(dc.m_hDC, hFontOld);
                DeleteObject(hFontNew);
            }
            dc.SetTextColor(oldColor);

        }
        else
            DeleteObject(hFontNew);
    }
}

void CPublishingFormatDialog::OnOK() {
    // Set activated profile
    if (m_iCurrentSelection != PublishingFormat::TYPE_NOTHING)
        ProfileUtils::SetDirectPublishProfile(m_iCurrentSelection);

    // Write custom profiles
    CArray<ProfileInfo *, ProfileInfo *> aProfileInfo;
    m_btnFormatCustom.GetProfileList(aProfileInfo);
    for (int i = 0; i < aProfileInfo.GetSize(); ++i) {
        if (aProfileInfo[i] != NULL)
            aProfileInfo[i]->Write();
    }
    
    // Write active profile to registry 
    // Delete active profile entry
    HRESULT hr = ProfileUtils::ResetActiveProfile();

    for (int i = 0; i < aProfileInfo.GetCount(); ++i) {
        if (aProfileInfo[i] != NULL && aProfileInfo[i]->IsActivated()) {
            // Save active profile in registry
            hr = ProfileUtils::WriteActiveProfile(aProfileInfo[i]->GetFilename());
        }
    }

    // Update recent profiles
    hr = ProfileUtils::AddProfileToRecentList(m_iCurrentSelection);

    CDialog::OnOK();
}

HBRUSH CPublishingFormatDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if (nCtlColor == CTLCOLOR_BTN ||
        nCtlColor == CTLCOLOR_DLG  ||
        nCtlColor == CTLCOLOR_STATIC || true) {
            SetBkColor(pDC->GetSafeHdc(), RGB(255, 255, 255));
            SetBkMode(pDC->GetSafeHdc(), OPAQUE);
            return m_hbrBg;
    }

    return hbr;
}

void CPublishingFormatDialog::OnFormatYouTube() {
    m_iCurrentSelection = PublishingFormat::TYPE_YOUTUBE;
}

void CPublishingFormatDialog::OnUpdateFormatYouTube(CCmdUI *pCmdUI) {
    BOOL bChecked = FALSE;

    if (m_iCurrentSelection == PublishingFormat::TYPE_YOUTUBE)
        bChecked = TRUE;

    m_btnFormatYouTube.SetChecked(bChecked);
    pCmdUI->SetCheck(bChecked);
}

void CPublishingFormatDialog::OnEditYouTube() {
    CYouTubeSettings dlgYouTubeSettings;

    dlgYouTubeSettings.DoModal();

    // The mouse over effect should be deleted.
    m_btnFormatYouTube.OnMouseMove(0, CPoint(-1, -1));
}

void CPublishingFormatDialog::OnFormatSlidestar() {
    m_iCurrentSelection = PublishingFormat::TYPE_SLIDESTAR;
}

void CPublishingFormatDialog::OnUpdateFormatSlidestar(CCmdUI *pCmdUI) {
    BOOL bChecked = FALSE;

    if (m_iCurrentSelection == PublishingFormat::TYPE_SLIDESTAR)
        bChecked = TRUE;

    m_btnFormatSlidestar.SetChecked(bChecked);
    pCmdUI->SetCheck(bChecked);
}

void CPublishingFormatDialog::OnEditSlidestar() {
    CSlidestarSettings dlgSlidestarSettings;

    dlgSlidestarSettings.DoModal();

    // The mouse over effect should be deleted.
    m_btnFormatSlidestar.OnMouseMove(0, CPoint(-1, -1));
}

void CPublishingFormatDialog::OnFormatFlash() {
    m_iCurrentSelection = PublishingFormat::TYPE_FLASH;
}

void CPublishingFormatDialog::OnUpdateFormatFlash(CCmdUI *pCmdUI) {
    BOOL bChecked = FALSE;

    if (m_iCurrentSelection == PublishingFormat::TYPE_FLASH)
        bChecked = TRUE;

    m_btnFormatFlash.SetChecked(bChecked);
    pCmdUI->SetCheck(bChecked);
}

void CPublishingFormatDialog::OnFormatWindowsMedia() {
    m_iCurrentSelection = PublishingFormat::TYPE_WINDOWSMEDIA;
}

void CPublishingFormatDialog::OnUpdateFormatWindowsMedia(CCmdUI *pCmdUI) {
    BOOL bChecked = FALSE;

    if (m_iCurrentSelection == PublishingFormat::TYPE_WINDOWSMEDIA)
        bChecked = TRUE;

    m_btnFormatWM.SetChecked(bChecked);
    pCmdUI->SetCheck(bChecked);
}

void CPublishingFormatDialog::OnFormatRealMedia() {
    m_iCurrentSelection = PublishingFormat::TYPE_REALMEDIA;
}

void CPublishingFormatDialog::OnUpdateFormatRealMedia(CCmdUI *pCmdUI) {
    BOOL bChecked = FALSE;

    if (m_iCurrentSelection == PublishingFormat::TYPE_REALMEDIA)
        bChecked = TRUE;

    m_btnFormatRM.SetChecked(bChecked);
    pCmdUI->SetCheck(bChecked);
}

void CPublishingFormatDialog::OnFormatIPod() {
    m_iCurrentSelection = PublishingFormat::TYPE_IPOD;
}

void CPublishingFormatDialog::OnUpdateFormatIPod(CCmdUI *pCmdUI) {
    BOOL bChecked = FALSE;

    if (m_iCurrentSelection == PublishingFormat::TYPE_IPOD)
        bChecked = TRUE;

    m_btnFormatIPOD.SetChecked(bChecked);
    pCmdUI->SetCheck(bChecked);
}

void CPublishingFormatDialog::OnFormatLPD() {
    m_iCurrentSelection = PublishingFormat::TYPE_LECTURNITY;
}

void CPublishingFormatDialog::OnUpdateFormatLPD(CCmdUI *pCmdUI) {
    BOOL bChecked = FALSE;

    if (m_iCurrentSelection == PublishingFormat::TYPE_LECTURNITY)
        bChecked = TRUE;

    m_btnFormatLPD.SetChecked(bChecked);
    pCmdUI->SetCheck(bChecked);
}

void CPublishingFormatDialog::OnFormatCustom() {
    m_iCurrentSelection = PublishingFormat::TYPE_CUSTOM;
}

void CPublishingFormatDialog::OnUpdateFormatCustom(CCmdUI *pCmdUI) {
    BOOL bChecked = FALSE;

    if (m_iCurrentSelection == PublishingFormat::TYPE_CUSTOM)
        bChecked = TRUE;

    m_btnFormatCustom.SetChecked(bChecked);
    pCmdUI->SetCheck(bChecked);
    
    // Edit button should be disabled if no profile is selected
    BOOL bEnable = FALSE;
    ProfileInfo *pProfile = m_btnFormatCustom.GetSelectedProfile();
    if (pProfile)
        bEnable = TRUE;

    m_btnFormatCustom.EnableEditButton(bEnable);
}

void CPublishingFormatDialog::OnEditCustom() {
    ProfileInfo *pProfile = m_btnFormatCustom.GetSelectedProfile();
    if (pProfile) {
        pProfile->Edit(this, m_csLrdName);

        ::EnableWindow(GetSafeHwnd(), FALSE);

        HRESULT hr = ProfileUtils::WaitForProfile(this);
    }
}

void CPublishingFormatDialog::OnProfileManager() {
    // Read Profiles

    CArray<ProfileInfo *, ProfileInfo *> aProfileInfo;
    m_btnFormatCustom.GetProfileList(aProfileInfo);
    
    m_pProfileManager = new CProfileManager(aProfileInfo, m_csLrdName, this);

    aProfileInfo.RemoveAll();

    if (m_pProfileManager) {
        m_pProfileManager->DoModal();
        // There may be changes on written profile
        m_pProfileManager->GetProfileInformation(aProfileInfo);
        m_btnFormatCustom.SetProfileList(aProfileInfo);
        aProfileInfo.RemoveAll();
    }

    if (m_pProfileManager)
        delete m_pProfileManager;
    m_pProfileManager = NULL;
}

void CPublishingFormatDialog::OnCancel() {
    m_iCurrentSelection = PublishingFormat::TYPE_NOTHING;

    CDialog::OnCancel();
}

void CPublishingFormatDialog::PublisherFinished(CString csPublisherMessage) {
    bool bCancel = true;
    __int64 iProfileID = 0;

    CLanguageSupport::SetThreadLanguage();

    HRESULT hr = 
        ProfileUtils::ParseProfileMessage(csPublisherMessage, bCancel, iProfileID);

    if (!bCancel)
        m_btnFormatCustom.UpdateProfileCombo();

    if (SUCCEEDED(hr)) {
        ::EnableWindow(GetSafeHwnd(), TRUE);
        SetForegroundWindow();
        // The mouse over effect should be deleted.
        m_btnFormatCustom.OnMouseMove(0, CPoint(-1, -1));
    }
}