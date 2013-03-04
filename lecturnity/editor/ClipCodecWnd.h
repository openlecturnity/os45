#if !defined(AFX_CLIPCODECWND_H__3AED1F83_F61D_4ACF_AE87_F84508D538D0__INCLUDED_)
#define AFX_CLIPCODECWND_H__3AED1F83_F61D_4ACF_AE87_F84508D538D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "videoprops.h"
#include "AVStreams.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CClipCodecWnd 

class CClipCodecWnd : public CVideoProps
{
	DECLARE_DYNCREATE(CClipCodecWnd)

// Konstruktion
public:
	CClipCodecWnd(CAviSettings *pAviSettings=NULL, const _TCHAR *tszClipSource=NULL, int nClipNr=0,
      int nClipStartMs=-1, int nClipEndMs=-1, CClipInfo *pClipInfo=NULL);
   //CClipCodecWnd();
	~CClipCodecWnd();

// Dialogfelddaten
	//{{AFX_DATA(CClipCodecWnd)
	enum { IDD = IDD_CLIPCODECS };
	CButton	m_boxVolume;
	CStatic	m_lbOrigVol;
	CStatic	m_lbClipVol;
	CButton	m_btApplyVolume;
	CSliderCtrl	m_sldOrigVolume;
	CStatic	m_edOrigVolume;
	CStatic	m_edVolume;
	CSliderCtrl	m_sldVolume;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CClipCodecWnd)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CClipCodecWnd)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnApplyCodecForAll();
	afx_msg void OnApplyVolumeForAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   virtual void MySetModified(BOOL bChanged=TRUE);

public:
   virtual void CopyCodecSettings(CClipCodecWnd *pCcw);
   virtual void CopyAudioSettings(CClipCodecWnd *pCcw);
   virtual int GetClipStartMs() { return m_nClipStartMs; }
   virtual int GetClipEndMs() { return m_nClipEndMs; }

protected:
   int m_nClipStartMs;
   int m_nClipEndMs;
   CClipInfo *m_pClipInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CLIPCODECWND_H__3AED1F83_F61D_4ACF_AE87_F84508D538D0__INCLUDED_
