// StartupView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#else
#include "securityutils.h"
#endif
#include "MainFrm.h"
#include "StartupView.h"

#include "backend\mlb_misc.h"


// CStartupView

IMPLEMENT_DYNAMIC(CStartupView, CScrollView)

BEGIN_MESSAGE_MAP(CStartupView, CScrollView)
   ON_WM_CREATE()
   ON_WM_TIMER()
   ON_WM_SIZE()
   ON_BN_CLICKED(IDC_CHECK_STARTPAGE, OnCheckStartpage)
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnUpdateButtons)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
   ON_COMMAND(IDC_CLOSESTARTPAGE, OnCloseStartpage)
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CStartupView::OnTimer(UINT nIDEvent)
{
   Invalidate(FALSE);
}

CStartupView::CStartupView()
{
   m_csStartPage.LoadString(IDS_STARTPAGE);

   m_csTabFontFamily = _T("Arial");
   m_iTabFontSize = 11;

   m_csFontFamily = _T("Segoe UI");
   m_iTitleFontSize = 14;
   m_iNormalFontSize = 12;

   m_iTabHeight = 0;
   m_iTabWidth = 0;

   m_pLecturnityLogo = NULL;
   m_pImcLogo = NULL;

   m_pBmpTopLeft = NULL;
   m_pBmpTopMiddle = NULL;
   m_pBmpTopRight = NULL;

   m_pBmpBlueLeft = NULL;
   m_pBmpBlueMiddle = NULL;
   m_pBmpBlueRight = NULL;

   m_pBmpWhiteLeft = NULL;
   m_pBmpWhiteMiddle = NULL;
   m_pBmpWhiteRight = NULL;

   m_pBmpLightBlueLeft = NULL;
   m_pBmpLightBlueMiddle = NULL;
   m_pBmpLightBlueRight = NULL;

   m_pBmpFootLeft = NULL;
   m_pBmpFootMiddle = NULL;
   m_pBmpFootRight = NULL;

   m_iButtonRegionHeight = BUTTON_TITLE_HEIGHT + BUTTON_SUBTITLE_HEIGHT + 
      SEPARATOR_HEIGHT + 7*BUTTON_HEIGHT;
   m_nImageAlignment = 0;  // middle left
   m_nTextAlignment = 0;   // middle left
   m_nTheme = 6;           // Office2007
   m_bUseVisualStyle = TRUE; 
   m_bScreenTips = TRUE;
   m_ctrlToolTipsContext =NULL;
   m_bToolTipShow = false;
   m_pImageManager = NULL;
   m_bIsRetFromEditMode = false;
   m_szLastSize = CSize(0,0);
   m_rcLastMainWndRect = CRect(0, 0, 0, 0);

}

CStartupView::~CStartupView()
{
   if (m_pLecturnityLogo)
      delete m_pLecturnityLogo;

   if (m_pImcLogo)
      delete m_pImcLogo;

   if(m_pBmpTopLeft)
      delete m_pBmpTopLeft;
   if(m_pBmpTopMiddle)
      delete m_pBmpTopMiddle;
   if(m_pBmpTopRight)
      delete m_pBmpTopRight;

   if(m_pBmpBlueLeft)
      delete m_pBmpBlueLeft;
   if(m_pBmpTopMiddle)
      delete m_pBmpBlueMiddle;
   if(m_pBmpTopRight)
      delete m_pBmpBlueRight;

   if(m_pBmpWhiteLeft)
      delete m_pBmpWhiteLeft;
   if(m_pBmpWhiteMiddle)
      delete m_pBmpWhiteMiddle;
   if(m_pBmpWhiteRight)
      delete m_pBmpWhiteRight;

   if(m_pBmpLightBlueLeft)
      delete m_pBmpLightBlueLeft;
   if(m_pBmpLightBlueMiddle)
      delete m_pBmpLightBlueMiddle;
   if(m_pBmpLightBlueRight)
      delete m_pBmpLightBlueRight;

   if(m_pBmpFootLeft)
      delete m_pBmpFootLeft;
   if(m_pBmpFootMiddle)
      delete m_pBmpFootMiddle;
   if(m_pBmpFootRight)
      delete m_pBmpFootRight;

   /*for (int i = 0; i < m_arBtnRecentFiles.GetSize(); ++i)
   {
      if (m_arBtnRecentFiles[i] != NULL)
         delete m_arBtnRecentFiles[i];
   }*/
   //m_arBtnRecentFiles.RemoveAll();

   CMDTARGET_RELEASE(m_ctrlToolTipsContext);
}
// CStartupView-Meldungshandler

int CStartupView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CScrollView::OnCreate(lpCreateStruct) == -1)
      return -1;
   CMainFrameA* pMainFrame = CMainFrameA::GetCurrentInstance();
   if(pMainFrame!=NULL)
   {
     m_pImageManager = pMainFrame->GetCommandBars()->GetImageManager();
   }
   WCHAR *pFontName = _T("Segoe UI");
   LOGFONT logFontBtn;
   XTPPaintManager()->GetRegularFont()->GetLogFont(&logFontBtn);
   //WCHAR *pFontName = _T("Segoe UI");
   _tcscpy(logFontBtn.lfFaceName, pFontName);
   //logFontBtn.lfUnderline = true;
   logFontBtn.lfHeight = 12 + 2;
   m_btnFont.CreateFontIndirect(&logFontBtn);

   LOGFONT logFontBtn14;
   XTPPaintManager()->GetRegularFont()->GetLogFont(&logFontBtn14);
   //WCHAR *pFontName = _T("Segoe UI");
   _tcscpy(logFontBtn14.lfFaceName, pFontName);
   logFontBtn14.lfWeight = FW_BOLD;
   logFontBtn14.lfHeight = 14 + 2;
   m_btnFont14.CreateFontIndirect(&logFontBtn14);

   LOGFONT logFontBtn12;
   XTPPaintManager()->GetRegularFont()->GetLogFont(&logFontBtn12);
   //WCHAR *pFontName = _T("Segoe UI");
   _tcscpy(logFontBtn12.lfFaceName, pFontName);
   logFontBtn12.lfWeight = FW_BOLD;
   logFontBtn12.lfHeight = 12 + 2;
   m_btnFont12.CreateFontIndirect(&logFontBtn12);



   LOGFONT logFontWebBtn;
   XTPPaintManager()->GetRegularFont()->GetLogFont(&logFontWebBtn);
   //WCHAR *pFontName = _T("Segoe UI");
   _tcscpy(logFontWebBtn.lfFaceName, pFontName);
   logFontWebBtn.lfItalic = true;
   logFontWebBtn.lfWeight = FW_BOLD;
   logFontWebBtn.lfHeight = 12 + 2;
   
   m_webBtnFont.CreateFontIndirect(&logFontWebBtn);
   
   // Calculate width and height of tab title region
   LOGFONT logFont;
   ASSISTANT::FillLogFont(logFont, m_csTabFontFamily, m_iTabFontSize, 
      _T("bold"), _T("roman"), false);
   m_iTabWidth = DrawSdk::Text::GetTextWidth(m_csStartPage, m_csStartPage.GetLength(), 
      &logFont) + 3;
   m_iTabHeight = DrawSdk::Text::GetTextHeight(m_csStartPage, m_csStartPage.GetLength(), 
      &logFont);

   LoadBitmaps();

   CreateCloseButton();
   CreateWelcomeButtons();
   //CreateRecentlyOpenedButtons();
   CreateShowAgainButton();
   CreateToolTips();

   SetScrollSizes( MM_TEXT, CSize(VIEW_DEFAULT_WIDTH, VIEW_DEFAULT_HEIGHT) );

   RepositionButtons();

   ShowToolTips();

   return 0;
}

// CStartupView-Zeichnung

void CStartupView::OnDraw(CDC* pDC)
{
   CDocument* pDoc = GetDocument();

    // Get the client rect.
    CRect rcClient;
    CSize szSize = GetTotalSize();
#ifdef _DEBUG_CODE
    int iScrollPosV = GetScrollPos(SB_VERT);
    int iScrollPosH = GetScrollPos(SB_HORZ);
    TRACE1("\nSize.x = %d",szSize.cx);
    TRACE1("\nSize.y = %d",szSize.cy);
    TRACE1("\nScrollPosV = %d",iScrollPosV);
    TRACE1("\nScrollPosH = %d",iScrollPosH);
#endif
    // Paint to a memory device context to help
    // eliminate screen flicker.
    rcClient.SetRect(0,0,szSize.cx,szSize.cy);
    CXTMemDC *pMemDC = new CXTMemDC(pDC, rcClient, RGB(255, 255, 255));

   Gdiplus::Graphics graphics(pMemDC->GetSafeHdc());
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

   // Different regions within the tab   
   DrawBackground(graphics);
   DrawTitleText(graphics);
   //DrawSeparator(graphics);
   //Draw

   DrawLecturnityLogo(graphics);
   //DrawImcLogo(graphics);

   delete pMemDC;

   if(m_bToolTipShow == false)
   {
      UpdateTransparentButtons();
   }
}


// CStartupView-Diagnose

#ifdef _DEBUG
void CStartupView::AssertValid() const
{
   CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CStartupView::Dump(CDumpContext& dc) const
{
   CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CStartupView-Meldungshandler

void CStartupView::OnSize(UINT nType, int cx, int cy)
{
   BOOL bHBar, bVBar;
   CheckScrollBars(bHBar, bVBar);
   CScrollView::OnSize(nType, cx, cy);

   int iNewWidth = cx < VIEW_DEFAULT_WIDTH ? VIEW_DEFAULT_WIDTH : cx;
   int iNewHeight = cy < VIEW_DEFAULT_HEIGHT ? VIEW_DEFAULT_HEIGHT : cy;
   
   int nVOffset = 0;
   int nHOffset = 0;
   DWORD dwStyle = AfxGetApp()->GetMainWnd()->GetStyle();
   CRect rcMainWnd;
   AfxGetApp()->GetMainWnd()->GetWindowRect(&rcMainWnd);
   if((dwStyle & WS_MAXIMIZE) != 0 || m_bIsRetFromEditMode)
   {
      nVOffset =  bVBar? 17: 0;
      nHOffset =  bHBar? 17: 0;
   }
   CSize siTotal = GetTotalSize();
   CRect rcClient(0, 0, siTotal.cx, siTotal.cy);

   if (!m_bIsRetFromEditMode || ! m_rcLastMainWndRect.EqualRect(&rcMainWnd)) {
       SetScrollSizes(MM_TEXT, CSize(iNewWidth + nHOffset, iNewHeight + nVOffset));
   } else {
       SetScrollSizes(MM_TEXT, m_szLastSize);
   }

   RepositionButtons();
   UpdateBars();
}


//void CStartupView::UpdateRecentFileList()
//{
//   /*for (int i = 0; i < m_arBtnRecentFiles.GetSize(); ++i)
//   {
//      if (m_arBtnRecentFiles[i])
//      {
//         m_arBtnRecentFiles[i]->ShowWindow(SW_HIDE);
//         delete m_arBtnRecentFiles[i];
//      }
//   }
//   m_arBtnRecentFiles.RemoveAll();
//
//   CreateRecentlyOpenedButtons();*/
//   RepositionButtons();
//}

void CStartupView::CreateWelcomeButtons()
{ 
   BOOL bCreationSucces=FALSE;
   CMainFrameA* pMainFrame = CMainFrameA::GetCurrentInstance();;
   CRect rcButton;
   rcButton.SetRect(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT);

   CString csTitle;

   // Record PowerPoint button
   /*CString csPrefix = _T("");
   csPrefix.LoadString(IDS_RECORD);*/
   csTitle.LoadString(IDS_POWERPOINT);
   //csTitle = csPrefix + _T(" ") + csTitle;
   if (csTitle.IsEmpty() == false) {
      bCreationSucces = m_btnRecordPowerPoint.Create(csTitle, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
         rcButton, this, IDC_RECORDPOWERPOINT);
   }
   if (bCreationSucces==TRUE) {
       SetButtonImage(&m_btnRecordPowerPoint, IDR_RECORD_PPT, CSize(32,32));
       SetButtonStyle(&m_btnRecordPowerPoint, xtThemeOffice2003, BS_XT_XPFLAT, &m_btnFont14, RGB(255, 255, 255));
   }

   // Record Other button
   csTitle.LoadString(IDS_OTHERAPP);
   //csTitle = csPrefix + _T(" ") + csTitle;
   bCreationSucces = m_btnRecordOther.Create(csTitle, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
      rcButton, this, ID_START_RECORDING_STARTUP_PAGE);
   if (bCreationSucces == TRUE) {
       SetButtonImage(&m_btnRecordOther,IDR_RECORD_OTHER, CSize(32,32));
       SetButtonStyle(&m_btnRecordOther, xtThemeOffice2003, BS_XT_XPFLAT, &m_btnFont14, RGB(255, 255, 255));
   }

   // "New Source Document" button
   csTitle.LoadString(IDS_NEW_LSD);
   bCreationSucces = m_btnNewSourceDoc.Create(csTitle, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
      rcButton, this, ID_LSD_NEW);
   if (bCreationSucces == TRUE) {
       SetButtonImage(&m_btnNewSourceDoc,IDR_NEW_LSD, CSize(32, 32));
       SetButtonStyle(&m_btnNewSourceDoc, xtThemeOffice2003, BS_XT_XPFLAT, &m_btnFont14, RGB(255, 255, 255));
   }

   // "Open Source Document" button
   csTitle.LoadString(IDS_OPEN_LSD);
   bCreationSucces = m_btnOpenSourceDoc.Create(csTitle, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
       rcButton, this, ID_LSD_OPEN);
   if (bCreationSucces == TRUE) {
       SetButtonImage(&m_btnOpenSourceDoc,IDR_OPEN_LSD, CSize(32,32));
       SetButtonStyle(&m_btnOpenSourceDoc, xtThemeOffice2003, BS_XT_XPFLAT, &m_btnFont14, RGB(255, 255, 255));
   }

   // "Import PowerPoint" button
   csTitle.LoadString(IDS_IMPORT_PPT);
   bCreationSucces = m_btnImportPowerPoint.Create(csTitle, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
       rcButton, this, ID_PPT_IMPORT);
   if (bCreationSucces == TRUE) {
       SetButtonImage(&m_btnImportPowerPoint,IDR_IMPORT_PPT, CSize(32, 32));
       SetButtonStyle(&m_btnImportPowerPoint, xtThemeOffice2003, BS_XT_XPFLAT, &m_btnFont14, RGB(255, 255, 255));
   }

   CRect rcNewButtons;
   rcNewButtons.SetRect(0, 0, BUTTON_WIDTH1, BUTTON_HEIGHT1);
   // Tutorials button 
   csTitle.LoadString(IDS_BUTTON_TUTORIALS);
   bCreationSucces = m_btnTutorial.Create(csTitle, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
      rcButton, this, IDS_SHOW_IMC_WEBPAGE_TUTORIALS);
   if(bCreationSucces)
   {
       SetButtonImage(&m_btnTutorial, IDR_PNG_TUTORIALS, CSize(32, 32));
       SetButtonStyle(&m_btnTutorial, xtThemeOffice2003, BS_XT_XPFLAT /*| BS_XT_TRANSPARENT*/, &m_webBtnFont, RGB(233, 243, 249));
       COLORREF clrWebButtons = 0x00333333;
       m_btnTutorial.SetColorText(clrWebButtons);
   }

   csTitle.LoadString(IDS_BUTTON_GUIDED);
   bCreationSucces = m_btnGuidedTour.Create(csTitle, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
      rcButton, this, IDS_SHOW_IMC_WEBPAGE_GUIDED_TOUR);
   if(bCreationSucces)
   {
       m_btnGuidedTour.SetTheme(xtThemeOffice2003);
       SetButtonImage(&m_btnGuidedTour, IDR_PNG_GUIDEDTOUR, CSize(32, 32));
       SetButtonStyle(&m_btnGuidedTour, xtThemeOffice2003, BS_XT_XPFLAT /*| BS_XT_TRANSPARENT*/, &m_webBtnFont, RGB(233, 243, 249));
       COLORREF clrWebButtons = 0x00333333;
       m_btnGuidedTour.SetColorText(clrWebButtons);
   }

   csTitle.LoadString(IDS_BUTTON_COMMUNITY);
   bCreationSucces = m_btnCommunity.Create(csTitle, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
      rcButton, this, IDS_SHOW_IMC_WEBPAGE_COMUNITY);
   if(bCreationSucces)
   {
       SetButtonImage(&m_btnCommunity, IDR_PNG_COMMUNITY, CSize(32, 32));
       COLORREF clrWebButtons = 0x00333333;
       m_btnCommunity.SetColorText(clrWebButtons);
       SetButtonStyle(&m_btnCommunity, xtThemeOffice2003, BS_XT_XPFLAT /*| BS_XT_TRANSPARENT*/, &m_webBtnFont, RGB(233, 243, 249));
       
   }


   CString csOwnVersion;
   csOwnVersion.LoadString(IDS_VERSION);
   CSecurityUtils secure;
   int iVersionType = secure.GetVersionType(csOwnVersion);
   bool bIsEvaluation = (iVersionType & EVALUATION_VERSION) != 0;

   if (bIsEvaluation) {
       csTitle.LoadString(IDS_BUYNOW);
       bCreationSucces = m_btnBuyNow.Create(csTitle, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON /*| BS_MULTILINE*/, 
           CRect(0,0, 190, 60), this, IDS_SHOW_IMC_WEBPAGE_BUYNOW);
       if (bCreationSucces) {
           SetButtonImage(&m_btnBuyNow, IDR_PNG_BUYNOW, CSize(54, 54));
           SetButtonStyle(&m_btnBuyNow, xtThemeOffice2003, BS_XT_XPFLAT, &m_webBtnFont, RGB(190, 217, 239));
           m_btnBuyNow.SetFontEx(&m_webBtnFont); 
       }
   }
}

//void CStartupView::CreateRecentlyOpenedButtons()
//{
//  /* CRecentFileList *pRecentFileList = NULL;
//
//   CWinApp *pApp = AfxGetApp();
//   if (pApp && pApp->IsKindOf(RUNTIME_CLASS(CAssistantApp)))
//   {
//      pRecentFileList = ((CAssistantApp *)pApp)->GetRecentFileList();
//   }
//
//   if (!pRecentFileList)
//      return;
//
//   CRect rcButton;
//   rcButton.SetRect(0, 0, 170, 30);
//   for (int i = 0; i < pRecentFileList->m_nSize; ++i)
//   {
//      CString csPath = pRecentFileList->m_arrNames[i];
//      if (csPath.GetLength() == 0)
//         break;
//
//      ASSISTANT::GetName(csPath);
//      CXTButton *pNewButton = new CXTButton();
//      pNewButton->Create(csPath, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, rcButton, this, ID_FILE_MRU_FILE1 + i);
//      SetButtonImage(pNewButton, IDR_NEW_PROJECT);
//      pNewButton->SetXButtonStyle( BS_XT_XPFLAT | BS_XT_TRANSPARENT);
//      pNewButton->SetFontEx(&m_btnFont);  
//      m_arBtnRecentFiles.Add(pNewButton);
//      m_arcsRecentFiles.Add(csPath);
//      UINT uiRecentFiles[] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,ID_FILE_MRU_FILE1+ i}; 
//      m_pImageManager->SetIcons(IDB_SYSTEM_MENU, uiRecentFiles, _countof(uiRecentFiles), CSize(32, 32));
//
//   }*/
//}

void CStartupView::CreateCloseButton()
{
   CRect rcButton;
   rcButton.SetRect(0, 0, 10, 10);
   m_btnClose.Create(_T("Close Startup page"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, rcButton, this, IDC_CLOSESTARTPAGE);
   SetButtonImage(&m_btnClose,IDR_CLOSE_STARTUP_PAGE, CSize(10,10));
   SetButtonStyle(&m_btnClose, xtThemeOffice2003, BS_XT_XPFLAT | BS_XT_TRANSPARENT, &m_webBtnFont, RGB(233, 243, 249));
   //SetButtonStyle(&m_btnClose, xtThemeOffice2003, BS_XT_XPFLAT, &m_webBtnFont, RGB(190, 217, 239));
   
   m_btnClose.SetImageGap(0);
   m_btnClose.SetTheme(xtThemeOffice2003);
}

void CStartupView::CreateShowAgainButton()
{
   CRect rcButton;
   rcButton.SetRect(0, 0, 280, 30);
   CString csTitle;
   csTitle.LoadString(ID_VIEW_SHOW_STARTUP_PAGE);
   m_btnShowAgain.Create(csTitle, WS_CHILD | WS_VISIBLE | BS_CHECKBOX, rcButton, this, IDC_CHECK_STARTPAGE);
   m_btnShowAgain.SetFlatStyle(TRUE);
   m_btnShowAgain.SetFont(&m_btnFont);
   m_btnShowAgain.SetTheme(xtpButtonThemeStandard);
   m_btnShowAgain.SetTransparent(TRUE);
   m_btnShowAgain.SetTextAlignment(BS_CENTER);

#ifndef _STUDIO
   CWinApp *pApp = AfxGetApp();
   if (pApp && pApp->IsKindOf(RUNTIME_CLASS(CAssistantApp)))
   {
      bool bShowStartupPage = ((CAssistantApp *)pApp)->GetShowStartupPage();
      m_btnShowAgain.SetChecked(bShowStartupPage);
   }
#endif
}

void CStartupView::CreateToolTips()
{
   
   // Set tooltip context
  // EnableToolTips(TRUE);
   if(m_ctrlToolTipsContext !=NULL)
   {
    ASSERT(m_ctrlToolTipsContext);
    return;
   }
   m_ctrlToolTipsContext = new CXTPToolTipContext();
   m_ctrlToolTipsContext->SetModuleToolTipContext();
   m_ctrlToolTipsContext->SetStyle(xtpToolTipOffice2007);
   m_ctrlToolTipsContext->SetMaxTipWidth(200);
   m_ctrlToolTipsContext->ShowTitleAndDescription();       
   m_ctrlToolTipsContext->ShowImage(TRUE,0);
   m_ctrlToolTipsContext->SetMargin(CRect(2, 2, 2, 2));  
   m_ctrlToolTipsContext->SetDelayTime(TTDT_INITIAL, 900);
   m_ctrlToolTipsContext->ShowToolTipAlways();

   CRecentFileList *pRecentFileList = NULL;
#ifndef _STUDIO
   CWinApp *pApp = AfxGetApp();
   if (pApp && pApp->IsKindOf(RUNTIME_CLASS(CAssistantApp)))
   {
      pRecentFileList = ((CAssistantApp *)pApp)->GetRecentFileList();
   }
#endif
   if (!pRecentFileList)
      return;
   for (int i = 0; i < pRecentFileList->m_nSize; ++i)
   {
      UINT uiRecentFiles[] = { -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1,-1,-1,ID_FILE_MRU_FILE1+ i}; 
      m_pImageManager->SetIcons(IDB_SYSTEM_MENU, uiRecentFiles, _countof(uiRecentFiles), CSize(32, 32));

   }

   UpdateData(TRUE);
}
void CStartupView::DrawBackground(Gdiplus::Graphics &graphics)
{
   CSize siTotal = GetTotalSize();
   CRect rcClient(0, 0, siTotal.cx, siTotal.cy);
   CRect rc;
   GetClientRect(&rc);

  /* int iLeft = 6;
   int iRight = 13;
   int iMiddle = rcClient.Width() - iLeft - iRight;*/
   Gdiplus::Rect gdipRcClient(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height());
   Gdiplus::SolidBrush bgBrush(Gdiplus::Color(255, 238, 238, 238));
   graphics.FillRectangle(&bgBrush, gdipRcClient);

   Gdiplus::REAL offsetY = (Gdiplus::REAL)OFFSET_Y;
   Gdiplus::REAL y = offsetY;
   float fHeight = (Gdiplus::REAL)TOP_HEIGHT;
   Gdiplus::RectF gdipRcLeft;
   Gdiplus::RectF gdipRcMiddle;
   Gdiplus::RectF gdipRcRight;

   
   CalculateBkgRects(gdipRcLeft, gdipRcMiddle, gdipRcRight, y, fHeight);

   
   graphics.DrawImage(m_pBmpTopLeft, gdipRcLeft);
   graphics.DrawImage(m_pBmpTopMiddle, gdipRcMiddle, 0.0, 0.0, 1.0, (Gdiplus::REAL)fHeight, Gdiplus::UnitPixel);
   graphics.DrawImage(m_pBmpTopRight, gdipRcRight);

   fHeight = (Gdiplus::REAL)BLUE_HEIGHT;
   CalculateBkgRects(gdipRcLeft, gdipRcMiddle, gdipRcRight, y, fHeight);
   graphics.DrawImage(m_pBmpBlueLeft, gdipRcLeft);
   graphics.DrawImage(m_pBmpBlueMiddle, gdipRcMiddle, 0.0, 0.0, 1.0, (Gdiplus::REAL)fHeight, Gdiplus::UnitPixel);
   graphics.DrawImage(m_pBmpBlueRight, gdipRcRight);

   fHeight =(Gdiplus::REAL)( rcClient.Height()  - OFFSET_Y  - TOP_HEIGHT - BLUE_HEIGHT - LIGHT_BLUE_HEIGHT - FOOT_HEIGHT); 
   CalculateBkgRects(gdipRcLeft, gdipRcMiddle, gdipRcRight, y, fHeight);
   graphics.DrawImage(m_pBmpWhiteLeft, gdipRcLeft/*, 0.0, 0.0, 1.0, (Gdiplus::REAL)296, Gdiplus::UnitPixel*/);
   graphics.DrawImage(m_pBmpWhiteMiddle, gdipRcMiddle, 0.0, 0.0, 1.0, (Gdiplus::REAL)296, Gdiplus::UnitPixel);
   graphics.DrawImage(m_pBmpWhiteRight, gdipRcRight/*, 0.0, 0.0, 1.0, (Gdiplus::REAL)296, Gdiplus::UnitPixel*/);

   fHeight = (Gdiplus::REAL)LIGHT_BLUE_HEIGHT;
   y--;
   CalculateBkgRects(gdipRcLeft, gdipRcMiddle, gdipRcRight, y , fHeight);
   graphics.DrawImage(m_pBmpLightBlueLeft, gdipRcLeft);
   graphics.DrawImage(m_pBmpLightBlueMiddle, gdipRcMiddle, 0.0, 0.0, 1.0, (Gdiplus::REAL)fHeight, Gdiplus::UnitPixel);
   graphics.DrawImage(m_pBmpLightBlueRight, gdipRcRight);

   fHeight = (Gdiplus::REAL)FOOT_HEIGHT;
   CalculateBkgRects(gdipRcLeft, gdipRcMiddle, gdipRcRight, y, fHeight);
   graphics.DrawImage(m_pBmpFootLeft, gdipRcLeft);
   graphics.DrawImage(m_pBmpFootMiddle, gdipRcMiddle, 0.0, 0.0, 1.0, (Gdiplus::REAL)fHeight, Gdiplus::UnitPixel);
   graphics.DrawImage(m_pBmpFootRight, gdipRcRight);

   Gdiplus::Color colorFrom[] = {Gdiplus::Color(255, 220, 223, 224),
      Gdiplus::Color(255, 215, 215, 215),
      Gdiplus::Color(255, 216, 216, 216),
      Gdiplus::Color(255, 219, 219, 219),
      Gdiplus::Color(255, 223, 223, 223),
      Gdiplus::Color(255, 226, 226, 226),
      Gdiplus::Color(255, 229, 229, 229),
      Gdiplus::Color(255, 231, 231, 231),
   };
   Gdiplus::Color colorTo[] = {Gdiplus::Color(255, 198, 199, 201),
      Gdiplus::Color(255, 185, 185, 185),
      Gdiplus::Color(255, 190, 190, 190),
      Gdiplus::Color(255, 197, 197, 197),
      Gdiplus::Color(255, 203, 203, 203),
      Gdiplus::Color(255, 213, 213, 213),
      Gdiplus::Color(255, 218, 218, 218),
      Gdiplus::Color(255, 226, 226, 226),
   };
   Gdiplus::RectF gdipRcM2;
   for(int i = 0; i < 8; i++)
   {
      gdipRcM2.X = gdipRcMiddle.X + gdipRcMiddle.Width - 30.0;
      gdipRcM2.Y = gdipRcMiddle.Y + gdipRcMiddle.Height - 8 + i;
      gdipRcM2.Width = 30.0;
      gdipRcM2.Height = 1.0;

      Gdiplus::LinearGradientBrush gradientBrush(gdipRcM2, 
      colorFrom[i], 
      colorTo[i], 
      Gdiplus::LinearGradientModeHorizontal);
      graphics.FillRectangle(&gradientBrush, gdipRcM2);
   }
}

void CStartupView::DrawTitleText(Gdiplus::Graphics &graphics)
{
   Gdiplus::Rect gdipRcLeftButtons;
   CalculateLeftButtonsRegion(gdipRcLeftButtons);

   Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 0, 0, 0));

   Gdiplus::Font titleFont(m_csFontFamily, m_iTitleFontSize, 
      Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
   Gdiplus::Font subTitleFont(m_csFontFamily, m_iNormalFontSize, 
      Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
   CString csRecord1;
   csRecord1.LoadString(IDS_RECORD);
   INT x = gdipRcLeftButtons.X;
   INT y = gdipRcLeftButtons.Y + 2*BUTTON_TITLE_HEIGHT;
   Gdiplus::PointF textOrigin(x/*gdipRcLeftButtons.X*/, y/*(gdipRcLeftButtons.Y + gdipRcLeftButtons.Height)*/ );
   graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
   graphics.DrawString(csRecord1, csRecord1.GetLength(), &titleFont, textOrigin, &blackBrush);
   
   x = gdipRcLeftButtons.X;
   y = gdipRcLeftButtons.Y + BUTTON_TITLE_HEIGHT + 2*BUTTON_SUBTITLE_HEIGHT + 2*(BUTTON_HEIGHT1 + 10) + SEPARATOR_HEIGHT + 5;
   //Gdiplus::PointF textOrigin(x/*gdipRcLeftButtons.X*/, y/*(gdipRcLeftButtons.Y + gdipRcLeftButtons.Height)*/ );
   textOrigin.X = x;
   textOrigin.Y = y;

   CString csWelcome;
   //csWelcome.LoadStringW(IDS_WELCOME);
   csWelcome.LoadStringW(ID_MENU_NEW);
   graphics.DrawString(csWelcome, csWelcome.GetLength(), &titleFont, textOrigin, &blackBrush); 

   textOrigin.Y += BUTTON_TITLE_HEIGHT;
   

   Gdiplus::Rect gdipRcRightButtons;
   CalculateRightButtonsRegion(gdipRcRightButtons);

   textOrigin.X = gdipRcRightButtons.X ;
   textOrigin.Y = gdipRcRightButtons.Y + BUTTON_TITLE_HEIGHT + BUTTON_HEIGHT;;
   CString csRecord;
   //csRecord.LoadStringW(IDS_RECORD);
   csRecord.LoadStringW(ID_MENU_OPEN);
   graphics.DrawString(csRecord, csRecord.GetLength(), &titleFont, textOrigin, &blackBrush); 

   /*textOrigin.X = gdipRcRightButtons.X ;
   textOrigin.Y += 3*BUTTON_HEIGHT;

   CString csRecent;
   csRecent.LoadStringW(IDS_RECENTLY_OPENED_PROJECTS);
   graphics.DrawString(csRecent, csRecent.GetLength(), &titleFont, textOrigin, &blackBrush); */

}

void CStartupView::DrawSeparator(Gdiplus::Graphics &graphics)
{
   Gdiplus::Rect gdipRcLogoRegion;
   Gdiplus::Rect gdipRcButtonRegion;
   Gdiplus::Rect gdipRcBottomRegion;
   CalculateRegions(gdipRcLogoRegion, gdipRcButtonRegion, gdipRcBottomRegion);

   Gdiplus::Pen greyPen(Gdiplus::Color(255, 190, 190, 190), 1);

   // horizontal separator on the left side
   Gdiplus::Rect gdipRcLeftButtons;
   CalculateLeftButtonsRegion(gdipRcLeftButtons);
   int x1 = gdipRcLeftButtons.X + SEPARATOR_SPACE;
   int y1 = gdipRcLeftButtons.Y + BUTTON_TITLE_HEIGHT + BUTTON_SUBTITLE_HEIGHT + 
      2 * BUTTON_HEIGHT + SEPARATOR_HEIGHT/2.0;
   int x2 = gdipRcLeftButtons.X + gdipRcLeftButtons.Width - SEPARATOR_SPACE;
   int y2 = y1;
   graphics.DrawLine(&greyPen, x1, y1, x2, y2); 

   // vertical separator in the middle of the page
   x1 = gdipRcButtonRegion.X + gdipRcButtonRegion.Width / 2.0;
   y1 = gdipRcLeftButtons.Y + SEPARATOR_SPACE;
   x2 = x1;
   y2 = gdipRcLeftButtons.Y + gdipRcLeftButtons.Height - SEPARATOR_SPACE;
   graphics.DrawLine(&greyPen, x1, y1, x2, y2); 
}

void CStartupView::DrawLecturnityLogo(Gdiplus::Graphics &graphics)
{
   if (m_pLecturnityLogo == NULL)
      return;

   Gdiplus::Rect gdipRcLogoRegion;
   Gdiplus::Rect gdipRcButtonRegion;
   Gdiplus::Rect gdipRcBottomRegion;
   CalculateRegions(gdipRcLogoRegion, gdipRcButtonRegion, gdipRcBottomRegion);

   // Calculate correct position
   Gdiplus::REAL dLogoX = (Gdiplus::REAL)20;/*(gdipRcLogoRegion.X + gdipRcLogoRegion.Width) - 
      (m_pLecturnityLogo->GetWidth() + LOGO_RIGHT_SPACE);*/
   Gdiplus::REAL dLogoY = (Gdiplus::REAL) OFFSET_Y + ( TOP_HEIGHT - m_pLecturnityLogo->GetHeight()) / 2.0;/*gdipRcLogoRegion.Y + 
      (int)((double)(gdipRcLogoRegion.Height - m_pLecturnityLogo->GetHeight()) / 2.0);*/

   graphics.DrawImage(m_pLecturnityLogo, dLogoX, dLogoY);
}

void CStartupView::DrawImcLogo(Gdiplus::Graphics &graphics)
{
   if (m_pImcLogo == NULL)
      return;

   Gdiplus::Rect gdipRcLogoRegion;
   Gdiplus::Rect gdipRcButtonRegion;
   Gdiplus::Rect gdipRcBottomRegion;
   CalculateRegions(gdipRcLogoRegion, gdipRcButtonRegion, gdipRcBottomRegion);

   // Calculate correct position
   Gdiplus::REAL dLogoX = (gdipRcBottomRegion.X + gdipRcBottomRegion.Width) - 
      (m_pImcLogo->GetWidth() + LOGO_RIGHT_SPACE);
   Gdiplus::REAL dLogoY = gdipRcBottomRegion.Y + 
      (int)((double)(gdipRcBottomRegion.Height - m_pImcLogo->GetHeight()) / 2.0);

   graphics.DrawImage(m_pImcLogo, dLogoX, dLogoY);
}

void CStartupView::UpdateTransparentButtons()
{
    if (m_btnBuyNow.m_hWnd != NULL)
        m_btnBuyNow.UpdateBackground();
   m_btnShowAgain.UpdateBackground();
   m_btnClose.UpdateBackground();
   m_btnTutorial.UpdateBackground();
   m_btnGuidedTour.UpdateBackground();
   m_btnCommunity.UpdateBackground();
}

void CStartupView::InvalidateAllButtons(BOOL bErase)
{
    
   m_btnRecordPowerPoint.Invalidate(bErase);
   m_btnRecordOther.Invalidate(bErase);
   
  /* m_btnNewProject.Invalidate(bErase);
   m_btnOpenProject.Invalidate(bErase);*/
    
   m_btnNewSourceDoc.Invalidate(bErase);
   m_btnOpenSourceDoc.Invalidate(bErase);
   m_btnImportPowerPoint.Invalidate(bErase);

   m_btnTutorial.Invalidate(bErase);
   m_btnGuidedTour.Invalidate(bErase);
   m_btnCommunity.Invalidate(bErase);
   if (m_btnBuyNow.m_hWnd != NULL)
       m_btnBuyNow.Invalidate(bErase);

  /* for (int i = 0; i < m_arBtnRecentFiles.GetSize(); ++i)
      m_arBtnRecentFiles[i]->Invalidate(bErase);*/

   m_btnShowAgain.Invalidate(bErase);
   
}

void CStartupView::RepositionButtons()
{
   int x;
   int y;
   int width;
   int height;

   Gdiplus::Rect gdipRcLogoRegion;
   Gdiplus::Rect gdipRcButtonRegion;
   Gdiplus::Rect gdipRcBottomRegion;
   CalculateRegions(gdipRcLogoRegion, gdipRcButtonRegion, gdipRcBottomRegion);

   width = BUTTON_WIDTH;
   height = BUTTON_HEIGHT1;


   // Close button
   CRect rcButton;
   rcButton.SetRect(gdipRcLogoRegion.Width - 19 , 
      OFFSET_Y, 
      gdipRcLogoRegion.Width - 7 , 
      13 + OFFSET_Y );
   m_btnClose.SetWindowPos(NULL, rcButton.left- GetScrollPos(SB_HORZ), rcButton.top- GetScrollPos(SB_VERT), 
      rcButton.Width(), rcButton.Height(), SWP_NOZORDER);
   m_btnClose.SetTextAndImagePos(CPoint(1,1), CPoint(16, 16));

   // Other buttons

   // Left buttons
   Gdiplus::Rect gdipRcLeftButtons;
   CalculateLeftButtonsRegion(gdipRcLeftButtons);

   x = gdipRcLeftButtons.X;

   y = gdipRcLeftButtons.Y + BUTTON_TITLE_HEIGHT + BUTTON_SUBTITLE_HEIGHT + BUTTON_TOP_SPACE ;
   m_btnRecordPowerPoint.SetWindowPos(NULL, x- GetScrollPos(SB_HORZ), y- GetScrollPos(SB_VERT), width, height, SWP_NOZORDER);
   m_btnRecordPowerPoint.SetTextAndImagePos(CPoint(ICON_X_POS - 1, ICON_Y_POS + 1), CPoint(TEXT_X_POS + 8, TEXT_Y_POS + 7));
 
   y += BUTTON_HEIGHT1+10 ;

   m_btnRecordOther.SetWindowPos(NULL, x- GetScrollPos(SB_HORZ), y- GetScrollPos(SB_VERT), width, height, SWP_NOZORDER);
   m_btnRecordOther.SetTextAndImagePos(CPoint(ICON_X_POS - 1, ICON_Y_POS + 1), CPoint(TEXT_X_POS + 8, TEXT_Y_POS + 7));
   y += BUTTON_HEIGHT1+10 ;

   //y += SEPARATOR_HEIGHT;

   y += BUTTON_HEIGHT1+3 ;//+ BUTTON_TITLE_HEIGHT;

   m_btnNewSourceDoc.SetWindowPos(NULL, x- GetScrollPos(SB_HORZ), y - GetScrollPos(SB_VERT), width, height, SWP_NOZORDER);
   m_btnNewSourceDoc.SetTextAndImagePos(CPoint(ICON_X_POS - 1, ICON_Y_POS + 1), CPoint(TEXT_X_POS + 10, TEXT_Y_POS + 7));
   y += BUTTON_HEIGHT1+10;

   

   m_btnImportPowerPoint.SetWindowPos(NULL, x - GetScrollPos(SB_HORZ), y- GetScrollPos(SB_VERT), width, height, SWP_NOZORDER);
   m_btnImportPowerPoint.SetTextAndImagePos(CPoint(ICON_X_POS - 1, ICON_Y_POS + 1), CPoint(TEXT_X_POS + 10, TEXT_Y_POS + 7));

   // Right buttons
   Gdiplus::Rect gdipRcRightButtons;
   CalculateRightButtonsRegion(gdipRcRightButtons);

   

   x = gdipRcRightButtons.X ;
   y = gdipRcRightButtons.Y + 2 * BUTTON_TITLE_HEIGHT + BUTTON_HEIGHT ;
   //height = BUTTON_HEIGHT;
   m_btnOpenSourceDoc.SetWindowPos(NULL, x- GetScrollPos(SB_HORZ), y- GetScrollPos(SB_VERT), width, height, SWP_NOZORDER);
   m_btnOpenSourceDoc.SetTextAndImagePos(CPoint(ICON_X_POS - 1, ICON_Y_POS - 1), CPoint(TEXT_X_POS + 10, TEXT_Y_POS + 7));
   
   y += 3*BUTTON_HEIGHT+3;
   /*for (int i = 0; i < m_arBtnRecentFiles.GetSize(); ++i)
   {
      y = gdipRcRightButtons.Y + BUTTON_TITLE_HEIGHT + i*(BUTTON_HEIGHT+5);
      m_arBtnRecentFiles[i]->SetWindowPos(NULL, x, y, width, height, SWP_NOZORDER);
      m_arBtnRecentFiles[i]->SetTextAndImagePos(CPoint(ICON_X_POS, ICON_Y_POS), CPoint(TEXT_X_POS, TEXT_Y_POS));
   }*/

   CSize siTotal = GetTotalSize();
   CRect rcClient(0, 0, siTotal.cx, siTotal.cy);  
   //Button Tutorial
   x = rcClient.left + 40 ;//gdipRcBottomRegion.X + 40;
   y = rcClient.bottom - FOOT_HEIGHT - LIGHT_BLUE_HEIGHT ;/*gdipRcBottomRegion.Y + 
      (gdipRcBottomRegion.Height - rcBtnShowAgain.Height()) / 2.0;*/
   m_btnTutorial.SetWindowPos(NULL, x- GetScrollPos(SB_HORZ), y- GetScrollPos(SB_VERT), BUTTON_WIDTH1, BUTTON_HEIGHT1, SWP_NOZORDER);
   m_btnTutorial.SetTextAndImagePos(CPoint(ICON_X_POS - 1, ICON_Y_POS + 1), CPoint(TEXT_X_POS + 10, TEXT_Y_POS + 9));

   //Button Guided Tour
   x = (rcClient.left + rcClient.Width() - BUTTON_WIDTH1) / 2;//gdipRcBottomRegion.X + 40;
   y = rcClient.bottom - FOOT_HEIGHT - LIGHT_BLUE_HEIGHT ;/*gdipRcBottomRegion.Y + 
      (gdipRcBottomRegion.Height - rcBtnShowAgain.Height()) / 2.0;*/
   m_btnGuidedTour.SetWindowPos(NULL, x - GetScrollPos(SB_HORZ), y- GetScrollPos(SB_VERT), BUTTON_WIDTH1, BUTTON_HEIGHT1, SWP_NOZORDER);
   m_btnGuidedTour.SetTextAndImagePos(CPoint(ICON_X_POS - 1, ICON_Y_POS + 1), CPoint(TEXT_X_POS + 10, TEXT_Y_POS + 9));

   //Button Community
   x = rcClient.right - 40 - 13 - BUTTON_WIDTH1;//gdipRcBottomRegion.X + 40;
   y = rcClient.bottom - FOOT_HEIGHT - LIGHT_BLUE_HEIGHT ;/*gdipRcBottomRegion.Y + 
      (gdipRcBottomRegion.Height - rcBtnShowAgain.Height()) / 2.0;*/
   m_btnCommunity.SetWindowPos(NULL, x- GetScrollPos(SB_HORZ), y- GetScrollPos(SB_VERT), BUTTON_WIDTH1, BUTTON_HEIGHT1, SWP_NOZORDER);
   m_btnCommunity.SetTextAndImagePos(CPoint(ICON_X_POS - 1, ICON_Y_POS + 1), CPoint(TEXT_X_POS + 10, TEXT_Y_POS + 9));
  
   if (m_btnBuyNow.m_hWnd != NULL) {
       // //Button Buy Now
       x = rcClient.left + m_pLecturnityLogo->GetWidth() + 10 ;//gdipRcBottomRegion.X + 40;
       y = rcClient.top + OFFSET_Y + 4 ;/*gdipRcBottomRegion.Y + 
                                        (gdipRcBottomRegion.Height - rcBtnShowAgain.Height()) / 2.0;*/
       m_btnBuyNow.SetWindowPos(NULL, x- GetScrollPos(SB_HORZ), y- GetScrollPos(SB_VERT), 120, 56, SWP_NOZORDER);
       m_btnBuyNow.SetTextAndImagePos(CPoint( 1, 1), CPoint(56, TEXT_Y_POS + 17));
   }
   
   // Place checkbox 40 pixel from right border (horizontally)
   // in the middle of the bottom region (vertically)
   CRect rcBtnShowAgain;
   m_btnShowAgain.GetClientRect(rcBtnShowAgain);
   
   
   //CRect rcClient(0, 0, siTotal.cx, siTotal.cy);   
   x = rcClient.left + 40 ;//gdipRcBottomRegion.X + 40;
   y = rcClient.bottom - FOOT_HEIGHT;/*gdipRcBottomRegion.Y + 
      (gdipRcBottomRegion.Height - rcBtnShowAgain.Height()) / 2.0;*/
   m_btnShowAgain.SetWindowPos(NULL, x- GetScrollPos(SB_HORZ), y - GetScrollPos(SB_VERT), 250, BUTTON_HEIGHT, SWP_NOZORDER);
   m_btnShowAgain.SetTextAlignment(SS_LEFT);
}


void CStartupView::CalculateRegions(Gdiplus::Rect &gdipRcLogoRegion, 
                                    Gdiplus::Rect &gdipRcButtonRegion, 
                                    Gdiplus::Rect &gdipRcBottomRegion)
{
   CSize siTotal = GetTotalSize();
   CRect rcTab(0, 0, siTotal.cx, siTotal.cy);

   double dHeightUnit = (double)(rcTab.Height()-TOP_SPACE-m_iTabHeight) / 35.0;

   gdipRcLogoRegion.X = rcTab.left;
   gdipRcLogoRegion.Y = rcTab.top + TOP_SPACE + m_iTabHeight;
   gdipRcLogoRegion.Width = rcTab.Width();
   gdipRcLogoRegion.Height = (int)(dHeightUnit * 5);

   gdipRcButtonRegion.X = rcTab.left;
   gdipRcButtonRegion.Y = gdipRcLogoRegion.Y + gdipRcLogoRegion.Height;
   gdipRcButtonRegion.Width = rcTab.Width();
   gdipRcButtonRegion.Height = (int)(dHeightUnit * 24);

   gdipRcBottomRegion.X = rcTab.left;
   gdipRcBottomRegion.Y = gdipRcButtonRegion.Y + gdipRcButtonRegion.Height;
   gdipRcBottomRegion.Width = rcTab.Width();
   gdipRcBottomRegion.Height = (int)(dHeightUnit * 6);
}

void CStartupView::CalculateLeftButtonsRegion(Gdiplus::Rect &gdipRcLeftButtons)
{
   Gdiplus::Rect gdipRcLogoRegion;
   Gdiplus::Rect gdipRcButtonRegion;
   Gdiplus::Rect gdipRcBottomRegion;
   CalculateRegions(gdipRcLogoRegion, gdipRcButtonRegion, gdipRcBottomRegion);

   // Middle of the left part of the button region
   gdipRcLeftButtons.X = gdipRcButtonRegion.X + 
      (int)(((gdipRcButtonRegion.Width / 2.0) - BUTTON_WIDTH) / 2.0);
   gdipRcLeftButtons.Y = gdipRcButtonRegion.Y; //+ 
      //(int)(((gdipRcButtonRegion.Height - m_iButtonRegionHeight) * 2.0) / 3.0);
   gdipRcLeftButtons.Width = BUTTON_WIDTH;
   gdipRcLeftButtons.Height = m_iButtonRegionHeight;

}

void CStartupView::CalculateRightButtonsRegion(Gdiplus::Rect &gdipRightRcButtons)
{
   Gdiplus::Rect gdipRcLogoRegion;
   Gdiplus::Rect gdipRcButtonRegion;
   Gdiplus::Rect gdipRcBottomRegion;
   CalculateRegions(gdipRcLogoRegion, gdipRcButtonRegion, gdipRcBottomRegion);

   // Middle of the left part of the button region
   gdipRightRcButtons.X = gdipRcButtonRegion.X + 
      (int)(((3*gdipRcButtonRegion.Width / 2.0) - BUTTON_WIDTH) / 2.0);
   gdipRightRcButtons.Y = gdipRcButtonRegion.Y ;//+ 
      //(int)(((gdipRcButtonRegion.Height - m_iButtonRegionHeight) * 2.0) / 3.0);
   gdipRightRcButtons.Width = BUTTON_WIDTH;
   gdipRightRcButtons.Height = m_iButtonRegionHeight;
}

void CStartupView::LoadBitmaps()
{
   CDC *pDC = GetDC();

   CPalette *pPalette = NULL;
   pPalette = pDC->GetCurrentPalette();

   HBITMAP hBitmap;

   // Load LECTURNITY Logo
   CSecurityUtils securityUtils;
   CString csOwnVersion;
   csOwnVersion.LoadString(IDS_VERSION);
   int nVersionType = securityUtils.GetVersionType(csOwnVersion);

   UINT nID = IDB_LECTURNITYLOGO;
   if (nVersionType != FULL_VERSION)
      nID = IDB_SP_LOGO_TEST;
   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(nID/*IDB_SP_LOGO_TEST*//*IDB_LECTURNITYLOGO*/));
   m_pLecturnityLogo = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);

   // Load IMC Logo
   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_IMCLOGO));
   m_pImcLogo = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);

   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_TOP_LEFT));
   m_pBmpTopLeft = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);
   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_TOP_MIDDLE));
   m_pBmpTopMiddle = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);
   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_TOP_RIGHT));
   m_pBmpTopRight = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);

   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_BLUE_LINE_LEFT));
   m_pBmpBlueLeft = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);
   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_BLUE_LINE_MIDDLE));
   m_pBmpBlueMiddle = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);
   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_BLUE_LINE_RIGHT));
   m_pBmpBlueRight = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);

   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_WHITE_LEFT));
   m_pBmpWhiteLeft = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);
   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_WHITE_MIDDLE));
   m_pBmpWhiteMiddle = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);
   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_WHITE_RIGHT));
   m_pBmpWhiteRight = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);

   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_LIGHT_BLUE_LEFT));
   m_pBmpLightBlueLeft = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);
   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_LIGHT_BLUE_MIDDLE));
   m_pBmpLightBlueMiddle = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);
   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_LIGHT_BLUE_RIGHT));
   m_pBmpLightBlueRight = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);

   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_FOOT_LEFT));
   m_pBmpFootLeft = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);
   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_FOOT_MIDDLE));
   m_pBmpFootMiddle = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);
   hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SP_FOOT_RIGHT));
   m_pBmpFootRight = Gdiplus::Bitmap::FromHBITMAP(hBitmap, (HPALETTE)pPalette->GetSafeHandle());
   ::DeleteObject(hBitmap);

   ReleaseDC(pDC);
}

BOOL CStartupView::OnCommand(WPARAM wParam, LPARAM lParam)
{
   WORD nCmdID = LOWORD(wParam);

   // These commands are part of the CMainFrame class
   if (nCmdID == ID_FILE_NEW ||
      nCmdID == ID_FILE_OPEN ||
      nCmdID == ID_LSD_NEW ||
      nCmdID == ID_LSD_OPEN ||
      nCmdID == ID_PPT_IMPORT ||
      nCmdID == ID_START_RECORDING_STARTUP_PAGE ||
      nCmdID == IDC_RECORDPOWERPOINT ||
      nCmdID == IDS_SHOW_IMC_WEBPAGE_TUTORIALS ||
      nCmdID == IDS_SHOW_IMC_WEBPAGE_GUIDED_TOUR ||
      nCmdID == IDS_SHOW_IMC_WEBPAGE_COMUNITY ||
      nCmdID == IDS_SHOW_IMC_WEBPAGE_BUYNOW) {
            CMainFrameA *pMainWnd = CMainFrameA::GetCurrentInstance();
            if (pMainWnd && pMainWnd->OnCommand(wParam, lParam)) {

                if (nCmdID == ID_START_RECORDING_STARTUP_PAGE || nCmdID == ID_FILE_NEW)
                    pMainWnd->CloseStartPage();

                return TRUE; 
            }
   } 

    if (nCmdID >= ID_FILE_MRU_FILE1 && nCmdID < ID_FILE_MRU_FILE1 + _AFX_MRU_COUNT)
    {
        CMainFrameA *pMainWnd = CMainFrameA::GetCurrentInstance();
        if (pMainWnd && pMainWnd->OnCommand(wParam, lParam)) 
        {

                return TRUE; 
        }
    }
   return CScrollView::OnCommand(wParam, lParam);
}

void CStartupView::OnCheckStartpage() 
{	
    CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();

    bool bSetChecked = false;
    if (pMainFrame != NULL)
        bSetChecked = pMainFrame->GetShowStartupPage();

    if (pMainFrame != NULL)
        pMainFrame->SetShowStartupPage(!bSetChecked);
}

LRESULT CStartupView::OnUpdateButtons(WPARAM wParam,LPARAM lParam)
{	
    CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
    BOOL bSetChecked = FALSE;
    if (pMainFrame)
        bSetChecked = pMainFrame->GetShowStartupPage();

    if (m_btnShowAgain.GetChecked() != bSetChecked)
    {
        m_btnShowAgain.SetChecked(bSetChecked);
        m_btnShowAgain.RedrawWindow();
    }

    return 0L;
}

BOOL CStartupView::PreTranslateMessage(MSG* pMsg)
{
   if(m_ctrlToolTipsContext!=NULL)
   {
      m_ctrlToolTipsContext->FilterToolTipMessage(this, pMsg);
      m_ctrlToolTipsContext->RelayToolTipMessage(this,pMsg);
   } 
   return CScrollView::PreTranslateMessage(pMsg);
}

BOOL CStartupView::SetButtonImage(CXTButton *btnButton, UINT uiID, CSize size) 
{
   CSize sz;
   CRect dimensions;
   btnButton->GetClientRect(dimensions);


   CXTPImageManagerIconHandle iconHandle;
   CXTPImageManagerIcon* pIcon; 
   // load icon
   iconHandle = pIcon->LoadBitmapFromResource(MAKEINTRESOURCE(uiID), NULL);
   pIcon = new CXTPImageManagerIcon(0, iconHandle.GetExtent().cx, iconHandle.GetExtent().cy);
      pIcon->SetNormalIcon(iconHandle);
      CMDTARGET_RELEASE(pIcon);

   sz.cx = size.cx;
   sz.cy = size.cy;
   // set icon to the button
   BOOL bIconSet = btnButton->SetIcon(sz,iconHandle,iconHandle);
   
   UINT uIcon[] = {(UINT)btnButton->GetDlgCtrlID()};
   m_pImageManager->SetIcons(uiID, uIcon, _countof(uIcon), size);

   return bIconSet;
}

void CStartupView::SetButtonStyle(CXTButton *btnButton, XTThemeStyle xtThemeStyle, DWORD dwxStyle, CFont *pFont, COLORREF clrBackground)
{
    btnButton->SetTheme(xtThemeStyle);
    btnButton->SetXButtonStyle(dwxStyle);
    btnButton->SetFontEx(pFont);
    btnButton->SetColorFace(clrBackground);
}

void CStartupView::RedrawButtons() 
{
   InvalidateAllButtons(TRUE);
}
CXTPToolTipContext* CStartupView::GetToolTipContext()
{
   return m_ctrlToolTipsContext;
}
void CStartupView::ShowToolTips() 
{
   CMainFrameA* pMainFrame = CMainFrameA::GetCurrentInstance();
   GetToolTipContext()->SetModuleToolTipContext();
   GetToolTipContext()->SetFont(pMainFrame->GetCommandBars()->GetPaintManager()->GetIconFont());
   GetToolTipContext()->ShowTitleAndDescription();
}

BOOL CStartupView::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
   ASSERT(pNMHDR->code == TTN_NEEDTEXT);

   TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;

   if (!(pTTT->uFlags & TTF_IDISHWND))
      return FALSE;

   UINT_PTR hWnd = pNMHDR->idFrom;
   // idFrom is actually the HWND of the tool
   UINT nID = (UINT)(WORD)::GetDlgCtrlID((HWND)hWnd);
   INT_PTR nHit = (UINT)(WORD)::GetDlgCtrlID((HWND)hWnd);
   pTTT->lpszText = MAKEINTRESOURCE(nID);
   pTTT->hinst = AfxGetInstanceHandle();

   *pResult = 0;
   // bring the tooltip window above other popup windows
   ::SetWindowPos(pNMHDR->hwndFrom,  HWND_TOP, 0, 0, 0, 0,
      SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
   m_bToolTipShow = true;
   return TRUE;    // message was handled
}
void  CStartupView::DrawStartupPage()
{
   Gdiplus::Graphics graphics(GetDC()->m_hDC);
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);

   // Different regions within the tab   
   DrawBackground(graphics);
   DrawTitleText(graphics);
   //DrawSeparator(graphics);
   //Draw

   DrawLecturnityLogo(graphics);
   //DrawImcLogo(graphics);
   //InvalidateAllButtons(TRUE);
}
INT_PTR CStartupView::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
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
      switch(nHit)
      {
      case ID_PPT_IMPORT: 
         {
            csTitle = GetToolTipByID(IDS_IMPORT_PPT);
            csDescription = GetToolTipByID(IDS_IMPORT_PPT_DESCRIPTION); break;
         }
      case ID_START_RECORDING_STARTUP_PAGE:
         {
            csTitle = GetToolTipByID(IDS_OTHERAPP);
            csDescription = GetToolTipByID(IDS_TOOLTIP_RECORDOTHER); break;
         }
      case IDR_RECORD_OTHER: csDescription = GetToolTipByID(IDS_TOOLTIP_RECORDPPT); break;
      case ID_LSD_NEW:
         { 
            
            csTitle = GetToolTipByID(IDS_NEW_LSD);
            csDescription = GetToolTipByID(IDS_NEW_LSD_DESCRIPTION); 
            break;
         }
      case IDC_RECORDPOWERPOINT:
         { 
            csTitle = GetToolTipByID(IDS_RECORD)+ _T(" ") + GetToolTipByID(IDS_POWERPOINT);
            csDescription = GetToolTipByID(IDS_RECORD_POWERPOINT_DESCRIPTION); 
            break;
         }
      case ID_LSD_OPEN:
         { 
            csTitle = GetToolTipByID(IDS_OPEN_LSD);
            csDescription = GetToolTipByID(IDS_OPEN_LSD_DESCRIPTION); 
            break;
         }
      case IDS_SHOW_IMC_WEBPAGE_TUTORIALS: 
         {
            csTitle = GetToolTipByID(IDS_BUTTON_TUTORIALS);
            csDescription = GetToolTipByID(IDS_BUTTON_TUTORIALS_DESCRIPTION); break;
         }
      case IDS_SHOW_IMC_WEBPAGE_GUIDED_TOUR: 
         {
            csTitle = GetToolTipByID(IDS_BUTTON_GUIDED);
            csDescription = GetToolTipByID(IDS_BUTTON_GUIDED_DESCRIPTION); break;
         }
      case IDS_SHOW_IMC_WEBPAGE_COMUNITY: 
         {
            csTitle = GetToolTipByID(IDS_BUTTON_COMMUNITY);
            csDescription = GetToolTipByID(IDS_BUTTON_COMMUNITY_DESCRIPTION); break;
         }
      case IDS_SHOW_IMC_WEBPAGE_BUYNOW: 
         {
            csTitle = GetToolTipByID(IDS_BUYNOW);
            csDescription = GetToolTipByID(IDS_BUTTON_BUYNOW_DESCRIPTION); break;
         }
      default: csDescription = GetToolTipByID(nHit); break;
      }

      CXTPToolTipContext::FillInToolInfo(pTI, GetSafeHwnd(), rcItem, nHit, csTitle, csTitle, csDescription, m_pImageManager);

     // InvalidateAllButtons(true);
      return nHit;
   }
   return -1;  // not found
}
CString CStartupView::GetToolTipByID(UINT uID) const 
{
   CString strTipText;
    // don't handle the message if no string resource found
	if (XTPResourceManager()->LoadString(&strTipText, uID))
		return strTipText;

   return _T("Title");
}
void CStartupView::OnCloseStartpage() 
{
   CMainFrameA* pMainFrame =  CMainFrameA::GetCurrentInstance();
   pMainFrame->CloseStartPage();
}

CXTPImageManager * CStartupView::GetImageManager()
{
   return m_pImageManager;
}

void CStartupView::CalculateBkgRects(Gdiplus::RectF &gdipRcLeft, 
      Gdiplus::RectF &gdipRcMiddle, 
      Gdiplus::RectF &gdipRcRight,
      Gdiplus::REAL &y, float fHeight)
{
   CSize siTotal = GetTotalSize();
   CRect rcClient(0, 0, siTotal.cx, siTotal.cy);

   int iLeft = 6;
   int iLeftOffset = 4;
   int iRight = 13;
   int iMiddle = rcClient.Width() - iLeft - iRight;
   
   gdipRcLeft.X = rcClient.left + iLeftOffset;
   gdipRcLeft.Y = rcClient.top + y;
   gdipRcLeft.Width = iLeft;
   gdipRcLeft.Height = fHeight;

   gdipRcMiddle.X = rcClient.left + iLeft + iLeftOffset;
   gdipRcMiddle.Y = rcClient.top + y;
   gdipRcMiddle.Width = iMiddle;
   gdipRcMiddle.Height = fHeight;

   gdipRcRight.X = rcClient.left + iLeft + iMiddle + iLeftOffset;
   gdipRcRight.Y = rcClient.top + y;
   gdipRcRight.Width = iRight;
   gdipRcRight.Height = fHeight;

   y += fHeight;
}

BOOL CStartupView::OnEraseBkgnd(CDC* pDC)
{
    // Erasing the background leads to flickering
   //return CScrollView::OnEraseBkgnd(pDC);
   /*CBrush br( GetSysColor( COLOR_WINDOW ) ); 
    FillOutsideRect( pDC, &br );*/

    return TRUE;
}


//BOOL CStartupView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
//{
//   // TODO: Add your specialized code here and/or call the base class
//
//   //InvalidateAllButtons(true);
//   UpdateTransparentButtons();
//  /* CSize siTotal = GetTotalSize();
//   CRect rcClient(0, 0, siTotal.cx, siTotal.cy);
//   CRect rc;
//   GetClientRect(&rc);
//   CRect rectRedraw;
//   rectRedraw.SetRect(0, rc.bottom- 50, rcClient.Width(), rc.bottom);
//   RedrawWindow(&rectRedraw);*/
//   return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
//   /*RedrawWindow();
//   return TRUE;*/
//}

void CStartupView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
    // TODO: Add your specialized code here and/or call the base class
    if (bActivate == FALSE) {
        m_szLastSize = GetTotalSize();
        AfxGetApp()->GetMainWnd()->GetWindowRect(&m_rcLastMainWndRect);
    }
    CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
