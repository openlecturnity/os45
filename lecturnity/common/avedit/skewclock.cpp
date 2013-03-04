#include "StdAfx.h"
#include "skewclock.h"

CSkewReferenceClock::CSkewReferenceClock(IReferenceClock *pRC, REFERENCE_TIME rtOffset)
{
#ifdef _DEBUGFILE
   DebugMsg("CSkewReferenceClock()\n");
#endif

   m_pRC      = pRC;
   if (m_pRC)
      m_pRC->AddRef();
   m_rtOffset       = rtOffset;
   m_nRefCounter    = 0;
   m_rtSkew         = 0;
   m_rtLastReturned = 0;
}

CSkewReferenceClock::~CSkewReferenceClock()
{
   if (m_pRC)
      m_pRC->Release();
   m_pRC = NULL;
}

HRESULT CSkewReferenceClock::GetTime(REFERENCE_TIME *pTime)
{
   HRESULT hr = E_NOTIMPL;
   if (m_pRC)
      hr = m_pRC->GetTime(pTime);

   if (SUCCEEDED(hr))
   {
      *pTime -= (m_rtOffset + m_rtSkew);
      if (*pTime < m_rtLastReturned)
         *pTime = m_rtLastReturned;
   }
   return hr;
}

HRESULT CSkewReferenceClock::AdviseTime(REFERENCE_TIME rtBaseTime,
                                      REFERENCE_TIME rtStreamTime,
                                      HEVENT hEvent,
                                      DWORD_PTR *pdwAdviseCookie)
{
   if (m_pRC)
      return m_pRC->AdviseTime(rtBaseTime + m_rtOffset + m_rtSkew, rtStreamTime, hEvent, pdwAdviseCookie);
   else
      return E_NOTIMPL;
}

HRESULT CSkewReferenceClock::AdvisePeriodic(REFERENCE_TIME rtStartTime,
                                          REFERENCE_TIME rtPeriodic,
                                          HSEMAPHORE hSemaphore,
                                          DWORD_PTR *pdwAdviseCookie)
{
   if (m_pRC)
      return m_pRC->AdvisePeriodic(rtStartTime + m_rtOffset + m_rtSkew, rtPeriodic, hSemaphore, pdwAdviseCookie);
   else
      return E_NOTIMPL;
}

HRESULT CSkewReferenceClock::Unadvise(DWORD_PTR dwAdviseCookie)
{
   if (m_pRC)
      return m_pRC->Unadvise(dwAdviseCookie);
   else
      return E_NOTIMPL;
}

ULONG CSkewReferenceClock::AddRef()
{
   m_nRefCounter += 1;
   return m_nRefCounter;
}

ULONG CSkewReferenceClock::Release()
{
   // Achtung: Hier wird eine temporäre Variable
   // angelegt, damit (im Falle, dass das Objekt
   // zerstört wird) nicht eine Zugriffsverletzung
   // beim Zugriff auf die (Member-)Variable
   // m_nRefCounter auftritt:
   ULONG tempRefC = --m_nRefCounter;
   if (m_nRefCounter <= 0)
      delete this;

   // Die temporäre Variable zurückgeben!
   return tempRefC;
}

HRESULT CSkewReferenceClock::QueryInterface(REFIID refIid, LPVOID *pDest)
{
   HRESULT hr = S_OK;

   if (refIid == IID_IUnknown)
   {
      *pDest = (IUnknown *) this;
   }
   else if (refIid == IID_IReferenceClock)
   {
      *pDest = (IReferenceClock *) this;
   }
   else
      hr = E_NOINTERFACE;

   if (SUCCEEDED(hr))
      AddRef();

   return hr;
}
