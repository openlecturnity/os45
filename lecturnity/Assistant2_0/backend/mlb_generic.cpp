/*********************************************************************

 program  : mlb_generic.cpp 
 authors  : Gabriela Maass
 date     : 08.03.2000
 revision : $Id: mlb_generic.cpp,v 1.16 2009-07-13 06:48:52 maass Exp $
 desc     : 

 **********************************************************************/

#include "StdAfx.h"

#include "mlb_misc.h"

//#include <iostream.h>
//#include <iterator>

#include "mlb_generic.h"
#include "la_project.h"

//using namespace std;

/**************************************
 *
 **************************************/

ASSISTANT::GenericObject::GenericObject(UINT _id, CString &_tkref) 
  : id(_id), tkref (_tkref) {
}

ASSISTANT::GenericObject::GenericObject(UINT _id) 
  : id(_id) {
}

ASSISTANT::GenericObject::~GenericObject() {
}


/**************************************
 *
 **************************************/
ASSISTANT::GenericContainer::GenericContainer(_TCHAR *_name, UINT _id, CString &tkref) 
  : GenericObject(_id, tkref) 
{
  containerName = _name;
  current = -1 ;
  thumbNumber = -1;
  
  scorm.SetTitle(_T(""));
  scorm.SetKeywords(_T(""));

  codePage = GetACP();

  m_bSelected = false;

  m_bContainerChanged = false;
  
}


ASSISTANT::GenericContainer::GenericContainer(_TCHAR *_name, UINT _id) 
  : GenericObject(_id) 
{
  containerName = _name;
  current  = -1;
  thumbNumber = -1;
  
  scorm.SetTitle(_T(""));
  scorm.SetKeywords(_T(""));

  codePage = GetACP();

  m_bSelected = false;

  m_bContainerChanged = false;
}


ASSISTANT::GenericContainer::GenericContainer(UINT _id, CString &tkref) 
  : GenericObject(_id, tkref) 
{
  current  = -1;
  thumbNumber = -1;
  
  scorm.SetTitle(_T(""));
  scorm.SetKeywords(_T(""));

  codePage = GetACP();  

  m_bSelected = false;

  m_bContainerChanged = false;
}


ASSISTANT::GenericContainer::GenericContainer(UINT _id) 
  : GenericObject(_id) 
{
  current  = -1;
  thumbNumber = -1;

  scorm.SetTitle(_T(""));
  scorm.SetKeywords(_T(""));

  codePage = GetACP();  

  m_bSelected = false;

  m_bContainerChanged = false;
}


ASSISTANT::GenericContainer::~GenericContainer() 
{
   if (subContainer.GetSize() > 0) 
   {
      for (int i = 0; i < subContainer.GetSize(); ++i)
         delete subContainer[i];
      subContainer.RemoveAll();
   }
   
   containerName.Empty();
   nodeName.Empty();
}

void ASSISTANT::GenericContainer::Init() 
{
   if (subContainer.GetSize() == 0) 
   {
      current = -1;
   }
   else 
   {
      for (int i = 0; i < subContainer.GetSize(); ++i)
         subContainer[i]->Init();
   }
}

void ASSISTANT::GenericContainer::InsertContainerCurrent(ASSISTANT::GenericContainer *container) 
{
   if (m_bSelected)
   {
      InsertContainerBegin(container);
      return;
   }

   if (subContainer.GetSize() == 0) 
   {
      subContainer.Add(container);
      current = 0;
   }
   else 
   {
      if ((current != -1) && (current < subContainer.GetSize())) 
      {
         ++current;
         subContainer.InsertAt(current, container);
      }
      else 
      {
         subContainer.Add(container);
         current = subContainer.GetSize() - 1;
      }
   }
}


void ASSISTANT::GenericContainer::InsertContainerEnd(GenericContainer* container) 
{
  subContainer.Add(container);

  current = subContainer.GetSize() - 1;
}

void ASSISTANT::GenericContainer::InsertContainerBegin(GenericContainer* container) 
{
  subContainer.InsertAt(0, container);

  current = 0;
}


void ASSISTANT::GenericContainer::SetPageDimension(int _width, int _height)
{
   for (int i = 0; i < subContainer.GetSize(); ++i)
      subContainer[i]->SetPageDimension(_width, _height);
}


ASSISTANT::GenericObject* ASSISTANT::GenericContainer::GetObject(UINT id) 
{
   if (id == this->GetID()) 
      return this;

   GenericObject* obj = NULL;
   for (int i = 0; i < subContainer.GetSize(); ++i)
   {
      if (subContainer[i]->GetID() == id) 
         return subContainer[i];
      obj = subContainer[i]->GetObject(id);
      if (obj) 
         return obj;
   }
  
   return NULL;
}


ASSISTANT::GenericObject* ASSISTANT::GenericContainer::GetGroup(UINT id) 
{
   GenericObject* obj = NULL;
   for (int i = 0; i < subContainer.GetSize(); ++i)
   {
      obj = subContainer[i]->GetGroup(id);
      if (obj) return obj;
   }
   
   return NULL;
}


float ASSISTANT::GenericContainer::GetObjectOrder(UINT id) 
{
   
   for (int i = 0; i < subContainer.GetSize(); ++i)
   {
      if (subContainer[i]->GetID() == id) 
         return subContainer[i]->GetOrder();
   }
   
   return 0; // not found
}

 
float ASSISTANT::GenericContainer::GetPushBackObjectOrder() 
{
   float
      lastOrder;

   if (subContainer.GetSize() == 0) 
      lastObjectOrder =  OO_FIRST;
   else 
   {
      lastOrder = subContainer[subContainer.GetSize() - 1]->GetOrder();
      lastObjectOrder = lastOrder + OO_OFFSET;
   } 

   return lastObjectOrder;
}

float ASSISTANT::GenericContainer::GetEndObjectOrder() 
{
   if (subContainer.GetSize() == 0) 
     return OO_FIRST;
   else 
     return (subContainer[subContainer.GetSize() - 1])->GetOrder();
}


float  ASSISTANT::GenericContainer::GetFirstObjectOrder() 
{
   if (subContainer.GetSize() == 0) 
      return OO_FIRST;
   else 
      return subContainer[0]->GetOrder();
}


float ASSISTANT::GenericContainer::GetNewObjectOrder(int pos) 
{
   if (subContainer.GetSize() == 0) {
      return OO_FIRST;
   } 
   else if (pos == 0) {
      return ((subContainer[0])->GetOrder() / 2);
   } 
   else if (pos >= GetSize()) {
      return (subContainer[subContainer.GetSize()-1])->GetOrder() + OO_OFFSET;
   } 
   else {
      float newOrder;

      int i = pos - 1;
      if (i == subContainer.GetSize()-1) 
      {
         newOrder = subContainer[i]->GetOrder() + OO_OFFSET;
      }
      else 
      {
         newOrder = subContainer[i]->GetOrder();
         ++i;
         float orderDiff = subContainer[i]->GetOrder() - newOrder;
         newOrder = newOrder + (orderDiff / 2);
      }

      return newOrder;
   }
}


float ASSISTANT::GenericContainer::GetNextObjectOrder(UINT _id) 
{
   for (int i = 0; i < subContainer.GetSize(); ++i)
   {
      if (subContainer[i]->GetID() == _id) 
      {
         if (i == subContainer.GetSize()-1) 
            return subContainer[i]->GetOrder() + OO_OFFSET;
         else {
            float newOrder = subContainer[i]->GetOrder();
            ++i;
            float orderDiff = subContainer[i]->GetOrder() - newOrder;
            newOrder = newOrder + (orderDiff / 2);
            return newOrder;
         }
      }
   }

   return 0;
}

void  ASSISTANT::GenericContainer::ReOrderObjects()
{ 
   float newOrder = OO_FIRST;

   for (int i = 0; i < subContainer.GetSize(); ++i)
   {
      subContainer[i]->SetOrder(newOrder);
      newOrder += OO_OFFSET;
   }

}

/*
ASSISTANT::GenericObject* ASSISTANT::GenericContainer::GetObjectWithOrder(float order) 
{
  for (std::list<GenericContainer*>::iterator i = subContainer.begin(); i != subContainer.end(); ++i) {
    if ((*i)->GetOrder() == order) return *i;
    if ((*i)->GetOrder() > order) return NULL; // not found
  }
  return NULL; // not found
}

*/

// Find direct parent container of Object obj
ASSISTANT::GenericContainer* ASSISTANT::GenericContainer::GetContainer(UINT id) 
{
   GenericContainer* container = NULL;
   for (int i = 0; i < subContainer.GetSize(); ++i) 
   {
      if (subContainer[i]->GetID() == id) 
         return this;
      container = subContainer[i]->GetContainer(id);
      if (container) 
         return container;
   }
   
   return NULL;
}


ASSISTANT::GenericContainer* ASSISTANT::GenericContainer::GetFirstPage() 
{
   GenericContainer* ret = NULL;
   
   for (int i = 0; i < subContainer.GetSize(); ++i) 
   {
      if (subContainer[i]->GetType() == GenericObject::PAGE) 
      {
         return subContainer[i];
      }
      ret = subContainer[i]->GetFirstPage();
      if (ret) 
         return ret;
   }
   
   return NULL; // no page in here
}


ASSISTANT::GenericContainer* ASSISTANT::GenericContainer::GetLastPage() 
{
  GenericContainer
     *ret = NULL;

  int i = subContainer.GetSize();
  while (i > 0) 
  {
     --i;
     if (subContainer[i]->GetType() == GenericObject::PAGE) 
     {
        return subContainer[i];
     }
      ret = subContainer[i]->GetLastPage();
      if (ret) return ret;
  }

  return NULL; // no page in here
}


ASSISTANT::GenericContainer* ASSISTANT::GenericContainer::GetPrevious(UINT id) 
{
  GenericContainer  *ret;
    
  ret = NULL;

  for (int i = 0; i < subContainer.GetSize(); ++i) {
    if (subContainer[i]->GetID() == id) {
      // found reference container
      if (i == 0) return NULL; // there is no previous container
      --i;
      return subContainer[i];
    }
    
    ret = subContainer[i]->GetPrevious(id);
    if (ret) return ret; // else not found
  }

  return NULL; // not found
}


ASSISTANT::GenericContainer* ASSISTANT::GenericContainer::GetNext(UINT id) {
  GenericContainer* ret = NULL;

  for (int i = 0; i < subContainer.GetSize(); ++i) {
    if (subContainer[i]->GetID() == id) {
      // found reference container
      ++i;
      if (i == -1) return NULL; // there is no next container
      return subContainer[i];
    }
    
    ret = subContainer[i]->GetNext(id);
    if (ret) return ret; // else not found
  }

  return NULL; // not found
}


bool ASSISTANT::GenericContainer::RenameContainer(UINT nContainerId, LPCTSTR szContainerName)
{
   _ASSERT(false);

   return false;
}

ASSISTANT::GenericObject* ASSISTANT::GenericContainer::DeleteObject(UINT id) 
{
   GenericObject
      *obj = NULL;
   int 
      nextContainer;
   
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

int ASSISTANT::GenericContainer::GetMaxWidth(int &actMax)
{
  for (int i = 0; i < subContainer.GetSize(); ++i) 
     subContainer[i]->GetMaxWidth(actMax);

  return actMax;
}

int ASSISTANT::GenericContainer::GetMaxHeight(int &actMax)
{
  for (int i = 0; i < subContainer.GetSize(); ++i) 
     subContainer[i]->GetMaxHeight(actMax);

  return actMax;
}

bool ASSISTANT::GenericContainer::SelectContainer(UINT nContainerId)
{
   if (nContainerId == id)
   {
      m_bSelected = true;
      if (subContainer.GetSize() > 0 && current != -1 && 
          subContainer[current]->GetType() == GenericContainer::FOLDER)
      {
         current = -1;
         for (int i = 0; i < subContainer.GetSize() && current == -1; ++i)
         {
            if (subContainer[i]->GetType() == GenericContainer::PAGE)
               current = i;
         }
      }
      return true;
   }

   m_bSelected = false;

   bool bSelected = false;
   for (int i = 0; i < subContainer.GetSize() && !bSelected; ++i)
   {
      bSelected = subContainer[i]->SelectContainer(nContainerId);
      if (bSelected)
         current = i;
   }
   return bSelected;
}

ASSISTANT::GenericContainer *ASSISTANT::GenericContainer::GetParent(UINT nId)
{
   if (id == nId)
      return NULL;

   for (int i = 0; i < subContainer.GetSize(); ++i)
   {
      if (subContainer[i]->GetID() == nId)
         return this;
      GenericContainer *pContainer = subContainer[i]->GetParent(nId);
      if (pContainer)
         return pContainer;
   }

   return NULL;
}

void ASSISTANT::GenericContainer::SelectFirstContainer()
{
   current = -1;

   if (subContainer.GetSize() > 0)
      current = 0;
}

bool ASSISTANT::GenericContainer::SaveContainer(UINT nContainerId, LPCTSTR szFileName)
{
   bool bSaved = false;

   if (nContainerId == id)
   {
      CString csPath = szFileName;
      ASSISTANT::GetPath(csPath);
      
      WCHAR *wszFilename = ASSISTANT::ConvertTCharToWChar(szFileName);

      FILE *fp = _wfopen(wszFilename, L"wb");

      if (wszFilename)
         delete wszFilename;

      if (fp) 
      {
         // Write Unicode BOMs
         BYTE bom1 = 0xFF;
         BYTE bom2 = 0xFE;
         fwrite(&bom1, 1, 1, fp);
         fwrite(&bom2, 1, 1, fp);

         CString csSgmlString;
         CString csName = GetName();
         StringManipulation::TransformForSgml(csName, csSgmlString);
         WCHAR *wszName = ASSISTANT::ConvertTCharToWChar(csSgmlString);

         fwprintf(fp, L"<ASSISTANT codepage=\"%d\" name=\"%s\">\n", GetCodepage(), wszName);

         SaveContainer(fp, csPath, 1, false);

         fwprintf(fp, L"</ASSISTANT>\n");

         fclose(fp);

         if (wszName)
            delete wszName;

         ASSISTANT::Project::active->OpenCloseMetadata(this, szFileName);
      }

      bSaved = true;
   }

   for (int i = 0; i < subContainer.GetSize() && !bSaved; ++i)
   {
      bSaved = subContainer[i]->SaveContainer(nContainerId, szFileName);
   }

   return bSaved;
}

