// ClipStream.h: Schnittstelle für die Klasse CClipStream.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIPSTREAM_H__9D4076E0_B747_4596_A3D8_E5A5CC4D36D1__INCLUDED_)
#define AFX_CLIPSTREAM_H__9D4076E0_B747_4596_A3D8_E5A5CC4D36D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define E_CS_INVALID_DATA     _HRESULT_TYPEDEF_(0x80ed0001L)
#define E_CS_CLIP_INFO_FAILED _HRESULT_TYPEDEF_(0x80ed0002L)
#define E_CS_REPLACE_CLIP     _HRESULT_TYPEDEF_(0x80ed0003L)
#define E_CS_NO_CLIP_SELECTED _HRESULT_TYPEDEF_(0x80ed0004L)
#define E_CS_SELECTION_ACTIVE _HRESULT_TYPEDEF_(0x80ed0005L)
#define E_CS_WRITE_FAILED     _HRESULT_TYPEDEF_(0x80ed0006L)


#include "AviSettings.h"
#include "BaseStream.h"
#include "Clipboard.h"
#include "ClipSegment.h"
#include "MediaSegment.h"
#include "UndoRedoManager.h"


class CMergedClip : public CMediaSegment
{
public:
   CMergedClip();
   CMergedClip(const CMergedClip &other);
   virtual ~CMergedClip();

   CMergedClip& operator= (const CMergedClip &other);

   /**
    * Add the segments that form one (output) clip/file.
    * The segments must be added in order and they must belong directly (BelongsTo())
    * together. There is no check for that.
    * Only a pointer is stored. There will be no copy made.
    */
   HRESULT AddSegmentAsPointer(CClipSegment *pSegment);

   int GetVideoStreamSelector();

   // PZI: write clip structuring status to LRD
   bool WriteClipStructure(CString &line);
   bool HasStructuredClips();

private:
   CArray<CClipSegment *, CClipSegment *> m_aClipParts;
};

class CClipStreamR : public CBaseStream
{
public:
	CClipStreamR();
	virtual ~CClipStreamR();

   // TODO: remove use of CClipInfo
   HRESULT FillClipInfo(UINT nClipNumber, CClipInfo *pClipInfo);
   HRESULT FillClipInfoAt(UINT nPositionMs, CClipInfo *pClipInfo);
   HRESULT ChangeClipInfoAt(UINT nPositionMs, CClipInfo *pClipInfo);
   // for denver documents:
   HRESULT ChangeAllClipInfo(CClipInfo *pClipInfo);
   // TODO: remove; only temporary (due to usage of CClipInfo)
   UINT GetSourceBegin(UINT nClipNumber);
   bool HasClipAudioAt(UINT nPositionMs);
   // is there any segment with audio (one is enough)
   bool HasAudio();
   // TODO remove direct access to array (currently necessary for export)
   CArray<CClipSegment, CClipSegment>* GetSegments() { return &m_aAllClipSegments; }
   bool BelongsToNextSegment(UINT nClipNumber);
   LPCTSTR GetFilenameAt(UINT nPositionMs);
   
   // TODO use these merged clips also for normal storage and operation.
   // Currently they are only used for export tasks.
   HRESULT MergeClipsForExport(CMap<CString, LPCTSTR, CAviSettings *, CAviSettings *> *pMapClipSettings);

   // TODO remove legacy support method
   HRESULT FillExportClipTimes(CArray<UINT, UINT> *paClipTimes);


   // TODO: find better solution (something with information hiding) for pClipAreas
   HRESULT Draw(CDC *pDC, CRect& rect, UINT nFromMs, UINT nLengthMs, CArray<CRect, CRect> *pClipAreas, int yOffset = 0);

   /**
    * Does NOT increase the length of the stream.
    * So this insertion can replace other clips or parts thereof.
    * And it will do that if bForceOverwrite == true. Otherwise an error
    * code is returned.
    */
   HRESULT IncludeClip(UINT nToMs, CClipInfo *pClipInfo, bool bForceOverwrite);
   HRESULT DeleteClipAt(UINT nPositionMs);

   HRESULT InsertClips(UINT nToMs, UINT nLengthMs, CClipStream *pOldClipStream, bool bUndoWanted);

   bool IsEmpty();
   UINT GetExportClipFileCount();

   /**
    * Determines all the clip segments belonging to the clip at that position.
    * Starts the "move mode". Only permitted methods: MoveSelectedClip(), 
    * EndMoveSelected() and Draw().
    */
   HRESULT SelectClipAt(UINT nPositionMs);
   HRESULT MoveSelectedClip(int iDiffMs);
   HRESULT CommitMoveSelected(bool bIsUserAction = true);

   HRESULT GetClipTimesAt(UINT nClickMs, UINT *pnClipStartMs, UINT *pnClipLengthMs);

   virtual HRESULT SpecificUndo(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs, UINT nLengthMs);


   HRESULT WriteLrdEntry(CFileOutput *pOutput, CString &csPrefix, bool bIsDenver);
   HRESULT WriteLepEntry(CFileOutput *pOutput, CString &strLepFileName);
   HRESULT ReadLepEntry(SgmlElement *pSgml, LPCTSTR tszLepFileName);

   // PZI: write clip structuring status
   HRESULT WriteClipStructure(CFileOutput *pOutput);
   bool HasStructuredClips();
   bool LoadIconForClip(UINT uiID);
   CXTPImageManagerIcon* GetIconForClip();
   void SetBackgroundImg( CXTPOffice2007Image* pImageBackgroundClip);

   /*
    * If the document is a structured screengrabbing
    * return the clip.
    */
   CMergedClip *GetClipToExport();
protected:
   HRESULT CutCopyOrDelete(UINT nFromMs, UINT nLengthMs, 
                           CClipboard *pClipboard, bool bDoCopy, bool bDoDelete);
   HRESULT InsertRange(UINT nToMs, UINT nLengthMs, BYTE *pData, UINT nByteCount, bool bUndoWanted);

private:
   /**
    * A clip on the UI (one rectangle) can consists of more than one segment.
    * This method identifies them.
    */
   HRESULT FindTotalClipAt(UINT nPositionMs, UINT *pnStartIndex, UINT *pnElementCount);

   /**
    * Finds the first index with a segment that contains nPositionMs or begins after it.
    * Return value is last index in the array + 1 if no such segment was found.
    */
   UINT FindInsertIndex(UINT nPositionMs);

   /**
    * Finds the range of segments intersecting this range.
    * If there is no intersection pnStartIndex will become the index of the first
    * segment after the range and pnElementCount remains 0.
    */
   bool ContainsSegments(UINT nFromMs, UINT nLengthMs,
      UINT *pnStartIndex = NULL, UINT *pnElementCount = NULL);
   HRESULT JoinSegments(UINT nStartIndex = 0, UINT nRangeLength = 0);
   
   
   CArray<CClipSegment, CClipSegment> m_aAllClipSegments;
   CArray<CMergedClip, CMergedClip>   m_aMergedSegments;

   UINT m_nSelectionStart;
   UINT m_nSelectionLength;
   int  m_iMoveDistanceMs;
   UINT m_nUpperBoundary;
   UINT m_nLowerBoundary;

   LOGFONT m_TextFont;

	CBitmap m_bmpBackground;

   CXTPOffice2007Image* m_pImageBackgroundClip;
   CXTPImageManagerIconHandle m_iconHandle;
   CXTPImageManagerIcon* m_pIcon;
   bool m_bIconClip;
};



#endif // !defined(AFX_CLIPSTREAM_H__9D4076E0_B747_4596_A3D8_E5A5CC4D36D1__INCLUDED_)
