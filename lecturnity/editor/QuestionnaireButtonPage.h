#if !defined(AFX_QUESTIONNAIREBUTTONPAGE_H__823DFB65_B9AA_46F9_89C5_19AC05D42DE3__INCLUDED_)
#define AFX_QUESTIONNAIREBUTTONPAGE_H__823DFB65_B9AA_46F9_89C5_19AC05D42DE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractionButtonPreview.h"
#include "QuestionnaireEx.h"
#include "ButtonAppearanceSettings.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CQuestionnaireButtonPage 

class CQuestionnaireButtonPage : public CPropertyPage {
    DECLARE_DYNCREATE(CQuestionnaireButtonPage)

// Konstruktion
public:
    CQuestionnaireButtonPage();
    ~CQuestionnaireButtonPage();

// Dialogfelddaten
    //{{AFX_DATA(CQuestionnaireButtonPage)
    enum { IDD = IDD_QUESTIONNAIRE_BUTTON_PAGE };
    CComboBox	m_wndChoosebuttonTypes;
    CInteractionButtonPreview m_wndPreview[CButtonAppearanceSettings::ACTION_EVENT_COUNT];
    //}}AFX_DATA


// Überschreibungen
    // Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
    //{{AFX_VIRTUAL(CQuestionnaireButtonPage)
public:
    virtual void OnOK();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    //}}AFX_VIRTUAL

// Operationen
public:
    void Init(CQuestionnaireEx *pQuestionnaire);
    void GetUserEntries(COLORREF *pClrFill, COLORREF *pClrLine, 
        COLORREF *pClrText, LOGFONT *pLogFont,
        bool &bIsImageButton, CString *csFilename,
        CString &csOk, CString &csSubmit, CString &csDelete);

// Implementierung
protected:
    // Generierte Nachrichtenzuordnungsfunktionen
    //{{AFX_MSG(CQuestionnaireButtonPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnEdit();
    afx_msg void OnSelchangeButtonTypes();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    UINT m_nButtonType;
    CString m_csButtonText;
    CRect m_rcArea;
    CString m_csDelete;
    CString m_csSubmit;
    CString m_csOk;

    CButtonAppearanceSettings *m_pButtonVisualSettings;
protected:
    CString m_csCaption;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_QUESTIONNAIREBUTTONPAGE_H__823DFB65_B9AA_46F9_89C5_19AC05D42DE3__INCLUDED_
