/**********************************************************************

 program : mlb_folder.h
 authors : Gabriela Maass
 date    : 18.02.2000
 desc    : 

 **********************************************************************/

#ifndef __ASSISTANT_FOLDER__
#define __ASSISTANT_FOLDER__

#ifdef WIN32
#  pragma warning (disable: 4786)
#endif

#include "sgml.h"
#include "mlb_generic.h"
#include "mlb_page.h"

namespace ASSISTANT {
  //using namespace std;

  class Folder : public GenericContainer {
    protected:
      bool
         isOpen_;
      
    public:
      Folder(uint32 id, _TCHAR *_name=NULL);
      Folder(uint32 id, _TCHAR *szFolderNumber, GenericContainer *session, SGMLElement *root);
      ~Folder();


      virtual void AddToStructureTree(CDocumentStructureView *pDocStructTree, UINT nParentId);


      void InitTelepointers();

      void Reset();

      /*** Functions derived from GenericObject ***/
      virtual void SetName(LPCTSTR _name);
      virtual int GetType() {return ASSISTANT::GenericObject::FOLDER;}
      virtual HRESULT GetKeywords(CString &csKeywords);
      virtual void SetKeywords(LPCTSTR keywords);

      /*** Functions derived from GenericContainer ***/
      virtual void Zoom();
      void OpenFolder();
      virtual void SaveContainer(FILE *fp, LPCTSTR path, int level, bool all);
      virtual void SaveMetadata(CFileOutput *pLmdFile);

      virtual void DeleteAll();

      virtual int GetNbrOfPages();
      virtual int GetPageNumber(UINT nId, bool &bFound);

      Page *GoToFirstPage();
      Page *GoToLastPage();

      bool ContainPages();

      /*** own functions ***/

      virtual GenericObject* DeleteObject(uint32 id); 

      Page *GoToPrevPage(GenericContainer *active, bool *found, bool _jump);
      Page *GoToNextPage(GenericContainer *active, bool *found, bool _jump);
      bool PreviousPageAvailable();
      bool NextPageAvailable();

      virtual bool RenameContainer(UINT nContainerId, LPCTSTR szContainerName);
      
      Page *GoToPageWithID(const _TCHAR *slideID);

      GenericContainer* GetNextContainer(GenericContainer* obj);

      virtual void ResetRecording();
      virtual bool HasChanged();
      virtual void SetChanged(bool changed);

      void SetFolderState(bool isOpen) {isOpen_ = isOpen;}

      
      Page *GetActivePage();
      Folder *GetActiveFolder();

    
      void InsertEmptyFolder();
      void InsertEmptyPage(int iWidth, int iHeight, COLORREF clrBackground);
      void DuplicatePage(ASSISTANT::Page *pPage);

#ifdef _ImageMagick
      void InsertImage(MagickLib::Image *image);
      void InsertImage(_TCHAR *filename);
      virtual void InsertImage(MagickLib::Image *image, _TCHAR *filename, int x, int y, bool single=true);
#endif

      virtual void InsertImage(_TCHAR *filename, _TCHAR *path, int x, int y, bool single=true);

      virtual void DrawWhiteboard(CRect &rcWhiteboard, CDC *pDC);
      virtual void DrawThumbWhiteboard(CRect rcWhiteboard, CDC *pDC){return;};
      virtual void Activate();
      virtual void DeActivate();

      virtual void ResetFirstInserted();

      virtual void CollectPages(CArray<Page *, Page *> &pageList);

    };
}

#endif
