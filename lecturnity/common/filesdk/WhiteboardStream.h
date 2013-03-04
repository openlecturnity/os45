#ifndef __WHITEBOARDSTREAM__
#define __WHITEBOARDSTREAM__

#include "export.h"
#include "mediastream.h"
#include "PageStream.h"

#include "..\drawsdk\tools.h"
#include "..\drawsdk\objects.h"
#include "..\drawsdk\structs.h"
#include "SgmlParser.h"
#include "InteractionReaderWriter.h"
#include "MarkReaderWriter.h"
#include "QuestionReaderWriter.h"

#define E_WB_INVALID_DATA        _HRESULT_TYPEDEF_(0x80d90001L)
#define E_WB_INVALID_INTERACTION _HRESULT_TYPEDEF_(0x80d90002L)
#define E_WB_INVALID_QUESTION    _HRESULT_TYPEDEF_(0x80d90003L)
#define E_WB_INVALID_SUPPORT     _HRESULT_TYPEDEF_(0x80d90004L)


#define FLAG_UNDERLINE 16

class CWhiteboardStream;

class FILESDK_EXPORT CWhiteboardEvent
{
public:
   CWhiteboardEvent(CWhiteboardStream *pWb);
   ~CWhiteboardEvent();

public:
   // set member variables to default values
   void Init();
   
   void InitEmpty();

   // reset member variables
   void Clear();

public:
   // get and set functions for member variables
   int GetTimestamp() {return m_iTimestamp;}
   void SetTimestamp(int timestamp) {m_iTimestamp = timestamp;}

   int GetPageNumber() {return m_iPageNumber;}
   void SetPageNumber(int pageNumber) {m_iPageNumber = pageNumber;}
   
   int GetExportPageNumber() {return m_iExportPageNumber;}
   void SetExportPageNumber(int pageNumber) {m_iExportPageNumber = pageNumber;}
   
   int GetBgColor() {return m_iBgColor;}
   void SetBgColor(int bgColor) {m_iBgColor = bgColor;}

   int GetObjectCount() {return m_iObjectCount;}
   void SetObjectCount(int objectCount) {m_iObjectCount = objectCount;}

   // sets the specified object at the specified postion of this event's object array
   void SetObject(int iObjectNumber, DrawSdk::DrawObject *pObject);

   void AddObjects(CPtrArray &objects);
   // adds the specified element from the specified array to the end of this event's object array
   void AddObject(int objectNumber, CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> &objectArray);
   // PZI: adds the specified object to the end of this event's object array
   void AddObject(DrawSdk::DrawObject *pObject, BOOL bAddToWBObjects = false);
   void RemovePointerObjects();
   void RemoveMousePointerObjects();
   void RemoveTelePointerObjects();

   // create duplicate of m_objects (only pointer to object are copied)
   CPtrArray* GetObjectsPointer() {return &m_objects;}

   void CreateSortedObjArray();

public:
   // read and write
   void Read(LPCSTR eventLine, CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> &objectArray);
   // write events and associated objects
   // PZI: additional argument added to switch off image copy
   void Write(CFileOutput *pEventOutput, CFileOutput *pObjectOutput,
      LPCTSTR tszRecordPath, LPCTSTR tszPrefix, int timeOffset, 
      CMap<CString, LPCTSTR, CString, CString&> &imageMap, bool bCopyImages, bool bRemovePointer);

public:
   void Draw(CDC *pDC, CRect &drawRect, DrawSdk::ZoomManager *zoom, CWhiteboardEvent *pLastEvent=NULL, bool bHidePointer=false);
   CWhiteboardEvent *Copy();
   
private:
   int m_iTimestamp;
   int m_iPageNumber;
   int m_iExportPageNumber;
   int m_iBgColor;

   int m_iObjectCount;

   CPtrArray m_objects;

   int *m_aSortedIds;
   CWhiteboardStream *m_pWb;
};

class FILESDK_EXPORT CWhiteboardStream : public CMediaStream
{
   friend class CWhiteboardEvent;
public:

   // create objects
   // For Bugfix 5301: Additional flag bSaveImageToList (Default: true. If false: image data is read from file and not kept in memory
   static DrawSdk::DrawObject *CreateObject(SgmlElement *pObjTag, CString &csRecordPath, bool bSaveImageToList=true, CString *pcsErrorFont = NULL);

   
   static int m_objectCounter;
   static int m_pageCounter;

   
   CWhiteboardStream();
   ~CWhiteboardStream();

public:
   CWhiteboardStream *Copy();
   // set member variables to default values
   void Init();

   void InitEmpty();

   // reset member variables
   void Clear();


public:
   static HRESULT CopyImage(DrawSdk::Image *pImage, 
      CString &csTargetPathAndPrefix, CMap<CString, LPCTSTR, CString, CString&> &imageMap, LPCTSTR tszSourcePath);

   // open event- and object-file, fill eventlist and object array with data
   HRESULT Open();
   // For Bugfix 5301: Additional flag bSaveImageToList (Default: true. If false: image data is read from file and not kept in memory
   HRESULT Open(bool bSaveImageToList);

   // For Bugfix 5301: Additional flag bSaveImageToList (Default: true. If false: image data is read from file and not kept in memory
   HRESULT ParseObjectSgml(SgmlElement *pObjectElement, CString &csRecordPath, bool bSaveImageToList=true);


   // write all events, located between from and to,
   // and associated objects to evqFile and objFile 
   // PZI: additional argument added to switch off image copy
   // note: "to" is NOT included - TODO: is this correct??
   void Write(CFileOutput *pEventOutput, CFileOutput *pObjectOutput,
      LPCTSTR tszRecordPath, LPCTSTR tszPrefix, int from, int to, 
      int timeOffset, bool copyImages, CPageStream *pPageStream);
   void Write(CString csRecordPath, CPageStream *pPageStream);

   bool Draw(CDC *pDC, int timestamp, CRect &drawRect, 
      DrawSdk::ZoomManager *zoom, bool bForce=true, bool bIncludeInteractives=false, bool bHidePointers=false);

   // PZI: add an event - currently just adds the event without checks
   void AddEvent(CWhiteboardEvent *event);
   // PZI: remove an event - currently just removes the event without checks
   void RemoveEvent(int timestamp);

   /**
    * Fills in the pages within the given time range of the 
    * current whiteboard stream into the pages variable.
    * "Returns" firstPos and lastPos; firstPos is the first
    * position at which a page is added, and lastPos the last.
    * These variables are left unchanged if nothing is added.
    * The variable offset is added to the begin and end times
    * of the added page (CPage) objects (in order to match source
    * and target stream times).
    */
   /*
   void GetPages(CArray<CPage *, CPage *> &pages, int fromMsec, int toMsec, int offset, int &firstPos, int &lastPos);
   int GetPrevPageBegin(int curMsec, int fromMsec ,int toMsec, int offset);
   int GetNextPageBegin(int curMsec, int fromMsec ,int toMsec, int offset);
   CPage *GetFirstPage(int fromMsec ,int toMsec, int offset);
   CPage *GetLastPage(int fromMsec ,int toMsec, int offset);
   */


   CWhiteboardEvent *GetEvent(int timestamp);
   CWhiteboardEvent *GetLastDrawnEvent();

   // set all object numbers to -1
   void ResetObjects();

   // return highest event entry
   int GetLength();

   // Get the object array
   void GetObjectArray(CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> &drawObjects);
   
   void GetEventArray(CArray<CWhiteboardEvent *, CWhiteboardEvent *> &events);
   void ClearLastDrawnEvent() { m_pLastDrawnEvent = NULL; }

public:
   // get and set functions for member variables
   LPCTSTR GetPageDimension() { return m_csPageDimension; }
   void SetPageDimension(LPCTSTR pageDimension) { m_csPageDimension = pageDimension; }

   bool GetPageDimension(CSize &dimPage);

   LPCTSTR GetEvqFilename() { return m_csEvqFilename; }
   void SetEvqFilename(LPCTSTR evqFilename) { m_csEvqFilename = evqFilename; }

   LPCTSTR GetObjFilename() { return m_csObjFilename; }
   void SetObjFilename(LPCTSTR objFilename) { m_csObjFilename = objFilename; }

   int GetID() {return m_iID;}
   void SetID(int id) {m_iID = id;}

   CPageStream *CreatePageStreamFromEvents();
   // set events export page number to number in page stream 
   void UpdateExportPageNumbers(CPageStream *pPageStream);

   bool ContainsMarks();
   // a copy of each object is made to the aInteractions array
   // the source object is then deleted
   HRESULT ExtractMarks(CStopJumpMark *aMarks, UINT *pnElementCount);
   bool ContainsInteractions();
   // a copy of each object is made to the aInteractions array
   // the source object is then deleted
   HRESULT ExtractInteractions(CInteractionArea **paInteractions, UINT *pnElementCount);
   HRESULT ExtractInteractions(CArray<CInteractionArea *, CInteractionArea *> *paInteractives);
   bool ContainsQuestions();
   // extracts the respective objects directly (as pointers)
   // only the pointers (and not the objects) get deleted then
   HRESULT ExtractQuestionnaires(CQuestionnaire **paQuestionnaires, UINT *pnElementCount);
   HRESULT ExtractQuestionInteractives(CArray<CInteractionArea *, CInteractionArea *> *paQuestionInteractives);
    void GetUnsupportedFonts(CArray<CString, CString> &acsUnsupportedFonts);
    void ClearUnsupportedFonts();

private:
   
   // create objects (private)
   static DrawSdk::DrawObject *CreatePolyobject(SgmlElement *objTag);
   static DrawSdk::DrawObject *CreateText(SgmlElement *objTag, CString* pcsErrorFont = NULL);

   // find out which color used (rgb, fcolor, ccolor)
   static Gdiplus::ARGB GetColor(SgmlElement *objTag);

   // read objects from object file and fill m_arrObjects
   // For Bugfix 5301: Additional flag bSaveImageToList (Default: true. If false: image data is read from file and not kept in memory
   HRESULT ReadObjectFile(bool bSaveImageToList=true);
   // read events from event file and append to event list
   bool ReadEventqueue();

   // find event which appears before timestamp
   // POSITION FindEventBefore(int timestamp);
   int FindEventBefore(int timestamp);
   // find event which appears after timestamp
   // POSITION FindEventAfter(int timestamp);
   int FindEventAfter(int timestamp);

private:
   // array to save image information
   static DrawSdk::CImageArray *s_pImageList;

   static UINT s_nInstanceCount;
 
   // name of event file
   CString m_csEvqFilename;
   // name of object file
   CString m_csObjFilename;

   // dimension of whiteboard page
   CString m_csPageDimension;

   // list of object created by object file
   CArray<DrawSdk::DrawObject *, DrawSdk::DrawObject *> m_arrObjects;
   // list of event created by event queue
   CArray<CWhiteboardEvent *, CWhiteboardEvent *> m_lstEvents;
   // reader and collector for mark objects
   CMarkReaderWriter m_MarkReader;
   // reader and collector for interaction objects
   CInteractionReaderWriter m_InteractionReader;
   // reader and collector for interaction objects
   CQuestionReaderWriter m_QuestionReader;

   CArray<CString, CString> m_acsUnsupportedFonts;
   void AddUnsupportedFont(CString csFont);
   

   // POSITION m_lastUsedPosition;
   CWhiteboardEvent *m_pLastDrawnEvent;

   int m_iID;
};


#endif //__WHITEBOARDSTREAM__
