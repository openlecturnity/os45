/*********************************************************************

 program : mlb_animate.cpp
 authors : Gabriela Maass
 date    : 11.03.2001
 desc    : Animated objects

 **********************************************************************/

#include "StdAfx.h"

#include "DrawObjects.h"
#include "PageObject.h"


ASSISTANT::AnimatedObject::AnimatedObject(UINT _id, int iAnimationId, int iAnimationType, int iClickId, int iTrigger, int iCallerId, 
                                          LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects) 
   : DrawObject(0, 0, 0, 0, -1, _id, _hyperlink, _currentDirectory, _linkedObjects)
{
  actItem = -1;
  visible = false;

  m_iAnimationId = iAnimationId;
  m_iAnimationType = iAnimationType;
  m_iClickId = iClickId;
  m_iTrigger = iTrigger;
  m_iCallerId = iCallerId;
  m_iStatus = PASSIV;
}


ASSISTANT::DrawObject* ASSISTANT::AnimatedObject::GetObject(int _idx) 
{
  if ((_idx < 0) || (_idx >= animatedObjects.GetSize())) return NULL;
  
  int iObjPos = 0;
  for (int i = 0; i < _idx; ++i) ++iObjPos;

  return animatedObjects[iObjPos];
}

   /*/
std::vector<uint32>* ASSISTANT::AnimatedObject::GetObjectIDs() 
{
  std::vector<uint32>* ids = new std::vector<uint32>;

  for (std::list<DrawObject*>::iterator i = animatedObjects.begin(); i != animatedObjects.end(); ++i) 
    ids->push_back((*i)->GetID());

  return ids;
}
   /*/   

/*
ASSISTANT::DrawObject* ASSISTANT::AnimatedObject::Copy(Page* page, bool keepZPosition) 
{
   std::list<DrawObject*>::iterator 
      iter;
   AnimatedObject
      *ret;
   DrawObject
      *obj;
  
   ret = new AnimatedObject(Project::active->GetObjectID(), hyperlink_, currentDirectory_);

   for (iter = animatedObjects.begin(); iter != animatedObjects.end(); ++iter) 
   {
      if ((*iter)->GetType() == ASSISTANT::GenericObject::GROUP) 
      {
         obj = ((Group*)*iter)->Copy(page, keepZPosition);
      }
      else 
      {
         obj = (*iter)->Copy();
			if (keepZPosition)
				obj->SetOrder((*iter)->GetOrder());
         if (obj)
         {
            if (obj->GetType() == GenericObject::IMAGE) 
            {
               ((WindowsImage *)obj)->BuildImage(page->GetCanvas(), ((WindowsImage *)(*iter))->GetImage(),  true);
            }
         }
      }
      if (obj) 
      {
         page->AppendObject(obj, false);
         ret->AddObject(obj);
         obj->SetInvisible();
      }
   }

   ret->SetX(GetX());
   ret->SetY(GetY());

   return ret;
}
*/

ASSISTANT::DrawObject* ASSISTANT::AnimatedObject::Copy(bool keepZPosition) 
{
   AnimatedObject
      *ret;
   DrawObject* obj;
   
   ret = new AnimatedObject(0, m_iAnimationId, m_iAnimationType, 0, m_iTrigger, m_iCallerId,  
      hyperlink_, currentDirectory_, m_ssLinkedObjects);
   
   if (visible)
      ret->SetVisible();
   else 
      ret->SetInvisible();

   for (int i = 0; i < animatedObjects.GetSize(); ++i) 
   {
      obj = animatedObjects[i]->Copy(keepZPosition);
      
      if (obj)
      {
         ret->AddObject(obj);
      
         if (animatedObjects[i]->IsVisible())
            obj->SetVisible();
         else
            obj->SetInvisible();
      }
   }
   
  return ret;
}

void ASSISTANT::AnimatedObject::SetVisible()
{
  visible = true;
  
  for (int i = 0; i < animatedObjects.GetSize(); ++i)
    animatedObjects[i]->SetVisible();
}

ASSISTANT::DrawObject *ASSISTANT::AnimatedObject::GetGroup()
{ 
   if (group)
      return group->GetGroup();
   else
      return this;
}


void ASSISTANT::AnimatedObject::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   for (int i = 0; i < animatedObjects.GetSize(); ++i)
      animatedObjects[i]->Draw(pDC, _zoomFactor);
}

void ASSISTANT::AnimatedObject::SetX(double _x)
{
   double diffX;

   diffX = _x - m_dX;
   for (int i = 0; i < animatedObjects.GetSize(); ++i)
   {
      animatedObjects[i]->SetX(animatedObjects[i]->GetX() + diffX);
   }
}

void ASSISTANT::AnimatedObject::SetY(double _y)
{
   double diffY;

   diffY = _y - m_dY;
   for (int i = 0; i < animatedObjects.GetSize(); ++i)
   {
      animatedObjects[i]->SetY(animatedObjects[i]->GetY() + diffY);
   }
}


void ASSISTANT::AnimatedObject::AddObject(ASSISTANT::DrawObject *obj)
{
  int
    x1, y1, x2, y2;

  obj->GetBBox(&x1,&y1,&x2,&y2);
  if (animatedObjects.GetSize() == 0) 
  {
    m_dX = x1;
    m_dY = y1;
    m_dWidth = (x2-m_dX);
    m_dHeight = (y2-m_dY);
  }
  else {
    if (x1 < m_dX) m_dX = x1;
    if (y1 < m_dY) m_dY = y1;
    if ((x2-m_dX) > m_dWidth) m_dWidth = (x2-m_dX); 
    if ((y2-m_dY) > m_dHeight) m_dHeight = (y2-m_dY); 
  }

  obj->SetGroup(this);
  
  if (actItem != -1) 
  {
     ++actItem;
      animatedObjects.InsertAt(actItem, obj);
  }
  else
  {
     animatedObjects.Add(obj);
     actItem = 0;
  }
}

void ASSISTANT::AnimatedObject::Hide()
{
   visible = false;
   
   for (int i = 0; i < animatedObjects.GetSize(); ++i)
      animatedObjects[i]->Hide();
}

void ASSISTANT::AnimatedObject::SetInvisible()
{
  visible = false;
  
   for (int i = 0; i < animatedObjects.GetSize(); ++i)
    animatedObjects[i]->SetInvisible();
}


void ASSISTANT::AnimatedObject::Zoom(float _zoomFactor)
{
}

bool ASSISTANT::AnimatedObject::IsInRegion(int _x1, int _y1, int _x2, int _y2)
{
  int 
    x1, y1,
    x2, y2;

  if (IsAllInvisible()) return false;

  GetBBox(&x1,&y1,&x2,&y2);
  if ((x1 < _x1) || (y1 < _y1) || (x2 > (_x2)) || (y2 > (_y2))) return false;

  return true;
}

void ASSISTANT::AnimatedObject::UpdateDimension()
{
   int
      x1, y1,
      x2, y2;
   
   if (animatedObjects.GetSize() == 0) return;
   
   (animatedObjects[0])->GetBBox(&x1,&y1,&x2,&y2);
   m_dX = x1;
   m_dY = y1;
   m_dWidth = x2-x1;
   m_dHeight = y2-y1;
   for (int i = 1; i < animatedObjects.GetSize(); ++i)
   {
      animatedObjects[i]->GetBBox(&x1,&y1,&x2,&y2);
      if (x1 < m_dX) m_dX = x1;
      if (y1 < m_dY) m_dY = y1;
      if ((x2-m_dX) > m_dWidth) m_dWidth = x2 - m_dX;
      if ((y2-m_dY) > m_dHeight) m_dHeight = y2 - m_dY;
   }
}


void ASSISTANT::AnimatedObject::SetFirstActive() 
{
  actItem = 0;
}


void ASSISTANT::AnimatedObject::SetLastActive() 
{
  actItem = animatedObjects.GetSize() - 1;
}


void ASSISTANT::AnimatedObject::SetLeftActive() 
{
   if (animatedObjects.GetSize() == 0) 
      return;
   
   if (actItem > 0)
      --actItem;
}


void ASSISTANT::AnimatedObject::SetRightActive() 
{
   if (animatedObjects.GetSize() == 0)
      return;
   
   if (actItem < animatedObjects.GetSize() - 1)
      ++actItem;
}

void ASSISTANT::AnimatedObject::SetActiveItem(DrawObject* obj) 
{
   for (int i = 1; i < animatedObjects.GetSize(); ++i)
   {
    if (animatedObjects[i] == obj) 
    {
      actItem = i;
      break;
    }
  }
}

void ASSISTANT::AnimatedObject::LoadObjects(ASSISTANT::Page *page, SGMLElement *_root, 
                                            LPCTSTR path, CStringArray &arLsdTmpFileNames, bool append)
{
   SGMLElement 
      *hilf; 
   DrawObject
      *obj;

   visible = false;

   for (hilf = _root->son; hilf != NULL; hilf = hilf->next) 
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
	      obj =Image::Load(hilf, page->GetImagePath(), arLsdTmpFileNames);
      }
      else if (hilf->Name == _T("TEXT")) 
      {
         obj = Text::Load(hilf);
      }
      else if (hilf->Name == _T("GROUP")) 
      {
         obj = Group::Load(hilf);
         if (obj)
         {
            ((Group *)obj)->LoadObjects(page, hilf, path, arLsdTmpFileNames, false);
            if (obj)
            {
               delete obj;
               obj = NULL;
            }
         }
      }

      if (obj) 
      {
         ((ASSISTANT::Page *)page)->AppendObject(obj, false);
      }
   }
}


void ASSISTANT::AnimatedObject::AdjustGui() 
{
}


ASSISTANT::DrawObject *ASSISTANT::AnimatedObject::Load(SGMLElement *hilf) 
{
   int iAnimationId = 0;
   TCHAR *tmp = hilf->GetAttribute("id");
   if (tmp)  
      iAnimationId = _ttoi(tmp);
   
   int iCallerId = 0;
   tmp = hilf->GetAttribute("callId");
   if (tmp)  
      iCallerId = _ttoi(tmp);

   int iAnimationType = ENTER;
   tmp = hilf->GetAttribute("type");
   if (tmp)  
      iAnimationType = _ttoi(tmp);

   int iClickId = 0;
   tmp = hilf->GetAttribute("clickId");
   if (tmp)  
      iClickId = _ttoi(tmp);

   int iTrigger = 0;
   tmp = hilf->GetAttribute("trigger");
   if (tmp)  
      iTrigger = _ttoi(tmp);

   AnimatedObject *pObject = new ASSISTANT::AnimatedObject(0, iAnimationId, iAnimationType, iClickId, iTrigger, iCallerId, NULL, NULL, NULL);
   
   return pObject;
}


bool ASSISTANT::AnimatedObject::HasHyperlink()
{
   if (animatedObjects.GetSize() == 0)
      return false;
	
   CString firstHyperlink = (animatedObjects[0])->GetHyperlink();
	
   for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
      {
         if (!obj->HasHyperlink())
            return false;
         else if (firstHyperlink != obj->GetHyperlink())
            return false;
			
      }
   }
   
   return true;
}

bool ASSISTANT::AnimatedObject::HasObjectLinks()
{
   if (animatedObjects.GetSize() == 0)
      return false;
	
   CString firstObjectLink = (animatedObjects[0])->GetInteractiveObjects();
	
   for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
      {
         if (!obj->HasObjectLinks())
            return false;
         else if (firstObjectLink != obj->GetInteractiveObjects())
            return false;
			
      }
   }
   
   return true;
}

LPCTSTR ASSISTANT::AnimatedObject::GetInteractiveObjects()
{
   if (animatedObjects.GetSize() == 0)
      return NULL;
	
   static CString firstObjectLinks;
	
   firstObjectLinks = (animatedObjects[0])->GetInteractiveObjects();
	
   for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
      {
         if (!obj->HasObjectLinks())
            return NULL;
         else if (firstObjectLinks != obj->GetInteractiveObjects())
            return NULL;
			
      }
   }
	
   CString objectLinks = firstObjectLinks;
	
   return firstObjectLinks;
}

LPCTSTR ASSISTANT::AnimatedObject::GetHyperlink()
{
   if (animatedObjects.GetSize() == 0)
      return NULL;
	
   static CString firstHyperlink;
	
   firstHyperlink = (animatedObjects[0])->GetHyperlink();
	
   for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
      {
         if (!obj->HasHyperlink())
            return NULL;
         else if (firstHyperlink != obj->GetHyperlink())
            return NULL;
			
      }
   }
	
   CString hyperlink = firstHyperlink;
	
   return firstHyperlink;
}

LPCTSTR ASSISTANT::AnimatedObject::GetLinkDirectory()
{
   if (animatedObjects.GetSize() == 0)
      return NULL;
	
   CString firstHyperlink;
	
   firstHyperlink = (animatedObjects[0])->GetHyperlink();
   static CString linkDirectory = (animatedObjects[0])->GetLinkDirectory();
	
   for (int i = 0; i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
      {
         if (!obj->HasHyperlink())
            return NULL;
         else if (firstHyperlink != obj->GetHyperlink())
            return NULL;
			
      }
   }
	
   return linkDirectory;
}

bool ASSISTANT::AnimatedObject::IsInternLink() 
{ 
   if (animatedObjects.GetSize() == 0)
      return NULL;
	
   bool isInternLink;
	
   isInternLink = (animatedObjects[0])->IsInternLink();
	
   for (int i = 0; i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
      {
         if (isInternLink != obj->IsInternLink())
            return false;   
      }
   }
	
   return isInternLink;
}

void ASSISTANT::AnimatedObject::LinkIsIntern(bool isIntern) 
{ 
   if (animatedObjects.GetSize() == 0)
      return;
	
   for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
         obj->LinkIsIntern(isIntern);
   }
	
}

void ASSISTANT::AnimatedObject::ActivateHyperlink() 
{
   for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
         obj->ActivateHyperlink();
   }
}


void ASSISTANT::AnimatedObject::InsertInVisibilityOrder(CArray<DrawObject *, DrawObject *> &sortedList)
{
	// only the group objects, not the group, are inserted into the list
	for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
      {
			obj->InsertInVisibilityOrder(sortedList);
      }
   } 
}

void ASSISTANT::AnimatedObject::AppendObjectsToPage(Page *page)
{ 
	
	for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
		if (obj) 
		{
			if (obj->GetType() == GROUP)
			{
				((Group *)obj)->AppendObjectsToPage(page);
			}
         else if (obj->GetType() == ANIMATED) 
         {
            ((AnimatedObject *)obj)->AppendObjectsToPage(page);
         }
			page->AppendObject(obj, false);
			
			if (visible)
				obj->SetVisible();
			else
				obj->SetInvisible();
		}
   }
	
	UpdateDimension();
}

bool ASSISTANT::AnimatedObject::IsIdentic(DrawObject *obj)
{
   if (this == obj)
      return true;

   if (GetType() != obj->GetType())
      return false;

   // If all objects belonging to animated object are identic and in the same order, 
   // the animated objects are identic.
   if (animatedObjects.GetSize() != ((AnimatedObject *)obj)->animatedObjects.GetSize())
      return false;

   for (int i = 0; i < animatedObjects.GetSize(); ++i)
   {
      if (!animatedObjects[i]->IsIdentic(((AnimatedObject *)obj)->animatedObjects[i]))
         return false;
   }

   return true;
}

void ASSISTANT::AnimatedObject::Hide(AnimatedObject *animObject)
{
   if (IsAllInvisible()) 
      return;

   //if (selected) DeleteSelection();
   
   for (int i = 0;i < animatedObjects.GetSize(); ++i)
   {
      for (int j = 0; j < animObject->animatedObjects.GetSize(); ++j)
      {
         if (animatedObjects[i]->IsIdentic(animObject->animatedObjects[j]))
            animatedObjects[i]->Hide();
      }
   }
}


void ASSISTANT::AnimatedObject::Show(AnimatedObject *animObject)
{
   visible = true;
   
   for (int i = 0;i < animatedObjects.GetSize(); ++i)
   {
      for (int j = 0; j < animObject->animatedObjects.GetSize(); ++j)
      {
         if (animatedObjects[i]->IsIdentic(animObject->animatedObjects[j]))
            animatedObjects[i]->SetVisible();
      }
   }
}

bool ASSISTANT::AnimatedObject::IsAllInvisible()
{
   bool allInvisible = true;
   
   for (int i =0 ; i < animatedObjects.GetSize(); ++i)
   {
      if (animatedObjects[i]->IsVisible())
      {
         allInvisible = false;
         break;
      }
   }

   return allInvisible;
}
