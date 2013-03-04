/*********************************************************************

  program  : mlb_group.cpp
  authors  : Gabriela Maass
  date     : 11.03.2001
  revision : $Id: GroupObject.cpp,v 1.1 2007-08-23 05:09:08 maass Exp $
  desc     : Object to group objects
  
**********************************************************************/

#include "StdAfx.h"

#include "DrawObjects.h"
#include "PageObject.h"

ASSISTANT::Group::Group(UINT _id, LPCTSTR _hyperlink, LPCTSTR _currentDirectory, LPCTSTR _linkedObjects) 
: DrawObject(0, 0, 0, 0, -1, _id, _hyperlink, _currentDirectory, _linkedObjects) 
{
	textGroup = false;
	actItem = -1;
   m_clrText = RGB(0, 0, 0);
   
   Text::FillLogFont(m_logFont, _T("Arial"), 10, _T("normal"), _T("roman"));
}

ASSISTANT::Group::~Group()
{
}

ASSISTANT::DrawObject* ASSISTANT::Group::GetObject(int _idx) 
{
	if ((_idx < 0) || (_idx >= groupObjects.GetSize())) return NULL;
	
	return groupObjects[_idx];
}

bool ASSISTANT::Group::AllEmpty() 
{
	if (!textGroup) return false;
	
	if (groupObjects.GetSize() == 0) return true;
	
	for (int i = 0; i < groupObjects.GetSize(); ++i)
		if (((Text*)groupObjects[i])->GetSize() > 0) return false; // found one being not empty
		
		return true;
}

ASSISTANT::DrawObject* ASSISTANT::Group::Copy(bool keepZPosition) 
{
	Group
		*ret;
	DrawObject* obj;
	
	ret = new Group(0, NULL, NULL, NULL);
	
	for (int i = 0; i < groupObjects.GetSize(); ++i) 
	{
		obj = groupObjects[i]->Copy(keepZPosition); 
		if (obj)
      {
			//ret->AddObject(obj);
      }
	}
	
	return ret;
}

void ASSISTANT::Group::SetVisible()
{
	visible = true;
	
	for (int i = 0; i < groupObjects.GetSize(); ++i)
		groupObjects[i]->SetVisible();
}

ASSISTANT::DrawObject *ASSISTANT::Group::GetGroup()
{
   if (group) 
   {
      return group->GetGroup();
   }
   else       
      return this;
}

ASSISTANT::DrawObject *ASSISTANT::Group::GetActive()
{
	if (actItem != -1)
	{
		return groupObjects[actItem];
	}
	else
	{
		return this;
	}
}

void ASSISTANT::Group::AddObject(ASSISTANT::DrawObject *obj)
{
   if (obj == NULL)
      return;

	if (obj->GetType() == TEXT) 
   {
		if (groupObjects.GetSize() == 0 || textGroup) 
			textGroup = true;
		else 
			textGroup = false;
      
      ((Text *)obj)->GetLogFont(&m_logFont);
      CString csColor = ((Text *)obj)->GetLineColor();
      m_clrText = CalculateColor(csColor);
	}
	
	obj->SetGroup(this);
	
   groupObjects.Add(obj);
   
}


void ASSISTANT::Group::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) return;
   
   for (int i = 0; i < groupObjects.GetSize(); ++i)
      groupObjects[i]->Draw(pDC, _zoomFactor, dOffX, dOffY);

}

void ASSISTANT::Group::SetX(double _x)
{
   double diffX;
	
   diffX = _x - m_dX;
   for (int i = 0; i < groupObjects.GetSize(); ++i)
   {
      groupObjects[i]->SetX(groupObjects[i]->GetX() + diffX);
   }
   m_dX = _x;
}

void ASSISTANT::Group::SetY(double _y)
{
   double diffY;
	
   diffY = _y - m_dY;
   for (int i = 0; i < groupObjects.GetSize(); ++i)
   {
      groupObjects[i]->SetY(groupObjects[i]->GetY() + diffY);
   }
   m_dY = _y;
}


void ASSISTANT::Group::Hide()
{
   if (!visible) 
      return;
	
   visible = false;
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      groupObjects[i]->Hide();
   }
}

void ASSISTANT::Group::SetInvisible()
{
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      groupObjects[i]->SetInvisible();
   }
}


void ASSISTANT::Group::Zoom(float _zoomFactor)
{
}

void ASSISTANT::Group::UpdateDimension()
{
	int
		x1, y1,
		x2, y2;
   
   if (groupObjects.GetSize() == 0) return;
   
   groupObjects[0]->GetBBox(&x1,&y1,&x2,&y2);
   m_dX = x1;
   m_dY = y1;
   m_dWidth = x2-x1;
   m_dHeight = y2-y1;
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      if (groupObjects[i]->IsVisible())
      {
         groupObjects[i]->GetBBox(&x1,&y1,&x2,&y2);
         if (x1 < m_dX) m_dX = x1;
         if (y1 < m_dY) m_dY = y1;
         if ((x2-m_dX) > m_dWidth) m_dWidth = x2 - m_dX;
         if ((y2-m_dY) > m_dHeight) m_dHeight = y2 - m_dY;
      }
   }
}


void ASSISTANT::Group::SetFirstActive() 
{
   actItem = 0;
}


void ASSISTANT::Group::SetLastActive() 
{
   actItem = groupObjects.GetSize() - 1;
}


void ASSISTANT::Group::SetLeftActive() 
{
   if (groupObjects.GetSize() == 0) 
      return;
	
   if (actItem != 0)
      --actItem;
}


void ASSISTANT::Group::SetRightActive() 
{
   if (groupObjects.GetSize() == 0) 
      return;
	
   if (actItem < groupObjects.GetSize()-1)
      ++actItem;
}


void ASSISTANT::Group::SetActiveItem(DrawObject* obj) 
{
   for (int i = 0; i < groupObjects.GetSize(); ++i)
   {
      if (groupObjects[i] == obj) 
      {
         actItem = i;
         break;
      }
   }
}

bool ASSISTANT::Group::IsTextGroup() 
{
   return textGroup;
}
void ASSISTANT::Group::LoadObjects(ASSISTANT::Page *page, SGMLElement *_root, 
                                   LPCTSTR path, CStringArray &arLsdTmpFileNames, bool append)
{
   SGMLElement 
      *hilf; 
   DrawObject
      *obj;
   bool
      bIsMasterGroup = false;
   CString csImagePath = path;
	
   if (_root->GetAttribute("type") != NULL &&
       _tcscmp(_root->GetAttribute("type"), _T("MASTER")) == 0)
      bIsMasterGroup = true;

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
			obj = Image::Load(hilf, csImagePath, arLsdTmpFileNames);
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
            ((Group *)obj)->LoadObjects(page, hilf, path, arLsdTmpFileNames);
            if (obj)
            {
               delete obj;
               obj = NULL;
            }
         }
      }
      if (obj) 
      {
         ((ASSISTANT::Page *)page)->AppendObject(obj);
      }
   }
}

ASSISTANT::DrawObject *ASSISTANT::Group::Load(SGMLElement *hilf) 
{
	Group 
		*obj;
	
	obj = new ASSISTANT::Group(0, NULL, NULL, NULL);
	
	return obj;
}

bool ASSISTANT::Group::HasHyperlink()
{
   if (groupObjects.GetSize() == 0)
      return false;
	
   CString firstHyperlink = (groupObjects[0])->GetHyperlink();
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
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


bool ASSISTANT::Group::HasObjectLinks()
{
   if (groupObjects.GetSize() == 0)
      return false;
	
   CString firstObjectLink = (groupObjects[0])->GetInteractiveObjects();
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
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


LPCTSTR ASSISTANT::Group::GetInteractiveObjects()
{
   if (groupObjects.GetSize() == 0)
      return NULL;
	
   static CString firstObjectLinks;
	
   firstObjectLinks = (groupObjects[0])->GetInteractiveObjects();
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj) 
      {
         if (!obj->HasObjectLinks())
            return NULL;
         else if (firstObjectLinks != obj->GetInteractiveObjects())
            return NULL;
			
      }
   }
	
   return firstObjectLinks;
}

LPCTSTR ASSISTANT::Group::GetHyperlink()
{
   if (groupObjects.GetSize() == 0)
      return NULL;
	
   static CString firstHyperlink;
	
   firstHyperlink = (groupObjects[0])->GetHyperlink();
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj) 
      {
         if (!obj->HasHyperlink())
            return NULL;
         else if (firstHyperlink != obj->GetHyperlink())
            return NULL;
			
      }
   }
	
   return firstHyperlink;
}

LPCTSTR ASSISTANT::Group::GetLinkDirectory()
{
   if (groupObjects.GetSize() == 0)
      return NULL;
	
   CString firstHyperlink;
	
   firstHyperlink = (groupObjects[0])->GetHyperlink();
   static CString linkDirectory = (groupObjects[0])->GetLinkDirectory();
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
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

bool ASSISTANT::Group::IsInternLink() 
{ 
   if (groupObjects.GetSize() == 0)
      return NULL;
	
   bool isInternLink;
	
   isInternLink = (groupObjects[0])->IsInternLink();
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj) 
      {
         if (isInternLink != obj->IsInternLink())
            return false;   
      }
   }
	
   return isInternLink;
}

void ASSISTANT::Group::LinkIsIntern(bool isIntern) 
{ 
   if (groupObjects.GetSize() == 0)
      return;
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj) 
         obj->LinkIsIntern(isIntern);
   }
	
}

void ASSISTANT::Group::ActivateHyperlink() 
{
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj) 
         obj->ActivateHyperlink();
   }
}

void ASSISTANT::Group::ActivateAssociatedHyperlink(DrawObject *hyperlink) 
{
   if (hyperlink->GetType() != TEXT)
      return;
	
   bool objFound = false;
   int hyperlinkPos = groupObjects.GetSize();
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (objFound && obj->GetType() != TEXT)
         break;
      if (objFound)
      {
         if (_tcscmp(obj->GetHyperlink(), hyperlink->GetHyperlink()) == 0)
            obj->ActivateHyperlink();
         else 
            break;
      }
      if (obj == hyperlink) 
      {
         objFound = true;
         hyperlinkPos = i;
      }
   } 
   
   // hyperlink was not found
   if (hyperlinkPos == groupObjects.GetSize() || hyperlinkPos == 0)
      return; 
	
   i = hyperlinkPos;
   do 
   {
      --i; 
      DrawObject *obj = groupObjects[i];
      if (objFound && obj->GetType() != TEXT)
         break;
      if (objFound)
      {
         if (_tcscmp(obj->GetHyperlink(), hyperlink->GetHyperlink()) == 0)
            obj->ActivateHyperlink();
         else 
            break;
      }
		
   } while (i < groupObjects.GetSize());
   
}


void ASSISTANT::Group::InsertInVisibilityOrder(CArray<DrawObject *, DrawObject *> &sortedList)
{
	// only the group objects, not the group, are inserted into the list
	for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj) 
      {
			obj->InsertInVisibilityOrder(sortedList);
      }
   } 
}

void ASSISTANT::Group::AppendObjectsToPage(Page *page)
{ 
	
	for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
		if (obj) 
		{
			if (groupObjects[i]->GetType() == GROUP)
			{
				((Group *)obj)->AppendObjectsToPage(page);
			}
         else if (groupObjects[i]->GetType() == ANIMATED) 
         {
            ((AnimatedObject *)obj)->AppendObjectsToPage(page);
         }
			page->AppendObject(obj, false);
		}
   }
	
}

bool ASSISTANT::Group::IsIdentic(DrawObject *obj)
{
   if (this == obj)
      return true;

   if (GetType() != obj->GetType())
      return false;

   // If all objects belonging to group are identic and in the same order, 
   // the groups are identic.
   if (groupObjects.GetSize() != ((Group *)obj)->groupObjects.GetSize())
      return false;
   
   for (int i = 0; i < groupObjects.GetSize(); ++i)
   {
      if (!groupObjects[i]->IsIdentic(((Group *)obj)->groupObjects[i]))
         return false;
   }

   return true;
}
