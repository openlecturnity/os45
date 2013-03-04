/*********************************************************************

 program : la_document.h
 authors : Gabriela Maass
 date    : 18.06.2002
 desc    : 

 **********************************************************************/

#ifndef __ASSISTANT_DOCUMENT__
#define __ASSISTANT_DOCUMENT__

#ifdef WIN32
#  pragma warning( disable: 4786)
#endif

#include "la_imagelist.h"

#include "mlb_folder.h"
#include "mlb_page.h"
//#include "mlb_replay.h"

namespace ASSISTANT {

  class Document : public Folder {
  protected:
    CString 
       m_csDocumentName;
    CString 
       m_csDocumentPath;
    CString
       lastSavedDocument;
    bool 
        m_bIsLoaded;
    bool
        m_bIsEmptyDocument;

  public:


    Document(uint32 id, float _zoomFakt=1.0);
    Document(uint32 id, LPCTSTR _filename, float _zoomFakt=1.0);   
    ~Document();

    /*** Functions derived from GenericObject ***/
    virtual int  GetType() {return GenericObject::DOCUMENT;}
    virtual void SetName(LPCTSTR _name);
    void SetDocumentFilename(LPCTSTR filename);
    CString &GetDocumentName() {return m_csDocumentName;}
    CString &GetDocumentPath() {return m_csDocumentPath;}

    /*** Functions derived from GenericContainer ***/
    //    virtual void SaveAsAOF(FILE *fp);
    virtual void SaveContainer(FILE *_fp, LPCTSTR path, int _level, bool all=false);


	CString &GetContainerName(){return containerName;}

    /*** own functions ***/
    HRESULT Load(CProgressDialogA *pProgress = NULL);
    HRESULT LoadInformation();
    HRESULT CreateLoadThread();
    static UINT LoadThread(LPVOID pParam);
    void UnLoad();

    /*** own functions called by Project ***/
    bool Save();
    bool SaveAs(LPCTSTR filename);
    bool WriteLSD(LPCTSTR  filename, bool bChangeFilename=true);
#ifndef _STUDIO
    bool Open(const _TCHAR *filename, bool bDoLoad);
#endif
    bool Close(bool bAskForSave);
    void GetLastSavedDocument(CString &csLastSavedDocument);

    /*** functions concerned active page ***/
    void RaiseObjects();
    void LowerObjects();

    virtual bool HasChanged();
    void SetAllUnchanged();

    void Undo();
    void CutObjects();
    void CopyObjects();
    void PasteObjects();

    void AddTo(Document *dest);

    bool IsLoaded() {return m_bIsLoaded;}
    bool IsEmpty() {return m_bIsLoaded ? (subContainer.IsEmpty() ? true : false) : m_bIsEmptyDocument;}

  };

}

#endif
