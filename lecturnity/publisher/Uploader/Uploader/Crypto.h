// Crypto.h: interface for the CCrypto class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRYPTO_H__96BD93A9_565B_4D27_B777_79216115B820__INCLUDED_)
#define AFX_CRYPTO_H__96BD93A9_565B_4D27_B777_79216115B820__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCrypto {
    // a binary key
    BYTE m_aKey[8];
public:
    CString Encode(CString csString);
    CString Decode(CString csString);
    CCrypto();
    virtual ~CCrypto();

};

#endif // !defined(AFX_CRYPTO_H__96BD93A9_565B_4D27_B777_79216115B820__INCLUDED_)
