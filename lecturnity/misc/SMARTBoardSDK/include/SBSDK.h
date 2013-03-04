//  __         ____ ___
// (_ |\/| /\ |_)|   | _  _|_ ._  _ | _  _ o _  _
// __)|  |/--\| \|   |(/_(_| || |(_)|(_)(_||(/__>
//                                       _|
// Copyright © 2003 SMART Technologies Inc.
// C++ Header File ------------------------._.-._.-._.-._.-._.-._.-._.-._.---80|
//
// *** WARNING:
// *** THIS FILE CANNOT CHANGE, IF YOU NEED TO ADD NEW MESSAGES YOU MUST
// *** NOW PUT IT IN CSBSDK2.
//
// SMART Board SDK Usage
// ------------------------._.-._.-._.-._.-._.-._.-._.-._.---|
//
// These steps are for an MFC based application.
// - Add sbsdk.lib to the linker / input / additional dependencies.
// - Derive your View/Dialog/Window from CSBSDK.  Include "SBSDK.h".
// - In OnInitialUpdate/InitDialog/OnCreate Call SBSDKAttach with the window handle;
// - Add "ON_REGISTERED_MESSAGE(CSBSDK::SBSDK_NEW_MESSAGE, OnSBSDKNewMessage)"
//  to the message map.
// - Add
//	LRESULT CYourClass::OnSBSDKNewMessage(WPARAM wParam, LPARAM lParam)
//	{
//		SBSDKProcessData();
//		return 0;
//	}
// - Add "afx_msg LRESULT OnSBSDKNewMessage(WPARAM wParam, LPARAM lParam);" in the .h.
//
// - Override OnXYDown, OnXYMove, and OnXYUp for projected events.
// - Override OnXYNonProjectedDown, OnXYNonProjectedMove, and OnXYNonProjectedUp
//   for non-projected events.
// - For each one, there is an iPointerID.  Use this ID to handle multiple boards
//   and multiple pointers on a board (in the case of DViT).  With the ID, retrieve
//   tool attributes for that pointer with the functions provided like SBSDKGetToolColor.
//   Also use the ID to separate the events coming from different boards so that they
//   don't conflict with each other.  This allows two people to write at the same time.
//   In non-projected mode, if there is more than one board (SBSDKGetConnectedBoardCount),
//   you may want to have each board write on a different page.  Use
//   GetBoardNumberFromPointerID to determine which page to associate with.
// - Override the tool events for visual feedback if desired when handling multiple
//   boards.  If not handling multiple boards, use the tool events to set your "global"
//   tool attributes.
// - Override the button events.  If SBSDKIsBoardProjected, only do the button action
//  if your window/application has focus.  Otherwise, always perform the button action
//  for non-projected mode.

#pragma once
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SBSDK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SBSDK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef SBSDK_EXPORTS
#define SBSDK_API __declspec(dllexport)
#elif SBSDK_AS_LIBRARY
#define SBSDK_API
#else
#define SBSDK_API __declspec(dllimport)
#endif

#if defined(UNICODE) || defined(_UNICODE)
#define OnXMLAnnot OnXMLAnnotW
#else
#define OnXMLAnnot OnXMLAnnotA
#endif

#define SBSDK_REG_ID 1
#define SBSDK2_REG_ID 2

enum SBSDK_TOOL_TYPE
{
	SB_NO_TOOL = 0,
	SB_PEN,
	SB_ERASER,
	SB_RECTANGLE,
	SB_LINE,
	SB_CIRCLE,
	SB_POLYGON,
	SB_STAMP
};

enum SBSDK_MOUSE_EVENT_FLAG
{
	SB_MEF_DEFAULT = 0,
	SB_MEF_ALWAYS,
	SB_MEF_NEVER
};

#define SBSDK_NON_PROJECTED_RANGE 4096

class CSBSDKInternal;
class CSBSDK
{
	friend class CSBSDKEX;
protected:
	SBSDK_API CSBSDK();
public:
	SBSDK_API virtual ~CSBSDK(void);

	//Connect your window.
	SBSDK_API void SBSDKAttach(HWND hWnd, bool bSendXMLAnnotations = false);
	// The window handle that is given to the first call of SBSDKAttach
	// is where CSBSDK::SBSDK_NEW_MESSAGE messages are sent.  If you
	// prefer to have the messages sent to another window call this.
	SBSDK_API void SBSDKAttachWithMsgWnd(HWND hWnd, bool bSendXMLAnnotations, HWND hWndProcessData);

	//Handle this message and call SBSDKProcessData.
	SBSDK_API static UINT SBSDK_NEW_MESSAGE;
	SBSDK_API void SBSDKProcessData();

	//Projected (X,Y) events.
	SBSDK_API virtual void OnXYDown(int x, int y, int z, int iPointerID){};
	SBSDK_API virtual void OnXYMove(int x, int y, int z, int iPointerID){};
	SBSDK_API virtual void OnXYUp(int x, int y, int z, int iPointerID){};

	//Non-projected (X,Y) events.
	SBSDK_API virtual void OnXYNonProjectedDown(int x, int y, int z, int iPointerID){};
	SBSDK_API virtual void OnXYNonProjectedMove(int x, int y, int z, int iPointerID){};
	SBSDK_API virtual void OnXYNonProjectedUp(int x, int y, int z, int iPointerID){};

	SBSDK_API virtual void OnXMLAnnotW(wchar_t * szXMLAnnot){};
	SBSDK_API virtual void OnXMLAnnotA(char * szXMLAnnot){};

	//Tool events.
	SBSDK_API virtual void OnNoTool(int iPointerID){};
	SBSDK_API virtual void OnPen(int iPointerID){};
	SBSDK_API virtual void OnEraser(int iPointerID){};
	SBSDK_API virtual void OnRectangle(int iPointerID){};
	SBSDK_API virtual void OnLine(int iPointerID){};
	SBSDK_API virtual void OnCircle(int iPointerID){};
	SBSDK_API virtual void OnStamp(int iPointerID){};

	//Button events.
	SBSDK_API virtual void OnPrint(int iPointerID){};
	SBSDK_API virtual void OnNext(int iPointerID){};
	SBSDK_API virtual void OnPrevious(int iPointerID){};
	SBSDK_API virtual void OnClear(int iPointerID){};

	//Pointer Attributes.
	SBSDK_API SBSDK_TOOL_TYPE SBSDKGetToolType(int iPointerID);
	SBSDK_API COLORREF SBSDKGetToolColor(int iPointerID);
	SBSDK_API float SBSDKGetToolOpacity(int iPointerID);
	SBSDK_API COLORREF SBSDKGetToolFillColor(int iPointerID);
	SBSDK_API float SBSDKGetToolFillOpacity(int iPointerID);
	SBSDK_API int SBSDKGetToolWidth(int iPointerID);
	SBSDK_API void SBSDKSendMouseEvents(HWND hAttachedWnd, SBSDK_MOUSE_EVENT_FLAG iFlag, int iPointerID);

	//Information
	SBSDK_API virtual void OnSBSDKBoardStatusChange(){};
	SBSDK_API bool SBSDKIsABoardConnected();
	SBSDK_API int SBSDKGetConnectedBoardCount();
	SBSDK_API int SBSDKGetBoardCount();
	SBSDK_API bool SBSDKIsBoardProjected(int iPointerID);
	SBSDK_API int SBSDKGetBoardNumberFromPointerID(int iPointerID);
	SBSDK_API bool SBSDKGetSoftwareVersion(int * piMajor, int * piMinor, int * piUpdate, int * piBuild);
	SBSDK_API int SBSDKGetCurrentBoard();
	SBSDK_API void SBSDKGetCurrentTool();  // a regular on tool change event will come after calling this

private:
	CSBSDKInternal * m_pSBSDKInternal;
};
