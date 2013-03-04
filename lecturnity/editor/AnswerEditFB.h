#if !defined(AFX_ANSWEREDITFB1_H__63F369A4_452F_43EE_85D6_7BAEAF9B5D1F__INCLUDED_)
#define AFX_ANSWEREDITFB1_H__63F369A4_452F_43EE_85D6_7BAEAF9B5D1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnswerEditFB1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnswerEditFB window
#include "EmptyField.h"
#include<vector>
using namespace std;

class CAnswerEditFB : public CEdit
{
// Construction
public:
	CAnswerEditFB();

// Operations
public:

	virtual ~CAnswerEditFB();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnswerEditFB)
	//}}AFX_VIRTUAL

	void AddBlank(bool bUpdate = true);
	void DeleteSelectedBlank();
	bool GetBlankSelState();
   CEmptyField *GetSelectedBlank();
   void GetTextAndBlanks(CStringArray &aArray);
   void GetBlankAnswers(int nStartPos, CStringArray &aAnswers);
   void SetTextAndBlanks(CStringArray &aArray, CArray<CStringArray *, CStringArray *> &aCorrectAnswers);
   void SetBlankAnswers(int nStartPos, CStringArray &aAnswers);


	// Generated message map functions
protected:
	CStatic m_wndRectangle;
	//{{AFX_MSG(CAnswerEditFB)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
   void CalculateBlankRect(CRect &rcBlank, int iPosition);
   bool UpdateCursorPosition(int iCurrentPos,  UINT nDirection);
   void UpdateBlanksPosition(int iCurrentPos, int iOffset);
	void UnselBlanks();
	bool SelectBlank(CPoint &point);
   bool DeleteBlank(int nCurPos);

private:
	CArray<CEmptyField *, CEmptyField *>m_aBlanks;
   UINT m_nTextHeight;

public:
   enum {
      WM_SELECTION_CHANGED = WM_USER + 1,
      BLANK_WIDTH = 12
   };

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANSWEREDITFB1_H__63F369A4_452F_43EE_85D6_7BAEAF9B5D1F__INCLUDED_)
