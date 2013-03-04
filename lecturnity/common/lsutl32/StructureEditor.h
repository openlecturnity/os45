#pragma once

// Why is it needed to specify this in every header??
#ifdef _LSUTL32_DLL
   #define LSUTL32_EXPORT __declspec(dllexport)
#else 
   #ifdef _LSUTL32_STATIC
      #define LSUTL32_EXPORT 
   #else
      #define LSUTL32_EXPORT __declspec(dllimport)
   #endif
#endif

#include "LrdFile.h"
#include "ScreengrabbingExtended.h"
#include "StructureEditorView.h"
#include "afxwin.h"

class LSUTL32_EXPORT CStructureEditor : public CDialog, IStructureEditorListener
{
	DECLARE_DYNAMIC(CStructureEditor)

public:
    CStructureEditor(CscreengrabbingDoc *pScreengrabbingDoc, CWnd *pParent=NULL);
    ~CStructureEditor();

    // Dialogfelddaten
    enum { IDD = IDD_STRUCTEDITOR_DIALOG };

public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual BOOL PreTranslateMessage(MSG* pMsg); // required for tooltips to work

    void OnSetFont(CFont* pFont);

public:
    //{{AFX_MSG(StructureEditor)
    afx_msg void OnBnClickedStdIndex();
    afx_msg void OnBnClickedStdIndexAlways();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

// functions from IStructureViewListener
public:
    virtual void FillPageList(CArray<CPageEntry *, CPageEntry *> &aPageEntries);
    virtual bool ClickPagesAreVisible();

private:
    void CreateCheckButtonBox();
    void CreatePageStructureView();

protected:
    CStatic m_wndStructureView;
    CscreengrabbingDoc *m_pScreengrabbingDoc;
    CStructureEditorView *m_pStructureView;
    CStatic m_wndGroupBox;
    CButton m_btnStandardIndexInRegistry;
    CButton m_btnStandardIndexInLmd;
    CFont *m_pFont;
    CSize m_szThumbnail;
    bool m_bShowUserEventPages;
};
