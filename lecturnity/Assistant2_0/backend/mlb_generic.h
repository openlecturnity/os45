/*********************************************************************

 program : mlb_generic.h 
 authors : Gabriela Maass
 date    : 08.03.2000
 desc    : 

 **********************************************************************/

#ifndef __ASSISTANT_GENERIC__
#define __ASSISTANT_GENERIC__

#ifdef WIN32
#  pragma warning (disable: 4786)
#endif

#include <stdio.h>
#include <afxtempl.h>
#include "FileOutput.h"
#include "..\DocumentStructureView.h"


#ifdef _ImageMagick
  namespace MagickLib
   {
#undef PACKAGE
#undef VERSION
      
#include <magick/api.h>
      
#undef class
    }
#endif

namespace ASSISTANT {
  //using namespace std;

   #define OO_FIRST 1
   #define OO_OFFSET 1

  class DrawObject;
  class Page;

  class GenericObject {
  protected:
    UINT 
      id;
    float
      order; 
    CString 
      tkref;
    
  public:
    enum type {
      RECTANGLE, 
      OVAL,
      LINE,
      ASCIITEXT,
      POLYGON,
      MARKER,
      FREEHAND,
      TELEPOINTER,
      POLYLINE,
      CHAR,
      IMAGE,
      PENCIL,
      GROUP,
      ANIMATED,
      DOCUMENT,
      FOLDER,
      PAGE,
      LAYER
    };

    GenericObject() : id (0) {}
    GenericObject(_TCHAR *name, UINT id, CString &tkref);
    GenericObject(_TCHAR *name, UINT id);
    GenericObject(UINT id, CString &tkref);
    GenericObject(UINT id);

    virtual ~GenericObject();
    
    virtual int   GetType() = 0;

    UINT GetID() {return id;}
    void   SetID(UINT _id) {id = _id;} 
    
    float GetOrder() {return order;} 
    virtual void SetOrder(float _order) {order = _order;}
    
    virtual int GetDeleteSize() {return -1;}
  };


  
   class Scorm  
   {
   protected:
      CString
         title_,
         keywords_,
         author_, 
         creator_,
         date_;
   public:
      Scorm() {}
      ~Scorm() {}

      void SetTitle(LPCTSTR title) {title_ = title;}
      void SetAuthor(LPCTSTR author) {author_ = author;}
      void SetCreator(LPCTSTR creator) {creator_ = creator;}
      void SetDate(LPCTSTR date) {date_ = date;}
      void SetKeywords(LPCTSTR keywords) {keywords_ = keywords;}

      CString &GetTitle() {return title_;}
      CString &GetAuthor() {return author_;}
      CString &GetCreator() {return creator_;}
      CString &GetDate() {return date_;}
      CString &GetKeywords() {return keywords_;}

   };


  class GenericContainer : public GenericObject {
  protected:
    CArray<GenericContainer *, GenericContainer *> 
      subContainer;
    int
      current;
    bool 
       m_bSelected;
    CString 
      nodeName;
    CString
      nodePath;
    CString
      containerName;
    unsigned int 
       codePage;
    int
       thumbNumber;
    float 
       lastObjectOrder; 
    Scorm
       scorm;
    bool m_bContainerChanged;
  public:
    GenericContainer();
    GenericContainer(_TCHAR *name, UINT id, CString &tkref);
    GenericContainer(_TCHAR *name, UINT id);
    GenericContainer(UINT id, CString &tkref);
    GenericContainer(UINT id);
    virtual ~GenericContainer();

    virtual void Init();
    virtual bool Empty() {return subContainer.GetSize() == 0;}
    virtual int  GetSize() {return subContainer.GetSize();}

    /*** General Functions ***/
    virtual void SetName(LPCTSTR _name)=0;
    virtual CString &GetName() {return containerName;}
    virtual void SetCodepage(unsigned int iCodePage) {codePage = iCodePage;}
    virtual unsigned int GetCodepage() {return codePage;}

    virtual HRESULT GetKeywords(CString &csKeywords)=0;
    virtual void SetKeywords(LPCTSTR keywords)=0;
    Scorm *GetScorm() {return &scorm;}

    virtual GenericContainer* GetCurrentContainer() {return current != -1 ? subContainer[current] : NULL;}

    virtual void SetPageDimension(int _width, int _height);

    virtual void Zoom()=0;
    /*** draw the document structure as tree ***/

    CString GetNodeName() {return nodeName;}
    void ClearNodeName() {nodeName = "";}
    CString &GetNodePath() {return nodePath;}

    /*** management of container and object hierachy ***/
    virtual float GetObjectOrder(UINT id);
    virtual float GetPushBackObjectOrder();
    virtual float GetEndObjectOrder();
    virtual float GetFirstObjectOrder();
    virtual float GetNewObjectOrder(int pos);
    virtual float GetNextObjectOrder(UINT id);
    virtual void  ReOrderObjects();
    //virtual GenericObject* GetObjectWithOrder(float order);

    virtual void InsertContainerCurrent(GenericContainer* container);
    virtual void InsertContainerBegin(GenericContainer* container);
    virtual void InsertContainerEnd(GenericContainer* container);

    virtual GenericObject* GetObject(UINT id);
    virtual GenericObject* GetGroup(UINT id);
    virtual GenericContainer* GetContainer(UINT id);
    virtual int GetNbrOfPages() {return 0;}
    virtual int GetPageNumber(UINT nId, bool &bFound) {return 0;}

    virtual GenericContainer* GetFirstPage();
    virtual GenericContainer* GetLastPage();
    virtual GenericContainer* GetPrevious(UINT id);
    virtual GenericContainer* GetNext(UINT id);
    
    virtual bool SelectContainer(UINT nContainerId);
    virtual GenericContainer *GetParent(UINT nId);
    virtual void SelectFirstContainer();
    virtual bool RenameContainer(UINT nContainerId, LPCTSTR szContainerName);
    virtual bool SaveContainer(UINT nContainerId, LPCTSTR szFileName);

    virtual GenericObject* DeleteObject(UINT id);
    virtual void DeleteAll() {}

    /*** save functions ***/
    virtual void SaveContainer(FILE *fp, LPCTSTR path, int level, bool all) = 0;
    virtual void SaveMetadata(CFileOutput *pLmdFile) = 0;

    /*** AOF Recording ***/
    virtual void ResetRecording() = 0;

    void SetThumbNumber(int number) {thumbNumber = number;}
    int GetThumbNumber() {return thumbNumber;}

    virtual bool HasChanged() = 0;
    virtual void SetChanged(bool changed) {m_bContainerChanged = changed;}
    virtual int GetMaxWidth(int &actMax);
    virtual int GetMaxHeight(int &actMin);
#ifdef _ImageMagick
    virtual void InsertImage(MagickLib::Image *image, _TCHAR *filename, int x, int y, bool single=true) = 0;
#endif
    virtual void InsertImage(_TCHAR *filename, _TCHAR *path, int x, int y, bool single=true) = 0;

    virtual void DrawWhiteboard(CRect &rcWhiteboard, CDC *pDC) = 0;
    virtual void DrawThumbWhiteboard(CRect rcWhiteboard, CDC *pDC) = 0;
    virtual void AddToStructureTree(CDocumentStructureView *pDocStructTree, UINT nParentId) = 0;
    virtual void Activate() = 0;
    virtual void DeActivate() = 0;

    virtual void ResetFirstInserted() = 0;

    virtual void CollectPages(CArray<Page *, Page *> &pageList) = 0;
 };
}

#endif
