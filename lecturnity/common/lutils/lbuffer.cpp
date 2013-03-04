#include "StdAfx.h"
#include "lutils.h"

#include <mmsystem.h>

LBuffer::LBuffer()
{
   Init(16384);
}

LBuffer::LBuffer(int nInitialSize)
{
   Init(nInitialSize);
}

LBuffer::LBuffer(LBuffer *pCopyBuffer)
{
   Init(pCopyBuffer->GetSize());
   this->AddBuffer(pCopyBuffer->GetBuffer(), pCopyBuffer->GetAddedSize());
}
   

LBuffer::~LBuffer()
{
   Release();
}

void LBuffer::Init(int nInitialSize)
{
   m_pBuffer = new char[nInitialSize];
   m_nBufferSize = nInitialSize;

   m_nStepSize = 16384;
   m_nBufferPointer = 0;
   m_nAddedSize = 0;
}

void LBuffer::Release()
{
   if (m_pBuffer)
      delete m_pBuffer;
   m_pBuffer = NULL;
   m_nBufferSize = 0;
   m_nBufferPointer = 0;
   m_nAddedSize = 0;
}

void LBuffer::SetSize(int nNewSize)
{
   void *pNew = new char[nNewSize];
   int minSize = m_nBufferSize < nNewSize ? m_nBufferSize : nNewSize;
   if (m_pBuffer)
   {
      memcpy(pNew, m_pBuffer, minSize);
   }

   int nBufferPointer = m_nBufferPointer;

   Release();
   m_pBuffer = pNew;
   m_nBufferSize = nNewSize;

   if (nBufferPointer > nNewSize)
      m_nBufferPointer = nNewSize;
   else
      m_nBufferPointer = nBufferPointer;
}

void LBuffer::SetBufferPointer(int nBufferPointer)
{
   if (nBufferPointer > m_nBufferSize)
   {
      SetSize(nBufferPointer);
   }

   m_nBufferPointer = nBufferPointer;
}

void LBuffer::UpdateAddedSize(int nAdditionalSize)
{
   // no error checking at all!

   m_nAddedSize += nAdditionalSize;
   _ASSERT(m_nAddedSize >= 0);
}

void LBuffer::SetAddedSize(int nAddedSize)
{
   m_nAddedSize = nAddedSize;
}

void LBuffer::AddBuffer(void *pBuffer, int nSize)
{
   // Does the buffer fit into the current buffer?
   if (m_nBufferSize >= nSize + m_nBufferPointer)
   {
      // Yes, copy it
      memcpy((void *) ((char *) m_pBuffer + m_nBufferPointer), pBuffer, nSize);
      m_nBufferPointer += nSize;
      m_nAddedSize += nSize;
   }
   else
   {
      // No, enlarge buffer
      int nMissingBytes = nSize - (m_nBufferSize - m_nBufferPointer);

      // Enlarge by multiples of m_nStepSize
      int nEnlargeTimes = nMissingBytes / m_nStepSize + 1;
      int nEnlargeBytes = m_nStepSize * nEnlargeTimes;

      SetSize(m_nBufferSize + nEnlargeBytes);

      // Now add the buffer
      AddBuffer(pBuffer, nSize);
   }
}


void LBuffer::SetAll(BYTE btValue)
{
   memset(m_pBuffer, btValue, m_nBufferSize);
}

void LBuffer::FillRandomly()
{
   ::srand(timeGetTime());

   BYTE *pBytes = (BYTE *)m_pBuffer;

   for (int i=0; i<m_nBufferSize; ++i)
      pBytes[i] = (BYTE)(rand() >> 1);

   m_nAddedSize = m_nBufferSize;
}