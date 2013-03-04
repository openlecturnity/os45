#ifndef __LIO_H__8CD52941_8273_4531_94DB_26263DF0AFCF
#define __LIO_H__8CD52941_8273_4531_94DB_26263DF0AFCF

#ifndef __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B
#pragma message( "Do not include \"" __FILE__ "\", include \"lutils.h\"")
#endif __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B

#include "lbuffer.h"
#include "lfile.h"

namespace LIo
{
   /**
    * Counts the subdirs in a given path. E.g., the path
    * C:\Dokumente und Einstellungen\Martin\Desktop\test.txt
    * would return 3.
    */
   int CountSubDirs(const _TCHAR *szPath);

   /**
    * Retrieves a subdir with a given number in a path, e.g. with
    * C:\Dokumente und Einstellungen\Martin\Desktop\test.txt
    * GetSubDir would return "Martin" for the index 1, and
    * "Desktop" for the index 2. I.e., index is zero-based.
    */
   bool GetSubDir(const _TCHAR *szPath, int nIndex, _TCHAR *szSubDir);

   /**
    * Retrieves the directory of a file or the parent directory of a directory.
    */
   bool GetDir(const _TCHAR *tszPath,  _TCHAR *tszParentDir, int *piLastSlashPos=NULL);

   /**
   * Writes the target path szTarget into szRelative, relative to
   * the source path szSource. E.g., Giving C:\TEMP\test.txt as
   * szSource and C:\Documents\crap.txt as szSource would render
   * ..\Documents\crap.txt as szRelative. Both szSource and szTarget
   * must be given as fully qualified paths, including a drive letter.
   * If the drive letters do not match, the full szTarget is copied
   * into szRelative. This function may successfully be compiled
   * with both _MBCS and _UNICODE flags set.
   */
   bool MakeRelativePath(_TCHAR *szRelative, const _TCHAR *szSource, const _TCHAR *szTarget);

   /**
    * The exact reverse of MakeRelativePath.
    */
   bool MakeAbsolutePath(_TCHAR *szAbsolute, const _TCHAR *szSource, const _TCHAR *szRelTarget);

   /**
    * Copies szSource to szTarget.
    */
   bool CopyFile(const _TCHAR *szSource, const _TCHAR *szTarget);


   /**
    * Copies directory szSource to szTarget.
    */
   bool CopyDir(const _TCHAR *szSource, const _TCHAR *szTarget, bool bDoNotRecurse=false);

   /**
    * Creates the new directories szDirs in directory szBase,
    * if not already exist.
    */
   bool CreateDirs(const _TCHAR *szBase, const _TCHAR *szDirs);

   /**
    * Creates a temporary directory. Puts the base path into
    * tszPath and the directory name in tszDir. The directory name
    * is prefixed with tszPrefix, e.g. _T("SCO") would render
    * a directory "C:\TEMP\SCO1234.tmp".
    */
   bool CreateTempDir(const _TCHAR *tszPrefix, _TCHAR *tszPath, _TCHAR *tszDir);

   /**
    * Recursively deletes a directory and all of it contents. Use
    * with caution.
    */
   bool DeleteDir(const _TCHAR *tszDir);

   /**
    * Deletes the file given in tszFile.
    */
   bool DeleteFile(const _TCHAR *tszFile);

   /**
    * Creates a link (shortcut) from tszSource to tszTarget, using
    * tszComment as a comment (if wanted).
    */
   //bool CreateLink(const _TCHAR *tszSource, const _TCHAR *tszTarget, const _TCHAR *tszComment=NULL);

   /**
    * Returns the file size as a 64-bit signed number, or -1 if it failed.
    */
   __int64 GetFileSize(const _TCHAR *tszFileName);

   /**
    * Returns the free disk space for the given path, or -1 if it failed.
    */
   __int64 GetDiskSpace(const _TCHAR *tszPath);


   class MessagePipe {
   public:
       MessagePipe();
       virtual ~MessagePipe();

       /** Initializes pipe.
        * tszPipeId - name of the pipe.
        * bCreate - True to create pipe, false to connect to a pipe.
        * dwDesiredAccess: write access only (Bugfix 5904: No read/write access with GENERIC_READ | GENERIC_WRITE)
        *
        * Return S_OK if completed successfully. An error code otherwise.
        */
       HRESULT Init(_TCHAR *tszPipeId, bool bCreate, DWORD dwDesiredAccess = GENERIC_WRITE);

       // Is blocking until there is something to read (if the pipe exists)
       HRESULT ReadMessage(_TCHAR *tszMessage, int iBufferLength);
       HRESULT ReadMessage(char *szMessage, int iBufferLength, DWORD &dwRead, bool bReconnect = true);
       // Is blocking until there is something to read (if the pipe exists) or a timeout is exceeded
       HRESULT PeekMessage(char *szMessage, int iBufferLength, DWORD &dwRead);

       // Is currently non-blocking; pipe must exist
       HRESULT WriteMessage(const _TCHAR *tszMessage);
       HRESULT WriteCharMessage(const char *szMessage);

   private: 
       HANDLE m_hPipe;
   };

   /**
    * Serves as generic base class for all IO input operations.
    */
   class InputStream
   {
   public:
      InputStream();
      
      virtual HRESULT Init(InputStream *pBaseInputStream);

      virtual HRESULT ReadByte(UINT *pTarget);
      virtual HRESULT ReadBytes(LBuffer *pBuffer, UINT nOff, UINT nLen, UINT *pnRead) = 0;

      virtual HRESULT Skip(UINT nByteCount) = 0;

   protected:
      InputStream *m_pBaseStream;
    
   };


   
   /**
    * Reads and decrypts bytes with the help of a provided XOR key buffer.
    */
   class XorInputStream : public InputStream
   {
   public:
      XorInputStream();
      ~XorInputStream();

      virtual HRESULT Init(InputStream *pBaseInputStream, LBuffer *pKeyBuffer);
      
      virtual HRESULT ReadBytes(LBuffer *pBuffer, UINT nOff, UINT nLen, UINT *pnRead);

      virtual HRESULT Skip(UINT nByteCount);

      /**
       * Sets the pointer in the key array to the beginning.
       * As if the input stream was just started.
       */
      HRESULT ResetKeyPointer();

   private:
      LBuffer *m_pKeyBuffer;
      DWORD    m_nBufferPointer;
   };


   class FileInputStream : public InputStream
   {
   public:
      FileInputStream();
      ~FileInputStream();

      virtual HRESULT Init(LFile *pFile);

      virtual HRESULT ReadByte(UINT *pnTarget);
      virtual HRESULT ReadBytes(LBuffer *pBuffer, UINT nOff, UINT nLen, UINT *pnRead);
      
      virtual HRESULT Skip(UINT nByteCount);

   private:
      LFile *m_pFile;
      bool   m_bDidOpen;
   };

   class BufferedInputStream : public InputStream
   {
   public:
      BufferedInputStream();
      ~BufferedInputStream();

      virtual HRESULT Init(InputStream *pBaseInputStream, UINT nBufferSize = 4096);
      
      virtual HRESULT ReadByte(UINT *pTarget);
      virtual HRESULT ReadBytes(LBuffer *pBuffer, UINT nOff, UINT nLen, UINT *pnRead);

      virtual HRESULT Skip(UINT nByteCount);

   private:
      /**
       * Returns if something could be filled into the buffer.
       */ 
      HRESULT FillBuffer();

      LBuffer *m_plBuffer; // only for satisfying underlying streams
      BYTE    *m_pBuffer;  // this is the structure internally used (part of the LBuffer)
      UINT     m_nBufferPointer;
      UINT     m_nBufferContent;
      UINT     m_nBufferSize;
   };

   class CounterInputStream : public InputStream
   {
   public:
      CounterInputStream();
      ~CounterInputStream();

      virtual HRESULT Init(InputStream *pBaseInputStream, bool bIncludeSkipped = false);
      
      virtual HRESULT ReadByte(UINT *pTarget);
      virtual HRESULT ReadBytes(LBuffer *pBuffer, UINT nOff, UINT nLen, UINT *pnRead);

      virtual HRESULT Skip(UINT nByteCount);

      /**
       * Returns the number of bytes read so far. 
       * Does normally NOT include the number of bytes skipped. This behaviour
       * can be configured in Init().
       */
      UINT GetReadBytes();
   private:
      UINT m_nBytesRead;
      bool m_bIncludeSkipped;
   };


   /**
    * Serves as generic base class for all IO output operations.
    */
   class OutputStream
   {
   public:
      OutputStream();
      
      virtual HRESULT Init(OutputStream *pBaseOutputStream);

      virtual HRESULT WriteBytes(LBuffer *pBuffer, UINT nOff, UINT nLen) = 0;
      
   protected:
      OutputStream *m_pBaseStream;
   };


   class FileOutputStream : public OutputStream
   {
   public:
      FileOutputStream();
      ~FileOutputStream();

      virtual HRESULT Init(LFile *pFile);
      
      virtual HRESULT WriteBytes(LBuffer *pBuffer, UINT nOff, UINT nLen);

   private:
      LFile *m_pFile;
      bool   m_bDidOpen;
   };

   class XorOutputStream : public OutputStream
   {
   public:
      XorOutputStream();
      ~XorOutputStream();

      
      virtual HRESULT Init(OutputStream *pBaseOutputStream, LBuffer *pKeyBuffer);

      virtual HRESULT WriteBytes(LBuffer *pBuffer, UINT nOff, UINT nLen);

      HRESULT ResetKeyPointer();

   private:
      LBuffer *m_pKeyBuffer;
      LBuffer *m_pHelperBuffer;
      DWORD    m_nBufferPointer;
   };
   
   class BufferedOutputStream : public OutputStream
   {
   public:
      BufferedOutputStream();
      ~BufferedOutputStream();

      
      virtual HRESULT Init(OutputStream *pBaseOutputStream, UINT nBufferSize = 4096);

      virtual HRESULT WriteBytes(LBuffer *pBuffer, UINT nOff, UINT nLen);

      HRESULT Flush();

   private:
      LBuffer *m_plBuffer;
      UINT     m_nBufferPointer;
   };

   class DataOutputStream : public OutputStream
   {
   public:
      DataOutputStream();
      ~DataOutputStream();

      virtual HRESULT Init(OutputStream *pBaseOutputStream);

      virtual HRESULT WriteBytes(LBuffer *pBuffer, UINT nOff, UINT nLen);


      /**
       * Writes this byte to the stream.
       * Signed or unsigned doesn't matter but the 
       * value range is thus/of course limited to -128 to 255.
       */
      HRESULT WriteByte(int iValue);

      /**
       * Writes the given value as a two byte little endian
       * number. Signed or unsigned doesn't matter but the 
       * value range is thus/of course limited to -32768 to 65535.
       */
      HRESULT WriteLeShort(int iValue);
      
      /**
       * Writes the given value as a four byte little endian number.
       */
      HRESULT WriteLeInt(int iValue);

      /**
       * Writes an UTF8 string with a two byte (little endian) length
       * counting the bytes prepended.
       * Also works for pure ASCII as long as only characters of the lower
       * common half are used.
       */
      HRESULT WriteUtf8WithLength(LString *plsValue);

   private:
      LBuffer m_lbHelperBuffer;
   };
};

#endif // __LIO_H__8CD52941_8273_4531_94DB_26263DF0AFCF