#if !defined(AFX_EXTSPLITTER_H__BB454230_2E7D_4794_BE28_A5020F3B054C__INCLUDED_)
#define AFX_EXTSPLITTER_H__BB454230_2E7D_4794_BE28_A5020F3B054C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtSplitter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExtSplitter frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


class CExtSplitter : public CSplitterWnd
{

public:
	CExtSplitter();           

	virtual ~CExtSplitter();

	void	HideColumn(int colHide);
	void	ShowColumn();

protected:



public:	
		
protected:
	int m_nHidedCol;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtSplitter)
	protected:
	//}}AFX_VIRTUAL


	// Generated message map functions
	//{{AFX_MSG(CExtSplitter)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTSPLITTER_H__BB454230_2E7D_4794_BE28_A5020F3B054C__INCLUDED_)
