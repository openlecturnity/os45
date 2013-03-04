#include "stdafx.h"
#include "CustomAutoComplete.h"

// Constructors/destructors

CCustomAutoComplete::CCustomAutoComplete() {
    InternalInit();
}

CCustomAutoComplete::CCustomAutoComplete(const HKEY p_hRootKey, const CString& p_sSubKey) {
    InternalInit();
    SetStorageSubkey(p_hRootKey, p_sSubKey);
}

CCustomAutoComplete::~CCustomAutoComplete() {
    m_asList.RemoveAll();

    if (m_hKey) {
        ::RegCloseKey(m_hKey);
    }
    if (m_pac) {
        m_pac.Release();
    }
}

BOOL CCustomAutoComplete::SetStorageSubkey(HKEY p_hRootKey, const CString& p_sSubKey) {
    ATLASSERT(p_hRootKey);
    ATLASSERT(p_sSubKey.GetLength());

    if (InitStorage(p_hRootKey, p_sSubKey)) {
        return LoadFromStorage();
    }

    return FALSE;
}

BOOL CCustomAutoComplete::Bind(HWND p_hWndEdit, DWORD p_dwOptions, LPCTSTR p_lpszFormatString) {
    ATLASSERT(::IsWindow(p_hWndEdit));

    if ((m_fBound) || (m_pac)) {
        return FALSE;
    }

    HRESULT hr = S_OK;
    hr = m_pac.CoCreateInstance(CLSID_AutoComplete);

    if (SUCCEEDED(hr)) {
        if (p_dwOptions) {
            CComQIPtr<IAutoComplete2> pAC2(m_pac);

            ATLASSERT(pAC2);

            hr = pAC2->SetOptions(p_dwOptions);			
            pAC2.Release();
        }

        hr = m_pac->Init(p_hWndEdit, this, NULL, p_lpszFormatString);

        if (SUCCEEDED(hr)) {
            m_fBound = TRUE;
            return TRUE;
        }
    }
    return FALSE;
}

VOID CCustomAutoComplete::Unbind() {
    if (!m_fBound) {
        return;
    }

    ATLASSERT(m_pac);

    if (m_pac) {
        m_pac.Release();
        m_fBound = FALSE;
    }
}

BOOL CCustomAutoComplete::AddItem(CString& p_sItem) {
    if (p_sItem.GetLength() != 0) {
        if (m_asList.Find(p_sItem) == -1) {
            CString csPosition = GetRegistryData(_T("Position"));
            int iCount = ::_ttoi(csPosition);

            if (iCount > 7) {
                m_iItemCount = 0;
            } else {
                m_iItemCount = iCount;
            }

            m_asList.SetAtIndex(m_iItemCount, p_sItem);
            m_iItemCount ++;

            if (m_hKey) {
                return AddToStorage(p_sItem);
            }
            return TRUE;
        }
    }
    return FALSE;
}

//	IUnknown implementation

STDMETHODIMP_(ULONG) CCustomAutoComplete::AddRef() {
    return ::InterlockedIncrement(reinterpret_cast<LONG*>(&m_nRefCount));
}

STDMETHODIMP_(ULONG) CCustomAutoComplete::Release() {
    ULONG nCount = 0;
    nCount = (ULONG) ::InterlockedDecrement(reinterpret_cast<LONG*>(&m_nRefCount));
    if (nCount == 0) {
        delete this;
    }
    return nCount;
}

STDMETHODIMP CCustomAutoComplete::QueryInterface(REFIID riid, void** ppvObject) {
    HRESULT hr = E_NOINTERFACE;

    if (ppvObject != NULL) {
        *ppvObject = NULL;
        if (IID_IUnknown == riid) {
            *ppvObject = static_cast<IUnknown*>(this);
        }
        if (IID_IEnumString == riid) {
            *ppvObject = static_cast<IEnumString*>(this);
        }
        if (*ppvObject != NULL) {
            hr = S_OK;
            ((LPUNKNOWN)*ppvObject)->AddRef();
        }
    } else {
        hr = E_POINTER;
    }

    return hr;
}

//	IEnumString implementation

STDMETHODIMP CCustomAutoComplete::Next(ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched) {
    HRESULT hr = E_FAIL;

    if (!celt) {
        celt = 1;
    }
    ULONG i;
    for (i = 0; i < celt; i++) {
        if (m_nCurrentElement == (ULONG)m_asList.GetSize()) {
            break;
        }
        rgelt[i] = (LPWSTR)::CoTaskMemAlloc((ULONG) sizeof(WCHAR) * (m_asList[m_nCurrentElement].GetLength() + 1));
        lstrcpy(rgelt[i], m_asList[m_nCurrentElement]);

        if (pceltFetched) {
            *pceltFetched++;
        }
        m_nCurrentElement++;
    }
    if (i == celt) {
        hr = S_OK;
    }
    return hr;
}

STDMETHODIMP CCustomAutoComplete::Skip(ULONG celt) {
    m_nCurrentElement += celt;
    if (m_nCurrentElement > (ULONG)m_asList.GetSize()) {
        m_nCurrentElement = 0;
    }
    return S_OK;
}

STDMETHODIMP CCustomAutoComplete::Reset(void) {
    m_nCurrentElement = 0;
    return S_OK;
}

STDMETHODIMP CCustomAutoComplete::Clone(IEnumString** ppenum) {
    if (!ppenum) {
        return E_POINTER;
    }

    CCustomAutoComplete* pnew = new CCustomAutoComplete();
    pnew->AddRef();
    *ppenum = pnew;

    return S_OK;
}

// Internal implementation

void CCustomAutoComplete::InternalInit() {
    m_nCurrentElement = 0;
    m_nRefCount = 0;
    m_fBound = FALSE;
    m_hKey = NULL;
    m_iItemCount = 0;
}

BOOL CCustomAutoComplete::InitStorage(HKEY p_hRootKey, const CString& p_sSubKey) {
    LONG lResult = ERROR_SUCCESS;
    SECURITY_ATTRIBUTES sa = {0};

    if (m_hKey) {
        ::RegCloseKey(m_hKey);
        m_hKey = NULL;
    }
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    lResult = ::RegCreateKeyEx(p_hRootKey, p_sSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, &sa, &m_hKey, NULL);

    if ((ERROR_SUCCESS == lResult) && (NULL != m_hKey)) {
        return TRUE;
    }
    return FALSE;
}

BOOL CCustomAutoComplete::LoadFromStorage() {
    ATLASSERT(m_hKey);
    DWORD dwCounter = 0;
    LONG lResult = ERROR_SUCCESS;
    DWORD dwValueNameSize = 0;
    TCHAR szValueName[MAX_PATH];				
    if (m_asList.GetSize() != 0) {
        m_asList.RemoveAll();
    }
    while (ERROR_SUCCESS == lResult) {
        dwValueNameSize = sizeof(szValueName);
        CString dataName;
        lResult = ::RegEnumValue(m_hKey, dwCounter, szValueName, &dwValueNameSize, NULL, NULL, NULL, NULL);

        CString csName = CString(szValueName);
        CString csValue = GetRegistryData(csName);

        if (ERROR_SUCCESS == lResult)
            m_asList.Add(CString(csValue));

        dwCounter++;
    }
    if ((ERROR_SUCCESS == lResult) || (ERROR_NO_MORE_ITEMS == lResult)) {
        return TRUE; 
    }
    return FALSE;
}

BOOL CCustomAutoComplete::AddToStorage(const CString& p_sName) {
    ATLASSERT(m_hKey);

    LONG lResult = 0;

    CString csText;  
    csText.Format(_T("%d"), m_iItemCount);

    lResult = ::RegSetValueEx(m_hKey, (LPCTSTR) csText, 0, REG_SZ, (BYTE*)(LPCTSTR) p_sName, p_sName.GetLength() * sizeof(TCHAR));
    lResult = ::RegSetValueEx(m_hKey, (LPCTSTR) _T("Position"), 0, REG_SZ, (BYTE*)(LPCTSTR) csText, p_sName.GetLength() * sizeof(TCHAR));

    if (ERROR_SUCCESS == lResult) {
        return TRUE;
    }

    return FALSE;
}

CString CCustomAutoComplete::GetRegistryData(CString csName) {
    DWORD dwType = REG_SZ;  
    TCHAR buffer[MAX_PATH];
    DWORD cbData = 2 * MAX_PATH - 1;

    RegQueryValueEx(m_hKey, csName, NULL, &dwType, (BYTE *)buffer, &cbData);
    CString csValue = CString(buffer, (cbData / sizeof(TCHAR)));

    return csValue;
}
