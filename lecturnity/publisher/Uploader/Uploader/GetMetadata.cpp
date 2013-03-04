// GetMetadata.cpp: implementation of the CGetMetadata class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Uploader.h"
#include "GetMetadata.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGetMetadata::CGetMetadata() {
}

CGetMetadata::~CGetMetadata() {
}

CString CGetMetadata::GetElement(LPTSTR strElement) {
    MSXML2::IXMLDOMNodeListPtr pIDOMNodeList;
    MSXML2::IXMLDOMNodePtr pIDOMNode;

    long value;
    BSTR bstrItemText;
    CString csRes = _T("");

    pIDOMNodeList = docPtr->getElementsByTagName(strElement);
    hr = pIDOMNodeList->get_length(&value);
    if (SUCCEEDED(hr)) {
        pIDOMNodeList->reset();
        for (int ii = 0; ii < value; ii++) {
            pIDOMNodeList->get_item(ii, &pIDOMNode);
            if (pIDOMNode) {
                pIDOMNode->get_text(&bstrItemText);
                csRes = CString(bstrItemText);
                break;
            }
        }
    }
    return csRes;
}

BOOL CGetMetadata::Parse(LPCTSTR tstrXmlFile, BOOL bDisplayError) {
    BOOL bResult = FALSE;

    try {
        //init
        hr = docPtr.CreateInstance(_T("msxml2.domdocument"));
        if (!SUCCEEDED(hr)) {
            return FALSE;
        }
        docPtr->async = FALSE;
        // load a document
        _variant_t varXml(tstrXmlFile);
        _variant_t varOut((bool)TRUE);

        if (docPtr->load(varXml) == FALSE) {
            if (bDisplayError) {
                ErrorMessage();
            }
            return FALSE;
        }

        m_tmMetadata.csTitle       = GetElement(_T("title"));
        m_tmMetadata.csSubtitle    = GetElement(_T("itunes:subtitle"));
        m_tmMetadata.csAuthor      = GetElement(_T("itunes:author"));
        m_tmMetadata.csDescription = GetElement(_T("description"));
        m_tmMetadata.csSummary     = GetElement(_T("itunes:summary"));
        m_tmMetadata.csLanguage    = GetElement(_T("language"));
        m_tmMetadata.csCopyright   = GetElement(_T("copyright"));
        m_tmMetadata.csOwner       = GetElement(_T("itunes:name"));
        m_tmMetadata.csEmail       = GetElement(_T("itunes:email"));
        m_tmMetadata.csLink        = GetElement(_T("link"));
        m_tmMetadata.csWebmaster   = GetElement(_T("webMaster"));
        m_tmMetadata.csExplicit    = GetElement(_T("itunes:explicit"));
        m_tmMetadata.csKeywords    = GetElement(_T("itunes:keywords"));

        bResult = TRUE;
    } catch (...) {
        ErrorMessage();
        bResult = FALSE;
    }

    return bResult;
}

void CGetMetadata::ErrorMessage() {
    CString csFormat, csErrorMessage;
    MSXML2::IXMLDOMParseErrorPtr errPtr = docPtr->GetparseError();
    if (errPtr->line == 0) {
        csErrorMessage.LoadString(IDS_ERRPARSE);
    } else {
        _bstr_t bstrErr(errPtr->reason);         
        csFormat.LoadString(IDS_ERRPARSE2);
        csErrorMessage.Format(csFormat,
            errPtr->errorCode, errPtr->line, errPtr->linepos, (char*)bstrErr);
    }
    AfxMessageBox(csErrorMessage, MB_ICONSTOP);
}

TransferMetadata CGetMetadata::GetMetadata() {
    return m_tmMetadata;
}