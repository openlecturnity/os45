#ifndef __L_MEDIASTREAM__
#define __L_MEDIASTREAM__

#include "export.h"

class FILESDK_EXPORT CMediaStream
{
public:
   CMediaStream();
   virtual ~CMediaStream() {}

public:
   virtual void AddReference();
   virtual int  ReleaseReference();
   
private:
   int m_nRefCounter;
};

#endif
