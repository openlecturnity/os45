// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "Assistant.h"

#include "MainFrm.h"

#include "ControlLineWidthSelector.h"
#include "ControlLineStyleSelector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame
#define IDR_LINECOLOR_POPUP 1000
#define IDR_FILLCOLOR_POPUP 1001

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	
	ON_WM_FONTCHANGE()

	ON_COMMAND(ID_PAGE_NEW, CMainFrame::InsertNewPage)
	ON_COMMAND(ID_SELECT_OBJECTS, CMainFrame::SetTool)
	ON_COMMAND(ID_MOVE_OBJECTS, CMainFrame::SetTool)
	ON_COMMAND(ID_COPY_OBJECTS, CMainFrame::SetTool)
	ON_COMMAND(ID_CUT_OBJECTS, CMainFrame::SetTool)
	ON_COMMAND(ID_UNDO, CMainFrame::UndoLastAction)

	ON_COMMAND(ID_TOOL_TEXT, CMainFrame::SetTool)
	ON_COMMAND(ID_TOOL_FREEHAND, CMainFrame::SetTool)
	ON_COMMAND(ID_TOOL_LINE, CMainFrame::SetTool)
	ON_COMMAND(ID_TOOL_RECTANGLE, CMainFrame::SetTool)
	ON_COMMAND(ID_TOOL_OVAL, CMainFrame::SetTool)
	ON_COMMAND(ID_TOOL_POLYLINE, CMainFrame::SetTool)
	ON_COMMAND(ID_TOOL_POLYGON, CMainFrame::SetTool)

	ON_COMMAND(ID_LINE_COLOR, OnChangeLineColor)
	ON_UPDATE_COMMAND_UI(ID_LINE_COLOR, OnUpdateLineColor)
	ON_COMMAND(IDS_NO_LINE_COLOR, OnNoLineColor)
	ON_COMMAND(IDS_MORE_LINE_COLORS, OnMoreLineColors)
	ON_XTP_EXECUTE(ID_SELECTOR_LINE_COLOR, OnSelectLineColor)
	ON_UPDATE_COMMAND_UI(ID_SELECTOR_LINE_COLOR, OnUpdateSelectLineColor)

	ON_COMMAND(ID_FILL_COLOR, OnChangeFillColor)
	ON_UPDATE_COMMAND_UI(ID_FILL_COLOR, OnUpdateFillColor)
	ON_COMMAND(IDS_NO_FILL_COLOR, OnNoFillColor)
	ON_COMMAND(IDS_MORE_FILL_COLORS, OnMoreFillColors)
	ON_XTP_EXECUTE(ID_SELECTOR_FILL_COLOR, OnSelectFillColor)
	ON_UPDATE_COMMAND_UI(ID_SELECTOR_FILL_COLOR, OnUpdateSelectFillColor)
	
	ON_COMMAND(ID_LINE_WIDTH, OnChangeLineWidth)
	ON_UPDATE_COMMAND_UI(ID_LINE_WIDTH, OnUpdateLineWidth)
	ON_COMMAND(IDS_MORE_LINE_WIDTH, OnMoreLineWidths)
	ON_XTP_EXECUTE(ID_SELECTOR_LINE_WIDTH, OnSelectLineWidth)
	ON_UPDATE_COMMAND_UI(ID_SELECTOR_LINE_WIDTH, OnUpdateSelectLineWidth)

	ON_COMMAND(ID_LINE_STYLE, OnChangeLineStyle)
	ON_UPDATE_COMMAND_UI(ID_LINE_STYLE, OnUpdateLineStyle)
	ON_XTP_EXECUTE(ID_SELECTOR_LINE_STYLE, OnSelectLineStyle)
	ON_UPDATE_COMMAND_UI(ID_SELECTOR_LINE_STYLE, OnUpdateSelectLineStyle)

	ON_COMMAND(ID_COMBO_FONT, OnChangeFontName)
	ON_UPDATE_COMMAND_UI(ID_COMBO_FONT, OnUpdateComboFont)
	ON_COMMAND(ID_COMBO_FONTSIZE, OnChangeFontSize)
	ON_UPDATE_COMMAND_UI(ID_COMBO_FONTSIZE, OnUpdateComboSize)
	ON_COMMAND(ID_FONT_BOLD, OnFontBold)
	ON_COMMAND(ID_FONT_ITALIC, OnFontItalic)
	ON_COMMAND(ID_FONT_UNDERLINE, OnFontUnderline)

	ON_COMMAND(ID_NEXT_PAGE, OnGoToNextPage)
	ON_COMMAND(ID_PREVIOUS_PAGE, OnGoToPreviousPage)
	ON_COMMAND(ID_FIRST_PAGE, OnGoToFirstPage)
	ON_COMMAND(ID_LAST_PAGE, OnGoToLastPage)

	ON_COMMAND(ID_MARKER_YELLOW, OnMarkerYellow)
	ON_COMMAND(ID_MARKER_ORANGE, OnMarkerOrange)
	ON_COMMAND(ID_MARKER_MAGENTA, OnMarkerMagenta)
	ON_COMMAND(ID_MARKER_GREEN, OnMarkerGreen)
	ON_COMMAND(ID_PEN_RED, OnPenRed)
	ON_COMMAND(ID_PEN_GREEN, OnPenGreen)
	ON_COMMAND(ID_PEN_BLUE, OnPenBlue)
	ON_COMMAND(ID_PEN_BLACK, OnPenBlack)
	ON_COMMAND(ID_PEN_WHITE, OnPenWhite)
	ON_COMMAND(ID_SIMPLE_NAVIGATION, OnSimpleNavigation)
	ON_COMMAND(ID_TELEPOINTER, OnTelepointer)
	ON_COMMAND(ID_FULL_SCREEN, OnFullScreen)
	ON_COMMAND(ID_SHOWHIDE_STRUCTURE, OnShowHideStructure)
	ON_COMMAND(ID_SHOWHIDE_TOOLBAR, OnShowHideToolbar)
	
	ON_COMMAND(ID_START_RECORDING, OnStartRecording)
	ON_UPDATE_COMMAND_UI(ID_START_RECORDING, OnUpdateStartRecording)
	ON_COMMAND(ID_PAUSE_RECORDING, OnPauseRecording)
	ON_UPDATE_COMMAND_UI(ID_PAUSE_RECORDING, OnUpdatePauseRecording)
	ON_COMMAND(ID_STOP_RECORDING, OnStopRecording)
	ON_UPDATE_COMMAND_UI(ID_STOP_RECORDING, OnUpdateStopRecording)


	ON_XTP_CREATECONTROL()
	ON_XTP_CREATECOMMANDBAR()

	
	ON_COMMAND(XTP_ID_CUSTOMIZE, OnCustomize)

	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame Erstellung/Zerstörung

CMainFrame::CMainFrame()
{
	// TODO: Hier Code für die Memberinitialisierung einfügen

	m_clrLine = RGB(0, 0, 0);
	m_bLineColor = true;
	m_clrFill = RGB(0, 0, 0);
	m_bFillColor = false;

	m_pFontAttributesBar = NULL;
	m_pToolAttributesBar = NULL;
	m_pPresentationBar	 = NULL;

	m_cf.cbSize = sizeof(CHARFORMAT);
	m_cf.dwMask = CFM_FACE | CFM_CHARSET | CFM_SIZE;
	m_cf.dwEffects = 0;
	m_cf.yHeight = 200; // Twips is pixel * 20
	m_cf.yOffset = 0;
	m_cf.crTextColor = 0;
	m_cf.bPitchAndFamily = DEFAULT_PITCH;
	strcpy(m_cf.szFaceName, "Arial");

	m_bRecording = false;
	m_bRecordingPaused = false;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

CMainFrame::~CMainFrame()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	  if (CXTPFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Create Status bar. 
    // Important: All control bars including the Status Bar 
    // must be created before CommandBars....
    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
        sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    // Initialize the command bars
    if (!InitCommandBars())
        return -1;

    // Get a pointer to the command bars object.
    CXTPCommandBars* pCommandBars = GetCommandBars();
    if(pCommandBars == NULL)
    {
        TRACE0("Failed to create command bars object.\n");
        return -1;      // fail to create
    }

    // Add the menu bar
    CXTPCommandBar* pMenuBar = pCommandBars->SetMenu(
        _T("Menu Bar"), IDR_MAINFRAME);       
    if(pMenuBar == NULL)
    {
        TRACE0("Failed to create menu bar.\n");
        return -1;      // fail to create
    }

    // Create ToolBar
    CXTPToolBar* pToolBar = (CXTPToolBar*)
        pCommandBars->Add(_T("Standard"), xtpBarTop);
    if (!pToolBar || !pToolBar->LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;
    }

	m_pToolAttributesBar = (CXTPToolBar*)pCommandBars->Add(_T("Tools"), xtpBarTop);
	if (!m_pToolAttributesBar ||
		!m_pToolAttributesBar->LoadToolBar(IDR_TOOL_ATTRIBUTES))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}
	DockRightOf(m_pToolAttributesBar, pToolBar);
	
	m_pFontAttributesBar = (CXTPToolBar*)pCommandBars->Add(_T("Font"), xtpBarTop);
	if (!m_pFontAttributesBar ||
		!m_pFontAttributesBar->LoadToolBar(IDR_FONT_ATTRIBUTES))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}
	DockRightOf(m_pFontAttributesBar, m_pToolAttributesBar);

	CXTPToolBar *pPageNavigation = (CXTPToolBar*)pCommandBars->Add(_T("Page Navigation"), xtpBarBottom);
	if (!pPageNavigation ||
		!pPageNavigation->LoadToolBar(IDR_PAGE_NAVIGATION))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}
	
	CXTPToolBar *pRecordingControl = (CXTPToolBar*)pCommandBars->Add(_T("Recording"), xtpBarBottom );
	if (!pRecordingControl ||
		!pRecordingControl->LoadToolBar(IDR_RECORDING_CONTROL))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}
	//CXTPControl *pControl = pRecordingControl->GetControl(0);
	//CXTPImageManagerIcon *pImage = pControl->GetImage();
	//HICON  hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LINE_WIDTH));
	//pImage->SetIcon(hIcon);
	DockRightOf(pRecordingControl, pPageNavigation);

	CXTPToolBar *pScreengrabbingBar = (CXTPToolBar*)pCommandBars->Add(_T("ScreenGrabbing"), xtpBarBottom );
	if (!pScreengrabbingBar ||
		!pScreengrabbingBar->LoadToolBar(IDR_SCREENGRABBING))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}
	DockRightOf(pScreengrabbingBar, pRecordingControl);

	CXTPToolBar *pMarkerAndPens = (CXTPToolBar*)pCommandBars->Add(_T("Marker and Pens"), xtpBarBottom );
	if (!pMarkerAndPens ||
		!pMarkerAndPens->LoadToolBar(IDR_MARKERANDPENS))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}
	DockRightOf(pMarkerAndPens, pScreengrabbingBar);


	m_pPresentationBar = (CXTPToolBar*)pCommandBars->Add(_T("Praesentation"), xtpBarBottom );
	if (!m_pPresentationBar ||
		!m_pPresentationBar->LoadToolBar(IDR_PRESENTATION_TOOLS))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}
	DockRightOf(m_pPresentationBar, pMarkerAndPens);


    // Set Office 2003 Theme
    CXTPPaintManager::SetTheme(xtpThemeOffice2003);

	LoadCommandBars(_T("CommandBars"));

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Ändern Sie hier die Fensterklasse oder die Darstellung, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return TRUE;
}


// CMainFrame Diagnose


int CMainFrame::OnCreateCommandBar(LPCREATEBARSTRUCT lpCreatePopup)
{
	if (lpCreatePopup->bTearOffBar && 
		(lpCreatePopup->nID == IDR_LINECOLOR_POPUP || lpCreatePopup->nID == IDR_FILLCOLOR_POPUP))
	{
		CXTPToolBar* pToolBar = DYNAMIC_DOWNCAST(CXTPToolBar, lpCreatePopup->pCommandBar);
		ASSERT(pToolBar);
		
		// Prevent docking.
		pToolBar->EnableDocking(xtpFlagFloating);
		// Prevent customization
		pToolBar->EnableCustomization(FALSE);
		return TRUE;		
	}
	return FALSE;
}

int CMainFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
	if (lpCreateControl->bToolBar)
	{
		CXTPToolBar* pToolBar = DYNAMIC_DOWNCAST(CXTPToolBar, lpCreateControl->pCommandBar);
		if (!pToolBar)
			return FALSE;

		if (lpCreateControl->nID == ID_COMBO_FONT && pToolBar->GetBarID() == IDR_FONT_ATTRIBUTES)
		{
			CXTPControlComboBox* pFontCombo = new CXTPControlFontComboBox();
			pFontCombo->SetDropDownListStyle();

			lpCreateControl->pControl = pFontCombo;
			return TRUE;
		}

		if (lpCreateControl->nID == ID_COMBO_FONTSIZE && pToolBar->GetBarID() == IDR_FONT_ATTRIBUTES)
		{
			CXTPControlComboBox* pComboSize = new CXTPControlSizeComboBox();
			pComboSize->SetDropDownListStyle();
         
			lpCreateControl->pControl = pComboSize;
			return TRUE;
		}
		
		if (lpCreateControl->nID == ID_LINE_COLOR && pToolBar->GetBarID() == IDR_TOOL_ATTRIBUTES)
		{
			CXTPControlPopupColor* pPopupColor = new CXTPControlPopupColor();
			
			CXTPPopupBar* pColorBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();
			
			pColorBar->GetControls()->Add(new CXTPControlButtonColor(), IDS_NO_LINE_COLOR);
			CXTPControlColorSelector *pControlColorSelector = new CXTPControlColorSelector();
			pControlColorSelector->SetBeginGroup(TRUE);
			pColorBar->GetControls()->Add(pControlColorSelector, ID_SELECTOR_LINE_COLOR);
			CXTPControlButtonColor *pControlButtonColor = new CXTPControlButtonColor();
			pControlButtonColor->SetBeginGroup(TRUE);
			pColorBar->GetControls()->Add(pControlButtonColor, IDS_MORE_LINE_COLORS);

			pPopupColor->SetCommandBar(pColorBar);

			pColorBar->SetTearOffPopup(_T("Line Color"), IDR_LINECOLOR_POPUP, 0);
			pColorBar->EnableCustomization(FALSE);

			pColorBar->InternalRelease();

			lpCreateControl->pControl = pPopupColor;
			return TRUE; 
		}
		
		if (lpCreateControl->nID == ID_FILL_COLOR && pToolBar->GetBarID() == IDR_TOOL_ATTRIBUTES)
		{
			CXTPControlPopupColor* pPopupColor = new CXTPControlPopupColor();
			
			CXTPPopupBar* pColorBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();
			
			pColorBar->GetControls()->Add(new CXTPControlButtonColor(), IDS_NO_FILL_COLOR);
			CXTPControlColorSelector *pControlColorSelector = new CXTPControlColorSelector();
			pControlColorSelector->SetBeginGroup(TRUE);
			pColorBar->GetControls()->Add(pControlColorSelector, ID_SELECTOR_FILL_COLOR);
			CXTPControlButtonColor *pControlButtonColor = new CXTPControlButtonColor();
			pControlButtonColor->SetBeginGroup(TRUE);
			pColorBar->GetControls()->Add(pControlButtonColor, IDS_MORE_FILL_COLORS);

			pPopupColor->SetCommandBar(pColorBar);

			pColorBar->SetTearOffPopup(_T("Fill Color"), IDR_FILLCOLOR_POPUP, 0);
			pColorBar->EnableCustomization(FALSE);

			pColorBar->InternalRelease();

			lpCreateControl->pControl = pPopupColor;
			return TRUE;
		}
		

		if (lpCreateControl->nID == ID_LINE_WIDTH && pToolBar->GetBarID() == IDR_TOOL_ATTRIBUTES)
		{
			CXTPControlPopup *pControlPopup = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);
			
			CXTPPopupBar* pLineWidthBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();
			
			CXTPControlButton *pControlButton = new CXTPControlButton();
			pLineWidthBar->GetControls()->Add(pControlButton, IDS_MORE_LINE_WIDTH);
			CControlLineWidthSelector *pControlLineWidthSelector = new CControlLineWidthSelector();
			pControlLineWidthSelector->SetBeginGroup(TRUE);
			pLineWidthBar->GetControls()->Add(pControlLineWidthSelector, ID_SELECTOR_LINE_WIDTH);
			pControlPopup->SetCommandBar(pLineWidthBar);
			
			pLineWidthBar->EnableCustomization(FALSE);
			pLineWidthBar->InternalRelease();

			lpCreateControl->pControl = pControlPopup;

			return TRUE;
		}

		if (lpCreateControl->nID == ID_LINE_STYLE && pToolBar->GetBarID() == IDR_TOOL_ATTRIBUTES)
		{
			CXTPControlPopup *pControlPopup = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);
			
			CXTPPopupBar* pLineStyleBar = (CXTPPopupBar*)CXTPPopupToolBar::CreateObject();
			
			CControlLineStyleSelector *pControlLineStyleSelector = new CControlLineStyleSelector();
			pLineStyleBar->GetControls()->Add(pControlLineStyleSelector, ID_SELECTOR_LINE_STYLE);
			pControlPopup->SetCommandBar(pLineStyleBar);
			
			pLineStyleBar->EnableCustomization(FALSE);
			pLineStyleBar->InternalRelease();

			lpCreateControl->pControl = pControlPopup;

			return TRUE;
		}


	}

	return FALSE;
}

void CMainFrame::OnCustomize()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if (pCommandBars != NULL)
	{
		CXTPCustomizeSheet cs(pCommandBars);

		CXTPCustomizeOptionsPage pageOptions(&cs);
		cs.AddPage(&pageOptions);

		CXTPCustomizeCommandsPage* pCommands = cs.GetCommandsPage();
		pCommands->AddCategories(IDR_MAINFRAME);
		//pCommands->GetControls(_T("File"))->Add(xtpControlButton, ID_FILE_OPEN, _T(""), 1);	

		pCommands->InsertAllCommandsCategory();
		pCommands->InsertBuiltInMenus(IDR_MAINFRAME);
		pCommands->InsertNewMenuCategory();

		cs.DoModal();
	}
}
// Toolbar functions

void CMainFrame::InsertNewPage()
{
}

void CMainFrame::SetTool()
{
}

void CMainFrame::UndoLastAction()
{
}


void CMainFrame::OnChangeFontName()
{
	if (m_pFontAttributesBar)
	{
		CXTPControlFontComboBox* pFontCombo = (CXTPControlFontComboBox*)m_pFontAttributesBar->GetControls()->FindControl(ID_COMBO_FONT);

		if (pFontCombo->GetType() == xtpControlComboBox)
		{
			strcpy(m_cf.szFaceName, pFontCombo->GetText());
		}		
	}
}

void CMainFrame::OnUpdateComboFont(CCmdUI* pCmd)
{
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmd->m_pOther;
	if (pToolBar)
	{	
		CXTPControlFontComboBox* pFontCombo = (CXTPControlFontComboBox*)pToolBar->GetControls()->FindControl(ID_COMBO_FONT);

		if (pFontCombo->GetType() == xtpControlComboBox)
		{
			pFontCombo->SetCharFormat((CHARFORMAT&)m_cf);
		}		
	}
	pCmd->Enable(TRUE);
}

void CMainFrame::OnChangeFontSize()
{
	if (m_pFontAttributesBar)
	{
		CXTPControlSizeComboBox* pFontSizeCombo = (CXTPControlSizeComboBox*)m_pFontAttributesBar->GetControls()->FindControl(ID_COMBO_FONTSIZE);

		if (pFontSizeCombo->GetType() == xtpControlComboBox)
		{
			CString csString = pFontSizeCombo->GetText();
			m_cf.yHeight = atoi(csString) * 20;
		}		
	}
}

void CMainFrame::OnUpdateComboSize(CCmdUI* pCmd)
{
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmd->m_pOther;
	if (pToolBar)
	{	
		CXTPControlSizeComboBox* pSizeCombo = (CXTPControlSizeComboBox*)pToolBar->GetControls()->GetAt(pCmd->m_nIndex);

		if (pSizeCombo->GetType() == xtpControlComboBox)
		{
			pSizeCombo->SetTwipSize( (m_cf.dwMask & CFM_SIZE) ? m_cf.yHeight : -1);
		}		
	}
	pCmd->Enable(TRUE);
}

void CMainFrame::OnFontBold()
{
}

void CMainFrame::OnFontItalic()
{
}

void CMainFrame::OnFontUnderline()
{
}

void CMainFrame::OnChangeLineColor()
{
}

void CMainFrame::OnUpdateLineColor(CCmdUI* pCmd)
{
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmd->m_pOther;
	if (pToolBar)
	{	
		CXTPControlPopupColor* pPopup = (CXTPControlPopupColor*)pToolBar->GetControls()->GetAt(pCmd->m_nIndex);
		if (m_bLineColor)
			pPopup->SetColor(m_clrLine);
		else
			pPopup->SetNoColor();
	}

	pCmd->Enable(TRUE);
}

void CMainFrame::OnNoLineColor()
{
	m_bLineColor = false;
}

void CMainFrame::OnSelectLineColor(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
		
	CXTPControlColorSelector* pControl = (CXTPControlColorSelector*)tagNMCONTROL->pControl;
	m_clrLine = pControl->GetColor();
	m_bLineColor = true;

	OnChangeLineColor();

	*pResult = 1;
}

void CMainFrame::OnUpdateSelectLineColor(CCmdUI* pCmd)
{
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmd->m_pOther;
	if (pToolBar)
	{	
		/*CHARFORMAT& cfm = GetCharFormatSelection( );	
		CXTPControlColorSelector* pSelector = (CXTPControlColorSelector*)pToolBar->GetControls()->GetAt(pCmd->m_nIndex);
		pSelector->SetColor(cfm.dwEffects & CFE_AUTOCOLOR? -1: cfm.crTextColor);*/
	}
	pCmd->Enable(TRUE);
}

void CMainFrame::OnMoreLineColors()
{
	CColorDialog cd(m_clrLine);
	if (cd.DoModal())
	{
		m_clrLine = cd.GetColor();
		m_bLineColor = true;
		OnChangeLineColor();
	}
}

void CMainFrame::OnChangeFillColor()
{
}

void CMainFrame::OnNoFillColor()
{
	m_bFillColor = false;
}

void CMainFrame::OnSelectFillColor(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
		
	CXTPControlColorSelector* pControl = (CXTPControlColorSelector*)tagNMCONTROL->pControl;
	m_clrFill = pControl->GetColor();
	m_bFillColor = true;

	OnChangeFillColor();

	*pResult = 1;
}

void CMainFrame::OnUpdateSelectFillColor(CCmdUI* pCmd)
{
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmd->m_pOther;
	if (pToolBar)
	{	
		/*CHARFORMAT& cfm = GetCharFormatSelection( );	
		CXTPControlColorSelector* pSelector = (CXTPControlColorSelector*)pToolBar->GetControls()->GetAt(pCmd->m_nIndex);
		pSelector->SetColor(cfm.dwEffects & CFE_AUTOCOLOR? -1: cfm.crTextColor);*/
	}
	pCmd->Enable(TRUE);
}

void CMainFrame::OnMoreFillColors()
{
	CColorDialog cd(m_clrFill);
	if (cd.DoModal())
	{
		m_clrFill = cd.GetColor();
		m_bFillColor = true;
		OnChangeFillColor();
	}
}

void CMainFrame::OnUpdateFillColor(CCmdUI* pCmd)
{
	
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmd->m_pOther;
	if (pToolBar)
	{	
		CXTPControlPopupColor* pPopup = (CXTPControlPopupColor*)pToolBar->GetControls()->GetAt(pCmd->m_nIndex);
		if (m_bFillColor)
		{
			pPopup->SetColor(m_clrFill);
		}
		else
			pPopup->SetNoColor();
	}

	pCmd->Enable(TRUE);
}

void CMainFrame::OnChangeLineWidth()
{
	if (m_pToolAttributesBar)
	{
	}
}

void CMainFrame::OnUpdateLineWidth(CCmdUI* pCmd)
{
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmd->m_pOther;
	if (pToolBar)
	{	
	}

	pCmd->Enable(TRUE);
}

void CMainFrame::OnMoreLineWidths()
{
	MessageBox("Halli");
	/*CColorDialog cd(m_clrFill);
	if (cd.DoModal())
	{
		m_clrFill = cd.GetColor();
		m_bFillColor = true;
		OnChangeFillColor();
	}*/
}

void CMainFrame::OnSelectLineWidth(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
		
	CControlLineWidthSelector* pControl = (CControlLineWidthSelector*)tagNMCONTROL->pControl;
	double m_dLineWidth = pControl->GetLineWidth();

	OnChangeLineWidth();

	*pResult = 1;
}


void CMainFrame::OnUpdateSelectLineWidth(CCmdUI* pCmd)
{
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmd->m_pOther;
	if (pToolBar)
	{	
	}
	pCmd->Enable(TRUE);
}

void CMainFrame::OnChangeLineStyle()
{
}

void CMainFrame::OnUpdateLineStyle(CCmdUI* pCmd)
{
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmd->m_pOther;
	if (pToolBar)
	{	
	}

	pCmd->Enable(TRUE);
}

void CMainFrame::OnSelectLineStyle(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
		
	CControlLineStyleSelector* pControl = (CControlLineStyleSelector*)tagNMCONTROL->pControl;
	int m_iLineStyle = pControl->GetLineStyle();

	OnChangeLineStyle();

	*pResult = 1;
}


void CMainFrame::OnUpdateSelectLineStyle(CCmdUI* pCmd)
{
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmd->m_pOther;
	if (pToolBar)
	{	
	}
	pCmd->Enable(TRUE);
}


void CMainFrame::OnGoToNextPage()
{
	MessageBox("Go to next page");
}

void CMainFrame::OnGoToPreviousPage()
{
	MessageBox("Go to previous page");
}

void CMainFrame::OnGoToFirstPage()
{
	MessageBox("Go to first page");
}

void CMainFrame::OnGoToLastPage()
{
	MessageBox("Go to last page");
}

void CMainFrame::OnMarkerYellow()
{
	MessageBox("Marker yellow");
}

void CMainFrame::OnMarkerOrange()
{
	MessageBox("Marker orange");
}

void CMainFrame::OnMarkerMagenta()
{
	MessageBox("Marker magenta");
}

void CMainFrame::OnMarkerGreen()
{
	MessageBox("Marker green");
}

void CMainFrame::OnPenRed()
{
	MessageBox("Pen red");
}

void CMainFrame::OnPenGreen()
{
	MessageBox("Pen green");
}

void CMainFrame::OnPenBlue()
{
	MessageBox("Pen blue");
}

void CMainFrame::OnPenBlack()
{
	MessageBox("Pen black");
}

void CMainFrame::OnPenWhite()
{
	MessageBox("Pen white");
}

void CMainFrame::OnSimpleNavigation()
{
	MessageBox("Simple Navigation");
}

void CMainFrame::OnTelepointer()
{
	MessageBox("telepointer");
}

void CMainFrame::OnFullScreen()
{
	MessageBox("Full Screen");
}

void CMainFrame::OnShowHideStructure()
{
	MessageBox("Show/Hide Structure");
}

void CMainFrame::OnShowHideToolbar()
{
	MessageBox("Show/Hide toolbar");
}

void CMainFrame::OnStartRecording()
{
	MessageBox("Start Recording");
	m_bRecording = true;
}

void CMainFrame::OnUpdateStartRecording(CCmdUI* pCmd)
{
	if (!m_bRecording)
		pCmd->Enable(TRUE);
	else
		pCmd->Enable(FALSE);
}

void CMainFrame::OnPauseRecording()
{
	MessageBox("Pause Recording");
	m_bRecordingPaused = true;
}

void CMainFrame::OnUpdatePauseRecording(CCmdUI* pCmd)
{
	if (m_bRecording)
		pCmd->Enable(TRUE);
	else
		pCmd->Enable(FALSE);
}

void CMainFrame::OnStopRecording()
{
	MessageBox("Stop Recording");
	m_bRecording = false;
}

void CMainFrame::OnUpdateStopRecording(CCmdUI* pCmd)
{
	if (m_bRecording)
		pCmd->Enable(TRUE);
	else
		pCmd->Enable(FALSE);
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame Meldungshandler


void CMainFrame::OnClose()
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
    SaveCommandBars(_T("CommandBars"));

	CXTPFrameWndBase<CFrameWnd>::OnClose();
}

BOOL CMainFrame::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.

	if (message == WM_XTP_COMMAND && wParam == ID_COMBO_FONTSIZE)
	{
		return TRUE;
	}

	return CXTPFrameWndBase<CFrameWnd>::OnWndMsg(message, wParam, lParam, pResult);
}
