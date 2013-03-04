#if !defined(AFX_PAGEOBJECTSVIEW_H__98AD2FC9_C624_4E26_ACDF_C0DE8C3F3A74__INCLUDED_)
#define AFX_PAGEOBJECTSVIEW_H__98AD2FC9_C624_4E26_ACDF_C0DE8C3F3A74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageObjectsView.h : Header-Datei
//

#include "PageObjectsList.h"

/////////////////////////////////////////////////////////////////////////////
// Ansicht CPageObjectsView 

class CPageObjectsView : public CView {
protected:
    CPageObjectsView();           // Dynamische Erstellung verwendet geschützten Konstruktor
    DECLARE_DYNCREATE(CPageObjectsView)

    // Operationen
public:
    void RefreshButtonFont();
    void RefreshPageObjectList();
    void RenameSelectedObject();

    // Überschreibungen
    // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
    //{{AFX_VIRTUAL(CPageObjectsView)
protected:
    virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    //}}AFX_VIRTUAL


    // Implementierung
protected:
    virtual ~CPageObjectsView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generierte Nachrichtenzuordnungsfunktionen
protected:
    //{{AFX_MSG(CPageObjectsView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnIdleUpdateUIMessage(WPARAM, LPARAM); // needed for ON_UPDATE_COMMAND_UI
    afx_msg void OnForward();
    afx_msg void OnUpdateForward(CCmdUI *pCmdUI);
    afx_msg void OnBackward();
    afx_msg void OnUpdateBackward(CCmdUI *pCmdUI);
    afx_msg void OnFront();
    afx_msg void OnBack();
    afx_msg void OnProperties();
    afx_msg void OnUpdateProperties(CCmdUI *pCmdUI);
    afx_msg void OnRename();
    afx_msg void OnUpdateRename(CCmdUI *pCmdUI);
    afx_msg void OnDelete();
    afx_msg void OnUpdateDelete(CCmdUI *pCmdUI);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    CPageObjectsList m_listPageObjects;	
    CImageList m_ilStateImages;

    CButton m_btnForward;
    CButton m_btnBackward;
    CStatic m_wndSeparator;
    CButton m_btnFront;
    CButton m_btnBack;
    CButton m_btnProperties;
    CButton m_btnRename;
    CButton m_btnDelete;

private:
    enum {
        BUTTON_HEIGHT = 22,
        BUTTON_SPACE = 7,
        BUTTON_VSPACE = 7,
        LIST_OFFSET = 7,
        BUTTONS_HEIGHT = 130
    };

    void ShowChildWindows(bool bShow);
    void GetListRect(CRect &rcList);
    void GetButtonRect(CRect &rcButton, int nID);

    CFont m_fntButton;
    CRgn m_rgnUpdate;
    CString m_csNameColumn;
    CString m_csTypeColumn;
    bool m_bFirstResize;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PAGEOBJECTSVIEW_H__98AD2FC9_C624_4E26_ACDF_C0DE8C3F3A74__INCLUDED_
