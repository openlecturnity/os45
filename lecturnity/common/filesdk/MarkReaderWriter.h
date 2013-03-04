#if !defined(AFX_MARKSTREAM_H__BEB1D35B_E49C_40E5_866B_B285D776F62A__INCLUDED_)
#define AFX_MARKSTREAM_H__BEB1D35B_E49C_40E5_866B_B285D776F62A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SgmlParser.h"


#define MAX_MARK_LABEL_LENGTH 200

#define E_SJM_UNINITIALIZED _HRESULT_TYPEDEF_(0x80fc0001L)
#define E_SJM_INVALID_DATA  _HRESULT_TYPEDEF_(0x80fc0002L)



class FILESDK_EXPORT CStopJumpMark  
{
public:
	CStopJumpMark();
	virtual ~CStopJumpMark();

   UINT GetId() { return m_nId; }

   HRESULT Init(bool bIsJumpMark, LPCTSTR tszLabel, UINT nPositionMs, UINT nId = 0, bool bIsDemoDocumentObject = false);
   HRESULT CloneTo(CStopJumpMark* pMark, bool bCreateNewId = false);

   bool IsJumpMark() { return m_bIsJumpMark; }
   bool IsStopMark() { return !m_bIsJumpMark; }
   bool IsDemoDocumentObject();

   UINT GetPosition() { return m_nPositionMs; }

   // normally the nMaxValue is not specified
   // however during moving you will want to have it bounded by the stream length
   HRESULT IncrementPosition(UINT nDiffMs, UINT nMaxValue = 0xffffffff);
   HRESULT DecrementPosition(UINT nDiffMs);

   LPCTSTR GetLabel() { return m_tszLabel; }
   HRESULT SetLabel(LPCTSTR tszLabel);
   void SetDemoDocumentObject(bool bIsDemoDocumentObject);

   HRESULT RegisterUser();
   HRESULT UnregisterUser();

   bool IsUsed() { return m_nUseCounter > 0; }


private:
   UINT CreateId(UINT nIdSeed);

   bool m_bIsJumpMark;
   UINT m_nPositionMs;
   _TCHAR m_tszLabel[MAX_MARK_LABEL_LENGTH + 1];

   UINT m_nId;
   bool m_bIsInitialized;
   // specifies if stop mark is a demo document object.
   bool m_bIsDemoDocumentObject;

   // this number is coupled with the id:
   // if that id is new its a new mark and the counter is reset
   UINT m_nUseCounter;
};


class FILESDK_EXPORT CMarkReaderWriter  
{
public:
	CMarkReaderWriter();
	virtual ~CMarkReaderWriter();

   HRESULT Parse(SgmlElement *pElement);
   HRESULT Extract(CStopJumpMark *aMarks);
   int GetCount();

private:
   CArray<CStopJumpMark *, CStopJumpMark *> m_aAllMarks;

};


#endif // !defined(AFX_MARKSTREAM_H__BEB1D35B_E49C_40E5_866B_B285D776F62A__INCLUDED_)
