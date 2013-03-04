#include "stdafx.h"
#include "EditInteractionDialog.h"
#include "InteractionButtonDialog.h"
#include "lutils.h"
#include "objects.h"
#include "MiscFunctions.h"
#include "MfcUtils.h"
#include "ButtonAppearanceSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEditInteractionDialog 

CString CEditInteractionDialog::m_csDefaultDirectory = _T("");

CEditInteractionDialog::CEditInteractionDialog(UINT nTemplateID, CWnd* pParent /*=NULL*/)
	: CDialog(nTemplateID, pParent) {
    //{{AFX_DATA_INIT(CEditInteractionDialog)
    // HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
    //}}AFX_DATA_INIT

    m_clrText = RGB(0, 0, 0);

    LFont::FillWithDefaultValues(&m_lfButton);

    m_csButton = "";

    if (m_csDefaultDirectory.IsEmpty()) {
        m_csDefaultDirectory = CButtonAppearanceSettings::GetButtonImagesPath();
    }
}

void CEditInteractionDialog::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CEditInteractionDialog)
    DDX_Control(pDX, IDC_ITALIC, m_btnItalic);
    DDX_Control(pDX, IDC_BOLD, m_btnBold);
    DDX_Control(pDX, IDC_BUTTON_PREVIEW, m_wndButtonPreview);
    DDX_Control(pDX, IDC_SIZE_LIST, m_fontsizeList);
    DDX_Control(pDX, IDC_FONT_LIST, m_fontfamilyList);
    DDX_Control(pDX, IDC_BUTTON_TEXT_COLOR, m_btnTextColor);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditInteractionDialog, CDialog)
    //{{AFX_MSG_MAP(CEditInteractionDialog)
    ON_MESSAGE_VOID( WM_IDLEUPDATECMDUI, OnIdleUpdateUIMessage )
    ON_CPN_XT_SELENDOK(IDC_BUTTON_TEXT_COLOR, OnSelEndOkTextClr)
    ON_BN_CLICKED(IDC_BOLD, OnBold)
    ON_UPDATE_COMMAND_UI(IDC_BOLD, OnUpdateBold)
    ON_BN_CLICKED(IDC_ITALIC, OnItalic)
    ON_UPDATE_COMMAND_UI(IDC_ITALIC, OnUpdateItalic)
    ON_CBN_SELCHANGE(IDC_FONT_LIST, OnSelchangeFontList)
    ON_CBN_SELCHANGE(IDC_SIZE_LIST, OnSelchangeSizeList)
    ON_BN_CLICKED(IDC_BUTTON_TEXT_COLOR, OnTextColor)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEditInteractionDialog 

BOOL CEditInteractionDialog::OnInitDialog() {
    CDialog::OnInitDialog();

    CString csTitle;
    csTitle.LoadString(IDD_EDIT_INTERACTIONBUTTON);
    SetWindowText(csTitle);

    DWORD dwIds[] = {IDOK,
        IDCANCEL,
        -1};
    MfcUtils::Localize(this, dwIds);
    // set current status
    CWnd *pFrameWnd = GetDlgItem(IDC_PREVIEW);
    pFrameWnd->SetWindowText(m_csTitle);

    // initialize preview window
    m_wndButtonPreview.SetText(m_csButton);
    m_wndButtonPreview.SetButtonFont(&m_lfButton);
    m_wndButtonPreview.SetTextColor(m_clrText);

    // initialize color buttons
    CString csBtnText;
    csBtnText.LoadString(IDS_TEXTCOLOR);
    m_btnTextColor.SetWindowText(csBtnText);
    m_btnTextColor.ShowText(true);
    m_btnTextColor.SetColor(m_clrText);

    // icon for bold
    m_hIconBold = static_cast<HICON>(LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_BUTTON_BOLD),
        IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR));

    // icon for italic
    m_hIconItalic = static_cast<HICON>(LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_BUTTON_ITALIC),
        IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR));

    m_btnBold.SetIcon(m_hIconBold);

    m_btnItalic.SetIcon(m_hIconItalic);

    // initialize font family and font size combo box
    m_fontfamilyList.InitControl(m_lfButton.lfFaceName);

    int nFontSizes[] = {8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};
    int nCount = 16;
    for (int i=0; i < 16; i++) {
        CString csEntry;
        csEntry.Format(_T("%d"), nFontSizes[i]);
        m_fontsizeList.AddString(csEntry);
        m_fontsizeList.SetItemData(i, nFontSizes[i]);
        if (nFontSizes[i] <= abs(m_lfButton.lfHeight))
            m_fontsizeList.SetCurSel(i);
    }


    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CEditInteractionDialog::OnIdleUpdateUIMessage() {
    UpdateDialogControls( this, TRUE );
}

BOOL CEditInteractionDialog::ContinueModal() {
    SendMessage( WM_IDLEUPDATECMDUI, WPARAM(TRUE), 0);
    return CDialog::ContinueModal();
}

void CEditInteractionDialog::Init(CString &csTitle, CString &csButton, CRect &rcArea,  
                                  COLORREF clrText, LOGFONT *pLfButton) {
    m_csTitle = csTitle;
    m_csButton = csButton;
    m_clrText = clrText;
    m_rcArea = rcArea;

    if (pLfButton)
        memcpy(&m_lfButton, pLfButton, sizeof(LOGFONT));
}

void CEditInteractionDialog::SetUserEntries(CRect &rcArea, COLORREF &clrText, LOGFONT *pLfButton) {
    rcArea = m_rcArea;
    clrText = m_clrText;
    memcpy(pLfButton, &m_lfButton, sizeof(LOGFONT));
}

void CEditInteractionDialog::OnBold() {
    if (m_lfButton.lfWeight == FW_NORMAL)
        m_lfButton.lfWeight = FW_BOLD;
    else
        m_lfButton.lfWeight = FW_NORMAL;

    m_wndButtonPreview.SetButtonFont(&m_lfButton);
}

void CEditInteractionDialog::OnUpdateBold(CCmdUI *pCmdUI) {
    if (m_lfButton.lfWeight == FW_BOLD)
        m_btnBold.SetPressed(TRUE);
    else
        m_btnBold.SetPressed(FALSE);
}

void CEditInteractionDialog::OnItalic() {
    if (m_lfButton.lfItalic)
        m_lfButton.lfItalic = FALSE;
    else
        m_lfButton.lfItalic = TRUE;

    m_wndButtonPreview.SetButtonFont(&m_lfButton);
}

void CEditInteractionDialog::OnUpdateItalic(CCmdUI *pCmdUI) {
    if (m_lfButton.lfItalic)
        m_btnItalic.SetPressed(TRUE);
    else
        m_btnItalic.SetPressed(FALSE);
}

void CEditInteractionDialog::OnSelchangeFontList() {
    /*XT_LOGFONT*/CXTLogFont lfButton;
    m_fontfamilyList.GetSelFont(lfButton);
    _tcscpy(m_lfButton.lfFaceName, lfButton.lfFaceName);
    m_wndButtonPreview.SetButtonFont(&m_lfButton);

    // update bounding box
    UINT nTextWidth, nTextHeight;
    DrawSdk::Text::GetTextDimension(m_csButton, m_csButton.GetLength(), &m_lfButton, nTextWidth, nTextHeight);
    m_rcArea.right = m_rcArea.left + nTextWidth;
    m_rcArea.bottom = m_rcArea.top + nTextHeight;
    m_wndButtonPreview.RedrawWindow();
}

void CEditInteractionDialog::OnSelchangeSizeList() {
    int iSelection = m_fontsizeList.GetCurSel();
    UINT nHeight = m_fontsizeList.GetItemData(iSelection);
    m_lfButton.lfHeight = -1 * nHeight;
    m_wndButtonPreview.SetButtonFont(&m_lfButton);

    // update bounding box
    UINT nTextWidth, nTextHeight;
    DrawSdk::Text::GetTextDimension(m_csButton, m_csButton.GetLength(), &m_lfButton, nTextWidth, nTextHeight);
    if (nTextWidth > (UINT)m_rcArea.Width())
        m_rcArea.right = m_rcArea.left + nTextWidth;
    if (nTextHeight > (UINT)m_rcArea.Height())
        m_rcArea.bottom = m_rcArea.top + nTextHeight;
    m_wndButtonPreview.RedrawWindow();
}

void CEditInteractionDialog::OnSelEndOkTextClr() {
    m_clrText = m_btnTextColor.GetColor();
    m_wndButtonPreview.SetTextColor(m_clrText);
}


void CEditInteractionDialog::OnTextColor() {
}

/*****/


CEditInteractionTextDialog::CEditInteractionTextDialog(CWnd* pParent /*=NULL*/)
    : CEditInteractionDialog(CEditInteractionTextDialog::IDD, pParent) {
    //{{AFX_DATA_INIT(CEditInteractionTextDialog)
    // HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
    //}}AFX_DATA_INIT

    m_clrFill = RGB(255, 255, 255);
    m_clrLine = RGB(0, 0, 0);
    m_rcRange.SetRectEmpty();
}

CEditInteractionTextDialog::CEditInteractionTextDialog(UINT nTemplateId, CWnd* pParent /*= NULL*/)
: CEditInteractionDialog(CEditInteractionTextDialog::IDD, pParent) {

    m_clrFill = RGB(255, 255, 255);
    m_clrLine = RGB(0, 0, 0);
    m_rcRange.SetRectEmpty();
}


void CEditInteractionTextDialog::DoDataExchange(CDataExchange* pDX)
{
    CEditInteractionDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CEditInteractionTextDialog)
    DDX_Control(pDX, IDC_BUTTON_LINE_COLOR, m_btnLineColor);
    DDX_Control(pDX, IDC_BUTTON_FILL_COLOR, m_btnFillColor);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditInteractionTextDialog, CEditInteractionDialog)
    //{{AFX_MSG_MAP(CEditInteractionTextDialog)
    ON_CPN_XT_SELENDOK(IDC_BUTTON_LINE_COLOR, OnSelEndOkLineClr)
    ON_CPN_XT_SELENDOK(IDC_BUTTON_FILL_COLOR, OnSelEndOkFillClr)
    ON_BN_CLICKED(IDC_BUTTON_LINE_COLOR, OnLineColor)
    ON_BN_CLICKED(IDC_BUTTON_FILL_COLOR, OnFillColor)
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_IBUTTON_RESET, &CEditInteractionTextDialog::OnBnClickedIbuttonReset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEditInteractionDialog 

BOOL CEditInteractionTextDialog::OnInitDialog() {
    CEditInteractionDialog::OnInitDialog();

    // set language dependant button text
    CString csBtnText;

    m_wndButtonPreview.IsImageButton(false);

    csBtnText.LoadString(IDS_LINECOLOR);
    m_btnLineColor.SetWindowText(csBtnText);
    m_btnLineColor.ShowText(true);
    m_btnLineColor.SetColor(m_clrLine);
    m_wndButtonPreview.SetLineColor(m_clrLine);

    csBtnText.LoadString(IDS_FILLCOLOR);
    m_btnFillColor.SetWindowText(csBtnText);
    m_btnFillColor.ShowText(true);
    m_btnFillColor.SetColor(m_clrFill);
    m_wndButtonPreview.SetFillColor(m_clrFill);

    CString csResetText;
    csResetText.LoadString(IDC_GODEF_BUTTON_RESET);
    GetDlgItem(IDC_IBUTTON_RESET)->SetWindowText(csResetText);


    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CEditInteractionTextDialog::Init(CString &csTitle, CString &csButton, CRect &rcArea, 
                                  COLORREF clrFill, COLORREF clrLine, COLORREF clrText,
                                  LOGFONT *pLfButton) {
    CEditInteractionDialog::Init(csTitle, csButton, rcArea, clrText, pLfButton);
 
    m_clrFill = clrFill;
    m_clrLine = clrLine;
}

void CEditInteractionTextDialog::SetUserEntries(CRect &rcArea, COLORREF &clrFill, COLORREF &clrLine, 
                                            COLORREF &clrText, LOGFONT *pLfButton) {
    CEditInteractionDialog::SetUserEntries(rcArea, clrText, pLfButton);

    clrFill = m_clrFill;
    clrLine = m_clrLine;
}

void CEditInteractionTextDialog::OnSelEndOkLineClr() {
    m_clrLine = m_btnLineColor.GetColor();
    m_wndButtonPreview.SetLineColor(m_clrLine);
}

void CEditInteractionTextDialog::OnSelEndOkFillClr() {
    m_clrFill = m_btnFillColor.GetColor();
    m_wndButtonPreview.SetFillColor(m_clrFill);
}

void CEditInteractionTextDialog::OnLineColor() {
}

void CEditInteractionTextDialog::OnFillColor() {
}

void CEditInteractionTextDialog::OnBnClickedIbuttonReset()
{
    // TODO: Add your control notification handler code here
    CString csPassive;
    csPassive.LoadString(IDS_PREVIEW_PASSIV);
    CString csNormal;
    csNormal.LoadString(IDS_PREVIEW_NORMAL);
    CString csOver;
    csOver.LoadString(IDS_PREVIEW_MOUSEOVER);
    CString csPressed;
    csPressed.LoadString(IDS_PREVIEW_PRESSED);

    _tcscpy(m_lfButton.lfFaceName, _T("Segoe UI"));
    m_lfButton.lfHeight = -16;
    m_lfButton.lfItalic = 0;
    m_lfButton.lfWeight = FW_NORMAL;
    

    m_btnBold.SetPressed(false);
    m_btnBold.RedrawWindow();
    m_btnItalic.SetPressed(false);
    m_btnItalic.RedrawWindow();
    

    m_fontfamilyList.InitControl(m_lfButton.lfFaceName);

    int nFontSizes[] = {8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};
    int nCount = 16;
    for (int i=0; i < 16; i++) {
      
        if (nFontSizes[i] <= abs(m_lfButton.lfHeight))
            m_fontsizeList.SetCurSel(i);
    }

    
    m_wndButtonPreview.SetButtonFont(&m_lfButton);

    if (m_csTitle == csPassive){
        m_clrFill = RGB(226, 255, 138);//RGB(171, 199, 238);  
        m_clrLine = RGB(140, 178, 32);//RGB(163, 193, 245);
        m_clrText = RGB(159, 174, 204);
    } else if (m_csTitle == csNormal){
        m_clrFill = RGB(255, 255, 255);//RGB(171, 199, 238);  
        m_clrLine = RGB(140, 178, 32);//RGB(163, 193, 245);
        m_clrText = RGB(140, 178, 32);
    } else if (m_csTitle == csOver) {
        m_clrFill = RGB(234, 234, 234);//RGB(255, 228, 176);  
        m_clrLine = RGB(140, 178, 32);//RGB(0, 0, 128);
        m_clrText = RGB(140, 178, 32);
    } else if (m_csTitle == csPressed){
        m_clrFill = RGB(178, 178, 178);  
        m_clrLine = RGB(140, 178, 32);//RGB(0, 0, 128);
        m_clrText = RGB(140, 178, 32);
    }
    m_btnTextColor.SetColor(m_clrText);
    m_btnLineColor.SetColor(m_clrLine);
    m_btnFillColor.SetColor(m_clrFill);
    m_wndButtonPreview.SetTextColor(m_clrText);
    m_wndButtonPreview.SetLineColor(m_clrLine);
    m_wndButtonPreview.SetFillColor(m_clrFill);
}

/*****/


CEditInteractionButtonDialog::CEditInteractionButtonDialog(CWnd* pParent /*=NULL*/)
    : CEditInteractionDialog(CEditInteractionButtonDialog::IDD, pParent) {
    //{{AFX_DATA_INIT(CEditInteractionButtonDialog)
    // HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
    //}}AFX_DATA_INIT
}


void CEditInteractionButtonDialog::DoDataExchange(CDataExchange* pDX) {
	CEditInteractionDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditInteractionButtonDialog)
	DDX_Control(pDX, IDC_CHOOSE_FILE, m_btnChooseFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditInteractionButtonDialog, CEditInteractionDialog)
	//{{AFX_MSG_MAP(CEditInteractionButtonDialog)
	ON_COMMAND(IDC_CHOOSE_FILE, OnChooseFile)
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_IBUTTON_RESET, &CEditInteractionButtonDialog::OnBnClickedIbuttonReset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEditInteractionDialog 

BOOL CEditInteractionButtonDialog::OnInitDialog() {
    CEditInteractionDialog::OnInitDialog();

    DWORD dwIds[] = {IDC_CHOOSE_FILE,
        -1};
    MfcUtils::Localize(this, dwIds);

    // set language dependant button text

    m_wndButtonPreview.IsImageButton(true);
    m_wndButtonPreview.SetImage(m_csImageFileName);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CEditInteractionButtonDialog::Init(CString &csTitle, CString &csButton, CRect &rcArea,
                                        CString &csImageFile, COLORREF clrText,
                                        LOGFONT *pLfButton) {
    CEditInteractionDialog::Init(csTitle, csButton, rcArea, clrText, pLfButton);
    m_csImageFileName = csImageFile;
}

void CEditInteractionButtonDialog::SetUserEntries(CRect &rcArea, CString &csImageFile, 
                                                  COLORREF &clrText, LOGFONT *pLfButton) {
    CEditInteractionDialog::SetUserEntries(rcArea, clrText, pLfButton);
    csImageFile = m_csImageFileName;
}

void CEditInteractionButtonDialog::OnChooseFile() {
    CString csFilter;
    csFilter.LoadString(IDS_IMAGE_FILTER_E);

    CFileDialog *pFileDialog = new CFileDialog(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, csFilter, this);
    pFileDialog->m_ofn.lpstrInitialDir = m_csDefaultDirectory;

    if (pFileDialog->DoModal() == IDOK) {
        m_csImageFileName = pFileDialog->GetPathName();

        m_csDefaultDirectory = m_csImageFileName;
        StringManipulation::GetPath(m_csDefaultDirectory);

        m_wndButtonPreview.SetImage(m_csImageFileName);
    }

    delete pFileDialog;
}

CString CEditInteractionButtonDialog::GetButtonImagesPath() {
    CString csButtonPath;

    unsigned long ulMaxLength = MAX_PATH;
    _TCHAR tszString[MAX_PATH];
    bool bSuccess = LRegistry::ReadSettingsString(_T(""), _T("InstallDir"), tszString, &ulMaxLength, NULL);
    if (bSuccess) {
        csButtonPath = tszString;
        csButtonPath += _T("Studio\\ButtonImages");

        if (_taccess(csButtonPath, 00) != 0) {
            ::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), 
                _T("Unexpected: Button directory does not exist."), _T("LECTURNITY"), MB_OK | MB_ICONERROR);
            csButtonPath = _T("");
        }
    }

    return csButtonPath;
}

void CEditInteractionButtonDialog::OnBnClickedIbuttonReset()
{
    // TODO: Add your control notification handler code here
    CString csPassive;
    csPassive.LoadString(IDS_PREVIEW_PASSIV);
    CString csNormal;
    csNormal.LoadString(IDS_PREVIEW_NORMAL);
    CString csOver;
    csOver.LoadString(IDS_PREVIEW_MOUSEOVER);
    CString csPressed;
    csPressed.LoadString(IDS_PREVIEW_PRESSED);

     _tcscpy(m_lfButton.lfFaceName, _T("Segoe UI"));
    m_lfButton.lfHeight = -16;
    m_lfButton.lfItalic = 0;
    m_lfButton.lfWeight = FW_NORMAL;
    

    m_btnBold.SetPressed(false);
    m_btnBold.RedrawWindow();
    m_btnItalic.SetPressed(false);
    m_btnItalic.RedrawWindow();
    

    m_fontfamilyList.InitControl(m_lfButton.lfFaceName);

    int nFontSizes[] = {8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};
    int nCount = 16;
    for (int i=0; i < 16; i++) {
      
        if (nFontSizes[i] <= abs(m_lfButton.lfHeight))
            m_fontsizeList.SetCurSel(i);
    }

    
    m_wndButtonPreview.SetButtonFont(&m_lfButton);

    //TODO Select default images
    CString csButtonImagePath = GetButtonImagesPath();
    if(!csButtonImagePath.IsEmpty()) {
        m_csDefaultDirectory = csButtonImagePath;
        csButtonImagePath += _T("\\Metro\\Medium\\");
        
        if (m_csTitle == csPassive) {
            m_csImageFileName = csButtonImagePath + _T("Passive.png");
        } else if (m_csTitle == csNormal) {
            m_csImageFileName = csButtonImagePath + _T("Normal.png");
        } else if (m_csTitle == csOver) {
            m_csImageFileName = csButtonImagePath + _T("MouseOver.png");
        } else if (m_csTitle == csPressed) {
            m_csImageFileName = csButtonImagePath + _T("MouseDown.png");
        }
        m_wndButtonPreview.SetImage(m_csImageFileName);
    }
    return;
   
}
// C:\LEC_code\LEC_trunk_0222\lecturnity\editor\EditInteractionDialog.cpp : implementation file
//

//#include "stdafx.h"
//#include "Studio.h"
//#include "C:\LEC_code\LEC_trunk_0222\lecturnity\editor\EditInteractionDialog.h"


// CEditGraphicalObjectDefaults dialog

//IMPLEMENT_DYNAMIC(CEditGraphicalObjectDefaults, CDialog)

CEditGraphicalObjectDefaults::CEditGraphicalObjectDefaults(CWnd* pParent /*=NULL*/)
	: CEditInteractionDialog(CEditGraphicalObjectDefaults::IDD, pParent)
{

}

CEditGraphicalObjectDefaults::~CEditGraphicalObjectDefaults()
{
}

void CEditGraphicalObjectDefaults::DoDataExchange(CDataExchange* pDX)
{
    CEditInteractionDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CEditGraphicalObjectDefaults)
    DDX_Control(pDX, IDC_BUTTON_LINE_COLOR, m_btnLineColor);
    DDX_Control(pDX, IDC_BUTTON_FILL_COLOR, m_btnFillColor);
    //}}AFX_DATA_MAP

    DDX_Control(pDX, IDC_GODEF_STATIC_LW, m_stLineWidth);
    DDX_Control(pDX, IDC_GODEF_STATIC_LS, m_stLineStyle);
    DDX_Control(pDX, IDC_GODEF_COMBO_LW, m_cmbLineWidth);
    DDX_Control(pDX, IDC_GODEF_COMBO_LS, m_cmbLineStyle);
    DDX_Control(pDX, IDC_GODEF_BUTTON_RESET, m_btnReset);
}


BEGIN_MESSAGE_MAP(CEditGraphicalObjectDefaults, CEditInteractionDialog)
    //{{AFX_MSG_MAP(CEditGraphicalObjectDefaults)
    ON_CPN_XT_SELENDOK(IDC_BUTTON_LINE_COLOR, OnSelEndOkLineClr)
    ON_CPN_XT_SELENDOK(IDC_BUTTON_FILL_COLOR, OnSelEndOkFillClr)
    ON_BN_CLICKED(IDC_BUTTON_LINE_COLOR, OnLineColor)
    ON_BN_CLICKED(IDC_BUTTON_FILL_COLOR, OnFillColor)
    //}}AFX_MSG_MAP
    ON_CBN_SELCHANGE(IDC_GODEF_COMBO_LS, &CEditGraphicalObjectDefaults::OnCbnSelchangeGodefComboLs)
    ON_CBN_SELCHANGE(IDC_GODEF_COMBO_LW, &CEditGraphicalObjectDefaults::OnCbnSelchangeGodefComboLw)
    ON_BN_CLICKED(IDC_GODEF_BUTTON_RESET, &CEditGraphicalObjectDefaults::OnBnClickedGodefButtonReset)
END_MESSAGE_MAP()

BOOL CEditGraphicalObjectDefaults::OnInitDialog() {
    CEditInteractionDialog::OnInitDialog();

    CString csTitle;
    csTitle.LoadString(IDS_GODEF_DILOG_TITLE);
    SetWindowText(csTitle);
    CString csBtnText;

    CWnd *pFrameWnd = GetDlgItem(IDC_PREVIEW);
    pFrameWnd->SetWindowText(_T(""));

    m_wndButtonPreview.IsImageButton(false);

    csBtnText.LoadString(IDS_LINECOLOR);
    m_btnLineColor.SetWindowText(csBtnText);
    m_btnLineColor.ShowText(true);
    m_btnLineColor.SetColor(m_clrLine);
    m_wndButtonPreview.SetLineColor(m_clrLine);

    csBtnText.LoadString(IDS_FILLCOLOR);
    m_btnFillColor.SetWindowText(csBtnText);
    m_btnFillColor.ShowText(true);
    m_btnFillColor.SetColor(m_clrFill);
    m_wndButtonPreview.SetFillColor(m_clrFill);

    CString csLabelText;
    csLabelText.LoadString(ID_LINE_WIDTH);
    m_stLineWidth.SetWindowText(csLabelText);
    csLabelText.LoadString(ID_LINE_STYLE);
    m_stLineStyle.SetWindowText(csLabelText);

    int aiLineWidth[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for (int i = 0; i < 10; i++) {
        CString csText;
        csText.Format(_T("%d"), aiLineWidth[i]);
        m_cmbLineWidth.AddString(csText);
    }
    m_cmbLineWidth.SetCurSel(m_iLineWidth - 1);
    m_wndButtonPreview.SetLineWidth(m_iLineWidth);

    CString csStyleText;
    csStyleText.LoadString(IDS_LS_SOLID);
    m_cmbLineStyle.AddString(csStyleText);
    csStyleText.LoadString(IDS_LS_DASH_E);
    m_cmbLineStyle.AddString(csStyleText);
    csStyleText.LoadString(IDS_LS_LONGDASH);
    m_cmbLineStyle.AddString(csStyleText);
    m_cmbLineStyle.SetCurSel(m_iLineStyle);
    m_wndButtonPreview.SetLineStyle(m_iLineStyle);

    CString csResetText;
    csResetText.LoadString(IDC_GODEF_BUTTON_RESET);
    m_btnReset.SetWindowText(csResetText);

    return TRUE;
}

void CEditGraphicalObjectDefaults::Init(CString &csTitle, CString &csButton, CRect &rcArea, COLORREF clrFill, COLORREF clrLine, COLORREF clrText, LOGFONT *pLfButton, int iLineWidth, int iLineStyle) {

    CEditInteractionDialog::Init(csTitle, csButton, rcArea, clrText, pLfButton);
    m_clrFill = clrFill;
    m_clrLine = clrLine;
    m_iLineWidth = iLineWidth;
    m_iLineStyle = iLineStyle;
}

void CEditGraphicalObjectDefaults::OnSelEndOkLineClr() {
    m_clrLine = m_btnLineColor.GetColor();
    m_wndButtonPreview.SetLineColor(m_clrLine);
}

void CEditGraphicalObjectDefaults::OnSelEndOkFillClr() {
    m_clrFill = m_btnFillColor.GetColor();
    m_wndButtonPreview.SetFillColor(m_clrFill);
}

void CEditGraphicalObjectDefaults::OnLineColor() {
}

void CEditGraphicalObjectDefaults::OnFillColor() {
}

void CEditGraphicalObjectDefaults::GetUserEntries(COLORREF &clrText, COLORREF &clrLine, COLORREF &clrFill, LOGFONT &lf, int &iLineWidth, int &iLineStyle) {
    clrText = m_clrText;
    clrLine = m_clrLine;
    clrFill = m_clrFill;
    lf = m_lfButton;
    iLineWidth = m_iLineWidth;
    iLineStyle = m_iLineStyle;
}

// CEditGraphicalObjectDefaults message handlers

void CEditGraphicalObjectDefaults::OnCbnSelchangeGodefComboLs()
{
    // TODO: Add your control notification handler code here
    m_iLineStyle = m_cmbLineStyle.GetCurSel();
    m_wndButtonPreview.SetLineStyle(m_iLineStyle);
}

void CEditGraphicalObjectDefaults::OnCbnSelchangeGodefComboLw()
{
    // TODO: Add your control notification handler code here
    m_iLineWidth = m_cmbLineWidth.GetCurSel() + 1;
    m_wndButtonPreview.SetLineWidth(m_iLineWidth);
}

void CEditGraphicalObjectDefaults::OnBnClickedGodefButtonReset()
{
    // TODO: Add your control notification handler code here
    m_clrText = RGB(0x8C, 0xB2, 0x20);//0xFFFFFF;
    m_clrLine = RGB(0x8C, 0xB2, 0x20);//0x385D8A;
    m_clrFill = RGB(0xFF, 0xFF, 0xFF);//0x4F81BD;
    //m_lfButton.lfFaceName = _T("Arial");
    _tcscpy(m_lfButton.lfFaceName, _T("Segoe UI"));
    m_lfButton.lfHeight = -18;
    m_lfButton.lfItalic = 0;
    m_lfButton.lfWeight = FW_NORMAL;
    m_iLineWidth = 1;
    m_iLineStyle = 0;

    m_btnBold.SetPressed(false);
    m_btnBold.RedrawWindow();
    m_btnItalic.SetPressed(false);
    m_btnItalic.RedrawWindow();
    m_btnTextColor.SetColor(m_clrText);
    m_btnLineColor.SetColor(m_clrLine);
    m_btnFillColor.SetColor(m_clrFill);
    m_cmbLineWidth.SetCurSel(m_iLineWidth - 1);
    m_cmbLineStyle.SetCurSel(m_iLineStyle);

     m_fontfamilyList.InitControl(m_lfButton.lfFaceName);

    int nFontSizes[] = {8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};
    int nCount = 16;
    for (int i=0; i < 16; i++) {
        /*CString csEntry;
        csEntry.Format(_T("%d"), nFontSizes[i]);
        m_fontsizeList.AddString(csEntry);
        m_fontsizeList.SetItemData(i, nFontSizes[i]);*/
        if (nFontSizes[i] <= abs(m_lfButton.lfHeight))
            m_fontsizeList.SetCurSel(i);
    }

    m_wndButtonPreview.SetTextColor(m_clrText);
    m_wndButtonPreview.SetLineColor(m_clrLine);
    m_wndButtonPreview.SetFillColor(m_clrFill);
    m_wndButtonPreview.SetButtonFont(&m_lfButton);
    m_wndButtonPreview.SetLineWidth(m_iLineWidth);
    m_wndButtonPreview.SetLineStyle(m_iLineStyle);

}

