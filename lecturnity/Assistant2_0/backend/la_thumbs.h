/*********************************************************************

 program  : la_thumbs.h
 authors  : Gabriela Maass
 date     : 08.01.2002
 revision : $Id: la_thumbs.h,v 1.9 2010-07-01 07:21:31 maass Exp $
 desc     : 

 **********************************************************************/

#ifndef __LA_THUMBS__
#define __LA_THUMBS__

#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <vector>

#ifndef WIN32
#  include <unistd.h>
#else
#  pragma warning (disable: 4786)
#  include <windows.h>
#endif
 
#include "mlb_generic.h"
#include "la_cliplist.h"
#include "LmdSgml.h"

namespace PLAYER
{ 

#define NEXT 0
#define SON 1

   class ThumbElement
   {
      protected:
         ThumbElement 
            *parent_;
         ASSISTANT::GenericContainer
            *container_;
         int
            number_;
      public:
         ThumbElement();
         virtual ~ThumbElement();

         void SetParent(ThumbElement *parent) {parent_ = parent;}
         ThumbElement *GetParent() {return parent_;}
         void SetContainer(ASSISTANT::GenericContainer *container) {container_ = container;}
         ASSISTANT::GenericContainer *GetContainer() {return container_;}
         virtual void WriteEntry(CFileOutput *lmdOutput, int level, ASSISTANT::ClipList *clipList, bool searchForClips=false) = 0;
         virtual void GetSlides(CArray<ThumbElement *, ThumbElement *> &aSlidesArray) = 0;
         virtual void Clear() = 0;
         virtual long GetEnd() = 0;
         virtual long GetBegin() = 0;
         int GetNumber() {return number_;}
         void SetNumber(int number) {number_ = number;}
         virtual int GetElementCount() = 0;

   };
   
   class ThumbPage: public ThumbElement
   {
      private:
         CString title_;
         long begin_;
         long end_;
         CString imageName_;
         CString keywords_;
      public:
         ThumbPage(ASSISTANT::GenericContainer *container, const _TCHAR *prefix, int time);
         ThumbPage(SgmlElement *pPageNode);
         virtual ~ThumbPage();

         virtual void WriteEntry(CFileOutput *lmdOutput, int level, ASSISTANT::ClipList *clipList, bool searchForClips=false);
         virtual void GetSlides(CArray<ThumbElement *, ThumbElement *> &aSlidesArray);
         virtual void Clear();
         void SetEnd(long time) {end_ = time;}
         long GetEnd() {return end_;}
         long GetBegin() {return begin_;}
         virtual int GetElementCount() {return 1;}
         CString &GetTitle() {return title_;}
         CString &GetKeywords() {return keywords_;}
   };

   class ThumbChapter : public ThumbElement
   {
      private:
         CArray<ThumbElement *, ThumbElement *> 
            elements;
         CString 
            title_;
         int 
            lastNumber;

      public:
         ThumbChapter();
         ThumbChapter(ASSISTANT::GenericContainer *container);
         ThumbChapter(SgmlElement *pChapterNode);
         virtual ~ThumbChapter();

         void Append(ThumbElement *element);
         ThumbChapter *Insert(int anz, ThumbPage *page);
         void Insert(ThumbPage *page);
         virtual void WriteEntry(CFileOutput *lmdOutput, int level, ASSISTANT::ClipList *clipList, bool searchForClips);
         virtual void GetSlides(CArray<ThumbElement *, ThumbElement *> &aSlidesArray);
         void WriteRootEntry(CFileOutput *lmdOutput, ASSISTANT::ClipList *clipList);
         virtual void Clear();
         long GetEnd();
         long GetBegin();
         virtual int GetElementCount();
   };


   class ThumbApplication : public ThumbElement
   {
      private:
         CString description;
         CString command;
         long begin;
         long end;
      public:
         ThumbApplication(int time);
         virtual ~ThumbApplication() {}
   };


   class Thumbs 
   {
      private:
         ThumbChapter
            *root_;
         ThumbPage
            *actual_;
         CString 
            prefix_,
            path_,   
            fileName_;

      public:
         Thumbs(bool bCreateRoot=true);
         Thumbs(CLmdSgml *pLmdSgml);
         virtual ~Thumbs();
         void AddElement(ASSISTANT::GenericContainer *container, int time);
         void CloseElement(ThumbPage *closeElement);
         ThumbChapter *FindLastCommonChapter(ASSISTANT::GenericContainer *page, int *anz);
         void SetName(LPCTSTR name, LPCTSTR path);
         void CloseThumbList(int _time);
         void Clear();
         void GetMetadata(LPCTSTR title, LPCTSTR author, LPCTSTR creator, LPCTSTR keywords);
         void WriteRootEntry(CFileOutput *lmdOutput, ASSISTANT::ClipList *clipList);
         void GetSlides(CArray<ThumbElement *, ThumbElement *> &aSlidesArray);
         ThumbChapter *GetRoot() {return root_;}
         void SetRoot(ThumbChapter *newRoot) {root_ = newRoot; actual_ = NULL;}
         virtual int GetElementCount();

   };
}

#endif