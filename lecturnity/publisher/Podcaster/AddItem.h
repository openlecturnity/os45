// AddItem.h: interface for the CAddItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADDITEM_H__2C7F6615_5536_47F2_90C3_FA5ED0022126__INCLUDED_)
#define AFX_ADDITEM_H__2C7F6615_5536_47F2_90C3_FA5ED0022126__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "msxml3.dll"
using namespace MSXML2;

class CAddItem  
{
   MSXML2::IXMLDOMDocumentPtr docPtr;
   MSXML2::IXMLDOMNodePtr root;
   MSXML2::IXMLDOMNodePtr element;

   HRESULT hr;
protected:
   BOOL MoveOnFront(LPCTSTR tstrXmlFileOut);
   BOOL AddElement(MSXML2::IXMLDOMNodePtr item, LPCTSTR tstrTagName, LPCTSTR tstrTagText);
   BOOL AddAttribute(MSXML2::IXMLDOMNodePtr item, LPCTSTR tstrAttributeName, LPCTSTR tstrAttributeText);
   BOOL AddElementAndAttributes(MSXML2::IXMLDOMNodePtr item, LPCTSTR tstrTagName, LPCTSTR tstrTagText);
   BOOL AddSeparator(MSXML2::IXMLDOMNodePtr node, LPCTSTR tstrSeparator);
   void ErrorMessage();
public:
	BOOL Insert(LPCTSTR tstrXmlFileInp, LPCTSTR tstrXmlFileOut);
	CAddItem();
	virtual ~CAddItem();

public:
   CString csTitle;
   CString csAuthor;
   CString csDate;
   // attributes
   CString csLength;
   CString csType;
   CString csUrl;
};

#endif // !defined(AFX_ADDITEM_H__2C7F6615_5536_47F2_90C3_FA5ED0022126__INCLUDED_)
