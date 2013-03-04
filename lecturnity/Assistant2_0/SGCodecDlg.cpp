// SGCodecDlg.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#endif
#include "SGCodecDlg.h"
#include "backend\la_project.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSGCodecDlg property page

IMPLEMENT_DYNCREATE(CSGCodecDlg, CPropertyPage)

CSGCodecDlg::CSGCodecDlg() : CPropertyPage(CSGCodecDlg::IDD)
{
	//{{AFX_DATA_INIT(CSGCodecDlg)
	//}}AFX_DATA_INIT
   
   m_pAssistantDoc = NULL;
}

CSGCodecDlg::~CSGCodecDlg()
{
}

void CSGCodecDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSGCodecDlg)
	DDX_Control(pDX, IDC_STATIC_SG_FRAMES, m_stFrames);
	DDX_Control(pDX, IDC_STATIC_SG_DISTANCE, m_stDistance);
	DDX_Control(pDX, IDC_STATIC_SG_ADAPTION_KB, m_stDatarateKb);
	DDX_Control(pDX, IDC_STATIC_SG_DATARATE, m_stDatarate);
	DDX_Control(pDX, IDC_SPIN_SG_QUALITY, m_spnQuality);
	DDX_Control(pDX, IDC_SLIDER_SG_QUALITY, m_sldQuality);
	DDX_Control(pDX, IDC_EDIT_SG_QUALITY, m_edtQuality);
	DDX_Control(pDX, IDC_EDIT_SG_KEYFRAME, m_edtKeyframe);
	DDX_Control(pDX, IDC_EDIT_SG_DATARATE, m_edtDatarate);
	DDX_Control(pDX, IDC_COMBO_SG_CODEC, m_cmbSgCodec);
	DDX_Control(pDX, IDC_CHECK_SG_KEYFRAME, m_chkKeyframe);
	DDX_Control(pDX, IDC_CHECK_SG_DATARATE, m_chkDatarate);
	DDX_Control(pDX, IDC_BUTTON_SG_CONFGURE, m_btnConfigure);
	DDX_Control(pDX, IDC_BUTTON_SG_CODEC_ABOUT, m_btnAbout);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSGCodecDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSGCodecDlg)
   ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SG_QUALITY, OnDeltaposSpinSgQuality)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SG_QUALITY, OnCustomdrawSliderSgQuality)
	ON_BN_CLICKED(IDC_CHECK_SG_KEYFRAME, OnCheckSgKeyframe)
	ON_BN_CLICKED(IDC_CHECK_SG_DATARATE, OnCheckSgDatarate)
	ON_BN_CLICKED(IDC_BUTTON_SG_CODEC_ABOUT, OnButtonSgCodecAbout)
	ON_BN_CLICKED(IDC_BUTTON_SG_CONFGURE, OnButtonSgConfgure)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_SG_CONFGURE, OnUpdateButtonCodecConfigure)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_SG_CODEC_ABOUT, OnUpdateButtonCodecAbout)
	ON_CBN_SELCHANGE(IDC_COMBO_SG_CODEC, OnSelchangeComboSgCodec)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSGCodecDlg message handlers

BOOL CSGCodecDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
   DWORD dwIds[] = {IDC_STATIC_GB_SG_CODEC_SETTINGS,
                    IDC_STATIC_GB_SG_CODEC_QUALITY,
                    IDC_STATIC_CODEC_SG_SETTINGS_CODEC,
                    IDC_BUTTON_SG_CODEC_ABOUT,
                    IDC_BUTTON_SG_CONFGURE,
                    IDC_CHECK_SG_KEYFRAME,          
                    IDC_STATIC_SG_DISTANCE,         
                    IDC_STATIC_SG_FRAMES,             
                    IDC_CHECK_SG_DATARATE,
                    IDC_STATIC_SG_DATARATE,
                    IDC_STATIC_SG_ADAPTION_KB,
                    -1};
   MfcUtils::Localize(this, dwIds);

   UINT nCodecIndex = 0;
   UINT nCodecQuality = 75;
   bool bUseKeyframes = true;
   UINT nKeyframeDistance = 30;
   bool bUseDatarate = false;
   UINT nDatarate = 500;
   if (m_pAssistantDoc)
      m_pAssistantDoc->GetCodecSettings(m_arCodecList, nCodecIndex, m_bHasAbout, m_bHasConfigure,
                                        m_bSupportsQuality, nCodecQuality, 
                                        m_bSupportsKeyframes, bUseKeyframes, nKeyframeDistance, 
                                        m_bSupportsDatarate, bUseDatarate, nDatarate);

   for (int i = 0; i < m_arCodecList.GetSize(); ++i)
      m_cmbSgCodec.AddString(m_arCodecList[i]);

   m_cmbSgCodec.SetCurSel(nCodecIndex);

	m_spnQuality.SetRange(0, 100);
   m_spnQuality.SetBuddy(&m_edtQuality);
   m_spnQuality.SetPos(nCodecQuality);
   
   m_sldQuality.SetRange(0, 100);
   m_sldQuality.SetTicFreq(5);
   m_sldQuality.SetPos(nCodecQuality);
   
   CString csWindow;
   m_chkKeyframe.SetCheck(bUseKeyframes);
   csWindow.Format(_T("%d"), nKeyframeDistance);
   m_edtKeyframe.SetWindowText(csWindow);
	m_stDistance.EnableWindow(m_chkKeyframe.GetCheck());
   m_edtKeyframe.EnableWindow(m_chkKeyframe.GetCheck());
   m_stFrames.EnableWindow(m_chkKeyframe.GetCheck());

   m_chkDatarate.SetCheck(bUseDatarate);
   csWindow.Format(_T("%d"), nDatarate);
   m_edtDatarate.SetWindowText(csWindow);
	m_stDatarate.EnableWindow(m_chkDatarate.GetCheck());
   m_edtDatarate.EnableWindow(m_chkDatarate.GetCheck());
   m_stDatarateKb.EnableWindow(m_chkDatarate.GetCheck());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSGCodecDlg::OnKickIdle()
{
   UpdateDialogControls(this, false);
}

void CSGCodecDlg::OnDeltaposSpinSgQuality(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int iPos = m_spnQuality.GetPos();
   m_sldQuality.SetPos(iPos);
	*pResult = 0;
}

void CSGCodecDlg::OnCustomdrawSliderSgQuality(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int iPos = m_sldQuality.GetPos();
   m_spnQuality.SetPos(iPos);
	*pResult = 0;
}

void CSGCodecDlg::OnCheckSgKeyframe() 
{
	// TODO: Add your control notification handler code here
	m_stDistance.EnableWindow(m_chkKeyframe.GetCheck());
   m_edtKeyframe.EnableWindow(m_chkKeyframe.GetCheck());
   m_stFrames.EnableWindow(m_chkKeyframe.GetCheck());
}

void CSGCodecDlg::OnCheckSgDatarate() 
{
	// TODO: Add your control notification handler code here
	m_stDatarate.EnableWindow(m_chkDatarate.GetCheck());
   m_edtDatarate.EnableWindow(m_chkDatarate.GetCheck());
   m_stDatarateKb.EnableWindow(m_chkDatarate.GetCheck());
}

void CSGCodecDlg::Init(CAssistantDoc *pAssistantDoc)
{
   m_pAssistantDoc = pAssistantDoc;
}

void CSGCodecDlg::OnOK() 
{
   CString csCodec;
   m_cmbSgCodec.GetWindowText(csCodec);
   
   UINT nCodecQuality = m_sldQuality.GetPos();

   CString csWindow;

   bool bUseKeyframes = m_chkKeyframe.GetCheck() == 0 ? false : true;
   m_edtKeyframe.GetWindowText(csWindow);
   UINT nKeyframeDistance = _ttoi(csWindow);
   
   bool bUseDatarate = m_chkDatarate.GetCheck() == 0 ? false : true;
   m_edtDatarate.GetWindowText(csWindow);
   UINT nDatarate = _ttoi(csWindow);

   if (m_pAssistantDoc)
      m_pAssistantDoc->SetCodecSettings(csCodec, nCodecQuality, 
                                        bUseKeyframes, nKeyframeDistance, 
                                        bUseDatarate, nDatarate);

	CPropertyPage::OnOK();
}

void CSGCodecDlg::OnButtonSgCodecAbout() 
{
   if (m_pAssistantDoc)
      m_pAssistantDoc->ShowSgCodecAbout();
}

void CSGCodecDlg::OnButtonSgConfgure() 
{
   if (m_pAssistantDoc)
      m_pAssistantDoc->ShowSgCodecConfigure();
}

void CSGCodecDlg::OnUpdateButtonCodecConfigure(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bHasConfigure);
}

void CSGCodecDlg::OnUpdateButtonCodecAbout(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bHasAbout);
}

void CSGCodecDlg::OnSelchangeComboSgCodec() 
{
   if (ASSISTANT::Project::active != NULL)
   {
      UINT nCodecIndex = m_cmbSgCodec.GetCurSel();
      CString csCodec = m_arCodecList[nCodecIndex];
      ASSISTANT::Project::active->SetSgCodec(csCodec, true);
      m_bHasAbout = ASSISTANT::Project::active->SgCodecHasAbout();
      m_bHasConfigure = ASSISTANT::Project::active->SgCodecCanBeConfigured();
   }
}
