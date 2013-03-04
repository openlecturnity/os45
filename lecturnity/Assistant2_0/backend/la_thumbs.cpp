/*********************************************************************

 program  : la_thumbs.cpp
 authors  : Gabriela Maass
 date     : 08.01.2002
 revision : $Id: la_thumbs.cpp,v 1.16 2010-07-01 07:21:31 maass Exp $
 desc     : 

 **********************************************************************/

#include "StdAfx.h"

//#include <time.h>

#include "la_thumbs.h"
#include "mlb_page.h"
#include "mlb_misc.h"
#include "la_project.h"

PLAYER::ThumbElement::ThumbElement()
{ 
   parent_ = NULL;
   number_ = -1;
}

PLAYER::ThumbElement::~ThumbElement()
{ 
   parent_ = NULL;
   number_ = -1;
   container_ = NULL;
}


//////////////////////////////////////////////////////////////////////////

PLAYER::ThumbChapter::ThumbChapter() : ThumbElement()
{
   lastNumber = 0;
}
         

PLAYER::ThumbChapter::ThumbChapter(ASSISTANT::GenericContainer *container) : ThumbElement()
{
   container_ = container;
   title_ = container->GetName();

   lastNumber = 0;
}
    
PLAYER::ThumbChapter::~ThumbChapter()
{
   Clear();
}
          

PLAYER::ThumbChapter::ThumbChapter(SgmlElement *pChapterNode) : ThumbElement() {

    if (pChapterNode == NULL)
        return;

    title_ = pChapterNode->GetValue(_T("title"));

    if (pChapterNode->GetValue(_T("nr")))
        number_ = pChapterNode->GetIntValue(_T("nr"));
    else
        number_ = -1;

    CArray<SgmlElement *, SgmlElement *> aElements;
    pChapterNode->GetElements(aElements);
    for (int i = 0; i < aElements.GetSize(); ++i) {
        SgmlElement *pElement = aElements[i];

        if (_tcscmp(pElement->GetName(), _T("chapter")) == 0) {
            ThumbChapter *pChapter = new ThumbChapter(pElement);
            Append(pChapter);
        }
        else if (_tcscmp(pElement->GetName(), _T("page")) == 0) {
            ThumbPage *pPage = new ThumbPage(pElement);
            Append(pPage);
        }
    }
}

void PLAYER::ThumbChapter::Append(ThumbElement *element)
{
   elements.Add(element);
   element->SetParent(this);
   if (element->GetNumber() < 0) 
   {
      ++lastNumber;
      element->SetNumber(lastNumber);
   }
}

PLAYER::ThumbChapter *PLAYER::ThumbChapter::Insert(int anz, ThumbPage *page) 
{
   anz--;
   if (anz == 0) 
   {
      container_ = ASSISTANT::Project::active->GetParentContainer(page->GetContainer()->GetID());
      title_ = container_->GetName();
      Append(page);
      page->SetParent(this);
      return this;
   }
   else 
   {
      ThumbChapter *newChapter = new ThumbChapter();
      ThumbChapter *ret = newChapter->Insert(anz, page);
      ret->SetParent(this);
      container_ = ASSISTANT::Project::active->GetParentContainer(ret->GetContainer()->GetID());
      title_ = container_->GetName();
      Append(newChapter);
      return this;
   }
}

void PLAYER::ThumbChapter::Insert(ThumbPage *page)
{
   int anz = 0;
   ASSISTANT::GenericContainer *parent = 
      ASSISTANT::Project::active->GetParentContainer(page->GetContainer()->GetID());

   while (parent) 
   {
      parent = ASSISTANT::Project::active->GetParentContainer(parent->GetID());
      ++anz;
   }

   ThumbChapter *newChapter = new ThumbChapter();

   ThumbChapter *ret = newChapter->Insert(anz, page);
   container_ = ASSISTANT::Project::active->GetParentContainer(ret->GetContainer()->GetID());
   ret->SetParent(this);
   Append(newChapter);
 
}

void PLAYER::ThumbChapter::WriteRootEntry(CFileOutput *lmdOutput, ASSISTANT::ClipList *clipList)
{
   int 
      nextLevel;

   if (elements.GetSize() == 0)
   {
      // This may be Denver mode: No chapters,
      // no pages, nothing. Let's check the clip list.
      if (clipList)
         clipList->WriteLmdEntry(lmdOutput);
   }
   else
   {
      // We have at least one page, so let's write
      // out everything.

      // Let us see if there are any clips before the
      // first chapter; in that case, write LMD entries
      // for those clips. Note that pFirst is always
      // defined, because we know the elements.size() is
      // larger than 0, i.e., there are elements in the
      // elements list.
      ThumbElement *pFirst = elements[0];
      int firstPageTimeStamp = pFirst->GetBegin();
      if (clipList)
         clipList->WriteLmdEntry(lmdOutput, -1, firstPageTimeStamp, 1);

      nextLevel = 1;
      for (int i = 0; i < elements.GetSize(); ++i)
      {
         if (elements[i] != NULL)
            elements[i]->WriteEntry(lmdOutput, nextLevel, clipList, true);
      }

      // Now let's do the same thing with clips coming
      // after the last chapter: Add them, as they have
      // not yet been added. Note that pLast is always
      // defined, because we know the elements.size() is
      // larger than 0, i.e., there are elements in the
      // elements list.
      if (elements.GetSize() > 0)
      {
         ThumbElement *pLast = elements[elements.GetSize() -1];
         int lastPageTimeStamp  = pLast->GetEnd();
         if (clipList)
            clipList->WriteLmdEntry(lmdOutput, lastPageTimeStamp, -1, 1);
      }
   }
}

void PLAYER::ThumbChapter::WriteEntry(CFileOutput *lmdOutput, int level, ASSISTANT::ClipList *clipList, bool searchForClips)
{
   CString csLine;
   CString csSgmlLine;

   CString csTab;
   for (int i=0; i<level; i++) 
      csTab += _T("  ");

   csLine = title_;
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   csLine.Format(_T("%s<chapter title=\"%s\" nr=\"%d\">\n"), csTab, csSgmlLine, number_);
   lmdOutput->Append(csLine);

   // Here all clips with a timestamp less than the first slide time stamp
   // were inserted in front of the current chapter. This is obviously wrong
   // if more than one chapter was used: All clips were inserted into each
   // chapter. See WriteRootEntry for the fix.
//   if (searchForClips)
//   {
//      int firstPageTimeStamp = GetBegin();
//      if (clipList)
//         clipList->WriteLmdEntry(fp, -1, firstPageTimeStamp, 2);
//   }

   for (i = 0; i < elements.GetSize(); ++i) 
   {
      if (elements[i])
         elements[i]->WriteEntry(lmdOutput, level+1, clipList);
   }

   // See above.
//   if (searchForClips)
//   {
//      int lastPageTimeStamp = GetEnd();
//      if (clipList)
//         clipList->WriteLmdEntry(fp, lastPageTimeStamp, -1, 2);
//   }

   csLine.Format(_T("%s</chapter>\n"), csTab);
   lmdOutput->Append(csLine);
}

void PLAYER::ThumbChapter::GetSlides(CArray<ThumbElement *, ThumbElement *> &aSlidesArray) {
   for (int i = 0; i < elements.GetSize(); ++i) {
      if (elements[i] != NULL)
         elements[i]->GetSlides(aSlidesArray);
   }
}


void PLAYER::ThumbChapter::Clear()
{
   parent_ = NULL;
   for (int i = 0; i < elements.GetSize(); ++i) 
   {
      if (elements[i] != NULL)
         delete elements[i];
   }

   elements.RemoveAll();

   lastNumber = 0;
   title_.Empty();
}

long PLAYER::ThumbChapter::GetEnd()
{

   int lastTime = 0;

   for (int i = 0; i < elements.GetSize(); ++i) 
   {
      if (elements[i] != NULL)
      {
         if (elements[i]->GetEnd() > lastTime)
            lastTime = elements[i]->GetEnd();
      }
   }

   return lastTime;
}

long PLAYER::ThumbChapter::GetBegin()
{
   if (elements.GetSize() == 0)
      return -1;

   int firstTime = elements[0]->GetBegin();

   for (int i = 1; i < elements.GetSize(); ++i) 
   {
      if (elements[i] != NULL)
      {
         if ((elements[i]->GetBegin() >= 0) && (elements[i]->GetBegin() < firstTime))
            firstTime = elements[i]->GetBegin();
      }
   }

   return firstTime;
}

int PLAYER::ThumbChapter::GetElementCount()
{
   int elCount = 0;

   for (int i = 0; i < elements.GetSize(); ++i) 
   {
      if (elements[i] != NULL)
        elCount += elements[i]->GetElementCount();
   }

   return elCount;
}

//////////////////////////////////////////////////////////////////////////


PLAYER::ThumbPage::ThumbPage(ASSISTANT::GenericContainer *container, const _TCHAR *prefix, int time) : ThumbElement()
{
   container_ = container;
   title_ = container_->GetName();
   begin_ = time;
   end_ = time;
   keywords_ = ((ASSISTANT::Page *)container)->GetScorm()->GetKeywords();

   _TCHAR charTime[30];
   _stprintf(charTime, _T("%d"), time);

   imageName_ = prefix;
   imageName_ += _T("_");
   imageName_ += charTime;
   imageName_ += _T("_thumb.png");
}

PLAYER::ThumbPage::ThumbPage(SgmlElement *pPageNode) : ThumbElement() {
    if (pPageNode == NULL)
        return;

   keywords_.Empty();

   CArray<SgmlElement *, SgmlElement *> aElements;
   pPageNode->GetElements(aElements);
   for (int i = 0; i < aElements.GetSize(); ++i) {
       SgmlElement *pElement = aElements[i];
       if (_tcscmp(pElement->GetName(), _T("title")) == 0) {
           title_ = pElement->GetParameter();
       } else if (_tcscmp(pElement->GetName(), _T("nr")) == 0) {
           if (pElement->GetParameter() != NULL)
               number_ = _ttoi(pElement->GetParameter());
           else
               number_ = -1;
       } else if (_tcscmp(pElement->GetName(), _T("begin")) == 0) {
           if (pElement->GetParameter() != NULL)
               begin_ = _ttoi(pElement->GetParameter());
           else
               begin_ = 0;
       } else if (_tcscmp(pElement->GetName(), _T("end")) == 0) {
           if (pElement->GetParameter() != NULL)
               end_ = _ttoi(pElement->GetParameter());
           else
               end_ = 0;
       } else if (_tcscmp(pElement->GetName(), _T("thumb")) == 0) {
           imageName_ = pElement->GetParameter();
       } else if (_tcscmp(pElement->GetName(), _T("keyword")) == 0) {
           keywords_ += pElement->GetParameter();
       }
   }
}
   
PLAYER::ThumbPage::~ThumbPage()
{
   parent_ = NULL;
}

void PLAYER::ThumbPage::WriteEntry(CFileOutput *lmdOutput, int level, ASSISTANT::ClipList *clipList, bool searchForClips)
{
   CString csLine;
   CString csSgmlLine;

   CString csTab;
   for (int i=0; i<level; i++) 
      csTab += _T("  ");

   csLine.Format(_T("%s<page>\n"), csTab);
   lmdOutput->Append(csLine);

   csLine = title_;
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   csLine.Format(_T("%s  <title>%s</title>\n"), csTab, csSgmlLine);
   lmdOutput->Append(csLine);

   csLine.Format(_T("%s  <nr>%d</nr>\n"), csTab, number_);
   lmdOutput->Append(csLine);

   csLine.Format(_T("%s  <begin>%d</begin>\n"), csTab, begin_);
   lmdOutput->Append(csLine);

   csLine.Format(_T("%s  <end>%d</end>\n"), csTab, end_);
   lmdOutput->Append(csLine);

   // This was wrong if the image name contained single quotes (')
   // fprintf(fp,"%s  <thumb>%s</thumb>\n",tab,imageName_);
   csLine = imageName_;
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   // remove any filename reserved characters from the filename
   CString thumbFile = csSgmlLine;
   StringManipulation::RemoveReservedFilenameChars(thumbFile);
   thumbFile.Replace(_T(" "), _T("_"));
   csLine.Format(_T("%s  <thumb>%s</thumb>\n"), csTab, thumbFile);
   lmdOutput->Append(csLine);
   
   CStringArray arKeywords;
   ASSISTANT::CreateListFromString(keywords_, arKeywords);
   for (i = 0; i < arKeywords.GetSize(); ++i) 
   {
      csLine = arKeywords[i];
      StringManipulation::TransformForSgml(csLine, csSgmlLine);
      csLine.Format(_T("%s  <keyword>%s</keyword>\n"), csTab, csSgmlLine);
      lmdOutput->Append(csLine);
   }
   arKeywords.RemoveAll();

   csLine.Format(_T("%s</page>\n"), csTab);
   lmdOutput->Append(csLine);

   if (clipList)
      clipList->WriteLmdEntry(lmdOutput, begin_, end_, level);
}

void PLAYER::ThumbPage::GetSlides(CArray<ThumbElement *, ThumbElement *> &aSlidesArray) {
    aSlidesArray.Add(this);
}

void PLAYER::ThumbPage::Clear()
{
   parent_ = NULL;
   title_.Empty();
   imageName_.Empty();
   keywords_.Empty();
}

//////////////////////////////////////////////////////////////////////////


PLAYER::ThumbApplication::ThumbApplication(int time) : ThumbElement()
{
}


//////////////////////////////////////////////////////////////////////////


PLAYER::Thumbs::Thumbs(bool bCreateRoot)
{
   DWORD 
      size=100;

   if (bCreateRoot)
      root_ = new ThumbChapter();
   else
      root_ = NULL;
   actual_ = NULL;
}

PLAYER::Thumbs::Thumbs(CLmdSgml *pLmdSgml) {
    actual_ = NULL;

    root_ = new ThumbChapter();

    SgmlElement *pStructureTag = pLmdSgml->Find(_T("structure"), _T("docinfo"));
    if (pStructureTag == NULL)
        return;

    CArray<SgmlElement *, SgmlElement *> aElements;
    pStructureTag->GetElements(aElements);

    for (int i = 0; i < aElements.GetSize(); ++i) {
        SgmlElement *pElement = aElements[i];
        if (pElement != NULL) {
            if (_tcscmp(pElement->GetName(), _T("chapter")) == 0) {
                ThumbChapter *chapter = new ThumbChapter(pElement);
                root_->Append(chapter);
            }
        }
    }
}


PLAYER::Thumbs::~Thumbs()
{
   Clear();

   actual_ = NULL;
   if (root_)
      delete root_;
   root_ = NULL;
}

PLAYER::ThumbChapter *PLAYER::Thumbs::FindLastCommonChapter(ASSISTANT::GenericContainer *page, int *anz)
{
   PLAYER::ThumbChapter
      *actualParent=NULL;
   ASSISTANT::GenericContainer
      *pageParent=NULL;
   
   if (!actual_) return NULL;

   actualParent = (PLAYER::ThumbChapter *)actual_->GetParent();
   while (actualParent) 
   {
      pageParent = ASSISTANT::Project::active->GetParentContainer(page->GetID());
      *anz = 0;
      while(pageParent) 
      {
         if (actualParent->GetContainer() == pageParent) 
         {
            return actualParent;
         }
         pageParent = ASSISTANT::Project::active->GetParentContainer(pageParent->GetID());
         (*anz)++;
      }
      actualParent = (PLAYER::ThumbChapter *)actualParent->GetParent();
   }

   return NULL;


}

void PLAYER::Thumbs::AddElement(ASSISTANT::GenericContainer *page, int time)
{
   int
      anz=0;

   if (!ASSISTANT::Project::active->IsSgOnlyDocument() && 
       ASSISTANT::Project::active->recording && 
       !ASSISTANT::Project::active->paused) 
   {
      if (actual_) 
         actual_->SetEnd(time);

      if (actual_ && (page == actual_->GetContainer())) {
         return;
      }

      ThumbElement *actualParent = FindLastCommonChapter(page, &anz);

      ThumbPage *newPage = new ThumbPage(page, prefix_, time);

      if (!actualParent) 
      {
         root_->Insert(newPage);
      }
      else
      {
         if ((actualParent == actual_->GetParent()) && (anz == 0)) 
         {
            ((ThumbChapter *)actualParent)->Append(newPage);
         }
         else 
         {
            ((ThumbChapter *)actualParent)->Insert(anz+1, newPage);
         }
      }

      actual_ = newPage;
   }

}


void PLAYER::Thumbs::CloseElement(PLAYER::ThumbPage *closeElement)
{
  int
    time;

   if (!ASSISTANT::Project::active->IsSgOnlyDocument() && 
       ASSISTANT::Project::active->recording && 
       !ASSISTANT::Project::active->paused) 
   {
      time = ASSISTANT::Project::active->GetRecordingTime();

      actual_ = closeElement;
   }
}
         
void PLAYER::Thumbs::SetName(LPCTSTR name, LPCTSTR path)
{
   prefix_ = name;
   path_ = path;

   fileName_ = path;

#ifdef WIN32
   fileName_ += _T("\\");
#else
   fileName_ += _T("/");
#endif

   fileName_ += name;
   fileName_ += _T(".lmd");

}

void PLAYER::Thumbs::WriteRootEntry(CFileOutput *lmdOutput, ASSISTANT::ClipList *clipList)
{
   if (root_)
      root_->WriteRootEntry(lmdOutput, clipList);
}

void PLAYER::Thumbs::GetSlides(CArray<ThumbElement *, ThumbElement *> &aSlidesArray) {
    if (root_)
        root_->GetSlides(aSlidesArray);
}

void PLAYER::Thumbs::CloseThumbList(int _time)
{
   if (actual_) 
      actual_->SetEnd(_time);

}

void PLAYER::Thumbs::Clear()
{
   actual_ = NULL;

   if (root_)
      root_->Clear();
}

int PLAYER::Thumbs::GetElementCount()
{
   if (!root_)
      return 0;

   int elCount = root_->GetElementCount();

   return elCount;
}
