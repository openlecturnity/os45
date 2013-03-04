#pragma once
#include "SgmlParser.h"
#include "export.h"

class CMetaInformation;
class CPageInformation;
class CClipInfo;

class FILESDK_EXPORT CLepSgml :
    public SgmlFile
{
public:
    CLepSgml(void);
    ~CLepSgml(void);

public:
    static CLepSgml *CreateDefaultLep(bool bWithVideo, bool bHasClips, CString sCompatibleLepVersion, CString sCurrentLepVersion);

    HRESULT GetMetaInfo(CString csNodeName, CString &csValue);
    HRESULT SetMetaInfo(CString csNodeName, LPCTSTR tszValue);
    HRESULT GetKeywords(CStringArray &aKeywords);
    HRESULT GetLrdNames(CStringArray &aLrdFiles);
    HRESULT GetVideoExportSettings(SgmlElement *pSgmlTag, 
                                   CString &csClipSource, CString &csMediaType, 
                                   CString &csFormatType, CString &csVideoFormatInitialized,
                                   CString &csCodec, CString &csQuality, CString &csKeyframeRate, 
                                   CString &csUseKeyframes, CString &csSmartRecompression,
                                   CString &csGlobalOffset, CString &csRememberAspectratio, 
                                   CString &csVolume, CString &csOrigVolume,
                                   CString &csOrigWidth, CString &csOrigHeight, 
                                   CString &csOrigFps, CString &csHasOrigSize);
    HRESULT GetAudioSources(CArray<SgmlElement *, SgmlElement *> &aAudioSources);
    HRESULT GetConfigParameter(CString csNodeName, CString &csParameter);
    HRESULT GetExportParameter(CString csMediaType, CString csNodeName, CString &csParameter);
    HRESULT GetStreamParameter(CString csType, CString csNodeName, CString &csParameter);


    int GetAudioStreamCount();

    HRESULT AddAudioStreamInfo(CString &csAudioName, int iStartTime, int iEndTime);
    HRESULT AddVideoStreamInfo(CString &csVideoName, int iStartTime, int iEndTime, int iVideoOffset);
    HRESULT AddWhiteboardStreamInfo(CString csFilePrefix, int iSlideWidth, int iSlideHeight, 
                                    int iStartTime, int iEndTime);
    HRESULT AddSlidesStreamInfo(CArray<CPageInformation *, CPageInformation *> &aSlides, int iStartTime, int iEndTime);
    HRESULT AddClipsStreamInfo(CClipInfo *pClipInfo, int iStartTime, int iEndTime);

    HRESULT AddSlidesMetadata(CString csPageTitle, int iSlideNr, int iStartTime, int iEndTime);
    HRESULT UpdateInteractionsTag(int iLength);
    HRESULT AddClickInformation(int iXcoord, int iYcoord, int iTimeMsec,
                                CString &csAccID, CString &csUiAutoID, 
                                CString &csUiAutoTreeID, CString &csSapID,
                                /*CTime &time,*/ CRect &rcBounds, CString &csName, 
                                CString &csProcessName, int iAccType, int iSapType);

    HRESULT AddLrdName(CString &csLrdFile);
    HRESULT ChangeLrdName(CString csOldLrdPrefix, CString csNewLrdPrefix);
};
