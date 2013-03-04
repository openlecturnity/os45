#pragma once
#include "SgmlParser.h"
#include "export.h"

class FILESDK_EXPORT CLmdSgml : public SgmlFile
{
public:
    CLmdSgml(void);
    ~CLmdSgml(void);

public:
    static SgmlElement *CreatePageTag(CString &csPageTitle, int nr, int begin, int end, CString csSgmlPrefix, 
                                      bool bHasAction, int iXcoord, int iYcoord, CString &csButtonType,
                                      LPCTSTR lpszType=NULL);

public:

    HRESULT GetMetaInfo(CString csNodeName, CString &csValue);
    HRESULT GetKeywords(CStringArray &aKeywords);
    HRESULT GetAudioInfo(CString csNodeName, CString &csValue);
    HRESULT GetVideoInfo(CString csNodeName, CString &csValue);
    HRESULT GetAllClips(CArray<SgmlElement *, SgmlElement *> &aClips);
    HRESULT GetAllPages(CArray<SgmlElement *, SgmlElement *> &aPages);

    HRESULT ChangeMetaInfo(CString csNodeName, CString csValue);
    HRESULT ChangeKeywords(CStringArray &aKeywords);

    HRESULT ReplaceClipWithPages(SgmlElement *pClipTag, CArray<SgmlElement *, SgmlElement *> &aPages);

private:
    HRESULT AddElements(SgmlElement *pTag, CString csNodeName, CArray<SgmlElement *, SgmlElement *> &aElements);
    SgmlElement *GetPageBefore(SgmlElement *pClipTag);
};
