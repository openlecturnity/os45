#if !defined(AFX_AVCSETTINGS_H__C167A64E_E099_4893_B0B0_2496E3E2DC35__INCLUDED_)
#define AFX_AVCSETTINGS_H__C167A64E_E099_4893_B0B0_2496E3E2DC35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AVCSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAVCSettings
#include "TabCtrlEx.h"
#include "AudioSettings.h"
#include "VideoSettings.h"
#include "CodecSettings.h"
//#include "SGSettingsSheet.h"

class CAVCSettings : public CPropertySheet
{
	DECLARE_DYNAMIC(CAVCSettings)

// Construction
public:
	CAVCSettings(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CAVCSettings(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
   CAVCSettings();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAVCSettings)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL ContinueModal();
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void EnableCodecTab(bool bState);
   void UpdateCodecPage();

	virtual ~CAVCSettings();
  
   void Init(CAssistantDoc *pAssistantDoc, int nActiveTab);
   void SaveSettings();


	// Generated message map functions
protected:
	//{{AFX_MSG(CAVCSettings)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CCodecSettings* m_pPageCodec;
	CVideoSettings* m_pPageVideo;
	CAudioSettings* m_pPageAudio;
   //CSGSettingsSheet* m_pPageTest;
   CTabCtrlEx m_tabCtrl;

private:
	int m_nActiveTab;
	bool m_bIsTabCodecEnabled;
	void Init();
   CString* m_csTitleAudio;
   CString* m_csTitleVideo;
   CString* m_csTitleCodec;


   // audio settings

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVCSETTINGS_H__C167A64E_E099_4893_B0B0_2496E3E2DC35__INCLUDED_)
