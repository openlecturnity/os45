#ifndef __ASSISTANT_CLIPLIST__
#define __ASSISTANT_CLIPLIST__

#include "FileOutput.h"

namespace ASSISTANT {

   class SgClip
   {
   private:
      CString
         fileName_;
      int
         timeStamp_;
      int 
         clipNumber_;
      CString
         clipTitle_;
      CStringArray
         keywords_;
      int 
         clipLength_;

   public:
      SgClip();
      SgClip(const _TCHAR *fileName, const _TCHAR *clipTitle);
      ~SgClip();

      // returns filename of clip
      CString &GetFileName();
      void SetFilename(const _TCHAR *filename);
      void SetTitle(const _TCHAR *title);
      CString &GetTitle();
      void SetStartTime(int timeStamp);
      int GetStartTime();
      void SetLength(int length);
      int GetLength();
      void SetClipNumber(int number);
      int GetClipNumber();
      void SetKeywords(CString &keywords);
      void AddKeyword(const _TCHAR *keyword);
      CString &GetKeywords();


      // write clip entry to lrd file
      void WriteToLrd(CFileOutput *lrdOutput);
      // write clip entry to lmd file if it is in range from [from, to]
      void WriteLmdEntry(CFileOutput *lmdOutput, int from, int to, int level);
      // write clip entry to lmd file
      void WriteLmdEntry(CFileOutput *lmdOutput);
   };

   class ClipList
   {
   private:
      CArray<SgClip *, SgClip *> m_aClips;
      int
         clipCount_;
   public:
      ClipList();
      ~ClipList();

      void RemoveElements();
      // create new sg-clip and returns the clip-filename
      LPCTSTR AddClip(LPCTSTR projectPrefix, int time);
      int GetNextClipNumber();
      void AddClip(SgClip *newClip); 
      void InsertClip(SgClip *newClip); 
      void RemoveClip(ASSISTANT::SgClip *remClip);

      // write sg entries to lrd-File
      void WriteToLrd(CFileOutput *lrdOutput);

      // return if cliplist is empty or not
      bool Empty();

      // delete all recorded clips and reset clip counter
      void Reset();

      // return last inserted clip
      SgClip *GetActualClip();
      SgClip *GetClipStartAt(int msec);
      SgClip *GetClip(int clipNumber);
      SgClip *GetClip(LPCTSTR clipName, int startAt);

      void WriteLmdEntry(CFileOutput *lmdOutput, int from, int to, int level);
      void WriteLmdEntry(CFileOutput *lmdOutput);

      void GetTclString(CString &tclString);
      void ReturnOrderedList(ClipList *orderedList);
   };
}

#endif