#include "StdAfx.h"

#include "SgmlParser.h"
#include "MiscFunctions.h"

#include "lutils.h"

#define NEWLINE _T('\r') // 13
#define RETURN  _T('\n') // 10

/*********************************************
*********************************************/

/* REVIEW UK
 * Memory management of SgmlElement strange:
 *  - SgmlElement::GetElements() empties parameter array: unexpected side effect
 *  - SgmlElement::RemoveAllElements() deletes the elements: ok
 *  - SgmlElement::SetElements() removes all existing which is a double code to RemoveAllElements
 *  - SgmlElement::SetElements() simply copies pointers: this requires the objects
 *     to be created elsewhere and then deleted here: unexpected side effect
 */

Attribute::Attribute() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Init();
}

Attribute::~Attribute() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Clear();
}

void Attribute::Init() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

void Attribute::Clear() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_csName.Empty();
    m_csValue.Empty();
}

Attribute* Attribute::Copy() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Attribute* pCopyAttribute = new Attribute();

    if ( pCopyAttribute != NULL ) {
        pCopyAttribute->m_csName = m_csName;
        pCopyAttribute->m_csValue = m_csValue;
    }

    return pCopyAttribute;
}

bool Attribute::GetBoolValue() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bValue = false;
    if (m_csValue.GetLength() > 0 && _tcscmp(m_csValue, _T("true")) == 0)
        bValue = true;

    return bValue;
}

int Attribute::GetIntValue() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    int iValue = 0;

    if (m_csValue.GetLength() > 0)
        iValue = _ttoi((LPCTSTR)m_csValue);

    return iValue;
}

double Attribute::GetDoubleValue() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    double dValue = 0;

    if (m_csValue.GetLength() > 0)
        dValue = _tstof((LPCTSTR)m_csValue);

    return dValue;
}

COLORREF Attribute::GetColorValue() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bTransparent;
    COLORREF value = GetColorValue(bTransparent);

    return value;
}

COLORREF Attribute::GetColorValue(bool &bTransparent) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    COLORREF clrValue = RGB(255, 255, 255);

    if (m_csValue.GetLength() > 0)
        clrValue = StringManipulation::GetColorref(m_csValue, bTransparent);

    return clrValue;
}

Gdiplus::ARGB Attribute::GetARGBValue() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Gdiplus::ARGB argbValue = StringManipulation::GetARGB(m_csValue);

    return argbValue;
}

/*********************************************
*********************************************/

SgmlElement::SgmlElement() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Init();
}

SgmlElement::~SgmlElement() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Clear();
}

void SgmlElement::Init() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_csName.Empty();
    m_csParameter.Empty();

    m_bIsClosed         = false;
    m_bIsParameterList  = false;
    m_bIsNotInitialized = true;
    m_bUseOneLine      = true;

    m_pParent = NULL;

    m_aAttributes.RemoveAll();
    m_aElements.RemoveAll();
}

SgmlElement* SgmlElement::Copy() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    SgmlElement* pCopyElement = new SgmlElement();
    if (pCopyElement != NULL) {
       pCopyElement->m_csName             = m_csName;       
       pCopyElement->m_csParameter.Add( m_csParameter.GetStringBuffer() ); // copy CStringBuffer
       pCopyElement->m_bIsClosed          = m_bIsClosed ;
       pCopyElement->m_bIsParameterList   = m_bIsParameterList;
       pCopyElement->m_bIsNotInitialized  = m_bIsNotInitialized;
       pCopyElement->m_bUseOneLine        = m_bUseOneLine;
      
       // copy attributes
       for (int i = 0; i < m_aAttributes.GetCount(); i++)
           pCopyElement->m_aAttributes.Add( m_aAttributes.GetAt(i)->Copy() );
       // copy sub elements
       for (int i = 0; i < m_aElements.GetCount(); i++)
           pCopyElement->Append( ((SgmlElement*)m_aElements.GetAt(i))->Copy() );

       // leave parent undefinded
       pCopyElement->m_pParent = NULL;
    }

    return pCopyElement;
}

void SgmlElement::Clear() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    RemoveAllElements(true);
    // empty or reset member variables
    m_csName.Empty();
    m_csParameter.Empty();

    m_bIsClosed         = false;
    m_bIsParameterList  = false;
    m_bIsNotInitialized = true;
    m_bUseOneLine      = true;

    m_pParent = NULL;

    // Delete all list members
    for (int i = 0; i < m_aAttributes.GetCount(); ++i) {
        Attribute *pAttribute = m_aAttributes[i];
        if (pAttribute != NULL)
            delete pAttribute;
    }
    m_aAttributes.RemoveAll();

}

void SgmlElement::SetParameter(LPCTSTR newParameter) {
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   
   m_csParameter.Empty();
   m_csParameter.Add(newParameter);
}

void SgmlElement::AppendChar(const char newChar, bool bIsParameter, bool bIsMasked) {
   AppendTChar(newChar & 0xff, bIsParameter, bIsMasked);
}

void SgmlElement::AppendTChar(const _TCHAR newChar, bool bIsParameter, bool bIsMasked) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    static bool s_bIsBlank = false;
    static bool s_bIsEqual = false;
    static bool s_bHasName = false;
    static bool s_bIsGroup = false;
    static CStringBuffer s_szBuf;
    static Attribute *s_pLastAttribute = NULL;

    if (m_bIsNotInitialized) {
        s_bIsBlank = false;
        s_bIsEqual = false;
        s_bHasName = false;
        s_bIsGroup = false;
        s_szBuf.Empty();
        s_pLastAttribute = NULL;
        m_bIsNotInitialized = false;
    }

    if (bIsParameter) {
        if (m_csParameter.IsEmpty() && newChar == _T(' ') && (_tcscmp(GetName(), _T("TEXTITEM")) != 0))
            return;
        m_csParameter.Add(newChar);
    } else {
        if (!bIsMasked && s_bIsBlank && newChar == _T(' '))
            return;

        if (!bIsMasked && newChar == _T('\"')) {
            if (s_bIsGroup)
                s_bIsGroup = false;
            else
                s_bIsGroup = true;
            return;
        }

        if (!bIsMasked && !s_bIsGroup && newChar == _T(' '))
            s_bIsBlank = true;
        else
            s_bIsBlank = false;

        if (!bIsMasked && !s_bIsGroup && newChar == _T('='))
            s_bIsEqual = true;
        else
            s_bIsEqual = false;

        if (s_bIsBlank) {
            if (!s_bHasName) {
                m_csName = s_szBuf.GetStringBuffer();
                s_bHasName = true;
            } else {
                if (s_pLastAttribute != NULL)
                    s_pLastAttribute->SetValue(s_szBuf.GetStringBuffer());
            }
            s_szBuf.Empty();
            s_pLastAttribute = new Attribute();
            m_aAttributes.Add(s_pLastAttribute);
        } else if (s_bIsEqual) {
            if (s_pLastAttribute) {
                s_pLastAttribute->SetName(s_szBuf.GetStringBuffer());
            }
            s_szBuf.Empty();
        } else if (!bIsMasked && newChar == _T('>')) {
            if (!s_bHasName) {
                m_csName = s_szBuf.GetStringBuffer();
                s_bHasName = true;
            }
            else if (s_pLastAttribute) {
                s_pLastAttribute->SetValue(s_szBuf.GetStringBuffer());
            }
            s_szBuf.Empty();
        } else {
            s_szBuf.Add(newChar);
        }
    }
}

void SgmlElement::Append(SgmlElement *pElement) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    pElement->SetParent(this);
    m_aElements.Add(pElement);
}

SgmlElement *SgmlElement::Append(LPCTSTR tszName, LPCTSTR tszParameter) {
    HRESULT hr = S_OK;

    SgmlElement *pElement = new SgmlElement();
    if (pElement == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr) && tszName != NULL) {
        pElement->SetName(tszName);
    }
    
    if (SUCCEEDED(hr) && tszParameter != NULL) {
        pElement->SetParameter(tszParameter);
    }

    if (SUCCEEDED(hr)) {
        Append(pElement);
    }

    return pElement;
}

void SgmlElement::InsertAtBegin(SgmlElement *pElement) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    pElement->SetParent(this);
    m_aElements.InsertAt(0, pElement);
}

void SgmlElement::InsertAfter(SgmlElement *pElement, SgmlElement *pPreviousElement) {

    bool bInserted = false;
    if (pPreviousElement != NULL) {
        for (int i = 0; i < m_aElements.GetSize() && !bInserted; ++i) {
            if (m_aElements[i] != NULL && m_aElements[i] == pPreviousElement) {
                ++i;
                pElement->SetParent(this);
                m_aElements.InsertAt(i, pElement);
                bInserted = true;
            }
        }
    }

    if (!bInserted) 
        Append(pElement);
}

void SgmlElement::InsertAfter(CArray<SgmlElement *, SgmlElement *> &aElements, SgmlElement *pPreviousElement) {
    bool bInserted = false;
    if (pPreviousElement != NULL) {
        for (int i = 0; i < m_aElements.GetSize() && !bInserted; ++i) {
            if (m_aElements[i] != NULL && m_aElements[i] == pPreviousElement) {
                ++i;
                for (int j = 0; j < aElements.GetCount(); ++j) {
                    SgmlElement *pElement = aElements[j]; 
                    if (pElement != NULL) {
                        pElement->SetParent(this);
                        m_aElements.InsertAt(i+j, pElement);
                    }
                }
                bInserted = true;
            }
        }
    }

    if (!bInserted) {
        for (int j = 0; j < aElements.GetCount(); ++j) {
            SgmlElement *pElement = aElements[j]; 
            if (pElement != NULL) {
                Append(pElement);
            }
        }
    }
}

// PZI: set attribute with specified name and value to attribute list
// create new attribute if not defined yet
void SgmlElement::SetAttribute(LPCTSTR tszName, LPCTSTR tszValue) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // check if attribute is available
    Attribute *pAttribute = FindAttribute(tszName);
    if (pAttribute == NULL) {
        // attribute not found; create and add new attribute
        pAttribute = new Attribute();
        if (pAttribute == NULL)
            // out of memory
            return;
        m_aAttributes.Add(pAttribute);
    }
    // set attribute
    pAttribute->SetName(tszName);
    pAttribute->SetValue(tszValue);
}

bool SgmlElement::Write(CFileOutput *pOutput, int iLevel) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (pOutput == NULL)
        return false;

    CString csTabString;
    if (iLevel != -1) {
        for (int i = 0; i < iLevel; ++i)
            csTabString += "  ";
    }

    CString csSgmlString;
    CString writeString;
    writeString.Format(_T("%s<%s"), csTabString, m_csName);

    for (int i = 0; i < m_aAttributes.GetSize(); ++i) {
        Attribute *pAttribute = m_aAttributes[i];
        if (pAttribute != NULL) {
            writeString += _T(" ");
            writeString += pAttribute->GetName();
            writeString += _T("=\"");
            StringManipulation::TransformForSgml(pAttribute->GetValue(), csSgmlString);
            writeString += csSgmlString;
            writeString += _T("\"");
        }
    }
    writeString += _T(">");

    pOutput->Append(writeString);

    if (!m_csParameter.IsEmpty()) {
        writeString.Empty();
        StringManipulation::TransformForSgml(m_csParameter.GetStringBuffer(), csSgmlString);
        if (m_bUseOneLine) {
            writeString.Format(_T("%s"), csSgmlString);
            csTabString.Empty();
        } else {
            writeString.Format(_T("\n%s\n"), csSgmlString);
        }

        pOutput->Append(writeString);
    }

    if (!m_aElements.IsEmpty()) {
        pOutput->Append(_T("\n"));
        for (int i = 0; i < m_aElements.GetSize(); ++i) {
            SgmlElement *pElement = m_aElements[i];
            if (pElement != NULL) {
                if (iLevel != -1)
                    pElement->Write(pOutput, iLevel + 1);
                else
                    pElement->Write(pOutput, iLevel);
            }
        }
    } else  // use only one line
        csTabString.Empty();

    CString csEndTag;
    csEndTag.Format(_T("%s</%s>\n"), csTabString, m_csName);
    pOutput->Append(csEndTag);

    return true;
}

SgmlElement *SgmlElement::Find(const _TCHAR *tszNode, _TCHAR *tszParent) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (m_csName == tszNode) {
        if (tszParent != NULL && m_pParent != NULL) {
            if (_tcscmp(m_pParent->GetName(), tszParent) == 0)
                return this;
        }
        else
            return this;
    }

    SgmlElement *pRetElement = NULL;
    if (!m_aElements.IsEmpty()) {
        for (int i = 0; i < m_aElements.GetSize() && pRetElement == NULL; ++i) {
            SgmlElement *pElement = m_aElements[i];
            if (pElement != NULL) 
                pRetElement = pElement->Find(tszNode, tszParent);
        }
    }

    return pRetElement;
}

void SgmlElement::FindAll(const _TCHAR *tszNodeName, CArray<SgmlElement *, SgmlElement *> &aElements) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (m_csName == tszNodeName)
        aElements.Add(this);

    if (!m_aElements.IsEmpty()) {
        for (int i = 0; i < m_aElements.GetSize(); ++i) {
            SgmlElement *pElement = m_aElements[i];
            if (pElement != NULL)
                pElement->FindAll(tszNodeName, aElements);
        }
    }
}

SgmlElement *SgmlElement::FindElement(CString csNodeName) {
    SgmlElement *pElement = NULL;

    for (int i = 0; i < m_aElements.GetSize() && pElement == NULL; ++i) {
        if (m_aElements[i] != NULL && csNodeName == m_aElements[i]->GetName())
            pElement = m_aElements[i];
    }

    return pElement;
}

HRESULT SgmlElement::GetElementParameter(CString csNodeName, CString &csParameter) {   
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    csParameter.Empty();

    SgmlElement *pElement = FindElement(csNodeName);
    if (pElement == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr))
        csParameter = pElement->GetParameter();

    return hr;
}

HRESULT SgmlElement::SetElementParameter(CString csNodeName, CString csParameter) {   
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    SgmlElement *pElement = FindElement(csNodeName);
    if (pElement == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr))
        pElement->SetParameter(csParameter);

    return hr;
}

void SgmlElement::Dump() {
    TRACE(_T("%s: "), GetName());

    for (int i = 0; i < m_aAttributes.GetSize(); ++i) {
        Attribute *pAttribute = m_aAttributes[i];
        if (pAttribute != NULL)
            TRACE(_T("%s "), pAttribute->GetName());
    }

    TRACE(_T("\n"));

    for (int i = 0; i < m_aElements.GetSize(); ++i) {
        SgmlElement *pElement = m_aElements[i];
        if (pElement != NULL)
            pElement->Dump();
    }
}

LPCTSTR SgmlElement::GetValue(LPCTSTR szName) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Attribute *pAttribute = FindAttribute(szName);
    if (pAttribute != NULL)
        return pAttribute->GetValue();
    else   
        return NULL;
}

bool SgmlElement::GetBoolValue(LPCTSTR szName) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Attribute *pAttribute = FindAttribute(szName);
    if (pAttribute != NULL)
        return pAttribute->GetBoolValue();
    else
        return false;
}

int SgmlElement::GetIntValue(LPCTSTR szName) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Attribute *pAttribute = FindAttribute(szName);
    if (pAttribute != NULL)
        return pAttribute->GetIntValue();
    else
        return 0;
}

double SgmlElement::GetDoubleFallback(LPCTSTR tszName) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Attribute *pAttribute = FindAttribute(tszName);
    if (pAttribute == NULL) {
        int iLen = _tcslen(tszName);
        if (iLen > 1 && tszName[iLen-1] == _T('f')) {
            // use a fallback name (eg "x") if the initial name (eg "xf") is not present
            _TCHAR *tszShortName = new _TCHAR[iLen];
            _tcsncpy(tszShortName, tszName, iLen - 1);
            tszShortName[iLen - 1] = 0;

            pAttribute = FindAttribute(tszShortName);
            delete[] tszShortName;
        }
    }

    if (pAttribute != NULL)
        return pAttribute->GetDoubleValue();
    else
        return 0;
}

COLORREF SgmlElement::GetColorValue(LPCTSTR szName) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bTransparent;
    COLORREF clrValue = GetColorValue(szName, bTransparent);

    return clrValue;
}

COLORREF SgmlElement::GetColorValue(LPCTSTR szName, bool &bTransparent) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Attribute *pAttribute = FindAttribute(szName);
    if (pAttribute != NULL)
        return pAttribute->GetColorValue(bTransparent);
    else
        return RGB(255, 255, 255);
}

Gdiplus::ARGB SgmlElement::GetARGBValue(LPCTSTR szName) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Attribute *pAttribute = FindAttribute(szName);
    if (pAttribute != NULL)
        return pAttribute->GetARGBValue();
    else
        return Gdiplus::Color::MakeARGB(0, 255, 255, 255);
}

void SgmlElement::GetElements(CArray<SgmlElement *, SgmlElement *> &aElements) {
    aElements.RemoveAll();
    if (!m_aElements.IsEmpty())
        aElements.Append(m_aElements);
}

SgmlElement *SgmlElement::GetFirstElement() {
    if (!m_aElements.IsEmpty())
        return m_aElements[0];
    else
        return NULL;
}

void SgmlElement::RemoveElement(SgmlElement *pElement) {
    for (int i = 0; i < m_aElements.GetSize(); ++i) {
        if (m_aElements[i] != NULL && m_aElements[i] == pElement) {
            m_aElements.RemoveAt(i);
            delete pElement;
            break;
        }
    }
}

void SgmlElement::RemoveAllElements(bool bDoDelete) {
    if (bDoDelete) {
        for (int i = 0; i < m_aElements.GetCount(); ++i) {
            SgmlElement *pElement = m_aElements[i];
            if (pElement != NULL)
                delete pElement;
        }
    }

    m_aElements.RemoveAll();
}

void SgmlElement::SetElements(CArray<SgmlElement *, SgmlElement *> &aElements) {
    for (int i = 0; i < m_aElements.GetSize(); ++i) {
        SgmlElement *pElement = m_aElements[i];
        if (pElement != NULL)
            delete pElement;
    }
    m_aElements.RemoveAll();


    for (int i = 0; i < aElements.GetSize(); ++i) {
        Append(aElements[i]);
    }

}

// private
Attribute *SgmlElement::FindAttribute(LPCTSTR szName) {
    for (int i = 0; i < m_aAttributes.GetSize(); ++i) {
        Attribute *pAttribute = m_aAttributes[i];
        if (pAttribute != NULL && _tcscmp(pAttribute->GetName(), szName) == 0)
            return pAttribute;
    }
    return NULL;
}


/*********************************************
*********************************************/

SgmlFile::SgmlFile() {
    Init();
}

SgmlFile::~SgmlFile() {
    Clear();
}

void SgmlFile::Init() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_nDataPointer = 0;
    m_pRoot = NULL;

    m_iBytesInBuffer = 0;
    m_iBufferPointer = 0;

    m_csFilename.Empty();
}

void SgmlFile::Clear() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_nDataPointer = 0;

    if (m_pRoot)
        delete m_pRoot;
    m_pRoot = NULL;

    m_iBytesInBuffer = 0;
    m_iBufferPointer = 0;
}

bool SgmlFile::Read(LPCTSTR tszFilename, _TCHAR *tszIdentifier) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return Read(tszFilename, NULL, 0, tszIdentifier);
}

bool SgmlFile::ReadMemory(BYTE *aSgmlData, int iSgmlBufferSize, _TCHAR *tszIdentifier) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return Read(NULL, aSgmlData, iSgmlBufferSize, tszIdentifier);
}

bool SgmlFile::Read(LPCTSTR tszFilename, BYTE *aSgmlData, int iSgmlArraySize, _TCHAR *tszIdent) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    Clear();

    bool bFileGiven = tszFilename != NULL;

    if (!bFileGiven && (aSgmlData == NULL || iSgmlArraySize <= 0))
        return false;

    HANDLE hFile = INVALID_HANDLE_VALUE;
    int iSgmlBufferPointer = 0;

    if (bFileGiven) {
        hFile = ::CreateFile(tszFilename, GENERIC_READ, FILE_SHARE_READ, NULL, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

        if (hFile == INVALID_HANDLE_VALUE)
            return false;
    }

    bool bSuccess = true;
    BYTE aBom[3];
    ZeroMemory(aBom, 3);
    if (bFileGiven) {
        DWORD dwRead = 0;
        bSuccess = ::ReadFile(hFile, aBom, 3, &dwRead, NULL) == TRUE;

        if (!bSuccess)
            return false;
    } else {
        if (iSgmlArraySize > 2) {
            aBom[0] = aSgmlData[0];
            aBom[1] = aSgmlData[1];
            aBom[2] = aSgmlData[2];

            iSgmlBufferPointer = 3;
        }
    }

    UINT nCodepage = CP_ACP;

    if (bSuccess) {
        bool bBomPresent = false;

        if (aBom[0] == 0xef && aBom[1] == 0xbb && aBom[2] == 0xbf) {
            nCodepage = CP_UTF8;
            bBomPresent = true;
        }

        if (!bBomPresent) {
            if (bFileGiven)
                ::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
            else
                iSgmlBufferPointer = 0;
        }
    }

    bool bFirstLine = true;
    if (bSuccess) {
        CString csLine;

        bool isMasked    = false;
        bool isBeginTag  = false;
        bool isEndTag    = true;
        bool isParameter = false;
        SgmlElement *actTag = NULL;

        do {
            bool bSomethingRead = false;
            if (bFileGiven)
                bSomethingRead = ReadLine(hFile, csLine, nCodepage);
            else
                bSomethingRead = ReadLine(aSgmlData, iSgmlArraySize, &iSgmlBufferPointer, csLine, nCodepage);

            if (!bSomethingRead)
                break;


            if (bFirstLine) {
                if (tszIdent != NULL) {
                    CString csIdent = tszIdent;

                    if (!StringManipulation::StartsWith(csLine, csIdent)) {
                        // TODO: Should be done properly at the end of the method 
                        if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
                            ::CloseHandle(hFile);


                        return false;
                    }
                }

                bFirstLine = false;
            }

            int iLength = csLine.GetLength();
            for (int i=0; i<iLength; ++i) {
                _TCHAR charBuf = csLine.GetAt(i);
                switch (charBuf) {
                    case _T('<'):
                        if (!isMasked) {
                            isBeginTag = true;
                            isEndTag   = false;
                            isParameter = false;
                            continue;
                        } else if (actTag) {
                            actTag->AppendTChar(charBuf, isParameter, isMasked);
                        }
                        isMasked = false;
                        break;
                    case _T('>'):
                        if (actTag) {
                            if (!isMasked && !isEndTag) {
                                actTag->AppendTChar(charBuf, isParameter, isMasked);
                                actTag->EndParams();
                                isParameter = true;
                            } else if (isMasked) {
                                actTag->AppendTChar(charBuf, isParameter, isMasked);
                            }
                        }
                        isMasked = false;
                        break;
                    case _T('\\'):
                        if (!isMasked)
                            isMasked = true;
                        else if (actTag) {
                            actTag->AppendTChar(charBuf, isParameter, isMasked);
                            isMasked = false;
                        }
                        else
                            isMasked = false;
                        break;
                    case _T('/'):
                        if (actTag != NULL) {
                            if (!isMasked && isBeginTag) {
                                if (actTag && actTag->IsClosed())
                                    actTag = actTag->GetParent();
                                if (actTag)
                                    actTag->Close();
                                isEndTag = true;
                            }
                            else
                                actTag->AppendTChar(charBuf, isParameter, isMasked);
                        }
                        isBeginTag = false;
                        isMasked = false;
                        break; 
                    default:
                        if (isBeginTag) {
                            SgmlElement *newTag = new SgmlElement();

                            if (actTag) {
                                if (actTag->IsClosed()) {
                                    if (actTag->GetParent() != NULL)
                                        actTag->GetParent()->Append(newTag);
                                } else {
                                    actTag->Append(newTag);
                                }
                            }
                            actTag = newTag;
                            if (m_pRoot == NULL)
                                m_pRoot = actTag;
                        }
                        if (actTag && !actTag->IsClosed())
                            actTag->AppendTChar(charBuf, isParameter, isMasked);

                        isMasked   = false;
                        isBeginTag = false;
                        break;
                }
            }

            // This was previously a case of the above switch (_T('\r') || _T('\n'))
            if (actTag && isParameter)
                actTag->UseOneLine(false);
            isMasked = false;
        } while (true);
    }

    ::CloseHandle(hFile);

    m_csFilename = tszFilename;

    return true;
}

bool SgmlFile::ReadNextBuffer(HANDLE hFile, BYTE *aBuffer, int iBufferSize, int *piBytesRead) {
    if (hFile == INVALID_HANDLE_VALUE || hFile == NULL || aBuffer == NULL || iBufferSize <= 0)
        return false;

    DWORD dwRead = 0;
    bool bSuccess = ::ReadFile(hFile, aBuffer, (DWORD)iBufferSize, &dwRead, NULL) == TRUE;

    if (bSuccess) {
        if (piBytesRead != NULL)
            *piBytesRead = (int)dwRead;

        return dwRead > 0;
    }

    return false;  
}

bool SgmlFile::ReadLine(BYTE *aBuffer, int iBufferLength, int *piReadPointer, CString &csLine, UINT nCodepage) {
    bool bSuccess = true;

    if (aBuffer == NULL || iBufferLength <= 0)
        bSuccess = false;

    if (bSuccess && (*piReadPointer >= iBufferLength))
        bSuccess = false;

    CArray<BYTE, BYTE> aLineBytes;
    aLineBytes.SetSize(0, 500);

    bool bBreakFound = false;
    if (bSuccess) {
        bSuccess = GetNextLineFromBuffer(aBuffer, aLineBytes, *piReadPointer, 
            iBufferLength, bBreakFound);
    }

    if (bSuccess && !bBreakFound)
        bSuccess = false;

    if (bSuccess)
        bSuccess = ConvertBufferToString(aLineBytes, csLine, nCodepage);

    return bSuccess;
}

bool SgmlFile::ReadLine(HANDLE hFile, CString &csLine, UINT nCodepage) {
    bool bSuccess = true;

    if (hFile == INVALID_HANDLE_VALUE || hFile == NULL)
        bSuccess = false;

    // The buffer is empty or all bytes are read
    if (bSuccess && 
        (m_iBytesInBuffer == 0 || m_iBufferPointer == m_iBytesInBuffer)) {

            bool bSomethingRead = ReadNextBuffer(hFile, m_aBuffer, 4096, &m_iBytesInBuffer);
            m_iBufferPointer = 0;

            if (!bSomethingRead)
                bSuccess = false;
    }

    CArray<BYTE, BYTE> aLineBytes;
    aLineBytes.SetSize(0, 500);

    bool bBreakFound = false;
    while (bSuccess && !bBreakFound) {

        bSuccess = GetNextLineFromBuffer(m_aBuffer, aLineBytes, m_iBufferPointer, 
            m_iBytesInBuffer, bBreakFound);

        // The end of the line is not in the current buffer
        // read next buffer
        if (!bBreakFound) {
            bool bSomethingRead = ReadNextBuffer(hFile, m_aBuffer, 4096, &m_iBytesInBuffer);
            m_iBufferPointer = 0;

            if (!bSomethingRead)
                bSuccess = false;
        }
    }

    if (!bBreakFound)
        bSuccess = false;

    if (bSuccess)
        bSuccess = ConvertBufferToString(aLineBytes, csLine, nCodepage);

    return bSuccess;
}

bool SgmlFile::GetNextLineFromBuffer(BYTE *pBuffer, CArray<BYTE, BYTE> &aLineBytes, int &iCurrentPos, 
                                     int &iBufferLength, bool &bLineComplete) {
    bool bSuccess = true;

    int i = 0;

    bLineComplete = false;
    for (i = iCurrentPos; i < iBufferLength && !bLineComplete; ++i) {
        if (pBuffer[i] != '\n')
            aLineBytes.Add(pBuffer[i]);
        else
            bLineComplete = true;
    }

    iCurrentPos = i;

    return bSuccess;
}

bool SgmlFile::ConvertBufferToString(CArray<BYTE, BYTE> &aLineBytes, CString &csLine, UINT nCodepage) {
    bool bSuccess = true;

    if (aLineBytes.GetSize() > 0) {
#ifdef _UNICODE
        WCHAR *wszLine = new WCHAR[aLineBytes.GetSize() + 1];
        int iChars = ::MultiByteToWideChar(nCodepage, 0, (const char *)aLineBytes.GetData(), 
            aLineBytes.GetSize(), wszLine, aLineBytes.GetSize());
        wszLine[iChars] = 0;

        // TODO what about the data already being Unicode? And in the wrong order?

        csLine = wszLine;
        delete[] wszLine;
#else
        aLineBytes.Add(0);
        csLine = aLineBytes.GetData();
#endif
    } else 
        // at least a \n was found (empty line)
        csLine.Empty();

    return bSuccess;
}

bool SgmlFile::Write() {
    if (m_csFilename.IsEmpty())
        return false;

    return Write(m_csFilename, false);
}

bool SgmlFile::Write(LPCTSTR tszFilename, bool isObjFile) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HANDLE sgmlFile = CreateFile(tszFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (sgmlFile == INVALID_HANDLE_VALUE) {
        // BUGFIX 5108: do not show error message here
        // MessageBox(NULL, _T("Open"), NULL, MB_OK);
        return false;
    }

    CFileOutput output;
    output.Init(sgmlFile, 0, true);

    int level;
    if (isObjFile)
        level = -1;
    else
        level = 0;

    if (m_pRoot)
        m_pRoot->Write(&output, level);

    output.Flush();
    CloseHandle(sgmlFile);

    m_csFilename = tszFilename;

    return true;

}

SgmlElement * SgmlFile::Find(const _TCHAR *node, _TCHAR *parent) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (m_pRoot)
        return m_pRoot->Find(node, parent);
    else
        return NULL;
}

void SgmlFile::SetRoot(SgmlElement *pRoot) {
    if (m_pRoot)
        delete m_pRoot;

    m_pRoot = pRoot;
}

void SgmlFile::Dump() { 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (m_pRoot != NULL)
        m_pRoot->Dump();
}

// STATIC METHODS
bool SgmlFile::ResolveFileNode(IN SgmlElement *pNode, OUT _TCHAR *tszSourceFile, OUT _TCHAR *tszTargetFile, 
                               IN bool bTemplate, OUT bool *pbIsIndex, IN bool bCreateDirs,
                               IN const _TCHAR *tszSourcePath,
                               IN const _TCHAR *tszTargetPath) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool success = true;

    if (tszSourceFile == NULL ||
        tszTargetFile == NULL ||
        tszTargetPath == NULL)
        return false;

    if (bCreateDirs && NULL == tszTargetPath)
        return false;

    CString csTargetPath(tszTargetPath);
    CString csSourcePath(_T(""));
    if (tszSourcePath)
        csSourcePath = tszSourcePath;

    const _TCHAR *tszFile = pNode->GetParameter();
    const _TCHAR *tszPrefix = pNode->GetValue(_T("prefix"));
    const _TCHAR *tszWriteToRoot = pNode->GetValue(_T("writeToRoot"));
    const _TCHAR *tszIsIndex = pNode->GetValue(_T("isIndex"));

    if (pbIsIndex)
        *pbIsIndex = false;

    bool bWriteToRoot = false;
    if (tszWriteToRoot)
        bWriteToRoot = (_tcscmp(_T("true"), tszWriteToRoot) == 0);
    bool bIsIndex = false;
    if (tszIsIndex)
        bIsIndex = (_tcscmp(_T("true"), tszIsIndex) == 0);


    bool bHasPrefix = false;
    CString csPrefix;
    if (tszPrefix) {
        csPrefix = tszPrefix;
        bHasPrefix = true;
    }

    if (tszFile) {
        CString csSrcFile;
        csSrcFile = tszFile;

        csSrcFile.Replace(_T('/'), _T('\\'));
        int nLastSlash = csSrcFile.ReverseFind(_T('\\'));
        CString csFileName = csSrcFile;
        if (nLastSlash > 0)
        {
            // Has directory/ies
            CString csDirs = csSrcFile.Mid(0, nLastSlash);
            csFileName = csSrcFile.Mid(nLastSlash + 1);
            if (!bWriteToRoot)
            {
                // TODO: this "look if the path is already in the array" algorithm
                // is O(n^2)!

                // 1.7.0: Do not add the directory names to the written files
                // list! This makes GetFile() return the directories also, which
                // is not in the specification! CleanUp() now deletes the entire
                // directory anyways, so there is no need to do this.
                // In case SetTargetDirectory() is used with a non-NULL argument
                // (not used right now), the directories would remain in the
                // target directory.

                /*
                CString csMarker("\\");

                // have a look if the path is more than one (new) diretory deep
                int nFirstSlash = csDirs.Find('\\');
                while (nFirstSlash > 0)
                {
                CString csDirSub = csDirs.Mid(0, nFirstSlash);
                CString csFullPath = m_csTargetPath + csDirSub + csMarker;
                if (FindInArray(m_writtenFiles, csFullPath) < 0)
                {
                m_writtenFiles.Add(csFullPath);
                }
                nFirstSlash = csDirs.Find('\\', nFirstSlash+1);
                }

                // the directory itself
                CString csFullPath = m_csTargetPath + csDirs + csMarker;
                if (FindInArray(m_writtenFiles, csFullPath) < 0)
                {
                m_writtenFiles.Add(csFullPath);
                }
                */

                // this is the "meat line":
                if (bCreateDirs)
                    success = LIo::CreateDirs(csTargetPath, csDirs);
                /*
                if (!success)
                {
                // TPERR_CREATE_DIR
                // config.xml: Beim Erstellen eines Verzeichnisses in '%s' ist ein Fehler aufgetreten.\nIst das Verzeichnis schreibgeschützt?
                // config.xml: Creating a directory in '%s' failed.\nIs the directory write protected?
                MakeErrorMessage(TPERR_CREATE_DIR, m_csTargetPath);
                }
                */
            }

            if (success) {
                if (bHasPrefix) {
                    CString csTmp = csPrefix;

                    if (bTemplate) {
                        // In case of "index" as a file name, remove "index"
                        if (csFileName.Mid(0, 5) == _T("index"))
                            csFileName = csFileName.Mid(5);
                    }

                    csTmp += csFileName;
                    if (bWriteToRoot)
                        csFileName = csTmp;
                    else
                        csFileName = csDirs + CString(_T("\\")) + csTmp;
                } else {
                    CString csTmp = csFileName;
                    if (bWriteToRoot)
                        csFileName = csTmp;
                    else
                        csFileName = csDirs + CString(_T("\\")) + csTmp;
                }
            }
        } else {
            // Document from root
            // In case of "index" as a file name, replace index with prefix
            if (csFileName.Mid(0, 5) == _T("index") && bHasPrefix) {
                csFileName = csFileName.Mid(5);
                csFileName = csPrefix + csFileName;
            }
        }

        if (success) {
            if (bTemplate) {
                // If the file name ends with ".tmpl", remove that
                if (csFileName.Mid(csFileName.GetLength() - 5) == _T(".tmpl"))
                    csFileName = csFileName.Mid(0, csFileName.GetLength() - 5);
            }

            //csSourceFile = m_csSourcePath + csSrcFile;
            CString csSourceFile = csSourcePath + csSrcFile;
            _tcscpy(tszSourceFile, csSourceFile);
            //csTargetFile = m_csTargetPath + csFileName;
            CString csTargetFile = csTargetPath + csFileName;
            _tcscpy(tszTargetFile, csTargetFile);

            if (bIsIndex && pbIsIndex)
                *pbIsIndex = true;
            //m_csIndexFileName = csFileName;

            // printf("%s --> %s\n", csSourceFile, csTargetFile);
        }
    } else {
        success = false;
    }

    return success;
}