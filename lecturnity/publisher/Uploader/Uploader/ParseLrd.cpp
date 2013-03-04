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

CParseLrd::CParseLrd() {
    csAuthor.Empty();
    csTitle.Empty();
}

CParseLrd::~CParseLrd() {
}


BOOL CParseLrd::Parse(LPCTSTR tstrLrdFile) {
    BOOL bResult = FALSE;

    csAuthor.Empty();
    csTitle.Empty();

    bResult = src.Open(tstrLrdFile, CFile::modeRead | CFile::shareDenyWrite, NULL);
    if (bResult) {
        char *pFileContent;
        DWORD dwFileLength = (DWORD)src.GetLength();
        pFileContent = new char[dwFileLength];
        if (pFileContent != NULL) {
            src.Read(pFileContent, dwFileLength);
            int len = MultiByteToWideChar(CP_UTF8, 0, pFileContent, dwFileLength, NULL, 0);
            LPWSTR lpWStr = new WCHAR[len];
            MultiByteToWideChar(CP_UTF8, 0, pFileContent, dwFileLength, lpWStr, len);

            CString csContent;
            csContent.SetString(lpWStr);

            int iStartPos = -1, iEndPos = -1;
            CString csText1, csText2;

            // search for tile
            csText1 = _T("<TITLE>");
            csText2 = _T("</TITLE>");
            iStartPos = csContent.Find(csText1);
            iEndPos = csContent.Find(csText2);

            if ((iStartPos != -1) && (iEndPos!= -1)) {
                iStartPos += csText1.GetLength();
                csTitle = csContent.Mid(iStartPos, iEndPos - iStartPos);
            }

            // search for author
            csText1 = _T("<AUTHOR>");
            csText2 = _T("</AUTHOR>");
            iStartPos = csContent.Find(csText1);
            iEndPos = csContent.Find(csText2);

            if ((iStartPos != -1) && (iEndPos!= -1)) {
                iStartPos += csText1.GetLength();
                csAuthor = csContent.Mid(iStartPos, iEndPos - iStartPos);
            }

            delete lpWStr;
            delete pFileContent;
        } else {
            bResult = FALSE;
        }
        src.Close();
    }

    return bResult;
}

CString CParseLrd::GetAuthor() {
    return csAuthor;
}

CString CParseLrd::GetTitle() {
    return csTitle;
}
