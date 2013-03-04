//------------------------------------------------------------------------------
// File: WavDest.cpp
//
// Desc: DirectShow sample code - a filter for writing WAV audio files (based
//       on CTransformFilter).
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


//
// To use this filter to write audio data into a WAV file: 
//
// Use GraphEdit (or a custom DirectShow app) to build a filter 
// graph with an audio stream connected to this filter's input pin and the 
// File Writer filter connected to its output pin. Run the graph and you'll have
// written a wave file.
//
//=============================================================================
//=============================================================================

#include "StdAfx.h"
#include "wavdest.h"
#include <aviriff.h>
#include <malloc.h>

#define LAD_XOR_LENGTH 892
#define LAD_INFO_LENGTH 128
#define LAD_HDR_LENGTH 1024 
// LAD_HDR_LENGTH = 4 + LAD_XOR_LENGTH + LAD_INFO_LENGTH

// {3C78B8E2-6C4D-11d1-ADE2-0000F8754B99}
static const GUID CLSID_WavDest =
{ 0x3c78b8e2, 0x6c4d, 0x11d1, { 0xad, 0xe2, 0x0, 0x0, 0xf8, 0x75, 0x4b, 0x99 } };

/*
static const WCHAR g_wszName[] = L"Some Filter";

const AMOVIESETUP_FILTER sudWavDest =
{
   &CLSID_WavDest,           // clsID
      g_wszName,              // strName
      MERIT_DO_NOT_USE,         // dwMerit
      0,                        // nPins
      0                         // lpPin
};


CFactoryTemplate g_Templates[]= {
   {g_wszName, &CLSID_WavDest, CWavDestFilter::CreateInstance, NULL, &sudWavDest},
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);;
//*/

// ------------------------------------------------------------------------
// filter constructor

#pragma warning(disable:4355)
CWavDestFilter::CWavDestFilter(LPUNKNOWN pUnk, HRESULT *phr) :
CTransformFilter(NAME("Lad/WavDest filter"), pUnk, CLSID_WavDest)
{
   ASSERT(m_pOutput == 0);
   
   m_rtCurrentPos = 0;
   m_dwBytesPerSecond = 0;

   if(SUCCEEDED(*phr))
   {
      // Create an output pin so we can have control over the connection
      // media type.
      CWavDestOutputPin *pOut = new CWavDestOutputPin(this, phr);
      if(pOut)
      {
         if(SUCCEEDED(*phr))
         {
            m_pOutput = pOut;
         }
         else
         {
            delete pOut;
         }
      }
      else
      {
         *phr = E_OUTOFMEMORY;
      }
      //
      // NOTE!: If we've created our own output pin we must also create
      // the input pin ourselves because the CTransformFilter base class 
      // will create an extra output pin if the input pin wasn't created.        
      //
      CTransformInputPin *pIn = new CTransformInputPin(
         NAME("Transform input pin"),
         this,              // Owner filter
         phr,               // Result code
         L"In");            // Pin name
      // a failed return code should delete the object
      if (pIn)
      {
         if (SUCCEEDED(*phr))
         {
            m_pInput = pIn;
         }
         else
         {
            delete pIn;
         }
      }
      else
      {
         *phr = E_OUTOFMEMORY;
      }
   }

   ZeroMemory(m_ladInfo, LAD_INFO_LENGTH);
   m_bCreateLadFile = false;

   srand(timeGetTime());
   for (int i=0; i<LAD_XOR_LENGTH; ++i)
      m_ladKey[i] = (BYTE) (rand() % 256);
}

// ------------------------------------------------------------------------
// destructor

CWavDestFilter::~CWavDestFilter()
{
}

/* alt
CUnknown * CWavDestFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
   return new CWavDestFilter(pUnk, phr);
}
*/

HRESULT CWavDestFilter::CreateInstance(CWavDestFilter **ppDest)
{
   HRESULT hr = S_OK;
   *ppDest = new CWavDestFilter(NULL, &hr);
   (*ppDest)->AddRef();
   return hr;
}

HRESULT CWavDestFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
   if (riid == IID_ILadMux)
      return GetInterface((ILadMux *) this, ppv);
   return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
}

//
// CWavDestFilter::CheckTransform
//
// To be able to transform the formats must be identical
//
HRESULT CWavDestFilter::CheckTransform(const CMediaType *mtIn,const CMediaType *mtOut)
{
   HRESULT hr;
   if (FAILED(hr = CheckInputType(mtIn))) {
      return hr;
   }
   return NOERROR;
} // CheckTransform

// overridden because we need to know if Deliver() failed.

HRESULT CWavDestFilter::Receive(IMediaSample *pSample)
{
   ULONG cbOld = m_cbWavData;
   HRESULT hr = CTransformFilter::Receive(pSample);
   
   // don't update the count if Deliver() downstream fails.
   if(hr != S_OK) {
      m_cbWavData = cbOld;
   }
   
   return hr;
}

//
// CWavDestFilter::Transform
//
//
HRESULT CWavDestFilter::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
   REFERENCE_TIME rtStart, rtEnd;
  
   // First just copy the data to the output sample
   HRESULT hr = Copy(pIn, pOut);
   if (FAILED(hr)) {
      return hr;
   }

   // Prepare it for writing    
   LONG lActual = pOut->GetActualDataLength();
   
   if(m_cbWavData + m_cbHeader + lActual < m_cbWavData + m_cbHeader ) { // overflow
      return E_FAIL;
   }
   
   rtStart = m_cbWavData + m_cbHeader;
   rtEnd = rtStart + lActual;
   m_cbWavData += lActual;

   if (m_dwBytesPerSecond != 0)
   {
      double dSeconds = m_cbWavData / m_dwBytesPerSecond;
      m_rtCurrentPos = (REFERENCE_TIME)(dSeconds * 10i64 * 1000*1000);
   }
   
   // Setting byte counts as sample values (below) is very strange.
   // But if you do not do it there will be an error.
   //pOut->SetTime(&rtSourceStart, &rtSourceEnd);
   
   EXECUTE_ASSERT(pOut->SetTime(&rtStart, &rtEnd) == S_OK);
  
   return S_OK;
}

//
// CWavDestFilter::Copy
//
// Make destination an identical copy of source
//
HRESULT CWavDestFilter::Copy(IMediaSample *pSource, IMediaSample *pDest) const
{
   // Copy the sample data
   
   BYTE *pSourceBuffer, *pDestBuffer;
   long lSourceSize = pSource->GetActualDataLength();
   
#ifdef DEBUG    
   long lDestSize	= pDest->GetSize();
   ASSERT(lDestSize >= lSourceSize);
#endif
   
   pSource->GetPointer(&pSourceBuffer);
   pDest->GetPointer(&pDestBuffer);
   
   CopyMemory( (PVOID) pDestBuffer,(PVOID) pSourceBuffer,lSourceSize);

   // If we're writing a LAD file, xor the data, too.
   // TimeStart and TimeEnd are the byte offsets
   if (m_bCreateLadFile)
   {
      int iXorOffset = (m_cbWavData + m_cbHeader - LAD_HDR_LENGTH + LAD_INFO_LENGTH) % LAD_XOR_LENGTH;
      for (int i=0; i<lSourceSize; ++i)
         pDestBuffer[i] ^= m_ladKey[(i + iXorOffset) % LAD_XOR_LENGTH];
   }

   // Copy the sample times
   
   REFERENCE_TIME TimeStart, TimeEnd;
   if (NOERROR == pSource->GetTime(&TimeStart, &TimeEnd)) {
      pDest->SetTime(&TimeStart, &TimeEnd);
   }

   LONGLONG MediaStart, MediaEnd;
   if (pSource->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR) {
      pDest->SetMediaTime(&MediaStart,&MediaEnd);
   }

   // Copy the media type
   AM_MEDIA_TYPE *pMediaType;
   pSource->GetMediaType(&pMediaType);
   pDest->SetMediaType(pMediaType);
   DeleteMediaType(pMediaType);
   
   // Copy the actual data length
   long lDataLength = pSource->GetActualDataLength();
   pDest->SetActualDataLength(lDataLength);
   return NOERROR;
   
} // Copy


//
// CheckInputType
//
HRESULT CWavDestFilter::CheckInputType(const CMediaType* mtIn)
{
   if(mtIn->formattype == FORMAT_WaveFormatEx) {
      return S_OK;
   }
   return S_FALSE;
}

//
// GetMediaType
//
HRESULT CWavDestFilter::GetMediaType(int iPosition, CMediaType *pMediaType)
{
   ASSERT(iPosition == 0 || iPosition == 1);
   if(iPosition == 0) 
   {
      pMediaType->SetType(&MEDIATYPE_Stream);
      if (!m_bCreateLadFile)
         pMediaType->SetSubtype(&MEDIASUBTYPE_WAVE);
      else
         pMediaType->SetSubtype(&MEDIASUBTYPE_LAD);
      return S_OK;
   }
   return VFW_S_NO_MORE_ITEMS;
}

//
// DecideBufferSize
//
// Tell the output pin's allocator what size buffers we
// require. Can only do this when the input is connected
//
HRESULT CWavDestFilter::DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties)
{
   // Is the input pin connected
   if (m_pInput->IsConnected() == FALSE) {
      return E_UNEXPECTED;
   }
   
   ASSERT(pAlloc);
   ASSERT(pProperties);
   HRESULT hr = NOERROR;
   
   pProperties->cBuffers = 1;
   pProperties->cbAlign = 1;
   
   // Get input pin's allocator size and use that
   ALLOCATOR_PROPERTIES InProps;
   IMemAllocator * pInAlloc = NULL;
   hr = m_pInput->GetAllocator(&pInAlloc);
   if (SUCCEEDED (hr))
   {
      hr = pInAlloc->GetProperties (&InProps);
      if (SUCCEEDED (hr))
      {
         pProperties->cbBuffer = InProps.cbBuffer;
      }
      pInAlloc->Release();
   }
   
   if (FAILED(hr))
      return hr;
   
   ASSERT(pProperties->cbBuffer);
   
   // Ask the allocator to reserve us some sample memory, NOTE the function
   // can succeed (that is return NOERROR) but still not have allocated the
   // memory that we requested, so we must check we got whatever we wanted
   
   ALLOCATOR_PROPERTIES Actual;
   hr = pAlloc->SetProperties(pProperties,&Actual);
   if (FAILED(hr)) {
      return hr;
   }
   
   ASSERT( Actual.cBuffers == 1 );
   
   if (pProperties->cBuffers > Actual.cBuffers ||
      pProperties->cbBuffer > Actual.cbBuffer) {
      return E_FAIL;
   }
   return NOERROR;
   
} // DecideBufferSize


//
// StartStreaming
//
// Compute the header size to allow space for us to write it at the end.
//
// 00000000    RIFF (00568BFE) 'WAVE'
// 0000000C        fmt  (00000010)
// 00000024        data (00568700)
// 0056872C
//
HRESULT CWavDestFilter::StartStreaming()
{
   // leave space for the header
   m_cbHeader = sizeof(RIFFLIST) + 
      sizeof(RIFFCHUNK) + 
      m_pInput->CurrentMediaType().FormatLength() + 
      sizeof(RIFFCHUNK);
   // if we're writing LAD files, leave another 1024 bytes
   if (m_bCreateLadFile)
      m_cbHeader += LAD_HDR_LENGTH;
   
   m_cbWavData = 0;
   m_rtCurrentPos = 0;
   m_dwBytesPerSecond = 0;

   if (m_pInput != NULL)
   {
      WAVEFORMATEX *pWfe = (WAVEFORMATEX *)m_pInput->CurrentMediaType().Format();
      m_dwBytesPerSecond = pWfe->nAvgBytesPerSec;
   }
   

   return S_OK;
}

// helper method
HRESULT CWavDestFilter::FillMissingBytes(IStream *pStream, UINT nMissing)
{
   if (pStream == NULL)
      return E_POINTER;

   UINT nArraySize = LAD_XOR_LENGTH;
   if (nArraySize > nMissing)
      nArraySize = nMissing;
 
   BYTE aEmpty[LAD_XOR_LENGTH];

   if (m_bCreateLadFile)
   {
      int iXorOffset = (m_cbWavData + m_cbHeader - LAD_HDR_LENGTH + LAD_INFO_LENGTH) % LAD_XOR_LENGTH;
      for (int i=0; i<LAD_XOR_LENGTH; ++i)
         aEmpty[i] = m_ladKey[(i + iXorOffset) % LAD_XOR_LENGTH];
      // fill the buffer with the xor key; this will be decoded as 0 upon replay
   }
   else
      ZeroMemory(&aEmpty, nArraySize);
   
   LARGE_INTEGER li;
   ZeroMemory(&li, sizeof li);
   HRESULT hrWrite = pStream->Seek(li, STREAM_SEEK_END, 0);
   
   if (SUCCEEDED(hrWrite))
   {
      UINT nWrittenTotal = 0;
      
      while (nWrittenTotal < nMissing && SUCCEEDED(hrWrite))
      {
         UINT nLeft = nMissing - nWrittenTotal;
         UINT nToWrite = nLeft > nArraySize ? nArraySize : nLeft;
         
         ULONG nWritten = 0;
         hrWrite = pStream->Write(&aEmpty, nToWrite, &nWritten);
         if (SUCCEEDED(hrWrite))
            nWrittenTotal += nWritten;
      }
   }
  
   return hrWrite;
}

//
// StopStreaming
//
// Write out the header
//
HRESULT CWavDestFilter::StopStreaming()
{
   IStream *pStream;
   if (m_pOutput->IsConnected() == FALSE)
      return E_FAIL;
   
   IPin * pDwnstrmInputPin = m_pOutput->GetConnected();
   
   if (!pDwnstrmInputPin)
      return E_FAIL;

   if (m_pInput->CurrentMediaType().majortype != MEDIATYPE_Audio)
      return E_UNEXPECTED;
   
   HRESULT hr = ((IMemInputPin *) pDwnstrmInputPin)->QueryInterface(IID_IStream, (void **)&pStream);
   if(SUCCEEDED(hr))
   {
      
      if (m_nExpectedLengthMs != 0)
      {
         WAVEFORMATEX *pWfe = (WAVEFORMATEX *)m_pInput->CurrentMediaType().Format();
         
         UINT nExpectedBytes = (UINT)(((m_nExpectedLengthMs + 0.5) / 1000.0) * pWfe->nAvgBytesPerSec);
         // "0.5": otherwise it will be (m_nExpectedLengthMs - 1).99 ms long

         if (m_cbWavData < nExpectedBytes)
         {
            // Note:
            // A small hack to force the output audio file to be the expected size.
            // It is normally a bit shorter because of rounding errors and framing in DES.
            // However the exact length is needed for proper replay with 
            // stop marks or areas at the very end.
            
            UINT nMissing = nExpectedBytes - m_cbWavData;

            HRESULT hrWrite = FillMissingBytes(pStream, nMissing);
            
            if (SUCCEEDED(hrWrite))
               m_cbWavData += nMissing; // important for length information in the WAV header
         }

         // TODO if there is an error somewhere above?
      }


      BYTE *pb = (BYTE *)_alloca(m_cbHeader);

      // if we want to write LAD files, we need to offset
      // the WAV header by 1024 bytes (sizeof BYTE == 1)
      int wavHeaderOffset = m_bCreateLadFile ? LAD_HDR_LENGTH : 0;

      RIFFLIST *pRiffWave = (RIFFLIST *)(pb + wavHeaderOffset);
      RIFFCHUNK *pRiffFmt = (RIFFCHUNK *)(pRiffWave + 1);
      RIFFCHUNK *pRiffData = (RIFFCHUNK *)(((BYTE *)(pRiffFmt + 1)) +  m_pInput->CurrentMediaType().FormatLength());;
      
      pRiffData->fcc = FCC('data');
      pRiffData->cb = m_cbWavData;
      
      pRiffFmt->fcc = FCC('fmt ');
      pRiffFmt->cb = m_pInput->CurrentMediaType().FormatLength();
      CopyMemory(pRiffFmt + 1, m_pInput->CurrentMediaType().Format(), pRiffFmt->cb);
      
      pRiffWave->fcc = FCC('RIFF');
      pRiffWave->cb = m_cbWavData + m_cbHeader - sizeof(RIFFCHUNK);
      pRiffWave->fccListType = FCC('WAVE');
      
      LARGE_INTEGER li;
      ZeroMemory(&li, sizeof(li));

      // are we writing LAD files?
      if (m_bCreateLadFile)
      {
         // Yes, fill in the LAD header
         pb[0] = 'L';
         pb[1] = 'A';
         pb[2] = 'D';
         pb[3] = ' ';

         // The LAD xor key
         int i = 0;
         for (i=0; i<LAD_XOR_LENGTH; ++i)
            pb[i+4] = m_ladKey[i];

         // The LAD info, xor'ed
         for (i=0; i<LAD_INFO_LENGTH; ++i)
            pb[i+LAD_XOR_LENGTH+4] = ((BYTE) m_ladInfo[i]) ^ m_ladKey[i];

         // Now xor the header, too
         int xorLen = m_cbHeader - LAD_HDR_LENGTH;
         BYTE *pbWav = pb + LAD_HDR_LENGTH;
         for (i=0; i<xorLen; ++i)
            pbWav[i] ^= m_ladKey[(LAD_INFO_LENGTH + i) % LAD_XOR_LENGTH];
      }
      
      hr = pStream->Seek(li, STREAM_SEEK_SET, 0);
      if(SUCCEEDED(hr)) {
         hr = pStream->Write(pb, m_cbHeader, 0);
      }
      pStream->Release();
   }
   
   return hr;
}

HRESULT CWavDestFilter::SetLadInfo(const char *szLadInfo)
{
   if (szLadInfo)
   {
      strncpy(m_ladInfo, szLadInfo, LAD_INFO_LENGTH);
      int ladInfoLen = strlen(m_ladInfo);
      for (int i=ladInfoLen + 1; i<LAD_INFO_LENGTH; ++i)
         m_ladInfo[i] = (char) (rand() % 255);
      m_bCreateLadFile = true;
   }
   else
   {
      m_bCreateLadFile = false;
   }

   return S_OK;
}

HRESULT CWavDestFilter::GetCurrentPosition(REFERENCE_TIME *prtTarget)
{
   if (prtTarget == NULL)
      return E_POINTER;

   *prtTarget = m_rtCurrentPos;

   return S_OK;
}


//
// CWavDestOutputPin::CWavDestOutputPin 
//
CWavDestOutputPin::CWavDestOutputPin(CTransformFilter *pFilter, HRESULT * phr) :
CTransformOutputPin(NAME("WavDest output pin"), pFilter, phr, L"Out")
{
}


//
// CWavDestOutputPin::EnumMediaTypes
//
STDMETHODIMP CWavDestOutputPin::EnumMediaTypes( IEnumMediaTypes **ppEnum )
{
   return CBaseOutputPin::EnumMediaTypes(ppEnum);
}

//
// CWavDestOutputPin::CheckMediaType
//
// Make sure it's our default type:
// MEDIATYPE_Stream and MEDIASUBTYPE_WAVE or _LAD
//
HRESULT CWavDestOutputPin::CheckMediaType(const CMediaType* pmt)
{
   if(pmt->majortype == MEDIATYPE_Stream && 
      (pmt->subtype == MEDIASUBTYPE_WAVE || pmt->subtype == MEDIASUBTYPE_LAD))
      return S_OK;
   else
      return S_FALSE;
}

/*
//
// Filter registration functions
//
STDAPI DllRegisterServer()
{
   return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
   return AMovieDllRegisterServer2(FALSE);
}
//*/