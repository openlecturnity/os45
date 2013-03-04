#include "stdafx.h"
#include "InteractionStructureView.h"
#include "EditorDoc.h"
#include "MainFrm.h"
#include "InteractionRecord.h"
#include "InteractionStream.h"
#include "QuestionEx.h"
#include "QuestionStream.h"
#include "LmdFile.h"

IMPLEMENT_DYNCREATE(CInteractionStructureView, CXTPReportView)

//#define ID_REPORT_CONTROL 100

BEGIN_MESSAGE_MAP(CInteractionStructureView, CXTPReportView)
    ON_WM_CREATE()
    ON_WM_SETFOCUS()
    ON_WM_PAINT()
    ON_NOTIFY(NM_CLICK, XTP_ID_REPORT_CONTROL, OnReportItemClick)
    ON_NOTIFY(NM_DBLCLK, XTP_ID_REPORT_CONTROL, OnReportItemDblClick)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CInteractionStructureView::CInteractionStructureView(void) {
    m_areEx = NULL;
    m_areElement = NULL;
    m_pAnswers = NULL;
    m_pButtons = NULL;
    m_pFeedback = NULL;
    m_pStatusDisplay = NULL;
    m_pMisc = NULL;
    m_areObjects = NULL;
	GetReportCtrl().SelectionEnable(FALSE);
}


CInteractionStructureView::~CInteractionStructureView(void) {
    SetReportCtrl(NULL);
    SAFE_DELETE(m_areEx);
    SAFE_DELETE(m_areElement);
    SAFE_DELETE(m_pAnswers);
    SAFE_DELETE(m_pButtons);
    SAFE_DELETE(m_pFeedback);
    SAFE_DELETE(m_pStatusDisplay);
    SAFE_DELETE(m_pMisc);
    SAFE_DELETE(m_areObjects);
}

void CInteractionStructureView::LoadImages() {
    UINT uiImageView[] = {INTERACTION_IMAGE_INDEX_VIEW}; 
    GetReportCtrl().GetImageManager()->SetIcons(IDB_VIEW, uiImageView, _countof(uiImageView), CSize(16, 16));

    UINT uiImageViewHide[] = {INTERACTION_IMAGE_INDEX_VIEW_HIDE}; 
    GetReportCtrl().GetImageManager()->SetIcons(IDB_VIEW_HIDE, uiImageViewHide, _countof(uiImageViewHide), CSize(16, 16));

    UINT uiImages1[] = {INTERACTION_IMAGE_INDEX_DEFAULT}; 
    GetReportCtrl().GetImageManager()->SetIcons(IDB_FORMAT_BACKGROUND, uiImages1, _countof(uiImages1), CSize(16, 16));

    UINT uiImages2[] = {INTERACTION_IMAGE_INDEX_FEEDBACK}; 
    GetReportCtrl().GetImageManager()->SetIcons(IDB_FEEDBACK, uiImages2, _countof(uiImages2), CSize(16, 16));

    UINT uiImages3[] = {INTERACTION_IMAGE_INDEX_ANSWERS}; 
    GetReportCtrl().GetImageManager()->SetIcons(IDB_MAIL_RULES, uiImages3, _countof(uiImages3), CSize(16, 16));

    UINT uiImages4[] = {INTERACTION_IMAGE_INDEX_STATUS_DISPLAY}; 
    GetReportCtrl().GetImageManager()->SetIcons(IDB_STATUS_DISPLAY, uiImages4, _countof(uiImages4), CSize(16, 16));

    UINT uiImages5[] = {INTERACTION_IMAGE_INDEX_BUTTONS}; 
    GetReportCtrl().GetImageManager()->SetIcons(IDB_BUTTON, uiImages5, _countof(uiImages5), CSize(16, 16));

    UINT uiImages6[] = {INTERACTION_IMAGE_INDEX_CHOICE, INTERACTION_IMAGE_INDEX_DRAGNDROP, INTERACTION_IMAGE_INDEX_TEXTGAP};
    GetReportCtrl().GetImageManager()->SetIcons(IDB_TAB_INSERT_SMALL_E, uiImages6, _countof(uiImages6), CSize(15, 15));

    UINT uiImages7[] = {INTERACTION_IMAGE_INDEX_INTERACTIVE_AREA};
    GetReportCtrl().GetImageManager()->SetIcons(IDB_INTERACTIVE_AREA, uiImages7, _countof(uiImages7), CSize(16, 16));

    UINT uiImages8[] = {INTERACTION_IMAGE_INDEX_CALLOUT};
    GetReportCtrl().GetImageManager()->SetIcons(IDB_ICON_CALLOUT, uiImages8, _countof(uiImages8), CSize(16, 16));

    UINT uiImages9[] = {INTERACTION_IMAGE_INDEX_TEXT};
    GetReportCtrl().GetImageManager()->SetIcons(IDB_ICON_TEXT, uiImages9, _countof(uiImages9), CSize(16, 16));

    UINT uiImages10[] = {INTERACTION_IMAGE_INDEX_INTERACTIVE_CALLOUT};
    GetReportCtrl().GetImageManager()->SetIcons(IDB_ICON_INTERACTIVE_CALLOUT, uiImages10, _countof(uiImages10), CSize(16, 16));
}

int CInteractionStructureView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CXTPReportView::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    GetReportCtrl().SetPaintManager(new CXTPReportInteractionStructurePaintManager());
    CXTPToolTipContext* pToolTipContext = GetReportCtrl().GetToolTipContext();
    if (pToolTipContext != NULL) {
        pToolTipContext->SetStyle(xtpToolTipOffice2007);
        pToolTipContext->SetMaxTipWidth(200);
        pToolTipContext->SetMargin(CRect(2, 2, 2, 2));  
        pToolTipContext->SetDelayTime(TTDT_INITIAL, 1000);
    }

    LoadImages();

    CXTPReportControl& wndReport = GetReportCtrl();
    CXTPReportColumn* pColumn = wndReport.AddColumn(new CXTPReportColumn(0, _T("Name"), 250));
    pColumn->SetTreeColumn(TRUE);
    pColumn->SetAlignment(DT_VCENTER);
    pColumn->SetEditable(FALSE);
    pColumn->SetSortable(TRUE);
    pColumn->SetSortIncreasing(TRUE);

    pColumn = wndReport.AddColumn(new CXTPReportColumn(ICON_COLUMN_NO /*1*/, _T(""), 30));
    pColumn->SetMinWidth(25);
    pColumn->SetAlignment(DT_RIGHT);
    pColumn->SetEditable(FALSE);
    pColumn->EnableResize(FALSE);

    wndReport.SetGridStyle(false, xtpReportGridNoLines);
    wndReport.GetReportHeader()->AllowColumnRemove(FALSE);

    wndReport.GetPaintManager()->SetColumnStyle(xtpReportColumnFlat);
    wndReport.EditOnClick(FALSE);
	wndReport.SetMultipleSelection(true);
    wndReport.SetTreeIndent(7);
    wndReport.ShowHeader(FALSE);
    wndReport.ShowRowFocus(FALSE);
    wndReport.GetReportHeader()->AllowColumnResize(false);
    wndReport.SetSortRecordChilds(TRUE);
    wndReport.GetColumns()->SetSortColumn(wndReport.GetColumns()->GetAt(0), TRUE);
    wndReport.GetPaintManager()->m_bShadeSortColumn = FALSE;

    ShowScrollBar(SB_HORZ, FALSE);

    RefreshList();
    return 0;
}

void CInteractionStructureView::OnPaint() {
    CXTPReportView::OnPaint();
}

void CInteractionStructureView::UpdateTimelineVisibleObjects(){
	CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
	ASSERT(pDoc != NULL);
	if (pDoc == NULL) {
		return;
	}
	CInteractionStream *pInteractionStream = pDoc->project.GetInteractionStream();
	if (!pInteractionStream) {
		return;
	}
	CArray<CInteractionAreaEx *, CInteractionAreaEx *> caInteractionsFind;
	CTimePeriod tp;
	tp.Init(INTERACTION_PERIOD_TIME,0,pDoc->m_docLengthMs);
	pInteractionStream->FindInteractions(&tp, false, &caInteractionsFind);
	for (int i = 0; i < caInteractionsFind.GetCount(); ++i) {
		if(caInteractionsFind[i]->IsTimelineDisplay())
			pDoc->GetWhiteboardView()->AddTimelineDisplayObject(caInteractionsFind[i]);
	}
	CInteractionAreaEx *pArea;
	if(pDoc->GetWhiteboardView()!=NULL)
	{
		for(int i=0;i<pDoc->GetWhiteboardView()->GetTimelineDisplayObjects().GetCount();i++)
		{
			if(pDoc->GetWhiteboardView()->GetTimelineDisplayObjects()[i] != NULL)
				pArea = pDoc->project.GetInteractionStream()->GetInteractionWithKey(pDoc->GetWhiteboardView()->GetTimelineDisplayObjects()[i]->GetHashKey());
			pDoc->GetWhiteboardView()->GetTimelineDisplayObjects().RemoveAt(i);
			if(pArea)
				pDoc->GetWhiteboardView()->GetTimelineDisplayObjects().InsertAt(i,pArea);
		}
	}
}

void CInteractionStructureView::RefreshList(bool bForceRefresh) {
	CEditorDoc *pDoc = (CEditorDoc *) GetDocument();
	ASSERT(pDoc != NULL);
	if (pDoc == NULL) {
		return;
	}
	CInteractionStream *pInteractionStream = pDoc->project.GetInteractionStream();
	if (!pInteractionStream) {
		GetReportCtrl().ResetContent();
		return;
	}
	UpdateTimelineVisibleObjects();
	CArray<CInteractionAreaEx *, CInteractionAreaEx *> caInteractionsFind;
	UINT uPageStart=0,uPageEnd=0;
	pDoc->project.GetPagePeriod(pDoc->m_curPosMs,uPageStart,uPageEnd);
	CTimePeriod tp;
	tp.Init(INTERACTION_PERIOD_TIME,uPageStart,uPageEnd);
	pInteractionStream->FindInteractions(&tp, false, &caInteractionsFind);
		// add new interactions to list
		GetReportCtrl().ResetContent();
		CXTPReportControl &wndReport = GetReportCtrl();

		// save new interactions
		m_caInteractions.RemoveAll();
		for (int i = 0; i < caInteractionsFind.GetSize(); ++i) {
			m_caInteractions.InsertAt(0, caInteractionsFind[i]);
				}
		int nImageIndex = 2;
		CString csInteractionName;
		CXTPReportRecord* pReportControl = NULL;
		if (GetStructureTypeCaptionForCurrentPage(csInteractionName, nImageIndex)) {
			if (m_areEx == NULL) {
				m_areEx = new CInteractionAreaEx();
			}

			m_areEx->SetInteractionName(csInteractionName, false);
			m_areEx->SetHiddenDuringEdit(false);

			pReportControl = new CInteractionRecord(m_areEx, 0, _T(""), nImageIndex);
			pReportControl->SetExpanded(TRUE);
		}

		CXTPReportRecord* pAnswers = NULL, *pButtons = NULL, *pFeedback = NULL, *pStatusDisplay = NULL, *pMisc = NULL;
		CXTPReportRecord* pReportControlInteractionElement = NULL, *pReportControlInteractionObjects = NULL;
		CString csCaption = _T("");
		for (int i = 0; i < m_caInteractions.GetSize(); ++i) {
			InteractionRecordType interactionType = m_caInteractions[i]->GetTypeId();
			switch (interactionType) {
				case INTERACTION_RECORD_TEXT: {
					if (pReportControl) {
						csCaption.LoadString(IDS_ASSIGNMENT_TEXT);
						CInteractionRecord *pItem = (CInteractionRecord*) pReportControl->GetChilds()->Add(
							new CInteractionRecord(m_caInteractions[i], INTERACTION_RECORD_PRIORITY_TEXT, csCaption));
					}
					break;
											  }
				case INTERACTION_RECORD_TITLE: {
					if (pReportControl) {
						csCaption.LoadString(IDC_LABEL_TITLE);
						csCaption = csCaption + _T(" ");
						CInteractionRecord *pItem = (CInteractionRecord*) pReportControl->GetChilds()->Add(
							new CInteractionRecord(m_caInteractions[i], INTERACTION_RECORD_PRIORITY_TITLE, csCaption ));
					}
					break;
											   }
				case INTERACTION_RECORD_ANSWERS: {
					if (pAnswers == NULL && pReportControl != NULL) {
						csCaption.LoadString(IDC_GROUP_ANSWERS);
						if (m_pAnswers == NULL) {
							m_pAnswers = new CInteractionAreaEx();
							m_pAnswers->SetInteractionName(csCaption, false);
							m_pAnswers->SetHiddenDuringEdit(false);
						}
						pAnswers = pReportControl->GetChilds()->Add(new CInteractionRecord(m_pAnswers, INTERACTION_RECORD_PRIORITY_ANSWERS, _T("") ,INTERACTION_IMAGE_INDEX_ANSWERS));
						pAnswers->SetExpanded(TRUE);
					}
					if (pAnswers) {
						pAnswers->GetChilds()->Add(new CInteractionRecord(m_caInteractions[i]));
					}
					break;
												 }
				case INTERACTION_RECORD_BUTTONS: {
					if (pButtons == NULL && pReportControl != NULL) {
						csCaption.LoadString(IDD_QUESTIONNAIRE_BUTTON_PAGE);
						if (m_pButtons == NULL) {
							m_pButtons = new CInteractionAreaEx();
							m_pButtons->SetInteractionName(csCaption, false);
							m_pButtons->SetHiddenDuringEdit(false);
						}
						pButtons = pReportControl->GetChilds()->Add(new CInteractionRecord(m_pButtons, INTERACTION_RECORD_PRIORITY_BUTTONS, _T("") , INTERACTION_IMAGE_INDEX_BUTTONS));
						pButtons->SetExpanded(TRUE);
					}
					if (pButtons) {
						pButtons->GetChilds()->Add(new CInteractionRecord(m_caInteractions[i]));
					}
					break;
												 }
				//case INTERACTION_RECORD_FEEDBACK: {
				//	if (pFeedback == NULL && pReportControl != NULL) {
				//		csCaption.LoadString(IDS_FEEDBACK);
				//		if (m_pFeedback == NULL) {
				//			m_pFeedback = new CInteractionAreaEx();
				//			m_pFeedback->SetInteractionName(csCaption, false);
				//			m_pFeedback->SetHiddenDuringEdit(false);
				//		}
				//		pFeedback = pReportControl->GetChilds()->Add(new CInteractionRecord(m_pFeedback, INTERACTION_RECORD_PRIORITY_FEEDBACK, _T("") , INTERACTION_IMAGE_INDEX_FEEDBACK));
				//		pFeedback->SetExpanded(TRUE);
				//	}
				//	if (pFeedback) {
				//		pFeedback->GetChilds()->Add(new CInteractionRecord(m_caInteractions[i]));
				//	}
				//	break;
				//								  }
				case INTERACTION_RECORD_STATUS: {
					if (pStatusDisplay == NULL && pReportControl != NULL) {
						csCaption.LoadString(IDS_STATUS_DISPLAY);
						if (m_pStatusDisplay == NULL) {
							m_pStatusDisplay = new CInteractionAreaEx();
							m_pStatusDisplay->SetInteractionName(csCaption, false);
							m_pStatusDisplay->SetHiddenDuringEdit(false);
						}
						pStatusDisplay = pReportControl->GetChilds()->Add(new CInteractionRecord(m_pStatusDisplay, INTERACTION_RECORD_PRIORITY_STATUSDISPLAY, _T(""), INTERACTION_IMAGE_INDEX_STATUS_DISPLAY));
						pStatusDisplay->SetExpanded(TRUE);
					}
					if (pStatusDisplay) {
						pStatusDisplay->GetChilds()->Add(
							new CInteractionRecord(m_caInteractions[i]));
					}
					break;
												}
				case INTERACTION_RECORD_ELEMENTS: {
					if (pReportControlInteractionElement == NULL) {
						csCaption.LoadString(IDS_INTERACTION_OBJECTS);
						if (m_areElement == NULL) {
							m_areElement = new CInteractionAreaEx();
							m_areElement->SetInteractionName(csCaption, false);
							m_areElement->SetHiddenDuringEdit(false);
						}
						pReportControlInteractionElement = new CInteractionRecord(m_areElement);
						pReportControlInteractionElement->SetExpanded(TRUE);
					}
					if (m_caInteractions[i]->GetButtonType() == BUTTON_TYPE_OTHER) {
						pReportControlInteractionElement->GetChilds()->Add(
							new CInteractionRecord(m_caInteractions[i], 0, _T(""), INTERACTION_IMAGE_INDEX_BUTTONS));
					} else {
						pReportControlInteractionElement->GetChilds()->Add(
							new CInteractionRecord(m_caInteractions[i], 0, _T(""), INTERACTION_IMAGE_INDEX_INTERACTIVE_AREA));
					}
					break;
												  }
				case INTERACTION_RECORD_MISC: {
					if (pMisc == NULL && pReportControl != NULL) {
						csCaption.LoadString(IDS_MISC);
						if (m_pMisc == NULL) {
							m_pMisc = new CInteractionAreaEx();
							m_pMisc->SetInteractionName(csCaption, false);
							m_pMisc->SetHiddenDuringEdit(false);
						}
						pMisc = pReportControl->GetChilds()->Add(new CInteractionRecord(m_pMisc, INTERACTION_RECORD_PRIORITY_MISC));
						pMisc->SetExpanded(TRUE);
					}
					if (pMisc) {
						pMisc->GetChilds()->Add(new CInteractionRecord(m_caInteractions[i]));
					}
					break;
											  }
				case INTERACTION_RECORD_GRAPHIC_OBJECTS: {
					if(m_caInteractions[i]->IsDemoDocumentObject()){
						if (pReportControlInteractionElement == NULL) {
							csCaption.LoadString(IDS_INTERACTION_OBJECTS);
							if (m_areElement == NULL) {
								m_areElement = new CInteractionAreaEx();
								m_areElement->SetInteractionName(csCaption, false);
								m_areElement->SetHiddenDuringEdit(false);
							}
							pReportControlInteractionElement = new CInteractionRecord(m_areElement);
							pReportControlInteractionElement->SetExpanded(TRUE);
						}
						if (m_caInteractions[i]->GetButtonType() == BUTTON_TYPE_OTHER) {
							pReportControlInteractionElement->GetChilds()->Add(
								new CInteractionRecord(m_caInteractions[i], 0, _T(""), INTERACTION_IMAGE_INDEX_BUTTONS));
						} else {
							pReportControlInteractionElement->GetChilds()->Add(
								new CInteractionRecord(m_caInteractions[i], 0, _T(""), INTERACTION_IMAGE_INDEX_INTERACTIVE_CALLOUT));
						}
					}
					else{
						if (pReportControlInteractionObjects == NULL) {
							csCaption.LoadString(ID_GROUP_GRAPHICAL_OBJECTS);
							if (m_areObjects == NULL) {
								m_areObjects = new CInteractionAreaEx();
								m_areObjects->SetInteractionName(csCaption, false);
								m_areObjects->SetHiddenDuringEdit(false);
							}
							pReportControlInteractionObjects = new CInteractionRecord(m_areObjects);
							pReportControlInteractionObjects->SetExpanded(TRUE);
						}
						if (m_caInteractions[i]->GetButtonType() == BUTTON_TYPE_OTHER) {
							pReportControlInteractionObjects->GetChilds()->Add(
								new CInteractionRecord(m_caInteractions[i], 0, _T(""), INTERACTION_IMAGE_INDEX_BUTTONS));
						} else {
                            CGraphicalObject *pGo = (CGraphicalObject*)m_caInteractions[i];
                            if (pGo->IsCallout())
                                pReportControlInteractionObjects->GetChilds()->Add(
                                new CInteractionRecord(m_caInteractions[i], 0, _T(""), INTERACTION_IMAGE_INDEX_CALLOUT));
                            else
                                pReportControlInteractionObjects->GetChilds()->Add(
                                new CInteractionRecord(m_caInteractions[i], 0, _T(""), INTERACTION_IMAGE_INDEX_TEXT));
						}
					}
					break;
														 }
				case INTERACTION_RECORD_UNKNOWN: {
					// should never happen
					ASSERT(false);
					break;
												 }
			}
		}

		if(pReportControlInteractionObjects && pReportControlInteractionObjects->GetChilds()->GetCount() > 0)
			wndReport.AddRecord(pReportControlInteractionObjects);
		if(pReportControlInteractionElement && pReportControlInteractionElement->GetChilds()->GetCount() > 0)
			wndReport.AddRecord(pReportControlInteractionElement);
		if(pReportControl && pReportControl->GetChilds()->GetCount() > 0)
			wndReport.AddRecord(pReportControl);

		wndReport.Populate();

	//clicked item is marked as selected so reselect timeline selected
	DeSelectItems();
	for (int i = 0; i < GetReportCtrl().GetRows()->GetCount(); i++)
	{
		if(((CInteractionRecord*)GetReportCtrl().GetRows()->GetAt(i)->GetRecord())->m_pInteractionArea->IsSelected())
			GetReportCtrl().GetRows()->GetAt(i)->SetSelected(TRUE);
		else
			GetReportCtrl().GetRows()->GetAt(i)->SetSelected(FALSE);
		((CInteractionRecord *)GetReportCtrl().GetRows()->GetAt(i)->GetRecord())->m_pItemIcon->m_bVisible = 
			((CInteractionRecord *)GetReportCtrl().GetRows()->GetAt(i)->GetRecord())->m_pInteractionArea->IsTimelineDisplay()||
			((CInteractionRecord *)GetReportCtrl().GetRows()->GetAt(i)->GetRecord())->m_pInteractionArea->IsSelected();
		((CInteractionRecord *)GetReportCtrl().GetRows()->GetAt(i)->GetRecord())->m_pItemIcon->UpdateVisibleIcon();
	}
	for (int i = 0; i < GetReportCtrl().GetRows()->GetCount(); i++) {
		CheckChildrenVisibility(GetReportCtrl().GetRows()->GetAt(i));
	}

	GetReportCtrl().RedrawControl();
}

bool CInteractionStructureView::CheckChildrenVisibility(CXTPReportRow *pRow) {
    if (pRow == NULL) {
        ASSERT(false);
        return false;
    }
    bool bAllHidden = true;
    if (pRow->HasChildren() == false) {
        return true;
    }
    CInteractionRecord *pRec = (CInteractionRecord *)pRow->GetRecord();
    for (int i = 0; i < pRow->GetChilds()->GetCount(); i++) {
        CInteractionRecord *pChildRecord = (CInteractionRecord*) pRow->GetChilds()->GetAt(i)->GetRecord();
        if (pChildRecord == NULL || pChildRecord->m_pItemIcon == NULL) {
            ASSERT(false);
            continue;
        }

        if (pChildRecord->m_pItemIcon->m_bVisible == TRUE) {
            pRec->m_pItemIcon->m_bVisible = true;
            pRec->m_pItemIcon->UpdateVisibleIcon();
            bAllHidden = false;
        }
    }
    if (bAllHidden == true) {
        pRec->m_pItemIcon->m_bVisible = false;
        pRec->m_pItemIcon->UpdateVisibleIcon();
    }
    return true;
}

void CInteractionStructureView::ShowChildWindows(bool bShow) {
    CXTPReportControl& wndReport = GetReportCtrl();
    if (bShow) {
        if (!wndReport.IsWindowVisible())
            wndReport.ShowWindow(SW_SHOW);
    } else {
        if (wndReport.IsWindowVisible())
            wndReport.ShowWindow(SW_HIDE);
    }
}

void CInteractionStructureView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) {
    CXTPReportView::OnUpdate(pSender, lHint, pHint);
    if (lHint != HINT_CHANGE_POSITION && lHint != HINT_CHANGE_PAGE && lHint != HINT_CHANGE_STRUCTURE) {
        return; // ignore
    }

    RefreshList(pHint != NULL);
    RedrawWindow();	
}

void CInteractionStructureView::OnSetFocus(CWnd* pOldWnd) {
    CView::OnSetFocus(pOldWnd);
    GetReportCtrl().SetFocus();
}

// Handles clicking ReportControl row.
void CInteractionStructureView::OnReportItemClick(NMHDR * pNotifyStruct, LRESULT * /*result*/) {
	XTP_NM_REPORTRECORDITEM* pItemNotify = (XTP_NM_REPORTRECORDITEM*) pNotifyStruct;

	if (!pItemNotify->pRow || !pItemNotify->pColumn) {
		return;
	}
	CMainFrameE *pMainWnd = CMainFrameE::GetCurrentInstance();
	CWhiteboardView *pWhiteboardView = NULL;
	if (pMainWnd == NULL)
		return;
	pWhiteboardView = pMainWnd->GetWhiteboardView();
	if(pWhiteboardView==NULL)
		return;

	// Do not do a thing if the row is expandable. 
	// These report items do not have a coresponding "Interaction Area" in Whiteboard View.
	if (pItemNotify->pRow->HasChildren() == TRUE) {
		pMainWnd->UpdateStreamView();
		return;
	}

	CInteractionRecord *pInteractionRecord = (CInteractionRecord*)pItemNotify->pRow->GetRecord();//pItem->GetRecord();
	if (pInteractionRecord == NULL) {
		ASSERT(false);
		return;
	}
	CInteractionAreaEx *pArea = pInteractionRecord->m_pInteractionArea;

	//MD - another way to fix wrong reaction on column checkbox click - comment line GetReportCtrl().Populate();
	//	instead of 	adding condition if (!bCheckBoxClicked)in function CXTPReportRecordItem::OnClick
	if (pItemNotify->pColumn->GetItemIndex() == ICON_COLUMN_NO) 
	{
		if (pInteractionRecord->m_pItemIcon == NULL) {
			ASSERT(false);
			return;
		}
		if(pInteractionRecord->m_pItemIcon->m_bVisible == TRUE)
		{
			pArea->SetSelected(false);
			pWhiteboardView->UpdateSelectionArray();
			pArea->SetTimelineDisplay(FALSE);
			pWhiteboardView->RemoveTimelineDisplayObject(pArea);
		}
		else
		{
			pArea->SetTimelineDisplay(TRUE);
			pWhiteboardView->AddTimelineDisplayObject(pArea);
		}

		pInteractionRecord->m_pItemIcon->m_bVisible = pInteractionRecord->m_pInteractionArea->IsTimelineDisplay() || pInteractionRecord->m_pInteractionArea->IsSelected();
		if (pItemNotify->pRow->GetParentRow() != NULL){
			CheckChildrenVisibility(pItemNotify->pRow->GetParentRow());
		}
		pInteractionRecord->m_pItemIcon->UpdateVisibleIcon();
		GetReportCtrl().Populate();

			pMainWnd->UpdateStreamView();
			pMainWnd->UpdateWhiteboardView();
	}
	else
	{			
		//whiteboard update
			pWhiteboardView->DeleteSelectionArray();
			pArea->SetSelected(true);
			pWhiteboardView->GetSelectedObjects().Add(pArea);
			pWhiteboardView->UpdateSelectionArray();
			pMainWnd->UpdateWhiteboardView();
			pMainWnd->UpdateStreamView();
	}
	DeSelectItems();
	for (int i = 0; i < GetReportCtrl().GetRows()->GetCount(); i++)
	{
		if(((CInteractionRecord*)GetReportCtrl().GetRows()->GetAt(i)->GetRecord())->m_pInteractionArea->IsSelected())
			GetReportCtrl().GetRows()->GetAt(i)->SetSelected(TRUE);
		else
			GetReportCtrl().GetRows()->GetAt(i)->SetSelected(FALSE);
		((CInteractionRecord *)GetReportCtrl().GetRows()->GetAt(i)->GetRecord())->m_pItemIcon->m_bVisible = 
			((CInteractionRecord *)GetReportCtrl().GetRows()->GetAt(i)->GetRecord())->m_pInteractionArea->IsTimelineDisplay()||
			((CInteractionRecord *)GetReportCtrl().GetRows()->GetAt(i)->GetRecord())->m_pInteractionArea->IsSelected();
		((CInteractionRecord *)GetReportCtrl().GetRows()->GetAt(i)->GetRecord())->m_pItemIcon->UpdateVisibleIcon();
	}
	for (int i = 0; i < GetReportCtrl().GetRows()->GetCount(); i++) {
		CheckChildrenVisibility(GetReportCtrl().GetRows()->GetAt(i));
	}

	GetReportCtrl().RedrawControl();
}

void CInteractionStructureView::OnReportItemDblClick(NMHDR * pNotifyStruct, LRESULT * result) {
    XTP_NM_REPORTRECORDITEM* pItemNotify = (XTP_NM_REPORTRECORDITEM*) pNotifyStruct;
    if (!pItemNotify->pRow || !pItemNotify->pColumn) {
        return;
    }

    // User double clicked on the glyph. Return from function to prevent expanding row again.
    if (pItemNotify->pItem == NULL) {
        return;
    }

    if (pItemNotify->pRow->HasChildren() == TRUE) {
        // OBS: These report items do not have a coresponding "Interaction Area" in Whiteboard View.
        pItemNotify->pRow->SetExpanded(!pItemNotify->pRow->IsExpanded());
        return;
    }

    CWhiteboardView *pWhiteboardView = CMainFrameE::GetCurrentInstance()->GetWhiteboardView();
    if (pWhiteboardView) {
        pWhiteboardView->ShowInteractionProperties();

    }
}

void CInteractionStructureView::DeSelectItems() {
    for (int i = 0; i < GetReportCtrl().GetRows()->GetCount(); i++) {
        GetReportCtrl().GetRows()->GetAt(i)->SetSelected(false);
    }
}

bool CInteractionStructureView::GetStructureTypeCaptionForCurrentPage(CString &csInteractionName, int &nImageInedx) {
    csInteractionName.LoadString(ID_ADD_QUESTION_UNK);
    CEditorDoc *pEditorDoc = (CEditorDoc *)GetDocument();
    if (pEditorDoc == NULL) {
        return false;
    }
    CQuestionStream *pQuestionStream = pEditorDoc->project.GetQuestionStream(false);
    if (pQuestionStream == NULL) {
        return false;
    }
    CQuestionEx *pQuestion = pQuestionStream->FindQuestionForTime(pEditorDoc->m_curPosMs);
    if (pQuestion == NULL) {
        return false;
    }

    switch (pQuestion->GetType()) {
        case QUESTION_TYPE_CHOICE: {
            csInteractionName.LoadString(ID_ADD_QUESTION_MC); 
            nImageInedx = INTERACTION_IMAGE_INDEX_CHOICE;
            break;
        }
        case QUESTION_TYPE_TEXTGAP: {
            csInteractionName.LoadString(ID_ADD_QUESTION_FB);
            nImageInedx = INTERACTION_IMAGE_INDEX_TEXTGAP;
            break;
        }
        case QUESTION_TYPE_DRAGNDROP: {
            csInteractionName.LoadString(ID_ADD_QUESTION_DD);
            csInteractionName.Replace(_T("&&"), _T("&"));
            nImageInedx = INTERACTION_IMAGE_INDEX_DRAGNDROP;
            break;
        }
        default: {
            // there should be no other cases
            ASSERT(false);
            return false;
        }
    }
    return true;
}