#pragma once

#include "InteractionAreaEx.h"
#include "editorDoc.h"

class CClickObject : public CInteractionAreaEx {
    DECLARE_DYNCREATE(CClickObject)
public:
    CClickObject(void);
    virtual ~CClickObject(void);
    virtual void Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing);
    HRESULT Init(CInteractionArea *pInteraction, CEditorProject *pProject);
    HRESULT Init(CLcElementInfo *pLcElementInfo, CEditorProject *pProject);
    virtual InteractionClassId GetClassId() { return INTERACTION_CLASS_CLICK_OBJECT; }
    virtual CInteractionAreaEx* MakeNew() { return new CClickObject(); }
    virtual bool IsInteractionNameText() { return false; }
   /* virtual HRESULT Move(float fDiffX, float fDiffY);
    virtual HRESULT SetArea(CRect *prcArea);*/
    HRESULT Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
        CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, 
        LPCTSTR tszExportPrefix, CEditorProject *pProject);

    CLcElementInfo* GetLcElementInfo(){return m_pLcElementInfo;};
private:
    CEditorProject *m_pEditorProject;
    CLcElementInfo* m_pLcElementInfo;
protected:
    virtual HRESULT CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey);
};
