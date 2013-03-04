// TwoGBWarning.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "assistant.h"
#include "TwoGBWarning.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTwoGBWarning 


CTwoGBWarning::CTwoGBWarning(CWnd* pParent /*=NULL*/)
	: CDialog(CTwoGBWarning::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTwoGBWarning)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

}


void CTwoGBWarning::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTwoGBWarning)
   DDX_Control(pDX, IDC_2GB_WARNING_TITLE, m_wndTitle);
   DDX_Control(pDX, IDC_2GB_WARNING, m_wndMinutes);
   //}}AFX_DATA_MAP
   DDX_Control(pDX, IDC_WARNING_ICON, m_wndWarning);
}


BEGIN_MESSAGE_MAP(CTwoGBWarning, CDialog)
	//{{AFX_MSG_MAP(CTwoGBWarning)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTwoGBWarning 

BOOL CTwoGBWarning::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	
	DWORD dwIds[] = {IDC_2GB_WARNING_TITLE,
                    IDC_2GB_WARNING_TEXT,
                    IDC_2GB_WARNING,
                    IDC_2GB_WARNING_TEXT2,
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

void CTwoGBWarning::Update(int iMinutesLeft)
{
   CString csText;
   csText.Format(IDC_2GB_WARNING, iMinutesLeft);
   m_wndMinutes.SetWindowText(csText);
}

void CTwoGBWarning::OnOK() 
{
	CDialog::OnOK();
}
