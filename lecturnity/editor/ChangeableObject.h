#if !defined(AFX_CHANGEABLEOBJECT_H__920901A1_B440_4515_AF53_87DBBDC75BE9__INCLUDED_)
#define AFX_CHANGEABLEOBJECT_H__920901A1_B440_4515_AF53_87DBBDC75BE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tools.h" // DrawSdk
#include "FileOutput.h"
#include "InteractionAreaEx.h"

class CChangeableObject : public CInteractionAreaEx  
{
   DECLARE_DYNCREATE(CChangeableObject)

public:
	CChangeableObject();
	virtual ~CChangeableObject();

   HRESULT Init(CRect *prcSize, CTimePeriod *pVisibility, CEditorProject *pProject, DrawSdk::DrawObject *pObject);
   HRESULT Init(DrawSdk::DrawObject *pObject, CEditorProject *pProject);

   virtual InteractionClassId GetClassId();

   DrawSdk::DrawObject *GetObject() { return m_pObject; }

   virtual HRESULT Move(float fDiffX, float fDiffY);
  
   virtual CInteractionAreaEx* MakeNew();
   virtual void Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing);
   virtual void DrawBorder(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager);
    
   virtual HRESULT SetInteractionName(CString &csName, bool bChangeText);
   virtual bool IsInteractionNameText() { return true; }

   virtual HRESULT Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
      CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, LPCTSTR tszExportPrefix, CEditorProject *pProject);

protected:
   virtual HRESULT CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey);

private:
   void CreateNameAndType();

   DrawSdk::DrawObject *m_pObject;
};

#endif // !defined(AFX_CHANGEABLEOBJECT_H__920901A1_B440_4515_AF53_87DBBDC75BE9__INCLUDED_)
