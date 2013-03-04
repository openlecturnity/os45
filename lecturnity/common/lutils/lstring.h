#ifndef __LSTRING_H__
#define __LSTRING_H__

#ifndef __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B
#pragma message( "Do not include \"" __FILE__ "\", include \"lutils.h\"")
#endif __LUTILS_H__A8C8AFC2_2213_4cd5_804B_7B4CFC10F14B

class LString
{
public:
 	_TCHAR *m_szString;

   LString(); // sometimes cannot be invoked? -> unresolved symbol, or worse compile errors at another line
	LString(_TCHAR *szSource, bool bCopySource = true);
   LString(int nSize);
	~LString();

   void Release();
   void Allocate(int size);

   /**
    * There are cases where you know you will only get and use normal char*.
    * Use this method to add them correctly.
    */
   void AllocateAndSet(char *szContent, int nStringLength = 0);

   /**
    * You have a UTF8 encoded string and you want to fill this LString
    * with its contents.
    * I do not fully understand the conversion routines below but
    * I fear they do not really help for this problem.
    * If you do not use UNICODE this conversion here may be lossy.
    */
   void AllocateAndSetUtf8(char *sUtf8String, int nByteLength = 0);
   
   /**
    * parameter nBegin is used for input and output: as input parameter it
    * denotes the position where the search for the token should start
    * (this will be most often nEnd of a previous search)
    * returns true if nBegin and nEnd were changed (i.e. another token was found)
    * in this case nBegin points to the beginning of the token and nEnd to
    * the last character +1 of the token, so nBegin and nEnd can be used 
    * directly for Substring()
    */
   bool NextToken(int *nBegin, int *nEnd, _TCHAR *szDelimiters);
   
   /** 
     * returns true if it did successfully fill lsTarget with the desired
     * substring, false otherwise (i.e. this is too short or the parameters
     * are bogus)
     * nBegin points to the beginning of the string and nEnd one character
     * beyond the end
     */
   bool Substring(int nBegin, int nEnd, LString *lsTarget);

   /**
    * returns the position of the substring if the string contains
    * szSearchString, otherwise -1. Search starts at the given starting index
    * or 0 if none is given.
    */
   int FindString(const _TCHAR *szSearchString, int nBegin = 0);

   /**
    * return true if the szSearchString is contained in the string.
    */
   bool Contains(const _TCHAR *szSearchString);

   /**
    * returns true if the string starts with szString.
    */
   bool StartsWith(const _TCHAR *szString);

   inline int Length() { return m_szString != NULL ? _tcslen(m_szString) : 0; }
   inline operator LPTSTR () { return m_szString; }
   inline BYTE* AccessBinary() { return (BYTE *)m_szString; }
   inline _TCHAR* Access() { return m_szString; }
   void operator= (LString &lsSource);
   void operator= (const _TCHAR *szSource);
   void operator+= (LString &lsAdd);
   void operator+= (_TCHAR *szAdd);
   _TCHAR operator[] (int nPos) { return m_szString[nPos]; }
   bool operator== (const LString &lsCompare);
   bool operator== (const _TCHAR *szCompare);

   static char *TCharToUtf8(const _TCHAR *tszString);
   static char *Utf8ToTChar(const _TCHAR *tszString);
   // delete[] the result if != NULL.
   static char *WideChar2Ansi(const _TCHAR *document);
private:
   //int    m_nLength;

   inline bool IsDelimiter(_TCHAR cCandidate, _TCHAR *szDelimiters)
   {
      _TCHAR *szIterator = szDelimiters;
      for (int i=0; szIterator[i] != _T('\0'); ++i)
      {
         if (szIterator[i] == cCandidate)
            return true;
      }
      return false;
   }
};

#endif