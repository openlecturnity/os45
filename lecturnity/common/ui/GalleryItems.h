// GalleryItems.h: interface for the CGalleryItemStyleSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GALLERYITEMS_H__04E890F7_BFE8_48DF_9E4A_AE16DBA3BD6A__INCLUDED_)
#define AFX_GALLERYITEMS_H__04E890F7_BFE8_48DF_9E4A_AE16DBA3BD6A__INCLUDED_

#pragma message("Do not use anymore: These files are now in lsutl32.")
#pragma message("Incomplete compile break statement...

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGalleryItemFontFace : public CXTPControlGalleryItem
{
public:
	CGalleryItemFontFace(LPCTSTR lpszCaption);

public:
	static void AddFontItems(CXTPControlGalleryItems* pItems);

public:
	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);
};

#endif // !defined(AFX_GALLERYITEMS_H__04E890F7_BFE8_48DF_9E4A_AE16DBA3BD6A__INCLUDED_)
