#pragma once

#include "ColorStatic.h"
#include "ImageStatic.h"

class CPageEntry
{
public:
    CPageEntry();
    ~CPageEntry();

public:
    void SetTitle(CString &csTitle);
    CString &GetTitle();
    void SetThumbImage(CString &csThumbFilename);
    CString &GetThumbFilename() {return m_csThumbFilename;}
    void ReleaseThumbImage();
    void SetBegin(int iBegin) {m_iBegin = iBegin;}
    int GetBegin() {return m_iBegin;}
    void SetPageNumber(int iPageNumber) {m_iPageNumber = iPageNumber;}
    int GetPageNumber() {return m_iPageNumber;}
    void SetDoRemove(bool bRemove) {m_bRemove = bRemove;}
    bool DoRemove() {return m_bRemove;}
    void SetIsClickPage(bool bIsClickPage) {m_bIsClickPage = bIsClickPage;}
    bool IsClickPage() {return m_bIsClickPage;}
    bool PageIsHidden() {return m_bPageIsHidden;}
    void Hide();
    void SetPosition(CPoint ptBegin);
    int GetHeight();
    int GetWidth();
    void SetFont(CFont *pFont) {m_pFont = pFont;}

    void CreateTimeField(CWnd *pParent);
    void CreateTitleField(CWnd *pParent);
    void CreateRemoveButton(CWnd *pParent);

    CEdit *GetTitleFieldWnd() {return m_pTitleField;}
    void SetTimeFieldWnd(CStatic *pTimeField) {m_pTimeField = pTimeField;}
    CStatic *GetTimeFieldWnd() {return m_pTimeField;}
    void SetRemoveButtonWnd(CBitmapButton *pRemoveButton) {m_pRemoveButton = pRemoveButton;}
    CBitmapButton *GetRemoveButtonWnd() {return m_pRemoveButton;}

    void Draw(CDC *pDC, CPoint &ptStart);

private:
    enum offsets {
        BORDER_SPACE = 5,
        BETWEEN_SPACE_X = 4,
        BETWEEN_SPACE_Y = 2
    };
    CEdit *m_pTitleField;
    CStatic *m_pTimeField;
    CBitmapButton *m_pRemoveButton;
    Gdiplus::Image *m_pThumbnailImage;
    Gdiplus::Bitmap *m_pClickIcon;

    bool m_bRemove;
    bool m_bIsClickPage;
    CString m_csTitle;
    CString m_csThumbFilename;
    int m_iBegin;
    bool m_bPageIsHidden;
    int m_iPageNumber;

    CFont *m_pFont;

    CSize m_szTimeField;
    CSize m_szRemoveButton;
    CSize m_szTitleField;
    CSize m_szClickIcon;
    CSize m_szThumbnail;
};

class IStructureEditorListener 
{
public:
   IStructureEditorListener() {}
   virtual ~IStructureEditorListener() {}

public:
    virtual void FillPageList(CArray<CPageEntry *, CPageEntry *> &aPageEntries) = 0;
    virtual bool ClickPagesAreVisible() = 0;

};

class CStructureEditorView : public CScrollView
{
    DECLARE_DYNCREATE(CStructureEditorView)

public:
    CStructureEditorView();
    virtual ~CStructureEditorView();

public:
    void SetListener(IStructureEditorListener *pListener) {m_pListener = pListener;}
    void GetPageEntries(CArray<CPageEntry *, CPageEntry *> &aPageEntries);
    void UpdatePageList(bool bHideUserEventPages);
    void SetFont(CFont* pFont);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    virtual void OnInitialUpdate();     // Erster Aufruf nach Erstellung
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void OnDraw(CDC* pDC);      // Überladen, um diese Ansicht darzustellen
    //{{AFX_MSG(StructureEditorView)
    afx_msg void OnDeleteButton( UINT nID );
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    CPoint m_ptTopLeft;
    CToolTipCtrl m_ttipctrl;
    CFont *m_pFont;
    IStructureEditorListener *m_pListener;
    CArray<CPageEntry *, CPageEntry *> m_aPageEntries;

};


