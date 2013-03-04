#if !defined(AFX_VIDEO2FLV_H__1AB08F6D_18E4_47CB_A1B1_EA6B95D73B5F__INCLUDED_)
#define AFX_VIDEO2FLV_H__1AB08F6D_18E4_47CB_A1B1_EA6B95D73B5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CVideo2Flv : public ISampleGrabberCB
{
public:
   // methods necessary for implementing ISampleGrabberCB and thus IUnknown
   STDMETHODIMP QueryInterface(REFIID riid, void **ppv) { 
      return E_NOTIMPL; 
   };
   STDMETHODIMP_(ULONG) AddRef() {
      ++m_lReferenceCount;
      return m_lReferenceCount;
   };
   STDMETHODIMP_(ULONG) Release() {
      --m_lReferenceCount;
      //if (m_lReferenceCount == 0)
      //   delete this;
      return m_lReferenceCount;
   };


	CVideo2Flv();
	virtual ~CVideo2Flv();

   static bool CheckOverwrite(LPCTSTR tszFilename);

   HRESULT SetInputFiles(LPCTSTR tszVideoFile, REFERENCE_TIME rtVideoOffset,
      LPCTSTR tszAudioFile, REFERENCE_TIME rtAudioOffset);
   HRESULT AddClip(LPCTSTR tszClipFile, REFERENCE_TIME rtClipOffset);
   HRESULT SetOutputParams(LPCTSTR tszOutputFile, 
      int iOutputWidth = 0, int iOutputHeight = 0, 
      int iVideoKBit = 768, int iAudioKBit = 32, 
      bool bIncreaseKeyframes = false, bool bDoPadding = false, bool bForVersion9 = true);
   HRESULT PrepareInput();
   HRESULT PrepareConversion(CDialog *pDlg = NULL);
   HRESULT PrepareConversionAvi(CDialog *pDlg = NULL);
   HRESULT StartConversion();
   HRESULT AbortConversion();
   int GetProgress() { return m_iCurrentProgress; }

   HRESULT CheckForFilters();

   virtual HRESULT STDMETHODCALLTYPE BufferCB(
      double dSampleTime,
      BYTE *pBuffer,
      long BufferLen);
   virtual HRESULT STDMETHODCALLTYPE SampleCB(
      double dSampleTime,
      IMediaSample *pSample);


private:
   static DWORD WINAPI ConversionLauncher(void *pParameter);

   HRESULT ConversionThread();

   //HRESULT FindFilter(bool bAudio, LPCTSTR tszCodecName, IBaseFilter **ppTargetFilter);
   void RemoveGraph();

   HRESULT MakeAudioVideoSource(bool bUseAudio, double dAudioLength, bool bDoPadding);

   // Bugfix 5940 ("Redo" of Bugfix 5331): Monogram FLV mux is required for A/V synchronity
   static HRESULT MakeMp3Target(IGraphBuilder *pGraph, IPin *pSourcePin, IBaseFilter *pMux, 
       int iAudioKBit, HWND hwndParent, bool bUseMonogramMux);

   int m_iOutputWidth;
   int m_iOutputHeight;
   int m_iVideoKBit;
   int m_iAudioKBit;
   double m_dOutputFps;
   bool m_bIncreaseKeyframes;
   bool m_bDoPadding;
   bool m_bHasEmptyVideo;
   bool m_bForVersion9;

   CComPtr<IGraphBuilder> m_pGraph;
   CComPtr<IRenderEngine> m_pEngine;
   CString m_csVideoFile;
   REFERENCE_TIME m_rtVideoOffset;
   CString m_csAudioFile;
   REFERENCE_TIME m_rtAudioOffset;
   CArray<CString, CString> m_aClipFiles;
   CArray<REFERENCE_TIME, REFERENCE_TIME> m_aClipOffsets;
   bool m_bConversionAborted;
   int m_iCurrentProgress;
   CString m_csOutputFile;
   
   CComPtr<IPin> m_pSourcePinVideo;
   CComPtr<IPin> m_pSourcePinAudio;

   CDialog *m_pSourceDialog;
  
   ULONG m_lReferenceCount;
};

#endif // !defined(AFX_VIDEO2FLV_H__1AB08F6D_18E4_47CB_A1B1_EA6B95D73B5F__INCLUDED_)
