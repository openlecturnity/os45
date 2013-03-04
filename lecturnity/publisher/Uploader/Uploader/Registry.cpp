// Registry.cpp: implementation of the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Uploader.h"
#include "Registry.h"
#include "Crypto.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistry::CRegistry() {
    HKEY hKey = NULL;
}

CRegistry::~CRegistry() {

}

LONG CRegistry::Create(CString csKey = _T("")) {
    CString csOpenKey = UPLOADER_KEY;
    csOpenKey += csKey;
    lResult = ::RegCreateKeyEx(HKEY_CURRENT_USER,
        csOpenKey,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,
        &hKey,
        NULL);

    if (lResult != ERROR_SUCCESS) {
        lResult;
    }

    return lResult;
}

LONG CRegistry::Open(CString csKey = _T("")) {
    CString csOpenKey = UPLOADER_KEY;
    csOpenKey += csKey;
    lResult = ::RegOpenKeyEx(HKEY_CURRENT_USER,
        csOpenKey,
        0,
        KEY_ALL_ACCESS,
        &hKey);

    if (lResult != ERROR_SUCCESS) {
        lResult;
    }

    return lResult;
}

LONG CRegistry::Close() {
    lResult = -1;
    if (hKey != NULL) {
        lResult = ::RegCloseKey(hKey);
    }
    return lResult;
}

LONG CRegistry::WriteProfileInt(CString pszSubKey, int iValue) {
    lResult = -1;
    if (hKey != NULL) {
        lResult = ::RegSetValueEx(hKey,
            pszSubKey,
            NULL,
            REG_DWORD,
            (BYTE *)&iValue,
            sizeof(iValue)
            );
    }
    return lResult;
}

LONG CRegistry::WriteProfileString(CString pszSubKey, CString csValue) {
    LPCTSTR sValue = (LPCTSTR)csValue;
    int iLength = csValue.GetLength() * sizeof(TCHAR);
    lResult = -1;
    if (hKey != NULL) {
        lResult = ::RegSetValueEx(hKey,
            pszSubKey,
            NULL,
            REG_SZ,
            (BYTE *)sValue,
            iLength
            );
    }
    return lResult;
}

int CRegistry::GetProfileInt(CString pszSubKey, int iDefaultValue) {
    int iValue = iDefaultValue;
    DWORD cbData = sizeof(int);
    DWORD dwType = REG_DWORD; 
    if (hKey != NULL) {
        LONG lResult = ::RegQueryValueEx(hKey,
            pszSubKey,
            NULL,
            &dwType,
            (BYTE *)&iValue,
            &cbData
            );
    }
    return iValue;
}

CString CRegistry::GetProfileString(CString pszSubKey, CString csDefaultValue) {
    TCHAR buffer[MAX_PATH];
    CString csValue = csDefaultValue;
    DWORD cbData = 2 * MAX_PATH - 1;
    DWORD dwType = REG_SZ; 
    if (hKey != NULL) {
        lResult = ::RegQueryValueEx(hKey,
            pszSubKey,
            NULL,
            &dwType,
            (BYTE *)buffer,
            &cbData
            );
        if (lResult == ERROR_SUCCESS) {
            csValue = CString(buffer, (cbData / sizeof(TCHAR)));
        } else {
            csValue = csDefaultValue;
        }
    }
    return csValue;
}

void CRegistry::LoadTransferParam(TransferParam &tp, CString pszKey) {
    CCrypto crypt;
    if (Open(pszKey) == ERROR_SUCCESS) {
        tp.iService      = GetProfileInt(_T("Service"), UPLOAD_FTP);   
        tp.csServer      = GetProfileString(_T("Server"), _T(""));
        tp.nPort         = GetProfileInt(_T("Port"), DEFAULT_FTP_PORT);   
        tp.csUsername    = GetProfileString(_T("Username"), _T(""));
        tp.bSavePassword = GetProfileInt(_T("SavePassword"), TRUE);   
        if (tp.bSavePassword) {
            CString csPassword = GetProfileString(_T("Password"), _T(""));
            tp.csPassword = crypt.Decode(csPassword);
        }
        tp.csDirectory   = GetProfileString(_T("Directory"), _T("/"));

        Close();
    }
}

void CRegistry::LoadChannelTransferParam(TransferParam &tp, CString pszKey) {
    CCrypto crypt;
    if (Open(pszKey) == ERROR_SUCCESS) {
        tp.csUrl         = GetProfileString(_T("Channel URL"), _T(""));
        tp.iService      = GetProfileInt(_T("Channel Service"), UPLOAD_FTP);   
        tp.csServer      = GetProfileString(_T("Channel Server"), _T(""));
        tp.nPort         = GetProfileInt(_T("Channel Port"), DEFAULT_FTP_PORT);   
        tp.csUsername    = GetProfileString(_T("Channel Username"), _T(""));
        tp.bSavePassword = GetProfileInt(_T("Channel SavePassword"), TRUE);   
        if (tp.bSavePassword) {
            CString csPassword = GetProfileString(_T("Channel Password"), _T(""));
            tp.csPassword = crypt.Decode(csPassword);
        }
        tp.csDirectory   = GetProfileString(_T("Channel Directory"), _T("/"));

        Close();
    }
}

void CRegistry::LoadMediaTransferParam(TransferParam &tp, CString pszKey) {
    CCrypto crypt;
    if (Open(pszKey) == ERROR_SUCCESS) {
        tp.csUrl         = GetProfileString(_T("Media URL"), _T(""));
        tp.iService      = GetProfileInt(_T("Media Service"), UPLOAD_FTP);   
        tp.csServer      = GetProfileString(_T("Media Server"), _T(""));
        tp.nPort         = GetProfileInt(_T("Media Port"), DEFAULT_FTP_PORT);   
        tp.csUsername    = GetProfileString(_T("Media Username"), _T(""));
        tp.bSavePassword = GetProfileInt(_T("Media SavePassword"), TRUE);   
        if (tp.bSavePassword) {
            CString csPassword = GetProfileString(_T("Media Password"), _T(""));
            tp.csPassword = crypt.Decode(csPassword);
        }
        tp.csDirectory   = GetProfileString(_T("Media Directory"), _T("/"));

        Close();
    }
}

void CRegistry::SaveTransferParam(TransferParam tp, CString pszKey) {
    CCrypto crypt;
    CString csPassword;
    if (Create(pszKey) == ERROR_SUCCESS) {
        //Uploader & Media
        WriteProfileInt(_T("Service"), tp.iService);   
        WriteProfileString(_T("Server"), tp.csServer);
        WriteProfileInt(_T("Port"), tp.nPort);   
        WriteProfileString(_T("Username"), tp.csUsername);
        WriteProfileInt(_T("SavePassword"), tp.bSavePassword);   
        if (tp.bSavePassword) {
            csPassword = crypt.Encode(tp.csPassword);
            WriteProfileString(_T("Password"), csPassword);
        } else {
            csPassword.Empty();
            WriteProfileString(_T("Password"), csPassword);
        }

        WriteProfileString(_T("Directory"), tp.csDirectory);

        Close();
    }
}

void CRegistry::SaveChannelTransferParam(TransferParam tp, CString pszKey) {
    CCrypto crypt;
    CString csPassword;
    if (Create(pszKey) == ERROR_SUCCESS) {
        WriteProfileInt(_T("Channel Service"), tp.iService);   
        WriteProfileString(_T("Channel Server"), tp.csServer);
        WriteProfileInt(_T("Channel Port"), tp.nPort);   
        WriteProfileString(_T("Channel Username"), tp.csUsername);
        WriteProfileInt(_T("Channel SavePassword"), tp.bSavePassword);   
        if (tp.bSavePassword) {
            csPassword = crypt.Encode(tp.csPassword);
            WriteProfileString(_T("Channel Password"), csPassword);
        } else {
            csPassword.Empty();
            WriteProfileString(_T("Channel Password"), csPassword);
        }

        WriteProfileString(_T("Channel Directory"), tp.csDirectory);
        WriteProfileString(_T("Channel URL"), tp.csUrl);

        Close();
    }
}

void CRegistry::SaveMediaTransferParam(TransferParam &tp, CString pszKey) {
    CCrypto crypt;
    CString csPassword;
    if (Open(pszKey) == ERROR_SUCCESS) {
        WriteProfileInt(_T("Media Service"), tp.iService);   
        WriteProfileString(_T("Media Server"), tp.csServer);
        WriteProfileInt(_T("Media Port"), tp.nPort);   
        WriteProfileString(_T("Media Username"), tp.csUsername);
        WriteProfileInt(_T("Media SavePassword"), tp.bSavePassword);   
        if (tp.bSavePassword) {
            csPassword = crypt.Encode(tp.csPassword);
            WriteProfileString(_T("Media Password"), csPassword);
        } else {
            csPassword.Empty();
            WriteProfileString(_T("Media Password"), csPassword);
        }

        WriteProfileString(_T("Media Directory"), tp.csDirectory);
        WriteProfileString(_T("Media URL"), tp.csUrl);

        Close();
    }
}