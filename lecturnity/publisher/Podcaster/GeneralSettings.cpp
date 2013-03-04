// GeneralSettings.cpp : implementation file
//

#include "stdafx.h"
#include <commctrl.h>
#include "podcaster.h"
#include "GeneralSettings.h"
#include "MetadataPage.h"
#include "TransferPage.h"
#include "ChannelMetadata.h"
#include "PasswordDlg.h"
#include "XmlValidator.h"
#include "Ftp.h"
#include "MfcUtils.h"
#include "WaitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeneralSettings dialog


CGeneralSettings::CGeneralSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CGeneralSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGeneralSettings)
	//}}AFX_DATA_INIT
	m_HWaitDlg = NULL;
}


void CGeneralSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralSettings)
	DDX_Control(pDX, IDB_REMOVE, m_btnRemove);
	DDX_Control(pDX, IDB_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_PAGES, m_TabControl);
	DDX_Control(pDX, IDC_CHANNELLIST, m_ListControl);
	DDX_Control(pDX, IDC_BARBOTTOM, m_stcBarBottom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeneralSettings, CDialog)
	//{{AFX_MSG_MAP(CGeneralSettings)
	ON_BN_CLICKED(IDB_REMOVE, OnRemove)
	ON_BN_CLICKED(IDB_DELETE, OnDelete)
	ON_BN_CLICKED(IDB_NEW, OnNew)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDB_IMPORT, OnImport)
	ON_NOTIFY(NM_CLICK, IDC_CHANNELLIST, OnClickList)
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()
//	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralSettings message handlers
// the session thread

BOOL CGeneralSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   // get app
   pApp = (CPodcasterApp *)AfxGetApp();
   SetIcon(pApp->LoadIcon(IDI_PODCASTER), TRUE);         // Set big icon
   DrawLogo();
   

   CString csCaption;
   m_TabControl.InitDialogs();
   csCaption.LoadString(IDT_METADATA);  
   m_TabControl.InsertItem(0, csCaption);
   csCaption.LoadString(IDT_TRANSFER);  
   m_TabControl.InsertItem(1, csCaption);
   
   m_TabControl.ActivateTabDialogs();


   DWORD dwID[] = {
         IDL_CHANNELS,
	      IDCANCEL,
         IDB_NEW,
         IDB_IMPORT,
         IDB_REMOVE,
         IDB_DELETE,
         -1 };
   MfcUtils::Localize(this, dwID);

   csCaption.LoadString(IDD_MAINSETTINGS);
   this->SetWindowText(csCaption);

//   LVCOLUMN col;
//   col.mask = LVCF_TEXT;
//   col.pszText = _T("Active");
//   m_ListControl.SetColumn(0, &col);
//   col.pszText = _T("Title");
//   m_ListControl.SetColumn(0, &col);

   csCaption.LoadString(IDL_TITLE2);  
   m_ListControl.InsertColumn(0, csCaption, LVCFMT_LEFT, 190, -1);

   csCaption.LoadString(IDL_ACTIVE);  
   m_ListControl.InsertColumn(1, csCaption, LVCFMT_CENTER, 50, -1);

   GetChannels();

//   CString csKey = m_ListControl.GetItemText(0, 0);
//   m_TabControl.GetTransferSettings((LPCTSTR)csKey);
   if(m_ListControl.GetItemCount() > 0)
   {
      CString *csKey = (CString *)m_ListControl.GetItemData(0);
      m_TabControl.GetTransferSettings((LPCTSTR)*csKey);
   }

   EnableControls();

   SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeneralSettings::DrawLogo()
{
   RECT rct;
   this->GetClientRect(&rct);
   m_stcBarBottom.MoveWindow(0, rct.bottom  - 44, rct.right, 2, TRUE);
}

CString CGeneralSettings::GetActiveChannel()
{
   HKEY hKey = NULL;
   TCHAR tstrSubKey[MAX_PATH];
   DWORD dwSubKey = MAX_PATH - 1;
   int iActive = 0;
   FILETIME ft;

   CString csResult;
   csResult.Empty();
   
   LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER,
         PODCASTER_KEY, 0, KEY_ENUMERATE_SUB_KEYS, &hKey);
   
   if(lResult != ERROR_SUCCESS)
   {
      csResult;
   }

   DWORD i = 0;
   while(TRUE)
   {
      csResult.Empty();
      dwSubKey = MAX_PATH - 1;
      lResult = RegEnumKeyEx(hKey, i, tstrSubKey,
         &dwSubKey, NULL, NULL, NULL, &ft);
      if(lResult == ERROR_NO_MORE_ITEMS)
      {
         break;
      }
      if(lResult != ERROR_SUCCESS)
      {
         break;
      }
      csResult = CString(tstrSubKey, dwSubKey);
      // get active value
      if(reg.Open(csResult) == ERROR_SUCCESS)
      {
         iActive = reg.GetProfileInt(_T("Active"), 0);   
         reg.Close();
      }
      if(iActive != 0)
      {
         // found
         break;
      }
   }

   RegCloseKey(hKey);
   return csResult;
}

void CGeneralSettings::GetLastChannel()
{
   HKEY hKey = NULL;
   TCHAR tstrSubKey[MAX_PATH];
   DWORD dwSubKey = MAX_PATH - 1;
   FILETIME ft;
   
   LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER,
         PODCASTER_KEY, 0, KEY_ENUMERATE_SUB_KEYS, &hKey);
   
   if(lResult != ERROR_SUCCESS)
   {
      return;
   }

   DWORD i = 0;
   iLastChannel = -1;
   while(TRUE)
   {
      dwSubKey = MAX_PATH - 1;
      lResult = RegEnumKeyEx(hKey, i++, tstrSubKey, &dwSubKey, NULL, NULL, NULL, &ft);
      if(lResult == ERROR_NO_MORE_ITEMS)
      {
         break;
      }
      if(lResult != ERROR_SUCCESS)
      {
         break;
      }
      // calculate last channel
      _stscanf(tstrSubKey, _T("Channel %d"), &iLastChannelTmp);
      if(iLastChannelTmp > iLastChannel)
      {
         iLastChannel = iLastChannelTmp;
      }
   }

   RegCloseKey(hKey);
}

void CGeneralSettings::GetChannels()
{
   HKEY hKey = NULL;
   TCHAR tstrSubKey[MAX_PATH];
   DWORD dwSubKey = MAX_PATH - 1;
   FILETIME ft;
   
   LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER,
         PODCASTER_KEY, 0, KEY_ENUMERATE_SUB_KEYS, &hKey);
   
   if(lResult != ERROR_SUCCESS)
   {
      return;
   }

   DWORD i = 0;
   iLastChannel = -1;
   m_ListControl.DeleteAllItems();
   while(TRUE)
   {
      dwSubKey = MAX_PATH - 1;
      lResult = RegEnumKeyEx(hKey, i, tstrSubKey, &dwSubKey, NULL, NULL, NULL, &ft);
      if(lResult == ERROR_NO_MORE_ITEMS)
      {
         break;
      }
      if(lResult != ERROR_SUCCESS)
      {
         break;
      }
      // insert item
      CString csItem = _T("??");
      m_ListControl.InsertItem(i, csItem);

      // use item data to store the channel name
      CString *csChannelName = new CString(tstrSubKey, dwSubKey);
      m_ListControl.SetItemData(i, (DWORD)csChannelName);

      i++;  // next channel
      // calculate last channel
      _stscanf(tstrSubKey, _T("Channel %d"), &iLastChannelTmp);
      if(iLastChannelTmp > iLastChannel)
      {
         iLastChannel = iLastChannelTmp;
      }
   }

   RegCloseKey(hKey);
	//show wait dialog
   CWnd *wait=FromHandle(m_HWaitDlg);
   wait->ShowWindow(SW_SHOW);
   wait->CenterWindow(this);
   wait->BringWindowToTop();
   wait->UpdateWindow();
   unsigned uCount = m_ListControl.GetItemCount();
   for (unsigned j = 0;j < uCount;j++)
   {
      //CString pszKey = m_ListControl.GetItemText(j, 0);
      CString *pszKey = (CString *)m_ListControl.GetItemData(j);
      if(reg.Open(*pszKey) == ERROR_SUCCESS)
      {
         int iActive = reg.GetProfileInt(_T("Active"), 0);   
         m_ListControl.SetState(j, 1, iActive);
         reg.Close();
      }

      // set channel name to channel title
      m_TabControl.GetTransferSettings((LPCTSTR)*pszKey);
      // metadata
      CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
      CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];
      meta->GetMetadata(trsp->m_csChannel, FALSE);
      m_ListControl.SetItemText(j, 0, meta->m_csTitle);
   }
   wait->ShowWindow(SW_HIDE);
}

LONG CGeneralSettings::DeleteKey(CString csKey)
{
   HKEY hKey = NULL;
   LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER,
         PODCASTER_KEY, 0, KEY_ALL_ACCESS, &hKey);  
   if(lResult != ERROR_SUCCESS)
   {
      return lResult;
   }
   lResult = RegDeleteKey(hKey, csKey);
   RegCloseKey(hKey);  
   return lResult;
}

BOOL CGeneralSettings::RemoveChannel(int iChannel)
{
   BOOL bResult = FALSE;
  
   if(iChannel < 0)
   {
      return bResult;
   }

   CString *csKey = (CString *)m_ListControl.GetItemData(iChannel);
   LONG lResult = DeleteKey(*csKey);
   bResult = (lResult == ERROR_SUCCESS);

   if(bResult)
   {
      // delete the channel name stored on item data
      delete csKey;  // remove the channel name 
      // and now from list
      m_ListControl.DeleteItem(iChannel); // delete from list
      SelectItem(iChannel - 1);
      ClearPages();
   }
   return bResult;
}

void CGeneralSettings::ClearPages() 
{
   if(m_ListControl.GetItemCount() == 0)
   {
      CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
      meta->GetMetadata(_T(""), FALSE);
      CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];
      trsp->ClearPage();
   }
}

void CGeneralSettings::OnRemove() 
{
	// TODO: Add your control notification handler code here
   CString csQuestion;
   csQuestion.LoadString(IDM_CONFIRMREMOVE);

   if(AfxMessageBox(csQuestion, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES)
   {
      return;
   }

   int iSelected = m_ListControl.GetNextItem(-1, LVNI_SELECTED);
   if(iSelected != -1)
   {
      RemoveChannel(iSelected);
   }
   EnableControls();
}

void CGeneralSettings::OnDelete() 
{
	// TODO: Add your control notification handler code here
   CString csQuestion;
   csQuestion.LoadString(IDM_CONFIRMDELETE);
   if(AfxMessageBox(csQuestion ,MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES)
   {
      return;
   }

   int iSelected = m_ListControl.GetNextItem(-1, LVNI_SELECTED);
   if(iSelected != -1)
   {
      // TODO: remove from remote server
      //CString csKey = m_ListControl.GetItemText(iSelected, 0);
      CString *csKey = (CString *)m_ListControl.GetItemData(iSelected);
      if(DeleteChannel(*csKey) == TRUE)
      {
         RemoveChannel(iSelected);
      }
   }
   EnableControls();

}

void CGeneralSettings::SelectChannel(CString &csChannel) 
{
   unsigned uCount = m_ListControl.GetItemCount();
   if (uCount > 0)
   {
      for (unsigned i = 0;i < uCount;i++)
      {
         //CString pszKey = m_ListControl.GetItemText(i, 0);
         CString *pszKey = (CString *)m_ListControl.GetItemData(i);
         if(csChannel.Compare(*pszKey) == 0)
         {
            m_ListControl.SetState(i, 1, 0);
            SelectItem(i);
            if(reg.Open(*pszKey) == ERROR_SUCCESS)
            {
               reg.WriteProfileInt(_T("Active"), 0);   
               reg.Close();
            }
            iLastChannel++;
            return;
         }
      }
   }       
}

void CGeneralSettings::Refresh() 
{
	CWnd *wait = FromHandle(m_HWaitDlg);
	wait->ShowWindow(SW_SHOW);
	wait->CenterWindow(this);
	wait->BringWindowToTop();
	wait->UpdateWindow();
   unsigned uCount = m_ListControl.GetItemCount();
   if (uCount > 0)
   {
      for (unsigned i = 0;i < uCount;i++)
      {
         //CString pszKey = m_ListControl.GetItemText(i, 0);
         UINT uState = m_ListControl.GetItemState(i, LVIS_SELECTED);
         bool bSelected = (uState == LVIS_SELECTED);
         if(bSelected)
         {
            // transfer
            //CString csKey = m_ListControl.GetItemText(iSelected, 0);
            CString *csKey = (CString *)m_ListControl.GetItemData(i);
            m_TabControl.GetTransferSettings((LPCTSTR)*csKey);
            // metadata
            CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
            CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];
            meta->GetMetadata(trsp->m_csChannel, FALSE);
            m_ListControl.SetItemText(i, 0, meta->m_csTitle);
			wait->ShowWindow(SW_HIDE);
            return;
         }
      }
   }
   wait->ShowWindow(SW_HIDE);

}

void CGeneralSettings::OnImport() 
{
	// TODO: Add your control notification handler code here
   GetLastChannel();

   CString csNewChannel;
   csNewChannel.Format(_T("Channel %d"), iLastChannel + 1);
   int iAppend = m_ListControl.GetItemCount();

   CTransferSettings dlgSettings;
   dlgSettings.m_csChannelName = csNewChannel;
   if(dlgSettings.DoModal() == IDOK)
   {
	CWnd *wait = FromHandle(m_HWaitDlg);
	wait->ShowWindow(SW_SHOW);
	wait->CenterWindow(this);
	wait->BringWindowToTop();
	wait->UpdateWindow();

      CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
      if(meta->GetMetadata(dlgSettings.m_csChannel) == TRUE)
      {
         m_ListControl.InsertItem(iAppend, _T("??"));
         m_ListControl.SetItemText(iAppend, 0, meta->m_csTitle);

         // the new channel
         CString *csChannelName = new CString(csNewChannel);
         m_ListControl.SetItemData(iAppend, (DWORD)csChannelName);
     
         SelectChannel(csNewChannel);
         SetAsActiveChannel();
         iLastChannel++;
      }
      else
      {
         DeleteKey(csNewChannel);
         SelectItem(0);
      }
	  wait->ShowWindow(SW_HIDE);
   }	

   EnableControls();
}

void CGeneralSettings::OnNew() 
{
	// TODO: Add your control notification handler code here
   // refresh the list
   CXmlValidator valid;
   CString csFile;

   BOOL bDelete = TRUE;
   GetLastChannel();

   CString csNewChannel;
   csNewChannel.Format(_T("Channel %d"), iLastChannel + 1);
   int iAppend = m_ListControl.GetItemCount();

   CTransferSettings dlgSettings;
   dlgSettings.m_csChannelName = csNewChannel;
   CWnd *wait=FromHandle(m_HWaitDlg);
   if(dlgSettings.DoModal() == IDOK)
   {
      // metadata
      CChannelMetadata dlgMeta;
      if(dlgMeta.DoModal() == IDOK)
      {
		wait->ShowWindow(SW_SHOW);
		wait->CenterWindow(this);
		wait->BringWindowToTop();
		wait->UpdateWindow();

         // crete new channel
         csFile = pApp->csTmpDirectory + _T("\\new.xml");
         bDelete = !dlgMeta.dlg.CreateNewChannel(csFile);
         if(bDelete == FALSE)
         {
            // arrange file
            valid.Validate(csFile, csFile);
            // upload to server
            bDelete = !UploadChannel(csNewChannel, csFile);
         }
      }
   }	

   // test if the key must be deleted
   if(bDelete)
   {
      DeleteKey(csNewChannel);
      SelectItem(0);
   }
   else
   {
      // add new channel
      m_ListControl.InsertItem(iAppend, _T("??"));     
      // the new channel
      CString *csChannelName = new CString(csNewChannel);
      m_ListControl.SetItemData(iAppend, (DWORD)csChannelName);
      
      m_ListControl.SetState(iAppend, 1, 0);
      SelectChannel(csNewChannel);
      SetAsActiveChannel();
      iLastChannel++;

      // set channel name to channel title
      m_TabControl.GetTransferSettings(csNewChannel);
      // metadata
      CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
      CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];
      meta->GetMetadata(trsp->m_csChannel, FALSE);
      m_ListControl.SetItemText(iAppend, 0, meta->m_csTitle);

      CString csFileDownload = pApp->csTmpDirectory + _T("\\tmp2.xml");
      BOOL bResult = valid.Validate(trsp->m_csChannel, csFileDownload);
      if(bResult)
      {
         bResult = CompareFiles(csFile, csFileDownload);
         CString csMessage;
         csMessage.LoadString((bResult) ? IDS_SUCCESS : IDS_NOTEQUALS);
         AfxMessageBox(csMessage, MB_ICONINFORMATION); 
      }
      else
      {
         CString csMessage;
         csMessage.LoadString(IDS_NOTEQUALS);
         AfxMessageBox(csMessage, MB_ICONINFORMATION); 
      }
	  wait->ShowWindow(SW_HIDE);
   }

   EnableControls();
}

void CGeneralSettings::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   
   // item
   int iSelected = pNMListView->iItem;
   if(iSelected != -1)
   {
	   	CWnd *wait = FromHandle(m_HWaitDlg);
		wait->ShowWindow(SW_SHOW);
		wait->CenterWindow(this);
		wait->BringWindowToTop();
		wait->UpdateWindow();
      // transfer
      //CString csKey = m_ListControl.GetItemText(iSelected, 0);
      CString *csKey = (CString *)m_ListControl.GetItemData(iSelected);
      m_TabControl.GetTransferSettings((LPCTSTR)*csKey);
      // metadata
      CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
      CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];
      meta->GetMetadata(trsp->m_csChannel, FALSE);
      m_ListControl.SetItemText(iSelected, 0, meta->m_csTitle);
	  wait->ShowWindow(SW_HIDE);
   }

   // subitem
   if(pNMListView->iSubItem == 1)
   {
      SetAsActiveChannel();
   }
   
   *pResult = 0;
   SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CGeneralSettings::SelectItem(int iItem)
{
   int iCount = m_ListControl.GetItemCount();
   if(iCount != 0)
   {
      m_ListControl.SetFocus();
      m_ListControl.SetItemState(iItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
      m_ListControl.RedrawItems(0, iCount - 1);
      Refresh();
   }
}

void CGeneralSettings::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
		SelectItem(0);
}

void CGeneralSettings::SetAsActiveChannel() 
{
	// TODO: Add your command handler code here
   int iSelected = m_ListControl.GetNextItem(-1, LVNI_SELECTED);
   // no selected item
   if(iSelected == -1)
   {
      return;
   }

   unsigned uCount = m_ListControl.GetItemCount();
   if (uCount > 0)
   {
      for (unsigned i = 0;i < uCount;i++)
      {
         //CString pszKey = m_ListControl.GetItemText(i, 0);
         CString *pszKey = (CString *)m_ListControl.GetItemData(i);
         UINT uState = m_ListControl.GetItemState(i, LVIS_SELECTED);

         bool bSelected = (uState == LVIS_SELECTED);       
         if(reg.Open(*pszKey) == ERROR_SUCCESS)
         {
            reg.WriteProfileInt(_T("Active"), (bSelected) ? 1 :0);   
            reg.Close();
            m_ListControl.SetState(i, 1, bSelected);
         }
      }
   }
}

//////////////////////////
// NETWORK
/////////////////////
void CGeneralSettings::LoadFromRegistry(CString pszKey)
{
   // Get a pointer to Application object
   if(reg.Open(pszKey) == ERROR_SUCCESS)
   {
	   CString csTemp(_T(""));
      m_iService      = reg.GetProfileInt(_T("Service"), FTP_SERVICE);   
	  
      csTemp     = reg.GetProfileString(_T("Channel"), _T(""));
	  m_csChannel = csTemp.GetString();
      csTemp      = reg.GetProfileString(_T("Server"), _T(""));
	  m_csServer = csTemp.GetString();
      m_nPort         = reg.GetProfileInt(_T("Port"), 21);   
      csTemp    = reg.GetProfileString(_T("Username"), _T(""));
	  m_csUsername = csTemp.GetString();
      m_bSavePassword = reg.GetProfileInt(_T("SavePassword"), TRUE);   
      if(m_bSavePassword)
      {
         CCrypto crypt;
         CString csAux = reg.GetProfileString(_T("Password"), _T(""));
		 CString csPassword = csAux.GetString();
         m_csPassword = crypt.Decode(csPassword);
      }
      
      csTemp = reg.GetProfileString(_T("ChannelDirectory"), _T("/"));
	  m_csChannelDirectory = csTemp.GetString();
      csTemp   = reg.GetProfileString(_T("MediaDirectory"), _T("/"));
	  m_csMediaDirectory = csTemp.GetString();

      if(!m_csMediaDirectory.IsEmpty())
      {
         if(m_csMediaDirectory.Right(1) != _T("/"))
         {
            m_csMediaDirectory += _T("/");
         }
      }

      if(!m_csChannelDirectory.IsEmpty())
      {
         if(m_csChannelDirectory.Right(1) != _T("/"))
         {
            m_csChannelDirectory += _T("/");
         }
      }

      csTemp     = reg.GetProfileString(_T("MediaFilePath"), _T("/"));
	  m_csMediaFileUrl = csTemp.GetString();

      m_iSSHVersion  = reg.GetProfileInt(_T("SSHVersion"), -1);
      m_iIPVersion   = reg.GetProfileInt(_T("IPVersion"), -1);
      m_bCompression = reg.GetProfileInt(_T("Compression"), 0);
      m_bPrivateKey  = reg.GetProfileInt(_T("PrivateKey"), 0);

      reg.Close();

      if(m_csMediaFileUrl.IsEmpty())
      {
         m_csMediaFileUrl += _T("/");
      }
      else
      {
         if(m_csMediaFileUrl.Right(1) != _T("/"))
         {
            m_csMediaFileUrl += _T("/");
         }
      }
   }
}

BOOL CGeneralSettings::DeleteChannel(CString pszKey)
{
   BOOL bResult = FALSE;
   // load settings
   LoadFromRegistry(pszKey);
   // 4. ask for password if necessary
   if(!m_bSavePassword)
   {
      if(!AskForPassword())
      {
         return bResult;
      }
   }

   if(m_iService == 0)
   {
      // FTP
      CFtp ftp;
      bResult = ftp.Open(m_csServer, m_nPort, m_csUsername, m_csPassword);
      if(bResult)
      {
         CString csXml = GetFileName(m_csChannel);
         bResult = ftp.Delete(csXml, m_csChannelDirectory);
         ftp.Close();
      }
   }
   else
   {
      CString csXml =  m_csChannelDirectory + GetFileName(m_csChannel);
      bResult = SshDelete(csXml, FALSE);
   }

   return bResult;
}

BOOL CGeneralSettings::AskForPassword()
{
   BOOL bResult = FALSE;
   CPasswordDlg dlgPsw;
   dlgPsw.m_csUsername = m_csUsername;
   if(dlgPsw.DoModal() == IDOK)
   {
      m_csPassword = dlgPsw.m_csPassword;
      bResult = TRUE;
   }
   return bResult;
}

CString CGeneralSettings::GetFileName(CString csFilePath)
{
   CString csFileName;
   csFileName = csFilePath;
   
   int iSize = csFilePath.GetLength();
   for(int i = iSize - 1;i > 0;i--)
   {
      if ((csFilePath[i] == _T('\\')) || (csFilePath[i] == _T('/')))
      {
         csFileName = csFilePath.Right(iSize - i - 1);
         break;
      }
   }
   
   return csFileName;
}

BOOL CGeneralSettings::UploadChannel(CString pszKey, CString csFile)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
   BOOL bResult = FALSE;
   // load settings
   LoadFromRegistry(pszKey);
   // 4. ask for password if necessary
   if(!m_bSavePassword)
   {
      if(!AskForPassword())
      {
         return bResult;
      }
   }

   if(m_iService == 0)
   {
      // FTP
      CFtp ftp;
      bResult = ftp.Open(m_csServer, m_nPort, m_csUsername, m_csPassword);
      if(bResult)
      {
         CString csNewChannel = GetFileName(m_csChannel);
         bResult = ftp.Upload(csFile, csNewChannel, m_csChannelDirectory);
         ftp.Close();
      }
   }
   else
   {
      CString csNewChannel = GetFileName(m_csChannel);
      CString csXmlFile = m_csChannelDirectory + csNewChannel;
      bResult = SshUpload(csFile, csXmlFile, TRUE);
   }
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
   return bResult;
}

////////////////////////////////////
// SSH
////////////////////////////////////
typedef int (__stdcall *SSHPROC)(int, char**, HWND, int); 

int CGeneralSettings::CallSsh(int argc, WCHAR** argv, HWND handle, int iDelete = 0)
{
   // argv: Convert wchar_t** to char**
   char** szargv = new char*[argc]; 
   for (int i = 0; i < argc; ++i)
   {
      //int nLen = wcslen(argv[i]);
      int nLen = ::WideCharToMultiByte(CP_ACP, 0, argv[i], -1, NULL, 0, NULL, NULL);
      //            int nLen = ::WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, NULL, 0, NULL, NULL);
      szargv[i] = new char[nLen + 1];
      ::WideCharToMultiByte(CP_ACP, 0, argv[i], -1, szargv[i], nLen + 1, NULL, NULL);
      //            ::WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, szargv[i], nLen + 1, NULL, NULL);
      szargv[i][nLen] = 0;
   }

   int iResult = CallSsh(argc, szargv, handle, iDelete);

   delete[] szargv;
   szargv = NULL;

   return iResult;
}

int CGeneralSettings::CallSsh(int argc, char** argv, HWND handle, int iDelete = 0)
{
   int iResult = 1;
   HMODULE hModule = ::LoadLibrary(_T("sshdll.dll"));
   if(hModule != NULL)
   {
      SSHPROC SshProc = (SSHPROC)::GetProcAddress((HINSTANCE)hModule, (LPCSTR)1);
      if(SshProc != NULL)
      {
         iResult = (SshProc)(argc, argv, handle, iDelete);
      }
      ::FreeLibrary(hModule);
   }
   else
   {
      CString csMessage;
      csMessage.LoadString(IDM_SSHDLL);
      AfxMessageBox(csMessage, MB_ICONSTOP);
   }
   return iResult;
}

BOOL CGeneralSettings::SshUpload(CString csSrcFileName, CString csDstFileName, BOOL bQuit = FALSE)
{
   BOOL bResult = FALSE;
   iArgc = 0;
   csArgc[iArgc++] = _T("podcaster.exe"); // application name
   csArgc[iArgc++] = (m_iService == 1) ? _T("-scp") : _T("-sftp");
   csArgc[iArgc++] = (m_iSSHVersion == 0) ? _T("-1") : _T("-2"); //ssh version
   csArgc[iArgc++] = (m_iIPVersion == 0) ? _T("-4") : _T("-6"); //ip version

   if(bQuit)
   {
      csArgc[iArgc++] = _T("-q"); // quit
   }

   if(m_bCompression)
   {
      csArgc[iArgc++] = _T("-C"); // compression
   }

   if(m_bPrivateKey)
   {
      csArgc[iArgc++] = _T("-i"); // username
      csArgc[iArgc++] = m_csPrivateKeyFile;
   }
   
   csArgc[iArgc++] = _T("-l"); // username
   csArgc[iArgc++] = m_csUsername; // 
   csArgc[iArgc++] = _T("-pw"); // password
   csArgc[iArgc++] = m_csPassword;  
   csArgc[iArgc++] = _T("-P"); // port
   csArgc[iArgc++].Format(_T("%d"), m_nPort);
   //source
   csArgc[iArgc++] = csSrcFileName;
   // destination
   csArgc[iArgc++].Format(_T("%s:%s"), m_csServer, csDstFileName);

   _TCHAR **atszParams = new _TCHAR*[32];
   for(int i=0;i < iArgc;i++)
   {
      LPCTSTR tszOneParam = csArgc[i];
      int iOneParamLen = _tcslen(tszOneParam);
      atszParams[i] = new _TCHAR[iOneParamLen + 1];
      _tcscpy(atszParams[i], tszOneParam);
   }

   bResult = CallSsh(iArgc, atszParams, m_hWnd, 0);   
   
   return (bResult == 0); 
}

BOOL CGeneralSettings::SshDelete(CString csSrcFileName, BOOL bQuit = FALSE)
{
   BOOL bResult = FALSE;

   for(int k=0;k < csSrcFileName.GetLength();k++)
   {
      // wildcards not allowed
      if( (csSrcFileName[k] == _T('*')) || (csSrcFileName[k] == _T('?')) )
      {
         return FALSE;
      }
   }


   iArgc = 0;
   csArgc[iArgc++] = _T("podcaster.exe"); // application name
   csArgc[iArgc++] = _T("-scp");
   csArgc[iArgc++] = (m_iSSHVersion == 0) ? _T("-1") : _T("-2"); //ssh version
   csArgc[iArgc++] = (m_iIPVersion == 0) ? _T("-4") : _T("-6"); //ip version

   if(bQuit)
   {
      csArgc[iArgc++] = _T("-q"); // quit
   }

   if(m_bCompression)
   {
      csArgc[iArgc++] = _T("-C"); // compression
   }
   
   if(m_bPrivateKey)
   {
      csArgc[iArgc++] = _T("-i"); // username
      csArgc[iArgc++] = m_csPrivateKeyFile;
   }

   csArgc[iArgc++] = _T("-l"); // username
   csArgc[iArgc++] = m_csUsername; // 
   csArgc[iArgc++] = _T("-pw"); // password
   csArgc[iArgc++] = m_csPassword;  
   csArgc[iArgc++] = _T("-P"); // port
   csArgc[iArgc++].Format(_T("%d"), m_nPort);
   //source
   csArgc[iArgc++].Format(_T("%s:%s"), m_csServer, csSrcFileName);
   // destination
   csArgc[iArgc++] = csSrcFileName;

   _TCHAR **atszParams = new _TCHAR*[32];
   for(int i=0;i < iArgc;i++)
   {
      LPCTSTR tszOneParam = csArgc[i];
      int iOneParamLen = _tcslen(tszOneParam);
      atszParams[i] = new _TCHAR[iOneParamLen + 1];
      _tcscpy(atszParams[i], tszOneParam);
   }

   bResult = CallSsh(iArgc, atszParams, m_hWnd, 1);   

   return (bResult == 0); 
}

BOOL CGeneralSettings::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class

   // clean up list to prevent memory leak
   unsigned uCount = m_ListControl.GetItemCount();
   for (unsigned j = 0;j < uCount;j++)
   {
      CString *pszKey = (CString *)m_ListControl.GetItemData(j);
      if(pszKey != NULL)
      {
         delete pszKey;
      }
   }
	
	return CDialog::DestroyWindow();
}

void CGeneralSettings::EnableControls()
{
   BOOL bEnable = (m_ListControl.GetItemCount() > 0);
   m_btnDelete.EnableWindow(bEnable);
   m_btnRemove.EnableWindow(bEnable);
   CMetadataPage* meta = (CMetadataPage*)m_TabControl.m_Dialog[0];
   CTransferPage* trsp = (CTransferPage*)m_TabControl.m_Dialog[1];
   meta->m_btnChange.EnableWindow(bEnable);
   trsp->m_btnChange.EnableWindow(bEnable);
}

BOOL CGeneralSettings::CompareFiles(CString srcFileName, CString dstFileName)
{
   BOOL bResult = FALSE;
   BYTE btSrc, btDst;
   
   CFile src, dst;

   if(src.Open(srcFileName,  CFile::modeRead | CFile::shareDenyWrite, NULL))
   {
      if(dst.Open(dstFileName,  CFile::modeRead | CFile::shareDenyWrite, NULL))
      {
         DWORD dwSrcLen = src.GetLength();
         DWORD dwDstLen = dst.GetLength();
         if(dwSrcLen == dwDstLen)
         {
            for(unsigned i=0;i < src.GetLength();i++)
            {
               src.Read((void *)&btSrc, 1);
               dst.Read((void *)&btDst, 1);
               bResult = (btSrc == btDst);
               if(!bResult)
               {
                  break;
               }               
            }
         }
         dst.Close();
      }
      src.Close();
   }

   return bResult;
}

void CGeneralSettings::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
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

//int CGeneralSettings::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	CWaitDlg *wait=(CWaitDlg*)FromHandle(m_HWaitDlg);
//	if(wait->IsWindowVisible())
//	{
//		wait->BringWindowToTop();
//		wait->UpdateWindow();
//		UpdateWindow();
//	}
//
//	return CDialog::OnMouseActivate(pDesktopWnd, nHitTest, message);
//}
