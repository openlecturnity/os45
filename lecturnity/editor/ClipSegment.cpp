// ClipSegment.cpp: Implementierung der Klasse CClipSegment.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClipSegment.h"
#include "avinfo.h"


CClipSegment::CClipSegment()
{
   m_nClassId = 0x390de29f;
   m_iVideoStreamIndex = -1;
   m_iAudioStreamIndex = -1; 
   m_iAudioVolume = 100;
   
   ZeroMemory(&m_tszLabel, (MAX_CLIP_LABEL_LENGTH + 1)  * sizeof _TCHAR);
   ZeroMemory(&m_tszAudioFilename, (MAX_PATH +1) * sizeof _TCHAR);
   ZeroMemory(&m_tszKeywords, (MAX_KEYWORDS_LENGTH +1) * sizeof _TCHAR);

   // PZI: information concerning structured clips
   m_bStructureAvailable = false;
   m_bImagesAvailable = false;
   m_bFulltextAvailable = false;
}

CClipSegment::~CClipSegment()
{

}



/* Attempts to replace the use of "CClipInfo"
HRESULT CClipSegment::Init(_TCHAR *tszFile, UINT nMediaLengthMs)
{
   // nMediaLengthMs will be ignored (reset) later

   HRESULT hr = S_OK;

   hr = CMediaSegment::Init(tszFile, nMediaLengthMs);

   AVInfo avInfo;
   if (SUCCEEDED(hr))
      hr = avInfo.Initialize(tszFile);
  
   if (SUCCEEDED(hr))
      hr = avInfo.GetVideoStreamIndex(&m_iVideoStreamNum);
   
   if (SUCCEEDED(hr))
      hr = avInfo.GetAudioStreamIndex(&m_iAudioStreamNum);
   
   if (SUCCEEDED(hr) && m_iVideoStreamNum < 0)
      hr = E_CSG_NO_VIDEO_FOUND;
   // TODO: what will outer/other classes do with this error code?

   if (SUCCEEDED(hr) && m_iAudioStreamNum >= 0)
      hr = InitAudio();

   return hr;
}
*/


HRESULT CClipSegment::Init(CClipInfo *pClipInfo)
{
   HRESULT hr = S_OK;

   if (pClipInfo == NULL)
      return E_POINTER;

   int iClipLength = pClipInfo->GetLength();
   if (iClipLength <= 0)
   {
      // try to determine the length of this clip
      if (!VideoInformation::GetVideoDuration(pClipInfo->GetFilename(), iClipLength) || iClipLength <= 0)
         hr = E_CSG_CLIP_INFO_FAILED;
      else
         pClipInfo->SetLength(iClipLength);
   }

   if (SUCCEEDED(hr))
      hr = CMediaSegment::Init(pClipInfo->GetFilename(), 0, iClipLength);


   if (SUCCEEDED(hr))
   {
      m_iVideoStreamIndex = pClipInfo->GetStreamSelector();
      m_iAudioStreamIndex = pClipInfo->GetAudioStreamSelector();
      m_iAudioVolume = pClipInfo->GetTempOriginalAudioVolume();
      
      if (pClipInfo->GetTitle() != NULL)
      {
         int iCopyLength = min(_tcslen(pClipInfo->GetTitle()), MAX_CLIP_LABEL_LENGTH);
         _tcsncpy(m_tszLabel, pClipInfo->GetTitle(), iCopyLength);
         m_tszLabel[iCopyLength] = 0; // destroyed by _tcsncpy()
      }

      if (pClipInfo->GetAudioFilename() != NULL)
      {
         int iCopyLength = min(_tcslen(pClipInfo->GetAudioFilename()), MAX_PATH);
         _tcsncpy(m_tszAudioFilename, pClipInfo->GetAudioFilename(), iCopyLength);
         m_tszAudioFilename[iCopyLength] = 0; // destroyed by _tcsncpy()
      }

      CString csKeywords;
      pClipInfo->GetKeywords(csKeywords);
      if (csKeywords.GetLength() > 0)
      {
         int iCopyLength = min(csKeywords.GetLength(), MAX_KEYWORDS_LENGTH);
         _tcsncpy(m_tszKeywords, (LPCTSTR)csKeywords, iCopyLength);
         m_tszKeywords[iCopyLength] = 0; // destroyed by _tcsncpy()
      }

      // PZI: information concerning structured clips
      m_bStructureAvailable = pClipInfo->GetStructureAvailable();
      m_bImagesAvailable = pClipInfo->GetImagesAvailable();
      m_bFulltextAvailable = pClipInfo->GetFulltextAvailable();
   }
  

   return hr;
}
   

HRESULT CClipSegment::CloneTo(CMediaSegment *sgmTarget)
{
   HRESULT hr = S_OK;

   if (sgmTarget == NULL)
      return E_POINTER;

   CClipSegment *pOther = (CClipSegment *)sgmTarget;

   if (pOther->m_nClassId != this->m_nClassId)
      return E_CSG_WRONG_CLASS;


   hr = CMediaSegment::CloneTo(sgmTarget);

   
   if (SUCCEEDED(hr))
      hr = CloneClipDataTo(pOther);

   return hr;
}

HRESULT CClipSegment::CloneClipDataTo(CClipSegment *sgmTarget)
{
   // TODO check for "isInitialized" or similar?

   sgmTarget->m_iVideoStreamIndex = this->m_iVideoStreamIndex;
   sgmTarget->m_iAudioStreamIndex = this->m_iAudioStreamIndex;
   sgmTarget->m_iAudioVolume = this->m_iAudioVolume;
   
   _tcscpy(sgmTarget->m_tszLabel, this->m_tszLabel);
   _tcscpy(sgmTarget->m_tszAudioFilename, this->m_tszAudioFilename);
   _tcscpy(sgmTarget->m_tszKeywords, this->m_tszKeywords);

   // PZI: information concerning structured clips
   sgmTarget->m_bStructureAvailable = this->m_bStructureAvailable;
   sgmTarget->m_bImagesAvailable = this->m_bImagesAvailable;
   sgmTarget->m_bFulltextAvailable = this-> m_bFulltextAvailable;

   return S_OK;
}


HRESULT CClipSegment::FillClipInfo(CClipInfo* pClipInfo)
{
   if (pClipInfo == NULL)
      return E_POINTER;

   pClipInfo->SetFilename(GetFilename(), m_iVideoStreamIndex);
   pClipInfo->SetTimestamp(GetTargetBegin());
   pClipInfo->SetLength(GetLength());
   pClipInfo->SetTitle(GetLabel());

   // even if there is no audio: important to set the index
   // as it can also be "-1" and you have to reset all
   // variables in CClipInfo structure
   pClipInfo->SetAudioStreamSelector(m_iAudioStreamIndex);

   pClipInfo->SetAudioFilename(m_tszAudioFilename);
   pClipInfo->SetTempOriginalAudioVolume(m_iAudioVolume);
   
   if (_tcslen(m_tszKeywords) > 0)
   {
      CString csKeywords = m_tszKeywords;
      pClipInfo->SetKeywords(csKeywords);
   }
   
   // PZI: information concerning structured clips
   pClipInfo->SetStructureAvailable(m_bStructureAvailable);
   pClipInfo->SetImagesAvailable(m_bImagesAvailable);
   pClipInfo->SetFulltextAvailable(m_bFulltextAvailable);

   return S_OK;
}

bool CClipSegment::HasAudio()
{
   return m_iAudioStreamIndex >= 0;
}


HRESULT CClipSegment::AppendLepTextData(_TCHAR *tszOutput, CString &csLepFileName, bool bWithDecoration)
{
   HRESULT hr = S_OK;

   if (tszOutput == NULL)
      return E_POINTER;

   if (bWithDecoration)
   {
      _tcscat(tszOutput, _T("<CLIP"));
      tszOutput += _tcslen(tszOutput);
   }

   // append the values stored in the super class
   hr = CMediaSegment::AppendLepTextData(tszOutput, csLepFileName, false);

   if (SUCCEEDED(hr))
   {
      tszOutput += _tcslen(tszOutput);
      
      CString csLabelSgml;
      StringManipulation::TransformForSgml(m_tszLabel, csLabelSgml);
       
      _stprintf(tszOutput, _T(" label=\"%s\" video-idx=\"%d\""), csLabelSgml, m_iVideoStreamIndex);
      tszOutput += _tcslen(tszOutput);
      
      
      if (m_iAudioStreamIndex >= 0)
      {
         // TODO what about the setting "mix audio"?

         CString csAudioFilename;
         if (_tcslen(m_tszAudioFilename) > 0)
            csAudioFilename = StringManipulation::MakeRelativeAndSgml(csLepFileName, m_tszAudioFilename);
         
         _stprintf(tszOutput, _T(" audio-file=\"%s\" audio-idx=\"%d\" volume=\"%d\""),
            csAudioFilename, m_iVideoStreamIndex, m_iAudioVolume);
         tszOutput += _tcslen(tszOutput);
      }

      if (_tcslen(m_tszKeywords) > 0)
      {
         CString csKeywordsSgml;
         StringManipulation::TransformForSgml(m_tszKeywords, csKeywordsSgml);

         _stprintf(tszOutput, _T(" keywords=\"%s\""), csKeywordsSgml);
         tszOutput += _tcslen(tszOutput);
      }

      if (m_bStructureAvailable) {
         _stprintf(tszOutput, _T(" structure=\"true\""));
         tszOutput += _tcslen(tszOutput);
      }

      if (m_bImagesAvailable) {
         _stprintf(tszOutput, _T(" images=\"true\""));
         tszOutput += _tcslen(tszOutput);
      }
      
      if (m_bFulltextAvailable) {
         _stprintf(tszOutput, _T(" fulltext=\"true\""));
         tszOutput += _tcslen(tszOutput);
      }

      
      if (bWithDecoration)
      {
         _tcscat(tszOutput, _T("></CLIP>"));
         tszOutput += _tcslen(tszOutput);
      }
   }

   return hr;
}

HRESULT CClipSegment::ReadFromSgml(SgmlElement *pSgml, LPCTSTR tszLepFileName) {
    HRESULT hr = S_OK;

    if (pSgml == NULL)
        return E_POINTER;

    hr = CMediaSegment::ReadFromSgml(pSgml, tszLepFileName);

    if (SUCCEEDED(hr)) {
        m_iVideoStreamIndex = pSgml->GetIntValue(_T("video-idx"));

        if (pSgml->GetValue(_T("label")) != NULL)
            _tcscpy(m_tszLabel, pSgml->GetValue(_T("label")));

        if (pSgml->GetValue(_T("audio-idx")) != NULL) {
            LPCTSTR tszAudioFileName = pSgml->GetValue(_T("audio-file"));
            if (tszAudioFileName != NULL) {
                // TODO what about checking for file existance (see CEditorProject::MakeAbsoluteAndCheck())?. Same also "CMediaSegment::ReadFromSgml"
                CString csAbsoluteAudioFilename = StringManipulation::MakeAbsolute(tszLepFileName, tszAudioFileName);
                _tcscpy(m_tszAudioFilename, (LPCTSTR)csAbsoluteAudioFilename);
            }

            m_iAudioStreamIndex = pSgml->GetIntValue(_T("audio-idx"));
            m_iAudioVolume = pSgml->GetIntValue(_T("volume"));
        }

        if (pSgml->GetValue(_T("keywords")) != NULL)
        {
            _tcscpy(m_tszKeywords, pSgml->GetValue(_T("keywords")));
        }

        if (pSgml->GetValue(_T("structure")) != NULL) {
            m_bStructureAvailable = pSgml->GetBoolValue(_T("structure"));
        }

        if (pSgml->GetValue(_T("images")) != NULL) {
            m_bImagesAvailable = pSgml->GetBoolValue(_T("images"));
        }

        if (pSgml->GetValue(_T("fulltext")) != NULL) {
            m_bFulltextAvailable = pSgml->GetBoolValue(_T("fulltext"));
        }
    }

    return hr;
}