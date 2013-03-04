#if !defined(AFX_MARKSTREAM_H__E6E6EAF7_E1EF_42D2_A293_25CAA5C4F2DF__INCLUDED_)
#define AFX_MARKSTREAM_H__E6E6EAF7_E1EF_42D2_A293_25CAA5C4F2DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define E_MS_WRITE_FAILED     _HRESULT_TYPEDEF_(0x80ef0001L)
#define E_MS_INVALID_DATA     _HRESULT_TYPEDEF_(0x80ef0002L)
#define E_MS_NO_MARK_SELECTED _HRESULT_TYPEDEF_(0x80ef0003L)
#define E_MS_SELECTION_ACTIVE _HRESULT_TYPEDEF_(0x80ef0004L)
#define E_MS_MARK_EXISTS      _HRESULT_TYPEDEF_(0x80ef0005L)

#include "BaseStream.h"
#include "Clipboard.h"
#include "MarkReaderWriter.h"
#include "UndoRedoManager.h"
#include "WhiteboardStream.h"

class CMarkStream : public CBaseStream
{
public:
	CMarkStream();
	virtual ~CMarkStream();
  
   HRESULT InsertMarks(UINT nToMs, UINT nLength, CWhiteboardStream *pWhiteboard, bool bUndoWanted);

   HRESULT IncludeMark(CStopJumpMark *pMark, bool bIsUserAction = true);
   // removes the first mark in this range
   HRESULT RemoveMark(UINT nFromMs, UINT nLengthMs, bool bIsUserAction = true);

   // counts all the jump marks added with "IncludeMark()".
   UINT GetJumpMarkCounter() { return m_nJumpMarkCounter; }

   // returns the stop position if there is a stop mark between last and current time
   bool CheckTime(UINT *pnNewPositionMs, UINT nLastPositionMs);

   bool IsExactStopAt(UINT nTimeMs);

   // returns 0xffffffff if the mark does not exist (anymore)
   UINT GetTimeForId(UINT nId);
   CStopJumpMark* GetMarkForId(UINT nId);

   HRESULT WriteLepEntry(CFileOutput *pOutput);
   HRESULT WriteObjEntry(CFileOutput *pOutput, bool bDoIdent);
   HRESULT ReadLepEntry(SgmlElement *pSgml);

   HRESULT Draw(CDC *pDC, CRect& rect, UINT nFromMs, UINT nLengthMs);

   /**
    * Retrieve the first mark in this range.
    * Currently only useful for changing the label of the mark.
    */
   CStopJumpMark* GetFirstMarkInRange(UINT nFromMs, UINT nLengthMs);
   
   CStopJumpMark* GetFirstStopMarkInRange(UINT nFromMs, UINT nLengthMs);
   CStopJumpMark* GetFirstJumpMarkInRange(UINT nFromMs, UINT nLengthMs);
   CStopJumpMark* GetLastStopMarkInRange(UINT nFromMs, UINT nLengthMs);

	/**
    * Retrieve all target marks in this range.
    */
	HRESULT GetJumpMarksInRegion(CArray<CStopJumpMark *, CStopJumpMark *> &caMarks, 
      UINT nFromMs, UINT nLengthMs);

   /** 
     * Returns S_FALSE if no mark was found (and selected).
     * Otherwise the last mark in this range is remembered for later changes.
     *
     * Note: This also starts the "move mode". During this only moving, 
     * deleting and disabling of
     * this selected element is allowed. And of course painting the stream. 
     * The mode is ended with RemoveSelectedMark(), Reset or CommitMoveSelected().
     */
   HRESULT SelectMark(UINT nFromMs, UINT nLengthMs);
   HRESULT MoveSelectedMark(int iMoveMs);
   HRESULT ResetMoveSelected();
   HRESULT CommitMoveSelected(bool bIsUserAction = true);
   HRESULT EnableSelectedMark(bool bEnable = true);
   bool SelectedMarkIsUsed();
   HRESULT RemoveSelectedMark(bool bIsUserInteraction = true);
   HRESULT GetMovedMarkTimes(int &iFromMs, int &iToMs);// {iFromMs = m_iMovedMarkFromMs; iToMs = m_iMovedMarkToMs;};
   
   /**
     * Returns true if selected mark is at the end of a page.
     * This is required because, in SS, the stop marks (that are automatically inserted) position cannot be changed.
     */
   bool SelectedStopMarkIsDemoDocumentObject();
   void RemoveAllEndOfPageStopMarks();
   int GetMarksCount(){return m_aAllMarks.GetCount();};


protected:

   virtual HRESULT SpecificUndo(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs, UINT nLengthMs);

   virtual HRESULT CutCopyOrDelete(UINT nFromMs, UINT nLengthMs, CClipboard *pClipboard, bool bDoCopy, bool bDoDelete);
   virtual HRESULT InsertRange(UINT nToMs, UINT nLengthMs, BYTE *pData, UINT nByteCount, bool bUndoWanted);

private:
	HRESULT FindMarksInRegion(CArray<CStopJumpMark *, CStopJumpMark *> &caMarks, 
      UINT nFromMs, UINT nLengthMs, UINT *pnStartIndex, bool bSelectedInOrder = true);
   HRESULT RemoveAndIntegrateMovedElement();

   CArray<CStopJumpMark, CStopJumpMark> m_aAllMarks;

   CStopJumpMark *m_pSelectedMark;
   UINT m_nSelectedIndex;
	CStopJumpMark m_MovedMark;
   bool m_bSelectedDisabled;

   UINT m_nJumpMarkCounter;

   CBitmap m_stopmarkBitmap[4];
   CBitmap m_targetmarkBitmap[2];
};

#endif // !defined(AFX_MARKSTREAM_H__E6E6EAF7_E1EF_42D2_A293_25CAA5C4F2DF__INCLUDED_)
