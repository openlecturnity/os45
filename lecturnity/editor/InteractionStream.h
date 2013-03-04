#if !defined(AFX_INTERACTIONSTREAM_H__ED4B3FA1_5E04_43DC_B6C8_FBDA087A60B0__INCLUDED_)
#define AFX_INTERACTIONSTREAM_H__ED4B3FA1_5E04_43DC_B6C8_FBDA087A60B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// for specific undo actions
#define IACTION_AREA_CHANGE 1
#define IACTION_STACKING_CHANGE 2
#define IACTION_TIMES_CHANGE 3


#include "BaseStream.h"
#include "DynamicObject.h"
#include "InteractionAreaEx.h"
#include "GraphicalObject.h"
#include "ClickObject.h"
#include "MoveableObjects.h"
#include "WhiteboardView.h"

class CInteractionStream : public CBaseStream  
{
public:
   static HRESULT CreateInteractionObject(
      CInteractionArea *pSource, CInteractionAreaEx **ppTarget, CEditorProject *pProject, UINT nToMs, 
      CInteractionAreaEx *pPrevious = NULL);

   static int CompareByStacking(const void *pOne, const void *pTwo);

   
   CInteractionStream();
	virtual ~CInteractionStream();

   HRESULT ShowHideDynamics(UINT nPositionMs, bool bPreviewing);

   HRESULT Draw(CDC *pDC, UINT nPositionMs, CPoint *ptOffsets, double dZoomFactor, bool bPreviewing);

   HRESULT Insert(UINT nToMs, UINT nLength, CWhiteboardStream *pWbStream);
   HRESULT InsertDenver(UINT nToMs, UINT nLengthMs);
   HRESULT ResolveTimes(UINT nMoveMs);
   HRESULT InsertSingle(CInteractionAreaEx *pInteraction, bool bIsUserInteraction = true);
   HRESULT DeleteSingle(UINT nHashKey, bool bIsUserInteraction = true);

   HRESULT ModifyArea(UINT nOldHashKey, CInteractionAreaEx *pNewInteraction, bool bIsUserInteraction = true);
   //HRESULT ChangePosition(UINT nHashKey, CRect *prcArea);
   HRESULT MoveToFront(UINT nHashKey);
   HRESULT MoveToBack(UINT nHashKey);
   // without nPositionMs it is undefined what will be the next upper or lower element
   HRESULT MoveOneUp(UINT nHashKey);
   HRESULT MoveOneDown(UINT nHashKey);

   /** Returns S_FALSE if there was nothing to reset. */
   HRESULT ResetAfterPreview();

   HRESULT ResetOtherRadioButtons(CDynamicObject *pSource);

   virtual HRESULT UndoAction(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs = 0, UINT nLengthMs = 0);

   virtual HRESULT ReleaseData(BYTE *pData, UINT nDataLength, bool bDeleteCompletely, 
      UndoActionId idActionCode, UINT nPositionMs, UINT nLengthMs);

   HRESULT FillCopyMouseActions(CArray<CMouseAction *, CMouseAction *> *paActions);

   /** Finds all the areas for a certain time. */
   HRESULT FindInteractions(UINT nPositionMs, bool bForPreview, bool bOnlyVisible,
      CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paTarget);

   /** Finds all areas for a time range. */
   HRESULT FindInteractions(CTimePeriod *pPeriod, bool bQuestionOnly,
      CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paTarget);
   
   /** Finds all the visible areas for a certain time and selection rectangle. Call only during "edit". */
   HRESULT FindInteractions(UINT nPositionMs, CRect *prcSelection,
      CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paTarget);

   /** Find the topmost visible area for a certain time and mouse position. */
   CInteractionAreaEx *FindTopInteraction(UINT nPositionMs, bool bForPreview, CPoint *pptMousePos);

   /** Find related demo objects interaction*/
   HRESULT FindRelatedDemoObjects(int iTimestamp, CArray<CInteractionAreaEx*, CInteractionAreaEx*> *paTarget, int iSourceID = -1);// bool bSourceIsCallout, bool bSourceIsMark = false);
   HRESULT ChangeRelatedDemoObjectsTimes(int iOrig ,int iFrom, int iTo, CGraphicalObject* pInteraction = NULL);
   HRESULT DeleteRelatedDemoObject(int iFrom);
 
   CInteractionAreaEx* GetInteractionWithKey(UINT nHashKey);
   
   bool ActivateObjects(UINT nPositionMs, bool bPreviewing);

   HRESULT ExecuteAction(CInteractionAreaEx *pInteraction, MouseActionId idMouseAction);


   HRESULT WriteLepEntry(CFileOutput *pOutput);
   HRESULT WriteObjEntry(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, LPCTSTR tszExportPrefix);
   HRESULT ReadLepEntry(SgmlElement *pSgml, LPCTSTR tszLepFileName);


   HRESULT CheckIntegrity(CEditorDoc *pDoc);

   /**
    * Removes all m_mapInteractions values that have m_bDemoDocumentObject set to true
    * Insert rollback point for the last item deleted
    */
   void DeleteAllDemoDocumentObjects();
   bool HasDemoDocumentObjects();
   int GetInteractionsCount(){return m_mapInteractions.GetCount();};
   bool InteractionsAreOnlyClickInfos();

protected:
   
   virtual HRESULT SpecificUndo(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs, UINT nLengthMs);

   virtual HRESULT CutCopyOrDelete(UINT nFromMs, UINT nLengthMs, CClipboard *pClipboard, bool bDoCopy, bool bDoDelete);
   virtual HRESULT InsertRange(UINT nToMs, UINT nLengthMs, BYTE *pData, UINT nByteCount, bool bUndoWanted);

private:

   HRESULT FindInteractions(UINT nPositionMs, CTimePeriod *pPeriod, 
      bool bForPreview, bool bOnlyVisible, bool bQuestionOnly,
      CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paTarget, 
      CPoint *pptMousePos, CRect *prcSelection, CInteractionAreaEx **ppTopMost);

   HRESULT ChangeTimes(CInteractionAreaEx *pInteraction, int iChangeMs);
   HRESULT ChangeStacking(UINT nHashKey, bool bToFront, bool bValueValid = false, 
      int iNewStackingValue = 0, bool bLookForOtherArea = false, bool bLookBelow = true);
   CInteractionAreaEx* GetNextInteraction(CInteractionAreaEx *pStartInteraction, bool bLookBelow);

   HRESULT JoinAreas(CInteractionAreaEx *pExisting, CInteractionAreaEx *pEnlarge, UINT nMoveMs);

   HRESULT DeleteArray(CInteractionAreaEx **ppInteractions, UINT nElementCount, bool bDeleteCompletely = true);

   void PrintState();

   CMap<UINT, UINT, CInteractionAreaEx *, CInteractionAreaEx *> m_mapInteractions;
   CInteractionAreaEx **m_ppLastClipboardData;
   UINT m_nLastClipboardLength;
};

#endif // !defined(AFX_INTERACTIONSTREAM_H__ED4B3FA1_5E04_43DC_B6C8_FBDA087A60B0__INCLUDED_)
