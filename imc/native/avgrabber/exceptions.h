#ifndef __EXCEPTIONS
#define __EXCEPTIONS

//#include "global.h"

using namespace std;

class AVGrabberException : public exception
{
public:
	AVGrabberException(const _TCHAR* tszMessage)
   {
      exception("AvGrabber Exception");
   }
};

class AudioException : public AVGrabberException
{
public:
	AudioException(const _TCHAR* msg) : AVGrabberException(msg) {}
};

class AudioIOException : public AudioException
{
public:
	AudioIOException(const _TCHAR* msg) : AudioException(msg) {}
};

class MemoryAudioException : public AudioException
{
public:
	MemoryAudioException(const _TCHAR* msg) : AudioException(msg) {}
};

class VideoException : public AVGrabberException
{
public:
	VideoException(const _TCHAR* msg) : AVGrabberException(msg) {}
};

class MemoryVideoException : public VideoException
{
public:
	MemoryVideoException(const _TCHAR* msg) : VideoException(msg) {}
};

class VideoIOException : public VideoException
{
public:
	VideoIOException(const _TCHAR* msg) : VideoException(msg) {}
};

class WdmException : public VideoException
{
public:
   WdmException(const _TCHAR* msg) : VideoException(msg) {}
};

#endif