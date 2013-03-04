#pragma once

#include "export.h"
#include "FileOutput.h"
#include "LmdFile.h"

#define IMAGE_SCREENSHOT 0
#define IMAGE_CLICK_VISUALIZATION 1
#define IMAGE_CLICK_AREA_SNAPSHOT 2

#define SLIDE_HEIGHT 847
#define SLIDE_WIDTH 450
#define HEADER_HEIGHT 91
#define FOOTER_HEIGHT 91
#define CHAPTER_TEXT_HEIGHT 34
#define CALLOUT_TEXT_HEIGHT 20
#define SNAPSHOT_HEIGHT 35
class CManualConfiguration;

class FILESDK_EXPORT CWordSlideObject
{
public:
    CWordSlideObject(void);
    ~CWordSlideObject(void);

public:
    void AddImage(DrawSdk::Image *pImage);
    void AddText(DrawSdk::Text *pText);
    DrawSdk::Image *GetImage() {return m_pImage;}
    void GetTexts(CArray<DrawSdk::Text  *, DrawSdk::Text  *> &aText) {aText.Append(m_aText);}
    void SetChapterTitle(CString csTitle) {m_csChapterTitle = csTitle;}
    CString GetChapterTitle() {return m_csChapterTitle;}
    void SetClickObjectBounds(CRect rcClickBounds) {m_rcClickBounds = rcClickBounds;}
    CRect GetClickObjectBounds() {return m_rcClickBounds;}
    void SetClickPoint(CPoint ptClick) {m_ptClick = ptClick;}
    CPoint GetClickPoint() {return m_ptClick;}
    

private:
    DrawSdk::Image *m_pImage;
    CArray<DrawSdk::Text  *, DrawSdk::Text  *> m_aText; 
    CString m_csChapterTitle;
    CRect m_rcClickBounds;
    CPoint m_ptClick;
};

class FILESDK_EXPORT CWordXml
{
public:
    CWordXml(CManualConfiguration *pManualConfiguration, LPCTSTR tszDocumentTitle);
    ~CWordXml(void);

public: 
    HRESULT Write(CString csFilename, CMetaInformation &metainfo, CArray<CWordSlideObject *, CWordSlideObject *> &aPage);

private:
    LPBYTE GetDataFromResource(int iTextResourceID, DWORD &dwSize);
    HRESULT GetCStringFromResource(int iTextResourceID, CString &csResource);
    HRESULT Replace(CString &csText, LPCTSTR tszTemplateVar, LPCTSTR tszReplaceText);
    HRESULT Write(CFileOutput &wordOutput, int iTextResourceID);
    HRESULT WriteProperties(CFileOutput &wordOutput, CMetaInformation &metainfo);
    /**
    * Writes Cover sheet, Table of content, all click information, page header and footer.
    */
    HRESULT WriteDocumentContent(CFileOutput &wordOutput, CArray<CWordSlideObject *, CWordSlideObject *> &aSlide, CMetaInformation &metainfo);
    HRESULT CWordXml::GenerateImageData(Gdiplus::Bitmap *bmpImage, CFileOutput &wordOutput, bool bFromFile, CString csImageName);
    HRESULT CWordXml::GenerateImages(CFileOutput &wordOutput, int &iImageId, int &iShapeId, DrawSdk::Image *pImage, int iImageType, CRect rcClickBounds, CString csClickNumber = _T(""), CString csCalloutText = _T(""), CPoint ptClick = CPoint(0,0));
    HRESULT WriteTableOfContent(CFileOutput &wordOutput, CArray<CWordSlideObject *, CWordSlideObject *> &aSlide, CMetaInformation &metainfo);
    HRESULT WriteCoverSheet(CFileOutput &wordOutput, CMetaInformation &metainfo);

    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
    void ReplaceAuthorAndDate(CString& csDest, CMetaInformation &metainfo);
    HRESULT GenerateThumbnail(CString csFilename, CArray<CWordSlideObject *, CWordSlideObject *> &aSlide, CMetaInformation &metainfo);
    void CalculateClickNumberPosition(CRect rcScreen, CRect rcClick, Gdiplus::RectF& rcEllipse, Gdiplus::PointF &p1, Gdiplus::PointF &p2);  

private:
    // pointer to the CEditorProject's manual configuration attribute
    CManualConfiguration *m_pManualConfiguration;
    LOGFONT m_logFont;
    CString m_csDocumentTitle;
    
};
