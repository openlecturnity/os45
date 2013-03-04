#ifndef __EDITORSTREAMS__
#define __EDITORSTREAMS__

#include "LrdFile.h"

class CStreamContainer
{
public:
   CStreamContainer();
   ~CStreamContainer();

public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:
   CAudioStream *GetAudioStream() {return m_audioStream;}
   void SetAudioStream(CAudioStream *audioStream); // {m_audioStream = audioStream;}

   CVideoStream *GetVideoStream() {return m_videoStream;}
   void SetVideoStream(CVideoStream *videoStream); // {m_videoStream = videoStream;}

   CClipStream *GetClipStream() {return m_clipStream;}
   void SetClipStream(CClipStream *clipStream); // {m_clipStream = clipStream;}
   void AddClipsToStream(CClipStream *clipStream);

   CWhiteboardStream *GetWhiteboardStream() {return m_whiteboardStream;}
   void SetWhiteboardStream(CWhiteboardStream *whiteboardStream); // {m_whiteboardStream = whiteboardStream;}
   
   CPageStream *GetPageStream() {return m_pageStream;}
   void SetPageStream(CPageStream *pageStream); // {m_whiteboardStream = whiteboardStream;}
   
   CMetadataStream *GetMetadata() {return m_metadata;}
   void SetMetadata(CMetadataStream *metadata); // {m_metadata = metadata;}

   int GetID() {return m_nUniqueID;}
   void SetID(int id) {m_nUniqueID = id;}

private:
   CAudioStream      *m_audioStream;
   CVideoStream      *m_videoStream;
   CClipStream       *m_clipStream;
   CWhiteboardStream *m_whiteboardStream;
   CPageStream       *m_pageStream;
   CMetadataStream   *m_metadata;

   int m_nUniqueID;
  
public:
   static int m_nContainerCounter;
   static int m_iAudioStreamCount;
   static int m_iVideoStreamCount;
   static int m_iClipStreamCount;
   static int m_iWhiteboardStreamCount;
   static int m_iPageStreamCount;
   static int m_iMetadataCount;
};


/**
 * This class maps event times from a source document (lrd + evq) to an 
 * exported target document.
 * So the source times apply to the source documents' time lines and the 
 * target times to the one time line of the exported document.
 */
class CPreviewSegment
{
public:
   CPreviewSegment();
   ~CPreviewSegment();

public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:
   int GetSourceBegin() {return m_iSourceBegin;}
   void SetSourceBegin(int begin) {m_iSourceBegin = begin;}

   int GetSourceEnd() {return m_iSourceEnd;}
   void SetSourceEnd(int end) {m_iSourceEnd = end;}

   int GetTargetBegin() {return m_iTargetBegin;}
   void SetTargetBegin(int begin) {m_iTargetBegin = begin;}

   int GetTargetEnd() {return m_iTargetEnd;}
   void SetTargetEnd(int end) {m_iTargetEnd = end;}

   CAudioStream *GetAudioStream() {return m_streams->GetAudioStream();}
   //void SetAudioStream(CAudioStream *audioStream) {m_streams.SetAudioStream(audioStream);}

   CVideoStream *GetVideoStream() {return m_streams->GetVideoStream();}
   //void SetVideoStream(CVideoStream *videoStream) {m_streams.SetVideoStream(videoStream);}

   CClipStream *GetClipStream() {return m_streams->GetClipStream();}
   //void SetClipStream(CClipStream *clipStream) {m_streams.SetClipStream(clipStream);}

   CWhiteboardStream *GetWhiteboardStream() {return m_streams->GetWhiteboardStream();}
   //void SetWhiteboardStream(CWhiteboardStream *whiteboardStream) {m_streams.SetWhiteboardStream(whiteboardStream);}
   
   CPageStream *GetPageStream() {return m_streams->GetPageStream();}
   //void SetWhiteboardStream(CWhiteboardStream *whiteboardStream) {m_streams.SetWhiteboardStream(whiteboardStream);}
   
   CMetadataStream *GetMetadata() {return m_streams->GetMetadata();}
   //void SetMetadata(CMetadataStream *metadata) {m_streams.SetMetadata(metadata);}

   CStreamContainer *GetStreams() {return m_streams;}
   void SetStreams(CStreamContainer *pStreams) {m_streams = pStreams;}

   int GetAudioOffset() {return m_audioOffset;}
   void SetAudioOffset(int offset) {m_audioOffset = offset;}

   int GetVideoOffset() {return m_videoOffset;}
   void SetVideoOffset(int offset) {m_videoOffset = offset;}


   bool GetUseFlatStructure() {return m_useFlatStructure;}
   void SetUseFlatStructure(bool useFlatStructure) {m_useFlatStructure = useFlatStructure;}

   // calculate source timestamp belonging to segment from a target timestamp
   int SourceTimestampToTarget(int iTimestamp);
   // calculate target timestamp belonging to segment from a source timestamp
   int TargetTimestampToSource(int iTimestamp);
public:
   // manipulation of member variables
   void IncrementTargetBegin(int byLength) {m_iTargetBegin += byLength;}
   void IncrementTargetEnd(int byLength) {m_iTargetEnd += byLength;}
   void DecrementTargetBegin(int byLength) {m_iTargetBegin -= byLength;}
   void DecrementTargetEnd(int byLength) {m_iTargetEnd -= byLength;}
   
   void IncrementSourceBegin(int byLength) {m_iSourceBegin += byLength;}
   void IncrementSourceEnd(int byLength) {m_iSourceEnd += byLength;}
   void DecrementSourceBegin(int byLength) {m_iSourceBegin -= byLength;}
   void DecrementSourceEnd(int byLength) {m_iSourceEnd -= byLength;}

   //void AddClip(CClipInfo *clip);

public:
   /**
    * Splits this object into a shorter begin part and a new end part.
    * This end part ist returned.
    * The given "timestamp" is applied to the target time.
    */
   CPreviewSegment *Split(int timestamp);

   /**
    * Makes a copy of this preview segment.
    * Normally the CStreamContainer remains the same. However since clips can 
    * be moved you also have to make a copy of it in clip move case.
    */
   CPreviewSegment *Copy(bool bCopyStreamContainer = false);

   /**
    * Invokes "Write()" on the associated WhiteboardStream object with the
    * respective source times.
    */
   void SaveWhiteboard(CFileOutput *pEventOutput, CFileOutput *pObjectOutput, 
      LPCTSTR tszRecordPath, LPCTSTR tszPrefix, CPageStream *pPageStream);
   void SaveAudio(HANDLE ladFile);
   void SaveVideo(HANDLE aviFile);
   void SaveClips(LPCSTR savePath);
   void CompleteStructureInfo(CStructureInformation &structureInfo, CArray<CClipInfo *, CClipInfo *> &clipStream);

   void SetStreamContainerIsCopy() {m_bStreamContainerIsCopy = true;}


private:
   int m_iSourceBegin;
   int m_iSourceEnd;
   int m_iTargetBegin;
   int m_iTargetEnd;

   // needed if original file ist replaced
   int m_audioOffset;
   int m_videoOffset;

   CStreamContainer *m_streams;

   bool m_useFlatStructure;
   bool m_bStreamContainerIsCopy;
};

class CUnRedoElement
{
public:
   CUnRedoElement();
   ~CUnRedoElement();

public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:
   enum type
   {
      UNDEFINED,
      CUT_PASTE,
      IMPORT_AUDIO,
      IMPORT_VIDEO,
      IMPORT_VIDEOAUDIO,
      DELETE_PASTE,
      BEGIN_MOVE_CLIP,
      MOVE_CLIP
   };

public:
   void Append(CArray<CPreviewSegment *, CPreviewSegment *> &elementArray, int from , int count, bool copy=true);
   void FillStreamListWith(CList<CStreamContainer *, CStreamContainer *> &streamList, bool copyAudio=false);
   bool MoveStreamsTo(CList<CStreamContainer *, CStreamContainer *> &streamList);
   bool GetStreamsFrom(CList<CStreamContainer *, CStreamContainer *> &streamList);
   void CleanElementList() {m_elementArray.RemoveAll();}
   void MoveElements(CArray<CPreviewSegment *, CPreviewSegment *> &elementArray);
   bool SetUndoClipElements(CUnRedoElement *pDeleteUndo, CUnRedoElement *pPasteUndo);

public:
   int GetBeginIndex() {return m_iBeginIndex;}
   void SetBeginIndex(int beginIndex) {m_iBeginIndex = beginIndex;}
   
   int GetInsertionCount() {return m_iInsertionCount;}
   void SetInsertionCount(int insertionCount) {m_iInsertionCount = insertionCount;}
   
   int GetTimeDiff() {return m_iTimeDiff;}
   void SetTimeDiff(int timeDiff) {m_iTimeDiff = timeDiff;}

   int GetType() {return m_nType;}
   void SetType(int type) {m_nType = type;}

   CUnRedoElement *GetDeleteUndo() {return m_pDeleteUndo;}
   CUnRedoElement *GetPasteUndo() {return m_pPasteUndo;}
   
private:
   int m_nType;

   int m_iBeginIndex;
   int m_iInsertionCount;
   int m_iTimeDiff;
   // structure needed for insert, cur and paste action
   CArray<CPreviewSegment *, CPreviewSegment *> m_elementArray;
   // structure needed for audio import
   CList<CStreamContainer *, CStreamContainer *> m_streamList;
   // elements needed by clip insertion
   CUnRedoElement *m_pDeleteUndo;
   CUnRedoElement *m_pPasteUndo;
};

class CSegmentList
{
public:
   CSegmentList();
   ~CSegmentList();

   int m_iBegin;
   int m_iEnd;
   CArray<CPreviewSegment *, CPreviewSegment *> m_clipBoard;

};

#endif // __EDITORSTREAMS__

