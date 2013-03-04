// Clipboard.h: Schnittstelle für die Klasse CClipboard.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIPBOARD_H__0C2B8290_D1D3_4D39_9845_88F310EBF357__INCLUDED_)
#define AFX_CLIPBOARD_H__0C2B8290_D1D3_4D39_9845_88F310EBF357__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClipboard  
{
public:
	CClipboard();
	virtual ~CClipboard();

   /**
    * Deletes all stored data. Can be used for example for reusing the
    * CClipboard object in a new project.
    */
   HRESULT Clear();

   /**
    * Adds an arbitrary portion of data to the clipboard.
    * The data is copied so if there are no pointers involved you can delete it
    * afterwards.
    * Data only gets deleted if you overwrite it with the same key.
    */
   HRESULT AddData(BYTE *pData, UINT nDataLength, UINT nClipboardKey);

   /**
    * Retrieve previously stored data.
    * If pData is NULL it only returns the needed/stored byte count.
    */
   HRESULT GetData(BYTE *pData, UINT *pnDataLength, UINT nClipboardKey);

private:
   CMap<UINT, UINT, CByteArray*, CByteArray*> m_mapData;
};

#endif // !defined(AFX_CLIPBOARD_H__0C2B8290_D1D3_4D39_9845_88F310EBF357__INCLUDED_)
