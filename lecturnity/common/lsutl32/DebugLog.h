// File: DebugLog.h
//
// Souce for DebugLog class which is used to write debug information to a log
// file.
//
#ifndef __DEBUG_LOG_H_INCLUDED
#define __DEBUG_LOG_H_INCLUDED

#include "stdafx.h"

/** 
 * Used to write data to a log file. 
 * File will be specified by an environment variable in the Init(...) function.
 *
 * <code>
 *    CDebugLog debugLog;
 *    debugLog.Init(_T("IMC_LEC_LC_LOG"));
 *    debugLog.LogEntry(_T("StopNewThreads"));
 * </code>
*/
class LSUTL32_EXPORT CDebugLog
{
public:

    /// Constructor
    CDebugLog();

    /// Destructor
    ~CDebugLog();

public: // Methods

    /// Writes an entry to the log file.
    void LogEntry(CString str);

    /// Writes and entry but does not time stamp
    void LogEntryNoTime(CString str);

    /// Writes a two string entry to the log file.
    void LogEntry2(CString str, CString str2 );

    /// Writes a string and integer entry to the log file
    void LogEntryInt(CString str, int value );

    /*Initilize variable.
    You must call this function once before any other function call.*/
    void CDebugLog::Init(CString envVariable);

    /// Writes an entry to the log file.
    void Write(CString str);

private: // Methods

    /// Creates the log file
    void Create();

    /// Gets the current time
    CString GetCurrentTime();

    /// Opens the log file
    void Open();

    /// Closes the log file
    void Close();

private: // Data Members

    // Handle of the Logging file
    FILE*   m_hLogFile;
    // The file path of the logger, including file name
    CString m_csLogFilePath;
    // Flag to store if local variables are initialized
    bool	m_bInit;
    // Flag to set if the logging is enabled. If true, will log entries to file. 
    // It will be set to false, if no environment variable with valid path was provided.
    bool	m_bLogFile;
    // Critical section that will not allow multiple threads to write to the log file, but syncronized.
    CRITICAL_SECTION m_cs;
};

#endif // __DEBUG_LOG_H_INCLUDED