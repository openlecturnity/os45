#include "stdafx.h"
#include "WritingAudioDlg.h"
#include "MfcUtils.h"
#include "avinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CWritingAudioDlg 


CWritingAudioDlg::CWritingAudioDlg(CWnd* pParent /*=NULL*/,
                                   const _TCHAR *tszSourceFile, const _TCHAR *tszTargetFile)
	: bInitialized(false), CDialog(CWritingAudioDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWritingAudioDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_csSourceFile.Empty();
   m_csTargetFile.Empty();
   if (tszSourceFile)
      m_csSourceFile = tszSourceFile;
   if (tszTargetFile)
      m_csTargetFile;
}


void CWritingAudioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWritingAudioDlg)
	DDX_Control(pDX, IDC_TARGET_EDIT, m_edTargetFile);
	DDX_Control(pDX, IDC_SOURCE_EDIT, m_edSourceFile);
	DDX_Control(pDX, IDC_WRITEAUDIO_PROGRESS, m_progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWritingAudioDlg, CDialog)
	//{{AFX_MSG_MAP(CWritingAudioDlg)
   ON_MESSAGE(WM_USER, OnCheckEndDialog)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_CANCEL_E, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CWritingAudioDlg 

BOOL CWritingAudioDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

   DWORD dwIds[] = {IDC_MSG_WRITING_AUDIO,
                    IDC_SOURCE_LABEL,
                    IDC_TARGET_LABEL,
                    IDC_CANCEL_E,
                    -1};
   MfcUtils::Localize(this, dwIds);
	
   bInitialized = true;

   m_progress.SetRange(0, 100);
   m_progress.SetPos(0);

   m_edSourceFile.SetWindowText(m_csSourceFile);
   m_edTargetFile.SetWindowText(m_csTargetFile);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

LRESULT CWritingAudioDlg::OnCheckEndDialog(WPARAM wParam, LPARAM lParam)
{
   if (wParam == END_DIALOG)
      EndDialog(0);
   else if (wParam == UPDATE_TARGET)
   {
      m_csTargetFile = (const _TCHAR *) lParam;
      m_edTargetFile.SetWindowText(m_csTargetFile);
   }
   else if (wParam == 999)
   {
      m_progress.SetPos(lParam);
   }

   return 0;
}

BOOL CWritingAudioDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	//return CDialog::OnSetCursor(pWnd, nHitTest, message);
   ::SetCursor(::LoadCursor(NULL, IDC_WAIT));

   return TRUE;
}

void CWritingAudioDlg::OnCancel() 
{
   AVInfo::StaticWriteWavFromFileCancel();	
}
