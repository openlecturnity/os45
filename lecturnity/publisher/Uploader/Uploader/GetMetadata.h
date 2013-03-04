// GetMetadata.h: interface for the CGetMetadata class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETMETADATA_H__A3CA71FF_6DC6_4163_A45D_EBE822D3977E__INCLUDED_)
#define AFX_GETMETADATA_H__A3CA71FF_6DC6_4163_A45D_EBE822D3977E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "msxml3.dll"
using namespace MSXML2;

class CGetMetadata {
    MSXML2::IXMLDOMDocumentPtr docPtr;

    HRESULT hr;
    CString GetElement(LPTSTR strElement);
    void ErrorMessage();
private:
    TransferMetadata m_tmMetadata;

public:
    CGetMetadata();
    virtual ~CGetMetadata();
    BOOL Parse(LPCTSTR tstrXmlFile, BOOL bDisplayError = TRUE);
    TransferMetadata GetMetadata();

};

#endif // !defined(AFX_GETMETADATA_H__A3CA71FF_6DC6_4163_A45D_EBE822D3977E__INCLUDED_)
