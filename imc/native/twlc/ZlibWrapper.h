#ifndef __ZLIBWRAPPER_H__
#define __ZLIBWRAPPER_H__

#include "zlib.h"

class OutputZlibWrapper
{
   unsigned char* target_;
   z_stream* stream_;
   bool finished_;
   
public:
   OutputZlibWrapper(unsigned char target[], int avail);
   ~OutputZlibWrapper();
   void compress(unsigned char data[], int length);
   void flush();
   int finish();
   void toString(char* buffer);
};

class InputZlibWrapper
{
   z_stream* stream_;
   
public:
   InputZlibWrapper(unsigned char source[], int length);
   ~InputZlibWrapper();
   void decompress(unsigned char data[], int length);
   void toString(char* buffer);
};

class CRC32 {
public:
    CRC32();

    void Update(const unsigned char *pBuffer, int iOffset, int iLength);
    unsigned long Value();

private:
    unsigned long m_lCrcId;
};

#endif