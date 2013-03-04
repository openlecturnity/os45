//  __         ____ ___
// (_ |\/| /\ |_)|   | _  _|_ ._  _ | _  _ o _  _
// __)|  |/--\| \|   |(/_(_| || |(_)|(_)(_||(/__>
//                                       _|
// Copyright © 2004 SMART Technologies Inc.
// C++ Header File ------------------------._.-._.-._.-._.-._.-._.-._.-._.---80|
//
// CSBSDK2Advanced provides extra functionality need by some specialized
// applications.  For instance researchers have been interested in this type
// of functionality.
//
// See SBSDK2.h for detailed instructions regarding the sdk.  Interfaces other
// than CSBSDK2 can be used independantly but it is recomended that CSBSDK2
// also be used because it provides the registered window message and the
// SBSDKProcessData method.

#pragma once

#include "SBSDK2.h"
#include "SBSDKProtocol.h"

#if defined(UNICODE) || defined(_UNICODE)
#define SBRegisterProjectedWindowByName SBRegisterProjectedWindowByNameW
#define SBSDKSetToolXML SBSDKSetToolXMLW
#else
#define SBRegisterProjectedWindowByName SBRegisterProjectedWindowByNameA
#define SBSDKSetToolXML SBSDKSetToolXMLA
#endif

enum SBSDK_PEN_TRAY_TOOL {SBSDK_PTT_NONE = 0, SBSDK_PTT_ERASER, SBSDK_PTT_BLACK_PEN,
							SBSDK_PTT_BLUE_PEN, SBSDK_PTT_RED_PEN, SBSDK_PTT_GREEN_PEN};

class CSBSDK2AdvancedEventHandler
{
public:
	// DViT
	SBSDK_API virtual void OnDViTTrackerData(int iBoardNumber, unsigned char ucData1, unsigned char ucData2, unsigned char ucData3){}
	SBSDK_API virtual void OnDViTRawWidthHeight(int iWidth, int iHeight, int iPointerID){}
	SBSDK_API virtual void OnDViTAspectRatioAndDeltaAngles(double dAspectRatio, double dAngle1, double dAngle2, double dAngle3, double dAngle4){}

	// Projected Raw (X,Y) events.
	SBSDK_API virtual void OnXYDownRaw(int x, int y, int z, int iPointerID){};
	SBSDK_API virtual void OnXYMoveRaw(int x, int y, int z, int iPointerID){};
	SBSDK_API virtual void OnXYUpRaw(int x, int y, int z, int iPointerID){};

	// Button events.
	SBSDK_API virtual void OnRightMouse(int iPointerID){};
	SBSDK_API virtual void OnMiddleMouse(int iPointerID){};
	SBSDK_API virtual void OnFloatMouse(int iPointerID){};
	SBSDK_API virtual void OnKeyboard(int iPointerID){};

	SBSDK_API virtual void OnDViTMultiPointerMode(int iBoardNumber, bool bOn){};

	SBSDK_API virtual void OnMouseStateChange(SBSDK_MOUSE_STATE iMouseState){};

	SBSDK_API virtual void OnRawDataFromController(int iBoardNumber,int iMessage,unsigned char * pBuff,int iLen){};
};

class CSBSDK2Advanced
{
public:
	// If you create an instance of CSBSDK2 that uses a seperate
	// connection then to associate the advanced object with the
	// correct CSBSDK2 object you need to pass it in the constructor.
	SBSDK_API CSBSDK2Advanced(CSBSDK2 *pSBSDK = NULL);
	SBSDK_API virtual ~CSBSDK2Advanced(void);

	SBSDK_API void SetEventHandler(CSBSDK2AdvancedEventHandler *pHandler);

	// SDK
	SBSDK_API bool SBSDKIsConnected();

	// Board
	SBSDK_API SB_BOARD_STATUS SBSDKGetBoardStatus(int iBoardNumber);
	SBSDK_API void SBSDKGetMultiPointerMode(int iBoardNumber);
	SBSDK_API void SBSDKSetMultiPointerMode(int iBoardNumber, bool bOn);

	// Tool
	SBSDK_API void SBSDKSetToolXMLW(const wchar_t *szToolXMLW);
	SBSDK_API void SBSDKSetToolXMLA(const char *szToolXMLA);

	// Registration
	SBSDK_API bool SBSDKRegisterProjectedWindowByNameW(const wchar_t *szWindowTitleW, bool bSendXMLAnnotations);
	SBSDK_API bool SBSDKRegisterProjectedWindowByNameA(const char *szWindowTitleA, bool bSendXMLAnnotations);

	SBSDK_API SBSDK_PEN_TRAY_TOOL SBSDKGetPenTrayToolType(int iPointerID);

	SBSDK_API void SBSDKRegisterDataMonitor();

	// Mouse state
	SBSDK_API SBSDK_MOUSE_STATE SBSDKGetMouseState();
	SBSDK_API void SBSDKSetMouseState(SBSDK_MOUSE_STATE iMouseState);

	// DViT
	SBSDK_API bool SBSDKIsBoardDViT(int iBoardNumber);
	SBSDK_API bool SBSDKIsBoardBluewave(int iBoardNumber);
	SBSDK_API void SBSDKDViTStartTrackerMode(int iBoardNumber);
	SBSDK_API void SBSDKDViTStopTrackerMode(int iBoardNumber);

protected:
	CSBSDKInternal *m_pSBSDKInternal;
	bool m_bReleaseSBSDKInternal;
};
