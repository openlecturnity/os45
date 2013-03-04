#if !defined(AFX_DENVERCODECPAGE_H__31C3B5F1_078A_4665_AB42_E8DA1A7B0401__INCLUDED_)
#define AFX_DENVERCODECPAGE_H__31C3B5F1_078A_4665_AB42_E8DA1A7B0401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DenverCodecPage.h : Header-Datei
//

#include "VideoProps.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDenverCodecPage 

class CDenverCodecPage : public CVideoProps
{
	DECLARE_DYNCREATE(CDenverCodecPage)

// Konstruktion
public:
	CDenverCodecPage(DWORD dwId=IDD, CAviSettings *pAviSettings=NULL);
	~CDenverCodecPage();

// Dialogfelddaten
	//{{AFX_DATA(CDenverCodecPage)
	enum { IDD = IDD_DENVERCODEC };
	CButton	m_cbbStretchSmallClips;
	CButton	m_cbbAutoVideoSize;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDenverCodecPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
   virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDenverCodecPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAutoVideosizeClick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
   void EnableVideoSizeGui(bool bEnable);

   CString m_csCaption;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DENVERCODECPAGE_H__31C3B5F1_078A_4665_AB42_E8DA1A7B0401__INCLUDED_
