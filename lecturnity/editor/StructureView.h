#if !defined(AFX_STRUCTUREVIEW_H__84F5E323_6E44_4D3C_8065_CAF5A639C110__INCLUDED_)
#define AFX_STRUCTUREVIEW_H__84F5E323_6E44_4D3C_8065_CAF5A639C110__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StructureView.h : Header-Datei
//
#include "DocumentStructure.h"
#include "lmdfile.h"

//class CDocumentStructure;

/////////////////////////////////////////////////////////////////////////////
// Ansicht CStructureView 
class CTreeDocStruct
{
public:
   CArray<CTreeDocStruct *, CTreeDocStruct *>m_aNode;
   CStructureInformation * m_pSi;
   CTreeDocStruct()
   {
      m_aNode.RemoveAll();
   };
   ~CTreeDocStruct()
   {
      m_pSi = NULL;
   };

};

class CDocStructRecord;

class CStructureView : public CXTPReportView, public CDocumentStructure
{
protected:
	CStructureView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CStructureView)

// Attribute
public:

// Operationen
public:

   // Overrides from CDocumentStructure
   virtual void SetName(void* pData, CString csText);
   virtual CString GetName(void* pData);
   virtual UINT GetType(void *pData);
   virtual void DrawThumbWhiteboard(void* pData, CRect rcThumb, CDC* pDC);
   virtual UINT GetID(void* pData);
   virtual void GetKeywords(void *pData, CString & csKeywords);
   virtual void SetKeywords(void *pData, CString csKeywords);
   virtual void LoadDocument(void *pData);
   virtual bool ShowClickIcon(void *pData);
   // End Overrides from CDocumentStructure

   // Functions from DocumentStructureView
   void SetRecordLayout();
   void SetChildrenRecordLayout(CDocStructRecord* pRecord, int nLayout);
   void UpdateImageList();
   void ChangeAllRootsIcon(UINT nImageIndex);
   void SetSelected(UINT nId);
   void SetSelected(int nTimestamp);
   void RemoveAll();
   void ShowChildWindows(bool bShow);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnReportItemClick(NMHDR * pNotifyStruct, LRESULT * result);
   afx_msg void OnPaint();

   // From Radu
   void RefreshList();
   void AddNewTreeNode(CTreeDocStruct* pChild, CTreeDocStruct* pParent);
   void PopulateRecord(CTreeDocStruct* pNode, CXTPReportRecord *pRecordRoot);

   CImageList m_ilStateImages;
   CArray<CStructureInformation *, CStructureInformation *>m_caStructInfo;
   CRgn m_rgnUpdate;
   int m_iLastActivePage;
   // End Functions from DocumentStructureView


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CStructureView)
protected:
	virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
	virtual void OnInitialUpdate();     // Zum ersten Mal nach der Konstruktion
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CStructureView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

   void DeleteDoc(CTreeDocStruct* pParent);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CStructureView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
   afx_msg void OnStatusBarSwitchView(UINT nID);
   afx_msg void OnUpdateStatusBarSwitchView(CCmdUI* pCmdUI);
   afx_msg void OnShowClickPages();
   afx_msg void OnUpdateShowClickPages(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   HRESULT UpdateThumbnailSize();

private:
   CXTPReportControl m_listElements;

private:
   CBitmap *m_pDrawingBitmap;
   CDC     *m_pBitmapDC;
   CSize   m_siMinimumStructure;
   CSize   m_siCurrentImage;
   int     m_nLastActivePage;
   LOGFONT m_defaultFont;
   CMenu   m_createStructPopup; // create document structure context menu
   CXTPStatusBar m_wndStatusBar;
   BOOL CreateStatusBar();
   bool m_bIsStatusBarCreated;
   int m_nCurrentLayout;
   bool m_bSelectFromOutside;
   
   CArray<int, int>m_caChIndexes;
   int GetParentChapterIndex(CStructureInformation* pElement, int iCurrentRoot);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_STRUCTUREVIEW_H__84F5E323_6E44_4D3C_8065_CAF5A639C110__INCLUDED_
