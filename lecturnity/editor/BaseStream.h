// BaseStream.h: Schnittstelle für die Klasse CBaseStream.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASESTREAM_H__0CDE7054_4B32_45FA_A45D_511224A0D81A__INCLUDED_)
#define AFX_BASESTREAM_H__0CDE7054_4B32_45FA_A45D_511224A0D81A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Clipboard.h"
#include "UndoRedoManager.h"

class CEditorProject;

class CBaseStream : public IUndoCapable
{
public:
	CBaseStream();
	virtual ~CBaseStream();

   virtual HRESULT Init(CEditorProject *pProject, CUndoRedoManager *pUndoManager, UINT nStreamLength = 0xffffffff);
 
   virtual HRESULT Cut(UINT nFromMs, UINT nLengthMs, CClipboard *pClipboard);
   virtual HRESULT Copy(UINT nFromMs, UINT nLengthMs, CClipboard *pClipboard);
   virtual HRESULT Paste(UINT nToMs, UINT nLengthMs, CClipboard *pClipboard);
   virtual HRESULT Delete(UINT nFromMs, UINT nLengthMs);

   // to inform about external changes affecting all streams
   virtual HRESULT EnlargeStreamLength(UINT nAdditionalMs);

      
   // IUndoCapable:
   virtual HRESULT UndoAction(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs = 0, UINT nLengthMs = 0);

   // most streams have plain data with no pointers and do not need to implement this
   virtual HRESULT ReleaseData(BYTE *pData, UINT nDataLength, bool bDeleteCompletely,
      UndoActionId idActionCode, UINT nPositionMs, UINT nLengthMs) { return S_OK; }


protected:
   virtual HRESULT CutCopyOrDelete(UINT nFromMs, UINT nLengthMs, CClipboard *pClipboard, bool bDoCopy, bool bDoDelete) = 0;
   virtual HRESULT InsertRange(UINT nToMs, UINT nLengthMs, BYTE *pData, UINT nByteCount, bool bUndoWanted) = 0;

   // sub classes have to reverse the more special actions
   virtual HRESULT SpecificUndo(UndoActionId idActionCode, 
      BYTE *pData, UINT nDataLength, UINT nPositionMs, UINT nLengthMs) = 0;
 

   CEditorProject *m_pEditorProject;
   CUndoRedoManager *m_pUndoManager;
   UINT m_nStreamLength;
};

#endif // !defined(AFX_BASESTREAM_H__0CDE7054_4B32_45FA_A45D_511224A0D81A__INCLUDED_)
