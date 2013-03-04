#include "StdAfx.h"

#include "ladparserpins.h"
#include "ladfilter.h"

/* ----------------------------------------------
 * INPUT PIN
 * ---------------------------------------------- */

CLadParserInputPin::CLadParserInputPin(
   CBaseFilter *pFilter,
   CCritSec *pLock,
   HRESULT *phr,
   LPCWSTR pName) :
CBasePin(
   _T("LAD Parser Input Pin"),
   pFilter,
   pLock,
   phr,
   pName,
   PINDIR_INPUT)
{
}

CLadParserInputPin::~CLadParserInputPin()
{
}

HRESULT CLadParserInputPin::CheckMediaType(const CMediaType *pmt)
{
   if (pmt == NULL)
      return E_POINTER;
   BOOL typeOk = IsEqualGUID(*(pmt->Type()), MEDIATYPE_Stream);
   BOOL subTypeOk = IsEqualGUID(*(pmt->Subtype()), GUID_NULL) ||
                    IsEqualGUID(*(pmt->Subtype()), MEDIASUBTYPE_None);

   HRESULT hr = VFW_E_TYPE_NOT_ACCEPTED;

   if (typeOk && subTypeOk)
      hr = S_OK;

   return hr;
}

HRESULT CLadParserInputPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
   if (iPosition > 0)
      return VFW_S_NO_MORE_ITEMS;
   if (iPosition < 0)
      return E_INVALIDARG;
   if (pMediaType == NULL)
      return E_POINTER;

   pMediaType->InitMediaType();
   pMediaType->SetType(&MEDIATYPE_Stream);
   pMediaType->SetSubtype(&GUID_NULL);

   return S_OK;
}

HRESULT CLadParserInputPin::CheckConnect(IPin *pPin)
{
   // Check if the other pin has an IAsyncReader interface
   IAsyncReader *pAsync = NULL;
   HRESULT hr = VFW_E_NO_TRANSPORT;

   hr = pPin->QueryInterface(IID_IAsyncReader, (void **) &pAsync);
   char ladHeader[4];
   if (SUCCEEDED(hr) && pAsync)
   {
      hr = pAsync->SyncRead(0, 4, (BYTE *) ladHeader);
   }

   // Try to read the 'LAD ' "FCC" from the async reader:
   if (SUCCEEDED(hr))
   {
      if (ladHeader[0] != 'L' ||
          ladHeader[1] != 'A' ||
          ladHeader[2] != 'D' ||
          ladHeader[3] != ' ')
         hr = VFW_E_INVALID_FILE_FORMAT;
   }

   // Now try to read the LAD key
   if (SUCCEEDED(hr))
   {
      hr = pAsync->SyncRead(4, 892, m_szLadKey);
   }

   // ... and the TypeInfo, just for fun
   if (SUCCEEDED(hr))
   {
      hr = pAsync->SyncRead(896, 128, (BYTE *) m_szTypeInfo);
   }

   // XOR the TypeInfo to make it readable
   if (SUCCEEDED(hr))
   {
      for (int i=0; i<128; ++i)
         m_szTypeInfo[i] ^= m_szLadKey[i];
#ifdef _DEBUG
      printf("LAD Parser: Type info '%s'\n", m_szTypeInfo, hr);
#endif
   }

   if (pAsync)
      pAsync->Release();
   pAsync = NULL;

   return hr;
}

HRESULT CLadParserInputPin::BeginFlush()
{
   return S_OK;
}

HRESULT CLadParserInputPin::EndFlush()
{
   return S_OK;
}

// CUnknown
HRESULT CLadParserInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
   if (riid == IID_IPin)
      return GetInterface((IPin *) this, ppv);
   return CUnknown::NonDelegatingQueryInterface(riid, ppv);
}

/* ----------------------------------------------
 * OUTPUT PIN ASYNC READER
 * ---------------------------------------------- */

COutputPinAsyncReader::COutputPinAsyncReader()
{
   m_pPin         = NULL;
   m_pAsyncReader = NULL;
   m_szLadKey     = NULL;
   m_lAlign		  = 1024;
}

COutputPinAsyncReader::~COutputPinAsyncReader()
{
   if (m_pAsyncReader)
      m_pAsyncReader->Release();
   m_pAsyncReader = NULL;
}

// IUnknown
ULONG COutputPinAsyncReader::AddRef()
{
   return m_pPin->AddRef();
}

ULONG COutputPinAsyncReader::Release()
{
   return m_pPin->Release();
}

HRESULT COutputPinAsyncReader::QueryInterface(REFIID riid, void **ppv)
{
   return m_pPin->QueryInterface(riid, ppv);
}

// IAsyncReader
HRESULT COutputPinAsyncReader::AssertAsyncReader()
{
   if (m_pAsyncReader)
      return S_OK;
   
   CLadParser *pParser = (CLadParser *) (m_pPin->m_pFilter);

   if (FALSE == pParser->m_pInputPin->IsConnected())
      return VFW_E_NOT_CONNECTED;

   IPin *pConnected = pParser->m_pInputPin->GetConnected();
   
   HRESULT hr = 
      pConnected->QueryInterface(IID_IAsyncReader, (void **) &m_pAsyncReader);
   return hr;
}

HRESULT COutputPinAsyncReader::BeginFlush()
{
   HRESULT hr = AssertAsyncReader();
   
   if (SUCCEEDED(hr))
      hr = m_pAsyncReader->BeginFlush();
   return hr;
}

HRESULT COutputPinAsyncReader::EndFlush()
{
   HRESULT hr = AssertAsyncReader();
   
   if (SUCCEEDED(hr))
      hr = m_pAsyncReader->EndFlush();
   return hr;
}

HRESULT COutputPinAsyncReader::Length(LONGLONG *pTotal, LONGLONG *pAvailable)
{
   HRESULT hr = AssertAsyncReader();

   if (SUCCEEDED(hr))
   {
      hr = m_pAsyncReader->Length(pTotal, pAvailable);
      *pTotal -= 1024i64;
      *pAvailable -= 1024i64;

   }

   return hr;
}

HRESULT COutputPinAsyncReader::RequestAllocator(
      IMemAllocator *pPreferred,
      ALLOCATOR_PROPERTIES *pProps,
      IMemAllocator **ppActual)
{
   HRESULT hr = AssertAsyncReader();

   if (SUCCEEDED(hr))
   {
      hr = m_pAsyncReader->RequestAllocator(
         pPreferred,
         pProps,
         ppActual);
   }
   m_lAlign = pProps->cbAlign;

   return hr;
}

void COutputPinAsyncReader::AddLadOffset(IMediaSample *pSample, int nTimes)
{
   LONGLONG timeStart, timeEnd;
   pSample->GetTime(&timeStart, &timeEnd);
   timeStart += 10240000000i64 * nTimes; // 1024 bytes = LAD header
   timeEnd   += 10240000000i64 * nTimes; // 1024 bytes = LAD header
   pSample->SetTime(&timeStart, &timeEnd);
}

void COutputPinAsyncReader::SubtractLadOffset(IMediaSample *pSample)
{
   LONGLONG timeStart, timeEnd;
   pSample->GetTime(&timeStart, &timeEnd);
   timeStart -= 10240000000i64; // 1024 bytes = LAD header
   timeEnd   -= 10240000000i64; // 1024 bytes = LAD header

   pSample->SetTime(&timeStart, &timeEnd);
}

void COutputPinAsyncReader::Decrypt(LONGLONG llPosition, LONG lLength, BYTE *pBuffer)
{
   if (m_szLadKey == NULL)
   {
      CLadParser *pParser = (CLadParser *) m_pPin->m_pFilter;

      if (pParser->m_pInputPin)
      {
         // Only copy the pointer!
         m_szLadKey = pParser->m_pInputPin->m_szLadKey;
      }
   }

   int iLength = (int) lLength;
   int iOffset = (int) ((llPosition + 128i64) % 892i64);

   for (int i=0; i<iLength; ++i)
      pBuffer[i] ^= m_szLadKey[(iOffset + i) % 892];
}

void COutputPinAsyncReader::Decrypt(IMediaSample *pSample)
{
   LONGLONG lStart100ns, lEnd100ns;
   LONGLONG lStartBytes;
   LONG     lLengthBytes;
   BYTE    *pBuffer;

   HRESULT hr = pSample->GetTime(&lStart100ns, &lEnd100ns);
   if (SUCCEEDED(hr))
      hr = pSample->GetPointer(&pBuffer);
   if (SUCCEEDED(hr))
   {
      lStartBytes = lStart100ns / 10000000i64;
      lLengthBytes = (LONG) ((lEnd100ns / 10000000i64) - lStartBytes);

      Decrypt(lStartBytes, lLengthBytes, pBuffer);
   }
}

HRESULT COutputPinAsyncReader::Request(IMediaSample *pSample, DWORD_PTR dwUser)
{
   HRESULT hr = AssertAsyncReader();

   if (SUCCEEDED(hr))
   {
	   LONGLONG ms = 0, me = 0;
	   pSample->GetTime(&ms,&me);
	   if(ms==0)
	   {
		   AddLadOffset(pSample,(m_lAlign - 1) / 1024 + 1);
	   }
	   hr = m_pAsyncReader->Request(pSample, dwUser);
   }

   return hr;
}

HRESULT COutputPinAsyncReader::SyncReadAligned(IMediaSample *pSample)
{
   HRESULT hr = AssertAsyncReader();

   if (SUCCEEDED(hr))
   {
      AddLadOffset(pSample);
      hr = m_pAsyncReader->SyncReadAligned(pSample);
      SubtractLadOffset(pSample);
   }

   if (SUCCEEDED(hr))
   {
      Decrypt(pSample);
   }

   return hr;
}

HRESULT COutputPinAsyncReader::SyncRead(LONGLONG llPosition, LONG lLength, BYTE *pBuffer)
{
   HRESULT hr = AssertAsyncReader();

   if (SUCCEEDED(hr))
   {
      hr = m_pAsyncReader->SyncRead(llPosition + 1024i64, lLength, pBuffer);
   }

   if (SUCCEEDED(hr))
      Decrypt(llPosition, lLength, pBuffer);
   return hr;
}

HRESULT COutputPinAsyncReader::WaitForNext(
   DWORD dwTimeout,
   IMediaSample **ppSample,
   DWORD_PTR *pdwUser)
{
   HRESULT hr = AssertAsyncReader();

   if (SUCCEEDED(hr))
   {
      hr = m_pAsyncReader->WaitForNext(dwTimeout, ppSample, pdwUser);
   }

   if (SUCCEEDED(hr))
   {
      SubtractLadOffset(*ppSample);
      Decrypt(*ppSample);
   }

   return hr;
}

 /* ----------------------------------------------
 * OUTPUT PIN
 * ---------------------------------------------- */

CLadParserOutputPin::CLadParserOutputPin(
      CBaseFilter *pFilter,
      CCritSec *pLock,
      HRESULT *phr,
      LPCWSTR pName) : 
   CBasePin(
      _T("LAD Parser Output Pin"),
      pFilter,
      pLock,
      phr,
      pName,
      PINDIR_OUTPUT)
{
   m_outputPinAsyncReader.m_pPin = this;
}

CLadParserOutputPin::~CLadParserOutputPin()
{
   ASSERT(m_cRef == 0);
}

HRESULT CLadParserOutputPin::CheckMediaType(const CMediaType *pmt)
{
   if (pmt == NULL)
      return E_POINTER;

   BOOL typeOk = IsEqualGUID(*(pmt->Type()), MEDIATYPE_Stream);
   BOOL subTypeOk = IsEqualGUID(*(pmt->Subtype()), MEDIASUBTYPE_WAVE);
   // For MP3 in LAD file:
   //BOOL subTypeOk = IsEqualGUID(*(pmt->Subtype()), MEDIASUBTYPE_MPEG1Audio);

   HRESULT hr = VFW_E_TYPE_NOT_ACCEPTED;

   if (typeOk && subTypeOk)
      hr = S_OK;

   return hr;
}

HRESULT CLadParserOutputPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
   if (iPosition > 0)
      return VFW_S_NO_MORE_ITEMS;
   if (iPosition < 0)
      return E_INVALIDARG;
   if (pMediaType == NULL)
      return E_POINTER;

   pMediaType->InitMediaType();
   pMediaType->SetType(&MEDIATYPE_Stream);
   pMediaType->SetSubtype(&MEDIASUBTYPE_WAVE);
   // For MP3 in LAD file:
   //pMediaType->SetSubtype(&MEDIASUBTYPE_MPEG1Audio);

   return S_OK;
}

HRESULT CLadParserOutputPin::BreakConnect()
{
   if (m_outputPinAsyncReader.m_pAsyncReader)
      m_outputPinAsyncReader.m_pAsyncReader->Release();
   m_outputPinAsyncReader.m_pAsyncReader = NULL;

   return S_OK;
}

// CUnknown
HRESULT CLadParserOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
   if (riid == IID_IAsyncReader)
      return GetInterface((IAsyncReader *) &m_outputPinAsyncReader, ppv);
   return CBasePin::NonDelegatingQueryInterface(riid, ppv);
}

HRESULT CLadParserOutputPin::BeginFlush()
{
   return S_OK;
}

HRESULT CLadParserOutputPin::EndFlush()
{
   return S_OK;
}
