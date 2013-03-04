#if !defined(AFX_RECORDINGSLIST_H__56249C9D_E66D_4F03_92CD_EDA0298454FC__INCLUDED_)
#define AFX_RECORDINGSLIST_H__56249C9D_E66D_4F03_92CD_EDA0298454FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RecordingsList.h : header file
//

#include "DlgMetadata.h"
/////////////////////////////////////////////////////////////////////////////
// CRecordingsList window

class CRecordingsList : public CXTListBox
{
   // Construction
public:
   CRecordingsList();
   CMapStringToString m_ToolTipMap; 

   // Operations
public:
   void SelectItem(int nItemID);
   int GetSelectedItem();
   void ReplayRecord();
   void ExportRecording();
   void PublishRecord();
   void PublishRecordAdvanced();
   void PublishRecordFormatSelection();
   void EditRecord();
   void RemoveEntry();
   void DeleteRecord();
   void RecordProperties();
   void RenameRecord();
   void ClipStructureInsert();
   void ClipStructureRemove();
   void ClipStructureEdit();
   void ExploreRecordingPath();

private:
    CXTPToolTipContext *m_ctrlToolTipsContext;
    void CreateToolTips();

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CRecordingsList)
   //}}AFX_VIRTUAL

   // Implementation
public:
   virtual ~CRecordingsList();
   virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
   virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

   // Generated message map functions
protected:
   //{{AFX_MSG(CRecordingsList)
   afx_msg void OnPaint();
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg LRESULT OnUpdateList(WPARAM wParam,LPARAM lParam);
   afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()

public:
   enum {
      WM_REMOVE_ENTRY = WM_USER + 1,
      WM_DELETE_ENTRY = WM_USER + 2,
      WM_PROPERTIES = WM_USER + 3
   };
public:
    virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECORDINGSLIST_H__56249C9D_E66D_4F03_92CD_EDA0298454FC__INCLUDED_)
