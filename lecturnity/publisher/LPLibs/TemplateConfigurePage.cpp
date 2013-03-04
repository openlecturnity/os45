// TemplateConfigurePage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "TemplateConfigurePage.h"
#include "resource.h"

#include "TemplateEngine.h"

#include "TemplateSelectPage.h"
#include "TemplateVarColor.h"
#include "TemplateVarImage.h"
#include "TemplateVarGeneral.h"
#include "TemplateVarText.h"
#include "TemplateVarSelect.h"
#include "TemplateVarInt.h"

#include "MfcUtils.h" // Localize

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateConfigurePage 


CTemplateConfigurePage::CTemplateConfigurePage(CTemplateSelectPage *pSelectPage)
	: CPropertyPage(CTemplateConfigurePage::IDD), m_pSelectPage(pSelectPage)
{
	//{{AFX_DATA_INIT(CTemplateConfigurePage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   if (FALSE != m_csCaption.LoadString(IDD))
   {
      m_psp.pszTitle = m_csCaption;
      m_psp.dwFlags |= PSP_USETITLE;
   }
}


void CTemplateConfigurePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateConfigurePage)
	DDX_Control(pDX, IDC_TEMPLATE_VAR_NOVARSELECTED, m_lbNoVarSelected);
	DDX_Control(pDX, IDC_TEMPLATE_VARIABLES, m_lbVariables);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateConfigurePage, CPropertyPage)
	//{{AFX_MSG_MAP(CTemplateConfigurePage)
	ON_LBN_SELCHANGE(IDC_TEMPLATE_VARIABLES, OnChangeVariable)
	ON_BN_CLICKED(IDC_TEMPLATE_VAR_RESETALL, OnResetAllVariables)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTemplateConfigurePage 

BOOL CTemplateConfigurePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   m_lbVariables.ResetContent();

   // Tab stops are set in MyListBox::DrawItem()
//   // Set tab stops
//   int nTabs[] = {140, 190};
//   m_lbVariables.SetTabStops(2, nTabs);

   // Set a horizontal extent (fixed end for horizontal scrollbar)
   m_lbVariables.SetHorizontalExtent(650);

   DWORD adwIds[] =
   {
      IDC_TMP_VARS_GROUP,
      IDC_TMP_VARS_DESC,
      IDC_TMP_VARS_CONTENT_GROUP,
      IDC_TEMPLATE_VAR_NOVARSELECTED,
      IDC_TEMPLATE_VAR_RESETALL,
      -1
   };
   MfcUtils::Localize(this, adwIds);

   CString csName, csType, csContent, csDesc;

   // Add a header line:
   csName.LoadString(IDC_TMP_VAR_VARIABLE_LABEL);
   csType.LoadString(IDC_TMP_VAR_TYPE_LABEL);
   csContent.LoadString(IDC_TMP_VAR_CONTENT_LABEL);
   csDesc.Format(_T("%s\t%s\t%s"), csName, csType, csContent);
   m_lbVariables.AddString(csDesc);

   // Now add the keywords
   int nKeywordCount = m_caKeywords.GetSize();
   for (int i=0; i<nKeywordCount; ++i)
   {
      CDialog *pDlg = NULL;
      CTemplateKeyword *pKey = m_caKeywords[i];
      switch (pKey->m_pConfig->dwType)
      {
      case TE_TYPE_IMAGE:
         pDlg = new CTemplateVarImage(pKey);
         pDlg->Create(IDD_TEMPLATE_VAR_IMAGE, this);
         csType.LoadString(IDC_TMP_VAR_IMAGE_TYPE);
         break;
      case TE_TYPE_INTEGER:
         pDlg = new CTemplateVarInt(pKey);
         pDlg->Create(IDD_TEMPLATE_VAR_INT, this);
         csType.LoadString(IDC_TMP_VAR_INT_TYPE);
         break;
      case TE_TYPE_COLOR:
         pDlg = new CTemplateVarColor(pKey);
         pDlg->Create(IDD_TEMPLATE_VAR_COLOR, this);
         csType.LoadString(IDC_TMP_VAR_COLOR_TYPE);
         break;
      case TE_TYPE_GENERAL:
         pDlg = new CTemplateVarGeneral(pKey);
         pDlg->Create(IDD_TEMPLATE_VAR_GENERAL, this);
         csType.LoadString(IDC_TMP_VAR_GENERAL_TYPE);
         break;
      case TE_TYPE_TEXT:
         pDlg = new CTemplateVarText(pKey);
         pDlg->Create(IDD_TEMPLATE_VAR_TEXT, this);
         csType.LoadString(IDC_TMP_VAR_TEXT_TYPE);
         break;
      case TE_TYPE_SELECT:
         pDlg = new CTemplateVarSelect(pKey);
         pDlg->Create(IDD_TEMPLATE_VAR_SELECT, this);
         csType.LoadString(IDC_TMP_VAR_SELECT_TYPE);
         break;
      }

      if (pDlg)
      {
//         CRect rcPos;
//         m_lbNoVarSelected.GetClientRect(&rcPos);
//         pDlg->SetWindowPos(&wndTop, rcPos.left, rcPos.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
         pDlg->ShowWindow(SW_HIDE);
         m_caConfigWnds.Add(pDlg);

         csName.Format(_T("%s"), pKey->m_csFriendlyName);

         csContent.Format(_T("%s"), pKey->GetReplaceString());
         if (pKey->GetType() == TE_TYPE_SELECT)
         {
            CTemplateSelectKeyword *pSelectKey = (CTemplateSelectKeyword*)pKey;
            int idx = _ttoi(pSelectKey->GetReplaceString());
            csContent.Format(_T("%s"), pSelectKey->m_caVisibleOptions.ElementAt(idx));
         }
         if (pKey->GetType() == TE_TYPE_IMAGE)
         {
            CTemplateImageKeyword *pImageKey = (CTemplateImageKeyword*)pKey;
            csContent.Format(_T("%s"), pImageKey->GetImageName());
         }

         //csName.Replace(_T("%"), _T(""));
//         csDesc.Format(_T("%s\t%s"), csName, csType);
         csDesc.Format(_T("%s\t%s\t%s"), csName, csType, csContent);
         m_lbVariables.AddString(csDesc);
      }
   }

   // 
   m_lbVariables.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CTemplateConfigurePage::OnChangeVariable() 
{
   m_lbNoVarSelected.ShowWindow(SW_HIDE);

//   int nIndex = m_lbVariables.GetCurSel();
   int nIndex = m_lbVariables.GetCurSel() - 1; // minus header line
//   int nCount = m_lbVariables.GetCount();
   int nCount = m_lbVariables.GetCount() - 1; // minus header line
   for (int i=0; i<nCount; ++i)
   {
      if (i != nIndex)
         m_caConfigWnds[i]->ShowWindow(SW_HIDE);
   }
   CRect rcLabelPos;
   m_lbNoVarSelected.GetWindowRect(&rcLabelPos);
   ScreenToClient(&rcLabelPos);
   if (nIndex >= 0)
   {
      m_caConfigWnds[nIndex]->SetWindowPos(&wndTop, rcLabelPos.left, rcLabelPos.top, 0, 0, 
         SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);
      //m_caConfigWnds[nIndex]->ShowWindow(SW_SHOW);
   }
   else
      m_lbNoVarSelected.ShowWindow(SW_SHOW);
}

void CTemplateConfigurePage::SetKeywords(CArray<CTemplateKeyword *, CTemplateKeyword *> &caKeywords)
{
   m_caKeywords.RemoveAll();
   int nSize = caKeywords.GetSize();
   for (int i=0; i<nSize; ++i)
   {
      m_caKeywords.Add(caKeywords[i]);
   }
}

CArray <CTemplateKeyword *, CTemplateKeyword *>* CTemplateConfigurePage::GetKeywords()
{
   return &m_caKeywords;
}

void CTemplateConfigurePage::UpdateSubWindows()
{
   int nSelectedIndex = m_lbVariables.GetCurSel();
   int nSize = m_caConfigWnds.GetSize();
   for (int i = 0; i < nSize; ++i)
      m_caConfigWnds.ElementAt(i)->OnInitDialog();
   OnInitDialog();
   m_lbVariables.SetCurSel(nSelectedIndex);
}

void CTemplateConfigurePage::OnResetAllVariables() 
{
   CString csMessage;
   csMessage.LoadString(IDS_TEMPLATE_VAR_RESETCONFIRM);
   if (IDYES == MessageBox(csMessage, _T("Publisher"), MB_YESNO|MB_ICONQUESTION))
   {
      // Make the SelectPage reload the template and reset
      // the configuration. It's important that no calls
      // are done after this call, as we may have been destroyed
      // in the mean time.
      csMessage.LoadString(IDS_LOAD_DEFAULT_CONFIG);
      if (IDYES == MessageBox(csMessage, _T("Publisher"), MB_YESNO|MB_ICONQUESTION))
         m_pSelectPage->ResetTemplateConfiguration(false); // Reset to system default
      else
         m_pSelectPage->ResetTemplateConfiguration(true); // Total reset
   }
}
