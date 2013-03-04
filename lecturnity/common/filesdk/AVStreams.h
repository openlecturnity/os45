#ifndef __AVSTREAMS__
#define __AVSTREAMS__

#include "export.h"
#include "mediastream.h"
// These two imports are not necessary for the File SDK
// library (they are alredy in the StdAfx.h), but for
// the editor. ==> StdAfx.h of Editor
//#include <qedit.h>
//#include <streams.h>

#define WM_AUDIO_PROGRESS (WM_USER + 1)

class FILESDK_EXPORT CAudioStream : public CMediaStream, public ISampleGrabberCB
{
public:
   CAudioStream();
   ~CAudioStream();
   CAudioStream *Copy();

   ULONG __stdcall AddRef() { return 1; }
   ULONG __stdcall Release() { return 1; }
   STDMETHODIMP QueryInterface(REFIID, void **) { return E_NOINTERFACE; }

public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();
   
public:
   int GetLength() {return m_lengthMs;}

public:
   // get and set functions for member variables
   LPCTSTR GetFilename() { return m_csFilename; }
   bool SetFilename(LPCTSTR filename, int nStream=0, int versionType=-1, HWND hWndProgress=NULL);
   // If SetFilename returned false, call this method
   // in order to find out if SetFilename() failed due
   // to a version error (campus, enterprise, eval mismatch)
   bool IsVersionError() { return m_isVersionError; }
   // Returns the type of the audio file, if HasLadInfo()
   // returns true.
   int GetVersionType() { return m_nVersionType; }
   // Returns true if the audio is an LAD file and
   // has LAD information.
   bool HasLadInfo() { return m_hasLadInfo; }

   // Returns true if the audio stream is a dummy stream
   bool IsDummyStream();

   void SetSampleRate(int iSampleRate);
   int GetSampleRate() {return m_wfe.nSamplesPerSec;}
   void SetBitsPerSample(int iBitsPerSample);
   int GetBitsPerSample() {return m_wfe.wBitsPerSample;}

   signed char *GetSampleBuffer() {return m_samples;}
   int GetSampleBufferLength() {return m_cbSamples;}

   int GetOffset() {return m_iOffset;}
   void SetOffset(int offset) {m_iOffset = offset;}

   int GetID() {return m_iID;}
   void SetID(int id) {m_iID = id;}

   int GetStreamSelector() { return m_nStreamSelector; }
   //void SetStreamSelector(int nStream);

   bool HasOriginalAudioSource() { return (FALSE == m_csOriginalAudioSource.IsEmpty()); }
   void SetOriginalAudioSource(const _TCHAR *tszSrc) { m_csOriginalAudioSource = tszSrc; }
   LPCTSTR GetOriginalAudioSource() { return m_csOriginalAudioSource; }
   void SetOriginalAudioSourceStream(int nStream) { m_nOriginalAudioSourceStream = nStream; }
   int GetOriginalAudioSourceStream() { return m_nOriginalAudioSourceStream; }

   // ISampleGrabberCB
   STDMETHODIMP BufferCB(double sampleTime, BYTE *pBuffer, long bufferLen);
   STDMETHODIMP SampleCB(double sampleTime, IMediaSample *pSample);
private:
   CString      m_csFilename;
//   int          m_iSampleRate;
//   int          m_iBitRate;
   WAVEFORMATEX m_wfe;

   int          m_lengthMs;
   signed char *m_samples;
   int          m_cbSamples;
   int          m_iReadBytes;
   int          m_iOffset;
   char         m_szLadInfo[128];
   bool         m_hasLadInfo;
   int          m_iID;
   // This variable decides which stream in the media
   // file to take (in case of audio data in a AVI file)
   int          m_nStreamSelector;
   int          m_nVersionType;
   bool         m_isVersionError;
   // This member is used for progress notification on the
   // import audio progress.
   HWND         m_hWndProgress;

   void    UpdateWaveFormat();
   HRESULT ReadAudioParameters(int versionType);

   // Thsi member is not empty if the audio was created from an
   // other audio stream (which was not uncompressed)
   CString      m_csOriginalAudioSource;
   int          m_nOriginalAudioSourceStream;
};

class FILESDK_EXPORT CVideoStream : public CMediaStream
{
public:
   CVideoStream();
   ~CVideoStream();

public:
   CVideoStream *Copy();
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:          
   // get and set functions for member variables

   LPCTSTR GetFilename() { return m_csFilename; }
   void SetFilename(LPCTSTR filename, int nStream=0);

   float GetFrameRate() {return m_iFrameRate;}
   void SetFrameRate(float frameRate) {m_iFrameRate = frameRate;}

   LPCTSTR GetCodec() { return (LPCTSTR)m_csCodec; }
   void SetCodec(LPCTSTR codec) { m_csCodec = codec; }

   int GetQuality() {return m_iQuality;}
   void SetQuality(int quality) {m_iQuality = quality;}

   int GetKeyframes() {return m_iKeyframes;}
   void SetKeyframes(int keyframes) {m_iKeyframes = keyframes;}
   
   int GetOffset() {return m_iOffset;}
   void SetOffset(int offset) {m_iOffset = offset;}
   
   int GetLength() {return m_nLength;}
   void SetLength(int length) {m_nLength = length;}
   
   int GetID() {return m_iID;}
   void SetID(int id) {m_iID = id;}

   int GetStreamSelector() { return m_nStreamSelector; }
   //void SetStreamSelector(int nStream);

private:
   CString m_csFilename;
   float   m_iFrameRate;
   CString m_csCodec;
   int     m_iQuality;
   int     m_iKeyframes;
   int     m_iOffset;
   int     m_nLength;
   int     m_iID;
   // This variable decides which stream in the media
   // file to take (i.e. the video stream of an AVI file)
   int     m_nStreamSelector;
};

class FILESDK_EXPORT CClipInfo
{
public: 
   CClipInfo();
   ~CClipInfo();

public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();
   

public:
   // get and set functions for member variables
   LPCTSTR GetFilename() { return m_csFilename; }
   void SetFilename(LPCTSTR filename, int nStream=0);

   LPCTSTR GetAudioFilename() { return m_csAudioSource; }
   void SetAudioFilename(LPCTSTR tszFilename) { m_csAudioSource = tszFilename; }

   int GetTimestamp() {return m_iTimestamp;}
   void SetTimestamp(int timestamp) {m_iTimestamp = timestamp;}

   int GetLength() {return m_iLength;}
   void SetLength(int length) {m_iLength = length;}

   int GetEndTime() {return m_iTimestamp + m_iLength;}

   LPCTSTR GetTitle() { return m_csTitle;}
   void SetTitle(LPCTSTR title) { m_csTitle = title; }

   void GetKeywords(CStringList &stringList);
   void SetKeywords(CStringList &stringList);
   void GetKeywords(CString &csKeywords);
   void SetKeywords(CString &csKeywords);

   int GetStreamSelector() { return m_nStreamSelector; }
   void SetStreamSelector(int nStream) { m_nStreamSelector = nStream; }

   bool HasAudioStream() { return GetAudioStreamSelector() >= 0; }
   int GetAudioStreamSelector() { return m_nAudioStreamSelector; }
   void SetAudioStreamSelector(int nStream) { m_nAudioStreamSelector = nStream; }

   void SetTempOriginalAudioVolume(int nVol) { m_nTempOriginalVolume = nVol; }
   int GetTempOriginalAudioVolume() { return m_nTempOriginalVolume; }

   bool HasOriginalAudioSource() { return (FALSE == m_csOriginalAudioSource.IsEmpty()); }
   void SetOriginalAudioSource(const _TCHAR *tszSrc) { m_csOriginalAudioSource = tszSrc; }
   LPCTSTR GetOriginalAudioSource() { return m_csOriginalAudioSource; }
   void SetOriginalAudioSourceStream(int nStream) { m_nOriginalAudioSourceStream = nStream; }
   int GetOriginalAudioSourceStream() { return m_nOriginalAudioSourceStream; }

   // PZI: information concerning structured clips
   bool GetStructureAvailable() { return m_bStructureAvailable; }
   void SetStructureAvailable(bool available) { m_bStructureAvailable = available; }
   bool GetImagesAvailable () { return m_bImagesAvailable; }
   void SetImagesAvailable(bool available) { m_bImagesAvailable = available; }
   bool GetFulltextAvailable () { return m_bFulltextAvailable; }
   void SetFulltextAvailable(bool available) { m_bFulltextAvailable = available; }



public:
   CClipInfo *Copy();

private:
   CString m_csFilename;
   CString m_csAudioSource;

   int     m_iTimestamp;
   int     m_iLength;
   CString m_csTitle;
   CStringList m_slKeywords;

   // This variable decides which stream in the media
   // file to take (i.e. the video stream of an AVI file)
   int     m_nStreamSelector;
   int     m_nAudioStreamSelector;

   // This variable is for temporary use; the original volume
   // for the underlying sound is stored in the CAviSettings
   // structure for each source clip.
   int     m_nTempOriginalVolume;

   // The audio for this clip was created from this file
   CString m_csOriginalAudioSource;
   // ... and this stream in that file.
   int     m_nOriginalAudioSourceStream;

   // PZI: information concerning structured clips
   bool m_bStructureAvailable;
   bool m_bImagesAvailable;
   bool m_bFulltextAvailable;
};

class FILESDK_EXPORT CTimeSpread
{
public:
   CTimeSpread();
   ~CTimeSpread() {}

   int m_iBegin;
   int m_iEnd;
};

class FILESDK_EXPORT CExportClip
{
public:
   CExportClip() {}
   ~CExportClip();

public:

   int GetLength();

   CClipInfo *m_clipInfo;
   CString m_csClipPrefix;
   int m_targetBegin;
   CList<CTimeSpread *, CTimeSpread *> m_cutList;

};


class FILESDK_EXPORT CClipStream : public CMediaStream
{
public:
   CClipStream();
   ~CClipStream();
   
public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:
   CClipStream *Copy();
   void AddClip(CClipInfo *clipInfo);
   CClipInfo *FindClip(int clipNumber);
   // PZI: extended to return clip number (starting at 1; or 0 if not found)
   CClipInfo *FindClipBeginsAt(int timestamp, int *returnClipNumber=NULL);
   // PZI: extended to return clip number (starting at 1; or 0 if not found)
   CClipInfo *FindClipAt(int timestamp, int *returnClipNumber=NULL);
   CClipInfo *FindClip(CString &clipFilename, int clipStart, int clipLength);
   bool HasClips();
   int GetSize();

   // write all clips which begins or end between sourceBegin and sourceEnd to lrd file
   void WriteToLrd(HANDLE lrdFile, LPCTSTR prefix, int targetBegin, int targetEnd, int sourceBegin, int sourceEnd);
   // append all clips which begins or end between sourceBegin and sourceEnd
   void Fill(CList<CExportClip *, CExportClip *> &exportClipList, int targetBegin, int targetEnd, int sourceBegin, int sourceEnd);
   // write all clips which begins or end between sourceBegin and sourceEnd to lep file
   void WriteToLep(HANDLE lepFile, int targetBegin, int targetEnd, int sourceBegin, int sourceEnd, LPCTSTR szLepFileName);

   // Find all clips started between fromMsec and toMsec
   void GetClips(CArray<CClipInfo *, CClipInfo *> &clips, int fromMsec, int toMsec, int offset, int &firstPos, int &lastPos);
   // Find the clipNumber clip in interval from fromMsec to toMsec
   CClipInfo *FindClip(int clipNumber, int fromMsec, int toMsec);

   int GetID() {return m_iID;}
   void SetID(int id) {m_iID = id;}
   
public:
   CArray<CClipInfo *, CClipInfo *> m_arrClip;
   int m_iID;
};

#endif // __AVMEDIASTREAMS__

