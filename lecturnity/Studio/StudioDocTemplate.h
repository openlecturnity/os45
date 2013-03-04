#if !defined(AFX_STUDIODOCTEMPLATEEX_H__878098E5_B02D_4951_A689_3B6F6217D125__INCLUDED_)
#define AFX_STUDIODOCTEMPLATEEX_H__878098E5_B02D_4951_A689_3B6F6217D125__INCLUDED_

#pragma once

class CStudioDocTemplate : public CMultiDocTemplate
{
public:	
	CStudioDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
    virtual ~CStudioDocTemplate();

public:
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);
	
    bool CloseDocumentFile();
    bool Save();
    bool m_bRetFromEditMode;


};

#endif // !defined(AFX_STUDIODOCTEMPLATEEX_H__878098E5_B02D_4951_A689_3B6F6217D125__INCLUDED_)
