#pragma once

#include "CheckButton.h"
// CStartupView-Ansicht
class CStartupView : public CScrollView
{ 
   DECLARE_DYNAMIC(CStartupView)

public:

   CStartupView();           // Dynamische Erstellung verwendet geschützten Konstruktor
   virtual ~CStartupView();
   void SetReturnFromEditMode(bool bRet){m_bIsRetFromEditMode = bRet;};

public:

   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
   INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

   virtual void OnDraw(CDC* pDC);      // Überladen, um diese Ansicht darzustellen
   virtual BOOL PreTranslateMessage(MSG* pMsg);
#ifdef _DEBUG
   virtual void AssertValid() const;
#ifndef _WIN32_WCE
   virtual void Dump(CDumpContext& dc) const;
#endif
#endif

  // void UpdateRecentFileList();

private:
   void CreateWelcomeButtons();
  // void CreateRecentlyOpenedButtons();
   void CreateCloseButton();
   void CreateShowAgainButton();
   void CreateToolTips();
   void DrawBackground(Gdiplus::Graphics &graphics);   
   void DrawTitleText(Gdiplus::Graphics &graphics);
   void DrawSeparator(Gdiplus::Graphics &graphics);
   void DrawLecturnityLogo(Gdiplus::Graphics &graphics);
   void DrawImcLogo(Gdiplus::Graphics &graphics);
   void UpdateTransparentButtons();
   void RepositionButtons();

   void CalculateRegions(Gdiplus::Rect &gdipRcLogoRegion, 
      Gdiplus::Rect &gdipRcButtonRegion, 
      Gdiplus::Rect &gdipRcBottomRegion);
    void CalculateBkgRects(Gdiplus::RectF &gdipRcLeft, 
      Gdiplus::RectF &gdipRcMiddle, 
      Gdiplus::RectF &gdipRcRight,
      Gdiplus::REAL &y, float fHeight);

   void CalculateLeftButtonsRegion(Gdiplus::Rect &gdipRcLeftButtons);
   void CalculateRightButtonsRegion(Gdiplus::Rect &gdipRcRightButtons);

   void LoadBitmaps();
   BOOL SetButtonImage(CXTButton *btnButton, UINT uiID, CSize size= CSize(16, 16));
   void SetButtonStyle(CXTButton *btnButton, XTThemeStyle xtThemeStyle, DWORD dwxStyle, CFont *pFont, COLORREF clrBackground);
   void SetButtonsFlatStyle() ;
   void SetButtonsTheme(UINT uiFlatStyle) ;
   void RedrawButtons() ;
   CXTPToolTipContext* GetToolTipContext();
   void ShowToolTips();
   void InvalidateAllButtons(BOOL bErase);
   void DrawStartupPage();
   CXTPImageManager * CStartupView::GetImageManager();
   //Gdiplus::Image* LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance =NULL);
   CString GetToolTipByID(UINT uID) const ;
protected:
   DECLARE_MESSAGE_MAP()

   virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
   void OnCheckStartpage() ;
   afx_msg LRESULT OnUpdateButtons(WPARAM wParam,LPARAM lParam);
   afx_msg void OnTimer(UINT nIDEvent);
   afx_msg void OnCloseStartpage() ;

private:

   enum 
   {
      TOP_SPACE = 3,
      BOTTOM_SPACE = 30,
      BUTTON_WIDTH = 260/*190*/,
      BUTTON_HEIGHT = 25,
      BUTTON_WIDTH1 = 160,
      BUTTON_HEIGHT1 = 40,
      BUTTON_LEFT_SPACE = 40,
      BUTTON_TOP_SPACE = 40,
      SEPARATOR_HEIGHT = 30,
      SEPARATOR_SPACE = 10,
      BUTTON_TITLE_HEIGHT = 30,
      BUTTON_SUBTITLE_HEIGHT = 20,
      LOGO_RIGHT_SPACE = 40,
      ICON_X_POS = 5,
      ICON_Y_POS = 3,
      TEXT_X_POS = 35,
      TEXT_Y_POS = 5,
      VIEW_DEFAULT_WIDTH = 620,
      VIEW_DEFAULT_HEIGHT = 540,
      OFFSET_Y = 10,
      TOP_HEIGHT = 67,
      BLUE_HEIGHT = 9,
      LIGHT_BLUE_HEIGHT = 42,
      FOOT_HEIGHT = 34
   };

   CString m_csTabFontFamily;
   int m_iTabFontSize;
   CString m_csFontFamily;
   int m_iTitleFontSize;
   int m_iNormalFontSize;

   CString m_csStartPage;
   int m_iTabWidth;
   int m_iTabHeight;
   Gdiplus::Bitmap *m_pLecturnityLogo;
   Gdiplus::Bitmap *m_pImcLogo;

   Gdiplus::Bitmap *m_pBmpTopLeft;
   Gdiplus::Bitmap *m_pBmpTopMiddle;
   Gdiplus::Bitmap *m_pBmpTopRight;

   Gdiplus::Bitmap *m_pBmpBlueLeft;
   Gdiplus::Bitmap *m_pBmpBlueMiddle;
   Gdiplus::Bitmap *m_pBmpBlueRight;

   Gdiplus::Bitmap *m_pBmpWhiteLeft;
   Gdiplus::Bitmap *m_pBmpWhiteMiddle;
   Gdiplus::Bitmap *m_pBmpWhiteRight;

   Gdiplus::Bitmap *m_pBmpLightBlueLeft;
   Gdiplus::Bitmap *m_pBmpLightBlueMiddle;
   Gdiplus::Bitmap *m_pBmpLightBlueRight;

   Gdiplus::Bitmap *m_pBmpFootLeft;
   Gdiplus::Bitmap *m_pBmpFootMiddle;
   Gdiplus::Bitmap *m_pBmpFootRight;



   int m_iButtonRegionHeight;

   CFont m_btnFont;
   CFont m_btnFont14;
   CFont m_btnFont12;
   CFont m_webBtnFont;

   CXTButton m_btnClose;
   CXTButton m_btnRecordPowerPoint;
   CXTButton m_btnRecordOther;
   //CXTButton m_btnNewProject;
   //CXTButton m_btnOpenProject;
   CXTButton m_btnNewSourceDoc;
   CXTButton m_btnOpenSourceDoc;
   CXTButton m_btnImportPowerPoint;
   CXTPToolTipContext *m_ctrlToolTipsContext;
   CToolTipCtrl m_ctrlToolTips;
   CXTPButton m_btnShowAgain;
   CXTPImageManager * m_pImageManager;
   //CArray<CXTButton *, CXTButton *> m_arBtnRecentFiles;
   CArray<CString, CString> m_arcsRecentFiles;
   int		m_nImageAlignment;
   int		m_nTextAlignment;
   int     m_nTheme;
   BOOL    m_bUseVisualStyle;
   BOOL    m_bScreenTips;
   bool    m_bToolTipShow;

   CXTButton m_btnTutorial;
   CXTButton m_btnGuidedTour;
   CXTButton m_btnCommunity;
   CXTButton m_btnBuyNow;
   bool m_bIsRetFromEditMode;
   CSize m_szLastSize;
   CRect m_rcLastMainWndRect;

public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
//   virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
protected:
    virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
};


