// ProfileManager.cpp: Implementierungsdatei
//

#include "stdafx.h"

#include "ProfileManager.h"
#include "ProfileNameDialog.h"
#include "ProfileUtils.h"
#include "MfcUtils.h"
#include "lutils.h"
#include "MiscFunctions.h"
#include "JNISupport.h"
#include "LanguageSupport.h"

#undef SubclassWindow

// CProfileManager-Dialogfeld

IMPLEMENT_DYNAMIC(CProfileManager, CDialog)

BEGIN_MESSAGE_MAP(CProfileManager, CDialog)
    ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateUIMessage)
    ON_WM_CTLCOLOR()

    ON_BN_CLICKED(IDC_NEW_PROFILE, &CProfileManager::OnNewProfile)
    ON_BN_CLICKED(IDC_EDIT_PROFILE, &CProfileManager::OnEditProfile)
    ON_BN_CLICKED(IDC_RENAME_PROFILE, &CProfileManager::OnRenameProfile)
    ON_BN_CLICKED(IDC_DELETE_PROFILE, &CProfileManager::OnDeleteProfile)
    ON_BN_CLICKED(IDC_IMPORT_PROFILE, &CProfileManager::OnImportProfile)
    ON_BN_CLICKED(IDC_EXPORT_PROFILE, &CProfileManager::OnExportProfile)

    ON_UPDATE_COMMAND_UI(IDC_EDIT_PROFILE, OnSelectionEnableButtons)
    ON_UPDATE_COMMAND_UI(IDC_RENAME_PROFILE, OnSelectionEnableButtons)
    ON_UPDATE_COMMAND_UI(IDC_DELETE_PROFILE, OnSelectionEnableButtons)
    ON_UPDATE_COMMAND_UI(IDC_EXPORT_PROFILE, OnSelectionEnableButtons)

    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

CProfileManager::CProfileManager(CArray<ProfileInfo *, ProfileInfo *> &aProfileInformation,
                                 CString &csLrdFilename, CWnd* pParent /*=NULL*/)
                                 : CDialog(CProfileManager::IDD, pParent) {
    m_bWaitForNewProfile = false;
    m_bWaitForChangedProfile = false;

    for (int i = 0; i < aProfileInformation.GetSize(); ++i) {
        if (aProfileInformation[i]) {
            ProfileInfo *pProfile = new ProfileInfo(aProfileInformation[i]);
            m_aProfileInformation.Add(pProfile);
        }
    }

    m_csLrdFilename = csLrdFilename;
    
    m_hbrBg = NULL;
    m_hPipe = 0;
}

CProfileManager::~CProfileManager() {
    for (int i = 0; i < m_aProfileInformation.GetCount(); ++i) {
        if (m_aProfileInformation[i])
            delete m_aProfileInformation[i];
    }
    m_aProfileInformation.RemoveAll();

    if (m_hbrBg)
        ::DeleteObject(m_hbrBg);

    if (m_hPipe)
        CloseHandle(m_hPipe);
}

// CProfileManager-Meldungshandler

BOOL CProfileManager::OnInitDialog() {
    CDialog::OnInitDialog();

    CString csCaption;
    csCaption.LoadString(IDS_PROFILE_MANAGER);
    SetWindowText(csCaption);

    DWORD dwIds[] = {IDC_NEW_PROFILE,
                     IDC_EDIT_PROFILE,
                     IDC_RENAME_PROFILE,
                     IDC_DELETE_PROFILE,
                     IDC_IMPORT_PROFILE,
                     IDC_EXPORT_PROFILE,
                     IDOK,
                     IDCANCEL,
                     -1};

    MfcUtils::Localize(this, dwIds);

    m_hbrBg = ::CreateSolidBrush(RGB(255, 255, 255));

    m_profileList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    CRect listRect;
    m_profileList.GetClientRect(&listRect);

    LV_COLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;

    CString csString;

    csString.LoadString(IDS_NAME_CAPTION);
    lvc.pszText = csString.GetBuffer(csString.GetLength());
    csString.ReleaseBuffer();
    lvc.cx = listRect.Width() - 440;
    lvc.fmt = LVCFMT_LEFT;
    lvc.iSubItem = 0;
    m_profileList.InsertColumn(0, &lvc);

    csString.LoadString(IDS_TARGET_FORMAT);
    lvc.pszText = csString.GetBuffer(csString.GetLength());
    csString.ReleaseBuffer();
    lvc.cx = 190;
    lvc.fmt = LVCFMT_LEFT;
    lvc.iSubItem = 2;
    m_profileList.InsertColumn(1, &lvc);

    csString.LoadString(IDS_STORAGE);
    lvc.pszText = csString.GetBuffer(csString.GetLength());
    csString.ReleaseBuffer();
    lvc.cx = 170;
    lvc.fmt = LVCFMT_LEFT;
    lvc.iSubItem = 3;
    m_profileList.InsertColumn(2, &lvc);

    csString.LoadString(IDS_ACTIVATED);
    lvc.pszText = csString.GetBuffer(csString.GetLength());
    csString.ReleaseBuffer();
    lvc.cx = 80;
    lvc.fmt = LVCFMT_LEFT;
    lvc.iSubItem = 1;
    m_profileList.InsertColumn(3, &lvc);

    FillProfileList();

    return TRUE;  // return TRUE unless you set the focus to a control
}

void CProfileManager::OnIdleUpdateUIMessage() {
    UpdateDialogControls( this, TRUE );
}

void CProfileManager::OnKillFocus(CWnd* pNewWnd) {
    CDialog::OnKillFocus(pNewWnd);

    EndEditItem(false);
}

BOOL CProfileManager::PreTranslateMessage(MSG* pMsg) {
    CString str;
    if (pMsg->message == WM_KEYDOWN) {
        if (pMsg->wParam == VK_F2) {
            OnRenameProfile();

            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);
            return TRUE;		
        }
        if (pMsg->wParam == VK_ESCAPE) {
            EndEditItem(true);

            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);
            return TRUE;		    	
        }
    }	

    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CProfileManager::ContinueModal() {
    SendMessage(WM_IDLEUPDATECMDUI, WPARAM(TRUE), 0);

    return CDialog::ContinueModal();
}

void CProfileManager::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROFILE_LIST, m_profileList);
}

BOOL CProfileManager::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
    NMHDR *pnmh = (LPNMHDR)lParam;
    if (pnmh->code == LVN_ENDLABELEDIT)
        EndEditItem(false);

    return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL CProfileManager::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
    return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void CProfileManager::OnCancel() {
    // remove item data
    for (int i = 0; i < m_profileList.GetItemCount(); ++i) {
        ProfileInfo *pInfo = (ProfileInfo *)m_profileList.GetItemData(i);
        if (pInfo != NULL)
            delete pInfo;
        pInfo = NULL;
    }

    CDialog::OnCancel();
}

void CProfileManager::OnOK() {
    // fill list with changed profiles
    for (int i = 0; i < m_aProfileInformation.GetSize(); ++i)
        if (m_aProfileInformation[i])
            delete m_aProfileInformation[i];
    m_aProfileInformation.RemoveAll();

    for (int i = 0; i < m_profileList.GetItemCount(); ++i) {
        ProfileInfo *pInfo = (ProfileInfo *)m_profileList.GetItemData(i);
        if (pInfo != NULL) {
            m_aProfileInformation.Add(pInfo);
            pInfo->Write();
        }
    }

    CDialog::OnOK();
}

HBRUSH CProfileManager::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
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

void CProfileManager::OnNewProfile() {
    if (!m_csLrdFilename.IsEmpty()) {
        SetCursor(LoadCursor(NULL, IDC_WAIT));
        HRESULT hr = New(m_csLrdFilename);
        if (SUCCEEDED(hr)) {
            m_bWaitForNewProfile = true;
            
            ::EnableWindow(GetSafeHwnd(), FALSE);
            hr = ProfileUtils::WaitForProfile(this);
        }
    }
}

void CProfileManager::OnEditProfile() {
    if (!m_csLrdFilename.IsEmpty()) {
        int iSelectedItem = GetSelectedItem();
        ProfileInfo *pInfo = (ProfileInfo *)m_profileList.GetItemData(iSelectedItem);
        if (pInfo) {
            SetCursor(LoadCursor(NULL, IDC_WAIT));
            HRESULT hr = pInfo->Edit(this, m_csLrdFilename);
            if (SUCCEEDED(hr)) {
                m_bWaitForChangedProfile = true;
                
                ::EnableWindow(GetSafeHwnd(), FALSE);
                hr = ProfileUtils::WaitForProfile(this);
            }
        }
    }
}

void CProfileManager::OnRenameProfile() {
    int iSelectedItem = GetSelectedItem();

    if (iSelectedItem != -1) {
        ProfileInfo *pInfo = (ProfileInfo *)m_profileList.GetItemData(iSelectedItem);

        if (pInfo != NULL) {
            if (!LMisc::IsUserAdmin() && pInfo->IsCreatedByAdmin()) {
                ProfileUtils::ShowErrorMessage(IDS_ERROR_NOADMIN, 0, IDS_CAPTION_NOPERMISSION);
                return;
            }
        }
        m_profileList.SetFocus();
        CEdit *pEdit = m_profileList.EditLabel(iSelectedItem);
    }
}

void CProfileManager::OnDeleteProfile() { 
    int iSelectedItem = GetSelectedItem();
    if (iSelectedItem >= 0) {
        ProfileInfo *pInfo = (ProfileInfo *)m_profileList.GetItemData(iSelectedItem);
        if (pInfo) {
            if (!::IsUserAnAdmin() && pInfo->IsCreatedByAdmin()) {
                ProfileUtils::ShowErrorMessage(IDS_ERROR_NOADMIN, 0, IDS_CAPTION_NOPERMISSION);
                return;
            }

            CString csDeleteCaption;
            csDeleteCaption.LoadString(IDS_CONFIRM_DELETE_CAPTION);
            CString csDeleteMessage;
            csDeleteMessage.LoadString(IDS_CONFIRM_DELETE_TEXT);

            int ret = MessageBox(csDeleteMessage, csDeleteCaption, MB_YESNO | MB_TOPMOST);

            if (ret == IDYES) {
                HRESULT hr = pInfo->DeleteProfile();
                if (FAILED(hr))
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_DELETE_FILE);
                delete pInfo;
                m_profileList.DeleteItem(iSelectedItem);
            }
        }
    }
}

void CProfileManager::OnImportProfile() { 
    CString filter;
    filter.LoadString(IDS_PPR_FILEFILTER);

    CString csInitialDir;
    GetPersonalFolder(csInitialDir);

    TCHAR tszCurrentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, tszCurrentDirectory);

    CString csFileName;
    FileDialog::GetOpenFilename(AfxGetApp(), csFileName, this->GetSafeHwnd(), 
    AfxGetInstanceHandle(), filter, _T("LPPIMPORT"), csInitialDir);

    SetCurrentDirectory(tszCurrentDirectory);

    if (!csFileName.IsEmpty()) {
        ProfileInfo *pNewInfo = new ProfileInfo(csFileName);

        if (pNewInfo) {
            HRESULT hr = pNewInfo->Read();
            if (FAILED(hr)) {
                if (hr == E_PM_FILE_NOTEXIST)
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_FILENOTFOUND_IMPORT);
                else if (hr == E_PM_FILE_OPEN)
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_PERMISSION_READ_IMPORT);
                else if (hr == E_PM_FILE_READ)
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_READ_IMPORT);
                else if (hr == E_PM_WRONG_FORMAT)
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_FORMAT_IMPORT);
            }

            if (SUCCEEDED(hr)) {
                bool bUserIsAdmin = LMisc::IsUserAdmin();
                pNewInfo->SetCreatedByAdmin(bUserIsAdmin);
                pNewInfo->EmptyFilename();

                CheckProfileID(pNewInfo);

                int iIndex = AppendNewProfile(pNewInfo);
                ActivateProfile(iIndex);
                m_profileList.SetItemState(iIndex, LVIS_SELECTED, LVIS_SELECTED);
                m_profileList.SortItems(MyCompareProc, (LPARAM) &m_profileList);
                hr = pNewInfo->Write();
                if (FAILED(hr)) {
                    if (hr == E_PM_FILE_CREATE || hr == E_PM_FILE_NOTEXIST)
                        ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_IMPORT, IDS_ERROR_PERMISSION);
                    else if (hr == E_PM_CREATE_SUBDIR)
                        ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_IMPORT, IDS_ERROR_CREATESUBDIR);
                    else if (hr == E_PM_FILE_WRITE)
                        ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_IMPORT, IDS_ERROR_WRITE);
                    else if (hr == E_PM_GET_APPLDIR)
                        ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_IMPORT, IDS_ERROR_APPLICATIONDIR);
                }
            }
        }
    }
}

void CProfileManager::OnExportProfile() {
    HRESULT hr = S_OK;

    CString filter;
    filter.LoadString(IDS_PPR_FILEFILTER);

    CString csInitialDir;
    GetPersonalFolder(csInitialDir);

    TCHAR tszCurrentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, tszCurrentDirectory);

    CString csFileName;
    bool bRet = FileDialog::GetSaveFilename(AfxGetApp(), csFileName, this->GetSafeHwnd(), 
        AfxGetInstanceHandle(), filter, _T(".lpp"), 
        _T("LPPEXPORT"), csInitialDir);

    SetCurrentDirectory(tszCurrentDirectory);

    if (bRet == false || csFileName.IsEmpty())
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CString csPath = csFileName;
        StringManipulation::GetPath(csPath);
        if (_taccess(csPath, 00) != 0) {
            ProfileUtils::ShowErrorMessage(IDS_ERROR_CREATESUBDIR_EXPORT);
            hr = E_FAIL;
        }
    }

    if (SUCCEEDED(hr)) {
        int iSelected = GetSelectedItem();
        if (iSelected >= 0) {
            ProfileInfo *pInfo = (ProfileInfo *)m_profileList.GetItemData(iSelected);
            if (pInfo) {
                HRESULT hr = pInfo->Write(csFileName);
                if (FAILED(hr)) {
                    if (hr == E_PM_FILE_CREATE || hr == E_PM_FILE_NOTEXIST)
                        ProfileUtils::ShowErrorMessage(IDS_ERROR_PERMISSION_EXPORT);
                    else if (hr == E_PM_FILE_WRITE)
                        ProfileUtils::ShowErrorMessage(IDS_ERROR_WRITE_EXPORT);
                }
            }
        }
    }
}

void CProfileManager::OnSelectionEnableButtons(CCmdUI *pCmdUI) {
    bool bEnable = false;

    // if the dialog wait for java answer, disble all buttons
    // if no item is selected disable the "Edit"-, "Rename"-, 
    // "Delete"-, "Activate"- and "Export"-Button
    if (m_profileList.GetSelectedCount() > 0)
        bEnable = true;

    pCmdUI->Enable(bEnable);
}

HRESULT CProfileManager::GetProfileInformation(CArray<ProfileInfo *, ProfileInfo *> &aProfileInformation) {
    HRESULT hr = S_OK;

    // Fill the profile information array with the items 
    // information readed from the list if "OK"-Button was pressed.
    aProfileInformation.RemoveAll();
    aProfileInformation.Append(m_aProfileInformation);

    return hr;
}

HRESULT CProfileManager::GetSelectedProfile(ProfileInfo **ppInfo) {
    HRESULT hr = S_OK;

    int iIndex = GetSelectedItem();
    *ppInfo = (ProfileInfo *)m_profileList.GetItemData(iIndex);

    return hr;
}

HRESULT CProfileManager::AppendOrEditProfile(__int64 iProfileID) {
    HRESULT hr = S_OK;

    CArray<ProfileInfo *, ProfileInfo *> aProfileInformation;
    ProfileUtils::ReadCustomProfiles(aProfileInformation);

    ProfileInfo *pCurrentProfile = NULL;
    for (int i = 0; i < aProfileInformation.GetSize() && pCurrentProfile == NULL; ++i) {
        if (aProfileInformation[i]->GetID() == iProfileID)
            pCurrentProfile = new ProfileInfo(aProfileInformation[i]);
    }

    // delete profile list
    for (int i = 0; i < aProfileInformation.GetSize(); ++i) {
        if (aProfileInformation[i] != NULL)
            delete aProfileInformation[i];
    }
    aProfileInformation.RemoveAll();

    if (pCurrentProfile == 0)
        hr = E_FAIL;

    SetCursor(NULL);
    ::EnableWindow(GetSafeHwnd(), TRUE);
    SetForegroundWindow();

    if (SUCCEEDED(hr) && m_bWaitForNewProfile) {
        CString csProfileName;
        csProfileName.LoadString(IDS_DEFAULT_NAME);
        CProfileNameDialog dialogProfileName(csProfileName, this);
        int iRet = dialogProfileName.DoModal();

        if (iRet == IDOK) {
            csProfileName = dialogProfileName.GetProfileName();
            pCurrentProfile->SetTitle(csProfileName);

            int iIndex = AppendNewProfile(pCurrentProfile);
            ActivateProfile(iIndex);
            m_profileList.SetItemState(iIndex, LVIS_SELECTED, LVIS_SELECTED);
            m_profileList.SortItems(MyCompareProc, (LPARAM) &m_profileList);

            bool bUserIsAdmin = LMisc::IsUserAdmin();
            pCurrentProfile->SetCreatedByAdmin(bUserIsAdmin);

            hr = pCurrentProfile->Write(); 
            if (FAILED(hr)) {
                if (hr == E_PM_FILE_CREATE || hr == E_PM_FILE_NOTEXIST)
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_NEW, IDS_ERROR_PERMISSION);
                else if (hr == E_PM_CREATE_SUBDIR)
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_NEW, IDS_ERROR_CREATESUBDIR);
                else if (hr == E_PM_FILE_WRITE)
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_NEW, IDS_ERROR_WRITE);
                else if (hr == E_PM_GET_APPLDIR)
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_NEW, IDS_ERROR_APPLICATIONDIR);
            }
        }
        m_bWaitForNewProfile = false;
    } else if (SUCCEEDED(hr) && m_bWaitForChangedProfile) {
        int iIndex = GetSelectedItem();
        if (iIndex < 0)
            hr = E_FAIL;

        ProfileInfo *pOldProfileInfo = NULL;
        if (SUCCEEDED(hr)) {
            pOldProfileInfo = (ProfileInfo *)m_profileList.GetItemData(iIndex);
            if (pOldProfileInfo == NULL) {
                ProfileUtils::ShowErrorMessage(IDS_ERROR_NOTINLIST_EDIT);
                hr = E_PM_NOTINLIST;
            } else
                delete pOldProfileInfo;
        }

        if (SUCCEEDED(hr)) {
            m_profileList.SetItemData(iIndex, (DWORD_PTR)pCurrentProfile);
            m_profileList.SetItemText(iIndex, 1, pCurrentProfile->GetTargetFormat());      
            m_profileList.SetItemText(iIndex, 2, pCurrentProfile->GetStorageDistribution());
            ActivateProfile(iIndex);
        }

        if (SUCCEEDED(hr)) {
            if (FAILED(hr)) {
                if (hr == E_PM_FILE_CREATE || hr == E_PM_FILE_NOTEXIST)
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_EDIT, IDS_ERROR_PERMISSION);
                else if (hr == E_PM_CREATE_SUBDIR)
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_EDIT, IDS_ERROR_CREATESUBDIR);
                else if (hr == E_PM_FILE_WRITE)
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_EDIT, IDS_ERROR_WRITE);
                else if (hr == E_PM_GET_APPLDIR)
                    ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_EDIT, IDS_ERROR_APPLICATIONDIR);
            }
        }

        m_bWaitForChangedProfile = false;
    }

    return hr;
}

HRESULT CProfileManager::CancelAppendOrEdit() {
    HRESULT hr = S_OK;

    if (m_bWaitForNewProfile)
        m_bWaitForNewProfile = false;
    else if (m_bWaitForChangedProfile)
        m_bWaitForChangedProfile = false;

    SetCursor(NULL);
    ::EnableWindow(GetSafeHwnd(), TRUE);
    SetForegroundWindow();

    return hr;
}

HRESULT CProfileManager::FillProfileList() {
    HRESULT hr = S_OK;

    for (int i = 0; i < m_aProfileInformation.GetSize(); ++i) {
        ProfileInfo *pProfile = new ProfileInfo(m_aProfileInformation[i]);
        int iIndex = AppendNewProfile(pProfile);
    }

    m_profileList.SortItems(MyCompareProc, (LPARAM) &m_profileList);

    return hr;
}

int CProfileManager::GetSelectedItem() {
    POSITION p = m_profileList.GetFirstSelectedItemPosition();
    int iSelected = m_profileList.GetNextSelectedItem(p);

    return iSelected;
}

void CProfileManager::EndEditItem(bool bEscape) {
    CEdit *pEdit = m_profileList.GetEditControl();
    int iSelectedItem = GetSelectedItem();

    if (pEdit != NULL && iSelectedItem >= 0) {
        if (bEscape) {
            CString csTmp = m_profileList.GetItemText(iSelectedItem, 0);
            pEdit->SetWindowText(csTmp);
        } else {
            CString csTmp;
            // change title field in list
            pEdit->GetWindowText(csTmp);

            m_profileList.SetItemText(iSelectedItem, 0, csTmp);
            // change title also in profile info
            ProfileInfo *pInfo = (ProfileInfo *)m_profileList.GetItemData(iSelectedItem);
            if (pInfo) {
                pInfo->SetTitle(csTmp);
                HRESULT hr = pInfo->Write();
                if (FAILED(hr)) {
                    if (hr == E_PM_FILE_CREATE || hr == E_PM_FILE_NOTEXIST)
                        ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_RENAME, IDS_ERROR_PERMISSION);
                    else if (hr == E_PM_CREATE_SUBDIR)
                        ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_RENAME, IDS_ERROR_CREATESUBDIR);
                    else if (hr == E_PM_FILE_WRITE)
                        ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_RENAME, IDS_ERROR_WRITE);
                    else if (hr == E_PM_GET_APPLDIR)
                        ProfileUtils::ShowErrorMessage(IDS_ERROR_ACTION_RENAME, IDS_ERROR_APPLICATIONDIR);
                }
                m_profileList.DeleteItem(iSelectedItem);
                int iIndex = AppendNewProfile(pInfo);
                m_profileList.SetItemState(iIndex, LVIS_SELECTED, LVIS_SELECTED);
                m_profileList.SortItems(MyCompareProc, (LPARAM) &m_profileList);
            }
        }
    }
}

void CProfileManager::ActivateProfile(int iItem) {
    CString csYes;
    csYes.LoadString(IDS_YES);
    CString csNo = "";

    if (iItem >= 0) {
        for (int i = 0; i < m_profileList.GetItemCount(); ++i) {
            ProfileInfo *pInfo = (ProfileInfo *)m_profileList.GetItemData(i);

            if (i == iItem) {
                m_profileList.SetItemText(i, 3, csYes);
                if (pInfo)
                    pInfo->SetActivated(true);
            } else {
                m_profileList.SetItemText(i, 3, csNo);
                if (pInfo)
                    pInfo->SetActivated(false);
            }
        }
    }
}

int CProfileManager::AppendNewProfile(ProfileInfo *pProfileInfo) {
    int iIndex = m_profileList.GetItemCount();

    // column 1: title
    iIndex = m_profileList.InsertItem(iIndex, pProfileInfo->GetTitle());
    // column 2: target format
    m_profileList.SetItemText(iIndex, 1, pProfileInfo->GetTargetFormat());      
    // column 3: storage/distribution
    m_profileList.SetItemText(iIndex, 2, pProfileInfo->GetStorageDistribution());
    // column 4: activation
    CString csActivated = "";
    if (pProfileInfo->IsActivated())
        csActivated.LoadString(IDS_YES);
    m_profileList.SetItemText(iIndex, 3, csActivated);

    // append complete profile information
    m_profileList.SetItemData(iIndex, (DWORD_PTR)pProfileInfo);

    return iIndex;
}

HRESULT CProfileManager::GetPersonalFolder(CString &csPersonalFolder) {
    HRESULT hr = S_OK;

    HKEY hKey; 
    BYTE *aData = NULL;

    LONG regErr = RegOpenKeyEx (HKEY_CURRENT_USER, 
        _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), NULL, KEY_READ, &hKey); 

    if (regErr != ERROR_SUCCESS)
    {
        regErr = RegOpenKeyEx (HKEY_CURRENT_USER, 
            _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Explorer\\Shell Folders"), NULL, KEY_READ, &hKey); 
    }

    if (regErr != ERROR_SUCCESS)
        hr = E_PM_OPEN_KEY;

    CString csAppData = _T("Personal");

    DWORD dwType;
    DWORD dwBufSize = 0;
    if (SUCCEEDED(hr)) {
        regErr = RegQueryValueEx(hKey, csAppData, NULL, &dwType, NULL, &dwBufSize );
        aData = new BYTE[dwBufSize + 8];

        if (regErr != ERROR_SUCCESS)
            hr = E_PM_QUERY_VALUE;
    }

    if (SUCCEEDED(hr)) {
        regErr = RegQueryValueEx(hKey, csAppData, NULL, &dwType, aData, &dwBufSize );

        if (regErr != ERROR_SUCCESS)
            hr = E_PM_QUERY_VALUE;
    }

    if (SUCCEEDED(hr)) {
        csPersonalFolder = (_TCHAR *)aData;
    }

    if (aData)
        delete[] aData;

    RegCloseKey(hKey);

    return hr;
}

HRESULT CProfileManager::CheckProfileID(ProfileInfo *pNewInfo) {
    HRESULT hr = S_OK;

    for (int i = 0; i < m_profileList.GetItemCount(); ++i) {
        ProfileInfo *pInfo = (ProfileInfo *)m_profileList.GetItemData(i);
        if (pInfo != NULL) {
            if (pNewInfo->GetID() == pInfo->GetID()) {
                __int64 lNewID = 0;
                ProfileUtils::GetNewProfileID(lNewID);
                pNewInfo->SetID(lNewID);
                break;
            }
        }
    }

    return hr;
}

HRESULT CProfileManager::New(CString csLrdName) {
    HRESULT hr = S_OK;

    CString csConverterPath = JNISupport::GetInstallDir();

    if (!csConverterPath.IsEmpty()) {
        if (csConverterPath[csConverterPath.GetLength() - 1] != _T('\\'))
            csConverterPath += _T("\\");
        csConverterPath += _T("Publisher");

        CString csConverterExe = csConverterPath;
        csConverterExe += _T("\\publisher.exe");

        // TODO: Create new ID
        __int64 iNewProfileID = 0; 
        hr = ProfileUtils::GetNewProfileID(iNewProfileID);

        if (SUCCEEDED(hr)) {
            ProfileUtils::HandlePublisherFinish(this);

            CString csParameter;
            csParameter.Format(_T("-profile %I64d \"%s\""), iNewProfileID, csLrdName);

            HINSTANCE hInstance = ShellExecute(NULL, _T("open"), csConverterExe, csParameter, csConverterPath, SW_SHOWNORMAL);
            if ((int)hInstance <= 32)
                hr = E_FAIL;
        }

    }
    else 
        hr = E_FAIL;

    return hr;
}

int CALLBACK CProfileManager::MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
    ProfileInfo *pInfo1 = (ProfileInfo *)lParam1;
    ProfileInfo *pInfo2 = (ProfileInfo *)lParam2;

    if (pInfo1 == NULL)
        return -1;
    if (pInfo2 == NULL)
        return 1;

    CString csTitle1 = pInfo1->GetTitle();
    CString csTitle2 = pInfo2->GetTitle();

    if (csTitle1.CompareNoCase(csTitle2) == 0) {
        if (csTitle1.Compare(csTitle2) == 0) {
            if (pInfo1->GetID() < pInfo2->GetID())
                return -1;
            else
                return 1;
        } else if (csTitle1.Compare(csTitle2) < 0) {
            return -1;
        } else if (csTitle1.Compare(csTitle2) > 0) {
            return 1;
        }
    } else if (csTitle1.CompareNoCase(csTitle2) < 0) {
        return -1;
    } else  if (csTitle1.CompareNoCase(csTitle2) > 0) {
        return 1;
    }

    return 0;

}

void CProfileManager::PublisherFinished(CString csPublisherMessage) {
    bool bCancel = true;
    __int64 iProfileID = 0;
    CLanguageSupport::SetThreadLanguage();

    HRESULT hr = 
        ProfileUtils::ParseProfileMessage(csPublisherMessage, bCancel, iProfileID);

    if (SUCCEEDED(hr)) {
        if (!bCancel)
            AppendOrEditProfile(iProfileID);
        else
            CancelAppendOrEdit();
    }
}