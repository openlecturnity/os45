#pragma once
#include "afxwin.h"
#include <shlguid.h>


class CCustomAutoComplete : public IEnumString {
public:
    // Constructors/destructors
    CCustomAutoComplete();
    CCustomAutoComplete(const HKEY p_hRootKey, const CString& p_sSubKey);
    ~CCustomAutoComplete();

private:
    CSimpleArray<CString> m_asList;
    CComPtr<IAutoComplete> m_pac;

    HKEY m_hKey;
    ULONG m_nCurrentElement;
    ULONG m_nRefCount;
    BOOL m_fBound;
    int m_iItemCount;

private:
    // Internal implementation
    void InternalInit();
    BOOL InitStorage(HKEY p_hRootKey, const CString& p_sSubKey);
    BOOL LoadFromStorage();
    BOOL AddToStorage(const CString& p_sName);
    BOOL SetStorageSubkey(HKEY p_hRootKey, const CString& p_sSubKey);
    CString GetRegistryData(CString csName);

public:
    // Implementation
    BOOL Bind(HWND p_hWndEdit, DWORD p_dwOptions = 0, LPCTSTR p_lpszFormatString = NULL);
    VOID Unbind();
    BOOL AddItem(CString& p_sItem);

private:
    //	IUnknown implementation
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);

    //	IEnumString implementation
    STDMETHODIMP Next(ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched);
    STDMETHODIMP Skip(ULONG celt);
    STDMETHODIMP Reset(void);
    STDMETHODIMP Clone(IEnumString** ppenum);
};
