#include "StdAfx.h"
#include "lutils.h"

enum FUNCTION
{
   FUNC_UNKNOWN = -1,
   FUNC_MAX = 1,
   FUNC_MIN,
   FUNC_IF,
   FUNC_MUL,
   FUNC_DIV
};


class LExpression
{
public:
   LExpression(const _TCHAR *tszBuffer, int nStart, int nEnd)
   {
      m_tszExp = tszBuffer;
      m_nStart = nStart;
      m_nEnd = nEnd;
   }

   ~LExpression()
   {
   }

   bool EvaluateInt(int *pResult)
   {
#ifdef _DEBUG
      _TCHAR *tszToEval = new _TCHAR[m_nEnd - m_nStart + 1];
      _tcsncpy(tszToEval, m_tszExp + m_nStart, m_nEnd - m_nStart);
      tszToEval[m_nEnd - m_nStart] = 0;
      _tprintf(_T("EvaluateInt(%s)\n"), tszToEval);
#endif
      int nPos = m_nStart;
      int nResult = 0;

      bool bReady = false;
      bool bHasResult = false;
      bool bNegateNext = false;
      bool bActionDone = false;
      bool bError = false;
      while (!bReady && !bError)
      {
         bActionDone = false;
         bHasResult = false;
         int nCurrent = 0;
         if (IsNumerical(nPos))
         {
            // positive number
            int nEnd = FindIntTokenEnd(nPos);
            nCurrent = GetInt(nPos, nEnd);
            nPos = nEnd;
            bHasResult = true;
            bActionDone = true;
         }
         else if (IsAlpha(nPos))
         {
            int nEnd = -1;
            int nParams = 0;
            FUNCTION func = GetFunction(nPos, &nEnd, &nParams);
            if (func != FUNC_UNKNOWN)
            {
               int *aParams = new int[nParams];
               int nStart = nEnd + 1;
               for (int i=0; i<nParams && !bError; ++i)
               {
                  if (i == nParams -1)
                  {
                     // Last parameter
                     nEnd = FindEndParenthesis(nStart);
                     if (nEnd != -1)
                     {
                        LExpression lExp(m_tszExp, nStart, nEnd - 1);
                        bError = !lExp.EvaluateInt(&aParams[i]);
                        }
                     else
                        bError = true;
                  }
                  else
                  {
                     nEnd = FindComma(nStart);
                     if (nEnd != -1)
                     {
                        LExpression lExp(m_tszExp, nStart, nEnd);
                        bError = !lExp.EvaluateInt(&aParams[i]);
                        nStart = nEnd + 1;
                     }
                     else
                        bError = true;
                  }
               }

               if (!bError)
               {
                  switch (func)
                  {
                  case FUNC_MIN:
                     nCurrent = min(aParams[0], aParams[1]);
                     break;
                  case FUNC_MAX:
                     nCurrent = max(aParams[0], aParams[1]);
                     break;
                  case FUNC_IF:
                     nCurrent = (aParams[0] != 0) ? aParams[1] : aParams[2];
                     break;
                  case FUNC_MUL:
                     nCurrent = aParams[0] * aParams[1];
                     break;
                  case FUNC_DIV:
                     nCurrent = aParams[0] / aParams[1];
                     break;
                  }
                  bActionDone = true;
                  bHasResult = true;
               }

               delete[] aParams;

               nPos = nEnd;
            }
         }
         else if (m_tszExp[nPos] == _T('('))
         {
            int nEnd = FindEndParenthesis(nPos);
            if (nEnd != -1)
            {
               LExpression lExp(m_tszExp, nPos + 1, nEnd - 1);
               bActionDone = lExp.EvaluateInt(&nCurrent);
               bHasResult = bActionDone;
               if (bHasResult)
                  nPos = nEnd + 1;
            }
         }
         else if (m_tszExp[nPos] == _T('-'))
         {
            // negative number
            bNegateNext = true;
            nPos++;
            bActionDone = true;
         }
         else if (m_tszExp[nPos] == _T('+'))
         {
            nPos++;
            bActionDone = true;
         }
         else if (m_tszExp[nPos] == _T(' '))
         {
            nPos++;
            bActionDone = true;
         }

         if (bHasResult)
         {
            if (bNegateNext)
               nResult += -nCurrent;
            else
               nResult += nCurrent;
            bNegateNext = false;
         }

         if (nPos >= m_nEnd)
            bReady = true;
         if (!bActionDone)
            bError = true;
      }

      if (!bError)
      {
         *pResult = nResult;
#ifdef _DEBUG
         _tprintf(_T("%s == %d\n"), tszToEval, nResult);
         if (tszToEval)
         {
            delete[] tszToEval;
            tszToEval = NULL;
         }
#endif
      }

      return !bError;
   }

   int FindIntTokenEnd(int nStart)
   {
      int nEnd = nStart;
      while (IsNumerical(nEnd) && nEnd < m_nEnd)
         nEnd++;
      return nEnd;
   }

   int FindEndParenthesis(int nStart)
   {
      int parCount = 1;
      int nPos = nStart + 1;
      while (parCount > 0 && nPos < m_nEnd)
      {
         if (m_tszExp[nPos] == _T('('))
            parCount++;
         else if (m_tszExp[nPos] == _T(')'))
            parCount--;
         nPos++;
      }

      if (parCount > 0)
         return -1;
      return nPos;
   }

   int FindComma(int nStart)
   {
      int nPos = nStart;
      bool bFinished = false;
      int nParanthesisCount = 0;
      while (!bFinished && nPos < m_nEnd)
      {
         if (m_tszExp[nPos] == _T(',') && nParanthesisCount == 0)
            bFinished = true;
         else
         {
            if (m_tszExp[nPos] == _T('('))
               nParanthesisCount++;
            else if (m_tszExp[nPos] == _T(')'))
               nParanthesisCount--;
            nPos++;
         }
      }
      if (m_tszExp[nPos] == _T(',') && bFinished)
         return nPos;
      else
         return -1;
   }

   int GetInt(int nStart, int nEnd)
   {
      _TCHAR tszTmp[16];
      _tcsncpy(tszTmp, m_tszExp+nStart, nEnd - nStart);
      tszTmp[nEnd - nStart] = 0;
      return _ttoi(tszTmp);
   }

   FUNCTION GetFunction(int nStart, int *pEnd, int *pParams)
   {
      int nPos = nStart;
      while (IsAlpha(nPos) && nPos < m_nEnd)
         nPos++;
      FUNCTION func = FUNC_UNKNOWN;
      int nParams = 0;
      if (_tcsnicmp(_T("min"), m_tszExp + nStart, nPos-nStart) == 0)
      {
         func = FUNC_MIN;
         nParams = 2;
      }
      else if (_tcsnicmp(_T("max"), m_tszExp + nStart, nPos-nStart) == 0)
      {
         func = FUNC_MAX;
         nParams = 2;
      }
      else if (_tcsnicmp(_T("if"), m_tszExp + nStart, nPos-nStart) == 0)
      {
         func = FUNC_IF;
         nParams = 3;
      }
      else if (_tcsnicmp(_T("mul"), m_tszExp + nStart, nPos-nStart) == 0)
      {
         func = FUNC_MUL;
         nParams = 2;
      }
      else if (_tcsnicmp(_T("div"), m_tszExp + nStart, nPos-nStart) == 0)
      {
         func = FUNC_DIV;
         nParams = 2;
      }

      if (func != FUNC_UNKNOWN)
      {
         *pEnd = nPos;
         *pParams = nParams;
      }

      return func;
   }

private:
   const _TCHAR *m_tszExp;
   int m_nStart;
   int m_nEnd;

   bool IsNumerical(int nPos)
   {
      return (m_tszExp[nPos] >= 48 && m_tszExp[nPos] < 58); 
   }

   bool IsAlpha(int nPos)
   {
      _TCHAR c = m_tszExp[nPos];
      return ((c >= 65 && c <= 90) || // A-Z
              (c >= 97 && c <= 122)); // a-z
   }
};


LExpressionParser::LExpressionParser(_TCHAR *tszExp)
{
   m_tszExpression = NULL;

   if (tszExp == NULL)
      return;

   int iLength = _tcslen(tszExp);

   m_tszExpression = new _TCHAR[iLength + 1];
   _tcsncpy(m_tszExpression, tszExp, iLength);
   m_tszExpression[iLength] = 0;
}

LExpressionParser::~LExpressionParser()
{
   if (m_tszExpression != NULL)
      delete[] m_tszExpression;
}

bool LExpressionParser::EvaluateInt(int *pResult)
{
   if (!pResult)
      return false;

   if (m_tszExpression == NULL)
      return false;
   
   LExpression lExp(m_tszExpression, 0, _tcslen(m_tszExpression));
   int nResult = 0;
   bool success = lExp.EvaluateInt(&nResult);
   *pResult = nResult;
   return success;
}
