//////////////////////////////////////////////////////////////////////
//
// CThreadStore.cpp: implementation of the CBaseThread, CThreadStore class.
//
//////////////////////////////////////////////////////////////////////

//
// Usage:
// Declare a CThreadStore object cs nd then call :
// 
// cs.StartThread(func, data, timeout)
//
// cs will manage the thread by starting another wrapper thread first and then running
// the func in that thread. When the func thread exits (or timesout) the wrapper thread
// will detect this and update the threadstore.
//
// By calling cs.ThreadsRunning() we can see how many threads are still running
// 

#include "stdafx.h"
#include "process.h"
#include "ThreadStore.h"

//////////////////////////////////////////////////////////////////////
// CBase Thread
//////////////////////////////////////////////////////////////////////
CBaseThread::CBaseThread(CThreadStore *store, unsigned(__stdcall *thread)(void *), LPVOID data, DWORD dwTimeout, CString mutex_name) {
    m_dwTimeOut = dwTimeout;
    m_bTimedOut = false;
    m_threadData = data;
    m_threadFunc = thread;
    m_parentStore = store;
    m_mutexName = mutex_name;
    m_hMutex = NULL;
    m_hThreadWrapHandle = HANDLE(_beginthreadex(NULL, 0, CBaseThread::Run, this, 0/*run*/, &m_uiThreadWrapID));
    CloseHandle(m_hThreadWrapHandle);
}

bool CBaseThread::GetMutex() {
    HANDLE hMutex = NULL;
    if (m_mutexName.IsEmpty())  {
        return true;
    }
    m_hMutex = ::CreateMutex( NULL, FALSE, m_mutexName);
    if (!m_hMutex) {
        // Serious error can't create the mutex return
        return false;
    }
    DWORD dwWaitResult = WaitForSingleObject( 
        m_hMutex,   // handle to mutex
        m_dwTimeOut);   // Wait /*just 50ms*/ m_dwTimeOut for the mutex - another tread might /*just*/ be finishing
    switch (dwWaitResult) {
        // The thread got mutex ownership.
        case WAIT_OBJECT_0:
            return true;
            break; 

        // Cannot get mutex ownership due to time-out so we just return
        // Some other thread has it
        case WAIT_TIMEOUT:
            m_parentStore->AddWarning(m_mutexName);
            return false;
        // Got ownership of the abandoned mutex object this means the process/thread that 
        // had the mutex did not release it should not happen
        case WAIT_ABANDONED: 
            m_parentStore->AddWarning(m_mutexName);
            return false; 
        case WAIT_FAILED:
            m_parentStore->AddWarning(m_mutexName);
            return false;
    }
    return true;
}

unsigned __stdcall CBaseThread::Run(LPVOID data) {
    CBaseThread *t = (CBaseThread *)data;
    if (t->GetMutex()) { // If we don't get the mutex just bomb out
        t->m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, t->m_threadFunc, t->m_threadData, 0/*run*/, &t->m_uiThreadID);
        if (t->m_hThreadHandle) { 
            // Log in the thread store
            (t->m_parentStore)->AddThreadID(t);	
            t->m_bTimedOut = (WaitForSingleObject(t->m_hThreadHandle, t->m_dwTimeOut) == WAIT_TIMEOUT);
            // When it has finished remove from the thread store
            t->m_parentStore->RemoveThreadID(t);
            CloseHandle(t->m_hThreadHandle);
            // OK we are done so we can exit too
        }
        if (t->m_hMutex) ReleaseMutex(t->m_hMutex); 
    }
    delete t; // Free up memory for CBaseThread
    _endthreadex(0);
    return 0;
}

CBaseThread::~CBaseThread() {
}

CRITICAL_SECTION CThreadStore::cs;

///////Thread Store keeps track of threads.
CThreadStore::CThreadStore() {
    m_bStopNewThreads = false;
    ::InitializeCriticalSection(&cs);
}

CThreadStore::~CThreadStore(void) {
    DeleteCriticalSection(&cs);
}

bool CThreadStore::StartThread(unsigned(__stdcall *thread)(void *), LPVOID data, DWORD dwTimeout, const _TCHAR *name) {
    if (m_bStopNewThreads == false) {
        CBaseThread *bt = new CBaseThread(this, thread, data, dwTimeout, name);
        return true;
    } else {
        return false;
    }
}

void CThreadStore::AddThreadID(CBaseThread * threadId) {
    ::EnterCriticalSection(&cs);
    m_threads.AddTail(threadId);
    ::LeaveCriticalSection(&cs);
}

void CThreadStore::RemoveThreadID(CBaseThread * threadId) {
    ::EnterCriticalSection(&cs);
    m_threads.RemoveAt(m_threads.Find(threadId));
    ::LeaveCriticalSection(&cs);
}

UINT CThreadStore::ThreadsRunning() {
    UINT i;
    ::EnterCriticalSection(&cs);
    i = (UINT) m_threads.GetCount();
    ::LeaveCriticalSection(&cs);
    return (i);
}

void CThreadStore::StopNewThreads(bool stopNewThreads) {
    m_bStopNewThreads = stopNewThreads;
}

void CThreadStore::AddWarning(CString csWarning) {
    ::EnterCriticalSection(&cs);
    m_caWarnings.Add(csWarning);
    ::LeaveCriticalSection(&cs);
}