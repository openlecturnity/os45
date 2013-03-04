// NewDrawWindow.cpp: Implementierungsdatei
//

#include "global.h"
#include "avgrabber.h"
#include "resource.h"
#include "NewDrawWindow.h"


// CNewDrawWindow

IMPLEMENT_DYNAMIC(CNewDrawWindow, CWnd)

BEGIN_MESSAGE_MAP(CNewDrawWindow, CWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_MOUSEMOVE()
    ON_WM_CHAR()
    ON_WM_CONTEXTMENU()
    ON_WM_SETCURSOR()
    ON_COMMAND(IDC_DELETE_ALL, &CNewDrawWindow::OnDeleteAll)
END_MESSAGE_MAP()

CNewDrawWindow::CNewDrawWindow(AudioCapturer *pAC, CString csFileName)
{
    m_pAC = pAC;

    m_hTelepointer = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_TELEPOINTER));
    m_hTelepointerInactive = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_TELEPOINTER_INACTIVE));
    m_hText = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_TEXT));
    m_hLine = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_LINE));
    m_hRectangle = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_RECTANGLE));
    m_hOval = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_OVAL));
    m_hFreehand = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_FREEHAND));
    m_hPolyline = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_POLYLINE));
    m_hPolygon = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_POLYGON));
    
    m_hMarkerBlueCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_MARKER_BLUE));
    m_hMarkerOrangeCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_MARKER_ORANGE));
    m_hMarkerRedCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_MARKER_RED));
    m_hMarkerYellowCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_MARKER_YELLOW));
    m_hMarkerGreenCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_MARKER_GREEN));
    m_hMarkerMagentaCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_MARKER_MAGENTA));

    m_hPenBlackCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_PEN_BLACK));
    m_hPenBlueCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_PEN_BLUE));
    m_hPenCyanCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_PEN_CYAN));
    m_hPenGreenCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_PEN_GREEN));
    m_hPenMagentaCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_PEN_MAGENTA));
    m_hPenRedCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_PEN_RED));
    m_hPenWhiteCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_PEN_WHITE));
    m_hPenYellowCursor = LoadCursor(g_hDllInstance, MAKEINTRESOURCE(IDC_CURSOR_PEN_YELLOW));

    m_hBackgroundImage = NULL;
    m_pBackgroundDc = NULL;

    m_nTool           = DrawSdk::TEXT;
    m_iLineWidth      = 20;
    m_iLineStyle      = 0;
    m_iArrowStyle     = 0;
    m_iArrowConfig    = 0;
    m_clrFill         = RGB(0, 255, 255);
    m_clrPen          = RGB(255, 0, 0);
    m_bIsFilled       = true;
    m_bIsClosed       = false;

    m_logFont.lfHeight         = 20;
    m_logFont.lfWidth          = 0;
    m_logFont.lfEscapement     = 0;
    m_logFont.lfOrientation    = 0;
    m_logFont.lfWeight         = FW_NORMAL;
    m_logFont.lfItalic         = FALSE;
    m_logFont.lfUnderline      = FALSE;
    m_logFont.lfStrikeOut      = FALSE;
    m_logFont.lfCharSet        = ANSI_CHARSET;
    m_logFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
    m_logFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
    m_logFont.lfQuality        = DEFAULT_QUALITY;
    m_logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    _stprintf((TCHAR *) &m_logFont.lfFaceName, _T("Times New Roman"));

    m_bUseTelepointer = false;

    m_pFreehand = NULL;
    m_pPolygon = NULL;
    m_pLine = NULL; 
    m_pLinePoints = NULL;
    m_pRectangle = NULL;
    m_pOval = NULL;
    m_pText = NULL;
    m_pMarker = NULL;

    m_bLButtonDown = false;
    m_bRButtonDown = false;
    m_bIsPolygonMode = false;
    m_bIsTextMode = false;
    m_bIsPointerLaidDown = false;
    m_bIsPointerMode = false;

    m_ptCursorHotSpot.SetPoint(-1, -1);

    m_rcClip.SetRectEmpty();

    m_iUndoElementPos = 0;

    m_bRespectTempPoint = false;

    CUndoListEntry *pULE = 
            new CUndoListEntry(m_pAC ? m_pAC->getWaveInTimeMs() : 0,
            0, new int[0]);
    m_arUndoElements.Add(pULE);

	m_lObjectNumber=0;
	m_lLastTime=-1;
	CString evqName, objName;
    evqName = csFileName;
    evqName += _T(".evq");
	m_fpEvq = _tfopen(evqName, _T("wb"));
    objName = csFileName;
    objName += _T(".obj");
	m_fpObj = _tfopen(objName, _T("wb"));
	fprintf(m_fpObj, "<WB orient=bottomleft>\n");
}

CNewDrawWindow::~CNewDrawWindow()
{
    if (m_hTelepointer)
        DestroyCursor(m_hTelepointer);
    if (m_hTelepointerInactive)
        DestroyCursor(m_hTelepointerInactive);
    if (m_hText)
        DestroyCursor(m_hText);
    if (m_hLine)
        DestroyCursor(m_hLine);
    if (m_hRectangle)
        DestroyCursor(m_hRectangle);
    if (m_hOval)
        DestroyCursor(m_hOval);
    if (m_hFreehand)
        DestroyCursor(m_hFreehand);
    if (m_hPolyline)
        DestroyCursor(m_hPolyline);
    if (m_hPolygon)
        DestroyCursor(m_hPolygon);

    if (m_contextMenu.m_hMenu)
        m_contextMenu.DestroyMenu();

    if (m_hBackgroundImage)
        ::DeleteObject(m_hBackgroundImage);
    m_hBackgroundImage = NULL;

    if (m_pBackgroundDc) {
        m_pBackgroundDc->DeleteDC();
        delete m_pBackgroundDc;
    }
    m_pBackgroundDc = NULL;

    for (int i = 0; i < m_arDrawObjects.GetSize(); ++i)
        if (m_arDrawObjects[i] != NULL)
            delete m_arDrawObjects[i];
    m_arDrawObjects.RemoveAll();

    for (int i = 0; i < m_arUndoElements.GetSize(); ++i)
        if (m_arUndoElements[i] != NULL)
            delete m_arUndoElements[i];
    m_arUndoElements.RemoveAll();

    m_arObjectSet.RemoveAll();

	fprintf(m_fpObj, "</WB>\n");
	fflush(m_fpObj);
	fclose(m_fpObj);
    m_fpObj = NULL;
	fflush(m_fpEvq);
	fclose(m_fpEvq);
    m_fpEvq = NULL;
}




// CNewDrawWindow-Meldungshandler

int CNewDrawWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_contextMenu.LoadMenu(IDR_SG_DRAW_CONTEXT);

    return 0;
}

void CNewDrawWindow::OnPaint()
{
    if (m_pBackgroundDc == NULL)
        return;

    CPaintDC dc(this); // device context for painting

    CRect rcWindow;
    GetClientRect(&rcWindow);

    CXTMemDC memDC(&dc, rcWindow);

    CRect rcClip;
    dc.GetClipBox(&rcClip);
    CRgn rgnClip;
    rgnClip.CreateRectRgn(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
    memDC.SelectClipRgn(&rgnClip);
    rgnClip.DeleteObject();

    // Copy background to window
    memDC.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), 
                 m_pBackgroundDc, 0, 0, SRCCOPY);
        
   // Draw the other DrawSdk::DrawObject objects:
    for (int i = 0; i < m_arObjectSet.GetSize(); ++i) 
        m_arDrawObjects[m_arObjectSet[i]]->Draw(memDC.GetSafeHdc());

   // do we need to paint the telepointer?
   if (m_bIsPointerLaidDown) 
      memDC.DrawIcon(m_ptLast.x - m_ptCursorHotSpot.x, 
                     m_ptLast.y - m_ptCursorHotSpot.y, 
                     m_hTelepointer);
}

void CNewDrawWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_bLButtonDown = true;
    if (m_bIsPointerLaidDown)
        InvalidatePointer();
    m_bIsPointerLaidDown = false;

    if (m_bIsPolygonMode && (m_nTool != DrawSdk::POLYGON))
        EndCreatePolygon();

    if (m_bIsPolygonMode && m_pPolygon) {
        m_pPolygon->AddPoint(DrawSdk::DPoint(point.x, point.y));
        DoClip(point.x, point.y);
        m_ptLast.x = point.x;
        m_ptLast.y = point.y;
        m_ptTemp.y = point.x;
        m_ptTemp.y = point.y;
        return;
    }

    if (m_bIsTextMode)
        LeaveTextMode();

    switch (m_nTool) {
    case DrawSdk::RECTANGLE: 
        m_bRespectTempPoint = true;
        if (m_bIsFilled)
            m_pRectangle = 
                new DrawSdk::Rectangle(point.x, point.y, 1.0, 1.0, 
                                       m_clrPen, m_clrFill, (double)m_iLineWidth, 0);
        else
            m_pRectangle = 
                new DrawSdk::Rectangle(point.x, point.y, 1.0, 1.0,
                                       m_clrPen, (double)m_iLineWidth, 0);
        AddObject(m_pRectangle);
        m_ptTemp.x = point.x;
        m_ptTemp.y = point.y;
        break;
    case DrawSdk::OVAL:
        m_bRespectTempPoint = true;
        if (m_bIsFilled)
            m_pOval = 
                new DrawSdk::Oval(point.x, point.y, 1.0, 1.0, 
                                  m_clrPen, m_clrFill, (double)m_iLineWidth, 0);
        else
            m_pOval = 
                new DrawSdk::Oval(point.x, point.y, 1.0, 1.0, 
                                  m_clrPen, (double)m_iLineWidth, 0);
        AddObject(m_pOval);
        m_ptTemp.x = point.x;
        m_ptTemp.y = point.y;
        break;

    case DrawSdk::LINE:
        m_bRespectTempPoint = true;
        m_pLinePoints = new DrawSdk::DPoint[2];
        m_pLinePoints[0].x = point.x;
        m_pLinePoints[0].y = point.y;
        m_pLinePoints[1].x = point.x;
        m_pLinePoints[1].y = point.y;
        m_pLine = 
            new DrawSdk::Polygon(m_pLinePoints, 2, m_clrPen, 
                                 (double)m_iLineWidth, m_iLineStyle, m_iArrowStyle, m_iArrowConfig);
        AddObject(m_pLine);
        m_ptTemp.x = point.x;
        m_ptTemp.y = point.y;
        break;
    case DrawSdk::TEXT:
        m_bIsTextMode = true;
        m_csText.Empty();
        m_pText = new DrawSdk::Text(point.x, point.y, m_clrPen, m_csText, m_csText.GetLength(), &m_logFont);
        m_pText->SetDrawCursor();
        AddObject(m_pText);
        m_ptLast.x = point.x;
        m_ptLast.y = point.y;
        InvalidateObject(m_pText);
        break;
    case DrawSdk::POLYGON:
        {
            m_bRespectTempPoint = true;
            m_bIsPolygonMode = true;

            DrawSdk::DPoint p(point.x, point.y);
            if (m_bIsFilled)
                m_pPolygon = new DrawSdk::Polygon(&p, 1, m_clrPen, m_clrFill, (double)m_iLineWidth, 0);
            else
                m_pPolygon = new DrawSdk::Polygon(&p, 1, m_clrPen, (double)m_iLineWidth, m_iLineStyle, 
                                                 m_iArrowStyle, m_iArrowConfig, m_bIsClosed);
            AddObject(m_pPolygon);
            m_ptTemp.x = point.x;
            m_ptTemp.y = point.y;
        }
        break;
    case DrawSdk::MARKER:
        {
            DrawSdk::DPoint p(point.x, point.y);
            m_pMarker = new DrawSdk::Marker(&p, 1, m_clrPen, (double)m_iLineWidth);
            AddObject(m_pMarker);
        }
        break;
    case DrawSdk::FREEHAND:
        {
            DrawSdk::DPoint p(point.x, point.y);
            m_pFreehand = new DrawSdk::Polygon(&p, 1, m_clrPen, (double)m_iLineWidth);
            AddObject(m_pFreehand);
        }
        break;

    case DrawSdk::TELEPOINTER:
        m_bIsPointerMode = true;
        SetCursor(m_hTelepointer);
        InvalidatePointer();
        break;
    }

    m_ptLast.x = point.x;
    m_ptLast.y = point.y;

    SetCapture();

    CWnd::OnLButtonDown(nFlags, point);
}

void CNewDrawWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_bLButtonDown = false;

    switch (m_nTool) {
        case DrawSdk::RECTANGLE:
        case DrawSdk::OVAL:
            m_bRespectTempPoint = false;
            break;
        case DrawSdk::LINE:
            delete [] m_pLinePoints;
            m_pLinePoints = NULL;
            m_bRespectTempPoint = false;
            break;
        case DrawSdk::TEXT:
            break;
        case DrawSdk::POLYGON:
            break;
        case DrawSdk::MARKER:
            m_pMarker = NULL;
			m_csLastObjects.Add(m_lObjectNumber-1);
			break;
        case DrawSdk::FREEHAND:
            m_pFreehand = NULL;
			m_csLastObjects.Add(m_lObjectNumber-1);
            break;
        case DrawSdk::TELEPOINTER:
            m_bIsPointerMode = false;
            m_bIsPointerLaidDown = true;
            m_ptLast.x = point.x;
            m_ptLast.y = point.y;
            SetCursor(NULL);
            InvalidatePointer();
            break;
    }

    ReleaseCapture();

    CWnd::OnLButtonUp(nFlags, point);
}

void CNewDrawWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if (m_bIsPolygonMode)
        EndCreatePolygon();
    else
        OnLButtonDown(point.x, point.y);

    CWnd::OnLButtonDblClk(nFlags, point);
}

void CNewDrawWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	long lTime;
    if (m_bLButtonDown) {
        switch (m_nTool) {
            case DrawSdk::RECTANGLE:
                if (!m_pRectangle)
                    return;
                m_pRectangle->SetWidth(point.x - m_ptLast.x);
                m_pRectangle->SetHeight(point.y - m_ptLast.y);
                DoClip(point.x, point.y);
                m_ptTemp.x = point.x;
                m_ptTemp.y = point.y;
                break;
            case DrawSdk::OVAL:
                if (!m_pOval)
                    return;
                m_pOval->SetWidth(point.x - m_ptLast.x);
                m_pOval->SetHeight(point.y - m_ptLast.y);
                DoClip(point.x, point.y);
                m_ptTemp.x = point.x;
                m_ptTemp.y = point.y;
                break;
            case DrawSdk::LINE:
                if (!m_pLine)
                    return;
                m_pLinePoints[1].x = point.x;
                m_pLinePoints[1].y = point.y;
                m_pLine->SetPoints(m_pLinePoints, 2);
                DoClip(point.x, point.y);
                m_ptTemp.x = point.x;
                m_ptTemp.y = point.y;
                break;
            case DrawSdk::TEXT:
                break;
            case DrawSdk::POLYGON:
                break;
            case DrawSdk::MARKER:
                if (!m_pMarker)
                    return;
                m_pMarker->AddPoint(DrawSdk::DPoint(point.x, point.y));
                DoClip(point.x, point.y);
                m_ptLast.x = point.x;
                m_ptLast.y = point.y;
				lTime = m_pAC->getWaveInTimeMs();
				//minimum 50 ms between moves
				if(lTime>m_lLastTime+50)
				{
					m_lLastTime = lTime;
					//evq print time, pageNumber, 7, number of objects, object number
					fprintf(m_fpEvq,"%d 1 7 %d",lTime,m_csLastObjects.GetCount() + 1);
					for(int i=0;i<m_csLastObjects.GetCount();i++)
						fprintf(m_fpEvq," %d",m_csLastObjects[i]);
					fprintf(m_fpEvq," %d\n",m_lObjectNumber);
					m_pMarker->ToFile(m_fpObj);
					m_lObjectNumber ++;
				}
                break;
            case DrawSdk::FREEHAND:
                if (!m_pFreehand)
                    return;
                m_pFreehand->AddPoint(DrawSdk::DPoint(point.x, point.y));
                DoClip(point.x, point.y);
                m_ptLast.x = point.x;
                m_ptLast.y = point.y;
				lTime = m_pAC->getWaveInTimeMs();
				//minimum 50 ms between moves
				if(lTime>m_lLastTime+50)
				{
					m_lLastTime = lTime;
					//evq print time, pageNumber, 7, number of objects, object number
					fprintf(m_fpEvq,"%d 1 7 %d",lTime,m_csLastObjects.GetCount() + 1);
					for(int i=0;i<m_csLastObjects.GetCount();i++)
						fprintf(m_fpEvq," %d",m_csLastObjects[i]);
					fprintf(m_fpEvq," %d\n",m_lObjectNumber);
					m_pFreehand->ToFile(m_fpObj);
					m_lObjectNumber ++;
				}
                break;
        }
        RedrawWindow();
    }
    CWnd::OnMouseMove(nFlags, point);
}

void CNewDrawWindow::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (m_bIsTextMode) {

        // if we're deleting a character, use the old bounding box
        bool bUseOldBBox = false;

        // temporary solution: only append at the end:
        switch (nChar) {
            case VK_BACK:
                if (m_csText.GetLength() > 0)
                    m_csText = m_csText.Left(m_csText.GetLength() -1);
                bUseOldBBox = true;
                break;
            case 13:
                LeaveTextMode();
                break;
            default:
                if (nChar < 31 && nChar >= 0)   
                    break;
                m_csText += (_TCHAR)nChar;
                break;
        }

        if (bUseOldBBox) {
            CRect rcBounding;
            m_pText->GetLogicalBoundingBox(&rcBounding);
            m_pText->SetString(m_csText, m_csText.GetLength());
            InvalidateRect(&rcBounding, FALSE);
        } else {
            m_pText->SetString(m_csText, m_csText.GetLength());
            InvalidateObject(m_pText);
        }
    }
    else {
        TRACE("%d %d %d\n", nChar, nRepCnt, nFlags);
    }

    CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CNewDrawWindow::OnContextMenu(CWnd* pWnd, CPoint point)
{
    CMenu *pPopup = m_contextMenu.GetSubMenu(0);
    if (pPopup)
        pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

BOOL CNewDrawWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (m_nTool == DrawSdk::TELEPOINTER) {
        if (m_bIsPointerMode)
            SetCursor(m_hTelepointer);
        else
            SetCursor(m_hTelepointerInactive);
        return TRUE;
    } else if (m_nTool == DrawSdk::TEXT) {
        SetCursor(m_hText);
        return TRUE;
    } else if (m_nTool == DrawSdk::LINE) {
        SetCursor(m_hLine);
        return TRUE;
    } else if (m_nTool == DrawSdk::RECTANGLE) {
        SetCursor(m_hRectangle);
        return TRUE;
    } else if (m_nTool == DrawSdk::OVAL) {
        SetCursor(m_hOval);
        return TRUE;
    } else if (m_nTool == DrawSdk::FREEHAND) {
        if (m_clrPen == Gdiplus::Color::MakeARGB(255, 255, 0, 0))
            SetCursor(m_hPenRedCursor);
        else if (m_clrPen == Gdiplus::Color::MakeARGB(255, 0, 255, 0))
            SetCursor(m_hPenGreenCursor);
        else if (m_clrPen == Gdiplus::Color::MakeARGB(255, 0, 0, 255))
            SetCursor(m_hPenBlueCursor);
        else if (m_clrPen == Gdiplus::Color::MakeARGB(255, 255, 255, 255))
            SetCursor(m_hPenWhiteCursor);
        else if (m_clrPen == Gdiplus::Color::MakeARGB(255, 0, 255, 255))
            SetCursor(m_hPenCyanCursor);
        else if (m_clrPen == Gdiplus::Color::MakeARGB(255, 255, 0, 255))
            SetCursor(m_hPenMagentaCursor);
        else if (m_clrPen == Gdiplus::Color::MakeARGB(255, 0, 0, 0))
            SetCursor(m_hPenBlackCursor);
        else if (m_clrPen == Gdiplus::Color::MakeARGB(255, 255, 255, 0))
            SetCursor(m_hPenYellowCursor);
        else
            SetCursor(m_hFreehand);
        return TRUE;
    } else if (m_nTool == DrawSdk::MARKER) {
        if (m_clrPen == Gdiplus::Color::MakeARGB(127, 255, 255, 0))
            SetCursor(m_hMarkerYellowCursor);
        else if (m_clrPen == Gdiplus::Color::MakeARGB(127, 255, 0, 0))
            SetCursor(m_hMarkerRedCursor);
        else if (m_clrPen == Gdiplus::Color::MakeARGB(127, 0, 255, 0))
            SetCursor(m_hMarkerGreenCursor);
        else if (m_clrPen == Gdiplus::Color::MakeARGB(127, 255, 0, 255))
            SetCursor(m_hMarkerMagentaCursor);
        else if (m_clrPen == Gdiplus::Color::MakeARGB(127, 0, 255, 255))
            SetCursor(m_hMarkerBlueCursor);
        else if (m_clrPen == Gdiplus::Color::MakeARGB(127, 255, 128, 0))
            SetCursor(m_hMarkerOrangeCursor);
        else
            SetCursor(m_hFreehand);
        return TRUE;
    } else if (m_nTool == DrawSdk::POLYGON) {
        if (m_bIsClosed || m_bIsFilled)
            SetCursor(m_hPolygon);
        else
            SetCursor(m_hPolyline);
        return TRUE;
    }

    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CNewDrawWindow::OnDeleteAll()
{
    m_arObjectSet.RemoveAll();
    if (m_iUndoElementPos + 1 == m_arUndoElements.GetSize()) {
        CUndoListEntry *pULE = 
            new CUndoListEntry(m_pAC ? m_pAC->getWaveInTimeMs() : 0, 
                               0, new int[0]);
        m_arUndoElements.Add(pULE);
        m_iUndoElementPos++;
    } else {
        ++m_iUndoElementPos;
        CUndoListEntry *temp = m_arUndoElements[m_iUndoElementPos];
        m_arUndoElements[m_iUndoElementPos] = 
            new CUndoListEntry(m_pAC ? m_pAC->getWaveInTimeMs() : 0, 
                               0, new int[0]);
        delete temp;
    }

    InvalidateRect(NULL, FALSE);
}


// public functions

void CNewDrawWindow::SetTopLeft(int x, int y)
{
    CRect rcWindow;
    GetWindowRect(&rcWindow);

    SetWindowPos(NULL, x, y, rcWindow.Width(), rcWindow.Height(), SWP_NOZORDER);
}

void CNewDrawWindow::SelectTool(int nTool)
{
    if (m_bIsPolygonMode)
        EndCreatePolygon();

    if (m_bIsPointerLaidDown) {
        m_bIsPointerLaidDown = false;
        InvalidatePointer();
    }

    m_nTool = nTool;
}

void CNewDrawWindow::SetFont(LOGFONT *pLogFont) {
    memcpy(&m_logFont, pLogFont, sizeof LOGFONT);
}

void CNewDrawWindow::SetPenColor(Gdiplus::ARGB color) { 
    m_clrPen = color; 
}

void CNewDrawWindow::SetFillColor(Gdiplus::ARGB color) { 
    m_clrFill = color; 
}

void CNewDrawWindow::SetFilled(bool bIsFilled) { 
    m_bIsFilled = bIsFilled; 
}

void CNewDrawWindow::SetClosed(bool bIsClosed) { 
    if (m_bIsPolygonMode)
        EndCreatePolygon();

    m_bIsClosed = bIsClosed; 
}

void CNewDrawWindow::SetLineWidth(int iLineWidth) { 
    m_iLineWidth = iLineWidth; 
}

void CNewDrawWindow::SetLineStyle(int iLineStyle) { 
    m_iLineStyle = iLineStyle; 
}

void CNewDrawWindow::SetArrowStyle(int iArrowStyle) { 
    m_iArrowStyle = iArrowStyle; 
}

void CNewDrawWindow::SetArrowConfig(int iArrowConfig) { 
    m_iArrowConfig = iArrowConfig; 
}

void CNewDrawWindow::Show() {
    GetBackground();
    ShowWindow(SW_SHOW);
    UpdateWindow();
	m_csLastObjects.RemoveAll();
	fprintf(m_fpEvq,"0 1 7 0\n");
}

void CNewDrawWindow::Hide() {
    ShowWindow(SW_HIDE);
}

void CNewDrawWindow::Undo() {
    if (m_iUndoElementPos < 1)
        return;

    --m_iUndoElementPos;
    m_arObjectSet.RemoveAll();

    CUndoListEntry *pULE = m_arUndoElements[m_iUndoElementPos];
    for (int i = 0; i < pULE->listSize; ++i)
        m_arObjectSet.Add(pULE->objectList[i]);

    InvalidateRect(NULL, FALSE);
}

long CNewDrawWindow::GetLastUndoTime() {
    if (!m_pAC)
        return -1;

    if (m_iUndoElementPos <= 1)
        return -1;

    return m_arUndoElements[m_iUndoElementPos]->timeStamp;
}

void CNewDrawWindow::Redo() {
    if (m_arUndoElements.GetSize() - 1 <= m_iUndoElementPos)
        return; // not possible

    ++m_iUndoElementPos;
    m_arObjectSet.RemoveAll();

    CUndoListEntry *pULE = m_arUndoElements[m_iUndoElementPos];
    for (int i = 0; i < pULE->listSize; ++i)
        m_arObjectSet.Add(pULE->objectList[i]);

    InvalidateRect(NULL, FALSE);
}

long CNewDrawWindow::GetNextRedoTime() {
    if (!m_pAC)
        return -1;

    if (m_iUndoElementPos + 1 >= m_arUndoElements.GetSize())
        return -1; // no redo possible

    return m_arUndoElements[m_iUndoElementPos + 1]->timeStamp;
}

//// private

void CNewDrawWindow::AddObject(DrawSdk::DrawObject *pObject) {
    m_arDrawObjects.Add(pObject);
    m_arObjectSet.Add(m_arDrawObjects.GetSize() - 1);

    int *aObjects = new int[m_arObjectSet.GetSize()];
    for (int i = 0; i < m_arObjectSet.GetSize(); ++i)
        aObjects[i] = m_arObjectSet[i];

    if (m_iUndoElementPos + 1 == m_arUndoElements.GetSize()) {
        // push new state into undo list
        CUndoListEntry *pULE = 
            new CUndoListEntry(m_pAC ? m_pAC->getWaveInTimeMs() : 0, 
                               m_arObjectSet.GetSize(), aObjects);
        m_arUndoElements.Add(pULE);
        ++m_iUndoElementPos;
    }
    else {
        // redos have been performed, overwrite a position.
        ++m_iUndoElementPos;
        CUndoListEntry *temp = m_arUndoElements[m_iUndoElementPos];
        m_arUndoElements[m_iUndoElementPos] = 
            new CUndoListEntry(m_pAC ? m_pAC->getWaveInTimeMs() : 0, 
                                m_arObjectSet.GetSize(), aObjects);
        delete temp;
    }
}

void CNewDrawWindow::EndCreatePolygon() {
    if (m_pPolygon != NULL && m_pPolygon->GetIsAutoClose()) {
        m_ptLast.x = m_pPolygon->GetX();
        m_ptLast.y = m_pPolygon->GetY();
        m_ptTemp.x = m_ptLast.x;
        m_ptTemp.y = m_ptLast.y;
        DoClip((int)(m_ptLast.x + m_pPolygon->GetWidth()), 
            (int)(m_ptLast.y + m_pPolygon->GetHeight()));
    }

    m_bIsPolygonMode = false;
    m_bRespectTempPoint = false;
}

void CNewDrawWindow::LeaveTextMode()
{
    m_bIsTextMode = false;
    m_pText->SetDrawCursor(false);
    InvalidateObject(m_pText);
}

void CNewDrawWindow::InvalidateObject(DrawSdk::DrawObject *pObject)
{
   CRect rcClip;
   pObject->GetLogicalBoundingBox(&rcClip);

   InvalidateRect(&rcClip, FALSE);
   RedrawWindow();
}

void CNewDrawWindow::InvalidatePointer() {
    if (m_hTelepointer == NULL)
        return;

    if (m_ptCursorHotSpot.x == -1) {
        // hot spot has not been retrieved
        ICONINFO iconInfo;
        BOOL success = ::GetIconInfo(m_hTelepointer, &iconInfo);
        if (success == FALSE)
            return;

        m_ptCursorHotSpot.x = iconInfo.xHotspot;
        m_ptCursorHotSpot.y = iconInfo.yHotspot;
    }

    CRect rcClip;
    rcClip.left   = m_ptLast.x - m_ptCursorHotSpot.x;
    rcClip.top    = m_ptLast.y - m_ptCursorHotSpot.y;
    rcClip.right  = rcClip.left + 32;
    rcClip.bottom = rcClip.top + 32;

    InvalidateRect(&rcClip, FALSE);
    RedrawWindow();
}

void CNewDrawWindow::GetBackground() {
    if (m_hBackgroundImage != NULL)
        ::DeleteObject(m_hBackgroundImage);
    m_hBackgroundImage = NULL;

    if (m_pBackgroundDc) {
        m_pBackgroundDc->DeleteDC();
        delete m_pBackgroundDc;
    }
    m_pBackgroundDc = NULL;

    CRect rcWindow;
    GetWindowRect(&rcWindow);

    CDC *pDC = GetDesktopWindow()->GetDC();
    m_pBackgroundDc = new CDC();
    m_hBackgroundImage = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), rcWindow.Width(), rcWindow.Height());
    m_pBackgroundDc->CreateCompatibleDC(pDC);
    m_pBackgroundDc->SelectObject(m_hBackgroundImage);
    m_pBackgroundDc->BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(),
                            pDC, rcWindow.left, rcWindow.top,
                            SRCCOPY);

 
    GetDesktopWindow()->ReleaseDC(pDC);
}

void CNewDrawWindow::DoClip(int x, int y) {
    if (m_bRespectTempPoint) {
        m_rcClip.left = m_ptLast.x;
        m_rcClip.top = m_ptLast.y;
        m_rcClip.right = x;
        m_rcClip.bottom = y;
        m_rcClip.NormalizeRect();
    }
    else {
        m_rcClip.left = m_ptLast.x;
        m_rcClip.right = m_ptLast.x;
        if (x < m_rcClip.left)
            m_rcClip.left = x;
        else // x >= lastPoint_.x
            m_rcClip.right = x;
        if (m_ptTemp.x < m_rcClip.left)
            m_rcClip.left = m_ptTemp.x;
        else {
            if (m_ptTemp.x > m_rcClip.right)
                m_rcClip.right = m_ptTemp.x;
        }
        m_rcClip.top = m_ptTemp.y;
        m_rcClip.bottom = m_ptTemp.y;
        if (y < m_ptTemp.y)
            m_rcClip.top = y;
        else
            m_rcClip.bottom = y;
        if (m_ptTemp.y < m_rcClip.top)
            m_rcClip.top = m_ptTemp.y;
        else {
            if (m_ptTemp.y > m_rcClip.bottom)
                m_rcClip.bottom = m_ptTemp.y;
        }
    }

    m_rcClip.left -= m_iLineWidth;
    m_rcClip.right += m_iLineWidth;
    m_rcClip.top -= m_iLineWidth;
    m_rcClip.bottom += m_iLineWidth;

    InvalidateRect(&m_rcClip, FALSE);
}
