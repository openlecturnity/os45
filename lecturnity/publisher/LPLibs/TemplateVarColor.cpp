// TemplateVarColor.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "TemplateVarColor.h"
#include "TemplateConfigurePage.h"

#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateVarColor 


CTemplateVarColor::CTemplateVarColor(CTemplateKeyword *pKeyword)
	: CDialog(CTemplateVarColor::IDD, NULL /* parent */)
{
	//{{AFX_DATA_INIT(CTemplateVarColor)
	//}}AFX_DATA_INIT

   m_pKeyword = (CTemplateColorKeyword *) pKeyword;
}


void CTemplateVarColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateVarColor)
	DDX_Control(pDX, IDC_TMP_VAR_COLOR_VARNAME, m_edVarName);
	DDX_Control(pDX, IDC_TEMPLATE_VAR_COLOR, m_clrStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateVarColor, CDialog)
	//{{AFX_MSG_MAP(CTemplateVarColor)
   ON_MESSAGE(WM_COLOR_CHANGED, OnColorChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTemplateVarColor 

BOOL CTemplateVarColor::OnInitDialog() 
{
	CDialog::OnInitDialog();

   DWORD adwIds[] =
   {
      IDC_TMP_VAR_VARIABLE_LABEL,
      IDC_TMP_VAR_TYPE_LABEL,
      IDC_TMP_VAR_COLOR_TYPE,
      IDC_TMP_VAR_CONTENT_LABEL,
      -1
   };
   MfcUtils::Localize(this, adwIds);

   m_edVarName.SetWindowText(m_pKeyword->m_csName);
   m_clrStatic.SetColorRef(m_pKeyword->GetColor());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

//void CTemplateVarColor::OnColorChanged(WPARAM wParam, LPARAM lParam)
void CTemplateVarColor::OnColorChanged()
{
   m_pKeyword->SetColor(m_clrStatic.GetColorRef());
   ((CTemplateConfigurePage*)GetParent())->UpdateSubWindows();
   g_templateEngine.SetTemplateChanged();
   ((CPropertyPage *) GetParent())->SetModified();
}

LRESULT CTemplateVarColor::OnColorChanged(WPARAM wParam, LPARAM lParam)
{
   OnColorChanged();
   return 0;
}
