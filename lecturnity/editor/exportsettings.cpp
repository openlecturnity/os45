#include "stdafx.h"
#include "exportsettings.h"
#include "MainFrm.h"
#include "MfcUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportSettings

IMPLEMENT_DYNAMIC(CExportSettings, CPropertySheet)

CExportSettings::CExportSettings(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage, const _TCHAR *tszClipFile)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   if (tszClipFile)
      m_csClipCodecsToDisplay = tszClipFile;
   else
      m_csClipCodecsToDisplay = _T("");
   Init();
}

CExportSettings::CExportSettings(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage, const _TCHAR *tszClipFile)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   if (tszClipFile)
      m_csClipCodecsToDisplay = tszClipFile;
   else
      m_csClipCodecsToDisplay = _T("");
   Init();
}

CExportSettings::~CExportSettings()
{
   if (m_pAudioProps)
      delete m_pAudioProps;
   if (m_pVideoProps)
      delete m_pVideoProps;
   if (m_pMetadataProps)
      delete m_pMetadataProps;
   if (m_pClipsProps)
      delete m_pClipsProps;
   if (m_pClips2Props)
      delete m_pClips2Props;
}

void CExportSettings::Init()
{
   CEditorDoc *pDoc = (CEditorDoc *)CMainFrameE::GetEditorDocument();
   if (pDoc->project.HasVideo())
      m_pVideoProps = new CVideoProps(IDD_VIDEO, &pDoc->project.m_videoAviSettings);
   else
      m_pVideoProps = NULL;
   m_pAudioProps = new CAudioProps();
   m_pMetadataProps = new CMetadataProps();
   if (pDoc->project.HasSgClips())
   {
      if (!pDoc->project.IsDenverDocument())
      {
         m_pClipsProps = NULL;
         m_pClips2Props = new CClips2Props(m_csClipCodecsToDisplay);
      }
      else
      {
         m_pClipsProps = new CDenverCodecPage(IDD_DENVERCODEC, &pDoc->project.m_videoAviSettings);
         m_pClips2Props = NULL;
      }
   }
   else
   {
      m_pClipsProps = NULL;
      m_pClips2Props = NULL;
   }
   bool videoFirst = true;
   if (!m_pVideoProps ||
       (m_pVideoProps && pDoc->project.m_videoAviSettings.bIsVideoFormatInitialized &&
        !pDoc->project.m_isAudioFormatInitialized))
      videoFirst = false;

   if (videoFirst)
   {
      if (m_pVideoProps)
         AddPage(m_pVideoProps);
      AddPage(m_pAudioProps);
   }
   else
   {
      AddPage(m_pAudioProps);
      if (m_pVideoProps)
         AddPage(m_pVideoProps);
   }

   AddPage(m_pMetadataProps);

   if (m_pClips2Props)
      AddPage(m_pClips2Props);

   if (m_pClipsProps)
      AddPage(m_pClipsProps);
}

BEGIN_MESSAGE_MAP(CExportSettings, CPropertySheet)
	//{{AFX_MSG_MAP(CExportSettings)
		// HINWEIS - Der Klassen-Assistent fügt hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CExportSettings 

BOOL CExportSettings::OnInitDialog() 
{
   BOOL bResult = CPropertySheet::OnInitDialog();

   DWORD dwIds[] = {IDOK,
                    IDCANCEL,
                    -1};

   MfcUtils::Localize(this, dwIds);

   CWnd *pApply = GetDlgItem(ID_APPLY_NOW);
   if(pApply != NULL)
      pApply->DestroyWindow();
   
   CWnd *pHelp = GetDlgItem(IDHELP);
   if(pHelp != NULL)
      pHelp->DestroyWindow();
   
   CRect rectBtn;
   int nSpacing = 6;        // space between two buttons...

   int ids [] = {IDOK, IDCANCEL};
   for( int i =0; i < sizeof(ids)/sizeof(int); i++)
   {
      GetDlgItem (ids [i])->GetWindowRect (rectBtn);
      
      ScreenToClient (&rectBtn);
      int btnWidth = rectBtn.Width();
      rectBtn.left = rectBtn.left + (btnWidth + nSpacing)* 2;
      rectBtn.right = rectBtn.right + (btnWidth + nSpacing)* 2;
      
      GetDlgItem (ids [i])->MoveWindow(rectBtn);
   }


   return bResult;
}
