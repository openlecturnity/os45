#ifndef __VIDEOCAPTURER
#define __VIDEOCAPTURER
#pragma warning(disable:4786)

#define DEFAULT_VIDEO_BUFFERS    10
static int VIDEO_BUFFERS = DEFAULT_VIDEO_BUFFERS;

using namespace std;

class AVGRABBER_EXPORT VideoCapturer
{
protected:
   //static int VIDEO_BUFFERS;

	UINT driverId_;
	bool preview_;
	bool captureInProgress_;
   bool captureStartSuccess_;

	static int globalnID_;

	bool monitorDisplayed_;
	bool driverConnected_;

   LARGE_INTEGER performanceCounter_;
   LARGE_INTEGER performanceFrequency_;
   bool hasPerformanceCounter_;
   bool hasGoodTimer_;

	HWND hWnd_;
	HWND captureHWnd_;
	CAPDRIVERCAPS driverCaps_;
	CAPTUREPARMS  captureParms_;
	BITMAPINFO* videoFormat_;
	DWORD videoFormatSize_;

	// Number of us (micro-seconds) each frame should take
	TIME_US usecsPerFrame_; // frameRate_;
   float frameRate_;

	AudioCapturer* audio_;
	bool audioSync_;
	// bool audioFirstSyncDone_;

   // This is the last reference time that was returned
   long lastReturnedRefTime_;

   // more sync variables:
   DWORD ulFirstControlTime_;
   DWORD ulPrerollTime_;
   bool  bFirstControlDone_;
   bool  bAudioControlDone_;
   // This variable is a help offset for the calculation of the
   // audio/video time factor. It gives the offset between the
   // audio time counter and the real time counter relative
   // to the first time in the videoStreamCallback() method.
   // This is _not_ the real audio/video offset which is stored
   // in the audioVideoOffset_ member variable.
   long lInternalAvOffset_;

	// Time stamp of most recent frame written to file, in us
	TIME_US mostRecentFrameWritten_;
	// Time stamp of most recent video header received, in us
	TIME_US mostRecentTimeCaptured_;
	// Paused time in us
	TIME_US pausedTime_;

	// The initial offset between audio and video, in ms
	long audioVideoOffset_;

   bool useFixedAudioVideoOffset_;
   long fixedAudioVideoOffset_;

	// The interval, in number of frames, in which the sync
	// between audio and video is checked (or data loss).
	int checkSyncInterval_;

	// The "media threshold"; if, in audio sync mode, video deviates 
	// more than this value (in us) from audio, the streams are 
	// synchronized anew.
	TIME_US mediaThreshold_;

	// True if capturing is paused.
	bool isPaused_;
	// True if capturing is no longer paused, but the first frame after
	// the pause has not yet been processed.
	bool wasPaused_;

	// Time stamp (system time), in ms, of the first call of videoStreamCallback
	long firstTime_;

	// The number of frames processed
	int frameCounter_;

   // The number of frames written to the last open AVI file.
   // This is the same as dwTotalFrames of the AVI Stream header,
   // but after the recording has been stopped, aviStreamHeader_
   // is deleted and can no longer be accessed.
   int aviFileFrameCounter_;
	// The number of frames dropped, i.e. inserted amount of null frames
	int dropCounter_;
	int dropCounterAudio_;
	TIME_US lastStartTime_;
	TIME_US lastOurStartTime_;

	// The current audio/video factor, a(t) == v(t) * audioVideoFactor_
	double audioVideoFactor_;

	// True if video is to be tightly sync'ed with audio, may
    // change from false (advanced sync) to true (normal, tight sync)
	// during the capturing process, e.g. if the recording is paused,
	// or if an audio drop is detected.
	bool audioSyncMode_;

   // True if the advanced audio sync mode is to be used at all,
   // advanced audio sync is used if audioSyncMode_ is false; tight
   // sync'ing is done if audioSyncMode_ is true. This defaults
   // to false (no advanced audio sync mode), and audioSyncMode_
   // is set in the initStartVariables() method.
   bool useAdvancedAudioSyncMode_;

   // The byte quota for a single frame:
   long frameByteQuota_;
   // The accumulated byte quota
   long accFrameByteQuota_;

   // the maximum frame size in bytes for the codec.
   long maxFrameSize_;

   // the approximate file size
   DWORD totalSize_;

	HANDLE captureDoneEvent_;
	HANDLE captureInitEvent_;
	HANDLE prerollEvent_;
	bool prerollDone_;

	VideoFile* aviOutputFile_;
	// string videoFileName_;
	_TCHAR szFileName_[512];
	MainAVIHeader* aviHeader_;
	LecturnityAVIStreamHeader* aviStreamHeader_;

	CodecInfo codecInfo_;

	bool hasExceptionOccurred_;
	_TCHAR exceptionMsg_[512];
	//exception* initException_;

   bool hasCapErrorOccurred_;
   _TCHAR capErrorMsg_[512];

   // true if the pixel data need to be decompressed
   // to get RGB data (biCompression != BI_RGB)
   bool decompressForThumbnail_;
   bool thumbnailPossible_;
   bool enableThumbnails_;

   // RGB bitmap for drawing thumbnails
   BITMAPINFO* thumbnailInfo_;
   char*       thumbnailData_;
   HIC         thumbnailHic_;
   int         lastThumbnailFrame_;

   // this is the HWND used for registering the instance:
   HWND registeredHwnd_;

   // true if null frames should be inserted instead of
   // compressing the current frame
   bool bNullFramesMode_;

   /* private methods */
   virtual void initVariables();
   virtual void registerMonitorWndClass();

   virtual void fourCCToString(FOURCC fcc, char* fccs);

	virtual void launchInitThread();
	virtual void initCaptureWindow();
	virtual void destroy();
	virtual void freeMem();
	virtual void destroyCaptureWindow();

	virtual void initDriverCaps();
	virtual void setCaptureParms();
	virtual void dumpCaptureParms();

	virtual void updateVideoFormat();

   virtual void initStartVariables(_TCHAR *tszFileName, AudioCapturer *audio);
	virtual void initAviFile();
	virtual bool closeAviFile();

   virtual void enterAudioSyncMode();

   virtual void initThumbnailMaker();
   virtual void writeThumbnail(void* data, int cbSize, BITMAPINFO* pHeader, char* fileName);

   // Returns a reference time. In the normal implementation,
   // this method should return something like timeGetTime().
   // May differ, e.g. with DV sources. Unit is milliseconds.
   virtual long getRefTime();

   // Returns the current video stream time. This is the exact
   // stream time of the video stream at the exact moment,
   // not the last frame being processed in the videoStreamCallback.
   virtual long getVideoTime();

	static void __cdecl captureWindowLoop(LPVOID param);
	static LRESULT CALLBACK 
		VideoCapturer::MonitorWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void dumpVideoHeader(LPVIDEOHDR);
	static LRESULT CALLBACK videoStreamCallback(HWND hwnd, LPVIDEOHDR lpVHdr);
   LRESULT VideoCallback(HWND hwnd, LPVIDEOHDR lpVHdr);
	static LRESULT CALLBACK statusCallback(HWND, int, LPCTSTR);
	static LRESULT CALLBACK controlCallback(HWND hWnd, int nState);
   static LRESULT CALLBACK errorCallback(HWND hWnd, int nId, LPCTSTR lpsz);

   virtual void registerInstance(HWND hWnd);
   virtual void unregisterInstance();
   static VideoCapturer *retrieveInstance(HWND hWnd);

public:
	VideoCapturer(UINT deviceId=0, bool preview=true);
	virtual ~VideoCapturer();

	virtual void start(_TCHAR* tszFileName, AudioCapturer* audio=NULL);

	/*
	Call this method before stopping the capture process, and
	before stopping the audio capturing. This method updates the
	avi stream header and corrects the actual needed frame rate.

	You will only have to call this method if you have supplied
	an audio capturer at start().
	*/
	virtual void finalizeSync();

   /*
   Stops the capture process and closes the output AVI.
   */
	virtual void stop();

   /*
   Pauses the capturing. This method is synchronize-safe.
   */
	virtual void pause();

   /*
   Unpauses the capturing, if in pause mode. This method is
   synchronize-safe.
   */
	virtual void unPause();

   /*
   Returns the number of milliseconds the video started before
   the audio, if an AudioCapturer has been supplied at the start.
   Otherwise, this method will return 0.
   */
	virtual long getVideoAudioOffset();

   /*
   Use this method in orde to have the VideoCapturer always return
   a fixed value for the A/V offset returned by getVideoAudioOffset()
   */
   virtual void setUseFixedAudioVideoOffset(bool bFixedOffset, long nOffsetMs);

   /*
   Returns an estimation for the average data rate during the capture
   process. This value is definitely too high, so it may be better
   to use the getAverageDataRate() method once in a while during
   capturing also. This method returns -1 if the settings are not
   valid.

   Note: This value is codec dependant!
   */
   virtual long getEstimatedDataRate();

   /*
   Returns the average data rate of the recorded video stream,
   measured in bytes per second.
   */
   virtual long getAverageDataRate();

   /*
   Returns the number of bytes written to the AVI file.
   */
   virtual DWORD getFileSize();

   /*
   Returns the current length of the recorded video, in ms.
   */
   virtual unsigned long getVideoLengthMs();

   /*
   Returns the number of dropped frames during the capture process;
   includes the number of forward *and* backward dropped frames.
   */
   virtual int getDroppedFramesCount();

   /*
   Returns the total number of frames written to the AVI file.
   */
   virtual int getTotalFramesCount() { return frameCounter_; }

   /*
   Sets the frame rate to capture with. The method returns
   the number of micro-seconds (us) for a single frame.
   */
   virtual TIME_US setFrameRate(double frameRate);

   /*
   Returns the number of microseconds each frame should occupy.
   This corresponds to the frame rate set with setFrameRate().
   */
	virtual TIME_US getUSecsPerFrame();

   /*
   Determines whether to use the advanced synchronizing algorithm
   or a tight (and safe but somewhat stupid) synchronizing algorithm.
   Defaults to false.
   */
   virtual void SetUseAdvancedAudioSync(bool bAdvancedSync);


   /*
   Sets the codec to use for compression. This has to be a
   CodecInfo structure as filled in by the getCodecList() method.
   */
	virtual void setCodec(CodecInfo* codecInfo);

   /*
   Calls the configuration dialog of the video codec, if available.
   */
	virtual void configureCodec(HWND hWnd);

   /*
   Calls the information dialog of the video codec, if available.
   */
	virtual void aboutCodec(HWND hWnd);

   /*
   Checks the current video input/output and codec settings. The return
   value can be one of the following (declared in AviStructs.h):

     * VIDEOCAP_OK              : Everythings ok.
     * VIDEOCAP_WRONG_OUTPUT    : The output format is invalid (for this codec)
     * VIDEOCAP_WRONG_INPUT     : The input format is invalid (for this codec)
     * VIDEOCAP_CODEC_NOT_FOUND : The codec could not be found and/or opened
     * VIDEOCAP_NO_MEMORY       : Not enough memory for capture process
   */
	virtual int checkSettings();

   /*
   Returns true if the system has a multimedia timer that is able to
   measure time to a millisecond's exactness, false otherwise.
   */
   virtual bool hasGoodTimer();

   static int getDeviceListSize();
	static void getDeviceList(_TCHAR **atszDevices, int iMaxLength);

	static void dumpCodecList();

	/*
	 Fills the codecList with codec information. codecList
	 has to be an array of CodecInfo structures, and the VideoCapturer
	 has to be initialized.
	 */
	virtual void getCodecList(CodecInfo** codecList);
	virtual int getCodecListSize();

	virtual void displayMonitor(bool b=true);
	virtual void getVideoFormat(BITMAPINFO* videoFormat);

	virtual void getDriverCaps(CAPDRIVERCAPS* driverCaps);

	virtual bool hasFormatDialog();
	virtual void displayFormatDialog();

	virtual bool hasSourceDialog();
	virtual void displaySourceDialog();

	virtual bool hasDisplayDialog();
	virtual void displayDisplayDialog();

   /*
   Enables or disables the creation of thumbnails.
   */
   virtual void setEnableThumbnails(bool b=true);

   /*
   Returns true if thumbnails are created (only in thumbnail mode
   and only if possible).
   */
   virtual bool isThumbnailEnabled();

   /*
   Fills the given structure with data on the thumbnail buffer.
   */
   virtual void getThumbnailData(ThumbnailData* thumb);

   /*
   Sets or unsets the NULL frames mode of the VideoCapturer. Note:
   This method may only be used while capturing, otherwise the
   mode will be reset on start of the capture process. If set to
   true, every frame received from the capture device will be written
   as a NULL frame in the AVI file (empty frames).
   */
   virtual void setNullFrameMode(bool b);

   /*
   Sets the number of video buffers to request from the system.
   The real number of video buffers may be smaller than the
   requested number if there is not enough memory available.
   */
   static void setVideoBuffers(int videoBuffers) { VIDEO_BUFFERS = videoBuffers; }

   /*
   Returns the current number of video buffers to request from
   the system. See setVideoBuffers(int).
   */
   static int getVideoBuffers() { return VIDEO_BUFFERS; }

   /*
   Throws an exception with the suiting error message for video
   errors (ICERR_* constants).
   */
   static void throwVideoException(DWORD errorCode);
};

#endif