#if !defined(AFX_SGSETTINGS_H__2B71ADE0_E78B_4AFC_81BB_AF3BD8778D32__INCLUDED_)
#define AFX_SGSETTINGS_H__2B71ADE0_E78B_4AFC_81BB_AF3BD8778D32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SGSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSGSettings
#include "SGSettingsDlg.h"
#include "SGHandlingDlg.h"
#include "SGHotkeysDlg.h"
#include "SGCodecDlg.h"

class CSGSettings : public CPropertySheet
{
	DECLARE_DYNAMIC(CSGSettings)

// Construction
public:
	CSGSettings(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSGSettings(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
   CSGSettings();

public:
   void Init(CAssistantDoc *pAssistantDoc);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSGSettings)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL ContinueModal();
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
   CSGCodecDlg*    m_pPageCodec;
   CSGHotkeysDlg*  m_pPageHotkeys;
   CSGHandlingDlg* m_pPageHandling;
   CSGSettingsDlg* m_pPageSettings;
	virtual ~CSGSettings();
   void SaveSettings();
// virtual BOOL Create(CWnd* pParentWnd = NULL, DWORD dwStyle = (DWORD)-1, DWORD dwExStyle = 0);
	// Generated message map functions
protected:
	//{{AFX_MSG(CSGSettings)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Init();
   CString* m_csTitleSettings;
   CString* m_csTitleHandling;
   CString* m_csTitleHotkeys;
   CString* m_csTitleCodec;

  
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SGSETTINGS_H__2B71ADE0_E78B_4AFC_81BB_AF3BD8778D32__INCLUDED_)
