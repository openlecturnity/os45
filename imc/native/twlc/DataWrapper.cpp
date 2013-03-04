#include "DataWrapper.h"

OutputDataWrapper::OutputDataWrapper(unsigned char target[], long length)
{
   target_ = target;
   availBytes_ = length;
   writtenBytes_ = 0;
}

OutputDataWrapper::~OutputDataWrapper()
{
}

void OutputDataWrapper::write(int i)
{
   *(target_++) = (unsigned char)(i >> 24);
   *(target_++) = (unsigned char)(i >> 16);
   *(target_++) = (unsigned char)(i >> 8);
   *(target_++) = (unsigned char)(i >> 0);

   writtenBytes_ += 4;
   availBytes_ -= 4;
}

void OutputDataWrapper::write(short s)
{
   *(target_++) = (unsigned char)(s >> 8);
   *(target_++) = (unsigned char)(s >> 0);

   writtenBytes_ += 2;
   availBytes_ -= 2;
}

void OutputDataWrapper::write(char c)
{
   *(target_++) = c;
   ++writtenBytes_;
   --availBytes_;
}

void OutputDataWrapper::write(unsigned char c)
{
   *(target_++) = c;
   ++writtenBytes_;
   --availBytes_;
}

void OutputDataWrapper::write(unsigned char c1, unsigned char c2)
{
   *(target_++) = c1;
   *(target_++) = c2;
   
   writtenBytes_ += 2;
   availBytes_ -= 2;
}

void OutputDataWrapper::write(unsigned char c1, unsigned char c2, unsigned char c3)
{
   *(target_++) = c1;
   *(target_++) = c2;
   *(target_++) = c3;
   
   writtenBytes_ += 3;
   availBytes_ -= 3;
}

unsigned char* OutputDataWrapper::getData()
{
   return target_ - writtenBytes_;
}

long OutputDataWrapper::countWrittenBytes()
{
   return writtenBytes_;
}

void OutputDataWrapper::clear()
{
   target_ -= writtenBytes_;
   availBytes_ += writtenBytes_;
   writtenBytes_ = 0;
}


/*****************************************************************************
*****************************************************************************/

InputDataWrapper::InputDataWrapper(unsigned char source[], long avail)
{
   source_ = source;
   readBytes_ = 0;
   availBytes_ = avail;
}

InputDataWrapper::~InputDataWrapper()
{
}

void InputDataWrapper::resetData(unsigned char source[], long avail)
{
   source_ = source;
   readBytes_ = 0;
   availBytes_ = avail;
}

short InputDataWrapper::readShort()
{
   availBytes_ -= 2;
   readBytes_ += 2;
   short result = *(source_++) << 8;
   result |= *(source_++);
   return result;
}

short InputDataWrapper::readIntelShort()
{
   availBytes_ -= 2;
   readBytes_ += 2;
   short result = *((short *)source_);
   source_ += 2;
   return result;
}


char InputDataWrapper::readChar()
{
   --availBytes_;
   ++readBytes_;
   return *(source_++);
}

unsigned char InputDataWrapper::readUChar()
{
   --availBytes_;
   ++readBytes_;
   return *(source_++);
}



void InputDataWrapper::read(int *i)
{
   // this cannot be one line because that would mean:
   // result = source | source | source | source; and then source += 4;
   *i &= *(source_++) << 24;
   *i |= *(source_++) << 16;
   *i |= *(source_++) << 8;
   *i |= *(source_++);
   availBytes_ -= 4;
   readBytes_ += 4;
}

void InputDataWrapper::read(short *s)
{
   *s &= *(source_++) << 8;
   *s |= *(source_++);
   availBytes_ -= 2;
   readBytes_ += 2;
}

void InputDataWrapper::read(char *c)
{
   --availBytes_;
   ++readBytes_;
   *c = *(source_++);
}

void InputDataWrapper::read(unsigned char *uc)
{
   --availBytes_;
   ++readBytes_;
   *uc = *(source_++);
}

void InputDataWrapper::read(unsigned char *uc1, unsigned char *uc2)
{
   availBytes_ -= 2;
   readBytes_ += 2;
   *uc1 = *(source_++);
   *uc2 = *(source_++);
}

void InputDataWrapper::read(unsigned char *uc1, unsigned char *uc2, unsigned char *uc3)
{
   availBytes_ -= 3;
   readBytes_ += 3;
   *uc1 = *(source_++);
   *uc2 = *(source_++);
   *uc3 = *(source_++);
}


long InputDataWrapper::countReadBytes()
{
   return readBytes_;
}