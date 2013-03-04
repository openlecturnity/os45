// TemplateVarGeneral.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "TemplateVarGeneral.h"
#include "TemplateConfigurePage.h"

#include "MfcUtils.h" // Localize

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateVarGeneral 


CTemplateVarGeneral::CTemplateVarGeneral(CTemplateKeyword *pKeyword)
	: CDialog(CTemplateVarGeneral::IDD, NULL /* parent */)
{
	//{{AFX_DATA_INIT(CTemplateVarGeneral)
	//}}AFX_DATA_INIT

   m_pKeyword = (CTemplateGeneralKeyword *) pKeyword;
}


void CTemplateVarGeneral::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateVarGeneral)
	DDX_Control(pDX, IDC_TEMPLATE_VAR_HTML_TEXT, m_edGeneral);
	DDX_Control(pDX, IDC_TMP_VAR_VARNAME, m_edVarName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateVarGeneral, CDialog)
	//{{AFX_MSG_MAP(CTemplateVarGeneral)
	ON_EN_KILLFOCUS(IDC_TEMPLATE_VAR_HTML_TEXT, OnLeaveTextField)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTemplateVarGeneral 

BOOL CTemplateVarGeneral::OnInitDialog() 
{
	CDialog::OnInitDialog();

   DWORD adwIds[] =
   {
      IDC_TMP_VAR_VARIABLE_LABEL,
      IDC_TMP_VAR_TYPE_LABEL,
      IDC_TMP_VAR_GENERAL_TYPE,
      IDC_TMP_VAR_CONTENT_LABEL,
      IDC_TMP_VAR_RESETBUTTON,
      -1
   };
   MfcUtils::Localize(this, adwIds);

   m_edVarName.SetWindowText(m_pKeyword->m_csName);
   CString csGeneral(_T(""));
   RETRIEVE_CSTRING(m_pKeyword->GetGeneral, csGeneral);
   m_edGeneral.SetWindowText(csGeneral);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CTemplateVarGeneral::OnLeaveTextField() 
{
   CString csGeneral;
   m_edGeneral.GetWindowText(csGeneral);
   m_pKeyword->SetGeneral(csGeneral);
   ((CTemplateConfigurePage*)GetParent())->UpdateSubWindows();
   g_templateEngine.SetTemplateChanged();
   ((CPropertyPage *) GetParent())->SetModified();
}
