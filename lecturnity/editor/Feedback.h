#if !defined(AFX_FEEDBACK_H__FAD86145_ED56_4517_A116_5DF7C7B43731__INCLUDED_)
#define AFX_FEEDBACK_H__FAD86145_ED56_4517_A116_5DF7C7B43731__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MoveableObjects.h"
#include "Questionnaire.h"

class CEditorDoc;
class CInteractionAreaEx;
class CInteractionStream;
//class CMoveableObjects;
class CTimePeriod;

enum FeedbackTypeId
{
   FEEDBACK_TYPE_NOTHING,
      FEEDBACK_TYPE_Q_CORRECT,
      FEEDBACK_TYPE_Q_WRONG,
      FEEDBACK_TYPE_Q_REPEAT,
      FEEDBACK_TYPE_Q_TRIES,
      FEEDBACK_TYPE_Q_TIME,
      FEEDBACK_TYPE_QQ_PASSED,
      FEEDBACK_TYPE_QQ_FAILED,
      FEEDBACK_TYPE_QQ_EVAL
};

class CFeedback : public CMoveableObjects
{
   DECLARE_DYNCREATE(CFeedback)

public:
	CFeedback();
	virtual ~CFeedback();

   HRESULT Init(CFeedbackDefinition *pDefinition);

   // default method for feedback of questions (normally either inherited or disabled;
   // one of the two parameters must be true)
   HRESULT Init(FeedbackTypeId idType, bool bIsDisabled, bool bIsInherited);

   // either a changed text is defined EXOR objects and a button
   HRESULT Init(FeedbackTypeId idType, bool bIsDisabled, bool bIsInherited, CString &csText,
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *pObjects, CInteractionAreaEx *pOkButton);

   HRESULT SetVisibility(CTimePeriod *pPageTime);

   virtual HRESULT Move(float fDiffX, float fDiffY);

   virtual InteractionClassId GetClassId() { return INTERACTION_CLASS_FEEDBACK; }
   virtual CInteractionAreaEx* MakeNew() { return new CFeedback(); }
   virtual HRESULT ChangeAppearance(CQuestionnaireEx *pQuestionnaire);
   virtual HRESULT RecalculateDimensions(CRect *prcTotal);
   virtual HRESULT ResizeBorderRectangles(CRect rcNewDimension);

   virtual void Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing);

   void SetPassed(bool bPassed) { m_bPassed = bPassed; }

   FeedbackTypeId GetType() { return m_idType; }
   bool IsQuestionnaireOnly();
   bool IsInherited() { return m_bIsInherited; }
   bool IsDisabled() { return m_bIsDisabled; }
   CString& GetText() { return m_csText; }

   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *GetObjects() { return &m_aObjects; }
   CInteractionAreaEx *GetOkButton() { return m_pOkButton; }

   HRESULT ToFront(CInteractionStream *pStream);

   virtual HRESULT SetInteractionName(CString &csName, bool bChangeText);
   virtual bool IsInteractionNameText() { return false; }

   virtual HRESULT Write(CFileOutput *pOutput, UINT nIdentLength, bool bUseFullImagePath, 
      CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, LPCTSTR tszExportPrefix, CEditorProject *pProject);

protected:
   virtual HRESULT CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey);


private:
   void CreateNameAndType();

   FeedbackTypeId m_idType;
   // specifies whether the feedback of a question inherits the definition from the questionnaire
   bool m_bIsInherited;
   bool m_bIsDisabled;
   // for type "inherited" of the feedback of a question it might not be set
   CString m_csText;
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_aObjects;
   CInteractionAreaEx *m_pOkButton;
   UINT m_nInteractionObjectHash;

   // only for display
   bool m_bPassed;
};

#endif // !defined(AFX_FEEDBACK_H__FAD86145_ED56_4517_A116_5DF7C7B43731__INCLUDED_)
