// DenverCodecPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "DenverCodecPage.h"
#include "editorDoc.h"
#include "MfcUtils.h" // MfcUtils::Localize

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CDenverCodecPage 

IMPLEMENT_DYNCREATE(CDenverCodecPage, CVideoProps)

CDenverCodecPage::CDenverCodecPage(DWORD dwId, CAviSettings *pAviSettings) : 
   CVideoProps(CDenverCodecPage::IDD, pAviSettings)
{
	//{{AFX_DATA_INIT(CDenverCodecPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_csCaption.LoadString(IDS_CLIP2_CAPTION);
   m_psp.pszTitle = m_csCaption;
   m_psp.dwFlags |= PSP_USETITLE;
}

CDenverCodecPage::~CDenverCodecPage()
{
}

void CDenverCodecPage::DoDataExchange(CDataExchange* pDX)
{
	CVideoProps::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDenverCodecPage)
	DDX_Control(pDX, IDC_STRETCH_SMALLCLIPS, m_cbbStretchSmallClips);
	DDX_Control(pDX, IDC_AUTO_VIDEOSIZE, m_cbbAutoVideoSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDenverCodecPage, CVideoProps)
	//{{AFX_MSG_MAP(CDenverCodecPage)
	ON_BN_CLICKED(IDC_AUTO_VIDEOSIZE, OnAutoVideosizeClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDenverCodecPage 

BOOL CDenverCodecPage::OnInitDialog() 
{
	CVideoProps::OnInitDialog();

   DWORD dwIds[] = {IDC_AUTO_VIDEOSIZE,
                    IDC_STRETCH_SMALLCLIPS,
                    -1};
   MfcUtils::Localize(this, dwIds);

   m_cbbAutoVideoSize.SetCheck(m_pDoc->project.m_clipExportAutoCalcDenverSize ? BST_CHECKED : BST_UNCHECKED);
   m_cbbStretchSmallClips.SetCheck(m_pDoc->project.m_clipExportStretchDenverClips ? BST_CHECKED : BST_UNCHECKED);
   EnableVideoSizeGui(!m_pDoc->project.m_clipExportAutoCalcDenverSize);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CDenverCodecPage::EnableVideoSizeGui(bool bEnable)
{
   BOOL b = bEnable ? TRUE : FALSE;
   m_sizeXEdit.EnableWindow(b);
   m_sizeYEdit.EnableWindow(b);
   m_keepAspectRadio.EnableWindow(b);
   m_frameRateEdit.EnableWindow(b);
}

void CDenverCodecPage::OnAutoVideosizeClick() 
{
	if (m_cbbAutoVideoSize.GetCheck() == BST_CHECKED)
   {
      EnableVideoSizeGui(false);
      m_pDoc->project.m_clipExportAutoCalcDenverSize = true;
      m_pDoc->project.RecalcDenverVideoParams();

      ReadDefaultSettings();
      PopulateCodecSettings();
   }
   else
   {
      EnableVideoSizeGui(true);
   }
}

void CDenverCodecPage::OnOK()
{
   CVideoProps::OnOK();

   m_pDoc->project.m_clipExportAutoCalcDenverSize = 
      m_cbbAutoVideoSize.GetCheck() == BST_CHECKED ? true : false;
   m_pDoc->project.m_clipExportStretchDenverClips =
      m_cbbStretchSmallClips.GetCheck() == BST_CHECKED ? true : false;
}
