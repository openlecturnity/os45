/*********************************************************************

  program  : mlb_page.cpp 
  authors  : Gabriela Maass
  date     : 06.03.2001
  revision : $Id: mlb_page.cpp,v 1.70 2010-09-21 05:49:43 maass Exp $
  desc     : 
  
**********************************************************************/

#include "StdAfx.h"
#include "..\..\Studio\Resource.h"

#ifdef WIN32
#pragma warning(disable: 4786)
#endif

/*
#include <set>
#include <tcl.h>
#include <math.h>
#include <time.h>
*/
#include "pointer.h"

#include "mlb_page.h"
#include "la_project.h"
#include "la_document.h"
#include "mlb_misc.h"

#include "lutils.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


ASSISTANT::Page::Page(uint32 _id, const _TCHAR *_name, int _width, int _height, COLORREF clrBackground): ASSISTANT::GenericContainer(_id)  
{
   SetName(_name);
   
   active      = NULL;
   width       = _width;
   height      = _height;
   m_argbBackground = ASSISTANT::ColorrefToGdipARGB(clrBackground);
   zoomFactor_ = 1.0;
   bFullScreenOn = false;
   
   Init();
   
   undoList = new UndoList();
   
   Project::active->pageCount++;
   
   pageNumber       = Project::active->pageCount;
   firstTime        = -1;
   insertionCounter = 0;
   
   TCHAR buf[20];
   _stprintf(buf, _T("a%d"), pageNumber);
   pageID_ = buf;
    
   backRect = new Rectangle(0.0, 0.0, width, height, -1, 
                            m_argbBackground, Project::active->GetObjectID(), 
                            NULL, NULL, NULL);
}

/***********************/
/***  CONSTRUCTORS  ***/
/**********************/

ASSISTANT::Page::Page(uint32 _id, _TCHAR *szPageNumber, GenericContainer *session, SGMLElement *_root) 
: ASSISTANT::GenericContainer(_id)
{
   if (!_root)
   {
      SetName(NULL);
      active      = NULL;
      width       = A4_HEIGHT;
      height      = A4_WIDTH;
      m_argbBackground  = Gdiplus::Color::MakeARGB(255, 255, 255, 255);
      zoomFactor_ = 1.0;
	  bFullScreenOn = false;
      
      Init();
      
      undoList = new UndoList();
      
      Project::active->pageCount++;
      
      pageNumber       = Project::active->pageCount;
      firstTime        = -1;
      insertionCounter = 0;
      pageID_          = _T("-1");
      
      backRect = new ASSISTANT::Rectangle(0.0, 0.0, width, height, -1, 
                                          m_argbBackground, Project::active->GetObjectID(), 
                                          NULL, NULL, NULL);
      
      return;
   }
   
   active = NULL;
   
   TCHAR *tmp;
   tmp = _root->GetAttribute("name");
   SetName(tmp);
   zoomFactor_ = 1.0;
   bFullScreenOn = false;
   
   tmp = _root->GetAttribute("width");
   if (tmp) width = _ttoi(tmp);
   else     width = A4_WIDTH;
   
   tmp = _root->GetAttribute("height");
   if (tmp) height = _ttoi(tmp);
   else     height = A4_HEIGHT;   
   
   CString background;
   tmp = _root->GetAttribute("color");
   if (tmp) background = tmp;
   else     background = _T("#ffffff");   

   m_argbBackground = ASSISTANT::StringToGdipARGB(background);
   
   tmp = _root->GetAttribute("keywords");
   if (tmp) scorm.SetKeywords(tmp);
   else     scorm.SetKeywords(_T(""));   
   
   tmp = _root->GetAttribute("slideid");
   if (tmp) pageID_ = tmp;
   else     pageID_ = _T("-1");   
   
   Init();
   
   if (session)
      imagePath_ = ((Document *)session)->GetDocumentPath();
   else 
      imagePath_ = _T(".");
   
   // Set page number
   Project::active->pageCount++;
   pageNumber       = Project::active->pageCount; 
   
   backRect = NULL;
   LoadObjects(_root, imagePath_, szPageNumber);
   
   undoList  = new UndoList();
   
   firstTime        = -1;
   insertionCounter = 0;
   if (pageID_ == _T("-1"))
   {
      TCHAR buf[20];
      _stprintf(buf, _T("a%d"),pageNumber);
      pageID_ = buf;
   }
   
   if (backRect == NULL)
       backRect = new ASSISTANT::Rectangle(0.0, 0.0, width, height, -1,
       m_argbBackground, Project::active->GetObjectID(), 
       NULL, NULL, NULL);
}


/***************************/
/***  END CONSTRUCTORS  ***/
/**************************/



/*********************/
/***  DESTRUCTORS  ***/
/*********************/


ASSISTANT::Page::~Page()
{
   if (undoList) 
      delete undoList;
   
   containerName.Empty();
   nodeName.Empty();
   
   if (activeObjects.GetSize() > 0) 
   {
      for (int i = 0; i < activeObjects.GetSize(); ++i)
      {
         if (activeObjects[i] != NULL && 
            (activeObjects[i]->GetType() == GenericObject::GROUP || 
             activeObjects[i]->GetType() == GenericObject::ANIMATED))
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
   
   if (onlinePointer) 
   {
      delete onlinePointer;
   }
   
   //delete canvas;
   active = NULL;

   if (backRect)
      delete backRect;

}


/*************************/
/***  END DESTRUCTORS  ***/
/*************************/



void ASSISTANT::Page::AddToStructureTree(CDocumentStructureView *pDocStructTree, UINT nParentId)
{
   if (pDocStructTree == NULL)
      return;

   pDocStructTree->InsertPage((ASSISTANT::GenericContainer *)this, nParentId);
}

/***************************/
/***  GENERAL FUNCTIONS  ***/
/***************************/

void ASSISTANT::Page::Init() 
{
   InitTelepointer(); 
}

ASSISTANT::Page *ASSISTANT::Page::Copy() 
{
   Page
      *ret;
   DrawObject
      *obj;
   
   ret = new Page(Project::active->GetObjectID(), (LPCTSTR)containerName, width, height, m_argbBackground);
   ret->SetCodepage(GetCodepage());
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   { 
      
      if (activeObjects[i]->IsVisible() || activeObjects[i]->GetType() == GenericObject::ANIMATED)
         obj = activeObjects[i]->Copy(true);
      else
         obj = NULL;
      
      if (obj)
      {
         if (obj->GetType() == ASSISTANT::GenericObject::GROUP)
            ((Group *)obj)->AppendObjectsToPage(ret);
         else if (obj->GetType() == ASSISTANT::GenericObject::ANIMATED)
            ((AnimatedObject *)obj)->AppendObjectsToPage(ret);
         
         // All not animated objects are visible after copy. For animated objects the visibility is set in copy
         /*
         if (obj->GetType() == GenericObject::ANIMATED)
         obj->SetInvisible();
         else
         obj->SetVisible();
         */
         
         ret->AppendObject(obj);
      }
   }
   
   return ret;
}

void ASSISTANT::Page::InitTelepointer() 
{
   onlinePointer = new Telepointer(0.0, 0.0, Gdiplus::Color::MakeARGB(255, 255, 0, 0), ASSISTANT::Telepointer::INACTIVE, Project::active->GetObjectID());
}


void ASSISTANT::Page::LoadObjects(SGMLElement *_root, LPCTSTR path, LPCTSTR szPageNumber)
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
   
   // find out background rectangle
   if (hilf->son && hilf->son->Name == _T("GROUP")) 
   {
      SGMLElement *pGroupNode = hilf->son; 
      SGMLElement *pBackground = pGroupNode->son;
      if (pBackground && pBackground->Name ==  _T("RECTANGLE"))
      {
         Rectangle *pRectangle = (Rectangle *)Rectangle::Load(pBackground);
         if (pRectangle &&
             pRectangle->GetWidth() == GetWidth() &&
             pRectangle->GetHeight() == GetHeight())
         {
            backRect = pRectangle;
            pGroupNode->son = pBackground->next;
            if (pGroupNode->son)
            {
               pGroupNode->son->prev = NULL;
               pGroupNode->son->parent = pGroupNode;
            }
            delete pBackground;
         }
         else if (pRectangle)
            delete pRectangle;
      }
   }

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
         obj = WindowsImage::Load(hilf, imagePath_);
         if (obj) 
         {
            ((WindowsImage *)obj)->BuildImage(this, (_TCHAR *)(LPCTSTR)path, false);
         }
      }
      else if (hilf->Name == _T("TEXT"))  // Text only appear in Group
      {
         obj = Text::Load(hilf);
         if (obj)
         {
            // check if font is installed TrueType font
            // if not: add page number to font list
            
            FontList *fontList = ASSISTANT::Project::active->GetFontList();
            
            if (!fontList->HasTTFFilename(((Text *)obj)->GetFamily()))
            {
               ((Text *)obj)->SetFontUnsupported();
               fontList->AddErrorPage(((Text *)obj)->GetFamily(), szPageNumber, GetPageNumber());
            }
            
            Group* group = new Group(Project::active->GetObjectID(), NULL, NULL, NULL);
         
            if (!group)
            {
               continue;
            }
            AppendObject(obj, false);
            group->AddObject(obj);
            group->UpdateDimension();

            obj = group;
         }
      }
      else if (hilf->Name == _T("GROUP")) 
      {
         obj = Group::Load(hilf);
         if (obj)
         {
            ((Group *)obj)->LoadObjects(this, szPageNumber, hilf, path);
            ((Group *)obj)->UpdateDimension();
            if (hilf->GetAttribute("type") != NULL && 
                _tcscmp(hilf->GetAttribute("type"), _T("MASTER")) == 0)
            {
               delete obj;
               obj = NULL;
            }
         }

      }
      else if (hilf->Name == _T("ANIMATED")) 
      {
         obj = AnimatedObject::Load(hilf);
         if (obj)
            ((AnimatedObject *)obj)->LoadObjects(this,  szPageNumber, hilf, (_TCHAR *)(LPCTSTR)path);
      }
      
      if (obj) 
      {
         obj->SetDocumentObject(true);
         AppendObject(obj);
      }
   }
}


void ASSISTANT::Page::SetName(LPCTSTR _name)
{
   containerName.Empty();
   
   if (_name == NULL) 
      containerName.LoadString(IDS_DEFAULT_PAGE_NAME);
   else    
      containerName = _name;  
   
   scorm.SetTitle(containerName);
}

void ASSISTANT::Page::SetKeywords(LPCTSTR szKeywords)
{
   scorm.SetKeywords(szKeywords);
   
   return;
}

HRESULT ASSISTANT::Page::GetKeywords(CString &csKeywords)
{  
   csKeywords = scorm.GetKeywords();
   
   return S_OK;
}

void ASSISTANT::Page::Activate() 
{
   active = NULL;
   UnSelectObjects();
   
   int time = ASSISTANT::Project::active->GetRecordingTime();
   
   Project::active->thumbs_.AddElement(this, time);
   
   SaveAsAOF(time);
   
   return;
}

void ASSISTANT::Page::DeActivate() 
{
   // Reset the mouse cursor to its previous
   // state. This fixes the following problem:
   // If you leave the mouse over a hyperlinked
   // object and then switch page by using the
   // keyboard, the LeaveWidget event is not
   // invoked, and thus the mouse cursor remains
   // a hand cursor.
   //canvas->ResetHyperlinkCursor();
   
   if (onlinePointer && onlinePointer->IsActive())
   {
      onlinePointer->SetInactive();
   }

   ResetInteractiveObjects();
}

void ASSISTANT::Page::SelectAllObjects() 
{
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if ((activeObjects[i]->GetType() == GenericObject::IMAGE && Project::active->DoFreezeImages()) ||
          (activeObjects[i]->IsMasterObject() && Project::active->DoFreezeMaster()) ||
          (activeObjects[i]->IsDocumentObject() && Project::active->DoFreezeDocument()))
         activeObjects[i]->SetSelected(false);
      else if (activeObjects[i]->IsVisible())
         activeObjects[i]->SetSelected(true);
   }
}

void ASSISTANT::Page::Undo()
{
   undoList->UndoLastAction();
   UnSelectObjects();
   SaveAsAOF(); 
}

int ASSISTANT::Page::GetLastUndoTime()
{
   int lastUndoTime = -1;
   
   if (undoList)
      lastUndoTime = undoList->GetLastUndoTimestamp();
   
   return lastUndoTime;
}

void ASSISTANT::Page::CutObjects()
{
   if (!Project::active) 
      return;

   CArray<DrawObject *, DrawObject *>arSelectedObjects;
   GetSelectedObjects(arSelectedObjects);

   if (arSelectedObjects.GetSize() > 0) 
   {
      undoList->AppendAction(UndoAction::UNDODELETE, arSelectedObjects);
      
      AddSelectedObjectsToPasteBuffer(arSelectedObjects);
      
      // Set Objects unvisible
      for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
      {
         arSelectedObjects[i]->Hide();
      }
      
      SaveAsAOF();
      UnSelectObjects();
   }
   
   active = NULL;
}

void ASSISTANT::Page::DeleteObjects(bool bDoUndo)
{
   if (!Project::active) 
      return;

   CArray<DrawObject *, DrawObject *>arSelectedObjects;
   GetSelectedObjects(arSelectedObjects);

   if (arSelectedObjects.GetSize() > 0) 
   {
      if (bDoUndo)
         undoList->AppendAction(UndoAction::UNDODELETE, arSelectedObjects);
      
      // Set Objects unvisible
      for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
      {
         arSelectedObjects[i]->Hide();
      }
      
      SaveAsAOF();
      UnSelectObjects();
   }
   
   active = NULL;
}

void ASSISTANT::Page::CopyObjects()
{
   if (!Project::active) 
      return;
   
   if (objects.GetSize() == 0) return;
   
   CArray<DrawObject *, DrawObject *>arSelectedObjects;
   GetSelectedObjects(arSelectedObjects);

   AddSelectedObjectsToPasteBuffer(arSelectedObjects);
   
   insertionCounter = 1;
   
   active = NULL;
}

void ASSISTANT::Page::PasteObjects()
{
   CArray<DrawObject *, DrawObject *>
      undoObjects;
   DrawObject
      *obj;
   
   if (!Project::active || Project::active->IsPasteBufferEmpty())
      return;
   
   UnSelectObjects();
   active = NULL;
   
   CArray<DrawObject *, DrawObject *> pasteBuffer;
   Project::active->GetPasteBuffer(pasteBuffer);
   
   int nextObjectOrder = GetEndObjectOrder();
   for (int i = 0; i < pasteBuffer.GetSize(); ++i) 
   {
      obj = pasteBuffer[i]->Copy(true);
      // The new visibility order is the order of the object increased 
      // by the order of the last object of the page
      obj->IncreaseOrder(nextObjectOrder);
      
      
      if (obj)
      {
         // Append all objects which belong to the group or animation to the page
         if (obj->GetType() == GenericObject::GROUP)
            ((Group *)obj)->AppendObjectsToPage(this);
         else if (obj->GetType() == GenericObject::ANIMATED)
            ((AnimatedObject *)obj)->AppendObjectsToPage(this);
         // the visibility is set in copy and AppendObjectsToPage
         //obj->SetInvisible();
         
         if (obj->GetType() == GenericObject::IMAGE) 
            ((WindowsImage *)obj)->BuildImage(((WindowsImage *)pasteBuffer[i])->GetImage(), false);
         undoObjects.Add(obj);
         AppendObject(obj);
         obj->SetX(obj->GetX() + (4*insertionCounter));
         obj->SetY(obj->GetY() + (4*insertionCounter));
         obj->SetSelected(true);
      }
   }
   
   insertionCounter++;
   
   undoList->AppendAction(UndoAction::UNDOINSERT,undoObjects);
   undoObjects.RemoveAll();
   
   SaveAsAOF();
}


void  ASSISTANT::Page::AddSelectedObjectsToPasteBuffer(CArray<DrawObject *, DrawObject *> &arSelectedObjects)
{ 
   
   Project::active->ClearPasteBuffer();

   if (arSelectedObjects.GetSize() > 0) 
   {							
      // Insert objects to paste buffer and in sorted (visibility order) list 
      CArray<DrawObject *, DrawObject *> sortedList;
      for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
      {
         DrawObject *obj = arSelectedObjects[i]->Copy(true);
         
         if (obj)
         {
            obj->SetPage(this);
            Project::active->AddObjectToPasteBuffer(obj);
            obj->InsertInVisibilityOrder(sortedList);
         }
      }
      
      // Renumber the objects
      int newOrder = 1;
      for (i = 0; i != sortedList.GetSize(); ++i) 
      {
         sortedList[i]->SetOrder(newOrder);
         ++newOrder;
      }
      
      sortedList.RemoveAll();
   }
}

/*******************************/
/***  END GENERAL FUNCTIONS  ***/
/*******************************/



/**************/
/***  DRAW  ***/
/**************/


void ASSISTANT::Page::DrawWhiteboard(CRect &rcWhiteboard, CDC *pDC)
{
   zoomFactor_ = CalculateZoomFactor(rcWhiteboard);

   UINT nOffsetX = 0;   
   UINT nOffsetY = 0;
   CalculateOffset(rcWhiteboard, zoomFactor_, nOffsetX, nOffsetY);

   //backRect->ChangeLineWidth(1);
   //backRect->Draw(pDC, zoomFactor_, nOffsetX, nOffsetY);
   
   
   Gdiplus::ARGB blackCol = ASSISTANT::ColorrefToGdipARGB(0x00000000);
   Gdiplus::DashStyle gdipLineStyle = Gdiplus::DashStyleSolid;
   ASSISTANT::Rectangle *blackBorder = new ASSISTANT::Rectangle(0.0, 0.0, width, height, -1, 
																blackCol, m_argbBackground, 1, gdipLineStyle, 0,
																NULL, NULL, NULL);
   blackBorder->Draw(pDC, zoomFactor_, nOffsetX, nOffsetY);
   delete blackBorder;

   if(!bFullScreenOn)
   {
	   Gdiplus::ARGB bkBlack = Gdiplus::Color::MakeARGB(255, 0, 0, 0);
	   ASSISTANT::Rectangle *shRRect = new ASSISTANT::Rectangle(width, 4.0, 4.0, height, -1, 
		   bkBlack, 0, NULL, NULL, NULL);
	   ASSISTANT::Rectangle *shDRect = new ASSISTANT::Rectangle(4.0, height, width, 4.0, -1, 
		   bkBlack, 0, NULL, NULL, NULL);
	   shRRect->Draw(pDC, zoomFactor_, nOffsetX, nOffsetY);
	   shDRect->Draw(pDC, zoomFactor_, nOffsetX, nOffsetY);

	   delete shRRect;
	   delete shDRect;
   }

   pDC->IntersectClipRect(nOffsetX + 1, nOffsetY + 1, nOffsetX + width * zoomFactor_ + 0.5, nOffsetY + height * zoomFactor_ + 0.5);

   for (int i = 0; i < objects.GetSize(); ++i)
      objects[i]->Draw(pDC, zoomFactor_, nOffsetX, nOffsetY); 

   for (i = 0; i <activeObjects.GetSize(); ++i)
   {
      if (activeObjects[i] && activeObjects[i]->IsSelected())
         activeObjects[i]->DrawSelection(pDC, zoomFactor_, nOffsetX, nOffsetY); 
   }
   
   if (onlinePointer) 
      onlinePointer->Draw(pDC, zoomFactor_, nOffsetX, nOffsetY);
}

void ASSISTANT::Page::DrawThumbWhiteboard(CRect rcWhiteboard, CDC *pDC)
{
   float fZoomFactor = 1.0;
   
   if (width > 0 && height > 0)
   {
	  
	  float fZoomX = (float)(rcWhiteboard.Width()) / width;
      float fZoomY = (float)(rcWhiteboard.Height()) / height;
      
      if (fZoomX > fZoomY)
         fZoomFactor = fZoomY;
      else
         fZoomFactor = fZoomX;
   }
   UINT nOffsetX = rcWhiteboard.left;   
   UINT nOffsetY = rcWhiteboard.top;
   Gdiplus::ARGB blackCol = ASSISTANT::ColorrefToGdipARGB(0x00000000);
   Gdiplus::DashStyle gdipLineStyle = Gdiplus::DashStyleSolid;
   ASSISTANT::Rectangle *blackBorder = new ASSISTANT::Rectangle(0.0, 0.0, width, height, -1, 
																blackCol, m_argbBackground, 1, gdipLineStyle, 0,
																NULL, NULL, NULL);
   blackBorder->Draw(pDC, fZoomFactor, nOffsetX, nOffsetY);
   delete blackBorder;

   pDC->IntersectClipRect(nOffsetX + 1, nOffsetY + 1, nOffsetX + width * fZoomFactor + 0.5, nOffsetY + height * fZoomFactor + 0.5);

   for (int i = 0; i < objects.GetSize(); ++i)
      objects[i]->Draw(pDC, fZoomFactor, nOffsetX, nOffsetY);

}

void ASSISTANT::Page::Zoom()
{
}


/******************/
/***  END DRAW  ***/
/******************/



/*************************/
/***  DELETE OR CLEAR  ***/
/*************************/

void ASSISTANT::Page::Clear() 
{
   for (int i = 0; i < objects.GetSize(); ++i) 
   {
      objects[i]->Hide();
   }
   
}


void ASSISTANT::Page::DeleteObjectFromContainer(GenericObject* obj) 
{
   for (int i = 0; i < objects.GetSize(); ++i) 
   {
      if (objects[i] == obj) 
      {
         objects.RemoveAt(i);
         return;
      }
   }
}


/*****************************/
/***  END DELETE OR CLEAR  ***/
/*****************************/


/**************/
/***  SAVE  ***/
/**************/

void ASSISTANT::Page::SaveContainer(FILE *_fp, LPCTSTR path, int _level, bool all)
{
   WCHAR *tab = (WCHAR *)malloc((_level*2+1)*sizeof(WCHAR));
   *tab = L'\0';
   for (int i = 0; i < _level; i++) 
      wcscat(tab, L"  ");

   CString csSgmlString;

   CString csName = containerName;
   StringManipulation::TransformForSgml(csName, csSgmlString);
   WCHAR *wszName = ASSISTANT::ConvertTCharToWChar(csSgmlString);

   CString csKeywords = scorm.GetKeywords();
   StringManipulation::TransformForSgml(csKeywords, csSgmlString);
   WCHAR *wszKeywords = ASSISTANT::ConvertTCharToWChar(csSgmlString);
   
   CString csColor;
   ASSISTANT::GdipARGBToString(m_argbBackground, csColor);
   WCHAR *wszColor = ASSISTANT::ConvertTCharToWChar(csColor);

   fwprintf(_fp, L"%s<PAGE name=\"%s\" width=\"%d\" height=\"%d\" color=\"%s\" slideid=\"%s\">\n",
                 tab, wszName, width, height, wszColor, pageID_);
   
   if (wszName)
      delete wszName;

   if (wszKeywords)
      delete wszKeywords;
   
   if (wszColor)
      delete wszColor;
   
   if (activeObjects.GetSize() > 0) 
   {
      for (i = 0; i < activeObjects.GetSize(); ++i)
         activeObjects[i]->SaveObject(_fp, (_level+1), path);
   }
   
   fwprintf(_fp, L"%s</PAGE>\n", tab);
   
   if (all)
      undoList->Clean();

   if (tab)
      delete tab;
}


void ASSISTANT::Page::SaveMetadata(CFileOutput *pLmdFile)
{
   CString csLine;
   CString csSgmlLine;
   
   
   csLine = _T("    <page>\n");
   pLmdFile->Append(csLine);

   csLine = containerName;
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   csLine.Format(_T("      <title>%s</title>\n"), csSgmlLine);
   pLmdFile->Append(csLine);

   csLine.Format(_T("      <nr>%d</nr>\n"), GetPageNumber());
   pLmdFile->Append(csLine);

   csLine.Format(_T("      <id>%s</id>\n"), pageID_);
   pLmdFile->Append(csLine);
   
   CStringArray arKeywords;
   CString keywords = scorm.GetKeywords();
   ASSISTANT::CreateListFromString(keywords, arKeywords);
   for (int i = 0; i < arKeywords.GetSize(); ++i) 
   {
      CString csLine = arKeywords[i];
      StringManipulation::TransformForSgml(csLine, csSgmlLine);
      csLine.Format(_T("      <keyword>%s</keyword>\n"), csSgmlLine);
      pLmdFile->Append(csLine);
   }
   
   csLine = _T("    </page>\n");
   pLmdFile->Append(csLine);
}


/******************/
/***  END SAVE  ***/
/******************/



/*************/
/*** FIND  ***/
/*************/


ASSISTANT::GenericObject* ASSISTANT::Page::GetObject(uint32 _id) {

   DrawObject
      *group;
   
   for (int i = 0; i < objects.GetSize(); ++i) 
   {
      if (objects[i]->GetID() == _id) 
      {
         return objects[i];
      }
      if (group = objects[i]->GetGroup()) 
      {
         if (group->GetID() == _id) 
         {
            return (group);
         }
      }
   }
   
   return NULL;
}


ASSISTANT::GenericObject* ASSISTANT::Page::GetGroup(uint32 _id) 
{
   for (int i = 0; i < activeObjects.GetSize(); ++i) 
   {
      if ((activeObjects[i]->GetID() == _id) && (activeObjects[i]->GetType() == GenericObject::GROUP)) 
         return activeObjects[i];
      if ((activeObjects[i]->GetID() == _id) && (activeObjects[i]->GetType() == GenericObject::ANIMATED)) 
         return activeObjects[i];
   }
   
   return NULL;
}


float ASSISTANT::Page::GetObjectOrder(uint32 _id) 
{
   for (int i = 0; i < objects.GetSize(); ++i) 
   {
      if (objects[i]->GetID() == _id) 
         return objects[i]->GetOrder();
   }
   
   return 0; // not found
}


float ASSISTANT::Page::GetPushBackObjectOrder() 
{
   if (objects.GetSize() == 0) 
   {
      lastObjectOrder =  OO_FIRST;
   }
   else 
   {
      uint32 order = objects[objects.GetSize()-1]->GetOrder();
      lastObjectOrder = order + OO_OFFSET;
   } 
   
   return lastObjectOrder;
}


float ASSISTANT::Page::GetEndObjectOrder() 
{
   if (objects.GetSize() == 0) 
      return OO_FIRST;
   else
      return (objects[objects.GetSize()-1])->GetOrder();
}


float ASSISTANT::Page::GetFirstObjectOrder() 
{
   if (objects.GetSize() == 0) 
      return 0;
   else
      return (objects[0])->GetOrder();
}


float ASSISTANT::Page::GetNextObjectOrder(uint32 _id) 
{
   for (int i = 0; i < objects.GetSize(); ++i) 
   {
      if (objects[i]->GetID() == _id) 
      {
         if (i == objects.GetSize()-1) 
            return objects[i]->GetOrder() + OO_OFFSET;
         else 
         {
            uint32 newOrder = objects[i]->GetOrder();
            ++i;
            float orderDiff = (objects[i]->GetOrder() - newOrder) / 2;
            return newOrder + orderDiff;
         }
      }
   }
   
   return 0;
}


/******************/
/***  END FIND  ***/
/******************/



/*******************/
/*** ATTRIBUTES  ***/
/*******************/


void ASSISTANT::Page::SetPageDimension(int _width, int _height)
{
   if (_width != width)//(_width > width)
   {
      backRect->SetWidth(_width);
      backRect->SetUnrecorded();
   }
   
   if (_height != height)//(_height > height)
   {
      backRect->SetHeight(_height);
      backRect->SetUnrecorded();
   }
   
   width  = _width;
   height = _height;
   
   SaveAsAOF();
}

void ASSISTANT::Page::SetColor(COLORREF clrBackground) 
{
   m_argbBackground = ASSISTANT::ColorrefToGdipARGB(clrBackground);
   
   backRect->ChangeLineColor(m_argbBackground);
   backRect->ChangeFillColor(m_argbBackground);
   backRect->SetUnrecorded();
   
   SaveAsAOF();
}

/************************/
/***  END ATTRIBUTES  ***/
/************************/



/****************/
/***  RETURN  ***/
/****************/


ASSISTANT::DrawObject *ASSISTANT::Page::GetActiveObject()
{
   
   if (objects.GetSize() == 0) return NULL;
   
   return active;
}


/********************/
/***  END RETURN  ***/
/********************/



/****************/
/***  INSERT  ***/
/****************/


void ASSISTANT::Page::AppendObject(DrawObject *obj, bool isActive, bool bAppendToUndo) 
{
   
   obj->SetPage(this);
   
   
   if	(obj->GetType() == ASSISTANT::GenericObject::ANIMATED)
   {
      // If the type of the animation is EXIT this animation may be shown first
      if (((AnimatedObject *)obj)->GetAnimationType() == AnimatedObject::EXIT)
      {
         bool bSetVisible = true;
         // Find out if there is an animation with the same id in activeObjects
         AnimatedObject *animObj = NULL;
         for (int i = 0; i < activeObjects.GetSize(); ++i)
         {
            if (activeObjects[i] && activeObjects[i]->GetType() == GenericObject::ANIMATED)
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
   
   if (obj && bAppendToUndo) 
      undoList->AppendAction(UndoAction::UNDOINSERT,obj);
         
   if (isActive)
      activeObjects.Add(obj);
   
   if (obj->GetType() == ASSISTANT::GenericObject::GROUP)
      return;
   
   if	(obj->GetType() == ASSISTANT::GenericObject::ANIMATED)
      return;
   
   if (objects.GetSize() == 0  || obj->GetOrder() < 0) 
   {
      if (obj->GetOrder() < 0)
         obj->SetOrder(GetPushBackObjectOrder());
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
   if (i >= 0)
   {
      if (objects[i]->GetOrder() <= obj->GetOrder())
         ++i;
   }
   else  // the new object has a lower order than every other object => insert at first position
      i = 0;
   objects.InsertAt(i, obj);
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

HRESULT ASSISTANT::Page::SelectObjects(CRect &rcObjects)
{  
   HRESULT hr = S_OK;

   if (activeObjects.GetSize() == 0) 
      return hr;
   
   UnSelectObjects();
   for (int i = activeObjects.GetSize()-1; i >= 0; --i)
   {
      if (activeObjects[i]->IsInRect(rcObjects))
      {
         DrawObject *pObject = activeObjects[i];
         if (pObject != NULL) 
         {
            if (!((pObject->GetType() == GenericObject::IMAGE && Project::active->DoFreezeImages()) ||
                  (activeObjects[i]->IsMasterObject() && Project::active->DoFreezeMaster()) ||
                  (activeObjects[i]->IsDocumentObject() && Project::active->DoFreezeDocument())))
               pObject->SetSelected(true);
         }
      }
   }

   return hr;
}

bool ASSISTANT::Page::HasSelectedObjects()
{
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if (activeObjects[i]->IsSelected())
         return true;
   }

   return false;
}

HRESULT ASSISTANT::Page::SelectSingleObject(CRect &rcObject)
{
   HRESULT hr = S_OK;

   if (activeObjects.GetSize() == 0) 
      return hr;
   
   UnSelectObjects();
   for (int i = activeObjects.GetSize()-1; i >= 0; --i)
   {
      if (activeObjects[i]->ContainsRect(rcObject))
      {
         DrawObject *pObject = activeObjects[i];
         if (pObject != NULL) 
         {

            if (!((pObject->GetType() == GenericObject::IMAGE && Project::active->DoFreezeImages()) ||
                  (activeObjects[i]->IsMasterObject() && Project::active->DoFreezeMaster()) ||
                  (activeObjects[i]->IsDocumentObject() && Project::active->DoFreezeDocument())))
            {
               pObject->SetSelected(true);
               return hr;
            }
         }
      }
   }

   return hr;
}

void ASSISTANT::Page::UnSelectObjects()
{
   active = NULL;
   
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if (activeObjects[i] != NULL)
      {
         activeObjects[i]->SetSelected(false);

         // Delete empty text fields
         if (activeObjects[i]->GetType() == DrawObject::GROUP &&
             ((Group *)activeObjects[i])->IsEmptyText())
            activeObjects[i]->Hide();
      }
   }
}

void ASSISTANT::Page::UnSelectImages()
{
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if (activeObjects[i]->GetType() == GenericObject::IMAGE)
         activeObjects[i]->SetSelected(false);
   }
}

void ASSISTANT::Page::UnSelectMasterObjects()
{
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if (activeObjects[i]->IsMasterObject())
         activeObjects[i]->SetSelected(false);
   }
}

void ASSISTANT::Page::UnSelectDocumentObjects()
{
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if (activeObjects[i]->IsDocumentObject())
         activeObjects[i]->SetSelected(false);
   }
}


/********************/
/***  END SELECT  ***/
/********************/



/****************/
/***  CHANGE  ***/
/****************/

void ASSISTANT::Page::Change(DrawObject *pObj, bool bAlreadyInserted)
{
   if (bAlreadyInserted)
      undoList->AppendAction(UndoAction::UNDOCHANGE, pObj);
   else
      undoList->AppendAction(UndoAction::UNDOINSERT, pObj);
}

void ASSISTANT::Page::ChangeFontFamily(LPCTSTR szFontFamily)
{
   Group
      *group;
   
   if (objects.GetSize() == 0) return;
   
   if (active && (active->GetType() == GenericObject::ASCIITEXT))  // 1. insert new text element into active text object
   {
      if (active->GetGroup() && active->GetGroup()->GetType() == GenericObject::GROUP)
      {
         group = (Group *)active->GetGroup();
         if (!group) return; // error
         //group->InsertNewFontfamily(this, szFontFamily);
         SaveAsAOF();
      }
   }
   else                                                            // 2. change font of whole selected textgroup
   {
      
      CArray<DrawObject *, DrawObject *>arSelectedObjects;
      GetSelectedObjects(arSelectedObjects);

      if (arSelectedObjects.GetSize() > 0) 
      {
         undoList->AppendAction(UndoAction::UNDOCHANGE, arSelectedObjects);
         for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
         {
            if (arSelectedObjects[i]->GetType() == GenericObject::GROUP) 
            {
               ((Group *)arSelectedObjects[i])->ChangeFontFamily(szFontFamily);
            }
            else if (arSelectedObjects[i]->GetType() == GenericObject::ASCIITEXT) 
            {
               ((Text *)arSelectedObjects[i])->ChangeFontFamily(szFontFamily);
            }
         }
         SaveAsAOF();
      }
   }
}

void ASSISTANT::Page::ChangeFontSize(int iFontSize)
{
   Group
      *group;
   
   if (objects.GetSize() == 0) return;
   
   if (active && (active->GetType() == GenericObject::ASCIITEXT))  // 1. insert new text element into active text object
   {
      if (active->GetGroup() && active->GetGroup()->GetType() == GenericObject::GROUP)
      {
         group = (Group *)active->GetGroup();
         if (!group) return; // error
         //group->InsertNewFontsize(this, iFontSize);
         SaveAsAOF();
      }
   }
   else                                                           // 2. change font of whole selected textgroup
   {
      CArray<DrawObject *, DrawObject *>arSelectedObjects;
      GetSelectedObjects(arSelectedObjects);

      if (arSelectedObjects.GetSize() > 0) 
      {
         undoList->AppendAction(UndoAction::UNDOCHANGE, arSelectedObjects);
         for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
         {
            if (arSelectedObjects[i]->GetType() == GenericObject::GROUP) 
            {
               ((Group *)arSelectedObjects[i])->ChangeFontSize(iFontSize);
            }
            else if (arSelectedObjects[i]->GetType() == GenericObject::ASCIITEXT) 
            {
               ((Text *)arSelectedObjects[i])->ChangeFontSize(iFontSize);
            }
         }
         SaveAsAOF();
      }
   }
}

void ASSISTANT::Page::ChangeFontWeight(bool bBold)
{
   if (objects.GetSize() == 0) return;
   
   CString weight = _T("normal");
   if (bBold)
      weight = _T("bold");

   if (active && (active->GetType() == GenericObject::ASCIITEXT))  // 1. insert new text element into active text object
   {
      if (active->GetGroup() && active->GetGroup()->GetType() == GenericObject::GROUP)
      {
         Group *group = (Group *)active->GetGroup();
         if (!group) return; // error
         //group->InsertNewFontweight(this, weight);
         SaveAsAOF();
      }
   }
   else                                                           // 2. change font of whole selected textgroup
   {
      CArray<DrawObject *, DrawObject *> arSelectedObjects;
      GetSelectedObjects(arSelectedObjects);
      if (arSelectedObjects.GetSize() > 0) 
      {
         undoList->AppendAction(UndoAction::UNDOCHANGE, arSelectedObjects);
         for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
         {
            if (arSelectedObjects[i]->GetType() == GenericObject::GROUP) 
            {
               ((Group *)arSelectedObjects[i])->ChangeFontWeight(weight);
            }
         }
         SaveAsAOF();
      }
   }
}


void ASSISTANT::Page::ChangeFontSlant(bool bItalic)
{
   if (objects.GetSize() == 0) return;
   
   CString slant = _T("roman");
   if (bItalic)
      slant = _T("italic");
   
   if (active && (active->GetType() == GenericObject::ASCIITEXT))   // 1. insert new text element into active text object
   { 
      if (active->GetGroup() && active->GetGroup()->GetType() == GenericObject::GROUP)
      {
         Group *group = (Group *)active->GetGroup();
         if (!group) return; // error
         //group->InsertNewFontslant(this, slant);
         SaveAsAOF();
      }
   }
   else                                                             // 2. change font of whole selected textgroup
   {
      CArray<DrawObject *, DrawObject *> arSelectedObjects;
      GetSelectedObjects(arSelectedObjects);
      if (arSelectedObjects.GetSize() > 0) 
      {
         undoList->AppendAction(UndoAction::UNDOCHANGE, arSelectedObjects);
         for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
         {
            if (arSelectedObjects[i]->GetType() == GenericObject::GROUP) 
            {
               ((Group *)arSelectedObjects[i])->ChangeFontSlant(slant);
            }
         }
         SaveAsAOF();
      }
   }
}

void ASSISTANT::Page::ChangeLineColor(COLORREF clrLine, int iTransparency)
{
   CArray<DrawObject *, DrawObject *>arSelectedObjects;
   GetSelectedObjects(arSelectedObjects);

   if (arSelectedObjects.GetSize() == 0) return;

   Gdiplus::ARGB argbLineColor = ASSISTANT::ColorrefToGdipARGB(clrLine, iTransparency);
   
   if (active && (active->GetType() == GenericObject::ASCIITEXT)) 
   {   
      if (active->GetGroup() && active->GetGroup()->GetType() == GenericObject::GROUP)
      {
         //((Group*)active->GetGroup())->InsertNewColor(this, lineColor);
      }
   }
   else
   {
      undoList->AppendAction(UndoAction::UNDOCHANGE, arSelectedObjects);
      
      for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
      {
         if (arSelectedObjects[i]->GetType() != GenericObject::IMAGE &&
             arSelectedObjects[i]->GetType() != GenericObject::ASCIITEXT) 
         {
            if (arSelectedObjects[i]->GetType() == GenericObject::GROUP) 
            {
               if (!((Group *)arSelectedObjects[i])->IsTextGroup())
                  ((Group*)arSelectedObjects[i])->ChangeLineColor(argbLineColor);
            }
            else 
               ((ColoredObject*)arSelectedObjects[i])->ChangeLineColor(argbLineColor);
         }
      }
      SaveAsAOF();
   }
}


void ASSISTANT::Page::ChangeFillColor(COLORREF clrFill, int iTransparency)
{
   CArray<DrawObject *, DrawObject *>arSelectedObjects;
   GetSelectedObjects(arSelectedObjects);

   if (arSelectedObjects.GetSize() == 0) 
      return;
   
   Gdiplus::ARGB argbFillColor = ASSISTANT::ColorrefToGdipARGB(clrFill, iTransparency);

   undoList->AppendAction(UndoAction::UNDOCHANGE, arSelectedObjects);
   
   for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
   {
      if (arSelectedObjects[i]->GetType() != GenericObject::IMAGE &&
          arSelectedObjects[i]->GetType() != GenericObject::LINE &&
          arSelectedObjects[i]->GetType() != GenericObject::POLYLINE &&
          arSelectedObjects[i]->GetType() != GenericObject::FREEHAND)
      {
         if (arSelectedObjects[i]->GetType() == GenericObject::GROUP) 
            ((Group*)arSelectedObjects[i])->ChangeFillColor(argbFillColor);
         else 
            ((ColoredObject*)arSelectedObjects[i])->ChangeFillColor(argbFillColor);
      }
   }
   SaveAsAOF();
}

void ASSISTANT::Page::ChangeTextColor(COLORREF clrText, int iTransparency)
{
   CArray<DrawObject *, DrawObject *>arSelectedObjects;
   GetSelectedObjects(arSelectedObjects);

   if (arSelectedObjects.GetSize() == 0) 
      return;
   
   bool bHasChangeableObjects = false;
   for (int i = 0; i < arSelectedObjects.GetSize() && !bHasChangeableObjects; ++i) 
   {
      if (arSelectedObjects[i]->GetType() == GenericObject::ASCIITEXT ||
          arSelectedObjects[i]->GetType() == GenericObject::GROUP)
      {
          bHasChangeableObjects = true;
      }
   }

   if (!bHasChangeableObjects)
      return;

   Gdiplus::ARGB argbTextColor = ASSISTANT::ColorrefToGdipARGB(clrText, iTransparency);
   
   undoList->AppendAction(UndoAction::UNDOCHANGE, arSelectedObjects);
   
   for (i = 0; i < arSelectedObjects.GetSize(); ++i) 
   {
      if (arSelectedObjects[i]->GetType() == GenericObject::ASCIITEXT ||
          arSelectedObjects[i]->GetType() == GenericObject::GROUP)
      {
         if (arSelectedObjects[i]->GetType() == GenericObject::GROUP) 
            ((Group*)arSelectedObjects[i])->ChangeLineColor(argbTextColor);
         else 
            ((ColoredObject*)arSelectedObjects[i])->ChangeLineColor(argbTextColor);
      }
   }
   SaveAsAOF();
}

void ASSISTANT::Page::ChangeFillColor()
{
   CArray<DrawObject *, DrawObject *>arSelectedObjects;
   GetSelectedObjects(arSelectedObjects);

   if (arSelectedObjects.GetSize() == 0) 
      return;
   
   Gdiplus::ARGB argbFillColor = 0;
   
   undoList->AppendAction(UndoAction::UNDOCHANGE, arSelectedObjects);
   
   for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
   {
      if (arSelectedObjects[i]->GetType() != GenericObject::IMAGE &&
          arSelectedObjects[i]->GetType() != GenericObject::LINE &&
          arSelectedObjects[i]->GetType() != GenericObject::POLYLINE &&
          arSelectedObjects[i]->GetType() != GenericObject::FREEHAND)
      {
         if (arSelectedObjects[i]->GetType() == GenericObject::GROUP) 
            ((Group*)arSelectedObjects[i])->ChangeFillColor(argbFillColor);
         else 
            ((ColoredObject*)arSelectedObjects[i])->ChangeFillColor(argbFillColor);
      }
   }
   SaveAsAOF();
}


void ASSISTANT::Page::ChangeLineWidth(int iLineWidth)
{
   CArray<DrawObject *, DrawObject *>arSelectedObjects;
   GetSelectedObjects(arSelectedObjects);

   if (arSelectedObjects.GetSize() == 0) 
      return;
   
   undoList->AppendAction(UndoAction::UNDOCHANGE, arSelectedObjects);
   for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
   {
      if ((arSelectedObjects[i]->GetType() != GenericObject::IMAGE) && 
         (arSelectedObjects[i]->GetType() != GenericObject::ASCIITEXT)) 
      {
         ((ColoredObject*)arSelectedObjects[i])->ChangeLineWidth(iLineWidth);
      }
   }
   SaveAsAOF();
}


void ASSISTANT::Page::ChangeLineStyle(int iLineStyle)
{
   CArray<DrawObject *, DrawObject *>arSelectedObjects;
   GetSelectedObjects(arSelectedObjects);

   if (arSelectedObjects.GetSize() == 0) 
      return;
   
   Gdiplus::DashStyle gdipLineStyle = Gdiplus::DashStyleSolid;
   if (iLineStyle == 1)
      gdipLineStyle = Gdiplus::DashStyleDot;
   else if (iLineStyle == 2)
      gdipLineStyle = Gdiplus::DashStyleDash;

   undoList->AppendAction(UndoAction::UNDOCHANGE, arSelectedObjects);
   for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
   {
      if ((arSelectedObjects[i]->GetType() != GenericObject::IMAGE) && 
         (arSelectedObjects[i]->GetType() != GenericObject::ASCIITEXT)) 
      { 
         if (arSelectedObjects[i]->GetType() == GenericObject::GROUP)
            ((Group*)arSelectedObjects[i])->ChangeLineStyle(gdipLineStyle);
         else 
            ((ColoredObject*)arSelectedObjects[i])->ChangeLineStyle(gdipLineStyle);
      }
   }
   SaveAsAOF();
}

void ASSISTANT::Page::ChangeArrowStyle(int iArrowConfig, LPCTSTR szArrowStyle)
{
   CArray<DrawObject *, DrawObject *>arSelectedObjects;
   GetSelectedObjects(arSelectedObjects);

   if (arSelectedObjects.GetSize() == 0) 
      return;
   
   undoList->AppendAction(UndoAction::UNDOCHANGE, arSelectedObjects);

   for (int i = 0; i < arSelectedObjects.GetSize(); ++i) 
   {
      if (arSelectedObjects[i]->GetType() == GenericObject::LINE) 
      {
         ((Line*)arSelectedObjects[i])->ChangeArrowStyle(szArrowStyle);
         ((Line*)arSelectedObjects[i])->ChangeArrowConfig(iArrowConfig);
      }
      else if (arSelectedObjects[i]->GetType() == GenericObject::POLYLINE) 
      {
         ((PolyLine*)arSelectedObjects[i])->ChangeArrowStyle(szArrowStyle);
         ((PolyLine*)arSelectedObjects[i])->ChangeArrowConfig(iArrowConfig);
      }
   }
   SaveAsAOF();
}

void ASSISTANT::Page::SetFullScreenOn(bool bState)
{
	bFullScreenOn = bState;
}

float ASSISTANT::Page::CalculateZoomFactor(CRect &rcWhiteboard)
{
   float fZoomFactor = 1.0;
   
   if (width > 0 && height > 0)
   {
	   float margin = (width > height)?(width/height):(height/width);
	   if(bFullScreenOn == true)
	   {
		   margin = 0;
	   }
	  float fZoomX = (float)(rcWhiteboard.Width() - (36 * margin)) / width;
      float fZoomY = (float)(rcWhiteboard.Height() - (36 * margin)) / height;
      
      if (fZoomX > fZoomY)
         fZoomFactor = fZoomY;
      else
         fZoomFactor = fZoomX;
   }

   return fZoomFactor;
}

void ASSISTANT::Page::CalculateOffset(CRect &rcWhiteboard, float fZoomFactor, UINT &nXOffset, UINT &nYOffset)
{
   nXOffset = 0;
   nYOffset = 0;
   if (width > 0 && height > 0)
   {
      nXOffset = (UINT)(rcWhiteboard.Width() - (width * fZoomFactor)) / 2;
      nYOffset = (UINT)(rcWhiteboard.Height() - (height * fZoomFactor)) / 2;
   }
}

void ASSISTANT::Page::RaiseObjects() 
{
   /*/
   if (objects.GetSize() == 0 || selectedObjects.empty()) return;
   
   for (int iter = selectedObjects.begin(); 
   iter != selectedObjects.end(); ++iter) 
   {
      activeObjects.remove(*iter);
      activeObjects.Add(*iter);
      if ((*iter)->GetType() == GenericObject::GROUP) 
      {
         DrawObject *obj = NULL;
         std::vector<uint32> *ids = ((Group*)*iter)->GetObjectIDs();
         for (std::vector<uint32>::iterator j = ids->begin(); j != ids->end(); ++j) 
         {
            obj = (DrawObject *)GetObject(*j);
            objects.remove(obj);
            objects.Add(obj);
         }	
         delete ids;
      }
      else 
      {
         objects.remove(*iter);
         objects.Add(*iter);
      }
   }
   
   /*/
   ReOrderObjects();
   
   SaveAsAOF();
}


void ASSISTANT::Page::LowerObjects() 
{
   /*/
   if (objects.GetSize() == 0 || selectedObjects.empty()) return;
   
   for (std::list<DrawObject*>::reverse_iterator iter = selectedObjects.rbegin(); iter != selectedObjects.rend(); ++iter) 
   {
      activeObjects.remove(*iter);
      activeObjects.push_front(*iter);
      if ((*iter)->GetType() == GenericObject::GROUP) {
  	      DrawObject* obj = NULL;
         std::vector<uint32>* ids = ((Group*)*iter)->GetObjectIDs();
  	      for (std::vector<uint32>::iterator j = ids->begin(); j != ids->end(); ++j) 
         {
            obj = (DrawObject*)(GetObject(*j));
            objects.remove(obj);
            objects.push_front(obj);
         }
  	      delete ids;
      }
      else 
      {
         objects.remove(*iter);
         objects.push_front(*iter);
      }
   } 
   
   /*/
   ReOrderObjects();
   
   SaveAsAOF();
}

void ASSISTANT::Page::InsertTextLine() 
{
   Group
      *group;
   
   if (objects.GetSize() == 0) return;
   
   if (active && (active->GetType() == ASSISTANT::GenericObject::ASCIITEXT)) 
   {
      if (active->GetGroup() && active->GetGroup()->GetType() == GenericObject::GROUP)
      {
         group = (Group *)active->GetGroup();
         //group->InsertNewLine(this);
      }
   }
}


void ASSISTANT::Page::InsertImage(_TCHAR *_filename, _TCHAR *path, int _x, int _y, bool _single)
{
   WindowsImage 
      *img;
   
   img = new WindowsImage(_x, _y, 0.0, 0.0, -1, _filename, Project::active->GetObjectID(), NULL, NULL, NULL); 
   if (img)
   {
      if (_single) {
         active = img;
      }
      
      //img->SetExtern(false);
      img->BuildImage(this, path, true);
      AppendObject(img, true, true);
      undoList->AppendAction(UndoAction::UNDOINSERT,img);
      SaveAsAOF();
   }
   
   return;
}


void ASSISTANT::Page::ActivateOnlinePointer(Gdiplus::PointF &ptObject)
{
   if (!onlinePointer)
      return;
   
   onlinePointer->Update(ptObject.X, ptObject.Y);
   onlinePointer->SetActive();
   onlinePointer->Hide();
   SaveAsAOF();
}
         
void ASSISTANT::Page::MoveOnlinePointer(Gdiplus::PointF &ptObject)
{
   if (!onlinePointer)
      return;
   
   onlinePointer->Update(ptObject.X, ptObject.Y);
   SaveAsAOF();
}
         
void ASSISTANT::Page::DrawOnlinePointer(CRect &rcWhiteboard, CDC *pDC)
{
   if (!onlinePointer)
      return;
   
   UINT nOffsetX = 0;
   UINT nOffsetY = 0;
   CalculateOffset(rcWhiteboard, zoomFactor_, nOffsetX, nOffsetY);
   onlinePointer->SetVisible();
   onlinePointer->Draw(pDC, zoomFactor_, nOffsetX, nOffsetY);
}
           
void ASSISTANT::Page::DeActivateOnlinePointer()
{
   if (!onlinePointer)
      return;
   
   if (onlinePointer && onlinePointer->IsActive())
   {
      onlinePointer->SetInactive();
      SaveAsAOF();
   }
}
        

/********************/
/***  END CHANGE  ***/
/********************/



/************/
/*** AOF  ***/
/************/


void ASSISTANT::Page::SaveAsAOF(int _time, bool doSave) 
{
   int
      time;
   
   if (doSave || 
       (!ASSISTANT::Project::active->IsSgOnlyDocument() && 
        ASSISTANT::Project::active->recording && 
        !ASSISTANT::Project::active->paused)) 
   {
      if (_time == -1) 
      {
         time = ASSISTANT::Project::active->GetRecordingTime();
      }
      else 
      {
         time = _time;
      }
      
      if (time < ASSISTANT::Project::active->lastRecordedTime_)
         return;
      
      ASSISTANT::Project::active->lastRecordedTime_ = time;
      
      CFileOutput *objFp = ASSISTANT::Project::active->GetObjFp();
      FILE *evqFp = ASSISTANT::Project::active->GetEvqFp();
      if ( evqFp == NULL )
         return;
      if (firstTime < 0) 
         firstTime = time;
      
      fprintf(evqFp,"%d %d 7 %d",time,pageNumber,RecordableObjects());
      WriteObjectFile();
      
      if (onlinePointer->IsActive()) 
      {
         onlinePointer->SetUnrecorded();
         onlinePointer->SaveAsAOF(objFp);
         fprintf(evqFp," %d",onlinePointer->GetRecordID());
      }
      
      fprintf(evqFp,"\n");
      fflush(evqFp);

      objFp->Flush();
   } 
}

int ASSISTANT::Page::RecordableObjects() 
{
   int
      ret;
   
   ret = 0;
   
   for (int i = 0; i < objects.GetSize(); ++i) 
   {
      ret += objects[i]->RecordableObjects();
   }
   
   if (onlinePointer && onlinePointer->IsActive()) 
   {
      ret++;
   }
   
   ret++;
   
   return ret;
}

void ASSISTANT::Page::WriteObjectFile()
{
   CFileOutput *objFp = ASSISTANT::Project::active->GetObjFp();
   FILE *evqFp = ASSISTANT::Project::active->GetEvqFp();
   
   backRect->SaveAsAOF(objFp);
   fprintf(evqFp," %d",backRect->GetRecordID());
   
   for (int i = 0; i < objects.GetSize(); ++i) 
   {
      if (objects[i]->IsVisible() && objects[i]->RecordableObjects() > 0) 
      {
         objects[i]->SaveAsAOF(objFp);
         fprintf(evqFp," %d",objects[i]->GetRecordID());
         
         // Ist das Objekt gefüllt? Wenn ja und es ist ein "normales" 
         // werden zwei Objekte in die Objektdatei geschrieben.
         // Besser wäre aber: Abfrage der "RecordableObjects" (wie unten)
         if ((objects[i]->GetType() != IMAGE) && (objects[i]->GetType() != GROUP)  && 
             (objects[i]->GetType() != ANIMATED) && (objects[i]->GetType() != POLYLINE) && 
             ((ColoredObject *)objects[i])->IsFilled())
            fprintf(evqFp," %d",objects[i]->GetRecordID()+1);
         
         if (objects[i]->GetType() == POLYLINE) 
         {
            if (objects[i]->RecordableObjects() == 2)
            {
               fprintf(evqFp," %d",objects[i]->GetRecordID()+1);
            }
            else if (objects[i]->RecordableObjects() == 3) 
            {
               fprintf(evqFp," %d",objects[i]->GetRecordID()+1);
               fprintf(evqFp," %d",objects[i]->GetRecordID()+2);
            }
         }
      }
   }
}

void ASSISTANT::Page::ResetRecording()
{
   // Turn off font smoothing temporarily. This is done because
   // WMF pictures are rendered against a white background, which
   // looks stupid if they are displayed later against a dark background
   // but with antialiased edges.
   BOOL bFontSmoothing = FALSE;
   ::SystemParametersInfo(SPI_GETFONTSMOOTHING, FALSE, &bFontSmoothing, 0);
   ::SystemParametersInfo(SPI_SETFONTSMOOTHING, FALSE, NULL, 0);
   
   backRect->SetUnrecorded();
   for (int i = 0; i < objects.GetSize(); ++i) 
   {
      objects[i]->SetUnrecorded();
   }
   
   ::SystemParametersInfo(SPI_SETFONTSMOOTHING, bFontSmoothing, NULL, 0);
   
   for (i = 0; i < activeObjects.GetSize(); ++i)
   {
      activeObjects[i]->SetUnrecorded();
   }
   
   firstTime = -1;
}

void ASSISTANT::Page::SavePointAsAOF(ASSISTANT::PolyObject *obj) 
{
   static int
      lastTime = 0;
   int
      time;
   
   if (!ASSISTANT::Project::active->IsSgOnlyDocument() && 
       ASSISTANT::Project::active->recording && 
       !ASSISTANT::Project::active->paused) 
   {
      time = ASSISTANT::Project::active->GetRecordingTime();
      if (time < lastTime) 
         lastTime = 0;
      if (time-lastTime < 40) 
         return;
      
      if (time < ASSISTANT::Project::active->lastRecordedTime_)
         return;
      
      ASSISTANT::Project::active->lastRecordedTime_ = time;
      
      lastTime = time;
      
      CFileOutput *objFp = ASSISTANT::Project::active->GetObjFp();
      FILE *evqFp = ASSISTANT::Project::active->GetEvqFp();
      
      
      if (obj->GetType() == GenericObject::MARKER)
      {
         obj->SetUnrecorded();
         SaveAsAOF();
      }
      else if (!objects.GetSize() == 0 && obj) 
      {
         if (obj->points.GetSize() > 0)
         {
            obj->SetInvisible();
            fprintf(evqFp,"%d %d 7 %d",time,pageNumber,RecordableObjects() + obj->points.GetSize());
            WriteObjectFile();
            obj->SetVisible();
            double dX = obj->points[0]->x;
            double dY = obj->points[0]->y;

            CString csColor;
            ASSISTANT::GdipARGBToObjString(obj->GetLineColor(), csColor);

            int iLineStyle = ASSISTANT::GdipLineStyleToObjStyle(obj->GetLineStyle());
            
            for (int i = 0; i < obj->points.GetSize(); ++i)
            {
               if (obj->points[i]->nr < 0) 
               {
                  obj->points[i]->nr = Project::active->recCount;
                  CString line;

                  line.Format(_T("<LINE x=%d y=%d width=%d height=%d xf=%2.2f yf=%2.2f widthf=%2.2f heightf=%2.2f linewidth=%d linestyle=%d ori=1 direction=0 rgb=0x%s></LINE>\n"),
                                  (int)dX, (int)dY, (int)(obj->points[i]->x-dX), (int)(obj->points[i]->y-dY),
                                  dX, dY, obj->points[i]->x-dX, obj->points[i]->y-dY,
                                  obj->GetLineWidth(), iLineStyle, csColor);
                  objFp->Append(line);
                  
                  Project::active->recCount++;
               }
               dX = obj->points[i]->x;
               dY = obj->points[i]->y;
               fprintf(evqFp," %d",obj->points[i]->nr);
            }
            fprintf(evqFp,"\n");
         }
      }
   } 
   else
      lastTime = 0;
}


/*****************/
/***  END AOF  ***/
/*****************/



/**************/
/***  SEND  ***/
/**************/


void ASSISTANT::Page::DeleteAll() 
{
   UnSelectObjects();
   
   for (int i = 0; i < subContainer.GetSize(); ++i)
   {
      subContainer[i]->DeleteAll(); // my DrawObjects are deleted in here
      delete  subContainer[i];
   }
   
   objects.RemoveAll();
   subContainer.RemoveAll();
   active = NULL;
   //GUI::Attributes::ResetAction();
}


/*****************/
/***  END SEND ***/
/*****************/


void ASSISTANT::Page::DeleteEmptyTextObjects() 
{
   
   Text
      *txt;
   Group
      *group;
   
   for (int i = 0; i < objects.GetSize(); ++i) 
   {
      if (objects[i]->GetType() == GenericObject::ASCIITEXT) 
      {
         txt   = (Text*)objects[i];
         if (txt->GetGroup() && txt->GetGroup()->GetType() == GenericObject::GROUP)
         {
            group = (Group*)txt->GetGroup();
            
            if (group && group->AllEmpty()) 
            {
               group->DeleteAll();
               group->Hide();
               //  	      group->DeleteTextCursor();
               //  	      DeleteObject(txt->GetID()); // delete empty text object
               //  	      group->DeleteAll();
               //  	      layer->DeleteObject(group->GetID()); // group is in layer only
               //  	      delete group;
               //  	      delete txt;
               //  	      i = objects.begin(); // reposition iterator cause object list has been changed
            }
         }
      }
   }
}

bool ASSISTANT::Page::ShowNextObject()
{
   int iClickId = -1;
   bool bHasAnimatedObject = false;
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if (activeObjects[i]->GetType() == GenericObject::ANIMATED && ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE) 
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

bool ASSISTANT::Page::HidePreviousObject()
{
   std::list<DrawObject*>::iterator 
      iter;
   
   int iClickId = -1;
   bool bHasAnimatedObject = false;
   for (int i = activeObjects.GetSize()-1; i >= 0; --i)
   {
      if (activeObjects[i]->GetType() == GenericObject::ANIMATED && ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE) 
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

bool ASSISTANT::Page::HasNextObject() {
    int iClickId = -1;
    bool bHasAnimatedObject = false;
    for (int i = 0; i < activeObjects.GetSize(); ++i) {
        if (activeObjects[i]->GetType() == GenericObject::ANIMATED &&
            ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE) {

            AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[i];
            if (pAnimatedObject->GetStatus() == AnimatedObject::PASSIV) {
                bHasAnimatedObject = true;
                break;
            }
        }
    }

    return bHasAnimatedObject;
}

bool ASSISTANT::Page::HasPreviousObject() {
    int iClickId = -1;
    bool bHasAnimatedObject = false;
    for (int i = activeObjects.GetSize()-1; i >= 0; --i) {
        if (activeObjects[i]->GetType() == GenericObject::ANIMATED && 
            ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE) {

            AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[i];
            if (pAnimatedObject->GetStatus() == AnimatedObject::ACTIVE) {
                bHasAnimatedObject = true;
                break;
            }
        }
    }

    return bHasAnimatedObject;
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
         if (activeObjects[i]->GetType() == GenericObject::ANIMATED && ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE)
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
         if (activeObjects[i]->GetType() == GenericObject::ANIMATED && ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE)
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
         if (activeObjects[i]->GetType() == GenericObject::ANIMATED && ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE)
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
         if (activeObjects[i]->GetType() == GenericObject::ANIMATED && ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE)
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
         if (activeObjects[i]->GetType() == GenericObject::ANIMATED && ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_OBJECT)
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
         if (activeObjects[i]->GetType() == GenericObject::ANIMATED && ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_OBJECT)
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
         if (activeObjects[i]->GetType() == GenericObject::ANIMATED && ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_OBJECT)
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
         if (activeObjects[i]->GetType() == GenericObject::ANIMATED && ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_OBJECT)
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
   for (int i = 0; i < _tcslen(objectIds); ++i)
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
         if (activeObjects[j]->GetType() == GenericObject::ANIMATED)
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
            if (activeObjects[j]->GetType() == GenericObject::ANIMATED)
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

bool ASSISTANT::Page::HasChanged()
{
   if (m_bContainerChanged) {
        return true;
   }

   if (undoList->Empty()) return false;
   
   return true;
}

void ASSISTANT::Page::SetChanged(bool changed)
{
    m_bContainerChanged = changed;

    if (!changed && !undoList->Empty()) {
        undoList->Clean();
    }
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


void ASSISTANT::Page::ResetFirstInserted()
{
   insertionCounter = 0;
}

void ASSISTANT::Page::CollectPages(CArray<Page *, Page *> &pageList)
{
   pageList.Add(this);
}

void ASSISTANT::Page::CallLink(DrawObject *object_, Gdiplus::PointF &ptMouse)
{
   CString documentName;
   CString subString;
   
   if (object_->HasObjectLinks(ptMouse))
   {
      ShowNextInteractiveObject(object_->GetInteractiveObjects(ptMouse), object_->GetPPTObjectId());
   }
   
   if (object_->HasHyperlink(ptMouse))
   {
      documentName = object_->GetHyperlink(ptMouse);
      if (!documentName.IsEmpty() && (object_->GetType() == GenericObject::ASCIITEXT || object_->GetType() == GenericObject::GROUP))
      {
         object_->ActivateHyperlink(ptMouse);
      }
      
      
      if (object_->IsInternLink())
      {
         Page *newPage = NULL;
         CString hyperlink = object_->GetHyperlink(ptMouse);
         if (hyperlink == _T("NEXT"))
         {
            bool found = false;
            if (Project::active && Project::active->GetActiveDocument())
                newPage = Project::active->GetActiveDocument()->GoToNextPage(NULL, &found, true);
         }
         else if (hyperlink == _T("PREV"))
         {
            bool found = false;
            if (Project::active && Project::active->GetActiveDocument())
                newPage = Project::active->GetActiveDocument()->GoToPrevPage(NULL, &found, true);
         }
         else if (hyperlink == _T("FIRST"))
         {
            if (Project::active && Project::active->GetActiveDocument())
                newPage = Project::active->GetActiveDocument()->GoToFirstPage();
         }
         else if (hyperlink == _T("LAST"))
         {
            if (Project::active && Project::active->GetActiveDocument())
                newPage = Project::active->GetActiveDocument()->GoToLastPage();
         }
         else
         {
            if (Project::active && Project::active->GetActiveDocument())
                newPage = Project::active->GetActiveDocument()->GoToPageWithID(hyperlink);
         }
         if (newPage == NULL)
         {
            CString csMessage;
            csMessage.LoadString(IDS_ERROR_ACTIVATE_INTERN_LINK);
            CString csCaption;
            csCaption.LoadString(IDS_WARNING);
            MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONWARNING | MB_TOPMOST);
            return;
         }
         if (Project::active)
             Project::active->ActivatePage();
      }
      else
      {
         CString linkDirectory;
         linkDirectory = object_->GetLinkDirectory(ptMouse);
         
         if (documentName.Find(_T("mailto:")) != 0 && documentName.Find(_T("http://")) != 0 &&
             documentName.Find(_T("ftp:")) != 0 && documentName.Find(_T("file:")) != 0)
         {
            bool absolutePath = false;
            // if documentName contains drive letter, it is an absolute path
            int pos = documentName.Find('\\');
            if (pos != -1)
            {
               CString firstSegment = documentName.Left(pos);
               pos = firstSegment.Find(':');
               if (pos != -1)
                  absolutePath = true;
            }
            // if documentName starts with '\\\\' (double backslash),
            // it's also an absolute path.
            if (!absolutePath && (documentName.Find(_T("\\\\")) == 0))
            {
               absolutePath = true;
            }
            
            if (!absolutePath)
            {
               int anzGoUp = 0;
               int nextPos = 0;
               nextPos = documentName.Find(_T("../"), 0);
               while (nextPos != -1)
               {
                  ++anzGoUp;
                  documentName = documentName.Right(documentName.GetLength() - nextPos+3);
                  nextPos = documentName.Find(_T("../"), 0);
               }
               
               nextPos = documentName.Find(_T("/"), 0);
               while (nextPos != -1)
               {
                  documentName.SetAt(nextPos, _T('\\'));
                  nextPos = documentName.Find(_T("/"), 0);
               }
               
               
               CString documentPath;
               for (int i = 0; i < anzGoUp; ++i)
               {
                  int pos = linkDirectory.ReverseFind(_T('\\'));
                  linkDirectory = linkDirectory.Left(pos);
               }
               
               documentPath = linkDirectory + _T("\\") + documentName;
               documentName = documentPath;
            }
         }
         
         
         // Reset the mouse cursor: This enables the wait cursor
         // and also fixes the problem that the hand cursor is
         // not reset if a new window is opened before the LeaveWidget
         // event is launched (see Canvas::ObjectIsHyperlink)
         //canvas->ResetHyperlinkCursor();
         
         HINSTANCE handle = ShellExecute(NULL, _T("open"), documentName, NULL, NULL, SW_SHOWNORMAL);
         
         if ((int)handle <= 32)
         {
            CString csMessage;
            csMessage.Format(IDS_ERROR_ACTIVATE_EXTERN_LINK, documentName);
            CString csCaption;
            csCaption.LoadString(IDS_WARNING);
            MessageBox(NULL, csMessage, csCaption, MB_OK | MB_ICONWARNING | MB_TOPMOST);
         }
      } 
   }
   
}


void ASSISTANT::Page::ResetInteractiveObjects()
{
   
   for (int i = 0; i < activeObjects.GetSize(); ++i) 
   {
      if (activeObjects[i]->GetType() == GenericObject::ANIMATED)
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
                  if (activeObjects[j]->GetType() == GenericObject::ANIMATED)
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
      if (activeObjects[i]->GetType() == GenericObject::GROUP)
      {

      }
   }

}

bool ASSISTANT::Page::RenameContainer(UINT nContainerId, LPCTSTR szContainerName)
{
   if (nContainerId == id)
   {
      containerName = szContainerName;
      return true;
   }

   return false;
}


void ASSISTANT::Page::TranslateFromScreenToObjectCoordinates(CRect &rcScreen, CRect &rcObjects)
{
   rcObjects.left = rcScreen.left / zoomFactor_;
   rcObjects.right = rcScreen.right / zoomFactor_;
   rcObjects.top = rcScreen.top / zoomFactor_;
   rcObjects.bottom = rcScreen.bottom / zoomFactor_;

   rcObjects.NormalizeRect();
}

ASSISTANT::DrawObject *ASSISTANT::Page::GetObject(Gdiplus::PointF &ptMouse)
{
   int iObjectIndex = -1;
  
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if (!((activeObjects[i]->GetType() == GenericObject::IMAGE && Project::active->DoFreezeImages()) ||
            (activeObjects[i]->IsMasterObject() && Project::active->DoFreezeMaster()) ||
            (activeObjects[i]->IsDocumentObject() && Project::active->DoFreezeDocument())) &&
          activeObjects[i]->ContainsPoint(ptMouse))
         iObjectIndex = i;
   }

   DrawObject *pObject = NULL;
   if (iObjectIndex != -1)
   {
      pObject = activeObjects[iObjectIndex];
   }

   return pObject;
}

bool ASSISTANT::Page::PointIsInObject(Gdiplus::PointF &ptMouse)
{
   int iObjectIndex = -1;
  
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if (!((activeObjects[i]->GetType() == GenericObject::IMAGE && Project::active->DoFreezeImages()) ||
            (activeObjects[i]->IsMasterObject() && Project::active->DoFreezeMaster()) ||
            (activeObjects[i]->IsDocumentObject() && Project::active->DoFreezeDocument())) &&
          activeObjects[i]->ContainsPoint(ptMouse))
         return true;
   }

   return false;
}

bool ASSISTANT::Page::IsEditablePoint(Gdiplus::PointF &ptObject, UINT &nEditType, int &iPointIndex)
{
   int iObjectIndex = -1;
  
   CArray<DrawObject *, DrawObject *>arSelectedObjects;
   GetSelectedObjects(arSelectedObjects);
   for (int i = 0; i < arSelectedObjects.GetSize(); ++i)
   {
      if (arSelectedObjects[i]->IsEditablePoint(ptObject, nEditType, iPointIndex, zoomFactor_))
         return true;
   }

   return false;
}

bool ASSISTANT::Page::IsHyperlinkPoint(Gdiplus::PointF &ptObject)
{ 
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if (activeObjects[i]->HasHyperlink(ptObject))
         return true;
   }

   return false;
}

void ASSISTANT::Page::ActivateHyperlink(Gdiplus::PointF &ptObject)
{
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if (activeObjects[i]->HasHyperlink(ptObject))
      {
         CallLink(activeObjects[i], ptObject);
      }
   }
}

void ASSISTANT::Page::ResizeObjects(float fDiffX,float fDiffY)
{

   CArray<DrawObject *, DrawObject *>arSelectedObjects;
   GetSelectedObjects(arSelectedObjects);
   
   if (m_iResizePoint != -1 && arSelectedObjects.GetSize() > 0) 
   {
      for (int i = 0; i < arSelectedObjects.GetSize(); ++i)
      {
         DrawObject *pDrawObject = arSelectedObjects[i];

         pDrawObject->Resize(fDiffX, fDiffY, m_iResizePoint);  
      }
   }
}

void ASSISTANT::Page::GetSelectedObjects(CArray<DrawObject *, DrawObject *> &arSelectedObjects)
{
   for (int i = 0; i < activeObjects.GetSize(); ++i)
   {
      if (activeObjects[i]->IsSelected())
         arSelectedObjects.Add(activeObjects[i]);
   }
}

void ASSISTANT::Page::AppendToUndoList(int action, CArray<DrawObject *, DrawObject *> &objects)
{
   if (undoList)
      undoList->AppendAction(action, objects);
}

bool ASSISTANT::Page::IsNextObjectAnimated() {
    bool bHasAnimatedObject = false;
    for (int i = 0; i < activeObjects.GetSize(); ++i) {
        if (activeObjects[i]->GetType() == GenericObject::ANIMATED && ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE) {
            AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[i];
            if (pAnimatedObject->GetStatus() == AnimatedObject::PASSIV) {
                bHasAnimatedObject = true;
                break;
            }
        }
    }
    return bHasAnimatedObject;
}

bool ASSISTANT::Page::IsPreviousObjectAnimated() {
    bool bHasAnimatedObject = false;
    for (int i = activeObjects.GetSize()-1; i >= 0; --i) {
        if (activeObjects[i]->GetType() == GenericObject::ANIMATED && ((AnimatedObject *)activeObjects[i])->GetTrigger() == AnimatedObject::CLICK_ON_PAGE) {
            AnimatedObject *pAnimatedObject = (AnimatedObject *)activeObjects[i];
            if (pAnimatedObject->GetStatus() == AnimatedObject::ACTIVE) {
                bHasAnimatedObject = true;
                break;
            }
        }
    }
    return bHasAnimatedObject;
}

// private functions

