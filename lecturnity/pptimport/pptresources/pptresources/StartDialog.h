#pragma once
#include "afxwin.h"
#include "resource.h"

#include "ColorStaticWithText.h"

#define WM_CHANGE_TEXT WM_USER+10
// CStartDialog-Dialogfeld

class CStartDialog : public CDialog
{
	DECLARE_DYNAMIC(CStartDialog)

public:
	CStartDialog(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CStartDialog();

// Dialogfelddaten
	enum { IDD = IDD_START_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
   
	afx_msg void OnIdleUpdateUIMessage(); // needed for ON_UPDATE_COMMAND_UI
   afx_msg void CStartDialog::OnUpdateSelectAllSlides(CCmdUI *pCmdUI);
   afx_msg void CStartDialog::OnUpdateSelectCurrentSlides(CCmdUI *pCmdUI);
   afx_msg void CStartDialog::OnUpdateSelectSlideRange(CCmdUI *pCmdUI);
   afx_msg void CStartDialog::OnUpdateButtonNext(CCmdUI *pCmdUI);

   LRESULT OnChangeText(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
public:
   CColorStaticWithText m_titleLabel;
public:
   afx_msg void OnBnClickedCancel();
public:
   HRESULT Init(int iSlideCount, int iCurrentSlide);
   bool m_bIsVisible;
private:
   enum {
      SELECT_ALL_SLIDES = 0,
      SELECT_CURRENT_SLIDE = 1,
      SELECT_SLIDE_RANGE = 2
   };
   int m_iSlideCount;
   int m_iCurrentSlide;
   BOOL m_bSelected[3];
   afx_msg void OnBnClickedSelectAllSlides();
   afx_msg void OnBnClickedSelectCurrentSlide();
   afx_msg void OnBnClickedSelectSlideRange();
   virtual BOOL ContinueModal();
   afx_msg void OnBnClickedNext();
   afx_msg void OnBnClickedPrevious();
   afx_msg void OnStnClickedUsageLabel();
   afx_msg void OnDestroy();
};
