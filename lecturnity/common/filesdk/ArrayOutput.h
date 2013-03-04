#if !defined(AFX_ARRAYOUTPUT_H__B8B3420A_59E1_4F65_8821_3ADFF71E0FD7__INCLUDED_)
#define AFX_ARRAYOUTPUT_H__B8B3420A_59E1_4F65_8821_3ADFF71E0FD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "export.h"
#include "..\drawsdk\objects.h"

#define E_OUT_UNINITIALIZED _HRESULT_TYPEDEF_(0x80c10001L)

class FILESDK_EXPORT CArrayOutput
{
public:
	CArrayOutput();
	virtual ~CArrayOutput();

   HRESULT Init(UINT nInitialSize = 1024, UINT nIndentLength = 0, bool bDoUtf8 = false);

   virtual HRESULT Append(CString &csAppend);
   /**
    * Automatically checks the string for a newline character "\n" at the end
    * if so the next output is prepended with nIndentLength space characters " ".
    */
   virtual HRESULT Append(LPCTSTR tszAppend, int iLength = -1);
   virtual HRESULT Append(BYTE *aBytes, UINT nByteCount);
   
   /**
     * Uses the internal buffer (free space must be long enough!). Can thus be used by any
     * extended class.
     * 
     * Note: This is (another) source of unsafety: What if a caller of this method (or 
     *       an extended class doesn't make sure the free space is enough in all cases?
     */
   virtual HRESULT Append(DrawSdk::DrawObject *pObject, bool bUseFullImagePath);

   BYTE *GetBuffer() { return m_aBuffer; }
   UINT GetDataLength() { return m_nCurrentCount; }
   UINT GetBufferLength() { return m_nMaximumLength; }
   UINT GetEmptyLength() { return m_nMaximumLength - m_nCurrentCount; }

   HRESULT Reset();

private:
   HRESULT Clear();

   BYTE *m_aBuffer;
   UINT m_nMaximumLength;
   UINT m_nCurrentCount;
   bool m_bDoIndentation;
   _TCHAR *m_tszIndentSpaces;
   UINT m_nIndentLength;
   bool m_bFirstAfterReturn;
   bool m_bDoUtf8;
};

#endif // !defined(AFX_ARRAYOUTPUT_H__B8B3420A_59E1_4F65_8821_3ADFF71E0FD7__INCLUDED_)
