#if !defined(AFX_INTERACTIONRECORD_H__91BC4FA9___INCLUDED_)
#define AFX_INTERACTIONRECORD_H__91BC4FA9___INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CInteractionRecord.h : Header-Datei
//
#include "reportcontrol\xtpreportrecord.h"

class CInteractionAreaEx;

#define INTERACTION_IMAGE_INDEX_VIEW 0
#define INTERACTION_IMAGE_INDEX_VIEW_HIDE 1
#define INTERACTION_IMAGE_INDEX_DEFAULT 2
#define INTERACTION_IMAGE_INDEX_FEEDBACK 3
#define INTERACTION_IMAGE_INDEX_ANSWERS 4
#define INTERACTION_IMAGE_INDEX_STATUS_DISPLAY 5
#define INTERACTION_IMAGE_INDEX_BUTTONS 6
#define INTERACTION_IMAGE_INDEX_CHOICE 7
#define INTERACTION_IMAGE_INDEX_TEXTGAP 8
#define INTERACTION_IMAGE_INDEX_DRAGNDROP 9
#define INTERACTION_IMAGE_INDEX_INTERACTIVE_AREA 10
#define INTERACTION_IMAGE_INDEX_CALLOUT 11
#define INTERACTION_IMAGE_INDEX_TEXT 12
#define INTERACTION_IMAGE_INDEX_INTERACTIVE_CALLOUT 13

//////////////////////////////////////////////////////////////////////////
// Customized record item, used for displaying read/unread icons.
class CInteractionRecordItemIcon : public CXTPReportRecordItem {
public:
    BOOL m_bVisible;	// TRUE if item is visible on whiteboard view, FALSE if it hidden.

    // Constructs record item with the initial read/unread value.
    CInteractionRecordItemIcon(BOOL bVisible = TRUE);

    // Updates record item icon index depending on read/unread value.
    void UpdateVisibleIcon();
    virtual void DoPropExchange(CXTPPropExchange* pPX);
    virtual int Draw(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs);
};

class CInteractionRecordItemInteractionText : public CXTPReportRecordItemText {
public:
    UINT m_nImageIndex;  // the index of the image that will be displayed in front of the report record. The images are loaded from the report control imagelist.
    UINT m_nPosition;	// TRUE if item is visible on whiteboard view, FALSE if it hidden.

    CInteractionRecordItemInteractionText(UINT nPosition, CString csText);
    int Compare(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem);
    virtual int Draw(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs);
};

class CInteractionRecord : public CXTPReportRecord {
public:
    CInteractionRecordItemIcon* m_pItemIcon;	// Display visible/hidden icon.
    CInteractionRecordItemInteractionText *m_pItemText;
    // We are storing pointer to this item for further use.
    CInteractionAreaEx *m_pInteractionArea;

public:
    CInteractionRecord(CInteractionAreaEx *pInteractionArea = NULL, UINT nPos = 0, CString csText = _T(""), UINT nImageIndex = INTERACTION_IMAGE_INDEX_DEFAULT);

    ~CInteractionRecord(void);

    // Set message as Visible
    BOOL SetVisible();
    BOOL SetParentIconVisible();
    virtual void DoPropExchange(CXTPPropExchange* pPX);
    void CInteractionRecord::UpdateParentIcon();
};

#endif //AFX_INTERACTIONRECORD_H__91BC4FA9___INCLUDED_