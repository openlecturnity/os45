// TextEdit.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "assistant.h"
#include "TextEdit.h"
#include "AssistantView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextEdit

CTextEdit::CTextEdit()
{
   m_pGroup = NULL;
}

CTextEdit::~CTextEdit()
{
}

BOOL CTextEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
   dwStyle |= ES_NOHIDESEL | ES_LEFT | ES_MULTILINE | ES_WANTRETURN;

	return CRichEditCtrl::Create(dwStyle, rect, pParentWnd, nID);
}

BEGIN_MESSAGE_MAP(CTextEdit, CRichEditCtrl)
	//{{AFX_MSG_MAP(CTextEdit)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTextEdit 


void CTextEdit::OnKillFocus(CWnd* pNewWnd) 
{
   CRichEditCtrl::OnKillFocus(pNewWnd);
}


void CTextEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == 9)   // TAB
      return;

	CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);
   
   if (m_pGroup)
   {
      CString csWindowText;
      GetWindowText(csWindowText);
      m_pGroup->ChangeObjects(csWindowText, true);
   }
}


void CTextEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CTextEdit::PreTranslateMessage(MSG* pMsg) 
{
   if (pMsg->message == WM_KEYDOWN && 
       (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT || 
        pMsg->wParam == VK_NEXT || pMsg->wParam == VK_PRIOR ||
        pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN))
   {
      SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam); 
      return TRUE;
   }

   if (pMsg->message == WM_KEYDOWN && 
       (pMsg->wParam == VK_SPACE || pMsg->wParam == VK_BACK))
   {
      SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam); 
      SendMessage(WM_CHAR, pMsg->wParam, pMsg->lParam); 
      return TRUE;
   }
   
   if (pMsg->message == WM_KEYDOWN && 
       pMsg->wParam == VK_DELETE)
   {
      SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam); 
      return TRUE;
   }

   return CRichEditCtrl::PreTranslateMessage(pMsg);
}

BOOL CTextEdit::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.

   return TRUE; //CRichEditCtrl::OnEraseBkgnd(pDC);
}
