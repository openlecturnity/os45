// SetMetadata.cpp: implementation of the CSetMetadata class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Uploader.h"
#include "SetMetadata.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSetMetadata::CSetMetadata() {
}

CSetMetadata::~CSetMetadata() {
}

void CSetMetadata::SetElement(LPTSTR strElement, LPCTSTR strValue) {
    MSXML2::IXMLDOMNodeListPtr pIDOMNodeList;
    MSXML2::IXMLDOMNodePtr pIDOMNode;

    long value;

    pIDOMNodeList = docPtr->getElementsByTagName(strElement);
    hr = pIDOMNodeList->get_length(&value);
    if (SUCCEEDED(hr)) {
        pIDOMNodeList->reset();
        for (int ii = 0; ii < value; ii++) {
            pIDOMNodeList->get_item(ii, &pIDOMNode);
            // only the first node should be modified
            pIDOMNodeList->get_item(ii, &pIDOMNode);
            if (pIDOMNode) {
                _variant_t v(strValue);
                pIDOMNode->put_text(v.bstrVal);
                return;
            }
        }
    }
}

BOOL CSetMetadata::Parse(LPCTSTR tstrXmlFile, LPCTSTR tstrXmlTmpFile) {
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
            ErrorMessage();
            return bResult;
        }

        SetElement(_T("title"), m_tmMetadata.csTitle);
        SetElement(_T("itunes:subtitle"), m_tmMetadata.csSubtitle);
        SetElement(_T("itunes:author"), m_tmMetadata.csAuthor);
        SetElement(_T("description"), m_tmMetadata.csDescription);
        SetElement(_T("itunes:summary"), m_tmMetadata.csSummary);
        SetElement(_T("language"), m_tmMetadata.csLanguage);
        SetElement(_T("copyright"), m_tmMetadata.csCopyright);
        SetElement(_T("itunes:name"), m_tmMetadata.csOwner);
        SetElement(_T("itunes:email"), m_tmMetadata.csEmail);
        SetElement(_T("link"), m_tmMetadata.csLink);
        SetElement(_T("webMaster"), m_tmMetadata.csWebmaster);
        SetElement(_T("itunes:explicit"), m_tmMetadata.csExplicit);
        SetElement(_T("itunes:keywords"), m_tmMetadata.csKeywords);

        _variant_t varXmlFileOut(tstrXmlTmpFile);
        docPtr->documentElement->normalize();
        hr = docPtr->save(varXmlFileOut);
        bResult = SUCCEEDED(hr);

    }
    catch (...) {
        ErrorMessage();
        bResult = FALSE;
    }

    return bResult;
}

void CSetMetadata::ErrorMessage() {
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

void CSetMetadata::SaveMetadata(TransferMetadata tmMetadata) {
    m_tmMetadata = tmMetadata;
}