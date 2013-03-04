#pragma once

// avoid unncessary includes:
namespace LIo {
    class MessagePipe;
};
#include "DebugLog.h"

#include "LiveContextElementInfo.h"

class LSUTL32_EXPORT CLiveContextCommunicator : public CObject {
public:
    static CLiveContextCommunicator* GetInstance();

	virtual ~CLiveContextCommunicator();

    /**
     * Establishes the communication with LIVECONTEXT (sending SYN and waiting for ACK).
     * 
     * Return 
     *  - S_OK if communication is established
     *  - E_NOT_CONNECTED if no communication is established
     */
    HRESULT Init(CString& sPipename);

    /**
     * Requests the information for the specified Gui element.
     *
     * The parameter pInfo is used for input and output: for input only the click/mouse
     * location must be specified.
     * The output with the same object is then done with 
     * ILiveContextInfoListener::ReportElementInfo() once the requested information is present.
     *
     * Return 
     *  - S_OK if communication is established
     *  - E_POINTER if pInfo is NULL
     *  - E_NOT_CONNECTED if no communication is established
     */
    HRESULT RequestElementInfo(CLiveContextElementInfo *pInfo);

    /**
     * Reads the reply from the LIVECONTEXT pipe.
     *
     * The parameter csMessage is used for output.
     *
     * Return 
     *  - S_OK if communication is established
     *  - E_POINTER if pInfo is NULL
     *  - E_NOT_CONNECTED if no communication is established
     */
    HRESULT GetMessageString(char *szMessage, int iBufferSize, DWORD &dwRead);
    
    HRESULT PeekElement(char *szMessage, int iBufferSize, DWORD &dwRead);

    /**
     * Sends an appropriate finish message to LIVECONTEXT.
     * Such a message can have warnings and it can have errors. Both arrays may also be empty though.
     *
     * Return
     *  - S_OK if message was send successfully
     *  - Any of E_NOT_CONNECTED, E_UNEXPECTED, E_POINTER or E_FAIL if message was not send.
     */
    HRESULT SendProgramExit(int iExitCode, CArray<CString, CString> *paWarnings, CArray<CString, CString> *paErrors);

    /**
     * Returns true if communication is established with named pipe server. 
     * Returns false otherwise.
    */
    bool Connected() {return m_bConnected;};


    void AddWarning(CString csWarning);
    void AddError(CString caError);
    CArray<CString, CString> &GetWarnings(){return m_caWarnings;};
    CArray<CString, CString> &GetErrors(){return m_caErrors;};

private:
    /**
     * Reads all characters from the LC pipe and returns resulting string.
     */
    char *GetMessageFromPipe(DWORD &dwRead);

    static CLiveContextCommunicator* s_pCommunicator;
   	CLiveContextCommunicator();

    CDebugLog m_cDebugLog;

    LIo::MessagePipe *m_DptPipe;
    // True if the communication with LIVECONTEXT if established. False otherwise. 
    // No other operations are allowed if this is False (no communication established for no matter reason).
    bool m_bConnected;

    // This warnings array contains all click that have not been successfully parsed from LC.
    CArray<CString, CString> m_caWarnings;
    // This errors array contains the errors for which the recording couldn't complete.
    CArray<CString, CString> m_caErrors;
    // Critical section to prevent accessing same resource from multiple threads. 
    // Currently these resources are: m_pFileSgActions, m_caWarnings and m_caErrors.
    CRITICAL_SECTION m_cs;
};