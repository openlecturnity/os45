// FilenameChangeDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filesdk.h"
#include "FilenameChangeDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFilenameChangeDlg 


CFilenameChangeDlg::CFilenameChangeDlg(const _TCHAR *tszFileName, CWnd* pParent /*=NULL*/)
	: CDialog(CFilenameChangeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilenameChangeDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_csFileName = tszFileName;
}


void CFilenameChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilenameChangeDlg)
	DDX_Control(pDX, IDC_FILENAME, m_edFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilenameChangeDlg, CDialog)
	//{{AFX_MSG_MAP(CFilenameChangeDlg)
	ON_BN_CLICKED(ID_OK, OnOk)
	ON_BN_CLICKED(ID_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFilenameChangeDlg 

BOOL CFilenameChangeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

   CString csCaption;
   csCaption.LoadString(IDD);
   SetWindowText(csCaption);

   int nLen = m_csFileName.GetLength();
   for (int i=0; i<nLen; ++i)
   {
      if (m_csFileName[i] > 127 || m_csFileName[i] < 32)
         m_csFileName.SetAt(i, _T('_'));
   }
   m_edFileName.SetWindowText(m_csFileName);

   DWORD dwIds[] = {IDC_FILENAME_DESC,
                    ID_OK,
                    ID_CANCEL,
                    -1};
   Localize(this, dwIds);

   m_edFileName.SetFocus();
   m_edFileName.SetSel(0, -1, FALSE);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CFilenameChangeDlg::OnOk() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
   CString csFileName;
   m_edFileName.GetWindowText(csFileName);
   int nLen = csFileName.GetLength();

   bool bContainsBadChars = false;
   for (int i=0; i<nLen; ++i)
   {
      _TCHAR c = csFileName[i];
      if (c > 127 || c < 32 ||
          c == '\\' || c == '/' ||
          c == '?' || c == '|' ||
          c == ':' || c == '*' ||
          c == '>' || c == '<' ||
          c == '"')
      {
         bContainsBadChars = true;
         csFileName.SetAt(i, _T('_'));
      }
   }

   if (bContainsBadChars)
   {
      m_edFileName.SetWindowText(csFileName);

      CString csMsg;
      csMsg.LoadString(IDS_MSG_STILL_WRONG);
      CString csCap;
      csCap.LoadString(IDS_WARNING);

      MessageBox(csMsg, csCap, MB_OK | MB_ICONINFORMATION);
   }
   else
   {
      m_csFileName = csFileName;
      EndDialog(IDOK);
   }
}

void CFilenameChangeDlg::OnCancel() 
{
   EndDialog(IDCANCEL);	
}
