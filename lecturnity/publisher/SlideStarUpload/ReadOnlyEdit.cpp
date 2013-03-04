#include "stdafx.h"
#include "ReadOnlyEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyEdit

CReadOnlyEdit::CReadOnlyEdit()
{
   //default text color
   m_crText = RGB(0,0,0);
}

CReadOnlyEdit::~CReadOnlyEdit()
{
   //delete brush
   if(m_brBackGnd.GetSafeHandle())
   {
      m_brBackGnd.DeleteObject();
   }
}


BEGIN_MESSAGE_MAP(CReadOnlyEdit, CEdit)
   //{{AFX_MSG_MAP(CReadOnlyEdit)
   ON_WM_CTLCOLOR_REFLECT()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyEdit message handlers



HBRUSH CReadOnlyEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
   
   
   // TODO: Return a non-NULL brush if the parent's handler should not be called
   
   //set text color
   pDC->SetTextColor(m_crText);
   //set the text's background color
   pDC->SetBkColor(m_crBackGnd);

   //return the brush used for background this sets control background
   return m_brBackGnd;
}


void CReadOnlyEdit::SetBackColor(COLORREF rgb)
{
   //set background color ref (used for text's background)
   m_crBackGnd = rgb;
   
   //free brush
   if (m_brBackGnd.GetSafeHandle())
   {
      m_brBackGnd.DeleteObject();
   }
   //set brush to new color
   m_brBackGnd.CreateSolidBrush(rgb);
   
   //redraw
   Invalidate(TRUE);
}


void CReadOnlyEdit::SetTextColor(COLORREF rgb)
{
   //set text color ref
   m_crText = rgb;

   //redraw
   Invalidate(TRUE);
}
