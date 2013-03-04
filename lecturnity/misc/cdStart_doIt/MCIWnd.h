/******************************************************************************
**
** MCIWnd.h
**
** MFC Wrapper class for the MCIWnd macros.
**
** Applications that use this class must link with Vfw32.lib.
**
******************************************************************************/

#if !defined(AFX_MCIWND_H__9F4FBE3C_939F_4927_992A_511207F8984B__INCLUDED_)
#define AFX_MCIWND_H__9F4FBE3C_939F_4927_992A_511207F8984B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Video for windows included here so users of CMCIWnd don't have to...
#include <vfw.h>

/******************************************************************************
**
** CMCIWnd
**
******************************************************************************/
class CMCIWnd : public CWnd
{
public:

	// Object Construction
	CMCIWnd();
	virtual ~CMCIWnd();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	
	// Window Management
	void ChangeStyles(UINT nMask, long nValue);
	UINT GetStyles();
	void SetOwner(CWnd * pOwner);

	// File and Device Management
	void Close();
	void Destroy();
	long Eject();
	long New(LPCSTR strDeviceName);
	long Open(LPCSTR strFileName, UINT nFlags = 0);
	long OpenDialog();
	void OpenInterface(LPUNKNOWN pUnk);
	long Save(LPCSTR strFileName);
	long SaveDialog();

	// Playback Options
	BOOL GetRepeat();
	long Pause();
	long Play();
	long PlayFrom(long nPosition);
	long PlayFromTo(long nStart, long nEnd);
	long PlayReverse();
	long PlayTo(long nPosition);
	long Resume();
	void SetRepeat(BOOL bRepeat);
	long Stop();

	// Recording
	long Record();

	// Positioning
	long End();
	long GetEnd();
	long GetLength();
	long GetPosition();
	long GetPositionString(CString & strPosition, UINT nMaxPosStringLength = 1000);
	long GetStart();
	long Home();
	long Seek(long nPosition);
	long Step(long nSteps);

	// Performance Tuning
	long GetSpeed();
	long GetVolume();
	UINT GetZoom();
	long SetSpeed(UINT nSpeed);
	long SetVolume(UINT nVolume);
	void SetZoom(UINT nZoom);

	// Image and Palette Adjustments
	long GetDest(CRect & rect);
	HPALETTE GetPalette();
	long GetSource(CRect & rect);
	long PutDest(CRect rect);
	long PutSource(CRect rect);
	long Realize(BOOL bBackground);
	long SetPalette(HPALETTE hPalette);

	// Event and Error Notification
	long GetError(CString & strError, UINT nMaxErrorStringLength = 1000);

	// Time Formats
	long GetTimeFormat(CString & strTimeFormat, UINT nMaxFormatStringLength = 1000);
	long SetTimeFormat(LPCSTR strTimeFormat);
	long UseFrames();
	long UseTime();
	void ValidateMedia();

	// Status Updates
	UINT GetActiveTimer();
	UINT GetInactiveTimer();
	void SetActiveTimer(UINT nMilliseconds);
	void SetInactiveTimer(UINT nMilliseconds);
	void SetTimers(UINT nActive, UINT nInactive);

	// Device Capabilities
	BOOL CanConfig();
	BOOL CanEject();
	BOOL CanPlay();
	BOOL CanRecord();
	BOOL CanSave();
	BOOL CanWindow();

	// MCI Device Settings
	UINT GetAlias();
	long GetDevice(CString & strDevName, UINT nMaxDevNameLength = 1000);
	UINT GetDeviceID();
	long GetFileName(CString & strFilename, UINT nMaxFilenameLength = 1000);
	long GetMode(CString & strMode, UINT nMaxModeLength = 1000);

	// MCI Command-String Interface
	long ReturnString(CString & strReply, UINT nMaxReplyStringLength = 1000);
	long SendString(LPCSTR strCommand);


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMCIWnd)
	//}}AFX_VIRTUAL


protected:

	// Generated message map functions
	//{{AFX_MSG(CMCIWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCIWND_H__9F4FBE3C_939F_4927_992A_511207F8984B__INCLUDED_)
