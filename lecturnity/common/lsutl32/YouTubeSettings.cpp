//#include "YouTubeSettings.h"
//#include "YouTubeSettings.h"
// DlgKeywords.cpp : implementation file
//

#include "stdafx.h"
#include "YouTubeSettings.h"
#include "MfcUtils.h"
#include "Crypto.h"

#include "lutils.h"     // lutils
#include "UploaderUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CYouTubeSettings dialog


BEGIN_MESSAGE_MAP(CYouTubeSettings, CDialog)
	//{{AFX_MSG_MAP(CYouTubeSettings)
    ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, OnOK)
END_MESSAGE_MAP()


CYouTubeSettings::CYouTubeSettings(CWnd* pParent) : CDialog(CYouTubeSettings::IDD, pParent) {
    //{{AFX_DATA_INIT(CYouTubeSettings)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    _tprintf(_T("!!!LSUTL32 CYouTubeSettings const\n"));
    m_bDefaultProfile = true;
    m_hbrBg = NULL;
    InitCategories();
}

CYouTubeSettings::CYouTubeSettings(YouTubeTransferSettings ytts, CWnd* pParent): CDialog(CYouTubeSettings::IDD, pParent) {
    m_bDefaultProfile = false;
    m_ytts = ytts;
    InitCategories();
    LoadErrorStrings();
}

CYouTubeSettings::~CYouTubeSettings() {
    if (m_hbrBg)
        ::DeleteObject(m_hbrBg);
}

void CYouTubeSettings::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_EDIT_USERNAME, m_edtUserName);
    DDX_Control(pDX, IDC_EDIT_PASSWORD, m_edtPassword);
    DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_edtDescription);
    DDX_Control(pDX, IDC_COMBO_CATEGORY, m_cmboCategory);
    DDX_Control(pDX, IDC_CHECK_PRIVACY, m_btnCheckPrivacy);
    DDX_Control(pDX, IDC_EDIT_YT_KEYWORDS, m_edtKeywords);
    DDX_Control(pDX, IDC_EDIT_YT_TITLE, m_edtTitle);
    DDX_Control(pDX, IDC_TITLE_YT, m_lblTitle);
    DDX_Control(pDX, IDC_KEYWORDS, m_lblKeywords);
    DDX_Control(pDX, IDC_DESCRIPTION, m_lblDescription);
    DDX_Control(pDX, IDC_CATEGORY, m_lblCategory);
    DDX_Control(pDX, IDC_PRIVACY, m_lblPrivacy);
    DDX_Control(pDX, IDOK, m_btnOK);
    DDX_Control(pDX, IDCANCEL, m_btnCancel);
    DDX_Control(pDX, IDC_TYDLG_SEPARATOR, m_cstSeparator);
}


BOOL CYouTubeSettings::OnInitDialog() { 
    CDialog::OnInitDialog();

    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

    DWORD dwIds[] = {
        IDC_USERNAME,
        IDC_PASSWORD,
        IDC_TITLE_YT, 
        IDC_DESCRIPTION,
        IDC_KEYWORDS,
        IDC_CATEGORY,
        IDC_PRIVACY,
        IDOK,
        IDCANCEL,
        -1
    };

    MfcUtils::Localize(this, dwIds);

    CString csTitle;
    csTitle.LoadString(IDS_YOUTUBE_DIALOG);
    SetWindowText(csTitle);

    m_hbrBg = ::CreateSolidBrush(RGB(255, 255, 255));
    
    
    /*m_cmboCategory.AddString(_T("Education"));
    m_cmboCategory.AddString(_T("Film"));
    m_cmboCategory.AddString(_T("Autos"));
    m_cmboCategory.AddString(_T("Music"));
    m_cmboCategory.AddString(_T("Animals"));
    m_cmboCategory.AddString(_T("Sports"));
    m_cmboCategory.AddString(_T("Travel"));
    m_cmboCategory.AddString(_T("Shortmov"));
    m_cmboCategory.AddString(_T("Videoblog"));
    m_cmboCategory.AddString(_T("Games"));
    m_cmboCategory.AddString(_T("Comedy"));
    m_cmboCategory.AddString(_T("People"));
    m_cmboCategory.AddString(_T("News"));
    m_cmboCategory.AddString(_T("Entertainment"));
    m_cmboCategory.AddString(_T("Howto"));
    m_cmboCategory.AddString(_T("Nonprofit"));
    m_cmboCategory.AddString(_T("Tech"));*/
    for(int i = 0; i < m_acsCategories.GetSize(); i++){
        CString csCatValue = GetLocalizedCategory(m_acsCategories.GetAt(i));
        m_cmboCategory.AddString(csCatValue);
    }

    if(m_bDefaultProfile){
        SetDefaultProfileDialog();
        ReadDefaultSettings();
    } else {
        //SetDefaultProfileDialog();
        SetFullDialogValues();
        
    }

    return TRUE; 
}

HBRUSH CYouTubeSettings::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
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

void CYouTubeSettings::OnOK() {

    if(m_bDefaultProfile){
        WriteDefaultSettings();
    } else {
        m_edtUserName.GetWindowText(m_ytts.csUser);
        m_edtPassword.GetWindowText(m_ytts.csPassword);
        m_edtTitle.GetWindowText(m_ytts.csTitle);
        m_edtDescription.GetWindowText(m_ytts.csDescription);
        m_edtKeywords.GetWindowText(m_ytts.csKeywords);
        m_ytts.csCategory = m_acsCategories.GetAt(m_cmboCategory.GetCurSel());
        m_ytts.iPrivacy = m_btnCheckPrivacy.GetCheck();
        //m_ytts.csUser
        bool bCheck = CheckYouTubeRequiredParameters(m_ytts.csTitle, m_ytts.csDescription, m_ytts.csKeywords);
        if(!bCheck) {
            MessageBox(m_csCheckError, _T("Publisher"), MB_OK | MB_ICONSTOP | MB_SETFOREGROUND | MB_TOPMOST | MB_TASKMODAL);
            return;
        }
    }
    CDialog::OnOK();
}

HRESULT CYouTubeSettings::ReadDefaultSettings() {
    HRESULT hr = S_OK;

    bool bSuccess = true;

    _TCHAR tszRegistryEntry[MAX_PATH];
    unsigned long lLength = MAX_PATH;

    // User name
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default"), 
        _T("UserName"), tszRegistryEntry, &lLength);
    if (bSuccess)
        m_edtUserName.SetWindowText(tszRegistryEntry);
    else
        m_edtUserName.SetWindowText(_T(""));
    
    // Password
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default"), 
        _T("Password"), tszRegistryEntry, &lLength);
    if (bSuccess) {
        CCrypto crypt;
        CString csDecodedPassword = crypt.Decode(tszRegistryEntry);
        m_edtPassword.SetWindowText(csDecodedPassword);
    } else
        m_edtPassword.SetWindowText(_T(""));
    
    // Description
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default"), 
        _T("Description"), tszRegistryEntry, &lLength);
    if (bSuccess)
        m_edtDescription.SetWindowText(tszRegistryEntry);
    else
        m_edtDescription.SetWindowText(_T(""));
    
    // Category
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default"), 
        _T("Category"), tszRegistryEntry, &lLength);
    CString csCategory;
    bool bFound = false;
    if (bSuccess) {
        csCategory = tszRegistryEntry;
        //for (int i = 0; i < m_cmboCategory.GetCount() && !bFound; ++i) {
        for (int i = 0; i < m_acsCategories.GetCount() && !bFound; ++i) {
            if (csCategory.CompareNoCase(m_acsCategories.GetAt(i)) == 0) {
                m_cmboCategory.SetCurSel(i);
                bFound = true;
            }
        }
    }

    if (!bFound) {
        for (int i = 0; i < m_acsCategories.GetCount() && !bFound; ++i) {
            if (m_acsCategories.GetAt(i).CompareNoCase( _T("Education")) == 0) {
                m_cmboCategory.SetCurSel(i);
                bFound = true;
            }
        }
    }

    DWORD dwPrivacy;
    bSuccess = LRegistry::ReadDwordRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default"), 
        _T("Privacy"), &dwPrivacy);
    if (bSuccess)
        m_btnCheckPrivacy.SetCheck(dwPrivacy == 0 ? FALSE : TRUE);
    else
        m_btnCheckPrivacy.SetCheck(TRUE);
    // TODO set checkbox


    return hr;
}

HRESULT CYouTubeSettings::WriteDefaultSettings() {
    HRESULT hr = S_OK;

    bool bSuccess = true;
    
    CString csUserName;
    m_edtUserName.GetWindowText(csUserName);
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default"),
        _T("UserName"), csUserName);

    CString csPassword;
    m_edtPassword.GetWindowText(csPassword);
    CCrypto crypt;
    CString csEncodedPassword = crypt.Encode(csPassword);
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default"),
        _T("Password"), csEncodedPassword);
    
    CString csDescription;
    m_edtDescription.GetWindowText(csDescription);
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default"),
        _T("Description"), csDescription);
    
    
    CString csCategory;
    int iCurSelection = m_cmboCategory.GetCurSel();
    if (iCurSelection >= 0)
        csCategory = m_acsCategories.GetAt(iCurSelection);
        /*m_cmboCategory.GetLBText(iCurSelection, csCategory);*/
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default"),
        _T("Category"), csCategory);

    DWORD dwIsPrivate = m_btnCheckPrivacy.GetCheck() == 0 ? 0 : 1;
    bSuccess = LRegistry::WriteDwordRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Publisher\\Profiles\\YouTube_Default"),
        _T("Privacy"), dwIsPrivate);


    return hr;
}

void CYouTubeSettings::SetFullDialogValues(void) {
    m_edtUserName.SetWindowText(m_ytts.csUser);
    m_edtPassword.SetWindowText(m_ytts.csPassword);
    m_edtTitle.SetWindowText(m_ytts.csTitle);
    m_edtDescription.SetWindowText(m_ytts.csDescription);
    m_edtKeywords.SetWindowText(m_ytts.csKeywords);
    m_cmboCategory.SetCurSel(GetCategoryIndex(m_ytts.csCategory));
    m_btnCheckPrivacy.SetCheck(m_ytts.iPrivacy == 0 ? 0 : 1);
}

void CYouTubeSettings::SetDefaultProfileDialog(void) {
    CRect rcClient;
    GetWindowRect(rcClient);
    CRect rcLblElement;
    CRect rcCtrlElement;
   
    m_lblTitle.ShowWindow(SW_HIDE);
    m_edtTitle.ShowWindow(SW_HIDE);

    m_lblKeywords.ShowWindow(SW_HIDE);
    m_edtKeywords.ShowWindow(SW_HIDE);

    int iOffset = 30;

    m_lblDescription.GetWindowRect(rcLblElement);
    ScreenToClient(&rcLblElement);
    m_lblDescription.SetWindowPos(NULL, rcLblElement.left, rcLblElement.top - iOffset, rcLblElement.Width(), rcLblElement.Height(), SWP_NOZORDER);
    m_edtDescription.GetWindowRect(rcCtrlElement);
    ScreenToClient(&rcCtrlElement);
    m_edtDescription.SetWindowPos(NULL, rcCtrlElement.left, rcCtrlElement.top - iOffset, rcCtrlElement.Width(), rcCtrlElement.Height(), SWP_NOZORDER);

    iOffset += 30;
    m_lblCategory.GetWindowRect(rcLblElement);
    ScreenToClient(&rcLblElement);
    m_lblCategory.SetWindowPos(NULL, rcLblElement.left, rcLblElement.top - iOffset, rcLblElement.Width(), rcLblElement.Height(), SWP_NOZORDER);
    m_cmboCategory.GetWindowRect(rcCtrlElement);
    ScreenToClient(&rcCtrlElement);
    m_cmboCategory.SetWindowPos(NULL, rcCtrlElement.left, rcCtrlElement.top - iOffset, rcCtrlElement.Width(), rcCtrlElement.Height(), SWP_NOZORDER);

    m_lblPrivacy.GetWindowRect(rcLblElement);
    ScreenToClient(&rcLblElement);
    m_lblPrivacy.SetWindowPos(NULL, rcLblElement.left, rcLblElement.top - iOffset, rcLblElement.Width(), rcLblElement.Height(), SWP_NOZORDER);
    m_btnCheckPrivacy.GetWindowRect(rcCtrlElement);
    ScreenToClient(&rcCtrlElement);
    m_btnCheckPrivacy.SetWindowPos(NULL, rcCtrlElement.left, rcCtrlElement.top - iOffset, rcCtrlElement.Width(), rcCtrlElement.Height(), SWP_NOZORDER);

    m_cstSeparator.GetWindowRect(rcLblElement);
    ScreenToClient(&rcLblElement);
    m_cstSeparator.SetWindowPos(NULL, rcLblElement.left, rcLblElement.top - iOffset, rcLblElement.Width(), rcLblElement.Height(), SWP_NOZORDER);

    m_btnOK.GetWindowRect(rcCtrlElement);
    ScreenToClient(&rcCtrlElement);
    m_btnOK.SetWindowPos(NULL, rcCtrlElement.left, rcCtrlElement.top - iOffset, rcCtrlElement.Width(), rcCtrlElement.Height(), SWP_NOZORDER);

    m_btnCancel.GetWindowRect(rcCtrlElement);
    ScreenToClient(&rcCtrlElement);
    m_btnCancel.SetWindowPos(NULL, rcCtrlElement.left, rcCtrlElement.top - iOffset, rcCtrlElement.Width(), rcCtrlElement.Height(), SWP_NOZORDER);

    MoveWindow(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height() - iOffset);

    CenterWindow();
}

bool CYouTubeSettings::CheckYouTubeRequiredParameters(CString csTitle, CString csDescription, CString csKeywords) {
    bool bRes = true;
    
    m_csCheckError.Empty();
    bool bTitle = CheckYouTubeTitle(csTitle);

    bool bDescription = CheckYouTubeDescription(csDescription);

    bool bKeywords = CheckYouTubeKeywords(csKeywords);

    bRes = bTitle && bDescription && bKeywords;

    return bRes;
}

void CYouTubeSettings::LoadErrorStrings() {
    m_csCharErrorFormat.LoadString(IDS_YTTS_ILLEGAL_CHARACTERS);;
    m_csTooLongFormat.LoadString(IDS_YTTS_TOO_LONG);
    m_csEmptyFormat.LoadString(IDS_YTTS_EMPTY);
    m_csTooShortFormat.LoadString(IDS_YTTS_TOO_SHORT);
    _tprintf(_T("!!!!LSU m_csCharErrorFormat: %s\n"), m_csCharErrorFormat);

}

bool CYouTubeSettings::CheckYouTubeTitle(CString csTitle) {
    bool bRetVal = true;
    CString csTitleStr;
    csTitleStr.LoadString(IDS_YTTS_TITLE);
       
    if(csTitle.IsEmpty()) {
        CString csEmpty;
        csEmpty.Format(m_csEmptyFormat, csTitleStr);
        m_csCheckError += csEmpty;
        bRetVal = false;
    }
    if(!CheckCharacters(csTitle)) {
        CString csCharError;
        csCharError.Format(m_csCharErrorFormat, csTitleStr);
        m_csCheckError += csCharError;
        bRetVal = false;
    }

    int iTitleByteLength = csTitle.GetLength() * sizeof(TCHAR);
    if(iTitleByteLength > MAX_YTT_TITLE_BYTES || csTitle.GetLength() > MAX_YTT_TITLE_LENGTH) {
        CString csTooLongErr;
        CString csErrCharsFormat;
        csErrCharsFormat.LoadString(IDS_YTTS_TITLE_CHARS);
        CString csErrChars;
        csErrChars.Format(csErrCharsFormat, MAX_YTT_TITLE_BYTES);
        csTooLongErr.Format(m_csTooLongFormat, csTitleStr, csTitleStr, MAX_YTT_TITLE_LENGTH, csErrChars);
        m_csCheckError += csTooLongErr;
        bRetVal = false;
    }
    return bRetVal;
}
 
bool CYouTubeSettings::CheckYouTubeDescription(CString csDescription)
{
    bool bRetVal = true;
    CString csDescriptionStr;
    csDescriptionStr.LoadString(IDS_YTTS_DESCRIPTION); 
    if(csDescription.IsEmpty()) {
        CString csEmpty;
        csEmpty.Format(m_csEmptyFormat, csDescriptionStr);
        m_csCheckError += csEmpty;
        bRetVal = false;
    }
    if(!CheckCharacters(csDescription)) {
        CString csCharError;
        csCharError.Format(m_csCharErrorFormat, csDescriptionStr);
        m_csCheckError += csCharError;
        bRetVal = false;
    }
    if(csDescription.GetLength() > MAX_YTT_DESCRIPTION_LENGTH) {
        CString csTooLongErr;
        csTooLongErr.Format(m_csTooLongFormat, csDescriptionStr, csDescriptionStr, MAX_YTT_DESCRIPTION_LENGTH, _T(""));
        m_csCheckError += csTooLongErr;
        bRetVal = false;
    }
    return bRetVal;
}
 
bool CYouTubeSettings::CheckYouTubeKeywords(CString csKeywords)
{
    bool bRetVal = true;
    if(!CheckCharacters(csKeywords)) {
        CString csKeywordsStr;
        csKeywordsStr.LoadString(IDS_YTTS_KEYWORD);
        CString csCharError;
        csCharError.Format(m_csCharErrorFormat, csKeywordsStr);
        m_csCheckError += csCharError;
        bRetVal = false;
    }

    CString csKeywordError = _T("");
    bool bKeywordsLength = CheckKeyworsLength(csKeywords, csKeywordError);
    if(bKeywordsLength == false){
        m_csCheckError += csKeywordError;
        bRetVal = false;
    }
    return bRetVal;
}

bool CYouTubeSettings::CheckCharacters(CString csMediaElement)
{
    bool bRetVal = true;

    if(csMediaElement.Find(_T("<")) != -1 || csMediaElement.Find(_T(">")) != -1) {
        bRetVal = false;
    }
    return bRetVal;
}

bool CYouTubeSettings::CheckKeyworsLength(CString csKeywords, CString &csKeywordError)
{
   /* CString csTooLongFormat;
    csTooLongFormat.LoadString(IDS_YTTS_TOO_LONG);
    CString csTooShortFormat;
    csTooShortFormat.LoadString(IDS_YTTS_TOO_SHORT);*/
    CString csKeywordNumberFormat;
    csKeywordNumberFormat.LoadString(IDS_YTTS_KEYWORD_NUMBER);
    CString csKeywordStr;
    csKeywordStr.LoadString(IDS_YTTS_KEYWORD);
    CString csTooLongErr;
    CString csTooShortErr;
    CString csKeywordNumber;
    bool bRetVal = true;

    if(csKeywords.IsEmpty()) {
        CString csEmptyFormat;
        csEmptyFormat.LoadString(IDS_YTTS_EMPTY);
        CString csEmpty;
        csEmpty.Format(csEmptyFormat, csKeywordStr);
        csKeywordError += csEmpty;
        return false;
    }


    CArray<CString, CString>aKeywords;
    int iPos = 0;
    if(csKeywords.Find(_T(",")) != -1) {
        CString csTok = csKeywords.Tokenize(_T(","), iPos);
        while(csTok != _T("")) {
            aKeywords.Add(csTok);
            csTok = csKeywords.Tokenize(_T(","), iPos);
        }
    } else {
        aKeywords.Add(csKeywords);
    }

    for(int i=0; i < aKeywords.GetCount(); i++) {
        csKeywordNumber.Format(csKeywordNumberFormat, i + 1);
        if(aKeywords.GetAt(i).GetLength() < MIN_YTT_KEYWORD_LENGTH) {
            csTooShortErr.Format(m_csTooShortFormat, csKeywordNumber, csKeywordStr, MIN_YTT_KEYWORD_LENGTH);
            csKeywordError += csTooShortErr;
        }
        if(aKeywords.GetAt(i).GetLength() > MAX_YTT_KEYWORD_LENGTH) {
            csTooLongErr.Format(m_csTooLongFormat, csKeywordNumber, csKeywordStr, MAX_YTT_KEYWORD_LENGTH, _T(""));
            csKeywordError += csTooLongErr;
        }
    }

    if(!csKeywordError.IsEmpty()) {
        bRetVal = false;
    }
    return bRetVal;
}

void CYouTubeSettings::InitCategories(){
    m_acsCategories.RemoveAll();
    m_acsCategories.Add(_T("Autos"));
    m_acsCategories.Add(_T("Comedy"));
    m_acsCategories.Add(_T("Education"));
    m_acsCategories.Add(_T("Entertainment"));
    m_acsCategories.Add(_T("Film"));
    m_acsCategories.Add(_T("Games"));
    m_acsCategories.Add(_T("Howto"));
    m_acsCategories.Add(_T("Music"));
    m_acsCategories.Add(_T("News"));
    m_acsCategories.Add(_T("Nonprofit"));
    m_acsCategories.Add(_T("People"));
    m_acsCategories.Add(_T("Animals"));
    m_acsCategories.Add(_T("Tech"));
    m_acsCategories.Add(_T("Sports"));
    m_acsCategories.Add(_T("Travel"));
}

int CYouTubeSettings::GetCategoryIndex(CString csCategory){
    int iRetIndex = 0;
    for(int i = 0; i < m_acsCategories.GetSize(); i++){
        if(csCategory.CompareNoCase(m_acsCategories.GetAt(i)) == 0){
            iRetIndex = i;
            break;
        }
    }
    return iRetIndex;
}

CString CYouTubeSettings::GetLocalizedCategory(CString csCategory){
    CString csRetVal;
    if(csCategory.CompareNoCase(_T("Autos")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_AUTOS);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("Comedy")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_COMEDY);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("Education")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_EDUCATION);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("Entertainment")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_ENTERTAINTMENT);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("Film")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_FILM);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("Games")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_GAMES);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("Howto")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_HOWTO);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("Music")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_MUSIC);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("News")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_NEWS);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("Nonprofit")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_NONPROFIT);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("People")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_PEOPLE);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("Animals")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_ANIMALS);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("Tech")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_TECH);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("Sports")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_SPORTS);
        return csRetVal;
    }
    if(csCategory.CompareNoCase(_T("Travel")) == 0){
        csRetVal.LoadString(IDS_YT_CAT_TRAVEL);
        return csRetVal;
    }
    return _T("");
}