/*********************************************************************

 program : mlb_animate.cpp
 authors : Gabriela Maass
 date    : 11.03.2001
 desc    : Animated objects

 **********************************************************************/

#include "StdAfx.h"

#include "mlb_objects.h"
#include "la_project.h"


ASSISTANT::AnimatedObject::AnimatedObject(uint32 _id, int iAnimationId, int iAnimationType, 
                                          int iClickId, int iTrigger, int iCallerId, 
                                          LPCTSTR _hyperlink, LPCTSTR _currentDirectory,LPCTSTR _linkedObjects) 
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


ASSISTANT::DrawObject* ASSISTANT::AnimatedObject::GetObjectFromID(uint32 _id) 
{ 
  for (int i = 0; i < animatedObjects.GetSize(); ++i)
    if (animatedObjects[i]->GetID() == _id) {
      return animatedObjects[i];
    }

  return NULL;
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
   
   ret = new AnimatedObject(Project::active->GetObjectID(), m_iAnimationId, m_iAnimationType, 0, m_iTrigger, m_iCallerId,  
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

void ASSISTANT::AnimatedObject::Move(float _diffX, float _diffY)
{
  recorded = false;

  m_dX += _diffX;
  m_dY += _diffY;

   for (int i = 0; i < animatedObjects.GetSize(); ++i)
      animatedObjects[i]->Move(_diffX,_diffY);
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


void ASSISTANT::AnimatedObject::SaveObject(FILE *fp, int level, LPCTSTR path)
{
   if (animatedObjects.GetSize() == 0) return;
   
   WCHAR *tab = (WCHAR *)malloc((level*2+1)*sizeof(WCHAR));
   *tab = L'\0';
   for (int i = 0; i < level; i++) 
      wcscat(tab, L"  ");
   
   fwprintf(fp, L"%s<ANIMATED id=\"%d\" type=\"%d\" clickId=\"%d\" trigger=\"%d\" callId=\"%d\">\n", 
                tab, m_iAnimationId, m_iAnimationType, m_iClickId, m_iTrigger, m_iCallerId);
   
   for (i = 0; i < animatedObjects.GetSize(); ++i)
   {
      bool bShown = true;
      if (!animatedObjects[i]->IsVisible())
      {
         bShown = false;
         animatedObjects[i]->SetVisible();
      }
      animatedObjects[i]->SaveObject(fp, (level+1), path);
      if (!bShown)
         animatedObjects[i]->SetInvisible();
   }

   fwprintf(fp, L"%s</ANIMATED>\n", tab);
   
   if (tab)
      delete tab;
}

void ASSISTANT::AnimatedObject::SetUnrecorded()
{
   recorded = false;
   for (int i = 0; i < animatedObjects.GetSize(); ++i)
      animatedObjects[i]->SetUnrecorded();
}

int ASSISTANT::AnimatedObject::RecordableObjects()
{
   if (IsAllInvisible() || animatedObjects.GetSize() == 0) 
      return 0;
   
   return 1;
}

void ASSISTANT::AnimatedObject::SaveAsAOF(CFileOutput *fp)
{
   if (IsAllInvisible() || animatedObjects.GetSize() == 0 || recorded) 
      return;

   recorded = true;

   recID = ASSISTANT::Project::active->recCount;
   ASSISTANT::Project::active->recCount++;

   CString line;

   line.Format(_T("<ANIMATED>\n"));
   fp->Append(line);

   for (int i = 0; i < animatedObjects.GetSize(); ++i)
      animatedObjects[i]->SaveAsAOF(fp);

   line.Format(_T("</ANIMATED>\n"));
   fp->Append(line);
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

ASSISTANT::GenericObject* ASSISTANT::AnimatedObject::DeleteObject(GenericObject* obj) 
{
   if (animatedObjects.GetSize() == 0) return NULL;
   
   for (int i = 0; i < animatedObjects.GetSize(); ++i)
   {
      if (animatedObjects[i] == obj) 
      {
         animatedObjects.RemoveAt(i);
         UpdateDimension();
         return obj;
      }
   }
   
   return NULL;
}


ASSISTANT::DrawObject* ASSISTANT::AnimatedObject::DeleteObject(uint32 id) 
{ 
   if (animatedObjects.GetSize() == 0) return NULL;
   
   for (int i = 0; i < animatedObjects.GetSize(); ++i)
   {
      if (animatedObjects[i]->GetID() == id) 
      {
         DrawObject *pRet = animatedObjects[i];
         animatedObjects.RemoveAt(i);
         UpdateDimension();
         return pRet;
      }
   }

  return NULL;
}



void ASSISTANT::AnimatedObject::DeleteAll() 
{
   if (animatedObjects.GetSize() == 0) return;
   
   for (int i = 0; i < animatedObjects.GetSize(); ++i)
   {
      /* Objekte einer Gruppe sind nicht , sondern nur auf der Seite */
      animatedObjects[i]->Hide();
   }
   animatedObjects.RemoveAll();
}


void ASSISTANT::AnimatedObject::DrawSelection(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (IsAllInvisible()) return;
   
   selected = true;
   
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform(dOffX, dOffY);
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);

   selectPoints[0].X = m_dX;
   selectPoints[0].Y = m_dY;
   selectPoints[1].X = m_dX+m_dWidth;
   selectPoints[1].Y = m_dY;
   selectPoints[2].X = m_dX+m_dWidth;
   selectPoints[2].Y = m_dY+m_dHeight;
   selectPoints[3].X = m_dX;
   selectPoints[3].Y = m_dY+m_dHeight;
      
   Gdiplus::Color clrWhite(255, 255, 255, 255);
   Gdiplus::Color clrBlack(255, 0, 0, 0);

   Gdiplus::REAL fLineWidth = 6.0 / _zoomFactor;
   Gdiplus::REAL fRadius = 3.0 / _zoomFactor;

   Gdiplus::REAL fPenWidth = 0.5f;
   Gdiplus::Pen blackPen(clrBlack, fPenWidth);
   Gdiplus::SolidBrush whiteBrush(clrWhite);
      
   for (int i = 0; i < 4; i++) 
   {
      graphics.FillEllipse(&whiteBrush, 
         (selectPoints[i].X - fRadius), (selectPoints[i].Y - fRadius), 
         2*fRadius, 2*fRadius);
      graphics.DrawEllipse(&blackPen, 
         (selectPoints[i].X - fRadius), (selectPoints[i].Y - fRadius), 
         2*fRadius, 2*fRadius);
   }
}

void ASSISTANT::AnimatedObject::Hide()
{
   visible = false;
   selected = false;
   
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


bool ASSISTANT::AnimatedObject::IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditType, int &iPointIndex, double dZoomFactor)
{
   if (IsAllInvisible() || !selected) 
      return 0;
  
   double dTolerance = 3 / dZoomFactor;
   
   for (int i = 0; i < 4; i++) 
   {
      if ((ptObject.X > selectPoints[i].X-dTolerance) && (ptObject.X < selectPoints[i].X+dTolerance)) 
      {
         if ((ptObject.Y > selectPoints[i].Y-dTolerance) && (ptObject.Y < selectPoints[i].Y+dTolerance)) 
         {
            nEditType = MOVE;
            return true;
         }
      }
   }

   if (ContainsPoint(ptObject))
   {
      nEditType = MOVE;
      return true;
   }

   return false;
}

void ASSISTANT::AnimatedObject::GetBBox(int *minX, int *minY, int *maxX, int *maxY)
{  
  UpdateDimension();
  if ((m_dX+m_dWidth) < m_dX) {
    *minX = m_dX+m_dWidth-3;
    *maxX = m_dX+3;
  }
  else {
    *minX = m_dX-3;
    *maxX = m_dX+m_dWidth+3;
  }
  if ((m_dY+m_dHeight) < m_dY) {
    *minY = m_dY+m_dHeight-3;
    *maxY = m_dY+3;
  }
  else {
    *minY = m_dY-3;
    *maxY = m_dY+m_dHeight+3;
  }

}

void ASSISTANT::AnimatedObject::GetBoundingBox(CRect &rcObject)
{
   UpdateDimension();
   
   rcObject.left = m_dX;
   rcObject.right = m_dX + m_dWidth;
   rcObject.top = m_dY;
   rcObject.bottom = m_dY + m_dHeight;
   
   rcObject.NormalizeRect();
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

bool ASSISTANT::AnimatedObject::Overlap(int _x, int _y, float _zoomFactor) 
{
   int 
      x1, y1,
      x2, y2;
   bool
      overlap;
   
   if (IsAllInvisible()) return false;
   
   GetBBox(&x1,&y1,&x2,&y2);
   if ((_x < x1*_zoomFactor) || (_y < y1*_zoomFactor) || (_x > x2*_zoomFactor) || (_y > y2*_zoomFactor)) 
      return false;
   
   overlap = false;
   for (int i = 0; i < animatedObjects.GetSize(); ++i)
   {
      if (animatedObjects[i]->Overlap(_x,_y, _zoomFactor)) 
      {
         overlap = true;
         actItem = i;
         break;
      }
   }

  return overlap;
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

void ASSISTANT::AnimatedObject::ChangeLineColor(Gdiplus::ARGB argbLineColor) 
{
   if (IsAllInvisible() || animatedObjects.GetSize() == 0) return;
   
   recorded = false;
   for (int i = 1; i < animatedObjects.GetSize(); ++i)
   {
      if (animatedObjects[i]->GetType() != GenericObject::IMAGE) 
         ((ColoredObject *)animatedObjects[i])->ChangeLineColor(argbLineColor);
   }
}

void ASSISTANT::AnimatedObject::ChangeFillColor(Gdiplus::ARGB argbFillColor) 
{
   if (IsAllInvisible() || animatedObjects.GetSize() == 0) return;
   
   recorded = false;
   for (int i = 1; i < animatedObjects.GetSize(); ++i)
   {
      if ((animatedObjects[i]->GetType() != GenericObject::IMAGE) && (animatedObjects[i]->GetType() != GenericObject::ASCIITEXT)) 
         ((ColoredObject *)animatedObjects[i])->ChangeFillColor(argbFillColor);
   }
}

void ASSISTANT::AnimatedObject::ChangeLineStyle(Gdiplus::DashStyle gdipLineStyle) 
{
   if (IsAllInvisible() || animatedObjects.GetSize() == 0) return;
   
   recorded = false;
   for (int i = 1; i < animatedObjects.GetSize(); ++i)
   {
      if ((animatedObjects[i]->GetType() != GenericObject::IMAGE) && 
          (animatedObjects[i]->GetType() != GenericObject::ASCIITEXT)) 
         ((ColoredObject *)animatedObjects[i])->ChangeLineStyle(gdipLineStyle);
   }
}


void ASSISTANT::AnimatedObject::ChangeLineWidth(int _lineWidth) 
{
  if (IsAllInvisible() || animatedObjects.GetSize() == 0) return;
  
  recorded = false;
   for (int i = 1; i < animatedObjects.GetSize(); ++i)
   {
    if ((animatedObjects[i]->GetType() != GenericObject::IMAGE) && (animatedObjects[i]->GetType() != GenericObject::ASCIITEXT)) 
	    ((ColoredObject *)animatedObjects[i])->ChangeLineWidth(_lineWidth);
  }
}


void ASSISTANT::AnimatedObject::LoadObjects(ASSISTANT::Page *page, LPCTSTR szPageNumber, 
                                            SGMLElement *_root, LPCTSTR path, bool append)
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
	      obj = WindowsImage::Load(hilf, page->GetImagePath());
         if (obj)
         {
            ((WindowsImage *)obj)->BuildImage(page, path, false);
         }
      }
      else if (hilf->Name == _T("TEXT")) 
      {
         obj = Text::Load(hilf);
         if (obj)
         {
            FontList *fontList = ASSISTANT::Project::active->GetFontList();
            if (!fontList->HasTTFFilename(((Text *)obj)->GetFamily()))
            {
               ((Text *)obj)->SetFontUnsupported();
               fontList->AddErrorPage(((Text *)obj)->GetFamily(), szPageNumber, page->GetPageNumber());
            }
         }
      }
      else if (hilf->Name == _T("GROUP")) 
      {
         obj = Group::Load(hilf);
         ((Group *)obj)->LoadObjects(page,szPageNumber,hilf,false);
      }

      if (obj) 
      {
         ((ASSISTANT::Page *)page)->AppendObject(obj, false);
         AddObject(obj);
         ((ASSISTANT::Page *)page)->InActivateObject(obj);
         obj->SetInvisible();
      }
   }
}


void ASSISTANT::AnimatedObject::AdjustGui() 
{
}


ASSISTANT::DrawObject *ASSISTANT::AnimatedObject::Load(SGMLElement *hilf) 
{
   TCHAR *tmp;

   int iAnimationId = 0;

   tmp = hilf->GetAttribute("id");
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

   AnimatedObject *pObject = new ASSISTANT::AnimatedObject(Project::active->GetObjectID(), iAnimationId, iAnimationType, iClickId, iTrigger, iCallerId, NULL, NULL, NULL);
   
   return pObject;
}


void ASSISTANT::AnimatedObject::TakeParameter(DrawObject *_obj) 
{
  DrawObject
    *obj;
  CString
    string1,
    string2; 
  float
      diffX,
      diffY,
      zoomFactor;

   zoomFactor = Project::active->GetActivePage()->GetZoomFactor();

  for (int i = 0; i < animatedObjects.GetSize(); ++i)
  {
    obj = ((AnimatedObject *)_obj)->GetObjectFromID(animatedObjects[i]->GetID());
    if (obj) 
    { 
      if (animatedObjects[i]->GetType() == GenericObject::ANIMATED) 
      {
	      ((AnimatedObject *)animatedObjects[i])->TakeParameter(obj);
      }
      else if (animatedObjects[i]->GetType() == GenericObject::GROUP) {
	      ((Group *)animatedObjects[i])->TakeParameter(obj);
      }
      else {
      	if ((obj->GetX() != animatedObjects[i]->GetX()) || (obj->GetY() != animatedObjects[i]->GetY())) {
	            diffX = (obj->GetX() - animatedObjects[i]->GetX());
	            diffY = (obj->GetY() - animatedObjects[i]->GetY());
	            animatedObjects[i]->Move(diffX,diffY);
	      }
	      if (animatedObjects[i]->GetWidth() != obj->GetWidth()) {
	        animatedObjects[i]->SetWidth(obj->GetWidth());
	      }
	      if (animatedObjects[i]->GetHeight() != obj->GetHeight()) {
	        animatedObjects[i]->SetHeight(obj->GetHeight());
	      }
	      if (animatedObjects[i]->GetType() != GenericObject::IMAGE) {
	        if (((ColoredObject *)animatedObjects[i])->GetLineColor() != ((ColoredObject *)obj)->GetLineColor()) {
	          ((ColoredObject *)animatedObjects[i])->ChangeLineColor(((ColoredObject *)obj)->GetLineColor());
	        }
	        if (((ColoredObject *)animatedObjects[i])->GetFillColor() != ((ColoredObject *)obj)->GetFillColor()) {
	          ((ColoredObject *)animatedObjects[i])->ChangeFillColor(((ColoredObject *)obj)->GetFillColor());
	        }
	        if (((ColoredObject *)animatedObjects[i])->GetLineWidth() != ((ColoredObject *)obj)->GetLineWidth()) {
	          ((ColoredObject *)animatedObjects[i])->ChangeLineWidth(((ColoredObject *)obj)->GetLineWidth());
	        }
	        if (((ColoredObject *)animatedObjects[i])->GetLineStyle() != ((ColoredObject *)obj)->GetLineStyle()) {
	          ((ColoredObject *)animatedObjects[i])->ChangeLineStyle(((ColoredObject *)obj)->GetLineStyle());
	        }
	        if (animatedObjects[i]->GetType() == GenericObject::ASCIITEXT) {
	          string1 = ((Text *)obj)->GetString();
	          string2 = ((Text *)(animatedObjects[i]))->GetString();
	          if (string1 != string2) {
	            ((Text *)animatedObjects[i])->ChangeString(string1);
	          }
	        }
	        if (obj->GetType() == GenericObject::LINE) {
	          if (((Line *)obj)->GetArrowStyle() != ((Line *)animatedObjects[i])->GetArrowStyle()) {
	            ((Line *)obj)->ChangeArrowStyle(((Line *)animatedObjects[i])->GetArrowStyle());
	          }
	          if (((Line *)obj)->GetArrowConfig() != ((Line *)animatedObjects[i])->GetArrowConfig()) {
	            ((Line *)obj)->ChangeArrowConfig(((Line *)animatedObjects[i])->GetArrowConfig());
	          }
	        }
	        if ((obj->GetType() == GenericObject::POLYLINE) || (obj->GetType() == GenericObject::FREEHAND)) {
	          if (((PolyLine *)obj)->GetArrowStyle() != ((PolyLine *)animatedObjects[i])->GetArrowStyle()) {
	            ((PolyLine *)obj)->ChangeArrowStyle(((PolyLine *)animatedObjects[i])->GetArrowStyle());
	          }
	          if (((PolyLine *)obj)->GetArrowConfig() != ((PolyLine *)animatedObjects[i])->GetArrowConfig()) {
	            ((PolyLine *)obj)->ChangeArrowConfig(((PolyLine *)animatedObjects[i])->GetArrowConfig());
	          }
	        }
	      }
      }
    }
    else {
      animatedObjects[i]->SetInvisible();
    }
  }
  UpdateDimension();

}

void ASSISTANT::AnimatedObject::TakeIds(DrawObject *_obj)
{
   DrawObject
      *obj;
   
   for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      obj = ((AnimatedObject *)_obj)->GetObject(i);
      if (obj) 
      {
         if (animatedObjects[i]->GetType() == GenericObject::GROUP)
            ((Group *)animatedObjects[i])->TakeIds(obj);
         animatedObjects[i]->SetID(obj->GetID());
      }
   }
}

bool ASSISTANT::AnimatedObject::HasHyperlink(Gdiplus::PointF &ptMouse)
{
   if (animatedObjects.GetSize() == 0)
      return false;
	
   for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
      {
         if (obj->HasHyperlink(ptMouse))
            return true;	
      }
   }
   
   return false;
}

bool ASSISTANT::AnimatedObject::HasObjectLinks(Gdiplus::PointF &ptMouse)
{
   if (animatedObjects.GetSize() == 0)
      return false;
	
   for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
      {
         if (obj->HasObjectLinks(ptMouse))
            return true;
      }
   }
   
   return false;
}

LPCTSTR ASSISTANT::AnimatedObject::GetInteractiveObjects(Gdiplus::PointF &ptMouse)
{
   if (animatedObjects.GetSize() == 0)
      return NULL;
	
   for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
      {
         if (obj->HasObjectLinks(ptMouse))
            return animatedObjects[i]->GetInteractiveObjects(ptMouse);
			
      }
   }
	
   return NULL;
}

LPCTSTR ASSISTANT::AnimatedObject::GetHyperlink(Gdiplus::PointF &ptMouse)
{
   if (animatedObjects.GetSize() == 0)
      return NULL;
	
   for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj && obj->ContainsPoint(ptMouse)) 
      {
         if (obj->HasHyperlink(ptMouse))
            return obj->GetHyperlink(ptMouse);
      }
   }
	
   return NULL;
}

LPCTSTR ASSISTANT::AnimatedObject::GetLinkDirectory(Gdiplus::PointF &ptMouse)
{
   if (animatedObjects.GetSize() == 0)
      return NULL;
	
   for (int i = 0; i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj && obj->ContainsPoint(ptMouse)) 
      {
         if (obj->HasHyperlink(ptMouse))
            return obj->GetLinkDirectory(ptMouse);
			
      }
   }
	
   return NULL;
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

void ASSISTANT::AnimatedObject::ActivateHyperlink(Gdiplus::PointF &ptObject) 
{
   for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj && obj->ContainsPoint(ptObject)) 
         obj->ActivateHyperlink(ptObject);
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


void ASSISTANT::AnimatedObject::IncreaseOrder(int increaseValue)
{
	for (int i = 0 ;i < animatedObjects.GetSize(); ++i) 
   {
      DrawObject *obj = animatedObjects[i];
      if (obj) 
      {
			obj->IncreaseOrder(increaseValue);
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
			if (obj->GetType() == ASSISTANT::GenericObject::GROUP)
			{
				((Group *)obj)->AppendObjectsToPage(page);
			}
         else if (obj->GetType() == ASSISTANT::GenericObject::ANIMATED) 
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
         if (!animatedObjects[i]->IsIdentic(animObject->animatedObjects[j]))
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


bool ASSISTANT::AnimatedObject::ContainsPoint(Gdiplus::PointF &ptMouse)
{
   if (!visible)
      return false; 

   for (int i = 0; i < animatedObjects.GetSize(); ++i)
   {
      if (animatedObjects[i]->ContainsPoint(ptMouse))
         return true;
   }

   return false;
}

void ASSISTANT::AnimatedObject::SetDocumentObject(bool bIsDocumentObject)
{
   m_bIsDocumentObject = bIsDocumentObject;

   for (int i = 0; i < animatedObjects.GetSize(); ++i)
   {
      animatedObjects[i]->SetDocumentObject(bIsDocumentObject);
   }
}
