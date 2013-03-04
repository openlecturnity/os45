#pragma once

#include "export.h"

class FILESDK_EXPORT CBase64
{
public:
    CBase64(void);
    ~CBase64(void);

public:
    static LPSTR Encode(int iInputLength, LPBYTE lpInputBuffer, int &iNewLength);
    static HRESULT Decode();
};
