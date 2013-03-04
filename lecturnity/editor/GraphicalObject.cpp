// Callout.cpp : implementation file
//

#include "stdafx.h"
#include "GraphicalObject.h"

// CGraphicalObject

IMPLEMENT_DYNCREATE(CGraphicalObject, CObject)


/* REVIEW UK (sorted by importance)
* - m_pPolygonObject and m_pTextObject are created but never deleted
*   -> Unnecessary memory leak?
*   -> However simply copying the pointer in CloneTo() is a problem here
*   -> Probably a true copy should be made
* - InitCalloutTypes() + GetCalloutTypeFromString(): This should use a static CMap
*   -> Having a string array in every object just for parsing/mapping something is inefficient.
* - Identifier "BUTTON": as this is no button it should be written as "CALLOUT"
*   -> What kind of changes does this need in the Publisher?
* - GetCalloutDimensions(): What does it do and why?
*   -> Complex/non-obvious code must have documentation/comments
* - Use the Code conventions
*   -> Curly braces ({) are not on a new line
*   -> if(: There is a space after "if" and after ")"
* - DestroyEditControl(): does much more than destroying
*   -> Copying and setting the text is even more important than destroying; the name should reflect this
* - Why must CalculateObjectsPosition() be called after Move()?
*   -> Shouldn't it be enough to move the already correctly sized objects?
* - Why has void CCallout::Init() a void as return type (and the other Init() has HRESULT)?
* - enum CalloutType: All of the identifiers should be prefixed with "CALLOUT_"
*   -> These enumerations are global and thus naming conflicts are easy
* - Init(): returns E_IAX_UNINITIALIZED when in fact E_IAX_INVALID_DATA is meant
*   -> E_IAX_UNINITIALIZED is used if a method is called on an uninitialized object
*   -> Consequently "bObjectsInitialized" is probably a "bInvalidSpecification"
* - Namings:
*   -> CalculateObjectsPosition() should be CalculateObjectPositions() ?
*   -> GetCalloutDimensions(... rcOring) should be rcOrig?
*/

#define CALLOUT_FINGER_WIDTH 20
#define CALLOUT_FINGER_HEIGHT 10
#define CALLOUT_FINGER_MARGIN 10

CArray<CString, CString>CGraphicalObject::m_scaGOType;

CGraphicalObject::CGraphicalObject() {
    m_idGraphicalObjectType = GOT_CALLOUT_TOP_LEFT;
    //m_pPolygonObject = NULL;
    //m_pTextObject = NULL;
    m_pEdit = NULL;
    m_pWbView = NULL;

    m_logFont.lfHeight         = -18;
    m_logFont.lfWidth          = 0;
    m_logFont.lfEscapement     = 0;
    m_logFont.lfOrientation    = 0;
    m_logFont.lfWeight         = FW_NORMAL;
    m_logFont.lfItalic         = FALSE;
    m_logFont.lfUnderline      = FALSE;
    m_logFont.lfStrikeOut      = FALSE;
    m_logFont.lfCharSet        = DEFAULT_CHARSET;
    m_logFont.lfOutPrecision   = OUT_TT_PRECIS;
    m_logFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
    m_logFont.lfQuality        = ANTIALIASED_QUALITY;
    m_logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    _stprintf((TCHAR *) &m_logFont.lfFaceName, _T("Segoe UI"));

    m_bIsObjectCallout = false;
    InitGraphicalObjectTypes();
    m_bDemoDocumentObject = false;
    m_clrText = RGB(255,255,255);
    m_pLcElementInfo = NULL;
    m_pClickObject = NULL;
    m_bIsUserEdited = false;
    m_bAutoMove = false;
    m_pDemoButton = NULL;
    m_csRtf = _T("");
    m_rcEdit = CRect(0,0,0,0);
	m_dLastZoom = 1.0;
	m_dOrigZoom = 1.0;
	m_bChanged = true;
	m_csDisplayText = "";
}

CGraphicalObject::~CGraphicalObject() {
    //SAFE_DELETE(m_pPolygonObject);// = NULL;
    //SAFE_DELETE(m_pTextObject);// = NULL;
    RemoveDrawObjects();
    m_pWbView = NULL;

    if (m_pEdit != NULL) {
        if (m_pEdit->m_hWnd != NULL) {
            m_pEdit->DestroyWindow();
        }

        delete m_pEdit;
        m_pEdit = NULL;
    }
    if (m_pLcElementInfo != NULL) {
        delete m_pLcElementInfo;
    }
    if (m_pClickObject != NULL) {
        delete m_pClickObject;
    }
    if (m_pDemoButton != NULL) {
       // delete m_pDemoButton;
        SAFE_DELETE(m_pDemoButton);
    }
}

void CGraphicalObject::RemoveDrawObjects(){
    for (int i = 0; i < m_aDrawObjects.GetCount(); i++) {
        delete m_aDrawObjects.GetAt(i);
    }
    m_aDrawObjects.RemoveAll();
}

void CGraphicalObject::RemoveTextObjects() {
    for (int i = 0; i < m_aDrawObjects.GetCount(); i++) {
        if (m_aDrawObjects.GetAt(i)->GetType() == DrawSdk::TEXT) {
            delete m_aDrawObjects.GetAt(i);
            m_aDrawObjects.RemoveAt(i);
            i--;
        }
    }
}

// CGraphicalObject message handlers

void CGraphicalObject::Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing) {
    /*m_pPolygonObject->Draw(pDC->m_hDC, ptOffsets->x, ptOffsets->y, pZoomManager);
    m_pTextObject->Draw(pDC->m_hDC, ptOffsets->x, ptOffsets->y, pZoomManager);*/
    for (int i = 0; i < m_aDrawObjects.GetCount(); i++) {
        m_aDrawObjects.GetAt(i)->Draw(pDC->m_hDC, ptOffsets->x, ptOffsets->y, pZoomManager);
    }

    if (m_pDemoButton != NULL) {
		if(m_pDemoButton->IsImageButton())
			m_pDemoButton->Draw(pDC, ptOffsets, pZoomManager, bPreviewing);
		else
		{
			CString csName, csNameOrig;
			csName = m_pDemoButton->GetInteractionName();
			csNameOrig = csName;
			if (m_bChanged){
				m_pDemoButton->SetInteractionName(csName,true);
				CRect rcButton = m_pDemoButton->GetArea();
				while (csName.GetLength() > 0 && m_pDemoButton->GetArea().Width() > m_rcDimensions.Width() - 2 * BUTTON_OUT_SPACE - 2 * BUTTON_IN_SPACE)
				{
					csName.Truncate(csName.GetLength()-1);
					m_pDemoButton->SetInteractionName(csName + _T("..."), true);
				}
				m_pDemoButton->Move(m_rcDimensions.left + (m_rcDimensions.Width() -m_pDemoButton->GetArea().Width() - BUTTON_OUT_SPACE - BUTTON_IN_SPACE)/2 - m_pDemoButton->GetArea().left ,0);
				m_pDemoButton->Draw(pDC, ptOffsets, pZoomManager, bPreviewing);
				m_pDemoButton->SetInteractionNameOnly(csNameOrig);
				m_bChanged = false;
				m_csDisplayText = csName;
			}
			else{
				csName = m_csDisplayText;
				if (csName.Compare(csNameOrig)!=0)
					csName = m_csDisplayText + _T("...");
				m_pDemoButton->SetInteractionNameOnly(csName);
				m_pDemoButton->Draw(pDC, ptOffsets, pZoomManager, bPreviewing);
				m_pDemoButton->SetInteractionNameOnly(csNameOrig);
			}
		}
    }

    if (!bPreviewing && IsSelected()) {
        CInteractionAreaEx::DrawBorder(pDC, ptOffsets, pZoomManager);
    }

    if (m_pEdit != NULL) {
        CPoint ptEditOffsets = m_pEdit->GetOffsets();
        CRect rcCalloutDimension = m_rcDimensions;
		if(!bPreviewing)
		{
            double dZoom = pZoomManager->GetZoomFactor();
            CPoint ptOff = *ptOffsets;
            if (m_pWbView != NULL) {
                dZoom = m_pWbView->GetZoomFactor();
                ptOff = m_pWbView->GetPageDimension().TopLeft();
            }
        //if ((ptEditOffsets.x != ptOffsets->x || ptEditOffsets.y != ptOffsets->y) && !bPreviewing) {
            rcCalloutDimension.left = (int)(rcCalloutDimension.left * dZoom/*pZoomManager->GetZoomFactor()*/ + 0.5);
            rcCalloutDimension.top = (int)(rcCalloutDimension.top * dZoom/*pZoomManager->GetZoomFactor()*/ + 0.5);
            rcCalloutDimension.right = (int)(rcCalloutDimension.right * dZoom/*pZoomManager->GetZoomFactor()*/ + 0.5);
            rcCalloutDimension.bottom = (int)(rcCalloutDimension.bottom * dZoom/*pZoomManager->GetZoomFactor()*/ + 0.5);
            rcCalloutDimension.OffsetRect(ptOff.x, ptOff.y);//(ptOffsets->x, ptOffsets->y);
            //m_pEdit->SetCalloutDimension(rcCalloutDimension);
			//To Do: update all font sizes
			/*if(m_dLastZoom !=pZoomManager->GetZoomFactor())
            {

                int iPosFind=0,iPosGet = 0, iFontSize;
                CString csNewRtf,csOldRtf;
                m_pEdit->GetRichText(m_pEdit,csOldRtf);
                iPosFind = csOldRtf.Find(_T("\\fs"),iPosFind);
                while(iPosFind >=0)
                {
                    csNewRtf+=csOldRtf.Mid(iPosGet,iPosFind-iPosGet+3);
                    iPosGet=csOldRtf.Find(_T(" "),iPosFind);
                    if(iPosGet < 0)
                        iPosGet = csOldRtf.Find(_T("\\"),iPosFind+1);
                    iFontSize =_ttoi(csOldRtf.Mid(iPosFind+3,iPosGet-iPosFind-3));
                    iFontSize = iFontSize/m_dLastZoom+0.5;
                    m_dLastZoom = pZoomManager->GetZoomFactor();
                    iFontSize = iFontSize*m_dLastZoom+0.5;
                    csNewRtf.AppendFormat(_T("%d"),iFontSize);
                    iPosFind = csOldRtf.Find(_T("\\fs"),iPosGet);
                }
                csNewRtf+=csOldRtf.Right(csOldRtf.GetLength()-iPosGet);
                char *szString;
                int nLen = ::WideCharToMultiByte(CP_ACP, 0, csNewRtf, -1, NULL, 0, NULL, NULL);
                szString = (char*)malloc((nLen + 1) * sizeof(char));
                ::WideCharToMultiByte(CP_ACP, 0, csNewRtf, -1, szString, nLen + 1, NULL, NULL);
                szString[nLen] = '\0';

                m_pEdit->SetRichText(m_pEdit, szString);
                SAFE_DELETE(szString);
            }*/
			//m_pEdit->SetRichText(m_pEdit,csNewRtf);
            //m_pEdit->SetOffsets(*ptOffsets);
			//m_pEdit->FixPos();
            //CFont font;
            //LOGFONT logFont;
            //memcpy(&logFont, &m_logFont, sizeof(LOGFONT));
            //logFont.lfHeight = -1 * (long)(abs(logFont.lfHeight) * pZoomManager->GetZoomFactor() + 0.5);
            //font.CreateFontIndirect(&logFont);
            //m_pEdit->SetFont(&font);
            //m_pEdit->SetLogFont(logFont);
            //m_pEdit->SetBackgroundColor(false, GetFilledObject()->GetFillColorRef());
            //CString csText;
            //m_pEdit->GetWindowText(csText);
            //m_pEdit->SetFocus();
            
            if (m_dLastZoom != dZoom/*pZoomManager->GetZoomFactor()*/ || ptOff != ptEditOffsets) {
                m_pEdit->SetCalloutDimension(rcCalloutDimension);
			    
                ZoomEdit(/*pZoomManager->GetZoomFactor()*/ dZoom* 100);
                m_pEdit->FixPos();
                m_pEdit->SetOffsets(ptOff);
                m_dLastZoom = dZoom;
            }

        } 
    }
}

HRESULT CGraphicalObject::Init(GraphicalObjectType iGOType, CEditorProject *pProject, CRect rcDimensions, int iTimestampPos, CString csText, bool bDemoMode, CLcElementInfo *pLcInfo) {
    HRESULT hr = S_OK;
    if (pProject == NULL) {
        return E_POINTER;
    }

    m_rcDimensions = rcDimensions;
    m_idGraphicalObjectType = iGOType;
    m_pEditorProject = pProject;

    m_pEditorProject->GetGraphicalObjectDefaultSettings(m_clrText, m_clrLine, m_clrFill, m_logFont, m_iLineWidth, m_iLineStyle);

    if (iGOType == GOT_TEXT) {
        DrawSdk::Rectangle* pFilledRectangleObject = new DrawSdk::Rectangle(m_rcDimensions.left, m_rcDimensions.top, m_rcDimensions.Width(), m_rcDimensions.Height(), m_clrLine/*RGB(56,93,138)*/, m_clrFill/*RGB(79,129,189)*/, 0/*2.0*/, 0/*0*/);
        m_aDrawObjects.Add(pFilledRectangleObject->Copy());
        DrawSdk::Rectangle* pOutlineRectangleObject = new DrawSdk::Rectangle(m_rcDimensions.left, m_rcDimensions.top, m_rcDimensions.Width(), m_rcDimensions.Height(), m_clrLine/*RGB(56,93,138)*/, m_iLineWidth/*2.0*/, m_iLineStyle/*0*/);
        m_aDrawObjects.Add(pOutlineRectangleObject->Copy());
        m_bIsObjectCallout = false;

        SAFE_DELETE(pFilledRectangleObject);
        SAFE_DELETE(pOutlineRectangleObject);
    } else {
        DrawSdk::Polygon* pFilledPolygonObject = new DrawSdk::Polygon(NULL, 0, m_clrLine/*RGB(56,93,138)*/, m_clrFill/*RGB(79,129,189)*/, 0.0/*2.0*/, 0/*0*/);
        pFilledPolygonObject->SetAutoClose(true);
        m_aDrawObjects.Add(pFilledPolygonObject->Copy());
        DrawSdk::Polygon* pOutlinePolygonObject = new DrawSdk::Polygon(NULL, 0, m_clrLine/*RGB(56,93,138)*/, m_iLineWidth/*2.0*/, m_iLineStyle);
        pOutlinePolygonObject->SetAutoClose(false);
        m_aDrawObjects.Add(pOutlinePolygonObject->Copy());
        m_bIsObjectCallout = true;

        SAFE_DELETE(pFilledPolygonObject);
        SAFE_DELETE(pOutlinePolygonObject); 
    }
    UINT nPageStartMs, nPageEndMs;
    CEditorDoc *pDoc = m_pEditorProject->m_pDoc;
    if (pDoc != NULL) {
        m_pWbView = pDoc->GetWhiteboardView();
    }
    m_pEditorProject->GetPagePeriod(iTimestampPos, nPageStartMs, nPageEndMs);
    //m_Visibility.Init(INTERACTION_PERIOD_TIME, iTimestampPos, iTimestampPos);
    int iVisibilityStart, iVisibilityEnd;
    int iActivityTime = iTimestampPos;
    int iActivityEnd;
    if (bDemoMode) {
        if (csText.IsEmpty()) { // crated with insert interactive callout
            iVisibilityStart = nPageStartMs;
            CMarkStream *pMarkStream = m_pEditorProject->GetMarkStream();
            if (pMarkStream != NULL){
                CStopJumpMark* pMark = pMarkStream->GetLastStopMarkInRange(nPageStartMs, iTimestampPos - nPageStartMs);
                if (pMark != NULL)
                    iVisibilityStart = pMark->GetPosition() + 1;
            }
            iVisibilityEnd = iTimestampPos;
            if (iVisibilityEnd < iVisibilityStart + m_pEditorProject->GetObjectMinTimeSpan())
                iVisibilityEnd = iVisibilityStart + m_pEditorProject->GetObjectMinTimeSpan();
        } else { // created from create demo document
            iVisibilityStart = iTimestampPos;
            iVisibilityEnd = iTimestampPos + m_pEditorProject->GetObjectMinTimeSpan() - 1;
        }
        m_bDemoDocumentObject = true;
        //m_Visibility.Init(INTERACTION_PERIOD_TIME, iTimestampPos, iTimestampPos);
        iActivityEnd = iVisibilityEnd;//iTimestampPos;
    } else {
        iVisibilityStart = iTimestampPos;
        iVisibilityEnd = nPageEndMs;
        if (nPageEndMs < iVisibilityStart + m_pEditorProject->GetObjectMinTimeSpan())
            iVisibilityEnd = iVisibilityStart + m_pEditorProject->GetObjectMinTimeSpan();
        //m_Visibility.Init(INTERACTION_PERIOD_TIME, nPageStartMs, nPageEndMs);
        iActivityTime = iVisibilityStart;
        iActivityEnd = iVisibilityEnd;
    }
    m_Visibility.Init(INTERACTION_PERIOD_TIME, iVisibilityStart, iVisibilityEnd);
    //m_Activity.Init(INTERACTION_PERIOD_TIME, iActivityTime, iActivityTime);
    m_Activity.Init(INTERACTION_PERIOD_TIME, iActivityTime, iActivityEnd);
    if (pLcInfo != NULL) {
        m_pLcElementInfo = pLcInfo->Copy();
        if (m_pClickObject != NULL) {
            delete m_pClickObject;
        }
        m_pClickObject = new CClickObject();
        m_pClickObject->Init(m_pLcElementInfo, pProject);
        csText = GetLcInfoDisplayText();
    }
    CreateNameAndType(csText);
    SetArea(&m_rcDimensions);
    return hr;
}

HRESULT CGraphicalObject::Init(CInteractionArea *pInteraction, CEditorProject *pProject) {
    HRESULT hr = S_OK;
    m_pEditorProject = pProject;
    CRect rcObjectDimensions = pInteraction->GetActivityArea();
    CEditorDoc *pDoc = m_pEditorProject->m_pDoc;
    if (pDoc != NULL) {
        m_pWbView = pDoc->GetWhiteboardView();
    }
    CString csCalloutType = pInteraction->GetButtonType();//GetCalloutType();
    if (pInteraction->IsDemoDocumentObject()) {
        m_bDemoDocumentObject = true;
    }

    if (pInteraction->IsUserEdited()) {
        m_bIsUserEdited = true;
    }

    m_csRtf = pInteraction->GetRtf();
    StringManipulation::DecodeXmlSpecialCharacters(m_csRtf);
    m_idGraphicalObjectType = GetGraphicalObjectTypeFromString(csCalloutType);
    if (m_idGraphicalObjectType == GOT_TEXT) {
        m_bIsObjectCallout = false;
    } else {
        m_bIsObjectCallout = true;
    }
    

    CString csVisibility = pInteraction->GetVisibility();
    if (csVisibility.GetLength() > 0) {
        hr = m_Visibility.Parse(csVisibility);
    }
    CString csActivity = pInteraction->GetActivity();
    if (csActivity.GetLength() > 0) {
        hr = m_Activity.Parse(csActivity);
    }
    //CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects;
    m_aDrawObjects.Append(*pInteraction->GetNormalObjects());

    if (m_bIsObjectCallout) {
        GetCalloutDimensions(m_rcDimensions, rcObjectDimensions, true);
    } else {
        m_rcDimensions.SetRect(rcObjectDimensions.TopLeft(), rcObjectDimensions.BottomRight());
    }

    CString csText;
    for (int i=0; i<m_aDrawObjects.GetCount(); i++) {
        int iDrawObjectType = m_aDrawObjects.GetAt(i)->GetType();
        if (iDrawObjectType != DrawSdk::TEXT && iDrawObjectType != DrawSdk::POLYGON && iDrawObjectType != DrawSdk::RECTANGLE) {
            return E_IAX_INVALID_DATA;
        }
        if (iDrawObjectType == DrawSdk::TEXT) {
            DrawSdk::Text *pTextObject = (DrawSdk::Text *)m_aDrawObjects.GetAt(i);
            //pTextObject->DrawSpecial(true);
            if (m_aDrawObjects.GetAt(i-1)->GetType() == DrawSdk::TEXT && !pTextObject->IsDrawSpecial()) 
                csText.Append(_T("\r\n"));
            /*} else {*/
                pTextObject->GetLogFont(&m_logFont);
                m_clrText = pTextObject->GetOutlineColorRef();
            //}
                CString s = pTextObject->GetString();
            csText.Append(pTextObject->GetString()); 
        }
    }
    int iPos = csText.ReverseFind(_T('\r'));
    if (iPos != -1 && iPos == csText.GetLength() -1) {
        csText = csText.Left(iPos); // remove last read \r\n
    }
    if (pInteraction->HasClickObject()) {
        m_pLcElementInfo = pInteraction->GetLcElementInfo();
        if (m_pClickObject != NULL) {
            delete m_pClickObject;
        }
        m_pClickObject = new CClickObject();
        m_pClickObject->Init(m_pLcElementInfo, pProject);
    }
    CreateNameAndType(csText);
   /* DrawSdk::Polygon *pFilledPoly = NULL, *pOutlinePoly = NULL;

    DrawSdk::Text *pText = NULL;
    if (SUCCEEDED(hr)) {
        for (int i = 0; i < aObjects.GetCount(); i++) {
            if (aObjects.GetAt(i)->GetType() == DrawSdk::TEXT) {
                pText = (DrawSdk::Text *)aObjects.GetAt(i);
            } else if (aObjects.GetAt(i)->GetType() == DrawSdk::POLYGON) {
                DrawSdk::Polygon *poly = (DrawSdk::Polygon *)aObjects.GetAt(i);
                if(poly->GetIsFilled() == true) {
                    pFilledPoly = poly;
                } else {
                    pOutlinePoly = poly;
                }
            } else {
                hr = E_IAX_INVALID_DATA;
            }
        }
    }

    bool bInvalidSpecification = true;
    if (pFilledPoly != NULL && pOutlinePoly != NULL) {
        m_pPolygonObject = pFilledPoly;
        m_pPolygonObject->SetLineColor(pOutlinePoly->GetOutlineColor());
        m_pPolygonObject->SetLineWidth(pOutlinePoly->GetLineWidth());
        m_pPolygonObject->SetAutoClose(true);
    } else {
        bInvalidSpecification = false;
    }

    if (pText != NULL) {
        m_pTextObject = pText;
    } else {
        bInvalidSpecification = false;
    }

    if (bInvalidSpecification) {
        CreateNameAndType();
    } else {
        hr = E_IAX_INVALID_DATA;
    }*/
    
    return hr;
}

HRESULT CGraphicalObject::AddDuringParsing(CInteractionArea *pInteraction, CEditorProject *pProject, UINT nInsertPositionMs) {
    
    HRESULT hr = S_OK;

    if (pInteraction == NULL)
        return E_POINTER;

    if (!pInteraction->IsButton())
        return E_INVALIDARG;

    CInteractionAreaEx *pDemoButton = new CInteractionAreaEx();
    hr = pDemoButton->Init(pInteraction, pProject, nInsertPositionMs);
    m_pDemoButton = pDemoButton->Copy(true);
    delete pDemoButton;
    return hr;
}

HRESULT CGraphicalObject::AddDemoButton(CInteractionAreaEx* pInteraction, bool bIsNew) {
    HRESULT hr = S_OK;

    if (pInteraction == NULL)
        return E_POINTER;

    if (!pInteraction->IsButton())
        return E_INVALIDARG;
    
    if (bIsNew && m_pDemoButton != NULL)
        return E_INVALIDARG;
    if(m_pDemoButton)
		SAFE_DELETE(m_pDemoButton);
    m_pDemoButton = pInteraction->Copy(true);
    return hr;
}

bool CGraphicalObject::Activate(UINT nPositionMs, bool bPreviewing, bool bAlwaysActive /*= false*/){
    bool bRet = false;
    if (m_pDemoButton != NULL) {
        bRet = m_pDemoButton->Activate(nPositionMs, bPreviewing, bAlwaysActive);
    }
    return bRet;
}

HRESULT CGraphicalObject::SetVisibilityPeriod(CTimePeriod *pOther) {
    HRESULT hr = S_OK;
    //CEditorDoc* pDoc = (CEditorDoc*)GetD
    CEditorDoc *pDoc = m_pEditorProject->m_pDoc;
    if (pDoc != NULL) {
        if (!pOther->Contains(pDoc->GetCurrentPreviewPos()) && m_pEdit != NULL)
            SetGraphicalObjectText(false);
    }
    int iCurrentEnd = GetVisibilityStart() + GetVisibilityLength() - 1;
    hr = CInteractionAreaEx::SetVisibilityPeriod(pOther);
    if (SUCCEEDED(hr)) {
        CTimePeriod ctp;
        ctp.Init(INTERACTION_PERIOD_TIME, pOther->GetEnd(), pOther->GetEnd());
        hr = CInteractionAreaEx::SetActivityPeriod(pOther);
        if (SUCCEEDED(hr) && m_pDemoButton != NULL) {
            m_pDemoButton->SetVisibilityPeriod(pOther);
            m_pDemoButton->SetActivityPeriod(&ctp);
            CStopJumpMark *pMark = m_pEditorProject->GetMarkStream()->GetFirstStopMarkInRange(iCurrentEnd, 1);
            if (pMark != NULL)
                pMark->IncrementPosition(ctp.GetEnd() - iCurrentEnd);
        }
    }
    return hr;
}

HRESULT CGraphicalObject::IncrementTimes(UINT nMoveMs) {
    HRESULT hr = CInteractionAreaEx::IncrementTimes(nMoveMs);
    if (SUCCEEDED(hr) && m_pDemoButton != NULL) {
        hr = m_pDemoButton->IncrementTimes(nMoveMs);
    }
    return hr;
}

HRESULT CGraphicalObject::DecrementTimes(UINT nMoveMs) {
    HRESULT hr = CInteractionAreaEx::DecrementTimes(nMoveMs);
    if (SUCCEEDED(hr) && m_pDemoButton != NULL) {
        hr = m_pDemoButton->DecrementTimes(nMoveMs);
    }
    return hr;
}

HRESULT CGraphicalObject::CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey) {
    HRESULT hr = S_OK;
    UINT nStartMs = m_Visibility.GetBegin();
    UINT nEndMs = m_Visibility.GetBegin() + m_Visibility.GetLength() - 1;
    hr = CInteractionAreaEx::CloneTo(pTarget, bCopyHashKey);

    if (SUCCEEDED(hr)) {
        if (pTarget->GetClassId() != GetClassId()) {
            return E_INVALIDARG;
        }

        CGraphicalObject *pOther = (CGraphicalObject *)pTarget;
        pOther->m_pEditorProject = m_pEditorProject;
        pOther->m_idGraphicalObjectType = m_idGraphicalObjectType;
        pOther->m_bIsObjectCallout = m_bIsObjectCallout;
        pOther->m_csRtf = m_csRtf;
        pOther->RemoveDrawObjects();
        for (int i = 0; i < m_aDrawObjects.GetCount(); i++) {
            pOther->m_aDrawObjects.Add(m_aDrawObjects.GetAt(i)->Copy());
        }
        pOther->m_pWbView = m_pWbView;
        pOther->m_logFont = m_logFont;
        pOther->m_clrText = m_clrText;

        if (m_pDemoButton != NULL) {
            pOther->m_pDemoButton = m_pDemoButton->Copy();
        }
        
        CTimePeriod ctp;
        ctp.Init(INTERACTION_PERIOD_TIME, nStartMs, nEndMs);
        pOther->SetVisibilityPeriod(&ctp);
        if (m_pEdit != NULL) {
            SetGraphicalObjectText();
        }
        pOther->SetUserEdited(m_bIsUserEdited);
        pOther->m_pEdit = NULL;
        if (m_pLcElementInfo != NULL) {
            pOther->m_pLcElementInfo = m_pLcElementInfo->Copy();
        }
        if (m_pClickObject != NULL) {
            pOther->m_pClickObject = (CClickObject *)m_pClickObject->Copy();
        }

		//pOther->m_bChanged = m_bChanged;
		pOther->m_csDisplayText = m_csDisplayText;
        
    }
    return hr;
}

void CGraphicalObject::CreateNameAndType(CString csName) {
    m_csDisplayName.Empty();
    if(!csName.IsEmpty()) {
        m_csDisplayName = csName;
    }
    m_csDisplayType.LoadString(m_bIsObjectCallout ? ID_GRAPHICALOBJECT_CALLOUT : ID_GRAPHICALOBJECT_TEXT);
}

void CGraphicalObject::ChangeFontAndColor(LOGFONT logFont, _TCHAR *tszFontFamily, int iFontSize, COLORREF clrText) {
    memcpy(&m_logFont, &logFont, sizeof(LOGFONT));
    _tcscpy(m_logFont.lfFaceName, tszFontFamily);
    m_logFont.lfHeight = iFontSize;
    m_clrText = clrText;
    for (int i=0; i<m_aDrawObjects.GetCount(); i++) {
        if (m_aDrawObjects.GetAt(i)->GetType() == DrawSdk::TEXT) {
            DrawSdk::Text *pTextObject = (DrawSdk::Text *)m_aDrawObjects.GetAt(i);
            pTextObject->SetFont(&m_logFont);
            pTextObject->ChangeFontFamily(tszFontFamily);
            pTextObject->ChangeFontSize(iFontSize);
            pTextObject->SetLineColor(m_clrText); 
        }
    }
    CalculateObjectPositions();
    if (m_pEdit != NULL) {
        CFont font;
        LOGFONT logFont;
        memcpy(&logFont, &m_logFont, sizeof(LOGFONT));
        double dZoomFactor = 1.0;
        if (m_pWbView != NULL) {
            dZoomFactor = m_pWbView->GetZoomFactor();
        }
        logFont.lfHeight = -1 * ((double)abs(logFont.lfHeight) * dZoomFactor);
        font.CreateFontIndirect(&logFont);
        m_pEdit->SetFont(&font);
        m_pEdit->SetLogFont(logFont); 

        /*CHARFORMAT cf;
        memset(&cf, 0, sizeof(CHARFORMAT));
        cf.cbSize = sizeof(CHARFORMAT);
        cf.dwMask = CFM_COLOR;
        cf.dwEffects = 0;
        m_pEdit->GetDefaultCharFormat(cf);
        cf.crTextColor = m_clrText;
        if (cf.dwEffects & CFE_AUTOCOLOR) {
            cf.dwEffects ^= CFE_AUTOCOLOR;
        }
        m_pEdit->SetDefaultCharFormat(cf);*/
        m_pEdit->SetBackgroundColor(false, GetFilledObject()->GetFillColorRef());
    }
}

HRESULT CGraphicalObject::Move(float fDiffX, float fDiffY) {
    HRESULT hr = CInteractionAreaEx::Move(fDiffX, fDiffY);
    //CalculateObjectPositions();
    /*m_pPolygonObject->Move(fDiffX, fDiffY);
    m_pTextObject->Move(fDiffX, fDiffY);*/
    for (int i = 0; i < m_aDrawObjects.GetCount(); i++) {
        m_aDrawObjects.GetAt(i)->Move(fDiffX, fDiffY);
    }

    //test comment
    /*if (m_bDemoDocumentObject && !m_bAutoMove) {
        CArray<CInteractionAreaEx*, CInteractionAreaEx*> aRelatedDemoObj;
        int iTimestamp = m_Visibility.GetEnd();
        HRESULT hr = m_pEditorProject->GetInteractionStream()->FindRelatedDemoObjects(iTimestamp, &aRelatedDemoObj, INTERACTION_CLASS_GRAPHICAL_OBJECT);
        if (SUCCEEDED(hr) && aRelatedDemoObj.GetCount() == 1)
        {
            CInteractionAreaEx* pCopy = aRelatedDemoObj.GetAt(0)->Copy();
            pCopy->AutoMove(fDiffX, fDiffY);
            m_pEditorProject->GetInteractionStream()->ModifyArea(aRelatedDemoObj.GetAt(0)->GetHashKey(), pCopy, true);
            //aRelatedDemoObj.GetAt(0)->AutoMove(fDiffX, fDiffY);
        }
    }*/
    if (m_pDemoButton != NULL)
        m_pDemoButton->Move(fDiffX, fDiffY);
    return hr;
}

HRESULT CGraphicalObject::AutoMove(float fDiffX, float fDiffY) {
    m_bAutoMove = true;
    HRESULT hr = Move(fDiffX, fDiffY);
    m_bAutoMove = false;

    return hr;
}

HRESULT CGraphicalObject::SetArea(CRect *prcArea) {
    if (prcArea == NULL) {
        return E_POINTER;
    }
    int width = m_pEditorProject->m_targetPageWidth;
    int height = m_pEditorProject->m_targetPageHeight;

    if (m_pWbView) {
        CRect rcWhiteboard = m_pWbView->GetPageDimension();
        width = rcWhiteboard.Width();
        height = rcWhiteboard.Height();
    }

    double dZoom = 1.0;
    if (m_pWbView) {
        dZoom = m_pWbView->GetZoomFactor();
    }

    if (prcArea->Height() < GOT_MIN_HEIGHT) {
        prcArea->bottom = prcArea->top + GOT_MIN_HEIGHT;

        // if the graphical object is outside whiteboard area, recalculate its position
        int iBottom = (int)(height * 1.0/dZoom + 0.5);
        if (prcArea->bottom > iBottom) {
            prcArea->top = iBottom - GOT_MIN_HEIGHT;
            prcArea->bottom = iBottom;
        }
    }
    if (prcArea->Width() < GOT_MIN_WIDTH) {
        prcArea->right = prcArea->left + GOT_MIN_WIDTH;

        // if the graphical object is outside whiteboard area, recalculate its position
        int iRight = (int)(width * 1.0/dZoom + 0.5); 
        if (prcArea->right > iRight) {
            prcArea->left = iRight - GOT_MIN_WIDTH;
            prcArea->right = iRight;
        }
    }
	int iOffsetY = m_rcDimensions.bottom - prcArea->bottom;
    m_rcDimensions = *prcArea;
	
    if (m_bDemoDocumentObject && m_pDemoButton != NULL) {
        //CRect rcButton(0, 0, 105, 30); // size of the small buttons (...\LECTURNITY\Studio\ButtonImages\Windows\Small\Blue\Normal.png)
		CRect rcButton = m_pDemoButton->GetArea(); 
        int iXpos = m_rcDimensions.left + (m_rcDimensions.Width() - rcButton.Width())/2;
		int iYPos = rcButton.top - iOffsetY;
        rcButton.MoveToXY(iXpos, iYPos);

        /*CArray<CInteractionAreaEx*, CInteractionAreaEx*> aRelatedDemoObj;
        int iTimestamp = m_Visibility.GetEnd();
        HRESULT hr = m_pEditorProject->GetInteractionStream()->FindRelatedDemoObjects(iTimestamp, &aRelatedDemoObj, INTERACTION_CLASS_GRAPHICAL_OBJECT);
        if (SUCCEEDED(hr) && aRelatedDemoObj.GetCount() == 1)
        {*/
        CRect rcButtonOld = m_pDemoButton->GetArea();//aRelatedDemoObj.GetAt(0)->GetArea();
            float fDiffX = rcButton.left - rcButtonOld.left;
            float fDiffY = rcButton.top - rcButtonOld.top;
            /*aRelatedDemoObj.GetAt(0)*/m_pDemoButton->Move(fDiffX, fDiffY);
        //}
    }
    CalculateObjectPositions();

    return S_OK;
}

void CGraphicalObject::CalculateObjectPositions(bool bRedrawText) {
    DrawSdk::DPoint pLinePoints[8];
    int nFingerHeight = CALLOUT_FINGER_HEIGHT;
    CRect rcCallout = m_rcDimensions;

    if (m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_LEFT || m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_RIGHT) {
        rcCallout.top = m_rcDimensions.bottom;
        rcCallout.bottom = m_rcDimensions.top;
        nFingerHeight = -(nFingerHeight);
    }
    if (m_idGraphicalObjectType == GOT_CALLOUT_RIGHT_BOTTOM || m_idGraphicalObjectType == GOT_CALLOUT_RIGHT_TOP) {
        rcCallout.left = m_rcDimensions.right;
        rcCallout.right = m_rcDimensions.left;
        nFingerHeight = -(nFingerHeight);
    }
    if (m_idGraphicalObjectType == GOT_CALLOUT_TOP_LEFT || m_idGraphicalObjectType == GOT_CALLOUT_TOP_RIGHT || m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_LEFT || m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_RIGHT) {
        pLinePoints[0].x = rcCallout.left;
        pLinePoints[0].y = rcCallout.top;
        //Finger
        if (m_idGraphicalObjectType == GOT_CALLOUT_TOP_LEFT || m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_LEFT) {
            pLinePoints[1].x = rcCallout.left + CALLOUT_FINGER_MARGIN;
            pLinePoints[1].y = rcCallout.top;
            pLinePoints[2].x = rcCallout.left + CALLOUT_FINGER_MARGIN + CALLOUT_FINGER_WIDTH/2;
            pLinePoints[2].y = rcCallout.top - nFingerHeight;
            pLinePoints[3].x = rcCallout.left + CALLOUT_FINGER_MARGIN + CALLOUT_FINGER_WIDTH;
            pLinePoints[3].y = rcCallout.top;
        } else if (m_idGraphicalObjectType == GOT_CALLOUT_TOP_RIGHT || m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_RIGHT) {
            pLinePoints[1].x = rcCallout.right - (CALLOUT_FINGER_MARGIN + CALLOUT_FINGER_WIDTH);
            pLinePoints[1].y = rcCallout.top;
            pLinePoints[2].x = rcCallout.right - (CALLOUT_FINGER_MARGIN + CALLOUT_FINGER_WIDTH/2);
            pLinePoints[2].y = rcCallout.top - nFingerHeight;
            pLinePoints[3].x = rcCallout.right - CALLOUT_FINGER_MARGIN;
            pLinePoints[3].y = rcCallout.top;
        }
        //End finger
        pLinePoints[4].x = rcCallout.right;
        pLinePoints[4].y = rcCallout.top;
        pLinePoints[5].x = rcCallout.right;
        pLinePoints[5].y = rcCallout.bottom;
        pLinePoints[6].x = rcCallout.left;
        pLinePoints[6].y = rcCallout.bottom;
    } else {
        pLinePoints[0].x = rcCallout.left;
        pLinePoints[0].y = rcCallout.top;
        //Finger
        if (m_idGraphicalObjectType == GOT_CALLOUT_LEFT_TOP || m_idGraphicalObjectType == GOT_CALLOUT_RIGHT_TOP) {
            pLinePoints[1].x = rcCallout.left ;
            pLinePoints[1].y = rcCallout.top + CALLOUT_FINGER_MARGIN;
            pLinePoints[2].x = rcCallout.left - nFingerHeight ;
            pLinePoints[2].y = rcCallout.top + CALLOUT_FINGER_MARGIN + CALLOUT_FINGER_WIDTH/2;
            pLinePoints[3].x = rcCallout.left;
            pLinePoints[3].y = rcCallout.top + CALLOUT_FINGER_MARGIN + CALLOUT_FINGER_WIDTH;
        } else if (m_idGraphicalObjectType == GOT_CALLOUT_LEFT_BOTTOM || m_idGraphicalObjectType == GOT_CALLOUT_RIGHT_BOTTOM) {
            pLinePoints[1].x = rcCallout.left ;
            pLinePoints[1].y = rcCallout.bottom - (CALLOUT_FINGER_MARGIN + CALLOUT_FINGER_WIDTH);
            pLinePoints[2].x = rcCallout.left - nFingerHeight;
            pLinePoints[2].y = rcCallout.bottom - (CALLOUT_FINGER_MARGIN + CALLOUT_FINGER_WIDTH/2);
            pLinePoints[3].x = rcCallout.left;
            pLinePoints[3].y = rcCallout.bottom - CALLOUT_FINGER_MARGIN;
        }
        //End finger
        pLinePoints[4].x = rcCallout.left;
        pLinePoints[4].y = rcCallout.bottom;
        pLinePoints[5].x = rcCallout.right;
        pLinePoints[5].y = rcCallout.bottom;
        pLinePoints[6].x = rcCallout.right;
        pLinePoints[6].y = rcCallout.top;
    }
    pLinePoints[7].x = pLinePoints[0].x;
    pLinePoints[7].y = pLinePoints[0].y;

    SetCalloutObjectsParameters(pLinePoints, 8, bRedrawText);
   /* m_pPolygonObject->SetPoints(pLinePoints, 8);

    CString csText = m_pTextObject->GetString();
    Gdiplus::RectF rcOutArea(m_rcDimensions.left, m_rcDimensions.top, m_rcDimensions.Width(), m_rcDimensions.Height());
    Gdiplus::PointF centerPoint;

    m_pTextObject->CalculateCenterPos(csText, csText.GetLength(), &m_logFont, rcOutArea, centerPoint);
    if (centerPoint.X < m_rcDimensions.left) {
        centerPoint.X = m_rcDimensions.left;
    }
    m_pTextObject->SetX(centerPoint.X);
    m_pTextObject->SetY(centerPoint.Y);*/
}

void CGraphicalObject::SetCalloutObjectsParameters(DrawSdk::DPoint *pPoints, int iPointsCount, bool bRedrawText) {
    for (int i = 0; i < m_aDrawObjects.GetCount(); i++) {
        if (m_aDrawObjects.GetAt(i)->GetType() == DrawSdk::RECTANGLE) {
            ((DrawSdk::Rectangle*)m_aDrawObjects.GetAt(i))->SetX(m_rcDimensions.left); 
            ((DrawSdk::Rectangle*)m_aDrawObjects.GetAt(i))->SetY(m_rcDimensions.top); 
            ((DrawSdk::Rectangle*)m_aDrawObjects.GetAt(i))->SetWidth(m_rcDimensions.Width()); 
            ((DrawSdk::Rectangle*)m_aDrawObjects.GetAt(i))->SetHeight(m_rcDimensions.Height());
        }
        if (m_aDrawObjects.GetAt(i)->GetType() == DrawSdk::POLYGON) {
            ((DrawSdk::Polygon*)m_aDrawObjects.GetAt(i))->SetPoints(pPoints, iPointsCount); 
        }
    }
    if (!m_csDisplayName.IsEmpty() && bRedrawText){
        if (m_pWbView == NULL) {
            CreateText(m_csDisplayName);
        } else {
            CreateEditControl(false);
            SetGraphicalObjectText(false);
        }
    }
}

DrawSdk::ColorObject *CGraphicalObject::GetFilledObject() {
    for (int i = 0; i < m_aDrawObjects.GetCount(); i++) {
        if (m_aDrawObjects.GetAt(i)->GetType() != DrawSdk::TEXT) {
            if (((DrawSdk::ColorObject*)m_aDrawObjects.GetAt(i))->GetIsFilled() == true) {
                return (DrawSdk::ColorObject*)m_aDrawObjects.GetAt(i);
            }
        }
    }
    return NULL;
}

DrawSdk::ColorObject *CGraphicalObject::GetOutlinedObject() {
    for (int i = 0; i < m_aDrawObjects.GetCount(); i++) {
        if (m_aDrawObjects.GetAt(i)->GetType() != DrawSdk::TEXT) {
            if (((DrawSdk::ColorObject*)m_aDrawObjects.GetAt(i))->GetIsFilled() == false) {
                return (DrawSdk::ColorObject*)m_aDrawObjects.GetAt(i);
            }
        }
    }
    return NULL;
}

void CGraphicalObject::GetTextObjects(CArray<DrawSdk::Text*, DrawSdk::Text*> &aTexts) {
    for (int i = 0; i < m_aDrawObjects.GetCount(); i++) {
        if (m_aDrawObjects.GetAt(i)->GetType() == DrawSdk::TEXT) {
            aTexts.Add((DrawSdk::Text*)m_aDrawObjects.GetAt(i)->Copy());
        }
    }
}

CRect CGraphicalObject::GetTextRect() {

    CRect rcEdit(0,0,0,0);

    CArray<DrawSdk::Text*, DrawSdk::Text*> aTexts;
    GetTextObjects(aTexts);
    bool bIsDrawSpecial = true;
    if (aTexts.GetCount() > 0) {
        bIsDrawSpecial = aTexts.GetAt(0)->IsDrawSpecial();
    }

    if(bIsDrawSpecial) {
        int x = 10000, y = 10000, x2 = 0, y2 = 0;
        double iMaxAscent = 0;

        for(int i = 0; i < aTexts.GetCount(); i ++)
        {
            DrawSdk::Text* pTextObject = aTexts.GetAt(i);
            double tx, ty;
            ty = pTextObject->GetY();
            if (ty < y) {
                y = ty;
                //rcEdit.bottom = y;
            }
            tx = pTextObject->GetX();
            if ( tx < x) {
                x = tx;
            }

            double tx2,ty2;
            tx2 = tx + pTextObject->GetWidth();
            if(tx2 > x2)
                x2 = tx2;
            ty2 = ty + pTextObject->GetHeight();

            if(ty2 > y2)
                y2 = ty2;
            
            LOGFONT lf;
            pTextObject->GetLogFont(&lf);
            double ascent = pTextObject->GetTextAscent(&lf);

            if (iMaxAscent < ascent)
                iMaxAscent = ascent;
            
        }
        
        rcEdit.SetRect(x, y, x2, y2);
        rcEdit.OffsetRect(0, -iMaxAscent);
    } else {
        DrawSdk::Text* pTextObject = NULL;
        if (aTexts.GetCount() > 0) {
            pTextObject = aTexts.GetAt(0);
            double dAscent = pTextObject->GetTextAscent(&m_logFont);
            rcEdit.top = pTextObject->GetY() - dAscent;
            pTextObject = aTexts.GetAt(aTexts.GetCount()-1);
            rcEdit.bottom = pTextObject->GetY();
        }
        int iMaxLength = 0;
        int iIndex = 0;
        for (int i = 0; i < aTexts.GetCount(); i++) {
            CString csText = aTexts.GetAt(i)->GetString();
            if (csText.GetLength() > iMaxLength) {
                iMaxLength = csText.GetLength();
                iIndex = i;
            }
        }
        pTextObject = aTexts.GetAt(iIndex);
        rcEdit.left = pTextObject->GetX();
        rcEdit.right = rcEdit.left + pTextObject->GetWidth();
    }

    for (int i=0; i<aTexts.GetCount(); i++) {
        delete aTexts[i];
    }
    aTexts.RemoveAll();

    //if (aTexts.GetCount() > 0) {
    //    DrawSdk::Text* pTextObject = NULL;
    //    if (aTexts.GetCount() > 0) {
    //        pTextObject = aTexts.GetAt(0);
    //        //double dAscent = pTextObject->GetTextAscent(&m_logFont);
    //        rcEdit.top = pTextObject->GetY() - dAscent;
    //        pTextObject = aTexts.GetAt(aTexts.GetCount()-1);
    //        rcEdit.bottom = pTextObject->GetY();
    //    }
    //    int iMaxLength = 0;
    //    int iIndex = 0;
    //    for (int i = 0; i < aTexts.GetCount(); i++) {
    //        CString csText = aTexts.GetAt(i)->GetString();
    //        if (csText.GetLength() > iMaxLength) {
    //            iMaxLength = csText.GetLength();
    //            iIndex = i;
    //        }
    //    }
    //    pTextObject = aTexts.GetAt(iIndex);
    //    rcEdit.left = pTextObject->GetX();
    //    rcEdit.right = rcEdit.left + pTextObject->GetWidth();

    //    for (int i=0; i<aTexts.GetCount(); i++) {
    //        delete aTexts[i];
    //    }
    //    aTexts.RemoveAll();
    //}

    return rcEdit; 
}

void CGraphicalObject::CreateEditControl(bool bDoShow) {
    CRect rcEditDimensions = m_rcDimensions;
    rcEditDimensions.left = m_rcDimensions.left;
    rcEditDimensions.top =  0;
    rcEditDimensions.right =  m_rcDimensions.right;
    rcEditDimensions.bottom = 0;

    CRect rcCalloutDimensions = m_rcDimensions;
    if (m_pWbView != NULL) {
        m_pEdit = m_pWbView->CreateGraphicalObjectTextField(rcEditDimensions, rcCalloutDimensions);
    } //else { //created from LC call
    //    m_pEdit = new CGraphicalObjectEditCtrl();
    //    //m_pEdit->Create(WS_CHILD | ES_MULTILINE | ES_CENTER | ES_NOHIDESEL | ES_MULTILINE | ES_WANTRETURN, rcEditDimensions, AfxGetApp()->GetMainWnd(), ID_EDIT_TEXT);
    //    m_pEdit->CreateEx(WS_EX_TOPMOST | WS_EX_NOPARENTNOTIFY ,WS_CHILD | ES_MULTILINE | ES_CENTER | ES_NOHIDESEL | ES_MULTILINE | ES_WANTRETURN, rcEditDimensions , NULL, ID_EDIT_TEXT); 
    //    m_pEdit->SetEventMask(m_pEdit->GetEventMask() | ENM_REQUESTRESIZE);
    //    m_pEdit->SetOffsets(CPoint(0,0));
    //}
    //

    double dZoomFactor = 1.0;
    if (m_pWbView != NULL) {
        dZoomFactor = m_pWbView->GetZoomFactor();
    }
    m_pEdit->SetCalloutDimension(rcCalloutDimensions);
    int iButtonHeight = 0;
    if (/*pDemoModeButton*/m_pDemoButton != NULL && m_bDemoDocumentObject) {
        CRect rcDemoButton = m_pDemoButton->GetArea();
        iButtonHeight = rcDemoButton.Height() * dZoomFactor;
    } else if (m_bDemoDocumentObject) {
        iButtonHeight = 26 * dZoomFactor;
    }
    if (iButtonHeight > 0) {
        m_pEdit->SetDemoModeButtonHeight(iButtonHeight);
    }
    CFont font;
    LOGFONT logFont;
    memcpy(&logFont, &m_logFont, sizeof(LOGFONT));
    
    //logFont.lfHeight = -1 * (long)(abs(logFont.lfHeight) * dZoomFactor + 0.5);
    font.CreateFontIndirect(&logFont);
    m_pEdit->SetFont(&font);
    m_pEdit->SetLogFont(logFont);

   
   /* CHARFORMAT cf;
    memset(&cf, 0, sizeof(CHARFORMAT));
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_COLOR;
    cf.dwEffects = 0;
    m_pEdit->GetDefaultCharFormat(cf);
    cf.crTextColor = m_clrText;
    if (cf.dwEffects & CFE_AUTOCOLOR) {
        cf.dwEffects ^= CFE_AUTOCOLOR;
    }
    m_pEdit->SetDefaultCharFormat(cf);*/
    m_pEdit->SetBackgroundColor(false, GetFilledObject()->GetFillColorRef());
    CString csText = GetInteractionName();
    RemoveTextObjects();
    if (m_pWbView != NULL) {
        m_pWbView->RedrawWindow();
    }
    
   /* CArray<CInteractionAreaEx *, CInteractionAreaEx *> aInteractions;
    CInteractionAreaEx *pDemoModeButton = NULL;
    CInteractionStream *pInteractionStream = m_pEditorProject->GetInteractionStream(false);
    pInteractionStream->FindInteractions(&m_Visibility, false, &aInteractions);
    for (int i = 0; i < aInteractions.GetCount(); i++) {
        CInteractionAreaEx *pInteraction = aInteractions[i];
        if (pInteraction != NULL && pInteraction->GetClassId() == INTERACTION_CLASS_BUTTON && pInteraction->IsDemoDocumentObject()) {
            pDemoModeButton = pInteraction;
            break;
        }
    }*/

    

    
    
    if (m_csRtf.IsEmpty()) {
        m_pEdit->SetWindowText(csText);
    } else {

			//int iPosFind=0,iPosGet = 0, iFontSize;
			//CString csNewRtf,csOldRtf=m_csRtf;
			////m_pEdit->GetRichText(m_pEdit,csOldRtf);
			//iPosFind = csOldRtf.Find(_T("\\fs"),iPosFind);
			//while(iPosFind >=0)
			//{
			//	csNewRtf+=csOldRtf.Mid(iPosGet,iPosFind-iPosGet+3);
			//	iPosGet=csOldRtf.Find(_T(" "),iPosFind);
			//	if(iPosGet < 0)
			//		iPosGet = csOldRtf.Find(_T("\\"),iPosFind+1);
			//	iFontSize =_ttoi(csOldRtf.Mid(iPosFind+3,iPosGet-iPosFind-3));
			//	iFontSize = iFontSize/m_dOrigZoom+0.5;
			//	//m_dLastZoom = dZoomFactor;
			//	iFontSize = iFontSize*dZoomFactor+0.5;
			//	csNewRtf.AppendFormat(_T("%d"),iFontSize);
			//	iPosFind = csOldRtf.Find(_T("\\fs"),iPosGet);
			//}
			//csNewRtf+=csOldRtf.Right(csOldRtf.GetLength()-iPosGet);
			//m_csRtf = csNewRtf;

    char *szString;
    int nLen = ::WideCharToMultiByte(CP_ACP, 0, m_csRtf, -1, NULL, 0, NULL, NULL);
    szString = (char*)malloc((nLen + 1) * sizeof(char));
    ::WideCharToMultiByte(CP_ACP, 0, m_csRtf, -1, szString, nLen + 1, NULL, NULL);
    szString[nLen] = '\0';
 
    m_pEdit->SetRichText(m_pEdit, szString);
    SAFE_DELETE(szString);
    }
    ZoomEdit(dZoomFactor * 100);
	m_dLastZoom = dZoomFactor;
    m_pEdit->CalculatePosition(csText);
    if (m_csRtf.IsEmpty()) {
        
        m_pEdit->SetSel(0,-1);
        //m_pEdit->SetSelectionFontSize(logFont.lfHeight * -1);
        m_pEdit->SetDefaultColor(m_clrText);
    }

    if (bDoShow) {
        m_pEdit->SetFocus();
        m_pEdit->ShowWindow(SW_SHOW);
    }
    
}

void CGraphicalObject::ZoomEdit(int iZoomPercent) {
    long numerator, denominator;

    if (iZoomPercent < 5)
        iZoomPercent = 5;
    if (iZoomPercent > 500)
        iZoomPercent = 500;

    if (iZoomPercent == 100){
        numerator = 0;
        denominator = 0;
    } else if (iZoomPercent < 100) {
        numerator = iZoomPercent;
        denominator = 100;
    } else {
        numerator = iZoomPercent / 1.960748;
        denominator = 51;
    }
    if (m_pEdit != NULL)
        m_pEdit->SendMessage(EM_SETZOOM, numerator, denominator);
}

void CGraphicalObject::CreateText(CArray<SEditLine, SEditLine> &aEditLines) {
    RemoveTextObjects();
    if (aEditLines.GetCount() == 0 )//|| m_pWbView == NULL)
        return;
    int iTotalHeight = 0;
    int yyy = aEditLines.GetCount();
    for (int i = 0; i < aEditLines.GetCount(); i++)
    {
        CString csLine = aEditLines.GetAt(i).csText;
        CRect rcLine = aEditLines.GetAt(i).rcLine;
        COLORREF color = aEditLines.GetAt(i).clr;
        if(m_pWbView != NULL)
            m_pWbView->TranslateToObjectCoordinates(rcLine);
		if (i==0 || i>0 && aEditLines.GetAt(i).iOrigLineNumber != aEditLines.GetAt(i-1).iOrigLineNumber)
			iTotalHeight += aEditLines.GetAt(i).iOrigLineHeight;
        LOGFONT lf;
        memcpy(&lf, &aEditLines.GetAt(i).lf, sizeof(LOGFONT));
        double fZoom = 1.0;
        if (m_pWbView != NULL)
            fZoom = m_pWbView->GetZoomFactor();
		m_dOrigZoom = fZoom;
        //lf.lfHeight = -1 * (long)(abs(lf.lfHeight) / fZoom + 0.5);
        double dTextWidth = DrawSdk::Text::GetTextWidth(csLine, csLine.GetLength(), &lf);//&m_logFont);
        //double dAscent = DrawSdk::Text::GetTextAscent(&lf);
        double dAscent = DrawSdk::Text::GetTextAscent(&lf);
        int iTextTop = rcLine.top + dAscent;
        DrawSdk::Text *pText = new DrawSdk::Text(rcLine.left, /*rcLine.top*/iTextTop , dTextWidth, rcLine.Height(), color/*m_clrText*/, 
            csLine, csLine.GetLength(), &lf);//&m_logFont);
        
        pText->DrawSpecial(true);
        m_aDrawObjects.Add(pText->Copy());
        SAFE_DELETE(pText);
    }

	int iAvailableHeight = m_rcDimensions.Height() - GetOutlinedObject()->GetLineWidth() * 2 - TEXT_BORDER;
    int iCalloutBottom = m_rcDimensions.bottom;

    if(m_pDemoButton != NULL && m_bDemoDocumentObject) {
        int iButtonOffset = iCalloutBottom - m_pDemoButton->GetArea().bottom ;
        iAvailableHeight -= (m_pDemoButton->GetArea().Height() + 2 * iButtonOffset);
		iAvailableHeight -=  TEXT_BORDER;
    } else if (m_bDemoDocumentObject) {
        
        iAvailableHeight -= (BUTTON_MIN_HEIGHT + 2 * TEXT_BORDER);    
    }
    if (iTotalHeight > iAvailableHeight) {
		double dOffset = (iTotalHeight - iAvailableHeight)/2 + 1 + GetOutlinedObject()->GetLineWidth() + TEXT_BORDER;
        /*double dMoveButtonOffset = dOffset;
        if (m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_LEFT || m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_RIGHT ||
            m_idGraphicalObjectType == GOT_CALLOUT_LEFT_BOTTOM || m_idGraphicalObjectType == GOT_CALLOUT_RIGHT_BOTTOM) {
                m_rcDimensions.top -= 2 * dOffset;
                dMoveButtonOffset = 0;
        } else {
            m_rcDimensions.bottom += 2 * dOffset;
            dMoveButtonOffset = 2 * dOffset;
        }*/
        m_rcDimensions.top -= dOffset;
        m_rcDimensions.bottom += dOffset;
        CalculateObjectPositions(false);
        if (m_pDemoButton != NULL && m_bDemoDocumentObject) {
            m_pDemoButton->Move(0, dOffset); 
        }
        if (m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_LEFT || m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_RIGHT ||
            m_idGraphicalObjectType == GOT_CALLOUT_LEFT_BOTTOM || m_idGraphicalObjectType == GOT_CALLOUT_RIGHT_BOTTOM) {
                Move(0, -dOffset);
        } else {
            Move (0, dOffset);
        }
    }
}

void CGraphicalObject::CreateText(CString csText) {
    double xPos = 0.0;
    double yPos = 0.0;
    double dTextWidth = 0.0;
    double dTextHeight = 0.0;
    double dTotalHeight = 0.0;

    CRect rcDim = m_rcDimensions;

    CArray<CString,CString> csTextArray;
    double dAscent = DrawSdk::Text::GetTextAscent(&m_logFont);

    if (csText.Find(_T("\n")) != -1) {
        if (csText.Find(_T("\r\n")) != -1) {
            while (csText.Find(_T("\r\n")) != -1) {
                int nDelim = csText.Find(_T("\r\n"));
                CString strAdd = csText.Left(nDelim);
                if(strAdd.IsEmpty()) {
                    strAdd = _T("\n");
                }
                csTextArray.Add(strAdd);
                csText = csText.Mid(nDelim + 2);
            }
        } else {
            while (csText.Find(_T("\n")) != -1) {
                int nDelim = csText.Find(_T("\n"));
                CString strAdd = csText.Left(nDelim);
                if(strAdd.IsEmpty()) {
                    strAdd = _T("\n");
                }
                csTextArray.Add(strAdd);
                csText = csText.Mid(nDelim + 1);
            }
        }
    } 
    if(csText.IsEmpty()) {
        csText = _T("\n");
    } 
    csTextArray.Add(csText); 
    
    //Automatic adaption of the text to the available space.
    for (int i=0; i<csTextArray.GetCount(); i++) {
        CString csLine = csTextArray[i];
        dTextWidth = DrawSdk::Text::GetTextWidth(csLine, csLine.GetLength(), &m_logFont);
        if (dTextWidth > (m_rcDimensions.Width() - 2*TEXT_BORDER)) {
            CString csNewText;
            int curPos = 0;
            CString resToken = csLine.Tokenize(_T(" "), curPos);
            // no line breaks within single words
            while (resToken != "") {
                CString csText = csNewText;
                csText += resToken + _T(" ");
                double dNewTextWidth = DrawSdk::Text::GetTextWidth(csText, csText.GetLength(), &m_logFont);

                if (dNewTextWidth < (m_rcDimensions.Width() - 2*TEXT_BORDER)) {
                    csNewText = csText;
                } else {
					double dResToken = DrawSdk::Text::GetTextWidth(resToken, resToken.GetLength(), &m_logFont);
					if (dResToken  >= (m_rcDimensions.Width() - 2*TEXT_BORDER)) {
						for (int j=0; j<resToken.GetLength(); j++) {
							csText = csNewText;
							csText.AppendChar(resToken.GetAt(j));
							dNewTextWidth = DrawSdk::Text::GetTextWidth(csText, csText.GetLength(), &m_logFont);
							if (dNewTextWidth < (m_rcDimensions.Width() - 2*TEXT_BORDER)) {
									csNewText.AppendChar(resToken.GetAt(j));
							} else {
									break;
							}
						}
            		}

                    csTextArray[i] = csNewText;
                    csTextArray.InsertAt(i+1, csLine.Mid(csNewText.GetLength()));
                    break;
                }
                resToken = csLine.Tokenize(_T(" "), curPos);
            }
        }
    }
    
    dTotalHeight = DrawSdk::Text::GetTextHeight(csText, csText.GetLength(), &m_logFont);
    dTotalHeight *= csTextArray.GetCount();
    double dAvailableHeight = m_rcDimensions.Height() - GetOutlinedObject()->GetLineWidth() * 2 - TEXT_BORDER;
    int iCalloutBottom = m_rcDimensions.bottom;

   /* CArray<CInteractionAreaEx *, CInteractionAreaEx *> aInteractions;
    CInteractionAreaEx *pDemoModeButton = NULL;
    CInteractionStream *pInteractionStream = m_pEditorProject->GetInteractionStream(false);
    pInteractionStream->FindInteractions(&m_Visibility, false, &aInteractions);
    for (int i = 0; i < aInteractions.GetCount(); i++) {
        CInteractionAreaEx *pInteraction = aInteractions[i];
        if (pInteraction != NULL && pInteraction->GetClassId() == INTERACTION_CLASS_BUTTON && pInteraction->IsDemoDocumentObject()) {
            pDemoModeButton = pInteraction;
            break;
        }
    }*/
    
    int iButtonHeight = 0;
    if (/*pDemoModeButton*/m_pDemoButton != NULL && m_bDemoDocumentObject) {
        //CRect rcDemoButton = m_pDemoButton->GetArea();
        //iButtonHeight = rcDemoButton.Height();
        //dAvailableHeight -= 30;//iButtonHeight; 
        int iButtonOffset = iCalloutBottom - m_pDemoButton->GetArea().bottom ;
        dAvailableHeight -= (m_pDemoButton->GetArea().Height() + 2 * iButtonOffset);
		dAvailableHeight -= TEXT_BORDER;
        iButtonHeight = m_pDemoButton->GetArea().Height();
    } else if (m_bDemoDocumentObject) {
        iButtonHeight = BUTTON_MIN_HEIGHT;//26;
        //dAvailableHeight -= iButtonHeight;    
        dAvailableHeight -= (BUTTON_MIN_HEIGHT + 2 * TEXT_BORDER);
    }
    double dMovedOffset = -1;
    if ((int)dTotalHeight > dAvailableHeight) {
        /*double dOffset = (dTotalHeight - dAvailableHeight)/2;
        m_rcDimensions.top -= dOffset;
        m_rcDimensions.bottom += dOffset;
        CalculateObjectPositions(false);
        if (m_pDemoButton != NULL && m_bDemoDocumentObject) {
            m_pDemoButton->Move(0, dOffset); 
        }*/
        double dOffset = (dTotalHeight - dAvailableHeight)/2 + 1 + GetOutlinedObject()->GetLineWidth() + TEXT_BORDER;
        /*double dMoveButtonOffset = dOffset;
        if (m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_LEFT || m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_RIGHT ||
            m_idGraphicalObjectType == GOT_CALLOUT_LEFT_BOTTOM || m_idGraphicalObjectType == GOT_CALLOUT_RIGHT_BOTTOM) {
                m_rcDimensions.top -= 2 * dOffset;
                dMoveButtonOffset = 0;
        } else {
            m_rcDimensions.bottom += 2 * dOffset;
            dMoveButtonOffset = 2 * dOffset;
        }*/
        dMovedOffset = dOffset;
        m_rcDimensions.top -= dOffset;
        m_rcDimensions.bottom += dOffset;
        CalculateObjectPositions(false);
        if (m_pDemoButton != NULL && m_bDemoDocumentObject) {
            m_pDemoButton->Move(0, dOffset); 
        }
    }
    dAvailableHeight = m_rcDimensions.Height() - iButtonHeight;

    RemoveTextObjects();
    for (int i=0; i<csTextArray.GetCount(); i++) {
        CString csLine = csTextArray[i];
        dTextWidth = DrawSdk::Text::GetTextWidth(csLine, csLine.GetLength(), &m_logFont);
        dTextHeight = DrawSdk::Text::GetTextHeight(csLine, csLine.GetLength(), &m_logFont);
        xPos = m_rcDimensions.left + (m_rcDimensions.Width() - dTextWidth) / 2;
        if (i == 0) {
            yPos = m_rcDimensions.top + (dAvailableHeight - dTotalHeight) / 2 + dAscent;
        } else {
            yPos += dTextHeight;
        }
        DrawSdk::Text *pText = new DrawSdk::Text(xPos, yPos, dTextWidth, dTextHeight, m_clrText, 
            csLine, csLine.GetLength(), &m_logFont);
        m_aDrawObjects.Add(pText->Copy());
        SAFE_DELETE(pText);
    }

    if (dMovedOffset != -1) {
        if (m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_LEFT || m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_RIGHT ||
            m_idGraphicalObjectType == GOT_CALLOUT_LEFT_BOTTOM || m_idGraphicalObjectType == GOT_CALLOUT_RIGHT_BOTTOM) {
                Move(0, -dMovedOffset);
        } else {
            Move(0, dMovedOffset);
        }
    }
    csTextArray.RemoveAll();
}

HRESULT CGraphicalObject::SetGraphicalObjectType(GraphicalObjectType goType) {
    HRESULT hr = S_FALSE;
    if (m_bIsObjectCallout) {
        m_idGraphicalObjectType = goType;
        CalculateObjectPositions();
        hr = S_OK;
    }
    return hr;
}

CString CGraphicalObject::GetLcInfoDisplayText() {
    CString csRet = _T("");
    CLcElementInfo* pLcElementInfo = GetClickObjectLcElementInfo();
    if (pLcElementInfo != NULL) {
        CString csAccType = _T("");
        CString csSapType = pLcElementInfo->GetSapType();

        // handle SAP type if available
        if (csSapType != _T("-1")) {
            CString csSapTypeString = _T("");
            int iPos = 0;
            for (UINT nId = IDS_ROLE_SYSTEM_UNKNOWN; nId <= IDS_ROLE_SYSTEM_DIALOGSHELL; nId++) {
                csSapTypeString.LoadString(nId);
                int curPos = 0;
                CString resToken = csSapTypeString.Tokenize(_T("_"), curPos);
                while (resToken != "") {
                    resToken = csSapTypeString.Tokenize(_T("_"), curPos);
                    if (csSapType.Compare(resToken) == 0) {
                        csAccType.Format(_T("%d"), iPos);
                        pLcElementInfo->SetAccType(csAccType);
                        pLcElementInfo->SetSapType(_T("-1"));
                        break;
                    }
                }
                iPos ++;
            }
        }
        csAccType = pLcElementInfo->GetAccType();
        CString csTypeString = _T("");
        CString csDisplayFormat = _T("");
        int iTypeId = _wtoi(csAccType);
        if (iTypeId == -1 && csSapType.CompareNoCase(_T("-1")) == 0) {
            iTypeId = 0;
        }
        int iPos = 0;
        for (UINT nId = IDS_ROLE_SYSTEM_UNKNOWN; nId <= IDS_ROLE_SYSTEM_DIALOGSHELL; nId++) {
            if (iTypeId == iPos) {
                if (iTypeId == 0 || iTypeId == 5 || iTypeId == 6 || iTypeId == 10) {
                    csRet.LoadString(IDS_CALL_LC_DISPLAY_TEXT1);
                } else {
                    csTypeString.LoadString(nId);
                    csDisplayFormat.LoadString(IDS_CALL_LC_DISPLAY_TEXT2);
                    int iCurPos = 0;
                    csAccType = csTypeString.Tokenize(_T("_"), iCurPos);
                    CString csArticle = csTypeString.Tokenize(_T("_"), iCurPos);
                    CString csControlName = pLcElementInfo->GetName();
                    StringManipulation::DecodeXmlSpecialCharacters(csControlName);
                    csRet.Format(csDisplayFormat, csArticle, csAccType, csControlName/*pLcElementInfo->GetName()*/);
                }
                break;
            }
            iPos++;    
        }
    }
    return csRet;
}

void CGraphicalObject::UpdateLcCalloutText() {
    m_csDisplayName = GetLcInfoDisplayText();
    CreateText(m_csDisplayName);
    if (m_pWbView != NULL) {
        m_pWbView->RedrawWindow();
    }
    if (m_pEditorProject->m_pDoc) {
        m_pEditorProject->m_pDoc->UpdateAllViews(NULL, HINT_CHANGE_PAGE, this);
    }
}

void CGraphicalObject::UpdateICallout(){
    if(m_pWbView != NULL) {
        CreateEditControl(false);
        SetGraphicalObjectText(false);
    } else {
        CreateText(m_csDisplayName);
    }
}

void CGraphicalObject::SetGraphicalObjectText(bool bDoUpdate) {
    if (m_pEdit != NULL) {
        CString csText;
        m_pEdit->GetWindowText(csText);

        if (m_csDisplayName != csText && !m_bIsUserEdited) {
            m_bIsUserEdited = true;
        }
        m_csDisplayName = csText;
        m_pEdit->GetRichText(m_pEdit, m_csRtf);
        m_rcEdit = m_pEdit->GetEditRect();
        CArray<SEditLine,SEditLine>aEditLines;
        m_pEdit->GetEditLines(aEditLines);
        //CreateText(csText);
        CreateText(aEditLines);

        if (m_pEdit->m_hWnd != NULL) {
            m_pEdit->ShowWindow(SW_HIDE);
            m_pEdit->DestroyWindow();
        }

        delete m_pEdit;
        m_pEdit = NULL;

        if (m_pWbView != NULL) {
            m_pWbView->ResetGraphicalObjectKey();
        }
        if (m_pEditorProject->m_pDoc && bDoUpdate) {
            m_pEditorProject->m_pDoc->UpdateAllViews(NULL, HINT_CHANGE_PAGE, this);
        }
    }
}

void CGraphicalObject::HideVisibleEditControl(UINT nPositionMs) {
    bool bContainsMs = m_Visibility.Contains(nPositionMs);
    if (!bContainsMs && m_pEdit != NULL)
        SetGraphicalObjectText(false);
}

HRESULT CGraphicalObject::Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
                        CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
                        LPCTSTR tszExportPrefix, CEditorProject *pProject) {
    HRESULT hr = S_OK;

    if(m_pEdit != NULL) {
        SetGraphicalObjectText();
    }

    _TCHAR *tszIdent = GetSubClassIdent();
    _TCHAR *tszGOType = _T("");

    tszIdent = _T("CALLOUT");/*_T("BUTTON")*/;
    CString csGOType;
    csGOType.Format(_T(" button-type=\"%s\""), m_scaGOType.GetAt(m_idGraphicalObjectType));
    tszGOType = csGOType.GetBuffer();

    static _TCHAR tszOutput[2000]; // hopefully enough
    tszOutput[0] = 0;

    CRect rcGODimensions;
    if (m_bIsObjectCallout) {
        GetCalloutDimensions(rcGODimensions, m_rcDimensions, false);
    } else {
        rcGODimensions.SetRect(m_rcDimensions.TopLeft(), m_rcDimensions.BottomRight());
    }

    _stprintf(tszOutput, _T("<%s x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\"%s"),
        tszIdent,
        rcGODimensions.left, rcGODimensions.top, rcGODimensions.Width(), rcGODimensions.Height(),
        tszGOType);

    if (SUCCEEDED(hr)) {
        hr = pOutput->Append(tszOutput);
    }

    if (m_bDemoDocumentObject && SUCCEEDED(hr)) {
        hr = pOutput->Append(_T(" demoDocumentObject=\"true\""));
    }

    if (SUCCEEDED(hr)) {
        if (m_bIsUserEdited == true) {
            hr = pOutput->Append(_T(" isUserEdited=\"true\""));
        } else {
            hr = pOutput->Append(_T(" isUserEdited=\"false\""));
        }
    }

    if (m_Visibility.GetPeriodId() != INTERACTION_PERIOD_NEVER && SUCCEEDED(hr)) {
        pOutput->Append(_T(" visible=\""));
        hr = m_Visibility.AppendTimes(pOutput);
        pOutput->Append(_T("\""));

        pOutput->Append(_T(" active=\""));
        hr = m_Activity.AppendTimes(pOutput);
        pOutput->Append(_T("\""));
    }

    pOutput->Append(_T(">\n"));

    static _TCHAR tszTimes[200];
    tszTimes[0] = 0;
    m_Visibility.AppendTimes(tszTimes);

    if (!m_csRtf.IsEmpty()) {
        CString csSgmlRtf;
        CString csRtf;
        StringManipulation::EncodeXmlSpecialCharacters(m_csRtf);
        StringManipulation::TransformForSgml(m_csRtf, csSgmlRtf);
        csRtf.Format(_T("<RTF>%s</RTF>\n"), csSgmlRtf);
        pOutput->Append(csRtf);
    }

    DrawSdk::CharArray chArray;

    pOutput->Append(_T("<normal>\n"));
    for (int i = 0; i < m_aDrawObjects.GetCount() && SUCCEEDED(hr); i++) {
        chArray.Reset();
        m_aDrawObjects.GetAt(i)->Write(&chArray, bUseFullImagePath);
        hr = pOutput->Append(chArray.GetBuffer(), chArray.GetLength());
    }
    /*m_pPolygonObject->Write(&chArray, bUseFullImagePath);
    hr = pOutput->Append(chArray.GetBuffer(), chArray.GetLength());
    m_pPolygonObject->SetAutoClose(false);
    m_pPolygonObject->Write(&chArray, bUseFullImagePath);
    hr = pOutput->Append(chArray.GetBuffer(), chArray.GetLength());
    chArray.Reset();
    m_pTextObject->SetVisibility(tszTimes);
    m_pTextObject->Write(&chArray, bUseFullImagePath);
    hr = pOutput->Append(chArray.GetBuffer(), chArray.GetLength());*/

    pOutput->Append(_T("</normal>\n"));
    if (m_pClickObject != NULL) {
        m_pClickObject->Write(pOutput, bDoIdent, bUseFullImagePath, 
            mapImagePaths, tszExportPrefix, pProject);
    }

    /* m_pPolygonObject->SetAutoClose(true);
    chArray.Reset();
    m_pPolygonObject->SetAutoClose(true);*/

    //pOutput->Append(_T("</BUTTON>\n"));
    pOutput->Append(_T("</CALLOUT>\n"));
    if (m_pDemoButton != NULL) {
        m_pDemoButton->Write(pOutput, bDoIdent, bUseFullImagePath,
            mapImagePaths, tszExportPrefix, pProject);
    }

    return hr;
}

void CGraphicalObject::InitGraphicalObjectTypes() {
    if (m_scaGOType.GetCount() == 0) {
        m_scaGOType.Add(_T("callout_top_left"));
        m_scaGOType.Add(_T("callout_top_right"));
        m_scaGOType.Add(_T("callout_bottom_left"));
        m_scaGOType.Add(_T("callout_bottom_right"));
        m_scaGOType.Add(_T("callout_left_bottom"));
        m_scaGOType.Add(_T("callout_left_top"));
        m_scaGOType.Add(_T("callout_right_bottom"));
        m_scaGOType.Add(_T("callout_right_top"));
        m_scaGOType.Add(_T("callout_text"));
    }
}

GraphicalObjectType CGraphicalObject::GetGraphicalObjectTypeFromString(CString csGOType) {
    GraphicalObjectType ct = GOT_CALLOUT_TOP_LEFT;

    for (int i = 0; i < m_scaGOType.GetCount(); i++) {
        if (csGOType.CompareNoCase(m_scaGOType.GetAt(i)) == 0) {
            ct = (GraphicalObjectType) i;
            break;
        }
    }
    return ct;
}

void CGraphicalObject::GetCalloutDimensions(CRect& rcCallout, CRect rcOrig, bool bFromFile) {
    /******************************************************************************
    *This method caclulates the values of callout dimensions when read or written
    *into recording file.
    *The finger dimesions are added to the value written to recording file.
    *The finger dimesions are substracted from the value read from recording file.
    ******************************************************************************/
    
    int iDiffX = CALLOUT_FINGER_HEIGHT;
    int iDiffY = CALLOUT_FINGER_HEIGHT;
    
   

    DrawSdk::Polygon* pFilledObject = (DrawSdk::Polygon*) GetFilledObject();
    if(pFilledObject){
        int iPointsNp ;//= GetPoints(NULL, 0);
        DrawSdk::DPoint *pts = NULL;//(DPoint *)calloc(iPointsNp, sizeof(DrawSdk::DPoint));
        iPointsNp = pFilledObject->GetPoints(NULL, 0);
        pts = (DrawSdk::DPoint *)calloc(iPointsNp, sizeof(DrawSdk::DPoint));
        pFilledObject->GetPoints(pts, 8);
        iDiffX = abs(pts[2].x - pts[3].x);
        iDiffY = abs(pts[2].y - pts[3].y);
    }

    rcCallout.SetRect(rcOrig.TopLeft(), rcOrig.BottomRight());
    if (m_idGraphicalObjectType == GOT_CALLOUT_TOP_LEFT || m_idGraphicalObjectType == GOT_CALLOUT_TOP_RIGHT) {
        if (bFromFile) {
            rcCallout.top += iDiffY;//CALLOUT_FINGER_HEIGHT;
        } else {
            rcCallout.top -= iDiffY;//CALLOUT_FINGER_HEIGHT;
        }
    }
    if (m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_LEFT || m_idGraphicalObjectType == GOT_CALLOUT_BOTTOM_RIGHT) {
        if (bFromFile) {
            rcCallout.bottom -= iDiffY;//CALLOUT_FINGER_HEIGHT;
        } else {
            rcCallout.bottom += iDiffY;//CALLOUT_FINGER_HEIGHT;
        }
    }
    if (m_idGraphicalObjectType == GOT_CALLOUT_LEFT_TOP || m_idGraphicalObjectType == GOT_CALLOUT_LEFT_BOTTOM) {
        if (bFromFile) {
            rcCallout.left += iDiffX;//CALLOUT_FINGER_HEIGHT;
        } else {
            rcCallout.left -= iDiffX;//CALLOUT_FINGER_HEIGHT;
        }
    }
    if (m_idGraphicalObjectType == GOT_CALLOUT_RIGHT_TOP || m_idGraphicalObjectType == GOT_CALLOUT_RIGHT_BOTTOM) {
        if (bFromFile) {
            rcCallout.right -= iDiffX;//CALLOUT_FINGER_HEIGHT;
        } else {
            rcCallout.right += iDiffX;//CALLOUT_FINGER_HEIGHT;
        }
    }
}

void CGraphicalObject::CalculatePositionDemoMode(GraphicalObjectType iGraphicalObjectType, CRect& rcCallout, CRect rcDrawArea) {
    int iPosX = 0;
    int iPosY = 0;
    if (iGraphicalObjectType == GOT_CALLOUT_TOP_LEFT || iGraphicalObjectType == GOT_CALLOUT_TOP_RIGHT) {
        iPosY = rcCallout.top + CALLOUT_FINGER_HEIGHT;
        if (iGraphicalObjectType == GOT_CALLOUT_TOP_LEFT) {
            iPosX = rcCallout.left - (CALLOUT_FINGER_MARGIN + CALLOUT_FINGER_WIDTH/2);
        } else {
            iPosX = rcCallout.left - (rcCallout.Width() - CALLOUT_FINGER_MARGIN - CALLOUT_FINGER_WIDTH/2);
        }
    }
    if (iGraphicalObjectType == GOT_CALLOUT_BOTTOM_LEFT || iGraphicalObjectType == GOT_CALLOUT_BOTTOM_RIGHT) {
        iPosY = rcCallout.top - (rcCallout.Height() + CALLOUT_FINGER_HEIGHT);
        if (iGraphicalObjectType == GOT_CALLOUT_BOTTOM_LEFT) {
            iPosX = rcCallout.left - (CALLOUT_FINGER_MARGIN + CALLOUT_FINGER_WIDTH/2);
        } else {
            iPosX = rcCallout.left - (rcCallout.Width() - CALLOUT_FINGER_MARGIN - CALLOUT_FINGER_WIDTH/2);
        }
    }
    if (iGraphicalObjectType == GOT_CALLOUT_LEFT_TOP || iGraphicalObjectType == GOT_CALLOUT_LEFT_BOTTOM) {
        iPosX = rcCallout.left + CALLOUT_FINGER_HEIGHT;
        if (iGraphicalObjectType == GOT_CALLOUT_LEFT_TOP) {
            iPosY = rcCallout.top - (CALLOUT_FINGER_MARGIN + CALLOUT_FINGER_WIDTH/2);
        } else {
            iPosY = rcCallout.top - (rcCallout.Height() - CALLOUT_FINGER_MARGIN - CALLOUT_FINGER_WIDTH/2);
        }    
    }
    if (iGraphicalObjectType == GOT_CALLOUT_RIGHT_TOP || iGraphicalObjectType == GOT_CALLOUT_RIGHT_BOTTOM) {
        iPosX = rcCallout.left - (rcCallout.Width() + CALLOUT_FINGER_HEIGHT);
        if (iGraphicalObjectType == GOT_CALLOUT_RIGHT_TOP) {
            iPosY = rcCallout.top - (CALLOUT_FINGER_MARGIN + CALLOUT_FINGER_WIDTH/2);
        } else {
            iPosY = rcCallout.top - (rcCallout.Height() - CALLOUT_FINGER_MARGIN - CALLOUT_FINGER_WIDTH/2);
        } 
    }
    if (iPosY < rcDrawArea.top) {
        iPosY = rcDrawArea.top;
    }
    if ((iPosY + rcCallout.Height()) > rcDrawArea.bottom) {
        iPosY = rcDrawArea.bottom - rcCallout.Height();
    }
    rcCallout.MoveToXY(iPosX, iPosY);
}

CLcElementInfo* CGraphicalObject::GetClickObjectLcElementInfo() {
    if (m_pClickObject != NULL) {
        return m_pClickObject->GetLcElementInfo();
    }
    return NULL;
}

CRect CGraphicalObject::GetClickObjectBounds() {
    CRect rcBounds(0, 0, 0, 0);
    if (m_pClickObject != NULL) {
        CLcElementInfo *pLcInfo = m_pClickObject->GetLcElementInfo();
        rcBounds.SetRect(pLcInfo->GetPtLocation(), pLcInfo->GetPtLocation() + pLcInfo->GetPtSize());
    }
    return rcBounds;
}