#include "global.h"
#include "stdafx.h"
//#include "Studio.h"
#include "LiveContextElementInfo.h"
#include "..\..\common\jsoncpp\json.h"

#define ELEMENT_INFO_REQUEST _T("ELEMENT-INFO-REQUEST")

UINT CLiveContextElementInfo::m_suiId = 0;

CLiveContextElementInfo::CLiveContextElementInfo(CPoint &ptClick): m_szSize(-1,-1) {
    m_bLcRead = false;
    m_iAccType = -1;
    m_iSapType = -1;

    m_ptLocation = ptClick;
    m_uiRequestID = CLiveContextElementInfo::GenerateID();

    m_sName = _T("");
    m_sProcessName = _T("");

    // internal ids only necessary for LC; passed back
    m_sIdAcc = _T("");
    m_sIdUiAuto = _T("");
    m_sIdUiAutoTree = _T("");
    m_sIdSap = _T("");

}

CLiveContextElementInfo::~CLiveContextElementInfo() {
}

HRESULT CLiveContextElementInfo::ParseJsonString(const char* sInfoMessage, DWORD dwRead) {
    Json::Value root;
    Json::Features features;
    Json::Reader reader(features);
    if (!reader.parse(sInfoMessage, sInfoMessage + dwRead, root)) {
        return S_FALSE;
    }
    if (root.isNull() || root["RequestId"].isNull()) {
        return S_FALSE;
    }

    if (!root["RequestId"].isNull()) {
        m_uiRequestID = root["RequestId"].asInt();
    }

    if (!root["ElementInfo"].isNull()) {
        if (!root["ElementInfo"]["IDs"].isNull()) {
            m_sIdAcc = root["ElementInfo"]["IDs"]["IdAcc"].asString().c_str();
            m_sIdUiAuto = root["ElementInfo"]["IDs"]["IdUiAuto"].asString().c_str();
            m_sIdUiAutoTree = root["ElementInfo"]["IDs"]["IdUiAutoTree"].asString().c_str();
            m_sIdSap = root["ElementInfo"]["IDs"]["IdSap"].asString().c_str();
        }
        if (!root["ElementInfo"]["Bounds"].isNull()) {
            m_ptLocation.y = root["ElementInfo"]["Bounds"]["Top"].asInt();
            m_ptLocation.x = root["ElementInfo"]["Bounds"]["Left"].asInt();
            m_szSize.cy = root["ElementInfo"]["Bounds"]["Height"].asInt();
            m_szSize.cx = root["ElementInfo"]["Bounds"]["Width"].asInt();
        }
        if (!root["ElementInfo"]["Name"].isNull()) {
            m_sName = root["ElementInfo"]["Name"].asString().c_str();
        }
        if (!root["ElementInfo"]["ProcessName"].isNull()) {
            m_sProcessName = root["ElementInfo"]["ProcessName"].asString().c_str();
        }

        if (!root["ElementInfo"]["AccType"].isNull()) {
            m_iAccType = root["ElementInfo"]["AccType"].asInt();
        }
        if (!root["ElementInfo"]["SapType"].isNull()) {
            m_iSapType = root["ElementInfo"]["SapType"].asInt();
        }
    }
    m_bLcRead = true;
    return S_OK;
}

HRESULT CLiveContextElementInfo::GetBounds(CRect& rcElement) {
    //if (!m_bLcRead)
    //    return E_NOT_FILLED_BY_LC;

    rcElement.left = m_ptLocation.x;
    rcElement.top = m_ptLocation.y;
    rcElement.right = rcElement.left + m_szSize.cx;
    rcElement.bottom = rcElement.top + m_szSize.cy;

    return S_OK;
}

CString CLiveContextElementInfo::ToJsonString() {
    Json::Value root;

    root["RequestId"] = m_uiRequestID;

    Json::Value elementInfo;
    // Create "IDs" object
    Json::Value jsonValIDs;
    jsonValIDs["IdAcc"] = m_sIdAcc;
    jsonValIDs["IdUiAuto"] = m_sIdUiAuto;
    jsonValIDs["IdUiAutoTree"] = m_sIdUiAutoTree;
    jsonValIDs["IdSap"] = m_sIdSap;
    elementInfo["IDs"] = jsonValIDs;

    // Create "Bounds" object
    CRect rcLocation;
    GetBounds(rcLocation);
    Json::Value jsonValBounds;
    jsonValBounds["Top"] = rcLocation.top;
    jsonValBounds["Left"] = rcLocation.left;
    jsonValBounds["Height"] = m_szSize.cy; 
    jsonValBounds["Width"] = m_szSize.cx;
    elementInfo["Bounds"] = jsonValBounds;

    elementInfo["Name"] = GetName();
    elementInfo["ProcessName"] = m_sProcessName;

    elementInfo["AccType"] = m_iAccType;
    elementInfo["SapType"] = m_iSapType;

    root["ElementInfo"] = elementInfo;

    Json::FastWriter writer;
    CString outputConfig(writer.write(root).c_str());
    return outputConfig;
}

Json::Value CLiveContextElementInfo::GetJsonRequestValue() {
    Json::Value root;
    root["MsgType"] = CString(ELEMENT_INFO_REQUEST);
    root["RequestId"] = m_uiRequestID;

    // Create "Position" object
    Json::Value jsonPosition;
    jsonPosition["X"] = m_ptLocation.x;
    jsonPosition["Y"] = m_ptLocation.y;
    root["Position"] = jsonPosition;

    return root;
}