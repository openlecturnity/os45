#include "StdAfx.h"

#include <io.h>
#include <tchar.h>
#include <commctrl.h>

#include "la_presentationinfo.h"
#include "la_project.h"
#include "mlb_misc.h"
#include "la_registry.h"
#include "lutils.h"                 //lutils

#include "..\..\Studio\Resource.h"
#include "..\..\Studio\Studio.h"
#include "..\..\editor\editorDoc.h"
#include "..\..\editor\MainFrm.h"
#include "..\..\Studio\MainFrm.h"

#include "LanguageSupport.h"    // lsutl32
#include "ProfileUtils.h"       // lsutl32
#include "MfcUtils.h"           // lsutl32

#include "ObjSgml.h"            // filesdk


ASSISTANT::PresentationInfo::PresentationInfo()
{
   clipList_ = new ClipList();
   isInitialized_ = false;
   isVideo_ = false;
   thumbs_ = NULL;
   m_bClipIsStructured = false;
   m_bClipIsSearchable = false;
   m_bIsDenverDocument = false;
   iCodepage_ = GetACP();
}

ASSISTANT::PresentationInfo::~PresentationInfo()
{
   if (clipList_)
      delete clipList_;
   clipList_ = NULL;

   if (thumbs_)
      delete thumbs_;
   thumbs_ = NULL;
}

//// Get- and Set-Functions
CString &ASSISTANT::PresentationInfo::GetFilename()
{
   return filename_;
}

void ASSISTANT::PresentationInfo::SetFilename(LPCTSTR filename)
{
   filename_ = filename;
}

int ASSISTANT::PresentationInfo::GetCount()
{
   return presentationCount_;
}

void ASSISTANT::PresentationInfo::SetCount(int presentationCount)
{
   presentationCount_ = presentationCount;
}

bool ASSISTANT::PresentationInfo::IsModified()
{
    bool bModified = false;
    if (!filename_.IsEmpty()) {
        CString csLepFile = filename_;

        StringManipulation::GetPathFilePrefix(csLepFile);
        csLepFile += _T(".lep");

        if (_taccess(csLepFile, 04) == 0)
            bModified = true;
    }

    return bModified;
}

CString &ASSISTANT::PresentationInfo::GetTitle()
{
   return title_;
}

void ASSISTANT::PresentationInfo::SetTitle(LPCTSTR title)
{
   isInitialized_ = true;
   title_ = title;
}

CString &ASSISTANT::PresentationInfo::GetAuthor()
{
   return author_;
}

void ASSISTANT::PresentationInfo::SetAuthor(LPCTSTR author)
{
   isInitialized_ = true;
   author_ = author;
}

CString &ASSISTANT::PresentationInfo::GetCreator()
{
   return creator_;
}

void ASSISTANT::PresentationInfo::SetCreator(LPCTSTR creator)
{
   isInitialized_ = true;
   creator_ = creator;
}

CString &ASSISTANT::PresentationInfo::GetKeywords()
{
   static CString csKeywords;

   CreateStringFromList(csKeywords, keywords_);

   return csKeywords;
}

void ASSISTANT::PresentationInfo::SetKeywords(LPCTSTR tszKeywords) {
    if (_tcslen(tszKeywords) == 0)
        return;

    isInitialized_ = true;
    keywords_.RemoveAll();
    CString csKeywords = tszKeywords;
    CreateListFromString(csKeywords, keywords_);
}

CString &ASSISTANT::PresentationInfo::GetRecorddate()
{
   return recorddate_;
}

void ASSISTANT::PresentationInfo::SetRecorddate(LPCTSTR tszRecorddate) {
    if (_tcslen(tszRecorddate) == 0)
        return;

    isInitialized_ = true;
    recorddate_ = tszRecorddate;
}

CString &ASSISTANT::PresentationInfo::GetRecordtime()
{
   return recordtime_;
}

void ASSISTANT::PresentationInfo::SetRecordtime(LPCTSTR tszRecordtime) {
    if (_tcslen(tszRecordtime) == 0)
        return;

    isInitialized_ = true;
    recordtime_ = tszRecordtime;
}

CString &ASSISTANT::PresentationInfo::GetRecordlength()
{
   return recordlength_;
}

void ASSISTANT::PresentationInfo::SetRecordlength(LPCTSTR tszRecordlength) {
    if (_tcslen(tszRecordlength) == 0)
        return;

    isInitialized_ = true;
    recordlength_ = tszRecordlength;
}

CString &ASSISTANT::PresentationInfo::GetAudioSamplerate()
{
   return audioSamplerate_;
}

void ASSISTANT::PresentationInfo::SetAudioSamplerate(LPCTSTR tszAudioSamplerate) {
    if (_tcslen(tszAudioSamplerate) == 0)
        return;

    isInitialized_ = true;
    audioSamplerate_ = tszAudioSamplerate;
}

CString &ASSISTANT::PresentationInfo::GetAudioSamplewidth()
{
   return audioSamplewidth_;
}

void ASSISTANT::PresentationInfo::SetAudioSamplewidth(LPCTSTR tszAudioSamplewidth) {
    if (_tcslen(tszAudioSamplewidth) == 0)
        return;

    isInitialized_ = true;
    audioSamplewidth_ = tszAudioSamplewidth;
}

float ASSISTANT::PresentationInfo::GetVideoFramerate()
{
   return m_fVideoFramerate;
}

void ASSISTANT::PresentationInfo::SetVideoFramerate(float fVideoFramerate)
{
   isInitialized_ = true;
   m_fVideoFramerate = fVideoFramerate;
}

CString &ASSISTANT::PresentationInfo::GetVideoCodec()
{
   return videoCodec_;
}

void ASSISTANT::PresentationInfo::SetVideoCodec(LPCTSTR videoCodec)
{
   isInitialized_ = true;
   videoCodec_ = videoCodec;
}

CString &ASSISTANT::PresentationInfo::GetVideoQuality()
{
   return videoQuality_;
}

void ASSISTANT::PresentationInfo::SetVideoQuality(LPCTSTR videoQuality)
{
   isInitialized_ = true;
   videoQuality_ = videoQuality;
}

CString &ASSISTANT::PresentationInfo::GetVideoKeyframes()
{
   return videoKeyframes_;
}

void ASSISTANT::PresentationInfo::SetVideoKeyframes(LPCTSTR videoKeyframes)
{
   isInitialized_ = true;
   videoKeyframes_ = videoKeyframes;
}
      
CString &ASSISTANT::PresentationInfo::GetVideoDatarate()
{
   return videoDatarate_;
}
      
void ASSISTANT::PresentationInfo::SetVideoDatarate(LPCTSTR videoDatarate)
{
   isInitialized_ = true;
   videoDatarate_ = videoDatarate;
}

////////////////////////////////

void ASSISTANT::PresentationInfo::ExtractMetadataFromLmd(CString &csLmdFile) {
    CString csLmdName = filename_;

    if (csLmdFile.IsEmpty()) {
        // read LMD filename as specified in LRD
        CLrdSgml lrdSgml;
        if (lrdSgml.Read(filename_, _T("<DOC")))
            lrdSgml.GetMetadataFilename(csLmdFile);
    }
    if (csLmdFile.IsEmpty()) {
        ASSISTANT::GetPrefix(csLmdName);
        csLmdName += _T(".lmd");
    }
    else {
        ASSISTANT::GetPath(csLmdName);
        csLmdName += _T("\\") + csLmdFile;
    }

    CLmdSgml lmdFile;
    bool bResult = lmdFile.Read(csLmdName, _T("<doc"));
    if (!bResult)
        return;

    ExtractGeneralMetadata(&lmdFile);
    ExtractInfosFromClip(&lmdFile);
    ExtractInfosFromAudio(&lmdFile);
    ExtractInfosFromVideo(&lmdFile);

    return;
}

void ASSISTANT::PresentationInfo::ExtractGeneralMetadata(CLmdSgml *pLmdSgml) {
    if (pLmdSgml == NULL)
        return;

    CString csTitle;
    pLmdSgml->GetMetaInfo(_T("title"), csTitle);
    SetTitle(csTitle);

    CString csAuthor;
    pLmdSgml->GetMetaInfo(_T("author"), csAuthor);
    SetAuthor(csAuthor);

    CString csCreator;
    pLmdSgml->GetMetaInfo(_T("creator"), csCreator);
    SetCreator(csCreator);

    CString csRecordDate;
    pLmdSgml->GetMetaInfo(_T("recorddate"), csRecordDate);
    SetRecorddate(csRecordDate);

    CString csRecordLength;
    pLmdSgml->GetMetaInfo(_T("recordlength"), csRecordLength);
    SetRecordlength(csRecordLength);

    CString csKeywords;
    pLmdSgml->GetMetaInfo(_T("keyword"), csKeywords);
    SetKeywords(csKeywords);
}

void ASSISTANT::PresentationInfo::ExtractInfosFromClip(CLmdSgml *pLmdSgml) {
    if (thumbs_) {
        delete thumbs_;
        thumbs_ = NULL;
    }

    thumbs_ = new PLAYER::Thumbs(pLmdSgml);

    if (pLmdSgml == NULL)
        return;

    // Tree traversal
    // clipNode should be a "chapter" tag here, probably
    CArray<SgmlElement *, SgmlElement *> aClips;
    pLmdSgml->GetAllClips(aClips);

    for (int i = 0; i < aClips.GetSize(); ++i) {
        SgmlElement *pClipTag = aClips[i];
        if (pClipTag != NULL) {
            SgClip *pNewClip = new SgClip();
            CArray<SgmlElement *, SgmlElement *> aClipElements;
            pClipTag->GetElements(aClipElements);
            for (int j = 0; j < aClipElements.GetCount(); ++j) {
                SgmlElement *pClipParameterTag = aClipElements[j];
                if (pClipParameterTag != NULL) {
                    if (_tcscmp(pClipParameterTag->GetName(), _T("title")) == 0)
                        pNewClip->SetTitle(pClipParameterTag->GetParameter());
                    else if (_tcscmp(pClipParameterTag->GetName(),_T("nr")) == 0)
                        pNewClip->SetClipNumber(_ttoi(pClipParameterTag->GetParameter()));
                    else if (_tcscmp(pClipParameterTag->GetName(),_T("begin")) == 0)
                        pNewClip->SetStartTime(_ttoi(pClipParameterTag->GetParameter()));
                    else if (_tcscmp(pClipParameterTag->GetName(),_T("end")) == 0) {
                        int iLength;
                        iLength = _ttoi(pClipParameterTag->GetParameter());
                        iLength -= pNewClip->GetStartTime();
                        pNewClip->SetLength(iLength);
                    } else if (_tcscmp(pClipParameterTag->GetName(),_T("keyword")) == 0) {
                        pNewClip->AddKeyword(pClipParameterTag->GetParameter());
                    } else if (_tcscmp(pClipParameterTag->GetName(),_T("thumb")) == 0) {
                        CString csFilename = pClipParameterTag->GetParameter();
                        int iDotPos = csFilename.Find(_T(".png"));
                        if (iDotPos != -1)
                            csFilename.Delete(iDotPos, 4);
                        pNewClip->SetFilename(csFilename);
                    }
                }
            }
            clipList_->AddClip(pNewClip);
        }
    }
}

void ASSISTANT::PresentationInfo::ExtractInfosFromAudio(CLmdSgml *pLmdSgml) {
    if (pLmdSgml == NULL)
        return;

    CString csSampleRate;
    pLmdSgml->GetAudioInfo(_T("samplerate"), csSampleRate);
    SetAudioSamplerate(csSampleRate);

    CString csBitRate;
    pLmdSgml->GetAudioInfo(_T("bitrate"), csBitRate);
    SetAudioSamplewidth(csBitRate);
}

void ASSISTANT::PresentationInfo::ExtractInfosFromVideo(CLmdSgml *pLmdSgml) {
    if (pLmdSgml == NULL)
        return;

    CString csFrameRate;
    pLmdSgml->GetVideoInfo(_T("framerate"), csFrameRate);
    TCHAR *tszStopString;
    float fFramerate = _tcstod(csFrameRate, &tszStopString);
    SetVideoFramerate(fFramerate);

    CString csCodec;
    pLmdSgml->GetVideoInfo(_T("codec"), csCodec);
    SetVideoCodec(csCodec);
    
    CString csQuality;
    pLmdSgml->GetVideoInfo(_T("quality"), csQuality);
    SetVideoQuality(csQuality);
    
    CString csKeyframeRate;
    pLmdSgml->GetVideoInfo(_T("keyframes"), csKeyframeRate);
    SetVideoKeyframes(csKeyframeRate);
    
    CString csDataRate;
    pLmdSgml->GetVideoInfo(_T("datarate"), csDataRate);
    SetVideoDatarate(csDataRate);
}

void ASSISTANT::PresentationInfo::ExtractMetadataFromLrd(CString &csLmdFile) {
    CLrdSgml lrdSgml;
    bool bResult = lrdSgml.Read(filename_, _T("<DOC"));
    if (!bResult)
        return;

    CString csCodePage;
    lrdSgml.GetMetaInfo(_T("CODEPAGE"), csCodePage);
    int iCodePage = ::GetACP();
    if (!csCodePage.IsEmpty()) {
        if (csCodePage.CompareNoCase(_T("utf-8")) == 0)
            iCodePage = NUM_CODEPAGE_UTF8;
        else
            iCodePage = _ttoi(csCodePage);
    }
    SetCodepage((unsigned int)iCodePage);

    CString csRecordDate;
    lrdSgml.GetMetaInfo(_T("RECORDDATE"), csRecordDate);
    CString csDate = csRecordDate;
    CString csTime = _T("??");
    int iPos = csRecordDate.Find(_T("-"));
    if (iPos != -1) {
        csDate = csRecordDate.Left(iPos-1);
        csTime = csRecordDate.Right(csRecordDate.GetLength() - (iPos+2));
    }
    SetRecorddate(csDate);
    SetRecordtime(csTime);

    CString csRecordLength;
    lrdSgml.GetMetaInfo(_T("LENGTH"), csRecordLength);
    if (!csRecordLength.IsEmpty())
        SetRecordlength(csRecordLength);

    CString csRecordType;
    lrdSgml.GetMetaInfo(_T("TYPE"), csRecordType);
    m_bIsDenverDocument = (csRecordType == _T("denver"));
    
    lrdSgml.GetMetadataFilename(csLmdFile);

    // check clip structuring status
    m_bClipIsStructured = false;
    m_bClipIsSearchable = false;

    CString csClipStructureValue;
    lrdSgml.GetClipStructureValue(_T("STRUCTURE"), csClipStructureValue);
    if (!csClipStructureValue.IsEmpty()) {
        m_bClipIsStructured = (csClipStructureValue == _T("available"));
    }
    
    CString csFulltextValue;
    lrdSgml.GetClipStructureValue(_T("FULLTEXT"), csFulltextValue);
    if (!csFulltextValue.IsEmpty()) {
        m_bClipIsSearchable = (csFulltextValue == _T("available"));
    }
}

void ASSISTANT::PresentationInfo::ExtractMetadataFromLep(CString &csLmdFile) {
	CString csLepName = filename_;
	if(!csLmdFile.IsEmpty())
	{
		ASSISTANT::GetPath(csLepName);
		csLepName += _T("\\") + csLmdFile;
	}
	ASSISTANT::GetPrefix(csLepName);
	csLepName += _T(".lep");

	CLepSgml lepFile;
	bool bResult = lepFile.Read(csLepName, _T("<edi"));
	if (!bResult)
		return;

	CString csTitle;
	lepFile.GetMetaInfo(_T("title"), csTitle);
	SetTitle(csTitle);

	CString csAuthor;
	lepFile.GetMetaInfo(_T("author"), csAuthor);
	SetAuthor(csAuthor);

	CString csCreator;
	lepFile.GetMetaInfo(_T("creator"), csCreator);
	SetCreator(csCreator);

	CString csRecordDate;
	lepFile.GetMetaInfo(_T("recorddate"), csRecordDate);
	SetRecorddate(csRecordDate);

	CString csRecordTime;
	lepFile.GetMetaInfo(_T("recordtime"), csRecordTime);
	SetRecordtime(csRecordTime);

	CString csRecordLength;
	lepFile.GetMetaInfo(_T("length"), csRecordLength);
	SetRecordlength(csRecordLength);

	CString csCodePage;
	lepFile.GetMetaInfo(_T("codepage"), csCodePage);
	int iCodePage = ::GetACP();
	if (!csCodePage.IsEmpty()) {
		if (csCodePage.CompareNoCase(_T("utf-8")) == 0)
			iCodePage = NUM_CODEPAGE_UTF8;
		else
			iCodePage = _ttoi(csCodePage);
	}
	SetCodepage((unsigned int)iCodePage);
}

ASSISTANT::ClipList *ASSISTANT::PresentationInfo::GetClipList()
{   
   return clipList_;
}
      
bool ASSISTANT::PresentationInfo::IsClipListEmpty()
{
   if (clipList_ == NULL || clipList_->Empty())
      return true;
   else
      return false;
}

void ASSISTANT::PresentationInfo::Initialize()
{
    if (!isInitialized_) {
        CString csLmdFile;
        ExtractMetadataFromLrd(csLmdFile);
        ExtractMetadataFromLmd(csLmdFile);
		ExtractMetadataFromLep(csLmdFile);
        isInitialized_ = true;
    }
    return;
}

bool ASSISTANT::PresentationInfo::IsFilenameEqual(LPCTSTR filename)
{
   if (filename_ == filename)
      return true;
   else 
      return false;
}
      
void ASSISTANT::PresentationInfo::FillMetainfoList(CString &metainfoList)
{
   metainfoList = _T("{");
   metainfoList += GetTitle();
   metainfoList += _T("} ");
   metainfoList += _T("{");
   metainfoList += GetAuthor();
   metainfoList += _T("} ");
   metainfoList += _T("{");
   metainfoList += GetCreator();
   metainfoList += _T("} ");
   metainfoList += _T("{");
   metainfoList += GetKeywords();
   metainfoList += _T("} ");
   metainfoList += _T("{");
   metainfoList += GetRecorddate();
   metainfoList += _T("} ");
}

void ASSISTANT::PresentationInfo::ParseAndReplaceLmdMetainfo(CLmdSgml *pLmdSgml) {
    if (pLmdSgml == NULL)
        return;

    pLmdSgml->ChangeMetaInfo(_T("author"), GetAuthor());

    pLmdSgml->ChangeMetaInfo(_T("title"), GetTitle());

    pLmdSgml->ChangeMetaInfo(_T("creator"), GetCreator());

    pLmdSgml->ChangeMetaInfo(_T("recorddate"), GetRecorddate());

    CStringArray aKeywords;
    ASSISTANT::CreateListFromString(GetKeywords(), aKeywords);
    pLmdSgml->ChangeKeywords(aKeywords);
    aKeywords.RemoveAll();
}

void ASSISTANT::PresentationInfo::ParseAndReplaceLrdMetainfo(CLrdSgml *pLrdSgml) {
    if (pLrdSgml == NULL)
        return;

    pLrdSgml->ChangeMetaInfo(_T("AUTHOR"), GetAuthor());

    pLrdSgml->ChangeMetaInfo(_T("TITLE"), GetTitle());

    pLrdSgml->ChangeMetaInfo(_T("RECORDLENGTH"), GetRecordlength());

    CString csRecordDate = GetRecorddate();
    csRecordDate += _T(" - ");
    csRecordDate += GetRecordtime();
    pLrdSgml->ChangeMetaInfo(_T("RECORDDATE"), csRecordDate);

    CString csCodePage;
    if (GetCodepage() == NUM_CODEPAGE_UTF8)
        csCodePage = _T("utf-8");
    else
        csCodePage.Format(_T("%d"), GetCodepage());
    pLrdSgml->ChangeMetaInfo(_T("CODEPAGE"), csCodePage);
}

void ASSISTANT::PresentationInfo::ReWriteLmdFile() {	
    // BUGFIX 5507: property changes not stored after rename
    // read proper LMD filename from LRD
    CString csLmdName;
    SgmlFile lrdFile;
    bool bResult = lrdFile.Read(filename_, _T("<DOC"));
    if ( bResult && lrdFile.GetRoot() ) {
        SgmlElement *lmdNode = lrdFile.GetRoot()->Find(_T("METADATA"), _T("FILES"));
        if ( lmdNode ) {
            csLmdName = filename_;
            ASSISTANT::GetPath(csLmdName);
            csLmdName += _T("\\");
            csLmdName += lmdNode->GetParameter();
        }
    }
    
    CLmdSgml lmdFile;
    bResult = lmdFile.Read(csLmdName,_T("<doc"));
    if (!bResult)
        return;

    ParseAndReplaceLmdMetainfo(&lmdFile);

    lmdFile.Write(csLmdName);
}

void ASSISTANT::PresentationInfo::ReWriteLrdFile() {
    CLrdSgml lrdFile;
    bool bResult = lrdFile.Read(filename_, _T("<DOC"));
    if (!bResult)
        return;

    ParseAndReplaceLrdMetainfo(&lrdFile);

    lrdFile.Write(filename_);
}
  
ASSISTANT::SgClip *ASSISTANT::PresentationInfo::FindClip(int clipNumber)
{
   SgClip *clip = clipList_->GetClip(clipNumber);
   if (clip)
   {
      return clip;
   }

   return NULL;
}
      
ASSISTANT::SgClip *ASSISTANT::PresentationInfo::FindClip(LPCTSTR clipName, int startAt)
{
   SgClip *clip = clipList_->GetClip(clipName, startAt);
   if (clip)
   {
      return clip;
   }

   return NULL;
}
      
void ASSISTANT::PresentationInfo::RemoveClip(int clipNumber)
{
   SgClip *clip = clipList_->GetClip(clipNumber);
   if (clip)
   {
      clipList_->RemoveClip(clip);
      ReWriteLrdFile();
      ReWriteLmdFile();
   }
}

void ASSISTANT::PresentationInfo::AddClip(SgClip *clip)
{
   clipList_->InsertClip(clip);
   ReWriteLrdFile();
   ReWriteLmdFile();
}

void ASSISTANT::PresentationInfo::ReOrderClipList()
{
   ClipList *orderedList = new ClipList();
   clipList_->ReturnOrderedList(orderedList);
   clipList_->RemoveElements();
   clipList_ =  NULL;
   clipList_ = orderedList;
   ReWriteLrdFile();
   ReWriteLmdFile();

}

void ASSISTANT::PresentationInfo::MoveThumbs(PLAYER::Thumbs *thumbs)
{
   if (thumbs_)
   {
      delete thumbs_;
      thumbs_ = NULL;
   }

   thumbs_ = new PLAYER::Thumbs(false);
   thumbs_->SetRoot(thumbs->GetRoot());

   thumbs->SetRoot(new PLAYER::ThumbChapter());
}

int ASSISTANT::PresentationInfo::GetNextClipNumber()
{
   if (clipList_)
      return clipList_->GetNextClipNumber();
   else
      return 0;
}

bool ASSISTANT::PresentationInfo::GetImageFileNames(LPCTSTR szObjectFilename) {
    CObjSgml objFile;
    bool bSuccess = objFile.Read(szObjectFilename, _T("<WB"));
    if (!bSuccess)
        return false;

    HRESULT hr = objFile.GetImageFileNames(m_slFileList);
    if (FAILED(hr))
        bSuccess = false;

    return bSuccess;
}

bool ASSISTANT::PresentationInfo::FillFileList() {
    bool bSuccess = true;

    EmptyFileList();

    // record directory
    CString csRecordPath = filename_;
    ASSISTANT::GetPath(csRecordPath);
    csRecordPath += _T("\\");

    // LRD file
    CString csLrdName = filename_;
    ASSISTANT::GetName(csLrdName);
    CString csFullFilePath = csRecordPath + csLrdName;
    if (_taccess(csFullFilePath, 00) == 0)
        m_slFileList.Add(csLrdName);

    // LMD file
    CString csLmdName = filename_;
    ASSISTANT::GetName(csLmdName);
    ASSISTANT::GetPrefix(csLmdName);
    // TODO: avoid unsafe construction of LMD filename; use name as specified in LRD
    //       in LEC 4.0 FillFileList() is not used (was previously used to "Pack")
    csLmdName += _T(".lmd");

    csFullFilePath = csRecordPath + csLmdName;
    if (_taccess(csFullFilePath,00) == 0)
        m_slFileList.Add(csLmdName);

    CLrdSgml lrdFile;
    bSuccess = lrdFile.Read(filename_, _T("<DOC"));
    if (!bSuccess)
        return bSuccess;

    HRESULT hr = S_OK;

    CString csAudioName;
    hr = lrdFile.GetAudioFilename(csAudioName);
    if (SUCCEEDED(hr)) {
        csFullFilePath = csRecordPath + csAudioName;
        if (_taccess(csFullFilePath,00) == 0)
            m_slFileList.Add(csAudioName);
    }

    CString csVideoFilename;
    CString csStillImageFilename;
    hr = lrdFile.GetVideoAndStillImageFilenames(csVideoFilename, csStillImageFilename);
    if (SUCCEEDED(hr)) {
        if (!csVideoFilename.IsEmpty()) {
            csFullFilePath = csRecordPath + csVideoFilename;
            if (_taccess(csFullFilePath,00) == 0)
                m_slFileList.Add(csVideoFilename);
        }
        if (!csStillImageFilename.IsEmpty()) {
            csFullFilePath = csRecordPath + csStillImageFilename;
            if (_taccess(csFullFilePath,00) == 0)
                m_slFileList.Add(csStillImageFilename);
        }
    }

    CStringArray aClipFilenames;
    CArray<int, int> aClipTimestamps;
    hr = lrdFile.GetClipFilenamesAndTimestamps(aClipFilenames, aClipTimestamps);
    if (SUCCEEDED(hr)) {
        if (!aClipFilenames.IsEmpty()) {
            for (int i = 0; i < aClipFilenames.GetSize(); ++i) {
                csFullFilePath = csRecordPath + aClipFilenames[i];
                if (_taccess(csFullFilePath,00) == 0)
                    m_slFileList.Add(aClipFilenames[i]);
            }
        }
    }

    CString csEvqFilename;
    CString csObjFilename;
    hr = lrdFile.GetEvqAndObjFilename(csEvqFilename, csObjFilename);
    if (SUCCEEDED(hr)) {
        if (!csEvqFilename.IsEmpty()) {
            csFullFilePath = csRecordPath + csEvqFilename;
            if (_taccess(csFullFilePath,00) == 0)
                m_slFileList.Add(csEvqFilename);
        }
        if (!csObjFilename.IsEmpty()) {
            csFullFilePath = csRecordPath + csObjFilename;
            if (_taccess(csFullFilePath,00) == 0)
                m_slFileList.Add(csObjFilename);
            GetImageFileNames(csFullFilePath);
        }
    }

    return bSuccess;
}

void ASSISTANT::PresentationInfo::EmptyFileList()
{
   m_slFileList.RemoveAll();
}

int ASSISTANT::PresentationInfo::GetFileCount()
{
   if (m_slFileList.GetSize() == 0)
      FillFileList();

   return m_slFileList.GetSize();
}

bool ASSISTANT::PresentationInfo::CopyFiles(LPCTSTR destDirectory, float fFileStep, float &fActualPosition, bool &bOverwriteAll, bool &bCancelCopy, CProgressDialogA *pProgress)
{
   if (m_slFileList.GetSize() == 0)
      FillFileList();

   // create new directory, if not exists
   if (!ASSISTANT::DirectoryExist(destDirectory))
      CreateDirectory(destDirectory, NULL);

   CString ssSourcePath = filename_;
   ASSISTANT::GetPath(ssSourcePath);
   ssSourcePath += _T("\\");
   // copy all files 
   int iFileCount = 0;
   bool bDoSave = true;
   
   for (int i = 0; i < m_slFileList.GetSize() && !bCancelCopy; ++i)
   {
      CString ssSourceFile = ssSourcePath + m_slFileList[i];
      CString ssTargetFile = destDirectory + m_slFileList[i];

      if (!bOverwriteAll && ASSISTANT::FileExist(ssTargetFile))
      {
         /*
         int answer = IDYES; //DialogBoxParam(hInstance, "YESNOALL", NULL, AskYesNoAllCancelCallback, (long)ssTargetFile);
         
         if (answer == IDYES)             // Yes
            bDoSave = true;
         else if (answer == IDYESALL)     // Yes, All
            bOverwriteAll = true;
         else if (answer == IDNO)         // No
            bDoSave = false;
         else if (answer == IDCANCEL)     // Cancel
            bCancelCopy = true;
            */
      }
      //if (Project::active->GetProgressCancelled())
      //   bCancelCopy = true;
      if (!bCancelCopy && (bOverwriteAll || bDoSave))
      {
         CString ssFileName = m_slFileList[i];
         BOOL bResult = CopyFile(ssSourceFile, ssTargetFile, FALSE);
         if (bResult == 0) 
         {
            DWORD dwError = GetLastError();
            
            if (dwError == ERROR_DISK_FULL)
            {   
               CString csMessage;
               csMessage.LoadString(IDS_DISK_FULL_ERROR);
               MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
               return false;
            }
            else
            {
               CString csMessage;
               csMessage.Format(IDS_COPY_FILE_ERROR, ssFileName);
               MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
            }
         }
      }
      //if (Project::active->GetProgressCancelled())
      //   bCancelCopy = true;

      ++iFileCount;
      if (pProgress != NULL)
      {
         fActualPosition = fActualPosition + fFileStep;
         pProgress->SetPos((int)fActualPosition);
      }
   }

   return true;
}

void ASSISTANT::PresentationInfo::StartReplay()
{
   CString csLrdFile = GetFilename();
   AfxGetApp()->OpenDocumentFile(csLrdFile);

   CEditorDoc *pDoc = CMainFrameE::GetEditorDocument();
   CMainFrameE *pMainFrameE = CMainFrameE::GetCurrentInstance();
   if (pDoc != NULL) {
        pDoc->BeginWaitCursor();
        HRESULT hrReset = pDoc->project.StartQuestionTimers();
        pDoc->StartPreview();
        if (pMainFrameE)
            pMainFrameE->UpdateWhiteboardView();
        pDoc->EndWaitCursor();
   }
   if (pDoc != NULL &&pDoc->IsPreviewActive()) {
      CWnd *pMainFrame = AfxGetMainWnd();
      if (pMainFrame && pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrame))
         ((CMainFrame *)pMainFrame)->UpdateEditorStatusBarAndMenu();
      if(pMainFrameE!=NULL)
      {
         //pMainFrameE->HideDocumentStructureView();
      }
   }
   /* Old from 3.0: Call Player
   ASSISTANT::SuiteEntry suiteEntry;
   CString csPlayerPath;
   if (!suiteEntry.ReadProgramPath(csPlayerPath))
      csPlayerPath.Empty();

   if (!csPlayerPath.IsEmpty()) 
   {
      if (csPlayerPath[csPlayerPath.GetLength() - 1] != _T('\\'))
         csPlayerPath += _T("\\");
      csPlayerPath += _T("Player");

      CString csPlayerExe = csPlayerPath;
      csPlayerExe += _T("\\player.exe");

      CString csLrdFile = _T(" \"");
      csLrdFile += GetFilename();
      csLrdFile += _T("\"");
      
      HINSTANCE hInstance = ShellExecute(NULL, _T("open"), csPlayerExe, csLrdFile, csPlayerPath, SW_SHOWNORMAL);
      if ((int)hInstance <= 32)
         MessageBox(NULL, _T(""), NULL, MB_OK);
   }
   else 
   {
      CString csMessage;
      csMessage.LoadString(IDS_ERROR_FIND_PLAYER);
      MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
   }

   return;
   */
}

struct FinalizeThreadParams {
    ASSISTANT::PresentationInfo *pPresentationInfo;
    bool bDirectMode;
    bool bSelectFormat;
    int iSelectedFormat;
};


void ASSISTANT::PresentationInfo::StartConvert(bool bDirectMode, bool bSelectFormat)
{
    FinalizeThreadParams *pParams = new FinalizeThreadParams; 
    // new: avoid it being deleted before the thread reaches it
    pParams->pPresentationInfo = this;
    pParams->bDirectMode = bDirectMode;
    pParams->bSelectFormat = bSelectFormat;
    pParams->iSelectedFormat = PublishingFormat::TYPE_NOTHING;

    bool bSelectionCancelled = false;
    if (bSelectFormat) {
        // format selection only
        __int64 lCustomProfileID = 0;
        pParams->iSelectedFormat = MfcUtils::ShowPublisherFormatSelection(AfxGetMainWnd()->GetSafeHwnd(), filename_);
        if (pParams->iSelectedFormat == PublishingFormat::TYPE_NOTHING)
            bSelectionCancelled = true;

        // The pParams should be deleted
        if (pParams)
            delete pParams;
        pParams = NULL;
    }
    // BUGFIX 4956: do not publish after selection
    else {
        if ( bDirectMode ) 
        {
            // get previously selected format
            pParams->iSelectedFormat = MfcUtils::GetPublisherFormatSelection();
            pParams->bSelectFormat = pParams->iSelectedFormat != PublishingFormat::TYPE_NOTHING;
        }
        if (pParams->iSelectedFormat != PublishingFormat::TYPE_NOTHING) {
            ProfileUtils::SetProfilePublished(pParams->iSelectedFormat, title_);
        }
        // publish
        CWinThread *pThread = ::AfxBeginThread(FinalizeThreadLauncher, pParams);
        
        // pParams can not be deleted here, because they are used by the thread

        // This will be implicitly blocking by showing a modalprogress dialog
        // for a possible finalization
    }
}

//static
UINT ASSISTANT::PresentationInfo::FinalizeThreadLauncher(LPVOID pParam) {
    HRESULT hr = CLanguageSupport::SetThreadLanguage();

    FinalizeThreadParams *pParams = (FinalizeThreadParams *)pParam;


    hr = pParams->pPresentationInfo->FinalizeThread(pParams->bDirectMode, pParams->bSelectFormat, pParams->iSelectedFormat);

    //Can pParams be deleted?
    /*
    if (pParams)
        delete pParams;
    pParams = NULL;
    */

    return hr;
}

HRESULT ASSISTANT::PresentationInfo::FinalizeThread(bool bDirectMode, bool bSelectFormat, int iSelectedFormat) {
    bool bFinalizeSuccess = true;

    CString csLrdFile = GetFilename();
    bool bFinalizationNeeded = LMisc::CheckNeedsFinalization(csLrdFile);
    if (bFinalizationNeeded) {
        
        // TRUE: make it manual reset: it will be used/checked twice
        HANDLE hExportFinish = ::CreateEvent(NULL, TRUE, FALSE, _T("FinishExport"));
        
        // NULL: standard mode; not Powertrainer (and thus hidden)
        bFinalizeSuccess = CStudioApp::Finalize(GetFilename(), NULL, hExportFinish, true);

        if (bFinalizeSuccess)
            DWORD dwFinish = ::WaitForSingleObject(hExportFinish, INFINITE);
    }

    if (bFinalizeSuccess) {
        
        bool bPublishSuccess = CStudioApp::Publish(csLrdFile, bDirectMode, bSelectFormat, 
            iSelectedFormat, CString());

        if (!bPublishSuccess) {
            bFinalizeSuccess = false;

            CString csMessage;
            csMessage.LoadString(IDS_ERROR_START_CONVERTER);
            CEditorDoc::ShowErrorMessage(csMessage);
        }

    }

    return bFinalizeSuccess ? S_OK : E_FAIL;
}

void ASSISTANT::PresentationInfo::StartEdit()
{
    CString csLrdFile = GetFilename();
    AfxGetApp()->OpenDocumentFile(csLrdFile);

   return;
}

void ASSISTANT::PresentationInfo::StartExport()
{
    CEditorProject p;

    CString csLrdPath = GetFilename();
    CString csLepPath = CEditorDoc::GetCorrespondingLep(csLrdPath);

    bool bSuccess = _taccess(csLepPath, 04) == 0;

    if (!bSuccess) {
        // TODO: No project file yet
    }

    if (bSuccess)
        bSuccess = p.StartOpen(csLepPath);

    if (bSuccess)
        CMainFrameE::DoFileExport(::AfxGetMainWnd(), &p, NULL); // shows a file dialog (for example)

    // TODO this is blocking, should be; see StartConvert() and FinalizeThread
}
