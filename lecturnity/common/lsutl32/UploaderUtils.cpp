#include "stdafx.h"
#include "UploaderUtils.h"

CUploaderUtils::CUploaderUtils(void) {
}

CUploaderUtils::~CUploaderUtils(void) {
}

int CUploaderUtils::ShowYouTubeSettingsDialog(CWnd* pParent, CString csUser, CString csPassword,CString csTitle, 
        CString csDescription, CString csKeywords, CString csCategory, int iPrivacy, bool bCheck) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
    m_ytts.csUser = csUser;
    m_ytts.csPassword = csPassword;
    m_ytts.csTitle = csTitle;
    m_ytts.csDescription = csDescription;
    m_ytts.csKeywords = csKeywords;
    m_ytts.csCategory = csCategory;
    m_ytts.iPrivacy = iPrivacy;

    
    CYouTubeSettings ytsDlg(m_ytts, pParent);
    if (bCheck == true) {
        bool bCheckResult = ytsDlg.CheckYouTubeRequiredParameters(m_ytts.csTitle, m_ytts.csDescription, m_ytts.csKeywords);
        if (bCheckResult == false) {
            CString csYtCheckError = ytsDlg.GetYouTubeRequiredErrorMessage();
            _tprintf(_T("!!!LSU csYtCheckError = %s\n"), csYtCheckError);
            MessageBox(NULL, csYtCheckError, _T("Publisher"), MB_OK | MB_ICONSTOP | MB_SETFOREGROUND | MB_TOPMOST | MB_TASKMODAL);
        } else {
            return IDIGNORE;
        }
    }
    int iResId = ytsDlg.DoModal();
    if (iResId == IDOK) {
        m_ytts = ytsDlg.GetTransferSettings();
    }
    return iResId;
}

void CUploaderUtils::GetYouTubeTransferSettings(CString &csUser, CString &csPassword,CString &csTitle, 
        CString &csDescription, CString &csKeywords, CString &csCategory, int &iPrivacy) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
    csUser = m_ytts.csUser;
    csPassword = m_ytts.csPassword;
    csTitle = m_ytts.csTitle;
    csDescription = m_ytts.csDescription;
    csKeywords = m_ytts.csKeywords;
    csCategory = m_ytts.csCategory;
    iPrivacy = m_ytts.iPrivacy;
}

int CUploaderUtils::ShowSlidestarSettingsDialog(CWnd* pParent, CString csServer, CString csUser, CString csPassword) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    
    m_sts.csServer = csServer;
    m_sts.csUserName = csUser;
    m_sts.csPassword = csPassword;
    CSlidestarSettings slsDld(m_sts, pParent);

    int iResId = slsDld.DoModal();
    if (iResId == IDOK) {
        m_sts = slsDld.GetSlidestarTransferSettings();
    }
    return iResId;
}
void CUploaderUtils::GetSlidestarTransferSettings(CString &csServer, CString &csUser, CString &csPassword) {
    csServer = m_sts.csServer;
    csUser = m_sts.csUserName;
    csPassword = m_sts.csPassword;
}