// Segment.cpp: Implementierung der Klasse CSegment.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MediaSegment.h"
#include "MiscFunctions.h"


CMediaSegment::CMediaSegment()
{
   ZeroMemory(m_tszFilename, (MAX_PATH + 1) * sizeof _TCHAR);
   
   m_nSourceBeginMs = 0;
   m_nTargetBeginMs = 0;
   m_nLengthMs = 0;
   m_bIsInitialized = false;
}

CMediaSegment::~CMediaSegment()
{

}

HRESULT CMediaSegment::Init(const _TCHAR *tszFile, UINT nTargetBeginMs, UINT nMediaLengthMs)
{
   // TODO: require file?
   //if (tszFile == NULL)
   //   return E_POINTER;

   if (tszFile != NULL)
   {
      int iCopyLength = min(MAX_PATH, _tcslen(tszFile));
      _tcsncpy(m_tszFilename, tszFile, iCopyLength);
      m_tszFilename[iCopyLength] = 0; // destroyed by _tcsncpy()
   }
   else
      ZeroMemory(m_tszFilename, (MAX_PATH + 1) * sizeof _TCHAR);

   m_nTargetBeginMs = nTargetBeginMs;

   m_nLengthMs = nMediaLengthMs;

   m_bIsInitialized = true;

   return S_OK;
}

HRESULT CMediaSegment::CloneTo(CMediaSegment *sgmTarget)
{
   _tcscpy(sgmTarget->m_tszFilename, m_tszFilename);

   sgmTarget->m_nSourceBeginMs = m_nSourceBeginMs;
   sgmTarget->m_nTargetBeginMs = m_nTargetBeginMs;
   sgmTarget->m_nLengthMs = m_nLengthMs;
   
   sgmTarget->m_bIsInitialized = true;

   return S_OK;
}

HRESULT CMediaSegment::IncreaseLength(UINT nIncreaseMs)
{
   m_nLengthMs += nIncreaseMs;

   return nIncreaseMs > 0 ? S_OK : S_FALSE;
}
   
HRESULT CMediaSegment::IncrementTargetBegin(UINT nMoveMs)
{
   if (!m_bIsInitialized)
      return E_MSM_UNINITIALIZED;

   if (nMoveMs > (UINT)4200000000)
      return S_FALSE; // BUGFIX #4377: ignore very high values: probably negative values

   m_nTargetBeginMs += nMoveMs;

   return S_OK;
}

HRESULT CMediaSegment::DecrementTargetBegin(UINT nMoveMs)
{
   HRESULT hr = S_OK;

   if (!m_bIsInitialized)
      return E_MSM_UNINITIALIZED;

   if (nMoveMs <= m_nTargetBeginMs)
      m_nTargetBeginMs -= nMoveMs;
   else
   {
      m_nTargetBeginMs = 0;
      hr = S_FALSE; // TODO meaningful error (value)?
   }

   return hr;
}


bool CMediaSegment::BeginsAtOrAfter(UINT nPositionMs)
{
   if (m_nTargetBeginMs >= nPositionMs)
      return true;
   else
      return false;
}


bool CMediaSegment::Contains(UINT nPositionMs, int iTemporaryMoveMs)
{
   if ((signed)m_nTargetBeginMs + iTemporaryMoveMs < 0)
      _ASSERT(false); // TODO internal error?

   if (nPositionMs >= m_nTargetBeginMs + iTemporaryMoveMs && nPositionMs < m_nTargetBeginMs + iTemporaryMoveMs + m_nLengthMs)
      return true;
   else
      return false;
}

bool CMediaSegment::IntersectsRange(UINT nFromMs, UINT nLengthMs, int iTemporaryMoveMs)
{
   if ((signed)m_nTargetBeginMs + iTemporaryMoveMs < 0)
      _ASSERT(false); // TODO internal error?

   // Check if this segment has overlapping regions with the range
   // or if it lies completely within the range:

   if (Contains(nFromMs, iTemporaryMoveMs) || Contains(nFromMs + nLengthMs - 1, iTemporaryMoveMs))
      return true;
   else if (m_nTargetBeginMs + iTemporaryMoveMs >= nFromMs && m_nTargetBeginMs + iTemporaryMoveMs + m_nLengthMs < nFromMs + nLengthMs)
      return true;
   else
      return false;

}

bool CMediaSegment::EntirelyWithin(UINT nFromMs, UINT nLengthMs)
{
   return nFromMs <= m_nTargetBeginMs && nFromMs + nLengthMs >= m_nTargetBeginMs + m_nLengthMs;
}

bool CMediaSegment::BelongsTo(CMediaSegment *sgmAfter, int iMoveMs)
{
   if (sgmAfter == NULL || !sgmAfter->m_bIsInitialized)
      return false;

   if (_tcscmp(m_tszFilename, sgmAfter->m_tszFilename) == 0)
   {
      if (m_nTargetBeginMs+m_nLengthMs+iMoveMs == sgmAfter->m_nTargetBeginMs)
         return true;
   }

   return false;
}

bool CMediaSegment::JoinsWith(CMediaSegment *sgmAfter)
{
   if (sgmAfter == NULL || !sgmAfter->m_bIsInitialized)
      return false;

   if (this->BelongsTo(sgmAfter))
   {
      if (m_nSourceBeginMs+m_nLengthMs == sgmAfter->m_nSourceBeginMs)
         return true;
   }

   return false;
}


HRESULT CMediaSegment::CutOut(UINT nFromMs, UINT nLengthMs, bool bMiddleAllowed)
{
   if (!m_bIsInitialized)
      return E_MSM_UNINITIALIZED;

   if (!IntersectsRange(nFromMs, nLengthMs))
      return E_INVALIDARG;

   if (Contains(nFromMs) && Contains(nFromMs+nLengthMs-1))
   {
      if (!bMiddleAllowed)
         return E_INVALIDARG;
      else
      {
         // source unimportant: you can delete parts from the middle by decreasing the length
         m_nLengthMs -= nLengthMs;
         return S_OK;
      }
   }

   UINT nTargetEndMs = m_nTargetBeginMs + m_nLengthMs - 1;
   
   if (m_nTargetBeginMs >= nFromMs && nTargetEndMs < nFromMs+nLengthMs)
   {
      // this segment is totally inside of the range to cut out
      m_nLengthMs = 0;
      m_bIsInitialized = false;
      // it is now empty and shouldn't be used anymore

      return S_FALSE;
   }


   if (Contains(nFromMs))
   {
      UINT nDiff = nTargetEndMs - nFromMs + 1;
      m_nLengthMs -= nDiff;
   }
   else if (Contains(nFromMs+nLengthMs))
   {
      UINT nDiff = (nFromMs+nLengthMs) - m_nTargetBeginMs;
      m_nTargetBeginMs = nFromMs;
      m_nSourceBeginMs += nDiff;
      m_nLengthMs -= nDiff;
   }

   return S_OK;
}
   
HRESULT CMediaSegment::TrimToRange(UINT nFromMs, UINT nLengthMs)
{
   if (!m_bIsInitialized)
      return E_MSM_UNINITIALIZED;

   if (!IntersectsRange(nFromMs, nLengthMs))
      return E_INVALIDARG;

   if (m_nTargetBeginMs < nFromMs)
   {
      UINT nDiff = nFromMs - m_nTargetBeginMs;
      m_nTargetBeginMs += nDiff;
      m_nSourceBeginMs += nDiff;
      m_nLengthMs -= nDiff;
   }

   UINT nTargetEndMs = m_nTargetBeginMs + m_nLengthMs - 1;
   if (nTargetEndMs >= nFromMs + nLengthMs)
   {
      UINT nDiff = nTargetEndMs - (nFromMs + nLengthMs - 1);
      m_nLengthMs -= nDiff;
   }


   return S_OK;
}

HRESULT CMediaSegment::Split(UINT nCutMs, CMediaSegment *sgmTarget)
{
   if (!m_bIsInitialized)
      return E_MSM_UNINITIALIZED;

   if (!Contains(nCutMs))
      return E_INVALIDARG;

   if (sgmTarget == NULL)
      return E_POINTER;

   if (nCutMs == m_nTargetBeginMs)
      return E_INVALIDARG; // *this would become empty in this case

   UINT nDiff = nCutMs - m_nTargetBeginMs;
   this->CloneTo(sgmTarget);
   sgmTarget->m_nSourceBeginMs = m_nSourceBeginMs + nDiff;
   sgmTarget->m_nTargetBeginMs = nCutMs;
   sgmTarget->m_nLengthMs = m_nLengthMs - nDiff;
   
   m_nLengthMs = nDiff;
   
   return S_OK;
}

HRESULT CMediaSegment::Join(CMediaSegment *sgmAfter)
{
   if (sgmAfter == NULL)
      return E_POINTER;

   if (!m_bIsInitialized || !sgmAfter->m_bIsInitialized)
      return E_MSM_UNINITIALIZED;

   if (!this->JoinsWith(sgmAfter))
      return E_INVALIDARG;

   m_nLengthMs += sgmAfter->m_nLengthMs;

   // make the other empty/unusable
   sgmAfter->m_nLengthMs = 0;
   sgmAfter->m_bIsInitialized = false;

   return S_OK;
}


HRESULT CMediaSegment::AppendLepTextData(_TCHAR *tszOutput, CString &strLepFileName, bool bWithDecoration)
{
   if (tszOutput == NULL)
      return E_POINTER;

   if (!m_bIsInitialized)
      return E_MSM_UNINITIALIZED;

   CString csEscapeFileName = 
      StringManipulation::MakeRelativeAndSgml(strLepFileName, m_tszFilename);

   
   if (bWithDecoration)
   {
      _tcscat(tszOutput, _T("<CLIP"));
      tszOutput += _tcslen(tszOutput);
   }

   _stprintf(tszOutput, _T(" file=\"%s\" target-ms=\"%d\" source-ms=\"%d\" length=\"%d\""), 
      csEscapeFileName, m_nTargetBeginMs, m_nSourceBeginMs, m_nLengthMs);
   tszOutput += _tcslen(tszOutput);
   
   if (bWithDecoration)
   {
      _tcscat(tszOutput, _T("></CLIP>"));
      tszOutput += _tcslen(tszOutput);
   }

   return S_OK;
}

HRESULT CMediaSegment::ReadFromSgml(SgmlElement *pSgml, LPCTSTR tszLepFileName)
{
   HRESULT hr = S_OK;

   if (pSgml == NULL)
      return E_POINTER;

   LPCTSTR tszFileName = pSgml->GetValue(_T("file"));
   if (tszFileName != NULL)
   {
      // TODO what about checking for file existance (see CEditorProject::MakeAbsoluteAndCheck())?
      CString csAbsoluteFilename = StringManipulation::MakeAbsolute(tszLepFileName, tszFileName);
   
      _tcscpy(m_tszFilename, (LPCTSTR)csAbsoluteFilename);
   }
   else
      hr = E_MSM_READ_FAILED;

   int iLength = 0;
   if (SUCCEEDED(hr))
      iLength = pSgml->GetIntValue(_T("length"));
   if (iLength > 0)
      m_nLengthMs = iLength;
   else
      hr = E_MSM_READ_FAILED;

   if (SUCCEEDED(hr))
   {
      m_nSourceBeginMs = pSgml->GetIntValue(_T("source-ms"));
      m_nTargetBeginMs = pSgml->GetIntValue(_T("target-ms"));

      // these two could also be unspecified (error) but 
      // we cannot judge that from just the value...
   }

   if (SUCCEEDED(hr))
      m_bIsInitialized = true;

   return hr;
}
