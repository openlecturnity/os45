//  __         ____ ___
// (_ |\/| /\ |_)|   | _  _|_ ._  _ | _  _ o _  _
// __)|  |/--\| \|   |(/_(_| || |(_)|(_)(_||(/__>
//                                       _|
// Copyright © 2004 SMART Technologies Inc.
// C++ Header File ------------------------._.-._.-._.-._.-._.-._.-._.-._.---80|
//
// CSBSDK2 contains the basic functionality need by applications that need to
// recieve contact and pen tray events such as Notebook.
//
// SMART Board SDK Usage
// ------------------------._.-._.-._.-._.-._.-._.-._.-._.---|
//
// These steps are for an MFC based application.
// - Add sbsdk.lib to the linker / input / additional dependencies.
// - Include "SBSDK2.h".
// - Create an instance of CSBSDK2.
// - Derive your View/Dialog/Window from CSBSDK2EventHandler.
// - In the constructor call CSBSDK2::SetEventHandler(this)
// - In OnInitialUpdate/InitDialog/OnCreate Call CSBSDK2::SBSDKAttach with the window handle;
// - Add "ON_REGISTERED_MESSAGE(CSBSDK2::SBSDK_NEW_MESSAGE, OnSBSDKNewMessage)"
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

#include "SBSDK.h"

#if defined(UNICODE) || defined(_UNICODE)
#define SBSDK_CHAR wchar_t
#define OnXMLToolChange OnXMLToolChangeW
#define SBSDKGetXMLTool SBSDKGetXMLToolW
#else
#define SBSDK_CHAR char
#define OnXMLToolChange OnXMLToolChangeA
#define SBSDKGetXMLTool SBSDKGetXMLToolA
#endif

class CSBSDK2EventHandler
{
public:
	// Projected (X,Y) events.
	SBSDK_API virtual void OnXYDown(int x, int y, int z, int iPointerID){};
	SBSDK_API virtual void OnXYMove(int x, int y, int z, int iPointerID){};
	SBSDK_API virtual void OnXYUp(int x, int y, int z, int iPointerID){};

	// Non-projected (X,Y) events.
	SBSDK_API virtual void OnXYNonProjectedDown(int x, int y, int z, int iPointerID){};
	SBSDK_API virtual void OnXYNonProjectedMove(int x, int y, int z, int iPointerID){};
	SBSDK_API virtual void OnXYNonProjectedUp(int x, int y, int z, int iPointerID){};

	// To recieve these events call CSBSDK2::SBSDKAttach with bSendXMLAnnotations
	// set to true.
	// You can implement OnXMLAnnot(SBSDK_CHAR *szXMLAnnot), not the W or A version.
	SBSDK_API virtual void OnXMLAnnotW(unsigned short *szXMLAnnot){};
	SBSDK_API virtual void OnXMLAnnotA(char *szXMLAnnot){};

	// Tool events.  These will not be fired if you request to recieve XML tool changes.
	SBSDK_API virtual void OnNoTool(int iPointerID){};
	SBSDK_API virtual void OnPen(int iPointerID){};
	SBSDK_API virtual void OnEraser(int iPointerID){};
	SBSDK_API virtual void OnRectangle(int iPointerID){};
	SBSDK_API virtual void OnLine(int iPointerID){};
	SBSDK_API virtual void OnCircle(int iPointerID){};
	SBSDK_API virtual void OnStamp(int iPointerID){};

	// To recieve these events call CSBSDK2::SBSDKSendXMLToolChanges with the value true.
	// You  implement OnXMLToolChange(int iBoardID, SBSDK_CHAR *szXMLTool), not the W or A version.
	// You can get the XML for the tool at a later time by calling CSBSDK2::SBSDKGetXMLTool
	SBSDK_API virtual void OnXMLToolChangeW(int iBoardID, unsigned short *szXMLTool){};
	SBSDK_API virtual void OnXMLToolChangeA(int iBoardID, char *szXMLTool){};

	// Button events.
	SBSDK_API virtual void OnPrint(int iPointerID){};
	SBSDK_API virtual void OnNext(int iPointerID){};
	SBSDK_API virtual void OnPrevious(int iPointerID){};
	SBSDK_API virtual void OnClear(int iPointerID){};

	// Information
	SBSDK_API virtual void OnSBSDKBoardStatusChange(){};
};

class CSBSDKInternal;
class CSBSDK2
{
public:
	// By default multiple instances of CSBSDK2 will use the
	// same connection to the service.  If you want each instance
	// to have seperate connections (allows you to have seperate
	// event handlers) then pass in true to the constructor.
	SBSDK_API CSBSDK2(bool bSeperateConnection = false);
	SBSDK_API virtual ~CSBSDK2(void);

	SBSDK_API void SetEventHandler(CSBSDK2EventHandler *pHandler);

	// Connect your window.
	SBSDK_API void SBSDKAttach(HWND hWnd, bool bSendXMLAnnotations = false);
	// The window handle that is given to the first call of SBSDKAttach
	// is where CSBSDK::SBSDK_NEW_MESSAGE messages are sent.  If you
	// prefer to have the messages sent to another window call this.
	SBSDK_API void SBSDKAttachWithMsgWnd(HWND hWnd, bool bSendXMLAnnotations, HWND hWndProcessData);
	SBSDK_API void SBSDKDetachWnd(HWND hWnd);

	SBSDK_API HWND SBSDKGetXYDestinationWnd();

	// Handle this message and call SBSDKProcessData.
	SBSDK_API static UINT SBSDK_NEW_MESSAGE;
	SBSDK_API void SBSDKProcessData();

	// Tool Attributes.  These will not work if you request to recieve
	// XML tool changes.
	SBSDK_API SBSDK_TOOL_TYPE SBSDKGetToolType(int iPointerID);
	SBSDK_API COLORREF SBSDKGetToolColor(int iPointerID);
	SBSDK_API float SBSDKGetToolOpacity(int iPointerID);
	SBSDK_API COLORREF SBSDKGetToolFillColor(int iPointerID);
	SBSDK_API float SBSDKGetToolFillOpacity(int iPointerID);
	SBSDK_API int SBSDKGetToolWidth(int iPointerID);

	// If you want to recieve XML tool changes call before you attach any windows.
	SBSDK_API void SBSDKSendXMLToolChanges(bool bSendXMLToolChanges);
	// These two calls return a temporary pointer.  If you want to keep the XML around
	// make a copy of it.
	SBSDK_API wchar_t* SBSDKGetXMLToolW(int iPointerID);
	SBSDK_API char* SBSDKGetXMLToolA(int iPointerID);

	// Mouse events, to apply this setting to all pointer ID's use -1 for the pointer ID
	SBSDK_API void SBSDKSendMouseEvents(HWND hAttachedWnd, SBSDK_MOUSE_EVENT_FLAG iFlag, int iPointerID);

	// Information
	SBSDK_API bool SBSDKIsABoardConnected();
	SBSDK_API int SBSDKGetConnectedBoardCount();
	SBSDK_API int SBSDKGetBoardCount();
	SBSDK_API bool SBSDKIsBoardProjected(int iPointerID);
	SBSDK_API int SBSDKGetBoardNumberFromPointerID(int iPointerID);
	SBSDK_API bool SBSDKGetSoftwareVersion(int * piMajor, int * piMinor, int * piUpdate, int * piBuild);
	SBSDK_API int SBSDKGetCurrentBoard();
	SBSDK_API void SBSDKGetCurrentTool();  // a regular on tool change event will come after calling this

	SBSDK_API CSBSDKInternal* GetSBSDKInternal();

private:
	CSBSDKInternal *m_pSBSDKInternal;
	bool m_bSeperateConnection;
};
