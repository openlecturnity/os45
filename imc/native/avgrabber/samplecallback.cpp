#include "global.h"

CSampleCallback::CSampleCallback()
{
   refCounter_ = 0;
   i64counter_ = 0;
   pWdm_       = NULL;
}

CSampleCallback::~CSampleCallback()
{
}

HRESULT CSampleCallback::SampleCB(double sampleTime, IMediaSample *pSample)
{
#ifdef _DEBUG
   printf("SampleCB(%f)\n", sampleTime);
#endif

   ((WdmCapturer *) pWdm_)->HandleSample(sampleTime, pSample);

   return S_OK;
}


HRESULT CSampleCallback::BufferCB(double sampleTime, BYTE *pBuffer, long bufferLen)
{
#ifdef _DEBUG
   printf("BufferCB(%f, %08x, %d)\n", sampleTime, pBuffer, bufferLen);
#endif
   ASSERT(false);

   i64counter_++;
   return S_OK;
}

HRESULT CSampleCallback::QueryInterface(REFIID riid, void **ppv)
{
   // cout << "CSampleCallback::QueryInterface" << endl;
   if (riid == IID_IUnknown ||
       riid == IID_ISampleGrabberCB)
   {
      *ppv = this;
      return S_OK;
   }

   return E_NOINTERFACE;
}

ULONG CSampleCallback::AddRef()
{
#ifdef _DEBUG
   printf("CSampleCallback::AddRef()\n");
#endif
   return (++refCounter_);
}

ULONG CSampleCallback::Release()
{
#ifdef _DEBUG
   printf("CSampleCallback::Release()\n");
#endif
   ULONG t = --refCounter_;

   if (refCounter_ == 0)
      delete this;

   return t;
}

void CSampleCallback::SetWdmCapturer(void *pWdmCapturer)
{
#ifdef _DEBUG
   printf("CSampleCallback::SetWdmCapturer()\n");
#endif
   pWdm_ = pWdmCapturer;
}
