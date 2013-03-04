#include <stdio.h>
#include <string.h>
#include "ZlibWrapper.h"

OutputZlibWrapper::OutputZlibWrapper(unsigned char target[], int avail)
{
   target_ = target;
   stream_ = new z_stream();
   stream_->zalloc = Z_NULL;
   stream_->zfree = Z_NULL;
   stream_->opaque = 0;
   finished_ = false;

   deflateInit(stream_, Z_DEFAULT_COMPRESSION);

   // stream_->next_in will be the data buffers passed to compress
   stream_->avail_in = 0; // this not given causes problems with 16-Bit data ??
   
   stream_->next_out = target;
   stream_->avail_out = avail;
}

OutputZlibWrapper::~OutputZlibWrapper()
{
   // TODO: check for Z_STREAM_END / ERRORS
   if (!finished_)
      deflate(stream_, Z_FINISH);
 
   deflateEnd(stream_);

   delete stream_;
}

void OutputZlibWrapper::compress(unsigned char data[], int length)
{
   stream_->next_in = data;
   stream_->avail_in = length;

   while (stream_->avail_in > 0)
   {
      deflate(stream_, 0);
      // TODO: check for errors (eg avail_out == 0)
   }
}

void OutputZlibWrapper::flush()
{
   deflate(stream_, Z_SYNC_FLUSH);
   // TODO: use Z_FULL_FLUSH or support it otherwise?
}

int OutputZlibWrapper::finish()
{
   // TODO: check for Z_STREAM_END / ERRORS
   deflate(stream_, Z_FINISH);
   finished_ = true;
   return stream_->total_out;
}

void OutputZlibWrapper::toString(char* buffer)
{
   char t[512];
   sprintf(t, "avail_in=%d avail_out=%d written=%d state=%s",
      stream_->avail_in, stream_->avail_out, stream_->total_out, finished_ ? "closed" : "open");
   strcat(buffer, t);
}


/*****************************************************************************
*****************************************************************************/


InputZlibWrapper::InputZlibWrapper(unsigned char source[], int length)
{
   // TODO: provide "stream" ability
   // that is provide small source arrays iterated

   stream_ = new z_stream();
   stream_->zalloc = Z_NULL;
   stream_->zfree = Z_NULL;
   stream_->opaque = 0;

   stream_->next_in = source;
   stream_->avail_in = length;

   // TODO: check for errors, namely Z_XYZ_ERROR
   inflateInit(stream_);
}

InputZlibWrapper::~InputZlibWrapper()
{
   // TODO: check for errors, namely Z_XYZ_ERROR
   inflateEnd(stream_);
   delete stream_;
}

int errorOccured = 0;

void InputZlibWrapper::decompress(unsigned char output[], int length)
{
   // TODO: provide ability to decompress less than length
   stream_->next_out = output;
   stream_->avail_out = length;

   // TODO: check for positive avail_out!!!
   int retVal = inflate(stream_, 0);

   if (retVal != Z_OK && retVal != Z_STREAM_END)
      errorOccured = retVal;
}

void InputZlibWrapper::toString(char* buffer)
{
   char t[100];
   sprintf(t, "Stream error state: %d", errorOccured);
   errorOccured = 0;
   strcat(buffer, t);
}


CRC32::CRC32() {
    m_lCrcId = crc32(0L, Z_NULL, 0);
}

void CRC32::Update(const unsigned char *pBuffer, int iOffset, int iLength) {
    m_lCrcId = crc32(m_lCrcId, pBuffer + iOffset, iLength);
}

unsigned long CRC32::Value() {
    return m_lCrcId;
}