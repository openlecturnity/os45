// TemplatePreviewDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "TemplatePreviewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplatePreviewDlg 


CTemplatePreviewDlg::CTemplatePreviewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplatePreviewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplatePreviewDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CTemplatePreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplatePreviewDlg)
	DDX_Control(pDX, IDC_TEMPLATE_PREVIEW_IMG, m_previewImg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplatePreviewDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplatePreviewDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTemplatePreviewDlg 

void CTemplatePreviewDlg::SetPreviewImage(const _TCHAR *tszFileName)
{
   m_previewImg.ReadImage(tszFileName);

   int w = 300;
   int h = 150;
   if (tszFileName)
   {
      if (_tcscmp(tszFileName, _T("")) != 0)
      {
         w = m_previewImg.GetImageWidth();
         h = m_previewImg.GetImageHeight();
      }
   }

   CRect rcClient;
   CRect rcWindow;
   GetClientRect(&rcClient);
   GetWindowRect(&rcWindow);
   int newWidth = rcWindow.Width() - rcClient.Width() + w;
   int newHeight = rcWindow.Height() - rcClient.Height() + h;
   SetWindowPos(&wndTop, 0, 0, newWidth, newHeight, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
   m_previewImg.SetWindowPos(&wndTop, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE);
}

BOOL CTemplatePreviewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   CString csCaption;
   csCaption.LoadString(IDD_TEMPLATE_PREVIEW);
   SetWindowText(csCaption);
   m_previewImg.SetDrawImageSize(false);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
