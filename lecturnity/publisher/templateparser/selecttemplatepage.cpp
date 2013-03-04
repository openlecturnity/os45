// selecttemplatepage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "templateparser.h"
#include "selecttemplatepage.h"

#include "lregistry.h"
#include "sgmlparser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern const char *szLECTURNITY;

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CSelectTemplatePage 

IMPLEMENT_DYNCREATE(CSelectTemplatePage, CPropertyPage)

CSelectTemplatePage::CSelectTemplatePage() : CPropertyPage(CSelectTemplatePage::IDD)
{
	//{{AFX_DATA_INIT(CSelectTemplatePage)
	//}}AFX_DATA_INIT
}

CSelectTemplatePage::~CSelectTemplatePage()
{
}

void CSelectTemplatePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectTemplatePage)
	DDX_Control(pDX, IDC_PATH, m_lbPath);
	DDX_Control(pDX, IDC_DESCRIPTION, m_lbDescription);
	DDX_Control(pDX, IDC_COPYRIGHT, m_lbCopyright);
	DDX_Control(pDX, IDC_AUTHOR, m_lbAuthor);
	DDX_Control(pDX, IDC_TEMPLATELIST, m_lsTemplates);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectTemplatePage, CPropertyPage)
	//{{AFX_MSG_MAP(CSelectTemplatePage)
	ON_LBN_SELCHANGE(IDC_TEMPLATELIST, OnChangeTemplate)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSelectTemplatePage 

BOOL CSelectTemplatePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   int nCount = 0;
   bool success = true;
   while (success)
   {
      char *szDefault = "<undefined>";
      char szPath[_MAX_PATH];
      CString csRegValue;
      csRegValue.Format(_T("TemplatePath%d"), nCount);
      DWORD dwSize = _MAX_PATH;
      LRegistry::ReadSettingsString(_T("Publisher\\Templates"),
         csRegValue, szPath, &dwSize, szDefault);

      CString csPath(szPath);
      if (csPath != szDefault)
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
      char szInstallDir[_MAX_PATH];
      DWORD dwSize = _MAX_PATH;
      success = LRegistry::ReadStringRegistryEntry(HKEY_LOCAL_MACHINE,
         _T("Software\\imc AG\\LECTURNITY"), _T("InstallDir"), szInstallDir, &dwSize);
      if (success)
      {
         CString csInstallDir(szInstallDir);
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
      else
      {
         // Emergency code
         CTemplateParserApp *pApp = (CTemplateParserApp *) AfxGetApp();
         if (pApp)
            AddTemplateToList(pApp->m_csSourcePath);
      }
   }

   // Try to find the selected one
   if (nCount > 0)
   {
      char szPath[_MAX_PATH];
      char *szDefault = "<undefined>";
      DWORD dwSize = _MAX_PATH;
      LRegistry::ReadSettingsString(_T("Publisher\\Templates"),
         _T("LastTemplate"), szPath, &dwSize, szDefault);
      CString csPath(szPath);
      int nSel = -1;
      if (csPath != szDefault)
      {
         for (int i=0; i<m_caTemplates.GetSize(); ++i)
         {
            if (m_caTemplates[i].m_csPath == csPath)
               nSel = i;
         }
      }

      if (nSel == -1)
         nSel = 0;

      m_lsTemplates.SetCurSel(nSel);
      m_lbAuthor.SetWindowText(m_caTemplates[nSel].m_csAuthor);
      m_lbCopyright.SetWindowText(m_caTemplates[nSel].m_csCopyright);
      m_lbPath.SetWindowText(GetSplittedPathString(m_caTemplates[nSel].m_csPath));
      m_lbDescription.SetWindowText(m_caTemplates[nSel].m_csDescription);
   }

   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

bool CSelectTemplatePage::AddTemplateToList(const _TCHAR *szPath)
{
   CString csPath(szPath);
   if (csPath[csPath.GetLength() - 1] != '\\')
      csPath += CString(_T("\\"));

   CString csConfig = csPath + CString(_T("config.xml"));

   CString csTitle;
   csTitle.LoadString(IDS_UNDEFINED);
   CString csDescription("");
   CString csAuthor("");
   CString csCopyright("");

   SgmlFile sgmlParser;
   SgmlElement *pRoot = NULL;
   bool success = sgmlParser.Read(csConfig, "<templateConfig");
   if (success)
   {
      pRoot = sgmlParser.GetRoot();
      success = (pRoot != NULL);
   }
   if (success)
   {
      CString csTitleLC;
      csTitleLC.LoadString(IDS_TITLE_LC);
      const char *szTitle = pRoot->GetValue(csTitleLC);
      if (szTitle)
         csTitle = szTitle;
      else
         success = false;
   }
   if (success)
   {
      CString csDescriptionLC;
      csDescriptionLC.LoadString(IDS_DESCRIPTION_LC);
      const char *szDescription = pRoot->GetValue(csDescriptionLC);
      if (szDescription)
         csDescription = szDescription;
   }
   if (success)
   {
      const char *szAuthor = pRoot->GetValue("author");
      if (szAuthor)
         csAuthor = szAuthor;
      const char *szCopyright = pRoot->GetValue("copyright");
      if (szCopyright)
         csCopyright = szCopyright;
   }

   if (success)
   {
      // Does it exist already?
      bool bExists = false;
      for (int i=0; i<m_caTemplates.GetSize(); ++i)
      {
         if (m_caTemplates[i].m_csPath == csPath)
            bExists = true;
      }

      if (!bExists)
      {
         m_caTemplates.Add(CTemplateInfo(
            csPath,
            csTitle,
            csDescription,
            csAuthor,
            csCopyright));
         m_lsTemplates.AddString(csTitle);
      }
      else
      {
         success = false;
      }
   }

   return success;
}

void CSelectTemplatePage::OnChangeTemplate() 
{
   int nSel = m_lsTemplates.GetCurSel();
   m_lbAuthor.SetWindowText(m_caTemplates[nSel].m_csAuthor);
   m_lbCopyright.SetWindowText(m_caTemplates[nSel].m_csCopyright);
   m_lbPath.SetWindowText(GetSplittedPathString(m_caTemplates[nSel].m_csPath));
   m_lbDescription.SetWindowText(m_caTemplates[nSel].m_csDescription);

   SetModified(TRUE);
}

void CSelectTemplatePage::OnBrowse() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	CString csFilter;
   csFilter.LoadString(IDS_CONFIG_FILEFILTER);
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
         csMsg.Format(IDS_NOTVALID, csPath);
         MessageBox(csMsg, _T("Publisher"), MB_OK | MB_ICONWARNING);
      }
      else
      {
         m_lsTemplates.SetCurSel(m_lsTemplates.GetCount() - 1);
         OnChangeTemplate();
      }
   }
}

BOOL CSelectTemplatePage::OnApply() 
{
   for (int i=0; i<m_caTemplates.GetSize(); ++i)
   {
      CString csRegValue;
      csRegValue.Format(_T("TemplatePath%d"), i);
      LRegistry::WriteSettingsString(_T("Publisher\\Templates"), csRegValue,
         m_caTemplates[i].m_csPath);
   }

   if (m_caTemplates.GetSize() > 0)
   {
      CTemplateParserApp *pApp = (CTemplateParserApp *) AfxGetApp();
      if (pApp)
         pApp->SetTemplateSourcePath(m_caTemplates[m_lsTemplates.GetCurSel()].m_csPath);
      LRegistry::WriteSettingsString(_T("Publisher\\Templates"), _T("LastTemplate"),
         m_caTemplates[m_lsTemplates.GetCurSel()].m_csPath);
   }

	return CPropertyPage::OnApply();
}

CString CSelectTemplatePage::GetSplittedPathString(CString csPath)
{
   // Insert a whitespace at every 64.th character 
   // --> automatic CR/LF in the text box

   int splitIndex = 64;

   if (csPath.GetLength() <= splitIndex)
      return csPath;

   CString csSplittedPath;
   csSplittedPath.Empty();
   csSplittedPath.Insert(0, csPath);

   for (int i=splitIndex; i<csPath.GetLength(); i+=splitIndex)
   {
      csSplittedPath.Insert(i, " ");
   }
   
   return csSplittedPath;
}

