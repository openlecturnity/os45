#if !defined(AFX_MANUALCONFIGURATION_H__F8C6E689_E143_4E7C_A809_77C512426653__INCLUDED_)
#define AFX_MANUALCONFIGURATION_H__F8C6E689_E143_4E7C_A809_77C512426653__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "export.h"
/*
 * Holds configuration data for manual creation
 */
class FILESDK_EXPORT CManualConfiguration
{
private:
    // Number of slides per page. From 1 to 3. Default 2.
    int m_iSlidesNumberPerPage;

    // True will show header in manual. False - no header is visible
    bool m_bShowHeader;

    // True will show footer in manual. False - no footer is visible
    bool m_bShowFooter;

    // True will show cover sheet in manual. False - no cover sheet is visible
    bool m_bShowCoverSheet;

    // True will show table of content in manual. False - no table of content is visible
    bool m_bShowTableOfContent;

public:
    CManualConfiguration(void);
    ~CManualConfiguration(void);

    // properties
    __declspec(property(get=GetSlidesNumberPerPage, put=SetSlidesNumberPerPage)) int SlidesNumberPerPage;
	inline int GetSlidesNumberPerPage(){return m_iSlidesNumberPerPage;};
	inline void SetSlidesNumberPerPage(int iSlidesNumberPerPage) {if (iSlidesNumberPerPage < 1 || iSlidesNumberPerPage > 3) ASSERT(false); m_iSlidesNumberPerPage = iSlidesNumberPerPage;};

    __declspec(property(get=GetShowHeader, put=SetShowHeader)) bool ShowHeader;
	inline bool GetShowHeader(){return m_bShowHeader;};
	inline void SetShowHeader(bool bShowHeader) {m_bShowHeader = bShowHeader;};

    __declspec(property(get=GetShowFooter, put=SetShowFooter)) bool ShowFooter;
	inline bool GetShowFooter(){return m_bShowFooter;};
	inline void SetShowFooter(bool bShowFooter) {m_bShowFooter = bShowFooter;};

    __declspec(property(get=GetShowCoverSheet, put=SetShowCoverSheet)) bool ShowCoverSheet;
	inline bool GetShowCoverSheet(){return m_bShowCoverSheet;};
	inline void SetShowCoverSheet(bool bShowCoverSheet) {m_bShowCoverSheet = bShowCoverSheet;};

    __declspec(property(get=GetShowTableOfContent, put=SetShowTableOfContent)) bool ShowTableOfContent;
	inline bool GetShowTableOfContent(){return m_bShowTableOfContent;};
	inline void SetShowTableOfContent(bool bShowTableOfContent) {m_bShowTableOfContent = bShowTableOfContent;};
};

#endif AFX_MANUALCONFIGURATION_H__F8C6E689_E143_4E7C_A809_77C512426653__INCLUDED_