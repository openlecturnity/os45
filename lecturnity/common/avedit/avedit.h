#if !defined(__AVEDIT__H__)
#define __AVEDIT__H__


#include <dshow.h> // AM_MEDIA_TYPE
#include <qedit.h> // IAMTimeline, ...
#include <atlbase.h>
#include "avinfo.h"
#include "skewclock.h"

#ifndef __STREAMS__
#include "streams.h"
#endif

class CWavDestFilter;

#define WM_AVEDIT (WM_USER + 1)

#define STRETCH_NOPRESERVE  RESIZEF_STRETCH
#define STRETCH_CROP        RESIZEF_CROP
#define STRETCH_ASPECTRATIO RESIZEF_PRESERVEASPECTRATIO
#define STRETCH_ASPECTRATIO_NOLETTERBOX RESIZEF_PRESERVEASPECTRATIO_NOLETTERBOX

enum
{
   AVEDIT_STREAM_FINISHED = 0,
      AVEDIT_STREAMPOSITION,
      AVEDIT_NEXTAUDIO,
      AVEDIT_NEXTVIDEO,
      AVEDIT_EXPORTPROGRESS,
      AVEDIT_ERROR = 0x7ffffff
};

// Der folgende ifdef-Block zeigt die Standardlösung zur Erstellung von Makros, 
// die das Exportieren aus einer DLL vereinfachen. Alle Dateien in dieser DLL
// wurden mit dem in der Befehlszeile definierten
// Symbol AVEDIT_EXPORTS kompiliert. Dieses Symbol sollte für kein Projekt 
// definiert werden, das diese DLL verwendet. Auf diese Weise betrachtet jedes 
// andere Projekt, dessen Quellcodedateien diese Datei 
// einbeziehen, AVEDIT_API-Funktionen als aus einer DLL importiert, während diese DLL 
// mit diesem Makro definierte Symbole als exportiert betrachtet.
#ifdef AVEDIT_EXPORTS
#define AVEDIT_API __declspec(dllexport)
#else
#define AVEDIT_API __declspec(dllimport)
#endif

#define MS_TO_REF(x) x*10000i64;



const int AV_LENGTH_SOURCE = 0x80000000;
const int AV_VALUE_UNDEFINED = 0x80000007;


class AVEDIT_API AVSource
{
   friend class AVTarget;
   
   _TCHAR *m_tszSourceFileName;
   int m_nStreamStart;
   int m_nStreamEnd;
   int m_nSourceStart;
   int m_nSourceEnd;
   int m_nStreamSelector;
   int m_nStretchMode;
   int m_nVolume;
   int m_nMainVolume; // if this is a clip audio it can change the main volume (below)
   bool m_bIsAudio;
   
public:
   AVSource(
      const _TCHAR *tszSourceFileName,
      int nStreamStart = 0, int nStreamEnd = AV_LENGTH_SOURCE, 
      int nSourceStart = 0, int nSourceEnd = AV_LENGTH_SOURCE, 
      int nStreamSelector = 0, int nStretchMode = STRETCH_ASPECTRATIO,
      int nVolume = 100, int nMainVolume = 100, bool bAllowNullFile = false);
   ~AVSource();
   
private:
   AVSource* DeepClone() const;
   HRESULT GetFormat(AM_MEDIA_TYPE *type) const;
   HRESULT AddTo(IAMTimelineTrack *pTrack, IAMTimeline *pTimeline, bool bFillBegin=false) const;
   bool HasInputFile() { return m_tszSourceFileName != NULL; }
   int GetMainVolume() { return m_nMainVolume; }
   int GetStreamStart() { return m_nStreamStart; }
   int GetStreamEnd() { return m_nStreamEnd; }
   int GetStreamLength() { return m_nStreamEnd - m_nStreamStart + 1; }
   bool IsAudio() { return m_bIsAudio; }
   bool IsSameFile(AVSource *pOther);
};

class AVEDIT_API AVTarget
{
   friend class AVEdit;
   
   AVSource **m_paSources;
   int m_nNumSources;
   AM_MEDIA_TYPE *m_pFormatDefinition;
   _TCHAR *m_tszTargetFileName;
   int m_nMasterStart;
   int m_nMasterEnd;
   // The FCC is mostly contained in the AM_MEDIA_TYPE,
   // but with some codecs, the FCC is different from
   // the stream handler FCC, e.g. 'divx' vs. 'DX50'.
   // This is a general solution to this problem.
   DWORD m_dwFcc;
   bool m_useSmartRecompression;
   int m_nKeyframeRate;
   int m_nQuality;
   
   char m_szLadInfo[128];
   bool m_hasLadInfo;
   bool m_bIsStartFillClip;
   
public:
   AVTarget(
      const AVSource **paSources, int nNumSources,
      const AM_MEDIA_TYPE *pFormatDefinition = NULL, 
      const _TCHAR *tszTargetFileName = NULL,
      bool useSmartRecompression = false,
      int nMasterStart = AV_VALUE_UNDEFINED, 
      int nMasterEnd = AV_VALUE_UNDEFINED,
      DWORD dwFcc=-1, // Not defined
      int nKeyframeRate = -1, // -1 == Default
      int nQuality = -1);
   ~AVTarget();
   
   // Returns the first time stamp found in the sources 
   int GetMasterStart();
   // Returns the last time stamp found in the sources
   int GetMasterEnd();
   
   void SetLadInfo(const char *szLadInfo);

   /** One video (clip) which spans the whole document length? -> Show it at the beginning. */
   bool IsStartFillClip() { return m_bIsStartFillClip; }
   
   // for debugging
   AM_MEDIA_TYPE *GetOutputFormat() { return m_pFormatDefinition; }
   
private:
   HRESULT GetInputFormat(AM_MEDIA_TYPE *pFormatDefinition);
   
   AVTarget* DeepClone() const;
   HRESULT AddTo(IAMTimeline *pTimeline, bool bForPreview);
   HRESULT SetFile(
      IRenderEngine *pRenderEngine, 
      ICaptureGraphBuilder2 *pBuilder,
      const int nGroupNumber, const _TCHAR *tszDirPrefix);

   HRESULT SetExpectedWavLength(IPin *pGroupPin, CWavDestFilter *pWavDest, IRenderEngine *pRenderEngine);
};

class AVEDIT_API AVProgressListener
{
   virtual void SetPosition(int i) = 0;
   virtual void SetMaximum(int m) = 0;
   virtual void reportError(HRESULT message) = 0;
};

class AVStreamControl
{
public:
   AVStreamControl(IFilterGraph *pGraph) : 
      pControl(pGraph)
         , pSeeking(pGraph)
         , pEvent(pGraph)
      {
         //pClock = NULL;
      }
      ~AVStreamControl()
      {
         //if (pClock)
         //   pClock->Release();
      }
      
      CComQIPtr<IMediaControl> pControl;
      CComQIPtr<IMediaSeeking> pSeeking;
      CComQIPtr<IMediaEvent>   pEvent;
      CComPtr<CSkewReferenceClock> pClock;
};

class AVEDIT_API AVEdit
{
   AVTarget        **m_paTargets;
   int               m_nNumTargets;
   IRenderEngine   **m_paRenderEngines;
   IAMTimeline     **m_paTimelines;
   AVStreamControl **m_paStreamControls;
   int               m_nNumStreamControls;
   
public:
   static HRESULT AddVolumeEffect(IAMTimelineObj *pObject,
      IAMTimeline *pTimeline, int nVolume, int nStartMs, int nEndMs);

   AVEdit();
   ~AVEdit();
   
   HRESULT InitializeTimelines(
      const AVTarget **paTargets, int nNumTargets, bool bForPreview, 
      AVProgressListener *pListener);
   HRESULT InitializePreview(
      // const HWND **paWindows, const int nNumWindows, 
      const HWND hVideoWnd,
      AVProgressListener *pListener, HWND hWndNotify);
   HRESULT InitializeExport(const _TCHAR *tszDirPrefix, HWND hWndNotify=NULL);
   HRESULT BuildPreview();
   HRESULT StartPreview(int startMs=-1, int endMs=-1);
   HRESULT PausePreview();
   HRESULT StopPreview();
   HRESULT JumpPreview(int nMillis, bool allStreams=false);
   bool    IsPreviewPaused();
   bool    IsPreviewRunning();
   UINT    GetPreviewPosition();
   HRESULT StartExport(AVProgressListener *pListener);
   HRESULT AbortExport();
   
   UINT GetAudioPreviewLength();
   
   HRESULT GetPreviewVideoSize(SIZE *pSize);
   HRESULT ResizeVideoWindow(RECT *pRect=NULL);
   
   void FreeResources(bool bRenderEnginesOnly);
   
   int GetNoProgressAbortTimeMs();

   /** One video (clip) which spans the whole document length? -> Show it at the beginning. */
   bool IsStartFillClip() { return m_bIsStartFillClip; }
   
   // for debugging
   AVTarget **GetInternalTargets() { return m_paTargets; }
   
private:
   HWND  m_hWndNotify;
   HWND  m_hVideoWnd;
   
   int   m_nVideoStream;
   bool  m_bCancelRequested;
   
   // Used if there was an abort due to lack of progress, milliseconds
   int   m_nNoProgressAbortTimeMs;
   
   // Used for resource locking:
   CCritSec *m_pLock;
   
   HRESULT CreateRenderEngines(bool bForPreview);
   HRESULT FitVideoWindow(int nStream=1, RECT *pRect=NULL);
   
   void ReleaseStreamControls();
   static void __cdecl LaunchPreviewThread(void *pInstance);
   void PreviewThread();
   static void __cdecl LaunchControlThread(void *pInstance);
   void ControlThread();
   void StopControlThreads();
   
   bool m_isControlThreadRunning;
   bool m_isPreviewThreadRunning;
   bool m_controlThreadStopRequest;
   bool m_previewThreadStopRequest;
   HANDLE m_hPreviewThreadInitEvent;
   HRESULT m_hrPreviewThreadResult;
   
   // The directory prefix used for exports
   _TCHAR *m_tszDirPrefix;
   
   HRESULT CreateCompressor(
      AM_MEDIA_TYPE *pTargetType, 
      IBaseFilter **pCompressor) const;

   bool m_bIsStartFillClip;
};



#endif // __AVEDIT__H__