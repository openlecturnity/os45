// FlashLostStopmarksDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "FlashLostStopmarksDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFlashLostStopmarksDlg 


CFlashLostStopmarksDlg::CFlashLostStopmarksDlg(CArray<CString, CString> *aSlideNumbers, 
                                               CArray<CString, CString> *aStopmarkNumbers, 
                                               CArray<CString, CString> *aStopmarkTimes, 
                                               CWnd* pParent /*=NULL*/)
	: CDialog(CFlashLostStopmarksDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFlashLostStopmarksDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_aSlideNumbers = aSlideNumbers;
   m_aStopmarkNumbers = aStopmarkNumbers;
   m_aStopmarkTimes = aStopmarkTimes;
}


void CFlashLostStopmarksDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlashLostStopmarksDlg)
	DDX_Control(pDX, IDS_FLASH_STOPMARK_LOSS, m_lbFlashStopmarkLoss);
	DDX_Control(pDX, IDC_FLASH_LOST_STOPMARKS_LIST, m_lstFlashLostStopmarks);
	//}}AFX_DATA_MAP
}

void CFlashLostStopmarksDlg::FillLostStopmarkList(CArray<CString, CString> *aSlideNumbers, 
                                                  CArray<CString, CString> *aStopmarkNumbers, 
                                                  CArray<CString, CString> *aStopmarkTimes)
{
   CString csSlide, csStopmarkNumber, csStopmarkTime;
   csSlide.LoadString(IDS_FLASH_PAGE); // IDS_FLASH_SLIDE
   csStopmarkNumber.LoadString(IDS_FLASH_STOPMARK_NUMBER);
   csStopmarkTime.LoadString(IDS_FLASH_STOPMARK_TIME);

   m_lstFlashLostStopmarks.InsertColumn(0, csSlide, LVCFMT_LEFT, 40);
   m_lstFlashLostStopmarks.InsertColumn(1, csStopmarkNumber, LVCFMT_LEFT, 120);
   m_lstFlashLostStopmarks.InsertColumn(2, csStopmarkTime, LVCFMT_LEFT, 140);

   for (int i = 0; i < aSlideNumbers->GetSize(); ++i)
   {
      m_lstFlashLostStopmarks.InsertItem(i, aSlideNumbers->ElementAt(i));
      m_lstFlashLostStopmarks.SetItemText(i, 1, aStopmarkNumbers->ElementAt(i));
      m_lstFlashLostStopmarks.SetItemText(i, 2, aStopmarkTimes->ElementAt(i));
   }
}

BEGIN_MESSAGE_MAP(CFlashLostStopmarksDlg, CDialog)
	//{{AFX_MSG_MAP(CFlashLostStopmarksDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFlashLostStopmarksDlg 

BOOL CFlashLostStopmarksDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

   CString csLabel;
   csLabel.LoadString(IDS_FLASH_STOPMARK_LOSS);
   m_lbFlashStopmarkLoss.SetWindowText(csLabel);
   
   FillLostStopmarkList(m_aSlideNumbers, m_aStopmarkNumbers, m_aStopmarkTimes);
	
	// TODO: Zusätzliche Initialisierung hier einfügen
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
