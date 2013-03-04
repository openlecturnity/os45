#if !defined(AFX_INTERACTIONREADERWRITER_H__1476B471_F5ED_4F34_8754_A314DF7884E0__INCLUDED_)
#define AFX_INTERACTIONREADERWRITER_H__1476B471_F5ED_4F34_8754_A314DF7884E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\drawsdk\objects.h"
#include "SgmlParser.h"
#include "InteractionArea.h"

class CWhiteboardStream;

class FILESDK_EXPORT CInteractionReaderWriter  
{
public:
	CInteractionReaderWriter();
	virtual ~CInteractionReaderWriter();

   bool IsInteractionObject(LPCTSTR tszIdentifier);
   HRESULT Parse(SgmlElement *pElement, CString& csRecordPath);

   HRESULT AddSupportObject(DrawSdk::DrawObject *pObject);

   /** 
    * Only use if runtime access is needed (ie video export).
    * Normally (ie the Editor) you call Extract() and handle the objects elsewhere.
    */
   CInteractionArea *GetObject(UINT nIdx);

   HRESULT Extract(CInteractionArea **paInteractions);
   UINT GetCount();
   HRESULT Extract(CArray<CInteractionArea *, CInteractionArea *> *paInteractions);
   
   /** Extracts all CInteractionArea that belong to a question. */
   HRESULT ExtractQuestionInteractives(CArray<CInteractionArea *, CInteractionArea *> *paInteractives);

private:
   CArray<CInteractionArea *, CInteractionArea *> m_aInteractions;

};

#endif // !defined(AFX_INTERACTIONREADERWRITER_H__1476B471_F5ED_4F34_8754_A314DF7884E0__INCLUDED_)
