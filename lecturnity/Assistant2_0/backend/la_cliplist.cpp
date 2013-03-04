#include "StdAfx.h"

#pragma warning (disable: 4786)
#include "la_cliplist.h"
#include "mlb_misc.h"
#include "MiscFunctions.h"

ASSISTANT::SgClip::SgClip()
{
   fileName_ = _T("");
   timeStamp_ = -1;
   clipTitle_ = _T("");
   clipLength_ = 0;
   clipNumber_ = -1;
}

ASSISTANT::SgClip::SgClip(const _TCHAR *fileName, const _TCHAR *clipTitle)
{
   fileName_ = fileName;
   timeStamp_ = -1;
   clipTitle_ = clipTitle;
   clipLength_ = 0;
   clipNumber_ = -1;
}

ASSISTANT::SgClip::~SgClip()
{
   fileName_.Empty();
   clipTitle_.Empty();
   keywords_.RemoveAll();
}

CString &ASSISTANT::SgClip::GetFileName()
{
   return fileName_;
}

void ASSISTANT::SgClip::SetFilename(const _TCHAR *filename)
{
   fileName_ = filename;
}

void ASSISTANT::SgClip::SetTitle(const _TCHAR *title)
{
   clipTitle_ = title;
}
    
CString &ASSISTANT::SgClip::GetTitle()
{
   return clipTitle_;
}
      
void ASSISTANT::SgClip::SetStartTime(int timeStamp)
{
   timeStamp_ = timeStamp;
}

int ASSISTANT::SgClip::GetStartTime()
{
   return timeStamp_;
}

void ASSISTANT::SgClip::SetLength(int clipLength)
{
   clipLength_ = clipLength;
}

int ASSISTANT::SgClip::GetLength()
{
   return clipLength_;
}

void ASSISTANT::SgClip::SetClipNumber(int number)
{
   clipNumber_ = number;
}
      
int ASSISTANT::SgClip::GetClipNumber()
{
   return clipNumber_;
}

void ASSISTANT::SgClip::SetKeywords(CString &keywords)
{ 
   keywords_.RemoveAll();
   CreateListFromString(keywords, keywords_);
}
     
void ASSISTANT::SgClip::AddKeyword(const _TCHAR *keyword)
{
   keywords_.Add(keyword);
}
     
CString &ASSISTANT::SgClip::GetKeywords()
{
   static CString csKeywords;

   for (int i =0; i < keywords_.GetSize(); ++i)
   {
      if (i < 0)
         csKeywords += _T("; ");
      csKeywords += keywords_[i];
   }
   return csKeywords;
}

void ASSISTANT::SgClip::WriteToLrd(CFileOutput *lrdOutput)
{
   CString csSgmlName;
   StringManipulation::TransformForSgml(fileName_, csSgmlName);
   
   CString line;
   line.Format(_T("        <OPT>%s</OPT>\n"), csSgmlName);
   lrdOutput->Append(line);
   line.Format(_T("        <OPT>%d</OPT>\n"), timeStamp_);
   lrdOutput->Append(line);
}
      
void ASSISTANT::SgClip::WriteLmdEntry(CFileOutput *lmdOutput, int from, int to, int level)
{ 
   if ((from != -1) && (from > timeStamp_))
      return;
   if ((to != -1) && (to < timeStamp_))
      return;

   CString csLine;
   CString csSgmlLine;

   CString csTab;
   for (int i=0; i<level; i++) 
      csTab += _T("  ");

   csLine.Format(_T("%s<clip>\n"), csTab);
   lmdOutput->Append(csLine);

   csLine = clipTitle_;
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   csLine.Format(_T("%s  <title>%s</title>\n"), csTab, csSgmlLine);
   lmdOutput->Append(csLine);

   csLine.Format(_T("%s  <nr>%d</nr>\n"), csTab, clipNumber_);
   lmdOutput->Append(csLine);

   csLine.Format(_T("%s  <begin>%d</begin>\n"), csTab, timeStamp_);
   lmdOutput->Append(csLine);

   csLine.Format(_T("%s  <end>%d</end>\n"), csTab, timeStamp_ + clipLength_);
   lmdOutput->Append(csLine);

   csLine = fileName_;
   ASSISTANT::GetPrefix(csLine);
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   // remove any filename reserved characters from the filename
   CString thumbFile = csSgmlLine;
   StringManipulation::RemoveReservedFilenameChars(thumbFile);
   thumbFile.Replace(_T(" "), _T("_"));
   csLine.Format(_T("%s  <thumb>%s.png</thumb>\n"), csTab, thumbFile);
   lmdOutput->Append(csLine);

   for (i = 0; i != keywords_.GetSize(); ++i)
   {
      csLine = keywords_[i];
      StringManipulation::TransformForSgml(csLine, csSgmlLine);
      csLine.Format(_T("%s  <keyword>%s</keyword>\n"), csTab, csSgmlLine);
      lmdOutput->Append(csLine);
   }

   csLine.Format(_T("%s</clip>\n"), csTab);
   lmdOutput->Append(csLine);

}
    
void ASSISTANT::SgClip::WriteLmdEntry(CFileOutput *lmdOutput)
{ 
   CString csLine;
   CString csSgmlLine;

   csLine = _T("      <clip>\n");
   lmdOutput->Append(csLine);

   csLine = clipTitle_;
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   csLine.Format(_T("        <title>%s</title>\n"), csSgmlLine);
   lmdOutput->Append(csLine);

   csLine.Format(_T("         <nr>%d</nr>\n"), clipNumber_);
   lmdOutput->Append(csLine);

   csLine.Format(_T("         <begin>%d</begin>\n"), timeStamp_);
   lmdOutput->Append(csLine);

   csLine.Format(_T("         <end>%d</end>\n"), timeStamp_ + clipLength_);
   lmdOutput->Append(csLine);


   csLine = fileName_;
   ASSISTANT::GetPrefix(csLine);
   StringManipulation::TransformForSgml(csLine, csSgmlLine);
   // remove any filename reserved characters from the filename
   CString thumbFile = csSgmlLine;
   StringManipulation::RemoveReservedFilenameChars(thumbFile);
   thumbFile.Replace(_T(" "), _T("_"));
   csLine.Format(_T("         <thumb>%s.png</thumb>\n"), thumbFile);
   lmdOutput->Append(csLine);

   for (int i = 0; i < keywords_.GetSize(); ++i)
   {
      csLine = keywords_[i];
      StringManipulation::TransformForSgml(csLine, csSgmlLine);
      csLine.Format(_T("        <keyword>%s</keyword>\n"), csSgmlLine);
      lmdOutput->Append(csLine);
   }

   csLine = _T("       </clip>\n");
   lmdOutput->Append(csLine);

}

//////////////////////////////////

ASSISTANT::ClipList::ClipList()
{
   m_aClips.RemoveAll();
   clipCount_ = 0;
}

ASSISTANT::ClipList::~ClipList()
{
   for (int i = 0; i < m_aClips.GetSize(); ++i)
   {
      if (m_aClips[i] != NULL)
         delete m_aClips[i];
   }
   m_aClips.RemoveAll();
   clipCount_ = 0;
}

void ASSISTANT::ClipList::RemoveElements()
{
   m_aClips.RemoveAll();
   clipCount_ = 0;
}

LPCTSTR ASSISTANT::ClipList::AddClip(LPCTSTR projectPrefix, int time)
{
   _TCHAR
      strClipNumber[20];

   int iClipNumber = GetNextClipNumber();

   _itot(iClipNumber, strClipNumber, 10);

   CString csClip = _T("Clip_");
   csClip += strClipNumber;

   CString csClipFile = projectPrefix;
   csClipFile.Replace(_T(" "), _T("_"));
   csClipFile += _T("_clip_");
   csClipFile += strClipNumber;
   csClipFile += _T(".avi");

   SgClip *newClip = new SgClip(csClipFile, csClip);
   newClip->SetClipNumber(iClipNumber);
   newClip->SetStartTime(time);

   m_aClips.Add(newClip);

   ++clipCount_;

   return newClip->GetFileName();
}

int ASSISTANT::ClipList::GetNextClipNumber()
{
   int newClipNumber = -1;

   for (int i = 0; i < m_aClips.GetSize(); ++i)
   {
      if (m_aClips[i] != NULL &&
          m_aClips[i]->GetClipNumber() > newClipNumber)
         newClipNumber =  m_aClips[i]->GetClipNumber();
   }

   if (newClipNumber == -1)
      newClipNumber = clipCount_;
   else
      ++newClipNumber;

   return newClipNumber;
}

void ASSISTANT::ClipList::AddClip(ASSISTANT::SgClip *newClip)
{
   m_aClips.Add(newClip);

   ++clipCount_;

   return;
}

void ASSISTANT::ClipList::InsertClip(ASSISTANT::SgClip *newClip)
{
   if (newClip->GetClipNumber() == -1)
   {
      newClip->SetClipNumber(GetNextClipNumber());
   }

   if (_tcslen(newClip->GetTitle()) <= 0)
   {
      CString csTitle;
      csTitle.Format(_T("Clip %d"), newClip->GetClipNumber());
      newClip->SetTitle(csTitle);
   }

   for (int i = 0; i < m_aClips.GetSize(); ++i)
   {
      if (m_aClips[i] != NULL &&
          m_aClips[i]->GetStartTime() > newClip->GetStartTime())
      {
         m_aClips.InsertAt(i, newClip);
         ++clipCount_;
         return;
      }
   }

   m_aClips.Add(newClip);
   ++clipCount_;
   return;
}


void ASSISTANT::ClipList::RemoveClip(ASSISTANT::SgClip *remClip)
{
   for (int i = 0; i < m_aClips.GetSize(); ++i)
   {
      if (remClip == m_aClips[i])
      {
         m_aClips.RemoveAt(i);
         --clipCount_;
         return;
      }
   }

   return;
}

void ASSISTANT::ClipList::WriteToLrd(CFileOutput *lrdOutput)
{
   for (int i = 0; i < m_aClips.GetSize(); ++i)
   {
      if (m_aClips[i] != NULL)
         m_aClips[i]->WriteToLrd(lrdOutput);
   }
}

bool ASSISTANT::ClipList::Empty()
{
   return m_aClips.GetSize() == 0;
}

void ASSISTANT::ClipList::Reset()
{
   for (int i = 0; i < m_aClips.GetSize(); ++i)
   {
      if (m_aClips[i] != NULL)
         delete m_aClips[i];
   }
   m_aClips.RemoveAll();

   clipCount_ = 0;
}

ASSISTANT::SgClip *ASSISTANT::ClipList::GetActualClip()
{
   if (m_aClips.GetSize() == 0)
      return NULL;
   else 
      return (m_aClips[m_aClips.GetSize() -1]);
}

ASSISTANT::SgClip *ASSISTANT::ClipList::GetClipStartAt(int msec)
{
   for (int i = 0; i < m_aClips.GetSize(); ++i)
   {
      if (m_aClips[i] != NULL && 
          m_aClips[i]->GetStartTime() == msec)
         return m_aClips[i];
   }

   return NULL;
}


ASSISTANT::SgClip *ASSISTANT::ClipList::GetClip(int clipNumber)
{
   for (int i = 0; i < m_aClips.GetSize(); ++i)
   {
      if (m_aClips[i] != NULL &&
          m_aClips[i]->GetClipNumber() == clipNumber)
         return m_aClips[i];
   }

   return NULL;
}


ASSISTANT::SgClip *ASSISTANT::ClipList::GetClip(LPCTSTR clipName, int startAt)
{
   for (int i = 0; i < m_aClips.GetSize(); ++i)
   {
      if (m_aClips[i] != NULL)
      {
         CString csFile = m_aClips[i]->GetFileName();
         csFile += _T(".avi");
         if (csFile == clipName)
            return m_aClips[i];
      }
   }

   return NULL;
}

void ASSISTANT::ClipList::WriteLmdEntry(CFileOutput *lmdOutput, int from, int to, int level)
{
   if (m_aClips.GetSize() == 0)
      return;
   
   for (int i = 0; i < m_aClips.GetSize(); ++i)
   {
      if (m_aClips[i] != NULL)
         m_aClips[i]->WriteLmdEntry(lmdOutput, from, to, level);
   }
}

void ASSISTANT::ClipList::WriteLmdEntry(CFileOutput *lmdOutput)
{
   if (m_aClips.GetSize() == 0)
      return;
   
   for (int i = 0; i < m_aClips.GetSize(); ++i)
   {
      if (m_aClips[i] != NULL)
         m_aClips[i]->WriteLmdEntry(lmdOutput);
   }
}

void ASSISTANT::ClipList::ReturnOrderedList(ASSISTANT::ClipList *orderedList)
{

   if (m_aClips.GetSize() == 0)
      return;
   
   for (int i = 0; i < m_aClips.GetSize(); ++i)
   {
      orderedList->InsertClip(m_aClips[i]);
   }
}