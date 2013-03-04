#include "stdafx.h"
#include "WhiteboardView.h"
#include "ChangeableObject.h"
#include "ControlLineWidthSelector.h"
#include "ControlLineStyleSelector.h"
#include "InteractionAreaDialog.h"
#include "InteractionButtonDialog.h"
#include "IButtonDialog.h"
#include "IAreaDialog.h"
#include "InteractionStream.h"
#include "MainFrm.h"
#include "MoveableObjects.h"
#include "BlankGroup.h"
#include "QuestionnaireSettings.h"
#include "TestSettings.h"
#include "GraphicalObject.h"
#include "CalloutConfigDialog.h"
#include "InteractiveCalloutConfigDialog.h"
#include "ClickInfoSettingsDlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FONT_FACTOR 0.42
#ifndef WM_GOEDITSELCHANGE
#define WM_GOEDITSELCHANGE (WM_USER + 135)
#endif
#ifndef WM_GOEDITTEXTCHANGE
#define WM_GOEDITTEXTCHANGE (WM_USER + 136)
#endif

/////////////////////////////////////////////////////////////////////////////
// CWhiteboardView

// static
HRESULT CWhiteboardView::ScaleAndOffsetRect(CRect &rcOrig, CPoint *ptOffsets, double dZoom)
{
   int iWidth = rcOrig.Width();
   int iHeight = rcOrig.Height();
   
   rcOrig.left = (int)(rcOrig.left * dZoom);
   rcOrig.top = (int)(rcOrig.top * dZoom);
   
   rcOrig.right = rcOrig.left + (int)(iWidth * dZoom);
   rcOrig.bottom = rcOrig.top + (int)(iHeight * dZoom);

   if (ptOffsets != NULL)
      rcOrig.OffsetRect(*ptOffsets);
  
   return S_OK;
}


IMPLEMENT_DYNCREATE(CWhiteboardView, CView)

CWhiteboardView::CWhiteboardView()
{
    m_drawingBitmap = NULL;
    m_bitmapDC = NULL;
    m_bForceRedraw = false;

    m_bStartRangeSelection = false;
    m_bRangeSelectionStarted = false;
    m_bMoveInteractionStarted = false;
    m_bEditInteractionStarted = false;
    m_bSelectInteractionStarted = false;
    m_nInteractionType = ID_INTERACTION_AREA;

    m_hCrossCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_AREA_SELECTION);
    m_hMoveCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE_E);
    m_hLinkCursor = LoadCursor(NULL, IDC_HAND);
    m_hAddInteractionCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_ADD_INTERACTION);
    m_hEdit_nwse_Cursor = LoadCursor(NULL, IDC_SIZENWSE);
    m_hEdit_nesw_Cursor = LoadCursor(NULL, IDC_SIZENESW);
    m_hEdit_we_Cursor = LoadCursor(NULL, IDC_SIZEWE);
    m_hEdit_ns_Cursor = LoadCursor(NULL, IDC_SIZENS);

    m_dLastZoom = 1.0;
    m_iMenuZoomPercent = 100;
    m_rcLastPage = CRect(0,0,0,0);

    m_arSelectedObjects.RemoveAll();
    //m_pActiveInteraction = NULL;

	m_arTimelineDisplayObjects.RemoveAll();

    m_clrFill = RGB(255, 255, 255);
    m_clrLine = RGB(0, 0, 0);
    m_clrText = RGB(0, 0, 0);
    m_iLineWidth = 1;
    m_iLineStyle = 0;
    m_CharFormat.cbSize = sizeof(CHARFORMAT);
    m_CharFormat.dwMask = CFM_FACE | CFM_CHARSET | CFM_SIZE;
    m_CharFormat.dwEffects = 0;
    m_CharFormat.yHeight = 8 * 20; // Twips is pixel * 20
    m_CharFormat.yOffset = 0;
    m_CharFormat.crTextColor = 0;
    m_CharFormat.bPitchAndFamily = DEFAULT_PITCH;
    _tcscpy(m_CharFormat.szFaceName, _T("Arial"));

    m_bFontBold = FALSE;
    m_bFontItalic = FALSE;
    m_iAlignment = 1; // 0=left, 1=center, 2=right

    m_mapOrigSizes.InitHashTable(37);
    m_lfOrigFontHeight = 0;

    m_aFeedbackQueue.SetSize(0, 3);
    m_nActivePage = 0;

    m_nGraphicalObjectKey = -1;

    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
    CMainFrameE * pMainFrame = pEditorDoc->GetMainFrame();
    if(pMainFrame != NULL) {
        CXTPRibbonBar* pRibbonBar = pMainFrame->GetRibbonBar();
        if(pRibbonBar != NULL) {
            m_pTabStart = pRibbonBar->FindTab(ID_TAB_START_E);
        }
    }
}

CWhiteboardView::~CWhiteboardView()
{
   if (m_bitmapDC)
   {      
      m_bitmapDC->DeleteDC();
      delete m_bitmapDC;
   }
   m_bitmapDC = NULL;

   if (m_drawingBitmap)
   {
      m_drawingBitmap->DeleteObject();
      delete m_drawingBitmap;
   }
   m_drawingBitmap = NULL;

   for (int i = 0; i < m_clipBoard.GetSize(); ++i)
   {
      if (m_clipBoard[i])
         delete m_clipBoard[i];
   }
   m_clipBoard.RemoveAll();

}

void CWhiteboardView::ResetVariables()
{
    if (m_drawingBitmap != NULL)
        delete m_drawingBitmap;
    m_drawingBitmap = NULL;

    if (m_bitmapDC != NULL)
        delete m_bitmapDC;
    m_bitmapDC = NULL;

    m_bForceRedraw = false;

    m_bStartRangeSelection = false;
    m_bRangeSelectionStarted = false;
    m_bMoveInteractionStarted = false;
    m_bEditInteractionStarted = false;
    m_bSelectInteractionStarted = false;
    m_nInteractionType = ID_INTERACTION_AREA;

    m_dLastZoom = 1.0;
    m_iMenuZoomPercent = 100;
    m_rcLastPage = CRect(0,0,0,0);

    m_arSelectedObjects.RemoveAll();
    //m_pActiveInteraction = NULL;
	m_arTimelineDisplayObjects.RemoveAll();

    m_clrFill = RGB(255, 255, 255);
    m_clrLine = RGB(0, 0, 0);
    m_clrText = RGB(0, 0, 0);
    m_iLineWidth = 1;
    m_iLineStyle = 0;
    m_CharFormat.cbSize = sizeof(CHARFORMAT);
    m_CharFormat.dwMask = CFM_FACE | CFM_CHARSET | CFM_SIZE;
    m_CharFormat.dwEffects = 0;
    m_CharFormat.yHeight = 8 * 20; // Twips is pixel * 20
    m_CharFormat.yOffset = 0;
    m_CharFormat.crTextColor = 0;
    m_CharFormat.bPitchAndFamily = DEFAULT_PITCH;
    _tcscpy(m_CharFormat.szFaceName, _T("Arial"));

    m_bFontBold = FALSE;
    m_bFontItalic = FALSE;

    m_nGraphicalObjectKey = -1;

    for (int i = 0; i < m_clipBoard.GetSize(); ++i)
    {
        if (m_clipBoard[i])
            delete m_clipBoard[i];
    }
    m_clipBoard.RemoveAll();
}

BEGIN_MESSAGE_MAP(CWhiteboardView, CView)
	//{{AFX_MSG_MAP(CWhiteboardView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_INTERACTION_STACKING_ORDER, OnInteractionStackingOrder)
	ON_COMMAND(ID_INTERACTION_PROPERTIES, OnInteractionProperties)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_DELETE_E, OnEditDelete)
	ON_COMMAND(IDC_FILL_COLOR, OnFillColor)
	ON_UPDATE_COMMAND_UI(IDC_FILL_COLOR, OnUpdateFillColor)
	ON_COMMAND(IDS_MORE_FILL_COLORS_E, OnMoreFillColors)
	ON_XTP_EXECUTE(ID_SELECTOR_FILL_COLOR_E, OnSelectFillColor)
	ON_UPDATE_COMMAND_UI(ID_SELECTOR_FILL_COLOR_E, OnUpdateSelectFillColor)
	ON_COMMAND(IDC_LINE_COLOR, OnLineColor)
	ON_UPDATE_COMMAND_UI(IDC_LINE_COLOR, OnUpdateLineColor)
	ON_COMMAND(IDS_MORE_LINE_COLORS_E, OnMoreLineColors)
	ON_XTP_EXECUTE(ID_SELECTOR_LINE_COLOR_E, OnSelectLineColor)
	ON_UPDATE_COMMAND_UI(ID_SELECTOR_LINE_COLOR_E, OnUpdateSelectLineColor)
	ON_COMMAND(IDC_TEXT_COLOR, OnTextColor)
	ON_UPDATE_COMMAND_UI(IDC_TEXT_COLOR, OnUpdateTextColor)
	ON_COMMAND(IDS_MORE_TEXT_COLORS_E, OnMoreTextColors)
	ON_XTP_EXECUTE(ID_SELECTOR_TEXT_COLOR_E, OnSelectTextColor)
	ON_UPDATE_COMMAND_UI(ID_SELECTOR_TEXT_COLOR_E, OnUpdateSelectTextColor)
	ON_COMMAND(IDC_LINE_WIDTH, OnLineWidth)
	ON_UPDATE_COMMAND_UI(IDC_LINE_WIDTH, OnUpdateLineWidth)
	ON_XTP_EXECUTE(ID_SELECTOR_LINE_WIDTH_E, OnSelectLineWidth)
	ON_UPDATE_COMMAND_UI(ID_SELECTOR_LINE_WIDTH_E, OnUpdateSelectLineWidth)
	ON_COMMAND(IDC_LINE_STYLE, OnLineStyle)
	ON_UPDATE_COMMAND_UI(IDC_LINE_STYLE, OnUpdateLineStyle)
	ON_XTP_EXECUTE(ID_SELECTOR_LINE_STYLE_E, OnSelectLineStyle)
    ON_UPDATE_COMMAND_UI(ID_SELECTOR_LINE_STYLE_E, OnUpdateSelectLineStyle)
    //ON_COMMAND(IDC_FONT_FAMILY, OnFontFamily)
    ON_XTP_EXECUTE(IDC_FONT_FAMILY, OnFontFamily)
    ON_UPDATE_COMMAND_UI(IDC_FONT_FAMILY, OnUpdateFontFamily)
    ON_UPDATE_COMMAND_UI(ID_FONT_FAMILY_LIST_E, OnUpdateFontList)
    ON_UPDATE_COMMAND_UI(ID_FONT_SIZE_LIST_E, OnUpdateFontList)
    //ON_COMMAND(IDC_FONT_SIZE, OnFontSize)
    ON_XTP_EXECUTE(IDC_FONT_SIZE, OnFontSize)
    ON_UPDATE_COMMAND_UI(IDC_FONT_SIZE, OnUpdateFontSize)
    ON_COMMAND(IDC_FONT_WEIGHT, OnFontWeight)
    ON_UPDATE_COMMAND_UI(IDC_FONT_WEIGHT, OnUpdateFontWeight)
    ON_COMMAND(IDC_FONT_SLANT, OnFontSlant)
    ON_UPDATE_COMMAND_UI(IDC_FONT_SLANT, OnUpdateFontSlant)
    ON_XTP_EXECUTE(ID_FONT_SIZE_INCREASE_E, OnFontSizeIncrease)
    ON_XTP_EXECUTE(ID_FONT_SIZE_DECREASE_E, OnFontSizeDecrease)
    ON_UPDATE_COMMAND_UI(ID_FONT_SIZE_INCREASE_E, OnUpdateFontSizeIncrease)
    ON_UPDATE_COMMAND_UI(ID_FONT_SIZE_DECREASE_E, OnUpdateFontSizeDecrease)
    ON_COMMAND(IDC_FONT_ALIGN_LEFT_E, OnAlignLeft)
    ON_UPDATE_COMMAND_UI(IDC_FONT_ALIGN_LEFT_E, OnUpdateAlignLeft)
    ON_COMMAND(IDC_FONT_ALIGN_CENTER_E, OnAlignCenter)
    ON_UPDATE_COMMAND_UI(IDC_FONT_ALIGN_CENTER_E, OnUpdateAlignCenter)
    ON_COMMAND(IDC_FONT_ALIGN_RIGHT_E, OnAlignRight)
    ON_UPDATE_COMMAND_UI(IDC_FONT_ALIGN_RIGHT_E, OnUpdateAlignRight)
    ON_WM_LBUTTONDBLCLK()
    ON_COMMAND(IDC_DELETE_QUESTION, OnDeleteQuestion)
    ON_UPDATE_COMMAND_UI(IDC_DELETE_QUESTION, OnUpdateDeleteQuestion)
    ON_BN_CLICKED(IDC_SHOW_QUESTIONAIRE_SETTINGS, OnShowQuestionaireSettings)
    ON_UPDATE_COMMAND_UI(IDC_SHOW_QUESTIONAIRE_SETTINGS, OnUpdateShowQuestionnaireSettings)
    ON_UPDATE_COMMAND_UI(IDC_STACK_ORDER, OnUpdateStackOrder)
    ON_COMMAND(ID_STACKORDER_BRINGTOFRONT, OnBringToFront)
    ON_COMMAND(ID_STACKORDER_SENDTOBACK, OnSendToBack)
    ON_COMMAND(ID_STACKORDER_BRINGFORWARD, OnBringForward)
    ON_COMMAND(ID_STACKORDER_SENDBACKWARD, OnSendBackward)
    ON_COMMAND(IDC_REMOVE_ALL_DEMO_OBJECTS, OnDeleteAllDemoDocumentObjects)
    ON_COMMAND(ID_CALLOUT_CLICK_INFO_SETTINGS, OnClickInfoSettings)
    ON_COMMAND(ID_CALLOUT_USE_CLICK_INFO_TEMPLATE, OnClickUseInfoTemplate)
    ON_COMMAND_RANGE(ID_CALLOUT_STYLE_TOP_LEFT, ID_CALLOUT_STYLE_RIGHT_TOP, OnChangeCalloutStyle)
    ON_MESSAGE(WM_GOEDITSELCHANGE, OnGoEditSelChange)
    ON_MESSAGE(WM_GOEDITTEXTCHANGE, OnGoEditTextChange)

    

    // Only used if there is a screen grabbing in simulation mode
    ON_COMMAND(ID_ZOOM_50, OnZoom50)
    ON_COMMAND(ID_ZOOM_100, OnZoom100)
    ON_COMMAND(ID_ZOOM_200, OnZoom200)
    ON_COMMAND(ID_ZOOM_FIT, OnZoomFit)
    ON_UPDATE_COMMAND_UI(ID_ZOOM_50, OnUpdateVideoZoom)
    ON_UPDATE_COMMAND_UI(ID_ZOOM_100, OnUpdateVideoZoom)
    ON_UPDATE_COMMAND_UI(ID_ZOOM_200, OnUpdateVideoZoom)
    ON_UPDATE_COMMAND_UI(ID_ZOOM_FIT, OnUpdateVideoZoom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CWhiteboardView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	
   m_popupInteraction.LoadMenu(IDR_INTERACTION_CONTEXT);
   m_popupCallout.LoadMenu(IDR_CONTEXT_MENU_CALLOUT);
   // Only used if there is a screen grabbing in simulation mode
   m_ZoomPopup.LoadMenu(IDR_VIDEOZOOM);

	BOOL bCwndResult = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

   return bCwndResult;
}

BOOL CWhiteboardView::PreTranslateMessage(MSG* pMsg)
{
    BOOL bResult = FALSE;

    if ((pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_F2 || pMsg->wParam == 65 || pMsg->wParam == 66
        || pMsg->wParam == 69 || pMsg->wParam == 83)) || pMsg->message == WM_CHAR)
    {
        if (m_arSelectedObjects.GetSize() == 1 && m_nGraphicalObjectKey == -1)
        {
            if(m_arSelectedObjects[0]->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT)
            {
                CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
                CInteractionStream *pInteractionStream = pDoc->project.GetInteractionStream();
                CGraphicalObject *pGraphicalObject = (CGraphicalObject *)m_arSelectedObjects[0];
                pGraphicalObject->CreateEditControl();
                m_nGraphicalObjectKey = pGraphicalObject->GetHashKey();
                CGraphicalObjectEditCtrl *pEdit = pGraphicalObject->GetEditControl();
                pEdit->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
            }
        }
    }

    POSITION pos = m_mapOrigSizes.GetStartPosition();
    while (pos != NULL && bResult == FALSE) {
        CWnd *pWnd = NULL;
        CRect rcOrig;
        m_mapOrigSizes.GetNextAssoc(pos, pWnd, rcOrig);
        if (pWnd != NULL)
            bResult = pWnd->PreTranslateMessage(pMsg);
    }

    if (bResult != FALSE)
        return bResult;


    return CView::PreTranslateMessage(pMsg);
}

// public functions

CEdit* CWhiteboardView::CreateTextField(CRect& rcOrig)
{
   static int id = 234806;


   CRect rcScaled = rcOrig;
   UpdateZoom();
   CPoint ptOffset(m_rcLastPage.left, m_rcLastPage.top);
   ScaleAndOffsetRect(rcScaled, &ptOffset, m_dLastZoom);
    
   CEdit *pEdit = new CExtEdit();
   pEdit->Create(WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, rcScaled, this, id++);
   pEdit->LimitText(200);

   CFont *font = new CFont();
   int iFontHeight = (int)(FONT_FACTOR * rcOrig.Height() * m_dLastZoom * 10);
   font->CreatePointFont(iFontHeight, _T("Arial"));
   ((CExtEdit *)pEdit)->SetExtFont(font);     

   m_mapOrigSizes.SetAt(pEdit, rcOrig); // makes a copy of the CRect

   return pEdit;
}

CGraphicalObjectEditCtrl* CWhiteboardView::CreateGraphicalObjectTextField(CRect& rcEditDimensions, CRect& rcCalloutDimensions) {
    
    CGraphicalObjectEditCtrl *pEdit = new CGraphicalObjectEditCtrl();
    pEdit->Create(WS_CHILD | ES_MULTILINE | ES_CENTER | ES_NOHIDESEL | ES_MULTILINE | ES_WANTRETURN, rcEditDimensions, this, ID_EDIT_TEXT);
    pEdit->SetEventMask(pEdit->GetEventMask() | ENM_REQUESTRESIZE | ENM_SELCHANGE);
    pEdit->SetOffsets(m_rcLastPage.TopLeft());
    TranslateFromObjectCoordinates(rcEditDimensions);
    TranslateFromObjectCoordinates(rcCalloutDimensions);

    return pEdit;
}

HRESULT CWhiteboardView::MoveTextField(CEdit *pEdit, int iDiffX, int iDiffY)
{
   if (pEdit == NULL)
      return E_POINTER;

   if (iDiffX != 0 || iDiffY != 0)
   {
      CRect rcOrig;
      BOOL bFound = m_mapOrigSizes.Lookup(pEdit, rcOrig);

      if (bFound)
      {
         rcOrig.OffsetRect(iDiffX, iDiffY);
         m_mapOrigSizes.SetAt(pEdit, rcOrig);

         ResizeChildWindows(pEdit); // update the window with the right scaled position

         return S_OK;
      }
   }

   return S_FALSE;
}

HRESULT CWhiteboardView::RemoveTextField(CEdit *pEdit)
{
   if (pEdit == NULL)
      return E_POINTER;

   if (pEdit->m_hWnd != NULL) // main window not yet closed
   {
      pEdit->ShowWindow(SW_HIDE);
      pEdit->DestroyWindow();
      // TODO how can you remove a child window from its parent?
   
      m_mapOrigSizes.RemoveKey(pEdit);
   }

   return S_OK;
}

HRESULT CWhiteboardView::ShowFeedback(CFeedback *pFeedback, bool bWaitForFinish)
{
   if (pFeedback == NULL)
      return E_POINTER;

   CWnd *pWndChild = GetWindow(GW_CHILD);
   while (pWndChild != NULL)
   {
      if (pWndChild->IsWindowVisible())
         pWndChild->ShowWindow(SW_HIDE);

      pWndChild = pWndChild->GetNextWindow();
   }


   m_aFeedbackQueue.Add(pFeedback); // schedule it for the next repaint
   RedrawWindow();

   if (bWaitForFinish)
   {
      WaitForFeedback();
   }

   return S_OK;
}

HRESULT CWhiteboardView::WaitForFeedback()
{
   while (IsFeedbackVisible())
      ::Sleep(50);

   CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
   if (pEditorDoc != NULL)
   {
      CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream(false);
      if (pInteractionStream != NULL)
         pInteractionStream->ShowHideDynamics(pEditorDoc->m_curPosMs, pEditorDoc->IsPreviewActive());
   }

   return S_OK;
}

HRESULT CWhiteboardView::RemoveAllFeedback()
{
   if (m_aFeedbackQueue.GetSize() > 0)
   {
      m_aFeedbackQueue.RemoveAll();
      RedrawWindow();

      return S_OK;
   }

   return S_FALSE;
}

HRESULT CWhiteboardView::ResetAfterPreview()
{
   m_aFeedbackQueue.RemoveAll();
   return S_OK;
}

HRESULT CWhiteboardView::Cut()
{
   HRESULT hr = S_OK;

   CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
   if (!pEditorDoc || pEditorDoc->project.IsEmpty() || pEditorDoc->IsPreviewActive())
      hr = E_UNEXPECTED;

   if (SUCCEEDED(hr))
      hr = Copy();

   UpdateSelectionArray();

   if (SUCCEEDED(hr) && 
      m_arSelectedObjects.GetSize() > 0)
   {
      UpdateHintId idUpdate = HINT_CHANGE_PAGE;
      CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();

      bool bSelectionContainQuestionObject = false;
      bool bSelectionContainsDemoObject = false;
      for (int i = 0; i < m_arSelectedObjects.GetSize() && !bSelectionContainQuestionObject; ++i)
      {
         CInteractionAreaEx *pInteraction = m_arSelectedObjects[i];
         if (pInteraction != NULL && pInteraction->IsQuestionObject())
            bSelectionContainQuestionObject = true;
         if (pInteraction != NULL && pInteraction->IsDemoDocumentObject())
             bSelectionContainsDemoObject = true;
      }

      if (bSelectionContainQuestionObject)
      {
         CString csMessage;
         csMessage.LoadString(IDS_NONSELECTABLEOBJECTS);
         pEditorDoc->ShowErrorMessage(csMessage);
      }
      else if (bSelectionContainsDemoObject)
      {
          CString csMessage;
          csMessage.LoadString(IDS_NONSELECTABLE_DEMO_OBJECTS);
          //csMessage = _T("Cut, copy or delete operations on demo objects \nare allowed only using the related stopmark!");

          pEditorDoc->ShowErrorMessage(csMessage);
          // TODO soare: add DE resource string
      }
      else 
      {
         for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i)
         {
            CInteractionAreaEx *pInteraction = m_arSelectedObjects[i];
            if (pInteraction != NULL && pInteractionStream != NULL)
            {
               if (pInteraction->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES)
                  idUpdate = HINT_CHANGE_STRUCTURE;

               if (i == m_arSelectedObjects.GetSize() - 1)
                  hr = pInteractionStream->DeleteSingle(pInteraction->GetHashKey(), true);   // UNDO POINT
               else
                  hr = pInteractionStream->DeleteSingle(pInteraction->GetHashKey(), false);
            }
            else
               hr = E_UNEXPECTED;
         }

         DeleteSelectionArray();
         pEditorDoc->SetModifiedFlag();
         pEditorDoc->UpdateAllViews(NULL, idUpdate);
      }
   }

   return hr;
}

HRESULT CWhiteboardView::Copy()
{
   HRESULT hr = S_OK;

   CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
   if (!pEditorDoc || pEditorDoc->project.IsEmpty() || pEditorDoc->IsPreviewActive())
      hr = E_UNEXPECTED;

   UpdateSelectionArray();

   if (SUCCEEDED(hr) && m_arSelectedObjects.GetSize() > 0)
   { 
      bool bSelectionContainQuestionObject = false;
      bool bSelectionContainsDemoObject = false;
      for (int i = 0; i < m_arSelectedObjects.GetSize() && !bSelectionContainQuestionObject; ++i)
      {
         CInteractionAreaEx *pInteraction = m_arSelectedObjects[i];
         if (pInteraction != NULL && pInteraction->IsQuestionObject())
            bSelectionContainQuestionObject = true;
         if (pInteraction != NULL && pInteraction->IsDemoDocumentObject())
             bSelectionContainsDemoObject = true;
      }

      if (bSelectionContainQuestionObject)
      {
         CString csMessage;
         csMessage.LoadString(IDS_NONSELECTABLEOBJECTS);
         pEditorDoc->ShowErrorMessage(csMessage);
      }
      else if (bSelectionContainsDemoObject)
      {
          CString csMessage;
          csMessage.LoadString(IDS_NONSELECTABLE_DEMO_OBJECTS);
          //csMessage = _T("Cut, copy or delete operations on demo objects \nare allowed only using the related stopmark!");

          pEditorDoc->ShowErrorMessage(csMessage);
          // TODO soare: add DE resource string
      }
      else
      {
         CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();
         if (pInteractionStream != NULL)
            PrepareClipboardData();
         else
            hr = E_UNEXPECTED;
      }
   }

   return hr;
}

HRESULT CWhiteboardView::Delete()
{
    HRESULT hr = S_OK;
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
    if (!pEditorDoc || pEditorDoc->project.IsEmpty() || pEditorDoc->IsPreviewActive()){
        hr = E_UNEXPECTED;
    }

    UpdateSelectionArray();
    if (SUCCEEDED(hr) && m_arSelectedObjects.GetSize() > 0) {
        CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();
        UpdateHintId idUpdate = HINT_CHANGE_PAGE;
        bool bSelectionContainQuestionObject = false;
      //bool bSelectionContainsDemoObject = false;
      for (int i = 0; i < m_arSelectedObjects.GetSize() && !bSelectionContainQuestionObject; ++i)
      {
         CInteractionAreaEx *pInteraction = m_arSelectedObjects[i];
         if (pInteraction != NULL && pInteraction->IsQuestionObject())
            bSelectionContainQuestionObject = true;
         /*if (pInteraction != NULL && pInteraction->IsDemoDocumentObject())
             bSelectionContainsDemoObject = true;*/
      }

      if (bSelectionContainQuestionObject)
      {
         CString csMessage;
         csMessage.LoadString(IDS_NONSELECTABLEOBJECTS);
         pEditorDoc->ShowErrorMessage(csMessage);
      } else {
      //else if (bSelectionContainsDemoObject)
      //{
      //    CString csMessage;
      //    csMessage.LoadString(IDS_NONSELECTABLE_DEMO_OBJECTS);
      //    //csMessage = _T("Cut, copy or delete operations on demo objects \nare allowed only using the related stopmark!");

      //    pEditorDoc->ShowErrorMessage(csMessage);
      //    // TODO soare: add DE resource string
      //} else {
            for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i) {
                CInteractionAreaEx *pInteraction = m_arSelectedObjects[i];
                if (pInteraction != NULL && pInteractionStream != NULL) {
                    if (pInteraction->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES){
                        idUpdate = HINT_CHANGE_STRUCTURE;
                    }

                    bool bContinue = false;
                    if (pInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT && pInteraction->IsDemoDocumentObject())
                    {
                        CString csMess;
                        csMess.LoadString(IDS_DELETE_DEMO_MARK);
                        CString csCap;
                        csCap.LoadString(IDS_QUESTION);
                        int iId = MessageBox(csMess, csCap, MB_OKCANCEL | MB_ICONQUESTION);//pEditorDoc->ShowInfoMessage(csMess);
                        if (iId == IDCANCEL)
                            bContinue = true;
                    }
                    if (bContinue)
                        continue;

                    if (i == m_arSelectedObjects.GetSize() - 1){
                        hr = pInteractionStream->DeleteSingle(pInteraction->GetHashKey(), true);   // UNDO POINT 
                    } else {
                        hr = pInteractionStream->DeleteSingle(pInteraction->GetHashKey(), false);
                    }
                } else {
                    hr = E_UNEXPECTED;
                }
            }

            DeleteSelectionArray();
            pEditorDoc->SetModifiedFlag();
            pEditorDoc->UpdateAllViews(NULL, idUpdate);
        }
        if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive() && pEditorDoc->ShowAsScreenSimulation()){
		    pEditorDoc->UpdateScreenMode();
        }
    }

	return hr;
}

HRESULT CWhiteboardView::Paste()
{
	HRESULT hr = S_OK;

	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
	if (!pEditorDoc || pEditorDoc->project.IsEmpty() || pEditorDoc->IsPreviewActive())
		hr = E_UNEXPECTED;

	if (SUCCEEDED(hr))
	{
      CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();
      if (pInteractionStream && m_clipBoard.GetSize() > 0)
      {
         UINT nByteCount = 0;
         UINT nClipboardKey = (UINT)this;
         
         for (int i = 0; i < m_clipBoard.GetSize(); ++i)
         {
            if (SUCCEEDED(hr))
            {
               CInteractionAreaEx *pInteractionToPaste = (CInteractionAreaEx *)m_clipBoard[i];
               CInteractionAreaEx *pNewInteraction = pInteractionToPaste->Copy();

               AreaPeriodId activityPeriod = pNewInteraction->GetActivityPeriod();
               UINT nActivityStartMs = 0;
               UINT nActivityEndMs = 0;
               switch (activityPeriod)
               {
               case INTERACTION_PERIOD_PAGE:
                  pEditorDoc->project.GetPagePeriod(pEditorDoc->m_curPosMs, 
                                                    nActivityStartMs, nActivityEndMs);
                  break;
               case INTERACTION_PERIOD_END_PAGE:
                  pEditorDoc->project.GetPagePeriod(pEditorDoc->m_curPosMs, 
                                                    nActivityStartMs, nActivityEndMs);
               
                  // Note: also in this case the time needs to be the whole page.
                  break;
               case INTERACTION_PERIOD_TIME:
                  nActivityStartMs = pEditorDoc->m_curPosMs;
                  nActivityEndMs = pEditorDoc->m_curPosMs;
                  break;
               case INTERACTION_PERIOD_ALL_PAGES:
                  nActivityStartMs = 0;
                  nActivityEndMs = pEditorDoc->m_displayEndMs;
                  // TODO soare: correct paste time
                  break;
               }
               pNewInteraction->CorrectActivityUponPaste(nActivityStartMs, nActivityEndMs);
            
               //if (pNewInteraction->IsButton())
               {
                  AreaPeriodId visibilityPeriod = pNewInteraction->GetVisibilityPeriod();
                  UINT nVisibilityStartMs = 0;
                  UINT nVisibilityEndMs = 0;
                  switch (visibilityPeriod)
                  {
                  case INTERACTION_PERIOD_PAGE:
                     pEditorDoc->project.GetPagePeriod(pEditorDoc->m_curPosMs, 
                                                       nVisibilityStartMs, nVisibilityEndMs);
                     break;
                  case INTERACTION_PERIOD_END_PAGE:
                     pEditorDoc->project.GetPagePeriod(pEditorDoc->m_curPosMs, 
                                                       nVisibilityStartMs, nVisibilityEndMs);
                  
                     // Note: also in this case the time needs to be the whole page.
                     break;
                  case INTERACTION_PERIOD_TIME:
                     nVisibilityStartMs = pEditorDoc->m_curPosMs;
                     nVisibilityEndMs = pEditorDoc->m_curPosMs;
                     break;
                  case INTERACTION_PERIOD_ALL_PAGES:
                     nVisibilityStartMs = 0;
                     nVisibilityEndMs = pEditorDoc->m_displayEndMs;
                     break;
                  }
                  pNewInteraction->CorrectVisibilityUponPaste(nVisibilityStartMs, nVisibilityEndMs);
               }

               CArray<CInteractionAreaEx *, CInteractionAreaEx *> aInteractions;
               CRect rcArea = pNewInteraction->GetArea();
               pInteractionStream->FindInteractions(pEditorDoc->m_curPosMs, &rcArea, &aInteractions);
               int i = 0;
               while (i < aInteractions.GetSize())
               {
                  CRect rcExistingArea = aInteractions[i]->GetArea();
                  if (rcExistingArea == rcArea)
                  {
                     CRect rcMovedArea = rcArea;
                     rcMovedArea.OffsetRect(5, 5);

                     pNewInteraction->Move(rcMovedArea.top - rcArea.top, rcMovedArea.left - rcArea.left);
                     rcArea = pNewInteraction->GetArea();
                     aInteractions.RemoveAll();
                     pInteractionStream->FindInteractions(pEditorDoc->m_curPosMs, &rcArea, &aInteractions);
                     i = 0;
                  }
                  else
                     ++i;
               }
               pInteractionStream->InsertSingle(pNewInteraction);
               DeleteSelectionArray();
               pNewInteraction->SetSelected(true);
               m_arSelectedObjects.Add(pNewInteraction);
            }
         }
         
         pEditorDoc->SetModifiedFlag();
      }
      else
         hr = E_UNEXPECTED;
   }
   
	return hr;
}

bool CWhiteboardView::IsActive()
{
   if (m_arSelectedObjects.GetSize() > 0)
   {
      for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i)
      {
         if (m_arSelectedObjects[i] && 
            m_arSelectedObjects[i]->IsSelected())
         {
            return true;
         }
      }
   }

   return false;
}

bool CWhiteboardView::IsChangingObject()
{
	if (m_arSelectedObjects.GetSize() > 0 && 
       (m_bMoveInteractionStarted || m_bEditInteractionStarted || m_bSelectInteractionStarted))
		return true;
	else
		return false;
}

void CWhiteboardView::SetInactive()
{
   if (m_arSelectedObjects.GetSize() > 0)
   {
      DeleteSelectionArray();

      CMainFrameE::GetCurrentInstance()->UpdatePageObjectsView();
      RedrawWindow();
   }
}

void CWhiteboardView::InformDeleteSelected(CInteractionAreaEx *pInteraction)
{
   if (m_arSelectedObjects.GetSize() > 0)
   {
      for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i)
      {
         if (m_arSelectedObjects[i] && 
             m_arSelectedObjects[i] == pInteraction)
         {
            m_arSelectedObjects[i] = NULL;
            break;
         }
      }
   }
}

void CWhiteboardView::UpdateSelectionArray()
{
   for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i)
   {
      if (m_arSelectedObjects[i] && 
          m_arSelectedObjects[i]->IsSelected() == false)
         m_arSelectedObjects[i] = NULL;
   }

   CUIntArray arDeletionIndizees;
   for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i)
   {
      if (m_arSelectedObjects[i] == NULL)
         arDeletionIndizees.Add(i);
   }

   for (int i = arDeletionIndizees.GetSize() - 1; i >= 0; --i)
      m_arSelectedObjects.RemoveAt(arDeletionIndizees[i]);
}

void CWhiteboardView::DeleteSelectionArray()
{
   for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i)
   {
      if (m_arSelectedObjects[i] != NULL)
         m_arSelectedObjects[i]->SetSelected(false);
   }

   m_arSelectedObjects.RemoveAll();

}

bool CWhiteboardView::IsInSelectionArray(CInteractionAreaEx *pInteraction)
{
   bool bIsInArray = false;

   if (m_arSelectedObjects.GetSize() > 0)
   {
      for (int i = 0; i < m_arSelectedObjects.GetSize() && !bIsInArray; ++i)
      {
         if (m_arSelectedObjects[i] && 
             m_arSelectedObjects[i] == pInteraction)
            bIsInArray = true;
      }
   }

   return bIsInArray;
}

bool CWhiteboardView::IsFibObject(CInteractionAreaEx *pInteraction)
{
   if (pInteraction == NULL)
      return false;

   if (!pInteraction->IsQuestionObject())
      return false;
   
   CEditorDoc* pEditorDoc = (CEditorDoc *)GetDocument();
   if (!pEditorDoc)
      return false;

   CQuestionStream *pQuestionStream = pEditorDoc->project.GetQuestionStream(false);
   if (!pQuestionStream)
      return false;
   
   CQuestionEx *pQuestion = 
      pQuestionStream->FindQuestionForTime(pEditorDoc->m_curPosMs);
   if (!pQuestion)
      return false;

   if (pQuestion->GetType() == QUESTION_TYPE_TEXTGAP)
      return true;
   else
      return false;
}

HRESULT CWhiteboardView::GetAllQuestionObjects(CArray<CInteractionAreaEx *, CInteractionAreaEx *> &arQuestionObjects)
{
   DeleteSelectionArray();

   CEditorDoc* pEditorDoc = (CEditorDoc *)GetDocument();
   if (!pEditorDoc)
      return E_FAIL;

   CQuestionStream *pQuestionStream = pEditorDoc->project.GetQuestionStream(false);
   if (!pQuestionStream)
      return E_FAIL;
   
   CQuestionEx *pQuestion = 
      pQuestionStream->FindQuestionForTime(pEditorDoc->m_curPosMs);
   if (!pQuestion)
      return E_FAIL;
   
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
   pQuestion->GetObjectPointers(&aObjects);
   for (int i = 0; i < aObjects.GetSize(); ++i)
   {
      CInteractionAreaEx *pInteraction = aObjects[i];
      if (pInteraction != NULL)
      {
         if (pInteraction->GetClassId() == INTERACTION_CLASS_MOVE)
         {
            CMoveableObjects *pObject = (CMoveableObjects *)pInteraction;
            CArray<DrawSdk::DrawObject *,DrawSdk::DrawObject *> *paObjects = pObject->GetObjects();
            for (int j = 0; j < paObjects->GetSize(); ++j)
            {
               DrawSdk::DrawObject *pObject = paObjects->GetAt(j);
               if (pObject != NULL && pObject->GetType() == DrawSdk::TEXT)
               {
                  DrawSdk::Text *pText = (DrawSdk::Text *)pObject;
                  if (pText->GetTextType() == DrawSdk::TEXT_TYPE_BLANK_SUPPORT)
                  {
                     arQuestionObjects.Add(pInteraction);
                  }
               }
            }
         }
         else if (pInteraction->GetClassId() == INTERACTION_CLASS_DYNAMIC)
         {
            arQuestionObjects.Add(pInteraction);
         }
      }
   }

   return S_OK;
}

bool CWhiteboardView::IsClipboardEmpty()
{

   UINT nDataLength = 0;
   UINT nClipboardKey = (UINT)this;

   UINT nCount = m_clipBoard.GetSize();

   if (nCount > 0)   // data in clipboard
      return false;
   else
      return true;
}

void CWhiteboardView::ShowInteractionProperties(bool bIsClickInfo)
{
   CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
   if (!pEditorDoc || pEditorDoc->project.IsEmpty() || pEditorDoc->IsPreviewActive())
      return;

   CEditorProject *pProject = &pEditorDoc->project;
   
   CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();
   
   if (pInteractionStream == NULL)
      return;
   
   UpdateSelectionArray();

   if (m_arSelectedObjects.GetSize() == 1)
   {
      CInteractionAreaEx *pActiveInteraction = m_arSelectedObjects[0];

      if (pActiveInteraction == 0)  // not possible
         return;

      UpdateHintId idUpdate = HINT_CHANGE_PAGE;

      bool bIsPartOfQuestion = pActiveInteraction->IsQuestionObject();
         
      if (!bIsPartOfQuestion)
      {
         if (pActiveInteraction->GetClassId() == INTERACTION_CLASS_AREA)
         {
            // double code'ish (see below)

            /*CInteractionAreaDialog*/CIAreaDialog dlgInteractionArea(this);
            dlgInteractionArea.Init(pActiveInteraction, pEditorDoc);
            
            int nRet = dlgInteractionArea.DoModal();
            
            if (nRet == IDOK)
            {
               CInteractionAreaEx *pInteraction = new CInteractionAreaEx();
               CRect rcArea = pActiveInteraction->GetArea();
               dlgInteractionArea.SetUserEntries(rcArea, pInteraction);
               
               if (pActiveInteraction->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES ||
                   pInteraction->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES)
                     idUpdate = HINT_CHANGE_STRUCTURE;
                  
               HRESULT hrModify = pInteractionStream->ModifyArea(pActiveInteraction->GetHashKey(), pInteraction);
               
               // set active and select modified interaction
               if (SUCCEEDED(hrModify))
               {
                  DeleteSelectionArray();
                  pInteraction->SetSelected(true);
                  m_arSelectedObjects.Add(pInteraction);
               }
            }
         }
         else if (pActiveInteraction->GetClassId() == INTERACTION_CLASS_BUTTON)
         {
             /*CInteractionButtonDialog*/CIButtonDialog dlgInteractionButton(this);
            dlgInteractionButton.Init(pActiveInteraction, pActiveInteraction->GetArea(), pProject);
            
            int nRet = dlgInteractionButton.DoModal();
            
            if (nRet == IDOK)
            {
               CInteractionAreaEx *pInteraction = new CInteractionAreaEx();
               CRect rcArea = pActiveInteraction->GetArea();
               dlgInteractionButton.SetUserEntries(rcArea, pInteraction);
               
               if (pActiveInteraction->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES ||
                   pInteraction->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES)
                   idUpdate = HINT_CHANGE_STRUCTURE;
                  
               HRESULT hrModify = pInteractionStream->ModifyArea(pActiveInteraction->GetHashKey(), pInteraction);
               
               // set active and select modified interaction
               if (SUCCEEDED(hrModify))
               {
                  DeleteSelectionArray();
                  pInteraction->SetSelected(true);
                  m_arSelectedObjects.Add(pInteraction);  
                  SetFontAndColor(pInteraction);
               }
            }
         }
         else if (pActiveInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT)
         {
             bool bIsOkPressed = false;
             if (!bIsClickInfo) {
				 int nRet;
				 int iDisplayTime;
				 int iPeriod;
				 if(pActiveInteraction->IsDemoDocumentObject()){
					 CInteractiveCalloutConfigDialog cicdlg(this);
					 CInteractionAreaEx *pInteractionButton = ((CGraphicalObject*)pActiveInteraction)->GetDemoButton()->Copy();
					 cicdlg.Init(pActiveInteraction->GetVisibilityStart(), pActiveInteraction->GetVisibilityLength(), pEditorDoc, pInteractionButton);
					 nRet = cicdlg.DoModal();
					 if(nRet == IDOK)
					 {
						 CInteractionAreaEx *pInteraction = new CInteractionAreaEx();
						 CRect rcAreaButton = ((CGraphicalObject*)pActiveInteraction)->GetDemoButton()->GetArea();
						 CRect rcArea = pActiveInteraction->GetArea();
						 cicdlg.GetDisplayParameters(iDisplayTime, iPeriod);
						 cicdlg.SetUserEntries(rcArea, rcAreaButton, pInteraction);
						 ((CGraphicalObject*)pActiveInteraction)->AddDemoButton(pInteraction,false);
						 ((CGraphicalObject*)pActiveInteraction)->SetArea(&rcArea);
						 ((CGraphicalObject*)pActiveInteraction)->SetChanged(true);
						 SAFE_DELETE(pInteraction);

					 }
				 }
				 else{
					 CCalloutConfigDialog ccdlg(this);
					 ccdlg.Init(pActiveInteraction->GetVisibilityStart(), pActiveInteraction->GetVisibilityLength(), pEditorDoc);
					 nRet = ccdlg.DoModal();
					 if(nRet == IDOK)
						 ccdlg.GetDisplayParameters(iDisplayTime, iPeriod);
				 }
                 if(nRet == IDOK)
                 {
                     int iCurrentStart = pActiveInteraction->GetVisibilityStart();
                     int iCurrentEnd = pActiveInteraction->GetVisibilityStart() + pActiveInteraction->GetVisibilityLength() - 1;
                     if (iCurrentStart == iDisplayTime && iCurrentEnd == iDisplayTime + iPeriod)
                     {
                         bIsOkPressed = true;
                     } else {
                         CTimePeriod ctpNew;
                         ctpNew.Init(INTERACTION_PERIOD_TIME, iDisplayTime, iDisplayTime + iPeriod /*- 1*/);
                         CTimePeriod ctpActive;
                         ctpActive.Init(INTERACTION_PERIOD_TIME, ctpNew.GetEnd(), ctpNew.GetEnd());
                         bool bChangeIsPossible = true;
                         if(pActiveInteraction->IsDemoDocumentObject()) {
                             bool bSameMark = iCurrentEnd == iDisplayTime + iPeriod;
                             bChangeIsPossible = !pEditorDoc->project.GetMarkStream()->IsExactStopAt(ctpNew.GetEnd()) || bSameMark;
                             //if (bChangeIsPossible) {
                             //    /*CArray<CInteractionAreaEx*, CInteractionAreaEx*>aRelatedDemoObjects; 
                             //    HRESULT hr = pEditorDoc->project.GetInteractionStream()->FindRelatedDemoObjects(iCurrentEnd, &aRelatedDemoObjects, INTERACTION_CLASS_GRAPHICAL_OBJECT);
                             //    if (SUCCEEDED(hr) && aRelatedDemoObjects.GetCount() == 1) {
                             //    aRelatedDemoObjects.GetAt(0)->SetVisibilityPeriod(&ctpNew);
                             //    CTimePeriod ctpActive;
                             //    ctpActive.Init(INTERACTION_PERIOD_TIME, ctpNew.GetEnd(), ctpNew.GetEnd());
                             //    aRelatedDemoObjects.GetAt(0)->SetActivityPeriod(&ctpActive);
                             //    }*/
                             //    /*CInteractionAreaEx *pDemoButton = ((CGraphicalObject*)pActiveInteraction)->GetDemoButton();
                             //    if (pDemoButton != NULL) {
                             //    pDemoButton->SetActivityPeriod(&ctpActive);
                             //    }*/
                             //    CStopJumpMark* pMark = pEditorDoc->project.GetMarkStream()->GetFirstStopMarkInRange(iCurrentEnd, 1);
                             //    if( pMark != NULL && pMark->IsDemoDocumentObject()) {
                             //        pMark->IncrementPosition(ctpNew.GetEnd() - iCurrentEnd);
                             //    }
                             //}

                         }
                         if (bChangeIsPossible) {
                             HRESULT hr = pEditorDoc->project.GetInteractionStream()->ChangeRelatedDemoObjectsTimes(-1, iDisplayTime, iDisplayTime + iPeriod, (CGraphicalObject*)pActiveInteraction);
                             //pActiveInteraction->SetVisibilityPeriod(&ctpNew);
                             //pActiveInteraction->SetActivityPeriod(&ctpActive);
                             bIsOkPressed = true;
                         } else {
                             //TODO soare: Add DE resources strings 
                             CString csMess;
                             csMess.LoadString(IDS_PERIOD_CHANGE_NOT_POSSIBLE);
                             CString csCap;
                             csCap.LoadString(IDS_INFORMATION);
                             MessageBox(csMess, csCap, MB_OK | MB_ICONINFORMATION);
                         }
                     }
                 }
             } else {
                 CGraphicalObject* pGraphicalObject = (CGraphicalObject*)pActiveInteraction;
                 if (pGraphicalObject) {
                     CLcElementInfo* pLcElementInfo = pGraphicalObject->GetClickObjectLcElementInfo();
                     if (pLcElementInfo) {
                         CClickInfoSettingsDlg dlg(pLcElementInfo, this);
                         //
                         // TODO: handle SAP type in dlg.OnInitDialog(): pLcElementInfo->GetSapType()
                         //
                         if (dlg.DoModal() == IDOK) {
                             //
                             // TODO: set SAP type inside dlg
                             //
                             if (!pGraphicalObject->IsUserEdited()) {
                                pGraphicalObject->UpdateLcCalloutText();
                             }
                             bIsOkPressed = true;
                         }
                     }
                 }
             }
             if (bIsOkPressed) {
                 idUpdate = HINT_CHANGE_STRUCTURE;
                 DeleteSelectionArray();
                 pActiveInteraction->SetSelected(true);
                 m_arSelectedObjects.Add(pActiveInteraction);
                 pEditorDoc->SetModifiedFlag();
             }
         }
         else
         {
            _ASSERT(false);
            
            // shouldn't happen
         }
      }
      else // bIsPartOfQuestion == true: belongs to a question
      {
         if (pActiveInteraction->GetClassId() == INTERACTION_CLASS_FEEDBACK)
         {
            if (((CFeedback *)pActiveInteraction)->IsQuestionnaireOnly())
            {
               OnShowQuestionaireSettings();
               return;
            }
         }
         CQuestionStream *pQuestionStream = pEditorDoc->project.GetQuestionStream(false);
         if (pQuestionStream)
         {
            CQuestionEx *pQuestion = 
               pQuestionStream->FindQuestionForTime(pEditorDoc->m_curPosMs);
            if (pQuestion != NULL)
            {
               int nAnswerPageId = QUESTION_TYPE_NOTHING;
               if (pQuestion->GetType() == QUESTION_TYPE_CHOICE)
                  nAnswerPageId = ID_ADD_QUESTION_MC;
               else if (pQuestion->GetType() == QUESTION_TYPE_TEXTGAP)
                  nAnswerPageId = ID_ADD_QUESTION_FB;
               else if (pQuestion->GetType() == QUESTION_TYPE_DRAGNDROP)
                  nAnswerPageId = ID_ADD_QUESTION_DD;
               
               CString csType;
               pActiveInteraction->GetInteractionType(csType);

               CString csBtnSubmitText;
               CString csBtnResetText;
               csBtnSubmitText.LoadString(IDS_BUTTON_SUBMIT_TYPE);
               csBtnResetText.LoadString(IDS_BUTTON_DELETE_TYPE);

               if((csType == csBtnSubmitText) || (csType == csBtnResetText))
               {
                  m_nActivePage = 2;
                  OnShowQuestionaireSettings();
                  m_nActivePage = 0;
                  return;
               }

               CString csQProgText;
               CString csQProgTitleText;
               csQProgText.LoadString(IDS_CHANGEABLE_PROGRESS);
               csQProgTitleText.LoadString(IDS_QUESTION_PROGRESS);

               if((csType == csQProgText) || (csType == csQProgTitleText))
               {
                  m_nActivePage = 1;
                  OnShowQuestionaireSettings();
                  m_nActivePage = 0;
                  return;
               }

               CTestSettings dialog(IDS_TEST_SETTINGS, nAnswerPageId, this, 0);
               dialog.Init(pQuestion, pProject);
               
               CString csQTitle;
               CString csQText;
               csQTitle.LoadString(IDS_QUESTION_TITLE);
               csQText.LoadString(IDS_QUESTION_QUESTION);
               if (pActiveInteraction->GetClassId() == INTERACTION_CLASS_FEEDBACK)
                  dialog.SetActivePage(2);
               else if((csType == csQTitle) || (csType == csQText))
                  dialog.SetActivePage(0);
               else
                  dialog.SetActivePage(1);
               int nRet = dialog.DoModal();
               if (nRet == IDOK)
               {
                  CQuestionEx *pQuestionModified = new CQuestionEx();
                  int iQuestionnaireIndex;
                  dialog.SetUserEntries(pQuestionModified, iQuestionnaireIndex);
                  
                  CArray<CQuestionnaireEx *, CQuestionnaireEx *> aQuestionnaires;
                  pQuestionStream->GetQuestionnaires(&aQuestionnaires);
                  CQuestionnaireEx *pQuestionnaire = aQuestionnaires[iQuestionnaireIndex];
                  
                  if (pQuestionnaire && pQuestionnaire->IsDoStopmarks())
                  {
                     UINT nActivityEndMs = pQuestionModified->GetObjectActivityEnd();
                     CMarkStream *pMarkStream = pEditorDoc->project.GetMarkStream(true);
                     
                     CStopJumpMark mark;
                     mark.Init(false, NULL, nActivityEndMs);
                     pMarkStream->IncludeMark(&mark, false);
                  }
                  
                  CRect rcArea = pActiveInteraction->GetArea();

                  pQuestion->RemoveObjects(); // only case where you have to call this
                  pQuestionStream->ChangeQuestion(pQuestion, pQuestionModified);
                  
                  pActiveInteraction = pInteractionStream->FindTopInteraction(pEditorDoc->m_curPosMs, false, &rcArea.TopLeft());
                  if (pActiveInteraction)
                  {
                     DeleteSelectionArray();
                     pActiveInteraction->SetSelected(true);
                     m_arSelectedObjects.Add(pActiveInteraction);    
                     SetFontAndColor(pActiveInteraction);
                  }
                  pQuestionModified->UpdateChangeableTexts(pQuestionModified->GetAvailableSeconds());
                  pEditorDoc->SetModifiedFlag();
               }
            }
         }
     
      }
      
      pEditorDoc->UpdateAllViews(NULL, idUpdate);
      
    }
}

void CWhiteboardView::SetActiveInteraction(CInteractionAreaEx *pInteraction) 
{
   pInteraction->SetSelected(true);
   SetFontAndColor(pInteraction);
   bool bIsAlreadyInserted = false;
   for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i)
   {
      if (pInteraction == m_arSelectedObjects[i])
      {
         bIsAlreadyInserted = true;
         break;
      }
   }

   if (!bIsAlreadyInserted)
      m_arSelectedObjects.Add(pInteraction);
}

void CWhiteboardView::FillCharFormat(CHARFORMAT2 &cf)
{
   cf.dwMask = m_CharFormat.dwMask;
   cf.yHeight = m_CharFormat.yHeight;
   cf.yOffset = m_CharFormat.yOffset;
   cf.crTextColor = m_CharFormat.crTextColor;
   cf.bPitchAndFamily = m_CharFormat.bPitchAndFamily;
   _tcscpy(cf.szFaceName, m_CharFormat.szFaceName);
}

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CWhiteboardView 

void CWhiteboardView::OnDraw(CDC* pDC)
{
   CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

   CRect rcClient;
   GetClientRect(rcClient);

   COLORREF clrDark = CMainFrameE::GetColor(CMainFrameE::COLOR_BG_DARK);

   CXTMemDC memDC(pDC, rcClient, clrDark);
   CPaintDC dc(this);
   CXTPClientRect rcClientRect(this);
   rcClientRect.right = rcClient.right;
   rcClientRect.bottom = rcClient.bottom;
   CXTPWindowRect rcWindowClient(GetDlgItem(AFX_IDW_PANE_FIRST));
   ScreenToClient(&rcWindowClient);
   CXTPCommandBars* pCommandBars = CMainFrameE::GetCurrentInstance()->GetCommandBars();

   if (pEditorDoc != NULL && (!pEditorDoc->IsScreenGrabbingDocument() || pEditorDoc->ShowAsScreenSimulation()) && !pEditorDoc->project.IsEmpty())
   {
       // Only use blue background for "normal" document with pages
       if (!pEditorDoc->ShowAsScreenSimulation())
           ((CXTPOffice2007Theme*)(pCommandBars->GetPaintManager()))->FillWorkspace(&memDC, rcClientRect, rcWindowClient);

      // remember offset and zoom values for later mouse movements and clicks
      UpdateZoom();
      
      if (!m_bitmapDC)
         CreateWbPageBitmap();

      if (m_bitmapDC)
      {
         CRect rcPage(0, 0, m_rcLastPage.Width(), m_rcLastPage.Height());
         if (pEditorDoc->project.DrawWhiteboard(m_bitmapDC, pEditorDoc->m_curPosMs, rcPage, m_dLastZoom, m_bForceRedraw))
         {
            memDC.BitBlt(m_rcLastPage.left, m_rcLastPage.top, m_rcLastPage.Width(), m_rcLastPage.Height(), m_bitmapDC, 0, 0, SRCCOPY);
            m_bForceRedraw = false;
         }
      }
      
      CPoint ptOffsets(m_rcLastPage.left, m_rcLastPage.top);
      
      CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();
      if (pInteractionStream != NULL)
      {
         pInteractionStream->ActivateObjects(pEditorDoc->m_curPosMs, pEditorDoc->IsPreviewActive());

         pInteractionStream->Draw(&memDC, pEditorDoc->m_curPosMs, 
            &ptOffsets, m_dLastZoom, pEditorDoc->IsPreviewActive());

         // Note: Interactive elements are painted on the memDC while the objects
         // are painted on the m_bitmapDC (above). The latter one ist the case to make changes
         // to the interactive elements and the following repaint faster.
         // They need different offsets but apart from that the paint code is the same.
      }

      if (m_aFeedbackQueue.GetSize() > 0)
      {
         DrawSdk::ZoomManager zoomManager(m_dLastZoom);
         CFeedback *pFeedback = m_aFeedbackQueue[0];
         pFeedback->Draw(&memDC, &ptOffsets, &zoomManager, true);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// Diagnose CWhiteboardView

#ifdef _DEBUG
void CWhiteboardView::AssertValid() const
{
	CView::AssertValid();
}

void CWhiteboardView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CWhiteboardView 

BOOL CWhiteboardView::OnEraseBkgnd(CDC* pDC) 
{
	// return CView::OnEraseBkgnd(pDC);
   return TRUE;
}

void CWhiteboardView::OnSize(UINT nType, int cx, int cy) 
{
   CreateWbPageBitmap();
  
   ResizeChildWindows();

   // will call RedrawWindow()/OnDraw(), so it is important to be called
   // after the adaptations above
   CView::OnSize(nType, cx, cy);
}

void CWhiteboardView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (lHint != HINT_CHANGE_POSITION && lHint != HINT_CHANGE_PAGE && lHint != HINT_CHANGE_STRUCTURE)
      return; // ignore

   CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
   if (pEditorDoc != NULL && !pEditorDoc->project.IsEmpty())
   {
      CInteractionStream *pInteractions = pEditorDoc->project.GetInteractionStream(false);
      if (pInteractions != NULL)
         pInteractions->ShowHideDynamics(pEditorDoc->m_curPosMs, pEditorDoc->IsPreviewActive());
      if(m_arSelectedObjects.IsEmpty()==false)
      {
         CMainFrameE * pMainFrame = pEditorDoc->GetMainFrame();
         if(pMainFrame!=NULL)
         {
            CStreamView * pStreamView = pMainFrame->GetStreamView();
            if(pStreamView!=NULL)
            {
              CStreamCtrl * pStreamCtrl = pStreamView->GetStreamCtrl();
              if(pStreamCtrl!=NULL)
              {
                 pStreamCtrl->RemoveSlection();
              }
            }
         }
      }
   }

   if (lHint == HINT_CHANGE_STRUCTURE)
   {
      // possibly these child windows were created with the wrong scaling (1.0)
      ResizeChildWindows();
   }
   
   // Note: RWD_UPDATENOW kills performance if used too often but otherwise
   // update frequency too low.
   if (pEditorDoc != NULL && pEditorDoc->IsUserScrolling())
      RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
   else
      RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
}

void CWhiteboardView::CreateWbPageBitmap()
{
   CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
   
   if (m_bitmapDC)
   {      
      m_bitmapDC->DeleteDC();
      delete m_bitmapDC;
   }
   m_bitmapDC = NULL;

   if (pEditorDoc && !pEditorDoc->project.IsEmpty())
   {
      // delete bitmap if necessary or create new one
      if (m_drawingBitmap != NULL)
         m_drawingBitmap->DeleteObject();
      else
         m_drawingBitmap = new CBitmap();

     // create bitmap
      CDC *pDC = GetDC();
      CRect rcClient;
      GetClientRect(&rcClient);
      pEditorDoc->project.CalculateWhiteboardArea(&rcClient, &m_rcLastPage, &m_dLastZoom, m_iMenuZoomPercent);
      m_drawingBitmap->CreateCompatibleBitmap(pDC, m_rcLastPage.Width(), m_rcLastPage.Height());

      // Create bitmap DC
	   m_bitmapDC = new CDC();
	   m_bitmapDC->CreateCompatibleDC(pDC);
	   m_bitmapDC->SelectObject(m_drawingBitmap);

      ReleaseDC(pDC);

      m_bForceRedraw = true;
   }
}

void CWhiteboardView::SetFontAndColor(CInteractionAreaEx *pInteraction)
{ 
   if (!pInteraction) 
      return;
   
   if (pInteraction->GetClassId() == INTERACTION_CLASS_AREA ||
       pInteraction->GetClassId() == INTERACTION_CLASS_DYNAMIC) //||
       //pInteraction->GetClassId() == INTERACTION_CLASS_FEEDBACK)
      return;

   if(pInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT)
   {
      CGraphicalObject *pGraphicalObject = (CGraphicalObject *)pInteraction;
      DrawSdk::ColorObject *pFillPolyObject = pGraphicalObject->GetFilledObject();
      DrawSdk::ColorObject *pOutlinePolyObject = pGraphicalObject->GetOutlinedObject();
      m_clrFill = pFillPolyObject->GetFillColorRef();
      m_clrLine = pOutlinePolyObject->GetOutlineColorRef();
      m_iLineWidth = pOutlinePolyObject->GetLineWidth();
      m_iLineStyle = pOutlinePolyObject->GetLineStyle();

      LOGFONT lf;
      pGraphicalObject->GetLogFont(&lf);
      m_CharFormat.yHeight = abs(lf.lfHeight * 20);
      _tcscpy(m_CharFormat.szFaceName, lf.lfFaceName);
      m_bFontBold = (lf.lfWeight == FW_BOLD) ? TRUE : FALSE;
      m_bFontItalic = lf.lfItalic;
      m_clrText = pGraphicalObject->GetTextColor();
   }
   else if (pInteraction->GetClassId() == INTERACTION_CLASS_BUTTON)
   {
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects;
      paObjects = pInteraction->GetInactiveObjects();
      
      for (int i = 0; i < paObjects->GetSize(); ++i)
      {
         DrawSdk::DrawObject *pObject = paObjects->GetAt(i);
         if (pObject->GetType() == DrawSdk::TEXT)
         {
            DrawSdk::Text *pText = (DrawSdk::Text *)pObject;
            m_clrText = pText->GetOutlineColorRef();
            LOGFONT lf;
            pText->GetLogFont(&lf);
		    UINT nw=0,nh=0;
		    CRect rcArea = pInteraction->GetArea();
		    pText->GetTextDimension(pText->GetString(),pText->GetTextLength(),&lf,nw,nh);
            if (pInteraction->IsImageButton())
            {
		 	   if (nh > rcArea.Height())
		 	   {
		  		   lf.lfHeight = -rcArea.Height();
	 			   pText->SetFont(&lf);
				   nh = rcArea.Height();
		 	   }
			   pText->GetTextDimension(pText->GetString(),pText->GetTextLength(),&lf,nw,nh);
 			   if (nw > rcArea.Width())
			   {
				   nw = rcArea.Width();
	 			   pText->SetWidth(rcArea.Width());
			   }
			   //double dta = pText->GetTextAscent(&lf);
			   //double dy = (double)rcArea.top + ((double)rcArea.Height() - (double)nh) / 2.0 + dta + 1.5;
			   //double dx = rcArea.left + (rcArea.Width() - nw) / 2;
			   //pText->SetY(dy); 
			   //pText->SetX(dx);
	 	   }
		   if (rcArea.Width() < nw/*dTextWidth*/)
		      /*dTextWidth*/nw = rcArea.Width();
		   double dta = pText->GetTextAscent(&lf);
		   double dy = (double)rcArea.top + ((double)rcArea.Height() - (double)nh) / 2.0 + dta + 1.5;
		   double dx = (double)rcArea.left + ((double)rcArea.Width() - (double)nw) / 2;
		   pText->SetY(dy); 
		   pText->SetX(dx);
           m_CharFormat.yHeight = abs(lf.lfHeight * 20);
           _tcscpy(m_CharFormat.szFaceName, lf.lfFaceName);
           m_bFontBold = (lf.lfWeight == FW_BOLD) ? TRUE : FALSE;
           m_bFontItalic = lf.lfItalic;
         }
         if (pObject->GetType() == DrawSdk::RECTANGLE)
         {
            DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle *)pObject;
            if (pRectangle->GetIsFilled())
               m_clrFill = pRectangle->GetFillColorRef();
            else
            {
               m_clrLine = pRectangle->GetOutlineColorRef();
               m_iLineWidth = pRectangle->GetLineWidth();
               m_iLineStyle = pRectangle->GetLineStyle();
            }
         }
      }
   }
   else if (pInteraction->GetClassId() == INTERACTION_CLASS_MOVE)
   {
      CMoveableObjects *pMoveable = (CMoveableObjects *)pInteraction;
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects;
      paObjects = pMoveable->GetObjects();
      
      int iChangeableObjectsCount = paObjects->GetSize();
      if (paObjects->GetSize() > 3)
         iChangeableObjectsCount = 3;

      bool bDimensionChanged = false;
      for (int i = 0; i < iChangeableObjectsCount; ++i)
      {
         DrawSdk::DrawObject *pObject = paObjects->GetAt(i);
         if (pObject->GetType() == DrawSdk::TEXT)
         {
            DrawSdk::Text *pText = (DrawSdk::Text *)pObject;
            m_clrText = pText->GetOutlineColorRef();
            LOGFONT lf;
            pText->GetLogFont(&lf);
            m_CharFormat.yHeight = abs(lf.lfHeight * 20);
            _tcscpy(m_CharFormat.szFaceName, lf.lfFaceName);
            m_bFontBold = (lf.lfWeight == FW_BOLD) ? TRUE : FALSE;
            m_bFontItalic = lf.lfItalic;
         }
         if (pObject->GetType() == DrawSdk::RECTANGLE)
         {
            DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle *)pObject;
            if (pRectangle->GetIsFilled())
               m_clrFill = pRectangle->GetFillColorRef();
            else
            {
               m_clrLine = pRectangle->GetOutlineColorRef();
               m_iLineWidth = pRectangle->GetLineWidth();
               m_iLineStyle = pRectangle->GetLineStyle();
            }
         }
      }
   }
   else if (pInteraction->GetClassId() == INTERACTION_CLASS_CHANGE)
   {
      CChangeableObject *pChangeable = (CChangeableObject *)pInteraction;
      DrawSdk::DrawObject *pObject = pChangeable->GetObject();
      
      if (pObject->GetType() == DrawSdk::TEXT)
      {
         DrawSdk::Text *pText = (DrawSdk::Text *)pObject;
         m_clrText = pText->GetOutlineColorRef();
         LOGFONT lf;
         pText->GetLogFont(&lf);
         m_CharFormat.yHeight = abs(lf.lfHeight * 20);
         _tcscpy(m_CharFormat.szFaceName, lf.lfFaceName);
         m_bFontBold = (lf.lfWeight == FW_BOLD) ? TRUE : FALSE;
         m_bFontItalic = lf.lfItalic;
      }
   }
   else if (pInteraction->GetClassId() == INTERACTION_CLASS_BLANKS)
   {
      CBlankGroup *pBlankGroup = (CBlankGroup *)pInteraction;
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects;
      
      paObjects = pBlankGroup->GetDrawObjects();
      
      if (paObjects->GetSize() > 0)
      {
         DrawSdk::DrawObject *pObject = paObjects->GetAt(0);
         DrawSdk::Text *pText = (DrawSdk::Text *)pObject;
         m_clrText = pText->GetOutlineColorRef();
         LOGFONT lf;
         pText->GetLogFont(&lf);
         m_CharFormat.yHeight = abs(lf.lfHeight * 20);
         _tcscpy(m_CharFormat.szFaceName, lf.lfFaceName);
         m_bFontBold = (lf.lfWeight == FW_BOLD) ? TRUE : FALSE;
         m_bFontItalic = lf.lfItalic;
      }
   }
   else if (pInteraction->GetClassId() == INTERACTION_CLASS_FEEDBACK)
   {
      CFeedback* pMoveable = (CFeedback *)pInteraction;
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects;
      paObjects = pMoveable->GetObjects();
      int iChangeableObjectsCount = paObjects->GetSize();
      if (paObjects->GetSize() > 3)
         iChangeableObjectsCount = 3;

      bool bDimensionChanged = false;
      for (int i = 0; i < iChangeableObjectsCount; ++i)
      {
         DrawSdk::DrawObject *pObject = paObjects->GetAt(i);
         if (pObject->GetType() == DrawSdk::TEXT)
         {
            DrawSdk::Text *pText = (DrawSdk::Text *)pObject;
            m_clrText = pText->GetOutlineColorRef();
            LOGFONT lf;
            pText->GetLogFont(&lf);
            m_CharFormat.yHeight = abs(lf.lfHeight * 20);
            _tcscpy(m_CharFormat.szFaceName, lf.lfFaceName);
            m_bFontBold = (lf.lfWeight == FW_BOLD) ? TRUE : FALSE;
            m_bFontItalic = lf.lfItalic;
         }
         if (pObject->GetType() == DrawSdk::RECTANGLE)
         {
            DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle *)pObject;
            if (pRectangle->GetIsFilled())
               m_clrFill = pRectangle->GetFillColorRef();
            else
            {
               m_clrLine = pRectangle->GetOutlineColorRef();
               m_iLineWidth = pRectangle->GetLineWidth();
               m_iLineStyle = pRectangle->GetLineStyle();
            }
         }
      }
   }
}

bool CWhiteboardView::ChangeGraphicalObjectFontAndColor(UINT nChangingType, CGraphicalObjectEditCtrl* pEdit) {
    bool bRet = false;
    if (pEdit == NULL)
        return bRet;

    int iFontSize =  (int)(m_CharFormat.yHeight / 20 );//* GetZoomFactor() + 0.5);
    switch (nChangingType) {
        case FAMILY:
            pEdit->SetSelectionFontName(m_CharFormat.szFaceName);
            bRet = true;
            break;
        case FONTSIZE:
            pEdit->SetSelectionFontSize(iFontSize);
            bRet = true;
            break;
        case FONTSLANT:
            pEdit->SetSelectionItalic();
            bRet = true;
            break;
        case FONTWEIGHT:
            pEdit->SetSelectionBold();
            bRet = true;
            break;
        case TEXT_COLOR:
            pEdit->SetSelectionColor(m_clrText);
            bRet = true;
            break;
        case ALIGN_LEFT:
            pEdit->SetParagraphLeft();
            bRet = true;
            break;
        case ALIGN_CENTER:
            pEdit->SetParagraphCenter();
            bRet = true;
            break;
        case ALIGN_RIGHT:
            pEdit->SetParagraphRight();
            bRet = true;
            break;
        default:
            break;
    }
    return bRet;
}

LRESULT CWhiteboardView::OnGoEditSelChange(WPARAM wParam, LPARAM lParam) {
    if (m_nGraphicalObjectKey != -1) {
        CEditorDoc *pDoc = (CEditorDoc*)GetDocument();
        if (pDoc) {
            CInteractionStream* pInteractionStream =  pDoc->project.GetInteractionStream();
            if (pInteractionStream) {
                CGraphicalObject* pGraphicalObject = (CGraphicalObject*)pInteractionStream->GetInteractionWithKey(m_nGraphicalObjectKey);
                if (pGraphicalObject != NULL) {
                    CGraphicalObjectEditCtrl* pEdit = pGraphicalObject->GetEditControl();
                    bool bIsBold, bIsItalic;
                    long iFontSize;
                    int iAlign;
                    CString csFontName;
                    COLORREF clrText;
                    if (pEdit != NULL) {
                        pEdit->GetSelectionFormatParameters(bIsBold, bIsItalic, csFontName, iFontSize, clrText, iAlign);

                        int iH = (int)(iFontSize / m_dLastZoom + 0.5);
                        m_CharFormat.yHeight = /*ceil(iFontSize / GetZoomFactor())*/ iFontSize * 20;
                        _tcscpy(m_CharFormat.szFaceName, csFontName);
                        m_bFontBold = bIsBold;
                        m_bFontItalic = bIsItalic;
                        m_clrText = clrText;
                        m_iAlignment = iAlign;
                    }

                }
            }
        }
    }
    return S_OK;
}

LRESULT CWhiteboardView::OnGoEditTextChange(WPARAM wParam, LPARAM lParam) {

    GetDocument()->SetModifiedFlag();

    return S_OK;
}

void CWhiteboardView::ChangeFontAndColor(UINT nChangingType)
{
    UpdateSelectionArray();
	if (GetDocument())
		((CEditorDoc*)GetDocument())->SetModifiedFlag();

    if (m_nGraphicalObjectKey != -1) {
        bool bIsGraphicalObjectEditOperation = false;
        CEditorDoc *pDoc = (CEditorDoc*)GetDocument();
        if (pDoc) {
            CInteractionStream* pInteractionStream =  pDoc->project.GetInteractionStream();
            CGraphicalObject* pGraphicalObject = (CGraphicalObject*)pInteractionStream->GetInteractionWithKey(m_nGraphicalObjectKey);
            CGraphicalObjectEditCtrl* pEdit = pGraphicalObject->GetEditControl();
            bIsGraphicalObjectEditOperation = ChangeGraphicalObjectFontAndColor(nChangingType, pEdit);
            if (!bIsGraphicalObjectEditOperation)
                pGraphicalObject->SetGraphicalObjectText(false);
        }
        if (bIsGraphicalObjectEditOperation)
            return;
    }

    for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i)
    {
        CInteractionAreaEx *pInteraction = m_arSelectedObjects[i];
        if (pInteraction != NULL && pInteraction->IsSelected())
        {
            CInteractionAreaEx *pCopy = pInteraction->Copy(true);
            CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects = NULL;
            DrawSdk::DrawObject *pObject = NULL;
            bool bIsButton = false;
            bool bIsFeedback = false;
            if(pCopy->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT)
            {       
               /* if (m_nGraphicalObjectKey != -1){
                    ((CGraphicalObject*)pCopy)->SetGraphicalObjectText(false);
                }*/
                CGraphicalObject *pGraphicalObject = (CGraphicalObject *)pCopy;

                DrawSdk::ColorObject *pFillPolyObject = pGraphicalObject->GetFilledObject();
                DrawSdk::ColorObject *pOutlinePolyObject = pGraphicalObject->GetOutlinedObject();
                pFillPolyObject->SetFillColor(m_clrFill);
                pOutlinePolyObject->SetLineColor(m_clrLine);
                pOutlinePolyObject->SetLineWidth(m_iLineWidth);
                pOutlinePolyObject->SetLineStyle(m_iLineStyle);

               /* LOGFONT lf;
                pGraphicalObject->GetLogFont(&lf);
                if (nChangingType == FONTWEIGHT)
                    lf.lfWeight = m_bFontBold ? FW_BOLD : FW_NORMAL;
                if (nChangingType == FONTSLANT)
                    lf.lfItalic = m_bFontItalic;
                int iFontSize = -1 * (m_CharFormat.yHeight / 20);*/
                //pGraphicalObject->ChangeFontAndColor(lf, m_CharFormat.szFaceName, iFontSize, m_clrText);
            }
            else if (pCopy->GetClassId() == INTERACTION_CLASS_BUTTON)
            {
                ChangeButtonFontAndColor(pCopy, nChangingType);
                bIsButton = true;
            }
            else if (pCopy->GetClassId() == INTERACTION_CLASS_MOVE)
            {
                CMoveableObjects *pMoveable = (CMoveableObjects *)pCopy;
                paObjects = pMoveable->GetObjects();
            }
            else if (pCopy->GetClassId() == INTERACTION_CLASS_FEEDBACK)
            {
                int OFFSET_X = 20;
                int OFFSET_Y = 5;
                bIsFeedback = true;
                CFeedback *pFeedback = (CFeedback *)pCopy;
                paObjects = pFeedback->GetObjects();
                double dScaleX = 1, dScaleY = 1, dX, dY, dH, dW;
                if (paObjects && paObjects->GetSize() > 0)
                {
                    int i, iObjectCount = paObjects->GetSize();
                    bool bScaleSet = false;
                    CRect rcArea = pCopy->GetArea();
                    CRect rcNewArea = rcArea;
                    CRect rcBtnNewArea, rcBtnArea;
                    for (i = 0; i < iObjectCount; i++)
                    {
                        pObject = paObjects->GetAt(i);
                        if (pObject->GetType() == DrawSdk::TEXT)
                        {
                            DrawSdk::Text *pText = (DrawSdk::Text *)pObject;
                            dH = pText->GetHeight();
                            dW = pText->GetWidth();
                            if (nChangingType == TEXT_COLOR)
                                pText->SetLineColor(m_clrText);

                            LOGFONT lf;
                            pText->GetLogFont(&lf);

                            if (nChangingType == FONTWEIGHT)
                                lf.lfWeight = m_bFontBold ? FW_BOLD : FW_NORMAL;
                            if (nChangingType == FONTSLANT)
                                lf.lfItalic = m_bFontItalic;
                            pText->SetFont(&lf);

                            if (nChangingType == FAMILY)
                                pText->ChangeFontFamily(m_CharFormat.szFaceName);

                            if (nChangingType == FONTSIZE)
                            {
                                int iFontSize = -1 * (m_CharFormat.yHeight / 20);
                                pText->ChangeFontSize(iFontSize);
                            }

                            if (!bScaleSet)
                            {
                                dScaleX = pText->GetWidth() / dW;
                                dScaleY = pText->GetHeight() /dH;
                                dX = pText->GetX() * (dScaleX - 1);
                                dY = pText->GetY() * (dScaleY - 1);
                                bScaleSet = true;
                            }
                        }
                        if (pObject->GetType() == DrawSdk::RECTANGLE)
                        {
                            DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle *)pObject;
                            if (nChangingType == FILL_COLOR && pRectangle->GetIsFilled())
                                pRectangle->SetFillColor(m_clrFill);
                            else 
                            {
                                if (nChangingType == LINE_COLOR)
                                    pRectangle->SetLineColor(m_clrLine);
                                if (nChangingType == LINE_STYLE)
                                    pRectangle->SetLineStyle(m_iLineStyle);
                                if (nChangingType == LINE_WIDTH)
                                    pRectangle->SetLineWidth(m_iLineWidth);
                            }
                        }
                    }
                    CInteractionAreaEx *pBtnOK = pFeedback->GetOkButton();
                    rcBtnNewArea = pBtnOK->GetArea();
                    rcBtnArea = pBtnOK->GetArea();
                    rcNewArea.left = (rcArea.left - OFFSET_X) * dScaleX - dX + OFFSET_X;
                    rcNewArea.top = (rcArea.top - OFFSET_Y)  * dScaleY - dY + OFFSET_Y;
                    rcNewArea.right = (rcArea.right - OFFSET_X) * dScaleX - dX + OFFSET_X;
                    long offY = 2 * (rcArea.bottom - rcBtnArea.bottom) + rcBtnArea.Height();
                    rcNewArea.bottom = (rcArea.bottom - offY) * dScaleY - dY + offY;
                    pCopy->SetArea(&rcNewArea);
                    for (i = 0; i < iObjectCount; i++)
                    {
                        pObject = paObjects->GetAt(i);
                        if (pObject->GetType() == DrawSdk::TEXT)
                        {
                            pObject->SetX((pObject->GetX() - OFFSET_X) * dScaleX + OFFSET_X - dX);
                            pObject->SetY((pObject->GetY() - OFFSET_Y) * dScaleY + OFFSET_Y - dY);
                        }
                        if (pObject->GetType() == DrawSdk::LINE)
                        {
                            pObject->SetX((pObject->GetX() - OFFSET_X) * dScaleX + OFFSET_X - dX);
                            pObject->SetY((pObject->GetY() - OFFSET_Y) * dScaleY + OFFSET_Y - dY);
                            pObject->SetWidth(pObject->GetWidth() * dScaleX);
                            pObject->SetHeight(pObject->GetHeight() * dScaleY);
                        }
                        if (pObject->GetType() == DrawSdk::RECTANGLE)
                        {
                            pObject->SetX(rcNewArea.left);
                            pObject->SetY(rcNewArea.top);
                            pObject->SetWidth(rcNewArea.Width());
                            pObject->SetHeight(rcNewArea.Height());
                        }
                    }
                    rcBtnNewArea.left = (rcBtnArea.left - OFFSET_X) * dScaleX + OFFSET_X - dX;
                    rcBtnNewArea.top = rcNewArea.bottom - (offY - rcBtnArea.Height()) / 2 - rcBtnArea.Height();
                    rcBtnNewArea.right = rcBtnNewArea.left + rcBtnArea.Width();
                    rcBtnNewArea.bottom = rcBtnNewArea.top + rcBtnArea.Height();
                    pBtnOK->Move(rcNewArea.left + (rcNewArea.Width() - rcBtnNewArea.Width()) / 2 - rcBtnArea.left, rcBtnNewArea.top - rcBtnArea.top);

                }
            }
            else if (pCopy->GetClassId() == INTERACTION_CLASS_CHANGE)
            {
                CChangeableObject *pChangeable = (CChangeableObject *)pCopy;
                pObject = pChangeable->GetObject();
            }
            else if (pCopy->GetClassId() == INTERACTION_CLASS_BLANKS)
            {
                CBlankGroup *pBlankGroup = (CBlankGroup *)pCopy;
                paObjects = pBlankGroup->GetDrawObjects();
            }

            if ((!bIsButton)&&(!bIsFeedback))
            {
                CRect rcArea = pCopy->GetArea();
                CRect rcNewArea = rcArea;
                bool bDimensionChanged = false;

                if (paObjects && paObjects->GetSize() > 0)
                {
                    int iChangeableObjectsCount = paObjects->GetSize();

                    pObject = paObjects->GetAt(0);
                    double dXPos = pObject->GetX();
                    double dYPos = pObject->GetY();

                    bool bIsTextOnlyObject = true;
                    int i = 0;
                    for (i = 0; i < iChangeableObjectsCount && bIsTextOnlyObject; ++i)
                    {
                        pObject = paObjects->GetAt(i);
                        if (pObject->GetType() != DrawSdk::TEXT)
                            bIsTextOnlyObject = false;
                        if (pObject->GetX() != dXPos)
                            bIsTextOnlyObject = false;
                    }

                    for (i = 0; i < iChangeableObjectsCount; ++i)
                    {
                        pObject = paObjects->GetAt(i);
                        if ((nChangingType == FAMILY || nChangingType == FONTSIZE ||
                            nChangingType == FONTSLANT || nChangingType == FONTWEIGHT ||
                            nChangingType == TEXT_COLOR) &&
                            (pObject->GetType() == DrawSdk::TEXT))
                        {
                            DrawSdk::Text *pText = (DrawSdk::Text *)pObject;

                            if (nChangingType == TEXT_COLOR)
                                pText->SetLineColor(m_clrText);

                            LOGFONT lf;
                            pText->GetLogFont(&lf);

                            if (nChangingType == FONTWEIGHT)
                                lf.lfWeight = m_bFontBold ? FW_BOLD : FW_NORMAL;
                            if (nChangingType == FONTSLANT)
                                lf.lfItalic = m_bFontItalic;
                            pText->SetFont(&lf);

                            if (nChangingType == FAMILY)
                                pText->ChangeFontFamily(m_CharFormat.szFaceName);

                            if (nChangingType == FONTSIZE)
                            {
                                int iFontSize = -1 * (m_CharFormat.yHeight / 20);
                                pText->ChangeFontSize(iFontSize);
                            }

                            if (bIsTextOnlyObject)
                            {
                                pText->SetY(dYPos);
                                dYPos += pText->GetHeight();
                            }
                            if ((pCopy->GetClassId() == INTERACTION_CLASS_BUTTON && !pCopy->IsImageButton()))
                            {
                                if (pText->GetWidth() > rcNewArea.Width())
                                {
                                    rcNewArea.right = rcNewArea.left + pText->GetWidth();
                                    bDimensionChanged = true;
                                }
                                if (pText->GetHeight() > rcNewArea.Height())
                                {
                                    rcNewArea.bottom = rcNewArea.top + pText->GetHeight();
                                    bDimensionChanged = true;
                                }
                            }
                            if (pCopy->GetClassId() == INTERACTION_CLASS_MOVE ||
                                pCopy->GetClassId() == INTERACTION_CLASS_BLANKS) 
                            {
                                if (pText->GetWidth() > rcNewArea.Width())
                                {
                                    rcNewArea.right = rcNewArea.left + pText->GetWidth();
                                    bDimensionChanged = true;
                                }
                                if (pText->GetHeight() > rcNewArea.Height() )
                                {
                                    rcNewArea.bottom = rcNewArea.top + pText->GetHeight();
                                    bDimensionChanged = true;
                                }
                            }
                        }
                        if (pObject->GetType() == DrawSdk::RECTANGLE)
                        {
                            DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle *)pObject;
                            if (nChangingType == FILL_COLOR && pRectangle->GetIsFilled())
                                pRectangle->SetFillColor(m_clrFill);
                            else 
                            {
                                if (nChangingType == LINE_COLOR)
                                    pRectangle->SetLineColor(m_clrLine);
                                if (nChangingType == LINE_STYLE)
                                    pRectangle->SetLineStyle(m_iLineStyle);
                                if (nChangingType == LINE_WIDTH)
                                    pRectangle->SetLineWidth(m_iLineWidth);
                            }
                        }
                    }

                    ChangeDimension(pCopy, paObjects, rcNewArea);
                    if (bDimensionChanged)
                    {
                        pCopy->SetArea(&rcNewArea);
                    }

                }
                else if (pObject)
                {
                    if (pObject->GetType() == DrawSdk::TEXT)
                    {
                        DrawSdk::Text *pText = (DrawSdk::Text *)pObject;

                        if ((nChangingType == FAMILY || nChangingType == FONTSIZE ||
                            nChangingType == FONTSLANT || nChangingType == FONTWEIGHT ||
                            nChangingType == TEXT_COLOR)) {

                            if (nChangingType == TEXT_COLOR) {
                                pText->SetLineColor(m_clrText);
                            }
                            LOGFONT lf;
                            pText->GetLogFont(&lf);

                            if (nChangingType == FONTWEIGHT) {
                                lf.lfWeight = m_bFontBold ? FW_BOLD : FW_NORMAL;
                            }
                            if (nChangingType == FONTSLANT) {
                                lf.lfItalic = m_bFontItalic;
                            }
                            pText->SetFont(&lf);

                            if (nChangingType == FAMILY) {
                                pText->ChangeFontFamily(m_CharFormat.szFaceName);
                            }
                            if (nChangingType == FONTSIZE) {
                                int iFontSize = -1 * (m_CharFormat.yHeight / 20);
                                pText->ChangeFontSize(iFontSize);
                            }
                        }
                        pText->GetLogicalBoundingBox(&rcNewArea);
                        pCopy->SetArea(&rcNewArea);
                    }

                }
            }

            CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
            CInteractionStream *pInteractionStream = pDoc->project.GetInteractionStream(true);
            HRESULT hr = pInteractionStream->ModifyArea(pInteraction->GetHashKey(), pCopy);
            pCopy->SetSelected(true);
            m_arSelectedObjects[i] = pCopy;
            pDoc->SetModifiedFlag();
        }
    }

    RedrawWindow();
}

void CWhiteboardView::ChangeButtonFontAndColor(CInteractionAreaEx *pInteraction, UINT nChangingType)
{
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paInactiveObjects = NULL;
   paInactiveObjects = pInteraction->GetInactiveObjects();
   
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paNormalObjects = NULL;
   paNormalObjects = pInteraction->GetNormalObjects();
   
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paPressedObjects = NULL;
   paPressedObjects = pInteraction->GetPressedObjects();
   
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paOverObjects = NULL;
   paOverObjects = pInteraction->GetOverObjects();
   
   
   DrawSdk::Text *paText[4];
   DrawSdk::Rectangle *paOutlineRect[4];
   DrawSdk::Rectangle *paFilledRect[4];

   for (int i = 0; i < 4; ++i)
   {
      paText[i] = NULL;
      paOutlineRect[i] = NULL;
      paFilledRect[i] = NULL;
   }
   
   DrawSdk::DrawObject *pObject = NULL;
   if (paInactiveObjects)
   {
      for (int i = 0; i < paInactiveObjects->GetSize(); ++i)
      {
         pObject = paInactiveObjects->GetAt(i);
         if (pObject->GetType() == DrawSdk::TEXT)
         {
            paText[0] = (DrawSdk::Text *)pObject;
         }
         if (pObject->GetType() == DrawSdk::RECTANGLE)
         {
            DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle *)pObject;
            if (pRectangle->GetIsFilled())
               paFilledRect[0] = pRectangle;
            else 
               paOutlineRect[0] = pRectangle;
         }
      }
   }

   if (paNormalObjects)
   {
      for (int i = 0; i < paNormalObjects->GetSize(); ++i)
      {
         pObject = paNormalObjects->GetAt(i);
         if (pObject->GetType() == DrawSdk::TEXT)
         {
            paText[1] = (DrawSdk::Text *)pObject;
         }
         if (pObject->GetType() == DrawSdk::RECTANGLE)
         {
            DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle *)pObject;
            if (pRectangle->GetIsFilled())
               paFilledRect[1] = pRectangle;
            else 
               paOutlineRect[1] = pRectangle;
         }
      }
   }
   
   if (paPressedObjects)
   {
      for (int i = 0; i < paPressedObjects->GetSize(); ++i)
      {
         pObject = paPressedObjects->GetAt(i);
         if (pObject->GetType() == DrawSdk::TEXT)
         {
            paText[2] = (DrawSdk::Text *)pObject;
         }
         if (pObject->GetType() == DrawSdk::RECTANGLE)
         {
            DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle *)pObject;
            if (pRectangle->GetIsFilled())
               paFilledRect[2] = pRectangle;
            else 
               paOutlineRect[2] = pRectangle;
         }
      }
   }

   if (paOverObjects)
   {
      for (int i = 0; i < paOverObjects->GetSize(); ++i)
      {
         pObject = paOverObjects->GetAt(i);
         if (pObject->GetType() == DrawSdk::TEXT)
         {
            paText[3] = (DrawSdk::Text *)pObject;
         }
         if (pObject->GetType() == DrawSdk::RECTANGLE)
         {
            DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle *)pObject;
            if (pRectangle->GetIsFilled())
               paFilledRect[3] = pRectangle;
            else 
               paOutlineRect[3] = pRectangle;
         }
      }
   }
   
   LOGFONT inactiveLogFont;
   if (paText[0])
      paText[0]->GetLogFont(&inactiveLogFont);
   else
      return;

   Gdiplus::ARGB argbLineColor;
   int iLineWidth = 0;
   int iLineStyle = 0;
   if (paOutlineRect[0])
   {
      argbLineColor = paOutlineRect[0]->GetOutlineColor();
      iLineWidth = paOutlineRect[0]->GetLineWidth();
      iLineStyle = paOutlineRect[0]->GetLineStyle();
   }
   
   CRect rcArea = pInteraction->GetArea();
   CRect rcNewArea = rcArea;
   bool bDimensionChanged = false;
   
   Gdiplus::ARGB argbFillColor;
   if (paFilledRect[0])
      argbFillColor = paFilledRect[0]->GetFillColor();
   
   for (i = 0; i < 4; ++i)
   {
      if (paText[i] != NULL)
      {
         if (nChangingType == FAMILY || nChangingType == FONTSIZE ||
             nChangingType == FONTSLANT || nChangingType == FONTWEIGHT ||
             nChangingType == TEXT_COLOR)
         {
            DrawSdk::Text *pText = (DrawSdk::Text *)paText[i];
            
            if (nChangingType == TEXT_COLOR)
            {
               if (paText[0]->GetOutlineColor() == pText->GetOutlineColor())
                  pText->SetLineColor(m_clrText);
            }
            
            LOGFONT logFont;
            pText->GetLogFont(&logFont);
            
            if (nChangingType == FONTWEIGHT)
            {
               if (inactiveLogFont.lfWeight == logFont.lfWeight)
                  logFont.lfWeight = m_bFontBold ? FW_BOLD : FW_NORMAL;
            }
            if (nChangingType == FONTSLANT)
            {
               if (inactiveLogFont.lfItalic == logFont.lfItalic)
                  logFont.lfItalic = m_bFontItalic;
            }
            
            if (nChangingType == FAMILY)
            {
               if (_tcscmp(inactiveLogFont.lfFaceName, logFont.lfFaceName) == 0)
                  _tcscpy(logFont.lfFaceName, m_CharFormat.szFaceName);
            }
            
            if (nChangingType == FONTSIZE)
            {
               if (inactiveLogFont.lfHeight == logFont.lfHeight)
                  logFont.lfHeight = -1 * abs(m_CharFormat.yHeight / 20);
            }
            
            pText->SetFont(&logFont);

            if (pInteraction->GetClassId() == INTERACTION_CLASS_BUTTON)
            {
               UINT nw=0,nh=0;
			   pText->GetTextDimension(pText->GetString(),pText->GetTextLength(),&logFont,nw,nh);
               if (!pInteraction->IsImageButton())
               {
                  if (nw > rcNewArea.Width())
                  {
					  rcNewArea.right = rcNewArea.left + nw;
                     bDimensionChanged = true;
                  }
                  if (nh > rcNewArea.Height())
                  {
                     rcNewArea.bottom = rcNewArea.top + nh;
                     bDimensionChanged = true;
                  }
               }
			   else
			   {
				   if (nh > rcArea.Height())
				   {
					   logFont.lfHeight = -rcArea.Height();
					   pText->SetFont(&logFont);
				   }
				   pText->GetTextDimension(pText->GetString(),pText->GetTextLength(),&logFont,nw,nh);
				   if (nw > rcArea.Width())
					   pText->SetWidth(rcArea.Width());
			   }
            }
         }
      }

      if (nChangingType == FILL_COLOR && 
          paFilledRect[0] && paFilledRect[i])
      {
         if (argbFillColor == paFilledRect[i]->GetFillColor())
            paFilledRect[i]->SetFillColor(m_clrFill);
      }
      else if (paOutlineRect[0] && paOutlineRect[i])
      {
         if (nChangingType == LINE_COLOR && 
             argbLineColor == paOutlineRect[i]->GetOutlineColor())
            paOutlineRect[i]->SetLineColor(m_clrLine);

         if (nChangingType == LINE_WIDTH&& 
             iLineWidth == paOutlineRect[i]->GetLineWidth())
            paOutlineRect[i]->SetLineWidth(m_iLineWidth);

         if (nChangingType == LINE_STYLE && 
             iLineStyle == paOutlineRect[i]->GetLineStyle())
            paOutlineRect[i]->SetLineStyle(m_iLineStyle);
      }
   }
   
   ChangeDimension(pInteraction, paInactiveObjects, rcNewArea);
   ChangeDimension(pInteraction, paNormalObjects, rcNewArea);
   ChangeDimension(pInteraction, paPressedObjects, rcNewArea);
   ChangeDimension(pInteraction, paOverObjects, rcNewArea);
   
   if (bDimensionChanged)
   {
      pInteraction->SetArea(&rcNewArea);
   }
}

void CWhiteboardView::ChangeDimension(CInteractionAreaEx *pInteraction,
                                      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects,
                                      CRect &rcNewArea)
{
   bool bIsFeedback = false;
   int iChangeableObjectsCount = paObjects->GetSize();
   if (paObjects->GetSize() > 3)
   {
      iChangeableObjectsCount = 3;
      bIsFeedback = true;
   }
   
   if (pInteraction->GetClassId() == INTERACTION_CLASS_AREA ||
       pInteraction->GetClassId() == INTERACTION_CLASS_BUTTON)
   {
      for (int i = 0; i < iChangeableObjectsCount; ++i)
      {
         DrawSdk::DrawObject *pObject = paObjects->GetAt(i); 
      
         if (!bIsFeedback && pObject->GetType() == DrawSdk::RECTANGLE)
         {
            DrawSdk::Rectangle *pRectangle = (DrawSdk::Rectangle *)pObject;
            pObject->SetX(rcNewArea.left);
            pObject->SetY(rcNewArea.top);
            pObject->SetWidth(rcNewArea.Width());
            pObject->SetHeight(rcNewArea.Height());
         }
      
         if (pObject->GetType() == DrawSdk::TEXT)
         {
            CString csButtonText = ((DrawSdk::Text *)pObject)->GetString();
            LOGFONT lf;
            ((DrawSdk::Text *)pObject)->GetLogFont(&lf);
         
            //double dTextWidth = DrawSdk::Text::GetTextWidth(csButtonText, csButtonText.GetLength(), &lf);
            //double dTextHeight = DrawSdk::Text::GetTextHeight(csButtonText, csButtonText.GetLength(), &lf);
			UINT nw,nh;
			DrawSdk::Text::GetTextDimension(csButtonText, csButtonText.GetLength(),&lf,nw,nh);
            double dAscent = DrawSdk::Text::GetTextAscent(&lf);
			if (rcNewArea.Width() < nw/*dTextWidth*/)
				/*dTextWidth*/nw = rcNewArea.Width();
			//if (rcNewArea.Height() < nh/*dTextHeight*/)
			//	/*dTextHeight*/nh = rcNewArea.Height();
            double dXPos = (double)rcNewArea.left + ((double)rcNewArea.Width() - (double)nw/*dTextWidth*/) / 2.0;
            double dYPos = (double)rcNewArea.top + ((double)rcNewArea.Height() - (double)nh/*dTextHeight*/) / 2.0 + dAscent + 1.5;
            if (bIsFeedback)
            {
               dYPos = pObject->GetY();
            }
            pObject->SetX(dXPos);
            pObject->SetY(dYPos);
		 }
      }
   }
   else if (pInteraction->GetClassId() == INTERACTION_CLASS_BLANKS)
   {
      //CRect rcArea = pInteraction->GetArea();
      pInteraction->SetArea(&rcNewArea);
   }
   else
   {
      rcNewArea.SetRectEmpty();
      for (int i=0; i < iChangeableObjectsCount; ++i)
      {
         CRect rcOneSize;
         paObjects->GetAt(i)->GetLogicalBoundingBox((LPRECT)rcOneSize);
         
         rcNewArea.UnionRect(rcNewArea, rcOneSize);
      }
   }
}

void CWhiteboardView::OnLButtonDown(UINT nFlags, CPoint point)  {
    ClickPerformed(point);
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();

    if (pDoc != NULL && !pDoc->project.IsEmpty()) {
        if (pDoc->IsPreviewActive())	{					// Preview
            PreviewHandling(MOUSE_DOWN, nFlags, point);
        }
        else if (m_bStartRangeSelection) {				// Select range for new interaction
            DeleteSelectionArray();

            if (m_rcLastPage.PtInRect(point)) {
                SelectInteractionRange(MOUSE_DOWN, point);
                m_bRangeSelectionStarted = true;
            }
        }
        else {													// start to select, move or modify interaction
            CInteractionStream *pInteractionStream = pDoc->project.GetInteractionStream();

            UpdateSelectionArray();

            m_iEditPoint = -1;
            if (pInteractionStream != NULL) {
                if (m_arSelectedObjects.GetSize() > 0) {
					for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i) {
						if (m_arSelectedObjects[i] != NULL && m_arSelectedObjects[i]->GetVisibilityStart() <= pDoc->m_curPosMs && m_arSelectedObjects[i]->GetVisibilityStart() + m_arSelectedObjects[i]->GetVisibilityLength()-1 >= pDoc->m_curPosMs)
						{
							// 
							bool bIsEditableObject = 
								m_arSelectedObjects[i] != NULL &&
								(!m_arSelectedObjects[i]->IsButton() || !m_arSelectedObjects[i]->IsImageButton()) &&
								(m_arSelectedObjects[i]->GetClassId() != INTERACTION_CLASS_MOVE) &&
								(m_arSelectedObjects[i]->GetClassId() != INTERACTION_CLASS_DYNAMIC) &&
								(m_arSelectedObjects[i]->GetClassId() != INTERACTION_CLASS_CHANGE);

							// d&d image 
							if (!bIsEditableObject && m_arSelectedObjects[i] != NULL &&
								(m_arSelectedObjects[i]->GetClassId() == INTERACTION_CLASS_MOVE &&
								(((CMoveableObjects *)m_arSelectedObjects[i])->IsMoveableSource() ||
								((CMoveableObjects *)m_arSelectedObjects[i])->IsMoveableTarget())))
								bIsEditableObject = true;

							// modify selected interaction
							if (bIsEditableObject && 
								IsEditPoint(point, m_arSelectedObjects[i], m_iEditPoint)) {
									EditInteraction(m_arSelectedObjects[i], MOUSE_DOWN, point);
									break;
							}
							if(m_arSelectedObjects[i]->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT && m_nGraphicalObjectKey == -1) {
								CGraphicalObject *pGraphicalObject = (CGraphicalObject *)m_arSelectedObjects[i];
								CRect rcText = pGraphicalObject->GetTextRect();
								CRect rcArea = m_arSelectedObjects[i]->GetArea();
								TranslateFromObjectCoordinates(rcText);
								TranslateFromObjectCoordinates(rcArea);
								CString csText = pGraphicalObject->GetInteractionName();

								if (rcText.PtInRect(point) || (csText.IsEmpty() && rcArea.PtInRect(point))) {
									pGraphicalObject->CreateEditControl();
									m_nGraphicalObjectKey = pGraphicalObject->GetHashKey();
								}
							}
						}
					}
                }

                if (::GetCursor() == AfxGetApp()->LoadStandardCursor(IDC_IBEAM)) {
                    CPoint ptMouse;
                    BOOL bResult = ::GetCursorPos(&ptMouse);
                    ScreenToClient(&ptMouse);
                    TranslateToObjectCoordinates(&ptMouse);
                    CInteractionAreaEx *pInteraction = pInteractionStream->FindTopInteraction(
                        pDoc->m_curPosMs, false, &ptMouse);
                    if (pInteraction) {
                        if (pInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT) {
                            DeleteSelectionArray();
                            pInteraction->SetSelected(true);
                            m_arSelectedObjects.Add(pInteraction); 
                            SetFontAndColor(pInteraction);
                            CGraphicalObject *pGraphicalObject = (CGraphicalObject *)pInteraction;
                            if (m_nGraphicalObjectKey == -1) {
                                pGraphicalObject->CreateEditControl();
                                m_nGraphicalObjectKey = pGraphicalObject->GetHashKey();
                            }
                            if (m_pTabStart != NULL && !m_pTabStart->IsSelected()) {
                                m_pTabStart->Select();
                            }
                            RedrawWindow();
							GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
                        }
                    }
                }
                else {
                    // move interaction
                    if (!m_bEditInteractionStarted) {
                        CPoint ptInObjectCoord(point);
                        TranslateToObjectCoordinates(&ptInObjectCoord);
                        CInteractionAreaEx *pInteraction = 
                            pInteractionStream->FindTopInteraction(pDoc->m_curPosMs, false, &ptInObjectCoord);

                        if (pInteraction)
                            MoveInteraction(MOUSE_DOWN, point);
                        else
                            SelectInteraction(MOUSE_DOWN, point);
                    }
                }
            }
        }
    }

    SetCapture();
    CView::OnLButtonDown(nFlags, point);
}

void CWhiteboardView::OnLButtonUp(UINT nFlags, CPoint point) 
{
   ReleaseCapture();

   CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
   
   if (pDoc != NULL && !pDoc->project.IsEmpty())
   {
      HRESULT hr = S_OK;

      // this is rather double code'ish (below)
      
      if (pDoc->IsPreviewActive())						                     // Preview
      {
         hr = PreviewHandling(MOUSE_UP, nFlags, point);
      }
      else if (m_bRangeSelectionStarted)				                     // Select range for new interaction
      {
         hr = SelectInteractionRange(MOUSE_UP, point);
      }
      else if (m_bMoveInteractionStarted)			// Move or change dimension
      {
         hr = MoveInteraction(MOUSE_UP, point);
      }
      else if (m_bSelectInteractionStarted)			// Move or change dimension
      {
         hr = SelectInteraction(MOUSE_UP, point);
      }
      else if (m_bEditInteractionStarted)			// Move or change dimension
      {
         hr = EditInteraction(NULL, MOUSE_UP, point);
      }

      _ASSERT(SUCCEEDED(hr));
   }
   
   CView::OnLButtonUp(nFlags, point);
}

void CWhiteboardView::OnMouseMove(UINT nFlags, CPoint point) 
{
   // TODO also track the mouse leaving the view area (NCMOVE??)
   
   CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
   
   if (pDoc != NULL && !pDoc->project.IsEmpty())
   {
      // this is rather double code'ish (above)
      
      if (pDoc->IsPreviewActive())
      {
         PreviewHandling(MOUSE_MOVE, nFlags, point);
      }
      else if (m_bRangeSelectionStarted)                                // Select range for new interaction
      {
         SelectInteractionRange(MOUSE_MOVE, point);
      }   
      else if (m_bMoveInteractionStarted)			// Move interaction
      {  
         MoveInteraction(MOUSE_MOVE, point);
      }
      else if (m_bSelectInteractionStarted)			// Move interaction
      {  
         SelectInteraction(MOUSE_MOVE, point);
      }
      else if (m_bEditInteractionStarted)			// Change dimension
      {  
         EditInteraction(NULL, MOUSE_MOVE, point);
      }
   }
		
	CView::OnMouseMove(nFlags, point);
}

void CWhiteboardView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   UpdateSelectionArray();

 	if (nChar == VK_ESCAPE)
	{
		if (m_bRangeSelectionStarted)
		{
         // finish selection of interaction area
         // without creating a new interaction
			m_bRangeSelectionStarted = false;
		}
		else if (m_bEditInteractionStarted)
		{
         // abort editing interaction area
			m_bEditInteractionStarted = false;
      }
		else if (m_bMoveInteractionStarted)
		{
         // abort moving interaction area
			m_bMoveInteractionStarted = false;
      }
		else if (m_bSelectInteractionStarted)
		{
         // abort moving interaction area
			m_bSelectInteractionStarted = false;
      }
 
      RedrawWindow();
	}
	else if (nChar == VK_RETURN || nChar == VK_SPACE)
	{
      // finish moving interaction
		if (m_bEditInteractionStarted)
		{
         EditInteraction(NULL, MOUSE_UP, CPoint(0, 0));
		}
		if (m_bMoveInteractionStarted)
		{
         MoveInteraction(MOUSE_UP, CPoint(0, 0));
		}
		if (m_bMoveInteractionStarted)
		{
         SelectInteraction(MOUSE_UP, CPoint(0, 0));
		}
	}
	else if (nChar == VK_DELETE || nChar == VK_BACK)
	{
		Delete();
	}
	else if (nChar == VK_LEFT || nChar == VK_RIGHT ||
            nChar == VK_UP || nChar ==  VK_DOWN)
   {
      // move selected interaction
      if (m_arSelectedObjects.GetSize() > 0)
      {
         int iDiffX = 0;
         int iDiffY = 0;
         if (GetKeyState(VK_CONTROL) < 0)		// ctrl key pressed
         {
            switch (nChar)
            {
            case VK_LEFT:
               iDiffX = -1;
               break;
            case VK_RIGHT:
               iDiffX = 1;
               break;
            case VK_UP:
               iDiffY = -1;
               break;
            case VK_DOWN:
               iDiffY = 1;
               break;
            }
         }
         else
         {
            switch (nChar)
            {
            case VK_LEFT:
               iDiffX = -5;
               break;
            case VK_RIGHT:
               iDiffX = 5;
               break;
            case VK_UP:
               iDiffY = -5;
               break;
            case VK_DOWN:
               iDiffY = 5;
               break;
            }
         }
         for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i)
         {
            if (m_arSelectedObjects[i])
            {
               CRect rcObject = m_arSelectedObjects[i]->GetArea();
               CRect rcNew = rcObject;
               rcNew.OffsetRect(iDiffX, iDiffY);

               TranslateFromObjectCoordinates(rcNew);
               
               if (!m_rcLastPage.PtInRect(rcNew.TopLeft()))
               {
                  iDiffX = 0;
                  iDiffY = 0;
                  if (rcNew.left < m_rcLastPage.left)
                     iDiffX = m_rcLastPage.left - rcNew.left;
                  if (rcNew.top < m_rcLastPage.top)
                     iDiffY = m_rcLastPage.top - rcNew.top;
                  rcNew.OffsetRect(iDiffX, iDiffY);
               }
               
               if (!m_rcLastPage.PtInRect(rcNew.BottomRight()))
               {
                  iDiffX = 0;
                  iDiffY = 0;
                  if (m_rcLastPage.right < rcNew.right)
                     iDiffX = m_rcLastPage.right - rcNew.right;
                  if (m_rcLastPage.bottom < rcNew.bottom)
                     iDiffY = m_rcLastPage.bottom -rcNew.bottom;
                  rcNew.OffsetRect(iDiffX, iDiffY);
               }
               
			      CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
               CInteractionStream *pInteractionStream = NULL;
               pInteractionStream = pEditorDoc->project.GetInteractionStream();
               if (pInteractionStream)
               {
                  TranslateToObjectCoordinates(rcNew);
                  iDiffX = rcNew.left - rcObject.left;
                  iDiffY = rcNew.top - rcObject.top;

                  CInteractionAreaEx *pCopy = m_arSelectedObjects[i]->Copy(true);
                  pCopy->Move(iDiffX, iDiffY);
                  HRESULT hr = pInteractionStream->ModifyArea(m_arSelectedObjects[i]->GetHashKey(), pCopy);
                  pCopy->SetSelected(true);
                  m_arSelectedObjects[i] = pCopy;
               }
            }
         }
         
         RedrawWindow();
      }
	}
   else if (nChar == VK_F2)
   {
      if (m_arSelectedObjects.GetSize() > 0)
      {
         ShowPageView();
         CInteractionStructureView *pInteractionStructureView = 
            CMainFrameE::GetCurrentInstance()->GetInteractionStructureView();
         pInteractionStructureView->UpdateWindow();
         if (pInteractionStructureView)
            pInteractionStructureView->RefreshList();
      }
   }

   CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CWhiteboardView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
    CPoint ptMouse;
    BOOL bResult = ::GetCursorPos(&ptMouse);
    // position of mouse in window
    ScreenToClient(&ptMouse);

    bool bPreviewActive = false;
    CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
    if (pDoc != NULL && !pDoc->project.IsEmpty())
        bPreviewActive = pDoc->IsPreviewActive();

    int iEditPoint;
    if (m_rcLastPage.PtInRect(ptMouse)) {
        if (!bPreviewActive) {
            if (m_bStartRangeSelection || m_bRangeSelectionStarted) {		// get the range for a new interaction
                ::SetCursor(m_hCrossCursor);
                return TRUE;
            } else if (m_bMoveInteractionStarted) {
                if (GetKeyState(VK_CONTROL) < 0) {		// ctrl key pressed
                    ::SetCursor(m_hAddInteractionCursor);
                    return TRUE;
                }
            } else if (m_arSelectedObjects.GetSize() > 0) {
                for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i) {
					if (m_arSelectedObjects[i] != NULL && m_arSelectedObjects[i]->GetVisibilityStart() <= pDoc->m_curPosMs && m_arSelectedObjects[i]->GetVisibilityStart() + m_arSelectedObjects[i]->GetVisibilityLength()-1 >= pDoc->m_curPosMs)
					{
						// editable interaction
						bool bIsEditableObject = 
							m_arSelectedObjects[i] != NULL &&
							(!m_arSelectedObjects[i]->IsButton() || !m_arSelectedObjects[i]->IsImageButton()) &&
							(m_arSelectedObjects[i]->GetClassId() != INTERACTION_CLASS_MOVE) &&
							(m_arSelectedObjects[i]->GetClassId() != INTERACTION_CLASS_DYNAMIC) &&
							(m_arSelectedObjects[i]->GetClassId() != INTERACTION_CLASS_CHANGE);

						// d&d image
						if (!bIsEditableObject && m_arSelectedObjects[i] != NULL &&
							(m_arSelectedObjects[i]->GetClassId() == INTERACTION_CLASS_MOVE &&
							(((CMoveableObjects *)m_arSelectedObjects[i])->IsMoveableSource() ||
							((CMoveableObjects *)m_arSelectedObjects[i])->IsMoveableTarget())))
							bIsEditableObject = true;

						if (bIsEditableObject) {
							if (IsEditPoint(ptMouse, m_arSelectedObjects[i], iEditPoint)) {
								if (iEditPoint == 0 || iEditPoint == 4)
									::SetCursor(m_hEdit_nwse_Cursor);
								else if (iEditPoint == 2 || iEditPoint == 6)
									::SetCursor(m_hEdit_nesw_Cursor);
								else if (iEditPoint == 3 || iEditPoint == 7)
									::SetCursor(m_hEdit_we_Cursor);
								else if (iEditPoint == 1 || iEditPoint == 5)
									::SetCursor(m_hEdit_ns_Cursor);
								return TRUE;
							}

							CPoint ptObject = ptMouse;
							TranslateToObjectCoordinates(&ptObject);
							if (m_arSelectedObjects[i]->ContainsPoint(&ptObject)) {
								if (GetKeyState(VK_CONTROL) < 0) {		// ctrl key pressed
									::SetCursor(m_hAddInteractionCursor);
									return TRUE;
								} else {
									if (m_arSelectedObjects[i]->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT) {
										CGraphicalObject *pGraphicalObject = (CGraphicalObject *)m_arSelectedObjects[i];
										CGraphicalObjectEditCtrl *pEditCtrl = pGraphicalObject->GetEditControl();
										if (pEditCtrl != NULL) {
											CRect rcEdit;
											pEditCtrl->GetWindowRect(&rcEdit);
											ScreenToClient(&rcEdit);
											if (rcEdit.PtInRect(ptMouse)) {
												::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_IBEAM));
												return TRUE;
											}
										} else {
											CRect rcText = pGraphicalObject->GetTextRect();
											TranslateFromObjectCoordinates(rcText);
											if (rcText.PtInRect(ptMouse)) {
												::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_IBEAM));
												return TRUE;
											} 
										}
										::SetCursor(m_hMoveCursor);
										return TRUE;

									} else {
										::SetCursor(m_hMoveCursor);
										return TRUE;
									}
								}
							}
						}
					}
                }
            }

            // Not yet returned? Make a general check:

            CInteractionStream *pInteractionStream = pDoc->project.GetInteractionStream();
            if (pInteractionStream != NULL) {
                CPoint ptObject = ptMouse;
                TranslateToObjectCoordinates(&ptObject);

                CInteractionAreaEx *pInteraction = pInteractionStream->FindTopInteraction(
                    pDoc->m_curPosMs, bPreviewActive, &ptObject);

                if (pInteraction != NULL) {
                    if (GetKeyState(VK_CONTROL) < 0) {	// ctrl key pressed
                        ::SetCursor(m_hAddInteractionCursor);
                        return TRUE;
                    } else {
                        if (pInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT) {
                            CGraphicalObject *pGraphicalObject = (CGraphicalObject *)pInteraction;
                            CRect rcText = pGraphicalObject->GetTextRect();
                            TranslateFromObjectCoordinates(rcText);
                            if (rcText.PtInRect(ptMouse)) {
                                ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_IBEAM));
                                return TRUE;
                            } 
                            ::SetCursor(m_hMoveCursor);
                            return TRUE; 
                        } else {
                            ::SetCursor(m_hMoveCursor);
                            return TRUE;
                        }
                    }
                }
            }
        } else {  // bPreviewActive
            CInteractionStream *pInteractionStream = pDoc->project.GetInteractionStream();
            if (pInteractionStream != NULL) {
                CPoint ptObject = ptMouse;
                TranslateToObjectCoordinates(&ptObject);

                CInteractionAreaEx *pInteraction = pInteractionStream->FindTopInteraction(
                    pDoc->m_curPosMs, bPreviewActive, &ptObject);
                if (pInteraction != NULL) {
                    if (pInteraction->GetClassId() == INTERACTION_CLASS_MOVE) {
                        CMoveableObjects *pMoveable = (CMoveableObjects *)pInteraction;
                        if (pMoveable->ContainsPoint(&ptObject) && !pMoveable->IsDisabledForPreview()) {
                            if (pMoveable->IsMoveableSource()) { // only move real moveables during preview
                                if (GetKeyState(VK_CONTROL) < 0) { // ctrl key pressed
                                    ::SetCursor(m_hAddInteractionCursor);
                                    return TRUE;
                                } else {
                                    ::SetCursor(m_hMoveCursor);
                                    return TRUE;
                                }
                            }
                        }
                    } else if (pInteraction->IsHandCursor() && pInteraction->ActiveIntersects(pDoc->m_curPosMs, 1)) {
                        if (!pInteraction->IsDisabledForPreview()) {
                            ::SetCursor(m_hLinkCursor);
                            return TRUE;
                        }
                    }
                }
            }
        }
    }
    return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CWhiteboardView::InsertInteractionArea(CRect &rcRange)
{
	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

	if (!pEditorDoc || pEditorDoc->project.IsEmpty())
		return;


	CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream(false);
   if (pInteractionStream == NULL)
   {
      pInteractionStream = pEditorDoc->project.GetInteractionStream(true);
   }

	/*CInteractionAreaDialog*/CIAreaDialog dlgInsertInteraction(this);
   dlgInsertInteraction.Init(NULL, pEditorDoc);
   
   INT_PTR nRet = dlgInsertInteraction.DoModal();

   if (nRet == IDOK)
   {  
      CInteractionAreaEx *pInteraction = new CInteractionAreaEx();
      dlgInsertInteraction.SetUserEntries(rcRange, pInteraction);

      HRESULT hrInsert = pInteractionStream->InsertSingle(pInteraction, true);

      if (SUCCEEDED(hrInsert))
      {
         DeleteSelectionArray();
         pInteraction->SetSelected(true);
         m_arSelectedObjects.Add(pInteraction);
         

         UpdateHintId idUpdate = HINT_CHANGE_PAGE;
         if (pInteraction->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES)
            idUpdate = HINT_CHANGE_STRUCTURE;

         pEditorDoc->UpdateAllViews(NULL, idUpdate);
      }
      else
         _ASSERT(false);
      // TODO else error message??
   }
   CMainFrameE::GetCurrentInstance()->ResetCurrentIteractionId();

}

void CWhiteboardView::InsertInteractionButton(CRect &rcRange)
{
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
    if (!pEditorDoc || pEditorDoc->project.IsEmpty()){
        return;
    }
    CEditorProject *pProject = &pEditorDoc->project;
    CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream(false);
    if (pInteractionStream == NULL) {
        pInteractionStream = pEditorDoc->project.GetInteractionStream(true);
    }

    /*CInteractionButtonDialog*/CIButtonDialog dlgInsertInteraction(this);
    dlgInsertInteraction.Init(NULL, rcRange, pProject);
    INT_PTR nRet = dlgInsertInteraction.DoModal();
    if (nRet == IDOK)
    {
        CInteractionAreaEx *pInteraction = new CInteractionAreaEx();
        dlgInsertInteraction.SetUserEntries(rcRange, pInteraction);
        HRESULT hrInsert = pInteractionStream->InsertSingle(pInteraction, true);
        if (SUCCEEDED(hrInsert))
        {
            DeleteSelectionArray();
            pInteraction->SetSelected(true);
            m_arSelectedObjects.Add(pInteraction);
            SetFontAndColor(pInteraction);

            UpdateHintId idUpdate = HINT_CHANGE_PAGE;
            if (pInteraction->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES) {
                idUpdate = HINT_CHANGE_STRUCTURE;
            }

            pEditorDoc->UpdateAllViews(NULL, idUpdate);
        }
        else
            _ASSERT(false);
            // TODO else error message??
    }
    CMainFrameE::GetCurrentInstance()->ResetCurrentIteractionId();
}

// private
void CWhiteboardView::TranslateToObjectCoordinates(CPoint *pptMousePos, float *pfPrecisionObjX, float *pfPrecisionObjY)
{
   pptMousePos->x -= m_rcLastPage.left;
   pptMousePos->y -= m_rcLastPage.top;

   float fObjX = (float)(pptMousePos->x * 1.0/m_dLastZoom);
   float fObjY = (float)(pptMousePos->y * 1.0/m_dLastZoom);

   // TODO this point should correspond to the coordinates used during zoomed painting
   pptMousePos->x = (int)fObjX;
   pptMousePos->y = (int)fObjY;

   if (pfPrecisionObjX != NULL)
      *pfPrecisionObjX = fObjX;
   if (pfPrecisionObjY != NULL)
      *pfPrecisionObjY = fObjY;
}


void CWhiteboardView::TranslateToObjectCoordinates(CRect &rcView)
{
	// subtract top-left corner
	rcView.OffsetRect(-m_rcLastPage.TopLeft());

   rcView.left = (int)(rcView.left * 1.0/m_dLastZoom + 0.5);
   rcView.top = (int)(rcView.top * 1.0/m_dLastZoom + 0.5);
   rcView.right = (int)(rcView.right * 1.0/m_dLastZoom + 0.5);
   rcView.bottom = (int)(rcView.bottom * 1.0/m_dLastZoom + 0.5);
	
}

void CWhiteboardView::TranslateFromObjectCoordinates(CRect &rcObject)
{
	// add top-left corner
   rcObject.left = (int)(rcObject.left * m_dLastZoom + 0.5);
   rcObject.top = (int)(rcObject.top * m_dLastZoom + 0.5);
   rcObject.right = (int)(rcObject.right * m_dLastZoom + 0.5);
   rcObject.bottom = (int)(rcObject.bottom * m_dLastZoom + 0.5);
   
	rcObject.OffsetRect(m_rcLastPage.TopLeft());
}

bool CWhiteboardView::IsEditPoint(CPoint ptMouse, CInteractionAreaEx *pInteraction, int &iEditPoint)
{
   if (pInteraction && pInteraction->IsSelected())
   {
      CRect rcArea = pInteraction->GetArea();
      TranslateFromObjectCoordinates(rcArea);
      
      int iTolerance = 3;

      if (pInteraction->IsButton() || 
          pInteraction->GetClassId() == INTERACTION_CLASS_BLANKS ||
          (pInteraction->GetClassId() == INTERACTION_CLASS_MOVE && 
           (((CMoveableObjects *)pInteraction)->IsMoveableSource() ||
            ((CMoveableObjects *)pInteraction)->IsMoveableTarget())))
         rcArea.InflateRect(3, 3, 3, 3);

      CRect rcSelectionAreas[8];
      rcSelectionAreas[0].SetRect(rcArea.left, rcArea.top, 
                                  rcArea.left, rcArea.top);
      rcSelectionAreas[1].SetRect(rcArea.left + rcArea.Width() / 2, rcArea.top, 
                                  rcArea.left + rcArea.Width() / 2, rcArea.top);
      rcSelectionAreas[2].SetRect(rcArea.right-1, rcArea.top, 
                                  rcArea.right-1, rcArea.top);
      rcSelectionAreas[3].SetRect(rcArea.right-1, rcArea.top + rcArea.Height() / 2, 
                                  rcArea.right-1, rcArea.top + rcArea.Height() / 2);
      rcSelectionAreas[4].SetRect(rcArea.right-1, rcArea.bottom-1,
                                  rcArea.right-1, rcArea.bottom-1);
      rcSelectionAreas[5].SetRect(rcArea.left + rcArea.Width() / 2, rcArea.bottom-1,
                                  rcArea.left + rcArea.Width() / 2, rcArea.bottom-1);
      rcSelectionAreas[6].SetRect(rcArea.left, rcArea.bottom-1,
                                  rcArea.left, rcArea.bottom-1);
      rcSelectionAreas[7].SetRect(rcArea.left, rcArea.top+ rcArea.Height() / 2,
                                  rcArea.left, rcArea.top+ rcArea.Height() / 2);
      for (int i = 0; i < 8; ++i)
      {
         rcSelectionAreas[i].InflateRect(iTolerance, iTolerance, iTolerance, iTolerance);
         if (rcSelectionAreas[i].PtInRect(ptMouse))
         {
            iEditPoint = i;
            return true;
         }
      }
   }

   return false;
}

bool CWhiteboardView::ChangeableObjectsAreaSelected() {

    bool bChangeableObjectsSelected = false;

    if (m_arSelectedObjects.GetSize() > 0) {
        for (int i = 0; i < m_arSelectedObjects.GetSize() && !bChangeableObjectsSelected; ++i) {
            CInteractionAreaEx *pInteraction = m_arSelectedObjects[i];
            if(pInteraction != NULL)
            {
               bool bIsTargetArea = pInteraction->GetClassId() == INTERACTION_CLASS_MOVE &&
                  ((CMoveableObjects *)pInteraction)->IsMoveableTarget();
               if (!bIsTargetArea)
                  bChangeableObjectsSelected = true;
            }
        }
    }

    return bChangeableObjectsSelected;
}

HRESULT CWhiteboardView::PrepareClipboardData()
{
   HRESULT hr = S_OK;

   for (int i = 0; i < m_clipBoard.GetSize(); ++i)
   {
      if (m_clipBoard[i])
         delete m_clipBoard[i];
   }
   m_clipBoard.RemoveAll();

   for (i = 0; i < m_arSelectedObjects.GetSize(); ++i)
   {
      if (m_arSelectedObjects[i] != NULL &&
          m_arSelectedObjects[i]->IsSelected())
      {
         CInteractionAreaEx *pCopy = m_arSelectedObjects[i]->Copy();
         m_clipBoard.Add(pCopy);
      }
   }

   return hr;
}

HRESULT CWhiteboardView::MoveInteraction(MouseStateId nMouseState, CPoint& point)
{
   static CRect s_rcNewInteraction;
   static CRect s_rcInteraction;
   static CPoint ptStart;
   static bool bTargetMoving;
   static CInteractionAreaEx *s_pActiveInteraction;
   static CArray<CRect, CRect> s_arMoveRectangles;
   static CArray<CRect, CRect> s_arNewMoveRectangles;
   HRESULT hr = S_OK;
   
   CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
   if (!pEditorDoc || pEditorDoc->project.IsEmpty())
      hr = E_FAIL;
   
   CInteractionStream *pInteractionStream = NULL;
   if (SUCCEEDED(hr))
      pInteractionStream = pEditorDoc->project.GetInteractionStream();
   
   if (SUCCEEDED(hr) && !pInteractionStream)
      hr = E_FAIL;
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_DOWN)
   {
      // find topmost interaction at (point.x, point.y)
      CPoint ptInObjectCoord(point);
      TranslateToObjectCoordinates(&ptInObjectCoord);
      CInteractionAreaEx *pInteraction = 
         pInteractionStream->FindTopInteraction(pEditorDoc->m_curPosMs, false, &ptInObjectCoord);

      if (pInteraction)
      {
         // set active interaction to topmost interaction at (point.x, point.y)
         s_pActiveInteraction = pInteraction;
         
         
         // find out area of active interaction
         s_rcInteraction = s_pActiveInteraction->GetArea();
         TranslateFromObjectCoordinates(s_rcInteraction);
         
         // draw moving rectangle
         s_rcNewInteraction.SetRect(s_rcInteraction.TopLeft(), s_rcInteraction.BottomRight());
         ptStart.x = point.x;
         ptStart.y = point.y;
         
         CDC *pDC = GetDC();
         pDC->DrawDragRect(&s_rcNewInteraction, CSize(1, 1), NULL, CSize(1, 1));
         ReleaseDC(pDC);

         bool bObjectIsSelected = false;
         for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i)
         {
            if (m_arSelectedObjects[i] == pInteraction)
            {
               bObjectIsSelected = true;
               break;
            }
         }

         if (!bObjectIsSelected)
         {
            if (GetKeyState(VK_CONTROL) >= 0)       // ctrl key not pressed
               DeleteSelectionArray();
            pInteraction->SetSelected(true);
            m_arSelectedObjects.Add(pInteraction);

            if(pInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT) {
                if (m_pTabStart != NULL && !m_pTabStart->IsSelected()) {
                    m_pTabStart->Select();
                }
            }
         }
         
         for (i = 0; i < m_arSelectedObjects.GetSize(); ++i)
         {
            if (m_arSelectedObjects[i])
            {
               CRect rcInteraction = m_arSelectedObjects[i]->GetArea();
               TranslateFromObjectCoordinates(rcInteraction);
               s_arMoveRectangles.Add(rcInteraction);
               s_arNewMoveRectangles.Add(rcInteraction);
            }
         }

         m_bMoveInteractionStarted = true;
      }
      else
      {
         s_arMoveRectangles.RemoveAll();
         s_arNewMoveRectangles.RemoveAll();
         s_pActiveInteraction = NULL;
         
         m_bMoveInteractionStarted = false;
         
         UpdateHintId idUpdate = HINT_CHANGE_PAGE;
         GetDocument()->UpdateAllViews(NULL, idUpdate);
      }
   }
   
   if (SUCCEEDED(hr) && 
       s_pActiveInteraction != NULL && nMouseState == MOUSE_MOVE)
   {
      int nDiffX = 0;
      int nDiffY = 0;
            
      CRect rcNew = s_rcInteraction;
      if (GetKeyState(VK_SHIFT) < 0)		// shift key pressed
      {
         if (abs(point.x - ptStart.x) > abs(point.y - ptStart.y))
            nDiffX = point.x - s_rcInteraction.left;
         else
            nDiffY = point.y - s_rcInteraction.top;
         rcNew = s_rcInteraction;
         rcNew.OffsetRect(nDiffX, nDiffY);
      }
      else
      {
         nDiffX = point.x - ptStart.x;
         nDiffY = point.y - ptStart.y;
         rcNew.OffsetRect(nDiffX, nDiffY);
      }
      rcNew.NormalizeRect();
      
      for (int i = 0; i < s_arMoveRectangles.GetSize(); ++i)
      {
         if (s_arMoveRectangles[i].left + nDiffX < m_rcLastPage.left)
            nDiffX = (m_rcLastPage.left - s_arMoveRectangles[i].left);

         if (s_arMoveRectangles[i].right + nDiffX > m_rcLastPage.right)
            nDiffX = (m_rcLastPage.right - s_arMoveRectangles[i].right);

         if (s_arMoveRectangles[i].top + nDiffY < m_rcLastPage.top)
            nDiffY = (m_rcLastPage.top - s_arMoveRectangles[i].top);

         if (s_arMoveRectangles[i].bottom + nDiffY > m_rcLastPage.bottom)
            nDiffY = (m_rcLastPage.bottom - s_arMoveRectangles[i].bottom);
      }

      CArray<CRect, CRect> arNewMoveRectangles;
      for (i = 0; i < s_arMoveRectangles.GetSize(); ++i)
      {
         arNewMoveRectangles.Add(s_arMoveRectangles[i]);
         arNewMoveRectangles[i].OffsetRect(nDiffX, nDiffY);
      }
      /*/
      // interaction area can not be moved out of page
      if (!m_rcLastPage.PtInRect(rcNew.TopLeft()))
      {
         int iDiffX = 0;
         int iDiffY = 0;
         if (rcNew.left < m_rcLastPage.left)
            iDiffX = m_rcLastPage.left - rcNew.left;
         if (rcNew.top < m_rcLastPage.top)
            iDiffY = m_rcLastPage.top - rcNew.top;
         rcNew.OffsetRect(iDiffX, iDiffY);
      }
      if (!m_rcLastPage.PtInRect(rcNew.BottomRight()))
      {
         int iDiffX = 0;
         int iDiffY = 0;
         if (m_rcLastPage.right < rcNew.right)
            iDiffX = rcNew.right - m_rcLastPage.right;
         if (m_rcLastPage.bottom < rcNew.bottom)
            iDiffY = rcNew.bottom - m_rcLastPage.bottom;
         rcNew.OffsetRect(-iDiffX, -iDiffY);
      }
      /*/
      
      CDC *pDC = GetDC();
      if (arNewMoveRectangles.GetSize() > 0)
      {
         for (int i = 0; i < arNewMoveRectangles.GetSize(); ++i)
         {
            if (arNewMoveRectangles[i])
               pDC->DrawDragRect(&arNewMoveRectangles[i], CSize(1, 1), &s_arNewMoveRectangles[i], CSize(1, 1));
         }

         s_arNewMoveRectangles.RemoveAll();
         s_arNewMoveRectangles.Append(arNewMoveRectangles);
         arNewMoveRectangles.RemoveAll();
      }
      else
      {
         pDC->DrawDragRect(&rcNew, CSize(1, 1), &s_rcNewInteraction, CSize(1, 1));
      }
      ReleaseDC(pDC);
      
      s_rcNewInteraction.CopyRect(rcNew);
   }
   
   if (SUCCEEDED(hr) && 
       s_pActiveInteraction != NULL && nMouseState == MOUSE_UP)
   {
      CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream(true);
            
      CDC *pDC = GetDC();
      if (!s_rcNewInteraction.IsRectNull())
         pDC->DrawDragRect(CRect(0, 0, 0, 0), CSize(0, 0), s_rcNewInteraction, CSize(1, 1));
      ReleaseDC(pDC);
      
      // move interaction
      CPoint ptStartMove(ptStart);
      TranslateToObjectCoordinates(&ptStartMove);
      CPoint ptEndMove(point);
      TranslateToObjectCoordinates(&ptEndMove);
         
      bool bAnyChange = false;
      
      CPoint ptDiff = ptEndMove - ptStartMove;

      bool bMoveToSamePos = false;
      
      if (ptDiff.x != 0 || ptDiff.y != 0)
      {
         CRect rcOldArea = s_pActiveInteraction->GetArea();
         
         if (GetKeyState(VK_CONTROL) < 0 && !bTargetMoving)	// ctrl key pressed -> copy to new position
         {
            CArray<CInteractionAreaEx *, CInteractionAreaEx *> arCopiedObjects;
            HRESULT hrInsert = S_OK;
            for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i)
            {
               CInteractionAreaEx *pNewInteraction = m_arSelectedObjects[i]->Copy();
               if (pNewInteraction != NULL)
               {
                  m_arSelectedObjects[i]->SetSelected(false);
                  
                  // insert interaction; new interaction is active interaction
                  hrInsert = pInteractionStream->InsertSingle(pNewInteraction, false);
                  
                  // TODO make the below move an undoable change
                  // and group it with this area insertion: set rollback point after the two
                  
                  if (SUCCEEDED(hrInsert))
                  {
                     arCopiedObjects.Add(pNewInteraction);
                  }
               }
            }
            
            DeleteSelectionArray();
            if (SUCCEEDED(hrInsert))
            {
               for (i = 0; i < arCopiedObjects.GetSize(); ++i)
               {
                  arCopiedObjects[i]->SetSelected(true);
                  m_arSelectedObjects.Add(arCopiedObjects[i]);
               }
               
               bAnyChange = true;
            }
         }
         
         // adjust coordinates of draw objects to new bounding box
         
         UpdateSelectionArray();

         CArray<CInteractionAreaEx *, CInteractionAreaEx *> arCopiedObjects;
         int j = 0;
         for (int i = 0; i < m_arSelectedObjects.GetSize(); ++i)
         {
            if (m_arSelectedObjects[i])
            {
               CRect rcArea = m_arSelectedObjects[i]->GetArea();
               CInteractionAreaEx *pCopy = m_arSelectedObjects[i]->Copy(true);
               CRect rcNewArea = s_arNewMoveRectangles[j];
               TranslateToObjectCoordinates(rcNewArea);
               float fDiffX = rcNewArea.left - rcArea.left;
               float fDiffY = rcNewArea.top - rcArea.top;
               pCopy->Move(fDiffX, fDiffY);
               if (i == m_arSelectedObjects.GetSize() - 1)
                  hr = pInteractionStream->ModifyArea(m_arSelectedObjects[i]->GetHashKey(), pCopy, true);   // UNDO POINT
               else
                  hr = pInteractionStream->ModifyArea(m_arSelectedObjects[i]->GetHashKey(), pCopy, false);
              
               arCopiedObjects.Add(pCopy);
               ++j;
            }
         }
         
         DeleteSelectionArray();
         for (i =0; i < arCopiedObjects.GetSize(); ++i)
         {
            if (SUCCEEDED(hr))
            {
               arCopiedObjects[i]->SetSelected(true);
               m_arSelectedObjects.Add(arCopiedObjects[i]);
               bAnyChange = true;
            }
         }
         
      } else {
          bMoveToSamePos = true;
      }
      
      if (SUCCEEDED(hr) && m_arSelectedObjects.GetSize() == 1)
      {
         m_arSelectedObjects[0]->SetSelected(true);
         SetFontAndColor(m_arSelectedObjects[0]);
      }

      //bAnyChange = true;
      // Note: ModifyArea() also sets an undo rollback point.

      bTargetMoving = false;
      
      if ((SUCCEEDED(hr) && bAnyChange) || (SUCCEEDED(hr) && bMoveToSamePos))
      {
         UpdateHintId idUpdate = HINT_CHANGE_PAGE;
         if (s_pActiveInteraction->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES)
            idUpdate = HINT_CHANGE_STRUCTURE;

         if (!bMoveToSamePos)
            pEditorDoc->SetModifiedFlag();
         GetDocument()->UpdateAllViews(NULL, idUpdate);
      }

      // reset interaction rectangle
      s_rcNewInteraction.SetRect(0, 0, 0, 0);
      s_rcInteraction.SetRect(0, 0, 0, 0);
      s_pActiveInteraction = NULL;
      s_arMoveRectangles.RemoveAll();
      s_arNewMoveRectangles.RemoveAll();
      m_bMoveInteractionStarted = false;
   }
   
   return hr;
}


HRESULT CWhiteboardView::SelectInteraction(MouseStateId nMouseState, CPoint& point)
{
   static CRect s_rcSelectInteraction;
   static CPoint ptStart;

   HRESULT hr = S_OK;
   
   CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
   if (!pEditorDoc || pEditorDoc->project.IsEmpty())
      hr = E_FAIL;
   

   if (SUCCEEDED(hr) && nMouseState == MOUSE_DOWN)
   {
      if (GetKeyState(VK_CONTROL) >= 0)		         // ctrl key not pressed
         DeleteSelectionArray();
      s_rcSelectInteraction.SetRect(point, point);
      ptStart.x = point.x;
      ptStart.y = point.y;
      m_bSelectInteractionStarted = true;
   }
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_MOVE)
   {
      if (GetKeyState(VK_LBUTTON) < 0)				         // left mouse key pressed
      {
         if (m_bSelectInteractionStarted)
         {
            CRect rcNew;
            CPoint ptFrom(ptStart);
            CPoint ptTo(point);
            
            if (!m_rcLastPage.PtInRect(point))           // interaction area can not be outside the page
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
            
            rcNew.SetRect(ptFrom, ptTo);
            rcNew.NormalizeRect();
            
            CDC *pDC = GetDC();
            if (s_rcSelectInteraction.IsRectNull())
               pDC->DrawDragRect(&rcNew, CSize(1, 1), NULL, CSize(1, 1));
            else
               pDC->DrawDragRect(&rcNew, CSize(1, 1), &s_rcSelectInteraction, CSize(1, 1));
            ReleaseDC(pDC);

            s_rcSelectInteraction.CopyRect(&rcNew);
         }
      }
   }
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_UP)
   {
      // delete selection rectangle
      CDC *pDC = GetDC();
      if (!s_rcSelectInteraction.IsRectNull())
         pDC->DrawDragRect(CRect(0, 0, 0, 0), CSize(0, 0), s_rcSelectInteraction, CSize(1, 1));
      ReleaseDC(pDC);
      
      // open interaction dialog
      TranslateToObjectCoordinates(s_rcSelectInteraction);

      CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
      
      if (pEditorDoc)
      {
         CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();
         if (pInteractionStream)
         {
            CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
            pInteractionStream->FindInteractions(pEditorDoc->m_curPosMs, &s_rcSelectInteraction, &aObjects);
            for (int i = 0; i < aObjects.GetSize(); ++i)
            {
               aObjects[i]->SetSelected(true);
               m_arSelectedObjects.Add(aObjects[i]);
            }
            
            if (m_arSelectedObjects.GetSize() == 1)
            {
               if (m_arSelectedObjects[0] && m_arSelectedObjects[0]->IsSelected())
                  SetFontAndColor(m_arSelectedObjects[0]);
            }


            GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
         }
      }
      
      // reset interaction rectangle
      s_rcSelectInteraction.SetRect(0, 0, 0, 0);
      
      m_bSelectInteractionStarted = false;
   }
      
   return hr;
}
void CWhiteboardView::SelectAllItems()
{
   static CRect s_rcSelectInteraction;
   GetClientRect(&s_rcSelectInteraction);
   CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
   if (pEditorDoc)
   {
      CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();
      if (pInteractionStream)
      {
         CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
         pInteractionStream->ResolveTimes(pEditorDoc->m_curPosMs);
         pInteractionStream->FindInteractions(pEditorDoc->m_curPosMs, &s_rcSelectInteraction, &aObjects);
         m_arSelectedObjects.RemoveAll();
         GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
         for (int i = 0; i < aObjects.GetSize(); ++i)
         {
            if(aObjects[i]->IsSelected())
            {
              aObjects[i]->SetSelected(false);
            }
            if(!aObjects[i]->IsQuestionObject())
            {
              aObjects[i]->SetSelected(true);
              m_arSelectedObjects.Add(aObjects[i]);
            }
         }
      }
      GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
   }
}
void CWhiteboardView::SelectAllItems(UINT uiCurPos)
{
   static CRect s_rcSelectInteraction;
   GetClientRect(&s_rcSelectInteraction);
   CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
   if (pEditorDoc)
   {
      CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();
      if (pInteractionStream)
      {
         CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
         pInteractionStream->ResolveTimes(pEditorDoc->m_curPosMs);
         pInteractionStream->FindInteractions(uiCurPos, &s_rcSelectInteraction, &aObjects);
         m_arSelectedObjects.RemoveAll();
         GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
         for (int i = 0; i < aObjects.GetSize(); ++i)
         {
            if(aObjects[i]->IsSelected())
            {
              aObjects[i]->SetSelected(false);
            }
            if(!aObjects[i]->IsQuestionObject())
            {
              aObjects[i]->SetSelected(true);
              m_arSelectedObjects.Add(aObjects[i]);
            }
         }
      }
      GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
   }
}
void CWhiteboardView::DeselectAllItems()
{
   static CRect s_rcSelectInteraction;
   GetClientRect(&s_rcSelectInteraction);
   CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
   if (pEditorDoc)
   {
      CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();
      if (pInteractionStream)
      {
         CArray<CInteractionAreaEx *, CInteractionAreaEx *> aObjects;
         pInteractionStream->FindInteractions(pEditorDoc->m_curPosMs, &s_rcSelectInteraction, &aObjects);
         m_arSelectedObjects.RemoveAll();
         //GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
         for (int i = 0; i < aObjects.GetSize(); ++i)
         {
            if(aObjects[i]->IsSelected())
            {
              aObjects[i]->SetSelected(false);
            }
         }
      }
      GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
   }
}
HRESULT CWhiteboardView::EditInteraction(CInteractionAreaEx *pActiveInteraction, MouseStateId nMouseState, CPoint& point)
{
   static CRect rcNewInteraction;
   static CRect rcInteracion;
   static CPoint ptStart;
   static CInteractionAreaEx *s_pActiveInteraction;
   HRESULT hr = S_OK;

   CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();

   if (!pEditorDoc || pEditorDoc->project.IsEmpty())
      hr = E_FAIL;

   CInteractionStream *pInteractionStream = NULL;
   if (SUCCEEDED(hr))
      pInteractionStream = pEditorDoc->project.GetInteractionStream();
   
   if (SUCCEEDED(hr) && !pInteractionStream)
      hr = E_FAIL;
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_DOWN)
   {
      // find out area of active interaction
      s_pActiveInteraction = pActiveInteraction;

      rcInteracion = s_pActiveInteraction->GetArea();
      TranslateFromObjectCoordinates(rcInteracion);

      // save first mouse position
      ptStart.x = point.x;
      ptStart.y = point.y;

      // draw editing rectangle
      rcNewInteraction.SetRect(rcInteracion.TopLeft(), rcInteracion.BottomRight());

      CDC *pDC = GetDC();
      pDC->DrawDragRect(&rcNewInteraction, CSize(1, 1), NULL, CSize(1, 1));
      ReleaseDC(pDC);
      
      m_bEditInteractionStarted = true;
   }
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_MOVE)
   {
      int nDiffX = 0;
      int nDiffY = 0;
      
      CRect rcNew(rcInteracion);
      
      if (GetKeyState(VK_CONTROL) < 0)		         // ctrl key pressed
      {
         switch (m_iEditPoint)
         {
         case 0:
            rcNew.left = point.x;
            rcNew.top = point.y;
            rcNew.right += rcInteracion.left - point.x;
            rcNew.bottom += rcInteracion.top - point.y;
            break;
         case 1:
            rcNew.top = point.y;
            rcNew.bottom += rcInteracion.top - point.y;
            break;
         case 2:
            rcNew.right = point.x;
            rcNew.top = point.y;
            rcNew.left += rcInteracion.right - point.x;
            rcNew.bottom += rcInteracion.top - point.y;
            break;
         case 3:
            rcNew.right = point.x;
            rcNew.left += rcInteracion.right - point.x;
            break;
         case 4:
            rcNew.right = point.x;
            rcNew.bottom = point.y;
            rcNew.left += rcInteracion.right - point.x;
            rcNew.top += rcInteracion.bottom - point.y;
            break;
         case 5:
            rcNew.bottom = point.y;
            rcNew.top += rcInteracion.bottom - point.y;
            break;
         case 6:
            rcNew.left = point.x;
            rcNew.bottom = point.y;
            rcNew.right += rcInteracion.left - point.x;
            rcNew.top += rcInteracion.bottom - point.y;
            break;
         case 7:
            rcNew.left = point.x;
            rcNew.right += rcInteracion.left - point.x;
            break;
         default:
            break;
         }
      }
      else
      {
         switch (m_iEditPoint)
         {
         case 0:
            rcNew.left = point.x;
            rcNew.top = point.y;
            break;
         case 1:
            rcNew.top = point.y;
            break;
         case 2:
            rcNew.right = point.x;
            rcNew.top = point.y;
            break;
         case 3:
            rcNew.right = point.x;
            break;
         case 4:
            rcNew.right = point.x;
            rcNew.bottom = point.y;
            break;
         case 5:
            rcNew.bottom = point.y;
            break;
         case 6:
            rcNew.left = point.x;
            rcNew.bottom = point.y;
            break;
         case 7:
            rcNew.left = point.x;
            break;
         default:
            break;
         }
      }
      
      rcNew.NormalizeRect();
      rcNew.IntersectRect(rcNew, m_rcLastPage);
      
      CDC *pDC = GetDC();
      pDC->DrawDragRect(&rcNew, CSize(1, 1), &rcNewInteraction, CSize(1, 1));
      ReleaseDC(pDC);
      
      rcNewInteraction.CopyRect(rcNew);
   }
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_UP)
   {
      CDC *pDC = GetDC();
      if (!rcNewInteraction.IsRectNull())
         pDC->DrawDragRect(CRect(0, 0, 0, 0), CSize(0, 0), rcNewInteraction, CSize(1, 1));
      ReleaseDC(pDC);
      
      // move interaction
      TranslateToObjectCoordinates(rcNewInteraction);
      
      if (SUCCEEDED(hr))
      {
         CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream(true);
      
         CInteractionAreaEx *pCopy = s_pActiveInteraction->Copy(true);
         
         if (!pCopy)
            hr = E_OUTOFMEMORY;
         
         if (SUCCEEDED(hr))
         {
            if (pCopy->IsButton())
            {
               // ???: May be you can use Move
               CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects[4];
               paObjects[0] = pCopy->GetInactiveObjects();
               paObjects[1] = pCopy->GetNormalObjects();
               paObjects[2] = pCopy->GetOverObjects();
               paObjects[3] = pCopy->GetPressedObjects();
               
               for (int i = 0; i < 4; ++i)
               {
                  for (int j = 0; j < paObjects[i]->GetSize(); ++j)
                  {
                     DrawSdk::DrawObject *pObject = paObjects[i]->GetAt(j);
                     if (pObject->GetType() == DrawSdk::TEXT)
                     {
                        int iWidth = abs((int)pObject->GetWidth());
                        int iHeight = abs((int)pObject->GetHeight());
                        
                        if (iWidth > rcNewInteraction.Width())
                           rcNewInteraction.right = rcNewInteraction.left + iWidth;
                        
                        if (iHeight > rcNewInteraction.Height())
                           rcNewInteraction.bottom = rcNewInteraction.top + iHeight;
                     }
                  }
               }
               
               CRect rcOldArea = pCopy->GetArea();
               for (i = 0; i < 4; ++i)
                  ChangeDimension(pCopy, paObjects[i], rcNewInteraction);
            }
            else if (pCopy->GetClassId() == INTERACTION_CLASS_MOVE &&
                     (((CMoveableObjects *)pCopy)->IsMoveableSource() ||
                      ((CMoveableObjects *)pCopy)->IsMoveableTarget()))
            {
               CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects;
               paObjects = ((CMoveableObjects *)pCopy)->GetObjects();

               for (int i = 0; i < paObjects->GetSize(); ++i)
               {
                  DrawSdk::DrawObject *pObject = paObjects->GetAt(i);
                  if (pObject && pObject->GetType() == DrawSdk::IMAGE)
                  {
                     pObject->SetX(rcNewInteraction.left);
                     pObject->SetY(rcNewInteraction.top);
                     pObject->SetWidth(rcNewInteraction.Width());
                     pObject->SetHeight(rcNewInteraction.Height());
                  }
                  else if (pObject && pObject->GetType() == DrawSdk::TARGET)
                  {
                     pObject->SetX(rcNewInteraction.left);
                     pObject->SetY(rcNewInteraction.top);
                     pObject->SetWidth(rcNewInteraction.Width());
                     pObject->SetHeight(rcNewInteraction.Height());
                  }
               }
            }

            pCopy->SetArea(&rcNewInteraction);
            hr = pInteractionStream->ModifyArea(s_pActiveInteraction->GetHashKey(), pCopy);
         
            pCopy->SetSelected(true);

            pEditorDoc->SetModifiedFlag();
            m_arSelectedObjects.Add(pCopy);

            GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
         }
      }

      // reset interaction rectangle
      rcNewInteraction.SetRect(0, 0, 0, 0);
      
      m_bEditInteractionStarted = false;

   }
   
   return hr;
}

HRESULT CWhiteboardView::SelectInteractionRange(MouseStateId nMouseState, CPoint& point)
{
   static CRect rcNewInteraction;
   static CPoint ptStart;

   HRESULT hr = S_OK;
   
   CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
   if (!pEditorDoc || pEditorDoc->project.IsEmpty())
      hr = E_FAIL;
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_DOWN)
   {
      rcNewInteraction.SetRect(point, point);
      ptStart.x = point.x;
      ptStart.y = point.y;
      m_bStartRangeSelection = false;
   }
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_MOVE)
   {
      if (GetKeyState(VK_LBUTTON) < 0)				         // left mouse key pressed
      {
         if (m_bRangeSelectionStarted)
         {
            CRect rcNew;
            CPoint ptFrom(ptStart);
            CPoint ptTo(point);
            
            if (!m_rcLastPage.PtInRect(point))           // interaction area can not be outside the page
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
            
            if (GetKeyState(VK_SHIFT) < 0)		         // shift key pressed
            {
               // rectangle is square
               int nDiffX = point.x - ptStart.x;
               int nDiffY = point.y - ptStart.y;
               
               if (abs(nDiffX) > abs(nDiffY))
               {
                  ptTo.x = point.x;
                  if (nDiffY > 0)
                     ptTo.y = ptStart.y + abs(nDiffX);
                  else
                     ptTo.y = ptStart.y - abs(nDiffX);
               }
               else
               {
                  ptTo.x = point.y;
                  if (nDiffX > 0)
                     ptTo.x = ptStart.x + abs(nDiffY);
                  else
                     ptTo.x = ptStart.x - abs(nDiffY);
               }
            }
            
            if (GetKeyState(VK_CONTROL) < 0)		         // ctrl key pressed
            {
               // use ptTo to calculate diffx and diffy, 
               // because SHIFT can also be pressed, 
               int nDiffX = abs(ptTo.x - ptFrom.x);
               int nDiffY = abs(ptTo.y - ptFrom.y);
               
               ptFrom.x = ptStart.x - nDiffX;
               ptTo.x   = ptStart.x + nDiffX;
               ptFrom.y = ptStart.y - nDiffY;
               ptTo.y   = ptStart.y + nDiffY;
            }
            
            rcNew.SetRect(ptFrom, ptTo);
            rcNew.NormalizeRect();
            
            CDC *pDC = GetDC();
            if (rcNewInteraction.IsRectNull())
               pDC->DrawDragRect(&rcNew, CSize(1, 1), NULL, CSize(1, 1));
            else
               pDC->DrawDragRect(&rcNew, CSize(1, 1), &rcNewInteraction, CSize(1, 1));
            ReleaseDC(pDC);

            rcNewInteraction.CopyRect(&rcNew);
         }
      }
   }
   
   if (SUCCEEDED(hr) && nMouseState == MOUSE_UP)
   {
      // delete selection rectangle
      CDC *pDC = GetDC();
      if (!rcNewInteraction.IsRectNull())
         pDC->DrawDragRect(CRect(0, 0, 0, 0), CSize(0, 0), rcNewInteraction, CSize(1, 1));
      ReleaseDC(pDC);
      
      // open interaction dialog
      TranslateToObjectCoordinates(rcNewInteraction);
      if(m_nInteractionType == ID_GRAPHICALOBJECT_CALLOUT || m_nInteractionType == ID_GRAPHICALOBJECT_TEXT)
          InsertGraphicalObject(rcNewInteraction, GOT_NOTHING);
      else if (m_nInteractionType == ID_DEMOOBJECT)
          InsertDemoGroup(rcNewInteraction);
      else if (m_nInteractionType == ID_INTERACTION_AREA)
         InsertInteractionArea(rcNewInteraction);
      else
         InsertInteractionButton(rcNewInteraction);

      pEditorDoc->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
      
      pEditorDoc->SetModifiedFlag();
      // reset interaction rectangle
      rcNewInteraction.SetRect(0, 0, 0, 0);
      
      m_bRangeSelectionStarted = false;
   }
      
   return hr;
}

HRESULT CWhiteboardView::PreviewHandling(MouseStateId idMouseState, UINT nFlags, CPoint& ptMouse)
{
   HRESULT hr = S_OK;
   static CInteractionAreaEx *pLastDownInteraction = NULL;
   static CInteractionAreaEx *pLastMoveInteraction = NULL;
   static CMoveableObjects   *pLastMoveMoveable    = NULL;
   static float fObjLastX = 0;
   static float fObjLastY = 0;
   static CArray<CPoint, CPoint> aTargetPoints;
   static int iLastTargetX = -1;
   static int iLastTargetY = -1;

   CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
   if (pEditorDoc == NULL || pEditorDoc->project.IsEmpty())
      return E_FAIL;

   CInteractionStream *pInteractionStream = NULL;
   if (SUCCEEDED(hr))
      pInteractionStream = pEditorDoc->project.GetInteractionStream(false);
   
   if (SUCCEEDED(hr) && pInteractionStream == NULL)
      return E_FAIL;

   float fObjMouseX = 0;
   float fObjMouseY = 0;
   CPoint ptObjMouse = ptMouse;
   TranslateToObjectCoordinates(&ptObjMouse, &fObjMouseX, &fObjMouseY);
 
   CInteractionAreaEx *pActiveInteraction = NULL;
   if (m_aFeedbackQueue.GetSize() == 0)
   {
      pActiveInteraction = pInteractionStream->FindTopInteraction(pEditorDoc->m_curPosMs, true, &ptObjMouse);
  
      if (pActiveInteraction != NULL)
      {
         // only handle active objects
         
         if (!pActiveInteraction->ActiveIntersects(pEditorDoc->m_curPosMs, 1)
            || pActiveInteraction->IsDisabledForPreview())
            pActiveInteraction = NULL;
         if (pActiveInteraction != NULL && pActiveInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT && pActiveInteraction->IsDemoDocumentObject()) {
             CInteractionAreaEx *pDemoButton = ((CGraphicalObject*)pActiveInteraction)->GetDemoButton();
             if (pDemoButton != NULL && pDemoButton->ContainsPoint(&ptObjMouse)) {
                 pActiveInteraction = pDemoButton;
             }
         }
      }
   }
   else
   {
      // feedback is visible: only check for the ok button
      CInteractionAreaEx *pOkButton = m_aFeedbackQueue[0]->GetOkButton();
      
      if (pOkButton != NULL && pOkButton->ContainsPoint(&ptObjMouse))
      {
         pActiveInteraction = pOkButton;
        
         if (idMouseState == MOUSE_UP)
            m_aFeedbackQueue.RemoveAt(0); // hide feedback
      }
   }
   
   if (pLastMoveMoveable != NULL || 
      pActiveInteraction != NULL && pActiveInteraction->GetClassId() == INTERACTION_CLASS_MOVE)
   {
      // Special treatment for moveable objects

      // Note: This is a very similar (program) behaviour compared to MoveInteraction().
      // However the technical solutions are different: Here the object itself is moved;
      // there only a drag rectangle is moved (displayed) and the actual moving occurs
      // after the mouse is released.

      
      if (idMouseState == MOUSE_DOWN && pActiveInteraction != NULL)
      {
         CMoveableObjects *pMoveable = (CMoveableObjects *)pActiveInteraction;
         
         if (pMoveable->IsMoveableSource() 
            && pMoveable->ContainsPoint(&ptObjMouse) && !pMoveable->IsDisabledForPreview())
         {
            // it is really a mouse click in the moveable objects itself

            pLastMoveMoveable = pMoveable;

            aTargetPoints.RemoveAll();
            iLastTargetX = -1;
            iLastTargetY = -1;
            // TODO maybe use INTEGER_MIN_VALUE as identifier for "not defined"

            // find all visible moveable objects and their target areas' centers
            CArray<CInteractionAreaEx *, CInteractionAreaEx *> aAreas;
            HRESULT hr = pInteractionStream->FindInteractions(pEditorDoc->m_curPosMs, true, true, &aAreas);
            if (SUCCEEDED(hr))
            {
               for (int i=0; i<aAreas.GetSize(); ++i)
               {
                  if (aAreas[i]->GetClassId() == INTERACTION_CLASS_MOVE)
                  {
                     CMoveableObjects *pMoveable2 = (CMoveableObjects *)aAreas[i];
                     if (pMoveable2->IsMoveableTarget())
                     {
                        DrawSdk::TargetArea *pTarget = (DrawSdk::TargetArea *)pMoveable2->GetObjects()->GetAt(0);
                        aTargetPoints.Add(CPoint((int)pTarget->GetCenterX(), (int)pTarget->GetCenterY()));

                        if (pMoveable2->GetBelongsKey() == pMoveable->GetBelongsKey())
                        {
                           iLastTargetX = (int)pTarget->GetCenterX();
                           iLastTargetY = (int)pTarget->GetCenterY();
                        }
                     }
                  }
               }
            }
         }
      }
      else if (idMouseState == MOUSE_MOVE && pLastMoveMoveable != NULL)
      {
         float fDiffX = fObjMouseX - fObjLastX;
         float fDiffY = fObjMouseY - fObjLastY;
         pLastMoveMoveable->MoveForPreviewOnly(fDiffX, fDiffY, &aTargetPoints, iLastTargetX, iLastTargetY);
      }
      else if (idMouseState == MOUSE_UP)
      {
         pLastMoveMoveable = NULL;
      }
      
      fObjLastX = fObjMouseX;
      fObjLastY = fObjMouseY;
      
      // TODO do this always?
      // TODO limit region?
      RedrawWindow();
      
      return S_OK;
   }
   


   // normale case: not a moveable object: execute actions; change button display

   bool bButtonChange = false;
   if (idMouseState == MOUSE_DOWN)
   {
      if (pActiveInteraction != NULL)
      {
         bButtonChange = pActiveInteraction->InformMouseStatus(true, true);
         
         pInteractionStream->ExecuteAction(pActiveInteraction, MOUSE_ACTION_DOWN);
      }
      pLastMoveInteraction = NULL;
      pLastDownInteraction = pActiveInteraction;
   } 
   else if (idMouseState == MOUSE_MOVE)
   {
      if (pActiveInteraction != pLastMoveInteraction)
      {
         if (pActiveInteraction != NULL)
         {
            bool bChange1 = pActiveInteraction->InformMouseStatus(true, (nFlags & MK_LBUTTON) != 0);
            if (bChange1)
               bButtonChange = true;
            
            pInteractionStream->ExecuteAction(pActiveInteraction, MOUSE_ACTION_ENTER);
         }
         
         if (pLastMoveInteraction != NULL)
         {
            bool bChange2 = pLastMoveInteraction->InformMouseStatus(false, (nFlags & MK_LBUTTON) != 0);
            if (bChange2)
               bButtonChange = true;
            
            if (!pLastMoveInteraction->ContainsPoint(&ptObjMouse))
            {
               // only in this case issue the mouse leave event
               pInteractionStream->ExecuteAction(pLastMoveInteraction, MOUSE_ACTION_LEAVE);
            }
            // else object disappeared under the mouse: do nothing
         }
         
         pLastMoveInteraction = pActiveInteraction;
      }
   }
   else if (idMouseState == MOUSE_UP)
   {
      if (pActiveInteraction != NULL)
      {
         bButtonChange = pActiveInteraction->InformMouseStatus(true, false);
         // Note: if this is a radio button then it will be selected (deselected)
         // with this call. If it is selected all the others get deselected in 
         // CInteractionStream::ExecuteAction.
         // In any case bButtonChange is true and thus the whole window will be
         // repainted. Which is a good thing because then all the other 
         // radio buttons will also be repainted.

         
         pInteractionStream->ExecuteAction(pActiveInteraction, MOUSE_ACTION_UP);
         
         if (pActiveInteraction == pLastDownInteraction)
         {
            pInteractionStream->ExecuteAction(pActiveInteraction, MOUSE_ACTION_CLICK);
         }
      }
      
      pLastDownInteraction = NULL;
   }

   if (bButtonChange)
      RedrawWindow();
   // TODO you could only redraw the needed part; but then you also have
   // to consider the Note above.
 
   return hr;
}

void CWhiteboardView::OnContextMenu(CWnd* pWnd, CPoint point) {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
    if (!pEditorDoc || pEditorDoc->project.IsEmpty())
        return;

    bool bMenuDisplayed = false;

    CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();
    if (pInteractionStream) {
        CPoint ptMouse(point);
        // position of mouse in window
        ScreenToClient(&ptMouse);

        TranslateToObjectCoordinates(&ptMouse);

        CInteractionAreaEx *pInteraction = pInteractionStream->FindTopInteraction(
            pEditorDoc->m_curPosMs, false, &ptMouse);
        if (pInteraction) {
            if (!IsInSelectionArray(pInteraction)) {
                DeleteSelectionArray();
                pInteraction->SetSelected(true);
                m_arSelectedObjects.Add(pInteraction);
            }

            SetFontAndColor(pInteraction);
            CMenu *pPopup;
            if(pInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT){
                GraphicalObjectType goType = GOT_NOTHING;
                if(m_nGraphicalObjectKey != -1) {
                    CGraphicalObject *pGraphicalObject = (CGraphicalObject *)pInteractionStream->GetInteractionWithKey(m_nGraphicalObjectKey);
                    if (pGraphicalObject != NULL){
                        
                        CGraphicalObjectEditCtrl *pEditCtrl = pGraphicalObject->GetEditControl();
                        if (pEditCtrl != NULL) {
                            pGraphicalObject->SetGraphicalObjectText();
                        }
                    }
                }

                goType = ((CGraphicalObject*)pInteraction)->GetGraphicalObjectType();
                CMenu menu;
                menu.LoadMenu(IDR_CONTEXT_MENU_CALLOUT);
                pPopup = menu.GetSubMenu(0);//m_popupCallout.GetSubMenu(0);
                
                // "Display time ..." menu item
                pPopup->EnableMenuItem(ID_INTERACTION_PROPERTIES, !pEditorDoc->IsPreviewActive() ? MF_ENABLED : MF_GRAYED);
                // "Delete" menu item
                pPopup->EnableMenuItem(ID_EDIT_DELETE_E, !pEditorDoc->IsPreviewActive() ? MF_ENABLED : MF_GRAYED);
                /** Styles menu item
                * Visible only in case of callout, not text item
                * Disable current callout type menu item
                */
                if(goType == GOT_TEXT || goType == GOT_NOTHING) {
                    pPopup->RemoveMenu(4, MF_BYPOSITION);
                } else {
                    pPopup->EnableMenuItem(4, !pEditorDoc->IsPreviewActive()? MF_ENABLED | MF_BYPOSITION : MF_GRAYED | MF_BYPOSITION); 
                    pPopup->CheckMenuItem(ID_CALLOUT_STYLE_BASE + (UINT)goType, MF_CHECKED);
                }
                /** Settings for demo mode
                * Settings should be visible only in case of demo mode
                */

                // "Click Component Values ..." menu item
                bool bHasClickInfo = ((CGraphicalObject*)pInteraction)->HasClickInfo();

                if (bHasClickInfo) {
                    pPopup->EnableMenuItem(ID_CALLOUT_CLICK_INFO_SETTINGS,  !pEditorDoc->IsPreviewActive()? MF_ENABLED : MF_GRAYED);//to do add enable condition
                    // "Use click info template ..." menu item
                    bool bIsUserEdited = ((CGraphicalObject*)pInteraction)->IsUserEdited();
                    pPopup->EnableMenuItem(ID_CALLOUT_USE_CLICK_INFO_TEMPLATE,  !pEditorDoc->IsPreviewActive() && bIsUserEdited? MF_ENABLED : MF_GRAYED);
                } else {
                    pPopup->RemoveMenu(ID_CALLOUT_CLICK_INFO_SETTINGS, MF_BYCOMMAND);
                    pPopup->RemoveMenu(ID_CALLOUT_USE_CLICK_INFO_TEMPLATE, MF_BYCOMMAND);
                }
                // add images for callout pointer styles
                CBitmap* bmp = new CBitmap();
                CBitmap* bmp1 = new CBitmap();
                for (int i = 0 ; i < 8 ; i ++){
                    HBITMAP hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_CALLOUT_STYLE_0 + 2*i),IMAGE_BITMAP,0,0,LR_SHARED |LR_VGACOLOR |LR_LOADMAP3DCOLORS);
                    bmp = CBitmap::FromHandle(hBitmap);
                    HBITMAP hBitmap1 = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_CALLOUT_STYLE_0 + 2*i + 1),IMAGE_BITMAP,0,0,LR_SHARED |LR_VGACOLOR |LR_LOADMAP3DCOLORS);
                    bmp1 = CBitmap::FromHandle(hBitmap1);
                    pPopup->SetMenuItemBitmaps(ID_CALLOUT_STYLE_BASE + i, MF_BYCOMMAND, bmp, bmp1);   
                }

                
                pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
                bmp->DeleteObject();
                bmp1->DeleteObject();
                bMenuDisplayed = true;
            } else {
                pPopup = m_popupInteraction.GetSubMenu(0);

                // "Properties..." menu item
                pPopup->EnableMenuItem(ID_INTERACTION_PROPERTIES, !pEditorDoc->IsPreviewActive() ? MF_ENABLED : MF_GRAYED);
                // "Delete Question" menu item
                pPopup->EnableMenuItem(IDC_DELETE_QUESTION, !pEditorDoc->IsPreviewActive() && pInteraction->IsQuestionObject() ? MF_ENABLED : MF_GRAYED);
                // "Test ..." menu item
                CQuestionStream *pQuestionStream = pEditorDoc->project.GetQuestionStream();
                pPopup->EnableMenuItem(IDC_SHOW_QUESTIONAIRE_SETTINGS, !pEditorDoc->IsPreviewActive() && pQuestionStream != NULL && pQuestionStream->GetQuestionCount() > 0 ? MF_ENABLED : MF_GRAYED);
                //"Remove Demo Document objects" menu item
                pPopup->EnableMenuItem(IDC_REMOVE_ALL_DEMO_OBJECTS, 
                    !pEditorDoc->IsPreviewActive() && pEditorDoc->ShowAsScreenSimulation() && pEditorDoc->IsDemoDocument() ? MF_ENABLED : MF_GRAYED);
                // "Delete" menu item
                pPopup->EnableMenuItem(ID_EDIT_DELETE_E, pEditorDoc->IsPreviewActive() || pInteraction->IsQuestionObject() ? MF_GRAYED : MF_ENABLED);

                pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
                bMenuDisplayed = true;
            }

            if (bMenuDisplayed) {
                CMainFrameE::GetCurrentInstance()->UpdatePageObjectsView();
                RedrawWindow();
            }
        }
    }

    if (!bMenuDisplayed) {
        bool bIsInSimulationMode = pEditorDoc->IsScreenGrabbingDocument() && pEditorDoc->ShowAsScreenSimulation();
        if (bIsInSimulationMode) {
            UINT nIDCheck = ID_ZOOM_FIT;
            if (m_iMenuZoomPercent == 50) nIDCheck = ID_ZOOM_50;
            if (m_iMenuZoomPercent == 100) nIDCheck = ID_ZOOM_100;
            if (m_iMenuZoomPercent == 200) nIDCheck = ID_ZOOM_200;
            m_ZoomPopup.CheckMenuRadioItem(ID_ZOOM_50, ID_ZOOM_FIT, nIDCheck, MF_BYCOMMAND);

            UINT uiEnable = pEditorDoc->project.IsEmpty() ? MF_BYCOMMAND | MF_DISABLED : MF_BYCOMMAND | MF_ENABLED;
            for (UINT id = ID_ZOOM_50; id <= ID_ZOOM_FIT; ++id)
                m_ZoomPopup.EnableMenuItem(id, uiEnable);

            CMenu *pPopup = m_ZoomPopup.GetSubMenu(0);
            if (pPopup)
                pPopup->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

        }
    }
}

void CWhiteboardView::OnEditCut() 
{
	Cut();	
}

void CWhiteboardView::OnEditCopy() 
{
	Copy();
}

void CWhiteboardView::OnEditDelete() 
{
	Delete();
}

void CWhiteboardView::OnInteractionStackingOrder() 
{
	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
	if (!pEditorDoc || pEditorDoc->project.IsEmpty())
		return;
}

void CWhiteboardView::OnInteractionProperties() 
{
   ShowInteractionProperties();
}

void CWhiteboardView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
	if (!pEditorDoc || pEditorDoc->project.IsEmpty())
		return;

	CPoint ptMouse;
	BOOL bResult = ::GetCursorPos(&ptMouse);
	// position of mouse in window
	ScreenToClient(&ptMouse);
	
	TranslateToObjectCoordinates(&ptMouse);
	
	CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();
   if (pInteractionStream)
   {
	   CInteractionAreaEx *pInteraction = pInteractionStream->FindTopInteraction(
         pEditorDoc->m_curPosMs, false, &ptMouse);
	   if (pInteraction)
	   {
         DeleteSelectionArray();
         pInteraction->SetSelected(true);
         m_arSelectedObjects.Add(pInteraction); 
         SetFontAndColor(pInteraction);
      
	      OnInteractionProperties(); 
	   }
   }


	CView::OnLButtonDblClk(nFlags, point);
}

void CWhiteboardView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	if (bActivate) 
   {
      CMainFrameE *pMainFrame = CMainFrameE::GetCurrentInstance();
      pMainFrame->ChangeActiveView(pActivateView, pDeactiveView);
   }

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CWhiteboardView::OnDeleteQuestion() 
{
   CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
   if (!pEditorDoc || pEditorDoc->project.IsEmpty() || pEditorDoc->IsPreviewActive())
      return;

   CEditorProject *pProject = &pEditorDoc->project;

   CQuestionStream *pQuestionStream = pEditorDoc->project.GetQuestionStream();
   if (pQuestionStream != NULL)
   {
      CQuestionEx *pQuestion = pQuestionStream->FindQuestionForTime(pEditorDoc->m_curPosMs);
      if (pQuestion)
      {
         pQuestionStream->DeleteSingle(pQuestion, true, false);
         
         CArray<CQuestionnaireEx *, CQuestionnaireEx *> aQuestionnaires;
         pQuestionStream->GetQuestionnaires(&aQuestionnaires);
         CQuestionnaireEx *pQuestionnaire = aQuestionnaires[0];

         CFeedback *pNewResultFeedback = CQuestionnaireEx::CreateResultFeedback(pProject, pQuestionnaire);
         HRESULT hr = pQuestionnaire->ReplaceFeedback(pNewResultFeedback, true); // also sets rollback point

         _ASSERT(SUCCEEDED(hr));

         pEditorDoc->SetModifiedFlag();
         pEditorDoc->UpdateAllViews(NULL, HINT_CHANGE_PAGE);

         UpdateSelectionArray();
      }
   }
    if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive() && pEditorDoc->ShowAsScreenSimulation()){
	    pEditorDoc->UpdateScreenMode();
    }
}

void CWhiteboardView::OnUpdateDeleteQuestion(CCmdUI* pCmdUI) 
{
	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
   
   if (!pEditorDoc || pEditorDoc->project.IsEmpty() || pEditorDoc->IsPreviewActive())
      pCmdUI->Enable(false);
   else
   {
      bool bEnable = false;
      CQuestionStream *pQuestionStream = pEditorDoc->project.GetQuestionStream();
      if (pQuestionStream != NULL)
      {
         CQuestionEx *pQuestion = pQuestionStream->FindQuestionForTime(pEditorDoc->m_curPosMs);
         if (pQuestion)
            bEnable = true;
      }

      pCmdUI->Enable(bEnable);
   }
}

void CWhiteboardView::OnShowQuestionaireSettings() 
{
	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
    CEditorProject *pProject = &pEditorDoc->project;
   
   CQuestionStream *pQuestionStream = pProject->GetQuestionStream(true);
   if (pQuestionStream)
   {
      CArray<CQuestionnaireEx *, CQuestionnaireEx *> aQuestionnaires;
      pQuestionStream->GetQuestionnaires(&aQuestionnaires);
      
      if (aQuestionnaires.GetSize() == 0)
      {
         CQuestionnaireEx *pQuestionnaire = 
            CQuestionnaireEx::CreateDefaultQuestionnaire(pProject);
         if (pQuestionnaire)
         {
            HRESULT hr = pQuestionStream->AddQuestionnaire(pQuestionnaire);
            _ASSERT(SUCCEEDED(hr));
         }
         
         pQuestionStream->GetQuestionnaires(&aQuestionnaires);
         
         if (aQuestionnaires.GetSize() == 0)
            return;
      }
      
      CQuestionnaireEx *pQuestionnaireOld = aQuestionnaires[0];
      
      
      if (pQuestionnaireOld == NULL)
         return;
      
      CQuestionnaireSettings dialog(IDS_QUESTIONNAIRE_SETTINGS, this, 0);
      dialog.SetActivePage(m_nActivePage);

      dialog.Init(pProject, pQuestionnaireOld);
      UINT nReturn = dialog.DoModal();
      
      if (nReturn == IDOK)
      {
         CQuestionnaireEx *pQuestionnaireNew = new CQuestionnaireEx();
         dialog.SetUserEntries(pProject, pQuestionnaireNew);
         
         CQuestionStream *pQuestionStream = pProject->GetQuestionStream(true);
         
         HRESULT hr = pQuestionStream->ChangeQuestionnaire(pQuestionnaireOld, pQuestionnaireNew, false);

         if (pProject->FirstQuestionnaireIsRandomTest()) {
             pProject->SetJumpRandomQuestionActions();
         } else {
             pProject->ClearJumpRandomQuestionActions();
         }

         // replace result feedback
         CFeedback *pNewResultFeedback = 
            CQuestionnaireEx::CreateResultFeedback(pProject, pQuestionnaireNew);
         
         if (SUCCEEDED(hr))
            hr = pQuestionnaireNew->ReplaceFeedback(pNewResultFeedback, true);
         
         pEditorDoc->SetModifiedFlag();

         pEditorDoc->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
   
         _ASSERT(SUCCEEDED(hr));
         
      }
   }
}

void CWhiteboardView::OnUpdateShowQuestionnaireSettings(CCmdUI *pCmdUI)
{
	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
   
   if (!pEditorDoc || pEditorDoc->project.IsEmpty() || 
       pEditorDoc->project.GetQuestionStream() == NULL ||
       pEditorDoc->project.GetQuestionStream()->GetQuestionCount() == 0 ||
       pEditorDoc->IsPreviewActive())
      pCmdUI->Enable(false);
   else
      pCmdUI->Enable(true);


}

void CWhiteboardView::OnUpdateStackOrder(CCmdUI *pCmdUI)
{
    if(m_arSelectedObjects.GetCount() > 0)
        pCmdUI->Enable(true);
    else
        pCmdUI->Enable(false);
}

void CWhiteboardView::OnBringToFront()
{
    CArray<CInteractionAreaEx*, CInteractionAreaEx*>caSelObjects;
    for(int i = 0; i < m_arSelectedObjects.GetCount(); i++)
    {
        caSelObjects.Add(m_arSelectedObjects.GetAt(i));
    }
    OrderSelectedElements(caSelObjects);
     CEditorDoc *pEditorDoc =  
          (CEditorDoc *)CMainFrameE::GetEditorDocument();
      if (pEditorDoc && pEditorDoc->project.GetInteractionStream())
      {
          for(int i = 0; i < caSelObjects.GetCount(); i++)
          {
              UINT nHashKey = caSelObjects.GetAt(i)->GetHashKey();
              pEditorDoc->project.GetInteractionStream()->MoveToFront(nHashKey);
          }
      }

   GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
}

void CWhiteboardView::OnSendToBack()
{
     CArray<CInteractionAreaEx*, CInteractionAreaEx*>caSelObjects;
    for(int i = 0; i < m_arSelectedObjects.GetCount(); i++)
    {
        caSelObjects.Add(m_arSelectedObjects.GetAt(i));
    }
    OrderSelectedElements(caSelObjects);
   CEditorDoc *pEditorDoc =  
          (CEditorDoc *)CMainFrameE::GetEditorDocument();
      if (pEditorDoc && pEditorDoc->project.GetInteractionStream())
      {
          for(int i = caSelObjects.GetCount() - 1; i >=0 ; i--)
          {
              UINT nHashKey = caSelObjects.GetAt(i)->GetHashKey();
              pEditorDoc->project.GetInteractionStream()->MoveToBack(nHashKey);
          }
      }

   GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
}

void CWhiteboardView::OnBringForward()
{
    CEditorDoc *pEditorDoc =  
          (CEditorDoc *)CMainFrameE::GetEditorDocument();
      if (pEditorDoc && pEditorDoc->project.GetInteractionStream())
      {
          for(int i = 0; i < m_arSelectedObjects.GetCount(); i++)
          {
              UINT nHashKey = m_arSelectedObjects.GetAt(i)->GetHashKey();
              pEditorDoc->project.GetInteractionStream()->MoveOneUp(nHashKey);
          }
      }

   GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
}

void CWhiteboardView::OnSendBackward()
{
    CEditorDoc *pEditorDoc =  
          (CEditorDoc *)CMainFrameE::GetEditorDocument();
      if (pEditorDoc && pEditorDoc->project.GetInteractionStream())
      {
          for(int i = 0; i < m_arSelectedObjects.GetCount(); i++)
          {
              UINT nHashKey = m_arSelectedObjects.GetAt(i)->GetHashKey();
              pEditorDoc->project.GetInteractionStream()->MoveOneDown(nHashKey);
          }
      }

   GetDocument()->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
}

//PRIVATE
void CWhiteboardView::OrderSelectedElements(CArray<CInteractionAreaEx*, CInteractionAreaEx*>&caSelectedElements)
{
    CInteractionAreaEx* pIax;
    bool bChange = true;
    while(bChange == true)
    {
        bChange = false;
        for(int i = 0; i < caSelectedElements.GetCount() - 1; i++)
        {
            if(caSelectedElements.GetAt(i)->GetStackingOrder() > caSelectedElements.GetAt(i+1)->GetStackingOrder())
            {
                pIax = caSelectedElements.GetAt(i);
                caSelectedElements.GetAt(i) = caSelectedElements.GetAt(i+1);
                caSelectedElements.GetAt(i+1) = pIax;
                bChange = true;
            }
        }
    }
}

// Toolbar commands

void CWhiteboardView::OnFillColor()
{
   ChangeFontAndColor(FILL_COLOR);
}

void CWhiteboardView::OnUpdateFillColor(CCmdUI *pCmdUI)
{
    // update color below symbol
    CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmdUI->m_pOther;
    if (pToolBar) {	
        CXTPControlPopupColor* pPopup = (CXTPControlPopupColor*)pToolBar->GetControl(pCmdUI->m_nIndex);
        if (pPopup)
            pPopup->SetColor(m_clrFill);
    }

    // enable or disable button
    BOOL bEnable = FALSE;

    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
    if (pEditorDoc && 
        !pEditorDoc->project.IsEmpty() && 
        !pEditorDoc->IsPreviewActive() &&
        ChangeableObjectsAreaSelected()) {
            bEnable = TRUE;
    }

    pCmdUI->Enable(bEnable);
}

void CWhiteboardView::OnSelectFillColor(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
		
	CXTPControlColorSelector* pControl = (CXTPControlColorSelector*)tagNMCONTROL->pControl;
	m_clrFill = pControl->GetColor();

	OnFillColor();

	*pResult = 1;
}

void CWhiteboardView::OnUpdateSelectFillColor(CCmdUI* pCmd)
{
	pCmd->Enable(TRUE);
}

void CWhiteboardView::OnMoreFillColors()
{
	CColorDialog cd(m_clrFill);
	if (cd.DoModal())
	{
		m_clrFill = cd.GetColor();
		OnFillColor();
	}
}

void CWhiteboardView::OnLineColor()
{
   ChangeFontAndColor(LINE_COLOR);
}

void CWhiteboardView::OnUpdateLineColor(CCmdUI *pCmdUI)
{
    // update color below symbol
    CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmdUI->m_pOther;
    if (pToolBar) {	
        CXTPControlPopupColor* pPopup = (CXTPControlPopupColor*)pToolBar->GetControl(pCmdUI->m_nIndex);
        if (pPopup)
            pPopup->SetColor(m_clrLine);
    }

    // enable or disable button
    BOOL bEnable = FALSE;

    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
    if (pEditorDoc && 
        !pEditorDoc->project.IsEmpty() && 
        !pEditorDoc->IsPreviewActive() && 
        ChangeableObjectsAreaSelected()) {
            bEnable = TRUE;
    }

    pCmdUI->Enable(bEnable);
}

void CWhiteboardView::OnSelectLineColor(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
		
	CXTPControlColorSelector* pControl = (CXTPControlColorSelector*)tagNMCONTROL->pControl;
	m_clrLine = pControl->GetColor();

	OnLineColor();

	*pResult = 1;
}

void CWhiteboardView::OnUpdateSelectLineColor(CCmdUI* pCmd)
{
	pCmd->Enable(TRUE);
}

void CWhiteboardView::OnMoreLineColors()
{
	CColorDialog cd(m_clrLine);
	if (cd.DoModal())
	{
		m_clrLine = cd.GetColor();
		OnLineColor();
	}
}

void CWhiteboardView::OnTextColor()
{
   ChangeFontAndColor(TEXT_COLOR);
}

void CWhiteboardView::OnUpdateTextColor(CCmdUI *pCmdUI)
{
   // update color below symbol
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmdUI->m_pOther;
	if (pToolBar)
	{	
		CXTPControlPopupColor* pPopup = (CXTPControlPopupColor*)pToolBar->GetControl(pCmdUI->m_nIndex);
      if (pPopup)
			pPopup->SetColor(m_clrText);
	}

   // enable or disable button
   BOOL bEnable = FALSE;

	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

   CInteractionStream *pInteractionStream = 
      pEditorDoc->project.GetInteractionStream();

	if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive())
   {
      if (m_arSelectedObjects.GetSize() > 0)
		   bEnable = TRUE;
   }

   pCmdUI->Enable(bEnable);
}

void CWhiteboardView::OnSelectTextColor(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
		
	CXTPControlColorSelector* pControl = (CXTPControlColorSelector*)tagNMCONTROL->pControl;
	m_clrText = pControl->GetColor();
	OnTextColor();
	*pResult = 1;
}

void CWhiteboardView::OnUpdateSelectTextColor(CCmdUI* pCmd)
{
	pCmd->Enable(TRUE);
}

void CWhiteboardView::OnMoreTextColors()
{
	CColorDialog cd(m_clrText);
	if (cd.DoModal())
	{
		m_clrText = cd.GetColor();
		OnTextColor();
	}
}

void CWhiteboardView::OnLineWidth()
{
   ChangeFontAndColor(LINE_WIDTH);
}

void CWhiteboardView::OnUpdateLineWidth(CCmdUI *pCmdUI)
{
   
   // update color below symbol
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmdUI->m_pOther;
   
	if (pToolBar)
	{	
		CXTPControlPopup* pPopup = (CXTPControlPopup*)pToolBar->GetControl(pCmdUI->m_nIndex);

      CXTPPopupBar *pCommandBar = NULL;
      if (pPopup)
         pCommandBar = (CXTPPopupBar *)pPopup->GetCommandBar();

      CControlLineWidthSelector *pControl = NULL;
      if (pCommandBar)
         pControl = 
            (CControlLineWidthSelector *)pCommandBar->GetControls()->FindControl(ID_SELECTOR_LINE_WIDTH_E);

      if (pControl)
			pControl->SetLineWidth(m_iLineWidth);
	}

   // enable or disable button
   BOOL bEnable = FALSE;

	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

   CInteractionStream *pInteractionStream = 
      pEditorDoc->project.GetInteractionStream();

	if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive())
   {
      if (ChangeableObjectsAreaSelected())
		   bEnable = TRUE;
   }

   pCmdUI->Enable(bEnable);
}

void CWhiteboardView::OnSelectLineWidth(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
		
	CControlLineWidthSelector* pControl = (CControlLineWidthSelector*)tagNMCONTROL->pControl;
	m_iLineWidth = (int)pControl->GetLineWidth();

	OnLineWidth();

	*pResult = 1;
}

void CWhiteboardView::OnUpdateSelectLineWidth(CCmdUI* pCmd)
{
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmd->m_pOther;
	if (pToolBar)
	{	
	}
	pCmd->Enable(TRUE);
}
   
void CWhiteboardView::OnLineStyle()
{
   ChangeFontAndColor(LINE_STYLE);
}

void CWhiteboardView::OnSelectLineStyle(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
		
	CControlLineStyleSelector* pControl = (CControlLineStyleSelector*)tagNMCONTROL->pControl;
	m_iLineStyle = pControl->GetObjLineStyle();

	OnLineStyle();

	*pResult = 1;
}

void CWhiteboardView::OnUpdateLineStyle(CCmdUI *pCmdUI)
{
   // update color below symbol
   
   CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmdUI->m_pOther;
   
   if (pToolBar)
   {	
      CXTPControlPopup* pPopup = (CXTPControlPopup*)pToolBar->GetControl(pCmdUI->m_nIndex);
      
      CXTPPopupBar *pCommandBar = NULL;
      if (pPopup)
         pCommandBar = (CXTPPopupBar *)pPopup->GetCommandBar();
      
      CControlLineStyleSelector *pControl = NULL;
      if (pCommandBar)
         pControl = 
            (CControlLineStyleSelector *)pCommandBar->GetControls()->FindControl(ID_SELECTOR_LINE_STYLE_E);
      
      if (pControl)
         pControl->SetObjLineStyle(m_iLineStyle);
   }
   
   // enable or disable button
   BOOL bEnable = FALSE;

	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

   CInteractionStream *pInteractionStream = 
      pEditorDoc->project.GetInteractionStream();

	if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive())
   {
      if (ChangeableObjectsAreaSelected())
		   bEnable = TRUE;
   }

   pCmdUI->Enable(bEnable);
}


void CWhiteboardView::OnUpdateSelectLineStyle(CCmdUI* pCmdUI)
{
   BOOL bEnable = FALSE;

	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

   CInteractionStream *pInteractionStream = 
      pEditorDoc->project.GetInteractionStream();

	if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive())
   {
      if (m_arSelectedObjects.GetSize() > 0)
		   bEnable = TRUE;
   }

   pCmdUI->Enable(bEnable);
}


void CWhiteboardView::OnFontFamily(NMHDR* pNMHDR, LRESULT* pResult)/*()*/
{
  // CXTPCommandBars* pCommandBars = CMainFrameE::GetCurrentInstance()->GetCommandBars();
   //CXTPToolBar *pToolbar = pCommandBars->GetToolBar(IDR_EDIT);
   
   
   /*CXTPControlFontComboBox* pFontCombo = 
      (CXTPControlFontComboBox*)pToolbar->GetControls()->FindControl(IDC_FONT_FAMILY);
   
   if (pFontCombo->GetType() == xtpControlComboBox)
   {
		_tcscpy(m_CharFormat.szFaceName, pFontCombo->GetText());
      ChangeFontAndColor(FAMILY);
   }	*/
   CMainFrameE *pMainFrame = CMainFrameE::GetCurrentInstance();

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
            _tcscpy(m_CharFormat.szFaceName, pControl->GetText());
            ChangeFontAndColor(FAMILY);
         }
      }
   }  
}

void CWhiteboardView::OnUpdateFontFamily(CCmdUI* pCmdUI)
{
   // update font entry
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmdUI->m_pOther;
	if (pToolBar)
	{	
		CXTPControlFontComboBox* pFontCombo = 
         (CXTPControlFontComboBox*)pToolBar->GetControls()->FindControl(IDC_FONT_FAMILY);

		if (pFontCombo->GetType() == xtpControlComboBox)
		{
			pFontCombo->SetCharFormat(m_CharFormat);
		}		
	}

   // Enable / disable button
   BOOL bEnable = FALSE;

	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

   CInteractionStream *pInteractionStream = 
      pEditorDoc->project.GetInteractionStream();

	if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive())
   {
      if (m_arSelectedObjects.GetSize() > 0)
		   bEnable = TRUE;
   }

   pCmdUI->Enable(bEnable);
}

void CWhiteboardView::OnFontSize(NMHDR* pNMHDR, LRESULT* pResult)/*()*/
{
   /*CXTPCommandBars* pCommandBars = CMainFrameE::GetCurrentInstance()->GetCommandBars();
   CXTPToolBar *pToolbar = pCommandBars->GetToolBar(IDR_EDIT);
   
   
   CXTPControlComboBox* pFontCombo = 
      (CXTPControlComboBox*)pToolbar->GetControls()->FindControl(IDC_FONT_SIZE);
   
   if (pFontCombo->GetType() == xtpControlComboBox)
   {
		CString csString = pFontCombo->GetText();
		m_CharFormat.yHeight = _ttoi(csString) * 20;
      ChangeFontAndColor(FONTSIZE);
   }	*/

   CMainFrameE *pMainFrame = CMainFrameE::GetCurrentInstance();

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
            m_CharFormat.yHeight = _ttoi(csString) * 20;
            ChangeFontAndColor(FONTSIZE);
         }
      }
   }   
}

void CWhiteboardView::OnFontSizeIncrease(NMHDR* pNMHDR, LRESULT* pResult)
{
   CMainFrameE *pMainFrame = CMainFrameE::GetCurrentInstance();

   if (pMainFrame == NULL)
      return;

   CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
   if (pCommandBars != NULL)
   {
      NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

      CXTPControlButton* pControl = (CXTPControlSizeComboBox*)tagNMCONTROL->pControl;
      if(pControl)
      { 
         CXTPRibbonGroup *pGroup = pControl->GetRibbonGroup();
         if(pGroup)
         {
            CXTPControlSizeComboBox* pControl = (CXTPControlSizeComboBox*)pGroup->FindControl(IDC_FONT_SIZE);
        
               if (pControl->GetType() == xtpControlComboBox)
               {
                  pControl->UpdatePopupSelection();
                  int iCurSel = pControl->GetCurSel();
                  if(iCurSel + 1 < 22)//pControl->GetCount())
                  {
                     pControl->SetCurSel(iCurSel + 1);
                  }
                  CString csString = pControl->GetText();
                  m_CharFormat.yHeight = _ttoi(csString) * 20;
                  ChangeFontAndColor(FONTSIZE);
               }
         }
      }
   }   
  
}

void CWhiteboardView::OnFontSizeDecrease(NMHDR* pNMHDR, LRESULT* pResult)
{

   CMainFrameE *pMainFrame = CMainFrameE::GetCurrentInstance();

   if (pMainFrame == NULL)
      return;

   CXTPCommandBars* pCommandBars = pMainFrame->GetCommandBars();
   if (pCommandBars != NULL)
   {
      NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

      CXTPControlButton* pControl = (CXTPControlSizeComboBox*)tagNMCONTROL->pControl;
      if(pControl)
      { 
         CXTPRibbonGroup *pGroup = pControl->GetRibbonGroup();
         if(pGroup)
         {
            CXTPControlSizeComboBox* pControl = (CXTPControlSizeComboBox*)pGroup->FindControl(IDC_FONT_SIZE);
        
               if (pControl->GetType() == xtpControlComboBox)
               {
                  pControl->UpdatePopupSelection();
                  int iCurSel = pControl->GetCurSel();
                  if(iCurSel - 1 >= 0)//pControl->GetCount())
                  {
                     pControl->SetCurSel(iCurSel - 1);
                  }
                  CString csString = pControl->GetText();
                  m_CharFormat.yHeight = _ttoi(csString) * 20;
                  ChangeFontAndColor(FONTSIZE);
               }
         }
      }
   }   
   
}

void CWhiteboardView::OnAlignLeft() {
    m_iAlignment = 0;
    ChangeFontAndColor(ALIGN_LEFT);
}

void CWhiteboardView::OnAlignCenter() {
    m_iAlignment = 1;
    ChangeFontAndColor(ALIGN_CENTER);
}
void CWhiteboardView::OnAlignRight() {
    m_iAlignment = 2;
    ChangeFontAndColor(ALIGN_RIGHT);
}

void CWhiteboardView::OnUpdateAlignLeft(CCmdUI* pCmdUI) {
    BOOL bEnable = FALSE;
    BOOL bCheck = FALSE;

    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

    CInteractionStream *pInteractionStream = 
        pEditorDoc->project.GetInteractionStream();

    if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive())
    {
        /*CInteractionAreaEx* pInt = NULL;
        if (m_arSelectedObjects.GetSize() > 0)
            CInteractionAreaEx* pInt = m_arSelectedObjects.GetAt(0);*/
        //if (pInt !=NULL && pInt->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT) {
        if (m_nGraphicalObjectKey != -1){
            bEnable = TRUE;
            if (m_iAlignment == 0)
                bCheck = TRUE;
        }
        /*if (m_arSelectedObjects.GetSize() == 1 && m_arSelectedObjects.GetAt(0)->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT)
        bEnable = TRUE;*/
    }

    pCmdUI->Enable(bEnable);
    pCmdUI->SetCheck(bCheck);
}

void CWhiteboardView::OnUpdateAlignCenter(CCmdUI* pCmdUI) {
    BOOL bEnable = FALSE;
    BOOL bCheck = FALSE;

    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

    CInteractionStream *pInteractionStream = 
        pEditorDoc->project.GetInteractionStream();

    if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive())
    {
       /* CInteractionAreaEx* pInt = NULL;
        if (m_arSelectedObjects.GetSize() > 0)
            CInteractionAreaEx* pInt = m_arSelectedObjects.GetAt(0);
        if (pInt !=NULL && pInt->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT){*/
        if (m_nGraphicalObjectKey != -1){
            bEnable = TRUE;
            if (m_iAlignment == 1)
                bCheck = TRUE;
        }
        /*if (m_arSelectedObjects.GetSize() == 1 && m_arSelectedObjects.GetAt(0)->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT)
        bEnable = TRUE;*/
    }

    pCmdUI->Enable(bEnable);
    pCmdUI->SetCheck(bCheck);
}

void CWhiteboardView::OnUpdateAlignRight(CCmdUI* pCmdUI) {
    BOOL bEnable = FALSE;
    BOOL bCheck = FALSE;

    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

    CInteractionStream *pInteractionStream = 
        pEditorDoc->project.GetInteractionStream();

    if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive())
    {
        /*CInteractionAreaEx* pInt = NULL;
        if (m_arSelectedObjects.GetSize() > 0)
            CInteractionAreaEx* pInt = m_arSelectedObjects.GetAt(0);
        if (pInt !=NULL && pInt->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT){*/
        if (m_nGraphicalObjectKey != -1){
            bEnable = TRUE;
            if (m_iAlignment == 2)
                bCheck = TRUE;
        }
        /*if (m_arSelectedObjects.GetSize() == 1 && m_arSelectedObjects.GetAt(0)->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT)
        bEnable = TRUE;*/
    }

    pCmdUI->Enable(bEnable);
    pCmdUI->SetCheck(bCheck);
}


void CWhiteboardView::OnUpdateFontSize(CCmdUI* pCmdUI)
{
	CXTPCommandBar* pToolBar = (CXTPToolBar*)pCmdUI->m_pOther;
	if (pToolBar)
	{	
		/*CXTPControlSizeComboBox* pSizeCombo = 
         (CXTPControlSizeComboBox*)pToolBar->GetControl(pCmdUI->m_nIndex);

		if (pSizeCombo->GetType() == xtpControlComboBox)
		{
			pSizeCombo->SetTwipSize( (m_CharFormat.dwMask & CFM_SIZE) ? m_CharFormat.yHeight : -1);
		}	*/	
      CXTPControlSizeComboBox* pSizeCombo = (CXTPControlSizeComboBox*)pToolBar->GetControls()->FindControl(pCmdUI->m_nID);//GetAt(pCmdUI->m_nIndex);
      
      if(!pSizeCombo->GetRibbonGroup())
      {
         if (pSizeCombo->GetType() == xtpControlComboBox)
         {
            pSizeCombo->SetTwipSize( (m_CharFormat.dwMask & CFM_SIZE) ? m_CharFormat.yHeight : -1);
         }	
      }
      else
      {
         CString strSize;
         strSize.Format(_T("%d"), (m_CharFormat.yHeight/20));
         pSizeCombo->SetEditText(strSize);
      }
	}
   

   // Enable / disable button
   BOOL bEnable = FALSE;

	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

   CInteractionStream *pInteractionStream = 
      pEditorDoc->project.GetInteractionStream();

	if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive())
   {
      if (m_arSelectedObjects.GetSize() > 0)
		   bEnable = TRUE;
   }

   pCmdUI->Enable(bEnable);
}

void CWhiteboardView::OnFontWeight()
{
   m_bFontBold = !m_bFontBold;
   ChangeFontAndColor(FONTWEIGHT);
}

void CWhiteboardView::OnUpdateFontWeight(CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   BOOL bCheck = 0;

	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

   CInteractionStream *pInteractionStream = 
      pEditorDoc->project.GetInteractionStream();

	if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive())
   {
      if (m_arSelectedObjects.GetSize() > 0)
		   bEnable = TRUE;
      if (m_arSelectedObjects.GetSize() > 0)
         bCheck = m_bFontBold;
   }


	pCmdUI->SetCheck(bCheck);
   pCmdUI->Enable(bEnable);
}

void CWhiteboardView::OnFontSlant()
{
   m_bFontItalic = !m_bFontItalic;
   ChangeFontAndColor(FONTSLANT);
}

void CWhiteboardView::OnUpdateFontSlant(CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   BOOL bCheck = 0;

	CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

   CInteractionStream *pInteractionStream = 
      pEditorDoc->project.GetInteractionStream();

	if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive())
   {
      if (m_arSelectedObjects.GetSize() > 0)
		   bEnable = TRUE;
      if (m_arSelectedObjects.GetSize() > 0)
         bCheck = m_bFontItalic;
   }

	pCmdUI->SetCheck(bCheck);
   pCmdUI->Enable(bEnable);
}

// private
void CWhiteboardView::ResizeChildWindows(CWnd *pWndSpecific)
{
   m_bZoomValid = false;
   UpdateZoom();

   CPoint ptOffset(m_rcLastPage.left, m_rcLastPage.top);
   
   POSITION pos = m_mapOrigSizes.GetStartPosition();
   while (pos != NULL)
   {
      CWnd *pWnd = NULL;
      CRect rcOrig;
      
      m_mapOrigSizes.GetNextAssoc(pos, pWnd, rcOrig);

      if (pWndSpecific == NULL || pWnd == pWndSpecific)
      {
         CRect rcScaled = rcOrig;
         ScaleAndOffsetRect(rcScaled, &ptOffset, m_dLastZoom);
         
         pWnd->SetWindowPos(NULL, rcScaled.left, rcScaled.top, rcScaled.Width(), rcScaled.Height(), SWP_NOOWNERZORDER);
         
         CFont *newFont = new CFont();
         int iFontHeight = (int)(FONT_FACTOR * rcOrig.Height() * m_dLastZoom * 10);
         newFont->CreatePointFont(iFontHeight, _T("Arial"));
         ((CExtEdit *)pWnd)->SetExtFont(newFont);
      }
   }
}

// private
bool CWhiteboardView::UpdateZoom()
{
   if (!m_bZoomValid)
   {
      CRect rcClient;
      GetClientRect(rcClient);
      CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

      if (pEditorDoc != NULL && !pEditorDoc->project.IsEmpty())
      {
         // remember values for later mouse movements and clicks
         pEditorDoc->project.CalculateWhiteboardArea(&rcClient, &m_rcLastPage, &m_dLastZoom, m_iMenuZoomPercent);

         m_bZoomValid = true;
      }
   }

   return m_bZoomValid;
}

void CWhiteboardView::ShowPageView()
{
   CMainFrameE::GetCurrentInstance()->ShowPane(CMainFrameE::XTP_PAGEOBJECTS_VIEW);
   CMainFrameE::GetCurrentInstance()->UpdatePageObjectsView();
}

void CWhiteboardView::OnUpdateFontList(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(TRUE);
}
void CWhiteboardView::OnUpdateFontSizeIncrease(CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

   if (pEditorDoc != NULL && !pEditorDoc->project.IsEmpty())
   {
      if (m_arSelectedObjects.GetSize() > 0)
      {
         bEnable = TRUE;
      }
   }
   pCmdUI->Enable(bEnable);
}
void CWhiteboardView::OnUpdateFontSizeDecrease(CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

   if (pEditorDoc != NULL && !pEditorDoc->project.IsEmpty())
   {
      if (m_arSelectedObjects.GetSize() > 0)
      {
         bEnable = TRUE;
      }
   }
   pCmdUI->Enable(bEnable);
}

void CWhiteboardView::OnDeleteAllDemoDocumentObjects() {
    DeleteAllDemoDocumentObjects();
}

void CWhiteboardView::OnClickInfoSettings() {
    ShowInteractionProperties(true);
}

void CWhiteboardView::OnClickUseInfoTemplate() {
    CString csInfoCaption;
    csInfoCaption.LoadString(IDS_CALLOUT_CLICK_INFO_TEMPLATE_CAPTION);
    CString csInfoText;
    csInfoText.LoadString(IDS_CALLOUT_USE_TEMPLATE_QUESTION);
    if (MessageBox(csInfoText, csInfoCaption, MB_YESNO | MB_ICONQUESTION) == IDYES) {
        CInteractionAreaEx *pActiveInteraction = m_arSelectedObjects[0];
        if (pActiveInteraction != NULL && pActiveInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT) {
            CGraphicalObject* pGraphicalObject = (CGraphicalObject*) pActiveInteraction;
            pGraphicalObject->UpdateLcCalloutText();
            pGraphicalObject->SetUserEdited(false);
        }
    }
}

void CWhiteboardView::OnChangeCalloutStyle(UINT nID) {
    GraphicalObjectType goType = (GraphicalObjectType)(nID - ID_CALLOUT_STYLE_BASE);
    ChangeCaloutStyle(goType);
}

void CWhiteboardView::ChangeCaloutStyle(GraphicalObjectType goType) {
    UpdateSelectionArray();
    if (m_arSelectedObjects.GetSize() == 1 && m_arSelectedObjects.GetAt(0)->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT) {
        CInteractionAreaEx* pInteraction = m_arSelectedObjects.GetAt(0);
        CInteractionAreaEx* pCopy = pInteraction->Copy(true);
        CGraphicalObject* pGraphicalObject = (CGraphicalObject*)pCopy;
        
        HRESULT hr = pGraphicalObject->SetGraphicalObjectType(goType);

        if (SUCCEEDED(hr)) {
            CEditorDoc* pDoc = (CEditorDoc *) GetDocument();
            CInteractionStream *pInteractionStream = pDoc->project.GetInteractionStream(true);
            hr = pInteractionStream->ModifyArea(pInteraction->GetHashKey(), pCopy);
            pCopy->SetSelected(true);
            m_arSelectedObjects[0] = pCopy;
            pDoc->SetModifiedFlag();
        }
        RedrawWindow();
    }
}


void CWhiteboardView::OnZoom50() {
    m_dLastZoom = 0.5;
    m_iMenuZoomPercent = 50;
    m_bZoomValid = true;
    CreateWbPageBitmap();
    RedrawWindow();
}

void CWhiteboardView::OnZoom100() {
    m_dLastZoom = 1.0;
    m_iMenuZoomPercent = 100;
    m_bZoomValid = true;
    CreateWbPageBitmap();
    RedrawWindow();
}

void CWhiteboardView::OnZoom200() {
    m_dLastZoom = 2.0;
    m_iMenuZoomPercent = 200;
    m_bZoomValid = true;
    CreateWbPageBitmap();
    RedrawWindow();
}

void CWhiteboardView::OnZoomFit() {
    m_bZoomValid = false;
    m_iMenuZoomPercent = 0;
    // m_dLastZoom should be not eqaul 0.5, 1.0 or 2.0
    m_dLastZoom = 0.9;
    UpdateZoom();
    CreateWbPageBitmap();
    RedrawWindow();
}

void CWhiteboardView::OnUpdateVideoZoom(CCmdUI *pCmdUI) {
	CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
   bool bEnable = false;

   if (pDoc && !pDoc->project.IsEmpty())
      bEnable = true;

   int check = 0;
   switch (pCmdUI->m_nID) {
       case ID_ZOOM_50:
          if (m_iMenuZoomPercent == 50) 
              check = 1; 
          break;
       case ID_ZOOM_100:
          if (m_iMenuZoomPercent == 100) 
              check = 1; 
          break;
       case ID_ZOOM_200:
          if (m_iMenuZoomPercent == 200) 
              check = 1; 
          break;
       case ID_ZOOM_FIT:
          if (m_dLastZoom != 50 && m_dLastZoom != 100 && m_dLastZoom != 100) 
              check = 1; 
          break;
   }

   pCmdUI->SetRadio(check);
   pCmdUI->Enable(bEnable);
}		

void CWhiteboardView::InsertDefaultDemoModeObjects() {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
    if (!pEditorDoc || pEditorDoc->project.IsEmpty()) {
        return;
    }

    // are there any interactions?
    DeleteSelectionArray();
    pEditorDoc->project.ConvertToDemoDocument();

    CMainFrameE * pMainFrame = pEditorDoc->GetMainFrame();
    if (pMainFrame!=NULL) {
        CStreamView * pStreamView = pMainFrame->GetStreamView();
        if (pStreamView!=NULL) {
            pStreamView->UpdateStreamVisibilities(pEditorDoc);
            pStreamView->RedrawStreamCtrl(NULL);
        }
    }

    pEditorDoc->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
}

bool CWhiteboardView::DeleteAllDemoDocumentObjects() {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
    ASSERT(pEditorDoc);
    ASSERT(!pEditorDoc->project.IsEmpty());
    ASSERT(pEditorDoc->GetWhiteboardView());
    if (!pEditorDoc
        || pEditorDoc->project.IsEmpty() 
        || !pEditorDoc->GetWhiteboardView() 
        || !pEditorDoc->ShowAsScreenSimulation()) {
        return false;
    }
    CString question;
    question.LoadString(IDS_QUESTION_REMOVE_ALL_DEMO_OBJECTS);
    if (MessageBox(question, NULL, MB_OKCANCEL | MB_ICONQUESTION | MB_TOPMOST) != IDOK) {
        return false;
    }

    /*CMarkStream *pMarkStream = pEditorDoc->project.GetMarkStream();
    if (pMarkStream && pEditorDoc->ShowAsScreenSimulation()) {
        pMarkStream->RemoveAllEndOfPageStopMarks();
    }*/

    CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream();
    if (pInteractionStream) {
        pInteractionStream->DeleteAllDemoDocumentObjects();
    }

    pEditorDoc->UpdateAllViews(NULL, HINT_CHANGE_STRUCTURE);

    if (pEditorDoc && !pEditorDoc->project.IsEmpty() && !pEditorDoc->IsPreviewActive() && pEditorDoc->ShowAsScreenSimulation()){
        pEditorDoc->UpdateScreenMode();
    }
    pEditorDoc->SetModifiedFlag();
    return true;
}

bool CWhiteboardView::InsertDemoObject(int iTimestamp) {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
    ASSERT(pEditorDoc);
    ASSERT(!pEditorDoc->project.IsEmpty());
    ASSERT(pEditorDoc->GetWhiteboardView());
    if (!pEditorDoc
        || pEditorDoc->project.IsEmpty() 
        || !pEditorDoc->GetWhiteboardView() 
        || !pEditorDoc->ShowAsScreenSimulation()) {
        return false;
    }

   
    CPoint ptCenter(50,50);// = rcPage.CenterPoint();
    CSize szCallout(GOT_MIN_WIDTH, GOT_MIN_HEIGHT);
    //ptCenter.Offset(- GOT_MIN_WIDTH / 2 , - GOT_MIN_HEIGHT / 2);

    CRect rcCallout(ptCenter, szCallout);
    TranslateFromObjectCoordinates(rcCallout);
    CGraphicalObject* pGO = new CGraphicalObject();
    pEditorDoc->project.InsertDemoDocumentObject(rcCallout, GOT_CALLOUT_TOP_LEFT, _T(""), iTimestamp, true, NULL, pGO);
    DeleteSelectionArray();
    pGO->SetSelected(true);
    m_arSelectedObjects.Add(pGO);
    SetFontAndColor(pGO);
    return true;
}

bool CWhiteboardView::InsertDemoGroup(CRect rcCallout) {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();
    ASSERT(pEditorDoc);
    ASSERT(!pEditorDoc->project.IsEmpty());
    ASSERT(pEditorDoc->GetWhiteboardView());
    if (!pEditorDoc
        || pEditorDoc->project.IsEmpty() 
        || !pEditorDoc->GetWhiteboardView() 
        || !pEditorDoc->ShowAsScreenSimulation()) {
        return false;
    }

    int iTimestamp = pEditorDoc->GetCurrentPreviewPos();

    CMarkStream *pMarkStream = pEditorDoc->project.GetMarkStream();

    bool bIsStopAtTimestamp = false;
    if (pMarkStream != NULL) {
        if (pMarkStream->IsExactStopAt(iTimestamp) || pMarkStream->IsExactStopAt(iTimestamp + 1))
            bIsStopAtTimestamp = true;
    }

    //bool bIsStopAtTimestamp = pEditorDoc->project.GetMarkStream()->GetFirstMarkInRange(iTimestamp - 1, 2);// IsExactStopAt(iTimestamp + 1);

    if (bIsStopAtTimestamp) {
        CString csMess;
        csMess.LoadString(IDS_CREATE_DEMO_OBJ_NOT_POSSIBLE);
        CString csCaption;
        csCaption.LoadString(IDS_INFORMATION);
        //AfxMessageBox(csMess);
        MessageBox(csMess, csCaption, MB_OK | MB_ICONINFORMATION);
        CMainFrameE::GetCurrentInstance()->ResetCurrentIteractionId();
        return false;
    }

    //pEditorDoc->project.InsertDemoDocumentStopmark(iTimestamp);
    
    GraphicalObjectType currentType = GOT_NOTHING;
    switch(pEditorDoc->project.GetActiveDemoObjectType()){
        case ID_DEMOOBJ_TOP_LEFT:
            currentType = GOT_CALLOUT_TOP_LEFT;
            break;
        case ID_DEMOOBJ_TOP_RIGHT:
            currentType = GOT_CALLOUT_TOP_RIGHT;
            break;
        case ID_DEMOOBJ_LEFT_TOP:
            currentType = GOT_CALLOUT_LEFT_TOP;
            break;
        case ID_DEMOOBJ_LEFT_BOTTOM:
            currentType = GOT_CALLOUT_LEFT_BOTTOM;
            break;
        case ID_DEMOOBJ_RIGHT_TOP:
            currentType = GOT_CALLOUT_RIGHT_TOP;
            break;
        case ID_DEMOOBJ_RIGHT_BOTTOM:
            currentType = GOT_CALLOUT_RIGHT_BOTTOM;
            break;
        case ID_DEMOOBJ_BOTTOM_LEFT:
            currentType = GOT_CALLOUT_BOTTOM_LEFT;
            break;
        case ID_DEMOOBJ_BOTTOM_RIGHT:
            currentType = GOT_CALLOUT_BOTTOM_RIGHT;
            break;
        default:
            currentType = GOT_CALLOUT_TOP_LEFT;
            break;
    }

    CGraphicalObject* pGO = new CGraphicalObject();
    pEditorDoc->project.InsertDemoDocumentObject(rcCallout, currentType, _T(""), iTimestamp, true, NULL, pGO);
    DeleteSelectionArray();
    pGO->SetSelected(true);
    m_arSelectedObjects.Add(pGO);
    SetFontAndColor(pGO);
    CMainFrameE::GetCurrentInstance()->ResetCurrentIteractionId();
    return true;

}

void CWhiteboardView::InsertGraphicalObject(CRect &rcRange, GraphicalObjectType iGraphicalObjectType, CString csText, bool bDemoMode) {
    CEditorDoc* pEditorDoc = (CEditorDoc *) GetDocument();

    if (!pEditorDoc || pEditorDoc->project.IsEmpty())
        return;

    CInteractionStream *pInteractionStream = pEditorDoc->project.GetInteractionStream(false);
    if (pInteractionStream == NULL) {
        pInteractionStream = pEditorDoc->project.GetInteractionStream(true);
    }
    CGraphicalObject *pGraphicalObject = new CGraphicalObject();
    if(m_nInteractionType == ID_GRAPHICALOBJECT_TEXT) {
        iGraphicalObjectType = GOT_TEXT;
    }
    else {
        if (iGraphicalObjectType == GOT_NOTHING) {
            switch(pEditorDoc->project.GetActiveCalloutType()) {
            case ID_CALLOUT_TOP_LEFT: 
                iGraphicalObjectType = GOT_CALLOUT_TOP_LEFT;
                break;
            case ID_CALLOUT_TOP_RIGHT: 
                iGraphicalObjectType = GOT_CALLOUT_TOP_RIGHT;
                break;
            case ID_CALLOUT_BOTTOM_LEFT: 
                iGraphicalObjectType = GOT_CALLOUT_BOTTOM_LEFT;
                break;
            case ID_CALLOUT_BOTTOM_RIGHT: 
                iGraphicalObjectType = GOT_CALLOUT_BOTTOM_RIGHT;
                break;
            case ID_CALLOUT_LEFT_BOTTOM: 
                iGraphicalObjectType = GOT_CALLOUT_LEFT_BOTTOM;
                break;
            case ID_CALLOUT_LEFT_TOP: 
                iGraphicalObjectType = GOT_CALLOUT_LEFT_TOP;
                break;
            case ID_CALLOUT_RIGHT_BOTTOM: 
                iGraphicalObjectType = GOT_CALLOUT_RIGHT_BOTTOM;
                break;
            case ID_CALLOUT_RIGHT_TOP: 
                iGraphicalObjectType = GOT_CALLOUT_RIGHT_TOP;
                break;
            }
        }
    }
    pGraphicalObject->Init(iGraphicalObjectType, &pEditorDoc->project, rcRange, pEditorDoc->m_curPosMs, csText, bDemoMode);
    HRESULT hrInsert = pInteractionStream->InsertSingle(pGraphicalObject, !bDemoMode);
    if (SUCCEEDED(hrInsert)) {
        DeleteSelectionArray();
        pGraphicalObject->SetSelected(true);
        m_arSelectedObjects.Add(pGraphicalObject);
        SetFontAndColor(pGraphicalObject);

		if (!bDemoMode) {
            pGraphicalObject->CreateEditControl();
            m_nGraphicalObjectKey = pGraphicalObject->GetHashKey();
        } 
    }
    else
        _ASSERT(false);
    CMainFrameE::GetCurrentInstance()->ResetCurrentIteractionId();
}

void CWhiteboardView::ClickPerformed(CPoint pPoint) {
    if(m_nGraphicalObjectKey != -1) {
        CEditorDoc* pDoc = (CEditorDoc *) GetDocument();

        CInteractionStream *pInteractionStream = pDoc->project.GetInteractionStream();
        if (pInteractionStream != NULL) {
            CGraphicalObject *pGraphicalObject = (CGraphicalObject *)pInteractionStream->GetInteractionWithKey(m_nGraphicalObjectKey);
            if (pGraphicalObject != NULL) {
                CGraphicalObjectEditCtrl *pEditCtrl = pGraphicalObject->GetEditControl();
                if (pEditCtrl != NULL) {
                    CRect rcEdit;
                    pEditCtrl->GetWindowRect(&rcEdit);
                    CRect rcView;
                    GetWindowRect(&rcView);

                    if (!rcEdit.PtInRect(pPoint)/* && rcView.PtInRect(pPoint)*/) {
                        pGraphicalObject->SetGraphicalObjectText();
                    }
                }
            }
        }
    }
}

void CWhiteboardView::SetZoomFactor(double dZoom) {
    m_bZoomValid = true; 
    m_dLastZoom = dZoom;
    if (m_dLastZoom > 0.49 && m_dLastZoom < 0.51)
        m_iMenuZoomPercent = 50;
    else if (m_dLastZoom > 0.99 && m_dLastZoom < 1.01)
        m_iMenuZoomPercent = 100;
    else if (m_dLastZoom > 1.99 && m_dLastZoom < 2.01)
        m_iMenuZoomPercent = 200;
    else
        m_iMenuZoomPercent = 0;

    CreateWbPageBitmap();
}

void CWhiteboardView::AddTimelineDisplayObject(CInteractionAreaEx *pInteraction)
{
	if(pInteraction && pInteraction->GetTypeId() != INTERACTION_RECORD_CLICK_OBJECTS && pInteraction->GetTypeId() != INTERACTION_RECORD_FEEDBACK)
	{
		bool bTimelineDisplay=true;
		for(int i=0;i<GetTimelineDisplayObjects().GetCount();i++){
			if(pInteraction==GetTimelineDisplayObjects()[i])
			{
				bTimelineDisplay = false;
				break;
			}
		}
		if(bTimelineDisplay)
		{
			GetTimelineDisplayObjects().Add(pInteraction);
		}
	}
}

void CWhiteboardView::RemoveTimelineDisplayObject(CInteractionAreaEx *pInteraction)
{
	if(pInteraction)
	{
		for(int i=0;i<GetTimelineDisplayObjects().GetCount();i++)
			if(pInteraction==GetTimelineDisplayObjects()[i])
				GetTimelineDisplayObjects().RemoveAt(i);
	}
}
