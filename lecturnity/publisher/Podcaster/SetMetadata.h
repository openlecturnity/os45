// GetMetadata.h: interface for the CGetMetadata class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETMETADATA_H__A3CA71FF_6DC6_4163_A45D_EBE822D3977E__INCLUDED_)
#define AFX_SETMETADATA_H__A3CA71FF_6DC6_4163_A45D_EBE822D3977E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "msxml3.dll"
using namespace MSXML2;

class CSetMetadata  
{
	MSXML2::IXMLDOMDocumentPtr docPtr;
   MSXML2::IXMLDOMElementPtr ElementPtr;
   MSXML2::IXMLDOMNodeListPtr childList;
	
	HRESULT hr;
   void SetElement(LPTSTR strElement, LPCTSTR strValue);
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
	CSetMetadata();
	virtual ~CSetMetadata();
	BOOL Parse(LPCTSTR tstrXmlFile, LPCTSTR tstrXmlTmpFile);

};

#endif // !defined(AFX_SETMETADATA_H__A3CA71FF_6DC6_4163_A45D_EBE822D3977E__INCLUDED_)
