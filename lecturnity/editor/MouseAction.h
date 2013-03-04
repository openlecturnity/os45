#if !defined(AFX_MOUSEACTION_H__512AFC9B_0868_4E1C_AB58_C2E7E273F843__INCLUDED_)
#define AFX_MOUSEACTION_H__512AFC9B_0868_4E1C_AB58_C2E7E273F843__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArrayOutput.h"
class CEditorProject;
class CEditorDoc;

enum MouseActionId
{
   MOUSE_ACTION_CLICK,
      MOUSE_ACTION_DOWN,
      MOUSE_ACTION_UP,
      MOUSE_ACTION_ENTER,
      MOUSE_ACTION_LEAVE
};

enum AreaActionTypeId
{
   INTERACTION_NO_ACTION = 0,
      INTERACTION_JUMP_NEXT_PAGE	= 1,
      INTERACTION_JUMP_PREVIOUS_PAGE = 2,
      INTERACTION_JUMP_SPECIFIC_PAGE = 3,
      INTERACTION_JUMP_TARGET_MARK	= 4,
      INTERACTION_JUMP_RANDOM_QUESTION = 5,
      INTERACTION_OPEN_URL	= 6,
      INTERACTION_OPEN_FILE = 7,
      INTERACTION_START_REPLAY = 8,
      INTERACTION_STOP_REPLAY	= 9,
      INTERACTION_AUDIO_ON = 10,
      INTERACTION_AUDIO_OFF = 11,
      INTERACTION_EXIT_PROCESS = 12,
      INTERACTION_RESET_QUESTION = 13,
      INTERACTION_CHECK_QUESTION = 14
};

enum AreaActionStringId
{
   NO_ACTION = 0,
   NEXT_PAGE = 1,
   PREVIOUS_PAGE = 2,
   SPECIFIC_PAGE = 3,
   TARGET_MARK	= 4,
   OPEN_URL	= 5,
   OPEN_FILE = 6,
   START_REPLAY = 7,
   STOP_REPLAY	= 8,
   AUDIO_ON = 9,
   AUDIO_OFF = 10,
   EXIT_PROCESS = 11,
   NEXT_PAGE_STARTREPLAY	= 12,
   NEXT_PAGE_STOPREPLAY	= 13,
   PREVIOUS_PAGE_STARTREPLAY = 14,
   PREVIOUS_PAGE_STOPREPLAY = 15,
   SPECIFIC_PAGE_STARTREPLAY = 16,
   SPECIFIC_PAGE_STOPREPLAY = 17,
   TARGET_MARK_STARTREPLAY	= 18,
   TARGET_MARK_STOPREPLAY	= 19,
   OPEN_URL_STARTREPLAY	= 20,
   OPEN_URL_STOPREPLAY	= 21,
   OPEN_FILE_STARTREPLAY = 22,
   OPEN_FILE_STOPREPLAY = 23,
   AUDIO_ON_STARTREPLAY = 24,
   AUDIO_ON_STOPREPLAY = 25,
   AUDIO_OFF_STARTREPLAY = 26,
   AUDIO_OFF_STOPREPLAY = 27,
   RANDOM_QUESTION = 28
};

class CMouseAction : public CObject
{
   DECLARE_DYNCREATE(CMouseAction)

public:
   CMouseAction();
   virtual ~CMouseAction();

   HRESULT Init(CEditorProject *pProject, AreaActionTypeId idAction, 
      UINT nPageMarkId = 0, LPCTSTR tszPath = NULL, AreaActionTypeId idActionFollowup = INTERACTION_NO_ACTION);
   HRESULT Parse(CString& csDefinition, CEditorProject *pProject, UINT nInsertPositionMs);
   HRESULT ResolveJumpTimes(CEditorProject *pProject, UINT nInsertPositionMs);
   HRESULT AddFollowingAction(CMouseAction *pAction);
   HRESULT CloneTo(CMouseAction *pTarget);
   CMouseAction* Copy();

   HRESULT Execute(CEditorDoc *pDoc);

   AreaActionTypeId GetActionId() { return m_idAction; }
   void SetActionId(AreaActionTypeId actionId);
   UINT GetPageMarkId() { return m_nPageMarkId; }
   void SetPageMarkId(UINT nPageMarkId) {m_nPageMarkId = nPageMarkId;}
   LPCTSTR GetPath() { return m_tszPath; }
   CMouseAction *GetNextAction() { return m_pNextAction; }

   void ChangeOutputFileName(UINT nUsageCount) { m_nNameUsageCount = nUsageCount; }
   HRESULT AppendData(CArrayOutput *pTarget, LPCTSTR tszExportPrefix, CEditorProject *pProject);

   static void GetActionStrings(CStringArray &aActionStrings, bool bIsRandomTest);
   static AreaActionStringId GetActionString(CMouseAction *pMouseAction);
   static AreaActionTypeId GetActionId(AreaActionStringId nActionStringId, 
                                       AreaActionTypeId &nNextActionId);

private:
   CEditorProject *m_pEditorProject;
   AreaActionTypeId m_idAction;
   UINT m_nPageMarkId;
   _TCHAR *m_tszPath;
   UINT m_nNameUsageCount;

   CMouseAction *m_pNextAction;
};

#endif // !defined(AFX_MOUSEACTION_H__512AFC9B_0868_4E1C_AB58_C2E7E273F843__INCLUDED_)
