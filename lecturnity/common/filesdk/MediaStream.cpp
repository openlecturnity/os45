#include "stdafx.h"
#include "mediastream.h"

CMediaStream::CMediaStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_nRefCounter = 0;
}

void CMediaStream::AddReference()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_nRefCounter++;
}


int CMediaStream::ReleaseReference()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int tempRefC = --m_nRefCounter;

   if (tempRefC <= 0)
      delete this;

   return tempRefC;
}
