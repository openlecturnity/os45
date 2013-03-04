#include "StdAfx.h"
#include "PageStream.h"
#include "MiscFunctions.h"


/*********************************************
 *********************************************/

// Linear congruential pseudo random number generator based on D. Lehmer
// s is the range; initialize with s < 0 if a truely random sequence is desired
// So the first call should not be with the desired range.
unsigned long int lehmer(long int s) {
    static unsigned long long a = 2007, b = 4194301, c = 2147483647, z = b;
    if ( s < 0 ) {s = -s; a = s;}
    z = (a + b * z) % c;
    return z % s;
}
bool s_bLehmerInitialized = false;

CPage::CPage()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();

   if (!s_bLehmerInitialized) {
       long lTime = time(NULL);
       if (lTime > 0)
           lTime = -lTime;
       lehmer(lTime);
       s_bLehmerInitialized = true;
   }
}

CPage::~CPage()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

/*
void CPage::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_pCopySource = NULL;
   m_iJoinId = -1;
   m_iNumber = -1;
   m_iBegin  = -1;
   m_iEnd    = -1;
}
*/

void CPage::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_pCopySource = NULL;
   m_iJoinId = -1;
   m_iNumber = -1;
   m_iBegin  = -1;
   m_iEnd    = -1;
   m_csTitle.Empty();
   m_slKeywords.RemoveAll();
   m_csThumb.Empty();
   m_bHidePointers = false;
}

CPage *CPage::Copy(bool bCopySource)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CPage *retPage = new CPage();
   if (bCopySource)
   {
      retPage->m_pCopySource = this;
      // Note: SetJoinId() will then also affect this source. Otherwise
      // changing this join id in the Editor (after GetPages()) will only change 
      // the copied element and the change would be lost after further changes (new copies).
   }
   retPage->SetJoinId(m_iJoinId);
   retPage->SetBegin(m_iBegin);
   retPage->SetEnd(m_iEnd);
   retPage->SetPageNumber(m_iNumber);
   retPage->SetTitle(m_csTitle);
   retPage->SetKeywords(m_slKeywords);
   retPage->SetThumb(m_csThumb);
   retPage->SetHidePointers(m_bHidePointers);

   return retPage;
}

void CPage::SetJoinId(int iJoinId)
{
    if (m_iJoinId != iJoinId) {
        m_iJoinId = iJoinId;
        if (m_pCopySource != NULL)
            m_pCopySource->SetJoinId(iJoinId);
    }
}

void CPage::GetKeywords(CStringList &stringList)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_slKeywords.GetHeadPosition();
   while (position)
   {
      CString string = m_slKeywords.GetNext(position);
      stringList.AddTail(string);
   }
}

void CPage::GetKeywords(CString &csKeywords)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_slKeywords.GetHeadPosition();
   while (position)
   {
      csKeywords += m_slKeywords.GetNext(position);
      if (position)
         csKeywords += "; ";
   }
}

void CPage::SetKeywords(CStringList &stringList)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_slKeywords.RemoveAll();

   POSITION position = stringList.GetHeadPosition();
   while (position)
   {
      CString string = stringList.GetNext(position);
      m_slKeywords.AddTail(string);
   }
}

void CPage::SetKeywords(CString &csKeywords)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_slKeywords.RemoveAll();

   int nStart = 0;
   int iNextPos = 0;
   while (iNextPos != -1)
   {
      iNextPos = csKeywords.Find(_T(";"), nStart);
      if (iNextPos != -1)
      {
         CString csKeyword = csKeywords.Mid(nStart, iNextPos-nStart);
         csKeyword.TrimLeft(_T(" "));
         m_slKeywords.AddTail(csKeyword);
         nStart = iNextPos + 1;
      }
      else 
      {
         CString csKeyword = csKeywords.Mid(nStart);
         csKeyword.TrimLeft(_T(" "));
         if (!csKeyword.IsEmpty())
            m_slKeywords.AddTail(csKeyword);
      }
   }
}

void CPage::SetHidePointers(bool bHide) { 
    m_bHidePointers = bHide; 
    if (m_pCopySource != NULL)
        m_pCopySource->SetHidePointers(bHide);
}

/*********************************************
 *********************************************/

CPageStream::CPageStream() : CMediaStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init();
}

CPageStream::~CPageStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}


CPageStream *CPageStream::Copy()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CPageStream *pCopyStream = new CPageStream();

   for (int i = 0; i < m_pageArray.GetSize(); ++i)
   {
      CPage *pPage = m_pageArray[i]->Copy();
      if (pPage)
         pCopyStream->Add(pPage);
   }

   return pCopyStream;
}


void CPageStream::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   //m_lastUsedPosition = NULL;
   m_iID              = -1;
}

void CPageStream::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // delete all pages
   for (int i = 0; i < m_pageArray.GetSize(); ++i)
   {
      CPage *page = m_pageArray.GetAt(i);
      if (page)
         delete page;
   }
   // remove array entries
   m_pageArray.RemoveAll();
   
   m_iID              = -1;
}

void CPageStream::GetPages(CArray<CPage *, CPage *> &pages, int fromMsec, int toMsec, int offset, int &firstPos, int &lastPos)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_pageArray.GetSize() <= 0)
      return;

   bool isFirst = true;
   for (int i = 0; i < m_pageArray.GetSize(); ++i)
   {
      CPage *page = m_pageArray[i];
      if (page &&
         ((page->GetEnd() + offset > fromMsec && page->GetEnd() + offset <= toMsec) ||
         (page->GetBegin() + offset >= fromMsec && page->GetBegin() + offset < toMsec) ||
         (page->GetBegin() + offset < fromMsec && page->GetEnd() + offset > toMsec)))
      {
         CPage *newPage = page->Copy(true);
         if (newPage)
         {
            newPage->SetBegin(newPage->GetBegin() + offset);
            newPage->SetEnd(newPage->GetEnd() + offset);
           
            pages.Add(newPage);
            if (isFirst)
               firstPos = pages.GetSize() - 1;
            isFirst = false;
         }
      }
   }

   // at least one page is inserted
   if (!isFirst)
      lastPos = pages.GetSize() - 1;
}

int CPageStream::GetPrevPageBegin(int curMsec, int fromMsec ,int toMsec, int offset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_pageArray.GetSize() <= 0)
      return -1;

   int prevPageBegin = -1;
   for (int i = m_pageArray.GetSize() - 1; i >= 0; --i)
   {
      CPage *page = m_pageArray[i];
      if (page &&
         ((page->GetEnd() + offset > fromMsec && page->GetEnd() + offset <= toMsec) ||
         (page->GetBegin() + offset >= fromMsec && page->GetBegin() + offset < toMsec) ||
         (page->GetBegin() + offset < fromMsec && page->GetEnd() + offset > toMsec)) &&
         (page->GetBegin() + offset < curMsec))
      {
         prevPageBegin = page->GetBegin() + offset;
         break;
      }
   }

   return prevPageBegin;
}
 
int CPageStream::GetNextPageBegin(int curMsec, int fromMsec ,int toMsec, int offset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_pageArray.GetSize() <= 0)
      return -1;

   int nextPageBegin = -1;
   bool found = false;
   for (int i = 0; i < m_pageArray.GetSize(); ++i)
   {
      CPage *page = m_pageArray[i];
      if (page &&
         ((page->GetEnd() + offset > fromMsec && page->GetEnd() + offset <= toMsec) ||
         (page->GetBegin() + offset >= fromMsec && page->GetBegin() + offset < toMsec) ||
         (page->GetBegin() + offset < fromMsec && page->GetEnd() + offset > toMsec)))
      {
         if (found)
         {
            page = m_pageArray[i];
            nextPageBegin = page->GetBegin() + offset;
            break;
         }
         if (page->GetEnd() + offset > curMsec)
         {
            found = true;
         }
      }

   }

   return nextPageBegin;
}

CPage *CPageStream::GetFirstPage(int fromMsec ,int toMsec, int offset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_pageArray.GetSize() <= 0)
      return NULL;

   for (int i = 0; i < m_pageArray.GetSize(); ++i)
   {
      CPage *page = m_pageArray[i];
      if (page &&
         (page->GetEnd() + offset > fromMsec && page->GetEnd() + offset <= toMsec) ||
         (page->GetBegin() + offset >= fromMsec && page->GetBegin() + offset < toMsec) ||
         (page->GetBegin() + offset < fromMsec && page->GetEnd() + offset > toMsec)) 
      {
         return page;
      }
   }

   return NULL;
}
  
CPage *CPageStream::GetLastPage(int fromMsec ,int toMsec, int offset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_pageArray.GetSize() <= 0)
      return NULL;

   for (int i =  m_pageArray.GetSize() - 1; i >= 0; --i)
   {
      CPage *page = m_pageArray[i];
      if (page &&
         (page->GetEnd() + offset > fromMsec && page->GetEnd() + offset <= toMsec) ||
         (page->GetBegin() + offset >= fromMsec && page->GetBegin() + offset < toMsec) ||
         (page->GetBegin() + offset < fromMsec && page->GetEnd() + offset > toMsec)) 
      {
         return page;
      }
   }

   return NULL;
}

void CPageStream::WriteToLep(CFileOutput *pOutput, int targetBegin, int targetEnd, 
                             int sourceBegin, int sourceEnd, LPCTSTR szLepFileName)
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   for (int i = 0; i < m_pageArray.GetSize(); ++i)
   {
      CPage *pPage = m_pageArray[i];
      if (pPage && 
          (pPage->GetEnd() >= sourceBegin) &&
          (pPage->GetBegin() < sourceEnd))
      {
         CString line;
         CString sTitleSgml;
         StringManipulation::TransformForSgml(pPage->GetTitle(), sTitleSgml);
         CString sKeywords, sKeywordsSgml;
         pPage->GetKeywords(sKeywords);
         StringManipulation::TransformForSgml(sKeywords, sKeywordsSgml);
         StringManipulation::TransformForSgml(pPage->GetTitle(), sTitleSgml);
         CString sHidePointers;
         if (pPage->IsHidePointers())
             sHidePointers = _T(" hide-pointers=\"true\"");
         line.Format(_T("        <metadata slide-start=\"%d\" slide-end=\"%d\" title=\"%s\" keywords=\"%s\" number=\"%d\" id=\"%d\"%s></metadata>\n"), 
            pPage->GetBegin(), pPage->GetEnd(), sTitleSgml, sKeywordsSgml, pPage->GetPageNumber(), pPage->GetJoinId(), sHidePointers);
         pOutput->Append(line); 
      }
   }
}

CPage *CPageStream::FindPage(int timestamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CPage *page = NULL;
   for (int i = 0; i < m_pageArray.GetSize(); ++i)
   {
      page = m_pageArray[i];
      if (page && page->GetBegin() == timestamp)
         return m_pageArray[i];
   }

   return NULL;
}

CPage *CPageStream::FindPageAtTimestamp(int timestamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CPage *page = NULL;
   for (int i = 0; i < m_pageArray.GetSize(); ++i)
   {
      page = m_pageArray[i];
      if (page && (timestamp >= page->GetBegin()) 
         && (timestamp < page->GetEnd()))
         return m_pageArray[i];
   }

   return NULL;
}

CPage *CPageStream::FindPageWithNumber(int pageNumber)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CPage *page = NULL;
   for (int i = 0; i < m_pageArray.GetSize(); ++i)
   {
      page = m_pageArray[i];
      if (page && page->GetPageNumber() == pageNumber)
         return m_pageArray[i];
   }

   return NULL;
}

CPage *CPageStream::GetPage(int iPageIndex)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (iPageIndex >= m_pageArray.GetSize())
      return NULL;

   return m_pageArray[iPageIndex];
}

void CPageStream::Add(CPage *pPage) 
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_pageArray.Add(pPage);
}

bool CPageStream::IsEmpty() 
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return m_pageArray.GetSize() == 0 ? true : false;
}