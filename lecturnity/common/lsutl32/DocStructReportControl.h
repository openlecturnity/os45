#if !defined(AFX_CDOCSTRUCTREPORTCONTROL_H__E0C1EF0A_0A4F_4474_8AF6_567139936FD6__INCLUDED_)
#define AFX_CDOCSTRUCTREPORTCONTROL_H__E0C1EF0A_0A4F_4474_8AF6_567139936FD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DocStructReportControl.h : header file
//
#include "reportcontrol\xtpreportcontrol.h"

class LSUTL32_EXPORT CDocStructReportControl : public CXTPReportControl {
public:
    CDocStructReportControl();
    virtual ~CDocStructReportControl();
    bool IsDroppedInFolder();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

    enum {
        WM_SELECTION_CHANGED = WM_USER + 1,
        WM_SELECTION_DELETE = WM_USER + 2,
        WM_SELECTION_SAVE = WM_USER + 3,
        WM_INSERT_PAGE = WM_USER + 4,
        WM_INSERT_CHAPTER = WM_USER + 5,
        WM_MOVE_ITEM = WM_USER + 6,
        WM_SELECTION_LOAD = WM_USER + 7,
        WM_SELECTION_CLOSE = WM_USER + 8,
        WM_SELECTION_REMOVE = WM_USER + 9

    };

protected:
    //-----------------------------------------------------------------------
    // Summary:
    //     This member is called when an item is dragged over the report control.
    // Parameters:
    //     pDataObject - Points to the COleDataObject being dragged over the
    //                   drop target.
    //     dwKeyState  - State of keys on keyboard.  Contains the state of the
    //                   modifier keys. This is a combination of any number of
    //                   the following: MK_CONTROL, MK_SHIFT, MK_ALT, MK_LBUTTON,
    //                   MK_MBUTTON, and MK_RBUTTON.
    //     point - The current mouse position relative to the report control.
    //     nState - The transition state (0 - enter, 1 - leave, 2 - over).
    // Returns:
    //     A value from the DROPEFFECT enumerated type, which indicates the type
    //     of drop that would occur if the user dropped the object at this
    //     position. The type of drop often depends on the current key state as
    //     indicated by dwKeyState. A standard mapping of keystates to DROPEFFECT
    //     values is:
    //       * <b>DROPEFFECT_NONE</b> The data object cannot be dropped in this
    //                                window.
    //       * <b>DROPEFFECT_COPY</b> for <b>MK_CONTROL</b>  Creates a copy of
    //                                                       the dropped object.
    //       * <b>DROPEFFECT_MOVE</b> for <b>MK_ALT</b> Creates a copy of the dropped
    //                                                  object and delete the original object.
    //                                                  This is typically the default drop effect,
    //                                                  when the view can accept the data object.

    virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point, int nState);
    //-----------------------------------------------------------------------
    // Summary:
    //     This member is called when an item has been dropped into the report control.
    // Parameters:
    //     pDataObject - Points to the COleDataObject that is dropped into the drop target.
    //     dropEffect  - The drop effect that the user has requested. Can be any of the values listed in the Remarks section.
    //     point - The current mouse position relative to the report control.
    // Remarks:
    //     The <i>dropEffect</i> parameter can be one of the following values:
    //     * <b>DROPEFFECT_COPY</b> Creates a copy of the data object being dropped.
    //     * <b>DROPEFFECT_MOVE</b> Moves the data object to the current mouse location.
    // Returns:
    //     TRUE if the drop was successful, otherwise FALSE.
    //-----------------------------------------------------------------------
    virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
    //-----------------------------------------------------------------------
    // Summary:
    //     This method is called when user start drag row
    // Parameters:
    //     point - Drag point
    // Remarks:
    //     Use can override this method or catch LVN_BEGINDRAG message to proceed drag
    //     operations.
    //-----------------------------------------------------------------------
    virtual void OnBeginDrag(CPoint point);
    virtual void DrawDropMarker(CDC* pDC);

    DECLARE_MESSAGE_MAP()

    //{{AFX_MSG(CDocStructReportControl)
    afx_msg void OnDragDropCompleted(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSelectionChanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    //}}AFX_MSG

private:
    //Attributes
    CXTPReportRow* m_pDropRow;
    CXTPReportRow* m_pDragRow;
    CXTPReportRow* m_pMouseOverRow;
    bool m_bDroppedInFolder;
    bool m_bFolderSelected;
    UINT m_nIdDrag;

    // Methods
    void OnLoadItem();
    void OnCloseItem();
    void OnRemoveItem();
    void OnRenameItem();
    void OnDeleteItem();
    void OnSavePageChapter();
    void OnInsertKeywords(); 
    void OnInsertPage();
    void OnInsertChapter();
};

#endif //AFX_CDOCSTRUCTREPORTCONTROL_H__E0C1EF0A_0A4F_4474_8AF6_567139936FD6__INCLUDED_