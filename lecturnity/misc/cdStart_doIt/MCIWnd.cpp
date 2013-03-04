/******************************************************************************
**
** MCIWnd.cpp
**
** MFC Wrapper class for the MCIWnd macros.
**
** Applications that use this class must link with Vfw32.lib.
**
******************************************************************************/

#include "stdafx.h"
#include "MCIWnd.h"
#include <DIGITALV.H>
#include <MMSYSTEM.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CMCIWnd, CWnd)
	//{{AFX_MSG_MAP(CMCIWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/******************************************************************************
**
** CMCIWnd::CMCIWnd
**
******************************************************************************/
CMCIWnd::CMCIWnd()
{
}


/******************************************************************************
**
** CMCIWnd::~CMCIWnd
**
******************************************************************************/
CMCIWnd::~CMCIWnd()
{
	if(::IsWindow(m_hWnd))
	{
		Destroy();
	}
}


/******************************************************************************
**
** CMCIWnd::Create
**
** Create an MCI window.
**
** dwStyle - See MCIWndCreate() function for list of styles.
**
** pParentWnd - The parent window.
**
** nID - The ID of the child window.
**
******************************************************************************/
BOOL CMCIWnd::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// Register the window class.
	MCIWndRegisterClass();
		
	CWnd* pWnd = this;
	return pWnd->Create(MCIWND_WINDOW_CLASS, NULL, dwStyle, rect, pParentWnd, nID);
}


/******************************************************************************
**
** CMCIWnd::CanConfig
**
** Determines if an MCI device can display a configuration dialog box.
**
** Returns TRUE if the device supports configuration or FALSE otherwise.
**
******************************************************************************/
BOOL CMCIWnd::CanConfig()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndCanConfig(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::CanEject
**
** Determines if an MCI device can eject its media.
**
** Returns TRUE if the device can eject its media or FALSE otherwise.
**
******************************************************************************/
BOOL CMCIWnd::CanEject()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndCanEject(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::CanPlay
**
** Determines if an MCI device can play a data file or content of some other 
** kind.
**
** Returns TRUE if the device supports playing the data or FALSE otherwise.
**
******************************************************************************/
BOOL CMCIWnd::CanPlay()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndCanPlay(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::CanRecord
**
** Determines if an MCI device supports recording.
**
** Returns TRUE if the device supports recording or FALSE otherwise.
**
******************************************************************************/
BOOL CMCIWnd::CanRecord()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndCanRecord(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::CanSave
**
** Determines if an MCI device can save data.
**
** Returns TRUE if the device supports saving data or FALSE otherwise.
**
******************************************************************************/
BOOL CMCIWnd::CanSave()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndCanSave(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::CanWindow
**
** Determines if an MCI device supports window-oriented MCI commands.
**
** Returns TRUE if the device supports window-oriented MCI commands or FALSE 
** otherwise.
**
******************************************************************************/
BOOL CMCIWnd::CanWindow()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndCanWindow(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::ChangeStyles
**
** Changes the styles used by the MCIWnd window.
**
** dwMask - Mask that identifies the styles that can change. This mask is the 
**          bitwise OR operator of all styles that will be permitted to change.
**
** dwValue - New style settings for the window. Specify zero for this parameter
**           to turn off all styles identified in the mask. For a list of the 
**           available styles, see the MCIWndCreate() function.
**
******************************************************************************/
void CMCIWnd::ChangeStyles(UINT nMask, long nValue)
{
	ASSERT(::IsWindow(m_hWnd));
	MCIWndChangeStyles(m_hWnd, nMask, nValue);
}


/******************************************************************************
**
** CMCIWnd::Close
**
** Closes an MCI device or file associated with an MCIWnd window. 
**
******************************************************************************/
void CMCIWnd::Close()
{
	ASSERT(::IsWindow(m_hWnd));
	MCIWndClose(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::Destroy
**
** Closes an MCI device or file associated with an MCIWnd window. 
**
******************************************************************************/
void CMCIWnd::Destroy()
{
	ASSERT(::IsWindow(m_hWnd));
	Close();
	CWnd::OnClose();
}


/******************************************************************************
**
** CMCIWnd::Eject
**
** Sends a command to an MCI device to eject its media.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::Eject()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndEject(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::End
**
** Moves the current position to the end of the content.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::End()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndEnd(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetActiveTimer
**
** Retrieves the update period used when the MCIWnd window is the active window.
**
** Returns the update period in milliseconds. The default is 500 milliseconds.
**
******************************************************************************/
UINT CMCIWnd::GetActiveTimer()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetActiveTimer(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetAlias
**
** Retrieves the alias used to open an MCI device or file with the 
** mciSendString() function. 
**
** Returns the device alias.
**
******************************************************************************/
UINT CMCIWnd::GetAlias()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetAlias(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetDest
**
** Retrieves the coordinates of the destination rectangle used for zooming or 
** stretching the images of an AVI file during playback.
**
** rect - Used to return the coordinates of the destination rectangle. 
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::GetDest(CRect & rect)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetDest(m_hWnd, (LPRECT) rect);
}


/******************************************************************************
**
** CMCIWnd::GetDevice
**
** Retrieves the name of the current MCI device.
**
** strDevName - String buffer to return the device name.
**
** nMaxDevNameLength - Maximum length allowed for a device name.  If the
**					   name is longer than this it will truncated.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::GetDevice(CString & strDevName, UINT nMaxDevNameLength /* = 1000 */)
{
	ASSERT(::IsWindow(m_hWnd));
	LPTSTR lpString = strDevName.GetBufferSetLength(nMaxDevNameLength);
	long nReturn = MCIWndGetDevice(m_hWnd, lpString, nMaxDevNameLength);
	strDevName.ReleaseBuffer();
	return nReturn;
}


/******************************************************************************
**
** CMCIWnd::GetDeviceID
**
** Retrieves the identifier of the current MCI device to use with the 
** mciSendCommand() function.
**
** Returns the device identifier.
**
******************************************************************************/
UINT CMCIWnd::GetDeviceID()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetDeviceID(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetEnd
**
** Retrieves the location of the end of the content of an MCI device or file.
**
** Returns the location in the current time format.
**
******************************************************************************/
long CMCIWnd::GetEnd()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetEnd(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetError
**
** Retrieves a string corresponding to the last MCI error encountered.
**
** strError - String buffer to return the error.  Set nMaxErrorStringLength
**			  to zero to inhibit the use of this parameter.
**
** nMaxErrorStringLength - Maximum length allowed for the error string. If the
**						   error is longer than this it will truncated.
**
** Returns the integer error value if successful.
**
******************************************************************************/
long CMCIWnd::GetError(CString & strError, UINT nMaxErrorStringLength /* = 1000 */)
{
	ASSERT(::IsWindow(m_hWnd));
	long nReturn;
	if(nMaxErrorStringLength != 0)
	{
		LPTSTR lpString = strError.GetBufferSetLength(nMaxErrorStringLength);
		nReturn = MCIWndGetError(m_hWnd, lpString, nMaxErrorStringLength);
		strError.ReleaseBuffer();
	}
	else
	{
		nReturn = MCIWndGetError(m_hWnd, NULL, 0);
	}
	return nReturn;
}


/******************************************************************************
**
** CMCIWnd::GetFileName
**
** Retrieves the filename used by an MCI device.
**
** strFilename - String buffer to return the filename.
**
** nMaxFilenameLength - Maximum length allowed for the filename string. If the
**						name is longer than this it will truncated.
**
** Returns zero if successful or 1 otherwise.
**
******************************************************************************/
long CMCIWnd::GetFileName(CString & strFilename, UINT nMaxFilenameLength /* = 1000 */)
{
	ASSERT(::IsWindow(m_hWnd));
	LPTSTR lpString = strFilename.GetBufferSetLength(nMaxFilenameLength);
	long nReturn = MCIWndGetFileName(m_hWnd, lpString, nMaxFilenameLength);
	strFilename.ReleaseBuffer();
	return nReturn;
}


/******************************************************************************
**
** CMCIWnd::GetInactiveTimer
**
** Retrieves the update period used when the MCIWnd window is the inactive 
** window. 
**
** Returns the update period, in milliseconds. The default value is 2000 
** milliseconds.
**
******************************************************************************/
UINT CMCIWnd::GetInactiveTimer()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetInactiveTimer(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetLength
**
** Retrieves the length of the content or file currently used by an MCI device.
**
** Returns the length. The units for the length depend on the current time format.
**
******************************************************************************/
long CMCIWnd::GetLength()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetLength(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetMode
**
** Retrieves the current operating mode of an MCI device. MCI devices have 
** several operating modes, which are designated by constants.
**
** strMode - String buffer to return the mode.
**
** nMaxModeLength - Maximum length allowed for the mode string. If the
**					mode is longer than this it will truncated.
**
** Returns an integer corresponding to the MCI constant defining the mode.
**
** Refer to MCIWndGetMode() for more information about modes.
**
******************************************************************************/
long CMCIWnd::GetMode(CString & strMode, UINT nMaxModeLength /* = 1000 */)
{
	ASSERT(::IsWindow(m_hWnd));
	LPTSTR lpString = strMode.GetBufferSetLength(nMaxModeLength);
	long nReturn = MCIWndGetMode(m_hWnd, lpString, nMaxModeLength);
	strMode.ReleaseBuffer();
	return nReturn;
}


/******************************************************************************
**
** CMCIWnd::GetPalette
**
** Retrieves a handle of the palette used by an MCI device.
**
** Returns the handle of the palette if successful.
**
******************************************************************************/
HPALETTE CMCIWnd::GetPalette()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetPalette(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetPosition
**
** Retrieves the numerical value of the current position within the content 
** of the MCI device.
**
** Returns an integer corresponding to the current position. The units for the
** position value depend on the current time format.
**
******************************************************************************/
long CMCIWnd::GetPosition()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetPosition(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetPositionString
**
** Retrieves the numerical value of the current position within the content 
** of the MCI device. Also provides the current position in a string form.
**
** strPosition - String buffer used to return the position.  Set 
**				 nMaxPosStringLength to zero to inhibit the use of this 
**				 parameter. If the device supports tracks, the string position
**				 information is returned in the form TT:MM:SS:FF where TT 
**				 corresponds to tracks, MM and SS correspond to minutes and 
**			     seconds, and FF corresponds to frames.
**
** nMaxPosStringLength - Maximum length allowed for the position string. If the
**						 position is longer than this it will truncated.
**
** Returns an integer corresponding to the current position. The units for the 
** position value depend on the current time format.
**
******************************************************************************/
long CMCIWnd::GetPositionString(CString & strPosition, UINT nMaxPosStringLength /* = 1000 */)
{
	ASSERT(::IsWindow(m_hWnd));
	long nReturn;
	if(nMaxPosStringLength != 0)
	{
		LPTSTR lpString = strPosition.GetBufferSetLength(nMaxPosStringLength);
		nReturn = MCIWndGetPositionString(m_hWnd, lpString, nMaxPosStringLength);
		strPosition.ReleaseBuffer();
	}
	else
	{
		nReturn = MCIWndGetPositionString(m_hWnd, 0, 0);
	}
	return nReturn;
}


/******************************************************************************
**
** CMCIWnd::GetRepeat
**
** Determines if continuous playback has been activated.
**
** Returns TRUE if continuous playback is activated or FALSE otherwise.
**
******************************************************************************/
BOOL CMCIWnd::GetRepeat()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetRepeat(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetSource
**
** Retrieves the coordinates of the source rectangle used for cropping the 
** images of an AVI file during playback.
**
** rect - Used to return the coordinates of the source rectangle. 
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::GetSource(CRect & rect)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetSource(m_hWnd, (LPRECT) rect);
}


/******************************************************************************
**
** CMCIWnd::GetSpeed
**
** Retrieves the playback speed of an MCI device.
**
** Returns the playback speed if successful. The value for normal speed is 
** 1000. Larger values indicate faster speeds; smaller values indicate slower 
** speeds.
**
******************************************************************************/
long CMCIWnd::GetSpeed()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetSpeed(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetStart
**
** Retrieves the location of the beginning of the content of an MCI device or 
** file.
**
** Returns the location in the current time format. Typically, the return value
** is zero; but some devices use a nonzero starting location. Seeking to this
** location sets the device to the start of the media.
**
******************************************************************************/
long CMCIWnd::GetStart()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetStart(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetStyles
**
** Retrieves the flags specifying the current MCIWnd window styles used by a 
** window. 
**
** Returns a value representing the current styles of the MCIWnd window. 
** The return value is the bitwise OR operator of the MCIWnd window styles 
** (MCIWNDF flags).
**
******************************************************************************/
UINT CMCIWnd::GetStyles()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetStyles(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetTimeFormat
**
** Retrieves the current time format of an MCI device in two forms: as a 
** numerical value and as a string.  Refer to MCIWndGetTimeFormat() for more
** information regarding time formats.
**
** strTimeFormat - String buffer used to return the time format.
**
** nMaxFormatStringLength - Maximum length allowed for the time format string. 
**							If the time format is longer than this it will 
**							truncated.
**
** Returns an integer corresponding to the MCI constant defining the time format.
**
******************************************************************************/
long CMCIWnd::GetTimeFormat(CString & strTimeFormat, UINT nMaxFormatStringLength /* = 1000 */)
{
	ASSERT(::IsWindow(m_hWnd));
	LPTSTR lpString = strTimeFormat.GetBufferSetLength(nMaxFormatStringLength);
	long nReturn = MCIWndGetTimeFormat(m_hWnd, lpString, nMaxFormatStringLength);
	strTimeFormat.ReleaseBuffer();
	return nReturn;
}


/******************************************************************************
**
** CMCIWnd::GetVolume
**
** Retrieves the current volume setting of an MCI device.
**
** Returns the current volume setting. The default value is 1000. Higher 
** values indicate louder volumes; lower values indicate quieter volumes.
**
******************************************************************************/
long CMCIWnd::GetVolume()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetVolume(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::GetZoom
**
** Retrieves the current zoom value used by an MCI device. 
**
** Returns the most recent values used with SetZoom. A return value of 100 
** indicates the image is not zoomed. A value of 200 indicates the image is 
** enlarged to twice its original size. A value of 50 indicates the image is 
** reduced to half its original size.
**
******************************************************************************/
UINT CMCIWnd::GetZoom()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndGetZoom(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::Home
**
** Moves the current position to the beginning of the content.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::Home()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndHome(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::New
**
** Creates a new file for the current MCI device.
**
** strDeviceName - The name of the MCI device that will use the file.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::New(LPCSTR strDeviceName)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndNew(m_hWnd, strDeviceName);
}


/******************************************************************************
**
** CMCIWnd::Open
**
** Open a file.
**
** strFileName - String identifying the filename or MCI device name to open.
**
** nFlags - Flags associated with the device or file to open. 
**			The MCIWNDOPENF_NEW flag specifies a new file is to be created 
**			with the name specified in szFile.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::Open(LPCSTR strFileName, UINT nFlags /* =0 */)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndOpen(m_hWnd, strFileName, nFlags);
}


/******************************************************************************
**
** CMCIWnd::OpenDialog
**
** Displays the Open dialog box for the user to select the data file to 
** associate with an MCI window. Opens a user-specified data file and 
** corresponding type of MCI device, and associates them with an MCIWnd window. 
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::OpenDialog()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndOpenDialog(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::OpenInterface
**
** Attaches the data stream or file associated with the specified interface to 
** an MCIWnd window.
**
** pUnk - Pointer to an IAVI interface that points to a file or a data stream 
**		  in a file.
**
******************************************************************************/
void CMCIWnd::OpenInterface(LPUNKNOWN pUnk)
{
	ASSERT(::IsWindow(m_hWnd));
	MCIWndOpenInterface(m_hWnd, pUnk);
}


/******************************************************************************
**
** CMCIWnd::Pause
**
** Sends a command to an MCI device to pause playing or recording. 
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::Pause()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndPause(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::Play
**
** Sends a command to an MCI device to start playing from the current position
** in the content.
**
** Will repetively loop the playing if SetRepeat(TRUE) was called.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::Play()
{
	ASSERT(::IsWindow(m_hWnd));
//	return MCIWndPlay(m_hWnd);

	DWORD dwFlags;
	MCI_DGV_PLAY_PARMS	mciPlay;
	
	// Put together the play call.
	mciPlay.dwCallback = MAKELONG(m_hWnd,0);
	mciPlay.dwFrom = GetPosition();
	mciPlay.dwTo = 0;
	dwFlags = MCI_NOTIFY;
	
	// If we are to repeat when playing.
	if(GetRepeat())
		dwFlags |= MCI_DGV_PLAY_REPEAT;

	return mciSendCommand(GetDeviceID(), MCI_PLAY, dwFlags,
					     (DWORD)(LPMCI_DGV_PLAY_PARMS)&mciPlay);
}


/******************************************************************************
**
** CMCIWnd::PlayFrom
**
** Plays the content of an MCI device from the specified location to the end of
** the content or until another command stops playback.
**
** nPosition - Starting location. The units for the starting location depend
**			   on the current time format.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::PlayFrom(long nPosition)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndPlayFrom(m_hWnd, nPosition);
}


/******************************************************************************
**
** CMCIWnd::PlayFromTo
**
** Plays a portion of content between specified starting and ending locations. 
** Seeks to the specified point to begin playback, then plays the content to 
** the specified ending location.
**
** nStart - Position to seek; it is also the starting location.  The units 
**			for the seek position depend on the current time format.
**
** nEnd - Ending location.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::PlayFromTo(long nStart, long nEnd)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndPlayFromTo(m_hWnd, nStart, nEnd);
}


/******************************************************************************
**
** CMCIWnd::PlayReverse
**
** Plays the current content in the reverse direction, beginning at the current
** position and ending at the beginning of the content or until another command
** stops playback. 
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::PlayReverse()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndPlayReverse(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::PlayTo
**
** Plays the content of an MCI device from the current position to the 
** specified ending location or until another command stops playback. If the 
** specified ending location is beyond the end of the content, playback stops 
** at the end of the content.
**
** nPosition - Ending location. The units for the ending location depend on 
**			   the current time format.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::PlayTo(long nPosition)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndPlayTo(m_hWnd, nPosition);
}


/******************************************************************************
**
** CMCIWnd::PutDest
**
** Redefines the coordinates of the destination rectangle used for zooming or 
** stretching the images of an AVI file during playback.
**
** rect - Used to return the coordinates of the destination rectangle. 
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::PutDest(CRect rect)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndPutDest(m_hWnd, (LPRECT) rect);
}


/******************************************************************************
**
** CMCIWnd::PutSource
**
** Redefines the coordinates of the source rectangle used for cropping the 
** images of an AVI file during playback.
**
** rect - Used to return the coordinates of the source rectangle. 
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::PutSource(CRect rect)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndPutSource(m_hWnd, (LPRECT) rect);
}


/******************************************************************************
**
** CMCIWnd::Realize
**
** Controls how an MCI window realized in the foreground or background. 
** Causes the palette for the MCI window to be realized in the process.
**
** bBackground - Background flag. Specify TRUE for this parameter for the window
**				 to be realized in the background or FALSE if the window can be 
**				 realized in the foreground. 
**
** Returns zero if successful or an error otherwise.
**
** A common use for MCIWndRealize is to coordinate palette ownership between an 
** MCI control and the application that contains it. The application can have the
** MCI window realize in the background and realize its own palette in the foreground. 
** If your application contains an MCI control, but does not need to realize its 
** palette, you can use this macro to handle the WM_PALETTECHANGED and 
** WM_QUERYNEWPALETTE messages, instead of using RealizePalette. However, it is 
** usually easier to call the SendMessage function to forward the message to the 
** MCIWnd window, which will automatically realize the palette.
**
******************************************************************************/
long CMCIWnd::Realize(BOOL bBackground)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndRealize(m_hWnd, bBackground);
}


/******************************************************************************
**
** CMCIWnd::Record
**
** Begins recording content using the MCI device. The recording process begins 
** at the current position in the content and will overwrite existing data for 
** the duration of the recording.  
**
** Returns zero if successful or an error otherwise.
**
** The function that an MCI device performs during recording depends on the 
** characteristics of the device. An MCI device that uses files, such as a 
** waveform-audio device, sends data to the file during recording. An MCI 
** device that does not use files, such as a video-cassette recorder, receives 
** and externally records data on another medium.
**
******************************************************************************/
long CMCIWnd::Record()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndRecord(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::Resume
**
** Resumes playback or recording content from the paused mode. Restarts 
** playback or recording from the current position in the content.  
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::Resume()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndResume(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::ReturnString
**
** Retrieves the reply to the most recent MCI string command sent to an MCI 
** device.
**
** strReply - String buffer used to return the reply.
**
** nMaxReplyStringLength - Maximum length allowed for the reply string. 
**						   If the reply is longer than this it will truncated.
**
** Returns an integer corresponding to the MCI string.
**
******************************************************************************/
long CMCIWnd::ReturnString(CString & strReply, UINT nMaxReplyStringLength /* = 1000 */)
{
	ASSERT(::IsWindow(m_hWnd));
	LPTSTR lpString = strReply.GetBufferSetLength(nMaxReplyStringLength);
	long nReturn = MCIWndReturnString(m_hWnd, lpString, nMaxReplyStringLength);
	strReply.ReleaseBuffer();
	return nReturn;
}


/******************************************************************************
**
** CMCIWnd::Save
**
** Saves the content currently used by an MCI device to a specified data file.
**
** strFileName - String containing the name and path of the destination file.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::Save(LPCSTR strFileName)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndSave(m_hWnd, strFileName);
}


/******************************************************************************
**
** CMCIWnd::SaveDialog
**
** Saves the content currently used by an MCI device. Displays the Save dialog 
** box to let the user select a filename to store the content.  
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::SaveDialog()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndSaveDialog(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::Seek
**
** Moves the playback position to the specified location in the content.
**
** nPosition - Position to seek. You can specify a position using the current 
**			   time format, the MCIWND_START constant to designate the beginning 
**			   of the content, or the MCIWND_END constant to designate the end 
**			   of the content.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::Seek(long nPosition)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndSeek(m_hWnd, nPosition);
}


/******************************************************************************
**
** CMCIWnd::SendString
**
** Sends an MCI command in string form to the device associated with the 
** MCIWnd window.
**
** strCommand - String containing the command.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::SendString(LPCSTR strCommand)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndSendString(m_hWnd, strCommand);
}


/******************************************************************************
**
** CMCIWnd::SetActiveTimer
**
** Sets the update period used by MCIWnd to update the trackbar in the MCIWnd 
** window, update position information displayed in the window title bar, and 
** send notification messages to the parent window when the MCIWnd window is 
** active.
**
** nMilliseconds - Update period, in milliseconds. The default is 500 milliseconds.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
void CMCIWnd::SetActiveTimer(UINT nMilliseconds)
{
	ASSERT(::IsWindow(m_hWnd));
	MCIWndSetActiveTimer(m_hWnd, nMilliseconds);
}


/******************************************************************************
**
** CMCIWnd::SetInactiveTimer
**
** Sets the update period used by MCIWnd to update the trackbar in the MCIWnd 
** window, update position information displayed in the window title bar, and 
** send notification messages to the parent window when the MCIWnd window is 
** inactive.
**
** nMilliseconds - Update period, in milliseconds. The default is 2000 milliseconds.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
void CMCIWnd::SetInactiveTimer(UINT nMilliseconds)
{
	ASSERT(::IsWindow(m_hWnd));
	MCIWndSetInactiveTimer(m_hWnd, nMilliseconds);
}


/******************************************************************************
**
** CMCIWnd::SetOwner
**
** Sets the window to receive notification messages associated with the MCIWnd 
** window.
**
** pOwner - Pointer to the window to receive the notification messages.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
void CMCIWnd::SetOwner(CWnd * pOwner)
{
	ASSERT(::IsWindow(m_hWnd));
	MCIWndSetOwner(m_hWnd, pOwner->GetSafeHwnd());
}


/******************************************************************************
**
** CMCIWnd::SetPalette
**
** Sends a palette handle to the MCI device associated with the MCIWnd window.
**
** hPalette - Palette handle.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::SetPalette(HPALETTE hPalette)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndSetPalette(m_hWnd, hPalette);
}


/******************************************************************************
**
** CMCIWnd::SetRepeat
**
** Sets the repeat flag associated with continuous playback.
**
** bRepeat - TRUE for continuous playback or FALSE otherwise.
**
** This function only affects playback that the user initiates by hitting the
** play button on the toolbar. It will not affect playback started with the 
** MCIWndPlay macro. Currently, MCIAVI is the only device that supports 
** continuous playback.
**
******************************************************************************/
void CMCIWnd::SetRepeat(BOOL bRepeat)
{
	ASSERT(::IsWindow(m_hWnd));
	MCIWndSetRepeat(m_hWnd, bRepeat);
}


/******************************************************************************
**
** CMCIWnd::SetSpeed
**
** Sets the playback speed of an MCI device.
**
** nSpeed - Playback speed. Specify 1000 for normal speed, larger values for 
**			faster speeds, and smaller values for slower speeds.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::SetSpeed(UINT nSpeed)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndSetSpeed(m_hWnd, nSpeed);
}


/******************************************************************************
**
** CMCIWnd::SetTimeFormat
**
** Sets the time format of an MCI device.
**
** strTimeFormat - Pointer to a buffer containing the null-terminated string 
**				   indicating the time format. Specify "frames" to set the time
**				   format to frames, or "ms" to set the time format to milliseconds.
**
** Returns zero if successful or an error otherwise.
**
** An application can specify time formats other than frames or milliseconds as
** long as the formats are supported by the MCI device. Noncontinuous formats, 
** such as tracks and SMPTE, can cause the trackbar to behave erratically. For 
** these time formats, you might want to turn off the toolbar by using the 
** ChangeStyles() function and specifying the MCIWNDF_NOPLAYBAR window style.
** If you want to set the time format to frames or milliseconds, you can also
** use the UseFrames() or UseTime() functions. 
**
** For a list of time formats, refer to MCIWndGetTimeFormat().
**
******************************************************************************/
long CMCIWnd::SetTimeFormat(LPCSTR strTimeFormat)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndSetTimeFormat(m_hWnd, strTimeFormat);
}


/******************************************************************************
**
** CMCIWnd::SetTimers
**
** Sets the update periods used by MCIWnd to update the trackbar in the MCIWnd 
** window, update the position information displayed in the window title bar, 
** and send notification messages to the parent window.
**
** nActive - Update period used by MCIWnd when it is the active window. The 
**			 default value is 500 milliseconds.
**
** nInactive - Update period used by MCIWnd when it is the inactive window. The
**			   default value is 2000 milliseconds. 
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
void CMCIWnd::SetTimers(UINT nActive, UINT nInactive)
{
	ASSERT(::IsWindow(m_hWnd));
	MCIWndSetTimers(m_hWnd, nActive, nInactive);
}


/******************************************************************************
**
** CMCIWnd::SetVolume
**
** Sets the volume level of an MCI device.
**
** nVolume - New volume level. Specify 1000 for normal volume level. Specify a
**			 higher value for a louder volume or a lower value for a quieter 
**			 volume.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::SetVolume(UINT nVolume)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndSetVolume(m_hWnd, nVolume);
}


/******************************************************************************
**
** CMCIWnd::SetZoom
**
** Resizes a video image according to a zoom factor. Adjusts the size of an 
** MCIWnd window while maintaining a constant aspect ratio.
**
** nZoom - Zoom factor expressed as a percentage of the original image. 
**		   Specify 100 to display the image at its authored size, 200 to 
**		   display the image at twice its normal size, or 50 to display the 
**		   image at half its normal size
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
void CMCIWnd::SetZoom(UINT nZoom)
{
	ASSERT(::IsWindow(m_hWnd));
	MCIWndSetZoom(m_hWnd, nZoom);
}


/******************************************************************************
**
** CMCIWnd::Step
**
** Moves the current position in the content forward or backward by a specified 
** increment.
**
** nSteps - Step value. Negative values step the device through the content in
**		    reverse. The units for the step value depend on the current time 
**			format.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::Step(long nSteps)
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndStep(m_hWnd, nSteps);
}


/******************************************************************************
**
** CMCIWnd::Stop
**
** Stops playing or recording the content of the MCI device associated with the 
** MCIWnd window.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::Stop()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndStop(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::UseFrames
**
** Sets the time format of an MCI device to frames.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::UseFrames()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndUseFrames(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::UseTime
**
** Sets the time format of an MCI device to milliseconds.
**
** Returns zero if successful or an error otherwise.
**
******************************************************************************/
long CMCIWnd::UseTime()
{
	ASSERT(::IsWindow(m_hWnd));
	return MCIWndUseTime(m_hWnd);
}


/******************************************************************************
**
** CMCIWnd::ValidateMedia
**
** Updates the starting and ending locations of the content, the current 
** position in the content, and the trackbar according to the current time 
** format.
**
******************************************************************************/
void CMCIWnd::ValidateMedia()
{
	ASSERT(::IsWindow(m_hWnd));
	MCIWndValidateMedia(m_hWnd);
}
