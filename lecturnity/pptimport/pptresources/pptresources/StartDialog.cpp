// StartDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "MfcUtils.h"
#include "pptresources.h"
#include "StartDialog.h"


// CStartDialog-Dialogfeld

IMPLEMENT_DYNAMIC(CStartDialog, CDialog)

CStartDialog::CStartDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CStartDialog::IDD, pParent)
{
	m_bIsVisible = true;
}

CStartDialog::~CStartDialog()
{
}

void CStartDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_TITLE_LABEL, m_titleLabel);
}


BEGIN_MESSAGE_MAP(CStartDialog, CDialog)
   ON_BN_CLICKED(IDCANCEL, &CStartDialog::OnBnClickedCancel)
   ON_UPDATE_COMMAND_UI(IDC_SELECT_ALL_SLIDES, OnUpdateSelectAllSlides)
   ON_UPDATE_COMMAND_UI(IDC_SELECT_CURRENT_SLIDE, OnUpdateSelectCurrentSlides)
   ON_UPDATE_COMMAND_UI(IDC_SELECT_SLIDE_RANGE, OnUpdateSelectSlideRange)
   ON_UPDATE_COMMAND_UI(IDC_NEXT, OnUpdateButtonNext)
   ON_BN_CLICKED(IDC_SELECT_ALL_SLIDES, &CStartDialog::OnBnClickedSelectAllSlides)
   ON_BN_CLICKED(IDC_SELECT_CURRENT_SLIDE, &CStartDialog::OnBnClickedSelectCurrentSlide)
   ON_BN_CLICKED(IDC_SELECT_SLIDE_RANGE, &CStartDialog::OnBnClickedSelectSlideRange)
	ON_MESSAGE_VOID( WM_IDLEUPDATECMDUI, OnIdleUpdateUIMessage )
   ON_BN_CLICKED(IDC_NEXT, &CStartDialog::OnBnClickedNext)
   ON_BN_CLICKED(IDC_PREVIOUS, &CStartDialog::OnBnClickedPrevious)
   ON_STN_CLICKED(IDC_USAGE_LABEL, &CStartDialog::OnStnClickedUsageLabel)
   ON_MESSAGE(WM_CHANGE_TEXT, OnChangeText)
   ON_WM_DESTROY()
END_MESSAGE_MAP()


// CStartDialog-Meldungshandler

BOOL CStartDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   CString csDialogTitle;
   csDialogTitle.LoadString(IDS_DIALOG_TITLE);
   SetWindowText(csDialogTitle);

   DWORD dwIds[] = {IDCANCEL,
                    IDC_PREVIOUS,
                    IDC_NEXT,
                    IDC_USAGE_LABEL,
                    IDC_SELECT_ALL_SLIDES,
                    IDC_SELECT_CURRENT_SLIDE,
                    IDC_SELECT_SLIDE_RANGE,
                    -1};
   MfcUtils::Localize(this, dwIds);

   m_bSelected[SELECT_ALL_SLIDES] = 1;
   m_bSelected[SELECT_CURRENT_SLIDE] = 0;
   m_bSelected[SELECT_SLIDE_RANGE] = 0;

   CString csTitle;
   csTitle.LoadString(IDS_STARTTITLE);
   m_titleLabel.SetWindowText(csTitle);

   CWnd *pSlideSelection = GetDlgItem(IDC_SLIDE_SELECTION);
   pSlideSelection->SetWindowText(_T(""));

   return TRUE;  // return TRUE unless you set the focus to a control
   // AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}

HRESULT CStartDialog::Init(int iSlideCount, int iCurrentSlide)
{

   return S_OK;
}

void CStartDialog::OnBnClickedCancel()
{
   // TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
   OnCancel();
}

void CStartDialog::OnUpdateSelectAllSlides(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck(m_bSelected[SELECT_ALL_SLIDES]);
}

void CStartDialog::OnUpdateSelectCurrentSlides(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck(m_bSelected[SELECT_CURRENT_SLIDE]);
}

void CStartDialog::OnUpdateSelectSlideRange(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck(m_bSelected[SELECT_SLIDE_RANGE]);
}

void CStartDialog::OnUpdateButtonNext(CCmdUI *pCmdUI)
{
   //pCmdUI->SetCheck(m_bSelected[SELECT_SLIDE_RANGE]);
	pCmdUI->Enable(true);
}

void CStartDialog::OnBnClickedSelectAllSlides()
{
   m_bSelected[SELECT_ALL_SLIDES] = 1;
   m_bSelected[SELECT_CURRENT_SLIDE] = 0;
   m_bSelected[SELECT_SLIDE_RANGE] = 0;
   //MessageBox(_T("ce!"));
}

void CStartDialog::OnBnClickedSelectCurrentSlide()
{
   m_bSelected[SELECT_ALL_SLIDES] = 0;
   m_bSelected[SELECT_CURRENT_SLIDE] = 1;
   m_bSelected[SELECT_SLIDE_RANGE] = 0;
}

void CStartDialog::OnBnClickedSelectSlideRange()
{
   m_bSelected[SELECT_ALL_SLIDES] = 0;
   m_bSelected[SELECT_CURRENT_SLIDE] = 0;
   m_bSelected[SELECT_SLIDE_RANGE] = 1;
}

void CStartDialog::OnIdleUpdateUIMessage()
{
	//UpdateDialogControls( this, TRUE );
}

BOOL CStartDialog::ContinueModal()
{
   SendMessage( WM_IDLEUPDATECMDUI, WPARAM(TRUE), 0);

   return CDialog::ContinueModal();
}

void CStartDialog::OnBnClickedNext()
{
   // TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	/*CFileSelection *dlgFSel = new CFileSelection;
	dlgFSel->DoModal();*/
	//MessageBox(_T("un text"));
}

void CStartDialog::OnBnClickedPrevious()
{
   // TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
}

void CStartDialog::OnStnClickedUsageLabel()
{
   // TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
}


LRESULT CStartDialog::OnChangeText(WPARAM wParam, LPARAM lParam)
{
   m_titleLabel.SetWindowText(_T(""));

   return TRUE;
}
void CStartDialog::OnDestroy()
{
	m_bIsVisible = false;
   CDialog::OnCancel();
   //CDialog::OnDestroy();
}
