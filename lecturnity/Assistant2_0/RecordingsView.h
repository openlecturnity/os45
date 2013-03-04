#if !defined(AFX_RECORDINGSVIEW_H__4C6D9511_DF53_4CF7_A5A4_3AF7C1CF06DD__INCLUDED_)
#define AFX_RECORDINGSVIEW_H__4C6D9511_DF53_4CF7_A5A4_3AF7C1CF06DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RecordingsView.h : header file
//

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "RecordingsList.h"
#include "RecordingsToolBar.h"


namespace ASSISTANT {
   class PresentationInfo;
}

class CDrawingToolSettings;

/////////////////////////////////////////////////////////////////////////////
// CRecordingsView view

class CRecordingsView : public CView
{
public:
   enum {
      WM_REMOVE_ENTRY = WM_USER + 1,
      WM_DELETE_ENTRY = WM_USER + 2,
      WM_PROPERTIES = WM_USER + 3
   };

protected:
   CRecordingsView();           // protected constructor used by dynamic creation
   virtual ~CRecordingsView();

   DECLARE_DYNCREATE(CRecordingsView)

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CRecordingsView)
protected:
    //{{AFX_MSG(CRecordingsView)
    // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

protected:
    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
    afx_msg LRESULT OnUpdateList(WPARAM wParam,LPARAM lParam);
    afx_msg void OnDblclkListBox();
    //afx_msg void OnProfileFlash();
    //afx_msg void OnUpdateProfileFlash(CCmdUI* pCmdUI);
    
#ifdef _DVS2005
    afx_msg LRESULT OnRemoveEntry(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDeleteEntry(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnProperties(WPARAM wParam, LPARAM lParam);
#else
    afx_msg void OnRemoveEntry(WPARAM wParam, LPARAM lParam);
    afx_msg void OnDeleteEntry(WPARAM wParam, LPARAM lParam);
    afx_msg void OnProperties(WPARAM wParam, LPARAM lParam);
#endif
    
    //}}AFX_VIRTUAL

public:
   void InsertPresentation(ASSISTANT::PresentationInfo *pPresentation, int nIndex);
   void RemoveAll();
   void SetSelected(UINT nItem);
   void SetAllUnselected();
   BOOL IsRecordSelected();
   void SetRecordSelected(BOOL bSelected); 
   CRecordingsList *GetRecordList();
   void ChangePublishButtonImage(int iProfileID, int iImageID, __int64 lCustomProfileID);
   void FillProfileGallery(CXTPControlGallery* pGallery, 
                           CXTPControlGalleryItems *pItems);
   void ShowProfileSelectionDialog();

#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

private:
   bool CreateList();
   BOOL CreateToolBar();

private:   
   CRecordingsList *m_listRecordings;
   BOOL m_bIsRecordSelected;
   CRecordingsToolBar m_wndToolBar;
   bool m_bIsToolBarCreated;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECORDINGSVIEW_H__4C6D9511_DF53_4CF7_A5A4_3AF7C1CF06DD__INCLUDED_)
