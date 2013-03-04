// ImageStatic.h: Schnittstelle für die Klasse CImageStatic.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGESTATIC_H__AFA891DB_A6A2_4232_A930_EB17D58BB718__INCLUDED_)
#define AFX_IMAGESTATIC_H__AFA891DB_A6A2_4232_A930_EB17D58BB718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace DrawSdk
{
   class Image;
}

class CImageStatic : public CStatic
{
public:
	CImageStatic();
	virtual ~CImageStatic();

   virtual void ReadImage(const _TCHAR *tszFileName);
   virtual int GetImageWidth() { return m_origWidth; }
   virtual int GetImageHeight() { return m_origHeight; }
   virtual void SetDrawImageSize(bool bDrawSize=true) { m_bDrawImageSize = bDrawSize; }

protected:
   afx_msg void OnPaint();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnDestroy();

   DECLARE_MESSAGE_MAP()

   DrawSdk::Image *m_pImage;
   bool            m_bHasCalcedSize;
   bool            m_bDrawImageSize;
   int             m_origWidth;
   int             m_origHeight;

   CFont           m_font;
};

#endif // !defined(AFX_IMAGESTATIC_H__AFA891DB_A6A2_4232_A930_EB17D58BB718__INCLUDED_)
