#ifndef __RIFFFILE
#define __RIFFFILE

//#include "global.h"
/*
#include <iostream>
#include <windows.h>
#include <vfw.h>
#include <wingdi.h>
#include <vector>
#include <string>
#include "exceptions.h"
#include "localizer.h"
#include "resource.h"
*/
#pragma warning(disable:4786)

using namespace std;

class RiffFile
{
private:
   MMCKINFO* chunkArray_;
   int       chunkPos_;
	bool fileOpen_;
	char buf_[4];

protected:
	HMMIO hmmio_;
	_TCHAR szFileName_[512];

public:
	RiffFile(_TCHAR* tszFileName);
	~RiffFile();

	void close();

	void openListChunk(_TCHAR *tszFccType);
	void openRiffChunk(_TCHAR *tszFccType);
	void openChunk(_TCHAR *tszCkid);
	void openChunk(DWORD ckid);
	bool closeChunk();
	void writeJunkChunk(long padLength);

	bool writeData(const void* data, int dataSize);
	void writeDWORD(DWORD n);
	//string FOURCCtoString(FOURCC fcc);
};

#endif