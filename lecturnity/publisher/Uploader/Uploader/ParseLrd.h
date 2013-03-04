// ParseLrd.h: interface for the CParseLrd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSELRD_H__0C287603_6AF4_41B9_9897_CEED91207DDE__INCLUDED_)
#define AFX_PARSELRD_H__0C287603_6AF4_41B9_9897_CEED91207DDE__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

class CParseLrd {
    CString csAuthor, csTitle;
    CFile src;
public:
    CString GetTitle();
    CString GetAuthor();
    BOOL Parse(LPCTSTR tstrLrdFile);
    CParseLrd();
    virtual ~CParseLrd();
};

#endif // !defined(AFX_PARSELRD_H__0C287603_6AF4_41B9_9897_CEED91207DDE__INCLUDED_)
