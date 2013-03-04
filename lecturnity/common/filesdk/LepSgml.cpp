#include "stdafx.h"
#include "LepSgml.h"
#include "LmdFile.h"
#include "AVStreams.h"

/* static functions */

CLepSgml *CLepSgml::CreateDefaultLep(bool bWithVideo, bool bHasClips, CString sCompatibleLepVersion, CString sCurrentLepVersion) {
    
    CLepSgml *pNewLepSgml = new CLepSgml;
    pNewLepSgml->Init();

    SgmlElement *pRoot = new SgmlElement();
    pRoot->SetName(_T("editor"));
    pNewLepSgml->SetRoot(pRoot);

    SgmlElement *pFormatVersion = new SgmlElement();
    pFormatVersion->SetName(_T("formatversion"));
    pFormatVersion->SetAttribute(_T("compatible"), sCompatibleLepVersion);
    pFormatVersion->SetParameter(sCurrentLepVersion);
    pRoot->Append(pFormatVersion);

    SgmlElement *pLastExportPath = new SgmlElement();
    pRoot->Append(_T("lastexportpath"), _T("original"));

    /// metadata
    SgmlElement *pMetadata = new SgmlElement();
    pMetadata->SetName(_T("metainfo"));
    //pMetadata->Append(_T("version"), _T("1.1")); // never used; superseeded by the version of the LEP
    pMetadata->Append(_T("title"), _T(""));
    pMetadata->Append(_T("author"), _T(""));
    pMetadata->Append(_T("creator"), _T(""));
    pMetadata->Append(_T("comments"), _T(""));
    pMetadata->Append(_T("length"), _T(""));
    pMetadata->Append(_T("recorddate"), _T(""));
    pMetadata->Append(_T("recordtime"), _T(""));
    pMetadata->Append(_T("codepage"), _T("utf-8"));
    pRoot->Append(pMetadata);

    /// config
    SgmlElement *pConfig = new SgmlElement();
    pConfig->SetName(_T("config"));
    pConfig->Append(_T("sg-showclickpages"), _T("1"));
    pConfig->Append(_T("sg-viewmode"), _T("recording"));
    pRoot->Append(pConfig);
    
    /// files
    SgmlElement *pFiles = new SgmlElement();
    pFiles->SetName(_T("files"));
    pRoot->Append(pFiles);

    /// streams
    SgmlElement *pStreams = new SgmlElement();
    pStreams->SetName(_T("streams"));
    SgmlElement *pAudioTag = pStreams->Append(_T("audio"));

    if (bWithVideo)
        SgmlElement *pVideoTag = pStreams->Append(_T("video"));

    SgmlElement *pWbTag = pStreams->Append(_T("wb"));

    SgmlElement *pSlidesTag = pStreams->Append(_T("slides"));

    if (bHasClips) 
        SgmlElement *pClipsTag = pStreams->Append(_T("clips"));
    
    SgmlElement *pIntearcionsTag = pStreams->Append(_T("interactions"));

    pRoot->Append(pStreams);
    
    /// streams
    SgmlElement *pExport = new SgmlElement();
    pExport->SetName(_T("export"));
    pRoot->Append(pExport);

    return pNewLepSgml;
}


//
CLepSgml::CLepSgml(void)
{
}

CLepSgml::~CLepSgml(void)
{
}

HRESULT CLepSgml::GetMetaInfo(CString csNodeName, CString &csValue) {
    HRESULT hr = S_OK;

    SgmlElement *pMetaInfoTag = Find(_T("metainfo"), _T("editor"));
    if (pMetaInfoTag == NULL)
        hr = E_FAIL;

    SgmlElement *pElement = NULL;
    if (SUCCEEDED(hr)) {
        pElement = pMetaInfoTag->FindElement(csNodeName);
        if (pElement == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr))
        csValue = pElement->GetParameter();

    return hr;
}

HRESULT CLepSgml::SetMetaInfo(CString csNodeName, LPCTSTR tszValue) {
    HRESULT hr = S_OK;

    SgmlElement *pMetaInfoTag = Find(_T("metainfo"), _T("editor"));
    if (pMetaInfoTag == NULL)
        hr = E_FAIL;

    SgmlElement *pElement = NULL;
    if (SUCCEEDED(hr)) {
        pElement = pMetaInfoTag->FindElement(csNodeName);
    }

    if (SUCCEEDED(hr)) {
        if (pElement == NULL) {
            pElement = new SgmlElement();
            pElement->SetName(csNodeName);
            pMetaInfoTag->Append(pElement);
        }
        if (pElement == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr))
        pElement->SetParameter(tszValue);

    return hr;
}

HRESULT CLepSgml::GetKeywords(CStringArray &aKeywords) {
    HRESULT hr = S_OK;

    SgmlElement *pMetaInfoTag = Find(_T("metainfo"), _T("editor"));
    if (pMetaInfoTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        pMetaInfoTag->GetElements(aElements);
        for (int i = 0; i < aElements.GetSize(); ++i) {
            SgmlElement *pElement = aElements[i];
            if (pElement && _tcscmp(pElement->GetName(), _T("keyword")) == 0)
                aKeywords.Add(pElement->GetParameter());
        }
    }

    return hr;
}

HRESULT CLepSgml::GetLrdNames(CStringArray &aLrdFiles) {
    HRESULT hr = S_OK;

    SgmlElement *pFilesTag = Find(_T("files"), _T("editor"));
    if (pFilesTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        pFilesTag->GetElements(aElements);
        for (int i = 0; i < aElements.GetSize(); ++i) {
            SgmlElement *pElement = aElements[i];
            if (pElement != NULL && _tcscmp(pElement->GetName(), _T("lrd")) == 0)
                aLrdFiles.Add(pElement->GetParameter());
        }
    }

    return hr;
}

HRESULT CLepSgml::GetVideoExportSettings(SgmlElement *pSgmlTag, 
                                         CString &csClipSource, CString &csMediaType, 
                                         CString &csFormatType, CString &csVideoFormatInitialized,
                                         CString &csCodec, CString &csQuality, CString &csKeyframeRate, 
                                         CString &csUseKeyframes, CString &csSmartRecompression,
                                         CString &csGlobalOffset, CString &csRememberAspectratio, 
                                         CString &csVolume, CString &csOrigVolume,
                                         CString &csOrigWidth, CString &csOrigHeight, 
                                         CString &csOrigFps, CString &csHasOrigSize) {
    HRESULT hr = S_OK;

    csClipSource.Empty();
    csMediaType.Empty();
    csFormatType.Empty();
    csVideoFormatInitialized.Empty();
    csCodec.Empty();
    csQuality.Empty(); 
    csKeyframeRate.Empty();
    csUseKeyframes.Empty();
    csSmartRecompression.Empty();
    csGlobalOffset.Empty();
    csRememberAspectratio.Empty();
    csVolume.Empty();
    csOrigVolume.Empty();
    csOrigWidth.Empty();
    csOrigHeight.Empty();
    csOrigFps.Empty();
    csHasOrigSize.Empty();

    if (pSgmlTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        pSgmlTag->GetElements(aElements);
        for (int i = 0; i < aElements.GetSize(); ++i) {
            SgmlElement *pElement = aElements[i];
            if (pElement != NULL) {
                CString csName = pElement->GetName();
                if (csName == _T("filename"))
                    csClipSource = pElement->GetParameter();
                else if (csName == _T("mediatype"))
                    csMediaType = pElement->GetParameter();
                else if (csName == _T("format"))
                    csFormatType = pElement->GetParameter();
                else if (csName == _T("ismediatypeinitialized"))
                    csVideoFormatInitialized = pElement->GetParameter();
                else if (csName == _T("codec")) 
                    csCodec = pElement->GetParameter();
                else if (csName == _T("quality"))
                    csQuality = pElement->GetParameter();
                else if (csName == _T("keyframerate"))
                    csKeyframeRate = pElement->GetParameter();
                else if (csName == _T("usekeyframes"))
                    csUseKeyframes = pElement->GetParameter();
                else if (csName == _T("smartrecompression"))
                    csSmartRecompression = pElement->GetParameter();
                else if (csName == _T("globaloffset"))
                    csGlobalOffset = pElement->GetParameter();
                else if (csName == _T("rememberaspectratio"))
                    csRememberAspectratio = pElement->GetParameter();
                else if (csName == _T("volume"))
                    csVolume = pElement->GetParameter();
                else if (csName == _T("origvolume"))
                    csOrigVolume = pElement->GetParameter();
                else if (csName == _T("origwidth"))
                    csOrigWidth = pElement->GetParameter();
                else if (csName == _T("origheight"))
                    csOrigHeight = pElement->GetParameter();
                else if (csName == _T("origfps"))
                    csOrigFps = pElement->GetParameter();
                else if (csName == _T("hasorigsize"))
                    csHasOrigSize = pElement->GetParameter();
            }
        }
    }

    return hr;
}

HRESULT CLepSgml::GetAudioSources(CArray<SgmlElement *, SgmlElement *> &aAudioSources) {
    HRESULT hr = S_OK;

    aAudioSources.RemoveAll();

    SgmlElement *pAudioStreamTag = Find(_T("audio"), _T("streams"));
    if (pAudioStreamTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        pAudioStreamTag->GetElements(aElements);
        for (int i = 0; i < aElements.GetSize(); ++i) {
            SgmlElement *pElement = aElements[i];
            if (pElement != NULL) {
                if (_tcscmp(pElement->GetName(), _T("source")) == 0)
                    aAudioSources.Add(pElement);
            }
        }
    }

    return hr;
}

HRESULT CLepSgml::GetConfigParameter(CString csNodeName, CString &csParameter) {
    HRESULT hr = S_OK;

    csParameter.Empty();

    SgmlElement *pConfigTag = Find(_T("config"), _T("editor"));
    if (pConfigTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        SgmlElement *pElement = pConfigTag->FindElement(csNodeName);
        if (pElement != NULL)
            csParameter = pElement->GetParameter();
    }

    return hr;
}

HRESULT CLepSgml::GetExportParameter(CString csMediaType, CString csNodeName, CString &csParameter) {
    HRESULT hr = S_OK;

    csParameter.Empty();

    SgmlElement *pExportTag = Find(csMediaType, _T("export"));
    if (pExportTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        SgmlElement *pElement = pExportTag->FindElement(csNodeName);
        if (pElement != NULL)
            csParameter = pElement->GetParameter();
    }

    return hr;
}

HRESULT CLepSgml::GetStreamParameter(CString csType, CString csNodeName, CString &csParameter) {
    HRESULT hr = S_OK;

    csParameter.Empty();

    SgmlElement *pStreamTag = Find(csType, _T("streams"));
    if (pStreamTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        SgmlElement *pElement = pStreamTag->FindElement(csNodeName);
        if (pElement != NULL)
            csParameter = pElement->GetParameter();
    }

    return hr;
}

int CLepSgml::GetAudioStreamCount() {
    int iAudioStreamCount = 0;

    SgmlElement *pAudioStreamTag = Find(_T("audio"), _T("streams"));

    if (pAudioStreamTag != NULL)
        iAudioStreamCount = pAudioStreamTag->GetElementCount();

    return iAudioStreamCount;
}

HRESULT CLepSgml::AddAudioStreamInfo(CString &csAudioName, int iStartTime, int iEndTime) {
    HRESULT hr = S_OK;

    SgmlElement *pAudioStreamTag = Find(_T("audio"), _T("streams"));

    if (pAudioStreamTag) {
        CString csStartTime;
        csStartTime.Format(_T("%d"), iStartTime);
        CString csEndTime;
        csEndTime.Format(_T("%d"), iEndTime);

        pAudioStreamTag->SetAttribute(_T("start"), csStartTime);
        pAudioStreamTag->SetAttribute(_T("end"), csEndTime);
        SgmlElement *pSourceTag = pAudioStreamTag->Append(_T("source"));
        if (pSourceTag) {
            pSourceTag->SetAttribute(_T("id"), _T("1"));
            pSourceTag->SetAttribute(_T("stream-id"), _T("1"));
            pSourceTag->SetAttribute(_T("file"), csAudioName);
            pSourceTag->SetAttribute(_T("stream-start"), csStartTime);
            pSourceTag->SetAttribute(_T("stream-end"), csEndTime);
            pSourceTag->SetAttribute(_T("source-start"), csStartTime);
            pSourceTag->SetAttribute(_T("source-end"), csEndTime);
            pSourceTag->SetAttribute(_T("source-offset"), _T("0"));
            pSourceTag->SetAttribute(_T("stream-selector"), _T("0"));
            pSourceTag->SetAttribute(_T("orig-file"), _T(""));
            pSourceTag->SetAttribute(_T("orig-stream-selector"), _T("-1"));
        }
    }

    return hr;
}

HRESULT CLepSgml::AddVideoStreamInfo(CString &csVideoName, int iStartTime, int iEndTime, int iVideoOffset) {
    HRESULT hr = S_OK;

    SgmlElement *pVideoStreamTag = Find(_T("video"), _T("streams"));

    CString csStartTime;
    csStartTime.Format(_T("%d"), iStartTime);
    CString csEndTime;
    csEndTime.Format(_T("%d"), iEndTime);

    if (pVideoStreamTag == NULL) {
        SgmlElement *pStreamTag = Find(_T("streams"), _T("editor"));
        if (pStreamTag != NULL) {
            pVideoStreamTag = new SgmlElement();
            pVideoStreamTag->SetName(_T("video"));
            pVideoStreamTag->SetAttribute(_T("start"), csStartTime);
            pVideoStreamTag->SetAttribute(_T("end"), csEndTime);
            SgmlElement *pAudioStreamTag = Find(_T("audio"), _T("streams"));
            if (pAudioStreamTag)
                pStreamTag->InsertAfter(pVideoStreamTag, pAudioStreamTag);
            else
                pStreamTag->Append(pVideoStreamTag);
        }
    }

    if (pVideoStreamTag) {
        pVideoStreamTag->SetAttribute(_T("start"), csStartTime);
        pVideoStreamTag->SetAttribute(_T("end"), csEndTime);
        SgmlElement *pSourceTag = pVideoStreamTag->Append(_T("source"));
        if (pSourceTag) {
            CString csVideoOffset;
            csVideoOffset.Format(_T("%d"), iVideoOffset);
            pSourceTag->SetAttribute(_T("id"), _T("1"));
            pSourceTag->SetAttribute(_T("stream-id"), _T("1"));
            pSourceTag->SetAttribute(_T("file"), csVideoName);
            pSourceTag->SetAttribute(_T("offset"), csVideoOffset);
            pSourceTag->SetAttribute(_T("stream-start"), csStartTime);
            pSourceTag->SetAttribute(_T("stream-end"), csEndTime);
            pSourceTag->SetAttribute(_T("source-start"), csStartTime);
            pSourceTag->SetAttribute(_T("source-end"), csEndTime);
            pSourceTag->SetAttribute(_T("source-offset"), _T("0"));
            pSourceTag->SetAttribute(_T("stream-selector"), _T("0"));
        }
    }

    return hr;
}

HRESULT CLepSgml::AddWhiteboardStreamInfo(CString csFilePrefix, int iSlideWidth, int iSlideHeight, 
                                          int iStartTime, int iEndTime) {
    HRESULT hr = S_OK;

    SgmlElement *pWhiteboardStreamTag = Find(_T("wb"), _T("streams"));

    if (pWhiteboardStreamTag != NULL) {
        CString csStartTime;
        csStartTime.Format(_T("%d"), iStartTime);
        CString csEndTime;
        csEndTime.Format(_T("%d"), iEndTime);

        pWhiteboardStreamTag->SetAttribute(_T("start"), csStartTime);
        pWhiteboardStreamTag->SetAttribute(_T("end"), csEndTime);
        SgmlElement *pTargetFormatTag = pWhiteboardStreamTag->Append(_T("targetformat"));
        if (pTargetFormatTag != NULL) {
            SgmlElement *pPageDimensionTag = pTargetFormatTag->Append(_T("pagedimension"));
            if (pPageDimensionTag != NULL) {
                CString csPageDimension;
                csPageDimension.Format(_T("%d x %d"), iSlideWidth, iSlideHeight);
                pPageDimensionTag->SetParameter(csPageDimension);
            }
        }
        SgmlElement *pSourceTag = pWhiteboardStreamTag->Append(_T("source"));
        if (pSourceTag) {
            pSourceTag->SetAttribute(_T("id"), _T("1"));
            pSourceTag->SetAttribute(_T("stream-id"), _T("1"));
            pSourceTag->SetAttribute(_T("stream-start"), csStartTime);
            pSourceTag->SetAttribute(_T("stream-end"), csEndTime);
            pSourceTag->SetAttribute(_T("source-start"), csStartTime);
            pSourceTag->SetAttribute(_T("source-end"), csEndTime);
            CString csEvqName = csFilePrefix + _T(".evq");
            pSourceTag->Append(_T("evq"), csEvqName);
            CString csObjName = csFilePrefix + _T(".obj");
            pSourceTag->Append(_T("obj"), csObjName);
            CString csLmdName = csFilePrefix + _T(".lmd");
            pSourceTag->Append(_T("lmd"), csLmdName);
        }
    }

    return hr;
}

HRESULT CLepSgml::AddSlidesStreamInfo(CArray<CPageInformation *, CPageInformation *> &aSlides, int iStartTime, int iEndTime) {
    HRESULT hr = S_OK;

    SgmlElement *pSlidesStreamTag = Find(_T("slides"), _T("streams"));
    if (pSlidesStreamTag != NULL) {
        CString csStartTime;
        csStartTime.Format(_T("%d"), iStartTime);
        CString csEndTime;
        csEndTime.Format(_T("%d"), iEndTime);

        pSlidesStreamTag->SetAttribute(_T("start"), csStartTime);
        pSlidesStreamTag->SetAttribute(_T("end"), csEndTime);
        SgmlElement *pSlideTag = pSlidesStreamTag->Append(_T("slide"));
        if (pSlideTag) {
            pSlideTag->SetAttribute(_T("id"), _T("1"));
            pSlideTag->SetAttribute(_T("stream-id"), _T("1"));
            pSlideTag->SetAttribute(_T("stream-start"), csStartTime);
            pSlideTag->SetAttribute(_T("stream-end"), csEndTime);
            pSlideTag->SetAttribute(_T("source-start"), csStartTime);
            pSlideTag->SetAttribute(_T("source-end"), csEndTime);
            for (int i = 0; i < aSlides.GetCount(); ++i) {
                CPageInformation *pPageInfo = aSlides[i];
                if (pPageInfo != NULL) {
                    SgmlElement *pMetadataTag = pSlideTag->Append(_T("metadata"));
                    if (pMetadataTag != NULL) {
                        CString csValue;
                        csValue.Format(_T("%d"), pPageInfo->GetBegin());
                        pMetadataTag->SetAttribute(_T("slide-start"), csValue);
                        csValue.Format(_T("%d"), pPageInfo->GetEnd());
                        pMetadataTag->SetAttribute(_T("slide-end"), csValue);
                        pMetadataTag->SetAttribute(_T("title"), pPageInfo->GetTitle());
                        pPageInfo->GetKeywords(csValue);
                        pMetadataTag->SetAttribute(_T("keywords"), csValue);
                        csValue.Format(_T("%d"), pPageInfo->GetNumber());
                        pMetadataTag->SetAttribute(_T("number"), csValue);
                        csValue.Format(_T("%d"), pPageInfo->GetInternalNumber());
                        pMetadataTag->SetAttribute(_T("id"), csValue);
                    }
                }
            }
        }
    }

    return hr;
}

HRESULT CLepSgml::AddClipsStreamInfo(CClipInfo *pClipInfo, int iStartTime, int iEndTime) {
    HRESULT hr = S_OK;

    SgmlElement *pClipsStreamTag = Find(_T("clips"), _T("streams"));
    if (pClipsStreamTag != NULL && pClipInfo != NULL) {
        CString csEndTime;
        csEndTime.Format(_T("%d"), iEndTime);
        pClipsStreamTag->SetAttribute(_T("length"), csEndTime);
        SgmlElement *pClipTag = pClipsStreamTag->Append(_T("CLIP "));
        if (pClipTag) {
            pClipTag->SetAttribute(_T("file"), pClipInfo->GetFilename());
            CString csTimestamp;
            csTimestamp.Format(_T("%d"), pClipInfo->GetTimestamp());
            pClipTag->SetAttribute(_T("target-ms"), csTimestamp);
            pClipTag->SetAttribute(_T("source-ms"), _T("0"));
            CString csLength;
            csLength.Format(_T("%d"), pClipInfo->GetLength());
            pClipTag->SetAttribute(_T("length"), csLength);
            pClipTag->SetAttribute(_T("label"), _T(""));
            pClipTag->SetAttribute(_T("video-idx"), _T("0"));
            pClipTag->SetAttribute(_T("structure"), _T("true"));
            pClipTag->SetAttribute(_T("images"), _T("true"));
            pClipTag->SetAttribute(_T("fulltext"), _T("true"));
        }
    }

    return hr;
}

HRESULT CLepSgml::AddSlidesMetadata(CString csPageTitle, int iSlideNr, int iStartTime, int iEndTime) {
    HRESULT hr = S_OK;

    SgmlElement *pSlideTag = NULL;
    if (SUCCEEDED(hr)) {
        pSlideTag = Find(_T("slide"), _T("slides"));
        if (pSlideTag == NULL) {
            hr = E_FAIL;
        }
    }

    SgmlElement *pMetadataTag = NULL;
    if (SUCCEEDED(hr)) { 
        pMetadataTag = pSlideTag->Append(_T("metadata"));
        if (pMetadataTag == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        CString csValue;
        csValue.Format(_T("%d"), iStartTime);
        pMetadataTag->SetAttribute(_T("slide-start"), csValue);
        csValue.Format(_T("%d"), iEndTime);
        pMetadataTag->SetAttribute(_T("slide-end"), csValue);
        pMetadataTag->SetAttribute(_T("title"), csPageTitle);
        pMetadataTag->SetAttribute(_T("keywords"), _T(""));
        csValue.Format(_T("%d"), iSlideNr); 
        pMetadataTag->SetAttribute(_T("number"), csValue);
        pMetadataTag->SetAttribute(_T("id"), csValue);
    }



    return hr;
}

HRESULT CLepSgml::UpdateInteractionsTag(int iLength) {
    HRESULT hr = S_OK;

    SgmlElement *pInteractionsTag = Find(_T("interactions"), _T("streams"));
    if (pInteractionsTag == NULL) {
        hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        CString csLength;
        csLength.Format(_T("%d"), iLength);
        pInteractionsTag->SetAttribute(_T("length"), csLength);
    }

    return hr;
}

HRESULT CLepSgml::AddClickInformation(int iXcoord, int iYcoord, int iTimeMsec,
                                      CString &csAccID, CString &csUiAutoID, 
                                      CString &csUiAutoTreeID, CString &csSapID,
                                      /*CTime &time,*/ CRect &rcBounds, CString &csName, 
                                      CString &csProcessName, int iAccType, int iSapType) {
    HRESULT hr = S_OK;

    SgmlElement *pInteractionTag = NULL;
    if (SUCCEEDED(hr)) {
        pInteractionTag = Find(_T("interactions"), _T("streams"));
        if (pInteractionTag == NULL) {
            hr = E_FAIL;
        }
    }

    SgmlElement *pClipStructureTag = NULL;
    if (SUCCEEDED(hr)) {
        pClipStructureTag = pInteractionTag->Append(_T("CLICK"));
        if (pClipStructureTag == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) {
        CString csValue;
        csValue.Format(_T("%d"), iXcoord);
        pClipStructureTag->SetAttribute(_T("x"), csValue);
        csValue.Format(_T("%d"), iYcoord);
        pClipStructureTag->SetAttribute(_T("y"), csValue);
        pClipStructureTag->SetAttribute(_T("width"), _T("1"));
        pClipStructureTag->SetAttribute(_T("height"), _T("1"));
        csValue.Format(_T("time %d"), iTimeMsec);
        pClipStructureTag->SetAttribute(_T("visible"), csValue);
        pClipStructureTag->SetAttribute(_T("active"), csValue);
    }

    SgmlElement *pIDsTag = NULL;
    if (SUCCEEDED(hr)) {
        pIDsTag = pClipStructureTag->Append(_T("IDs"));
        if (pIDsTag == NULL)
            hr = E_FAIL;
    }
    if (SUCCEEDED(hr)) {
        pIDsTag->SetAttribute(_T("acc"), csAccID);
        pIDsTag->SetAttribute(_T("UIAuto"), csUiAutoID);
        pIDsTag->SetAttribute(_T("UiAutoTree"), csUiAutoTreeID);
        pIDsTag->SetAttribute(_T("SAP"), csSapID);
    }

    /* not used
    SgmlElement *pTimestampTag = NULL;
    if (SUCCEEDED(hr)) {
        CString csTimestamp = time.Format(_T("%d.%m.%Y %H:%M:%S"));
        pTimestampTag = pClipStructureTag->Append(_T("Timestamp"), csTimestamp);
        if (pTimestampTag == NULL)
            hr = E_FAIL;
    }
    */

    SgmlElement *pBoundsTag = NULL;
    if (SUCCEEDED(hr)) {
        pBoundsTag = pClipStructureTag->Append(_T("Bounds"));
        if (pBoundsTag == NULL)
            hr = E_FAIL;
    }
    if (SUCCEEDED(hr)) {
        CString csValue;
        csValue.Format(_T("%d"), rcBounds.top);
        pBoundsTag->SetAttribute(_T("top"), csValue);
        csValue.Format(_T("%d"), rcBounds.left);
        pBoundsTag->SetAttribute(_T("left"), csValue);
        csValue.Format(_T("%d"), rcBounds.Width());
        pBoundsTag->SetAttribute(_T("width"), csValue);
        csValue.Format(_T("%d"), rcBounds.Height());
        pBoundsTag->SetAttribute(_T("height"), csValue);
    }

    SgmlElement *pNameTag = NULL;
    if (SUCCEEDED(hr)) {
        CString csNameEncoded = csName;
        StringManipulation::EncodeXmlSpecialCharacters(csNameEncoded);
        pNameTag = pClipStructureTag->Append(_T("Name"), csNameEncoded);
        if (pNameTag == NULL)
            hr = E_FAIL;
    }
    
    SgmlElement *pProcessNameTag = NULL;
    if (SUCCEEDED(hr)) {
        pProcessNameTag = pClipStructureTag->Append(_T("ProcessName"), csProcessName);
        if (pProcessNameTag == NULL)
            hr = E_FAIL;
    }

    SgmlElement *pAccTypeTag = NULL;
    if (SUCCEEDED(hr)) {
        CString csValue;
        csValue.Format(_T("%d"), iAccType);
        pAccTypeTag = pClipStructureTag->Append(_T("AccType"), csValue);
        if (pAccTypeTag == NULL)
            hr = E_FAIL;
    }
    SgmlElement *pSapTypeTag = NULL;
    if (SUCCEEDED(hr)) {
        CString csValue;
        csValue.Format(_T("%d"), iSapType);
        pSapTypeTag = pClipStructureTag->Append(_T("SapType"), csValue);
        if (pSapTypeTag == NULL)
            hr = E_FAIL;
    }

    return hr;
}

HRESULT CLepSgml::AddLrdName(CString &csLrdFile) {
    HRESULT hr = S_OK;

    SgmlElement *pFilesTag = Find(_T("files"), _T("editor"));
    if (pFilesTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        SgmlElement *pElement = new SgmlElement();
        pElement->SetName(_T("lrd"));
        pElement->SetParameter(csLrdFile);
        pFilesTag->Append(pElement);
    }

    return hr;
}

HRESULT CLepSgml::ChangeLrdName(CString csOldLrdPrefix, CString csNewLrdPrefix) {
    HRESULT hr = S_OK;

    SgmlElement *pFileTag = Find(_T("files"), _T("editor"));
    if (pFileTag == NULL) 
        hr = E_FAIL;

    // Find all occurances of the "old" lrd file name 
    // and replace them with the new one
    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> arFileElements;
        pFileTag->GetElements(arFileElements);
        CString csOldLrdName = csOldLrdPrefix + _T(".lrd");
        CString csNewLrdName = csNewLrdPrefix + _T(".lrd");
        for (int i = 0; i < arFileElements.GetSize(); ++i) {
            SgmlElement *pLrdTag = arFileElements[i];
            if (pLrdTag != NULL && _tcscmp(pLrdTag->GetName(), _T("lrd")) == 0) {
                CString csLrdName = pLrdTag->GetParameter();
                if (csLrdName == csOldLrdName) 
                    pLrdTag->SetParameter(csNewLrdName);
            }
        }
    }

    return hr;
}

