// TemplateVarText.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "TemplateVarText.h"
#include "TemplateConfigurePage.h"

#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateVarText 


CTemplateVarText::CTemplateVarText(CTemplateKeyword *pKeyword)
	: CDialog(CTemplateVarText::IDD, NULL /* pParent */)
{
	//{{AFX_DATA_INIT(CTemplateVarText)
	//}}AFX_DATA_INIT

   m_pKeyword = (CTemplateTextKeyword *) pKeyword;
}


void CTemplateVarText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateVarText)
	DDX_Control(pDX, IDC_TEMPLATE_VAR_TEXT, m_edText);
	DDX_Control(pDX, IDC_TMP_VAR_VARNAME, m_edVarName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateVarText, CDialog)
	//{{AFX_MSG_MAP(CTemplateVarText)
	ON_EN_KILLFOCUS(IDC_TEMPLATE_VAR_TEXT, OnLeaveTextField)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTemplateVarText 

BOOL CTemplateVarText::OnInitDialog() 
{
	CDialog::OnInitDialog();

   DWORD adwIds[] =
   {
      IDC_TMP_VAR_VARIABLE_LABEL,
      IDC_TMP_VAR_TYPE_LABEL,
      IDC_TMP_VAR_CONTENT_LABEL,
      IDC_TMP_VAR_TEXT_TYPE,
      IDC_TMP_VAR_RESETBUTTON,
      -1
   };
   MfcUtils::Localize(this, adwIds);
	
   m_edVarName.SetWindowText(m_pKeyword->m_csName);
   CString csText;
   RETRIEVE_CSTRING(m_pKeyword->GetText, csText);
   m_edText.SetWindowText(csText);

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CTemplateVarText::OnLeaveTextField() 
{
   CString csText;
   m_edText.GetWindowText(csText);
   m_pKeyword->SetText(csText);
   ((CTemplateConfigurePage*)GetParent())->UpdateSubWindows();
   g_templateEngine.SetTemplateChanged();
   ((CPropertyPage *) GetParent())->SetModified();
}
