#include "stdafx.h"
#include "Resource.h"
#include "PresentationBar.h"
#include "UtileDefines.h"
#include "backend\la_project.h"
#include "AssistantDoc.h"
#include "..\..\Studio\Studio.h"

BEGIN_MESSAGE_MAP(CPresentationBar, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_COMMAND(IDC_EXIT_FULL_SCREEN, OnButtonExitFullScreen)
    ON_COMMAND(ID_PRESENTATIONBAR_PENS, OnButtonPens)
    ON_COMMAND(ID_PRESENTATIONBAR_MARKERS, OnButtonMarkers)
    ON_COMMAND(ID_PRESENTATIONBAR_TELEPOINTER, OnButtonTelepointer)
    ON_COMMAND(ID_PRESENTATIONBAR_MOUSENAV, OnButtonMouseNav)
    ON_COMMAND(ID_PRESENTATIONBAR_START, OnButtonStart)
    ON_COMMAND(ID_PRESENTATIONBAR_STOP, OnButtonStop) 
    ON_COMMAND(ID_PRESENTATIONBAR_PAUSE, OnButtonPause)
    ON_COMMAND(ID_PRESENTATIONBAR_SCREENGRABBING, OnButtonScreenGrabbing)
    ON_COMMAND(ID_PRESENTATIONBAR_PREVIOUSPAGE, OnButtonPreviousPage)
    ON_COMMAND(ID_PRESENTATIONBAR_NEXTPAGE, OnButtonNextPage)

    ON_COMMAND(ID_PRESENTATIONBAR_PAGEFOCUSED, OnButtonPageFocused)
    ON_COMMAND(ID_PRESENTATIONBAR_DOCUMENTSTRUCT, OnButtonDocumentStructure)
    ON_COMMAND(ID_PRESENTATIONBAR_RECORDINGS, OnButtonRecordings)
    ON_COMMAND(ID_PRESENTATIONBAR_CUT, OnButtonCut)
    ON_COMMAND(ID_PRESENTATIONBAR_COPY, OnButtonCopy)
    ON_COMMAND(ID_PRESENTATIONBAR_PASTE, OnButtonPaste)
    ON_COMMAND(ID_PRESENTATIONBAR_UNDO, OnButtonUndo)
    ON_COMMAND(ID_PRESENTATIONBAR_ONECLICKCOPY, OnButtonClickCopy)
    ON_COMMAND(ID_PRESENTATIONBAR_TEXT, OnButtonText)
    ON_COMMAND(ID_PRESENTATIONBAR_LINE, OnButtonLine)
    ON_COMMAND(ID_PRESENTATIONBAR_POLYLINE, OnButtonPolyline)
    ON_COMMAND(ID_PRESENTATIONBAR_FREEHAND, OnButtonFreehand)
    ON_COMMAND(ID_PRESENTATIONBAR_ELLIPSE, OnButtonEllipse)
    ON_COMMAND(ID_PRESENTATIONBAR_RECTANGLE, OnButtonRectangle)
    ON_COMMAND(ID_PRESENTATIONBAR_POLYGON, OnButtonPolygon)
    ON_COMMAND(ID_PRESENTATIONBAR_INSERTPAGE, OnButtonInsertPage)

    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_PENS, OnUpdateTools)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_MARKERS, OnUpdateTools)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_TELEPOINTER, OnUpdateTools)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_MOUSENAV, OnUpdateTools)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_ONECLICKCOPY, OnUpdateTools)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_TEXT, OnUpdateTools)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_LINE, OnUpdateTools)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_POLYLINE, OnUpdateTools)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_FREEHAND, OnUpdateTools)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_ELLIPSE, OnUpdateTools)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_RECTANGLE, OnUpdateTools)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_POLYGON, OnUpdateTools)

    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_START, OnUpdateStart)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_STOP, OnUpdateStop)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_PAUSE, OnUpdatePause)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_SCREENGRABBING, OnUpdateButtonScreenGrabbing)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_CUT, OnUpdateCutCopy)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_COPY, OnUpdateCutCopy)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_PASTE, OnUpdatePaste)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_UNDO, OnUpdateUndo)

    ON_UPDATE_COMMAND_UI(IDC_EXIT_FULL_SCREEN, OnUpdateNonSgControls)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_PAGEFOCUSED, OnUpdateNonSgControls)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_INSERTPAGE, OnUpdateNonSgControls)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_DOCUMENTSTRUCT, OnUpdateNonSgControls)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_RECORDINGS, OnUpdateNonSgControls)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_PREVIOUSPAGE, OnUpdateNonSgControls)
    ON_UPDATE_COMMAND_UI(ID_PRESENTATIONBAR_NEXTPAGE, OnUpdateNonSgControls)

    ON_CBN_DROPDOWN(ID_PRESENTATIONBAR_MARKERS, OnMarkersSplitButton)
    ON_CBN_DROPDOWN(ID_PRESENTATIONBAR_PENS, OnPensSplitButton)

    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)

    ON_WM_ERASEBKGND()
    ON_WM_SHOWWINDOW()
    ON_WM_DESTROY()
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// static functions
UINT CPresentationBar::StartShowHideAnimation(LPVOID pParam) {

    CPresentationBar *pPresentationBar = (CPresentationBar *)pParam;

    bool bShow = !pPresentationBar->m_bWindowVisible;

    int iTopMiddle = 2;
    int iBottomMiddle = -2;
    int iLeftMiddle = 2;
    int iRightMiddle = -2;
    if (!bShow) {
        iTopMiddle = -2;
        iBottomMiddle = 2;
        iLeftMiddle = -2;
        iRightMiddle = 2;
    }

    if (bShow)
       pPresentationBar->m_bWindowVisible = true;

    CRect rcWnd;
    pPresentationBar->GetWindowRect(rcWnd);
    CRect rcStandard = pPresentationBar->GetStandardWindowRect();

    CRect rcNewPos(rcWnd);
    pPresentationBar->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

    bool bFinished = false;
    while (!bFinished) {
        if (pPresentationBar->GetPosition() == OT_TOPMIDDLE) {
            rcNewPos.OffsetRect(0, iTopMiddle);
            if ((iTopMiddle == 2 && rcNewPos.top >= rcStandard.top) ||
                (iTopMiddle == -2 && rcNewPos.bottom <= rcStandard.top))
                bFinished = true;
        }
        else if (pPresentationBar->GetPosition() == OT_BOTTOMMIDDLE) {
            rcNewPos.OffsetRect(0, iBottomMiddle);
            if ((iBottomMiddle == -2 && rcNewPos.bottom <= rcStandard.bottom) ||
                (iBottomMiddle == 2 && rcNewPos.top >= rcStandard.bottom))
                bFinished = true;
        }
        else if (pPresentationBar->GetPosition() == OT_MIDDLELEFT) {
            rcNewPos.OffsetRect(iLeftMiddle, 0);
            if ((iLeftMiddle == 2 && rcNewPos.left >= rcStandard.left) ||
                (iLeftMiddle == -2 && rcNewPos.right <= rcStandard.left))
                bFinished = true;
        }
        else if(pPresentationBar->GetPosition() == OT_MIDDLERIGHT) {
            rcNewPos.OffsetRect(iRightMiddle, 0);
            if ((iRightMiddle == -2 && rcNewPos.right <= rcStandard.right) ||
                (iRightMiddle == 2 && rcNewPos.left >= rcStandard.right))
                bFinished = true;
        }
        if(pPresentationBar && pPresentationBar->IsWindowVisible()) {
            pPresentationBar->SetWindowPos(&wndTopMost, rcNewPos.left, rcNewPos.top, rcNewPos.Width(), rcNewPos.Height(), SWP_NOZORDER); 
            Sleep(20);
        } else {
            bFinished = true;
        }
    }

    if (!bShow)
       pPresentationBar->m_bWindowVisible = false;

    pPresentationBar->SetAnimationFinished();
    return 0;
}

IMPLEMENT_DYNAMIC(CPresentationBar, CWnd)

CPresentationBar::CPresentationBar(CAssistantDoc *pAssistantDoc)
{
   m_pCommandBars = NULL;
   m_bIsPauseFlashing = false;

   m_nPenIndex = 1;
   m_nMarkerIndex = 4;

   m_rcStandard = CRect(0,0,0,0);
   m_ctrlToolTipsContext = NULL;

   m_pAnimateWindowThread = NULL;
   m_bAnimationFinished = true;

   m_pAssistantDoc = pAssistantDoc;

   m_bIsPreparationMode = false;
   m_pMainFrmWnd = NULL;

   m_bWindowVisible = true;
   
   m_pListener = NULL;
   m_hNoCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_NO_CURSOR);
}
#ifndef SAFE_DELETE_WINDOW
#define SAFE_DELETE_WINDOW(ptr) if(ptr){ptr->DestroyWindow(); delete ptr; ptr=NULL;}
#endif
CPresentationBar::~CPresentationBar()
{
    while (!m_bAnimationFinished)
        Sleep(10);

   m_pCommandBars = NULL;
   SaveSettings(); 
   CMDTARGET_RELEASE(m_ctrlToolTipsContext);
}

int CPresentationBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   while (!m_bAnimationFinished)
      Sleep(10);
   
   if (CWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   InitSettings();
   
   SetWindowPos(&wndTopMost,100,100,965,37,SWP_NOZORDER);
  
   CreateButtons();

   CreateToolTips();

   return 0;
}

void CPresentationBar::InitSettings()
{
   m_nPosition = OT_BOTTOMMIDDLE;
   m_bIsAutoHide = false;
   m_bMouseOver = false;
   m_bSmallIcons = false;
   m_bShowTransparent = true;

   m_bShowExitFullScreen = true;
   m_bShowPencils = true;
   m_bShowMarkers = true;
   m_bShowPointer = true;
   m_bShowMouseNav = true;
   m_bShowStart = true;
   m_bShowStop = true;
   m_bShowPause = true;
   m_bShowScreenGrabbing = true;
   m_bShowPageFocused = false;

   m_bShowDocumentStruct = false;
   m_bShowRecordings = false;
   m_bShowCut = false;
   m_bShowCopy = false;
   m_bShowPaste = false;
   m_bShowUndo = false;
   m_bShowOneClickCopy = false;
   m_bShowText = false;
   m_bShowLine = false;
   m_bShowPolyline = false;
   m_bShowFreeHand = false;
   m_bShowEllipse = false;
   m_bShowRectangle = false;
   m_bShowPolygon = false;
   m_bShowInsertPage = false;

   m_bShowPreviousPage = true;
   m_bShowNextPage = true;

   m_bShowSeparator1 = true;
   m_bShowSeparator2 = true;
   m_bShowSeparator3 = true;
   m_bShowSeparator4 = true;

   CMDTARGET_RELEASE(m_ctrlToolTipsContext);
}

void CPresentationBar::CreateButtons()
{
   int nLeft = 4;

   // Start
   m_pBtnStart.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_START);
   m_pBtnStart.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);
   
   //Pause
   m_pBtnPause.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(1,1,50,96),this,ID_PRESENTATIONBAR_PAUSE);
   nLeft += 37;
   m_pBtnPause.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);
   
   //Stop
   m_pBtnStop.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(1,1,50,96),this,ID_PRESENTATIONBAR_STOP);
   nLeft += 37;
   m_pBtnStop.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);
   
   // Separator 1
   m_pBtnSeparator1.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,IDC_PRESENTATIONBAR_SEPARATOR1);
   nLeft += 37 + 3;
   m_pBtnSeparator1.SetWindowPos(NULL,nLeft,0,1,37, SWP_NOZORDER);

   //Screengrabbing
   m_pBtnSg.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_SCREENGRABBING);
   nLeft += 3;
   m_pBtnSg.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);
   
   // Separator 2
   m_pBtnSeparator2.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,IDC_PRESENTATIONBAR_SEPARATOR2);
   nLeft += 37 + 1;
   m_pBtnSeparator2.SetWindowPos(NULL,nLeft,0,1,37, SWP_NOZORDER);

   //Button Previous
   m_pBtnPrevious.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_PREVIOUSPAGE);
   nLeft += 3;
   m_pBtnPrevious.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Button Next
   m_pBtnNext.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_NEXTPAGE);
   nLeft += 37;
   m_pBtnNext.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   // Separator 4
   m_pBtnSeparator4.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,IDC_PRESENTATIONBAR_SEPARATOR4);
   nLeft += 37 + 1;
   m_pBtnSeparator4.SetWindowPos(NULL,nLeft,0,1,37, SWP_NOZORDER);

   // Pens
   m_pBtnPens.Create(NULL, WS_CHILD|WS_VISIBLE , CRect(0,0,0,0),this,ID_PRESENTATIONBAR_PENS);
   nLeft += 3;
   m_pBtnPens.SetWindowPos(NULL,nLeft,1,44,36, SWP_NOZORDER);
   m_pBtnPens.SetPushButtonStyle(xtpButtonSplitDropDown);
   
   //Markers
   m_pBtnMarkers.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_MARKERS);
   nLeft += 37 + 8;
   m_pBtnMarkers.SetWindowPos(NULL,nLeft,1,44,36, SWP_NOZORDER);
   m_pBtnMarkers.SetPushButtonStyle(xtpButtonSplitDropDown);

   //Telepointer 
   m_pBtnTelepointer.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,65,33),this,ID_PRESENTATIONBAR_TELEPOINTER);
   nLeft += 37 + 8;
   m_pBtnTelepointer.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);
   
   // Mouse Nav
   m_pBtnMouseNav.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(1,1,50,96),this,ID_PRESENTATIONBAR_MOUSENAV);
   nLeft += 37;
   m_pBtnMouseNav.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Page focused
   m_pBtnPageFocused.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_PAGEFOCUSED);
   nLeft += 37;
   m_pBtnPageFocused.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Document structure
   m_pBtnDocumentStruct.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_DOCUMENTSTRUCT);
   nLeft += 37;
   m_pBtnDocumentStruct.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Recordings
   m_pBtnRecordings.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_RECORDINGS);
   nLeft += 37;
   m_pBtnRecordings.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Cut
   m_pBtnCut.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_CUT);
   nLeft += 37;
   m_pBtnCut.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Copy
   m_pBtnCopy.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_COPY);
   nLeft += 37;
   m_pBtnCopy.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Paste
   m_pBtnPaste.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_PASTE);
   nLeft += 37;
   m_pBtnPaste.SetWindowPos(NULL,nLeft/*511*/,1,36,36, SWP_NOZORDER);

   //Undo
   m_pBtnUndo.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_UNDO);
   nLeft += 37;
   m_pBtnUndo.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //One click copy
   m_pBtnOneClickCopy.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_ONECLICKCOPY);
   nLeft += 37;
   m_pBtnOneClickCopy.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Text
   m_pBtnText.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_TEXT);
   nLeft += 37;
   m_pBtnText.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Line
   m_pBtnLine.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_LINE);
   nLeft += 37;
   m_pBtnLine.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Polyline//pb
   m_pBtnPolyLine.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_POLYLINE);
   nLeft += 37;
   m_pBtnPolyLine.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Freehand
   m_pBtnFreeHand.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_FREEHAND);
   nLeft += 37;
   m_pBtnFreeHand.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Ellipse
   m_pBtnEllipse.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_ELLIPSE);
   nLeft += 37;
   m_pBtnEllipse.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Rectangle
   m_pBtnRectangle.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_RECTANGLE);
   nLeft += 37;
   m_pBtnRectangle.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //Polygon
   m_pBtnPolygon.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_POLYGON);
   nLeft += 37;
   m_pBtnPolygon.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   //InsertPage
   m_pBtnInsertPage.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,ID_PRESENTATIONBAR_INSERTPAGE);
   nLeft += 37;
   m_pBtnInsertPage.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);
   
   // Separator 3
   m_pBtnSeparator3.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),this,IDC_PRESENTATIONBAR_SEPARATOR3);
   nLeft += 37 + 1;
   m_pBtnSeparator3.SetWindowPos(NULL,nLeft,0,1,37, SWP_NOZORDER);

   // Exit Full Screen
   m_pBtnExitFullScreen.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0,0,50,50), this, IDC_EXIT_FULL_SCREEN);
   nLeft += 4;
   m_pBtnExitFullScreen.SetWindowPos(NULL,nLeft,1,36,36, SWP_NOZORDER);

   nLeft += 37 + 4;
   SetWindowPos(&wndTopMost,100,100,nLeft,37,SWP_NOZORDER);

   m_pBtnSeparator1.SetFlatStyle(TRUE);
   m_pBtnSeparator1.SetTransparent(TRUE);
   m_pBtnSeparator1.SetShowFocus(FALSE);

   m_pBtnSeparator2.SetFlatStyle(TRUE);
   m_pBtnSeparator2.SetTransparent(TRUE);
   m_pBtnSeparator2.SetShowFocus(FALSE);

   m_pBtnSeparator3.SetFlatStyle(TRUE);
   m_pBtnSeparator3.SetTransparent(TRUE);
   m_pBtnSeparator3.SetShowFocus(FALSE);

   m_pBtnSeparator4.SetFlatStyle(TRUE);
   m_pBtnSeparator4.SetTransparent(TRUE);
   m_pBtnSeparator4.SetShowFocus(FALSE);

   ShowHidePresentationTools(ID_PRESENTATIONBAR_PAGEFOCUSED, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_DOCUMENTSTRUCT, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_RECORDINGS, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_CUT, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_COPY, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_PASTE, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_UNDO, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_ONECLICKCOPY, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_TEXT, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_LINE, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_POLYLINE, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_FREEHAND, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_ELLIPSE, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_RECTANGLE, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_POLYGON, false);
   ShowHidePresentationTools(ID_PRESENTATIONBAR_INSERTPAGE, false);
}

void CPresentationBar::OnPaint()
{
	if(m_bAnimationFinished)
	{
		CPaintDC dc(this); // device context for painting

		dc.SetBkMode(TRANSPARENT);
		CRect rcWindow;
		GetClientRect(&rcWindow);

		Gdiplus::Graphics graphics(dc);

		// Fill with transparent color
		Gdiplus::SolidBrush transpBrush(Gdiplus::Color(255, 255, 255, 255));
		graphics.FillRectangle(&transpBrush, 0, 0, rcWindow.Width(), rcWindow.Height());

		// Draw background
		DrawBackground();

		// Draw separators
		DrawSeparators(graphics);

		UpdateControls(); 
	}
	CWnd::OnPaint();
}

BOOL CPresentationBar::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CPresentationBar::UpdateControls()
{
   m_pBtnExitFullScreen.UpdateBackground();  
   m_pBtnPens.UpdateBackground(); 
   m_pBtnMarkers.UpdateBackground(); 
   m_pBtnTelepointer.UpdateBackground(); 
   m_pBtnMouseNav.UpdateBackground(); 
   m_pBtnStart.UpdateBackground(); 
   m_pBtnStop.UpdateBackground(); 
   m_pBtnPause.UpdateBackground(); 
   m_pBtnSg.UpdateBackground(); 
   m_pBtnPrevious.UpdateBackground();
   m_pBtnNext.UpdateBackground();
   m_pBtnSeparator1.UpdateBackground();
   m_pBtnSeparator2.UpdateBackground();
   m_pBtnSeparator3.UpdateBackground();
   m_pBtnSeparator4.UpdateBackground();

   m_pBtnPageFocused.UpdateBackground();
   m_pBtnDocumentStruct.UpdateBackground();
   m_pBtnRecordings.UpdateBackground();
   m_pBtnCut.UpdateBackground();
   m_pBtnCopy.UpdateBackground();
   m_pBtnPaste.UpdateBackground();
   m_pBtnUndo.UpdateBackground();
   m_pBtnOneClickCopy.UpdateBackground();
   m_pBtnText.UpdateBackground();
   m_pBtnLine.UpdateBackground();
   m_pBtnPolyLine.UpdateBackground();
   m_pBtnFreeHand.UpdateBackground();
   m_pBtnEllipse.UpdateBackground();
   m_pBtnRectangle.UpdateBackground();
   m_pBtnPolygon.UpdateBackground();
   m_pBtnInsertPage.UpdateBackground();
}

void CPresentationBar::DrawBackground()
{
   CRect rcWindow;
   GetClientRect(&rcWindow);

   CDC *dc = GetDC();
   
   CBrush brFillColor;
   CPen penBorder;

   brFillColor.CreateSolidBrush(RGB(235,240,242));
   penBorder.CreatePen(PS_SOLID, 1, RGB(235,240,242));

   dc->SelectObject(&brFillColor);
   dc->SelectObject(&penBorder);

   int nCornerRadius = 12;
   int nOffset = 20;
   if(m_bSmallIcons)
   {
      nCornerRadius = 6;
      nOffset /= 2;
   }

   dc->RoundRect(0, 0, rcWindow.Width(), rcWindow.Height(), nCornerRadius, nCornerRadius);
   
   switch(m_nPosition)
   {
   case OT_BOTTOMMIDDLE:
      dc->RoundRect(0, nOffset, rcWindow.Width(), rcWindow.Height(), 0, 0);  
      break;
   case OT_TOPMIDDLE:
      dc->RoundRect(0, 0, rcWindow.Width(), nOffset, 0, 0);  
      break;
   case OT_MIDDLERIGHT:  
      dc->RoundRect(nOffset, 0, rcWindow.Width(), rcWindow.Height(), 0, 0);  
      break;
   case OT_MIDDLELEFT:
      dc->RoundRect(0, 0, nOffset, rcWindow.Height(), 0, 0);  
      break;
   }
   ReleaseDC(dc);
}

void CPresentationBar::DrawSeparators(Gdiplus::Graphics &graphics)
{
   if(m_pBtnSeparator1.IsWindowVisible())
   { 
      CRect rcCtrl;
      ::GetWindowRect(m_pBtnSeparator1.GetSafeHwnd(), rcCtrl);
      ScreenToClient(rcCtrl);
      Gdiplus::RectF rcFBtn(rcCtrl.left, rcCtrl.top, rcCtrl.Width(), rcCtrl.Height());
      
      Gdiplus::Pen grayPen(Gdiplus::Color(255, 150, 161, 167), 1);
      if(m_nPosition == OT_TOPMIDDLE || m_nPosition == OT_BOTTOMMIDDLE)
         graphics.DrawLine(&grayPen, rcFBtn.X, rcFBtn.Y, rcFBtn.X, rcFBtn.Height);
      else
         graphics.DrawLine(&grayPen, rcFBtn.X, rcFBtn.Y, rcFBtn.Width, rcFBtn.Y );
   }
   if(m_pBtnSeparator2.IsWindowVisible())
   {
      CRect rcCtrl;
      ::GetWindowRect(m_pBtnSeparator2.GetSafeHwnd(), rcCtrl);
      ScreenToClient(rcCtrl);
      Gdiplus::RectF rcFBtn(rcCtrl.left, rcCtrl.top, rcCtrl.Width(), rcCtrl.Height());

      Gdiplus::Pen grayPen(Gdiplus::Color(255, 178, 191, 198), 1);
      if(m_nPosition == OT_TOPMIDDLE || m_nPosition == OT_BOTTOMMIDDLE)
         graphics.DrawLine(&grayPen, rcFBtn.X, rcFBtn.Y, rcFBtn.X, rcFBtn.Height);
      else
         graphics.DrawLine(&grayPen, rcFBtn.X, rcFBtn.Y, rcFBtn.Width, rcFBtn.Y );
   }
   if(m_pBtnSeparator4.IsWindowVisible())
   {
      CRect rcCtrl;
      ::GetWindowRect(m_pBtnSeparator4.GetSafeHwnd(), rcCtrl);
      ScreenToClient(rcCtrl);
      Gdiplus::RectF rcFBtn(rcCtrl.left, rcCtrl.top, rcCtrl.Width(), rcCtrl.Height());

      Gdiplus::Pen grayPen(Gdiplus::Color(255, 178, 191, 198), 1);
      if(m_nPosition == OT_TOPMIDDLE || m_nPosition == OT_BOTTOMMIDDLE)
         graphics.DrawLine(&grayPen, rcFBtn.X, rcFBtn.Y, rcFBtn.X, rcFBtn.Height);
      else
         graphics.DrawLine(&grayPen, rcFBtn.X, rcFBtn.Y, rcFBtn.Width, rcFBtn.Y );
   }
   if(m_pBtnSeparator3.IsWindowVisible())
   {
      CRect rcCtrl;
      ::GetWindowRect(m_pBtnSeparator3.GetSafeHwnd(), rcCtrl);
      ScreenToClient(rcCtrl);
      Gdiplus::RectF rcFBtn(rcCtrl.left, rcCtrl.top, rcCtrl.Width(), rcCtrl.Height());

      Gdiplus::Pen grayPen(Gdiplus::Color(255, 150, 161, 167), 1);
      if(m_nPosition == OT_TOPMIDDLE || m_nPosition == OT_BOTTOMMIDDLE)
         graphics.DrawLine(&grayPen, rcFBtn.X, rcFBtn.Y, rcFBtn.X, rcFBtn.Height);
      else
         graphics.DrawLine(&grayPen, rcFBtn.X, rcFBtn.Y, rcFBtn.Width, rcFBtn.Y );
   }
}

void CPresentationBar::SetCommandBars(CXTPCommandBars* pCommandBars)
{
   m_pCommandBars = pCommandBars;

   LoadIcons();

   LoadSettings();
}

void CPresentationBar::LoadIcons()
{
   CXTPImageManagerIcon *pIcon = NULL;

   CSize sizeImage;

   if(m_bSmallIcons)
      sizeImage.SetSize(16, 16);
   else
      sizeImage.SetSize(32, 32);
  
   bool bIsScreenGrabbing = ASSISTANT::Project::active->isScreenGrabbingActive_ && !ASSISTANT::Project::active->IsSgOnlyDocument();
   bool bIsRecording = ASSISTANT::Project::active->IsRecording();

   UINT uPresentationBar[] = {IDC_EXIT_FULL_SCREEN, 
      m_nMarkerIndex == 0 ? ID_PRESENTATIONBAR_MARKERS : -1, 
      m_nMarkerIndex == 1 ? ID_PRESENTATIONBAR_MARKERS : -1,
      m_nMarkerIndex == 2 ? ID_PRESENTATIONBAR_MARKERS : -1,
      m_nMarkerIndex == 3 ? ID_PRESENTATIONBAR_MARKERS : -1,
      m_nMarkerIndex == 4 ? ID_PRESENTATIONBAR_MARKERS : -1,
      m_nMarkerIndex == 5 ? ID_PRESENTATIONBAR_MARKERS : -1,
      m_nPenIndex == 0 ? ID_PRESENTATIONBAR_PENS : -1, 
      m_nPenIndex == 1 ? ID_PRESENTATIONBAR_PENS : -1, 
      m_nPenIndex == 2 ? ID_PRESENTATIONBAR_PENS : -1, 
      m_nPenIndex == 3 ? ID_PRESENTATIONBAR_PENS : -1, 
      m_nPenIndex == 4 ? ID_PRESENTATIONBAR_PENS : -1, 
      m_nPenIndex == 5 ? ID_PRESENTATIONBAR_PENS : -1, 
      m_nPenIndex == 6 ? ID_PRESENTATIONBAR_PENS : -1, 
      m_nPenIndex == 7 ? ID_PRESENTATIONBAR_PENS : -1,
      ID_PRESENTATIONBAR_TELEPOINTER,ID_PRESENTATIONBAR_MOUSENAV,
      ID_PRESENTATIONBAR_PAGEFOCUSED,ID_PRESENTATIONBAR_DOCUMENTSTRUCT,ID_PRESENTATIONBAR_RECORDINGS,
      ID_PRESENTATIONBAR_CUT,ID_PRESENTATIONBAR_COPY,ID_PRESENTATIONBAR_PASTE,ID_PRESENTATIONBAR_UNDO,ID_PRESENTATIONBAR_ONECLICKCOPY,
      ID_PRESENTATIONBAR_TEXT,ID_PRESENTATIONBAR_LINE,ID_PRESENTATIONBAR_POLYLINE,ID_PRESENTATIONBAR_FREEHAND,ID_PRESENTATIONBAR_ELLIPSE,
      ID_PRESENTATIONBAR_RECTANGLE,ID_PRESENTATIONBAR_POLYGON,ID_PRESENTATIONBAR_INSERTPAGE,
      bIsRecording ? -1 : ID_PRESENTATIONBAR_START,
      bIsRecording ? ID_PRESENTATIONBAR_START : -1,
      ID_PRESENTATIONBAR_STOP,ID_PRESENTATIONBAR_PAUSE,
      bIsScreenGrabbing == false ? ID_PRESENTATIONBAR_SCREENGRABBING : -1, 
      bIsScreenGrabbing == true ? ID_PRESENTATIONBAR_SCREENGRABBING : -1,
      ID_PRESENTATIONBAR_PREVIOUSPAGE, ID_PRESENTATIONBAR_NEXTPAGE};

   m_pCommandBars->GetImageManager()->SetIcons(IDB_PRESENTATIONBAR_SMALLICONS_HOT, uPresentationBar, _countof(uPresentationBar), CSize(16, 16));
   m_pCommandBars->GetImageManager()->SetIcons(IDB_PRESENTATIONBAR_ICONS_HOT, uPresentationBar, _countof(uPresentationBar), CSize(32, 32));

   UINT uiStartRecordingDisabled[] = {-1, -1, -1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,ASSISTANT::Project::active->GetActiveDocument() ? ID_PRESENTATIONBAR_START : -1,-1, -1, -1, -1};

   m_pCommandBars->GetImageManager()->SetIcons(IDB_PRESENTATIONBAR_SMALLICONS_DISABLED, uiStartRecordingDisabled, _countof(uiStartRecordingDisabled), CSize(16, 16),xtpImageDisabled);
   m_pCommandBars->GetImageManager()->SetIcons(IDB_PRESENTATIONBAR_ICONS_DISABLED, uiStartRecordingDisabled, _countof(uiStartRecordingDisabled), CSize(32, 32),xtpImageDisabled);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(IDC_EXIT_FULL_SCREEN, sizeImage.cx);
   m_pBtnExitFullScreen.SetIcon(sizeImage, pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_PREVIOUSPAGE, sizeImage.cx);
   m_pBtnPrevious.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_NEXTPAGE, sizeImage.cx);
   m_pBtnNext.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_PENS, sizeImage.cx);
   m_pBtnPens.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_MARKERS, sizeImage.cx);
   m_pBtnMarkers.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_TELEPOINTER, sizeImage.cx);
   m_pBtnTelepointer.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_MOUSENAV, sizeImage.cx);
   m_pBtnMouseNav.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_START, sizeImage.cx);
   m_pBtnStart.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_STOP, sizeImage.cx);
   m_pBtnStop.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_PAUSE, sizeImage.cx);
   m_pBtnPause.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_SCREENGRABBING, sizeImage.cx);
   m_pBtnSg.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_PAGEFOCUSED, sizeImage.cx);
   m_pBtnPageFocused.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_DOCUMENTSTRUCT, sizeImage.cx);
   m_pBtnDocumentStruct.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_RECORDINGS, sizeImage.cx);
   m_pBtnRecordings.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_CUT, sizeImage.cx);
   m_pBtnCut.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_COPY, sizeImage.cx);
   m_pBtnCopy.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_PASTE, sizeImage.cx);
   m_pBtnPaste.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_UNDO, sizeImage.cx);
   m_pBtnUndo.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_ONECLICKCOPY, sizeImage.cx);
   m_pBtnOneClickCopy.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_TEXT, sizeImage.cx);
   m_pBtnText.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_LINE, sizeImage.cx);
   m_pBtnLine.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_POLYLINE, sizeImage.cx);
   m_pBtnPolyLine.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_FREEHAND, sizeImage.cx);
   m_pBtnFreeHand.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_ELLIPSE, sizeImage.cx);
   m_pBtnEllipse.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_RECTANGLE, sizeImage.cx);
   m_pBtnRectangle.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_POLYGON, sizeImage.cx);
   m_pBtnPolygon.SetIcon(sizeImage,pIcon);

   pIcon = m_pCommandBars->GetImageManager()->GetImage(ID_PRESENTATIONBAR_INSERTPAGE, sizeImage.cx);
   m_pBtnInsertPage.SetIcon(sizeImage,pIcon);
}

void CPresentationBar::SetPosition(UINT nPos)
{ 
   RefreshWindowPos(nPos, m_bSmallIcons);
   m_nPosition = nPos;

   m_pBtnMarkers.SetPosition(nPos);
   m_pBtnPens.SetPosition(nPos);

   RedrawWindow();
}

UINT CPresentationBar::GetPosition()
{ 
   return m_nPosition;
}

void CPresentationBar::RefreshWindowPos(UINT nPos, bool bSmallIcons)
{
   CRect rcMainWnd;
   ::GetWindowRect(::GetDesktopWindow(), rcMainWnd);
   
   int nHeight, nWidth, nLeft, nTop;

   bool bIsHorizontal = (m_nPosition == OT_BOTTOMMIDDLE || m_nPosition == OT_TOPMIDDLE);
   bool bIsHorizontalNewPos = (nPos == OT_BOTTOMMIDDLE || nPos == OT_TOPMIDDLE);
   bool bSmallFromLargeIcons = (bSmallIcons && !m_bSmallIcons);
   bool bLargeFromSmallIcons = (!bSmallIcons && m_bSmallIcons);

   CRect rcWnd;
   GetWindowRect(rcWnd);
   ScreenToClient(rcWnd);

   if(bSmallFromLargeIcons)
   {
      nWidth = rcWnd.Width()/2;
      nHeight = rcWnd.Height()/2;
   }
   else
   {
      if(bLargeFromSmallIcons)
      {
         nWidth = rcWnd.Width()*2;
         nHeight = rcWnd.Height()*2;
      }
   }
   if((bIsHorizontal && !bIsHorizontalNewPos) || (!bIsHorizontal && bIsHorizontalNewPos))
   {
      nWidth = rcWnd.Height();
      nHeight = rcWnd.Width();
   }
   else
   {
      if(!bSmallFromLargeIcons && !bLargeFromSmallIcons)
      {
         nWidth = rcWnd.Width();
         nHeight = rcWnd.Height();
      }
   }

   nLeft = (rcMainWnd.Width()-nWidth)/2;
   nTop = rcMainWnd.Height()-nHeight;

   switch(nPos)
   {
   case OT_BOTTOMMIDDLE:
      SetWindowPos(&wndTopMost, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER);
      m_rcStandard.SetRect(nLeft, nTop, nLeft+nWidth, nTop+nHeight);
      break;
   case OT_TOPMIDDLE:
      SetWindowPos(&wndTopMost, nLeft, 0, nWidth, nHeight, SWP_NOZORDER);
      m_rcStandard.SetRect(nLeft, 0, nLeft+nWidth, 0+nHeight);
      break;
   case OT_MIDDLELEFT:
      SetWindowPos(&wndTopMost, 0, nTop/2, nWidth, nHeight, SWP_NOZORDER);
      m_rcStandard.SetRect(0, nTop/2, 0+nWidth, nTop/2+nHeight);
      break;
   case OT_MIDDLERIGHT:
      SetWindowPos(&wndTopMost, nLeft*2+1, nTop/2, nWidth, nHeight, SWP_NOZORDER);
      m_rcStandard.SetRect(nLeft*2+1, nTop/2, nLeft*2+1+nWidth, nTop/2+nHeight);
      break;
   }
   RefreshButtonsPos(nPos, bSmallIcons);
}

void CPresentationBar::RefreshButtonsPos(UINT nPos, bool bSmallIcons)
{ 
   bool bIsHorizontal = (m_nPosition == OT_BOTTOMMIDDLE || m_nPosition == OT_TOPMIDDLE);
   bool bIsHorizontalNewPos = (nPos == OT_BOTTOMMIDDLE || nPos == OT_TOPMIDDLE);
   bool bSmallFromLargeIcons = (bSmallIcons && !m_bSmallIcons);
   bool bLargeFromSmallIcons = (!bSmallIcons && m_bSmallIcons);

   if(((bIsHorizontal && bIsHorizontalNewPos) || (!bIsHorizontal && !bIsHorizontalNewPos))
      && (!bSmallFromLargeIcons && !bLargeFromSmallIcons))
      return;
   else
   {
      HWND hwndChild = ::GetWindow(GetSafeHwnd(), GW_CHILD);  

      while(hwndChild)   
      {
         CWnd *pwndCtrl = CWnd::FromHandle(hwndChild);
         CRect rcCtrl, rcCtrlNew;
         pwndCtrl->GetWindowRect(rcCtrl);
         ScreenToClient(rcCtrl);
         if(bSmallFromLargeIcons)
         {
            rcCtrlNew.left = rcCtrl.left/2;
            rcCtrlNew.top = rcCtrl.top/2;
            rcCtrlNew.right = rcCtrl.right/2;
            rcCtrlNew.bottom = rcCtrl.bottom/2;
         }
         else
         {
            if(bLargeFromSmallIcons)
            {
               rcCtrlNew.left = rcCtrl.left*2;
               rcCtrlNew.top = rcCtrl.top*2;
               rcCtrlNew.right = rcCtrl.right*2;
               rcCtrlNew.bottom = rcCtrl.bottom*2;
            }
         }
         if((bIsHorizontal && !bIsHorizontalNewPos) || (!bIsHorizontal && bIsHorizontalNewPos))
         {
            rcCtrlNew.left = rcCtrl.top;
            rcCtrlNew.top = rcCtrl.left;
            rcCtrlNew.right = rcCtrl.bottom;
            rcCtrlNew.bottom = rcCtrl.right;
         }
         pwndCtrl->SetWindowPos(NULL,rcCtrlNew.left,rcCtrlNew.top,rcCtrlNew.Width(),rcCtrlNew.Height(), SWP_NOZORDER);
         hwndChild = ::GetWindow( hwndChild, GW_HWNDNEXT);
      }	
   }
}

void CPresentationBar::OnButtonExitFullScreen()
{
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_ACC_VK_ESC);
    else if (m_pListener != NULL)
        m_pListener->KeyEscPressed();
}

void CPresentationBar::OnButtonPens()
{
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_BUTTON_PENS);
    else if (m_pListener != NULL)
        m_pListener->ButtonPensPressed();
}

void CPresentationBar::OnButtonMarkers()
{
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_BUTTON_MARKERS);
    else if (m_pListener != NULL)
        m_pListener->ButtonMarkersPressed();
}

void CPresentationBar::OnButtonTelepointer()
{
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_TOOL_TELEPOINTER);
    else if (m_pListener != NULL)
        m_pListener->ButtonTelepointerPressed();
}
void CPresentationBar::OnButtonMouseNav()
{
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_TOOL_SIMPLENAVIGATION);
    else if (m_pListener != NULL)
        m_pListener->ButtonSimpleNavigationPressed();
}

void CPresentationBar::OnButtonStart()
{
    if (m_pMainFrmWnd != NULL) {
        if(m_bIsPreparationMode)
            m_pMainFrmWnd->SendMessage(WM_FINISH_SGSELECTION);
        else
            m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_START_RECORDING);
    }
    else if (m_pListener != NULL)
        m_pListener->ButtonStartPressed();
}

void CPresentationBar::OnButtonStop()
{
    if (m_pMainFrmWnd != NULL) {
        if(ASSISTANT::Project::active->IsSgOnlyDocument())
            m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_TOOL_SCREENGRABBING);
        else
            m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_STOP_RECORDING);
    }
    else if (m_pListener != NULL)
        m_pListener->ButtonStopPressed();
   
   if(m_bIsPauseFlashing)
      m_bIsPauseFlashing = false;
}

void CPresentationBar::OnButtonPause() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_PAUSE_RECORDING);
    else if (m_pListener != NULL)
        m_pListener->ButtonPausePressed();
}

void CPresentationBar::ChangeFlashStatus(bool bIsFlashing) {
    m_bIsPauseFlashing = bIsFlashing;
}

void CPresentationBar::OnButtonScreenGrabbing() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_TOOL_SCREENGRABBING);
    else if (m_pListener != NULL)
        m_pListener->ButtonScreenGrabbingPressed();
}

void CPresentationBar::OnButtonPreviousPage() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_PREVIOUS_PAGE);
    else if (m_pListener != NULL)
        m_pListener->ButtonScreenPreviousPagePressed();
}

void CPresentationBar::OnButtonNextPage() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_PAGE_DOWN);
    else if (m_pListener != NULL)
        m_pListener->ButtonScreenNextPagePressed();
}

void CPresentationBar::SetPauseButtonFlash(UINT nFlashPause)
{ 
   if(m_bIsPauseFlashing)
   {
      if(nFlashPause)
         m_pBtnPause.SetChecked(TRUE);
      else
         m_pBtnPause.SetChecked(FALSE);
   }
}

void CPresentationBar::ResetAllControls()
{
} 

void CPresentationBar::OnPensSplitButton()
{
   CRect rcPens;
   m_pBtnPens.GetWindowRect(rcPens);

   if(m_pCommandBars)
   {
      switch(m_nPosition)
      {
      case OT_BOTTOMMIDDLE:
         m_pCommandBars->TrackPopupMenu(ID_MENU_PENS, TPM_LEFTBUTTON , rcPens.left-30 , rcPens.top, NULL, false);
         break;
      case OT_TOPMIDDLE:
         m_pCommandBars->TrackPopupMenu(ID_MENU_PENS, TPM_LEFTBUTTON , rcPens.left-30 , rcPens.bottom, NULL, false);
         break;
      case OT_MIDDLELEFT:
         m_pCommandBars->TrackPopupMenu(ID_MENU_PENS, TPM_LEFTBUTTON , rcPens.Width(), rcPens.top-30, NULL, false);
         break;
      case OT_MIDDLERIGHT:
         m_pCommandBars->TrackPopupMenu(ID_MENU_PENS, TPM_LEFTBUTTON , rcPens.left-158, rcPens.top-30, NULL, false);
         break;
      }
   }
}

void CPresentationBar::OnMarkersSplitButton()
{
   CRect rcMarkers;
   m_pBtnMarkers.GetWindowRect(rcMarkers);

   int nOffset = 30;
   if(m_bSmallIcons)
      nOffset /= 2;
   if(m_pCommandBars)
   {
      switch(m_nPosition)
      {
      case OT_BOTTOMMIDDLE:
         m_pCommandBars->TrackPopupMenu(ID_MENU_MARKERS, TPM_LEFTBUTTON , rcMarkers.left-nOffset , rcMarkers.top, NULL, false);
         break;
      case OT_TOPMIDDLE:
         m_pCommandBars->TrackPopupMenu(ID_MENU_MARKERS, TPM_LEFTBUTTON , rcMarkers.left-nOffset , rcMarkers.bottom, NULL, false);
         break;
      case OT_MIDDLELEFT:
         m_pCommandBars->TrackPopupMenu(ID_MENU_MARKERS, TPM_LEFTBUTTON , rcMarkers.Width(), rcMarkers.top-nOffset, NULL, false);
         break;
      case OT_MIDDLERIGHT:
         m_pCommandBars->TrackPopupMenu(ID_MENU_MARKERS, TPM_LEFTBUTTON , rcMarkers.left-120, rcMarkers.top-nOffset, NULL, false);
         break;
      }
   }
}

void CPresentationBar::OnRButtonDown(UINT nFlags, CPoint point)
{
   ClientToScreen(&point);

   if(m_pCommandBars)
      m_pCommandBars->TrackPopupMenu(IDR_PRESENTATIONBAR, TPM_RIGHTBUTTON , point.x, point.y);

   CWnd::OnRButtonDown(nFlags, point);
}

void CPresentationBar::SetPresentationBarPen(int nPenIndex)
{
   m_nPenIndex = nPenIndex;
   LoadIcons();	   
   RedrawWindow();
}

void CPresentationBar::SetPresentationBarMarker(int nMarkerIndex)
{
   m_nMarkerIndex = nMarkerIndex;
   LoadIcons();
   RedrawWindow();
}

BOOL CPresentationBar::PreTranslateMessage(MSG* pMsg)
{
   if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd != GetSafeHwnd())
   {
      ScreenToClient(&pMsg->pt);
      OnRButtonDown(0, pMsg->pt);
   }

   return CWnd::PreTranslateMessage(pMsg);
}

bool CPresentationBar::IsTransparent()
{
   return m_bShowTransparent;
}

void CPresentationBar::ShowTransparent(bool bShowTransparent)
{
   m_bShowTransparent = bShowTransparent;
   if(!m_bShowTransparent)
      m_bMouseOver = false;
}

void CPresentationBar::SetSmallIcons(bool bSmallIcons)
{
   m_pBtnMarkers.SetSmallIcons(bSmallIcons);
   m_pBtnPens.SetSmallIcons(bSmallIcons);
   RefreshWindowPos(m_nPosition, bSmallIcons);
   m_bSmallIcons = bSmallIcons;
   
   LoadIcons();
}

void CPresentationBar::RecalculateButtonsPos(UINT nButtonId, bool bShow, int nOffset)
{
   bool bIsHorizontal = (m_nPosition == OT_BOTTOMMIDDLE || m_nPosition == OT_TOPMIDDLE);
   bool bFound = false;
   HWND hwndChild = ::GetWindow(GetSafeHwnd(), GW_CHILD);  

   while(hwndChild)   
   {
      CWnd *pwndCtrl = CWnd::FromHandle(hwndChild);
      if(pwndCtrl->GetDlgCtrlID() != nButtonId)
      {
         if(bFound)
         {
            CRect rcCtrl;
            pwndCtrl->GetWindowRect(rcCtrl);
            ScreenToClient(rcCtrl);
            if(bIsHorizontal)
               pwndCtrl->SetWindowPos(NULL, rcCtrl.left+nOffset, rcCtrl.top, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);
            else
               pwndCtrl->SetWindowPos(NULL, rcCtrl.left, rcCtrl.top+nOffset, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);
         }
      }
      else
         bFound = true;
      hwndChild = ::GetWindow( hwndChild, GW_HWNDNEXT);
   }	

   CRect rcWnd;
   GetWindowRect(rcWnd);

   if(bIsHorizontal)
   {
      SetWindowPos(&wndTopMost, rcWnd.left, rcWnd.top, rcWnd.Width()+nOffset, rcWnd.Height(), SWP_NOZORDER);
   }
   else
   {
      SetWindowPos(&wndTopMost, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height()+nOffset, SWP_NOZORDER);
   }
   RedrawWindow();
}

void CPresentationBar::ShowHideSeparators(int &nOffset)
{
   bool bShowExitFullScreen = false;
   bool bShowPresentationTools = false;
   bool bShowRecorderTools = false;
   bool bShowScreenGrabbing = false;
   bool bShowPrevNext = false;

   if(m_bShowStart || m_bShowPause || m_bShowStop)
      bShowRecorderTools = true;

   if(m_bShowScreenGrabbing)
      bShowScreenGrabbing = true;

   if(m_bShowPencils || m_bShowMarkers || m_bShowPointer || m_bShowMouseNav ||
      m_bShowPageFocused || m_bShowDocumentStruct || m_bShowRecordings ||
      m_bShowCut || m_bShowCopy || m_bShowPaste || m_bShowUndo ||
      m_bShowOneClickCopy || m_bShowText || m_bShowLine || m_bShowPolyline ||
      m_bShowFreeHand || m_bShowEllipse || m_bShowRectangle || m_bShowPolygon || m_bShowInsertPage)

   {
      bShowPresentationTools = true;
   }

   if(m_bShowExitFullScreen)
      bShowExitFullScreen = true;

   if(m_bShowPreviousPage || m_bShowNextPage)
      bShowPrevNext = true;

   if(bShowRecorderTools && bShowScreenGrabbing)
   {
      if(!m_bShowSeparator1)
      {
         m_pBtnSeparator1.ShowWindow(TRUE);
         m_bShowSeparator1 = true;
         nOffset += 2;
      }
   }
   else
   {
      if(m_bShowSeparator1)
      {
         m_pBtnSeparator1.ShowWindow(FALSE);
         m_bShowSeparator1 = false;
         nOffset += 2;
      }
   }

   if(bShowPrevNext && (bShowRecorderTools || bShowScreenGrabbing))
   {
      if(!m_bShowSeparator2)
      {
         m_pBtnSeparator2.ShowWindow(TRUE);
         m_bShowSeparator2 = true;
         nOffset += 2;
      }
   } 
   else
   {
      if(m_bShowSeparator2)
      {
         m_pBtnSeparator2.ShowWindow(FALSE);
         m_bShowSeparator2 = false;
         nOffset += 2;
      }
   }

   if(bShowPresentationTools && (bShowRecorderTools || bShowScreenGrabbing || bShowPrevNext))
   {
      if(!m_bShowSeparator4)
      {
         m_pBtnSeparator4.ShowWindow(TRUE);
         m_bShowSeparator4 = true;
         nOffset += 2;
      }
   } 
   else
   {
      if(m_bShowSeparator4)
      {
         m_pBtnSeparator4.ShowWindow(FALSE);
         m_bShowSeparator4 = false;
         nOffset += 2;
      }
   }
  
   if(bShowExitFullScreen && (bShowRecorderTools || bShowScreenGrabbing || bShowPresentationTools || bShowPrevNext))
   {
      if(!m_bShowSeparator3)
      {
         m_pBtnSeparator3.ShowWindow(TRUE);
         m_bShowSeparator3 = true;
         nOffset += 2;
      }
   }
   else
   {
      if(m_bShowSeparator3)
      {
         m_pBtnSeparator3.ShowWindow(FALSE);
         m_bShowSeparator3 = false;
         nOffset += 2;
      }
   }
}

void CPresentationBar::ShowHideExitFullScreen(bool bShow)
{
   int nOffset = 37 + 3;
   m_bShowExitFullScreen = bShow;
   if(bShow)
   {
      m_pBtnExitFullScreen.ShowWindow(TRUE); 
      ShowHideSeparators(nOffset);
   }
   else
   {
      m_pBtnExitFullScreen.ShowWindow(FALSE);
      ShowHideSeparators(nOffset);
      nOffset = -nOffset;
   }
   if(m_bSmallIcons)
      nOffset /= 2;
   RecalculateButtonsPos(IDC_EXIT_FULL_SCREEN, bShow, nOffset);
   RefreshWindowPos(m_nPosition, m_bSmallIcons);
}

void CPresentationBar::ShowHideAnnotations(UINT nId, bool bShow)
{
   int nOffset = 37+8;
   CWnd *pWnd = GetDlgItem(nId);

   if(nId == ID_PRESENTATIONBAR_PENS)
      m_bShowPencils = bShow;
   else
      m_bShowMarkers = bShow;

   if(bShow)
   {
      pWnd->ShowWindow(TRUE);
      ShowHideSeparators(nOffset);
   }
   else
   {
      pWnd->ShowWindow(FALSE);
      ShowHideSeparators(nOffset);
      nOffset = -nOffset;
   }
   if(m_bSmallIcons)
      nOffset /= 2;

   RecalculateButtonsPos(nId, bShow, nOffset);
   RefreshWindowPos(m_nPosition, m_bSmallIcons);
}

void CPresentationBar::ShowHidePresentationTools(UINT nId, bool bShow)
{
   int nOffset = 37;
   CWnd *pWnd = GetDlgItem(nId);

   switch(nId)
   {
   case ID_PRESENTATIONBAR_TELEPOINTER:
      m_bShowPointer = bShow; break;
   case ID_PRESENTATIONBAR_MOUSENAV:
      m_bShowMouseNav = bShow; break;
   case ID_PRESENTATIONBAR_PAGEFOCUSED:
      m_bShowPageFocused = bShow; break;
   case ID_PRESENTATIONBAR_DOCUMENTSTRUCT:
      m_bShowDocumentStruct = bShow; break;
   case ID_PRESENTATIONBAR_RECORDINGS:
      m_bShowRecordings = bShow; break;
   case ID_PRESENTATIONBAR_CUT:
      m_bShowCut = bShow; break;
   case ID_PRESENTATIONBAR_COPY:
      m_bShowCopy = bShow; break;
   case ID_PRESENTATIONBAR_PASTE:
      m_bShowPaste = bShow; break;
   case ID_PRESENTATIONBAR_UNDO:
      m_bShowUndo = bShow; break;
   case ID_PRESENTATIONBAR_ONECLICKCOPY:
      m_bShowOneClickCopy = bShow; break;
   case ID_PRESENTATIONBAR_TEXT:
      m_bShowText = bShow; break;
   case ID_PRESENTATIONBAR_LINE:
      m_bShowLine = bShow; break;
   case ID_PRESENTATIONBAR_POLYLINE:
      m_bShowPolyline = bShow; break;
   case ID_PRESENTATIONBAR_FREEHAND:
      m_bShowFreeHand = bShow; break;
   case ID_PRESENTATIONBAR_ELLIPSE:
      m_bShowEllipse = bShow; break;
   case ID_PRESENTATIONBAR_RECTANGLE:
      m_bShowRectangle = bShow; break;
   case ID_PRESENTATIONBAR_POLYGON:
      m_bShowPolygon = bShow; break;
   case ID_PRESENTATIONBAR_INSERTPAGE:
      m_bShowInsertPage = bShow; break;
   case ID_PRESENTATIONBAR_PREVIOUSPAGE:
      m_bShowPreviousPage = bShow; break;
   case ID_PRESENTATIONBAR_NEXTPAGE:
      m_bShowNextPage = bShow; break;
   }

   if(bShow)
   {
      pWnd->ShowWindow(TRUE);
      ShowHideSeparators(nOffset);
   }
   else
   {
      pWnd->ShowWindow(FALSE);
      ShowHideSeparators(nOffset);
      nOffset = -nOffset;
   }
   if(m_bSmallIcons)
      nOffset /= 2;
   RecalculateButtonsPos(nId, bShow, nOffset);
   RefreshWindowPos(m_nPosition, m_bSmallIcons);
}

void CPresentationBar::ShowHideRecordingTools(UINT nId, bool bShow)
{
   int nOffset = 37;
   CWnd *pWnd = GetDlgItem(nId);

   if(nId == ID_PRESENTATIONBAR_START)
      m_bShowStart = bShow;
   else if(nId == ID_PRESENTATIONBAR_STOP)
      m_bShowStop = bShow;
   else
      m_bShowPause = bShow;

   if(bShow)
   {
      pWnd->ShowWindow(TRUE);
      ShowHideSeparators(nOffset);
   }
   else
   {
      pWnd->ShowWindow(FALSE);
      ShowHideSeparators(nOffset);
      nOffset = -nOffset;
   }
   if(m_bSmallIcons)
      nOffset /= 2;
   RecalculateButtonsPos(nId, bShow, nOffset);
   RefreshWindowPos(m_nPosition, m_bSmallIcons);
}

void CPresentationBar::ShowHideScreenGrabbing(bool bShow)
{
   int nOffset = 8+36;
   m_bShowScreenGrabbing = bShow;
   if(bShow)
   {
      m_pBtnSg.ShowWindow(TRUE);
      ShowHideSeparators(nOffset);
   }
   else
   {
      m_pBtnSg.ShowWindow(FALSE);
      ShowHideSeparators(nOffset);
      nOffset = -nOffset;
   }
   if(m_bSmallIcons)
      nOffset /= 2;
   RecalculateButtonsPos(ID_PRESENTATIONBAR_SCREENGRABBING, bShow, nOffset);
   RefreshWindowPos(m_nPosition, m_bSmallIcons);
}

void CPresentationBar::OnButtonPageFocused() {
    m_pBtnPageFocused.SetChecked(!m_pBtnPageFocused.GetChecked());
    if (m_pMainFrmWnd != NULL) {
        if(m_pBtnPageFocused.GetChecked())
            m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_PAGE_FOCUSED_LAYOUT);
        else
            m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_STANDARD_LAYOUT);     
    }
    else if (m_pListener != NULL)
        m_pListener->ButtonPageFocusedPressed();
}

void CPresentationBar::OnButtonDocumentStructure() {
    m_pBtnDocumentStruct.SetChecked(!m_pBtnDocumentStruct.GetChecked());
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_SHOW_STRUCTURE);
    else if (m_pListener != NULL)
        m_pListener->ButtonDocumentStructurePressed();
}

void CPresentationBar::OnButtonRecordings() {
    m_pBtnRecordings.SetChecked(!m_pBtnRecordings.GetChecked());
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_SHOW_RECORDINGS);
    else if (m_pListener != NULL)
        m_pListener->ButtonRecordingsPressed();
}

void CPresentationBar::OnButtonCut() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_EDIT_CUT);
    else if (m_pListener != NULL)
        m_pListener->ButtonCutPressed();
}

void CPresentationBar::OnButtonCopy() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_EDIT_COPY);
    else if (m_pListener != NULL)
        m_pListener->ButtonCopyPressed();
}

void CPresentationBar::OnButtonPaste() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_EDIT_PASTE);
    else if (m_pListener != NULL)
        m_pListener->ButtonPastePressed();
}

void CPresentationBar::OnButtonUndo() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_EDIT_UNDO);
    else if (m_pListener != NULL)
        m_pListener->ButtonUndoPressed();
}

void CPresentationBar::OnButtonClickCopy() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_TOOL_COPY);
    else if (m_pListener != NULL)
        m_pListener->ButtonClickCopyPressed();
}

void CPresentationBar::OnButtonText() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_TOOL_TEXT);
    else if (m_pListener != NULL)
        m_pListener->ButtonTextPressed();
}

void CPresentationBar::OnButtonLine() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_TOOL_LINE);
    else if (m_pListener != NULL)
        m_pListener->ButtonLinePressed();
}

void CPresentationBar::OnButtonPolyline() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_TOOL_POLYLINE);
    else if (m_pListener != NULL)
        m_pListener->ButtonPolylinePressed();
}

void CPresentationBar::OnButtonFreehand() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_TOOL_FREEHAND);
    else if (m_pListener != NULL)
        m_pListener->ButtonFreehandPressed();
}

void CPresentationBar::OnButtonEllipse() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_TOOL_OVAL);
    else if (m_pListener != NULL)
        m_pListener->ButtonEllipsePressed();
}

void CPresentationBar::OnButtonRectangle() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_TOOL_RECTANGLE);
    else if (m_pListener != NULL)
        m_pListener->ButtonRectanglePressed();
}

void CPresentationBar::OnButtonPolygon() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_TOOL_POLYGON);
    else if (m_pListener != NULL)
        m_pListener->ButtonPolygonPressed();
}

void CPresentationBar::OnButtonInsertPage() {
    if (m_pMainFrmWnd != NULL)
        m_pMainFrmWnd->SendMessage(WM_COMMAND, ID_PAGE_NEW);
    else if (m_pListener != NULL)
        m_pListener->ButtonInsertPagePressed();
}

void CPresentationBar::OnUpdateButtonScreenGrabbing(CCmdUI* pCmdUI)
{
   BOOL bEnable = FALSE; 
   if(ASSISTANT::Project::active != NULL && 
      (ASSISTANT::Project::active->IsRecording() || !ASSISTANT::Project::active->GetActiveDocument()))
      bEnable = TRUE;

   if(ASSISTANT::Project::active->IsSgOnlyDocument())
      bEnable = FALSE;

   if(m_bIsPreparationMode)
      bEnable = FALSE;

   pCmdUI->Enable(bEnable);
}

void CPresentationBar::OnUpdateTools(CCmdUI* pCmdUI)
{
    BOOL bEnable = FALSE; 
    BOOL bToolSelected = FALSE;

    if((ASSISTANT::Project::active && ASSISTANT::Project::active->GetActivePage()) || 
        ASSISTANT::Project::active->isScreenGrabbingActive_) {
            CAssistantView *pAssistantView = NULL;
            if (m_pAssistantDoc) {
                pAssistantView = m_pAssistantDoc->GetWhiteboardView();
                if (pAssistantView) {
                    int iCurrentTool = pAssistantView->GetCurrentTool();
                    if ((iCurrentTool == ID_TOOL_PENBLACK ||  iCurrentTool == ID_TOOL_PENBLUE || 
                        iCurrentTool == ID_TOOL_PENCYAN ||  iCurrentTool == ID_TOOL_PENGREEN || 
                        iCurrentTool == ID_TOOL_PENMAGENTA ||  iCurrentTool == ID_TOOL_PENRED || 
                        iCurrentTool == ID_TOOL_PENWHITE ||  iCurrentTool == ID_TOOL_PENYELLOW) && 
                        pCmdUI->m_nID == ID_PRESENTATIONBAR_PENS)
                        bToolSelected = TRUE;
                    else if ((iCurrentTool == ID_TOOL_MARKERBLUE ||  iCurrentTool == ID_TOOL_MARKERGREEN || 
                        iCurrentTool == ID_TOOL_MARKERMAGENTA ||  iCurrentTool == ID_TOOL_MARKERORANGE || 
                        iCurrentTool == ID_TOOL_MARKERRED ||  iCurrentTool == ID_TOOL_MARKERYELLOW) && 
                        pCmdUI->m_nID == ID_PRESENTATIONBAR_MARKERS)
                        bToolSelected = TRUE;
                    else if (iCurrentTool == ID_TOOL_TELEPOINTER && pCmdUI->m_nID == ID_PRESENTATIONBAR_TELEPOINTER)
                        bToolSelected = TRUE;
                    else if (iCurrentTool == ID_TOOL_SIMPLENAVIGATION && pCmdUI->m_nID == ID_PRESENTATIONBAR_MOUSENAV)
                        bToolSelected = TRUE;
                    else if (iCurrentTool == ID_TOOL_COPY && pCmdUI->m_nID == ID_PRESENTATIONBAR_ONECLICKCOPY)
                        bToolSelected = TRUE;
                    else if (iCurrentTool == ID_TOOL_TEXT && pCmdUI->m_nID == ID_PRESENTATIONBAR_TEXT)
                        bToolSelected = TRUE;
                    else if (iCurrentTool == ID_TOOL_LINE && pCmdUI->m_nID == ID_PRESENTATIONBAR_LINE)
                        bToolSelected = TRUE;
                    else if (iCurrentTool == ID_TOOL_POLYLINE && pCmdUI->m_nID == ID_PRESENTATIONBAR_POLYLINE)
                        bToolSelected = TRUE;
                    else if (iCurrentTool == ID_TOOL_FREEHAND && pCmdUI->m_nID == ID_PRESENTATIONBAR_FREEHAND)
                        bToolSelected = TRUE;
                    else if (iCurrentTool == ID_TOOL_OVAL && pCmdUI->m_nID == ID_PRESENTATIONBAR_ELLIPSE)
                        bToolSelected = TRUE;
                    else if (iCurrentTool == ID_TOOL_RECTANGLE && pCmdUI->m_nID == ID_PRESENTATIONBAR_RECTANGLE)
                        bToolSelected = TRUE;
                    else if (iCurrentTool == ID_TOOL_POLYGON && pCmdUI->m_nID == ID_PRESENTATIONBAR_POLYGON)
                        bToolSelected = TRUE;
                }

            }
            bEnable = TRUE;
    }

    CWnd *pButton = GetDlgItem(pCmdUI->m_nID);
    if (pButton && pButton->GetRuntimeClass() == RUNTIME_CLASS(CPresentationBarControls))
        ((CPresentationBarControls *)pButton)->SetChecked(bToolSelected);
    else
        pCmdUI->SetCheck(bToolSelected);

    if(m_bIsPreparationMode)
      bEnable = FALSE;

    if(ASSISTANT::Project::active->isScreenGrabbingActive_)
    {
       if(pCmdUI->m_nID != ID_PRESENTATIONBAR_PENS && pCmdUI->m_nID != ID_PRESENTATIONBAR_MARKERS
          && pCmdUI->m_nID != ID_PRESENTATIONBAR_TELEPOINTER)
       {  
          bEnable = FALSE;
       }
    }

    pCmdUI->Enable(bEnable);
}

void CPresentationBar::OnUpdateStart(CCmdUI* pCmdUI)
{
    BOOL bEnable = FALSE;

    if (CStudioApp::IsLiveContextMode()) {
        bEnable = FALSE;
    } else {
        if ((m_pAssistantDoc && m_pAssistantDoc->IsRecording()) || !ASSISTANT::Project::active->GetActiveDocument())
            bEnable = FALSE;
        else
            bEnable = TRUE;
    }

    if(m_bIsPreparationMode)
       bEnable = TRUE;

    pCmdUI->Enable(bEnable);
}

void CPresentationBar::OnUpdateStop(CCmdUI* pCmdUI) {
    BOOL bEnable = FALSE;

    if (CStudioApp::IsLiveContextMode() && 
        ASSISTANT::Project::active != NULL && ASSISTANT::Project::active->isScreenGrabbingActive_) {
        bEnable = TRUE;
    } else if (m_pAssistantDoc && m_pAssistantDoc->IsRecording()) {
        if(!ASSISTANT::Project::active->IsSgOnlyDocument() && ASSISTANT::Project::active->isScreenGrabbingActive_ )
            bEnable = FALSE;
        else
            bEnable = TRUE;
    }
    else
        bEnable = FALSE;

    if (m_bIsPreparationMode)
        bEnable = FALSE;

    pCmdUI->Enable(bEnable);
}

void CPresentationBar::OnUpdatePause(CCmdUI* pCmdUI) {
    BOOL bEnable = FALSE;

    if (CStudioApp::IsLiveContextMode() && 
        ASSISTANT::Project::active != NULL && ASSISTANT::Project::active->isScreenGrabbingActive_) {    
        if (!ASSISTANT::Project::active->IsRecordingPaused()) {
            CWnd *pButton = GetDlgItem(pCmdUI->m_nID);
            ((CPresentationBarControls *)pButton)->SetChecked(false);
        }
        bEnable = TRUE;
    } else if (m_pAssistantDoc && m_pAssistantDoc->IsRecording()) {
        if (!m_pAssistantDoc->IsRecordingPaused()) {
            CWnd *pButton = GetDlgItem(pCmdUI->m_nID);
            ((CPresentationBarControls *)pButton)->SetChecked(false);
        }
        bEnable = TRUE;
    }
    else
        bEnable = FALSE;

    if (m_bIsPreparationMode)
        bEnable = FALSE;

    pCmdUI->Enable(bEnable);
}

void CPresentationBar::OnUpdateCutCopy(CCmdUI* pCmdUI)
{
   BOOL bEnable = FALSE; 
   if (ASSISTANT::Project::active && 
      ASSISTANT::Project::active->GetActiveDocument() && 
      ASSISTANT::Project::active->GetActivePage() && 
      ASSISTANT::Project::active->HasSelectedObjects())
      bEnable = TRUE;

   if(m_bIsPreparationMode)
      bEnable = FALSE;

   pCmdUI->Enable(bEnable);
}

void CPresentationBar::OnUpdatePaste(CCmdUI* pCmdUI)
{
   BOOL bEnable = FALSE; 
   if (ASSISTANT::Project::active && 
      ASSISTANT::Project::active->GetActiveDocument() && 
      ASSISTANT::Project::active->GetActivePage() && 
      !ASSISTANT::Project::active->IsPasteBufferEmpty())
      bEnable = TRUE;

   if(m_bIsPreparationMode)
      bEnable = FALSE;
   pCmdUI->Enable(bEnable);
}

void CPresentationBar::OnUpdateUndo(CCmdUI* pCmdUI) 
{
   BOOL bEnable = FALSE;
   bool bIsRecording = (m_pAssistantDoc && m_pAssistantDoc->IsRecording() && ASSISTANT::Project::active->GetAvGrabber()->getSgAnnotationLastUndoTimeStamp() != -1);
   bool bIsPageChanged = (m_pAssistantDoc && m_pAssistantDoc->HasLoadedDocuments() && m_pAssistantDoc->PageIsChanged());
 
   if(bIsRecording || bIsPageChanged)
      bEnable = TRUE;

   if(m_bIsPreparationMode)
      bEnable = FALSE;

   pCmdUI->Enable(bEnable);
}

void CPresentationBar::OnUpdateNonSgControls(CCmdUI* pCmdUI) {
    BOOL bEnable = FALSE; 

    if(!ASSISTANT::Project::active->isScreenGrabbingActive_ && !m_bIsPreparationMode)
        bEnable = TRUE;

    if (bEnable) {
        if(ASSISTANT::Project::active->GetActivePage()) {
            if(pCmdUI->m_nID == ID_PRESENTATIONBAR_PREVIOUSPAGE) {
                if (!ASSISTANT::Project::active->PreviousPageAvailable())
                    bEnable = FALSE;
            } else if(pCmdUI->m_nID == ID_PRESENTATIONBAR_NEXTPAGE) {
                if (!ASSISTANT::Project::active->NextPageAvailable())
                    bEnable = FALSE;
            }
        } else
            bEnable = FALSE;
    }

    CXTPButton *pButton = (CXTPButton *)GetDlgItem(pCmdUI->m_nID);
    pButton->EnableWindow(bEnable);
}

void CPresentationBar::SetWindowAnimation()
{
    if (m_bAnimationFinished) {
        m_bAnimationFinished = false;
        m_pAnimateWindowThread = AfxBeginThread(StartShowHideAnimation, this);
    }
}

CRect CPresentationBar::GetStandardWindowRect()
{
    return m_rcStandard;
}

void CPresentationBar::GetTooltipText(UINT nTextID, CString &csTitle, CString &csDescription)  const 
{ 
   CString csFromResources; 
   csFromResources.LoadString(nTextID); 
   int iPos = csFromResources.ReverseFind(_T('\n')); 
   csTitle = csFromResources.Right(csFromResources.GetLength() - iPos - 1); 
   csDescription = csFromResources.Left(iPos); 
   if(csDescription.IsEmpty()==true)
   {
     csDescription = csTitle;
   }
}

CString CPresentationBar::GetToolTipByID(UINT uID) const 
{
   CString strTipText;
   // don't handle the message if no string resource found
   if (XTPResourceManager()->LoadString(&strTipText, uID))
      return strTipText;

   return _T("Title");
}

BOOL CPresentationBar::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
   ASSERT(pNMHDR->code == TTN_NEEDTEXT);

   TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;

   if (!(pTTT->uFlags & TTF_IDISHWND))
      return FALSE;

   UINT_PTR hWnd = pNMHDR->idFrom;
   // idFrom is actually the HWND of the tool
   UINT nID = (UINT)(WORD)::GetDlgCtrlID((HWND)hWnd);
   INT_PTR nHit = (UINT)(WORD)::GetDlgCtrlID((HWND)hWnd);

 
   pTTT->hinst = AfxGetInstanceHandle();

   *pResult = 0;
   // bring the tooltip window above other popup windows
   ::SetWindowPos(pNMHDR->hwndFrom,  HWND_TOP, 0, 0, 0, 0,
      SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
   return TRUE;    // message was handled
}

INT_PTR CPresentationBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
   HWND hWndChild = NULL;
   // find child window which hits the point
   // (don't use WindowFromPoint, because it ignores disabled windows)
   // see _AfxChildWindowFromPoint(m_hWnd, point);
   ::ClientToScreen(m_hWnd, &point);
   HWND hChild = ::GetWindow(m_hWnd, GW_CHILD);
   for (; hChild != NULL; hChild = ::GetWindow(hChild, GW_HWNDNEXT))
   {
      if ((UINT)(WORD)::GetDlgCtrlID(hChild) != (WORD)-1 &&
         (::GetWindowLong(hChild, GWL_STYLE) & WS_VISIBLE))
      {
         // see if point hits the child window
         CRect rect;
         ::GetWindowRect(hChild, rect);
         if (rect.PtInRect(point))
         {
            hWndChild = hChild;
            break;
         }
      }
   }

   if (hWndChild != NULL)
   {
      // return positive hit if control ID isn't -1
      INT_PTR nHit = (UINT)(WORD)::GetDlgCtrlID(hWndChild);
      CRect rcItem;
      ::GetWindowRect(hWndChild, rcItem);

      // hits against child windows always center the tip
      if (pTI != NULL && pTI->cbSize >= sizeof(XTP_TOOLTIP_TOOLINFO))
      {
         // setup the TOOLINFO structure
         pTI->hwnd = m_hWnd;
         pTI->uId = (UINT_PTR)hWndChild;
         pTI->uFlags |= TTF_IDISHWND;
         pTI->lpszText = LPSTR_TEXTCALLBACK;

         // set TTF_NOTBUTTON and TTF_CENTERTIP if it isn't a button
         if (!(::SendMessage(hWndChild, WM_GETDLGCODE, 0, 0) &
            DLGC_BUTTON))
            pTI->uFlags |= TTF_NOTBUTTON|TTF_CENTERTIP;
      }
      CString csDescription;
      CString csTitle = GetToolTipByID(nHit);
      bool bIsScreenGrabbing = ASSISTANT::Project::active->isScreenGrabbingActive_ && !ASSISTANT::Project::active->IsSgOnlyDocument();
      bool bIsScreenGrabbingActive= ASSISTANT::Project::active->isScreenGrabbingActive_;
      bool bIsRecording = ASSISTANT::Project::active->IsRecording();
      switch(nHit)
      {
      case ID_PRESENTATIONBAR_START: 
         {
            if((m_bIsPreparationMode==true)||(bIsScreenGrabbingActive==true))
            {
            if(bIsRecording == true)
            {
                  GetTooltipText(IDS_ID_START_RECORDING_SG_ACTIVE,csTitle,csDescription);
               }
               else
               {
                  GetTooltipText(IDS_ID_START_RECORDING_SG_INACTIVE,csTitle,csDescription);
               }
            }
            else if(bIsScreenGrabbingActive==false)
            {
               if(bIsRecording == true)
               {
              GetTooltipText(IDS_ID_START_RECORDING_PRESENTATION_ACTIVE,csTitle,csDescription);
            }
            else
            {
              GetTooltipText(IDS_ID_START_RECORDING_PRESENTATION_INACTIVE,csTitle,csDescription);
            }
            }
            break;
         }
      case ID_PRESENTATIONBAR_PAUSE: 
         {

            if((m_bIsPreparationMode==true)||(bIsScreenGrabbingActive==true))
            {
               if(m_pAssistantDoc && m_pAssistantDoc->IsRecordingPaused() == true)
               {
                  GetTooltipText(IDS_ID_RESUME_SG_RECORDING,csTitle,csDescription);
               }
               else
               {
                  GetTooltipText(IDS_ID_PAUSE_SG_RECORDING,csTitle,csDescription);
               }
            }
            else if(bIsScreenGrabbingActive==false)
            {
               if(m_pAssistantDoc && m_pAssistantDoc->IsRecordingPaused() == false)
               {
               //GetTooltipText(IDS_ID_RESUME_PRESENTATION_RECORDING,csTitle,csDescription);
                  GetTooltipText(IDS_ID_PAUSE_PRESENTATION_RECORDING,csTitle,csDescription);
               }
               else
               {
                  GetTooltipText(IDS_ID_RESUME_PRESENTATION_RECORDING,csTitle,csDescription);
               //GetTooltipText(IDS_ID_PAUSE_PRESENTATION_RECORDING,csTitle,csDescription);
               }
            }

            break;
         }
      case ID_PRESENTATIONBAR_STOP: 
         {
            if((m_bIsPreparationMode==false)&&(bIsScreenGrabbingActive==false))
            {
            GetTooltipText(IDS_ID_STOP_RECORDING,csTitle,csDescription);
            }
            else
            {
                if(bIsScreenGrabbingActive == true && !ASSISTANT::Project::active->IsSgOnlyDocument())
                    GetTooltipText(IDS_ID_STOP_RECORDING_NOT_SG_ONLY, csTitle, csDescription);
                else
                    GetTooltipText(IDS_ID_STOP_RECORDING_SG,csTitle,csDescription);
            }           
            break;
         }
      case ID_PRESENTATIONBAR_NEXTPAGE: 
         {
            GetTooltipText(IDS_ID_PRESENTATIONBAR_NEXTPAGE,csTitle,csDescription);
            break;
         }
      case ID_PRESENTATIONBAR_PREVIOUSPAGE: 
         {
            GetTooltipText(IDS_ID_PRESENTATIONBAR_PREVIOUSPAGE,csTitle,csDescription);
            break;
         }
      case ID_PRESENTATIONBAR_SCREENGRABBING: 
         {
            //if(bIsScreenGrabbing)
            if((m_bIsPreparationMode==false)&&(bIsScreenGrabbingActive==false))
               //GetTooltipText(IDS_PRESENTATIONBAR_SCREENGRABBING,csTitle,csDescription);
               GetTooltipText(/*IDS_ID_TOOL_SCREENGRABBING*/IDS_TOOLTIP_SG_S2,csTitle,csDescription);
            else
               //GetTooltipText(/*IDS_ID_TOOL_SCREENGRABBING*/IDS_TOOLTIP_SG_S2,csTitle,csDescription);
               GetTooltipText(IDS_PRESENTATIONBAR_SCREENGRABBING,csTitle,csDescription);
            break;
         }
      case ID_PRESENTATIONBAR_PENS: 
         {
            if((m_bIsPreparationMode==false)&&(bIsScreenGrabbingActive==false))
            {
            GetTooltipText(IDS_ID_BUTTON_PENS,csTitle,csDescription);
            }
            else
            {
               GetTooltipText(IDS_ID_BUTTON_PENS_SG,csTitle,csDescription);
            }
            break;
         }
      case ID_PRESENTATIONBAR_MARKERS: 
         {
            if((m_bIsPreparationMode==false)&&(bIsScreenGrabbingActive==false))
            {
            GetTooltipText(IDS_ID_BUTTON_MARKERS,csTitle,csDescription);
            }
            else
            {
               GetTooltipText(IDS_ID_BUTTON_MARKERS_SG,csTitle,csDescription);
            }
            break;
         }
      case ID_PRESENTATIONBAR_TELEPOINTER: 
         {
            if((m_bIsPreparationMode==false)&&(bIsScreenGrabbingActive==false))
            {
            GetTooltipText(IDS_ID_TOOL_TELEPOINTER,csTitle,csDescription);
            }           
            else
            {
               GetTooltipText(IDS_ID_TOOL_TELEPOINTER_SG,csTitle,csDescription);
            }
            break;
         }
      case ID_PRESENTATIONBAR_MOUSENAV: 
         {
            GetTooltipText(IDS_ID_TOOL_SIMPLENAVIGATION,csTitle,csDescription);
            break;
         }
      case ID_PRESENTATIONBAR_PAGEFOCUSED: 
         {
            GetTooltipText(IDS_ID_PAGE_FOCUSED_LAYOUT,csTitle,csDescription); 
            break;
         }
      case ID_PRESENTATIONBAR_DOCUMENTSTRUCT: 
         {
            GetTooltipText(ID_SHOW_STRUCTURE,csTitle,csDescription);
            break;
         }
      case ID_PRESENTATIONBAR_RECORDINGS: 
         {
            GetTooltipText(ID_SHOW_RECORDINGS,csTitle,csDescription);
            break;
         }
      case ID_PRESENTATIONBAR_CUT: 
         {
            GetTooltipText(IDS_ID_EDIT_CUT,csTitle,csDescription); 
            break;
         }
      case ID_PRESENTATIONBAR_COPY: 
         {
            GetTooltipText(IDS_ID_EDIT_COPY,csTitle,csDescription);
            break;
         }
      case ID_PRESENTATIONBAR_PASTE: 
         {
            GetTooltipText(IDS_ID_EDIT_PASTE,csTitle,csDescription); 
            break;
         }
      case ID_PRESENTATIONBAR_UNDO: 
         {
            GetTooltipText(ID_EDIT_UNDO,csTitle,csDescription);
            break;
         }
      case ID_PRESENTATIONBAR_ONECLICKCOPY: 
         {
            GetTooltipText(IDS_ID_TOOL_COPY,csTitle,csDescription); 
            break;
         }
      case ID_PRESENTATIONBAR_TEXT:
         {
            GetTooltipText(IDS_ID_TOOL_TEXT,csTitle,csDescription); 
            break;
         }
      case ID_PRESENTATIONBAR_LINE:
         {
            GetTooltipText(IDS_ID_TOOL_LINE,csTitle,csDescription);
            break;
         }
      case ID_PRESENTATIONBAR_POLYLINE:
         { 

            GetTooltipText(IDS_ID_TOOL_POLYLINE,csTitle,csDescription);
            break;
         }
      case ID_PRESENTATIONBAR_FREEHAND:
         { 
            GetTooltipText(IDS_ID_TOOL_FREEHAND,csTitle,csDescription);
            break;
         }
      case ID_PRESENTATIONBAR_ELLIPSE:
         { 
            GetTooltipText(IDS_ID_TOOL_OVAL,csTitle,csDescription);
            break;
         }
      case ID_PRESENTATIONBAR_RECTANGLE: 
         {
            GetTooltipText(IDS_ID_TOOL_RECTANGLE,csTitle,csDescription); 
            break;
         }
      case ID_PRESENTATIONBAR_POLYGON: 
         {
            GetTooltipText(IDS_ID_TOOL_POLYGON,csTitle,csDescription); 
            break;
         }
      case ID_PRESENTATIONBAR_INSERTPAGE: 
         {
            GetTooltipText(IDS_ID_PAGE_NEW,csTitle,csDescription);
            break;
         }
      case IDC_EXIT_FULL_SCREEN: 
         {
            GetTooltipText(IDC_EXIT_FULL_SCREEN,csTitle,csDescription);
            break;
         }
      default: GetTooltipText(nHit,csTitle,csDescription); break;
      }

      m_ctrlToolTipsContext->FillInToolInfo(pTI, GetSafeHwnd(), rcItem, nHit, csTitle, csTitle, csDescription, NULL);

      return nHit;
   }
   return -1;  // not found
}
void CPresentationBar::CreateToolTips()
{
   // Set tooltip context
   if(m_ctrlToolTipsContext !=NULL)
   {
    ASSERT(m_ctrlToolTipsContext);
    return;
   }
   EnableToolTips(TRUE);
   
   m_ctrlToolTipsContext = new CXTPToolTipContext();
   m_ctrlToolTipsContext->SetModuleToolTipContext();
   m_ctrlToolTipsContext->SetStyle(xtpToolTipOffice2007);
   m_ctrlToolTipsContext->SetMaxTipWidth(200);
   m_ctrlToolTipsContext->SetMargin(CRect(2, 2, 2, 2));  
   m_ctrlToolTipsContext->SetDelayTime(TTDT_INITIAL, 900);
   m_ctrlToolTipsContext->ShowTitleAndDescription();
   m_ctrlToolTipsContext->ShowToolTipAlways();
}

void CPresentationBar::UpdateAppearance() 
{
   CPoint ptMouse;
   GetCursorPos(&ptMouse);

   bool bIsOnWnd = (ptMouse.x>=m_rcStandard.left &&  ptMouse.x<=m_rcStandard.right && ptMouse.y>=m_rcStandard.top && ptMouse.y<=m_rcStandard.bottom);
   
   if(m_bShowTransparent)
   {
      if(!m_bMouseOver && bIsOnWnd)
      {
         m_bMouseOver = true; 
         SetLayeredWindowAttributes(RGB(255, 255, 255), 255, LWA_ALPHA | LWA_COLORKEY); 
         LoadIcons();
      }
      else
      {
         if(m_bMouseOver && !bIsOnWnd)
         {
            m_bMouseOver = false;
            SetLayeredWindowAttributes(RGB(255, 255, 255), (255 * 30) / 100, LWA_ALPHA | LWA_COLORKEY);
            LoadIcons();
         }
      }
   }
   else
   {
      if(!m_bMouseOver)
      {
         m_bMouseOver = true; 
         SetLayeredWindowAttributes(RGB(255, 255, 255), 255, LWA_ALPHA | LWA_COLORKEY); 
         LoadIcons();
      }
   }

   if(m_bIsAutoHide && IsWindowVisible())
   {
      if(bIsOnWnd && !m_bWindowVisible)
         SetWindowAnimation();
      else
         if(!bIsOnWnd && m_bWindowVisible)
            SetWindowAnimation();
   } 
}

void CPresentationBar::PutWindowOnTop() {
    POINT pt;
    switch(m_nPosition)
    {
    case OT_BOTTOMMIDDLE:
        pt.x = m_rcStandard.left + 1;
        pt.y = m_rcStandard.top + 5;
        break;
    case OT_TOPMIDDLE:
        pt.x = m_rcStandard.left + 1;
        pt.y = m_rcStandard.top + 1;
        break;
    case OT_MIDDLELEFT:
        pt.x = m_rcStandard.left + 1;
        pt.y = m_rcStandard.top + 1;
        break;
    case OT_MIDDLERIGHT:
        pt.x = m_rcStandard.left + 5;
        pt.y = m_rcStandard.top + 1;
        break;
    }

    // Find window where Presentation bar should be displayed. If this window is not Presentation bar, put presentation bar on top
    // (selected point must not be a button from presentation bar)
    CWnd *wndFromPoint = WindowFromPoint(pt);
    if (wndFromPoint != this) {
        this->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
    }
}

void CPresentationBar::OnMouseMove(UINT nFlags, CPoint point) 
{
   //SetForegroundWindow();
   CWnd::OnMouseMove(nFlags, point);
}

void CPresentationBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
   CWnd::OnShowWindow(bShow, nStatus);
   if(bShow && m_bIsAutoHide)
   {
      SetWindowAnimation();
   }
}
void CPresentationBar::SetAutoHide(bool bAutoHide)
{
   m_bIsAutoHide = bAutoHide;
   if(m_bIsAutoHide)
   {
      m_bMouseOver = true;
   }
   
   RedrawWindow();
}

bool CPresentationBar::IsAutoHide()
{
   return m_bIsAutoHide;
}

void CPresentationBar::SetMainFrmWnd(CWnd *pMainFrmWnd)
{
   m_pMainFrmWnd = pMainFrmWnd;
}

void CPresentationBar::SetPreparationMode(bool bIsPreparationMode)
{
   m_bIsPreparationMode = bIsPreparationMode;
}

void CPresentationBar::SaveSettings()
{
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Position"), GetPosition());

   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Transparent"), m_bShowTransparent);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("SmallIcons"), m_bSmallIcons);

   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("ExitFullScreen"), m_bShowExitFullScreen);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Pencils"), m_bShowPencils);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Markers"), m_bShowMarkers);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Pointer"), m_bShowPointer);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Navigation"), m_bShowMouseNav);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Start"), m_bShowStart);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Stop"), m_bShowStop);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Pause"), m_bShowPause);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("ScreenGrabbing"), m_bShowScreenGrabbing);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("PageFocused"), m_bShowPageFocused);

   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("DocumentStruct"), m_bShowDocumentStruct);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Recordings"), m_bShowRecordings);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Cut"), m_bShowCut);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Copy"), m_bShowCopy);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Paste"), m_bShowPaste);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Undo"), m_bShowUndo);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("OneClickCopy"), m_bShowOneClickCopy);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Text"), m_bShowText);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Line"), m_bShowLine);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Polyline"), m_bShowPolyline);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("FreeHand"), m_bShowFreeHand);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Ellipse"), m_bShowEllipse);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Rectangle"), m_bShowRectangle);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("Polygon"), m_bShowPolygon);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("InsertPage"), m_bShowInsertPage);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("PreviousPage"), m_bShowPreviousPage);
   AfxGetApp()->WriteProfileInt(_T("PresentationBarOptions"), _T("NextPage"), m_bShowNextPage);
}

void CPresentationBar::LoadSettings()
{
   
   bool bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("ExitFullScreen"), 1) == 1)?true:false;
   if(bShowButton != m_bShowExitFullScreen)
      ShowHideExitFullScreen(bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Pencils"), 1) == 1)?true:false;
   if(bShowButton != m_bShowPencils)
      ShowHideAnnotations(ID_PRESENTATIONBAR_PENS, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Markers"), 1) == 1)?true:false;
   if(bShowButton != m_bShowMarkers)
      ShowHideAnnotations(ID_PRESENTATIONBAR_MARKERS, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Pointer"), 1) == 1)?true:false;
   if(bShowButton != m_bShowPointer)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_TELEPOINTER, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Navigation"), 1) == 1)?true:false;
   if(bShowButton != m_bShowMouseNav)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_MOUSENAV, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Start"), 1) == 1)?true:false;
   if(bShowButton != m_bShowStart)
      ShowHideRecordingTools(ID_PRESENTATIONBAR_START, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Stop"), 1) == 1)?true:false;
   if(bShowButton != m_bShowStop)
      ShowHideRecordingTools(ID_PRESENTATIONBAR_STOP, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Pause"), 1) == 1)?true:false;
   if(bShowButton != m_bShowPause)
      ShowHideRecordingTools(ID_PRESENTATIONBAR_PAUSE, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("ScreenGrabbing"), 1) == 1)?true:false;
   if(bShowButton != m_bShowScreenGrabbing)
      ShowHideScreenGrabbing(bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("PageFocused"), 0) == 1)?true:false;
   if(bShowButton != m_bShowPageFocused)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_PAGEFOCUSED, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("DocumentStruct"), 0) == 1)?true:false;
   if(bShowButton != m_bShowDocumentStruct)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_DOCUMENTSTRUCT, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Recordings"), 0) == 1)?true:false;
   if(bShowButton != m_bShowRecordings)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_RECORDINGS, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Cut"), 0) == 1)?true:false;
   if(bShowButton != m_bShowCut)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_CUT, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Copy"), 0) == 1)?true:false;
   if(bShowButton != m_bShowCopy)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_COPY, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Paste"), 0) == 1)?true:false;
   if(bShowButton != m_bShowPaste)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_PASTE, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Undo"), 0) == 1)?true:false;
   if(bShowButton != m_bShowUndo)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_UNDO, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("OneClickCopy"), 0) == 1)?true:false;
   if(bShowButton != m_bShowOneClickCopy)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_ONECLICKCOPY, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Text"), 0) == 1)?true:false;
   if(bShowButton != m_bShowText)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_TEXT, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Line"), 0) == 1)?true:false;
   if(bShowButton != m_bShowLine)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_LINE, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Polyline"), 0) == 1)?true:false;
   if(bShowButton != m_bShowPolyline)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_POLYLINE, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("FreeHand"), 0) == 1)?true:false;
   if(bShowButton != m_bShowFreeHand)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_FREEHAND, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Ellipse"), 0) == 1)?true:false;
   if(bShowButton != m_bShowEllipse)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_ELLIPSE, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Rectangle"), 0) == 1)?true:false;
   if(bShowButton != m_bShowRectangle)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_RECTANGLE, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Polygon"), 0) == 1)?true:false;
   if(bShowButton != m_bShowPolygon)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_POLYGON, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("InsertPage"), 0) == 1)?true:false;
   if(bShowButton != m_bShowInsertPage)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_INSERTPAGE, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("PreviousPage"), 1) == 1)?true:false;
   if(bShowButton != m_bShowPreviousPage)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_PREVIOUSPAGE, bShowButton);

   bShowButton = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("NextPage"), 1) == 1)?true:false;
   if(bShowButton != m_bShowNextPage)
      ShowHidePresentationTools(ID_PRESENTATIONBAR_NEXTPAGE, bShowButton);
  
   bool bSmallIcons = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("SmallIcons"), 0) == 1)?true:false;
   SetSmallIcons(bSmallIcons);

   bool bShowTransparent = (AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Transparent"), 1) == 1)?true:false;
   ShowTransparent(bShowTransparent);

   UINT nPos = AfxGetApp()->GetProfileInt(_T("PresentationBarOptions"), _T("Position"), OT_BOTTOMMIDDLE);
   SetPosition(nPos);
}

void CPresentationBar::OnDestroy()
{
   SaveSettings(); 
   CWnd::OnDestroy();
}

BOOL CPresentationBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
    if (!m_bIsPreparationMode 
        && !ASSISTANT::Project::active->isScreenGrabbingActive_
        && m_pAssistantDoc != NULL
        && m_pAssistantDoc->HasLoadedDocuments()
        && m_pAssistantDoc->HasActivePage()
        && !m_pAssistantDoc->GetShowMouseCursor()) {
            ::SetCursor(m_hNoCursor);
            return TRUE;
    }
    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
