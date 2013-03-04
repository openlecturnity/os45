// File: DebugLog.cpp
//
// Souce for CDebugLog class which is used to write debug information to a log
// file.
//

#include "stdafx.h"
#include "DebugLog.h"
#include "MfcUtils.h"
#include "SecurityUtils.h"

/* REVIEW UK
 * CString parameters should be used with "&" (eg CString &strParameter) to avoid copying
 *  -> Also for return values (?)
 * Variable names should be more meaningful than just the type ("str", "m_cs")
 * Do not use wchar and tchar (also CString is a "tchar") methods at the same time
    -> fputws vs _T() for example
 * GetCurrentTime() also operates with the thread id and it does not return the time but a string.
    -> Better name?
 * What does this do: (GetFileAttributes(m_csLogFilePath) & INVALID_FILE_ATTRIBUTES) == INVALID_FILE_ATTRIBUTES
    -> Either more clearly formulated (!file.exists() ?) or at least a comment
 */

/// Constructor
CDebugLog::CDebugLog() {
    m_bInit = false;
    m_bLogFile = false;
    m_hLogFile = NULL;
}

/// Destructor
CDebugLog::~CDebugLog() {
	if (m_hLogFile)
		Close();
    if (m_bInit)
        DeleteCriticalSection(&m_cs);
}

/*Initilize variable.
You must call this function once before any other function call.*/
void CDebugLog::Init(CString envVariable) {
    if (m_bInit)
        return;
    m_bInit = true;
    InitializeCriticalSection(&m_cs);
    EnterCriticalSection(&m_cs);

    DWORD dwLength = ::GetEnvironmentVariable((LPCWSTR)(LPCTSTR)envVariable, NULL, 0);       
    m_bLogFile = dwLength > 0;

    if (!m_bLogFile) {
        LeaveCriticalSection(&m_cs);
        return;
    }
    
    if (m_hLogFile == NULL) {
        TCHAR szLogPath[MAX_PATH];
        szLogPath[0] = 0;
        DWORD dwLength = ::GetEnvironmentVariable((LPCWSTR)(LPCTSTR)envVariable, szLogPath, MAX_PATH);
        m_csLogFilePath = szLogPath;

        if ((GetFileAttributes(m_csLogFilePath) & INVALID_FILE_ATTRIBUTES) == INVALID_FILE_ATTRIBUTES) {
            Create();
        }
    }
    LeaveCriticalSection(&m_cs);
}

/* Gets the current time*/
CString CDebugLog::GetCurrentTime() {
    CTime time(CTime::GetCurrentTime());
    CString treadId;
    treadId.Format(_T("  (%d)\t "), GetCurrentThreadId());

    CString csTime;
    csTime.Format(_T("%d-%02d-%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(),
        time.GetHour(), time.GetMinute(), time.GetSecond());

    return csTime + treadId;
    return "";
}

/// Creates the log file (the content of the logger is lost!)
void CDebugLog::Create() {
    if (!m_bLogFile) {
        return;
    }
    m_hLogFile = _tfopen(m_csLogFilePath, _T("w"));
    if (m_hLogFile == NULL) {
        m_bLogFile = false;
        ::MessageBoxA(NULL, "Log file could not be created.", "Error", MB_OK | MB_ICONERROR);
        return;
    }
    LogEntry(_T("------------------------------------------------------------------------------"));
    //CString csVersion; // TODO Get version and write it in log file

    // Add the windows version details
    OSVERSIONINFO versionInfo;
    wchar_t buffer[200];

    versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (GetVersionEx(&versionInfo)) {
        wsprintf( buffer, _T("					OS version: %d.%d  Platform ID: %d "),
            versionInfo.dwMajorVersion,
            versionInfo.dwMinorVersion,
            versionInfo.dwPlatformId );
        LogEntry2( buffer, versionInfo.szCSDVersion );
    } else {
        LogEntry( "Unknown OS version" );
    }

    CString csBetaRelease;
    CString csVersion;
    CSecurityUtils cSec;
    cSec.GetVersionString(csVersion, csBetaRelease);
    //cSec.GetCompactVersionString(csVersion, csBetaRelease);

    CString csBuild;
    MfcUtils::GetPrivateBuild(csBuild);

    LogEntry(_T("                      LECTURNITY Assistant ") + csVersion + _T(" (") + csBuild + _T(")"));
    LogEntry("------------------------------------------------------------------------------");
}

/// Opens the log file for append
void CDebugLog::Open() {
    if (!m_bLogFile) {
        return;
    }
    EnterCriticalSection(&m_cs);
    m_hLogFile = _tfopen(m_csLogFilePath, _T("a"));
    ASSERT(m_hLogFile != NULL);
    LeaveCriticalSection(&m_cs);
}

/// Closes the log file
void CDebugLog::Close() {
    if (!m_bLogFile || m_hLogFile == NULL) {
        return;
    }
    fclose(m_hLogFile);
    m_hLogFile = NULL;
}

/// Writes an entry to the log file.
void CDebugLog::LogEntry(CString str) {
    if (!m_bLogFile) {
        return;
    }
	EnterCriticalSection(&m_cs);
    Open();
    if (m_hLogFile == NULL) {
        LeaveCriticalSection(&m_cs);
        return;
    }
	fputws(GetCurrentTime(), m_hLogFile);
	fputws(str, m_hLogFile);
	fputws(_T("\r"), m_hLogFile);
	fflush(m_hLogFile);
    Close();
	LeaveCriticalSection(&m_cs);
}

/// Writes and entry but does not time stamp
void CDebugLog::LogEntryNoTime(CString str) {
    if (!m_bLogFile) {
        return;
    }
    EnterCriticalSection(&m_cs);
    Open();
    if (m_hLogFile == NULL) {
        LeaveCriticalSection(&m_cs);
        return;
    }
    fputws(str, m_hLogFile);
    fputws(_T("\r"), m_hLogFile);
    fflush(m_hLogFile);
    Close();
    LeaveCriticalSection(&m_cs);
}

/// Writes a two string entry to the log file.
void CDebugLog::LogEntry2(CString str, CString str2) {
    if (!m_bLogFile) {
        return;
    }
    EnterCriticalSection(&m_cs);
    Open();
    if (m_hLogFile == NULL) {
        LeaveCriticalSection(&m_cs);
        return;
    }
    fputws(GetCurrentTime(), m_hLogFile);
    fputws(str, m_hLogFile);
    fputws(str2, m_hLogFile);

    fputws(_T("\r"), m_hLogFile);
    fflush(m_hLogFile);
    Close();
    LeaveCriticalSection(&m_cs);
}

/// Writes a string and integer entry to the log file
void CDebugLog::LogEntryInt(CString  str, int value) {
	if (!m_bLogFile) {
        return;
    }
	wchar_t buffer[128];
	wsprintf( buffer,_T("%d"), value );

	EnterCriticalSection(&m_cs);
    Open();
    if (m_hLogFile == NULL) {
        LeaveCriticalSection(&m_cs);
        return;
    }
	fputws(GetCurrentTime(), m_hLogFile);
	fputws(str, m_hLogFile);
	fputws(buffer, m_hLogFile);

	fputws(_T("\r"), m_hLogFile);
	fflush(m_hLogFile);
    Close();
	LeaveCriticalSection(&m_cs);
}

/// Writes an entry to the log file.
void CDebugLog::Write(CString str) {
    if (!m_bLogFile) {
        return;
    }
	LogEntry(str);
}
