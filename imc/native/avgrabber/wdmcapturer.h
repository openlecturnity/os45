#ifndef __WDMCAPTURER_H__
#define __WDMCAPTURER_H__

// DirectShow/DirectX includes
#include <dshow.h>
#include <qedit.h>
#include <process.h>
#include <objbase.h>
#include <dvdmedia.h>
#include <stdio.h>
#include <tchar.h>
#include "streams.h"

typedef enum WDM_INTERFACE_ID 
{
   ID_CAPTUREFILTER,
   ID_ANALOGVIDEODECODER,
   ID_STREAMCONFIG,
   ID_CROSSBAR,
   ID_TVTUNER,
   ID_DV_DECODER,
   ID_VFW_SOURCE,
   ID_VFW_FORMAT,
   ID_VFW_DISPLAY
};

typedef enum DVRES
{
   DVRES_FULL = DVRESOLUTION_FULL,
   DVRES_HALF = DVRESOLUTION_HALF,
   DVRES_QUARTER = DVRESOLUTION_QUARTER,
   DVRES_EIGHTH = DVRESOLUTION_DC
};

typedef struct
{
   WDM_INTERFACE_ID widId;
   _TCHAR           tszDescription[64];
} WDM_INTERFACE;

typedef struct
{
   char *pAmMediaType;
   int   cbAmMediaType;
   char *pVideoHeader;
   int   cbVideoHeader;
} STREAMCONFIGDATA;

class AVGRABBER_EXPORT WdmCapturer : public VideoCapturer
{
   friend class CSampleCallback;
public:
   WdmCapturer(_TCHAR *tszDeviceName);
   virtual ~WdmCapturer();

   virtual void start(_TCHAR *tszFileName, AudioCapturer *audio=NULL);
   virtual void stop();

   virtual long getRefTime();
   virtual long getVideoTime();

   virtual void displayMonitor(bool b=true);

   virtual TIME_US setFrameRate(double frameRate);

   // Returns false, does not have source dialog.
   virtual bool hasSourceDialog() { return false; }
   // Returns false, does not have format dialog.
   virtual bool hasFormatDialog() { return false; }
   // Returns false, does not have display dialog.
   virtual bool hasDisplayDialog() { return false; }
   
   // Returns true if the given WDM source is a DV device
   virtual bool IsDvSource();
   virtual bool IsVfwSource();

   // Returns the number of interfaces that were found on this
   // capture device. Retrieve the interfaces with the
   // GetInterfaces method.
   virtual int  GetInterfaceCount();

   // Retrieves the interfaces this capture device supports. Display
   // an interface configuration dialog by using the DisplayConfiguration-
   // Dialog method.
   virtual void GetInterfaces(WDM_INTERFACE *pInterfaces);

   // Displays a configuration dialog. Retrieve the possible parameters
   // for WDM_INTERFACE_ID with the GetInterfaces and GetInterfaceCount
   // methods.
   virtual bool DisplayConfigurationDialog(WDM_INTERFACE_ID, HWND=NULL);

   // Use this method in order to make the WDM Capturer convert DV
   // (digital video) data to RGB data prior to passing the data to
   // a codec. This setting only has effect if the source filter is
   // a DV camera/DV VCR.
   virtual void SetConvertDvToRgb(bool bRgb=true);
   virtual bool GetConvertDvToRgb();

   // Use this method in order to tell the WDM capturer to which 
   // resolution the DV data is to be converted to RGB data. This
   // may be one of the DVRES enumeration: DVRES_FULL to DVRES_EIGHTH.
   virtual void SetDvResolution(DVRES dvRes=DVRES_HALF);
   virtual DVRES GetDvResolution();

   virtual void SetStreamConfigData(char *pAmMediaType, int cbAmMedia,
                                    char *pVideoHeader, int cbVideoHeader);

   virtual int  GetStreamConfigMediaTypeSize();
   virtual int  GetStreamConfigVideoHeaderSize();
   virtual void GetStreamConfigData(char *pAmMediaType, int cbAmMedia,
                                    char *pVideoHeader, int cbVideoHeader);

   // Retrieves the number of WDM capture devices currently available
   // in the system. Note: Calling this method initializes the device
   // names you retrieve with GetWdmSources, so you can be sure that
   // the number of devices matches the names returned by GetWdmSources.
   // Also note that you *cannot* be sure that you can instanciate
   // a specific capture device, as they may change without notice.
   static HRESULT GetWdmSourcesCount(int *pnDevCount);

   // Retrieves the names of the capture devices. These are initialized
   // through call to the GetWdmSourcesCount method. See there for
   // further details. The paDevices variable has to be an array
   // of (char *) variables, each containing at least maxLen space.
   static HRESULT GetWdmSources(_TCHAR **paDevices, int maxLen);

   // Sets the default DV parameters. These parameters are only affecting
   // DV source filters. Set dvRes to the desired default DV resolution,
   // if converted to RGB data. Set bRgb to true in order to convert the
   // DV data to a 24 Bit RGB format; if bRgb is set to false, the dvRes
   // variable is of no importance.
   static void SetDefaultDvParameters(DVRES dvRes=DVRES_HALF, bool bRgb=true);
private:
   // instance counter
   static int instanceCount_;
   DWORD      dwThreadId_;

   _TCHAR       szDeviceName_[128];

   HWND    hMonitor_;
   HANDLE  hInitEvent_;
   HRESULT hInitResult_;
   HANDLE  hExitEvent_;

   // DirectShow filters:
   ICaptureGraphBuilder2 *pBuilder_;
   IGraphBuilder         *pGraph_;
   IBaseFilter           *pSrc_;
   IVideoWindow          *pVideo_;
   IReferenceClock       *pRefClock_;

   BITMAPINFO             bmiVideoStream_;

   // Interface for the sample grabber callback
   CSampleCallback       *pSampleCallback_;

   bool                   isGraphBuilt_;
   bool                   isPreviewing_;
   bool                   isDvSource_;
   bool                   isVfwSource_;

   bool                   isCapturing_;

   int                    debugPosition_;

   static bool            convertDvDataDefault_;
   bool                   convertDvData_;
   static _DVRESOLUTION   dvResolutionDefault_;
   _DVRESOLUTION          dvResolution_;

   STREAMCONFIGDATA       streamConfigData_;
   AM_MEDIA_TYPE         *m_pStreamMediaType; // TODO should be used instead of the one above; join with videoFormat_
   bool                   streamConfigDataValid_;

   // Video interfaces; is a vector<WDM_INTERFACE_ID>
   void                  *pInterfaces_;

   static _TCHAR **s_atszDevices;
   static int      s_iDeviceListSize;

   // --------------------------------- //

   // Methods
   void ReleaseResources();

   void RegisterWdmMonitorClass();
   void UnregisterWdmMonitorClass();

   void WindowLoop();
   void MakeWindow();

   static void __cdecl     WindowLauncher(void *pData);
   static LRESULT CALLBACK MonitorProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

   // WDM/DirectShow methods
   HRESULT DestroyWdmCapturer();
   HRESULT InitFilterGraph();
   HRESULT FindVideoInterfaces();
   HRESULT ReleaseFilterGraph();
   HRESULT InitCaptureSource(_TCHAR *tszDeviceName, IBaseFilter **pSrc);
   HRESULT CopyPreviewSettings();
   HRESULT SetDvParameters();
   HRESULT CopyBitmapInfoHeader(AM_MEDIA_TYPE *pMt);
   HRESULT RetrieveVideoSettings();
   HRESULT FindReferenceClock();
   HRESULT SetWdmFrameRate(REFERENCE_TIME *pAvgTimePerFrame);

   HRESULT InitWdmStreamConfigData();
   HRESULT SetWdmDefaultParameters();
   HRESULT SetWdmStreamConfigData();
   HRESULT GetWdmStreamConfigData();
   HRESULT GetWdmStreamConfigMediaTypeSize(DWORD *dwSize);
   HRESULT GetWdmStreamConfigVideoHeaderSize(DWORD *dwSize);

   HRESULT StartPreview();
   HRESULT BuildPreviewGraph();
   HRESULT StopPreview();
   HRESULT StopPreviewGraph();
   HRESULT StartCapture();
   HRESULT StopCapture();
   HRESULT BuildCaptureGraph();
   HRESULT DestroyCaptureGraph();
   HRESULT BuildDVCaptureGraph();
   HRESULT DestroyDVCaptureGraph();

   void    HandleSample(double sampleTime, IMediaSample *pSample);

   void    TearDownGraph(IFilterGraph *pFg, IBaseFilter *pFilter);

   static BOOL CALLBACK DvDialogProc(HWND, UINT, WPARAM, LPARAM);

   virtual bool DisplayCaptureFilterDialog(HWND=NULL);
   virtual bool DisplayStreamConfigDialog(HWND=NULL);
   virtual bool DisplayAnalogVideoDecoderDialog(HWND=NULL);
   virtual bool DisplayCrossbarDialog(HWND=NULL);
   virtual bool DisplayTvTunerDialog(HWND=NULL);
   virtual bool DisplayDvDecoderDialog(HWND=NULL);
   virtual bool DisplayVfwFormatDialog(HWND=NULL);
   virtual bool DisplayVfwDisplayDialog(HWND=NULL);
   virtual bool DisplayVfwSourceDialog(HWND=NULL);

   HRESULT ShowPropertyPages(IUnknown *pFilter, HWND hWnd=NULL);
};

#endif // __WDMCAPTURER_H__
