// ImportClipAudioDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "ImportClipAudioDlg.h"

#include "MfcUtils.h" // Localize

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CImportClipAudioDlg 


CImportClipAudioDlg::CImportClipAudioDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImportClipAudioDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportClipAudioDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CImportClipAudioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportClipAudioDlg)
	DDX_Control(pDX, IDC_AUDIO_PULLDOWN, m_pdAudio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportClipAudioDlg, CDialog)
	//{{AFX_MSG_MAP(CImportClipAudioDlg)
	ON_BN_CLICKED(IDC_NO_AUDIO, OnNoAudio)
	ON_BN_CLICKED(IDC_YES_AUDIO, OnUseAudio)
	ON_BN_CLICKED(IDC_CANCEL_AUDIO, OnCancelAudio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CImportClipAudioDlg 

BOOL CImportClipAudioDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

   CString csCaption;
   csCaption.LoadString(IDD);
   SetWindowText(csCaption);

   // Fill with text in current language
   DWORD dwIds[] = {IDS_MSG_IMPORTCLIP_USEAUDIO,
                    IDS_YES_LABEL,
                    IDC_YES_AUDIO,
                    IDC_NO_AUDIO,
                    IDC_CANCEL_AUDIO,
                    -1};
   MfcUtils::Localize(this, dwIds);

   CString csEntry;
   csEntry.LoadString(IDC_MIX_AUDIO);
   int result = m_pdAudio.AddString(csEntry);
   csEntry.LoadString(IDC_REPLACE_AUDIO);
   result = m_pdAudio.AddString(csEntry);
   csEntry.LoadString(IDC_INSERT_AUDIO);
   result = m_pdAudio.AddString(csEntry);
   m_pdAudio.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CImportClipAudioDlg::OnNoAudio() 
{
   EndDialog(IDC_NO_AUDIO);
}

void CImportClipAudioDlg::OnUseAudio() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	int nCurSel = m_pdAudio.GetCurSel();
   if (nCurSel == 0)
      EndDialog(IDC_MIX_AUDIO);
   else if (nCurSel == 1)
      EndDialog(IDC_REPLACE_AUDIO);
   else if (nCurSel == 2)
      EndDialog(IDC_INSERT_AUDIO);
   else
	   CDialog::OnCancel();
}


void CImportClipAudioDlg::OnCancel() 
{
	// TODO: Zusätzlichen Bereinigungscode hier einfügen
	
	CDialog::OnCancel();
}

void CImportClipAudioDlg::OnCancelAudio() 
{
   EndDialog(IDCANCEL);	
}
