#ifndef __SCREENCAPTURER
#define __SCREENCAPTURER

#include "FileOutput.h"                 // filesdk
#include "ThreadStore.h"                // lsutl32
#include "LiveContextCommunicator.h"    // lsutl32
#include "SingleProgressDialog.h"       // lsutl32

class CLiveContext;                     // lsutl32
class ILiveContextElementInfo;          // lsutl32

#define MODE_INTERACTION   0x00
#define MODE_ANNOTATION    0x01

#define MAX_CLICKS         10

#define DEFAULT_ANIM_MS          1000
#define DEFAULT_ANIM_MAXSIZE     14

typedef struct
{
    int nWidth;
    int nHeight;
    float fDesiredRate;
    float fMinRate;
    float fMaxRate;
} ADAPTFRAMERATEDATA;

typedef struct
{
    long  timeStamp;
    long  frameCount;
    DWORD mouseFlags;
    POINT clickPos;
} CLICKDATA;

class LcMessageString {
public:
    LcMessageString();
    LcMessageString(char *szMessage, DWORD dwRead);
    ~LcMessageString();
public:
    char *GetMessage() {return m_szMessage;}
    DWORD GetCount() {return m_dwRead;}

private:
    int m_iBufferSize;
    char *m_szMessage;
    DWORD m_dwRead;
};

class SrMouseData {
private:
    BYTE *m_pMouseBgraData;
    int   m_iMouseDataLength;
    POINT m_ptPositionInVideo;
    POINT m_ptCursorSize;

public:
    SrMouseData();
    virtual ~SrMouseData();

    void UpdateData(RECT *prcRecordArea);
    int  WriteData(BYTE *pBufferEnd);

};

class SrClickData {
private:
    POINT m_aptPositionInVideo[MAX_CLICKS];
    bool  m_abIsLeftMouseButton[MAX_CLICKS];
    int   m_aiRadius[MAX_CLICKS];

    int   m_iCurrentClickCount;

public:
    SrClickData();
    virtual ~SrClickData();

    void UpdateData(CLICKDATA *aCurrentClicks, 
        int iCurrentClickCount, int iCurrentTimeMs, 
        int iAnimationMaxRadius, int iAnimationDurationMs, RECT *prcVideoOnScreen);
    int  WriteData(BYTE *pBufferEnd);

};


class AVGRABBER_EXPORT ScreenCapturer : public VideoCapturer, IMouseHookListener
{
public:
    /* Constructor */
    ScreenCapturer(_TCHAR *tszFcc=NULL, VideoCapturer *pVideoCapturer=NULL, 
        CLiveContext *pLiveContext = NULL);

    /* Destructor */
    ~ScreenCapturer();

public:
    /* Sets the screen portion to be captured. If the RECT
    * is not correct (too large, etc.), it will be corrected.
    *
    * This method returns true if the new capture rect was
    * successfully set, otherwise false. 
    */
    virtual bool setScreenCaptureRect(RECT* pRect);

    /* returns the screen portion to be captured. */
    virtual bool getScreenCaptureRect(RECT* pRect);

    /* This method lets the user choose an input area interactively
    * by using the mouse. Single click selects a window, drag and drop
    * to select a custom capture rectangle. 
    */
    virtual bool chooseCaptureRect();

    /* See VideoCapturer */
    virtual void start(_TCHAR *tszFileName, AudioCapturer *audio, bool bStartPaused, bool bCaptureActions);

    /* See VideoCapturer */
    virtual void stop();

    /* Enables or disables the automatic setting of the frame rate.
    * Usage: The given width, height and frame rate are used to inter/extrapolate
    * a suitable frame rate based on the number of pixels to be grabbed. You
    * may also supply a maximal and minimum frame rate. The defaults are: 
    * 320x240 is captured at 10 fps, maximum frame rate is 10 fps, minimum
    * 1 fps. 
    */
    virtual void setUseAutoFrameRate(bool bEnable=true, int nWidth=320, int nHeight=240, float fRate=10.0, float fMaxRate=10.0, float fMinRate=1.0);

    /* This method overrides the setFrameRate of the VideoCapturer.
    * This method only has effect on any SG clips if setUseAutoFrameRate
    * is set to false (default). 
    */
    virtual TIME_US setFrameRate(double frameRate);

    /* Not supported, does nothing. */
    virtual void getDriverCaps(CAPDRIVERCAPS *pDriverCaps) {}

    /* Not supported, does nothing. */
    virtual void displayMonitor(bool b=true) {}

    /* Returns false, does not have source dialog. */
    virtual bool hasSourceDialog() { return false; }

    /* Returns false, does not have format dialog. */
    virtual bool hasFormatDialog() { return false; }

    /* Returns false, does not have display dialog. */
    virtual bool hasDisplayDialog() { return false; }

    /* This method is not supported for the ScreenCapturer. */
    virtual void setEnableThumbnails(bool b=true) { VideoCapturer::setEnableThumbnails(false); }

    /* Returns false, thumbnails are not supported. */
    virtual bool isThumbnailEnabled() { return false; }

    /* Does nothing, not supported. */
    virtual void getThumbnailData(ThumbnailData *thumb) {}

    /*
    * Activates the annotation mode. Any other mode which is
    * currently active will be deactivated.
    */
    virtual void setAnnotationMode();

    /*
    * Activates the interaction mode. Any other mode which is
    * currently active will be deactivated.
    */
    virtual void setInteractionMode();

    /*
    * Selects the currently active annotation tool. May be one of
    * the tools in the tools.h file.
    * 
    * Note: This method will not activate the annotation mode if
    * it is not already active. This has to be done separately.
    */
    virtual void setAnnotationTool(int nTool);
    /*
    * The following functions sets the parameters 
    * which should be used for annotation
    */
    virtual void setAnnotationPenColor(Gdiplus::ARGB color);
    virtual void setAnnotationFillColor(Gdiplus::ARGB color);
    virtual void setAnnotationLineWidth(int lineWidth);
    virtual void setAnnotationLineStyle(int lienStyle);
    virtual void setAnnotationArrowStyle(int arrowStyle);
    virtual void setAnnotationArrowConfig(int arrowConfig);
    virtual void setAnnotationFont(LOGFONT *pLogFont);
    virtual void setAnnotationIsFilled(bool b=true);
    virtual void setAnnotationIsClosed(bool b=true);

    /*
    * Undos the last SG annotation action, if available. 
    * The time stamp of this action may be retrieved with the 
    * getLastSgUndoTimeStamp() method.
    */
    virtual void annotationUndo();

    /*
    * Retrieves the last undo time stamp for action in the SG 
    * annotation mode. Returns -1 if not applicable.
    */
    virtual long getAnnotationLastUndoTimeStamp();

    /*
    * Redos the last undo action in the SG annotation mode, if available.
    * The time stamp of this action may be retrieved with the 
    * getNextSgRedoTimeStamp() method.
    */
    virtual void annotationRedo();

    /*
    * Retrieves the next redo time stamp for action in the SG 
    * annotation mode. Returns -1 if not applicable.
    */
    virtual long getAnnotationNextRedoTimeStamp();


    /*
    * Setting: If set to true, all annotations are deleted after switching
    * to interaction mode. This means that the next time the annotation mode
    * is used, there are no visible annotation at the start. Otherwise, old
    * annotations are not deleted before reentering the annotation mode.
    * Defaults to "true".
    */
    virtual void setDeleteAllOnInteractionMode(bool b=true);

    /*
    * Setting: Set to true if the flashing frame around the capture are should
    * be hidden if it is panned. This minimizes problems with left-over frame
    * parts in the SG clips. Defaults to "false".
    */
    virtual void setHideWhilePanning(bool b=true);

    /*
    * Sets the number of milliseconds used for the animation effect (rings) for
    * visualizing the mouse clicks. Defaults to 1000.  The maximum ellipse 
    * diameter may also be set. This value defaults to 14.
    */
    virtual void setClickEffectFrames(int time=DEFAULT_ANIM_MS,
        int maxSize=DEFAULT_ANIM_MAXSIZE);

    /*
    * Returns the currently set animation effect parameters. See above
    * (setClickEffectFrames) for an explanation.
    */
    virtual void getClickEffectFrames(int *time, int *maxSize);

    /*
    * Enables or disables the click visualization effects. You may enable or
    * disable the audiovisual and/or visual effects. Both settings default to true.
    */
    virtual void setEnableClickEffects(bool audioEffects=true, 
        bool videoEffects=true);

    /*
    * Enables or disables the quick capture mode.
    */
    virtual void setQuickCaptureMode(bool b=false);

    /*
    * Returns true if quick capture mode is enabled, otherwise false.
    */
    virtual bool getQuickCaptureMode();
    /*
    * See if the user manually changed the screen capture interactively. (using chooseCaptureRect)
    */
    bool GetIsCustomSet(){return bIsCustomSet;};
    /*
    * Sets if user manually changed the screen capture interactively. (using chooseCaptureRect)
    */
    void SetIsCustomSet(bool isCustomSet){bIsCustomSet = isCustomSet;};

    HWND GetAnnotationWindow();

    CLiveContext *GetLiveContext() {return m_pLiveContext;}

    /*
     * If screen recording was called from LIVECONTEXT, 
     * and click information was requested,
     * wait until threads are finished
     */
    void WaitClickElementInfo(CSingleProgressDialog *pProgress);

    /*
    * Virtual functions derived from iMouseHookListener
    */
    virtual void MouseMoved(POINT ptMouse, DWORD dwFlag);
    virtual void MouseDown(POINT ptMouse, DWORD dwFlag);
    virtual void MouseUp(POINT ptMouse, DWORD dwFlag);
    virtual void MouseDblClick(POINT ptMouse, DWORD dwFlag);
    virtual void SetPanningMode(bool bDoPanning);

    void ParseElementInfo(const char *szMessage, DWORD dwRead);

private:
    // static functions

    /* WndProc for the frame to display capture area. */
    static LRESULT CALLBACK FrameWindowProc(HWND hWnd, UINT uMsg,
                                            WPARAM wParam, LPARAM lParam);

    /* This is the window thread for the frame for displaying the capture
     * area while capturing. 
     */
    static void __cdecl FrameWindowThread(void *lpData);

    /* This is the start proc of the thread which consequently grabs
     * the screen. 
     */
    static void __cdecl GrabberThreadLauncher(void *lpData);

    /* This is the start proc of the thread which calls the videoStreamCallback
     * and compresses the grabbed data. 
     */
    static void __cdecl CompressorThread(void *lpData);

    /* This thread reads the replies sended from LIVECONTEXT
     * and distribute them.
     */
    static void __cdecl LcClickRequestThread(void *lpData);

private:
    /* This method registers the WNDCLASS for the display frame */
    bool registerFrameWndClass();

    void frameWindowLoop();

    void GrabberThread();
    /* Grabs the screen with the current settings (rect,...) into
     * the given character array. The time stamp comes either from the
     * audio capturer, or is the current system time. This is used
     * for timing purposes for the click effects. 
     */
    void grabScreen(unsigned char *lpData, DWORD timeStamp, SrMouseData *pMouse, SrClickData *pClickData);

    /* Updates the screen format (bit depth, pixel format) */
    void updateScreenFormat(int width=320, int height=240);

    /* Call this to start the mouse panning mode. The capture window
    * will follow the mouse relatively. 
    */
    virtual void startMousePan();

    /* This method is called if we're in mouse pan mode. */
    void doMousePan();

    /* Call this to end mouse pan mode. See startMousePan(). */
    virtual void endMousePan();

    /* Initializes the VIDEOHDR structures for the video buffers. */
    void initVHDRs();

    /* Deletes the VIDEOHDR structures for the video buffers. */
    void deleteVHDRs();

    /* Returns an HCURSOR handle to the current mouse cursor. 
     * The current mouse position is written into the pt POINT 
     * variable. 
     */
    HCURSOR getCurrentCursorHandle(POINT &pt, POINT *ptMouseHotPos);

    /* Displays or hides the flashy fancy cpature area window. */
    void displayCaptureFrame(bool bDisplay=true);
    
    long WriteClickActionEntry(POINT &ptMouse, DWORD dwMouseFlags, DWORD dwActionFlags);
    void WriteMoveActionEntry(POINT &ptMouse);
	void WriteRemoveTeleEntry(POINT &ptMouse);
    
    /*
    * Return true if the mouse position is inside selection rectangle
    */
    bool MousePosInSelectionRect(POINT &ptMouse);

    void ReportElementInfo(CLiveContextElementInfo *pInfo);

    void RequestElementInfoFromLiveContext(long lTimestamp, POINT &ptMouse);

    bool UseMouseClick(POINT &ptMouse);
    bool IsLiveContextConnected();

    void EmptyLcElementInfoMap();

protected:
    virtual long getRefTime();      // see VideoCapturer.h
    virtual long getVideoTime();    // see VideoCapturer.h

private:
    // Pointer needed for Gdiplus startup and shutdown
    ULONG_PTR m_pGdiplus;

    /* Pointer to an eventual VideoCapturer */
    VideoCapturer *pVideoCapturer_;

    /* Reference to the livecontext instance */
    CLiveContext *m_pLiveContext;

    CThreadStore *m_pThreadStore;

    /* Physical width of screen */
    int screenWidth_;
    /* Physical height of screen */
    int screenHeight_;

    /* This is the rect of the screen being captured. */
    RECT captureRect_;

    /* The array for the video header buffers. These contain
     * the buffers for the video data. 
     */
    VIDEOHDR **paVideoHdr_;
    QCDATA **paQcData_;
    SrMouseData **paMouseData_;
    SrClickData **paClickData_;

    CLSID *pPngClsid;
    bool m_bIsLsgc;

    /* The time stamp of the first frame grabbed, in ms. */
    DWORD dwFirstTimeMs_;

    /* The current buffer into which is being grabbed. */
    int nCurrentGrabBuffer_;

    /* The buffer currently being compressed. */
    int nCurrentCompressBuffer_;

    /* This event handle is used for notifications between the
     * GrabberThread and CompressorThread procedures. 
     */
    HANDLE hBufferEvent_;

    /* Before the grabber thread can start, the window thread must habe
     * create the window handle. This events prevents the grabber thread
     * from starting too early. 
     */
    HANDLE hWindowCreatedEvent_;

    HANDLE m_hFileSgActionsEvent;

    bool m_bLcReceiveMessageStopped;
    bool m_bStopLcCommunication;

    /* If stop() has been called, this member is set to true
     * until the capture process has stopped. 
     */
    bool bStopRequested_;

    /* The HWND of the flashing frame around the capture rect. */
    HWND hFlashWnd_;

    /* The current color of the frame window. */
    COLORREF cFrameColor_;
    bool bFrameColorIsBlack_;

    /* This is the handle to the DLL handling the mouse and
     * keyboard hooks. 
     */
    HINSTANCE hHookDll_;

    /* This is a handle to the global mouse hook */
    HHOOK hMouseHook_;

    /* Click data for the click effect. */
    CLICKDATA clickData_[MAX_CLICKS];
    int m_iActiveClickCount;

    /*
     * number of frames to animate a click on and increment 
     * radius of ellipse effect
     */
    int clickAnimMs_;
    int clickAnimMaxSize_;
    HPEN hRedPen_;
    HPEN hBluePen_;
    bool doVideoClickEffect_;
    bool doAudioClickEffect_;

    /* This variable is true if we're in mouse pan mode. */
    bool m_bIsPanningActive;

    /* This is the position of the mouse at the start of panning mode. */
    POINT panStartMouse_;
    /* This is the position of the window at the start of panning mode. */
    RECT panStartWnd_;

    /* Set to true if you want to use mouse click icons. */
    bool bClickIconEnabled_;

    /* The image representing a mouse click */
    HICON hClickIcon_;
    HICON hRClickIcon_;

    /* Unequal zero if a click image should be inserted at the next frame */
    DWORD dwInsertClickIcon_;

    /* The last set frame rate (using setFrameRate()). */
    float lastSetFrameRate_;

    /* True if automatic frame rate adaption is to be used. */
    bool useAutoFrameRate_;

    /* This structure contains data for the automatic frame rate
     * adaption algorithm. 
     */
    ADAPTFRAMERATEDATA autoFrameRateData_;

    /* This is true if the user is currently dragging the capture window. */
    bool isDragging_;

    /* This is the point of the cursor where a drag action started. */
    POINT dragStartPoint_;

    /* This is the position of the capture window as a drag action started. */
    RECT dragStartWndPos_;

    /* This variable is used to see whether the grabber thread
    * has really stopped after a stop() call. 
    */
    bool           bGrabThreadStopped_;

    // This is the current screen bitmap
    HBITMAP        hbitmap_;
    // This is the DC for the current screen bitmap
    HDC            hdcBitmap_;

    // This is the internal frame counter of the grabber thread.
    long           nFrame_;
    long           getCurrentFrame() { return nFrame_; }

    /* This variable is used to see whether the compressor thread
    * has really stopped after a stop() call. 
    */
    bool           bCompressThreadStopped_;

    /* True if quick capture mode is enabled */
    bool           bQuickCaptureMode_;
    int            qcFrameInterval_;
    int            qcLastCompleteFrame_;
    HBITMAP        hQcBackup_;
    HDC            hQcBackupDc_;
    CRectKeeper   *pChangedRectsNow_;
    CRectKeeper   *pChangedRectsLast_;
    bool           bQcNothingChanged_;
    bool           bQcLastDrewClicks_;
    int            qcLastMousePosX_;
    int            qcLastMousePosY_;
    /* true if the user manually choosed a screen capture */
    bool				bIsCustomSet;

    /*
    * ANNOTATIONS
    */

    /* A draw window for the annotations: */
    CNewDrawWindow   *m_pDrawWindow;

    // The currently selected annotation tool (see tools.h)
    int nAnnoTool_;

    // the current mode, either MODE_ANNOTATION or MODE_INTERACTION.
    int nCurrentMode_;

    // if true, annotations are deleted after entering interaction mode
    bool bDeleteOnInteractionMode_;

    // if true, the flashy frame is hidden while panning
    bool           bHideWhilePanning_;

    // this method is called if the mode changes from interaction to annotation
    // mode
    void enterAnnotationMode();

    // this method is called if the mode changes from annotation to interaction
    // mode
    void enterInteractionMode();

    //
    // CLICK NOISE
    //

    // Inserts a click noise into the audio stream (if available)
    void insertClickNoise(POINT &ptMouse, DWORD clickFlags);

    // Resets the click visualization frame counter.
    void resetClickFrameCounter();

    CFileOutput *m_pFileSgActions;

    /* Save window content as image */
    CString m_csImageFilepath;
    
    /* Variable to notice if the window is in panning mode */ 
    bool m_bDoPanning;

    /* Variables for Demo mode */
    long m_lLastMouseMove;
    bool m_bIsDragging;
    int m_iLastX;
    int m_iLastY;

    CDebugLog *m_pDebugLog;

    CMap<UINT, UINT, CLiveContextElementInfo *, CLiveContextElementInfo *> m_mapLcElementInfo;
};

#endif