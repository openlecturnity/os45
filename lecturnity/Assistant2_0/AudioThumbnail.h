#if !defined(AFX_AUDIOTHUMBNAIL_H__ABC75E84_FF21_4E68_BB27_93F94776B477__INCLUDED_)
#define AFX_AUDIOTHUMBNAIL_H__ABC75E84_FF21_4E68_BB27_93F94776B477__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AudioThumbnail.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAudioThumbnail window

class CAudioThumbnail : public CStatic
{
// Construction
public:
	CAudioThumbnail(UINT nOnPressMessage);

// Attributes
public:

// Operations
public:


// Implementation
public:
	virtual ~CAudioThumbnail();
   void SetBitmaps(
      UINT nBmp0,UINT nBmp1, UINT nBmp2, UINT nBmp3, UINT nBmp4, UINT nBmp5,
      UINT nBmp6, UINT nBmp7, UINT nBmp8, UINT nBmp9, UINT nBmp10,
      UINT nBmp11, UINT nBmp12, UINT nBmp13, UINT nBmp14, UINT nBmp15,
      UINT nBmp16, UINT nBmp17, UINT nBmp18, UINT nBmp19, UINT nBmp20,
      UINT nBmp21, UINT nBmp22, UINT nBmp23, UINT nBmp24, UINT nBmp25,
      UINT nBmp26, UINT nBmp27, UINT nBmp28, UINT nBmp29, UINT nBmp30,
      UINT nBmp31, UINT nBmp32, UINT nBmp33, UINT nBmp34, UINT nBmp35,
      UINT nBmp36, UINT nBmp37, UINT nBmp38, UINT nBmp39
      );
   void SetPosition(float fPosition = 0.0);
   void SetToolTipText(CString spText, BOOL bActivate = TRUE);
	void SetToolTipText(UINT nId, BOOL bActivate = TRUE);
   void DeleteToolTip();
   void SetMonitorGroupElement();
	// Generated message map functions
protected:
	//{{AFX_MSG(CAudioThumbnail)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	//}}AFX_MSG
  
   // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioThumbnail)
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	DECLARE_MESSAGE_MAP()
private:
   CArray<HBITMAP,HBITMAP> m_AudioBitmapsArray;
   CToolTipCtrl* m_ToolTip;
   CString m_csToolText;
   UINT m_WMOnPressButton;
   BOOL m_bTracking;
   BOOL m_bMonitorElement;

private:
   void InitToolTip();
   void ActivateTooltip(BOOL bActivate = TRUE);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOTHUMBNAIL_H__ABC75E84_FF21_4E68_BB27_93F94776B477__INCLUDED_)
