// XmlValidator.h: interface for the CXmlValidator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLVALIDATOR_H__57DBA954_AA8C_4DA7_A771_4A577C018821__INCLUDED_)
#define AFX_XMLVALIDATOR_H__57DBA954_AA8C_4DA7_A771_4A577C018821__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "msxml3.dll"
using namespace MSXML2;

class CXmlValidator  
{
   MSXML2::IXMLDOMDocumentPtr docPtr;
   void ErrorMessage();
public:
   BOOL Validate(LPCTSTR tstrXmlFile, LPCTSTR tstrSaveFile = NULL);
	CXmlValidator();
	virtual ~CXmlValidator();

};

#endif // !defined(AFX_XMLVALIDATOR_H__57DBA954_AA8C_4DA7_A771_4A577C018821__INCLUDED_)
