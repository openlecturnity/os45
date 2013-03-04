#include "StdAfx.h"
#include "MfcUtils.h"

void MfcUtils::Localize(CWnd *pWnd, DWORD *adwIds) {
    int nIndex = 0;
    DWORD dwId = adwIds[nIndex];

    while (dwId != -1) {
        CString csString;
        if (FALSE != csString.LoadString(dwId)) {
            CWnd *pChild = pWnd->GetDlgItem(dwId);
            if (pChild) {
                pWnd->SetDlgItemText(dwId, csString);
            }
#ifdef _DEBUG
            else {
                CString csMsg;
                csMsg.Format(_T("MfcUtils::Localize()\n\nGetDlgItem(%d) returned NULL."), dwId);
                ::MessageBox(::GetForegroundWindow(), csMsg, _T("Warning"), MB_OK | MB_ICONWARNING);
            }
#endif
        }
#ifdef _DEBUG
        else {
            CString csMsg;
            csMsg.Format(_T("MfcUtils::Localize()\n\nLoadString failed for ID %d."), dwId);
            ::MessageBox(::GetForegroundWindow(), csMsg, _T("Warning"), MB_OK | MB_ICONWARNING);
        }
#endif
        dwId = adwIds[++nIndex];
    }
}
