#include "stdafx.h"
#include "LrdSgml.h"

#define NUM_CODEPAGE_UTF8 10646

CLrdSgml::CLrdSgml(void)
{
}

CLrdSgml::~CLrdSgml(void)
{
}

HRESULT CLrdSgml::GetAudioFilename(CString &csAudioFilename) {
    HRESULT hr = S_OK;

    csAudioFilename.Empty();

    SgmlElement *pAudiofileTag = Find(_T("AUDIO"), _T("FILES"));
    if (pAudiofileTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr))
        csAudioFilename = pAudiofileTag->GetParameter();

    return hr;
}

HRESULT CLrdSgml::GetMetadataFilename(CString &csMetadataFilename) {
    HRESULT hr = S_OK;

    csMetadataFilename.Empty();

    SgmlElement *pMetadataTag = Find(_T("METADATA"), _T("FILES"));
    if (pMetadataTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr))
        csMetadataFilename = pMetadataTag->GetParameter();

    return hr;
}

HRESULT CLrdSgml::GetEvqAndObjFilename(CString &csEvqFilename, CString &csObjFilename) {
    HRESULT hr = S_OK;

    csEvqFilename.Empty();
    csObjFilename.Empty();

    SgmlElement *pWbPlayHelper = GetWbPlayHelper();
    if (pWbPlayHelper == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aOpts;
        pWbPlayHelper->GetElements(aOpts);
        bool bFinished = false;
        for (int i = 0; i < aOpts.GetSize() && !bFinished; ++i) {
            SgmlElement *pOptTag = aOpts[i];
            if (pOptTag != NULL) { 
                CString csFileSuffix = pOptTag->GetParameter();
                StringManipulation::GetFileSuffix(csFileSuffix);
                csFileSuffix.MakeUpper();
                if (csFileSuffix == _T("OBJ"))
                    csObjFilename = pOptTag->GetParameter();
                else if (csFileSuffix == _T("EVQ"))
                    csEvqFilename = pOptTag->GetParameter();
                if (!csEvqFilename.IsEmpty() && !csObjFilename.IsEmpty())
                    bFinished = true;
            }
        }
        aOpts.RemoveAll();
    }

    return hr;
}

HRESULT CLrdSgml::GetPageDimension(CString &csPageDimension) {
    HRESULT hr = S_OK;

    csPageDimension.Empty();

    SgmlElement *pWbPlayHelper = GetWbPlayHelper();
    if (pWbPlayHelper == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aOpts;
        pWbPlayHelper->GetElements(aOpts);
        bool bFinished = false;
        bool bNextIsDimension = false;
        for (int i = 0; i < aOpts.GetSize() && !bFinished; ++i) {
            CArray<SgmlElement *, SgmlElement *> aOpts;
            pWbPlayHelper->GetElements(aOpts);
            for (int j = 0; j < aOpts.GetSize() && !bFinished; ++j) {
                SgmlElement *pOptTag = aOpts[j];
                if (pOptTag != NULL) {
                    if (_tcscmp(pOptTag->GetParameter(), _T("-G")) == 0) {
                        bNextIsDimension = true;
                    } else if (bNextIsDimension) {
                        csPageDimension = pOptTag->GetParameter();
                        bFinished = true;
                    }
                }
            }
        }
        aOpts.RemoveAll();
    }

    return hr;
}

HRESULT CLrdSgml::GetVideoAndStillImageFilenames(CString &csVideoFilename, CString &csStillImageFilename) {  
    HRESULT hr = S_OK;

    csVideoFilename.Empty();
    csStillImageFilename.Empty();
    
    SgmlElement *pJavaHelper = GetJavaHelper();
    if (pJavaHelper == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aOpts;
        pJavaHelper->GetElements(aOpts);
        bool bFinished = false;
        bool bLastWasVideo = false;
        bool bLastWasStillImage = false;
        bool bLastWasMulti = false;
        for (int i = 0; i < aOpts.GetCount() && !bFinished; ++i) {
            SgmlElement *pOptTag = aOpts[i];
            if (pOptTag != NULL) {
                // initialize video filename
                if (_tcscmp(pOptTag->GetParameter(), _T("-video")) == 0) {
                    bLastWasVideo = true;
                } else if (_tcscmp(pOptTag->GetParameter(), _T("-still")) == 0) {
                    bLastWasStillImage = true;
                    bLastWasVideo = false;
                } else if (_tcscmp(pOptTag->GetParameter(), _T("-multi")) == 0) {
                    bFinished = true;
                    bLastWasVideo = false;
                } else if (_tcscmp(pOptTag->GetParameter(), _T("imc.epresenter.player.JMFVideoPlayer")) == 0) {
                    // in old lrd files there is no "-video"
                    // if the next entry was -still or -multi the variable will be resetted
                    bLastWasVideo = true;
                } else if (bLastWasVideo) {
                    csVideoFilename = pOptTag->GetParameter();
                    bLastWasVideo = false;
                } else if (bLastWasStillImage) {
                    csStillImageFilename = pOptTag->GetParameter();
                    bLastWasStillImage = false;
                }
            }
        }
        aOpts.RemoveAll();
    }

    return hr;
}

HRESULT CLrdSgml::GetClipFilenamesAndTimestamps(CStringArray &aClipFilenames, CArray<int, int> &aTimestamps) {  
    HRESULT hr = S_OK;

    aClipFilenames.RemoveAll();
    aTimestamps.RemoveAll();

    SgmlElement *pJavaHelper = GetJavaHelper();
    if (pJavaHelper == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aOpts;
        pJavaHelper->GetElements(aOpts);

        bool bLastWasMulti = false;
        bool bNextIsTimestamp = false;
        for (int i = 0; i < aOpts.GetCount() ; ++i) {
            SgmlElement *pOptTag = aOpts[i];
            if (pOptTag != NULL) {
                if (_tcscmp(pOptTag->GetParameter(), _T("-multi")) == 0) {
                    bLastWasMulti = true;
                } else if (bNextIsTimestamp) {
                    int iTimestamp = _ttoi(pOptTag->GetParameter());
                    aTimestamps.Add(iTimestamp);
                    bNextIsTimestamp = false;
                } else if (bLastWasMulti) {
                    aClipFilenames.Add(pOptTag->GetParameter());
                    bNextIsTimestamp = true;
                }
            }
        }
        aOpts.RemoveAll();
    }

    return hr;
}

HRESULT CLrdSgml::GetMetaInfo(CString csNodeName, CString &csValue) {
    HRESULT hr = S_OK;

    csValue.Empty();

    SgmlElement *pInfoTag = Find(_T("INFO"));
    if (pInfoTag == NULL)
        hr = E_FAIL;

    SgmlElement *pValueTag = NULL;
    if (SUCCEEDED(hr)) {
        pValueTag = pInfoTag->Find(csNodeName);
        if (pValueTag == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) 
        csValue = pValueTag->GetParameter();

    return hr;
}

HRESULT CLrdSgml::GetAudioOffset(int &iAudioOffset) {
    HRESULT hr = S_OK;

    // The audio offset is the first offset in list
    iAudioOffset = GetOffset(0);

    return hr;
}

HRESULT CLrdSgml::GetVideoOffset(int &iVideoOffset) {
    HRESULT hr = S_OK;

    // The video offset is the second offset in list
    iVideoOffset = GetOffset(1);

    return hr;
}

HRESULT CLrdSgml::GetClipStructureValue(CString csNodeName, CString &csValue) {
    HRESULT hr = S_OK;

    csValue.Empty();

    SgmlElement *pClipTag = Find(_T("CLIP"), _T("CLIPSTRUCTURE"));
    if (pClipTag == NULL)
        hr = E_FAIL;

    SgmlElement *pSubTag = NULL;
    if (SUCCEEDED(hr)) {
        pSubTag = pClipTag->Find(csNodeName);
        if (pSubTag == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr))
        csValue = pSubTag->GetParameter();

    return hr;
}

bool CLrdSgml::GetClipFlagAvailable(CString csFlagName, int iClipNr) {
    // get first clip structure info
    SgmlElement *pClipStructureTag = Find(_T("CLIPSTRUCTURE"), _T("DOCUMENT"));
    if (pClipStructureTag == NULL)
        return false;

    // skip to specified clipNr
    CArray<SgmlElement *, SgmlElement *> aClipStructureElements;
    pClipStructureTag->GetElements(aClipStructureElements);

    SgmlElement *pClipElement = NULL;
    if (iClipNr <= aClipStructureElements.GetCount() && iClipNr > 0)
        pClipElement = aClipStructureElements[iClipNr-1];

    // read clip structure flag
    if (pClipElement) {
        SgmlElement *pStructureAvailable = pClipElement->Find(csFlagName);
        if (pStructureAvailable) 
            return _tcscmp(pStructureAvailable->GetParameter(),_T("available")) == 0;
        // note: attributes for LRD are not supported for Player #Bug 4538  
        //return structureAvailable->GetBoolValue(_T("available"));

    }

    //not found means not available    
    return false;
}

HRESULT CLrdSgml::SetClipFlagAvailable(CString csFlagName, int iClipNr, bool bAvailable) {
    HRESULT hr = S_OK;

    // try to find existing CLIPSTRUCTURE
    SgmlElement *pClipStructureTag = Find(_T("CLIPSTRUCTURE"), _T("DOCUMENT"));

    // not found: add missing CLIPSTRUCTURE
    if (pClipStructureTag == NULL) {
        pClipStructureTag = new SgmlElement();
        pClipStructureTag->SetName(_T("CLIPSTRUCTURE"));
        SgmlElement *pDocumentTag = Find(_T("DOCUMENT"));
        // add after FILES - FILES must be valid or no clips could have been loaded
        SgmlElement *pFilesTag = Find(_T("FILES"), _T("DOCUMENT"));

        pDocumentTag->InsertAfter(pClipStructureTag, pFilesTag);
    }

    if (pClipStructureTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        // get first clip structure info
        CArray<SgmlElement *, SgmlElement*> aClipStructureElements;
        pClipStructureTag->GetElements(aClipStructureElements);

        int iClipsToInsert = iClipNr - aClipStructureElements.GetCount();
        int iNewClipNr = aClipStructureElements.GetCount();
        SgmlElement *pClipTag = NULL;
        for (int i = 0; i < iClipsToInsert && SUCCEEDED(hr); ++i) {
            pClipTag = CreateNewClip(iNewClipNr);
            ++iNewClipNr;
            if (pClipTag != NULL) {
                // attach new clip tag
                if (iClipNr < aClipStructureElements.GetCount()) 
                    pClipStructureTag->InsertAfter(pClipTag, aClipStructureElements[iClipNr]);
                else
                    pClipStructureTag->Append(pClipTag);
            }
        }

        // this clip tag was already inserted
        if (pClipTag == NULL) {
            pClipTag = aClipStructureElements[iClipNr-1];
        }

        if (pClipTag != NULL) {
            // set flag
            SgmlElement *pElement = pClipTag->Find(csFlagName);
            if (pElement) {
                if (bAvailable)
                    pElement->SetParameter(_T("available"));
                else
                    pElement->SetParameter(_T(""));
            }
        }
    }

    return hr;
}

HRESULT CLrdSgml::ChangeMetaInfo(CString csNodeName, CString csValue) {
    HRESULT hr = S_OK;

    SgmlElement *pInfoTag = Find(_T("INFO"));
    if (pInfoTag == NULL)
        hr = E_FAIL;

    SgmlElement *pValueTag = NULL;
    if (SUCCEEDED(hr)) {
        pValueTag = pInfoTag->Find(csNodeName);
        if (pValueTag == NULL)
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr)) 
        pValueTag->SetParameter(csValue);

    return hr;
}

HRESULT CLrdSgml::ChangeAudioFileName(CString csAudioFileName) {
    HRESULT hr = S_OK;

    SgmlElement *pAudioFileTag = Find(_T("AUDIO"), _T("FILES"));
    if (pAudioFileTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr))
        pAudioFileTag->SetParameter(csAudioFileName);

    return hr;
}

HRESULT CLrdSgml::ChangeMetaDataFileName(CString csMetadataFileName) {
    HRESULT hr = S_OK;

    SgmlElement *pMetadataTag = Find(_T("METADATA"), _T("FILES"));
    if (pMetadataTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr))
        pMetadataTag->SetParameter(csMetadataFileName);

    return hr;
}

HRESULT CLrdSgml::ChangeVideoFileName(CString csVideoFileName) {
    HRESULT hr = S_OK;

    SgmlElement *pJavaHelper = GetJavaHelper();
    if (pJavaHelper == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aOpts;
        pJavaHelper->GetElements(aOpts);

        bool bFinished = false;
        bool bIsVideoEntry = false;
        for (int i = 0; i < aOpts.GetSize() && !bFinished; ++i) {
            SgmlElement *pOptTag = aOpts[i];
            if (pOptTag != NULL) {
                if (_tcscmp(pOptTag->GetParameter(), _T("-video")) == 0) {
                    bIsVideoEntry = true;
                } else if (bIsVideoEntry) {
                    pOptTag->SetParameter(csVideoFileName);
                    bFinished = true;
                }
            }
        }
        aOpts.RemoveAll();
    }

    return hr;
}

HRESULT CLrdSgml::ChangeStillImageName(CString csStillImageName) {
    HRESULT hr = S_OK;

    SgmlElement *pJavaHelper = GetJavaHelper();
    if (pJavaHelper == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aOpts;
        pJavaHelper->GetElements(aOpts);

        bool bFinished = false;
        bool bIsStillImageEntry = false;
        for (int i = 0; i < aOpts.GetSize() && !bFinished; ++i) {
            SgmlElement *pOptTag = aOpts[i];
            if (pOptTag != NULL) {
                if (_tcscmp(pOptTag->GetParameter(), _T("-still")) == 0) {
                    bIsStillImageEntry = true;
                } else if (bIsStillImageEntry) {
                    pOptTag->SetParameter(csStillImageName);
                    bFinished = true;
                }
            }
        }
        aOpts.RemoveAll();
    }

    return hr;
}

HRESULT CLrdSgml::ChangeClipFilename(CString csClipFilename, int iClipPosition) {
    HRESULT hr = S_OK;

    SgmlElement *pJavaHelper = GetJavaHelper();
    if (pJavaHelper == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aOpts;
        pJavaHelper->GetElements(aOpts);

        bool bFinished = false;
        bool bClipEntryFound = false;
        int iClipCounter = 0;
        for (int i = 0; i < aOpts.GetSize() && !bFinished; ++i) {
            SgmlElement *pOptTag = aOpts[i];
            if (pOptTag != NULL) {
                if (_tcscmp(pOptTag->GetParameter(), _T("-multi")) == 0) {
                    bClipEntryFound = true;
                } else if (bClipEntryFound) {
                    if (iClipCounter / 2 == iClipPosition) {
                        pOptTag->SetParameter(csClipFilename);
                        bFinished = true;
                    }
                    ++iClipCounter;
                }
            }
        }
        aOpts.RemoveAll();
    }

    return hr;
}

HRESULT CLrdSgml::ChangeEvqFileName(CString csEvqFilename) {
    HRESULT hr = S_OK;

    SgmlElement *pWbPlayHelper = GetWbPlayHelper();
    if (pWbPlayHelper == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aOpts;
        pWbPlayHelper->GetElements(aOpts);
        bool bFinished = false;
        for (int i = 0; i < aOpts.GetSize() && !bFinished; ++i) {
            SgmlElement *pOptTag = aOpts[i];
            if (pOptTag != NULL) {
                CString csFileSuffix = pOptTag->GetParameter();
                StringManipulation::GetFileSuffix(csFileSuffix);
                csFileSuffix.MakeUpper();
                if (csFileSuffix == _T("EVQ")) {
                    pOptTag->SetParameter(csEvqFilename);
                    bFinished = true;
                }
            }
        }
        aOpts.RemoveAll();
    }

    return hr;
}

HRESULT CLrdSgml::ChangeObjFileName(CString csObjFilename) {
    HRESULT hr = S_OK;

    SgmlElement *pWbPlayHelper = GetWbPlayHelper();
    if (pWbPlayHelper == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aOpts;
        pWbPlayHelper->GetElements(aOpts);
        bool bFinished = false;
        for (int i = 0; i < aOpts.GetSize() && !bFinished; ++i) {
            SgmlElement *pOptTag = aOpts[i];
            if (pOptTag != NULL) {
                CString csFileSuffix = pOptTag->GetParameter();
                StringManipulation::GetFileSuffix(csFileSuffix);
                csFileSuffix.MakeUpper();
                if (csFileSuffix == _T("OBJ")) {
                    pOptTag->SetParameter(csObjFilename);
                    bFinished = true;
                }
            }
        }
        aOpts.RemoveAll();
    }

    return hr;
}

HRESULT CLrdSgml::CreateWhiteboardHelper(CString csEvqFilename, CString csObjFilename, 
                                         int iResolutionWidth, int iResolutionHeight) {
    HRESULT hr = S_OK;
  
    // get helper
    SgmlElement *pHelperTag = Find(_T("HELPERS"), _T("FILES"));
    if (pHelperTag) {
        // remember if whiteboard helper is added
        bool bSet = false;

        // search whiteboard helper
        CArray<SgmlElement *, SgmlElement *> aHelperElements;
        pHelperTag->GetElements(aHelperElements);
        bool bLastWasWbPlay = false;
        bool bSetDimension = false;

        SgmlElement *pWbPlayHelper = GetWbPlayHelper();
        if (pWbPlayHelper != NULL) {
            CArray<SgmlElement *, SgmlElement *> aOpts;
            pWbPlayHelper->GetElements(aOpts);
            bool bFinished = false;
            for (int i = 0; i < aOpts.GetSize() && !bFinished; ++i) {
                SgmlElement *pOptTag = aOpts[i];
                if (pOptTag != NULL) {
                    if (_tcscmp(pOptTag->GetParameter(), _T("-G")) == 0) {
                        bSetDimension = true;
                    } else if (bSetDimension) {
                        CString csResolution;
                        csResolution.Format(_T("%ix%i"), iResolutionWidth, iResolutionHeight);
                        pOptTag->SetParameter(csResolution);
                        bSetDimension = false;
                    } else {
                        CString csFileSuffix = pOptTag->GetParameter();
                        StringManipulation::GetFileSuffix(csFileSuffix);
                        csFileSuffix.MakeUpper();
                        if (csFileSuffix == _T("OBJ"))
                            pOptTag->SetParameter(csObjFilename);
                        else if (csFileSuffix == _T("EVQ"))
                            pOptTag->SetParameter(csEvqFilename);
                    }
                    bSet = true;
                }
            }
            aOpts.RemoveAll();
        }

        // add additional whiteboard helper (if required)
        if (!bSet) {
            // <H>
            SgmlElement *pNewWbHelper = new SgmlElement();
            pNewWbHelper->SetName(_T("H"));

            // <OPT>wbPlay</OPT>
            SgmlElement *pOpt = new SgmlElement();
            pOpt->SetName(_T("OPT"));
            pOpt->SetParameter(_T("wbPlay"));
            pNewWbHelper->Append(pOpt);

            // <OPT>-G</OPT>
            pOpt = new SgmlElement();
            pOpt->SetName(_T("OPT"));
            pOpt->SetParameter(_T("-G"));
            pNewWbHelper->Append(pOpt);

            // <OPT>WIDTHxHEIGHT</OPT>
            CString resolution;
            resolution.Format(_T("%ix%i"), iResolutionWidth, iResolutionHeight);
            pOpt = new SgmlElement();
            pOpt->SetName(_T("OPT"));
            pOpt->SetParameter(resolution);
            pNewWbHelper->Append(pOpt);

            // <OPT>xxx.evq</OPT>
            pOpt = new SgmlElement();
            pOpt->SetName(_T("OPT"));
            pOpt->SetParameter(csEvqFilename);
            pNewWbHelper->Append(pOpt);

            // <OPT>xxx.obj</OPT>
            pOpt = new SgmlElement();
            pOpt->SetName(_T("OPT"));
            pOpt->SetParameter(csObjFilename);
            pNewWbHelper->Append(pOpt);

            // add as new son before current son
            pHelperTag->InsertAtBegin(pNewWbHelper);
        }
    }

    return hr;
}

int CLrdSgml::GetNumberOfClips() {
    int iClipCount = 0;

    SgmlElement *pJavaHelper = GetJavaHelper();
    if (pJavaHelper != NULL) {

        CArray<SgmlElement *, SgmlElement *> aOpts;
        pJavaHelper->GetElements(aOpts);

        bool bClipEntryFound = false;
        for (int i = 0; i < aOpts.GetCount(); ++i) {
            SgmlElement *pOptTag = aOpts[i];
            if (_tcscmp(pOptTag->GetParameter(), _T("-multi")) == 0) {
                bClipEntryFound = true;
            } else if (bClipEntryFound) {
                ++iClipCount;
            }
        }
        // every clip entry consists of filename and timestamp
        iClipCount = iClipCount / 2;
    }

    return iClipCount;
}

//// private
int CLrdSgml::GetOffset(int iOffsetPos) {
    HRESULT hr = S_OK;

    int iOffset = 0;

    SgmlElement *pOffsetsTag = Find(_T("OFFSETS"), _T("FILES"));
    if (pOffsetsTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aOffsets;
        pOffsetsTag->GetElements(aOffsets);
        if (iOffsetPos < aOffsets.GetSize())  {
            SgmlElement *pOffset = aOffsets[iOffsetPos];
            iOffset = _ttoi(pOffset->GetParameter());
        }
    }

    return iOffset;
}

SgmlElement *CLrdSgml::GetWbPlayHelper() {
    HRESULT hr = S_OK;

    SgmlElement *pHelperTag = Find(_T("HELPERS"), _T("FILES"));
    if (pHelperTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aHelpers;
        pHelperTag->GetElements(aHelpers);

        for (int i = 0; i < aHelpers.GetSize(); ++i) {
            SgmlElement *pHelper = aHelpers[i];
            if (pHelper != NULL && _tcscmp(pHelper->GetName(), _T("H")) == 0) {
                CArray<SgmlElement *, SgmlElement *> aOpts;
                pHelper->GetElements(aOpts);

                bool bLastWasWbPlay = false;
                for (int j = 0; j < aOpts.GetSize(); ++j) {
                    SgmlElement *pOptTag = aOpts[j];
                    if (pOptTag != NULL) {
                        if (_tcscmp(pOptTag->GetParameter(),  _T("wbPlay")) == 0)
                            return pHelper;
                    }
                }
                aOpts.RemoveAll();
            }
        }
        aHelpers.RemoveAll();
    }

    return NULL;
}
    
SgmlElement *CLrdSgml::GetJavaHelper() {
    HRESULT hr = S_OK;

    SgmlElement *pHelperTag = Find(_T("HELPERS"), _T("FILES"));
    if (pHelperTag == NULL)
        hr = E_FAIL;

    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aHelpers;
        pHelperTag->GetElements(aHelpers);

        for (int i = 0; i < aHelpers.GetSize(); ++i) {
            SgmlElement *pHelper = aHelpers[i];
            if (pHelper != NULL && _tcscmp(pHelper->GetName(), _T("H")) == 0) {
                CArray<SgmlElement *, SgmlElement *> aOpts;
                pHelper->GetElements(aOpts);

                bool bLastWasWbPlay = false;
                for (int j = 0; j < aOpts.GetSize(); ++j) {
                    SgmlElement *pOptTag = aOpts[j];
                    if (pOptTag != NULL) {
                        if (_tcscmp(pOptTag->GetParameter(),  _T("java")) == 0)
                            return pHelper;
                    }
                }
                aOpts.RemoveAll();
            }
        }
        aHelpers.RemoveAll();
    }

    return NULL;
}

SgmlElement *CLrdSgml::CreateNewClip(int iClipId) {

    SgmlElement *pClipTag = new SgmlElement();

    if (pClipTag != NULL) {
        // set values
        pClipTag->SetName(_T("CLIP"));

        // STRUCTURE
        SgmlElement *pStructureTag = new SgmlElement();
        if (pStructureTag != NULL) {
            pStructureTag->SetName(_T("STRUCTURE"));
            pClipTag->Append(pStructureTag);
        }

        // IMAGES
        SgmlElement *pImagesTag = new SgmlElement();
        if (pImagesTag != NULL) {
            pImagesTag->SetName(_T("IMAGES"));
            pClipTag->Append(pImagesTag);
        }

        // FULLTEXT
        SgmlElement *pFulltextTag = new SgmlElement();
        if (pFulltextTag != NULL) {
            pFulltextTag->SetName(_T("FULLTEXT"));
            pClipTag->Append(pFulltextTag);
        }
    }

    return pClipTag;
}