#include "StdAfx.h"

#include "LmdFile.h"
#include "Resource.h"


//#define ACTIVE_PAGE_RGB RGB(0xA6, 0xC7, 0xF5)
#define ACTIVE_PAGE_RGB RGB(0xDF, 0xEE, 0xFE)
#define ACTIVE_PAGE_HOT_RGB RGB(0xFD, 0xE5, 0x92)

int CStructureInformation::m_iIdCounter = 0;

/*********************************************
 *********************************************/

CMetaInformation::CMetaInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init();
}

CMetaInformation::~CMetaInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

void CMetaInformation::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

void CMetaInformation::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_csTitle.Empty();
   m_csAuthor.Empty();

   m_csCreator.Empty();
   m_csRecordDate.Empty();
   m_slKeywords.RemoveAll();
   
   m_csVersion.Empty();
   m_csComments.Empty();
   m_csLength.Empty();
   m_csRecordTime.Empty();
}

bool CMetaInformation::ParseFrom(CLepSgml *pLepFile) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (pLepFile == NULL)
        return false;

    CString csTitle;
    pLepFile->GetMetaInfo(_T("title"), csTitle);
    if (!csTitle.IsEmpty())
        SetTitle(csTitle);

    CString csAuthor;
    pLepFile->GetMetaInfo(_T("author"), csAuthor);
    if (!csAuthor.IsEmpty())
        SetAuthor(csAuthor);

    CString csCreator;
    pLepFile->GetMetaInfo(_T("creator"), csCreator);
    if (!csCreator.IsEmpty())
        SetCreator(csCreator);

    CString csComments;
    pLepFile->GetMetaInfo(_T("comments"), csComments);
    if (!csComments.IsEmpty())
        SetComments(csComments);

    CString csRecordLength;
    pLepFile->GetMetaInfo(_T("length"), csRecordLength);
    if (!csRecordLength.IsEmpty())
        SetLength(csRecordLength);

    CString csRecordDate;
    pLepFile->GetMetaInfo(_T("recorddate"), csRecordDate);
    if (!csRecordDate.IsEmpty())
        SetRecordDate(csRecordDate);

    CString csRecordTime;
    pLepFile->GetMetaInfo(_T("recordtime"), csRecordTime);
    if (!csRecordTime.IsEmpty())
        SetRecordTime(csRecordTime);

    CStringArray aKeywords;
    pLepFile->GetKeywords(aKeywords);
    for (int i = 0; i < aKeywords.GetSize(); ++i) 
        AddKeyword(aKeywords[i]);

    return true;
}

bool CMetaInformation::ParseFrom(CLmdSgml *pLmdFile) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (pLmdFile == NULL)
        return false;

    CString csTitle;
    pLmdFile->GetMetaInfo(_T("title"), csTitle);
    if (!csTitle.IsEmpty())
        SetTitle(csTitle);

    CString csAuthor;
    pLmdFile->GetMetaInfo(_T("author"), csAuthor);
    if (!csAuthor.IsEmpty())
        SetAuthor(csAuthor);

    CString csCreator;
    pLmdFile->GetMetaInfo(_T("creator"), csCreator);
    if (!csCreator.IsEmpty())
        SetCreator(csCreator);

    CString csRecordDate;
    pLmdFile->GetMetaInfo(_T("recorddate"), csRecordDate);
    if (!csRecordDate.IsEmpty())
        SetRecordDate(csRecordDate);

    CStringArray aKeywords;
    pLmdFile->GetKeywords(aKeywords);
    for (int i = 0; i < aKeywords.GetSize(); ++i) 
        AddKeyword(aKeywords[i]);

    return true;
}

bool CMetaInformation::ParseFrom(CLrdSgml *pLrdFile) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (pLrdFile == NULL)
        return false;
    
    CString csTitle;
    pLrdFile->GetMetaInfo(_T("TITLE"), csTitle);
    if (!csTitle.IsEmpty())
        SetTitle(csTitle);
        
    CString csAuthor;
    pLrdFile->GetMetaInfo(_T("AUTHOR"), csAuthor);
    if (!csAuthor.IsEmpty())
        SetAuthor(csAuthor);
    
    CString csComments;
    pLrdFile->GetMetaInfo(_T("COMMENTS"), csComments);
    if (!csComments.IsEmpty())
        SetComments(csComments);
    
    CString csLength;
    pLrdFile->GetMetaInfo(_T("LENGTH"), csLength);
    if (!csLength.IsEmpty())
        SetLength(csLength);
    
    CString csRecordDate;
    pLrdFile->GetMetaInfo(_T("RECORDDATE"), csRecordDate);
    if (!csRecordDate.IsEmpty())
        SetRecordTime(csRecordDate);

    return true;
}

void CMetaInformation::Fill(CMetaInformation *pMetaInfo)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (!pMetaInfo)
      return;

   pMetaInfo->SetTitle(m_csTitle);
   pMetaInfo->SetAuthor(m_csAuthor);
   pMetaInfo->SetCreator(m_csCreator);
   pMetaInfo->SetRecordDate(m_csRecordDate);
   pMetaInfo->SetKeywords(m_slKeywords);
}

void CMetaInformation::SetRecordTime(LPCTSTR recordTime)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_csRecordTime = recordTime;


   int pos = m_csRecordTime.ReverseFind('-');
   if (pos != -1)
   {
      m_csRecordTime.Delete(0, pos+1);
      m_csRecordTime.TrimLeft(' ');
   }
}

void CMetaInformation::GetKeywords(CStringList &stringList)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_slKeywords.GetHeadPosition();
   while (position)
   {
      CString string = m_slKeywords.GetNext(position);
      stringList.AddTail(string);
   }
}

void CMetaInformation::SetKeywords(CStringList &stringList)
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
   
void CMetaInformation::AddKeyword(CString &keyword)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_slKeywords.AddTail(keyword);
}

void CMetaInformation::AddKeyword(LPCTSTR keyword)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CString string = keyword;
   m_slKeywords.AddTail(string);
}

void CMetaInformation::GetKeywords(CString &keywords)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   keywords.Empty();

   bool isFirst = true;
   POSITION position = m_slKeywords.GetHeadPosition();
   while (position)
   {
      CString string = m_slKeywords.GetNext(position);
      if (!isFirst)
         keywords += _T("; ");
      keywords += string;
      isFirst = false;
   }
   
}

void CMetaInformation::SetKeywords(CString &csKeywords) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    int iPosition = 0;
    m_slKeywords.RemoveAll();
    int iLastPosition = 0;
    bool bKeywordFound = false;
    while (iPosition != -1) {
        iPosition = csKeywords.Find(_T(';'), iLastPosition);
        if (iPosition != -1) {
            CString csKeyword = csKeywords;
            csKeyword.Delete(iPosition, csKeyword.GetLength() - iPosition);
            if (iLastPosition > 0)
                csKeyword.Delete(0, iLastPosition);
            csKeyword.TrimLeft(_T(' '));
            if (!csKeyword.IsEmpty())
                m_slKeywords.AddTail(csKeyword);
            iLastPosition = iPosition + 1;
            bKeywordFound = true;
        }
    }

    if (iLastPosition > 0 || !bKeywordFound) {
        CString csKeyword = csKeywords;
        if (bKeywordFound)
            csKeyword.Delete(0, iLastPosition);
        csKeyword.TrimLeft(_T(' '));
        if (!csKeyword.IsEmpty())
            m_slKeywords.AddTail(csKeyword);
    }
}

/*********************************************
 *********************************************/

CStructureInformation::CStructureInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init();
}

CStructureInformation::~CStructureInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

void CStructureInformation::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iUniqueId = -1;
   m_iBegin    = -1;
   m_iEnd      = -1;
   m_pCopySource = NULL;
   m_csTitle = _T("");
}

void CStructureInformation::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_elementList.IsEmpty())
      return;

   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
         delete element;
   }

   m_elementList.RemoveAll();
}

bool CStructureInformation::ParseFrom(CLmdSgml *pLmdFile) {
    bool bFirstPageInSegment = true;
    if (pLmdFile == NULL)
        return false;

    SgmlElement *pStructureTag = pLmdFile->Find(_T("structure"), _T("docinfo"));
    return Fill(pStructureTag, bFirstPageInSegment);
}

bool CStructureInformation::Fill(SgmlElement *pStructureTag, bool &bFirstPageInSegment) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (pStructureTag == NULL)
        return false;

    CArray<SgmlElement *, SgmlElement *> aElements;
    pStructureTag->GetElements(aElements);
    for (int i = 0; i < aElements.GetSize(); ++i) {
        SgmlElement *pElement = aElements[i];
        if (pElement != NULL) {
            if (_tcscmp(pElement->GetName(), _T("chapter")) == 0) {
                CChapterInformation *pChapter = new CChapterInformation();
                pChapter->Fill(pElement, bFirstPageInSegment);
                pChapter->SetUniqueId(m_iIdCounter);
                ++m_iIdCounter;
                m_elementList.AddTail(pChapter);
            } else if (_tcscmp(pElement->GetName(), _T("page")) == 0) {
                CPageInformation *pPage = new CPageInformation();
                pPage->Fill(pElement, bFirstPageInSegment);
                pPage->SetUniqueId(m_iIdCounter);
                ++m_iIdCounter;
                m_elementList.AddTail(pPage);
            } else if (_tcscmp(pElement->GetName(), _T("clip")) == 0) {
                CClipInformation *pClip = new CClipInformation();
                pClip->Fill(pElement, bFirstPageInSegment);
                pClip->SetUniqueId(m_iIdCounter);
                ++m_iIdCounter;
                m_elementList.AddTail(pClip);
            }
        }
    }

    if (!m_elementList.IsEmpty()) {
        m_iBegin = GetFirstTime(); // was: 0
        m_iEnd = GetLastTime();
    }

    return true;
}

bool CStructureInformation::FillWithNewPage(CString &csPageTitle, UINT nLengthMs)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   this->SetUniqueId(m_iIdCounter);
   ++m_iIdCounter;

   CChapterInformation *pChapter = new CChapterInformation();
   pChapter->FillWithNewPage(csPageTitle, nLengthMs);
   pChapter->SetUniqueId(m_iIdCounter);
   ++m_iIdCounter;

   m_elementList.AddTail(pChapter);
 

   m_iBegin = 0;
   m_iEnd = nLengthMs;

   return true;
}


void CStructureInformation::AppendStructure(CStructureInformation *structure)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_elementList.IsEmpty())
      return;

   CStructureInformation *lastElement = m_elementList.GetTail();

   if (!lastElement)
      return;

   // only merge the first element of the new list with the 
   // last element of current list, if possible
   bool firstElement = true;
   POSITION position = structure->m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = structure->m_elementList.GetNext(position);
      bool bElementMerged = false;
      if (firstElement && element)
      {
         if (firstElement && lastElement &&
            element->GetType() == lastElement->GetType() &&
            element->GetUniqueId() == lastElement->GetUniqueId())
         {
            if (element->GetType() == PAGE)
            {
               lastElement->SetEnd(element->GetEnd());
               // delete element
               delete element;
               element = NULL;
               bElementMerged = true;
            }
            else if (element->GetType() == CHAPTER || element->GetType() == BASE)
            {
               lastElement->AppendStructure(element);
               lastElement->SetEnd(element->GetEnd());
               // delete element without deleting element of m_elementList
               element->m_elementList.RemoveAll();
               delete element;
               element = NULL;
               bElementMerged = true;
            }
         }
      }
      
      if (!bElementMerged)
         m_elementList.AddTail(element);
      
      firstElement = false;
   }
}

bool CStructureInformation::Complete(CClipStream *clipStream, CWhiteboardStream *whiteboardStream, CPageStream *pageStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
      {
         switch (element->GetType())
         {
         case PAGE:
            ((CPageInformation *)element)->Complete(whiteboardStream, pageStream);
            break;
         case CLIP:
            ((CClipInformation *)element)->Complete(clipStream);
            break;
         case BASE:
         case CHAPTER:
            element->Complete(clipStream, whiteboardStream, pageStream);
            break;
         }
      }
   }
   

   return true;
}

bool CStructureInformation::Complete(CClipStream *clipStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
      {
         switch (element->GetType())
         {
         case CLIP:
            ((CClipInformation *)element)->Complete(clipStream);
            break;
         case BASE:
         case CHAPTER:
            element->Complete(clipStream);
            break;
         }
      }
   }
   

   return true;
}

void CStructureInformation::UpdateMetadata(CPageStream *pPageStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
      {
         switch (element->GetType())
         {
         case PAGE:
            ((CPageInformation *)element)->UpdateMetadata(pPageStream);
            break;
         case BASE:
         case CHAPTER:
            element->UpdateMetadata(pPageStream);
            break;
         }
      }
   }
   

   return;
}

void CStructureInformation::GetAllElements(CArray<CStructureInformation *, CStructureInformation *> *paElements)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (paElements == NULL)
      return;

   paElements->Add(this);

   POSITION pos = m_elementList.GetHeadPosition();
   while (pos != NULL)
   {
      CStructureInformation *pElement = m_elementList.GetNext(pos);
      if (pElement)
         pElement->GetAllElements(paElements);
   }
}

void CStructureInformation::ChangeUniqueIds()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iUniqueId = m_iIdCounter;
   ++m_iIdCounter;

   POSITION pos = m_elementList.GetHeadPosition();
   while (pos != NULL)
   {
      CStructureInformation *pElement = m_elementList.GetNext(pos);
      if (pElement)
         pElement->ChangeUniqueIds();
   }

   if (m_pCopySource != NULL)
      m_pCopySource->ChangeUniqueIds();
}

void CStructureInformation::HideClickPages(CArray<CPage *, CPage *> &aPageArray) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CArray<int, int> aPagesToDelete;
    int iPageNr = 0;

    CString csPage;
    csPage.LoadString(IDS_DEFAULT_PAGE_TITLE);
    csPage += " ";

    CPage *pPreviousPage = NULL;
    for (int i = 0; i < aPageArray.GetSize(); ++i) {
        if (aPageArray[i] != NULL) {
            int iPageBegin = aPageArray[i]->GetBegin();
            int iPageEnd = aPageArray[i]->GetEnd();
            bool bDeleted = false;
            
            CStructureInformation *pElement = FindElement(iPageBegin, PAGE);
            if (pElement && pElement->GetType() == PAGE && pElement->GetBegin() == iPageBegin) {
                if (((CPageInformation *)pElement)->GetPageType() == _T("click")) {
                    if (pPreviousPage) {
                        pPreviousPage->SetEnd(iPageEnd);
                    }
                    aPagesToDelete.Add(i);
                } else {
                    if (pPreviousPage == NULL)
                        aPageArray[i]->SetBegin(0);
                    pPreviousPage = aPageArray[i];
                    // dynamic page titles
                    CString csOldPageTitle = ((CPageInformation *)pElement)->GetTitle(); 
                    iPageNr++;
                    bool bDynamic = (csPage == csOldPageTitle.Left(csPage.GetLength()));
                    // check for valid number
                    int number = _wtoi( csOldPageTitle.Right(csOldPageTitle.GetLength()-csPage.GetLength()).GetBuffer() );
                    bDynamic &= number > 0;
                    // additional check because _wtoi cut off additional non-digits
                    CString csTitleCheck;
                    csTitleCheck.Format(_T("%ls%i"), csPage, number);
                    bDynamic &= (csTitleCheck == csOldPageTitle);
                    // set dynamic title if format matches
                    if ( bDynamic ) {
                        CString csNewTitle;
                        csNewTitle.Format(_T("%ls%i"), csPage, iPageNr);
                        ((CPageInformation *)pElement)->SetTitle(csNewTitle);
                        aPageArray[i]->SetTitle(csNewTitle);
                    } 
                }
            }
        }
    }

    if (aPagesToDelete.GetSize() > 0) {
        for (int i = aPagesToDelete.GetSize()-1; i >= 0; --i) {
            CPage *pPage = aPageArray[aPagesToDelete[i]];
            aPageArray.RemoveAt(aPagesToDelete[i]);
            delete pPage;
        }
    }

}

void CStructureInformation::UpdatePageTitles(CArray<CPage *, CPage *> &aPageArray) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CArray<int, int> aPagesToDelete;
    int iPageNr = 0;

    CString csPage;
    csPage.LoadString(IDS_DEFAULT_PAGE_TITLE);
    csPage += " ";

    CPage *pPreviousPage = NULL;
    for (int i = 0; i < aPageArray.GetSize(); ++i) {
        if (aPageArray[i] != NULL) {
            int iPageBegin = aPageArray[i]->GetBegin();
            int iPageEnd = aPageArray[i]->GetEnd();
            bool bDeleted = false;

            CStructureInformation *pElement = FindElement(iPageBegin, PAGE);
            if (pElement && pElement->GetType() == PAGE && pElement->GetBegin() == iPageBegin) {
                // dynamic page titles
                CString csOldPageTitle = ((CPageInformation *)pElement)->GetTitle(); 
                iPageNr++;
                bool bDynamic = (csPage == csOldPageTitle.Left(csPage.GetLength()));
                // check for valid number
                int number = _wtoi( csOldPageTitle.Right(csOldPageTitle.GetLength()-csPage.GetLength()).GetBuffer() );
                bDynamic &= number > 0;
                // additional check because _wtoi cut off additional non-digits
                CString csTitleCheck;
                csTitleCheck.Format(_T("%ls%i"), csPage, number);
                bDynamic &= (csTitleCheck == csOldPageTitle);
                // set dynamic title if format matches
                if ( bDynamic ) {
                    CString csNewTitle;
                    csNewTitle.Format(_T("%ls%i"), csPage, iPageNr);
                    ((CPageInformation *)pElement)->SetTitle(csNewTitle);
                    aPageArray[i]->SetTitle(csNewTitle);
                } 
            }
        }
    }
}

void CStructureInformation::UpdateNonClickPageTitles(int &iNextNumber) {  
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // This function was only called for the automatically created pages 
    // of a pure screen grabbing recording
    // Therefore the pages are all in the same chapter
    if (m_elementList.IsEmpty())
        return;

    CString csDefaultTitle;
    csDefaultTitle.LoadString(IDS_DEFAULT_PAGE_TITLE);
    csDefaultTitle += " ";

    POSITION position = m_elementList.GetHeadPosition();
    while (position != NULL) {
        CStructureInformation *pElement = m_elementList.GetNext(position);
        if (pElement != NULL && pElement->GetType() == PAGE) {
            if (((CPageInformation *)pElement)->GetPageType() != "click") {
                CString csOldPageTitle = ((CPageInformation *)pElement)->GetTitle(); 
                bool bDynamic = (csDefaultTitle == csOldPageTitle.Left(csDefaultTitle.GetLength()));
                // check for valid number
                int number = _wtoi( csOldPageTitle.Right(csOldPageTitle.GetLength()-csDefaultTitle.GetLength()).GetBuffer() );
                bDynamic &= number > 0;
                // additional check because _wtoi cut off additional non-digits
                CString csTitleCheck;
                csTitleCheck.Format(_T("%ls%i"), csDefaultTitle, number);
                bDynamic &= (csTitleCheck == csOldPageTitle);
                // set dynamic title if format matches
                if ( bDynamic ) {
                    CString csNewTitle;
                    csNewTitle.Format(_T("%ls%i"), csDefaultTitle, iNextNumber);
                    ((CPageInformation *)pElement)->SetTitle(csNewTitle);
                } 
                iNextNumber++;
            }
        } else
            pElement->UpdateNonClickPageTitles(iNextNumber);
    }

    return;
}

void CStructureInformation::DeleteClickPages() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // This function was only called for the automatically created pages 
    // of a pure screen grabbing recording
    // Therefore the pages are all in the same chapter
    if (m_elementList.IsEmpty())
        return;

    POSITION position = m_elementList.GetHeadPosition();
    while (position != NULL) {
        CStructureInformation *pElement = m_elementList.GetNext(position);
        if (pElement != NULL && pElement->GetType() == PAGE) {
            if (((CPageInformation *)pElement)->GetPageType() == "click") {
                int iEndTimestamp = pElement->GetEnd();
                int iBeginTimestamp = pElement->GetBegin();
                delete pElement;

                POSITION delPosition = position;
                bool bLastObject = false;
                if (position == NULL) {
                    delPosition = m_elementList.GetTailPosition();
                    bLastObject = true;
                }
                else {
                    m_elementList.GetPrev(delPosition);
                }
                if (delPosition) {
                    m_elementList.RemoveAt(delPosition);
                }
                // update timestamps of "regular" pages

                // if the deleted one was not the head, change end timestamp
                // The delete one was the tail
                POSITION prevPos = position;
                if (bLastObject) 
                    prevPos = m_elementList.GetTailPosition();
                else
                    m_elementList.GetPrev(prevPos);
            
                CStructureInformation *pElementToChange = NULL;
                if (prevPos != NULL)
                    pElementToChange = m_elementList.GetAt(prevPos);

                if (pElementToChange != NULL)
                    pElementToChange->SetEnd(iEndTimestamp);

                // The delete one was the first one
                // Change the begin of the head
                if (pElementToChange == NULL) {
                    pElementToChange = m_elementList.GetHead();
                    if (pElementToChange)
                        pElementToChange->SetBegin(iBeginTimestamp);
                }
            } 
        } else {
            pElement->DeleteClickPages();
        }
    }

    return;
}

bool CStructureInformation::HasClickPages( bool bIncludeAnalysisPagesWithClicks ) {  
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    // This function was only called for the automatically created pages 
    // of a pure screen grabbing recording
    // Therefore the pages are all in the same chapter
    if (m_elementList.IsEmpty())
        return bReturn;

    POSITION position = m_elementList.GetHeadPosition();
    while (position != NULL && !bReturn) {
        CStructureInformation *pElement = m_elementList.GetNext(position);
        if (pElement != NULL && pElement->GetType() == PAGE) {
            if (((CPageInformation *)pElement)->GetPageType() == "click")
                return true;
            if (bIncludeAnalysisPagesWithClicks && !((CPageInformation *)pElement)->GetPageClickButton().IsEmpty())
               return true;
        } else
            bReturn = pElement->HasClickPages(bIncludeAnalysisPagesWithClicks);
    }

    return bReturn;
}

bool CStructureInformation::GetLastNotClickTimestamp(int &iTimestamp) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bShouldBeInHere = m_iBegin <= iTimestamp && m_iEnd > iTimestamp;
    bool bIsNoClickPage = GetType() == PAGE && ((CPageInformation *)this)->GetPageType() != _T("click");

    // If the timestamp is part of a "regular" page, do not change the timestamp.
    if (bShouldBeInHere && bIsNoClickPage)
        return true;

    bool bReturn = false;
    if (!m_elementList.IsEmpty()) {
        POSITION position = m_elementList.GetTailPosition();
        while (position != NULL) {
            CStructureInformation *pElement = m_elementList.GetPrev(position);
            if (pElement != NULL) {
                bReturn = pElement->GetLastNotClickTimestamp(iTimestamp);
                if (!bReturn && iTimestamp >= pElement->GetEnd()) {
                    bIsNoClickPage = pElement->GetType() == PAGE && 
                                     ((CPageInformation *)pElement)->GetPageType() != _T("click");
                    if (bIsNoClickPage) {
                        iTimestamp = pElement->GetEnd()-1;
                        bReturn = true;
                    }
                }
            }
            if (bReturn)
                break;
        }
    }

    return bReturn;
}

bool CStructureInformation::FindFirstNotClickTimestamp(int &iTimestamp) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bIsNoClickPage = GetType() == PAGE && ((CPageInformation *)this)->GetPageType() != _T("click");

    // If the timestamp is part of a "regular" page, do not change the timestamp.
    if (bIsNoClickPage) {
        iTimestamp = GetBegin();
        return true;
    }

    bool bReturn = false;
    if (!m_elementList.IsEmpty()) {
        POSITION position = m_elementList.GetHeadPosition();
        while (position != NULL && !bReturn) {
            CStructureInformation *pElement = m_elementList.GetNext(position);
            if (pElement != NULL)
                bReturn = pElement->FindFirstNotClickTimestamp(iTimestamp);
        }
    }

    return bReturn;
}

void CStructureInformation::GetAllPageTimestamps(CList<int> &lstPageTimestamps) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (GetType() == PAGE)
        lstPageTimestamps.AddTail(GetBegin());

    if (!m_elementList.IsEmpty()) {
        POSITION position = m_elementList.GetHeadPosition();
        while (position != NULL) {
            CStructureInformation *element = m_elementList.GetNext(position);
            if (element)
                element->GetAllPageTimestamps(lstPageTimestamps);
        }
    }
}

void CStructureInformation::Write(CFileOutput *pOutput, LPCTSTR prefix, CArray<UINT, UINT> *paExportClipTimes, int level)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (pOutput == NULL)
      return;

   if (!m_elementList.IsEmpty())
   {
      POSITION position = m_elementList.GetHeadPosition();
      while (position != NULL)
      {
         CStructureInformation *element = m_elementList.GetNext(position);
         if (element)
            element->Write(pOutput, prefix, paExportClipTimes, level);
      }
   }
}

void CStructureInformation::WriteDenverEntry(CFileOutput *pOutput, LPCTSTR prefix, int level)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (pOutput == NULL)
      return;

   CList<CClipInformation *, CClipInformation *> clipList;
   GetClips(clipList);

   if (clipList.IsEmpty())
      return;

   CClipInformation *pFirstClip = clipList.GetHead();
   CClipInformation *pLastClip = clipList.GetTail();

   CString tabString;
   CString csSgmlString;
   for (int i = 0; i < level+1; ++i)
      tabString += _T("  ");

   CString line;
   line.Format(_T("%s<clip>\n"), tabString);
   
   // Double code,
   // see CClipInformation::Write()
   pOutput->Append(line);

   StringManipulation::TransformForSgml(pFirstClip->GetTitle(), csSgmlString);
   line.Format(_T("%s  <title>%s</title>\n"), tabString, csSgmlString);
   pOutput->Append(line);
   line.Format(_T("%s  <nr>%d</nr>\n"), tabString, 0);
   pOutput->Append(line);
   line.Format(_T("%s  <begin>0</begin>\n"), tabString);
   pOutput->Append(line);
   line.Format(_T("%s  <end>%d</end>\n"), tabString, pLastClip->GetEnd());
   pOutput->Append(line);
   StringManipulation::TransformForSgml(prefix, csSgmlString);
   // remove any filename reserved characters from the filename
   CString thumbFile = csSgmlString;
   StringManipulation::RemoveReservedFilenameChars(thumbFile);
   thumbFile.Replace(_T(" "), _T("_"));
   line.Format(_T("%s  <thumb>%s_clip.png</thumb>\n"), tabString, thumbFile);
   pOutput->Append(line);

   CStringList slKeywords;
   POSITION position = clipList.GetHeadPosition();
   while (position)
   {
      CClipInformation *pClip = clipList.GetNext(position);
      CStringList clipKeywords;
      pClip->GetKeywords(clipKeywords);
      if (!clipKeywords.IsEmpty())
         slKeywords.AddTail(&clipKeywords);
   }

   if (!slKeywords.IsEmpty())
   {
      POSITION position = slKeywords.GetHeadPosition();
      while (position)
      {
         CString keyword = slKeywords.GetNext(position);
         StringManipulation::TransformForSgml(keyword, csSgmlString);
         line.Format(_T("%s  <keyword>%s</keyword>\n"), tabString, csSgmlString);
         pOutput->Append(line);
      }
   }

   line.Format(_T("%s</clip>\n"), tabString);
   pOutput->Append(line);



}

int CStructureInformation::GetHeight(int y, int height)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_elementList.GetHeadPosition();
   
   int zy = y;
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
         element->GetNextY(zy, height);
   }

   return zy;
}

int CStructureInformation::GetTimestamp(int y, int &actY, int height)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int timestamp = -1;

   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
      {
         int timestamp = element->GetTimestamp(y, actY, height);
         if (timestamp >= 0)
            return timestamp;
      }
   }

   return timestamp;
}

void CStructureInformation::GetNextY(int &y, int height)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
         element->GetNextY(y, height);
   }
}

int CStructureInformation::GetDrawHeight(int pageHeight)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return 0;
}

bool CStructureInformation::GetYPos(int &y, int height, int timestamp)
{  
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_elementList.IsEmpty())
      return false;

   POSITION position = m_elementList.GetHeadPosition();
   if (m_iBegin <= timestamp)
   {
      y += GetDrawHeight(height);
      while (position)
      {
         CStructureInformation *element = m_elementList.GetNext(position);
         if (element)
         {  
            if (element->GetType() == CHAPTER || element->GetType() == BASE)
            {  
               bool bPosFound = element->GetYPos(y, height, timestamp);
               if (bPosFound)
                  return true;
            }
            else 
            {
               if (element->GetEnd() < timestamp)
                  y += element->GetDrawHeight(height);
               else
                  return true;
            }
         }
      }
   }

   return false;
}

int CStructureInformation::GetActivePage(int timestamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_elementList.IsEmpty())
      return -1;

   POSITION position = m_elementList.GetTailPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetPrev(position);
      if (element)
      {
         int activePage = element->GetActivePage(timestamp);
         if (activePage >= 0)
            return activePage;
      }
   }

   // active page is in this chapter
   if (timestamp >= m_iBegin && timestamp < m_iEnd)
   {
      CPageInformation *page = FindFirstPage();
      if (page != NULL && timestamp < page->GetBegin())
         return page->GetInternalNumber();
   }

   return -1;
}

void CStructureInformation::Draw(CDC *pDC, int &x, int &y, int pageWidth, int pageHeight, int viewWidth, int activePage, int lastActivePage, LOGFONT &logFont, DrawSdk::ZoomManager *zoom)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // Note: drawing is not recursive anymore; use GetAllElements() and call Draw() on each.
}

Gdiplus::Image * CStructureInformation::LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance /*=NULL*/)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   Gdiplus::Image * pImage = NULL;

   if( lpszType == RT_BITMAP )
   {
      HBITMAP hBitmap = ::LoadBitmap( hInstance, MAKEINTRESOURCE(nID) );
      pImage = (Gdiplus::Image*)Gdiplus::Bitmap::FromHBITMAP(hBitmap, 0);
      ::DeleteObject(hBitmap);
      return pImage;
   }		

   hInstance = (hInstance == NULL) ? ::AfxGetResourceHandle() : hInstance;
   HRSRC hRsrc = ::FindResource ( hInstance, MAKEINTRESOURCE(nID), lpszType); 
   ASSERT(hRsrc != NULL);

   DWORD dwSize = ::SizeofResource( hInstance, hRsrc);
   LPBYTE lpRsrc = (LPBYTE)::LoadResource( hInstance, hRsrc);
   ASSERT(lpRsrc != NULL);

   HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
   if ( hMem )
   {
      LPBYTE pMem = (LPBYTE)::GlobalLock(hMem);
      if ( pMem )
      {
         memcpy( pMem, lpRsrc, dwSize);
         IStream * pStream = NULL;
         ::CreateStreamOnHGlobal( hMem, FALSE, &pStream);

         if ( pStream != NULL )
         {
            pImage = Gdiplus::Image::FromStream(pStream);
            pStream->Release();
         }
         ::GlobalUnlock(hMem);
      }
      ::GlobalFree(hMem);
   }
   ::FreeResource(lpRsrc);

   return pImage;
}

void CStructureInformation::Increment(int timeOffset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iBegin += timeOffset;
   m_iEnd   += timeOffset;

   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
         element->Increment(timeOffset);
   }
}

void CStructureInformation::CutBefore(int timeStamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_iBegin < timeStamp)
      m_iBegin = timeStamp;

   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
         element->CutBefore(timeStamp);
   }
}

void CStructureInformation::CutAfter(int timeStamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_iEnd > timeStamp)
      m_iEnd = timeStamp;
   
   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
         element->CutAfter(timeStamp);
   }
}

bool CStructureInformation::IsEmpty()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element && !element->IsEmpty())
         return false;
         
   }

   return true;
}

CPageInformation *CStructureInformation::FindFirstPage()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (this->GetType() == PAGE)
      return (CPageInformation *)this;

   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      CPageInformation *page = element->FindFirstPage();
      if (page) 
         return page;
   }

   return NULL;
}

CPageInformation *CStructureInformation::FindLastPage()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (this->GetType() == PAGE || this->GetType() == CLIP)
      return (CPageInformation *)this;

   POSITION position = m_elementList.GetTailPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetPrev(position);
      CPageInformation *page = element->FindLastPage();
      if (page) 
         return page;
   }

   return NULL;
}

void CStructureInformation::GetClips(CList<CClipInformation *, CClipInformation*> &clipList)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (this->GetType() == CLIP)
      clipList.AddTail((CClipInformation *)this);
   else
   {
      POSITION position = m_elementList.GetHeadPosition();
      while (position)
      {
         CStructureInformation *element = m_elementList.GetNext(position);
         element->GetClips(clipList);
      }
   }

   return;
}

CStructureInformation *CStructureInformation::FindFirstElement()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_elementList.IsEmpty())
      return NULL;

   CStructureInformation *element = m_elementList.GetHead();

   return element;
}


CStructureInformation *CStructureInformation::FindLastElement()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_elementList.IsEmpty())
      return NULL;

   CStructureInformation *element = m_elementList.GetTail();

   return element;
}

bool CStructureInformation::InsertMissingClips(CArray<CClipInfo *, CClipInfo *> &clipStream)
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   
   if (clipStream.GetSize() == 0)
      return true;

   CArray<CClipInfo *, CClipInfo *> mergedClipStream;

   CClipInfo *lastInsertedClip = NULL;
   int i = 0;
   for (i = 0; i < clipStream.GetSize(); ++i)
   {
      CClipInfo *pClipInfo = clipStream[i];
      // If the filenames are equal and the new clip begins directly after 
      // the previous one, it is another part of the same clip and only the length has to be adjusted.
      // Otherwise the clip has to be added to the new list.
      if (lastInsertedClip &&
         _tcscmp(pClipInfo->GetFilename(), lastInsertedClip->GetFilename()) == 0 &&
         pClipInfo->GetTimestamp() == lastInsertedClip->GetTimestamp() + lastInsertedClip->GetLength())
      {
         int newClipLength = lastInsertedClip->GetLength() + pClipInfo->GetLength();
         lastInsertedClip->SetLength(newClipLength);
      }
      else
      {
         mergedClipStream.Add(pClipInfo);
         lastInsertedClip = pClipInfo;
      }
          
   }

   for (i = 0; i < mergedClipStream.GetSize(); ++i)
   {
      CClipInfo *pClipInfo = mergedClipStream[i];

      //PZI: only add unstructured clips
      if(!(pClipInfo->GetStructureAvailable())) {

      CClipInformation *pNewClip = new CClipInformation();
      pNewClip->SetBegin(pClipInfo->GetTimestamp());
      pNewClip->SetEnd(pClipInfo->GetTimestamp() + pClipInfo->GetLength());
      pNewClip->SetFilename(pClipInfo->GetFilename());
      pNewClip->SetTitle(pClipInfo->GetTitle());
      CStringList slKeywords;
      pClipInfo->GetKeywords(slKeywords);
      pNewClip->SetKeywords(slKeywords);
      slKeywords.RemoveAll();

      POSITION position = m_elementList.GetHeadPosition();
      bool bClipIsInserted = false;
      while (position && !bClipIsInserted)
      {
         CStructureInformation *element = m_elementList.GetNext(position);
         // BUGFIX 5129: Inconsistent LMD on Export
         // the inconsistency is created here while loading
         // occurs since the introduction of the new type BASE for the outer <structure> element
         // but the condiontalin here still texted for the previously used type CHAPTER
         if (element && (element->GetType() == CHAPTER || element->GetType() == BASE))
         {
            // The clip begins in this element if the timestamp of the clip is 
            // between the begin and the emd of the chapter
            if (pClipInfo &&
                pClipInfo->GetTimestamp() >= element->GetBegin() && 
                pClipInfo->GetTimestamp() < element->GetEnd())
            {
              if (!element->InsertClip(pNewClip))
               {
                  delete pNewClip;
                  pNewClip = NULL;
               }
               bClipIsInserted = true;
            }
         }
      }
      }
   }

   return true;
}

bool CStructureInformation::InsertClip(CClipInformation *pClip)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   // Clip does not begin in this chapter
   if (pClip->GetBegin() < m_iBegin || pClip->GetBegin() >= m_iEnd)
      return false;

   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetAt(position);
      if (element)
      {
         if (pClip->GetBegin() >= element->GetBegin() && pClip->GetBegin() < element->GetEnd())
         {
            if (element->GetType() == PAGE)
            {
               // if there are clips beginning on this page search 
               // the correct insert position
               POSITION clipPosition = position;
               // this is the next element
               m_elementList.GetNext(clipPosition);
               bool bPositionFound = false;
               while (clipPosition && !bPositionFound)
               {
                  // the next element is a clip and begins on actual page (element)
                  CStructureInformation *clipElement = m_elementList.GetAt(clipPosition);
                  if (clipElement->GetType() == CLIP &&
                      clipElement->GetBegin() >= element->GetBegin() && 
                      clipElement->GetBegin() < element->GetEnd())
                  {
                     // new clip will be inserted before this clip
                     if (clipElement->GetBegin() > pClip->GetBegin())
                     {
                        m_elementList.GetPrev(clipPosition);
                        position = clipPosition;
                        bPositionFound = true;
                     }
                     else // new clip will be inserted after this clip,
                          // but this clip have not to be the last
                     {
                        position = clipPosition;
                        m_elementList.GetNext(clipPosition);
                     }
                  }
                  else
                     bPositionFound = true;
               }

               m_elementList.InsertAfter(position, pClip);
               return true;
            }
            else if (element->GetType() == CHAPTER || element->GetType() == BASE)
            {
               return element->InsertClip(pClip);
            }
         }
         else if (pClip->GetBegin() < element->GetBegin())
         {
            m_elementList.InsertBefore(position, pClip);
            return true;
         }

      }
      m_elementList.GetNext(position);
   }

   // clip is part of this element but 
   // not on a page or in a sub chapter.
   if (GetType() == CHAPTER || GetType() == BASE)
   {
      if (m_elementList.IsEmpty())
         m_elementList.AddHead(pClip);
      else
      {
         CStructureInformation *element = m_elementList.GetTail();
         if (element->GetEnd() <= pClip->GetBegin())
            m_elementList.AddTail(pClip);
         else
            m_elementList.AddHead(pClip);

      }
      return true;
   }


   return false;
}

/*
CStructureInformation *CStructureInformation::FindElement(int timestamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CStructureInformation *ret = NULL;

   if (!m_elementList.IsEmpty())
   {
      POSITION position = m_elementList.GetHeadPosition();
      while (position != NULL)
      {
         CStructureInformation *element = m_elementList.GetNext(position);
         if (element)
            ret = element->FindElement(timestamp);
         if (ret)
            break;
      }
   }

   return NULL;
}
*/

CStructureInformation *CStructureInformation::FindElementWithId(int id)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (id == m_iUniqueId)
      return this;

   CStructureInformation *ret = NULL;
   if (!m_elementList.IsEmpty())
   {
      POSITION position = m_elementList.GetHeadPosition();
      while (position != NULL)
      {
         CStructureInformation *element = m_elementList.GetNext(position);
         if (element)
            ret = element->FindElementWithId(id);
         if (ret)
            break;
      }
   }

   return ret;
}

CStructureInformation *CStructureInformation::FindElement(int iTimestamp, type nType)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   bool bShouldBeInHere = m_iBegin <= iTimestamp && m_iEnd > iTimestamp;

   if (GetType() == nType && bShouldBeInHere)
   {
      return this;
   }

   CStructureInformation *pReturn = NULL;
   if (!m_elementList.IsEmpty())
   {
      POSITION position = m_elementList.GetHeadPosition();
      while (position != NULL)
      {
         CStructureInformation *pElement = m_elementList.GetNext(position);
         if (pElement != NULL)
         {
            pReturn = pElement->FindElement(iTimestamp, nType);

            if (pReturn == NULL && iTimestamp < pElement->GetBegin() && pElement->GetType() == nType)
               return pElement; // some pages do not start at the beginning of the segment/chapter
         }
         if (pReturn != NULL)
            break;
      }
   }

   return pReturn;
}

void CStructureInformation::Append(CStructureInformation *newElement)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_elementList.AddTail(newElement);
}

CStructureInformation *CStructureInformation::Copy()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CStructureInformation *newStructure = new CStructureInformation();
   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
      {
         newStructure->Append(element->Copy());
      }
   }
   
   newStructure->SetBegin(m_iBegin);
   newStructure->SetEnd(m_iEnd);
   newStructure->SetUniqueId(m_iUniqueId);

   newStructure->m_pCopySource = this;

   return newStructure;
}

void CStructureInformation::Fill(CStructureInformation *pStructureInformation)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (!pStructureInformation)
      return;

   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
      {
         pStructureInformation->Append(element->Copy());
      }
   }
   
   pStructureInformation->SetBegin(m_iBegin);
   pStructureInformation->SetEnd(m_iEnd);
   pStructureInformation->SetUniqueId(m_iUniqueId);
}

void CStructureInformation::Copy(CList<CStructureInformation *, CStructureInformation *> &elementList,
                                 int from, int to, int timeOffset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CStructureInformation *newStructure = new CStructureInformation();
   newStructure->SetBegin(m_iBegin + timeOffset);
   newStructure->SetEnd(m_iEnd + timeOffset);
   newStructure->SetUniqueId(m_iUniqueId);

   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element && element->GetType() != CLIP &&
          (element->GetEnd() > from) && (element->GetBegin() < to))
      {
         element->Copy(newStructure->m_elementList, from, to, timeOffset);
      }
   }

   newStructure->m_pCopySource = this;

   elementList.AddTail(newStructure);
}

void CStructureInformation::CopyPages(CList<CStructureInformation *, CStructureInformation *> &elementList, int from, int to, int timeOffset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_elementList.IsEmpty())
      return;

   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element &&  element->GetType() != CLIP &&
          (element->GetEnd() > from) && (element->GetBegin() < to))
      {
         element->CopyPages(elementList, from, to, timeOffset);
      }
   }
}

bool CStructureInformation::DeleteRemovedClips(CArray<CClipInfo *, CClipInfo *> &clipStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_elementList.IsEmpty())
      return false;

   bool bHasDeleted = false;
   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
      {
         if (element->GetType() == CLIP)
         {
    
            bool bClipIsInArray = false;
            for (int i = 0; i < clipStream.GetSize(); ++i)
            {
               CClipInfo *pClipInfo = clipStream[i];
               if (pClipInfo && (pClipInfo->GetTimestamp() == element->GetBegin()) &&
                   _tcscmp(pClipInfo->GetFilename(), ((CClipInformation *) element)->GetFilename()) == 0)
               {
                  bClipIsInArray = true;
                  break;
               }
            }

            if (!bClipIsInArray)
            {
               delete element;
               POSITION delPosition = position;
               if (position == NULL)
                  delPosition = m_elementList.GetTailPosition();
               else
                  m_elementList.GetPrev(delPosition);
               if (delPosition)
               {
                  m_elementList.RemoveAt(delPosition);
                  bHasDeleted = true;
               }
            }
         }
         else
         {
            element->DeleteRemovedClips(clipStream);
         }
      }
   }

   return bHasDeleted;

}

void CStructureInformation::UpdatePageNumbers(int &number, CMap<int, int, int, int> &mapUniquePageIds)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int iUsed = 0;
   bool bContains = mapUniquePageIds.Lookup(GetUniqueId(), iUsed) == TRUE;
   if (bContains)
      ChangeUniqueIds();

   mapUniquePageIds.SetAt(GetUniqueId(), 1);

   if (m_elementList.IsEmpty())
      return;

   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
         element->UpdatePageNumbers(number, mapUniquePageIds);
   }
}

void CStructureInformation::UpdateTimestamps(int initialBegin)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_elementList.IsEmpty())
      return;

   POSITION position = m_elementList.GetHeadPosition();
   CStructureInformation *previousElement = NULL;
   while (position != NULL)
   {
      CStructureInformation *actualElement = m_elementList.GetNext(position);
      if (actualElement)
      {
         if (actualElement->GetType() == CHAPTER || actualElement->GetType() == BASE)
         {
            // TODO evil: end time is also the begin time of the next entity (make it -1)
            // However: end time ist meant exclusiv but it causes problems anyway (see Bug #2376)
            if (previousElement)
               actualElement->SetBegin(previousElement->GetEnd());
            else
               actualElement->SetBegin(initialBegin);
         }
         actualElement->UpdateTimestamps(actualElement->GetBegin());
         if (actualElement->GetType() != CLIP)
            previousElement = actualElement;
      }
   }
}

void CStructureInformation::ResetClipThumb()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_elementList.IsEmpty())
      return;

   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
         element->ResetClipThumb();
   }
}


int CStructureInformation::GetFirstTime()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int iFirstTime = GetBegin();
   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (iFirstTime < 0 || iFirstTime > element->GetFirstTime())
         iFirstTime = element->GetFirstTime();
   }

   return iFirstTime;
}

int CStructureInformation::GetLastTime()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   //int iLastTime = GetEnd();
   int iLastTime = 0;
   if (GetType() == CLIP)
      iLastTime = GetFirstTime();
   else
      iLastTime = GetEnd();
 
   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element->GetType() != CLIP)
      {
         if (iLastTime < element->GetLastTime())
            iLastTime = element->GetLastTime();
      }
   }

   return iLastTime;
}

void CStructureInformation::Print(int iLevel)
{
   for (int i=0; i<iLevel; ++i)
      TRACE(_T("  "));

   TRACE(_T("STRUCT %d-%d %d\n"), GetBegin(), GetEnd(), GetUniqueId());

   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *pElement = m_elementList.GetNext(position);

      pElement->Print(iLevel + 1);
   }
}


/*********************************************
 *********************************************/

CChapterInformation::CChapterInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init();
}

CChapterInformation::~CChapterInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

void CChapterInformation::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iNumber = -1;
}

void CChapterInformation::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_csTitle.Empty();
   m_iNumber = -1;
}

bool CChapterInformation::Fill(SgmlElement *pChapterTag, bool &bFirstPage) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (!pChapterTag)
        return false;

    m_csTitle = pChapterTag->GetValue(_T("title"));

    CString szInt = pChapterTag->GetValue(_T("nr"));
    if (!szInt.IsEmpty())
        m_iNumber = _ttoi(szInt);

    // a chapter may contain elements
    CStructureInformation::Fill(pChapterTag, bFirstPage);
    return true;
}

bool CChapterInformation::FillWithNewPage(CString &csPageTitle, UINT nLengthMs)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CPageInformation *pPage = new CPageInformation();
   pPage->FillWithNewPage(csPageTitle, nLengthMs);
   pPage->SetUniqueId(m_iIdCounter);
   ++m_iIdCounter;

   m_elementList.AddTail(pPage);
 

   m_csTitle = csPageTitle;
   m_iNumber = 0;

   m_iBegin = 0;
   m_iEnd = nLengthMs;

   return true;
}

void CChapterInformation::Write(CFileOutput *pOutput, LPCTSTR prefix, CArray<UINT, UINT> *paExportClipTimes, int level)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_elementList.IsEmpty())
      return;

   if (pOutput == NULL)
      return;

   CString csSgmlString;
   CString tabString;
   for (int i = 0; i < level; ++i)
      tabString += _T("  ");

   StringManipulation::TransformForSgml(m_csTitle, csSgmlString);
   CString line;
   line.Format(_T("%s<chapter title=\"%s\" nr=\"%d\">\n"), tabString, csSgmlString, m_iNumber);

   pOutput->Append(line);
   
   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
         element->Write(pOutput, prefix, paExportClipTimes, level+1);
   }
  
   line.Format(_T("%s</chapter>\n"), tabString);
   pOutput->Append(line);

}

void CChapterInformation::Copy(CList<CStructureInformation *, CStructureInformation *> &elementList,
                               int from, int to, int timeOffset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CChapterInformation *newChapter = new CChapterInformation();
   newChapter->SetTitle(m_csTitle);
   newChapter->SetNumber(m_iNumber);
   newChapter->SetBegin(m_iBegin + timeOffset);
   newChapter->SetEnd(m_iEnd + timeOffset);
   newChapter->SetUniqueId(m_iUniqueId);


   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element && element->GetType() != CLIP && 
          (element->GetEnd() > from) && (element->GetBegin() < to))
      {
         element->Copy(newChapter->m_elementList, from, to, timeOffset);
      }
   }

   newChapter->m_pCopySource = this;
   
   elementList.AddTail(newChapter);
}

CStructureInformation *CChapterInformation::Copy()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CChapterInformation *newChapter = new CChapterInformation();
   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
      {
         newChapter->Append(element->Copy());
      }
   }
   
   newChapter->SetTitle(m_csTitle);
   newChapter->SetNumber(m_iNumber);
   newChapter->SetBegin(m_iBegin);
   newChapter->SetEnd(m_iEnd);
   newChapter->SetUniqueId(m_iUniqueId);

   newChapter->m_pCopySource = this;

   return newChapter;
}

int CChapterInformation::GetTimestamp(int y, int &actY, int height)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int timestamp = -1;

   actY += 20;
   
   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
      {
         int timestamp = element->GetTimestamp(y, actY, height);
         if (timestamp >= 0)
            return timestamp;
      }
   }

   return timestamp;
}

void CChapterInformation::GetNextY(int &y, int height)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   y += GetDrawHeight(height);
   
   POSITION position = m_elementList.GetHeadPosition();
   while (position)
   {
      CStructureInformation *element = m_elementList.GetNext(position);
      if (element)
         element->GetNextY(y, height);
   }
}

int CChapterInformation::GetDrawHeight(int pageHeight)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return 20;
}

void CChapterInformation::Draw(CDC *pDC, int &x, int &y, int pageWidth, int pageHeight, int viewWidth, int activePage, int lastActivePage, LOGFONT &logFont, DrawSdk::ZoomManager *zoom)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CRect rect(0, y-5, viewWidth, y + 15);
   
   CBrush brush(RGB(207, 223, 240)/*(255, 255, 255)*/);
   pDC->FillRect(&rect, &brush);
   brush.DeleteObject();

   pDC->SetBkMode(TRANSPARENT);
   pDC->SetTextColor(RGB(155, 173, 198)/*(0, 0, 0)*/);
   
   CFont font;
   logFont.lfWeight = FW_BOLD;
   font.CreateFontIndirect(&logFont);

   CFont *oldFont = (CFont *)pDC->SelectObject(&font);

   pDC->DrawText(m_csTitle, CRect(x, y, viewWidth, y+15), DT_LEFT);

   if (oldFont)
      pDC->SelectObject(oldFont);
   font.DeleteObject();

   y += 20 + 10;

   // Note: drawing is not recursive anymore; use GetAllElements() and call Draw() on each.
}
 

void CChapterInformation::Print(int iLevel)
{
   for (int i=0; i<iLevel; ++i)
      TRACE(_T("  "));

   TRACE(_T("CHAPT %d-%d %s %d\n"), GetBegin(), GetEnd(), (LPCTSTR)m_csTitle, GetUniqueId());

   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *pElement = m_elementList.GetNext(position);

      pElement->Print(iLevel + 1);
   }
}

/*********************************************
 *********************************************/

CPageInformation::CPageInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init(); 
}

CPageInformation::~CPageInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

void CPageInformation::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iNumber = -1;
   m_iBegin  = -1;
   m_iEnd    = -1;

   m_csClickButton.Empty();
   m_iClickX       = -1;  
   m_iClickY       = -1;

   m_drawEvent = NULL;

   m_nInternalNumber = -1;
}

void CPageInformation::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iNumber = -1;
   m_iBegin  = -1;
   m_iEnd    = -1;
   m_csTitle.Empty();
   m_csThumb.Empty();
   m_slKeywords.RemoveAll();
   
   m_drawEvent = NULL;

   m_nInternalNumber = -1;
}

bool CPageInformation::Fill(SgmlElement *pPageTag, bool &bFirstPage) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (pPageTag == NULL)
        return false;

    CArray<SgmlElement *, SgmlElement *> arElements;
    pPageTag->GetElements(arElements);
    if (!arElements.IsEmpty()) {
        for (int i = 0; i < arElements.GetSize(); ++i) {
            SgmlElement *pElement = arElements[i];
            if (pElement != NULL) {
                CString csName = pElement->GetName();
                if (csName == _T("title")) {
                    m_csTitle = pElement->GetParameter();
                } else if (csName == _T("nr")) {
                    CString szInt = pElement->GetParameter();
                    if (!szInt.IsEmpty())
                        m_iNumber = _ttoi(szInt);
                } else if (csName == _T("type")) {
                    CString csType = pElement->GetParameter();
                    if (!csType.IsEmpty()) {
                        m_csType = csType;
                        if (pElement->GetValue(_T("button")) != NULL) {
                            m_csClickButton = pElement->GetValue(_T("button"));
                            m_iClickX = pElement->GetIntValue(_T("x"));
                            m_iClickY = pElement->GetIntValue(_T("y"));
                        }
                    }
                } else if (csName == _T("begin")) {
                    CString szInt = pElement->GetParameter();
                    if (!szInt.IsEmpty())
                        m_iBegin = _ttoi(szInt);

                    if (bFirstPage) {
                        if (m_iBegin > 0) {
                            m_iBegin = 0;

                            // Bugfix #3086:
                            // make the first page in a segment start at the segment boundary (event time is also corrected)
                        }
                        bFirstPage = false;
                    }

                } else if (csName == _T("end")) {
                    CString szInt = pElement->GetParameter();
                    if (!szInt.IsEmpty())
                        m_iEnd = _ttoi(szInt);
                } else if (csName == _T("thumb")) {
                    m_csThumb = pElement->GetParameter();
                } else if (csName == _T("keyword")) {
                    m_slKeywords.AddTail(pElement->GetParameter());
                }
            }
        }
    } else {
        if (pPageTag->GetParameter())
            m_csTitle = pPageTag->GetParameter();
        m_iNumber = pPageTag->GetIntValue(_T("nr"));
        m_iBegin = pPageTag->GetIntValue(_T("begin"));
        m_iEnd = pPageTag->GetIntValue(_T("end"));
        m_csThumb = pPageTag->GetValue(_T("thumb"));
    }

    return true;
}

bool CPageInformation::FillWithNewPage(CString &csPageTitle, UINT nLengthMs)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_csTitle = csPageTitle;
   m_iNumber = 1;
   m_iBegin = 0;
   m_iEnd = nLengthMs;

   m_csThumb = _T("new_page.png");

   return true;
}

bool CPageInformation::Complete(CWhiteboardStream *whiteboardStream, CPageStream *pageStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (whiteboardStream)
      m_drawEvent = whiteboardStream->GetEvent(m_iEnd-1);
   else
      m_drawEvent = NULL;

   if (pageStream)
   {
      CPage *page = pageStream->FindPage(GetBegin());
      // may be the value of the first page in lmd file differs from 
      // begin in event queue
      if (page == NULL && 
          pageStream->GetPage(0) != NULL && pageStream->GetPage(0)->GetBegin() > GetBegin())
          page = pageStream->GetPage(0);

      if (page)
      {
         page->SetTitle(GetTitle());
         CString csKeywords;
         GetKeywords(csKeywords);
         page->SetKeywords(csKeywords);
      }
   }

   return true;
}

void CPageInformation::UpdateMetadata(CPageStream *pPageStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (pPageStream)
   {
      CPage *page = pPageStream->FindPage(GetBegin()); 
      // may be the value of the first page in lmd file differs from 
      // begin in event queue
      if (page == NULL && 
          pPageStream->GetPage(0) != NULL && pPageStream->GetPage(0)->GetBegin() > GetBegin())
          page = pPageStream->GetPage(0);

      if (page)
      {
         SetTitle(page->GetTitle());
         CString csKeywords;
         page->GetKeywords(csKeywords);
         SetKeywords(csKeywords);
      }
   }

   return;
}

bool CPageInformation::IsEmpty()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return false;
}

void CPageInformation::Copy(CList<CStructureInformation *, CStructureInformation *> &elementList, 
                            int from, int to, int timeOffset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CPageInformation *newPage = new CPageInformation();
   newPage->SetTitle(m_csTitle);
   newPage->SetNumber(m_iNumber);
   newPage->SetBegin(m_iBegin + timeOffset);
   newPage->SetEnd(m_iEnd + timeOffset);
   newPage->SetUniqueId(m_iUniqueId);
   newPage->SetThumb(m_csThumb);
   newPage->SetKeywords(m_slKeywords);
   newPage->SetDrawEvent(m_drawEvent);
   newPage->SetPageType(m_csType);
   newPage->SetPageClickButton(m_csClickButton);
   newPage->SetPageClickX(m_iClickX);
   newPage->SetPageClickY(m_iClickY);

   newPage->m_pCopySource = this;

   elementList.AddTail(newPage);
}

CStructureInformation *CPageInformation::Copy()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CPageInformation *newPage = new CPageInformation();
   newPage->SetTitle(m_csTitle);
   newPage->SetNumber(m_iNumber);
   newPage->SetBegin(m_iBegin);
   newPage->SetEnd(m_iEnd);
   newPage->SetUniqueId(m_iUniqueId);
   newPage->SetThumb(m_csThumb);
   newPage->SetKeywords(m_slKeywords);
   newPage->SetDrawEvent(m_drawEvent);
   newPage->SetPageType(m_csType);
   newPage->SetPageClickButton(m_csClickButton);
   newPage->SetPageClickX(m_iClickX);
   newPage->SetPageClickY(m_iClickY);
   
   newPage->m_pCopySource = this;
   
   return newPage;
}


void CPageInformation::Write(CFileOutput *pOutput, LPCTSTR prefix, CArray<UINT, UINT> *paExportClipTimes, int level)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (pOutput == NULL)
      return;

   CString tabString;
   for (int i = 0; i < level; ++i)
      tabString += _T("  ");

   CString csSgmlString;
   CString line;
   line.Format(_T("%s<page>\n"), tabString);
   
   pOutput->Append(line);

   StringManipulation::TransformForSgml(m_csTitle, csSgmlString);
   line.Format(_T("%s  <title>%s</title>\n"), tabString, csSgmlString);
   pOutput->Append(line);
   if (!m_csType.IsEmpty()) {
       if (!m_csClickButton.IsEmpty())
          line.Format(_T("%s  <type x=\"%d\" y=\"%d\" button=\"%s\">%s</type>\n"), tabString, m_iClickX, m_iClickY, m_csClickButton, m_csType);
       else
          line.Format(_T("%s  <type>%s</type>\n"), tabString, m_csType);
       pOutput->Append(line);
   }
   line.Format(_T("%s  <nr>%d</nr>\n"), tabString, m_nInternalNumber);
   pOutput->Append(line);
   line.Format(_T("%s  <begin>%d</begin>\n"), tabString, m_iBegin);
   pOutput->Append(line);
   line.Format(_T("%s  <end>%d</end>\n"), tabString, m_iEnd);
   pOutput->Append(line);
   StringManipulation::TransformForSgml(prefix, csSgmlString);
   // remove any filename reserved characters from the filename
   CString thumbFile = csSgmlString;
   StringManipulation::RemoveReservedFilenameChars(thumbFile);
   thumbFile.Replace(_T(" "), _T("_"));
   line.Format(_T("%s  <thumb>%s_%d_thumb.png</thumb>\n"), tabString, thumbFile, m_iBegin);
   pOutput->Append(line); 
   if (!m_slKeywords.IsEmpty())
   {
      POSITION position = m_slKeywords.GetHeadPosition();
      while (position)
      {
         CString keyword = m_slKeywords.GetNext(position);
         StringManipulation::TransformForSgml(keyword, csSgmlString);
         line.Format(_T("%s  <keyword>%s</keyword>\n"), tabString, csSgmlString);
         pOutput->Append(line);

      }
   }


   line.Format(_T("%s</page>\n"), tabString);
   pOutput->Append(line);

}

/*
CStructureInformation *CPageInformation::FindElement(int timestamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_iBegin > timestamp)
      return this;

   return NULL;
}
*/

int CPageInformation::GetTimestamp(int y, int &actY, int height)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GetNextY(actY, height);

   if (y < actY)
      return m_iBegin;

   return -1;
}

void CPageInformation::GetNextY(int &y, int height)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   y += GetDrawHeight(height);
}

int CPageInformation::GetDrawHeight(int pageHeight)
{
   
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return pageHeight + 30 + 10;
}

int CPageInformation::GetActivePage(int timestamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (timestamp >= m_iBegin && timestamp < m_iEnd)
      return m_nInternalNumber;

   return -1;
}

void CPageInformation::Draw(CDC *pDC, int &x, int &y, int pageWidth, int pageHeight, int viewWidth, int activePage, int lastActivePage, LOGFONT &logFont, DrawSdk::ZoomManager *zoom)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // int endPos = y;
  // if(pageHeight == -1)
  //    pageHeight = 2;
  // GetNextY(endPos, pageHeight);

  // //Graphics gr(pDC->m_hDC);
  ///* HBITMAP bmpBack = LoadBitmap(AfxGetInstanceHandle(), ;*/
  // if (lastActivePage == -1 ||
  //     activePage == m_nInternalNumber || lastActivePage == m_nInternalNumber )
  // {
  //    // Draw rect
  //    

  //    //CRect rect(0, y-5, pageWidth + 10/*viewWidth*/, y + pageHeight + 25);
  //    int nOffset  = (viewWidth <= pageWidth + 15)? 0 : 5;
  //    
  //    CRect rect(5, y-5, viewWidth - nOffset/*pageWidth + 10*//*viewWidth*/, y + pageHeight + 25);
  //    Gdiplus::Graphics graphics(pDC->m_hDC);
  //    COLORREF clr3dRect = RGB(0, 0, 0);
  //    /*CRgn rgn;
  //    rgn.CreateRoundRectRgn(rect.left, rect.top, rect.right, rect.bottom, 10, 10);*/
  //    if (activePage == m_nInternalNumber)
  //    {
  //       //CBrush brush(ACTIVE_PAGE_RGB);
  //       //CBrush brush(ACTIVE_PAGE_HOT_RGB);
  //       ////pDC->FillRect(&rect, &brush);
  //       //pDC->FillRgn(/*&rect*/&rgn, &brush);
  //       ///*pDC->GradientFill(*/
  //       //brush.DeleteObject();
  //       /*HBITMAP hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SLIDE_BKG_SELECTED));
  //       Gdiplus::Bitmap bitmap(hBitmap, NULL);*/
  //       clr3dRect = RGB(235, 176, 57);
  //       Gdiplus::Image* pImage = LoadImage(IDB_SLIDE_BKG_SELECTED, _T("PNG"), ::AfxGetResourceHandle());
  //       //graphics.DrawImage(&bitmap, (Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, (Gdiplus::REAL)rect.Width(), (Gdiplus::REAL)rect.Height());
  //       graphics.DrawImage(pImage, rect.left, rect.top, rect.Width(), rect.Height());
  //    }
  //    else
  //    {
  //       
  //       //CBrush brush(RGB(255, 255, 255));
  //       //CBrush brush(ACTIVE_PAGE_RGB);
  //       ////pDC->FillRect(&rect, &brush);
  //       ////pDC->GradientFill(vert, 
  //       //pDC->FillRgn(/*&rect*/&rgn, &brush);
  //       //brush.DeleteObject();
  //       /*HBITMAP hBitmap = ::LoadBitmap(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SLIDE_BKG_NORMAL));
  //       Gdiplus::Bitmap bitmap(hBitmap, NULL);
  //       graphics.DrawImage(&bitmap, (Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, (Gdiplus::REAL)rect.Width(), (Gdiplus::REAL)rect.Height());*/
  //       clr3dRect = RGB(182, 209, 245);
  //       Gdiplus::Image* pImage = LoadImage(IDB_SLIDE_BKG_NORMAL, _T("PNG"), ::AfxGetResourceHandle());
  //       //graphics.DrawImage(&bitmap, (Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, (Gdiplus::REAL)rect.Width(), (Gdiplus::REAL)rect.Height());
  //       graphics.DrawImage(pImage, rect.left, rect.top, rect.Width(), rect.Height());
  //    }

      if(pageWidth != -1)
      {
         CRect drawRect;
         drawRect.left = x /*+ 5*/;
         drawRect.right = x /*+ 5*/ + pageWidth;
         //if(rect.Width() > pageWidth)
         //{
         //   drawRect.left = rect.left + (rect.Width() - pageWidth)/2;//x;
         //   drawRect.right = drawRect.left + pageWidth;
         //}
         drawRect.top = y;
         drawRect.bottom = y + pageHeight;
         COLORREF clrPageBkg = RGB(255, 255, 255);
         if (m_drawEvent)
            m_drawEvent->Draw(pDC, drawRect, zoom);

         //pDC->Draw3dRect(x, y, pageWidth, pageHeight, RGB(0, 0, 0), RGB(0, 0, 0));
         //pDC->Draw3dRect(drawRect.left, y, pageWidth, pageHeight, clr3dRect, clr3dRect);
         pDC->Draw3dRect(drawRect.left - 1, y - 1, pageWidth + 2, pageHeight +2, RGB(0,0,0), RGB(0,0,0));
      }
      /*CFont font;
      logFont.lfWeight = FW_NORMAL;
      font.CreateFontIndirect(&logFont);

      CFont *oldFont = (CFont *)pDC->SelectObject(&font)*/;

      pDC->SetBkMode(TRANSPARENT);
      /*pDC->SetTextColor(RGB(0, 0, 0));
      pDC->DrawText(m_csTitle, CRect(x, y+pageHeight, viewWidth, y+pageHeight+15), DT_LEFT);*/
     /* pDC->SetTextColor(RGB(155, 173, 198));
      pDC->DrawText(m_csTitle, CRect(x + 5, y+pageHeight + 3, viewWidth, y+pageHeight+18), DT_LEFT);

      if (oldFont)
         pDC->SelectObject(oldFont);
      font.DeleteObject();*/

   //}

   GetNextY(y, pageHeight);

}

void CPageInformation::Increment(int timeOffset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iBegin += timeOffset;
   m_iEnd   += timeOffset;
}

void CPageInformation::CopyPages(CList<CStructureInformation *, CStructureInformation *> &elementList, int from, int to, int timeOffset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
  
   CStructureInformation *copy = Copy();
   if (copy)
   {
      copy->Increment(timeOffset);
      elementList.AddTail(copy);
   }
}

void CPageInformation::UpdatePageNumbers(int &number, CMap<int, int, int, int> &mapUniquePageIds)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int iUsed = 0;
   bool bContains = mapUniquePageIds.Lookup(GetUniqueId(), iUsed) == TRUE;
   if (bContains)
      ChangeUniqueIds();

   mapUniquePageIds.SetAt(GetUniqueId(), 1);

   ++number;
   m_nInternalNumber = number;
}

void CPageInformation::GetKeywords(CStringList &stringList)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_slKeywords.GetHeadPosition();
   while (position)
   {
      CString string = m_slKeywords.GetNext(position);
      stringList.AddTail(string);
   }
}

void CPageInformation::GetKeywords(CString &csKeywords)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = m_slKeywords.GetHeadPosition();
   csKeywords.Empty();
   while (position)
   {
      csKeywords += m_slKeywords.GetNext(position);
      if (position)
         csKeywords += "; ";
   }
}

void CPageInformation::SetKeywords(CStringList &stringList)
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
   
void CPageInformation::SetKeywords(CString &csKeywords)
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

void CPageInformation::Print(int iLevel)
{
   for (int i=0; i<iLevel; ++i)
      TRACE(_T("  "));

   TRACE(_T("PAGE %d-%d %s %d\n"), GetBegin(), GetEnd(), (LPCTSTR)m_csTitle, GetUniqueId());

   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *pElement = m_elementList.GetNext(position);

      pElement->Print(iLevel + 1);
   }
}

/*********************************************
 *********************************************/

CClipInformation::CClipInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init();
}

CClipInformation::~CClipInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

void CClipInformation::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iNumber = -1;
   m_iBegin  = -1;
   m_iEnd    = -1;
   
   m_nThumbWidth  = 0;
   m_nThumbHeight = 0;
   m_bThumIsInitialized = false;
   m_bThumbIsCreated    = false;
}

void CClipInformation::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iNumber = -1;
   m_iBegin  = -1;
   m_iEnd    = -1;
   
   if (m_bThumbIsCreated)
      DeleteObject(m_clipThumb);

   m_nThumbWidth  = 0;
   m_nThumbHeight = 0;
   m_bThumIsInitialized = false;
   m_bThumbIsCreated      = false;

   m_csTitle.Empty();
   m_csThumb.Empty();
   m_csFilename.Empty();
   m_slKeywords.RemoveAll();
}

bool CClipInformation::Fill(SgmlElement *pClipInfoTag, bool &bFirstPage) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (pClipInfoTag == NULL) {
        MessageBox(NULL, _T("Fill: (!infoTag)"), NULL, MB_OK);
        return false;
    }

    CArray<SgmlElement *, SgmlElement *> arElements;
    pClipInfoTag->GetElements(arElements);
    if (!arElements.IsEmpty()) {
        for (int i = 0; i < arElements.GetSize(); ++i) {
            SgmlElement *pElement = arElements[i];
            if (pElement != NULL) {
                CString csName = pElement->GetName();
                if (csName == _T("title")) {
                    m_csTitle = pElement->GetParameter();
                } else if (csName == _T("nr")) {
                    CString szInt = pElement->GetParameter();
                    if (!szInt.IsEmpty())
                        m_iNumber = _ttoi(szInt);
                } else if (csName == _T("begin")) {
                    CString szInt = pElement->GetParameter();
                    if (!szInt.IsEmpty())
                        m_iBegin = _ttoi(szInt);
                } else if (csName == _T("end")) {
                    CString szInt = pElement->GetParameter();
                    if (!szInt.IsEmpty())
                        m_iEnd = _ttoi(szInt);
                } else if (csName == _T("thumb")) {
                    m_csThumb = pElement->GetParameter();
                } else if (csName == _T("keyword")) {
                    m_slKeywords.AddTail(pElement->GetParameter());
                }
            }
        }
    } else {
        if (pClipInfoTag->GetParameter())
            m_csTitle = pClipInfoTag->GetParameter();
        m_iNumber = pClipInfoTag->GetIntValue(_T("nr"));
        m_iBegin = pClipInfoTag->GetIntValue(_T("begin"));
        m_iEnd = pClipInfoTag->GetIntValue(_T("end"));
        m_csThumb = pClipInfoTag->GetValue(_T("thumb"));
    }

    return true; 
}


bool CClipInformation::Complete(CClipStream *clipStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (!clipStream)
      return false;

   CClipInfo *clipInfo = clipStream->FindClipBeginsAt(m_iBegin);
   if (clipInfo)
   {
      clipInfo->SetLength(m_iEnd - m_iBegin);
      if (_tcscmp(clipInfo->GetTitle(), _T("")) == 0)
         clipInfo->SetTitle(m_csTitle);
      CString csKeywords;
      clipInfo->GetKeywords(csKeywords);
      if (csKeywords.IsEmpty())
         clipInfo->SetKeywords(m_slKeywords);
      SetFilename(clipInfo->GetFilename());
   }

   return true;
}


void CClipInformation::Write(CFileOutput *pOutput, LPCTSTR prefix, CArray<UINT, UINT> *paExportClipTimes, int level)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (pOutput == NULL)
      return;

   // This is a bugfix for clips which are too short to be exported.
   // Check if the current clip is in the list of clips which are
   // to be exported. If the clip cannot be found, it will not
   // be written into the LMD file.
   if (paExportClipTimes != NULL)
   {
      bool bFound = false;
      for (int i=0; i<paExportClipTimes->GetSize(); ++i)
      {
         if (paExportClipTimes->GetAt(i) == (unsigned)m_iBegin)
            bFound = true;
      }

      if (!bFound)
         return;
   }
   

   // Double code!
   // See CStructureInformation::Write()
   CString tabString;
   for (int i = 0; i < level; ++i)
      tabString += _T("  ");

   CString csSgmlString;
   CString line;
   line.Format(_T("%s<clip>\n"), tabString);
   
   pOutput->Append(line);

   StringManipulation::TransformForSgml(m_csTitle, csSgmlString);
   line.Format(_T("%s  <title>%s</title>\n"), tabString, csSgmlString);
   pOutput->Append(line);
   line.Format(_T("%s  <nr>%d</nr>\n"), tabString, m_nInternalNumber);
   pOutput->Append(line);
   line.Format(_T("%s  <begin>%d</begin>\n"), tabString, m_iBegin);
   pOutput->Append(line);
   line.Format(_T("%s  <end>%d</end>\n"), tabString, m_iEnd);
   pOutput->Append(line);
   StringManipulation::TransformForSgml(prefix, csSgmlString);
   // remove any filename reserved characters from the filename
   CString thumbFile = csSgmlString;
   StringManipulation::RemoveReservedFilenameChars(thumbFile);
   thumbFile.Replace(_T(" "), _T("_"));
   line.Format(_T("%s  <thumb>%s_clip_%d.png</thumb>\n"), tabString, thumbFile, m_iBegin);
   pOutput->Append(line);
   if (!m_slKeywords.IsEmpty())
   {
      POSITION position = m_slKeywords.GetHeadPosition();
      while (position)
      {
         CString keyword = m_slKeywords.GetNext(position);
         StringManipulation::TransformForSgml(keyword, csSgmlString);
         line.Format(_T("%s  <keyword>%s</keyword>\n"), tabString, csSgmlString);
         pOutput->Append(line);
      }
   }

   line.Format(_T("%s</clip>\n"), tabString);
   pOutput->Append(line);

}

/*
CStructureInformation *CClipInformation::FindElement(int timestamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

  if (m_iBegin > timestamp)
      return this;

   return NULL;
}
*/

void CClipInformation::Copy(CList<CStructureInformation *, CStructureInformation *> &elementList, 
                            int from, int to, int timeOffset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CStructureInformation *newClip = Copy();

   newClip->SetBegin(m_iBegin + timeOffset);
   newClip->SetEnd(m_iEnd + timeOffset);

   ((CClipInformation *)newClip)->m_pCopySource = this;

   elementList.AddTail(newClip);
}

CStructureInformation *CClipInformation::Copy()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CClipInformation *newClip = new CClipInformation();

   newClip->SetTitle(m_csTitle);
   newClip->SetNumber(m_iNumber);
   newClip->SetBegin(m_iBegin);
   newClip->SetEnd(m_iEnd);
   newClip->SetUniqueId(m_iUniqueId);
   newClip->SetThumb(m_csThumb);
   newClip->SetKeywords(m_slKeywords);
   newClip->SetFilename(m_csFilename);

   newClip->m_pCopySource = this;

   return newClip;
}

void CClipInformation::CopyPages(CList<CStructureInformation *, CStructureInformation *> &elementList, int from, int to, int timeOffset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CStructureInformation *copy = Copy();
   if (copy)
   {
      copy->Increment(timeOffset);
      elementList.AddTail(copy);
   }
}

int CClipInformation::GetTimestamp(int y, int &actY, int height)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   GetNextY(actY, height);

   if (y < actY)
      return m_iBegin;

   return -1;
}

int CClipInformation::GetClipHeight(int pageHeight)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return (int)(4*pageHeight / 5.0);
}

void CClipInformation::GetNextY(int &y, int height)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   y += GetDrawHeight(height);
}

int CClipInformation::GetDrawHeight(int pageHeight)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return GetClipHeight(pageHeight) + 30 + 10*5/4;
}

int CClipInformation::GetActivePage(int timestamp)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (timestamp >= m_iBegin && timestamp < m_iEnd)
      return m_nInternalNumber;

   return -1;
}

void CClipInformation::Draw(CDC *pDC, int &x, int &y, int pageWidth, int pageHeight, int viewWidth, int activePage, int lastActivePage, LOGFONT &logFont, DrawSdk::ZoomManager *zoom)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int endPos = y;
   if(pageHeight == -1)
      pageHeight = 2;
   GetNextY(endPos, pageHeight);

   if (lastActivePage == -1 ||
       activePage == m_nInternalNumber || lastActivePage == m_nInternalNumber )
   { 
      if (!m_bThumIsInitialized)
      {
         if (m_bThumbIsCreated)
         {
            DeleteObject(m_clipThumb);
            m_bThumbIsCreated = false;
         }

         m_nThumbWidth = viewWidth - 15;
         m_nThumbHeight = GetClipHeight(pageHeight);
         if (VideoInformation::GetPosterFrame((LPCTSTR)m_csFilename, m_nThumbWidth , m_nThumbHeight, &m_clipThumb))
            m_bThumbIsCreated = true;
         else
            m_bThumbIsCreated = false;

         m_bThumIsInitialized = true;
      }

      int clipHeight = GetClipHeight(pageHeight);

      Gdiplus::Graphics graphics(pDC->m_hDC);
      int nOffset  = (viewWidth <= pageWidth + 15)? 0 : 5;
      //CRect rect(0, y-5, pageWidth + 10/*viewWidth*/, y + clipHeight + 25);
      CRect rect(5, y-5, viewWidth - nOffset/*pageWidth + 10*//*viewWidth*/, y + clipHeight + 25);
      if (activePage == m_nInternalNumber)
      {
         /*CBrush brush(ACTIVE_PAGE_RGB);
         pDC->FillRect(&rect, &brush);
         brush.DeleteObject();*/
         Gdiplus::Image* pImage = LoadImage(IDB_SLIDE_BKG_SELECTED, _T("PNG"), ::AfxGetResourceHandle());
         //graphics.DrawImage(&bitmap, (Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, (Gdiplus::REAL)rect.Width(), (Gdiplus::REAL)rect.Height());
         graphics.DrawImage(pImage, rect.left, rect.top, rect.Width(), rect.Height());
      }
      else
      {
         /*CBrush brush(RGB(255, 255, 255));
         pDC->FillRect(&rect, &brush);
         brush.DeleteObject();*/
         Gdiplus::Image* pImage = LoadImage(IDB_SLIDE_BKG_NORMAL, _T("PNG"), ::AfxGetResourceHandle());
         //graphics.DrawImage(&bitmap, (Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, (Gdiplus::REAL)rect.Width(), (Gdiplus::REAL)rect.Height());
         graphics.DrawImage(pImage, rect.left, rect.top, rect.Width(), rect.Height());
      }

      if (m_bThumbIsCreated && pageWidth != -1)
      {
         //CRect rect(5, y, 5 + viewWidth - 15, y + GetClipHeight(pageHeight));

         CRect rect1(10, y, 10 + viewWidth - 5, y + GetClipHeight(pageHeight));
         
         CDC bitmapDC;
         bitmapDC.CreateCompatibleDC(pDC);
         bitmapDC.SelectObject(m_clipThumb);
         //pDC->BitBlt(5, y, m_nThumbWidth, m_nThumbHeight, &bitmapDC, 0, 0, SRCCOPY);
         pDC->BitBlt(10, y, m_nThumbWidth, m_nThumbHeight, &bitmapDC, 0, 0, SRCCOPY);
      }

      CFont font;
      logFont.lfWeight = FW_NORMAL;
      font.CreateFontIndirect(&logFont);

      CFont *oldFont = (CFont *)pDC->SelectObject(&font);

      pDC->SetBkMode(TRANSPARENT);
      /*pDC->SetTextColor(RGB(0, 0, 0));
      pDC->DrawText(m_csTitle, CRect(x, y+clipHeight, viewWidth, y+clipHeight+15), DT_LEFT);*/
      pDC->SetTextColor(RGB(155, 173, 198));
      pDC->DrawText(m_csTitle, CRect(x + 5, y+clipHeight + 3, viewWidth, y+clipHeight+18), DT_LEFT);

      if (oldFont)
         pDC->SelectObject(oldFont);
      font.DeleteObject();
   }
    
   GetNextY(y, pageHeight);
}

void CClipInformation::Increment(int timeOffset)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iBegin += timeOffset;
   m_iEnd   += timeOffset;
}

void CClipInformation::UpdatePageNumbers(int &number, CMap<int, int, int, int> &mapUniquePageIds)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   int iUsed = 0;
   bool bContains = mapUniquePageIds.Lookup(GetUniqueId(), iUsed) == TRUE;
   if (bContains)
      ChangeUniqueIds();
      
   mapUniquePageIds.SetAt(GetUniqueId(), 1);

   ++number;
   m_nInternalNumber = number;
}


void CClipInformation::Print(int iLevel)
{
   for (int i=0; i<iLevel; ++i)
      TRACE(_T("  "));

   TRACE(_T("CLIP %d-%d %s\n"), GetBegin(), GetEnd(), (LPCTSTR)m_csFilename);

   POSITION position = m_elementList.GetHeadPosition();
   while (position != NULL)
   {
      CStructureInformation *pElement = m_elementList.GetNext(position);

      pElement->Print(iLevel + 1);
   }
}

/*********************************************
 *********************************************/

CFileInformation::CFileInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init();
}

CFileInformation::~CFileInformation()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

void CFileInformation::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iAudioSamplerate = 22050;
   m_iAudioBitrate    = 16;

   m_fVideoFramerate  = 25.0;
   m_iVideoQuality    = 100;
   m_iVideoKeyframes  = 15;
   m_csVideoCodec = _T("IV50");

   m_bShowClickPages = true;
}

void CFileInformation::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iAudioSamplerate = 22050;
   m_iAudioBitrate    = 16;

   m_fVideoFramerate  = 25.0;
   m_iVideoQuality    = 100;
   m_iVideoKeyframes  = 15;
   m_csVideoCodec = _T("IV50");
}

bool CFileInformation::ParseFrom(CLmdSgml *pLmdFile) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (pLmdFile == NULL)
        return false;

    SgmlElement *pFileInfoTag = pLmdFile->Find(_T("fileinfo"));
    if (pFileInfoTag == NULL)
        return false;

    CArray<SgmlElement *, SgmlElement *> aElements;
    pFileInfoTag->GetElements(aElements);
    for (int i = 0; i < aElements.GetSize(); ++i) {
        SgmlElement *pElement = aElements[i];
        if (pElement != NULL) {
            CString csName = pElement->GetName();
            if (csName == _T("audio")) {
                CArray<SgmlElement *, SgmlElement *> aAudioElements;
                pElement->GetElements(aAudioElements);
                for (int j = 0; j < aAudioElements.GetSize(); ++j) {
                    SgmlElement *pAudioElement = aAudioElements[j];
                    if (pAudioElement != NULL) {
                        CString csAudioTagName = pAudioElement->GetName();
                        if (csAudioTagName == _T("samplerate")) {
                            CString szInt = pAudioElement->GetParameter();
                            if (!szInt.IsEmpty())
                                m_iAudioSamplerate = _ttoi(szInt);
                        } else if (csAudioTagName == _T("bitrate")) {
                            CString szInt = pAudioElement->GetParameter();
                            if (!szInt.IsEmpty())
                                m_iAudioBitrate = _ttoi(szInt);
                        }
                    }
                }
                aAudioElements.RemoveAll();
            } else if (csName == _T("video")) {
                CArray<SgmlElement *, SgmlElement *> aVideoElements;
                pElement->GetElements(aVideoElements);
                for (int j = 0; j < aVideoElements.GetSize(); ++j) {
                    SgmlElement *pVideoElement = aVideoElements[j];
                    if (pVideoElement != NULL) {
                        CString csVideoTagName = pVideoElement->GetName();
                        if (csVideoTagName == _T("framerate")) {
                            CString csFloat = pVideoElement->GetParameter();
                            if (!csFloat.IsEmpty())
                                m_fVideoFramerate = (float)_tcstod(csFloat, NULL);
                        } else if (csVideoTagName == _T("codec")) {
                            m_csVideoCodec = pVideoElement->GetParameter();
                        } else if (csVideoTagName == _T("keyframes")) {
                            CString szInt = pVideoElement->GetParameter();
                            if (!szInt.IsEmpty())
                                m_iVideoKeyframes = _ttoi(szInt);
                        } else if (csVideoTagName == _T("quality")) {
                            CString szInt = pVideoElement->GetParameter();
                            if (!szInt.IsEmpty())
                                m_iVideoQuality = _ttoi(szInt);
                        }
                    }
                }
            } else if (csName == _T("showClickPages")) {
                CString csBool = pElement->GetParameter();
                if (!csBool.IsEmpty()) {
                    if (csBool.MakeLower() == _T("true"))
                        m_bShowClickPages = true;
                    else
                        m_bShowClickPages = false;
                }
            }
        }
    }

    return true;
}

void CFileInformation::Fill(CFileInformation *pFileInfo)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   
   if (!pFileInfo)
      return;

   pFileInfo->SetAudioSamplerate(m_iAudioSamplerate);
   pFileInfo->SetAudioBitrate(m_iAudioBitrate);
   pFileInfo->SetVideoFramerate(m_fVideoFramerate);
   pFileInfo->SetVideoQuality(m_iVideoQuality);
   pFileInfo->SetVideoKeyframes(m_iVideoKeyframes);
   pFileInfo->SetVideoCodec(m_csVideoCodec);
   pFileInfo->SetShowClickPages(m_bShowClickPages);
}

bool CFileInformation::Complete(CAudioStream *audioStream, CVideoStream *videoStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (audioStream)
   {
      // These two lines are not necessary anymore; the information
      // on the audio stream is read directly from the audio stream
      // in the CAudioStream class (see AVStreams.cpp).
//      audioStream->SetSampleRate(m_iAudioSamplerate);
//      audioStream->SetBitRate(m_iAudioBitrate);

   }
   if (videoStream)
   {
      videoStream->SetFrameRate(m_fVideoFramerate);
      videoStream->SetCodec(m_csVideoCodec);
      videoStream->SetKeyframes(m_iVideoKeyframes);
      videoStream->SetQuality(m_iVideoQuality);
   }
   return true;
}


/*********************************************
 *********************************************/

CMetadataStream::CMetadataStream() : CMediaStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Init();
}

CMetadataStream::~CMetadataStream()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   Clear();
}

void CMetadataStream::Init()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iID = -1;

   m_structureInfo.SetUniqueId(CStructureInformation::m_iIdCounter);
   ++CStructureInformation::m_iIdCounter;
}

void CMetadataStream::Clear()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iID = -1;

   m_csFilename.Empty();
   m_metaInfo.Clear();
   m_structureInfo.Clear();
   m_fileInfo.Clear();

   m_lmdSgml.Clear();
}

CMetadataStream *CMetadataStream::Copy()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   CMetadataStream *pCopyStream = new CMetadataStream;

   pCopyStream->SetFilename(m_csFilename);
   pCopyStream->SetStructureInformation(&m_structureInfo);
   pCopyStream->SetMetaInformation(&m_metaInfo);
   pCopyStream->SetFileInformation(&m_fileInfo);
   pCopyStream->SetSgmlInformation(&m_lmdSgml);
   pCopyStream->SetID(m_iID);

   return pCopyStream;
}

bool CMetadataStream::ParseFrom(CLrdSgml *pLrdFile) {
    if (pLrdFile == NULL) 
        return false;

    return m_metaInfo.ParseFrom(pLrdFile);
}

bool CMetadataStream::Open() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bResult = m_lmdSgml.Read(m_csFilename);
    if (!bResult)
        return false;

    m_metaInfo.ParseFrom(&m_lmdSgml);
    m_structureInfo.ParseFrom(&m_lmdSgml);
    m_fileInfo.ParseFrom(&m_lmdSgml);

    return true;
}

bool CMetadataStream::Complete(CAudioStream *audioStream, CVideoStream *videoStream, CClipStream *clipStream, CWhiteboardStream *whiteboardStream, CPageStream *pageStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_structureInfo.Complete(clipStream, whiteboardStream, pageStream);
   m_fileInfo.Complete(audioStream, videoStream);

   return true;
}

bool CMetadataStream::Complete(CClipStream *clipStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_structureInfo.Complete(clipStream);

   return true;
}

void CMetadataStream::UpdateMetadata(CPageStream *pPageStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_structureInfo.UpdateMetadata(pPageStream);

   return;
}

bool CMetadataStream::Write(LPCTSTR lmdFilename)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   return m_lmdSgml.Write(lmdFilename);
}

bool CMetadataStream::DeleteRemovedClips(CList<CStructureInformation *, CStructureInformation *> &list, CArray<CClipInfo *, CClipInfo *> &clipStream)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   POSITION position = list.GetHeadPosition();
   while (position)
   {
      CStructureInformation *pElement = list.GetNext(position);
      if (pElement)
         pElement->DeleteRemovedClips(clipStream);
   }

   return true;
}

void CMetadataStream::Append(CStructureInformation &structure, int sourceBegin, int sourceEnd, int timeOffset, 
                      CArray<CClipInfo *, CClipInfo *> &clipStream, bool useFlatStructure)
{ 
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   if (m_structureInfo.IsEmpty())
      return;

   CList<CStructureInformation *, CStructureInformation *> newList;
   if (useFlatStructure)
      m_structureInfo.CopyPages(newList, sourceBegin, sourceEnd, timeOffset);
   else
      m_structureInfo.Copy(newList, sourceBegin, sourceEnd, timeOffset);

   if (newList.IsEmpty())
      return;
   
   // find out if there are elements in list
   POSITION position = newList.GetHeadPosition();
   bool bHasElements = false;
   while (position)
   {
      CStructureInformation *pElement = newList.GetNext(position);
      if (pElement && !pElement->IsEmpty())
      {
         bHasElements = true;
         break;
      }
   }
   
   if (!bHasElements)
   {
      POSITION position = newList.GetHeadPosition();
      while (position)
      {
         CStructureInformation *pElement = newList.GetNext(position);
         if (pElement)
            delete pElement;
      }
      newList.RemoveAll();
      CStructureInformation *pElement = new CStructureInformation();
      pElement->SetBegin(sourceBegin + timeOffset);
      pElement->SetEnd(sourceEnd + timeOffset);
      structure.Append(pElement);
      return;
   }

   // update begin and end of list

   CStructureInformation *firstElement = newList.GetHead();
   if (firstElement)
      firstElement->CutBefore(sourceBegin + timeOffset);

   CStructureInformation *lastElement = newList.GetTail();
   if (lastElement)
   {
      lastElement->CutAfter(sourceEnd + timeOffset);
      lastElement->SetEnd(sourceEnd + timeOffset);
   }
 
   // last element of actual list
   lastElement  = structure.FindLastElement();
   // first element of list to append
   firstElement = newList.GetHead();
   if (lastElement && firstElement && (lastElement->GetUniqueId() == firstElement->GetUniqueId()))
   {
      lastElement->AppendStructure(firstElement);
      lastElement->SetEnd(firstElement->GetEnd());
      // delete root element without deleting element of m_elementList
      firstElement->m_elementList.RemoveAll();
      delete firstElement;
      firstElement = NULL;
   }
   else
   {
      POSITION position = newList.GetHeadPosition();
      while (position)
      {
         CStructureInformation *element = newList.GetNext(position);
         if (element)
         {
            structure.Append(element);
         }
      } 
   }

   structure.SetEnd(sourceEnd + timeOffset);
   
   newList.RemoveAll();

}

void CMetadataStream::SetStructureInformation(CStructureInformation *pStructureInfo)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   pStructureInfo->Fill(&m_structureInfo);
}

void CMetadataStream::SetMetaInformation(CMetaInformation *pMetaInfo)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   pMetaInfo->Fill(&m_metaInfo);
}

void CMetadataStream::SetFileInformation(CFileInformation *pFileInfo)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   pFileInfo->Fill(&m_fileInfo);
}

void CMetadataStream::SetSgmlInformation(SgmlFile *pLmdSgml)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

