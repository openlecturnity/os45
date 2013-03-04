#if !defined(AFX_EDITINTERACTIONDIALOG_H__B73E1186_5A0C_417B_B46C_D1A6FCCCA101__INCLUDED_)
#define AFX_EDITINTERACTIONDIALOG_H__B73E1186_5A0C_417B_B46C_D1A6FCCCA101__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditInteractionDialog.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEditInteractionDialog 
#include "ColorButton.h"
#include "ImageButtonWithStyle.h"
#include "InteractionButtonPreview.h"
#include "..\Studio\resource.h"
#include "afxwin.h"

class CEditInteractionDialog : public CDialog {
// Konstruktion
public:
    CEditInteractionDialog(UINT nTemplateId, CWnd* pParent = NULL);   // Standardkonstruktor

    void Init(CString &csTitle, CString &csButton, CRect &rcArea, COLORREF clrText, LOGFONT *pLfButton);
    void SetUserEntries(CRect &rcArea, COLORREF &clrText, LOGFONT *pLfButton);

private:  
    BOOL ContinueModal();


// Überschreibungen
    // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
    //{{AFX_VIRTUAL(CEditInteractionDialog)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    //}}AFX_VIRTUAL

// Implementierung
protected:

    // Generierte Nachrichtenzuordnungsfunktionen
    //{{AFX_MSG(CEditInteractionDialog)
    virtual BOOL OnInitDialog();
    afx_msg void OnIdleUpdateUIMessage(); // needed for ON_UPDATE_COMMAND_UI
    afx_msg void OnSelEndOkTextClr();
    afx_msg void OnBold();
    afx_msg void OnUpdateBold(CCmdUI *pCmdUI);
    afx_msg void OnItalic();
    afx_msg void OnUpdateItalic(CCmdUI *pCmdUI);
    afx_msg void OnSelchangeFontList();
    afx_msg void OnSelchangeSizeList();
    afx_msg void OnTextColor();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:
    // Dialogfelddaten
    //{{AFX_DATA(CEditInteractionDialog)
    CImageButtonWithStyle	m_btnItalic;
    CImageButtonWithStyle	m_btnBold;
    CInteractionButtonPreview	m_wndButtonPreview;
    CXTFlatComboBox	m_fontsizeList;
    CXTFontCombo	m_fontfamilyList;
    CDlgColorButton m_btnTextColor;
    //}}AFX_DATA


    HICON m_hIconBold;
    HICON m_hIconItalic; 

    // changeable user settings
    COLORREF m_clrText;
    CFont    m_fntButton;
    LOGFONT	m_lfButton;

    // fixed user settings
    CString m_csTitle;
    CString m_csButton;

    // size of the button
    CRect m_rcArea;

    static CString m_csDefaultDirectory;
};


class CEditInteractionTextDialog : public CEditInteractionDialog {
// Konstruktion
public:
    CEditInteractionTextDialog(CWnd* pParent = NULL);   // Standardkonstruktor
    CEditInteractionTextDialog(UINT nTemplateId, CWnd* pParent = NULL);

    void Init(CString &csTitle, CString &csButton, CRect &rcArea, COLORREF clrFill, COLORREF clrLine, COLORREF clrText,
        LOGFONT *pLfButton);
    void SetUserEntries(CRect &rcArea, COLORREF &clrFill, COLORREF &clrLine, COLORREF &clrText, LOGFONT *pLfButton);

private:  


// Überschreibungen
    // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
    //{{AFX_VIRTUAL(CEditInteractionDialog)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    //}}AFX_VIRTUAL

// Implementierung
protected:

    // Generierte Nachrichtenzuordnungsfunktionen
    //{{AFX_MSG(CEditInteractionDialog)
    virtual BOOL OnInitDialog();
    afx_msg void OnSelEndOkLineClr();
    afx_msg void OnSelEndOkFillClr();
    afx_msg void OnLineColor();
    afx_msg void OnFillColor();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:
    // Dialogfelddaten
    //{{AFX_DATA(CEditInteractionDialog)
    enum { IDD = IDD_EDIT_INTERACTIONTEXT };
    CDlgColorButton m_btnLineColor;
    CDlgColorButton m_btnFillColor;
    //}}AFX_DATA

    // changeable user settings
    COLORREF m_clrLine;
    COLORREF m_clrFill;
    CRect m_rcRange;
public:
    afx_msg void OnBnClickedIbuttonReset();
};


class CEditInteractionButtonDialog : public CEditInteractionDialog {
// Konstruktion
public:
    CEditInteractionButtonDialog(CWnd* pParent = NULL);   // Standardkonstruktor

    void Init(CString &csTitle, CString &csButton, CRect &rcArea, CString &csImageFile, COLORREF clrText,
        LOGFONT *pLfButton);
    void SetUserEntries(CRect &rcArea, CString &csImageFile, COLORREF &clrText, LOGFONT *pLfButton);

private:  


// Überschreibungen
    // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
    //{{AFX_VIRTUAL(CEditInteractionDialog)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    //}}AFX_VIRTUAL

// Implementierung
protected:

    // Generierte Nachrichtenzuordnungsfunktionen
    //{{AFX_MSG(CEditInteractionButtonDialog)
    virtual BOOL OnInitDialog();
    afx_msg void OnChooseFile();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:
    // Dialogfelddaten
    //{{AFX_DATA(CEditInteractionDialog)
    enum { IDD = IDD_EDIT_INTERACTIONBUTTON };
    CButton m_btnChooseFile;
    //}}AFX_DATA

    // changeable user settings
    CString m_csImageFileName;
    CString GetButtonImagesPath();
public:
    afx_msg void OnBnClickedIbuttonReset();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EDITINTERACTIONDIALOG_H__B73E1186_5A0C_417B_B46C_D1A6FCCCA101__INCLUDED_
#pragma once


// CEditGraphicalObjectDefaults dialog

class CEditGraphicalObjectDefaults : public CEditInteractionDialog
{
	//DECLARE_DYNAMIC(CEditGraphicalObjectDefaults)

public:
	CEditGraphicalObjectDefaults(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditGraphicalObjectDefaults();

public:
    void Init(CString &csTitle, CString &csButton, CRect &rcArea, COLORREF clrFill, COLORREF clrLine, COLORREF clrText,
        LOGFONT *pLfButton, int iLineWidth, int iLineStyle);
    void GetUserEntries(COLORREF &clrText, COLORREF &clrLine, COLORREF &clrFill, LOGFONT &lf, int &iLineWidth, int &iLineStyle);
// Dialog Data
	enum { IDD = IDD_EDIT_GO_DEFAULTS };
    CDlgColorButton m_btnLineColor;
    CDlgColorButton m_btnFillColor;
    //}}AFX_DATA

    // changeable user settings
    COLORREF m_clrLine;
    COLORREF m_clrFill;
    CRect m_rcRange;
    int m_iLineWidth;
    int m_iLineStyle;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnSelEndOkLineClr();
    afx_msg void OnSelEndOkFillClr();
    afx_msg void OnLineColor();
    afx_msg void OnFillColor();
	DECLARE_MESSAGE_MAP()

    CStatic m_stLineWidth;
    CStatic m_stLineStyle;
public:
    CComboBox m_cmbLineWidth;
protected:
    CComboBox m_cmbLineStyle;
public:
    afx_msg void OnCbnSelchangeGodefComboLs();
public:
    afx_msg void OnCbnSelchangeGodefComboLw();
public:
    afx_msg void OnBnClickedGodefButtonReset();
public:
    CButton m_btnReset;
};
