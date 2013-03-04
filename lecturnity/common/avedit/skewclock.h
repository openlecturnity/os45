#ifndef __SKEWCLOCK_H__
#define __SKEWCLOCK_H__

//#include "StdAfx.h"

class CSkewReferenceClock : public IReferenceClock
{
public:
   CSkewReferenceClock(IReferenceClock *pRC, REFERENCE_TIME rtOffset);
   ~CSkewReferenceClock();

   HRESULT STDMETHODCALLTYPE GetTime(REFERENCE_TIME *pTime);
   HRESULT STDMETHODCALLTYPE AdviseTime(
      REFERENCE_TIME rtBaseTime,
      REFERENCE_TIME rtStreamTime,
      HEVENT hEvent,
      DWORD_PTR *pdwAdviseCookie);
   HRESULT STDMETHODCALLTYPE AdvisePeriodic(
      REFERENCE_TIME rtStartTime,
      REFERENCE_TIME rtPeriodTime,
      HSEMAPHORE hSemaphore,
      DWORD_PTR *pdwAdviseCookie);
   HRESULT STDMETHODCALLTYPE Unadvise(DWORD_PTR dwAdviseCookie);

   // IUnknown:
   ULONG STDMETHODCALLTYPE AddRef();
   ULONG STDMETHODCALLTYPE Release();

   HRESULT STDMETHODCALLTYPE QueryInterface(
      REFIID refIid,
      LPVOID *pDest);

   void SetSkew(REFERENCE_TIME rtSkew) { m_rtSkew += rtSkew; }
   REFERENCE_TIME GetSkew() { return m_rtSkew; }
   REFERENCE_TIME GetOffset() { return m_rtOffset; }

private:
   IReferenceClock *m_pRC;
   REFERENCE_TIME   m_rtOffset;
   REFERENCE_TIME   m_rtSkew;
   REFERENCE_TIME   m_rtLastReturned;
   ULONG            m_nRefCounter;
};

#endif