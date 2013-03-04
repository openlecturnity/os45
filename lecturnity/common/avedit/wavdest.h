//------------------------------------------------------------------------------
// File: WavDest.h
//
// Desc: DirectShow sample code - header file for WAV audio file writer filter.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#if !defined(__WAVDEST__H__)
#define __WAVDEST__H__

#ifndef __STREAMS__
#include <streams.h> // CTransformOutputPin
#endif

#pragma warning(disable: 4097 4511 4512 4514 4705)

// {908D089F-60BC-424e-AB3F-394DAA5C16A9}
DEFINE_GUID(IID_ILadMux, 
0x908d089f, 0x60bc, 0x424e, 0xab, 0x3f, 0x39, 0x4d, 0xaa, 0x5c, 0x16, 0xa9);

// {3C1A3763-C816-4c57-9B69-9F9AE3E1EDDC}
DEFINE_GUID(MEDIASUBTYPE_LAD, 
0x3c1a3763, 0xc816, 0x4c57, 0x9b, 0x69, 0x9f, 0x9a, 0xe3, 0xe1, 0xed, 0xdc);

interface ILadMux : public IUnknown
{
public:
   STDMETHOD(SetLadInfo)(const char *szLadInfo=NULL) = 0;
   STDMETHOD(GetCurrentPosition)(REFERENCE_TIME *prtTarget) = 0;
};

class CWavDestOutputPin : public CTransformOutputPin
{
public:
   CWavDestOutputPin(CTransformFilter *pFilter, HRESULT * phr);
   
   STDMETHODIMP EnumMediaTypes( IEnumMediaTypes **ppEnum );
   HRESULT CheckMediaType(const CMediaType* pmt);
};

class CWavDestFilter : public CTransformFilter, public ILadMux
{
   
public:
   
   DECLARE_IUNKNOWN;
   
   CWavDestFilter(LPUNKNOWN pUnk, HRESULT *pHr);
   ~CWavDestFilter();
   /* alt
   static CUnknown *CreateInstance(LPUNKNOWN punk, HRESULT *pHr);
   */
   static HRESULT CreateInstance(CWavDestFilter **ppDest);
   
   STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
   
   HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);
   HRESULT Receive(IMediaSample *pSample);
   
   HRESULT CheckInputType(const CMediaType* mtIn) ;
   HRESULT CheckTransform(const CMediaType *mtIn,const CMediaType *mtOut);
   HRESULT GetMediaType(int iPosition, CMediaType *pMediaType) ;
   
   HRESULT DecideBufferSize(IMemAllocator *pAlloc,
      ALLOCATOR_PROPERTIES *pProperties);
   
   HRESULT StartStreaming();
   HRESULT StopStreaming();
   
   HRESULT CompleteConnect(PIN_DIRECTION direction,IPin *pReceivePin) { return S_OK; }
   
   // ILadMux
   STDMETHODIMP SetLadInfo(const char *szLadInfo=NULL);
   STDMETHODIMP GetCurrentPosition(REFERENCE_TIME *prtTarget);

   
   STDMETHODIMP SetExpectedLength(UINT nLengthMs) { m_nExpectedLengthMs = nLengthMs; return S_OK; }
private:
   
   HRESULT Copy(IMediaSample *pSource, IMediaSample *pDest) const;
   HRESULT Transform(IMediaSample *pMediaSample);
   HRESULT Transform(AM_MEDIA_TYPE *pType, const signed char ContrastLevel) const;

   HRESULT FillMissingBytes(IStream *pStream, UINT nMissing);
   
   DWORD m_dwBytesPerSecond;
   REFERENCE_TIME m_rtCurrentPos;

   ULONG m_cbWavData;
   ULONG m_cbHeader;
   UINT  m_nExpectedLengthMs;

   BYTE  m_ladKey[892];
   char  m_ladInfo[128];

   bool  m_bCreateLadFile;
};

#endif // __WAVDEST__H__