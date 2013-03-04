#if !defined(AFX_DYNAMICOBJECT_H__62B5065C_15B0_4282_B3E3_62FF40EF610B__INCLUDED_)
#define AFX_DYNAMICOBJECT_H__62B5065C_15B0_4282_B3E3_62FF40EF610B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractionAreaEx.h"
#include "WhiteboardView.h"

enum DynamicTypeId
{
   DYNAMIC_NONE,
      DYNAMIC_RADIOBUTTON,
      DYNAMIC_CHECKBOX,
      DYNAMIC_TEXTFIELD
};

class CDynamicObject : public CInteractionAreaEx
{
   DECLARE_DYNCREATE(CDynamicObject)

public:
	CDynamicObject();
	virtual ~CDynamicObject();
   
   // one and only one of the "correct answer" parameters must be set
   HRESULT Init(CRect *prcSize, CTimePeriod *pVisibility, CTimePeriod *pActivity, CEditorProject *pProject, 
      DynamicTypeId idType, bool bHasToBeChecked, CArray<CString, CString> *paCorrectTexts);

   HRESULT Init(CInteractionArea *pInteraction, CEditorProject *pProject);

   virtual HRESULT Move(float fDiffX, float fDiffY);
      
   virtual InteractionClassId GetClassId();

   HRESULT AddNativeToView();
   /** Returns true if the visibility was changed. */
   bool ShowHide(UINT nPositionMs, bool bPreviewing, bool bForceHide = false);
   virtual bool InformMouseStatus(bool bMouseOver, bool bMousePressed);
   virtual bool Reset(bool bAfterPreview);
   bool ResetRadio(); // TODO change: used by CInteractionStream to reset button radio states

   virtual CInteractionAreaEx* MakeNew();
   virtual void Draw(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager, bool bPreviewing);
   virtual void DrawBorder(CDC *pDC, CPoint *ptOffsets, DrawSdk::ZoomManager *pZoomManager);
   
   DynamicTypeId GetType() { return m_idType; }
   bool GetHasToBeChecked() { return m_bHasToBeChecked; }
   CArray<CString, CString> *GetCorrectTexts() { return &m_aCorrectTexts; }

   bool IsAnswerCorrect();
   
   virtual _TCHAR* GetSubClassIdent();
   virtual HRESULT AppendSubClassParams(_TCHAR *tszOutput);
      
   virtual HRESULT SetInteractionName(CString &csName, bool bChangeText);
   virtual bool IsInteractionNameText() { return false; }

   virtual HRESULT Write(CFileOutput *pOutput, bool bDoIdent, bool bUseFullImagePath, 
      CMap<CString, LPCTSTR, CString, CString&> &mapImagePaths, LPCTSTR tszExportPrefix, CEditorProject *pProject);

protected:
   virtual HRESULT CloneTo(CInteractionAreaEx *pTarget, bool bCopyHashKey);

private:
   void CreateNameAndType();

   DynamicTypeId m_idType;
   bool m_bHasToBeChecked;
   CArray<CString, CString> m_aCorrectTexts;
   
   bool m_bIsChecked;

   // objects for the native component
   CEditorProject *m_pEditorProject;
   CWhiteboardView *m_pWbView;
   CEdit *m_pEdit;
};

#endif // !defined(AFX_DYNAMICOBJECT_H__62B5065C_15B0_4282_B3E3_62FF40EF610B__INCLUDED_)
