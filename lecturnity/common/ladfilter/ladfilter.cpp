#include "StdAfx.h"

//
// ladfilter.cpp
//
// LadParser

#include "ladfilter.h"

//
// Constructor
//
// If any part of construction fails *phr should be set to 
// a failure code.
CLadParser::CLadParser(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
   : CBaseFilter(tszName, punk, &m_FilterLock, *sudLadFilter.clsID)
   , m_FilterLock(), m_addRefs(0), m_releaseRefs(0)
{
   m_pInputPin = NULL;
   m_pOutputPin = NULL;
}


//
// Destructor
//
CLadParser::~CLadParser() 
{
   //MessageBox(NULL, "Destroy CLadParser", NULL, MB_OK);
   if (m_pInputPin)
      delete m_pInputPin;
   m_pInputPin = NULL;
   if (m_pOutputPin)
      delete m_pOutputPin;
   m_pOutputPin = NULL;
}

//
// CreateInstance
//
// Provide the way for the COM support routines in <streams.h>
// to create a CLadParser object
CUnknown * WINAPI CLadParser::CreateInstance(LPUNKNOWN punk, HRESULT *phr) {
   
   CUnknown *pNewObject = new CLadParser(NAME("LAD Parser"), punk, phr );
   if (pNewObject == NULL) {
      *phr = E_OUTOFMEMORY;
   }
   
   return pNewObject;
}

HRESULT CLadParser::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
   if (riid == IID_ILadParser)
      return GetInterface((ILadParser *) this, ppv);
   return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
}

//
// GetPinCount
//
// return the number of pins this filter has
int CLadParser::GetPinCount() 
{
   return sudLadFilter.nPins;
}


//
// GetPin
//
// return pointer to a pin.
CBasePin* CLadParser::GetPin(int n) {
   
   // TODO: Create each pin, and return
   // a pointer to the requested pin.
   if (n == 0) // Input Pin
   {
      if (m_pInputPin == NULL)
      {
         HRESULT hr = S_OK;
         m_pInputPin = new CLadParserInputPin(
            this,
            &m_FilterLock,
            &hr,
            L"LAD Parser Input Pin");
      }

      return m_pInputPin;
   }
   else if (n == 1)
   {
      if (m_pOutputPin == NULL)
      {
         HRESULT hr = S_OK;
         m_pOutputPin = new CLadParserOutputPin(
            this,
            &m_FilterLock,
            &hr,
            L"LAD Parser Output Pin");
      }

      return m_pOutputPin;
   }

   return NULL;
}

HRESULT CLadParser::QueryLadInfo(char *szLadInfo, int *nCount)
{
   if (m_pInputPin == NULL)
      return VFW_E_NOT_CONNECTED;
   if (FALSE == m_pInputPin->IsConnected())
      return VFW_E_NOT_CONNECTED;
   if (nCount == NULL)
      return E_POINTER;

   int nSize = strlen(m_pInputPin->m_szTypeInfo);
   if (nSize > 128)
      nSize = 128;

   *nCount = nSize;
   if (szLadInfo)
      strncpy(szLadInfo, m_pInputPin->m_szTypeInfo, 128);

   return S_OK;
}

HRESULT CLadParser::QueryLadKey(BYTE *pLadKey)
{
   if (m_pInputPin == NULL)
      return VFW_E_NOT_CONNECTED;
   if (FALSE == m_pInputPin->IsConnected())
      return VFW_E_NOT_CONNECTED;
   if (pLadKey == NULL)
      return E_POINTER;

   memcpy(pLadKey, m_pInputPin->m_szLadKey, 892);

   return S_OK;
}

