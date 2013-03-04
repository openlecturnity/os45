#include "stdafx.h"
#include "UndoRedoManager.h"
#include "editorDoc.h" // UpdateHintId

CUndoRedoManager::CUndoRedoManager()
{
   m_lstUndoStack.RemoveAll();
   m_lstRedoStack.RemoveAll();
   m_bUndoActive = false;
}

CUndoRedoManager::~CUndoRedoManager()
{
   // must clear objects pointed to in the lists
   Clear();
}

HRESULT CUndoRedoManager::Clear()
{
   ClearActionData(&m_lstUndoStack, true);
   ClearActionData(&m_lstRedoStack, false);

   return S_OK;
}

bool CUndoRedoManager::HasUndoElements()
{
   return !m_lstUndoStack.IsEmpty() && !m_lstUndoStack.GetHead()->IsBlocker();
}

bool CUndoRedoManager::HasRedoElements()
{
   return !m_lstRedoStack.IsEmpty();
}

HRESULT CUndoRedoManager::RegisterAction(IUndoCapable *pSource, 
                                         UndoActionId idActionCode, 
                                         BYTE *pData, UINT nDataLength,
                                         UpdateHintId idUpdateType,
                                         UINT nPositionMs, UINT nLengthMs,
                                         bool bCopyData)
{
   if (pSource == NULL)
      return E_POINTER;
   
   if (nDataLength > 0 && pData == NULL)
      return E_INVALIDARG;

   // if it is a normal action empty the redo stack
   if (!m_bUndoActive && !m_bRedoActive && m_lstRedoStack.GetCount() > 0)
      ClearActionData(&m_lstRedoStack, false);
   
   CUndoActionDefinition *pAction = new CUndoActionDefinition();
   pAction->Init(pSource, idActionCode, pData, nDataLength,
      idUpdateType, nPositionMs, nLengthMs, bCopyData);

   if (!m_bUndoActive)
      m_lstUndoStack.AddHead(pAction);
   else
      m_lstRedoStack.AddHead(pAction);

   return S_OK;
}

   
HRESULT CUndoRedoManager::SetRollbackPoint(bool bIsBlocker)
{
   CList<CUndoActionDefinition*, CUndoActionDefinition*> *plstStack = NULL;
   
   if (!m_bUndoActive)
      plstStack = &m_lstUndoStack; // this is where the last action was added to
   else
      plstStack = &m_lstRedoStack; // or here

   if (plstStack->IsEmpty() || plstStack->GetHead()->IsSeparator())
         return S_FALSE; // do not add a second separator
   

   // find out the maximum change id and store it in the separator
   UpdateHintId idMaximumUpdate = HINT_CHANGE_NOTHING;

   POSITION pos = plstStack->GetHeadPosition();
   while (pos != NULL)
   {
      CUndoActionDefinition *pAction = plstStack->GetNext(pos);

      if (pAction->IsSeparator())
         break;

      if (pAction->GetUpdateId() > idMaximumUpdate)
         idMaximumUpdate = pAction->GetUpdateId();
   }


   CUndoActionDefinition *pRollbackAction = new CUndoActionDefinition();
   pRollbackAction->Init(NULL, bIsBlocker ? UNDO_BLOCKER : UNDO_SEPARATOR, NULL, 0, idMaximumUpdate);

   plstStack->AddHead(pRollbackAction);

   m_bUndoActive = false;
   m_bRedoActive = false;


   return S_OK;
}

HRESULT CUndoRedoManager::PerformUndo(UpdateHintId *pidMaximumUpdate, CArray<UINT, UINT> *paBeginTimes)
{
   m_bUndoActive = true;

   HRESULT hr = RollbackOneAction(&m_lstUndoStack, pidMaximumUpdate, paBeginTimes);

   // m_bUndoActive will not be reset here but in SetRollbackPoint()!
   SetRollbackPoint();

   return hr;
}

HRESULT CUndoRedoManager::PerformRedo(UpdateHintId *pidMaximumUpdate, CArray<UINT, UINT> *paBeginTimes)
{
   m_bRedoActive = true;

   HRESULT hr = RollbackOneAction(&m_lstRedoStack, pidMaximumUpdate, paBeginTimes);
   
   // m_bRedoActive will not be reset here but in SetRollbackPoint()!
   SetRollbackPoint();

   return hr;
}


// private
HRESULT CUndoRedoManager::RollbackOneAction(CList<CUndoActionDefinition*, CUndoActionDefinition*> *pList,
                                            UpdateHintId *pidMaximumUpdate, CArray<UINT, UINT> *paBeginTimes)
{
   HRESULT hr = S_OK;

   if (pList->IsEmpty())
      return S_FALSE;

   CUndoActionDefinition *pAction = pList->RemoveHead();

   if (!pAction->IsSeparator() || pAction->IsBlocker())
   {
      _ASSERT(false);
      hr = E_URM_INVALID_STATE; // before rollback a rollback point must be defined
   }

   if (pidMaximumUpdate != NULL)
      *pidMaximumUpdate = pAction->GetUpdateId();

   delete pAction;

   CMap<UINT, UINT, bool, bool> mapUsedTimes;
   mapUsedTimes.InitHashTable(13);

   while (!pList->IsEmpty() && SUCCEEDED(hr)) // rollback until empty or separator reached
   {
      pAction = pList->RemoveHead();
      if (!pAction->IsSeparator())
      {
         hr = pAction->InvokeUndo();

         if (paBeginTimes != NULL)
         {
            if (pAction->GetUpdateId() == HINT_CHANGE_PAGE)
               mapUsedTimes.SetAt(pAction->GetPosition(), true);
            // TODO what about not-PAGE? eg OBJECT?
         }

         delete pAction;
      }
      else
      {
         pList->AddHead(pAction);
         break;
      }
   }

   // transfer all the begin times (probably only one) to the array
   if (paBeginTimes != NULL && SUCCEEDED(hr))
   {
      POSITION pos = mapUsedTimes.GetStartPosition();
      while (pos != NULL)
      {
         UINT nPosition = 0;
         bool bUsed = false;
         mapUsedTimes.GetNextAssoc(pos, nPosition, bUsed);

         paBeginTimes->Add(nPosition);
      }
   }

   return hr;
}

// private
void CUndoRedoManager::ClearActionData(CList<CUndoActionDefinition*, CUndoActionDefinition*> *pActionList, bool bIsUndo)
{
   if (pActionList != NULL)
   {
      POSITION iter = pActionList->GetHeadPosition();
      while (iter != NULL)
      {
         CUndoActionDefinition *pAction = pActionList->GetNext(iter);
         pAction->ClearNotCopiedData(bIsUndo);
         delete pAction;
      }
   }
   pActionList->RemoveAll();
}



//////////////////////////////////////////////////////////////////////
// ActionDefinition Klasse
//////////////////////////////////////////////////////////////////////

CUndoActionDefinition::CUndoActionDefinition()
{
   m_idActionCode = UNDO_NO_ACTION;
   m_abActionData.RemoveAll();
   m_pSource = NULL;
   m_idUpdateType = HINT_CHANGE_NOTHING;
   m_nPositionMs = 0;
   m_nLengthMs = 0;
   m_bDataIsCopy = true;
   m_pbNotCopiedPointer = NULL;
}

CUndoActionDefinition::~CUndoActionDefinition()
{
   // Note: not copied data is only released partially here 
   // (only for a possible enclosing array).
   
   if (!m_bDataIsCopy && m_abActionData.GetSize() > 0)
   {
      m_pSource->ReleaseData(
         m_pbNotCopiedPointer, m_abActionData.GetSize(), false,
         m_idActionCode, m_nPositionMs, m_nLengthMs);
   }

   m_abActionData.RemoveAll();
}

HRESULT CUndoActionDefinition::Init(IUndoCapable *pSource, UndoActionId idActionCode, 
   BYTE *pData, UINT nDataLength, UpdateHintId idUpdateType, UINT nPositionMs, UINT nLengthMs, bool bCopyData)
{
   if ((idActionCode != UNDO_SEPARATOR && idActionCode != UNDO_BLOCKER) && pSource == NULL)
      return E_POINTER;

   if (nDataLength > 0 && pData == NULL)
      return E_POINTER;

   m_pSource = pSource;
   m_idActionCode = idActionCode;
   m_idUpdateType = idUpdateType;
   m_nPositionMs = nPositionMs;
   m_nLengthMs = nLengthMs;
   m_bDataIsCopy = bCopyData;

   if (nDataLength > 0)
   {
      m_abActionData.SetSize(nDataLength, 0);
      memcpy(m_abActionData.GetData(), pData, nDataLength);

      // Note: Regardless of the value of bCopyData a copy is made (and released).
      // However during release not-copied data will be released by the origin object
      // with the method "ReleaseData()".

      if (!bCopyData)
         m_pbNotCopiedPointer = pData;
   }

   return S_OK;
}

BYTE* CUndoActionDefinition::GetDataAccess()
{
   return m_abActionData.GetData();
}

HRESULT CUndoActionDefinition::InvokeUndo()
{
   if (m_idActionCode != UNDO_NO_ACTION && m_pSource == NULL)
      return E_AD_UNINITIALIZED;

   return m_pSource->UndoAction(m_idActionCode, 
      m_abActionData.GetData(), m_abActionData.GetSize(), m_nPositionMs, m_nLengthMs);
}

HRESULT CUndoActionDefinition::ClearNotCopiedData(bool bIsUndo)
{
   if (!m_bDataIsCopy && m_pbNotCopiedPointer != NULL)
   {
      bool bRightAction = bIsUndo && m_idActionCode == UNDO_INSERT_OBJECT ||
         !bIsUndo && m_idActionCode == UNDO_REMOVE_OBJECT;

      if (bRightAction)
      {
         m_pSource->ReleaseData(
            m_pbNotCopiedPointer, m_abActionData.GetSize(), true,
            m_idActionCode, m_nPositionMs, m_nLengthMs);
         m_abActionData.RemoveAll(); // prevent deleting twice
         return S_OK;
      }
   }
   
   return S_FALSE;
   
   // Note: the problem here is that m_pSource must be still valid:
   // So upon destruction the CUndoRedoManager and thus the CUndoActionDefinition
   // objects must be deleted first.
   
   // TODO the above is a major problem. However if you have data with pointers
   // you generally have a problem like this.
}
