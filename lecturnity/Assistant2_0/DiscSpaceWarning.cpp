// DiscSpaceWarning.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "assistant.h"
#include "DiscSpaceWarning.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDiscSpaceWarning 


CDiscSpaceWarning::CDiscSpaceWarning(CWnd* pParent /*=NULL*/)
	: CDialog(CDiscSpaceWarning::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiscSpaceWarning)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CDiscSpaceWarning::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiscSpaceWarning)
	DDX_Control(pDX, IDC_DISCSPACE_TEXT, m_wndText);
	DDX_Control(pDX, IDC_DISCSPACE_TITLE, m_wndTitle);
	DDX_Control(pDX, IDC_DISCSPACE, m_wndMinutes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiscSpaceWarning, CDialog)
	//{{AFX_MSG_MAP(CDiscSpaceWarning)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDiscSpaceWarning 

void CDiscSpaceWarning::OnOK() 
{
	// TODO: Zusätzliche Prüfung hier einfügen
	
	CDialog::OnOK();
}

BOOL CDiscSpaceWarning::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	DWORD dwIds[] = {IDC_DISCSPACE_TITLE,
                    IDC_DISCSPACE_TEXT,
                    IDC_DISCSPACE,
                    IDC_DISCSPACE_TEXT2,
                   -1};

   MfcUtils::Localize(this, dwIds);

   HICON hIcon = LoadIcon(NULL, IDI_WARNING);
   m_wndWarning.SetIcon(hIcon);

   CString csTitle;
   csTitle.LoadString(IDS_WARNING);
   SetWindowText(csTitle);
   
   CFont *pWindowFont = XTPPaintManager()->GetRegularFont();
   
   if (pWindowFont != NULL)
   {
      LOGFONT lf;
      pWindowFont->GetLogFont(&lf);
      m_font.DeleteObject();
      lf.lfWeight = FW_BOLD;
      m_font.CreateFontIndirect(&lf);
      m_wndTitle.SetFont(&m_font);
      m_wndMinutes.SetFont(&m_font);
   }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CDiscSpaceWarning::Update(LPCTSTR szRecordDrive, int iMinutesLeft)
{
   
   CString csText;
   csText.Format(IDC_DISCSPACE_TEXT, szRecordDrive);
   m_wndText.SetWindowText(csText);
   
   csText.Format(IDC_DISCSPACE, iMinutesLeft);
   m_wndMinutes.SetWindowText(csText);

}
