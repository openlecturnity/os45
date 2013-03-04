#include "stdafx.h"
#include "MfcUtils.h"
#include "StructureEditor.h"
#include "StructureEditorView.h"

#ifndef SAFE_DELETE_WINDOW
#define SAFE_DELETE_WINDOW(ptr) if(ptr){ptr->DestroyWindow(); delete ptr; ptr=NULL;}
#endif

static void TranslateMsecToTimeString(int msec, CString &timeString) {
    _TCHAR timebuf[128];

    int totSec = msec / 1000;
    int min  = totSec / 60;
    int sec  = totSec % 60;

    _stprintf(timebuf, _T("%02d:%02d"), min, sec);  

    timeString = timebuf;
}

// CStructureEditor

IMPLEMENT_DYNAMIC(CStructureEditor, CDialog)

BEGIN_MESSAGE_MAP(CStructureEditor, CDialog)
   ON_BN_CLICKED(IDC_STD_INDEX, &CStructureEditor::OnBnClickedStdIndex)
   ON_BN_CLICKED(IDC_STD_INDEX_ALWAYS, &CStructureEditor::OnBnClickedStdIndexAlways)
END_MESSAGE_MAP()

CStructureEditor::CStructureEditor(CscreengrabbingDoc *pScreengrabbingDoc, CWnd *pParent) : CDialog(CStructureEditor::IDD, pParent) { 
    ASSERT(pScreengrabbingDoc);
    m_pScreengrabbingDoc = pScreengrabbingDoc;
    
    m_pStructureView = NULL;
}

CStructureEditor::~CStructureEditor() {
    m_pScreengrabbingDoc = NULL;
    m_pFont = NULL;
};

void CStructureEditor::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STD_INDEX, m_btnStandardIndexInLmd);
    DDX_Control(pDX, IDC_STD_INDEX_ALWAYS, m_btnStandardIndexInRegistry);
    DDX_Control(pDX, IDC_GROUPBOX, m_wndGroupBox);
    DDX_Control(pDX, IDC_STRUCTURE_VIEW, m_wndStructureView);
}

BOOL CStructureEditor::OnInitDialog() {
    CDialog::OnInitDialog();

    TRACE("STRUCTURE EDITOR\n");

    // localization
    CString csDialogTitle;
    csDialogTitle.LoadString(IDD_STRUCTEDITOR_DIALOG);
    SetWindowText(csDialogTitle);

    DWORD dwIds[] = { IDC_STRUCTEDITOR_HEADER,
        IDC_STRUCTEDITOR_HINT,
        IDOK,
        IDCANCEL,
        IDC_STD_INDEX_ALWAYS,
        IDC_STD_INDEX,
        IDC_CLICKPAGES_GROUPBOX,
        -1};
    MfcUtils::Localize(this, dwIds);

    
    // create button box and adjust checkboxes according to state
    CreateCheckButtonBox();

    // Create view to display page structure
    CreatePageStructureView();

    return true;
}

void CStructureEditor::OnOK() {
    
    CArray<CPageEntry *, CPageEntry *> aPageEntries;
    if (m_pStructureView != NULL)
        m_pStructureView->GetPageEntries(aPageEntries);

    // set new page titles and remove marked pages
    // note: decreasing loop to avoid numbering problems while removing pages 
    for (int iPageNr = m_pScreengrabbingDoc->GetNumberOfPages(); iPageNr > 0; iPageNr--) {
        CPageEntry *pPageEntry = aPageEntries.GetAt(iPageNr-1);

        if(pPageEntry->DoRemove()) {
            // free thumb image (otherwise file is blocked and cannot be deleted)
            pPageEntry->ReleaseThumbImage();
            // delete thumbnails file(s)
            m_pScreengrabbingDoc->DeleteThumbnailFiles(iPageNr);      

            // delete page index
            m_pScreengrabbingDoc->RemovePage(iPageNr);      
        } else {
            // set title of page
            CString csTitle = pPageEntry->GetTitle();
            m_pScreengrabbingDoc->SetPageTitle(iPageNr, csTitle);
        }
    }
	
	// BUGFIX 5866: click state written on Cancel
    // store click page state of document in OnOK()
    bool bShowClickPages = IsDlgButtonChecked(IDC_STD_INDEX) == BST_UNCHECKED;
    m_pScreengrabbingDoc->ShowClickPages( bShowClickPages );

    // store click page default state in registry
    UINT checked = IsDlgButtonChecked(IDC_STD_INDEX_ALWAYS);
    LRegistry::WriteBoolRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Assistant\\SgOptions\\"), 
        _T("ShowClickPages"), checked == BST_UNCHECKED);

    EndDialog(IDOK);
}

// required for tooltips to work
BOOL CStructureEditor::PreTranslateMessage(MSG* pMsg) {
   return CDialog::PreTranslateMessage(pMsg);
}

void CStructureEditor::OnSetFont(CFont *pFont) {
    m_pFont = pFont;
    if (m_pStructureView != NULL)
        m_pStructureView->SetFont(pFont);
}


// event handle for the buttons
void CStructureEditor::OnBnClickedStdIndex() {
    // store document status
    bool bShowClickPages = IsDlgButtonChecked(IDC_STD_INDEX) == BST_UNCHECKED;
	
	// BUGFIX 5866: click state written on Cancel
    // store click page state of document in OnOK()
    // m_pScreengrabbingDoc->ShowClickPages( bShowClickPages );

    // update GUI
    if (m_pStructureView != NULL) 
        m_pStructureView->UpdatePageList(!bShowClickPages);
}

void CStructureEditor::OnBnClickedStdIndexAlways()
{
   // adjust other checkbox
   CButton* pCheckButtonStdIndex = (CButton*) GetDlgItem(IDC_STD_INDEX);
   UINT checked = IsDlgButtonChecked(IDC_STD_INDEX_ALWAYS);
   pCheckButtonStdIndex->SetCheck(checked);
   pCheckButtonStdIndex->EnableWindow(checked != BST_CHECKED);

   // update document status and GUI
   OnBnClickedStdIndex();
}

void CStructureEditor::CreateCheckButtonBox() {
    // read state from registry
    bool bShowClickPagesRegistry;
    bool bRet = LRegistry::ReadBoolRegistryEntry(HKEY_CURRENT_USER, 
        _T("Software\\imc AG\\LECTURNITY\\Assistant\\SgOptions\\"), 
        _T("ShowClickPages"), &bShowClickPagesRegistry);
    if ( !bRet )
        bShowClickPagesRegistry = true;

    bool bShowClickPages = bShowClickPagesRegistry && m_pScreengrabbingDoc->IsShowClickPages();

    CButton* pCheckButtonStdIndexAlways = (CButton*) GetDlgItem(IDC_STD_INDEX_ALWAYS);
    pCheckButtonStdIndexAlways->SetCheck( bShowClickPagesRegistry ? BST_UNCHECKED : BST_CHECKED);
    CButton* pCheckButtonStdIndex = (CButton*) GetDlgItem(IDC_STD_INDEX);
    pCheckButtonStdIndex->SetCheck( bShowClickPages ? BST_UNCHECKED : BST_CHECKED);
    pCheckButtonStdIndex->EnableWindow( bShowClickPagesRegistry );
}

void CStructureEditor::FillPageList(CArray<CPageEntry *, CPageEntry *> &aPageEntries) {

    if (m_pScreengrabbingDoc == NULL)
        return;

    aPageEntries.RemoveAll();

    for (int iPageNr = 1; iPageNr <= m_pScreengrabbingDoc->GetNumberOfPages(); iPageNr++) {
        CPageEntry *pPageEntry = new CPageEntry();
        pPageEntry->SetPageNumber(iPageNr);
        pPageEntry->SetTitle(m_pScreengrabbingDoc->GetPageTitle(iPageNr));
        pPageEntry->SetBegin(m_pScreengrabbingDoc->GetPageBeginTime(iPageNr));
        pPageEntry->SetThumbImage(m_pScreengrabbingDoc->GetThumbnailFilename(iPageNr));
        pPageEntry->SetIsClickPage(m_pScreengrabbingDoc->IsClickPage(iPageNr));
        aPageEntries.Add(pPageEntry);
    }
}

bool CStructureEditor::ClickPagesAreVisible()  {
    bool bClickPagesAreVisible = false;

	// BUGFIX 5866: click state written on Cancel
    // read click state from button because document state is not wriiten yet (will be in OnOK())
    bClickPagesAreVisible = IsDlgButtonChecked(IDC_STD_INDEX) == BST_UNCHECKED;
    //if (m_pScreengrabbingDoc != NULL)
    //    bClickPagesAreVisible = m_pScreengrabbingDoc->IsShowClickPages();

    return bClickPagesAreVisible;
}

void CStructureEditor::CreatePageStructureView() {   

    // create scroll view
    CRect rcStructureView;
    m_wndStructureView.GetClientRect(&rcStructureView);
    m_pStructureView = new CStructureEditorView();
    m_pStructureView->SetListener(this);
    m_pStructureView->SetFont(m_pFont);
    m_pStructureView->Create(NULL, _T("Pages"), WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP, 
                             rcStructureView, &m_wndStructureView, (UINT)-1);

    m_pStructureView->UpdatePageList(!m_pScreengrabbingDoc->IsShowClickPages());
}
