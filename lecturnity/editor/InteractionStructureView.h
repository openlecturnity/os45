#if !defined(AFX_INTERACTIONSTRUCTURESVIEW_H__98AD2FC9___INCLUDED_)
#define AFX_INTERACTIONSTRUCTURESVIEW_H__98AD2FC9___INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InteractionStructureView.h : Header-Datei
//

#include "afxwin.h"
#include "InteractionAreaEx.h"

class CInteractionRecord;
#define ICON_COLUMN_NO 1

class CInteractionStructureView : public CXTPReportView {
    DECLARE_DYNCREATE(CInteractionStructureView)
private:
    enum INTERACTION_RECORD_PRIORITY {
        INTERACTION_RECORD_PRIORITY_TITLE = 0,
        INTERACTION_RECORD_PRIORITY_TEXT,
        INTERACTION_RECORD_PRIORITY_ANSWERS,
        INTERACTION_RECORD_PRIORITY_BUTTONS,
        INTERACTION_RECORD_PRIORITY_FEEDBACK,
        INTERACTION_RECORD_PRIORITY_STATUSDISPLAY,
        INTERACTION_RECORD_PRIORITY_MISC,
        INTERACTION_RECORD_PRIORITY_NONE
    };

    /* Check the CInteractionAreaEx to see if all children are either all visible or all hidden. 
     * caItem - Subgroup of items to that will be checked */
    bool CInteractionStructureView::CheckChildrenVisibility(CXTPReportRow *pParentRow);

    bool GetStructureTypeCaptionForCurrentPage(CString &csInteractionName, int &nImageInedx);

    void LoadImages();

protected:
    // Variables

    CInteractionAreaEx *m_areEx;
    // interaction elements group like Interactive Area, Button
    CInteractionAreaEx *m_areElement;
    // Interactive objects like Callout, Text
    CInteractionAreaEx *m_areObjects;

    CInteractionAreaEx* m_pAnswers;
    CInteractionAreaEx* m_pButtons;
    CInteractionAreaEx* m_pFeedback;
    CInteractionAreaEx* m_pStatusDisplay;
    CInteractionAreaEx* m_pMisc;

    CArray<CInteractionAreaEx *, CInteractionAreaEx *> m_caInteractions;

    // Methods
    void DeSelectItems();

    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnPaint();
    afx_msg void OnReportItemClick(NMHDR * pNotifyStruct, LRESULT * result);
    afx_msg void OnReportItemDblClick(NMHDR * pNotifyStruct, LRESULT * result);

    //}}AFX_MSG

public:
    CInteractionStructureView(void);
    virtual ~CInteractionStructureView(void);

    void RefreshList(bool bForceRefresh = false);
	void UpdateTimelineVisibleObjects();
    void ShowChildWindows(bool bShow);

    DECLARE_MESSAGE_MAP()
};

#endif //AFX_INTERACTIONSTRUCTURESVIEW_H__98AD2FC9___INCLUDED_