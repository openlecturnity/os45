// CodecSettings.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "AssistantDoc.h"
#include "CodecSettings.h"
#include "backend\la_project.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCodecSettings property page

IMPLEMENT_DYNCREATE(CCodecSettings, CPropertyPage)

CCodecSettings::CCodecSettings() : CPropertyPage(CCodecSettings::IDD)
{
	//{{AFX_DATA_INIT(CCodecSettings)
	//}}AFX_DATA_INIT

   m_nCodecIndex = 0;
   m_nQuality = 75;
   m_bUseKeyframes = true;
   m_nKeyframeDistance = 15;
   m_bUseDatarateAdaption = false;
   m_nDatarate = 500;
   m_bHasAbout = false;
   m_bHasConfigure = false;
}

CCodecSettings::~CCodecSettings()
{
}

void CCodecSettings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCodecSettings)
	DDX_Control(pDX, IDC_COMBO_CODECS, m_cmbCodecs);
	DDX_Control(pDX, IDC_STATIC_KEYFRAME_DISTANCE, m_stKeyframeDistance);
	DDX_Control(pDX, IDC_STATIC_KEYFRAME_FRAMES, m_stKeyframeFrames);
	DDX_Control(pDX, IDC_STATIC_DATARATE_TARGET, m_stDatarateTarget);
	DDX_Control(pDX, IDC_STATIC_DATARATE_KB, m_stDatarateKb);
	DDX_Control(pDX, IDC_EDIT_DATARATE, m_edtDatarate);
	DDX_Control(pDX, IDC_EDIT_KEYFRAME, m_edtKeyrame);
	DDX_Control(pDX, IDC_CHECK_CODEC_KEYFRAME, m_chkKeyframe);
	DDX_Control(pDX, IDC_CHECK_CODEC_DATARATE, m_chkDatarate);
	DDX_Control(pDX, IDC_SPIN_QUALITY, m_spnQuality);
	DDX_Control(pDX, IDC_SLIDER_QUALITY, m_sldQuality);
	DDX_Control(pDX, IDC_EDIT_QUALITY, m_edtQuality);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCodecSettings, CPropertyPage)
	//{{AFX_MSG_MAP(CCodecSettings)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_QUALITY, OnDeltaposSpinQuality)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_QUALITY, OnCustomdrawSliderQuality)
	ON_BN_CLICKED(IDC_CHECK_CODEC_KEYFRAME, OnCheckCodecKeyframe)
	ON_BN_CLICKED(IDC_CHECK_CODEC_DATARATE, OnCheckCodecDatarate)
   ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
   ON_UPDATE_COMMAND_UI(IDC_STATIC_KEYFRAME_DISTANCE, OnUpdateLblDistance)
   ON_UPDATE_COMMAND_UI(IDC_EDIT_KEYFRAME, OnUpdateEdtDistance)
   ON_UPDATE_COMMAND_UI(IDC_CHECK_CODEC_KEYFRAME, OnUpdateChkFrames)
   ON_UPDATE_COMMAND_UI(IDC_STATIC_KEYFRAME_FRAMES, OnUpdateLblFrames)
   ON_UPDATE_COMMAND_UI(IDC_CHECK_CODEC_DATARATE, OnUpdateChkTargetDatarate)
   ON_UPDATE_COMMAND_UI(IDC_STATIC_DATARATE_TARGET, OnUpdateLblTargetDatarate)
   ON_UPDATE_COMMAND_UI(IDC_EDIT_DATARATE, OnUpdateEdtTargetDatarate)
   ON_UPDATE_COMMAND_UI(IDC_STATIC_DATARATE_KB, OnUpdateLblKBS)
	ON_EN_CHANGE(IDC_EDIT_QUALITY, OnChangeEditQuality)
	ON_EN_CHANGE(IDC_EDIT_KEYFRAME, OnChangeEditKeyframe)
	ON_EN_CHANGE(IDC_EDIT_DATARATE, OnChangeEditDatarate)
	ON_CBN_SELCHANGE(IDC_COMBO_CODECS, OnSelchangeComboCodecs)
	ON_BN_CLICKED(IDC_BUTTON_CODEC_ABOUT, OnButtonCodecAbout)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_CODEC_ABOUT, OnUpdateButtonCodecAbout)
	ON_BN_CLICKED(IDC_BUTTON_CODEC_CONFIGURE, OnButtonCodecConfigure)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_CODEC_CONFIGURE, OnUpdateButtonCodecConfigure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCodecSettings message handlers

BOOL CCodecSettings::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
   DWORD dwIds[] = {IDC_STATIC_CODEC_GB_SETTINGS,
                    IDC_STATIC_CODEG_GB_QUALITY,
                    IDC_STATIC_CODEC_SETTINGS_CODEC,
                    IDC_BUTTON_CODEC_ABOUT,
                    IDC_BUTTON_CODEC_CONFIGURE,
                    IDC_CHECK_CODEC_KEYFRAME,          
                    IDC_STATIC_KEYFRAME_DISTANCE,         
                    IDC_STATIC_KEYFRAME_FRAMES,             
                    IDC_CHECK_CODEC_DATARATE,                          
                    IDC_STATIC_DATARATE_TARGET,         
                    IDC_STATIC_DATARATE_KB,         
                    -1};
   MfcUtils::Localize(this, dwIds);

	m_spnQuality.SetRange(0, 100);
   m_spnQuality.SetBuddy(&m_edtQuality);
   
   m_sldQuality.SetRange(0, 100);
   m_sldQuality.SetTicFreq(5);
   
   UpdatePage();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCodecSettings::Init(CAssistantDoc *pAssistantDoc)
{
   m_pAssistantDoc = pAssistantDoc;
}

void CCodecSettings::OnDeltaposSpinQuality(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int iPos = m_spnQuality.GetPos();
   m_sldQuality.SetPos(iPos);

   if (m_pAssistantDoc)
      m_pAssistantDoc->SetCodecQuality(iPos);

	*pResult = 0;
}

void CCodecSettings::OnCustomdrawSliderQuality(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int iPos = m_sldQuality.GetPos();
   m_spnQuality.SetPos(iPos);

   if (m_pAssistantDoc)
      m_pAssistantDoc->SetCodecQuality(iPos);

	*pResult = 0;
}

void CCodecSettings::OnCheckCodecKeyframe() 
{
   m_bUseKeyframes = m_chkKeyframe.GetCheck() == 0 ? false : true;
   if (m_pAssistantDoc)
      m_pAssistantDoc->SetUseKeyframes(m_bUseKeyframes);
}

void CCodecSettings::OnCheckCodecDatarate() 
{
   m_bUseDatarateAdaption = m_chkDatarate.GetCheck() == 0 ? false : true;
   if (m_pAssistantDoc)
      m_pAssistantDoc->SetUseDatarate(m_bUseDatarateAdaption);
}

void CCodecSettings::OnKickIdle()
{
   UpdateDialogControls(this, false);
}

void CCodecSettings::OnUpdateChkFrames(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (m_pAssistantDoc)
      bEnable = m_pAssistantDoc->KeyframesPossible();

   if (!bEnable && m_chkKeyframe.GetCheck() == 1)
      m_chkKeyframe.SetCheck(0);

   pCmdUI->Enable(bEnable);
}

void CCodecSettings::OnUpdateLblDistance(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkKeyframe.GetCheck());
}

void CCodecSettings::OnUpdateEdtDistance(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkKeyframe.GetCheck());
}

void CCodecSettings::OnUpdateLblFrames(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkKeyframe.GetCheck());
}

void CCodecSettings::OnUpdateChkTargetDatarate(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (m_pAssistantDoc)
      bEnable = m_pAssistantDoc->DataratePossible();

   if (!bEnable && m_chkDatarate.GetCheck() == 1)
      m_chkDatarate.SetCheck(0);

   pCmdUI->Enable(bEnable);
}

void CCodecSettings::OnUpdateLblTargetDatarate(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkDatarate.GetCheck());
}

void CCodecSettings::OnUpdateEdtTargetDatarate(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkDatarate.GetCheck());
}

void CCodecSettings::OnUpdateLblKBS(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_chkDatarate.GetCheck());
}

void CCodecSettings::OnChangeEditQuality() 
{
   if (m_pAssistantDoc)
   {
      CString csText;
      m_edtQuality.GetWindowText(csText);
      m_nQuality = _ttoi(csText);
      m_pAssistantDoc->SetCodecQuality(m_nQuality);
   }
}

void CCodecSettings::OnChangeEditKeyframe() 
{
   if (m_pAssistantDoc)
   {
      CString csText;
      m_edtKeyrame.GetWindowText(csText);
      m_nKeyframeDistance = _ttoi(csText);
      m_pAssistantDoc->SetKeyframes(m_nKeyframeDistance);
   }
}

void CCodecSettings::OnChangeEditDatarate() 
{
   if (m_pAssistantDoc)
   {
      CString csText;
      m_edtDatarate.GetWindowText(csText);
      m_nDatarate = _ttoi(csText);
      m_pAssistantDoc->SetDatarate(m_nDatarate);
   }
}

void CCodecSettings::OnSelchangeComboCodecs() 
{
   m_nCodecIndex = m_cmbCodecs.GetCurSel();

   if (m_pAssistantDoc)
   {
      m_pAssistantDoc->SetCodec(m_arCodecs[m_nCodecIndex], true);
      if (ASSISTANT::Project::active)
      {
         m_bHasAbout = ASSISTANT::Project::active->CodecHasAbout();
         m_bHasConfigure = ASSISTANT::Project::active->CodecCanBeConfigured();
      }
   }
}


// private functions
void CCodecSettings::UpdatePage()
{
   m_nCodecIndex = 0;
   m_arCodecs.RemoveAll();
   m_nQuality = 75;
   m_bUseKeyframes = true;
   m_nKeyframeDistance = 15;
   m_bUseDatarateAdaption = false;
   m_nDatarate = 500;

   if (m_pAssistantDoc)
   {
      m_pAssistantDoc->GetCodecSettings(m_arCodecs, m_nCodecIndex, m_nQuality, 
                                        m_bUseKeyframes, m_nKeyframeDistance,
                                        m_bUseDatarateAdaption, m_nDatarate,
                                        m_bHasAbout, m_bHasConfigure);
   }


   m_cmbCodecs.ResetContent();
   for (int i = 0; i < m_arCodecs.GetSize(); ++i)
      m_cmbCodecs.AddString(m_arCodecs[i]);
   m_cmbCodecs.SetCurSel(m_nCodecIndex);

   m_spnQuality.SetPos(m_nQuality);
   m_sldQuality.SetPos(m_nQuality);

   CString csText;
   m_chkKeyframe.SetCheck(m_bUseKeyframes);
   csText.Format(_T("%d"), m_nKeyframeDistance);
   m_edtKeyrame.SetWindowText(csText);

   m_chkDatarate.SetCheck(m_bUseDatarateAdaption);
   csText.Format(_T("%d"), m_nDatarate);
   m_edtDatarate.SetWindowText(csText);
}


void CCodecSettings::OnButtonCodecAbout() 
{
   if (ASSISTANT::Project::active)
      ASSISTANT::Project::active->ShowCodecAbout();
}

void CCodecSettings::OnUpdateButtonCodecAbout(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bHasAbout);
}

void CCodecSettings::OnButtonCodecConfigure() 
{
   if (ASSISTANT::Project::active)
      ASSISTANT::Project::active->ShowCodecConfigure();
}

void CCodecSettings::OnUpdateButtonCodecConfigure(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bHasConfigure);
}
