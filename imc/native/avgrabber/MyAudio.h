#ifndef __AUDIODEFS
#define __AUDIODEFS

//#include "global.h"
/*
#include <iostream>
#include <string>
#include <windows.h>
#include <mmsystem.h>
#include <Sys/timeb.h>
#include <math.h>
#include <process.h>
#include "localizer.h"
#include "exceptions.h"
#include "resource.h"
#include "global.h"
*/

static const int WAVE_BUFFER_SIZE     = 8192;
static const int WAVE_NUM_BUFFER      = 32;
static const int INITIAL_WRITE_BUFFER = 8192;

static const int MEASURE_TIME     = 30000; // in ms
static int       SILENCE_TIMEOUT  = 30000;

#define AUDIO_NORMAL       0
#define AUDIO_SILENCE      1

#define SIZE_LADKEY        892
#define SIZE_LADINFO       128

#define MAXIMUM_CLICK_NOISE 16

typedef struct
{
	FILE*    wavFileHandle;
	MMCKINFO riffChunk;
	bool riffChunkOpen;
	MMCKINFO dataChunk;
	bool dataChunkOpen;
} WAVFILE;

typedef struct
{
	HWAVEIN waveIn;
	bool driverOpen;
	bool active;
	bool inProgressFlag;
	HANDLE audioEvent;
	HANDLE finishEvent;
	WAVEHDR* buffers;
	MMTIME mmTime;
} WAVHandles;

class AudioFile
{
private:
	WAVFILE *wavFile_;

	char myBuf_[4];
   bool useLadFormat_;
   char szLadInfo_[SIZE_LADINFO];
   char szLadKey_[SIZE_LADKEY];
   int  keyOffset_;
   int  fileOffset_;
   int  lastAscendOffset_;
   MMCKINFO* currentMmckInfo_;

   char* xorBuffer_;
   int   xorBufferSize_;

	// a couple of forward declarations:
	bool writeBytes(const char*, long);
	bool writeLE16Int(const unsigned __int16);
	bool writeLE32Int(const unsigned __int32);
   char getRandomByte();

   FILE* myOpen(TCHAR* tszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen);
   int myCreateChunk(FILE* hmmio, LPMMCKINFO pmmcki, UINT fuCreate);
   int myAscend(FILE* hmmio, LPMMCKINFO pmmcki, UINT fuAscend);
   long myWrite(FILE* hmmio, const char* pch, LONG cch);
   int myClose(FILE* hmmio, UINT fuClose);

public:
	AudioFile(_TCHAR* tszFileName, 
			  int nSamplesPerSec, 
			  int nBitsPerSample, 
			  int nChannels,
           char* szLadInfo = NULL);
	~AudioFile();

	bool writeData(char* lpData, long size);
	bool close();
};

class AVGRABBER_EXPORT AudioCapturer
{
private:
	_TCHAR szFileName_[512];

	WAVHandles* handles_;

	AudioFile* audioFile_;

	int nChannels_;
	int nSampleRate_;
	int nBlockAlign_;
	int nBitsPerSample_;
	int nDeviceID_;
	int meanAudioLevel_;
	int maxAudioLevel_;

	long firstSysTimeMs_;
	long bytesWritten_;
	long currentFrameMs_;
	long pauseByteCounter_;

   /* This is the time (in bytes) of the last mouse click to insert
      into the audio stream. */
   long lClickTimeBytes_;

   /* The (WAV) data of the click noise. May be either 8 bit or
      16 bit PCM data (unsigned char or short). */
   char *pClickNoiseData_;
   /* The length of the click noise sample in number of samples (not bytes!). */
   long lClickNoiseLength_;

   /* Contains the sum of the max audio peaks (in dBFS) of the
    * audioSilenceCount_ last audio sample blocks */
   int audioSilenceSum_;
   /* See audioSilenceSum_ */
   int audioSilenceCount_;
   /* The current state of the audio recording. May be AUDIO_NORMAL
    * if everything is normal, or AUDIO_SILENCE if silence has been
    * detected for more than 30 seconds. */
   int audioSilenceState_;

   /* The length of an audio block, in ms */
   double audioBlockLengthMs_;

   char szLadInfo_[128];
   bool useLadFormat_;

	bool dataLost_;

	bool isPaused_;
   bool dryCaptureMode_;

	void init();
	bool deInit();

	bool initDeviceList();
	bool prepareHeaders();
	void createWavFile();
	bool unPrepareHeaders();
	void calcAudioLevel(int bufNr);
	void writeBuffer(int bufNr);

   long clickTimeList_[MAXIMUM_CLICK_NOISE];
   int  clickTimeListPtr_;
   bool clickTimeListLock_;
   void initClickNoise(WAVEFORMATEX *pWfeSrc, char  *pSrcData, long  lSrcLength,
                       WAVEFORMATEX *pWfeTrg, char **pTrgData, long *lTrgLength);
   void loadWavData(_TCHAR *tszFileName, char **pWavData, WAVEFORMATEX **ppWfe, long *plWavLength);
   long mixAudioStreams8(unsigned char *pStream1, long lLength1, long lOffset1,
                         unsigned char *pStream2, long lLength2, long lOffset2);
   long mixAudioStreams16(short *pStream1, long lLength1, long lOffset1,
                          short *pStream2, long lLength2, long lOffset2);

	static void __cdecl captureThreadProc(LPVOID parameter);
public:
	AudioCapturer(UINT nDeviceID, int sampleRate, int nBitsPerSample, 
				  int nChannels);
	~AudioCapturer();

	bool close();
   bool loadClickNoise(_TCHAR *tszWavFileName);

   void useLadFormat(char* szLadInfo);
	bool start(_TCHAR* tszFileName, bool bStartPaused = false);
	bool stop();
	void pause();
	void unPause();

	bool hasDataBeenLost();
	long getWaveInTimeBytes();
	long getWaveInTimeSamples();
	long getWaveInTimeMs();

	int getMeanAudioLevel();
	int getMaxAudioLevel();

   int getState();

   // returns the point of time the click is inserted at
   long insertClickNoise();

	static long getSystemTime();
	static int getDeviceCount();
	static void getDeviceList(WAVEINCAPS** deviceList);

   /* Sets the silence timeout (see AvGrabber::setAudioSilenceTimeout()) */
   static void setSilenceTimeout(int millis) { SILENCE_TIMEOUT = millis; }
   /* Returns the silence timeout */
   static int  getSilenceTimeout()           { return SILENCE_TIMEOUT;   }
};

#endif
