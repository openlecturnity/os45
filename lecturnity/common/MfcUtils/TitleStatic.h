// TitleStatic.h: Schnittstelle für die Klasse CTitleStatic.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TITLESTATIC_H__93482CD4_7538_4925_B11D_903D0E38EBB7__INCLUDED_)
#define AFX_TITLESTATIC_H__93482CD4_7538_4925_B11D_903D0E38EBB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTitleStatic : public CStatic
{
public:
	CTitleStatic();
	virtual ~CTitleStatic();

protected:
   afx_msg void OnPaint();

   DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_TITLESTATIC_H__93482CD4_7538_4925_B11D_903D0E38EBB7__INCLUDED_)
