// MainFrm.h : Schnittstelle der Klasse CMainFrame
//


#pragma once

class CMainFrame : public CXTPFrameWnd
{
	
protected: // Nur aus Serialisierung erstellen
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attribute
public:

// Operationen
public:

// Überschreibungen
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementierung
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // Eingebundene Elemente der Steuerleiste
	ULONG_PTR m_gdiplusToken;
	CStatusBar  m_wndStatusBar;
	CXTPToolBar* m_pFontAttributesBar;
	CXTPToolBar* m_pToolAttributesBar;
	CXTPToolBar* m_pPresentationBar;
	CHARFORMAT	m_cf;
	COLORREF	m_clrLine;
	bool		m_bLineColor;
	COLORREF	m_clrFill;
	bool		m_bFillColor;
	bool		m_bRecording;
	bool		m_bRecordingPaused;


// Generierte Funktionen für die Meldungstabellen
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
	afx_msg void OnCustomize();
	afx_msg int OnCreateCommandBar(LPCREATEBARSTRUCT lpCreatePopup);
	DECLARE_MESSAGE_MAP()

// Toolbar functions
	
protected:
	void InsertNewPage();
	void SetTool();
	void UndoLastAction();
	
	afx_msg void OnChangeLineColor();
	afx_msg void OnUpdateLineColor(CCmdUI* pCmd);
	afx_msg void OnNoLineColor();
	afx_msg void OnMoreLineColors();
	afx_msg void OnSelectLineColor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateSelectLineColor(CCmdUI* pCmd);

	afx_msg void OnChangeFillColor();
	afx_msg void OnUpdateFillColor(CCmdUI* pCmd);
	afx_msg void OnNoFillColor();
	afx_msg void OnMoreFillColors();
	afx_msg void OnSelectFillColor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateSelectFillColor(CCmdUI* pCmd);

	afx_msg void OnChangeLineWidth();
	afx_msg void OnUpdateLineWidth(CCmdUI* pCmd);
	afx_msg void OnMoreLineWidths();
	afx_msg void OnSelectLineWidth(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateSelectLineWidth(CCmdUI* pCmd);

	afx_msg void OnChangeLineStyle();
	afx_msg void OnUpdateLineStyle(CCmdUI* pCmd);
	afx_msg void OnSelectLineStyle(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateSelectLineStyle(CCmdUI* pCmd);

	afx_msg void OnChangeFontName();
	afx_msg void OnUpdateComboFont(CCmdUI* pCmd);
	afx_msg void OnChangeFontSize();
	afx_msg void OnUpdateComboSize(CCmdUI* pCmd);
	afx_msg void OnFontBold();
	afx_msg void OnFontItalic();
	afx_msg void OnFontUnderline();

	afx_msg void OnGoToNextPage();
	afx_msg void OnGoToPreviousPage();
	afx_msg void OnGoToFirstPage();
	afx_msg void OnGoToLastPage();

	afx_msg void OnMarkerYellow();
	afx_msg void OnMarkerOrange();
	afx_msg void OnMarkerMagenta();
	afx_msg void OnMarkerGreen();
	
	afx_msg void OnPenRed();
	afx_msg void OnPenGreen();
	afx_msg void OnPenBlue();
	afx_msg void OnPenBlack();
	afx_msg void OnPenWhite();
	
	afx_msg void OnSimpleNavigation();
	afx_msg void OnTelepointer();
	afx_msg void OnFullScreen();
	afx_msg void OnShowHideStructure();
	afx_msg void OnShowHideToolbar();

	afx_msg void OnStartRecording();
	afx_msg void OnUpdateStartRecording(CCmdUI* pCmd);
	afx_msg void OnPauseRecording();
	afx_msg void OnUpdatePauseRecording(CCmdUI* pCmd);
	afx_msg void OnStopRecording();
	afx_msg void OnUpdateStopRecording(CCmdUI* pCmd);

public:
	afx_msg void OnClose();
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};


