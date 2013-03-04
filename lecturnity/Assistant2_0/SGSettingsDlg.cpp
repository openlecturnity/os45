// SGSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#endif
#include "SGSettingsDlg.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSGSettingsDlg property page

IMPLEMENT_DYNCREATE(CSGSettingsDlg, CPropertyPage)

CSGSettingsDlg::CSGSettingsDlg() : CPropertyPage(CSGSettingsDlg::IDD)
{
	//{{AFX_DATA_INIT(CSGSettingsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
   
   m_pAssistantDoc = NULL;
}

CSGSettingsDlg::~CSGSettingsDlg()
{
}

void CSGSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSGSettingsDlg)
	DDX_Control(pDX, IDC_STATIC_FIXED_WIDTH, m_stFixedWidth);
	DDX_Control(pDX, IDC_STATIC_FR_ADAPTIVE_WIDTH, m_stAdaptiveWidth);
	DDX_Control(pDX, IDC_STATIC_FR_ADAPTIVE_HEIGHT, m_stAdaptiveHeight);
	DDX_Control(pDX, IDC_STATIC_FIXED_REG_Y, m_stFixedY);
	DDX_Control(pDX, IDC_STATIC_FIXED_REG_X, m_stFixedX);
	DDX_Control(pDX, IDC_STATIC_FIXED_REG_HEIGHT, m_stFixedHeight);
	DDX_Control(pDX, IDC_SPIN_FR_STATIC, m_spnFrStatic);
	DDX_Control(pDX, IDC_SPIN_FR_ADAPTIVE, m_spnFrAdaptive);
	DDX_Control(pDX, IDC_EDIT_FR_STATIC, m_edtFrStatic);
	DDX_Control(pDX, IDC_EDIT_FR_ADAPTIVE_WIDTH, m_edtFrAdaptiveWidth);
	DDX_Control(pDX, IDC_EDIT_FR_ADAPTIVE_HEIGHT, m_edtFrAdaptiveHeight);
	DDX_Control(pDX, IDC_EDIT_FR_ADAPTIVE, m_edtFrAdaptive);
	DDX_Control(pDX, IDC_EDIT_FIXED_Y, m_edtFixedY);
	DDX_Control(pDX, IDC_EDIT_FIXED_X, m_edtFixedX);
	DDX_Control(pDX, IDC_EDIT_FIXED_WIDTH, m_edtFixedWidth);
	DDX_Control(pDX, IDC_EDIT_FIXED_HEIGHT, m_edtFixedHeight);
	DDX_Control(pDX, IDC_CHECK_MOUSE_VISUAL, m_chkMouseVisual);
	DDX_Control(pDX, IDC_CHECK_MOUSE_ACOUSTIC, m_chkMouseAcoustic);
	DDX_Control(pDX, IDC_CHECK_FIXED_REGION, m_chkFixedRegion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSGSettingsDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSGSettingsDlg)
	ON_BN_CLICKED(IDC_RADIO_FRAMERATE_STATIC, OnRadioFramerateStatic)
	ON_BN_CLICKED(IDC_RADIO_FRAMERATE_ADAPTIVE, OnRadioFramerateAdaptive)
	ON_BN_CLICKED(IDC_CHECK_FIXED_REGION, OnCheckFixedRegion)
	ON_EN_UPDATE(IDC_EDIT_FR_ADAPTIVE, OnUpdateEditFrAdaptive)
	ON_EN_UPDATE(IDC_EDIT_FR_STATIC, OnUpdateEditFrStatic)
	ON_EN_UPDATE(IDC_EDIT_FIXED_HEIGHT, OnUpdateCheckFixedRegion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSGSettingsDlg message handlers

BOOL CSGSettingsDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
   DWORD dwIds[] = {IDC_STATIC_SG_GB_SETTINGS_MOUSE,
                    IDC_STATIC_SG_GB_SETTINGS_FRAMERATE,
                    IDC_STATIC_SG_GB_REGION,
                    IDC_CHECK_MOUSE_VISUAL,
                    IDC_CHECK_MOUSE_ACOUSTIC,
                    IDC_RADIO_FRAMERATE_STATIC,          
                    IDC_RADIO_FRAMERATE_ADAPTIVE,         
                    IDC_CHECK_FIXED_REGION,             
                    IDC_STATIC_FR_ADAPTIVE_WIDTH,                          
                    IDC_STATIC_FR_ADAPTIVE_HEIGHT,         
                    IDC_STATIC_FIXED_REG_X,
                    IDC_STATIC_FIXED_REG_Y,
                    IDC_STATIC_FIXED_REG_HEIGHT,
                    IDC_STATIC_FIXED_WIDTH,
                    -1};
   MfcUtils::Localize(this, dwIds);

   bool bMouseVisual = true;
   bool bMouseAcoustic = true;
   if (m_pAssistantDoc)
      m_pAssistantDoc->GetMouseEffects(bMouseVisual, bMouseAcoustic);
   m_chkMouseVisual.SetCheck(bMouseVisual);
   m_chkMouseAcoustic.SetCheck(bMouseAcoustic);

   bool bStaticFramerate = true;
   float fStaticFramerate = 10;
   float fAdaptiveFramerate = 10;
   UINT nWidth = 320;
   UINT nHeight = 240;
   if (m_pAssistantDoc)
      m_pAssistantDoc->GetFramerateSettings(bStaticFramerate, fStaticFramerate, 
                                            fAdaptiveFramerate, nWidth, nHeight);
      
   if (bStaticFramerate)
   {
      CButton* radioStatic = (CButton*)GetDlgItem(IDC_RADIO_FRAMERATE_STATIC);
      radioStatic->SetCheck(bStaticFramerate);
   }
   else
   {
      CButton* radioAdaptive = (CButton*)GetDlgItem(IDC_RADIO_FRAMERATE_ADAPTIVE);
      radioAdaptive->SetCheck(!bStaticFramerate);
   }

	m_spnFrStatic.SetRange(0, 30);
   m_spnFrStatic.SetBuddy(&m_edtFrStatic);
   m_spnFrStatic.SetPos((int)fStaticFramerate);

   m_spnFrAdaptive.SetRange(0, 30);
   m_spnFrAdaptive.SetBuddy(&m_edtFrAdaptive);
   m_spnFrAdaptive.SetPos((int)fAdaptiveFramerate);

   m_edtFrStatic.LimitText(2);
   m_edtFrAdaptive.LimitText(2);

   CString csWindow;
   csWindow.Format(_T("%d"), nWidth);
	m_edtFrAdaptiveWidth.SetWindowText(csWindow);
   csWindow.Format(_T("%d"), nHeight);
	m_edtFrAdaptiveHeight.SetWindowText(csWindow);

   bool bRegionFixed = false;
   UINT nFixedX = 0;
   UINT nFixedY = 0;
   UINT nFixedWidth = 320;
   UINT nFixedHeight = 240;
   if (m_pAssistantDoc)
      m_pAssistantDoc->GetRegionSettings(bRegionFixed, nFixedX, nFixedY, nFixedWidth, nFixedHeight);
   m_chkFixedRegion.SetCheck(bRegionFixed);

   csWindow.Format(_T("%d"), nFixedX);
   m_edtFixedX.SetWindowText(csWindow);
   csWindow.Format(_T("%d"), nFixedY);
   m_edtFixedY.SetWindowText(csWindow);
   csWindow.Format(_T("%d"), nFixedWidth);
   m_edtFixedWidth.SetWindowText(csWindow);
   csWindow.Format(_T("%d"), nFixedHeight);
   m_edtFixedHeight.SetWindowText(csWindow);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSGSettingsDlg::OnRadioFramerateStatic() 
{
	// TODO: Add your control notification handler code here
	m_edtFrAdaptive.EnableWindow(false);
   m_stAdaptiveWidth.EnableWindow(false);
   m_edtFrAdaptiveWidth.EnableWindow(false);
   m_stAdaptiveHeight.EnableWindow(false);
   m_edtFrAdaptiveHeight.EnableWindow(false);
   m_spnFrAdaptive.EnableWindow(false);

   m_spnFrStatic.EnableWindow(true);
   m_edtFrStatic.EnableWindow(true);
}

void CSGSettingsDlg::OnRadioFramerateAdaptive() 
{
	// TODO: Add your control notification handler code here
   m_spnFrStatic.EnableWindow(false);
	m_edtFrStatic.EnableWindow(false);

   m_edtFrAdaptive.EnableWindow(true);
   m_stAdaptiveWidth.EnableWindow(true);
   m_edtFrAdaptiveWidth.EnableWindow(true);
   m_stAdaptiveHeight.EnableWindow(true);
   m_edtFrAdaptiveHeight.EnableWindow(true);
   m_spnFrAdaptive.EnableWindow(true);
}

void CSGSettingsDlg::OnCheckFixedRegion() 
{
	// TODO: Add your control notification handler code here
   m_stFixedX.EnableWindow(m_chkFixedRegion.GetCheck());
   m_edtFixedX.EnableWindow(m_chkFixedRegion.GetCheck());
   m_stFixedY.EnableWindow(m_chkFixedRegion.GetCheck());
   m_edtFixedY.EnableWindow(m_chkFixedRegion.GetCheck());
   m_stFixedWidth.EnableWindow(m_chkFixedRegion.GetCheck());
   m_edtFixedWidth.EnableWindow(m_chkFixedRegion.GetCheck());
   m_stFixedHeight.EnableWindow(m_chkFixedRegion.GetCheck());
   m_edtFixedHeight.EnableWindow(m_chkFixedRegion.GetCheck());
}

void CSGSettingsDlg::OnOK() 
{
   CString csWindow;

   int nDefaultStaticFramerate = 5;
   int nDefaultAdaptiveFramerate = 10;

   bool bMouseVisual =  m_chkMouseVisual.GetCheck() == 0 ? false: true;
   bool bMouseAcoustic = m_chkMouseAcoustic.GetCheck() == 0 ? false: true;
   if (m_pAssistantDoc)
      m_pAssistantDoc->SetMouseEffects(bMouseVisual, bMouseAcoustic);


   CButton* radioStatic = (CButton*)GetDlgItem(IDC_RADIO_FRAMERATE_STATIC);
   bool bStaticFramerate = radioStatic->GetCheck() == 0 ? false: true;

   m_edtFrStatic.GetWindowText(csWindow);
   if(csWindow == _T(""))
   {
      m_spnFrStatic.SetPos(nDefaultStaticFramerate);
   }

   m_edtFrAdaptive.GetWindowText(csWindow);
   if(csWindow == _T(""))
   {
      m_spnFrAdaptive.SetPos(nDefaultAdaptiveFramerate);
   }

   float fStaticFramerate = (float)m_spnFrStatic.GetPos();
   float fAdaptiveFramerate = (float)m_spnFrAdaptive.GetPos();
   m_edtFrAdaptiveWidth.GetWindowText(csWindow);
   UINT nWidth = _ttoi(csWindow);
   m_edtFrAdaptiveHeight.GetWindowText(csWindow);
   UINT nHeight = _ttoi(csWindow);
   if (m_pAssistantDoc)
      m_pAssistantDoc->SetFramerateSettings(bStaticFramerate, fStaticFramerate, fAdaptiveFramerate,
                                            nWidth, nHeight);
      
   bool bRegionFixed = m_chkFixedRegion.GetCheck() == 0 ? false: true;

   m_edtFixedX.GetWindowText(csWindow);
   UINT nFixedX = _ttoi(csWindow);
   m_edtFixedY.GetWindowText(csWindow);
   UINT nFixedY = _ttoi(csWindow);
   m_edtFixedWidth.GetWindowText(csWindow);
   UINT nFixedWidth = _ttoi(csWindow);
   m_edtFixedHeight.GetWindowText(csWindow);
   UINT nFixedHeight = _ttoi(csWindow);
   if (m_pAssistantDoc)
      m_pAssistantDoc->SetRegionSettings(bRegionFixed, nFixedX, nFixedY, nFixedWidth, nFixedHeight);
 
	CPropertyPage::OnOK();
}

void CSGSettingsDlg::Init(CAssistantDoc *pAssistantDoc)
{
   m_pAssistantDoc = pAssistantDoc;


}

void CSGSettingsDlg::OnUpdateEditFrAdaptive() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	CString csText;
   m_edtFrAdaptive.GetWindowText(csText);
   int nVal = _ttoi(csText);
   if(nVal > 30)
   {
      nVal = 30;
      csText.Format(_T("%d"), nVal);

      m_edtFrAdaptive.SetWindowText(csText);
   }
   
   m_edtFrAdaptive.SetSel(csText.GetLength(), -1);


   CButton* radioAdaptiv = (CButton*)GetDlgItem(IDC_RADIO_FRAMERATE_ADAPTIVE);
   bool bAdaptiveFramerate = radioAdaptiv->GetCheck() == 0 ? false : true;

   m_edtFrAdaptive.EnableWindow(bAdaptiveFramerate);
   m_stAdaptiveWidth.EnableWindow(bAdaptiveFramerate);
   m_edtFrAdaptiveWidth.EnableWindow(bAdaptiveFramerate);
   m_stAdaptiveHeight.EnableWindow(bAdaptiveFramerate);
   m_edtFrAdaptiveHeight.EnableWindow(bAdaptiveFramerate);
   m_spnFrAdaptive.EnableWindow(bAdaptiveFramerate);
}

void CSGSettingsDlg::OnUpdateEditFrStatic() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
   CString csText;

   m_edtFrStatic.GetWindowText(csText);
   int nVal = _ttoi(csText);
   if(nVal > 30)
   {
      nVal = 30;
      csText.Format(_T("%d"), nVal);
      m_edtFrStatic.SetWindowText(csText);
   }
   int iLen = csText.GetLength();
   m_edtFrStatic.SetSel(csText.GetLength(), -1);

   
   CButton* radioStatic = (CButton*)GetDlgItem(IDC_RADIO_FRAMERATE_STATIC);
   bool bStaticFramerate = radioStatic->GetCheck() == 0 ? false : true;
   
   m_spnFrStatic.EnableWindow(bStaticFramerate);
   m_edtFrStatic.EnableWindow(bStaticFramerate);
}

void CSGSettingsDlg::OnUpdateCheckFixedRegion() 
{
   
   bool bRegionFixed = m_chkFixedRegion.GetCheck() == 0 ? false: true;
   
   m_stFixedX.EnableWindow(bRegionFixed);
   m_edtFixedX.EnableWindow(bRegionFixed);
   m_stFixedY.EnableWindow(bRegionFixed);
   m_edtFixedY.EnableWindow(bRegionFixed);
   m_stFixedWidth.EnableWindow(bRegionFixed);
   m_edtFixedWidth.EnableWindow(bRegionFixed);
   m_stFixedHeight.EnableWindow(bRegionFixed);
   m_edtFixedHeight.EnableWindow(bRegionFixed);
}


