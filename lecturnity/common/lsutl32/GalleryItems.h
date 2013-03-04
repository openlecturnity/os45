// GalleryItems.h: interface for the CGalleryItemStyleSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GALLERYITEMS_H__04E890F7_BFE8_48DF_9E4A_AE16DBA3BD6A__INCLUDED_)
#define AFX_GALLERYITEMS_H__04E890F7_BFE8_48DF_9E4A_AE16DBA3BD6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Why is it needed to specify this in every header??
#ifdef _LSUTL32_DLL
   #define LSUTL32_EXPORT __declspec(dllexport)
#else 
   #ifdef _LSUTL32_STATIC
      #define LSUTL32_EXPORT 
   #else
      #define LSUTL32_EXPORT __declspec(dllimport)
   #endif
#endif


class LSUTL32_EXPORT CGalleryItemFontFace : public CXTPControlGalleryItem
{
public:
	CGalleryItemFontFace(LPCTSTR lpszCaption);

public:
	static void AddFontItems(CXTPControlGalleryItems* pItems);

public:
	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);
};

#endif // !defined(AFX_GALLERYITEMS_H__04E890F7_BFE8_48DF_9E4A_AE16DBA3BD6A__INCLUDED_)
