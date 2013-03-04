#ifndef __AVGRABBER
#define __AVGRABBER
#pragma warning(disable:4786)

#include "global.h"

/*
// Avoid including (all/too many) headers in every user of AvGrabber

#ifdef AVGRABBER_DLL
   #define AVGRABBER_EXPORT   __declspec(dllexport)
#else
   #define AVGRABBER_EXPORT   __declspec(dllimport)
#endif

class AudioCapturer;
class AudioMixer;
class AudioDevice;
class VideoCapturer;
class VideoDevice;
class VideoSettings;
class WdmCapturer;
struct WDM_INTERFACE;
typedef enum DVRES;
class ScreenCapturer;
class CPUUsageReader;
class ThumbnailData;
class CodecInfo;

#include "hwaccelmanager.h"
*/

class CLiveContext;
#define AV_NULL_ON_SG      0x00
#define AV_CONTINUE_ON_SG  0x01

class CAvgrabberDllApp : public CWinApp
{
public:
	CAvgrabberDllApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

namespace AVGRABBER {

    class AVGRABBER_EXPORT HotKey 
    {
    public:
        HotKey() {}
        ~HotKey() {}

        void Copy(HotKey &hotKey);

        CString hotkeyName;
        bool hasCtrl;
        bool hasShift;
        bool hasAlt;
        unsigned int vKey;
        CString vKeyString;
        int id;
    }; 

}

class AVGRABBER_EXPORT AvGrabber
{
private:
	AudioCapturer  *audio_;
	AudioMixer     *audioMixer_;
	VideoCapturer  *video_;
   WdmCapturer    *wdm_;
   ScreenCapturer *screen_;
   CPUUsageReader *cpuMeter_;
   HwAccelManager  hwAccelManager_;
   DWORD           dwLastAccelerationLevel_;
   bool            disableHwAcceleration_;

   long audioDataRate_;
   int  nSgVideoMode_;

	bool inProgress_;
	bool isPaused_;
   bool isGrabbingScreen_;
	bool displayMonitor_;
   bool enableMixer_;
   bool dryCapture_;
	_TCHAR szFileName_[512];
   char szLadInfo_[128];

	bool enableVideo_;

   // methods:
   void notifyVideoSgClipStart();
   void notifyVideoSgClipStop();

public:
   /*
   Constructor. Creates a new AvGrabber object.
   */
	AvGrabber(CLiveContext *pLiveContext = NULL);

   /*
   Destructor. Destroys the AvGrabber object. If you are using
   an audio mixer (AudioMixer instance), be sure to set your
   variable to NULL prior to deleting the AvGrabber: The
   AudioMixer will be destroyed here, too.
   */
	~AvGrabber();

	/*
	Starts the capturing process. Audio is always recorded as
	szFileName + ".wav", and if video is enabled, a video is
	recorded as szFileName + ".avi". The method returns after
	the capturing process has really been started, i.e. it is
	safe to call stop() directly after start().

	If capturing is already in progress, an exception is thrown.
	If anything goes wrong during the start, other exceptions
	may be thrown.
	*/
	void start(_TCHAR* tszFileName, char* szLadInfo = NULL);

	/*
	Returns a pointer to a character string containing the file
	name prefix given in the start(char*) method. This pointer is
	only valid for the time recording and/or until the AvGrabber
	instance is destructed.
	*/
	_TCHAR* getFileNamePrefix();

	/*
	Pauses the recording process. This method is synchronize-safe,
	i.e. after unpausing the capture process with unPause(), the 
	recorded sound and video files will still be synchrone.
	*/
	void pause();

	/*
	A call to this method unpauses the capturing process. If the
	recording is not in pause mode, an exception is thrown.
	*/
	void unPause();

	/*
	Stops the capturing process. If no capturing is in progress,
	an exception is thrown.
	*/
	void stop();

	/*
	Returns the current recording time, derived from the audio
	recording time (derived the actual number of written bytes into
	the audio file).
	*/
	long getRecordingTime();

   /*
   Returns an estimate of the current CPU usage, normed between
   0 and 100. If the function fails, this method returns -1,
   */
   int getCpuUsage();

   // ////////////////////////////////
   //
	//    A U D I O   M E T H O D S
   //
   // ////////////////////////////////

	/*
	Returns the number of audio devices on the system.
	*/
	int  getAudioDeviceCount();

	/*
	Fills the array of getAudioDeviceCount() AudioDevice structures
	(see there) with data about the found ausio devices. The structures
	have to be preallocated before calling this method. It is safe
	to free the structures directly after a call to this method.
	*/
	void getAudioDevices(AudioDevice* deviceList[]);

	/*
	Sets the current audio device. Before starting the capturing process,
	you must call this method at least once. If something goes wrong during
	initialization, an exception is thrown. An AudioMixer is returned,
	if available. If the use of an audio mixer has been switched off using
   setEnableMixer(), this method will always return NULL.
	*/
	AudioMixer* setAudioDevice(int nDeviceID, 
                              int sampleRate, 
                              int bitsPerSample, 
                              MIXERCALLBACK mixerCallback=NULL);
	
	/*
	Returns the current audio recording level as a number between 0 and 100.
	This number reflects what is currently recorded. My tip is: Any number
	that exceeds 50 means that the audio is too highly peaked.
	*/
	int getAudioLevel();

   /*
   Returns the audio data rate, measured in bytes per second. There exists
   only this method to retrieve the audio data rate, as the data rate is
   fix for audio data (is not compressed).
   */
   long getAudioDataRate();

   /*
   Returns the state of the audio grabbing process. Can be AUDIO_NORMAL if
   everything is ok, or AUDIO_SILENCE, if silence is detected for more than
   20 seconds.

   If audio is not realized or initialized, this method returns -1.
   */
   int getAudioState();

   /*
   Sets the amount of time (in milliseconds) after which the state (see also
   getAudioState()) goes from AUDIO_NORMAL to AUDIO_SILENCE if silence is
   detected on the line. Defaults to 30000 ms (30 seconds).
   */
   static void setAudioSilenceTimeout(int millis);

   /*
   Returns the audio silence timeout. See setAudioSilenceTimeout(int)
   */
   static int getAudioSilenceTimeout();

   /*
   If you do not want to use a mixer (too instable?), you may switch it
   off here. Call this method prior to setting an audio device.
   */
   void setEnableMixer(bool b);

   /*
   If you need mixer functions, you can get a pointer to the current mixer.
   */
   AudioMixer *getAudioMixer() {return audioMixer_;};


   // ////////////////////////////////
   //
	//    V I D E O   M E T H O D S
   //
   // ////////////////////////////////

   /*
	Enables or disables video capturing. By default, video is disabled.
	If capturing is in progress, an exception is thrown.
	*/
	void setEnableVideo(bool bEnable);

   /*
   Returns true if video recording is enabled, false otherwise.
   */
   bool getEnableVideo();

   ///////////////////////////////////
   //
   //   V F W   M E T H O D S
   //
   ///////////////////////////////////

	/*
	Returns the number of Video for Windows (VfW) video devices found 
   on this system.
	*/
	int  getVideoDeviceCount();
   void getVideoDeviceList(_TCHAR **aszList, int maxLength);

   void getVideoDeviceParameters(VideoDevice &videoDevice);

	/*
   Video for Windows only:

	Sets the video device to use for capturing purposes. You have to call this
	method at least once before starting a capture process with video enabled
    (Note: By default, video is disabled). You cannot retrieve or set any settings
	before you have set the video device once.
	*/
	void setVideoDevice(int nDeviceID);

   /*
   Video for Windows only:

   Deletes the current video device, if it exists. If it does not exist, then
   nothing happens. If capturing is in progress, an exception is thrown.
   */
   void deleteVideoDevice();

	/*
   Video for Windows only:

   Displays the video format dialog of the current video driver, if available.
	The video device has to have been set before calling this method.
	*/
	void displayVideoFormatDialog();

	/*
   Video for Windows only:

   Displays the video source dialog of the current video driver, if available.
	The video device has to have been set before calling this method.
	*/
	void displayVideoSourceDialog();

	/*
   Video for Windows only:

	Displays the video display dialog of the current video driver, if available.
	The video device has to have been set before calling this method.
	*/
	void displayVideoDisplayDialog();

   /*
   Video for Windows only:

   Set the number of video frame buffers to use. The default value is 10
   buffers, which correspond to about 0.4 seconds of video @ 25 fps.
   The more buffers you request, the samller is the risk of dropping frames,
   but the amount of memory used increases for each additional buffer.
   If you have lots of memory, you may increase this number.
   */
   void setVideoBufferSize(int bufferSize) { VideoCapturer::setVideoBuffers(bufferSize); }

   ///////////////////////////////////
   //
   //   W D M   M E T H O D S
   //
   ///////////////////////////////////

   /*
   WDM only:

   This method returns true if WDM capturing is enabled, false otherwise. Note
   that this does not mean that there are any WDM capture devices at the moment,
   it just means that WDM is supported by the operating system.
   */
   bool isWdmEnabled();

   /*
   WDM only:

   Returns the current number of WDM capture devices on the current system.
   This method fills the array that is returned by the getWdmDeviceNames()
   method. In order to refresh the list, simply call this method again, and
   the list will be recreated.

   If an error occurs, an exception is thrown.
   */
   int getWdmDeviceCount();

   /*
   WDM only:

   Returns a list of WDM devices on the current system. Simply calling this
   method will not refresh the list; you will need a call to the getWdm-
   DeviceCount() method in order to do that. Note that the number of capture
   devices may change!

   If an error occurs, an exception is thrown.

   Parameter: paDevices - An array of (previously allocated) char * pointers
                          which will be filled with the current video device
                          names.
              maxLen    - The minimum length of each char * entry in the array.
   */
   void getWdmDeviceNames(_TCHAR **paDevices, int maxLen);

   /*
   WDM only:

   Sets a WDM video device. This cannot be used if there already is an active
   VfW video device (set by setVideoDevice()).

   If an error occurs, an exception is thrown.

   Parameter: szDeviceName - The name of the device to set, as returned by the
                             getWdmDeviceNames() method.
   */
   void setWdmVideoDevice(_TCHAR *tszDeviceName);

   /*
   WDM only:

   Deletes the currently set WDM device. If no device is set, nothing happens.
   */
   void deleteWdmDevice();

   /*
   WDM only:

   Returns true if the currently set device is a DV (digital video) device.
   */
   bool isWdmDvDevice();

   /*
   WDM only:

   Returns the number of configuration interfaces found for the currently set
   WDM capture device (a capture device has to have been set in order to call
   this method).
   */
   int  getWdmInterfaceCount();

   /*
   WDM only:

   Returns the IDs and descriptions of the configuration interfaces for the
   currently set capture device.

   Parameters: paWdmInterfaces - An (preallocated) array of WDM_INTERFACE
                                 structures which will contain information on the
                                 configuration interfaces.
   */
   void getWdmInterfaces(WDM_INTERFACE *paWdmInterfaces);

   /*
   WDM only:

   Displays the specified configuration dialog. The hWnd parameter is used as
   the parent window of the configuration dialog. Note that you should only call
   this method for WDM_INTERFACE_IDs that were returned by the getWdmInterfaces()
   method.

   Parameters: wdmId - One of the interface ids returned by getWdmInterfaces().
               hWnd  - Parent window for the configuration dialog.
   */
   void displayWdmConfigurationDialog(WDM_INTERFACE_ID wdmId, HWND hWnd=NULL);

   /*
   WDM only:

   Sets the default DV parameters. These parameters are only affecting
   DV source filters. Set dvRes to the desired default DV resolution,
   if converted to RGB data. Set bRgb to true in order to convert the
   DV data to a 24 Bit RGB format; if bRgb is set to false, the dvRes
   variable is of no importance.
   */
   void setWdmDefaultDvParameters(DVRES dvRes=DVRES_HALF, bool bRgb=true);

   /*
   WDM only:
   
   Use this method in order to make the WDM Capturer convert DV
   (digital video) data to RGB data prior to passing the data to
   a codec. This setting only has effect if the source filter is
   a DV camera/DV VCR.
   */
   void setWdmConvertDvDataToRgb(bool bRgb=true);
   bool getWdmConvertDvDataToRgb();

   /*
   WDM only:
   
   Use this method in order to tell the WDM capturer to which 
   resolution the DV data is to be converted to RGB data. This
   may be one of the DVRES enumeration: DVRES_FULL to DVRES_EIGHTH.
   */
   void setWdmDvResolution(DVRES dvRes=DVRES_HALF);
   DVRES getWdmDvResolution();

   /*
   */
   void setWdmStreamConfigData(char *pAmMediaType, int cbAmMedia,
                               char *pVideoHeader, int cbVideoHeader);
   void getWdmStreamConfigData(char *pAmMediaType, int cbAmMedia,
                               char *pVideoHeader, int cbVideoHeader);
   int  getWdmStreamConfigMediaTypeSize();
   int  getWdmStreamConfigVideoHeaderSize();

   ///////////////////////////////////
   //
   //   GENERAL VIDEO METHODS
   //
   ///////////////////////////////////

   /*
	Fills the VideoSettings structure with the current data of the capture
	device. The video device has to have been set before calling this method.
	*/
	void getVideoSettings(VideoSettings &videoSettings);

	/*
	Enables or disables the video monitor. If it is enabled, a window with the
	video monitor will be popped up which can only be closed by using this
	method again.
	The video device has to have been set before calling this method.
	*/
	void setVideoMonitor(bool bDisplay);

	/*
	Returns the current settings for the frame rate, expressed as number of
	micro-seconds per frame (1 us = 1000 ms).
	The video device has to have been set before calling this method.
	*/
	long getVideoUSecsPerFrame();

	/*
	Sets the current frame rate. This number is a floating point number which
	expresses the number of frames per second. The long returned is the number
	of microseconds used per frame, as returned by getVideoUSecsPerFrame().
	The video device has to have been set before calling this method.
	*/
	long setVideoFrameRate(float reqFrameRate);

	/*
	After the capture process has finished, you can use this method to retrieve
	the inital video offset in milliseconds. This (positive) number is the number
	of milliseconds that needs to be cut off in front of the video in order to
	make audio and video synchrone. The video device has to have been set before 
	calling this method.
	*/
	long getVideoOffset();

   /*
   Use this method in orde to have the VideoCapturer always return
   a fixed value for the A/V offset returned by getVideoAudioOffset()
   */
   void setUseFixedAudioVideoOffset(bool bFixedOffset=false, long nOffsetMs=0);

   /*
   Returns an estimation of the data rate for the video stream, measured in
   bytes per second. This value will probably be much too big, so keep on
   measuring the video data rate during capturing (use getVideoAverageDataRate()).

   You must not use this method while capturing is in progress!
   */
   long getVideoEstimatedDataRate();

   /*
   Returns the average video data rate, measured in bytes per second. You may
   only use thís method if capturing is in progress, reversly to the
   getVideoEstimatedDataRate() method.
   */
   long getVideoAverageDataRate();

   /*
   Returns the number of dropped frames during the capture process, or zero if
   video is not captured at the moment.
   */
   int getVideoDroppedFramesCount();

   /*
   Returns the current size of the Video file. This method is only available
   if capture is currently in progress.
   */
   unsigned long getVideoFileSize();

   /*
   Returns the current length of the recorded video, in ms.
   */
   unsigned long getVideoLengthMs();

   /*
   Returns the number of video frame buffers used for capturing.
   */
   int getVideoBufferSize() { return VideoCapturer::getVideoBuffers(); }

   void AvGrabber::setEnableVideoThumbnail(bool b);

   bool getEnableVideoThumbnail();

   void getVideoThumbnailData(ThumbnailData* thumb);

   /*
   Determines whether to use the advanced synchronizing algorithm
   or a tight (and safe but somewhat stupid) synchronizing algorithm.
   Defaults to false.
   */
   virtual void setUseAdvancedAudioSync(bool bAdvancedSync);

   // ////////////////////////////////////////////////
   //
	//    S C R E E N   G R A B B E R   M E T H O D S
   //
   // ////////////////////////////////////////////////

   /* 
   Creates a ScreenCapturer object for use in this
   presentation recording. The codec is currently set to "divx" by default. 
   */
   void initScreenCapturer(CLiveContext *pLiveContext);

   /* 
   Destroys the current ScreenCapturer instance.
   */
   void destroyScreenCapturer();

	/*
	Returns a pointer to the current screen capturer
	*/
	inline ScreenCapturer *GetScreenCapturer(){return screen_;};

	/*
	Sets the screen capturer with the one specified
	*/
	//void SetScreenCapturer(ScreenCapturer &screenC){screen_ = screenC;};

   /* Lets the user choose a capture area interactively. Returns
   true if everything is ok. If it returns false, the user may
   have cancelled the area selection. You should not start recording
   in that case. */
   bool chooseSgCaptureArea(bool isStandAlone=false);

   /*
   Sets the cacture area to the given RECT.
   */
   bool setSgCaptureRect(RECT* pRect);

   /*
   Get the cacture area to the given RECT.
   */
   bool getSgCaptureRect(RECT* pRect);

   /*
   Enables or disables the SG quick capture (QC) mode. In QC mode, only
   one frame per second is actually captured from the primary display;
   the rest are only copied. The mouse cursor and the click visualizations
   are copied onto the buffer in any case. This may dramatically improve
   capture performance, but may render unwanted results.
   */
   void setSgQuickCaptureMode(bool b=false);

   /*
   Sets the requested screen grabbing frame rate. Note: This
   setting only has effect if the automatic frame rate control
   is not enabled. Returns the frame time in us (=10^6/reqFrameRate).
   */
   long setSgFrameRate(float reqFrameRate);

   /*
   Specifies the use of automatic frame rate adaption for the
   SG mode. Setting fRate to 10 for width x height at 320 x 240
   adapts all other sizes according the number of pixels.
   */
   void setSgAutoFrameRate(bool bEnable=true, int nWidth=320, 
                           int nHeight=240, float fRate=10.0, 
                           float fMaxRate=10.0, float fMinRate=1.0);

   /*
   Returns the length of the currently recorded SG clip, in ms.
   Otherwise, this method returns 0.
   */
   unsigned long getSgClipLengthMs();

   void WaitClickElementInfo(CSingleProgressDialog *pProgress);

   /* Starts a screen grabbing clip. It is recorded into the
   file with the filename szFileName (it is an AVI file) using
   the current codec settings. */
   void startScreenGrabbingClip(_TCHAR *szClipName, bool bCaptureActions, const _TCHAR *szAudioName=NULL, const char *szLadInfo=NULL);

   /* Stops the current screen grabbing clip recording. */
   void stopScreenGrabbingClip();

   /* Returns the start time of the clip in the current presentation.
   This method can only be called while a Screen Capturer exists,
   after or while a clip is recorded, and until the next clip is
   commenced. */
   long getScreenGrabbingClipStartTime();

   /* Returns the average data rate of the currently recorded screen
   grabbing clip, if available. Otherwise it returns 0. */
   long getSgAverageDataRate();

   /*
   Returns the number of dropped frames during the current screen
   grabbing session.
   */
   int getSgDroppedFramesCount();

   /*
   Returns the total number of frames written to file during the
   grabbing session.
   */
   int getSgTotalFramesCount();

   /*
   Sets the SG/Video interaction mode. Depending on this mode, the
   video capturer chooses a different action when SG clip capturing
   starts. Choose from:
     - AV_NULL_ON_SG: Null frames are inserted into AVI file until
           SG clip has finished (default).
     - AV_CONTINUE_ON_SG: Video capturing continues as normal even
           if an SG clip is being recorded (not recommended)
   */
   void setSgVideoMode(int nMode);

   /*
   Loads the given WAV file as click noise for the screen grabbing clips.
   Supply NULL to turn off click noise. Returns true if everything went
   fine. False may be returned in the following cases:
     - WAV file was not in PCM format
     - WAV file was not found or corrupt
     - AudioCapturer has not been initialized (see setAudioDevice())
     - Audio file too long.
   */
   bool loadSgClickNoise(_TCHAR *szWavFileName=NULL);

   /*
   Activates the annotation mode. Any other mode which is
   currently active will be deactivated.
   */
   void setSgAnnotationMode();

   /*
   Activates the interaction mode. Any other mode which is
   currently active will be deactivated.
   */
   void setSgInteractionMode();

   /*
   Selects the currently active annotation tool. May be one of
   the tools in the tools.h file.

   Note: This method will not activate the annotation mode if
   it is not already active. This has to be done separately.
   */
   void setSgAnnotationTool(int nTool);

   void setSgAnnotationPenColor(Gdiplus::ARGB color);
   void setSgAnnotationFillColor(Gdiplus::ARGB color);
   void setSgAnnotationLineWidth(int lineWidth);
   void setSgAnnotationLineStyle(int lineStyle);
   void setSgAnnotationArrowStyle(int arrowStyle);
   void setSgAnnotationArrowConfig(int arrowConfig);
   void setSgAnnotationFont(LOGFONT *pLogFont);
   void setSgAnnotationIsFilled(bool b=true);
   void setSgAnnotationIsClosed(bool b=true);

   /*
   Setting: Set to true if the flashing frame around the capture are should
   be hidden if it is panned. This minimizes problems with left-over frame
   parts in the SG clips. Defaults to "false".
   */
   void setSgHideWhilePanning(bool b=true);

   /*
   Setting: If set to true, all annotations are deleted after switching
   to interaction mode. This means that the next time the annotation mode
   is used, there are no visible annotation at the start. Otherwise, old
   annotations are not deleted before reentering the annotation mode.
   Defaults to "true".
   */
   void setSgDeleteAllOnInteractionMode(bool b=true);

   /*
   Sets the number of milliseconds used for the animation effect (rings) for
   visualizing the mouse clicks. Defaults to 1000.  The maximum ellipse 
   diameter may also be set. This value defaults to 14.
   */
   void setSgClickEffectFrames(int time=DEFAULT_ANIM_MS,
                               int maxSize=DEFAULT_ANIM_MAXSIZE);

   /*
   Enables or disables the click visualization effects. You may enable or
   disable the audiovisual and/or visual effects. Both settings default to true.
   */
   void setSgEnableClickEffects(bool audioEffects=true, 
                                bool videoEffects=true);

   /*
   
   */
   void setSgBlockUntilClickInfoResponse(bool bBlockUntilClickInfoResponse);

   /*
   Undos the last SG annotation action, if available. 
   The time stamp of this action may be retrieved with the 
   getLastSgUndoTimeStamp() method.
   */
   void sgAnnotationUndo();

   /*
   Retrieves the last undo time stamp for action in the SG 
   annotation mode. Returns -1 if not applicable.
   */
   long getSgAnnotationLastUndoTimeStamp();

   /*
   Redos the last undo action in the SG annotation mode, if available.
   The time stamp of this action may be retrieved with the 
   getNextSgRedoTimeStamp() method.
   */
   void sgAnnotationRedo();

   /*
   Retrieves the next redo time stamp for action in the SG 
   annotation mode. Returns -1 if not applicable.
   */
   long getSgAnnotationNextRedoTimeStamp();
   
   // ////////////////////////////////////////
   //
	//    D A T A   R A T E   M E T H O D S
   //
   // ////////////////////////////////////////

   /*
   Returns a coarse estimation of the data rate during the capture process.
   This method must *not* be used while the capturing is in progress. If you
   need the current data rate, use the getAverageDataRate() method. This data
   rate is the sum of the audio and the video data rate. It is measured in
   bytes per second.

   If you call this method while in progress, the current average data rate
   (getAverageDataRate()) is returned.
   */
   long getEstimatedDataRate();

   /*
   Returns the current average data rate for both audio and video (sum). It
   is measured in bytes per second. You must not use this method if capturing
   is not in progress. In that case, use the getEstimatedDataRate() method.
   */
   long getAverageDataRate();

   // ////////////////////////////////
   //
	//    C O D E C   M E T H O D S
   //
   // ////////////////////////////////

	/*
	Returns the number of applicable codecs for the current video settings. After
	any settings have changed, this number is obsolete. Changes can occur by calling
	any of the displayVideo...Dialog() methods. This is due to the fact that not
	all video codecs can handle every video size and/or color depth. E.g., the DIVX
	codec cannot handle 15 Bit color depth, but only 24 Bit.
    The video device has to have been set before calling this method.
	*/
	int  getCodecCount();

   /*
   Returns the number of codecs for the ScreenCapturer. See above (getCodecCount).
   */
   int  getSgCodecCount();

	/*
	Fills the array of CodecInfo structures with information about the video codecs
	fitting the current video settings. As with getCodecCount(), any changes to the
	video settings obsolete these codecs.
	The video device has to have been set before calling this method.
	*/
	void getCodecs(CodecInfo* codecList[]);

   /*
   Fills the array with the available codecs for the ScreenCapturer. Like getCodecs().
   */
   void getSgCodecs(CodecInfo* codecList[]);

	/*
	Sets the video codec to use for capturing video. This should be one of the codecs
	returned by the getCodecs() method. You may alter the information on the CodecInfo
	structure in order to customize the video capture process.
    The video device has to have been set before calling this method.
	*/
	void setVideoCodec(CodecInfo* codec);

   /* 
   Sets the codec to use with screen grabbing. Defaults to "divx" at the moment.
   */
   void setScreenGrabberCodec(CodecInfo* codec);

	/*
	Displays an about dialog of the currently set codec (by using setVideoCodec()), if
	available. The video device and a valid codec has to have been set before calling 
	this method.
	*/
	void displayCodecAboutDialog(HWND hParent);

	/*
	Displays a configuration dialog of the currently set codec (by using setVideoCodec()), if
	available. The video device and a valid codec has to have been set before calling 
	this method.
	*/
	void displayCodecConfigureDialog(HWND hParent);

   /*
	Displays an about dialog of the currently set codec (by using setScreenGrabberCodec()), if
	available. The screen grabber and a valid codec has to have been set before calling 
	this method.
	*/
   void displaySgCodecAboutDialog(HWND hParent);

   /*
	Displays a configuration dialog of the currently set codec (by using setScreenGrabberCodec()), if
	available. The screen grabber and a valid codec has to have been set before calling 
	this method.
	*/
   void displaySgCodecConfigureDialog(HWND hParent);

   /*
   Enables/disables disabling video hardware acceleration on SG clips.
   Defaults to false.
   */
   void setSgDisableVideoAcceleration(bool b=false);

   /*
   Returns the current state of disabling the video hardware acceleration
   on SG clips.
   */
   bool getSgDisableVideoAcceleration();

   // ////////////////////////////////////////
   //
	//    A N N O T A T I O N   M E T H O D S
   //
   // ////////////////////////////////////////

   /*
   Activates the annotation mode. Any other mode which is
   currently active will be deactivated.
   */
   void setAnnotationMode();

   /*
   Activates the interaction mode. Any other mode which is
   currently active will be deactivated.
   */
   void setInteractionMode();

   /*
   Selects the currently active annotation tool. May be one of
   the tools in the tools.h file.

   Note: This method will not activate the annotation mode if
   it is not already active. This has to be done separately.
   */
   void setAnnotationTool(int nTool);

   /*
   Setting: If set to true, all annotations are deleted after switching
   to interaction mode. This means that the next time the annotation mode
   is used, there are no visible annotation at the start. Otherwise, old
   annotations are not deleted before reentering the annotation mode.
   Defaults to "true".
   */
   void setDeleteAllOnInteractionMode(bool b=true);

   // ////////////////////////////////////////
   //
	//    S T A T I C   U T I L I T Y   M E T H O D S
   //
   // ////////////////////////////////////////

   /*
   Returns the length of the given video file in milliseconds,
   or -1 if failed.
   */
   static int staticGetVideoLength(_TCHAR *szVideoFile);
};

#endif
