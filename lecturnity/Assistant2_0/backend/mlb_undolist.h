/*********************************************************************

 program : mlb_undolist.h
 authors : Gabriela Maass
 date    : 7.05.2001
 desc    : 

 **********************************************************************/

#ifndef __ASSISTANT_UNDOLIST__
#define __ASSISTANT_UNDOLIST__

#ifdef WIN32
#  pragma warning (disable: 4786)
#endif

#include "mlb_objects.h"


namespace ASSISTANT {
   
   class UndoAction
   {
   protected:
      CArray<DrawObject *, DrawObject *>
         objects;
      int
         timeStamp_;
   public:
      enum action {
         UNDODELETE = 0,
            UNDOINSERT = 1,
            UNDOCHANGE = 2
      };
      
      UndoAction();
      virtual ~UndoAction();
      
      virtual int GetType() = 0;
      virtual void AppendObject(DrawObject *_obj, Page *_page=NULL);
      virtual void Undo() = 0;
      void SetTimeStamp(int timeStamp) {timeStamp_ = timeStamp;}
      int GetTimeStamp() {return timeStamp_;}
   };
   
   class DeleteAction : public UndoAction
   {
   public:
      DeleteAction();
      virtual ~DeleteAction();
      
      virtual int GetType() {return ASSISTANT::UndoAction::UNDODELETE;}
      
      void Undo();
   };
   
   class InsertAction : public UndoAction
   {
   public:
      InsertAction();
      virtual ~InsertAction();
      
      virtual int GetType() {return ASSISTANT::UndoAction::UNDOINSERT;}
      
      void Undo();
   };
   
   class ChangeAction : public UndoAction
   {
   public:
      ChangeAction();
      virtual ~ChangeAction();
      
      virtual int GetType() {return ASSISTANT::UndoAction::UNDOCHANGE;}
      
      virtual void AppendObject(DrawObject *_obj, Page *_page=NULL);
      
      void Undo();
   };
   
   
   class UndoList 
   {
   protected:
      CArray<UndoAction *, UndoAction *>
         actions;
      
   public:
      
      
      UndoList();
      ~UndoList();
      
      void AppendAction(int _action, CArray<DrawObject *, DrawObject *> &_objects);
      void AppendAction(int _action, DrawObject *_obj);
      void UndoLastAction();
      int GetLastUndoTimestamp();
      bool Empty();
      void Clean();
      
      
   };
   
}

#endif __ASSISTANT_UNDOLIST__
