// EmptyField.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "EmptyField.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEmptyField

CEmptyField::CEmptyField()
{
   m_bSelected = false;
}

CEmptyField::~CEmptyField()
{
}


BEGIN_MESSAGE_MAP(CEmptyField, CStatic)
	//{{AFX_MSG_MAP(CEmptyField)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// public functions

void CEmptyField::UpdateAnswers(CStringArray &aAnswers)
{
   if (m_aPossibleAnswers.GetSize() > 0)
      m_aPossibleAnswers.RemoveAll();

   for (int i = 0; i < aAnswers.GetSize(); ++i)
      m_aPossibleAnswers.Add(aAnswers[i]);
}

void CEmptyField::GetAnswers(CStringArray &aAnswers)
{
   if (aAnswers.GetSize() > 0)
      aAnswers.RemoveAll();

   for (int i = 0; i < m_aPossibleAnswers.GetSize(); ++i)
      aAnswers.Add(m_aPossibleAnswers[i]);
}


void CEmptyField::SetAnswers(CStringArray &aAnswers)
{
   if (m_aPossibleAnswers.GetSize() > 0)
      m_aPossibleAnswers.RemoveAll();

   for (int i = 0; i < aAnswers.GetSize(); ++i)
      m_aPossibleAnswers.Add(aAnswers[i]);
}

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEmptyField 

void CEmptyField::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
   CRect rcClient;
   GetClientRect(&rcClient);

   rcClient.left += 3;

   CBrush *pBrush;
   if (m_bSelected)
   {
      pBrush = new CBrush(RGB(255, 0, 0)); //::GetSysColor(COLOR_HIGHLIGHT));
   }
   else
   {
      pBrush = new CBrush(RGB(255, 255, 255));
   }

   CBrush *pOldBrush = dc.SelectObject(pBrush);

   CPen borderPen(PS_SOLID, RGB(0, 0, 0), 1);
   CPen *pOldPen = dc.SelectObject(&borderPen);

   dc.Rectangle(rcClient);

   dc.SelectObject(pOldPen);
   dc.SelectObject(pOldBrush);

   delete pBrush;
}
