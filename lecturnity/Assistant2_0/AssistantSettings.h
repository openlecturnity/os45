#pragma once

#include "MiscSettings.h"
#include "CustomizeSheet.h"
#include "AVSettingsSheet.h"
#include "SGSettingsSheet.h"

// CAssistantSettings

class CAssistantSettings : public CXTPPropertySheet/*CPropertySheet*/
{
	DECLARE_DYNAMIC(CAssistantSettings)

public:
	CAssistantSettings(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CAssistantSettings(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
   CAssistantSettings(LPCTSTR pszCaption, CXTPCommandBars* pCommandBars, CWnd* pParentWnd = NULL, UINT iSelectPage = 0, bool bCustom = false, bool bLiveContext=false);
	virtual ~CAssistantSettings();

protected:
	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
public:
   virtual BOOL ContinueModal();
protected:
   CMiscSettings*    m_pPageMisc;
   CCustomizeSheet*  m_pPageCustomize;
   CAVSettingsSheet* m_pPageAudioVideo;
   CSGSettingsSheet* m_pPageScreengrabbing;
  /* CTabCtrlEx        m_tabCtrl;*/

   CString* m_pcsTitleMisc;
   CString* m_pcsTitleCustomize;
   CString* m_pcsTitleAudioVideo;
   CString* m_pcsTitleScreengrabbing;
   int GetMaxStringWidth();
   UINT m_nSelectedPage;

   CAssistantDoc* m_pDocument;
   void Init(CXTPCommandBars* pCommandBars = NULL, UINT nSelectedPage = 0, bool bCustom = false, bool bLiveContext=false);
public:
   void Init(CAssistantDoc *pAssistantDoc);
};


