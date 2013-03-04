/*********************************************************************

 program : mlb_undolist.cpp
 authors : Gabriela Maass
 date    : 7.05.2001
 desc    : 

 **********************************************************************/
#include "StdAfx.h"

/*
#include <string.h>
*/
#include "mlb_undolist.h"
#include "la_project.h" // In StdAfx.h

ASSISTANT::UndoAction::UndoAction()
{
}

ASSISTANT::UndoAction::~UndoAction()
{
}

void ASSISTANT::UndoAction::AppendObject(DrawObject *_obj, Page *_page)
{
  objects.Add(_obj);
}

ASSISTANT::DeleteAction::DeleteAction()
{
}

ASSISTANT::DeleteAction::~DeleteAction()
{
  objects.RemoveAll();
}

void ASSISTANT::DeleteAction::Undo()
{
  
  for (int i = 0; i < objects.GetSize(); ++i)
    objects[i]->SetVisible();
}

ASSISTANT::InsertAction::InsertAction()
{
}

ASSISTANT::InsertAction::~InsertAction()
{
  objects.RemoveAll();
}

void ASSISTANT::InsertAction::Undo()
{
  for (int i = 0; i < objects.GetSize(); ++i)
    objects[i]->Hide();
 
}


ASSISTANT::ChangeAction::ChangeAction()
{
}

ASSISTANT::ChangeAction::~ChangeAction()
{
   for (int i = 0; i < objects.GetSize(); ++i) 
   {
      if (objects[i])
      {
         if (objects[i]->GetType() == GenericObject::GROUP)
         {
            Group *pGroup = (Group *)objects[i];
            for (int j = 0; j < pGroup->groupObjects.GetSize(); ++j)
            {
               if (pGroup->groupObjects[j])
                  delete pGroup->groupObjects[j];
            }
         }
         delete objects[i];
      }
   }

  objects.RemoveAll();
}

void ASSISTANT::ChangeAction::Undo()
{
  DrawObject
    *obj;
	float
    diffX, diffY;
   bool
      dimensionChanged;
   float
      zoomFactor;

   if (!ASSISTANT::Project::active)
      return;

   if (!ASSISTANT::Project::active->GetActivePage())
      return;

   zoomFactor = Project::active->GetActivePage()->GetZoomFactor();

  for (int i = 0; i < objects.GetSize(); ++i) 
  {
    obj = (DrawObject *)Project::active->GetActivePage()->GetObject(objects[i]->GetID());
    if (!obj) return;

    if (obj->GetType() == GenericObject::GROUP) {
      ((Group *)obj)->TakeParameter(objects[i]);
      obj->SetUnrecorded();
      continue;
    }
    if (obj->GetType() == GenericObject::ANIMATED) {
      ((AnimatedObject *)obj)->TakeParameter(objects[i]);
      obj->SetUnrecorded();
      continue;
    }
	  if ((obj->GetX() != objects[i]->GetX()) || (obj->GetY() != objects[i]->GetY())) {
	    diffX = (objects[i]->GetX() - obj->GetX());
	    diffY = (objects[i]->GetY() - obj->GetY());
	    obj->Move(diffX,diffY);
	  }
     dimensionChanged = false;
	  if (obj->GetWidth() != objects[i]->GetWidth()) {
	    obj->SetWidth(objects[i]->GetWidth());
       dimensionChanged = true;
	  }
	  if (obj->GetHeight() != objects[i]->GetHeight()) {
	    obj->SetHeight(objects[i]->GetHeight());
       dimensionChanged = true;
	  }
     if ((obj->GetType() == GenericObject::IMAGE) && dimensionChanged)
     {
        ((WindowsImage *)obj)->Resize(obj->GetWidth(),obj->GetHeight());
	  }
	  if (obj->GetType() != GenericObject::IMAGE) {
	    if (((ColoredObject *)obj)->GetLineColor() != ((ColoredObject *)objects[i])->GetLineColor()) {
	      ((ColoredObject *)obj)->ChangeLineColor(((ColoredObject *)objects[i])->GetLineColor());
	    }
	    if (((ColoredObject *)obj)->GetFillColor() != ((ColoredObject *)objects[i])->GetFillColor()) {
	      ((ColoredObject *)obj)->ChangeFillColor(((ColoredObject *)objects[i])->GetFillColor());
	    }
	    if (((ColoredObject *)obj)->GetLineWidth() != ((ColoredObject *)objects[i])->GetLineWidth()) {
	      ((ColoredObject *)obj)->ChangeLineWidth(((ColoredObject *)objects[i])->GetLineWidth());
	    }
	    if (((ColoredObject *)obj)->GetLineStyle() != ((ColoredObject *)objects[i])->GetLineStyle()) {
	      ((ColoredObject *)obj)->ChangeLineStyle(((ColoredObject *)objects[i])->GetLineStyle());
	    }
	  }
     if (obj->GetType() == GenericObject::LINE) {
	    if (((Line *)obj)->GetArrowStyle() != ((Line *)objects[i])->GetArrowStyle()) {
	      ((Line *)obj)->ChangeArrowStyle(((Line *)objects[i])->GetArrowStyle());
	    }
	    if (((Line *)obj)->GetArrowConfig() != ((Line *)objects[i])->GetArrowConfig()) {
	      ((Line *)obj)->ChangeArrowConfig(((Line *)objects[i])->GetArrowConfig());
	    }
	  }
	  if ((obj->GetType() == GenericObject::POLYLINE) || (obj->GetType() == GenericObject::FREEHAND)) {
	    
	    if (((PolyLine *)obj)->GetArrowStyle() != ((PolyLine *)objects[i])->GetArrowStyle()) {
	      ((PolyLine *)obj)->ChangeArrowStyle(((PolyLine *)objects[i])->GetArrowStyle());
	    }
	    if (((PolyLine *)obj)->GetArrowConfig() != ((PolyLine *)objects[i])->GetArrowConfig()) {
	      ((PolyLine *)obj)->ChangeArrowConfig(((PolyLine *)objects[i])->GetArrowConfig());
	    }
	  }
    if (obj->GetType() == GenericObject::ASCIITEXT) {
      if (_tcscmp(((Text *)obj)->GetString(), ((Text *)objects[i])->GetString()) == 0) {
        ((Text *)obj)->ChangeString(((Text *)objects[i])->GetString());
      }
    }
	  obj->SetUnrecorded();
  }
}

void ASSISTANT::ChangeAction::AppendObject(DrawObject *_obj, Page *_page)
{
   DrawObject
      *obj;
   
   obj = _obj->Copy();
   obj->SetPage(_obj->GetPage());
   obj->SetID(_obj->GetID());
   if (obj->GetType() == GenericObject::GROUP) 
   {
      ((Group *)obj)->TakeIds(_obj);
   }
   if (obj->GetType() == GenericObject::ANIMATED) 
   {
      ((AnimatedObject *)obj)->TakeIds(_obj);
   }
   objects.Add(obj);
}


ASSISTANT::UndoList::UndoList()
{
}

ASSISTANT::UndoList::~UndoList()
{
  for (int i = 0; i < actions.GetSize(); ++i) 
  {
    delete actions[i];
  }
  actions.RemoveAll();
}

void ASSISTANT::UndoList::AppendAction(int _action, CArray<DrawObject *, DrawObject *> &_objects)
{
   UndoAction
      *action;
   
   if (_objects.GetSize() == 0) return;
   
   int i = 0;
   switch (_action) {
   case UndoAction::UNDODELETE:
      action = new DeleteAction();
      for (i =0; i < _objects.GetSize(); ++i)
      {
         action->AppendObject(_objects[i]);
      }
      actions.Add(action);
      break;
   case UndoAction::UNDOINSERT:
      action = new InsertAction();
      for (i =0; i < _objects.GetSize(); ++i) {
         action->AppendObject(_objects[i]);
      }
      actions.Add(action);
      break;
   case UndoAction::UNDOCHANGE:
      action = new ChangeAction();
      for (i =0; i < _objects.GetSize(); ++i) {
         action->AppendObject(_objects[i]);
      }
      actions.Add(action);
      break;
   }

   if (action)
   {
      int timeStamp = ASSISTANT::Project::active->GetRecordingTime();
      action->SetTimeStamp(timeStamp);
   }
}

void ASSISTANT::UndoList::AppendAction(int _action, DrawObject *_obj)
{
   UndoAction
      *action;
   
   switch (_action) {
   case UndoAction::UNDODELETE:
      action = new DeleteAction();
      action->AppendObject(_obj);
      actions.Add(action);
      break;
   case UndoAction::UNDOINSERT:
      action = new InsertAction();
      action->AppendObject(_obj);
      actions.Add(action);
      break;
   case UndoAction::UNDOCHANGE:
      action = new ChangeAction();
      action->AppendObject(_obj);
      actions.Add(action);
      break;
   }
   
   if (action)
   {
      int timeStamp = ASSISTANT::Project::active->GetRecordingTime();
      action->SetTimeStamp(timeStamp);
   }
}

void ASSISTANT::UndoList::UndoLastAction()
{
  UndoAction
    *action;

  if (actions.GetSize() == 0) return;

  action = actions[actions.GetSize()-1];
  if (action) {
    actions.RemoveAt(actions.GetSize()-1);
    action->Undo();
    delete action;
  }
  
}

int ASSISTANT::UndoList::GetLastUndoTimestamp()
{

   int timeStamp = -1;
   
   if (actions.GetSize() > 0)
   {
      UndoAction *lastAction = actions[actions.GetSize()-1];
      if (lastAction)
         timeStamp = lastAction->GetTimeStamp();
   }

   return timeStamp;
}

void ASSISTANT::UndoList::Clean()
{
  for (int i = 0; i < actions.GetSize(); ++i)
  {
    delete actions[i];
  }
  actions.RemoveAll();
}

bool ASSISTANT::UndoList::Empty()
{
   if (actions.GetSize() == 0) 
      return true;

   return false;
}
