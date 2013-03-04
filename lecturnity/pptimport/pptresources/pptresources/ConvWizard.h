#pragma once
#include "StartDlg.h"
#include "FileSelectDlg.h"


// CConvWizard

class CConvWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CConvWizard)

public:
	CConvWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CConvWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CConvWizard();
	bool m_bIsActive;
	bool m_bProgress;
	void Init(CString wszPPTName, int iSlideCount, int iCurrentSlide, WCHAR *wszSlideSelection, WCHAR *wszLSDName, HWND *pHWnd);

protected:
	DECLARE_MESSAGE_MAP()
private:
	void InitWiz(void);
	CStartDlg		*m_pStartDlg;
	CFileSelectDlg	*m_pFileSelectDlg;
	CString			*m_pcsDialogTitle;
	CEvent			*m_pEvent;
	HWND			*m_pHwnd;
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual BOOL ContinueModal();
	HICON m_hIcon;
};


