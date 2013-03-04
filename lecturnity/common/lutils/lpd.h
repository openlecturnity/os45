#ifndef __LPD_H_F26B9A1A_E85B_4345_AAB9_2FF35206EC69
#define __LPD_H_F26B9A1A_E85B_4345_AAB9_2FF35206EC69

#ifndef __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B
#pragma message( "Do not include \"" __FILE__ "\", include \"lutils.h\"")
#endif __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B

#include <tchar.h>

#include "lio.h"
#include "lprogress.h"


#define JAVA_OBJECT_FILE_ID 0xaced0005

#define JAVA_ID_BOOLEAN 0x5a
#define JAVA_ID_BYTE 0x42
#define JAVA_ID_SHORT 0x53
#define JAVA_ID_INT 0x49
#define JAVA_ID_LONG 0x4a
#define JAVA_ID_FLOAT 0x44
#define JAVA_ID_DOUBLE 0x46
#define JAVA_ID_NOTPRIMITIVE 0x4c
#define JAVA_ID_STRING 0x74
#define JAVA_ID_NULL 0x70
#define JAVA_ID_SAME_TYPE 0x71

#define JAVA_STREAM_ID_ARRAY 0x7572
#define JAVA_STREAM_ID_CLASS 0x7372
#define JAVA_STREAM_ID_VALUES 0x7870
#define JAVA_STREAM_ID_SEPARATOR 0x7371

#define E_PARSING _HRESULT_TYPEDEF_(0x80050901L)


class JavaVariable
{
public:
   USHORT m_nType;
   LString m_lstrName;

   /**
    * Only one of the following two is set; according to the type.
    */
   LONGLONG m_lValue;
   LString m_lstrValue;

   JavaVariable();
   ~JavaVariable();

   /**
    * Returns the determined type.
    */
   USHORT ReadTypeAndName(LIo::InputStream *pStream, JavaVariable *pPrevious);

   /**
    * Currently only supports boolean, int and String!
    */
   HRESULT WriteTypeAndName(LIo::DataOutputStream *pStream);

   /**
    * Currently only supports boolean, int and String!
    */
   HRESULT ReadValue(LIo::InputStream *pStream);

   /**
    * Currently only supports boolean, int and String!
    */
   HRESULT WriteValue(LIo::DataOutputStream *pStream);
};


class JavaClassInstance
{
public:
   JavaClassInstance();
   ~JavaClassInstance();

   virtual HRESULT InitAndReadValues(JavaVariable *paVariables, UINT nVariableCount, LIo::InputStream *pStream);
   UINT GetVariableCount();

   /**
    * Writes the variable definitions (types and names). This is
    * usually only called once for the header.
    */
   HRESULT WriteVariables(LIo::DataOutputStream *pStream);
   
   /**
    * Writes the variables' values in the order as defined by WriteVariables().
    */
   virtual HRESULT WriteValues(LIo::DataOutputStream *pStream);

protected:
   UINT m_nVariableCount;
   JavaVariable *m_paVariables;
};

class LpdFileStruct : public JavaClassInstance
{
public:
   LpdFileStruct();
   ~LpdFileStruct();

   virtual HRESULT InitAndReadValues(JavaVariable *paVariables, UINT nVariableCount, LIo::InputStream *pStream);
   virtual HRESULT WriteValues(LIo::DataOutputStream *pStream);

   LString m_lsFileName;
   bool m_bIsCompressed;
   UINT m_nOffset;
   UINT m_nRealSize;
   UINT m_nCompressedSize;
   
private:

};


class LpdHandler
{
public:
   LpdHandler();
   ~LpdHandler();

   HRESULT Init(const _TCHAR *tszDocument, LProgressListener *pListener = NULL);

   HRESULT CopyPackedFiles(const _TCHAR *tszPath);
   HRESULT RecreateLpd(const _TCHAR *tszPathAndName, BYTE btNewType, bool *pbTypesDifferent);

   /**
    * If there is an action underway it will be cancelled (or signalled). 
    * Otherwise nothing is done.
    * Cancel is finished if the used method (above) returns. 
    * No files are deleted.
    */
   void RequestCancelAction();

private:

   HRESULT ParseHeader(LIo::InputStream *pStreamIn, UINT *pnHeaderLength);
   HRESULT RewriteBaseHeader(LIo::OutputStream *pOutput);
   HRESULT RewriteValuesAndFiles(LIo::OutputStream *pOutput, BYTE btNewType);
   
   /**
    * Puts the (packed) _internal_versiontypeinfo file with the new type into
    * the provided buffer. If the types do not differ it does nothing; the buffer
    * contains the old packed data.
    * Doesn't take into account that the file might
    * not be compressed (very unlikely).
    */
   HRESULT RepackTypeInfo(LBuffer *pResultBuffer, BYTE btNewType, bool *pbTypesDifferent);
   HRESULT Copy(LIo::InputStream *pLpdStream, LIo::OutputStream *pOutput, UINT nLength);
   HRESULT DecompressData(LBuffer *pSourceBuffer, LBuffer *pTargetBuffer, UINT *pnUncompressedLength);
   HRESULT CompressData(LBuffer *pSourceBuffer, LBuffer *pTargetBuffer, UINT *pnCompressedLength);

   LpdFileStruct *m_paFiles;
   UINT           m_nFileCount;
   UINT           m_nTotalBytes;
   LString        m_lsFileName;
   BYTE           m_nConfigByte;
   UINT           m_nFilesStart;
   LBuffer       *m_pKeyBuffer;

   bool           m_bMethodActive;
   bool           m_bCancelRequested;
   UINT           m_nCopiedBytes;
   LProgressListener *m_pProgressListener;

};

#endif // __LPD_H_F26B9A1A_E85B_4345_AAB9_2FF35206EC69