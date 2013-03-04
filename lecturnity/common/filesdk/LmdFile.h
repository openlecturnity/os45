#ifndef __RECORDINGMETADATA__
#define __RECORDINGMETADATA__

#include "export.h"

#include "LmdSgml.h"
#include "LepSgml.h"
#include "LrdSgml.h"
#include "AVStreams.h"
#include "WhiteboardStream.h"
#include "PageStream.h"

class CPageInformation;
class CClipInformation;

class FILESDK_EXPORT CMetaInformation
{
public:
   CMetaInformation();
   ~CMetaInformation();

public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:
   // fill with content from sgml-tag
   bool ParseFrom(CLepSgml *pLepFile);
   bool ParseFrom(CLmdSgml *pLmdFile);
   bool ParseFrom(CLrdSgml *pLrdFile);
   void Fill(CMetaInformation *pMetaInfo);

public:
   LPCTSTR GetTitle() { return m_csTitle; }
   void SetTitle(LPCTSTR title) { m_csTitle = title; }

   LPCTSTR GetAuthor() { return m_csAuthor; }
   void SetAuthor(LPCTSTR author) { m_csAuthor = author; }
   
   LPCTSTR GetCreator() { return m_csCreator; }
   void SetCreator(LPCTSTR creator) { m_csCreator = creator; }
   
   LPCTSTR GetRecordDate() { return m_csRecordDate; }
   void SetRecordDate(LPCTSTR recordDate) { m_csRecordDate = recordDate; }

   LPCTSTR GetComments() { return m_csComments; }
   void SetComments(LPCTSTR comments) { m_csComments = comments; }

   LPCTSTR GetLength() { return m_csLength; }
   void SetLength(LPCTSTR length) { m_csLength = length; }

   LPCTSTR GetRecordTime() { return m_csRecordTime; }
   void SetRecordTime(LPCTSTR recordTime);

   void GetKeywords(CStringList &stringList);
   void SetKeywords(CStringList &stringList);
   
   void GetKeywords(CString &keywords);
   void SetKeywords(CString &keywords);

   void AddKeyword(CString &keyword);
   void AddKeyword(LPCTSTR tszKeyword);

private:
   // common metadata
   CString m_csTitle;
   CString m_csAuthor;

   // metadata from lmd
   CString m_csCreator;
   CString m_csRecordDate;
   CStringList m_slKeywords;
   
   // metadata from lrd
   CString m_csVersion;
   CString m_csComments;
   CString m_csLength;
   CString m_csRecordTime;  
};

class FILESDK_EXPORT CStructureInformation
{
public:
   CStructureInformation();
   virtual ~CStructureInformation();

public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:
   enum type
   {
      CHAPTER,
      PAGE,
      CLIP,
      BASE
   };

public:
   // fill with content from sgml-tag
   bool ParseFrom(CLmdSgml *pLmdFile);
   virtual bool Fill(SgmlElement *pStructureTag, bool &bFirstPageInSegment);
   virtual bool FillWithNewPage(CString &csPageTitle, UINT nLengthMs);
   virtual bool Complete(CClipStream *clipStream, CWhiteboardStream *whiteboardStream, CPageStream *pageStream);
   virtual bool Complete(CClipStream *clipStream);
   virtual void UpdateMetadata(CPageStream *pPageStream);
   // write structure to lmd-file
   virtual void Write(CFileOutput *pOutput, LPCTSTR prefix, CArray<UINT, UINT> *paExportClipTimes, int level=0);
   virtual void WriteDenverEntry(CFileOutput *pOutput, LPCTSTR prefix, int level=0);
    //virtual CStructureInformation *FindElement(int timestamp);
   virtual CStructureInformation *FindElementWithId(int id);
   virtual CStructureInformation *FindElement(int iTimestamp, type nType);
   virtual int GetType() {return BASE/*CHAPTER*/;}
   virtual CStructureInformation *Copy();
   void Fill(CStructureInformation *pStructureInformation);
   virtual void Copy(CList<CStructureInformation *, CStructureInformation *> &elementList, int from, int to, int timeOffset);
   virtual void CopyPages(CList<CStructureInformation *, CStructureInformation *> &elementList, int from, int to, int timeOffset);
   bool DeleteRemovedClips(CArray<CClipInfo *, CClipInfo *> &clipStream);
   void Append(CStructureInformation *newElement);
   virtual void Increment(int timeOffset);
   void GetAllElements(CArray<CStructureInformation *, CStructureInformation *> *paElements);
   void ChangeUniqueIds();
   void HideClickPages(CArray<CPage *, CPage *> &aPageArray);
   void UpdatePageTitles(CArray<CPage *, CPage *> &aPageArray);
   void UpdateNonClickPageTitles(int &iNextNumber);
   void DeleteClickPages();
   bool HasClickPages( bool bIncludeAnalysisPagesWithClicks=false );
   bool GetLastNotClickTimestamp(int &iTimestamp);
   bool FindFirstNotClickTimestamp(int &iTimestamp);
   void GetAllPageTimestamps(CList<int> &lstPageTimestamps);

public:
   virtual int GetBegin() {return m_iBegin;}
   virtual void SetBegin(int begin) {m_iBegin = begin;}

   virtual int GetEnd() {return m_iEnd;}
   virtual void SetEnd(int end) {m_iEnd = end;}

   // Find the lowest Timestamp in the element list (including clips)
   virtual int GetFirstTime();
   // Find the highest Timestamp in the element list (excluding clips)
   virtual int GetLastTime();


   virtual int GetUniqueId() {return m_iUniqueId;}
   virtual void SetUniqueId(int id) {m_iUniqueId = id;}

   virtual LPCTSTR GetTitle() { return m_csTitle; }

public:
   virtual int GetHeight(int y, int height);
   
   virtual void Draw(CDC *pDC, int &x, int &y, int pageWidth, int pageHeight, int viewWidth, int activePage, int lastActivePage, LOGFONT &logFont, DrawSdk::ZoomManager *zoom);

//   virtual int Draw(CDC *pDC, int &x, int &y, int height, int viewWidth, int timestamp, CFont *font, CRect &clipRect, DrawSdk::ZoomManager *zoom = NULL);
   Gdiplus::Image * LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance = NULL);

   virtual void CutBefore(int timeStamp);
   virtual void CutAfter(int timeStamp);

   virtual bool IsEmpty();

   virtual CPageInformation *FindFirstPage();
   virtual CPageInformation *FindLastPage();
   
   virtual void GetClips(CList<CClipInformation *, CClipInformation*> &clipList);

   virtual CStructureInformation *FindFirstElement();
   virtual CStructureInformation *FindLastElement();

   virtual bool InsertMissingClips(CArray<CClipInfo *, CClipInfo *> &clipStream);
   virtual bool InsertClip(CClipInformation *pClip);

   virtual int GetTimestamp(int y, int &actY, int height);
   virtual void GetNextY(int &y, int height);
   virtual int GetDrawHeight(int pageHeight);
   virtual bool GetYPos(int &y, int height, int timestamp);
   virtual int GetActivePage(int timestamp);

   // merge mergeElement in structure
   virtual void AppendStructure(CStructureInformation *element);

   virtual void UpdatePageNumbers(int &number, CMap<int, int, int, int> &mapUniquePageIds);
   // This function sets the beginning of the chapters to the end of the
   // previous element. This is to avoid gaps between elements.
   virtual void UpdateTimestamps(int initialBegin);

   virtual void ResetClipThumb();

   virtual void Print(int iLevel = 0);

protected:
   int m_iBegin;
   int m_iEnd;
   int m_iUniqueId;
   CString m_csTitle;
   CStructureInformation *m_pCopySource;

public:
   static int m_iIdCounter;
   // list with subnodes
   CList<CStructureInformation *, CStructureInformation *> m_elementList;
};

class CChapterInformation : public CStructureInformation
{
public:
   CChapterInformation();
   virtual ~CChapterInformation();
   
public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:
   // fill with content from sgml-tag
   virtual bool Fill(SgmlElement *pChapterTag, bool &bFirstPageInSegment);
   virtual bool FillWithNewPage(CString &csPageTitle, UINT nLengthMs);
   virtual void Write(CFileOutput *pOutput, LPCTSTR prefix, CArray<UINT, UINT> *paExportClipTimes, int level=0);
   virtual int GetType() {return CHAPTER;}
   virtual CStructureInformation *Copy();
   virtual void Copy(CList<CStructureInformation *, CStructureInformation *> &elementList, int from, int to, int timeOffset);
public:
   LPCTSTR GetTitle() { return m_csTitle; }
   void SetTitle(LPCTSTR title) { m_csTitle = title; }

   int GetNumber() {return m_iNumber;}
   void SetNumber(int number) {m_iNumber = number;}

public:
   virtual void Draw(CDC *pDC, int &x, int &y, int pageWidth, int pageHeight, int viewWidth, int activePage, int lastActivePage, LOGFONT &logFont, DrawSdk::ZoomManager *zoom);

//   virtual int Draw(CDC *pDC, int &x, int &y, int height, int viewWidth, int timestamp, CFont *font, CRect &clipRect, DrawSdk::ZoomManager *zoom = NULL);
   virtual int GetTimestamp(int y, int &actY, int height);
   virtual void GetNextY(int &y, int height);
   virtual int GetDrawHeight(int pageHeight);
   //virtual int GetYPos(int &y, int height, int timestamp);

   virtual void Print(int iLevel);

protected:
   CString m_csTitle;
   int m_iNumber;
};

class FILESDK_EXPORT CPageInformation : public CStructureInformation
{
public:
   CPageInformation();
   ~CPageInformation();

public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:
   // fill with content from sgml-tag
   virtual bool Fill(SgmlElement *pPageTag, bool &bFirstPageInSegment);
   virtual bool FillWithNewPage(CString &csPageTitle, UINT nLengthMs);
   virtual bool IsEmpty();
   virtual bool Complete(CWhiteboardStream *whiteboardStream, CPageStream *pageStream);
   virtual void UpdateMetadata(CPageStream *pPageStream);
   virtual void Write(CFileOutput *pOutput, LPCTSTR prefix, CArray<UINT, UINT> *paExportClipTimes, int level=0);
   //rtual CStructureInformation *FindElement(int timestamp);
   virtual int GetType() {return PAGE;}
   virtual bool Insert(CList<CStructureInformation *, CStructureInformation *> &structureInfoElements, int timestamp) {return false;}
   virtual void Increment(int timeOffset);
   virtual CStructureInformation *Copy();
   virtual void Copy(CList<CStructureInformation *, CStructureInformation *> &elementList, int from, int to, int timeOffset);
   virtual void CopyPages(CList<CStructureInformation *, CStructureInformation *> &elementList, int from, int to, int timeOffset);
  
public:
   int GetInternalNumber() {return m_nInternalNumber;}
   void SetInternalNumber(int number) {m_nInternalNumber = number;} 

   virtual int GetNumber() {return m_iNumber;}
   virtual void SetNumber(int number) {m_iNumber = number;} 

   virtual LPCTSTR GetTitle() { return m_csTitle;}
   virtual void SetTitle(LPCTSTR title) { m_csTitle = title; } 

   virtual LPCTSTR GetThumb() { return m_csThumb; }
   virtual void SetThumb(LPCTSTR thumb) { m_csThumb = thumb; } 

   virtual void GetKeywords(CStringList &stringList);
   virtual void SetKeywords(CStringList &stringList);
   virtual void GetKeywords(CString &csKeywords);
   virtual void SetKeywords(CString &cskeywords);

   CString &GetPageType() {return m_csType;}
   void SetPageType(LPCTSTR szType) {m_csType = szType;} 

   CString &GetPageClickButton() { return m_csClickButton; }
   void SetPageClickButton(LPCTSTR szClickButton) { m_csClickButton = szClickButton; } 
   int GetPageClickX() { return m_iClickX; }
   void SetPageClickX(int iClickX) { m_iClickX = iClickX; }
   int GetPageClickY() { return m_iClickY; }
   void SetPageClickY(int iClickY) { m_iClickY = iClickY; }
   
   CWhiteboardEvent *GetDrawEvent() {return m_drawEvent;}
   void SetDrawEvent(CWhiteboardEvent *event) {m_drawEvent = event;} 

public:
   virtual void Draw(CDC *pDC, int &x, int &y, int pageWidth, int pageHeight, int viewWidth, int activePage, int lastActivePage, LOGFONT &logFont, DrawSdk::ZoomManager *zoom);

//   virtual int Draw(CDC *pDC, int &x, int &y, int height, int viewWidth, int timestamp, CFont *font, CRect &clipRect, DrawSdk::ZoomManager *zoom = NULL);
   virtual int GetTimestamp(int y, int &actY, int height);
   virtual void GetNextY(int &y, int height);
   virtual int GetDrawHeight(int pageHeight);
   //virtual int GetYPos(int &y, int height, int timestamp);
   virtual int GetActivePage(int timestamp);
   
   virtual void UpdatePageNumbers(int &number, CMap<int, int, int, int> &mapUniquePageIds);
   
   virtual void Print(int iLevel);

protected:
   int     m_nInternalNumber;
   int     m_iNumber;
   CString m_csType;
   CString m_csClickButton;
   int     m_iClickX;
   int     m_iClickY;
   CString m_csTitle;
   CString m_csThumb;
   CStringList m_slKeywords;

   CWhiteboardEvent *m_drawEvent;

};

class FILESDK_EXPORT CClipInformation : public CPageInformation
{
public:
   CClipInformation();
   ~CClipInformation();

public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:
   // fill with content from sgml-tag
   virtual bool Fill(SgmlElement *pClipInfoTag, bool &bFirstPageInSegment);
   virtual bool Complete(CClipStream *clipStream);
   virtual void Write(CFileOutput *pOutput, LPCTSTR prefix, CArray<UINT, UINT> *paExportClipTimes, int level=0);
   //virtual CStructureInformation *FindElement(int timestamp);
   virtual int GetType() {return CLIP;}
   virtual bool Insert(CList<CStructureInformation *, CStructureInformation *> &structureInfoElements, int timestamp) {return false;}
   virtual void Increment(int timeOffset);
   virtual CStructureInformation *Copy();
   virtual void Copy(CList<CStructureInformation *, CStructureInformation *> &elementList, int from, int to, int timeOffset);
   virtual void CopyPages(CList<CStructureInformation *, CStructureInformation *> &elementList, int from, int to, int timeOffset);
   
   virtual void UpdatePageNumbers(int &number, CMap<int, int, int, int> &mapUniquePageIds);


public:
   virtual void Draw(CDC *pDC, int &x, int &y, int pageWidth, int pageHeight, int viewWidth, int activePage, int lastActivePage, LOGFONT &logFont, DrawSdk::ZoomManager *zoom);

//   virtual int Draw(CDC *pDC, int &x, int &y, int height, int viewWidth, int timestamp, CFont *font, CRect &clipRect, DrawSdk::ZoomManager *zoom = NULL);
   virtual int GetTimestamp(int y, int &actY, int height);
   virtual void GetNextY(int &y, int height);
   virtual int GetDrawHeight(int pageHeight);
   //virtual int GetYPos(int &y, int height, int timestamp);
   virtual int GetActivePage(int timestamp);
   
   LPCTSTR GetFilename() { return m_csFilename; }
   void SetFilename(LPCTSTR filename) { m_csFilename = filename; }
   
   int GetClipHeight(int height);

   virtual void ResetClipThumb() {m_bThumIsInitialized = false;}

   virtual void Print(int iLevel);

public:
   CString m_csFilename;
   HBITMAP m_clipThumb;
   int m_nThumbWidth;
   int m_nThumbHeight;
   bool m_bThumIsInitialized;
   bool m_bThumbIsCreated;
};

class CFileInformation
{
public:
   CFileInformation();
   ~CFileInformation();

public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:
   // fill with content from sgml-tag
   bool ParseFrom(CLmdSgml *pLmdFile);
   void Fill(CFileInformation *pFileInfo);

   void SetAudioSamplerate(int iAudioSamplerate) {m_iAudioSamplerate = iAudioSamplerate;}
   void SetAudioBitrate(int iAudioBitrate) {m_iAudioBitrate = iAudioBitrate;}
   void SetVideoFramerate(float fVideoFramerate) {m_fVideoFramerate = fVideoFramerate;}
   void SetVideoQuality(int iVideoQuality) {m_iVideoQuality = iVideoQuality;}
   void SetVideoKeyframes(int iVideoKeyframes) {m_iVideoKeyframes = iVideoKeyframes;}
   void SetVideoCodec(LPCTSTR szVideoCodec) {m_csVideoCodec = szVideoCodec;}

   int GetAudioSamplerate() {return m_iAudioSamplerate;}
   int GetAudioBitrate() {return m_iAudioBitrate;}
   float GetVideoFramerate() {return m_fVideoFramerate;}
   int GetVideoQuality() {return m_iVideoQuality;}
   int GetVideoKeyframes() {return m_iVideoKeyframes;}
   LPCTSTR GetVideoCodec() {return m_csVideoCodec;}

   bool ShowClickPages() {return m_bShowClickPages;}
   void SetShowClickPages(bool bShowClickPages) {m_bShowClickPages = bShowClickPages;}

   // complete stream information (with framerate, bitrate, ... from lmd-file)
   bool Complete(CAudioStream *audioStream, CVideoStream *videoStream);

private:
   int m_iAudioSamplerate;
   int m_iAudioBitrate;
   float m_fVideoFramerate;
   int m_iVideoQuality;
   int m_iVideoKeyframes;
   CString m_csVideoCodec;
    bool m_bShowClickPages;
};


class FILESDK_EXPORT CMetadataStream : public CMediaStream
{
public:
   CMetadataStream();
   ~CMetadataStream();
   
public:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:
   CMetadataStream *Copy();
   bool ParseFrom(CLrdSgml *pLrdFile);
   // open LMD-file, fill metadata and document structure with according information
   bool Open();
   // complete 
   bool Complete(CAudioStream *audioStream, CVideoStream *videoStream, CClipStream *clipStream, 
                 CWhiteboardStream *whiteboardStream, CPageStream *pageStream);
   bool Complete(CClipStream *clipStream);
   void UpdateMetadata(CPageStream *pPageStream);
   // write sgml structure
   bool Write(LPCTSTR lmdFilename);
   // append all structure entries between sourceBegin and sourceEnd
   // is useFlatStructure is true, insert only pages and clips 
   void Append(CStructureInformation &structure, int sourceBegin, int sourceEnd, int timeOffset, CArray<CClipInfo *, CClipInfo *> &clipStream, bool useFlatStructure);
   // remove clips not appears in stream
   bool DeleteRemovedClips(CList<CStructureInformation *, CStructureInformation *> &list, CArray<CClipInfo *, CClipInfo *> &clipStream);

public:
   // get and set functions for member variables
   LPCTSTR GetFilename() { return m_csFilename; }
   void SetFilename(LPCTSTR filename) { m_csFilename = filename; } 

   CMetaInformation *GetMetaInformation() {return &m_metaInfo;}
   CFileInformation *GetFileInformation() {return &m_fileInfo;}
   CStructureInformation *GetStructureInformation() {return &m_structureInfo;}
   
   
   void SetStructureInformation(CStructureInformation *pStructureInfo);
   void SetMetaInformation(CMetaInformation *pMetaInfo);
   void SetFileInformation(CFileInformation *pFileInfo);
   void SetSgmlInformation(SgmlFile *pLmdSgml);


   int GetID() {return m_iID;}
   void SetID(int id) {m_iID = id;}

protected:
   CString               m_csFilename;
   CMetaInformation      m_metaInfo;
   CStructureInformation m_structureInfo;
   CFileInformation      m_fileInfo;

   CLmdSgml              m_lmdSgml;
   int                   m_iID;
};


#endif // __RECORDINGMETADATA__