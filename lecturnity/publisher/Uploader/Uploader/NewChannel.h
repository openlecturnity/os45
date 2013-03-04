// NewChannel.h: interface for the CNewChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWCHANNEL_H__2C7F6615_5536_47F2_90C3_FA5ED0022126__INCLUDED_)
#define AFX_NEWCHANNEL_H__2C7F6615_5536_47F2_90C3_FA5ED0022126__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "msxml3.dll"
using namespace MSXML2;
#include "Uploader.h"

class CNewChannel {
    MSXML2::IXMLDOMDocumentPtr docPtr;
    MSXML2::IXMLDOMProcessingInstructionPtr pi;
    MSXML2::IXMLDOMNodePtr root;
    MSXML2::IXMLDOMNodePtr element; //last 

    HRESULT hr;

protected:
    BOOL AddElement(MSXML2::IXMLDOMNodePtr item, LPCTSTR strTagName, LPCTSTR tstrTagText);
    BOOL AddAttribute(MSXML2::IXMLDOMNodePtr item, LPCTSTR strAttributeName, LPCTSTR tstrAttributeText);
    BOOL AddSeparator(MSXML2::IXMLDOMNodePtr node, LPCTSTR tstrSeparator);
    void ErrorMessage();

public:
    BOOL Create(LPCTSTR tstrXmlFile);
    CNewChannel();
    virtual ~CNewChannel();

private:
    TransferMetadata m_tmMetadata;

public:
    void SetMetadata(TransferMetadata tmMetadata);
};

#endif // !defined(AFX_NEWCHANNEL_H__2C7F6615_5536_47F2_90C3_FA5ED0022126__INCLUDED_)
