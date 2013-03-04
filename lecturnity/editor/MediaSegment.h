// Segment.h: Schnittstelle für die Klasse CSegment.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEGMENT_H__C741E78C_B6DA_488E_B018_4B72A9957F4A__INCLUDED_)
#define AFX_SEGMENT_H__C741E78C_B6DA_488E_B018_4B72A9957F4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVStreams.h"
#include "SgmlParser.h"

#define E_MSM_UNINITIALIZED _HRESULT_TYPEDEF_(0x80fb0001L)
#define E_MSM_READ_FAILED   _HRESULT_TYPEDEF_(0x80fb0002L)


class CMediaSegment  
{
public:
	CMediaSegment();
	virtual ~CMediaSegment();

   virtual HRESULT Init(const _TCHAR *tszFile, UINT nTargetBeginMs = 0, UINT nMediaLengthMs = 0);
   virtual HRESULT CloneTo(CMediaSegment *sgmTarget);

   UINT GetTargetBegin() { return m_nTargetBeginMs; }
   // TODO: these get methods necessary? Source begin for example
   // is currently (only) used for creating the export format for clips.
   UINT GetSourceBegin() { return m_nSourceBeginMs; }
   UINT GetLength() { return m_nLengthMs; }

   LPCTSTR GetFilename() { return (const _TCHAR*)&m_tszFilename; }
  
   bool BeginsAtOrAfter(UINT nPositionMs);
   bool Contains(UINT nPositionMs, int iTemporaryMoveMs = 0);
   bool IntersectsRange(UINT nFromMs, UINT nLengthMs, int iTemporaryMoveMs = 0);
   bool EntirelyWithin(UINT nFromMs, UINT nLengthMs);
   // iMoveMs != 0 if this segment is (temporarily) moved
   bool BelongsTo(CMediaSegment *sgmAfter, int iMoveMs = 0);
   bool JoinsWith(CMediaSegment *sgmAfter);
   
   HRESULT IncrementTargetBegin(UINT nMoveMs);
   HRESULT DecrementTargetBegin(UINT nMoveMs);
   HRESULT IncreaseLength(UINT nIncreaseMs);
   
   HRESULT Split(UINT nCutMs, CMediaSegment *sgmTarget);
   HRESULT Join(CMediaSegment *sgmAfter);

   HRESULT CutOut(UINT nFromMs, UINT nLengthMs, bool bMiddleAllowed = false);
   // and the opposite:
   HRESULT TrimToRange(UINT nFromMs, UINT nLengthMs);

   virtual HRESULT AppendLepTextData(_TCHAR *tszOutput, CString &csLepFileName, bool bWithDecoration = true);
   virtual HRESULT ReadFromSgml(SgmlElement *pSgml, LPCTSTR tszLepFileName);
  

protected:
   bool   m_bIsInitialized;

private:
   _TCHAR m_tszFilename[MAX_PATH + 1];
   UINT   m_nSourceBeginMs;
   UINT   m_nTargetBeginMs;
   UINT   m_nLengthMs;
};

#endif // !defined(AFX_SEGMENT_H__C741E78C_B6DA_488E_B018_4B72A9957F4A__INCLUDED_)
