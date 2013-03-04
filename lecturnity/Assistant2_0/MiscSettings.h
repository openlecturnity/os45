#pragma once


// CMiscSettings dialog

class CMiscSettings : public CXTPPropertyPage
{
	DECLARE_DYNAMIC(CMiscSettings)

public:
	CMiscSettings();
	virtual ~CMiscSettings();

// Dialog Data
	enum { IDD = IDD_DIALOG_MISC_SETTINGS };
   CComboBox m_cmbColor;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   afx_msg void OnKickIdle();
   afx_msg void OnSelectHome();
   afx_msg void OnUpdateSelectHome(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
public:
   virtual void OnOK();
protected:
   CXTPPropertyPageStaticCaption m_caption[4];
   void Init();
   void SaveSettings();
   BOOL m_bCheckStartupInit;
   BOOL m_bCheckFontInit;
   BOOL m_bCheckAudioInit;
   BOOL m_bCheckImportInit;
   BOOL m_bCheckSmartBoardPenInit;
   BOOL m_bCheckSwitchByStop;
   CString m_csNewLecturnityHome;
   UINT m_nCurComboSel;
};
