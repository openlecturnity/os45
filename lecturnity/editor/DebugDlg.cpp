// DebugDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "DebugDlg.h"

#include "Dvdmedia.h" // DirectShow

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDebugDlg 

#include <dshow.h>


CDebugDlg::CDebugDlg(LPCTSTR tszMessage, CWnd* pParent /*=NULL*/)
	: CDialog(CDebugDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDebugDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
   m_csDebugMessage = tszMessage;
}


void CDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDebugDlg)
	DDX_Control(pDX, IDC_DEBUGINFO, m_edDebug);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDebugDlg, CDialog)
	//{{AFX_MSG_MAP(CDebugDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDebugDlg::CreateMediaTypeInfo(CString &csTarget, AM_MEDIA_TYPE *pMt)
{
   CString csMt;
   CString csTmp;
   if (pMt->majortype == MEDIATYPE_Video)
   {
      csMt = _T("MEDIATYPE_Video\r\n");
      csTmp.Format(_T("bFixedSizeSamples: %d\r\n"), pMt->bFixedSizeSamples);
      csMt += csTmp;
      csTmp.Format(_T("bTemporalCompression: %d\r\n"), pMt->bTemporalCompression);
      csMt += csTmp;
      csTmp.Format(_T("lSampleSize: %d\r\n"), pMt->lSampleSize);
      csMt += csTmp;
      if (pMt->formattype == FORMAT_VideoInfo)
      {
         csMt += _T("formattype: FORMAT_VideoInfo\r\n");
         VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *) pMt->pbFormat;
         csTmp.Format(_T("Source: (%d, %d) - (%d, %d)\r\n"), pVih->rcSource.left, pVih->rcSource.top,
            pVih->rcSource.right, pVih->rcSource.bottom);
         csMt += csTmp;
         csTmp.Format(_T("Target: (%d, %d) - (%d, %d)\r\n"), pVih->rcTarget.left, pVih->rcTarget.top,
            pVih->rcTarget.right, pVih->rcTarget.bottom);
         csMt += csTmp;
         csTmp.Format(_T("dwBitRate: %d\r\n"), pVih->dwBitRate);
         csMt += csTmp;
         csTmp.Format(_T("AvgTimePerFrame (us): %d\r\n"), (int) (pVih->AvgTimePerFrame / 10i64));
         csMt += csTmp;
         csMt += _T("\r\nBITMAPINFOHEADER:\r\n");
         BITMAPINFOHEADER *pBih = &pVih->bmiHeader;
         csTmp.Format(_T("biWidth: %d\r\n"), pBih->biWidth);
         csMt += csTmp;
         csTmp.Format(_T("biHeight: %d\r\n"), pBih->biHeight);
         csMt += csTmp;
         csTmp.Format(_T("biPlanes: %d\r\n"), pBih->biPlanes);
         csMt += csTmp;
         csTmp.Format(_T("biBitCount: %d\r\n"), pBih->biBitCount);
         csMt += csTmp;
         csTmp.Format(_T("biCompression: %d\r\n"), pBih->biCompression);
         csMt += csTmp;
         csTmp.Format(_T("biSizeImage: %d\r\n"), pBih->biSizeImage);
         csMt += csTmp;
      }
      else if (pMt->formattype == FORMAT_VideoInfo2)
      {
         csMt += _T("formattype: FORMAT_VideoInfo2\r\n");
         VIDEOINFOHEADER2 *pVih = (VIDEOINFOHEADER2 *) pMt->pbFormat;
      }
   }
   else if (pMt->majortype == MEDIATYPE_Audio)
   {
      csMt = _T("MEDIATYPE_Audio\r\n");
   }

   csTarget = csMt;
}

void CDebugDlg::DisplayAviSettings(CWnd *pParent, CAviSettings *pAviSettings)
{
   CString csMsg = _T("AVI Settings\r\n");
   CString csTmp;
   csTmp.Format(_T("Initialized: %d\r\n"), pAviSettings->bIsVideoFormatInitialized);
   csMsg += csTmp;
   csTmp.Format(_T("FCC: %d\r\n"), pAviSettings->dwFCC);
   csMsg += csTmp;
   csTmp.Format(_T("Quality: %d\r\n"), pAviSettings->quality);
   csMsg += csTmp;
   csTmp.Format(_T("Keyframe rate: %d\r\n"), pAviSettings->keyframeRate);
   csMsg += csTmp;
   csTmp.Format(_T("Use Key frames: %d\r\n"), pAviSettings->bUseKeyframes);
   csMsg += csTmp;
   csTmp.Format(_T("Use smart recompression: %d\r\n\r\n"), pAviSettings->bUseSmartRecompression);
   csMsg += csTmp;

   if (pAviSettings->bIsVideoFormatInitialized)
   {
      CString csMt;
      CreateMediaTypeInfo(csMt, &pAviSettings->videoFormat);
      csMsg += csMt;
   }
   
   CDebugDlg debugDlg(csMsg, pParent);

   debugDlg.DoModal();
}

void CDebugDlg::DisplayMediaType(CWnd *pParent, AM_MEDIA_TYPE *pMt)
{
   CString csMt;
   if (pMt != NULL)
      CreateMediaTypeInfo(csMt, pMt);

   CDebugDlg debugDlg(csMt, pParent);
   debugDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDebugDlg 

BOOL CDebugDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

   m_edDebug.SetWindowText(m_csDebugMessage);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
