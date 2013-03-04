// AnswerEditFB1.cpp : implementation file
//

#include "stdafx.h"
#include "AnswerEditFB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnswerEditFB


CAnswerEditFB::CAnswerEditFB()
{
   m_nTextHeight = 0;
}

CAnswerEditFB::~CAnswerEditFB()
{
   for (int i = 0; i < m_aBlanks.GetSize(); ++i)
   {
      CEmptyField *pSpace = m_aBlanks[i];
      if (pSpace)
         delete pSpace;
   }
   m_aBlanks.RemoveAll();
}


BEGIN_MESSAGE_MAP(CAnswerEditFB, CEdit)
	//{{AFX_MSG_MAP(CAnswerEditFB)
   ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// public functions

void CAnswerEditFB::AddBlank(bool bUpdate)
{	
	int nStart, nEnd;

	GetSel(nStart, nEnd);

   SetSel(nStart, nStart);
   CString csEmpty;
   if (bUpdate)
   {
      for (int i = 0; i < BLANK_WIDTH; ++i)
         csEmpty += " ";
      ReplaceSel(csEmpty);
   }

   CEmptyField *pSpace = new CEmptyField();

   CRect rcBlank;
   CalculateBlankRect(rcBlank, nStart);

   pSpace->Create(NULL, WS_CHILD | WS_VISIBLE, rcBlank, this); 

   pSpace->SetInsertionPos(nStart);

   if (bUpdate)
      UpdateBlanksPosition(nStart, BLANK_WIDTH);

   m_aBlanks.Add(pSpace);
   
}

void CAnswerEditFB::DeleteSelectedBlank()
{
	for (int i = 0; i < m_aBlanks.GetSize(); i++)
	{
		if(m_aBlanks[i]->IsSelected())
		{
	      int nPos = m_aBlanks[i]->GetInsertionPos();
	      SetSel(nPos, nPos + BLANK_WIDTH);
	      ReplaceSel(_T(""));

         CEmptyField *pDeleteItem = m_aBlanks[i];
         m_aBlanks.RemoveAt(i);
         delete pDeleteItem;

         UpdateBlanksPosition(nPos, -BLANK_WIDTH);
			break;
		}
	}

}

bool CAnswerEditFB::GetBlankSelState()
{
	for (int i = 0; i < m_aBlanks.GetSize(); i++)
	{
		if (m_aBlanks[i]->IsSelected())
		{
         return true;
		}
	}

   return false;
}

CEmptyField *CAnswerEditFB::GetSelectedBlank()
{
	for (int i = 0; i < m_aBlanks.GetSize(); i++)
	{
		if(m_aBlanks[i]->IsSelected())
		{
         return m_aBlanks[i];
		}
	}

   return NULL;
}

void CAnswerEditFB::GetTextAndBlanks(CStringArray &aArray)
{
   CString csWindowText;
   GetWindowText(csWindowText);

   CString csText;
   int iLength = csWindowText.GetLength();
   for (int i = 0; i < csWindowText.GetLength(); ++i)
   {
      bool bBlankFound = false;
      if (csWindowText[i] == ' ')
      {
         for (int j = 0; j < m_aBlanks.GetSize(); ++j)
         {
            CEmptyField *pBlank = m_aBlanks[j];
            if(pBlank->GetInsertionPos() == i)
            {
               if (!csText.IsEmpty())
               {
                  aArray.Add(csText);
                  csText.Empty();
               }
               
               aArray.Add(_T(""));
               i += BLANK_WIDTH-1;
               bBlankFound = true;
               break;
            }
         }
      }

      if (!bBlankFound)
         csText += csWindowText[i];
   }

   if (!csText.IsEmpty())
   {
      aArray.Add(csText);
      csText.Empty();
   }
}
   
void CAnswerEditFB::GetBlankAnswers(int nStartPos, CStringArray &aAnswers)
{
   for (int i = 0; i < m_aBlanks.GetSize(); ++i)
   {
      CEmptyField *pBlank = m_aBlanks[i];
      if (pBlank && pBlank->GetInsertionPos() == nStartPos)
      {
         pBlank->GetAnswers(aAnswers);
         return;
      }
   }
}

void CAnswerEditFB::SetTextAndBlanks(CStringArray &aArray, CArray<CStringArray *, CStringArray *> &aCorrectAnswers)
{
   CString csWindowText;

   UINT nCharCount = 0;
   CArray<UINT, UINT> aBlankPositions;
   for (int i = 0; i < aArray.GetSize(); ++i)
   {
      if (aArray[i] == _T(""))
      {
         aBlankPositions.Add(nCharCount);
         CString csEmpty;
         for (int i = 0; i < BLANK_WIDTH; ++i)
            csEmpty += _T(" ");
         csWindowText += csEmpty;
         nCharCount += BLANK_WIDTH;
      }
      else
      {
         csWindowText += aArray[i];
         nCharCount += aArray[i].GetLength();
      }
   }

   SetWindowText(csWindowText);

   for (i = 0; i < aBlankPositions.GetSize(); ++i)
   {
      SetSel(aBlankPositions[i], aBlankPositions[i]+1, FALSE);
      AddBlank(false);
      SetBlankAnswers(aBlankPositions[i], *aCorrectAnswers[i]);
   }

   SetSel(-1, 0);

}
   
void CAnswerEditFB::SetBlankAnswers(int nStartPos, CStringArray &aAnswers)
{
   for (int i = 0; i < m_aBlanks.GetSize(); ++i)
   {
      CEmptyField *pBlank = m_aBlanks[i];
      if (pBlank && pBlank->GetInsertionPos() == nStartPos)
      {
         pBlank->SetAnswers(aAnswers);
         return;
      }
   }
}


/////////////////////////////////////////////////////////////////////////////
// CAnswerEditFB message handlers (protected)

void CAnswerEditFB::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   /*
	for(int i = 0; i <  m_aBlanks.GetSize(); i++)
	{
		if(buf[i] == 1)
		{
			m_aBlanks[i]->SetSelected(true);
			m_bIsBlankSelected = true;
			m_nBlankNo = i;
		}
		else
		{
			m_aBlanks[i]->SetSelected(false);
		}
		buf[i] = 0;
	}
*/
   CEdit::OnLButtonDblClk(nFlags, point);
}

void CAnswerEditFB::OnLButtonDown(UINT nFlags, CPoint point) 
{
   if (!SelectBlank(point))
	   CEdit::OnLButtonDown(nFlags, point);
   
   RedrawWindow();
}

void CAnswerEditFB::OnLButtonUp(UINT nFlags, CPoint point) 
{
   CEdit::OnLButtonUp(nFlags, point);
   
   RedrawWindow();
}

void CAnswerEditFB::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   int iCurrentPos, iEnd;
   GetSel(iCurrentPos, iEnd);

   int iBlankOffset = 0;

   if (nChar == VK_DELETE)
   {
      if (DeleteBlank(iCurrentPos))
      {
         RedrawWindow();
         return;
      }
      iBlankOffset = -1;
   }

   if (nChar == VK_LEFT || nChar == VK_RIGHT)
   {
      if (UpdateCursorPosition(iCurrentPos, nChar))
         return;
   }

   /*/
   if (nChar == VK_RETURN)
   {
      return;
   }
   /*/
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
   
   if (iBlankOffset != 0)
      UpdateBlanksPosition(iCurrentPos, iBlankOffset);

   RedrawWindow();
}

void CAnswerEditFB::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   int iCurrentPos, iEnd;
   GetSel(iCurrentPos, iEnd);

   int nBlankOffset = 1;

   if (nChar == VK_BACK)
   {
      if (DeleteBlank(iCurrentPos-1))
      {
         RedrawWindow();
         return;
      }

      nBlankOffset = -1;
   }
   /*/
   else if (nChar == VK_RETURN || (nChar == 10 && nFlags == 28))  // RETURN and Ctrl-RETURN
   {
      return;
   }
   /*/

	CEdit::OnChar(nChar, nRepCnt, nFlags);
   
   UpdateBlanksPosition(iCurrentPos, nBlankOffset);

   RedrawWindow();
}

/////////////////////////////////////////////////////////////////////////////
// private functions

void CAnswerEditFB::UpdateBlanksPosition(int iCurrentPos, int iOffset)
{
   for (int i = 0; i < m_aBlanks.GetSize(); ++i)
   {
      UINT nInsertionPos = m_aBlanks[i]->GetInsertionPos();
      if (iCurrentPos <= nInsertionPos)
      {
         nInsertionPos += iOffset;
         CRect rcBlank;
         m_aBlanks[i]->SetInsertionPos(nInsertionPos);
         CalculateBlankRect(rcBlank, nInsertionPos);
         m_aBlanks[i]->SetWindowPos(NULL, rcBlank.left, rcBlank.top, 
                                          rcBlank.Width(), rcBlank.Height(), SWP_NOZORDER);
      }
   }
}

bool CAnswerEditFB::UpdateCursorPosition(int iCurrentPos, UINT nDirection)
{
   bool bUpdated = false;

   for (int i = 0; i < m_aBlanks.GetSize(); ++i)
   {
      int iInsertionPos = m_aBlanks[i]->GetInsertionPos();
		if ((iCurrentPos >= iInsertionPos ) && (iCurrentPos <= iInsertionPos + BLANK_WIDTH)) 
      {
         switch (nDirection)
         {
         case VK_LEFT:
            if (iCurrentPos != iInsertionPos)
            {
               SetSel(iInsertionPos, iInsertionPos);
               bUpdated = true;
            }
            break;
         case VK_RIGHT:
            if (iCurrentPos != iInsertionPos + BLANK_WIDTH)
            {
               SetSel(iInsertionPos + BLANK_WIDTH, iInsertionPos + BLANK_WIDTH);
               bUpdated = true;
            }
            break;
         }
         break;
      }  
   }

   return bUpdated;
}

void CAnswerEditFB::CalculateBlankRect(CRect &rcBlank, int iPosition)
{
   CPoint ptBegin = PosFromChar(iPosition);
   CPoint ptEnd = PosFromChar(iPosition + BLANK_WIDTH-1); 

   if (m_nTextHeight == 0)
   {
      CFont *pFont = GetFont();
      LOGFONT lf;
      pFont->GetLogFont(&lf);
      CDC *pDC = GetDC();
      TEXTMETRIC tm;
      pDC->GetTextMetrics(&tm);

      m_nTextHeight = tm.tmAscent;
      ReleaseDC(pDC);
   }

   CString csText;
   GetWindowText(csText);

   if (ptBegin.y != ptEnd.y)
   {
      ptBegin.x = 0;
      ptBegin.y = ptEnd.y;
   }
   rcBlank.SetRect(ptBegin.x, ptBegin.y, ptEnd.x, ptEnd.y + m_nTextHeight);
}

bool CAnswerEditFB::DeleteBlank(int nCurPos)
{
   bool bDeleted = false;
   
   int iStart, iEnd;
   GetSel(iStart, iEnd);
   if (iStart != iEnd)
   {
      for (int i = iStart; i < iEnd; ++i)
      {
         for (int j = 0; j < m_aBlanks.GetSize(); j++)
         {
            UINT nInsertionPos = m_aBlanks[j]->GetInsertionPos();
            if (i == nInsertionPos ) 
            {
               if (nInsertionPos + BLANK_WIDTH > iEnd)
                  iEnd = nInsertionPos + BLANK_WIDTH;
            }
         }
      }

      int iDeletionCount = 0;
      for (i = iStart; i < iEnd; ++i)
      {
         SetSel(i, i);
         if (DeleteBlank(i))
         {
            iDeletionCount += BLANK_WIDTH;
            i += BLANK_WIDTH;
         }
      }

      if (iDeletionCount < iEnd - iStart)
      {
         int iDeletionEnd = iStart + (iEnd - iStart) - (iDeletionCount);
         SetSel(iStart, iDeletionEnd);
         ReplaceSel(_T(""));
         UpdateBlanksPosition(iStart, -(iDeletionEnd - iStart));
      }

      bDeleted = true;

   }
   else
   {
      
      for (int i = 0; i < m_aBlanks.GetSize(); i++)
      {
         UINT nInsertionPos = m_aBlanks[i]->GetInsertionPos();
         if ((nCurPos >= nInsertionPos ) && (nCurPos < nInsertionPos + BLANK_WIDTH)) 
         {
            m_aBlanks[i]->DestroyWindow();
            m_aBlanks.RemoveAt(i);	
            
	         SetSel(nInsertionPos, nInsertionPos + BLANK_WIDTH, FALSE);
	         ReplaceSel(_T(""));

            UpdateBlanksPosition(nInsertionPos, -BLANK_WIDTH);

            bDeleted = true;
            break;
         }
      }
   }
   return bDeleted;
}


void CAnswerEditFB::UnselBlanks()
{
	for(int i = 0; i < m_aBlanks.GetSize(); i++)
   {
		m_aBlanks[i]->SetSelected(false);
	}
}


bool CAnswerEditFB::SelectBlank(CPoint &point)
{
   bool bBlankSelected = false;

   UnselBlanks();

	for(int i = 0; i < m_aBlanks.GetSize(); i++)
   {
      CRect rcBlank;
      m_aBlanks[i]->GetWindowRect(&rcBlank);
      
      ScreenToClient(rcBlank);
      if (rcBlank.PtInRect(point))
      {
		   m_aBlanks[i]->SetSelected(true);
         int iInsertionPos = m_aBlanks[i]->GetInsertionPos();
         SetSel(-1, 0);
         bBlankSelected = true;
         break;
      }
	}
   
   CWnd *pParent = GetParent();
   pParent->SendMessage(WM_SELECTION_CHANGED);

   return bBlankSelected;
}
