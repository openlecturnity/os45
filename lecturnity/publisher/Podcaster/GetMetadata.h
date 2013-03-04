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

class CGetMetadata  
{
   MSXML2::IXMLDOMDocumentPtr docPtr;
   MSXML2::IXMLDOMElementPtr ElementPtr;
   MSXML2::IXMLDOMNodeListPtr childList;
	
	HRESULT hr;
   CString GetElement(LPTSTR strElement);
   void ErrorMessage();
public:
   CString csTitle;
   CString csSubtitle;
	CString csAuthor;
   CString csDescription;
	CString csSummary;
  	CString csLanguage;
	CString csCopyright;
  	CString csOwner;
	CString csEMail;
  	CString csLink;
	CString csWebMaster;
  	CString csExplicit;
	CString csKeywords;
public:
	CGetMetadata();
	virtual ~CGetMetadata();
	BOOL Parse(LPCTSTR tstrXmlFile, BOOL bDisplayError = TRUE);

};

#endif // !defined(AFX_GETMETADATA_H__A3CA71FF_6DC6_4163_A45D_EBE822D3977E__INCLUDED_)
