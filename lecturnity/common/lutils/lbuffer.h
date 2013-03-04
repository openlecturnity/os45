#ifndef __LBUFFER_H_27C3C299_173B_4f00_9B2A_FC5B3899CEC1
#define __LBUFFER_H_27C3C299_173B_4f00_9B2A_FC5B3899CEC1

#ifndef __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B
#pragma message( "Do not include \"" __FILE__ "\", include \"lutils.h\"")
#endif __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B

class LBuffer
{
public:
   LBuffer();
   LBuffer(int nInitialSize);
   LBuffer(LBuffer *pCopyBuffer);
   ~LBuffer();

   void Release();
   void SetSize(int nNewSize);
   void SetAll(BYTE btValue);
   int  GetSize() { return m_nBufferSize; }

   /**
    * If you request the buffer with "GetBuffer()" and alter its
    * contents the LBuffer doesn't know about it.
    * Use this method to update the internal content counter ("AddedSize").
    * 
    * Caution: No error checking can and will be done!
    */
   void UpdateAddedSize(int nAdditionalSize);
   void SetAddedSize(int nAddedSize);
   void AddBuffer(void *pBuffer, int nSize);
   int  GetAddedSize() { return m_nAddedSize; }
   virtual void SetBufferPointer(int nBufferPointer);
   virtual void ResetBufferPointer() { SetBufferPointer(0); }

   void FillRandomly();

   inline void *GetBuffer() { return m_pBuffer; }
   inline BYTE* GetByteAccess() { return (BYTE *)m_pBuffer; }
   inline operator LPVOID () { return m_pBuffer; }

   void SetResizeStepSize(int nResizeStep = 16384) { m_nStepSize = nResizeStep; }

protected:
   void Init(int nInitialSize);

   void *m_pBuffer;
   int   m_nBufferSize;
   int   m_nAddedSize;
   int   m_nStepSize;
   int   m_nBufferPointer;
};

#endif // __LBUFFER_H_27C3C299_173B_4f00_9B2A_FC5B3899CEC1
