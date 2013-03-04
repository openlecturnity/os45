#if !defined(AFX_UNDOREDOMANAGER_H__A4B439C7_AB19_437D_B76A_36BC09FB3D89__INCLUDED_)
#define AFX_UNDOREDOMANAGER_H__A4B439C7_AB19_437D_B76A_36BC09FB3D89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define E_AD_UNINITIALIZED  _HRESULT_TYPEDEF_(0x80fd0001L)
#define E_URM_INVALID_STATE _HRESULT_TYPEDEF_(0x80fd0002L)

enum UpdateHintId;

enum UndoActionId
{
   UNDO_NO_ACTION,
   UNDO_INSERT_RANGE,
   UNDO_REMOVE_RANGE,
   UNDO_INSERT_OBJECT,
   UNDO_REMOVE_OBJECT,
   UNDO_CHANGE_OBJECT,
   UNDO_MOVE_OBJECT,
   UNDO_LEGACY_CHANGE,
   UNDO_SEPARATOR,
   UNDO_BLOCKER // some actions (during program startup) cannot be undone
};


class IUndoCapable
{
public:
   // there is no need for a special "redo" function as both are nearly the same:
   // they just contain the information to reverse an action
   virtual HRESULT UndoAction(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs = 0, UINT nLengthMs = 0) = 0;

   /**
    * The object implementing this interface must be able to delete its
    * own data. Because if the data contains pointers only this object knows
    * how to delete this data.
    * Method will be called when undo data is deleted
    */
   virtual HRESULT ReleaseData(BYTE *pData, UINT nDataLength, bool bDeleteCompletely, 
      UndoActionId idActionCode, UINT nPositionMs = 0, UINT nLengthMs = 0) = 0;
};


class CUndoActionDefinition
{
public:
   CUndoActionDefinition();
   virtual ~CUndoActionDefinition();

   HRESULT Init(IUndoCapable *pSource, UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UpdateHintId idUpdateType, 
      UINT nPositionMs = 0, UINT nLengthMs = 0, bool bCopyData = true);

   BYTE* GetDataAccess();
   UINT GetPosition() { return m_nPositionMs; }
   UndoActionId GetActionId() { return m_idActionCode; }
   UpdateHintId GetUpdateId() { return m_idUpdateType; }
   HRESULT InvokeUndo();

   bool IsSeparator() { return m_idActionCode == UNDO_SEPARATOR || m_idActionCode == UNDO_BLOCKER; }
   bool IsBlocker() { return m_idActionCode == UNDO_BLOCKER; }

   /** 
    * Only invoked at flushing the redo stack or at the very end of program runtime. 
    * Otherwise the data is a pointer which probably gets reused on the other (undo/redo) stack.
    */
   HRESULT ClearNotCopiedData(bool bIsUndo);

private:
   UndoActionId m_idActionCode;
   CByteArray m_abActionData;
   IUndoCapable *m_pSource;
   UpdateHintId m_idUpdateType;
   UINT m_nPositionMs;
   UINT m_nLengthMs;
   bool m_bDataIsCopy;
   BYTE *m_pbNotCopiedPointer;
};


class CUndoRedoManager  
{
public:
	CUndoRedoManager();
	virtual ~CUndoRedoManager();

   HRESULT Clear();

   // checks which stack to use (undo or redo):
   // if the call to RegisterAction() occured during PerformUndo 
   // it goes to the redo stack otherwise to the undo stack
   HRESULT RegisterAction(IUndoCapable *pSource, UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UpdateHintId idUpdateType, 
      UINT nPositionMs = 0, UINT nLengthMs = 0, bool bCopyData = true);
   
   HRESULT SetRollbackPoint(bool bIsBlocker = false);
   HRESULT PerformUndo(UpdateHintId *pidMaximumUpdate = NULL, CArray<UINT, UINT> *paBeginTimes = NULL);
   HRESULT PerformRedo(UpdateHintId *pidMaximumUpdate = NULL, CArray<UINT, UINT> *paBeginTimes = NULL);

   bool HasUndoElements();
   bool HasRedoElements();

private:
   HRESULT RollbackOneAction(CList<CUndoActionDefinition*, CUndoActionDefinition*> *pList,
      UpdateHintId *pidMaximumUpdate, CArray<UINT, UINT> *paBeginTimes);
   void ClearActionData(CList<CUndoActionDefinition*, CUndoActionDefinition*> *pList, bool bIsUndo);

   CList<CUndoActionDefinition*, CUndoActionDefinition*> m_lstUndoStack;
   CList<CUndoActionDefinition*, CUndoActionDefinition*> m_lstRedoStack;
   bool m_bUndoActive;
   bool m_bRedoActive;
};


#endif // !defined(AFX_UNDOREDOMANAGER_H__A4B439C7_AB19_437D_B76A_36BC09FB3D89__INCLUDED_)
