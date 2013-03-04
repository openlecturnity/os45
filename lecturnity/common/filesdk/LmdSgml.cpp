#include "stdafx.h"
#include "LmdSgml.h"
#include "SgmlParser.h"

/*
 * static functions
 */

/*
 * create a new page tag with given parameters
 */
SgmlElement* CLmdSgml::CreatePageTag(CString &csPageTitle, int nr, int begin, int end, CString csSgmlPrefix, 
                                     bool bHasAction, int iXcoord, int iYcoord, CString &csButtonType,
                                     LPCTSTR lpszType) {
    HRESULT hr = S_OK;

    SgmlElement *pPageTag = new SgmlElement();
    if (pPageTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        pPageTag->SetName(_T("page"));
    }

    // <title>
    SgmlElement *pTitleTag = NULL;
    if (SUCCEEDED(hr)) {
        pTitleTag = new SgmlElement();
        if (pTitleTag == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        pTitleTag->SetName(_T("title"));
        pTitleTag->SetParameter(csPageTitle);
        pPageTag->Append(pTitleTag);
    }

    // <type>
    CString csType = lpszType;
    if (!csType.IsEmpty() && SUCCEEDED(hr)) {
        SgmlElement *pTypeTag = NULL;
        if (SUCCEEDED(hr)) {
            pTypeTag = new SgmlElement();
            if (pTypeTag == NULL)
                hr = E_FAIL;
        }

        if (SUCCEEDED(hr)) {
            pTypeTag->SetName(_T("type"));
            CString csType = lpszType;
            pTypeTag->SetParameter(csType);
            if (bHasAction) {
                CString csPos;
                csPos.Format(_T("%i"), iXcoord);
                pTypeTag->SetAttribute(_T("x"),csPos);
                csPos.Format(_T("%i"), iYcoord);
                pTypeTag->SetAttribute(_T("y"),csPos);
                pTypeTag->SetAttribute(_T("button"), csButtonType);
            }
            pPageTag->Append(pTypeTag);
        }
    }


    // <nr>
    SgmlElement *pNrTag = NULL;
    if (SUCCEEDED(hr)) {
        pNrTag = new SgmlElement();
        if (pNrTag == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        pNrTag->SetName(_T("nr"));
        CString csNr;
        csNr.Format(_T("%i"), nr);
        pNrTag->SetParameter(csNr);
        pPageTag->Append(pNrTag);
    }

    // <begin>
    SgmlElement *pBeginTag = NULL;
    if (SUCCEEDED(hr)) {
        pBeginTag = new SgmlElement();
        if (pBeginTag == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        pBeginTag->SetName(_T("begin"));
        CString csBegin;
        csBegin.Format(_T("%i"), begin);
        pBeginTag->SetParameter(csBegin);
        pPageTag->Append(pBeginTag);
    }

    // <end>
    SgmlElement *pEndTag = NULL;
    if (SUCCEEDED(hr)) {
        pEndTag = new SgmlElement();
        if (pEndTag == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        pEndTag->SetName(_T("end"));
        CString csEnd;
        csEnd.Format(_T("%i"), end);
        pEndTag->SetParameter(csEnd);
        pPageTag->Append(pEndTag);
    }

    // <thumb>
    SgmlElement *pThumbTag = NULL;
    if (SUCCEEDED(hr)) {
        pThumbTag = new SgmlElement();
        if (pThumbTag == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        pThumbTag->SetName(_T("thumb"));
        CString csThumbFilename;
        csThumbFilename.Format(_T("%ls_%i_thumb.png"), csSgmlPrefix, begin);
        pThumbTag->SetParameter(csThumbFilename);
        pPageTag->Append(pThumbTag);
    }

    if (FAILED(hr) && pPageTag != NULL) {
        delete pPageTag;
        pPageTag = NULL;
    }

    return pPageTag;
}


// Special functions to read/change/write sgml structure of lmd file

CLmdSgml::CLmdSgml(void)
{
}

CLmdSgml::~CLmdSgml(void)
{
    int i = 0;
}

HRESULT CLmdSgml::GetMetaInfo(CString csNodeName, CString &csValue) {
    HRESULT hr = S_OK;

    SgmlElement *pMetaInfoTag = Find(_T("metainfo"), _T("docinfo"));
    if (pMetaInfoTag == NULL)
        hr = E_FAIL;

    csValue.Empty();
    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        pMetaInfoTag->GetElements(aElements);
        for (int i = 0; i < aElements.GetSize(); ++i) {
            SgmlElement *pElement = aElements[i];
            if (pElement != NULL) {
                CString csName = pElement->GetName();
                if (csName == csNodeName) {
                    if (!csValue.IsEmpty())
                        csValue += _T("; ");
                    csValue += pElement->GetParameter();
                }
            }
        }
    }

    return hr;
}

HRESULT CLmdSgml::GetKeywords(CStringArray &aKeywords) {
    HRESULT hr = S_OK;

    SgmlElement *pMetaInfoTag = Find(_T("metainfo"), _T("docinfo"));
    if (pMetaInfoTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        pMetaInfoTag->GetElements(aElements);
        for (int i = 0; i < aElements.GetSize(); ++i) {
            SgmlElement *pElement = aElements[i];
            if (pElement && _tcscmp(pElement->GetName(), _T("keyword")) == 0)
                aKeywords.Add(pElement->GetParameter());
        }
    }

    return hr;
}

HRESULT CLmdSgml::GetAudioInfo(CString csNodeName, CString &csValue) {
    HRESULT hr = S_OK;

    csValue.Empty();

    SgmlElement *pAudioTag = Find(_T("audio"), _T("fileinfo"));
    if (pAudioTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        SgmlElement *pElement = pAudioTag->FindElement(csNodeName);
        if (pElement != NULL)
            csValue = pElement->GetParameter();
    }

    return hr;
}

HRESULT CLmdSgml::GetVideoInfo(CString csNodeName, CString &csValue) {
    HRESULT hr = S_OK;

    csValue.Empty();

    SgmlElement *pVideoTag = Find(_T("video"), _T("fileinfo"));
    if (pVideoTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        SgmlElement *pElement = pVideoTag->FindElement(csNodeName);
        if (pElement != NULL)
            csValue = pElement->GetParameter();
    }

    return hr;
}

HRESULT CLmdSgml::GetAllClips(CArray<SgmlElement *, SgmlElement *> &aClips) {
    HRESULT hr = S_OK;

    SgmlElement *pStructureTag = Find(_T("structure"), _T("docinfo"));
    if (pStructureTag == NULL) 
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        AddElements(pStructureTag, _T("clip"), aClips);
    }

    return hr;
}

HRESULT CLmdSgml::GetAllPages(CArray<SgmlElement *, SgmlElement *> &aPages) {
    HRESULT hr = S_OK;

    SgmlElement *pStructureTag = Find(_T("structure"), _T("docinfo"));
    if (pStructureTag == NULL) 
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        AddElements(pStructureTag, _T("page"), aPages);
    }

    return hr;
}

HRESULT CLmdSgml::ChangeMetaInfo(CString csNodeName, CString csValue) {

    HRESULT hr = S_OK;

    SgmlElement *pMetaInfoTag = Find(_T("metainfo"), _T("docinfo"));
    if (pMetaInfoTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        SgmlElement *pElement = pMetaInfoTag->FindElement(csNodeName);
        if (pElement != NULL) {
            pElement->SetParameter(csValue);
        }
    }

    return hr;
}

HRESULT CLmdSgml::ChangeKeywords(CStringArray &aKeywords) {
    HRESULT hr = S_OK;

    SgmlElement *pMetaInfoTag = Find(_T("metainfo"), _T("docinfo"));
    if (pMetaInfoTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        pMetaInfoTag->GetElements(aElements);
        CArray<SgmlElement *, SgmlElement *> aElementKeywords;
        aElementKeywords.RemoveAll();
        for (int i = 0; i < aElements.GetSize(); ++i) {
            if (aElements[i] != NULL && _tcscmp(aElements[i]->GetName(), _T("keyword")) == 0)
                aElementKeywords.Add(aElements[i]);
        }

        for (int i = 0; i < aElementKeywords.GetSize(); ++i) {
            pMetaInfoTag->RemoveElement(aElementKeywords[i]);
        }
        for (int i = 0; i < aKeywords.GetSize(); ++i) {
            SgmlElement *pKeyword = new SgmlElement();
            pKeyword->SetName(_T("keyword"));
            pKeyword->SetParameter(aKeywords[i]);
            pMetaInfoTag->Append(pKeyword);
        }
    }

    return hr;
}

HRESULT CLmdSgml::ReplaceClipWithPages(SgmlElement *pClipTag, CArray<SgmlElement *, SgmlElement *> &aPages) {
    HRESULT hr = S_OK;

    if (aPages.IsEmpty())
        hr = E_FAIL;

    SgmlElement *pStructure = NULL;
    if (SUCCEEDED(hr)) {
        pStructure = Find(_T("structure"), _T("docinfo"));
        if (pStructure == NULL)
            hr = E_FAIL;
    }

    SgmlElement *pParentTag = NULL;
    if (SUCCEEDED(hr)) {
        pParentTag = pClipTag->GetParent();
        if (pParentTag == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        // adjust end of previous page and add copy as subsequent page
        // note required because normal pages include clips, i.e. start before end end after clip
        SgmlElement *pPageBefore = GetPageBefore(pClipTag);
        if (pPageBefore != NULL) {
            CString csClipBegin;
            pClipTag->GetElementParameter(_T("begin"), csClipBegin);
            CString csClipEnd;
            pClipTag->GetElementParameter(_T("end"), csClipEnd);

            // next page is copy of previous page but starts at clip end
            SgmlElement *pPageAfter = pPageBefore->Copy();
            pPageAfter->SetElementParameter(_T("begin"), csClipEnd);
            pParentTag->InsertAfter(pPageAfter, pClipTag);

            // previous page ends at clip start
            pPageBefore->SetElementParameter(_T("end"), csClipBegin);
        }

        // insert new pages after clip tag
        pParentTag->InsertAfter(aPages, pClipTag);

        // remove the replaced clip tag
        pParentTag->RemoveElement(pClipTag);
    }

    return hr;
}

//// private
HRESULT CLmdSgml::AddElements(SgmlElement *pTag, CString csNodeName, CArray<SgmlElement *, SgmlElement *> &aElements) {
    HRESULT hr = S_OK;

    CArray<SgmlElement *, SgmlElement *> arElements;
    pTag->GetElements(arElements);
    for (int i = 0; i < arElements.GetSize(); ++i) {
        SgmlElement *pElement = arElements[i];
        if (pElement != NULL) {
            if (_tcscmp(pElement->GetName(), csNodeName) == 0) {
                // clip found - compute index
                aElements.Add(pElement); 
            } else if (_tcscmp(pElement->GetName(),_T("structure")) == 0 || 
                       _tcscmp(pElement->GetName(),_T("chapter")) == 0) {
                // find clips in subtree
                // NOTE: can chapters be empty???
                hr = AddElements(pElement, csNodeName, aElements);
            }
        }
    }

    return hr;
}

SgmlElement *CLmdSgml::GetPageBefore(SgmlElement *pClipTag) { 
    HRESULT hr = S_OK;

    SgmlElement *pPageTag = NULL;

    SgmlElement *pParentTag = NULL;
    if (SUCCEEDED(hr)) {
        pParentTag = pClipTag->GetParent();
        if (pParentTag == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aParentElements;
        pParentTag->GetElements(aParentElements);
        for (int i = 0; i < aParentElements.GetSize(); ++i) {
            SgmlElement *pElement = aParentElements[i];
            if (pElement != NULL && pElement == pClipTag) {
                if (i > 0) {
                    --i;
                    pPageTag = aParentElements[i];
                    break;
                }
            }
        }
    }

    return pPageTag;
}
