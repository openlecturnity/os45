#include "stdafx.h"
#include "lsutl32.h"
#include "EnterLicenseDialog.h"

#include "MfcUtils.h"
#include "KeyGenerator.h"
#include "lutils.h" // WriteStringRegistryEntry()

// static
bool CEnterLicenseDialog::ShowItModal(CWnd* pParent) {
    //
    // This method is necessary to create with AFX_MANAGE_STATE
    // the right context to find the right (local) resources.
    //
    // It is especially important that creation and DoModal are
    // in the same context/method.
    //

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CEnterLicenseDialog dlg(pParent);

    dlg.DoModal();


    //} else {
    //    dlg->Create(CEnterLicenseDialog::IDD);
    //    dlg->ShowWindow(SW_SHOW);
    //}

    return dlg.HasKeyEntered();
}


IMPLEMENT_DYNAMIC(CEnterLicenseDialog, CDialog)

CEnterLicenseDialog::CEnterLicenseDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CEnterLicenseDialog::IDD, pParent) {

    m_bKeyEntered = false;
}

CEnterLicenseDialog::~CEnterLicenseDialog() {
}

void CEnterLicenseDialog::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDT_KEY, m_edtKey);
    DDX_Control(pDX, IDOK, m_btnEnter);
}


BEGIN_MESSAGE_MAP(CEnterLicenseDialog, CDialog)
    ON_BN_CLICKED(IDOK, &CEnterLicenseDialog::OnBnClickedBtnEnterLicense)
END_MESSAGE_MAP()


BOOL CEnterLicenseDialog::OnInitDialog() {
    BOOL bSuccess = CDialog::OnInitDialog(); // must be first statement

    AFX_MANAGE_STATE(AfxGetStaticModuleState()); // is then probably unnecessary (done in ShowItModal())?

    DWORD dwIds[] = {
        IDC_STC_ENTER_LICENSE_TEXT,
        IDC_STC_KEY_GROUP,
        IDOK,
        IDCANCEL,
        -1
    };

    MfcUtils::Localize(this, dwIds);

    if (LMisc::IsWindowsVistaOrHigher())
        m_btnEnter.SetBitmap(CSize(16, 16), IDB_BM_SHIELD);

    m_edtKey.SetFocus();

    return TRUE;
}

void CEnterLicenseDialog::OnBnClickedBtnEnterLicense() {
    CString csKey;

    m_edtKey.GetWindowText(csKey);

    if (csKey.GetLength() == 0) {
        CString csKeyMissing;
        csKeyMissing.LoadString(IDS_KEY_MISSING);
        MessageBox(csKeyMissing, _T("LECTURNITY"), MB_ICONWARNING | MB_OK);

        return;
    }

    // Doubled in CSplashScreen2
    bool bCanWriteRegistry = 
        LMisc::IsUserAdmin() || LMisc::IsWindowsVistaOrHigher() && LMisc::CanUserElevate();

    if (!bCanWriteRegistry) {
        CString csNoAdmin;
        csNoAdmin.LoadString(IDS_NO_ADMIN);
        MessageBox(csNoAdmin, _T("LECTURNITY"), MB_ICONERROR | MB_OK);
        
        return;
    }


    // This is also done in KerberokHandler::WriteKerberokAll()
    // Here only for key integrity check and version type check.
    csKey.Replace(_T(" "), _T(""));
    csKey.Replace(_T("-"), _T(""));

    char szKey[40];
    for (int i=0; i<csKey.GetLength() && i < 40-1; ++i)
        szKey[i] = csKey.GetAt(i);
    szKey[csKey.GetLength()] =  0;
    

    KeyGenerator g;

    char szName[20];
    char szVersion[10];
    int iType, iLang, iPeriod, iExpires;
    bool bRm, bWm, bCd, bSg, bEd, bMp4;

    bool bKeySuccess = g.DecodeKey20(szKey, szName, szVersion, &iType, &iLang,
        &iPeriod, &iExpires, &bRm, &bWm, &bCd, &bSg, &bEd, &bMp4);

    if (bKeySuccess && (iType == TYPE_FULL_VERSION || iType == TYPE_CAMPUS_VERSION || iType == TYPE_EVAL_VERSION)) {

        // TODO:
        //LMisc::LaunchProgram(

        LIo::MessagePipe pipe;
        HRESULT hrPipe = pipe.Init(_T("key-success-return"), true);

        WCHAR wszCurrentDir[MAX_PATH];
        wszCurrentDir[0] = 0;
        DWORD dwReturn = ::GetCurrentDirectory(MAX_PATH, wszCurrentDir);

        WCHAR *wszVerb = L"open";
        if (LMisc::IsWindowsVistaOrHigher())
            wszVerb = L"runas";

        WCHAR wszCommandLine[300];
        swprintf_s(wszCommandLine, 300, L"lsutl32.dll,UpdateLicenseKey %S", szKey);

        int iResult = (int)::ShellExecute(
            m_hWnd, wszVerb, L"rundll32.exe", wszCommandLine, wszCurrentDir, 0);

        bool bKeyWriteSuccess = false;

        if (iResult > 32) {
            // Everything alright; but only with the starting

            _TCHAR tszMessage[40];
            tszMessage[0] = 0;

            if (SUCCEEDED(hrPipe))
                hrPipe = pipe.ReadMessage(tszMessage, 40);

            if (SUCCEEDED(hrPipe)) {
                if (_tcsncmp(tszMessage, _T("ok"), 2) == 0)
                    bKeyWriteSuccess = true;
            }
        }


        if (!bKeyWriteSuccess) {
            CString csKeyWriteError;
            csKeyWriteError.LoadString(IDS_KEY_FAILURE);
            MessageBox(csKeyWriteError, _T("LECTURNITY"), MB_ICONERROR | MB_OK);
        } else {
            CString csKeySuccess;
            csKeySuccess.LoadString(IDS_KEY_SUCCESS);
            MessageBox(csKeySuccess, _T("LECTURNITY"), MB_ICONINFORMATION | MB_OK);

            m_bKeyEntered = true; // is never reset as only successful writes change registry

            SendMessage(WM_CLOSE);
        }
        

        return;


    } else {
        CString csKeyError;
        csKeyError.LoadString(IDS_KEY_INVALID);
        MessageBox(csKeyError, _T("LECTURNITY"), MB_ICONERROR | MB_OK);

        // TODO does not look good
    }

}
