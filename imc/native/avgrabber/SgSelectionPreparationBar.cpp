// CSgSelectionPreparationBar.cpp: Implementierungsdatei
//

#include "global.h"
#include "resource.h"
#include "SgSelectionPreparationBar.h"

#include "MfcUtils.h"       // lsutl32
//#include "UtileDefines.h"

// PreparationBarListener 

// CSgSelectionPreparationBar

IMPLEMENT_DYNAMIC(CSgSelectionPreparationBar, CWnd)

BEGIN_MESSAGE_MAP(CSgSelectionPreparationBar, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()

    ON_COMMAND(IDC_RECORDFULLSCREEN, &CSgSelectionPreparationBar::OnChangeRecordFullScreen)
    ON_COMMAND(IDC_RECORDCUSTOM, &CSgSelectionPreparationBar::OnChangeRecordCustom)
    ON_COMMAND(IDC_SHOWPRESENTATIONBAR, &CSgSelectionPreparationBar::OnShowHidePresentationBar)

    /*ON_EN_CHANGE*/ON_EN_UPDATE(IDC_EDIT_X, &CSgSelectionPreparationBar::OnChangeEditX)
    /*ON_EN_CHANGE*/ON_EN_UPDATE(IDC_EDIT_Y, &CSgSelectionPreparationBar::OnChangeEditY)
    /*ON_EN_CHANGE*/ON_EN_UPDATE(IDC_EDIT_WIDTH, &CSgSelectionPreparationBar::OnChangeEditWidth)
    /*ON_EN_CHANGE*/ON_EN_UPDATE(IDC_EDIT_HEIGHT, &CSgSelectionPreparationBar::OnChangeEditHeight)
    ON_LBN_SELCHANGE(IDC_COMBO_START, &CSgSelectionPreparationBar::OnChangeHotKey)
    ON_BN_CLICKED(IDC_CHECK_START_CTRL, &CSgSelectionPreparationBar::OnChangeHotKey)
    ON_BN_CLICKED(IDC_CHECK_START_ALT, &CSgSelectionPreparationBar::OnChangeHotKey)
    ON_BN_CLICKED(IDC_CHECK_START_SHIFT, &CSgSelectionPreparationBar::OnChangeHotKey)

    ON_BN_CLICKED(IDC_SHOWAUDIOSETTINGS, OnShowAudioSettings)
    ON_BN_CLICKED(IDC_SHOWSGSETTINGS, OnShowSgSettings)
    ON_BN_CLICKED(IDC_STARTSELECTION, OnStartSelection)
    ON_BN_CLICKED(IDC_STARTRECORDING, OnStartRecording)
    ON_BN_CLICKED(IDC_CANCELSELECTION, OnCancelSelection)

    ON_UPDATE_COMMAND_UI(IDC_RECORDFULLSCREEN, &CSgSelectionPreparationBar::OnUpdateRecordFullScreen)
    ON_UPDATE_COMMAND_UI(IDC_RECORDCUSTOM, &CSgSelectionPreparationBar::OnUpdateRecordCustom)
    ON_UPDATE_COMMAND_UI(IDC_EDIT_X, &CSgSelectionPreparationBar::OnUpdateDimensionEdit)
    ON_UPDATE_COMMAND_UI(IDC_SPIN_X, &CSgSelectionPreparationBar::OnUpdateDimensionEdit)
    ON_UPDATE_COMMAND_UI(IDC_EDIT_Y, &CSgSelectionPreparationBar::OnUpdateDimensionEdit)
    ON_UPDATE_COMMAND_UI(IDC_SPIN_Y, &CSgSelectionPreparationBar::OnUpdateDimensionEdit)
    ON_UPDATE_COMMAND_UI(IDC_EDIT_WIDTH, &CSgSelectionPreparationBar::OnUpdateDimensionEdit)
    ON_UPDATE_COMMAND_UI(IDC_SPIN_WIDTH, &CSgSelectionPreparationBar::OnUpdateDimensionEdit)
    ON_UPDATE_COMMAND_UI(IDC_EDIT_HEIGHT, &CSgSelectionPreparationBar::OnUpdateDimensionEdit)
    ON_UPDATE_COMMAND_UI(IDC_SPIN_HEIGHT, &CSgSelectionPreparationBar::OnUpdateDimensionEdit)
    ON_UPDATE_COMMAND_UI(IDC_AUDIO_SLIDER, &CSgSelectionPreparationBar::OnUpdateAudioSlider)
    ON_UPDATE_COMMAND_UI(IDC_SHOWPRESENTATIONBAR, &CSgSelectionPreparationBar::OnUpdateShowHidePresentationBar)
    ON_UPDATE_COMMAND_UI(IDC_STARTSELECTION, &CSgSelectionPreparationBar::OnUpdateStartSelection)

    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_AUDIO_SLIDER, &CSgSelectionPreparationBar::OnReleasedcaptureAudioSlider)

    //ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    //ON_WM_LBUTTONUP()
    ON_WM_SETCURSOR()
    ON_WM_MOVE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


CSgSelectionPreparationBar::CSgSelectionPreparationBar(AvGrabber *pAvGrabber, 
                                                       SGSelectionListener *pSelectionListener,
                                                       SGSettingsListener *pSettingsListener,
                                                       SGSelectAreaListener *pSelectAreaListener)
{
    m_pAvGrabber = pAvGrabber;

    m_pAudioInputLevelIndicator = NULL;

    m_bShowPresentationBar = TRUE;

    m_pSelectionListener = pSelectionListener;
    m_pSettingsListener = pSettingsListener;
    m_pSelectAreaListener = pSelectAreaListener;

    m_bRecordFullScreen = m_pSelectionListener->IsDoRecordFullScreen();
    m_bFromSelection = false;
    m_bLeftButtonPressed = false;
    m_ptStartDrag.x = -1;
    m_ptStartDrag.x = -1;

    m_pPresentationBar = pSelectAreaListener->GetPresentationBar();
    m_pWidgetRef = pSelectAreaListener->GetWidget();
}

CSgSelectionPreparationBar::~CSgSelectionPreparationBar()
{
    if (m_pAudioInputLevelIndicator != NULL)
        delete m_pAudioInputLevelIndicator;
}



// CSgSelectionPreparationBar-Meldungshandler

void CSgSelectionPreparationBar::OnDestroy()
{
   //KillTimer(13);
    CWnd::OnDestroy();
}

void CSgSelectionPreparationBar::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CRect rcWindow;
    GetClientRect(&rcWindow);

    Gdiplus::Graphics graphics(dc.m_hDC);
    Gdiplus::SolidBrush transpBrush(Gdiplus::Color(255, 255, 255, 254));
    graphics.FillRectangle(&transpBrush, 0, 0, rcWindow.Width(), rcWindow.Height());
    Gdiplus::Image* pImage = LoadImage(IDB_PREPARATION_BAR_BKG, _T("PNG")/*, ::AfxGetResourceHandle()*/);
         //graphics.DrawImage(&bitmap, (Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, (Gdiplus::REAL)rect.Width(), (Gdiplus::REAL)rect.Height());
    graphics.DrawImage(pImage, rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height());
    // Draw background with gradient
    /*TRIVERTEX tv[2] = { {0, 0, 255*256, 255*256, 255*256, 65280},
    {rcWindow.right, rcWindow.bottom, 91*256, 109*256, 126*256, 65280} };
    GRADIENT_RECT gr[1] = { {0, 1} };
    dc.GradientFill(tv, 2, gr, 1, GRADIENT_FILL_RECT_V);

    CPen bluePen(PS_SOLID, 1, RGB(175, 195, 213));
    dc.SelectObject(&bluePen);
    LOGBRUSH logBrush;
    logBrush.lbStyle = BS_HOLLOW;
    CBrush transparentBrush;
    transparentBrush.CreateBrushIndirect(&logBrush);
    dc.SelectObject(&transparentBrush);

    dc.RoundRect(m_rcSelectionButtons.left, m_rcSelectionButtons.top, m_rcSelectionButtons.right, m_rcSelectionButtons.bottom, 6, 6);
    dc.RoundRect(m_rcAudioSettings.left, m_rcAudioSettings.top, m_rcAudioSettings.right, m_rcAudioSettings.bottom, 6, 6);
    dc.RoundRect(m_rcOtherButtons.left, m_rcOtherButtons.top, m_rcOtherButtons.right, m_rcOtherButtons.bottom, 6, 6);*/

    m_btnRecordFullScreen.UpdateBackground();
    m_btnRecordCustom.UpdateBackground();
    m_btnCancelSelection.UpdateBackground();

    UpdateDialogControls(this, FALSE);
}

int CSgSelectionPreparationBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    //Create font 1
   LOGFONT logFontBtn1, logFontBtn2, logFont3;
   XTPPaintManager()->GetRegularFont()->GetLogFont(&logFontBtn1);
   //logFontBtn1.lfHeight += 0.1;
   
   m_Font1.CreateFontIndirect(&logFontBtn1);
   XTPPaintManager()->GetRegularFont()->GetLogFont(&logFontBtn2);
   logFontBtn2.lfHeight += 2;
   m_Font2.CreateFontIndirect(&logFontBtn2);

   XTPPaintManager()->GetRegularFont()->GetLogFont(&logFont3);
   logFont3.lfHeight += 4;
   m_Font3.CreateFontIndirect(&logFont3);
   

    CRect rcClient;
    GetClientRect(&rcClient);
    // Create close button
    CRect rcElement(rcClient.right - 18 - 2/*- 6*/, rcClient.top,
                    rcClient.right -2/*-0 - 6*/, rcClient.top+18);
    CRect rcButton;
    rcButton.SetRect(0,0, 16, 16);
    BOOL bCreate = m_btnCancelSelection.Create(_T(""), WS_CHILD | WS_VISIBLE /*| BS_PUSHBUTTON*/,
                                rcButton, this, IDC_CANCELSELECTION);
    if(bCreate == TRUE)
    {
       
       SetButtonImage(&m_btnCancelSelection, IDR_CLOSE_SGHINTS, CSize(16, 16));
       m_btnCancelSelection.SetFlatStyle(true);
       m_btnCancelSelection.SetTheme(xtpButtonThemeOffice2007);
       m_btnCancelSelection.SetShowFocus(FALSE);
       m_btnCancelSelection.SetTransparent(TRUE);
      
    }
    m_btnCancelSelection.SetWindowPos(NULL, rcElement.left, rcElement.top, rcElement.Width(), rcElement.Height(), SWP_NOZORDER);


    UINT nYPos = 7;
    CreateSgSelectionButtons(nYPos);
    CreateAudioButtons(nYPos);
    CreateSgSettingButtons(nYPos);

    UpdateDialogControls(this, FALSE);
    //SetTimer(13, 10, 0);

    if(m_pPresentationBar)
    {
       m_pPresentationBar->DestroyWindow();
       m_pPresentationBar->CreateEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, 
          AfxRegisterWndClass(0,0,0,0), NULL, 
          WS_POPUP | WS_VISIBLE, CRect(100, 100, 400, 200), this, 0);

       m_pPresentationBar->SetLayeredWindowAttributes(RGB(255, 255, 255), (255 * 30) / 100, LWA_ALPHA | LWA_COLORKEY);
    }
    
    if(m_pPresentationBar!=NULL && m_bShowPresentationBar)
    {
       m_pPresentationBar->ShowWindow(SW_SHOW);
    }

    if(m_pWidgetRef)
    {
       CRect rcWidget(0,0,100,80);
       if ( m_pWidgetRef->GetSafeHwnd() != NULL)
         m_pWidgetRef->GetWindowRect(rcWidget);
       m_pWidgetRef->DestroyWindow();
       m_pWidgetRef->CreateEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, 
          AfxRegisterWndClass(0,0,0,0), NULL, 
          WS_POPUP | WS_VISIBLE, CRect(100, 100, 400, 200), this, 0);
       m_pWidgetRef->StartSavePosition();
       // hide it for mom. will be displayed manually.
       m_pWidgetRef->ShowWindow(SW_HIDE);
       m_pWidgetRef->UpdateWindowPlacement();// == false )
       //   m_pWidgetRef->SetWindowPos(NULL, 100, 100, rcWidget.Width(), rcWidget.Height(), SWP_NOZORDER);
    }

    UpdateDialogControls(this, FALSE);

    return 0;
}

void CSgSelectionPreparationBar::OnShowAudioSettings() 
{
    ShowAudioSettings();
}

void CSgSelectionPreparationBar::OnShowSgSettings() 
{
    if (m_pSettingsListener == NULL)
        return;

    m_pSettingsListener->ShowSgSettings();

    UpdateHotKeyButton();
    
    UpdateDialogControls(this, FALSE);
}

void CSgSelectionPreparationBar::OnStartSelection() 
{
    if (m_pSelectionListener)
        m_pSelectionListener->StartSelection();
}

void CSgSelectionPreparationBar::OnStartRecording() 
{
    if (m_pSelectAreaListener) {
        // OnChangeHotkey gets the newest information from the dialog
        OnChangeHotKey();
        // This sets the project hotkey and save it to registry
       // m_pSettingsListener->SetSgStartStopHotKey(m_hkStartStop);
        m_pSelectAreaListener->FinishedSgSelection(m_bRecordFullScreen);
    }
}

void CSgSelectionPreparationBar::OnCancelSelection() 
{
    if (m_pSelectAreaListener) {
        // OnChangeHotkey gets the newest information from the dialog
        OnChangeHotKey();
        // This sets the project hotkey and save it to registry
       // m_pSettingsListener->SetSgStartStopHotKey(m_hkStartStop);
        m_pSelectAreaListener->CancelSgSelection();
    }
}

void CSgSelectionPreparationBar::OnChangeRecordFullScreen()
{
    m_bRecordFullScreen = true;
    m_pSelectionListener->SetRecordFullScreen(m_bRecordFullScreen); 

   

    CRect rcDesktop;
    // The first monitor
    MfcUtils::GetDesktopRectangle(rcDesktop);

    CRect rcCustom = GetCustomSelRect();
    if (m_pSelectionListener) {
       m_pSelectionListener->ChangeSelectionRectangle(rcDesktop, false);
       m_pSelectionListener->SetLastCustomRect(rcCustom);
    }

    UpdateDialogControls(this, FALSE);
}

void CSgSelectionPreparationBar::OnChangeRecordCustom()
{
    m_bRecordFullScreen = false;
    m_pSelectionListener->SetRecordFullScreen(m_bRecordFullScreen); 

    CRect rcCustom = GetCustomSelRect();

    if (m_pSelectionListener) {
       m_pSelectionListener->ChangeSelectionRectangle(rcCustom, false);
       m_pSelectionListener->SetLastCustomRect(rcCustom);
    }

    UpdateDialogControls(this, FALSE);
}

void CSgSelectionPreparationBar::OnShowHidePresentationBar()
{
    if (m_bShowPresentationBar == TRUE)
        m_bShowPresentationBar = FALSE;
    else
        m_bShowPresentationBar = TRUE;
    m_pSelectionListener->SetShowPresentationBar(m_bShowPresentationBar == TRUE);

    if(m_pPresentationBar != NULL)
    {
       m_pPresentationBar->ShowWindow(m_bShowPresentationBar);
       //m_pPresentationBar->SetForegroundWindow();
    }

    UpdateDialogControls(this, FALSE);
}

void CSgSelectionPreparationBar::OnUpdateRecordFullScreen(CCmdUI *pCmdUI)
{
    if (m_bRecordFullScreen) {
        pCmdUI->SetCheck(BST_CHECKED);
        m_btnRecordFullScreen.SetChecked(TRUE);
    }
    else {
        pCmdUI->SetCheck(BST_UNCHECKED);
        m_btnRecordFullScreen.SetChecked(FALSE);
    }
}

void CSgSelectionPreparationBar::OnUpdateRecordCustom(CCmdUI *pCmdUI)
{
    if (m_bRecordFullScreen){
        pCmdUI->SetCheck(BST_UNCHECKED);
        m_btnRecordCustom.SetChecked(FALSE);
    }
    else {
        pCmdUI->SetCheck(BST_CHECKED);
        m_btnRecordCustom.SetChecked(TRUE);
    }
}

void CSgSelectionPreparationBar::OnUpdateDimensionEdit(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!m_bRecordFullScreen);
}

void CSgSelectionPreparationBar::OnUpdateAudioSlider(CCmdUI *pCmdUI)
{
    if (m_pAvGrabber && 
        m_pAvGrabber->getAudioMixer() && 
        m_pAvGrabber->getAudioMixer()->getSourceCount() > 0)
        pCmdUI->Enable(true);
    else
        pCmdUI->Enable(false);

    if (m_pAvGrabber && GetKeyState(VK_LBUTTON) >= 0) // left mouse not key pressed)
    {
        AudioMixer *pAudioMixer = m_pAvGrabber->getAudioMixer();
        if (pAudioMixer) {
            UINT nMixerVolume = pAudioMixer->getVolume();
            if (nMixerVolume < 0)
                nMixerVolume = 0;

            if (nMixerVolume > 100)
                nMixerVolume = 100;

            m_sldAudioInputLevel.SetPos(100 - nMixerVolume);
        }
    }
}

void CSgSelectionPreparationBar::OnUpdateShowHidePresentationBar(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_bShowPresentationBar);
}

void CSgSelectionPreparationBar::OnUpdateStartSelection(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(!m_bRecordFullScreen);
}

void CSgSelectionPreparationBar::OnReleasedcaptureAudioSlider(NMHDR* pNMHDR, LRESULT* pResult) 
{
    if (m_pAvGrabber) {
        
        AudioMixer *pAudioMixer = m_pAvGrabber->getAudioMixer();
        if (pAudioMixer) {
            UINT nCode = pNMHDR->code;
            pAudioMixer->setVolume(100 - m_sldAudioInputLevel.GetPos());
        }
    }

    *pResult = 0;
}

void CSgSelectionPreparationBar::OnChangeEditX()
{
    if (m_pSelectionListener == NULL)
        return;

    CString csEditX;
    m_edtLeft.GetWindowText(csEditX);
    int iNewX = _ttoi(csEditX);

    CRect rcSelection = m_pSelectionListener->GetSelectionRect();

    CRect rcDesktop;
    MfcUtils::GetDesktopRectangle(rcDesktop);
    if (iNewX < rcDesktop.left) {
        iNewX = rcDesktop.left;
        CString csX;
        csX.Format(_T("%d"), iNewX);
        m_edtLeft.SetWindowText(csX); 
    }
    if (m_edtWidth.GetSafeHwnd() != NULL && iNewX + rcSelection.Width() > rcDesktop.right) {
        int iNewWidth = rcDesktop.right - iNewX;
        CString csWidth;
        csWidth.Format(_T("%d"), iNewWidth);
        m_edtWidth.SetWindowText(csWidth);
    }

    rcSelection.right = iNewX + rcSelection.Width();
    rcSelection.left = iNewX;

    if(!m_bFromSelection && !m_bRecordFullScreen) {
       m_pSelectionListener->ChangeSelectionRectangle(rcSelection, false);
       m_pSelectionListener->SetLastCustomRect(rcSelection);
    }
}

void CSgSelectionPreparationBar::OnChangeEditY()
{
    if (m_pSelectionListener == NULL)
        return;

    CString csEditY;
    m_edtTop.GetWindowText(csEditY);
    int iNewY = _ttoi(csEditY);

    CRect rcSelection = m_pSelectionListener->GetSelectionRect();

    CRect rcDesktop;
    MfcUtils::GetDesktopRectangle(rcDesktop);
    if (iNewY < rcDesktop.top) {
        iNewY = rcDesktop.top;
        CString csY;
        csY.Format(_T("%d"), iNewY);
        m_edtTop.SetWindowText(csY);
    }
    if (m_edtHeight.GetSafeHwnd() != NULL && iNewY + rcSelection.Height() > rcDesktop.bottom) {
        int iNewHeight = rcDesktop.bottom - iNewY;
        CString csHeight;
        csHeight.Format(_T("%d"), iNewHeight);
        m_edtHeight.SetWindowText(csHeight);
    }

    rcSelection.bottom = iNewY + rcSelection.Height();
    rcSelection.top = iNewY;

    if(!m_bFromSelection && !m_bRecordFullScreen) {
       m_pSelectionListener->ChangeSelectionRectangle(rcSelection, false);
       m_pSelectionListener->SetLastCustomRect(rcSelection);
    }
}

void CSgSelectionPreparationBar::OnChangeEditWidth()
{
    if (m_pSelectionListener == NULL)
        return;

    CString csEditWidth;
    m_edtWidth.GetWindowText(csEditWidth);
    int iNewWidth = _ttoi(csEditWidth);
    
    CRect rcSelection = m_pSelectionListener->GetSelectionRect();

    CRect rcDesktop;
    MfcUtils::GetDesktopRectangle(rcDesktop);
    if (rcSelection.left + iNewWidth > rcDesktop.right) {
        iNewWidth = rcDesktop.right - rcSelection.left;
        CString csWidth;
        csWidth.Format(_T("%d"), iNewWidth);
        m_edtWidth.SetWindowText(csWidth);
    }
    
    rcSelection.right = rcSelection.left + iNewWidth;

    if(!m_bFromSelection && !m_bRecordFullScreen) {
       m_pSelectionListener->ChangeSelectionRectangle(rcSelection, false);
       m_pSelectionListener->SetLastCustomRect(rcSelection);
    }
}

void CSgSelectionPreparationBar::OnChangeEditHeight()
{
    if (m_pSelectionListener == NULL)
        return;

    CString csEditHeight;
    m_edtHeight.GetWindowText(csEditHeight);
    int iNewHeight = _ttoi(csEditHeight);
    
    CRect rcSelection = m_pSelectionListener->GetSelectionRect();

    CRect rcDesktop;
    MfcUtils::GetDesktopRectangle(rcDesktop);
    if (rcSelection.top + iNewHeight > rcDesktop.bottom) {
        iNewHeight = rcDesktop.bottom - rcSelection.top;
        CString csHeight;
        csHeight.Format(_T("%d"), iNewHeight);
        m_edtHeight.SetWindowText(csHeight);
    }

    rcSelection.bottom = rcSelection.top + iNewHeight;

    if(!m_bFromSelection && !m_bRecordFullScreen) {
       m_pSelectionListener->ChangeSelectionRectangle(rcSelection, false);
       m_pSelectionListener->SetLastCustomRect(rcSelection);
    }
}

void CSgSelectionPreparationBar::OnChangeHotKey()
{
    //m_cmbStartStop.GetWindowText(m_hkStartStop.vKeyString);
    CString cs;
    m_cmbStartStop.GetLBText(m_cmbStartStop.GetCurSel(), m_hkStartStop.vKeyString);
    
    CButton *pCheckButton = NULL;
    pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_START_CTRL);
    m_hkStartStop.hasCtrl = pCheckButton->GetCheck() == 0 ? false: true;
    pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_START_SHIFT);
    m_hkStartStop.hasShift = pCheckButton->GetCheck() == 0 ? false: true;
    pCheckButton = (CButton *)GetDlgItem(IDC_CHECK_START_ALT);
    m_hkStartStop.hasAlt = pCheckButton->GetCheck() == 0 ? false: true;;
    m_pSettingsListener->SetSgStartStopHotKey(m_hkStartStop);
}

void CSgSelectionPreparationBar::UpdateSelectionRectangle(CRect &rcSelection)
{
    CString csText;
  
    //m_bRecordFullScreen = FALSE;
    
    m_bFromSelection = true;
    if (m_edtLeft.m_hWnd != NULL) {
        csText.Format(_T("%d"), rcSelection.left);
        m_edtLeft.SetWindowText(csText);
    }

    if (m_edtTop.m_hWnd != NULL) {
        csText.Format(_T("%d"), rcSelection.top);
        m_edtTop.SetWindowText(csText);
    }

    if (m_edtWidth.m_hWnd != NULL) {
        csText.Format(_T("%d"), rcSelection.Width());
        m_edtWidth.SetWindowText(csText);
    }

    if (m_edtHeight.m_hWnd != NULL) {
        csText.Format(_T("%d"), rcSelection.Height());
        m_edtHeight.SetWindowText(csText);
    }
    
    m_bFromSelection = false;
}

void CSgSelectionPreparationBar::UpdateControls(CRect &rcSelection)
{
    UpdateDialogControls(this, FALSE);
}

void CSgSelectionPreparationBar::ShowAudioSettings()
{
    if (m_pSettingsListener)
        m_pSettingsListener->ShowAudioSettings();
    
    UpdateDialogControls(this, FALSE);
}

int CSgSelectionPreparationBar::GetAudioLevel()
{
    int iAudioLevel = 0;

    if (m_pAvGrabber != NULL)
        iAudioLevel = m_pAvGrabber->getAudioLevel();

    return iAudioLevel;
}

void CSgSelectionPreparationBar::CreateSgSelectionButtons(UINT &nYPos)
{
    CRect rcClient;
    GetClientRect(&rcClient);

    CRect rcElement;
    CPoint ptElementPosition;
    CSize szElement;

    m_rcSelectionButtons.SetRectEmpty();
    m_rcSelectionButtons.left = X_OFFSET_SMALL;
    m_rcSelectionButtons.top = nYPos + Y_OFFSET_SMALL;

    ptElementPosition.SetPoint(X_OFFSET + X_OFFSET_SMALL, nYPos + Y_OFFSET * 2);

    // "FullScreen"  button
    //szElement.SetSize(RADIOBUTTON_WIDTH, BUTTON_HEIGHT);
    CString csTitle;
    csTitle = LoadMyString(IDS_PREP_FULLSCREEN);
    szElement.SetSize(SEL_MODE_WIDTH, SEL_MODE_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_btnRecordFullScreen.Create(/*_T("Fullscreen")*/csTitle, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 
        rcElement, this, IDC_RECORDFULLSCREEN);
    SetButtonImage(&m_btnRecordFullScreen, IDR_FULL_SCREEN, CSize(32, 32));
    m_btnRecordFullScreen.SetFlatStyle(TRUE);
    m_btnRecordFullScreen.SetTheme(xtpButtonThemeOffice2007/*xtpButtonThemeStandard*/);
    m_btnRecordFullScreen.SetTextImageRelation(xtpButtonImageAboveText);
    m_btnRecordFullScreen.SetShowFocus(FALSE);
    m_btnRecordFullScreen.SetTransparent(TRUE);
    m_btnRecordFullScreen.SetFont(&m_Font1);
    m_btnRecordFullScreen.SetChecked(m_bRecordFullScreen);
    //ptElementPosition.Offset(0, BUTTON_HEIGHT + Y_OFFSET);
    ptElementPosition.Offset(SEL_MODE_WIDTH + X_OFFSET_SMALL, 0);

    // "Custom"  button
    //szElement.SetSize(RADIOBUTTON_WIDTH, BUTTON_HEIGHT);
    szElement.SetSize(SEL_MODE_WIDTH, SEL_MODE_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    csTitle.Empty();
    csTitle = LoadMyString(IDS_PREP_CUSTOM);
    m_btnRecordCustom.Create(/*_T("Custom Area")*/csTitle, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 
        rcElement, this, IDC_RECORDCUSTOM);
    SetButtonImage(&m_btnRecordCustom, IDR_CUSTOM_AREA, CSize(32, 32));
    m_btnRecordCustom.SetFlatStyle(TRUE);
    m_btnRecordCustom.SetTheme(xtpButtonThemeOffice2007/*xtpButtonThemeStandard*/);
    m_btnRecordCustom.SetTextImageRelation(xtpButtonImageAboveText);
    m_btnRecordCustom.SetShowFocus(FALSE);
    m_btnRecordCustom.SetTransparent(TRUE);
    m_btnRecordCustom.SetFont(&m_Font1);
    m_btnRecordCustom.SetChecked(!m_bRecordFullScreen);
    //ptElementPosition.Offset(0, BUTTON_HEIGHT + Y_OFFSET);
    ptElementPosition.Offset(-(SEL_MODE_WIDTH + X_OFFSET_SMALL), SEL_MODE_HEIGHT + Y_OFFSET);

    
    CRect rcDesktop;
    MfcUtils::GetDesktopRectangle(rcDesktop);

    UINT nEditStyle = WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_CENTER;
    UINT nSpinStyle = WS_CHILD | WS_VISIBLE | 
        UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS| UDS_NOTHOUSANDS;

    CRect rcSelection;
    if (m_pSelectionListener != NULL)
        m_pSelectionListener->GetLastCustomRect(rcSelection);
    else
        rcSelection.SetRectEmpty();

    //Create Edit boxes
    ptElementPosition.Offset(X_OFFSET, Y_OFFSET_SMALL);
    szElement.SetSize(TEXT_WIDTH_SMALL , TEXT_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_txtX.Create(_T("X:"), WS_VISIBLE | WS_CHILD, rcElement, this,IDC_STATIC);
    m_txtX.SetFont(&m_Font2);
    ptElementPosition.Offset(TEXT_WIDTH_SMALL, -Y_OFFSET_SMALL);

    szElement.SetSize(EDIT_WIDTH, EDIT_HEIGHT);
    rcElement.SetRect(ptElementPosition.x , ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_edtLeft.Create(nEditStyle, rcElement, this, IDC_EDIT_X);
    m_edtLeft.SetFont(&m_Font3/*m_Font2*/);
    m_spnLeft.Create(nSpinStyle, rcElement, this, IDC_SPIN_X);
    m_spnLeft.SetRange32(0, rcDesktop.Width());
    m_spnLeft.SetBuddy(&m_edtLeft);
    m_spnLeft.SetPos32(rcSelection.left);
    ptElementPosition.Offset(EDIT_WIDTH + X_OFFSET, 0);

    ptElementPosition.Offset(-2 + X_NEW_OFFSET / 2, Y_OFFSET_SMALL);
    szElement.SetSize(TEXT_WIDTH + X_OFFSET_SMALL , TEXT_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    csTitle.Empty();
    csTitle = LoadMyString(IDS_PREP_WIDTH);
    m_txtWidth.Create(/*_T("Width:")*/csTitle, WS_VISIBLE | WS_CHILD | SS_RIGHT, rcElement, this,IDC_STATIC);
    m_txtWidth.SetFont(&m_Font2);
    ptElementPosition.Offset(TEXT_WIDTH + X_OFFSET_SMALL + 2, -Y_OFFSET_SMALL);

    szElement.SetSize(EDIT_WIDTH, EDIT_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_edtWidth.Create(nEditStyle, rcElement, this, IDC_EDIT_WIDTH);
    m_edtWidth.SetFont(&m_Font3/*m_Font2*/);
    m_spnWidth.Create(nSpinStyle, rcElement, this, IDC_SPIN_WIDTH);
    m_spnWidth.SetRange32(0, rcDesktop.Width());
    m_spnWidth.SetBuddy(&m_edtWidth);
    m_spnWidth.SetPos32(rcSelection.Width());
    ptElementPosition.Offset(-(X_OFFSET + TEXT_WIDTH_SMALL + X_OFFSET_SMALL+ EDIT_WIDTH + TEXT_WIDTH), EDIT_HEIGHT + Y_OFFSET_SMALL);

    ptElementPosition.Offset(/*0 */- X_NEW_OFFSET / 2 , Y_OFFSET_SMALL);
    szElement.SetSize(TEXT_WIDTH_SMALL , TEXT_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_txtY.Create(_T("Y:"), WS_VISIBLE | WS_CHILD, rcElement, this,IDC_STATIC);
    m_txtY.SetFont(&m_Font2);
    ptElementPosition.Offset(TEXT_WIDTH_SMALL, -Y_OFFSET_SMALL);


    szElement.SetSize(EDIT_WIDTH, EDIT_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_edtTop.Create(nEditStyle, rcElement, this, IDC_EDIT_Y);
    m_edtTop.SetFont(&m_Font3/*m_Font2*/);
    m_spnTop.Create(nSpinStyle, rcElement, this, IDC_SPIN_Y);
    m_spnTop.SetRange32(0, rcDesktop.Height());
    m_spnTop.SetBuddy(&m_edtTop);
    m_spnTop.SetPos32(rcSelection.top);
    ptElementPosition.Offset(EDIT_WIDTH + X_OFFSET ,0);

    ptElementPosition.Offset(-2 + X_NEW_OFFSET / 2, Y_OFFSET_SMALL);
    szElement.SetSize(TEXT_WIDTH + X_OFFSET_SMALL, TEXT_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    csTitle.Empty();
    csTitle = LoadMyString(IDS_PREP_HEIGHT);
    m_txtHeight.Create(/*_T("Height:")*/csTitle, WS_VISIBLE | WS_CHILD | SS_RIGHT, rcElement, this,IDC_STATIC);
    m_txtHeight.SetFont(&m_Font2);
    ptElementPosition.Offset(TEXT_WIDTH + X_OFFSET_SMALL + 2, -Y_OFFSET_SMALL);


    szElement.SetSize(EDIT_WIDTH, EDIT_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_edtHeight.Create(nEditStyle, rcElement, this, IDC_EDIT_HEIGHT);
    m_edtHeight.SetFont(&m_Font3/*m_Font2*/);
    m_spnHeight.Create(nSpinStyle, rcElement, this, IDC_SPIN_HEIGHT);
    m_spnHeight.SetRange32(0, rcDesktop.Height());
    m_spnHeight.SetBuddy(&m_edtHeight);
    m_spnHeight.SetPos32(rcSelection.Height());
    ptElementPosition.Offset(-(EDIT_WIDTH  + X_OFFSET + TEXT_WIDTH + TEXT_WIDTH_SMALL), EDIT_HEIGHT + Y_OFFSET);
    ptElementPosition.Offset(-X_OFFSET - X_OFFSET_SMALL - X_NEW_OFFSET / 2, 0);

    // "StartSelection" button
    szElement.SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    csTitle.Empty();
    csTitle = LoadMyString(IDS_PREP_START_SEL);
    m_btnStartSelection.Create(/*_T("Start Selection")*/csTitle, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        rcElement, this, IDC_STARTSELECTION);
    m_btnStartSelection.SetTheme(xtpButtonThemeOffice2007/*xtpButtonThemeStandard*/);
    m_btnStartSelection.SetShowFocus(FALSE);
    m_btnStartSelection.SetFont(&m_Font2);
    ptElementPosition.Offset(0, BUTTON_HEIGHT);

    m_rcSelectionButtons.right = rcClient.Width() - X_OFFSET_SMALL;
    m_rcSelectionButtons.bottom = ptElementPosition.y + Y_OFFSET_SMALL;

    nYPos = ptElementPosition.y + Y_OFFSET;
}

void CSgSelectionPreparationBar::CreateAudioButtons(UINT &nYPos)
{ 
    CRect rcClient;
    GetClientRect(&rcClient);

    CRect rcElement;
    CPoint ptElementPosition;
    CSize szElement;

    m_rcAudioSettings.SetRectEmpty();
    m_rcAudioSettings.left = X_OFFSET_SMALL;
    m_rcAudioSettings.top = nYPos + Y_OFFSET_SMALL;

    ptElementPosition.SetPoint(X_OFFSET + X_OFFSET_SMALL, nYPos + Y_OFFSET);
    CString csTitle;
    csTitle = LoadMyString(IDS_PREP_REC_SETTINGS);

    //Button Record Settings
    szElement.SetSize(BUTTON_WIDTH_SMALL, BUTTON_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_btnShowSgSettings.Create(/*_T("Record Settings")*/csTitle, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        rcElement, this, IDC_SHOWSGSETTINGS);
    m_btnShowSgSettings.SetTheme(xtpButtonThemeOffice2007/*xtpButtonThemeStandard*/);
    m_btnShowSgSettings.SetShowFocus(FALSE);
    m_btnShowSgSettings.SetFont(&m_Font2);
    ptElementPosition.Offset(0, BUTTON_HEIGHT + Y_OFFSET_SMALL);

    //Button Audio Settings
    szElement.SetSize(BUTTON_WIDTH_SMALL, BUTTON_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    csTitle.Empty();
    csTitle = LoadMyString(IDS_PREP_AUDIO_SETTINGS);
    m_btnAudioSettings.Create(/*_T("Audio Settings")*/csTitle, WS_CHILD | WS_VISIBLE, 
                              rcElement, this, IDC_SHOWAUDIOSETTINGS);
    m_btnAudioSettings.SetTheme(xtpButtonThemeOffice2007/*xtpButtonThemeStandard*/);
    m_btnAudioSettings.SetShowFocus(FALSE);
    m_btnAudioSettings.SetFont(&m_Font2);
    //ptElementPosition.Offset(0, BUTTON_HEIGHT);
    ptElementPosition.Offset(BUTTON_WIDTH_SMALL + 2/* + X_NEW_OFFSET*/ /*- 5*X_OFFSET*/, -(BUTTON_HEIGHT + Y_OFFSET));

    //Slider Audio Volume
    //szElement.SetSize(/*SLIDER_WIDTH*/BUTTON_HEIGHT , BUTTON_HEIGHT * 2 + Y_OFFSET * 2);
    szElement.SetSize(24,SLIDER_WIDTH / 2);
    rcElement.SetRect(ptElementPosition.x - 8, ptElementPosition.y -3,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy-1);
    m_sldAudioInputLevel.Create(WS_CHILD | WS_VISIBLE | TBS_VERT |TBS_BOTH |  WS_TABSTOP, 
        rcElement, this, IDC_AUDIO_SLIDER);
    //ptElementPosition.Offset(-(AUDIOLEVEL_WIDTH + X_OFFSET_SMALL), AUDIOLEVEL_HEIGHT + Y_OFFSET); 
    ptElementPosition.Offset(BUTTON_HEIGHT + X_OFFSET_SMALL /*+ X_NEW_OFFSET*/ , 2*Y_OFFSET); 

    //Indicator Audio Level
    szElement.SetSize(AUDIOLEVEL_WIDTH, AUDIOLEVEL_HEIGHT );
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_pAudioInputLevelIndicator = new CAudioLevelIndicator((AudioLevelIndicatorListener *)this);  
    m_pAudioInputLevelIndicator->Create(NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_CENTERIMAGE | SS_BITMAP,
       rcElement, this, IDC_VIEW_AUDIO);
   
    //m_pAudioInputLevelIndicator->SetPosition();
    ptElementPosition.Offset(AUDIOLEVEL_WIDTH + X_OFFSET_SMALL, AUDIOLEVEL_HEIGHT); 

    /*szElement.SetSize(SLIDER_WIDTH, BUTTON_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_sldAudioInputLevel.Create(WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_BOTH | WS_TABSTOP, 
        rcElement, this, IDC_AUDIO_SLIDER);
    ptElementPosition.Offset(-(AUDIOLEVEL_WIDTH + X_OFFSET_SMALL), AUDIOLEVEL_HEIGHT + Y_OFFSET); */

    /*szElement.SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_btnAudioSettings.Create(_T("Audio Settings"), WS_CHILD | WS_VISIBLE, 
                              rcElement, this, IDC_SHOWAUDIOSETTINGS);
    ptElementPosition.Offset(0, BUTTON_HEIGHT);*/

    m_rcAudioSettings.right = rcClient.Width() - X_OFFSET_SMALL;
    m_rcAudioSettings.bottom = ptElementPosition.y + Y_OFFSET_SMALL;

    nYPos = ptElementPosition.y + Y_OFFSET;
}

void CSgSelectionPreparationBar::CreateSgSettingButtons(UINT &nYPos)
{
    CRect rcClient;
    GetClientRect(&rcClient);

    CRect rcElement;
    CPoint ptElementPosition;
    CSize szElement;

    m_rcOtherButtons.SetRectEmpty();
    m_rcOtherButtons.left = X_OFFSET_SMALL;
    m_rcOtherButtons.top = nYPos + Y_OFFSET_SMALL;

    ptElementPosition.SetPoint(3 * X_OFFSET, nYPos + Y_OFFSET);

    //Checkbox Record Audio
    CString csTitle;
    //szElement.SetSize(BUTTON_WIDTH, BUTTON_HEIGHT + Y_OFFSET);
    //rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
    //    ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    //CString csTitle;
    //csTitle = LoadMyString(IDS_PREP_REC_AUDIO);
    //m_btnRecordAudio.Create(/*_T("Record Audio")*/csTitle, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_MULTILINE, 
    //    rcElement, this, IDC_RECORDAUDIO);
    //m_btnRecordAudio.SetFlatStyle(TRUE);
    //m_btnRecordAudio.SetTheme(xtpButtonThemeStandard);
    //m_btnRecordAudio.SetFont(&m_Font1);
    //m_btnRecordAudio.SetTransparent(TRUE);
    //ptElementPosition.Offset(0, BUTTON_HEIGHT + /*2**/Y_OFFSET_SMALL);

    //Checkbox Record Show/Hide Presentationbar
    //szElement.SetSize(RADIOBUTTON_WIDTH, BUTTON_HEIGHT + Y_OFFSET);
    szElement.SetSize(BUTTON_WIDTH, BUTTON_HEIGHT + Y_OFFSET);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    csTitle.Empty();
    csTitle = LoadMyString(IDS_PREP_SHOW_HIDE);
    m_btnShowHidePresentationBar.Create(/*_T("Show/Hide Presentationbar")*/csTitle, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_MULTILINE, 
        rcElement, this, IDC_SHOWPRESENTATIONBAR);
    m_btnShowHidePresentationBar.SetFlatStyle(TRUE);
    m_btnShowHidePresentationBar.SetTheme(xtpButtonThemeStandard);
    m_btnShowHidePresentationBar.SetTransparent(TRUE);
    m_btnShowHidePresentationBar.SetFont(&m_Font1);
    ptElementPosition.Offset(-X_OFFSET, BUTTON_HEIGHT + 2*Y_OFFSET);

    // Hotkey buttons
    // Start/Stop
    szElement.SetSize(BUTTON_WIDTH, 2* TEXT_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    csTitle.Empty();
    csTitle = LoadMyString(IDS_PREP_SHORTCUT);
    m_txtShortcut.Create(/*_T("Shortcut to stop Recording")*/csTitle, WS_VISIBLE | WS_CHILD , rcElement, this,IDC_STATIC);
    m_txtShortcut.SetFont(&m_Font1);
    ptElementPosition.Offset(0, 2*TEXT_HEIGHT + Y_OFFSET_SMALL);

    szElement.SetSize(TEXT_WIDTH, TEXT_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    csTitle.Empty();
    csTitle = LoadMyString(IDS_PREP_CTRL);
    m_txtStartStopUseCtrl.Create(/*_T("ctrl")*/csTitle, WS_VISIBLE | WS_CHILD, rcElement, this,IDC_STATIC);
    m_txtStartStopUseCtrl.SetFont(&m_Font1);
    ptElementPosition.Offset(0, TEXT_HEIGHT);
    szElement.SetSize(CHECKBOX_WIDTH, CHECKBOX_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_btnStartStopUseCtrl.Create(_T("+"), WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | BS_TOP, 
        rcElement, this, IDC_CHECK_START_CTRL);
    m_btnStartStopUseCtrl.SetFlatStyle(TRUE);
    m_btnStartStopUseCtrl.SetTheme(xtpButtonThemeStandard);
    m_btnStartStopUseCtrl.SetFont(&m_Font1);
    m_btnStartStopUseCtrl.SetTransparent(TRUE);
    ptElementPosition.Offset(CHECKBOX_WIDTH + X_OFFSET_SMALL, -TEXT_HEIGHT);

    szElement.SetSize(TEXT_WIDTH, TEXT_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_txtStartStopUseShft.Create(_T("shift"), WS_VISIBLE | WS_CHILD, rcElement, this, IDC_STATIC);
    m_txtStartStopUseShft.SetFont(&m_Font1);
    ptElementPosition.Offset(0, TEXT_HEIGHT);
    szElement.SetSize(CHECKBOX_WIDTH, CHECKBOX_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_btnStartStopUseShft.Create(_T("+"), WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | BS_TOP, 
        rcElement, this, IDC_CHECK_START_SHIFT);
    m_btnStartStopUseShft.SetFlatStyle(TRUE);
    m_btnStartStopUseShft.SetTheme(xtpButtonThemeStandard);
    m_btnStartStopUseShft.SetFont(&m_Font1);
    m_btnStartStopUseShft.SetTransparent(TRUE);
    ptElementPosition.Offset(CHECKBOX_WIDTH + X_OFFSET_SMALL, -TEXT_HEIGHT);

    szElement.SetSize(TEXT_WIDTH, TEXT_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_txtStartStopUseAlt.Create(_T("alt"), WS_VISIBLE | WS_CHILD, rcElement, this,IDC_STATIC);
    m_txtStartStopUseAlt.SetFont(&m_Font1);
    ptElementPosition.Offset(0, TEXT_HEIGHT);
    szElement.SetSize(CHECKBOX_WIDTH, CHECKBOX_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_btnStartStopUseAlt.Create(_T("+"), WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | BS_TOP, 
        rcElement, this, IDC_CHECK_START_ALT);
    m_btnStartStopUseAlt.SetFlatStyle(TRUE);
    m_btnStartStopUseAlt.SetTheme(xtpButtonThemeStandard);
    m_btnStartStopUseAlt.SetFont(&m_Font1);
    m_btnStartStopUseAlt.SetTransparent(TRUE);
    ptElementPosition.Offset(CHECKBOX_WIDTH + X_OFFSET_SMALL, 0);

    szElement.SetSize(COMBO_WIDTH, COMBO_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y-5,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy + 200);
    m_cmbStartStop.Create(WS_VISIBLE | WS_CHILD |CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP, 
        rcElement, this, IDC_COMBO_START);
    m_cmbStartStop.SetFont(&m_Font2);
    ptElementPosition.Offset(-3*(CHECKBOX_WIDTH + X_OFFSET_SMALL), COMBO_HEIGHT /*+ Y_OFFSET*/);

    UpdateHotKeyButton();

    // SG Settings button
    /*szElement.SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    m_btnShowSgSettings.Create(_T("Settings"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        rcElement, this, IDC_SHOWSGSETTINGS);
    ptElementPosition.Offset(0, BUTTON_HEIGHT + Y_OFFSET);*/

    // Record button
    ptElementPosition.Offset(-X_OFFSET_SMALL, 0);
    szElement.SetSize(BUTTON_WIDTH, BUTTON_HEIGHT * 2);
    rcElement.SetRect(ptElementPosition.x, ptElementPosition.y,
        ptElementPosition.x + szElement.cx, ptElementPosition.y + szElement.cy);
    csTitle.Empty();
    csTitle = LoadMyString(IDS_PREP_START_REC);
    m_btnStartRecording.Create(/*_T("Start Recording")*/csTitle, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        rcElement, this, IDC_STARTRECORDING);
    SetButtonImage(&m_btnStartRecording, IDR_RECORD, CSize(32, 32));
    m_btnStartRecording.SetTheme(xtpButtonThemeOffice2007/*xtpButtonThemeStandard*/);
    m_btnStartRecording.SetShowFocus(FALSE);
    m_btnStartRecording.SetFont(&m_Font2);
    ptElementPosition.Offset(0, BUTTON_HEIGHT);

    m_rcOtherButtons.right = rcClient.Width() - X_OFFSET_SMALL;
    m_rcOtherButtons.bottom = ptElementPosition.y + Y_OFFSET_SMALL;

    nYPos = ptElementPosition.y + Y_OFFSET;

}

void CSgSelectionPreparationBar::UpdateHotKeyButton()
{
    if (m_pSettingsListener == NULL)
        return;

    m_pSettingsListener->GetSgStartStopHotKey(m_hkStartStop);

    // Filll combo with list of possible keys for hot key
    CStringArray arPossibleKeys;
    m_pSettingsListener->GetPossibleKeys(arPossibleKeys);
    bool bSelected = false;
    for(int i = 0; i < arPossibleKeys.GetSize(); i++)
    {
        m_cmbStartStop.AddString(arPossibleKeys[i]);

        if (m_hkStartStop.vKeyString == arPossibleKeys[i]) {
            m_cmbStartStop.SetCurSel(i);
            bSelected = true;
        }
    }
    if (!bSelected)
        m_cmbStartStop.SetCurSel(0);


    // StartStop check boxes
    if (m_hkStartStop.hasCtrl)
        m_btnStartStopUseCtrl.SetCheck(BST_CHECKED);
    else
        m_btnStartStopUseCtrl.SetCheck(BST_UNCHECKED);

    if (m_hkStartStop.hasShift)
        m_btnStartStopUseShft.SetCheck(BST_CHECKED);
    else
        m_btnStartStopUseShft.SetCheck(BST_UNCHECKED);
    
    if (m_hkStartStop.hasAlt)
        m_btnStartStopUseAlt.SetCheck(BST_CHECKED);
    else
        m_btnStartStopUseAlt.SetCheck(BST_UNCHECKED);
    
}

BOOL CSgSelectionPreparationBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   CPoint ptCursor;
   GetCursorPos(&ptCursor);
   CRect rcClient;
   GetClientRect(&rcClient);

   CRect rcDrag;
   rcDrag.SetRect(rcClient.left, rcClient.top, rcClient.right, rcClient.top + 15);

   ScreenToClient(&ptCursor);
   if(rcDrag.PtInRect(ptCursor))
      SetCursor(LoadCursor(NULL, IDC_HAND));

   return CWnd::OnSetCursor(pWnd, nHitTest, message);;
}

//void CSgSelectionPreparationBar::OnMouseMove(UINT nFlags, CPoint point)
//{
//   /*if(m_bLeftButtonPressed)
//   {
//      int cx = m_ptStartDrag.x - point.x;
//      int cy = m_ptStartDrag.y - point.y;
//      CRect rcClient;
//      GetClientRect(&rcClient);
//      ::SetWindowPos( m_hWnd
//         , NULL
//         , cx
//         , cy
//         , 0
//         , 0
//         , SWP_NOZORDER | SWP_NOSIZE
//         );
//      m_ptStartDrag = point;
//   }*/
//   CWnd::OnMouseMove(nFlags, point);
//}

void CSgSelectionPreparationBar::OnLButtonDown(UINT nFlags, CPoint point)
{
   CRect rcClient;
   GetClientRect(&rcClient);

   CRect rcDrag;
   rcDrag.SetRect(rcClient.left, rcClient.top, rcClient.right, rcClient.top + 15);
   if(rcDrag.PtInRect(point))
   {
      /*m_ptStartDrag.x = point.x;
      m_ptStartDrag.x = point.y;

      m_bLeftButtonPressed = true;*/
      m_pSelectionListener->SetStickyPreparationBar(false);
      ::SendMessage( GetSafeHwnd(), WM_SYSCOMMAND, SC_MOVE | GUI_INMOVESIZE, 0);
   }
   CWnd::OnLButtonDown(nFlags, point);
}

//void CSgSelectionPreparationBar::OnLButtonUp(UINT nFlags, CPoint point)
//{
//   /*if(m_bLeftButtonPressed)
//   {
//      m_ptStartDrag.x = -1;
//      m_ptStartDrag.x = -1;
//   }
//   m_bLeftButtonPressed = false;*/
//   CWnd::OnLButtonUp(nFlags, point);
//}

void CSgSelectionPreparationBar::OnMove(int x, int y)
{
   CWnd::OnMove(x, y);

   if( ::IsWindow(m_hWnd) )
   {
      ::SetWindowPos( m_hWnd
         , NULL
         , x
         , y
         , 0
         , 0
         , SWP_NOZORDER | SWP_NOSIZE
         );
   }
}

Gdiplus::Image * CSgSelectionPreparationBar::LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance /*=NULL*/)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   Gdiplus::Image * pImage = NULL;

   if( lpszType == RT_BITMAP )
   {
      HBITMAP hBitmap = ::LoadBitmap( hInstance, MAKEINTRESOURCE(nID) );
      pImage = (Gdiplus::Image*)Gdiplus::Bitmap::FromHBITMAP(hBitmap, 0);
      ::DeleteObject(hBitmap);
      return pImage;
   }		

   hInstance = (hInstance == NULL) ? ::AfxGetResourceHandle() : hInstance;
   HRSRC hRsrc = ::FindResource ( hInstance, MAKEINTRESOURCE(nID), lpszType); 
   ASSERT(hRsrc != NULL);

   DWORD dwSize = ::SizeofResource( hInstance, hRsrc);
   LPBYTE lpRsrc = (LPBYTE)::LoadResource( hInstance, hRsrc);
   ASSERT(lpRsrc != NULL);

   HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
   LPBYTE pMem = (LPBYTE)::GlobalLock(hMem);
   memcpy( pMem, lpRsrc, dwSize);
   IStream * pStream = NULL;
   ::CreateStreamOnHGlobal( hMem, FALSE, &pStream);

   pImage = Gdiplus::Image::FromStream(pStream);

   ::GlobalUnlock(hMem);
   pStream->Release();
   ::FreeResource(lpRsrc);

   return pImage;
}

void CSgSelectionPreparationBar::SetButtonImage(CXTPButton* pButton, UINT nID, CSize sz)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CXTPImageManagerIconHandle iconHandle;
   CXTPImageManagerIcon* pIcon; 
   // load icon
   iconHandle = pIcon->LoadBitmapFromResource(::AfxGetResourceHandle(), MAKEINTRESOURCE(nID), NULL);
   pIcon = new CXTPImageManagerIcon(0, iconHandle.GetExtent().cx, iconHandle.GetExtent().cy);
   pIcon->SetNormalIcon(iconHandle);
   pButton->SetIcon(sz,iconHandle,iconHandle);
   CMDTARGET_RELEASE(pIcon);
}
CRect CSgSelectionPreparationBar::GetCustomSelRect()
{
   CRect rcCustom;
    CString csText;
    m_edtLeft.GetWindowText(csText);
    int x = _ttoi(csText);
    m_edtTop.GetWindowText(csText);
    int y = _ttoi(csText);
    m_edtWidth.GetWindowText(csText);
    int x2 = x + _ttoi(csText);
    m_edtHeight.GetWindowText(csText);
    int y2 = y + _ttoi(csText);

    rcCustom.SetRect(x, y , x2, y2);
    return rcCustom;
}

CString CSgSelectionPreparationBar::LoadMyString(UINT nID)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HINSTANCE hInstance =::AfxGetResourceHandle();

   TCHAR szBuffer[MAX_PATH];
   ::LoadString(hInstance, nID, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));

   CString csRetVal(szBuffer);

   return csRetVal;
}

BOOL CSgSelectionPreparationBar::PreTranslateMessage(MSG* pMsg)
{
	// Make sure that the keystrokes continue to the edit control.
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
      OnCancelSelection();
      return TRUE;
   }
   return CWnd::PreTranslateMessage(pMsg);
}