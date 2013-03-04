/*********************************************************************

  program  : mlb_group.cpp
  authors  : Gabriela Maass
  date     : 11.03.2001
  revision : $Id: mlb_group.cpp,v 1.48 2009-10-13 07:45:33 maass Exp $
  desc     : Object to group objects
  
**********************************************************************/

#include "StdAfx.h"

#include "mlb_objects.h"
#include "la_project.h"
#include "mlb_misc.h"
#include "..\MainFrm.h"
#include "..\TextEdit.h"

ASSISTANT::Group::Group(uint32 _id, const _TCHAR *_hyperlink, const _TCHAR *_currentDirectory, const _TCHAR *_linkedObjects) 
: DrawObject(0, 0, 0, 0, -1, _id, _hyperlink, _currentDirectory, _linkedObjects) 
{
	
	textGroup = false;
	actItem = -1;
   m_pEditText = NULL;
   m_bResetEditText = false;
   m_clrText = RGB(0, 0, 0);
   
   ASSISTANT::FillLogFont(m_logFont, _T("Arial"), 10, _T("normal"), _T("roman"));
}

ASSISTANT::Group::~Group()
{
   if (m_pEditText != NULL)
      delete m_pEditText;
}

ASSISTANT::DrawObject* ASSISTANT::Group::GetObject(int _idx) 
{
	if ((_idx < 0) || (_idx >= groupObjects.GetSize())) return NULL;
	
	return groupObjects[_idx];
}


ASSISTANT::DrawObject* ASSISTANT::Group::GetObjectFromID(uint32 _id) 
{ 
	for (int i = 0; i < groupObjects.GetSize(); ++i)
	{
		if (groupObjects[i]->GetID() == _id) {
			return groupObjects[i];
		}
	}
	
	return NULL;
}

/*/
std::vector<uint32>* ASSISTANT::Group::GetObjectIDs() 
{
	std::vector<uint32>* ids = new std::vector<uint32>;
	
	for (int i = 0; i < groupObjects.GetSize(); ++i)
		ids->push_back(groupObjects[i]->GetID());
	
	return ids;
}

/*/

bool ASSISTANT::Group::AllEmpty() 
{
	if (!textGroup) return false;
	
	if (groupObjects.GetSize() == 0) return true;
	
	for (int i = 0; i < groupObjects.GetSize(); ++i)
		if (((Text*)groupObjects[i])->GetSize() > 0) return false; // found one being not empty
		
		return true;
}

/*
ASSISTANT::DrawObject* ASSISTANT::Group::Copy(Page* page, bool keepZPosition) 
{
   std::list<DrawObject*>::iterator 
      iter;
   Group 
      *ret;
   DrawObject
      *obj;
	
   ret = new Group(Project::active->GetObjectID(), NULL, NULL);
	
   for (iter = groupObjects.begin(); iter != groupObjects.end(); ++iter) 
   {
      if (groupObjects[i]->GetType() == ASSISTANT::GenericObject::GROUP) 
      {
         obj = ((Group*)*iter)->Copy(page, keepZPosition);
      }
      else 
      {
         obj = groupObjects[i]->Copy(keepZPosition);
			
         if (obj) 
         {
            if (obj->GetType() == GenericObject::IMAGE) 
            {
               ((WindowsImage *)obj)->BuildImage(page->GetCanvas(), ((WindowsImage *)groupObjects[i])->GetImage(), false);
            }
         }
      }
      if (obj) 
      {
         page->AppendObject(obj, false);
         ret->AddObject(obj);
      }
   }
	
	ret->UpdateDimension();
	//ret->SetX(GetX());
	//ret->SetY(GetY());
	
   return ret;
}
*/

ASSISTANT::DrawObject* ASSISTANT::Group::Copy(bool keepZPosition) 
{
   Group
      *ret;
   DrawObject* obj;

   ret = new Group(Project::active->GetObjectID(), NULL, NULL, NULL);

   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      obj = groupObjects[i]->Copy(keepZPosition); 
      if (obj)
      {
         ret->AddObject(obj);
      }
   }

   ret->UpdateDimension();
   /*
   ret->SetX(m_dX);
   ret->SetY(m_dY);
   ret->SetWidth(m_dWidth);
   ret->SetHeight(m_dHeight);
   */


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

void ASSISTANT::Group::Draw(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) return;
   
   for (int i = 0; i < groupObjects.GetSize(); ++i)
      groupObjects[i]->Draw(pDC, _zoomFactor, dOffX, dOffY);

}

void ASSISTANT::Group::Resize(float fDiffX, float fDiffY, int iPointIndex)
{
   SetUnrecorded();

   switch (iPointIndex)
   {
   case 0:
      m_dX += fDiffX;
      m_dY += fDiffY;
      m_dWidth -= fDiffX;
      m_dHeight -= fDiffY;
      break;
   case 1:
      m_dY += fDiffY;
      m_dHeight -= fDiffY;
      break;
   case 2:
      m_dY += fDiffY;
      m_dWidth += fDiffX;
      m_dHeight -= fDiffY;
      break;
   case 3:
      m_dWidth += fDiffX;
      break;
   case 4:
      m_dWidth += fDiffX;
      m_dHeight += fDiffY;
      break;
   case 5:
      m_dHeight += fDiffY;
      break;
   case 6:
      m_dX += fDiffX;
      m_dWidth -= fDiffX;
      m_dHeight += fDiffY;
      break;
   case 7:
      m_dX += fDiffX;
      m_dWidth -= fDiffX;
      break;
   }

   if (selected &&
       m_pEditText != NULL &&
       m_pEditText->m_hWnd != NULL &&
       m_pEditText->IsWindowVisible())
   {
      Page *pPage = ASSISTANT::Project::active->GetActivePage();
      if (pPage)
      {
         UpdateWindowRect();
      }
         
   }
}

void ASSISTANT::Group::Move(float _diffX, float _diffY)
{
   if (!visible) return;
   
   if (_diffX == 0 && _diffY == 0)
      return;

   recorded = false;
   
   m_dX += _diffX;
   m_dY += _diffY;
   
   if (selected &&
       m_pEditText != NULL &&
       m_pEditText->m_hWnd != NULL &&
       m_pEditText->IsWindowVisible())
   {
      UpdateWindowRect();
   }
   else
   {
      for (int i = 0; i < groupObjects.GetSize(); ++i)
         groupObjects[i]->Move(_diffX, _diffY);
   }
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

void ASSISTANT::Group::SaveObject(FILE *fp, int level, LPCTSTR path)
{
   if (groupObjects.GetSize() == 0) return;
	
   WCHAR *tab = (WCHAR *)malloc((level*2+1)*sizeof(WCHAR));
   *tab = L'\0';
   for (int i = 0; i < level; i++) 
      wcscat(tab, L"  ");
	
   fwprintf(fp, L"%s<GROUP>\n", tab);
	
   for (i = 0; i < groupObjects.GetSize(); ++i)
   {
      groupObjects[i]->SaveObject(fp,(level+1), path);
   }
	
   fwprintf(fp, L"%s</GROUP>\n", tab);
	
   if (tab)
      delete tab;
}

void ASSISTANT::Group::SetUnrecorded()
{
    recorded = false;

   for (int i = 0; i < groupObjects.GetSize(); ++i)
      groupObjects[i]->SetUnrecorded();
}

int ASSISTANT::Group::RecordableObjects()
{
	if (groupObjects.GetSize() == 0 || !textGroup) visible = false;
	
	if (!visible) return 0;
	
	return 1;
}

void ASSISTANT::Group::SaveAsAOF(CFileOutput *fp)
{
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
      groupObjects[i]->SaveAsAOF(fp);
}

void ASSISTANT::Group::AddObject(ASSISTANT::DrawObject *obj)
{
   if (obj == NULL)
      return;

	if (obj->GetType() == ASSISTANT::GenericObject::ASCIITEXT) 
   {
		if (groupObjects.GetSize() == 0 || textGroup) 
			textGroup = true;
		else 
			textGroup = false;
      
      ((Text *)obj)->GetLogFont(&m_logFont);
      Gdiplus::ARGB argbLineColor = ((Text *)obj)->GetLineColor();
      m_clrText = ASSISTANT::GdipARGBToColorref(argbLineColor);
	}
	
	obj->SetGroup(this);
	
   groupObjects.Add(obj);
   
}

ASSISTANT::GenericObject* ASSISTANT::Group::DeleteObject(GenericObject* obj) 
{ 
   if (groupObjects.GetSize() == 0) return NULL;
	
   if (textGroup) 
   {
      int toDel = 0;
		
      while ((toDel < groupObjects.GetSize()) && (groupObjects[toDel] != obj)) 
      {
         ++toDel;
      }
		
      if (toDel == groupObjects.GetSize()) 
         return NULL; // not found
		
      int 
         offX = groupObjects[toDel]->GetWidth();
      Text *txt;
		
      for (int i = toDel; i < groupObjects.GetSize(); ++i) 
      {
         txt = (Text*)groupObjects[i];
         if (txt->GetY() != groupObjects[toDel]->GetY()) 
            break;
         txt->SetX(txt->GetX() - offX);
      }
		
      if (actItem == toDel)
         if (actItem ==0)
            ++actItem;
         else
            --actItem;
			
			groupObjects.RemoveAt(toDel);
			UpdateDimension();
			
			return obj;
   } 
   else 
   {
      for (int i = 0; i < groupObjects.GetSize(); ++i) 
      {
         if (groupObjects[i] == obj) 
         {
				groupObjects.RemoveAt(i);
				UpdateDimension();
				return obj;
         }
      }
      return NULL;
   }
}


ASSISTANT::DrawObject* ASSISTANT::Group::DeleteObject(uint32 id) 
{ 
   if (groupObjects.GetSize() == 0) return NULL;
	
   if (textGroup) 
   {
      DrawObject* obj = NULL;
		
      for (int i = 0; i < groupObjects.GetSize(); ++i) 
      {
         if (groupObjects[i]->GetID() == id) 
         {
            obj = groupObjects[i];
            if (obj->GetType() == GenericObject::ASCIITEXT) 
            {
					int offX = obj->GetWidth();
					Text* txt;
					
					for (int j = i+1; j < groupObjects.GetSize(); ++j) 
               {
						txt = (Text*)groupObjects[j];
						if (txt->GetY() != obj->GetY()) break;
						txt->SetX(txt->GetX() - offX);
					}
            }
				
            if (actItem == i) 
               if (actItem == 0)
                  ++actItem;
               else
                  --actItem;
					
					groupObjects.RemoveAt(i);
					UpdateDimension();
					break;
         }
      }
		
      return obj;
   } 
   else 
   {
      for (int i = 0; i < groupObjects.GetSize(); ++i) 
      {
         if (groupObjects[i]->GetID() == id) 
         {
            DrawObject *pObj = groupObjects[i];
				groupObjects.RemoveAt(i);
				UpdateDimension();
				return pObj;
         }
      }
      return NULL;
   }
}


void ASSISTANT::Group::DeleteAll() 
{
   if (groupObjects.GetSize() == 0) return;
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      /* Objekte einer Gruppe sind nicht , sondern nur auf der Seite */
      groupObjects[i]->Hide();
   }
   groupObjects.RemoveAll();
}


void ASSISTANT::Group::DrawSelection(CDC *pDC, float _zoomFactor, double dOffX, double dOffY)
{
   if (!visible) return;
   
   if (m_pEditText == NULL)
   {
      CAssistantView *pWhiteboardView = NULL;

      CWnd *pMainFrame = CMainFrameA::GetCurrentInstance(); //AfxGetMainWnd();
      if (pMainFrame && pMainFrame->GetRuntimeClass() == RUNTIME_CLASS(CMainFrameA))
         pWhiteboardView = ((CMainFrameA *)pMainFrame)->GetWhiteboardView();
      
      if (pWhiteboardView)
      {
         CRect rcEdit(0, 0, 0, 0);
         m_pEditText = new CTextEdit();
         m_pEditText->Create(WS_CHILD, rcEdit, pWhiteboardView, ID_EDIT_TEXT);
         m_pEditText->SetGroup(this);
      }
   }
   
   if (m_pEditText != NULL && m_pEditText->m_hWnd != NULL)
   {  
      CRect rcParent;
      CWnd *pParent = m_pEditText->GetParent();
      if (pParent)
      {
         pParent->GetWindowRect(&rcParent);
         CRect rcParentClient;
         pParent->GetClientRect(&rcParentClient);
         // subtract border
         if (rcParentClient.Width() != rcParent.Width())
         {
            rcParent.top += (rcParent.Height() - rcParentClient.Height()) / 2;
            rcParent.left += (rcParent.Width() - rcParentClient.Width()) / 2;
         }
      }

      CRect rcEdit;
      m_pEditText->GetWindowRect(&rcEdit);

      rcEdit.left = rcEdit.left - rcParent.left; 
      rcEdit.right = rcEdit.right - rcParent.left; 
      rcEdit.top = rcEdit.top - rcParent.top; 
      rcEdit.bottom = rcEdit.bottom - rcParent.top; 

      int x = m_dX * _zoomFactor + dOffX + 0.5;
      int y = m_dY * _zoomFactor + dOffY + 0.5;
      int width = m_dWidth * _zoomFactor + 0.5;
      int height = m_dHeight * _zoomFactor + 0.5;

      if (rcEdit.Width() != width || rcEdit.Height() != height ||
          rcEdit.left != x || rcEdit.top != y)
         m_bResetEditText = true;
      
      
      if (m_bResetEditText || !m_pEditText->IsWindowVisible())
      {
         long iFirst = 0;
         long iLast = 0;
         if (m_pEditText->IsWindowVisible())
            m_pEditText->GetSel(iFirst, iLast);
         
         m_pEditText->Clear();

         LOGFONT logFont;
         memcpy(&logFont, &m_logFont, sizeof(LOGFONT));
         logFont.lfHeight = -1 * ((double)abs(logFont.lfHeight) * _zoomFactor);
         CFont font;
         font.CreateFontIndirect(&logFont);
         m_pEditText->SetFont(&font);
         

         CHARFORMAT cf;
         memset(&cf, 0, sizeof(CHARFORMAT));
         cf.cbSize = sizeof(CHARFORMAT);
         cf.dwMask = CFM_COLOR;
         cf.dwEffects = 0;
         
         m_pEditText->GetDefaultCharFormat(cf);
         
         cf.crTextColor = m_clrText;
         
         if (cf.dwEffects & CFE_AUTOCOLOR )
            cf.dwEffects ^= CFE_AUTOCOLOR;
         
         m_pEditText->SetDefaultCharFormat(cf);

         if (width < 0)
         {
            x = x + width;
            width = -1 * width;
         }
         if (height < 0)
         {
            y = y + height;
            height = -1 * height;
         }

         CString csWindowText;
         CreateTextString(csWindowText);
         m_pEditText->SetWindowText(csWindowText);
         if (!m_pEditText->IsWindowVisible())
         {
            m_pEditText->ShowWindow(SW_SHOW);
            m_pEditText->SetFocus();
         }

         m_pEditText->MoveWindow(x, y, width, height, true);
         
         CRect rcText(0, 0, width, height);
         m_pEditText->SetRect(&rcText);
      
         m_pEditText->SetSel(iFirst, iFirst);
      }
      
      m_bResetEditText = false;
   }
   
   Gdiplus::Graphics graphics(pDC->m_hDC);
   graphics.TranslateTransform(dOffX, dOffY);
   graphics.ScaleTransform(_zoomFactor, _zoomFactor);

   float fSelectionWidth = 6 / _zoomFactor;

   double dX = m_dX;
   double dY = m_dY;
   double dWidth = m_dWidth;
   double dHeight = m_dHeight;
   if (dWidth < 0)
   {
      dX = dX + dWidth;
      dWidth = -1 * dWidth;
   }
   if (dHeight < 0)
   {
      dY = dY + dHeight;
      dHeight = -1 * dHeight;
   }

   Gdiplus::RectF rcSelection(dX, dY, dWidth, dHeight);

   rcSelection.Inflate(fSelectionWidth, fSelectionWidth);
   
   Gdiplus::Color clrWhite(255, 255, 255, 255);
   Gdiplus::Color clrBlack(255, 0, 0, 0);
   Gdiplus::HatchBrush brush(Gdiplus::HatchStyleLightUpwardDiagonal, clrBlack, clrWhite);
   Gdiplus::Pen selectionRectPen(&brush, fSelectionWidth);

   graphics.DrawRectangle(&selectionRectPen, rcSelection);
   
   Gdiplus::REAL fLineWidth = 6.0 / _zoomFactor;
   Gdiplus::REAL fRadius = 3.0 / _zoomFactor;

   Gdiplus::REAL fPenWidth = 0.5f;
   Gdiplus::Pen blackPen(clrBlack, fPenWidth);
   Gdiplus::SolidBrush whiteBrush(clrWhite);
   
   selectPoints[0].X = dX-fSelectionWidth;
   selectPoints[0].Y = dY-fSelectionWidth;
   selectPoints[1].X = dX+(dWidth/2);
   selectPoints[1].Y = dY-fSelectionWidth;
   selectPoints[2].X = dX+dWidth+fSelectionWidth;
   selectPoints[2].Y = dY-fSelectionWidth;
   selectPoints[3].X = dX+dWidth+fSelectionWidth;
   selectPoints[3].Y = dY+(dHeight/2);
   selectPoints[4].X = dX+dWidth+fSelectionWidth;
   selectPoints[4].Y = dY+dHeight+fSelectionWidth;
   selectPoints[5].X = dX+(dWidth/2);
   selectPoints[5].Y = dY+dHeight+fSelectionWidth;
   selectPoints[6].X = dX-fSelectionWidth;
   selectPoints[6].Y = dY+dHeight+fSelectionWidth;
   selectPoints[7].X = dX-fSelectionWidth;
   selectPoints[7].Y = dY+(dHeight/2);
   for (int i = 0; i < 8; i++) 
   {
      graphics.FillEllipse(&whiteBrush, 
         (selectPoints[i].X - fRadius), (selectPoints[i].Y - fRadius), 
         2*fRadius, 2*fRadius);
      graphics.DrawEllipse(&blackPen, 
         (selectPoints[i].X - fRadius), (selectPoints[i].Y - fRadius), 
         2*fRadius, 2*fRadius);
   }
}

void ASSISTANT::Group::UpdateWindowRect()
{
   if (m_pEditText == NULL || m_pEditText->m_hWnd == NULL ||
       !m_pEditText->IsWindowVisible())
      return;

   if (!ASSISTANT::Project::active || !ASSISTANT::Project::active->GetActivePage())
      return;

   Page *pPage = ASSISTANT::Project::active->GetActivePage();

   float fZoomFactor = pPage->GetZoomFactor();

   CRect rcParent;
   if (m_pEditText->GetParent())
      m_pEditText->GetParent()->GetClientRect(&rcParent);
   UINT nOffsetX, nOffsetY;
   pPage->CalculateOffset(rcParent, fZoomFactor, nOffsetX, nOffsetY);
   int x = m_dX * fZoomFactor + nOffsetX + 0.5;
   int y = m_dY * fZoomFactor + nOffsetY + 0.5;
   int width = m_dWidth * fZoomFactor + 0.5;
   int height = m_dHeight * fZoomFactor + 0.5;
   
   // Normalize
   if (width < 0)
   {
      x = x + width;
      width = -1 * width;
   }
   if (height < 0)
   {
      y = y + height;
      height = -1 * height;
   }

   m_pEditText->MoveWindow(x, y,width, height, true);

   CRect rcText(0, 0, width, height);
   m_pEditText->SetRect(&rcText);

   CWnd *pParent = m_pEditText->GetParent();
   if (pParent)
      pParent->RedrawWindow();
}

bool ASSISTANT::Group::IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditType, int &iPointIndex, double dZoomFactor)
{
   if (!visible || !selected) 
      return false;
   
   double dTolerance = 3 / dZoomFactor;
   
   nEditType = 0;
   iPointIndex = -1;
   for (int i = 0; i < 8; i++) 
   {
      if ((ptObject.X > selectPoints[i].X-dTolerance) && (ptObject.X < selectPoints[i].X+dTolerance)) 
      {
         if ((ptObject.Y > selectPoints[i].Y-dTolerance) && (ptObject.Y < selectPoints[i].Y+dTolerance)) 
         {
            if (i == 0 || i == 4)
               nEditType = SIZE_NWSE;
            if (i == 1 || i == 5)
               nEditType = SIZE_NS;
            if (i == 2 || i == 6)
               nEditType = SIZE_NESW;
            if (i == 3 || i == 7)
               nEditType = SIZE_WE;
            iPointIndex = i;
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

void ASSISTANT::Group::SetSelected(bool bIsSelected)
{
   
   if (group) 
   {
      selected = false;
      group->SetSelected(bIsSelected);
   }
   else 
   {
      selected = bIsSelected;
   }

   if (!selected)
   {
      if (m_pEditText != NULL &&
          m_pEditText->m_hWnd != NULL &&
          m_pEditText->IsWindowVisible())
      {
         CString csWindowText;
         m_pEditText->GetWindowText(csWindowText);
         
         if (csWindowText.IsEmpty())
         {
            if (ASSISTANT::Project::active)
            {
               Page *pPage = ASSISTANT::Project::active->GetActivePage();
               if (pPage)
               {
                  m_pEditText->ShowWindow(SW_HIDE);
                  pPage->DeleteObjectFromContainer(this);
               }
            }
         }
         else
            ChangeObjects(csWindowText, false);

         m_pEditText->ShowWindow(SW_HIDE);
      }
   }
}

void ASSISTANT::Group::Hide()
{
   if (!visible) 
      return;
	
   visible = false;
   selected = false;
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      groupObjects[i]->Hide();
   }
}

void ASSISTANT::Group::SetInvisible()
{
   visible = false;
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      groupObjects[i]->SetInvisible();
   }
}


void ASSISTANT::Group::Zoom(float _zoomFactor)
{
}

void ASSISTANT::Group::GetBBox(int *minX, int *minY, int *maxX, int *maxY)
{  
	if ((m_dX+m_dWidth) < m_dX) 
   {
		*minX = m_dX+m_dWidth-3;
		*maxX = m_dX+3;
	}
	else 
   {
		*minX = m_dX-3;
		*maxX = m_dX+m_dWidth+3;
	}
	if ((m_dY+m_dHeight) < m_dY) 
   {
		*minY = m_dY+m_dHeight-3;
		*maxY = m_dY+3;
	}
	else 
   {
		*minY = m_dY-3;
		*maxY = m_dY+m_dHeight+3;
	}
	
}

void ASSISTANT::Group::GetBoundingBox(CRect &rcObject)
{
   rcObject.left = m_dX;
   rcObject.right = m_dX + m_dWidth;
   rcObject.top = m_dY;
   rcObject.bottom = m_dY + m_dHeight;
   
   rcObject.NormalizeRect();
}

bool ASSISTANT::Group::IsInRegion(int _x1, int _y1, int _x2, int _y2)
{
	int 
		x1, y1,
		x2, y2;
	
	if (!visible) return false;
	
	GetBBox(&x1,&y1,&x2,&y2);
	if ((x1 < _x1) || (y1 < _y1) || (x2 > (_x2)) || (y2 > (_y2))) return false;
	
	return true;
}

bool ASSISTANT::Group::Overlap(int _x, int _y, float _zoomFactor) 
{
   int 
      x1, y1,
      x2, y2;
   bool
      overlap;
	
   if (!visible) return false;
	
   GetBBox(&x1,&y1,&x2,&y2);
   if ((_x < x1*_zoomFactor) || (_y < y1*_zoomFactor) || (_x > x2*_zoomFactor) || (_y > y2*_zoomFactor)) 
      return false;
	
   overlap = false;
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      if (groupObjects[i]->Overlap( _x, _y, _zoomFactor)) 
      {
         overlap = true;
         actItem = i;
         break;
      }
   }
	
   return overlap;
}

void ASSISTANT::Group::UpdateDimension()
{
	if (groupObjects.GetSize() == 0) return;
	
   if (textGroup &&
       ASSISTANT::Project::active != NULL &&
       ASSISTANT::Project::active->GetActivePage() &&
       m_pEditText != NULL &&
       m_pEditText->m_hWnd != NULL &&
       m_pEditText->IsWindowVisible())
   {
      
      CString csWindowText;
      m_pEditText->GetWindowText(csWindowText);
      
      m_bResetEditText = true;
      ChangeObjects(csWindowText, false);
   }
   else
   {
      if (textGroup)
      {
         Gdiplus::RectF rcTextBBox;
         ((Text *)groupObjects[0])->GetBoundingBox(rcTextBBox);
         m_dX = rcTextBBox.X;
         m_dY = rcTextBBox.Y;
         m_dWidth = rcTextBBox.Width;
         m_dHeight = rcTextBBox.Height;

         CString csText = ((Text *)groupObjects[0])->GetString();
         double dWidth = DrawSdk::Text::GetTextWidth(csText, csText.GetLength(), &m_logFont);
         if ((rcTextBBox.X+dWidth-m_dX) > m_dWidth) 
            m_dWidth = rcTextBBox.X+dWidth - m_dX;

         for (int i = 0; i < groupObjects.GetSize(); ++i) 
         {
            if (groupObjects[i]->IsVisible())
            {
               ((Text *)groupObjects[i])->GetBoundingBox(rcTextBBox);
               if (rcTextBBox.X < m_dX) m_dX = rcTextBBox.X;
               if (rcTextBBox.Y < m_dY) m_dY = rcTextBBox.Y;
               if ((rcTextBBox.GetRight()-m_dX) > m_dWidth) m_dWidth = rcTextBBox.GetRight() - m_dX;
               if ((rcTextBBox.GetBottom()-m_dY) > m_dHeight) m_dHeight = rcTextBBox.GetBottom() - m_dY;
               
               
               CString csText = ((Text *)groupObjects[i])->GetString();
               double dWidth = DrawSdk::Text::GetTextWidth(csText, csText.GetLength(), &m_logFont);
               if ((rcTextBBox.X+dWidth-m_dX) > m_dWidth) 
                  m_dWidth = rcTextBBox.X+dWidth - m_dX;
            }
         }
         m_dWidth += 2;
      }
      else
      {
         CRect rcTextBBox;
         groupObjects[0]->GetBoundingBox(rcTextBBox);
         m_dX = rcTextBBox.left;
         m_dY = rcTextBBox.top;
         m_dWidth = rcTextBBox.Width();
         m_dHeight = rcTextBBox.Height();
         for (int i = 0; i < groupObjects.GetSize(); ++i) 
         {
            if (groupObjects[i]->IsVisible())
            {
               groupObjects[i]->GetBoundingBox(rcTextBBox);
               if (rcTextBBox.left < m_dX) m_dX = rcTextBBox.left;
               if (rcTextBBox.top < m_dY) m_dY = rcTextBBox.top;
               if ((rcTextBBox.right-m_dX) > m_dWidth) m_dWidth = rcTextBBox.right - m_dX;
               if ((rcTextBBox.bottom-m_dY) > m_dHeight) m_dHeight = rcTextBBox.bottom - m_dY;
            }
         }
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

bool ASSISTANT::Group::IsEmptyText()
{
   bool bIsEmptyText = false;

   if (groupObjects.GetSize() == 0)
      bIsEmptyText = true;
   else if (IsTextGroup())
   {
      bIsEmptyText = true;
      for (int i = 0; i < groupObjects.GetSize() && bIsEmptyText; ++i)
      {
         Text *pText = (Text *)groupObjects[i];
         if (pText && !pText->Empty())
            bIsEmptyText = false;
      }
   }

   return bIsEmptyText;
}

void ASSISTANT::Group::ChangeFontFamily(LPCTSTR szFontFamily) 
{
   if (!visible || szFontFamily == NULL)
      return;
   
   _tcscpy(m_logFont.lfFaceName, szFontFamily);
   m_bResetEditText = true;

   if (groupObjects.GetSize() == 0 || !textGroup) 
      return;

   recorded = false;
   
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      if (groupObjects[i]->GetType() == GenericObject::ASCIITEXT)
      {
         ((Text*)groupObjects[i])->ChangeFontFamily(szFontFamily);
      }
   }
   
   if (textGroup &&
      ASSISTANT::Project::active != NULL &&
      ASSISTANT::Project::active->GetActivePage() &&
      m_pEditText != NULL &&
      m_pEditText->m_hWnd != NULL &&
      m_pEditText->IsWindowVisible())
   {
      CFont font;
      font.CreateFontIndirect(&m_logFont);
      m_pEditText->SetFont(&font);
   }

   UpdateDimension();
}


void ASSISTANT::Group::ChangeFontSize(int iFontSize) 
{
   if (!visible)
      return;

   m_logFont.lfHeight = -1 * iFontSize;
   m_bResetEditText = true;

   if (groupObjects.GetSize() == 0 || !textGroup) 
      return;

   recorded = false;
   

   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      if (groupObjects[i]->GetType() == GenericObject::ASCIITEXT)
      {
         ((Text*)groupObjects[i])->ChangeFontSize(iFontSize);
      }
   }
   
   if (textGroup &&
      ASSISTANT::Project::active != NULL &&
      ASSISTANT::Project::active->GetActivePage() &&
      m_pEditText != NULL &&
      m_pEditText->m_hWnd != NULL &&
      m_pEditText->IsWindowVisible())
   {
      CFont font;
      font.CreateFontIndirect(&m_logFont);
      m_pEditText->SetFont(&font);
   }

   UpdateDimension();
}


void ASSISTANT::Group::ChangeFontWeight(LPCTSTR weight) 
{
   if (!visible || weight == NULL) 
      return;

   m_logFont.lfWeight = (_tcscmp(weight, _T("bold")) == 0) ? FW_BOLD : FW_NORMAL;
   m_bResetEditText = true;

   if (groupObjects.GetSize() == 0 || !textGroup) 
      return;

   recorded = false;
   
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      if (groupObjects[i]->GetType() == GenericObject::ASCIITEXT)
      {
			((Text*)groupObjects[i])->ChangeFont(((Text*)groupObjects[i])->GetFamily(),
            ((Text*)groupObjects[i])->GetSize(), weight, ((Text*)groupObjects[i])->GetSlant());
      }
   }

   if (textGroup &&
      ASSISTANT::Project::active != NULL &&
      ASSISTANT::Project::active->GetActivePage() &&
      m_pEditText != NULL &&
      m_pEditText->m_hWnd != NULL &&
      m_pEditText->IsWindowVisible())
   {
      CFont font;
      font.CreateFontIndirect(&m_logFont);
      m_pEditText->SetFont(&font);
   }

   UpdateDimension();
}


void ASSISTANT::Group::ChangeFontSlant(LPCTSTR slant) 
{
   if (!visible || slant == NULL) 
      return;

   m_logFont.lfItalic = (_tcscmp(slant, _T("italic")) == 0) ? true : false;
   m_bResetEditText = true;

   if (groupObjects.GetSize() == 0 || !textGroup) 
      return;

   recorded = false;
   
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      if (groupObjects[i]->GetType() == GenericObject::ASCIITEXT)
      {
			((Text*)groupObjects[i])->ChangeFont(((Text*)groupObjects[i])->GetFamily(),
            ((Text*)groupObjects[i])->GetSize(), ((Text*)groupObjects[i])->GetWeight(), slant);
      }
	}

   if (textGroup &&
      ASSISTANT::Project::active != NULL &&
      ASSISTANT::Project::active->GetActivePage() &&
      m_pEditText != NULL &&
      m_pEditText->m_hWnd != NULL &&
      m_pEditText->IsWindowVisible())
   {
      CFont font;
      font.CreateFontIndirect(&m_logFont);
      m_pEditText->SetFont(&font);
   }

   UpdateDimension();
}



void ASSISTANT::Group::ChangeLineColor(Gdiplus::ARGB argbLineColor) 
{
   if (!visible || groupObjects.GetSize() == 0) return;

   recorded = false;

   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      if (groupObjects[i]->GetType() != GenericObject::IMAGE) 
         ((ColoredObject *)groupObjects[i])->ChangeLineColor(argbLineColor);
   }

   m_clrText = ASSISTANT::GdipARGBToColorref(argbLineColor);

   if (selected && 
      m_pEditText != NULL &&
      m_pEditText->m_hWnd != NULL &&
      m_pEditText->IsWindowEnabled())
   {
      m_bResetEditText = true;
   }
}

void ASSISTANT::Group::ChangeFillColor(Gdiplus::ARGB argbFillColor) 
{
	if (!visible || groupObjects.GetSize() == 0 || textGroup) return;
	
	recorded = false;
   
	for (int i = 0; i < groupObjects.GetSize(); ++i) {
		if ((groupObjects[i]->GetType() != GenericObject::IMAGE) && 
          (groupObjects[i]->GetType() != GenericObject::ASCIITEXT)) 
			((ColoredObject *)groupObjects[i])->ChangeFillColor(argbFillColor);
	}
}

void ASSISTANT::Group::ChangeLineStyle(Gdiplus::DashStyle gdipLineStyle) 
{
	if (!visible || groupObjects.GetSize() == 0 || textGroup) return;
	
	recorded = false;
   
	for (int i = 0; i < groupObjects.GetSize(); ++i) {
		if ((groupObjects[i]->GetType() != GenericObject::IMAGE) && (groupObjects[i]->GetType() != GenericObject::ASCIITEXT)) 
			((ColoredObject *)groupObjects[i])->ChangeLineStyle(gdipLineStyle);
	}
}


void ASSISTANT::Group::ChangeLineWidth(int _lineWidth) 
{
	if (!visible || groupObjects.GetSize() == 0 || textGroup) return;
	
	recorded = false;
   
	for (int i = 0; i < groupObjects.GetSize(); ++i) {
		if ((groupObjects[i]->GetType() != GenericObject::IMAGE) && (groupObjects[i]->GetType() != GenericObject::ASCIITEXT)) 
			((ColoredObject *)groupObjects[i])->ChangeLineWidth(_lineWidth);
	}
}


void ASSISTANT::Group::CreateTextString(CString &csText)
{
   if (!visible || groupObjects.GetSize() == 0 || !textGroup) return;
   
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      if (groupObjects[i]->GetType() == GenericObject::ASCIITEXT) 
      {
         csText += ((Text *)groupObjects[i])->GetString();
         if (((Text *)groupObjects[i])->HasReturnAtEnd())
         {
            csText += (TCHAR)13;
            csText += (TCHAR)10;
         }
      }
   }
}

void ASSISTANT::Group::ChangeObjects(CString &csWindowText, bool bSave)
{
	textGroup = true;

   if (ASSISTANT::Project::active == NULL)
      return;

   Page *pPage = ASSISTANT::Project::active->GetActivePage();

   if (pPage == NULL) 
      return;

   // Normalize
   if (m_dWidth < 0)
   {
      m_dX = m_dX + m_dWidth;
      m_dWidth = -1 * m_dWidth;
   }
   if (m_dHeight < 0)
   {
      m_dY = m_dY + m_dHeight;
      m_dHeight = -1 * m_dHeight;
   }

   if (m_pEditText != NULL &&
       m_pEditText->m_hWnd != NULL &&
       m_pEditText->IsWindowVisible())
   {

      bool bIsAlreadyInserted = true;
      if (groupObjects.GetSize() == 0)
         bIsAlreadyInserted = false;
      
      if (bSave)
         pPage->Change(this, bIsAlreadyInserted);

      float fZoomFactor = pPage->GetZoomFactor();

      int nFontSize = abs(m_logFont.lfHeight);
      CString csSize;
      csSize.Format(_T("%d"), nFontSize);
      CString csFontFamily = m_logFont.lfFaceName;
      CString csWeight = m_logFont.lfWeight == FW_BOLD ? _T("bold") : _T("normal");
      CString csSlant = m_logFont.lfItalic ? _T("italic") : _T("roman");
      
      Gdiplus::ARGB argbTextColor = ASSISTANT::ColorrefToGdipARGB(m_clrText);

      CString csLine;
      int iGroupObjects = 0;
      CPoint ptChar = m_pEditText->GetCharPos(0);
      double dLastTextY = m_dY + ((double)ptChar.y / fZoomFactor);

      int iReturnCount = 0;
      Text *pText = NULL;
      for (int i = 0; i < csWindowText.GetLength(); ++i)
      {
         CPoint ptChar = m_pEditText->GetCharPos(i-iReturnCount);
         double dActTextY = m_dY + ((double)ptChar.y / fZoomFactor);

         if (csWindowText[i] == 13 || abs(dLastTextY - dActTextY) > 1)
         {
            pText = NULL;
            if (csLine.GetLength() > 0)
            {
               UINT nFirstCharPos = (i - iReturnCount) - csLine.GetLength();
               CPoint ptChar = m_pEditText->GetCharPos(nFirstCharPos);
               double dTextX = m_dX + ((double)ptChar.x / fZoomFactor);
               double dTextY = m_dY + ((double)ptChar.y / fZoomFactor);
               
               if (iGroupObjects < groupObjects.GetSize())
               {
                  pText = (Text *)groupObjects[iGroupObjects];
                  pText->ChangeString(csLine);
                  pText->ChangeFont(csFontFamily, csSize, csWeight, csSlant);
                  pText->SetX(dTextX);
                  pText->SetY(dTextY);
               }
               else
               {
                  pText = new ASSISTANT::Text(dTextX, dTextY, 0.0, nFontSize, -1, 
                                              argbTextColor, csLine, 
                                              csFontFamily, csSize, csWeight, csSlant, 0,
                                              ASSISTANT::Project::active->GetObjectID(), NULL, NULL, NULL, NULL);
                  AddObject(pText);
                  pPage->AppendObject(pText, false, false);
               }
               
               if (pText)
               {
                  float fAscent= pText->GetAscent();
                  pText->Move(0, fAscent);
               }
               ++iGroupObjects;
            }
            csLine.Empty();

            if (csWindowText[i] == 13)
            {
               if (pText)
                  pText->SetReturnAtEnd(true);
               ++i;         
               ++iReturnCount;
            }
            else
            {
               if (pText)
                  pText->SetReturnAtEnd(false);
               csLine += csWindowText[i];  // automatic line break
            }
            dLastTextY = dActTextY;
         }
         else
         {
            csLine += csWindowText[i];
         }
      }
      
      if (!csLine.IsEmpty())
      {
         UINT nFirstCharPos = (csWindowText.GetLength() - iReturnCount) - csLine.GetLength();
         CPoint ptChar = m_pEditText->GetCharPos(nFirstCharPos);
         double dTextX = m_dX + (ptChar.x / fZoomFactor);
         double dTextY = m_dY + (ptChar.y / fZoomFactor);
         
         Text *pText = NULL;
         if (iGroupObjects < groupObjects.GetSize())
         {
            pText = (Text *)groupObjects[iGroupObjects];
            pText->ChangeString(csLine);
            pText->ChangeFont(csFontFamily, csSize, csWeight, csSlant);
            pText->SetX(dTextX);
            pText->SetY(dTextY);

            UINT nLastCharPos = i;
            CPoint ptLastChar = m_pEditText->GetCharPos(i);
            double dWidth = (double)(ptLastChar.x - ptChar.x) / fZoomFactor;
            pText->SetWidth(dWidth);
         }
         else
         {
            pText = new ASSISTANT::Text(dTextX, dTextY, 0.0, nFontSize, -1, 
                                        argbTextColor, csLine, csFontFamily, csSize, 
                                        csWeight, csSlant, 0, ASSISTANT::Project::active->GetObjectID(), 
                                        NULL, NULL, NULL, NULL);
            AddObject(pText);
            pPage->AppendObject(pText, false, false);
         }
         
         if (pText)
         {
            float fAscent= pText->GetAscent();
            pText->Move(0, fAscent);
         }

         ++iGroupObjects;
      }
      
      if (iGroupObjects < groupObjects.GetSize())
      {
         for (int i = groupObjects.GetSize()-1; i >= iGroupObjects; --i)
         {
            groupObjects[i]->Hide();
            groupObjects.RemoveAt(i);
         }
      }
      
      UINT nLastCharPos = csWindowText.GetLength() - 1;
      CPoint ptLastChar = m_pEditText->GetCharPos(nLastCharPos);
      UINT nYOffset = 0;
      if (csWindowText.GetLength() > 2 &&
          csWindowText[csWindowText.GetLength() - 1] == 10 &&
          csWindowText[csWindowText.GetLength() - 2] == 13) // last char is return
      {
         nYOffset = nFontSize;
      }

      if (m_dHeight != ((ptLastChar.y / fZoomFactor) + 1.5*nFontSize) + nYOffset)
      {
         m_dHeight = (ptLastChar.y / fZoomFactor) + 1.5*nFontSize + nYOffset;
         UpdateWindowRect();
      }

      if (bSave)
         pPage->SaveAsAOF();
      
   }

}

void ASSISTANT::Group::GetTextFontFamily(CString &csFontFamily)
{
   csFontFamily.Empty();

   if (!IsTextGroup())
      return;

   if (groupObjects.GetSize() == 0)
      return;

   csFontFamily = ((Text *)groupObjects[0])->GetFamily();
   for (int i = 1; i < groupObjects.GetSize(); ++i)
   {
      if (csFontFamily != ((Text *)groupObjects[i])->GetFamily())
      {
         csFontFamily.Empty();
         return;
      }
   }
}

int ASSISTANT::Group::GetTextFontSize()
{
   if (!IsTextGroup())
      return 0;

   if (groupObjects.GetSize() == 0)
      return 0;

   int iFontSize = ((Text *)groupObjects[0])->GetFontSize();
   for (int i = 1; i < groupObjects.GetSize(); ++i)
   {
      if (iFontSize != ((Text *)groupObjects[i])->GetFontSize())
      {
         return 0;
      }
   }

   return iFontSize;
}

void ASSISTANT::Group::GetTextFontSlant(CString &csFontSlant)
{
   csFontSlant.Empty();

   if (!IsTextGroup())
      return;

   if (groupObjects.GetSize() == 0)
      return;

   csFontSlant = ((Text *)groupObjects[0])->GetSlant();
   for (int i = 1; i < groupObjects.GetSize(); ++i)
   {
      if (csFontSlant != ((Text *)groupObjects[i])->GetSlant())
      {
         csFontSlant.Empty();
         return;
      }
   }
}

void ASSISTANT::Group::GetTextFontWeight(CString &csFontWeight)
{
   csFontWeight.Empty();

   if (!IsTextGroup())
      return;

   if (groupObjects.GetSize() == 0)
      return;

   csFontWeight = ((Text *)groupObjects[0])->GetWeight();
   for (int i = 1; i < groupObjects.GetSize(); ++i)
   {
      if (csFontWeight != ((Text *)groupObjects[i])->GetWeight())
      {
         csFontWeight.Empty();
         return;
      }
   }
}

void ASSISTANT::Group::LoadObjects(ASSISTANT::Page *page, LPCTSTR szPageNumber, 
                                   SGMLElement *_root, LPCTSTR path, bool append)
{
   SGMLElement 
      *hilf; 
   DrawObject
      *obj;
   bool
      bIsMasterGroup = false;
	
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
			obj = WindowsImage::Load(hilf,page->GetImagePath());
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
         if (obj)
            ((Group *)obj)->LoadObjects(page,szPageNumber,hilf,path);
      }
      if (obj) 
      {
         if (bIsMasterGroup)
         {
            obj->SetMasterObject(true);
            obj->SetDocumentObject(true);
            ((ASSISTANT::Page *)page)->AppendObject(obj);
         }
         else
         {
            ((ASSISTANT::Page *)page)->AppendObject(obj, false);
            AddObject(obj);
            ((ASSISTANT::Page *)page)->InActivateObject(obj);
         }
      }
   }
}


void ASSISTANT::Group::AdjustGui() 
{
	Text* txt;
	
	if (textGroup && groupObjects.GetSize() > 0) {
		txt = (Text*)groupObjects[0];
		txt->AdjustGui();
	} 
}


ASSISTANT::DrawObject *ASSISTANT::Group::Load(SGMLElement *hilf) 
{
	Group 
		*obj;
	
	obj = new ASSISTANT::Group(Project::active->GetObjectID(), NULL, NULL, NULL);
	
	return obj;
}


void ASSISTANT::Group::TakeParameter(DrawObject *_obj) 
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

   m_dX = _obj->GetX();
   m_dY = _obj->GetY();
   m_dWidth = _obj->GetWidth();
   m_dHeight = _obj->GetHeight();

   zoomFactor = Project::active->GetActivePage()->GetZoomFactor();
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      obj = ((Group *)_obj)->GetObjectFromID(groupObjects[i]->GetID());
      if (obj) 
      { 
         if (groupObjects[i]->GetType() == GenericObject::GROUP) 
         {
            ((Group *)groupObjects[i])->TakeParameter(obj);
         }
         else 
         {
            if (obj->GetX() != groupObjects[i]->GetX() || obj->GetY() != groupObjects[i]->GetY()) 
            {
               diffX = (obj->GetX() - groupObjects[i]->GetX());
               diffY = (obj->GetY() - groupObjects[i]->GetY());
               groupObjects[i]->Move(diffX,diffY);
            }
            if (groupObjects[i]->GetWidth() != obj->GetWidth()) 
            {
               groupObjects[i]->SetWidth(obj->GetWidth());
            }
            if (groupObjects[i]->GetHeight() != obj->GetHeight()) 
            {
               groupObjects[i]->SetHeight(obj->GetHeight());
            }
            if (groupObjects[i]->GetType() != GenericObject::IMAGE) 
            {
               if (((ColoredObject *)groupObjects[i])->GetLineColor() != ((ColoredObject *)obj)->GetLineColor()) 
               {
                  if (IsTextGroup())
                     ChangeLineColor(((ColoredObject *)obj)->GetLineColor());
                  else
                     ((ColoredObject *)groupObjects[i])->ChangeLineColor(((ColoredObject *)obj)->GetLineColor());
               }
               if (((ColoredObject *)groupObjects[i])->GetFillColor() != ((ColoredObject *)obj)->GetFillColor()) 
               {
                  ((ColoredObject *)groupObjects[i])->ChangeFillColor(((ColoredObject *)obj)->GetFillColor());
               }
               if (((ColoredObject *)groupObjects[i])->GetLineWidth() != ((ColoredObject *)obj)->GetLineWidth()) 
               {
                  ((ColoredObject *)groupObjects[i])->ChangeLineWidth(((ColoredObject *)obj)->GetLineWidth());
               }
               if (((ColoredObject *)groupObjects[i])->GetLineStyle() != ((ColoredObject *)obj)->GetLineStyle()) 
               {
                  ((ColoredObject *)groupObjects[i])->ChangeLineStyle(((ColoredObject *)obj)->GetLineStyle());
               }
               if (groupObjects[i]->GetType() == GenericObject::ASCIITEXT) 
               {
                  if (IsTextGroup())
                  {
                     ChangeFontFamily(((Text *)obj)->GetFamily());
                     ChangeFontSize(((Text *)obj)->GetFontSize());
                     ChangeFontWeight(((Text *)obj)->GetWeight());
                     ChangeFontSlant(((Text *)obj)->GetSlant());
                  }
                  else
                  {
                     ((Text *)groupObjects[i])->ChangeFont(((Text *)obj)->GetFamily(), ((Text *)obj)->GetSize(), 
                        ((Text *)obj)->GetWeight(), ((Text *)obj)->GetSlant()); 
                  }

                  string1 = ((Text *)obj)->GetString();
                  string2 = ((Text *)(groupObjects[i]))->GetString();
                  if (string1 != string2) 
                  {
                     ((Text *)groupObjects[i])->ChangeString(string1);
                  }
               }
               if (obj->GetType() == GenericObject::LINE) 
               {
                  if (((Line *)obj)->GetArrowStyle() != ((Line *)groupObjects[i])->GetArrowStyle()) 
                  {
                     ((Line *)obj)->ChangeArrowStyle(((Line *)groupObjects[i])->GetArrowStyle());
                  }
                  if (((Line *)obj)->GetArrowConfig() != ((Line *)groupObjects[i])->GetArrowConfig()) 
                  {
                     ((Line *)obj)->ChangeArrowConfig(((Line *)groupObjects[i])->GetArrowConfig());
                  }
               }
               if ((obj->GetType() == GenericObject::POLYLINE) || (obj->GetType() == GenericObject::FREEHAND)) 
               {
                  if (((PolyLine *)obj)->GetArrowStyle() != ((PolyLine *)groupObjects[i])->GetArrowStyle()) 
                  {
                     ((PolyLine *)obj)->ChangeArrowStyle(((PolyLine *)groupObjects[i])->GetArrowStyle());
                  }
                  if (((PolyLine *)obj)->GetArrowConfig() != ((PolyLine *)groupObjects[i])->GetArrowConfig()) 
                  {
                     ((PolyLine *)obj)->ChangeArrowConfig(((PolyLine *)groupObjects[i])->GetArrowConfig());
                  }
               }
            }
         }
      }
      else 
      {
         groupObjects[i]->SetInvisible();
      }
   }

}

void ASSISTANT::Group::TakeIds(DrawObject *_obj)
{
	DrawObject
		*obj;
	
	for (int i = 0; i < groupObjects.GetSize(); ++i) {
		obj = ((Group *)_obj)->GetObject(i);
		if (obj) {
			groupObjects[i]->SetID(obj->GetID());
		}
	}
}

bool ASSISTANT::Group::HasHyperlink(Gdiplus::PointF &ptMouse)
{
   if (groupObjects.GetSize() == 0)
      return false;
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj) 
      {
         if (obj->HasHyperlink(ptMouse))
            return true;
      }
   }
   
   return false;
}


bool ASSISTANT::Group::HasObjectLinks(Gdiplus::PointF &ptMouse)
{
   if (groupObjects.GetSize() == 0)
      return false;
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj) 
      {
         if (obj->HasObjectLinks(ptMouse))
            return true;
      }
   }
   
   return false;
}


const _TCHAR *ASSISTANT::Group::GetInteractiveObjects(Gdiplus::PointF &ptMouse)
{
   if (groupObjects.GetSize() == 0)
      return NULL;
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj) 
      {
         if (obj->HasObjectLinks(ptMouse))
            return (groupObjects[i])->GetInteractiveObjects(ptMouse);
      }
   }
	
   return NULL;
}

LPCTSTR ASSISTANT::Group::GetHyperlink(Gdiplus::PointF &ptMouse)
{
   if (groupObjects.GetSize() == 0)
      return NULL;
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj && obj->ContainsPoint(ptMouse)) 
      {
         if (obj->HasHyperlink(ptMouse))
            return obj->GetHyperlink(ptMouse);
      }
   }
	

   return NULL;
}

LPCTSTR ASSISTANT::Group::GetLinkDirectory(Gdiplus::PointF &ptMouse)
{
   if (groupObjects.GetSize() == 0)
      return NULL;
	
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj && obj->ContainsPoint(ptMouse)) 
      {
         if (obj->HasHyperlink(ptMouse))
            return obj->GetLinkDirectory(ptMouse);
      }
   }
	
   return NULL;
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

void ASSISTANT::Group::ActivateHyperlink(Gdiplus::PointF &ptObject) 
{
   for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj && obj->ContainsPoint(ptObject)) 
         obj->ActivateHyperlink(ptObject);
   }
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

void ASSISTANT::Group::IncreaseOrder(int increaseValue)
{
	for (int i = 0; i < groupObjects.GetSize(); ++i) 
   {
      DrawObject *obj = groupObjects[i];
      if (obj) 
      {
			obj->IncreaseOrder(increaseValue);
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
			if (groupObjects[i]->GetType() == ASSISTANT::GenericObject::GROUP)
			{
				((Group *)obj)->AppendObjectsToPage(page);
			}
         else if (groupObjects[i]->GetType() == ASSISTANT::GenericObject::ANIMATED) 
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

bool ASSISTANT::Group::ContainsPoint(Gdiplus::PointF &ptMouse)
{
   if (!visible)
      return false; 

   if (IsTextGroup() || groupObjects.GetSize() == 0)
   {
      CRect rcObject;
      GetBoundingBox(rcObject);
      
      if (selected && ASSISTANT::Project::active->GetActivePage())
      {
         float fZoomFactor = ASSISTANT::Project::active->GetActivePage()->GetZoomFactor();
         int iBorder = 6 / fZoomFactor;

         CRgn rgnRectangle;
         CRect rcOutside = rcObject;
         rcOutside.InflateRect(iBorder, iBorder, iBorder, iBorder);
         int bRet = rgnRectangle.CreateRectRgnIndirect(rcOutside);
         if (!bRet)
            return false;

         CRgn rgnRectangle2;
         CRect rcInside = rcObject;
         bRet = rgnRectangle2.CreateRectRgnIndirect(rcInside);
         if (!bRet)
            return false;
         
         rgnRectangle.CombineRgn(&rgnRectangle, &rgnRectangle2, RGN_DIFF);
         if (rgnRectangle.PtInRegion((int)ptMouse.X, (int)ptMouse.Y))
            return true;
         else
            return false;
      }
      else 
      {
         CPoint pt((int)ptMouse.X, (int)ptMouse.Y);
         
         if (rcObject.PtInRect(pt))
            return true;
         else
            return false;
      }
   }
   else
   {
      for (int i = 0; i < groupObjects.GetSize(); ++i)
      {
         if (groupObjects[i]->ContainsPoint(ptMouse))
            return true;
      }
   }
   
   return false;
}

void ASSISTANT::Group::SetDocumentObject(bool bIsDocumentObject)
{
   m_bIsDocumentObject = bIsDocumentObject;

   for (int i = 0; i < groupObjects.GetSize(); ++i)
   {
      groupObjects[i]->SetDocumentObject(bIsDocumentObject);
   }
}
