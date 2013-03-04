#pragma once
#include "afxwin.h"


// CStartDlg dialog

class CStartDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CStartDlg)

public:
	CStartDlg();
	virtual ~CStartDlg();

// Dialog Data
	enum { IDD = IDD_START_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	enum {
      SELECT_ALL_SLIDES = 0,
      SELECT_CURRENT_SLIDE = 1,
      SELECT_SLIDE_RANGE = 2
   };
	void InitWiz(void);
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	bool aSelected[3];
	WCHAR *m_pSlideSelection;
	int m_iSlideCount;
	int m_iCurrentSlide;
	CButton m_rbtAll;
public:
	void Init(int iSlideCount, int iCurrentSlide, WCHAR *wszSlideSelection);
	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedRadioCurrent();
	afx_msg void OnBnClickedRadioSlides();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual LRESULT OnWizardNext();
private:
	CEdit m_edtRange;
	bool VerifyRange(CString csRange);
	
	afx_msg void OnEnUpdateSlideSelection();
};
