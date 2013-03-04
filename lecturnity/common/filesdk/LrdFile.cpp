#include "StdAfx.h"

#include "LrdFile.h"
#include "Resource.h"
#include "MiscFunctions.h"
#include "lutils.h"

/*********************************************
 *********************************************/

CLrdFile::CLrdFile()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init();
}

CLrdFile::~CLrdFile()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();

   delete[] m_anNavigationStates;
   m_anNavigationStates = NULL;
}

void CLrdFile::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_nCodePage = ::GetACP();

   m_anNavigationStates = new int[NAVIGATION_STATES];
   for (int i =0; i < NAVIGATION_STATES; ++i)
      m_anNavigationStates[i] = 0;
}

void CLrdFile::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_lrdSgml.Clear();

   m_csFilename.Empty();
   m_csRecordPath.Empty();
   
   m_csMetadataFilename.Empty();
   m_csAudioFilename.Empty();
   m_csVideoFilename.Empty();
   m_csEvqFilename.Empty();
   m_csObjFilename.Empty();
   m_csPageDimension.Empty();
   m_aClipFilenames.RemoveAll();
   m_aClipTimestamps.RemoveAll();

   for (int i =0; i < NAVIGATION_STATES; ++i)
      m_anNavigationStates[i] = 0;
}

bool CLrdFile::Open(LPCTSTR szFilename) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    Clear();

    m_csFilename = szFilename;
    StringManipulation::GetFilename(m_csFilename);
    m_csRecordPath = szFilename;
    StringManipulation::GetPath(m_csRecordPath);

    if (!m_lrdSgml.Read(szFilename, _T("<DOC")))
        return false;

    // Read the codepage. Default to current code page
    CString csCodePage;
    m_lrdSgml.GetMetaInfo(_T("CODEPAGE"), csCodePage);
    m_nCodePage = ::GetACP();
    if (!csCodePage.IsEmpty()) {
        if (csCodePage.CompareNoCase(_T("utf-8")) == 0)
            m_nCodePage = NUM_CODEPAGE_UTF8;
        else
            m_nCodePage = _ttoi(csCodePage);
    }

    // initialize metadata filename
    CString csMetadataFilename;
    hr = m_lrdSgml.GetMetadataFilename(csMetadataFilename);
    if (SUCCEEDED(hr))
        m_csMetadataFilename.Format(_T("%s\\%s"),m_csRecordPath, csMetadataFilename);

    // initialize audio filename
    CString csAudioFilename;
    hr = m_lrdSgml.GetAudioFilename(csAudioFilename);
    if (SUCCEEDED(hr))
        m_csAudioFilename.Format(_T("%s\\%s"),m_csRecordPath, csAudioFilename);

    // initialize event queue and object file name
    CString csEvqFilename;
    CString csObjFilename;
    hr = m_lrdSgml.GetEvqAndObjFilename(csEvqFilename, csObjFilename);
    if (SUCCEEDED(hr)) {
        m_csEvqFilename.Format(_T("%s\\%s"),m_csRecordPath, csEvqFilename);
        m_csObjFilename.Format(_T("%s\\%s"),m_csRecordPath, csObjFilename);
    }

    hr = m_lrdSgml.GetPageDimension(m_csPageDimension);

    CString csVideoFilename;
    CString csStillImageFilename;
    hr = m_lrdSgml.GetVideoAndStillImageFilenames(csVideoFilename, csStillImageFilename);
    if (SUCCEEDED(hr)) {
        if (!csVideoFilename.IsEmpty())
            m_csVideoFilename.Format(_T("%s\\%s"),m_csRecordPath, csVideoFilename);
        if (!csStillImageFilename.IsEmpty())
            m_csStillImageFilename.Format(_T("%s\\%s"),m_csRecordPath, csStillImageFilename);
    }

    CStringArray aClipFilenames;
    hr = m_lrdSgml.GetClipFilenamesAndTimestamps(aClipFilenames, m_aClipTimestamps);
    if (SUCCEEDED(hr)) {
        for (int i = 0; i < aClipFilenames.GetSize(); ++i) {
            CString csClipFilename;
            csClipFilename.Format(_T("%s\\%s"),m_csRecordPath, aClipFilenames[i]);
            m_aClipFilenames.Add(csClipFilename);
        }
    }

    // Ignore success, always return true. Even if reading
    // the code page does not work, return true.
    return true;
}

int CLrdFile::GetAudioOffset() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    int iAudioOffset = 0;
    HRESULT hr = m_lrdSgml.GetAudioOffset(iAudioOffset);
    return iAudioOffset;
}

int CLrdFile::RetrieveAudioLength() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString csAudioFileName = GetAudioFilename();
    if (!csAudioFileName.IsEmpty()) {
        HRESULT hr = ::CoInitialize(NULL);
        if (SUCCEEDED(hr)) {
            CComPtr<IMediaDet> pDet;
            hr = ::CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, IID_IMediaDet, (void **)&pDet);

            CComBSTR bstrAudioFile(csAudioFileName);
            if (SUCCEEDED(hr))
                hr = pDet->put_Filename(bstrAudioFile);

            // TODO search for audio
            if (SUCCEEDED(hr))
                hr = pDet->put_CurrentStream(0); // only one stream in an audio file

            double dStreamLength = 0;
            if (SUCCEEDED(hr))
                hr = pDet->get_StreamLength(&dStreamLength);


            if (SUCCEEDED(hr) && dStreamLength > 0)
                return (int)(dStreamLength * 1000);
        }
        ::CoUninitialize();
    }

    return -1;
}

CVideoStream *CLrdFile::CreateVideoStream() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CVideoStream *pVideoStream = NULL;

    if (!m_csVideoFilename.IsEmpty()) {		 
        pVideoStream = new CVideoStream();
        pVideoStream->SetFilename(m_csVideoFilename);
        return pVideoStream;
    }

    return NULL;
}

// PZI: set filename of still image
// TODO: untested
void CLrdFile::SetStillImage(LPCTSTR tszStillImageName) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_csStillImageFilename = tszStillImageName;
    HRESULT hr = m_lrdSgml.ChangeStillImageName(tszStillImageName);
    return;
}

int CLrdFile::GetVideoOffset() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    int iVideoOffset = 0;
    HRESULT hr = m_lrdSgml.GetVideoOffset(iVideoOffset);

    return iVideoOffset;
}

CClipStream *CLrdFile::CreateClipStream() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CClipStream *pClipStream = NULL;

    SgmlElement *pClipStructureTag = m_lrdSgml.Find(_T("CLIP"), _T("CLIPSTRUCTURE"));
    if (!m_aClipFilenames.IsEmpty() && !m_aClipTimestamps.IsEmpty()) {
        pClipStream = new CClipStream();
        ASSERT(m_aClipFilenames.GetCount() == m_aClipTimestamps.GetCount());
        for (int i = 0; i < m_aClipFilenames.GetCount(); ++i) {
            CClipInfo *pNewClip = new CClipInfo();
            pClipStream->AddClip(pNewClip);
            pNewClip->SetFilename(m_aClipFilenames[i]);
            pNewClip->SetTimestamp(m_aClipTimestamps[i]); 
            if (pClipStructureTag != NULL) {
                // read structuring status
                CString csStructureAvailable;
                pClipStructureTag->GetElementParameter(_T("STRUCTURE"), csStructureAvailable);
                bool bStructureAvailable = (csStructureAvailable == _T("available")) ? true : false;
                pNewClip->SetStructureAvailable(bStructureAvailable);
                
                CString csImagesAvailable;
                pClipStructureTag->GetElementParameter(_T("IMAGES"), csImagesAvailable);
                bool bImagesAvailable = (csImagesAvailable == _T("available")) ? true : false;
                pNewClip->SetImagesAvailable(bImagesAvailable);
                
                CString csFulltextAvailable;
                pClipStructureTag->GetElementParameter(_T("FULLTEXT"), csFulltextAvailable);
                bool bFulltextAvailable = (csFulltextAvailable == _T("available")) ? true : false;
                pNewClip->SetFulltextAvailable(bFulltextAvailable);
            }
        }
    }

    return pClipStream;
}

CWhiteboardStream *CLrdFile::CreateWhiteboardStream(bool bOpen) { 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CWhiteboardStream *pWhiteboardStream = NULL;

    if (!m_csEvqFilename.IsEmpty() && !m_csObjFilename.IsEmpty()) {
        pWhiteboardStream = new CWhiteboardStream();
        if (m_csPageDimension.IsEmpty())
            m_csPageDimension = _T("720x540");
        pWhiteboardStream->SetPageDimension(m_csPageDimension);
        pWhiteboardStream->SetObjFilename(m_csObjFilename);
        pWhiteboardStream->SetEvqFilename(m_csEvqFilename);
        if (bOpen) {
            HRESULT hrIgnored = pWhiteboardStream->Open();
            _ASSERT(SUCCEEDED(hrIgnored));
        }
        return pWhiteboardStream;
    }

    return NULL;
}

CMetadataStream *CLrdFile::CreateMetadata() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CMetadataStream *pMetadata = NULL;
    if (!m_csMetadataFilename.IsEmpty()) {
        pMetadata = new CMetadataStream();
    }

    if (pMetadata != NULL) {
        pMetadata->SetFilename(m_csMetadataFilename);
        // Initialize metadata stream with information from lrd
        pMetadata->ParseFrom(&m_lrdSgml);
        // Open lmd, overwrite metainfo readed from lrd file
        pMetadata->Open();
    }

    return pMetadata;
}

// BUGFIX 5108: fixed error handling - return bool result (was void)
bool CLrdFile::Write(LPCTSTR szFilename)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return m_lrdSgml.Write(szFilename);
}

// needed only for LPD publishing 
bool CLrdFile::PutNavigationStates(int *anNavigationStates) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // Set the member variable
    for (int i = 0; i < NAVIGATION_STATES; ++i)
        m_anNavigationStates[i] = anNavigationStates[i];

    // Add the 'standard navigation' states to the SGML structure

    // Get the root node (<DOCUMENT> tag)
    SgmlElement *pRootTag = m_lrdSgml.GetRoot();
    bool success = (NULL != pRootTag);
    if (!success)
        return false;

    // Look for "<NAVIGATION>" tag:
    // if it exists, clear it - otherwise create a new one
    SgmlElement *pNavigationTag = m_lrdSgml.Find((LPCTSTR)CS_NAVIGATION_NODE, _T("DOCUMENT"));
    if (NULL != pNavigationTag)
        pNavigationTag->Clear();
    else
        pNavigationTag = new SgmlElement();
    pNavigationTag->SetName(CS_NAVIGATION_NODE);

    // Add subnodes to <NAVIGATION>
    for (int i = 0; i < NAVIGATION_STATES; ++i) {
        SgmlElement *pSubNavigationTag = new SgmlElement();
        pSubNavigationTag->SetName(CS_NAVIGATION_SUBNODES[i]);
        CString csNavigationState = CS_NAVIGATION_STATES[anNavigationStates[i]];
        pSubNavigationTag->SetParameter(csNavigationState);
        pNavigationTag->Append(pSubNavigationTag);
    }

    // Finally:
    // Append <NAVIGATION> in root
    pRootTag->Append(pNavigationTag);

    return true;
}

// PZI: returns true if document contains pure screengrabbing only 
bool CLrdFile::IsDenverDocument()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool isDenver = false;
    SgmlElement *typeTag = m_lrdSgml.Find(_T("TYPE"), _T("INFO"));
    if(typeTag && _tcscmp(typeTag->GetParameter(), _T("denver")) == 0)
        isDenver = true;         

    return isDenver;
}

// Returns true if document will be displayed in simulation mode
bool CLrdFile::IsSimulationDocument()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool isSimulation = false;
    SgmlElement *typeTag = m_lrdSgml.Find(_T("VIEWTYPE"), _T("INFO"));
    if(typeTag && _tcscmp(typeTag->GetParameter(), _T("simulation")) == 0)
        isSimulation = true;         

    return isSimulation;
}

// PZI: set whiteboard helper values; adds whiteboard helper if helper does not exist
bool CLrdFile::SetWhiteboardHelper(CString evqFilename, CString objFilename, int resolutionWidth, int resolutionHeight) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // remove TYPE DENVER (pure screengrabbing)
    SgmlElement *typeTag = m_lrdSgml.Find(_T("TYPE"), _T("INFO"));
    if(typeTag && _tcscmp(typeTag->GetParameter(), _T("denver")) == 0)  {
        if (typeTag->GetParent())
            typeTag->GetParent()->RemoveElement(typeTag);
    }

    m_lrdSgml.CreateWhiteboardHelper(evqFilename, objFilename, resolutionWidth, resolutionHeight);
  
    return true;
}

// PZI: set metadata filename (in member variable and SGML)
void CLrdFile::SetMetadataFilename(LPCTSTR metadataFilename) { 
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // update SGML: filename without path
   m_lrdSgml.ChangeMetaDataFileName(metadataFilename);

   // update member
   m_csMetadataFilename.Format(_T("%s\\%s"), m_csRecordPath, metadataFilename);

}

// PZI: set audio filename (in member variable and SGML)
void CLrdFile::SetAudioFilename(LPCTSTR audioFilename)
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // update SGML: filename without path
   m_lrdSgml.ChangeAudioFileName(audioFilename);

   // update member
   m_csAudioFilename.Format(_T("%s\\%s"), m_csRecordPath, audioFilename);

}

// PZI: set video filename (in member variable and SGML)
// note: will not video entry if not existent
void CLrdFile::SetVideoFilename(LPCTSTR tszVideoFilename) { 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // update SGML: filename without path
    m_lrdSgml.ChangeVideoFileName(tszVideoFilename);

    // update member
    m_csVideoFilename.Format(_T("%s\\%s"), m_csRecordPath, tszVideoFilename);

}

// GG: set still image filename (in member variable and SGML)
// note: will not still image entry if not existent
void CLrdFile::SetStillImageName(LPCTSTR tszStillImageName) { 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // update SGML: filename without path
    m_lrdSgml.ChangeStillImageName(tszStillImageName);

    // update member
    m_csStillImageFilename.Format(_T("%s\\%s"), m_csRecordPath, tszStillImageName);
}

// PZI: set EVQ filename (in member variable and SGML)
// note: will not add whiteboard helper if not existent
void CLrdFile::SetEvqFilename(LPCTSTR evqFilename) { 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    //// update SGML: filename without path
    m_lrdSgml.ChangeEvqFileName(evqFilename);

    // update member
    m_csEvqFilename.Format(_T("%s\\%s"), m_csRecordPath, evqFilename);
}

// PZI: set OBJ filename (in member variable and SGML)
// note: will not add whiteboard helper if not existent
void CLrdFile::SetObjFilename(LPCTSTR objFilename) { 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    //// update SGML: filename without path
    m_lrdSgml.ChangeObjFileName(objFilename);

    // update member
    m_csObjFilename.Format(_T("%s\\%s"), m_csRecordPath, objFilename);

}

// PZI: set clip filename of clip at specified position (in member variable and SGML) - starting at position 1
void CLrdFile::SetClipFilename(int iClipPosition, LPCTSTR csClipFilename) { 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // update SGML (first clip has position 0)
    if (iClipPosition > 0)
        m_lrdSgml.ChangeClipFilename(csClipFilename, iClipPosition-1);
}

// PZI: setter/getter for clip structuring process status
// clipNr starting at 1 for first clip
// setter will manipulate SGML
void CLrdFile::SetClipStructureAvailable(int clipNr, bool available) {
   SetClipFlagAvailable(clipNr, _T("STRUCTURE"), available);
}
bool CLrdFile::GetClipStructureAvailable(int clipNr) {
   return GetClipFlagAvailable(clipNr, _T("STRUCTURE"));
}
void CLrdFile::SetClipImagesAvailable(int clipNr, bool available) {
   SetClipFlagAvailable(clipNr, _T("IMAGES"), available);
}
bool CLrdFile::GetClipImagesAvailable(int clipNr) {
   return GetClipFlagAvailable(clipNr, _T("IMAGES"));
}
void CLrdFile::SetClipFulltextAvailable(int clipNr, bool available) {
   SetClipFlagAvailable(clipNr, _T("FULLTEXT"), available);
}
bool CLrdFile::GetClipFulltextAvailable(int clipNr) {
   return GetClipFlagAvailable(clipNr, _T("FULLTEXT"));
}

bool CLrdFile::GetClipFlagAvailable(int iClipNr, LPCTSTR tszFlagName) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return m_lrdSgml.GetClipFlagAvailable(tszFlagName, iClipNr);

    //not found means not available    
    return false;
}

int CLrdFile::GetNumberOfClips() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    int iClipCount = m_lrdSgml.GetNumberOfClips();

    return iClipCount;
}

HRESULT CLrdFile::CopyDocumentTo(CString csTargetLrd, ExportProgressListener *pProgress) { 

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    // Source path (ending with backslahs)
    CString csOldLrdPath = m_csRecordPath;
    if (csOldLrdPath[m_csRecordPath.GetLength()-1] != _T('\\'))
        csOldLrdPath += _T("\\");

    // Target path (ending with backslahs)
    CString csNewLrdPath = csTargetLrd;
    StringManipulation::GetPath(csNewLrdPath);
    if (csNewLrdPath[csNewLrdPath.GetLength()-1] != _T('\\'))
        csNewLrdPath += _T("\\");

    // Target Lrd prefix (used to create new filenames)
    CString csNewLrdPrefix = csTargetLrd;
    StringManipulation::GetFilePrefix(csNewLrdPrefix);

    // Initialize progress dialog
    UINT nCurrentStep = 0;
    if (pProgress) {
        CString csLabel;
        csLabel.LoadString(IDS_EXPORT_INITIALIZE);
        pProgress->SetOverallLabel(csLabel);
        UINT nProgressSteps = 4; // INITIALIZE, AUDIO, METADATA, FILE UPDATE
        if (!m_csEvqFilename.IsEmpty() && !m_csObjFilename.IsEmpty())
            ++nProgressSteps;
        if (!m_csVideoFilename.IsEmpty() || !m_csStillImageFilename.IsEmpty())
            ++nProgressSteps;
        if (!m_aClipFilenames.GetSize() == 0)
            ++nProgressSteps;
        pProgress->SetOverallSteps(nProgressSteps);
        pProgress->SetOverallProgress(0);
    }

    if (pProgress) {
        CString csLabel;
        csLabel.LoadString(IDS_EXPORT_INITIALIZE);
        pProgress->SetOverallLabel(csLabel);
        pProgress->SetParticularProgress(0);
    }

    CArray<CopyFileStruct *, CopyFileStruct *> aCopyFiles;
    hr = FillCopyFilesStruct(csOldLrdPath, csNewLrdPath, csNewLrdPrefix, aCopyFiles, pProgress);

    if (SUCCEEDED(hr) && pProgress) {
        pProgress->SetParticularProgress(100);
        ++nCurrentStep;
        pProgress->SetOverallProgress(nCurrentStep);
    }

    // WB
    if (SUCCEEDED(hr) && !m_csEvqFilename.IsEmpty() && !m_csObjFilename.IsEmpty()) {
        hr = CopyFilesOfType(aCopyFiles, pProgress, CopyFileStruct::WB);
        ++nCurrentStep;
        if (SUCCEEDED(hr) && pProgress)
            pProgress->SetOverallProgress(nCurrentStep);
    }

    // METADATA
    if (SUCCEEDED(hr) && !m_csMetadataFilename.IsEmpty()) {
        hr = CopyFilesOfType(aCopyFiles, pProgress, CopyFileStruct::METADATA);
        ++nCurrentStep;
        if (SUCCEEDED(hr) && pProgress)
            pProgress->SetOverallProgress(nCurrentStep);
    }

    // AUDIO
    if (SUCCEEDED(hr) && !m_csAudioFilename.IsEmpty()) {
        hr = CopyFilesOfType(aCopyFiles, pProgress, CopyFileStruct::AUDIO);
        ++nCurrentStep;
        if (SUCCEEDED(hr) && pProgress)
            pProgress->SetOverallProgress(nCurrentStep);
    }
    
    // CLIPS
    if (SUCCEEDED(hr) && !m_aClipFilenames.GetSize() == 0) {
        hr = CopyFilesOfType(aCopyFiles, pProgress, CopyFileStruct::CLIPS);
        ++nCurrentStep;
        if (SUCCEEDED(hr) && pProgress)
            pProgress->SetOverallProgress(nCurrentStep);
    }

    // VIDEO
    if (SUCCEEDED(hr) && (!m_csVideoFilename.IsEmpty() || !m_csStillImageFilename.IsEmpty())) {
        hr = CopyFilesOfType(aCopyFiles, pProgress, CopyFileStruct::VIDEO);
        ++nCurrentStep;
        if (SUCCEEDED(hr) && pProgress)
            pProgress->SetOverallProgress(nCurrentStep);
    }

    if (SUCCEEDED(hr) && pProgress) {
        pProgress->SetOverallLabel(IDS_EXPORT_POSTPROCESS);
        pProgress->SetParticularLabel(_T(""));
    }

    if (SUCCEEDED(hr))
        SetRecordPath(csNewLrdPath);

    if (SUCCEEDED(hr))
        hr = ChangeTags(aCopyFiles);
    
    if (SUCCEEDED(hr) && pProgress)
        pProgress->SetParticularProgress(30);

    if (SUCCEEDED(hr) && !m_csObjFilename.IsEmpty())
        hr = AdaptRelativePathsInObj(csOldLrdPath, csNewLrdPath, csNewLrdPrefix);
    
    if (SUCCEEDED(hr))
        hr = AdaptThumbEntries(csNewLrdPrefix);

    if (SUCCEEDED(hr) && pProgress)
        pProgress->SetParticularProgress(70);

    if (SUCCEEDED(hr)) 
        hr = m_lrdSgml.Write(csTargetLrd, false);

    if (SUCCEEDED(hr) && pProgress) {
        pProgress->SetParticularProgress(100);
        ++nCurrentStep;
        pProgress->SetOverallProgress(nCurrentStep);
    }

    for (int i = 0; i < aCopyFiles.GetSize(); ++i)
        delete aCopyFiles[i];
    aCopyFiles.RemoveAll();

    return hr;
}

HRESULT CLrdFile::DeleteFiles() { 

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    CArray<CString, CString> aDeleteFiles;
    hr = FillFileArray(aDeleteFiles);

    for (int i = 0; i < aDeleteFiles.GetSize(); ++i) {
       //TRACE(_T("DELETE %ls : %i\n"), aDeleteFiles[i], ::DeleteFile(aDeleteFiles[i]));
       ::DeleteFile(aDeleteFiles[i]);
    }
    aDeleteFiles.RemoveAll();

    return hr;
}

void CLrdFile::SetClipFlagAvailable(int iClipNr, LPCTSTR tszFlagName, bool bAvailable) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_lrdSgml.SetClipFlagAvailable(tszFlagName, iClipNr, bAvailable);
}

// PZI: get filename prefix used to create other filenames
CString CLrdFile::GetFilenamePrefix() {
   // get filename prefix for thumbnail filenames
   CString filenamePrefix = GetFilename();
   StringManipulation::GetFilePrefix(filenamePrefix);
   CString csSgmlPrefix;
   StringManipulation::TransformForSgml(filenamePrefix, csSgmlPrefix);
   return csSgmlPrefix;
}

HRESULT CLrdFile::ChangeTags(CArray<CopyFileStruct *, CopyFileStruct *> &aCopyFiles) {
    HRESULT hr = S_OK;

    // change tag in lrd
    if (SUCCEEDED(hr)) {
        for (int i = 0; i < aCopyFiles.GetSize(); ++i) {
            if (aCopyFiles[i]->GetSourceFileName() == m_csAudioFilename || 
                aCopyFiles[i]->GetSourceFileName() == m_csVideoFilename || 
                aCopyFiles[i]->GetSourceFileName() == m_csStillImageFilename || 
                aCopyFiles[i]->GetSourceFileName() == m_csMetadataFilename || 
                aCopyFiles[i]->GetSourceFileName() == m_csEvqFilename || 
                aCopyFiles[i]->GetSourceFileName() == m_csObjFilename) {

                // backslash special characters in file name for sgml
                CString csFileEntry = aCopyFiles[i]->GetTargetFileName();
                StringManipulation::GetFilename(csFileEntry);
                CString csSgmlFileEntry;
                StringManipulation::TransformForSgml(csFileEntry, csSgmlFileEntry);

                // Change entry in lrd file
                if (aCopyFiles[i]->GetSourceFileName() == m_csAudioFilename)
                    SetAudioFilename(csSgmlFileEntry);
                else if (aCopyFiles[i]->GetSourceFileName() == m_csVideoFilename)
                    SetVideoFilename(csSgmlFileEntry);
                else if (aCopyFiles[i]->GetSourceFileName() == m_csStillImageFilename)
                    SetStillImageName(csSgmlFileEntry);
                else if (aCopyFiles[i]->GetSourceFileName() == m_csMetadataFilename)
                    SetMetadataFilename(csSgmlFileEntry);
                else if (aCopyFiles[i]->GetSourceFileName() == m_csEvqFilename)
                    SetEvqFilename(csSgmlFileEntry);
                else if (aCopyFiles[i]->GetSourceFileName() == m_csObjFilename)
                    SetObjFilename(csSgmlFileEntry);

            } else if (aCopyFiles[i]->GetType() == CopyFileStruct::CLIPS) {

                for (int j = 0; j < m_aClipFilenames.GetSize(); ++j) {
                    CString csClipFileName = aCopyFiles[i]->GetSourceFileName();
                    StringManipulation::GetFilename(csClipFileName);
                    CString csClipFileName2 = m_aClipFilenames[j];
                    StringManipulation::GetFilename(csClipFileName2);
                    if (csClipFileName == csClipFileName2) {

                        // backslash special characters in file name for sgml
                        CString csFileEntry = aCopyFiles[i]->GetTargetFileName();
                        StringManipulation::GetFilename(csFileEntry);
                        CString csSgmlFileEntry;
                        StringManipulation::TransformForSgml(csFileEntry, csSgmlFileEntry);

                        // Change entry in lrd file
                        SetClipFilename(j+1, csSgmlFileEntry);

                        break;
                    }
                }
            }
        }
    }

    return hr;
}

HRESULT CLrdFile::AdaptRelativePathsInObj(CString csOldLrdPath, CString csNewLrdPath, CString csNewLrdPrefix) {
    HRESULT hr = S_OK;

    if (m_csObjFilename.IsEmpty() || _taccess(m_csObjFilename, 04) != 0)
        hr = E_EXPORT_ACCESS_ERROR;

    // Read sgml structure
    SgmlFile objFile;
    if (SUCCEEDED(hr)) {
        bool bSuccess = objFile.Read(m_csObjFilename);
        if (!bSuccess)
            hr = E_EXPORT_READ_WB_ERROR;
    }

    // Search relative paths and adapt if necessary
    bool bObjFileChanged = false;
    if (SUCCEEDED(hr)) 
        hr = AdaptRelativePathsInObj(objFile, csOldLrdPath, csNewLrdPath, bObjFileChanged);

    // Write object file
    if (SUCCEEDED(hr) && bObjFileChanged) {
        CString csNewObjFileName = csNewLrdPath + csNewLrdPrefix + _T(".obj");
        bool bSuccess = objFile.Write(csNewObjFileName, true);
        if (!bSuccess)
            hr = E_EXPORT_WRITE_WB_ERROR;
    }

    return hr;
}

HRESULT CLrdFile::AdaptRelativePathsInObj(SgmlFile &objFile, LPCTSTR tszOldlrdPath, 
                                       LPCTSTR tszNewLrdPath, bool &bSomethingChanged) {
    HRESULT hr = S_OK;

    bSomethingChanged = false;

    SgmlElement *pRoot = objFile.GetRoot();
    if (!pRoot)
        hr = E_EXPORT_READ_WB_ERROR;

    // find the path parameter in the file tag (sub tag of QUESTIONNAIRE)
    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        pRoot->FindAll(_T("file"), aElements);

        for (int i = 0; i < aElements.GetSize(); ++i) {
            SgmlElement *pFileTag = aElements[i];
            CString csRelativePath = pFileTag->GetValue(_T("path"));
            if (!csRelativePath.IsEmpty()) {
                // Make both (old and new) paths relative
                CString csOldAbsolutePath = 
                    StringManipulation::MakeAbsolute(tszOldlrdPath, csRelativePath);
                CString csNewRelativePath = 
                    StringManipulation::MakeRelative(tszNewLrdPath, csOldAbsolutePath);
                // Compare old and new path, change if differs
                if (csNewRelativePath != csRelativePath) {
                    pFileTag->SetAttribute(_T("path"), csNewRelativePath);
                    bSomethingChanged = true;
                }
            }
        }
    }

    return hr;
}
HRESULT CLrdFile::AdaptThumbEntries(CString csNewLrdPrefix) {
    SgmlFile *pLmdFile = new SgmlFile();
    if (pLmdFile == NULL)
        return E_FAIL;

    bool bSuccess = pLmdFile->Read(m_csMetadataFilename);
    if (!bSuccess)
        return E_FAIL;

    CArray<SgmlElement *, SgmlElement *> aThumbs;
    if (pLmdFile->GetRoot() != NULL)
        pLmdFile->GetRoot()->FindAll(_T("thumb"), aThumbs);

    CString csOldLrdPrefix = m_csFilename;
    StringManipulation::GetFilePrefix(csOldLrdPrefix);
    for (int i = 0; i < aThumbs.GetSize(); ++i) {
        if (aThumbs[i] != NULL) {
            CString csParameter = aThumbs[i]->GetParameter();
            csParameter.Replace(csOldLrdPrefix, csNewLrdPrefix);
            aThumbs[i]->SetParameter(csParameter);
        }
    }

    pLmdFile->Write();

    delete pLmdFile;

    return S_OK;
}

HRESULT CLrdFile::FillCopyFilesStruct(CString csOldLrdPath, CString csNewLrdPath, CString csNewLrdPrefix, 
                                   CArray<CopyFileStruct *, CopyFileStruct *> &aCopyFiles, ExportProgressListener *pProgress) { 
    HRESULT hr = S_OK;

    // LRD file
    if (SUCCEEDED(hr) && !m_csFilename.IsEmpty()) {
        CopyFileStruct *pLrdFile = new CopyFileStruct();
        hr = pLrdFile->Fill(csOldLrdPath, csNewLrdPath, csNewLrdPrefix, 
                            m_csFilename, CopyFileStruct::WB);
        if (SUCCEEDED(hr))
            aCopyFiles.Add(pLrdFile);
    }

    // EVQ file
    if (SUCCEEDED(hr) && !m_csEvqFilename.IsEmpty()) {
        CopyFileStruct *pEvqFile = new CopyFileStruct();
        hr = pEvqFile->Fill(csOldLrdPath, csNewLrdPath, csNewLrdPrefix, 
                            m_csEvqFilename, CopyFileStruct::WB);
        if (SUCCEEDED(hr))
            aCopyFiles.Add(pEvqFile);
    }
    

    // OBJ file
    if (SUCCEEDED(hr) && !m_csObjFilename.IsEmpty()) {
        CopyFileStruct *pObjFile = new CopyFileStruct();
        hr = pObjFile->Fill(csOldLrdPath, csNewLrdPath, csNewLrdPrefix, 
                            m_csObjFilename, CopyFileStruct::WB);
        if (SUCCEEDED(hr))
            aCopyFiles.Add(pObjFile);
    }

    if (pProgress)
        pProgress->SetParticularProgress(25);

    // Read object file
    SgmlFile objFile;
    if (SUCCEEDED(hr) && !m_csObjFilename.IsEmpty()) {

        // obj file has to exist
        if (_taccess(m_csObjFilename, 04) != 0)
            hr = E_EXPORT_ACCESS_WB_ERROR;

        if (SUCCEEDED(hr)) {
            // search for files to copy in obj
            bool bReadSuccess = objFile.Read(m_csObjFilename, _T("<WB"));
            if (!bReadSuccess)
                hr = E_EXPORT_READ_WB_ERROR;
        }
    }

    // Images (from pages and tests)
    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        SgmlElement *pRoot = objFile.GetRoot();
        if (pRoot) {
            pRoot->FindAll(_T("IMAGE"), aElements);
            for (int i = 0; i < aElements.GetSize() && SUCCEEDED(hr); ++i) {
                CString csImageName = aElements[i]->GetValue(_T("fname"));
                if (!csImageName.IsEmpty()) {
                    CopyFileStruct *pImageFile = new CopyFileStruct();
                    hr = pImageFile->Fill(csOldLrdPath, csNewLrdPath, NULL, 
                        csImageName, CopyFileStruct::WB);
                    if (SUCCEEDED(hr))
                        aCopyFiles.Add(pImageFile);
                }
            }
        }
    }

    // Files (file open action)
    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        SgmlElement *pRoot = objFile.GetRoot();
        if (pRoot) {
            pRoot->FindAll(_T("BUTTON"), aElements);
            for (int i = 0; i < aElements.GetSize() && SUCCEEDED(hr); ++i) {
                CString csAction = aElements[i]->GetValue(_T("onClick"));
                CString csFileName;
                csFileName.Empty();
                if (csAction.Find(_T("open-file")) >= 0)
                    csFileName = csAction.Right(csAction.GetLength() - 10);
                if (!csFileName.IsEmpty()) {
                    CopyFileStruct *pFileFile = new CopyFileStruct();
                    hr = pFileFile->Fill(csOldLrdPath, csNewLrdPath, NULL, 
                        csFileName, CopyFileStruct::WB);
                    if (SUCCEEDED(hr))
                        aCopyFiles.Add(pFileFile);
                }
            }
        }
    }
    
    if (pProgress)
        pProgress->SetParticularProgress(50);

    // Audio file
    if (SUCCEEDED(hr) && !m_csAudioFilename.IsEmpty()) {
        CopyFileStruct *pAudioFile = new CopyFileStruct();
        hr = pAudioFile->Fill(csOldLrdPath, csNewLrdPath, csNewLrdPrefix, 
                              m_csAudioFilename, CopyFileStruct::AUDIO);
        if (SUCCEEDED(hr))
            aCopyFiles.Add(pAudioFile);
    }

    // Video file
    if (SUCCEEDED(hr) && !m_csVideoFilename.IsEmpty()) {
        CopyFileStruct *pVideoFile = new CopyFileStruct();
        hr = pVideoFile->Fill(csOldLrdPath, csNewLrdPath, csNewLrdPrefix, 
                              m_csVideoFilename, CopyFileStruct::VIDEO);
        if (SUCCEEDED(hr))
            aCopyFiles.Add(pVideoFile);
    }

    // Still image file
    if (SUCCEEDED(hr) && !m_csStillImageFilename.IsEmpty()) {
        CString csStillImagePrefix;
        csStillImagePrefix.Format(_T("%s_still"), csNewLrdPrefix);
        CopyFileStruct *pStillImageFile = new CopyFileStruct();
        hr = pStillImageFile->Fill(csOldLrdPath, csNewLrdPath, csStillImagePrefix, 
                                   m_csStillImageFilename, CopyFileStruct::VIDEO);
        if (SUCCEEDED(hr))
            aCopyFiles.Add(pStillImageFile);
    }
    
    // Metadata file
    if (SUCCEEDED(hr) && !m_csMetadataFilename.IsEmpty()){
        bool bSuccess = true;

        // Get thumb filenames
        SgmlFile *pLmdFile = new SgmlFile();
        if (pLmdFile == NULL)
            bSuccess = false;
        if (bSuccess) 
            bSuccess = pLmdFile->Read(m_csMetadataFilename);
        if (bSuccess) {
            CArray<SgmlElement *, SgmlElement *> aThumbs;
            if (pLmdFile->GetRoot() != NULL)
                pLmdFile->GetRoot()->FindAll(_T("thumb"), aThumbs);
            for (int i = 0; i < aThumbs.GetSize(); ++i) {
                CString csOrigFilename = aThumbs[i]->GetParameter();

                CString csOldLrdPrefix = m_csFilename;
                StringManipulation::GetFilePrefix(csOldLrdPrefix);

                CString csNewPrefix = csOrigFilename;
                StringManipulation::GetFilePrefix(csNewPrefix);
                csNewPrefix.Replace(csOldLrdPrefix, csNewLrdPrefix);

                CString csThumb;
                CString csThumbPrefix;

                int aSizes[3] = {100, 150, 200};
                for (int j = 0; j < 3; ++j) {
                    csThumb.Format(_T("%d_%s"), aSizes[j], csOrigFilename);
                    csThumbPrefix.Format(_T("%d_%s"), aSizes[j], csNewPrefix);
                    CopyFileStruct *pThumbFile = new CopyFileStruct();
                    hr = pThumbFile->Fill(csOldLrdPath, csNewLrdPath, csThumbPrefix, 
                        csThumb, CopyFileStruct::METADATA);
                    if (SUCCEEDED(hr))
                        aCopyFiles.Add(pThumbFile);
                    else
                        delete pThumbFile;
                }
            }
            aThumbs.RemoveAll();
        }

        if (pLmdFile != NULL)
            delete pLmdFile;
        CopyFileStruct *pMetadataFile = new CopyFileStruct();
        hr = pMetadataFile->Fill(csOldLrdPath, csNewLrdPath, csNewLrdPrefix, 
                                 m_csMetadataFilename, CopyFileStruct::METADATA);
        if (SUCCEEDED(hr))
            aCopyFiles.Add(pMetadataFile);
    }
    
    if (pProgress)
        pProgress->SetParticularProgress(75);

    // Screengrabbing Clips
    if (SUCCEEDED(hr) && !m_aClipFilenames.IsEmpty()) {
        for (int i = 0; i < m_aClipFilenames.GetSize() && SUCCEEDED(hr); ++i) {

            // Create new clip name
            CString csClipName = m_aClipFilenames[i];
            StringManipulation::GetFilename(csClipName);
            CString csNewClipPrefix = csClipName;
            int iPos = csClipName.Find(_T("_clip_"));
            if (iPos > 0)
                csNewClipPrefix = csNewLrdPrefix + csClipName.Right(csClipName.GetLength() - iPos);
            StringManipulation::GetFilePrefix(csNewClipPrefix);

            CopyFileStruct *pClipFile = new CopyFileStruct();
            hr = pClipFile->Fill(csOldLrdPath, csNewLrdPath, csNewClipPrefix, 
                                 csClipName, CopyFileStruct::CLIPS);
            if (SUCCEEDED(hr))
                aCopyFiles.Add(pClipFile);
        }
    }

    return hr;
}

HRESULT CLrdFile::FillFileArray(CArray<CString, CString> &aFiles) { 
    HRESULT hr = S_OK;

    // LRD file
    if (SUCCEEDED(hr) && !m_csFilename.IsEmpty()) {
        aFiles.Add(m_csRecordPath + _T("\\") + m_csFilename);
        // May be there is a backup
        aFiles.Add(m_csRecordPath + _T("\\") + m_csFilename + _T(".orig"));
    }

    // EVQ file
    if (SUCCEEDED(hr) && !m_csEvqFilename.IsEmpty()) {
        aFiles.Add(m_csEvqFilename);
    }
    
    // OBJ file
    if (SUCCEEDED(hr) && !m_csObjFilename.IsEmpty()) {
        aFiles.Add(m_csObjFilename);
    }

    // Read object file
    SgmlFile objFile;
    if (SUCCEEDED(hr) && !m_csObjFilename.IsEmpty()) {

        // obj file has to exist
        if (_taccess(m_csObjFilename, 04) != 0)
            hr = E_EXPORT_ACCESS_WB_ERROR;

        if (SUCCEEDED(hr)) {
            // search for files to copy in obj
            bool bReadSuccess = objFile.Read(m_csObjFilename, _T("<WB"));
            if (!bReadSuccess)
                hr = E_EXPORT_READ_WB_ERROR;
        }
    }

    // Images (from pages and tests)
    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        SgmlElement *pRoot = objFile.GetRoot();
        if (pRoot) {
            pRoot->FindAll(_T("IMAGE"), aElements);
            for (int i = 0; i < aElements.GetSize() && SUCCEEDED(hr); ++i) {
                CString csImageName = aElements[i]->GetValue(_T("fname"));
                if (!csImageName.IsEmpty()) {
                    aFiles.Add(m_csRecordPath + _T("\\") + csImageName);
                }
            }
        }
    }

    // Files (file open action)
    if (SUCCEEDED(hr)) {
        CArray<SgmlElement *, SgmlElement *> aElements;
        SgmlElement *pRoot = objFile.GetRoot();
        if (pRoot) {
            pRoot->FindAll(_T("BUTTON"), aElements);
            for (int i = 0; i < aElements.GetSize() && SUCCEEDED(hr); ++i) {
                CString csAction = aElements[i]->GetValue(_T("onClick"));
                CString csFileName;
                csFileName.Empty();
                if (csAction.Find(_T("open-file")) >= 0)
                    csFileName = csAction.Right(csAction.GetLength() - 10);
                if (!csFileName.IsEmpty()) {
                    aFiles.Add(m_csRecordPath + _T("\\") + csFileName);
                }
            }
        }
    }
    
    // Audio file
    if (SUCCEEDED(hr) && !m_csAudioFilename.IsEmpty()) {
        aFiles.Add(m_csAudioFilename);
    }

    // Video file
    if (SUCCEEDED(hr) && !m_csVideoFilename.IsEmpty()) {
        aFiles.Add(m_csVideoFilename);
    }

    // Still image file
    if (SUCCEEDED(hr) && !m_csStillImageFilename.IsEmpty()) {
        aFiles.Add(m_csStillImageFilename);
    }
    
    // Metadata file
    if (SUCCEEDED(hr) && !m_csMetadataFilename.IsEmpty()){
        aFiles.Add(m_csMetadataFilename);
        // May be there is a backup
        aFiles.Add(m_csMetadataFilename + _T(".orig"));
    }
    // Thumbs in Metadata file
    if (SUCCEEDED(hr) && !m_csMetadataFilename.IsEmpty()) {
        SgmlFile lmdFile;
        bool bSuccess = lmdFile.Read(m_csMetadataFilename, _T("<doc"));
        if (bSuccess) {
            CArray<SgmlElement *, SgmlElement *> aElements;
            SgmlElement *pRoot = lmdFile.GetRoot();
            if (pRoot != NULL) {
                pRoot->FindAll(_T("thumb"), aElements);
                for (int i = 0; i < aElements.GetSize() && SUCCEEDED(hr); ++i) {
                    CString csFileName = aElements[i]->GetParameter();
                    if (!csFileName.IsEmpty()) {
                        aFiles.Add(m_csRecordPath + _T("\\") + csFileName);
                        // There may be three different files
                        aFiles.Add(m_csRecordPath + _T("\\100_") + csFileName);
                        aFiles.Add(m_csRecordPath + _T("\\150_") + csFileName);
                        aFiles.Add(m_csRecordPath + _T("\\200_") + csFileName);
                    }
                }
            }
        }
    }
    
    // Screengrabbing Clips
    if (SUCCEEDED(hr) && !m_aClipFilenames.IsEmpty()) {
        for (int i = 0; i < m_aClipFilenames.GetSize() && SUCCEEDED(hr); ++i) {
            // BUGFIX 5709: avi file not deleted
            // since bugfix 5538 m_aClipFilenames[i] includes path - do not add path here
            aFiles.Add(m_aClipFilenames[i]);
            // There may be additional files with the ending ".sge"
            CString csSgeFile = m_aClipFilenames[i];
            StringManipulation::GetPathFilePrefix(csSgeFile);
            csSgeFile += _T(".sge");
            aFiles.Add(csSgeFile);
            // BUGFIX 5709: delete files with ending ".sga"
            CString csSgaFile = m_aClipFilenames[i];
            StringManipulation::GetPathFilePrefix(csSgaFile);
            csSgaFile += _T(".sga");
            aFiles.Add(csSgaFile);

            // BUGFIX 5709: delete png file related to clip
            //CString csDirectory = m_csRecordPath + _T("\\");
            //StringManipulation::GetPath(csDirectory);
            CString csClipPrefix = m_aClipFilenames[i];
            StringManipulation::GetFilePrefix(csClipPrefix);
            WIN32_FIND_DATA FindFileData;
            HANDLE hFind = FindFirstFile(m_csRecordPath + _T("\\*"), &FindFileData);

            if (hFind != INVALID_HANDLE_VALUE && !csClipPrefix.IsEmpty()) 
            {
               while (FindNextFile(hFind, &FindFileData) != 0) 
               {
                  CString csFile(FindFileData.cFileName);
                  if(csFile.Left(csClipPrefix.GetLength()) == csClipPrefix && csFile.Right(4) == _T(".png")) {
                     // delete matching PNG file
                     aFiles.Add(m_csRecordPath + _T("\\") + FindFileData.cFileName);
                  }
               }
               // close handle
               FindClose(hFind);
            }
        }
    }

    // Additional files, created during screen grabbing structure process, 
    // and not deleted because of process cancel
    aFiles.Add(m_csRecordPath + _T("\\_internal_thumbinfo"));
    aFiles.Add(m_csRecordPath + _T("\\ocr.txt"));
    aFiles.Add(m_csRecordPath + _T("\\ocr.bmp"));

    return hr;
}

DWORD CALLBACK CopyProgressRoutine(LARGE_INTEGER TotalFileSize, LARGE_INTEGER TotalBytesTransferred,
                                   LARGE_INTEGER StreamSize, LARGE_INTEGER StreamBytesTransferred,
                                   DWORD dwStreamNumber, DWORD dwCallbackReason,
                                   HANDLE hSourceFile, HANDLE hDestinationFile,
                                   LPVOID lpData) {
    CopyProgressStruct *pCopyProgressStruct = (CopyProgressStruct *)lpData;

    if (pCopyProgressStruct) {

        UINT nPercent = (int)(((double)(pCopyProgressStruct->m_nByteOffset + TotalBytesTransferred.QuadPart) /
            (double)pCopyProgressStruct->m_nCompleteFileSize) * 100);

        if (pCopyProgressStruct->m_pExportProgress) {
            pCopyProgressStruct->m_pExportProgress->SetParticularProgress(nPercent);
            if (pCopyProgressStruct->m_pExportProgress->ExportWasCancelled())
                return PROGRESS_CANCEL;
        }
    }

    // Continue the file copy. It can also be stopped.
    return PROGRESS_CONTINUE;
}

HRESULT CLrdFile::CopyFileWithProgress(CopyFileStruct *pFileToCopy, __int64 nTotalSize, __int64 nBytesOffset, ExportProgressListener *pProgress) {
    HRESULT hr = S_OK;

    CopyProgressStruct copyProgressStruct(pProgress, nTotalSize, nBytesOffset);
    if (pProgress) {
        CString csFilename = pFileToCopy->GetTargetFileName();
        StringManipulation::GetFilename(csFilename);
        pProgress->SetParticularLabel(csFilename);
    }

    BOOL bCancel = FALSE;
    BOOL bCopySuccess = 
        CopyFileEx(pFileToCopy->GetSourceFileName(), pFileToCopy->GetTargetFileName(), 
                   CopyProgressRoutine, (LPVOID)&copyProgressStruct, &bCancel, 0);

    if (bCopySuccess == FALSE) {
        // There is nothing to do if the source file is equal to the target file
        bool bSameFileAnExists =  
            (pFileToCopy->GetSourceFileName() == pFileToCopy->GetTargetFileName() &&
            _taccess(pFileToCopy->GetSourceFileName(), 04) == 0);

        if (!bSameFileAnExists)
            hr = E_EXPORT_COPY_FAILED;
    }


    return hr;
}

HRESULT CLrdFile::CopyFilesOfType(CArray<CopyFileStruct *, CopyFileStruct *> &aCopyFiles, ExportProgressListener *pProgress, UINT nType) {
    HRESULT hr = S_OK;

    if (pProgress) {
        CString csLabel;
        csLabel.Empty();
        if (nType == CopyFileStruct::WB)
            csLabel.LoadString(IDS_EXPORT_WHITEBOARD);
        else if (nType == CopyFileStruct::METADATA)
            csLabel.LoadString(IDS_EXPORT_LMD);
        else if (nType == CopyFileStruct::AUDIO)
            csLabel.LoadString(IDS_EXPORT_AUDIO);
        else if (nType == CopyFileStruct::VIDEO)
            csLabel.LoadString(IDS_EXPORT_VIDEO);
        else if (nType == CopyFileStruct::CLIPS)
            csLabel.LoadString(IDS_EXPORT_CLIPS);
        pProgress->SetOverallLabel(csLabel);
    }

    __int64 nCompleteFilesSize = 0;
    for (int i = 0; i < aCopyFiles.GetSize(); ++i) {
        if (aCopyFiles[i]->GetType() == nType)
            nCompleteFilesSize += aCopyFiles[i]->GetFileSize();
    }

    __int64 nBytesCopied = 0;
    for (int i = 0; i < aCopyFiles.GetSize() && SUCCEEDED(hr); ++i) {
        if (aCopyFiles[i]->GetType() == nType) {
            hr = CopyFileWithProgress(aCopyFiles[i], nCompleteFilesSize, nBytesCopied, pProgress);
            nBytesCopied += aCopyFiles[i]->GetFileSize();
        }
    }

    if (pProgress && pProgress->ExportWasCancelled())
        hr = E_EXPORT_CANCELLED;

    return hr;
}


////********************************************************************////

CopyFileStruct::CopyFileStruct() {

    m_csOldFilename.Empty();
    m_csNewFilename.Empty();
    m_nFileSize = 0;
    m_nType = UNDEFINED;
}

HRESULT CopyFileStruct::Fill(LPCTSTR tszOldLrdPath, LPCTSTR tszNewLrdPath, 
                             LPCTSTR tszNewFilePrefix, LPCTSTR tszFilename, CopyFileType nType) {
    HRESULT hr = S_OK;

    m_csOldFilename = tszFilename;
    if (m_csOldFilename.Find(_T("\\")) < 0)
        m_csOldFilename.Format(_T("%s%s"), tszOldLrdPath, tszFilename);

    if (_taccess(m_csOldFilename, 04) != 0)
        hr = E_EXPORT_ACCESS_ERROR;

    if (SUCCEEDED(hr)) {
        if (tszNewFilePrefix == NULL || _tcslen(tszNewFilePrefix) == 0) {
            CString csOldFilePrefix = tszFilename;
            StringManipulation::GetFilename(csOldFilePrefix);
            StringManipulation::GetFilePrefix(csOldFilePrefix);
            m_csNewFilename.Format(_T("%s%s"), tszNewLrdPath, tszFilename);
        } else {
            CString csFileSuffix = tszFilename;
            StringManipulation::GetFileSuffix(csFileSuffix);
            m_csNewFilename.Format(_T("%s%s.%s"), tszNewLrdPath, tszNewFilePrefix, csFileSuffix);
        }
    }

    if (SUCCEEDED(hr)) {
        m_nFileSize = LIo::GetFileSize(m_csOldFilename);
        m_nType = nType;
    }

    return hr;
}

