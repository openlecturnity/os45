#if !defined(AFX_BLANKGROUP_H__FBC97065_ED56_9547_DD34_5B77C8B43731__INCLUDED_)
#define AFX_BLANKGROUP_H__FBC97065_ED56_9547_DD34_5B77C8B43731__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MoveableObjects.h"

class CEditorDoc;
class CInteractionAreaEx;

class CBlankGroup : public CMoveableObjects
{
   DECLARE_DYNCREATE(CBlankGroup)

public:
	CBlankGroup();
	virtual ~CBlankGroup();

   HRESULT Init(CTimePeriod *pVisibility, CTimePeriod *pActivity,
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects, 
      CArray<CInteractionAreaEx *, CInteractionAreaEx *> *paBlanks, CEditorProject *pProject);
   
   HRESULT Init(DrawSdk::DrawObject *pObject, CInteractionArea *pBlank, CEditorProject *pProject);

   HRESULT AddDuringParsing(DrawSdk::DrawObject *pObject);
   HRESULT AddDuringParsing(CInteractionArea *pBlank, CEditorProject *pProject);

   virtual HRESULT SetArea(CRect *prcArea);
   virtual HRESULT Move(float fDiffX, float fDiffY);

   virtual InteractionClassId GetClassId() { return INTERACTION_CLASS_BLANKS; }
   virtual CInteractionAreaEx* MakeNew() { return new CBlankGroup(); }
 
   virtual void Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing);
   virtual void DrawBorder(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager);

   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *GetDrawObjects() { return &m_aDrawObjects; }
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> *GetBlankObjects() { return &m_aBlankObjects; }

   virtual HRESULT SetInteractionName(CString &csName, bool bChangeText);
   virtual bool IsInteractionNameText() { return false; }

   virtual HRESULT Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
      CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, LPCTSTR tszExportPrefix, CEditorProject *pProject);

   // preview stuff

   virtual bool Reset(bool bAfterPreview);
   HRESULT AddNativesToView();
   bool ShowHide(UINT nPositionMs, bool bPreviewing, bool bForceHide = false);
   bool IsAnswerCorrect();

protected:
   virtual HRESULT CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey);


private:
   void CreateNameAndType();
   void CalculateNewPosition(CRect &rcArea);

private:

   //CEditorDoc *m_pEditorDoc;
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_aDrawObjects;
   CArray<CInteractionAreaEx *, CInteractionAreaEx *> m_aBlankObjects;
};

#endif // !defined(AFX_BLANKGROUP_H__FBC97065_ED56_9547_DD34_5B77C8B43731__INCLUDED_)
