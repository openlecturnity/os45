// DummyWnd.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Studio.h"
#include "LiveContextHelperWnd.h"
#include "..\Assistant2_0\backend\la_project.h"


// CDummyWnd

IMPLEMENT_DYNAMIC(CLiveContextHelperWnd, CWnd)

CLiveContextHelperWnd::CLiveContextHelperWnd(ILiveContextListener *pListener) {
    m_pListener = pListener;
    m_pPresentationBar = NULL;
    m_nPresentationBarUpdateTimer = 0;
    m_nPauseButtonTimer = 0;
    m_nFlashPause = 1;
    m_nAudioTimer = 0;
    m_nCheckDiskSpaceTimer = 0;
    m_nUpdateRecordTimer = 0;
    m_nUpdateDiscSpaceTimer = 0;
}

CLiveContextHelperWnd::~CLiveContextHelperWnd() {
}


BEGIN_MESSAGE_MAP(CLiveContextHelperWnd, CWnd)
    ON_MESSAGE(WM_HOTKEY, OnHotkeys)
    ON_WM_CREATE()
    ON_WM_TIMER()
    
    // presentation bar context menu
    ON_XTP_EXECUTE(ID_PRESENTATIONBAR_POSITION_TOP, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_PRESENTATIONBAR_POSITION_RIGHT, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_PRESENTATIONBAR_POSITION_BOTTOM, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_PRESENTATIONBAR_POSITION_LEFT, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_PRESENTATIONBAR_SHOWTRANSPARENT, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_PRESENTATIONBAR_SMALLICONS, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_PRESENTATIONBAR_AUTOHIDE, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_VIEW_AUDIOVIDEO_WIDGET, OnPresentationBarContextMenu)
    

    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_EXITFULLSCREEN, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_PENCILS, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_MARKERS, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_POINTER, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_SIMPLIFIEDNAVIGATION, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_START, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_STOP, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_PAUSERECORDING, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_SCREENGRABBING, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_PAGEFOCUSED, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_DOCUMENTSTRUCT, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_RECORDINGS, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_CUT, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_COPY, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_PASTE, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_UNDO, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_ONECLICKCOPY, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_TEXT, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_LINE, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_POLYLINE, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_FREEHAND, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_ELLIPSE, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_RECTANGLE, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_POLYGON, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_INSERTPAGE, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_PREVIOUSPAGE, OnPresentationBarContextMenu)
    ON_XTP_EXECUTE(ID_ADDORREMOVEBUTTONS_NEXTPAGE, OnPresentationBarContextMenu)

    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_POSITION_TOP, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_POSITION_RIGHT, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_POSITION_BOTTOM, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_POSITION_LEFT, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_SHOWTRANSPARENT, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_SMALLICONS, OnUpdatePresentationBar)

    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_EXITFULLSCREEN, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_PENCILS, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_MARKERS, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_POINTER, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_SIMPLIFIEDNAVIGATION, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_START, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_STOP, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_PAUSERECORDING, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_SCREENGRABBING, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_PAGEFOCUSED, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_DOCUMENTSTRUCT, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_RECORDINGS, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_CUT, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_COPY, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_PASTE, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_UNDO, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_ONECLICKCOPY, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_TEXT, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_LINE, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_POLYLINE, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_FREEHAND, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_ELLIPSE, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_RECTANGLE, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_POLYGON, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_INSERTPAGE, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_PREVIOUSPAGE, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_ADDORREMOVEBUTTONS_NEXTPAGE, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_AUTOHIDE, OnUpdatePresentationBar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_AUDIOVIDEO_WIDGET, OnUpdatePresentationBar)

    ON_XTP_EXECUTE(ID_PRESENTATION_PENCILS, OnXTPGalleryPens)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATION_PENCILS, OnEnableGalleryButton)
    ON_XTP_EXECUTE(ID_PRESENTATION_MARKERS, OnXTPGalleryMarkers)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATION_MARKERS, OnEnableGalleryButton)

    // audio widget context menu
    ON_UPDATE_COMMAND_UI(ID_AUDIOWIDGET_SHOWDURATION, OnUpdateAudioWidget)
    ON_UPDATE_COMMAND_UI(ID_AUDIOWIDGET_SHOWDISKSPACE, OnUpdateAudioWidget)
    ON_UPDATE_COMMAND_UI(ID_AUDIOWIDGET_LOCKPOSITION, OnUpdateAudioWidget)

    ON_XTP_EXECUTE(ID_AUDIOWIDGET_SHOWDURATION, OnShowAudioWidget)
    ON_XTP_EXECUTE(ID_AUDIOWIDGET_SHOWDISKSPACE, OnShowAudioWidget)
    ON_XTP_EXECUTE(ID_AUDIOWIDGET_LOCKPOSITION, OnShowAudioWidget)
    
    ON_XTP_CREATECONTROL()

END_MESSAGE_MAP()

void CLiveContextHelperWnd::KillTimers() {
    if (m_nPresentationBarUpdateTimer != 0)
        KillTimer(m_nPresentationBarUpdateTimer);
    if (m_nPauseButtonTimer != 0)
        KillTimer(m_nPauseButtonTimer);
    if (m_nAudioTimer != 0)
        KillTimer(m_nAudioTimer);
    if (m_nCheckDiskSpaceTimer != 0)
        KillTimer(m_nCheckDiskSpaceTimer);
    if (m_nUpdateRecordTimer != 0)
        KillTimer(m_nUpdateRecordTimer);
    if (m_nUpdateDiscSpaceTimer != 0)
        KillTimer(m_nUpdateDiscSpaceTimer);
}

void CLiveContextHelperWnd::StartPauseTimer() {
    if (m_nPauseButtonTimer == 0)
        m_nPauseButtonTimer = SetTimer(PAUSE_BUTTON, 500, 0);
}

void CLiveContextHelperWnd::StopPauseTimer() {
    if(m_nPauseButtonTimer > 0)
        KillTimer(m_nPauseButtonTimer);
    m_nPauseButtonTimer = 0;
}

void CLiveContextHelperWnd::StartAudioTimer() {
    if (m_nAudioTimer == 0)
        m_nAudioTimer = SetTimer(AUDIO_INDICATOR, 250, 0);
}

void CLiveContextHelperWnd::StartRecordingTimer() {
    if (m_nCheckDiskSpaceTimer == 0) {  
        ASSISTANT::Project::active->CheckDiskSpace(true);
        m_nCheckDiskSpaceTimer = SetTimer(CHECK_DISCSPACE, 1000, 0);
    }

   if (m_nUpdateRecordTimer == 0)
      m_nUpdateRecordTimer = SetTimer(RECORDTIME_TIMER, 1000, 0);
}

LRESULT CLiveContextHelperWnd::OnHotkeys(WPARAM wParam, LPARAM lParam) {
    if (wParam == ID_TOOL_SCREENGRABBING) {
        if (m_pListener != NULL)
            m_pListener->StartStopHotkeyCalled();
    }

    return 1;
}


int CLiveContextHelperWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_nPresentationBarUpdateTimer = SetTimer(PRESENTATIONBAR_UPDATE, 20, NULL);

    return 0;
}

void CLiveContextHelperWnd::OnTimer(UINT_PTR nIDEvent)
{
    if (m_pPresentationBar != NULL) {
        if (nIDEvent == m_nPresentationBarUpdateTimer)
            m_pPresentationBar->UpdateDialogControls(m_pPresentationBar, FALSE);
        else if (nIDEvent == m_nPauseButtonTimer) {
            m_nFlashPause = 1 - m_nFlashPause;
            m_pPresentationBar->SetPauseButtonFlash(m_nFlashPause);
        } else if (nIDEvent == m_nAudioTimer) {
            double dAudioValue = 0;
            if (ASSISTANT::Project::active != NULL)
                dAudioValue = ASSISTANT::Project::active->GetAudioLevel();
            if (m_pListener != NULL)
                m_pListener->ChangeAudioValue(dAudioValue);
        }
    }
    if (nIDEvent == m_nCheckDiskSpaceTimer) {
        if (ASSISTANT::Project::active != NULL)
            ASSISTANT::Project::active->CheckDiskSpace(false);
    }
    if (nIDEvent == m_nUpdateRecordTimer) {
        if (m_pListener != NULL)
            m_pListener->UpdateAudioWidget();
    }

    CWnd::OnTimer(nIDEvent);
}

void CLiveContextHelperWnd::OnPresentationBarContextMenu(NMHDR* pNMHDR, LRESULT* pResult) {
    if (m_pListener != NULL) {
        m_pListener->OnPresentationBarContextMenu(pNMHDR, pResult);
    }
}

void CLiveContextHelperWnd::OnUpdatePresentationBar(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}

void CLiveContextHelperWnd::OnXTPGalleryMarkers(NMHDR* pNMHDR, LRESULT* pResult) {
    if (m_pListener != NULL)
        m_pListener->XTPGalleryMarkers(pNMHDR, pResult);
}

void CLiveContextHelperWnd::OnXTPGalleryPens(NMHDR* pNMHDR, LRESULT* pResult) {
    if (m_pListener != NULL)
        m_pListener->XTPGalleryPens(pNMHDR, pResult);
}

int CLiveContextHelperWnd::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl) {  
    BOOL bFound = FALSE;

    if (m_pListener == NULL)
        return FALSE;

    CXTPCommandBars *pCommandBars = m_pListener->GetCommandBars();

    if (lpCreateControl->nID == ID_PRESENTATION_PENCILS) {
        CXTPControlGalleryItems *m_pItemsPens = CXTPControlGalleryItems::CreateItems(pCommandBars, ID_PRESENTATION_PENCILS);

        m_pItemsPens->GetImageManager()->SetMaskColor(0x000000);
        m_pItemsPens->GetImageManager()->SetIcons(IDB_TAB_TOOLS, 0, 0, CSize(32, 32)); // Get images from same id.
        m_pItemsPens->GetImageManager()->SetMaskColor(COLORREF_NULL); // Default TopLeft Pixel.
        m_pItemsPens->ClipItems(TRUE);
        m_pItemsPens->SetItemSize(CSize(38, 38));
        // Add each item to the gallery list

        m_pItemsPens->AddItem(ID_TOOL_PENBLUE, 9);
        m_pItemsPens->AddItem(ID_TOOL_PENBLACK, 8);
        m_pItemsPens->AddItem(ID_TOOL_PENCYAN, 10);
        m_pItemsPens->AddItem(ID_TOOL_PENGREEN, 11);
        m_pItemsPens->AddItem(ID_TOOL_PENYELLOW, 12);
        m_pItemsPens->AddItem(ID_TOOL_PENRED, 13);
        m_pItemsPens->AddItem(ID_TOOL_PENMAGENTA, 14);
        m_pItemsPens->AddItem(ID_TOOL_PENWHITE, 15);


        CXTPControlGallery* pControlGallery = new CXTPControlGallery();
        pControlGallery->SetControlSize(CSize(32*4 + 6*4 /*4 columns. 32 is button size, 6 is button margins*/, 32 * 2 + 6*2 /*2 lines*/));
        pControlGallery->SetItems(m_pItemsPens);
        pControlGallery->ShowScrollBar(FALSE);

        lpCreateControl->pControl = pControlGallery;

        return TRUE;
    }

    if (lpCreateControl->nID == ID_PRESENTATION_MARKERS) {
        CXTPControlGalleryItems *m_pItemsMarkers = CXTPControlGalleryItems::CreateItems(pCommandBars, ID_PRESENTATION_MARKERS);

        m_pItemsMarkers->GetImageManager()->SetMaskColor(0x000000);
        m_pItemsMarkers->GetImageManager()->SetIcons(IDB_TAB_TOOLS, 0, 0, CSize(32, 32));
        m_pItemsMarkers->GetImageManager()->SetMaskColor(COLORREF_NULL); // Default TopLeft Pixel.
        m_pItemsMarkers->ClipItems(TRUE);
        m_pItemsMarkers->SetItemSize(CSize(38, 38));
        // Add each item to the gallery list

        m_pItemsMarkers->AddItem(ID_TOOL_MARKERBLUE,6);
        m_pItemsMarkers->AddItem(ID_TOOL_MARKERORANGE,3);
        m_pItemsMarkers->AddItem(ID_TOOL_MARKERRED,4);
        m_pItemsMarkers->AddItem(ID_TOOL_MARKERGREEN,5);
        m_pItemsMarkers->AddItem(ID_TOOL_MARKERYELLOW,2);
        m_pItemsMarkers->AddItem(ID_TOOL_MARKERMAGENTA,7);

        CXTPControlGallery* pControlGallery = new CXTPControlGallery();
        pControlGallery->SetControlSize(CSize(32*3 + 6*3 /*3 columns. 32 is button size, 6 is difference between ItemSize and IconSize*/, 32 * 2 + 6*2 /*2 lines*/));
        pControlGallery->SetItems(m_pItemsMarkers);
        pControlGallery->ShowScrollBar(FALSE);

        lpCreateControl->pControl = pControlGallery;

        return TRUE;
    }

    return FALSE;
}

void CLiveContextHelperWnd::OnEnableGalleryButton(CCmdUI* pCmdUI) {
    if (m_pListener != NULL)
        m_pListener->EnableGalleryButton(pCmdUI);
}

void CLiveContextHelperWnd::OnUpdateAudioWidget(CCmdUI* pCmdUI) {
    if (m_pListener != NULL)
        m_pListener->UpdateAudioContextMenu(pCmdUI);
}

void CLiveContextHelperWnd::OnShowAudioWidget(NMHDR* pNMHDR, LRESULT* pResult) {
    if (m_pListener != NULL)
        m_pListener->ChangeAudioWidget(pNMHDR, pResult);
}