// Registry.h: interface for the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTRY_H__9A5D8F29_4536_45FC_9D3B_A8F88141DD04__INCLUDED_)
#define AFX_REGISTRY_H__9A5D8F29_4536_45FC_9D3B_A8F88141DD04__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000
#define DEFAULT_FTP_PORT 21

class CRegistry {
    HKEY hKey;
    LONG lResult;
public:
    CRegistry();
    virtual ~CRegistry();

    LONG Create(CString csKey);
    LONG Open(CString csKey);
    LONG Close();
    LONG WriteProfileInt(CString pszSubKey, int iValue);
    LONG WriteProfileString(CString pszSubKey, CString csValue);
    int GetProfileInt(CString pszSubKey, int iDefaultValue);
    CString GetProfileString(CString pszSubKey, CString csDefaultValue);
    void SaveTransferParam(TransferParam tp, CString pszKey = _T(""));
    void LoadTransferParam(TransferParam &tp, CString pszKey = _T(""));

    void SaveChannelTransferParam(TransferParam tp, CString pszKey);
    void SaveMediaTransferParam(TransferParam &tp, CString pszKey);

    void LoadChannelTransferParam(TransferParam &tp, CString pszKey);
    void LoadMediaTransferParam(TransferParam &tp, CString pszKey);
};

#endif // !defined(AFX_REGISTRY_H__9A5D8F29_4536_45FC_9D3B_A8F88141DD04__INCLUDED_)
