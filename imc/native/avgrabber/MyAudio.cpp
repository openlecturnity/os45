#include "global.h"
#include "resource.h"

#include "LanguageSupport.h"
//#include "MyAudio.h"
//#include <vfw.h>

using namespace std;

#ifdef _DEBUG
#define _DEBUGFILE
#endif

//int AudioCapturer::silenceTimeout_ = 30000;
#ifdef _DEBUGFILE
   void AudioDebugMsg(char *szFormat)
   {
      static FILE *debugFile = NULL;

      if (!debugFile)
         debugFile = fopen("C:\\audio_debug.txt", "w");
      fprintf(debugFile, "%05d - Thread 0x%08x: ", ::timeGetTime()%100000, GetCurrentThreadId());
      fprintf(debugFile, szFormat);
      fflush(debugFile);
   }
#endif

void AvGrabberDebugMsg(char *szFormat, bool bPrefix);

/*
* This function first checks if the given device (nDeviceID)
* can manage the required parameters, and then, if successful,
* it opens the audio port for capture purposes.
*/
AudioCapturer::AudioCapturer(UINT nDeviceID, int nSampleRate, 
				 int nBitsPerSample, int nChannels)
{
#ifdef _DEBUGFILE
   AudioDebugMsg("AudioCapturer\n");
#endif
   if (nDeviceID >= getDeviceCount())
		throw AudioException(Local::localizeString(AUDIO_NODEVICE));

   // initialise all global buffers and crap
	init();

	firstSysTimeMs_ = -1;
	bytesWritten_   = 0;
	currentFrameMs_ = -1;
	audioFile_      = NULL;
	isPaused_       = true; // not in progress

	dataLost_ = false;
   dryCaptureMode_ = false;

   // for the click noise:
   pClickNoiseData_   = NULL;
   lClickNoiseLength_ = 0;
   clickTimeListLock_ = false;

	MMRESULT res;
	WAVEFORMATEX wfe;
	bool formatOk = false;
	
#ifdef DEBUG
	cout << "Audio::open(" << nDeviceID << ", " << nSampleRate << ", " <<
		nBitsPerSample << ", " << nChannels << ")" << endl;
#endif
	
	nBlockAlign_    = nChannels * nBitsPerSample / 8;
	nBitsPerSample_ = nBitsPerSample;
	nSampleRate_    = nSampleRate;
	nChannels_      = nChannels;
	nDeviceID_      = nDeviceID;
	
	// save settings for later:
	/*
	settings_.nBitsPerSample = nBitsPerSample;
	settings_.nChannels      = nChannels;
	settings_.nSamplesPerSec = nSampleRate;
	settings_.nAvgBytesPerMs = ((float) (nSampleRate * nBlockAlign_)) / 1000.0;
	*/
	
	// fill the WAVEFORMATEX structure as needed:
	wfe.wFormatTag      = WAVE_FORMAT_PCM;
	wfe.nChannels       = nChannels;
	wfe.nSamplesPerSec  = nSampleRate;
	wfe.nAvgBytesPerSec = nSampleRate * nBlockAlign_;
	wfe.nBlockAlign     = nBlockAlign_;
	wfe.wBitsPerSample  = nBitsPerSample;
	wfe.cbSize = 0;

#ifdef _DEBUGFILE
      AvGrabberDebugMsg("AudioCapturer::1\n", true);
#endif
	
	res = waveInOpen(NULL, nDeviceID, &wfe, CALLBACK_NULL,
		NULL, WAVE_FORMAT_QUERY);

#ifdef _DEBUGFILE
      char szMessage[300];
      sprintf(szMessage, "AudioCapturer::2 -> %d\n", res);
      
      AvGrabberDebugMsg(szMessage, true);
#endif

	
	switch (res)
	{
	case MMSYSERR_NOERROR:
		cerr << "Driver supports " << nSampleRate << " Hz, " <<
			nBitsPerSample << " Bits, " << nChannels << " channels." <<
			endl;
		formatOk = true;
		break;
		
	case WAVERR_BADFORMAT:
		cerr << "Driver does not support " << nSampleRate << " Hz, " <<
			nBitsPerSample << " Bits, " << nChannels << " channels." <<
			endl;
		formatOk = false;
		break;
		
	default:
		cerr << "Other error: " << res << endl;
		formatOk = false;
		break;
	}
	
	if (!formatOk)
		throw AudioException(Local::localizeString(AUDIO_FORMAT_ERR));
}

AudioCapturer::~AudioCapturer()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("~AudioCapturer\n");
#endif
	deInit();
}

// ================================================== 
// ================================================== 
// ================================================== 

inline short convertStereoToMono16(char *pSource)
{
   return (short) ((long) ((long) (*(short *) pSource) + (*(short *) (pSource + 2))) / 2);
}

inline unsigned char convertStereoToMono8(char *pSource)
{
   return (unsigned char) 
          ( ( (unsigned long) (*( (unsigned char *) pSource) )
                         + (*((unsigned char *) (pSource + 1)) )
                                                                  ) / 2);
}

inline short convert8To16(unsigned char src)
{
   // long t = (((long) src) - 128) * 256;
   return ((((long) src) - 128) * 256);
}

inline unsigned char convert16To8(short src)
{
   short t = src / 256 + 128;
   if (t < 0)
      return (unsigned char) 0;
   if (t > 255)
      return (unsigned char) 255;
   return (unsigned char) t;
}

unsigned char addSamples8(unsigned long s1, unsigned long s2, double w1, double w2)
{
   unsigned long t = (unsigned long) (w1*s1 + w2*s2);
   if (t > 255)
      return (unsigned char) 255;
   return (unsigned char) t;
}

short addSamples16(long s1, long s2, double w1, double w2)
{
   long t = (long) (w1*s2 + w2*s2);
   if (t < -32768)
      return (short) -32768;
   
   if (t > 32767)
      return (short) 32767;
   return (short) t;
}

void writeWavFile(_TCHAR *tszFileName, WAVEFORMATEX *pWfe, char *pData, long lLength)
{
   HMMIO hmmio = mmioOpen(tszFileName, NULL, MMIO_WRITE | MMIO_CREATE);
   MMCKINFO riffChunk;
   riffChunk.cksize = 0;
   riffChunk.fccType = 'EVAW';
   MMRESULT mmres = mmioCreateChunk(hmmio, &riffChunk, MMIO_CREATERIFF);
   MMCKINFO fmtChunk;
   fmtChunk.cksize = 0;
   fmtChunk.ckid = ' tmf';
   mmres = mmioCreateChunk(hmmio, &fmtChunk, 0);
   mmres = mmioWrite(hmmio, (const char *) pWfe, sizeof WAVEFORMATEX);
   mmres = mmioAscend(hmmio, &fmtChunk, 0);
   MMCKINFO dataChunk;
   dataChunk.cksize = 0;
   dataChunk.ckid = 'atad';
   mmres = mmioCreateChunk(hmmio, &dataChunk, 0);
   mmres = mmioWrite(hmmio, pData, lLength);
   mmres = mmioAscend(hmmio, &dataChunk, 0);
   mmres = mmioAscend(hmmio, &riffChunk, 0);
   mmres = mmioClose(hmmio, 0);
}

// ================================================== 

void AudioCapturer::initClickNoise(WAVEFORMATEX *pWfeSrc, char  *pSrcData, long  lSrcLength,
                                   WAVEFORMATEX *pWfeTrg, char **pTrgData, long *lTrgLength)
{
#ifdef _DEBUGFILE
   AudioDebugMsg("initClickNoise\n");
#endif
   // new sample length (in samples):
   double sampleFactor = ((double) pWfeTrg->nSamplesPerSec) / ((double) pWfeSrc->nSamplesPerSec);
   (*lTrgLength) = (LONG) (sampleFactor * lSrcLength);
#ifdef _DEBUG
   printf("New sample length: %d\n", (*lTrgLength));
#endif

   (*pTrgData) = new char[(*lTrgLength) * pWfeTrg->nBlockAlign];
   double dTargetSample;
   int nSample1, nSample2;
   double dWeight1, dWeight2;
   bool nSameSampleRate = (pWfeTrg->nSamplesPerSec == pWfeSrc->nSamplesPerSec);

   for (int i=0; i<(*lTrgLength); ++i)
   {
      dTargetSample = ((double) i) / sampleFactor;
      if (!nSameSampleRate)
      {
         nSample1 = (int) dTargetSample;
         nSample2 = (int) (dTargetSample + 1.0);
         dWeight1 = 1.0 - (dTargetSample - ((double) nSample1));
         dWeight2 = 1.0 - dWeight1;
         // dWeight1 = 0.5; // 1.0 - (dTargetSample - ((double) nSample1));
         // dWeight2 = 0.5; // 1.0 - dWeight1;
         if (nSample2 >= lSrcLength)
            nSample2 = lSrcLength - 1;
      }
      else
      {
         nSample1 = (int) (dTargetSample + .5);
         nSample2 = (int) (dTargetSample + .5);
         dWeight1 = 1.0;
         dWeight2 = 0.0;
      }

      if (pWfeSrc->wBitsPerSample == 16)
      {
         // source sample has 16 bits.
         if (pWfeTrg->wBitsPerSample == 16)
         {
            // target also has 16 bits.
            if (pWfeSrc->nChannels == 2)
            {
               // source is stereo, 16 bits, target 16 bits, mono
               long sample1 = convertStereoToMono16(&pSrcData[nSample1 * pWfeSrc->nBlockAlign]);
               long sample2 = convertStereoToMono16(&pSrcData[nSample2 * pWfeSrc->nBlockAlign]);
               ((short *) (*pTrgData))[i] = addSamples16(sample1, sample2, dWeight1, dWeight2);
            }
            else
            {
               // source is mono, 16 bits, target 16 bits, mono
               long sample1 = ((short *) pSrcData)[nSample1];
               long sample2 = ((short *) pSrcData)[nSample2];
               ((short *) (*pTrgData))[i] = addSamples16(sample1, sample2, dWeight1, dWeight2);
            }
         }
         else
         {
            // target has 8 bits.
            if (pWfeSrc->nChannels == 2)
            {
               // source is stereo, 16 bits, target is 8 bits, mono
               unsigned long sample1 = convert16To8(convertStereoToMono16(&pSrcData[nSample1 * pWfeSrc->nBlockAlign]));
               unsigned long sample2 = convert16To8(convertStereoToMono16(&pSrcData[nSample2 * pWfeSrc->nBlockAlign]));
               // printf("s1 == %d, s2 == %d\n", sample1, sample2, dWeight1, dWeight2);
               (*pTrgData)[i] = addSamples8(sample1, sample2, dWeight1, dWeight2); //(signed char) ((sample1 + sample2) / 2);
            }
            else
            {
               // source is mono, 16 bits, target is 8 bits, mono
               unsigned long sample1 = convert16To8(((short *) pSrcData)[nSample1]);
               unsigned long sample2 = convert16To8(((short *) pSrcData)[nSample2]);
               (*pTrgData)[i] = addSamples8(sample1, sample2, dWeight1, dWeight2);
            }
         }
      }
      else
      {
         // assume 8 bit
         if (pWfeTrg->wBitsPerSample == 8)
         {
            // target also has 8 bits
            if (pWfeSrc->nChannels == 2)
            {
               // source is stereo, 8 bit, target is 8 bit, mono
               unsigned long sample1 = convertStereoToMono8(&pSrcData[nSample1 * pWfeSrc->nBlockAlign]);
               unsigned long sample2 = convertStereoToMono8(&pSrcData[nSample2 * pWfeSrc->nBlockAlign]);
               (*pTrgData)[i] = addSamples8(sample1, sample2, dWeight1, dWeight2);
            }
            else
            {
               // source is mono, 8 bit, target is 8 bit, mono
               unsigned long sample1 = pSrcData[nSample1];
               unsigned long sample2 = pSrcData[nSample2];
               (*pTrgData)[i] = addSamples8(sample1, sample2, dWeight1, dWeight2);
            }
         }
         else
         {
            // target has 16 bits.
            if (pWfeSrc->nChannels == 2)
            {
               // source is stereo, 8 bit, target is 16 bit, mono
               long sample1 = convert8To16(convertStereoToMono8(&pSrcData[nSample1 * pWfeSrc->nBlockAlign]));
               long sample2 = convert8To16(convertStereoToMono8(&pSrcData[nSample2 * pWfeSrc->nBlockAlign]));
               ((short *) (*pTrgData))[i] = addSamples16(sample1, sample2, dWeight1, dWeight2);
            }
            else
            {
               // source is mono, 8 bit, target is 16 bit, mono
               long sample1 = convert8To16(pSrcData[nSample1]);
               long sample2 = convert8To16(pSrcData[nSample2]);
               ((short *) (*pTrgData))[i] = addSamples16(sample1, sample2, dWeight1, dWeight2);
            }
         }
      }
   }
}

void AudioCapturer::loadWavData(_TCHAR *tszFileName, char **pWavData, WAVEFORMATEX **ppWfe, long *plWavLength)
{
#ifdef _DEBUGFILE
   AudioDebugMsg("loadWavData\n");
#endif
   AVIFileInit();

   PAVIFILE pWavFile;
#ifdef _DEBUG
   _tprintf(_T("Opening \"%s\"...\n"), tszFileName);
#endif
   HRESULT hr = AVIFileOpen(&pWavFile, tszFileName, OF_READ, NULL);
   if (hr == 0)
   {
#ifdef _DEBUG
      printf("AVIFileOpen() succeeded.\n");
#endif

      PAVISTREAM pWavStream;
      hr = AVIFileGetStream(pWavFile, &pWavStream, streamtypeAUDIO, 0);

      if (hr == 0)
      {
#ifdef _DEBUG
         printf("AVIFileGetStream succeeded.\n");
#endif

         LONG lFormatSize = 0;
         hr = AVIStreamReadFormat(pWavStream, 0, NULL, &lFormatSize);
         if (hr == 0)
         {
#ifdef _DEBUG
            printf("Reading format data size succeeded.\n");
#endif
            (*ppWfe) = (WAVEFORMATEX *) new char[lFormatSize];

            hr = AVIStreamReadFormat(pWavStream, 0, (LPVOID) (*ppWfe), &lFormatSize);
            if (hr == 0)
            {
#ifdef _DEBUG
               printf("AVIStreamReadFormat() succeeded.\n");
               printf("Sample rate: %d\n", (*ppWfe)->nSamplesPerSec);
               printf("Sample size: %d\n", (*ppWfe)->wBitsPerSample);
               printf("Channels   : %d\n", (*ppWfe)->nChannels);
               printf("Block align: %d\n", (*ppWfe)->nBlockAlign);
               printf("PCM Format : ");
               if ((*ppWfe)->wFormatTag == WAVE_FORMAT_PCM)
                  printf("Yes\n");
               else
                  printf("No\n");
#endif

               LONG lLength = AVIStreamLength(pWavStream);
               if (lLength != -1)
               {
#ifdef _DEBUG
                  printf("AVIStreamLength() succeeded: %d samples.\n", lLength);
#endif
                  double lenSecs_ = ((double) lLength) / ((double) (*ppWfe)->nSamplesPerSec);
                  if (lenSecs_ <= 5.0)
                  {
                     LONG lBufLen = lLength * (*ppWfe)->nBlockAlign;
                     (*pWavData) = new char[lBufLen];

                     LONG lReadSamples;
                     LONG lReadBytes;
                     hr = AVIStreamRead(pWavStream, 0, lLength, (LPVOID) (*pWavData), lBufLen, &lReadBytes, &lReadSamples);
                     if (hr == 0)
                     {
   #ifdef _DEBUG
                        printf("AVIStreamRead() succeeded, read %d samples.\n", lReadSamples);
   #endif
                        (*plWavLength) = lLength;
                     }
                  }
                  else
                  {
                     ::MessageBox(NULL, Local::localizeString(AUDIO_CLICK_TOO_LONG), NULL, 
                                  MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
                  }
               }
            }
         }
         AVIStreamRelease(pWavStream);
      }

      AVIFileRelease(pWavFile);
   }

   if (!(*pWavData))
   {
      if ((*ppWfe) != NULL)
      {
         delete[] (*ppWfe);
         *ppWfe = NULL;
      }
   }

   AVIFileExit();
}

bool AudioCapturer::loadClickNoise(_TCHAR *tszWavFileName)
{
#ifdef _DEBUGFILE
   AudioDebugMsg("loadClickNoise\n");
#endif
   if (tszWavFileName == NULL)
   {
      // no click sound wanted
      if (pClickNoiseData_)
         delete[] pClickNoiseData_;
      pClickNoiseData_ = NULL;
      lClickNoiseLength_ = 0;
      return true;
   }

   WAVEFORMATEX *pWfeSource = NULL;
   char *pWavData = NULL;
   long  lWavLength = 0;

   loadWavData(tszWavFileName, &pWavData, &pWfeSource, &lWavLength);

   if (pWavData)
   {
      WAVEFORMATEX wfeTarget;
      wfeTarget.nSamplesPerSec = nSampleRate_;
      wfeTarget.nBlockAlign = nBlockAlign_;
      wfeTarget.nChannels = nChannels_;
      wfeTarget.wBitsPerSample = nBitsPerSample_;
      wfeTarget.cbSize = sizeof WAVEFORMATEX;
      wfeTarget.wFormatTag = WAVE_FORMAT_PCM;

      if (pClickNoiseData_)
         delete[] pClickNoiseData_;
      pClickNoiseData_ = NULL;
      lClickNoiseLength_ = 0;

      initClickNoise(pWfeSource, pWavData, lWavLength,
                     &wfeTarget, &pClickNoiseData_, &lClickNoiseLength_);

      delete[] pWfeSource;
      delete[] pWavData;
   }
   else
   {
       _TCHAR msg[256];
       _TCHAR cap[256];
       _tcscpy(msg, Local::localizeString(AUDIO_NO_CLICK_NOISE));//, audioSources_[activeSource_].tszName);
       _tcscpy(cap, Local::localizeString(MSGBOX_WARNING));
       MessageBox(NULL, msg, cap, MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
       //  MessageBox(NULL, Local::localizeString(AUDIO_NO_CLICK_NOISE), /*Local::localizeString(MSGBOX_WARNING)*/_T("Warning"), MB_OK | MB_TOPMOST);
       if (pWfeSource)
           delete[] pWfeSource;

       lClickNoiseLength_ = 0;
       pClickNoiseData_ = NULL;

       return false;
   }

   return true;
}

/*
 * Call this function before you open() an audio port
 * for capture purposes. This function initializes some
 * variables and allocates some memory for audio input
 * buffers.
 */
void AudioCapturer::init()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("init\n");
#endif
#ifdef DEBUG
	cout << "Audio::init()" << endl;
#endif
	bool res = false;
	
	handles_ = (WAVHandles*) malloc(sizeof(WAVHandles));
	if (handles_ == NULL)
	{
		cerr << "Could not allocate memory for WAVHandles struct!" << endl;
		throw MemoryAudioException(Local::localizeString(AUDIO_NOMEM));
		// return false;
	}
	
	handles_->audioEvent     = NULL;
	handles_->driverOpen     = false;
	handles_->active         = false;
	handles_->inProgressFlag = false;
	handles_->mmTime.wType   = TIME_BYTES;
	
	handles_->audioEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
	handles_->finishEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	if (handles_->audioEvent == NULL || handles_->finishEvent == NULL)
	{
		int res;
		cerr << "CreateEvent failed: ";
		res = GetLastError();
		switch (res)
		{
		case ERROR_INVALID_HANDLE:
			cerr << "ERROR_INVALID_HANDLE" << endl;
			break;
		case ERROR_ALREADY_EXISTS:
			cerr << "ERROR_ALREADY_EXISTS" << endl;
			break;
		default:
			cerr << res << "(?)" << endl;
			break;
		}
		
		// return false;
		throw AudioException(Local::localizeString(AUDIO_INIT_ERR));
	}
	
	// res = initDeviceList();
	
	/* --- create audio buffers --- */
	handles_->buffers = new WAVEHDR[WAVE_NUM_BUFFER];
	if (handles_->buffers == NULL)
	{
		cerr << "Not enough memory for buffer container!" << endl;
		// return false;
		throw MemoryAudioException(Local::localizeString(AUDIO_NOMEM));
	}
	
	res = true;
	
	for (int i=0; i<WAVE_NUM_BUFFER; i++)
	{
		handles_->buffers[i].lpData = (char *) malloc(WAVE_BUFFER_SIZE);
		if (handles_->buffers[i].lpData == NULL)
		{
			cerr << "Allocating audio buffer #" << i << " failed!" << endl;
			res = false;
			break;
		}
		
		if (res)
		{
			handles_->buffers[i].dwBufferLength = WAVE_BUFFER_SIZE;
			handles_->buffers[i].dwFlags        = 0;
		}
	}
	
	// return res;
	if (!res)
		throw MemoryAudioException(Local::localizeString(AUDIO_HEADER));
}

/*
* This function deinitializes the audio capture part.
* It frees up buffers and resets variables. This function
* should be called after your are ready with everything,
* i.e. after you have called stop() and close().
*/
bool AudioCapturer::deInit()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("deInit\n");
#endif
#ifdef DEBUG
	cout << "Audio::deInit()" << endl;
#endif
	if (handles_ != NULL)
	{
		int res;
		
		if (handles_->driverOpen)
		{
			close();
		}
		
		res = CloseHandle(handles_->audioEvent);
		if (res == 0)
		{
			cerr << "CloseHandle failed: " << GetLastError() << endl;
		}
		
		res = CloseHandle(handles_->finishEvent);
		if (res == 0)
		{
			cerr << "CloseHandle failed: " << GetLastError() << endl;
		}
		
		if (handles_->buffers != NULL)
		{
			for (int i=0; i<WAVE_NUM_BUFFER; i++)
			{
				if (handles_->buffers[i].lpData != NULL)
					free(handles_->buffers[i].lpData);
			}
			
			delete [] handles_->buffers;
			
			handles_->buffers = NULL;
		}
	}
	
	if (handles_ != NULL)
		free(handles_);
	handles_ = NULL;

   if (pClickNoiseData_)
      delete[] pClickNoiseData_;
   pClickNoiseData_ = NULL;
   lClickNoiseLength_ = 0;
   
	return true;
}


/*
* Prepares the input buffers for audio capture. This function
* is called from the open() function and is not for manual use.
*/
bool AudioCapturer::prepareHeaders()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("prepareHeaders\n");
#endif
	bool res = true;
	
#ifdef DEBUG
	cout << "Audio::prepareHeaders()" << endl;
#endif
	
	for (int i=0; i<WAVE_NUM_BUFFER; i++)
	{
		MMRESULT mmres;
		
		mmres = waveInPrepareHeader(handles_->waveIn, 
			&handles_->buffers[i], 
			sizeof(handles_->buffers[i]));
		
		switch (mmres)
		{
		case MMSYSERR_NOERROR:
			break;
			
		case MMSYSERR_INVALHANDLE:
			cerr << "Invalid handle (MMSYSERR_INVALHANDLE) at buffer #" << i << endl;
			res = false;
			break;
			
		case MMSYSERR_NODRIVER:
			cerr << "No device driver present (MMSYSERR_NODRIVER) at buffer #" << i << endl;
			res = false;
			break;
			
		case MMSYSERR_NOMEM:
			cerr << "Unable to allocate or lock memory (MMSYSERR_NOMEM) at buffer #" << i << endl;
			res = false;
			break;
			
		default:
			cerr << "Unknown error: " << mmres << endl;
			res = false;
			break;
		}
		
		if (mmres == MMSYSERR_NOERROR)
		{
#ifdef DEBUG
			cout << "Adding audio buffer #" << i << "." << endl;
#endif
			mmres = waveInAddBuffer(handles_->waveIn, 
				&handles_->buffers[i], 
				sizeof(handles_->buffers[i]));
			if (mmres != MMSYSERR_NOERROR)
			{
				cerr << "Buffer #" << i << " could not be added! Error == " << mmres << endl;
				res = false;
			}
		}
	}
	
	return true;
}

/*
* Creates and opens a WAV file for output of the data read
* from the audio capture port
*/
void AudioCapturer::createWavFile()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("createWavFile\n");
#endif
   if (useLadFormat_)
   {
	   audioFile_ = new AudioFile(szFileName_,
   		nSampleRate_,
		   nBitsPerSample_,
		   nChannels_,
         szLadInfo_);
   }
   else
   {
      audioFile_ = new AudioFile(szFileName_,
         nSampleRate_,
         nBitsPerSample_,
         nChannels_);
   }
}

/*
* Closes the audio device. This will not work if capturing is
* in progress.
*/
bool AudioCapturer::close()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("close\n");
#endif
	MMRESULT res;
	
#ifdef DEBUG
	cout << "Audio::close()" << endl;
#endif

   if (handles_->active)
	{
		cerr << "Capturing in progress!" << endl;
		return false;
	}
	
	if (audioFile_ != NULL && !dryCaptureMode_)
	{
		audioFile_->close();
		delete audioFile_;
	}
	
	if (handles_ == NULL)
	{
		cerr << "No handles open!" << endl;
		return false;
	}
	
	if (handles_->driverOpen == false)
	{
		cerr << "Driver not open!" << endl;
		return true;
	}
	
	if (handles_->waveIn == NULL)
	{
		cerr << "Driver handle not found!" << endl;
		return false;
	}
	
	res = waveInReset(handles_->waveIn);
	
	unPrepareHeaders();
	
	res = waveInClose(handles_->waveIn);
	
	if (res != MMSYSERR_NOERROR)
	{
		cerr << "waveInClose() returned " << res << "!" << endl;
		return false;
	}
	
	handles_->driverOpen = false;
	
	return true;
}

/*
* This function is called from the close() function. It unprepares
* the headers used for audio captures (see also prepareHeaders()).
*/
bool AudioCapturer::unPrepareHeaders()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("unPrepareHeaders\n");
#endif
#ifdef DEBUG
	cout << "Audio::unPrepareHeaders()" << endl;
#endif
	
	MMRESULT mmres;
	bool res = true;
	
	if (handles_ == NULL)
		return false;
	
	if (handles_->buffers == NULL)
		return false;
	
	res = true;
	
	for (int i=0; i<WAVE_NUM_BUFFER; i++)
	{
		mmres = waveInUnprepareHeader(handles_->waveIn, 
										  &handles_->buffers[i],
										  sizeof(handles_->buffers[i]));
		if (mmres != MMSYSERR_NOERROR)
		{
			cerr << "Note: waveInUnprepareHeader() failed on buffer #" << i << endl;
			res = false;
		}
	}
	
	return res;
}

void AudioCapturer::useLadFormat(char* szLadInfo)
{
#ifdef _DEBUGFILE
   AudioDebugMsg("useLadFormat\n");
#endif
   if (szLadInfo != NULL)
   {
      useLadFormat_ = true;
      strcpy(szLadInfo_, szLadInfo);
   }
   else
   {
      useLadFormat_ = false;
      strcpy(szLadInfo_, "u;1.0.0;00:00:00;01.01.01");
   }
}

/*
* Starts the audio capture sequence. This procedure returns
* (almost) immediately after having created a new thread
* for the capture process. See also captureThreadProc().
* Stop the capturing by using the stop() function.
*/
bool AudioCapturer::start(_TCHAR* tszFileName, bool bStartPaused)
{
#ifdef _DEBUGFILE
   AudioDebugMsg("start\n");
#endif
#ifdef DEBUG
	cout << "Audio::start()" << endl;
#endif

   // are we in "dry" mode?
   if (tszFileName == NULL)
      dryCaptureMode_ = true;
   else
      dryCaptureMode_ = false;

   // copy the file name
   if (!dryCaptureMode_)
	   _tcscpy(szFileName_, tszFileName);

	// the driver says it can handle the requested format,
	// so let's open it

	WAVEFORMATEX wfe;
	// fill the WAVEFORMATEX structure as needed:
	wfe.wFormatTag      = WAVE_FORMAT_PCM;
	wfe.nChannels       = nChannels_;
	wfe.nSamplesPerSec  = nSampleRate_;
	wfe.nAvgBytesPerSec = nSampleRate_ * nBlockAlign_;
	wfe.nBlockAlign     = nBlockAlign_;
	wfe.wBitsPerSample  = nBitsPerSample_;
	wfe.cbSize = 0;

   lClickTimeBytes_ = -10000000;

   for (int i=0; i<16; ++i)
      clickTimeList_[i] = 0;

	MMRESULT res = waveInOpen(&(handles_->waveIn), nDeviceID_, &wfe, 
		(DWORD) handles_->audioEvent, 0, CALLBACK_EVENT);
	if (res == MMSYSERR_NOERROR)
	{
		// everything's fine, let's prepare the wavein buffers
		handles_->driverOpen = true;
		bool res = prepareHeaders();
		if (!res)
			throw AudioException(Local::localizeString(AUDIO_HEADER));
	}
	else
	{
		throw AudioException(Local::localizeString(AUDIO_INIT_ERR));
		//cerr << "waveInOpen() failed! Error == " << res << endl;
		//return false;
	}

	// create the WAV file, if not in dry mode
   if (!dryCaptureMode_)
	   createWavFile();
	
	bytesWritten_     = 0;
	pauseByteCounter_ = 0;
	dataLost_         = false;
	isPaused_         = bStartPaused;

   clickTimeListPtr_ = 0;

	firstSysTimeMs_   = -1;
	currentFrameMs_   = -1;

   audioSilenceCount_ = 1;
   audioSilenceSum_   = 0;
   audioSilenceState_ = AUDIO_NORMAL;

   // calculate the block length in milliseconds
   audioBlockLengthMs_ = (1000.0 * WAVE_BUFFER_SIZE) / (1.0 * nSampleRate_ * nBlockAlign_);
	
	handles_->inProgressFlag = true;
	
	//thread = CreateThread(NULL, NULL, captureThreadProc, this, 0, &threadId);
	//if (thread == NULL)
	//{
	//	cerr << "CreateThread() failed: Error == " << GetLastError() << endl;
   //   throw new AudioException(Local::localizeString(AUDIO_NOTHREAD));
	//}
   unsigned int thread = 
      _beginthread(captureThreadProc, 0, this);

	// audio needs _high_ priority
	SetThreadPriority((HANDLE) thread, THREAD_PRIORITY_HIGHEST);

	return true;
}

void AudioCapturer::pause()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("pause\n");
#endif
	isPaused_ = true;
}

void AudioCapturer::unPause()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("unPause\n");
#endif
#ifdef _DEBUG
   printf("audio paused time: == %d\n", pauseByteCounter_ * 1000 / nBlockAlign_ / nSampleRate_);
#endif
	isPaused_ = false;
}

/*
* This is a thread function called by _beginthread
* in the start() procedure. It handles the capturing
* process by using Event handles to communicate with
* the the hardware.
*/
// DWORD WINAPI AudioCapturer::captureThreadProc(LPVOID parameter)
void __cdecl AudioCapturer::captureThreadProc(LPVOID parameter)
{
   CLanguageSupport::SetThreadLanguage();

#ifdef _DEBUGFILE
   AudioDebugMsg("captureThreadProc\n");
#endif
#ifdef DEBUG
	cout << "Audio::captureThreadProc(" << parameter << ")" << endl;
#endif
	AudioCapturer* ac = (AudioCapturer*) parameter;

	MMRESULT mmres;
	
	ResetEvent(ac->handles_->audioEvent);
	
#ifdef DEBUG
	cout << "Before start: waveInGetPosition() == " << ac->getWaveInTimeSamples() << endl;
#endif
	mmres = waveInStart(ac->handles_->waveIn);
	
	ac->firstSysTimeMs_ = getSystemTime();
	
	if (mmres != MMSYSERR_NOERROR)
	{
		cerr << "waveInStart() failed! Error == " << mmres << endl;
		return; // mmres;
	}
	
	// set state:
	ac->handles_->active = true;
	
	int bufNr = 0; // in {0, ..., WAVE_NUM_BUFFERS-1}
	int lastBufNr = bufNr;
	
	bool exitRequest = false;
	
	while (!exitRequest)
	{
		WaitForSingleObject(ac->handles_->audioEvent, INFINITE);
		ResetEvent(ac->handles_->audioEvent);
		
		if (!ac->handles_->inProgressFlag)
			exitRequest = true;
		
		bool firstBuffer = true;
		lastBufNr = bufNr;
		
		while ((ac->handles_->buffers[bufNr].dwFlags & WHDR_DONE) > 0)
		{
			if (!firstBuffer)
			{
				if (lastBufNr == bufNr)
				{
					// boy, we're in trouble
					break;
				}
			}
			
         ac->calcAudioLevel(bufNr);
            
			if (!ac->isPaused_)
			{
				// first add eventual click sound, then write the buffer
				ac->writeBuffer(bufNr);
			}
			else
			{
				ac->pauseByteCounter_ += ac->handles_->buffers[bufNr].dwBytesRecorded;
			}
			
			bufNr++;
			if (bufNr == WAVE_NUM_BUFFER)
				bufNr = 0;
			
			firstBuffer = false;
		}
		
		if (lastBufNr == bufNr && !ac->isPaused_) // ignore data loss if paused
		{
			ac->dataLost_ = true;
#ifdef _DEBUG
			cerr << "------> Out of buffer!" << endl;
         AudioDebugMsg("~~~~~ Out of buffer\n");
#endif
		}
		
		// re-add the buffers:
		{
			int tmpBufNr = lastBufNr;
			bool first = true;
			while (tmpBufNr != bufNr || first)
			{
				mmres = waveInAddBuffer(ac->handles_->waveIn, 
					&ac->handles_->buffers[tmpBufNr],
					sizeof(ac->handles_->buffers[tmpBufNr]));
				if (mmres != MMSYSERR_NOERROR)
				{
					cerr << "Could not re-add buffer #" << tmpBufNr << endl;
				}
				
				tmpBufNr++;
				if (tmpBufNr == WAVE_NUM_BUFFER)
					tmpBufNr = 0;
				first = false;
			}
		}
	}
	
	ac->handles_->active = false;
	
	// signalize we're done!
	SetEvent(ac->handles_->finishEvent);
	
	return; // 0;
}
	
void AudioCapturer::writeBuffer(int bufferNr)
{
#ifdef _DEBUGFILE
   AudioDebugMsg("writeBuffer\n");
#endif
   if (!dryCaptureMode_)
   {
      if (pClickNoiseData_)
      {
         clickTimeListLock_ = true;
         for (int i=0; i<clickTimeListPtr_; ++i)
         {
            
            long bufferStart = bytesWritten_;
            long bufferEnd   = bytesWritten_ + handles_->buffers[bufferNr].dwBytesRecorded;
            long sampleStart = clickTimeList_[i]; // lClickTimeBytes_;
            long sampleEnd   = clickTimeList_[i]; // lClickTimeBytes_;
            switch (nBitsPerSample_)
            {
            case 8: 
               sampleEnd += lClickNoiseLength_; 
               //((unsigned char*) handles_->buffers[bufferNr].lpData)[0] = 255;
               break;
            default:
            case 16:
               sampleEnd += lClickNoiseLength_ * 2;
               //((short *) handles_->buffers[bufferNr].lpData)[0] = 32767;
               break;
            }
            
            // do we have a sample/buffer overlap?
            if (!((sampleEnd < bufferStart) || (sampleStart > bufferEnd)) && pClickNoiseData_)
            {
#ifdef _DEBUG
               //printf("Sample: [%d, %d), Buffer: [%d, %d) overlap!\n", sampleStart, sampleEnd, bufferStart, bufferEnd);
#endif
               // [a,b] cut [c,d] is not empty?
               // <=> not ((d < a) || (c > b))
               
               if (nBitsPerSample_ == 8)
               {
                  // 8 bits per sample
                  if (sampleStart >= bufferStart && sampleStart < bufferEnd)
                  {
                     // start time is in the current buffer
                     if (sampleEnd < bufferEnd)
                     {
                        // sample to insert is totally contained in buffer:
                        mixAudioStreams8((unsigned char *) handles_->buffers[bufferNr].lpData,
                           handles_->buffers[bufferNr].dwBytesRecorded,
                           clickTimeList_[i] - bytesWritten_,
                           (unsigned char *) pClickNoiseData_,
                           lClickNoiseLength_, 
                           0);
                     }
                     else
                     {
                        // mix first part of sample
                        mixAudioStreams8((unsigned char *) handles_->buffers[bufferNr].lpData,
                           handles_->buffers[bufferNr].dwBytesRecorded,
                           clickTimeList_[i] - bytesWritten_,
                           (unsigned char *) pClickNoiseData_,
                           lClickNoiseLength_, 
                           0);
                     }
                  }
                  else if (sampleStart <= bufferStart && 
                     sampleEnd > bufferEnd)
                  {
                     // sample to insert covers whole buffer
                     mixAudioStreams8((unsigned char *) handles_->buffers[bufferNr].lpData,
                        handles_->buffers[bufferNr].dwBytesRecorded,
                        0,
                        (unsigned char *) pClickNoiseData_,
                        lClickNoiseLength_,
                        bytesWritten_ - clickTimeList_[i]);
                  }
                  else if (sampleEnd >= bufferStart &&
                     sampleEnd < bufferEnd)
                  {
                     // sample ends in current buffer
                     mixAudioStreams8((unsigned char *) handles_->buffers[bufferNr].lpData,
                        handles_->buffers[bufferNr].dwBytesRecorded,
                        0,
                        (unsigned char *) pClickNoiseData_,
                        lClickNoiseLength_,
                        bytesWritten_ - clickTimeList_[i]);
                  }
                  
               }
               else
               {
                  // 16 bits per sample
                  if (sampleStart >= bufferStart && sampleStart < bufferEnd)
                  {
                     // start time is in the current buffer
                     if (sampleEnd < bufferEnd)
                     {
                        //printf("--> sample totally contained.\n");
                        // sample to insert is totally contained in buffer:
                        mixAudioStreams16((short *) handles_->buffers[bufferNr].lpData,
                           handles_->buffers[bufferNr].dwBytesRecorded / 2,
                           (clickTimeList_[i] - bytesWritten_) / 2,
                           (short *) pClickNoiseData_,
                           lClickNoiseLength_, 
                           0);
                     }
                     else
                     {
                        //printf("--> mix first part of sample.\n");
                        // mix first part of sample
                        mixAudioStreams16((short *) handles_->buffers[bufferNr].lpData,
                           handles_->buffers[bufferNr].dwBytesRecorded / 2,
                           (clickTimeList_[i] - bytesWritten_) / 2,
                           (short *) pClickNoiseData_,
                           lClickNoiseLength_, 
                           0);
                     }
                  }
                  else if (sampleStart <= bufferStart && 
                     sampleEnd > bufferEnd)
                  {
                     //printf("--> sample to insert covers whole buffer.\n");
                     // sample to insert covers whole buffer
                     mixAudioStreams16((short *) handles_->buffers[bufferNr].lpData,
                        handles_->buffers[bufferNr].dwBytesRecorded / 2,
                        0,
                        (short *) pClickNoiseData_,
                        lClickNoiseLength_,
                        (bytesWritten_ - clickTimeList_[i]) / 2);
                  }
                  else if (sampleEnd >= bufferStart &&
                     sampleEnd < bufferEnd)
                  {
                     //printf("--> sample ends in buffer.\n");
                     // sample ends in current buffer
                     mixAudioStreams16((short *) handles_->buffers[bufferNr].lpData,
                        handles_->buffers[bufferNr].dwBytesRecorded / 2,
                        0,
                        (short *) pClickNoiseData_,
                        lClickNoiseLength_,
                        (bytesWritten_ - clickTimeList_[i]) / 2);
                  }
                  else
                  {
                     printf("<-> What am I doing here?\n");
                  }
               }
            }
         }
         
         if (clickTimeListPtr_ > 0)
         {
            bool updateReady = false;
            while (!updateReady)
            {
               if (nBitsPerSample_ == 8)
               {
                  if (clickTimeList_[0] + lClickNoiseLength_ >= bytesWritten_ + handles_->buffers[bufferNr].dwBytesRecorded)
                     updateReady = true;
               }
               else
               {
                  if (clickTimeList_[0] + lClickNoiseLength_ * 2 >= bytesWritten_ + handles_->buffers[bufferNr].dwBytesRecorded)
                     updateReady = true;
               }
               
               if (!updateReady)
               {
//                  printf("removing sample...\n");
                  for (int j=1; j<16; ++j)
                     clickTimeList_[j-1] = clickTimeList_[j];
                  --clickTimeListPtr_;
                  
                  if (clickTimeListPtr_ == 0)
                     updateReady = true;
               }
            }
         }
         
         clickTimeListLock_ = false;
      }
      
      audioFile_->writeData(handles_->buffers[bufferNr].lpData, 
         handles_->buffers[bufferNr].dwBytesRecorded);
      
      bytesWritten_ += handles_->buffers[bufferNr].dwBytesRecorded;
   }
}

void AudioCapturer::calcAudioLevel(int bufferNr)
{
#ifdef _DEBUGFILE
   AudioDebugMsg("calcAudioLevel\n");
#endif
	// look at some samples
   int everyBytes = 64;
	int takeThese = handles_->buffers[bufferNr].dwBytesRecorded / everyBytes / nBlockAlign_;
   if (takeThese <= 0)
      return;

   int tmpAudioLevel = 0;
	int maxAudioLevel = 0;
   int silenceThreshold = 0;

	if (nBitsPerSample_ > 8)
	{
      double zeroThreshold    = 30.0; // dBFS
		signed short* value;
		short tmp;

      silenceThreshold = 5; // == 10 dBFS span

		// we have signed, 16 bit numbers
		for (int i=0; i<takeThese; ++i)
		{
			value = (signed short*) (handles_->buffers[bufferNr].lpData + i*everyBytes*nBlockAlign_);
			tmp = abs(*value);
			tmpAudioLevel += tmp;
			if (tmp > maxAudioLevel)
				maxAudioLevel = tmp;
		}

		meanAudioLevel_ = ((tmpAudioLevel * 100) / takeThese) / 32768;

      //maxAudioLevel = (maxAudioLevel / (32768/32)) * (32768/32);
      double dBFS = -96.0; // means 0 or 1
      if (maxAudioLevel > 0)
         dBFS = 20.0 * log10(((double) maxAudioLevel) / 32768.0) - zeroThreshold;
      if (dBFS < -96.0)
         dBFS = -96.0;
      dBFS += zeroThreshold;

      // normalize maxAudioLevel_ between 0 and 100.
		maxAudioLevel_  = (int) (((96.0 - zeroThreshold) + dBFS) * (100.0 / (96.0 - zeroThreshold)));//(maxAudioLevel * 100) / 32768;
	}
	else
	{
      double zeroThreshold = 2.0; // dBFS (8 Bit)
		unsigned char* value;
		int tmp;

      silenceThreshold = 3; // == 6 dBFS span

      for (int i=0; i<takeThese; ++i)
		{
			value = (unsigned char *) (handles_->buffers[bufferNr].lpData + i*everyBytes*nBlockAlign_);
			tmp = abs((int) (*value) - 127);
			tmpAudioLevel += tmp;
			if (tmp > maxAudioLevel)
				maxAudioLevel = tmp;
		}

      double dBFS = -42.0; // means 0 or 1
      if (maxAudioLevel > 0)
         dBFS = 20.0 * log10(((double) maxAudioLevel) / 128.0) - zeroThreshold;
      if (dBFS < -42.0)
         dBFS = -42.0;
      dBFS += zeroThreshold;

		meanAudioLevel_ = ((tmpAudioLevel * 100) / takeThese) / 128;

      // normalize maxAudioLevel_ between 0 and 100.
		maxAudioLevel_  = (int) (((42.0 - zeroThreshold) + dBFS) * (100.0 / (42.0 - zeroThreshold))); // (maxAudioLevel * 100) / 128;
   }

   // Here we check for audio silence. The algorithm works inductively
   // as follows: We assume that we have a mean dBFS for some values
   // to start with.
   //  - If the current maxAudioLevel_ deviates more than 5 (8 Bit: 3) dBFS
   //    from the current mean, a new mean calculation is started, as
   //    we assume tha no silence loudness deviation can be that large,
   //  - If the deviation is not large enough, the dBFS value is added to a
   //    sum counter which is used to calculate the new mean in the next
   //    step; a counter of the steps which have detected silence is of
   //    course also updated.
   //  - If the time span (deducted from the number of blocks containig
   //    silence due to our definition) in which silence have been detected
   //    is larger than silenceTimeout_, the state of the recording is
   //    set to AUDIO_SILENCE.
   if (abs(maxAudioLevel_ - (audioSilenceSum_ / audioSilenceCount_)) > silenceThreshold)
   {
      // Deviation from mean larger than threshold,
      // restart mean calculation.
      audioSilenceSum_ = maxAudioLevel_;
      audioSilenceCount_ = 1;

      // Audio state is AUDIO_NORMAL
      audioSilenceState_ = AUDIO_NORMAL;
   }
   else
   {
      // the deviation from the mean was not large enough to assume non-
      // silence. Update the sum and the counter for the peak mean.
      audioSilenceSum_ += maxAudioLevel_;
      audioSilenceCount_++;

      // has the silence been detected for long?
      if (audioBlockLengthMs_ * audioSilenceCount_ > SILENCE_TIMEOUT)
      {
         // yes, the audio state has to be set to AUDIO_SILENCE
         audioSilenceState_ = AUDIO_SILENCE;
      }
      else // no, we still assume AUDIO_NORMAL state.
         audioSilenceState_ = AUDIO_NORMAL;
   }

   // normalize the maxAudioLevel_ variable
   if (maxAudioLevel_ < 0) maxAudioLevel_ = 0;
   if (maxAudioLevel_ > 100) maxAudioLevel_ = 100;
}

int AudioCapturer::getState()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("getState\n");
#endif
   if (dryCaptureMode_ || isPaused_)
      return AUDIO_NORMAL;

   return audioSilenceState_;
}

int AudioCapturer::getMeanAudioLevel()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("getMeanAudioLevel\n");
#endif
	return meanAudioLevel_;
}

int AudioCapturer::getMaxAudioLevel()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("getMaxAudioLevel\n");
#endif
	return maxAudioLevel_;
}

bool AudioCapturer::hasDataBeenLost()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("hasDataBeenLost\n");
#endif
	return dataLost_;
}
	
bool AudioCapturer::stop()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("stop\n");
#endif
#ifdef DEBUG
	cout << "Audio::stop()" << endl;
#endif
	ResetEvent(handles_->finishEvent);
	
	// signalize we want to quit:
	handles_->inProgressFlag = false;
	
	MMRESULT mmres = waveInStop(handles_->waveIn);
	if (mmres != MMSYSERR_NOERROR)
	{
		cerr << "waveInStop() failed! Error == " << mmres << endl;
	}
	
	bool res = true;
	
	// wait for last frame to finish:
	DWORD wRes = WaitForSingleObject(handles_->finishEvent, 10000);
	
	switch (wRes)
	{
	case WAIT_FAILED:
		cerr << "WaitForSingleObject() failed! Error == " << GetLastError() << endl;
		res = false;
		break;
		
	case WAIT_TIMEOUT:
		cerr << "WaitForSingleObject() [finish of audio capture] was timed out!" << endl;
		res = false;
		break;
		
	case WAIT_ABANDONED:
		cerr << "WaitForSingleObject() [finish of audio capture] was abandoned!" << endl;
		res = false;
		break;
		
	case WAIT_OBJECT_0:
#ifdef DEBUG
		cout << "Audio capture has finished successfully." << endl;
#endif
		break;
	}

#ifdef DEBUG
	cout << "Written bytes: " << bytesWritten_ << 
		", waveInGetPosition == " << getWaveInTimeSamples() << endl;
#endif
	// anyhow, close the WAV file and unprepare all headers:
	close();

   // reset some variables:
	pauseByteCounter_ = 0;
   isPaused_         = true;

	return res;
}
	
long AudioCapturer::getWaveInTimeSamples()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("getWaveInTimeSamples\n");
#endif
	return getWaveInTimeBytes() / nBlockAlign_;
}

long AudioCapturer::getWaveInTimeBytes()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("getWaveInTimeBytes\n");
#endif
   if (!handles_->active)
      return bytesWritten_;

	handles_->mmTime.wType = TIME_BYTES;
	MMRESULT mmres = waveInGetPosition(handles_->waveIn, 
		&handles_->mmTime, sizeof(MMTIME));

	// if we're paused, return the last byte position, i.e.
	// the "written bytes" counter, otherwise the current
	// audio position
	if (isPaused_)
		return bytesWritten_;
	else
		return handles_->mmTime.u.cb - pauseByteCounter_;
}

long AudioCapturer::getWaveInTimeMs()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("getWaveInTimeMs\n");
#endif
	double bytes = getWaveInTimeBytes() * 1000.0;
	double divisor = nSampleRate_ * nBlockAlign_;
	return (long) (bytes / divisor);
	//return (long) ((1000L * getWaveInTimeBytes()) / ((long) (nSampleRate_ * nBlockAlign_)));
}

long AudioCapturer::insertClickNoise()
{
    // This method uses side effect: clickTimeList_ is used/modified in writeBuffer()

    if (clickTimeListPtr_ >= MAXIMUM_CLICK_NOISE)
        return -1;


#ifdef _DEBUGFILE
   AudioDebugMsg("insertClickNoise\n");
#endif
   if (isPaused_)
      return -1;

   lClickTimeBytes_ = getWaveInTimeBytes();
#ifdef _DEBUG
   printf("** Click sound @ %d Bytes.\n", lClickTimeBytes_);
#endif

   while (clickTimeListLock_)
   {
      printf("???");
      Sleep(1);
   }

   clickTimeList_[clickTimeListPtr_] = lClickTimeBytes_;
   ++clickTimeListPtr_;
   return (1000.0*lClickTimeBytes_) / (nSampleRate_ * nBlockAlign_);
}

long AudioCapturer::mixAudioStreams8(unsigned char *pStream1, long lLength1, long lOffset1,
                                     unsigned char *pStream2, long lLength2, long lOffset2)
{
#ifdef _DEBUGFILE
   AudioDebugMsg("mixAudioStreams8\n");
#endif
   // how much do we want to mix?
   // this much, if we have space:
   long lMixLen = lLength2 - lOffset2;
   if (lMixLen > lLength1 - lOffset1)
      lMixLen = lLength1 - lOffset1;

   unsigned long ulMixed;
   long j = lOffset2;
   for (long i=lOffset1; i<lMixLen + lOffset1; ++i)
   {
      ulMixed = (unsigned long) pStream1[i] + (unsigned long) pStream2[j];
      if (ulMixed > 255)
         ulMixed = 255;
      pStream1[i] = (unsigned char) ulMixed;
      ++j;
   }

   return lMixLen;
}

long AudioCapturer::mixAudioStreams16(short *pStream1, long lLength1, long lOffset1,
                                      short *pStream2, long lLength2, long lOffset2)
{
#ifdef _DEBUGFILE
   AudioDebugMsg("mixAudioStreams16\n");
#endif
#ifdef _DEBUG
   //printf("mixAudioStreams16(..., %d, %d, ..., %d, %d)", lLength1, lOffset1, lLength2, lOffset2);
#endif
   long lMixLen = lLength2 - lOffset2;
   if (lMixLen > lLength1 - lOffset1)
      lMixLen = lLength1 - lOffset1;
#ifdef _DEBUG
   //printf(", lMixLen == %d\n", lMixLen);
#endif

   long lMixed;
   long j = lOffset2;
   for (long i=lOffset1; i<lMixLen + lOffset1; ++i)
   {
      lMixed = (long) pStream1[i] + (long) pStream2[j];
      if (lMixed < -32768)
         lMixed = -32768;
      if (lMixed > 32767)
         lMixed = 32767;
      pStream1[i] = (short) lMixed;
      ++j;
   }

   return lMixLen;
}

/*
 *
 *  S T A T I C  M E T H O D S
 *
 */

int AudioCapturer::getDeviceCount()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("getDeviceCount\n");
#endif
#ifdef DEBUG
	cout << "Audio::getDeviceCount()" << endl;
#endif
	return waveInGetNumDevs();
	// return waveCapsNum_;
}

long AudioCapturer::getSystemTime()
{
#ifdef _DEBUGFILE
   AudioDebugMsg("getSystemTime\n");
#endif
	return timeGetTime();
}

void AudioCapturer::getDeviceList(WAVEINCAPS** deviceList)
{
#ifdef _DEBUGFILE
   AudioDebugMsg("getDeviceList\n");
#endif
	int devCount = getDeviceCount();

	for (int i=0; i<devCount; ++i)
	{
		MMRESULT res = waveInGetDevCaps(i, deviceList[i], sizeof(WAVEINCAPS));

		if (res != MMSYSERR_NOERROR)
			throw AudioException(Local::localizeString(AUDIO_GETCAPS_ERR));

	}
	return;
}
