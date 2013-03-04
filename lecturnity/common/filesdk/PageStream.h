#ifndef __PAGESTREAM__
#define __PAGESTREAM__

#include "FileOutput.h"
#include "MediaStream.h"

class FILESDK_EXPORT CPage
{
public:
   CPage();
   ~CPage();

public:
   // set member variables to default values
   // void Init();
   // reset member variables
   void Clear();

public:
   // get and set functions for member variables
   int GetJoinId() { return m_iJoinId; }
   void SetJoinId(int joinId); // also affects the m_pCopySource: in order to store
                                // this value across structural changes (cut & paste)

   int GetJumpId() { return GetJoinId(); }
   void SetJumpId(int iJumpId) { SetJoinId(iJumpId); }
   // also affects the m_pCopySource: in order to store
   // this value across structural changes (cut & paste)

   int GetPageNumber() {return m_iNumber;}
   void SetPageNumber(int pageNumber) {m_iNumber = pageNumber;}
   
   int GetBegin() {return m_iBegin;}
   void SetBegin(int begin) {m_iBegin = begin;}

   int GetEnd() {return m_iEnd;}
   void SetEnd(int end) {m_iEnd = end;}

   LPCTSTR GetTitle() { return m_csTitle; }
   void SetTitle(LPCTSTR title) { m_csTitle = title; }
   void GetKeywords(CStringList &stringList);
   void SetKeywords(CStringList &stringList);
   void GetKeywords(CString &csKeywords);
   void SetKeywords(CString &cskeywords);

   bool IsHidePointers() { return m_bHidePointers; }
   void SetHidePointers(bool bHide);


   LPCTSTR GetThumb() { return m_csThumb; }
   void SetThumb(LPCTSTR thumb) { m_csThumb = thumb; }

public:
   CPage *Copy(bool bCopySource = false);
   
private:
   CPage  *m_pCopySource;
   int     m_iJoinId;   // unique id: which page segments belong together
                        // changes only with title or keyword changes
   //int     m_iJumpId;   // every (new) occurance of a page in the stream view
                        // has a different id: used for jumps of interaction areas
   int     m_iNumber;
   int     m_iBegin;
   int     m_iEnd;
   CString m_csTitle;
   CStringList m_slKeywords;
   CString m_csThumb;
   bool    m_bHidePointers;
};

class FILESDK_EXPORT CPageStream : public CMediaStream
{

public:
   CPageStream();
   ~CPageStream();

public:
   CPageStream *Copy();
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();


public:
   
   /**
    * Fills in the pages within the given time range of the 
    * current whiteboard stream into the pages variable.
    * "Returns" firstPos and lastPos; firstPos is the first
    * position at which a page is added, and lastPos the last.
    * These variables are left unchanged if nothing is added.
    * The variable offset is added to the begin and end times
    * of the added page (CPage) objects (in order to match source
    * and target stream times).
    */
   void GetPages(CArray<CPage *, CPage *> &pages, int fromMsec, int toMsec, int offset, int &firstPos, int &lastPos);
   int GetPrevPageBegin(int curMsec, int fromMsec ,int toMsec, int offset);
   int GetNextPageBegin(int curMsec, int fromMsec ,int toMsec, int offset);
   CPage *GetFirstPage(int fromMsec ,int toMsec, int offset);
   CPage *GetLastPage(int fromMsec ,int toMsec, int offset);

   void WriteToLep(CFileOutput *pOutput, int targetBegin, int targetEnd, 
      int sourceBegin, int sourceEnd, LPCTSTR szLepFileName);


   // find page which begins at timestamp
   CPage *FindPage(int timestamp);
   // find actual page at a given timestamp
   CPage *FindPageAtTimestamp(int timestamp);
   CPage *FindPageWithNumber(int pageNumber);
   CPage *GetPage(int iPageIndex);

   void Add(CPage *pPage);

   bool IsEmpty(); 

public:
   // get and set functions for member variables
   int GetID() {return m_iID;}
   void SetID(int id) {m_iID = id;}

private:

public:
   // TODO: is m_pageCounter used anywhere??? Or just dead code leading to irritations???
   //static int m_pageCounter;

private:

   // list of pages created by event queue
   CArray<CPage *, CPage *> m_pageArray;

   int m_iID;
};

#endif // __PAGESTREAM__