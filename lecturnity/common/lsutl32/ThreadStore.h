// ThreadMgr.h: interface for the ThreadMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADMGR_H__B78048AF_1149_4A8D_AAF5_A22E3291642E__INCLUDED_)
#define AFX_THREADMGR_H__B78048AF_1149_4A8D_AAF5_A22E3291642E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <process.h>
#include "stdafx.h"

class CThreadStore;
class CBaseThread {
public:
    CBaseThread(CThreadStore *, unsigned(__stdcall *thread)(void *), LPVOID data, DWORD dwTimeout, CString name);
    ~CBaseThread();
    UINT		ThreadID() const		{ return m_uiThreadID; }

protected:
    HANDLE		m_hThreadHandle;
    HANDLE      m_hThreadWrapHandle;
    UINT		m_uiThreadID;
    UINT		m_uiThreadWrapID;
    LPVOID      m_threadData;
    DWORD       m_dwTimeOut;
    bool        m_bTimedOut;
    CString     m_mutexName;
    HANDLE      m_hMutex;
    unsigned(__stdcall *m_threadFunc)(void *) ;
    CThreadStore *m_parentStore;
private:
    bool		GetMutex();
    static unsigned __stdcall CBaseThread::Run( LPVOID data );
};


class LSUTL32_EXPORT CThreadStore {
public:
    CThreadStore();
    ~CThreadStore(void);
    bool StartThread(unsigned(__stdcall *thread)(void *), LPVOID data, DWORD dwTimeout, const _TCHAR* name=NULL);
    void AddThreadID(CBaseThread *);
    void RemoveThreadID(CBaseThread *);
    UINT ThreadsRunning();
    void StopNewThreads(bool stopNewThreads = true);
    void AddWarning(CString csWarning);
    CArray<CString, CString> &GetWarnings(){return m_caWarnings;};

protected:
    CList<CBaseThread *>   m_threads;
    // Pointer to an existing array of warnings.
    CArray<CString, CString> m_caWarnings;

    static CRITICAL_SECTION cs;
    bool m_bStopNewThreads;
};

#endif // !defined(AFX_THREADMGR_H__B78048AF_1149_4A8D_AAF5_A22E3291642E__INCLUDED_)
