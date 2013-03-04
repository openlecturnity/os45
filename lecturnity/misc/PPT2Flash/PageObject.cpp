/*********************************************************************

  program  : mlb_page.cpp 
  authors  : Gabriela Maass
  date     : 06.03.2001
  revision : $Id: PageObject.cpp,v 1.2 2007-08-24 11:57:13 maass Exp $
  desc     : 
  
**********************************************************************/

#include "StdAfx.h"
#include "resource.h"

#ifdef WIN32
#pragma warning(disable: 4786)
#endif

#include "PageObject.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


/***********************/
/***  CONSTRUCTORS  ***/
/**********************/

ASSISTANT::Page::Page(SGMLElement *_root, CString &csDocumentPath, CStringArray &arLsdTmpFileNames) 
{
   if (!_root)
      return;
   
   active = NULL;
   
   TCHAR 
      *tmp;
   
   imagePath_ = csDocumentPath;

   tmp = _root->GetAttribute("name");
   SetName(tmp);
   zoomFactor_ = 1.0;
   
   tmp = _root->GetAttribute("width");
   if (tmp) width = _ttoi(tmp);
   else     width = A4_WIDTH;
   
   tmp = _root->GetAttribute("height");
   if (tmp) height = _ttoi(tmp);
   else     height = A4_HEIGHT;   
   
   Color background;
   tmp = _root->GetAttribute("color");
   if (tmp) background = tmp;
   else     background = _T("#ffffff");   

   m_clrBackground = DrawObject::CalculateColor(background);
   
   tmp = _root->GetAttribute("slideid");
   if (tmp) pageID_ = tmp;
   else     pageID_ = _T("-1");   
   
   LoadObjects(_root, imagePath_, arLsdTmpFileNames);
   
   firstTime        = -1;
   if (pageID_ == _T("-1"))
   {
      TCHAR buf[20];
      _stprintf(buf, _T("a%d"), pageNumber);
      pageID_ = buf;
   }
   
   backRect = new ASSISTANT::Rectangle(0.0, 0.0, width, height, -1,
      background, 0, NULL, NULL, NULL);
}


/***************************/
/***  END CONSTRUCTORS  ***/
/**************************/



/*********************/
/***  DESTRUCTORS  ***/
/*********************/


ASSISTANT::Page::~Page()
{
   if (activeObjects.GetSize() > 0) 
   {
      for (int i = 0; i < activeObjects.GetSize(); ++i)
      {
         if (activeObjects[i] != NULL && 
            (activeObjects[i]->GetType() == GROUP || 
             activeObjects[i]->GetType() == ANIMATED))
            delete activeObjects[i];
      }
      activeObjects.RemoveAll();
   }
   
   if (objects.GetSize() > 0) 
   {
      for (int i = 0; i < objects.GetSize(); ++i) 
      {
         if (objects[i] != NULL)
            delete objects[i];
      }
      objects.RemoveAll();
   }
   
   //delete canvas;
   active = NULL;

   if (backRect)
      delete backRect;
}


/*************************/
/***  END DESTRUCTORS  ***/
/*************************/



void ASSISTANT::Page::LoadObjects(SGMLElement *_root, LPCTSTR path, CStringArray &arLsdTmpFileNames)
{
   SGMLElement 
      *hilf; 
   DrawObject
      *obj;
   
   hilf = _root;
   
   if (!hilf)
      return;

   if (hilf->son && hilf->son->Name == _T("LAYER")) 
      hilf = hilf->son;
   
   for (hilf = hilf->son; hilf != NULL; hilf = hilf->next) 
   {
      obj = NULL;
      if (hilf->Name == _T("LINE"))           
         obj = Line::Load(hilf);
      else if (hilf->Name == _T("RECTANGLE")) 
         obj = Rectangle::Load(hilf);
      else if (hilf->Name == _T("OVAL"))      
         obj = Oval::Load(hilf);
      else if (hilf->Name == _T("POLYLINE"))  
         obj = PolyLine::Load(hilf);
      else if (hilf->Name == _T("POLYGON"))   
         obj = Polygon::Load(hilf);
      else if (hilf->Name == _T("MARKER"))   
         obj = Marker::Load(hilf);
      else if (hilf->Name == _T("IMAGE")) 
      {
         obj = Image::Load(hilf, imagePath_, arLsdTmpFileNames);
      }
      else if (hilf->Name == _T("TEXT"))  // Text only appear in Group
      {
         obj = Text::Load(hilf);
      }
      else if (hilf->Name == _T("GROUP")) 
      {
         obj = Group::Load(hilf);
         if (obj)
         {
            ((Group *)obj)->LoadObjects(this, hilf, imagePath_, arLsdTmpFileNames);
            
            delete obj;
            obj = NULL;
         }
      }
      else if (hilf->Name == _T("ANIMATED")) 
      {
         obj = AnimatedObject::Load(hilf);
         if (obj)
         {
            ((AnimatedObject *)obj)->LoadObjects(this,  hilf, path, arLsdTmpFileNames);

            delete obj;
            obj = NULL;
         }
      }
      
      if (obj) 
      {
         AppendObject(obj);
      }
   }
}


void ASSISTANT::Page::DrawWhiteboard(CRect &rcWhiteboard, CDC *pDC)
{
}

void ASSISTANT::Page::AppendObject(DrawObject *obj, bool isActive, bool bAppendToUndo) 
{
   
   obj->SetPage(this);
   
   
   if	(obj->GetType() == ANIMATED)
   {
      // If the type of the animation is EXIT this animation may be shown first
      if (((AnimatedObject *)obj)->GetAnimationType() == AnimatedObject::EXIT)
      {
         bool bSetVisible = true;
         // Find out if there is an animation with the same id in activeObjects
         AnimatedObject *animObj = NULL;
         for (int i = 0; i < activeObjects.GetSize(); ++i)
         {
            if (activeObjects[i] && activeObjects[i]->GetType() == ANIMATED)
            {
               animObj = (AnimatedObject *)activeObjects[i];
               if (animObj->GetAnimationId() == ((AnimatedObject *)obj)->GetAnimationId())
               {
                  if (animObj->IsIdentic(obj))
                  {
                     bSetVisible = false;
                     break;
                  }
               }
            }
         }
         // if this animation is the first with this id, show this object
         if (bSetVisible)
         {
            if (animObj && animObj->GetAnimationId() == ((AnimatedObject *)obj)->GetAnimationId())
               ((AnimatedObject *)obj)->Show(animObj);
            else
               obj->SetVisible();
         }
      }
   }
   
   if (isActive)
      activeObjects.Add(obj);
   
   if (obj->GetType() == GROUP)
      return;
   
   if	(obj->GetType() == ANIMATED)
      return;
   
   if (objects.GetSize() == 0  || obj->GetOrder() < 0) 
   {
      if (obj->GetOrder() < 0)
         obj->SetOrder(0);
      objects.Add(obj);
      return;
   }
   
   for (int i = objects.GetSize()-1; i >= 0; --i)
   {
      if ((obj->GetOrder() >= objects[i]->GetOrder()))
         break;
   }
   
   // "insert" inserts before "pos". Only if pos is the position of the first element and 
   // the order of the inserted object ist smaller than the position of the first list element 
   // insert before. In all other cases increment pos before inserting.
   if (i == -1 || (objects[i]->GetOrder() <= obj->GetOrder()))
      ++i;
   objects.InsertAt(i, obj);
}

bool ASSISTANT::Page::ShowNextObject()
{
   int iClickId = -1;
   bool bHasAnimatedObject = false;
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if (activeObjects[i]->GetType() == ANIMATED && 
          ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE) 
      {
         AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[i];
         if (pAnimatedObject->GetStatus() == AnimatedObject::PASSIV)
         {
            bHasAnimatedObject = true;
            if (iClickId == -1)
               iClickId = pAnimatedObject->GetClickId();
            if (iClickId == pAnimatedObject->GetClickId())
            {
               pAnimatedObject->SetStatus(AnimatedObject::ACTIVE);
               if (pAnimatedObject->GetAnimationType() == AnimatedObject::ENTER)
               {
                  ShowAnimatedObject(i);
               }
               else // type is EXIT
               {
                  HideAnimatedObject(i);
               }
            }
            else // all objects with the same click id are processed
            {
               break;
            }
            // only one object per click
            if (iClickId == 0)
               break;
         }
      }
   }
   
   return bHasAnimatedObject;
}

void ASSISTANT::Page::InActivateObject(DrawObject *obj)
{
   if (activeObjects.GetSize() == 0) return;
   
   for (int i = 0; i < activeObjects.GetSize(); ++i) 
   {
      if (activeObjects[i] == obj) 
      {
         activeObjects.RemoveAt(i);
         return;
      }
   }
}


bool ASSISTANT::Page::HidePreviousObject()
{
   int iClickId = -1;
   bool bHasAnimatedObject = false;
   for (int i = activeObjects.GetSize()-1; i >= 0; --i)
   {
      if (activeObjects[i]->GetType() == ANIMATED && 
          ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE) 
      {
         AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[i];
         if (pAnimatedObject->GetStatus() == AnimatedObject::ACTIVE)
         {
            bHasAnimatedObject = true;
            if (iClickId == -1)
               iClickId = pAnimatedObject->GetClickId();
            if (iClickId == pAnimatedObject->GetClickId())
            {
               pAnimatedObject->SetStatus(AnimatedObject::PASSIV);
               if (pAnimatedObject->GetAnimationType() == AnimatedObject::ENTER)
               {
                  HideAnimatedObject(i, false);
                  ShowObjectIfEnteredBefore(i);
               }
               else // type is EXIT
               {
                  ShowAnimatedObject(i, false);
               }
            }
            else // all objects with the same click id are processed
            {
               break;
            }
            // only one object per click
            if (iClickId == 0)
               break;
         }
      }
   }
   
   return bHasAnimatedObject;
}

bool ASSISTANT::Page::HasNextObject()
{
   return false;
}

bool ASSISTANT::Page::HasPreviousObject()
{
   return false;
}

void ASSISTANT::Page::ShowAnimatedObject(int actualPos, bool backwards)
{  
   AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[actualPos];
   
   if (backwards)
   {
      // Hide all previous objects with the same animation id 
      // and the same parameters
      for (int i = actualPos-1; i >= 0; --i)
      {
         if (activeObjects[i]->GetType() == ANIMATED && 
             ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE)
         {
            AnimatedObject *animObj = (AnimatedObject *)activeObjects[i];
            if (animObj->GetAnimationId() == pAnimatedObject->GetAnimationId() && 
               (animObj->GetClickId() == 0 ||
               animObj->GetClickId() != pAnimatedObject->GetClickId()))
               
            {
               // Hide only identical objects
               animObj->Hide(pAnimatedObject);
            }
         }
      }
   }
   else
   {
      // Hide all following objects with the same animation id 
      // and the same parameters
      for (int i = actualPos; i < activeObjects.GetSize(); ++i)
      {
         if (activeObjects[i]->GetType() == ANIMATED && 
             ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE)
         {
            AnimatedObject *animObj = (AnimatedObject *)activeObjects[i];
            if (animObj->GetAnimationId() == pAnimatedObject->GetAnimationId() && 
               (animObj->GetClickId() == 0 ||
               animObj->GetClickId() != pAnimatedObject->GetClickId()))
            {
               // Hide only identical objects
               animObj->Hide(pAnimatedObject);
            }
         }
      }
   }
   
   // show object
   pAnimatedObject->SetVisible();
}

void ASSISTANT::Page::HideAnimatedObject(int actualPos, bool backwards)
{  
   AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[actualPos];
   
   if (backwards)
   {
      // Hide all previous objects with the same animation id 
      // and the same parameters
      for (int i = actualPos-1; i >= 0; --i)
      {
         if (activeObjects[i]->GetType() == ANIMATED && 
             ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE)
         {
            AnimatedObject *animObj = (AnimatedObject *)activeObjects[i];
            if (animObj->GetAnimationId() == pAnimatedObject->GetAnimationId() && 
               (animObj->GetClickId() == 0 ||
               animObj->GetClickId() != pAnimatedObject->GetClickId())) 
            {
               // Hide only identical objects
               animObj->Hide(pAnimatedObject);
            }
         }
      }
   }
   else
   {  // Hide all following objects with the same animation id 
      // and the same parameters
      for (int i = actualPos; i < activeObjects.GetSize(); ++i)
      {
         if (activeObjects[i]->GetType() == ANIMATED && 
             ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE)
         {
            AnimatedObject *animObj = (AnimatedObject *)activeObjects[i];
            if (animObj->GetAnimationId() == pAnimatedObject->GetAnimationId() && 
               (animObj->GetClickId() == 0 ||
               animObj->GetClickId() != pAnimatedObject->GetClickId()))
            {
               // Hide only identical objects
               animObj->Hide(pAnimatedObject);
            }
         }
      }
   }
   
   pAnimatedObject->SetInvisible();
}

bool ASSISTANT::Page::ShowObjectIfEnteredBefore(int actualPos)
{
   if (actualPos == 0)
      return false;
   
   AnimatedObject *pActualObject = (AnimatedObject *)activeObjects[actualPos];
   
   for (int i = actualPos-1; i >= 0; --i)
   {
      if ((((AnimatedObject *)activeObjects[i])->GetAnimationId() == pActualObject->GetAnimationId()) &&
         pActualObject->IsIdentic(activeObjects[i]))
      {
         if (((AnimatedObject *)activeObjects[i])->GetAnimationType() == AnimatedObject::ENTER)
         {
            activeObjects[i]->SetVisible();
            return true;
         }
         else
            return false;
      }
   }
   return false;
}


void ASSISTANT::Page::ShowInteractiveObject(int actualPos, int iObjectId, bool backwards)
{  
   AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[actualPos];
   
   if (backwards)
   {
      // Hide all previous objects with the same animation id 
      // and the same parameters
      for (int i = actualPos-1; i >= 0; --i)
      {
         if (activeObjects[i]->GetType() == ANIMATED && 
             ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_OBJECT)
         {
            AnimatedObject *animObj = (AnimatedObject *)activeObjects[i];
            if (iObjectId == animObj->GetAnimationId())
            {
               if (animObj->GetAnimationId() == pAnimatedObject->GetAnimationId() && 
                  (animObj->GetClickId() == 0 ||
                  animObj->GetClickId() != pAnimatedObject->GetClickId()))
                  
               {
                  // Hide only identical objects
                  animObj->Hide(pAnimatedObject);
               }
            }
         }
      }
   }
   else
   {
      // Hide all following objects with the same animation id 
      // and the same parameters
      for (int i = actualPos; i < activeObjects.GetSize(); ++i)
      {
         if (activeObjects[i]->GetType() == ANIMATED && 
             ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_OBJECT)
         {
            AnimatedObject *animObj = (AnimatedObject *)activeObjects[i];
            if (iObjectId == animObj->GetAnimationId())
            {
               if (animObj->GetAnimationId() == pAnimatedObject->GetAnimationId() && 
                  (animObj->GetClickId() == 0 ||
                  animObj->GetClickId() != pAnimatedObject->GetClickId()))
               {
                  // Hide only identical objects
                  animObj->Hide(pAnimatedObject);
               }
            }
         }
      }
   }
   
   // show object
   pAnimatedObject->SetVisible();
}

void ASSISTANT::Page::HideInteractiveObject(int actualPos, int iObjectId, bool backwards)
{  
   AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[actualPos];
   
   if (backwards)
   {
      // Hide all previous objects with the same animation id 
      // and the same parameters
      for (int i = actualPos-1; i >= 0; --i)
      {
         if (activeObjects[i]->GetType() == ANIMATED && 
             ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_OBJECT)
         {
            AnimatedObject *animObj = (AnimatedObject *)activeObjects[i];
            if (iObjectId == animObj->GetAnimationId())
            {
               if (animObj->GetAnimationId() == pAnimatedObject->GetAnimationId() && 
                  (animObj->GetClickId() == 0 ||
                  animObj->GetClickId() != pAnimatedObject->GetClickId())) 
               {
                  // Hide only identical objects
                  animObj->Hide(pAnimatedObject);
               }
            }
         }
      }
   }
   else
   {  // Hide all following objects with the same animation id 
      // and the same parameters
      for (int i = actualPos; i < activeObjects.GetSize(); ++i)
      {
         if (activeObjects[i]->GetType() == ANIMATED && 
             ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_OBJECT)
         {
            AnimatedObject *animObj = (AnimatedObject *)activeObjects[i];
            if (iObjectId == animObj->GetAnimationId())
            {
               if (animObj->GetAnimationId() == pAnimatedObject->GetAnimationId() && 
                  (animObj->GetClickId() == 0 ||
                  animObj->GetClickId() != pAnimatedObject->GetClickId()))
               {
                  // Hide only identical objects
                  animObj->Hide(pAnimatedObject);
               }
            }
         }
      }
   }
   
   pAnimatedObject->SetInvisible();
}

bool ASSISTANT::Page::ShowNextInteractiveObject(LPCTSTR objectIds, int iPPTObjectId)
{
   CArray<int, int>
      slObjects;
   
   CString ssNumber;
   ssNumber.Empty();
   for (int i = 0; i < (int)_tcslen(objectIds); ++i)
   {
      if (objectIds[i] != _T(' '))
      {
         ssNumber += objectIds[i];
      }
      else
      {
         slObjects.Add(_ttoi(ssNumber));
         ssNumber.Empty();
      }
   }
   
   int iClickId = -1;
   bool bHasAnimatedObject = false;

   bool bAllActive = true;
   bool bObjectFound = false;
   bool bObjectActivated = false;

   for (i = 0; i < slObjects.GetSize() && !bObjectActivated; ++i)
   {
      int iObjectNumber = slObjects[i];

      for (int j = 0; j < activeObjects.GetSize() && !bObjectActivated; ++j) 
      {
         if (activeObjects[j]->GetType() == ANIMATED)
         {
            AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[j];
            
            if (pAnimatedObject->GetTrigger() == AnimatedObject::CLICK_ON_OBJECT  &&
                pAnimatedObject->GetAnimationId() == iObjectNumber)
            {
             
               if (iPPTObjectId <= 0 || pAnimatedObject->GetCallerId() <= 0 ||
                   pAnimatedObject->GetCallerId() == iPPTObjectId)
               {
                  bObjectFound = true;
                  if (pAnimatedObject->GetStatus() == AnimatedObject::PASSIV)
                  {
                     bAllActive = false;
                     bObjectActivated = true;
                     bHasAnimatedObject = true;
                     if (iClickId == -1)
                        iClickId = pAnimatedObject->GetClickId();
                     if (iClickId == pAnimatedObject->GetClickId())
                     {
                        pAnimatedObject->SetStatus(AnimatedObject::ACTIVE);
                        if (pAnimatedObject->GetAnimationType() == AnimatedObject::ENTER)
                        {
                           ShowInteractiveObject(i, iObjectNumber);
                        }
                        else // type is EXIT
                        {
                           HideInteractiveObject(i, iObjectNumber);
                        }
                     }
                     else // all objects with the same click id are processed
                     {
                        break;
                     }
                     // only one object per click
                     if (iClickId == 0)
                        break;
                  }
               }
            }
         }
      }
   }

   if (bObjectFound && bAllActive)
   { 
      for (i = 0; i < slObjects.GetSize(); ++i)
      {
         int iObjectNumber = slObjects[i];

         for (int j = 0; j < activeObjects.GetSize(); ++j) 
         {
            if (activeObjects[j]->GetType() == ANIMATED)
            {
               AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[j];
               
               if (pAnimatedObject->GetTrigger() == AnimatedObject::CLICK_ON_OBJECT  &&
                  pAnimatedObject->GetAnimationId() == iObjectNumber)
               {
                  pAnimatedObject->SetStatus(AnimatedObject::PASSIV);
                  if (pAnimatedObject->GetAnimationType() == AnimatedObject::ENTER)
                  {
                     pAnimatedObject->SetInvisible();
                  }
                  else // type is EXIT
                  {
                     pAnimatedObject->SetVisible();
                  }
               }
            }
         }
      }
      ShowNextInteractiveObject(objectIds, iPPTObjectId);
   }
      
   return bHasAnimatedObject;

}

int ASSISTANT::Page::GetMaxWidth(int &actMax)
{
   if (width > actMax)
   {
      actMax = width;
   }
   
   return actMax;
}

int ASSISTANT::Page::GetMaxHeight(int &actMax)
{
   if (height > actMax)
   {
      actMax = height;
   }
   
   return actMax;
}

void ASSISTANT::Page::CollectPages(CArray<Page *, Page *> &pageList)
{
   pageList.Add(this);
}

void ASSISTANT::Page::ResetInteractiveObjects()
{
   
   for (int i = 0; i < activeObjects.GetSize(); ++i) 
   {
      if (activeObjects[i]->GetType() == ANIMATED)
      {
         AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[i];
       
         if (pAnimatedObject->GetTrigger() == AnimatedObject::CLICK_ON_OBJECT)
         {
            pAnimatedObject->SetStatus(AnimatedObject::PASSIV);
            if (pAnimatedObject->GetAnimationType() == AnimatedObject::ENTER)
            {
               pAnimatedObject->SetInvisible();
            }
            else // type is EXIT
            {
               bool bSetVisible = true;
               AnimatedObject *pPreviouslyAnimatedObject = NULL;
               // find out if object will previously be inserted
               for (int j = 0; j < i; ++j)
               {
                  if (activeObjects[j]->GetType() == ANIMATED)
                  {
                     pPreviouslyAnimatedObject = (AnimatedObject *)activeObjects[j];
                     if (pPreviouslyAnimatedObject->GetTrigger() == AnimatedObject::CLICK_ON_OBJECT)
                     { 
                        if (pPreviouslyAnimatedObject->GetAnimationId() == pAnimatedObject->GetAnimationId())
                        {
                           if (pPreviouslyAnimatedObject->IsIdentic(pAnimatedObject))
                           {
                              bSetVisible = false;
                              break;
                           }
                        }
                     }
                  } 
               }
               if (bSetVisible)
                  pAnimatedObject->SetVisible();
            }
         }
      }
      if (activeObjects[i]->GetType() == GROUP)
      {

      }
   }

}

void ASSISTANT::Page::GetObjects(CArray<DrawObject *, DrawObject *> &arObjects)
{
   for (int i = 0; i < objects.GetSize(); ++i)
   {
      if (objects[i]->IsVisible())
         arObjects.Add(objects[i]);
   }
}
