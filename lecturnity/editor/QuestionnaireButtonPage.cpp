// QuestionnaireButtonPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "QuestionnaireButtonPage.h"
#include "EditInteractionDialog.h"
#include "InteractionButtonDialog.h"
#include "MfcUtils.h"
#include "ButtonAppearanceSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "lutils.h"

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CQuestionnaireButtonPage 

IMPLEMENT_DYNCREATE(CQuestionnaireButtonPage, CPropertyPage)

CQuestionnaireButtonPage::CQuestionnaireButtonPage() : CPropertyPage(CQuestionnaireButtonPage::IDD)
{
	//{{AFX_DATA_INIT(CQuestionnaireButtonPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
   
   
   m_csCaption.LoadString(CQuestionnaireButtonPage::IDD);
   m_psp.pszTitle = m_csCaption;
   m_psp.dwFlags |= PSP_USETITLE;

   m_nButtonType = 0;

   m_pButtonVisualSettings = new CButtonAppearanceSettings();
   
   for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
       m_pButtonVisualSettings->m_lf[i].lfHeight = -14;
   }
   
   m_csDelete.LoadString(IDC_BUTTON_DELETE);
   m_csSubmit.LoadString(IDS_BUTTON_SUBMIT);
   m_csOk.LoadString(IDS_BUTTON_OK);

   m_csButtonText.LoadString(IDS_DEFAULT_TEXT);
   m_rcArea.SetRect(0, 0, 80, 40);
   
   m_nButtonType = 0;
}

CQuestionnaireButtonPage::~CQuestionnaireButtonPage()
{
    SAFE_DELETE(m_pButtonVisualSettings);
}

void CQuestionnaireButtonPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuestionnaireButtonPage)
	DDX_Control(pDX, IDC_CHOOSE_BUTTON_TYPES, m_wndChoosebuttonTypes);
	DDX_Control(pDX, IDC_PREVIEW_PASSIV, m_wndPreview[CButtonAppearanceSettings::ACTION_PASSIV]);
	DDX_Control(pDX, IDC_PREVIEW_NORMAL, m_wndPreview[CButtonAppearanceSettings::ACTION_NORMAL]);
    DDX_Control(pDX, IDC_PREVIEW_MOUSEOVER, m_wndPreview[CButtonAppearanceSettings::ACTION_OVER]);
    DDX_Control(pDX, IDC_PREVIEW_PRESSED, m_wndPreview[CButtonAppearanceSettings::ACTION_PRESSED]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuestionnaireButtonPage, CPropertyPage)
	//{{AFX_MSG_MAP(CQuestionnaireButtonPage)
	ON_BN_CLICKED(IDC_EDIT_PASSIV, OnEdit)
	ON_BN_CLICKED(IDC_EDIT_NORMAL, OnEdit)
	ON_BN_CLICKED(IDC_EDIT_MOUSEOVER, OnEdit)
	ON_BN_CLICKED(IDC_EDIT_PRESSED, OnEdit)
	ON_CBN_SELCHANGE(IDC_CHOOSE_BUTTON_TYPES, OnSelchangeButtonTypes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CQuestionnaireButtonPage 

BOOL CQuestionnaireButtonPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
   
   DWORD dwIds[] = {IDC_LABEL_DELETE,
                    IDC_LABEL_SUBMIT,
                    IDC_LABEL_CONFIRM,
                    IDC_GROUP_BUTTONTEXTS,
                    IDC_GROUP_BUTTONVIEW,
                    IDC_LABEL_BUTTON_TYPE,
                    IDC_LABEL_PASSIV,
                    IDC_LABEL_NORMAL,
                    IDC_LABEL_MOUSEOVER,
                    IDC_LABEL_MOUSEDOWN,
                    IDC_EDIT_PASSIV,
                    IDC_EDIT_NORMAL,
                    IDC_EDIT_MOUSEOVER,
                    IDC_EDIT_PRESSED,
                    -1};
   MfcUtils::Localize(this, dwIds);

   CString csType;
   csType.LoadString(IDS_TEXT_BUTTON);
   m_wndChoosebuttonTypes.AddString(csType);
   csType.LoadString(IDS_IMAGE_BUTTON);
   m_wndChoosebuttonTypes.AddString(csType);
   m_wndChoosebuttonTypes.SetCurSel(0);   

   m_wndChoosebuttonTypes.SetCurSel(m_nButtonType);
   bool bIsImagebutton = m_nButtonType != 0;

   for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i)
   {
      m_wndPreview[i].IsImageButton(bIsImagebutton);
      m_wndPreview[i].SetImage(m_pButtonVisualSettings->m_csFileNames[i]);
      m_wndPreview[i].GetImageDimension(m_pButtonVisualSettings->m_siImageDimension[i]);
      m_wndPreview[i].SetFillColor(m_pButtonVisualSettings->m_clrFill[i]);
      m_wndPreview[i].SetLineColor(m_pButtonVisualSettings->m_clrLine[i]);

      m_wndPreview[i].SetTextColor(m_pButtonVisualSettings->m_clrText[i]);
      m_wndPreview[i].SetButtonFont(&m_pButtonVisualSettings->m_lf[i]);
      m_wndPreview[i].SetText(m_csButtonText);
   }

   CEdit *pEdit = NULL;
   
   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DELETE);
   if (pEdit)
      pEdit->SetWindowText(m_csDelete);

   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_SEND);
   if (pEdit)
      pEdit->SetWindowText(m_csSubmit);

   pEdit = (CEdit *)GetDlgItem(IDC_EDIT_APPROVE);
   if (pEdit)
      pEdit->SetWindowText(m_csOk);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CQuestionnaireButtonPage::OnEdit() 
{
    const MSG* pMsg = GetCurrentMessage( );

    int iAction = -1;
    CString csTitle;
    switch (WPARAM(pMsg->wParam))
    {
    case IDC_EDIT_PASSIV:
        csTitle.LoadString(IDS_PREVIEW_PASSIV);
        iAction = CButtonAppearanceSettings::ACTION_PASSIV;
        break;
    case IDC_EDIT_NORMAL:
        csTitle.LoadString(IDS_PREVIEW_NORMAL);
        iAction = CButtonAppearanceSettings::ACTION_NORMAL;
        break;
    case IDC_EDIT_MOUSEOVER:
        csTitle.LoadString(IDS_PREVIEW_MOUSEOVER);
        iAction = CButtonAppearanceSettings::ACTION_OVER;
        break;
    case IDC_EDIT_PRESSED:
        csTitle.LoadString(IDS_PREVIEW_PRESSED);
        iAction = CButtonAppearanceSettings::ACTION_PRESSED;
        break;
    }

    // TODO Error message
    if (iAction < 0)
        return;

    if (m_wndChoosebuttonTypes.GetCurSel() == 0) {
        CEditInteractionTextDialog dlgEdit(this);

        dlgEdit.Init(csTitle, m_csButtonText, m_rcArea, m_pButtonVisualSettings->m_clrFill[iAction], m_pButtonVisualSettings->m_clrLine[iAction],
            m_pButtonVisualSettings->m_clrText[iAction], &m_pButtonVisualSettings->m_lf[iAction]);

        if (dlgEdit.DoModal() == IDOK) {

            dlgEdit.SetUserEntries(m_rcArea, m_pButtonVisualSettings->m_clrFill[iAction], m_pButtonVisualSettings->m_clrLine[iAction],
                m_pButtonVisualSettings->m_clrText[iAction], &m_pButtonVisualSettings->m_lf[iAction]);

            m_wndPreview[iAction].SetColors(m_pButtonVisualSettings->m_clrFill[iAction], m_pButtonVisualSettings->m_clrLine[iAction],
                m_pButtonVisualSettings->m_clrText[iAction]);
            m_wndPreview[iAction].SetButtonFont(&m_pButtonVisualSettings->m_lf[iAction]);
        }
    } else {
        CEditInteractionButtonDialog dlgEdit(this);

        dlgEdit.Init(csTitle, m_csButtonText, m_rcArea, m_pButtonVisualSettings->m_csFileNames[iAction],
            m_pButtonVisualSettings->m_clrText[iAction], &m_pButtonVisualSettings->m_lf[iAction]);

        if (dlgEdit.DoModal() == IDOK) {
            dlgEdit.SetUserEntries(m_rcArea, m_pButtonVisualSettings->m_csFileNames[iAction],
                m_pButtonVisualSettings->m_clrText[iAction], &m_pButtonVisualSettings->m_lf[iAction]);

            m_wndPreview[iAction].SetTextColor(m_pButtonVisualSettings->m_clrText[iAction]);
            m_wndPreview[iAction].SetButtonFont(&m_pButtonVisualSettings->m_lf[iAction]);
            m_wndPreview[iAction].SetImage(m_pButtonVisualSettings->m_csFileNames[iAction]);
            m_wndPreview[iAction].GetImageDimension(m_pButtonVisualSettings->m_siImageDimension[iAction]);

            // if passive button was changed, apply to all other buttons
            if (WPARAM(pMsg->wParam) == IDC_EDIT_PASSIV) {
                CString csImagePath = m_pButtonVisualSettings->m_csFileNames[iAction];
                int iSlashPos = csImagePath.ReverseFind('\\');
                csImagePath = csImagePath.Left(iSlashPos);

                m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_NORMAL] = csImagePath;
                m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_NORMAL] += "\\Normal.png";
                m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_OVER] = csImagePath;
                m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_OVER] += "\\MouseOver.png";
                m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_PRESSED] = csImagePath;
                m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_PRESSED] += "\\MouseDown.png";

                for (int i = 1; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
                    m_wndPreview[i].SetImage(m_pButtonVisualSettings->m_csFileNames[i]);
                    m_wndPreview[i].GetImageDimension(m_pButtonVisualSettings->m_siImageDimension[i]);
                }
            }
        }
    }

    m_wndPreview[iAction].SetText(m_csButtonText);
}

void CQuestionnaireButtonPage::OnSelchangeButtonTypes()  {
    m_nButtonType = m_wndChoosebuttonTypes.GetCurSel();

    bool bIsImageButton = m_nButtonType == 0 ? false : true;

    for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        m_wndPreview[i].IsImageButton(bIsImageButton);
        if (bIsImageButton) {
            m_wndPreview[i].SetImage(m_pButtonVisualSettings->m_csFileNames[i]);
            m_wndPreview[i].GetImageDimension(m_pButtonVisualSettings->m_siImageDimension[i]);
        } else {
            m_wndPreview[i].SetFillColor(m_pButtonVisualSettings->m_clrFill[i]);
            m_wndPreview[i].SetLineColor(m_pButtonVisualSettings->m_clrLine[i]);
        }
    }
}

void CQuestionnaireButtonPage::OnOK() {
    CEdit *pEdit = NULL;

    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DELETE);
    if (pEdit)
        pEdit->GetWindowText(m_csDelete);

    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_SEND);
    if (pEdit)
        pEdit->GetWindowText(m_csSubmit);

    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_APPROVE);
    if (pEdit)
        pEdit->GetWindowText(m_csOk);

    CPropertyPage::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// public functions

void CQuestionnaireButtonPage::Init(CQuestionnaireEx *pQuestionnaire) {
    m_nButtonType = 0;

    if (pQuestionnaire != NULL) {
        pQuestionnaire->GetInactiveButtonValues(m_pButtonVisualSettings->m_clrFill[CButtonAppearanceSettings::ACTION_PASSIV]
            , m_pButtonVisualSettings->m_clrLine[CButtonAppearanceSettings::ACTION_PASSIV]
            , m_pButtonVisualSettings->m_clrText[CButtonAppearanceSettings::ACTION_PASSIV]
            , m_pButtonVisualSettings->m_lf[CButtonAppearanceSettings::ACTION_PASSIV]
            , m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_PASSIV]);

        pQuestionnaire->GetNormalButtonValues(m_pButtonVisualSettings->m_clrFill[1]
            , m_pButtonVisualSettings->m_clrLine[CButtonAppearanceSettings::ACTION_NORMAL]
            , m_pButtonVisualSettings->m_clrText[CButtonAppearanceSettings::ACTION_NORMAL]
            , m_pButtonVisualSettings->m_lf[CButtonAppearanceSettings::ACTION_NORMAL]
            , m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_NORMAL]);

        pQuestionnaire->GetMouseoverButtonValues(m_pButtonVisualSettings->m_clrFill[CButtonAppearanceSettings::ACTION_OVER]
            , m_pButtonVisualSettings->m_clrLine[CButtonAppearanceSettings::ACTION_OVER]
            , m_pButtonVisualSettings->m_clrText[CButtonAppearanceSettings::ACTION_OVER]
            , m_pButtonVisualSettings->m_lf[CButtonAppearanceSettings::ACTION_OVER]
            , m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_OVER]);

        pQuestionnaire->GetPressedButtonValues(m_pButtonVisualSettings->m_clrFill[CButtonAppearanceSettings::ACTION_PRESSED]
            , m_pButtonVisualSettings->m_clrLine[CButtonAppearanceSettings::ACTION_PRESSED]
            , m_pButtonVisualSettings->m_clrText[CButtonAppearanceSettings::ACTION_PRESSED]
            , m_pButtonVisualSettings->m_lf[CButtonAppearanceSettings::ACTION_PRESSED]
            , m_pButtonVisualSettings->m_csFileNames[CButtonAppearanceSettings::ACTION_PRESSED]);

        m_csOk = pQuestionnaire->GetOkText();
        m_csSubmit = pQuestionnaire->GetSubmitText();
        m_csDelete = pQuestionnaire->GetDeleteText();
        m_nButtonType = pQuestionnaire->ButtonsAreImageButtons() ? 1 : 0;
    }
}

void CQuestionnaireButtonPage::GetUserEntries(COLORREF *pClrFill, COLORREF *pClrLine, 
                                              COLORREF *pClrText, LOGFONT *pLogFont,
                                              bool &bIsImageButton, CString *csFilename,
                                              CString &csOk, CString &csSubmit, CString &csDelete) {
    bIsImageButton = m_nButtonType != 0;

    for (int i = 0; i < CButtonAppearanceSettings::ACTION_EVENT_COUNT; ++i) {
        csFilename[i] = m_pButtonVisualSettings->m_csFileNames[i];
        pClrFill[i] = m_pButtonVisualSettings->m_clrFill[i];
        pClrLine[i] = m_pButtonVisualSettings->m_clrLine[i];
        pClrText[i] = m_pButtonVisualSettings->m_clrText[i];
        pLogFont[i] = m_pButtonVisualSettings->m_lf[i];
    }

    csOk = m_csOk;
    csSubmit = m_csSubmit;
    csDelete = m_csDelete;
}