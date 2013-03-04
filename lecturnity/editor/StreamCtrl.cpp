// StreamCtrl.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "StreamCtrl.h"
#include "editorDoc.h"
#include "InsertStopmarkDialog.h"
#include "InsertTargetmarkDialog.h"
#include "MainFrm.h"
#include "PageProperties.h"
#include "WhiteboardView.h"
#include "InteractionStream.h"

#ifdef _STUDIO
#include "..\Studio\Studio.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CAPTION_SPACING 4
#define CAPTION_COLOR RGB(85, 85, 85)

#define MIN_TIMELINE_WIDTH 9

/////////////////////////////////////////////////////////////////////////////
// CStreamCtrl


BEGIN_MESSAGE_MAP(CStreamCtrl, CWnd)
    //{{AFX_MSG_MAP(CStreamCtrl)
    //ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_COMMAND(ID_CLIP_PROPERTIES, OnClipProperties)
    ON_COMMAND(ID_CLIP_REMOVE, OnClipRemove)
    ON_UPDATE_COMMAND_UI(ID_CLIP_REMOVE, OnUpdateClipRemove)
    ON_COMMAND(ID_VIDEO_PROPERTIES, OnVideoProperties)
    ON_COMMAND(ID_REMOVE_VIDEO, OnVideoRemove)
    ON_COMMAND(ID_PAGE_PROPERTIES, OnPageProperties)
    ON_COMMAND(ID_AUDIO_PROPERTIES, OnAudioProperties)
    ON_COMMAND(ID_MARK_PAGE, OnMarkPage)
    ON_COMMAND(ID_CLIP_MARK, OnMarkClip)
    ON_COMMAND(ID_CLIP_TITLE, OnChangeClipTitle)
    ON_WM_SETCURSOR()
    ON_WM_CONTEXTMENU()
    ON_COMMAND(IDC_INSERT_STOPMARK, OnInsertStopmark)
    ON_COMMAND(IDC_REMOVE_STOPMARK, OnRemoveMark)
    ON_COMMAND(IDC_REMOVE_ALL_DEMO_OBJECTS, OnRemoveAllDemoDocumentObjects)
    ON_COMMAND(IDC_INSERT_TARGETMARK, OnInsertTargetmark)
    ON_COMMAND(IDC_TARGETMARK_PROPERTIES, OnTargetmarkProperties)
    ON_COMMAND(IDC_REMOVE_TARGETMARK, OnRemoveMark)
    ON_COMMAND(ID_INSERT_DEMO_OBJECT, OnInsertDemoObject)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CStreamCtrl::CStreamCtrl() {
    m_pEditorDoc = NULL;
    FontManipulation::CreateDefaultFont(m_logFont);

    // variables needed for moving marks and clips
    m_markStartPixel = -1;
    m_lastPixel = -1;
    m_lastMs = -1;
    m_iButtonStatus = BUTTON_UP;

	//variables needed for timeline selected object move/resize
	m_bMoveSelObj = false;
	m_bResizeLeftSelObj=false;
	m_bResizeRightSelObj=false;
	m_pUsedArea = NULL;

    // variable to handle mark stream
    m_rcMarksStreamRect.SetRectEmpty();
    m_bMoveMark = false;
    m_bMouseMoveOutOfMarksStream = false;
    m_bShowMarksStream  = false;
    m_bMoveDemoMark = false;
    m_iDemoMarkOrigMs = -1;;

    // variable to handle video stream
    m_rcVideoStreamRect.SetRectEmpty();
    m_bShowVideoStream  = false;
    m_bmpBackground.LoadBitmap(IDB_VIDEO_STREAM_BG_BLUE);

    // variable to handle clip stream
    m_rcClipStreamRect.SetRectEmpty();
    m_bMoveClip = false;
    m_bMoveAudioClip = false;
    m_bShowClipsStream  = false;

    // variable to handle page stream
    m_rcSlidesStreamRect.SetRectEmpty();
    m_bShowSlidesStream = false;

    // variable to handle audio stream
    m_rcAudioStreamRect.SetRectEmpty();
    m_bShowAudioStream  = true;

    // variables to hold information for event handling
    m_nContextmenuPixelPos = 0;
    m_nContextmenuTimestamp = 0;

    // mouse cursors
    m_hSelectCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_SELECTION);
    m_hMoveCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE_E);
    m_hDeleteCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_DELETE);
	m_hResizeHCursor = LoadCursor(NULL, IDC_SIZEWE);;

    m_pImageBackgroundVideo=NULL;
    m_pImageBackgroundSlides=NULL;
    m_pImageBackgroundClip=NULL;
    m_pImageBackgroundAudio=NULL;
    m_pImageBackgroundCaptionStream=NULL;
    m_pImageBackgroundStream=NULL;

    m_bBackgrounds = LoadBackgroundImages();
    m_bIconVideo   = LoadIconForStream(IDR_TIMELINE_ICON_VIDEO);
    m_bIconSlides  = LoadIconForStream(IDR_TIMELINE_ICON_SLIDES);
    m_bIconCaption =  LoadIconForStream(IDR_TIMELINE_VIDEO_SCREEN);

    m_penLight.CreatePen(PS_SOLID, 1, RGB(235, 176, 57/*224, 246, 192*/));
    m_penMiddle.CreatePen(PS_SOLID, 1, RGB(204, 153, 49/*164, 228, 63*/));
    m_penDark.CreatePen(PS_SOLID, 1, RGB(235, 176, 57/*53, 82, 11*/));
    //Create Audio Waveform pen
    m_cpAudioWavePen.CreatePen(PS_SOLID, 1, RGB(53, 76, 112));
    
}

void CStreamCtrl::ColorSchemeChanged() {
    m_bBackgrounds = LoadBackgroundImages();
    if (m_bBackgrounds == false) {
        m_pImageBackgroundVideo=NULL;
        m_pImageBackgroundSlides=NULL;
        m_pImageBackgroundClip=NULL;
        m_pImageBackgroundAudio=NULL;
        m_pImageBackgroundCaptionStream=NULL;
        m_pImageBackgroundStream=NULL;
    }
    m_bIconVideo   = LoadIconForStream(IDR_TIMELINE_ICON_VIDEO);
    m_bIconSlides  = LoadIconForStream(IDR_TIMELINE_ICON_SLIDES);
    m_bIconCaption =  LoadIconForStream(IDR_TIMELINE_VIDEO_SCREEN);
}

CStreamCtrl::~CStreamCtrl() {
    m_arSelectedClips.RemoveAll();
    m_arClipStreamRect.RemoveAll();
    m_arClipAudioRect.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CStreamCtrl 

int CStreamCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CWnd::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    // contextmenus for marks stream
    m_marksPopup.LoadMenu(IDR_MARKS_CONTEXT);
    m_stopmarkPopup.LoadMenu(IDR_STOPMARK_CONTEXT);
    m_targetmarkPopup.LoadMenu(IDR_TARGETMARK_CONTEXT);

    // contextmenu for video stream
    m_videoPopup.LoadMenu(IDR_VIDEO_PROPERTIES);

    // bitmaps for clip stream
    m_clipAudioNote.CreateFromIcon(IDI_NOTEICON);

    // contextmenu for clipstream
    m_clipPopup.LoadMenu(IDR_CLIP_PROPERTIES);

    // contextmenu for clipstream: RemoveClip is not possible in LiveContext edit mode
    bool bEnable = true;
    if (CStudioApp::IsLiveContextMode() && !CStudioApp::IsLiveContextPreview()) {
        bEnable = false;
    }
    if (!bEnable) {
        m_clipPopup.EnableMenuItem(ID_CLIP_REMOVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }

    // contextmenu for slide stream
    m_pagePopup.LoadMenu(IDR_PAGE_PROPERTIES);
    // contextmenu for slide stream if document is screengrabbing without structure
    m_createStructPopup.LoadMenu(IDR_CREATESTRUCTURE_CONTEXT);

    // contextmenu for audio stream
    m_audioPopup.LoadMenu(IDR_AUDIO_PROPERTIES);

    // initialization
    m_cursorPositionPixel = 0;
    return 0;
}

void CStreamCtrl::OnSize(UINT nType, int cx, int cy) {
    if (cx > 0 && cy > 0) {
        if (NULL != m_doubleBuffer.m_hObject) {
            m_doubleBufferDC.SelectObject(m_pOldObject);
            m_doubleBuffer.DeleteObject();
        }
        if (NULL != m_doubleBufferDC.m_hDC) {
            m_doubleBufferDC.DeleteDC();
        }

        CDC *pDC = GetDC();
        m_doubleBuffer.CreateCompatibleBitmap(pDC, cx, cy);
        m_doubleBufferDC.CreateCompatibleDC(pDC);
        m_pOldObject = m_doubleBufferDC.SelectObject(&m_doubleBuffer);
        ReleaseDC(pDC);
    }
}

void CStreamCtrl::DrawItem(CRect &rcClient, CDC *pDC) {
    m_arClipStreamRect.RemoveAll();
    m_arClipAudioRect.RemoveAll();
    if (m_pEditorDoc && !m_pEditorDoc->project.IsEmpty()) {
        CalculateStreamRectangles();

        m_pImageBackgroundStream->DrawImage(pDC, rcClient, m_pImageBackgroundStream->GetSource(), CRect(5, 5, 5, 5), COLORREF_NULL);

        CRect rcCaption(rcClient.left, rcClient.top + 1, rcClient.left + LEFT_BORDER, rcClient.top + 1);

        // mark stream
        if (m_bShowMarksStream) {
            rcCaption.bottom = rcCaption.top + STREAM_HEIGHT;
            CRect rcMarksStreamRect(rcCaption.right, rcCaption.top, rcClient.right, rcCaption.top + STREAM_HEIGHT);
            CMarkStream *pMarkStream = m_pEditorDoc->project.GetMarkStream();
            if (pMarkStream) {
                pMarkStream->Draw(pDC, rcMarksStreamRect, m_pEditorDoc->m_displayStartMs, 
                    m_pEditorDoc->m_displayEndMs - m_pEditorDoc->m_displayStartMs + 1);
            }
            DrawStreamCaption(pDC, rcCaption, IDS_STREAM_MARKS);
            rcCaption.top += STREAM_HEIGHT;
        }

        // slides stream
        if (m_bShowSlidesStream) {
            rcCaption.bottom = rcCaption.top + STREAM_HEIGHT * 3;
			DrawSlidesStream(pDC, rcCaption.right, rcCaption.top, rcClient.right, rcCaption.bottom);
			DrawSlidesStreamCaption(pDC,rcCaption);
            rcCaption.top += STREAM_HEIGHT*3;
        }

        // video stream
        if (m_bShowVideoStream) {
            rcCaption.bottom = rcCaption.top + STREAM_HEIGHT;
            DrawVideoStream(pDC, rcCaption.right, rcCaption.top, rcClient.right, rcCaption.top + STREAM_HEIGHT);
            DrawStreamCaption(pDC, rcCaption, IDS_STREAM_VIDEO);
            if ((m_bIconCaption==true)&&(m_pIconCaption!=NULL)) {
                CSize sz(16, 16);
                // set drawing point
                CPoint pt(rcCaption.right-m_iconHandle.GetExtent().cx-15,rcCaption.top+4);
                // draw an image (icon)
                m_pIconCaption->Draw(pDC, pt, m_pIconCaption->GetIcon(), sz);
            }
            rcCaption.top += STREAM_HEIGHT;
        }

        // clip stream
        if (m_bShowClipsStream) {
            rcCaption.bottom = rcCaption.top + STREAM_HEIGHT;
            DrawClipsStream(pDC, rcCaption.right, rcCaption.top, rcClient.right, rcCaption.top + STREAM_HEIGHT, rcClient.top);
            DrawStreamCaption(pDC, rcCaption, IDS_STREAM_CLIPS);
            if ((m_bIconCaption==true)&&(m_pIconCaption!=NULL)) {
                CSize sz(16, 16);
                // set drawing point
                CPoint pt(rcCaption.right-m_iconHandle.GetExtent().cx-15,rcCaption.top+4);
                // draw an image (icon)
                m_pIconCaption->Draw(pDC, pt, m_pIconCaption->GetIcon(), sz);
            }
            rcCaption.top += STREAM_HEIGHT;
        }

        // audio stream
        if (m_bShowAudioStream) {
            rcCaption.bottom = rcClient.bottom;
            DrawAudioStream(pDC, rcCaption.right, rcCaption.top -1, rcClient.right, rcClient.bottom);
            //if (m_arClipAudioRect.GetSize() > 0)
            if (m_arClipStreamRect.GetSize() > 0){
                CDC dc2;
                dc2.CreateCompatibleDC(pDC);
                CBitmap b;
                b.CreateCompatibleBitmap(&dc2, 1, 1);
                CBitmap *pOldBitmap = dc2.SelectObject(&b);

                BLENDFUNCTION bf;
                bf.BlendOp = AC_SRC_OVER;
                bf.BlendFlags = 0;
                bf.SourceConstantAlpha = 0x3f;
                bf.AlphaFormat = 0;

                for (int c=0; c<m_arClipStreamRect.GetSize(); ++c) {
                    // TODO What to do when zoomed?
                    CRect rcClip = m_arClipStreamRect[c];
                    CClipStreamR *pClipStream = m_pEditorDoc->project.GetClipStream();
                    if (pClipStream->HasClipAudioAt(GetMsecFromPixel(rcClip.left - LEFT_BORDER + 1))) {
                        // TODO remove +1 due to round down scale up problem with positions and pixels
                        // TODO Use a better mechanism than HasClipAudioAt()
                        AlphaBlend(pDC->m_hDC, rcClip.left, rcCaption.top + 1, rcClip.right-rcClip.left, 9, dc2.m_hDC, 0, 0, 1, 1, bf);
                        CRect rcNoteRect(rcClip.left + 2, rcCaption.top + 3, rcClip.left + 18, rcCaption.top + 19);
                        m_clipAudioNote.Render(pDC, &rcNoteRect, &rcClient);
                    }
                }

                dc2.SelectObject(pOldBitmap);
                b.DeleteObject();
                dc2.DeleteDC();
            }

            DrawStreamCaption(pDC, rcCaption, IDS_STREAM_AUDIO);
        }

        // draw selection rectangle
        if (m_pEditorDoc->m_markStartMs != -1 && m_pEditorDoc->m_markEndMs != -1) {
            if (rcClient.Width() > 0) {
                int x1 = GetPixelFromMsec(m_pEditorDoc->m_markStartMs, m_pEditorDoc->m_displayStartMs);
                int x2 = GetPixelFromMsec(m_pEditorDoc->m_markEndMs, m_pEditorDoc->m_displayStartMs);

                x1 += rcClient.left + LEFT_BORDER;
                x2 += rcClient.left + LEFT_BORDER;

                if (x1 < rcClient.left + LEFT_BORDER)
                    x1 = rcClient.left + LEFT_BORDER;
                if (x2 > rcClient.right)
                    x2 = rcClient.right;

                CDC dc2;
                dc2.CreateCompatibleDC(pDC);
                CBitmap b;
                b.LoadBitmap(IDB_SELECTION); //CreateCompatibleBitmap(&dc2, 1, 1);
                CBitmap *pOldBitmap = dc2.SelectObject(&b);

                BLENDFUNCTION bf;
                bf.BlendOp = AC_SRC_OVER;
                bf.BlendFlags = 0;
                bf.SourceConstantAlpha = 0x3f;
                bf.AlphaFormat = 0;

                AlphaBlend(pDC->m_hDC, x1, rcClient.top + 2, x2-x1, rcClient.bottom-(rcClient.top+2), dc2.m_hDC, 0, 0, 1, 1, bf);

                dc2.SelectObject(pOldBitmap);
                b.DeleteObject();
                dc2.DeleteDC();
            }
        }

        if (!m_pEditorDoc->project.IsEmpty())
            DrawCursor(pDC, rcClient.left + LEFT_BORDER, rcClient.right, rcClient.top, rcClient.bottom);
    } else {
        // draw streams background
        COLORREF clrBackground = CMainFrameE::GetColor(CMainFrameE::COLOR_BG_DARK);
        CBrush bgBrush(clrBackground);
        pDC->FillRect(&rcClient, &bgBrush);
        bgBrush.DeleteObject();
    }
}

void CStreamCtrl::OnPaint() {
    CPaintDC deviceContext(this); // device context for painting

    if (NULL == m_doubleBuffer.m_hObject) {
        return;
    }

    CRect rcClient;
    GetClientRect(&rcClient);

    // Clipping
    CRect rcClip;
    deviceContext.GetClipBox(&rcClip);

    CRgn rgnClip;
    rgnClip.CreateRectRgn(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
    m_doubleBufferDC.SelectClipRgn(&rgnClip);

    //CDC *pDC = &m_doubleBufferDC;

    DrawItem(rcClient, &m_doubleBufferDC);

    // Glonk double buffer onto real component DC
    deviceContext.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &m_doubleBufferDC, 0, 0, SRCCOPY);

    ((CStreamView *)GetParent())->UpdateSliderPos(false);
}

void CStreamCtrl::DrawSlidesStreamCaption(CDC *pDC, CRect &rcCaption) {	
    CFont *pFontCaption = XTPPaintManager()->GetIconBoldFont();
    CFont *pFontOld = pDC->SelectObject(pFontCaption);

    if (m_pImageBackgroundCaptionStream!=NULL) {
        m_pImageBackgroundCaptionStream->DrawImage(pDC, rcCaption, m_pImageBackgroundCaptionStream->GetSource(), CRect(5, 5, 5, 5), COLORREF_NULL);
    }  

    int oldBkMode = pDC->SetBkMode(TRANSPARENT);

    CPen linePen(PS_SOLID, 1, RGB(0xBC, 0xD1, 0xED));
    CPen *pOldPen = pDC->SelectObject(&linePen);
    pDC->MoveTo(rcCaption.left, rcCaption.bottom-1);
    pDC->LineTo(rcCaption.right, rcCaption.bottom-1);
    pDC->LineTo(rcCaption.right, rcCaption.top-1);
    pDC->SelectObject(pOldPen);

    // draw text
    CRect rcText(rcCaption);
    rcText.left += 10;
	rcText.right -=10;
    COLORREF clrOld = pDC->SetTextColor(RGB(0x55, 0x55, 0x55));

    CString csCaption;

	rcText.bottom = rcText.top + STREAM_HEIGHT;
	csCaption.LoadString(IDS_STREAM_SLIDES);
	pDC->DrawText(csCaption, rcText, DT_VCENTER | DT_SINGLELINE | DT_LEFT );

	LOGFONT lf;
	pFontCaption->GetLogFont(&lf);
	CFont fnt;
	lf.lfHeight-=1;
	fnt.CreateFontIndirect(&lf);
	pDC->SelectObject(fnt);
	rcText.top = rcText.bottom;
	rcText.bottom = rcText.top + 2 * STREAM_HEIGHT / 3;
	csCaption.LoadString(ID_GROUP_GRAPHICAL_OBJECTS);
	pDC->DrawText(csCaption, rcText, DT_VCENTER | DT_SINGLELINE | DT_RIGHT );

	rcText.top = rcText.bottom;
	rcText.bottom = rcText.top + 2 * STREAM_HEIGHT / 3;
	csCaption.LoadString(IDS_INTERACTION_OBJECTS);
	pDC->DrawText(csCaption, rcText, DT_VCENTER | DT_SINGLELINE | DT_RIGHT );

	rcText.top = rcText.bottom;
	rcText.bottom = rcText.top + 2 * STREAM_HEIGHT / 3;
	csCaption.LoadString(IDS_QUESTION_OBJECTS);
	pDC->DrawText(csCaption, rcText, DT_VCENTER | DT_SINGLELINE | DT_RIGHT );

	// reset 
    pDC->SetBkMode(oldBkMode);
    pDC->SetTextColor(clrOld);
    pDC->SelectObject(pFontOld);

}



void CStreamCtrl::DrawStreamCaption(CDC *pDC, CRect &rcCaption, UINT nTextId) {	
    CFont *pFontCaption = XTPPaintManager()->GetIconBoldFont();
    CFont *pFontOld = pDC->SelectObject(pFontCaption);

    if (m_pImageBackgroundCaptionStream!=NULL) {
        m_pImageBackgroundCaptionStream->DrawImage(pDC, rcCaption, m_pImageBackgroundCaptionStream->GetSource(), CRect(5, 5, 5, 5), COLORREF_NULL);
    }  

    int oldBkMode = pDC->SetBkMode(TRANSPARENT);
    // draw background
    //COLORREF clrLight = CMainFrameE::GetColor(CMainFrameE::COLOR_LIGHT);
    //COLORREF clrDark = CMainFrameE::GetColor(CMainFrameE::COLOR_DARK);
    //XTPPaintManager()->GradientFill(pDC, rcCaption, color1/*clrLight*/,color2 /*clrDark*/, FALSE);

    CPen linePen(PS_SOLID, 1, RGB(0xBC, 0xD1, 0xED));
    CPen *pOldPen = pDC->SelectObject(&linePen);
    pDC->MoveTo(rcCaption.left, rcCaption.bottom-1);
    pDC->LineTo(rcCaption.right, rcCaption.bottom-1);
    pDC->LineTo(rcCaption.right, rcCaption.top-1);
    pDC->SelectObject(pOldPen);

    // draw text
    CRect rcText(rcCaption);
    rcText.left += 10;
    COLORREF clrOld = pDC->SetTextColor(RGB(0x55, 0x55, 0x55));

    CString csCaption;
    csCaption.LoadString(nTextId);
    pDC->DrawText(csCaption, rcText, DT_VCENTER | DT_SINGLELINE );

    // reset 
    pDC->SetBkMode(oldBkMode);
    pDC->SetTextColor(clrOld);
    pDC->SelectObject(pFontOld);

}

void CStreamCtrl::DrawStreamBackground(CDC *pDC, CRect &rcStream) {	
    // draw background
    CRect rcBack(rcStream);
    if (m_pImageBackgroundStream) {
        m_pImageBackgroundStream->DrawImage(pDC, rcBack, m_pImageBackgroundStream->GetSource(), CRect(5, 5, 5, 5), COLORREF_NULL);
    } else {
        COLORREF clrBg = RGB(0xF5, 0xF5, 0xF5);
        //COLORREF clrLine = RGB(107, 102, 97);
        CBrush bgBrush(clrBg);
        pDC->FillRect(rcBack, &bgBrush);
    }

    /*CPen linePen(PS_SOLID, 1, clrLine);
    CPen *pPenOld = pDC->SelectObject(&linePen);

    pDC->MoveTo(rcBack.left, rcBack.top);
    pDC->LineTo(rcBack.right, rcBack.top);
    pDC->MoveTo(rcBack.left, rcBack.bottom);
    pDC->LineTo(rcBack.right, rcBack.bottom);

    pDC->SelectObject(pPenOld);*/
}

void CStreamCtrl::DrawVideoStream(CDC *pDC, int left, int top, int right, int bottom) {
    CRect rcClip;
    pDC->GetClipBox(&rcClip);

    if (rcClip.bottom < top || rcClip.top > bottom) {
        return;
    }

    int fromMsec = m_pEditorDoc->m_displayStartMs;
    int toMsec   = m_pEditorDoc->m_displayEndMs;

    CArray<CPreviewSegment *, CPreviewSegment *> *pPreviewList = &m_pEditorDoc->project.m_previewList;

    if (pPreviewList->GetSize() == 0)
        return;
    CFont *pFontCaption = XTPPaintManager()->GetIconFont();
    CFont *pFontOld = pDC->SelectObject(pFontCaption);
    int iModeOld = pDC->SetBkMode(TRANSPARENT);

    int globalOffset = m_pEditorDoc->project.m_videoExportGlobalOffset;
    CVideoStream *pLastVideoStream = NULL;
    int nLastVideoBegin = -1;
    int nLastVideoEnd = -1;
    for (int i=0; i<pPreviewList->GetSize(); ++i) {
        CPreviewSegment *segment = pPreviewList->GetAt(i);
        if (segment &&
            (segment->GetTargetEnd() >= fromMsec && segment->GetTargetEnd() <= toMsec) ||
            (segment->GetTargetBegin() >= fromMsec && segment->GetTargetBegin() <= toMsec) ||
            (segment->GetTargetBegin() < fromMsec && segment->GetTargetEnd() > toMsec)) {
                CVideoStream *videoStream = segment->GetVideoStream();
            if (videoStream) {
                int offsetDiff = globalOffset - segment->GetVideoOffset() + segment->GetVideoStream()->GetOffset() - segment->GetSourceBegin();
                if (offsetDiff < 0)
                    offsetDiff = 0;
                int lengthDiff = segment->GetSourceEnd() - (globalOffset - segment->GetVideoOffset() + segment->GetVideoStream()->GetOffset() + segment->GetVideoStream()->GetLength());
                if (lengthDiff < 0)
                    lengthDiff = 0;

                int nVideoBegin = segment->GetTargetBegin() + offsetDiff;
                int nVideoEnd = segment->GetTargetEnd() - lengthDiff;

                if (nVideoBegin < nVideoEnd) {
                    CRect rcVideoRect;
                    bool bCutFirst = false;
                    bool bCutLast = false;
                    if (pLastVideoStream && 
                        _tcscmp(pLastVideoStream->GetFilename(), videoStream->GetFilename()) == 0 &&
                        nLastVideoEnd == nVideoBegin) {
                            // if pLastVideoStream == videoStream use begin of pLastVideoStream to calculate rectangles
                            CalculateBitmapRectangles(rcVideoRect, nLastVideoBegin, nVideoEnd,
                                fromMsec, toMsec, m_pEditorDoc->m_displayStartMs,
                                left, right, top, bottom, bCutFirst, bCutLast);
                            nLastVideoEnd = nVideoEnd;
                    } else {
                        CalculateBitmapRectangles(rcVideoRect, nVideoBegin, nVideoEnd,
                            fromMsec, toMsec, m_pEditorDoc->m_displayStartMs,
                            left, right, top, bottom, bCutFirst, bCutLast);
                        UINT uim_displayEndMs = m_pEditorDoc->m_displayEndMs;
                        pLastVideoStream = videoStream;
                        nLastVideoBegin  = nVideoBegin;
                        nLastVideoEnd    = nVideoEnd;
                        if (( m_pEditorDoc->m_displayEndMs < nVideoEnd)&&(fromMsec==0)) {
                            rcVideoRect.right += 5;  
                        } else if(( m_pEditorDoc->m_displayEndMs < nVideoEnd)&&(fromMsec>0)) {
                            rcVideoRect.right += 5; 
                            rcVideoRect.left -= 4;
                        } else if(( fromMsec < nVideoEnd)&&(fromMsec>0)) {
                            rcVideoRect.left -= 4;
                        }
                    }
                    rcVideoRect.DeflateRect(1, 1, 0, 1);

                    COLORREF clrPen = CMainFrameE::GetColor(CMainFrameE::COLOR_STREAM_LINE);
                    CPen borderPen(PS_SOLID, 1, clrPen);
                    CPen *pPenOld =  pDC->SelectObject(&borderPen);

                    pDC->Rectangle(&rcVideoRect);

                    CBrush bmpBrush;
                    bmpBrush.CreatePatternBrush(&m_bmpBackground);

                    pDC->SetBrushOrg(rcVideoRect.left, rcVideoRect.top);
                    if ((m_bBackgrounds==true)&&(m_pImageBackgroundVideo!=NULL)) {
                        m_pImageBackgroundVideo->DrawImage(pDC, rcVideoRect, m_pImageBackgroundVideo->GetSource(), CRect(5, 5, 5, 5), COLORREF_NULL);
                    } else {
                        pDC->FillRect(rcVideoRect, &bmpBrush);
                    }
                    if ((m_bIconVideo==true)&&(m_pIconVideo!=NULL)) {
                        CSize sz(16, 16);
                        UINT uiOffsetIcon = 0;
                        // set drawing point
                        if (( fromMsec < nVideoEnd)&&(fromMsec>0)) {
                            uiOffsetIcon = 4;
                        }
                        CPoint pt(rcVideoRect.left + 10+uiOffsetIcon,rcVideoRect.top+2);
                        // draw an image (icon)
                        m_pIconVideo->Draw(pDC, pt, m_pIconVideo->GetIcon(), sz);
                    }
                    // draw shadow
                    CPen shadowPen(PS_SOLID, 1, RGB(190, 190, 190));
                    pDC->SelectObject(&shadowPen);
                    /*pDC->MoveTo(rcVideoRect.left - 2, rcVideoRect.bottom + 2);
                    pDC->LineTo(rcVideoRect.right + 2, rcVideoRect.bottom + 2);*/

                    pDC->SelectObject(pPenOld);
                    bmpBrush.DeleteObject();

                    CString videoTitle = videoStream->GetFilename();
                    StringManipulation::GetFilename(videoTitle);
                    if (!videoTitle.IsEmpty()) {
                        UINT uiOffsetText = 0;
                        pDC->SetTextColor(CAPTION_COLOR);
                        CRect rcText(rcVideoRect);
                        rcText.DeflateRect(CAPTION_SPACING, 0);
                        if ((m_bIconVideo==true)&&(m_pIconVideo!=NULL)) {
                            if ((fromMsec < nVideoEnd)&&(fromMsec>0)) {
                                uiOffsetText = 4;                             
                            }
                            rcText.MoveToX(rcVideoRect.left + 10 +m_iconHandle.GetExtent().cx+ 5 + uiOffsetText);
                        }
                        //rcText.OffsetRect(CAPTION_LEFT_SPACE, 0);
                        if (rcText.Width() > 0) {
                            pDC->DrawText(videoTitle, &rcText, 
                                DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOCLIP);
                        }
                    }
                }
            }
        }
    }
    pDC->SelectObject(pFontOld);
}

void CStreamCtrl::DrawSlidesStream(CDC *pDC, int left, int top, int right, int bottom)
{
	CRect rcClip;
	pDC->GetClipBox(&rcClip);
	if (rcClip.bottom < top || rcClip.top > bottom)
		return;
	if (m_pEditorDoc->project.IsDenverDocument())
		return;

	CArray<CPage *, CPage *> pages;
	int fromMsec = m_pEditorDoc->m_displayStartMs;
	int toMsec   = m_pEditorDoc->m_displayEndMs;

	m_pEditorDoc->project.GetPages(pages, fromMsec, toMsec);

	int nLastPageEnd = 0;
	// set font
	CFont *pFont = XTPPaintManager()->GetIconFont();
	CFont *oldFont = pDC->SelectObject(pFont);
	int nOldMode = pDC->SetBkMode(TRANSPARENT);

	// create and set colors
	COLORREF clrPen = CMainFrameE::GetColor(CMainFrameE::COLOR_STREAM_LINE);
	CPen borderPen(PS_SOLID, 1, clrPen);
	CPen *pOldPen =  pDC->SelectObject(&borderPen);

	COLORREF clrBg = CMainFrameE::GetColor(CMainFrameE::COLOR_STREAM_BG);
	CBrush fillBrush(clrBg);

	CPen shadowPen(PS_SOLID, 1, RGB(190, 190, 190));
	bool bCutFirst = false;
	bool bCutLast = false;

	for (int i = 0; i < pages.GetSize(); ++i) {
		CPage *page = pages[i];
		CRect rcSlide;
		CalculateBitmapRectangles(rcSlide, page->GetBegin(), page->GetEnd(),
			fromMsec, toMsec, m_pEditorDoc->m_displayStartMs,
			left, right, top, bottom, bCutFirst, bCutLast);

		UINT uiPageBegin = page->GetBegin();
		UINT uiPageEnd = page->GetEnd();

		UINT uiClipLeft = rcClip.left;
		UINT uiClipRight = rcClip.right;

		if((uiPageBegin < fromMsec ) && (uiPageEnd < toMsec )) {
			rcSlide.left -= 4;
		} else if(((uiPageBegin > fromMsec ) && (uiPageEnd > toMsec ))||
			(( m_pEditorDoc->m_displayEndMs < page->GetEnd())&&(fromMsec==0))) {
				rcSlide.right += 5;
		} else if((uiPageBegin < fromMsec )&&(uiPageEnd > toMsec )) {
			rcSlide.right += 5; 
			rcSlide.left -= 4;
		}
		rcSlide.DeflateRect(0, 1, 0, 1);
		rcSlide.right += 1;
		rcSlide.left += 1;

		if(i>0)
			rcSlide.left -= 2;

		if ((m_bBackgrounds==true)&&(m_pImageBackgroundSlides!=NULL)) {
			m_pImageBackgroundSlides->DrawImage(pDC, rcSlide, m_pImageBackgroundSlides->GetSource(), CRect(5, 5, 5, 5), COLORREF_NULL);
		} else {
			pDC->FillRect(rcSlide, &fillBrush);
		}
		if ((m_bIconSlides==true)&&(m_pIconSlides!=NULL)) {
			CSize sz(16, 16);
			UINT uiOffsetIcon = 0;
			if(( fromMsec <page->GetEnd())&&(fromMsec>0))
			{
				uiOffsetIcon = 4;
			}
			// set drawing point
			CPoint pt(rcSlide.left + 10 + uiOffsetIcon,rcSlide.top+2);
			// draw an image (icon)
			m_pIconSlides->Draw(pDC, pt, m_pIconSlides->GetIcon(), sz);
		}

		CString pageTitle = page->GetTitle();
		if (!pageTitle.IsEmpty()) {  
			UINT uiOffsetText = 0;
			pDC->SetTextColor(CAPTION_COLOR);
			CRect rcText(rcSlide);
			rcText.bottom=rcText.top + rcText.Height()/3;
			rcText.DeflateRect(CAPTION_SPACING, 0);
			if ((m_bIconSlides==true)&&(m_pIconSlides!=NULL)) {
				if (( fromMsec <page->GetEnd())&&(fromMsec>0)) {
					uiOffsetText = 4;
				}
				rcText.MoveToX(rcSlide.left + 10 +m_iconHandle.GetExtent().cx+ 5 +uiOffsetText);
			}
			if (rcText.Width() > 0) {
				pDC->DrawText(pageTitle, &rcText, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOCLIP);
			}
		}
		nLastPageEnd = page->GetEnd();
	}

	//draw timeline object duration
	CInteractionAreaEx *pInteraction;
	CRect rcObj;
	int iGraphicalTop,iInteractionTop,iQuestionTop,iHeight;
	iHeight = 2*(bottom-top)/3;
	iGraphicalTop = bottom-iHeight;
	iHeight=iHeight/3;
	iGraphicalTop = iGraphicalTop + iHeight*0.1;
	iInteractionTop = iGraphicalTop+iHeight;
	iQuestionTop=iInteractionTop+iHeight;
	iHeight=iHeight*0.8;
    Gdiplus::Graphics graphics(pDC->m_hDC);
	if(m_pEditorDoc->GetWhiteboardView() != NULL)
	{
        
		for(int i=0;i<m_pEditorDoc->GetWhiteboardView()->GetTimelineDisplayObjects().GetCount();i++)
		{
			rcObj.SetRectEmpty();
			pInteraction = m_pEditorDoc->GetWhiteboardView()->GetTimelineDisplayObjects().ElementAt(i);
			if(pInteraction && !m_pEditorDoc->IsPreviewActive() && pInteraction->IsTimelineDisplay())
			{
				CalculateBitmapRectangles(rcObj, pInteraction->GetVisibilityStart(), pInteraction->GetVisibilityStart()+pInteraction->GetVisibilityLength(),
					fromMsec, toMsec, m_pEditorDoc->m_displayStartMs, left, right, top, bottom, bCutFirst, bCutLast);
				if(rcObj.left == rcObj.right)
					rcObj.right = rcObj.left + 1;
				if (fromMsec == 0 && pInteraction->GetVisibilityStart() == 0)
					rcObj.left += 2;
				if(pInteraction->GetTypeId()==INTERACTION_RECORD_GRAPHIC_OBJECTS && !pInteraction->IsDemoDocumentObject())
				{
					rcObj.top =iGraphicalTop;
					rcObj.bottom =iGraphicalTop+iHeight;
                    
					/*COLORREF clrBgSel = RGB(155, 155, 255);
					CBrush fillBrush(clrBgSel);
					pDC->FillRect(rcObj, &fillBrush);*/
                    Gdiplus::SolidBrush fillBrush(Gdiplus::Color(120, 155, 155, 255));
                    graphics.FillRectangle(&fillBrush, rcObj.left, rcObj.top, rcObj.Width(), rcObj.Height());
					//if(m_pEditorDoc->GetWhiteboardView()->GetTimelineSelectedObject()==pInteraction)
					//	m_rcSelectedObject.CopyRect(rcObj);
					if (rcObj.Width() < MIN_TIMELINE_WIDTH){
						Gdiplus::Point *zPts = (Gdiplus::Point *)malloc(3*sizeof(Gdiplus::Point));
						zPts[0].X = rcObj.left;
						zPts[0].Y = rcObj.top;
						zPts[1].X = rcObj.left - (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
						zPts[1].Y = rcObj.top + rcObj.Height() / 2;
						zPts[2].X = rcObj.left;
						zPts[2].Y = rcObj.bottom;
						graphics.FillPolygon(&fillBrush,zPts,3);
						zPts[0].X = rcObj.right;
						zPts[0].Y = rcObj.top;
						zPts[1].X = rcObj.right + (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
						zPts[1].Y = rcObj.top + rcObj.Height() / 2;
						zPts[2].X = rcObj.right;
						zPts[2].Y = rcObj.bottom;
						graphics.FillPolygon(&fillBrush,zPts,3);
						free(zPts);
					}
				}
				else if(pInteraction->GetTypeId()==INTERACTION_RECORD_ELEMENTS || (pInteraction->GetTypeId()==INTERACTION_RECORD_GRAPHIC_OBJECTS && pInteraction->IsDemoDocumentObject()))
				{
					rcObj.top =iInteractionTop;
					rcObj.bottom =iInteractionTop+iHeight;
					/*COLORREF clrBgSel = RGB(255, 155, 155);
					CBrush fillBrush(clrBgSel);
					pDC->FillRect(rcObj, &fillBrush);*/
                    Gdiplus::SolidBrush fillBrush(Gdiplus::Color(120, 255, 155, 155));
                    graphics.FillRectangle(&fillBrush, rcObj.left, rcObj.top, rcObj.Width(), rcObj.Height());
					//if(m_pEditorDoc->GetWhiteboardView()->GetTimelineSelectedObject()==pInteraction)
					//	m_rcSelectedObject.CopyRect(rcObj);
					if (rcObj.Width() < MIN_TIMELINE_WIDTH){
						Gdiplus::Point *zPts = (Gdiplus::Point *)malloc(3*sizeof(Gdiplus::Point));
						zPts[0].X = rcObj.left;
						zPts[0].Y = rcObj.top;
						zPts[1].X = rcObj.left - (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
						zPts[1].Y = rcObj.top + rcObj.Height() / 2;
						zPts[2].X = rcObj.left;
						zPts[2].Y = rcObj.bottom;
						graphics.FillPolygon(&fillBrush,zPts,3);
						zPts[0].X = rcObj.right;
						zPts[0].Y = rcObj.top;
						zPts[1].X = rcObj.right + (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
						zPts[1].Y = rcObj.top + rcObj.Height() / 2;
						zPts[2].X = rcObj.right;
						zPts[2].Y = rcObj.bottom;
						graphics.FillPolygon(&fillBrush,zPts,3);
						free(zPts);
					}
				}
				else
				{
					rcObj.top =iQuestionTop;
					rcObj.bottom =iQuestionTop+iHeight;
					/*COLORREF clrBgSel = RGB(155, 255, 155);
					CBrush fillBrush(clrBgSel);
					pDC->FillRect(rcObj, &fillBrush);*/
                    Gdiplus::SolidBrush fillBrush(Gdiplus::Color(120, 155, 255, 155));
                    graphics.FillRectangle(&fillBrush, rcObj.left, rcObj.top, rcObj.Width(), rcObj.Height());
					//if(m_pEditorDoc->GetWhiteboardView()->GetTimelineSelectedObject()==pInteraction)
					//	m_rcSelectedObject.CopyRect(rcObj);
					if (rcObj.Width() < MIN_TIMELINE_WIDTH){
						Gdiplus::Point *zPts = (Gdiplus::Point *)malloc(3*sizeof(Gdiplus::Point));
						zPts[0].X = rcObj.left;
						zPts[0].Y = rcObj.top;
						zPts[1].X = rcObj.left - (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
						zPts[1].Y = rcObj.top + rcObj.Height() / 2;
						zPts[2].X = rcObj.left;
						zPts[2].Y = rcObj.bottom;
						graphics.FillPolygon(&fillBrush,zPts,3);
						zPts[0].X = rcObj.right;
						zPts[0].Y = rcObj.top;
						zPts[1].X = rcObj.right + (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
						zPts[1].Y = rcObj.top + rcObj.Height() / 2;
						zPts[2].X = rcObj.right;
						zPts[2].Y = rcObj.bottom;
						graphics.FillPolygon(&fillBrush,zPts,3);
						free(zPts);
					}
				}
			}
		}
		for(int i=0;i<m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetCount();i++)
		{
			pInteraction = m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetAt(i);
			if(pInteraction)
			{
				rcObj.SetRectEmpty();
				//pInteraction = m_pEditorDoc->GetWhiteboardView()->GetTimelineSelectedObject();
				if(/*pInteraction && */!m_pEditorDoc->IsPreviewActive())
				{
					CalculateBitmapRectangles(rcObj, pInteraction->GetVisibilityStart(), pInteraction->GetVisibilityStart()+pInteraction->GetVisibilityLength(),
						fromMsec, toMsec, m_pEditorDoc->m_displayStartMs, left, right, top, bottom, bCutFirst, bCutLast);
					if(rcObj.left == rcObj.right)
						rcObj.right = rcObj.left + 1;
					if (fromMsec == 0 && pInteraction->GetVisibilityStart() == 0)
						rcObj.left += 2;
					if(pInteraction->GetTypeId()==INTERACTION_RECORD_GRAPHIC_OBJECTS  && !pInteraction->IsDemoDocumentObject())
					{
						rcObj.top =iGraphicalTop;
						rcObj.bottom =iGraphicalTop+iHeight;
						/*COLORREF clrBgSel = RGB(155, 155, 255);
						CBrush fillBrush(clrBgSel);
						pDC->FillRect(rcObj, &fillBrush);*/
						Gdiplus::SolidBrush fillBrush(Gdiplus::Color(120, 155, 155, 255));
						graphics.FillRectangle(&fillBrush, rcObj.left, rcObj.top, rcObj.Width(), rcObj.Height());
						pInteraction->SetTimelineRect(rcObj);
						if (rcObj.Width() < MIN_TIMELINE_WIDTH){
							Gdiplus::Point *zPts = (Gdiplus::Point *)malloc(3*sizeof(Gdiplus::Point));
							zPts[0].X = rcObj.left;
							zPts[0].Y = rcObj.top;
							zPts[1].X = rcObj.left - (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
							zPts[1].Y = rcObj.top + rcObj.Height() / 2;
							zPts[2].X = rcObj.left;
							zPts[2].Y = rcObj.bottom;
							graphics.FillPolygon(&fillBrush,zPts,3);
							zPts[0].X = rcObj.right;
							zPts[0].Y = rcObj.top;
							zPts[1].X = rcObj.right + (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
							zPts[1].Y = rcObj.top + rcObj.Height() / 2;
							zPts[2].X = rcObj.right;
							zPts[2].Y = rcObj.bottom;
							graphics.FillPolygon(&fillBrush,zPts,3);
							free(zPts);
						}
					}
					else if(pInteraction->GetTypeId()==INTERACTION_RECORD_ELEMENTS || (pInteraction->GetTypeId()==INTERACTION_RECORD_GRAPHIC_OBJECTS && pInteraction->IsDemoDocumentObject()))
					{
						rcObj.top =iInteractionTop;
						rcObj.bottom =iInteractionTop+iHeight;
						/*COLORREF clrBgSel = RGB(255, 155, 155);
						CBrush fillBrush(clrBgSel);
						pDC->FillRect(rcObj, &fillBrush);*/
						Gdiplus::SolidBrush fillBrush(Gdiplus::Color(120, 255, 155, 155));
						graphics.FillRectangle(&fillBrush, rcObj.left, rcObj.top, rcObj.Width(), rcObj.Height());
						pInteraction->SetTimelineRect(rcObj);
						if (rcObj.Width() < MIN_TIMELINE_WIDTH){
							Gdiplus::Point *zPts = (Gdiplus::Point *)malloc(3*sizeof(Gdiplus::Point));
							zPts[0].X = rcObj.left;
							zPts[0].Y = rcObj.top;
							zPts[1].X = rcObj.left - (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
							zPts[1].Y = rcObj.top + rcObj.Height() / 2;
							zPts[2].X = rcObj.left;
							zPts[2].Y = rcObj.bottom;
							graphics.FillPolygon(&fillBrush,zPts,3);
							zPts[0].X = rcObj.right;
							zPts[0].Y = rcObj.top;
							zPts[1].X = rcObj.right + (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
							zPts[1].Y = rcObj.top + rcObj.Height() / 2;
							zPts[2].X = rcObj.right;
							zPts[2].Y = rcObj.bottom;
							graphics.FillPolygon(&fillBrush,zPts,3);
							free(zPts);
						}
					}
					else
					{
						rcObj.top =iQuestionTop;
						rcObj.bottom =iQuestionTop+iHeight;
						/*COLORREF clrBgSel = RGB(155, 255, 155);
						CBrush fillBrush(clrBgSel);
						pDC->FillRect(rcObj, &fillBrush);*/
						Gdiplus::SolidBrush fillBrush(Gdiplus::Color(120, 155, 255, 155));
						graphics.FillRectangle(&fillBrush, rcObj.left, rcObj.top, rcObj.Width(), rcObj.Height());
						pInteraction->SetTimelineRect(rcObj);
						if (rcObj.Width() < MIN_TIMELINE_WIDTH){
							Gdiplus::Point *zPts = (Gdiplus::Point *)malloc(3*sizeof(Gdiplus::Point));
							zPts[0].X = rcObj.left;
							zPts[0].Y = rcObj.top;
							zPts[1].X = rcObj.left - (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
							zPts[1].Y = rcObj.top + rcObj.Height() / 2;
							zPts[2].X = rcObj.left;
							zPts[2].Y = rcObj.bottom;
							graphics.FillPolygon(&fillBrush,zPts,3);
							zPts[0].X = rcObj.right;
							zPts[0].Y = rcObj.top;
							zPts[1].X = rcObj.right + (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
							zPts[1].Y = rcObj.top + rcObj.Height() / 2;
							zPts[2].X = rcObj.right;
							zPts[2].Y = rcObj.bottom;
							graphics.FillPolygon(&fillBrush,zPts,3);
							free(zPts);
						}
					}
					//if(!m_rcSelectedObject.IsRectEmpty() && !m_pEditorDoc->IsPreviewActive())
					//{
					/*COLORREF clrBgSel = RGB(5, 5, 155);
					CBrush fillBrush(clrBgSel);
					pDC->FrameRect(m_rcSelectedObject, &fillBrush);*/
					Gdiplus::SolidBrush fillBrush(Gdiplus::Color(180, 5, 5, 155));
					Gdiplus::Pen pen(Gdiplus::Color(120, 5, 5, 155), 1.0);
					graphics.DrawRectangle(&pen, rcObj.left, rcObj.top, rcObj.Width()-1, rcObj.Height()-1);
					if (rcObj.Width() < MIN_TIMELINE_WIDTH){
						Gdiplus::Point *zPts = (Gdiplus::Point *)malloc(3*sizeof(Gdiplus::Point));
						zPts[0].X = rcObj.left;
						zPts[0].Y = rcObj.top;
						zPts[1].X = rcObj.left - (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
						zPts[1].Y = rcObj.top + rcObj.Height() / 2;
						zPts[2].X = rcObj.left;
						zPts[2].Y = rcObj.bottom-1;
						graphics.DrawPolygon(&pen,zPts,3);
						zPts[0].X = rcObj.right;
						zPts[0].Y = rcObj.top;
						zPts[1].X = rcObj.right + (MIN_TIMELINE_WIDTH - rcObj.Width())/2;
						zPts[1].Y = rcObj.top + rcObj.Height() / 2;
						zPts[2].X = rcObj.right;
						zPts[2].Y = rcObj.bottom-1;
						graphics.DrawPolygon(&pen,zPts,3);
						free(zPts);
						//}
					}
				}
			}
		}
	}

	pDC->SelectObject(oldFont);
	pDC->SelectObject(pOldPen);
	borderPen.DeleteObject();
	fillBrush.DeleteObject();
	pDC->SetBkMode(nOldMode);
	pages.RemoveAll();
}

void CStreamCtrl::DrawClipsStream(CDC *pDC, int left, int top, int right, int bottom, int yOffset) {
    CRect rcClip;
    pDC->GetClipBox(&rcClip);

    if (rcClip.bottom < top || rcClip.top > bottom)
        return;

    CClipStreamR *pClipStream = m_pEditorDoc->project.GetClipStream();

    if (pClipStream != NULL) {
        m_arClipStreamRect.RemoveAll();

        UINT nDisplayStartMs = m_pEditorDoc->m_displayStartMs;
        UINT nDisplayLengthMs = m_pEditorDoc->m_displayEndMs - nDisplayStartMs;
        if (m_pImageBackgroundClip!=NULL)
        {
            pClipStream->SetBackgroundImg( m_pImageBackgroundClip );
        }
        pClipStream->Draw(pDC, CRect(left, top, right, bottom), 
            nDisplayStartMs, nDisplayLengthMs, &m_arClipStreamRect, yOffset);
    }
}

void CStreamCtrl::DrawAudioStream(CDC *pDC, int _left, int _top, int _right, int _bottom)
{
    CRect rcClip;
    pDC->GetClipBox(&rcClip);
    CRect rcAudio(_left, _top, _right, _bottom);
    if (rcClip.bottom < rcAudio.top || rcClip.top > rcAudio.bottom)
        return;

    int fromMsec = m_pEditorDoc->m_displayStartMs;
    int toMsec   = m_pEditorDoc->m_displayEndMs;
    if (toMsec < fromMsec)
        return;

    // get preview list from project
    CArray<CPreviewSegment *, CPreviewSegment *> *pPreviewList = &m_pEditorDoc->project.m_previewList;
    if (pPreviewList->GetSize() == 0)
        return;

    // draw background
    // remember the old pen
    CPen *pPenOld = pDC->SelectObject(&m_cpAudioWavePen);

    rcAudio.DeflateRect(1, 1, 0, 1);

    // draw audio
    int firstDisplayMsec = m_pEditorDoc->m_displayStartMs;
    rcAudio.DeflateRect(0, 1);
    if (m_pImageBackgroundAudio != NULL) {
        m_pImageBackgroundAudio->DrawImage(pDC, rcAudio, m_pImageBackgroundAudio->GetSource(), CRect(15, 15, 15, 15), COLORREF_NULL);
    }

    CRect rcClient;
    GetClientRect(&rcClient);
    rcClient.left += LEFT_BORDER;

    for (int i=0; i<pPreviewList->GetSize(); ++i)
    {
        CPreviewSegment *segment = pPreviewList->GetAt(i);
        if (segment &&
            (segment->GetTargetEnd() > fromMsec && segment->GetTargetEnd() <= toMsec) ||
            (segment->GetTargetBegin() >= fromMsec && segment->GetTargetBegin() < toMsec) ||
            (segment->GetTargetBegin() < fromMsec && segment->GetTargetEnd() > toMsec))
        {
            //         TRACE1("Drawing segment #%d\n", i);
            int from = segment->GetTargetBegin();
            int leftOffset = 0;
            if (from < fromMsec)
                leftOffset = fromMsec - from;
            from += leftOffset;
            int sourceFrom = segment->GetSourceBegin() + leftOffset + segment->GetAudioOffset();

            int to = segment->GetTargetEnd();
            int rightOffset = 0;
            if (to > toMsec)
                rightOffset = toMsec - to;
            to += rightOffset;
            int sourceTo = segment->GetSourceEnd() + rightOffset + segment->GetAudioOffset();

            CAudioStream *audio = segment->GetAudioStream();
            if (audio)
            {
                signed char *pSamples = audio->GetSampleBuffer();
                int bufferLen = audio->GetSampleBufferLength();

                int sLeft  = GetPixelFromMsec(from, firstDisplayMsec, rcClient);
                int sRight = GetPixelFromMsec(to, firstDisplayMsec, rcClient);

                int middle = rcAudio.Height() / 2;
                int sampleNr=0;
                int sampleNrOld=0;

                int pixelWidth = sRight - sLeft ;
                double stretch = (double) rcAudio.Height() / 256.0;

                if (bufferLen > 0)
                {
                    pDC->MoveTo(rcAudio.left + sLeft, rcAudio.top + middle + (int) (stretch * pSamples[sourceFrom / 10]));
                    for (double x=sLeft; x<sRight; x += 0.1)
                    {
                        sampleNr = (int)((double)sourceFrom / (double)10.0)+ (int) ((double) (sourceTo - sourceFrom) * (x - sLeft) / (double) pixelWidth / 10.0);
                        if( sampleNrOld != sampleNr)
                        {
                            if (sampleNr >= bufferLen)
                                sampleNr = bufferLen - 1;
                            if (sampleNr < 0)
                                sampleNr = 0;

                            int ix = (int) x + rcAudio.left;
                            if (ix >= rcClip.left && ix <= rcClip.right)
                            {
                                pDC->LineTo(rcAudio.left + (int) x, rcAudio.top + middle + (int) (stretch * pSamples[sampleNr]));
                            }
                        }
                        sampleNrOld = sampleNr;
                    }
                }
                else
                {
                    // Dummy audio buffer
                    pDC->MoveTo(rcAudio.left + sLeft, rcAudio.top + middle);
                    pDC->LineTo(rcAudio.left + sRight, rcAudio.top + middle);
                }
            }
        }
    }

    pDC->SelectObject(pPenOld);
}

void CStreamCtrl::CalculateBitmapRectangles(CRect &rcSegment, int xBegin, int xEnd, int fromMsec, int toMsec,
                                            int firstDisplayMsec,
                                            int left, int right, int top, int bottom, bool &bCutFirst, bool &bCutLast) {
    // initializ top and bottom of rectangle
    rcSegment.top    = top;
    rcSegment.bottom = bottom;

    bCutFirst = false;
    bCutLast = false;
    if (xBegin < fromMsec) {
        bCutFirst = true;
        rcSegment.left = left + GetPixelFromMsec(fromMsec, firstDisplayMsec);
    } else {
        rcSegment.left = left + GetPixelFromMsec(xBegin, firstDisplayMsec);
    }

    if (xEnd > toMsec) {
        bCutLast = true;
        rcSegment.right = left + GetPixelFromMsec(toMsec, firstDisplayMsec);
    } else {
        rcSegment.right = left + GetPixelFromMsec(xEnd, firstDisplayMsec);
    }
    rcSegment.right -= 1;
}

void CStreamCtrl::DrawCursor(CDC *pDC, int left, int right, int top, int bottom) { 
    m_cursorPositionPixel = LEFT_BORDER + GetPixelFromMsec(m_pEditorDoc->m_curPosMs, m_pEditorDoc->m_displayStartMs);

    if (m_cursorPositionPixel < LEFT_BORDER || m_cursorPositionPixel > right)
        m_cursorPositionPixel = -1;

    if (m_cursorPositionPixel != -1) {
        CPen *oldPen = pDC->SelectObject(&m_penLight);
        pDC->MoveTo(m_cursorPositionPixel-1, top);
        pDC->LineTo(m_cursorPositionPixel-1, bottom);
        pDC->SelectObject(&m_penMiddle);
        pDC->MoveTo(m_cursorPositionPixel, top);
        pDC->LineTo(m_cursorPositionPixel, bottom);
        pDC->SelectObject(&m_penDark);
        pDC->MoveTo(m_cursorPositionPixel+1, top);
        pDC->LineTo(m_cursorPositionPixel+1, bottom);
        pDC->SelectObject(oldPen);
    }
}

BOOL CStreamCtrl::OnEraseBkgnd(CDC* pDC) {
    return TRUE;
}

void CStreamCtrl::UpdateSelection(int x, int status) {
    static int lastX = -1;

    if (x > 0 && m_markStartPixel > 0) {
        CRect rcStreamView;
        GetClientRect(&rcStreamView);
        rcStreamView.left += LEFT_BORDER;

        if (rcStreamView.Width() > 0) {
            if (status == INITIALIZE) {
                m_pEditorDoc->m_markStartMs = -1;
                m_pEditorDoc->m_markEndMs   = -1;
            } else if (status == MODIFY) {
                if (x < m_markStartPixel) {
                    m_pEditorDoc->m_markStartMs = GetMsecFromPixel(x - rcStreamView.left);
                    m_pEditorDoc->m_markEndMs = GetMsecFromPixel(m_markStartPixel - rcStreamView.left);
                } else {               
                    m_pEditorDoc->m_markStartMs = GetMsecFromPixel(m_markStartPixel - rcStreamView.left);
                    m_pEditorDoc->m_markEndMs = GetMsecFromPixel(x - rcStreamView.left);

                }
                if (m_pEditorDoc->m_markStartMs < 0)
                    m_pEditorDoc->m_markStartMs = 0;
                if (m_pEditorDoc->m_markEndMs > m_pEditorDoc->m_docLengthMs)
                    m_pEditorDoc->m_markEndMs = m_pEditorDoc->m_docLengthMs;
            } 

            if (status != INITIALIZE && lastX > 0) {
                /*/
                // This is to make redrawing much more efficient.
                // Create a region to redraw
                CRgn clipRgn;
                if (lastX > x)
                clipRgn.CreateRectRgn(x-1, rcStreamView.top, lastX+1, rcStreamView.bottom);
                else
                clipRgn.CreateRectRgn(lastX-1, rcStreamView.top, x+1, rcStreamView.bottom);

                //RedrawWindow(NULL, &clipRgn, RDW_INVALIDATE);
                clipRgn.DeleteObject();

                // The timeline control also has to be redrawn
                // in order to visualize the mark positions;
                // to make it more efficient and less flickery,
                // create a redraw region for this control, too.
                CWnd *pTL = &((CStreamView *) GetParent())->m_timelineCtrl;
                CRect timeLineRect;
                pTL->GetClientRect(&timeLineRect);

                if (lastX > x)
                clipRgn.CreateRectRgn(x-10-LEFT_BORDER, timeLineRect.top, lastX+10-LEFT_BORDER, timeLineRect.bottom);
                else
                clipRgn.CreateRectRgn(lastX-10-LEFT_BORDER, timeLineRect.top, x+10-LEFT_BORDER, timeLineRect.bottom);
                //pTL->RedrawWindow(NULL, &clipRgn, RDW_INVALIDATE);
                clipRgn.DeleteObject();
                /*/

                GetParent()->RedrawWindow(NULL, NULL, RDW_INVALIDATE);
            } else {
                GetParent()->RedrawWindow(NULL, NULL, RDW_INVALIDATE);
            }

            lastX = x;
        }
    }
}

void CStreamCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
    // 3 Dinge werden hier getan:
    // - Zu verschiebende Stopp- oder Zielmarken ermitteln ODER
    // - Zu verschiebenden Clip ermitteln ODER
    // - Anfangsposition der Auswahl setzen
    // - Die Mausposition wird aber in/für beiden Fällen gespeichert

    if (!m_pEditorDoc || m_pEditorDoc->project.IsEmpty() || m_pEditorDoc->IsPreviewActive())
        return;

    CRect windowRect;
    GetWindowRect(&windowRect);

    m_iPixelDiff = 0;

    // initialize variables for select area
    m_pEditorDoc->m_markStartMs = -1;
    m_pEditorDoc->m_markEndMs   = -1;

	//initialize variables for timeline select object
	m_bMoveSelObj=false;
	m_bResizeLeftSelObj=false;
	m_bResizeRightSelObj=false;
	bool bOverSelectedObjects = false;
	m_pUsedArea = NULL;

    // initialize variables for mark stream
    m_bMoveMark = false;
	m_bMoveDemoMark = false;
    m_bMouseMoveOutOfMarksStream = false;

    // initialize variables for clip stream
    m_bMoveClip = false;
    m_arSelectedClips.RemoveAll();
    if (m_pEditorDoc->GetWhiteboardView()&&!PointIsInSlidesStream(point)&&!PointIsInMarksStream(point))
	{
		m_pEditorDoc->GetWhiteboardView()->DeleteSelectionArray();
		CInteractionStructureView *pInteractionStructureView = 
			CMainFrameE::GetCurrentInstance()->GetInteractionStructureView();
		if (pInteractionStructureView)
			pInteractionStructureView->RefreshList();
		m_pEditorDoc->GetWhiteboardView()->RedrawWindow();
	}
    // initialize variables for moving marks or clips
    m_iMoveDistanceMs = 0;
    m_iMousePositionInClipPx = 0;

    // Rechteck in dem die Ströme gezeichnet werden
    CRect rect;
    GetClientRect(&rect);
    rect.left += LEFT_BORDER;

    // mouse in mark stream. 
    if (PointIsInMarksStream(point)) {
        CMarkStream *pMarkStream = m_pEditorDoc->project.GetMarkStream();
        if (pMarkStream) {
            int nFromMs = GetMsecFromPixel(point.x - rect.left - SELECTION_OFFSET);
            int nToMs = GetMsecFromPixel(point.x - rect.left + SELECTION_OFFSET);
            HRESULT hr = pMarkStream->SelectMark(nFromMs, nToMs-nFromMs);
            CInteractionStream *pInteractionStream = m_pEditorDoc->project.GetInteractionStream(false);
            if (hr == S_OK) {
                // if we are in screen simultation, the selected mark is at the end of page and document has "next" demo buttons don't allow movement.
                if (m_pEditorDoc->ShowAsScreenSimulation() && pMarkStream->SelectedStopMarkIsDemoDocumentObject() && m_pEditorDoc->IsDemoDocument()) {
                    //pMarkStream->ResetMoveSelected();
                    m_bMoveDemoMark = true;
					pMarkStream->GetMovedMarkTimes(nFromMs,nToMs);
					pMarkStream->ResetMoveSelected();
                    m_iDemoMarkOrigMs = nFromMs;
					 CArray<CInteractionAreaEx*, CInteractionAreaEx*>aRelatedDemoObjects;
					hr = pInteractionStream->FindRelatedDemoObjects(m_iDemoMarkOrigMs, &aRelatedDemoObjects);
					if (SUCCEEDED(hr) && aRelatedDemoObjects.GetCount() == 1 && m_pEditorDoc->GetWhiteboardView()) {
						m_pEditorDoc->GetWhiteboardView()->DeleteSelectionArray();
						aRelatedDemoObjects.GetAt(0)->SetSelected(true);
						m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().Add(aRelatedDemoObjects.GetAt(0));
						m_pEditorDoc->GetWhiteboardView()->UpdateSelectionArray();
						CInteractionStructureView *pInteractionStructureView = 
						CMainFrameE::GetCurrentInstance()->GetInteractionStructureView();
						if (pInteractionStructureView)
							pInteractionStructureView->RefreshList();
						m_pEditorDoc->GetWhiteboardView()->Invalidate();
					}
				}
                    m_bMoveMark = true;
            }
        }
    }

	//mouse in timeline selected object
	if (PointIsInSlidesStream(point) && m_pEditorDoc->GetWhiteboardView())
	{
		CRect rcTmp;
		CInteractionAreaEx* pArea;
		for(int i=0;i<m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetCount();i++)
		{
			pArea = m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetAt(i);
			if(pArea)
			{
				rcTmp.CopyRect(pArea->GetTimelineRect());
				if (rcTmp.Width() < MIN_TIMELINE_WIDTH)
				{
					rcTmp.left = rcTmp.left - (MIN_TIMELINE_WIDTH - rcTmp.Width()) / 2;
					rcTmp.right = pArea->GetTimelineRect().right + (MIN_TIMELINE_WIDTH - pArea->GetTimelineRect().Width()) / 2 + 1;
				}
				if(rcTmp.PtInRect(point) && pArea->IsSelected())
				{
					rcTmp.CopyRect(pArea->GetTimelineRect());
					if (rcTmp.Width() < MIN_TIMELINE_WIDTH)
						rcTmp.left = rcTmp.left - (MIN_TIMELINE_WIDTH - rcTmp.Width()) / 2;
					rcTmp.right = rcTmp.left + 3;
					if( rcTmp.PtInRect(point))
					{
						m_bResizeLeftSelObj=true;
						m_pUsedArea = pArea;
					}
					else
					{
						rcTmp.CopyRect(pArea->GetTimelineRect());
						if (rcTmp.Width() < MIN_TIMELINE_WIDTH)
							rcTmp.right = pArea->GetTimelineRect().right + (MIN_TIMELINE_WIDTH - pArea->GetTimelineRect().Width()) / 2 + 1;
						rcTmp.left = rcTmp.right-3;
						if(rcTmp.PtInRect(point))
						{
							m_bResizeRightSelObj=true;
							m_pUsedArea = pArea;
						}
						else
							m_bMoveSelObj = true;
					}
					break;
				}
			}
		}
		if(!m_bMoveSelObj && !m_bResizeLeftSelObj && !m_bResizeRightSelObj){
			//select first displayed object on position
			bool bGraphic = PointIsInGraphicObjects(point);
			bool bInter = PointIsInInteractionObjects(point);
			bool bQuest = PointIsInQuestionObjects(point);
			CArray<CInteractionAreaEx*, CInteractionAreaEx*>aObjects;
			HRESULT hr;
			CTimePeriod tp;tp.Init(INTERACTION_PERIOD_TIME,GetMsecFromPixel(point.x -4 -LEFT_BORDER,true),GetMsecFromPixel(point.x +5 -LEFT_BORDER,true));
			if (m_pEditorDoc->project.GetInteractionStream()){
				hr = m_pEditorDoc->project.GetInteractionStream(false)->FindInteractions(&tp,false, &aObjects);
				if (SUCCEEDED(hr)){
					for (int i=0;i<aObjects.GetCount();i++){
						if (aObjects.GetAt(i)->IsTimelineDisplay())
							if ((bGraphic && aObjects.GetAt(i)->GetTypeId() == INTERACTION_RECORD_GRAPHIC_OBJECTS)||
								(bInter && (aObjects.GetAt(i)->GetTypeId() == INTERACTION_RECORD_ELEMENTS  || (aObjects.GetAt(i)->GetTypeId()==INTERACTION_RECORD_GRAPHIC_OBJECTS && aObjects.GetAt(i)->IsDemoDocumentObject())))||
								(bQuest && aObjects.GetAt(i)->GetTypeId() != INTERACTION_RECORD_GRAPHIC_OBJECTS && aObjects.GetAt(i)->GetTypeId() != INTERACTION_RECORD_ELEMENTS))
							{
								//whiteboard update
								m_pEditorDoc->GetWhiteboardView()->DeleteSelectionArray();
								aObjects.GetAt(i)->SetSelected(true);
								m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().Add(aObjects.GetAt(i));
								m_pEditorDoc->GetWhiteboardView()->UpdateSelectionArray();
								CInteractionStructureView *pInteractionStructureView = 
									CMainFrameE::GetCurrentInstance()->GetInteractionStructureView();
								if (pInteractionStructureView)
									pInteractionStructureView->RefreshList();
								m_pEditorDoc->GetWhiteboardView()->RedrawWindow();
								m_bMoveSelObj = true;
								break;
							}
					}
				}
			}
		}
	}

    if (PointIsInClipsStream(point) && !m_bMoveMark) {
        // Mousepointer over clipstream rectangle
        for (int i = 0; i < m_arClipStreamRect.GetSize(); ++i) {
            if (m_arClipStreamRect[i].PtInRect(point)) { 	
                // Mouseposition in milli seconds
                int iMouseClickMsec = GetMsecFromPixel(point.x - rect.left + 1);
                // TODO remove +1 due to round down scale up problem with positions and pixels

                // Mousepointer over clip rectangle
                CClipStreamR *pClipStream = m_pEditorDoc->project.GetClipStream();

                if (pClipStream != NULL) {
                    HRESULT hr = pClipStream->SelectClipAt(iMouseClickMsec);
                    if (hr == S_OK) {
                        m_bMoveClip = true;

                        // TODO in the case of a clip with inserted audio you have a gap
                        // in all the other streams (they contain no data at this clips position).
                        // This gab should also be moved (and is in 1.7) but isn't at the moment (1.8).
                        // See also Bugs and Issues #, #, # in Bugzilla.
                        //m_bMoveAudioClip = m_pEditorDoc->project.IsClipWithAudioInsertion(iMouseClickMsec);
                    }
                    _ASSERT(SUCCEEDED(hr));
                }

                // if selection was successful or not the search is over
                break;

                /* TODO remove
                m_pEditorDoc->project.FindAssociateClipsAt(iMouseClickMsec, m_arSelectedClips);
                if (m_arSelectedClips.GetSize() > 0)
                {
                // m_arSelectedClips enthält einen Clip
                // dieser kann (nach vorherigem Verschieben, Schneiden) aber aus 
                // mehreren kleinen Stückchen bestehen

                m_iMousePositionInClipPx = GetMsecFromPixel(point.x - LEFT_BORDER, false) - m_arSelectedClips[0]->GetTimestamp(); // - GetPixelFromMsec(m_arSelectedClips[0]->GetTimestamp() - m_pEditorDoc->m_displayStartMs);
                m_bMoveClip = true;
                break;
                }
                */
            }   
        }	
    }
    if (point.x > LEFT_BORDER) {
        //UpdateCurrentPos(point);
        m_markStartPixel = point.x;
        m_lastPixel      = point.x;
		m_lastMs         = GetMsecFromPixel(point.x,m_bResizeLeftSelObj||m_bResizeRightSelObj||m_bMoveSelObj||m_bMoveDemoMark);
        m_iButtonStatus  = BUTTON_DOWN;
        if (m_bMoveClip || m_bMoveMark ||m_bMoveSelObj || m_bResizeLeftSelObj || m_bResizeRightSelObj) {
            GetParent()->RedrawWindow(NULL, NULL, RDW_INVALIDATE);
        } else {
            // Wenn nicht über Clip oder Markierung: Auswahl verändern
            UpdateSelection(point.x, INITIALIZE);
        }
        SetCapture();
    }
    CWnd::OnLButtonDown(nFlags, point);
}

void CStreamCtrl::RemoveSlection() {
    m_iPixelDiff = 0;
    // initialize variables for select area
    m_pEditorDoc->m_markStartMs = -1;
    m_pEditorDoc->m_markEndMs   = -1;

    // initialize variables for mark stream
    m_bMoveMark = false;
	m_bMoveDemoMark = false;
    m_bMouseMoveOutOfMarksStream = false;

	//initialize variables for timeline selected object
	m_bMoveSelObj=false;
	m_bResizeLeftSelObj=false;
	m_bResizeRightSelObj=false;
	m_pUsedArea = NULL;

    // initialize variables for clip stream
    m_bMoveClip = false;
    // initialize variables for moving marks or clips
    m_iMoveDistanceMs = 0;
    m_iMousePositionInClipPx = 0;

    UpdateSelection(0, INITIALIZE);
}

void CStreamCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_pEditorDoc || m_pEditorDoc->project.IsEmpty() || m_pEditorDoc->IsPreviewActive())
		return;
	
	if (m_iButtonStatus == BUTTON_DOWN)
	{
		UpdateCurrentPos(point);
		ReleaseCapture();
		
		if (m_bMoveMark)
		{
			// move selected mark if mouse is over marks stream
			CMarkStream *pMarkStream = m_pEditorDoc->project.GetMarkStream();
			if (pMarkStream)
			{
            HRESULT hr = S_OK;
				
            if (!m_bMouseMoveOutOfMarksStream) {     // move
                //pMarkStream->s
                if (!m_bMoveDemoMark)
					hr = pMarkStream->CommitMoveSelected();
                else
                    {
						hr = m_pEditorDoc->project.GetInteractionStream()->ChangeRelatedDemoObjectsTimes(m_iDemoMarkOrigMs, m_iDemoMarkOrigMs, m_iDemoMarkOrigMs + GetMsecFromPixel(point.x)-m_lastMs);
						if(SUCCEEDED(hr))
							m_pEditorDoc->SetModifiedFlag();
						m_iDemoMarkOrigMs = -1;
						CInteractionStructureView *pInteractionStructureView = 
							CMainFrameE::GetCurrentInstance()->GetInteractionStructureView();
						if (pInteractionStructureView)
							pInteractionStructureView->RefreshList();
                        // TODO soare: add proper undo redo
                    }
            }	else	                                 // drag away
            {
               // TODO do this properly:
				int iResult = IDYES;
			   if (m_bMoveDemoMark)
			   {
				   pMarkStream->SelectMark(m_iDemoMarkOrigMs,1);
			   }
               if (pMarkStream->SelectedMarkIsUsed())
               {
                  CString csCaption;
                  csCaption.LoadString(IDS_QUESTION);
                  CString csMessage;
                  if (pMarkStream->SelectedStopMarkIsDemoDocumentObject()) {
                      //
                      // TODO soare: add DE resource string
                      csMessage.LoadString(IDS_DELETE_DEMO_MARK);
                  } else {
                      csMessage.LoadString(IDS_DELETE_USED_MARK);
                  }
					  iResult = MessageBox(csMessage, csCaption, MB_YESNO | MB_TOPMOST); 
               }

               if (iResult == IDYES) {
                   if (m_bMoveDemoMark) {
					   hr = pMarkStream->ResetMoveSelected();
                       hr = m_pEditorDoc->project.GetInteractionStream()->DeleteRelatedDemoObject(m_iDemoMarkOrigMs);
					   CInteractionStructureView *pInteractionStructureView = 
							CMainFrameE::GetCurrentInstance()->GetInteractionStructureView();
						if (pInteractionStructureView)
							pInteractionStructureView->RefreshList();
                   } 
				   else 
					   hr = pMarkStream->RemoveSelectedMark();
               }  else {
                   hr = pMarkStream->ResetMoveSelected();
               }
            }
				
            if (SUCCEEDED(hr) && hr == S_OK)
            {
               // something was really changed
				  
               ((CStreamView *)GetParent())->RedrawStreamCtrl(NULL);
					//RedrawWindow(&m_rcMarksStreamRect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
					
               m_pEditorDoc->SetModifiedFlag();
               if (m_bMoveDemoMark) {
                   m_pEditorDoc->GetWhiteboardView()->RedrawWindow();
                   m_bMoveDemoMark = false;
               }
            }
			}
		}
		else if(m_bMoveSelObj && m_pEditorDoc->GetWhiteboardView())
		{
			//set sel obj time
			CInteractionAreaEx* pInteraction;
			bool bMove = true;
			for(int i=0;i<m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetCount();i++)
			{
				pInteraction = m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetAt(i);
				if(pInteraction && pInteraction->IsSelected())
				{
					int iPixelMs = GetMsecFromPixel(point.x);
					int dif = iPixelMs - m_lastMs;
					int visStart= pInteraction->GetVisibilityStart();
					int visEnd = pInteraction->GetVisibilityStart()+pInteraction->GetVisibilityLength()-1;
					visStart+=dif;
					visEnd+=dif;
					if(visStart<0)
					{
						visStart = 0;
						visEnd = pInteraction->GetVisibilityLength()-1;
					}
					if(visEnd > m_pEditorDoc->m_docLengthMs)
					{
						visEnd = m_pEditorDoc->m_docLengthMs;
						visStart = visEnd - pInteraction->GetVisibilityLength()+1;
					}
					CInteractionStream *pInteractionStream = m_pEditorDoc->project.GetInteractionStream();
					if(pInteractionStream != NULL){
						CArray<CInteractionAreaEx*, CInteractionAreaEx*>aRelatedDemoObjects;
						HRESULT hr;
						hr = pInteractionStream->FindRelatedDemoObjects(visEnd, &aRelatedDemoObjects);
						if (!SUCCEEDED(hr) || aRelatedDemoObjects.GetCount() != 0){
							bMove = false;
							break;
						}
					}
				}
			}
			if(bMove)
			{
				for(int i=0;i<m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetCount();i++)
				{
					pInteraction = m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetAt(i);
					if(pInteraction && pInteraction->IsSelected())
					{
						int iPixelMs = GetMsecFromPixel(point.x);
						int dif = iPixelMs - m_lastMs;
						int visStart= pInteraction->GetVisibilityStart();
						int visEnd = pInteraction->GetVisibilityStart()+pInteraction->GetVisibilityLength()-1;
						visStart+=dif;
						visEnd+=dif;
						if(visStart<0)
						{
							visStart = 0;
							visEnd = pInteraction->GetVisibilityLength()-1;
						}
						if(visEnd > m_pEditorDoc->m_docLengthMs)
						{
							visEnd = m_pEditorDoc->m_docLengthMs;
							visStart = visEnd - pInteraction->GetVisibilityLength()+1;
						}
						CInteractionStream *pInteractionStream = m_pEditorDoc->project.GetInteractionStream();
						if(pInteractionStream != NULL){
							CArray<CInteractionAreaEx*, CInteractionAreaEx*>aRelatedDemoObjects;
							HRESULT hr;
							hr = pInteractionStream->FindRelatedDemoObjects(visEnd, &aRelatedDemoObjects);
							if (SUCCEEDED(hr) && aRelatedDemoObjects.GetCount() == 0){
								CTimePeriod pctpNew;
								pctpNew.Init(INTERACTION_PERIOD_TIME, visStart,visEnd);
								pInteraction->SetVisibilityPeriod(&pctpNew);
								m_pEditorDoc->SetModifiedFlag();
							}
						}
					}
				}
				CInteractionStructureView *pInteractionStructureView = 
					CMainFrameE::GetCurrentInstance()->GetInteractionStructureView();
				//pInteractionStructureView->UpdateWindow();
				if (pInteractionStructureView)
					pInteractionStructureView->RefreshList();
			}
		}
		else if(m_bResizeLeftSelObj)
		{
			if(m_pUsedArea)
			{
				int iPixelMs = GetMsecFromPixel(point.x);
				int dif = iPixelMs - m_lastMs;
				int visStart= m_pUsedArea->GetVisibilityStart();
				int visEnd = m_pUsedArea->GetVisibilityStart()+m_pUsedArea->GetVisibilityLength()-1;
				visStart+=dif;
				if(visEnd > visStart)
				{
					if(visStart<0)
					{
						visStart = 0;
						visEnd = m_pUsedArea->GetVisibilityLength()-1;
					}
					CTimePeriod pctpNew;
					pctpNew.Init(INTERACTION_PERIOD_TIME, visStart,visEnd);
					m_pUsedArea->SetVisibilityPeriod(&pctpNew);
					m_pEditorDoc->SetModifiedFlag();
					CInteractionStructureView *pInteractionStructureView = 
						CMainFrameE::GetCurrentInstance()->GetInteractionStructureView();
					//pInteractionStructureView->UpdateWindow();
					if (pInteractionStructureView)
						pInteractionStructureView->RefreshList();
				}
			}
		}
		else if(m_bResizeRightSelObj)
		{
			if(m_pUsedArea)
			{
			int iPixelMs = GetMsecFromPixel(point.x);
			int dif = iPixelMs - m_lastMs;
			int visStart= m_pUsedArea->GetVisibilityStart();
			int visEnd = m_pUsedArea->GetVisibilityStart()+m_pUsedArea->GetVisibilityLength()-1;
			visEnd+=dif;
			if(visStart < visEnd)
			{
			if(visEnd >= m_pEditorDoc->m_docLengthMs)
			{
				visEnd = m_pEditorDoc->m_docLengthMs;
				visStart = visEnd - m_pUsedArea->GetVisibilityLength()+1;
			}
			CInteractionStream *pInteractionStream = m_pEditorDoc->project.GetInteractionStream();
			if(pInteractionStream != NULL){
				CArray<CInteractionAreaEx*, CInteractionAreaEx*>aRelatedDemoObjects;
				HRESULT hr;
				hr = pInteractionStream->FindRelatedDemoObjects(visEnd, &aRelatedDemoObjects);
				if (SUCCEEDED(hr) && aRelatedDemoObjects.GetCount() == 0){
					CTimePeriod pctpNew;
					pctpNew.Init(INTERACTION_PERIOD_TIME, visStart,visEnd);
					m_pUsedArea->SetVisibilityPeriod(&pctpNew);
					m_pEditorDoc->SetModifiedFlag();
				}
			}

			CInteractionStructureView *pInteractionStructureView = 
            CMainFrameE::GetCurrentInstance()->GetInteractionStructureView();
         //pInteractionStructureView->UpdateWindow();
         if (pInteractionStructureView)
            pInteractionStructureView->RefreshList();
		 m_pEditorDoc->SetModifiedFlag();
			}
			}
		}
		else if (m_bMoveClip)
      {
         
         CClipStreamR *pClipStream = m_pEditorDoc->project.GetClipStream();
         
         if (pClipStream != NULL)
         {
            HRESULT hr = pClipStream->CommitMoveSelected();
				
            if (SUCCEEDED(hr) && hr != S_FALSE)
            {
               m_pEditorDoc->NotifyCommitMoveClip();


            }
            
         }
		}
		else
		{
			// update selection
			if (point.x != m_markStartPixel && point.x > LEFT_BORDER)
				UpdateSelection(point.x, MODIFY);
		}
	}
	
	// reset variables
	m_bMoveMark = false;
	m_bMoveDemoMark = false;
	m_bMouseMoveOutOfMarksStream = false;
	m_bMoveClip = false;
	m_iButtonStatus = BUTTON_UP;

	m_bMoveSelObj = false;
	m_bResizeLeftSelObj = false;
	m_bResizeRightSelObj=false;
	m_pUsedArea = NULL;
	

	CWnd::OnLButtonUp(nFlags, point);
}

void CStreamCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Siehe auch OnLButtonDown()
	// Verschiebt Clip-Rechteck ODER verändert Auswahl
	
	// Linke Maustaste ist gedrückt
	if (m_iButtonStatus == BUTTON_DOWN)
	{
        if (!m_pEditorDoc || m_pEditorDoc->project.IsEmpty() || m_pEditorDoc->IsPreviewActive()) {
		    return;
        }

		UpdateCurrentPos(point);
		if (m_bMoveMark)
		{	
			CMarkStream *pMarkStream = m_pEditorDoc->project.GetMarkStream();
			if (pMarkStream)
			{
				if (point.y >= m_rcMarksStreamRect.top && point.y < m_rcMarksStreamRect.bottom)
				{
					if(m_bMoveDemoMark)
					{
						HRESULT hr;
						int nDiffMs = GetMsecFromPixel(point.x) - m_lastMs;
						hr = m_pEditorDoc->project.GetInteractionStream()->ChangeRelatedDemoObjectsTimes(m_iDemoMarkOrigMs, m_iDemoMarkOrigMs, m_iDemoMarkOrigMs + nDiffMs);
						if(SUCCEEDED(hr))
							m_pEditorDoc->SetModifiedFlag();
						m_iDemoMarkOrigMs += nDiffMs;
						m_lastMs = GetMsecFromPixel(point.x);
						CInteractionStructureView *pInteractionStructureView = 
							CMainFrameE::GetCurrentInstance()->GetInteractionStructureView();
						if (pInteractionStructureView)
							pInteractionStructureView->RefreshList();
					}
					else
					{
					pMarkStream->EnableSelectedMark(true);
					int nDiffMs = GetMsecFromPixel(point.x, false) - m_lastMs;
					pMarkStream->MoveSelectedMark(nDiffMs);
					m_lastMs += nDiffMs;
					}
					m_bMouseMoveOutOfMarksStream = false;
					::SetCursor(m_hMoveCursor);
				}
				else
				{
					m_bMouseMoveOutOfMarksStream = true;
					pMarkStream->EnableSelectedMark(false);
					::SetCursor(m_hDeleteCursor);
				}
         }
         ((CStreamView *)GetParent())->RedrawStreamCtrl(NULL);
			//RedrawWindow(&m_rcMarksStreamRect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
		else if(m_bMoveSelObj && m_pEditorDoc->GetWhiteboardView())
		{
			//move sel obj
			CInteractionAreaEx* pInteraction;
			bool bMove = true;
			int iPixelMs = m_lastMs;
			for(int i=0;i<m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetCount();i++)
			{
				pInteraction = m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetAt(i);
				if(pInteraction && pInteraction->IsSelected())
				{
					iPixelMs = GetMsecFromPixel(point.x);
					int dif = iPixelMs - m_lastMs;
					int visStart= pInteraction->GetVisibilityStart();
					int visEnd = pInteraction->GetVisibilityStart()+pInteraction->GetVisibilityLength()-1;
					visStart+=dif;
					visEnd+=dif;
					if(visStart<0)
					{
						visStart = 0;
						visEnd = pInteraction->GetVisibilityLength()-1;
					}
					if(visEnd > m_pEditorDoc->m_docLengthMs)
					{
						visEnd = m_pEditorDoc->m_docLengthMs;
						visStart = visEnd - pInteraction->GetVisibilityLength()+1;
					}
					CInteractionStream *pInteractionStream = m_pEditorDoc->project.GetInteractionStream();
					if(pInteractionStream != NULL){
						CArray<CInteractionAreaEx*, CInteractionAreaEx*>aRelatedDemoObjects;
						HRESULT hr;
						hr = pInteractionStream->FindRelatedDemoObjects(visEnd, &aRelatedDemoObjects);
						if (!SUCCEEDED(hr) || aRelatedDemoObjects.GetCount() != 0){
							bMove = false;
							break;
						}
					}
				}
			}
			if(bMove)
			{
				for(int i=0;i<m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetCount();i++)
				{
					pInteraction = m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetAt(i);
					if(pInteraction && pInteraction->IsSelected())
					{
						iPixelMs = GetMsecFromPixel(point.x);
						int dif = iPixelMs - m_lastMs;
						int visStart= pInteraction->GetVisibilityStart();
						int visEnd = pInteraction->GetVisibilityStart()+pInteraction->GetVisibilityLength()-1;
						visStart+=dif;
						visEnd+=dif;
						if(visStart<0)
						{
							visStart = 0;
							visEnd = pInteraction->GetVisibilityLength()-1;
						}
						if(visEnd > m_pEditorDoc->m_docLengthMs)
						{
							visEnd = m_pEditorDoc->m_docLengthMs;
							visStart = visEnd - pInteraction->GetVisibilityLength()+1;
						}
						CInteractionStream *pInteractionStream = m_pEditorDoc->project.GetInteractionStream();
						if(pInteractionStream != NULL){
							CArray<CInteractionAreaEx*, CInteractionAreaEx*>aRelatedDemoObjects;
							HRESULT hr;
							hr = pInteractionStream->FindRelatedDemoObjects(visEnd, &aRelatedDemoObjects);
							if (SUCCEEDED(hr) && aRelatedDemoObjects.GetCount() == 0){
								CTimePeriod pctpNew;
								pctpNew.Init(INTERACTION_PERIOD_TIME, visStart,visEnd);
								pInteraction->SetVisibilityPeriod(&pctpNew);
								m_pEditorDoc->SetModifiedFlag();
								//m_lastMs = iPixelMs;
							}
						}
					}
				}
				m_lastMs = iPixelMs;
				((CStreamView *)GetParent())->RedrawStreamCtrl(NULL);
				m_pEditorDoc->GetWhiteboardView()->RedrawWindow();
			}
		}
		else if(m_bResizeLeftSelObj)
		{
            if(m_pUsedArea)
            {
                int iPixelMs = GetMsecFromPixel(point.x);
                int dif = iPixelMs - m_lastMs;
                int visStart= m_pUsedArea->GetVisibilityStart();
                int visEnd = m_pUsedArea->GetVisibilityStart()+m_pUsedArea->GetVisibilityLength()-1;
                visStart+=dif;
                if(visEnd > visStart)
                {
                    if (visEnd < visStart + 10) 
                    {
                        visStart = visEnd - 10;
                    }
                    if(visStart<0)
                    {
                        visStart = 0;
                    }
                    
                    CTimePeriod pctpNew;
                    pctpNew.Init(INTERACTION_PERIOD_TIME, visStart,visEnd);
                    m_pUsedArea->SetVisibilityPeriod(&pctpNew);
                    m_pEditorDoc->SetModifiedFlag();
                    m_lastMs = iPixelMs;
                    ((CStreamView *)GetParent())->RedrawStreamCtrl(NULL);
                    m_pEditorDoc->GetWhiteboardView()->RedrawWindow();
                }
            }
		}
		else if(m_bResizeRightSelObj)
		{
            if(m_pUsedArea)
            {
                int iPixelMs = GetMsecFromPixel(point.x);
                int dif = iPixelMs - m_lastMs;
                int visStart= m_pUsedArea->GetVisibilityStart();
                int visEnd = m_pUsedArea->GetVisibilityStart()+m_pUsedArea->GetVisibilityLength()-1;
                visEnd+=dif;
                if(visStart < visEnd)
                {
                    if(visEnd < visStart + 10)
                    {
                        visEnd = visStart + 10;
                    }
                    if(visEnd > m_pEditorDoc->m_docLengthMs)
                    {
                        visEnd = m_pEditorDoc->m_docLengthMs;
                    }
                    
                    CInteractionStream *pInteractionStream = m_pEditorDoc->project.GetInteractionStream();
                    if(pInteractionStream != NULL){
                        CArray<CInteractionAreaEx*, CInteractionAreaEx*>aRelatedDemoObjects;
                        HRESULT hr;
                        hr = pInteractionStream->FindRelatedDemoObjects(visEnd, &aRelatedDemoObjects);
                        if (SUCCEEDED(hr) && aRelatedDemoObjects.GetCount() == 0){
                            CTimePeriod pctpNew;
                            pctpNew.Init(INTERACTION_PERIOD_TIME, visStart,visEnd);
                            m_pUsedArea->SetVisibilityPeriod(&pctpNew);
                            m_pEditorDoc->SetModifiedFlag();
                            m_lastMs = iPixelMs;
                        }
                    }
                    ((CStreamView *)GetParent())->RedrawStreamCtrl(NULL);
                    m_pEditorDoc->GetWhiteboardView()->RedrawWindow();
                }
            }
		}
		else if (m_bMoveClip)
		{ 
        
         CClipStreamR *pClipStream = m_pEditorDoc->project.GetClipStream();
         
         if (pClipStream != NULL)
         {
            int iPixelMs = GetMsecFromPixel(point.x, false);
				
            HRESULT hr = pClipStream->MoveSelectedClip(iPixelMs - m_lastMs);
				
            m_lastMs = iPixelMs;
				
            
            if (SUCCEEDED(hr))
            {
               ((CStreamView *)GetParent())->RedrawStreamCtrl(NULL);
               //RedrawWindow(&m_rcClipStreamRect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
            }
				
            // TODO: SetCursorClipRect()
         }
			
		}
		else
		{
			//if (point.x > LEFT_BORDER)
			UpdateSelection(point.x, MODIFY);
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

int CStreamCtrl::GetMsecFromPixel(int iPixelPos, bool bAddDisplayStartMs)
{ 
    if (!m_pEditorDoc)
        return 0;

    CRect crClient;
    GetClientRect(&crClient);
    crClient.left += LEFT_BORDER;

    int widthInMs    = m_pEditorDoc->m_displayEndMs - m_pEditorDoc->m_displayStartMs;
    int widthInPixel = crClient.Width();

    int iMsPos = Calculator::GetMsecFromPixel(iPixelPos, widthInPixel, widthInMs);

    if (bAddDisplayStartMs)
        iMsPos += m_pEditorDoc->m_displayStartMs;

    return iMsPos;
}

int CStreamCtrl::GetPixelFromMsec(int iMsec, int iOffset)
{ 
    if (!m_pEditorDoc)
        return 0;

    CRect crClient;
    GetClientRect(&crClient);
    crClient.left += LEFT_BORDER;
    return Calculator::GetPixelFromMsec(iMsec, crClient.Width(), m_pEditorDoc->m_displayEndMs - m_pEditorDoc->m_displayStartMs, iOffset);
}

int CStreamCtrl::GetPixelFromMsec(int iMsec, int iOffset, CRect rcClient) { 
    return Calculator::GetPixelFromMsec(iMsec, rcClient.Width(), m_pEditorDoc->m_displayEndMs - m_pEditorDoc->m_displayStartMs, iOffset);
}

void CStreamCtrl::UpdateCurrentPos(CPoint &point)
{
    if (m_pEditorDoc->project.IsEmpty())
        return;

    CRect clientRect;
    GetClientRect(&clientRect);

    int iDiffX = 0;
    if (point.x <= clientRect.left + LEFT_BORDER)
    {
        point.x = clientRect.left + LEFT_BORDER;
        //m_lastMs += GetMsecFromPixel(10);
        iDiffX = -10;
    }
    else if (point.x+1 >= clientRect.right)
    {
        point.x = clientRect.right;
        //m_lastMs -= GetMsecFromPixel(10);
        iDiffX = 10;
    }
    else
        return;        

    double displayWidthMs = m_pEditorDoc->m_displayEndMs - m_pEditorDoc->m_displayStartMs;
    double displayWidthPx = clientRect.right - clientRect.left;

    int newPos;
    double dDiffMs = (((double) iDiffX) / displayWidthPx) * displayWidthMs;
    if (iDiffX > 0)
        newPos = m_pEditorDoc->m_displayEndMs + (int) dDiffMs;
    else
        newPos = m_pEditorDoc->m_displayStartMs + (int) dDiffMs;


    if (newPos < 0)
    {
        dDiffMs = -m_pEditorDoc->m_displayStartMs;
        iDiffX = (int)((dDiffMs / displayWidthMs) * displayWidthPx);
        newPos = 0;
    }
    else if (newPos > m_pEditorDoc->m_docLengthMs)
    {
        dDiffMs = m_pEditorDoc->m_docLengthMs - m_pEditorDoc->m_displayEndMs;
        iDiffX = (int)((dDiffMs / displayWidthMs) * displayWidthPx);
        newPos = m_pEditorDoc->m_docLengthMs;
    }

    m_lastMs -= (int) dDiffMs;

    m_markStartPixel -= iDiffX;

    if (newPos > m_pEditorDoc->m_displayEndMs)
    {
        int displayWidth = m_pEditorDoc->m_displayEndMs - m_pEditorDoc->m_displayStartMs;
        m_pEditorDoc->m_displayEndMs = newPos;
        m_pEditorDoc->m_displayStartMs = m_pEditorDoc->m_displayEndMs - displayWidth;

        //m_pEditorDoc->UpdateAllViews(NULL, TLC_MOUSEMOVE);
        //m_pEditorDoc->UpdateAllViews(NULL, UPDATE_ZOOM);
    }
    else if (newPos < m_pEditorDoc->m_displayStartMs)
    {
        int displayWidth = m_pEditorDoc->m_displayEndMs - m_pEditorDoc->m_displayStartMs;
        m_pEditorDoc->m_displayStartMs = newPos;
        m_pEditorDoc->m_displayEndMs = m_pEditorDoc->m_displayStartMs + displayWidth;

        //m_pEditorDoc->UpdateAllViews(NULL, TLC_MOUSEMOVE);
        //m_pEditorDoc->UpdateAllViews(NULL, UPDATE_ZOOM);
    }

}

void CStreamCtrl::CalculateStreamRectangles()
{
    CRect rcClient;
    GetClientRect (&rcClient);

    m_rcMarksStreamRect.SetRectEmpty();
    m_rcSlidesStreamRect.SetRectEmpty();
    m_rcVideoStreamRect.SetRectEmpty();
    m_rcClipStreamRect.SetRectEmpty();
    m_rcAudioStreamRect.SetRectEmpty();

    if (m_pEditorDoc && !m_pEditorDoc->project.IsEmpty())
    {
        int iStreamTop = rcClient.top + 1;
        // marks stream
        if (m_bShowMarksStream)
        {
            m_rcMarksStreamRect.SetRect(rcClient.left + LEFT_BORDER, iStreamTop,
                rcClient.right, iStreamTop + STREAM_HEIGHT);
            iStreamTop += STREAM_HEIGHT;
        }

        // slides stream
        if (m_bShowSlidesStream)
        {
            m_rcSlidesStreamRect.SetRect(rcClient.left + LEFT_BORDER, iStreamTop,
                rcClient.right, iStreamTop + STREAM_HEIGHT*3);
            iStreamTop += STREAM_HEIGHT*3;
        }

        // video stream
        if (m_bShowVideoStream)
        {
            m_rcVideoStreamRect.SetRect(rcClient.left + LEFT_BORDER, iStreamTop,
                rcClient.right, iStreamTop + STREAM_HEIGHT);
            iStreamTop += STREAM_HEIGHT;
        }

        // clip stream
        if (m_bShowClipsStream)
        {
            m_rcClipStreamRect.SetRect(rcClient.left + LEFT_BORDER, iStreamTop,
                rcClient.right, iStreamTop + STREAM_HEIGHT);
            iStreamTop += STREAM_HEIGHT;
        }

        // audio stream
        if (m_bShowAudioStream)
        {
            m_rcAudioStreamRect.SetRect(rcClient.left + LEFT_BORDER, iStreamTop,
                rcClient.right, rcClient.bottom);
        }
    }
}

void CStreamCtrl::SetCursorClipRect()
{
    if (!m_pEditorDoc)
        return;

    int iClipStartMs  = m_arSelectedClips[0]->GetTimestamp();
    int iClipLengthMs = 0;
    for (int i = 0; i < m_arSelectedClips.GetSize(); ++i)
        iClipLengthMs += m_arSelectedClips[i]->GetLength();

    int iClipStartPos = GetPixelFromMsec(iClipStartMs, m_pEditorDoc->m_displayStartMs);
    int iClipWidth    = GetPixelFromMsec(iClipLengthMs);

    CRect clipRect;
    clipRect.top    = m_rcClipStreamRect.top;
    clipRect.bottom = m_rcClipStreamRect.bottom;
    clipRect.left   = m_rcClipStreamRect.left + iClipStartPos;
    clipRect.right  = clipRect.left + iClipWidth;

    if (clipRect.left < m_rcClipStreamRect.left)
        clipRect.left = m_rcClipStreamRect.left;
    if (clipRect.right > m_rcClipStreamRect.right)
        clipRect.right = m_rcClipStreamRect.right;

    CRect windowRect;
    GetWindowRect(windowRect);
    clipRect.OffsetRect(windowRect.left, windowRect.top);
    //ClipCursor(&clipRect);
}

CStopJumpMark *CStreamCtrl::FindMarkInRegion(UINT nFromPx, UINT nLength)
{	
    if (!m_pEditorDoc || m_pEditorDoc->project.IsEmpty())
        return NULL;

    CStopJumpMark *pMark = NULL;
    CMarkStream *pMarkStream = m_pEditorDoc->project.GetMarkStream();
    if (pMarkStream)
    {
        // Klickposition der Maus -2 Pixel und +2 Pixel in Millisekunden 
        UINT iMouseLeftMs = GetMsecFromPixel(nFromPx);
        UINT iMouseRightMs = GetMsecFromPixel(nFromPx + nLength);

        pMark = pMarkStream->GetFirstMarkInRange(iMouseLeftMs, iMouseRightMs - iMouseLeftMs);
    }

    return pMark;
}

bool CStreamCtrl::PointIsInMarksStream(CPoint &pt)
{
    if (IsMarksStreamShown())
        return (m_rcMarksStreamRect.PtInRect(pt) != 0);

    return false;
}

bool CStreamCtrl::PointIsInClipsStream(CPoint &pt)
{
    if (IsClipStreamShown())
        return (m_rcClipStreamRect.PtInRect(pt) != 0);

    return false;
}

bool CStreamCtrl::PointIsInVideoStream(CPoint &pt)
{
    if (IsVideoStreamShown())
        return (m_rcVideoStreamRect.PtInRect(pt) != 0);

    return false;
}

bool CStreamCtrl::PointIsInSlidesStream(CPoint &pt)
{
    if (IsSlideStreamShown())
        return (m_rcSlidesStreamRect.PtInRect(pt) != 0);

    return false;
}
bool CStreamCtrl::PointIsInQuestionObjects(CPoint &pt)
{
	if (IsSlideStreamShown()){
		CRect rcTmp(m_rcSlidesStreamRect);
		rcTmp.top += STREAM_HEIGHT + 4 * STREAM_HEIGHT / 3;
		rcTmp.bottom = rcTmp.top + 2 * STREAM_HEIGHT / 3;
        return (rcTmp.PtInRect(pt) != 0);
	}
    return false;
}
bool CStreamCtrl::PointIsInGraphicObjects(CPoint &pt)
{
	if (IsSlideStreamShown()){
		CRect rcTmp(m_rcSlidesStreamRect);
		rcTmp.top += STREAM_HEIGHT;
		rcTmp.bottom = rcTmp.top + 2 * STREAM_HEIGHT / 3;
        return (rcTmp.PtInRect(pt) != 0);
	}
    return false;
}
bool CStreamCtrl::PointIsInInteractionObjects(CPoint &pt)
{
	if (IsSlideStreamShown()){
		CRect rcTmp(m_rcSlidesStreamRect);
		rcTmp.top += STREAM_HEIGHT + 2 * STREAM_HEIGHT / 3;
		rcTmp.bottom = rcTmp.top + 2 * STREAM_HEIGHT / 3;
        return (rcTmp.PtInRect(pt) != 0);
	}
    return false;
}
bool CStreamCtrl::PointIsInAudioStream(CPoint &pt)
{
    if (IsAudioStreamShown())
        return (m_rcAudioStreamRect.PtInRect(pt) != 0);

    return false;
}

void CStreamCtrl::OnClipProperties()
{
    if (!m_pEditorDoc->project.IsDenverDocument())
    {
        // TODO m_csPopupClipFilename is too few information
        // there might be more than one segment or clip with the same (source) filename.
        CMainFrameE::GetCurrentInstance()->DisplayExportSettings(4, m_csPopupClipFilename);
    }
    else
        CMainFrameE::GetCurrentInstance()->DisplayExportSettings(3);
}

void CStreamCtrl::OnClipRemove()
{
    m_pEditorDoc->DeleteClipAt(m_nClipPopupPosMs);
}

void CStreamCtrl::OnUpdateClipRemove(CCmdUI *pCmdUI)
{
    bool bEnable = true;

    if (m_pEditorDoc != NULL && m_pEditorDoc->IsScreenGrabbingDocument())
        bEnable = false;

    // RemoveClip is not possible in LiveContext edit mode
    if (CStudioApp::IsLiveContextMode() && !CStudioApp::IsLiveContextPreview())
        bEnable = false;


    pCmdUI->Enable(bEnable);
}

void CStreamCtrl::OnVideoProperties() 
{
    CMainFrameE::GetCurrentInstance()->DisplayExportSettings(0);
}

void CStreamCtrl::OnVideoRemove()
{
    CMainFrameE::GetCurrentInstance()->DoRemoveVideo();
}

void CStreamCtrl::OnPageProperties() 
{
    m_pEditorDoc->project.ChangePageTitleAt(m_nPagePopupPosMs);
}

void CStreamCtrl::OnAudioProperties() 
{
    CMainFrameE::GetCurrentInstance()->DisplayExportSettings(1);	
}

void CStreamCtrl::OnMarkPage() 
{
    m_pEditorDoc->MarkPageAt(m_nPagePopupPosMs);
}

void CStreamCtrl::OnMarkClip()
{
    m_pEditorDoc->MarkClipAt(m_nClipPopupPosMs);
}

void CStreamCtrl::OnChangeClipTitle()
{
    m_pEditorDoc->project.ChangeClipTitleAt(m_nClipPopupPosMs);
}


BOOL CStreamCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    if (!m_pEditorDoc || m_pEditorDoc->project.IsEmpty() || m_pEditorDoc->IsPreviewActive() || nHitTest != HTCLIENT)
        return CWnd::OnSetCursor(pWnd, nHitTest, message);

    // position of cursor in screen coordinates
    CPoint ptPoint;
    BOOL bResult = ::GetCursorPos(&ptPoint);

    // position of mouse in window
    ScreenToClient(&ptPoint);

    // cursor in markss stream?
    bool bCursorIsOverMark = false;
    bool bCursorMovedOutOfMarksStream = false;

    // cursor moved out of marks stream while moving mark
    if (m_bMouseMoveOutOfMarksStream && m_bMoveMark)
        bCursorMovedOutOfMarksStream = true;

    // cursor is over mark stream?
    if (!bCursorMovedOutOfMarksStream && PointIsInMarksStream(ptPoint))
    {
        if (m_bMoveMark)
        {
            bCursorIsOverMark = true;
        }
        else
        {
            CMarkStream *pMarkStream = m_pEditorDoc->project.GetMarkStream();
            if (pMarkStream)
            {
                UINT nMousePosPx = ptPoint.x - LEFT_BORDER;
                CStopJumpMark *pMark = FindMarkInRegion(nMousePosPx - SELECTION_OFFSET, 2*SELECTION_OFFSET);

                if (pMark)
                    bCursorIsOverMark = true;
            }
        }
    }

	//cursor in timeline line selected object?
	bool bCursorIsOverSelObj = false;
	bool bCursorIsOverSelObjMargin = false;
	bool bCursorIsOverDisplayedObj = false;
	if(PointIsInSlidesStream(ptPoint) && m_pEditorDoc->GetWhiteboardView())
	{
		CRect rcTmp;
		CInteractionAreaEx* pInteraction;
		//set cursor over unselected object
		bool bGraphic = PointIsInGraphicObjects(ptPoint);
		bool bInter = PointIsInInteractionObjects(ptPoint);
		bool bQuest = PointIsInQuestionObjects(ptPoint);
		CArray<CInteractionAreaEx*, CInteractionAreaEx*>aObjects;
		HRESULT hr;
		CTimePeriod tp;tp.Init(INTERACTION_PERIOD_TIME,GetMsecFromPixel(ptPoint.x -4 -LEFT_BORDER,true),GetMsecFromPixel(ptPoint.x +5 -LEFT_BORDER,true));
		if (m_pEditorDoc->project.GetInteractionStream()){
			hr = m_pEditorDoc->project.GetInteractionStream(false)->FindInteractions(&tp,false, &aObjects);
			if (SUCCEEDED(hr)){
				for (int i=0;i<aObjects.GetCount();i++){
					if (aObjects.GetAt(i)->IsTimelineDisplay())
						if ((bGraphic && aObjects.GetAt(i)->GetTypeId() == INTERACTION_RECORD_GRAPHIC_OBJECTS)||
							(bInter && (aObjects.GetAt(i)->GetTypeId() == INTERACTION_RECORD_ELEMENTS  || (aObjects.GetAt(i)->GetTypeId()==INTERACTION_RECORD_GRAPHIC_OBJECTS && aObjects.GetAt(i)->IsDemoDocumentObject())))||
							(bQuest && aObjects.GetAt(i)->GetTypeId() != INTERACTION_RECORD_GRAPHIC_OBJECTS && aObjects.GetAt(i)->GetTypeId() != INTERACTION_RECORD_ELEMENTS))
						{
							bCursorIsOverDisplayedObj = true;
							break;
						}
				}
			}
		}
		//set cursor over selected object
		for(int i=0;i<m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetCount();i++)
		{
			pInteraction = m_pEditorDoc->GetWhiteboardView()->GetSelectedObjects().GetAt(i);
			if(pInteraction)
			{
				rcTmp.CopyRect(pInteraction->GetTimelineRect());
				if (rcTmp.Width() < MIN_TIMELINE_WIDTH)
				{
					rcTmp.left = rcTmp.left - (MIN_TIMELINE_WIDTH - rcTmp.Width()) / 2;
					rcTmp.right = pInteraction->GetTimelineRect().right + (MIN_TIMELINE_WIDTH - pInteraction->GetTimelineRect().Width()) / 2 + 1;
				}
				if(rcTmp.PtInRect(ptPoint))
				{
					rcTmp.CopyRect(pInteraction->GetTimelineRect());
					if (rcTmp.Width() < MIN_TIMELINE_WIDTH)
						rcTmp.left = pInteraction->GetTimelineRect().left - (MIN_TIMELINE_WIDTH - pInteraction->GetTimelineRect().Width()) / 2;
					rcTmp.right = rcTmp.left + 3;
					if(rcTmp.PtInRect(ptPoint))
						bCursorIsOverSelObjMargin = true;
					else
					{
						rcTmp.CopyRect(pInteraction->GetTimelineRect());
						if (rcTmp.Width() < MIN_TIMELINE_WIDTH)
							rcTmp.right = pInteraction->GetTimelineRect().right + (MIN_TIMELINE_WIDTH - pInteraction->GetTimelineRect().Width()) / 2 + 1;
						rcTmp.left = rcTmp.right-3;
						if(rcTmp.PtInRect(ptPoint))
							bCursorIsOverSelObjMargin = true;
						else
							bCursorIsOverSelObj = true;
					}
				}
			}
		}
		if(bCursorIsOverSelObjMargin)
			bCursorIsOverDisplayedObj = false;
	}

    // cursor in clips stream?
    bool bCursorIsOverClip = false;

    // cursor is over clip
    if (!bCursorMovedOutOfMarksStream && !bCursorIsOverMark && PointIsInClipsStream(ptPoint))
    {
        for (int i = 0; i < m_arClipStreamRect.GetSize(); ++i)
        {
            if (m_arClipStreamRect[i].PtInRect(ptPoint))
            {
                bCursorIsOverClip = true;
                break;
            }
        }
    }

    if (bCursorMovedOutOfMarksStream)
        ::SetCursor(m_hDeleteCursor);
	else if (bCursorIsOverMark || bCursorIsOverClip || bCursorIsOverSelObj || bCursorIsOverDisplayedObj)
        ::SetCursor(m_hMoveCursor);
	else if (bCursorIsOverSelObjMargin)
		::SetCursor(m_hResizeHCursor);
    else
        ::SetCursor(m_hSelectCursor);

    return TRUE;
}

void CStreamCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    // left mouse is pressed
    if (m_iButtonStatus == BUTTON_DOWN)
        return;

    if (m_pEditorDoc->IsPreviewActive())
        return;

    // position of mouse in window
    CPoint ptPoint = point;
    ScreenToClient(&ptPoint);


    // cursor pos in milliseconds
    int msPos = GetMsecFromPixel(ptPoint.x - LEFT_BORDER + 1);
    // TODO remove +1 due to round down scale up problem with positions and pixels
    m_nContextmenuPixelPos = ptPoint.x - LEFT_BORDER; 
    m_nContextmenuTimestamp = msPos;
    if (PointIsInMarksStream(ptPoint))
    {
        m_pContextMenuMark = NULL;

        UINT nMousePosPx = ptPoint.x - LEFT_BORDER;
        CStopJumpMark *pMark = FindMarkInRegion(nMousePosPx - SELECTION_OFFSET, 2 * SELECTION_OFFSET);

        // special context menu or common context menu
        if (pMark)
        {
            CMenu *pPopup = NULL;
            m_pContextMenuMark = pMark;
            bool bIsDemoStopmark = false;
            if (pMark->IsStopMark()){
                /*CMenu menu;
                menu.LoadMenu(IDR_STOPMARK_CONTEXT); */
                pPopup = m_stopmarkPopup.GetSubMenu(0);//menu.GetSubMenu(0);//
                bIsDemoStopmark = pMark->IsDemoDocumentObject();
                CString csDeleteText;
                if (!bIsDemoStopmark){
                    csDeleteText.LoadString(IDS_NORMAL_STOPMARK_DELETE);
                } else {
                    csDeleteText.LoadString(IDS_DEMO_STOPMARK_DELETE);
                }
                pPopup->ModifyMenu(IDC_REMOVE_STOPMARK, 0, IDC_REMOVE_STOPMARK, csDeleteText);
                pPopup->EnableMenuItem(IDC_REMOVE_ALL_DEMO_OBJECTS, 
                    m_pEditorDoc->ShowAsScreenSimulation() && m_pEditorDoc->IsDemoDocument() ? MF_ENABLED : MF_GRAYED);
                pPopup->EnableMenuItem(ID_INSERT_DEMO_OBJECT, 
                    m_pEditorDoc->ShowAsScreenSimulation() && m_pEditorDoc->IsDemoDocument() && !bIsDemoStopmark ? MF_ENABLED : MF_GRAYED);
            }
            else
                pPopup = m_targetmarkPopup.GetSubMenu(0);
            if (pPopup){
                

                
                pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
            }
        }
        else
        {
            CMenu *pPopup = m_marksPopup.GetSubMenu(0);
            if (pPopup){
                pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
            }
        }
    }
    else if (PointIsInVideoStream(ptPoint))
    { 
        CPreviewSegment *pSegment = m_pEditorDoc->project.FindPreviewSegment(msPos);
        if (pSegment && pSegment->GetVideoStream())
        {
            CVideoStream *pVideoStream = pSegment->GetVideoStream();

            int nGlobalOffset = m_pEditorDoc->project.m_videoExportGlobalOffset;
            int nOffsetDiff = nGlobalOffset + pVideoStream->GetOffset() - pSegment->GetSourceBegin();
            if (nOffsetDiff < 0)
                nOffsetDiff = 0;
            int lengthDiff = pSegment->GetSourceEnd() - (nGlobalOffset + pVideoStream->GetOffset() + pVideoStream->GetLength());
            if (lengthDiff < 0)
                lengthDiff = 0;

            int nVideoBegin = pSegment->GetTargetBegin() + nOffsetDiff;
            int nVideoEnd = pSegment->GetTargetEnd() - lengthDiff;

            if (msPos >= nVideoBegin && msPos < nVideoEnd)
            {
                CMenu *pPopup = m_videoPopup.GetSubMenu(0);
                if (pPopup)
                    pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
            }
        }
    }
    else if (PointIsInClipsStream(ptPoint))
    {
        bool bFound = false;
        for (int i = 0; i < m_arClipStreamRect.GetSize(); ++i)
        {
            // this DOES consider zoom as the "m_arClipStreamRect"s always get
            // updated during repaint
            if (m_arClipStreamRect[i].PtInRect(ptPoint)) 
            { 	
                m_nClipPopupPosMs = msPos;

                m_csPopupClipFilename = "";
                CClipStreamR *pClipStream = m_pEditorDoc->project.GetClipStream();
                if (pClipStream != NULL)
                    m_csPopupClipFilename = pClipStream->GetFilenameAt(m_nClipPopupPosMs);

                CMenu *pPopup = m_clipPopup.GetSubMenu(0);
                if (pPopup) {
                    if (m_pEditorDoc != NULL && m_pEditorDoc->IsScreenGrabbingDocument()) 
                        pPopup->EnableMenuItem(ID_CLIP_REMOVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
                    else
                        pPopup->EnableMenuItem(ID_CLIP_REMOVE, MF_BYCOMMAND | MF_ENABLED);
                    pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
                }

                bFound = true;
            }
        }

        if (!bFound)
        {
            CMenu *pPopup = m_marksPopup.GetSubMenu(0);
            if (pPopup)
                pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
        }
    }
    else if (PointIsInSlidesStream(ptPoint))
    {
        if (m_pEditorDoc->project.IsDenverDocument()) {
            CMenu *pPopup = m_createStructPopup.GetSubMenu(0);
            if (pPopup)
                pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
        }
        else {
            CPage *pPage = m_pEditorDoc->project.FindPageAt(msPos);
            if (pPage)
            {
                m_nPagePopupPosMs = msPos;
                CMenu *pPopup = m_pagePopup.GetSubMenu(0);
                if (pPopup)
                    pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
            }
        }
    }
    else if (PointIsInAudioStream(ptPoint))
    {
        CMenu *pPopup = m_audioPopup.GetSubMenu(0);
        if (pPopup)
            pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
    }
    else
    {
#ifdef _DEBUG

        MessageBox(_T("No stream at this position"));
#endif
    }

}

void CStreamCtrl::OnInsertStopmark() 
{
    if (!m_pEditorDoc || m_pEditorDoc->project.IsEmpty())
        return;

    int iCheckID = IDC_CURRENT_TIMESTAMP;
    InsertStopmark(iCheckID, m_nContextmenuTimestamp);

    ((CStreamView *)GetParent())->UpdateStreamVisibilities(m_pEditorDoc);
}

void CStreamCtrl::OnRemoveMark() 
{
    if (!m_pEditorDoc || m_pEditorDoc->project.IsEmpty())
        return;

    CMarkStream *pMarkStream = m_pEditorDoc->project.GetMarkStream();
    if (pMarkStream)
    {	
        UINT nMouseLeftMs = GetMsecFromPixel(m_nContextmenuPixelPos - SELECTION_OFFSET);
        UINT nMouseRightMs = GetMsecFromPixel(m_nContextmenuPixelPos + 2 * SELECTION_OFFSET);

        HRESULT hr = pMarkStream->SelectMark(nMouseLeftMs, nMouseRightMs-nMouseLeftMs);

        int iResult = IDNO;
        bool bIsDemoObject = pMarkStream->SelectedStopMarkIsDemoDocumentObject();
        int iFrom, iTo;
                
        if (pMarkStream->SelectedMarkIsUsed())
        {
            CString csCaption;
            csCaption.LoadString(IDS_QUESTION);
            CString csMessage;
            if (bIsDemoObject) {
                pMarkStream->GetMovedMarkTimes(iFrom, iTo);
                //csMessage = _T("This mark is related to a demo object which will also be deleted.\n Do you really want to delete them?  ");
                      // TODO soare: add DE resource string
                csMessage.LoadString(IDS_DELETE_DEMO_MARK);
            } else {
                csMessage.LoadString(IDS_DELETE_USED_MARK);
            }
				iResult = MessageBox(csMessage, csCaption, MB_YESNO | MB_TOPMOST); 
        }

        hr = pMarkStream->ResetMoveSelected();
        if (iResult == IDYES) {
            if(bIsDemoObject) {
                hr = m_pEditorDoc->project.GetInteractionStream()->DeleteRelatedDemoObject(iFrom);
            } else {
                pMarkStream->RemoveMark(nMouseLeftMs, nMouseRightMs - nMouseLeftMs);
            }
        }

        ((CStreamView *)GetParent())->UpdateStreamVisibilities(m_pEditorDoc);
        ((CStreamView *)GetParent())->RedrawStreamCtrl(NULL);
        if(bIsDemoObject)
            //m_pEditorDoc->GetWhiteboardView()->RedrawWindow();
			m_pEditorDoc->UpdateAllViews(NULL,HINT_CHANGE_PAGE);
        //RedrawWindow(&m_rcMarksStreamRect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void CStreamCtrl::OnRemoveAllDemoDocumentObjects(){
    if (!m_pEditorDoc || m_pEditorDoc->project.IsEmpty() || !m_pEditorDoc->GetWhiteboardView())
        return;
    m_pEditorDoc->GetWhiteboardView()->DeleteAllDemoDocumentObjects();
}

void CStreamCtrl::OnInsertDemoObject() {
    if (!m_pEditorDoc || m_pEditorDoc->project.IsEmpty() || !m_pEditorDoc->GetWhiteboardView())
        return;
    UINT nMousePosPx = m_nContextmenuPixelPos;//ptPoint.x - LEFT_BORDER;
    CStopJumpMark *pMark = FindMarkInRegion(nMousePosPx - SELECTION_OFFSET, 2 * SELECTION_OFFSET);
    int iTimestamp = -1;
    if (pMark) {
        iTimestamp = pMark->GetPosition();
        pMark->SetDemoDocumentObject(true);
        //pMark->
        CMarkStream* pMarkStream = m_pEditorDoc->project.GetMarkStream();
        if (pMarkStream)
            pMarkStream->RemoveMark(iTimestamp, 1, false);
        else 
            iTimestamp = -1;
        bool bInsert = m_pEditorDoc->GetWhiteboardView()->InsertDemoObject(iTimestamp);
        
        m_pEditorDoc->JumpPreview(iTimestamp);

    }
   /* bool bInsert = m_pEditorDoc->GetWhiteboardView()->InsertDemoObject(m_nContextmenuTimestamp);
    if (bInsert) {
        CMarkStream* pMarkStream = m_pEditorDoc->project.GetMarkStream();
        if (pMarkStream != NULL) {
            CStopJumpMark* pStopMark = pMarkStream->GetFirstStopMarkInRange(m_nContextmenuTimestamp, 1);
            if (pStopMark != NULL) {
                pStopMark->SetDemoDocumentObject(true);
                m_pEditorDoc->JumpPreview(m_nContextmenuTimestamp);
            }
        }
    }*/
}

void CStreamCtrl::OnInsertTargetmark() 
{
    if (!m_pEditorDoc || m_pEditorDoc->project.IsEmpty())
        return;

    int iCheckID = IDC_CURRENT_TIMESTAMP;

    UINT nNextTargetCount = 1;

    CMarkStream *pMarkStream = m_pEditorDoc->project.GetMarkStream();
    if (pMarkStream)
        nNextTargetCount = pMarkStream->GetJumpMarkCounter() + 1;

    CString csTargetmarkName;
    csTargetmarkName.Format(IDS_TARGET, nNextTargetCount);

    InsertTargetmark(csTargetmarkName, iCheckID, m_nContextmenuTimestamp);

    ((CStreamView *)GetParent())->UpdateStreamVisibilities(m_pEditorDoc);
}

void CStreamCtrl::OnTargetmarkProperties() 
{
    if (!m_pContextMenuMark)
        return;

    int iTimestamp = m_pContextMenuMark->GetPosition();
    int iCheckID = IDC_CURRENT_TIMESTAMP;
    CString csTargetmarkName = m_pContextMenuMark->GetLabel();

    CInsertTargetmarkDialog::ShowDialog(this, iCheckID, csTargetmarkName, true);


    if (iCheckID != IDCANCEL)
    {
        m_pContextMenuMark->SetLabel(csTargetmarkName);
        //RedrawWindow(&m_rcMarksStreamRect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

        ((CStreamView *)GetParent())->RedrawStreamCtrl(NULL);
    }

    m_pContextMenuMark = NULL;
}

HRESULT CStreamCtrl::InsertStopmark(int iCheckID, int iTimestamp)
{
    HRESULT hr = S_OK;

    if (!m_pEditorDoc || m_pEditorDoc->project.IsEmpty())
        hr = E_FAIL;

    // Einfüge-Dialog für Stoppmarken anzeigen
    if (SUCCEEDED(hr))
    {
        CInsertStopmarkDialog::ShowDialog(this, iCheckID);

        if (iCheckID != IDCANCEL)
        {
            // Markierungsstrom erfragen und falls er nicht existiert, erzeugen
            CMarkStream* pMarkStream = m_pEditorDoc->project.GetMarkStream(true);
            if (!pMarkStream)	// TODO: Fehlermeldung
                hr = E_FAIL;

            // Stoppmarke erzeugen und einfügen.
            if (SUCCEEDED(hr))
            {
                int iInsertionTimestamp = -1;
                if (iCheckID != IDC_ALL_SLIDES)
                {
                    if (iCheckID == IDC_CURRENT_SLIDE_END)
                    {
                        UINT nStartMs, nEndMs;
                        m_pEditorDoc->project.GetPagePeriod(iTimestamp, nStartMs, nEndMs);
                        iInsertionTimestamp = nEndMs;
                    }
                    else if (iCheckID = IDC_CURRENT_TIMESTAMP)
                        iInsertionTimestamp = iTimestamp;

                    if (iInsertionTimestamp >= 0)
                    {
                        CStopJumpMark *pMark = new CStopJumpMark();
                        pMark->Init(false, NULL, iInsertionTimestamp);
                        pMarkStream->IncludeMark(pMark);
                        delete pMark;
                    }
                }
                else
                {
                    CArray<CPage *, CPage *> caAllSlides;
                    m_pEditorDoc->project.GetPages(caAllSlides, 0, m_pEditorDoc->m_docLengthMs);

                    for (int i = 0; i < caAllSlides.GetSize(); ++i)
                    {
                        int iInsertionTimestamp = caAllSlides[i]->GetEnd()-1;
                        if (i == caAllSlides.GetSize()-1)
                            iInsertionTimestamp = m_pEditorDoc->m_docLengthMs-1;
                        CStopJumpMark *pMark = new CStopJumpMark();
                        pMark->Init(false, NULL, iInsertionTimestamp);
                        pMarkStream->IncludeMark(pMark);
                        delete pMark;
                    }
                }
            }

            if (SUCCEEDED(hr) && IsMarksStreamShown())
            {
                ((CStreamView *)GetParent())->RedrawStreamCtrl(NULL);
                //RedrawWindow(&m_rcMarksStreamRect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
            }

            m_pEditorDoc->SetModifiedFlag();
        }
    }	

    return hr;
}

HRESULT CStreamCtrl::InsertTargetmark(CString &csName, int iCheckID, int iTimestamp)
{
    HRESULT hr = S_OK;

    if (!m_pEditorDoc || m_pEditorDoc->project.IsEmpty())
        hr = E_FAIL;

    // Einfüge-Dialog für Stoppmarken anzeigen
    if (SUCCEEDED(hr))
    {	
        CInsertTargetmarkDialog::ShowDialog(this, iCheckID, csName);

        if (iCheckID != IDCANCEL)
        {
            // Markierungsstrom erfragen und falls er nicht existiert, erzeugen
            CMarkStream *pMarkStream = m_pEditorDoc->project.GetMarkStream(true);
            if (!pMarkStream)	// TODO: Fehlermeldung
                hr = E_FAIL;

            if (SUCCEEDED(hr))
            {
                // Zielmarke erzeugen und einfügen.
                int iInsertionTimestamp = -1;
                if (iCheckID == IDC_CURRENT_SLIDE_BEGIN)
                {
                    UINT nStartMs, nEndMs;
                    m_pEditorDoc->project.GetPagePeriod(iTimestamp, nStartMs, nEndMs);
                    iInsertionTimestamp = nStartMs;
                }
                else if (iCheckID = IDC_CURRENT_TIMESTAMP)
                    iInsertionTimestamp = iTimestamp;

                if (iInsertionTimestamp >= 0)
                {
                    CStopJumpMark *pMark = new CStopJumpMark();
                    pMark->Init(true, csName, iInsertionTimestamp);
                    hr = pMarkStream->IncludeMark(pMark);
                    delete pMark;
                    if (hr == E_MS_MARK_EXISTS)
                    {
                        CStopJumpMark *pMark = pMarkStream->GetFirstMarkInRange(iInsertionTimestamp, 1);
                        CString csErrorMessage;
                        csErrorMessage.Format(IDS_MSG_TARGETMARK_EXISTS, pMark->GetLabel());
                        CString csCaption;
                        csCaption.LoadString(IDS_WARNING_E);
                        ::MessageBox(this->GetSafeHwnd(), csErrorMessage, csCaption, MB_OK|MB_ICONWARNING);
                    }
                }

                if (SUCCEEDED(hr) && IsMarksStreamShown())
                {
                    ((CStreamView *)GetParent())->RedrawStreamCtrl(NULL);
                    //RedrawWindow(&m_rcMarksStreamRect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
                }

                m_pEditorDoc->SetModifiedFlag();
            }
        }
    }

    return hr;
}



BOOL CStreamCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
    WORD nCmdID = LOWORD(wParam);

    // These commands are part of the CMainFrameE class
    if (nCmdID == ID_CREATESTRUCTURE)
    {
        CMainFrameE *pMainWnd = CMainFrameE::GetCurrentInstance();
        if (pMainWnd != NULL && pMainWnd->OnCommand(wParam, lParam))
        {
            return TRUE; 
        }
    } 

    return CWnd::OnCommand(wParam, lParam);
}

bool CStreamCtrl::LoadIconForStream(UINT nID)
{
    bool bRetValue=false;
    // icon size
    CSize sz(16, 16);
    // load icon
    if(nID == IDR_TIMELINE_ICON_VIDEO)
    {
        m_iconHandle = m_pIconVideo->LoadBitmapFromResource(MAKEINTRESOURCE(nID), NULL);
        if(m_iconHandle!=NULL)
        {
            m_pIconVideo = new CXTPImageManagerIcon(0, m_iconHandle.GetExtent().cx, m_iconHandle.GetExtent().cy);
            m_pIconVideo->SetNormalIcon(m_iconHandle);
            if(m_pIconVideo!=NULL)
            {
                bRetValue=true;
            }
        }
    }
    else if(nID == IDR_TIMELINE_ICON_SLIDES)
    {
        m_iconHandle = m_pIconSlides->LoadBitmapFromResource(MAKEINTRESOURCE(nID), NULL);
        if(m_iconHandle!=NULL)
        {
            m_pIconSlides = new CXTPImageManagerIcon(0, m_iconHandle.GetExtent().cx, m_iconHandle.GetExtent().cy);
            m_pIconSlides->SetNormalIcon(m_iconHandle);
            if(m_pIconSlides!=NULL)
            {
                bRetValue=true;
            }
        }
    } 
    else if(nID == IDR_TIMELINE_VIDEO_SCREEN)
    {
        m_iconHandle = m_pIconCaption->LoadBitmapFromResource(MAKEINTRESOURCE(nID), NULL);
        if(m_iconHandle!=NULL)
        {     
            m_pIconCaption = new CXTPImageManagerIcon(0, m_iconHandle.GetExtent().cx, m_iconHandle.GetExtent().cy);
            m_pIconCaption->SetNormalIcon(m_iconHandle);
            if(m_pIconSlides!=NULL)
            {
                bRetValue=true;
            }
        }
    }
    return bRetValue;
}

bool CStreamCtrl::LoadBackgroundImages()
{
    bool bRetValue=false;
    //load background images
    m_pImageBackgroundVideo         = XTPOffice2007Images()->LoadFile(_T("STREAMVIDEO"));
    m_pImageBackgroundClip          = XTPOffice2007Images()->LoadFile(_T("STREAMCLIPS"));
    m_pImageBackgroundSlides        = XTPOffice2007Images()->LoadFile(_T("STREAMSLIDES"));
    m_pImageBackgroundAudio         = XTPOffice2007Images()->LoadFile(_T("STREAMAUDIO"));
    m_pImageBackgroundCaptionStream = XTPOffice2007Images()->LoadFile(_T("BACKGROUNDCAPTION"));
    m_pImageBackgroundStream        = XTPOffice2007Images()->LoadFile(_T("BACKGROUNDSTREAM"));
    if((m_pImageBackgroundVideo!=NULL)&&
        (m_pImageBackgroundClip!=NULL)&&
        (m_pImageBackgroundSlides!=NULL)&&
        (m_pImageBackgroundAudio!=NULL)&&
        (m_pImageBackgroundCaptionStream!=NULL)&&
        (m_pImageBackgroundStream!=NULL))
    {
        bRetValue = true;
    }
    return bRetValue;
}