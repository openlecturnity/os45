#ifndef __CLADPARSERPINS_H__
#define __CLADPARSERPINS_H__

#ifndef __STREAMS__
#include <streams.h> // CTransformOutputPin
#endif

class CLadParserInputPin : public CBasePin
{
   friend class CLadParserOutputPin;
   friend class COutputPinAsyncReader;
   friend class CLadParser;
public:
   CLadParserInputPin(
      CBaseFilter *pFilter,
      CCritSec *pLock,
      HRESULT *phr,
      LPCWSTR pName);
   ~CLadParserInputPin();

   // IPin
   virtual HRESULT CheckMediaType(const CMediaType *pmt);
   virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
   virtual HRESULT CheckConnect(IPin *pPin);

   virtual HRESULT __stdcall BeginFlush();
   virtual HRESULT __stdcall EndFlush();

   // CUnknown
   virtual STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

private:
   BYTE m_szLadKey[892];
   char m_szTypeInfo[128];
};

class COutputPinAsyncReader : public IAsyncReader
{
   friend class CLadParserOutputPin;
public:
   COutputPinAsyncReader();
   ~COutputPinAsyncReader();

   // IAsyncReader
   //*
   virtual HRESULT __stdcall BeginFlush();
   virtual HRESULT __stdcall EndFlush();
   virtual HRESULT __stdcall Length(LONGLONG *pTotal, LONGLONG *pAvailable);
   virtual HRESULT __stdcall RequestAllocator(
      IMemAllocator *pPreferred,
      ALLOCATOR_PROPERTIES *pProps,
      IMemAllocator **ppActual);
   virtual HRESULT __stdcall Request(IMediaSample *pSample, DWORD_PTR dwUser);
   virtual HRESULT __stdcall SyncReadAligned(IMediaSample *pSample);
   virtual HRESULT __stdcall SyncRead(LONGLONG llPosition, LONG lLength, BYTE *pBuffer);
   virtual HRESULT __stdcall WaitForNext(
      DWORD dwTimeout,
      IMediaSample **ppSample,
      DWORD_PTR *pdwUser);

   // IUnknown
   virtual ULONG __stdcall AddRef();
   virtual ULONG __stdcall Release();
   virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppvObject);

private:
   CLadParserOutputPin *m_pPin;
   IAsyncReader        *m_pAsyncReader;
   BYTE                *m_szLadKey; // from CLadParserInputPin
   long					m_lAlign;

   HRESULT AssertAsyncReader();
   void AddLadOffset(IMediaSample *pSample, int nTimes = 1);
   void SubtractLadOffset(IMediaSample *pSample);
   
   void Decrypt(LONGLONG lPosition, LONG lLength, BYTE *pBuffer);
   void Decrypt(IMediaSample *pSample);
};

class CLadParserOutputPin : public CBasePin
{
   friend class CLadParserInputPin;
   friend class COutputPinAsyncReader;
public:
   CLadParserOutputPin(
      CBaseFilter *pFilter,
      CCritSec *pLock,
      HRESULT *phr,
      LPCWSTR pName);
   ~CLadParserOutputPin();

   // IPin
   virtual HRESULT CheckMediaType(const CMediaType *pmt);
   virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
   virtual HRESULT BreakConnect();

   virtual HRESULT __stdcall BeginFlush();
   virtual HRESULT __stdcall EndFlush();

   // CUnknown
   virtual STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

private:
   COutputPinAsyncReader m_outputPinAsyncReader;
};

#endif