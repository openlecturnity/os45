#include "StdAfx.h"

#include "AVStreams.h"
#include "MiscFunctions.h"
#include "ladfilter.h"


/*********************************************
 *********************************************/

#define FULL_VERSION 0L
#define EVALUATION_VERSION 1L
#define UNIVERSITY_VERSION 2L
#define UNIVERSITY_EVALUATION_VERSION 3L

CAudioStream::CAudioStream() : CMediaStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init();
}

CAudioStream *CAudioStream::Copy()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CAudioStream *p = new CAudioStream();
   p->m_cbSamples = m_cbSamples;
   p->m_hasLadInfo = m_hasLadInfo;
   memcpy(p->m_szLadInfo, m_szLadInfo, 128);
   if (m_cbSamples > 0)
   {
      p->m_samples = new signed char[m_cbSamples];
      memcpy(p->m_samples, m_samples, m_cbSamples);
   }
   else
      p->m_samples = NULL;
   p->m_wfe = m_wfe;
   p->m_csFilename = m_csFilename;

   p->m_lengthMs = m_lengthMs;
   p->m_iReadBytes = m_iReadBytes;
   p->m_iOffset = m_iOffset;
   p->m_nStreamSelector = m_nStreamSelector;
   p->m_isVersionError = m_isVersionError;
   p->m_nVersionType = m_nVersionType;
   p->m_csOriginalAudioSource = m_csOriginalAudioSource;
   p->m_nOriginalAudioSourceStream = m_nOriginalAudioSourceStream;

   return p;
}

CAudioStream::~CAudioStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

bool CAudioStream::SetFilename(LPCTSTR filename, int nStream, int versionType, HWND hWndProgress)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   ASSERT(versionType != -1);

   HRESULT hr = S_OK;

   if (filename)
   {
      m_csFilename      = filename;
      m_nStreamSelector = nStream;
      m_hWndProgress    = hWndProgress;

      if (_taccess(m_csFilename, 04) != -1)
      {
         hr = ReadAudioParameters(versionType);
      }
      else
      {
         // File not found!
         return false;
      }
   }
   else
   {
      // This is a dummy audio stream

      Init();
      m_csFilename.Empty();
      m_lengthMs = 2147483647; // Very long
      m_nVersionType = versionType;

      return true;
   }

   return SUCCEEDED(hr) ? true : false;
}

void CAudioStream::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

//   m_iSampleRate = 22050;
//   m_iBitRate    = 16;
   ZeroMemory(&m_wfe, sizeof WAVEFORMATEX);
   m_wfe.nSamplesPerSec = 22050;
   m_wfe.wBitsPerSample = 16;
   UpdateWaveFormat();
   m_samples   = NULL;
   m_cbSamples = 0;
   m_lengthMs  = 0;
   m_iID       = -1;
   m_iOffset   = 0;
   m_nVersionType = EVALUATION_VERSION;
   m_isVersionError = false;
   m_nStreamSelector = 0;
   m_csOriginalAudioSource.Empty();
   m_nOriginalAudioSourceStream = -1;
}

void CAudioStream::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_csFilename.Empty();
   if (m_samples)
      delete[] m_samples;
   m_samples   = NULL;
   m_cbSamples = 0;
   m_lengthMs  = 0;
   m_iID       = -1;
   ZeroMemory(&m_wfe, sizeof WAVEFORMATEX);
}

bool CAudioStream::IsDummyStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_csFilename.IsEmpty())
      return true;
   else
      return false;
}

void CAudioStream::SetBitsPerSample(int iBitsPerSample)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_wfe.wBitsPerSample = iBitsPerSample;
   UpdateWaveFormat();
}

void CAudioStream::SetSampleRate(int iSampleRate)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_wfe.nSamplesPerSec = iSampleRate;
   UpdateWaveFormat();
}

void CAudioStream::UpdateWaveFormat()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // We assume here that sample rate and bit rate
   // are correctly set and calculate the rest
   // of the fields from that.
   m_wfe.cbSize          = sizeof WAVEFORMATEX;
   m_wfe.nChannels       = 1;
   m_wfe.wFormatTag      = WAVE_FORMAT_PCM;
   m_wfe.nBlockAlign     = m_wfe.wBitsPerSample / 8;
   m_wfe.nAvgBytesPerSec = m_wfe.nSamplesPerSec * m_wfe.nBlockAlign;
}

HRESULT CAudioStream::BufferCB(double sampleTime, BYTE *pBuffer, long bufferLen)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   bool is16Bit = (m_wfe.wBitsPerSample == 16);
   bool isMono  = (m_wfe.nChannels == 1);

   // Distance between two subsequent samples at 100 samples per second,
   // measured in bytes
   double distance      = ((double) (m_wfe.nSamplesPerSec * m_wfe.nBlockAlign)) / 100.0;
   int upToNowCount     = (int) (m_iReadBytes / distance);
   int rest             = m_iReadBytes - ((int) (distance * upToNowCount));

   int nextSample    = rest == 0 ? 0 : (int) (distance - rest);
   int align = nextSample % m_wfe.nBlockAlign;
   if (align > 0)
      nextSample += m_wfe.nBlockAlign - align;

   int sampleCount   = (int) ((bufferLen - rest) / distance);

   // Now read the samples from this block.
   // Each sample is transformed using the
   // log10 function to make the samples
   // more visible (usual transform). Between
   // 1 and 10, the log10() function ranges
   // between 0 and 1, increasing fast in
   // the beginning.
   int sampleBytes = 0;
   int sampleNr    = upToNowCount;
   for (int i=0; i<sampleCount; ++i)
   {
      sampleBytes = nextSample + (int) (distance * i);
      sampleNr = upToNowCount + i;
      if (sampleBytes % m_wfe.nBlockAlign != 0)
         sampleBytes += m_wfe.nBlockAlign - sampleBytes % m_wfe.nBlockAlign;
      ASSERT(sampleBytes <= bufferLen - m_wfe.nBlockAlign);

      if (is16Bit)
      {
         // 16 Bit
         if (isMono)
         {
            // 16 Bit, Mono
            // m_samples[sampleNr] = (signed char) (rand() % 256 - 128);//(signed char) (*((short *) (pBuffer + sampleBytes)) / 256);
            int s = *((short *) (pBuffer + sampleBytes));
            if (s > 0)
               s = (int) (32767.0 * log10(1.0 + 9.0*((double) s) / 32767.0));
            else
               s = (int) (-32768.0 * log10(1.0 + 9.0*((double) -s) / 32768.0));
            m_samples[sampleNr] = (signed char) (s / 256);
         }
         else
         {
            // 16 Bit, Stereo
            int s1 = *((short *) (pBuffer + sampleBytes));
            int s2 = *((short *) (pBuffer + sampleBytes + 2));
            int s = (s1 + s2) / 2;
            if (s > 0)
               s = (int) (32767.0 * log10(1.0 + 9.0*((double) s) / 32767.0));
            else
               s = (int) (-32768.0 * log10(1.0 + 9.0*((double) -s) / 32768.0));
            m_samples[sampleNr] = (signed char) (s / 256);
         }
      }
      else
      {
         // 8 Bit
         if (isMono)
         {
            // 8 Bit, Mono
            int s = *((unsigned char *) (pBuffer + sampleBytes));
            s -= 128;
            if (s > 0)
               s = (int) (127.0 * log10(1.0 + 9.0*((double) s) / 127.0));
            else
               s = (int) (-128.0 * log10(1.0 + 9.0*((double) -s) / 128.0));
            m_samples[sampleNr] = (signed char) s;
         }
         else
         {
            // 8 Bit, Stereo
            int s1 = *((unsigned char *) (pBuffer + sampleBytes));
            s1 -= 128;
            int s2 = *((unsigned char *) (pBuffer + sampleBytes));
            s2 -= 128;
            int s = (s1 + s2) / 2;
            if (s > 0)
               s = (int) (127.0 * log10(1.0 + 9.0*((double) s) / 127.0));
            else
               s = (int) (-128.0 * log10(1.0 + 9.0*((double) -s) / 128.0));
            m_samples[sampleNr] = (signed char) s;
         }
      }
   }

   m_iReadBytes += bufferLen;

   if (m_hWndProgress)
   {
      int msSampleTime = (int) (1000 * sampleTime);
      int progress = 100 * msSampleTime / m_lengthMs;

      ::PostMessage(m_hWndProgress, WM_AUDIO_PROGRESS, 0, progress);
   }

   return S_OK;
}

HRESULT CAudioStream::SampleCB(double sampleTime, IMediaSample *pSample)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // Wrap the SampleCB call to the BufferCB method.
   HRESULT hr = S_OK;
   BYTE *pBuffer;
   hr = pSample->GetPointer(&pBuffer);
   long bufferLen = pSample->GetActualDataLength();
   if (SUCCEEDED(hr))
      return BufferCB(sampleTime, pBuffer, bufferLen);
   return hr;
}

// This method creates a DirectShow filter graph
// and reads out all samples in this particular
// audio stream. Furthermore, it reads out the
// wave format of this stream (WAVEFORMATEX).
HRESULT CAudioStream::ReadAudioParameters(int versionType)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   HRESULT hr = S_OK;

   hr = CoInitialize(NULL);

   CComPtr<IGraphBuilder> pGraph;
   CComPtr<IBaseFilter> pBaseFileSource;
   CComPtr<ISampleGrabber> pSampleGrabber;
   CComPtr<IBaseFilter> pBaseSampleGrabber;
   CComPtr<IBaseFilter> pNullRenderer;
   CComPtr<ICaptureGraphBuilder2> pBuilder;
   CComPtr<IMediaControl> pControl;
   CComPtr<IMediaEvent> pEvent;

   // First, create a filter graph instance
   if (SUCCEEDED(hr))
      hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_ALL, IID_IGraphBuilder, (void **) &pGraph);
   // We also need a Capture graph builder because of the
   // RenderStream() method (easier to use). The term
   // capture graph builder does not mean that you can't
   // use it to build other graphs than capture graphs.
   if (SUCCEEDED(hr))
      hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_ALL, IID_ICaptureGraphBuilder2, (void **) &pBuilder);
   // Attach the filter graph to the Capture graph builder
   if (SUCCEEDED(hr))
      hr = pBuilder->SetFiltergraph(pGraph);
  
   // Creates a FileSourceFilter and adds it to the graph.
   CComBSTR bstrFilename((LPCTSTR)m_csFilename);
   if (SUCCEEDED(hr))
      hr = pGraph->AddSourceFilter(bstrFilename, L"Audio Source Filter", &pBaseFileSource);

   if (SUCCEEDED(hr))
   {
       // This one speeds up "RenderStream()" lateron. Otherwise searching for the LAD filter
       // (by intelligent connect) is very time consuming.

      CComPtr<IBaseFilter> pLadParser;
      if (SUCCEEDED(hr))
         hr = CoCreateInstance(CLSID_LadFilter, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pLadParser);

      if (SUCCEEDED(hr))
         hr = pGraph->AddFilter(pLadParser, L"LAD Parser");
   }

    // We need a sample grabber instance in order to grab
   // the samples from the audio file.
   if (SUCCEEDED(hr))
      hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_ALL, IID_ISampleGrabber, (void **) &pSampleGrabber);
   // As with the file source filter, we need the IBaseFilter
   // pointer to the sample grabber in order to insert the
   // filter into the filter graph.
   if (SUCCEEDED(hr))
      hr = pSampleGrabber->QueryInterface(IID_IBaseFilter, (void **) &pBaseSampleGrabber);
   // Ok, now insert the sample grabber into the graph
   if (SUCCEEDED(hr))
      hr = pGraph->AddFilter(pBaseSampleGrabber, L"Sample Grabber");
   
   // Specify the media type to use in the sample grabber: 
   // We need an Audio PCM format of some kind.
   if (SUCCEEDED(hr))
   {
      CMediaType mtInput;
      mtInput.InitMediaType();
      mtInput.SetType(&MEDIATYPE_Audio);
      mtInput.SetSubtype(&MEDIASUBTYPE_PCM);
      hr = pSampleGrabber->SetMediaType(&mtInput);
   }
   // This class already implements the ISampleGrabberCB
   // (callback) interface, so we may pass us to the
   // sample grabber like this. The second parameter means
   // that the BufferCB() method will be called, a 0 here
   // would call the SampleCB() method for sample callbacks.
   if (SUCCEEDED(hr))
     hr = pSampleGrabber->SetCallback((ISampleGrabberCB *) this, 1);

   // Now connect the file source with the sample grabber.
   // Depending on the type of the source file, other inter-
   // mediate filters may be inserted in order to create
   // a PCM audio stream for the sample grabber
   if (SUCCEEDED(hr))
      hr = pBuilder->RenderStream(NULL, NULL, pBaseFileSource, NULL, pBaseSampleGrabber);
   // Note: without "&MEDIATYPE_Audio" as type this can take rather long;
   // at least in a debug build. However: using it yields no error but the 
   // below "GetConnectedMediaType() will fail.

   // If everything has gone alright up to now, we should be
   // able to read out the format of the stream coming from
   // the audio file now
   if (SUCCEEDED(hr))
   {
      // Read out the format:
      AM_MEDIA_TYPE mt;
      hr = pSampleGrabber->GetConnectedMediaType(&mt);

      if (SUCCEEDED(hr))
      {
         if (mt.formattype == FORMAT_WaveFormatEx)
         {
            WAVEFORMATEX *pWfe = (WAVEFORMATEX *) mt.pbFormat;
            // Copy the wave format block:
            m_wfe = *pWfe;
         }
         else
            hr = VFW_E_INVALIDMEDIATYPE;
      
         FreeMediaType(mt);
      }

   }

   // We need to connect the sample grabber to a Renderer;
   // you always have to render a filter graph in some way.
   // We will create a null renderer which simply throws
   // away and releases the samples as they are about to be
   // rendered.
   if (SUCCEEDED(hr))
     hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_ALL, IID_IBaseFilter, (void **) &pNullRenderer);
   // Add the null renderer to the filter graph
   if (SUCCEEDED(hr))
     hr = pGraph->AddFilter(pNullRenderer, L"Null Renderer");
   // Now connect the sample grabber with the null renderer
   if (SUCCEEDED(hr))
     hr = pBuilder->RenderStream(NULL, NULL, pBaseSampleGrabber, NULL, pNullRenderer);

   CComPtr<ILadParser> pLadParser;
   HRESULT hrLad = S_OK;
   if (SUCCEEDED(hr))
      hrLad = pBuilder->FindInterface(NULL, NULL, pBaseFileSource, IID_ILadParser, (void **) &pLadParser);
   else
      hrLad = E_FAIL;
   if (SUCCEEDED(hrLad) && pLadParser)
   {
      int nCount = 0;
      hrLad = pLadParser->QueryLadInfo(NULL, &nCount);
      if (SUCCEEDED(hrLad))
         hrLad = pLadParser->QueryLadInfo(m_szLadInfo, &nCount);
      if (SUCCEEDED(hrLad))
      {
         m_hasLadInfo = true;
         char versionTypeChar = m_szLadInfo[0];
         if (versionTypeChar == 'u')
            m_nVersionType = UNIVERSITY_VERSION;
         else if (versionTypeChar == 'f')
            m_nVersionType = FULL_VERSION;
         else // default
            m_nVersionType = EVALUATION_VERSION;

         if (versionType == FULL_VERSION ||
             versionType == UNIVERSITY_VERSION)
         {
            // Full and Campus versions don't work with evaluation documents
            if (m_nVersionType == EVALUATION_VERSION)
            {
               m_isVersionError = true;
               hr = E_FAIL;
            }
         }
         else // EVALUATION_VERSION
         {
            if (m_nVersionType != EVALUATION_VERSION)
            {
               m_isVersionError = true;
               hr = E_FAIL;
            }
         }
      }
      else
         m_hasLadInfo = false;
   }
   else
      m_hasLadInfo = false;

   // We need an IMediaControl interface to the graph in order
   // to start the processing of the source file
   if (SUCCEEDED(hr))
      hr = pGraph->QueryInterface(IID_IMediaControl, (void **) &pControl);
   // ... and an IMediaEvent interface in order to know when
   // the processing has finished.
   if (SUCCEEDED(hr))
      hr = pGraph->QueryInterface(IID_IMediaEvent, (void **) &pEvent);

   // In order to process the samples as fast as possible, we
   // need the IMediaFilter interface of the filter graph.
   // By setting the sync source to NULL the samples are
   // processed without checking for the reference time. Without
   // doing this, the samples would be processed in real time,
   // which is not desirable here
   CComPtr<IMediaFilter> pMediaFilter;
   if (SUCCEEDED(hr))
      hr = pGraph->QueryInterface(IID_IMediaFilter, (void **) &pMediaFilter);
   if (SUCCEEDED(hr))
      hr = pMediaFilter->SetSyncSource(NULL);

   // Now we'll try to retrieve the duration of the complete
   // media stream by querying for an IMediaSeeking interface
   // on the filter graph.
   CComPtr<IMediaSeeking> pSeeking;
   if (SUCCEEDED(hr))
      hr = pGraph->QueryInterface(IID_IMediaSeeking, (void **) &pSeeking);
   if (SUCCEEDED(hr))
   {
      LONGLONG duration;

      // The returned duration is given in the unit
      // 100ns, thus we have to divide by 10000 to get millis.
      if (SUCCEEDED(hr = pSeeking->GetDuration(&duration)))
         m_lengthMs = (int) (duration / 10000i64);
      else
         m_lengthMs = -1;
   }

   // DWORD dwRegister = 0;
   // AddToRot(pGraph, &dwRegister);

   // Reset the byte counter:
   m_iReadBytes = 0;
   // Create a sample array
   if (SUCCEEDED(hr))
   {
      // Make room for 100 samples per second:
      m_cbSamples = (m_lengthMs / 60 + 1) * 100;
      m_samples = new signed char[m_cbSamples];
      if (m_samples == NULL)
         hr = E_OUTOFMEMORY;
      else
         ZeroMemory(m_samples, m_cbSamples);
   }

   // Run the graph to read out the samples
   long evCode = 0;
   if (SUCCEEDED(hr))
      hr = pControl->Run();
   // Wait until it's ready
   if (SUCCEEDED(hr))
      hr = pEvent->WaitForCompletion(INFINITE, &evCode);

   // RemoveFromRot(dwRegister);

   ::CoUninitialize();

   return hr;
}

/*********************************************
 *********************************************/

CVideoStream::CVideoStream() : CMediaStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init();
}

CVideoStream::~CVideoStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

CVideoStream *CVideoStream::Copy()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CVideoStream *pCopyStream = new CVideoStream();

   pCopyStream->SetFilename(m_csFilename);
   pCopyStream->SetFrameRate(m_iFrameRate);
   pCopyStream->SetCodec(m_csCodec);
   pCopyStream->SetQuality(m_iQuality);
   pCopyStream->SetKeyframes(m_iKeyframes);
   pCopyStream->SetOffset(m_iOffset);
   pCopyStream->m_nStreamSelector = m_nStreamSelector;

   return pCopyStream;
}

void CVideoStream::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iFrameRate = 0.0;
   m_iQuality   = 0;
   m_iKeyframes = 0;
   m_iID        = -1;
   m_iOffset    = 0;
   m_nLength    = 0;
}

void CVideoStream::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_csFilename.Empty();
   m_iFrameRate = 0.0;
   m_csCodec.Empty();
   m_iQuality   = 0;
   m_iKeyframes = 0;
   m_iID        = -1;
}

void CVideoStream::SetFilename(LPCTSTR filename, int nStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_csFilename      = filename;
   m_nStreamSelector = nStream;

}

/*********************************************
 *********************************************/

CTimeSpread::CTimeSpread()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iBegin = -1;
   m_iEnd = -1;
}


/*********************************************
 *********************************************/

CExportClip::~CExportClip()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (!m_cutList.IsEmpty())
   {
      POSITION position = m_cutList.GetHeadPosition();
      while (position)
      {
         CTimeSpread *timeSpread = m_cutList.GetNext(position);
         if (timeSpread)
            delete timeSpread;
      }
      m_cutList.RemoveAll();
   }

}

int CExportClip::GetLength()
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int nTotalMs = 0;
   POSITION pos = m_cutList.GetHeadPosition();
   while (pos)
   {
      CTimeSpread *pSpread = m_cutList.GetNext(pos);
      if (pSpread)
         nTotalMs += (pSpread->m_iEnd - pSpread->m_iBegin);
   }

   return nTotalMs;
}

/*********************************************
 *********************************************/

CClipInfo::CClipInfo()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init();
}

CClipInfo::~CClipInfo()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

void CClipInfo::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iTimestamp = 0;
   m_iLength    = 0;
   m_nStreamSelector = 0;
   m_nAudioStreamSelector = -1;
   m_nTempOriginalVolume = 100;
   m_csOriginalAudioSource.Empty();
   m_nOriginalAudioSourceStream = -1;
   // PZI:
   m_bStructureAvailable = false;
   m_bImagesAvailable = false;
   m_bFulltextAvailable = false;
}

void CClipInfo::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_csFilename.Empty();
   m_csAudioSource.Empty();
   m_csTitle.Empty();
   m_slKeywords.RemoveAll();
   m_iTimestamp = 0;
   m_iLength    = 0;
   m_nStreamSelector = 0;
   m_nAudioStreamSelector = -1;
   m_nTempOriginalVolume = 100;
   m_csOriginalAudioSource.Empty();
   m_nOriginalAudioSourceStream = -1;
   // PZI:
   m_bStructureAvailable = false;
   m_bImagesAvailable = false;
   m_bFulltextAvailable = false;
}

CClipInfo *CClipInfo::Copy()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CClipInfo *retClip = new CClipInfo();

   retClip->SetFilename(m_csFilename);
   retClip->SetAudioFilename(m_csAudioSource);
   retClip->SetTimestamp(m_iTimestamp);
   retClip->SetKeywords(m_slKeywords);
   retClip->SetLength(m_iLength);
   retClip->SetTitle(m_csTitle);
   retClip->m_nStreamSelector = m_nStreamSelector;
   retClip->SetAudioStreamSelector(m_nAudioStreamSelector);
   retClip->SetTempOriginalAudioVolume(m_nTempOriginalVolume);
   retClip->SetOriginalAudioSource(m_csOriginalAudioSource);
   retClip->SetOriginalAudioSourceStream(m_nOriginalAudioSourceStream);
   // PZI:
   retClip->SetStructureAvailable(m_bStructureAvailable);
   retClip->SetImagesAvailable(m_bImagesAvailable);
   retClip->SetFulltextAvailable(m_bFulltextAvailable);

   return retClip;
}

void CClipInfo::SetFilename(LPCTSTR filename, int nStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_csFilename      = filename;
   m_nStreamSelector = nStream;
}

void CClipInfo::GetKeywords(CStringList &stringList)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_slKeywords.GetHeadPosition();
   while (position)
   {
      CString string = m_slKeywords.GetNext(position);
      stringList.AddTail(string);
   }
}

void CClipInfo::GetKeywords(CString &csKeywords)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_slKeywords.GetHeadPosition();
   while (position)
   {
      csKeywords += m_slKeywords.GetNext(position);
      if (position)
         csKeywords += "; ";
   }
}

void CClipInfo::SetKeywords(CStringList &stringList)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_slKeywords.RemoveAll();

   POSITION position = stringList.GetHeadPosition();
   while (position)
   {
      CString string = stringList.GetNext(position);
      m_slKeywords.AddTail(string);
   }
}

void CClipInfo::SetKeywords(CString &csKeywords)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_slKeywords.RemoveAll();

   int nStart = 0;
   int iNextPos = 0;
   while (iNextPos != -1)
   {
      iNextPos = csKeywords.Find(_T(";"), nStart);
      if (iNextPos != -1)
      {
         CString csKeyword = csKeywords.Mid(nStart, iNextPos-nStart);
         csKeyword.TrimLeft(_T(" "));
         m_slKeywords.AddTail(csKeyword);
         nStart = iNextPos + 1;
      }
      else 
      {
         CString csKeyword = csKeywords.Mid(nStart);
         csKeyword.TrimLeft(_T(" "));
         if (!csKeyword.IsEmpty())
            m_slKeywords.AddTail(csKeyword);
      }
   }
}
/*********************************************
 *********************************************/

CClipStream::CClipStream() : CMediaStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init();
}

CClipStream::~CClipStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

void CClipStream::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iID = -1;
}

void CClipStream::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   for (int i = 0; i < m_arrClip.GetSize(); ++i)
   {
      CClipInfo *element = m_arrClip[i];
      if (element)
         delete element;
   }

   m_arrClip.RemoveAll();

   m_iID = -1;
}

CClipStream *CClipStream::Copy()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CClipStream *pCopyStream = new CClipStream();
   for (int i = 0; i < m_arrClip.GetSize(); ++i)
   {
      pCopyStream->AddClip(m_arrClip[i]->Copy());
   }

   return pCopyStream;
}

void CClipStream::AddClip(CClipInfo *clipInfo)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_arrClip.Add(clipInfo);
}


CClipInfo *CClipStream::FindClip(int clipNumber)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_arrClip.GetSize() == 0)
      return NULL;

   CClipInfo *retClip = NULL;

   if (clipNumber < m_arrClip.GetSize())
      retClip = m_arrClip[clipNumber];

   return retClip;
}

// PZI: extended to return clip number (starting at 1; or 0 if not found)
CClipInfo *CClipStream::FindClipBeginsAt(int timestamp, int *returnClipNumber)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if(returnClipNumber) *returnClipNumber = 0;

   if (m_arrClip.GetSize() == 0)
      return NULL;

   for (int i = 0; i < m_arrClip.GetSize(); ++i)
   {
      CClipInfo *clip = m_arrClip[i];
      if (clip && 
          (timestamp == clip->GetTimestamp()))
      {
         if(returnClipNumber) *returnClipNumber = i+1;
         return clip;
      }
   }

   return NULL;
}

// PZI: extended to return clip number (starting at 1; or 0 if not found)
CClipInfo *CClipStream::FindClipAt(int timestamp, int *returnClipNumber)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if(returnClipNumber) *returnClipNumber = 0;

   if (m_arrClip.GetSize() == 0)
      return NULL;

   for (int i = 0; i < m_arrClip.GetSize(); ++i)
   {
      CClipInfo *clip = m_arrClip[i];
      if (clip && 
          clip->GetTimestamp() <= timestamp &&
          clip->GetEndTime() > timestamp) {
         if(returnClipNumber) *returnClipNumber = i+1;
         return clip;
      }
   }

   return NULL;
}

CClipInfo *CClipStream::FindClip(CString &clipFilename, int clipStart, int clipLength)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_arrClip.GetSize() == 0)
      return NULL;

   for (int i = 0; i < m_arrClip.GetSize(); ++i)
   {
      CClipInfo *clip = m_arrClip[i];
      if (clip && 
         (clipFilename == clip->GetFilename()) &&
         (clipStart == clip->GetTimestamp()) &&
         (clipLength == clip->GetLength()))
         return clip;
   }

   return NULL;
}

bool CClipStream::HasClips()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_arrClip.GetSize() > 0)
      return true;

   return false;
}

int CClipStream::GetSize()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return m_arrClip.GetSize();
}

void CClipStream::WriteToLrd(HANDLE lrdFile, LPCTSTR prefix, int targetBegin, int targetEnd, int sourceBegin, int sourceEnd)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   for (int i = 0; i < m_arrClip.GetSize(); ++i)
   {
      CClipInfo *clipInfo = m_arrClip[i];
      if (clipInfo && 
          ((clipInfo->GetTimestamp() + clipInfo->GetLength()) >= sourceBegin) && 
          (clipInfo->GetTimestamp() < sourceEnd))
      {
         CString line;
         DWORD written;
         CString sgmlString;
         CString filename;
         filename.Format(_T("%s_clip_%d.avi"), prefix, clipInfo->GetTimestamp());
         StringManipulation::TransformForSgml(filename, sgmlString);
         line.Format(_T("        <OPT>%s</OPT>\n"), sgmlString);
         WriteFile(lrdFile, line, line.GetLength(), &written, false);
         line.Format(_T("        <OPT>%d</OPT>\n"), clipInfo->GetTimestamp() + (targetBegin - sourceBegin));
         WriteFile(lrdFile, line, line.GetLength(), &written, false);
      }
   }
}

void CClipStream::Fill(CList<CExportClip *, CExportClip *> &exportClipList, int targetBegin, int targetEnd, int sourceBegin, int sourceEnd)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (targetBegin == targetEnd)
      return;

   CExportClip *lastClip = NULL;
   
   if (!exportClipList.IsEmpty())
      lastClip = exportClipList.GetTail();

   bool isFirst = true;
   for (int i = 0; i < m_arrClip.GetSize(); ++i)
   {
      CClipInfo *clipInfo = m_arrClip[i];
      if (clipInfo && 
          ((clipInfo->GetTimestamp() + clipInfo->GetLength()) >= sourceBegin) && 
          (clipInfo->GetTimestamp() < sourceEnd))
      { 
         CTimeSpread *timeSpread = new CTimeSpread();
         if (clipInfo->GetTimestamp() >= sourceBegin)
            timeSpread->m_iBegin = 0;
         else
            timeSpread->m_iBegin = sourceBegin - clipInfo->GetTimestamp();

         if (clipInfo->GetTimestamp() + clipInfo->GetLength() <= sourceEnd)
            timeSpread->m_iEnd = clipInfo->GetLength();
         else
            timeSpread->m_iEnd = sourceEnd - clipInfo->GetTimestamp();

         // In special cases, like after removing a video clip,
         // a zero-length clip sneaks in here. Let's remove it
         // by checking that this export clip has a positive length.
         if (timeSpread->m_iEnd - timeSpread->m_iBegin > 0)
         {
            if (isFirst && lastClip && 
                (_tcscmp(lastClip->m_clipInfo->GetFilename(), clipInfo->GetFilename()) == 0) &&
                (!lastClip->m_cutList.IsEmpty()) && 
                (lastClip->m_targetBegin + lastClip->GetLength() >= clipInfo->GetTimestamp() + timeSpread->m_iBegin + (targetBegin - sourceBegin)))
            {
               lastClip->m_cutList.AddTail(timeSpread);
            }
            else
            {
               CExportClip *newClip = new CExportClip();
               newClip->m_clipInfo = clipInfo;
               newClip->m_targetBegin = clipInfo->GetTimestamp() + (targetBegin - sourceBegin) + timeSpread->m_iBegin;
               newClip->m_cutList.AddTail(timeSpread);
               exportClipList.AddTail(newClip);
            }
            isFirst = false;
         }
         else
         {
            // If the clip has a length of zero,
            // we don't need this CTimeSpread,
            // so let's delete it.
            delete timeSpread;
         }
      }
   }
}

void CClipStream::WriteToLep(HANDLE lepFile, int targetBegin, int targetEnd, int sourceBegin, int sourceEnd, LPCTSTR szLepFileName)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   for (int i = 0; i < m_arrClip.GetSize(); ++i)
   {
      CClipInfo *clipInfo = m_arrClip[i];
      if (clipInfo && 
          ((clipInfo->GetTimestamp() + clipInfo->GetLength()) >= sourceBegin) &&
          (clipInfo->GetTimestamp() < sourceEnd))
      {
         CString line;
         DWORD written;
         CString filenameSgml = StringManipulation::MakeRelativeAndSgml(szLepFileName, clipInfo->GetFilename());
         CString audioFileSgml = _T("");
         if (clipInfo->HasAudioStream())
            audioFileSgml = StringManipulation::MakeRelativeAndSgml(szLepFileName, clipInfo->GetAudioFilename());
         CString origAudioFileSgml = _T("");
         if (clipInfo->HasAudioStream() && clipInfo->HasOriginalAudioSource())
            origAudioFileSgml = StringManipulation::MakeRelativeAndSgml(szLepFileName, clipInfo->GetOriginalAudioSource());
         // StringManipulation::TransformForSgml(clipInfo->GetFilename(), filenameSgml);
         CString titleSgml;
         StringManipulation::TransformForSgml(clipInfo->GetTitle(), titleSgml);
         int clipSourceStart = clipInfo->GetTimestamp();
         int clipSourceEnd   = clipSourceStart + clipInfo->GetLength();
         CString csKeywords, csKeywordsSgml;
         clipInfo->GetKeywords(csKeywords);
         StringManipulation::TransformForSgml(csKeywords, csKeywordsSgml);
         CString csTitleSgml;
         StringManipulation::TransformForSgml(clipInfo->GetTitle(), csTitleSgml);
         line.Format(_T("        <source file=\"%s\" clip-start=\"%d\" clip-end=\"%d\" title=\"%s\" keywords=\"%s\" stream-selector=\"%d\" audio-file=\"%s\" audio-stream-selector=\"%d\" orig-audio-file=\"%s\" orig-audio-stream-selector=\"%d\">%s</source>\n"),
            filenameSgml, clipSourceStart, clipSourceEnd, 
            csTitleSgml, csKeywordsSgml, 
            clipInfo->GetStreamSelector(), audioFileSgml, clipInfo->GetAudioStreamSelector(), 
            origAudioFileSgml, clipInfo->GetOriginalAudioSourceStream(), titleSgml);
         WriteFile(lepFile, line, line.GetLength(), &written, false);
      }
   }
}

CClipInfo *CClipStream::FindClip(int clipNumber, int fromMsec, int toMsec)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_arrClip.GetSize() == 0)
      return NULL;

   CClipInfo *retClip = NULL;

   int actClip = -1;
   for (int i = 0; i < m_arrClip.GetSize(); ++i)
   {
      retClip = m_arrClip[i];
      if (retClip)
      {
         int clipBegin = retClip->GetTimestamp();
         int clipEnd   = retClip->GetTimestamp() + retClip->GetLength() ;

         if ((clipEnd > fromMsec && clipEnd <= toMsec) ||
             (clipBegin >= fromMsec && clipBegin < toMsec) ||
             (clipBegin < fromMsec && clipEnd > toMsec))
         {
            ++actClip;
            if (actClip == clipNumber)
               return retClip;
         }
      }
   }

   return NULL;
}

void CClipStream::GetClips(CArray<CClipInfo *, CClipInfo *> &clips, int fromMsec, int toMsec, int offset, int &firstPos, int &lastPos)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_arrClip.GetSize() == 0)
      return;

   bool isFirst = true;
   for (int i = 0; i < m_arrClip.GetSize(); ++i)
   {
      CClipInfo *clipInfo = m_arrClip[i];
      if (clipInfo)
      {
         int clipBegin = clipInfo->GetTimestamp() + offset;
         int clipEnd   = clipInfo->GetTimestamp() + clipInfo->GetLength() + offset;
         if ((clipEnd > fromMsec && clipEnd <= toMsec) ||
             (clipBegin >= fromMsec && clipBegin < toMsec) ||
             (clipBegin < fromMsec && clipEnd > toMsec))
         {
            CClipInfo *newClip = clipInfo->Copy(); 
            newClip->SetTimestamp(clipBegin);
            newClip->SetLength(clipEnd - clipBegin);
            clips.Add(newClip);
            if (isFirst)
               firstPos = clips.GetSize() -1;
            isFirst = false;
         }
      }
   }

   // at least one clip is inserted
   if (!isFirst)
      lastPos = clips.GetSize() - 1;
}
