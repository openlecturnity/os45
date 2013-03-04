#include "global.h"
#include "resource.h"
//#include "RiffFile.h"

using namespace std;

static char* szPadJunk = "Recorded using Lecturnity (R). Lecturnity is a registered trade mark of imc AG. http://www.im-c.de";

RiffFile::RiffFile(_TCHAR* tszFileName)
{
#ifdef DEBUG
	cout << "RiffFile(" << tszFileName << ")" << endl;
#endif

	fileOpen_ = false;

	chunkArray_ = (MMCKINFO*) malloc(16 * sizeof(MMCKINFO));
   chunkPos_   = 0;

	_tcscpy(szFileName_, tszFileName);
	hmmio_ = mmioOpen(szFileName_, NULL, MMIO_CREATE | MMIO_WRITE);
	
   if (hmmio_ == NULL)
   {
      _TCHAR tmp[256];
      _stprintf(tmp, Local::localizeString(RIFF_ERROR_OUTPUT), szFileName_);
      throw AVGrabberException(tmp);
	}

	fileOpen_ = true;
}	

RiffFile::~RiffFile()
{
	if (fileOpen_)
		close();

	while (closeChunk()) {} // close all pending chunks

	// delete the rest if there are still pending chunks
	// vector<MMCKINFO*>::iterator iter;
	//for (iter = chunkStack_->begin(); iter != chunkStack_->end(); ++iter)
	//	free(*iter);
   //delete chunkStack_;

   free(chunkArray_);
   chunkArray_ = NULL;
   chunkPos_ = 0;
}

void RiffFile::close()
{
	MMRESULT res = mmioClose(hmmio_, 0);
	fileOpen_ = false;
	hmmio_ = NULL;
	if (res != MMSYSERR_NOERROR)
	{
		switch (res)
		{
		case MMIOERR_CANNOTWRITE:
			throw AVGrabberException(Local::localizeString(RIFF_NOBUFWRITE));
		default:
			_TCHAR tmp[256];
			_stprintf(tmp, Local::localizeString(RIFF_CLOSEERR), szFileName_);
			throw AVGrabberException(tmp);
		}

	}
}

void RiffFile::openRiffChunk(_TCHAR *tszFccType)
{
	MMCKINFO* riffChunk = &chunkArray_[chunkPos_++];

   riffChunk->cksize   = 0;
	riffChunk->fccType  = mmioStringToFOURCC(tszFccType, 0);

	MMRESULT res = mmioCreateChunk(hmmio_, riffChunk, MMIO_CREATERIFF);
	
	if (res != MMSYSERR_NOERROR)
	{
		_TCHAR tmp[256];
		_stprintf(tmp, Local::localizeString(RIFF_OPENCHUNKERR), szFileName_);
		throw AVGrabberException(tmp);
	}

	//chunkStack_->push_back(riffChunk);
}

void RiffFile::openListChunk(_TCHAR *tszFccType)
{
	MMCKINFO* listChunk = (MMCKINFO*) &chunkArray_[chunkPos_++];
    listChunk->cksize   = 0;
	listChunk->fccType  = mmioStringToFOURCC(tszFccType, 0);

	MMRESULT res = mmioCreateChunk(hmmio_, listChunk, MMIO_CREATELIST);
	
	if (res != MMSYSERR_NOERROR)
	{
		_TCHAR tmp[256];
		_stprintf(tmp, Local::localizeString(RIFF_OPENCHUNKERR), szFileName_);
		throw AVGrabberException(tmp);
	}

	//chunkStack_->push_back(listChunk);
}

void RiffFile::openChunk(_TCHAR *tszCkid)
{
	openChunk(mmioStringToFOURCC(tszCkid, 0));
}

void RiffFile::openChunk(DWORD ckid)
{
	MMCKINFO* chunk = &chunkArray_[chunkPos_++];
	chunk->cksize = 0;
	chunk->ckid   = ckid;

	MMRESULT res = mmioCreateChunk(hmmio_, chunk, 0);

	if (res != MMSYSERR_NOERROR)
	{
		_TCHAR tmp[256];
		_stprintf(tmp, Local::localizeString(RIFF_OPENCHUNKERR), szFileName_);
		throw AVGrabberException(tmp);
	}

	//chunkStack_->push_back(chunk);
}

bool RiffFile::closeChunk()
{
	if (chunkPos_ <= 0)
      return false;

	MMCKINFO* chunk = &chunkArray_[--chunkPos_];

	MMRESULT res = mmioAscend(hmmio_, chunk, 0);

	// free(chunk);

	if (res != MMSYSERR_NOERROR)
	{
		_TCHAR tmp[256];
		_stprintf(tmp, Local::localizeString(RIFF_CLOSECHUNKERR), szFileName_);
		throw AVGrabberException(tmp);
	}

	return true;
}

bool RiffFile::writeData(const void* data, int dataSize)
{
	LONG written = mmioWrite(hmmio_, (char*) data, dataSize);
	return (written == dataSize);
}

void RiffFile::writeDWORD(DWORD n)
{
	buf_[0] = (char) (n & 0xff);
	buf_[1] = (char) ((n >> 8) & 0xff);
	buf_[2] = (char) ((n >> 16) & 0xff);
	buf_[3] = (char) ((n >> 24) & 0xff);
	if (!writeData(buf_, 4))
	{
		_TCHAR tmp[256];
		_stprintf(tmp, Local::localizeString(RIFF_WRITEERR), szFileName_);
		throw AVGrabberException(tmp);
	}
}

void RiffFile::writeJunkChunk(long padLength)
{
	const int BUF_SIZE = 2048;

	openChunk(_T("JUNK"));

	char tempBuf[BUF_SIZE];
	memset(tempBuf, 0, BUF_SIZE);
   memcpy(tempBuf, szPadJunk, strlen(szPadJunk));

	int loops = padLength / BUF_SIZE;
	int remains = padLength % BUF_SIZE;
#ifdef DEBUG
	cout << "Padding 2048 bytes " << loops << " times, remains " << remains << " bytes." << endl;
#endif

	for (int i=0; i<loops; ++i)
		writeData(tempBuf, BUF_SIZE);
	if (remains > 0)
		writeData(tempBuf, remains);

	closeChunk();
}

/*
string RiffFile::FOURCCtoString(FOURCC fcc)
{
	string sfcc;
	sfcc += (char) (fcc & 0x000000ff);
	sfcc += (char) (fcc >> 8 & 0x000000ff);
	sfcc += (char) (fcc >> 16 & 0x000000ff);
	sfcc += (char) (fcc >> 24 & 0x000000ff);
	return sfcc;
}
*/