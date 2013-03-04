// ParseLrd.cpp: implementation of the CParseLrd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ParseLrd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParseLrd::CParseLrd()
{
   csAuthor.Empty();
   csTitle.Empty();
}

CParseLrd::~CParseLrd()
{
}


BOOL CParseLrd::Parse(LPCTSTR tstrLrdFile)
{
   BOOL bResult = FALSE;
   int iLength = 0, iGarbage = 0;
   char *pszBeg = NULL;
   char *pszEnd = NULL;
   char *pszLine = NULL;
   DWORD dwFileLength = 0;

   csAuthor.Empty();
   csTitle.Empty();
   
   bResult = src.Open(tstrLrdFile, CFile::modeRead | CFile::shareDenyWrite, NULL);
   if(bResult)
   {
      dwFileLength = src.GetLength();
      pszLine = new char[dwFileLength];
      if(pszLine != NULL)
      {
         src.Read((void *)pszLine, dwFileLength);

         // search for tile
         pszBeg = strstr(pszLine, "<TITLE>");
         pszEnd = strstr(pszLine, "</TITLE>");
         if((pszBeg != NULL) && (pszEnd != NULL))
         {
            iGarbage = strlen("<TITLE>");
            iLength = pszEnd - pszBeg - iGarbage;
            char *szTitle = new char[iLength+1];
            strncpy(szTitle, pszBeg + iGarbage, iLength);
            szTitle[iLength] = '\0';
            csTitle = CString(szTitle);
         }

         // search for author
         pszBeg = strstr(pszLine, "<AUTHOR>");
         pszEnd = strstr(pszLine, "</AUTHOR>");
         if((pszBeg != NULL) && (pszEnd != NULL))
         {
            iGarbage = strlen("<AUTHOR>");
            iLength = pszEnd - pszBeg - iGarbage;
            char *szAuthor = new char[iLength+1];
            strncpy(szAuthor, pszBeg + iGarbage, iLength);
            szAuthor[iLength] = '\0';
            csAuthor = CString(szAuthor);
         }

         delete pszLine;
      }
      else
      {
         bResult = FALSE;
      }
      src.Close();
   }
   
   return bResult;
}

CString CParseLrd::GetAuthor()
{
   return csAuthor;
}

CString CParseLrd::GetTitle()
{
   return csTitle;
}
