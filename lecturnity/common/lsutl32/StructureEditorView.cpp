// StructureEditorView.cpp: Implementierungsdatei
//
#include "stdafx.h"
#include "StructureEditorView.h"
#include "Resource.h"

#ifndef SAFE_DELETE_WINDOW
#define SAFE_DELETE_WINDOW(ptr) if(ptr){ptr->DestroyWindow(); delete ptr; ptr=NULL;}
#endif

static void TranslateMsecToTimeString(int msec, CString &timeString) {
    _TCHAR timebuf[128];

    int totSec = msec / 1000;
    int min  = totSec / 60;
    int sec  = totSec % 60;

    _stprintf(timebuf, _T("%02d:%02d"), min, sec);  

    timeString = timebuf;
}

// CPageEntry
CPageEntry::CPageEntry() {
    
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    m_pTitleField = NULL;
    m_pTimeField = NULL;
    m_pRemoveButton = NULL;
    m_pThumbnailImage = NULL;
    m_pClickIcon = Gdiplus::Bitmap::FromResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_CLICK_ICON));

    m_bRemove = false;
    m_bIsClickPage = false;
    m_iBegin = 0;
    m_bPageIsHidden = false;
    m_iPageNumber = 0;

    m_pFont = NULL;

    m_szTimeField.SetSize(50, 18);
    m_szRemoveButton.SetSize(18, 18);
    m_szTitleField.SetSize(200, 18);
    m_szClickIcon.SetSize(18, 18);
}

CPageEntry::~CPageEntry() {
    SAFE_DELETE_WINDOW(m_pTitleField);
    SAFE_DELETE_WINDOW(m_pRemoveButton);
    SAFE_DELETE_WINDOW(m_pTimeField);
    if (m_pThumbnailImage)
        delete m_pThumbnailImage;
    if (m_pClickIcon)
        delete m_pClickIcon;
}

void CPageEntry::SetTitle(CString &csTitle) {
    m_csTitle = csTitle;

    if (m_pTitleField)
        m_pTitleField->SetWindowText(csTitle);
}

CString &CPageEntry::GetTitle() {
    if (m_pTitleField)
        m_pTitleField->GetWindowText(m_csTitle);

    return m_csTitle;
}

void CPageEntry::SetThumbImage(CString &csThumbFilename) {
    m_csThumbFilename = csThumbFilename;

    if (m_pThumbnailImage != NULL)
        delete m_pThumbnailImage;

    /*
    int iStringLength = m_csThumbFilename.GetLength() + 1;
    WCHAR *wcFilename = (WCHAR *)malloc(iStringLength*sizeof(WCHAR));
#ifdef _UNICODE
    wcscpy(wcFilename, m_csThumbFilename);
#else
    MultiByteToWideChar( CP_ACP, 0, m_csThumbFilename, iStringLength, 
                        wcFilename, iStringLength);
#endif

    Gdiplus::Image *tmpImage = Gdiplus::Image::FromFile(wcFilename, FALSE);
    */

    m_pThumbnailImage = Gdiplus::Image::FromFile(csThumbFilename);
    m_szThumbnail.cx = m_pThumbnailImage->GetWidth();
    m_szThumbnail.cy = m_pThumbnailImage->GetHeight();
}

void CPageEntry::ReleaseThumbImage() {
   if ( m_pThumbnailImage )
      delete m_pThumbnailImage;
   m_pThumbnailImage = NULL;
}

void CPageEntry::Hide() {
    m_bPageIsHidden = true;

    if (m_pTitleField != NULL) 
        m_pTitleField->ShowWindow(SW_HIDE);
    if (m_pTimeField != NULL)
        m_pTimeField->ShowWindow(SW_HIDE);
    if (m_pRemoveButton != NULL)
        m_pRemoveButton->ShowWindow(SW_HIDE);
}

void CPageEntry::SetPosition(CPoint ptBegin) {
    // set positions
    m_bPageIsHidden = false;

    int xStart = ptBegin.x + BORDER_SPACE;
    if (m_pRemoveButton != NULL)
        m_pRemoveButton->SetWindowPos(NULL, xStart, ptBegin.y+BORDER_SPACE, 0, 0, 
                                      SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

    xStart += m_szRemoveButton.cx + BETWEEN_SPACE_X;
    if (m_pTitleField != NULL) 
        m_pTitleField->SetWindowPos(NULL, xStart, ptBegin.y+BORDER_SPACE, 0, 0, 
                                    SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

    xStart += m_szTitleField.cx + BETWEEN_SPACE_X;
    if (m_pTimeField != NULL) 
       m_pTimeField->SetWindowPos(NULL, xStart, ptBegin.y+BORDER_SPACE, 0, 0, 
                                  SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
}

int CPageEntry::GetHeight() {
    int iControlHeight;
    iControlHeight = m_szRemoveButton.cy;
    if (m_szTitleField.cy > iControlHeight)
        iControlHeight = m_szTitleField.cy;
    if (m_szTimeField.cy > iControlHeight)
        iControlHeight = m_szTimeField.cy;

    iControlHeight += BETWEEN_SPACE_Y;
    iControlHeight += m_szThumbnail.cy;
    iControlHeight += 2*BORDER_SPACE;

    return iControlHeight;
}

int CPageEntry::GetWidth() {
    int iControlWidth;

    iControlWidth = m_szRemoveButton.cx + m_szTitleField.cx + m_szTimeField.cx;
    iControlWidth += 2 * BETWEEN_SPACE_X + 2*BORDER_SPACE;

    return iControlWidth;
}

void CPageEntry::CreateTimeField(CWnd *pParent) {     
    if (m_pTimeField != NULL)
        m_pTimeField->DestroyWindow();
    delete m_pTimeField;

    m_pTimeField  = new CStatic();
    CString csTime;
    TranslateMsecToTimeString(m_iBegin, csTime);
    m_pTimeField->Create(csTime, WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,
                       CRect(CPoint(0,0), m_szTimeField), pParent, (UINT)-1);
    m_pTimeField->SetFont(m_pFont);
}

void CPageEntry::CreateTitleField(CWnd *pParent) {  
    if (m_pTitleField != NULL)
        m_pTitleField->DestroyWindow();
    delete m_pTitleField;

    // create title field for page
    m_pTitleField = new CEdit();
    m_pTitleField->Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER,
                          CRect(CPoint(0,0), m_szTitleField), pParent, (UINT)-1);
    m_pTitleField->SetFont(m_pFont);
    m_pTitleField->SetWindowTextW(m_csTitle);
}

void CPageEntry::CreateRemoveButton(CWnd *pParent) {
    if (m_pRemoveButton != NULL)
        m_pRemoveButton->DestroyWindow();
    delete m_pRemoveButton;

    m_pRemoveButton= new CBitmapButton();
    m_pRemoveButton->Create(NULL, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                            CRect(CPoint(0,0), m_szRemoveButton), pParent, 
                            IDC_PAGE_REMOVE_BUTTON_ARRAY + m_iPageNumber);
    m_pRemoveButton->SetFont(m_pFont);
    m_pRemoveButton->LoadBitmaps(IDB_DELETE_BUTTON, IDB_DELETE_BUTTON_SEL, 0, IDB_DELETE_BUTTON_DISABLED);
}

void CPageEntry::Draw(CDC *pDC, CPoint &ptStart) {
    Gdiplus::Graphics graphics(pDC->GetSafeHdc());

    Gdiplus::Rect rcBackground;
    rcBackground.X = ptStart.x;
    rcBackground.Y = ptStart.y;
    rcBackground.Width = GetWidth();
    rcBackground.Height = GetHeight();
    rcBackground.Inflate(-2, -2);
    Gdiplus::SolidBrush bgBrush(Gdiplus::Color(255, 255, 255));
    if (m_bIsClickPage)
        bgBrush.SetColor(Gdiplus::Color(225, 225, 225));

    graphics.FillRectangle(&bgBrush, rcBackground);

    Gdiplus::Rect rcImage;
    // the click pages should be right aligned to the time control
    if (m_bIsClickPage)
        rcImage.X = (ptStart.x + BORDER_SPACE + 
                    m_szRemoveButton.cx + BETWEEN_SPACE_X + 
                    m_szTitleField.cx + BETWEEN_SPACE_X + 
                    m_szTimeField.cx) - m_szThumbnail.cx;
    else
        rcImage.X = ptStart.x + BORDER_SPACE + m_szRemoveButton.cx + BETWEEN_SPACE_X;
    rcImage.Y = ptStart.y + BORDER_SPACE + m_szRemoveButton.cy + BETWEEN_SPACE_Y;
    rcImage.Width = m_szThumbnail.cx;
    rcImage.Height = m_szThumbnail.cy;
    if (m_pThumbnailImage)
        graphics.DrawImage(m_pThumbnailImage, rcImage);

    if (m_pClickIcon != NULL && m_bIsClickPage) {
        Gdiplus::Rect rcIcon;
        rcIcon.X = ptStart.x + BORDER_SPACE;
        rcIcon.Y = ptStart.y + BORDER_SPACE + m_szRemoveButton.cy + BETWEEN_SPACE_Y + m_szThumbnail.cy - 16;
        rcIcon.Width = 16;
        rcIcon.Height = 16;
       
        graphics.DrawImage(m_pClickIcon, rcIcon);
    }
}

// CStructureEditorView
IMPLEMENT_DYNCREATE(CStructureEditorView, CScrollView)

BEGIN_MESSAGE_MAP(CStructureEditorView, CScrollView)
   ON_CONTROL_RANGE(BN_CLICKED,IDC_PAGE_REMOVE_BUTTON_ARRAY,IDC_PAGE_REMOVE_BUTTON_ARRAY + 1000,OnDeleteButton)
   ON_WM_CREATE()
   ON_WM_DESTROY()
END_MESSAGE_MAP()


CStructureEditorView::CStructureEditorView() {
    m_pListener = NULL;
    m_ptTopLeft.x = 10;
    m_ptTopLeft.y = 5;
}

CStructureEditorView::~CStructureEditorView() {
    // the entries are deleted in OnDestrox
    m_aPageEntries.RemoveAll();
}

void CStructureEditorView::OnInitialUpdate() {
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: Gesamte Größe dieser Ansicht berechnen
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CStructureEditorView::GetPageEntries(CArray<CPageEntry *, CPageEntry *> &aPageEntries) {
    aPageEntries.RemoveAll();

    aPageEntries.Copy(m_aPageEntries);
}

void CStructureEditorView::UpdatePageList(bool bHideUserEventPages) {
    // init (respecting current scroll position)

    CString csPage;
    csPage.LoadString(IDC_PAGE);
    csPage += " ";

    int pageNr = 0;

    CPoint ptStart(m_ptTopLeft.x, (m_ptTopLeft.y - GetDeviceScrollPosition().y));
    int iMaxPageHeight = m_ptTopLeft.y;
    int iMaxPageWidth = 0;
    for (int i = 0; i < m_aPageEntries.GetSize(); i++) {
        CPageEntry *pPageEntry = m_aPageEntries.GetAt(i);
        if ( pPageEntry->DoRemove() || (bHideUserEventPages && pPageEntry->IsClickPage()) ) {
            // hide pages that are marked for removal
            // hide user event pages if checkbox is deselected
            pPageEntry->Hide();
        } else {
            // show other page
            pPageEntry->SetPosition(ptStart);

            ptStart.y += pPageEntry->GetHeight();
            iMaxPageHeight += pPageEntry->GetHeight();
            if (pPageEntry->GetWidth() > iMaxPageWidth)
                iMaxPageWidth = pPageEntry->GetWidth();

            // dynamic page titles
            pageNr++;
            CString csTitle = pPageEntry->GetTitle();
            // check current format
            bool bDynamic = (csPage == csTitle.Left(csPage.GetLength()));
            // check for valid number
            int number = _wtoi( csTitle.Right(csTitle.GetLength()-csPage.GetLength()).GetBuffer() );
            bDynamic &= number > 0;
            // additional check because _wtoi cut off additional non-digits
            CString csTitleCheck;
            csTitleCheck.Format(_T("%ls%i"), csPage, number);
            bDynamic &= (csTitleCheck == csTitle);
            // set dynamic title if format matches
            if ( bDynamic ) {
                csTitle.Format(_T("%ls%i"), csPage, pageNr);
                pPageEntry->SetTitle(csTitle);
            }
        }
    }

    RedrawWindow();
    // size of scrollable area (respecting current scroll position)
    SIZE sizeTotal;
    sizeTotal.cx = iMaxPageWidth;
    sizeTotal.cy = iMaxPageHeight + 5;
    SetScrollSizes(MM_TEXT, sizeTotal);
}

void CStructureEditorView::SetFont(CFont* pFont) {
    m_pFont = pFont;
}

int CStructureEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CScrollView::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_ttipctrl.Create(this, TTS_ALWAYSTIP);

    if (m_pListener != NULL)
        m_pListener->FillPageList(m_aPageEntries);

    for (int i = 0; i < m_aPageEntries.GetCount(); i++) {    
        m_aPageEntries[i]->SetFont(m_pFont);
        m_aPageEntries[i]->CreateTimeField(this);

        m_aPageEntries[i]->CreateTitleField(this);
        CEdit *pTitleField = m_aPageEntries[i]->GetTitleFieldWnd();
        if (pTitleField) {
            m_ttipctrl.AddTool(pTitleField, IDS_STRUCTEDITOR_MODIFY_TITLE);

            // create "remove page" button
            m_aPageEntries[i]->CreateRemoveButton(this);
            CButton *pRemoveButton = m_aPageEntries[i]->GetRemoveButtonWnd();
            if (pRemoveButton) {
                if (i == 0) {
                    // first page cannot be removed
                    pRemoveButton->ModifyStyle(0, WS_DISABLED);
                    m_ttipctrl.AddTool(pRemoveButton, IDS_STRUCTEDITOR_DELETE_DISABLED);
                }
                else
                    m_ttipctrl.AddTool(pRemoveButton, IDS_STRUCTEDITOR_DELETE);
            }
        }
    }

    m_ttipctrl.Activate(TRUE);

    return 0;
}

void CStructureEditorView::OnDestroy() {
    // Each CPageEntry's attribute must be released.
    if ( m_aPageEntries.GetSize() > 0) {
        for ( int i = 0; i < m_aPageEntries.GetSize(); i++ ) {
            if (m_aPageEntries[i] != NULL)
                delete m_aPageEntries[i];
        }
        m_aPageEntries.RemoveAll();
    }

    CScrollView::OnDestroy();
}

void CStructureEditorView::OnDraw(CDC* pDC) {
    CPoint ptStart(m_ptTopLeft);
    for (int i = 0; i < m_aPageEntries.GetSize(); ++i) {
        if (!m_aPageEntries[i]->PageIsHidden()) {
            m_aPageEntries[i]->Draw(pDC, ptStart);
            ptStart.y += m_aPageEntries[i]->GetHeight();
        }
    }
}

void CStructureEditorView::OnDeleteButton( UINT nID ) {  
    CString csMessage;
    csMessage.LoadString(IDS_STRUCTEDITOR_CONFIRM_QUESTION);

    CString csCaption;
    csCaption.LoadString(IDS_STRUCTEDITOR_CONFIRM_DELETE);

    int iRet = MessageBox(csMessage, csCaption, MB_YESNO | MB_TOPMOST);
    if (iRet == IDYES) {
        // mark page to be removed (on OK)
        int iPageNr = nID - IDC_PAGE_REMOVE_BUTTON_ARRAY;
        CPageEntry *pRemovingPageEntry = m_aPageEntries.ElementAt(iPageNr-1);
        pRemovingPageEntry->SetDoRemove(true);
        // set layout
        if (m_pListener != NULL)
            UpdatePageList(!m_pListener->ClickPagesAreVisible());
    }
}

BOOL CStructureEditorView::PreTranslateMessage(MSG* pMsg) {
    if(IsWindow(m_ttipctrl.m_hWnd)) {
        //m_ttipctrl.RelayEvent(pMsg);
        MSG msg = *pMsg;
        msg.hwnd = (HWND)m_ttipctrl.SendMessage(TTM_WINDOWFROMPOINT, 0,
            (LPARAM)&msg.pt);
        CPoint pt = pMsg->pt;
        if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
            ::ScreenToClient(msg.hwnd, &pt);
        msg.lParam = MAKELONG(pt.x, pt.y);

        // Let the ToolTip process this message.
        m_ttipctrl.RelayEvent(&msg);
    }

    return CScrollView::PreTranslateMessage(pMsg);
}
