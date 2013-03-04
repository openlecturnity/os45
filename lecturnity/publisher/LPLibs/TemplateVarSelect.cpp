// TemplateVarSelect.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "TemplateVarSelect.h"
#include "TemplateConfigurePage.h"

#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateVarSelect 


CTemplateVarSelect::CTemplateVarSelect(CTemplateKeyword *pKeyword)
	: CDialog(CTemplateVarSelect::IDD, NULL /* pParent */)
{
	//{{AFX_DATA_INIT(CTemplateVarSelect)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_pKeyword = (CTemplateSelectKeyword *) pKeyword;
}


void CTemplateVarSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateVarSelect)
	DDX_Control(pDX, IDC_TEMPLATE_VAR_SELECT, m_cbSelect);
	DDX_Control(pDX, IDC_TMP_VAR_VARNAME, m_edVarName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateVarSelect, CDialog)
	//{{AFX_MSG_MAP(CTemplateVarSelect)
	ON_CBN_SELCHANGE(IDC_TEMPLATE_VAR_SELECT, OnChangeSelection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTemplateVarSelect 

BOOL CTemplateVarSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   DWORD adwIds[] =
   {
      IDC_TMP_VAR_VARIABLE_LABEL,
      IDC_TMP_VAR_TYPE_LABEL,
      IDC_TMP_VAR_CONTENT_LABEL,
      IDC_TMP_VAR_WARNING_RP8_LABEL,
      IDC_TMP_VAR_SELECT_TYPE,
      -1
   };
   MfcUtils::Localize(this, adwIds);

   m_edVarName.SetWindowText(m_pKeyword->m_csName);

   m_cbSelect.ResetContent();
   int i = 0;
   for (i = 0; i < m_pKeyword->m_nOptionCount; ++i)
      m_cbSelect.AddString(m_pKeyword->m_caVisibleOptions[i]);

   CString csText;
   RETRIEVE_CSTRING(m_pKeyword->GetText, csText);

   int nSel = -1;
   for (i = 0; i < m_pKeyword->m_nOptionCount && nSel == -1; ++i)
   {
      if (csText == m_pKeyword->m_caOptions[i])
         nSel = i;
   }
   if (nSel == -1)
      nSel = 0;

   m_cbSelect.SetCurSel(nSel);

   CheckForRealPlayerWarning();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CTemplateVarSelect::OnChangeSelection() 
{
   int nSel = m_cbSelect.GetCurSel();
   m_pKeyword->SetText(m_pKeyword->m_caOptions[nSel]);

   CheckForStandardNavigation(nSel);
   CheckForRealPlayerWarning();

   g_templateEngine.SetTemplateChanged();
   ((CPropertyPage *) GetParent())->SetModified();
}

void CTemplateVarSelect::CheckForStandardNavigation(int nSel)
{
   // Special treatment for the states of 'Standard Navigation':
   // The change of one state can affect other states. 

   // What we are doing next is the following:
   // If a 'Standard Navigation' keyword is found 
   // all keywords are re-checked and maybe changed.
   // Afterwards the display is updated.

   CString csVarName;
   m_edVarName.GetWindowText(csVarName);
   // A temporary "collector" for keywords:
   CArray <CTemplateKeyword *, CTemplateKeyword *> caKeywords;
   int nKeywords = ((CTemplateConfigurePage*)GetParent())->GetKeywords()->GetSize();

   // Keyword %NavigationControlBar% controls
   // %NavigationStandardButtons%, %NavigationTimeLine% and %NavigationTimeDisplay%
   if (csVarName.Find(_T("%NavigationControlBar%")) == 0)
   {
      caKeywords.RemoveAll();
      for (int i = 0; i < nKeywords; ++i)
      {
         caKeywords.Add(((CTemplateConfigurePage*)GetParent())->GetKeywords()->ElementAt(i));

         if (caKeywords.ElementAt(i)->m_csName.Find(_T("%NavigationStandardButtons%")) == 0)
            ((CTemplateSelectKeyword*)caKeywords.ElementAt(i))->SetText(m_pKeyword->m_caOptions[nSel]);
         if (caKeywords.ElementAt(i)->m_csName.Find(_T("%NavigationTimeLine%")) == 0)
            ((CTemplateSelectKeyword*)caKeywords.ElementAt(i))->SetText(m_pKeyword->m_caOptions[nSel]);
         if (caKeywords.ElementAt(i)->m_csName.Find(_T("%NavigationTimeDisplay%")) == 0)
            ((CTemplateSelectKeyword*)caKeywords.ElementAt(i))->SetText(m_pKeyword->m_caOptions[nSel]);
      }
      // Re-set the (maybe modified) keywords:
      ((CTemplateConfigurePage*)GetParent())->SetKeywords(caKeywords);
   }

   // Change of %NavigationStandardButtons%, %NavigationTimeLine% or %NavigationTimeDisplay%
   // activates %NavigationControlBar% automatically
   if (csVarName.Find(_T("%NavigationStandardButtons%")) == 0)
   {
      caKeywords.RemoveAll();
      for (int i = 0; i < nKeywords; ++i)
      {
         caKeywords.Add(((CTemplateConfigurePage*)GetParent())->GetKeywords()->ElementAt(i));

         if (caKeywords.ElementAt(i)->m_csName.Find(_T("%NavigationControlBar%")) == 0)
            ((CTemplateSelectKeyword*)caKeywords.ElementAt(i))->SetText(m_pKeyword->m_caOptions[0]);
      }
      ((CTemplateConfigurePage*)GetParent())->SetKeywords(caKeywords);
   }
   if (csVarName.Find(_T("%NavigationTimeLine%")) == 0)
   {
      caKeywords.RemoveAll();
      for (int i = 0; i < nKeywords; ++i)
      {
         caKeywords.Add(((CTemplateConfigurePage*)GetParent())->GetKeywords()->ElementAt(i));

         if (caKeywords.ElementAt(i)->m_csName.Find(_T("%NavigationControlBar%")) == 0)
            ((CTemplateSelectKeyword*)caKeywords.ElementAt(i))->SetText(m_pKeyword->m_caOptions[0]);
      }
      ((CTemplateConfigurePage*)GetParent())->SetKeywords(caKeywords);
   }
   if (csVarName.Find(_T("%NavigationTimeDisplay%")) == 0)
   {
      caKeywords.RemoveAll();
      for (int i = 0; i < nKeywords; ++i)
      {
         caKeywords.Add(((CTemplateConfigurePage*)GetParent())->GetKeywords()->ElementAt(i));

         if (caKeywords.ElementAt(i)->m_csName.Find(_T("%NavigationControlBar%")) == 0)
            ((CTemplateSelectKeyword*)caKeywords.ElementAt(i))->SetText(m_pKeyword->m_caOptions[0]);
      }
      ((CTemplateConfigurePage*)GetParent())->SetKeywords(caKeywords);
   }

   // Keyword %NavigationPluginContextMenu% does not work with RealPlayer =ne and higher
   if (csVarName.Find(_T("%NavigationPluginContextMenu%")) == 0)
   {
   }

   // Update the display of the keyword list
   ((CTemplateConfigurePage*)GetParent())->UpdateSubWindows();
}

void CTemplateVarSelect::CheckForRealPlayerWarning()
{
   CString csVarName;
   m_edVarName.GetWindowText(csVarName);
   int nSel = m_cbSelect.GetCurSel();
   CWnd *pWndIcon = GetDlgItem(IDC_TMP_VAR_WARNING_ICON);
   CWnd *pWndMessage = GetDlgItem(IDC_TMP_VAR_WARNING_RP8_LABEL);

   if ((csVarName.Find(_T("%NavigationPluginContextMenu%")) == 0) && (nSel != 0))
   {
      pWndIcon->ShowWindow(SW_SHOW);
      pWndMessage->ShowWindow(SW_SHOW);
   }
   else
   {
      pWndIcon->ShowWindow(SW_HIDE);
      pWndMessage->ShowWindow(SW_HIDE);
   }
}
