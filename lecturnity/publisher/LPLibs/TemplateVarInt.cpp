// TemplateVarInt.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "lplibs.h"
#include "TemplateVarInt.h"
#include "TemplateConfigurePage.h"

#include "MfcUtils.h" // Localize

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTemplateVarInt 


CTemplateVarInt::CTemplateVarInt(CTemplateKeyword *pKeyword)
	: CDialog(CTemplateVarInt::IDD, NULL /* pParent */)
{
	//{{AFX_DATA_INIT(CTemplateVarInt)
	//}}AFX_DATA_INIT

   m_pKeyword = (CTemplateIntegerKeyword *) pKeyword;
}


void CTemplateVarInt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateVarInt)
	DDX_Control(pDX, IDC_TMP_VAR_INT_SPIN, m_sbcValue);
	DDX_Control(pDX, IDC_TMP_VAR_INT_VALUE, m_edValue);
	DDX_Control(pDX, IDC_TMP_VAR_VARNAME, m_edVarName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateVarInt, CDialog)
	//{{AFX_MSG_MAP(CTemplateVarInt)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TMP_VAR_INT_SPIN, OnSpinButtonClick)
	ON_EN_KILLFOCUS(IDC_TMP_VAR_INT_VALUE, OnLeaveValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTemplateVarInt 

BOOL CTemplateVarInt::OnInitDialog() 
{
	CDialog::OnInitDialog();

   DWORD adwIds[] =
   {
      IDC_TMP_VAR_VARIABLE_LABEL,
      IDC_TMP_VAR_TYPE_LABEL,
      IDC_TMP_VAR_CONTENT_LABEL,
      IDC_TMP_VAR_INT_TYPE,
      -1,
   };
   MfcUtils::Localize(this, adwIds);
	
   m_edVarName.SetWindowText(m_pKeyword->m_csName);
   SetValue(m_pKeyword->GetValue(), false);

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CTemplateVarInt::OnSpinButtonClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

   if (pNMUpDown->iDelta < 0)
      SetValue(GetValue() + 1);
   else
      SetValue(GetValue() - 1);
	
	*pResult = 0;
}

int CTemplateVarInt::GetValue()
{
   CString csValue;
   m_edValue.GetWindowText(csValue);
   return _ttoi(csValue);
}

void CTemplateVarInt::SetValue(int value, bool bIsChange)
{
   if (value < m_pKeyword->m_nMinValue)
      value = m_pKeyword->m_nMinValue;
   else if (value > m_pKeyword->m_nMaxValue)
      value = m_pKeyword->m_nMaxValue;

   CString csValue;
   csValue.Format(_T("%d"), value);
   m_edValue.SetWindowText(csValue);

   if (bIsChange)
   {
      m_pKeyword->SetValue(value);
      ((CTemplateConfigurePage*)GetParent())->UpdateSubWindows();
      g_templateEngine.SetTemplateChanged();
      ((CPropertyPage *) GetParent())->SetModified();
   }
}

void CTemplateVarInt::OnLeaveValue() 
{
   SetValue(GetValue());	
}
