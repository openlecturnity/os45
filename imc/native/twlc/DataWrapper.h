#ifndef __DATAWRAPPER_H__
#define __DATAWRAPPER_H__

class OutputDataWrapper
{
   unsigned char* target_;
   long writtenBytes_;
   long availBytes_;
   
public:
   OutputDataWrapper(unsigned char target[], long avail);
   ~OutputDataWrapper();
   void write(int i);
   void write(short s);
   void write(char c);
   void write(unsigned char c);
   void write(unsigned char c1, unsigned char c2);
   void write(unsigned char c1, unsigned char c2, unsigned char c3);
   unsigned char* getData();
   long countWrittenBytes();
   void clear();
};

class InputDataWrapper
{
   unsigned char* source_;
   long availBytes_;
   long readBytes_;
   
public:
   InputDataWrapper(unsigned char source[], long avail);
   ~InputDataWrapper();
   void resetData(unsigned char source[], long avail);
   short readShort();
   short readIntelShort();
   char readChar();
   unsigned char readUChar();
   void read(int *i);
   void read(short *s);
   void read(char *c);
   void read(unsigned char *uc);
   void read(unsigned char *uc1, unsigned char *uc2);
   void read(unsigned char *uc1, unsigned char *uc2, unsigned char *uc3);
   long countReadBytes();
};

#endif