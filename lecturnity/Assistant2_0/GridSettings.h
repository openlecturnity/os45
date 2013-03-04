#pragma once
#include "afxwin.h"
class CAssistantDoc;


// CGridSettings dialog

class CGridSettings : public CDialog
{
	DECLARE_DYNAMIC(CGridSettings)

public:
	CGridSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGridSettings();

// Dialog Data
	enum { IDD = IDD_GRID_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:	
	afx_msg void OnBnClickedCheckSnap();
	afx_msg void OnCbnCloseupComboSpacing();
	afx_msg void OnBnClickedCheckDisplayGrid();
	afx_msg void OnOk();

public:
	 void Init(CAssistantDoc *pAssistantDoc);

private:
	CButton m_checkSnap;
	CComboBox m_comboSpacing;
	CButton m_checkDisplay;
	UINT m_nSnap;
	UINT m_nSpacing;
	UINT m_nDisplay;
    CAssistantDoc *m_pAssistantDoc;
	
};
