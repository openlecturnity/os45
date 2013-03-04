// LFont.h: Schnittstelle für die Klasse LFont.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LFONT_H__A0218525_BE15_4CB1_89AD_E2BAFE14AF32__INCLUDED_)
#define AFX_LFONT_H__A0218525_BE15_4CB1_89AD_E2BAFE14AF32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class LFont  
{
public:
	LFont();
	virtual ~LFont();

   static void FillWithDefaultValues(LOGFONT *pLf, LPCTSTR tszFontFamily = NULL);

};

#endif // !defined(AFX_LFONT_H__A0218525_BE15_4CB1_89AD_E2BAFE14AF32__INCLUDED_)
