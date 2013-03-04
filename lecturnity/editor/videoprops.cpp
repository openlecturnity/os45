// videoprops.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "videoprops.h"
#include "MainFrm.h"
#include "MfcUtils.h" // Localize
#include "misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "DebugDlg.h"

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CVideoProps 

IMPLEMENT_DYNCREATE(CVideoProps, CPropertyPage)

CVideoProps::CVideoProps(DWORD dwId, CAviSettings *pAviSettings /*=NULL*/, 
                         const _TCHAR *tszClipSource/*=NULL*/, int nClipNr/*=0*/) : 
   CPropertyPage(dwId)
{
	//{{AFX_DATA_INIT(CVideoProps)
	//}}AFX_DATA_INIT

   m_plCodecList = NULL;
   m_codecListSize = 0;
   m_pAviSettings = pAviSettings;
#ifdef _DEBUG
   //CDebugDlg::DisplayAviSettings(NULL, pAviSettings);
#endif
   if (tszClipSource)
   {
      m_bHasClipSource = true;
      m_csClipSource = tszClipSource;
      m_csCaption.Format(_T("Clip %d"), nClipNr);
      m_psp.pszTitle = m_csCaption;
      m_psp.dwFlags |= PSP_USETITLE;
   }
   else
   {
      m_bHasClipSource = false;
      m_csClipSource = _T("");
   }
}

CVideoProps::~CVideoProps()
{
   if (m_plCodecList)
   {
      for (int i=0; i<m_codecListSize; ++i)
         delete m_plCodecList[i];
      delete[] m_plCodecList;
      m_plCodecList = NULL;
      m_codecListSize = 0;
   }
}

void CVideoProps::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoProps)
	DDX_Control(pDX, IDC_RESET_VIDEOSIZE, m_btResetVideoSize);
	DDX_Control(pDX, IDC_LABEL_OFFSET, m_offsetLabel);
	DDX_Control(pDX, IDC_LABEL_MS, m_msLabel);
	DDX_Control(pDX, IDC_VP_OFFSET, m_offsetEdit);
	DDX_Control(pDX, IDC_VP_SMART_RECOMPRESS, m_smartRecompressCheck);
	DDX_Control(pDX, IDC_VP_SETDEFAULT, m_setDefaultButton);
	DDX_Control(pDX, IDC_VP_ABOUT, m_aboutButton);
	DDX_Control(pDX, IDC_VP_CONFIGURE, m_configureButton);
	DDX_Control(pDX, IDC_KEYFRAMES_EDIT, m_keyframesEdit);
	DDX_Control(pDX, IDC_KEYFRAMES_CHECK, m_useKeyframesRadio);
	DDX_Control(pDX, IDC_QUALITYLABEL, m_qualityLabel);
	DDX_Control(pDX, IDC_QUALITY, m_qualitySlider);
	DDX_Control(pDX, IDC_VP_FRAMERATE, m_frameRateEdit);
	DDX_Control(pDX, IDC_VP_SIZEY, m_sizeYEdit);
	DDX_Control(pDX, IDC_VP_SIZEX, m_sizeXEdit);
	DDX_Control(pDX, IDC_VP_KEEPASPECT, m_keepAspectRadio);
	DDX_Control(pDX, IDC_VP_CODEC, m_codecCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVideoProps, CPropertyPage)
	//{{AFX_MSG_MAP(CVideoProps)
	ON_BN_CLICKED(IDC_VP_KEEPASPECT, OnKeepAspectClick)
	ON_EN_KILLFOCUS(IDC_VP_SIZEX, OnSizeXLostFocus)
	ON_EN_KILLFOCUS(IDC_VP_SIZEY, OnSizeYLostFocus)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_VP_CODEC, OnCodecChange)
	ON_BN_CLICKED(IDC_VP_SETDEFAULT, OnSetCodecDefault)
	ON_BN_CLICKED(IDC_VP_CONFIGURE, OnConfigureCodec)
	ON_BN_CLICKED(IDC_VP_ABOUT, OnAboutCodec)
	ON_EN_CHANGE(IDC_VP_SIZEX, OnEditChange)
	ON_BN_CLICKED(IDC_KEYFRAMES_CHECK, OnUseKeyframesClick)
	ON_EN_KILLFOCUS(IDC_VP_FRAMERATE, OnFrameRateLostFocus)
	ON_BN_CLICKED(IDC_VP_SMART_RECOMPRESS, OnSmartRecompressClick)
	ON_EN_CHANGE(IDC_VP_OFFSET, OnChangeOffset)
	ON_EN_CHANGE(IDC_VP_SIZEY, OnEditChange)
	ON_EN_CHANGE(IDC_VP_FRAMERATE, OnEditChange)
	ON_EN_CHANGE(IDC_KEYFRAMES_EDIT, OnEditChange)
	ON_EN_KILLFOCUS(IDC_VP_OFFSET, OnOffsetLostFocus)
   ON_BN_CLICKED(IDC_RESET_VIDEOSIZE, OnResetVideoSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CVideoProps 

BOOL CVideoProps::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Zusätzliche Initialisierung hier einfügen

   DWORD dwIds[] = {IDC_VIDEOSIZE_BOX, // this id is not always present; ie not when called from CClipCodecWnd
                    IDC_VIDEOSIZE_LABEL,
                    IDC_VP_KEEPASPECT,
                    IDC_FRAMERATE_LABEL,
                    IDC_LABEL_OFFSET,
                    IDC_LABEL_MS,
                    IDC_CODEC_BOX,
                    IDC_CODEC_LABEL,
                    IDC_VP_CONFIGURE,
                    IDC_VP_ABOUT,
                    IDC_QUALITY_EXPL,
                    IDC_KEYFRAMES_EXPL,
                    IDC_KEYFRAMES_CHECK,
                    IDC_KEYFRAMES_LABEL,
                    IDC_VP_SETDEFAULT,
                    IDC_VP_SMART_RECOMPRESS,
                    IDC_RESET_VIDEOSIZE,
                    -1};
   MfcUtils::Localize(this, dwIds);

   m_pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();

   /*
   m_codecCombo.AddString("Hallo 1");
   m_codecCombo.AddString("Hallo 2");
   m_codecCombo.AddString("Hallo 3");
   m_codecCombo.AddString("Hallo 4");
   m_codecCombo.AddString("Hallo 5");
   m_codecCombo.AddString("Hallo 6");
   m_codecCombo.AddString("Hallo 7");
   m_codecCombo.AddString("Hallo 8");
   m_codecCombo.AddString("Hallo 9");
   m_codecCombo.AddString("Hallo 8");
   m_codecCombo.AddString("Hallo 7");
   m_codecCombo.AddString("Hallo 6");
   m_codecCombo.AddString("Hallo 5");
   m_codecCombo.SetCurSel(3);
   */

   if (!m_pDoc->project.HasVideo() && 
       !m_bHasClipSource && 
       !m_pDoc->project.IsDenverDocument())
   {
      Misc::EnableChildWindows(this, FALSE);

      m_isEnabled = false;

      return TRUE;
   }

   m_isEnabled = true;

   m_lastVideoSize.cx = -1;
   m_lastVideoSize.cy = -1;
   m_lastCodecSelected = 0xffffffff; // -1
   m_isFirstTime = true;

   m_keepAspectRadio.SetCheck(TRUE);

   m_sizeXEdit.SetLimitText(4);
   m_sizeYEdit.SetLimitText(4);

   m_qualitySlider.SetRange(0, 100);
   m_qualitySlider.SetPos(75);
   m_qualityLabel.SetWindowText(_T("75%"));
   m_useKeyframesRadio.SetCheck(TRUE);
   m_keyframesEdit.SetLimitText(4);
   m_keyframesEdit.SetWindowText(_T("15"));

   if (m_bHasClipSource)
   {
      m_offsetEdit.ShowWindow(SW_HIDE);
      m_msLabel.ShowWindow(SW_HIDE);
      m_offsetLabel.ShowWindow(SW_HIDE);
   }

   ReadDefaultSettings();
   PopulateCodecSettings();

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CVideoProps::ReadDefaultSettings()
{
   if (!m_pAviSettings->bIsVideoFormatInitialized)
   {
      if (!m_bHasClipSource)
      {
         // Normal video case
         m_pDoc->project.InitMediaTypes();
      }
      else
      {
         // Clip case
         m_pDoc->project.InitAviSettingsFromFile(m_pAviSettings, m_csClipSource, NULL);
      }
   }

   if (!m_pAviSettings->bIsVideoFormatInitialized)
   {
      // disable whole darn page
      EnableWindow(FALSE);
      return;
   }

   if (!(m_pAviSettings->videoFormat.formattype == FORMAT_VideoInfo))
   {
      if (m_pAviSettings->videoFormat.formattype == FORMAT_WaveFormatEx)
         MessageBox(_T("FORMAT_WaveFormatEx"), _T("Information"));
      else if (m_pAviSettings->videoFormat.formattype == FORMAT_VideoInfo2)
         MessageBox(_T("FORMAT_VideoInfo2"), _T("Information"));
      if (m_pAviSettings->videoFormat.formattype == FORMAT_DvInfo)
         MessageBox(_T("FORMAT_DvInfo"), _T("Information"));
      if (m_pAviSettings->videoFormat.formattype == FORMAT_MPEGVideo)
         MessageBox(_T("FORMAT_MPEGVideo"), _T("Information"));
      if (m_pAviSettings->videoFormat.formattype == FORMAT_MPEG2Video)
         MessageBox(_T("FORMAT_MPEG2Video"), _T("Information"));
      
      // TODO: Error message
      MessageBox(_T("Unknown error"));

      m_sizeXEdit.SetWindowText(_T("320"));
      m_sizeYEdit.SetWindowText(_T("240"));
      m_frameRateEdit.SetWindowText(_T("25.000"));
      return;
   }

   VIDEOINFOHEADER *pHeader = 
      (VIDEOINFOHEADER *) m_pAviSettings->videoFormat.pbFormat;

   CString xsize;
   xsize.Format(_T("%d"), pHeader->bmiHeader.biWidth);
   m_sizeXEdit.SetWindowText(xsize);
   CString ysize;
   ysize.Format(_T("%d"), pHeader->bmiHeader.biHeight);
   m_sizeYEdit.SetWindowText(ysize);

   // Aspect ratio?
   if (m_pAviSettings->bHasOriginalSize)
   {
      m_initialAspectRatio = 
         ((double) m_pAviSettings->nOriginalWidth) / ((double) m_pAviSettings->nOriginalHeight);
   }
   else
   {
      m_initialAspectRatio = 
         ((double) pHeader->bmiHeader.biWidth) / ((double) pHeader->bmiHeader.biHeight);
   }
   m_keepAspectRadio.SetCheck(m_pAviSettings->bRememberAspectRatio ? BST_CHECKED : BST_UNCHECKED);

   if (m_pAviSettings->bHasOriginalSize)
      m_btResetVideoSize.EnableWindow(TRUE);
   else
      m_btResetVideoSize.EnableWindow(FALSE);

   // calculate the frame rate
   m_avgTimePerFrame100ns = pHeader->AvgTimePerFrame;
   double fps = 25.0;
   if (m_avgTimePerFrame100ns <= 0)
   {
      m_avgTimePerFrame100ns = 400000i64;
      pHeader->AvgTimePerFrame = 400000i64;
      CString csMsg;
      csMsg.LoadString(IDS_MSG_INVALID_FRAMERATE);
      CString csCap;
      csCap.LoadString(IDS_WARNING_E);
      MessageBox(csMsg, csCap, MB_OK | MB_ICONWARNING);
   }
   fps = 10000000.0 / ((double) m_avgTimePerFrame100ns);
   CString framerate;
   framerate.Format(_T("%0.3f"), fps);
   m_frameRateEdit.SetWindowText(framerate);

   // set the global video offset
   CString csOffset;
   csOffset.Format(_T("%d"), m_pDoc->project.m_videoExportGlobalOffset);
   m_offsetEdit.SetWindowText(csOffset);
}

void CVideoProps::PopulateCodecSettings()
{
   CWaitCursor wc;

   CString temp;
   m_sizeXEdit.GetWindowText(temp);
   int xsize = _ttoi(temp);
   m_sizeYEdit.GetWindowText(temp);
   int ysize = _ttoi(temp);

   if (xsize == m_lastVideoSize.cx &&
       ysize == m_lastVideoSize.cy)
      return;

   m_lastVideoSize.cx = xsize;
   m_lastVideoSize.cy = ysize;

   if (xsize <= 0 || ysize <= 0)
   {
      return;
   }

   //BeginWaitCursor();

   if (m_plCodecList)
   {
      for (int i=0; i<m_codecListSize; ++i)
         delete m_plCodecList[i];
      delete[] m_plCodecList;
      m_plCodecList = NULL;
      m_codecListSize = 0;
   }

   //AVInfo avInfo;
   m_codecListSize = m_avInfo.GetCodecListSize(xsize, ysize);
   m_plCodecList = new CODECINFO*[m_codecListSize];
   for (int i=0; i<m_codecListSize; ++i)
      m_plCodecList[i] = new CODECINFO;
   m_avInfo.GetCodecList(m_plCodecList);

   m_codecCombo.ResetContent();
   for (i=0; i<m_codecListSize; ++i)
   {
      m_codecCombo.AddString(m_plCodecList[i]->tszDescription);
      m_codecCombo.SetItemDataPtr(i, m_plCodecList[i]);
   }

   // Let's see if we can find the codec that
   // was specified; default is number 0 if
   // no suitable codec was found
   int codecNr = -1;

   // first see if we can find a previously selected
   // codec
   codecNr = FindCodec(m_lastCodecSelected);

   if (codecNr == -1 && 
       m_pAviSettings->bIsVideoFormatInitialized)
   {
      codecNr = FindCodec(m_pAviSettings->dwFCC);
   }

   if (codecNr == -1)
   {
      // Actual codec was not found, let's see if
      // we can find Indeo 5
      DWORD iv50Fcc = MAKEFOURCC('I', 'V', '5', '0');

      codecNr = FindCodec(iv50Fcc);
   }

   if (codecNr == -1)
   {
      // Yikes, neither is iv50 installed;
      // ok, try uncompressed video then.
      codecNr = 0;
   }

   m_codecCombo.SetCurSel(codecNr);
   UpdateCodecControls();

   if (m_plCodecList[codecNr]->bSupportsQuality && m_isFirstTime)
   {
      // check quality setting from document
      int givenQuality = m_pAviSettings->quality;
      int quality = 0;
      if (givenQuality > 0 && givenQuality <= 100)
         quality = givenQuality;
      else
         quality = m_plCodecList[codecNr]->nDefaultQuality;
      m_qualitySlider.SetPos(quality);
      CString q;
      q.Format(_T("%d%%"), quality);
      m_qualityLabel.SetWindowText(q);
   }

   if (m_plCodecList[codecNr]->bSupportsKeyframes && m_isFirstTime)
   {
      m_useKeyframesRadio.SetCheck(m_pAviSettings->bUseKeyframes);
      // check keyframe setting from document
      int givenKFRate = m_pAviSettings->keyframeRate;
      CString kf;
      if (givenKFRate > 0 && givenKFRate <= 500)
         kf.Format(_T("%d"), givenKFRate);
      else
         kf.Format(_T("%d"), m_plCodecList[codecNr]->nDefaultKeyframeRate);
      m_keyframesEdit.SetWindowText(kf);
   }

   if (m_isFirstTime)
   {
      m_smartRecompressCheck.SetCheck(m_pAviSettings->bUseSmartRecompression);
   }

   m_isFirstTime = false;

   // Now set the previously selected codec
   m_lastCodecSelected = m_plCodecList[codecNr]->fcc;

   //EndWaitCursor();
}

int CVideoProps::FindCodec(DWORD fcc)
{
   // DWORD fcc = m_pDoc->project.m_videoExportFCC;
   bool foundCodec = false;
   int codecNr = -1;
   for (int i=0; i<m_codecListSize && !foundCodec; ++i)
   {
      if (m_plCodecList[i]->fcc == fcc ||
          m_plCodecList[i]->alternativeFcc == fcc)
      {
         codecNr = i;
         foundCodec = true;
      }
   }

   return codecNr;
}

void CVideoProps::UpdateCodecControls()
{
   CODECINFO *pInfo = 
      (CODECINFO *) m_codecCombo.GetItemDataPtr(m_codecCombo.GetCurSel());
   if (pInfo)
   {
      m_configureButton.EnableWindow(pInfo->bHasConfigureDialog);
      m_aboutButton.EnableWindow(pInfo->bHasAboutDialog);

      bool q = pInfo->bSupportsQuality;
      m_qualityLabel.EnableWindow(q);
      m_qualitySlider.EnableWindow(q);

      bool k = pInfo->bSupportsKeyframes;
      m_useKeyframesRadio.EnableWindow(k);
      if (m_useKeyframesRadio.GetCheck() != FALSE)
         m_keyframesEdit.EnableWindow(k);
      else
         m_keyframesEdit.EnableWindow(false);

      m_setDefaultButton.EnableWindow(q || k);

      m_lastCodecSelected = pInfo->fcc;
   }
   else
   {
      m_configureButton.EnableWindow(FALSE);
      m_aboutButton.EnableWindow(FALSE);
      m_qualityLabel.EnableWindow(FALSE);
      m_qualitySlider.EnableWindow(FALSE);
      m_useKeyframesRadio.EnableWindow(FALSE);
      m_keyframesEdit.EnableWindow(FALSE);
   }
}

void CVideoProps::OnKeepAspectClick() 
{
   // Let the dialog match the Y size if keep aspect ratio
   // was activated
   if (m_keepAspectRadio.GetCheck() == BST_CHECKED)
      OnSizeXLostFocus();

   MySetModified();
}

void CVideoProps::OnSizeXLostFocus() 
{
   // Match y size
   CString xsizeText;
   m_sizeXEdit.GetWindowText(xsizeText);
   int xsize = _ttoi(xsizeText);
   if (xsize <= 0)
   {
      xsize = 320;
      m_sizeXEdit.SetWindowText(_T("320"));
   }

   if (m_keepAspectRadio.GetCheck() != 0)
   {
      int ysize = (int) (((double) xsize) / m_initialAspectRatio + .5);
      CString ysizeText;
      ysizeText.Format(_T("%d"), ysize);
      m_sizeYEdit.SetWindowText(ysizeText);
   }

   PopulateCodecSettings();
}

void CVideoProps::OnSizeYLostFocus() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	
   // MessageBox("Lost focus");
   // Match y size
   CString ysizeText;
   m_sizeYEdit.GetWindowText(ysizeText);
   int ysize = _ttoi(ysizeText);
   if (ysize <= 0)
   {
      ysize = 240;
      m_sizeYEdit.SetWindowText(_T("240"));
   }

   if (m_keepAspectRadio.GetCheck() != 0)
   {
      int xsize = (int) (((double) ysize) * m_initialAspectRatio + .5);
      CString xsizeText;
      xsizeText.Format(_T("%d"), xsize);
      m_sizeXEdit.SetWindowText(xsizeText);
   }

   PopulateCodecSettings();
}

void CVideoProps::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
   if (pScrollBar) // && nSBCode != TB_ENDTRACK) // && nSBCode != TB_PAGEDOWN && nSBCode != TB_PAGEUP)
   {
      int pos = m_qualitySlider.GetPos();
      CString temp;
      temp.Format(_T("%d%%"), pos);
      m_qualityLabel.SetWindowText(temp);
      MySetModified();
   }
	
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CVideoProps::OnCodecChange() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	
   UpdateCodecControls();
   MySetModified();
}

void CVideoProps::OnSetCodecDefault() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen

   int codecNr = m_codecCombo.GetCurSel();
   CODECINFO *pInfo = (CODECINFO *) m_codecCombo.GetItemDataPtr(codecNr);

   if (!pInfo)
      return;

   if (pInfo->bSupportsKeyframes)
   {
      m_useKeyframesRadio.SetCheck(TRUE);
      CString kf;
      kf.Format(_T("%d"), pInfo->nDefaultKeyframeRate);
      m_keyframesEdit.SetWindowText(kf);
   }
   else
   {
      m_useKeyframesRadio.SetCheck(TRUE);
      m_keyframesEdit.SetWindowText(_T("25"));
   }

   if (pInfo->bSupportsQuality)
   {
      m_qualitySlider.SetPos(pInfo->nDefaultQuality);
      CString q;
      q.Format(_T("%d%%"), pInfo->nDefaultQuality);
      m_qualityLabel.SetWindowText(q);
   }
   else
   {
      m_qualitySlider.SetPos(75);
      m_qualityLabel.SetWindowText(_T("75%"));
   }

   MySetModified();
}

void CVideoProps::OnConfigureCodec() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen

   int codecNr = m_codecCombo.GetCurSel();
   CODECINFO *pInfo = (CODECINFO *) m_codecCombo.GetItemDataPtr(codecNr);
   m_avInfo.ConfigureCodec(pInfo->fcc);
}

void CVideoProps::OnAboutCodec() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	
   int codecNr = m_codecCombo.GetCurSel();
   CODECINFO *pInfo = (CODECINFO *) m_codecCombo.GetItemDataPtr(codecNr);
   m_avInfo.AboutCodec(pInfo->fcc);
}

void CVideoProps::OnOK() 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen

   if (m_isEnabled)
   {
      if (!m_pAviSettings->bIsVideoFormatInitialized)
      {
         ZeroMemory(&m_pAviSettings->videoFormat, sizeof AM_MEDIA_TYPE);
      }

      int codecNr = m_codecCombo.GetCurSel();
      CODECINFO *pInfo = (CODECINFO *) m_codecCombo.GetItemDataPtr(codecNr);
      if (pInfo && (int)pInfo != -1)
      {
         if (!m_avInfo.CreateStreamFormat(pInfo->fcc, 
                                          m_lastVideoSize.cx,
                                          m_lastVideoSize.cy,
                                          &m_pAviSettings->videoFormat))
         {
            CString msg;
            msg.LoadString(IDS_MSG_MEDIATYPEERROR);
            CString cap;
            cap.LoadString(IDS_WARNING_E);
            MessageBox(msg, cap, MB_ICONEXCLAMATION | MB_OK);
         }
         else
         {
            m_pAviSettings->bIsVideoFormatInitialized = true;

            m_pAviSettings->bUseSmartRecompression = 
               (m_smartRecompressCheck.GetCheck() != BST_UNCHECKED);

            m_pAviSettings->bRememberAspectRatio =
               (m_keepAspectRadio.GetCheck() != BST_UNCHECKED);

            VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER *)
               m_pAviSettings->videoFormat.pbFormat;

            m_pAviSettings->dwFCC = pInfo->fcc;
            // Do we need to change the frame rate?
            double oldFps = 10000000.0 / ((double) m_avgTimePerFrame100ns);
            CString oldFramerate;
            oldFramerate.Format(_T("%0.3f"), oldFps);
            CString newFramerate;
            m_frameRateEdit.GetWindowText(newFramerate);
            if (oldFramerate != newFramerate)
            {
               // Yes, it has been changed
               double newFps = _tcstod(newFramerate, NULL);
               if (newFps <= 0.0)
               {
                  newFps = 25.0;
                  CString msg;
                  msg.Format(IDS_MSG_INVALIDFRAMERATE, newFramerate);
                  CString cap;
                  cap.LoadString(IDS_WARNING_E);
                  MessageBox(msg, cap, MB_ICONEXCLAMATION | MB_OK);
               }

               // Now calculate the corresponding "average
               // time per frame" (in 100ns units)
               LONGLONG timePerFrame = (LONGLONG) (10000000.0 / newFps);
               pVIH->AvgTimePerFrame = timePerFrame;
            }
            else
            {
               // No, we can keep the old frame rate
               pVIH->AvgTimePerFrame = m_avgTimePerFrame100ns;
            }

            m_pAviSettings->keyframeRate = GetKeyframeRate();
            m_pAviSettings->quality = m_qualitySlider.GetPos();
            m_pAviSettings->bUseKeyframes = 
               (m_useKeyframesRadio.GetCheck() != FALSE) ? true : false;

            if (!m_pAviSettings->bUseKeyframes &&
                pInfo->bSupportsKeyframes)
            {
               CString msg;
               msg.LoadString(IDS_MSG_NOKEYFRAMES);
               CString cap;
               cap.LoadString(IDS_WARNING_E);
               MessageBox(msg, cap, MB_ICONEXCLAMATION | MB_OK);
            }

            CString csOffset;
            m_offsetEdit.GetWindowText(csOffset);
            m_pDoc->project.m_videoExportGlobalOffset = _ttoi(csOffset);

            // The document has changed, as the video export format
            // has changed.
            m_pDoc->SetModifiedFlag();

            // Let's rebuild the preview, just in case something
            // important has changed:
            // m_pDoc->RebuildPreview();
            m_pDoc->SetPreviewRebuildNeeded();
         }
      }
      else
      {
         MessageBox(_T("Unknown error."));
      }
   } // m_isEnabled
   
#ifdef _DEBUG
   //CDebugDlg::DisplayAviSettings(this, m_pAviSettings);
#endif
	CPropertyPage::OnOK();
}

void CVideoProps::OnEditChange() 
{
   MySetModified();
}

void CVideoProps::OnUseKeyframesClick() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
   m_keyframesEdit.EnableWindow(m_useKeyframesRadio.GetCheck());

   MySetModified();
}

int CVideoProps::GetKeyframeRate()
{
   CString frameRateText;
   m_keyframesEdit.GetWindowText(frameRateText);
   int frameRate = _ttoi(frameRateText);
   if (frameRate <= 0)
      frameRate = 15;

   return frameRate;
}

void CVideoProps::OnFrameRateLostFocus() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	
   CString frameRateText;
   m_frameRateEdit.GetWindowText(frameRateText);
   double frameRate = _tcstod(frameRateText, NULL);
   if (frameRate <= 0.0)
   {
      frameRate = 25.0;
      m_frameRateEdit.SetWindowText(_T("25.000"));
   }
}

void CVideoProps::OnSmartRecompressClick() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen

   MySetModified();
}

void CVideoProps::OnChangeOffset() 
{
   MySetModified();
}

void CVideoProps::OnOffsetLostFocus() 
{
   CString csOffsetText;
   m_offsetEdit.GetWindowText(csOffsetText);
   int offset = _ttoi(csOffsetText);
   csOffsetText.Format(_T("%d"), offset);
   m_offsetEdit.SetWindowText(csOffsetText);
}

void CVideoProps::MySetModified(BOOL bChanged)
{
   CPropertyPage::SetModified(bChanged);
}

void CVideoProps::OnResetVideoSettings()
{
   if (m_pAviSettings->bHasOriginalSize)
   {
      CString csTmp;
      csTmp.Format(_T("%.3f"), m_pAviSettings->dOriginalFps);
      m_frameRateEdit.SetWindowText(csTmp);
      csTmp.Format(_T("%d"), m_pAviSettings->nOriginalWidth);
      m_sizeXEdit.SetWindowText(csTmp);
      csTmp.Format(_T("%d"), m_pAviSettings->nOriginalHeight);
      m_sizeYEdit.SetWindowText(csTmp);

      PopulateCodecSettings();

      MySetModified(TRUE);
   }
}