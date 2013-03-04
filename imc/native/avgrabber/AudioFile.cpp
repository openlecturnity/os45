#define __AUDIOFILE_CPP__

#include "global.h"
#include "resource.h"

using namespace std;

AudioFile::AudioFile(_TCHAR* tszFileName, 
					 int nSamplesPerSec, 
					 int nBitsPerSample, 
					 int nChannels,
                char* szLadInfo)
{
	wavFile_ = (WAVFILE*) malloc(sizeof(WAVFILE));
	if (wavFile_ == NULL)
	{
		throw MemoryAudioException(_T("AudioFile() could not malloc() memory!"));
	}

   xorBuffer_ = NULL;

   useLadFormat_ = (szLadInfo != NULL);
   if (useLadFormat_)
   {
      strcpy(szLadInfo_, szLadInfo);
      xorBufferSize_ = INITIAL_WRITE_BUFFER;
      xorBuffer_ = (char*) malloc(xorBufferSize_);
      if (xorBuffer_ == NULL)
      {
         free(wavFile_);
         throw MemoryAudioException(_T("Not enough memory for audio write buffer!"));
      }
   }
   else
   {
      strcpy(szLadInfo_, "u;1.0.0;00:00:00;01.01.01");
   }

	int mmres;

	wavFile_->wavFileHandle = myOpen(tszFileName, NULL, MMIO_CREATE | MMIO_READWRITE);

	if (wavFile_->wavFileHandle == NULL)
	{
		cerr << "mmioOpen() failed!" << endl;
		// free the WAVFILE struct:
		free(wavFile_);
		wavFile_ = NULL;

      _TCHAR tmp[512];
      _stprintf(tmp, Local::localizeString(AUDIO_OUTPUT_ERROR), tszFileName);
      throw AudioIOException(tmp);
	}

	wavFile_->dataChunkOpen = false;
	wavFile_->riffChunkOpen = false;

	// open the RIFF chunk...
	wavFile_->riffChunk.fccType = mmioStringToFOURCC(_T("WAVE"), 0);
	wavFile_->riffChunk.cksize  = 0;
	mmres = myCreateChunk(wavFile_->wavFileHandle, &wavFile_->riffChunk, MMIO_CREATERIFF);

   // cout << "myCreateChunk() has returned" << endl;

	if (mmres != MMSYSERR_NOERROR)
	{
		cerr << "mmioCreateChunk(RIFF) failed! Error == " << mmres << endl;
		close();
		// return false;
		throw AudioIOException(Local::localizeString(AUDIO_IOERR));
	}

   wavFile_->riffChunkOpen = true;

	// create the format chunk...
	MMCKINFO fmtChunk;
	fmtChunk.ckid   = mmioStringToFOURCC(_T("fmt "), 0);
	fmtChunk.cksize = 0;
	mmres = myCreateChunk(wavFile_->wavFileHandle, &fmtChunk, 0);
	if (mmres != MMSYSERR_NOERROR)
	{
		cerr << "mmioCreateChunk(fmt ) failed! Error == " << mmres << endl;
		close();
		throw AudioIOException(Local::localizeString(AUDIO_IOERR));
		// return false;
	}

	// write some data on the stream:
	int nBlockAlign = nBitsPerSample * nChannels / 8;

#ifdef DEBUG
	cout << "nChannels == " << nChannels << endl;
	cout << "nSamplesPerSec == " << nSamplesPerSec << endl;
	cout << "nAvgBytesPerSec == " << (nSamplesPerSec * nBlockAlign) << endl;
	cout << "nBlockAlign == " << nBlockAlign << endl;
	cout << "nBitsPerSample == " << nBitsPerSample << endl;
#endif DEBUG

	bool res = true;
	res = res && writeLE16Int(1);
	res = res && writeLE16Int(nChannels);
	res = res && writeLE32Int(nSamplesPerSec);
	res = res && writeLE32Int(nSamplesPerSec * nBlockAlign);
	res = res && writeLE16Int(nBlockAlign);
	res = res && writeLE16Int(nBitsPerSample);

	if (!res)
	{
		cerr << "Error writing 'fmt ' chunk!" << endl;
		close();
		throw AudioIOException(Local::localizeString(AUDIO_IOERR));
	}

	mmres = myAscend(wavFile_->wavFileHandle,
					   &fmtChunk,
					   0);
	if (mmres != MMSYSERR_NOERROR)
	{
		cerr << "mmioAscend('fmt ') failed!" << endl;
		close();
		throw AudioIOException(Local::localizeString(AUDIO_IOERR));
	}

	wavFile_->dataChunk.ckid   = mmioStringToFOURCC(_T("data"), 0);
	wavFile_->dataChunk.cksize = 0;
	mmres = myCreateChunk(wavFile_->wavFileHandle,
							&wavFile_->dataChunk,
							0);
	if (mmres != MMSYSERR_NOERROR)
	{
		cerr << "mmioCreateChunk('data') failed!" << endl;
		close();
		throw AudioIOException(Local::localizeString(AUDIO_IOERR));
	}

	wavFile_->dataChunkOpen = true;
}

AudioFile::~AudioFile()
{
	close();
}

bool AudioFile::writeData(char* lpData, long size)
{
	return writeBytes(lpData, size);
}

bool AudioFile::close()
{
	if (wavFile_ != NULL)
	{
		MMRESULT mmres;
		if (wavFile_->dataChunkOpen)
		{
			mmres = myAscend(wavFile_->wavFileHandle,
							   &wavFile_->dataChunk,
							   0);
			wavFile_->dataChunkOpen = false;
			if (mmres != MMSYSERR_NOERROR)
			{
				cerr << "mmioAscend('data') failed!" << endl;
			}
		}

		if (wavFile_->riffChunkOpen)
		{
			mmres = myAscend(wavFile_->wavFileHandle,
							   &wavFile_->riffChunk,
							   0);
			wavFile_->riffChunkOpen = false;
			if (mmres != MMSYSERR_NOERROR)
			{
				cerr << "mmioAscend('RIFF') failed!" << endl;
			}
		}

		mmres = myClose(wavFile_->wavFileHandle, 0);
		if (mmres != MMSYSERR_NOERROR)
		{
			cerr << "mmioClose() failed! Error == " << mmres << endl;
		}

		free(wavFile_);
		wavFile_ = NULL;

      if (useLadFormat_ && xorBuffer_ != NULL)
      {
         free(xorBuffer_);
         xorBuffer_ = NULL;
      }
	}

	return true;
}

bool AudioFile::writeBytes(const char* buf, long num)
{
	static long written;

	if (wavFile_->wavFileHandle == NULL)
		return false;

	written = myWrite(wavFile_->wavFileHandle, buf, num);
	if (written != num)
	{
		cerr << "*** writeBytes(): num != written!!" << endl;
		return false;
	}

	return true;
}

bool AudioFile::writeLE16Int(const unsigned __int16 n)
{
	myBuf_[0] = (char) (n & 0xff);
	myBuf_[1] = (char) ((n >> 8) & 0xff);

	return writeBytes(myBuf_, 2);
}

bool AudioFile::writeLE32Int(const unsigned __int32 n)
{
	myBuf_[0] = (char) (n & 0xff);
	myBuf_[1] = (char) ((n >> 8) & 0xff);
	myBuf_[2] = (char) ((n >> 16) & 0xff);
	myBuf_[3] = (char) ((n >> 24) & 0xff);

	return writeBytes(myBuf_, 4);
}

char AudioFile::getRandomByte()
{
   static char
      urk = 0;
   static bool
      first = true;
   signed char
      ret;

   if (first) {
      srand( (unsigned)timeGetTime() );
      first = false;
   }

   ret = (char)(rand() % 128);

   return ret;
}

FILE* AudioFile::myOpen(_TCHAR* tszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen)
{
   FILE* file = _tfopen(tszFileName, _T("wb")); // obviously "b" has an effect while writing...
   if (file == NULL)
      return NULL;

   fileOffset_ = 0;
   lastAscendOffset_ = 0;

   if (useLadFormat_)
   {
      // write the LAD header
      char lad[4];
      lad[0] = 'L';
      lad[1] = 'A';
      lad[2] = 'D';
      lad[3] = ' ';

      fwrite(lad, 1, 4, file);
      fileOffset_ += 4;

      // generate a random key
      for (int i=0; i<SIZE_LADKEY; ++i)
         szLadKey_[i] = getRandomByte();

      fwrite(szLadKey_, 1, SIZE_LADKEY, file);
      fileOffset_ += SIZE_LADKEY;
      keyOffset_ = 0;

      // fill the LAD info field with random bytes
      int infoSize = strlen(szLadInfo_);
      for (i=infoSize + 1; i<SIZE_LADINFO; ++i)
         szLadInfo_[i] = getRandomByte();

      // write the LAD info encryptedly
      myWrite(file, szLadInfo_, SIZE_LADINFO);
   }

   return file;
}

int AudioFile::myCreateChunk(FILE* hmmio, LPMMCKINFO pmmcki, UINT fuCreate)
{
   char buf[12]; // 12 is enough
   if (fuCreate & MMIO_CREATERIFF)
   {
      pmmcki->ckid = 'FFIR';
      pmmcki->dwDataOffset = fileOffset_;
      DWORD* p = (DWORD*) buf;
      (*p) = pmmcki->ckid;
      p++;
      (*p) = 0;
      p++;
      (*p) = pmmcki->fccType;

      myWrite(hmmio, buf, 12);
   }
   else
   {
      pmmcki->dwDataOffset = fileOffset_;
      // assume fuCreate == 0
      DWORD* p = (DWORD*) buf;
      (*p) = pmmcki->ckid;
      p++;
      (*p) = 0;
      
      myWrite(hmmio, buf, 8);
   }

   return 0;
}

int AudioFile::myAscend(FILE* hmmio, LPMMCKINFO pmmcki, UINT fuAscend)
{
   int backupKeyOffset = keyOffset_;
   int backupFileOffset = fileOffset_;
   // check for odd chunk size
   unsigned __int32 chunkLength = fileOffset_ - pmmcki->dwDataOffset - 8;
   if (chunkLength % 2 == 1)
   {
      // pad 1 byte
      char t[2];
      t[0] = 0;
      myWrite(hmmio, t, 1);
      backupKeyOffset = keyOffset_;
      backupFileOffset = fileOffset_;
      chunkLength += 1;
   }

   fseek(hmmio, pmmcki->dwDataOffset + 4, SEEK_SET);
   keyOffset_ = (pmmcki->dwDataOffset - 896 + 4) % SIZE_LADKEY;
   fileOffset_ = pmmcki->dwDataOffset + 4;

   writeLE32Int(chunkLength);

   fseek(hmmio, backupFileOffset, SEEK_SET);
   fileOffset_ = backupFileOffset;
   keyOffset_  = backupKeyOffset;

   return MMSYSERR_NOERROR;
}

long AudioFile::myWrite(FILE* hmmio, const char* pch, LONG cch)
{
   const char* buf;

   if (useLadFormat_)
   {
      bool bufferChanged = false;
      while (cch > xorBufferSize_)
      {
         xorBufferSize_ *= 2;
         bufferChanged = true;
      }
      if (bufferChanged)
      {
         free(xorBuffer_);
         xorBuffer_ = (char*) realloc(xorBuffer_, xorBufferSize_);
         if (xorBuffer_ == NULL)
            throw MemoryAudioException(_T("Could not increase write buffer size!"));
      }

      // now we know that the write buffer is large enough!
      for (int i=0; i<cch; ++i)
      {
         xorBuffer_[i] = (char) ((pch[i]) ^ (szLadKey_[keyOffset_]));
         keyOffset_ = (keyOffset_ + 1) % SIZE_LADKEY;
      }

      buf = xorBuffer_;
   }
   else
   {
      buf = pch;
   }

   int written = fwrite(buf, 1, cch, hmmio);
   fileOffset_ += written;

   if (written != cch)
   {
      cerr << "Arghll! myWrite() did not write buffer completely!" << endl;
   }

   // Finish the lad/wav file (update the two length fields).
   //
   // Do not do it at the beginning, do not do it for padding (cch == 1)
   // and only do this every once in a while.
   if (fileOffset_ - lastAscendOffset_ > 80000 && cch > 2) 
   {
      if (wavFile_->dataChunkOpen)
      {
         int mmres = myAscend(wavFile_->wavFileHandle, &wavFile_->dataChunk, 0);
      }

      if (wavFile_->riffChunkOpen)
      {
         int mmres = myAscend(wavFile_->wavFileHandle, &wavFile_->riffChunk, 0);
      }

      lastAscendOffset_ = fileOffset_;
   }

   return written;
}

int AudioFile::myClose(FILE* hmmio, UINT fuClose)
{
   fclose(hmmio);

   return MMSYSERR_NOERROR;
}
