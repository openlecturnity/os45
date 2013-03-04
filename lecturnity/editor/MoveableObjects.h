#if !defined(AFX_MOVEABLEOBJECT_H__84187645_989A_4172_8424_82368BBA38BA__INCLUDED_)
#define AFX_MOVEABLEOBJECT_H__84187645_989A_4172_8424_82368BBA38BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileOutput.h" // FileSdk
#include "InteractionAreaEx.h"

enum MoveableTypeId
{
   MOVEABLE_TYPE_NONE,
      MOVEABLE_TYPE_SOURCE,
      MOVEABLE_TYPE_TARGET,
      MOVEABLE_TYPE_OTHER
};

class CMoveableObjects : public CInteractionAreaEx  
{
   DECLARE_DYNCREATE(CMoveableObjects)

public:
	CMoveableObjects();
	virtual ~CMoveableObjects();

   /**
    * You can specify an additional target area. If specified the size here should only reflect
    * all the normal objects. in "paObjects".
    */
   HRESULT Init(CRect *prcSize, CTimePeriod *pVisibility, CEditorProject *pProject,
      CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects,
      MoveableTypeId idMoveableType, UINT nBelongsKey = 0);
 
   /** Only use for parsing (translation from filesdk). */
   HRESULT Init(DrawSdk::DrawObject *pObject, CEditorProject *pProject);

   virtual InteractionClassId GetClassId();

   virtual CRect& GetArea();

   MoveableTypeId GetMoveableType() { return m_idMoveableType; }
   bool IsMoveableSource() { return m_idMoveableType == MOVEABLE_TYPE_SOURCE; }
   bool IsMoveableTarget() { return m_idMoveableType == MOVEABLE_TYPE_TARGET; }
   UINT GetBelongsKey() { return m_nBelongsKey; }

   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *GetObjects() { return &m_aObjects; }

   virtual CInteractionAreaEx* MakeNew();
   virtual void Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing);
   virtual void DrawBorder(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager);
   
   virtual HRESULT Move(float fDiffX, float fDiffY);
  
   /**
    * Actually changes the real position (Move() is called); this is important for
    * other object tracking methods to still work after moving.
    * However you can and should reset the move when preview is finished with ResetPreviewOnlyMove().
    */
   HRESULT MoveForPreviewOnly(
      float fDiffX, float fDiffY, CArray<CPoint, CPoint> *paAllTargets, int iTargetX, int iTargetY);
   
   virtual bool Reset(bool bAfterPreview);

   bool IsAnswerCorrect() { return m_bCorrectSnapping; }
 
   virtual HRESULT SetInteractionName(CString &csName, bool bChangeText);
   virtual bool IsInteractionNameText();

   virtual HRESULT Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
      CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, LPCTSTR tszExportPrefix, CEditorProject *pProject);
      
protected:
   virtual HRESULT CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey);

   HRESULT CalculateDimensions(CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> *paObjects, CRect *prcTotal);

private:
   void CreateNameAndType();

   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_aObjects;
   MoveableTypeId m_idMoveableType;
   UINT m_nBelongsKey; 
   
   // only for preview
   float m_fMoveForPreviewOnlyX;
   float m_fMoveForPreviewOnlyY;
   float m_fSnapInMoveX;
   float m_fSnapInMoveY;
   bool  m_bCorrectSnapping;

};

#endif // !defined(AFX_MOVEABLEOBJECT_H__84187645_989A_4172_8424_82368BBA38BA__INCLUDED_)
