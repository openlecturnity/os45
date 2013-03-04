#pragma once

#include "PresentationBarControls.h"
//#include "AssistantDoc.h"

class CAssistantDoc;
class CAssistantView;

class IPresentationBarListener
{
public:
    IPresentationBarListener() {}
    ~IPresentationBarListener() {}

public:
    virtual void KeyEscPressed() = 0;
    virtual void ButtonPensPressed() = 0;
    virtual void ButtonMarkersPressed() = 0;
    virtual void ButtonTelepointerPressed() = 0;
    virtual void ButtonSimpleNavigationPressed() = 0;
    virtual void ButtonStartPressed() = 0;
    virtual void ButtonStopPressed() = 0;
    virtual void ButtonPausePressed() = 0;
    virtual void ButtonScreenGrabbingPressed() = 0;
    virtual void ButtonScreenPreviousPagePressed() = 0;
    virtual void ButtonScreenNextPagePressed() = 0;
    virtual void ButtonPageFocusedPressed() = 0;
    virtual void ButtonDocumentStructurePressed() = 0;
    virtual void ButtonRecordingsPressed() = 0;
    virtual void ButtonCutPressed() = 0;
    virtual void ButtonCopyPressed() = 0;
    virtual void ButtonPastePressed() = 0;
    virtual void ButtonUndoPressed() = 0;
    virtual void ButtonClickCopyPressed() = 0;
    virtual void ButtonTextPressed() = 0;
    virtual void ButtonLinePressed() = 0;
    virtual void ButtonPolylinePressed() = 0;
    virtual void ButtonFreehandPressed() = 0;
    virtual void ButtonEllipsePressed() = 0;
    virtual void ButtonRectanglePressed() = 0;
    virtual void ButtonPolygonPressed() = 0;
    virtual void ButtonInsertPagePressed() = 0;
};

class CPresentationBar : public CWnd
{
	DECLARE_DYNAMIC(CPresentationBar)

private:
    static UINT StartShowHideAnimation(LPVOID pParam);

public:
	CPresentationBar(CAssistantDoc *pAssistantDoc);
	virtual ~CPresentationBar();
    

protected:
   DECLARE_MESSAGE_MAP()

   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnPaint();
   //afx_msg void OnButtonAction(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnButtonExitFullScreen();
   afx_msg void OnButtonPens();
   afx_msg void OnButtonMarkers();
   afx_msg void OnButtonTelepointer();
   afx_msg void OnButtonMouseNav();
   afx_msg void OnButtonStart();
   afx_msg void OnButtonStop();
   afx_msg void OnButtonPause();
   afx_msg void OnButtonScreenGrabbing();
   afx_msg void OnMarkersSplitButton();
   afx_msg void OnPensSplitButton();
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   //afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   virtual BOOL PreTranslateMessage(MSG* pMsg);

   afx_msg void OnButtonPageFocused();
   afx_msg void OnButtonDocumentStructure();
   afx_msg void OnButtonRecordings();
   afx_msg void OnButtonCut();
   afx_msg void OnButtonCopy();
   afx_msg void OnButtonPaste();
   afx_msg void OnButtonUndo();
   afx_msg void OnButtonClickCopy();
   afx_msg void OnButtonText();
   afx_msg void OnButtonLine();
   afx_msg void OnButtonPolyline();
   afx_msg void OnButtonFreehand();
   afx_msg void OnButtonEllipse();
   afx_msg void OnButtonRectangle();
   afx_msg void OnButtonPolygon();
   afx_msg void OnButtonInsertPage();

   afx_msg void OnButtonPreviousPage();
   afx_msg void OnButtonNextPage();

   afx_msg void OnUpdateButtonScreenGrabbing(CCmdUI* pCmdUI);
   afx_msg void OnUpdateStart(CCmdUI* pCmdUI);
   afx_msg void OnUpdateStop(CCmdUI* pCmdUI);
   afx_msg void OnUpdatePause(CCmdUI* pCmdUI);
   afx_msg void OnUpdateTools(CCmdUI* pCmdUI);
   afx_msg void OnUpdateCutCopy(CCmdUI* pCmdUI);
   afx_msg void OnUpdatePaste(CCmdUI* pCmdUI);
   afx_msg void OnUpdateUndo(CCmdUI* pCmdUI); 
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);

   
   afx_msg void OnUpdateNonSgControls(CCmdUI* pCmdUI);
   afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnDestroy();
   afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
private:
   CPresentationBarControls m_pBtnExitFullScreen;
   CPresentationBarControls m_pBtnPens;
   CPresentationBarControls m_pBtnMarkers;
   CPresentationBarControls m_pBtnTelepointer;
   CPresentationBarControls m_pBtnMouseNav;
   CPresentationBarControls m_pBtnPageFocused;
   CPresentationBarControls m_pBtnDocumentStruct;
   CPresentationBarControls m_pBtnRecordings;
   CPresentationBarControls m_pBtnCut;
   CPresentationBarControls m_pBtnCopy;
   CPresentationBarControls m_pBtnPaste;
   CPresentationBarControls m_pBtnUndo;
   CPresentationBarControls m_pBtnOneClickCopy;
   CPresentationBarControls m_pBtnText;
   CPresentationBarControls m_pBtnLine;
   CPresentationBarControls m_pBtnPolyLine;
   CPresentationBarControls m_pBtnFreeHand;
   CPresentationBarControls m_pBtnEllipse;
   CPresentationBarControls m_pBtnRectangle;
   CPresentationBarControls m_pBtnPolygon;
   CPresentationBarControls m_pBtnInsertPage;

   CPresentationBarControls m_pBtnStart;
   CPresentationBarControls m_pBtnStop;
   CPresentationBarControls m_pBtnPause;
   CPresentationBarControls m_pBtnSg;

   CPresentationBarControls m_pBtnPrevious;
   CPresentationBarControls m_pBtnNext;

private:
   CXTPButton m_pBtnSeparator1;
   CXTPButton m_pBtnSeparator2;
   CXTPButton m_pBtnSeparator3;
   CXTPButton m_pBtnSeparator4;
   

   CXTPCommandBars *m_pCommandBars;
   UINT m_nPosition;
   bool m_bSmallIcons;
   bool m_bIsPauseFlashing;
   bool m_bMouseOver;
   bool m_bShowTransparent;
   int m_nPenIndex;
   int m_nMarkerIndex;
   CRect m_rcStandard;

   CWinThread *m_pAnimateWindowThread;
   bool m_bAnimationFinished;

   CXTPToolTipContext *m_ctrlToolTipsContext;
   CAssistantDoc *m_pAssistantDoc;

   bool m_bIsAutoHide;
   bool m_bIsPreparationMode;
   CWnd *m_pMainFrmWnd;
   bool m_bWindowVisible;

   bool m_bShowExitFullScreen;
   bool m_bShowPencils;
   bool m_bShowMarkers;
   bool m_bShowPointer;
   bool m_bShowMouseNav;
   bool m_bShowStart;
   bool m_bShowStop;
   bool m_bShowPause;
   bool m_bShowScreenGrabbing;
   bool m_bShowPageFocused;
   bool m_bShowDocumentStruct;
   bool m_bShowRecordings;
   bool m_bShowCut;
   bool m_bShowCopy;
   bool m_bShowPaste;
   bool m_bShowUndo;
   bool m_bShowOneClickCopy;
   bool m_bShowText;
   bool m_bShowLine;
   bool m_bShowPolyline;
   bool m_bShowFreeHand;
   bool m_bShowEllipse;
   bool m_bShowRectangle;
   bool m_bShowPolygon;
   bool m_bShowInsertPage;
   bool m_bShowPreviousPage;
   bool m_bShowNextPage;
   bool m_bShowSeparator1;
   bool m_bShowSeparator2;
   bool m_bShowSeparator3;
   bool m_bShowSeparator4;

   IPresentationBarListener *m_pListener;
   HCURSOR m_hNoCursor;

private:
   void CreateButtons();
   void DrawBackground();
   void DrawSeparators(Gdiplus::Graphics &graphics);
   void UpdateControls();
   void ResetAllControls();
   void RefreshButtonsPos(UINT nPos, bool bSmallIcons);
   void LoadIcons();
   void RecalculateButtonsPos(UINT nButtonId, bool bShow, int nOffset);
   void ShowHideSeparators(int &nOffset);

   INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
   BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
   void CreateToolTips();

   CString GetToolTipByID(UINT uID) const ;
   void GetTooltipText(UINT nTextID, CString &csTitle, CString &csDescription) const; 

   void SaveSettings();
   void LoadSettings();
   void InitSettings();

public:
   void SetCommandBars(CXTPCommandBars* pCommandBars);
   CXTPCommandBars* GetCommandBars() {return m_pCommandBars;}
   void SetPosition(UINT nPos);
   UINT GetPosition();
   void SetPauseButtonFlash(UINT nFlashPause);
   void RefreshWindowPos(UINT nPos, bool bSmallIcons);
   void SetPresentationBarPen(int nPenIndex);
   void SetPresentationBarMarker(int nMarkerIndex);
   bool IsTransparent();
   void ShowTransparent(bool bShowTransparent);
   void SetSmallIcons(bool bSmallIcons);
   void ShowHideExitFullScreen(bool bShow);
   void ShowHideAnnotations(UINT nId, bool bShow);
   void ShowHidePresentationTools(UINT nId, bool bShow);
   void ShowHideRecordingTools(UINT nId, bool bShow);
   void ShowHideScreenGrabbing(bool bShow);
   void SetAnimationFinished() {m_bAnimationFinished = true;}
   void SetWindowAnimation();
   void SetAutoHide(bool bAutoHide);
   CRect GetStandardWindowRect();
   void SetMainFrmWnd(CWnd *pMainFrmWnd);
   void SetListener(IPresentationBarListener *pListener) {m_pListener = pListener;}
   void SetPreparationMode(bool bIsPreparationMode);
   bool IsAutoHide(); 
   void UpdateAppearance(); 
   // Will set this window on top just if it is not on top.
   void PutWindowOnTop();  
   void ChangeFlashStatus(bool bIsFlashing);
};