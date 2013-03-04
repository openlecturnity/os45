// AssistantView.cpp : implementation of the CAssistantView class
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#endif

#include "MainFrm.h"
#include "AssistantView.h"
#include "AssistantDoc.h"
#include "backend\la_project.h"
#include "backend\mlb_misc.h"
#include "DrawingToolSettings.h"

#ifdef _STUDIO
#include "..\Studio\MainFrm.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CAssistantView

#define GRID_IDENT_SIZE 5
#define GRID_MAX_SIZE 60
#define GRID_MIN_SIZE 5
#define WHITEBOARD_EDGE_SIZE 6
#define WHITEBOARD_OFFSET_SIZE 13
#define SCREEN_OFFSET_SIZE 0
#define COLOR_BG_BLACK 0x00000000
#define COLOR_THEME_BG_BLUE 0x00AE9990
#define COLOR_THEME_BG_OLIVE 0x007BA097
#define COLOR_THEME_BG_SILVER 0x00B39A9B
#define COLOR_THEME_BG_STANDARD 0x00808080

IMPLEMENT_DYNCREATE(CAssistantView, CView)

BEGIN_MESSAGE_MAP(CAssistantView, CView)
	//{{AFX_MSG_MAP(CAssistantView)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()  
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONUP()
   ON_COMMAND(ID_INSERT_IMAGE, OnInsertImage)
   ON_UPDATE_COMMAND_UI(ID_INSERT_IMAGE, OnUpdateInsertImage)
   ON_COMMAND(ID_FILL_COLOR, OnFillColor)
   ON_UPDATE_COMMAND_UI(ID_FILL_COLOR, OnUpdateFillColor)
   ON_COMMAND(ID_LINE_COLOR, OnLineColor)
   ON_UPDATE_COMMAND_UI(ID_LINE_COLOR, OnUpdateLineColor)
   ON_COMMAND(ID_TEXT_COLOR, OnTextColor)
   ON_UPDATE_COMMAND_UI(ID_TEXT_COLOR, OnUpdateTextColor)
   //ON_COMMAND(ID_FONTFAMILY, OnFontFamily)
   //ON_COMMAND(ID_FONTSIZE, OnFontSize)
   ON_XTP_EXECUTE(ID_FONTFAMILY, OnFontFamily)
   ON_XTP_EXECUTE(ID_FONTSIZE, OnFontSize)
   ON_UPDATE_COMMAND_UI(ID_FONT_FAMILY_LIST, OnUpdateFontList)
   ON_UPDATE_COMMAND_UI(ID_FONT_SIZE_LIST, OnUpdateFontList)
   ON_COMMAND(ID_FONTSLANT, OnFontSlant)
   ON_UPDATE_COMMAND_UI(ID_FONTSLANT, OnUpdateFontSlant)
   ON_COMMAND(ID_FONTWEIGHT, OnFontWeight)
   ON_UPDATE_COMMAND_UI(ID_FONTWEIGHT, OnUpdateFontWeight)
   ON_XTP_EXECUTE(ID_FONT_SIZE_INCREASE, OnFontSizeIncrease)
   ON_XTP_EXECUTE(ID_FONT_SIZE_DECREASE, OnFontSizeDecrease)
   
   ON_UPDATE_COMMAND_UI(ID_FONT_SIZE_INCREASE, OnUpdateFontSizeIncrease)
   ON_UPDATE_COMMAND_UI(ID_FONT_SIZE_DECREASE, OnUpdateFontSizeDecrease)
   
   ON_COMMAND(ID_LINE_WIDTH, OnLineWidth)
   ON_UPDATE_COMMAND_UI(ID_LINE_WIDTH, OnUpdateLineWidth)
   ON_COMMAND(ID_LINE_STYLE, OnLineStyle)
   ON_UPDATE_COMMAND_UI(ID_LINE_STYLE, OnUpdateLineStyle)
   ON_COMMAND(ID_ARROW_STYLE, OnArrowStyle)
   ON_UPDATE_COMMAND_UI(ID_ARROW_STYLE, OnUpdateArrowStyle)
   ON_UPDATE_COMMAND_UI(ID_FONTFAMILY, OnUpdateFontFamily)
   ON_UPDATE_COMMAND_UI(ID_FONTSIZE, OnUpdateFontSize)
   ON_COMMAND(ID_TOOL_COPY, OnChangeTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_TEXT, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_COPY, OnUpdateCopyTool)
	ON_WM_ERASEBKGND()
   ON_COMMAND(ID_EDIT_UNDO, OnUndo)
   ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateUndo)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_SHOW_MOSE_POINTER, OnShowMosePointer)
	ON_UPDATE_COMMAND_UI(ID_SHOW_MOSE_POINTER, OnUpdateShowMosePointer)
	ON_COMMAND(ID_HIDE_MOUSE_POINTER, OnHideMousePointer)
	ON_UPDATE_COMMAND_UI(ID_HIDE_MOUSE_POINTER, OnUpdateHideMousePointer)
	ON_WM_CREATE()
   ON_COMMAND(ID_TOOL_TEXT, OnChangeTool)
   ON_COMMAND(ID_TOOL_FREEHAND, OnChangeTool)
   ON_COMMAND(ID_TOOL_LINE, OnChangeTool)
   ON_COMMAND(ID_TOOL_RECTANGLE, OnChangeTool)
   ON_COMMAND(ID_TOOL_OVAL, OnChangeTool)
   ON_COMMAND(ID_TOOL_POLYLINE, OnChangeTool)
   ON_COMMAND(ID_TOOL_POLYGON, OnChangeTool)
   ON_COMMAND(ID_TOOL_MARKERBLUE, OnChangeTool)
   ON_COMMAND(ID_TOOL_MARKERGREEN, OnChangeTool)
   ON_COMMAND(ID_TOOL_MARKERMAGENTA, OnChangeTool)
   ON_COMMAND(ID_TOOL_MARKERORANGE, OnChangeTool)
   ON_COMMAND(ID_TOOL_MARKERRED, OnChangeTool)
   ON_COMMAND(ID_TOOL_MARKERYELLOW, OnChangeTool)
   ON_COMMAND(ID_TOOL_PENBLACK, OnChangeTool)
   ON_COMMAND(ID_TOOL_PENBLUE, OnChangeTool)
   ON_COMMAND(ID_TOOL_PENCYAN, OnChangeTool)
   ON_COMMAND(ID_TOOL_PENGREEN, OnChangeTool)
   ON_COMMAND(ID_TOOL_PENMAGENTA, OnChangeTool)
   ON_COMMAND(ID_TOOL_PENRED, OnChangeTool)
   ON_COMMAND(ID_TOOL_PENWHITE, OnChangeTool)
   ON_COMMAND(ID_TOOL_PENYELLOW, OnChangeTool)
   ON_COMMAND(ID_TOOL_TELEPOINTER, OnChangeTool)
   ON_COMMAND(ID_TOOL_SIMPLENAVIGATION, OnChangeTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_FREEHAND, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_LINE, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_RECTANGLE, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_OVAL, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_POLYLINE, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_POLYGON, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_MARKERBLUE, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_MARKERGREEN, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_MARKERMAGENTA, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_MARKERORANGE, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_MARKERRED, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_MARKERYELLOW, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_PENBLACK, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_PENBLUE, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_PENCYAN, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_PENGREEN, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_PENMAGENTA, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_PENRED, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_PENWHITE, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_PENYELLOW, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_TELEPOINTER, OnUpdateTool)
   ON_UPDATE_COMMAND_UI(ID_TOOL_SIMPLENAVIGATION, OnUpdateTool)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
   
   
   ON_COMMAND(IDS_NO_FILL_COLOR, OnNoFillColor)
   ON_COMMAND(IDS_MORE_FILL_COLORS, OnMoreFillColors)
   ON_XTP_EXECUTE(ID_SELECTOR_FILL_COLOR, OnSelectFillColor)
   ON_UPDATE_COMMAND_UI(ID_SELECTOR_FILL_COLOR, OnUpdateSelectFillColor)
   ON_UPDATE_COMMAND_UI(IDS_NO_FILL_COLOR, OnUpdateNoFillColor)
   
   ON_COMMAND(IDS_NO_LINE_COLOR, OnNoLineColor)
   ON_COMMAND(IDS_MORE_LINE_COLORS, OnMoreLineColors)
   ON_XTP_EXECUTE(ID_SELECTOR_LINE_COLOR, OnSelectLineColor)
   ON_UPDATE_COMMAND_UI(ID_SELECTOR_LINE_COLOR, OnUpdateSelectLineColor)
   ON_UPDATE_COMMAND_UI(IDS_NO_LINE_COLOR, OnUpdateNoLineColor)
   
   ON_COMMAND(IDS_NO_TEXT_COLOR, OnNoTextColor)
   ON_COMMAND(IDS_MORE_TEXT_COLORS, OnMoreTextColors)
   ON_XTP_EXECUTE(ID_SELECTOR_TEXT_COLOR, OnSelectTextColor)
   ON_UPDATE_COMMAND_UI(ID_SELECTOR_TEXT_COLOR, OnUpdateSelectTextColor)
   
   ON_XTP_EXECUTE(ID_SELECTOR_LINE_WIDTH, OnSelectLineWidth)
   ON_UPDATE_COMMAND_UI(ID_SELECTOR_LINE_WIDTH, OnUpdateSelectLineWidth)
   
   ON_XTP_EXECUTE(ID_SELECTOR_LINE_STYLE, OnSelectLineStyle)
   ON_UPDATE_COMMAND_UI(ID_SELECTOR_LINE_STYLE, OnUpdateSelectLineStyle)
   
   ON_XTP_EXECUTE(ID_SELECTOR_ARROW_STYLE, OnSelectArrowStyle)
   ON_UPDATE_COMMAND_UI(ID_SELECTOR_ARROW_STYLE, OnUpdateSelectArrowStyle)

   ON_XTP_EXECUTE(ID_PRESENTATION_PENCILS, OnXTPGalleryPens)
   ON_UPDATE_COMMAND_UI(ID_PRESENTATION_PENCILS, OnEnableGalleryButton)
   ON_COMMAND(ID_BUTTON_PENS, OnButtonPen)
   ON_UPDATE_COMMAND_UI(ID_BUTTON_PENS, OnUpdateButtonPen)

   ON_XTP_EXECUTE(ID_PRESENTATION_MARKERS, OnXTPGalleryMarkers)
   ON_UPDATE_COMMAND_UI(ID_PRESENTATION_MARKERS, OnEnableGalleryButton)
   ON_COMMAND(ID_BUTTON_MARKERS, OnButtonMarker)
   ON_UPDATE_COMMAND_UI(ID_BUTTON_MARKERS, OnUpdateButtonMarker)

   //ON_REGISTERED_MESSAGE(CSBSDK::SBSDK_NEW_MESSAGE, OnSBSDKNewMessage)

   ON_UPDATE_COMMAND_UI(ID_VIEW_PRESENTATION, OnUpdateViewPresentation)

   ON_COMMAND(ID_GRID_DISPLAY, OnCheckDisplayGrid)
   ON_UPDATE_COMMAND_UI(ID_GRID_DISPLAY, OnUpdateDisplayGrid)
   ON_COMMAND(ID_GRID_SNAP, OnCheckSnapToGrid)
   ON_UPDATE_COMMAND_UI(ID_GRID_SNAP, OnUpdateSnapGrid)
   ON_UPDATE_COMMAND_UI(IDS_GRID_SPACING, OnEnableLabel)

	ON_UPDATE_COMMAND_UI(ID_GRID_SPACING, OnUpdateGridSpacing)
	ON_XTP_EXECUTE(ID_GRID_SPACING, OnGridSpacing)
	ON_NOTIFY(XTP_FN_SPINUP, ID_GRID_SPACING, OnGridSpacingSpin)
	ON_NOTIFY(XTP_FN_SPINDOWN, ID_GRID_SPACING, OnGridSpacingSpin)

   ON_UPDATE_COMMAND_UI(ID_FULL_SCREEN, OnUpdateFullScreen)
   ON_UPDATE_COMMAND_UI(ID_FULL_SCREEN_CS, OnUpdateFullScreenCS)
   
   END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAssistantView construction/destruction

CAssistantView::CAssistantView()
{
	m_bButtonPressed = false;
   m_bMoveObjects = false;
   m_bSelectObjects = false;
   m_bCreateObjects = false;
   m_bInsertImage = false;
   m_bSelectTextArea = false;
   
   m_clrFill = RGB(255, 255, 255);
   m_bFillColor = false;
   
   m_clrLine = RGB(0, 0, 0);
   m_bLineColor = true;
   
   m_clrText= RGB(0, 0, 0);
   m_bTextColor = true;
   
   m_bFontItalic = false;
   m_bFontBold = false;

   m_bSnapObject = false;

   m_dLineWidth = 2;
   m_iLineStyle = 0;
   m_iArrowStyle = 0;
   m_nPencilWidth = 2;
   m_nMarkerWidth = 20;

   m_cf.cbSize = sizeof(CHARFORMAT2);
   m_cf.dwMask = CFM_FACE | CFM_CHARSET | CFM_SIZE;
   m_cf.dwEffects = 0;
   m_cf.yHeight = 200; // Twips is pixel * 20
   m_cf.yOffset = 0;
   m_cf.crTextColor = 0;
   m_cf.bPitchAndFamily = DEFAULT_PITCH;
   _tcscpy(m_cf.szFaceName, _T("Arial"));
   
   m_hMoveCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE);
   m_hEdit_nwse_Cursor = LoadCursor(NULL, IDC_SIZENWSE);
   m_hEdit_nesw_Cursor = LoadCursor(NULL, IDC_SIZENESW);
   m_hEdit_we_Cursor = LoadCursor(NULL, IDC_SIZEWE);
   m_hEdit_ns_Cursor = LoadCursor(NULL, IDC_SIZENS);
   m_hTelepointerCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_TELEPOINTER);
   m_hSmallCrossCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_SMALLCROSS);
   m_hNoCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_NO_CURSOR);
   m_hHandCurosr = LoadCursor(NULL, IDC_HAND);
	m_hRectangleCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_RECTANGLE);
   m_hOvalCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_OVAL);
   m_hPolylineCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_POLYLINE);
   m_hPolygonCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_POLYGON);
   m_hFreeHandCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_FREEHAND);
   m_hLineCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_LINE);
   m_hTextCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_TEXT);

   m_hMarkerBlueCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_MARKER_BLUE);
   m_hMarkerOrangeCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_MARKER_ORANGE);
   m_hMarkerRedCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_MARKER_RED);
   m_hMarkerYellowCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_MARKER_YELLOW);
   m_hMarkerGreenCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_MARKER_GREEN);
   m_hMarkerMagentaCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_MARKER_MAGENTA);

   m_hPenBlackCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_PENCIL_BLACK);
   m_hPenBlueCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_PENCIL_BLUE);
   m_hPenCyanCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_PENCIL_CYAN);
   m_hPenGreenCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_PENCIL_GREEN);
   m_hPenMagentaCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_PENCIL_MAGENTA);
   m_hPenRedCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_PENCIL_RED);
   m_hPenWhiteCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_PENCIL_WHITE);
   m_hPenYellowCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_PENCIL_YELLOW);
   m_hCopyCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_COPY);

   m_hAreaSelection = AfxGetApp()->LoadCursor(IDC_CURSOR_AREA_SELECTION);

   m_dLineWidth = 1.0;

   m_iCurrentTool = ID_TOOL_NOTHING;
   m_pActiveObject = NULL;

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
   
   m_nGridSnap = 0;
   m_nGridDisplay = 0;
   m_nGridSpacing = 30;

}

CAssistantView::~CAssistantView()
{
   if ( m_Markers != NULL )
      delete m_Markers;
   if ( m_Pens != NULL )
      delete m_Pens;
}

BOOL CAssistantView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

int CAssistantView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CView::OnCreate(lpCreateStruct) == -1)
      return -1;


	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CAssistantView drawing

void CAssistantView::OnDraw(CDC* pDC)
{
    CAssistantDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    CRect rcClient;
    GetClientRect(rcClient);

    COLORREF bgMyColor;

    if(pDoc->IsFullScreenOn()) {
        bgMyColor = COLOR_BG_BLACK;
    } else {
        XTPCurrentSystemTheme crtSystemTheme;
        crtSystemTheme = /*XTPDrawHelpers()*/XTPColorManager()->GetCurrentSystemTheme();
        switch(crtSystemTheme)
        {
        case xtpSystemThemeBlue: 
            bgMyColor = COLOR_THEME_BG_BLUE;
            break;
        case xtpSystemThemeOlive:
            bgMyColor = COLOR_THEME_BG_OLIVE;
            break;
        case xtpSystemThemeSilver:
            bgMyColor = COLOR_THEME_BG_SILVER;
            break;
        default:
            bgMyColor = COLOR_THEME_BG_STANDARD;
            break;
        }
   }

    COLORREF bgColor = bgMyColor;//GetXtremeColor(COLOR_3DFACE);//
    CRect rcUpdate;
    pDC->GetClipBox(&rcUpdate);
    CXTMemDC memDC(pDC, rcClient, bgColor);
    if(pDoc->IsFullScreenOn() == false) {
        CXTPClientRect rcClientRect(this); 
        rcClientRect.right = rcClient.right;
        rcClientRect.bottom = rcClient.bottom;
        CXTPWindowRect rcWindowClient(GetDlgItem(AFX_IDW_PANE_FIRST));
        ScreenToClient(&rcWindowClient);
        CMainFrameA*pMainFrameA =  CMainFrameA::GetCurrentInstance();
        CXTPCommandBars* pCommandBars = pMainFrameA->GetCommandBars();
        ((CXTPOffice2007Theme*)(pCommandBars->GetPaintManager()))->FillWorkspace(&memDC, rcClientRect, rcWindowClient);
    }

    CRgn rgnUpdate;
    rgnUpdate.CreateRectRgn(rcUpdate.left, rcUpdate.top, rcUpdate.right, rcUpdate.bottom);
    memDC.SelectClipRgn(&rgnUpdate);

    //CBrush greyBrush(bgColor);
    //memDC.FillRect(rcClient, &greyBrush);

    pDoc->DrawWhiteboard(rcClient, &memDC);

    if (!pDoc->IsFullScreenOn()) {
        DrawGrid(rcClient, &memDC);
    }

    CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
    if (pMainFrame != NULL) {
        pMainFrame->SetRefreshTransparentBtn(false);
        pMainFrame->RefreshTransparentButton();
    }
}

void CAssistantView::DrawGrid(CRect rcClient, CDC *pDC)
{
	CAssistantDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   UINT uiAlphaFactor = 200;
    //here the big if
	if(ASSISTANT::Project::active && (pDoc->IsGridVisible() || pDoc->IsSnapEnabled()))
	{
		UINT nPageW, nPageH, nOffX, nOffY;
		if(pDoc->GetPagePropertiesEx(rcClient, nPageW, nPageH, nOffX, nOffY) == S_OK)
		{
			ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
	        float fZoomFactor = pPage->CalculateZoomFactor(rcClient);
	        m_fGridWidth = pDoc->GetGridWidth()*fZoomFactor;
			
			if(pDoc->IsGridVisible())
			{
				int nNrOfVLines = nPageW / m_fGridWidth;
				int nNrOfHLines = nPageH / m_fGridWidth;

				Gdiplus::Graphics graphics(pDC->m_hDC);
				
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);
				
				float spacing = 4*fZoomFactor;
	         spacing = (float)m_fGridWidth/5 - 1;

            if(((int)m_fGridWidth < 20)&&
               ((int)m_fGridWidth >= 5))
            {	
               spacing = (float)m_fGridWidth/3 - 1;
               uiAlphaFactor = 70;
            }
				Gdiplus::Color lineColor = Gdiplus::Color(uiAlphaFactor,0x00, 0x00, 0x00);
            float aArray [] = {1 , spacing};
				Gdiplus::Pen pen(lineColor, 1);
				pen.SetDashPattern(aArray, 2);
				pen.SetAlignment(Gdiplus::PenAlignmentCenter);
				for(int i = 1; i <= nNrOfVLines; i++)
				{	
					graphics.DrawLine(&pen, (Gdiplus::REAL)nOffX + (m_fGridWidth * i), (Gdiplus::REAL)nOffY, (Gdiplus::REAL)nOffX + (m_fGridWidth * i), (Gdiplus::REAL)nOffY + nPageH );   	
				}

				for(int j = 1; j <= nNrOfHLines; j++)
				{
					graphics.DrawLine(&pen, (Gdiplus::REAL)nOffX, (Gdiplus::REAL)nOffY + (m_fGridWidth * j), (Gdiplus::REAL)nOffX + nPageW, (Gdiplus::REAL)nOffY + (m_fGridWidth * j));
				}
			}

		}
	}

}

/////////////////////////////////////////////////////////////////////////////
// CAssistantView printing

BOOL CAssistantView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAssistantView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAssistantView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CAssistantView diagnostics

#ifdef _DEBUG
void CAssistantView::AssertValid() const
{
	CView::AssertValid();
}

void CAssistantView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

CAssistantDoc* CAssistantView::GetDocument()
{
    return (CAssistantDoc*)CMainFrameA::GetAssistantDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CAssistantView message handlers

void CAssistantView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bButtonPressed = true;

   LeftMouseDown(point);

   SetCapture();
   
   //if((m_iCurrentTool ==  ID_TOOL_COPY )&&(m_bOneClickCopySelecteObject==false))
   //{
   //  m_iCurrentTool = ID_TOOL_NOTHING;
   //}

   CView::OnLButtonDown(nFlags, point);
}

void CAssistantView::OnLButtonUp(UINT nFlags, CPoint point) 
{
    LeftMouseUp(point);

    ReleaseCapture();

    m_bButtonPressed = false;
    CView::OnLButtonUp(nFlags, point);
}

void CAssistantView::OnMouseMove(UINT nFlags, CPoint point) 
{ 
   if (!m_bButtonPressed && (nFlags == MK_LBUTTON))
      OnLButtonDown(MK_LBUTTON, point);

   LeftMouseMove(point);
      
   /*CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
   if (pMainFrame != NULL)
   {
       pMainFrame->SetPresentationBarTransparent();
       pMainFrame->SetPresentationBarAutoHide(point);
   }*/

   CView::OnMouseMove(nFlags, point);
}

void CAssistantView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    bool bIsMultiObject = m_pActiveObject && 
        (m_pActiveObject->GetType() == ASSISTANT::GenericObject::POLYLINE ||
        m_pActiveObject->GetType() == ASSISTANT::GenericObject::POLYGON);

    if (bIsMultiObject) {
        EndCreateObjects();
    } else if (m_iCurrentTool == ID_TOOL_SIMPLENAVIGATION) {
        // Two "up" evens are scheduled anyway by the system
    }
    CView::OnLButtonDblClk(nFlags, point);
}


// messages from menu

void CAssistantView::OnInsertImage() 
{
    if (ASSISTANT::Project::active == NULL)
        return; 

   CString csImageName;
   ASSISTANT::GetOpenFilename(IDS_IMAGE_FILTER, csImageName, _T("IMAGE"));
   
   ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
   if (!csImageName.IsEmpty() && pPage != NULL)
   {
      CString csImagePath = csImageName;
      ASSISTANT::GetPath(csImagePath);
      
      pPage->InsertImage((_TCHAR *)(LPCTSTR)csImageName, (_TCHAR *)(LPCTSTR)csImagePath, 0, 0);

	  CAssistantDoc* pDoc = GetDocument();

	  m_bInsertImage = true;

      RedrawWindow();
   }
}

void CAssistantView::OnUpdateInsertImage(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDoc = GetDocument();
   
   if (pDoc && pDoc->HasActivePage())
      pCmdUI->Enable(true);
   else
      pCmdUI->Enable(false);
}

// messages from toolbar

void CAssistantView::OnFillColor() 
{
   CAssistantDoc *pDoc = GetDocument();
   if (pDoc)
   {
      if (m_bFillColor)
         pDoc->ChangeFillColor(m_clrFill);
      else
         pDoc->ChangeFillColor();

      if (m_bFillColor)
         pDoc->ChangeFillColor(m_clrFill);
      else
         pDoc->ChangeFillColor();

      if (pDoc->HasActivePage())
      {
          CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
          if (pMainFrame != NULL)
              pMainFrame->SetRefreshTransparentBtn();
          RedrawWindow();   
      }
   }
}

void CAssistantView::OnUpdateFillColor(CCmdUI* pCmdUI) 
{
   // TODO: Add your command update UI handler code here
   CXTPCommandBar* pRibbonBar = (CXTPRibbonBar*)pCmdUI->m_pOther;
   if (pRibbonBar)
   {	
	   CXTPControlPopupColor* pPopup = (CXTPControlPopupColor*)pRibbonBar->GetControls()->GetAt(pCmdUI->m_nIndex);
	   if (m_bFillColor && m_clrFill != ((COLORREF)-1))
	   {
		   pPopup->SetColor(m_clrFill);	
	   }
		else
		{
			pPopup->SetColor(NULL);
			pPopup->SetNoColor();
	    }
   }
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnSelectFillColor(NMHDR* pNMHDR, LRESULT* pResult)
{
   NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
   
   CXTPControlColorSelector* pControl = (CXTPControlColorSelector*)tagNMCONTROL->pControl;
   m_clrFill = pControl->GetColor();
   
   m_bFillColor = true;
   
   OnFillColor();
   
   *pResult = 1;
}

void CAssistantView::OnUpdateSelectFillColor(CCmdUI* pCmdUI)
{
   CXTPCommandBar* pRibbonBar = (CXTPRibbonBar*)pCmdUI->m_pOther;
   if (pRibbonBar)
   {	
	  CXTPControlColorSelector *pControlColorSelector = (CXTPControlColorSelector*)pRibbonBar->GetControls()->GetAt(pCmdUI->m_nIndex);
	  if (m_bFillColor  && m_clrFill != ((COLORREF)-1))
	  {
         pControlColorSelector->SetColor(m_clrFill);
	  }
      else
	  {
		  pControlColorSelector->SetColor((COLORREF)-1);
	  }
   }
 
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
   
}

void CAssistantView::OnMoreFillColors()
{
   CColorDialog cd(m_clrFill);
   if (cd.DoModal())
   {
      m_clrFill = cd.GetColor();
      m_bFillColor = true;
      OnFillColor();
   }
}

void CAssistantView::OnUpdateNoFillColor(CCmdUI* pCmdUI)
{ 
   CXTPCommandBar* pRibbonBar = (CXTPRibbonBar*)pCmdUI->m_pOther;
   if (pRibbonBar)
   {	
	CXTPControlButtonColor* pNoFillColorBar = (CXTPControlButtonColor*)pRibbonBar->GetControls()->GetAt(pCmdUI->m_nIndex);
	
	if (m_bFillColor && m_clrFill != ((COLORREF)-1))
		pNoFillColorBar->SetChecked(false);
	else
		//if(!m_bFillColor && m_clrFill == ((COLORREF)-1))
		//{
			pNoFillColorBar->SetChecked(true);
		//}
   }
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnNoFillColor()
{
   m_clrFill = (COLORREF)-1;
   m_bFillColor = false;
   OnFillColor();
}

void CAssistantView::OnLineColor() 
{
   CAssistantDoc *pDoc = GetDocument();

   if (pDoc)
   {
      pDoc->ChangeLineColor(m_clrLine);

      if ( pDoc->HasActivePage())
      {
          CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
          if (pMainFrame != NULL)
              pMainFrame->SetRefreshTransparentBtn();
          RedrawWindow();    
      }
   }
}

void CAssistantView::OnUpdateLineColor(CCmdUI* pCmdUI) 
{
   // TODO: Add your command update UI handler code here
   CXTPCommandBar* pRibbonBar = (CXTPRibbonBar*)pCmdUI->m_pOther;
   if (pRibbonBar)
   {	
      CXTPControlPopupColor* pPopup = (CXTPControlPopupColor*)pRibbonBar->GetControls()->GetAt(pCmdUI->m_nIndex);
      if (m_bLineColor && m_clrLine != ((COLORREF)-1))
         pPopup->SetColor(m_clrLine);
      else
	  {
         pPopup->SetColor(NULL);
         pPopup->SetNoColor();
	  }

   }
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnSelectLineColor(NMHDR* pNMHDR, LRESULT* pResult)
{
   NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
   
   CXTPControlColorSelector* pControl = (CXTPControlColorSelector*)tagNMCONTROL->pControl;
   m_clrLine = pControl->GetColor();
   m_bLineColor = true;
   
   OnLineColor();
   
   *pResult = 1;
}

void CAssistantView::OnUpdateSelectLineColor(CCmdUI* pCmdUI)
{
   CXTPCommandBar* pRibbonBar = (CXTPRibbonBar*)pCmdUI->m_pOther;
   if (pRibbonBar)
   {	
      CXTPControlColorSelector *pControlColorSelector = (CXTPControlColorSelector*)pRibbonBar->GetControls()->GetAt(pCmdUI->m_nIndex);
      if (m_bLineColor  && m_clrLine != ((COLORREF)-1))
      {
         pControlColorSelector->SetColor(m_clrLine);
      }
      else
      {
         pControlColorSelector->SetColor((COLORREF)-1);
      }
   }
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnMoreLineColors()
{
   CColorDialog cd(m_clrLine);
   if (cd.DoModal())
   {
      m_clrLine = cd.GetColor();
      m_bLineColor = true;
      OnLineColor();
   }
}

void CAssistantView::OnNoLineColor()
{
   m_clrLine = (COLORREF)-1;
   m_bLineColor = false;
   OnLineColor();
}

void CAssistantView::OnUpdateNoLineColor(CCmdUI* pCmdUI)
{ 
   CXTPCommandBar* pRibbonBar = (CXTPRibbonBar*)pCmdUI->m_pOther;
   if (pRibbonBar)
   {	
      CXTPControlButtonColor* pNoLineColorBar = (CXTPControlButtonColor*)pRibbonBar->GetControls()->GetAt(pCmdUI->m_nIndex);

      if (m_bLineColor && m_clrLine != ((COLORREF)-1))
         pNoLineColorBar->SetChecked(false);
      else
         //if(!m_bLineColor && m_clrLine == ((COLORREF)-1))
         //{
         pNoLineColorBar->SetChecked(true);
      //}
   }
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnTextColor() 
{
   CAssistantDoc *pDoc = GetDocument();

   if (pDoc)
   {
      pDoc->ChangeTextColor(m_clrText);
      if  (pDoc->HasActivePage())
      {
          CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
          if (pMainFrame != NULL)
              pMainFrame->SetRefreshTransparentBtn();
          RedrawWindow();      
      }
   }
}

void CAssistantView::OnUpdateTextColor(CCmdUI* pCmdUI) 
{
   // TODO: Add your command update UI handler code here
   CXTPCommandBar* pRibbonBar = (CXTPRibbonBar*)pCmdUI->m_pOther;
   if (pRibbonBar)
   {	
      CXTPControlPopupColor* pPopup = (CXTPControlPopupColor*)pRibbonBar->GetControls()->GetAt(pCmdUI->m_nIndex);
      if (m_bTextColor)
         pPopup->SetColor(m_clrText);
      else
         pPopup->SetNoColor();
   }
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnSelectTextColor(NMHDR* pNMHDR, LRESULT* pResult)
{
   NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
   
   CXTPControlColorSelector* pControl = (CXTPControlColorSelector*)tagNMCONTROL->pControl;
   m_clrText = pControl->GetColor();
   m_bTextColor = true;
   
   OnTextColor();
   
   *pResult = 1;
}

void CAssistantView::OnUpdateSelectTextColor(CCmdUI* pCmdUI)
{
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnMoreTextColors()
{
   CColorDialog cd(m_clrText);
   if (cd.DoModal())
   {
      m_clrLine = cd.GetColor();
      m_bTextColor = true;
      OnTextColor();
   }
}

void CAssistantView::OnNoTextColor()
{
   m_bTextColor = false;
}

void CAssistantView::OnFontFamily(NMHDR* pNMHDR, LRESULT* pResult) 
{
    if (ASSISTANT::Project::active == NULL)
        return; 
   CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();

   if (pMainFrame == NULL)
      return;

   CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
   if (pCommandBars != NULL)
   {
 
      USES_CONVERSION;
      NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
      CXTPControlFontComboBox* pControl = (CXTPControlFontComboBox*)tagNMCONTROL->pControl;
      
      if(pControl)
      {
         if (pControl->GetType() == xtpControlComboBox)
         {
            _tcscpy(m_cf.szFaceName, pControl->GetText());
         }
      }
   }   

   CAssistantDoc *pDoc = GetDocument();

   if (m_bCreateObjects && m_pActiveObject && 
       m_pActiveObject->GetType() == ASSISTANT::GenericObject::GROUP)
   {
      ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
   }

   if (pDoc)
   {
      pDoc->ChangeFontFamily(m_cf.szFaceName);
      if (pDoc->HasActivePage())
      {
         pMainFrame->SetRefreshTransparentBtn();
         RedrawWindow();     
      }
   }
}

void CAssistantView::OnFontSize(NMHDR* pNMHDR, LRESULT* pResult) 
{
    if (ASSISTANT::Project::active == NULL)
        return; 
   CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();

   if (pMainFrame == NULL)
      return;

   CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
   if (pCommandBars != NULL)
   {
      NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

      CXTPControlSizeComboBox* pControl = (CXTPControlSizeComboBox*)tagNMCONTROL->pControl;
      if(pControl)
      { 
         if (pControl->GetType() == xtpControlComboBox)
         {
            CString csString = pControl->GetText();
            m_cf.yHeight = _ttoi(csString) * 20;
         }
      }
   }   

   CAssistantDoc *pDoc = GetDocument();

   if (m_bCreateObjects && m_pActiveObject && 
       m_pActiveObject->GetType() == ASSISTANT::GenericObject::GROUP)
   {
      ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
   }

   if (pDoc)
   {
      int iFontSize = m_cf.yHeight / 20;
      pDoc->ChangeFontSize(iFontSize);
      if (pDoc->HasActivePage())
      {
         pMainFrame->SetRefreshTransparentBtn();
         RedrawWindow();    
      }
   }
}

void CAssistantView::OnFontWeight()
{
    if (ASSISTANT::Project::active == NULL)
        return; 
   CAssistantDoc *pDoc = GetDocument();

   m_bFontBold = !m_bFontBold;

   if (ASSISTANT::Project::active)
      ASSISTANT::Project::active->ChangeFontWeight(m_bFontBold);

   if (pDoc && pDoc->HasActivePage())
   {
       CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
       if (pMainFrame != NULL)
           pMainFrame->SetRefreshTransparentBtn();

       RedrawWindow();      
   }
}

void CAssistantView::OnUpdateFontWeight(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck(m_bFontBold);
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnFontSizeIncrease(NMHDR* pNMHDR, LRESULT* pResult) 
{
    if (ASSISTANT::Project::active == NULL)
        return; 
   CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();

   if (pMainFrame == NULL)
      return;

   CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
   if (pCommandBars != NULL)
   {
      NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

      CXTPControlSizeComboBox* pControl = (CXTPControlSizeComboBox*)tagNMCONTROL->pControl;
      if(pControl)
      { 
         CXTPRibbonGroup *pGroup = pControl->GetRibbonGroup();
         if(pGroup)
         {
            CXTPControlSizeComboBox* pControl = (CXTPControlSizeComboBox*)pGroup->FindControl(ID_FONTSIZE);

            if (pControl->GetType() == xtpControlComboBox)
            {
               pControl->UpdatePopupSelection();
               int iCurSel = pControl->GetCurSel();
               if(iCurSel + 1 < 22)
               {
                  pControl->SetCurSel(iCurSel + 1);
               }
               CString csString = pControl->GetText();
               m_cf.yHeight = _ttoi(csString) * 20;
            }
         }
      }
   }   

   CAssistantDoc *pDoc = GetDocument();

   if (m_bCreateObjects && m_pActiveObject && 
      m_pActiveObject->GetType() == ASSISTANT::GenericObject::GROUP)
   {
      ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
   }

   if (pDoc)
   {
      int iFontSize = m_cf.yHeight / 20;
      pDoc->ChangeFontSize(iFontSize);
      if (pDoc->HasActivePage())
      {
         pMainFrame->SetRefreshTransparentBtn();
         RedrawWindow();    
      }
   }
}

void CAssistantView::OnFontSizeDecrease(NMHDR* pNMHDR, LRESULT* pResult) 
{
    if (ASSISTANT::Project::active == NULL)
        return; 
   CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();

   if (pMainFrame == NULL)
      return;

   CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
   if (pCommandBars != NULL)
   {
      NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

      CXTPControlSizeComboBox* pControl = (CXTPControlSizeComboBox*)tagNMCONTROL->pControl;
      if(pControl)
      { 
         CXTPRibbonGroup *pGroup = pControl->GetRibbonGroup();
         if(pGroup)
         {
            CXTPControlSizeComboBox* pControl = (CXTPControlSizeComboBox*)pGroup->FindControl(ID_FONTSIZE);

            if (pControl->GetType() == xtpControlComboBox)
            {
               pControl->UpdatePopupSelection();
               int iCurSel = pControl->GetCurSel();
               if(iCurSel - 1 >= 0)
               {
                  pControl->SetCurSel(iCurSel - 1);
               }
               CString csString = pControl->GetText();
               m_cf.yHeight = _ttoi(csString) * 20;
            }
         }
      }
   }   

   CAssistantDoc *pDoc = GetDocument();

   if (m_bCreateObjects && m_pActiveObject && 
      m_pActiveObject->GetType() == ASSISTANT::GenericObject::GROUP)
   {
      ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
   }

   if (pDoc)
   {
      int iFontSize = m_cf.yHeight / 20;
      pDoc->ChangeFontSize(iFontSize);
      if (pDoc->HasActivePage())
      {
         pMainFrame->SetRefreshTransparentBtn();
         RedrawWindow();    
      }
   }
}

void CAssistantView::OnFontSlant()
{
    if (ASSISTANT::Project::active == NULL)
        return; 
   CAssistantDoc *pDoc = GetDocument();

   m_bFontItalic = !m_bFontItalic;

   if (ASSISTANT::Project::active)
      ASSISTANT::Project::active->ChangeFontSlant(m_bFontItalic);

   if (pDoc && pDoc->HasActivePage())
   {
      CMainFrameA *pMainFrame = GetMainFrame();
      if (pMainFrame != NULL)
         pMainFrame->SetRefreshTransparentBtn();

      RedrawWindow();      
   }
}

void CAssistantView::OnUpdateFontSlant(CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
	pCmdUI->SetCheck(m_bFontItalic);
      bEnable = TRUE;
}
   pCmdUI->Enable(bEnable);
}
void CAssistantView::OnUpdateFontSizeIncrease(CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}
void CAssistantView::OnUpdateFontSizeDecrease(CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}
void CAssistantView::OnLineWidth() 
{
   CAssistantDoc *pDoc = GetDocument();

   if (pDoc)
   {
      pDoc->ChangeLineWidth(m_dLineWidth);

      if (pDoc->HasActivePage())
      {
         CMainFrameA *pMainFrame = GetMainFrame();

         if (pMainFrame != NULL)
            pMainFrame->SetRefreshTransparentBtn();
         RedrawWindow();      
      }
   }
}

void CAssistantView::OnUpdateLineWidth(CCmdUI* pCmdUI) 
{
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
   
}

void CAssistantView::OnSelectLineWidth(NMHDR* pNMHDR, LRESULT* pResult)
{
   NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
   
   CControlLineWidthSelector* pControl = (CControlLineWidthSelector*)tagNMCONTROL->pControl;
   m_dLineWidth = pControl->GetLineWidth();
   
   OnLineWidth();
   
   *pResult = 1;
}


void CAssistantView::OnUpdateSelectLineWidth(CCmdUI* pCmdUI)
{
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnLineStyle() 
{
   CAssistantDoc *pDoc = GetDocument();

   if (pDoc)
   {
      pDoc->ChangeLineStyle(m_iLineStyle);

      if (pDoc->HasActivePage())
      {
         CMainFrameA *pMainFrame = GetMainFrame();

         if (pMainFrame != NULL)
            pMainFrame->SetRefreshTransparentBtn();
         RedrawWindow();    
      }
   }
}

void CAssistantView::OnUpdateLineStyle(CCmdUI* pCmdUI) 
{
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnSelectLineStyle(NMHDR* pNMHDR, LRESULT* pResult)
{
   NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
   
   CControlLineStyleSelector* pControl = (CControlLineStyleSelector*)tagNMCONTROL->pControl;
   m_iLineStyle = pControl->GetObjLineStyle();
   
   OnLineStyle();
   
   *pResult = 1;
}


void CAssistantView::OnUpdateSelectLineStyle(CCmdUI* pCmdUI)
{
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnArrowStyle() 
{
   CAssistantDoc *pDoc = GetDocument();

   if (pDoc)
   {
      int iArrowConfig;
      CString csArrowStyle;
      ASSISTANT::GetArrowConfig(m_iArrowStyle, iArrowConfig, csArrowStyle);
      pDoc->ChangeArrowStyle(iArrowConfig, csArrowStyle);
      
      if (pDoc->HasActivePage()) {
         CMainFrameA *pMainFrame = GetMainFrame();
         if (pMainFrame != NULL)
            pMainFrame->SetRefreshTransparentBtn();
         RedrawWindow();      
      }
   }
}

void CAssistantView::OnUpdateArrowStyle(CCmdUI* pCmdUI) 
{
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
   
}

void CAssistantView::OnSelectArrowStyle(NMHDR* pNMHDR, LRESULT* pResult)
{
   NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
   
   CControlArrowStyleSelector* pControl = (CControlArrowStyleSelector*)tagNMCONTROL->pControl;
   m_iArrowStyle = pControl->GetArrowStyle();
   
   OnArrowStyle();
   
   *pResult = 1;
}


void CAssistantView::OnUpdateSelectArrowStyle(CCmdUI* pCmdUI)
{
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnUpdateFontFamily(CCmdUI* pCmdUI) 
{
   CXTPCommandBar* pRibbonBar = (CXTPRibbonBar*)pCmdUI->m_pOther;
   if (pRibbonBar)
   {	
      CXTPControlFontComboBox* pFontCombo = (CXTPControlFontComboBox*)pRibbonBar->GetControls()->FindControl(ID_FONTFAMILY);

      if (pFontCombo->GetType() == xtpControlComboBox)
      {
         pFontCombo->SetCharFormat(m_cf);
      }		
   }
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnUpdateFontSize(CCmdUI* pCmdUI) 
{
   CXTPCommandBar* pRibbonBar = (CXTPRibbonBar*)pCmdUI->m_pOther;
   
   if (pRibbonBar)
   {	
      CXTPControlSizeComboBox* pSizeCombo = (CXTPControlSizeComboBox*)pRibbonBar->GetControls()->FindControl(pCmdUI->m_nID);//GetAt(pCmdUI->m_nIndex);

      if(!pSizeCombo->GetRibbonGroup())
      {
         if (pSizeCombo->GetType() == xtpControlComboBox)
         {
            pSizeCombo->SetTwipSize( (m_cf.dwMask & CFM_SIZE) ? m_cf.yHeight : -1);
         }	
      }
      else
      {
         CString strSize;
         strSize.Format(_T("%d"), (m_cf.yHeight/20));
         pSizeCombo->SetEditText(strSize);
      }
   }

   CAssistantDoc *pDoc = GetDocument();  
   BOOL bEnable = FALSE;

   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   } 
   
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnUpdateFontList(CCmdUI* pCmdUI) 
{
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}


void CAssistantView::OnChangeTool() 
{
   CAssistantDoc *pDoc = GetDocument();

	const MSG* pMsg = GetCurrentMessage( );

   int iActivatedTool = WPARAM(pMsg->wParam) & 0xFFFF;

   ChangeTool(iActivatedTool);
}

void CAssistantView::OnUpdateTool(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDoc = GetDocument();
   
   bool bEnable = false;
   
   bEnable = true;
   if (pCmdUI->m_nID == m_iCurrentTool)
   {
      pCmdUI->SetCheck(true);
   }
   else
   {
      pCmdUI->SetCheck(false);
   }
   if (pDoc->HasActivePage() || (pCmdUI->m_nID == ID_TOOL_TELEPOINTER && pDoc->IsRecording()))
      bEnable = true;
   else
      bEnable = false;
   pCmdUI->Enable(bEnable);

}

void CAssistantView::OnUpdateCopyTool(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDoc = (CAssistantDoc *)CMainFrameA::GetAssistantDocument();

   //if (pDoc != NULL && pDoc->HasLoadedDocuments() && 
   //   pDoc->HasActivePage() && pDoc->HasSelectedObjects())
   //   pCmdUI->Enable(true);
   //else
   //   pCmdUI->Enable(false);
   //CAssistantDoc *pDoc = GetDocument();
   
   bool bEnable = false;
   
   bEnable = true;
   if (pCmdUI->m_nID == m_iCurrentTool)
   {
      pCmdUI->SetCheck(true);
   }
   else
   {
      pCmdUI->SetCheck(false);
   }

   if (pDoc != NULL && pDoc->HasLoadedDocuments() && 
      pDoc->HasActivePage() /*&& pDoc->HasSelectedObjects()*/)
      bEnable = true;
   else
      bEnable = false;
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnUndo() 
{
   CAssistantDoc *pDoc = GetDocument();
   bool bIsRecording = (pDoc->IsRecording() && ASSISTANT::Project::active->GetAvGrabber()->getSgAnnotationLastUndoTimeStamp() != -1);

   if ((pDoc && pDoc->HasLoadedDocuments() && pDoc->PageIsChanged()) || bIsRecording)
   {
      pDoc->Undo();
      
      CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
      if (pMainFrame != NULL)
         pMainFrame->SetRefreshTransparentBtn();

      RedrawWindow();
   }

   
}

void CAssistantView::OnUpdateUndo(CCmdUI* pCmdUI) 
{
   CAssistantDoc *pDoc = GetDocument();
   bool bIsRecording = (pDoc->IsRecording() && ASSISTANT::Project::active->GetAvGrabber()->getSgAnnotationLastUndoTimeStamp() != -1);
   
   if ((pDoc->HasLoadedDocuments() && pDoc->PageIsChanged()) || bIsRecording)
      pCmdUI->Enable(true);
   else
      pCmdUI->Enable(false);

}

void CAssistantView::OnShowMosePointer() 
{
   CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
   pMainFrame->SetShowMouseCursor(!pMainFrame->GetShowMouseCursor());
}

void CAssistantView::OnUpdateShowMosePointer(CCmdUI* pCmdUI) 
{
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
   CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
   pCmdUI->SetCheck(pMainFrame->GetShowMouseCursor());
}

void CAssistantView::OnHideMousePointer() 
{
   CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();
   pMainFrame->SetShowMouseCursor(!pMainFrame->GetShowMouseCursor());
}

void CAssistantView::OnUpdateHideMousePointer(CCmdUI* pCmdUI) 
{
   bool bIsStartupPage = false;
   BOOL bEnable = TRUE;
   if(GetMainFrame()!=NULL)
   {
      if(GetMainFrame()->GetStartupPageVisibility() == true)
      {
         bEnable = FALSE;
      }
      else
      {
         bEnable = TRUE;
         bool bShowMouseCursor = GetMainFrame()->GetShowMouseCursor();
   pCmdUI->SetCheck( !bShowMouseCursor );
}
   }
   pCmdUI->Enable(bEnable);
}


HRESULT CAssistantView::SelectTextArea(MouseStateId nMouseState, CPoint &point)
{
   static CRect rcSelection;
   static CPoint ptStart;

   HRESULT hr = S_OK;
   
   CAssistantDoc *pDoc = (CAssistantDoc *)GetDocument();
   if (pDoc == NULL || !pDoc->HasLoadedDocuments() || !pDoc->HasActivePage())
      hr = E_FAIL;
   
   CDC *pDC = GetDC();

   if (SUCCEEDED(hr) && nMouseState == MOUSE_DOWN)
   {
      // delete previous text if it is an empty text
      DeleteEmptyText();

      ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
      if (pPage)
         pPage->UnSelectObjects();
      rcSelection.SetRectEmpty();
      m_bSelectTextArea = true;
      ptStart = point;
	  /*int nFontSize;
	  pDoc->GetFontSize(nFontSize);
	  m_cf.yHeight = nFontSize * 20;*/
   }
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_MOVE)
   {
      if (GetKeyState(VK_LBUTTON) < 0)				         // left mouse key pressed
      {
         if (m_bSelectTextArea)
         {
			CRect rcNew;
            CPoint ptFrom(ptStart);
            CPoint ptTo(point);
            rcNew.SetRect(ptFrom, ptTo);
            rcNew.NormalizeRect();
            pDC->DrawDragRect(&rcNew, CSize(1, 1), &rcSelection, CSize(1, 1));
            rcSelection.CopyRect(&rcNew);
         }
      }
   }
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_UP)
   {
      if (!rcSelection.IsRectNull())
         pDC->DrawDragRect(CRect(0, 0, 0, 0), CSize(0, 0), rcSelection, CSize(1, 1));
      
      m_bSelectTextArea = false;

      ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();

      ASSISTANT::Group *pTextGroup = 
         new ASSISTANT::Group(ASSISTANT::Project::active->GetObjectID(), NULL, NULL, NULL);
   
      CPoint ptGroup(rcSelection.left, rcSelection.top);
      if (rcSelection.IsRectEmpty())
      {
         ptGroup = point;
      }

      Gdiplus::PointF ptObject;
      TranslateScreenToObjectCoordinates(ptGroup, ptObject);
	 
	  CAssistantDoc* pDoc = GetDocument();

	  CRect rcClient;
      GetClientRect(&rcClient);
	  float fZoomFactor = pPage->CalculateZoomFactor(rcClient);
	  int nFontSize = abs(m_cf.yHeight/20);

	  if(pDoc->IsSnapEnabled())
	  { 
		  SnapToGrid(ptGroup.x, ptGroup.y);
		  ptGroup.y = ptGroup.y - nFontSize*fZoomFactor;
		  TranslateScreenToObjectCoordinates(ptGroup, ptObject);
	  }

      pTextGroup->SetX(ptObject.X);
      pTextGroup->SetY(ptObject.Y);

      if (((double)rcSelection.Width() / fZoomFactor) < 10)
         rcSelection.right = rcSelection.left + 150;
      pTextGroup->SetWidth((double)rcSelection.Width() / fZoomFactor);

      if (((double)rcSelection.Height() / fZoomFactor) < nFontSize)
         rcSelection.bottom = rcSelection.top + 1.5*nFontSize;
      pTextGroup->SetHeight((double)rcSelection.Height() / fZoomFactor);

      pTextGroup->SetTextColor(m_clrText);
      
      CString csWeight = m_bFontBold ? _T("bold") : _T("normal");
      CString csSlant = m_bFontItalic ? _T("italic") : _T("roman");
   
      LOGFONT logFont;
      ASSISTANT::FillLogFont(logFont, m_cf.szFaceName, nFontSize, csWeight, csSlant);
      pTextGroup->SetLogFont(logFont);
   
      pPage->AppendObject(pTextGroup, true, false);

      pTextGroup->SetSelected(true);

      RedrawWindow();
   }
      
   ReleaseDC(pDC);

   
   return S_OK;
}

HRESULT CAssistantView::SelectObjects(MouseStateId nMouseState, CPoint &point)
{
   static CRect rcSelection;
   static CPoint ptStart;
   static Gdiplus::PointF ptLast;
   static UINT nEditType;
   static bool bEditObject;

   HRESULT hr = S_OK;
   
   CAssistantDoc *pDoc = (CAssistantDoc *)GetDocument();
   if (pDoc == NULL || !pDoc->HasLoadedDocuments() || !pDoc->HasActivePage())
      hr = E_FAIL;
   
   
   CDC *pDC = GetDC();

   ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
   if (SUCCEEDED(hr) && nMouseState == MOUSE_DOWN)
   {
      bEditObject = false;
      nEditType = ASSISTANT::DrawObject::NOTHING;
      if (pDoc->HasSelectedObjects())
      {
         Gdiplus::PointF ptObject;
         TranslateScreenToObjectCoordinates(point, ptObject);
         int iPointIndex;
         if (pPage && pPage->IsEditablePoint(ptObject, nEditType, iPointIndex))
         {
            if (nEditType != ASSISTANT::DrawObject::MOVE)
            {
               pPage->SetResizePoint(iPointIndex);
               bEditObject = true;
               ptLast = ptObject;
            }
         }
      }
      
      if (!bEditObject)
      {
         pDoc->UnSelectObjects();
         rcSelection.SetRect(point, point);
      }
      
      ptStart.x = point.x;
      ptStart.y = point.y;

      m_bSelectObjects = true;
   }
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_MOVE)
   {
      if (GetKeyState(VK_LBUTTON) < 0)				         // left mouse key pressed
      {
         if (m_bSelectObjects)
         {
            CRect rcNew;
            CPoint ptFrom(ptStart);
            CPoint ptTo(point);
            
            /*/
            if (!m_rcLastPage.PtInRect(point))           // selection can not be outside the page
            {
               if (ptTo.x < m_rcLastPage.left)
                  ptTo.x = m_rcLastPage.left;
               if (ptTo.y < m_rcLastPage.top)
                  ptTo.y = m_rcLastPage.top;
               if (ptTo.x > m_rcLastPage.right)
                  ptTo.x = m_rcLastPage.right;
               if (ptTo.y > m_rcLastPage.bottom)
                  ptTo.y = m_rcLastPage.bottom;
                  }
            /*/
            
            if (!bEditObject)
            {
               rcNew.SetRect(ptFrom, ptTo);
               rcNew.NormalizeRect();
               
               if (rcSelection.IsRectNull())
                  pDC->DrawDragRect(&rcNew, CSize(1, 1), NULL, CSize(1, 1));
               else
                  pDC->DrawDragRect(&rcNew, CSize(1, 1), &rcSelection, CSize(1, 1));
               rcSelection.CopyRect(&rcNew);
            }
            else
            {
               
			   Gdiplus::PointF ptObject;
               TranslateScreenToObjectCoordinates(point, ptObject);
			   
			   if(pDoc->IsSnapEnabled())
			   {
				   SnapToGrid(point.x,point.y);
				   TranslateScreenToObjectCoordinates(point, ptObject);   
			   }
			   
		       float fDiffX = (ptObject.X - ptLast.X);
		       float fDiffY = (ptObject.Y - ptLast.Y);
			   pPage->ResizeObjects(fDiffX, fDiffY); 
			  
               RedrawWindow();
               ptLast = ptObject;
            }
         }
      }
   }
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_UP)
   {
      if (!bEditObject)
      {
         // delete selection rectangle
         if (!rcSelection.IsRectNull())
            pDC->DrawDragRect(CRect(0, 0, 0, 0), CSize(0, 0), rcSelection, CSize(1, 1));
         
         CRect rcObject;
         TranslateScreenToObjectCoordinates(rcSelection, rcObject);
         pDoc->SelectObjectsInRange(rcObject);
         
         // Select single Object
         if (!pDoc->HasSelectedObjects() && 
            (rcSelection.Width() == 0 && rcSelection.Height() == 0))
         {
            CPoint ptScreen(rcSelection.left, rcSelection.top);
            Gdiplus::PointF ptObject;
            TranslateScreenToObjectCoordinates(ptScreen, ptObject);

            // search for object contains ptObject and select it
            ASSISTANT::DrawObject *pObject = pDoc->GetObject(ptObject);
            if (pObject && !pObject->IsSelected())
            {
               pObject->SetSelected(true);
            }

         }

         // reset interaction rectangle
         rcSelection.SetRect(0, 0, 0, 0);
      }
      else {
          if (pPage)
              pPage->SaveAsAOF();
      }

      UpdateColorSettings();
      UpdateFontSettings();

      m_bSelectObjects = false;
      
      RedrawWindow();
   }
      
   ReleaseDC(pDC);

   return hr;
}

HRESULT CAssistantView::MoveObjects(MouseStateId nMouseState, CPoint &ptScreen)
{
   HRESULT hr = S_OK;

   static Gdiplus::PointF s_ptLast;
   static CArray<ASSISTANT::DrawObject *, ASSISTANT::DrawObject *> s_arSelectedObjects;

   CAssistantDoc *pDoc = (CAssistantDoc *)GetDocument();
   if (pDoc == NULL || !pDoc->HasLoadedDocuments())
      hr = E_FAIL;
   
   Gdiplus::PointF ptObject;
   TranslateScreenToObjectCoordinates(ptScreen, ptObject);

   if (SUCCEEDED(hr) && (nMouseState == MOUSE_DOWN))
   {
	  s_arSelectedObjects.RemoveAll();

      ASSISTANT::DrawObject *pObject = pDoc->GetObject(ptObject);
      if (pObject != NULL)
      {
         // if CTRL is not pressed and point is not in an object -> unselect
         if (GetKeyState(VK_CONTROL) >= 0 && !pObject->IsSelected())
            pDoc->UnSelectObjects();
        
         pDoc->GetSelectedObjects(s_arSelectedObjects);
         if (!pObject->IsSelected())
         {
            pObject->SetSelected(true);
            s_arSelectedObjects.Add(pObject);
         }	

		 if(s_arSelectedObjects.GetSize() == 1)
			 if(pObject->GetType() == ASSISTANT::GenericObject::LINE ||
				pObject->GetType() == ASSISTANT::GenericObject::RECTANGLE ||
				pObject->GetType() == ASSISTANT::GenericObject::OVAL ||
				pObject->GetType() == ASSISTANT::GenericObject::POLYLINE ||
				pObject->GetType() == ASSISTANT::GenericObject::POLYGON ||
				pObject->GetType() == ASSISTANT::GenericObject::GROUP ||
				pObject->GetType() == ASSISTANT::GenericObject::IMAGE)
					m_bSnapObject = true;
			 else
				m_bSnapObject = false;
		 else
			 m_bSnapObject = true;
         
		 s_ptLast = ptObject;
         
         ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
         if (m_iCurrentTool == ID_TOOL_COPY)
         {
            CArray<ASSISTANT::DrawObject *, ASSISTANT::DrawObject *> arCopyObjects;
            for (int i = 0; i < s_arSelectedObjects.GetSize(); ++i)
            {
               ASSISTANT::DrawObject *pObject = s_arSelectedObjects[i];
               ASSISTANT::DrawObject *pCopyObject = NULL;
               if (pObject)
               {
                  pObject->SetSelected(false);
                  pCopyObject = pObject->Copy();
               }
               
               if (pCopyObject)
               {
                  if (pCopyObject->GetType() == ASSISTANT::GenericObject::GROUP)
                     ((ASSISTANT::Group *)pCopyObject)->AppendObjectsToPage(pPage);
                  else if (pCopyObject->GetType() == ASSISTANT::GenericObject::ANIMATED)
                     ((ASSISTANT::AnimatedObject *)pCopyObject)->AppendObjectsToPage(pPage);
                  pPage->AppendObject(pCopyObject, true);
                  arCopyObjects.Add(pCopyObject);
                  pCopyObject->SetSelected(true);
                  m_bOneClickCopySelecteObject = true;
               }

            }
            s_arSelectedObjects.RemoveAll();
            s_arSelectedObjects.Append(arCopyObjects);
         }
         else
         {
            if (pPage)
               pPage->AppendToUndoList(ASSISTANT::UndoAction::UNDOCHANGE, s_arSelectedObjects);
            m_bOneClickCopySelecteObject = false;
         }

         m_bMoveObjects = true;
		 
      }	  	  
      else
      {
         pDoc->UnSelectObjects();
         m_bMoveObjects = false;
         m_bOneClickCopySelecteObject = false;
      }
      RedrawWindow();
   }
   else if (SUCCEEDED(hr) && nMouseState == MOUSE_MOVE)
   {
      if (GetKeyState(VK_LBUTTON) < 0)				         // left mouse key pressed
      {
         if (m_bMoveObjects && s_arSelectedObjects.GetSize() > 0)
         {
            for (int i = 0; i < s_arSelectedObjects.GetSize(); ++i)
            {
               if(pDoc->IsSnapEnabled() && m_bSnapObject)
			   {
				   if((ptObject.X != s_ptLast.X) && (ptObject.Y != s_ptLast.Y))
				   {
					   SnapToGrid(ptScreen.x,ptScreen.y);
					   TranslateScreenToObjectCoordinates(ptScreen, ptObject);
					   s_arSelectedObjects[i]->SetX(ptObject.X);
					   s_arSelectedObjects[i]->SetY(ptObject.Y);
				   }
			   }
			   else
			   {
				   double dDiffX = ptObject.X - s_ptLast.X;
				   double dDiffY = ptObject.Y - s_ptLast.Y;
				   s_arSelectedObjects[i]->Move(dDiffX, dDiffY); 
			   }	
			   
            }
            s_ptLast = ptObject;
            RedrawWindow();
         }
      }
   }
   else if (SUCCEEDED(hr) && nMouseState == MOUSE_UP)
   {
      ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
      if (m_iCurrentTool == ID_TOOL_COPY)
      {
         if (pPage)
            pPage->AppendToUndoList(ASSISTANT::UndoAction::UNDOINSERT, s_arSelectedObjects);
      }
      s_arSelectedObjects.RemoveAll();
      m_bMoveObjects = false;
      m_bInsertImage = false;

      UpdateColorSettings();
      UpdateFontSettings();

      if (pPage)
         pPage->SaveAsAOF();
      RedrawWindow();
   }
   
   return hr;
}

HRESULT CAssistantView::SelectMoveObjects(MouseStateId nMouseState, CPoint &point)
{
   static CRect rcSelection;
   static CPoint ptStart;
   
   HRESULT hr = S_OK;
   
   CAssistantDoc *pDoc = (CAssistantDoc *)GetDocument();
   if (pDoc == NULL || !pDoc->HasLoadedDocuments() || !pDoc->HasActivePage())
      hr = E_FAIL;
   
   CDC *pDC = GetDC();
   
   
   Gdiplus::PointF ptObject;
   TranslateScreenToObjectCoordinates(point, ptObject);
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_DOWN)
   {
      ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
      if (pPage != NULL)
      {
         UINT nEditType;
         int iResizePoint;
         bool bIsEditablePoint = pPage->IsEditablePoint(ptObject, nEditType, iResizePoint);
         
         if ((bIsEditablePoint && nEditType == ASSISTANT::DrawObject::MOVE) || 
             (!bIsEditablePoint && pDoc->PointIsInObject(ptObject)))
         {
            bool bHyperlinkActivated = false;
            if (ASSISTANT::Project::active)
            {
               if (pPage->IsHyperlinkPoint(ptObject))
               {
                  bHyperlinkActivated = true;
                  ActivateHyperlink(ptObject);
               }
               
            }
            
            if (!bHyperlinkActivated)
               MoveObjects(nMouseState, point);
         }
         else if ((pDoc->GetFreezeMaster() || 
                   pDoc->GetFreezeDocument()) && 
                   pPage->IsHyperlinkPoint(ptObject))
         {
            ActivateHyperlink(ptObject);
         }
         else
            SelectObjects(nMouseState, point);
      }
   }
   else if (SUCCEEDED(hr) && (nMouseState == MOUSE_MOVE || nMouseState == MOUSE_UP))
   {
      if (GetKeyState(VK_LBUTTON) < 0)				         // left mouse key pressed
      {
         if (m_bMoveObjects)
            MoveObjects(nMouseState, point);
         else if (m_bSelectObjects)
            SelectObjects(nMouseState, point);
      }
   }
   
   ReleaseDC(pDC);
   
   return hr;
}

HRESULT CAssistantView::ActivateHyperlink(Gdiplus::PointF &ptObject)
{
   HRESULT hr = S_OK;

   ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
   if (pPage == NULL)
      hr = E_FAIL;

   CAssistantDoc *pDoc = NULL;
   if (SUCCEEDED(hr))
   {
      pDoc = (CAssistantDoc *)GetDocument();
      if (pDoc == NULL || !pDoc->HasLoadedDocuments() || !pDoc->HasActivePage())
         hr = E_FAIL;
   }

   if (SUCCEEDED(hr))
   {
      pPage->ActivateHyperlink(ptObject);
      if (ASSISTANT::Project::active && ASSISTANT::Project::active->GetActivePage())
      {
         int iActivePage = ASSISTANT::Project::active->GetActivePage()->GetID();

         CMainFrameA *pMainFrame = GetMainFrame();
         if (pMainFrame != NULL)
         {
            if (pMainFrame->GetStructureTree())
            {
               pMainFrame->GetStructureTree()->SetSelected(iActivePage);
            }
         }
      }

      pDoc->UpdateAllViews(NULL);
   }

   return hr;
}

HRESULT CAssistantView::CreateObjects(MouseStateId nMouseState, CPoint &ptScreen)
{
   static Gdiplus::PointF s_ptStart;
   static CPoint s_ptLastFreehand;
   static Gdiplus::ARGB s_freehandColor;
   static double s_dFreehandWidth;

   HRESULT hr = S_OK;

   ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
   if (pPage == NULL) 
      return E_FAIL;

    
   Gdiplus::PointF ptObject;
   TranslateScreenToObjectCoordinates(ptScreen, ptObject);

   CAssistantDoc* pDoc = GetDocument();
   bool snapObject = false;

   if (m_iCurrentTool == ID_TOOL_LINE ||
	   m_iCurrentTool == ID_TOOL_RECTANGLE ||
	   m_iCurrentTool == ID_TOOL_OVAL ||
	   m_iCurrentTool == ID_TOOL_POLYLINE ||
	   m_iCurrentTool == ID_TOOL_POLYGON)
	   snapObject = true;
   
   if(pDoc->IsSnapEnabled() && snapObject == true)
   {
	   SnapToGrid(ptScreen.x,ptScreen.y);
	   TranslateScreenToObjectCoordinates(ptScreen, ptObject);
   }
   
   if (nMouseState == MOUSE_DOWN)
   { 
      if (m_bCreateObjects)
      {
         if (!m_pActiveObject) 
            return S_OK;

		 if (m_pActiveObject->GetType() == ASSISTANT::GenericObject::POLYGON ||
             m_pActiveObject->GetType() == ASSISTANT::GenericObject::POLYLINE)
         {
            ((ASSISTANT::PolyObject *)m_pActiveObject)->AppendNewPoint(ptObject.X, ptObject.Y);
            
            if (pPage)
            {
               pPage->SavePointAsAOF((ASSISTANT::PolyObject *)m_pActiveObject);
               UINT nOffX = 0.0;
               UINT nOffY = 0.0;
               float fZoomFactor = 1.0;
               CRect rcClient;
               GetClientRect(&rcClient);
               fZoomFactor = pPage->CalculateZoomFactor(rcClient);
               pPage->CalculateOffset(rcClient, fZoomFactor, nOffX, nOffY);
               ((ASSISTANT::PolyObject *)m_pActiveObject)->Redraw(this, nOffX, nOffY, fZoomFactor);
            }
         }
         else
         {
            m_bCreateObjects = false;
            m_pActiveObject = NULL;
         }
      }

      if (!m_bCreateObjects)
      {
         pPage->UnSelectObjects();

         if (m_iCurrentTool == ID_TOOL_TELEPOINTER)
         {
            if (pPage)
            {
               pPage->ActivateOnlinePointer(ptObject);
               ::SetCursor(m_hTelepointerCursor);
               m_bCreateObjects = true;
               RedrawWindow();
            }
            return S_OK;
         }

         s_ptStart.X = ptObject.X;
         s_ptStart.Y = ptObject.Y;

         // Needed to draw freehand line segment
         s_ptLastFreehand = ptScreen;

         Gdiplus::ARGB argbLineColor = 
            Gdiplus::Color::MakeARGB(m_nTransperency, GetRValue(m_clrLine), 
                                     GetGValue(m_clrLine), GetBValue(m_clrLine)); 
         

         Gdiplus::ARGB argbFillColor = 0;
         
         if (m_bFillColor)
            argbFillColor = Gdiplus::Color::MakeARGB(m_nTransperency, GetRValue(m_clrFill), 
                                                     GetGValue(m_clrFill), GetBValue(m_clrFill)); 
         
         Gdiplus::DashStyle gdipLineStyle = Gdiplus::DashStyleSolid;
         if (m_iLineStyle == 1)
            gdipLineStyle = Gdiplus::DashStyleDot;
         else if (m_iLineStyle == 2)
            gdipLineStyle = Gdiplus::DashStyleDash;

         CString csFontFamily = m_cf.szFaceName;
         UINT nFontSize = abs(m_cf.yHeight/20);
         CString csSize;
         csSize.Format(_T("%d"), nFontSize);
         
         CString csWeight = (m_cf.dwEffects & CFE_BOLD) ? _T("bold") : _T("normal");
         CString csSlant = (m_cf.dwEffects & CFE_ITALIC) ? _T("italic") : _T("roman");
         
         
         int iArrowConfig = 0;
         CString csArrowStyle = _T("none");
         ASSISTANT::GetArrowConfig(m_iArrowStyle, iArrowConfig, csArrowStyle);
         
         ASSISTANT::DrawObject *pNewObject = NULL;
         ASSISTANT::DrawObject *pNewText = NULL;
        

         switch (m_iCurrentTool)
         {
         case ID_TOOL_FREEHAND:
            pNewObject = new ASSISTANT::PolyLine(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                                 argbLineColor, (int)m_dLineWidth, Gdiplus::DashStyleSolid, 
                                                 _T("none"), 0, ASSISTANT::Project::active->GetObjectID(), 
                                                 NULL, NULL, NULL, true, false);
            ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
            if (pPage)
               pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
            s_freehandColor = argbLineColor;
            s_dFreehandWidth = m_dLineWidth;
            break;
         case ID_TOOL_LINE:
            pNewObject = new ASSISTANT::Line(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                             argbLineColor, (int)m_dLineWidth, gdipLineStyle,
                                             csArrowStyle, iArrowConfig, ASSISTANT::Project::active->GetObjectID(), 
                                             NULL, NULL, NULL);
            break;
         case ID_TOOL_RECTANGLE:
            pNewObject = new ASSISTANT::Rectangle(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                                  argbLineColor, argbFillColor, (int)m_dLineWidth, gdipLineStyle,
                                                  ASSISTANT::Project::active->GetObjectID(), 
                                                  NULL, NULL, NULL);
            break;
         case ID_TOOL_OVAL:
            pNewObject = new ASSISTANT::Oval(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                             argbLineColor, argbFillColor, (int)m_dLineWidth, gdipLineStyle,
                                             ASSISTANT::Project::active->GetObjectID(),
                                             NULL, NULL, NULL);
            break;
         case ID_TOOL_POLYLINE:
            pNewObject = new ASSISTANT::PolyLine(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                                 argbLineColor,  (int)m_dLineWidth, gdipLineStyle,
                                                 csArrowStyle, iArrowConfig, 
                                                 ASSISTANT::Project::active->GetObjectID(), 
                                                 NULL, NULL, NULL, false, false);

            ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
            ((ASSISTANT::PolyObject *)pNewObject)->DrawNewSegment(this, 0, 0, 0, ptScreen, true);

            if (pPage)
               pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);

            break;
         case ID_TOOL_POLYGON:
            pNewObject = new ASSISTANT::Polygon(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                                argbLineColor, argbFillColor, (int)m_dLineWidth, gdipLineStyle,
                                                ASSISTANT::Project::active->GetObjectID(), 
                                                NULL, NULL, NULL);

            ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
            ((ASSISTANT::PolyObject *)pNewObject)->DrawNewSegment(this, 0, 0, 0, ptScreen, true);

            if (pPage)
               pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
            
            break;
         case ID_TOOL_MARKERBLUE:
            {
               Gdiplus::ARGB argbBlue = Gdiplus::Color::MakeARGB(127, 0, 255, 255);
               pNewObject = new ASSISTANT::Marker(ptObject.X, ptObject.Y, -1, argbBlue, m_nMarkerWidth, 
                                                  ASSISTANT::Project::active->GetObjectID(), 
                                                  NULL, NULL, NULL);
               
               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
            }
            break;
         case ID_TOOL_MARKERGREEN:
            {
               Gdiplus::ARGB argbGreen = Gdiplus::Color::MakeARGB(127, 0, 255, 0);
               pNewObject = new ASSISTANT::Marker(ptObject.X, ptObject.Y, -1, argbGreen, m_nMarkerWidth, 
                                                  ASSISTANT::Project::active->GetObjectID(), 
                                                  NULL, NULL, NULL);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
            }
            break;
         case ID_TOOL_MARKERMAGENTA:
            {
               Gdiplus::ARGB argbMagenta = Gdiplus::Color::MakeARGB(127, 255, 0, 255);
               pNewObject = new ASSISTANT::Marker(ptObject.X, ptObject.Y, -1, argbMagenta, m_nMarkerWidth, 
                                                  ASSISTANT::Project::active->GetObjectID(), 
                                                  NULL, NULL, NULL);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
            }
            break;
         case ID_TOOL_MARKERORANGE:
            {
               Gdiplus::ARGB argbOrange = Gdiplus::Color::MakeARGB(127, 255, 128, 0);
               pNewObject = new ASSISTANT::Marker(ptObject.X, ptObject.Y, -1, argbOrange, m_nMarkerWidth, 
                                                  ASSISTANT::Project::active->GetObjectID(), 
                                                  NULL, NULL, NULL);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
            }
            break;
         case ID_TOOL_MARKERRED:
            {
               Gdiplus::ARGB argbRed = Gdiplus::Color::MakeARGB(127, 255, 0, 0);
               pNewObject = new ASSISTANT::Marker(ptObject.X, ptObject.Y, -1, argbRed, m_nMarkerWidth, 
                                                  ASSISTANT::Project::active->GetObjectID(), 
                                                  NULL, NULL, NULL);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
            }
            break;
         case ID_TOOL_MARKERYELLOW:
            {
               Gdiplus::ARGB argbYellow = Gdiplus::Color::MakeARGB(127, 255, 255, 0);
               pNewObject = new ASSISTANT::Marker(ptObject.X, ptObject.Y, -1, argbYellow, m_nMarkerWidth, 
                                                  ASSISTANT::Project::active->GetObjectID(), 
                                                  NULL, NULL, NULL);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
            }
            break;
         case ID_TOOL_PENBLACK:
            {
               Gdiplus::ARGB argbBlack = Gdiplus::Color::MakeARGB(255, 0, 0, 0);
               pNewObject = new ASSISTANT::PolyLine(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                                    argbBlack, m_nPencilWidth, Gdiplus::DashStyleSolid,
                                                    _T("none"), 0, ASSISTANT::Project::active->GetObjectID(), 
                                                    NULL, NULL, NULL, true, true);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
               s_freehandColor = argbBlack;
               s_dFreehandWidth = m_nPencilWidth;
			  
            }
            break;
         case ID_TOOL_PENBLUE:
            {
               Gdiplus::ARGB argbBlue = Gdiplus::Color::MakeARGB(255, 0, 0, 255);
               pNewObject = new ASSISTANT::PolyLine(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                                    argbBlue, m_nPencilWidth, Gdiplus::DashStyleSolid,
                                                    _T("none"), 0, ASSISTANT::Project::active->GetObjectID(), 
                                                    NULL, NULL, NULL, true, true);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
               s_freehandColor = argbBlue;
               s_dFreehandWidth = m_nPencilWidth;
            }
            break;
         case ID_TOOL_PENCYAN:
            {
               Gdiplus::ARGB argbCyan = Gdiplus::Color::MakeARGB(255, 0, 255, 255);
               pNewObject = new ASSISTANT::PolyLine(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                                    argbCyan, m_nPencilWidth, Gdiplus::DashStyleSolid,
                                                    _T("none"), 0, ASSISTANT::Project::active->GetObjectID(), 
                                                    NULL, NULL, NULL, true, true);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
               s_freehandColor = argbCyan;
               s_dFreehandWidth = m_nPencilWidth;
            }
            break;
         case ID_TOOL_PENGREEN:
            {
               Gdiplus::ARGB argbGreen = Gdiplus::Color::MakeARGB(255, 0, 255, 0);
               pNewObject = new ASSISTANT::PolyLine(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                                    argbGreen, m_nPencilWidth, Gdiplus::DashStyleSolid,
                                                    _T("none"), 0, ASSISTANT::Project::active->GetObjectID(), 
                                                    NULL, NULL, NULL, true, true);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
               s_freehandColor = argbGreen;
               s_dFreehandWidth = m_nPencilWidth;
            }
            break;
         case ID_TOOL_PENMAGENTA:
            {
               Gdiplus::ARGB argbMagenta = Gdiplus::Color::MakeARGB(255, 255, 0, 255);
               pNewObject = new ASSISTANT::PolyLine(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                                    argbMagenta, m_nPencilWidth, Gdiplus::DashStyleSolid,
                                                    _T("none"), 0, ASSISTANT::Project::active->GetObjectID(), 
                                                    NULL, NULL, NULL, true, true);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
               s_freehandColor = argbMagenta;
               s_dFreehandWidth = m_nPencilWidth;
            }
            break;
         case ID_TOOL_PENRED:
            {
               Gdiplus::ARGB argbRed = Gdiplus::Color::MakeARGB(255, 255, 0, 0);
               pNewObject = new ASSISTANT::PolyLine(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                                    argbRed, m_nPencilWidth, Gdiplus::DashStyleSolid,
                                                    _T("none"), 0, ASSISTANT::Project::active->GetObjectID(), 
                                                    NULL, NULL, NULL, true, true);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
               s_freehandColor = argbRed;
               s_dFreehandWidth = m_nPencilWidth;
            }
            break;
         case ID_TOOL_PENWHITE:
            {
               Gdiplus::ARGB argbWhite = Gdiplus::Color::MakeARGB(255, 255, 255, 255);
               pNewObject = new ASSISTANT::PolyLine(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                                    argbWhite, m_nPencilWidth, Gdiplus::DashStyleSolid,
                                                    _T("none"), 0, ASSISTANT::Project::active->GetObjectID(), 
                                                    NULL, NULL, NULL, true, true);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
               s_freehandColor = argbWhite;
               s_dFreehandWidth = m_nPencilWidth;
            }
            break;
         case ID_TOOL_PENYELLOW:
            {
               Gdiplus::ARGB argbYellow = Gdiplus::Color::MakeARGB(255, 255, 255, 0);
               pNewObject = new ASSISTANT::PolyLine(ptObject.X, ptObject.Y, 0.0, 0.0, -1, 
                                                    argbYellow, m_nPencilWidth, Gdiplus::DashStyleSolid,
                                                    _T("none"), 0, ASSISTANT::Project::active->GetObjectID(), 
                                                    NULL, NULL, NULL, true, true);

               ((ASSISTANT::PolyObject *)pNewObject)->AppendNewPoint(ptObject.X, ptObject.Y);
               if (pPage)
                  pPage->SavePointAsAOF((ASSISTANT::PolyObject *)pNewObject);
               s_freehandColor = argbYellow;
               s_dFreehandWidth = m_nPencilWidth;

            }
            break;
         }
         
         m_pActiveObject = pNewObject;
         
         if (m_pActiveObject)
         {
            if (pPage)
            {
                pPage->AppendObject(m_pActiveObject, true, true);
            }
            m_bCreateObjects = true;
         }
         else
            m_bCreateObjects = false;
      }
   }
   else if (nMouseState == MOUSE_MOVE)
   {
      if (m_iCurrentTool == ID_TOOL_TELEPOINTER)
      {
         if (pPage)
         {
            pPage->MoveOnlinePointer(ptObject);
         }
         return S_OK;
      }

      if (!m_pActiveObject) 
         return S_OK;
      
      Gdiplus::RectF rcObject;

      if (s_ptStart.X > ptObject.X)
      {
		 rcObject.Width = s_ptStart.X - ptObject.X;
         rcObject.X = ptObject.X;	  
      }
      else
      {
		  rcObject.Width = ptObject.X - s_ptStart.X;
          rcObject.X = s_ptStart.X;
      }
      
      if (s_ptStart.Y > ptObject.Y)
      {
		  rcObject.Height = s_ptStart.Y - ptObject.Y;
          rcObject.Y = ptObject.Y;
      }
      else
      {
          rcObject.Height = ptObject.Y - s_ptStart.Y;
		  rcObject.Y = s_ptStart.Y;
      }
      
      CRect rcUpdate;
      CRect rcScreen;

      switch (m_iCurrentTool)
      {
      case ID_TOOL_PENBLACK:
      case ID_TOOL_PENBLUE:
      case ID_TOOL_PENCYAN:
      case ID_TOOL_PENGREEN:
      case ID_TOOL_PENMAGENTA:
      case ID_TOOL_PENRED:
      case ID_TOOL_PENWHITE:
      case ID_TOOL_PENYELLOW:
      case ID_TOOL_FREEHAND:
         { 
            double dWidth=0.0;
            ((ASSISTANT::PolyLine *)m_pActiveObject)->AppendNewPoint(ptObject.X, ptObject.Y);
            if (pPage)
               pPage->SavePointAsAOF((ASSISTANT::PolyObject *)m_pActiveObject);
            ((ASSISTANT::PolyLine *)m_pActiveObject)->GetLastPointsRect(rcUpdate);
            TranslateObjectToScreenCoordinates(rcUpdate, rcScreen); 
            // Draw freehand line segment
            CDC *pDC = GetDC();
            Gdiplus::Graphics graphics(pDC->GetSafeHdc());
            double dZoomedWidth = s_dFreehandWidth;
            if (pPage)
               dZoomedWidth = dZoomedWidth * pPage->GetZoomFactor();
            Gdiplus::Pen freehandPen(s_freehandColor, dZoomedWidth);
            freehandPen.SetLineCap(Gdiplus::LineCapRound , Gdiplus::LineCapRound, Gdiplus::DashCapRound);
            freehandPen.SetLineJoin(Gdiplus::LineJoinRound);

            graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

            if(s_ptLastFreehand.x>rcScreen.right)
            { 
              s_ptLastFreehand.x = rcScreen.right-(WHITEBOARD_EDGE_SIZE);
            }
            else if(s_ptLastFreehand.x<rcScreen.left)
            { 
              s_ptLastFreehand.x = rcScreen.left+(WHITEBOARD_EDGE_SIZE);
            }
            if(s_ptLastFreehand.y>rcScreen.bottom)
            { 
              s_ptLastFreehand.y = rcScreen.bottom-(WHITEBOARD_EDGE_SIZE);
            }
            else if(s_ptLastFreehand.y<rcScreen.top)
            { 
              s_ptLastFreehand.y = rcScreen.top+(WHITEBOARD_EDGE_SIZE);
            }

            if(ptScreen.x>rcScreen.right)
            { 
              ptScreen.x = rcScreen.right-(WHITEBOARD_EDGE_SIZE);
            }
            else if(ptScreen.x<rcScreen.left)
            { 
              ptScreen.x = rcScreen.left+(WHITEBOARD_EDGE_SIZE);
            }
            if(ptScreen.y>rcScreen.bottom)
            { 
              ptScreen.y = rcScreen.bottom-(WHITEBOARD_EDGE_SIZE);
            }
            else if(ptScreen.y<rcScreen.top)
            { 
              ptScreen.y = rcScreen.top+(WHITEBOARD_EDGE_SIZE);
            }

            if((ptScreen.x<rcScreen.right)&&( ptScreen.x>rcScreen.left)&&
               (ptScreen.y<rcScreen.bottom)&&( ptScreen.y>rcScreen.top))
            {  
               graphics.DrawLine(&freehandPen, 
                  (Gdiplus::REAL)s_ptLastFreehand.x, (Gdiplus::REAL)s_ptLastFreehand.y, 
                  (Gdiplus::REAL)ptScreen.x, (Gdiplus::REAL)ptScreen.y);
              // s_ptLastFreehand = ptScreen;

            }
            else
            {
               CRect rcClientScreen;
               CRect rcClientScreenTemp;
               rcClientScreenTemp.SetRectEmpty();
               rcClientScreen.SetRectEmpty();
               GetClientRect(&rcClientScreen);
               rcClientScreenTemp = rcClientScreen;
               if( ptScreen.y<rcScreen.top+WHITEBOARD_EDGE_SIZE)
               {
                  rcClientScreenTemp.bottom = rcScreen.top + (WHITEBOARD_EDGE_SIZE+WHITEBOARD_OFFSET_SIZE);
                  RedrawWindow(&rcClientScreenTemp);
               }
               else if(ptScreen.y>rcScreen.bottom-(WHITEBOARD_EDGE_SIZE+WHITEBOARD_OFFSET_SIZE))
               {
                  rcClientScreenTemp.top = rcScreen.bottom - (WHITEBOARD_EDGE_SIZE+WHITEBOARD_OFFSET_SIZE);
                  RedrawWindow(&rcClientScreenTemp);
               }              
               if( ptScreen.x<rcScreen.left+WHITEBOARD_EDGE_SIZE)
               {
                  rcClientScreenTemp.right = rcScreen.left + (WHITEBOARD_EDGE_SIZE+WHITEBOARD_OFFSET_SIZE);
                  RedrawWindow(&rcClientScreenTemp);
               }
               else if(ptScreen.x>rcScreen.right-(WHITEBOARD_EDGE_SIZE+WHITEBOARD_OFFSET_SIZE))
               {
                  rcClientScreenTemp.left = rcScreen.right - (WHITEBOARD_EDGE_SIZE+WHITEBOARD_OFFSET_SIZE);
                  RedrawWindow(&rcClientScreenTemp);
               }
               CRect rcFreehand;
               rcFreehand.SetRectEmpty();
               ((ASSISTANT::PolyLine *)m_pActiveObject)->GetUpdateRect(rcFreehand);
               if (rcFreehand.IsRectEmpty())
                   RedrawWindow();
               else
               {
                  CRect rcScreen;
                  TranslateObjectToScreenCoordinates(rcFreehand, rcScreen); 
                  RedrawWindow(&rcScreen);
                  //RedrawWindow();
               }
            }
            s_ptLastFreehand = ptScreen;
            ReleaseDC(pDC);
         }
         break;
      case ID_TOOL_MARKERBLUE:
      case ID_TOOL_MARKERGREEN:
      case ID_TOOL_MARKERMAGENTA:
      case ID_TOOL_MARKERORANGE:
      case ID_TOOL_MARKERRED:
         case ID_TOOL_MARKERYELLOW:

            ((ASSISTANT::PolyLine *)m_pActiveObject)->AppendNewPoint(ptObject.X, ptObject.Y);
            if (pPage)
               pPage->SavePointAsAOF((ASSISTANT::PolyObject *)m_pActiveObject);

            ((ASSISTANT::PolyLine *)m_pActiveObject)->GetLastPointsRect(rcUpdate);
            TranslateObjectToScreenCoordinates(rcUpdate, rcScreen); 
            RedrawWindow(&rcScreen);

            break;
         case ID_TOOL_RECTANGLE:
         case ID_TOOL_OVAL:
			m_pActiveObject->SetX(rcObject.X);
            m_pActiveObject->SetY(rcObject.Y);
            m_pActiveObject->SetWidth(rcObject.Width);
            m_pActiveObject->SetHeight(rcObject.Height);
            RedrawWindow();
            break;
         case ID_TOOL_LINE:
            m_pActiveObject->SetWidth(ptObject.X - m_pActiveObject->GetX());
            m_pActiveObject->SetHeight(ptObject.Y - m_pActiveObject->GetY());
            RedrawWindow();
            break;
         case ID_TOOL_POLYGON:
         case ID_TOOL_POLYLINE:
            if (m_pActiveObject->GetType() == ASSISTANT::GenericObject::POLYGON ||
                m_pActiveObject->GetType() == ASSISTANT::GenericObject::POLYLINE)
            {
               ASSISTANT::PolyObject *pPolyObject = (ASSISTANT::PolyObject *)m_pActiveObject;
               
               UINT nOffX = 0.0;
               UINT nOffY = 0.0;
               float fZoomFactor = 1.0;
               if (pPage)
               {
                  CRect rcClient;
                  GetClientRect(&rcClient);
                  fZoomFactor = pPage->CalculateZoomFactor(rcClient);
                  pPage->CalculateOffset(rcClient, fZoomFactor, nOffX, nOffY);
               }
			   pPolyObject->DrawNewSegment(this, nOffX, nOffY, fZoomFactor,ptScreen);
            }
            break;
      }
   }
   else if (nMouseState == MOUSE_UP)
   {
      if (m_iCurrentTool == ID_TOOL_TELEPOINTER)
      {
         if (pPage)
         {
            CDC *pDC = GetDC();
            CRect rcClient;
            GetClientRect(rcClient);
            ::SetCursor(m_hSmallCrossCursor);
            pPage->DrawOnlinePointer(rcClient, pDC);
            ReleaseDC(pDC);
            m_bCreateObjects = false;
         }
         return S_OK;
      }

      //CRect rcFreehand;
      //rcFreehand.SetRectEmpty();
      //if (m_pActiveObject->GetType() == ASSISTANT::GenericObject::FREEHAND)
      //{
      //   ((ASSISTANT::PolyLine *)m_pActiveObject)->GetUpdateRect(rcFreehand);
      //}

      if (m_pActiveObject->GetType() != ASSISTANT::GenericObject::POLYGON &&
          m_pActiveObject->GetType() != ASSISTANT::GenericObject::POLYLINE &&
          m_pActiveObject->GetType() != ASSISTANT::GenericObject::ASCIITEXT &&
          m_pActiveObject->GetType() != ASSISTANT::GenericObject::GROUP)
      {
         EndCreateObjects();
      }
      
      //if (rcFreehand.IsRectEmpty())
      //   RedrawWindow();
      //else
      //{
      //   CRect rcScreen;
      //   TranslateObjectToScreenCoordinates(rcFreehand, rcScreen); 
      //   RedrawWindow(&rcScreen);
      //}
   }

   return hr;
}

HRESULT CAssistantView::UpdateFontSettings()
{
   HRESULT hr = S_OK;

   bool bFontFamilySet = false;

   CAssistantDoc *pDoc = GetDocument();

   if (pDoc && pDoc->HasSelectedObjects())
   {
      CArray<ASSISTANT::DrawObject *, ASSISTANT::DrawObject *> aSelectedObjects;
      pDoc->GetSelectedObjects(aSelectedObjects);

      if (aSelectedObjects.GetSize() == 1)
      {
         if (aSelectedObjects[0]->GetType() == ASSISTANT::GenericObject::ASCIITEXT)
         {
            _tcscpy(m_cf.szFaceName, ((ASSISTANT::Text *)aSelectedObjects[0])->GetFamily());
            m_cf.yHeight = ((ASSISTANT::Text *)aSelectedObjects[0])->GetFontSize() * 20;
            CString csSlant = ((ASSISTANT::Text *)aSelectedObjects[0])->GetSlant();
            if (csSlant == _T("italic"))
               m_bFontItalic = true;
            else 
               m_bFontItalic = false;

            CString csWeight = ((ASSISTANT::Text *)aSelectedObjects[0])->GetWeight();
            if (csWeight == _T("bold"))
               m_bFontBold = true;
            else 
               m_bFontBold = false;

            bFontFamilySet = true;
         }
         if (aSelectedObjects[0]->GetType() == ASSISTANT::GenericObject::GROUP)
         {
            CString csFontFamily;
            ((ASSISTANT::Group *)aSelectedObjects[0])->GetTextFontFamily(csFontFamily);
            if (!csFontFamily.IsEmpty())
            {
               _tcscpy(m_cf.szFaceName, csFontFamily);
               bFontFamilySet = true;
            }
            int iTextFontSize = ((ASSISTANT::Group *)aSelectedObjects[0])->GetTextFontSize();
            if (iTextFontSize > 0)
               m_cf.yHeight =  iTextFontSize * 20;

            CString csSlant;
            ((ASSISTANT::Group *)aSelectedObjects[0])->GetTextFontSlant(csSlant);
            if (csSlant == _T("italic"))
               m_bFontItalic = true;
            else 
               m_bFontItalic = false;

            CString csWeight;
            ((ASSISTANT::Group *)aSelectedObjects[0])->GetTextFontWeight(csWeight);
            if (csWeight == _T("bold"))
               m_bFontBold = true;
            else 
               m_bFontBold = false;
         }
      }
   }

   if (!bFontFamilySet)
      _tcscpy(m_cf.szFaceName, _T("Arial"));

   return hr;
}

HRESULT CAssistantView::UpdateColorSettings()
{
   HRESULT hr = S_OK;

   CAssistantDoc *pDoc = GetDocument();

   if (pDoc && pDoc->HasSelectedObjects())
   {
      CArray<ASSISTANT::DrawObject *, ASSISTANT::DrawObject *> aSelectedObjects;
      pDoc->GetSelectedObjects(aSelectedObjects);

      if (aSelectedObjects.GetSize() == 1)
      {
         ASSISTANT::DrawObject *pObject = aSelectedObjects[0];
         if (pObject->GetType() != ASSISTANT::GenericObject::IMAGE &&
             pObject->GetType() != ASSISTANT::GenericObject::GROUP &&
             pObject->GetType() != ASSISTANT::GenericObject::ANIMATED)
         {
            Gdiplus::ARGB argbFill = ((ASSISTANT::ColoredObject *)pObject)->GetFillColor();
           
            if (argbFill != 0)
            {
               m_bFillColor = true; 
               m_clrFill = ASSISTANT::GdipARGBToColorref(argbFill);
            }
            else
            {
               m_bFillColor = false; 
            }

            Gdiplus::ARGB argbLine = ((ASSISTANT::ColoredObject *)pObject)->GetLineColor();
            m_clrLine = ASSISTANT::GdipARGBToColorref(argbLine);
            m_bLineColor = true;
         }
      }

      aSelectedObjects.RemoveAll();
   }

   return hr;
}

void CAssistantView::SnapToGrid(long &x, long &y)
{

	CAssistantDoc* pDoc = GetDocument();
	CRect rcClient;
	GetClientRect(rcClient);
	UINT nPageW, nPageH, nOffX, nOffY;
	if(pDoc->GetPagePropertiesEx(rcClient, nPageW, nPageH, nOffX, nOffY) == S_OK)
	{
		float widthX = m_fGridWidth + nOffX;
		while(widthX <= x)
			widthX = widthX + m_fGridWidth;
		widthX = widthX - m_fGridWidth;
		float middle = widthX + m_fGridWidth/2;
		if(x <= middle)
			x = widthX;
		else
			x = widthX + m_fGridWidth;

		float widthY = m_fGridWidth + nOffY;
		while(widthY <= y)
			widthY = widthY + m_fGridWidth;
		widthY = widthY - m_fGridWidth;
		middle = widthY + m_fGridWidth/2;
		if(y <= middle)
			y = widthY;
		else
			y = widthY + m_fGridWidth;
	}
}
	
void CAssistantView::ChangeTool(int iCurrentTool)
{
   m_nMarkerWidth = 20;
   m_nPencilWidth = 2;
   m_nTransperency = 255;

   
   ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
   if (pPage)
   {
      if (iCurrentTool != ID_TOOL_TEXT)
      {
         DeleteEmptyText();
         RedrawWindow();
      }

      if (iCurrentTool != ID_TOOL_COPY)
      {
         pPage->UnSelectObjects();
         RedrawWindow();
      }
   }
   
   if (m_iCurrentTool == ID_TOOL_TELEPOINTER)
   {
      if (pPage)
      {
         pPage->DeActivateOnlinePointer();
         RedrawWindow();
      }
   }
   
   if (m_pActiveObject && 
       (m_pActiveObject->GetType() == ASSISTANT::GenericObject::POLYLINE ||
        m_pActiveObject->GetType() == ASSISTANT::GenericObject::POLYGON))
   {
      EndCreateObjects();
   }
   
   if (m_iCurrentTool == iCurrentTool)
   {
      m_iCurrentTool = ID_TOOL_NOTHING;
      if (ASSISTANT::Project::active)
      {
         m_Pens->Active = false;
         m_Markers->Active = false;
         ASSISTANT::Project::active->ResetAnnotationMode();
      }
   }
   else
   {
      m_iCurrentTool = iCurrentTool;
      if (ASSISTANT::Project::active)
      {
         m_Markers->Active = false;
         m_Pens->Active = false;
         if (m_iCurrentTool != ID_TOOL_NOTHING)
         {
            if ( m_Markers->Find(m_iCurrentTool) ) 
            {
               m_Markers->Selected = m_iCurrentTool;
               m_Markers->Active = true;
            }
            else if ( m_Pens->Find(m_iCurrentTool) )
            {
               m_Pens->Selected = m_iCurrentTool;
               m_Pens->Active = true;
            }
            ASSISTANT::Project::active->SetAnnotationMode(m_iCurrentTool, 
               m_dLineWidth, m_iLineStyle, m_iArrowStyle, 
               m_clrLine, m_clrFill, m_bFillColor, m_clrText);
         }
         else
         {
            ASSISTANT::Project::active->ResetAnnotationMode();
         }
      }
   }
   SendMessage(WM_SETCURSOR, 0, 0);
   
}

HRESULT CAssistantView::EndCreateObjects()
{
   ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
   if (pPage)
   {
      pPage->SaveAsAOF();
      pPage->UnSelectObjects();
   }

   m_bCreateObjects = false;
   m_pActiveObject = NULL;
   return S_OK;
}

HRESULT CAssistantView::DeleteEmptyText()
{
   HRESULT hr = S_OK;

   CAssistantDoc *pDoc = (CAssistantDoc *)GetDocument();
   if (pDoc == NULL || !pDoc->HasLoadedDocuments() || !pDoc->HasActivePage())
      hr = E_FAIL;
   
   CArray<ASSISTANT::DrawObject *, ASSISTANT::DrawObject *> arSelectedObjects;
   pDoc->GetSelectedObjects(arSelectedObjects);
   if (arSelectedObjects.GetSize() == 1)
   {
      ASSISTANT::DrawObject *pObject = arSelectedObjects[0];
      if (pObject &&
         pObject->GetType() == ASSISTANT::DrawObject::GROUP && 
         ((ASSISTANT::Group *)pObject)->IsEmptyText())
      {
         pDoc->DeleteObjects(false);
      }
   }

   return hr;
}

BOOL CAssistantView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	return TRUE;
}


HRESULT CAssistantView::TranslateScreenToObjectCoordinates(CPoint &ptScreen, Gdiplus::PointF &ptObject)
{
   CAssistantDoc *pDoc = (CAssistantDoc *)GetDocument();
   if (pDoc == NULL || !pDoc->HasLoadedDocuments())
      return E_FAIL;
  
   CRect rcClient;
   GetClientRect(&rcClient);

   HRESULT hr = pDoc->TranslateScreenToObjectCoordinates(rcClient, ptScreen, ptObject);

   return hr;
}

HRESULT CAssistantView::TranslateScreenToObjectCoordinates(CRect &rcScreen, CRect &rcObject)
{
   CAssistantDoc *pDoc = (CAssistantDoc *)GetDocument();
   if (pDoc == NULL || !pDoc->HasLoadedDocuments())
      return E_FAIL;
  
   CRect rcClient;
   GetClientRect(&rcClient);

   HRESULT hr = pDoc->TranslateScreenToObjectCoordinates(rcClient, rcScreen, rcObject);

   return hr;
}

HRESULT CAssistantView::TranslateObjectToScreenCoordinates(CRect &rcObject, CRect &rcScreen)
{
   if (!ASSISTANT::Project::active->GetActivePage())
      return E_FAIL;

   CRect rcClient;
   GetClientRect(&rcClient);

   ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();

   float fZoomFactor = pPage->CalculateZoomFactor(rcClient);
   UINT nOffsetX = 0;
   UINT nOffsetY = 0;
   pPage->CalculateOffset(rcClient, fZoomFactor, nOffsetX, nOffsetY);

   rcScreen.left = floorf((rcObject.left * fZoomFactor) + nOffsetX);
   rcScreen.top = floorf((rcObject.top * fZoomFactor) + nOffsetY);
   rcScreen.right = floorf((rcObject.right * fZoomFactor) + nOffsetX);
   rcScreen.bottom = floorf((rcObject.bottom * fZoomFactor) + nOffsetY);

   return S_OK;
}

HRESULT CAssistantView::TranslateObjectToScreenCoordinates(CPoint &ptObject, CPoint &ptScreen)
{
   if (!ASSISTANT::Project::active->GetActivePage())
      return E_FAIL;

   CRect rcClient;
   GetClientRect(&rcClient);

   ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();

   float fZoomFactor = pPage->CalculateZoomFactor(rcClient);
   UINT nOffsetX = 0;
   UINT nOffsetY = 0;
   pPage->CalculateOffset(rcClient, fZoomFactor, nOffsetX, nOffsetY);

   ptScreen.x = (ptObject.x * fZoomFactor) + nOffsetX;
   ptScreen.y = (ptObject.y * fZoomFactor) + nOffsetY;

   return S_OK;
}

void CAssistantView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAssistantView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   if (m_bCreateObjects && m_pActiveObject)
   {
      ASSISTANT::DrawObject *pObject = m_pActiveObject;
      if (m_pActiveObject->GetType() == ASSISTANT::GenericObject::GROUP)
      {
         if (nChar == VK_RETURN)
         {
            if (ASSISTANT::Project::active)
            {
               ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
            }
         }
         else
            pObject = ((ASSISTANT::Group *)m_pActiveObject)->GetActive();
      }

      if (pObject->GetType() == ASSISTANT::GenericObject::ASCIITEXT)
      {
         CString csInput;
         for (int i = 0; i < nRepCnt; ++i)
            csInput += (_TCHAR)nChar;
         ((ASSISTANT::Text *)pObject)->InsertNewChars((LPCTSTR)csInput);
         RedrawWindow();
      }
   }
   CView::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CAssistantView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
   
   CAssistantDoc* pDoc = GetDocument();

   // start page is active
   if (pDoc == NULL)
      return TRUE;

   
   if (pDoc != NULL && pDoc->HasLoadedDocuments() && 
      pDoc->HasActivePage())
   {
      if (!pDoc->GetShowMouseCursor())
      {
         ::SetCursor(m_hNoCursor);
         return TRUE;
      }
    
      CPoint ptMouse;
      BOOL bResult = ::GetCursorPos(&ptMouse);
      // position of mouse in window
      ScreenToClient(&ptMouse);
      
      // position of mouse in window
      Gdiplus::PointF ptObject;
      TranslateScreenToObjectCoordinates(ptMouse, ptObject);
      
      CAssistantDoc *pDoc = (CAssistantDoc *)GetDocument();
      
      if (m_iCurrentTool == ID_TOOL_RECTANGLE)
      {
         ::SetCursor(m_hRectangleCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_TEXT)
      {
         ::SetCursor(m_hAreaSelection);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_OVAL)
      {
         ::SetCursor(m_hOvalCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_POLYLINE)
      {
         ::SetCursor(m_hPolylineCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_POLYGON)
      {
         ::SetCursor(m_hPolygonCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_FREEHAND)
      {
         ::SetCursor(m_hFreeHandCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_LINE)
      {
         ::SetCursor(m_hLineCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_MARKERBLUE)
      {
         ::SetCursor(m_hMarkerBlueCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_MARKERORANGE)
      {
         ::SetCursor(m_hMarkerOrangeCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_MARKERRED)
      {
         ::SetCursor(m_hMarkerRedCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_MARKERYELLOW)
      {
         ::SetCursor(m_hMarkerYellowCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_MARKERGREEN)
      {
         ::SetCursor(m_hMarkerGreenCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_MARKERMAGENTA)
      {
         ::SetCursor(m_hMarkerMagentaCursor);
         return TRUE;
      }
      
      
      if (m_iCurrentTool == ID_TOOL_PENBLACK)
      {
         ::SetCursor(m_hPenBlackCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_PENBLUE)
      {
         ::SetCursor(m_hPenBlueCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_PENCYAN)
      {
         ::SetCursor(m_hPenCyanCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_PENGREEN)
      {
         ::SetCursor(m_hPenGreenCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_PENMAGENTA)
      {
         ::SetCursor(m_hPenMagentaCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_PENRED)
      {
         ::SetCursor(m_hPenRedCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_PENWHITE)
      {
         ::SetCursor(m_hPenWhiteCursor);
         return TRUE;
      }
      
      if (m_iCurrentTool == ID_TOOL_PENYELLOW)
      {
         ::SetCursor(m_hPenYellowCursor);
         return TRUE;
      }
      
      
      if (m_iCurrentTool == ID_TOOL_TELEPOINTER)
      {
         if (m_bCreateObjects)
            ::SetCursor(m_hTelepointerCursor);
         else
            ::SetCursor(m_hSmallCrossCursor);
         return TRUE;
      }
      
      if (pDoc != NULL && ASSISTANT::Project::active != NULL)
      {
         
         ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
         if (pPage != NULL)
         {
            bool bIsHyperlinkPoint = pPage->IsHyperlinkPoint(ptObject);
            if (m_iCurrentTool == ID_TOOL_NOTHING && bIsHyperlinkPoint)
            {
               ::SetCursor(m_hHandCurosr);
               return TRUE;
            }

            UINT nEditType;
            int iResizePoint;
            bool bIsEditablePoint = pPage->IsEditablePoint(ptObject, nEditType, iResizePoint);
            if (bIsEditablePoint)
            {
               if (m_iCurrentTool == ID_TOOL_SELECT || m_iCurrentTool == ID_TOOL_NOTHING)
               {
                  bool bCursorChanged = false;
                  if (nEditType == ASSISTANT::DrawObject::SIZE_NWSE)
                  {
                     ::SetCursor(m_hEdit_nwse_Cursor);
                     bCursorChanged = true;
                  }
                  else if (nEditType == ASSISTANT::DrawObject::SIZE_NESW)
                  {
                     ::SetCursor(m_hEdit_nesw_Cursor);
                     bCursorChanged = true;
                  }
                  else if (nEditType == ASSISTANT::DrawObject::SIZE_WE)
                  {
                     ::SetCursor(m_hEdit_we_Cursor);
                     bCursorChanged = true;
                  }
                  else if (nEditType == ASSISTANT::DrawObject::SIZE_NS)
                  {
                     ::SetCursor(m_hEdit_ns_Cursor);
                     bCursorChanged = true;
                  }
                  
                  if (bCursorChanged)
                     return TRUE;
               }
               
               if (m_iCurrentTool == ID_TOOL_MOVE || m_iCurrentTool == ID_TOOL_NOTHING)
               {
                  if (m_iCurrentTool == ID_TOOL_NOTHING)
                  {
                     bool bObjectHasHyperlink = pPage->IsHyperlinkPoint(ptObject);
                     if (bObjectHasHyperlink)
                     {
                        ::SetCursor(m_hHandCurosr);
                        return TRUE;
                     }
                  }
                  if (nEditType == ASSISTANT::DrawObject::MOVE)
                  {
                     ::SetCursor(m_hMoveCursor);
                     return TRUE;
                  }
               }
               
               
               if (nEditType == ASSISTANT::DrawObject::MOVE &&
                  m_iCurrentTool == ID_TOOL_COPY)
               {
                  ::SetCursor(m_hCopyCursor);
                  return TRUE;
               }
               
            }
            else if ((m_iCurrentTool == ID_TOOL_NOTHING || m_iCurrentTool == ID_TOOL_COPY) &&
                     pPage->PointIsInObject(ptObject))
            {
               
               if (m_iCurrentTool == ID_TOOL_COPY)
               {
                  ::SetCursor(m_hCopyCursor);
                  return TRUE;
               }
               
               if (m_iCurrentTool == ID_TOOL_NOTHING)
               {
                  bool bObjectHasHyperlink = pPage->IsHyperlinkPoint(ptObject);
                  if (bObjectHasHyperlink)
                  {
                     ::SetCursor(m_hHandCurosr);
                     return TRUE;
                  }
               }
               ::SetCursor(m_hMoveCursor);
               return TRUE;
            }
         }
      }
   }

   return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CAssistantView::SBSDKOnNoTool()
{
   if (ASSISTANT::Project::active == NULL)
      return;

   ChangeTool(ID_TOOL_NOTHING);
}

void CAssistantView::SBSDKOnPen(int iLineWidth, 
                                float fTransperency, float fFillTransperency,
                                COLORREF clrLine, COLORREF clrFill)
{
   if (ASSISTANT::Project::active == NULL)
      return;

   int iTransperency = (int)(fTransperency * 255);
   bool bIsFilled = fFillTransperency > 0 ? true : false;

   int iCurrentTool;
   if (iTransperency == 255)
   {
      if (clrLine == RGB(255, 0, 0) ||
         clrLine == RGB(192, 0, 0))
         iCurrentTool = ID_TOOL_PENRED;
      else if (clrLine == RGB(0, 255, 0) ||
         clrLine == RGB(0, 157, 75))
         iCurrentTool = ID_TOOL_PENGREEN;
      else if (clrLine == RGB(0, 0, 255) ||
         clrLine == RGB(0, 0, 192))
         iCurrentTool = ID_TOOL_PENBLUE;
      else if (clrLine == RGB(0, 0, 0))
         iCurrentTool = ID_TOOL_PENBLACK;
      else if (clrLine == RGB(255, 255, 255))
         iCurrentTool = ID_TOOL_PENWHITE;
      else
         iCurrentTool = ID_TOOL_FREEHAND;
   }
   else
   {
      if (iTransperency >= 126 && iTransperency <= 128)
      {
         if (clrLine == RGB(255, 255, 0))
            iCurrentTool = ID_TOOL_MARKERYELLOW;
         else if (clrLine == RGB(255, 127, 0))
            iCurrentTool = ID_TOOL_MARKERORANGE;
         else if (clrLine == RGB(0, 255, 0))
            iCurrentTool = ID_TOOL_MARKERGREEN;
         else if (clrLine == RGB(255, 0, 255))
            iCurrentTool = ID_TOOL_MARKERMAGENTA;
         else
            iCurrentTool = ID_TOOL_FREEHAND;
      }
   }
   
   if (m_iCurrentTool != iCurrentTool)
   {
      ChangeTool(iCurrentTool);
   }

   if (iCurrentTool == ID_TOOL_PENRED ||
       iCurrentTool == ID_TOOL_PENGREEN ||
       iCurrentTool == ID_TOOL_PENBLUE ||
       iCurrentTool == ID_TOOL_PENBLACK ||
       iCurrentTool == ID_TOOL_PENWHITE )
   {
      m_nPencilWidth = iLineWidth;
   }
   else if (iCurrentTool == ID_TOOL_MARKERYELLOW ||
       iCurrentTool == ID_TOOL_MARKERORANGE ||
       iCurrentTool == ID_TOOL_MARKERGREEN ||
       iCurrentTool == ID_TOOL_MARKERMAGENTA)
   {
      m_nMarkerWidth = iLineWidth;
   }
   else
   {
      m_dLineWidth = iLineWidth;
      m_clrLine = clrLine;
      m_bFillColor = false;

      if (iTransperency != 255)
         m_nTransperency = iTransperency;
   }
   
}

void CAssistantView::SBSDKOnRectangle(int iLineWidth, 
                                      float fTransperency, float fFillTransperency,
                                      COLORREF clrLine, COLORREF clrFill)
{
   if (ASSISTANT::Project::active == NULL)
      return;

   int iTransperency = (int)(fTransperency * 255);
   bool bIsFilled = fFillTransperency > 0 ? true : false;

   m_dLineWidth = iLineWidth;
   m_clrLine = clrLine;
   m_nTransperency = iTransperency;
   m_clrFill = clrFill;
   if (bIsFilled)
      m_bFillColor = true;
   else
      m_bFillColor = false;

   m_nTransperency = iTransperency;
   
   if (m_iCurrentTool != ID_TOOL_RECTANGLE)
   {
      ChangeTool(ID_TOOL_RECTANGLE);
   }
}

void CAssistantView::SBSDKOnLine(int iLineWidth,
                                 float fTransperency, float fFillTransperency,
                                 COLORREF clrLine, COLORREF clrFill)
{
   if (ASSISTANT::Project::active == NULL)
      return;

   if (m_iCurrentTool != ID_TOOL_LINE)
   {
      ChangeTool(ID_TOOL_LINE);
   }

   bool bIsFilled = fFillTransperency > 0 ? true : false;

   m_clrFill = clrFill;
   if (bIsFilled)
      m_bFillColor = true;
   else
      m_bFillColor = false;
   
   m_clrLine = clrLine;

   m_dLineWidth = iLineWidth;

   m_nTransperency = (int)(fTransperency * 255);
}

void CAssistantView::SBSDKOnCircle(int iLineWidth,
                                   float fTransperency, float fFillTransperency,
                                   COLORREF clrLine, COLORREF clrFill)
{
   if (ASSISTANT::Project::active == NULL)
      return;

   int iTransperency = (int)(fTransperency * 255);
   m_clrLine = clrLine;
   
   bool bIsFilled = fFillTransperency > 0 ? true : false;
   m_clrFill = clrFill;
   if (bIsFilled)
      m_bFillColor = true;
   else
      m_bFillColor = false;

   m_nTransperency = iTransperency;
   if (m_iCurrentTool != ID_TOOL_OVAL)
   {
      ChangeTool(ID_TOOL_OVAL);
   }

}

void CAssistantView::OnRButtonUp(UINT nFlags, CPoint point) 
{
   CAssistantDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   
   if (pDoc != NULL && pDoc->HasLoadedDocuments() && 
       pDoc->HasActivePage())
   {
      if (pDoc->HasLoadedDocuments())
      {
         if (m_iCurrentTool == ID_TOOL_SIMPLENAVIGATION)
         {
            CMainFrameA *pMainFrame = GetMainFrame();
            if (pMainFrame != NULL) {
               pMainFrame->GoToPreviousPage();
            }
         }
      }
   }
   
   CView::OnRButtonUp(nFlags, point);
}

void CAssistantView::LeftMouseDown(CPoint &point)
{
   CAssistantDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   
   if (pDoc != NULL && pDoc->HasLoadedDocuments() && 
      pDoc->HasActivePage())
   {
      m_bMoveObjects = false;
      m_bSelectObjects = false;
      m_bInsertImage = false;
      m_bSelectTextArea = false;
      
      if (pDoc->HasLoadedDocuments())
      {
         CMainFrameA *pMainFrame = GetMainFrame();

         if (m_iCurrentTool == ID_TOOL_SELECT)
         {
            SelectObjects(MOUSE_DOWN, point);
         }
         else if (m_iCurrentTool == ID_TOOL_TEXT)
         {
            SelectTextArea(MOUSE_DOWN, point);
         }
         else if (m_iCurrentTool == ID_TOOL_COPY)
         {
            MoveObjects(MOUSE_DOWN, point);
         }
         else if (m_iCurrentTool == ID_TOOL_NOTHING)
         { 
            SelectMoveObjects(MOUSE_DOWN, point);
         }
         else
         {
            CreateObjects(MOUSE_DOWN, point);
         }
         if (pMainFrame != NULL)
            pMainFrame->HideTransparentButton();
      }
   }
}

void CAssistantView::LeftMouseMove(CPoint &point)
{
   CAssistantDoc *pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   if (pDoc != NULL && pDoc->HasLoadedDocuments() && 
       pDoc->HasActivePage())
   {
	  if (m_bSelectObjects)
      {
         SelectObjects(MOUSE_MOVE, point);
      }
      else if (m_bSelectTextArea)
      {
         SelectTextArea(MOUSE_MOVE, point);
      }
      else if (m_bMoveObjects)
      {
		  MoveObjects(MOUSE_MOVE, point);
      }
      else if (m_iCurrentTool == ID_TOOL_NOTHING)
      {
         
      }
      else if (m_bCreateObjects)
      {
         CreateObjects(MOUSE_MOVE, point);
      }
      else if (m_bInsertImage)
	  {
         Gdiplus::PointF ptObject;
         TranslateScreenToObjectCoordinates(point, ptObject);
		 
         if (ASSISTANT::Project::active != NULL)
         {
            ASSISTANT::Page *pPage = ASSISTANT::Project::active->GetActivePage();
            if (pPage)
            {
               ASSISTANT::DrawObject *pImage = pPage->GetActiveObject();
               if (pImage)
               {
                  pImage->SetX(ptObject.X);
                  pImage->SetY(ptObject.Y);
               }
            }
         }
         RedrawWindow();
      }
   }
}

void CAssistantView::LeftMouseUp(CPoint &point)
{
    CAssistantDoc *pDoc = GetDocument();

    ASSERT_VALID(pDoc);

    if (pDoc != NULL && pDoc->HasLoadedDocuments() && 
        pDoc->HasActivePage())
    {
        CMainFrameA *pMainFrame = GetMainFrame();

        if (m_iCurrentTool == ID_TOOL_SIMPLENAVIGATION)
        {
            if (pMainFrame != NULL) {
                pMainFrame->GoToNextPage();
            }
        } 
        else if (m_bSelectObjects)
        {
            SelectObjects(MOUSE_UP, point);
        }
        else if (m_bSelectTextArea)
        {
            SelectTextArea(MOUSE_UP, point);
        }
        else if (m_bMoveObjects)
        {
            MoveObjects(MOUSE_UP, point);
        }
        else if (m_iCurrentTool == ID_TOOL_NOTHING)
        {

        }
        else if (m_bCreateObjects)
        {
            CreateObjects(MOUSE_UP, point);
        }

        if (pMainFrame != NULL)
            pMainFrame->HideTransparentButton(false);
    } 
}
void CAssistantView::OnEnableGalleryButton(CCmdUI* pCmdUI)
{
   CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, CXTPControl::FromUI(pCmdUI));

   if (pGallery)
   {
      if ( m_Markers->Active == true )
         pGallery->SetCheckedItem(m_Markers->Selected);
      else if ( m_Pens->Active == true )
         pGallery->SetCheckedItem(m_Pens->Selected);
      else
      {
         pGallery->SetCheckedItem(ID_TOOL_NOTHING);
      }
   }

   pCmdUI->Enable();
}

void CAssistantView::OnButtonPen()
{
   m_Pens->Active = !m_Pens->Active;
   ChangeTool(m_Pens->Selected);
}

void CAssistantView::OnButtonMarker()
{
   m_Markers->Active = !m_Markers->Active;
   ChangeTool(m_Markers->Selected);
}

void CAssistantView::OnXTPGalleryMarkers(NMHDR* pNMHDR, LRESULT* pResult)
{
   NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
   CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);
   if (pGallery)
   {
      CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());
      if (pItem)
      {
         ChangeTool(pItem->GetID());

         CMainFrameA *pMainFrameA = GetMainFrame();
         if ( pMainFrameA == NULL )
            return;
#ifdef _STUDIO
         CMainFrame *pMainFrameStudio = (CMainFrame*)AfxGetMainWnd();
         if (pMainFrameStudio == NULL || pMainFrameStudio->GetRuntimeClass() != RUNTIME_CLASS(CMainFrame))
            return ;
         CXTPCommandBars* pCommandBars = pMainFrameStudio->GetCommandBars();
#endif

#ifndef _STUDIO
         CXTPCommandBars* pCommandBars = pMainFrameA->GetCommandBars();
#endif

         if ( pCommandBars != NULL )
         {
            int nMarkerIndex;
            UINT uiTabTools[] = {-1, -1,
               m_Markers->GetAt(4) == m_Markers->Selected ? ID_BUTTON_MARKERS : -1, // MARKER YELLOW
               m_Markers->GetAt(1) == m_Markers->Selected ? ID_BUTTON_MARKERS: -1,  // MARKER ORANGE
               m_Markers->GetAt(2) == m_Markers->Selected ? ID_BUTTON_MARKERS: -1,  // MARKER RED
               m_Markers->GetAt(3) == m_Markers->Selected ? ID_BUTTON_MARKERS: -1,  // MARKER GREEN
               m_Markers->GetAt(0) == m_Markers->Selected ? ID_BUTTON_MARKERS: -1,  // MARKER BLUE
               m_Markers->GetAt(5) == m_Markers->Selected ? ID_BUTTON_MARKERS: -1,  // MARKER MAGENTA
               -1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1,-1, -1, -1};
            nMarkerIndex=m_Markers->get_Selected();
            switch(nMarkerIndex)
            {
            case ID_TOOL_MARKERYELLOW: nMarkerIndex= 0;break;
            case ID_TOOL_MARKERORANGE: nMarkerIndex= 1;break; 
            case ID_TOOL_MARKERRED: nMarkerIndex= 2;break; 
            case ID_TOOL_MARKERGREEN: nMarkerIndex= 3;break;
            case ID_TOOL_MARKERBLUE: nMarkerIndex= 4;break; 
            case ID_TOOL_MARKERMAGENTA: nMarkerIndex= 5;break;
            }
            pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiTabTools, _countof(uiTabTools), CSize(32, 32));
            pCommandBars->RedrawCommandBars();

            pMainFrameA->SetPresentationBarMarker(nMarkerIndex);
         }
      }
      *pResult = TRUE; // Handled
   }
}

void CAssistantView::OnXTPGalleryPens(NMHDR* pNMHDR, LRESULT* pResult)
{
   NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
   CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);
   if (pGallery)
   {
      CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());
      if (pItem)
      {
         ChangeTool(pItem->GetID());

         CMainFrameA *pMainFrameA = GetMainFrame();
         if ( pMainFrameA == NULL )
            return;

#ifdef _STUDIO
         CMainFrame *pMainFrameStudio = (CMainFrame*)AfxGetMainWnd();
         if (pMainFrameStudio == NULL || pMainFrameStudio->GetRuntimeClass() != RUNTIME_CLASS(CMainFrame))
            return ;
         CXTPCommandBars* pCommandBars = pMainFrameStudio->GetCommandBars();
#endif

#ifndef _STUDIO
         CXTPCommandBars* pCommandBars = pMainFrameA->GetCommandBars();
#endif

         if ( pCommandBars != NULL )
         {
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
            nPenIndex=m_Pens->get_Selected();
            switch(nPenIndex)
            {
            case ID_TOOL_PENBLACK: nPenIndex= 0;break;
            case ID_TOOL_PENBLUE: nPenIndex= 1;break; 
            case ID_TOOL_PENCYAN: nPenIndex= 2;break; 
            case ID_TOOL_PENGREEN: nPenIndex= 3;break;
            case ID_TOOL_PENYELLOW: nPenIndex= 4;break; 
            case ID_TOOL_PENRED: nPenIndex= 5;break;
            case ID_TOOL_PENMAGENTA: nPenIndex= 6;break;
            case ID_TOOL_PENWHITE: nPenIndex= 7;break;
            }
            pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiTabTools, _countof(uiTabTools), CSize(32, 32));
            pCommandBars->RedrawCommandBars();

            pMainFrameA->SetPresentationBarPen(nPenIndex);
         }      
      }
      *pResult = TRUE; // Handled
   }
}

void CAssistantView::OnUpdateButtonPen(CCmdUI* pCmdUI)
{
   CXTPControl* pPensBtn = DYNAMIC_DOWNCAST(CXTPControl, CXTPControl::FromUI(pCmdUI));
   if (pPensBtn)
   {
      pPensBtn->SetChecked(m_Pens->Active);
      if ( m_Pens->Find(m_iCurrentTool) == true )
      {
         UpdatePencilIcon();
      }
   }
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if(GetMainFrame()!=NULL)
   {
      if (((pDoc!=NULL) && (pDoc->HasActivePage())) || 
          (ASSISTANT::Project::active != NULL && ASSISTANT::Project::active->isScreenGrabbingActive_== true))
      {
         bEnable = TRUE;
      }
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnUpdateButtonMarker(CCmdUI* pCmdUI)
{
   CXTPControl* pMarkersBtn = DYNAMIC_DOWNCAST(CXTPControl, CXTPControl::FromUI(pCmdUI));
   if (pMarkersBtn)
   {
      pMarkersBtn->SetChecked(m_Markers->Active);
      if ( m_Markers->Find(m_iCurrentTool) == true )
      {
         UpdateMarkerIcon();
      }
   }
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if (((pDoc!=NULL) && (pDoc->HasActivePage())) ||
       (ASSISTANT::Project::active != NULL && ASSISTANT::Project::active->isScreenGrabbingActive_== true))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}

// PRIVATE
void CAssistantView::UpdateMarkerIcon()
{
   CMainFrameA *pMainFrame = GetMainFrame();
   if (pMainFrame == NULL)
      return;
   // Get a pointer to the command bars object.
   /*CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
   if ( pCommandBars != NULL )
   {
      UINT uiTabTools[] = {-1, -1,
         m_Markers->GetAt(4) == m_Markers->Selected ? ID_BUTTON_MARKERS : -1, // MARKER YELLOW
         m_Markers->GetAt(1) == m_Markers->Selected ? ID_BUTTON_MARKERS : -1, // MARKER ORANGE
         m_Markers->GetAt(2) == m_Markers->Selected ? ID_BUTTON_MARKERS : -1, // MARKER RED
         m_Markers->GetAt(3) == m_Markers->Selected ? ID_BUTTON_MARKERS : -1, // MARKER GREEN
         m_Markers->GetAt(0) == m_Markers->Selected ? ID_BUTTON_MARKERS : -1, // MARKER BLUE
         m_Markers->GetAt(5) == m_Markers->Selected ? ID_BUTTON_MARKERS : -1, // MARKER MAGENTA
         -1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1,-1, -1, -1};
      pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiTabTools, _countof(uiTabTools), CSize(32, 32));
      pCommandBars->RedrawCommandBars();
   }*/
}

// PRIVATE
void CAssistantView::UpdatePencilIcon()
{
   CMainFrameA *pMainFrame = GetMainFrame();
   if (pMainFrame == NULL)
      return;

   // Get a pointer to the command bars object.
   /*CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
   if ( pCommandBars != NULL )
   {
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
      pCommandBars->GetImageManager()->SetIcons(IDB_TAB_TOOLS, uiTabTools, _countof(uiTabTools), CSize(32, 32));
      pCommandBars->RedrawCommandBars();
   }*/
}


void CAssistantView::OnUpdateViewPresentation(CCmdUI* pCmdUI)
{
   CAssistantDoc *pDoc = GetDocument();
   BOOL bEnable = TRUE;
   if(GetMainFrame()!=NULL)
   {
      if ((( pDoc != NULL )&&(pDoc->HasActivePage()==FALSE))||(GetMainFrame()->GetStartupPageVisibility() == true))
      {
         bEnable = FALSE;
      }
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnCheckDisplayGrid()
{
   CXTPControl *pControl = GetControlOnRibbonBar(ID_GROUP_GRID, ID_GRID_DISPLAY);
   if ( pControl != NULL )
   {
      pControl->SetChecked(!pControl->GetChecked());
      m_nGridDisplay = (UINT)pControl->GetChecked();
      UpdateGridChangesOnAllViews();
   }
}

void CAssistantView::OnUpdateDisplayGrid(CCmdUI* pCmdUI)
{
   CAssistantDoc *pDoc = GetDocument();
   BOOL bEnable = TRUE;
   if(GetMainFrame()!=NULL)
   {
   if ( pDoc != NULL )
   {
      pDoc->GetGridSettings(m_nGridSnap, m_nGridSpacing, m_nGridDisplay);
      pCmdUI->SetCheck(m_nGridDisplay);

         if ((pDoc->HasActivePage()==FALSE) || (GetMainFrame()->GetStartupPageVisibility() == true))
      {
         bEnable = FALSE;
      }
      }
      else
      {
         if (GetMainFrame()->GetShowStartupPage() == true)
         {
            bEnable = FALSE;
         }
      }
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnCheckSnapToGrid()
{
   CXTPControl *pControl = GetControlOnRibbonBar(ID_GROUP_GRID, ID_GRID_SNAP);
   if ( pControl != NULL )
   {
      pControl->SetChecked(!pControl->GetChecked());
      m_nGridSnap = pControl->GetChecked();
      UpdateGridChangesOnAllViews();
   }
}

void CAssistantView::OnUpdateSnapGrid(CCmdUI* pCmdUI)
{
   CAssistantDoc *pDoc = GetDocument();
   bool bIsStartupPage = false;
   BOOL bEnable = TRUE;
   if(GetMainFrame()!=NULL)
   {
   if ( pDoc != NULL )
   {
      pDoc->GetGridSettings(m_nGridSnap, m_nGridSpacing, m_nGridDisplay);
      pCmdUI->SetCheck(m_nGridSnap);

         if ((pDoc->HasActivePage()==FALSE)||(GetMainFrame()->GetStartupPageVisibility()==true))
      {
         bEnable = FALSE;
      }
      }
      else
      {
         if (GetMainFrame()->GetStartupPageVisibility()==true)
         {
            bEnable = FALSE;
         }
      } 
   }
   pCmdUI->Enable(bEnable);
}

void CAssistantView::OnUpdateGridSpacing(CCmdUI* pCmdUI)
{
	CXTPControlEdit* pControl = DYNAMIC_DOWNCAST(CXTPControlEdit, CXTPControl::FromUI(pCmdUI));
	if (pControl && !pControl->HasFocus())
	{
      GetDocument()->GetGridSettings(m_nGridSnap, m_nGridSpacing, m_nGridDisplay);

      pControl->SetEditText( FormatSpacing(m_nGridSpacing) );
	}
   BOOL bEnable = TRUE;
   CAssistantDoc *pDoc = GetDocument();
   if(GetMainFrame()!=NULL)
   {
      if (((pDoc!=NULL) && (pDoc->HasActivePage()==FALSE))||(GetMainFrame()->GetStartupPageVisibility()== true))
      {
         bEnable = FALSE;
      }
   }
   pCmdUI->Enable(bEnable);
}


void CAssistantView::OnGridSpacing(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
	CXTPControlEdit* pControl = DYNAMIC_DOWNCAST(CXTPControlEdit, tagNMCONTROL->pControl);

	if (pControl)
	{
      m_nGridSpacing = _ttoi(pControl->GetEditText());
      if ( m_nGridSpacing < GRID_MIN_SIZE || m_nGridSpacing > GRID_MAX_SIZE )
         GetDocument()->GetGridSettings(m_nGridSnap, m_nGridSpacing, m_nGridDisplay);
      UpdateGridChangesOnAllViews();
		*pResult = TRUE;
	}
}

CString CAssistantView::FormatSpacing(int nIndent)
{
	CString strSpacing;
	strSpacing.Format(_T("%i px"), (int)(nIndent));
	return strSpacing;
}

void CAssistantView::OnGridSpacingSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPUPDOWN* tagNMCONTROL = (NMXTPUPDOWN*)pNMHDR;
	CXTPControlEdit* pControl = DYNAMIC_DOWNCAST(CXTPControlEdit, tagNMCONTROL->pControl);

   if ( tagNMCONTROL->iDelta < 0 && m_nGridSpacing <= GRID_IDENT_SIZE
      || tagNMCONTROL->iDelta > 0 && m_nGridSpacing >= GRID_MAX_SIZE )
   {
      GetDocument()->GetGridSettings(m_nGridSnap, m_nGridSpacing, m_nGridDisplay);
      pControl->SetEditText( FormatSpacing( m_nGridSpacing ) );
      return;
   }

	if (pControl)
	{
      m_nGridSpacing  +=  tagNMCONTROL->iDelta * GRID_IDENT_SIZE;
      if ( m_nGridSpacing > GRID_MAX_SIZE )
         m_nGridSpacing = GRID_MAX_SIZE;
      if ( m_nGridSpacing < GRID_MIN_SIZE )
         m_nGridSpacing = GRID_MIN_SIZE;
		pControl->SetEditText( FormatSpacing( m_nGridSpacing ) );
      UpdateGridChangesOnAllViews();
	}
	*pResult = 1;
}

void CAssistantView::UpdateGridChangesOnAllViews()
{
   // Update changes on grid
   CAssistantDoc *pDoc = GetDocument();
   if ( pDoc != NULL )
   {
      pDoc->SetGridSettings(m_nGridSnap, m_nGridSpacing, m_nGridDisplay);
      pDoc->UpdateAllViews(NULL);
   }
}

CXTPControl *CAssistantView::GetControlOnRibbonBar(UINT nGroupId, UINT nControlId)
{
	CMainFrameA *pMainFrame = GetMainFrame();
	if (pMainFrame == NULL)
		return NULL;
	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
	if (pCommandBars != NULL)
	{
		CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)pCommandBars->GetMenuBar();
		if ( pRibbonBar != NULL )
		{
			CXTPRibbonGroup *pGroup = (CXTPRibbonGroup*)pRibbonBar->FindGroup(nGroupId);
			if ( pGroup != NULL )
			{
				return (CXTPControl*)pGroup->FindControl(nControlId);
         }
      }
   }
   return NULL;
}

void CAssistantView::OnEnableLabel(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}

// PRIVATE
CXTPRibbonBar* CAssistantView::GetRibbonBar()
{
#ifndef _STUDIO
    CMainFrameA *pMainFrame = GetMainFrame();
#else
    CMainFrame *pMainFrame = NULL;
    CWnd *pMainWnd = AfxGetMainWnd();
    if (pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
        pMainFrame = (CMainFrame *)pMainWnd;
#endif

    if (pMainFrame == NULL)
        return NULL;

    // Get a pointer to the command bars object.
    CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
    if (pCommandBars != NULL) {
        CXTPRibbonBar* pRibbonBar = DYNAMIC_DOWNCAST(CXTPRibbonBar, pCommandBars->GetMenuBar());
        ASSERT(pRibbonBar);
        return pRibbonBar;
    }
    return NULL;
}

 //PRIVATE
CMainFrameA* CAssistantView::GetMainFrame() {
#ifdef _STUDIO
    CMainFrameA *pMainFrame = CMainFrameA::GetCurrentInstance();

    return pMainFrame;
#else
   CWnd *pMainFrame = AfxGetMainWnd();
   if (pMainFrame == NULL || pMainFrame->GetRuntimeClass() != RUNTIME_CLASS(CMainFrameA))
      return NULL;
  
   return (CMainFrameA*)pMainFrame;
#endif
}
void CAssistantView::OnUpdateFullScreenCS(CCmdUI* pCmdUI) 
{
   // TODO: Add your command update UI handler code here   
   //pCmdUI->SetCheck(((m_nCurrentLayout == AssistantLayouts(FullscreenLayout)) || (m_nCurrentLayout == AssistantLayouts(FullscreenPagefocusLayout))) ? true : false);
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}
void CAssistantView::OnUpdateFullScreen(CCmdUI* pCmdUI) 
{
   // TODO: Add your command update UI handler code here   
   CMainFrameA *pMainFrame = GetMainFrame();
   if ( pMainFrame == NULL )
      return;
   
   pCmdUI->SetCheck(((pMainFrame->GetCurrentLayout() ==pMainFrame->GetFullscreenLayout()) || (pMainFrame->GetCurrentLayout() == pMainFrame->GetFullscreenPagefocusLayout())) ? true : false);
   BOOL bEnable = FALSE;
   CAssistantDoc *pDoc = GetDocument();
   if ((pDoc!=NULL) && (pDoc->HasActivePage()))
   {
      bEnable = TRUE;
   }
   pCmdUI->Enable(bEnable);
}