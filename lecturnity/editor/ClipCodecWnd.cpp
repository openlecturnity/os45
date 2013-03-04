#include "stdafx.h"
#include "ClipCodecWnd.h"
#include "AviSettings.h"
#include "Clips2Props.h"
#include "editorDoc.h"
#include "EditorStructures.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CClipCodecWnd 

IMPLEMENT_DYNCREATE(CClipCodecWnd, CVideoProps)

CClipCodecWnd::CClipCodecWnd(CAviSettings *pAviSettings, const _TCHAR *tszClipSource, int nClipNr, 
                             int nClipStartMs, int nClipEndMs, CClipInfo *pClipInfo) : 
   CVideoProps(CClipCodecWnd::IDD, pAviSettings, tszClipSource, nClipNr)
{
	//{{AFX_DATA_INIT(CClipCodecWnd)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   m_nClipStartMs = nClipStartMs;
   m_nClipEndMs = nClipEndMs;
   m_pClipInfo = pClipInfo;
}

CClipCodecWnd::~CClipCodecWnd()
{
}

void CClipCodecWnd::DoDataExchange(CDataExchange* pDX)
{
	CVideoProps::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClipCodecWnd)
	DDX_Control(pDX, IDC_VOLUME_BOX, m_boxVolume);
	DDX_Control(pDX, IDC_DESC_ORIGVOL, m_lbOrigVol);
	DDX_Control(pDX, IDC_DESC_CLIPVOL, m_lbClipVol);
	DDX_Control(pDX, IDC_APPLY_VOLUME_FOR_ALL, m_btApplyVolume);
	DDX_Control(pDX, IDC_ORIG_VOLUME_SLIDER, m_sldOrigVolume);
	DDX_Control(pDX, IDC_ORIG_VOLUME_LABEL, m_edOrigVolume);
	DDX_Control(pDX, IDC_VOLUME_LABEL, m_edVolume);
	DDX_Control(pDX, IDC_VOLUME_SLIDER, m_sldVolume);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClipCodecWnd, CVideoProps)
	//{{AFX_MSG_MAP(CClipCodecWnd)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_APPLY_CODEC_FOR_ALL, OnApplyCodecForAll)
	ON_BN_CLICKED(IDC_APPLY_VOLUME_FOR_ALL, OnApplyVolumeForAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CClipCodecWnd 

BOOL CClipCodecWnd::OnInitDialog() 
{
	CVideoProps::OnInitDialog();

   // There are still some dlg items which need to be localized
   DWORD dwIds[] = {IDC_CLIPSIZE_BOX,
                    IDC_APPLY_CODEC_FOR_ALL,
                    IDC_VOLUME_BOX,
                    IDC_APPLY_VOLUME_FOR_ALL,
                    IDC_DESC_CLIPVOL,
                    IDC_DESC_ORIGVOL,
                    -1};
   MfcUtils::Localize(this, dwIds);

   m_sldVolume.SetRange(0, 200);
   m_sldVolume.SetPos(m_pAviSettings->nAudioVolume);
   CString csTmp;
   csTmp.Format(_T("%d%%"), m_pAviSettings->nAudioVolume);
   m_edVolume.SetWindowText(csTmp);
   m_sldOrigVolume.SetRange(0, 100);
   m_sldOrigVolume.SetPos(m_pAviSettings->nOriginalAudioVolume);
   csTmp.Format(_T("%d%%"), m_pAviSettings->nOriginalAudioVolume);
   m_edOrigVolume.SetWindowText(csTmp);

   if (m_pClipInfo)
   {
      if (!m_pClipInfo->HasAudioStream())
      {
         m_sldVolume.EnableWindow(FALSE);
         m_edVolume.EnableWindow(FALSE);
         m_sldOrigVolume.EnableWindow(FALSE);
         m_edOrigVolume.EnableWindow(FALSE);
         m_btApplyVolume.EnableWindow(FALSE);
         m_lbClipVol.EnableWindow(FALSE);
         m_lbOrigVol.EnableWindow(FALSE);
         m_boxVolume.EnableWindow(FALSE);
      }
      else
      {
         // We have audio in this clip. But do we
         // have underlying audio?

         bool bHasRealAudio = false;
         // Iterate through all preview segments...
         CArray<CPreviewSegment *, CPreviewSegment *> *pPreviewSegs = &m_pDoc->project.m_previewList;
         for (int i=0; i<pPreviewSegs->GetSize() && !bHasRealAudio; ++i)
         {
            // Does this preview segment overlap the clip's time span?
            CPreviewSegment *pSeg = pPreviewSegs->GetAt(i);
            if (!(m_nClipEndMs <= pSeg->GetTargetBegin() || m_nClipStartMs >= pSeg->GetTargetEnd()))
            {
               // Overlaps.
               CAudioStream *pAudio = pSeg->GetAudioStream();
               if (pAudio)
               {
                  const _TCHAR *tszAudioSource = pAudio->GetFilename();
                  if (tszAudioSource)
                  {
                     // Dummy audio sources have file name ""
                     if (_tcscmp(tszAudioSource, _T("")) != 0)
                        bHasRealAudio = true;
                  }
               }
            }
         }

         if (!bHasRealAudio)
         {
            m_sldOrigVolume.EnableWindow(FALSE);
            m_edOrigVolume.EnableWindow(FALSE);
            m_lbOrigVol.EnableWindow(FALSE);
         }
      }
   }

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CClipCodecWnd::MySetModified(BOOL bChanged)
{
   CVideoProps::SetModified(bChanged);
   ((CPropertyPage *) GetParent())->SetModified(bChanged);
}

void CClipCodecWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   if (((CSliderCtrl *) pScrollBar) == &m_sldVolume)
   {
      int pos = m_sldVolume.GetPos();
      CString temp;
      temp.Format(_T("%d%%"), pos);
      m_edVolume.SetWindowText(temp);
      MySetModified();
   }
   else if (((CSliderCtrl *) pScrollBar) == &m_sldOrigVolume)
   {
      int pos = m_sldOrigVolume.GetPos();
      CString temp;
      temp.Format(_T("%d%%"), pos);
      m_edOrigVolume.SetWindowText(temp);
      MySetModified();
   }
	
	CVideoProps::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CClipCodecWnd::OnApplyCodecForAll() 
{
   CString csMsg;
   csMsg.LoadString(IDS_MSG_APPLY_CODEC_FOR_ALL);
   CString csCap;
   csCap.LoadString(IDS_QUESTION);
   int nRes = MessageBox(csMsg, csCap, MB_YESNO | MB_ICONQUESTION);

   if (nRes == IDYES)
   {
      ((CClips2Props *) GetParent())->ApplyCodecSettingsForAllClips();
   }
}

void CClipCodecWnd::OnApplyVolumeForAll() 
{
   CString csMsg;
   csMsg.LoadString(IDS_MSG_APPLY_VOLUME_FOR_ALL);
   CString csCap;
   csCap.LoadString(IDS_QUESTION);
   int nRes = MessageBox(csMsg, csCap, MB_YESNO | MB_ICONQUESTION);

   if (nRes == IDYES)
   {
      ((CClips2Props *) GetParent())->ApplyAudioSettingsForAllClips();
   }
}

void CClipCodecWnd::OnOK() 
{
	CVideoProps::OnOK();

   int nVolume = m_sldVolume.GetPos();
   m_pAviSettings->nAudioVolume = nVolume;
   int nOrigVolume = m_sldOrigVolume.GetPos();
   m_pAviSettings->nOriginalAudioVolume = nOrigVolume;

   // Reset and recreate the video preview format in case something
   // has changed.
   if (!m_pDoc->project.HasVideo())
      m_pDoc->project.ResetPreviewFormat();
}

void CClipCodecWnd::CopyCodecSettings(CClipCodecWnd *pCcw)
{
   int nOtherSel = pCcw->m_codecCombo.GetCurSel();
   int dwOtherFcc = pCcw->m_plCodecList[nOtherSel]->fcc;
   int nThisSel = FindCodec(dwOtherFcc);
   if (nThisSel == -1)
   {
      // No good, the codec is not valid for this clip
      CString csMsg;
      csMsg.Format(IDS_MSG_CLIP_CODEC_NOT_APPLICABLE, m_csClipSource);
      CString csCap;
      csCap.LoadString(IDS_WARNING_E);
      MessageBox(csMsg, csCap, MB_OK | MB_ICONWARNING);
      return;
   }

   m_codecCombo.SetCurSel(nThisSel);
   m_qualitySlider.SetPos(pCcw->m_qualitySlider.GetPos());
   CString csTmp;
   csTmp.Format(_T("%d%%"), m_qualitySlider.GetPos());
   m_qualityLabel.SetWindowText(csTmp);
   m_useKeyframesRadio.SetCheck(pCcw->m_useKeyframesRadio.GetCheck());
   pCcw->m_keyframesEdit.GetWindowText(csTmp);
   m_keyframesEdit.SetWindowText(csTmp);
   m_smartRecompressCheck.SetCheck(pCcw->m_smartRecompressCheck.GetCheck());

   UpdateCodecControls();

   MySetModified();
}

void CClipCodecWnd::CopyAudioSettings(CClipCodecWnd *pCcw)
{
   if (NULL == pCcw)
      return;

   int nNewVol = pCcw->m_sldVolume.GetPos();
   m_sldVolume.SetPos(nNewVol);
   CString csTmp;
   csTmp.Format(_T("%d%%"), nNewVol);
   m_edVolume.SetWindowText(csTmp);

   nNewVol = pCcw->m_sldOrigVolume.GetPos();
   m_sldOrigVolume.SetPos(nNewVol);
   csTmp.Format(_T("%d%%"), nNewVol);
   m_edOrigVolume.SetWindowText(csTmp);

   MySetModified();
}
