#ifndef __CSAMPLECALLBACK_H__
#define __CSAMPLECALLBACK_H__

class CSampleCallback : public ISampleGrabberCB
{
public:
   CSampleCallback();
   ~CSampleCallback();

   // For the WdmCapturer:
   void SetWdmCapturer(void *pWdmCapturer);

   // ISampleGrabberCB:
   virtual HRESULT __stdcall SampleCB(double sampleTime, IMediaSample *pSample);
   virtual HRESULT __stdcall BufferCB(double sampleTime, BYTE *pBuffer, long bufferLen);

   // IUnknown:
   virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppv);
   virtual ULONG __stdcall AddRef();
   virtual ULONG __stdcall Release();

private:
   ULONG refCounter_;
   __int64 i64counter_;

   void *pWdm_;
};

#endif
