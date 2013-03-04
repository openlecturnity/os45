/*********************************************************************

  program  : mlb_folder.cpp 
  authors  : Gabriela Maass
  date     : 18.02.2000
  revision : $Id: mlb_folder.cpp,v 1.29 2010-09-21 05:49:06 maass Exp $
  desc     : 
  
**********************************************************************/

#include "StdAfx.h"

#pragma warning(disable: 4786)

//#include <strstrea.h>

#include "..\..\Studio\Resource.h"
#include "mlb_folder.h"
#include "mlb_page.h"
#include "la_project.h" 
#include "mlb_misc.h"
#include "la_thumbs.h"

ASSISTANT::Folder::Folder(uint32 _id, _TCHAR *_name) 
: GenericContainer(_id) 
{
   _TCHAR
      name[100];
   DWORD 
      size=100;
   
   SetName(_name);
   
   GetUserName(name,&size);
   if (!name) 
   {
      scorm.SetAuthor(_T(""));
      scorm.SetCreator(_T(""));
   }
   else
   {
      scorm.SetAuthor(name);
      scorm.SetCreator(name);
   }
   
   isOpen_ = true;
}


ASSISTANT::Folder::Folder(uint32 _id, _TCHAR *szFolderNumber, GenericContainer *session, SGMLElement *root) 
: GenericContainer(_id) 
{
   ASSISTANT::Folder
      *folder;
   ASSISTANT::Page
      *page;
   SGMLElement 
      *hilf;
   _TCHAR 
      *tmp;
   
   if (!root) return;
   
   SetName(root->GetAttribute("name"));
   
   tmp = root->GetAttribute("author");
   if (tmp) scorm.SetAuthor(tmp);
   else     scorm.SetAuthor(_T(""));   
   
   tmp = root->GetAttribute("creator");
   if (tmp) scorm.SetCreator(tmp);
   else     scorm.SetCreator(_T(""));   
   
   tmp = root->GetAttribute("keywords");
   if (tmp) scorm.SetKeywords(tmp);
   else     scorm.SetKeywords(_T(""));   
   
   
   int iContainerNumber = 0;
   hilf = root->son;
   while (hilf) 
   {
      if (hilf->Name == _T("FOLDER")) 
      {
         ++iContainerNumber;
         _TCHAR szTmpFolderNumber[256];
         _stprintf (szTmpFolderNumber, _T("%s.%d"), szFolderNumber, iContainerNumber);

         folder = new ASSISTANT::Folder(Project::active->GetObjectID(), szTmpFolderNumber, session,hilf);
         folder->SetCodepage(GetCodepage());
         InsertContainerCurrent(folder);
      }
      else if (hilf->Name == _T("PAGE")) 
      {
         ++iContainerNumber;
         _TCHAR szTmpPageNumber[256];
         _stprintf (szTmpPageNumber, _T("%s.%d"), szFolderNumber, iContainerNumber);

         page = new ASSISTANT::Page(Project::active->GetObjectID(), szTmpPageNumber, session, hilf);
         page->SetCodepage(GetCodepage());
         InsertContainerCurrent(page);
      }
      else 
      {
         CString csMessage;
         csMessage.LoadString(IDS_WRONG_LSD);
         MessageBox(NULL, csMessage, NULL, MB_OK | MB_TOPMOST);
      }
      hilf = hilf->next;
   }

   if (subContainer.GetSize() > 0)
      current = 0;
   else
      current = -1;
   
   isOpen_ = true;
}


void ASSISTANT::Folder::InitTelepointers() 
{
   if (subContainer.GetSize() == 0)
      return;
   
   for (int i = 0; i < subContainer.GetSize(); ++i) 
   {
      if (subContainer[i]->GetType() == GenericObject::PAGE) 
      {
         ((Page*)subContainer[i])->InitTelepointer();
      } 
      else 
      {
         ((Folder*)subContainer[i])->InitTelepointers();
      }
   }
}


ASSISTANT::Folder::~Folder() 
{
   if (subContainer.GetSize() == 0)
      return;

   for (int i = 0; i < subContainer.GetSize(); ++i) 
   {
      if (subContainer[i] != NULL)
         delete subContainer[i];
   }

   subContainer.RemoveAll();
}


void ASSISTANT::Folder::AddToStructureTree(CDocumentStructureView *pDocStructTree, UINT nParentId)
{
   if (pDocStructTree == NULL)
      return;

   pDocStructTree->InsertChapter((ASSISTANT::GenericContainer *)this, nParentId);
   
   for (int i = 0; i < subContainer.GetSize(); ++i) 
      subContainer[i]->AddToStructureTree(pDocStructTree, id);

}

void ASSISTANT::Folder::SetName(LPCTSTR _name)
{
   containerName.Empty();
   
   if (_name == NULL) 
      containerName.LoadString(IDS_DEFAULT_CHAPTER_NAME);
   else    
      containerName = _name;  
   
   scorm.SetTitle(containerName);
}


ASSISTANT::Page *ASSISTANT::Folder::GoToFirstPage()
{
   GenericContainer
      *ret;
   
   ret = 0;
   if (subContainer.GetSize() > 0) 
   {
      for (int i = 0; i < subContainer.GetSize(); ++i)
      {
         ret = subContainer[i];
         if (ret->GetType() == GenericObject::PAGE)
         {
            OpenFolder();
            current = i;
            return (Page *)ret;
         }
         else 
         {
            ret = ((Folder *)subContainer[i])->GoToFirstPage();
            if (ret)
            {
               OpenFolder();
               current = i;
               return (Page *)ret;
            }
         }
      }
   }
   
   return NULL;
   
}

ASSISTANT::Page *ASSISTANT::Folder::GoToLastPage()
{
   Page
      *page;
   
   
   if (subContainer.GetSize() > 0) 
   {
      int i = subContainer.GetSize();
      do 
      {
         --i;
         if (subContainer[i]->GetType() == GenericObject::PAGE)
         {
            OpenFolder();
            current = i;
            return (Page *)subContainer[i];
         }
         else 
         {
            if ((page = ((Folder *)subContainer[i])->GoToLastPage()))
            {
               OpenFolder();
               current = i;
               return page;
            }
         }
      } while (i > 0);
      
   }
   
   return NULL;
   
}

/*/
ASSISTANT::GenericContainer *ASSISTANT::Folder::SetCurrentContainer(std::vector<uint32> &idVector, int pos) 
{
   GenericContainer
      *ret;
   uint32 
      id, id1;
   
   
   ret = 0;
   if (subContainer.GetSize() > 0 && !idVector.empty() && (pos < idVector.size())) 
   {
      id = idVector[pos];
      temp = subContainer.begin();
      while (temp != subContainer.end()) 
      {
         id1 = (*temp)->GetID();
         if (id1 == id) 
         {
            OpenFolder();
            current = temp;
            ret = (*current)->SetCurrentContainer(idVector,pos+1);
            break;
         }
         temp++;
      }
   }
   else 
      ret = this;
   
   return ret;
}

/*/

ASSISTANT::GenericObject* ASSISTANT::Folder::DeleteObject(uint32 id) 
{
/*
GenericObject
*obj = NULL;

  for (std::list<GenericContainer*>::iterator i = subContainer.begin(); i != subContainer.end(); ++i) 
  {
  if ((*i)->GetID() == id) 
  {
  obj = *i;
  subContainer.erase(i);
  current = subContainer.end();
  if (obj == currentPage) 
		currentPage = NULL;
      return obj;
      }
      obj = (*i)->DeleteObject(id);
      if (obj) return obj;
      }
      
        return NULL;
   */
   
   GenericObject
      *obj = NULL;
   int
      nextContainer;
   
   if (subContainer.GetSize() == 0)
      return NULL;
   
   for (int i = 0; i < subContainer.GetSize(); ++i) 
   {
      if (subContainer[i]->GetID() == id) 
      {
         obj = subContainer[i];
         nextContainer = i;
         subContainer.RemoveAt(i);
         break;
      }
      else
      {
         obj = subContainer[i]->DeleteObject(id);
         if (obj) 
         {
            nextContainer = i;
            break;
         }
      }
   }
   
   
   if (obj) 
   {
      if (nextContainer >= subContainer.GetSize())
         current = subContainer.GetSize() -1;
      else if (subContainer.GetSize() == 0)
         current = -1;
      else
         current = nextContainer;
      return obj;
   }
   
   
   return NULL;
}


ASSISTANT::GenericContainer* ASSISTANT::Folder::GetNextContainer(ASSISTANT::GenericContainer* obj) 
{
   if (subContainer.GetSize() == 0)
      return NULL;
   
   if (subContainer.GetSize() == 1)
      return this; // there is only one element in this container; therefore select the container itself
   
   for (int i = 0; i < subContainer.GetSize(); ++i)
   {
      if (subContainer[i]->GetID() == obj->GetID()) break;
   }
   
   if (i == subContainer.GetSize()) 
      return NULL;
   
   int next = i;
   ++next;
   
   if (next == subContainer.GetSize()) 
   {
      --next; // = i
      --next; // previous element
   } 
   
   return subContainer[next];
}


void ASSISTANT::Folder::DeleteAll() 
{
   if (subContainer.GetSize() == 0)
      return;
   
   for (int i = 0; i < subContainer.GetSize(); ++i) 
   {
      subContainer[i]->DeleteAll();
      delete subContainer[i];
   }
   
   subContainer.RemoveAll();
} 

void ASSISTANT::Folder::Zoom() 
{
   for (int i = 0; i < subContainer.GetSize(); ++i) 
   {
      /*/
      GUI::Progress::IncrementProgress();
      /*/
      subContainer[i]->Zoom();
   }
}


void ASSISTANT::Folder::OpenFolder() 
{
      /*/
   if (!nodeName.IsEmpty())
      GUI::PageView::OpenFolderNode(nodeName);
      /*/
}



int ASSISTANT::Folder::GetNbrOfPages() 
{
   if (subContainer.GetSize() == 0)
      return 0;
   
   int count = 0;
   for (int i = 0; i < subContainer.GetSize(); ++i)
   {
      if (subContainer[i]->GetType() == GenericObject::PAGE) 
         ++count;
      else 
         count += subContainer[i]->GetNbrOfPages();
   }
   return count;
}

int ASSISTANT::Folder::GetPageNumber(UINT nId, bool &bFound) 
{
   if (subContainer.GetSize() == 0)
      return 0;
   
   int count = 0;
   for (int i = 0; i < subContainer.GetSize(); ++i)
   {
      if (subContainer[i]->GetType() == GenericObject::PAGE) 
      {
         ++count;
         if(subContainer[i]->GetID() == nId)
         {
            bFound = true;
            return count;
         }
      }
      else 
         count += subContainer[i]->GetPageNumber(nId, bFound);
   }
   return count;
}

void  ASSISTANT::Folder::SaveContainer(FILE *fp, LPCTSTR path, int level, bool all)
{
   WCHAR *tab = (WCHAR *)malloc((level*2+1)*sizeof(WCHAR));
   *tab = L'\0';
   for (int i = 0; i < level; i++) 
      wcscat(tab, L"  ");
   
   CString csSgmlString;

   CString csName = containerName;
   StringManipulation::TransformForSgml(csName, csSgmlString);
   WCHAR *wszName = ASSISTANT::ConvertTCharToWChar(csSgmlString);
   
   CString csKeywords = scorm.GetKeywords();
   StringManipulation::TransformForSgml(csKeywords, csSgmlString);
   WCHAR *wszKeywords = ASSISTANT::ConvertTCharToWChar(csSgmlString);
   
   CString csAuthor = scorm.GetAuthor();
   StringManipulation::TransformForSgml(csAuthor, csSgmlString);
   WCHAR *wszAuthor = ASSISTANT::ConvertTCharToWChar(csSgmlString);
   
   CString csCreator = scorm.GetCreator();
   StringManipulation::TransformForSgml(csCreator, csSgmlString);
   WCHAR *wszCreator = ASSISTANT::ConvertTCharToWChar(csSgmlString);
   
   
   fwprintf(fp, L"%s<FOLDER name=\"%s\" author=\"%s\" creator=\"%s\" keywords=\"%s\">\n",
            tab, wszName, wszAuthor, wszCreator, wszKeywords);
   
   
   if (wszName)
      delete wszName;
   
   if (wszAuthor)
      delete wszAuthor;
   
   if (wszCreator)
      delete wszCreator;
   
   if (wszKeywords)
      delete wszKeywords;
   
   if (subContainer.GetSize() > 0) 
   {
      for (int i = 0; i < subContainer.GetSize(); ++i)
      {
         subContainer[i]->SaveContainer(fp, path, (level+1), all);
      }
   }
   
   for (i = 0; i < level; i++)
      fwprintf(fp, L"  ");
   
   fwprintf(fp, L"%s</FOLDER>\n", tab);
   
   if (tab)
      delete tab;
}

void ASSISTANT::Folder::SaveMetadata(CFileOutput *pLmdFile)
{
   for (int i = 0; i < subContainer.GetSize(); ++i)
      subContainer[i]->SaveMetadata(pLmdFile);
}


void ASSISTANT::Folder::ResetRecording()
{
   if (subContainer.GetSize() > 0) 
   {
      for (int i = 0; i < subContainer.GetSize(); ++i)
         subContainer[i]->ResetRecording();
   }
}


ASSISTANT::Page *ASSISTANT::Folder::GoToPrevPage(ASSISTANT::GenericContainer *_active, bool *found, bool _jump)
{
   Page 
      *prevPage;
  
   if (subContainer.GetSize() == 0)
      return NULL;
   
   if (current == -1)
      current = 0;

   if (subContainer[current]->GetType() == GenericObject::PAGE)
   {
      if (!_jump && ((Page *)subContainer[current])->HidePreviousObject()) 
      {
         prevPage = (Page *)subContainer[current];
      }
      else 
      {
         int i = current;
         prevPage = NULL;
         while (!prevPage && (i > 0))
         {
            --i;
            if (subContainer[i]->GetType() == GenericObject::PAGE)
            {
               prevPage = (Page *)subContainer[i];
               current = i;
            }
            else
            {
               prevPage = ((Folder *)subContainer[i])->GoToLastPage();
               if (prevPage)
                  current = i;
            }
         }
      }
   }
   else
   {
      prevPage = ((Folder *)subContainer[current])->GoToPrevPage(_active, found, _jump);
      if (!prevPage)
      {
         int i = current;
         while (!prevPage && (i > 0))
         {
            --i;
            if (subContainer[i]->GetType() == GenericObject::PAGE)
            {
               prevPage = (Page *)subContainer[i];
               current = i;
            }
            else
            {
               prevPage = ((Folder *)subContainer[i])->GoToLastPage();
               if (prevPage)
                  current = i;
            }
         }
      }
   }
   
   return prevPage;
}

ASSISTANT::Page *ASSISTANT::Folder::GoToNextPage(ASSISTANT::GenericContainer *_active, bool *found, bool _jump)
{
   
   Page 
      *nextPage;
   
   if (subContainer.GetSize() == 0)
      return NULL;
   
   if (current == -1)
      current = 0;

   if (subContainer[current]->GetType() == GenericObject::PAGE)
   {
      if (!_jump && ((Page *)subContainer[current])->ShowNextObject()) 
      {
         nextPage = (Page *)subContainer[current];
      }
      else 
      {
         int i = current;
         nextPage = NULL;
         
         ++i;
         while (!nextPage && (i < subContainer.GetSize()))
         {
            if (subContainer[i]->GetType() == GenericObject::PAGE)
            {
               nextPage = (Page *)subContainer[i];
               current = i;
            }
            else
            {
               nextPage = ((Folder *)subContainer[i])->GoToFirstPage();
               if (nextPage)
                  current = i;
            }
            ++i;
         }
      }
   }
   else
   {
      nextPage = ((Folder *)subContainer[current])->GoToNextPage(_active, found, _jump);
      if (!nextPage)
      {
         int i = current;
         ++i;
         while (!nextPage && (i < subContainer.GetSize()))
         {
            if (subContainer[i]->GetType() == GenericObject::PAGE)
            {
               nextPage = (Page *)subContainer[i];
               current = i;
            }
            else
            {
               nextPage = ((Folder *)subContainer[i])->GoToFirstPage();
               if (nextPage)
                  current = i;
            }
            ++i;
         }
      }
   }
   
   return nextPage;
}

bool ASSISTANT::Folder::PreviousPageAvailable() {
     if (subContainer.GetSize() == 0)
        return false;

    if (current == -1)
        current = 0;

    if ((subContainer[current]->GetType() == GenericObject::PAGE && 
        ((Page *)subContainer[current])->HasPreviousObject()) ||
        ((Folder *)subContainer[current])->PreviousPageAvailable()) {
            return true;
    } else {
        int i = current;
        while (i > 0) {
            --i;
            if (subContainer[i]->GetType() == GenericObject::PAGE) {
                return true;
            } else {
                if (((Folder *)subContainer[i])->ContainPages())
                    return true;
            }
        }
    }

    return false;
}

bool ASSISTANT::Folder::NextPageAvailable() {
    if (subContainer.GetSize() == 0)
        return false;

    if (current == -1)
        current = 0;

    if ((subContainer[current]->GetType() == GenericObject::PAGE && 
        ((Page *)subContainer[current])->HasNextObject()) ||
        ((Folder *)subContainer[current])->NextPageAvailable()) {
            return true;
    } else {
        int i = current;
        ++i;
        while (i < subContainer.GetSize()) {
            if (subContainer[i]->GetType() == GenericObject::PAGE) {
                return true;
            } else {
                if (((Folder *)subContainer[i])->ContainPages()) 
                    return true;
            }
            ++i;
        }
    }

    return false;
}

ASSISTANT::Page *ASSISTANT::Folder::GoToPageWithID(const _TCHAR *slideID)
{
   if (subContainer.GetSize() == 0)
      return NULL;
   
   for (int i = 0; i < subContainer.GetSize(); ++i)
   {
      if (subContainer[i]->GetType() == GenericObject::PAGE)
      {
         Page *page = (Page *)subContainer[i];
         CString pageID = page->GetSlideId();
         if (_tcscmp(page->GetSlideId(), slideID) == 0)
         {
            current = i;
            return page;
         }
      }
      else
      {
         Page *page = ((Folder *)subContainer[i])->GoToPageWithID(slideID);
         if (page)
         {
            current = i;
            return page;
         }
      }
   }
   
   return NULL;
}

bool ASSISTANT::Folder::ContainPages() {
    if (subContainer.GetSize() == 0)
        return false;

    for (int i = 0; i < subContainer.GetSize(); ++i) {
        if (subContainer[i]->GetType() == GenericObject::PAGE)
            return true;
        else {
            if (((Folder *)subContainer[i])->ContainPages())
                return true;
        }
    }

    return false;
}

bool ASSISTANT::Folder::RenameContainer(UINT nContainerId, LPCTSTR szContainerName)
{
   if (nContainerId == id)
   {
      containerName = szContainerName;
      return true;
   }

   bool bRenamed = false;
   for (int i = 0; i < subContainer.GetSize() && !bRenamed; ++i)
   {
      bRenamed = subContainer[i]->RenameContainer(nContainerId, szContainerName);
   }

   return bRenamed;
}

void ASSISTANT::Folder::SetKeywords(LPCTSTR szKeywords)
{
   scorm.SetKeywords(szKeywords);

   return;
}

HRESULT ASSISTANT::Folder::GetKeywords(CString &csKeywords)
{  
   csKeywords = scorm.GetKeywords();
   
   return S_OK;
}

bool ASSISTANT::Folder::HasChanged()
{
   for (int i = 0; i < subContainer.GetSize(); ++i) 
   {
      if (subContainer[i]->HasChanged())
         return true;
   }
   
   return false;
}

void ASSISTANT::Folder::SetChanged(bool changed)
{
    m_bContainerChanged = changed;

    if (subContainer.GetSize() > 0) {
        for (int i = 0; i < subContainer.GetSize(); ++i) {
            if (subContainer[i] != NULL)
                subContainer[i]->SetChanged(changed);
        }
    }
}

ASSISTANT::Page *ASSISTANT::Folder::GetActivePage()
{
   Page
      *ret = NULL;
   
   if (subContainer.GetSize() == 0)
      return NULL;
   
   if (current != -1)
   {
      if (subContainer[current]->GetType() == GenericObject::PAGE)
      {
         ret = (Page *)subContainer[current];
      }
      else if (subContainer[current]->GetType() == GenericObject::FOLDER)
      {
         ret = ((Folder *)subContainer[current])->GetActivePage();
      }
   }
   
   return ret;
   
}

ASSISTANT::Folder *ASSISTANT::Folder::GetActiveFolder()
{
   Folder
      *ret = NULL;
   
   if (subContainer.GetSize() == 0)
      return NULL;
   
   if (current != -1 && current < subContainer.GetSize())
   {
      if (subContainer[current]->GetType() == GenericObject::FOLDER)
      {
         ret = ((Folder *)subContainer[current])->GetActiveFolder();
         if (ret == NULL) ret = ((Folder *)subContainer[current]);
      }
      else
         ret = this;
   }
   else
      ret = this;
   
   return ret;
}


void ASSISTANT::Folder::InsertEmptyFolder()
{
   if (subContainer.GetSize() > 0 && 
       current != -1 && !m_bSelected &&
       subContainer[current]->GetType() == GenericObject::FOLDER)
   {
      ((Folder *)subContainer[current])->InsertEmptyFolder();
      return;
   }
   
   Folder *folder = new Folder(ASSISTANT::Project::active->GetObjectID());
   folder->SetCodepage(GetCodepage());
   
   InsertContainerCurrent(folder);
}

void ASSISTANT::Folder::InsertEmptyPage(int iWidth, int iHeight, COLORREF clrBackground)
{
   if (subContainer.GetSize() > 0 && 
       current != -1 && !m_bSelected &&
       subContainer[current]->GetType() == GenericObject::FOLDER)
   {
      ((Folder *)subContainer[current])->InsertEmptyPage(iWidth, iHeight, clrBackground);
      return;
   }
   
   CString csPageName;
   csPageName.LoadString(IDS_DEFAULT_PAGE_NAME);

   Page *page = new Page(ASSISTANT::Project::active->GetObjectID(), (LPCTSTR)csPageName, 
                         iWidth, iHeight, clrBackground);
   page->SetCodepage(GetCodepage());
   
   InsertContainerCurrent(page);
}

void ASSISTANT::Folder::DuplicatePage(ASSISTANT::Page *pPage)
{
   if (subContainer.GetSize() > 0 && current != -1 && 
       subContainer[current]->GetType() == GenericObject::FOLDER)
   {
      ((Folder *)subContainer[current])->DuplicatePage(pPage);
      return;
   }
   
   Page *pNewPage = pPage->Copy();
   
   InsertContainerCurrent(pNewPage);
}

void ASSISTANT::Folder::InsertImage(_TCHAR *filename, _TCHAR *path, int x, int y, bool single)
{
   if (subContainer.GetSize() > 0 && (current != -1))
      subContainer[current]->InsertImage(filename, path, x, y, single);
}

void ASSISTANT::Folder::DrawWhiteboard(CRect &rcWhiteboard, CDC *pDC)
{
   if (subContainer.GetSize() > 0&& (current != -1))
      subContainer[current]->DrawWhiteboard(rcWhiteboard, pDC);
}

void ASSISTANT::Folder::Activate()
{
   if (subContainer.GetSize() > 0 && (current != -1))
   {
      if (current >= subContainer.GetSize())
         current = subContainer.GetSize() - 1;
      subContainer[current]->Activate();
   }
}

void ASSISTANT::Folder::DeActivate()
{
   if (subContainer.GetSize() > 0 && 
       current < subContainer.GetSize() && (current != -1))
      subContainer[current]->DeActivate();
}

void ASSISTANT::Folder::ResetFirstInserted()
{
   
   if (subContainer.GetSize() == 0)
      return;
   
   for (int i = 0; i < subContainer.GetSize(); ++i) 
      subContainer[i]->ResetFirstInserted();
}

void ASSISTANT::Folder::CollectPages(CArray<Page *, Page *> &pageList)
{
   for (int i = 0; i < subContainer.GetSize(); ++i)
      subContainer[i]->CollectPages(pageList);
}

