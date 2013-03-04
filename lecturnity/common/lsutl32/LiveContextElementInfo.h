#pragma once
#include "..\..\common\jsoncpp\features.h"

#define E_LC_NOT_PRESENT   _HRESULT_TYPEDEF_(0x80c40001L)
#define E_NOT_FILLED_BY_LC _HRESULT_TYPEDEF_(0x80c40002L)
#define E_NOT_CONNECTED    _HRESULT_TYPEDEF_(0x80c40004L)


class LSUTL32_EXPORT CLiveContextElementInfo : public CObject {
public:
	CLiveContextElementInfo(CPoint &ptClick);
	virtual ~CLiveContextElementInfo();

public:
    HRESULT ParseJsonString(const char* sInfoMessage, DWORD dwRead);

    CString& GetName() { return m_sName; }
    HRESULT  GetBounds(CRect& rcElement);

    void SetID(UINT uiRequestID) { m_uiRequestID = uiRequestID; }
    UINT GetID() { return m_uiRequestID; }
    
    CString &GetAccID() {return m_sIdAcc;}
    CString &GetUiAutoID() {return m_sIdUiAuto;}
    CString &GetUiAutoTreeID() {return m_sIdUiAutoTree;}
    CString &GetSapID() {return m_sIdSap;}
    //CTime &GetTimestamp() {return m_ctTimeStamp;}
    CString &GetProcessName() {return m_sProcessName;}
    int GetAccType() {return m_iAccType;}
    int GetSapType() {return m_iSapType;}

    CString ToJsonString();

    Json::Value GetJsonRequestValue();

    static UINT GenerateID() {return m_suiId++;};

private:
    bool    m_bLcRead;

    CString m_sName;
    CString m_sProcessName;
    // CF: TODO is this required?
    //CTime   m_ctTimeStamp;

    CPoint  m_ptLocation;
    CSize   m_szSize;

    // type of MS Accessibility SDK
    int m_iAccType;
    // type of SAP API
    int m_iSapType;

    // internal ids only necessary for LC; passed back
    CString m_sIdAcc;
    CString m_sIdUiAuto;
    CString m_sIdUiAutoTree;
    CString m_sIdSap;

    // Each instance has a unique request id attached. 
    // This will be used to match request with response of same CLiveContextElementInfo instance.
    UINT m_uiRequestID;

    // Used to generate unique ids. Might be subject to change.
    static UINT m_suiId;
};


