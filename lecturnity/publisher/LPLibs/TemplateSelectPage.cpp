// TemplateSelectPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "TemplateSelectPage.h"
#include "MfcUtils.h" // Localize

#include "TemplateDlg.h"
#include "TemplateEngine.h"

#include "lutils.h" // LRegistry...

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CTemplateSelectPage 

IMPLEMENT_DYNCREATE(CTemplateSelectPage, CPropertyPage)

CTemplateSelectPage::CTemplateSelectPage() : CPropertyPage(CTemplateSelectPage::IDD)
{
	//{{AFX_DATA_INIT(CTemplateSelectPage)
	//}}AFX_DATA_INIT

   if (FALSE != m_csCaption.LoadString(IDD))
   {
      m_psp.pszTitle = m_csCaption;
      m_psp.dwFlags |= PSP_USETITLE;
   }

   m_pConfigurePage = NULL;
}

CTemplateSelectPage::~CTemplateSelectPage()
{
}

void CTemplateSelectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateSelectPage)
	DDX_Control(pDX, IDC_TMP_SELECT_SCORM, m_lbScormCompatible);
	DDX_Control(pDX, IDC_TMP_SELECT_PREVIEW, m_cbPreview);
	DDX_Control(pDX, IDC_TMP_SELECT_TEMPLATES, m_listTemplates);
	DDX_Control(pDX, IDC_TMP_SELECT_REMOVE, m_btRemove);
	DDX_Control(pDX, IDC_TMP_SELECT_PATH, m_lbPath);
	DDX_Control(pDX, IDC_TMP_SELECT_DESCRIPTION, m_lbDescription);
	DDX_Control(pDX, IDC_TMP_SELECT_COPYRIGHT, m_lbCopyright);
	DDX_Control(pDX, IDC_TMP_SELECT_AUTHOR, m_lbAuthor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateSelectPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTemplateSelectPage)
	ON_LBN_SELCHANGE(IDC_TMP_SELECT_TEMPLATES, OnChangeTemplate)
	ON_BN_CLICKED(IDC_TMP_SELECT_ADD, OnAddTemplate)
	ON_BN_CLICKED(IDC_TMP_SELECT_REMOVE, OnRemoveTemplate)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TMP_SELECT_PREVIEW, OnTogglePreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTemplateSelectPage 

BOOL CTemplateSelectPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   DWORD adwIds[] =
   {
      IDC_TMP_SELECT_GROUP,
      IDC_TMP_SELECT_DESC,
      IDC_TMP_SELECT_ADD,
      IDC_TMP_SELECT_REMOVE,
      IDC_TMP_SELECT_INFORMATION_GROUP,
      IDC_TMP_SELECT_AUTHOR_LABEL,
      IDC_TMP_SELECT_COPYRIGHT_LABEL,
      IDC_TMP_SELECT_PATH_LABEL,
      IDC_TMP_SELECT_DESCRIPTION_LABEL,
      IDC_TMP_SELECT_PREVIEW,
      IDC_TMP_SELECT_SCORM_LABEL,
      -1
   };
   MfcUtils::Localize(this, adwIds);
	
   m_lbAuthor.SetWindowText(_T(""));
   m_lbCopyright.SetWindowText(_T(""));
   m_lbDescription.SetWindowText(_T(""));
   m_lbPath.SetWindowText(_T(""));
   m_btRemove.EnableWindow(FALSE);

   // Create the preview dialog, but do not display it immediately
   m_previewDlg.Create(IDD_TEMPLATE_PREVIEW, GetParent());
   m_previewDlg.ShowWindow(SW_HIDE);

   // Read settings from registry
   int i = 0;
   int nCount = 0;
   bool success = true;
   while (success)
   {
      _TCHAR *tszDefault = _T("<undefined>");
      _TCHAR tszPath[_MAX_PATH];
      CString csRegValue;
      csRegValue.Format(_T("TemplatePath%d"), nCount);
      DWORD dwSize = _MAX_PATH;
      LRegistry::ReadSettingsString(_T("Publisher\\Templates"),
         csRegValue, tszPath, &dwSize, tszDefault);

      CString csPath(tszPath);
      if (csPath != tszDefault)
      {
         AddTemplateToList(csPath);
         nCount++;
      }
      else
      {
         success = false;
      }
   }

   // Always try to add the ones in the "Web Templates" directory
   {
      _TCHAR tszInstallDir[_MAX_PATH];
      DWORD dwSize = _MAX_PATH;
      success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
         _T("Software\\imc AG\\LECTURNITY"), _T("InstallDir"), tszInstallDir, &dwSize);
      if (success)
      {
         CString csInstallDir(tszInstallDir);
         if (csInstallDir[csInstallDir.GetLength() - 1] != '\\')
            csInstallDir += CString(_T("\\"));

         bool bFound = true;
         int nCount = 1;
         while (bFound)
         {
            bFound = false;
            CString csTemplateDir;
            csTemplateDir.Format(_T("%sPublisher\\Web Templates\\WebPlayer%d"), csInstallDir, nCount);
            DWORD dwAttr = ::GetFileAttributes(csTemplateDir);
            if (dwAttr != 0xFFFFFFFF)
            {
               if ((dwAttr & FILE_ATTRIBUTE_DIRECTORY) > 0)
               {
                  bFound = true;
                  nCount++;
                  AddTemplateToList(csTemplateDir);
               }
            }
         }
      }
   }

   // Check for even more templates in the registry
   for (int nLoop = 0; nLoop < 2; ++nLoop)
   {
      HKEY hkKey = NULL;
      if (nLoop == 1)
         hkKey = HKEY_CURRENT_USER;
      else
         hkKey = HKEY_LOCAL_MACHINE;

      DWORD dwTemplateCount = 0;
      bool bHasMoreTemplates = LRegistry::EnumRegistryValues(hkKey, 
         _T("Software\\imc AG\\LECTURNITY\\Publisher\\MoreTemplates"),
         NULL, &dwTemplateCount);
      if (bHasMoreTemplates)
      {
         LRegistry::ENTRY **paEntries = new LRegistry::ENTRY*[dwTemplateCount];
         for (i = 0; i < dwTemplateCount; ++i)
            paEntries[i] = new LRegistry::ENTRY;

         success = LRegistry::EnumRegistryValues(hkKey, 
            _T("Software\\imc AG\\LECTURNITY\\Publisher\\MoreTemplates"),
            paEntries, &dwTemplateCount, true);
         if (success)
         {
            for (i = 0; i < dwTemplateCount; ++i)
            {
               if (paEntries[i]->dwType == REG_SZ)
               {
                  _TCHAR tszConfigPath[1024];
                  DWORD dwSize = 1024;
                  bool successRead = LRegistry::ReadStringRegistryEntry(hkKey,
                     _T("Software\\imc AG\\LECTURNITY\\Publisher\\MoreTemplates"),
                     paEntries[i]->lsName, tszConfigPath, &dwSize);
                  if (successRead)
                  {
                     CString csConfigPath(tszConfigPath);
                     if (csConfigPath[csConfigPath.GetLength() - 1] == _T('\\'))
                        csConfigPath = csConfigPath.Left(csConfigPath.GetLength() - 1);

                     DWORD dwAttr = ::GetFileAttributes(csConfigPath);
                     if (dwAttr != 0xFFFFFFFF &&
                         ((dwAttr & FILE_ATTRIBUTE_DIRECTORY) > 0))
                     {
                        AddTemplateToList(csConfigPath);
                     }
                  }
               }
            }
         }

         for (i=0; i<dwTemplateCount; ++i)
            delete paEntries[i];
         delete[] paEntries;
         paEntries = NULL;
      }
   }

   // Try to find the selected one
   if (m_caTemplates.GetSize() > 0)
   {
      CString csCurrentPath;
      RETRIEVE_CSTRING(g_templateEngine.GetCurrentTemplatePath, csCurrentPath);

      int nSel = -1;
      if (csCurrentPath.GetLength() > 0)
      {
         for (i = 0; i < m_caTemplates.GetSize(); ++i)
         {
            if (m_caTemplates[i] == csCurrentPath)
            {
               nSel = i;
            }
         }
      }

      if (nSel == -1)
         nSel = 0;

      m_listTemplates.SetCurSel(nSel);
      SelectTemplate(nSel);
      // Note: this also loads the variables and enables the "config" page/tab
      m_nOriginalTemplate = nSel;

      /*
      m_lbAuthor.SetWindowText(m_caTemplates[nSel].m_csAuthor);
      m_lbCopyright.SetWindowText(m_caTemplates[nSel].m_csCopyright);
      m_lbPath.SetWindowText(GetSplittedPathString(m_caTemplates[nSel].m_csPath));
      m_lbDescription.SetWindowText(m_caTemplates[nSel].m_csDescription);
      */
   }
   else
   {
      // Disable "Remove" button
      m_btRemove.EnableWindow(FALSE);
   }

   // Also disable "Remove" button, if no entry is selected
   if (m_listTemplates.GetCurSel() == LB_ERR)
      m_btRemove.EnableWindow(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

bool CTemplateSelectPage::AddTemplateToList(const _TCHAR *tszPath)
{
   CString csPath(tszPath);
   if (csPath[csPath.GetLength() - 1] != '\\')
      csPath += CString(_T("\\"));

   bool success = true;
   if (success)
   {
      // Does it exist already?
      bool bExists = false;
      for (int i=0; i<m_caTemplates.GetSize(); ++i)
      {
         if (m_caTemplates[i] == csPath)
            bExists = true;
      }

      if (bExists)
      {
         success = false;
      }
   }

   CString csTemplateTitle;
   if (success)
   {
      UINT nRes = g_templateEngine.CheckTemplate(csPath, csTemplateTitle);
      success = (S_OK == nRes);
   }

   if (success)
   {
      m_listTemplates.AddString(csTemplateTitle);
      m_caTemplates.Add(csPath);

      if (m_caTemplates.GetSize() > 1)
         m_btRemove.EnableWindow(TRUE);
   }

   return success;
}

void CTemplateSelectPage::SaveTemplateConfiguration(bool bSavePath)
{
   bool bForAllUsers = false;

   if (TRUE == LMisc::IsUserAdmin() &&
       TRUE == LMisc::IsWindowsNT())
   {
      CString csMsg;
      csMsg.LoadString(IDS_SAVE_TEMPLATE_FOR_ALL);
      int nRes = MessageBox(csMsg, NULL, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
      
      if (IDYES == nRes)
         bForAllUsers = true;
   }
      /*
      {
         // Save template settings as standard for all users
         g_templateEngine.StoreKeywordConfiguration(true);
      }
      else
      {
         // Save template settings for current user
         g_templateEngine.StoreKeywordConfiguration(false);
      }
   }
   else
   {
      // Windows 9x or user is not Administrator
      g_templateEngine.StoreKeywordConfiguration(false);
   }
   */

   g_templateEngine.WriteSettings(bForAllUsers, bSavePath);
}

void CTemplateSelectPage::SelectTemplate(int nIndex, int nReset /*=TEMPLATE_RESET_NORESET*/)
{
   if (g_templateEngine.HasTemplateChanged() && nReset == TEMPLATE_RESET_NORESET)
   {
      CString csTitle;
      RETRIEVE_CSTRING(g_templateEngine.GetTitle, csTitle);
      CString csMsg;
      csMsg.Format(IDS_TMP_TEMPLATE_CHANGED, csTitle);
      int nRes = MessageBox(csMsg, NULL, MB_YESNO | MB_ICONQUESTION);
      if (IDYES == nRes)
      {
         SaveTemplateConfiguration(false);
      }
   }

   CTemplateDlg *pDlg = (CTemplateDlg *) GetParent();
   m_caKeywords.RemoveAll();
   if (m_pConfigurePage)
   {
      pDlg->RemovePage(m_pConfigurePage);
      delete m_pConfigurePage;
      m_pConfigurePage = NULL;
   }

   CString csPath = m_caTemplates[nIndex];
   CString csDummy;
   UINT nRes = g_templateEngine.LoadTemplate(csPath, nReset, false, csDummy);
   CString csTitle;
   CString csDescription;
   CString csAuthor;
   CString csCopyright;
   CString csPreviewImage;
   RETRIEVE_CSTRING(g_templateEngine.GetTitle, csTitle);
   RETRIEVE_CSTRING(g_templateEngine.GetDescription, csDescription);
   RETRIEVE_CSTRING(g_templateEngine.GetAuthor, csAuthor);
   RETRIEVE_CSTRING(g_templateEngine.GetCopyright, csCopyright);
   RETRIEVE_CSTRING(g_templateEngine.GetPreviewImage, csPreviewImage);
   
   m_lbDescription.SetWindowText(csDescription);  
   m_lbPath.SetWindowText(csPath);
   m_lbAuthor.SetWindowText(csAuthor);
   m_lbCopyright.SetWindowText(csCopyright);
   m_previewDlg.SetPreviewImage(csPreviewImage);

   g_templateEngine.GetTemplateKeywords(m_caKeywords);
   if (m_caKeywords.GetSize() > 0)
   {
      m_pConfigurePage = new CTemplateConfigurePage(this);
      m_pConfigurePage->SetKeywords(m_caKeywords);
      pDlg->AddPage(m_pConfigurePage);
   }

   BOOL bScormCompliant = FALSE;
   g_templateEngine.IsScormCompliant(&bScormCompliant);
   CString csCompliant;
   if (FALSE == bScormCompliant)
      csCompliant.LoadString(IDS_NOT_COMPATIBLE);
   else
      csCompliant.LoadString(IDS_COMPATIBLE);
   m_lbScormCompatible.SetWindowText(csCompliant);
}

void CTemplateSelectPage::OnChangeTemplate() 
{
   SetModified();
   int nSel = m_listTemplates.GetCurSel();
   SelectTemplate(nSel);
}

void CTemplateSelectPage::ResetTemplateConfiguration(bool bTotalReset)
{
   SelectTemplate(m_listTemplates.GetCurSel(), 
      bTotalReset ? TEMPLATE_RESET_TEMPLATEDEFAULT : TEMPLATE_RESET_SYSTEMDEFAULT);
   ((CPropertySheet *) GetParent())->SetActivePage(1);
   CString csMessage;
   csMessage.LoadString(IDS_TEMPLATE_VAR_RESETNOTIFY);
   MessageBox(csMessage, _T("Publisher"), MB_OK | MB_ICONINFORMATION);
}

BOOL CTemplateSelectPage::OnApply() 
{
   if (g_templateEngine.IsSaveToRegistry())
   {
      //if (g_templateEngine.HasTemplateChanged())
         SaveTemplateConfiguration(true);
      //g_templateEngine.WriteSettings();

      g_templateEngine.UpdateStandardNavigationStates();

      // Now write the currently loaded templates to the
      // registry. But first delete any old entries.
      bool success = true;
      int nIndex = 0;
      while (success)
      {
         CString csValue;
         csValue.Format(_T("TemplatePath%d"), nIndex++);
         success = LRegistry::DeleteRegistryValue(HKEY_CURRENT_USER,
            _T("SOFTWARE\\imc AG\\LECTURNITY\\Publisher\\Templates"),
            csValue);
      }
      int nCount = m_caTemplates.GetSize();
      for (int i=0; i<nCount; ++i)
      {
         CString csValue;
         csValue.Format(_T("TemplatePath%d"), i);
         LRegistry::WriteSettingsString(_T("Publisher\\Templates"),
            csValue, m_caTemplates[i]);
      }

      g_templateEngine.OverrideCurrentPath(NULL, true);
   }
   else
      g_templateEngine.OverrideCurrentPath();

	return CPropertyPage::OnApply();
}

void CTemplateSelectPage::OnCancel() 
{
   // Force a reload of the old template. If
   // the template has been changed, the user is
   // asked if he wants to save the changes.
   if (m_nOriginalTemplate >= 0)
      SelectTemplate(m_nOriginalTemplate);
	
	CPropertyPage::OnCancel();
}

void CTemplateSelectPage::OnAddTemplate() 
{
	CString csFilter;
   csFilter.LoadString(IDS_TMP_CONFIG_FILEFILTER);
   CFileDialog fd(TRUE, NULL, _T("config.xml"), OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
      csFilter, this);
   if (IDOK == fd.DoModal())
   {
      CString csFileName = fd.GetPathName();
      int nLastSlash = csFileName.ReverseFind('\\');
      CString csPath = csFileName.Mid(0, nLastSlash + 1);
      bool success = AddTemplateToList(csPath);
      if (!success)
      {
         CString csMsg;
         csMsg.Format(IDS_TMP_CONFIG_NOTVALID, csPath);
         MessageBox(csMsg, _T("Publisher"), MB_OK | MB_ICONWARNING);
      }
      else
      {
         m_listTemplates.SetCurSel(m_listTemplates.GetCount() - 1);
         SelectTemplate(m_listTemplates.GetCount() - 1);
      }
   }
}

void CTemplateSelectPage::OnRemoveTemplate() 
{
   int nSel = m_listTemplates.GetCurSel();
   m_listTemplates.DeleteString(nSel);
   m_caTemplates.RemoveAt(nSel);
   if (nSel >= m_listTemplates.GetCount())
      nSel--;
   m_listTemplates.SetCurSel(nSel);
   // If we want to remove the template, don't care if it has
   // changed.
   g_templateEngine.SetTemplateChanged(false);
   SelectTemplate(nSel);

   if (m_listTemplates.GetCount() <= 1)
      m_btRemove.EnableWindow(FALSE);
}

void CTemplateSelectPage::OnDestroy() 
{
   //m_previewDlg.SetPreviewImage(NULL);
   m_previewDlg.DestroyWindow();	

   CPropertyPage::OnDestroy();
}

void CTemplateSelectPage::OnTogglePreview() 
{
   bool bPreview = (m_cbPreview.GetCheck() == BST_CHECKED);
   if (bPreview)
   {
      CRect rcWin;
      GetParent()->GetWindowRect(&rcWin);
      m_previewDlg.SetWindowPos(&wndTop, rcWin.right, rcWin.top, 0, 0, 
         SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
   }
   else
   {
      m_previewDlg.ShowWindow(SW_HIDE);
   }
}
