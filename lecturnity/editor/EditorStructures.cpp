#include "stdafx.h"

#include "EditorStructures.h"
/*********************************************
 *********************************************/

int CStreamContainer::m_nContainerCounter = 0;

int CStreamContainer::m_iAudioStreamCount = 0;
int CStreamContainer::m_iVideoStreamCount = 0;
int CStreamContainer::m_iClipStreamCount = 0;
int CStreamContainer::m_iWhiteboardStreamCount = 0;
int CStreamContainer::m_iPageStreamCount = 0;
int CStreamContainer::m_iMetadataCount = 0;

CStreamContainer::CStreamContainer()
{
   Init();
}

CStreamContainer::~CStreamContainer()
{
   Clear();
}

void CStreamContainer::Init()
{
   m_audioStream      = NULL;
   m_videoStream      = NULL;
   m_clipStream       = NULL;
   m_whiteboardStream = NULL;
   m_pageStream       = NULL;
   m_metadata         = NULL;
}

void CStreamContainer::Clear()
{
   if (m_audioStream)
      m_audioStream->ReleaseReference();
   m_audioStream = NULL;
   if (m_videoStream)
      m_videoStream->ReleaseReference();
   m_videoStream = NULL;
   if (m_clipStream)
      m_clipStream->ReleaseReference();
   m_clipStream = NULL;
   if (m_metadata)
      m_metadata->ReleaseReference();
   m_metadata = NULL;
   if (m_whiteboardStream)
      m_whiteboardStream->ReleaseReference();
   m_whiteboardStream = NULL;
   if (m_pageStream)
      m_pageStream->ReleaseReference();
   m_pageStream = NULL;
} 

void CStreamContainer::AddClipsToStream(CClipStream *clipStream)
{
   if (!clipStream->HasClips())
      return;

   if (!m_clipStream)
   {
      m_clipStream = new CClipStream();
      m_clipStream->AddReference();
   }

   // if a clip from clipStream is not in m_clipStream: insert pointer to clip
   for (int i = 0; i < clipStream->GetSize(); ++i)
   {
      CClipInfo *clip = clipStream->FindClip(i);
      if (clip)
      {
         CString clipFilename = clip->GetFilename();
         CClipInfo *findClip = m_clipStream->FindClip(clipFilename, clip->GetTimestamp(), clip->GetLength());
         if (!findClip)
            m_clipStream->AddClip(clip);
      }
   }
}

void CStreamContainer::SetAudioStream(CAudioStream *audioStream)
{
   if (m_audioStream)
      m_audioStream->ReleaseReference();
   m_audioStream = audioStream;
   if (audioStream)
      audioStream->AddReference();
}

void CStreamContainer::SetVideoStream(CVideoStream *videoStream)
{
   if (m_videoStream)
      m_videoStream->ReleaseReference();
   m_videoStream = videoStream;
   if (videoStream)
      videoStream->AddReference();
}

void CStreamContainer::SetClipStream(CClipStream *clipStream)
{
   if (m_clipStream)
      m_clipStream->ReleaseReference();
   m_clipStream = clipStream;
   if (clipStream)
      clipStream->AddReference();
}

void CStreamContainer::SetMetadata(CMetadataStream *metadata)
{
   if (m_metadata)
      m_metadata->ReleaseReference();
   m_metadata = metadata;
   if (metadata)
      metadata->AddReference();
}

void CStreamContainer::SetWhiteboardStream(CWhiteboardStream *whiteboardStream)
{
   if (m_whiteboardStream)
      m_whiteboardStream->ReleaseReference();
   m_whiteboardStream = whiteboardStream;
   if (m_whiteboardStream)
      m_whiteboardStream->AddReference();
}

void CStreamContainer::SetPageStream(CPageStream *pageStream)
{
   if (m_pageStream)
      m_pageStream->ReleaseReference();
   m_pageStream = pageStream;
   if (m_pageStream)
      m_pageStream->AddReference();
}

/*********************************************
 *********************************************/

CPreviewSegment::CPreviewSegment()
{
   Init();
}

CPreviewSegment::~CPreviewSegment()
{
   Clear();
}

void CPreviewSegment::Init()
{
   m_iSourceBegin     = -1;
   m_iSourceEnd       = -1;
   m_iTargetBegin     = -1;
   m_iTargetEnd       = -1;

   m_audioOffset      = 0;
   m_videoOffset      = 0;

   m_useFlatStructure = false;

   m_bStreamContainerIsCopy = false;
}

void CPreviewSegment::Clear()
{
   m_iSourceBegin     = -1;
   m_iSourceEnd       = -1;
   m_iTargetBegin     = -1;
   m_iTargetEnd       = -1;

   m_audioOffset      = 0;
   m_videoOffset      = 0;

   if (m_bStreamContainerIsCopy && m_streams)
      delete m_streams;

   m_streams          = NULL;

   m_useFlatStructure = false;
}

CPreviewSegment *CPreviewSegment::Split(int timestamp)
{
   // TODO m_iTargetEnd is already the beginning of the next segment (1 too big)
   // so splitting a the very end is not recognized: a 1ms element is created (and shouldn't).
   // More generally: It is possible to create segments without slide/wb/page data
   // these cause (caused) problems after reloading the corresponding LEP.


   if (timestamp <= m_iTargetBegin || timestamp >= m_iTargetEnd)
      return NULL;

   // create new Element
   CPreviewSegment *newElement = new CPreviewSegment();

   // set Streams
   newElement->SetStreams(m_streams);

   // set timestamps
   int diffTime = timestamp - m_iTargetBegin;
   newElement->SetSourceBegin(m_iSourceBegin + diffTime);
   newElement->SetSourceEnd(m_iSourceEnd);
   newElement->SetTargetBegin(m_iTargetBegin + diffTime);
   newElement->SetTargetEnd(m_iTargetEnd);
   newElement->SetAudioOffset(m_audioOffset);
   newElement->SetVideoOffset(m_videoOffset);

   diffTime = m_iTargetEnd-timestamp;
   // adjust end timestamps
   m_iSourceEnd -= diffTime;
   m_iTargetEnd -= diffTime;
   
   return newElement;
}

CPreviewSegment *CPreviewSegment::Copy(bool bCopyStreamContainer)
{ 
   // create new Element
   CPreviewSegment *newElement = new CPreviewSegment();

   // set Streams
   if (bCopyStreamContainer)
   {
      CStreamContainer *pCopyContainer = new CStreamContainer();
      
      CAudioStream *pAudioStream = m_streams->GetAudioStream();
      if (pAudioStream)
      {
         pCopyContainer->SetAudioStream(pAudioStream);
      }
      
      CVideoStream *pVideoStream = m_streams->GetVideoStream();
      if (pVideoStream)
      {
         pCopyContainer->SetVideoStream(pVideoStream);
      }
      
      CClipStream *pClipStream = m_streams->GetClipStream();
      if (pClipStream)
      {
         pCopyContainer->SetClipStream(pClipStream);
      }
      
      CWhiteboardStream *pWhiteboardStream = m_streams->GetWhiteboardStream();
      if (pWhiteboardStream)
      {
         pCopyContainer->SetWhiteboardStream(pWhiteboardStream);
      }
      
      CMetadataStream *pMetadata = m_streams->GetMetadata();
      if (pMetadata)
      {
         pCopyContainer->SetMetadata(pMetadata);
      }
      
      CPageStream *pPageStream = m_streams->GetPageStream();
      if (pPageStream)
      {
         pCopyContainer->SetPageStream(pPageStream);
      }
      newElement->SetStreams(pCopyContainer);
      newElement->SetStreamContainerIsCopy();
   }
   else
   {
      newElement->SetStreams(m_streams);
   }

   // set timestamps
   newElement->SetSourceBegin(m_iSourceBegin);
   newElement->SetSourceEnd(m_iSourceEnd);
   newElement->SetTargetBegin(m_iTargetBegin);
   newElement->SetTargetEnd(m_iTargetEnd);
   newElement->SetAudioOffset(m_audioOffset);
   newElement->SetVideoOffset(m_videoOffset);
   
   return newElement;
}

void CPreviewSegment::SaveWhiteboard(CFileOutput *pEventOutput, CFileOutput *pObjectOutput, 
                                     LPCTSTR tszRecordPath, LPCTSTR tszPrefix, CPageStream *pPageStream)
{
   int timeOffset = m_iTargetBegin - m_iSourceBegin;
   
   if (m_streams->GetWhiteboardStream())
   {
      m_streams->GetWhiteboardStream()->UpdateExportPageNumbers(pPageStream);
      m_streams->GetWhiteboardStream()->Write(pEventOutput, pObjectOutput,
         tszRecordPath, tszPrefix, m_iSourceBegin, m_iSourceEnd, timeOffset, true, pPageStream);
   }

}

void CPreviewSegment::SaveAudio(HANDLE ladFile)
{
   int timeOffset = m_iTargetBegin - m_iSourceBegin;
   
   if (m_streams->GetAudioStream())
   {
      //m_streams.GetAudioStream()
   }

}

void CPreviewSegment::SaveVideo(HANDLE aviFile)
{
   int timeOffset = m_iTargetBegin - m_iSourceBegin;
   
   if (m_streams->GetVideoStream())
   {
      //m_streams.GetAudioStream()
   }

}

void CPreviewSegment::SaveClips(LPCSTR savePath)
{
   int timeOffset = m_iTargetBegin - m_iSourceBegin;
   
   if (m_streams->GetClipStream())
   {
      //m_streams.GetClipStream()->AddClip//
   }

}

void CPreviewSegment::CompleteStructureInfo(CStructureInformation &structureInfo, CArray<CClipInfo *, CClipInfo *> &clipStream)
{
   if (m_streams->GetMetadata())
   {
      int timeOffset = m_iTargetBegin - m_iSourceBegin;
      m_streams->GetMetadata()->Append(structureInfo, m_iSourceBegin, m_iSourceEnd, timeOffset, clipStream, m_useFlatStructure);
   }
}

/*
void CPreviewSegment::AddClip(CClipInfo *clip)
{
  
   CClipStream *clipStream = m_streams->GetClipStream();

   if (!clipStream)
   {
      clipStream = new CClipStream();
      m_streams.SetClipStream(clipStream);
   }

   clipStream->AddClip(clip);
}
*/

int CPreviewSegment::SourceTimestampToTarget(int iTimestamp)
{
   return (iTimestamp + (m_iTargetBegin - m_iSourceBegin));
}

int CPreviewSegment::TargetTimestampToSource(int iTimestamp)
{
   return (iTimestamp + (m_iSourceBegin - m_iTargetBegin));
}

/*********************************************
 *********************************************/

CUnRedoElement::CUnRedoElement()
{
   Init();
}

CUnRedoElement::~CUnRedoElement()
{
   Clear();
}

void CUnRedoElement::Init()
{
   m_nType = UNDEFINED;

   m_pDeleteUndo = NULL;
   m_pPasteUndo  = NULL;

   m_iBeginIndex     = -1;
   m_iInsertionCount = -1;

}

void CUnRedoElement::Clear()
{
   if (m_pDeleteUndo)
      delete m_pDeleteUndo;
   m_pDeleteUndo = NULL;
   
   if (m_pPasteUndo)
      delete m_pPasteUndo;
   m_pPasteUndo  = NULL;

   m_iBeginIndex     = -1;
   m_iInsertionCount = -1;
   for (int i = 0; i < m_elementArray.GetSize(); ++i)
   {
      CPreviewSegment *element = m_elementArray[i];
      if (element)
         delete element;
   }
   m_elementArray.RemoveAll();

   if (!m_streamList.IsEmpty())
   {
      POSITION position = m_streamList.GetHeadPosition();
      while (position)
      {
         CStreamContainer *pContainer = m_streamList.GetNext(position);
         if (pContainer)
         {
            delete pContainer;
         }
         pContainer = NULL;
      }
      m_streamList.RemoveAll();
   }
}

void CUnRedoElement::Append(CArray<CPreviewSegment *, CPreviewSegment *> &elementArray, int from , int count, bool copy)
{
   if (from < 0)
      return;

   for (int i = from; i < from + count; ++i)
   {
      CPreviewSegment *element = elementArray[i];
      if (copy && element)
         element = element->Copy();
      if (element)
         m_elementArray.Add(element);
   }
}

void CUnRedoElement::FillStreamListWith(CList<CStreamContainer *, CStreamContainer *> &streamList, bool copyAudio)
{
   POSITION position = streamList.GetHeadPosition();
   while (position)
   {
      CStreamContainer *pContainer = streamList.GetNext(position);
      if (pContainer)
      {
         CStreamContainer *pCopyContainer = new CStreamContainer();
         pCopyContainer->SetAudioStream(pContainer->GetAudioStream());

         CVideoStream *pVideoStream = pContainer->GetVideoStream();
         if (pVideoStream)
         {
             pCopyContainer->SetVideoStream(pVideoStream);
         }

         CClipStream *pClipStream = pContainer->GetClipStream();
         if (pClipStream)
         {
             pCopyContainer->SetClipStream(pClipStream);
         }

         CWhiteboardStream *pWhiteboardStream = pContainer->GetWhiteboardStream();
         if (pWhiteboardStream)
         {
             pCopyContainer->SetWhiteboardStream(pWhiteboardStream);
         }

         CMetadataStream *pMetadata = pContainer->GetMetadata();
         if (pMetadata)
         {
             pCopyContainer->SetMetadata(pMetadata);
         }
         
         CPageStream *pPageStream = pContainer->GetPageStream();
         if (pPageStream)
         {
            pCopyContainer->SetPageStream(pPageStream);
         }
         
         m_streamList.AddTail(pCopyContainer);
      }
   }

}

bool CUnRedoElement::MoveStreamsTo(CList<CStreamContainer *, CStreamContainer *> &streamList)
{
   if (streamList.GetCount() == 0)
      return false;

   POSITION srcPosition = m_streamList.GetHeadPosition();
   POSITION destPosition = streamList.GetHeadPosition();
   while (srcPosition && destPosition)
   {
      CStreamContainer *pSrcContainer = m_streamList.GetNext(srcPosition);
      CStreamContainer *pDestContainer = streamList.GetNext(destPosition);
      if (pSrcContainer && pDestContainer)
      {
         pDestContainer->SetAudioStream(pSrcContainer->GetAudioStream());
         pDestContainer->SetVideoStream(pSrcContainer->GetVideoStream());
         pDestContainer->SetClipStream(pSrcContainer->GetClipStream());
         pDestContainer->SetWhiteboardStream(pSrcContainer->GetWhiteboardStream());
         pDestContainer->SetMetadata(pSrcContainer->GetMetadata());
      }
   }

   return true;
}

bool CUnRedoElement::GetStreamsFrom(CList<CStreamContainer *, CStreamContainer *> &streamList)
{
   if (streamList.GetCount() == 0)
      return false;

   POSITION srcPosition = streamList.GetHeadPosition();
   while (srcPosition)
   {
      CStreamContainer *pSrcContainer = streamList.GetNext(srcPosition);
      if (pSrcContainer)
      {
         CStreamContainer *pDestContainer = new CStreamContainer();
         pDestContainer->SetAudioStream(pSrcContainer->GetAudioStream());
         pDestContainer->SetVideoStream(pSrcContainer->GetVideoStream());
         pDestContainer->SetClipStream(pSrcContainer->GetClipStream());
         pDestContainer->SetWhiteboardStream(pSrcContainer->GetWhiteboardStream());
         pDestContainer->SetPageStream(pSrcContainer->GetPageStream());
         pDestContainer->SetMetadata(pSrcContainer->GetMetadata());
         m_streamList.AddTail(pDestContainer);
      }
   }

   return true;
}

void CUnRedoElement::MoveElements(CArray<CPreviewSegment *, CPreviewSegment *> &elementArray)
{  
   // append objects to new array
   elementArray.Append(m_elementArray);

   // empty array
   m_elementArray.RemoveAll();
}


bool CUnRedoElement::SetUndoClipElements(CUnRedoElement *pDeleteUndo, CUnRedoElement *pPasteUndo)
{
   m_pDeleteUndo = pDeleteUndo;
   m_pPasteUndo  = pPasteUndo;

   return true;
}
/*********************************************
 *********************************************/

CSegmentList::CSegmentList()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   m_iBegin = -1;
   m_iEnd = -1;
}

CSegmentList::~CSegmentList()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());

   for (int i = 0; i < m_clipBoard.GetSize(); ++i)
   {
      CPreviewSegment *part = m_clipBoard.GetAt(i);
      if (part)
         delete part;
   }

   m_clipBoard.RemoveAll();
}


/*********************************************
 *********************************************/
