#include "stdafx.h"
#include "EditorDoc.h"
#include "editor.h"
#include "MiscFunctions.h"
#include "..\Studio\resource.h"
#include "DebugDlg.h"
#include "ImportClipAudioDlg.h"
#include "PageProperties.h"
#include "lutils.h"
#include "timelinectrl.h" // TLC_MOUSEUP
#include "misc.h"
#include "InteractionStream.h"
#include "QuestionStream.h"
#include "WhiteboardView.h"
#include "WordXml.h"            // filesdk
#include "InteractionButtonDialog.h"
#include "ButtonAppearanceSettings.h"
#include "LanguageSupport.h"                // lsutl32
#include "ManualConfigurationDialog.h"      // lsutl32
#include "ScreenGrabbingExtendedHelper.h"   //lsutl32
#include "QuestionnaireEx.h"

#ifdef _STUDIO
#include "..\Assistant2_0\DocumentManager.h"
#include "..\Studio\Studio.h"
#endif
// PZI:
//#include "ScreengrabbingExtendedExternal.h" // lsutl32

//#ifdef _STUDIO
#include "MainFrm.h"
//#endif


void DebugMsg(_TCHAR *tszMessage)
{
   /*
   _TCHAR tszLogPath[MAX_PATH];
   HRESULT hr = ::SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, tszLogPath);
   if (SUCCEEDED(hr) && hr != S_FALSE)
   {
      _tcscat(tszLogPath, _T("\\editor.log.txt"));

      FILE *f = _tfopen(tszLogPath, _T("a+"));
      if (f != NULL && tszMessage != NULL)
      {
         _ftprintf(f, tszMessage);
         fflush(f);
         fclose(f);
      }
   }
   */
}


struct InsertStruct
{
   CEditorProject *pProject;
   LPCTSTR         tszFilename;
   int             timestamp;
};

CEditorProject::CEditorProject()
{
   Init();
}

CEditorProject::~CEditorProject()
{
   Clear();
}

#define MYFREEMEDIATYPE(mt) { \
   if (mt.pbFormat != NULL && mt.cbFormat != 0) \
      FreeMediaType(mt); \
   mt.pbFormat = NULL; \
   mt.cbFormat = NULL; \
   mt.formattype = GUID_NULL; \
}

void CEditorProject::Init()
{
    CSecurityUtils securityUtils;
    CString csOwnVersion;
    csOwnVersion.LoadString(IDS_VERSION_E);
    m_nVersionType = securityUtils.GetVersionType(csOwnVersion);

    m_pExportProgressDialog = NULL;
    m_pDoc = NULL;

    m_pTargetStructureWithClickPages = NULL; 

    m_iObjectMinTimeSpan = 10;
    
    ResetUserSettings();
    ResetProjectVariables();
}

void CEditorProject::ResetUserSettings()
{
   m_nThumbWidth = THUMB_MIDDLE;
}

void CEditorProject::ResetProjectVariables()
{
   m_bOpening = false;
   m_actualPreviewSegment = 0;

   m_pProgress = NULL;
   m_bCancelExportRequested = false;
   m_bLastExportSuccess = false;
   
   ZeroMemory(&m_videoAviSettings.videoFormat, sizeof AM_MEDIA_TYPE);
   ZeroMemory(&m_mtAudioFormat, sizeof AM_MEDIA_TYPE);
   m_videoAviSettings.bIsVideoFormatInitialized = false;
   m_isAudioFormatInitialized = false;
   ZeroMemory(&m_videoBackupAviSettings.videoFormat, sizeof AM_MEDIA_TYPE);
   m_videoBackupAviSettings.bIsVideoFormatInitialized = false;
   
   if (m_clipAviSettings.GetCount() > 0)
   {
      POSITION pos = m_clipAviSettings.GetStartPosition();
      while (pos)
      {
         CString csFilename;
         CAviSettings *pSettings = NULL;
         m_clipAviSettings.GetNextAssoc(pos, csFilename, pSettings);
         if (pSettings)
         {	 
		    // unnecessary as the destructor does that (now)
            //if (pSettings->bIsVideoFormatInitialized)
            //   FreeMediaType(pSettings->videoFormat);
            delete pSettings;
         }
      }
      
      m_clipAviSettings.RemoveAll();
   }
   
   m_videoAviSettings.dwFCC = BI_RGB;
   m_videoAviSettings.keyframeRate = 15;
   m_videoAviSettings.quality = 75;
   m_videoAviSettings.bUseKeyframes = true;
   m_videoAviSettings.bUseSmartRecompression = false;
   m_videoExportGlobalOffset = 0;
   
   m_clipExportUseSmartRecompression = false;
   CEditorApp *pApp = (CEditorApp *) AfxGetApp();
   if (pApp->GetDXVersion() >= 0x00090000)
      m_clipExportStretchDenverClips = false;
   else
      m_clipExportStretchDenverClips = true;
   m_clipExportAutoCalcDenverSize = true;
   
   m_cachedDenverModeFileNames.RemoveAll();
   ZeroMemory(&m_cachedDenverModeMediaType, sizeof AM_MEDIA_TYPE);
   
   m_targetPageWidth  = 640;
   m_targetPageHeight = 480;
   m_pLastDrawnWhiteboard = NULL;
   
   // Code page defaults to system code page
   m_dwCodePage = ::GetACP();

   m_pMarkStream = NULL;
   m_pClipStream = NULL;
   m_pInteractionStream = NULL;
   m_pQuestionStream = NULL;

   if (m_aStreams.GetSize() == 0)
      m_aStreams.SetSize(0, 5);

   if (m_pTargetStructureWithClickPages != NULL)
       delete m_pTargetStructureWithClickPages;
   m_pTargetStructureWithClickPages = NULL;

   m_bUndoLegacyAction = false;

   m_iLastPasteTo = 0;
   m_iLastPasteLength = 0;
   
   m_bSuppressSuccess = false;
   m_hExportFinishEvent = NULL;
   
   m_bShowClickPages = true;
   m_bShowScreenGrabbingAsRecording = true;
   m_nActiveCalloutType = ID_CALLOUT_TOP_LEFT;
   m_nActiveDemoObjectType = ID_DEMOOBJ_TOP_LEFT;

    m_bHasClickPages = false;
    m_bHasAnalysisPages = false;
    m_bRandomTestStarted = false;
    m_bIsRandomTestEnable = true;
}

void CEditorProject::Clear()
{
   m_szFilename.Empty();
   
   for (int i = 0; i < m_previewList.GetSize(); ++i)
      delete m_previewList[i];
   m_previewList.RemoveAll();
   
   for (i = 0; i < m_clipBoard.GetSize(); ++i)
      delete m_clipBoard[i];
   m_clipBoard.RemoveAll();
   
   // delete ecording documents
   POSITION position = m_recordingList.GetHeadPosition();
   while (position)
   {
      CLrdFile *recording = m_recordingList.GetNext(position);
      delete recording;
   }
   m_recordingList.RemoveAll();
   
   // delete audio-, video-, clip- and whiteboard-streams, delete associated metadata
   position = m_streamList.GetHeadPosition();
   while (position)
   {
      CStreamContainer *stream = m_streamList.GetNext(position);
      if (stream)
         delete stream;
      stream = NULL;
   }
   m_streamList.RemoveAll();
   
   // empty undo list
   EmptyUndoList();
   
   // empty redo list
   EmptyRedoList();
   
   m_targetStructure.Clear();
   m_targetMetainfo.Clear();
   m_csStillImage.Empty();
   
   for (i = 0; i < m_clipStream.GetSize(); ++i)
   {
      if (m_clipStream[i])
         delete m_clipStream[i];
   }
   m_clipStream.RemoveAll();
   
   for (i = 0; i < m_aPageStream.GetSize(); ++i)
   {
      if (m_aPageStream[i])
         delete m_aPageStream[i];
   }
   m_aPageStream.RemoveAll();
   
   // First we have to free the pbFormat block (if initialized)
   // of the media types, then we can use the ResetProjectVariables()
   // method to clear them and reset the remaining project
   // variables.
   MYFREEMEDIATYPE(m_videoAviSettings.videoFormat);
   MYFREEMEDIATYPE(m_mtAudioFormat);
   MYFREEMEDIATYPE(m_cachedDenverModeMediaType);

   // the undo manager must be deleted first as it accesses stream objects for that
   m_UndoManager.Clear();

   // set them NULL first so deleting them does not try to access any informations of others
   m_pMarkStream = NULL;
   m_pClipStream = NULL;
   m_pInteractionStream = NULL;
   m_pQuestionStream = NULL;

   // delete all (new) streams
   for (i=0; i<m_aStreams.GetSize(); ++i)
      delete m_aStreams[i];
   m_aStreams.RemoveAll();

   m_ClipboardNew.Clear();
   m_nActiveCalloutType = ID_CALLOUT_TOP_LEFT;
   m_nActiveDemoObjectType = ID_DEMOOBJ_TOP_LEFT;

   CInteractionButtonDialog::Clean();
   
   // Now clear the project:
   ResetProjectVariables();
   
   // Note that ResetUserSettings() is not called here: Any
   // settings that are not project specific will not be
   // reset.
}
	
void CEditorProject::ConvertToDemoDocument() {
    CInteractionStream *pInteractionStream = GetInteractionStream(false);
    if (pInteractionStream == NULL) {
        pInteractionStream = GetInteractionStream(true);
    }

    CRect rcDrawArea(0, 0, m_targetPageWidth, m_targetPageHeight);

    int iLeft = rcDrawArea.left;
    int iTop = rcDrawArea.top;
    int iRight = rcDrawArea.left + rcDrawArea.Width()/8;
    int iBottom = rcDrawArea.top + rcDrawArea.Height()/2;
   
    CRect rcLeftTop(iLeft, iTop, iRight, iBottom);
    CRect rcLeftBottom(iLeft, iTop+rcDrawArea.Height()/2, iRight, iBottom+rcDrawArea.Height()/2);
    
    iLeft = iRight;
    iRight = rcDrawArea.left + rcDrawArea.Width()/2;
    CRect rcTopLeft(iLeft, iTop, iRight, iBottom);
    CRect rcBottomLeft(iLeft, iTop+rcDrawArea.Height()/2, iRight, iBottom+rcDrawArea.Height()/2);
    
    iLeft = iRight;
    iRight = rcDrawArea.left + (rcDrawArea.Width() - rcDrawArea.Width()/8);
    CRect rcTopRight(iLeft, iTop, iRight, iBottom);
    CRect rcBottomRight(iLeft, iTop+rcDrawArea.Height()/2, iRight, iBottom+rcDrawArea.Height()/2);
    
    iLeft = iRight;
    iRight = rcDrawArea.left + rcDrawArea.Width();
    CRect rcRightTop(iLeft, iTop, iRight, iBottom);
    CRect rcRightBottom(iLeft, iTop+rcDrawArea.Height()/2, iRight, iBottom+rcDrawArea.Height()/2);
   
    // Find last click page
    CLcElementInfo *pLcLastClickPage = NULL;
    CStructureInformation *pLastClickPage = NULL;
    if (CStudioApp::IsLiveContextMode()) {  
        for (int i = m_aPageStream.GetSize() -1; i > 1; i--) { // ignore first page. Start from end
            if (m_aPageStream[i] == NULL){
                continue;
            }
            CTimePeriod currentPeriod;
            currentPeriod.Init(INTERACTION_PERIOD_TIME, m_aPageStream[i]->GetBegin(), m_aPageStream[i]->GetEnd());

            CArray<CInteractionAreaEx *, CInteractionAreaEx *> aInteractions;
            pInteractionStream->FindInteractions(&currentPeriod, false, &aInteractions);

            if (aInteractions.GetSize() > 0) {
                CInteractionAreaEx *pInteraction = aInteractions[0];
                if (pInteraction != NULL && pInteraction->GetClassId() == INTERACTION_CLASS_CLICK_OBJECT) {
                    pLcLastClickPage = ((CClickObject *)pInteraction)->GetLcElementInfo();
                    break;
                }
            }
        }
    } else {
        for (int i = m_aPageStream.GetSize() -1; i > 1; i--) { // ignore first page. Start from end
            if (m_aPageStream[i] == NULL){
                continue;
            }
            CStructureInformation *pElement = GetStructureInformationAtTimeStamp(m_aPageStream[i]->GetBegin());
            if (pElement && pElement->GetType() == CStructureInformation::PAGE && pElement->GetBegin() == m_aPageStream[i]->GetBegin()) {
                CString csClickButton = ((CPageInformation *)pElement)->GetPageClickButton();
                if (!csClickButton.IsEmpty()) {
                    pLastClickPage = pElement;
                    break;
                }
            }

        }
    }

    CButtonAppearanceSettings visualSettings;
    for (int i = 0/*1*/; i < m_aPageStream.GetSize(); ++i) { // ignore first page only in case of !IsLiveContextMode()
        if (m_aPageStream[i] == NULL){
            continue;
        }
        if (i == 0 && !CStudioApp::IsLiveContextMode()) {
            continue;
        }
        CLcElementInfo *pLcElementInfo = NULL;
        int iClickVisibilityStart = -1;
        CStructureInformation *pElement = NULL;
        CString csButtonName = _T("");
        int x, y;
        if (CStudioApp::IsLiveContextMode()) { 
            CTimePeriod currentPeriod;
            currentPeriod.Init(INTERACTION_PERIOD_TIME, m_aPageStream[i]->GetBegin(), m_aPageStream[i]->GetEnd() - 1);

            CArray<CInteractionAreaEx *, CInteractionAreaEx *> aInteractions;
            pInteractionStream->FindInteractions(&currentPeriod, false, &aInteractions);

            //if (aInteractions.GetSize() > 0) {
            for (int i = 0; i < aInteractions.GetCount(); i++) {
                CInteractionAreaEx *pInteraction = aInteractions[i];
                if (pInteraction != NULL && pInteraction->GetClassId() == INTERACTION_CLASS_CLICK_OBJECT) {
                    pLcElementInfo = ((CClickObject *)pInteraction)->GetLcElementInfo();
                    iClickVisibilityStart = ((CClickObject *)pInteraction)->GetVisibilityStart();
                    pInteractionStream->DeleteSingle(pInteraction->GetHashKey(), false); 
                    break;
                }
            }
            if (pLcElementInfo != NULL) {
                CRect rcArea;
                rcArea = pLcElementInfo->GetArea();
                x = rcArea.left; 
                y = rcArea.top;
                csButtonName = pLcElementInfo->GetProcessName();
            }
        } else{
            pElement = GetStructureInformationAtTimeStamp(m_aPageStream[i]->GetBegin());
            if (pElement && pElement->GetType() == CStructureInformation::PAGE && pElement->GetBegin() == m_aPageStream[i]->GetBegin()) {
                x = ((CPageInformation *)pElement)->GetPageClickX();
                y = ((CPageInformation *)pElement)->GetPageClickY();
                csButtonName = ((CPageInformation *)pElement)->GetPageClickButton();
            }
        }
        if (!csButtonName.IsEmpty()) {
            CPoint ptClick(x, y);
            GraphicalObjectType iGraphicalObjectType = GOT_NOTHING;
            if(rcLeftTop.PtInRect(ptClick)) {
                iGraphicalObjectType = GOT_CALLOUT_LEFT_TOP;
            }
            if(rcTopLeft.PtInRect(ptClick)) {
                iGraphicalObjectType = GOT_CALLOUT_TOP_LEFT;
            }
            if(rcTopRight.PtInRect(ptClick)) {
                iGraphicalObjectType = GOT_CALLOUT_TOP_RIGHT;
            }
            if(rcRightTop.PtInRect(ptClick)) {
                iGraphicalObjectType = GOT_CALLOUT_RIGHT_TOP;
            }
            if(rcLeftBottom.PtInRect(ptClick)) {
                iGraphicalObjectType = GOT_CALLOUT_LEFT_BOTTOM;
            }
            if(rcBottomLeft.PtInRect(ptClick)) {
                iGraphicalObjectType = GOT_CALLOUT_BOTTOM_LEFT;
            }
            if(rcBottomRight.PtInRect(ptClick)) {
                iGraphicalObjectType = GOT_CALLOUT_BOTTOM_RIGHT;
            }
            if(rcRightBottom.PtInRect(ptClick)) {
                iGraphicalObjectType = GOT_CALLOUT_RIGHT_BOTTOM;
            }
            if (iGraphicalObjectType != GOT_NOTHING) {
                CString csClick;
                bool bLastInteraction;
                if (CStudioApp::IsLiveContextMode()) {
                    csClick = csButtonName;
                    bLastInteraction = (pLcElementInfo == pLcLastClickPage);
                } else {
                    if (csButtonName == _T("left")) {
                        csClick.LoadString(IDS_CLICK_LEFT);
                    } else if (csButtonName == _T("right")) {
                        csClick.LoadString(IDS_CLICK_RIGHT);
                    } else {
                        csClick.LoadString(IDS_CLICK_MIDDLE);
                    }
                    bLastInteraction = (pElement == pLastClickPage);
                }
                //Implementation of new general algorithm for callout time calculation
                //The new algorithm covers both cases of calculation, with and without click information
                //Old implementation is commented out
                int iTimeStamp;
                int iLength, iBegin, iEnd;
                if (CStudioApp::IsLiveContextMode() && iClickVisibilityStart != -1) {
                    /*if (iClickVisibilityStart <= 500) {
                        iTimeStamp = iClickVisibilityStart / 2;
                    } else {
                        iTimeStamp = iClickVisibilityStart - 500;
                    }*/
                    if (iClickVisibilityStart == m_aPageStream[i]->GetBegin()) {
                        if (i == 0) {
                            iBegin = 0;
                        } else {
                            iBegin = m_aPageStream[i-1]->GetBegin();
                        }
                    } else {
                        iBegin = m_aPageStream[i]->GetBegin();
                    }
                    iEnd = iClickVisibilityStart;

                } else {
                    iBegin = m_aPageStream[i-1]->GetBegin();
                    iEnd = m_aPageStream[i-1]->GetEnd();
                    /*int nPageLength = m_aPageStream[i-1]->GetEnd() - m_aPageStream[i-1]->GetBegin();
                    if (nPageLength <= 1000) {
                        iTimeStamp = m_aPageStream[i-1]->GetBegin();
                    }
                    if (nPageLength > 1000 && nPageLength < 1500) {
                        iTimeStamp = m_aPageStream[i]->GetBegin() - 1000;
                    }
                    if (nPageLength >= 1500){
                        iTimeStamp = m_aPageStream[i-1]->GetBegin() + 500;
                    }*/
                }
                iLength = iEnd - iBegin;
                if (iLength <= 1000) {
                    iTimeStamp = iBegin;
                }
                if (iLength > 1000 && iLength < 1500) {
                    iTimeStamp = iEnd - 1000;
                }
                if (iLength >= 1500) {
                    iTimeStamp = iBegin + 500;
                }

                CRect rcCallout(ptClick.x, ptClick.y, ptClick.x + 200, ptClick.y + 120);
                CRect rcDrawArea(0, 0, m_targetPageWidth, m_targetPageHeight);
                CGraphicalObject::CalculatePositionDemoMode(iGraphicalObjectType, rcCallout, rcDrawArea);

                //InsertDemoDocumentStopmark(iTimeStamp);
                InsertDemoDocumentObject(rcCallout, iGraphicalObjectType, csClick, iTimeStamp, bLastInteraction, pLcElementInfo, NULL);

                //CGraphicalObject *pGraphicalObject = new CGraphicalObject();
                //InsertGraphicalObject(pGraphicalObject, rcCallout, iGraphicalObjectType, csClick, true, iTimeStamp, pLcElementInfo);
                //rcCallout = pGraphicalObject->GetArea();

                //CRect rcButton(0, 0, 105, 30); // size of the small buttons (...\LECTURNITY\Studio\ButtonImages\Windows\Small\Blue\Normal.png)
                //int iXpos = rcCallout.left + (rcCallout.Width() - rcButton.Width())/2;
                //int iYPos = rcCallout.bottom - rcButton.Height();
                //rcButton.MoveToXY(iXpos, iYPos);

                //// Insert stop mark for interaction
                //CMarkStream* pMarkStream = GetMarkStream(true);
                //if (pMarkStream) {
                //    CStopJumpMark *pMark = new CStopJumpMark();
                //    pMark->Init(false, NULL, iTimeStamp);
                //    pMark->SetDemoDocumentObject(true);
                //    pMarkStream->IncludeMark(pMark, false);
                //    SAFE_DELETE(pMark);
                //}

                //CInteractionAreaEx *pInteraction = new CInteractionAreaEx();
                //visualSettings.CreateDemoButton(this, pInteraction, rcButton, iTimeStamp);

                //// add undo point for last button inserted. 
                //// Remark: always insert target marks first and "Next" buttons last (just to add rollback point at the end).
                //HRESULT hrInsert = pInteractionStream->InsertSingle(pInteraction, bLastInteraction);
            }
        }
    }

    if (m_pDoc) {
        m_pDoc->SetModifiedFlag();
    }
}

void CEditorProject::InsertDemoDocumentStopmark(int iTimestamp) {
    // Insert stop mark for interaction
    CMarkStream* pMarkStream = GetMarkStream(true);
    if (pMarkStream) {
        CStopJumpMark *pMark = new CStopJumpMark();
        pMark->Init(false, NULL, iTimestamp);
        pMark->SetDemoDocumentObject(true);
        //pMark->SetDemoGroupId(nIndex);
        pMark->RegisterUser();
        pMarkStream->IncludeMark(pMark, false);
        SAFE_DELETE(pMark);
    }
}

void CEditorProject::InsertDemoDocumentObject(CRect rcCallout, GraphicalObjectType iGotType, CString csClick, int iTimestamp, bool bLastInteraction, CLcElementInfo* pLcElementInfo, CGraphicalObject* pGO){


    CInteractionStream *pInteractionStream = GetInteractionStream(false);
    if (pInteractionStream == NULL) {
        pInteractionStream = GetInteractionStream(true);
    }
    CButtonAppearanceSettings visualSettings;
    CGraphicalObject *pGraphicalObject = pGO;
    if (pGraphicalObject == NULL)
        pGraphicalObject = new CGraphicalObject();
    InsertGraphicalObject(pGraphicalObject, rcCallout, iGotType, csClick, true, iTimestamp, bLastInteraction ,pLcElementInfo);
    rcCallout = pGraphicalObject->GetArea();
    //pGraphicalObject->SetDemoGroupId(nIndex);

	CRect rcButton(0, 0, GOT_MIN_WIDTH - 2 * BUTTON_OUT_SPACE, BUTTON_MIN_HEIGHT); // size of the small buttons (...\LECTURNITY\Studio\ButtonImages\Windows\Small\Blue\Normal.png)
    int iXpos = rcCallout.left + (rcCallout.Width() - rcButton.Width())/2;
	int iYPos = rcCallout.bottom - rcButton.Height() - BUTTON_OUT_SPACE;
    rcButton.MoveToXY(iXpos, iYPos);

    //// Insert stop mark for interaction
    //CMarkStream* pMarkStream = GetMarkStream(true);
    //if (pMarkStream) {
    //    CStopJumpMark *pMark = new CStopJumpMark();
    //    pMark->Init(false, NULL, iTimestamp);
    //    pMark->SetDemoDocumentObject(true);
    //    pMark->SetDemoGroupId(nIndex);
    //    pMarkStream->IncludeMark(pMark, false);
    //    SAFE_DELETE(pMark);
    //}

    CInteractionAreaEx *pInteraction = new CInteractionAreaEx();
    visualSettings.CreateDemoButton(this, pInteraction, rcButton, iTimestamp);
	CInteractionAreaEx *pInteractionCopy = pInteraction->Copy();
    pGraphicalObject->AddDemoButton(pInteractionCopy);
	SAFE_DELETE(pInteractionCopy);
    CTimePeriod ctp;
    ctp.Init(INTERACTION_PERIOD_TIME, pGraphicalObject->GetVisibilityStart(), pGraphicalObject->GetVisibilityStart() + pGraphicalObject->GetVisibilityLength());
    pGraphicalObject->SetVisibilityPeriod(&ctp);
	SAFE_DELETE(pInteraction);
    pGraphicalObject->UpdateICallout();

    // add undo point for last button inserted. 
    // Remark: always insert target marks first and "Next" buttons last (just to add rollback point at the end).
    /*HRESULT hrInsert = pInteractionStream->InsertSingle(pInteraction, bLastInteraction);
    pInteraction->SetDemoGroupId(nIndex);*/
}

void CEditorProject::InsertGraphicalObject(CGraphicalObject *pGraphicalObject, CRect &rcRange, GraphicalObjectType iGraphicalObjectType, CString csText, bool bDemoMode, int iTimestampPos, bool bLastInteraction, CLcElementInfo *pLcInfo) {
    if (IsEmpty())
        return;

    CInteractionStream *pInteractionStream = GetInteractionStream(false);
    if (pInteractionStream == NULL) {
        pInteractionStream = GetInteractionStream(true);
    }
        if (iGraphicalObjectType == GOT_NOTHING) {
            switch(GetActiveCalloutType()) {
            case ID_CALLOUT_TOP_LEFT: 
                iGraphicalObjectType = GOT_CALLOUT_TOP_LEFT;
                break;
            case ID_CALLOUT_TOP_RIGHT: 
                iGraphicalObjectType = GOT_CALLOUT_TOP_RIGHT;
                break;
            case ID_CALLOUT_BOTTOM_LEFT: 
                iGraphicalObjectType = GOT_CALLOUT_BOTTOM_LEFT;
                break;
            case ID_CALLOUT_BOTTOM_RIGHT: 
                iGraphicalObjectType = GOT_CALLOUT_BOTTOM_RIGHT;
                break;
            case ID_CALLOUT_LEFT_BOTTOM: 
                iGraphicalObjectType = GOT_CALLOUT_LEFT_BOTTOM;
                break;
            case ID_CALLOUT_LEFT_TOP: 
                iGraphicalObjectType = GOT_CALLOUT_LEFT_TOP;
                break;
            case ID_CALLOUT_RIGHT_BOTTOM: 
                iGraphicalObjectType = GOT_CALLOUT_RIGHT_BOTTOM;
                break;
            case ID_CALLOUT_RIGHT_TOP: 
                iGraphicalObjectType = GOT_CALLOUT_RIGHT_TOP;
                break;
            }
        }
    pGraphicalObject->Init(iGraphicalObjectType, this, rcRange, iTimestampPos, csText, bDemoMode, pLcInfo);
    HRESULT hrInsert = pInteractionStream->InsertSingle(pGraphicalObject, bLastInteraction);
    if (SUCCEEDED(hrInsert)) {
       // pGraphicalObject->SetSelected(true);
    } else {
        ASSERT(false);
    }
}

void CEditorProject::GetGraphicalObjectDefaultSettings(COLORREF &clrText, COLORREF &clrLine, COLORREF &clrFill, LOGFONT &lf, int &iLineWidth, int &iLineStyle) {
    
    bool bTransparent = false;

    bool bSuccess = false;
    _TCHAR tszRegistryEntry[MAX_PATH];
    unsigned long nLenght = MAX_PATH;
    
    CString csTextColor = _T("0xFFFFFF");//AfxGetApp()->GetProfileString(_T("EditorGraphicalObject"), _T("TextColor"), _T("0xFFFFFF"));
    //clrText = StringManipulation::GetColorref(csTextColor, bTransparent);
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("TextColor"), tszRegistryEntry, &nLenght);
    if(bSuccess)
        csTextColor = tszRegistryEntry;
    clrText = StringManipulation::GetColorref(csTextColor, bTransparent);
        


    CString csLineColor = _T("0x385D8A");//AfxGetApp()->GetProfileString(_T("EditorGraphicalObject"), _T("LineColor"), _T("0x385D8A"));
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("LineColor"), tszRegistryEntry, &nLenght);
    if(bSuccess)
        csLineColor = tszRegistryEntry;
    clrLine = StringManipulation::GetColorref(csLineColor, bTransparent);

    CString csFillColor = _T("0x4F81BD");//AfxGetApp()->GetProfileString(_T("EditorGraphicalObject"), _T("FillColor"), _T("0x4F81BD"));
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("FillColor"), tszRegistryEntry, &nLenght);
    if(bSuccess)
        csFillColor = tszRegistryEntry;
    clrFill = StringManipulation::GetColorref(csFillColor, bTransparent);

    CString csFontFamily = _T("Arial");//AfxGetApp()->GetProfileString(_T("EditorGraphicalObject"), _T("FontFamily"), _T("Arial"));
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("FontFamily"), tszRegistryEntry, &nLenght);
    if(bSuccess)
        csFontFamily = tszRegistryEntry;

    CString csFontWeight = _T("");//AfxGetApp()->GetProfileString(_T("EditorGraphicalObject"), _T("FontWeight"), _T(""));
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("FontWeight"), tszRegistryEntry, &nLenght);
    if(bSuccess)
        csFontWeight = tszRegistryEntry;
    
    CString csFontslant = _T("");//AfxGetApp()->GetProfileString(_T("EditorGraphicalObject"), _T("FontSlant"), _T(""));
    bSuccess = LRegistry::ReadStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("FontSlant"), tszRegistryEntry, &nLenght);
    if(bSuccess)
        csFontslant = tszRegistryEntry;

    DWORD dwRegValue;
    int iFontsize = 18;//AfxGetApp()->GetProfileInt(_T("EditorGraphicalObject"), _T("FontSize"), 18);
    bSuccess = LRegistry::ReadDwordRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("FontSize"), &dwRegValue);
    if(bSuccess)
        iFontsize = (int)dwRegValue;
    

    FontManipulation::Create(csFontFamily, iFontsize, csFontWeight, csFontslant, false, lf);

    iLineWidth = 2;//AfxGetApp()->GetProfileInt(_T("EditorGraphicalObject"), _T("LineWidth"), 2);
    bSuccess = LRegistry::ReadDwordRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("LineWidth"), &dwRegValue);
    if(bSuccess)
        iLineWidth = (int)dwRegValue;

    iLineStyle = 0;//AfxGetApp()->GetProfileInt(_T("EditorGraphicalObject"), _T("LineStyle"), 2);
    bSuccess = LRegistry::ReadDwordRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("LineStyle"), &dwRegValue);
    if(bSuccess)
        iLineStyle = (int)dwRegValue;

}
void CEditorProject::SetGraphicalObjectDefaultSettings(COLORREF clrText, COLORREF clrLine, COLORREF clrFill, LOGFONT lf, int iLineWidth, int iLineStyle) {

    bool bSuccess = true;
    CString csColorFormat = _T("0x%02X%02X%02X");
    CString csColor;
    
    csColor.Format(csColorFormat, GetRValue(clrText), GetGValue(clrText), GetBValue(clrText));
    //AfxGetApp()->WriteProfileString(_T("EditorGraphicalObject"), _T("TextColor"), csColor);
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("TextColor"), csColor);

    csColor.Format(csColorFormat, GetRValue(clrLine), GetGValue(clrLine), GetBValue(clrLine));
    //AfxGetApp()->WriteProfileString(_T("EditorGraphicalObject"), _T("LineColor"), csColor);
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("LineColor"), csColor);

    csColor.Format(csColorFormat, GetRValue(clrFill), GetGValue(clrFill), GetBValue(clrFill));
    //AfxGetApp()->WriteProfileString(_T("EditorGraphicalObject"), _T("FillColor"), csColor);
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("FillColor"), csColor);

    //AfxGetApp()->WriteProfileString(_T("EditorGraphicalObject"), _T("FontFamily"), lf.lfFaceName);
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("FontFamily"), lf.lfFaceName);
    //AfxGetApp()->WriteProfileString(_T("EditorGraphicalObject"), _T("FontWeight"), lf.lfWeight == FW_BOLD ? _T("bold") : _T(""));
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("FontWeight"), lf.lfWeight == FW_BOLD ? _T("bold") : _T(""));
    //AfxGetApp()->WriteProfileString(_T("EditorGraphicalObject"), _T("FontSlant"), lf.lfItalic == (BYTE)true ? _T("italic") : _T(""));
    bSuccess = LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("FontSlant"), lf.lfItalic == (BYTE)true ? _T("italic") : _T(""));
    //AfxGetApp()->WriteProfileInt(_T("EditorGraphicalObject"), _T("FontSize"), -1 * lf.lfHeight);
    bSuccess = LRegistry::WriteDwordRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("FontSize"), (DWORD)(-1 * lf.lfHeight));

    //AfxGetApp()->WriteProfileInt(_T("EditorGraphicalObject"), _T("LineWidth"), iLineWidth);
    bSuccess = LRegistry::WriteDwordRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("LineWidth"), (DWORD)iLineWidth);
    //AfxGetApp()->WriteProfileInt(_T("EditorGraphicalObject"), _T("LineStyle"), iLineStyle);
    bSuccess = LRegistry::WriteDwordRegistryEntry(HKEY_CURRENT_USER,
        _T("Software\\imc AG\\LECTURNITY\\Studio\\EditorGraphicalObject"),
        _T("LineStyle"), (DWORD)iLineStyle);
}

CMarkStream* CEditorProject::GetMarkStream(bool bCreate)
{
   if (m_pMarkStream == NULL && bCreate)
   {
      m_pMarkStream = new CMarkStream();
      m_pMarkStream->Init(this, &m_UndoManager, GetTotalLength());
      
      // Note: normally (insert lrd, read lep) GetTotalLength() returns 0 here.
      // However during for example mark insertion there will be already
      // a project length.

      m_aStreams.Add(m_pMarkStream);
   }

   return m_pMarkStream;
}

CClipStreamR* CEditorProject::GetClipStream(bool bCreate)
{
   if (m_pClipStream == NULL && bCreate)
   {
      m_pClipStream = new CClipStreamR();
      m_pClipStream->Init(this, &m_UndoManager, GetTotalLength());

      m_aStreams.Add(m_pClipStream);
   }

   return m_pClipStream;
}

CInteractionStream* CEditorProject::GetInteractionStream(bool bCreate)
{
   if (m_pInteractionStream == NULL && bCreate)
   {
      m_pInteractionStream = new CInteractionStream();
      m_pInteractionStream->Init(this, &m_UndoManager, GetTotalLength());

      m_aStreams.Add(m_pInteractionStream);
   }

   return m_pInteractionStream;
}

CQuestionStream* CEditorProject::GetQuestionStream(bool bCreate)
{
   if (m_pQuestionStream == NULL && bCreate)
   {
      m_pQuestionStream = new CQuestionStream();
      m_pQuestionStream->Init(this, &m_UndoManager, GetTotalLength());

      m_aStreams.Add(m_pQuestionStream);
   }

   return m_pQuestionStream;
}

bool CEditorProject::StartOpen(LPCTSTR tszFilename)
{
   m_hDialogInitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   m_pProgress = new CProgressDialogE(CMainFrameE::GetCurrentInstance(), this);
   
   InsertStruct insertStruct;
   insertStruct.pProject = this;
   insertStruct.tszFilename = tszFilename;
   insertStruct.timestamp = 0;
   
   CWinThread *thread = AfxBeginThread(OpenThread, &insertStruct);
   m_pProgress->DoModal();
   
   delete m_pProgress;
   m_pProgress = NULL;
   CloseHandle(m_hDialogInitEvent);
   m_hDialogInitEvent = NULL;
   
   if (!m_bOpenSuccess)
      return false;
   
   return true;
}

UINT CEditorProject::OpenThread(LPVOID pParam)
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   InsertStruct *pInsertStruct = (InsertStruct *)pParam;
   
   CEditorProject *pProject = pInsertStruct->pProject;
   
   pProject->m_bOpenSuccess = pProject->Open(pInsertStruct->tszFilename);
   
   if (pProject->m_pProgress)
      PostMessage(pProject->m_pProgress->GetSafeHwnd(), WM_USER, CProgressDialogE::END_DIALOG, NULL);
   
   return 0;
}

bool CEditorProject::Open(LPCTSTR tszFilename, IProgressListener *pListener) {
    m_bOpening = true;
    bool bShowFormatMessage = pListener == NULL;

    if (pListener == NULL)
        pListener = this; // default case

    if (m_hDialogInitEvent != NULL && m_hDialogInitEvent != INVALID_HANDLE_VALUE)
        ::WaitForSingleObject(m_hDialogInitEvent, 10000);

    if (!IsEmpty())
        Close();

    if (pListener != NULL) {
        pListener->SetProgress(0.0f);
        pListener->SetProgressLabel(IDS_OPEN_PROJECT_E);
    }

    CLepSgml projectFile;
    if (!projectFile.Read(tszFilename, _T("<edi"))) {
        CString msg;
        msg.Format(IDS_MSG_OPEN_PROJECT_FAILED, tszFilename);

        if (pListener != NULL)
            pListener->ShowMessage(PROGRESS_MSG_ERROR, msg, m_pProgress);
        m_bOpening = false;
        return false;
    }

    m_szFilename = tszFilename;

    if (!CheckVersion(&projectFile, bShowFormatMessage)) {
        m_bOpening = false;
        // Error message is handled there
        return false;
    }

    // Normal case of project opening
    if (!ParseMetainfo(&projectFile)) {
        m_bOpening = false;
        // TODO?: Fehlermeldung
        return false;
    }

    if (!ParseFiles(&projectFile, tszFilename)) {
        m_bOpening = false;
        // TODO?: Fehlermeldung
        return false;
    }

    if (!ParseExportSettings(&projectFile, tszFilename)) {
        m_bOpening = false;
        // TODO?: Fehlermeldung
        return false;
    }

    if (pListener != NULL)
        pListener->SetProgress(0.05f); // 5%

    bool bSuccess = ParseStreams(&projectFile, tszFilename, pListener);
    if (!bSuccess) {
        if (pListener != NULL)
            pListener->ShowMessage(PROGRESS_MSG_ERROR, IDS_MSG_PARSE_STREAMS_FAILED, m_pProgress);
        m_bOpening = false;
        return false;
    }

    // should be the last entry to be parsed
    if (!ParseConfig(&projectFile)) {
        // TODO?: Fehlermeldung
        m_bOpening = false;
        return false;
    }

    if (bSuccess)
        UpdateStructureAndStreams(); // probably (but not always!) be called later again

    if (pListener != NULL)
        pListener->SetProgress(1.0f); // finished

    bool bBlockerUndo = true; // not possible to take back
    m_UndoManager.SetRollbackPoint(bBlockerUndo);
    m_bOpening = false;

    return true;
}

void CEditorProject::Import(LPCTSTR tszFilename, bool bUpdateStructure)
{
   Close();
   
   CString fileSuffix;
   fileSuffix = tszFilename;
   StringManipulation::GetFileSuffix(fileSuffix);
   fileSuffix.MakeUpper();
   
   // moved to CEditorDoc::OnOpenDocument (editorDoc.cpp) 
   // because LRD file is opened here and read-only
   //// TODO: error handling
   //CScreengrabbingExtendedExternal::RunPostProcessing(tszFilename);

   StartInsertLrd(tszFilename);
   
   CreateDefaultProject();

   if (bUpdateStructure)
       UpdateStructureAndStreams();
}

void CEditorProject::Close()
{
   Clear();
   CWhiteboardStream::m_pageCounter    = 0;
   CWhiteboardStream::m_objectCounter  = 0;
   CStructureInformation::m_iIdCounter = 0;
}

void CEditorProject::Save(LPCTSTR tszFilename, TCHAR *tszLastExportPath)
{
   if (tszFilename == NULL)
       tszFilename = this->m_szFilename;
   // TODO: This should be more general: Saving always selects the filename automatically.
   //       Currently this is called with NULL by Assistant mode->Publish().

   CString lepPath;
   CString newPrefix;
   
   lepPath = tszFilename;
   StringManipulation::GetPath(lepPath);
   
   newPrefix = tszFilename;
   StringManipulation::GetFilePrefix(newPrefix);
   
   SaveProject(lepPath, newPrefix, tszLastExportPath);
}

bool CEditorProject::SaveAs(LPCTSTR tszNewLrdPathName) {
    if (tszNewLrdPathName == NULL)
        return false;

    CString csTargetPath = tszNewLrdPathName;
    StringManipulation::GetPath(csTargetPath);

    CString csSourcePath = m_szFilename;
    StringManipulation::GetPath(csSourcePath);

    if (csTargetPath == csSourcePath) {
        CWnd *pWndParent = CMainFrameE::GetActiveWindow();
        HWND hwndParent = NULL;
        if (pWndParent != NULL)
            hwndParent = pWndParent->GetSafeHwnd();

        CString msg;
        msg.LoadString(IDS_MSG_SAVEAS_SAMEDIR);
        CEditorDoc::ShowErrorMessage(msg, hwndParent);

        return false;
    }

    csSourcePath += _T("\\*.*"); // otherwise only the directory itself is copied (non-recursing)

    CString csSourceName = m_szFilename;
    if (!csSourceName.IsEmpty()) {
        // make sure it is the lrd file:
        if (csSourceName.GetLength() >= 4 && csSourceName[csSourceName.GetLength() - 4] == _T('.'))
            csSourceName = csSourceName.Left(csSourceName.GetLength() - 4) + _T(".lrd");
    }

    // TODO: Maybe check if the directory is empty and if not create a subdirectory with the name?

    bool bSuccess = LIo::CopyDir(csSourcePath, csTargetPath, true);

    CString csSourceNameOnly = csSourceName;
    StringManipulation::GetFilename(csSourceNameOnly);

    CString csTargetNameOnly = tszNewLrdPathName;
    StringManipulation::GetFilename(csTargetNameOnly);

    bool bNamesDiffer = csSourceNameOnly != csTargetNameOnly;

    if (bSuccess && bNamesDiffer) {
        ::CopyFile(csSourceName, tszNewLrdPathName, FALSE); // make sure the file name is correct

        CString csSurplusLrdPath = csTargetPath;
        csSurplusLrdPath += _T("\\");
        csSurplusLrdPath += csSourceNameOnly;

        ::DeleteFile(csSurplusLrdPath);
    }

    // Note: Paths to external files (action target) will work: They are stored as complete path.

    bool bIsModified = true;
    if (m_pDoc != NULL && !m_pDoc->IsModified())
        bIsModified = false;

    CString csOldLepName = m_szFilename;
    bool bOldLepExists = _taccess(csOldLepName, 00) == 0;

    if (bSuccess && (bIsModified || bOldLepExists)) {
        Save(tszNewLrdPathName); // saves with .lep

        // TODO: Check for error (change the Save() method).

        // TODO: This then might have a surplus .lep (the one with the old name from the orig directory)
        //       You might use bNamesDiffer from above as trigger.

        if (m_pDoc != NULL)
            m_pDoc->SetModifiedFlag(FALSE);
    }

#ifdef _STUDIO
    if (bSuccess && !CStudioApp::IsLiveContextMode()) {
        CDocumentManager docManager(AfxGetApp());
        docManager.AddRecordingToRegistry(tszNewLrdPathName);
    } // else // Do nothing related to media library (LC has its own)
#endif

    // TODO: This will probably not work for additional "external" LRDs used in the LEP

    // TODO: this is partly blocking (copy) and partly not (Save())
    // TODO: This takes quite some time (CopyDir())

    return bSuccess;
}


HRESULT CEditorProject::SaveRecordingAsWord(LPCTSTR tszWordFilename, LPCTSTR tszDocumentTitle) {
    HRESULT hr = S_OK;

    UINT nResult = CManualConfigurationDialog::ShowItModal(GetManualConfiguration());
    if (nResult != IDOK) {
        if (nResult == IDCANCEL)
            return E_ABORT;
        else
            return E_FAIL;
    }
    CString csWordName;
    if (tszWordFilename == NULL) {
        CString csFilter = _T("Microsoft Word Dokument (*.doc)|*.doc|Alle Dateien (*.*)|*.*||");
        CString csInitialDir = CMainFrameE::GetCurrentInstance()->GetInitialLepDir();
        FileDialog::GetSaveFilename(AfxGetApp(), csWordName, AfxGetMainWnd()->GetSafeHwnd(),
            AfxGetResourceHandle(), csFilter, 
            _T(".doc"), _T("DOC"), csInitialDir);
    } else {
        csWordName = tszWordFilename;
    }

    if (csWordName.IsEmpty())
        hr = E_FAIL;

    CArray<CWordSlideObject *, CWordSlideObject *> aPage;
    for (int i = 0; i < m_aPageStream.GetCount(); ++i) {
            CPage *pPage = m_aPageStream[i];
            if (pPage != NULL) {
            CWordSlideObject *pNewPage = new CWordSlideObject();
            pNewPage->SetChapterTitle(pPage->GetTitle());

            // Add image
                int iPageBegin = pPage->GetBegin();
                int iPageEnd = pPage->GetEnd();
                CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> aObjects;
                GetWhiteboardElements(iPageBegin, iPageEnd, aObjects);
                bool bFirstElement = true;
                for (int j = 0; j < aObjects.GetCount(); ++j) {
                    if (aObjects[j]->GetType() == DrawSdk::IMAGE) {
                    pNewPage->AddImage((DrawSdk::Image *)aObjects[j]);
                    }
                }
                aObjects.RemoveAll();

            // Add callout texts
                CArray<CGraphicalObject *, CGraphicalObject *> aGraphicalObject;
                CGraphicalObject * pGraphicalObjectDemo = NULL;
                GetGraphicalObjects(iPageBegin, iPageEnd, aGraphicalObject);
                for (int j = 0; j < aGraphicalObject.GetCount(); ++j) {
                    CArray<DrawSdk::Text *, DrawSdk::Text *> aTexts;
                    aGraphicalObject[j]->GetTextObjects(aTexts);
                    bool bFirstText = true;
                    for (int k = 0; k < aTexts.GetCount(); ++k) {
                        if (aTexts[k]->GetType() == DrawSdk::TEXT) {
                        pNewPage->AddText((DrawSdk::Text *)aTexts[k]);
                        }
                        delete aTexts[k];
                    }
                // There should be space between the different texts
                    
                    pNewPage->AddText(NULL);
                    aTexts.RemoveAll();
                    if (aGraphicalObject[j]->IsDemoDocumentObject()) {
                        pGraphicalObjectDemo = aGraphicalObject[j];
                    }
                }
                if (pGraphicalObjectDemo != NULL) {
                    CRect rcClick = pGraphicalObjectDemo->GetClickObjectLcElementInfo()->GetArea();
                    if (!rcClick.IsRectEmpty()) {
                        CPoint ptClick(rcClick.left, rcClick.top);
                        pNewPage->SetClickPoint(ptClick);
                    }
                    CRect rcBounds = pGraphicalObjectDemo->GetClickObjectBounds();
                    if (!rcBounds.IsRectEmpty()) {
                        pNewPage->SetClickObjectBounds(rcBounds);
                    } else if (!rcClick.IsRectEmpty()) {
                        CRect newRcBounds(rcClick.left, rcClick.top, rcClick.left, rcClick.top);
                        newRcBounds.InflateRect(CSize(37, 8));
                        pNewPage->SetClickObjectBounds(newRcBounds);
                    }
                }
                aGraphicalObject.RemoveAll();

                
            aPage.Add(pNewPage);
            }
        }

    CWordXml *pWordXml = new CWordXml(GetManualConfiguration(), tszDocumentTitle);
    hr = pWordXml->Write(csWordName, m_targetMetainfo, aPage);
    delete pWordXml;

    for (int i = 0; i < aPage.GetCount(); ++i) {
        if (aPage[i] != NULL)
            delete aPage[i];
    }
    aPage.RemoveAll();

    return hr;
}

/*
void CEditorProject::ExportSilent(const _TCHAR *tszTargetLrdPathName) {
    if (tszTargetLrdPathName == NULL)
        return;

    m_exportFileName = tszTargetLrdPathName;
    ExportThread();
}


struct RenameThreadParams {
    CEditorProject *pProject;
    CString csSourceDir;
    CString csTargetDir;
    HANDLE hExportFinish;
};


bool CEditorProject::OpenAndExportAutomatic(_TCHAR *tszSourcePathName, bool bBlocking) {
    // Opens and exports to a unique intermediate directory.
    // After that this directory is moved to a name that matches the export time in the lep.

    if (tszSourcePathName == NULL || _taccess(tszSourcePathName, 04) != 0)
        return false;

    // TODO: This is blocking in any case. Should there be a non-blocking variant?
    ASSERT(bBlocking);
    bool bOpenSuccess = Open(tszSourcePathName);

    if (!bOpenSuccess)
        return false;
    
    return ExportAutomatic(bBlocking);
}
*/

bool CEditorProject::ExportAutomatic(IProgressListener *pListener, bool bSuppressSuccess, HANDLE hExportFinish) {
    // Exports to a unique intermediate directory.
    // After that this directory is moved to a name that matches the export time in the lep.

    CString csAutoLrdPath = m_szFilename;
    StringManipulation::GetPath(csAutoLrdPath);

    CString csTargetPath = csAutoLrdPath;

    __int64 lTimeMs = LMisc::GetLocalTimeMs();

    CString csAppend;
    csAppend.Format(_T("\\Export_%I64d\\"), lTimeMs);
    csAutoLrdPath += csAppend;

    if (_taccess(csAutoLrdPath, 00) != 0)
        ::CreateDirectory(csAutoLrdPath, NULL);
    // TODO: error management/error message?

    CString csAutoLrdPathName = csAutoLrdPath;
    csAppend = m_szFilename;
    StringManipulation::GetFilePrefix(csAppend);
    csAutoLrdPathName += csAppend;
    csAutoLrdPathName += _T(".lrd");

    bool bSuccess = true;

    if (hExportFinish == NULL || hExportFinish == INVALID_HANDLE_VALUE)
        hExportFinish = ::CreateEvent(NULL, FALSE, FALSE, _T("FinishExport"));

    bool bDoHidden = pListener != NULL; // the Powertrainer mode

    bSuccess = Export(csAutoLrdPathName, bDoHidden, pListener, bSuppressSuccess, hExportFinish); // this is blocking (Dlg->DoModal())

    if (bSuccess) {
        // TODO: Waiting is a bit superfluous here as Export() is blocking

        DWORD dwFinish = WAIT_OBJECT_0;
        if (hExportFinish != NULL)
            dwFinish = ::WaitForSingleObject(hExportFinish, INFINITE);
    }

    ::CloseHandle(hExportFinish);

    
    return bSuccess;
}

/*
//static 
UINT CEditorProject::RenameThreadLauncher(LPVOID pParam) {
    HRESULT hr = CLanguageSupport::SetThreadLanguage(); // probably not needed

    RenameThreadParams *pRenameParams = (RenameThreadParams *)pParam;

    pRenameParams->pProject->RenameThread(pRenameParams->csSourceDir,
        pRenameParams->csTargetDir, pRenameParams->hExportFinish);

    delete pRenameParams;

    return S_OK;
}

// Note: The CStrings must be copied, the struct for transmitting them may vanish
void CEditorProject::RenameThread(CString csSourceDir, CString csTargetDir, HANDLE hExportFinish) {
    DWORD dwFinish = WAIT_OBJECT_0;
    
    if (hExportFinish != NULL)
        dwFinish = ::WaitForSingleObject(hExportFinish, INFINITE);

    if (WAIT_OBJECT_0 == dwFinish) { // everything alright
        CString csAppend;
        csAppend.Format(_T("\\Export_%I64d\\"), m_lLastExportMs);

        csTargetDir += csAppend;

        ::MoveFileEx(csSourceDir, csTargetDir, MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);
    }

    ::CloseHandle(hExportFinish);
}
*/

bool CEditorProject::Export(LPCTSTR tszFilename, bool bDoHidden, IProgressListener *pListener, 
                            bool bSuppressSuccess, HANDLE hExportFinish)
{
   m_bCancelExportRequested = false;
   m_bLastExportSuccess = false;
   m_exportFileName = tszFilename;   
   m_hExportFinishEvent = hExportFinish;
   
   m_bSuppressSuccess = bSuppressSuccess;
   if (bDoHidden)
       m_bSuppressSuccess = true;

   DWORD dwStatusXvid = 0;
   LRegistry::ReadDwordRegistryEntry(
      HKEY_CURRENT_USER, _T("Software\\GNU\\Xvid"), _T("display_status"), &dwStatusXvid);

   if (dwStatusXvid != 0)
   {
      // turn display of Xvid status window off
      LRegistry::WriteDwordRegistryEntry(
         HKEY_CURRENT_USER, _T("Software\\GNU\\Xvid"), _T("display_status"), 0);
   }
   
   if (!bDoHidden) {
       m_hDialogInitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
       m_pExportProgressDialog = new CExportProgressDialog(this, AfxGetMainWnd());
       AfxBeginThread(ExportThreadLauncher, this, 0, 0, 0, NULL);
       m_pExportProgressDialog->DoModal();
       delete m_pExportProgressDialog;
       m_pExportProgressDialog = NULL;
       CloseHandle(m_hDialogInitEvent);
       m_hDialogInitEvent = NULL;
   } else {
       // As of 4.0.p1 this is the "Powertrainer mode"

       ExportThread(pListener);
   }

   if (dwStatusXvid != 0)
   {
      // turn display of Xvid status window on again
      LRegistry::WriteDwordRegistryEntry(
         HKEY_CURRENT_USER, _T("Software\\GNU\\Xvid"), _T("display_status"), dwStatusXvid);
   }

   return m_bLastExportSuccess;
}

UINT CEditorProject::ExportThreadLauncher(LPVOID pData)
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   CEditorProject *pProject = (CEditorProject *) pData;
   pProject->ExportThread(NULL); // NULL: use old standard mechanism
   AfxEndThread(0, TRUE);
   return 0;
}

void CEditorProject::ExportThread(IProgressListener *pListener)
{
    // TODO m_pExportProgressDialog should not be handled differntly than m_pProgress in Open()
    //if (pListener == NULL)
    //    pListener = this;

   HWND hwndParent = NULL;

   // As we need the dialog, wait for it to initialize
   if (m_pExportProgressDialog != NULL) {
       DWORD res = ::WaitForSingleObject(m_hDialogInitEvent, INFINITE);
       hwndParent = m_pExportProgressDialog->GetSafeHwnd();
   }
   
   // This is an other thread than normal, so we have
   // to call CoInitialize()
   HRESULT hr = ::CoInitialize(NULL);
   
   bool success = true;
   
   if (pListener != NULL)
       pListener->SetProgressLabel(IDS_EXPORT_WHITEBOARD);
   else if (m_pExportProgressDialog != NULL)
       m_pExportProgressDialog->SetParticularLabel(IDS_EXPORT_WHITEBOARD);
   
   if (FAILED(hr))
   {
      CString msg;
      msg.Format(IDS_MSG_COINIT_FAILED, hr);
      CEditorDoc::ShowErrorMessage(msg, hwndParent);

      if (m_pExportProgressDialog != NULL)
          PostMessage(hwndParent, WM_USER, CExportProgressDialog::END_DIALOG, NULL);
      return;
   }
   /* Moved to ExportVideo() which initilizes m_videoStreamLength. Why was it here?
   if ((HasVideo() && m_videoStreamLength <= 0))
   {
	   CString csMsg;
	   csMsg.LoadString(IDS_MSG_AVEXPORT_FAILED);
	   CEditorDoc::ShowWarningMessage(csMsg, hwndParent);
   }
   else*/
   {
	   CString exportPath = m_exportFileName;
	   StringManipulation::GetPath(exportPath);
	   
	   CString newPrefix = m_exportFileName;
	   StringManipulation::GetFilePrefix(newPrefix);
	   
	   
	   CArray<UINT, UINT> aClipTimes;
	   if (HasSgClips())
	   {
		  if (GetClipStream() != NULL)
		  {
			 GetClipStream()->MergeClipsForExport(&m_clipAviSettings);

			 GetClipStream()->FillExportClipTimes(&aClipTimes);
		  }
	   }
	   
	   UINT nClipCount = 0;
	   if (GetClipStream() != NULL)
		  nClipCount = GetClipStream()->GetExportClipFileCount();
	   
       int iProgressSteps = 3; // wb, lmd, audio

       // Set up the total number of video streams to export:
       int totalVideoCount = IsDenverDocument() ? 1 : nClipCount;
       if (HasVideo())
           totalVideoCount++;

       iProgressSteps += totalVideoCount;
       
       if (m_pExportProgressDialog != NULL) {

           m_pExportProgressDialog->SetTotalVideoCount(totalVideoCount);
           m_pExportProgressDialog->SetHasVideo(HasVideo());

           m_pExportProgressDialog->SetOverallSteps(iProgressSteps);
       }
  
	   if (success && !IsDenverDocument() && !m_bCancelExportRequested)
		  success = ExportWhiteboard(exportPath, newPrefix, pListener);
	     
       if (pListener != NULL) {
           pListener->SetProgressLabel(IDS_EXPORT_LMD);
           pListener->SetProgress(1.0f/iProgressSteps);
       } else if (m_pExportProgressDialog != NULL) {
	       m_pExportProgressDialog->SetOverallProgress(1);
           m_pExportProgressDialog->SetParticularLabel(IDS_EXPORT_LMD);
       }
	   
	   if (success && !m_bCancelExportRequested)
		  success = ExportMetadata(exportPath, newPrefix, &aClipTimes, pListener);
	   
	   if (success && !m_bCancelExportRequested)
		  success = ExportLrd(exportPath, newPrefix, pListener);

       if (pListener != NULL) {
           pListener->SetProgressLabel(IDS_EXPORT_AUDIO);
           pListener->SetProgress(2.0f/iProgressSteps);
       } else if (m_pExportProgressDialog != NULL) {
           m_pExportProgressDialog->SetOverallProgress(2);
           m_pExportProgressDialog->SetParticularLabel(IDS_EXPORT_AUDIO);
       }

	   if (success && !m_bCancelExportRequested)
		  success = ExportAudio(exportPath, newPrefix, pListener, hwndParent);
      // Also exports video!
	   
       if (pListener != NULL)
           pListener->SetProgress(3.0f/iProgressSteps);
       else if (m_pExportProgressDialog != NULL)
           m_pExportProgressDialog->SetOverallProgress(3);

	   //ExportVideo(exportPath, newPrefix);

	   // Now we want to export the clips. If we're in Denver
	   // mode, the clips are exported in the ExportAudio part
	   // (as a video).
       if (HasSgClips() && success && !m_bCancelExportRequested && !IsDenverDocument()) {
           if (pListener != NULL) {
               pListener->SetProgressLabel(IDS_EXPORT_CLIPS);
           } else if (m_pExportProgressDialog != NULL) {
               m_pExportProgressDialog->SetParticularLabel(IDS_EXPORT_CLIPS);
           }

           success = ExportClips(exportPath, newPrefix, pListener);
       }
	 
	   if (success && !m_bCancelExportRequested)
	   {
           if (pListener != NULL)
               pListener->SetProgress(1.0f);
           else if (!m_bSuppressSuccess) {
               CString msg;
               msg.LoadString(IDS_MSG_EXPORTSUCCEEDED);	      
               CEditorDoc::ShowInfoMessage(msg, hwndParent);
           }

		  // Bugfix 1941: Check for Videos larger than 2GB
		  // As of version 2.0 this is a check for 1GB

		  if (HasVideo())
		  {
			 CString csAviFileName;
			 csAviFileName.Format(_T("%s\\%s.avi"), exportPath, newPrefix);
	         
			 __int64 i64AviFileSize = LIo::GetFileSize(csAviFileName);
			 if (i64AviFileSize != -1i64)
			 {
				if (i64AviFileSize > 1073741824i64)
				{
				   double dFileSize = (double) i64AviFileSize;
				   double d1GB = 0.95*1073741824.0;
	               
				   double dCurrentLenMs = GetTotalLength();
				   double dMaxLenMs = (d1GB / dFileSize) * dCurrentLenMs;
	               
				   int nMaxLenMs = (int) dMaxLenMs;
	               
				   CString csMaxLen;
				   Misc::FormatTickString(csMaxLen, nMaxLenMs, SU_MINUTES, true);
	               
				   CString csMsg;
				   csMsg.Format(IDS_MSG_WARNING_1GB, csMaxLen);

                   if (pListener == NULL)
				       CEditorDoc::ShowWarningMessage(csMsg, hwndParent);
                   else
                       pListener->ShowMessage(PROGRESS_MSG_WARNING, csMsg, NULL);
				}
			 }
		  }
	      
	   }
       else if (m_bCancelExportRequested)
       {
           if (pListener == NULL) {
               if (CStudioApp::IsLiveContextMode())
                   CStudioApp::SetLiveContextExitCode(EXIT_CODE_LC_TASK_CANCEL);
               else {
                   CString msg;
                   msg.LoadString(IDS_MSG_OPERATIONCANCELLED);
                   CEditorDoc::ShowInfoMessage(msg, hwndParent);
                   success = false;
               }
           }
       }

	   if (success && !m_bCancelExportRequested)
		  success = CreateThumbnailsForStructuredSgClip(exportPath, newPrefix);
	   
   }

   // Export is a project change, the project file has to be saved.
   if (success) {
       CStudioApp *pApp = (CStudioApp *)AfxGetApp();
       // If the export is called from a context menu in record mode -> Save
       // else set modified flag.
       if (pApp != NULL && pApp->GetCurrentMode() == CStudioApp::MODE_ASSISTANT) {
           // Do not Save the project if LIVECONTEXT creates a demo document
           if (!pApp->IsLiveContextMode() || 
               (pApp->GetLiveContext().GetModeType() != LcCreateDemoDoc)) {
               Save(NULL, (TCHAR *)(LPCTSTR)m_exportFileName);
           }
       }
       else if (m_pDoc)
           m_pDoc->SetModifiedFlag();
   }

   m_bLastExportSuccess = success;

   PostMessage(hwndParent, WM_USER, CExportProgressDialog::END_DIALOG, NULL);
   
   CoUninitialize();

   if (m_hExportFinishEvent != NULL) {
       ::SetEvent(m_hExportFinishEvent);
       m_hExportFinishEvent = NULL;
   }

   m_bSuppressSuccess = false;
}

bool CEditorProject::CopyExportedDocument(LPCTSTR tszFilename, LPCTSTR szExportedFilename) {
    m_bCancelExportRequested = false;
    m_bLastExportSuccess = false;
    m_exportFileName = tszFilename;   
    if (_tcslen(szExportedFilename) > 0)
        m_csLastExportFileName = szExportedFilename;
    else {
        // In this case there is no lep file. Use project filename 
        // to get correct lrd file name
        m_csLastExportFileName = m_szFilename;
        StringManipulation::GetPathFilePrefix(m_csLastExportFileName);
        m_csLastExportFileName += _T(".lrd");
    }
    m_bSuppressSuccess = false;

    m_hDialogInitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_pExportProgressDialog = new CExportProgressDialog(this, AfxGetMainWnd());
    AfxBeginThread(CopyThreadLauncher, this, 0, 0, 0, NULL);

    m_pExportProgressDialog->DoModal();
    delete m_pExportProgressDialog;
    m_pExportProgressDialog = NULL;
    CloseHandle(m_hDialogInitEvent);

    return m_bLastExportSuccess;
}

UINT CEditorProject::CopyThreadLauncher(LPVOID pData) {
    HRESULT hr = CLanguageSupport::SetThreadLanguage();

    CEditorProject *pProject = (CEditorProject *) pData;
    pProject->CopyThread(NULL); // NULL: use old standard mechanism
    AfxEndThread(0, TRUE);
    return 0;
}

void CEditorProject::CopyThread(IProgressListener *pListener) {
    HWND hwndParent = NULL;

    // As we need the dialog, wait for it to initialize
    if (m_pExportProgressDialog != NULL) {
        DWORD res = ::WaitForSingleObject(m_hDialogInitEvent, INFINITE);
        hwndParent = m_pExportProgressDialog->GetSafeHwnd();
    }

    CLrdFile lrdFile;
    CString csExportedLrd;
    StringManipulation::TransformFromSgml(m_csLastExportFileName, csExportedLrd);

    lrdFile.Open(csExportedLrd);

    HRESULT hr = lrdFile.CopyDocumentTo(m_exportFileName, m_pExportProgressDialog);

    // there should be no message box in LiveContext mode
    if (!CStudioApp::IsLiveContextMode()) {
        if (SUCCEEDED(hr)) {
            CString msg;
            msg.LoadString(IDS_MSG_EXPORTSUCCEEDED);	      
            CEditorDoc::ShowInfoMessage(msg, hwndParent);
        } else if (hr == E_EXPORT_CANCELLED) {
            if (CStudioApp::IsLiveContextMode()) {
                CStudioApp::SetLiveContextExitCode(EXIT_CODE_LC_TASK_CANCEL);
                hr = S_OK;
            }
            else {
                CString msg;
                msg.LoadString(IDS_MSG_OPERATIONCANCELLED);
                CEditorDoc::ShowInfoMessage(msg, hwndParent);
            }
        } else {
            CString msg;
            if (hr == E_EXPORT_COPY_FAILED)
                msg.LoadString(IDS_EXPORT_ERROR_COPY);
            else if (hr == E_EXPORT_READ_WB_ERROR || 
                hr == E_EXPORT_WRITE_WB_ERROR || 
                hr == E_EXPORT_ACCESS_WB_ERROR ||
                hr == E_EXPORT_ACCESS_ERROR)
                msg.LoadString(IDS_EXPORT_ERROR_WB);
            CEditorDoc::ShowErrorMessage(msg, hwndParent);
        }
    }

    m_bLastExportSuccess = SUCCEEDED(hr);


    PostMessage(hwndParent, WM_USER, CExportProgressDialog::END_DIALOG, NULL);

    m_bSuppressSuccess = false;
}

void CEditorProject::GetWhiteboardElements(int iBeginTimestamp, int iEndTimestamp, CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> &aObjects) {
    CWhiteboardStream::m_objectCounter = 0;
    for (int i = 0; i < m_previewList.GetSize(); ++i)    {
        CPreviewSegment *part = m_previewList[i];
        if (part && (part->GetSourceBegin() >= iEndTimestamp))
            break;
        if (part && part->GetSourceEnd() >= iBeginTimestamp && part->GetWhiteboardStream()) {
            CWhiteboardStream *pWhiteboardStream = part->GetWhiteboardStream();
            CArray<CWhiteboardEvent * , CWhiteboardEvent *> aEvents;
            pWhiteboardStream->GetEventArray(aEvents);
            for (int j = 0; j < aEvents.GetCount(); ++j) {
                if (aEvents[j] != NULL && 
                    aEvents[j]->GetTimestamp() >= iBeginTimestamp && 
                    aEvents[j]->GetTimestamp() < iEndTimestamp) {

                    CPtrArray *pObjectPointer = aEvents[j]->GetObjectsPointer();
                    for (int k = 0; k < pObjectPointer->GetCount(); ++k) {
                        bool bFound = false;
                        for (int l = 0; l < aObjects.GetCount() && !bFound; ++l) {
                            if (aObjects[l] == pObjectPointer->GetAt(k)) {
                                bFound = true;
                            }
                        }
                        if (!bFound)
                            aObjects.Add((DrawSdk::DrawObject *)pObjectPointer->GetAt(k));
                    }
                }
            }
        }
    }
}

void CEditorProject::GetGraphicalObjects(int iBeginTimestamp, int iEndTimestamp, CArray<CGraphicalObject *, CGraphicalObject *> &aObjects) {
    HRESULT hr = S_OK;

    aObjects.RemoveAll();

    CInteractionStream *pInteractionStream = GetInteractionStream();
    if (pInteractionStream == NULL)
        hr = E_FAIL;

    CTimePeriod currentPeriod;
    if (SUCCEEDED(hr))
        hr = currentPeriod.Init(INTERACTION_PERIOD_TIME, iBeginTimestamp, iEndTimestamp-1, true);

    CArray<CInteractionAreaEx *, CInteractionAreaEx *> aInteractions;
    if (SUCCEEDED(hr))
        hr = pInteractionStream->FindInteractions(&currentPeriod, false, &aInteractions);

    if (SUCCEEDED(hr)) {
        for (int i = 0; i < aInteractions.GetCount(); ++i) {
            CInteractionAreaEx *pInteraction = aInteractions[i];
            if (pInteraction != NULL && pInteraction->GetTypeId() == INTERACTION_RECORD_GRAPHIC_OBJECTS) {
                aObjects.Add((CGraphicalObject *)pInteraction);
            }
        }
    }
 
    if (FAILED(hr)) {
        aObjects.RemoveAll();
    }
}

bool CEditorProject::ExportWhiteboard(CString &recordPath, CString &prefix, IProgressListener *pListener)
{
   HWND hwndParent = NULL;
   if (m_pExportProgressDialog != NULL)
       hwndParent = m_pExportProgressDialog->GetSafeHwnd();

   // reset object counter and objects
   CWhiteboardStream::m_objectCounter = 0;
   for (int i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *part = m_previewList[i];
      if (part && part->GetWhiteboardStream())
         part->GetWhiteboardStream()->ResetObjects();
   }
   
   CString evqFilename;
   evqFilename.Format(_T("%s\\%s.evq"), recordPath, prefix);
   
   CString objFilename;
   objFilename.Format(_T("%s\\%s.obj"), recordPath, prefix);

   CString csExportPrefix = recordPath;
   csExportPrefix += _T("\\");
   csExportPrefix += prefix;
   
   HANDLE hObjFile = CreateFile(objFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hObjFile == INVALID_HANDLE_VALUE)
   {
      CString msg;
      msg.Format(IDS_MSG_OPEN_OBJ_FAILED, objFilename);
      if (pListener == NULL)
          CEditorDoc::ShowErrorMessage(msg, hwndParent);
      else
          pListener->ShowMessage(PROGRESS_MSG_ERROR, msg, NULL);
      return false;
   }
   HANDLE hEvqFile = CreateFile(evqFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hEvqFile == INVALID_HANDLE_VALUE)
   {
      CString msg;
      msg.Format(IDS_MSG_OPEN_EVQ_FAILED, evqFilename);
      if (pListener == NULL)
          CEditorDoc::ShowErrorMessage(msg, hwndParent);
      else
          pListener->ShowMessage(PROGRESS_MSG_ERROR, msg, NULL);
      CloseHandle(hObjFile);
      return false;
   }

   HRESULT hr = S_OK;

   CFileOutput outObjectQueue;
   if (SUCCEEDED(hr))
      hr = outObjectQueue.Init(hObjFile, 0, true);

   CFileOutput outEventQueue;
   if (SUCCEEDED(hr))
      hr = outEventQueue.Init(hEvqFile, 0, false);
 

   if (SUCCEEDED(hr))
      hr = outObjectQueue.Append(_T("<WB orient=bottomleft>\n"));
   
   //
   // write all regular whiteboard data (old objects and events)
   //
   if (SUCCEEDED(hr))
   {
      for (i = 0; i < m_previewList.GetSize(); ++i)
      {
         CPreviewSegment *part = m_previewList[i];
         if (part)
         {
            part->SaveWhiteboard(&outEventQueue, &outObjectQueue, recordPath, prefix, part->GetPageStream());
         }
      }
   }

   //
   // find out all the mouse actions which copy a file: 
   // for duplicate file names change the target file names
   //
   CArray<CMouseAction *, CMouseAction *> aCopyMouseActions;
   aCopyMouseActions.SetSize(0, 10);

   if (m_pInteractionStream != NULL)
      m_pInteractionStream->FillCopyMouseActions(&aCopyMouseActions);
   
   if (m_pQuestionStream != NULL)
      m_pQuestionStream->FillCopyMouseActions(&aCopyMouseActions);

   if (aCopyMouseActions.GetSize() > 0)
   {
      // make target file names unique
      CMap<CString, LPCTSTR, UINT, UINT> mapActionCopyFiles;

      for (i=0; i<aCopyMouseActions.GetSize(); ++i)
      {
         LPCTSTR tszPath = aCopyMouseActions[i]->GetPath();

         bool bIsFile = tszPath != NULL && _taccess(tszPath, 04) == 0;

         if (bIsFile)
         {
            CString csFileName = tszPath;
            StringManipulation::GetFilename(csFileName);
            UINT nUsageCount = 0;
            bool bFound = mapActionCopyFiles.Lookup(csFileName, nUsageCount) == TRUE;

            ++nUsageCount;

            if (bFound)
               aCopyMouseActions[i]->ChangeOutputFileName(nUsageCount);

            mapActionCopyFiles.SetAt(csFileName, nUsageCount);
         }
      }
   }
   

   if (SUCCEEDED(hr) && m_pMarkStream != NULL)
      hr = m_pMarkStream->WriteObjEntry(&outObjectQueue, false);

   if (SUCCEEDED(hr) && m_pInteractionStream != NULL)
      hr = m_pInteractionStream->WriteObjEntry(&outObjectQueue, false, false, csExportPrefix);

   if (SUCCEEDED(hr) && m_pQuestionStream != NULL)
      hr = m_pQuestionStream->WriteFileEntry(&outObjectQueue, false, false, csExportPrefix, NULL);

   
   if (SUCCEEDED(hr))
      hr = outObjectQueue.Append(_T("</WB>\n"));
   
   if (SUCCEEDED(hr))
      hr = outEventQueue.Flush();
   CloseHandle(hEvqFile);

   if (SUCCEEDED(hr))
      hr = outObjectQueue.Flush();
   CloseHandle(hObjFile);
   
   return SUCCEEDED(hr);
}

bool CEditorProject::ExportMetadata(CString &recordPath, CString &prefix, 
                                    CArray<UINT, UINT> *paClipTimes, IProgressListener *pListener)
{
   HWND hwndParent = NULL;
   if (m_pExportProgressDialog != NULL)
       hwndParent = m_pExportProgressDialog->GetSafeHwnd();
   
   bool useFlatStructure = false;
   
   CString lmdFilename;
   lmdFilename.Format(_T("%s\\%s.lmd"), recordPath, prefix);
   
   HANDLE hLmdFile = ::CreateFile(lmdFilename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hLmdFile == INVALID_HANDLE_VALUE)
   {
      CString msg;
      msg.Format(IDS_MSG_OPEN_LMD_FAILED, lmdFilename);

      if (pListener == NULL)
          CEditorDoc::ShowErrorMessage(msg, hwndParent);
      else
          pListener->ShowMessage(PROGRESS_MSG_ERROR, msg, NULL);

      return false;
   }

   CFileOutput output;
   output.Init(hLmdFile, 0, true);
   
   CString csSgmlString;
   CString line;
   
   line = _T("<docinfo>\n");
   output.Append(line);
   
   line = _T("  <metainfo>\n");
   output.Append(line);

   output.Flush();
   
   StringManipulation::TransformForSgml(m_targetMetainfo.GetTitle(), csSgmlString);
   line.Format(_T("    <title>%s</title>\n"), csSgmlString);
   output.Append(line);
   StringManipulation::TransformForSgml(m_targetMetainfo.GetAuthor(), csSgmlString);
   line.Format(_T("    <author>%s</author>\n"), csSgmlString);
   output.Append(line);
   StringManipulation::TransformForSgml(m_targetMetainfo.GetCreator(), csSgmlString);
   line.Format(_T("    <creator>%s</creator>\n"), csSgmlString);
   output.Append(line);
   StringManipulation::TransformForSgml(m_targetMetainfo.GetRecordDate(), csSgmlString);
   line.Format(_T("    <recorddate>%s</recorddate>\n"), csSgmlString);
   output.Append(line);
   CStringList stringList;
   m_targetMetainfo.GetKeywords(stringList);
   if (!stringList.IsEmpty())
   {
      POSITION position = stringList.GetHeadPosition();
      while(position)
      {
         CString string = stringList.GetNext(position);
         StringManipulation::TransformForSgml(string, csSgmlString);
         line.Format(_T("    <keyword>%s</keyword>\n"), csSgmlString);
         output.Append(line);
      }
   }
   
   line = _T("  </metainfo>\n");
   output.Append(line);
   
   output.Flush();

   line = _T("    <structure>\n");
   output.Append(line);

   if (!IsDenverDocument())
   {
       if (!m_bShowClickPages && m_pTargetStructureWithClickPages != NULL) {
           // the page numbers should be updated before export
           int number = 0;
           CMap<int, int, int, int> mapUniquePageIds;
           mapUniquePageIds.InitHashTable(101);
           m_pTargetStructureWithClickPages->UpdatePageNumbers(number, mapUniquePageIds);
           // The non click pages with default title should be corrected (continuous numbering)
           int iFirstNumber = 1;
           m_pTargetStructureWithClickPages->UpdateNonClickPageTitles(iFirstNumber);
           // write
           m_pTargetStructureWithClickPages->Write(&output, prefix, paClipTimes, 2);
       }
       else {
           m_targetStructure.Write(&output, prefix, paClipTimes, 2);
       }
   }
   else 
   {
       UINT nClipCount = 0;
       if (GetClipStream() != NULL)
           nClipCount = GetClipStream()->GetExportClipFileCount();

       if (nClipCount > 0)
           m_targetStructure.WriteDenverEntry(&output, prefix, 2);
       // else project empty
   }

   line = _T("    </structure>\n");
   output.Append(line);
   
   line = _T("  <fileinfo>\n");
   output.Append(line);

   if (IsDenverDocument() || HasStructuredSgClips()) {
       if (ShowClickPages())
           line = _T("    <showClickPages>true</showClickPages>\n");
       else
           line = _T("    <showClickPages>false</showClickPages>\n");
       output.Append(line);
   }

   line = _T("    <audio>\n");
   output.Append(line);
   int sampleRate = 0;
   int bitRate = 0;
   // Now let's find out the sample and bit rates...
   if (m_mtAudioFormat.formattype == FORMAT_WaveFormatEx)
   {
      WAVEFORMATEX *pWfe = (WAVEFORMATEX *) m_mtAudioFormat.pbFormat;
      if (pWfe)
      {
         sampleRate = pWfe->nSamplesPerSec;
         bitRate    = pWfe->wBitsPerSample;
      }
   }
   line.Format(_T("      <samplerate>%d</samplerate>\n"), sampleRate);
   output.Append(line);
   line.Format(_T("      <bitrate>%d</bitrate>\n"), bitRate);
   output.Append(line);
   line = _T("    </audio>\n");
   output.Append(line);
   
   if (HasVideo())
   {
      line = _T("    <video>\n");
      output.Append(line);
      
      // Note: This is wrong!
      line.Format(_T("      <framerate>%f</framerate>\n"), m_videoAviSettings.keyframeRate);
      output.Append(line);
      
      _TCHAR tszFcc[5];
      if (m_videoAviSettings.dwFCC == BI_RGB)
      {
         _tcsncpy(tszFcc, _T("DIB "), 4);
         tszFcc[4] = 0;
      }
      else
      {
         tszFcc[0] = (_TCHAR) (m_videoAviSettings.dwFCC & 0x000000ff);
         tszFcc[1] = (_TCHAR) ((m_videoAviSettings.dwFCC & 0x0000ff00) >> 8);
         tszFcc[2] = (_TCHAR) ((m_videoAviSettings.dwFCC & 0x00ff0000) >> 16);
         tszFcc[3] = (_TCHAR) ((m_videoAviSettings.dwFCC & 0xff000000) >> 24);
         tszFcc[4] = 0;
      }
      
      line.Format(_T("      <codec>%s</codec>\n"), tszFcc);
      output.Append(line);
      line.Format(_T("      <quality>%d</quality>\n"), m_videoAviSettings.quality);
      output.Append(line);
      line.Format(_T("      <keyframes>%d</keyframes>\n"), m_videoAviSettings.keyframeRate);
      output.Append(line);
      
      line = _T("    </video>\n");
      output.Append(line);
   }
   
   line = _T("  </fileinfo>\n");
   output.Append(line);
   
   line = _T("</docinfo>\n");
   output.Append(line);
   
   output.Flush();
   CloseHandle(hLmdFile);
   
   return true;
}

bool CEditorProject::CreateThumbnailsForStructuredSgClip(CString &recordPath, CString &prefix) {
                                       
    if (HasStructuredSgClips()) {
       CList<int> lstPageTimestamps;
       CMergedClip *pClipToExport = m_pClipStream->GetClipToExport();   
       if (pClipToExport == NULL)
           return false;

       if (!m_bShowClickPages && m_pTargetStructureWithClickPages != NULL) 
           m_pTargetStructureWithClickPages->GetAllPageTimestamps(lstPageTimestamps);
       else
           m_targetStructure.GetAllPageTimestamps(lstPageTimestamps);

       // Create thumbs
       // Double code from ScreengrabbingExtended::CreateScreenshots
       if (lstPageTimestamps.GetCount() > 0) {

           POSITION position = lstPageTimestamps.GetHeadPosition();
           int iOffset = pClipToExport->GetTargetBegin();
           while (position != NULL) {
               int iTimestamp = lstPageTimestamps.GetAt(position);
               lstPageTimestamps.SetAt(position, (iTimestamp-iOffset));
               lstPageTimestamps.GetNext(position);
           }

           CString csImagePrefix;
           csImagePrefix.Format(_T("%s\\%s_"), recordPath, prefix);
           CString csClipFilename;
           if (!IsDenverDocument())
               csClipFilename.Format(_T("%s\\%s_clip_%d.avi"), recordPath, prefix, pClipToExport->GetTargetBegin());
           else
               csClipFilename.Format(_T("%s\\%s.avi"), recordPath, prefix); 
           HRESULT hr = WritePosterFrames(csClipFilename, csImagePrefix, &lstPageTimestamps, iOffset);

           // get encoder fpr PNG images
           CLSID pngClsid;
           VideoInformation::GetEncoderClsid(L"image/png", &pngClsid);

           position = lstPageTimestamps.GetHeadPosition();
           while (SUCCEEDED(hr) && position != NULL) {
               int streamTime = lstPageTimestamps.GetNext(position);

               // set image file name
               CString csOrigImageFilename;
               csOrigImageFilename.Format(_T("%ls\\%ls_%i.png"), recordPath, prefix, streamTime+iOffset);

               //////////////////////////////////////////////////////////////
               // create thumbnails in three sizes: max 100, 150 & 200 pixel
               //////////////////////////////////////////////////////////////

               // get current screenshot
               Gdiplus::Image *pImage = Gdiplus::Image::FromFile(csOrigImageFilename);

               for ( int width = 100; width <=200; width += 50 ) {
                   // set thumbnail size
                   int height = width;
                   int iNewImageWidth;
                   int iNewImageHeight;
                   // set thumbnail image file name
                   CString csImageFilename;
                   csImageFilename.Format(_T("%ls\\%i_%ls_%i_thumb.png"), recordPath, width, prefix, streamTime+iOffset);
                   // get scaled thumbnail image
                   Calculator::FitRectInRect(width, height, pImage->GetWidth(), pImage->GetHeight(), &iNewImageWidth, &iNewImageHeight);
                   Gdiplus::Image *pThumbnail = pImage->GetThumbnailImage(iNewImageWidth, iNewImageHeight);
                   // draw border
                   Gdiplus::Graphics *newGraphics = Gdiplus::Graphics::FromImage(pThumbnail);
                   Gdiplus::Pen pen(Gdiplus::Color::Black);
                   newGraphics->DrawRectangle(&pen, 0, 0, pThumbnail->GetWidth()-1, pThumbnail->GetHeight()-1);
                   // write thumbnail image
                   pThumbnail->Save(csImageFilename, &pngClsid, NULL);
                   delete newGraphics;
                   delete pThumbnail;
               }

               delete pImage;
               DeleteFile(csOrigImageFilename);
           }
       }
   }
   
   return true;
}

bool CEditorProject::ExportLrd(CString &recordPath, CString &prefix, IProgressListener *pListener)
{
   HWND hwndParent = NULL;
   if (m_pExportProgressDialog != NULL)
       hwndParent = m_pExportProgressDialog->GetSafeHwnd();
   
   CString lrdFilename;
   lrdFilename.Format(_T("%s\\%s.lrd"), recordPath, prefix);
   
   HANDLE hLrdFile = CreateFile(lrdFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hLrdFile == INVALID_HANDLE_VALUE)
   {
      CString msg;
      msg.Format(IDS_MSG_OPEN_LRD_FAILED, lrdFilename);

      if (pListener == NULL)
          CEditorDoc::ShowErrorMessage(msg, hwndParent);
      else
          pListener->ShowMessage(PROGRESS_MSG_ERROR, msg, NULL);

      return false;
   }

   CFileOutput out;
   out.Init(hLrdFile, 0, true);
   
   CString csSgmlPrefix;
   StringManipulation::TransformForSgml(prefix, csSgmlPrefix);
   
   CString csSgmlString;
   CString line;
   
   line = _T("<DOCUMENT>\n");
   out.Append(line);
   
   // Metadata
   line = _T("  <INFO>\n");
   out.Append(line);
   
   // everything written since 2.0.p2 is utf-8
   out.Append(_T("    <CODEPAGE>utf-8</CODEPAGE>\n"));
  
   StringManipulation::TransformForSgml(m_targetMetainfo.GetTitle(), csSgmlString);
   line.Format(_T("    <TITLE>%s</TITLE>\n"), csSgmlString);
   out.Append(line);
   StringManipulation::TransformForSgml(m_targetMetainfo.GetAuthor(), csSgmlString);
   line.Format(_T("    <AUTHOR>%s</AUTHOR>\n"), csSgmlString);
   out.Append(line);
   StringManipulation::TransformForSgml(m_targetMetainfo.GetComments(), csSgmlString);
   line.Format(_T("    <COMMENTS>%s</COMMENTS>\n"), csSgmlString);
   out.Append(line);
   // Bugfix of Bug 1338:
   // line.Format("    <LENGTH>%s</LENGTH>\n", m_targetMetainfo.GetLength());
   {
      int ms = GetTotalLength();
      int mins = ms / (60*1000);
      int secs = (ms - (mins * 60 * 1000)) / 1000;
      line.Format(_T("    <LENGTH>%d:%02d</LENGTH>\n"), mins, secs);
   }
   out.Append(line);
   line.Format(_T("    <RECORDDATE>%s - %s</RECORDDATE>\n"), m_targetMetainfo.GetRecordDate(), m_targetMetainfo.GetRecordTime());
   out.Append(line);
   if (IsDenverDocument()) {
       line.Format(_T("    <TYPE>denver</TYPE>\n"));
       out.Append(line);
   }

   if (IsDenverDocument() || HasStructuredSgClips()) {
       if (ShowScreenGrabbingAsRecording())
           line.Format(_T("    <VIEWTYPE>recording</VIEWTYPE>\n"));
       else
           line.Format(_T("    <VIEWTYPE>simulation</VIEWTYPE>\n"));
       out.Append(line);
   }

   
   line = _T("  </INFO>\n");
   out.Append(line);
   
   // Files and Helper
   
   line = _T("  <FILES>\n");
   out.Append(line);
   
   
   line.Format(_T("    <AUDIO>%s.lad</AUDIO>\n"), csSgmlPrefix);
   out.Append(line);
   
   line = _T("    <HELPERS>\n");
   out.Append(line);
   
   if (!IsDenverDocument())
   {
      line = _T("      <H>\n");
      out.Append(line);
      line.Format(_T("        <OPT>wbPlay</OPT>\n"));
      out.Append(line);
      line.Format(_T("        <OPT>-G</OPT>\n"));
      out.Append(line);
      line.Format(_T("        <OPT>%dx%d</OPT>\n"), m_targetPageWidth, m_targetPageHeight);
      out.Append(line);
      line.Format(_T("        <OPT>%s.evq</OPT>\n"), csSgmlPrefix);
      out.Append(line);
      line.Format(_T("        <OPT>%s.obj</OPT>\n"), csSgmlPrefix);
      out.Append(line);
      line = _T("      </H>\n");
      out.Append(line);
   }

   UINT nClipCount = 0;
   if (GetClipStream() != NULL)
      nClipCount = GetClipStream()->GetExportClipFileCount();
   
   if (HasVideo() || nClipCount > 0 || !m_csStillImage.IsEmpty())
   {
      line = _T("      <H>\n");
      out.Append(line);
      line.Format(_T("        <OPT>java</OPT>\n"));
      out.Append(line);
      line.Format(_T("        <OPT>imc.epresenter.player.JMFVideoPlayer</OPT>\n"));
      out.Append(line);
      if (HasVideo())
      {
         line.Format(_T("        <OPT>-video</OPT>\n"));
         out.Append(line);
         line.Format(_T("        <OPT>%s.avi</OPT>\n"), csSgmlPrefix);
         out.Append(line);
      }
      else if (!m_csStillImage.IsEmpty())
      {
         //CString csImageName = m_csStillImage;
         //StringManipulation::GetFilename(csImageName);
         // Create a still image name for this export
         CString csSuffix = m_csStillImage;
         StringManipulation::GetFileSuffix(csSuffix);
         CString csImageName;
         csImageName.Format(_T("%s_still.%s"), prefix, csSuffix);
         
         line.Format(_T("        <OPT>-still</OPT>\n"));
         out.Append(line);
         StringManipulation::TransformForSgml(csImageName, csSgmlString);
         line.Format(_T("        <OPT>%s</OPT>\n"), csSgmlString);
         out.Append(line);
         CString csImagePath;
         csImagePath.Format(_T("%s\\%s"), recordPath, csImageName);
         if (CopyFile(m_csStillImage, csImagePath, false) == 0)
         {
            CString msg;
            msg.Format(IDS_MSG_COPYFAIL, m_csStillImage, csImagePath);

            if (pListener == NULL)
                CEditorDoc::ShowWarningMessage(msg, hwndParent);
            else
                pListener->ShowMessage(PROGRESS_MSG_WARNING, msg, NULL);
         }
      }
      if (GetClipStream() != NULL)
      {
         bool bFirstClipOnly = IsDenverDocument();

         HRESULT hr = GetClipStream()->WriteLrdEntry(&out, csSgmlPrefix, bFirstClipOnly);
         _ASSERT(SUCCEEDED(hr));
      }

      line = _T("      </H>\n");
      out.Append(line);
   }
   
   line = _T("    </HELPERS>\n");
   out.Append(line);
   
   
   line = _T("    <OFFSETS>\n");
   out.Append(line);
   
   line = _T("      <O>0</O>\n");
   out.Append(line);
   
   line = _T("    </OFFSETS>\n");
   out.Append(line);
   
   
   line = _T("    <SLIDES></SLIDES>\n");
   out.Append(line);
   
   line = _T("    <APPS></APPS>\n");
   out.Append(line);
   
   
   line.Format(_T("    <METADATA>%s.lmd</METADATA>\n"), csSgmlPrefix);
   out.Append(line);
   
   line = _T("  </FILES>\n");
   out.Append(line);

   // PZI: write clip structuring status
   if (GetClipStream() != NULL) {
       GetClipStream()->WriteClipStructure(&out);
   }

   
   // configuration infos
   line = _T("  <CONFIG>\n");
   out.Append(line);
   
   line = _T("    <SLIDES></SLIDES>\n");
   out.Append(line);
   
   line = _T("    <APPS></APPS>\n");
   out.Append(line);
   
   line = _T("  </CONFIG>\n");
   out.Append(line);
   
   
   line = _T("</DOCUMENT>\n");
   out.Append(line);
   
   out.Flush();
   CloseHandle(hLrdFile);
   
   return true;
}

bool CEditorProject::ExportAudio(CString &recordPath, CString &prefix, IProgressListener *pListener, HWND hWndNotify)
{
   HWND hwndParent = NULL;
   if (m_pExportProgressDialog != NULL)
       hwndParent = m_pExportProgressDialog->GetSafeHwnd();

   //HRESULT hr = InitAudioVideo(false, prefix); // false == for export, true == for preview
   HRESULT hr = PrepareRenderGraph(false, prefix);

   if (SUCCEEDED(hr) && HasVideo() && m_videoStreamLength <= 0)
   {
	   CString csMsg;
	   csMsg.LoadString(IDS_MSG_AVEXPORT_FAILED);

       if (pListener == NULL)
	       CEditorDoc::ShowWarningMessage(csMsg, hwndParent);
       else
           pListener->ShowMessage(PROGRESS_MSG_WARNING, csMsg, NULL);

       hr = E_FAIL;
   }
   
   if (SUCCEEDED(hr))
   {
      hr = m_avEdit.InitializeExport(recordPath, hWndNotify);
   }

   if (SUCCEEDED(hr))
   {
      hr = m_avEdit.StartExport(NULL);
   }
   
   if (FAILED(hr) && !m_bCancelExportRequested)
   {
      bool bErrorMessageDisplayed = false;
      if (hr == E_FAIL && 
         m_videoAviSettings.bUseSmartRecompression &&
         (m_videoAviSettings.dwFCC == '05vi' ||
         m_videoAviSettings.dwFCC == '05VI'))
      {
         // Check for special Indeo Video 5 case
         if (m_videoAviSettings.videoFormat.formattype == MEDIATYPE_Video)
         {
            BITMAPINFOHEADER *pBi = HEADER(m_videoAviSettings.videoFormat.pbFormat);
            if (pBi->biWidth > 640 ||
               pBi->biHeight > 480)
            {
               CString msg;
               msg.LoadString(IDS_MSG_AVEXPORT_IV50);
               if (pListener == NULL)
                   CEditorDoc::ShowErrorMessage(msg, hwndParent);
               else
                   pListener->ShowMessage(PROGRESS_MSG_ERROR, msg, NULL);
               bErrorMessageDisplayed = true;
            }
         }
      }

      if (!bErrorMessageDisplayed && hr == E_OUTOFMEMORY)
      {
         // E_OUTOFMEMORY --> disk full
         CString csMsg;
         csMsg.LoadString(IDS_MSG_DISKFULL);
         if (pListener == NULL)
             CEditorDoc::ShowErrorMessage(csMsg, hwndParent);
         else
             pListener->ShowMessage(PROGRESS_MSG_ERROR, csMsg, NULL);
         bErrorMessageDisplayed = true;
      }

      if (!bErrorMessageDisplayed && hr == RPC_E_TIMEOUT)
      {
         // Maybe corrupt video input data?
         CString csMsg;
         CString csMs;
         Misc::FormatTickString(csMs, m_avEdit.GetNoProgressAbortTimeMs(), SU_MINUTES, true);
         csMsg.Format(IDS_MSG_NOPROGRESS, csMs);
         if (pListener == NULL)
             CEditorDoc::ShowErrorMessage(csMsg, hwndParent);
         else
             pListener->ShowMessage(PROGRESS_MSG_ERROR, csMsg, NULL);
         bErrorMessageDisplayed = true;

         // Set the current position to the error
         // position. Set a suitable selection.
         m_pDoc->m_curPosMs = m_avEdit.GetNoProgressAbortTimeMs();
         m_pDoc->m_markStartMs = m_pDoc->m_curPosMs - 200;
         m_pDoc->m_markEndMs = m_pDoc->m_curPosMs + 800;
      }
      
      // hr can be E_ABORT if the audio/video initialization
      // was cancelled due to a potential 2 GB video size problem.
      if (!bErrorMessageDisplayed && hr != E_ABORT)
      {
         CString msg;
         msg.Format(IDS_MSG_AVEXPORT_FAILED, hr);
         if (pListener == NULL)
             CEditorDoc::ShowErrorMessage(msg, hwndParent);
         else
             pListener->ShowMessage(PROGRESS_MSG_ERROR, msg, NULL);
      }
   }
   
   return SUCCEEDED(hr) ? true : false;
}

void CEditorProject::ExportAudioAsWav(LPCTSTR tszRecordPath, LPCTSTR tszFileName)
{
   m_exportAudioAsWavFileName = tszFileName;
   m_exportAudioAsWavPath     = tszRecordPath;
   m_bCancelExportRequested   = false;
   
   m_hDialogInitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   m_pExportProgressDialog = new CExportProgressDialog(this, AfxGetMainWnd());
   
   AfxBeginThread(ExportAudioAsWavThreadLauncher, this, 0, 0, 0, NULL);
   m_pExportProgressDialog->DoModal();
   delete m_pExportProgressDialog;
   m_pExportProgressDialog = NULL;
   CloseHandle(m_hDialogInitEvent);
   m_hDialogInitEvent = NULL;
}

void CEditorProject::CancelExport()
{
   m_bCancelExportRequested = true;
   m_avEdit.AbortExport();
}

bool CEditorProject::InitAudioStream(CAudioStream *pAudioStream, LPCTSTR tszFileName, 
                                     int nStreamSelector, UINT nErrorID, CWnd *hWnd,
                                     HWND hWndProgress, IProgressListener *pListener)
{
    if (pListener == NULL)
        pListener = this;

   bool success = pAudioStream->SetFilename(tszFileName, nStreamSelector, m_nVersionType, hWndProgress);
   if (!success)
   {
      if (pAudioStream->IsVersionError())
      {
         CString csMyType;
         CString csDocType;
         if (m_nVersionType == FULL_VERSION)
            csMyType.LoadString(IDS_ENTERPRISE_VERSION);
         else if (m_nVersionType == UNIVERSITY_VERSION)
            csMyType.LoadString(IDS_CAMPUS_VERSION);
         else
            csMyType.LoadString(IDS_EVALUATION_VERSION);
         int docVersionType = pAudioStream->GetVersionType();
         if (docVersionType == FULL_VERSION)
            csDocType.LoadString(IDS_ENTERPRISE_VERSION);
         else if (docVersionType == UNIVERSITY_VERSION)
            csDocType.LoadString(IDS_CAMPUS_VERSION);
         else
            csDocType.LoadString(IDS_EVALUATION_VERSION);
         
         CString msg;
         msg.Format(IDS_MSG_VERSION_CONFLICT, csDocType, csMyType);

         if (pListener != NULL)
             pListener->ShowMessage(PROGRESS_MSG_ERROR, msg, hWnd);
      }
      else
      {
         // General audio error  
         if (pListener != NULL)
             pListener->ShowMessage(PROGRESS_MSG_ERROR, nErrorID, hWnd);
      }
   }
   
   return success;
}

bool CEditorProject::ImportAudio(LPCTSTR tszFileName)
{
   if (m_previewList.GetSize() == 0)
      return false;
   
   m_hDialogInitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   m_pProgress = new CProgressDialogE(CMainFrameE::GetCurrentInstance(), this);
   m_csImportAudioFileName = tszFileName;
   
   AfxBeginThread(ImportAudioThreadLauncher, this, 0, 0, 0, NULL);
   m_pProgress->DoModal();
   delete m_pProgress;
   m_pProgress = NULL;
   CloseHandle(m_hDialogInitEvent);
   m_hDialogInitEvent = NULL;

   // TODO if the length of the project increases due to an audio import
   // the new streams do not notice that.
   // Maybe there are other external influences on project length?

   
   return true;
}

UINT CEditorProject::ImportAudioThreadLauncher(LPVOID lParam)
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   CEditorProject *pProject = (CEditorProject *) lParam;
   pProject->ImportAudioThread();
   if (pProject->m_pProgress)
      PostMessage(pProject->m_pProgress->GetSafeHwnd(), WM_USER, CProgressDialogE::END_DIALOG, NULL);
   return 0;
}

void CEditorProject::ImportAudioThread()
{
   // As we need the dialog, wait for it to initialize
   DWORD res = ::WaitForSingleObject(m_hDialogInitEvent, INFINITE);
   
   CString csFileName = m_csImportAudioFileName;
   
   m_pProgress->SetLabel(IDS_MSG_IMPORTAUDIO);
   m_pProgress->SetRange(0, 100);
   
   bool bCorrectFormat = false;
   CString csOriginalAudioFile(_T(""));
   int nOriginalAudioStream = -1;
   
   {
      // Check for Audio format
      AVInfo avInfo;
      AM_MEDIA_TYPE mtAudio;
      ZeroMemory(&mtAudio, sizeof AM_MEDIA_TYPE);
      HRESULT hr = avInfo.Initialize(csFileName);
      if (SUCCEEDED(hr))
         hr = avInfo.GetStreamFormat(0, &mtAudio);
      
      if (SUCCEEDED(hr) && !AVInfo::StaticIsPcmAudio(&mtAudio))
      {
         CString csAudioFileName;
         int nAudioStream = 0;
         bool cool = CheckAndRestoreAudioFile(csAudioFileName, nAudioStream, csFileName, 0);
         
         if (cool)
         {
            csOriginalAudioFile = csFileName;
            nOriginalAudioStream = 0;

            bCorrectFormat = true;
            // Replace input audio with newly written audio
            csFileName = csAudioFileName;
         }
      }
      else if (SUCCEEDED(hr))
      {
         bCorrectFormat = true;
      }
      
      if (SUCCEEDED(hr))
      {
         MYFREEMEDIATYPE(mtAudio);
      }
   }
   
   if (!bCorrectFormat)
   {
      CString csMsg;
      csMsg.LoadString(IDS_MSG_IMPORTAUDIO_FAILED);
      CEditorDoc::ShowErrorMessage(csMsg, m_pProgress->GetSafeHwnd());
      return;
   }
   
   CUnRedoElement *undo = new CUnRedoElement();
   undo->SetType(CUnRedoElement::IMPORT_AUDIO);
   undo->FillStreamListWith(m_streamList);
   // copy preview list to undo element
   undo->Append(m_previewList, 0, m_previewList.GetSize(), true);
   // remove elements from redo list
   EmptyRedoList();
   m_undoList.AddTail(undo);

   
   CAudioStream *pAudioStream = new CAudioStream();
   pAudioStream->SetID(++CStreamContainer::m_iAudioStreamCount);
   pAudioStream->SetOriginalAudioSource(csOriginalAudioFile);
   pAudioStream->SetOriginalAudioSourceStream(nOriginalAudioStream);
   
   if (!InitAudioStream(pAudioStream, csFileName, 0, IDS_MSG_IMPORTAUDIO_FAILED, 
      m_pProgress, m_pProgress->GetSafeHwnd()))
   {
      delete pAudioStream;
      return;// false;
   }
   
   POSITION position = m_streamList.GetHeadPosition();
   while (position)
   {
      CStreamContainer *pContainer = m_streamList.GetNext(position);
      if (pContainer)
      {
         pContainer->SetAudioStream(pAudioStream);
      }
   }
   
   for (int i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *pSegment = m_previewList[i];
      if (pSegment)
      {
         int audioOffset = pSegment->GetTargetBegin() - pSegment->GetSourceBegin();
         pSegment->SetAudioOffset(audioOffset);
      }
   }
   
   if (GetTotalLength() < pAudioStream->GetLength())
   {
      // if the newly imported audio is longer than all the existing streams
      // add a PreviewSegment with just an audio stream that represents
      // this additional portion

      CStreamContainer *pStream = new CStreamContainer();
      pStream->SetID(++CStreamContainer::m_nContainerCounter);
      pStream->SetAudioStream(pAudioStream);
      m_streamList.AddTail(pStream);
      CPreviewSegment *pSegment = new CPreviewSegment();
      pSegment->SetStreams(pStream);
      int segmentStart = GetTotalLength();
      int segmentStop = pAudioStream->GetLength();

      UINT nAdditionalMs = segmentStop - segmentStart;

      pSegment->SetSourceBegin(segmentStart);
      pSegment->SetSourceEnd(segmentStop);
      pSegment->SetTargetBegin(segmentStart);
      pSegment->SetTargetEnd(segmentStop);
      m_previewList.Add(pSegment);

      for (int i=0; i<m_aStreams.GetSize(); ++i)
         m_aStreams[i]->EnlargeStreamLength(nAdditionalMs);

      // only to make last page longer
      UpdateStructureAndStreams();
   }  


   HRESULT hr = m_UndoManager.RegisterAction(this, UNDO_LEGACY_CHANGE, NULL, 0, HINT_CHANGE_STRUCTURE);
   
   if (SUCCEEDED(hr)) 
      hr = m_UndoManager.SetRollbackPoint();

   // return true;
}

bool CEditorProject::ImportVideo(LPCTSTR tszFileName)
{
   bool success = true;
   
   if (m_previewList.GetSize() == 0)
      return false;
   
   m_hDialogInitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   m_pProgress = new CProgressDialogE(CMainFrameE::GetCurrentInstance(), this);
   
   m_csImportVideoFileName = tszFileName;
   m_bFunctionSucceeded = false;
   AfxBeginThread(ImportVideoThreadLauncher, this, 0, 0, 0, NULL);
   
   m_pProgress->DoModal();
   delete m_pProgress;
   m_pProgress = NULL;
   CloseHandle(m_hDialogInitEvent);
   m_hDialogInitEvent = NULL;
   
   return m_bFunctionSucceeded;
}

UINT CEditorProject::ImportVideoThreadLauncher(LPVOID lParam)
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   CEditorProject *pProject = (CEditorProject *) lParam;
   pProject->ImportVideoThread();
   if (pProject->m_pProgress)
      PostMessage(pProject->m_pProgress->GetSafeHwnd(), WM_USER, CProgressDialogE::END_DIALOG, NULL);
   return 0;
}

void CEditorProject::ImportVideoThread()
{
   // As we need the dialog, wait for it to initialize
   DWORD res = ::WaitForSingleObject(m_hDialogInitEvent, INFINITE);
   
   CString csFileName = m_csImportVideoFileName;
   
   m_pProgress->SetLabel(IDS_MSG_IMPORTVIDEO);
   m_pProgress->SetRange(0, 100);
   
   AVInfo avInfo;
   HRESULT hr = avInfo.Initialize(csFileName);
   long nStreams = 0;
   if (SUCCEEDED(hr))
      hr = avInfo.GetStreamCount(&nStreams);
   int nAudio = -1;
   int nVideo = -1;
   for (int i=0; i<nStreams && SUCCEEDED(hr); ++i)
   {
      AM_MEDIA_TYPE amStream;
      ZeroMemory(&amStream, sizeof AM_MEDIA_TYPE);
      hr = avInfo.GetStreamFormat(i, &amStream);
      if (SUCCEEDED(hr))
      {
         if (amStream.majortype == MEDIATYPE_Video && nVideo == -1)
            nVideo = i;
         else if (amStream.majortype == MEDIATYPE_Audio && nAudio == -1)
            nAudio = i;
         MYFREEMEDIATYPE(amStream);
      }
   }
   
   if (nVideo == -1)
   {
      // No video in AVI file. Strange, but true.
      // Output an error message.
      CString msg;
      msg.LoadString(IDS_MSG_IMPORTVIDEO_NOVIDEO);
      CEditorDoc::ShowErrorMessage(msg, m_pProgress->GetSafeHwnd());
      m_bFunctionSucceeded = false;
      return;// false;
   }
   
   // Assume we're using the same file for audio and video.
   // We'll check this here.
   CString csAudioFileName = csFileName;
   CString csOriginalAudioFile(_T(""));
   int nOriginalAudioStream = -1;
   
   // Now we do have a video, check if we have an audio
   // stream, too.
   if (nAudio != -1)
   {
      // Ask the user if he wants to import the audio file,
      // too.
      CString msg;
      msg.LoadString(IDS_MSG_IMPORTVIDEO_USEAUDIO);
      CString cap;
      cap.LoadString(IDS_QUESTION);
      int res = MessageBox(m_pProgress->GetSafeHwnd(), msg, cap, MB_ICONQUESTION | MB_YESNOCANCEL);
      // TODO use CEditorDoc::ShowQuestionMessage()
      if (res == IDCANCEL)
      {
         m_bFunctionSucceeded = false;
         return;// false;
      }
      else if (res == IDYES)
      {
         // Check if we have a decent audio format here
         AM_MEDIA_TYPE mtAudio;
         ZeroMemory(&mtAudio, sizeof AM_MEDIA_TYPE);
         hr = avInfo.GetStreamFormat(nAudio, &mtAudio);
         if (SUCCEEDED(hr))
         {
            bool wrongFormat = !AVInfo::StaticIsPcmAudio(&mtAudio);
            
            if (wrongFormat)
            {
               int nAudioStreamBackup = nAudio;

               bool cool = CheckAndRestoreAudioFile(csAudioFileName, nAudio, csFileName, nAudio);
               if (cool)
               {
                  csOriginalAudioFile = csFileName;
                  nOriginalAudioStream = nAudioStreamBackup;
                  // The audio stream is always 0 in WAV files.
                  nAudio = 0;
               }
               else
               {
                  /*
                  CString msg;
                  msg.LoadString(IDS_MSG_IMPORTVIDEO_AUDIOFAILED);
                  CString cap;
                  cap.LoadString(IDS_ERROR);
                  ::MessageBox(m_pProgress->GetSafeHwnd(), msg, cap,
                     MB_ICONERROR | MB_OK);
                  
                  FreeMediaType(mtAudio);
                  */
                  m_bFunctionSucceeded = false;
                  return;// false;
               }
            }
            
            MYFREEMEDIATYPE(mtAudio);
         }
      }
      else // Don't use audio
         nAudio = -1;
   }
   
   if (SUCCEEDED(hr))
   {
      
      CUnRedoElement *undo = new CUnRedoElement();
      if (nAudio == -1)
         undo->SetType(CUnRedoElement::IMPORT_VIDEO);
      else
         undo->SetType(CUnRedoElement::IMPORT_VIDEOAUDIO);
      
      if (nAudio != -1)
         undo->FillStreamListWith(m_streamList);
      else
         undo->FillStreamListWith(m_streamList, true);
      // copy preview list to undo element
      undo->Append(m_previewList, 0, m_previewList.GetSize(), true);
      // remove elements from redo list
      EmptyRedoList();
      m_undoList.AddTail(undo);
      

      CAudioStream *pAudioStream = NULL;
      if (nAudio != -1)
      {
         
         pAudioStream = new CAudioStream();
         pAudioStream->SetID(++CStreamContainer::m_iAudioStreamCount);
         pAudioStream->SetOriginalAudioSource(csOriginalAudioFile);
         pAudioStream->SetOriginalAudioSourceStream(nOriginalAudioStream);
         
         if (!InitAudioStream(pAudioStream, csAudioFileName, nAudio, IDS_MSG_IMPORTVIDEO_AUDIOOPEN_FAILED, 
            m_pProgress, m_pProgress->GetSafeHwnd()))
         {
            delete pAudioStream;
            m_bFunctionSucceeded = false;
            return; // false;
         }
         
         POSITION position = m_streamList.GetHeadPosition();
         while (position)
         {
            CStreamContainer *pContainer = m_streamList.GetNext(position);
            if (pContainer)
            {
               pContainer->SetAudioStream(pAudioStream);
            }
         }
         
         
         for (int i = 0; i < m_previewList.GetSize(); ++i)
         {
            CPreviewSegment *pSegment = m_previewList[i];
            if (pSegment)
            {
               int audioOffset = pSegment->GetTargetBegin() - pSegment->GetSourceBegin();
               pSegment->SetAudioOffset(audioOffset);
            }
         }
         
      }
      
      CVideoStream *pVideoStream = new CVideoStream();
      pVideoStream->SetID(++CStreamContainer::m_iVideoStreamCount);
      pVideoStream->SetFilename(csFileName, nVideo);
      // TODO: Can we get the real audio/video offset
      // from the AVI file here?
      pVideoStream->SetOffset(0);  
      // tripple code:
      int videoLength;
      if (!VideoInformation::GetVideoDuration(pVideoStream->GetFilename(), videoLength))
      {
         CString msg;
         msg.Format(IDS_MSG_GET_VIDEO_LENGTH_FAILED, pVideoStream->GetFilename());
         CEditorDoc::ShowWarningMessage(msg, m_pProgress->GetSafeHwnd());
      }
      else
      {
         pVideoStream->SetLength(videoLength);
      }
      
      POSITION position = m_streamList.GetHeadPosition();
      while (position)
      {
         CStreamContainer *pContainer = m_streamList.GetNext(position);
         if (pContainer)
         {
            pContainer->SetVideoStream(pVideoStream);
         }
      }
      
      for (int i = 0; i < m_previewList.GetSize(); ++i)
      {
         CPreviewSegment *pSegment = m_previewList[i];
         if (pSegment)
         {
            int videoOffset = pSegment->GetTargetBegin() - pSegment->GetSourceBegin();
            pSegment->SetVideoOffset(videoOffset);
         }
      }
      
      
      if (pAudioStream && GetTotalLength() < pAudioStream->GetLength())
      {
         // There is some similar code in ImportAudio!!

         // see notes there what this code is for

         CStreamContainer *pStream = new CStreamContainer();
         pStream->SetID(++CStreamContainer::m_nContainerCounter);
         pStream->SetAudioStream(pAudioStream);
         pStream->SetVideoStream(pVideoStream);
         m_streamList.AddTail(pStream);
         CPreviewSegment *pSegment = new CPreviewSegment();
         pSegment->SetStreams(pStream);
         int segmentStart = GetTotalLength();
         int segmentStop = pAudioStream->GetLength();
         
         UINT nAdditionalMs = segmentStop - segmentStart;
         
         pSegment->SetSourceBegin(segmentStart);
         pSegment->SetSourceEnd(segmentStop);
         pSegment->SetTargetBegin(segmentStart);
         pSegment->SetTargetEnd(segmentStop);
         m_previewList.Add(pSegment);
         
         for (int i=0; i<m_aStreams.GetSize(); ++i)
            m_aStreams[i]->EnlargeStreamLength(nAdditionalMs);
         
         // only to make last page longer
         UpdateStructureAndStreams();
      }
      
   }
   
   
   if (SUCCEEDED(hr))
      hr = m_UndoManager.RegisterAction(this, UNDO_LEGACY_CHANGE, NULL, 0, HINT_CHANGE_STRUCTURE);
   
   if (SUCCEEDED(hr)) 
      hr = m_UndoManager.SetRollbackPoint();
   

   if (FAILED(hr))
   {
      HandleError(IDS_MSG_IMPORTVIDEO_FAILED, hr);
   }
   else
   {
      // Forces the Editor to reinitialize the video export settings
      // upon preview/export using the just imported video file.
      ResetPreviewFormat();
   }
   
   //success = SUCCEEDED(hr) ? true : false;
   
   m_bFunctionSucceeded = true;
   //return success;
}

bool CEditorProject::WriteWavFromFile(const _TCHAR *tszFileName, int nStreamId, CString &csOutFileName, bool bTryGivenFileName)
{
   m_tszWriteWavFromFileFileName = tszFileName;
   m_nWriteWavFromFileStreamId = nStreamId;
   m_bWriteWavFromFileTryGivenFileName = bTryGivenFileName;
   if (bTryGivenFileName)
      m_csWriteWavFromFileTargetFileName = csOutFileName;
   
   m_pWritingAudioDlg = new CWritingAudioDlg(AfxGetMainWnd(), tszFileName);
   
   AfxBeginThread(WriteWavFromFileThreadLauncher, this);
   
   m_pWritingAudioDlg->DoModal();
   
   if (m_bWriteWavFromFileReturnValue)
      csOutFileName = m_csWriteWavFromFileTargetFileName;
   
   return m_bWriteWavFromFileReturnValue;
}

UINT CEditorProject::WriteWavFromFileThreadLauncher(LPVOID pData)
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   ((CEditorProject *) pData)->WriteWavFromFileThread();
   return 0L;
}

void CEditorProject::WriteWavFromFileThread()
{
   while (!m_pWritingAudioDlg->bInitialized)
      Sleep(100);
   
   const _TCHAR *tszFileName = m_tszWriteWavFromFileFileName;
   int nStreamId = m_nWriteWavFromFileStreamId;
   bool bTryGivenFileName = m_bWriteWavFromFileTryGivenFileName;
   
   CString csSourceFileName(tszFileName);
   CString csSuffix(tszFileName);
   CString csTargetFileName(tszFileName);
   CString csTargetFilePath(tszFileName);
   CString csOutFileName(_T(""));
   
   StringManipulation::GetFileSuffix(csSuffix);
   csSuffix.MakeLower();
   bool bWindowsMediaMode = false;
   if (csSuffix.Find(_T("wm"), 0) >= 0 || 
      csSuffix.Find(_T("asf"), 0) >= 0)
      bWindowsMediaMode = true;
   StringManipulation::GetFilePrefix(csTargetFileName);
   csTargetFileName += _T(".wav");
   StringManipulation::GetPath(csTargetFilePath);
   
   CString csSaveFileName;
   if (bTryGivenFileName)
      csSaveFileName = m_csWriteWavFromFileTargetFileName;
   else
      csSaveFileName = tszFileName;

   bool success = true;
   bool bWasCancelled = false;
   bool bFirstTime = true;
   bool bUseOldFile = false;

   while (!success && !bWasCancelled || bFirstTime)
   {
      success = true;
      if (!(bFirstTime && bTryGivenFileName))
      {
         csSaveFileName = tszFileName;
         while (success && csSaveFileName == tszFileName)
         {
            success = FileDialog::GetSaveFilename(AfxGetApp(), csSaveFileName, m_pWritingAudioDlg->GetSafeHwnd(), AfxGetInstanceHandle(), 
               _T("WAV Files (*.wav)|*.wav||"), _T(".wav"), _T("WAVEXPORT"), csTargetFilePath, csTargetFileName);
            if (csSaveFileName == tszFileName)
            {
               CString csMsg;
               csMsg.LoadString(IDS_MSG_AUDIOFILENAMES_EQUAL);
               CEditorDoc::ShowWarningMessage(csMsg, m_pWritingAudioDlg->GetSafeHwnd());
            }

            if (csSaveFileName.IsEmpty())
            {
               success = false;
               bWasCancelled = true;
            }
         }

         if (success)
         {
            LFile file(csSaveFileName);
            if (file.Exists())
            {
               AVInfo avInfo;
               HRESULT hr = avInfo.Initialize(csSaveFileName);
               if (SUCCEEDED(hr))
               {
                  AM_MEDIA_TYPE mt;
                  ZeroMemory(&mt, sizeof AM_MEDIA_TYPE);
                  hr = avInfo.GetStreamFormat(0, &mt);
                  if (SUCCEEDED(hr))
                  {
                     if (true == AVInfo::StaticIsPcmAudio(&mt))
                     {
                        CString csMsg;
                        csMsg.LoadString(IDS_MSG_WAV_FILE_IS_OK);
                        CString csCap;
                        csCap.LoadString(IDS_QUESTION);
                        int nRes = ::MessageBox(m_pWritingAudioDlg->GetSafeHwnd(), 
                                                csMsg, csCap, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
                        // TODO: use CEditorDoc::ShowQuestionMessage()

                        if (IDYES == nRes)
                        {
                           csOutFileName = csSaveFileName;
                           bUseOldFile = true;
                        }
                     }

                     MYFREEMEDIATYPE(mt);
                  }
               }
            }
         }
      }
   
      bool bOutOfDisk = false;
      if (success && !bUseOldFile)
      {
         ::PostMessage(m_pWritingAudioDlg->GetSafeHwnd(), WM_USER, 
            CWritingAudioDlg::UPDATE_TARGET, (LPARAM) (const _TCHAR *) csSaveFileName);
         HRESULT hr = AVInfo::StaticWriteWavFromFile(tszFileName, nStreamId, csSaveFileName, bWindowsMediaMode, m_pWritingAudioDlg->GetSafeHwnd());
         success = SUCCEEDED(hr);
         if (hr == E_ABORT)
            bWasCancelled = true;
         else if (hr == E_OUTOFMEMORY)
            bOutOfDisk = true;
         if (success)
            csOutFileName = csSaveFileName;
      }

      if (!success)
      {
         CString csMsg;
         if (bWasCancelled)
         {
            csMsg.LoadString(IDS_MSG_OPERATIONCANCELLED);
            CEditorDoc::ShowInfoMessage(csMsg, m_pWritingAudioDlg->GetSafeHwnd());
         }
         else if (bOutOfDisk)
         {
            csMsg.LoadString(IDS_MSG_DISKFULL_WAV);
            if (IDNO == CEditorDoc::ShowErrorMessage(csMsg, m_pWritingAudioDlg->GetSafeHwnd(), MB_YESNO))
            {
               bWasCancelled = true;
            }
         }
         else
         {
            csMsg.Format(IDS_MSG_IMPORTVIDEO_AUDIOFAILED, csSaveFileName);
            if (IDCANCEL == CEditorDoc::ShowErrorMessage(csMsg, m_pWritingAudioDlg->GetSafeHwnd(), MB_OKCANCEL))
            {
               bWasCancelled = true;
            }
         }
      }

      bFirstTime = false;
   }
   
   if (success)
      m_csWriteWavFromFileTargetFileName = csOutFileName;
   
   m_bWriteWavFromFileReturnValue = success;
   
   ::PostMessage(m_pWritingAudioDlg->GetSafeHwnd(), WM_USER, CWritingAudioDlg::END_DIALOG, NULL);
}

bool CEditorProject::CheckAndRestoreAudioFile(CString &csAudioFile, int &nAudioStream, 
                                              const _TCHAR *tszOrigAudio, int nOrigStream)
{
   // Does the file exist?
   bool success = true;
   success = (FALSE == csAudioFile.IsEmpty());
   if (success)
   {
      LFile file(csAudioFile);
      success = file.Exists();
   }

   if (success)
      success = AVInfo::StaticIsFilePcmAudio(csAudioFile);

   // If the file still exists here, we're ready
   if (success)
      return true;

   // Ok, we know that csAudioFile does not exist anymore,
   // or it has a wrong format (i.e. was overwritten in some
   // way). We need to write a new file.

   // Does the original file exist?
   if (NULL == tszOrigAudio)
      return false;
   LFile origFile(tszOrigAudio);
   if (!origFile.Exists())
      return false;

   // Ok, we will do something with some audio files.
   // Tell the project that we have changed.
   m_pDoc->SetModifiedFlag();
   m_pDoc->SetDirtyDuringOpen(true);

   CString csLecturnityHome(_T(""));
   CString csEditorTemp(_T(""));
   CSecurityUtils sec;
   success = sec.GetLecturnityHome(csLecturnityHome);
   if (success)
   {
      if (csLecturnityHome[csLecturnityHome.GetLength() - 1] != _T('\\'))
         csLecturnityHome += _T('\\');
      csEditorTemp = csLecturnityHome;
      csEditorTemp += _T("Editor Temp\\");

      success = LIo::CreateDirs(csLecturnityHome, _T("Editor Temp"));

      if (success)
      {
         CString csFilePrefix(tszOrigAudio);
         StringManipulation::GetFilePrefix(csFilePrefix);

         __int64 i64FileSize = LIo::GetFileSize(tszOrigAudio);
         int nFileSize = (int) i64FileSize;

         CString csTmpAudioFile;
         csTmpAudioFile.Format(_T("%s%s_%d.wav"), csEditorTemp, csFilePrefix, nFileSize);

         // Does this file already exist? And does it already have the correct format?
         if (AVInfo::StaticIsFilePcmAudio(csTmpAudioFile))
         {
            csAudioFile = csTmpAudioFile;
            nAudioStream = 0;
            return true;
         }

         // The file does either not exist or it has the wrong format.
         // Write a new WAV file. Note: csTmpAudioFile may be changed
         // in the WriteWavFromFile method!
         success = WriteWavFromFile(tszOrigAudio, nOrigStream, csTmpAudioFile, true);

         if (success)
         {
            csAudioFile = csTmpAudioFile;
            nAudioStream = 0;
            return true;
         }
      }
   }

   return success;
}

bool CEditorProject::RemoveVideo()
{
   // See Bug 2232
   bool bHadVideo = HasVideo();

   CUnRedoElement *undo = new CUnRedoElement();
   undo->SetType(CUnRedoElement::IMPORT_VIDEO);
   
   undo->FillStreamListWith(m_streamList, true);
   // copy preview list to undo element
   undo->Append(m_previewList, 0, m_previewList.GetSize(), true);
   // remove elements from redo list
   EmptyRedoList();
   m_undoList.AddTail(undo);
   

   HRESULT hr = m_UndoManager.RegisterAction(this, UNDO_LEGACY_CHANGE, NULL, 0, HINT_CHANGE_STRUCTURE);
   
   if (SUCCEEDED(hr)) 
      hr = m_UndoManager.SetRollbackPoint();


   POSITION position = m_streamList.GetHeadPosition();
   while (position)
   {
      CStreamContainer *pContainer = m_streamList.GetNext(position);
      if (pContainer)
      {
         pContainer->SetVideoStream(NULL);
      }
   }
   
   for (int i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *pSegment = m_previewList[i];
      if (pSegment)
      {
         pSegment->SetVideoOffset(0);
      }
   }

   // See Bug 2232
   if (bHadVideo && !HasVideo())
      m_videoBackupAviSettings.Clone(&m_videoAviSettings);
   
   ResetPreviewFormat();
   
   return true;
}

UINT CEditorProject::ExportAudioAsWavThreadLauncher(LPVOID lParam)
{
   ASSERT(lParam);
 
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   ((CEditorProject *) lParam)->ExportAudioAsWavThread();
   
   return 0;
}

void CEditorProject::ExportAudioAsWavThread()
{
   LPCTSTR tszFileName = m_exportAudioAsWavFileName;
   LPCTSTR tszRecordPath = m_exportAudioAsWavPath;
   
   // As we need the dialog, wait for it to initialize
   DWORD res = ::WaitForSingleObject(m_hDialogInitEvent, INFINITE);
   
   m_pExportProgressDialog->SetParticularLabel(IDS_EXPORT_AUDIO);
   m_pExportProgressDialog->SetOverallSteps(4);
   m_pExportProgressDialog->SetOverallProgress(0);
   
   HRESULT hr = ::CoInitialize(NULL);
   
   bool bExcludeClipAudio = false;

   /*
   // Does this document contain clips with audio?
   if (HasSgClips() && !IsDenverDocument())
   {
      bool bHasClipsWithAudio = false;

      CClipStreamR *pClipStream = GetClipStream();
      if (pClipStream != NULL)
      {
         bHasClipsWithAudio = pClipStream->HasAudio();
      }
 
      
      if (bHasClipsWithAudio)
      {
         CString csMsg;
         csMsg.LoadString(IDS_MSG_EXPORTCLIPAUDIO);
         CString csCap;
         csCap.LoadString(IDS_QUESTION);
         
         int nRes = ::MessageBox(m_pExportProgressDialog->GetSafeHwnd(), csMsg, csCap, 
            MB_YESNO | MB_ICONQUESTION);
         if (IDNO == nRes)
            bExcludeClipAudio = true;
      }
   }

      //hr = InitAudioVideo(false, szFileName, true, bExcludeClipAudio);
      */
   
   // TODO consider bExcludeClipAudio (see above)
   if (SUCCEEDED(hr))
      hr = PrepareRenderGraph(false, tszFileName, true);

   m_pExportProgressDialog->SetOverallProgress(1);
   if (SUCCEEDED(hr))
      hr = m_avEdit.InitializeExport(tszRecordPath, m_pExportProgressDialog->GetSafeHwnd());
   m_pExportProgressDialog->SetOverallProgress(2);
   if (SUCCEEDED(hr))
      hr = m_avEdit.StartExport(NULL);
   
   if (FAILED(hr) && !m_bCancelExportRequested)
   {
      // Check disk full?
      if (hr == E_OUTOFMEMORY)
      {
         CString csMsg;
         csMsg.LoadString(IDS_MSG_DISKFULL);
         CEditorDoc::ShowErrorMessage(csMsg, m_pExportProgressDialog->GetSafeHwnd());
      }
      else
      {
         // General error message
         CString msg;
         msg.Format(IDS_MSG_AVEXPORT_FAILED, hr);
         CEditorDoc::ShowErrorMessage(msg, m_pExportProgressDialog->GetSafeHwnd());
      }
   }
   
   if (m_bCancelExportRequested)
   {
      CString msg;
      msg.LoadString(IDS_MSG_OPERATIONCANCELLED);
      CEditorDoc::ShowInfoMessage(msg, m_pExportProgressDialog->GetSafeHwnd());
   }
   
   ::CoUninitialize();
   
   ::PostMessage(m_pExportProgressDialog->GetSafeHwnd(), WM_USER, CExportProgressDialog::END_DIALOG, NULL);
}

//void CEditorProject::ExportVideo(CString &recordPath, CString &prefix)
//{
//}

bool CEditorProject::ExportClips(CString &recordPath, CString &prefix, IProgressListener *pListener)
{
   HWND hwndParent = NULL;
   if (m_pExportProgressDialog != NULL)
       hwndParent = m_pExportProgressDialog->GetSafeHwnd();

   bool success = true;
   
   ASSERT(!IsDenverDocument());
   

   CClipStreamR *pClipStream = GetClipStream();
   if (pClipStream != NULL)
   {
      // TODO GetSegments(): evil hack(s) for something that should be handled in CClipStreamR
      CArray<CClipSegment, CClipSegment> *pSegments = pClipStream->GetSegments();

      for (int i=0; i<pSegments->GetSize(); ++i)
      {
         CClipSegment *pClipSegment = &pSegments->GetAt(i);

         UINT nStartPosMs = pClipSegment->GetTargetBegin();
            
         
         CString csClipSource = pClipSegment->GetFilename();
         CAviSettings *pClipAviSettings = 
            LookupClipAviSettings(csClipSource, pClipSegment->GetTempOriginalAudioVolume());

         CString clipFileName;
         clipFileName.Format(_T("%s_clip_%d.avi"), prefix, pClipSegment->GetTargetBegin());
      
         CArray<const AVSource *, const AVSource *> clipSources;

         // construct an AVSource object for every segment;
         // several segments are joined in one AVTarget (file) if 
         // they belong together

         UINT nSegmentStartMs = 0;
         bool bJoinSegments = true;
         while (bJoinSegments)
         {
            const AVSource *pClipSource = new AVSource(
               pClipSegment->GetFilename(),
               nSegmentStartMs,
               nSegmentStartMs + pClipSegment->GetLength(),
               pClipSegment->GetSourceBegin(),
               pClipSegment->GetSourceBegin() + pClipSegment->GetLength(),
               pClipSegment->GetVideoStreamSelector()
               );
            clipSources.Add(pClipSource);
            nSegmentStartMs += pClipSegment->GetLength();
            
            // TODO the merged segments in CClipStreamR unfortunately cannot
            // be used for this (at the moment) as we need all the segments here.
            // But it should be used. Or even better this be done in CClipStreamR.
            if (i+1 < pSegments->GetSize() && pClipSegment->BelongsTo(&pSegments->GetAt(i+1)))
            {
               pClipSegment = &pSegments->GetAt(i+1);
               ++i;
            }
            else
            {
               bJoinSegments = false;
            }
         }

         UINT nFileLengthMs = nSegmentStartMs;

         bool bRemoveClip = false;
         
         // check if length is enough for input frame rate
         // TODO why use input frame rate here?
         AVInfo avInfo;
         HRESULT hr = avInfo.Initialize(pClipSegment->GetFilename());
         if (SUCCEEDED(hr))
         {
            AM_MEDIA_TYPE clipMt;
            hr = avInfo.GetStreamFormat(pClipSegment->GetVideoStreamSelector(), &clipMt);
            if (SUCCEEDED(hr))
            {
               if (clipMt.formattype == FORMAT_VideoInfo)
               {
                  VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *) clipMt.pbFormat;
                  ULONG lTimePerFrame = (ULONG) (pVih->AvgTimePerFrame / 10000i64);
                  
                  bRemoveClip = nFileLengthMs < lTimePerFrame;
               }
            }
         }
         
         if (bRemoveClip)
         {
            CString csClipPos;
            Misc::FormatTickString(csClipPos, nStartPosMs, SU_MINUTES, true);
            CString csMsg;
            csMsg.Format(IDS_MSG_CLIP_TOO_SHORT, csClipPos, nFileLengthMs);

            if (pListener == NULL)
                CEditorDoc::ShowInfoMessage(csMsg, hwndParent);
            else
                pListener->ShowMessage(PROGRESS_MSG_WARNING, csMsg, NULL);
         }
         
        
         if (!bRemoveClip)
         {
            int clipSegmentCount = clipSources.GetSize();
            const AVSource **paSources = new const AVSource*[clipSegmentCount];
            for (int i=0; i<clipSegmentCount; ++i)
               paSources[i] = clipSources.GetAt(i);
            
            const AVTarget **paTargets = new const AVTarget*[1];
            const AVTarget *pTarget = new AVTarget(
               paSources,
               clipSegmentCount,
               &pClipAviSettings->videoFormat, // Media type
               clipFileName,
               pClipAviSettings->bUseSmartRecompression,
               AV_VALUE_UNDEFINED,
               AV_VALUE_UNDEFINED,
               pClipAviSettings->dwFCC, // FCC: Not defined/Use default
               pClipAviSettings->bUseKeyframes ? pClipAviSettings->keyframeRate : 0,
               pClipAviSettings->quality
               );
            
            paTargets[0] = pTarget;
            
            HRESULT hr = S_OK;
            if (SUCCEEDED(hr))
               hr = m_avEdit.InitializeTimelines(paTargets, 1, false, NULL);
            if (SUCCEEDED(hr))
               hr = m_avEdit.InitializeExport(recordPath, hwndParent);
            if (SUCCEEDED(hr))
               hr = m_avEdit.StartExport(NULL);
            
            if (FAILED(hr) && !m_bCancelExportRequested)
            {
               if (hr == E_FAIL &&
                  m_clipExportUseSmartRecompression)
               {
                  CString msg;
                  msg.LoadString(IDS_MSG_CLIPEXPORT_80004005);

                  if (pListener == NULL)
                      CEditorDoc::ShowErrorMessage(msg, hwndParent);
                  else
                      pListener->ShowMessage(PROGRESS_MSG_ERROR, msg, NULL);

                  success = false;
               }
               else
               {
                  CString msg;
                  msg.Format(IDS_MSG_CLIPEXPORT_FAILED, hr);

                  if (pListener == NULL)
                      CEditorDoc::ShowErrorMessage(msg, hwndParent);
                  else
                      pListener->ShowMessage(PROGRESS_MSG_ERROR, msg, NULL);

                  success = false;
               }
            }

            // cleanup, m_avEdit does a copy (DeepClone()) of AVSource and AVTarget objects
            delete pTarget;
            delete[] paSources;
            delete[] paTargets;
         }

         // cleanup, m_avEdit does a copy (DeepClone()) of AVSource and AVTarget objects
         if (clipSources.GetSize() > 0)
         {
            for (int i=0; i<clipSources.GetSize(); ++i)
               delete clipSources[i];
         }

      }
   }

   
   return success;
}

void CEditorProject::SaveProject(CString &recordPath, CString &prefix, TCHAR *tszLastExportPath)
{
   if (m_previewList.GetSize() == 0)
      return;

    if (FirstQuestionnaireIsRandomTest()) {
        if (m_pDoc && m_pDoc->IsPreviewActive()) {
            CQuestionnaireEx *pFirstQuestionnaire = GetFirstQuestionnaireEx();
            if (pFirstQuestionnaire)
                pFirstQuestionnaire->RestoreDocumentFromRandomizePlay();
        }
    }

   CString lepFilename;
   lepFilename.Format(_T("%s\\%s.lep"), recordPath, prefix);
  
   HANDLE hLepFile = CreateFile(lepFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hLepFile == INVALID_HANDLE_VALUE)
   {
      CString msg;
      msg.Format(IDS_MSG_OPEN_EVQ_FAILED, lepFilename);
      CEditorDoc::ShowErrorMessage(msg);
      return;
   }

   CFileOutput out;
   out.Init(hLepFile, 0, true);
   
   CString csSgmlString;
   CString line;
   
   line = _T("<editor>\n");
   out.Append(line);

   line.Format(_T("  <formatversion compatible=\"%s\">%s</formatversion>\n"), 
       CStudioApp::s_sCompatibleLepVersion, CStudioApp::s_sCurrentLepVersion);
   out.Append(line);

   // NOTE: This line must be at the beginning of the file because other code (ie in lutils)
   //       depends on it and only reads a small amount of the lep file.
   CString csSgmlLastExportPath;
   csSgmlLastExportPath.Empty();
   if (tszLastExportPath != NULL)
       StringManipulation::TransformForSgml(tszLastExportPath, csSgmlLastExportPath);
   line.Format(_T("  <lastexportpath>%s</lastexportpath>\n"), csSgmlLastExportPath);
   out.Append(line); // utf-8 is taken care of by CArrayOutput out
   
   // project meta information
   WriteMetaInformation(&out);
   
   // configuration information
   WriteConfiguration(&out);
   
   // source file information
   WriteFileInformation(&out, recordPath, prefix);

   // write stream information
   line = _T("  <streams>\n");
   out.Append(line);  
   
   // audio stream
   WriteAudioStreamEntry(&out, recordPath, prefix);
   
   // video stream
   if (HasVideo())
      WriteVideoStreamEntry(&out, recordPath, prefix);
   
   if (!m_csStillImage.IsEmpty())
   {
      line.Format(_T("    <image>%s</image>\n"), StringManipulation::MakeRelativeAndSgml(lepFilename, m_csStillImage));
      out.Append(line);
   }
   
   
   // wb stream
   WriteWhiteboardStreamEntry(&out, recordPath, prefix);

   if (m_aPageStream.GetSize() > 0)
      WriteSlideStream(&out, recordPath, prefix);

   HRESULT hr = S_OK;
   if (m_pMarkStream != NULL)
      hr = m_pMarkStream->WriteLepEntry(&out);
   if (SUCCEEDED(hr) && m_pClipStream != NULL)
     hr = m_pClipStream->WriteLepEntry(&out, lepFilename);
   if (SUCCEEDED(hr) && m_pInteractionStream != NULL)
      hr = m_pInteractionStream->WriteLepEntry(&out);
   if (SUCCEEDED(hr) && m_pQuestionStream != NULL)
      hr = m_pQuestionStream->WriteLepEntry(&out, lepFilename);
   _ASSERT(SUCCEEDED(hr));

   
   line = _T("  </streams>\n");
   out.Append(line);
   
   line = _T("  <export>\n");
   out.Append(line);  
   
   // The m_videoAviSettings member is used both for video codec settings
   // and for the codec settings of pure screen grabbing documents (denver
   // mode documents).
   if (HasVideo() || IsDenverDocument())
   {
      line = _T("    <video>\n");
      out.Append(line);  
      
      WriteVideoExportType(&out, &m_videoAviSettings);

      line.Format(_T("      <globaloffset>%d</globaloffset>\n"), m_videoExportGlobalOffset);
      out.Append(line);
      
      line = _T("    </video>\n");
      out.Append(line);
   }
   
   line = _T("    <audio>\n");
   out.Append(line);  
   
   int mediaSize = 0;
   int formatSize = 0;
   Misc::GetStreamConfigDataSizes(&m_mtAudioFormat, &mediaSize, &formatSize);
   char *media = new char[mediaSize];
   char *format = new char[formatSize];
   Misc::GetStreamConfigData(&m_mtAudioFormat, media, &mediaSize, format, &formatSize);
   line.Format(_T("      <mediatype>%S</mediatype>\n"), media);
   out.Append(line);
   line.Format(_T("      <format>%S</format>\n"), format);
   out.Append(line);
   line.Format(_T("      <ismediatypeinitialized>%d</ismediatypeinitialized>\n"), m_isAudioFormatInitialized);
   out.Append(line);
   
   delete[] media;
   delete[] format;

   line = _T("    </audio>\n");
   out.Append(line);
   
   if (HasSgClips())
   {
      line = _T("    <clips>\n");
      out.Append(line);  
      
      line.Format(_T("      <smartrecompression>%d</smartrecompression>\n"), m_clipExportUseSmartRecompression);
      out.Append(line);
      
      line.Format(_T("      <stretchclips>%d</stretchclips>\n"), m_clipExportStretchDenverClips);
      out.Append(line);

      line.Format(_T("      <autocalcsize>%d</autocalcsize>\n"), m_clipExportAutoCalcDenverSize);
      out.Append(line);
      
      line = _T("    </clips>\n");
      out.Append(line);
      
      POSITION pos = m_clipAviSettings.GetStartPosition();
      while (pos)
      {
         CString csClipSource(_T(""));
         CAviSettings *pClipSettings = NULL;
         m_clipAviSettings.GetNextAssoc(pos, csClipSource, pClipSettings);
         if (pClipSettings)
         {
            line = _T("    <clip>\n");
            out.Append(line);
            
            line.Format(_T("      <filename>%s</filename>\n"), 
               StringManipulation::MakeRelativeAndSgml(lepFilename, csClipSource));
            out.Append(line);
            WriteVideoExportType(&out, pClipSettings);
            
            line = _T("    </clip>\n");
            out.Append(line);
         }
      }
   }
   
   line = _T("  </export>\n");
   out.Append(line);
   
   line = _T("</editor>\n");
   out.Append(line);
   
   out.Flush();
   CloseHandle(hLepFile);
}

void CEditorProject::WriteVideoExportType(CFileOutput *pOutput, CAviSettings *pAviSettings)
{
   int mediaSize = 0;
   int formatSize = 0;
   Misc::GetStreamConfigDataSizes(&pAviSettings->videoFormat, &mediaSize, &formatSize);
   char *media = new char[mediaSize];
   char *format = new char[formatSize];
   Misc::GetStreamConfigData(&pAviSettings->videoFormat, media, &mediaSize, format, &formatSize);
   
   CString line;
   line.Format(_T("      <mediatype>%S</mediatype>\n"), media);
   pOutput->Append(line);
   line.Format(_T("      <format>%S</format>\n"), format);
   pOutput->Append(line);
   line.Format(_T("      <ismediatypeinitialized>%d</ismediatypeinitialized>\n"), pAviSettings->bIsVideoFormatInitialized);
   pOutput->Append(line);
   line.Format(_T("      <codec>%d</codec>\n"), pAviSettings->dwFCC);
   pOutput->Append(line);
   line.Format(_T("      <quality>%d</quality>\n"), pAviSettings->quality);
   pOutput->Append(line);
   line.Format(_T("      <keyframerate>%d</keyframerate>\n"), pAviSettings->keyframeRate);
   pOutput->Append(line);
   line.Format(_T("      <usekeyframes>%d</usekeyframes>\n"), pAviSettings->bUseKeyframes);
   pOutput->Append(line);
   line.Format(_T("      <smartrecompression>%d</smartrecompression>\n"), pAviSettings->bUseSmartRecompression);
   pOutput->Append(line);
   line.Format(_T("      <rememberaspectratio>%d</rememberaspectratio>\n"), pAviSettings->bRememberAspectRatio);
   pOutput->Append(line);
   line.Format(_T("      <volume>%d</volume>\n"), pAviSettings->nAudioVolume);
   pOutput->Append(line);
   line.Format(_T("      <origvolume>%d</origvolume>\n"), pAviSettings->nOriginalAudioVolume);
   pOutput->Append(line);
   line.Format(_T("      <origwidth>%d</origwidth>\n"), pAviSettings->nOriginalWidth);
   pOutput->Append(line);
   line.Format(_T("      <origheight>%d</origheight>\n"), pAviSettings->nOriginalHeight);
   pOutput->Append(line);
   line.Format(_T("      <origfps>%f</origfps>\n"), pAviSettings->dOriginalFps);
   pOutput->Append(line);
   line.Format(_T("      <hasorigsize>%d</hasorigsize>\n"), pAviSettings->bHasOriginalSize);
   pOutput->Append(line);

   delete[] media;
   delete[] format;
}

void CEditorProject::WriteMetaInformation(CFileOutput *pOutput)
{
   CString line;
   CString sgmlString;
   
   line = _T("  <metainfo>\n");
   pOutput->Append(line);
   
   StringManipulation::TransformForSgml(m_targetMetainfo.GetTitle(), sgmlString);
   line.Format(_T("    <title>%s</title>\n"), sgmlString);
   pOutput->Append(line);
   
   StringManipulation::TransformForSgml(m_targetMetainfo.GetAuthor(), sgmlString);
   line.Format(_T("    <author>%s</author>\n"), sgmlString);
   pOutput->Append(line);
   
   StringManipulation::TransformForSgml(m_targetMetainfo.GetCreator(), sgmlString);
   line.Format(_T("    <creator>%s</creator>\n"), sgmlString);
   pOutput->Append(line);
   
   StringManipulation::TransformForSgml(m_targetMetainfo.GetComments(), sgmlString);
   line.Format(_T("    <comments>%s</comments>\n"), sgmlString);
   pOutput->Append(line);

   //make sure correct length is used
   CString csLen = _T("00:00");
   int iRecordingTime = m_avEdit.GetAudioPreviewLength();
   //on export, m_avEdit returns 0 so fall back to m_targetMetainfo
   if (iRecordingTime){
	   int min = (float)iRecordingTime / 60000;
	   int msecLeft = iRecordingTime - (min*60000);
	   int sec = (float)msecLeft / 1000;
	   csLen.Format(_T("%02d:%02d"), min, sec);
   }
   else
	   csLen = m_targetMetainfo.GetLength();

   //StringManipulation::TransformForSgml(m_targetMetainfo.GetLength(), sgmlString);
   StringManipulation::TransformForSgml(csLen, sgmlString);
   line.Format(_T("    <length>%s</length>\n"), sgmlString);
   pOutput->Append(line);
   
   StringManipulation::TransformForSgml(m_targetMetainfo.GetRecordDate(), sgmlString);
   line.Format(_T("    <recorddate>%s</recorddate>\n"), sgmlString);
   pOutput->Append(line);
   
   StringManipulation::TransformForSgml(m_targetMetainfo.GetRecordTime(), sgmlString);
   line.Format(_T("    <recordtime>%s</recordtime>\n"), sgmlString);
   pOutput->Append(line); 
   
   // everything written since 2.0.p2 is utf-8
   pOutput->Append(_T("    <codepage>utf-8</codepage>\n"));
   
   CStringList stringList;
   m_targetMetainfo.GetKeywords(stringList);
   if (!stringList.IsEmpty())
   {
      POSITION position = stringList.GetHeadPosition();
      while(position)
      {
         CString string = stringList.GetNext(position);
         StringManipulation::TransformForSgml(string, sgmlString);
         line.Format(_T("    <keyword>%s</keyword>\n"), sgmlString);
         pOutput->Append(line);
      }
   }
   
   line = _T("  </metainfo>\n");
   pOutput->Append(line);
}


void CEditorProject::WriteFileInformation(CFileOutput *pOutput, CString &recordPath, CString &prefix)
{
   CString csLepFileName;
   csLepFileName.Format(_T("%s\\%s"), recordPath, prefix);
   
   CString line;
   CString sgmlString;
   
   line = _T("  <files>\n");
   pOutput->Append(line);  
   POSITION position = m_recordingList.GetHeadPosition();
   while (position)
   {
      CLrdFile *element = m_recordingList.GetNext(position);
      if (element)
      {
         CString csAbsFileName;
         csAbsFileName.Format(_T("%s\\%s"), element->GetRecordPath(), element->GetFilename());
         line.Format(_T("    <lrd>%s</lrd>\n"), StringManipulation::MakeRelativeAndSgml(csLepFileName, csAbsFileName));
         pOutput->Append(line);
      }
   }
   line = _T("  </files>\n");
   pOutput->Append(line);
}

void CEditorProject::WriteConfiguration(CFileOutput *pOutput) {
    CString line;
    CString sgmlString;

    line = _T("  <config>\n");
    pOutput->Append(line);  

    line.Format(_T("      <sg-showclickpages>%d</sg-showclickpages>\n"), m_bShowClickPages);
    pOutput->Append(line);  

    if (m_bShowScreenGrabbingAsRecording)
        line = _T("      <sg-viewmode>recording</sg-viewmode>\n");
    else
        line = _T("      <sg-viewmode>simulation</sg-viewmode>\n");
    pOutput->Append(line);  

    line = _T("  </config>\n");
    pOutput->Append(line);
}

void CEditorProject::WriteAudioStreamEntry(CFileOutput *pOutput, CString &recordPath, CString &prefix)
{  
   if (m_previewList.GetSize() == 0)
      return;
   
   CString csLepFileName;
   csLepFileName.Format(_T("%s\\%s"), recordPath, prefix);
   
   int begin = -1;
   int end   = -1;
   for (int i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *element = m_previewList[i];
      if (element && element->GetAudioStream())
      {
         if (begin == -1 || element->GetTargetBegin() < begin)
            begin = element->GetTargetBegin();
         if (end == -1 || element->GetTargetEnd() > end)
            end = element->GetTargetEnd();
      }
   }
   if (begin == -1 || end == -1) 
      return;
   
   CString line;
   CString sgmlString;
   
   line.Format(_T("    <audio start=\"%d\" end=\"%d\">\n"), begin, end);
   pOutput->Append(line); 
   
   for (i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *element = m_previewList[i];
      if (element && element->GetAudioStream())
      {
         CString csOrigAudioSource = _T("");
         if (element->GetAudioStream()->HasOriginalAudioSource())
         {
            csOrigAudioSource = StringManipulation::MakeRelativeAndSgml
               (csLepFileName, element->GetAudioStream()->GetOriginalAudioSource());
         }
         line.Format(_T("      <source id=\"%d\" stream-id=\"%d\" file=\"%s\" stream-start=\"%d\" stream-end=\"%d\" source-start=\"%d\" source-end=\"%d\" source-offset=\"%d\" stream-selector=\"%d\" orig-file=\"%s\" orig-stream-selector=\"%d\"></source>\n"), 
            element->GetStreams()->GetID(), element->GetAudioStream()->GetID(), 
            StringManipulation::MakeRelativeAndSgml(csLepFileName, element->GetAudioStream()->GetFilename()), //sgmlString, 
            element->GetTargetBegin(), element->GetTargetEnd(),
            element->GetSourceBegin(), element->GetSourceEnd(),
            element->GetAudioOffset(), element->GetAudioStream()->GetStreamSelector(),
            csOrigAudioSource, 
            element->GetAudioStream()->GetOriginalAudioSourceStream());
         pOutput->Append(line); 
      }
   }
   line = _T("    </audio>\n");
   pOutput->Append(line); 
}

void CEditorProject::WriteVideoStreamEntry(CFileOutput *pOutput, CString &recordPath, CString &prefix)
{
   if (m_previewList.GetSize() == 0)
      return;
   
   CString csLepFileName;
   csLepFileName.Format(_T("%s\\%s"), recordPath, prefix);
   
   int begin = -1;
   int end   = -1;
   for (int i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *element = m_previewList[i];
      if (element && element->GetVideoStream())
      {
         if (begin == -1 || element->GetTargetBegin() < begin)
            begin = element->GetTargetBegin();
         if (end == -1 || element->GetTargetEnd() > end)
            end = element->GetTargetEnd();
      }
   }
   
   if (begin == -1 || end == -1) 
      return;
   
   CString line;
   CString sgmlString;
   
   line.Format(_T("    <video start=\"%d\" end=\"%d\">\n"), begin, end);
   pOutput->Append(line);
   
   for (i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *element = m_previewList[i];
      if (element && element->GetVideoStream())
      {
         line.Format(_T("      <source id=\"%d\" stream-id=\"%d\" file=\"%s\" offset=\"%d\" stream-start=\"%d\" stream-end=\"%d\" source-start=\"%d\" source-end=\"%d\" source-offset=\"%d\" stream-selector=\"%d\"></source>\n"), 
            element->GetStreams()->GetID(), element->GetVideoStream()->GetID(),
            StringManipulation::MakeRelativeAndSgml(csLepFileName, element->GetVideoStream()->GetFilename()), 
            element->GetVideoStream()->GetOffset(), 
            element->GetTargetBegin(), element->GetTargetEnd(),
            element->GetSourceBegin(), element->GetSourceEnd(),
            element->GetVideoOffset(), element->GetVideoStream()->GetStreamSelector());
         pOutput->Append(line); 
      }
   }
   line = _T("    </video>\n");
   pOutput->Append(line);  
   
}

void CEditorProject::WriteWhiteboardStreamEntry(CFileOutput *pOutput, CString &recordPath, CString &prefix)
{  
   if (m_previewList.GetSize() == 0)
      return;
   
   CString csLepFileName;
   csLepFileName.Format(_T("%s\\%s"), recordPath, prefix);
   
   if (!IsDenverDocument())
   {
      int begin = -1;
      int end   = -1;
      for (int i = 0; i < m_previewList.GetSize(); ++i)
      {
         CPreviewSegment *element = m_previewList[i];
         if (element && element->GetWhiteboardStream())
         {
            if (begin == -1 || element->GetTargetBegin() < begin)
               begin = element->GetTargetBegin();
            if (end == -1 || element->GetTargetEnd() > end)
               end = element->GetTargetEnd();
         }
      }
      
      if (begin == -1 || end == -1) 
         return;
      
      CString line;
      CString sgmlString;
      
      line.Format(_T("    <wb start=\"%d\" end=\"%d\">\n"), begin, end);
      pOutput->Append(line); 
      line = _T("      <targetformat>\n");
      pOutput->Append(line); 
      line.Format(_T("        <pagedimension>%d x %d</pagedimension>\n"), m_targetPageWidth, m_targetPageHeight);
      pOutput->Append(line); 
      line = _T("      </targetformat>\n");
      pOutput->Append(line); 
      for (i = 0; i < m_previewList.GetSize(); ++i)
      {
         CPreviewSegment *element = m_previewList[i];
         if (element && element->GetWhiteboardStream())
         {
            line.Format(_T("      <source id=\"%d\" stream-id=\"%d\" stream-start=\"%d\" stream-end=\"%d\" source-start=\"%d\" source-end=\"%d\">\n"), 
               element->GetStreams()->GetID(), element->GetWhiteboardStream()->GetID(), 
               element->GetTargetBegin(), element->GetTargetEnd(),
               element->GetSourceBegin(), element->GetSourceEnd());
            pOutput->Append(line); 
            
            line.Format(_T("        <evq>%s</evq>\n"), StringManipulation::MakeRelativeAndSgml(csLepFileName, element->GetWhiteboardStream()->GetEvqFilename()));
            pOutput->Append(line); 
            
            line.Format(_T("        <obj>%s</obj>\n"), StringManipulation::MakeRelativeAndSgml(csLepFileName, element->GetWhiteboardStream()->GetObjFilename()));
            pOutput->Append(line); 
            
            if (element->GetMetadata())
            {
               line.Format(_T("        <lmd>%s</lmd>\n"), StringManipulation::MakeRelativeAndSgml(csLepFileName, element->GetMetadata()->GetFilename()));
               pOutput->Append(line); 
            }
            
            line = _T("      </source>\n");
            pOutput->Append(line); 
         }
      }
      line = _T("    </wb>\n");
      pOutput->Append(line);  
   }
   else
   {
      int begin = -1;
      int end   = -1;
      for (int i = 0; i < m_previewList.GetSize(); ++i)
      {
         CPreviewSegment *element = m_previewList[i];
         if (element && element->GetMetadata())
         {
            if (begin == -1 || element->GetTargetBegin() < begin)
               begin = element->GetTargetBegin();
            if (end == -1 || element->GetTargetEnd() > end)
               end = element->GetTargetEnd();
         }
      }
      
      if (begin == -1 || end == -1) 
         return;
      
      CString line;
      CString sgmlString;
      
      line.Format(_T("    <wb start=\"%d\" end=\"%d\">\n"), begin, end);
      pOutput->Append(line); 
      
      for (i = 0; i < m_previewList.GetSize(); ++i)
      {
         CPreviewSegment *element = m_previewList[i];
         if (element && element->GetMetadata())
         {
            line.Format(_T("      <source id=\"%d\" stream-id=\"%d\" stream-start=\"%d\" stream-end=\"%d\" source-start=\"%d\" source-end=\"%d\">\n"), 
               element->GetStreams()->GetID(), element->GetMetadata()->GetID(), 
               element->GetTargetBegin(), element->GetTargetEnd(),
               element->GetSourceBegin(), element->GetSourceEnd());
            pOutput->Append(line); 
            
            StringManipulation::TransformForSgml(element->GetMetadata()->GetFilename(), sgmlString);
            line.Format(_T("        <lmd>%s</lmd>\n"), sgmlString);
            pOutput->Append(line); 
            
            line = _T("      </source>\n");
            pOutput->Append(line); 
         }
      }
      line = _T("    </wb>\n");
      pOutput->Append(line);  
   }
}

void CEditorProject::WriteSlideStream(CFileOutput *pOutput, CString &recordPath, CString &prefix)
{
   if (m_previewList.GetSize() == 0)
      return;
   
   CString line;
   
   CString csLepFileName;
   csLepFileName.Format(_T("%s\\%s"), recordPath, prefix);
   
   CPreviewSegment *firstElement = m_previewList.GetAt(0);
   CPreviewSegment *lastElement = m_previewList.GetAt(m_previewList.GetSize()-1);
   int begin = 0;
   if (firstElement)
      begin = firstElement->GetTargetBegin();
   int end = 0;
   if (lastElement)
      end = lastElement->GetTargetEnd();
   
   line.Format(_T("    <slides start=\"%d\" end=\"%d\">\n"), begin, end);
   pOutput->Append(line); 
   
   for (int i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *pSegment = m_previewList[i];
      if (pSegment && pSegment->GetPageStream())
      {
         line.Format(_T("      <slide id=\"%d\" stream-id=\"%d\" stream-start=\"%d\" stream-end=\"%d\" source-start=\"%d\" source-end=\"%d\">\n"), 
            pSegment->GetStreams()->GetID(), pSegment->GetPageStream()->GetID(),
            pSegment->GetTargetBegin(), pSegment->GetTargetEnd(),
            pSegment->GetSourceBegin(), pSegment->GetSourceEnd());
         pOutput->Append(line); 
         
         pSegment->GetPageStream()->WriteToLep(pOutput, pSegment->GetTargetBegin(), pSegment->GetTargetEnd(),
            pSegment->GetSourceBegin(), pSegment->GetSourceEnd(), csLepFileName);
       
         line = _T("      </slide>\n");
         pOutput->Append(line); 
      }
   }

   line = _T("    </slides>\n");
   pOutput->Append(line); 
}

CPreviewSegment *CEditorProject::FindPreviewSegmentBeginsAt(int targetBegin)
{
   if (m_previewList.GetSize() == 0)
      return NULL;
   
   if (m_actualPreviewSegment >= m_previewList.GetSize())
      m_actualPreviewSegment = 0;
   
   bool goForward = true;
   if (targetBegin < m_previewList[m_actualPreviewSegment]->GetTargetBegin())
   {
      goForward = false;
   }
   
   if (goForward)
   {
      for (int i = m_actualPreviewSegment; i < m_previewList.GetSize(); ++i)
      {
         CPreviewSegment *segment = m_previewList[i];
         if (segment)
         {
            if (segment->GetTargetBegin() == targetBegin)
            {
               m_actualPreviewSegment = i;
               return segment;
            }
         }
      }
   }
   else
   {
      for (int i = m_actualPreviewSegment; i >= 0; --i)
      {
         CPreviewSegment *segment = m_previewList[i];
         if (segment)
         {
            if (segment->GetTargetBegin() == targetBegin)
            {
               m_actualPreviewSegment = i;
               return segment;
            }
         }
      }
   }
   return NULL;
}

CPreviewSegment *CEditorProject::FindPreviewSegment(int timestamp)
{
   if (m_previewList.GetSize() == 0)
      return NULL;
   
   if (m_actualPreviewSegment >= m_previewList.GetSize())
      m_actualPreviewSegment = 0;
   
   bool goForward = true;
   if (timestamp < m_previewList[m_actualPreviewSegment]->GetTargetBegin())
   {
      goForward = false;
   }
   
   if (goForward)
   {
      for (int i = m_actualPreviewSegment; i < m_previewList.GetSize(); ++i)
      {
         CPreviewSegment *segment = m_previewList[i];
         if (segment)
         {
            if (segment->GetTargetBegin() <= timestamp && segment->GetTargetEnd() > timestamp)
            {
               m_actualPreviewSegment = i;
               return segment;
            }
         }
      }
   }
   else
   {
      for (int i = m_actualPreviewSegment; i >= 0; --i)
      {
         CPreviewSegment *segment = m_previewList[i];
         if (segment)
         {
            if (segment->GetTargetBegin() <= timestamp && segment->GetTargetEnd() > timestamp)
            {
               m_actualPreviewSegment = i;
               return segment;
            }
         }
      }
   }
   return NULL;
}

CPreviewSegment *CEditorProject::FindSegmentWithStream(int timestamp, int streamId)
{
   if (m_previewList.GetSize() == 0)
      return NULL;
   
   for (int i = m_previewList.GetSize()-1; i >= 0; --i)
   {
      CPreviewSegment *segment = m_previewList[i];
      if (segment)
      {
         if (segment->GetTargetBegin() <= timestamp)
         {
            switch (streamId)
            {
            case STREAM_UNDEFINED:
               return segment;
            case STREAM_AUDIO:
               if (segment->GetAudioStream())
                  return segment;
               break;
            case STREAM_VIDEO:
               if (segment->GetVideoStream())
                  return segment;
               break;
            case STREAM_WHITEBOARD:
               if (segment->GetWhiteboardStream())
                  return segment;
               break;
            case STREAM_CLIPS:
               if (segment->GetClipStream())
                  return segment;
               break;
            case STREAM_METADATA:
               if (segment->GetMetadata())
                  return segment;
               break;
            }

            // there is no selected stream at this position
            return NULL;
         }
      }
   }
   
   return NULL;
}

CStreamContainer *CEditorProject::FindStreamContainer(int containerID)
{
   if (m_streamList.IsEmpty())
      return NULL;
   
   POSITION position = m_streamList.GetHeadPosition();
   while (position)
   {
      CStreamContainer *pContainer = m_streamList.GetNext(position);
      if (pContainer && pContainer->GetID() == containerID)
         return pContainer;
   }
   
   return NULL;
}

CAudioStream *CEditorProject::FindAudioStream(int audioStreamID)
{
   if (m_streamList.IsEmpty())
      return NULL;
   
   POSITION position = m_streamList.GetHeadPosition();
   while (position)
   {
      CStreamContainer *pContainer = m_streamList.GetNext(position);
      if (pContainer && pContainer->GetAudioStream() &&
         pContainer->GetAudioStream()->GetID() == audioStreamID)
         return pContainer->GetAudioStream();
   }
   
   return NULL;
}

CVideoStream *CEditorProject::FindVideoStream(int videoStreamID)
{
   if (m_streamList.IsEmpty())
      return NULL;
   
   POSITION position = m_streamList.GetHeadPosition();
   while (position)
   {
      CStreamContainer *pContainer = m_streamList.GetNext(position);
      if (pContainer && pContainer->GetVideoStream() &&
         pContainer->GetVideoStream()->GetID() == videoStreamID)
         return pContainer->GetVideoStream();
   }
   
   return NULL;
}

CClipStream *CEditorProject::FindClipStream(int clipStreamID)
{
   if (m_streamList.IsEmpty())
      return NULL;
   
   POSITION position = m_streamList.GetHeadPosition();
   while (position)
   {
      CStreamContainer *pContainer = m_streamList.GetNext(position);
      if (pContainer && pContainer->GetClipStream() &&
         pContainer->GetClipStream()->GetID() == clipStreamID)
         return pContainer->GetClipStream();
   }
   
   return NULL;
}

CWhiteboardStream *CEditorProject::FindWhiteboardStream(int whiteboardStreamID)
{
   if (m_streamList.IsEmpty())
      return NULL;
   
   POSITION position = m_streamList.GetHeadPosition();
   while (position)
   {
      CStreamContainer *pContainer = m_streamList.GetNext(position);
      if (pContainer && pContainer->GetWhiteboardStream() &&
         pContainer->GetWhiteboardStream()->GetID() == whiteboardStreamID)
         return pContainer->GetWhiteboardStream();
   }
   
   return NULL;
}

CPageStream *CEditorProject::FindPageStream(int pageStreamID)
{
   if (m_streamList.IsEmpty())
      return NULL;
   
   POSITION position = m_streamList.GetHeadPosition();
   while (position)
   {
      CStreamContainer *pContainer = m_streamList.GetNext(position);
      if (pContainer && pContainer->GetPageStream() &&
         pContainer->GetPageStream()->GetID() == pageStreamID)
         return pContainer->GetPageStream();
   }
   
   return NULL;
}

CMetadataStream *CEditorProject::FindMetadata(int metadataID)
{
   if (m_streamList.IsEmpty())
      return NULL;
   
   POSITION position = m_streamList.GetHeadPosition();
   while (position)
   {
      CStreamContainer *pContainer = m_streamList.GetNext(position);
      if (pContainer && pContainer->GetMetadata() &&
         pContainer->GetMetadata()->GetID() == metadataID)
         return pContainer->GetMetadata();
   }
   
   return NULL;
}

void CEditorProject::CreateDefaultProject()
{
   if (!m_streamList.IsEmpty())
   {
      CStreamContainer *streams = m_streamList.GetHead();
      CMetadataStream *metadata = streams->GetMetadata();
      if (metadata)
      {  
         CMetaInformation *metaInformation = metadata->GetMetaInformation();
         
         m_targetMetainfo.SetTitle(metaInformation->GetTitle());
         m_targetMetainfo.SetAuthor(metaInformation->GetAuthor());
         m_targetMetainfo.SetCreator(metaInformation->GetCreator());
         m_targetMetainfo.SetRecordDate(metaInformation->GetRecordDate());
         m_targetMetainfo.SetComments(metaInformation->GetComments());
         m_targetMetainfo.SetLength(metaInformation->GetLength());
         m_targetMetainfo.SetRecordTime(metaInformation->GetRecordTime());
         CStringList stringList;
         metaInformation->GetKeywords(stringList);
         if (!stringList.IsEmpty())
            m_targetMetainfo.SetKeywords(stringList);
         
      }
      CWhiteboardStream *whiteboardStream = streams->GetWhiteboardStream();
      if (whiteboardStream)
      {
         CString csWidth = whiteboardStream->GetPageDimension();
         int pos = csWidth.Find(_T('x'));
         csWidth.Delete(pos+1, (csWidth.GetLength() - (pos+1)));
         m_targetPageWidth = _ttoi(csWidth);
         
         CString csHeight = whiteboardStream->GetPageDimension();
         pos = csHeight.Find(_T('x'));
         csHeight.Delete(0, pos+1);
         m_targetPageHeight = _ttoi(csHeight);
      }
      
   }
}

void CEditorProject::StartInsertLrd(LPCTSTR tszFilename, int timestamp)
{
    bool bDoNormalInsert = true;


    CString csLrdPath = tszFilename;
    CString csLepPath = CEditorDoc::GetCorrespondingLep(csLrdPath);

    if (!csLepPath.IsEmpty()) {
        bDoNormalInsert = false;

        // BUGFIX 5354; not yet finished/solved/enabled

        CString csMessage;
        csMessage.LoadString(IDS_MSG_DOCUMENT_EDITED);
        CString csCaption;
        csCaption.LoadString(IDS_QUESTION);

        HWND wndParent = NULL;
        if (m_pDoc->GetMainFrame() != NULL)
            wndParent = m_pDoc->GetMainFrame()->GetSafeHwnd();
        int res = MessageBox(wndParent, csMessage, csCaption, 
            MB_ICONWARNING | MB_OK);


        
        // The below code is a big code change and has still problems with pages and their ids


        // Should in future:
        // use the existing/modified LEP for loading
        

        /*
        CEditorProject *pAnotherOne = new CEditorProject;

        bool bLoadSuccess = pAnotherOne->StartOpen(csLepPath);

        if (bLoadSuccess) {
            // TODO What about an error message? Where is it displayed?
            // UpdateStructureAndStreams() before interaction insertion??

            // Use the clipboard(s) for this insertion.

            int iLength = pAnotherOne->GetTotalLength();
            pAnotherOne->Copy(0, iLength, this->m_clipBoard, this->m_ClipboardNew, true);
            bool bPasteSuccess = this->Paste(timestamp, this->m_clipBoard, iLength);

            ClearStreamClipboard();
        }
        

        delete pAnotherOne;
        */
    }
    
    if (bDoNormalInsert) {
        // normal/old case: insert unmodified LRD directly
        m_pProgress = new CProgressDialogE(CMainFrameE::GetCurrentInstance(), this);

        InsertStruct insertStruct;
        insertStruct.pProject = this;
        insertStruct.tszFilename = tszFilename;
        insertStruct.timestamp = timestamp;

        CWinThread *thread = AfxBeginThread(InsertLrdThread, &insertStruct);

        m_pProgress->DoModal();

        delete m_pProgress;
        m_pProgress = NULL;
    }
}

UINT CEditorProject::InsertLrdThread(LPVOID pParam)
{
   HRESULT hr = CLanguageSupport::SetThreadLanguage();

   InsertStruct *pInsertStruct = (InsertStruct *)pParam;
   
   CEditorProject *pDoc = pInsertStruct->pProject;
   
   pDoc->InsertLrd(pInsertStruct->tszFilename, pInsertStruct->timestamp);

   if (pDoc->m_pProgress)
      PostMessage(pDoc->m_pProgress->GetSafeHwnd(), WM_USER, CProgressDialogE::END_DIALOG, NULL);
   
   return 0;
}

bool CEditorProject::InsertLrd(LPCTSTR tszFilename, int iTimestamp) {
    // TODO could/should use IProgressListener (=this) here.
    if (m_pProgress) {
        m_pProgress->SetPos(0);
        m_pProgress->SetLabel(IDS_PARSE_STREAMS);
    }

    HRESULT hrStreams = S_OK;

    bool bWasDenverDocument = !IsEmpty() && IsDenverDocument();
    bool bHadVideo = !IsEmpty() && HasVideo();
    bool bProjectEmpty = this->IsEmpty();

    CLrdFile *recording;
    recording = new CLrdFile();
    if (!recording->Open(tszFilename)) {
        HWND hWnd = NULL;
        if (m_pProgress)
            hWnd = m_pProgress->GetSafeHwnd();
        CString msg;
        msg.Format(IDS_MSG_INSERT_LRD_FAILED, tszFilename);
        CEditorDoc::ShowErrorMessage(msg, hWnd);
        delete recording;
        return false;
    }

    if (m_pProgress)
        m_pProgress->SetPos(10);

    // create audio stream with information from lrd-file
    CAudioStream *audioStream = NULL;
    CString csAudioFileName = recording->GetAudioFilename();
    if (!csAudioFileName.IsEmpty()) {
        audioStream = new CAudioStream();
        if (!InitAudioStream(audioStream, csAudioFileName, 0, IDS_MSG_OPEN_AUDIO_FAILED, m_pProgress)) {
            delete audioStream;
            delete recording;
            return false;
        }

        int audioOffset = recording->GetAudioOffset();
        audioStream->SetOffset(audioOffset);
        audioStream->SetID(++CStreamContainer::m_iAudioStreamCount);
    } else {
        HWND hWnd = NULL;
        if (m_pProgress)
            hWnd = m_pProgress->GetSafeHwnd();
        CString msg;
        msg.Format(IDS_MSG_LRD_NO_AUDIO_ENTRY, tszFilename);
        CEditorDoc::ShowErrorMessage(msg, hWnd);
        delete recording;
        return false;
    }

    if (m_recordingList.IsEmpty())
        m_bShowScreenGrabbingAsRecording = !recording->IsSimulationDocument();

    m_recordingList.AddTail(recording);

    CStreamContainer *streams = new CStreamContainer();
    streams->SetID(++CStreamContainer::m_nContainerCounter);
    m_streamList.AddTail(streams);

    streams->SetAudioStream(audioStream);

    if (m_pProgress)
        m_pProgress->SetPos(30);

    // create video stream with information from lrd-file
    CVideoStream *videoStream = recording->CreateVideoStream();
    if (videoStream) {
        if (_taccess(videoStream->GetFilename(), 00) == -1) {
            HWND hWnd = NULL;
            if (m_pProgress)
                hWnd = m_pProgress->GetSafeHwnd();
            CString msg;
            msg.Format(IDS_MSG_FILE_NOT_FOUND, videoStream->GetFilename());
            CEditorDoc::ShowWarningMessage(msg, hWnd);

            delete videoStream;
            videoStream = NULL;
        } else {
            int videoOffset = recording->GetVideoOffset();
            videoStream->SetOffset(videoOffset);
            videoStream->SetID(++CStreamContainer::m_iVideoStreamCount);

            // tripple code:
            int videoLength;
            if (!VideoInformation::GetVideoDuration(videoStream->GetFilename(), videoLength)) {
                HWND hWnd = NULL;
                if (m_pProgress)
                    hWnd = m_pProgress->GetSafeHwnd();
                CString msg;
                msg.Format(IDS_MSG_GET_VIDEO_LENGTH_FAILED, videoStream->GetFilename());
                CEditorDoc::ShowWarningMessage(msg, hWnd);
                delete videoStream;
                videoStream = NULL;
            } else {
                videoStream->SetLength(videoLength);
            }
        }

        streams->SetVideoStream(videoStream);
    }

    if (m_pProgress)
        m_pProgress->SetPos(50);

    // create clip stream with information from lrd-file
    CClipStream *clipStream = recording->CreateClipStream();
    if (clipStream) {
        clipStream->SetID(++CStreamContainer::m_iClipStreamCount);
        streams->SetClipStream(clipStream);

    }

    if (m_pProgress)
        m_pProgress->SetPos(60);

    // create whiteboard stream with information from lrd-file
    CWhiteboardStream *whiteboardStream = recording->CreateWhiteboardStream();
    bool bNewContainsQuestions = false;
    CPageStream *pageStream = NULL;
    // open eventqueue and object file
    if (whiteboardStream) {
        bNewContainsQuestions = whiteboardStream->ContainsQuestions();

        whiteboardStream->SetID(++CStreamContainer::m_iWhiteboardStreamCount);
        streams->SetWhiteboardStream(whiteboardStream);

        pageStream = whiteboardStream->CreatePageStreamFromEvents();
        if (pageStream) {
            pageStream->SetID(whiteboardStream->GetID());
            if (CStreamContainer::m_iPageStreamCount < pageStream->GetID())
                CStreamContainer::m_iPageStreamCount = pageStream->GetID();
            streams->SetPageStream(pageStream);
        }

        if (whiteboardStream->ContainsMarks() || GetMarkStream(false) != NULL) {
            if (m_pProgress)
                m_pProgress->SetPos(65);


            // TODO proper error handling
            HRESULT hr = 
                GetMarkStream(true)->InsertMarks(iTimestamp, audioStream->GetLength(), 
                whiteboardStream, !bProjectEmpty);

            _ASSERT(SUCCEEDED(hr));

            if (FAILED(hr)) {
                HWND hWnd = NULL;
                if (m_pProgress)
                    hWnd = m_pProgress->GetSafeHwnd();
                CString msg;
                msg.Format(IDS_MSG_STREAMS_ERROR);
                CEditorDoc::ShowWarningMessage(msg, hWnd);
            }
        }
        whiteboardStream->GetUnsupportedFonts(m_acsUnsupportedFonts);
        whiteboardStream->ClearUnsupportedFonts();
    } else {
        // it is a Denver document: make new streams longer; lengthen questions or areas

        if (GetMarkStream(false) != NULL) {
            HRESULT hr = GetMarkStream(false)->InsertMarks(iTimestamp, audioStream->GetLength(), NULL, true);
        }
    }

    if (m_csStillImage.IsEmpty()) {
        m_csStillImage = recording->GetStillImageFilename();
    }

    if (m_pProgress) {
        m_pProgress->SetPos(70);
        m_pProgress->SetLabel(IDS_INIT_STREAMS);
    }

    // create metadata and fill with information from lrd-file
    CMetadataStream *metadata = recording->CreateMetadata();
    // append metadata information to streams
    if (metadata) {
        metadata->Complete(audioStream, videoStream, clipStream, whiteboardStream, pageStream);
        metadata->SetID(++CStreamContainer::m_iMetadataCount);
        streams->SetMetadata(metadata);
        if (metadata->GetFileInformation())
            SetShowClickPages(metadata->GetFileInformation()->ShowClickPages());

        if (clipStream != NULL || GetClipStream(false) != NULL) {
            // Only at this (late) point all information for the clips
            // (i.e. the title) is known.

            HRESULT hr = S_OK;

            if (clipStream != NULL) {
                hr = GetClipStream(true)->InsertClips(iTimestamp, audioStream->GetLength(), 
                    clipStream, !bProjectEmpty);
            } else {
                // still move or split existing elements
                hr = GetClipStream(false)->InsertClips(iTimestamp, audioStream->GetLength(), NULL, true);
            }

            _ASSERT(SUCCEEDED(hr));

            if (FAILED(hr)) {
                HWND hWnd = NULL;
                if (m_pProgress)
                    hWnd = m_pProgress->GetSafeHwnd();
                CString msg;
                msg.Format(IDS_MSG_STREAMS_ERROR);
                CEditorDoc::ShowWarningMessage(msg, hWnd);
            }
        }

    }

    if (m_pProgress)
        m_pProgress->SetPos(80);

    bool bDoRollback = false;
    if (audioStream) {
        if (m_previewList.GetSize() == 0) {
            CPreviewSegment *newElement = new CPreviewSegment();
            newElement->SetSourceBegin(0);
            newElement->SetSourceEnd(audioStream->GetLength());
            newElement->SetTargetBegin(0);
            newElement->SetTargetEnd(audioStream->GetLength());

            newElement->SetStreams(streams);

            m_previewList.InsertAt(0, newElement);
        } else {
            // code similar to Paste
            int insertIndex = FindElementAt(iTimestamp);

            if (insertIndex >= 0) {
                CUnRedoElement *undo = new CUnRedoElement();
                undo->SetType(CUnRedoElement::CUT_PASTE);
                // copy preview list to undo element
                undo->Append(m_previewList, 0, m_previewList.GetSize(), true);

                CPreviewSegment *newElement = m_previewList[insertIndex]->Split(iTimestamp);

                int insertionCount = 1;
                int insertionOffset = 0;

                if (newElement || m_previewList[insertIndex]->GetTargetBegin() < iTimestamp)
                    insertionOffset = 1;

                CPreviewSegment *insertElement = new CPreviewSegment();
                insertElement->SetSourceBegin(0);
                insertElement->SetSourceEnd(audioStream->GetLength());
                insertElement->SetTargetBegin(iTimestamp);
                insertElement->SetTargetEnd(iTimestamp + audioStream->GetLength());

                insertElement->SetStreams(streams);

                m_previewList.InsertAt(insertIndex + insertionOffset, insertElement);
                ++insertionCount;
                ++insertionOffset;

                if (newElement) {
                    newElement->IncrementTargetBegin(audioStream->GetLength());
                    newElement->IncrementTargetEnd(audioStream->GetLength()),
                        m_previewList.InsertAt(insertIndex + insertionOffset, newElement);
                    ++insertionCount;
                    ++insertionOffset;
                }

                // remove elements from redo list
                EmptyRedoList();
                // append new Element to undo list
                m_undoList.AddTail(undo);


                HRESULT hr = m_UndoManager.RegisterAction(this, UNDO_LEGACY_CHANGE, NULL, 0, HINT_CHANGE_STRUCTURE);

                bDoRollback = true;

                IncrementFrom(insertIndex + insertionOffset, audioStream->GetLength());
            }
        }
    } 


    if (whiteboardStream) {
        if (whiteboardStream->ContainsInteractions() || bNewContainsQuestions
            || GetInteractionStream(false) != NULL) {     
                // must be done before interaction interpretation
                UpdateStructureAndStreams();
                // will be done again after this by CEditorDoc::InsertLrd
        }

        bool bResolveTimes = 
            whiteboardStream->ContainsInteractions() || bNewContainsQuestions;
        // will be removed lateron; but this knowledge is needed even later

        if (whiteboardStream->ContainsInteractions() || GetInteractionStream(false) != NULL) {
            bDoRollback = true;

            if (m_pProgress)
                m_pProgress->SetPos(85);

            HRESULT hr = GetInteractionStream(true)->Insert(iTimestamp, audioStream->GetLength(), 
                whiteboardStream);

            // TODO: properly handle format and other errors:
            // the interaction stream should maybe be in a valid state although
            // there was an error.
            // Do this also for mark stream and maybe clip stream.


            _ASSERT(SUCCEEDED(hr));

            if (FAILED(hr)) {
                HWND hWnd = NULL;
                if (m_pProgress)
                    hWnd = m_pProgress->GetSafeHwnd();
                CString msg;
                msg.Format(IDS_MSG_STREAMS_ERROR);
                CEditorDoc::ShowWarningMessage(msg, hWnd);
            }
        }

        if (bNewContainsQuestions || GetQuestionStream(false) != NULL) {
            if (m_pProgress)
                m_pProgress->SetPos(90);

            bool bOldContainsQuestions = GetQuestionStream(false) != NULL;

            HRESULT hr = GetQuestionStream(true)->Insert(
                iTimestamp, audioStream->GetLength(), whiteboardStream);

            // TODO: properly handle format and other errors:
            // the interaction stream should maybe be in a valid state although
            // there was an error.
            // Do this also for mark stream and maybe clip stream.

            _ASSERT(SUCCEEDED(hr));


            if (FAILED(hr)) {
                HWND hWnd = NULL;
                if (m_pProgress)
                    hWnd = m_pProgress->GetSafeHwnd();
                CString msg;
                msg.Format(IDS_MSG_STREAMS_ERROR);
                CEditorDoc::ShowWarningMessage(msg, hWnd);
            }

            if (SUCCEEDED(hr) && bNewContainsQuestions && bOldContainsQuestions)
                hr = UpdateQuestionnaireFeedback();
        }

        if (bResolveTimes) {
            // TODO proper error handling (also above).

            HRESULT hr = GetInteractionStream()->ResolveTimes(iTimestamp);
            if (SUCCEEDED(hr) && GetQuestionStream(false) != NULL)
                hr = GetQuestionStream(false)->ResolveJumpTimes(iTimestamp);

            _ASSERT(SUCCEEDED(hr));
        }

    } else {
        // it is a Denver document: make new streams longer; lengthen questions or areas

        if (GetInteractionStream(false) != NULL) {
            HRESULT hr = GetInteractionStream(false)->InsertDenver(iTimestamp, audioStream->GetLength());
        }

        if (GetQuestionStream(false) != NULL) {
            HRESULT hr = GetQuestionStream(false)->InsertDenver(iTimestamp, audioStream->GetLength());
        }
    }


    if (bDoRollback) 
        HRESULT hr = m_UndoManager.SetRollbackPoint(bProjectEmpty);

    if (IsDenverDocument() && !m_csStillImage.IsEmpty())
        m_csStillImage.Empty();

    if (IsDenverDocument())
        RecalcDenverVideoParams();
    else {
        // It's not a Denver document anymore
        if (bWasDenverDocument) {
            // But it was one
            // Reset all preview/video formats
            ResetPreviewFormat();
        }

        if (!bHadVideo && HasVideo()) {
            // Reset video format; this forces the Editor
            // to read the video format from the video file
            // (using InitMediaTypes()) when it is needed
            // (at preview, from InitAudioVideo).
            ResetPreviewFormat();
        }
    }

    //TRACE("insert lrd length %d\n", GetTotalLength());

    // if all successfully loaded and there is no m_szFilename, set this
    if (m_szFilename.IsEmpty()) {
        m_szFilename = tszFilename;
        StringManipulation::GetPathFilePrefix(m_szFilename);
        m_szFilename += _T(".lep");
    }


    if (m_pProgress)
        m_pProgress->SetPos(100);

    return true;
}

HRESULT CEditorProject::CalculateWhiteboardArea(CRect *prcView, CRect *prcTarget, double *pdZoomFactor, int iMenuZoom)
{
   if (prcView == NULL || prcTarget == NULL || pdZoomFactor == NULL)
      return E_POINTER;

   // client area minus 10 pixel on each side
   CRect rcInnerRect(prcView);
   bool bIsSimulation = (IsDenverDocument() || HasStructuredSgClips()) && !ShowScreenGrabbingAsRecording();
   if (!bIsSimulation)
       rcInnerRect.DeflateRect(10, 10, 10, 10);

   double dZoomFactorX = (double)rcInnerRect.Width() / m_targetPageWidth;
   double dZoomFactorY = (double)rcInnerRect.Height() / m_targetPageHeight;
   double dZoomFactor = 0.0;
   if (dZoomFactorX > dZoomFactorY)
       dZoomFactor = dZoomFactorY;
   else
       dZoomFactor = dZoomFactorX;

   if (bIsSimulation) {
       // if the current zoom factor is 0.5, 1.0 or 2.0 and the view is large enough to 
       // display the content with this zoom factor use it.
       if (iMenuZoom == 50 || iMenuZoom == 100 || iMenuZoom == 200) {
           double dMenuZoom = (double)iMenuZoom / 100.0;
           if (dZoomFactor > dMenuZoom)
               dZoomFactor = dMenuZoom;
       }
   }
   
   // calculate drawing rectangle
   prcTarget->left = rcInnerRect.left + (int)((double)rcInnerRect.Width() - (m_targetPageWidth * dZoomFactor)) / 2;
   prcTarget->top = rcInnerRect.top + (int)((double)rcInnerRect.Height()- (m_targetPageHeight * dZoomFactor)) / 2;
   prcTarget->right = prcTarget->left + (int)(m_targetPageWidth * dZoomFactor);
   prcTarget->bottom = prcTarget->top + (int)(m_targetPageHeight * dZoomFactor);

   int iZoomPercent = dZoomFactor * 100;
   double dZoom = iZoomPercent / 100.0;
   *pdZoomFactor = dZoom;//dZoomFactor;

   return S_OK;
}
   
HRESULT CEditorProject::GetPageDimension(CSize &siPage)
{
   siPage.cx = m_targetPageWidth;
   siPage.cy = m_targetPageHeight;

   return S_OK;
}

bool CEditorProject::DrawWhiteboard(CDC *pDC, int timestamp, CRect &rcPage, double dZoomFactor, bool bForce)
{
   if (m_previewList.GetSize() == 0)
      return false;
   
   CPreviewSegment *part = FindSegmentWithStream(timestamp, STREAM_WHITEBOARD);

   int iSourceTimestamp = 0;
   if (part == NULL)    // Draw last event from previews segment
   { 
      int nCurrentSegment = FindElementAt(timestamp);
      if (m_previewList[nCurrentSegment]->GetTargetBegin() <= timestamp)
      {
         int nSegmentWithWhiteboard = nCurrentSegment - 1;
         while (nSegmentWithWhiteboard >= 0)
         {
            if (m_previewList[nSegmentWithWhiteboard]->GetWhiteboardStream())
               break;
            else
               --nSegmentWithWhiteboard;
         }
         if (nSegmentWithWhiteboard >= 0)
            part = m_previewList[nSegmentWithWhiteboard];
      }
      if (part)
         iSourceTimestamp = part->GetSourceEnd()-1;
   }
   else
   {
      int offset = part->GetTargetBegin() - part->GetSourceBegin();
      iSourceTimestamp = timestamp - offset;
   }
   
   if (part && part->GetWhiteboardStream())
   {
      bool forceRedraw = bForce;
      
      if (m_pLastDrawnWhiteboard != part->GetWhiteboardStream())
      {
         forceRedraw = true;
      }
      
      m_pLastDrawnWhiteboard = part->GetWhiteboardStream();
     
      // draw whiteboard data
      DrawSdk::ZoomManager *zoom = new DrawSdk::ZoomManager(dZoomFactor);
      // The Draw() method returns true if the whiteboard was really
      // redrawn, otherwise false
      if (!ShowClickPages() && m_pTargetStructureWithClickPages != NULL) {
          //Recalculate(iSourceTimestamp);
          bool bReturn = m_pTargetStructureWithClickPages->GetLastNotClickTimestamp(iSourceTimestamp);
          if (!bReturn)
              m_pTargetStructureWithClickPages->FindFirstNotClickTimestamp(iSourceTimestamp);
      }

      bool bHidePointers = false;
      CPage *pPage = FindPageAt(timestamp);
      if (pPage != NULL && pPage->IsHidePointers())
          bHidePointers = true;

      part->GetWhiteboardStream()->Draw(pDC, iSourceTimestamp, rcPage, zoom, forceRedraw, false, bHidePointers);

      // Delete the ZoomManager
      delete zoom;

      return true;
   }
   
   return false;
}

int CEditorProject::GetThumbHeight()
{
   double zoomFactor = (double)GetStructureWidth() / m_targetPageWidth;
   int pageHeight = (int)((double)m_targetPageHeight * zoomFactor);
   
   return pageHeight;
}

int CEditorProject::GetStructureHeight()
{
   int pageHeight = GetThumbHeight();
   int retHeight = m_targetStructure.GetHeight(5, pageHeight);
   
   return retHeight;
}

int CEditorProject::GetActualStructurePos(int timestamp, int nPageHeight)
{
   int pageHeight = GetThumbHeight();
   if(nPageHeight != -1)
      pageHeight = 2;
   
   int iYOffset = 5;
   
   int y = iYOffset;
   bool success  = m_targetStructure.GetYPos(y, pageHeight, timestamp);
   
   // timestamp is out of the structure.
   if (!success && timestamp > 0) // if timestamp is beyond the structure
      y = m_targetStructure.GetHeight(iYOffset, pageHeight);
   
   return y;
}

int CEditorProject::GetTimestamp(int y, int nHeight)
{
   double zoomFactor = (double)GetStructureWidth() / m_targetPageWidth;
   int pageHeight = (int)((double)m_targetPageHeight * zoomFactor);
   if(nHeight != -1)
      pageHeight = 2;
   
   int actPos = 5;
   int timestamp  = m_targetStructure.GetTimestamp(y, actPos, pageHeight);
   
   if (timestamp == -1)
   {
      if (y >= actPos)
      {
         timestamp = m_pDoc->m_docLengthMs;
      }
      else
      {
         timestamp = 0;
      }
   }
   
   return timestamp;
}

HRESULT CEditorProject::GetStructureElements(CArray<CStructureInformation *, CStructureInformation *> *paElements)
{
   if (m_previewList.GetSize() == 0 || m_targetStructure.IsEmpty())
      return S_FALSE;

   m_targetStructure.GetAllElements(paElements);

   return S_OK;
}

CStructureInformation *CEditorProject::GetStructureInformationAtTimeStamp(int iPageBegin) {
    return m_targetStructure.FindElement(iPageBegin, CStructureInformation::PAGE);
}

int CEditorProject::GetActivePage(int timestamp)
{
   int activePage = m_targetStructure.GetActivePage(timestamp);
   
   if (activePage == -1)
   {
      
      if (timestamp <= m_targetStructure.GetBegin())
      {
         return 0;
      }
      else
      {
         CPageInformation *lastPage = m_targetStructure.FindLastPage();
         if (lastPage)
            return lastPage->GetInternalNumber();
         else
            return -1;
      }
   }
   
   
   return activePage;
}

void CEditorProject::UpdatePageNumbers()
{
   int number = 0;

   CMap<int, int, int, int> mapUniquePageIds;
   mapUniquePageIds.InitHashTable(101);
   
   m_targetStructure.UpdatePageNumbers(number, mapUniquePageIds);
   
}

void CEditorProject::CalculateSlideSize()
{
   int nSize = m_previewList.GetSize();
   if (nSize > 0)
   {
      int nMaxWidth = 0;
      int nMaxHeight = 0;
      
      for (int i=0; i<nSize; ++i)
      {
         CPreviewSegment *pSegment = m_previewList[i];
         if (pSegment)
         {
            CWhiteboardStream *pWb = pSegment->GetWhiteboardStream();
            if (pWb)
            {
               CString csWidth = pWb->GetPageDimension();
               int pos = csWidth.Find('x');
               csWidth.Delete(pos+1, (csWidth.GetLength() - (pos+1)));
               int nWidth = _ttoi(csWidth);
               
               CString csHeight = pWb->GetPageDimension();
               pos = csHeight.Find('x');
               csHeight.Delete(0, pos+1);
               int nHeight = _ttoi(csHeight);
               
               if (nWidth > nMaxWidth)
                  nMaxWidth = nWidth;
               if (nHeight > nMaxHeight)
                  nMaxHeight = nHeight;
            }
         }
      }
      
      if (nMaxWidth > 0 && nMaxHeight > 0)
      {
         m_targetPageWidth = nMaxWidth;
         m_targetPageHeight = nMaxHeight;
      }
   }
}

void CEditorProject::GetPages(CArray<CPage *, CPage *> &pages, int fromMsec, int toMsec)
{
   if (m_aPageStream.GetSize() == 0)
      return;
   
   for (int i = 0; i < m_aPageStream.GetSize(); ++i)
   {
      CPage *page = m_aPageStream[i];
      if (page &&
         (page->GetEnd() > fromMsec && page->GetEnd() <= toMsec) ||
         (page->GetBegin() >= fromMsec && page->GetBegin() < toMsec) ||
         (page->GetBegin() < fromMsec && page->GetEnd() > toMsec))
      {
         pages.Add(page);
      }
   }
   
   return;
}

CString& CEditorProject::GetPageTitleForId(UINT nJumpId)
{
   static CString csReturn;
   
   for (int i=0; i<m_aPageStream.GetSize(); ++i)
   {
      if ((unsigned)m_aPageStream[i]->GetJumpId() == nJumpId)
      {
         csReturn = m_aPageStream[i]->GetTitle();
         break;
      }
   }
   
   return csReturn;
}


CPage *CEditorProject::FindPageAt(int timestamp)
{
   if (m_aPageStream.GetSize() == 0)
      return NULL;
   
   CPage *pReturnPage = NULL;
      
   for (int i = 0; i < m_aPageStream.GetSize(); ++i)
   {
      CPage *pPage = m_aPageStream[i];
      if (pPage->GetBegin() <= timestamp && pPage->GetEnd() > timestamp)
      {
         pReturnPage = pPage;
         break;
      }
   }

   if (pReturnPage == NULL)
   {
      if (timestamp < 0)
         pReturnPage = m_aPageStream[0];
      else if (timestamp >= m_aPageStream[m_aPageStream.GetSize() - 1]->GetEnd())
         pReturnPage = m_aPageStream[m_aPageStream.GetSize() - 1];
   }
   
   return pReturnPage;
}

HRESULT CEditorProject::GetPagePeriod(int timestamp, UINT &iStartMs, UINT &iEndMs)
{
   CPage *pPage = FindPageAt(timestamp);
   if (pPage != NULL)
   {
      iStartMs = pPage->GetBegin();
      iEndMs = pPage->GetEnd() - 1;

      return S_OK;
   }
   else
      return E_UNEXPECTED;
}

void CEditorProject::ChangeClipTitleAt(int nTimeMs)
{
   CClipStreamR *pClipStream = GetClipStream();
   if (pClipStream != NULL)
   {
      CClipInfo clipInfo;
      HRESULT hr = pClipStream->FillClipInfoAt(nTimeMs, &clipInfo);

      bool bSomethingChanged = false;
      if (SUCCEEDED(hr) && hr == S_OK)
      {
         CPageProperties pageProperties(&clipInfo, IDS_PAGE_PROPERTIES);
         
         int nResult = pageProperties.DoModal();
         if (nResult != IDCANCEL)
         {
            if (!IsDenverDocument())
               hr = pClipStream->ChangeClipInfoAt(nTimeMs, &clipInfo);
            else
            {
               hr = pClipStream->ChangeAllClipInfo(&clipInfo);

               // TODO if it is a denver document all segments get the same
               // title and keywords.
               // However this fact is not visualized properly in the user interface.
            }

            bSomethingChanged = true;
         }
      }

      _ASSERT(SUCCEEDED(hr));
      
      if (bSomethingChanged)
      {
         // TODO this needed? -> yes clip title is displayed in structure view
         UpdateStructureAndStreams();
         
         m_pDoc->UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE); // TODO small change: big update!
         m_pDoc->SetModifiedFlag(TRUE);
         
      }
   }

}

void CEditorProject::ChangePageTitleAt(int nTimeMs)
{
   CPage *pPage = FindPageAt(nTimeMs);

   if (pPage == NULL)
      return;

   int nPageStartMs = pPage->GetBegin();
   int nPageEndMs = pPage->GetEnd();


   CArray<CPreviewSegment *, CPreviewSegment *> arPreviewSegments;
   m_pDoc->project.GetPreviewSegments(arPreviewSegments, nPageStartMs, nPageEndMs);
   
   bool bIsFirstPage = true;
   CString csTitle;
   CStringList slKeywords;
   // do not change join or other ids anymore: there shouldn't be two indepenedent pages with same ids anymore:
   //int iNewUniqueId;
   //int iNewPageId;
   for (int i = 0; i < arPreviewSegments.GetSize(); ++i)
   {
      CPreviewSegment *pSegment = arPreviewSegments[i];
      if (pSegment)
      {
         CMetadataStream *pMetadata = pSegment->GetMetadata();
         CStructureInformation *pStructure = pMetadata->GetStructureInformation();
         int iSourceTimestamp = pSegment->TargetTimestampToSource(nPageStartMs);
         if (iSourceTimestamp < pSegment->GetSourceBegin())
            iSourceTimestamp = pSegment->GetSourceBegin();

         CPageInformation *pPageInfo = 
            (CPageInformation *) pStructure->FindElement(iSourceTimestamp, CStructureInformation::PAGE);
         if (pPageInfo)
         {
            if (bIsFirstPage)
            {
               CPageProperties pageProperties(pPageInfo, IDS_PAGE_PROPERTIES);
               int nResult = pageProperties.DoModal();
               if (nResult == IDCANCEL)
                 return;

               csTitle = pPageInfo->GetTitle();
               pPageInfo->GetKeywords(slKeywords);
               //iNewUniqueId = CStructureInformation::m_iIdCounter;
               //++CStructureInformation::m_iIdCounter;
               //++CWhiteboardStream::m_pageCounter;
               //iNewPageId = CWhiteboardStream::m_pageCounter;
               bIsFirstPage = false;
            }
            else
            {
               pPageInfo->SetTitle(csTitle);
               pPageInfo->SetKeywords(slKeywords);
            }
            //pPageInfo->SetUniqueId(iNewUniqueId);
            pStructure->Complete(pSegment->GetClipStream(), pSegment->GetWhiteboardStream(), pSegment->GetPageStream());
            /*
            CPage *pPage = pSegment->GetPageStream()->FindPageWithNumber(pPageInfo->GetNumber());
            if (pPage)
            {
               pPage->SetJoinId(iNewPageId);
               pPage->SetPageNumber(iNewPageId);
               pPageInfo->SetNumber(iNewPageId);
            }
            */
         }
      }
   }
   arPreviewSegments.RemoveAll();

   slKeywords.RemoveAll();
   UpdateStructureAndStreams();
   m_pDoc->UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE); // TODO: small change, big update!
   m_pDoc->SetModifiedFlag(TRUE);
}

int CEditorProject::GetPrevPageBegin(int curMsec, bool bReallyPrevious)
{
   if (m_previewList.GetSize() == 0)
      return 0;
   
   if (m_aPageStream.GetSize() > 0)
   {
      for (int i=0; i<m_aPageStream.GetSize(); ++i)
      {
         // find the page segment containing curMsec

         CPage *pPage = m_aPageStream[i];
         
         if (curMsec < pPage->GetEnd())
         {
            // current page found
            if ((pPage->GetBegin() == curMsec || bReallyPrevious) && i > 0)
               pPage = m_aPageStream[i-1];

            return pPage->GetBegin();
         }
      }

      // curMsec after all pages: use last page
      return m_aPageStream[m_aPageStream.GetSize() - 1]->GetBegin();
   }
   
   return 0;
}

int CEditorProject::GetNextPageBegin(int curMsec)
{
   if (m_previewList.GetSize() == 0)
      return 0;
   
   for (int i=0; i<m_aPageStream.GetSize(); ++i)
   {
      CPage *pPage = m_aPageStream[i];

      if (pPage->GetBegin() > curMsec)
         return pPage->GetBegin();
   }

   return m_pDoc->m_docLengthMs;
}

CStopJumpMark* CEditorProject::GetMarkWithId(UINT nMarkId)
{
   CMarkStream *pMarkStream = GetMarkStream();
   if (pMarkStream != NULL)
   {
      return pMarkStream->GetMarkForId(nMarkId);
   }

   return NULL; // id not defined
}

CStopJumpMark* CEditorProject::GetJumpMarkAt(UINT nPositionMs)
{
   CMarkStream *pMarkStream = GetMarkStream();
   if (pMarkStream != NULL)
      return pMarkStream->GetFirstJumpMarkInRange(nPositionMs, 1);

   return NULL;
}

CStopJumpMark* CEditorProject::GetStopMarkAt(UINT nPositionMs)
{
   CMarkStream *pMarkStream = GetMarkStream();
   if (pMarkStream != NULL)
      return pMarkStream->GetFirstStopMarkInRange(nPositionMs, 1);

   return NULL;
}
   

UINT CEditorProject::GetTimeForMarkId(UINT nMarkId)
{
   CMarkStream *pMarkStream = GetMarkStream();
   if (pMarkStream != NULL)
   {
      return pMarkStream->GetTimeForId(nMarkId);
   }

   return 0xffffffff; // value for "mark not found"; but shouldn't happen
}


UINT CEditorProject::GetTimeForPageId(UINT nPageId)
{
   if (m_aPageStream.GetSize() > 0)
   {
      for (int i=0; i<m_aPageStream.GetSize(); ++i)
      {
         if (m_aPageStream[i]->GetJumpId() == (signed)nPageId) // page with this id
            return m_aPageStream[i]->GetBegin();
      }
   }

   return 0xffffffff; // page not found
}

void CEditorProject::Cut(int from, int to, bool bCutFromEditor)
{
   if (m_previewList.GetSize() == 0)
      return;
   
   Copy(from, to, m_clipBoard, bCutFromEditor);
   
   Delete(from, to);
}

void CEditorProject::Copy(int from, int to, CArray<CPreviewSegment *, CPreviewSegment *> &clipBoard, bool bCopyFromEditor)
{
   HRESULT hr = S_OK;
   for (int s=0; s<m_aStreams.GetSize() && SUCCEEDED(hr); ++s)
      hr = m_aStreams[s]->Copy(from, to-from, &m_ClipboardNew);
   _ASSERT(SUCCEEDED(hr));

   if (m_previewList.GetSize() == 0)
      return;
   
   int fromIndex = FindElementAt(from);
   int toIndex   = FindElementAt(to);
   
   DeleteClipBoard(clipBoard);
   
   if (fromIndex == toIndex)
   {
      // copy part to clipBoard
      CPreviewSegment *part = m_previewList[fromIndex]->Copy(bCopyFromEditor);
      part->IncrementSourceBegin(from - part->GetTargetBegin());
      part->DecrementSourceEnd(part->GetTargetEnd() - to);
      part->SetTargetBegin(from);
      part->SetTargetEnd(to);
      if (part->GetSourceBegin() != part->GetSourceEnd())
         clipBoard.Add(part);
      else
         delete part;
   }
   else
   {
      // copy first part to clipBoard
      CPreviewSegment *part = m_previewList[fromIndex]->Copy(bCopyFromEditor);
      // update begin of part
      part->IncrementSourceBegin(from - part->GetTargetBegin());
      part->SetTargetBegin(from);
      
      if (part->GetSourceBegin() != part->GetSourceEnd())
         clipBoard.Add(part);
      else
         delete part;
      
      int elementsToCopy = toIndex - (fromIndex + 1);
      
      for (int i = 0; i < elementsToCopy; ++i)
      {
         // copy parts to clipboard
         part = m_previewList.GetAt(fromIndex + i + 1)->Copy(bCopyFromEditor);
         if (part->GetSourceBegin() != part->GetSourceEnd())
            clipBoard.Add(part);    
         else
            delete part;
      }
      
      // copy last part to clipBoard
      part = m_previewList[toIndex]->Copy(bCopyFromEditor);
      // update end of part
      part->DecrementSourceEnd(part->GetTargetEnd() - to);
      part->SetTargetEnd(to);
      
      if (part->GetSourceBegin() != part->GetSourceEnd())
         clipBoard.Add(part);    
      else
         delete part;
   }
   
}


// helper method
CStreamContainer *CreateNewContainer(CPreviewSegment *pSource)
{
   if (pSource == NULL)
      return NULL;

   CStreamContainer *pContainer = new CStreamContainer();
   pContainer->SetID(++CStreamContainer::m_nContainerCounter);
   pContainer->SetAudioStream(pSource->GetAudioStream());
   pContainer->SetVideoStream(pSource->GetVideoStream());
   pContainer->SetWhiteboardStream(pSource->GetWhiteboardStream());
   
   if (pSource->GetPageStream() != NULL)
   {
      CPageStream *pPageStream = pSource->GetPageStream()->Copy();
      pPageStream->SetID(++CStreamContainer::m_iPageStreamCount);
      pContainer->SetPageStream(pPageStream);
   }
   
   if (pSource->GetClipStream() != NULL)
   {
      CClipStream *pClipStream = pSource->GetClipStream()->Copy();
      pClipStream->SetID(++CStreamContainer::m_iClipStreamCount);
      pContainer->SetClipStream(pClipStream);
   }
   
   if (pSource->GetMetadata() != NULL)
   {
      CMetadataStream *pMetadata = pSource->GetMetadata()->Copy();
      pMetadata->SetID(++CStreamContainer::m_iMetadataCount);
      pContainer->SetMetadata(pMetadata);

      //pMetadata->GetStructureInformation()->ChangeUniqueIds();
      // new segments will get a new unique id if 
      // they do not join with their new neighbors (in UpdateStructureAndStreams())
   }

   return pContainer;
}


bool CEditorProject::Paste(int to, CArray<CPreviewSegment *, CPreviewSegment *> &clipBoard, int &insertLength, bool bUseNormalClipboard, bool bSetUndoRollback)
{
   insertLength = 0;
   
   if (clipBoard.GetSize() == 0)
      return false;
  
   // See Bug 2232
   bool bHadVideo = HasVideo();

   int toIndex = FindElementAt(to);
   
   CUnRedoElement *undo = new CUnRedoElement();
   undo->SetType(CUnRedoElement::CUT_PASTE);
   // copy preview list to undo element
   undo->Append(m_previewList, 0, m_previewList.GetSize(), true);
   
   bool isPreviewEmpty = (m_previewList.GetSize() == 0);
   
   // split element if necessary
   CPreviewSegment *newPart = isPreviewEmpty ? NULL : m_previewList[toIndex]->Split(to);
   
   // position to insert next element
   int insertionOffset  = 0;
   
   // insert before element at position toIndex (default: after element at position toIndex)
   if (!isPreviewEmpty && (newPart || m_previewList[toIndex]->GetTargetBegin() < to))
      insertionOffset = 1;
   else if (isPreviewEmpty) // Preview list is empty; so insert at position 0
      toIndex = 0;
   
   // insert elements from clipboard
   int partOffset = 0;
   for (int i = 0; i < clipBoard.GetSize(); ++i)
   {
      CPreviewSegment *insertPart = clipBoard[i]->Copy();
      if (insertPart)
      {
         int partLength = insertPart->GetTargetEnd() - insertPart->GetTargetBegin();
         insertPart->SetTargetBegin(to + partOffset);
         insertPart->SetTargetEnd(to + partOffset + partLength);

         CStreamContainer *pContainer = CreateNewContainer(insertPart);

         m_streamList.AddTail(pContainer);
         insertPart->SetStreams(pContainer);

         m_previewList.InsertAt(toIndex + insertionOffset, insertPart);
         partOffset += partLength;
         ++insertionOffset;
      }
   }

   
   // insert second part of splitted element
   if (newPart)
   {
      newPart->IncrementTargetBegin(partOffset);
      newPart->IncrementTargetEnd(partOffset);
      
      CStreamContainer *pContainer = CreateNewContainer(newPart);

      m_streamList.AddTail(pContainer);
      newPart->SetStreams(pContainer);

      m_previewList.InsertAt(toIndex + insertionOffset, newPart);
      ++insertionOffset;
   }
   
   insertLength = partOffset;

   
   if (m_aStreams.GetSize() > 0)
   {
      HRESULT hr = S_OK;

      CClipboard emptyClipboard;

      CClipboard *pClipboardToUse = &m_ClipboardNew;

      if (!bUseNormalClipboard)
         pClipboardToUse = &emptyClipboard;
      // (old) misuse of paste mechanism for clip insertion (with length increase)
      // TODO remove/rewrite later
      // also used for insertion of a new page (new code)

      for (int s=0; s<m_aStreams.GetSize() && SUCCEEDED(hr); ++s)
         hr = m_aStreams[s]->Paste(to, partOffset, pClipboardToUse);
      
      _ASSERT(SUCCEEDED(hr));
   }

   
   // remove elements from redo list
   EmptyRedoList();
   // append new Element to undo list
   m_undoList.AddTail(undo);

   
   HRESULT hr = m_UndoManager.RegisterAction(this, UNDO_LEGACY_CHANGE, NULL, 0, HINT_CHANGE_STRUCTURE);
   
   // "bSetUndoRollback": 
   // (old) misuse of paste mechanism for clip insertion (with length increase)
   // TODO remove/rewrite later
   if (SUCCEEDED(hr) && bSetUndoRollback) 
      hr = m_UndoManager.SetRollbackPoint();


   
   if (!isPreviewEmpty)
   {
      // update timestamps of element behind last inserted
      IncrementFrom(toIndex+insertionOffset, partOffset); 
   }
   
   RemoveEmptyParts();

   // See Bug 2232
   if (!bHadVideo && HasVideo())
   {
      if (m_videoBackupAviSettings.bIsVideoFormatInitialized)
      {
         m_videoAviSettings.Clone(&m_videoBackupAviSettings);
      }
   }

   m_iLastPasteTo = to;
   m_iLastPasteLength = insertLength;
   
   return true;
}

void CEditorProject::Delete(int from, int to)
{
   if (m_aStreams.GetSize() > 0)
   {
      HRESULT hr = S_OK;
      
      for (int s=0; s<m_aStreams.GetSize() && SUCCEEDED(hr); ++s)
         hr = m_aStreams[s]->Delete(from, to-from);
      
      _ASSERT(SUCCEEDED(hr));
   }


   if (m_previewList.GetSize() == 0)
      return;

   bool bHadVideo = HasVideo();
   
   int fromIndex = FindElementAt(from);
   int toIndex   = FindElementAt(to);
   
   CUnRedoElement *undo = new CUnRedoElement();
   undo->SetType(CUnRedoElement::CUT_PASTE);
   // copy preview list to undo element
   undo->Append(m_previewList, 0, m_previewList.GetSize(), true);
   
   int insertionCount = 0;
   if (fromIndex == toIndex)
   {
      // split element and adjust timestamps
      CPreviewSegment *newPart = m_previewList[fromIndex]->Split(from);
      int timeDiff = to - from;
      if (newPart)
      {
         newPart->IncrementSourceBegin(timeDiff);
         newPart->DecrementTargetEnd(timeDiff);
         m_previewList.InsertAt(fromIndex+1, newPart);
         insertionCount = 2;
         // set tiIndex to index after changed element
         toIndex = fromIndex + 2;
      }
      else
      {
         m_previewList[fromIndex]->IncrementSourceBegin(timeDiff);
         m_previewList[fromIndex]->DecrementTargetEnd(timeDiff);
         insertionCount = 1;
         // set tiIndex to index after changed element
         toIndex = fromIndex + 1;
      }
   }
   else
   {
      // adjust timestamps and delete elements
      int timeDiff = m_previewList[fromIndex]->GetTargetEnd() - from;
      m_previewList[fromIndex]->DecrementSourceEnd(timeDiff);
      m_previewList[fromIndex]->DecrementTargetEnd(timeDiff);
      int elementsToDelete = toIndex - (fromIndex + 1);
      // delete elements from previewList
      for (int i = 0; i < elementsToDelete; ++i)
      {
         CPreviewSegment *pSegment = m_previewList[fromIndex + 1 + i];
         if (pSegment)
            delete pSegment;
         pSegment = NULL;
      }
      m_previewList.RemoveAt(fromIndex + 1, elementsToDelete);
      
      // update index
      toIndex = fromIndex + 1;
      
      timeDiff = to - m_previewList[toIndex]->GetTargetBegin();
      
      m_previewList[toIndex]->IncrementSourceBegin(timeDiff);
      m_previewList[toIndex]->IncrementTargetBegin(timeDiff);
      
      insertionCount = 2;
      
   }
   
   // remove elements from redo list
   EmptyRedoList();
   // append new Element to undo list
   m_undoList.AddTail(undo);


   HRESULT hr = m_UndoManager.RegisterAction(this, UNDO_LEGACY_CHANGE, NULL, 0, HINT_CHANGE_STRUCTURE);
   
   if (SUCCEEDED(hr)) 
      hr = m_UndoManager.SetRollbackPoint();

      
   
   // adjust timestamps of followin elements
   for (int i = toIndex; i < m_previewList.GetSize(); ++i)
   {
      int timeDiff = to - from;
      m_previewList[i]->DecrementTargetBegin(timeDiff);
      m_previewList[i]->DecrementTargetEnd(timeDiff);
   }
   
   // remove all parts with begin == end
   RemoveEmptyParts();

   if (bHadVideo && !HasVideo())
   {
      // See Bug 2232
      m_videoBackupAviSettings.Clone(&m_videoAviSettings);

      ResetPreviewFormat();

      // Note: This ignores the possibility that a
      // clip could be removed which would normally
      // impose a change of the preview format.
   }

   // Check the video size of denver documents?
   if (IsDenverDocument())
      RecalcDenverVideoParams();
}

bool CEditorProject::UndoRedoCutPaste(int action)
{
   CList<CUnRedoElement *, CUnRedoElement *> *fromList;
   CList<CUnRedoElement *, CUnRedoElement *> *toList;
   
   if (action == ACTION_UNDO)
   {
      fromList = &m_undoList;
      toList   = &m_redoList;
   }
   else
   {
      fromList = &m_redoList;
      toList   = &m_undoList;
   }
   
   if (fromList->IsEmpty())
      return false;
   
   // extract last undo element;
   CUnRedoElement *fromElement = fromList->GetTail();
   if (!fromElement)
      return false;
   
   CUnRedoElement *toElement = new CUnRedoElement();
   toElement->SetType(CUnRedoElement::CUT_PASTE);
   // move preview list to redo list
   toElement->Append(m_previewList, 0, m_previewList.GetSize(), false);
   m_previewList.RemoveAll();
   
   // move preview segments from undo list to preview list
   fromElement->MoveElements(m_previewList);
   
   //delete undo element
   delete fromElement;
   fromList->RemoveTail();
   
   // append new redo
   toList->AddTail(toElement);
   
   return true;
}

bool CEditorProject::UnRedoImportAudio(int action)
{
   CList<CUnRedoElement *, CUnRedoElement *> *fromList;
   CList<CUnRedoElement *, CUnRedoElement *> *toList;
   
   if (action == ACTION_UNDO)
   {
      fromList = &m_undoList;
      toList   = &m_redoList;
   }
   else
   {
      fromList = &m_redoList;
      toList   = &m_undoList;
   }
   
   if (fromList->IsEmpty())
      return false;
   
   CUnRedoElement *fromElement = fromList->GetTail();
   CUnRedoElement *toElement = new CUnRedoElement();
   toElement->SetType(CUnRedoElement::IMPORT_AUDIO);
   
   toElement->GetStreamsFrom(m_streamList);
   fromElement->MoveStreamsTo(m_streamList);
   
   // move preview list to redo list
   toElement->Append(m_previewList, 0, m_previewList.GetSize(), false);
   m_previewList.RemoveAll();
   
   // move preview segments from undo list to preview list
   fromElement->MoveElements(m_previewList);
   
   toList->AddTail(toElement);
   delete fromElement;
   fromElement = NULL;
   fromList->RemoveTail();
   
   return true;
}

bool CEditorProject::UnRedoImportVideo(int action)
{
   CList<CUnRedoElement *, CUnRedoElement *> *fromList;
   CList<CUnRedoElement *, CUnRedoElement *> *toList;
   
   if (action == ACTION_UNDO)
   {
      fromList = &m_undoList;
      toList   = &m_redoList;
   }
   else
   {
      fromList = &m_redoList;
      toList   = &m_undoList;
   }
   
   if (fromList->IsEmpty())
      return false;
   
   CUnRedoElement *fromElement = fromList->GetTail();
   CUnRedoElement *toElement = new CUnRedoElement();
   toElement->SetType(CUnRedoElement::IMPORT_VIDEO);
   
   toElement->GetStreamsFrom(m_streamList);
   fromElement->MoveStreamsTo(m_streamList);
   
   // move preview list to redo list
   toElement->Append(m_previewList, 0, m_previewList.GetSize(), false);
   m_previewList.RemoveAll();
   
   // move preview segments from undo list to preview list
   fromElement->MoveElements(m_previewList);
   
   toList->AddTail(toElement);
   delete fromElement;
   fromElement = NULL;
   fromList->RemoveTail();
   
   return true;
}

bool CEditorProject::UnRedoImportVideoAudio(int action)
{
   CList<CUnRedoElement *, CUnRedoElement *> *fromList;
   CList<CUnRedoElement *, CUnRedoElement *> *toList;
   
   if (action == ACTION_UNDO)
   {
      fromList = &m_undoList;
      toList   = &m_redoList;
   }
   else
   {
      fromList = &m_redoList;
      toList   = &m_undoList;
   }
   
   if (fromList->IsEmpty())
      return false;
   
   CUnRedoElement *fromElement = fromList->GetTail();
   CUnRedoElement *toElement = new CUnRedoElement();
   toElement->SetType(CUnRedoElement::IMPORT_VIDEOAUDIO);
   
   toElement->GetStreamsFrom(m_streamList);
   fromElement->MoveStreamsTo(m_streamList);
   
   // move preview list to redo list
   toElement->Append(m_previewList, 0, m_previewList.GetSize(), false);
   m_previewList.RemoveAll();
   
   // move preview segments from undo list to preview list
   fromElement->MoveElements(m_previewList);
   
   toList->AddTail(toElement);
   delete fromElement;
   fromElement = NULL;
   fromList->RemoveTail();
   
   return true;
}

bool CEditorProject::UndoDeletePaste()
{
   if (m_undoList.GetCount() < 2)
      return false;
   
   POSITION position = m_undoList.GetTailPosition();
   CUnRedoElement *pPaste = m_undoList.GetPrev(position);
   CUnRedoElement *pDelete = m_undoList.GetPrev(position);
   
   if (pPaste->GetType() != CUnRedoElement::DELETE_PASTE || 
      pDelete->GetType() != CUnRedoElement::DELETE_PASTE)
   {
      // this could not be
      // TODO?: Fehlermeldung
      return false;
   }
   
   // undo paste
   UndoRedoCutPaste(0);
   
   // then undo delete
   UndoRedoCutPaste(0);
   
   position = m_redoList.GetTailPosition();
   pPaste = m_redoList.GetPrev(position);
   if (pPaste)
      pPaste->SetType(CUnRedoElement::DELETE_PASTE);
   
   pDelete = m_redoList.GetPrev(position);
   if (pDelete)
      pDelete->SetType(CUnRedoElement::DELETE_PASTE);
   
   // Check preview format if we do not have a video
   if (!HasVideo())
      ResetPreviewFormat();
   
   return true;
}

bool CEditorProject::RedoDeletePaste()
{
   if (m_redoList.GetCount() < 2)
      return false;
   
   POSITION position = m_redoList.GetTailPosition();
   CUnRedoElement *pPaste  = m_redoList.GetPrev(position);
   CUnRedoElement *pDelete = m_redoList.GetPrev(position);
   
   if (pPaste->GetType() != CUnRedoElement::DELETE_PASTE || 
      pDelete->GetType() != CUnRedoElement::DELETE_PASTE)
   {
      // this could not be
      // TODO?: Fehlermeldung
      return false;
   }
   
   // redo delete
   UndoRedoCutPaste(1);
   
   // then redo paste
   UndoRedoCutPaste(1);
   
   position = m_undoList.GetTailPosition();
   pPaste = m_undoList.GetPrev(position);
   if (pPaste)
      pPaste->SetType(CUnRedoElement::DELETE_PASTE);
   
   pDelete = m_undoList.GetPrev(position);
   if (pDelete)
      pDelete->SetType(CUnRedoElement::DELETE_PASTE);
   
   // Check preview format if we do not have a video
   if (!HasVideo())
      ResetPreviewFormat();
   
   return true;
}

bool CEditorProject::UnRedoMoveClip(int action)
{
   CList<CUnRedoElement *, CUnRedoElement *> *fromList;
   CList<CUnRedoElement *, CUnRedoElement *> *toList;
   
   if (action == ACTION_UNDO)
   {
      fromList = &m_undoList;
      toList   = &m_redoList;
   }
   else
   {
      fromList = &m_redoList;
      toList   = &m_undoList;
   }
   
   if (fromList->IsEmpty())
      return false;
   
   POSITION fromPosition = fromList->GetTailPosition();
   bool bFinished = false;
   bool bFirst = true;
   while (fromPosition && !bFinished)
   {
      CUnRedoElement *pPaste = fromList->GetPrev(fromPosition);
      CUnRedoElement *pDelete = fromList->GetPrev(fromPosition);
      
      if (pDelete->GetType() == CUnRedoElement::BEGIN_MOVE_CLIP)
         bFinished = true;
      
      if (!bFinished && 
         (pPaste->GetType() != CUnRedoElement::MOVE_CLIP || 
         pDelete->GetType() != CUnRedoElement::MOVE_CLIP))
      {
         // this could not be
         // TODO?: Fehlermeldung
         return false;
      }
      
      // undo paste
      UndoRedoCutPaste(action);
      
      // then undo delete
      UndoRedoCutPaste(action);
      
      POSITION toPosition   = toList->GetTailPosition();
      pPaste = toList->GetPrev(toPosition);
      if (pPaste)
         pPaste->SetType(CUnRedoElement::MOVE_CLIP);
      
      pDelete = toList->GetPrev(toPosition);
      if (pDelete)
      {
         if (bFirst)
            pDelete->SetType(CUnRedoElement::BEGIN_MOVE_CLIP);
         else
            pDelete->SetType(CUnRedoElement::MOVE_CLIP);
      }
      
      bFirst = false;
   }
   
   return true;
}

bool CEditorProject::IsUndoPossible()
{
   return m_UndoManager.HasUndoElements();
}


HRESULT CEditorProject::UndoAction(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs, UINT nLengthMs)
{
   HRESULT hr = S_OK;

   if (idActionCode == UNDO_LEGACY_CHANGE)
   {
      // This action exists to always have something registered in the new undo
      // management, even for actions not concerning new streams.
      // Actions only concerning new streams will not have such an action registered.
      // "m_bUndoLegacyAction" will stay "false".
      // So the new undo management is always in sync with the old one.
   
      m_bUndoLegacyAction = true;

      // an undo of a legacy action will probably lead to other legacy actions
      if (SUCCEEDED(hr))
         hr = m_UndoManager.RegisterAction(this, UNDO_LEGACY_CHANGE, NULL, 0, HINT_CHANGE_STRUCTURE);
   }
   else
      hr = S_FALSE;

   return hr;
}


bool CEditorProject::Undo(UpdateHintId *pidMaximumChange, CArray<UINT, UINT> *paBeginTimes)
{
   m_bUndoLegacyAction = false;

   HRESULT hr = S_OK;
   hr = m_UndoManager.PerformUndo(pidMaximumChange, paBeginTimes);
   
   _ASSERT(SUCCEEDED(hr));


   if (!m_bUndoLegacyAction)
      return true; // TODO: "true" means: repaint (and update). Is that always correct?

   if (m_undoList.IsEmpty())
      return false;
   
   // See Bug 2232
   bool bHadVideo = HasVideo();

   bool bReturnVal = false;

   CUnRedoElement *lastElement = m_undoList.GetTail();
   if (lastElement->GetType() == CUnRedoElement::CUT_PASTE)
      bReturnVal = UndoRedoCutPaste(ACTION_UNDO);
   else if (lastElement->GetType() == CUnRedoElement::IMPORT_AUDIO)
      bReturnVal = UnRedoImportAudio(ACTION_UNDO);
   else if (lastElement->GetType() == CUnRedoElement::IMPORT_VIDEO)
      bReturnVal = UnRedoImportVideo(ACTION_UNDO);
   else if (lastElement->GetType() == CUnRedoElement::IMPORT_VIDEOAUDIO)
      bReturnVal = UnRedoImportVideoAudio(ACTION_UNDO);
   else if (lastElement->GetType() == CUnRedoElement::MOVE_CLIP)
      bReturnVal = UnRedoMoveClip(ACTION_UNDO);
   else if (lastElement->GetType() == CUnRedoElement::DELETE_PASTE)
      bReturnVal = UndoDeletePaste();
   
   RemoveEmptyParts();

   // See Bug 2232
   if (bHadVideo && !HasVideo())
   {
      m_videoBackupAviSettings.Clone(&m_videoAviSettings);
      ResetPreviewFormat();
   }

   // See Bug 2232
   if (!bHadVideo && HasVideo())
   {
      if (m_videoBackupAviSettings.bIsVideoFormatInitialized)
         m_videoAviSettings.Clone(&m_videoBackupAviSettings);
   }

   // See Bug 2244:
   if (IsDenverDocument())
      RecalcDenverVideoParams();
   
   return bReturnVal;
}

bool CEditorProject::IsRedoPossible()
{
   return m_UndoManager.HasRedoElements();
}

bool CEditorProject::Redo(UpdateHintId *pidMaximumChange, CArray<UINT, UINT> *paBeginTimes)
{
   m_bUndoLegacyAction = false;

   HRESULT hr = S_OK;
   hr = m_UndoManager.PerformRedo(pidMaximumChange, paBeginTimes);
 
   _ASSERT(SUCCEEDED(hr));


   if (!m_bUndoLegacyAction)
      return true; // TODO: "true" means: repaint (and update). Is that always correct?

   if (m_redoList.IsEmpty())
      return false;

   // See Bug 2232
   bool bHadVideo = HasVideo();
   
   bool bReturnVal = false;

   CUnRedoElement *lastElement = m_redoList.GetTail();
   if (lastElement->GetType() == CUnRedoElement::CUT_PASTE)
      bReturnVal = UndoRedoCutPaste(ACTION_REDO);
   else if (lastElement->GetType() == CUnRedoElement::IMPORT_AUDIO)
      bReturnVal = UnRedoImportAudio(ACTION_REDO);
   else if (lastElement->GetType() == CUnRedoElement::IMPORT_VIDEO)
      bReturnVal = UnRedoImportVideo(ACTION_REDO);
   else if (lastElement->GetType() == CUnRedoElement::IMPORT_VIDEOAUDIO)
      bReturnVal = UnRedoImportVideoAudio(ACTION_REDO);
   else if (lastElement->GetType() == CUnRedoElement::MOVE_CLIP)
      bReturnVal = UnRedoMoveClip(ACTION_REDO);
   else if (lastElement->GetType() == CUnRedoElement::DELETE_PASTE)
      bReturnVal = RedoDeletePaste();
   
   RemoveEmptyParts();

   // See Bug 2232
   if (bHadVideo && !HasVideo())
   {
      m_videoBackupAviSettings.Clone(&m_videoAviSettings);
      ResetPreviewFormat();
   }

   // See Bug 2232
   if (!bHadVideo && HasVideo())
   {
      if (m_videoBackupAviSettings.bIsVideoFormatInitialized)
         m_videoAviSettings.Clone(&m_videoBackupAviSettings);
   }

   // See Bug 2244:
   if (IsDenverDocument())
      RecalcDenverVideoParams();

   return bReturnVal;
}

void CEditorProject::EmptyUndoList()
{
   POSITION position = m_undoList.GetHeadPosition();
   while (position)
   {
      CUnRedoElement *element = m_undoList.GetNext(position);
      delete element;
   }
   m_undoList.RemoveAll();
   
   
}

void CEditorProject::EmptyRedoList()
{ 
   POSITION position = m_redoList.GetHeadPosition();
   while (position)
   {
      CUnRedoElement *element = m_redoList.GetNext(position);
      delete element;
   }
   m_redoList.RemoveAll();
}

HRESULT CEditorProject::ClearStreamClipboard()
{
   DeleteClipBoard(m_clipBoard); // old clipboard

   return m_ClipboardNew.Clear();
}


HRESULT CEditorProject::InsertNewPage(UINT nPositionMs, CString &csPageTitle, UINT nLengthMs)
{
   if (nLengthMs > 0)
   {
      CPreviewSegment *pSegment = new CPreviewSegment();
      pSegment->SetSourceBegin(0);
      pSegment->SetSourceEnd(nLengthMs);
      pSegment->SetTargetBegin(nPositionMs);
      pSegment->SetTargetEnd(nPositionMs + nLengthMs);
      
      CStreamContainer *pContainer = new CStreamContainer();
      pContainer->SetID(++CStreamContainer::m_nContainerCounter);



      CPageStream *pPageStream = new CPageStream();
      pPageStream->SetID(++CStreamContainer::m_iPageStreamCount);

      CPage *pPage = new CPage();
      pPage->SetBegin(0);
      pPage->SetEnd(nLengthMs);
      pPage->SetTitle(csPageTitle);
      // join id (and jump id) must/will be created  during UpdateStructureAndStreams()

      pPageStream->Add(pPage);


      
      CWhiteboardStream *pWhiteboardStream = new CWhiteboardStream();
      pWhiteboardStream->SetID(++CStreamContainer::m_iWhiteboardStreamCount);
      pWhiteboardStream->InitEmpty();


      CMetadataStream *pMetadataStream = new CMetadataStream();
      pMetadataStream->SetID(++CStreamContainer::m_iMetadataCount);
      
      pMetadataStream->GetStructureInformation()->FillWithNewPage(csPageTitle, nLengthMs);


      pMetadataStream->GetStructureInformation()->Complete(NULL, pWhiteboardStream, pPageStream);
      // only important for setting "m_drawEvent" in the CPageInformation
     
 
      CAudioStream *pAudioStream = new CAudioStream();
      pAudioStream->SetID(++CStreamContainer::m_iAudioStreamCount);
      pAudioStream->SetFilename(NULL, 0, m_nVersionType);


      pContainer->SetWhiteboardStream(pWhiteboardStream);
      pContainer->SetPageStream(pPageStream);
      pContainer->SetMetadata(pMetadataStream);
      pContainer->SetAudioStream(pAudioStream);

     
      m_streamList.AddTail(pContainer);
      pSegment->SetStreams(pContainer);

      // Note: audio stream remains NULL; this is possible and just plays and exports silence
      
      int iInsertLength = 0;
      CArray<CPreviewSegment *, CPreviewSegment *> aInsertSegments;
      aInsertSegments.Add(pSegment);
      Paste(nPositionMs, aInsertSegments, iInsertLength, false, true);
      DeleteClipBoard(aInsertSegments);
      
      // TODO misuse of Paste():
      // this also takes care of new streams and old undo
      // see Note below
      
      return S_OK;
   }
 
   return S_FALSE;
}


bool CEditorProject::InsertClip(LPCTSTR tszFilename, int timestamp, bool &bNeedUpdate)
{
   CString csFileName = tszFilename;
   
   bNeedUpdate = false;
   int clipLength;
   if (_taccess(csFileName, 00) == -1)
   {
      CString msg;
      msg.Format(IDS_MSG_FILE_NOT_FOUND_SEARCH, csFileName);
      int result = CEditorDoc::ShowWarningMessage(msg, NULL, MB_YESNO);
      if (result == IDYES)
      {
         CString csDir = csFileName;
         StringManipulation::GetPath(csDir);
         CString filter;
         filter.LoadString(IDS_AVI_FILEFILTER);
         FileDialog::GetOpenFilename(AfxGetApp(), csFileName, AfxGetMainWnd()->GetSafeHwnd(), 
            AfxGetInstanceHandle(), filter, _T("CLIPIMPORT"), csDir);
         if (csFileName.IsEmpty())
            return false;
      }
      else
         return false;
   }
   
   if (!VideoInformation::GetVideoDuration(csFileName, clipLength))
   {
      CString msg;
      msg.Format(IDS_MSG_GET_VIDEO_LENGTH_FAILED, csFileName);
      CEditorDoc::ShowWarningMessage(msg);
      return false;
   }
   
   // Check for length of clip vs. end of presentation,
   // i.e., CEditorDoc::m_docLengthMs (GetTotalLength())
   int totalLength = GetTotalLength();
   if (totalLength <= timestamp)
   {
      // This should not be possible
      ASSERT(false);
      return false;
   }
   
   // Try to find the correct stream ID for video data
   int nVideoStreamID = -1;
   int nAudioStreamID = -1;
   // Audio Action, can be IDC_MIX_AUDIO, IDC_INSERT_AUDIO, IDC_REPLACE_AUDIO or IDC_NO_AUDIO
   int nAudioAction = IDC_NO_AUDIO;
   // Assume audio is contained in clip. This may be changed later if
   // we need to convert the audio file to a WAV file
   CString csAudioStreamSource = csFileName;
   CString csOriginalAudioSource = _T("");
   int nOriginalAudioStream = -1;
   
   {
      int nStreamID = 0;
      AVInfo avInfo;
      HRESULT hr = avInfo.Initialize(csFileName);
      if (SUCCEEDED(hr))
         hr = avInfo.GetVideoStreamIndex(&nVideoStreamID);
      if (SUCCEEDED(hr))
         hr = avInfo.GetAudioStreamIndex(&nAudioStreamID);
     
      
      if (nVideoStreamID != -1)
         hr = S_OK;
      
      if (FAILED(hr) || nVideoStreamID == -1)
      {
         CString csMsg;
         csMsg.LoadString(IDS_MSG_CLIP_NOVIDEOSTREAM);
         CEditorDoc::ShowWarningMessage(csMsg);
         return false;
      }
      
      if (nAudioStreamID != -1)
      {
         CImportClipAudioDlg dlg;
         nAudioAction = dlg.DoModal();
         
         if (nAudioAction == IDCANCEL)
            return false;

         if (nAudioAction != IDC_NO_AUDIO)
         {
            // Clip contains audio, import it!
            AM_MEDIA_TYPE mtAudio;
            hr = avInfo.GetStreamFormat(nAudioStreamID, &mtAudio);
            if (SUCCEEDED(hr))
            {
               if (!AVInfo::StaticIsPcmAudio(&mtAudio))
               {
                  CString csTargetWavFileName;
                  int nAudioStreamBackup = nAudioStreamID;
                  
                  bool cool = CheckAndRestoreAudioFile(csTargetWavFileName, nAudioStreamID, csFileName, nAudioStreamID);
                  if (cool)
                  {
                     // Remember where we got the information from
                     csOriginalAudioSource = csFileName;
                     nOriginalAudioStream = nAudioStreamBackup;

                     // And remember the new file name.
                     csAudioStreamSource = csTargetWavFileName;
                     // The Stream ID in the WAV file is 0
                     nAudioStreamID = 0;

                  }
                  else
                  {
                     MYFREEMEDIATYPE(mtAudio);
                     return false;
                  }
               }
               
               MYFREEMEDIATYPE(mtAudio);
            }
         }
         else
         {
            // The user does not want audio. So don't import it.
            nAudioStreamID = -1;
         }
      }
   }


   CClipInfo *pClip = new CClipInfo();

   pClip->SetFilename(csFileName);
   pClip->SetLength(clipLength);
   pClip->SetTimestamp(0);
   pClip->SetTitle(_T("Clip"));
   pClip->SetStreamSelector(nVideoStreamID);
   pClip->SetAudioStreamSelector(nAudioStreamID);
   if (pClip->HasAudioStream())
   {
      pClip->SetAudioFilename(csAudioStreamSource);
      
      if (IDC_REPLACE_AUDIO == nAudioAction)
         pClip->SetTempOriginalAudioVolume(0);
      else
         pClip->SetTempOriginalAudioVolume(100);
      
      pClip->SetOriginalAudioSource(csOriginalAudioSource);
      pClip->SetOriginalAudioSourceStream(nOriginalAudioStream);
   }
   
   
   

   if (nAudioAction == IDC_INSERT_AUDIO)
   {
      CPreviewSegment *segment = new CPreviewSegment();
      segment->SetSourceBegin(0);
      segment->SetSourceEnd(clipLength);
      segment->SetTargetBegin(timestamp);
      segment->SetTargetEnd(timestamp + clipLength);
      
      // create new stream container and copy audio-, video-, whiteboardstream and metadata
      CStreamContainer *pContainer = new CStreamContainer();
      pContainer->SetID(++CStreamContainer::m_nContainerCounter);
      
      CAudioStream *pAudioStream = new CAudioStream();
      pAudioStream->SetID(++CStreamContainer::m_iAudioStreamCount);
      pAudioStream->SetFilename(NULL, 0, m_nVersionType);
      pContainer->SetAudioStream(pAudioStream);
      
      pContainer->SetVideoStream(NULL);
      pContainer->SetWhiteboardStream(NULL);
      pContainer->SetMetadata(NULL);
      
      // create new clip stream
      CClipStream *pClipStream = new CClipStream();
      pClipStream->SetID(++CStreamContainer::m_iClipStreamCount);
      pClipStream->AddClip(pClip);
      
      pContainer->SetClipStream(pClipStream);
      
      m_streamList.AddTail(pContainer);
      segment->SetStreams(pContainer);
      
      int insertLength = 0;
      CArray<CPreviewSegment *, CPreviewSegment *> clipBoard;
      clipBoard.Add(segment);
      Paste(timestamp, clipBoard, insertLength, false, false);
      DeleteClipBoard(clipBoard);
      
      // (old) misuse of Paste():
      // this also makes room for the new clip in the new clip stream
      // see Note below

      // length changed:
      bNeedUpdate = true;
   }

   
   if (nAudioAction == IDC_NO_AUDIO ||
      nAudioAction == IDC_MIX_AUDIO ||
      nAudioAction == IDC_REPLACE_AUDIO)
   {
      
      if (timestamp + clipLength > totalLength)
      {
         // clip too long for document? show dialog 

         int originalClipLength = clipLength;
         clipLength = totalLength - timestamp;

         // TODO this is not the real clip length; maybe that is also needed/useful?
         
         CString csOrigLength;
         CString csNewLength;
         Misc::FormatTickString(csOrigLength, originalClipLength, SU_MINUTES, false);
         Misc::FormatTickString(csNewLength, clipLength, SU_MINUTES, false);
         CString msg;
         msg.Format(IDS_MSG_SHORTENCLIP, csOrigLength, csNewLength);
         CEditorDoc::ShowInfoMessage(msg);
      }
      
      // TODO this is wrong: different clip segments can have different settings
      
      /* see below
      // Do we have settings for this clip already?
      CAviSettings *pAviSettings = NULL;
      if (TRUE == m_clipAviSettings.Lookup(csFileName, pAviSettings))
      {
         if (IDC_REPLACE_AUDIO == nAudioAction)
            pAviSettings->nOriginalAudioVolume = 0;
         else
            pAviSettings->nOriginalAudioVolume = 100;
      }
      //*/
   }

   // creates settings if clip not already present
   CAviSettings *pAviSettings = LookupClipAviSettings(csFileName); 
   if (IDC_REPLACE_AUDIO == nAudioAction)
      pAviSettings->nOriginalAudioVolume = 0;
   else
      pAviSettings->nOriginalAudioVolume = 100;
   // TODO this resets this setting if there was already a clip (segment) with this file name.
  
       
   HRESULT hr = GetClipStream(true)->IncludeClip(timestamp, pClip, false);
   if (FAILED(hr) && hr == E_CS_REPLACE_CLIP)
   {
      // ask the user if he wants to replace existing clip (parts)

      CString msg;
      msg.LoadString(IDS_MSG_REPLACE_EXISTING_CLIP);
      CString cap;
      cap.LoadString(IDS_QUESTION);
      int res = MessageBox(m_pProgress->GetSafeHwnd(), msg, cap, MB_ICONQUESTION | MB_YESNOCANCEL);
      // TODO: use CEditorDoc::ShowQuestionMessage()
      if (res == IDYES)
      {
         hr = GetClipStream(true)->IncludeClip(timestamp, pClip, true);
      }
      else
      {
         hr = S_FALSE;
      }        
   }

   // Note: currently IDC_INSERT_AUDIO does not matter here as in this case there was
   // made room for the clip above. This might change in future (above).
      
   _ASSERT(SUCCEEDED(hr));
      

   if (nAudioAction != IDC_INSERT_AUDIO)
      delete pClip;
   // else the clip stream stores and deletes the reference


      
 
   // Do not mess with the video format if we have a video
   // already.
   if (!HasVideo())
      ResetPreviewFormat();
   
   return true;

}

bool CEditorProject::InsertClipFromLrd(LPCTSTR tszFilename, int timestamp, CString &csClipFileName)
{
   CLrdFile *recording;
   recording = new CLrdFile();
   if (!recording->Open(tszFilename))
   {
      CString msg;
      msg.Format(IDS_MSG_INSERT_CLIP_LRD_FAILED);
      CEditorDoc::ShowErrorMessage(msg);
      delete recording;
      return false;
   }
   
   CClipStream *pClipStream = recording->CreateClipStream();
   if (!pClipStream || !pClipStream->HasClips())
   {
      CString msg;
      msg.Format(IDS_MSG_INSERT_CLIP_LRD_FAILED);
      CEditorDoc::ShowErrorMessage(msg);
      if (pClipStream)
         delete pClipStream;
      delete recording;
      return false;
   }
   
   if (pClipStream->m_arrClip.GetSize() > 1)
   {
      CString msg;
      msg.Format(IDS_MSG_MULTIPLE_CLIPS);
      CEditorDoc::ShowInfoMessage(msg);
   }
   
   CClipInfo *pClipInfo =  pClipStream->m_arrClip[0];
   
   if (!pClipInfo)
   {
      CString msg;
      msg.Format(IDS_MSG_INSERT_CLIP_LRD_FAILED);
      CEditorDoc::ShowErrorMessage(msg);
      delete pClipStream;
      delete recording;
      return false;
   }
   
   CString clipFilename = pClipInfo->GetFilename();
   
   delete pClipStream;
   delete recording;

   HRESULT hr = AVInfo::StaticCheckRenderable(clipFilename);
   if (S_OK != hr)
   {
      CString csMsg;
      csMsg.Format(IDS_MSG_VIDEOFILE_NOT_RENDERABLE, hr);
      CEditorDoc::ShowErrorMessage(csMsg);
      return false;
   }
   
   bool bNeedUpdate;
   bool bResult = InsertClip(clipFilename, timestamp, bNeedUpdate);
   if (bResult)
   {
      csClipFileName = clipFilename;
   }
   return bResult;
}

bool CEditorProject::DeleteClip(int timestamp, bool &bNeedUpdate)
{
   bNeedUpdate = false;

   CClipStreamR *pClipStream = GetClipStream();
   if (pClipStream != NULL)
   {
      UINT nClipStartMs = 0;
      UINT nClipLengthMs = 0;
      pClipStream->GetClipTimesAt(timestamp, &nClipStartMs, &nClipLengthMs);

      if (StreamsAreEmpty(nClipStartMs, nClipStartMs + nClipLengthMs))
      {
         // special case for clips with IDC_INSERT_AUDIO (project was enlarged)

         Delete(nClipStartMs, nClipStartMs + nClipLengthMs);

         bNeedUpdate = true; // the length changed

         return true;
      }


      HRESULT hr = pClipStream->DeleteClipAt(timestamp);

      _ASSERT(SUCCEEDED(hr));

      if(SUCCEEDED(hr) && hr == S_OK)
      {
         if (!HasVideo())
            ResetPreviewFormat();
         
         return true;
      }
      else
      {
         CString msg;
         msg.Format(IDS_MSG_DELETE_CLIP_FAILED);
         CEditorDoc::ShowErrorMessage(msg);
         return false;
      }
      
      
   }


   return false;
 
}
  

bool CEditorProject::InsertStillImage(LPCTSTR tszFilename)
{
   m_csStillImage = tszFilename;
   
   return true;
}

bool CEditorProject::RemoveStillImage()
{
   if (m_csStillImage.IsEmpty())
      return false;
   
   m_csStillImage.Empty();
   
   return true;
}

bool CEditorProject::IsEmpty() {
   return m_previewList.GetSize() == 0;
}

bool CEditorProject::IsLrdUsed(CString &lrdName)
{
   POSITION position = m_recordingList.GetHeadPosition();
   while (position)
   {
      CLrdFile *pLrdFile = m_recordingList.GetNext(position);
      if (pLrdFile)
      {
         CString lrdFilename;
         lrdFilename.Format(_T("%s\\%s"), pLrdFile->GetRecordPath(), pLrdFile->GetFilename());
         if (lrdFilename == lrdName)
            return true;
      }
   }
   
   return false;
}

CString CEditorProject::MakeAbsolute(LPCTSTR tszLepFileName, LPCTSTR tszRelPath)
{
   if (tszRelPath == NULL)
      return CString(_T(""));
   else if (_tcscmp(tszRelPath, _T("")) == 0)
      return CString(_T(""));

   CString csAbsPath = StringManipulation::MakeAbsolute(tszLepFileName, tszRelPath);
   return csAbsPath;
}

CString CEditorProject::MakeAbsoluteAndCheck(LPCTSTR tszLepFileName, LPCTSTR tszRelPath)
{
   CString csAbsPath = MakeAbsolute(tszLepFileName, tszRelPath);
   if (0 != _taccess(csAbsPath, 04))
   {
	  CString csMsg;
      csMsg.Format(IDS_MSG_FILENOTFOUND, csAbsPath);
      // Try to make modal from progress bar, if possible
      HWND hWnd = ::AfxGetMainWnd()->GetSafeHwnd();
      //if (m_pProgress)
         //hWnd = m_pProgress->GetSafeHwnd();
      CEditorDoc::ShowWarningMessage(csMsg, hWnd);
   }
   
   return csAbsPath;
}

bool CEditorProject::CheckVersion(CLepSgml *pLepFile, bool bShowMessage) {
    if (pLepFile == NULL)
        return false;

    SgmlElement *pVersionTag = pLepFile->Find(L"formatversion", L"editor");
    if (pVersionTag != NULL) {
        CString csVersion = pVersionTag->GetParameter();
        if (pVersionTag->GetValue(L"compatible") != NULL)
            csVersion = pVersionTag->GetValue(L"compatible");

        // Only does a lexicographical comparisson
        if (csVersion.Compare(CStudioApp::s_sCurrentLepVersion) > 0) {
            CString sVersionWarning;
            sVersionWarning.LoadString(IDS_MSG_LEPVERSIONWARNING);

            if (bShowMessage) {
                int iRes = CEditorDoc::ShowWarningMessage(sVersionWarning, NULL, MB_YESNO);

                if (iRes == IDNO)
                    return false; // Do not continue
            } // else it is probably called by another program (pListener != NULL|CEditorProject)
            // TODO this is not solved cleanly (else case: an error is ignored and never reported)

        } // else everything ok: format version is equal or older

    } // else no version present: that is ok (old)

    return true;
}

bool CEditorProject::ParseMetainfo(CLepSgml *pLepFile) {
    if (pLepFile == NULL)
        return false;

    m_targetMetainfo.ParseFrom(pLepFile);

    m_dwCodePage = ::GetACP();
    CString csCodepage;
    pLepFile->GetMetaInfo(_T("codepage"), csCodepage);
    if (!csCodepage.IsEmpty()) 
            m_dwCodePage = _ttoi(csCodepage);

    return true;
}

bool CEditorProject::ParseFiles(CLepSgml *pLepFile, LPCTSTR tszLepFileName) {
    if (pLepFile == NULL)
        return false;

    CStringArray aLrdNames;
    pLepFile->GetLrdNames(aLrdNames);

    for (int i = 0; i < aLrdNames.GetSize(); ++i) {
        CLrdFile *pRecording = new CLrdFile();

        CString csFilename = MakeAbsoluteAndCheck(tszLepFileName, aLrdNames[i]);
        CString csBackup = csFilename;
        StringManipulation::GetFilename(csFilename);
        pRecording->SetFilename(csFilename);

        CString csFilepath = csBackup;
        StringManipulation::GetPath(csFilepath);
        pRecording->SetRecordPath(csFilepath);

        m_recordingList.AddTail(pRecording);
    }

    return true;
}

bool CEditorProject::ParseConfig(CLepSgml *pLepFile) {
    if (pLepFile == NULL)
        return false;

    CString csShowClickPages;
    pLepFile->GetConfigParameter(_T("sg-showclickpages"), csShowClickPages);
    if (!csShowClickPages.IsEmpty())
        m_bShowClickPages = (csShowClickPages == _T("0") ? false : true);

    CString csViewMode;
    pLepFile->GetConfigParameter(_T("sg-viewmode"), csViewMode);
    if (!csViewMode.IsEmpty())
        m_bShowScreenGrabbingAsRecording = (csViewMode == _T("simulation") ? false : true);

    return true;
}                                   

void CEditorProject::ExtractVideoExportSettings(CLepSgml *pLepFile, SgmlElement *pExportTag, CAviSettings *pAviSettings, const _TCHAR *tszLepFile) {

    if (pLepFile == NULL)
        return;

    // video tag
    if (pExportTag == NULL)
        pExportTag = pLepFile->Find(_T("video"), _T("export"));

    if (pExportTag == NULL)
        return;

    CString csMediaType;
    CString csFormatType;
    CString csClipSource;
    CString csVideoFormatInitialized;
    CString csCodec;
    CString csQuality;
    CString csKeyframeRate;
    CString csUseKeyframes;
    CString csSmartRecompression;
    CString csGlobalOffset;
    CString csRememberAspectratio;
    CString csVolume;
    CString csOrigVolume;
    CString csOrigWidth;
    CString csOrigHeight;
    CString csOrigFps;
    CString csHasOrigSize;

    pLepFile->GetVideoExportSettings(pExportTag, csClipSource, csMediaType, csFormatType, csVideoFormatInitialized,
        csCodec, csQuality, csKeyframeRate, csUseKeyframes, csSmartRecompression,
        csGlobalOffset, csRememberAspectratio, csVolume, csOrigVolume,
        csOrigWidth, csOrigHeight, csOrigFps, csHasOrigSize);

    if (!csVideoFormatInitialized.IsEmpty())
        pAviSettings->bIsVideoFormatInitialized = (_ttoi(csVideoFormatInitialized) != 0);

    if (!csCodec.IsEmpty())
        pAviSettings->dwFCC = _ttoi(csCodec);

    if (!csQuality.IsEmpty())
        pAviSettings->quality = _ttoi(csQuality);

    if (!csKeyframeRate.IsEmpty())
        pAviSettings->keyframeRate = _ttoi(csKeyframeRate);

    if (!csUseKeyframes.IsEmpty()) 
        pAviSettings->bUseKeyframes = (_ttoi(csUseKeyframes) != 0);

    if (!csSmartRecompression.IsEmpty())
        pAviSettings->bUseSmartRecompression = (_ttoi(csSmartRecompression) != 0);

    if (!csGlobalOffset.IsEmpty())
        m_videoExportGlobalOffset = _ttoi(csGlobalOffset);

    if (!csRememberAspectratio.IsEmpty()) 
        pAviSettings->bRememberAspectRatio = (_ttoi(csRememberAspectratio) != 0);

    if (!csVolume.IsEmpty()) 
        pAviSettings->nAudioVolume = _ttoi(csVolume);

    if (!csOrigVolume.IsEmpty())
        pAviSettings->nOriginalAudioVolume = _ttoi(csOrigVolume);

    if (!csOrigWidth.IsEmpty()) 
        pAviSettings->nOriginalWidth = _ttoi(csOrigWidth);

    if (!csOrigHeight.IsEmpty())
        pAviSettings->nOriginalHeight = _ttoi(csOrigHeight);

    if (!csOrigFps.IsEmpty())
        pAviSettings->dOriginalFps = _tcstod(csOrigFps, NULL);

    if (!csHasOrigSize.IsEmpty())
        pAviSettings->bHasOriginalSize = (_ttoi(csHasOrigSize) != 0);

    if (pAviSettings->bIsVideoFormatInitialized && !csMediaType.IsEmpty() && !csFormatType.IsEmpty()) {
        // double code

        BYTE *szMediaTypeHex = new BYTE[csMediaType.GetLength()];
        BYTE *szFormatTypeHex = new BYTE[csFormatType.GetLength()];
#ifdef _UNICODE
        ::WideCharToMultiByte(CP_ACP, 0, csMediaType, csMediaType.GetLength(), (char *)szMediaTypeHex, csMediaType.GetLength(), NULL, NULL);
        ::WideCharToMultiByte(CP_ACP, 0, csFormatType, csFormatType.GetLength(), (char *)szFormatTypeHex, csFormatType.GetLength(), NULL, NULL);
#else
        strncpy((char *)szMediaTypeHex, mediatype, mediatype.GetLength());
        strncpy((char *)szFormatTypeHex, formattype, formattype.GetLength());
#endif

        Misc::SetStreamConfigData(&pAviSettings->videoFormat, szMediaTypeHex, csMediaType.GetLength(), 
            szFormatTypeHex, csFormatType.GetLength());

        delete[] szMediaTypeHex;
        delete[] szFormatTypeHex;

        if (!csClipSource.IsEmpty())
            m_clipAviSettings.SetAt(MakeAbsoluteAndCheck(tszLepFile, csClipSource), pAviSettings);
    }
}

bool CEditorProject::ParseExportSettings(CLepSgml *pLepFile, const _TCHAR *tszLepFile) {
    if (pLepFile == NULL)
        return false;

    ExtractVideoExportSettings(pLepFile, NULL, &m_videoAviSettings, tszLepFile);

    CString csMediaType;
    pLepFile->GetExportParameter(_T("audio"),_T("mediatype"), csMediaType);
    
    CString csFormatType;
    pLepFile->GetExportParameter(_T("audio"),_T("format"), csFormatType);

    CString csIsMediaTypeInitialized;
    pLepFile->GetExportParameter(_T("audio"),_T("ismediatypeinitialized"), csFormatType);
    if (!csIsMediaTypeInitialized.IsEmpty())
        m_isAudioFormatInitialized = (_ttoi(csIsMediaTypeInitialized) != 0);

    if (m_isAudioFormatInitialized && !csMediaType.IsEmpty() && !csFormatType.IsEmpty()) {
        // double code (see above)

        BYTE *szMediaTypeHex = new BYTE[csMediaType.GetLength()];
        BYTE *szFormatTypeHex = new BYTE[csFormatType.GetLength()];
#ifdef _UNICODE
        ::WideCharToMultiByte(CP_ACP, 0, csMediaType, csMediaType.GetLength(), (char *)szMediaTypeHex, csMediaType.GetLength(), NULL, NULL);
        ::WideCharToMultiByte(CP_ACP, 0, csFormatType, csFormatType.GetLength(), (char *)szFormatTypeHex, csFormatType.GetLength(), NULL, NULL);
#else
        strncpy((char *)szMediaTypeHex, csMediaType, csMediaType.GetLength());
        strncpy((char *)szFormatTypeHex, csFormatType, csFormatType.GetLength());
#endif

        Misc::SetStreamConfigData(&m_mtAudioFormat, szMediaTypeHex, csMediaType.GetLength(), 
            szFormatTypeHex, csFormatType.GetLength());

        delete[] szMediaTypeHex;
        delete[] szFormatTypeHex;
    }


    CString csSmartRecompression;
    pLepFile->GetExportParameter(_T("clips"),_T("smartrecompression"), csSmartRecompression);
    if (!csSmartRecompression.IsEmpty()) 
        m_clipExportUseSmartRecompression = (_ttoi(csSmartRecompression) != 0);

    CString csStretchClips;
    pLepFile->GetExportParameter(_T("clips"),_T("stretchclips"), csStretchClips);
    if (!csStretchClips.IsEmpty()) 
        m_clipExportStretchDenverClips = (_ttoi(csStretchClips) != 0);

    CString csAutoCalcSize;
    pLepFile->GetExportParameter(_T("clips"),_T("autocalcsize"), csAutoCalcSize);
    if (!csAutoCalcSize.IsEmpty()) 
        m_clipExportAutoCalcDenverSize = (_ttoi(csAutoCalcSize) != 0);

    // Now check for the Clip Media Types
    SgmlElement *pExportTag = pLepFile->Find(_T("export"), _T("editor"));
    if (pExportTag != NULL) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        pExportTag->GetElements(aElements);
        for (int i = 0; i < aElements.GetSize(); ++i) {
            SgmlElement *pClipTag = aElements[i];
            if (pClipTag != NULL) {
                if (_tcsicmp(pClipTag->GetName(), _T("clip")) == 0) {
                    CAviSettings *pClipAviSettings = new CAviSettings();
                    ExtractVideoExportSettings(pLepFile, pClipTag, pClipAviSettings, tszLepFile);
                }
            }
        }
    }

    return true;
}

bool CEditorProject::ParseStreams(CLepSgml *pLepFile, LPCTSTR tszLepFileName, IProgressListener *pListener) {
    if (pLepFile == NULL)
        return false;

    if (pListener != NULL) {
        pListener->SetProgress(0.05f);
        pListener->SetProgressLabel(IDS_PARSE_STREAMS);
    }

    // Note: ParseAudioStream() uses 40% of the progress: 0.05 -> 0.45 after
    if (!ParseAudioStream(pLepFile, tszLepFileName, pListener))
        return false;

    if (pListener != NULL)
        pListener->SetProgress(0.45f);

    if (!ParseVideoStream(pLepFile, tszLepFileName))
        return false;

    SgmlElement *pImageTag = pLepFile->Find(_T("image"), _T("streams"));
    if (pImageTag != NULL)
        m_csStillImage = MakeAbsoluteAndCheck(tszLepFileName, pImageTag->GetParameter());
    else
        m_csStillImage.Empty();

    if (pListener != NULL)
        pListener->SetProgress(0.5f);

    if (!ParseWbStream(pLepFile, tszLepFileName))
        return false;

    if (!ParseClipStream(pLepFile, tszLepFileName))
        return false;

    // Search for changes slide titles and keywords
    if (!ParseSlideStream(pLepFile, tszLepFileName))
        return false;

    SgmlElement *pMarkStreamTag = pLepFile->Find(_T("marks"), _T("streams"));
    if (pMarkStreamTag != NULL) {
        if (FAILED(GetMarkStream(true)->ReadLepEntry(pMarkStreamTag)))
            return false;
        // TODO: What about error messages here?
    }

    SgmlElement *pClipStreamTag = pLepFile->Find(_T("clips"), _T("streams"));
    if (pClipStreamTag != NULL) {
        if (FAILED(GetClipStream(true)->ReadLepEntry(pClipStreamTag, tszLepFileName)))
            return false;
        // TODO: What about error messages here?
    }

    // interactions and questions are parsed below

    if (pListener != NULL) {
        pListener->SetProgress(0.55f);
        pListener->SetProgressLabel(IDS_INIT_STREAMS);
    }

    int progressStep = (int)(15.0 /  m_previewList.GetSize());

    // Complete Metadata
    POSITION position = m_streamList.GetHeadPosition();
    while (position)
    {
        CStreamContainer *pContainer = m_streamList.GetNext(position);

        if (pContainer->GetMetadata())
            pContainer->GetMetadata()->Complete(pContainer->GetAudioStream(), pContainer->GetVideoStream(),
            pContainer->GetClipStream(), pContainer->GetWhiteboardStream(), pContainer->GetPageStream());
    }

    SgmlElement *pInteractionStreamTag = pLepFile->Find(_T("interactions"), _T("streams"));
    if (pInteractionStreamTag != NULL) {
        // must be done before interaction interpretation
        UpdateStructureAndStreams();
        // will be done again after this by CEditorDoc::Open

        if (FAILED(GetInteractionStream(true)->ReadLepEntry(pInteractionStreamTag, tszLepFileName)))
            return false;
        // TODO: What about error messages here?

        if (FAILED(GetInteractionStream()->ResolveTimes(0)))
            return false;

        SgmlElement *pQuestionStreamTag = pLepFile->Find(_T("questions"), _T("streams"));
        if (pQuestionStreamTag != NULL) {
            if (FAILED(GetQuestionStream(true)->ReadLepEntry(pQuestionStreamTag)))
                return false;
            // TODO: What about error messages here?

            if (FAILED(GetQuestionStream()->ResolveJumpTimes(0)))
                return false;
        }
    }

    if (pListener != NULL)
        pListener->SetProgress(0.7f);

    return true;
}

bool CEditorProject::ParseAudioStream(CLepSgml *pLepFile, LPCTSTR tszLepFileName, IProgressListener *pListener) {
    if (pLepFile == NULL)
        return false;

    // calculate progress step
    int iAudioStreamCount = pLepFile->GetAudioStreamCount();

    float fCurrentProgress = 0.05f; // done by ParseStreams()

    float fProgressStep = 0.0f;
    if (iAudioStreamCount > 0)
        fProgressStep = (0.4f /  iAudioStreamCount); // this method can/should use 40% of the progress

    // read audio information
    CArray<SgmlElement *, SgmlElement *> aAudioSources;
    pLepFile->GetAudioSources(aAudioSources);

    for (int i = 0; i < aAudioSources.GetCount(); ++i) {
        SgmlElement *pElement = aAudioSources[i];
        if (pElement != NULL) {
            int streamID = pElement->GetIntValue(_T("id"));
            int audioStreamID = pElement->GetIntValue(_T("stream-id"));
            CStreamContainer *pContainer = FindStreamContainer(streamID);
            if (!pContainer) {
                pContainer = new CStreamContainer();
                pContainer->SetID(streamID);
                if (CStreamContainer::m_nContainerCounter < streamID)
                    CStreamContainer::m_nContainerCounter = streamID;
                m_streamList.AddTail(pContainer);
            }

            if (!pContainer->GetAudioStream()) {
                CAudioStream *audioStream = FindAudioStream(audioStreamID);
                if (!audioStream) {
                    audioStream = new CAudioStream();

                    int nStreamSelector = pElement->GetIntValue(_T("stream-selector"));
                    CString origAudioFile = _T("");
                    if (NULL != pElement->GetValue(_T("orig-file")))
                        origAudioFile = MakeAbsolute(tszLepFileName, pElement->GetValue(_T("orig-file")));
                    int nOrigAudioStream = -1;
                    if (NULL != pElement->GetValue(_T("orig-stream-selector")))
                        nOrigAudioStream = pElement->GetIntValue(_T("orig-stream-selector"));

                    CString audioFilename = _T("");
                    if (origAudioFile.IsEmpty()) {
                        audioFilename = MakeAbsoluteAndCheck(tszLepFileName, pElement->GetValue(_T("file")));
                    } else {
                        audioFilename = MakeAbsolute(tszLepFileName, pElement->GetValue(_T("file")));

                        bool success = CheckAndRestoreAudioFile(audioFilename, nStreamSelector, 
                            origAudioFile, nOrigAudioStream);
                        if (!success) {
                            CString csMsg;
                            csMsg.Format(IDS_MSG_AUDIOSOURCE_NOTFOUND, origAudioFile);
                            CEditorDoc::ShowWarningMessage(csMsg);
                            delete audioStream;
                            return false;
                        }  
                    }

                    CWnd *hWnd = AfxGetMainWnd();
                    if (m_pProgress)
                        hWnd = m_pProgress;

                    if (_tcscmp(audioFilename, _T("")) != 0) {
                        // It's not a dummy audio stream
                        bool bAudioSuccess = InitAudioStream(audioStream, audioFilename, 
                            nStreamSelector, IDS_MSG_OPEN_AUDIO_FAILED, hWnd, hWnd->GetSafeHwnd(), pListener);
                        if (!bAudioSuccess) {
                            delete audioStream;
                            return false;
                        }
                    } else {
                        // Initialize a dummy stream
                        audioStream->SetFilename(NULL);
                    }

                    audioStream->SetID(audioStreamID);
                    if (CStreamContainer::m_iAudioStreamCount < audioStreamID)
                        CStreamContainer::m_iAudioStreamCount = audioStreamID;

                }

                pContainer->SetAudioStream(audioStream);
            }

            int targetBegin = pElement->GetIntValue(_T("stream-start"));
            CPreviewSegment *previewSegment = FindPreviewSegment(targetBegin);
            if (previewSegment) {
                if (!previewSegment->GetStreams())
                    previewSegment->SetStreams(pContainer);
                previewSegment->SetAudioOffset(pElement->GetIntValue(_T("source-offset")));
            } else {
                previewSegment = new CPreviewSegment();
                previewSegment->SetStreams(pContainer);
                previewSegment->SetTargetBegin(targetBegin);
                previewSegment->SetTargetEnd(pElement->GetIntValue(_T("stream-end")));
                previewSegment->SetSourceBegin(pElement->GetIntValue(_T("source-start")));
                previewSegment->SetSourceEnd(pElement->GetIntValue(_T("source-end")));
                previewSegment->SetAudioOffset(pElement->GetIntValue(_T("source-offset")));
                m_previewList.Add(previewSegment);
            }
        }

        fCurrentProgress += fProgressStep;
        if (pListener != NULL)
            pListener->SetProgress(fCurrentProgress);
    }

return true;
}

bool CEditorProject::ParseVideoStream(CLepSgml *pLepFile, LPCTSTR tszLepFileName) { 
    if (pLepFile == NULL)
        return false;

    // read video information
    SgmlElement *pVideoStreamTag = pLepFile->Find(_T("video"), _T("streams"));
    if (pVideoStreamTag == NULL)
        return true;

    CArray <SgmlElement *, SgmlElement *> aElements;
    pVideoStreamTag->GetElements(aElements);

    for (int i = 0; i < aElements.GetSize(); ++i) {
        SgmlElement *pElement = aElements[i];
        if (pElement != NULL) {
            if (_tcscmp(pElement->GetName(), _T("source")) == 0)
            {
                int streamID = pElement->GetIntValue(_T("id"));
                int videoStreamID = pElement->GetIntValue(_T("stream-id"));
                CStreamContainer *pContainer = FindStreamContainer(streamID);
                if (!pContainer) {
                    pContainer = new CStreamContainer();
                    pContainer->SetID(streamID);
                    if (CStreamContainer::m_nContainerCounter < streamID)
                        CStreamContainer::m_nContainerCounter = streamID;
                    m_streamList.AddTail(pContainer);
                }

                if (!pContainer->GetVideoStream())
                {
                    CVideoStream *videoStream = FindVideoStream(videoStreamID);
                    if (!videoStream) {
                        int nStreamSelector = pElement->GetIntValue(_T("stream-selector"));
                        CString videoFilename = MakeAbsoluteAndCheck(tszLepFileName, pElement->GetValue(_T("file")));
                        int videoOffset = pElement->GetIntValue(_T("offset"));
                        videoStream = new CVideoStream();
                        videoStream->SetFilename(videoFilename, nStreamSelector);
                        videoStream->SetOffset(videoOffset);
                        // tripple code:
                        int videoLength;
                        if (!VideoInformation::GetVideoDuration(videoStream->GetFilename(), videoLength)) {
                            HWND hWnd = NULL;
                            if (m_pProgress)
                                hWnd = m_pProgress->GetSafeHwnd();
                            CString msg;
                            msg.Format(IDS_MSG_GET_VIDEO_LENGTH_FAILED, videoStream->GetFilename());
                            CEditorDoc::ShowWarningMessage(msg, hWnd);
                        }
                        else
                            videoStream->SetLength(videoLength);
                        videoStream->SetID(videoStreamID);
                        if (CStreamContainer::m_iVideoStreamCount < videoStreamID)
                            CStreamContainer::m_iVideoStreamCount = videoStreamID;
                    }
                    pContainer->SetVideoStream(videoStream);
                }

                int targetBegin = pElement->GetIntValue(_T("stream-start"));
                CPreviewSegment *previewSegment = FindPreviewSegment(targetBegin);
                if (previewSegment) {
                    if (!previewSegment->GetStreams())
                        previewSegment->SetStreams(pContainer);
                    previewSegment->SetVideoOffset(pElement->GetIntValue(_T("source-offset")));
                } else {
                    previewSegment = new CPreviewSegment();
                    previewSegment->SetStreams(pContainer);
                    previewSegment->SetTargetBegin(targetBegin);
                    previewSegment->SetTargetEnd(pElement->GetIntValue(_T("stream-end")));
                    previewSegment->SetSourceBegin(pElement->GetIntValue(_T("source-start")));
                    previewSegment->SetSourceEnd(pElement->GetIntValue(_T("source-end")));
                    previewSegment->SetVideoOffset(pElement->GetIntValue(_T("source-offset")));
                    m_previewList.Add(previewSegment);
                }
            }
        }
    }

    return true;
}

bool CEditorProject::ParseWbStream(CLepSgml *pLepFile, LPCTSTR tszLepFileName) { 
    if (pLepFile == NULL)
        return false;

   SgmlElement *pWbTag = pLepFile->Find(_T("wb"), _T("streams"));
   if (pWbTag == NULL)
       return true;

   CArray<SgmlElement *, SgmlElement *> aElements;
   pWbTag->GetElements(aElements);
   for (int i = 0; i < aElements.GetCount(); ++i) {
       SgmlElement *pElement = aElements[i];
       if (pElement != NULL) {
           if (_tcscmp(pElement->GetName(), _T("targetformat")) == 0) {
               CArray<SgmlElement *, SgmlElement *> aTargetFormats;
               pElement->GetElements(aTargetFormats);
               for (int j = 0; j < aTargetFormats.GetCount(); ++j) {
                   SgmlElement *pFormatTag = aTargetFormats[j];
                   if (_tcscmp(pFormatTag->GetName(),_T("pagedimension")) == 0) {
                       if (pFormatTag->GetParameter()) {
                           CString csWidth = pFormatTag->GetParameter();
                           int pos = csWidth.Find(_T('x'));
                           csWidth.Delete(pos+1, (csWidth.GetLength() - (pos+1)));
                           m_targetPageWidth = _ttoi(csWidth);

                           CString csHeight = pFormatTag->GetParameter();
                           pos = csHeight.Find(_T('x'));
                           csHeight.Delete(0, pos+1);
                           m_targetPageHeight = _ttoi(csHeight);
                       }
                   }
               }
           }
           else if (_tcscmp(pElement->GetName(), _T("source")) == 0) {
               int streamID = pElement->GetIntValue(_T("id"));
               int wbStreamID = pElement->GetIntValue(_T("stream-id"));
               CStreamContainer *pContainer = FindStreamContainer(streamID);
               if (!pContainer) {
                   pContainer = new CStreamContainer();
                   pContainer->SetID(streamID);
                   if (CStreamContainer::m_nContainerCounter < streamID)
                       CStreamContainer::m_nContainerCounter = streamID;
                   m_streamList.AddTail(pContainer);
               }

               // find filename of evq, obj and lmd
               CString evqFilename;
               CString objFilename;
               CString metadataFilename;
               CArray<SgmlElement *, SgmlElement *> aFileElements;
               pElement->GetElements(aFileElements);
               for (int j = 0; j < aFileElements.GetCount(); ++j) {
                   SgmlElement *pFileTag = aFileElements[j];
                   if (pFileTag != NULL) {
                       if (_tcscmp(pFileTag->GetName(), _T("evq")) == 0)
                           evqFilename = MakeAbsoluteAndCheck(tszLepFileName, pFileTag->GetParameter());
                       else if (_tcscmp(pFileTag->GetName(), _T("obj")) == 0)
                           objFilename = MakeAbsoluteAndCheck(tszLepFileName, pFileTag->GetParameter());
                       else if (_tcscmp(pFileTag->GetName(), _T("lmd")) == 0)
                           metadataFilename = MakeAbsoluteAndCheck(tszLepFileName, pFileTag->GetParameter());
                   }
               }

               if (!evqFilename.IsEmpty() && !objFilename.IsEmpty() && !pContainer->GetWhiteboardStream()) {
                   CWhiteboardStream *wbStream = FindWhiteboardStream(wbStreamID);
                   if (!wbStream) {
                       wbStream = new CWhiteboardStream();
                       wbStream->SetEvqFilename(evqFilename);
                       wbStream->SetObjFilename(objFilename);
                       wbStream->SetID(wbStreamID);
                       if (CStreamContainer::m_iWhiteboardStreamCount < wbStreamID)
                           CStreamContainer::m_iWhiteboardStreamCount = wbStreamID;
                       wbStream->Open();

                       // Note: marks and interaction areas are also read here.
                       // However they are not extracted from the CWhiteboardStream
                       // (like in InsertLrd()).
                       // They are handled by their own streams entry in the LEP.

                       // TODO you could extract them anyway to relese the storage.
                       // The CWhiteboardStream object isn't delete now/here...
                   }

                   CPageStream *pageStream = FindPageStream(wbStreamID);
                   if (!pageStream)
                   {
                       pageStream = wbStream->CreatePageStreamFromEvents();
                       if (pageStream)
                       {
                           pageStream->SetID(wbStreamID);
                           if (CStreamContainer::m_iPageStreamCount < pageStream->GetID())
                               CStreamContainer::m_iPageStreamCount = pageStream->GetID();
                       }
                   }

                   pContainer->SetPageStream(pageStream);
                   pContainer->SetWhiteboardStream(wbStream);
               }

               if (!metadataFilename.IsEmpty() && !pContainer->GetMetadata())
               {
                   CMetadataStream *metadata = FindMetadata(wbStreamID);

                   if (!metadata)
                   {
                       metadata = new CMetadataStream();
                       metadata->SetFilename(metadataFilename);
                       metadata->SetID(wbStreamID);
                       if (CStreamContainer::m_iMetadataCount < wbStreamID)
                           CStreamContainer::m_iMetadataCount = wbStreamID;
                       metadata->Open();
                   }

                   if (metadata != NULL && metadata->GetFileInformation() != NULL)
                       SetShowClickPages(metadata->GetFileInformation()->ShowClickPages());

                   pContainer->SetMetadata(metadata);
               }

               int targetBegin = pElement->GetIntValue(_T("stream-start"));
               CPreviewSegment *previewSegment = FindPreviewSegment(targetBegin);
               if (previewSegment) {
                   if (!previewSegment->GetStreams())
                       previewSegment->SetStreams(pContainer);
               } else {
                   previewSegment = new CPreviewSegment();
                   previewSegment->SetStreams(pContainer);
                   previewSegment->SetTargetBegin(targetBegin);
                   previewSegment->SetTargetEnd(pElement->GetIntValue(_T("stream-end")));
                   previewSegment->SetSourceBegin(pElement->GetIntValue(_T("source-start")));
                   previewSegment->SetSourceEnd(pElement->GetIntValue(_T("source-end")));
                   m_previewList.Add(previewSegment);
               }
           }
       }
   }

   return true;
}

bool CEditorProject::ParseClipStream(CLepSgml *pLepFile, LPCTSTR tszLepFileName) {
    if (pLepFile == NULL)
        return false;

    CArray<CString, CString> issuedErrorFiles;

    SgmlElement *pSgTag = pLepFile->Find(_T("sg"), _T("streams"));
    if (pSgTag == NULL)
        return true;

    CArray<SgmlElement *, SgmlElement *> aElements;
    pSgTag->GetElements(aElements);

    for (int i = 0; i < aElements.GetSize(); ++i) {
        SgmlElement *pElement = aElements[i];
        if (pElement != NULL) {
            if (_tcscmp(pElement->GetName(), _T("clip")) == 0) {
                int streamID = pElement->GetIntValue(_T("id"));
                int clipStreamID = pElement->GetIntValue(_T("stream-id"));
                CStreamContainer *pContainer = FindStreamContainer(streamID);
                if (!pContainer) {
                    pContainer = new CStreamContainer();
                    pContainer->SetID(streamID);
                    if (CStreamContainer::m_nContainerCounter < streamID)
                        CStreamContainer::m_nContainerCounter = streamID;
                    m_streamList.AddTail(pContainer);
                }
                CArray<SgmlElement *, SgmlElement *> aSourceElements;
                pElement->GetElements(aSourceElements);
                for (int j = 0; j < aSourceElements.GetSize(); ++j) {
                    SgmlElement *pSourceTag = aSourceElements[j];
                    if (pSourceTag != NULL) {
                        CString clipFilename = MakeAbsoluteAndCheck(tszLepFileName, pSourceTag->GetValue(_T("file")));
                        int clipStart = pSourceTag->GetIntValue(_T("clip-start"));
                        int clipLength = pSourceTag->GetIntValue(_T("clip-end")) - clipStart;

                        if (!pContainer->GetClipStream())
                        {

                            CClipStream *clipStream = FindClipStream(clipStreamID);
                            if (!clipStream)
                            {
                                clipStream = new CClipStream();
                                clipStream->SetID(clipStreamID);
                                if (CStreamContainer::m_iClipStreamCount < clipStreamID)
                                    CStreamContainer::m_iClipStreamCount = clipStreamID;
                            }

                            pContainer->SetClipStream(clipStream);
                        }

                        CClipInfo *clip = pContainer->GetClipStream()->FindClip(clipFilename, clipStart, clipLength);
                        if (!clip)
                        {
                            CString csTitle;
                            csTitle = pSourceTag->GetValue(_T("title"));
                            CString csKeywords;
                            csKeywords = pSourceTag->GetValue(_T("keywords"));
                            int nStreamSelector = pSourceTag->GetIntValue(_T("stream-selector"));
                            int nAudioStreamSelector = -1;
                            CString csAudioStreamFile = _T("");
                            if (NULL != pSourceTag->GetValue(_T("audio-stream-selector")))
                                nAudioStreamSelector = pSourceTag->GetIntValue(_T("audio-stream-selector"));
                            if (NULL != pSourceTag->GetValue(_T("audio-file")))
                                csAudioStreamFile = MakeAbsolute(tszLepFileName, pSourceTag->GetValue(_T("audio-file")));
                            CString csOrigAudioStreamFile = _T("");
                            int nOrigAudioStreamSelector = -1;
                            if (NULL != pSourceTag->GetValue(_T("orig-audio-file")))
                                csOrigAudioStreamFile = MakeAbsolute(tszLepFileName, pSourceTag->GetValue(_T("orig-audio-file")));
                            if (NULL != pSourceTag->GetValue(_T("orig-audio-stream-selector")))
                                nOrigAudioStreamSelector = pSourceTag->GetIntValue(_T("orig-audio-stream-selector"));

                            CString csOrigAudioStreamFileBackup = csOrigAudioStreamFile;
                            bool success = true;
                            if (!csAudioStreamFile.IsEmpty() && nAudioStreamSelector >= 0)
                            {
                                // If this clip has a temporary audio file, check that
                                // it's still there. If it's not, restore it from the original
                                // file and stream.
                                success = CheckAndRestoreAudioFile(csAudioStreamFile, nAudioStreamSelector,
                                    csOrigAudioStreamFile, nOrigAudioStreamSelector);
                                if (!success)
                                {
                                    // Remove audio from this clip
                                    csAudioStreamFile = _T("");
                                    nAudioStreamSelector = -1;
                                    csOrigAudioStreamFile = _T("");
                                    nOrigAudioStreamSelector = -1;

                                    // This a change to the document!
                                    m_pDoc->SetModifiedFlag();
                                    m_pDoc->SetDirtyDuringOpen(true);
                                }
                            }

                            clip = new CClipInfo();

                            clip->SetFilename(clipFilename, nStreamSelector);
                            clip->SetTimestamp(clipStart);
                            clip->SetLength(clipLength);
                            clip->SetTitle(csTitle);
                            clip->SetKeywords(csKeywords);
                            clip->SetAudioFilename(csAudioStreamFile);
                            clip->SetAudioStreamSelector(nAudioStreamSelector);
                            clip->SetOriginalAudioSource(csOrigAudioStreamFile);
                            clip->SetOriginalAudioSourceStream(nOrigAudioStreamSelector);

                            pContainer->GetClipStream()->AddClip(clip);

                            if (!success)
                            {
                                // Have we output an error message for this file before?
                                bool bFound = false;
                                for (int nFile=0; nFile<issuedErrorFiles.GetSize(); ++nFile)
                                {
                                    if (issuedErrorFiles[nFile] == csOrigAudioStreamFile)
                                        bFound = true;
                                }

                                if (!bFound)
                                {
                                    issuedErrorFiles.Add(csOrigAudioStreamFile);
                                    CString csMsg;
                                    csMsg.Format(IDS_MSG_AUDIOSOURCE_CLIP_NOTFOUND, csOrigAudioStreamFileBackup, clipFilename);
                                    CEditorDoc::ShowWarningMessage(csMsg);
                                }
                            }
                        }
                        if (clip && pSourceTag->GetParameter())
                            clip->SetTitle(pSourceTag->GetParameter());
                        clipFilename.Empty();
                    }
                }

                int targetBegin = pElement->GetIntValue(_T("stream-start"));
                CPreviewSegment *previewSegment = FindPreviewSegment(targetBegin);
                if (previewSegment) {
                    if (!previewSegment->GetStreams())
                        previewSegment->SetStreams(pContainer);
                } else {
                    previewSegment = new CPreviewSegment();
                    previewSegment->SetStreams(pContainer);
                    previewSegment->SetTargetBegin(targetBegin);
                    previewSegment->SetTargetEnd(pElement->GetIntValue(_T("stream-end")));
                    previewSegment->SetSourceBegin(pElement->GetIntValue(_T("source-start")));
                    previewSegment->SetSourceEnd(pElement->GetIntValue(_T("source-end")));
                    m_previewList.Add(previewSegment);
                }
            }
        } 
    }

    return true;
}

bool CEditorProject::ParseSlideStream(CLepSgml *pLepFile, LPCTSTR tszLepFileName) {
    if (pLepFile == NULL)
        return false;

    SgmlElement *pSlideTag = pLepFile->Find(_T("slides"), _T("streams"));
    if (pSlideTag == NULL)
        return true;

    CArray<CString, CString> issuedErrorFiles;

    // read clip information
    CArray<SgmlElement *, SgmlElement *> aElements;
    pSlideTag->GetElements(aElements);
    for (int i = 0; i < aElements.GetCount(); ++i) {
        SgmlElement *pElement = aElements[i];
        if (pElement != NULL) {
            if (_tcscmp(pElement->GetName(), _T("slide")) == 0)
            {
                int streamID = pElement->GetIntValue(_T("id"));
                int pageStreamID = pElement->GetIntValue(_T("stream-id"));
                CStreamContainer *pContainer = FindStreamContainer(streamID);
                if (!pContainer) {
                    pContainer = new CStreamContainer();
                    pContainer->SetID(streamID);
                    if (CStreamContainer::m_nContainerCounter < streamID)
                        CStreamContainer::m_nContainerCounter = streamID;
                    m_streamList.AddTail(pContainer);
                }

                // do not use page stream created from eventqueue
                if (pContainer->GetPageStream() && pContainer->GetPageStream()->GetID() != pageStreamID)
                    pContainer->SetPageStream(NULL);

                CArray<SgmlElement *, SgmlElement *> aSourceElements;
                pElement->GetElements(aSourceElements);
                for (int j = 0; j < aSourceElements.GetCount(); ++j) {
                    SgmlElement *pSourceElement = aSourceElements[j];
                    if (pSourceElement != NULL) {
                        int pageStart = pSourceElement->GetIntValue(_T("slide-start"));
                        int pageEnd = pSourceElement->GetIntValue(_T("slide-end"));

                        if (!pContainer->GetPageStream()) {
                            CPageStream *pageStream = FindPageStream(pageStreamID);
                            if (!pageStream) {
                                pageStream = new CPageStream();
                                pageStream->SetID(pageStreamID);
                                if (CStreamContainer::m_iPageStreamCount < pageStreamID)
                                    CStreamContainer::m_iPageStreamCount = pageStreamID;

                                if (pContainer->GetMetadata()) {
                                    CMetadataStream *pMetadata = pContainer->GetMetadata()->Copy();
                                    pMetadata->SetID(++CStreamContainer::m_iMetadataCount);
                                    pContainer->SetMetadata(pMetadata);
                                }
                            }

                            pContainer->SetPageStream(pageStream);
                        }

                        CPage *page = pContainer->GetPageStream()->FindPage(pageStart);
                        CString csTitle;
                        csTitle = pSourceElement->GetValue(_T("title"));
                        CString csKeywords;
                        csKeywords = pSourceElement->GetValue(_T("keywords"));
                        int nPageNumber = pSourceElement->GetIntValue(_T("number"));
                        int nPageId = pSourceElement->GetIntValue(_T("id"));
                        bool bHidePointers = pSourceElement->GetBoolValue(_T("hide-pointers"));
                        if (!page) {
                            page = new CPage();

                            page->SetBegin(pageStart);
                            page->SetEnd(pageEnd);
                            page->SetTitle(csTitle);
                            page->SetKeywords(csKeywords);
                            page->SetPageNumber(nPageNumber);
                            page->SetJoinId(nPageId);
                            page->SetHidePointers(bHidePointers);
                            pContainer->GetPageStream()->Add(page);
                        } else {
                            page->SetTitle(csTitle);
                            page->SetKeywords(csKeywords);
                            page->SetHidePointers(bHidePointers);
                        }
                    }
                }

                int targetBegin = pElement->GetIntValue(_T("stream-start"));
                CPreviewSegment *previewSegment = FindPreviewSegment(targetBegin);
                if (previewSegment) {
                    if (!previewSegment->GetStreams())
                        previewSegment->SetStreams(pContainer);
                    pContainer->GetMetadata()->UpdateMetadata(pContainer->GetPageStream());
                } else {
                    previewSegment = new CPreviewSegment();
                    previewSegment->SetStreams(pContainer);
                    previewSegment->SetTargetBegin(targetBegin);
                    previewSegment->SetTargetEnd(pElement->GetIntValue(_T("stream-end")));
                    previewSegment->SetSourceBegin(pElement->GetIntValue(_T("source-start")));
                    previewSegment->SetSourceEnd(pElement->GetIntValue(_T("source-end")));
                    m_previewList.Add(previewSegment);
                }
            } 
        }
    }

    return true;
}

bool CEditorProject::SetThumbWidth(int width)
{
   if (m_nThumbWidth != width)
   {
      m_nThumbWidth = width;
      m_targetStructure.ResetClipThumb();
      return true;
   }
   
   return false;
}


bool CEditorProject::HasStructure() {
   return !m_targetStructure.IsEmpty();
}

bool CEditorProject::HasMetadata()
{
   for (int i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *segment = m_previewList[i];
      if (segment)
      {
         if (segment->GetMetadata())
            return true;
      }
   }
   
   return false;
}

bool CEditorProject::HasMarks() {
    return m_pMarkStream != NULL && (m_pMarkStream->GetMarksCount() > 0);
}

bool CEditorProject::HasVideo()
{
   for (int i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *segment = m_previewList[i];
      if (segment)
      {
         if (segment->GetVideoStream())
            return true;
      }
   }
   
   return false;
}

bool CEditorProject::HasSgClips()
{
   return GetClipStream() != NULL && !GetClipStream()->IsEmpty();
}

bool CEditorProject::HasStructuredSgClips()
{
   return GetClipStream() != NULL && GetClipStream()->HasStructuredClips();
}

bool CEditorProject::IsDenverDocument() {
    if (m_previewList.GetSize() == 0)
        return false;

    for (int i = 0; i < m_previewList.GetSize(); ++i) {
        CPreviewSegment *segment = m_previewList[i];
        if (segment && segment->GetWhiteboardStream() != NULL) {
            return false;
        }
    }

    return true;
}

void CEditorProject::SetShowClickPages(bool bShowClickPages, bool bRedraw) {
    m_bShowClickPages = bShowClickPages;

    if (bRedraw) {
        UpdateStructureAndStreams();
        m_pDoc->UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);
        m_pDoc->SetModifiedFlag(TRUE);
    }
}

bool CEditorProject::IsClipWithAudioInsertion(UINT nClickPosMs)
{
   CClipStreamR *pClipStream = GetClipStream();
   if (pClipStream != NULL)
   {
      if (pClipStream->HasClipAudioAt(nClickPosMs))
      {
         UINT nClipStartMs = 0;
         UINT nClipLengthMs = 0;
         pClipStream->GetClipTimesAt(nClickPosMs, &nClipStartMs, &nClipLengthMs);

         if (StreamsAreEmpty(nClipStartMs, nClipStartMs + nClipLengthMs))
         {
            return true;
         }
      }
   }

   return false;
}

bool CEditorProject::StreamsAreEmpty(int nFromMs, int nToMs)
{
   int nFirstSegment = FindElementAt(nFromMs);
   for (int i = nFirstSegment; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *pSegment = m_previewList[i];
      if (pSegment && pSegment->GetTargetBegin() < nToMs)
      {
         if (pSegment->GetWhiteboardStream() != NULL ||
             pSegment->GetVideoStream() != NULL ||
             !pSegment->GetAudioStream()->IsDummyStream())
             return false;
      }
      if (pSegment->GetTargetEnd() >= nToMs)
         break;
   }
   return true;
}

int CEditorProject::GetTotalLength()
{
   int totalLength = 0;
   
   for (int i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *segment = m_previewList[i];
      if (segment)
      {
         totalLength += segment->GetTargetEnd() - segment->GetTargetBegin();
      }
   }
   
   return totalLength;
}

UINT CEditorProject::GetAudioPreviewLength() {
    int iAudioPreviewLength =  m_avEdit.GetAudioPreviewLength();
    if (iAudioPreviewLength == 0)
        iAudioPreviewLength = GetTotalLength();

    return iAudioPreviewLength;
}

int CEditorProject::FindElementAt(int timestamp)
{
   for (int i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *segment = m_previewList[i];
      if (segment)
      {
         if (segment->GetTargetEnd() > timestamp)
            return i;
      }
   }
   
   return m_previewList.GetSize()-1;
}

void CEditorProject::GetPreviewSegments(CArray<CPreviewSegment *, CPreviewSegment *> &arPreviewSegments, int fromMsec, int toMsec)
{
   for (int i = 0; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *pSegment = m_previewList[i];
      if (pSegment &&
         (pSegment->GetTargetEnd() > fromMsec && pSegment->GetTargetEnd() <= toMsec) ||
         (pSegment->GetTargetBegin() >= fromMsec &&  pSegment->GetTargetBegin() < toMsec) ||
         (pSegment->GetTargetBegin() < fromMsec && pSegment->GetTargetEnd() > toMsec))
      {
         arPreviewSegments.Add(pSegment);
      }
   }
   return;
}


CClipInfo *CEditorProject::FindClipAt(int timestamp)
{
   for (int i = 0; i < m_clipStream.GetSize(); ++i)
   {
      CClipInfo *pClipInfo = m_clipStream[i];
      if (pClipInfo && 
         pClipInfo->GetTimestamp() <= timestamp && 
         pClipInfo->GetTimestamp() + pClipInfo->GetLength() >= timestamp)
         return pClipInfo;
   }
   
   return NULL;
}


void CEditorProject::IncrementFrom(int insertIndex, int byLength)
{
   for (int i = insertIndex; i < m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *part = m_previewList[i];
      if (part)
      {
         part->IncrementTargetBegin(byLength);
         part->IncrementTargetEnd(byLength);
      }
   }
}

void CEditorProject::UpdateStructureAndStreams()
{
   // Note: there are in total three structures for storing page informations:
   // - the CPageStream objects in the respective preview segments
   // - the array m_aPageStream of CPage objects (created below)
   // - and the tree structure starting with m_targetStructure 
   //   containing CPageInformation objects (created and updated below)
   // TODO make this only one structure.


   // delete page stream
   for (int i = 0; i < m_aPageStream.GetSize(); ++i)
   {
      if (m_aPageStream[i])
         delete m_aPageStream[i];
   }
   m_aPageStream.RemoveAll();
   
   // delete clip stream 
   for (i = 0; i < m_clipStream.GetSize(); ++i)
   {
      if (m_clipStream[i])
         delete m_clipStream[i];
   }
   m_clipStream.RemoveAll();
   
   // delete structure
   m_targetStructure.Clear();
   
   
   // if preview list is empty nothing to do
   if (m_previewList.GetSize() == 0)
      return;
   
   //
   // update page stream
   //
   CArray<CPage *, CPage *> aPageSegments;
   aPageSegments.SetSize(0, 40);
   
   // gather page segments
   for (i = 0; i < m_previewList.GetSize(); ++i)
   {
      // this compiles an array of consecutive CPage objects
      // one page (with the same id) can occur several times 
      // according to the boundaries of preview segments

      CPreviewSegment *segment = m_previewList[i];

      int from   = segment->GetTargetBegin();
      int to     = segment->GetTargetEnd();
      int offset = segment->GetTargetBegin() - segment->GetSourceBegin();
      
      CPageStream *pPageStream = segment->GetPageStream();
      if (pPageStream)
      {
         int firstPos = -1;
         int lastPos  = -1;
         pPageStream->GetPages(aPageSegments, from, to, offset, firstPos, lastPos);
         
         // confine pages to the segment boundaries
         if ((firstPos >= 0) && (aPageSegments[firstPos]->GetBegin() != segment->GetTargetBegin()))
         {
            aPageSegments[firstPos]->SetBegin(segment->GetTargetBegin());
         }   
         if ((lastPos >= 0) && (aPageSegments[lastPos]->GetEnd() != segment->GetTargetEnd()))
         {
            aPageSegments[lastPos]->SetEnd(segment->GetTargetEnd());
         }
      }
   }

   // consolidate page segments
   CArray<bool, bool> aPageSegmentMoved;

   if (aPageSegments.GetSize() > 0)
   {
      aPageSegmentMoved.InsertAt(0, false, aPageSegments.GetSize()); // initialize with false

      m_aPageStream.Add(aPageSegments[0]);
      aPageSegmentMoved.SetAt(0, true); // at least the first element is used

      //m_aPageStream[0]->SetBegin(0); // just to make sure
   }

   for (i=1; i<aPageSegments.GetSize(); ++i)
   {
      if (aPageSegments[i]->GetJoinId() != aPageSegments[i-1]->GetJoinId())
      {
         m_aPageStream.Add(aPageSegments[i]);
         aPageSegmentMoved.SetAt(i, true); 
      }
      else
      {
         m_aPageStream[m_aPageStream.GetSize() - 1]->SetEnd(aPageSegments[i]->GetEnd());
      }
   }

   if (m_aPageStream.GetSize() > 0)
      m_aPageStream[m_aPageStream.GetSize() - 1]->SetEnd(GetTotalLength());


   for (i=0; i<aPageSegments.GetSize(); ++i)
   {
      if (!aPageSegmentMoved[i])
         delete aPageSegments[i];
   }

   // update join id of pages: after the joining above: 
   // no independent (and old) page part should have the same id
   CMap<int, int, int, int> mapJoinIds;
   mapJoinIds.InitHashTable(101); // prime number size
   
   for (i=0; i<m_aPageStream.GetSize(); ++i)
   {
      bool bIsNewlyAdded = false;
      
      CPage *pPage = m_aPageStream[i];
      
      if (m_iLastPasteLength > 0)
      {
         if (pPage->GetBegin() >= m_iLastPasteTo && pPage->GetBegin() < m_iLastPasteTo + m_iLastPasteLength)
         {
            // these are new page segements;
            bIsNewlyAdded = true;
         }
      }

      if (!bIsNewlyAdded)
      {
         int iUsed = 0;
         if (mapJoinIds.Lookup(pPage->GetJoinId(), iUsed))
         {
            pPage->SetJoinId(++CWhiteboardStream::m_pageCounter); // it should have a different id
         }
      }
      else
      {
         pPage->SetJoinId(++CWhiteboardStream::m_pageCounter);
      }

      mapJoinIds.SetAt(pPage->GetJoinId(), 1);
   }      

   // reset: they were considered above
   m_iLastPasteTo = 0;
   m_iLastPasteLength = 0;
   

   CClipStreamR *pClipStream = GetClipStream();
   if (pClipStream != NULL)
   {
      // like with the CPage array above:
      // an array of clip segments; the same clip (with different timestamps)
      // can occur several times

      CArray<CClipSegment, CClipSegment> *pClipSegments = pClipStream->GetSegments();

      for (int i=0; i<pClipSegments->GetSize(); ++i)
      {
         CClipInfo *pClipInfo = new CClipInfo;
         pClipSegments->GetAt(i).FillClipInfo(pClipInfo);
         m_clipStream.Add(pClipInfo);

         // TODO copy needed or is a pointer sufficient?
         // TODO even better of course: do not use GetSegments() but do this in CClipStreamR
      }
   }

   // update structure information
   for (i = 0; i < m_previewList.GetSize(); ++i)
   {
      // builds a tree like structure from pages (CPageInformation) 
      // and clips (CClipInformation) and the enclosing chapters (CChapterInformation)
      // m_targetStructure is the root node of that tree

      CPreviewSegment *segment = m_previewList[i];
      if (segment)
         segment->CompleteStructureInfo(m_targetStructure, m_clipStream);
   }

   // remove all marks from end of page before recreating click pages
   /*if (m_pDoc->IsDemoDocument() && m_pDoc->m_docLengthMs > 0) {
        m_pMarkStream->RemoveAllEndOfPageStopMarks();
   }*/

   if (m_pTargetStructureWithClickPages != NULL) 
       delete m_pTargetStructureWithClickPages;
   m_pTargetStructureWithClickPages = NULL;

   if (IsDenverDocument() || HasStructuredSgClips()) {
       if (ShowClickPages()) {
           m_targetStructure.UpdatePageTitles(m_aPageStream);
           m_bHasClickPages = m_targetStructure.HasClickPages();
           m_bHasAnalysisPages = m_targetStructure.HasClickPages(true);
       } else {
           m_pTargetStructureWithClickPages = m_targetStructure.Copy();
           m_targetStructure.HideClickPages(m_aPageStream);
           m_targetStructure.DeleteClickPages();
           m_bHasClickPages = m_pTargetStructureWithClickPages->HasClickPages();
           m_bHasAnalysisPages = m_pTargetStructureWithClickPages->HasClickPages(true);
       }
   }


   if (!m_targetStructure.IsEmpty())
   {
      // there should be no empty time between two chapters
      m_targetStructure.UpdateTimestamps(0);
      
      m_targetStructure.SetBegin(0);
      m_targetStructure.InsertMissingClips(m_clipStream);
      
      UpdatePageNumbers();
      // Also updates the unique ids if necessary; the content in m_targetStructure
      // is already joined so any new element with a duplicate id must get a new one.
   }
   else if (IsDenverDocument() && m_clipStream.GetSize() > 0)
   {
      int nDocumentLength = GetTotalLength();
      m_targetStructure.SetBegin(0);
      m_targetStructure.SetEnd(nDocumentLength);
      CClipInformation *pClip = new CClipInformation;
      CClipInfo *pFirstClip = m_clipStream[0];
      CString csKeywords;
      pFirstClip->GetKeywords(csKeywords);
      pClip->SetBegin(0);
      pClip->SetEnd(nDocumentLength);
      pClip->SetFilename(pFirstClip->GetFilename());
      pClip->SetKeywords(csKeywords);
      pClip->SetNumber(0);
      pClip->SetTitle(pFirstClip->GetTitle());
      m_targetStructure.InsertClip(pClip);
   }


   // For randomized questions remove all objects with action jump to make sure there is no invinite loop created.
   CQuestionnaireEx *pFirstQuestionnaire = GetFirstQuestionnaireEx();
   if (pFirstQuestionnaire && pFirstQuestionnaire->IsRandomTest()
       && pFirstQuestionnaire->GetQuestionTotal() > 0) 
       AddStopmarkAtEndOfEachQuestionPage();

   //m_targetStructure.Print();
 
   return;
}

// TODO remove this method if the audio length is always correct (and the assert never occurs).
void CEditorProject::VerifyLastPageEnd()
{
   UINT nAudioLengthMs = m_avEdit.GetAudioPreviewLength();

   if (nAudioLengthMs > 0 && m_aPageStream.GetSize() > 0)
   {
      CPage *pLastPage = m_aPageStream[m_aPageStream.GetSize() - 1];

      if (pLastPage->GetBegin() < (signed)nAudioLengthMs)
      {
         int iDiff = (pLastPage->GetEnd() - 1) - (signed)nAudioLengthMs;

         if (iDiff < -1 || iDiff > 1)
            _ASSERT(false); // shouldn't happen anymore
         // remove whole method if it doesn't happen
      }
   }
}

void CEditorProject::RemoveEmptyParts()
{
   
   int i = 0;
   while (i < m_previewList.GetSize())
   {
      CPreviewSegment *part = m_previewList.GetAt(i);
      if (part->GetTargetBegin() == part->GetTargetEnd())
      {
         delete part;
         m_previewList.RemoveAt(i, 1);
      }
      else 
         ++i;
   }
}

void CEditorProject::DeleteClipBoard(CArray<CPreviewSegment *, CPreviewSegment *> &clipBoard)
{
   for (int i = 0; i < clipBoard.GetSize(); ++i)
   {
      CPreviewSegment *part = clipBoard.GetAt(i);
      if (part)
         delete part;
   }
   
   clipBoard.RemoveAll();
}

bool CEditorProject::InitAviSettingsFromFile(CAviSettings *pAviSettings, const _TCHAR *tszFilename, 
                                             CVideoStream *pVideoStream/*=NULL*/)
{
   int nQuality = 75;
   int nKeyframes = 15;
   CString fccString(_T(""));
   
   if (pVideoStream)
   {
      nKeyframes = pVideoStream->GetKeyframes();
      if (nKeyframes == 0)
         nKeyframes = 15;
      nQuality = pVideoStream->GetQuality();
      if (nQuality == 0)
         nQuality = 75;
      fccString = pVideoStream->GetCodec();
   }
   
   AVInfo avInfo;
   HRESULT hr = avInfo.Initialize(tszFilename);
   long nStreams = 0;
   if (SUCCEEDED(hr))
      hr = avInfo.GetStreamCount(&nStreams);
   int nStreamId = 0;
   bool bFoundVideo = false;
   while (SUCCEEDED(hr) && nStreamId < nStreams && !bFoundVideo)
   {
      hr = avInfo.GetStreamFormat(nStreamId, &pAviSettings->videoFormat);
      if (SUCCEEDED(hr))
      {
         if (pAviSettings->videoFormat.majortype == MEDIATYPE_Video)
         {
            bFoundVideo = true;

            if (pAviSettings->videoFormat.formattype == FORMAT_VideoInfo &&
                pAviSettings->videoFormat.pbFormat)
            {
               VIDEOINFOHEADER *pVh = (VIDEOINFOHEADER *) pAviSettings->videoFormat.pbFormat;
               if (pVh->AvgTimePerFrame > 0i64)
               {
                  double dTicksPerFrame = (double) pVh->AvgTimePerFrame;
                  pAviSettings->dOriginalFps = 10000000.0 / dTicksPerFrame;

                  BITMAPINFOHEADER *pBi = HEADER(pVh);
                  pAviSettings->nOriginalWidth = pBi->biWidth;
                  pAviSettings->nOriginalHeight = pBi->biHeight;
                  pAviSettings->bHasOriginalSize = true;
               }
            }
         }
         else
         {
            nStreamId++;
            MYFREEMEDIATYPE(pAviSettings->videoFormat);
         }
      }
   }
   
   //if (SUCCEEDED(hr))
   //   hr = avInfo.GetStreamFormat(0, &pAviSettings->videoFormat);
   
   //CString fccString = pVideo->GetCodec();
   if (fccString.GetLength() >= 4)
   {
      pAviSettings->dwFCC = ((fccString[0]) |
         (fccString[1] << 8) |
         (fccString[2] << 16) |
         (fccString[3] << 24));
   }
   else
   {
      if (pAviSettings->videoFormat.formattype == FORMAT_VideoInfo)
      {
         BITMAPINFOHEADER *pBi = HEADER(pAviSettings->videoFormat.pbFormat);
         pAviSettings->dwFCC = pBi->biCompression;
      }
      else
      {
         pAviSettings->dwFCC = '05vi';
      }
   }
   
   pAviSettings->keyframeRate = nKeyframes;
   
   // Assume true, as this was not saved!
   pAviSettings->bUseKeyframes = true;
   pAviSettings->quality = nQuality;
   
   if (SUCCEEDED(hr))
      pAviSettings->bIsVideoFormatInitialized = true;
   else
      ZeroMemory(&pAviSettings->videoFormat, sizeof AM_MEDIA_TYPE);
   
   return pAviSettings->bIsVideoFormatInitialized;
}

CAviSettings *CEditorProject::LookupClipAviSettings(const _TCHAR *tszClipFilename, int nTempVolume)
{
   CAviSettings *pClipAviSettings = NULL;
   CString csClipSource(tszClipFilename);
   BOOL bFound = m_clipAviSettings.Lookup(csClipSource, pClipAviSettings);
   if (!bFound || !pClipAviSettings)
   {
      pClipAviSettings = new CAviSettings();
      InitAviSettingsFromFile(pClipAviSettings, csClipSource, NULL);
      if (nTempVolume >= 0)
         pClipAviSettings->nOriginalAudioVolume = nTempVolume;
      m_clipAviSettings.SetAt(csClipSource, pClipAviSettings);
   }

   return pClipAviSettings;
}

void CEditorProject::InitMediaTypes()
{
   // Let's see if we can find a preview segment
   // with a video and audio
   
   bool videoSegmentFound = m_videoAviSettings.bIsVideoFormatInitialized;
   if (!HasVideo())
      videoSegmentFound = true; // None exists, so don't look
   // If the audio format is already initialized, don't
   // overwrite it:
   bool audioSegmentFound = m_isAudioFormatInitialized;
   for (int i=0; i<m_previewList.GetSize() && (!videoSegmentFound || !audioSegmentFound); ++i)
   {
      CPreviewSegment *pSeg = m_previewList.GetAt(i);
      
      CVideoStream *pVideo = pSeg->GetVideoStream();
      if (pVideo && !videoSegmentFound)
      {
         bool success = InitAviSettingsFromFile(&m_videoAviSettings, pVideo->GetFilename(), pVideo);
         
         videoSegmentFound = true;
      }
      
      CAudioStream *pAudio = pSeg->GetAudioStream();
      if (pAudio && !audioSegmentFound)
      {
         AVInfo avInfo;
         HRESULT hr = avInfo.Initialize(pAudio->GetFilename());
           DWORD dwTime1 = ::timeGetTime();
         if (SUCCEEDED(hr))
            hr = avInfo.GetStreamFormat(0, &m_mtAudioFormat);

         if (SUCCEEDED(hr))
         {
            m_isAudioFormatInitialized = true;
            if (m_mtAudioFormat.majortype == MEDIATYPE_Audio &&
               m_mtAudioFormat.subtype == MEDIASUBTYPE_PCM &&
               m_mtAudioFormat.formattype == FORMAT_WaveFormatEx)
            {
               WAVEFORMATEX *pWfe = (WAVEFORMATEX *) m_mtAudioFormat.pbFormat;
               
               // Remove these four lines to reenable stereo
               pWfe->nChannels = 1;
               pWfe->nBlockAlign = pWfe->wBitsPerSample / 8;
               pWfe->nAvgBytesPerSec = pWfe->nSamplesPerSec * pWfe->nBlockAlign;
               m_mtAudioFormat.lSampleSize = pWfe->nBlockAlign;
               
               if (pWfe->nSamplesPerSec == 16000)
               {
                  pWfe->nSamplesPerSec = 22050;
                  pWfe->nAvgBytesPerSec = pWfe->nSamplesPerSec * pWfe->nBlockAlign;
               }
            }
         }
         else
            ZeroMemory(&m_mtAudioFormat, sizeof AM_MEDIA_TYPE);
         audioSegmentFound = true;
      }
   }
}

void CEditorProject::ResetPreviewFormat()
{
   if (m_videoAviSettings.bIsVideoFormatInitialized)
   {
      MYFREEMEDIATYPE(m_videoAviSettings.videoFormat);
      ZeroMemory(&m_videoAviSettings.videoFormat, sizeof AM_MEDIA_TYPE);
      m_videoAviSettings.bIsVideoFormatInitialized = FALSE;
   }
}

void CEditorProject::CreatePreviewFormat()
{
   ResetPreviewFormat();
   
   if (HasVideo())
   {
      InitMediaTypes();
   }
   else
   {
      if (HasSgClips())
      {
         
         int nWidth = 0;
         int nHeight = 0;
         
         bool bHasDenverFcc = false;
         DWORD dwDenverFcc = BI_RGB;
         
         REFERENCE_TIME rtMinFrameRate = 333333i64; // 30 fps
         
         CClipStreamR *pClipStream = GetClipStream();
         if (pClipStream != NULL)
         {
            
            CArray<CClipSegment, CClipSegment> *pSegments = pClipStream->GetSegments();
            
            for (int i=0; i<pSegments->GetSize(); ++i)
            {
               CClipSegment *pClipSegment = &pSegments->GetAt(i);
               
               CAviSettings *pAviSettings = LookupClipAviSettings(pClipSegment->GetFilename());
               
               if (pAviSettings->bIsVideoFormatInitialized)
               {
                  if (pAviSettings->videoFormat.formattype == FORMAT_VideoInfo)
                  {
                     VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *) pAviSettings->videoFormat.pbFormat;
                     int nThisWidth = pVih->bmiHeader.biWidth;
                     int nThisHeight = pVih->bmiHeader.biHeight;
                     REFERENCE_TIME rtThisFrameTime = pVih->AvgTimePerFrame;
                     
                     if (nThisWidth > nWidth)
                        nWidth = nThisWidth;
                     if (nThisHeight > nHeight)
                        nHeight = nThisHeight;
                     if (rtThisFrameTime > rtMinFrameRate)
                        rtMinFrameRate = rtThisFrameTime;
                     
                     if (IsDenverDocument() && !bHasDenverFcc)
                     {
                        dwDenverFcc = pVih->bmiHeader.biCompression;
                        bHasDenverFcc = true;
                     }
                  }
               }
            }
         }
         
         /*
         if (!exportClipList.IsEmpty())
         {
            POSITION pos = exportClipList.GetHeadPosition();
            while (pos)
            {
               CExportClip *pClip = exportClipList.GetNext(pos);
               if (pClip)
                  delete pClip;
            }
            exportClipList.RemoveAll();
         }
         */

         if (nWidth == 0)
            nWidth = 320;
         if (nHeight == 0)
            nHeight = 240;
         
         AVInfo avInfo;
         if (!IsDenverDocument())
         {
            avInfo.CreateStreamFormat(BI_RGB, nWidth, nHeight, &m_videoAviSettings.videoFormat);
            m_videoAviSettings.dwFCC = 0;
         }
         else
         {
            bool bResult = avInfo.CreateStreamFormat(dwDenverFcc, nWidth, nHeight, &m_videoAviSettings.videoFormat);
            
            //if (!bResult)
            //   bResult = avInfo.CreateStreamFormat(BI_RGB, nWidth, nHeight, &m_videoAviSettings.videoFormat);
            // Note: Do not create an uncompressed format here (tried for Bugfix #4593; lead to #4629)
            // as it will also be used for export of clips then/and upon "ok" in videoprops.cpp.

            m_videoAviSettings.dwFCC = dwDenverFcc;
         }

         ((VIDEOINFOHEADER *) m_videoAviSettings.videoFormat.pbFormat)->AvgTimePerFrame = rtMinFrameRate;
         m_videoAviSettings.bIsVideoFormatInitialized = TRUE;
      }
   }
}

void CEditorProject::RecalcDenverVideoParams()
{
   if (!IsDenverDocument())
   {
#ifdef _DEBUG
      MessageBox(::GetForegroundWindow(), _T("RecalcDenverVideoParams() was called with a non-Denver document"), _T("Debug"), MB_OK | MB_ICONWARNING);
#endif
      return;
   }

   // We need the ::CoInitialize() here because this method may
   // be called from an arbitrary thread
   HRESULT hr = ::CoInitialize(NULL);
   if (m_videoAviSettings.bIsVideoFormatInitialized)
   {
      if (m_clipExportAutoCalcDenverSize)
      {
         CAviSettings aviSettings;
         // Clone the entire CAviSettings structure, including the
         // format block. If you would do a simple '=' here, the destructor
         // of aviSettings would free the media type of m_videoAviSettings,
         // rendering an access violation as soon as m_videoAviSettings
         // is freed.
         // This was new in 1.7.0.p4 due to the fix of a memory leak 
         // (CAviSettings not freed).
         aviSettings.Clone(&m_videoAviSettings);
         ResetPreviewFormat();
         CreatePreviewFormat();
         m_videoAviSettings.bUseKeyframes = aviSettings.bUseKeyframes;
         m_videoAviSettings.bUseSmartRecompression = aviSettings.bUseSmartRecompression;
         m_videoAviSettings.dwFCC = aviSettings.dwFCC;
         m_videoAviSettings.bRememberAspectRatio = aviSettings.bRememberAspectRatio;
         m_videoAviSettings.keyframeRate = aviSettings.keyframeRate;
         m_videoAviSettings.nAudioVolume = aviSettings.nAudioVolume;
         m_videoAviSettings.quality = aviSettings.quality;

         VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *) m_videoAviSettings.videoFormat.pbFormat;
         pVih->bmiHeader.biCompression = aviSettings.dwFCC;
      }
      // else: Do not mind; leave everything as it is
   }
   else
   {
      CreatePreviewFormat();
   }
   ::CoUninitialize();
}


struct CLIPAUDIOSEG
{
   CLIPAUDIOSEG() : nTargetBegin(0), nTargetEnd(0), nOrigAudioVolume(100) {}
   CLIPAUDIOSEG(int nBegin, int nEnd, int nVolume)
   {
      nTargetBegin = nBegin;
      nTargetEnd = nEnd;
      nOrigAudioVolume = nVolume;
   }
   int nTargetBegin;
   int nTargetEnd;
   int nOrigAudioVolume;
};


int CEditorProject::FindStretchMode(CClipInfo *pClipInfo, bool bIsDenver)
{
   int iStretchMode = STRETCH_ASPECTRATIO;

   if (pClipInfo != NULL)
   {
      bool bCurrentClipIsSmaller = false;
      if (m_videoAviSettings.bIsVideoFormatInitialized)
      {
         VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *) m_videoAviSettings.videoFormat.pbFormat;
         int nWidth = pVih->bmiHeader.biWidth;
         int nHeight = pVih->bmiHeader.biHeight;
         
         CAviSettings *pSettings = LookupClipAviSettings(
            pClipInfo->GetFilename(), pClipInfo->GetTempOriginalAudioVolume());
         if (pSettings != NULL)
         {
            if (pSettings->bHasOriginalSize)
            {
               if (pSettings->nOriginalHeight <= nHeight &&
                  pSettings->nOriginalWidth <= nWidth)
                  bCurrentClipIsSmaller = true;
            }
         }
      }
      
      if (bIsDenver)
      {
         if (!m_clipExportStretchDenverClips && bCurrentClipIsSmaller)
            iStretchMode = STRETCH_CROP;
      }
      else
      {
         if (bCurrentClipIsSmaller)
            iStretchMode = STRETCH_CROP;
      }
   }
   
   return iStretchMode;
}

CString& CEditorProject::FindLadString()
{
   static CString csReturn;

   csReturn.Empty();

   CString csVersion;
   CSecurityUtils cSec;
   cSec.GetVersionStringShort(csVersion);
   
   CString csVersionType;
   switch (m_nVersionType)
   {
   case FULL_VERSION: 
      csVersionType = "f"; 
      break;
   case UNIVERSITY_VERSION: 
      csVersionType = "u"; 
      break;
   default: 
      csVersionType = "e"; 
      break;
   }
   CTime time = CTime::GetCurrentTime();
   CString csTime = time.Format("%H:%M:%S");
   CString csDate = time.Format("%d.%m.%Y");
   CString csLadInfo;
   csReturn.Format(_T("%s;%s;%s;%s"), csVersionType, csVersion, csTime, csDate);

   return csReturn;
}


// private
HRESULT CEditorProject::PrepareRenderGraph(bool bForPreview, LPCTSTR tszPrefix, bool bMainAudioOnly)
{
   // Note: Only called by ExportAudio() but it also creates and exports video.

   ASSERT(!IsEmpty());

   HWND hwndParent = NULL;
   if (m_pExportProgressDialog != NULL)
       hwndParent = m_pExportProgressDialog->GetSafeHwnd();

   if (!bForPreview && tszPrefix == NULL)
      return E_POINTER;

   bool bHasVideo = HasVideo();
   bool bHasSgClips = HasSgClips();
   bool bHasStill = HasStillImage();
   bool bIsDenver = IsDenverDocument();

   // REMARK: If there is no video view, but video or clips (iVideoCount > 0)
   // a video window will be automatically created by DXSDK
   CVideoView *pVideoView = NULL;
   if (m_pDoc != NULL)
       pVideoView = m_pDoc->GetVideoView();
      
   if (bForPreview && pVideoView != NULL) {
       pVideoView->SetIsNearlyOneClip(false);
       pVideoView->ClearClipTimes();
   }


   CArray<AVSource *, AVSource *> aAudioSources;
   CArray<AVSource *, AVSource *> aVideoSources;

   CMap<CString, LPCTSTR, UINT, UINT> mapVideoFileLengths;
 
   bool bSomeVideoFound = false;

   for (int i=0; i<m_previewList.GetSize(); ++i)
   {
      CPreviewSegment *segment = m_previewList[i];
   
      CAudioStream *pAudioStream = segment->GetAudioStream();
      CVideoStream *pVideoStream = segment->GetVideoStream();

      bool bHasAudio = true;
      bool bAllowNullAudio = false;
      int iAudioSelector = 0;
      CString csAudioFileName;

      if (pAudioStream != NULL && !pAudioStream->IsDummyStream())
      {
         csAudioFileName = pAudioStream->GetFilename();
         iAudioSelector = pAudioStream->GetStreamSelector();
      }
      else
      {
         bAllowNullAudio = true;
         bHasAudio = false;
      }

 
      // there is an audio source for every segment, it can be empty however

      AVSource *pAudioSource = new AVSource(
         bHasAudio ? (LPCTSTR)csAudioFileName : NULL,
         segment->GetTargetBegin(),
         segment->GetTargetEnd(),
         segment->GetSourceBegin() + segment->GetAudioOffset(),
         segment->GetSourceEnd() + segment->GetAudioOffset(),
         iAudioSelector, 0, 100, 100, bAllowNullAudio);
      aAudioSources.Add(pAudioSource);

      if (bHasVideo && !bMainAudioOnly)
      {
         if (pVideoStream != NULL)
         {
            CString csVideoFileName = pVideoStream->GetFilename();
            int iStreamSelector = pVideoStream->GetStreamSelector();
            int iSourceStart = 
               segment->GetSourceBegin() - pVideoStream->GetOffset() + segment->GetVideoOffset() - m_videoExportGlobalOffset;
            int iSourceEnd = 
               segment->GetSourceEnd() - pVideoStream->GetOffset() + segment->GetVideoOffset() - m_videoExportGlobalOffset;
            
            AVSource *pVideoSource = new AVSource(
               csVideoFileName,
               segment->GetTargetBegin(),
               segment->GetTargetEnd(),
               iSourceStart,
               iSourceEnd,
               iStreamSelector
               );
            aVideoSources.Add(pVideoSource);

            if (!bForPreview)
            {
               UINT nFoundVideoLength = 0;
               bool bFound = mapVideoFileLengths.Lookup(csVideoFileName, nFoundVideoLength) == TRUE;

               UINT nSegmentLength = segment->GetSourceEnd() - segment->GetSourceBegin();
               mapVideoFileLengths.SetAt(csVideoFileName, nSegmentLength + nFoundVideoLength);
            }

            bSomeVideoFound = true;
         }
         // else no video for this segment?
      }

   } // for all preview segments

   /* ignore still image here: it is handled by the CVideoView directly
   if (bHasStill && !bSomeVideoFound && bForPreview && !bMainAudioOnly)
   {
      // Note: video superseeds still image (they shouldn't be defined both 
      // at the same time anyway).
      // Still image is ignored in export case; it is handled (copied) elsewhere.
      
      UINT nStillImageLengthMs = GetTotalLength();
      if (nStillImageLengthMs < 61001) 
         nStillImageLengthMs = 61001; // very strang hack: otherwise nothing will be displayed (black)

      AVSource *pStillSource = new AVSource(
         m_csStillImage,
         0,
         nStillImageLengthMs,
         0,
         AV_LENGTH_SOURCE,
         0,
         bHasSgClips ? STRETCH_ASPECTRATIO : 0
         );
      aVideoSources.Add(pStillSource);

      //TRACE("still image for 0 - %d\n", GetTotalLength());
   }
   */
 
   if (bHasSgClips && !bMainAudioOnly)
   {
      // Note: clips are not segmented by m_previewList so they are handled
      // outside of the below loop.
      // In order to have them on-top of the video they are added after the video segments.
      // For export clips are ignored here and handled by ExportClips(). // TODO change?

      CClipStreamR *pClipStream = GetClipStream();
      
      if (pClipStream != NULL)
      {
         UINT nClipNr = 0;
         
         CClipInfo clipInfo;
         
         HRESULT hr = pClipStream->FillClipInfo(nClipNr, &clipInfo);

         while (SUCCEEDED(hr) && hr != S_FALSE)
         {
            int iStretchMode = FindStretchMode(&clipInfo, bIsDenver);

            int iTargetMs = clipInfo.GetTimestamp();
            int iTargetEnd = iTargetMs + clipInfo.GetLength();
            int iSourceMs = pClipStream->GetSourceBegin(nClipNr);
            int iSourceEnd = iSourceMs + clipInfo.GetLength();

            if (bForPreview || bIsDenver)
            {
               AVSource *pClipSource = new AVSource(
                  clipInfo.GetFilename(),
                  iTargetMs,
                  iTargetEnd,
                  iSourceMs,
                  iSourceEnd,
                  clipInfo.GetStreamSelector(),
                  iStretchMode
                  );
               aVideoSources.Add(pClipSource); // clips are on top of video (during preview): added after video
            }

            if (bForPreview && pVideoView != NULL)
               pVideoView->AddClipTimes(iTargetMs, iTargetEnd);


            if (clipInfo.HasAudioStream())
            {
               int nClipVolume = 100;
               int nOriginalVolume = 100;

               CAviSettings *pClipSettings = NULL;
               if (FALSE != m_clipAviSettings.Lookup(clipInfo.GetFilename(), pClipSettings))
               {
                  nClipVolume = pClipSettings->nAudioVolume;
                  nOriginalVolume = pClipSettings->nOriginalAudioVolume;
               }
               
               if (nClipVolume > 0 || nOriginalVolume != 100)
               {
                  AVSource *pClipAudioSource = new AVSource(
                     clipInfo.GetAudioFilename(),
                     iTargetMs,
                     iTargetEnd,
                     iSourceMs,
                     iSourceEnd,
                     clipInfo.GetAudioStreamSelector(),
                     0,
                     nClipVolume, nOriginalVolume); 
                  aAudioSources.Add(pClipAudioSource);
               }
            }

            ++nClipNr;
            hr = pClipStream->FillClipInfo(nClipNr, &clipInfo);
         }
      }
   }
  

   int iAudioCount = aAudioSources.GetSize();
   int iVideoCount = aVideoSources.GetSize();
   
   int iTargetCount = 1; // audio

   ASSERT(iAudioCount > 0); // There must be some input of some kind; otherwise iTargetCount = 1 is wrong

   if (iVideoCount > 0)
      iTargetCount++;
   
   AVTarget **paTargets = new AVTarget*[iTargetCount];
   DWORD dwTime1k5 = ::timeGetTime();
   if (!m_isAudioFormatInitialized || !m_videoAviSettings.bIsVideoFormatInitialized)
      InitMediaTypes();

   // TODO: bIsVideoFormatInitialized might still be false (silent error)
   //       this will fail in AVTarget.AddTo (AVInfo::StaticMakeUncompressed()) below
   // This is not sufficient as a check (what about clips)?
   //if (HasVideo() && !m_videoAviSettings.bIsVideoFormatInitialized

   AVSource **paAudioSources = new AVSource*[iAudioCount];
   if (iAudioCount > 0)
   {
      for (int i=0; i<iAudioCount; ++i)
         paAudioSources[i] = aAudioSources.GetAt(i);

      CString csAudioOutputName;

      if (!bForPreview)
         csAudioOutputName.Format(_T("%s.lad"), tszPrefix);

      if (bMainAudioOnly)
         csAudioOutputName = tszPrefix;

      AVTarget *pAudioTarget = new AVTarget(
         (const AVSource **)paAudioSources,
         iAudioCount,
         &m_mtAudioFormat,
         bForPreview ? NULL : (LPCTSTR)csAudioOutputName
         );

      if (!bForPreview && !bMainAudioOnly)
      {
         CString csLadInfo = FindLadString();

         int iLength = csLadInfo.GetLength();
         char szLadInfo[200]; // only plain ASCII data
         int iToCopy = iLength < 200-1 ? iLength : 200-1;
#ifdef _UNICODE
         ::WideCharToMultiByte(CP_ACP, 0, csLadInfo, iLength, szLadInfo, iToCopy, NULL, NULL);
#else
         strncpy(szLadInfo, csLadInfo, iLength);
#endif
         szLadInfo[iToCopy] = 0;

         pAudioTarget->SetLadInfo(szLadInfo);
      }
      
      paTargets[0] = pAudioTarget;
   }

   AVSource **paVideoSources = new AVSource*[iVideoCount];
   if (iVideoCount > 0)
   {
      for (int i=0; i<iVideoCount; ++i)
         paVideoSources[i] = aVideoSources.GetAt(i);
      
      if (bForPreview && !m_videoAviSettings.bIsVideoFormatInitialized)
      {
         // Let's create a preview format, even if we do not have a real video.
         CreatePreviewFormat();
      }
      // TODO what about failures here? Ie bIsVideoFormatInitialized could still be false.

      AVTarget *pVideoTarget = NULL;
      if (bForPreview)
      {
         AM_MEDIA_TYPE *pFormat = NULL;
         if (m_videoAviSettings.bIsVideoFormatInitialized)
         {
            pFormat = &m_videoAviSettings.videoFormat;
#ifdef _DEBUG
            //CDebugDlg::DisplayMediaType(NULL, pFormat);
#endif
         }

         pVideoTarget = new AVTarget(
            (const AVSource **)paVideoSources,
            iVideoCount,
            pFormat
            );
      }
      else
      {
         CString csVideoOutputName;
         csVideoOutputName.Format(_T("%s.avi"), tszPrefix);
         
         // As of 1.7.0.p3, Denver mode documents also use the member variable
         // m_videoAviSettings to determine the output format, so we do not
         // need to distinguish video and pure screen grabbing documents anymore.

         pVideoTarget = new AVTarget(
            (const AVSource **)paVideoSources,
            iVideoCount,
            &m_videoAviSettings.videoFormat,
            csVideoOutputName,
            m_videoAviSettings.bUseSmartRecompression,
            AV_VALUE_UNDEFINED,
            AV_VALUE_UNDEFINED,
            m_videoAviSettings.dwFCC,
            m_videoAviSettings.bUseKeyframes ? m_videoAviSettings.keyframeRate : 0,
            m_videoAviSettings.quality
            );
      }

      paTargets[1] = pVideoTarget;
	  m_videoStreamLength = paTargets[1]->GetMasterEnd();
   }

   HRESULT hr = m_avEdit.InitializeTimelines((const AVTarget **) paTargets, iTargetCount, bForPreview, NULL);

   // This is true side effect: InitializeTimelines() sets this variable in AVTarget::AddTo()!
   if (iTargetCount > 1) {
       bool bStartFillClip = m_avEdit.IsStartFillClip();
       if (bStartFillClip) {
           if (pVideoView != NULL)
               pVideoView->SetIsNearlyOneClip(true);
           // Shows the clip also at the beginning (Bug #5048)
       }
   }

 
   for (i=0; i<iTargetCount; ++i)
      delete paTargets[i];
   delete[] paTargets;
   
   for (i=0; i<iAudioCount; ++i)
      delete paAudioSources[i];
   delete[] paAudioSources;
  
   for (i=0; i<iVideoCount; ++i)
      delete paVideoSources[i];
   delete[] paVideoSources;


   if (SUCCEEDED(hr) && !bForPreview && mapVideoFileLengths.GetCount() > 0)
   {
      // Bugfix 1941, part II: Do the 2 GB Check here
      // As of version 2.0 this is a 1 GB check

      HRESULT hrInfo = S_OK;

      __int64 i64EstimatedVideoBytes = 0;
         
      POSITION pos = mapVideoFileLengths.GetStartPosition();

      while (pos != NULL && SUCCEEDED(hr))
      {
         CString csFilename;
         UINT nVideoFileLength;

         mapVideoFileLengths.GetNextAssoc(pos, csFilename, nVideoFileLength);


         __int64 i64InputVideoBytes = LIo::GetFileSize(csFilename);
         double dInputLengthSeconds = 0;

         AVInfo avInfo;
         hrInfo = avInfo.Initialize(csFilename);
         if (SUCCEEDED(hrInfo))
            hrInfo = avInfo.GetLength(&dInputLengthSeconds);

         double dOutputLengthSeconds = nVideoFileLength / 1000.0;
       
         __int64 i64EstimatedPartBytes = 
            (__int64)((dOutputLengthSeconds / dInputLengthSeconds) * (double)i64InputVideoBytes);

         i64EstimatedVideoBytes += i64EstimatedPartBytes;
      }

      if (SUCCEEDED(hrInfo) && i64EstimatedVideoBytes > 1000000000i64) // approx. 0.95 of 1 GB
      {
         CString csMsg;
         csMsg.LoadString(IDS_MSG_WARNING_EST_1GB);
         CString csCap;
         csCap.LoadString(IDS_WARNING_E);
         
         int nRes = CEditorDoc::ShowWarningMessage(csMsg, hwndParent, MB_YESNO | MB_DEFBUTTON2);
         
         if (nRes == IDNO)
           hr = E_ABORT;
      }

   }

   return hr;
}

void CEditorProject::SetProgress(float f) {
    if (m_pProgress != NULL)
        m_pProgress->SetPos((int)(f * 100));

    // TODO search for occurances of m_pProgress
}

void CEditorProject::SetProgressLabel(CString& csLabel) {
    if (m_pProgress)
        m_pProgress->SetLabel(csLabel);
}

void CEditorProject::ShowMessage(ProgressMessageType nType, CString& csMessage, CWnd *pWndParent) {
    HWND pParent = NULL;
    if (pWndParent != NULL)
        pParent = pWndParent->GetSafeHwnd();

    if (nType == PROGRESS_MSG_ERROR)
        CEditorDoc::ShowErrorMessage(csMessage, pParent);
    else if (nType == PROGRESS_MSG_WARNING)
        CEditorDoc::ShowWarningMessage(csMessage, pParent);
    else
        CEditorDoc::ShowInfoMessage(csMessage, pParent); // should not happen

    // TODO search for occurances of  CEditorDoc::Show and replace
}

/* TODO remove: is replaced by PrepareRenderGraph; remove its commented invocations, too.
HRESULT CEditorProject::InitAudioVideo(bool forPreview, LPCSTR prefix, bool onlyAudio, bool excludeClipAudio)
{
   // TODO join (partly) with ExportClips()?

   // For convenience:
   bool forExport = !forPreview;
   
   CArray<CPreviewSegment *, CPreviewSegment *> *pList = &m_previewList;
   
   //   int audioCount = 0;
   //   int videoCount = 0;
   
   CArray<const AVSource *, const AVSource *> audioSources;
   CArray<const AVSource *, const AVSource *> videoSources;
   CArray<CString         , CString>          clipFileNames;
   
   // Bugfix 1941:
   // These two arrays are used in order to estimate the export
   // AVI file size. In some cases it is possible to warn the
   // user that a video file is going to be larger than 2 GB.
   CArray<CString         , CString>          videoFileNames;
   CArray<int             , int>              videoFileLengths;
   // <---
   
   int firstVideoSegment = -1;
   HRESULT hr = S_OK;
   
   // TODO remove lots of the below clip code; leave the lines from/for the new stream
   for (int i=0; i<pList->GetSize(); ++i)
   {
      CPreviewSegment *segment = pList->GetAt(i);
      
      CAudioStream *pAudio = segment->GetAudioStream();
      CVideoStream *pVideo = segment->GetVideoStream();
      //CClipStream  *pClips = segment->GetClipStream();
      
      CArray<int, int> clipBeginTimes;
      CArray<int, int> clipEndTimes;
      CArray<CLIPAUDIOSEG, CLIPAUDIOSEG> clipAudioSegments;
      
      //if (pClips && ((pClips->HasClips() && forPreview && !onlyAudio) ||
      //               (IsDenverDocument() && forExport && !onlyAudio)))
      // Let's go through all the clips in this preview segment and
      // check if we need to insert them into the time line. If we're
      // in export mode, we won't add the video parts to the video
      // stream, but we'll always add the audio streams of the clips
      // if there are any.

      CClipStreamR *pClipStream = GetClipStream();
      
      if (pClipStream != NULL && i == 0)
      {
         int clipNr = 0;
         int targetBegin = segment->GetTargetBegin();
         int targetEnd   = segment->GetTargetEnd();
         int sourceBegin = segment->GetSourceBegin();
         int sourceEnd   = segment->GetSourceEnd();
         
         ASSERT(targetEnd - targetBegin == sourceEnd - sourceBegin);
         
         //CClipInfo *pClip = pClips->FindClip(clipNr, sourceBegin, sourceEnd);

         CClipInfo clipInfo;
         CClipInfo *pClip = &clipInfo;
         
         HRESULT hr = pClipStream->FillClipInfo((unsigned)clipNr, pClip);
         
         while (pClip && SUCCEEDED(hr) && hr != S_FALSE)
         {
            //int targetBegin = segment->GetTargetBegin();
            //int targetEnd   = segment->GetTargetEnd();
            //int sourceBegin = segment->GetSourceBegin();
            //int sourceEnd   = segment->GetSourceEnd();

            targetBegin = pClip->GetTimestamp();
            targetEnd = pClip->GetTimestamp() + pClip->GetLength();
            sourceBegin = pClipStream->GetSourceBegin(clipNr);
            sourceEnd = sourceBegin + pClip->GetLength();

            ASSERT(targetEnd - targetBegin == sourceEnd - sourceBegin);
            
            
            int clipSegOff = pClip->GetTimestamp() - sourceBegin;
            int clipSegRest = sourceEnd - (pClip->GetTimestamp() + pClip->GetLength());
            
            int clipBeginSource = pClip->GetTimestamp();
            int clipLength = pClip->GetLength();

            int clipBeginTargetSelf = sourceBegin;
            
            /* no adjustments needed
            if (clipSegOff < 0)
            {
               clipBeginSource += -clipSegOff;
               clipLength += clipSegOff;
            }
            if (clipSegRest < 0)
               clipLength += clipSegRest;
            
            int clipBeginTargetSelf = 0;
            if (clipSegOff < 0)
               clipBeginTargetSelf = -clipSegOff;
            
            if (clipSegOff >= 0)
               targetBegin += clipSegOff;
            targetEnd    = targetBegin + clipLength;
            
            //            if (clipLength > targetEnd - targetBegin)
            //               clipLength = targetEnd - targetBegin;
            */
            /*
            ASSERT(targetBegin >= 0);
            ASSERT(targetEnd >= 0);
            ASSERT(clipBeginTargetSelf >= 0);
            ASSERT(clipLength >= 0);
            //ASSERT(targetBegin >= segment->GetTargetBegin() && targetBegin < segment->GetTargetEnd());
            //ASSERT(targetEnd >= targetBegin && targetEnd <= segment->GetTargetEnd());
            //ASSERT(clipBeginTargetSelf >= 0 && clipBeginTargetSelf <= pClip->GetLength());
            //            ASSERT(clipLength == targetEnd - targetBegin);
            
            //            if (clipLength > 0)
            //            {
            //int stretchMode = IsDenverDocument() ? STRETCH_CROP : STRETCH_ASPECTRATIO;
            int stretchMode = STRETCH_ASPECTRATIO;

            bool bCurrentClipIsSmaller = false;
            if (m_videoAviSettings.bIsVideoFormatInitialized)
            {
               VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *) m_videoAviSettings.videoFormat.pbFormat;
               int nWidth = pVih->bmiHeader.biWidth;
               int nHeight = pVih->bmiHeader.biHeight;

               CAviSettings *pSettings = LookupClipAviSettings(pClip->GetFilename(), pClip->GetTempOriginalAudioVolume());
               if (pSettings)
               {
                  if (pSettings->bHasOriginalSize)
                  {
                     if (pSettings->nOriginalHeight <= nHeight &&
                         pSettings->nOriginalWidth <= nWidth)
                        bCurrentClipIsSmaller = true;
                  }
               }
            }

            if (IsDenverDocument())
            {
               if (!m_clipExportStretchDenverClips && bCurrentClipIsSmaller)
                  stretchMode = STRETCH_CROP;
            }
            else
            {
               if (bCurrentClipIsSmaller)
                  stretchMode = STRETCH_CROP;
            }
            
            // In some cases, clip audio is not desired, e.g.
            // when exporting only the audio stream.
            if (pClip->HasAudioStream() && !excludeClipAudio)
            {
               int nVolume = 100;
               int nOriginalVolume = 100;
               CAviSettings *pClipSettings = NULL;
               if (FALSE != m_clipAviSettings.Lookup(pClip->GetFilename(), pClipSettings))
               {
                  nVolume = pClipSettings->nAudioVolume;
                  nOriginalVolume = pClipSettings->nOriginalAudioVolume;
               }
               if (nVolume > 0)
               {
                  const AVSource *pClipAudioSrc = new AVSource(
                     pClip->GetAudioFilename(),
                     targetBegin,
                     targetEnd,
                     clipBeginTargetSelf,
                     clipBeginTargetSelf + clipLength,
                     //sourceBegin,
                     //sourceEnd,
                     pClip->GetAudioStreamSelector(),
                     0,
                     nVolume); // VOLUME!!!
                  audioSources.Add(pClipAudioSrc);
               }
               // We only have to consider these audio segmenst
               // if they affect the original audio!
               if (nOriginalVolume != 100)
                  clipAudioSegments.Add(CLIPAUDIOSEG(targetBegin, targetEnd, nOriginalVolume));
            }
            
            // We will only take care of the clips here
            // if we're in preview mode; otherwise the clips
            // are handled separately, as they have own
            // formats. There is just one case: Denver mode
            // documents are also handled here.
            //if ((pClips->HasClips() && forPreview && !onlyAudio) ||
            if ((forPreview && !onlyAudio) ||
               (IsDenverDocument() && forExport && !onlyAudio))
            {
               const AVSource *pClipSource = new AVSource(
                  pClip->GetFilename(),
                  targetBegin,
                  targetEnd,
                  clipBeginTargetSelf,
                  clipBeginTargetSelf + clipLength,
                  //sourceBegin,
                  //sourceEnd,
                  pClip->GetStreamSelector(),
                  stretchMode
                  );
               videoSources.Add(pClipSource);
               
               clipBeginTimes.Add(targetBegin);
               clipEndTimes.Add(targetEnd);
               bool clipFileNameFound = false;
               for (int j=0; j<clipFileNames.GetSize() && !clipFileNameFound; ++j)
               {
                  if (clipFileNames.GetAt(j) == pClip->GetFilename())
                     clipFileNameFound = true;
               }
               if (!clipFileNameFound)
                  clipFileNames.Add(CString(pClip->GetFilename()));
               
            }
            
            //            }
            clipNr++;
            //pClip = pClips->FindClip(clipNr, sourceBegin, sourceEnd);
            hr = pClipStream->FillClipInfo((unsigned)clipNr, pClip);
            if (hr != S_OK)
               pClip = NULL;

         }
      } // end if (pClips)
      
      
      if ((pVideo || (!m_csStillImage.IsEmpty() && !forExport)) && !onlyAudio)//  && !(pClips && pClips->HasClips()))
      {
         // If we're in export mode, then we don't have
         // to care about the clips here. Clips will be
         // handled separately (except Denver mode).
         // In export mode the next part is always done.
         
         ASSERT(!IsDenverDocument());
         
         CString csFileName;
         // Dirty trick to display the still image in
         // case we don't have a video, but e.g. clips:
         // Replace the file names
         bool useStillImage = false;
         if (!pVideo)
            useStillImage = true;
         
         if (useStillImage)
            csFileName = m_csStillImage;
         else
            csFileName = pVideo->GetFilename();
         
         if (clipBeginTimes.GetSize() == 0 || forExport)
         {
            const AVSource *pVideoSource = new AVSource(
               csFileName,
               segment->GetTargetBegin(),
               segment->GetTargetEnd(),
               useStillImage ? 0 : segment->GetSourceBegin() - pVideo->GetOffset() + segment->GetVideoOffset() - m_videoExportGlobalOffset,
               useStillImage ? 0 : segment->GetSourceEnd() - pVideo->GetOffset() + segment->GetVideoOffset() - m_videoExportGlobalOffset,
               useStillImage ? 0 : pVideo->GetStreamSelector()
               );
            videoSources.Add(pVideoSource);
            if (firstVideoSegment == -1)
               firstVideoSegment = videoSources.GetSize() - 1;
            
            // =====
            // Accumulate how much from each video is taken into
            // the exported document. We need this for the estimation
            // of the video size. (Bugfix 1941)
            if (forExport)
            {
               int nPos = -1;
               for (int i=0; i<videoFileNames.GetSize() && nPos == -1; ++i)
               {
                  if (videoFileNames[i] == csFileName)
                     nPos = i;
               }
               if (nPos == -1)
               {
                  videoFileNames.Add(csFileName);
                  videoFileLengths.Add(segment->GetSourceEnd() - segment->GetSourceBegin());
                  segment->GetVideoStream()->GetLength();
               }
               else
               {
                  int nLen = videoFileLengths[nPos];
                  videoFileLengths[nPos] = nLen + (segment->GetSourceEnd() - segment->GetSourceBegin());
               }
            }
            // =====
         }
         else
         {
            ASSERT(forPreview);
            int clipCount = clipBeginTimes.GetSize();
            // First part
            int firstClipStart = clipBeginTimes.GetAt(0);
            if (firstClipStart > 0)
            {
               const AVSource *pVideoSource = new AVSource(
                  csFileName,
                  segment->GetTargetBegin(),
                  firstClipStart,
                  useStillImage ? 0 : segment->GetSourceBegin() - pVideo->GetOffset() + segment->GetVideoOffset() - m_videoExportGlobalOffset,
                  useStillImage ? 0 : segment->GetSourceBegin() - pVideo->GetOffset() + (firstClipStart - segment->GetTargetBegin()) + segment->GetVideoOffset() - m_videoExportGlobalOffset,
                  useStillImage ? 0 : pVideo->GetStreamSelector()
                  );
               videoSources.Add(pVideoSource);
               if (firstVideoSegment == -1)
                  firstVideoSegment = videoSources.GetSize() - 1;
            }
            
            if (clipCount > 1)
            {
               // Middle parts
               for (int clip=0; clip<clipCount - 1; ++clip)
               {
                  int thisClipEnd = clipEndTimes.GetAt(clip);
                  int nextClipStart = clipBeginTimes.GetAt(clip + 1);
                  if (nextClipStart - thisClipEnd > 0)
                  {
                     const AVSource *pVideoSource = new AVSource(
                        csFileName,
                        thisClipEnd,
                        nextClipStart,
                        useStillImage ? 0 : segment->GetSourceBegin() - pVideo->GetOffset() + (thisClipEnd - segment->GetTargetBegin()) + segment->GetVideoOffset() - m_videoExportGlobalOffset,
                        useStillImage ? 0 : segment->GetSourceBegin() - pVideo->GetOffset() + (nextClipStart  - segment->GetTargetBegin()) + segment->GetVideoOffset() - m_videoExportGlobalOffset,
                        useStillImage ? 0 : pVideo->GetStreamSelector()
                        );
                     videoSources.Add(pVideoSource);
                     if (firstVideoSegment == -1)
                        firstVideoSegment = videoSources.GetSize() - 1;
                  }
               }
            }
            
            // Last part
            int lastClipEnd = clipEndTimes.GetAt(clipCount - 1);
            if (lastClipEnd < segment->GetTargetEnd())
            {
               const AVSource *pVideoSource = new AVSource(
                  csFileName,
                  lastClipEnd,
                  segment->GetTargetEnd(),
                  useStillImage ? 0 : segment->GetSourceBegin() - pVideo->GetOffset() + (lastClipEnd - segment->GetTargetBegin()) + segment->GetVideoOffset() - m_videoExportGlobalOffset,
                  useStillImage ? 0 : segment->GetSourceEnd() - pVideo->GetOffset() + segment->GetVideoOffset() - m_videoExportGlobalOffset,
                  useStillImage ? 0 : pVideo->GetStreamSelector()
                  );
               videoSources.Add(pVideoSource);
               if (firstVideoSegment == -1)
                  firstVideoSegment = videoSources.GetSize() - 1;
            }
         }
      }

      // Now for the audio. Consider the parts of this segment which
      // contain clip audio. Does it overwrite the clip audio?
      if (pAudio)
      {
         CString csAudioFileName = pAudio->GetFilename();
         // Do not add dummy audio sources.
         if (_tcscmp(csAudioFileName, _T("")) != 0)
         {
            if (clipAudioSegments.GetSize() == 0)
            {
               const AVSource *pAudioSource = new AVSource(
                  csAudioFileName,
                  segment->GetTargetBegin(),
                  segment->GetTargetEnd(),
                  segment->GetSourceBegin() + segment->GetAudioOffset(),
                  segment->GetSourceEnd() + segment->GetAudioOffset(),
                  pAudio->GetStreamSelector()
                  );
               audioSources.Add(pAudioSource);
            }
            else
            {
               // We have clips which contain audio affecting
               // the original audio stream.
               int nSegTargetBegin = segment->GetTargetBegin();
               int nSegTargetEnd = segment->GetTargetEnd();
               int nSourceBegin = segment->GetSourceBegin() + segment->GetAudioOffset();
               int nSourceEnd = segment->GetSourceEnd() + segment->GetAudioOffset();

               int nCurrentClip = 0;
               // Do we need a first part?
               if (nSegTargetBegin < clipAudioSegments[0].nTargetBegin)
               {
                  const AVSource *pAudioSource = new AVSource(
                     csAudioFileName,
                     nSegTargetBegin,
                     clipAudioSegments[0].nTargetEnd,
                     nSourceBegin,
                     nSourceBegin + (clipAudioSegments[0].nTargetEnd - nSegTargetBegin),
                     pAudio->GetStreamSelector()); // Full volume!
                  audioSources.Add(pAudioSource);
               }

               // Now for the middle parts
               for (nCurrentClip = 0; nCurrentClip < clipAudioSegments.GetSize(); ++nCurrentClip)
               {
                  {
                     int nSourceDelta = clipAudioSegments[nCurrentClip].nTargetBegin - nSegTargetBegin;
                     int nClipSegLength = clipAudioSegments[nCurrentClip].nTargetEnd - 
                        clipAudioSegments[nCurrentClip].nTargetBegin;

                     const AVSource *pAudioSource = new AVSource(
                        csAudioFileName,
                        clipAudioSegments[nCurrentClip].nTargetBegin,
                        clipAudioSegments[nCurrentClip].nTargetEnd,
                        nSourceBegin + nSourceDelta,
                        nSourceBegin + nSourceDelta + nClipSegLength,
                        pAudio->GetStreamSelector(),
                        0, // Stretch mode is not important
                        clipAudioSegments[nCurrentClip].nOrigAudioVolume);
                     audioSources.Add(pAudioSource);
                  }
                  
                  // Do we have more segments?
                  if (nCurrentClip + 1 < clipAudioSegments.GetSize())
                  {
                     // Do we have any space between the two?
                     if (clipAudioSegments[nCurrentClip + 1].nTargetBegin >
                         clipAudioSegments[nCurrentClip].nTargetEnd)
                     {
                        // Then we have to insert a small part of the original
                        // audio with full volume here, too.
                        int nSourceDelta = clipAudioSegments[nCurrentClip].nTargetEnd - nSegTargetBegin;
                        int nBetweenLength = clipAudioSegments[nCurrentClip + 1].nTargetBegin -
                           clipAudioSegments[nCurrentClip].nTargetEnd;

                        const AVSource *pAudioSource = new AVSource(
                           csAudioFileName,
                           clipAudioSegments[nCurrentClip].nTargetEnd,
                           clipAudioSegments[nCurrentClip + 1].nTargetBegin,
                           nSourceBegin + nSourceDelta,
                           nSourceBegin + nSourceDelta + nBetweenLength,
                           pAudio->GetStreamSelector()); // Full volume!
                        audioSources.Add(pAudioSource);
                     }
                  }
               }

               // Select the last clip audio segment in the list
               nCurrentClip = clipAudioSegments.GetSize() - 1;
               // Do we need a last part?
               if (nSegTargetEnd > clipAudioSegments[nCurrentClip].nTargetEnd)
               {
                  const AVSource *pAudioSource = new AVSource(
                     csAudioFileName,
                     clipAudioSegments[nCurrentClip].nTargetEnd,
                     nSegTargetEnd,
                     nSourceBegin + (clipAudioSegments[nCurrentClip].nTargetEnd - nSegTargetBegin),
                     nSourceEnd,
                     pAudio->GetStreamSelector()); // Full volume!
                  audioSources.Add(pAudioSource);
               }
            }
         }
      }
      else
      {
         // for segments without audio; e.g. dummy audio of "empty pages" added in the Editor

         bool bAllowNullAudio = true;

         const AVSource *pAudioSource = new AVSource(
            NULL,
            segment->GetTargetBegin(),
            segment->GetTargetEnd(),
            segment->GetSourceBegin() + segment->GetAudioOffset(),
            segment->GetSourceEnd() + segment->GetAudioOffset(),
            0, 0, 100, bAllowNullAudio);
         audioSources.Add(pAudioSource);
      }
     
   }  // end for CPreviewSegment List
   
   int audioCount = audioSources.GetSize();
   int videoCount = videoSources.GetSize();
   
   int targetCount = 1; // audio
   if (videoCount > 0)
      targetCount++;
   
   AVTarget **plTargets = new AVTarget*[targetCount];
   
   if (!m_isAudioFormatInitialized ||
      !m_videoAviSettings.bIsVideoFormatInitialized)
      InitMediaTypes();
   
   const AVSource **plAudioSources = new const AVSource*[audioCount];
   if (audioCount > 0)
   {
      for (int i=0; i<audioCount; ++i)
         plAudioSources[i] = audioSources.GetAt(i);
      if (forPreview)
      {
         AVTarget *pAudioTarget = new AVTarget(
            plAudioSources,
            audioCount,
            &m_mtAudioFormat
            );
         plTargets[0] = pAudioTarget;
      }
      else
      {
         CString outName;
         if (!onlyAudio)
            outName.Format("%s.lad", prefix);
         else
            outName.Format("%s", prefix);
         ASSERT(forExport);
         AVTarget *pAudioTarget = new AVTarget(
            plAudioSources,
            audioCount,
            &m_mtAudioFormat,
            outName
            );
         
         // Now set the LAD info if needed
         if (!onlyAudio)
         {
            // Create an LAD info
            CString version;
            CSecurityUtils cSec;
            cSec.GetVersionStringShort(version);
            
            CString versionType;
            switch (m_nVersionType)
            {
            case FULL_VERSION: versionType = "f"; break;
            case UNIVERSITY_VERSION: versionType = "u"; break;
            default: versionType = "e"; break;
            }
            CTime time = CTime::GetCurrentTime();
            CString timeString = time.Format("%H:%M:%S");
            CString dateString = time.Format("%d.%m.%Y");
            CString ladInfo;
            ladInfo.Format("%s;%s;%s;%s", versionType, version, timeString, dateString);
            pAudioTarget->SetLadInfo(ladInfo);
         }
         plTargets[0] = pAudioTarget;
      }
   }
   
   const AVSource **plVideoSources = new const AVSource*[videoCount];
   if (videoCount > 0)
   {
      if (forPreview)
      {
         //if (!IsDenverDocument())
         {
            // This is a small hack to make the first
            // video segment the most important part for
            // the preview; otherwise we would get the
            // clip resolution for the preview window,
            // and that dramatically kills performance.
            int tempCount = 0;
            if (firstVideoSegment >= 0 && HasVideo())
            {
               plVideoSources[0] = videoSources.GetAt(firstVideoSegment);
               tempCount++;
            }
            else if (!HasVideo())
            {
               // This may be the case if we're implanting a
               // still image instead of the video into
               // the clip stream
               firstVideoSegment = -1;
            }
            
            for (int i=0; i<videoCount; ++i)
            {
               const AVSource *pVideoSource = videoSources.GetAt(i);

               if (i != firstVideoSegment)
                  plVideoSources[tempCount++] = videoSources.GetAt(i);
            }
            
            AM_MEDIA_TYPE *pmtOut = NULL;
            if (m_videoAviSettings.bIsVideoFormatInitialized)
            {
               pmtOut = &m_videoAviSettings.videoFormat;
#ifdef _DEBUG
               //CDebugDlg::DisplayMediaType(NULL, pmtOut);
#endif
            }
            else
            {
               // Let's create a preview format, even if we
               // do not have a real video.
               CreatePreviewFormat();
               if (m_videoAviSettings.bIsVideoFormatInitialized)
                  pmtOut = &m_videoAviSettings.videoFormat;
            }
            
            AVTarget *pVideoTarget = new AVTarget(
               plVideoSources,
               videoCount,
               pmtOut
               );
            
            plTargets[1] = pVideoTarget;
         }
         /*
         else
         {
            // For Denver mode preview, we'll need to 
            // create a preview type which matches the
            // output type. Because of that, we don't need
            // the "first video part" hack from above
            // (apart from that, we don't have normal video
            // in the project here).
            for (int i=0; i<videoCount; ++i)
               plVideoSources[i] = videoSources.GetAt(i);
            
            AM_MEDIA_TYPE mtOut;
            ZeroMemory(&mtOut, sizeof AM_MEDIA_TYPE);
            hr = MakeDenverMediaType(clipFileNames, mtOut);
            
            if (SUCCEEDED(hr))
            {
               AVTarget *pDenverTarget = new AVTarget(
                  plVideoSources,
                  videoCount,
                  &mtOut
                  );
               
               plTargets[1] = pDenverTarget;
            }
            else
            {
               plTargets[1] = NULL;
            }
         }
         *//*
      }
      else
      {
         // We don't need that hack in export mode (see above)
         ASSERT(forExport);
         for (int i=0; i<videoCount; ++i)
            plVideoSources[i] = videoSources.GetAt(i);
         
         CString outName;
         outName.Format("%s.avi", prefix);
         
         // As of 1.7.0.p3, Denver mode documents also use the member variable
         // m_videoAviSettings to determine the output format, so we do not
         // need to distinguish video and pure screen grabbing documents anymore.

         //if (!IsDenverDocument())
         //{
         AVTarget *pVideoTarget = new AVTarget(
            plVideoSources,
            videoCount,
            &m_videoAviSettings.videoFormat,
            outName,
            m_videoAviSettings.bUseSmartRecompression,
            AV_VALUE_UNDEFINED,
            AV_VALUE_UNDEFINED,
            m_videoAviSettings.dwFCC,
            m_videoAviSettings.bUseKeyframes ? m_videoAviSettings.keyframeRate : 0,
            m_videoAviSettings.quality
            );
         plTargets[1] = pVideoTarget;
         /*
         }
         else
         {
            // We want to write a Denver mode document;
            // we need to find a suitable output media type
            AM_MEDIA_TYPE mtOut;
            ZeroMemory(&mtOut, sizeof AM_MEDIA_TYPE);
            hr = MakeDenverMediaType(clipFileNames, mtOut);
            
            if (SUCCEEDED(hr))
            {
               AVTarget *pVideoTarget = new AVTarget(
                  plVideoSources,
                  videoCount,
                  &mtOut,
                  outName,
                  m_clipExportUseSmartRecompression,
                  AV_VALUE_UNDEFINED,
                  AV_VALUE_UNDEFINED,
                  -1, // FCC: Not defined/Use default
                  -1, // Use defaults
                  -1  // Use defaults
                  );
               plTargets[1] = pVideoTarget;
            }
            else
            {
               plTargets[1] = NULL;
            }
            
            FreeMediaType(mtOut);
         }
         *//*
      }
      
      m_videoStreamLength = plTargets[1]->GetMasterEnd();
   }
   
   if (SUCCEEDED(hr))
      hr = m_avEdit.InitializeTimelines((const AVTarget **) plTargets, targetCount, NULL);
   
   // Now we can free all our objects again;
   // The AVEdit module always copies all its
   // parameters.
   for (i=0; i<targetCount; ++i)
   {
      if (plTargets[i])
         delete plTargets[i];
   }
   delete[] plTargets;
   plTargets = NULL;
  
   audioSources.RemoveAll();
   videoSources.RemoveAll();
   
   for (i=0; i<audioCount; ++i)
      delete plAudioSources[i];
   delete[] plAudioSources;
   plAudioSources = NULL;
   
   for (i=0; i<videoCount; ++i)
      delete plVideoSources[i];
   delete[] plVideoSources;
   plVideoSources = NULL;
   
   if (SUCCEEDED(hr) && forExport && HasVideo())
   {
      // Bugfix 1941, part II
      // Do the 2 GB Check here
      CArray<int, int> videoTotalFileLengths;
      CArray<__int64, __int64> videoFileByteSizes;
      HRESULT hr2 = S_OK;
      for (int i=0; i<videoFileNames.GetSize() && SUCCEEDED(hr2); ++i)
      {
         videoFileByteSizes.Add(LIo::GetFileSize(videoFileNames[i]));
         AVInfo avInfo;
         hr2 = avInfo.Initialize(videoFileNames[i]);
         double dLength = 0.0;
         if (SUCCEEDED(hr2))
            hr2 = avInfo.GetLength(&dLength);
         if (SUCCEEDED(hr2))
            videoTotalFileLengths.Add((int) (1000.0*dLength));
      }
      
      if (SUCCEEDED(hr2))
      {
         __int64 i64EstVideoSize = 0;
         for (int i=0; i<videoFileNames.GetSize(); ++i)
         {
            double dVideoPartsMs = videoFileLengths[i];
            double dVideoTotalMs = videoTotalFileLengths[i];
            __int64 i64VideoTotalSizeBytes = videoFileByteSizes[i];
            
            __int64 i64VideoPartsSizeBytes = 
               (__int64) ((dVideoPartsMs / dVideoTotalMs) * ((double) i64VideoTotalSizeBytes));
            
            i64EstVideoSize += i64VideoPartsSizeBytes;
         }
         
         if (i64EstVideoSize > 2147483648i64)
         {
            CString csMsg;
            csMsg.LoadString(IDS_MSG_WARNING_EST_2GB);
            CString csCap;
            csCap.LoadString(IDS_WARNING);
            
            int nRes = ::MessageBox(m_pExportProgressDialog->GetSafeHwnd(),
               csMsg, csCap, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
            
            if (nRes == IDNO)
            {
               hr = E_ABORT;
            }
         }
      }
   }
   
   return hr;
}*/

HRESULT CEditorProject::CheckQuestion()
{
   UINT nPreviewMs = m_pDoc->m_curPosMs;
   CQuestionStream *pStream = GetQuestionStream(false);
   if (pStream != NULL)
   {
      HRESULT hr = pStream->OperateQuestion(nPreviewMs, QUESTION_OPERATION_CHECK);
      return hr;
   }
   else
      return S_FALSE;
}

HRESULT CEditorProject::ResetQuestion()
{
   UINT nPreviewMs = m_pDoc->m_curPosMs;
   CQuestionStream *pStream = GetQuestionStream(false);
   if (pStream != NULL)
   {
      HRESULT hr = pStream->OperateQuestion(nPreviewMs, QUESTION_OPERATION_RESET);
      if (SUCCEEDED(hr) && hr != S_FALSE)
         m_pDoc->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
      return hr;
   }
   else
      return S_FALSE;
}

HRESULT CEditorProject::StartQuestionTimers()
{
   CQuestionStream *pStreamQ = GetQuestionStream(false);
   if (pStreamQ != NULL)
      return pStreamQ->StartTimerThread();
   else
      return S_FALSE;
}

HRESULT CEditorProject::ResetQuestionsAfterPreview()
{
   HRESULT hr = S_FALSE;
   
   CInteractionStream *pStream = GetInteractionStream(false);
   if (pStream != NULL)
   {
      hr = pStream->ResetAfterPreview();
   }

   CQuestionStream *pStreamQ = GetQuestionStream(false);
   if (pStreamQ != NULL && SUCCEEDED(hr))
   {
      hr = pStreamQ->ResetAfterPreview();
   }

   CWhiteboardView *pView = m_pDoc->GetWhiteboardView();
   if (pView != NULL && SUCCEEDED(hr))
      pView->ResetAfterPreview(); // remove any left-over feedback
   
   return hr;
}

HRESULT CEditorProject::UpdateQuestionnaireFeedback()
{
   HRESULT hr = S_OK;

   if (m_pDoc == NULL)
      hr = E_FAIL;

   CQuestionStream *pQuestionStream = NULL;
   if (SUCCEEDED(hr))
   {
      pQuestionStream = m_pDoc->project.GetQuestionStream();
      if (pQuestionStream == NULL)
         hr = E_FAIL;
   }
   
   CArray<CQuestionnaireEx *, CQuestionnaireEx *> aQuestionnaires;
   if (SUCCEEDED(hr))
   {
      pQuestionStream->GetQuestionnaires(&aQuestionnaires);
      if (aQuestionnaires.GetCount() == 0)
         hr = E_FAIL;
   }

   CQuestionnaireEx *pQuestionnaire = NULL;
   if (SUCCEEDED(hr))
   {
      pQuestionnaire = aQuestionnaires[0];
      if (pQuestionnaire == NULL)
         hr = E_FAIL;
   }

   CFeedback *pNewResultFeedback = NULL;
   if (SUCCEEDED(hr))
   {
      pNewResultFeedback = CQuestionnaireEx::CreateResultFeedback(this, pQuestionnaire);
      if (pNewResultFeedback == NULL)
         hr = E_FAIL;
   }

   if (SUCCEEDED(hr))
         hr = pQuestionnaire->ReplaceFeedback(pNewResultFeedback);
   return hr;
}

void CEditorProject::HandleError(UINT msgId, HRESULT hr)
{
   TCHAR dxErr[512];
   DWORD res = AMGetErrorText(hr, dxErr, 512);
   if (res >= 0)
   {
      CString msg;
      msg.Format(msgId, hr, dxErr);
      CEditorDoc::ShowErrorMessage(msg);
   }
   else
   {
      CString msg;
      msg.Format(msgId, hr, _T("<unknown>"));
      CEditorDoc::ShowErrorMessage(msg);
   }
}

void CEditorProject::GetUnsupportedFonts(CArray<CString, CString> &acsUnsupportedFonts)
{
    acsUnsupportedFonts.RemoveAll();
    for(int i = 0; i < m_acsUnsupportedFonts.GetCount(); i++)
    {
        acsUnsupportedFonts.Add(m_acsUnsupportedFonts.GetAt(i));
    }
}

/* Start Randomize questions implementation */

CQuestionnaireEx *CEditorProject::GetFirstQuestionnaireEx() {
    if (!m_pQuestionStream)
        return NULL;
    CArray<CQuestionnaireEx *, CQuestionnaireEx *> aQuestionnaires;
    m_pQuestionStream->GetQuestionnaires(&aQuestionnaires);
    if (aQuestionnaires.GetSize() > 0) {
        return aQuestionnaires[0];
    }
    return NULL;
}

bool CEditorProject::FirstQuestionnaireIsRandomTest() {
    CQuestionnaireEx *pFirstQuestionnaire = GetFirstQuestionnaireEx();
    if (!pFirstQuestionnaire)
        return false;
    return pFirstQuestionnaire->IsRandomTest();
}

int CEditorProject::GetFirstQuestionBeginTimeStamp() {
    if (!m_pQuestionStream)
        return -1;
    for (int i = 0; i < m_aPageStream.GetSize(); i++) {
        CQuestionEx *pQuestion = m_pQuestionStream->FindQuestionForTime(m_aPageStream[i]->GetBegin());
        if (pQuestion != NULL) {
            return pQuestion->GetPageBegin();
        }
    }
    return -1;
}

int CEditorProject::GetLastQuestionEndTimeStamp() {
    if (!m_pQuestionStream)
        return -1;

    for (int i = m_aPageStream.GetSize() -1; i >= 0; i--) {
        CQuestionEx *pQuestion = m_pQuestionStream->FindQuestionForTime(m_aPageStream[i]->GetBegin());
        if (pQuestion != NULL)
            // subtract 1, because the END timestamp of page is equal with BEGIN timestamp of next page. See CEditorProject::FindPageAt or CPageStream::FindPageAtTimestamp
            return pQuestion->GetPageBegin() + pQuestion->GetPageLength() - 1;
    }
    return -1;
}

bool CEditorProject::StartRandomTest(int iNewPositionTimeStamp) {
    if (!m_bRandomTestStarted
        && iNewPositionTimeStamp >= GetFirstQuestionBeginTimeStamp()) {
        m_bRandomTestStarted = true;
        CQuestionnaireEx *pFirstQuestionnaire = GetFirstQuestionnaireEx();
        CQuestionEx *pQuestion = pFirstQuestionnaire->GetRandomQuestionFromPosition(0);
        if (pQuestion && m_pDoc)
            m_pDoc->JumpPreview(pQuestion->GetPageBegin());
        return true;
    }
    return false;
}

void CEditorProject::StartRandomTest(bool bStart) {
    m_bRandomTestStarted = bStart;
}

void CEditorProject::AddStopmarkAtEndOfEachQuestionPage() {
    if (m_pDoc == NULL || m_pMarkStream == NULL || m_pQuestionStream == NULL)
        return;

    CArray<CPage *, CPage *> caAllSlides;
    GetPages(caAllSlides, 0, m_pDoc->m_docLengthMs);
    for (int i = 0; i < caAllSlides.GetSize(); ++i)
    {
        CQuestionEx *pQuestion = m_pQuestionStream->FindQuestionForTime(caAllSlides[i]->GetBegin());
        if (pQuestion)
        {
            int iInsertionTimestamp = caAllSlides[i]->GetEnd()-1;
            if (i == caAllSlides.GetSize()-1)
                iInsertionTimestamp = m_pDoc->m_docLengthMs-1;

            if (GetStopMarkAt(iInsertionTimestamp) == NULL) {
                CStopJumpMark *pMark = new CStopJumpMark();
                pMark->Init(false, NULL, iInsertionTimestamp);
                pMark->RegisterUser();
                m_pMarkStream->IncludeMark(pMark);
                delete pMark;
            }
        }
    }
}

bool CEditorProject::IsRandomTestEnabled() {
    return m_bIsRandomTestEnable;
}

void CEditorProject::SetRandomTestEnabled(bool bEnableRandomTest) {
    if (bEnableRandomTest == false 
        && FirstQuestionnaireIsRandomTest()
        && m_pDoc && m_pDoc->IsPreviewActive()) {

        CQuestionnaireEx *pFirstQuestionnaire = GetFirstQuestionnaireEx();
        if (pFirstQuestionnaire)
            pFirstQuestionnaire->RestoreDocumentFromRandomizePlay();
    }

    m_bIsRandomTestEnable = bEnableRandomTest;
}


void CEditorProject::ClearJumpRandomQuestionActions() {
    CQuestionStream *pQuestionStream = GetQuestionStream(true);

    // Change all questions actions so there is no INTERACTION_JUMP_RANDOM_QUESTION set
    CArray<CPage *, CPage *> caAllSlides;
    GetPages(caAllSlides, 0, m_pDoc->m_docLengthMs);
    for (int i = 0; i < caAllSlides.GetSize(); ++i)
    {
        CQuestionEx *pQuestion = pQuestionStream->FindQuestionForTime(caAllSlides[i]->GetBegin());
        if (pQuestion) {
            ClearJumpRandomQuestionAction(pQuestion->GetCorrectAction());
            ClearJumpRandomQuestionAction(pQuestion->GetFailureAction());
        }
    }

    // Change all interactions actions so there is no INTERACTION_JUMP_RANDOM_QUESTION set
    CTimePeriod ctpDocument;
    ctpDocument.Init(INTERACTION_PERIOD_ALL_PAGES, 0, m_pDoc->m_docLengthMs);
    CArray<CInteractionAreaEx *, CInteractionAreaEx *> arAllObjects;
    GetInteractionStream()->FindInteractions(&ctpDocument, false, &arAllObjects);
    if (arAllObjects.GetSize() > 0) {
        for (int i = 0; i< arAllObjects.GetSize(); i++) {
            CInteractionAreaEx *pInteraction = arAllObjects[i];
            if(pInteraction->GetClassId() == INTERACTION_CLASS_AREA
                || pInteraction->GetClassId() == INTERACTION_CLASS_BUTTON){
                    ClearJumpRandomQuestionAction(pInteraction->GetClickAction());
                    ClearJumpRandomQuestionAction(pInteraction->GetUpAction());
                    ClearJumpRandomQuestionAction(pInteraction->GetDownAction());
                    ClearJumpRandomQuestionAction(pInteraction->GetEnterAction());
                    ClearJumpRandomQuestionAction(pInteraction->GetLeaveAction());
            }
        }
    }
}

void CEditorProject::ClearJumpRandomQuestionAction(CMouseAction *pAction) {
    if (pAction && pAction->GetActionId() == INTERACTION_JUMP_RANDOM_QUESTION) {
        pAction->SetActionId(INTERACTION_NO_ACTION);
        if (pAction->GetNextAction())
            pAction->GetNextAction()->SetActionId(INTERACTION_NO_ACTION);
    }
}

void CEditorProject::SetJumpRandomQuestionActions() {
    CQuestionStream *pQuestionStream = GetQuestionStream(true);

    // Change all questions actions to INTERACTION_JUMP_RANDOM_QUESTION
    CArray<CPage *, CPage *> caAllSlides;
    GetPages(caAllSlides, 0, m_pDoc->m_docLengthMs);
    for (int i = 0; i < caAllSlides.GetSize(); ++i)
    {
        CQuestionEx *pQuestion = pQuestionStream->FindQuestionForTime(caAllSlides[i]->GetBegin());
        if (pQuestion) {
            SetJumpRandomQuestionAction(pQuestion->GetCorrectAction());
            SetJumpRandomQuestionAction(pQuestion->GetFailureAction());
        }
    }
}

void CEditorProject::SetJumpRandomQuestionAction(CMouseAction *pAction) {
    if (pAction) {
        pAction->SetActionId(INTERACTION_JUMP_RANDOM_QUESTION);
        if (pAction->GetNextAction())
            pAction->GetNextAction()->SetActionId(INTERACTION_START_REPLAY);
    }
}