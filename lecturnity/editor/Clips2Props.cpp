// Clips2Props.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "Clips2Props.h"
#include "MainFrm.h"
#include "PageProperties.h"
#include "MfcUtils.h" // Localize
#include "misc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CClips2Props 


CClips2Props::CClips2Props(const _TCHAR *tszClipToDisplay)
	: CPropertyPage(CClips2Props::IDD)
{
	//{{AFX_DATA_INIT(CClips2Props)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

   if (tszClipToDisplay)
      m_csClipToDisplay = tszClipToDisplay;
   else
      m_csClipToDisplay = _T("");

   m_csCaption.LoadString(IDS_CLIP1_CAPTION);
   m_psp.pszTitle = m_csCaption;
   m_psp.dwFlags |= PSP_USETITLE;
}

CClips2Props::~CClips2Props()
{
   for (int i=0; i<m_aClipCodecWnds.GetSize(); ++i)
      delete m_aClipCodecWnds[i];
   m_aClipCodecWnds.RemoveAll();
}

void CClips2Props::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClips2Props)
	DDX_Control(pDX, IDC_CLIPCODEC_DUMMY, m_lbPlaceholder);
	DDX_Control(pDX, IDC_CLIP_SOURCE, m_edClip);
	DDX_Control(pDX, IDC_CLIPSELECT, m_cbClip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClips2Props, CPropertyPage)
	//{{AFX_MSG_MAP(CClips2Props)
	ON_CBN_SELCHANGE(IDC_CLIPSELECT, OnSelectClip)
	ON_BN_CLICKED(IDC_CHANGE_CLIP_TITLE, OnChangeClipTitle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CClips2Props 

BOOL CClips2Props::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
   m_pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();

   DWORD dwIds[] = {IDC_CLIP_LABEL,
                    IDC_SOURCE_LABEL,
                    IDC_CHANGE_CLIP_TITLE,
                    -1};
   MfcUtils::Localize(this, dwIds);

   m_cbClip.ResetContent();

   CRect rcCodec;
   m_lbPlaceholder.GetWindowRect(&rcCodec);
   ScreenToClient(&rcCodec);

   int nSelectThisPage = 0;

   CClipStreamR *pClipStream = m_pDoc->project.GetClipStream();
   if (pClipStream != NULL)
   {
      UINT nClipNr = 0;
      CClipInfo clipInfo;
      CClipInfo *pClip = &clipInfo;
      
      HRESULT hr = pClipStream->FillClipInfo(nClipNr, pClip);
      
      while (pClip && SUCCEEDED(hr))
      {
         CAviSettings *pClipSettings = NULL;
         CString       csClipSourceName = pClip->GetFilename();
         if (_tcscmp(csClipSourceName, m_csClipToDisplay) == 0)
            nSelectThisPage = nClipNr;
         BOOL bFound = m_pDoc->project.m_clipAviSettings.Lookup(csClipSourceName, pClipSettings);

         if (!bFound)
         {
            pClipSettings = new CAviSettings();
            m_pDoc->project.m_clipAviSettings.SetAt(csClipSourceName, pClipSettings);
         }

         CString csClipTitle = pClip->GetTitle();
         CString csComboBoxTitle;
         CString csTime;
         Misc::FormatTickString(csTime, pClip->GetTimestamp(), SU_MINUTES, true);
         csComboBoxTitle.Format(_T("%s: %s"), csTime, csClipTitle);
         m_cbClip.AddString(csComboBoxTitle);

         int nClipLengthMs = pClip->GetLength();
         
         CClipCodecWnd *pCcw = new CClipCodecWnd(pClipSettings, csClipSourceName, nClipNr + 1, 
            pClip->GetTimestamp(), pClip->GetTimestamp() + nClipLengthMs, pClip);
         pCcw->Create(IDD_CLIPCODECS, this);
         pCcw->SetWindowPos(NULL, rcCodec.left, rcCodec.top, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER);
         pCcw->ShowWindow(SW_HIDE);
         m_aClipCodecWnds.Add(pCcw);


         // TODO evil hack(s) for something that should be handled in CClipStreamR
         while (pClipStream->BelongsToNextSegment(nClipNr))
            ++nClipNr;
         
         ++nClipNr;
         hr = pClipStream->FillClipInfo(nClipNr, pClip);
         if (hr != S_OK)
            pClip = NULL;
         
      }
   }
   else
   {
      _ASSERT(false);
      // shouldn't happen
   }

   /* TODO remove
   
   CList<CExportClip *, CExportClip *> exportClipList;
   int nListSize = 0;
   m_pDoc->project.FillExportClipList(exportClipList, &nListSize);
   
   POSITION pos = exportClipList.GetHeadPosition();
   int nCount = 1;

   while (pos)
   {
      CExportClip *pClip = exportClipList.GetNext(pos);
      if (pClip)
      {
         CAviSettings *pClipSettings = NULL;
         CString       csClipSourceName = pClip->m_clipInfo->GetFilename();
         if (_tcscmp(csClipSourceName, m_csClipToDisplay) == 0)
            nSelectThisPage = nCount - 1;
         BOOL bFound = m_pDoc->project.m_clipAviSettings.Lookup(csClipSourceName, pClipSettings);

         if (!bFound)
         {
            pClipSettings = new CAviSettings();
            m_pDoc->project.m_clipAviSettings.SetAt(csClipSourceName, pClipSettings);
         }

         CString csClipTitle = pClip->m_clipInfo->GetTitle();
         //pClip->m_clipInfo->SetTitle(_T("Yee haa!"));
         CString csComboBoxTitle;
         CString csTime;
         Misc::FormatTickString(csTime, pClip->m_targetBegin, SU_MINUTES, true);
         csComboBoxTitle.Format(_T("%s: %s"), csTime, csClipTitle);
         m_cbClip.AddString(csComboBoxTitle);

         // How long is the clip right now? We need to calculate where
         // clip ends in the target line.
         int nClipLengthMs = 0;
         POSITION pos = pClip->m_cutList.GetHeadPosition();
         while (pos)
         {
            CTimeSpread *pSpan = pClip->m_cutList.GetNext(pos);
            if (pSpan)
               nClipLengthMs += pSpan->m_iEnd - pSpan->m_iBegin;
         }

         CClipCodecWnd *pCcw = new CClipCodecWnd(pClipSettings, csClipSourceName, nCount, 
            pClip->m_targetBegin, pClip->m_targetBegin + nClipLengthMs, pClip->m_clipInfo);
         pCcw->Create(IDD_CLIPCODECS, this);
         pCcw->SetWindowPos(NULL, rcCodec.left, rcCodec.top, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER);
         pCcw->ShowWindow(SW_HIDE);
         m_aClipCodecWnds.Add(pCcw);

         nCount++;
      }
   }

  */

   m_cbClip.SetCurSel(nSelectThisPage);
   OnSelectClip();

   /*
   pos = exportClipList.GetHeadPosition();
   while (pos)
   {
      CExportClip *pClip = exportClipList.GetNext(pos);
      if (pClip)
         delete pClip;
   }
   */

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CClips2Props::OnSelectClip() 
{
   int nSel = m_cbClip.GetCurSel();
   CClipCodecWnd *pWnd = m_aClipCodecWnds.GetAt(nSel);

   m_edClip.SetWindowText(pWnd->m_csClipSource);
   for (int i=0; i<m_aClipCodecWnds.GetSize(); ++i)
      m_aClipCodecWnds[i]->ShowWindow(SW_HIDE);

   pWnd->ShowWindow(SW_SHOW);
}

void CClips2Props::OnChangeClipTitle() 
{
   
   int nSel = m_cbClip.GetCurSel();
   CClipCodecWnd *pWnd = m_aClipCodecWnds.GetAt(nSel);
   
   int iFromMs = pWnd->GetClipStartMs();
   
   m_pDoc->project.ChangeClipTitleAt(iFromMs);

   /*
   int nSel = m_cbClip.GetCurSel();
   CClipCodecWnd *pWnd = m_aClipCodecWnds.GetAt(nSel);
   
   CArray<CPreviewSegment *, CPreviewSegment *> arPreviewSegments;
   int iFromMs = pWnd->GetClipStartMs();
   int iToMs = pWnd->GetClipEndMs();
   
   m_pDoc->project.GetPreviewSegments(arPreviewSegments, iFromMs, iToMs);
   CString csTitle;
   CStringList slKeywords;
   bool bIsFirstClip = true;
   for (int i = 0; i < arPreviewSegments.GetSize(); ++i)
   {
      CPreviewSegment *pSegment = arPreviewSegments[i];
      if (pSegment)
      {
         CClipStream *pClipStream = pSegment->GetClipStream();
         int iSourceTimestamp = pSegment->TargetTimestampToSource(iFromMs);
         if (iSourceTimestamp < pSegment->GetSourceBegin())
            iSourceTimestamp = pSegment->GetSourceBegin();

         CClipInfo *pClip = pClipStream->FindClipAt(iSourceTimestamp);
         if (pClip)
         {
            if (bIsFirstClip)
            {
               CPageProperties pageProperties(pClip, IDS_PAGE_PROPERTIES);
               pageProperties.DoModal();
               csTitle = pClip->GetTitle();
               pClip->GetKeywords(slKeywords);
               bIsFirstClip = false;
            }
            pClip->SetTitle(csTitle);
            pClip->SetKeywords(slKeywords);
         }
      }
   }
   arPreviewSegments.RemoveAll();

  */
   // Update the combo box
   {
      CString csNewDropdown;
      CString csTime;
      Misc::FormatTickString(csTime, iFromMs, SU_MINUTES, true);
      CClipInfo *pClipInfo = m_pDoc->project.FindClipAt(iFromMs);
      if (pClipInfo)
      {
         csNewDropdown.Format(_T("%s: %s"), csTime, pClipInfo->GetTitle());
         int nCurSel = m_cbClip.GetCurSel();
         m_cbClip.DeleteString(nCurSel);
         m_cbClip.InsertString(nCurSel, csNewDropdown);
         m_cbClip.SetCurSel(nCurSel);
      }
   }

   /*
   slKeywords.RemoveAll();
   m_pDoc->project.UpdateStructureAndStreams();
   m_pDoc->UpdateAllViews(NULL);
   */
}

void CClips2Props::OnOK() 
{
   for (int i=0; i<m_aClipCodecWnds.GetSize(); ++i)
      m_aClipCodecWnds[i]->OnOK();

	CPropertyPage::OnOK();
}

void CClips2Props::ApplyCodecSettingsForAllClips()
{
   int nSel = m_cbClip.GetCurSel();
   CClipCodecWnd *pWnd = m_aClipCodecWnds[nSel];
   for (int i=0; i<m_aClipCodecWnds.GetSize(); ++i)
   {
      if (i != nSel)
         m_aClipCodecWnds[i]->CopyCodecSettings(pWnd);
   }

   SetModified();
}

void CClips2Props::ApplyAudioSettingsForAllClips()
{
   int nSel = m_cbClip.GetCurSel();
   CClipCodecWnd *pWnd = m_aClipCodecWnds[nSel];
   for (int i=0; i<m_aClipCodecWnds.GetSize(); ++i)
   {
      if (i != nSel)
         m_aClipCodecWnds[i]->CopyAudioSettings(pWnd);
   }

   SetModified();
}
