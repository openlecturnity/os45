// XmlValidator.cpp: implementation of the CXmlValidator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Uploader.h"
#include "XmlValidator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlValidator::CXmlValidator() {
    ::CoInitialize(NULL);
}

CXmlValidator::~CXmlValidator() {
    ::CoUninitialize();
}

BOOL CXmlValidator::Validate(LPCTSTR tstrXmlFile, LPCTSTR tstrSaveFile) {
    BOOL bResult = FALSE;

    try {
        //init
        HRESULT hr = docPtr.CreateInstance(_T("msxml2.domdocument"));
        if (!SUCCEEDED(hr)) {
            return FALSE;
        }
        docPtr->async = FALSE;
        docPtr->preserveWhiteSpace = FALSE;
        // load a document
        _variant_t varXml(tstrXmlFile);
        _variant_t varOut((bool)TRUE);
        if (docPtr->load(varXml) == FALSE) {
            ErrorMessage();
            return FALSE;
        }

        // save file
        if (tstrSaveFile != NULL) {
            _variant_t varSaveFile(tstrSaveFile);
            hr = docPtr->save(varSaveFile);        
            bResult = SUCCEEDED(hr);
        } else {
            bResult = TRUE;
        }

        if (bResult == FALSE) {
            ErrorMessage();
        }
    } catch (...) {
        ErrorMessage();
        bResult = FALSE;
    }

    return bResult;
}

void CXmlValidator::ErrorMessage() {
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