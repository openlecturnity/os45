/////////////////////////////////////////////////////////////////////////////
// SingleInstanceApp.cpp
//

#include "stdafx.h"
#include "SingleInstanceApp.h"

#ifndef ASSERT
#include <cassert>
#define ASSERT(f) assert(f)
#endif // ASSERT

#ifndef VERIFY
#ifdef _DEBUG
#define VERIFY(f) ASSERT(f)
#else
#define VERIFY(f) (static_cast<void>(f))
#endif
#endif // VERIFY


/////////////////////////////////////////////////////////////////////////////
// SingleInstanceApp implementation

SingleInstanceApp::SingleInstanceApp(LPCTSTR pszUniqueString, UINT& uMessage)
{
	ASSERT(pszUniqueString);
	VERIFY(uMessage = ::RegisterWindowMessage(pszUniqueString));

	VERIFY(_hMapping = ::CreateFileMapping(
		INVALID_HANDLE_VALUE,
		0,
		PAGE_READWRITE,
		0,
		sizeof(*_pSharedData),
		pszUniqueString));
	
	// ::GetLastError returns ERROR_ALREADY_EXISTS for the second instance
	_bAmITheFirst = S_OK == ::GetLastError();

	VERIFY(_pSharedData = 
		static_cast<SharedData*>(::MapViewOfFile(_hMapping, FILE_MAP_WRITE, 0, 0, 0)));

	if (_bAmITheFirst)
	{
		// set the thread ID of the first instance to the shared mapping
		VERIFY(_pSharedData->dwThreadID = ::GetCurrentThreadId());
		// set empty command line
		_pSharedData->pszCmdLine[0] = 0; 
	}
	else
	{
		// copy command line to shared mapping
		::lstrcpyn(_pSharedData->pszCmdLine, ::GetCommandLine(), 
			sizeof(_pSharedData->pszCmdLine) / sizeof(*_pSharedData->pszCmdLine));
		// post the notification message to the first instance
		::PostThreadMessage(_pSharedData->dwThreadID, uMessage, 0, 0);
	}
}

/////////////////////////////////////////////////////////////////////////////

SingleInstanceApp::~SingleInstanceApp()
{
	::UnmapViewOfFile(_pSharedData);
	::CloseHandle(_hMapping);
}

/////////////////////////////////////////////////////////////////////////////

bool SingleInstanceApp::AmITheFirst() const
{
	return _bAmITheFirst;
}

/////////////////////////////////////////////////////////////////////////////

LPCTSTR SingleInstanceApp::GetSecondInstanceCmdLine() const
{
	return _pSharedData->pszCmdLine;
}

/////////////////////////////////////////////////////////////////////////////

#ifdef __AFXWIN_H__

/////////////////////////////////////////////////////////////////////////////
// CSingleInstanceApp implementation

BEGIN_MESSAGE_MAP(CSingleInstanceApp, CWinApp)
	ON_REGISTERED_THREAD_MESSAGE(sm_uMessage, OnSecondInstance)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

UINT CSingleInstanceApp::sm_uMessage;

CSingleInstanceApp::CSingleInstanceApp(LPCTSTR pszUniqueString)
	:	SingleInstanceApp(pszUniqueString, sm_uMessage)
{
}

/////////////////////////////////////////////////////////////////////////////

void CSingleInstanceApp::OnSecondInstance(UINT, LPARAM)
{
	TRACE(TEXT("---- Second instance was tried to launch.\n"));
	TRACE(TEXT("---- Command line is \"%s\".\n"), GetSecondInstanceCmdLine());
	TRACE(TEXT("---- Override CSingleInstanceApp::OnSecondInstance to do something.\n"));
}

/////////////////////////////////////////////////////////////////////////////

#endif // __AFXWIN_H__
