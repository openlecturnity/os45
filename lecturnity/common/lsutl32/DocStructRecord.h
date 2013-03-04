#if !defined(AFX_CDOCSTRUCTRECORD_H__E0C1EF0A_0A4F_4474_8AF6_567139936FD6__INCLUDED_)
#define AFX_CDOCSTRUCTRECORD_H__E0C1EF0A_0A4F_4474_8AF6_567139936FD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DocStructRecord.h : header file
//

// Why is it needed to specify this in every header??
#ifdef _LSUTL32_DLL
    #define LSUTL32_EXPORT __declspec(dllexport)
#else 
    #ifdef _LSUTL32_STATIC
        #define LSUTL32_EXPORT 
    #else
        #define LSUTL32_EXPORT __declspec(dllimport)
    #endif
#endif

#include "reportcontrol\xtpreportrecord.h"

class CDocumentStructure;

class LSUTL32_EXPORT CDocStructRecordItem : public CXTPReportRecordItem {
public:
    CDocStructRecordItem(CString csCaption, UINT nType, bool bEditOnDblClick = true);
    ~CDocStructRecordItem();
    virtual void DoPropExchange(CXTPPropExchange* pPX);
    virtual void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);
    //-----------------------------------------------------------------------
    // Summary:
    //     Overrides CXTPReportRecordItem::Draw.
    //     Extra functionality added:
    //          - Draw image for each item on left side (except if Thumbnail view and item is Page).
    //          - If parent record is not visible, draw blue rectangle over it's subitems (but not root)
    //          - In Thumbnail view, increase item's image rectangle of an expanded record to surround all subitems.
    //          - Modify caption or edit control position in thumbnail view if item is Page.
    // Parameters:
    //     pDrawArgs - structure which contains drawing arguments:
    // Remarks:
    //     Call this member function to draw an item. Actually this function
    //     only prepares and calls needed drawing functions from PaintManager.
    //     Thus if you wish to change the look of your report item, you must just provide
    //     your own implementation of PaintManager
    //-----------------------------------------------------------------------
    virtual int Draw(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs);
    virtual void OnBeginEdit(XTP_REPORTRECORDITEM_ARGS* pItemArgs);
    virtual void OnDrawCaption(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pMetrics);
    virtual void OnDblClick(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs);

    int GetReportAreaRows(CXTPReportControl *pReportControl, int nStartRow, BOOL bMoveDown);

    UINT m_nImageIndex;  // the index of the image that will be displayed in front of the report record. The images are loaded from the report control imagelist.

protected:
    CString m_csInitialCaption;
    bool m_bEditOnDblClick;
    int GetExpandedChildsHeight(CXTPReportControl *pReportControl, CXTPReportRecord *pRecord);
};

class LSUTL32_EXPORT CDocStructRecord : public CXTPReportRecord {
public:
    CDocStructRecord(CDocumentStructure* pContainer, void* pvData, bool bIsExpanded, int nCurrentLayout, UINT nImageIndex, bool bEdtiOnDblClick = true);
    ~CDocStructRecord(void);

    virtual void DoPropExchange(CXTPPropExchange* pPX);
    void UpdateTooltip();
    bool IsPage();
    bool IsChapter();
    bool IsDocument();
    bool IsLoadedDocument();
    bool IsEmptyDocument();

    // Helper class that extracts information from m_pvData for each record
    CDocumentStructure* m_pContainer;
    // Items Data. Hold information like caption, type, id, keywords
    void *m_pvData;
    bool m_bIsExpanded;
    int m_nCurrentLayout;
    CDocStructRecordItem *m_pItemText;
    static int s_PAGE;
    static int s_CHAPTER;
    static int s_DOCUMENT;
};

class LSUTL32_EXPORT CXTPDocumentStructurePaintManager : public CXTPReportPaintManager {
public:
    CXTPDocumentStructurePaintManager();
    virtual ~CXTPDocumentStructurePaintManager();

    virtual int GetRowHeight(CDC* pDC, CXTPReportRow* pRow);
    virtual CSize DrawCollapsedBitmap(CDC* pDC, const CXTPReportRow* pRow, CRect& rcBitmap);
    virtual CSize DrawCustomBitmap(CDC* pDC, const CXTPReportRow* pRow, CRect& rcBitmap, UINT nImageIndex);

    DECLARE_DYNAMIC(CXTPDocumentStructurePaintManager);
};

#endif //AFX_CDOCSTRUCTRECORD_H__E0C1EF0A_0A4F_4474_8AF6_567139936FD6__INCLUDED_