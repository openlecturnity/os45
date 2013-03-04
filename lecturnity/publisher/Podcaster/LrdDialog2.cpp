// LrdDialog2.cpp : implementation file
//

#include "stdafx.h"
#include "podcaster.h"
#include "LrdDialog2.h"
#include "GeneralSettings.h"
#include "Registry.h"
#include "PodcasterDlg.h"
#include "MfcUtils.h"
#include "WaitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLrdDialog2 dialog


CLrdDialog2::CLrdDialog2(CWnd* pParent /*=NULL*/)
	: CDialog(CLrdDialog2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLrdDialog2)
	m_csMp4Files = _T("");
	m_HWaitDlg = NULL;
	//}}AFX_DATA_INIT
}


void CLrdDialog2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLrdDialog2)
	DDX_Control(pDX, IDC_PODCASTDOC, m_Mp4Files);
	DDX_Control(pDX, IDC_BARBOTTOM, m_stcBarBottom);
	DDX_CBString(pDX, IDC_PODCASTDOC, m_csMp4Files);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLrdDialog2, CDialog)
	//{{AFX_MSG_MAP(CLrdDialog2)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDB_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDB_PODCAST, OnPodcast)
	ON_BN_CLICKED(IDB_SETINGS, OnSetings)
	ON_CBN_EDITCHANGE(IDC_PODCASTDOC, OnEditchangePodcastdoc)
	ON_CBN_SELCHANGE(IDC_PODCASTDOC, OnSelchangePodcastdoc)
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()
//	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLrdDialog2 message handlers
void CLrdDialog2::DrawLogo()
{
   RECT rct;
   this->GetClientRect(&rct);
   m_stcBarBottom.MoveWindow(0, rct.bottom  - 44, rct.right, 2, TRUE);
}

BOOL CLrdDialog2::OnInitDialog() 
{
	CDialog::OnInitDialog();
   SetIcon(AfxGetApp()->LoadIcon(IDI_PODCASTER), TRUE);         // Set big icon
	
	// TODO: Add extra initialization here
	RECT rt;
   dlg.Create(IDD_LRD_DIALOG, this);
   dlg.ShowWindow(SW_SHOW);
   GetClientRect(&rt);
   ::MoveWindow(dlg.m_hWnd, BORDER, BORDER + 26, rt.right - 2*BORDER,
      rt.bottom - 2*BORDER - 70, TRUE);

   DrawLogo();

   // from command line ?
   if(!csIniMp4File.IsEmpty())
   {
      // Does the file name contain special characters?
      CString csIniMp4Name(csIniMp4File);
      // Does the path / file name contain special characters?
///      int nLastSeparator = csIniMp4File.ReverseFind(_T('\\'));
///      if (nLastSeparator >= 0)
///         csIniMp4Name = csIniMp4File.Right(nLastSeparator+1);
      if (!CheckUrlFileName(csIniMp4Name))
      {
         CString csError;
         csError.LoadString(IDS_ERR_MP4URL);
         AfxMessageBox(csError, MB_ICONSTOP);
      }
      else
      {
         //m_Mp4Files.AddString(csIniMp4File);
         m_Mp4Files.SetCurSel(0);
      }
   }

   DWORD dwID[] = {
                  IDL_PODCASTDOC,
                  IDB_BROWSE,
                  IDB_PODCAST,
                  IDB_SETINGS,
                  -1 };
   MfcUtils::Localize(this, dwID);

   BOOL bValid = m_Mp4Files.GetCount();
   ::EnableWindow(::GetDlgItem(m_hWnd, IDB_PODCAST), bValid);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLrdDialog2::SearchForLrd(CString csMediaFile) 
{
   TCHAR strDirectory[MAX_PATH];
   _tcscpy(strDirectory, csMediaFile);
   // construct the path
   for(int i = csMediaFile.GetLength() - 1;i >= 0; i--)
   {
      if(strDirectory[i] == _T('\\'))
      {
         strDirectory[i] = 0;
         break;
      }
   }
   // set as current directory   
   ::SetCurrentDirectory(strDirectory);
   // update 
   dlg.UpdateData();
   dlg.m_csLRDocument = dlg.FindFirstLrdFile();
   dlg.ParseLrd();
   dlg.UpdateData(FALSE);

}

void CLrdDialog2::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CLrdDialog2::OnOK2() 
{
	// TODO: Add extra validation here	
	OnOK();
}

void CLrdDialog2::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CLrdDialog2::OnCancel2() 
{
   OnCancel();
}

void CLrdDialog2::OnDestroy() 
{
	CDialog::OnDestroy();
}

void CLrdDialog2::OnBrowse() 
{
	// TODO: Add your control notification handler code here
   CString csFilter = _T("Podcast Files (*.mp4)|*.mp4||");
   CFileDialog dlgOpen(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, csFilter, this);

   if(dlgOpen.DoModal() == IDOK)
   {
      UpdateData();
      // Does the path / file name contain special characters?
      if (!CheckUrlFileName(dlgOpen.GetPathName()))
      {
         CString csError;
         csError.LoadString(IDS_ERR_MP4URL);
         AfxMessageBox(csError, MB_ICONSTOP);
      }
      else
      {
         // get the item
         m_csMp4Files = dlgOpen.GetPathName();
         SearchForLrd(m_csMp4Files);
      
         //CString csItem;
         //BOOL bFound = FALSE;
         //for(int i=0;i < m_Mp4Files.GetCount();i++)
         //{
         //   m_Mp4Files.GetLBText(i, csItem);
         //   if(m_csMp4Files == csItem)
         //   {
         //      bFound = TRUE;
         //      break;
         //   }
         //}
      
         //if(!bFound)
         //{
         //   m_Mp4Files.AddString(dlgOpen.GetPathName());
         //}
      
         UpdateData(FALSE);
         BOOL bValid = !m_csMp4Files.IsEmpty();
         ::EnableWindow(::GetDlgItem(m_hWnd, IDB_PODCAST), bValid);
      }
   }
}

void CLrdDialog2::OnPodcast() 
{
	// TODO: Add your control notification handler code here
	CPodcasterDlg dlgPodcaster;

   UpdateData();
   dlg.UpdateData();
   
   dlgPodcaster.csMediaFile = m_csMp4Files;
   dlgPodcaster.csAuthor = dlg.m_csAuthor;
   dlgPodcaster.csTitle = dlg.m_csTitle;
   
   dlg.UpdateData(FALSE);
   UpdateData(FALSE);

   // test if file exist
   FILE *fp = _tfopen(m_csMp4Files, _T("r"));
   if(fp != NULL)
   {
      fclose(fp);
      dlgPodcaster.DoModal();
   }
   else
   {
      CString csMessage, csFormat;
      csFormat.LoadString(IDS_ERROPENFILE);
      csMessage.Format(csFormat, m_csMp4Files);
      AfxMessageBox( csMessage, MB_OK + MB_ICONSTOP);
      //EnableWindow(::GetDlgItem(m_hWnd, IDB_PODCAST), bResult);
   }

}

void CLrdDialog2::OnSetings() 
{
	// TODO: Add your control notification handler code here
   CGeneralSettings dlgSettings;
   SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
   dlgSettings.m_HWaitDlg = m_HWaitDlg;
   //CWnd *wait = FromHandle(m_HWaitDlg);
   //wait->SetParent(this);
   dlgSettings.DoModal();
}

void CLrdDialog2::OnEditchangePodcastdoc() 
{
	// TODO: Add your control notification handler code here
   UpdateData();
   BOOL bValid = !m_csMp4Files.IsEmpty();
   ::EnableWindow(::GetDlgItem(m_hWnd, IDB_PODCAST), bValid);
	
}

void CLrdDialog2::OnSelchangePodcastdoc() 
{
	// TODO: Add your control notification handler code here
   BOOL bValid = m_Mp4Files.GetCount();
   ::EnableWindow(::GetDlgItem(m_hWnd, IDB_PODCAST), bValid);
	
}

BOOL CLrdDialog2::CheckUrlFileName(CString csUrl)
{
   BOOL bAllOk = TRUE;

   _TCHAR tsz;

   for (int i = 0; i < csUrl.GetLength(); ++i)
   {
      tsz = csUrl.GetAt(i);
      bAllOk = bAllOk && ((tsz >= _T('a') && tsz <= _T('z')) || (tsz >= _T('A') && tsz <= _T('Z')) ||
                          (tsz >= _T('0') && tsz <= _T('9')) || 
                          (tsz == _T('-')) || (tsz == _T('.')) || (tsz == _T('_')) || (tsz == _T('~')) ||
                          (tsz == _T(':')) || (tsz == _T('@')) || 
                          (tsz == _T('!')) || (tsz == _T('$')) || (tsz == _T('\'')) || 
                          (tsz == _T('*')) || (tsz == _T('+')) || 
                          (tsz == _T(',')) || (tsz == _T(';')) || (tsz == _T('=')) || 
                          (tsz == _T('%')) || (tsz == _T('/')) || (tsz == _T('\\')));
      // These 'allowed' characters seem not to work:
      // (tsz == _T('&')) || (tsz == _T('(')) || (tsz == _T(')')) || (tsz == _T(' ')) || 

      // "escape" sequence: "%" hex hex
      if (tsz == _T('%'))
      {
         if (i < (csUrl.GetLength()-2))
         {
            i++;
            _TCHAR h1 = csUrl.GetAt(i);
            i++;
            _TCHAR h2 = csUrl.GetAt(i);
            bAllOk = bAllOk && ((h1 >= _T('0') && h1 <= _T('9')) || 
                                (h1 >= _T('a') && h1 <= _T('f')) || (h1 >= _T('A') && h1 <= _T('F')));
            bAllOk = bAllOk && ((h2 >= _T('0') && h2 <= _T('9')) || 
                                (h2 >= _T('a') && h2 <= _T('f')) || (h2 >= _T('A') && h2 <= _T('F')));
         }
         else
            bAllOk = bAllOk && FALSE;
      }
   }

   return bAllOk;
}

void CLrdDialog2::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: Add your message handler code here

	CWaitDlg *wait=(CWaitDlg*)FromHandle(m_HWaitDlg);
	if(wait->IsWindowVisible())
	{
		if(nState!=WA_INACTIVE)
		{
			wait->BringWindowToTop();
			wait->UpdateWindow();
		}
		//SelectItem(0);
		UpdateWindow();
	}

}

//int CLrdDialog2::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	CWaitDlg *wait=(CWaitDlg*)FromHandle(m_HWaitDlg);
//	if(wait->IsWindowVisible())
//	{
//		wait->BringWindowToTop();
//		wait->UpdateWindow();
//		//SelectItem(0);
//		UpdateWindow();
//	}
//
//	return CDialog::OnMouseActivate(pDesktopWnd, nHitTest, message);
//}
