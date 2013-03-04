#include "stdafx.h"
#include "atlenc.h"

#include "Base64.h"

CBase64::CBase64(void)
{
}

CBase64::~CBase64(void)
{
}

LPSTR CBase64::Encode(int iInputLength, LPBYTE lpInputBuffer, int &iNewLength) {
    HRESULT hr = S_OK;

    if (iInputLength == 0 || lpInputBuffer == NULL)
        hr = E_FAIL;

    iNewLength = 0;
    if (SUCCEEDED(hr)) {
        iNewLength = Base64EncodeGetRequiredLength(iInputLength, ATL_BASE64_FLAG_NONE);
        if (iNewLength == 0)
            hr = E_FAIL;
    }

    LPSTR lpOutputBuffer = NULL;
    if (SUCCEEDED(hr)) {
        lpOutputBuffer = (LPSTR)malloc(iNewLength);
        BOOL bResult = Base64Encode(lpInputBuffer, iInputLength, lpOutputBuffer, &iNewLength, ATL_BASE64_FLAG_NONE);
        if (!bResult)
            hr = E_FAIL;
    }

    if (FAILED(hr)) {
        if (lpOutputBuffer != NULL)
            delete lpOutputBuffer;
        lpOutputBuffer = NULL;
    }

    return lpOutputBuffer;
}

HRESULT CBase64::Decode() {
    return S_OK;
}