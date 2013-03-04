#pragma once
#include "sgmlparser.h"
#include "export.h"

class FILESDK_EXPORT CLrdSgml : public SgmlFile
{
public:
    CLrdSgml(void);
    ~CLrdSgml(void);

public:
    HRESULT GetAudioFilename(CString &csAudioFilename);
    HRESULT GetMetadataFilename(CString &csMatadataFilename);
    HRESULT GetEvqAndObjFilename(CString &csEvqFilename, CString &csObjFilename);
    HRESULT GetPageDimension(CString &csPageDimension);
    HRESULT GetVideoAndStillImageFilenames(CString &csVideoFilename, CString &csStillImageFilename);
    HRESULT GetClipFilenamesAndTimestamps(CStringArray &arClipFilenames, CArray<int, int> &aTimestamps);
    
    /**
     * Needed to get title, author, ... from metainfo tag
     */
    HRESULT GetMetaInfo(CString csNodeName, CString &csValue);
    HRESULT GetAudioOffset(int &iAudioOffset);
    HRESULT GetVideoOffset(int &iVideoOffset);
    HRESULT GetClipStructureValue(CString csNodeName, CString &csValue);

    HRESULT ChangeMetaInfo(CString csNodeName, CString csValue);
    HRESULT ChangeAudioFileName(CString csAudioFileName);
    HRESULT ChangeMetaDataFileName(CString csMetadataFileName);
    HRESULT ChangeVideoFileName(CString csVideoFileName);
    HRESULT ChangeStillImageName(CString csStillImageName);
    HRESULT ChangeClipFilename(CString csClipFilename, int iClipPosition);
    HRESULT ChangeEvqFileName(CString csEvqFilename);
    HRESULT ChangeObjFileName(CString csObjFilename);

    HRESULT CreateWhiteboardHelper(CString csEvqFilename, CString csObjFilename, 
                                   int iResolutionWidth, int iResolutionHeight);

    int GetNumberOfClips();
    bool GetClipFlagAvailable(CString csFlagName, int iClipNr);
    HRESULT SetClipFlagAvailable(CString csFlagName, int iClipNr, bool bAvailable);

private:
    int GetOffset(int iOffsetPos);
    SgmlElement *GetWbPlayHelper();
    SgmlElement *GetJavaHelper();
    SgmlElement *CreateNewClip(int iClipId);
};
