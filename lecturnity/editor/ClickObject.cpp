#include "stdafx.h"
#include "ClickObject.h"

// CClickObject

IMPLEMENT_DYNCREATE(CClickObject, CObject)
CClickObject::CClickObject(void) {
    m_pLcElementInfo = NULL;
}

CClickObject::~CClickObject(void) {
    if (m_pLcElementInfo != NULL)
        delete m_pLcElementInfo;
}

void CClickObject::Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing) {
    return;
}

HRESULT CClickObject::Init(CInteractionArea *pInteraction, CEditorProject *pProject) {
    HRESULT hr = S_OK;

    m_pEditorProject = pProject;
    m_rcDimensions = pInteraction->GetActivityArea();

    CString csVisibility = pInteraction->GetVisibility();
    if (csVisibility.GetLength() > 0) {
        hr = m_Visibility.Parse(csVisibility);
    }

    m_pLcElementInfo = pInteraction->GetLcElementInfo();

    return hr;
}

HRESULT CClickObject::Init(CLcElementInfo *pLcElementInfo, CEditorProject *pProject) {
    HRESULT hr = S_OK;
    m_pEditorProject = pProject;
    m_rcDimensions = pLcElementInfo->GetArea();
    CString csVisibility = pLcElementInfo->GetVisibility();
    if (csVisibility.GetLength() > 0) {
        hr = m_Visibility.Parse(csVisibility);
    }
    m_pLcElementInfo = pLcElementInfo->Copy();
    return hr;
}

HRESULT CClickObject::Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
        CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
        LPCTSTR tszExportPrefix, CEditorProject *pProject)
{
    HRESULT hr = S_OK;

    _TCHAR *tszIdent = GetSubClassIdent();
    _TCHAR *tszGOType = _T("");

    tszIdent = _T("CLICK");/*_T("BUTTON")*/;
    
    static _TCHAR tszOutput[2000]; // hopefully enough
    tszOutput[0] = 0;

    _stprintf(tszOutput, _T("<%s x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\""),
        tszIdent,
        m_rcDimensions.left, m_rcDimensions.top, m_rcDimensions.Width(), m_rcDimensions.Height());

    if (SUCCEEDED(hr)) {
        hr = pOutput->Append(tszOutput);
    }

    if (m_Visibility.GetPeriodId() != INTERACTION_PERIOD_NEVER && SUCCEEDED(hr)) {
        pOutput->Append(_T(" visible=\""));
        hr = m_Visibility.AppendTimes(pOutput);
        pOutput->Append(_T("\""));

        pOutput->Append(_T(" active=\""));
        hr = m_Visibility.AppendTimes(pOutput);
        pOutput->Append(_T("\""));
    }

    pOutput->Append(_T(">\n"));

    CString csIds;
    csIds.Format(_T("<IDs acc=\"%s\" UIAuto=\"%s\" UiAutoTree=\"%s\" SAP=\"%s\"></IDs>\n"),
        m_pLcElementInfo->GetIdAcc(), m_pLcElementInfo->GetIdUiAuto(), m_pLcElementInfo->GetIdUiAutoTree(), m_pLcElementInfo->GetIdSap());
    pOutput->Append(csIds);

    // currently not used
    //CString csTimestamp;
    //csTimestamp.Format(_T("<Timestamp>%s</Timestamp>\n"), m_pLcElementInfo->GetTimestamp());
    //pOutput->Append(csTimestamp);

    CString csBounds;
    csBounds.Format(_T("<Bounds top=\"%d\" left=\"%d\" height=\"%d\" width=\"%d\"></Bounds>\n"),
        m_pLcElementInfo->GetPtLocation().y, m_pLcElementInfo->GetPtLocation().x, m_pLcElementInfo->GetPtSize().y, m_pLcElementInfo->GetPtSize().x);
    pOutput->Append(csBounds);

    CString csName;
    csName.Format(_T("<Name>%s</Name>\n"), m_pLcElementInfo->GetName());
    pOutput->Append(csName);

    CString csProcessName;
    csProcessName.Format(_T("<ProcessName>%s</ProcessName>\n"), m_pLcElementInfo->GetProcessName());
    pOutput->Append(csProcessName);

    CString csAccType;
    csAccType.Format(_T("<AccType>%s</AccType>\n"), m_pLcElementInfo->GetAccType());
    pOutput->Append(csAccType);

    CString csSapType;
    csSapType.Format(_T("<SapType>%s</SapType>\n"), m_pLcElementInfo->GetSapType());
    pOutput->Append(csSapType);

    pOutput->Append(_T("</CLICK>\n"));

    return hr;
}

HRESULT CClickObject::CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey) {
    HRESULT hr = CInteractionAreaEx::CloneTo(pTarget, bCopyHashKey);

    if (SUCCEEDED(hr)) {
        if (pTarget->GetClassId() != GetClassId()) {
            return E_INVALIDARG;
        }
        CClickObject *pOther = (CClickObject *)pTarget;
        pOther->m_pEditorProject = m_pEditorProject;
        pOther->m_pLcElementInfo = m_pLcElementInfo->Copy();/*new LCElementInfo;
        pOther->m_pLcElementInfo->m_csName = m_pLcElementInfo->m_csName;
        pOther->m_pLcElementInfo->m_csProcessName = m_pLcElementInfo->m_csProcessName;
        pOther->m_pLcElementInfo->m_csTimeStamp = m_pLcElementInfo->m_csTimeStamp;
        pOther->m_pLcElementInfo->m_ptLocation = m_pLcElementInfo->m_ptLocation;
        pOther->m_pLcElementInfo->m_ptSize = m_pLcElementInfo->m_ptSize;
        pOther->m_pLcElementInfo->m_csElementType = m_pLcElementInfo->m_csElementType;

        pOther->m_pLcElementInfo->m_csIdAcc = m_pLcElementInfo->m_csIdAcc;
        pOther->m_pLcElementInfo->m_csIdUiAuto = m_pLcElementInfo->m_csIdUiAuto;
        pOther->m_pLcElementInfo->m_csIdUiAutoTree = m_pLcElementInfo->m_csIdUiAutoTree;
        pOther->m_pLcElementInfo->m_csIdSap = m_pLcElementInfo->m_csIdSap;*/

    }
    return hr;
}