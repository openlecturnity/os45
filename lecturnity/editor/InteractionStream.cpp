#include "stdafx.h"
#include "InteractionStream.h"
#include "editorDoc.h"
#include "BlankGroup.h"
#include "ChangeableObject.h"
#include "DynamicObject.h"
#include "MoveableObjects.h"


//static 
HRESULT CInteractionStream::CreateInteractionObject(CInteractionArea *pSource, 
                                                    CInteractionAreaEx **ppTarget, CEditorProject *pProject, UINT nToMs, 
                                                    CInteractionAreaEx *pPrevious)
{
   HRESULT hr = S_OK;

   if (pSource == NULL || *ppTarget != NULL)
      return E_POINTER;

   bool bPreviousIsBlankGroup = pPrevious != NULL && pPrevious->GetClassId() == INTERACTION_CLASS_BLANKS;
   bool bPreviousIsDemoCallout = pPrevious != NULL && pPrevious->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT && pPrevious->IsDemoDocumentObject();

   if (pSource->IsSupport())
   {
      DrawSdk::DrawObject *pObject = pSource->GetSupportObject();
      // a copy is made below
      
      bool bIsTypedText = 
         pObject->GetType() == DrawSdk::TEXT && ((DrawSdk::Text *)pObject)->HasTextType();
      bool bIsTarget = pObject->GetType() == DrawSdk::TARGET;
      
      if (bIsTypedText)
      {
         DrawSdk::Text *pText = (DrawSdk::Text *)pObject;

         if (pText->HasChangeableType())
         {
            CChangeableObject *pChange = new CChangeableObject();
            hr = pChange->Init(pObject, pProject);
            
            if (SUCCEEDED(hr))
               *ppTarget = pChange;
         }
         else
         {
            if (pText->GetTextType() != DrawSdk::TEXT_TYPE_BLANK_SUPPORT)
            {
                // TODO this is a bit double code
               // and it is a misuse of "CMoveableObjects" as these objects (typed text)
               // are not moveable normally.
               
               CMoveableObjects *pMoveableThis = new CMoveableObjects();
               hr = pMoveableThis->Init(pObject, pProject);
               
               if (SUCCEEDED(hr))
                  *ppTarget = pMoveableThis;
            }
            else
            {
               CBlankGroup *pGroup = NULL;

               if (!bPreviousIsBlankGroup)
               {
                  pGroup = new CBlankGroup();

                  hr = pGroup->Init(pObject, NULL, pProject);

                  if (SUCCEEDED(hr))
                     *ppTarget = pGroup;
               }
               else
               {
                  pGroup = (CBlankGroup *)pPrevious;
                  hr = pGroup->AddDuringParsing(pObject);
               }
            }
         }
         
      }
      else if (pObject->IsDndMoveable() || bIsTarget)
      {
         CMoveableObjects *pMoveableThis = new CMoveableObjects();
         hr = pMoveableThis->Init(pObject, pProject);
         
         if (SUCCEEDED(hr))
            *ppTarget = pMoveableThis;
      }
      else
      {
         // else ignore; shouldn't happen
         _ASSERT(false);
      }
   }
   else
   {
      if (pSource->IsRadioButton() || pSource->IsCheckBox())
      {
         CDynamicObject* pDynamic = new CDynamicObject();
         hr = pDynamic->Init(pSource, pProject);
         if (SUCCEEDED(hr))
            *ppTarget = pDynamic;
      }
      else if (pSource->IsTextField())
      {
         // double code'ish (see above)

         CBlankGroup *pGroup = NULL;

         if (!bPreviousIsBlankGroup)
         {
            pGroup = new CBlankGroup();
            if (SUCCEEDED(hr))
               hr = pGroup->Init(NULL, pSource, pProject);
            
            if (SUCCEEDED(hr))
               *ppTarget = pGroup;
         }
         else
         {
            pGroup = (CBlankGroup *)pPrevious;
            
            hr = pGroup->AddDuringParsing(pSource, pProject);
         }
         
      }
      else if(pSource->IsCallout())
      {
          CGraphicalObject *pCallout = new CGraphicalObject();
          hr = pCallout->Init(pSource, pProject);
          if(SUCCEEDED(hr))
              *ppTarget = pCallout;
      }
      else if(pSource->IsClickObject())
      {
          CClickObject *pClickObject = new CClickObject();
          hr = pClickObject->Init(pSource, pProject);
          if(SUCCEEDED(hr))
              *ppTarget = pClickObject;
      }
      else
      {
         // is area or button
         CInteractionAreaEx *pArea = NULL;
         if (bPreviousIsDemoCallout && pSource->IsButton() ) {
             pArea = (CGraphicalObject*) pPrevious;
             pArea->AddDuringParsing(pSource, pProject, nToMs);
             //sdfgf;
         } else {
             pArea = new CInteractionAreaEx();
             hr = pArea->Init(pSource, pProject, nToMs);
             if (SUCCEEDED(hr))
                 *ppTarget = pArea;
         }
      }
   }

   return hr;
}


CInteractionStream::CInteractionStream()
{
   Gdiplus::ARGB outlineColor = RGB(255, 255, 0);
   Gdiplus::ARGB fillColor = RGB(255, 0, 255);


   //m_pRectangle = new DrawSdk::Rectangle(10, 10, 100, 100, outlineColor, fillColor, 2);

   m_mapInteractions.RemoveAll();
   m_ppLastClipboardData = NULL;
   m_nLastClipboardLength = 0;
}

CInteractionStream::~CInteractionStream()
{
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aFlatNormal;
   aFlatNormal.SetSize(0, m_mapInteractions.GetCount());
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aFlatFeedback;
   aFlatFeedback.SetSize(0, m_mapInteractions.GetCount());

   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL)
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      if (pInteraction->GetClassId() == INTERACTION_CLASS_FEEDBACK)
         aFlatFeedback.Add(pInteraction);
      else
         aFlatNormal.Add(pInteraction);

      // BUGFIX #3105:
      // On some documents with questions there is a crash upon "New" or "Exit".
      // I don't know why and when this happens but the splitting above and below
      // in normal and feedback objects seems to be solving the problem.

      //delete pInteraction;
   }
   m_mapInteractions.RemoveAll();

   for (int i=0; i<aFlatNormal.GetSize(); ++i)
      delete aFlatNormal[i];

   for (i=0; i<aFlatFeedback.GetSize(); ++i)
      delete aFlatFeedback[i];
   

   if (m_ppLastClipboardData != NULL)
      DeleteArray(m_ppLastClipboardData, m_nLastClipboardLength);
}

HRESULT CInteractionStream::ShowHideDynamics(UINT nPositionMs, bool bPreviewing)
{
   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL)
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      
      // shows or hides the native components of the dynamic objects;
      // this works surprisingly well as CWnd::ShowWindow()
      // handles redraws itself
      if (pInteraction->GetClassId() == INTERACTION_CLASS_DYNAMIC)
         ((CDynamicObject *)pInteraction)->ShowHide(nPositionMs, bPreviewing);
      else if (pInteraction->GetClassId() == INTERACTION_CLASS_BLANKS)
         ((CBlankGroup *)pInteraction)->ShowHide(nPositionMs, bPreviewing);
      else if (pInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT)
          ((CGraphicalObject*)pInteraction)->HideVisibleEditControl(nPositionMs);
   }

   return S_OK;
}


HRESULT CInteractionStream::Draw(CDC *pDC, UINT nPositionMs, CPoint *ptOffsets, double dZoomFactor, bool bPreviewing)
{
   DrawSdk::ZoomManager zoomManager(dZoomFactor);

   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aAreas;
   HRESULT hr = FindInteractions(nPositionMs, bPreviewing, true, &aAreas); // sorted by stacking order

   for (int i=0; i<aAreas.GetSize(); ++i)
      aAreas[i]->Draw(pDC, ptOffsets, &zoomManager, bPreviewing);

   return S_OK;
}

// static
int CInteractionStream::CompareByStacking(const void *pOneVoid, const void *pTwoVoid)
{
   CInteractionAreaEx **ppOne = (CInteractionAreaEx **)pOneVoid;
   CInteractionAreaEx **ppTwo = (CInteractionAreaEx **)pTwoVoid;

   if ((*ppOne)->GetStackingOrder() < (*ppTwo)->GetStackingOrder())
      return -1;
   else if ((*ppOne)->GetStackingOrder() > (*ppTwo)->GetStackingOrder())
      return 1;
   else
      return 0;
}

HRESULT CInteractionStream::FillCopyMouseActions(CArray<CMouseAction *, CMouseAction *> *paActions)
{
   if (paActions == NULL)
      return E_POINTER;

   int iBeginSize = paActions->GetSize();

   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL)
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      if (pInteraction->GetClickAction() != NULL && pInteraction->GetClickAction()->GetPath() != NULL)
         paActions->Add(pInteraction->GetClickAction());

      if (pInteraction->GetDownAction() != NULL && pInteraction->GetDownAction()->GetPath() != NULL)
         paActions->Add(pInteraction->GetDownAction());

      if (pInteraction->GetUpAction() != NULL && pInteraction->GetUpAction()->GetPath() != NULL)
         paActions->Add(pInteraction->GetUpAction());

      if (pInteraction->GetEnterAction() != NULL && pInteraction->GetEnterAction()->GetPath() != NULL)
         paActions->Add(pInteraction->GetEnterAction());

      if (pInteraction->GetLeaveAction() != NULL && pInteraction->GetLeaveAction()->GetPath() != NULL)
         paActions->Add(pInteraction->GetLeaveAction());

   }

   return paActions->GetSize() > iBeginSize ? S_OK : S_FALSE;
}

HRESULT CInteractionStream::FindInteractions(UINT nPositionMs, 
                                             bool bForPreview, bool bOnlyVisible,
                                             CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paTarget)
{
   return FindInteractions(nPositionMs, NULL, bForPreview, bOnlyVisible, false, paTarget, NULL, NULL, NULL);
}

HRESULT CInteractionStream::FindInteractions(CTimePeriod *pPeriod, bool bQuestionOnly,
                                             CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paTarget)
{
   if (pPeriod == NULL)
      return E_POINTER;

   return FindInteractions(0, pPeriod, false, false, bQuestionOnly, paTarget, NULL, NULL, NULL);
}

HRESULT CInteractionStream::FindInteractions(UINT nPositionMs, CRect *prcSelection,
                                             CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paTarget)
{
   return FindInteractions(nPositionMs, NULL, false, true, false, paTarget, NULL, prcSelection, NULL);
}

CInteractionAreaEx *CInteractionStream::FindTopInteraction(UINT nPositionMs, 
                                                           bool bForPreview, CPoint *pptMousePos)
{
   CInteractionAreaEx *pArea = NULL;
   HRESULT hr = FindInteractions(nPositionMs, NULL, bForPreview, true, false, NULL, pptMousePos, NULL, &pArea);

   return pArea; // will be NULL for any error
}

HRESULT CInteractionStream::FindRelatedDemoObjects(int iTimestamp, CArray<CInteractionAreaEx*, CInteractionAreaEx*> *paTarget,  int iSourceID) {//bool bSourceIsCallout, bool bSourceIsMark /*= false*/){
   
   
    CArray<CInteractionAreaEx*, CInteractionAreaEx*>aAllTimestampInteractions;

    HRESULT hr = FindInteractions(iTimestamp, false, false, &aAllTimestampInteractions);
    if (SUCCEEDED(hr) && aAllTimestampInteractions.GetCount() >= 1){ // at least 1 demo objects should be at the given timestamp
        for(int i = 0; i < aAllTimestampInteractions.GetCount(); i++) {
            CInteractionAreaEx* pInteraction = aAllTimestampInteractions.GetAt(i);
			if(pInteraction->IsDemoDocumentObject() && pInteraction->GetVisibilityStart() + pInteraction->GetVisibilityLength() - 1 == iTimestamp) {
                if (iSourceID == -1) {
                    paTarget->Add(pInteraction);
                } else if (pInteraction->GetClassId() != (InteractionClassId) iSourceID) {
                    //if (pInteraction->GetClassId() == INTERACTION_CLASS_BUTTON)
                        paTarget->Add(pInteraction);
                } /*else {
                    if (pInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT)
                        paTarget->Add(pInteraction);
                }*/
            }
        }
        return paTarget->GetCount() > 0 ? S_OK : S_FALSE;
    } else {
        return S_FALSE;
    }
}
HRESULT CInteractionStream::ChangeRelatedDemoObjectsTimes(int iOrig ,int iFrom, int iTo , CGraphicalObject* pInteraction) {
    HRESULT hr = E_FAIL;
    CGraphicalObject* pGraphicalObject = pInteraction;
    if (pGraphicalObject == NULL){
		CArray<CInteractionAreaEx*, CInteractionAreaEx*>aRelatedDemoObjects;
		hr = FindRelatedDemoObjects(iTo, &aRelatedDemoObjects);
		if (SUCCEEDED(hr) && aRelatedDemoObjects.GetCount() == 0){
			hr = FindRelatedDemoObjects(iOrig, &aRelatedDemoObjects);
			if (SUCCEEDED(hr) && aRelatedDemoObjects.GetCount() == 1) {
				pGraphicalObject = (CGraphicalObject*)aRelatedDemoObjects.GetAt(0);
			}
		}
    }
    //if (SUCCEEDED(hr) && aRelatedDemoObjects.GetCount() == 1) {
    //    for(int i = 0; i < aRelatedDemoObjects.GetCount() && SUCCEEDED(hr); i++){
    //        //hr = ChangeTimes(aRelatedDemoObjects.GetAt(i), iTo - iFrom);
    //        //CInteractionAreaEx* pDemoButton = ((CGraphicalObject*)aRelatedDemoObjects.GetAt(i))->GetDemoButton();

    //        if (iTo > iFrom){
    //            hr = aRelatedDemoObjects.GetAt(i)->IncrementTimes(iTo - iFrom);
    //            /*if (pDemoButton != NULL)
    //                pDemoButton->IncrementTimes(iTo - iFrom);*/
    //        } else {
    //            hr = aRelatedDemoObjects.GetAt(i)->DecrementTimes(iFrom - iTo);
    //            /*if (pDemoButton != NULL)
    //                pDemoButton->IncrementTimes(iFrom - iTo);*/
    //        }
    //    }
    //}
    // TODO soare: cleanup
    if (pGraphicalObject != NULL) {
        CTimePeriod ctp;
        int iStart, iEnd;
        if (iOrig == -1) {
            iStart = iFrom;
            iEnd = iTo;
        } else {
            iStart = pGraphicalObject->GetVisibilityStart() + iTo - iFrom;
			if(iStart < 0)
				iStart = 0;
            iEnd = iStart + pGraphicalObject->GetVisibilityLength() - 1;
        }
        ctp.Init(INTERACTION_PERIOD_TIME, iStart, iEnd);
        pGraphicalObject->SetVisibilityPeriod(&ctp);
		hr = S_OK;
    }
	else
		hr = E_FAIL;
    return hr;
}

HRESULT CInteractionStream::DeleteRelatedDemoObject(int iFrom) {
    HRESULT hr = S_OK;
    CArray<CInteractionAreaEx*, CInteractionAreaEx*>aRelatedDemoObjects;
    hr = FindRelatedDemoObjects(iFrom, &aRelatedDemoObjects);
    if (SUCCEEDED(hr) && aRelatedDemoObjects.GetCount() == 1) {
        hr = DeleteSingle(aRelatedDemoObjects.GetAt(0)->GetHashKey());
    }
    return hr;
}


// private
HRESULT CInteractionStream::FindInteractions(UINT nPositionMs, CTimePeriod *pPeriod,
                                             bool bForPreview, bool bOnlyVisible, bool bQuestionOnly,
                                             CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paTarget, 
                                             CPoint *pptMousePos, CRect *prcSelection, CInteractionAreaEx **ppTopMost)
{
   if (paTarget == NULL && ppTopMost == NULL)
      return E_POINTER;

   if (paTarget != NULL && paTarget->GetSize() > 0)
      return E_INVALIDARG;

   if (pptMousePos != NULL && prcSelection != NULL)
      return E_INVALIDARG; // either one or the other

   bool bArrayCreated = paTarget == NULL;
   if (paTarget == NULL)
   {
      paTarget = new CArray<CInteractionAreaEx *, CInteractionAreaEx *>;
      paTarget->SetSize(0, 10);
   }

   CTimePeriod periodDummy;
   if (pPeriod == NULL)
   {
      periodDummy.Init(INTERACTION_PERIOD_TIME, nPositionMs, nPositionMs);
      pPeriod = &periodDummy;
   }
   
   bool bSomethingFound = false;

   int iCounter = 0;
   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL)
   {
      iCounter++;

      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      bool bIntersects = pPeriod->GetPeriodId() == INTERACTION_PERIOD_ALL_PAGES;
      if (!bIntersects)
         bIntersects = pInteraction->VisibleIntersects(pPeriod->GetBegin(), pPeriod->GetLength());

      if (bIntersects && (pptMousePos != NULL || prcSelection != NULL))
      {
         if (pptMousePos != NULL)
            bIntersects = pInteraction->ContainsPoint(pptMousePos);
         else if (prcSelection != NULL)
            bIntersects = pInteraction->LiesInsideRect(prcSelection);
      }
      
      if (bIntersects && bOnlyVisible)
      {
         if (bForPreview && pInteraction->IsHiddenDuringPreview())
            bIntersects = false;
         
         if (!bForPreview && pInteraction->IsHiddenDuringEdit())
            bIntersects = false;
      }

      if (bIntersects && bQuestionOnly)
      {
         bIntersects = pInteraction->IsQuestionObject();
      }

      if (bIntersects)
      {
         paTarget->Add(pInteraction);
         bSomethingFound = true;
      }
   }

   // sort by stacking order: lowest order is first
   if (bSomethingFound)
      qsort(paTarget->GetData(), paTarget->GetSize(), sizeof (CInteractionAreaEx *), CompareByStacking);

   if (bSomethingFound && ppTopMost != NULL)
      *ppTopMost = paTarget->GetAt(paTarget->GetSize() - 1);

   if (bArrayCreated)
      delete paTarget;

   return bSomethingFound ? S_OK : S_FALSE;
}

CInteractionAreaEx* CInteractionStream::GetInteractionWithKey(UINT nHashKey)
{
   CInteractionAreaEx *pInteraction = NULL;
   bool bFound = m_mapInteractions.Lookup(nHashKey, pInteraction) == TRUE;

   return pInteraction;
}

HRESULT CInteractionStream::ExecuteAction(CInteractionAreaEx *pInteraction, MouseActionId idMouseAction)
{
   // TODO only execute action when active. Join with Activate()?
   CEditorDoc *pDoc = m_pEditorProject->m_pDoc;
   return pInteraction->Execute(idMouseAction, pDoc);
}


HRESULT CInteractionStream::InsertSingle(CInteractionAreaEx *pInteraction, bool bIsUserInteraction)
{
   HRESULT hr = S_OK;

   if (pInteraction == NULL)
      return E_POINTER;

   CInteractionAreaEx *pAlreadyInMap = NULL;
   if (m_mapInteractions.Lookup(pInteraction->GetHashKey(), pAlreadyInMap))
   {
      _ASSERT(false);
      return E_INVALIDARG;
   }

   m_mapInteractions.SetAt(pInteraction->GetHashKey(), pInteraction);
   if (m_pEditorProject != NULL && pInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT && pInteraction->IsDemoDocumentObject()){
       m_pEditorProject->InsertDemoDocumentStopmark(pInteraction->GetVisibilityStart() + pInteraction->GetVisibilityLength() - 1);
   }

   if (m_pUndoManager != NULL && SUCCEEDED(hr))
   {
      // pointers used: make a copy for the undo stack
      // use an array because of "ReleaseData()"
      CInteractionAreaEx **paUndo = new CInteractionAreaEx*[1];
      paUndo[0] = pInteraction;

      if (SUCCEEDED(hr))
      {
         UpdateHintId idUpdate = HINT_CHANGE_PAGE;
         if (pInteraction->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES)
            idUpdate = HINT_CHANGE_STRUCTURE;

         hr = m_pUndoManager->RegisterAction(this, UNDO_INSERT_OBJECT, 
           (BYTE *)paUndo, sizeof (CInteractionAreaEx*), idUpdate, 
           pInteraction->GetVisibilityStart(), 0, false);
      }

      // this is an action only concerning this stream
      if (SUCCEEDED(hr) && bIsUserInteraction)
         hr = m_pUndoManager->SetRollbackPoint();
      // else !bIsUserInteraction: probably an undo which handles rollback points itself
   }
   // else if bIsTextField: see DeleteSingle() for explanation
   
   return hr;
}

HRESULT CInteractionStream::DeleteSingle(UINT nHashKey, bool bIsUserInteraction) {
    HRESULT hr = S_OK;

    CInteractionAreaEx *pInteraction = NULL;
    bool bFound = m_mapInteractions.Lookup(nHashKey, pInteraction) == TRUE;

    if (bFound) {
        m_mapInteractions.RemoveKey(nHashKey);

        CEditorDoc *pDoc = m_pEditorProject->m_pDoc;
        if (pInteraction->IsSelected() && pDoc != NULL && pDoc->GetWhiteboardView() != NULL) {
            pDoc->GetWhiteboardView()->InformDeleteSelected(pInteraction);
        }

        // hide native window during deletion;
        // it really gets removed if the text field object is deleted
        if (pInteraction->GetClassId() == INTERACTION_CLASS_DYNAMIC) {
            CDynamicObject *pDynamic = (CDynamicObject *)pInteraction;
            if (pDynamic->GetType() == DYNAMIC_TEXTFIELD) {
                pDynamic->ShowHide(0, true, true);
            }
        } else if (pInteraction->GetClassId() == INTERACTION_CLASS_BLANKS) {
            ((CBlankGroup *)pInteraction)->ShowHide(0, true, true);
        } else if (pInteraction->GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT) {
            ((CGraphicalObject*)pInteraction)->SetGraphicalObjectText();
            if (m_pEditorProject != NULL && pInteraction->IsDemoDocumentObject()) {
                CMarkStream *pMarkStream = m_pEditorProject->GetMarkStream();
                if(pMarkStream)
                    pMarkStream->RemoveMark(pInteraction->GetVisibilityStart() + pInteraction->GetVisibilityLength() - 1, 1, false);
            }
        }

        // if has any stopmarks "attached" remove this reference.
        pInteraction->UnregisterMarks();

        if (m_pUndoManager != NULL) {
            // TODO this is a dirty quick fix: copying a text field and putting it
            // on the undo stack also prohibits removing the native text field from the whiteboard.

            // use an array because of "ReleaseData()"
            CInteractionAreaEx **paUndo = new CInteractionAreaEx*[1];
            paUndo[0] = pInteraction;

            if (SUCCEEDED(hr)) {
                UpdateHintId idUpdate = HINT_CHANGE_PAGE;
                if (pInteraction->GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES) {
                    idUpdate = HINT_CHANGE_STRUCTURE;
                }

                hr = m_pUndoManager->RegisterAction(this, UNDO_REMOVE_OBJECT, 
                    (BYTE *)paUndo, sizeof (CInteractionAreaEx*), idUpdate,
                    pInteraction->GetVisibilityStart(), 0, false);
            }

            // Note: this area is not deleted here (but stored on the undo/redo stack);
            // it will be deleted later in ReleaseData()

            // this is an action only concerning this stream
            if (SUCCEEDED(hr) && bIsUserInteraction) {
                hr = m_pUndoManager->SetRollbackPoint();
            }

            // else !bIsUserInteraction: probably an undo which handles rollback points itself
        } else {
            delete pInteraction;
        }
    }

    if (bFound) {
        return hr;
    } else {
        return S_FALSE;
    }
}


HRESULT CInteractionStream::ModifyArea(UINT nOldHashKey, CInteractionAreaEx *pNewInteraction, bool bIsUserInteraction)
{
   HRESULT hr = S_OK;
   
   if (pNewInteraction == NULL)
      return E_POINTER;

   CInteractionAreaEx *pOldInteraction = NULL;
   bool bFound = m_mapInteractions.Lookup(nOldHashKey, pOldInteraction) == TRUE;

   if (!bFound || pOldInteraction->GetClassId() != pNewInteraction->GetClassId())
      return E_INVALIDARG;


   int iOldStacking = pOldInteraction->GetStackingOrder();
   pNewInteraction->SetStackingOrder(iOldStacking); // new object replaces old

   if (pOldInteraction->IsSelected() != pNewInteraction->IsSelected())
      pNewInteraction->SetSelected(pOldInteraction->IsSelected());
   
   if (SUCCEEDED(hr))
      hr = DeleteSingle(nOldHashKey, false);

   if (SUCCEEDED(hr))
      pNewInteraction->SetHashKey(nOldHashKey);
   
   if (SUCCEEDED(hr))
      hr = InsertSingle(pNewInteraction, bIsUserInteraction); // normally set a rollback point
   
   return hr;
}

/*
HRESULT CInteractionStream::ChangePosition(UINT nHashKey, CRect *prcArea)
{
   HRESULT hr = S_OK;
   
   if (prcArea == NULL)
      return E_POINTER;

   CInteractionAreaEx *pInteraction = NULL;
   bool bFound = m_mapInteractions.Lookup(nHashKey, pInteraction) == TRUE;

   if (!bFound)
      return E_INVALIDARG;

   CRect rcOldPosition = pInteraction->GetArea();

   hr = pInteraction->SetArea(prcArea);
   
   if (SUCCEEDED(hr))
   {
      int aiValues[6];
      aiValues[0] = IACTION_AREA_CHANGE;
      aiValues[1] = nHashKey;
      aiValues[2] = rcOldPosition.left;
      aiValues[3] = rcOldPosition.top;
      aiValues[4] = rcOldPosition.Width();
      aiValues[5] = rcOldPosition.Height();

      if (24 != sizeof aiValues)
         _ASSERT(false);

      hr = m_pUndoManager->RegisterAction(this, UNDO_CHANGE_OBJECT, 
         (BYTE *)&aiValues, sizeof aiValues, 0, 0, true);
   }
   
   // this is an action only concerning this stream
   if (SUCCEEDED(hr))
      hr = m_pUndoManager->SetRollbackPoint();
   
   return hr;
}
*/

HRESULT CInteractionStream::MoveToFront(UINT nHashKey)
{
   return ChangeStacking(nHashKey, true);
}

HRESULT CInteractionStream::MoveToBack(UINT nHashKey)
{
   return ChangeStacking(nHashKey, false);
}

HRESULT CInteractionStream::MoveOneUp(UINT nHashKey)
{
   HRESULT hr = S_OK;
   
   CInteractionAreaEx *pInteraction = GetInteractionWithKey(nHashKey);
   
   if (pInteraction == NULL)
      return E_INVALIDARG;
   
   int iOldStacking = pInteraction->GetStackingOrder();
   
   hr = ChangeStacking(nHashKey, false, true, iOldStacking + 1, true, false);

   return hr;
}

HRESULT CInteractionStream::MoveOneDown(UINT nHashKey)
{
   HRESULT hr = S_OK;
   
   CInteractionAreaEx *pInteraction = GetInteractionWithKey(nHashKey);
   
   if (pInteraction == NULL)
      return E_INVALIDARG;
   
   int iOldStacking = pInteraction->GetStackingOrder();
   
   hr = ChangeStacking(nHashKey, false, true, iOldStacking - 1, true, true);

   return hr;
}

HRESULT CInteractionStream::CheckIntegrity(CEditorDoc *pDoc)
{
#ifdef _DEBUG
   // Check every object for validity: call a mehtod; if the
   // object is invalid (already deleted) this will probably give an exception.
   // Also checks the object lengths with respect to page lengths.

   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL)
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      InteractionClassId idClass = pInteraction->GetClassId();

      if (idClass <= 0 || idClass > 20) // arbitrary but "big" value: more than the count of defined classes
         _ASSERT(false);

      AreaPeriodId idVisible = pInteraction->GetVisibilityPeriod();

      if (idVisible == INTERACTION_PERIOD_PAGE)
      {
         UINT nPageProjectStartMs = 0;
         UINT nPageProjectEndMs = 0;
         pDoc->project.GetPagePeriod(pInteraction->GetVisibilityStart(), nPageProjectStartMs, nPageProjectEndMs);

         if (pInteraction->GetVisibilityStart() != nPageProjectStartMs 
            || pInteraction->GetVisibilityStart() + pInteraction->GetVisibilityLength() - 1 != nPageProjectEndMs)
         {
            UINT nInteractionStartMs = pInteraction->GetVisibilityStart();
            UINT nInteractionEndMs = pInteraction->GetVisibilityStart() + pInteraction->GetVisibilityLength() - 1;

            _ASSERT(false);
            pDoc->project.GetPagePeriod(pInteraction->GetVisibilityStart(), nPageProjectStartMs, nPageProjectEndMs);
         }
      }

      if (idVisible == INTERACTION_PERIOD_END_PAGE)
      {
         UINT nPageProjectStartMs = 0;
         UINT nPageProjectEndMs = 0;
         pDoc->project.GetPagePeriod(pInteraction->GetVisibilityStart(), nPageProjectStartMs, nPageProjectEndMs);

         if (pInteraction->GetVisibilityStart() != nPageProjectEndMs)
            _ASSERT(false);
      }
   }
#endif

   return S_OK;
}

HRESULT CInteractionStream::ChangeTimes(CInteractionAreaEx *pInteraction, int iChangeMs)
{
   HRESULT hr = S_OK;
   
   if (iChangeMs < 0)
      hr = pInteraction->DecrementTimes((unsigned)-iChangeMs);
   else
      hr = pInteraction->IncrementTimes((unsigned)iChangeMs);
   
   if (SUCCEEDED(hr) && m_pUndoManager != NULL)
   {
      int aiValues[3];
      aiValues[0] = IACTION_TIMES_CHANGE;
      aiValues[1] = pInteraction->GetHashKey();
      aiValues[2] = iChangeMs;
      
      hr = m_pUndoManager->RegisterAction(this, UNDO_CHANGE_OBJECT, 
         (BYTE *)&aiValues, sizeof aiValues, HINT_CHANGE_NOTHING, 0, 0, true);
   }
   
   return hr;
}
   
// private
HRESULT CInteractionStream::ChangeStacking(UINT nHashKey, bool bToFront, bool bValueValid, 
                                           int iNewStackingValue, bool bLookForOtherArea, bool bLookBelow)
{
   HRESULT hr = S_OK;

   CInteractionAreaEx *pInteraction = GetInteractionWithKey(nHashKey);

   if (pInteraction == NULL)
      return E_INVALIDARG;

   int iOldStacking = pInteraction->GetStackingOrder();


   CInteractionAreaEx *pInteractionConflict = NULL;
   int iOldConflictStacking = 0; // undo value used below

   if (bValueValid)
   {
      if (bLookForOtherArea)
      {
         pInteractionConflict = GetNextInteraction(pInteraction, bLookBelow);
         if (pInteractionConflict != NULL)
         {
            // if there is a conflicting area with the same (new) stacking value:
            // these two areas change their stacking value
            
            iOldConflictStacking = pInteractionConflict->GetStackingOrder();
            pInteractionConflict->SetStackingOrder(iOldStacking);
         }
      }

      if (iOldConflictStacking == 0) // no other area found
         hr = pInteraction->SetStackingOrder(iNewStackingValue);
      else
         hr = pInteraction->SetStackingOrder(iOldConflictStacking);
   }
   else
   {
      if (bToFront)
         hr = pInteraction->ToFront();
      else
         hr = pInteraction->ToBack();
   }

   if (SUCCEEDED(hr))
   {
      int aiValues[3];
      aiValues[0] = IACTION_STACKING_CHANGE;
      aiValues[1] = nHashKey;
      aiValues[2] = iOldStacking;

      if (12 != sizeof aiValues)
         _ASSERT(false);

      hr = m_pUndoManager->RegisterAction(this, UNDO_CHANGE_OBJECT, (BYTE *)&aiValues, sizeof aiValues, 
         HINT_CHANGE_PAGE, pInteraction->GetVisibilityStart(), 0, true);


      if (SUCCEEDED(hr) && pInteractionConflict != NULL)
      {
         // also store undo information for this change
         
         aiValues[1] = pInteractionConflict->GetHashKey();
         aiValues[2] = iOldConflictStacking;
         
         hr = m_pUndoManager->RegisterAction(this, UNDO_CHANGE_OBJECT, (BYTE *)&aiValues, sizeof aiValues, 
            HINT_CHANGE_PAGE, pInteractionConflict->GetVisibilityStart(), 0, true);
      }
   }
   
   // this is an action only concerning this stream
   if (SUCCEEDED(hr))
      hr = m_pUndoManager->SetRollbackPoint();

   return hr;
}

// private
CInteractionAreaEx* CInteractionStream::GetNextInteraction(CInteractionAreaEx *pStartInteraction, bool bLookBelow)
{
   if (pStartInteraction == NULL)
      return NULL;

   int iStartStacking = pStartInteraction->GetStackingOrder();

   CInteractionAreaEx *pBestMatch = NULL;
   int iBestMatchStacking = bLookBelow ? 0x80000000 : 0x7fffffff;

   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL)
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      int iStacking = pInteraction->GetStackingOrder();
      if (pInteraction != pStartInteraction)
      {
         if (pInteraction->Intersects(pStartInteraction))
         {
            // normal case: objects at one position/on one page

            if (bLookBelow && iStacking > iBestMatchStacking && iStacking < iStartStacking)
            {
               pBestMatch = pInteraction;
               iBestMatchStacking = iStacking;
            }
            else if (!bLookBelow && iStacking < iBestMatchStacking && iStacking > iStartStacking)
            {
               pBestMatch = pInteraction;
               iBestMatchStacking = iStacking;
            }
         }
         else
         {
            // also look for objects which are not visible/intersecting;
            // they shouldn't have the same stacking order

            if (bLookBelow && iStacking == iStartStacking - 1 || !bLookBelow && iStacking == iStartStacking + 1)
            {
               // conflicting area found
               pBestMatch = pInteraction;
               break;
            }
         }
      }
      // else do not find the area itself, or other areas which are not visible
   }

   return pBestMatch;
}

HRESULT CInteractionStream::Insert(UINT nToMs, UINT nLength, CWhiteboardStream *pWbStream)
{
   HRESULT hr = S_OK;

   CArray<CInteractionArea *, CInteractionArea *> aAllInteractions;
   aAllInteractions.SetSize(0, 120);
          
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aAllInteractionsEx;
   
   if (pWbStream != NULL && pWbStream->ContainsInteractions())
   {
      hr = pWbStream->ExtractInteractions(&aAllInteractions);
      
      if (SUCCEEDED(hr))
         aAllInteractionsEx.SetSize(0, aAllInteractions.GetSize());

      for (int i=0; i<aAllInteractions.GetSize() && SUCCEEDED(hr); ++i)
      {
         CInteractionAreaEx *pPrevious = NULL;
         if (aAllInteractionsEx.GetSize() > 0)
            pPrevious = aAllInteractionsEx[aAllInteractionsEx.GetSize() - 1];

         CInteractionAreaEx *pInteractionEx = NULL;
         hr = CreateInteractionObject(aAllInteractions[i], &pInteractionEx, m_pEditorProject, nToMs, pPrevious);

         if (SUCCEEDED(hr) && pInteractionEx != NULL)
            aAllInteractionsEx.Add(pInteractionEx);
         
         if (FAILED(hr))
            _ASSERT(false);
      }
   }

   if (SUCCEEDED(hr))
   {
      BYTE *pData = NULL;
      if (aAllInteractionsEx.GetSize() > 0)
         pData = (BYTE *)aAllInteractionsEx.GetData();

      hr = InsertRange(nToMs, nLength, pData, 
         aAllInteractionsEx.GetSize() * sizeof (CInteractionAreaEx*), true);
   }

   if (SUCCEEDED(hr))
      m_nStreamLength += nLength;
   
   // InsertRange() makes a copy:
   for (int i=0; i<aAllInteractionsEx.GetSize(); ++i)
   {
      if (aAllInteractionsEx[i] != NULL)
         delete aAllInteractionsEx[i];
   }
   
   for (i=0; i<aAllInteractions.GetSize(); ++i)
      delete aAllInteractions[i];

   return hr;
}

HRESULT CInteractionStream::InsertDenver(UINT nToMs, UINT nLengthMs)
{
   HRESULT hr = S_OK;

   // slightly double code'ish to InsertRange()
   
   // go through the areas in the right order;
   // and the map iteration is not constant to changes (below):
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aExistingElements;
   aExistingElements.SetSize(m_mapInteractions.GetCount(), 0);

   int iInsertPos = 0;
   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL && SUCCEEDED(hr))
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      aExistingElements[iInsertPos++] = pInteraction;
   }

   qsort(aExistingElements.GetData(), aExistingElements.GetSize(), sizeof (CInteractionAreaEx *), CompareByStacking);


   for (int i=0; i<aExistingElements.GetSize() && SUCCEEDED(hr); ++i)
   {
      CInteractionAreaEx *pInteraction = aExistingElements[i];

      if (pInteraction->IsSplitPosition(nToMs))
      {
         CInteractionAreaEx *pDummyArea = pInteraction->Copy(true); // small hack: have something for lengthening
         CInteractionAreaEx *pCopyArea = pInteraction->Copy(true);
         if (pCopyArea != NULL && pDummyArea != NULL)
         {
            hr = pDummyArea->DecrementTimes(pDummyArea->GetVisibilityStart());
            if (SUCCEEDED(hr))
               hr = pDummyArea->TrimTo(0, nLengthMs);
            if (SUCCEEDED(hr))
               hr = pCopyArea->AddArea(pDummyArea, nToMs, false);

            if (SUCCEEDED(hr))
               hr = ModifyArea(pInteraction->GetHashKey(), pCopyArea, false);

            delete pDummyArea;
         }
         else
            hr = E_FAIL;
      }
      else if (pInteraction->GetVisibilityStart() >= nToMs)
      {
         hr = ChangeTimes(pInteraction, nLengthMs);
      }
   }

   if (SUCCEEDED(hr))
      m_nStreamLength += nLengthMs;
   
   if (m_pUndoManager != NULL && SUCCEEDED(hr))
   {
      hr = m_pUndoManager->RegisterAction(
         this, UNDO_INSERT_RANGE, NULL, 0, HINT_CHANGE_STRUCTURE, nToMs, nLengthMs, false);
      // dummy entry to shorten the stream upon undo/redo
   }

   return hr;    
}
      

HRESULT CInteractionStream::ResolveTimes(UINT nMoveMs)
{
   HRESULT hr = S_OK;

   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL && SUCCEEDED(hr))
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);
   
      hr = pInteraction->ResolveTimes(m_pEditorProject, nMoveMs);
   }

   return hr;
}

bool CInteractionStream::ActivateObjects(UINT nPositionMs, bool bPreviewing)
{
   bool bChange = false;
   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL)
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      bool bSingleChange = pInteraction->Activate(nPositionMs, bPreviewing);
      if (bSingleChange)
         bChange = true;
   }

   return bChange;
}

HRESULT CInteractionStream::WriteLepEntry(CFileOutput *pOutput)
{
   HRESULT hr = S_OK;

   if (pOutput == NULL)
      return E_POINTER;

   _TCHAR tszOutput[100];
   _stprintf(tszOutput, _T("    <interactions length=\"%d\">\n"), this->m_nStreamLength);
   hr = pOutput->Append(tszOutput);
   
   if (SUCCEEDED(hr))
      hr = WriteObjEntry(pOutput, true, true, NULL);

   _stprintf(tszOutput, _T("    </interactions>\n"));
   if (SUCCEEDED(hr))
      hr = pOutput->Append(tszOutput);

   return hr;
}

HRESULT CInteractionStream::WriteObjEntry(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, LPCTSTR tszExportPrefix)
{
   HRESULT hr = S_OK;

   if (pOutput == NULL)
      return E_POINTER;
  
   // generate a plain list sorted by stacking order

   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aInteractions;
      
   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL)
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      aInteractions.Add(pInteraction);
   }

   qsort(aInteractions.GetData(), aInteractions.GetSize(), sizeof (CInteractionArea *), CompareByStacking);


   CMap<CString, LPCTSTR, CString, CString&> mapImagePaths;

   // write them in that order to the file

   for (int i=0; i<aInteractions.GetSize() && SUCCEEDED(hr); ++i)
   {
      if (aInteractions[i]->GetClassId() != INTERACTION_CLASS_FEEDBACK)
         hr = aInteractions[i]->Write(pOutput, bDoIdent, bUseFullImagePath, mapImagePaths, tszExportPrefix, m_pEditorProject);
      // else written by CQuestion(naire)Ex
   }

   return hr;
}

HRESULT CInteractionStream::ReadLepEntry(SgmlElement *pSgml, LPCTSTR tszLepFileName)
{
   HRESULT hr = S_OK;
  
   if (pSgml == NULL)
      return E_POINTER;

   int iStreamLength = pSgml->GetIntValue(_T("length"));
   if (iStreamLength > 0)
      m_nStreamLength = iStreamLength;

   CWhiteboardStream wbStream;

   CString csRecordPath = tszLepFileName;
   StringManipulation::GetPath(csRecordPath);
   hr = wbStream.ParseObjectSgml(pSgml, csRecordPath);

   if (SUCCEEDED(hr))
      hr = Insert(0, m_nStreamLength, &wbStream);

   return hr;
}



HRESULT CInteractionStream::SpecificUndo(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs, UINT nLengthMs)
{
   HRESULT hr = S_OK;

   if (idActionCode == UNDO_INSERT_OBJECT)
   {
      CInteractionAreaEx **paInteractions = (CInteractionAreaEx **)pData;
      hr = DeleteSingle(paInteractions[0]->GetHashKey(), false);
   }
   else if (idActionCode == UNDO_REMOVE_OBJECT)
   {
      CInteractionAreaEx **paInteractions = (CInteractionAreaEx **)pData;
      hr = InsertSingle(paInteractions[0], false);
   }
   else if (idActionCode == UNDO_CHANGE_OBJECT)
   {
      if (nDataLength < 12) // at least id, key, value
         return E_IAX_INVALID_DATA;

      int *pValues = (int *)pData;
      
      UINT nHashKey = pValues[1];
      
      CInteractionAreaEx *pInteraction = NULL;
      bool bFound = m_mapInteractions.Lookup(nHashKey, pInteraction) == TRUE;
      
      if (!bFound)
         return E_IAX_INVALID_DATA;
     
      /*
      if (pValues[0] == IACTION_AREA_CHANGE)
      {
         if (nDataLength < 24)
            return E_IAX_INVALID_DATA;

         int x = pValues[2];
         int y = pValues[3];
         int w = pValues[4];
         int h = pValues[5];

         CRect rcNewPosition(CPoint(x, y), CSize(w, h));

         hr = ChangePosition(nHashKey, &rcNewPosition); // takes care of undo/redo
      }
      else 
      */
      if (pValues[0] == IACTION_STACKING_CHANGE)
      {
         int iOldStacking = pValues[2];

         hr = ChangeStacking(nHashKey, false, true, iOldStacking, false); // takes care of undo/redo
      }
      else if (pValues[0] == IACTION_TIMES_CHANGE)
      {
         // Note: stream operations can and will affect many areas after
         // the insert/delete position.
         // And all of these areas only change few informations.
         // Furthermore during undo/redo of stream operations you cannot
         // know which areas are old or new and which should be changed and
         // which not.

         int iTimesChange = pValues[2];

         hr = ChangeTimes(pInteraction, -iTimesChange);
      }
      else
         return E_IAX_INVALID_DATA;
   }
   else if (idActionCode == UNDO_INSERT_RANGE)
   {
      m_nStreamLength -= nLengthMs;

      if (m_pUndoManager != NULL && SUCCEEDED(hr))
      {
         hr = m_pUndoManager->RegisterAction(
            this, UNDO_REMOVE_RANGE, NULL, 0, HINT_CHANGE_STRUCTURE, nPositionMs, nLengthMs, false);
         // dummy entry to shorten the stream upon undo/redo
      }
   }
   else if (idActionCode == UNDO_REMOVE_RANGE)
   {
      m_nStreamLength += nLengthMs;

      if (m_pUndoManager != NULL && SUCCEEDED(hr))
      {
         hr = m_pUndoManager->RegisterAction(
            this, UNDO_INSERT_RANGE, NULL, 0, HINT_CHANGE_STRUCTURE, nPositionMs, nLengthMs, false);
         // dummy entry to shorten the stream upon undo/redo
      }
   }
   else
      hr = S_FALSE;

      
   // TODO solve this centrally (better): update what and when
   //m_pEditorDoc->UpdateAllViews(NULL, HINT_CHANGE_PAGE);
   

   return hr;
}


HRESULT CInteractionStream::ResetAfterPreview()
{
   bool bChange = false;

   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL)
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      bool bSingleChange = pInteraction->Reset(true);
      if (bSingleChange)
         bChange = true;

      // text field will be disabled and should be repainted anyway
      // TODO this is a nasty side effect: using repaint to actually disable the field
      if (pInteraction->GetClassId() == INTERACTION_CLASS_DYNAMIC)
      {
         CDynamicObject *pDynamic = (CDynamicObject *)pInteraction;
         if (pDynamic->GetType() == DYNAMIC_TEXTFIELD)
            bChange = true;
      }
      else if (pInteraction->GetClassId() == INTERACTION_CLASS_BLANKS)
         bChange = true;
   }

   return bChange ? S_OK : S_FALSE;
}

HRESULT CInteractionStream::ResetOtherRadioButtons(CDynamicObject *pSource)
{
   HRESULT hr = S_OK;

   CEditorDoc *pDoc = m_pEditorProject->m_pDoc;

   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aInteractions;
   aInteractions.SetSize(0, 20);
   hr = FindInteractions(pDoc->m_curPosMs, true, true, &aInteractions);
   
   if (SUCCEEDED(hr))
      hr = S_FALSE;

   for (int i=0; i<aInteractions.GetSize(); ++i)
   {
      if (aInteractions[i]->GetClassId() == INTERACTION_CLASS_DYNAMIC)
      {
         CDynamicObject *pDynamic2 = (CDynamicObject *)aInteractions[i];
         if (pDynamic2 != pSource)
         {
            if (pDynamic2->GetType() == DYNAMIC_RADIOBUTTON)
            {
               pDynamic2->ResetRadio();
               hr = S_OK;
            }
         }
      }
   }
   
   return hr;
}


HRESULT CInteractionStream::UndoAction(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs, UINT nLengthMs)
{
   // handle all undo (especially INSERT and REMOVE_RANGE)
   return SpecificUndo(idActionCode, pData, nDataLength, nPositionMs, nLengthMs);
}


HRESULT CInteractionStream::ReleaseData(BYTE *pData, UINT nByteCount, bool bDeleteCompletely,
                                        UndoActionId idActionCode, UINT nPositionMs, UINT nLengthMs)
{
   if (pData == NULL && nByteCount > 0)
      return E_POINTER;

   if (nByteCount % sizeof (CInteractionAreaEx*) != 0)
      return E_INVALIDARG;

   UINT nElementCount = nByteCount / sizeof (CInteractionAreaEx*);
   CInteractionAreaEx **paInteractions = (CInteractionAreaEx **)pData;

   // this should be always an array; even for only one element (InsertSingle(), ...)
   DeleteArray(paInteractions, nElementCount, bDeleteCompletely);

   return S_OK;
}


HRESULT CInteractionStream::CutCopyOrDelete(UINT nFromMs, UINT nLengthMs, CClipboard *pClipboard, bool bDoCopy, bool bDoDelete)
{
   HRESULT hr = S_OK;

   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aAffectedInteractions;
   aAffectedInteractions.SetSize(0, 10);

   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL && SUCCEEDED(hr))
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      if (pInteraction->Intersects(nFromMs, nLengthMs, true) 
         && pInteraction->GetVisibilityPeriod() != INTERACTION_PERIOD_ALL_PAGES)
      {
         aAffectedInteractions.Add(pInteraction);
      }
      else if (bDoDelete)
      {
         if (pInteraction->GetVisibilityStart() >= nFromMs + nLengthMs)
         {
            hr = ChangeTimes(pInteraction, -(int)nLengthMs);
         }
      }
   }


   if (bDoCopy && SUCCEEDED(hr))
   {
      if (pClipboard == NULL)
         return E_POINTER;

      CArray<CInteractionAreaEx *, CInteractionAreaEx *> aCopyInteractions;
      aCopyInteractions.SetSize(0, 10);

      for (int i=0; i<aAffectedInteractions.GetSize() && SUCCEEDED(hr); ++i)
      {
         CInteractionAreaEx *pAffected = aAffectedInteractions[i];

         bool bActive = pAffected->ActiveIntersects(nFromMs, nLengthMs);
         bool bFeedback = pAffected->GetClassId() == INTERACTION_CLASS_FEEDBACK;

         CInteractionAreaEx *pCopyArea = pAffected->Copy(true);
         if (pCopyArea != NULL)
         {
            if (SUCCEEDED(hr) && pCopyArea->IsQuestionObject())
               pCopyArea->FindQuestionId(m_pEditorProject);
            if (SUCCEEDED(hr))
               hr = pCopyArea->TrimTo(nFromMs, nLengthMs);
            if (SUCCEEDED(hr))
               hr = pCopyArea->DecrementTimes(nFromMs);
            if (SUCCEEDED(hr))
               aCopyInteractions.Add(pCopyArea);
            else
               delete pCopyArea;
         }
         else
            hr = E_FAIL;
         
      }

      if (SUCCEEDED(hr))
      {
         if (m_ppLastClipboardData != NULL)
         {
            for (int i=0; i<(signed)m_nLastClipboardLength; ++i)
               delete m_ppLastClipboardData[i];
            delete[] m_ppLastClipboardData;
         }
         
         UINT nElementCount = (unsigned)aCopyInteractions.GetSize();
         UINT nByteCount = nElementCount * sizeof (CInteractionAreaEx*);
         m_ppLastClipboardData = new CInteractionAreaEx*[nElementCount];
         m_nLastClipboardLength = nElementCount;
         
         for (int j=0; j<(signed)nElementCount; ++j)
            m_ppLastClipboardData[j] = aCopyInteractions[j];

         qsort(m_ppLastClipboardData, nElementCount, sizeof (CInteractionAreaEx *), CompareByStacking);
         
         UINT nClipboardKey = (UINT)this;
         hr = pClipboard->AddData((BYTE *)m_ppLastClipboardData, nByteCount, nClipboardKey);
         
         // Note: a copy of that data is made and released in CClipboard.
         // But in order to properly delete all data the destructor of all CInteractionAreaEx
         // objects must be called, thus the doubled m_ppLastClipboardData array.
      }
   }

   if (bDoDelete && SUCCEEDED(hr))
   {
      for (int i=0; i<aAffectedInteractions.GetSize() && SUCCEEDED(hr); ++i)
      {
         CInteractionAreaEx *pAffected = aAffectedInteractions[i];

         if (pAffected->EntirelyWithin(nFromMs, nLengthMs))
            hr = DeleteSingle(pAffected->GetHashKey(), false);
         else
         {
            CInteractionAreaEx *pPartArea = pAffected->Copy(true);

            hr = pPartArea->RemovePart(nFromMs, nLengthMs);
            if (SUCCEEDED(hr))
               hr = ModifyArea(pAffected->GetHashKey(), pPartArea, false);
         }
      }
   }

   if (m_pUndoManager && bDoDelete && SUCCEEDED(hr))
   {
      hr = m_pUndoManager->RegisterAction(
         this, UNDO_REMOVE_RANGE, NULL, 0, HINT_CHANGE_STRUCTURE, nFromMs, nLengthMs, false);
      // dummy entry to enlarge the stream upon undo/redo
   }

   //PrintState();

   return hr;
}

HRESULT CInteractionStream::InsertRange(UINT nToMs, UINT nLengthMs, BYTE *pData, UINT nByteCount, bool bUndoWanted)
{
   // ignore bUndoWanted: we need proper undo informations for memory management
   
   HRESULT hr = S_OK;

   if (pData == NULL && nByteCount > 0)
      return E_POINTER;

   if (nByteCount % sizeof (CInteractionAreaEx*) != 0)
      return E_INVALIDARG;

   UINT nInsertCount = nByteCount / sizeof (CInteractionAreaEx*);
   CInteractionAreaEx **paInteractions = (CInteractionAreaEx **)pData;

   CMap<UINT, UINT, CInteractionAreaEx *, CInteractionAreaEx *> mapInsertElements;
   if (nInsertCount > 0)
      mapInsertElements.InitHashTable(nInsertCount * 2 - 1);

   bool bIgnoreQuestionObjects = false;

   for (int i=0; i<(signed)nInsertCount && SUCCEEDED(hr); ++i)
   {
      /* solution not sufficient: 
         you cannot just lengthen a few areas on arbitrary (more then one question?) inserts
      if (paInteractions[i]->IsQuestionObject() && !bIgnoreQuestionObjects)
      {
         CInteractionAreaEx *pMatching = NULL;
         bool bMatchesExisting = m_mapInteractions.Lookup(paInteractions[i]->GetHashKey(), pMatching) == TRUE;
         if (bMatchesExisting && pMatching->MeetsWith(paInteractions[i], nToMs, false))
         {
            // it is a question object being inserted into another question object:
            // the CQuestionStream and CQuestionEx already took care of lengthening all the objects;
            // ignore them here

            bIgnoreQuestionObjects = true;
         }
      }
      */

      mapInsertElements.SetAt(paInteractions[i]->GetHashKey(), paInteractions[i]);
   }

   // the map iteration is not constant to changes (below):
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aExistingElements;
   aExistingElements.SetSize(m_mapInteractions.GetCount(), 0);

   int iInsertPos = 0;
   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL && SUCCEEDED(hr))
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      aExistingElements[iInsertPos++] = pInteraction;
   }

   // go through the areas in the right order
   qsort(aExistingElements.GetData(), aExistingElements.GetSize(), sizeof (CInteractionAreaEx *), CompareByStacking);

   for (i=0; i<aExistingElements.GetSize(); ++i)
   {
      CInteractionAreaEx *pInteraction = aExistingElements[i];

      if (pInteraction->IsSplitPosition(nToMs))
      {
         CInteractionAreaEx *pMatchingInsert = NULL;
         bool bMatchesInInsert = mapInsertElements.Lookup(pInteraction->GetHashKey(), pMatchingInsert) == TRUE;
         bool bProducesGap = false;

         if (bMatchesInInsert)
         {
            if (pMatchingInsert->IsQuestionObject() && bIgnoreQuestionObjects)
               bProducesGap = false; // do nothing; ignore
            else if (pMatchingInsert->GetVisibilityStart() > 0)
               bProducesGap = true;
            else if (pMatchingInsert->GetVisibilityLength() < nLengthMs)
               bProducesGap = true;
         }

         if (!bMatchesInInsert || bProducesGap)
         {
            // not present among insertion elements or producing gap: split existing and move later half

            CInteractionAreaEx *pCopyArea = pInteraction->Copy(true);
            CInteractionAreaEx *pSecondHalf = NULL;

            if (pCopyArea != NULL)
            {
               pSecondHalf = pCopyArea->Split(nToMs);
               if (pSecondHalf != NULL)
               {
                  hr = ModifyArea(pInteraction->GetHashKey(), pCopyArea, false);
                  if (SUCCEEDED(hr))
                     hr = pSecondHalf->IncrementTimes(nLengthMs);

                  UINT nVisibilityStart = pSecondHalf->GetVisibilityStart();

                  if (SUCCEEDED(hr))
                     hr = InsertSingle(pSecondHalf, false);
               }
               else
                  hr = E_FAIL;
            }
            else 
               hr = E_FAIL;

            // already join here: new element gets different hash key;
            // for future inserts this is ok as also newly inserted pages get a new id

            if (SUCCEEDED(hr))
            {
               if (pCopyArea->MeetsWith(pMatchingInsert, nToMs))
               {
                  hr = JoinAreas(pCopyArea, pMatchingInsert, nToMs);

                  mapInsertElements.RemoveKey(pMatchingInsert->GetHashKey());
               }
               else if (pSecondHalf->MeetsWith(pMatchingInsert, nToMs, true))
               {
                  hr = JoinAreas(pSecondHalf, pMatchingInsert, nToMs);

                  mapInsertElements.RemoveKey(pMatchingInsert->GetHashKey());
               }
               
            }
         }
      }
      else if (pInteraction->GetVisibilityStart() >= nToMs)
      {
         hr = ChangeTimes(pInteraction, nLengthMs);
      }
   }
 
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> aNewElements;
   aNewElements.SetSize(0, mapInsertElements.GetCount());

   pos = mapInsertElements.GetStartPosition();
   while (pos != NULL && SUCCEEDED(hr))
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInsertInteraction = NULL;
      
      mapInsertElements.GetNextAssoc(pos, nKey, pInsertInteraction);

      if (!(pInsertInteraction->IsQuestionObject() && bIgnoreQuestionObjects))
      {
         // normal case
         
         CInteractionAreaEx *pExisting = NULL;
         bool bExists = m_mapInteractions.Lookup(pInsertInteraction->GetHashKey(), pExisting) == TRUE;
         
         if (bExists && pExisting->MeetsWith(pInsertInteraction, nToMs))
         {
            hr = JoinAreas(pExisting, pInsertInteraction, nToMs);
         }
         else
         {
            // insert as new
            aNewElements.Add(pInsertInteraction);
         }
      }
      else
      {
         // is an ignored question object (see above)
         // do nothing
      }
      
   }

   // maintain the right order for the new elements
   qsort(aNewElements.GetData(), aNewElements.GetSize(), sizeof (CInteractionAreaEx *), CompareByStacking);


   for (i=0; i<aNewElements.GetSize() && SUCCEEDED(hr); ++i)
   {
      CInteractionAreaEx *pInsertInteraction = aNewElements[i];

      CInteractionAreaEx *pExisting = NULL;
      bool bExists = m_mapInteractions.Lookup(pInsertInteraction->GetHashKey(), pExisting) == TRUE;

      bool bCopyHashKey = !bExists;
      CInteractionAreaEx *pNewInteraction = pInsertInteraction->Copy(bCopyHashKey);
      
      if (pNewInteraction != NULL)
      {   
         if (nToMs > 0)
            hr = pNewInteraction->IncrementTimes(nToMs);
         
         if (SUCCEEDED(hr))
            hr = InsertSingle(pNewInteraction, false);
      }
      else
         hr = E_FAIL;
   }

   //PrintState();

   if (m_pUndoManager != NULL && SUCCEEDED(hr))
   {
      hr = m_pUndoManager->RegisterAction(
         this, UNDO_INSERT_RANGE, NULL, 0, HINT_CHANGE_STRUCTURE, nToMs, nLengthMs, false);
      // dummy entry to shorten the stream upon undo/redo
      
   }

   return hr;
}

// private
HRESULT CInteractionStream::JoinAreas(CInteractionAreaEx *pExisting, CInteractionAreaEx *pEnlarge, UINT nMoveMs)
{
   HRESULT hr = S_OK;

   if (pExisting == NULL || pEnlarge == NULL)
      return E_POINTER;

   CInteractionAreaEx *pCopyArea = pExisting->Copy(true);
   if (pCopyArea != NULL)
   {
      hr = pCopyArea->AddArea(pEnlarge, nMoveMs, true);
      if (SUCCEEDED(hr))
         hr = ModifyArea(pExisting->GetHashKey(), pCopyArea, false);
   }
   else
      hr = E_FAIL;
   
   return hr;
}

// private
HRESULT CInteractionStream::DeleteArray(CInteractionAreaEx **ppInteractions, UINT nElementCount, bool bDeleteCompletely)
{
   if (ppInteractions != NULL)
   {
      if (bDeleteCompletely)
      {
         for (int i=0; i<(signed)nElementCount; ++i)
         {
            if (ppInteractions[i] != NULL)
            {
               CInteractionAreaEx *pInteractionInMap = NULL;
      
               bool bStillInUse = m_mapInteractions.Lookup(ppInteractions[i]->GetHashKey(), pInteractionInMap) == TRUE;

               if (bStillInUse)
                  bStillInUse = ppInteractions[i] == pInteractionInMap; // precise match

               if (!bStillInUse)
                  delete ppInteractions[i];
               // else will be delete later by ~CInteractionStream()
            }
         }
      }

      delete[] ppInteractions;
   }

   return S_OK;
}

// private
void CInteractionStream::PrintState()
{
   TRACE(_T("### %d\n"), m_mapInteractions.GetCount());

   POSITION pos = m_mapInteractions.GetStartPosition();
   while (pos != NULL)
   {
      UINT nKey = 0;
      CInteractionAreaEx *pInteraction = NULL;
      
      m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);

      UINT nVisibilityStart = pInteraction->GetVisibilityStart();
      UINT nVisibilityEnd = pInteraction->GetVisibilityStart() + pInteraction->GetVisibilityLength() - 1;

      TRACE(_T("%08x: %d - %d\n"), (int)pInteraction, nVisibilityStart, nVisibilityEnd);
   }
}

void CInteractionStream::DeleteAllDemoDocumentObjects(){
    // get demo document objects number
    POSITION pos = m_mapInteractions.GetStartPosition();
    int iLastDemoDocObject = 0;
    while (pos != NULL) {
        UINT nKey = 0;
        CInteractionAreaEx *pInteraction = NULL;
        m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);
        if (pInteraction->IsDemoDocumentObject()){
            iLastDemoDocObject++;
        }
    }

    int i = 0;
    pos = m_mapInteractions.GetStartPosition();
    while (pos != NULL) {
        UINT nKey = 0;
        CInteractionAreaEx *pInteraction = NULL;
        m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);
        if (pInteraction->IsDemoDocumentObject()){
            i++;
            DeleteSingle(nKey, i == iLastDemoDocObject /*Insert rollback point for the last demo document object deleted*/);
        }
    }
}

bool CInteractionStream::HasDemoDocumentObjects(){
    POSITION pos = m_mapInteractions.GetStartPosition();
    while (pos != NULL) {
        UINT nKey = 0;
        CInteractionAreaEx *pInteraction = NULL;
        m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);
        if (pInteraction->IsDemoDocumentObject()){
            return true;
        }
    }
    return false;
}

bool CInteractionStream::InteractionsAreOnlyClickInfos() {
    POSITION pos = m_mapInteractions.GetStartPosition();
    while (pos != NULL) {
        UINT nKey = 0;
        CInteractionAreaEx *pInteraction = NULL;
        m_mapInteractions.GetNextAssoc(pos, nKey, pInteraction);
        if (pInteraction != NULL && pInteraction->GetClassId() != INTERACTION_CLASS_CLICK_OBJECT){
            return false;
        }
    }
    return true;
}
