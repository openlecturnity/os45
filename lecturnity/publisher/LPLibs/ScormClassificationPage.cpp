// ScormClassificationPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "LPLibs.h"
#include "ScormClassificationPage.h"
#include "ScormEngine.h"
#include "MfcUtils.h"

extern CScormEngine g_scormEngine;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CScormClassificationPage 

IMPLEMENT_DYNCREATE(CScormClassificationPage, CPropertyPage)

CScormClassificationPage::CScormClassificationPage() : CPropertyPage(CScormClassificationPage::IDD)
{
	//{{AFX_DATA_INIT(CScormClassificationPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   if (FALSE != m_csCaption.LoadString(IDD))
   {
      m_psp.pszTitle = m_csCaption;
      m_psp.dwFlags |= PSP_USETITLE;
   }
}

CScormClassificationPage::~CScormClassificationPage()
{
}


void CScormClassificationPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScormClassificationPage)
	DDX_Control(pDX, IDC_CLASSIFICATION_KEYWORD, m_edClsfKeyword);
	DDX_Control(pDX, IDC_CLASSIFICATION_DESCRIPTION, m_edClsfDescription);
	DDX_Control(pDX, IDC_SCORM_PURPOSE, m_cbbClsfPurpose);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScormClassificationPage, CPropertyPage)
	//{{AFX_MSG_MAP(CScormClassificationPage)
	ON_EN_CHANGE(IDC_CLASSIFICATION_DESCRIPTION, OnChangeAnything)
	ON_EN_CHANGE(IDC_CLASSIFICATION_KEYWORD, OnChangeAnything)
	ON_CBN_SELCHANGE(IDC_SCORM_PURPOSE, OnSelectChangeScormPurpose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CScormClassificationPage 

BOOL CScormClassificationPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   // Copy the original scorm settings structure to a temporary structure
   // so that we have/know the settings from the 
   // registry in for example RestoreValuesTemporarily()
   g_scormEngine.m_scormSettings.CopyClassificationDataTo(m_scormSettingsTemp);

   // set the "purpose" index
   m_nActualPurposeIndex = m_scormSettingsTemp.m_nPurposeIndex;
   m_cbbClsfPurpose.SetCurSel(m_nActualPurposeIndex);

   // OnSelectChangeScormPurpose() will not get called with SetCurSel() above
   // so we need to restore the values for this index here:
   RestoreValuesTemporarily(m_nActualPurposeIndex);


   DWORD dwIds[] = {IDC_SCORM_CLASSIFICATION_GROUP,
                    IDC_SCORM_LB_PURPOSE,
                    IDC_SCORM_DESC_SETTINGS_2000CHARS,
                    IDC_SCORM_LB_CLASSIFICATION_DESCRIPTION,
                    IDC_SCORM_LB_CLASSIFICATION_KEYWORD,
                    -1};
   MfcUtils::Localize(this, dwIds);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL CScormClassificationPage::OnApply() 
{
   // store the currently selected index
   m_scormSettingsTemp.m_nPurposeIndex = m_nActualPurposeIndex;

   // Copy the temporary scorm settings structure back to the original structure.
   // Overwrite only the settings changed in this dialog not the others which
   // might have been changed elsewhere.
   m_scormSettingsTemp.CopyClassificationDataTo(g_scormEngine.m_scormSettings);
   
   g_scormEngine.WriteSettings();
	
	return CPropertyPage::OnApply();
}

void CScormClassificationPage::OnChangeAnything() 
{
   // make sure recent changes are reflected in the temporary data structure
   StoreValuesTemporarily(m_nActualPurposeIndex);	

   CPropertyPage::SetModified();
}

void CScormClassificationPage::OnSelectChangeScormPurpose() 
{
   // Store the values of the previous selected purpose
   StoreValuesTemporarily(m_nActualPurposeIndex);

   // The actual purpose index has changed
   m_nActualPurposeIndex = m_cbbClsfPurpose.GetCurSel();

   // Restore the values of the new purpose index
   RestoreValuesTemporarily(m_nActualPurposeIndex);
}

/*private*/ void CScormClassificationPage::StoreValuesTemporarily(int nActualIndex)
{
   CString csClsfDescription;
   m_edClsfDescription.GetWindowText(csClsfDescription);
   CString csClsfKeyword;
   m_edClsfKeyword.GetWindowText(csClsfKeyword);
   _tcsncpy(m_scormSettingsTemp.m_atszClsfDescription[nActualIndex], csClsfDescription, SCORM_CLSF_DESCRIPTION_SIZE-1);
   _tcsncpy(m_scormSettingsTemp.m_atszClsfKeyword[nActualIndex], csClsfKeyword, SCORM_CLSF_KEYWORD_SIZE-1);

}

/*private*/ void CScormClassificationPage::RestoreValuesTemporarily(int nActualIndex)
{
   CString csClsfDescription = m_scormSettingsTemp.m_atszClsfDescription[nActualIndex];
   CString csClsfKeyword = m_scormSettingsTemp.m_atszClsfKeyword[nActualIndex];
   m_edClsfDescription.SetWindowText(csClsfDescription);
   m_edClsfKeyword.SetWindowText(csClsfKeyword);
}
