// ClipSegment.h: Schnittstelle für die Klasse CClipSegment.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIPSEGMENT_H__F80959DF_291A_4DBD_8513_8141618BBF5B__INCLUDED_)
#define AFX_CLIPSEGMENT_H__F80959DF_291A_4DBD_8513_8141618BBF5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_CLIP_LABEL_LENGTH 200
#define MAX_KEYWORDS_LENGTH   500

#define E_CSG_WRONG_CLASS      _HRESULT_TYPEDEF_(0x80ea0001L)
#define E_CSG_NO_VIDEO_FOUND   _HRESULT_TYPEDEF_(0x80ea0001L)
#define E_CSG_CLIP_INFO_FAILED _HRESULT_TYPEDEF_(0x80ea0001L)


#include "MediaSegment.h"

class CClipSegment : public CMediaSegment  
{
public:
	CClipSegment();
	virtual ~CClipSegment();

   // Reasons for the decision to reuse "CClipInfo" here:
   // CClipInfo might not be very elegant and it is code that belongs in this class
   // but it is working and it has not much to do with the CPreviewSegments
   // (which are to be replaced) so it can stay for the moment.
   //
   // And we do not simply store a pointer here (but replicate the
   // information) as the interaction
   // with CArray<CClipSegment> is very problematic with such a pointer.
   // Copies will be created in standard and non-standard ways (copy a
   // memory block) everywhere. And often you get a segmentation fault
   // when deleting the pointer object (twice).

   HRESULT Init(CClipInfo *pClipInfo);
   HRESULT FillClipInfo(CClipInfo *pClipInfo);

   virtual HRESULT CloneTo(CMediaSegment *sgmTarget);
   HRESULT CloneClipDataTo(CClipSegment *sgmTarget);

   LPCTSTR GetLabel() { return (const _TCHAR*)&m_tszLabel; }
   bool HasAudio();
   int GetVideoStreamSelector() { return m_iVideoStreamIndex; }
   int GetTempOriginalAudioVolume() { return m_iAudioVolume; }

   virtual HRESULT AppendLepTextData(_TCHAR *tszOutput, CString &csLepFileName, bool bWithDecoration = true);
   virtual HRESULT ReadFromSgml(SgmlElement *pSgml, LPCTSTR tszLepFileName);

   // PZI: information concerning structured clips
   bool GetStructureAvailable() { return m_bStructureAvailable; }
   void SetStructureAvailable(bool available) { m_bStructureAvailable = available; }
   bool GetImagesAvailable () { return m_bImagesAvailable; }
   void SetImagesAvailable(bool available) { m_bImagesAvailable = available; }
   bool GetFulltextAvailable () { return m_bFulltextAvailable; }
   void SetFulltextAvailable(bool available) { m_bFulltextAvailable = available; }

private:
   UINT m_nClassId;

   int m_iVideoStreamIndex;
   int m_iAudioStreamIndex; 
   int m_iAudioVolume;
   _TCHAR m_tszLabel[MAX_CLIP_LABEL_LENGTH+1];
   _TCHAR m_tszAudioFilename[MAX_PATH+1];
   _TCHAR m_tszKeywords[MAX_KEYWORDS_LENGTH+1];

   // PZI: information concerning structured clips
   // must be copied from CClipInfo to be able to copy it back to new CClipInfo object generated from CCLipSegments
   bool m_bStructureAvailable;
   bool m_bImagesAvailable;
   bool m_bFulltextAvailable;
};


#endif // !defined(AFX_CLIPSEGMENT_H__F80959DF_291A_4DBD_8513_8141618BBF5B__INCLUDED_)


