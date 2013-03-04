#include "stdafx.h"
#include "InteractionAreaEx.h"
#include "editorDoc.h"
#include "QuestionnaireEx.h"
#include "QuestionStream.h"
#include "InteractionStream.h"

/* REVIEW UK
 * Remove double code in SetInteractionName() (ie loop 0..3 and taking a different array each time)
 */

CTimePeriod::CTimePeriod()
{
   m_idPeriod = INTERACTION_PERIOD_NEVER;
   m_nPageStartMs = 0xffffffff;
}

CTimePeriod::~CTimePeriod()
{

}

HRESULT CTimePeriod::Init(AreaPeriodId idPeriod, UINT nPageMarkTime, UINT nPageEndTime, bool bAllowLengthOne)
{
   HRESULT hr = S_OK;

   if (nPageEndTime < nPageMarkTime)
      return E_INVALIDARG;

   m_idPeriod = idPeriod;

   if (idPeriod == INTERACTION_PERIOD_PAGE || idPeriod == INTERACTION_PERIOD_END_PAGE)
   {
      
      if (nPageMarkTime == nPageEndTime && !bAllowLengthOne)
      {
         // was previously used for END_PAGE but shouldn't anymore; see below
         _ASSERT(false);
      }

      // in order for hit check during stream operations (Paste) to work properly
      m_nPageStartMs = nPageMarkTime;
      
      if (idPeriod == INTERACTION_PERIOD_END_PAGE)
      {
         // in order for hit check during visualization to work properly
         nPageMarkTime = nPageEndTime; 
      }
   }

   hr = m_RawTimes.Init(NULL, nPageMarkTime, nPageEndTime - nPageMarkTime + 1);

   return hr;
}

HRESULT CTimePeriod::Parse(CString& csTimesDefine)
{
   // Note: the timely insert position of an area (in the document)
   // is changed/added after parsing.

   if (csTimesDefine.GetLength() > 0)
   {
      CString csRegion = csTimesDefine;
      StringManipulation::GetParameter(csRegion);
      
      CString csStartMs = csRegion;
      CString csEndMs = csRegion;

      int idxMinus = csRegion.Find(_T('-'));
      if (idxMinus > -1)
      {
         csStartMs = csRegion.Left(idxMinus);
         csEndMs = csRegion.Right(csRegion.GetLength() - idxMinus - 1);
      }

      if (csStartMs.GetLength() == 0 || csEndMs.GetLength() == 0 || csEndMs.GetLength() < csStartMs.GetLength())
         return E_IAX_INVALID_DATA;

      UINT nStartMs = _ttoi(csStartMs);
      UINT nEndMs = _ttoi(csEndMs);

      if (nEndMs < nStartMs)
         return E_IAX_INVALID_DATA;


      AreaPeriodId idPeriod = INTERACTION_PERIOD_NEVER;
      UINT nPageMarkId = 0;

      if (StringManipulation::StartsWith(csTimesDefine, CString(_T("all-pages"))))
      {
         idPeriod = INTERACTION_PERIOD_ALL_PAGES;
      }
      else if (StringManipulation::StartsWith(csTimesDefine, CString(_T("page-end"))))
      {
         idPeriod = INTERACTION_PERIOD_END_PAGE;
      }
      else if (StringManipulation::StartsWith(csTimesDefine, CString(_T("page"))))
      {
         idPeriod = INTERACTION_PERIOD_PAGE;
      }
      else if (StringManipulation::StartsWith(csTimesDefine, CString(_T("time"))))
      {
         idPeriod = INTERACTION_PERIOD_TIME;
      }
      else
      {
         return E_IAX_INVALID_DATA;
      }


      return Init(idPeriod, nStartMs, nEndMs, idPeriod == INTERACTION_PERIOD_END_PAGE);
   }
   else
      return S_FALSE;
}

HRESULT CTimePeriod::ResolveTimes(CEditorProject *pProject)
{
   HRESULT hr = S_OK;

   if (m_idPeriod == INTERACTION_PERIOD_END_PAGE)
   {
      if (m_nPageStartMs == m_RawTimes.GetTargetBegin())
      {
         UINT nPageStartMs = 0;
         UINT nPageEndMs = 0;

         hr = pProject->GetPagePeriod(m_RawTimes.GetTargetBegin(), nPageStartMs, nPageEndMs);
         if (SUCCEEDED(hr))
            m_nPageStartMs = nPageStartMs;
      }  
   }

   return hr;
}

HRESULT CTimePeriod::CloneTo(CTimePeriod *pTarget)
{
   if (pTarget == NULL)
      return E_POINTER;

   pTarget->m_idPeriod = m_idPeriod;
   pTarget->m_nPageStartMs = m_nPageStartMs;

   return m_RawTimes.CloneTo(&pTarget->m_RawTimes);

   //return ClonePartTo(pTarget, 0, GetEnd() + 1); // clone entire range
}

/*
HRESULT CTimePeriod::ClonePartTo(CTimePeriod *pTarget, UINT nFromMs, UINT nLengthMs)
{
   if (pTarget == NULL)
      return E_POINTER;

   if (m_idPeriod != INTERACTION_PERIOD_ALL_PAGES && !m_RawTimes.IntersectsRange(nFromMs, nLengthMs))
      return E_INVALIDARG;

   pTarget->m_idPeriod = m_idPeriod;
   pTarget->m_nPageStartMs = m_nPageStartMs;

   m_RawTimes.CloneTo(&pTarget->m_RawTimes);
   if (m_idPeriod != INTERACTION_PERIOD_ALL_PAGES)
   {
      pTarget->m_RawTimes.TrimToRange(nFromMs, nLengthMs);
      pTarget->DecrementBegin(nFromMs);
   }

   return S_OK;
}
*/

HRESULT CTimePeriod::EnlargeWith(UINT nFromMs, UINT nLengthMs)
{
   HRESULT hr = S_OK;

   if (m_idPeriod == INTERACTION_PERIOD_NEVER)
      return E_IAX_INVALID_STATE;

   if (m_idPeriod == INTERACTION_PERIOD_ALL_PAGES)
      return S_OK;

   bool bIsEndPage = m_idPeriod == INTERACTION_PERIOD_END_PAGE;

   if (IntersectsRange(nFromMs, nLengthMs, bIsEndPage) || MeetsRange(nFromMs, nLengthMs, bIsEndPage))
   {
      if (m_nPageStartMs != 0xffffffff && nFromMs < m_nPageStartMs)
      {
         UINT nDiff = m_nPageStartMs - nFromMs;

         if (nLengthMs < nDiff && bIsEndPage)
         {
            _ASSERT(false);
            return E_INVALIDARG; // wrong parameters?
         }

         m_nPageStartMs -= nDiff;

         if (bIsEndPage)
            nLengthMs -= nDiff;
      }

      if (!bIsEndPage)
      {
         // normal case
         
         if (nFromMs < m_RawTimes.GetTargetBegin())
            hr = m_RawTimes.DecrementTargetBegin(m_RawTimes.GetTargetBegin() - nFromMs);
         
         if (SUCCEEDED(hr))
            hr = m_RawTimes.IncreaseLength(nLengthMs);
      }
      else
         hr = m_RawTimes.Init(NULL, m_RawTimes.GetTargetBegin() + nLengthMs, 1);
   }
   else
      hr = E_INVALIDARG;

   return hr;
}

HRESULT CTimePeriod::IncreaseLength(UINT nLengthMs)
{
   HRESULT hr = S_OK;

   if (m_idPeriod == INTERACTION_PERIOD_NEVER)
      return E_IAX_INVALID_STATE;

   if (m_idPeriod == INTERACTION_PERIOD_ALL_PAGES)
      return S_OK;

   bool bIsEndPage = m_idPeriod == INTERACTION_PERIOD_END_PAGE;

   if (!bIsEndPage)
      hr = m_RawTimes.IncreaseLength(nLengthMs);
   else
      hr = m_RawTimes.Init(NULL, m_RawTimes.GetTargetBegin() + nLengthMs, 1);

   return hr;
}
   

HRESULT CTimePeriod::TrimTo(UINT nFromMs, UINT nLengthMs)
{
   HRESULT hr = S_OK;

   if (m_idPeriod == INTERACTION_PERIOD_NEVER)
      return E_IAX_INVALID_STATE;

   if (m_idPeriod == INTERACTION_PERIOD_ALL_PAGES)
      return S_OK;

   bool bIsPageEnd = m_idPeriod == INTERACTION_PERIOD_END_PAGE;

   bool bIntersects = IntersectsRange(nFromMs, nLengthMs, bIsPageEnd);

   if (!bIntersects)
      return E_INVALIDARG;

   if (m_nPageStartMs != 0xffffffff && nFromMs > m_nPageStartMs)
      m_nPageStartMs = nFromMs;
   
   if (!bIsPageEnd)
      hr = m_RawTimes.TrimToRange(nFromMs, nLengthMs);
   else
   {
      UINT nPageEndMs = m_RawTimes.GetTargetBegin();

      UINT nNewPageEndMs = nFromMs + nLengthMs - 1;

      if (nNewPageEndMs < nPageEndMs)
         hr = m_RawTimes.Init(NULL, nNewPageEndMs, 1);
      else
         hr = S_OK;
   }

   
   // INTERACTION_PERIOD_TIME should have a length (1 ms)
   // and after this it will possibly be 0

   if (SUCCEEDED(hr) && m_RawTimes.GetLength() == 0)
      m_idPeriod = INTERACTION_PERIOD_NEVER;
 
   return hr;
}
   

HRESULT CTimePeriod::Subtract(UINT nFromMs, UINT nLengthMs)
{
   HRESULT hr = S_OK;

   if (m_idPeriod == INTERACTION_PERIOD_NEVER)
      return E_IAX_INVALID_STATE;
 
   if (m_idPeriod == INTERACTION_PERIOD_ALL_PAGES)
      return S_OK;

   bool bIsPageEnd = m_idPeriod == INTERACTION_PERIOD_END_PAGE;

   UINT nBegin = GetBegin(bIsPageEnd);
   UINT nEnd = GetEnd();

   bool bEntirelyWithin = nBegin > nFromMs && nEnd <= nFromMs + nLengthMs - 1;

   if (m_nPageStartMs != 0xffffffff && nFromMs < m_nPageStartMs)
      m_nPageStartMs = nFromMs;

   if (!bEntirelyWithin)
   {
      if (IntersectsRange(nFromMs, nLengthMs, bIsPageEnd))
      {
         if (!bIsPageEnd)
            hr = m_RawTimes.CutOut(nFromMs, nLengthMs, true);
         else
         {
            UINT nPageEndMs = m_RawTimes.GetTargetBegin();

            if (nPageEndMs > nFromMs && nFromMs > 0)
               hr = m_RawTimes.Init(NULL, nFromMs - 1, 1);
            else
               hr = S_OK; // do nothing
         }
      }
      else
         hr = S_OK; // do nothing
   }
   else
   {
      hr = m_RawTimes.Init(NULL, 0, 0); // make it empty
   }

   
   // INTERACTION_PERIOD_TIME should have a length (1 ms)
   // and after this it will possibly be 0

   if (SUCCEEDED(hr) && m_RawTimes.GetLength() == 0)
      m_idPeriod = INTERACTION_PERIOD_NEVER;

   return hr;
}


HRESULT CTimePeriod::DecrementBegin(UINT nMoveMs)
{
   if (m_nPageStartMs != 0xffffffff)
      m_nPageStartMs -= nMoveMs;

   return m_RawTimes.DecrementTargetBegin(nMoveMs);
}

HRESULT CTimePeriod::IncrementBegin(UINT nMoveMs)
{
   if (m_nPageStartMs != 0xffffffff)
      m_nPageStartMs += nMoveMs;

   return m_RawTimes.IncrementTargetBegin(nMoveMs);
}

UINT CTimePeriod::GetBegin(bool bUsePageTimes)
{
   if (!bUsePageTimes || m_idPeriod != INTERACTION_PERIOD_END_PAGE)
      return m_RawTimes.GetTargetBegin();
   else
      return m_nPageStartMs;
}

UINT CTimePeriod::GetEnd()
{
   return m_RawTimes.GetTargetBegin() + m_RawTimes.GetLength() - 1;
}

UINT CTimePeriod::GetLength(bool bUsePageTimes)
{
   if (!bUsePageTimes || m_idPeriod != INTERACTION_PERIOD_END_PAGE)
      return m_RawTimes.GetLength();
   else
      return GetEnd() - GetBegin(true) + 1;
}

HRESULT CTimePeriod::SetDocumentLength(UINT nLengthMs)
{
   if (m_idPeriod != INTERACTION_PERIOD_ALL_PAGES)
   {
      // only supported and necessary for ALL_PAGES
      _ASSERT(false);
      return E_UNEXPECTED;
   }

   return m_RawTimes.Init(NULL, 0, nLengthMs);
}

bool CTimePeriod::Contains(UINT nPositionMs, bool bUsePageTimes)
{
   if (m_idPeriod == INTERACTION_PERIOD_ALL_PAGES)
      return true;

   if (m_idPeriod == INTERACTION_PERIOD_NEVER)
      return false;

   if (bUsePageTimes && m_idPeriod == INTERACTION_PERIOD_END_PAGE)
      return nPositionMs >= GetBegin(true) && nPositionMs <= GetEnd();
 
   return m_RawTimes.Contains(nPositionMs);
}
 
bool CTimePeriod::IntersectsRange(UINT nFromMs, UINT nLengthMs, bool bUsePageTimes)
{
   if (m_idPeriod == INTERACTION_PERIOD_ALL_PAGES)
      return true;

   if (m_idPeriod == INTERACTION_PERIOD_NEVER)
      return false;
   
   if (bUsePageTimes && m_idPeriod == INTERACTION_PERIOD_END_PAGE)
   {
      UINT nOtherEnd = nFromMs + nLengthMs - 1;

      UINT nBegin = GetBegin(true);
      UINT nEnd = GetEnd();

      return nFromMs >= nBegin && nFromMs <= nEnd 
         || nOtherEnd >= nBegin && nOtherEnd <= nEnd 
         || nFromMs < nBegin && nOtherEnd > nEnd;
   }

   return m_RawTimes.IntersectsRange(nFromMs, nLengthMs);
}

bool CTimePeriod::MeetsRange(UINT nFromMs, UINT nLengthMs, bool bUsePageTimes)
{
   if (m_idPeriod == INTERACTION_PERIOD_ALL_PAGES)
      return false;

   if (m_idPeriod == INTERACTION_PERIOD_NEVER)
      return false;

   return GetBegin(bUsePageTimes) == nFromMs + nLengthMs 
      || GetBegin(bUsePageTimes) + GetLength(bUsePageTimes) == nFromMs;
}

bool CTimePeriod::IntersectsRange(CTimePeriod *pOther)
{
   if (pOther == NULL)
      return false;

   return IntersectsRange(pOther->GetBegin(), pOther->GetLength());
}
   
bool CTimePeriod::EntirelyWithin(UINT nFromMs, UINT nLengthMs)
{
   if (m_idPeriod == INTERACTION_PERIOD_ALL_PAGES)
      return false;

   if (m_idPeriod == INTERACTION_PERIOD_NEVER)
      return true;

   return m_RawTimes.EntirelyWithin(nFromMs, nLengthMs);
}

bool CTimePeriod::EntirelyWithin(CTimePeriod *pOther)
{
   if (pOther == NULL)
      return false;

   return EntirelyWithin(pOther->GetBegin(), pOther->GetLength());
}
   
bool CTimePeriod::IsEmpty()
{
   if (m_idPeriod == INTERACTION_PERIOD_ALL_PAGES)
      return false;

   if (m_idPeriod == INTERACTION_PERIOD_NEVER || m_RawTimes.GetLength() == 0)
      return true;
   else
      return false;
}

HRESULT CTimePeriod::RegisterWithMark(CEditorProject *pProject)
{
   if (m_idPeriod != INTERACTION_PERIOD_NEVER)
   {
      UINT nEndTimeMs = GetEnd();
      if (m_idPeriod == INTERACTION_PERIOD_ALL_PAGES)
         nEndTimeMs = pProject->GetAudioPreviewLength();
 
      CStopJumpMark *pMark = pProject->GetStopMarkAt(nEndTimeMs);
      if (pMark != NULL)
         pMark->RegisterUser();
   }

   return S_OK;
}

HRESULT CTimePeriod::UnregisterWithMark(CEditorProject *pProject)
{
   if (m_idPeriod != INTERACTION_PERIOD_NEVER)
   {
      UINT nEndTimeMs = GetEnd();
      if (m_idPeriod == INTERACTION_PERIOD_ALL_PAGES)
         nEndTimeMs = pProject->GetAudioPreviewLength();
 
      CStopJumpMark *pMark = pProject->GetStopMarkAt(nEndTimeMs);
      if (pMark != NULL)
         pMark->UnregisterUser();
   }

   return S_OK;
}

HRESULT CTimePeriod::AppendTimes(_TCHAR *tszOutput)
{
   if (tszOutput == NULL)
      return E_POINTER;

   switch (GetPeriodId())
   {
   case INTERACTION_PERIOD_PAGE:
      _tcscat(tszOutput, _T("page"));
      tszOutput += _tcslen(tszOutput);
      break;
      
   case INTERACTION_PERIOD_END_PAGE:
      _tcscat(tszOutput, _T("page-end"));
      tszOutput += _tcslen(tszOutput);
      break;
      
   case INTERACTION_PERIOD_TIME:
      _tcscat(tszOutput, _T("time"));
      tszOutput += _tcslen(tszOutput);
      break;
      
   case INTERACTION_PERIOD_ALL_PAGES:
      _tcscat(tszOutput, _T("all-pages"));
      tszOutput += _tcslen(tszOutput);
      break;
   }
   
   if (GetLength() <= 1)
      _stprintf(tszOutput, _T(" %d"), GetBegin());
   else
      _stprintf(tszOutput, _T(" %d-%d"), GetBegin(), GetEnd());
   tszOutput += _tcslen(tszOutput);

   return S_OK;
}
   

HRESULT CTimePeriod::AppendTimes(CArrayOutput *pTarget)
{
   // if INTERACTION_PERIOD_NEVER: no output

   if (pTarget == NULL)
      return E_POINTER;

   static _TCHAR tszOutput[200];
   tszOutput[0] = 0;
   
   AppendTimes(tszOutput);
   
   pTarget->Append(tszOutput);
   
   return S_OK;
}



//////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CInteractionAreaEx, CObject)

int CInteractionAreaEx::s_iUpperStacking = 0; // highest stacking on start
int CInteractionAreaEx::s_iLowerStacking = -1; // lowest stacking on start

CInteractionAreaEx::CInteractionAreaEx()
{
   m_nHashKey = 0;

   m_iStackingLevel = s_iUpperStacking; // default: one over each other
   ++s_iUpperStacking;

   m_pEditorProject = NULL;

   m_rcDimensions = CRect(0,0,0,0);
   m_fMoveOverhangX = 0;
   m_fMoveOverhangY = 0;
   m_bHandCursor = false;

   m_bSelected = false;

   m_pClickAction = NULL;
   m_pDownAction = NULL;
   m_pUpAction = NULL;
   m_pEnterAction = NULL;
   m_pLeaveAction = NULL;

   m_Visibility.Init(INTERACTION_PERIOD_ALL_PAGES, 0, 0);
   m_Activity.Init(INTERACTION_PERIOD_NEVER, 0, 0);

   m_bTimesResolved = false;
   m_nQuestionJoinId = 0;

   m_bIsInitialized = false;

   m_bIsButton = false;
   m_bIsImageButton = false;

   m_bMouseOver = false;
   m_bMousePressed = false;
   m_bActive = false;
   m_bDisabledForPreview = false;
   m_bHiddenDuringEdit = false;
   m_bTimelineDisplay = true;
   m_bHiddenDuringPreview = false;
   m_bDemoDocumentObject = false;

   m_bAutoMove = false;

}

CInteractionAreaEx::~CInteractionAreaEx()
{
   Clear();
}

// private
void CInteractionAreaEx::Clear() {
    m_rcDimensions = CRect(0,0,0,0);
    m_bHandCursor = false;

    // Stacking order needn't be reset.

    if (m_pClickAction != NULL)
        delete m_pClickAction;
    m_pClickAction = NULL;

    if (m_pDownAction != NULL)
        delete m_pDownAction;
    m_pDownAction = NULL;

    if (m_pUpAction != NULL)
        delete m_pUpAction;
    m_pUpAction = NULL;

    if (m_pEnterAction != NULL)
        delete m_pEnterAction;
    m_pEnterAction = NULL;

    if (m_pLeaveAction != NULL)
        delete m_pLeaveAction;
    m_pLeaveAction = NULL;

    UnregisterMarks();
    RemoveButtonObjects();

    m_bIsInitialized = false;
    m_bDemoDocumentObject = false;
}

void CInteractionAreaEx::UnregisterMarks() {
    if (m_pEditorProject != NULL) {
        m_Visibility.UnregisterWithMark(m_pEditorProject);
    }

    if (m_pEditorProject != NULL) {
        m_Activity.UnregisterWithMark(m_pEditorProject);
    }
}

HRESULT CInteractionAreaEx::Init(CRect *prcRect, bool bHandCursor, CMouseAction *pClickAction,
                                 CMouseAction *pDownAction, CMouseAction *pUpAction,
                                 CMouseAction *pEnterAction, CMouseAction *pLeaveAction,
                                 CTimePeriod *pVisibility, CTimePeriod *pActivity,
                                 CEditorProject *pProject)
{
   HRESULT hr = S_OK;

   if (m_bIsInitialized)
      return E_IAX_INVALID_STATE;  // you cannot call this twice

   // Note: it's possible that there is no action: i.e. for moveable objects

   if (pProject == NULL)
      return E_POINTER;

   Clear();

   m_pEditorProject = pProject;

   m_rcDimensions = *prcRect;
   m_bHandCursor = bHandCursor;

   // Bugfix #3383: Also normal areas may not be "invisible" because they won't
   //               be found for hit testing then.
   //if (GetClassId() == INTERACTION_CLASS_AREA) // only do this for non-extended classes
   //   m_bHiddenDuringPreview = true;

   if (pClickAction != NULL && SUCCEEDED(hr))
   {
      m_pClickAction = new CMouseAction;
      hr = pClickAction->CloneTo(m_pClickAction);
   }

   if (pUpAction != NULL && SUCCEEDED(hr))
   {
      m_pUpAction = new CMouseAction;
      hr = pUpAction->CloneTo(m_pUpAction);
   }

   if (pDownAction != NULL && SUCCEEDED(hr))
   {
      m_pDownAction = new CMouseAction;
      hr = pDownAction->CloneTo(m_pDownAction);
   }

   if (pEnterAction != NULL && SUCCEEDED(hr))
   {
      m_pEnterAction = new CMouseAction;
      hr = pEnterAction->CloneTo(m_pEnterAction);
   }

   if (pLeaveAction != NULL && SUCCEEDED(hr))
   {
      m_pLeaveAction = new CMouseAction;
      hr = pLeaveAction->CloneTo(m_pLeaveAction);
   }
   
   // Note: 
   // * The visibility is not so important, as for example normal areas have none.
   // * Normal areas and buttons need an activity but extension classes maybe don't.

   if (SUCCEEDED(hr) && pVisibility != NULL)
   {
      hr = pVisibility->CloneTo(&m_Visibility);

      if (SUCCEEDED(hr))
         m_Visibility.RegisterWithMark(m_pEditorProject);
   }

   
   if (SUCCEEDED(hr) && pActivity != NULL)
   {
      hr = pActivity->CloneTo(&m_Activity);
   
      if (m_Activity.GetPeriodId() != INTERACTION_PERIOD_NEVER && SUCCEEDED(hr))
         m_Activity.RegisterWithMark(m_pEditorProject);
   }


   if (SUCCEEDED(hr))
      m_bTimesResolved = true;
   // this is no parsing or/and all subclasses have no actions with jumps
 
   CreateNameAndType();

   if (SUCCEEDED(hr))
      m_bIsInitialized = true;
   
   return hr;
}

HRESULT CInteractionAreaEx::InitButton(ButtonTypeId idButtonType,
                                       CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paNormalObjects,
                                       CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paOverObjects,
                                       CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paPressedObjects,
                                       CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paInactiveObjects,
                                       bool bIsImageButton)
{
   HRESULT hr = S_OK;

   if (!m_bIsInitialized)
      return E_IAX_INVALID_STATE; // you need to call the normal Init() first

   if (m_bIsButton)
      return E_IAX_INVALID_STATE;  // you cannot call this twice

   if (paNormalObjects == NULL || paOverObjects == NULL || 
      paPressedObjects == NULL || paInactiveObjects == NULL)
      return E_POINTER;
 
   RemoveButtonObjects();

   m_bIsButton = true;
   m_bIsImageButton = bIsImageButton;
   m_idButtonType = idButtonType;
   m_bHiddenDuringPreview = false;

   // copy given objects
   m_aNormalObjects.SetSize(0, max(paNormalObjects->GetSize(), 10));
   for (int i=0; i<paNormalObjects->GetSize(); ++i)
      m_aNormalObjects.Add(paNormalObjects->GetAt(i)->Copy());
   m_aOverObjects.SetSize(0, max(paOverObjects->GetSize(), 10));
   for (i=0; i<paOverObjects->GetSize(); ++i)
      m_aOverObjects.Add(paOverObjects->GetAt(i)->Copy());
   m_aPressedObjects.SetSize(0, max(paPressedObjects->GetSize(), 10));
   for (i=0; i<paPressedObjects->GetSize(); ++i)
      m_aPressedObjects.Add(paPressedObjects->GetAt(i)->Copy());
   m_aInactiveObjects.SetSize(0, max(paInactiveObjects->GetSize(), 10));
   for (i=0; i<paInactiveObjects->GetSize(); ++i)
      m_aInactiveObjects.Add(paInactiveObjects->GetAt(i)->Copy());

   CreateNameAndType();

   return hr;
}


HRESULT CInteractionAreaEx::Init(CInteractionArea *pInteraction, CEditorProject *pProject, UINT nInsertPositionMs)
{
   HRESULT hr = S_OK;

   if (pInteraction == NULL)
      return E_POINTER;

   if (!pInteraction->IsInitialized())
      return E_IAX_UNINITIALIZED;

   Clear();

   m_pEditorProject = pProject;
   m_bDemoDocumentObject = pInteraction->IsDemoDocumentObject();

   // Stacking order was initialized during construction.

   // m_nHashKey will be generated on demand

   m_rcDimensions = pInteraction->GetActivityArea();
   m_bHandCursor = pInteraction->IsHandCursor();

   CString csClickAction = pInteraction->GetMouseClickAction();
   if (SUCCEEDED(hr) && csClickAction.GetLength() > 0)
   {
      m_pClickAction = new CMouseAction;
      hr = m_pClickAction->Parse(csClickAction, pProject, nInsertPositionMs);
   }

   CString csDownAction = pInteraction->GetMouseDownAction();
   if (SUCCEEDED(hr) && csDownAction.GetLength() > 0)
   {
      m_pDownAction = new CMouseAction;
      hr = m_pDownAction->Parse(csDownAction, pProject, nInsertPositionMs);
   }

   CString csUpAction = pInteraction->GetMouseUpAction();
   if (SUCCEEDED(hr) && csUpAction.GetLength() > 0)
   {
      m_pUpAction = new CMouseAction;
      hr = m_pUpAction->Parse(csUpAction, pProject, nInsertPositionMs);
   }

   CString csEnterAction = pInteraction->GetMouseEnterAction();
   if (SUCCEEDED(hr) && csEnterAction.GetLength() > 0)
   {
      m_pEnterAction = new CMouseAction;
      hr = m_pEnterAction->Parse(csEnterAction, pProject, nInsertPositionMs);
   }

   CString csLeaveAction = pInteraction->GetMouseLeaveAction();
   if (SUCCEEDED(hr) && csLeaveAction.GetLength() > 0)
   {
      m_pLeaveAction = new CMouseAction;
      hr = m_pLeaveAction->Parse(csLeaveAction, pProject, nInsertPositionMs);
   }

   CString csVisibility = pInteraction->GetVisibility();
   if (SUCCEEDED(hr))
   {
      if (csVisibility.GetLength() > 0)
         hr = m_Visibility.Parse(csVisibility);
      else
      {
         if (!pInteraction->IsButton())
            hr = m_Visibility.Init(INTERACTION_PERIOD_ALL_PAGES, 0, 0); // visibility must not be given
         else
            hr = E_IAX_INVALID_DATA; // a button needs a visibility (but no activity)
      }
      
   }

   CString csActivity = pInteraction->GetActivity();
   if (SUCCEEDED(hr))
   {
      if (csActivity.GetLength() > 0)
         hr = m_Activity.Parse(csActivity);
      else 
      {
         if (!pInteraction->IsButton())
            hr = E_IAX_INVALID_DATA; // activity MUST be given for normal areas
         else
            m_Activity.Init(INTERACTION_PERIOD_NEVER, 0, 0);
      }

   }


   if (SUCCEEDED(hr))
   {
      if (pInteraction->IsButton())
      {
         m_bIsButton = true;
         
         CString csButtonType = pInteraction->GetButtonType();

         if (csButtonType.Compare(_T("submit")) == 0)
            m_idButtonType = BUTTON_TYPE_SUBMIT;
         else if (csButtonType.Compare(_T("delete")) == 0)
            m_idButtonType = BUTTON_TYPE_DELETE;
         else if (csButtonType.Compare(_T("ok")) == 0)
            m_idButtonType = BUTTON_TYPE_OK;
         else
            m_idButtonType = BUTTON_TYPE_OTHER;
        
         m_aNormalObjects.Append(*pInteraction->GetNormalObjects());
         m_aOverObjects.Append(*pInteraction->GetOverObjects());
         m_aPressedObjects.Append(*pInteraction->GetPressedObjects());
         m_aInactiveObjects.Append(*pInteraction->GetInactiveObjects());
         
         bool bSomeImages = false;
         for (int i=0; i<m_aNormalObjects.GetSize(); ++i)
         {
            if (m_aNormalObjects[i]->GetType() == DrawSdk::IMAGE)
            {
               bSomeImages = true;
               break;
            }
         }

         m_bIsImageButton = bSomeImages;
      }
      else
      {
         m_bIsButton = false;

         // Bugfix #3383: Also normal areas may not be "invisible" because they won't
         //               be found for hit testing then.
         //m_bHiddenDuringPreview = true;
      }
   }

   CreateNameAndType();


   if (SUCCEEDED(hr))
      m_bIsInitialized = true;

   // Note: ResolveJumpTimes() must (should) be called after this.

   return hr;
}

bool CInteractionAreaEx::IsQuestionObject()
{
   bool bIsQuestion = true;

   if (GetClassId() == INTERACTION_CLASS_AREA || GetClassId() == INTERACTION_CLASS_GRAPHICAL_OBJECT)
      bIsQuestion = false;
   else if (GetClassId() == INTERACTION_CLASS_BUTTON)
   {
      ButtonTypeId idButtonType = GetButtonType();

      if (idButtonType == BUTTON_TYPE_OTHER)
         bIsQuestion = false;
   }

   return bIsQuestion;
}

InteractionClassId CInteractionAreaEx::GetClassId()
{
   if (!m_bIsInitialized)
      return INTERACTION_CLASS_NONE;

   if (m_bIsButton)
      return INTERACTION_CLASS_BUTTON;
   else
      return INTERACTION_CLASS_AREA;
}

HRESULT CInteractionAreaEx::ResolveTimes(CEditorProject *pProject, UINT nInsertPositionMs)
{
   HRESULT hr = S_OK;

   if (pProject == NULL)
      return E_POINTER;

   if (!m_bTimesResolved)
   {
      if (m_pClickAction != NULL && SUCCEEDED(hr))
         hr = m_pClickAction->ResolveJumpTimes(pProject, nInsertPositionMs);
      
      if (m_pDownAction != NULL && SUCCEEDED(hr))
         hr = m_pDownAction->ResolveJumpTimes(pProject, nInsertPositionMs);
      
      if (m_pUpAction != NULL && SUCCEEDED(hr))
         hr = m_pUpAction->ResolveJumpTimes(pProject, nInsertPositionMs);
      
      if (m_pEnterAction != NULL && SUCCEEDED(hr))
         hr = m_pEnterAction->ResolveJumpTimes(pProject, nInsertPositionMs);
      
      if (m_pLeaveAction != NULL && SUCCEEDED(hr))
         hr = m_pLeaveAction->ResolveJumpTimes(pProject, nInsertPositionMs);

      m_Visibility.ResolveTimes(pProject);
      m_Activity.ResolveTimes(pProject);

      m_Visibility.RegisterWithMark(pProject);
      m_Activity.RegisterWithMark(pProject);

      
      m_bTimesResolved = true;
   }
  
   return hr;
}

CInteractionAreaEx* CInteractionAreaEx::MakeNew()
{
   return new CInteractionAreaEx();
}
   
CInteractionAreaEx* CInteractionAreaEx::Copy(bool bCopyHashKey)
{
   CInteractionAreaEx* pNewArea = MakeNew();

   HRESULT hr = CloneTo(pNewArea, bCopyHashKey);

   if (FAILED(hr) && pNewArea != NULL)
   {
      delete pNewArea;
      pNewArea = NULL;
   }

   return pNewArea;
}

// protected
HRESULT CInteractionAreaEx::CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey)
{
   HRESULT hr = S_OK;

   if (pTarget == NULL)
      return E_POINTER;

   pTarget->Clear();


   // Normally the hash key is not copied (it is a new area);
   // however for copies to undo stack or clipboard it is.
   if (bCopyHashKey)
   {
      pTarget->m_nHashKey = m_nHashKey;
      pTarget->m_iStackingLevel = m_iStackingLevel; // TODO "side" effect
   }

   // The stacking order is different and was created during construction of pTarget.

   pTarget->m_pEditorProject = m_pEditorProject;

   pTarget->m_rcDimensions = m_rcDimensions;
   pTarget->m_bHandCursor = m_bHandCursor;

   pTarget->m_bHiddenDuringEdit = m_bHiddenDuringEdit;
   pTarget->m_bHiddenDuringPreview = m_bHiddenDuringPreview;

   pTarget->m_csDisplayName = m_csDisplayName;
   pTarget->m_csDisplayType = m_csDisplayType;
   pTarget->m_bDemoDocumentObject = m_bDemoDocumentObject;

   if (m_pClickAction != NULL)
   {
      pTarget->m_pClickAction = new CMouseAction;
      m_pClickAction->CloneTo(pTarget->m_pClickAction);
   }

   if (m_pDownAction != NULL)
   {
      pTarget->m_pDownAction = new CMouseAction;
      m_pDownAction->CloneTo(pTarget->m_pDownAction);
   }

   if (m_pUpAction != NULL)
   {
      pTarget->m_pUpAction = new CMouseAction;
      m_pUpAction->CloneTo(pTarget->m_pUpAction);
   }

   if (m_pEnterAction != NULL)
   {
      pTarget->m_pEnterAction = new CMouseAction;
      m_pEnterAction->CloneTo(pTarget->m_pEnterAction);
   }

   if (m_pLeaveAction != NULL)
   {
      pTarget->m_pLeaveAction = new CMouseAction;
      m_pLeaveAction->CloneTo(pTarget->m_pLeaveAction);
   }

   m_Visibility.CloneTo(&pTarget->m_Visibility);
   m_Activity.CloneTo(&pTarget->m_Activity);

   
   if (IsButton())
   {
      pTarget->m_bIsButton = true;
      pTarget->m_bIsImageButton = m_bIsImageButton;
      pTarget->m_idButtonType = m_idButtonType;

      // TODO maybe distinguish between normal clone and one without copies?

      // make copies of the used DrawObjects
      for (int i=0; i<m_aNormalObjects.GetSize(); ++i)
         pTarget->m_aNormalObjects.Add(m_aNormalObjects[i]->Copy());
      for (i=0; i<m_aOverObjects.GetSize(); ++i)
         pTarget->m_aOverObjects.Add(m_aOverObjects[i]->Copy());
      for (i=0; i<m_aPressedObjects.GetSize(); ++i)
         pTarget->m_aPressedObjects.Add(m_aPressedObjects[i]->Copy());
      for (i=0; i<m_aInactiveObjects.GetSize(); ++i)
         pTarget->m_aInactiveObjects.Add(m_aInactiveObjects[i]->Copy());
      
   }
   else
      pTarget->m_bIsButton = false;

   if (SUCCEEDED(hr))
      pTarget->m_bIsInitialized = true;

   return hr;
}

HRESULT CInteractionAreaEx::ChangeAppearance(CQuestionnaireEx *pQuestionnaire)
{
   if (pQuestionnaire == NULL)
      return E_INVALIDARG;

   HRESULT hr = S_OK;

   hr = RemoveButtonObjects();

   COLORREF clrFill, clrLine, clrText;
   LOGFONT lf;
   CString csFilename;

   CSize siMaxButton(m_rcDimensions.Width(), m_rcDimensions.Height());

   CRect rcOldDimension = m_rcDimensions;

   CString csButton;
   if (m_idButtonType == BUTTON_TYPE_SUBMIT)
      csButton = pQuestionnaire->GetSubmitText();
   else if (m_idButtonType == BUTTON_TYPE_DELETE)
      csButton = pQuestionnaire->GetDeleteText();
   else if (m_idButtonType == BUTTON_TYPE_OK)
      csButton = pQuestionnaire->GetOkText();

   CSize siButton;
   pQuestionnaire->CalculateButtonSize(m_idButtonType, siMaxButton);

   if (m_idButtonType == BUTTON_TYPE_OK)
   {
      double dOffX = (rcOldDimension.Width() - siMaxButton.cx) / 2;
      m_rcDimensions.left += dOffX;
      m_rcDimensions.right = m_rcDimensions.left + siMaxButton.cx;
      m_rcDimensions.bottom = m_rcDimensions.top + siMaxButton.cy;
   }
   else
   {
      m_rcDimensions.right = m_rcDimensions.left + siMaxButton.cx;
      m_rcDimensions.bottom = m_rcDimensions.top + siMaxButton.cy;
   }
      
   pQuestionnaire->GetNormalButtonValues(clrFill, clrLine, clrText, lf, csFilename);
   if (pQuestionnaire->ButtonsAreImageButtons())
   {
      hr = CInteractionAreaEx::CreateButtonObjects(&m_aNormalObjects, m_rcDimensions, siMaxButton, 
                                                   csFilename, csButton, clrText, &lf);
   }
   else
   {
      hr = CInteractionAreaEx::CreateButtonObjects(&m_aNormalObjects, m_rcDimensions, 
                                                   clrFill, clrLine, 1, 0, csButton, clrText, &lf);  
   }
   
   pQuestionnaire->GetMouseoverButtonValues(clrFill, clrLine, clrText, lf, csFilename);
   if (pQuestionnaire->ButtonsAreImageButtons())
   {
      hr = CInteractionAreaEx::CreateButtonObjects(&m_aOverObjects, m_rcDimensions, siMaxButton, 
                                                   csFilename, csButton, clrText, &lf);
   }
   else
   {
      hr = CInteractionAreaEx::CreateButtonObjects(&m_aOverObjects, m_rcDimensions, 
                                                   clrFill, clrLine, 1, 0, csButton, clrText, &lf);  
   }
   
   pQuestionnaire->GetPressedButtonValues(clrFill, clrLine, clrText, lf, csFilename);
   if (pQuestionnaire->ButtonsAreImageButtons())
   {
      hr = CInteractionAreaEx::CreateButtonObjects(&m_aPressedObjects, m_rcDimensions, siMaxButton, 
                                                   csFilename, csButton, clrText, &lf);
   }
   else
   {
      hr = CInteractionAreaEx::CreateButtonObjects(&m_aPressedObjects, m_rcDimensions, 
                                                   clrFill, clrLine, 1, 0, csButton, clrText, &lf);  
   }
   
   pQuestionnaire->GetInactiveButtonValues(clrFill, clrLine, clrText, lf, csFilename);
   if (pQuestionnaire->ButtonsAreImageButtons())
   {
      hr = CInteractionAreaEx::CreateButtonObjects(&m_aInactiveObjects, m_rcDimensions, siMaxButton, 
                                                   csFilename, csButton, clrText, &lf);
   }
   else
   {
      hr = CInteractionAreaEx::CreateButtonObjects(&m_aInactiveObjects, m_rcDimensions, 
                                                   clrFill, clrLine, 1, 0, csButton, clrText, &lf);  
   }

   return S_OK;
}

bool CInteractionAreaEx::SetSelected(bool bSelected)
{
    bool bChange = bSelected != m_bSelected;
    m_bSelected = bSelected;
    /*if(bSelected  && IsDemoDocumentObject() 
        && m_pEditorProject && m_pEditorProject->m_pDoc && m_pEditorProject->m_pDoc->GetWhiteboardView()){
        m_pEditorProject->m_pDoc->GetWhiteboardView()->SelectAllOtherDemoModeObjects(this);
    }*/
    return bChange;
}

bool CInteractionAreaEx::SetHiddenDuringEdit(bool bHidden)
{
   bool bChange = m_bHiddenDuringEdit != bHidden;
   m_bHiddenDuringEdit = bHidden;
   return bChange;
}

bool CInteractionAreaEx::SetTimelineDisplay(bool bVisible)
{
   bool bChange = m_bTimelineDisplay != bVisible;
   m_bTimelineDisplay = bVisible;
   return bChange;
}

bool CInteractionAreaEx::SetHiddenDuringPreview(bool bHidden)
{
   bool bChange = m_bHiddenDuringPreview != bHidden;
   m_bHiddenDuringPreview = bHidden;
   return bChange;
}


HRESULT CInteractionAreaEx::FindQuestionId(CEditorProject *pProject)
{
   if (pProject == NULL)
   {
      if (m_pEditorProject != NULL)
         pProject = m_pEditorProject;
      else
      {
         _ASSERT(false); // pDoc should be != NULL
         return E_POINTER;
      }
   }

   CQuestionStream *pQuestionStream = pProject->GetQuestionStream(false);
   if (pQuestionStream != NULL)
   {
      CQuestionEx *pQuestion = pQuestionStream->FindQuestionForTime(GetVisibilityStart());
      if (pQuestion != NULL)
      {
         m_nQuestionJoinId = pQuestion->GetJoinId();
         
         return S_OK;
      }
   }

   return S_FALSE; // TODO is this an error?
}


void CInteractionAreaEx::Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing)
{
   if (!bPreviewing && m_bHiddenDuringEdit || bPreviewing && m_bHiddenDuringPreview)
      return;

   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   graphics.TranslateTransform((float)ptOffsets->x, (float)ptOffsets->y);
   graphics.ScaleTransform((float)pZoomManager->GetZoomFactor(), (float)pZoomManager->GetZoomFactor());
   
   
   Gdiplus::Rect rect(m_rcDimensions.left, m_rcDimensions.top, m_rcDimensions.Width(), m_rcDimensions.Height());
   if (!bPreviewing && !IsButton())
   {
      // normal area in edit mode

      Gdiplus::HatchBrush horizontalBrush(Gdiplus::HatchStyleNarrowHorizontal, 
         Gdiplus::Color(128, 191, 215, 247), Gdiplus::Color(0, 191, 215, 247));
      graphics.FillRectangle(&horizontalBrush, rect);

      Gdiplus::Pen pen(Gdiplus::Color(255, 255, 178, 125), 1.0f);
      // otherwise nothing gets painted:
      pen.SetDashStyle(Gdiplus::DashStyleSolid);
      graphics.DrawRectangle(&pen, rect);
   }
   else
   {
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects = NULL;
   
      if (!m_bActive)
         paObjects = &m_aInactiveObjects;
      else if (m_bMousePressed && m_bMouseOver)
         paObjects = &m_aPressedObjects;
      else if (m_bMouseOver)
         paObjects = &m_aOverObjects;
      else
         paObjects = &m_aNormalObjects;
      
      for (int i=0; i<paObjects->GetSize(); ++i)
      {
         paObjects->GetAt(i)->Draw(pDC->m_hDC, (float)ptOffsets->x, (float)ptOffsets->y, pZoomManager);
      }
   }

   if (!bPreviewing && m_bSelected)
   {
      DrawBorder(pDC, ptOffsets, pZoomManager);
   }
   
}

void CInteractionAreaEx::DrawBorder(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager)
{
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   graphics.TranslateTransform((float)ptOffsets->x, (float)ptOffsets->y);

   Gdiplus::Color clrWhite(255, 255, 255, 255);
   Gdiplus::Color clrBlack(255, 0, 0, 0);
   Gdiplus::RectF rcOut((Gdiplus::REAL)(m_rcDimensions.left * pZoomManager->GetZoomFactor()), 
                        (Gdiplus::REAL)(m_rcDimensions.top * pZoomManager->GetZoomFactor()), 
                        (Gdiplus::REAL)(m_rcDimensions.Width() * pZoomManager->GetZoomFactor()), 
                        (Gdiplus::REAL)(m_rcDimensions.Height() * pZoomManager->GetZoomFactor()));

   Gdiplus::REAL fLineWidth = 6.0;
   Gdiplus::REAL fRadius = 3.0;
   if (m_bIsButton)
   {
      Gdiplus::HatchBrush brush(Gdiplus::HatchStyleLightUpwardDiagonal, clrBlack, clrWhite);
      Gdiplus::Pen selectionRectPen(&brush, fLineWidth);
      rcOut.Inflate((Gdiplus::REAL)fLineWidth / 2, (Gdiplus::REAL)fLineWidth / 2);
      graphics.DrawRectangle(&selectionRectPen, rcOut);
   }
   
   if (!m_bIsImageButton)
   {
      Gdiplus::REAL fPenWidth = 0.5f;
      Gdiplus::Pen blackPen(clrBlack, fPenWidth);
      Gdiplus::SolidBrush whiteBrush(clrWhite);
      
      
      Gdiplus::PointF ptSelected[8];
      ptSelected[0] = Gdiplus::PointF(rcOut.X, rcOut.Y-0.5f);
      ptSelected[1] = Gdiplus::PointF((rcOut.X + rcOut.Width / 2.0f), rcOut.Y-0.5f);
      ptSelected[2] = Gdiplus::PointF(rcOut.GetRight()-0.5f, rcOut.Y-0.5f);
      ptSelected[3] = Gdiplus::PointF(rcOut.GetRight()-0.5f, (rcOut.Y + rcOut.Height / 2.0f));
      ptSelected[4] = Gdiplus::PointF(rcOut.GetRight()-0.5f, rcOut.GetBottom());
      ptSelected[5] = Gdiplus::PointF((rcOut.X + rcOut.Width / 2.0f), rcOut.GetBottom());
      ptSelected[6] = Gdiplus::PointF(rcOut.X-0.5f, rcOut.GetBottom());
      ptSelected[7] = Gdiplus::PointF(rcOut.X-0.5f, (rcOut.Y + rcOut.Height / 2.0f));

      for (int i = 0; i < 8; ++i)
      {
         graphics.FillEllipse(&whiteBrush, 
            (ptSelected[i].X - fRadius), (ptSelected[i].Y - fRadius), 
            2*fRadius, 2*fRadius);
         graphics.DrawEllipse(&blackPen, 
            (ptSelected[i].X - fRadius), (ptSelected[i].Y - fRadius), 
            2*fRadius, 2*fRadius);
      }
   }
}


HRESULT CInteractionAreaEx::TrimTo(UINT nFromMs, UINT nLengthMs)
{
   HRESULT hr = m_Visibility.TrimTo(nFromMs, nLengthMs);
   if (SUCCEEDED(hr))
      m_Activity.TrimTo(nFromMs, nLengthMs);
   return hr;
}

HRESULT CInteractionAreaEx::RemovePart(UINT nFromMs, UINT nLengthMs)
{
   HRESULT hr = m_Visibility.Subtract(nFromMs, nLengthMs);
   if (SUCCEEDED(hr))
      m_Activity.Subtract(nFromMs, nLengthMs);
   return hr;
}

CInteractionAreaEx *CInteractionAreaEx::Split(UINT nSplitMs)
{
   if (!IsSplitPosition(nSplitMs))
      return NULL;

   CInteractionAreaEx *pCopyArea = Copy();
   if (pCopyArea != NULL)
   {
      UINT nNewLength1 = nSplitMs - m_Visibility.GetBegin(true);
      UINT nNewLength2 = m_Visibility.GetLength(true) - nNewLength1;

      HRESULT hr = TrimTo(m_Visibility.GetBegin(true), nNewLength1);
      if (SUCCEEDED(hr))
         hr = pCopyArea->TrimTo(nSplitMs, nNewLength2);
      
      if (FAILED(hr))
      {
         delete pCopyArea;
         pCopyArea = NULL;
      }
   }

   return pCopyArea;
}

bool CInteractionAreaEx::EntirelyWithin(UINT nFromMs, UINT nLengthMs)
{
   return m_Visibility.EntirelyWithin(nFromMs, nLengthMs);
}

bool CInteractionAreaEx::NextTo(UINT nFromMs, UINT nLengthMs, bool bUsePageTimes)
{
   bool bNextTo = m_Visibility.GetBegin(bUsePageTimes) == nFromMs + nLengthMs;
   if (!bNextTo)
      bNextTo = m_Visibility.GetEnd() + 1 == nFromMs;

   return bNextTo;
}

bool CInteractionAreaEx::IsSplitPosition(UINT nPositionMs)
{
   if (GetVisibilityPeriod() == INTERACTION_PERIOD_ALL_PAGES)
      return false;

   return m_Visibility.Contains(nPositionMs, true) && nPositionMs != m_Visibility.GetBegin(true);
}


AreaPeriodId CInteractionAreaEx::GetVisibilityPeriod()
{
   return m_Visibility.GetPeriodId();
}

UINT CInteractionAreaEx::GetVisibilityStart()
{
   return m_Visibility.GetBegin();
}

UINT CInteractionAreaEx::GetVisibilityLength()
{
    return m_Visibility.GetLength();
}

AreaPeriodId CInteractionAreaEx::GetActivityPeriod()
{
   return m_Activity.GetPeriodId();
}

UINT CInteractionAreaEx::GetActivityStart() {
    return m_Activity.GetBegin();
}

UINT CInteractionAreaEx::GetActivityLength(){
    return m_Activity.GetLength();
}

HRESULT CInteractionAreaEx::SetArea(CRect *prcArea)
{
   if (prcArea == NULL)
      return E_POINTER;

   m_rcDimensions = *prcArea;

   return S_OK;
}

HRESULT CInteractionAreaEx::Move(float fDiffX, float fDiffY)
{
   float fCorrectedDiffX = fDiffX + m_fMoveOverhangX;
   float fCorrectedDiffY = fDiffY + m_fMoveOverhangY;
   
   m_rcDimensions.OffsetRect((int)fCorrectedDiffX, (int)fCorrectedDiffY);

   if (IsButton())
   {
      for (int i=0; i<m_aNormalObjects.GetSize(); ++i)
         m_aNormalObjects[i]->Move(fDiffX, fDiffY);

      for (i=0; i<m_aOverObjects.GetSize(); ++i)
         m_aOverObjects[i]->Move(fDiffX, fDiffY);
      
      for (i=0; i<m_aPressedObjects.GetSize(); ++i)
         m_aPressedObjects[i]->Move(fDiffX, fDiffY);
      
      for (i=0; i<m_aInactiveObjects.GetSize(); ++i)
         m_aInactiveObjects[i]->Move(fDiffX, fDiffY);

      //if (m_bDemoDocumentObject && !m_bAutoMove) {
      //    CArray<CInteractionAreaEx*, CInteractionAreaEx*> aRelatedDemoObj;
      //    int iTimestamp = m_Visibility.GetBegin();
      //    HRESULT hr = m_pEditorProject->GetInteractionStream()->FindRelatedDemoObjects(iTimestamp, &aRelatedDemoObj, INTERACTION_CLASS_BUTTON);
      //    if (SUCCEEDED(hr) && aRelatedDemoObj.GetCount() == 1)
      //    {
      //        CInteractionAreaEx* pCopy = aRelatedDemoObj.GetAt(0)->Copy();
      //        pCopy->AutoMove(fDiffX, fDiffY);
      //        m_pEditorProject->GetInteractionStream()->ModifyArea(aRelatedDemoObj.GetAt(0)->GetHashKey(), pCopy, true);
      //        //aRelatedDemoObj.GetAt(0)->AutoMove(fDiffX, fDiffY);
      //    }
      //}
   }
   
   m_fMoveOverhangX = fCorrectedDiffX - (int)fCorrectedDiffX;
   m_fMoveOverhangY = fCorrectedDiffY - (int)fCorrectedDiffY;

   return S_OK;
}

HRESULT CInteractionAreaEx::AutoMove(float fDiffX, float fDiffY) {
    m_bAutoMove = true;
    HRESULT hr = Move(fDiffX, fDiffY);
    m_bAutoMove = false;

    return hr;
}

HRESULT CInteractionAreaEx::AddArea(CInteractionAreaEx *pArea, UINT nToMs, bool bIgnoreHash)
{
   HRESULT hr = S_OK;

   if (pArea == NULL)
      return E_POINTER;

   if (!bIgnoreHash && pArea->GetHashKey() != GetHashKey())
      return E_INVALIDARG;

   // Note: the area to be added has the same hash key. So it must come from
   // the clipboard. In this case the right segment informations should be in that area.

   CTimePeriod *pPeriod = &m_Visibility; // normal areas have no visibility
   CTimePeriod *pPeriodOther = &pArea->m_Visibility;
   
   for (int i=0; i<2 && SUCCEEDED(hr); ++i)
   {
      bool bUsePageTimes = pPeriodOther->GetPeriodId() == INTERACTION_PERIOD_END_PAGE;
      
      UINT nIncludeStart = pPeriodOther->GetBegin(bUsePageTimes) + nToMs;
      UINT nIncludeLength = pPeriodOther->GetLength(bUsePageTimes);

      // Note: this period id might differ from the other one (this might be changed)
      
      hr = MergePeriods(pPeriod, nIncludeStart, nIncludeLength);
      
      pPeriod = &m_Activity; // normal areas have no visibility
      pPeriodOther = &pArea->m_Activity;
   }
   
   return hr;
}

// private
HRESULT CInteractionAreaEx::MergePeriods(CTimePeriod *pPeriodThis, UINT nOtherStart, UINT nOtherLength)
{
   HRESULT hr = S_OK;

   AreaPeriodId idPeriod = pPeriodThis->GetPeriodId();
   
   if (idPeriod == INTERACTION_PERIOD_NEVER)
   {
      // do nothing
      // probably the activity of a feedback
   }
   else if (idPeriod == INTERACTION_PERIOD_ALL_PAGES)
   {
      // do nothing
   }
   else if (idPeriod == INTERACTION_PERIOD_TIME)
   {
      // do nothing: old time stays the same
   }
   else if (idPeriod == INTERACTION_PERIOD_PAGE || idPeriod == INTERACTION_PERIOD_END_PAGE)
   {
      bool bUsePageTimes = idPeriod == INTERACTION_PERIOD_END_PAGE;
      
      bool bNextTo = pPeriodThis->GetBegin(bUsePageTimes) + pPeriodThis->GetLength(bUsePageTimes) == nOtherStart
         || nOtherStart + nOtherLength == pPeriodThis->GetBegin(bUsePageTimes);
      
      if (bNextTo || pPeriodThis->IntersectsRange(nOtherStart, nOtherLength, bUsePageTimes))
      {
         hr = pPeriodThis->EnlargeWith(nOtherStart, nOtherLength);
      }
   }
   else
   {
      // unknown type
      _ASSERT(false);
   }

   return hr;
}

bool CInteractionAreaEx::MeetsWith(CInteractionAreaEx *pOther, UINT nToMs, bool bIgnoreHash)
{
   if (pOther == NULL)
      return false;

   if (!bIgnoreHash && pOther->GetHashKey() != GetHashKey())
      return false;

   UINT nOtherStartMs = pOther->m_Visibility.GetBegin() + nToMs;
   UINT nOtherLengthMs = pOther->m_Visibility.GetLength();

   if (pOther->m_Visibility.GetPeriodId() == INTERACTION_PERIOD_END_PAGE)
   {
      nOtherStartMs = pOther->m_Visibility.GetBegin(true) + nToMs;
      nOtherLengthMs = pOther->m_Visibility.GetEnd() + nToMs - nOtherStartMs + 1;
   }

   return Intersects(nOtherStartMs, nOtherLengthMs, true) || NextTo(nOtherStartMs, nOtherLengthMs, true);
}

bool CInteractionAreaEx::IsRandomTest() {
    ASSERT(m_pEditorProject);
    return m_pEditorProject && m_pEditorProject->FirstQuestionnaireIsRandomTest();
}

HRESULT CInteractionAreaEx::Enlarge(UINT nFromMs, UINT nLengthMs)
{
   HRESULT hr = m_Visibility.IncreaseLength(nLengthMs);
   if (SUCCEEDED(hr))
      hr = m_Activity.IncreaseLength(nLengthMs);
   return hr;
}

HRESULT CInteractionAreaEx::IncrementTimes(UINT nMoveMs)
{
   HRESULT hr = S_OK;
   
   hr = m_Visibility.IncrementBegin(nMoveMs);
   if (SUCCEEDED(hr))
      hr = m_Activity.IncrementBegin(nMoveMs);

   return hr;
}

HRESULT CInteractionAreaEx::DecrementTimes(UINT nMoveMs)
{
   HRESULT hr = S_OK;
   
   hr = m_Visibility.DecrementBegin(nMoveMs);
   if (SUCCEEDED(hr))
      hr = m_Activity.DecrementBegin(nMoveMs);

   return hr;
}

HRESULT CInteractionAreaEx::CorrectActivityUponPaste(UINT nActivityStartMs, UINT nActivityEndMs)
{
   return m_Activity.Init(m_Activity.GetPeriodId(), nActivityStartMs, nActivityEndMs);
}

HRESULT CInteractionAreaEx::CorrectVisibilityUponPaste(UINT nVisibilityStartMs, UINT nVisibilityEndMs)
{
   return m_Visibility.Init(m_Visibility.GetPeriodId(), nVisibilityStartMs, nVisibilityEndMs);
}

HRESULT CInteractionAreaEx::SetActivityPeriod(CTimePeriod *pOther)
{
   if (pOther == NULL)
      return E_POINTER;

   return pOther->CloneTo(&m_Activity);
}

HRESULT CInteractionAreaEx::SetVisibilityPeriod(CTimePeriod *pOther)
{
   if (pOther == NULL)
      return E_POINTER;

   return pOther->CloneTo(&m_Visibility);
}

HRESULT CInteractionAreaEx::Execute(MouseActionId idMouse, CEditorDoc *pDoc)
{
   HRESULT hr = S_OK;

   if (!m_bIsInitialized)
      return E_IAX_UNINITIALIZED;

   switch(idMouse)
   {
   case MOUSE_ACTION_CLICK:
      if (m_pClickAction != NULL)
         hr = m_pClickAction->Execute(pDoc);
      break;

   case MOUSE_ACTION_DOWN:
      if (m_pDownAction != NULL)
         hr = m_pDownAction->Execute(pDoc);
      break;

   case MOUSE_ACTION_UP:
      if (m_pUpAction != NULL)
         hr = m_pUpAction->Execute(pDoc);
      break;

   case MOUSE_ACTION_ENTER:
      if (m_pEnterAction != NULL)
         hr = m_pEnterAction->Execute(pDoc);
      break;

   case MOUSE_ACTION_LEAVE:
      if (m_pLeaveAction != NULL)
         hr = m_pLeaveAction->Execute(pDoc);
      break;

   }

   return hr;
}


UINT CInteractionAreaEx::GetHashKey()
{
   static UINT s_nCounter = 0;

   if (m_nHashKey == 0)
   {
      ++s_nCounter;

      m_nHashKey = s_nCounter * 567; // have an arbitrary but different number
   }

   return m_nHashKey;
}

bool CInteractionAreaEx::ContainsPoint(CPoint *pptMousePos)
{
   if (pptMousePos == NULL)
      return false;

   CRect rcCurrent = GetArea(); // can be moved during preview

   return rcCurrent.PtInRect(*pptMousePos) == TRUE;
}

bool CInteractionAreaEx::LiesInsideRect(CRect *prcOtherRect)
{
   if (prcOtherRect == NULL)
      return false;

   // Code for intersects check
   //bool bOutside = prcOtherRect->bottom < m_rcDimensions.top
   //   || prcOtherRect->left > m_rcDimensions.right
   //   || prcOtherRect->right < m_rcDimensions.left
   //   || prcOtherRect->top > m_rcDimensions.bottom;

   //return !bOutside;

   bool bInside = m_rcDimensions.top >= prcOtherRect->top 
      && m_rcDimensions.left >= prcOtherRect->left
      && m_rcDimensions.right <= prcOtherRect->right
      && m_rcDimensions.bottom <= prcOtherRect->bottom;

   return bInside;
}

// Note: visibility is always equal or longer to activity.

bool CInteractionAreaEx::Intersects(UINT nRangeStartMs, UINT nRangeLengthMs, bool bUsePageTimes)
{
   if (!bUsePageTimes || m_Visibility.GetPeriodId() != INTERACTION_PERIOD_END_PAGE)
   {
      return VisibleIntersects(nRangeStartMs, nRangeLengthMs);
   }
   else
   {
      return m_Visibility.IntersectsRange(nRangeStartMs, nRangeLengthMs, true);
   }
}

bool CInteractionAreaEx::Intersects(CInteractionAreaEx *pOther)
{
   return VisibleIntersects(pOther->m_Visibility.GetBegin(), pOther->m_Visibility.GetLength());
}

bool CInteractionAreaEx::ActiveIntersects(UINT nRangeStartMs, UINT nRangeLengthMs)
{
   if (m_Activity.GetPeriodId() != INTERACTION_PERIOD_NEVER)
      return m_Activity.IntersectsRange(nRangeStartMs, nRangeLengthMs);
   else
      return false;
}

bool CInteractionAreaEx::VisibleIntersects(UINT nRangeStartMs, UINT nRangeLengthMs)
{
   if (m_Visibility.GetPeriodId() != INTERACTION_PERIOD_NEVER)
      return m_Visibility.IntersectsRange(nRangeStartMs, nRangeLengthMs);
   else
      return false;
}



HRESULT CInteractionAreaEx::ToFront()
{
   m_iStackingLevel = s_iUpperStacking;
   ++s_iUpperStacking;

   return S_OK;
}

HRESULT CInteractionAreaEx::ToBack()
{
   m_iStackingLevel = s_iLowerStacking;
   --s_iLowerStacking;

   return S_OK;
}

bool CInteractionAreaEx::InformMouseStatus(bool bMouseOver, bool bMousePressed)
{
   if (m_bDisabledForPreview)
      return false;

   bool bChanged = false;

   if (bMouseOver != m_bMouseOver || bMousePressed != m_bMousePressed)
      bChanged = true;

   m_bMouseOver = bMouseOver;
   m_bMousePressed = bMousePressed;

   return bChanged && IsButton();
}


bool CInteractionAreaEx::Activate(UINT nPositionMs, bool bPreviewing, bool bAlwaysActive)
{
   bool bIsActive = bAlwaysActive;
   if (!m_bDisabledForPreview && bPreviewing && m_Activity.GetPeriodId() != INTERACTION_PERIOD_NEVER)
   {
      if (m_Activity.Contains(nPositionMs))
      {
         bIsActive = true;
      }
   }

   bool bChange = m_bActive != bIsActive;
   
   m_bActive = bIsActive;

   return bChange;
}

bool CInteractionAreaEx::DisableForPreview()
{
   bool bChange = m_bDisabledForPreview != true;
   m_bDisabledForPreview = true;
   return bChange;
}

bool CInteractionAreaEx::Reset(bool bAfterPreview)
{
   if (bAfterPreview)
   {
      bool bChange = m_bDisabledForPreview != false;
      m_bDisabledForPreview = false;
      return bChange;
   }
   else
      return false;
}

HRESULT CInteractionAreaEx::SetInteractionNameOnly(CString &csName)
{
	HRESULT hr = S_OK;

	m_csDisplayName = csName;
	for (int i=0; i<m_aNormalObjects.GetSize(); ++i)
	{
		if (m_aNormalObjects[i]->GetType() == DrawSdk::TEXT)
		{
			((DrawSdk::Text *)m_aNormalObjects[i])->SetString(csName, csName.GetLength());
		}

	}

	for (i=0; i<m_aInactiveObjects.GetSize(); ++i)
	{
		if (m_aInactiveObjects[i]->GetType() == DrawSdk::TEXT)
		{
			((DrawSdk::Text *)m_aInactiveObjects[i])->SetString(csName, csName.GetLength());
		}
	}

	for (i=0; i<m_aOverObjects.GetSize(); ++i)
	{
		if (m_aOverObjects[i]->GetType() == DrawSdk::TEXT)
		{
			((DrawSdk::Text *)m_aOverObjects[i])->SetString(csName, csName.GetLength());
		}
	}

	for (i=0; i<m_aPressedObjects.GetSize(); ++i)
	{
		if (m_aPressedObjects[i]->GetType() == DrawSdk::TEXT)
		{
			((DrawSdk::Text *)m_aPressedObjects[i])->SetString(csName, csName.GetLength());
		}
	}
	return hr;
}

HRESULT CInteractionAreaEx::SetInteractionName(CString &csName, bool bChangeTexts)
{
   HRESULT hr = S_OK;

   m_csDisplayName = csName;


   if (m_bIsButton && bChangeTexts)
   {
      CRect rcNewArea = m_rcDimensions;
	  bool bIsNewAreaWidthChanged = false;
	  bool bIsNewAreaHeightChanged = false;

	  for (int i=0; i<m_aNormalObjects.GetSize(); ++i)
	  {
		  if (m_aNormalObjects[i]->GetType() == DrawSdk::TEXT)
		  {
			  ((DrawSdk::Text *)m_aNormalObjects[i])->SetString(csName, csName.GetLength());

			  CRect rcObject;
			  m_aNormalObjects[i]->GetLogicalBoundingBox(&rcObject);
			  if (m_bDemoDocumentObject && !m_bIsImageButton && rcObject.Width() < GOT_MIN_WIDTH - 2 * BUTTON_OUT_SPACE - 2 * BUTTON_IN_SPACE)
				  rcObject.right = rcObject.left + GOT_MIN_WIDTH - 2 * BUTTON_OUT_SPACE - 2 * BUTTON_IN_SPACE;
			  if (m_bDemoDocumentObject && !m_bIsImageButton && rcObject.Height() < BUTTON_MIN_HEIGHT - 2 * BUTTON_IN_SPACE)
				  rcObject.top = rcObject.bottom - BUTTON_MIN_HEIGHT - 2 * BUTTON_IN_SPACE;
			  if ((m_bDemoDocumentObject && !m_bIsImageButton && (!bIsNewAreaWidthChanged || bIsNewAreaWidthChanged && rcObject.Width() > rcNewArea.Width())) || rcObject.Width() > rcNewArea.Width()){
				  rcNewArea.right = rcNewArea.left + rcObject.Width();
				  bIsNewAreaWidthChanged = true;
			  }
			  if ((m_bDemoDocumentObject && !m_bIsImageButton && (!bIsNewAreaHeightChanged || bIsNewAreaHeightChanged && rcObject.Height() > rcNewArea.Height())) || rcObject.Height() > rcNewArea.Height()){
				  rcNewArea.top = rcNewArea.bottom - rcObject.Height() - 2 * BUTTON_IN_SPACE;
				  bIsNewAreaHeightChanged = true;
			  }
		  }

	  }

	  for (i=0; i<m_aInactiveObjects.GetSize(); ++i)
	  {
		  if (m_aInactiveObjects[i]->GetType() == DrawSdk::TEXT)
		  {
			  ((DrawSdk::Text *)m_aInactiveObjects[i])->SetString(csName, csName.GetLength());

			  CRect rcObject;
			  m_aInactiveObjects[i]->GetLogicalBoundingBox(&rcObject);
			  if (m_bDemoDocumentObject && !m_bIsImageButton && rcObject.Width() < GOT_MIN_WIDTH - 2 * BUTTON_OUT_SPACE - 2 * BUTTON_IN_SPACE)
				  rcObject.right = rcObject.left + GOT_MIN_WIDTH - 2 * BUTTON_OUT_SPACE - 2 * BUTTON_IN_SPACE;
			  if (m_bDemoDocumentObject && !m_bIsImageButton && rcObject.Height() < BUTTON_MIN_HEIGHT - 2 * BUTTON_IN_SPACE)
				  rcObject.top = rcObject.bottom - BUTTON_MIN_HEIGHT - 2 * BUTTON_IN_SPACE;
			  if ((m_bDemoDocumentObject && !m_bIsImageButton && (!bIsNewAreaWidthChanged || bIsNewAreaWidthChanged && rcObject.Width() > rcNewArea.Width())) || rcObject.Width() > rcNewArea.Width()){
				  rcNewArea.right = rcNewArea.left + rcObject.Width();
				  bIsNewAreaWidthChanged = true;
			  }
			  if ((m_bDemoDocumentObject && !m_bIsImageButton && (!bIsNewAreaHeightChanged || bIsNewAreaHeightChanged && rcObject.Height() > rcNewArea.Height())) || rcObject.Height() > rcNewArea.Height()){
				  rcNewArea.top = rcNewArea.bottom - rcObject.Height() - 2 * BUTTON_IN_SPACE;
				  bIsNewAreaHeightChanged = true;
			  }
		  }
	  }

	  for (i=0; i<m_aOverObjects.GetSize(); ++i)
	  {
		  if (m_aOverObjects[i]->GetType() == DrawSdk::TEXT)
		  {
			  ((DrawSdk::Text *)m_aOverObjects[i])->SetString(csName, csName.GetLength());

			  CRect rcObject;
			  m_aOverObjects[i]->GetLogicalBoundingBox(&rcObject);
			  if (m_bDemoDocumentObject && !m_bIsImageButton && rcObject.Width() < GOT_MIN_WIDTH - 2 * BUTTON_OUT_SPACE - 2 * BUTTON_IN_SPACE)
				  rcObject.right = rcObject.left + GOT_MIN_WIDTH - 2 * BUTTON_OUT_SPACE - 2 * BUTTON_IN_SPACE;
			  if (m_bDemoDocumentObject && !m_bIsImageButton && rcObject.Height() < BUTTON_MIN_HEIGHT - 2 * BUTTON_IN_SPACE)
				  rcObject.top = rcObject.bottom - BUTTON_MIN_HEIGHT - 2 * BUTTON_IN_SPACE;
			  if ((m_bDemoDocumentObject && !m_bIsImageButton && (!bIsNewAreaWidthChanged || bIsNewAreaWidthChanged && rcObject.Width() > rcNewArea.Width())) || rcObject.Width() > rcNewArea.Width()){
				  rcNewArea.right = rcNewArea.left + rcObject.Width();
				  bIsNewAreaWidthChanged = true;
			  }
			  if ((m_bDemoDocumentObject && !m_bIsImageButton && (!bIsNewAreaHeightChanged || bIsNewAreaHeightChanged && rcObject.Height() > rcNewArea.Height())) || rcObject.Height() > rcNewArea.Height()){
				  rcNewArea.top = rcNewArea.bottom - rcObject.Height() - 2 * BUTTON_IN_SPACE;
				  bIsNewAreaHeightChanged = true;
			  }
		  }
	  }

	  for (i=0; i<m_aPressedObjects.GetSize(); ++i)
	  {
		  if (m_aPressedObjects[i]->GetType() == DrawSdk::TEXT)
		  {
			  ((DrawSdk::Text *)m_aPressedObjects[i])->SetString(csName, csName.GetLength());

			  CRect rcObject;
			  m_aPressedObjects[i]->GetLogicalBoundingBox(&rcObject);
			  if (m_bDemoDocumentObject && !m_bIsImageButton && rcObject.Width() < GOT_MIN_WIDTH - 2 * BUTTON_OUT_SPACE - 2 * BUTTON_IN_SPACE)
				  rcObject.right = rcObject.left + GOT_MIN_WIDTH - 2 * BUTTON_OUT_SPACE - 2 * BUTTON_IN_SPACE;
			  if (m_bDemoDocumentObject && !m_bIsImageButton && rcObject.Height() < BUTTON_MIN_HEIGHT - 2 * BUTTON_IN_SPACE)
				  rcObject.top = rcObject.bottom - BUTTON_MIN_HEIGHT - 2 * BUTTON_IN_SPACE;
			  if ((m_bDemoDocumentObject && !m_bIsImageButton && (!bIsNewAreaWidthChanged || bIsNewAreaWidthChanged && rcObject.Width() > rcNewArea.Width())) || rcObject.Width() > rcNewArea.Width()){
				  rcNewArea.right = rcNewArea.left + rcObject.Width();
				  bIsNewAreaWidthChanged = true;
			  }
			  if ((m_bDemoDocumentObject && !m_bIsImageButton && (!bIsNewAreaHeightChanged || bIsNewAreaHeightChanged && rcObject.Height() > rcNewArea.Height())) || rcObject.Height() > rcNewArea.Height()){
				  rcNewArea.top = rcNewArea.bottom - rcObject.Height() - 2 * BUTTON_IN_SPACE;
				  bIsNewAreaHeightChanged = true;
			  }
		  }
	  }

      bool bChangeArea = false;
      if (m_rcDimensions.Width() != rcNewArea.Width())
      {
		 //rcNewArea.right += 2 * BUTTON_SPACE;
         m_rcDimensions.right = m_rcDimensions.left + rcNewArea.Width();
         bChangeArea = true;
      }
	  if (m_rcDimensions.Height() != rcNewArea.Height())
      {
		  m_rcDimensions.top = m_rcDimensions.bottom - rcNewArea.Height();
         bChangeArea = true;
      }

      for (i=0; i<m_aNormalObjects.GetSize(); ++i)
      {
         if (/*bChangeArea && */m_aNormalObjects[i]->GetType() == DrawSdk::RECTANGLE)
         {
			 m_aNormalObjects[i]->SetWidth(m_rcDimensions.Width() + 2 * BUTTON_IN_SPACE);
            m_aNormalObjects[i]->SetHeight(m_rcDimensions.Height());
			m_aNormalObjects[i]->SetY(m_rcDimensions.top);
         }
         if (m_aNormalObjects[i]->GetType() == DrawSdk::TEXT)
         {
            double dTextWidth = ((DrawSdk::Text *)m_aNormalObjects[i])->GetWidth();
            double dTextHeight = ((DrawSdk::Text *)m_aNormalObjects[i])->GetHeight();
            LOGFONT logFont;
            ((DrawSdk::Text *)m_aNormalObjects[i])->GetLogFont(&logFont);
            double dAscent = DrawSdk::Text::GetTextAscent(&logFont);
            double dXPos = m_rcDimensions.left + (m_rcDimensions.Width() + 2 * BUTTON_IN_SPACE - dTextWidth) / 2;
            double dYPos = m_rcDimensions.top + (m_rcDimensions.Height() - dTextHeight) / 2 + dAscent + 1.5;
            m_aNormalObjects[i]->SetX(dXPos);
            m_aNormalObjects[i]->SetY(dYPos);
         }
         
      }
      
      for (i=0; i<m_aInactiveObjects.GetSize(); ++i)
      {
         if (/*bChangeArea && */m_aInactiveObjects[i]->GetType() == DrawSdk::RECTANGLE)
         {
            m_aInactiveObjects[i]->SetWidth(rcNewArea.Width() + 2 * BUTTON_IN_SPACE);
            m_aInactiveObjects[i]->SetHeight(rcNewArea.Height());
			m_aInactiveObjects[i]->SetY(m_rcDimensions.top);
         }
         if (m_aInactiveObjects[i]->GetType() == DrawSdk::TEXT)
         {
            double dTextWidth = ((DrawSdk::Text *)m_aInactiveObjects[i])->GetWidth();
            double dTextHeight = ((DrawSdk::Text *)m_aInactiveObjects[i])->GetHeight();
            LOGFONT logFont;
            ((DrawSdk::Text *)m_aInactiveObjects[i])->GetLogFont(&logFont);
            double dAscent = DrawSdk::Text::GetTextAscent(&logFont);
            double dXPos = rcNewArea.left + (rcNewArea.Width() + 2 * BUTTON_IN_SPACE - dTextWidth) / 2;
            double dYPos = rcNewArea.top + (rcNewArea.Height() - dTextHeight) / 2 + dAscent + 1.5;
            m_aInactiveObjects[i]->SetX(dXPos);
            m_aInactiveObjects[i]->SetY(dYPos);
         }
      }
      
      for (i=0; i<m_aOverObjects.GetSize(); ++i)
      {
         if (/*bChangeArea && */m_aOverObjects[i]->GetType() == DrawSdk::RECTANGLE)
         {
            m_aOverObjects[i]->SetWidth(rcNewArea.Width() + 2 * BUTTON_IN_SPACE);
            m_aOverObjects[i]->SetHeight(rcNewArea.Height());
			m_aOverObjects[i]->SetY(m_rcDimensions.top);
         }
         if (m_aOverObjects[i]->GetType() == DrawSdk::TEXT)
         {
            double dTextWidth = ((DrawSdk::Text *)m_aOverObjects[i])->GetWidth();
            double dTextHeight = ((DrawSdk::Text *)m_aOverObjects[i])->GetHeight();
            LOGFONT logFont;
            ((DrawSdk::Text *)m_aOverObjects[i])->GetLogFont(&logFont);
            double dAscent = DrawSdk::Text::GetTextAscent(&logFont);
            double dXPos = rcNewArea.left + (rcNewArea.Width() + 2 * BUTTON_IN_SPACE - dTextWidth) / 2;
            double dYPos = rcNewArea.top + (rcNewArea.Height() - dTextHeight) / 2 + dAscent + 1.5;
            m_aOverObjects[i]->SetX(dXPos);
            m_aOverObjects[i]->SetY(dYPos);
         }
      }
      
      for (i=0; i<m_aPressedObjects.GetSize(); ++i)
      {
         if (/*bChangeArea && */m_aPressedObjects[i]->GetType() == DrawSdk::RECTANGLE)
         {
            m_aPressedObjects[i]->SetWidth(rcNewArea.Width() + 2 * BUTTON_IN_SPACE);
            m_aPressedObjects[i]->SetHeight(rcNewArea.Height());
			m_aPressedObjects[i]->SetY(m_rcDimensions.top);
         }
         if (m_aPressedObjects[i]->GetType() == DrawSdk::TEXT)
         {
            double dTextWidth = ((DrawSdk::Text *)m_aPressedObjects[i])->GetWidth();
            double dTextHeight = ((DrawSdk::Text *)m_aPressedObjects[i])->GetHeight();
            LOGFONT logFont;
            ((DrawSdk::Text *)m_aPressedObjects[i])->GetLogFont(&logFont);
            double dAscent = DrawSdk::Text::GetTextAscent(&logFont);
            double dXPos = rcNewArea.left + (rcNewArea.Width() + 2 * BUTTON_IN_SPACE - dTextWidth) / 2;
            double dYPos = rcNewArea.top + (rcNewArea.Height() - dTextHeight) / 2 + dAscent + 1.5;
            m_aPressedObjects[i]->SetX(dXPos);
            m_aPressedObjects[i]->SetY(dYPos);
         }
      }
   }

   return hr;
}


HRESULT CInteractionAreaEx::Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
                                  CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
                                  LPCTSTR tszExportPrefix, CEditorProject *pProject)
{
   HRESULT hr = S_OK;

   if (!m_bIsInitialized)
      return E_IAX_UNINITIALIZED;

   if (pOutput == NULL)
      return E_POINTER;
 
   // TODO indentation?
   //CArrayOutput outputArray;
   //hr = outputArray.Init(16384, bDoIdent ? 6 : 0);
   // magic number "6": number of spaces in LEP file

   _TCHAR *tszIdent = GetSubClassIdent();
   _TCHAR *tszButtonType = _T("");
   if (IsButton())
   {
      tszIdent = _T("BUTTON");

      if (m_idButtonType == BUTTON_TYPE_SUBMIT)
         tszButtonType = _T(" button-type=\"submit\"");
      else if (m_idButtonType == BUTTON_TYPE_DELETE)
         tszButtonType = _T(" button-type=\"delete\"");
      else if (m_idButtonType == BUTTON_TYPE_OK)
         tszButtonType = _T(" button-type=\"ok\"");
      else
         tszButtonType = _T(" button-type=\"other\"");
   }
   else if (tszIdent == NULL)
      tszIdent = _T("AREA");

   static _TCHAR tszOutput[2000]; // hopefully enough
   tszOutput[0] = 0;

   _stprintf(tszOutput, _T("<%s x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\"%s"),
      tszIdent,
      m_rcDimensions.left, m_rcDimensions.top, m_rcDimensions.Width(), m_rcDimensions.Height(),
      tszButtonType);
   
   if (SUCCEEDED(hr))
      hr = pOutput->Append(tszOutput);

   if (m_bHandCursor && SUCCEEDED(hr))
      hr = pOutput->Append(_T(" cursor=\"link\""));

   if (m_bDemoDocumentObject && SUCCEEDED(hr))
      hr = pOutput->Append(_T(" demoDocumentObject=\"true\""));

   if (m_pClickAction != NULL && SUCCEEDED(hr))
   {
      pOutput->Append(_T(" onClick=\""));
      hr = m_pClickAction->AppendData(pOutput, tszExportPrefix, pProject);
      pOutput->Append(_T("\""));
   }
      
   if (m_pDownAction != NULL && SUCCEEDED(hr))
   {
      pOutput->Append(_T(" onDown=\""));
      hr = m_pDownAction->AppendData(pOutput, tszExportPrefix, pProject);
      pOutput->Append(_T("\""));
   }

   if (m_pUpAction != NULL && SUCCEEDED(hr))
   {
      pOutput->Append(_T(" onUp=\""));
      hr = m_pUpAction->AppendData(pOutput, tszExportPrefix, pProject);
      pOutput->Append(_T("\""));
   }

   if (m_pEnterAction != NULL && SUCCEEDED(hr))
   {
      pOutput->Append(_T(" onEnter=\""));
      hr = m_pEnterAction->AppendData(pOutput, tszExportPrefix, pProject);
      pOutput->Append(_T("\""));
   }

   if (m_pLeaveAction != NULL && SUCCEEDED(hr))
   {
      pOutput->Append(_T(" onLeave=\""));
      hr = m_pLeaveAction->AppendData(pOutput, tszExportPrefix, pProject);
      pOutput->Append(_T("\""));
   }


   if (m_Visibility.GetPeriodId() != INTERACTION_PERIOD_NEVER && SUCCEEDED(hr))
   {
      if (m_Visibility.GetPeriodId() == INTERACTION_PERIOD_ALL_PAGES)
         m_Visibility.SetDocumentLength(pProject->GetAudioPreviewLength());

      pOutput->Append(_T(" visible=\""));
      hr = m_Visibility.AppendTimes(pOutput);
      pOutput->Append(_T("\""));
   }

   if (m_Activity.GetPeriodId() != INTERACTION_PERIOD_NEVER && SUCCEEDED(hr))
   {
      if (m_Activity.GetPeriodId() == INTERACTION_PERIOD_ALL_PAGES)
         m_Activity.SetDocumentLength(pProject->GetAudioPreviewLength());

      pOutput->Append(_T(" active=\""));
      hr = m_Activity.AppendTimes(pOutput);
      pOutput->Append(_T("\""));
   }


   if (SUCCEEDED(hr))
   {
      if (IsButton())
      {
         pOutput->Append(_T(">\n"));
         TCHAR *tszSourcePath = NULL;
         if (pProject != NULL) {
             CString csProjectPath = pProject->GetFilename();
             StringManipulation::GetPath(csProjectPath);
             tszSourcePath = new TCHAR[csProjectPath.GetLength() + 1];
             _tcscpy(tszSourcePath, csProjectPath);
         }

         if (m_aNormalObjects.GetSize() > 0)
            hr = WriteObjectArray(&m_aNormalObjects, pOutput, _T("normal"), bUseFullImagePath, mapImagePaths, tszSourcePath, tszExportPrefix);
         if (m_aOverObjects.GetSize() > 0)
            hr = WriteObjectArray(&m_aOverObjects, pOutput, _T("over"), bUseFullImagePath, mapImagePaths, tszSourcePath, tszExportPrefix);
         if (m_aPressedObjects.GetSize() > 0)
            hr = WriteObjectArray(&m_aPressedObjects, pOutput, _T("pressed"), bUseFullImagePath, mapImagePaths, tszSourcePath, tszExportPrefix);
         if (m_aInactiveObjects.GetSize() > 0)
            hr = WriteObjectArray(&m_aInactiveObjects, pOutput, _T("inactive"), bUseFullImagePath, mapImagePaths, tszSourcePath, tszExportPrefix);
         
         if (tszSourcePath != NULL)
             delete tszSourcePath;
         
         pOutput->Append(_T("</BUTTON>\n"));
      }
      else
      {
         tszOutput[0] = 0;
         hr = AppendSubClassParams(tszOutput);
         pOutput->Append(tszOutput);
         
         pOutput->Append(_T("></"));
         pOutput->Append(tszIdent);
         pOutput->Append(_T(">\n"));
      }
   }

   return hr;
}

// private
HRESULT CInteractionAreaEx::RemoveButtonObjects()
{
   if (m_bIsButton)
   {
      for (int i=0; i<m_aNormalObjects.GetSize(); ++i)
         delete m_aNormalObjects[i];
      for (i=0; i<m_aOverObjects.GetSize(); ++i)
         delete m_aOverObjects[i];
      for (i=0; i<m_aPressedObjects.GetSize(); ++i)
         delete m_aPressedObjects[i];
      for (i=0; i<m_aInactiveObjects.GetSize(); ++i)
         delete m_aInactiveObjects[i];
      
      m_aNormalObjects.RemoveAll();
      m_aOverObjects.RemoveAll();
      m_aPressedObjects.RemoveAll();
      m_aInactiveObjects.RemoveAll();
   }

   return S_OK;
}

// private
HRESULT CInteractionAreaEx::WriteObjectArray(CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects,
                                             CFileOutput *pOutput, LPCTSTR tszIdentifier, bool bUseFullImagePath,
                                             CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
                                             LPCTSTR tszSourcePath, LPCTSTR tszExportPrefix)
{
   HRESULT hr = S_OK;

   if (paObjects == NULL || pOutput == NULL || tszIdentifier == NULL)
      return E_POINTER;

   CString csExportPrefix = tszExportPrefix;

   pOutput->Append(_T("<"));
   pOutput->Append(tszIdentifier);
   pOutput->Append(_T(">\n"));
   
   DrawSdk::CharArray array;
   for (int i=0; i<paObjects->GetSize() && SUCCEEDED(hr); ++i)
   {
      if (tszExportPrefix != NULL && paObjects->GetAt(i)->GetType() == DrawSdk::IMAGE)
      {
         // TODO this side effect on m_objectCounter is very bad style.

         paObjects->GetAt(i)->SetObjectNumber(CWhiteboardStream::m_objectCounter++);
         hr = CWhiteboardStream::CopyImage((DrawSdk::Image *)paObjects->GetAt(i), 
            csExportPrefix, mapImagePaths, tszSourcePath);
         // this might change the image path

         // TODO this is a side effect of export and might lead to problems
         // if you continue using the document.
      }
      
      paObjects->GetAt(i)->Write(&array, bUseFullImagePath);
    
      // TODO no check for returns and indentation at the moment
      pOutput->Append(array.GetBuffer(), array.GetLength());

      array.Reset();
   }
   
   pOutput->Append(_T("</"));
   pOutput->Append(tszIdentifier);
   pOutput->Append(_T(">\n"));
   
   return S_OK;
}

// private
void CInteractionAreaEx::CreateNameAndType()
{
   if (m_bIsButton)
   {
      m_csDisplayType.LoadString(IDS_INTERACTIONBUTTON);
      m_csDisplayName.LoadString(IDS_INTERACTIONBUTTON);
      
      if (m_idButtonType == BUTTON_TYPE_SUBMIT)
         m_csDisplayType.LoadString(IDS_BUTTON_SUBMIT_TYPE);
      else if (m_idButtonType == BUTTON_TYPE_DELETE)
         m_csDisplayType.LoadString(IDS_BUTTON_DELETE_TYPE);
      
      for (int i = 0; i < m_aNormalObjects.GetSize(); ++i) // find first text
      {
         if (m_aNormalObjects[i]->GetType() == DrawSdk::TEXT)
         {
            m_csDisplayName = ((DrawSdk::Text *)m_aNormalObjects[i])->GetString();
            break;
         }
      }
      
   }
   else
   {
      m_csDisplayType.LoadString(IDS_INTERACTIONAREA);
      m_csDisplayName.LoadString(IDS_INTERACTIONAREA);
   }
}

// static
HRESULT CInteractionAreaEx::CreateButtonObjects(
                            CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects,
                            CRect &rcArea, COLORREF clrFill, COLORREF clrLine, 
                            int iLineWidth, int iLineStyle,
                            CString &csButtonText, COLORREF clrText, LOGFONT *pLf)
{
   HRESULT hr = S_OK;

   DrawSdk::Rectangle *pFillRect = new DrawSdk::Rectangle(rcArea.left, rcArea.top, rcArea.Width(), rcArea.Height(), 
                                                          clrFill, clrFill, 1.0, 0);
	paObjects->Add(pFillRect);

   DrawSdk::Rectangle *pOutRect = new DrawSdk::Rectangle(rcArea.left, rcArea.top, rcArea.Width(), rcArea.Height(), 
                                                         clrLine, (double)iLineWidth, iLineStyle);                        
	paObjects->Add(pOutRect);
      

   double dTextWidth = DrawSdk::Text::GetTextWidth(csButtonText, csButtonText.GetLength(), pLf);
   double dTextHeight = DrawSdk::Text::GetTextHeight(csButtonText, csButtonText.GetLength(), pLf);
   double dAscent = DrawSdk::Text::GetTextAscent(pLf);
   double dXPos = rcArea.left + (rcArea.Width() - dTextWidth) / 2;
   double dYPos = rcArea.top + (rcArea.Height() - dTextHeight) / 2 + dAscent + 1.5;
   DrawSdk::Text *pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight,
		                                      clrText, csButtonText, csButtonText.GetLength(), pLf);                
	paObjects->Add(pText);

   return hr;

}

// static
HRESULT CInteractionAreaEx::CreateButtonObjects(
                            CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects,
                            CRect &rcArea, CSize &siImage, CString &csFilename,
                            CString &csButtonText, COLORREF clrText, LOGFONT *pLf)
{
   HRESULT hr = S_OK;

   DrawSdk::Image *pImage = new DrawSdk::Image(rcArea.left, rcArea.top, siImage.cx, siImage.cy, csFilename);              
   paObjects->Add(pImage);

   double dTextWidth = DrawSdk::Text::GetTextWidth(csButtonText, csButtonText.GetLength(), pLf);
   double dTextHeight = DrawSdk::Text::GetTextHeight(csButtonText, csButtonText.GetLength(), pLf);
   double dAscent = DrawSdk::Text::GetTextAscent(pLf);
   if (siImage.cx < dTextWidth)
	   dTextWidth = siImage.cx;
   if (siImage.cy < dTextHeight)
	   dTextHeight = siImage.cy;
   double dXPos = rcArea.left + (rcArea.Width() - dTextWidth) / 2;
   double dYPos = rcArea.top + (rcArea.Height() - dTextHeight) / 2 + dAscent;
   DrawSdk::Text *pText = new DrawSdk::Text(dXPos, dYPos, dTextWidth, dTextHeight,
		                                      clrText, csButtonText, csButtonText.GetLength(), pLf);                
	paObjects->Add(pText);

   return hr;

}

// static
HRESULT CInteractionAreaEx::CalculateButtonSize(CSize &siButton, CString csButtonText, LOGFONT *pLf)
{
   HRESULT hr = S_OK;

   UINT nWidth;
   UINT nHeight;
   DrawSdk::Text::GetTextDimension(csButtonText, csButtonText.GetLength(), pLf, nWidth, nHeight);
   
   siButton.cx = nWidth;
   siButton.cy = nHeight;

   return hr;
}

// static
HRESULT CInteractionAreaEx::CalculateButtonSize(CSize &siButton, CString csFileName)
{
   HRESULT hr = S_OK;

   Gdiplus::Image *pGdipImage;
   
   siButton.cx = 0;
   siButton.cy = 0;

   if (!csFileName.IsEmpty())
   {
      int iStringLength = csFileName.GetLength();
      WCHAR *wcString = (WCHAR *)malloc((iStringLength+1)*sizeof(WCHAR)); // +1 for null termination
#ifdef _UNICODE
      wcscpy(wcString, csFileName);
#else
      MultiByteToWideChar(CP_ACP, 0, csFileName, iStringLength+1, 
         wcString, iStringLength+1);
#endif
      
      pGdipImage = Gdiplus::Image::FromFile(wcString, FALSE);
      
      if (pGdipImage)
      {
         siButton.cx = pGdipImage->GetWidth();
         siButton.cy = pGdipImage->GetHeight();
      }
      
      if (wcString)
         free(wcString);
      
      if (pGdipImage)
         delete pGdipImage;
   }

   return hr;
}

bool CInteractionAreaEx::IsInStringArray(CString cStr, CArray<CString, CString>&acsTarget)
{
   for(int i = 0; i < acsTarget.GetSize(); ++i)
   {
      if(cStr == acsTarget.GetAt(i))
      {
         return true;
      }
   }
   return false;
}

InteractionRecordType CInteractionAreaEx::GetTypeId()
{
	if (GetClassId() == INTERACTION_CLASS_CLICK_OBJECT)
		return INTERACTION_RECORD_CLICK_OBJECTS;
   CString csType;
   csType.LoadString(IDS_QUESTION_TITLE);
   if ( GetInteractionType() == csType ) 
      return INTERACTION_RECORD_TITLE;

   csType.LoadString(IDS_QUESTION_QUESTION);
   if ( GetInteractionType() == csType ) 
      return INTERACTION_RECORD_TEXT;



   UINT nIDSAnswers[] = {IDS_QUESTION_RADIOBUTTON, IDS_QUESTION_CHECKBOX, IDS_QUESTION_TARGET_TITLE, IDS_BLANK_GROUP_TYPE};
   for(int i = 0; i< _countof( nIDSAnswers ); ++i)
   {
      csType.LoadString(nIDSAnswers[i]);
      if ( GetInteractionType() == csType )
         return INTERACTION_RECORD_ANSWERS;
   }

   UINT nIDSButtons[] = {IDS_BUTTON_SUBMIT_TYPE, IDS_BUTTON_DELETE_TYPE};
   for(int i = 0; i< _countof(nIDSButtons); ++i)
   {
      csType.LoadString(nIDSButtons[i]);
      if ( GetInteractionType() == csType )
         return INTERACTION_RECORD_BUTTONS;
   }

   UINT nIDSFeedback[] = {IDS_TYPE_CORRECT, IDS_TYPE_FAILED, IDS_TYPE_PASSED,
      IDS_TYPE_SUMMARY, IDS_TYPE_TIME, IDS_TYPE_TRIES, IDS_TYPE_WRONG};
   for(int i = 0; i< _countof(nIDSFeedback); ++i)
   {
      csType.LoadString(nIDSFeedback[i]);
      if ( GetInteractionType() == csType )
         return INTERACTION_RECORD_FEEDBACK;
   }

   UINT nIDSStatus[] = {IDS_CHANGEABLE_PROGRESS, IDS_CHANGEABLE_TIME, IDS_CHANGEABLE_TRIES,
      IDS_QUESTION_PROGRESS, IDS_QUESTION_TIME, IDS_QUESTION_TRIES};
   for(int i = 0; i< _countof(nIDSStatus); ++i)
   {
      csType.LoadString(nIDSStatus[i]);
      if ( GetInteractionType() == csType )
         return INTERACTION_RECORD_STATUS;
   }

   UINT nIDSElements[] = {IDS_INTERACTIONAREA, IDS_INTERACTIONBUTTON};
   for(int i = 0; i < _countof(nIDSElements); ++i)
   {
      csType.LoadString(nIDSElements[i]);
      if ( GetInteractionType() == csType )
         return INTERACTION_RECORD_ELEMENTS;
   }

   UINT nIDSMisc[] = {IDS_QUESTION_IMAGE, IDS_QUESTION_TARGET_AREA, IDS_DYNAMIC_RADIOBUTTON, IDS_DYNAMIC_CHECKBOX};
   for(int i = 0; i< _countof(nIDSMisc); ++i)
   {
      csType.LoadString(nIDSMisc[i]);
      if ( GetInteractionType() == csType )
         return INTERACTION_RECORD_MISC;
   }

   UINT nIDSGraphicalObjects[] = {ID_GRAPHICALOBJECT_CALLOUT, ID_GRAPHICALOBJECT_TEXT};
   for(int i = 0; i < _countof(nIDSGraphicalObjects); ++i) {
      csType.LoadString(nIDSGraphicalObjects[i]);
      if (GetInteractionType() == csType) {
         return INTERACTION_RECORD_GRAPHIC_OBJECTS;
      }
   }

   ASSERT(false);
   return INTERACTION_RECORD_UNKNOWN;
}