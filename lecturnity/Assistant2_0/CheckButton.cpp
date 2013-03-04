// CheckButton.cpp: Implementierungsdatei
//


#include "stdafx.h"
#ifndef _STUDIO
#include "Assistant.h"
#else
#include "..\Studio\Resource.h"
#endif
#include "CheckButton.h"


// CCheckButton

IMPLEMENT_DYNAMIC(CCheckButton, CXTButton)

CCheckButton::CCheckButton()
{
   m_bChecked = false;

   m_hChecked = NULL;
   m_hUnchecked = NULL;
   m_hMouseOverChecked = NULL;
   m_hMouseOverUnchecked = NULL;
}

CCheckButton::~CCheckButton()
{
   if (m_hChecked)
      AfxDeleteObject((HGDIOBJ*)&m_hChecked);
   m_hChecked = NULL;

   if (m_hUnchecked)
      AfxDeleteObject((HGDIOBJ*)&m_hUnchecked);
   m_hUnchecked = NULL;

   if (m_hMouseOverChecked)
      AfxDeleteObject((HGDIOBJ*)&m_hMouseOverChecked);
   m_hMouseOverChecked = NULL;

   if (m_hMouseOverUnchecked)
      AfxDeleteObject((HGDIOBJ*)&m_hMouseOverUnchecked);
   m_hMouseOverUnchecked = NULL;

}


BEGIN_MESSAGE_MAP(CCheckButton, CXTButton)
   ON_WM_CREATE()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CCheckButton-Meldungshandler



void CCheckButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   HBITMAP hBitmap = 0;// m_hBitmap;

   //if (m_bChecked)
   //{
   //   if (1/*!m_bHilite*/)
   //    //  m_hBitmap = m_hChecked;
   //   else
   //   //   m_hBitmap = m_hMouseOverChecked;
   //}
   //else
   //{
   //   if (1/*!m_bHilite*/)
   //    //  m_hBitmap = m_hUnchecked;
   //   else
   //   //   m_hBitmap = m_hMouseOverUnchecked;
   //}


   CXTButton::DrawItem(lpDIS);

}

int CCheckButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CXTButton::OnCreate(lpCreateStruct) == -1)
      return -1;

	// Get the resource handle for the toolbar bitmap.
	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_CHECKED), RT_BITMAP);
	HRSRC hRsrcChecked = ::FindResource(hInst, MAKEINTRESOURCE(IDB_CHECKED), RT_BITMAP);
	hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_UNCHECKED), RT_BITMAP);
	HRSRC hRsrcUnchecked = ::FindResource(hInst, MAKEINTRESOURCE(IDB_UNCHECKED), RT_BITMAP);
    
	hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_CHECKED_MO), RT_BITMAP);
	HRSRC hRsrcMoChecked = ::FindResource(hInst, MAKEINTRESOURCE(IDB_CHECKED_MO), RT_BITMAP);
	hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_UNCHECKED_MO), RT_BITMAP);
	HRSRC hRsrcMoUnchecked = ::FindResource(hInst, MAKEINTRESOURCE(IDB_UNCHECKED_MO), RT_BITMAP);

	if (!hRsrcChecked || !hRsrcUnchecked) {
		return FALSE;
	}

   
   // Load the normal bitmap.
//	m_hChecked = _xtAfxLoadSysColorBitmap(hInst, hRsrcChecked, FALSE);
	// Load the normal bitmap.
//	m_hUnchecked = _xtAfxLoadSysColorBitmap(hInst, hRsrcUnchecked, FALSE);
    

	// Load the normal bitmap.
//	m_hMouseOverChecked = _xtAfxLoadSysColorBitmap(hInst, hRsrcMoChecked, FALSE);
	// Load the normal bitmap.
//	m_hMouseOverUnchecked = _xtAfxLoadSysColorBitmap(hInst, hRsrcMoUnchecked, FALSE);

	// Return false if the bitmap handle is NULL.
	if (!m_hChecked || !m_hUnchecked)
	{
		TRACE1( "Failed to load Bitmap resource %d.\n", IDB_CHECKED );
		return FALSE;
	}

	// Save the image size.
	m_sizeImage = CSize(16, 16);


   return 0;
}