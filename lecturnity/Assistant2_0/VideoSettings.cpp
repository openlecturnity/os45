// VideoSettings.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "VideoSettings.h"
#include "AVCSettings.h"
#include "AssistantDoc.h"
#include "backend\la_project.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CVideoSettings property page

IMPLEMENT_DYNCREATE(CVideoSettings, CPropertyPage)

CVideoSettings::CVideoSettings() : CPropertyPage(CVideoSettings::IDD)
{
	//{{AFX_DATA_INIT(CVideoSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
   
   m_bRecordVideo = false;
   m_bShowMonitor = false;
   m_bUseAlternativeSync = false;
   m_bSetAvOffset = false;
   m_iAvOffset = 0;
   m_nXResolution = 0;
   m_nYResolution = 0;
   m_nBitPerColor = 0;
   m_nFrameRateIndex = 0;
   m_bUseVFW = true;
   m_nVFWIndex = 0;
   m_nWDMIndex = 0;

   m_cursorWait = LoadCursor(NULL, IDC_WAIT); 

}

CVideoSettings::~CVideoSettings()
{
}

void CVideoSettings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoSettings)
	DDX_Control(pDX, IDC_COMBO_VIDEO_FRAMERATE, m_cmbFramerate);
	DDX_Control(pDX, IDC_COMBO_VIDEO_DRIVER, m_cmbDriverModel);
	DDX_Control(pDX, IDC_STATIC_RESOLUTION, m_stResolution);
	DDX_Control(pDX, IDC_STATIC_COLORDEPTH, m_stColordepth);
	DDX_Control(pDX, IDC_EDIT_AV_OFFSET, m_edtAvOffset);
	DDX_Control(pDX, IDC_STATIC_AV_MS, m_stAvMs);
	DDX_Control(pDX, IDC_CHECK_AV_OFFSET, m_chkAvOffset);
	DDX_Control(pDX, IDC_CHECK_ALTERNATIVE_AV_SYNC, m_chkAltAVSync);
	DDX_Control(pDX, IDC_CHECK_VIDEO_MONITOR, m_chkVideoMonitor);
	DDX_Control(pDX, IDC_CHECK_RECORD_VIDEO, m_chkRecordVideo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVideoSettings, CPropertyPage)
	//{{AFX_MSG_MAP(CVideoSettings)
	ON_BN_CLICKED(IDC_CHECK_RECORD_VIDEO, OnCheckRecordVideo)
	ON_BN_CLICKED(IDC_CHECK_VIDEO_MONITOR, OnCheckVideoMonitor)
	ON_BN_CLICKED(IDC_CHECK_AV_OFFSET, OnCheckAvOffset)
	ON_EN_CHANGE(IDC_EDIT_AV_OFFSET, OnChangeEditAvOffset)
	ON_BN_CLICKED(IDC_CHECK_ALTERNATIVE_AV_SYNC, OnCheckAlternativeAvSync)
	ON_UPDATE_COMMAND_UI(IDC_CHECK_ALTERNATIVE_AV_SYNC, OnUpdateCheckAlternativeAvSync)
   ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
   ON_UPDATE_COMMAND_UI(IDC_CHECK_RECORD_VIDEO, OnUpdateChkRecordVideo)
   ON_UPDATE_COMMAND_UI(IDC_CHECK_VIDEO_MONITOR, OnUpdateChkVideoMonitor)
   ON_UPDATE_COMMAND_UI(IDC_CHECK_ALTERNATIVE_AV_SYNC, OnUpdateChkAlternativeAVSync)
   ON_UPDATE_COMMAND_UI(IDC_CHECK_AV_OFFSET, OnUpdateChkAVOffset)
   ON_UPDATE_COMMAND_UI(IDC_EDIT_AV_OFFSET, OnUpdateEdtAVOffset)
   ON_UPDATE_COMMAND_UI(IDC_STATIC_AV_MS, OnUpdateLblAVms)
   ON_UPDATE_COMMAND_UI(IDC_COMBO_VIDEO_FRAMERATE, OnUpdateCmbFramerate)
   ON_UPDATE_COMMAND_UI(IDC_COMBO_VIDEO_DRIVER, OnUpdateBtnWindowsDriver)
	ON_CBN_SELCHANGE(IDC_COMBO_VIDEO_DRIVER, OnSelchangeComboVideoDriver)
	ON_BN_CLICKED(IDC_BUTTON_WDM_DV, OnButtonWdmDv)
   ON_UPDATE_COMMAND_UI(IDC_BUTTON_WDM_DV, OnUpdateButtonWdmDv)
	ON_BN_CLICKED(IDC_BUTTON_WDM_FORMAT, OnButtonWdmFormat)
   ON_UPDATE_COMMAND_UI(IDC_BUTTON_WDM_FORMAT, OnUpdateButtonWdmFormat)
	ON_BN_CLICKED(IDC_BUTTON_WDM_SETTINGS, OnButtonWdmSettings)
   ON_UPDATE_COMMAND_UI(IDC_BUTTON_WDM_SETTINGS, OnUpdateButtonWdmSettings)
	ON_BN_CLICKED(IDC_BUTTON_WDM_SOURCE, OnButtonWdmSource)
   ON_UPDATE_COMMAND_UI(IDC_BUTTON_WDM_SOURCE, OnUpdateButtonWdmSource)
	ON_BN_CLICKED(IDC_CHECK_AV_OFFSET, OnAvOffset)
   ON_UPDATE_COMMAND_UI(IDC_STATIC_RESOLUTION, OnUpdateResolutionSource)
   ON_UPDATE_COMMAND_UI(IDC_STATIC_COLORDEPTH, OnUpdateColorDepthSource)
	ON_CBN_SELCHANGE(IDC_COMBO_VIDEO_FRAMERATE, OnSelchangeComboVideoFramerate)
	ON_CBN_EDITCHANGE(IDC_COMBO_VIDEO_FRAMERATE, OnEditchangeComboVideoFramerate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoSettings message handlers


BOOL CVideoSettings::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
   /*IDC_STATIC_VIDEO_GB_SETTINGS    
   IDC_STATIC_VIDEO_RESOLUTION     
   IDC_STATIC_VIDEO_COLORDEPTH     
   IDC_STATIC_VIDEO_FRAMERATE  */
   DWORD dwIds[] = {IDC_STATIC_VIDEO_GB_SETTINGS,
                    IDC_STATIC_VIDEO_RESOLUTION,
                    IDC_STATIC_VIDEO_COLORDEPTH,
                    IDC_STATIC_VIDEO_FRAMERATE,
                    IDC_STATIC_AV_MS,
                    IDC_CHECK_RECORD_VIDEO,          
                    IDC_CHECK_VIDEO_MONITOR,         
                    IDC_CHECK_ALTERNATIVE_AV_SYNC,             
                    IDC_CHECK_AV_OFFSET,            
                    IDC_BUTTON_WDM_SETTINGS, 
                    IDC_BUTTON_WDM_SOURCE,
                    IDC_BUTTON_WDM_FORMAT,
                    IDC_BUTTON_WDM_DV,
                    IDC_STATIC_VIDEO_GB_DRIVER,
                    -1};
   MfcUtils::Localize(this, dwIds);


   if (m_bRecordVideo)
   {
      CAVCSettings* pParent = (CAVCSettings*)GetParent();
      
      SetCursor(m_cursorWait);
      HRESULT hr = m_pAssistantDoc->EnableVideoRecording(m_bRecordVideo, false);
      
      if (!SUCCEEDED(hr))
         m_chkRecordVideo.SetCheck(false);

      SetCursor(NULL);
   }

   UpdatePage();
   UpdateCodecPage();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVideoSettings::Init(CAssistantDoc *pAssistantDoc)
{
   m_pAssistantDoc = pAssistantDoc;
}

void CVideoSettings::OnCheckRecordVideo() 
{
	CAVCSettings* pParent = (CAVCSettings*)GetParent();
   
   m_bRecordVideo = m_chkRecordVideo.GetCheck() == 0 ? false: true;

   SetCursor(m_cursorWait);
   HRESULT hr = m_pAssistantDoc->EnableVideoRecording(m_bRecordVideo, true);

   if (!SUCCEEDED(hr))
      m_chkRecordVideo.SetCheck(false);

   UpdatePage();
   UpdateCodecPage();
   
   SetCursor(NULL);

}

void CVideoSettings::UpdatePage()
{
   m_bRecordVideo = false;
   m_bShowMonitor = false;
   m_bUseAlternativeSync = false;
   m_bSetAvOffset = false;
   m_iAvOffset = 0;
   m_nXResolution = 0;
   m_nYResolution = 0;
   m_nBitPerColor = 0;
   m_arFrameRates.RemoveAll();
   m_nFrameRateIndex = 0;
   m_bUseVFW = true;
   m_arVFWSources.RemoveAll();
   m_nVFWIndex = 0;
   m_arWDMSources.RemoveAll();
   m_nWDMIndex = 0;

   if (m_pAssistantDoc)
   {
      m_pAssistantDoc->GetVideoSettings(m_bRecordVideo, m_bShowMonitor, m_bUseAlternativeSync,
                                        m_bSetAvOffset, m_iAvOffset);
      
      m_pAssistantDoc->GetVideoParameter(m_bUseVFW, m_arVFWSources, m_nVFWIndex,
                                         m_arWDMSources, m_nWDMIndex,
                                         m_nXResolution, m_nYResolution, m_nBitPerColor, 
                                         m_arFrameRates, m_nFrameRateIndex);
   }

   CButton *pRecordVideo = (CButton *)GetDlgItem(IDC_CHECK_RECORD_VIDEO);
   pRecordVideo->SetCheck(m_bRecordVideo);

   CButton *pShowMonitorButton = (CButton *)GetDlgItem(IDC_CHECK_VIDEO_MONITOR);
   pShowMonitorButton->SetCheck(m_bShowMonitor);
   
   CButton *pUseAlternativeSyncButton = (CButton *)GetDlgItem(IDC_CHECK_ALTERNATIVE_AV_SYNC);
   pUseAlternativeSyncButton->SetCheck(m_bUseAlternativeSync);

   CButton *pSetAvOffsetButton = (CButton *)GetDlgItem(IDC_CHECK_AV_OFFSET);
   pSetAvOffsetButton->SetCheck(m_bSetAvOffset);
   
	CEdit* pAvOffsetEdit = (CEdit*) GetDlgItem(IDC_EDIT_AV_OFFSET);
   CString csAvOffset;
   csAvOffset.Format(_T("%d"), m_iAvOffset);
   pAvOffsetEdit->SetWindowText(csAvOffset);

   CString csResolution;
   csResolution.Format(_T("%d x %d"), m_nXResolution, m_nYResolution);
   CStatic *pResolution = (CStatic *)GetDlgItem(IDC_STATIC_RESOLUTION);
   pResolution->SetWindowText(csResolution);
   
   CString csBitPerColor;
   csBitPerColor.Format(_T("%d"), m_nBitPerColor);
   CStatic *pBitPerColor = (CStatic *)GetDlgItem(IDC_STATIC_COLORDEPTH);
   pBitPerColor->SetWindowText(csBitPerColor);

   CComboBox *pFrameRates = (CComboBox *)GetDlgItem(IDC_COMBO_VIDEO_FRAMERATE);
   pFrameRates->ResetContent();
   for (int i = 0; i < m_arFrameRates.GetSize(); ++i)
   {
      CString csFramerate;
      csFramerate.Format(_T("%2.2f"), m_arFrameRates[i]);
      pFrameRates->AddString(csFramerate);
   }
   pFrameRates->SetCurSel(m_nFrameRateIndex);

   CComboBox *pWDMSource = (CComboBox *)GetDlgItem(IDC_COMBO_VIDEO_DRIVER);
   pWDMSource->ResetContent();
   for (i = 0; i < m_arWDMSources.GetSize(); ++i)
      pWDMSource->AddString(m_arWDMSources[i]);
   pWDMSource->SetCurSel(m_nWDMIndex);

}
/*
void CVideoSettings::EnableCodecPage(bool bEnable)
{
   CAVCSettings* pParent = (CAVCSettings*)GetParent();

   if (pParent)
      pParent->EnableCodecTab(bEnable);
}
*/
void CVideoSettings::UpdateCodecPage()
{
   CAVCSettings* pParent = (CAVCSettings*)GetParent();

   if (pParent && m_bRecordVideo)
      pParent->UpdateCodecPage();
}

void CVideoSettings::OnCheckVideoMonitor() 
{
   if (m_pAssistantDoc)
   {
      bool bShow = m_chkVideoMonitor.GetCheck() == 0 ? false: true;
      m_pAssistantDoc->ShowVideoMonitor(bShow);
   }
}

void CVideoSettings::OnCheckAvOffset() 
{
   m_bSetAvOffset = m_chkAvOffset.GetCheck() == 0 ? false: true;

   CString csAvOffset;
   m_edtAvOffset.GetWindowText(csAvOffset);
   m_iAvOffset = _ttoi(csAvOffset);

   if (m_pAssistantDoc)
   {
      m_pAssistantDoc->SetUseAvOffset(m_bSetAvOffset);
   }

	m_edtAvOffset.EnableWindow(m_chkAvOffset.GetCheck());
}

void CVideoSettings::OnChangeEditAvOffset() 
{
   if (m_pAssistantDoc)
   {
      CString csText;
      m_edtAvOffset.GetWindowText(csText);
      m_iAvOffset = _ttoi(csText);
      m_pAssistantDoc->SetAvOffset(m_iAvOffset);
   }
}

void CVideoSettings::OnCheckAlternativeAvSync() 
{
   if (m_pAssistantDoc)
   {
      m_bUseAlternativeSync = m_chkAltAVSync.GetCheck() == 0 ? false: true;
      m_pAssistantDoc->SetAlternativeAvSync(m_bUseAlternativeSync);
   }
}

void CVideoSettings::OnUpdateCheckAlternativeAvSync(CCmdUI *pCmdUI) 
{
   if (m_bUseAlternativeSync)
      pCmdUI->SetCheck(true);
   else
      pCmdUI->SetCheck(false);

}

void CVideoSettings::OnSelchangeComboVideoDriver() 
{
   int iCurrentDevice = m_cmbDriverModel.GetCurSel();

   if (iCurrentDevice != -1)
   {
      CString csWdmDevice = m_arWDMSources[iCurrentDevice];
      m_pAssistantDoc->ChangeWdmDevice(csWdmDevice);
      UpdateCodecPage();
   }
}

void CVideoSettings::OnOK() 
{
   if (m_pAssistantDoc)
   {
      m_pAssistantDoc->ShowVideoMonitor(false);
      m_pAssistantDoc->WriteVideoSettings();
   }


	CPropertyPage::OnOK();
}

void CVideoSettings::OnKickIdle()
{
   UpdateDialogControls(this, false);
}

void CVideoSettings::OnUpdateChkRecordVideo(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck(m_bRecordVideo);

//   EnableCodecPage(m_bRecordVideo);
}

void CVideoSettings::OnUpdateChkVideoMonitor(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bRecordVideo);
}

void CVideoSettings::OnUpdateChkAlternativeAVSync(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bRecordVideo);
}

void CVideoSettings::OnUpdateChkAVOffset(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bRecordVideo);
}

void CVideoSettings::OnUpdateEdtAVOffset(CCmdUI *pCmdUI)
{
   pCmdUI->Enable((m_bRecordVideo)&&(m_bSetAvOffset));
}

void CVideoSettings::OnUpdateLblAVms(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bRecordVideo);
}

void CVideoSettings::OnUpdateCmbFramerate(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_bRecordVideo);
}

void CVideoSettings::OnUpdateBtnWindowsDriver(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(!m_bUseVFW);
}

void CVideoSettings::OnButtonWdmDv() 
{
   if (ASSISTANT::Project::active != NULL)
   {
      ASSISTANT::Project::active->ShowWdmDv();  
      UpdateCodecPage();
   }
}

void CVideoSettings::OnUpdateButtonWdmDv(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (ASSISTANT::Project::active != NULL)
      bEnable = ASSISTANT::Project::active->HasWdmDv();

   pCmdUI->Enable(bEnable);
}

void CVideoSettings::OnButtonWdmFormat() 
{
   if (ASSISTANT::Project::active != NULL)
   {
      ASSISTANT::Project::active->ShowWdmFormat();
      UpdateCodecPage();
   }
}
void CVideoSettings::OnUpdateButtonWdmFormat(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (ASSISTANT::Project::active != NULL)
      bEnable = ASSISTANT::Project::active->HasWdmFormat();

   pCmdUI->Enable(bEnable);
}


void CVideoSettings::OnButtonWdmSettings() 
{
   if (ASSISTANT::Project::active != NULL)
   {
      ASSISTANT::Project::active->ShowWdmSettings();
      UpdateCodecPage();
   }
}

void CVideoSettings::OnUpdateButtonWdmSettings(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (ASSISTANT::Project::active != NULL)
      bEnable = ASSISTANT::Project::active->HasWdmSettings();

   pCmdUI->Enable(bEnable);
}

void CVideoSettings::OnButtonWdmSource() 
{
   if (ASSISTANT::Project::active != NULL)
   {
      ASSISTANT::Project::active->ShowWdmSource();  
      UpdateCodecPage();
   }
}

void CVideoSettings::OnUpdateButtonWdmSource(CCmdUI *pCmdUI)
{
   bool bEnable = false;

   if (ASSISTANT::Project::active != NULL)
      bEnable = ASSISTANT::Project::active->HasWdmSource();

   pCmdUI->Enable(bEnable);
}


void CVideoSettings::OnAvOffset() 
{
   m_bUseAlternativeSync = !m_bUseAlternativeSync;
   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SetAlternativeAvSync(m_bUseAlternativeSync);
}

void CVideoSettings::OnUpdateResolutionSource(CCmdUI *pCmdUI)
{
   CString csResolution;
   if (ASSISTANT::Project::active && m_bRecordVideo)
   {
      ASSISTANT::Project::active->GetVideoResolution(m_nXResolution, m_nYResolution);
      csResolution.Format(_T("%d x %d"), m_nXResolution, m_nYResolution);
      pCmdUI->SetText(csResolution);
   }
   else
   {
      pCmdUI->SetText(_T("0 x 0"));
   }
}

void CVideoSettings::OnUpdateColorDepthSource(CCmdUI *pCmdUI)
{
   CString csResolution;
   if (ASSISTANT::Project::active && m_bRecordVideo)
   {
      ASSISTANT::Project::active->GetColorDepth(m_nBitPerColor);
      CString csBitPerColor;
      csBitPerColor.Format(_T("%d"), m_nBitPerColor);
      pCmdUI->SetText(csBitPerColor);
   }
   else
   {
      pCmdUI->SetText(_T("0"));
   }
}

void CVideoSettings::OnSelchangeComboVideoFramerate() 
{
   m_nFrameRateIndex = m_cmbFramerate.GetCurSel();
   float fFramerate = m_arFrameRates[m_nFrameRateIndex];

   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SetVideoFramerate(fFramerate);
}

void CVideoSettings::OnEditchangeComboVideoFramerate() 
{
   CString csFramerate;
   m_cmbFramerate.GetWindowText(csFramerate);
   TCHAR *stopstring;
   float fFramerate = _tcstod(csFramerate, &stopstring);
   
   if (ASSISTANT::Project::active != NULL)
      ASSISTANT::Project::active->SetVideoFramerate(fFramerate);
}

void CVideoSettings::OnCancel() 
{
   if (m_pAssistantDoc)
   {
      m_pAssistantDoc->ReadVideoSettings(false);
   }

	CPropertyPage::OnCancel();
}
