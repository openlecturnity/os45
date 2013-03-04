#include "stdafx.h"
#include "editor.h"
#include "ExtEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CExtEdit::CExtEdit()
{
   m_pLastFont = NULL;

   m_clrText = RGB(0, 0, 0);
   m_clrBack = RGB(255, 255, 255);
   m_brshBkActive.CreateSolidBrush(m_clrBack);
   
}

CExtEdit::~CExtEdit()
{
   if (m_pLastFont != NULL)
      delete m_pLastFont;
}


void CExtEdit::SetExtFont(CFont *pFont)
{
   if (m_pLastFont != NULL)
      delete m_pLastFont;

   CEdit::SetFont(pFont);

   m_pLastFont = pFont;
}


BEGIN_MESSAGE_MAP(CExtEdit, CEdit)
	//{{AFX_MSG_MAP(CExtEdit)
   ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CExtEdit::PreTranslateMessage(MSG* pMsg) 
{
    if (IsWindowVisible()) {
        if (pMsg->hwnd == GetSafeHwnd()) {
            // Make sure that the keystrokes continue to the appropriate handlers
            if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) {
                ::TranslateMessage(pMsg);
                ::DispatchMessage(pMsg);
                return TRUE;
            }

            // Catch the Alt key so we don't choke if focus is going to an owner drawn button
            if (pMsg->message == WM_SYSCHAR)
                return TRUE;
        }
    }

    return CEdit::PreTranslateMessage(pMsg);
}

void CExtEdit::SetBackColor(COLORREF clrBack)
{
   m_clrBack = clrBack;
   m_brshBkActive.CreateSolidBrush(m_clrBack);
}

HBRUSH CExtEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
   if (IsWindowEnabled())
   {
      pDC->SetTextColor(m_clrText);
      pDC->SetBkColor(m_clrBack);
      return (HBRUSH)m_brshBkActive;
   }
   else
   {
      // defer to super class
      return NULL;
   }
}

