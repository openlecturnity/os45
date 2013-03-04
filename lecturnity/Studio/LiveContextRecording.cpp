#include "StdAfx.h"

#include "LiveContextRecording.h"
#include "Studio.h"
#include "MfcUtils.h"   // lsutl32
#include "RegistrySettings.h"   // lsutl32
#include "lutils.h"     // lutils
#include "..\Assistant2_0\backend\la_project.h"
#include "..\Assistant2_0\backend\mlb_misc.h"
#include "..\Assistant2_0\AssistantSettings.h"

CLiveContextRecording::CLiveContextRecording(void) {

    InitializeVariables();

    ActivateAvGrabber();

    m_pHelperWnd = new CLiveContextHelperWnd(this);
    m_pHelperWnd->CreateEx(WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, 
        AfxRegisterWndClass(NULL,0,0,0), NULL, 
        WS_POPUP | WS_VISIBLE, CRect(0, 0, 1, 1), NULL, 0);

    CreateCommandBars();
    UINT uPresentationBar[] = {ID_ADDORREMOVEBUTTONS_EXITFULLSCREEN, 
      -1,-1,-1,-1,ID_ADDORREMOVEBUTTONS_MARKERS, -1,-1,ID_ADDORREMOVEBUTTONS_PENCILS,
      -1,-1,-1,-1,-1,-1,ID_ADDORREMOVEBUTTONS_POINTER,ID_ADDORREMOVEBUTTONS_SIMPLIFIEDNAVIGATION,
      ID_ADDORREMOVEBUTTONS_PAGEFOCUSED,ID_ADDORREMOVEBUTTONS_DOCUMENTSTRUCT,ID_ADDORREMOVEBUTTONS_RECORDINGS,
      ID_ADDORREMOVEBUTTONS_CUT,ID_ADDORREMOVEBUTTONS_COPY,ID_ADDORREMOVEBUTTONS_PASTE,ID_ADDORREMOVEBUTTONS_UNDO,ID_ADDORREMOVEBUTTONS_ONECLICKCOPY,
      ID_ADDORREMOVEBUTTONS_TEXT,ID_ADDORREMOVEBUTTONS_LINE,ID_ADDORREMOVEBUTTONS_POLYLINE,ID_ADDORREMOVEBUTTONS_FREEHAND,
      ID_ADDORREMOVEBUTTONS_ELLIPSE,ID_ADDORREMOVEBUTTONS_RECTANGLE,ID_ADDORREMOVEBUTTONS_POLYGON,ID_ADDORREMOVEBUTTONS_INSERTPAGE,
      ID_ADDORREMOVEBUTTONS_START,-1,ID_ADDORREMOVEBUTTONS_STOP,ID_ADDORREMOVEBUTTONS_PAUSERECORDING,ID_ADDORREMOVEBUTTONS_SCREENGRABBING,-1,
      ID_ADDORREMOVEBUTTONS_PREVIOUSPAGE, ID_ADDORREMOVEBUTTONS_NEXTPAGE};
    m_pCommandBars->GetImageManager()->SetIcons(IDB_PRESENTATIONBAR_SMALLICONS_HOT, uPresentationBar, _countof(uPresentationBar), CSize(16, 16)); 
    
    CreatePresentationBar();

    CreateAudioWidget();

    InitializeTools();
	SetSgHotkeys();
}

CLiveContextRecording::~CLiveContextRecording(void) {

    if (ASSISTANT::Project::active != NULL) {
        ASSISTANT::Project::DeleteAvGrabber();
        delete ASSISTANT::Project::active;
    }
    ASSISTANT::Project::active = NULL;

    if (m_pWndCaptureRect != NULL) {
        m_pWndCaptureRect->DestroyWindow();
        delete m_pWndCaptureRect;
    }
    m_pWndCaptureRect = NULL;

    if (m_pPresentationBar != NULL) {
        m_pPresentationBar->DestroyWindow();
        delete m_pPresentationBar;
    }
    m_pPresentationBar = NULL;

    if (m_pCommandBars != NULL) {
        m_pCommandBars->InternalRelease();
    }

    if (m_pAudioWidget != NULL) {
        m_pAudioWidget->DestroyWindow();
        delete m_pAudioWidget;
    }
    m_pAudioWidget = NULL;

    if ( m_Markers != NULL )
        delete m_Markers;
    if ( m_Pens != NULL )
        delete m_Pens;

    if (m_pHelperWnd != NULL) {
        m_pHelperWnd->KillTimers();
        m_pHelperWnd->DestroyWindow();
        delete m_pHelperWnd;
        m_pHelperWnd = NULL;
    }
}

bool CLiveContextRecording::StartScreenRecording(CLiveContextStartScreen *pLiveContextStartScreen) {

    
    if (ASSISTANT::Project::active == NULL)
        return false;

    if (pLiveContextStartScreen != NULL) 
        pLiveContextStartScreen->UpdateProgress(3,5);
    if (!ASSISTANT::Project::active->IsAudioDeviceAvailable())
        return false;

    if (pLiveContextStartScreen != NULL) 
        pLiveContextStartScreen->UpdateProgress(4,5);
    // Start Screen grabbing
    bool bSetPureSG = true;
    ASSISTANT::Project::active->DoPureScreenGrabbing(bSetPureSG);

    if (pLiveContextStartScreen != NULL) {
        pLiveContextStartScreen->UpdateProgress(5,5);
        pLiveContextStartScreen->Close();
    }
    // Start area selection 
    StartSgSelection();
    
    m_bRecordingFinished = false;
    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0 && !m_bRecordingFinished)
    {    
        if (bRet == -1) {
            // handle the error and possibly exit
        } else {
            if (m_pPresentationBar != NULL && m_pPresentationBar->IsWindowVisible() && !m_pPresentationBar->IsAutoHide())
                m_pPresentationBar->PutWindowOnTop();
            //m_pPresentationBar->UpdateAppearance(); //TODO: uncomment simultaniously with commenting creation of UpdatePresentationBarThread
            if (msg.message == WM_KEYDOWN && msg.wParam == VK_SPACE) {
                  ButtonPausePressed();
            }
            if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
                CancelSgSelection();
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    if (m_pPresentationBar != NULL) {
        bool bPresentationBarAutoHide = m_pPresentationBar->IsAutoHide();
        AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("AutoHidePresentationBar"), bPresentationBarAutoHide);
    }
    return true;
}

// Virtual functions from ILiveContextHelperListener

void CLiveContextRecording::StartStopHotkeyCalled() {
	if(m_bRecordingStarted)
		StopSGRecording();
	else if (m_pWndCaptureRect != NULL)
        FinishedSgSelection(m_pWndCaptureRect->IsDoRecordFullScreen());
}

void CLiveContextRecording::OnPresentationBarContextMenu(NMHDR* pNMHDR, LRESULT* pResult) {
   CXTPControl *pCtrlPos = m_pCommandBars->GetContextMenus()->GetAt(3)->GetControl(0);
  
   int nWidth = 0;
   int nHeight = 0;
   CXTPControl *pCtrlAutoHide = NULL;

   if (m_pCommandBars != NULL) {
      NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

      CXTPControl* pControl = tagNMCONTROL->pControl;
      if(pControl) {
         BOOL bIsChecked = pControl->GetChecked();
         if(pControl->GetParent() == pCtrlPos->GetCommandBar()) {
            for(int i=0;i<pCtrlPos->GetCommandBar()->GetControlCount();i++)
               pCtrlPos->GetCommandBar()->GetControl(i)->SetChecked(FALSE);
         }
         if(bIsChecked)
            pControl->SetChecked(FALSE);
         else
            pControl->SetChecked(TRUE);

         switch(pControl->GetID()) {
         case ID_PRESENTATIONBAR_POSITION_BOTTOM:
            m_pPresentationBar->SetPosition(OT_BOTTOMMIDDLE);
            break;
         case ID_PRESENTATIONBAR_POSITION_TOP:
            m_pPresentationBar->SetPosition(OT_TOPMIDDLE);  
            break;
         case ID_PRESENTATIONBAR_POSITION_LEFT:
            m_pPresentationBar->SetPosition(OT_MIDDLELEFT);
            break;
         case ID_PRESENTATIONBAR_POSITION_RIGHT:
            m_pPresentationBar->SetPosition(OT_MIDDLERIGHT);
            break;
         case ID_PRESENTATIONBAR_SHOWTRANSPARENT:
            m_pPresentationBar->ShowTransparent(pControl->GetChecked()?true:false);
            break;
         case ID_PRESENTATIONBAR_SMALLICONS:
            m_pPresentationBar->SetSmallIcons(pControl->GetChecked()?true:false);
            break;
         case ID_PRESENTATIONBAR_AUTOHIDE:
            m_pPresentationBar->SetAutoHide(pControl->GetChecked()?true:false);
            break;
         case ID_VIEW_AUDIOVIDEO_WIDGET:
             ShowHideAudioWidget();
             break;
         case ID_ADDORREMOVEBUTTONS_EXITFULLSCREEN:
            m_pPresentationBar->ShowHideExitFullScreen(pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_PENCILS:
            m_pPresentationBar->ShowHideAnnotations(ID_PRESENTATIONBAR_PENS, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_MARKERS:
            m_pPresentationBar->ShowHideAnnotations(ID_PRESENTATIONBAR_MARKERS, pControl->GetChecked()?true:false);          
            break;
         case ID_ADDORREMOVEBUTTONS_POINTER:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_TELEPOINTER, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_SIMPLIFIEDNAVIGATION:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_MOUSENAV, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_START:
            m_pPresentationBar->ShowHideRecordingTools(ID_PRESENTATIONBAR_START, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_STOP:
            m_pPresentationBar->ShowHideRecordingTools(ID_PRESENTATIONBAR_STOP, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_PAUSERECORDING:
            m_pPresentationBar->ShowHideRecordingTools(ID_PRESENTATIONBAR_PAUSE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_SCREENGRABBING:
            m_pPresentationBar->ShowHideScreenGrabbing(pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_PAGEFOCUSED:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_PAGEFOCUSED, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_DOCUMENTSTRUCT:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_DOCUMENTSTRUCT, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_RECORDINGS:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_RECORDINGS, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_CUT:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_CUT, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_COPY:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_COPY, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_PASTE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_PASTE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_UNDO:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_UNDO, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_ONECLICKCOPY:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_ONECLICKCOPY, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_TEXT:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_TEXT, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_LINE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_LINE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_POLYLINE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_POLYLINE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_FREEHAND:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_FREEHAND, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_ELLIPSE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_ELLIPSE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_RECTANGLE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_RECTANGLE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_POLYGON:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_POLYGON, pControl->GetChecked()?true:false);
            break; 
         case ID_ADDORREMOVEBUTTONS_INSERTPAGE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_INSERTPAGE, pControl->GetChecked()?true:false);
            break; 
         case ID_ADDORREMOVEBUTTONS_PREVIOUSPAGE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_PREVIOUSPAGE, pControl->GetChecked()?true:false);
            break;
         case ID_ADDORREMOVEBUTTONS_NEXTPAGE:
            m_pPresentationBar->ShowHidePresentationTools(ID_PRESENTATIONBAR_NEXTPAGE, pControl->GetChecked()?true:false);
            break;
         }
      }
   }
}

// Virtual methods from SGSettingsListener
void CLiveContextRecording::ShowSgSettings() {
    CString csTitle;
    csTitle.LoadString(ID_MENU_ASSISTANT_SETTINGS);

    // page 1 == Screengrabbing
    CAssistantSettings assistantSettings(csTitle, m_pCommandBars, NULL, 1, false, true);

    assistantSettings.m_psh.dwFlags |= PSH_NOAPPLYNOW;
    assistantSettings.m_psh.dwFlags &= ~PSH_HASHELP;

    assistantSettings.Init((CAssistantDoc *)NULL);

    assistantSettings.DoModal();
}

void CLiveContextRecording::ShowAudioSettings() {
    CString csTitle;
    csTitle.LoadString(ID_MENU_ASSISTANT_SETTINGS);

    // page 0 == Audio
    CAssistantSettings assistantSettings(csTitle, m_pCommandBars, NULL, 0, false, true);

    assistantSettings.m_psh.dwFlags |= PSH_NOAPPLYNOW;
    assistantSettings.m_psh.dwFlags &= ~PSH_HASHELP;

    assistantSettings.Init((CAssistantDoc *)NULL);

    assistantSettings.DoModal();

    if (m_pAudioWidget != NULL)
       m_pAudioWidget->SetAudioVideoTooltips(GetAudioTooltip(), _T(""));
}

void CLiveContextRecording::GetSgStartStopHotKey(AVGRABBER::HotKey &hotKeyStartStop) {
   if (ASSISTANT::Project::active == NULL)
      return;
   
   HRESULT hr = ASSISTANT::Project::active->GetSgStartStopHotKey(hotKeyStartStop);
}

void CLiveContextRecording::SetSgStartStopHotKey(AVGRABBER::HotKey &hotKeyStartStop) {
   if (ASSISTANT::Project::active == NULL)
      return;
   
   HRESULT hr = ASSISTANT::Project::active->SetSgStartStopHotKey(hotKeyStartStop);
   SetSgHotkeys();
}

void CLiveContextRecording::GetPossibleKeys(CStringArray &arPossibleKeys) {
    ASSISTANT::GetPossibleKeys(arPossibleKeys);
}

// Virtual methods from SGSelectAreaListener

void CLiveContextRecording::FinishedSgSelection(bool bRecordFullScreen) {
   if (m_pWndCaptureRect != NULL) {

       CRect rcLastCustom;
       m_pWndCaptureRect->GetLastCustomRect(rcLastCustom);

       // Save settings to registry
       CRegistrySettings::SetLastSelectionRectangle(rcLastCustom);
       CRegistrySettings::SetRecordFullScreen(bRecordFullScreen);

      m_bIsShowSGPresentationBar = m_pWndCaptureRect->GetShowPresentationBar();
      m_nPositionSG = m_pPresentationBar->GetPosition();

      CRect rcSelection = m_pWndCaptureRect->GetSelectionRect();

      m_pWndCaptureRect->DestroyWindow();
      delete m_pWndCaptureRect;
      m_pWndCaptureRect = NULL;

      StartSGRecording(rcSelection);
   }
}

void CLiveContextRecording::CancelSgSelection() {
    ASSISTANT::Project::active->DoPureScreenGrabbing(false);

    // make sure no audio widget is visible
    if (m_pAudioWidget->GetSafeHwnd() && m_pAudioWidget->IsWindowVisible() )
        m_pAudioWidget->ShowWindow(SW_HIDE);

    // Destroy capture window
    if (m_pWndCaptureRect != NULL) {
        CRect rcLastCustom;
        m_pWndCaptureRect->GetLastCustomRect(rcLastCustom);
        bool bIsFullScreenMode = m_pWndCaptureRect->IsDoRecordFullScreen();

        CString csLastSelectionRectangle;
        csLastSelectionRectangle.Format(_T("%d;%d;%d;%d"), 
                                        rcLastCustom.left, rcLastCustom.top,
                                        rcLastCustom.right, rcLastCustom.bottom);

        LRegistry::WriteStringRegistryEntry(HKEY_CURRENT_USER,
            _T("Software\\imc AG\\LECTURNITY\\Assistant\\SgSettings\\"), 
            _T("LastSelectionRectangleCustom"), csLastSelectionRectangle);

        LRegistry::WriteBoolRegistryEntry(HKEY_CURRENT_USER,
            _T("Software\\imc AG\\LECTURNITY\\Assistant\\SgSettings\\"), 
            _T("DoRecordFullScreen"), bIsFullScreenMode);

        m_pWndCaptureRect->DestroyWindow();
        delete m_pWndCaptureRect;
    }
    m_pWndCaptureRect = NULL;

    m_bRecordingFinished = true;
	m_bRecordingStarted = false;
    CStudioApp::SetLiveContextExitCode(EXIT_CODE_LC_NO_RECORDING_DONE);
}

// virtual functions from IPresentationBarListener

void CLiveContextRecording::ButtonPensPressed() {
    m_Pens->Active = !m_Pens->Active;
    ChangeTool(m_Pens->Selected);
}

void CLiveContextRecording::ButtonMarkersPressed() {
    m_Markers->Active = !m_Markers->Active;
    ChangeTool(m_Markers->Selected);
}

void CLiveContextRecording::ButtonTelepointerPressed() {
   ChangeTool(ID_TOOL_TELEPOINTER);
}

void CLiveContextRecording::ButtonStartPressed() {
    if (m_pWndCaptureRect != NULL)
        FinishedSgSelection(m_pWndCaptureRect->IsDoRecordFullScreen());
}

void CLiveContextRecording::ButtonStopPressed() {
    StopSGRecording();
}

void CLiveContextRecording::ButtonPausePressed() {
    if (ASSISTANT::Project::active != NULL && ASSISTANT::Project::active->IsRecording()) {
        if (ASSISTANT::Project::active->IsRecordingPaused()) {
            HRESULT hr = ASSISTANT::Project::active->UnPauseRecording();
            if (m_pHelperWnd != NULL)
                m_pHelperWnd->StopPauseTimer();
            if (m_pPresentationBar != NULL) {
                m_pPresentationBar->ChangeFlashStatus(false);
            }
        }
        else {
            HRESULT hr = ASSISTANT::Project::active->PauseRecording();
            if (m_pHelperWnd != NULL)
                 m_pHelperWnd->StartPauseTimer();
            if (m_pPresentationBar != NULL) {
                m_pPresentationBar->ChangeFlashStatus(true);
            }
        }
    }
}
// private functions


void CLiveContextRecording::ChangeAudioValue(double dAudioValue) {
    if (m_pAudioWidget != NULL && m_pAudioWidget->GetSafeHwnd() != NULL)
        m_pAudioWidget->SetAudioPosition(dAudioValue);
}

void CLiveContextRecording::XTPGalleryMarkers(NMHDR* pNMHDR, LRESULT* pResult) { NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
    CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);
    if (pGallery) {
        CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());
        if (pItem) {
            ChangeTool(pItem->GetID());

            if (m_pCommandBars != NULL) {
                int nMarkerIndex;
                UINT uiTabTools[] = {-1, -1,
                    m_Markers->GetAt(4) == m_Markers->Selected ? ID_BUTTON_MARKERS : -1, // MARKER YELLOW
                    m_Markers->GetAt(1) == m_Markers->Selected ? ID_BUTTON_MARKERS: -1,  // MARKER ORANGE
                    m_Markers->GetAt(2) == m_Markers->Selected ? ID_BUTTON_MARKERS: -1,  // MARKER RED
                    m_Markers->GetAt(3) == m_Markers->Selected ? ID_BUTTON_MARKERS: -1,  // MARKER GREEN
                    m_Markers->GetAt(0) == m_Markers->Selected ? ID_BUTTON_MARKERS: -1,  // MARKER BLUE
                    m_Markers->GetAt(5) == m_Markers->Selected ? ID_BUTTON_MARKERS: -1,  // MARKER MAGENTA
                    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1,-1, -1, -1};
                nMarkerIndex = m_Markers->get_Selected();
                switch(nMarkerIndex) {
                    case ID_TOOL_MARKERYELLOW: 
                        nMarkerIndex= 0;
                        break;
                    case ID_TOOL_MARKERORANGE: 
                        nMarkerIndex= 1;
                        break; 
                    case ID_TOOL_MARKERRED: 
                        nMarkerIndex= 2;
                        break; 
                    case ID_TOOL_MARKERGREEN: 
                        nMarkerIndex= 3;
                        break;
                    case ID_TOOL_MARKERBLUE: 
                        nMarkerIndex= 4;
                        break; 
                    case ID_TOOL_MARKERMAGENTA: 
                        nMarkerIndex= 5;
                        break;
                }
                m_pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiTabTools, _countof(uiTabTools), CSize(32, 32));
                m_pCommandBars->RedrawCommandBars();

                if (m_pPresentationBar != NULL)
                    m_pPresentationBar->SetPresentationBarMarker(nMarkerIndex);
            }
        }
        *pResult = TRUE; // Handled
    }
}

void CLiveContextRecording::XTPGalleryPens(NMHDR* pNMHDR, LRESULT* pResult) {
    NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
    CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);
    if (pGallery) {
        CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());
        if (pItem) {
            ChangeTool(pItem->GetID());
            if (m_pCommandBars != NULL ) {
                int nPenIndex;
                UINT uiTabTools[] = {-1, -1, -1, -1, -1, -1, -1, -1,
                    m_Pens->GetAt(1) == m_Pens->Selected ? ID_BUTTON_PENS : -1, // PEN BLACK
                    m_Pens->GetAt(0) == m_Pens->Selected ? ID_BUTTON_PENS : -1, // PEN BLUE
                    m_Pens->GetAt(2) == m_Pens->Selected ? ID_BUTTON_PENS : -1, // PEN CYAN
                    m_Pens->GetAt(3) == m_Pens->Selected ? ID_BUTTON_PENS : -1, // PEN GREEN
                    m_Pens->GetAt(4) == m_Pens->Selected ? ID_BUTTON_PENS : -1, // PEN YELLOW
                    m_Pens->GetAt(5) == m_Pens->Selected ? ID_BUTTON_PENS : -1, // PEN RED
                    m_Pens->GetAt(6) == m_Pens->Selected ? ID_BUTTON_PENS : -1, // PEN MAGENTA
                    m_Pens->GetAt(7) == m_Pens->Selected ? ID_BUTTON_PENS : -1, // PEN WHITE
                    -1,-1,-1,-1,-1,-1};
                nPenIndex = m_Pens->get_Selected();
                switch(nPenIndex) {
                    case ID_TOOL_PENBLACK: 
                        nPenIndex= 0;
                        break;
                    case ID_TOOL_PENBLUE: 
                        nPenIndex= 1;
                        break; 
                    case ID_TOOL_PENCYAN: 
                        nPenIndex= 2;
                        break; 
                    case ID_TOOL_PENGREEN: 
                        nPenIndex= 3;
                        break;
                    case ID_TOOL_PENYELLOW: 
                        nPenIndex= 4;
                        break; 
                    case ID_TOOL_PENRED: 
                        nPenIndex= 5;
                        break;
                    case ID_TOOL_PENMAGENTA: 
                        nPenIndex= 6;
                        break;
                    case ID_TOOL_PENWHITE: 
                        nPenIndex= 7;
                        break;
                }
                m_pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiTabTools, _countof(uiTabTools), CSize(32, 32));
                m_pCommandBars->RedrawCommandBars();

                if (m_pPresentationBar != NULL)
                    m_pPresentationBar->SetPresentationBarPen(nPenIndex);
            }      
        }
        *pResult = TRUE; // Handled
    }
}

void CLiveContextRecording::EnableGalleryButton(CCmdUI* pCmdUI) {
   CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, CXTPControl::FromUI(pCmdUI));

   if (pGallery) {
      if (m_Markers->Active == true )
         pGallery->SetCheckedItem(m_Markers->Selected);
      else if ( m_Pens->Active == true )
         pGallery->SetCheckedItem(m_Pens->Selected);
      else 
         pGallery->SetCheckedItem(ID_TOOL_NOTHING);
   }

   pCmdUI->Enable();
}

void CLiveContextRecording::UpdateAudioWidget() {
    // Update duration
    CString csWindowText;
    CString csStatus = _T("0:00:00");
    if (ASSISTANT::Project::active != NULL && m_pAudioWidget != NULL) {
        int iRecordingTime = ASSISTANT::Project::active->GetRecordingTime();
        int hour = (float)iRecordingTime / 3600000;
        int msecLeft = iRecordingTime - (hour*3600000);
        int min = (float)msecLeft / 60000;
        msecLeft = msecLeft - (min*60000);
        int sec = (float)msecLeft / 1000;
        csStatus.Format(_T("%d:%02d:%02d"),hour, min, sec);
        m_pAudioWidget->UpdateDuration(csStatus);
    }

    // Update disc space
    if (ASSISTANT::Project::active != NULL && m_pAudioWidget != NULL) {
        CString csWindowText;
        CString csStatus = _T("??MB");
        CString csRecordPath = ASSISTANT::Project::active->GetRecordPath();
        if (csRecordPath.IsEmpty()) 
            ASSISTANT::GetLecturnityHome(csRecordPath);  

        int iMbLeft = 0;
        if (!csRecordPath.IsEmpty()) {
            SystemInfo::CalculateDiskSpace(csRecordPath, iMbLeft);
            if (iMbLeft > 1024)
                csStatus.Format(_T("%dGB"), (int)(iMbLeft/1024));
            else 
                csStatus.Format(_T("%dMB"), iMbLeft);
        }
        m_pAudioWidget->UpdateDiskSpace(csStatus);
    }
}

void CLiveContextRecording::UpdateAudioContextMenu(CCmdUI* pCmdUI) {
    pCmdUI->Enable();
    switch (pCmdUI->m_nID) {
       case ID_AUDIOWIDGET_SHOWDURATION:
           pCmdUI->SetCheck(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetAudioShowDuration"), 1) == 1);
           break;
       case ID_AUDIOWIDGET_SHOWDISKSPACE:
           pCmdUI->SetCheck(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetAudioShowDiskSpace"), 1) == 1);
           break;
       case ID_AUDIOWIDGET_LOCKPOSITION:
           pCmdUI->SetCheck(AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("WidgetAudioLockPosition"), 0) == 1);
           break;
    }
}

void CLiveContextRecording::ChangeAudioWidget(NMHDR* pNMHDR, LRESULT* pResult) {
    if (m_pAudioWidget == NULL)
        return;

    if (m_pCommandBars != NULL) {
        NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

        CXTPControl* pControl = tagNMCONTROL->pControl;
        if (pControl) {
            BOOL bIsChecked = pControl->GetChecked();
            if(bIsChecked)
                pControl->SetChecked(FALSE);
            else
                pControl->SetChecked(TRUE);

            switch(pControl->GetID()) {
                case ID_AUDIOWIDGET_SHOWDURATION:
                    m_pAudioWidget->ShowDuration(!bIsChecked);
                    break;
                case ID_AUDIOWIDGET_SHOWDISKSPACE:
                    m_pAudioWidget->ShowDiskSpace(!bIsChecked);
                    break;
                case ID_AUDIOWIDGET_LOCKPOSITION:
                    m_pAudioWidget->LockPosition(!bIsChecked);
                    break;
            }
        }
    }
}



// private functions

UINT CLiveContextRecording::UpdatePresentationBarThread(LPVOID pParam) {
    CPresentationBar *pPresentationBar = (CPresentationBar *)pParam;

    CXTPPopupBar* pPopupContextMenu = (CXTPPopupBar*)pPresentationBar->GetCommandBars()->GetContextMenus()->FindCommandBar(IDR_PRESENTATIONBAR);

    if (pPresentationBar) {
        while (pPresentationBar->IsWindowVisible()) {
            //pPresentationBar->UpdateDialogControls(pPresentationBar, FALSE);
            while (pPopupContextMenu->IsVisible()) {
                Sleep(20);
            }
            pPresentationBar->UpdateAppearance();
            Sleep(20);
        }
    }

    return 1;
}

void CLiveContextRecording::InitializeVariables() {
    m_pWndCaptureRect = NULL;
    m_pPresentationBar = NULL;
    m_pCommandBars = NULL;
    m_pAudioWidget = NULL;

    m_bRecordingFinished = true;
	m_bRecordingStarted = false;
    m_nPositionSG = AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("PositionSG"), OT_MIDDLERIGHT);
    m_bIsShowSGPresentationBar = true;

    m_pHelperWnd = NULL;

    m_iCurrentTool = ID_TOOL_NOTHING;
    m_Markers = NULL;
    m_Pens = NULL;
}

void CLiveContextRecording::ActivateAvGrabber() {
    const _TCHAR *tszDesiredLrdName = NULL;
    tszDesiredLrdName = CStudioApp::GetLiveContextRecordFile();
    ASSISTANT::Project::active = new ASSISTANT::Project(true, false, false, false, tszDesiredLrdName);
    ASSISTANT::Project::InitAvGrabber();
    ASSISTANT::Project::active->ActivateAudioAndVideo();
}

void CLiveContextRecording::CreateCommandBars() {
    CXTPPaintManager::SetTheme(xtpThemeRibbon);

    m_pCommandBars =  (CXTPCommandBars*)RUNTIME_CLASS(CXTPCommandBars)->CreateObject();
    ASSERT(m_pCommandBars);
    if (!m_pCommandBars)
        return;
    m_pCommandBars->GetContextMenus()->RemoveAll();
    m_pCommandBars->SetSite(m_pHelperWnd);

    CMenu mnu;
    mnu.LoadMenu(IDR_MONITOR_WIDGET);
    m_pCommandBars->GetContextMenus()->Add(IDR_MONITOR_WIDGET, _T("Context Menu"), mnu.GetSubMenu(0));

    CMenu mnuPens;
    mnuPens.LoadMenu(ID_MENU_PENS);
    m_pCommandBars->GetContextMenus()->Add(ID_MENU_PENS, _T("Menu Pens"), mnuPens.GetSubMenu(0));

    CMenu mnuMarkers;
    mnuMarkers.LoadMenu(ID_MENU_MARKERS);
    m_pCommandBars->GetContextMenus()->Add(ID_MENU_MARKERS, _T("Menu Markers"), mnuMarkers.GetSubMenu(0));
}

void CLiveContextRecording::CreatePresentationBar() {
    CXTPCommandBars* pCommandBars = m_pCommandBars;

    CMenu mnuPresentationBar;
    mnuPresentationBar.LoadMenu(IDR_PRESENTATIONBAR);

    bool bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("ExitFullScreen"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_EXITFULLSCREEN, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Pencils"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_PENCILS, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Markers"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_MARKERS, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Pointer"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_POINTER, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Navigation"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_SIMPLIFIEDNAVIGATION, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Start"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_START, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Stop"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_STOP, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Pause"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_PAUSERECORDING, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("ScreenGrabbing"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_SCREENGRABBING, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("PageFocused"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_PAGEFOCUSED, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("DocumentStruct"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_DOCUMENTSTRUCT, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Recordings"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_RECORDINGS, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Cut"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_CUT, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Copy"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_COPY, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Paste"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_PASTE, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Undo"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_UNDO, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("OneClickCopy"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_ONECLICKCOPY, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Text"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_TEXT, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Line"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_LINE, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Polyline"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_POLYLINE, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("FreeHand"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_FREEHAND, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Ellipse"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_ELLIPSE, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Rectangle"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_RECTANGLE, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Polygon"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_POLYGON, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("InsertPage"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_INSERTPAGE, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("PreviousPage"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_PREVIOUSPAGE, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("NextPage"), 1) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_ADDORREMOVEBUTTONS_NEXTPAGE, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("SmallIcons"), 0) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_PRESENTATIONBAR_SMALLICONS, bShowButton?MF_CHECKED:MF_UNCHECKED);

    bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Transparent"), 0) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_PRESENTATIONBAR_SHOWTRANSPARENT, bShowButton?MF_CHECKED:MF_UNCHECKED);

    UINT nPos = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Position"), 0) == 1)?true:false;
    mnuPresentationBar.CheckMenuItem(ID_PRESENTATIONBAR_POSITION_BOTTOM, (nPos==OT_BOTTOMMIDDLE)?MF_CHECKED:MF_UNCHECKED);
    mnuPresentationBar.CheckMenuItem(ID_PRESENTATIONBAR_POSITION_LEFT, (nPos==OT_MIDDLELEFT)?MF_CHECKED:MF_UNCHECKED);
    mnuPresentationBar.CheckMenuItem(ID_PRESENTATIONBAR_POSITION_RIGHT, (nPos==OT_MIDDLERIGHT)?MF_CHECKED:MF_UNCHECKED);
    mnuPresentationBar.CheckMenuItem(ID_PRESENTATIONBAR_POSITION_TOP, (nPos==OT_TOPMIDDLE)?MF_CHECKED:MF_UNCHECKED);

    pCommandBars->GetContextMenus()->Add(IDR_PRESENTATIONBAR, _T("Context Menu PB"), mnuPresentationBar.GetSubMenu(0));

    m_pPresentationBar = new CPresentationBar(NULL);
    m_pPresentationBar->SetMainFrmWnd(NULL);
    m_pPresentationBar->CreateEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 
        AfxRegisterWndClass(NULL,0,0,0), NULL, 
        WS_POPUP | WS_VISIBLE, CRect(100, 100, 400, 200), NULL, 0);
    m_pPresentationBar->SetLayeredWindowAttributes(RGB(255, 255, 255), (255 * 30) / 100, LWA_ALPHA | LWA_COLORKEY);
    m_pPresentationBar->SetCommandBars(m_pCommandBars);
    m_pPresentationBar->ShowWindow(SW_HIDE);
    m_pPresentationBar->SetListener(this);

    // All messages are send to the helper window
    m_pHelperWnd->SetPresentationBar(m_pPresentationBar);
}

void CLiveContextRecording::CreateAudioWidget() {
    CMenu mnuAudioWidget;
    mnuAudioWidget.LoadMenu(IDR_AUDIO_WIDGET);
    if (m_pCommandBars)
        m_pCommandBars->GetContextMenus()->Add(IDR_AUDIO_WIDGET, _T("Context Menu"), mnuAudioWidget.GetSubMenu(0));

    m_pAudioWidget = new CAudioWidget();
    CWnd *pDesktopWnd = CWnd::FromHandle(GetDesktopWindow());
    m_pAudioWidget->CreateEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, AfxRegisterWndClass(CS_DBLCLKS,0,0,0), NULL, 
        WS_POPUP | WS_VISIBLE, CRect(100, 100, 150, 150), pDesktopWnd, 0);
    m_pAudioWidget->SetCommandBars(m_pCommandBars);
    m_pAudioWidget->ShowWindow(SW_HIDE);

    m_pAudioWidget->SetAudioVideoTooltips(GetAudioTooltip(), _T(""));
}

void CLiveContextRecording::SetSgHotkeys() {
    if (m_pHelperWnd == NULL)
        return;

    AVGRABBER::HotKey hkStartStop;
    HRESULT hr = ASSISTANT::Project::active->GetSgStartStopHotKey(hkStartStop);
    BOOL bUnregisterResult = UnregisterHotKey(m_pHelperWnd->GetSafeHwnd(), hkStartStop.id);

    CString csNone;
    csNone.LoadString(IDS_SG_HOTKEYS_CMB_NONE);

    if (hkStartStop.vKeyString != csNone) {
        unsigned int modifier = 0;
        if (hkStartStop.hasCtrl)
            modifier |= MOD_CONTROL;
        if (hkStartStop.hasShift)
            modifier |= MOD_SHIFT;
        if (hkStartStop.hasAlt)
            modifier |= MOD_ALT;

        if (hkStartStop.vKeyString == _T("F9"))
            hkStartStop.vKey = VK_F9;
        else if (hkStartStop.vKeyString == _T("F10"))
            hkStartStop.vKey = VK_F10;
        else if (hkStartStop.vKeyString == _T("F11"))
            hkStartStop.vKey = VK_F11;
        else if (hkStartStop.vKeyString == _T("F12"))
            hkStartStop.vKey = VK_F12;
        else
            hkStartStop.vKey = hkStartStop.vKeyString[0];

        BOOL bResult = RegisterHotKey(m_pHelperWnd->GetSafeHwnd(), hkStartStop.id, modifier, hkStartStop.vKey);
        if (bResult == FALSE) {
            CString csHotkey;
            csHotkey = _T("\"");
            if (hkStartStop.hasCtrl)
                csHotkey += _T("Ctrl-");
            if (hkStartStop.hasShift)
                csHotkey += _T("Shift-");
            if (hkStartStop.hasAlt)
                csHotkey += _T("Alt-");
            csHotkey += hkStartStop.vKeyString;
            csHotkey += _T("\" ");
            CString csMessage;
            csMessage.Format(IDS_ERROR_REGISTER_HOTKEY, csHotkey);
            MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
        }
    }

}

HRESULT CLiveContextRecording::StartSgSelection() {
    CRect rcDesktop;
    MfcUtils::GetDesktopRectangle(rcDesktop);

    CRect rcLastSelection;
    CRegistrySettings::GetLastSelectionRectangle(rcLastSelection);
    bool bIsFullScreen = CRegistrySettings::GetRecordFullScreen();
    if(rcLastSelection.IsRectEmpty())
        rcLastSelection.SetRect(0, 0, 640, 480);

    m_pWndCaptureRect = new CNewCaptureRectChooser(ASSISTANT::Project::active->GetAvGrabber(),
                                                   rcLastSelection, bIsFullScreen, this, this);

    m_pWndCaptureRect->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                                AfxRegisterWndClass(0,0,0,0), NULL, 
                                WS_POPUP | WS_VISIBLE, rcDesktop, NULL, 0);

    CXTPControl *pControl = NULL;
    if (m_pCommandBars != NULL) {
        CXTPControl *pCtrlPos = m_pCommandBars->GetContextMenus()->GetAt(3)->GetControl(0);
        pCtrlPos->SetStyle(xtpButtonCaption);
        switch(m_nPositionSG) {
        case OT_MIDDLERIGHT:
            pControl = pCtrlPos->GetCommandBar()->GetControls()->FindControl(ID_PRESENTATIONBAR_POSITION_RIGHT);
            break;
        case OT_BOTTOMMIDDLE:
            pControl = pCtrlPos->GetCommandBar()->GetControls()->FindControl(ID_PRESENTATIONBAR_POSITION_BOTTOM);
            break;
        case OT_TOPMIDDLE:
            pControl = pCtrlPos->GetCommandBar()->GetControls()->FindControl(ID_PRESENTATIONBAR_POSITION_TOP);
            break;
        case OT_MIDDLELEFT:
            pControl = pCtrlPos->GetCommandBar()->GetControls()->FindControl(ID_PRESENTATIONBAR_POSITION_LEFT);
            break;
        }
    }

    if(pControl != NULL && !pControl->GetChecked()) {
       NMXTPCONTROL tagNMCONTROL;
       tagNMCONTROL.pControl = pControl;
       NMHDR * pNmhdr = (NMHDR *) (LPARAM)&tagNMCONTROL;

       OnPresentationBarContextMenu(pNmhdr, 0);
    } else {
       m_pPresentationBar->SetPosition(m_nPositionSG);
    }

    bool bPresentationBarAutoHide = 
        AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("AutoHidePresentationBar"), 0) == 1 ? true : false;
    bool bShowAudioMonitor = 
        AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("ShowAudioMonitor"), 1) == 0 ? false : true;
    CXTPControl *pCtrlAutoHide = m_pCommandBars->GetContextMenus()->GetAt(3)->GetControl(3);
    pCtrlAutoHide->SetChecked(bPresentationBarAutoHide);
    CXTPControl *pCtrlAMonitor = m_pCommandBars->GetContextMenus()->GetAt(3)->GetControl(4);
    pCtrlAMonitor->SetChecked(bShowAudioMonitor);
    CXTPControl *pCtrlAddRemove = m_pCommandBars->GetContextMenus()->GetAt(3)->GetControl(5);
    pCtrlAddRemove->SetStyle(xtpButtonCaption);
    m_pPresentationBar->SetPreparationMode(true); 
    
    if (m_pHelperWnd != NULL) {
        m_pHelperWnd->StartAudioTimer();
    }
    if (bPresentationBarAutoHide)
        m_pPresentationBar->SetAutoHide(true);
    m_pAudioWidget->ShowWindow(bShowAudioMonitor? SW_SHOW : SW_HIDE);
    if (m_pPresentationBar && m_bIsShowSGPresentationBar) {
        CWinThread *pThread = AfxBeginThread(UpdatePresentationBarThread, m_pPresentationBar);
    }
    

    return S_OK;
}

void CLiveContextRecording::StartSGRecording(CRect &rcSelection) {
    ShowPresentationBarInSgRec(false);
   
    // Show audio widget if set
    if (m_pAudioWidget != NULL) {
        m_pAudioWidget->DestroyWindow();
        delete m_pAudioWidget;
        m_pAudioWidget = NULL;
    }
    CreateAudioWidget();

    bool bShowAudioMonitor = 
        AfxGetApp()->GetProfileInt(_T("Assistant Settings"), _T("ShowAudioMonitor"), 1) == 0 ? false : true;
    CRect rcWidget;
    m_pAudioWidget->GetWindowRect(rcWidget);
    m_pAudioWidget->StartSavePosition();
    m_pAudioWidget->SetCommandBars(m_pCommandBars);
    if (m_pAudioWidget->UpdateWindowPlacement() == false )
        m_pAudioWidget->SetWindowPos(NULL, 100, 100, rcWidget.Width(), rcWidget.Height(), SWP_NOZORDER);
    m_pAudioWidget->ShowWindow(bShowAudioMonitor? SW_SHOW : SW_HIDE);


    SetSgHotkeys();
    HRESULT hr = ASSISTANT::Project::active->StartSg(rcSelection);
	m_bRecordingStarted = true;
    if (m_pHelperWnd != NULL) {
        m_pHelperWnd->StartAudioTimer();
        m_pHelperWnd->StartRecordingTimer();
    }
}

void CLiveContextRecording::StopSGRecording() {

    if (ASSISTANT::Project::active == NULL)
        return;

    m_pAudioWidget->ShowWindow(SW_HIDE);
    if (m_pPresentationBar != NULL) {
        m_pPresentationBar->ShowWindow(SW_HIDE);
    }

    HRESULT hr = ASSISTANT::Project::active->StopSg();

    m_bRecordingFinished = true;
	m_bRecordingStarted = false;
}

void CLiveContextRecording::ShowHideAudioWidget() {
    bool bIsVisible = m_pAudioWidget->IsWindowVisible() == TRUE ? true : false;

    m_pAudioWidget->ShowWindow(bIsVisible? SW_HIDE : SW_SHOW);
    AfxGetApp()->WriteProfileInt(_T("Assistant Settings"), _T("ShowAudioMonitor"), !bIsVisible);

}
void CLiveContextRecording::ShowPresentationBarInSgRec(bool bFixedArea) {
    if (m_pPresentationBar != NULL) {
        if (!m_bIsShowSGPresentationBar) {
            m_pPresentationBar->DestroyWindow();
            m_pPresentationBar = NULL;
        } else {
            m_pPresentationBar->DestroyWindow();

            HWND hParent = NULL;
            hParent = FindWindow(NULL, _T("CDrawWindow"));
            CWnd *pParent = CWnd::FromHandle(hParent);

            m_pPresentationBar->CreateEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, 
                AfxRegisterWndClass(0,0,0,0), NULL, WS_POPUP | WS_VISIBLE, CRect(100, 100, 400, 200), 
                pParent, 0);

            m_pPresentationBar->SetLayeredWindowAttributes(RGB(255, 255, 255), (255 * 30) / 100, LWA_ALPHA | LWA_COLORKEY);  

            m_pPresentationBar->SetForegroundWindow();
            m_pPresentationBar->SetCommandBars(m_pCommandBars);
            m_pPresentationBar->SetListener(this);
            m_pPresentationBar->SetPreparationMode(false);

            m_pPresentationBar->RedrawWindow(); 

            CWinThread *pThread = AfxBeginThread(UpdatePresentationBarThread, m_pPresentationBar);

            m_pPresentationBar->SetPosition(m_nPositionSG);
            CXTPControl *pCtrlAutoHide = m_pCommandBars->GetContextMenus()->GetAt(3)->GetControl(3);
             //SetChecked(bPresentationBarAutoHide);
            m_pPresentationBar->SetAutoHide(pCtrlAutoHide->GetChecked() == TRUE ? true : false);
            m_pPresentationBar->RedrawWindow();  
        }
    }
}

CString CLiveContextRecording::GetAudioTooltip() {

    CString csToolTip = _T("");

    if (ASSISTANT::Project::active != NULL) {
        CStringArray arDevices;
        arDevices.RemoveAll();
        UINT nDevice = 0;
        ASSISTANT::Project::active->GetAudioDevices(arDevices, nDevice);
        if (arDevices.GetSize() > 0 && nDevice < arDevices.GetSize()) {
            CString csDevice;
            csDevice.Format(IDS_DEVICE, arDevices[nDevice]);
            csToolTip += csDevice;
        }
        arDevices.RemoveAll();

        CUIntArray arSampleWidths;
        arSampleWidths.RemoveAll();
        UINT nSampleWidth = 0;
        ASSISTANT::Project::active->GetSampleWidths(nDevice, arSampleWidths, nSampleWidth);
        if (arSampleWidths.GetSize() > 0 && nSampleWidth < arSampleWidths[nSampleWidth]) {
            CString csSampleWidth;
            csSampleWidth.Format(IDS_SAMPLE_WIDTH, arSampleWidths[nSampleWidth]);
            csToolTip += csSampleWidth;
        }
        arSampleWidths.RemoveAll();

        CUIntArray arSampleRates;
        arSampleRates.RemoveAll();
        UINT nSampleRate = 0;
        ASSISTANT::Project::active->GetSampleRates(nDevice, arSampleRates, nSampleRate);
        if (arSampleRates.GetSize() > 0 && nSampleRate < arSampleRates.GetSize()) {
            CString csSampleRate;
            csSampleRate.Format(IDS_SAMPLE_RATE, arSampleRates[nSampleRate]);
            csToolTip += csSampleRate;
        }
        arSampleRates.RemoveAll();

        CStringArray arSources;
        arSources.RemoveAll();
        UINT nSource = 0;
        ASSISTANT::Project::active->GetAudioSources(arSources, nSource);
        if (arSources.GetSize() > 0 && nSource < arSources.GetSize()) {
            CString csAudioSource;
            csAudioSource.Format(IDS_AUDIO_SOURCE, arSources[nSource]);
            csToolTip += csAudioSource;
        }
        arSources.RemoveAll();

        UINT nVolume = 0;
        CString csVolume;
        nVolume = ASSISTANT::Project::active->GetMixerVolume();
        csVolume.Format(IDS_VOLUME, nVolume);
        csToolTip += csVolume;

        CString csDblClick;
        csDblClick.LoadString(IDS_DBLCLICK_AVSETTINGS);
        csToolTip += csDblClick;
   }
   
   return csToolTip;
}

void CLiveContextRecording::InitializeTools() {
    m_iCurrentTool = ID_TOOL_NOTHING;

    // Create Pens
    m_Pens = new CDrawingToolSettings();
    m_Pens->Add(ID_TOOL_PENBLUE);
    m_Pens->Add(ID_TOOL_PENBLACK);
    m_Pens->Add(ID_TOOL_PENCYAN);
    m_Pens->Add(ID_TOOL_PENGREEN);
    m_Pens->Add(ID_TOOL_PENYELLOW);
    m_Pens->Add(ID_TOOL_PENRED);
    m_Pens->Add(ID_TOOL_PENMAGENTA);
    m_Pens->Add(ID_TOOL_PENWHITE);
    m_Pens->Selected = ID_TOOL_PENBLUE;
    m_Pens->Active = false;

    // Create Markers
    m_Markers = new CDrawingToolSettings();
    m_Markers->Add(ID_TOOL_MARKERBLUE);
    m_Markers->Add(ID_TOOL_MARKERORANGE);
    m_Markers->Add(ID_TOOL_MARKERRED);
    m_Markers->Add(ID_TOOL_MARKERGREEN);
    m_Markers->Add(ID_TOOL_MARKERYELLOW);
    m_Markers->Add(ID_TOOL_MARKERMAGENTA);
    m_Markers->Selected = ID_TOOL_MARKERBLUE;
    m_Markers->Active = false;
}

void CLiveContextRecording::ChangeTool(int iCurrentTool) {
    if (m_iCurrentTool == iCurrentTool) {
        m_iCurrentTool = ID_TOOL_NOTHING;
        if (ASSISTANT::Project::active != NULL) {
            m_Pens->Active = false;
            m_Markers->Active = false;
            ASSISTANT::Project::active->ResetAnnotationMode();
        }
    } else {
        m_iCurrentTool = iCurrentTool;
        if (ASSISTANT::Project::active != NULL) {
            m_Markers->Active = false;
            m_Pens->Active = false;
            if (m_iCurrentTool != ID_TOOL_NOTHING) {
                if (m_Markers->Find(m_iCurrentTool)) {
                    m_Markers->Selected = m_iCurrentTool;
                    m_Markers->Active = true;
                } else if (m_Pens->Find(m_iCurrentTool)) {
                    m_Pens->Selected = m_iCurrentTool;
                    m_Pens->Active = true;
                }
                ASSISTANT::Project::active->SetAnnotationMode(m_iCurrentTool, 
                                                              1, 0, 0, 
                                                              RGB(0, 0, 0), RGB(0, 0, 0), false, RGB(0, 0, 0));
            }
            else {
                ASSISTANT::Project::active->ResetAnnotationMode();
            }
        }
    }
}
