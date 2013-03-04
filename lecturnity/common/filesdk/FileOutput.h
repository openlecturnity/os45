#if !defined(AFX_FILEOUTPUT_H__5666B019_6EE8_41C9_97D8_84CFC2B01500__INCLUDED_)
#define AFX_FILEOUTPUT_H__5666B019_6EE8_41C9_97D8_84CFC2B01500__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArrayOutput.h"

#define E_OUT_WRITE_FAILED _HRESULT_TYPEDEF_(0x80c20001L)

class FILESDK_EXPORT CFileOutput : public CArrayOutput  
{
public:
	CFileOutput();
	virtual ~CFileOutput();

    HRESULT OpenForWrite(CString csFilename, bool bDoUtf8 = false);
   HRESULT Init(HANDLE hFile, UINT nIndentLength = 0, bool bDoUtf8 = false);

   virtual HRESULT Append(LPCTSTR tszAppend, int iLength = -1) { return CArrayOutput::Append(tszAppend, iLength); }
   virtual HRESULT Append(CString &csAppend) { return CArrayOutput::Append(csAppend); }
   virtual HRESULT Append(BYTE *aBytes, UINT nByteCount);

   /**
    * Does a Flush() before calling the base class Append().
    */
   virtual HRESULT Append(DrawSdk::DrawObject *pObject, bool bUseFullImagePath);

   HRESULT Flush();

private:

   HANDLE m_hFile;
   bool m_bFileOpened;
   bool m_bDoUtf8Bom;
};

#endif // !defined(AFX_FILEOUTPUT_H__5666B019_6EE8_41C9_97D8_84CFC2B01500__INCLUDED_)
