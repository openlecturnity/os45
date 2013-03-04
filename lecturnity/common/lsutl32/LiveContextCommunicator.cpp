#include "global.h"
#include "stdafx.h"
//#include "Studio.h"
#include "LiveContextCommunicator.h"
#include "lutils.h" // LIo::MessagePipe
#include <string>
#include "..\..\common\jsoncpp\json.h"
#include "DebugLog.h"


CLiveContextCommunicator* CLiveContextCommunicator::s_pCommunicator = NULL;

// static
CLiveContextCommunicator*  CLiveContextCommunicator::GetInstance() {
    if (s_pCommunicator == NULL)
        s_pCommunicator = new CLiveContextCommunicator;
    return s_pCommunicator;
}

// private
CLiveContextCommunicator::CLiveContextCommunicator() {
    m_bConnected = false;
    m_DptPipe = new LIo::MessagePipe();
    m_cDebugLog.Init(_T("IMC_LEC_LC_LOG"));
    m_caWarnings.RemoveAll();
    InitializeCriticalSection(&m_cs);
}

CLiveContextCommunicator::~CLiveContextCommunicator() {
    SAFE_DELETE(m_DptPipe);
    m_caWarnings.RemoveAll();
    DeleteCriticalSection(&m_cs);
}

HRESULT CLiveContextCommunicator::Init(CString& sPipename) {
    m_DptPipe->Init(sPipename.GetBuffer(), false, GENERIC_WRITE | GENERIC_READ);
    sPipename.ReleaseBuffer();

    Json::Value root;
    root["MsgType"] = CString("HANDSHAKE-SYN");
    Json::FastWriter writer;

    m_cDebugLog.LogEntry(CString("Request: ") + CString(writer.write(root).c_str()));

    m_DptPipe->WriteCharMessage(writer.write(root).c_str());

    DWORD dwRead = 0;
    char *msg = GetMessageFromPipe(dwRead);

    m_cDebugLog.LogEntry(CString("Response: ") + CString(msg, dwRead));

    Json::Reader reader;
    bool bSucc = reader.parse(msg, msg + strlen(msg), root);
    SAFE_DELETE(msg);
    if (!bSucc) {
        return E_NOT_CONNECTED;
    }

    CString msgType(root["MsgType"].asString().c_str());
    if (msgType.Compare(CString("HANDSHAKE-ACK")) >= 0) {
        m_bConnected = true;
    }

    return m_bConnected ? S_OK : E_NOT_CONNECTED;
}

HRESULT CLiveContextCommunicator::RequestElementInfo(CLiveContextElementInfo *pInfo) {
    if (!m_bConnected)
        return E_NOT_CONNECTED;
    if (pInfo == NULL)
        return E_POINTER;

    HRESULT hr = S_OK;

    Json::Value root = pInfo->GetJsonRequestValue();
    if (root.isNull()) {
        m_cDebugLog.LogEntry(CString("Request is null: ") + pInfo->ToJsonString());
        return S_FALSE;
    }
    Json::FastWriter writer;

    CString output(CString(writer.write(root).c_str()));
    m_cDebugLog.LogEntry(CString("Request: ") + output);

    ::EnterCriticalSection(&m_cs);

    hr = m_DptPipe->WriteCharMessage(writer.write(root).c_str());
  
    ::LeaveCriticalSection(&m_cs);

    return hr;
}

HRESULT CLiveContextCommunicator::GetMessageString(char *szMessage, int iBufferSize, DWORD &dwRead) {
    if (!m_bConnected)
        return E_NOT_CONNECTED;	   
		
		
    HRESULT hr = S_OK;
	
    ::EnterCriticalSection(&m_cs);

    dwRead = 0;
    char *msg = GetMessageFromPipe(dwRead);	 

    ::LeaveCriticalSection(&m_cs);
	
    if (dwRead > 0) {
        int iBytesToCopy = dwRead < iBufferSize ? dwRead : iBufferSize;
        strncpy(szMessage, msg, dwRead);
        dwRead = iBytesToCopy;
    } 


    SAFE_DELETE(msg);

    return hr;
}

HRESULT CLiveContextCommunicator::PeekElement(char *szMessage, int iBufferSize, DWORD &dwRead) {
    if (!m_bConnected)
        return E_NOT_CONNECTED;	   

    if (!m_DptPipe)
        ASSERT(false);

    HRESULT hr = S_OK;

    CString csDebugMessage;
    if (m_DptPipe != NULL) {
        hr = m_DptPipe->PeekMessage(szMessage, iBufferSize, dwRead); 
        if (dwRead > 0) {
            if (dwRead < iBufferSize-1)
                szMessage[dwRead+1] = '\0';
            else
                szMessage[iBufferSize-1] = '\0';
        }


    }

    return hr;
}

HRESULT CLiveContextCommunicator::SendProgramExit(
    int iExitCode, CArray<CString, CString> *paWarnings, 
    CArray<CString, CString> *paErrors) {

    if (!m_bConnected)
        return E_NOT_CONNECTED;

    Json::Value root;
    root["MsgType"] = CString(_T("EXIT"));
    root["ReturnCode"] = iExitCode;

    if (paWarnings != NULL && paWarnings->GetSize() > 0) {
        Json::Value warnings;
        for (int i = 0; i < paWarnings->GetSize(); i++) {
            Json::Value warning;
            warning["Text"] = paWarnings->GetAt(i);
            warnings.append(warning);
        }
        root["Warnings"] = warnings;
    }

    if (paErrors != NULL && paErrors->GetSize() > 0) {
        Json::Value errors;
        for (int i = 0; i < paErrors->GetSize(); i++) {
            Json::Value error;
            error["Text"] = paErrors->GetAt(i);
            errors.append(error);
        }
        root["Errors"] = errors;
    }

    Json::FastWriter writer;
    m_cDebugLog.LogEntry(CString("Quit request: ") + writer.write(root).c_str());
    return m_DptPipe->WriteCharMessage(writer.write(root).c_str());
}

char *CLiveContextCommunicator::GetMessageFromPipe(DWORD &dwRead) {
    CString csMessage;
    HRESULT hrPipe = S_OK;
    const UINT buffSize = 4096;
    char szMessage[buffSize];
    do {
        if (!m_DptPipe) {
            ASSERT(false);
            return LString::WideChar2Ansi(csMessage);
        }
        hrPipe = m_DptPipe->ReadMessage(szMessage, buffSize, dwRead, false);
        if (SUCCEEDED(hrPipe)) {
            csMessage.Append(CString(szMessage), dwRead);
            if (dwRead < buffSize) {
                break;
            }
        }
    } while (SUCCEEDED(hrPipe));

    return LString::WideChar2Ansi(csMessage);
}

void CLiveContextCommunicator::AddWarning(CString csWarning) {
    ::EnterCriticalSection(&m_cs);
    m_caWarnings.Add(csWarning);
    ::LeaveCriticalSection(&m_cs);
};

void CLiveContextCommunicator::AddError(CString caError) {
    ::EnterCriticalSection(&m_cs);
    m_caErrors.Add(caError);
    ::LeaveCriticalSection(&m_cs);
};
