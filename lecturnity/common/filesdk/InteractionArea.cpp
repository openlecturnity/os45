#include "stdafx.h"
#include "InteractionArea.h"
#include "WhiteboardStream.h" // Note: inclusion is only possible in this cpp file not in the header.
#include "MiscFunctions.h"

CInteractionArea::CInteractionArea()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_rcDimensions = CRect(0,0,0,0);
   m_bHandCursor = false;
   m_csClickAction = _T("");
   m_csDownAction = _T("");
   m_csUpAction = _T("");
   m_csEnterAction = _T("");
   m_csLeaveAction = _T("");

   m_bIsButton = false;
   m_bIsRadio = false;
   m_bIsCheck = false;
   m_bIsField = false;
   m_bIsSupport = false;
   m_bIsCallout = false;
   m_bIsClickObject = false;
   m_bHasClickObject = false;
   m_bIsUserEdited = false;
   m_csRtf = _T("");

   m_nVisibilityStartMs = 0xffffffff;
   m_nVisibilityEndMs = 0xffffffff;

   m_csCorrectText = _T("");
   m_bHasToBeChecked = false;
   
   m_bIsInitialized = false;
   m_bObjectsExtracted = false;
   m_pSupportObject = NULL;
   m_bDemoDocumentObject = false;

   m_pLcElementInfo = NULL;
}

CInteractionArea::~CInteractionArea()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (!m_bObjectsExtracted)
   {
      // we have to delete the DrawObjects here

      int i = 0;
      for (i=0; i<m_aNormalObjects.GetSize(); ++i)
         delete m_aNormalObjects[i];
      for (i=0; i<m_aOverObjects.GetSize(); ++i)
         delete m_aOverObjects[i];
      for (i=0; i<m_aPressedObjects.GetSize(); ++i)
         delete m_aPressedObjects[i];
      for (i=0; i<m_aInactiveObjects.GetSize(); ++i)
         delete m_aInactiveObjects[i];
   }
   // else they were used and (hopefully) deleted elsewhere

   // TODO the above is nasty (heavy side effect). Is there a better solution?

   if (m_pSupportObject != NULL)
      delete m_pSupportObject;
}

HRESULT CInteractionArea::Init(DrawSdk::DrawObject *pSupportObject)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (pSupportObject == NULL)
      return E_POINTER;

   if (!pSupportObject->HasVisibility())
      return E_INVALIDARG;

   //bool bIsChangeableText = pSupportObject->GetType() == DrawSdk::TEXT && ((DrawSdk::Text *)pSupportObject)->HasChangeType();
   //if (!bIsChangeableText && pSupportObject->GetType() != DrawSdk::TARGETPOINT && !pSupportObject->IsDndMoveable())
   //   return E_INVALIDARG;
   
   m_bIsSupport = true;
   m_pSupportObject = pSupportObject->Copy();
   
   m_csVisibility = pSupportObject->GetVisibility();

   CArray<CPoint, CPoint> aVisibilityPeriods;
   GetVisibilityTimes(&aVisibilityPeriods);
   
   if (aVisibilityPeriods.GetSize() > 0)
   {
      m_nVisibilityStartMs = aVisibilityPeriods[0].x;
      m_nVisibilityEndMs = aVisibilityPeriods[0].y;
   }
   
   // TODO: what about precision loss?
   m_rcDimensions = CRect((int)pSupportObject->GetX(), (int)pSupportObject->GetY(),
      (int)pSupportObject->GetWidth(), (int)pSupportObject->GetHeight());

   m_bIsInitialized = true;

   return S_OK;
}

HRESULT CInteractionArea::ReadFromSgml(SgmlElement *pInteractionSgml, CString& csRecordPath) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    const _TCHAR *tszName = pInteractionSgml->GetName();
    if (_tcscmp(tszName, _T("AREA")) != 0 && _tcscmp(tszName, _T("BUTTON")) != 0
        && _tcscmp(tszName, _T("CALLOUT")) != 0 && _tcscmp(tszName, _T("CLICK")) != 0
        && _tcscmp(tszName, _T("RADIO")) != 0 && _tcscmp(tszName, _T("CHECK")) != 0
        && _tcscmp(tszName, _T("FIELD")) != 0)
        return E_IA_INVALID_DATA;

    int x = pInteractionSgml->GetIntValue(_T("x"));
    int y = pInteractionSgml->GetIntValue(_T("y"));
    int w = pInteractionSgml->GetIntValue(_T("width"));
    int h = pInteractionSgml->GetIntValue(_T("height"));

    if (w < 0 || h < 0)
        return E_IA_INVALID_DATA;

    bool bHandCursor = false;

    const _TCHAR *tszCursor = pInteractionSgml->GetValue(_T("cursor"));
    if (tszCursor != NULL && _tcscmp(tszCursor, _T("link")) == 0)
        bHandCursor = true;

    const _TCHAR *tszActionClick = pInteractionSgml->GetValue(_T("onClick"));
    const _TCHAR *tszActionDown = pInteractionSgml->GetValue(_T("onDown"));
    const _TCHAR *tszActionUp = pInteractionSgml->GetValue(_T("onUp"));
    const _TCHAR *tszActionEnter = pInteractionSgml->GetValue(_T("onEnter"));
    const _TCHAR *tszActionLeave = pInteractionSgml->GetValue(_T("onLeave"));

    const _TCHAR *tszVisibility = pInteractionSgml->GetValue(_T("visible"));
    const _TCHAR *tszActivity = pInteractionSgml->GetValue(_T("active"));

    if (SUCCEEDED(hr)) {

        //if (tszVisibility == NULL || tszActivity == NULL)
        //   return E_IA_INVALID_DATA;

        // Note: for normal AREAs you do not need a visibility (they are invisible all
        // the time).
        // For a BUTTON no activity must be given. That is not very useful but it
        // can be inactive all the time.
        // TODO really require no activity?

        m_rcDimensions = CRect(CPoint(x, y), CSize(w, h));

        m_bHandCursor = bHandCursor;

        if (tszActionClick != NULL)
            m_csClickAction = tszActionClick;

        if (tszActionDown != NULL)
            m_csDownAction = tszActionDown;

        if (tszActionUp != NULL)
            m_csUpAction = tszActionUp;

        if (tszActionEnter != NULL)
            m_csEnterAction = tszActionEnter;

        if (tszActionLeave != NULL)
            m_csLeaveAction = tszActionLeave;

        if (tszVisibility != NULL) {
            m_csVisibility = tszVisibility;

            CArray<CPoint, CPoint> aVisibilityPeriods;
            GetVisibilityTimes(&aVisibilityPeriods);

            if (aVisibilityPeriods.GetSize() > 0) {
                m_nVisibilityStartMs = aVisibilityPeriods[0].x;
                m_nVisibilityEndMs = aVisibilityPeriods[0].y;
            }
        }

        if (tszActivity != NULL)
            m_csActivity = tszActivity;
    }

    if (_tcscmp(tszName, _T("BUTTON")) == 0) {
        // parse all sub-objects (different visualizations) of BUTTON

        m_bIsButton = true;

        LPCTSTR tszButtonType = pInteractionSgml->GetValue(_T("button-type"));
        if (tszButtonType != NULL)
            m_csButtonType = tszButtonType;

        CString csCallout(tszButtonType);
        if(csCallout.Find(_T("callout")) != -1)
            m_bIsCallout = true;
        LPCTSTR tszDemoMode = pInteractionSgml->GetValue(_T("demoDocumentObject"));
        if (tszDemoMode != NULL)
            m_bDemoDocumentObject = true;


        CArray<SgmlElement *, SgmlElement *> aElements;
        pInteractionSgml->GetElements(aElements);
        for (int i = 0; i < aElements.GetSize() && SUCCEEDED(hr); ++i) {
            SgmlElement *pElement = aElements[i];
            if (pElement != NULL) {
                CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjectCollector = NULL;

                if (_tcscmp(pElement->GetName(), _T("normal")) == 0)
                    paObjectCollector = &m_aNormalObjects;
                else if (_tcscmp(pElement->GetName(), _T("over")) == 0)
                    paObjectCollector = &m_aOverObjects;
                else if (_tcscmp(pElement->GetName(), _T("pressed")) == 0)
                    paObjectCollector = &m_aPressedObjects;
                else if (_tcscmp(pElement->GetName(), _T("inactive")) == 0)
                    paObjectCollector = &m_aInactiveObjects;
                else
                    hr = E_IA_INVALID_DATA;

                CArray<SgmlElement *, SgmlElement *> arSubElements;
                pElement->GetElements(arSubElements);
                for (int j = 0; j < arSubElements.GetSize() && SUCCEEDED(hr); ++j) {
                    SgmlElement *pSubElement = arSubElements[j];
                    if (pSubElement != NULL) {
                        DrawSdk::DrawObject *pObject = CWhiteboardStream::CreateObject(pSubElement, csRecordPath);
                        if (pObject != NULL)
                            paObjectCollector->Add(pObject);
                    }
                }
            }
        }
    } else if (_tcscmp(tszName, _T("CALLOUT")) == 0) {
        // parse all sub-objects (different visualizations) of BUTTON

        m_bIsButton = true;

        LPCTSTR tszButtonType = pInteractionSgml->GetValue(_T("button-type"));
        if (tszButtonType != NULL)
            m_csButtonType = tszButtonType;

       /* CString csCallout(tszButtonType);
        if(csCallout.Find(_T("callout")) != -1)*/
        m_bIsCallout = true;
        LPCTSTR tszDemoMode = pInteractionSgml->GetValue(_T("demoDocumentObject"));
        if (tszDemoMode != NULL)
            m_bDemoDocumentObject = true;

        LPCTSTR tszUserEdited = pInteractionSgml->GetValue(_T("isUserEdited"));
        if (tszUserEdited != NULL) {
            CString csUserEdited(tszUserEdited);
            if (csUserEdited.CompareNoCase(_T("true")) == 0)
                m_bIsUserEdited = true;
        }


        CArray<SgmlElement *, SgmlElement *> aElements;
        pInteractionSgml->GetElements(aElements);
        for (int i = 0; i < aElements.GetSize() && SUCCEEDED(hr); ++i) {
            SgmlElement *pElement = aElements[i];
            if (pElement != NULL) {
                CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjectCollector = NULL;

                if (_tcscmp(pElement->GetName(), _T("normal")) == 0)
                    paObjectCollector = &m_aNormalObjects;
                else if (_tcscmp(pElement->GetName(), _T("over")) == 0)
                    paObjectCollector = &m_aOverObjects;
                else if (_tcscmp(pElement->GetName(), _T("pressed")) == 0)
                    paObjectCollector = &m_aPressedObjects;
                else if (_tcscmp(pElement->GetName(), _T("inactive")) == 0)
                    paObjectCollector = &m_aInactiveObjects;
                else if (_tcscmp(pElement->GetName(), _T("CLICK")) == 0) {
                    hr = SetLcElementInfo(pElement);
                    if (SUCCEEDED(hr))
                        m_bHasClickObject = true;
                    continue;
                } else if (_tcscmp(pElement->GetName(), _T("RTF")) == 0) {
                    CString csRtf = pElement->GetParameter();
                    if ( !csRtf.IsEmpty())
                        m_csRtf = csRtf;
                } else
                    hr = E_IA_INVALID_DATA;

                CArray<SgmlElement *, SgmlElement *> arSubElements;
                pElement->GetElements(arSubElements);
                for (int j = 0; j < arSubElements.GetSize() && SUCCEEDED(hr); ++j) {
                    SgmlElement *pSubElement = arSubElements[j];
                    if (pSubElement != NULL) {
                        DrawSdk::DrawObject *pObject = CWhiteboardStream::CreateObject(pSubElement, csRecordPath);
                        if (pObject != NULL)
                            paObjectCollector->Add(pObject);
                    }
                }
            }
        }
    } else if (_tcscmp(tszName, _T("CLICK")) == 0) {
        m_bIsClickObject = true;
        hr = SetLcElementInfo(pInteractionSgml);
    } else if (_tcscmp(tszName, _T("RADIO")) == 0) {
        m_bIsRadio = true;

        LPCTSTR tszHasToBe = pInteractionSgml->GetValue(_T("give"));
        if (tszHasToBe != NULL && _tcscmp(_T("w"), tszHasToBe) == 0)
            m_bHasToBeChecked = true;
    } else if (_tcscmp(tszName, _T("CHECK")) == 0) {
        m_bIsCheck = true;

        LPCTSTR tszHasToBe = pInteractionSgml->GetValue(_T("give"));
        if (tszHasToBe != NULL && _tcscmp(_T("w"), tszHasToBe) == 0)
            m_bHasToBeChecked = true;
    } else if (_tcscmp(tszName, _T("FIELD")) == 0) {
        m_bIsField = true;

        LPCTSTR tszHasToBe = pInteractionSgml->GetValue(_T("give"));
        if (tszHasToBe != NULL)
            m_csCorrectText = tszHasToBe;
        else
            hr = E_IA_INVALID_DATA;
    }

    if (SUCCEEDED(hr))
        m_bIsInitialized = true;

    return hr;
}

HRESULT CInteractionArea::SetLcElementInfo(SgmlElement *pSgml){
    HRESULT hr = S_OK;
    if (m_pLcElementInfo == NULL)
        m_pLcElementInfo = new CLcElementInfo;

    int x = pSgml->GetIntValue(_T("x"));
    int y = pSgml->GetIntValue(_T("y"));
    int w = pSgml->GetIntValue(_T("width"));
    int h = pSgml->GetIntValue(_T("height"));
    CRect rcArea;
    rcArea.SetRect(x, y, x + w, y + h);
    m_pLcElementInfo->SetArea(rcArea);

    CString csVisibility = pSgml->GetValue(_T("visible"));
    m_pLcElementInfo->SetVisibility(csVisibility);

    CArray<SgmlElement *, SgmlElement *> aElements;
    pSgml->GetElements(aElements);
    for (int i = 0; i < aElements.GetSize() && SUCCEEDED(hr); ++i) {
        SgmlElement *pElement = aElements[i];
        if (pElement != NULL) {  
            if (_tcscmp(pElement->GetName(), _T("IDs")) == 0){                
                m_pLcElementInfo->SetIdAcc(pElement->GetValue(_T("acc")));
                m_pLcElementInfo->SetIdUiAuto(pElement->GetValue(_T("UIAuto")));
                m_pLcElementInfo->SetIdUiAutoTree(pElement->GetValue(_T("UiAutoTree")));
                m_pLcElementInfo->SetIdSap(pElement->GetValue(_T("SAP")));                
            // currently not used
            //} else if (_tcscmp(pElement->GetName(), _T("Timestamp")) == 0) {
            //    m_pLcElementInfo->SetTimestamp(pElement->GetParameter());
            } else if (_tcscmp(pElement->GetName(), _T("Bounds")) == 0) {
                CPoint ptLocation;
                ptLocation.x = pElement->GetIntValue(_T("left"));
                ptLocation.y = pElement->GetIntValue(_T("top"));
                m_pLcElementInfo->SetPtLocation(ptLocation);
                CPoint ptSize;
                ptSize.x = pElement->GetIntValue(_T("width"));
                ptSize.y = pElement->GetIntValue(_T("height"));
                m_pLcElementInfo->SetPtSize(ptSize);
            } else if (_tcscmp(pElement->GetName(), _T("Name")) == 0) {
                //CString csName = pElement->GetParameter();
                ////StringManipulation::DecodeXmlSpecialCharacters(csName);
                //CString csN = csName;
                //StringManipulation::TransformForSgml(csName, csN);
                m_pLcElementInfo->SetName(pElement->GetParameter());
            } else if (_tcscmp(pElement->GetName(), _T("ProcessName")) == 0) {
                m_pLcElementInfo->SetProcessName(pElement->GetParameter());
            } else if (_tcscmp(pElement->GetName(), _T("AccType")) == 0) {
                m_pLcElementInfo->SetAccType(pElement->GetParameter());
            } else if (_tcscmp(pElement->GetName(), _T("SapType")) == 0) {
                m_pLcElementInfo->SetSapType(pElement->GetParameter());
            } else {
                hr = E_IA_INVALID_DATA;
            }
        }
    }
    return hr;
}

bool CInteractionArea::IsVisibleAt(UINT nTimestampMs)
{
   return nTimestampMs >= m_nVisibilityStartMs && nTimestampMs <= m_nVisibilityEndMs;
}

void CInteractionArea::Draw(HDC hdc, double dOffX, double dOffY, DrawSdk::ZoomManager *pZoom)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (IsButton())
   {
      // TODO regard activity and inactivity
      // TODO maybe always paint inactive state (they are not usable)

      for (int i=0; i<m_aNormalObjects.GetSize(); ++i)
         m_aNormalObjects[i]->Draw(hdc, dOffX, dOffY, pZoom);
   }
   else if (IsSupport() && m_pSupportObject != NULL)
   {
      m_pSupportObject->Draw(hdc, dOffX, dOffY, pZoom);
   }
   else if (IsRadioButton() || IsCheckBox() || IsTextField())
   {
      // TODO regard activity and inactivity
      // TODO regard radio button

      CRect rcPaintArea = m_rcDimensions;
      rcPaintArea.OffsetRect((int)dOffX, (int)dOffY);
      double dZoom = 1.0;
      if (pZoom != NULL)
         dZoom = pZoom->GetZoomFactor();
      if (dZoom != 1.0)
      {
         rcPaintArea.left = (int)(dZoom * rcPaintArea.left);
         rcPaintArea.top = (int)(dZoom * rcPaintArea.top);
         rcPaintArea.right = (int)(dZoom * rcPaintArea.right);
         rcPaintArea.bottom = (int)(dZoom * rcPaintArea.bottom);
      }

      if (IsRadioButton())
      {
         DrawFrameControl(hdc, rcPaintArea, DFC_BUTTON, DFCS_BUTTONRADIO);
      }
      else if (IsCheckBox())
      {
         DrawFrameControl(hdc, rcPaintArea, DFC_BUTTON, DFCS_BUTTONCHECK);
      }
      else if (IsTextField())
      {
         Rectangle(hdc, rcPaintArea.left, rcPaintArea.top, rcPaintArea.right, rcPaintArea.bottom);
      }
   }
}

CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *>* CInteractionArea::GetNormalObjects()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_bObjectsExtracted = true; // one variable for all four object gropus
   return &m_aNormalObjects;
}

CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *>* CInteractionArea::GetOverObjects()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_bObjectsExtracted = true; // one variable for all four object gropus
   return &m_aOverObjects;
}

CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *>* CInteractionArea::GetPressedObjects()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_bObjectsExtracted = true; // one variable for all four object gropus
   return &m_aPressedObjects;
}

CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *>* CInteractionArea::GetInactiveObjects()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_bObjectsExtracted = true; // one variable for all four object gropus
   return &m_aInactiveObjects;
}

HRESULT CInteractionArea::GetVisibilityTimes(CArray<CPoint, CPoint>* paVisibilityTimePeriods)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // Visibilty times are stored in an array of CPoints as pairs of beginning/end timestamps.
   // The String 'm_csVisibility' can also contain text.
   return StringManipulation::ExtractAllTimePeriodsFromString(m_csVisibility, paVisibilityTimePeriods);
}

HRESULT CInteractionArea::GetActivityTimes(CArray<CPoint, CPoint>* paActivityTimePeriods)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // Activity times are stored in an array of CPoints as pairs of beginning/end timestamps.
   // The String 'm_csActivity' can also contain text.
   return StringManipulation::ExtractAllTimePeriodsFromString(m_csActivity, paActivityTimePeriods);
}

HRESULT CInteractionArea::GetCorrectTexts(CArray<CString, CString> *paCorrectTexts)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (IsTextField())
   {
      if (paCorrectTexts == NULL)
         return E_POINTER;

      LPCTSTR tszCorrectTexts = (LPCTSTR)m_csCorrectText;

      int iThisStringStart = 0;
      _TCHAR tszBuffer[400];
      int iLength = _tcslen(tszCorrectTexts);
      for (int i=0; i<iLength; ++i)
      {
         bool bWasEscaped = i > 0 && tszCorrectTexts[i-1] == '\\';
         if (tszCorrectTexts[i] == ';' && !bWasEscaped)
         {
            if (i - iThisStringStart > 0)
            {
               int iBufferPointer = 0;
               int iSubLength = i - iThisStringStart;
               for (int c=iThisStringStart; c<iThisStringStart+iSubLength; ++c)
               {
                  bool bWasEscapedInner = c > 0 && tszCorrectTexts[c-1] == '\\';
                  if (tszCorrectTexts[c] != '\\' || bWasEscapedInner)
                     tszBuffer[iBufferPointer++] = tszCorrectTexts[c];
               }
               tszBuffer[iBufferPointer] = 0;

               CString csOneText = tszBuffer;

               paCorrectTexts->Add(csOneText);
            }

            iThisStringStart = i+1;
         }
      }

      return S_OK;
   }
   else
      return S_FALSE;
}
   
/////////////////////////////////
// Class CLcElementInfo
CLcElementInfo::CLcElementInfo() {
}

CLcElementInfo::~CLcElementInfo() {
}

CLcElementInfo* CLcElementInfo::Copy() {
    CLcElementInfo* pNew = new CLcElementInfo;
    pNew->SetArea(m_rcArea);
    pNew->SetVisibility(m_csVisibility);
    pNew->SetName(m_csName);
    pNew->SetProcessName(m_csProcessName);
    pNew->SetPtLocation(m_ptLocation);
    pNew->SetPtSize(m_ptSize);
    pNew->SetAccType(m_csAccType);
    pNew->SetSapType(m_csSapType);
    pNew->SetIdAcc(m_csIdAcc);
    pNew->SetIdUiAuto(m_csIdUiAuto);
    pNew->SetIdUiAutoTree(m_csIdUiAutoTree);
    pNew->SetIdSap(m_csIdSap);
    return pNew;
}

void CLcElementInfo::SetElementInfo(CLcElementInfo *lcElementInfo){
    ASSERT(lcElementInfo);

    SetArea(lcElementInfo->GetArea());
    SetVisibility(lcElementInfo->GetVisibility());
    SetName(lcElementInfo->GetName());
    SetProcessName(lcElementInfo->GetProcessName());
    SetPtLocation(lcElementInfo->GetPtLocation());
    SetPtSize(lcElementInfo->GetPtSize());
    SetAccType(lcElementInfo->GetAccType());
    SetSapType(lcElementInfo->GetSapType());
    SetIdAcc(lcElementInfo->GetIdAcc());
    SetIdUiAuto(lcElementInfo->GetIdUiAuto());
    SetIdUiAutoTree(lcElementInfo->GetIdUiAutoTree());
    SetIdSap(lcElementInfo->GetIdSap());
}

bool CLcElementInfo::operator == (CLcElementInfo& lcElementInfo) const {
    return (lcElementInfo.GetArea() == m_rcArea
        && lcElementInfo.GetVisibility().Compare(m_csVisibility) == 0
        && lcElementInfo.GetName().Compare(m_csName) == 0
        && lcElementInfo.GetProcessName().Compare(m_csProcessName) == 0
        && lcElementInfo.GetPtLocation() == m_ptLocation
        && lcElementInfo.GetPtSize() == m_ptSize
        && lcElementInfo.GetAccType().Compare(m_csAccType) == 0
        && lcElementInfo.GetSapType().Compare(m_csSapType) == 0
        && lcElementInfo.GetIdAcc().Compare(m_csIdAcc) == 0
        && lcElementInfo.GetIdUiAuto().Compare(m_csIdUiAuto) == 0
        && lcElementInfo.GetIdUiAutoTree().Compare(m_csIdUiAutoTree) == 0
        && lcElementInfo.GetIdSap().Compare(m_csIdSap) == 0);
}

bool CLcElementInfo::operator != (CLcElementInfo& lcElementInfo) const {
    return !(*this == lcElementInfo);
}