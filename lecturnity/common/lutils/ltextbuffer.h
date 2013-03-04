#ifndef __LTEXTBUFFER_H__58E0CA4F_647F_4184_AA9C_36E563F38DE1
#define __LTEXTBUFFER_H__58E0CA4F_647F_4184_AA9C_36E563F38DE1

#ifndef __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B
#pragma message( "Do not include \"" __FILE__ "\", include \"lutils.h\"")
#endif __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B

class LReplaceContainer;

class LTextBuffer : public LBuffer
{
public:
   LTextBuffer();
   LTextBuffer(int nInitialSize);

   // LTextBuffer may be initialized with both char* and wchar_t* strings;
   // the internal handling depends on whether UNICODE is defined, or not.
   // If UNICODE is defined, the internal buffer is a wchar_t* buffer,
   // otherwise it's a char* (by using _TCHAR).
   LTextBuffer(const char *szInitialString);
   LTextBuffer(const wchar_t *wszInitialString);
   ~LTextBuffer();

   // "-1": There is a trailing 0 character...
   int GetTextLength() { return (GetSize() - 1) / sizeof _TCHAR; }

   // Loads an entire file into the buffer
   bool LoadFile(LPCTSTR tszFileName);

   // Saves the entire buffer into a file, including
   // any replacements that have been performed. Writes Unicode files
   // as default if compiled as Unicode, otherwise ANSI files.
   // The dwFileType can be one of the following
   //   LFILE_TYPE_TEXT_UNICODE
   //   LFILE_TYPE_TEXT_ANSI
   //   LFILE_TYPE_TEXT_UTF8
   // (see lfile.h)
#ifdef _UNICODE
   bool SaveFile(LPCTSTR tszFileName, DWORD dwFileType=LFILE_TYPE_TEXT_UNICODE, bool bWriteBOM=false);
#else
   bool SaveFile(LPCTSTR tszFileName, DWORD dwFileType=LFILE_TYPE_TEXT_ANSI, bool bWriteBOM=false);
#endif

   // Finds a needle in the buffer; if found, returns true, and the buffer
   // pointer is set to the next character after the line containing the needle.
   // If not found, the buffer pointer is reset.
   bool FindLineContaining(LPCTSTR tszNeedle, LPTSTR tszLine, DWORD dwBufSize=0xffffffff);

   // Returns true if the needle is found in the buffer, starting from
   // the current seek position, false
   // otherwise. If found, the seek pointer is set to a position
   // after the needle. Useful in combination with ReplaceAfter
   bool Find(LPCTSTR tszNeedle);

   // Replaces the content between after tszNeedle and tszDelim
   // by tszReplaceWith. Please note that these change do not take
   // place until SaveFile() is called, and that the changes are
   // only written to the file, and not to memory.
   bool ReplaceAfter(LPCTSTR tszNeedle, LPCTSTR tszDelim, LPCTSTR tszReplaceWith);

   // Replaces all occurrances of tszNeedle with tszReplace.
   bool ReplaceAll(LPCTSTR tszNeedle, LPCTSTR tszReplace);

   // Commits all replace operations to the internal buffer.
   void Commit();

   // Sets the buffer pointer in _TCHARs.
   virtual void SetBufferPointer(int nBufferPointer);

   _TCHAR *GetString();
private:
   _TCHAR *m_pPointer;

   LReplaceContainer *m_pHead;
   LReplaceContainer *m_pTail;

   void InsertContainer(LReplaceContainer *pContainer);
   void ReleaseContainers();
};

#endif // __LTEXTBUFFER_H__58E0CA4F_647F_4184_AA9C_36E563F38DE1
