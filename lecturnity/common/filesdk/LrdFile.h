#ifndef __RECORDINGPARAMETER__
#define __RECORDINGPARAMETER__

#include "export.h"
#include "LrdSgml.h"
#include "LmdFile.h"
#include "WhiteboardStream.h"
#include "lutils.h"



// Keywords for 'Standard Navigation'
#define NAVIGATION_STATES 7

#define NUM_CODEPAGE_UTF8 10646

const CString CS_NAVIGATION_NODE = _T("NAVIGATION");
const CString CS_NAVIGATION_SUBNODES[] = {
   _T("CONTROLBAR"), 
   _T("STANDARDBUTTONS"), 
   _T("TIMELINE"), 
   _T("TIMEDISPLAY"), 
   _T("DOCUMENTSTRUCTURE"), 
   _T("PLAYERSEARCHFIELD"), 
   _T("PLAYERCONFIGBUTTONS")
};
const CString CS_NAVIGATION_STATES[] = {
   _T("enabled"),     /* 0 */
   _T("disabled"),    /* 1 */
   _T("hidden")       /* 2 */
};

#define E_EXPORT_FAILED             _HRESULT_TYPEDEF_(0x80ef0001L)
#define E_EXPORT_CANCELLED          _HRESULT_TYPEDEF_(0x80ef0002L)
#define E_EXPORT_COPY_FAILED        _HRESULT_TYPEDEF_(0x80ef0003L)
#define E_EXPORT_READ_WB_ERROR      _HRESULT_TYPEDEF_(0x80ef0004L)
#define E_EXPORT_WRITE_WB_ERROR     _HRESULT_TYPEDEF_(0x80ef0005L)
#define E_EXPORT_ACCESS_WB_ERROR    _HRESULT_TYPEDEF_(0x80ef0006L)
#define E_EXPORT_ACCESS_ERROR       _HRESULT_TYPEDEF_(0x80ef0007L)

class CopyFileStruct {
public:
    enum CopyFileType {
        UNDEFINED = 0,
        WB = 1,
        AUDIO = 2,
        METADATA = 3,
        VIDEO = 4,
        CLIPS = 5
    };
   CopyFileStruct();
   ~CopyFileStruct() {}

   HRESULT Fill(LPCTSTR tszOldLrdPath, LPCTSTR tszNewLrdPath, 
                LPCTSTR tszNewFilePrefix, LPCTSTR tszFilename, CopyFileType nType);
   CString &GetSourceFileName() {return m_csOldFilename;}
   CString &GetTargetFileName() {return m_csNewFilename;}
   CopyFileType GetType() {return m_nType;}
   __int64 GetFileSize() {return m_nFileSize;}

private:
    CString m_csOldFilename;
    CString m_csNewFilename;
    __int64 m_nFileSize;
    CopyFileType m_nType;
};

class CopyProgressStruct {
public:
    CopyProgressStruct(ExportProgressListener *pExportProgress, __int64 nCompleteFileSize, 
                       __int64 nByteOffset) {
        m_pExportProgress = pExportProgress;
        m_nCompleteFileSize = nCompleteFileSize;
        m_nByteOffset = nByteOffset;
    }
    ~CopyProgressStruct() {}

public:
    ExportProgressListener *m_pExportProgress;
    __int64 m_nCompleteFileSize;
    __int64 m_nByteOffset;
};

class FILESDK_EXPORT CLrdFile
{
public:
   CLrdFile();
   ~CLrdFile();

private:
   // set member variables to default values
   void Init();
   // reset member variables
   void Clear();

public:
   // open lrd-file, create sgml structure
   bool Open(LPCTSTR tszFilename);

   // write sgml structure
   // BUGFIX 5108: fixed error handling - return bool result (was void)
   bool Write(LPCTSTR tszFilename);

public:
   // Read from Sgml: The first offset entry is the audio offset
   int GetAudioOffset();
   // opens the file and determines its length with IMediaDet
   int RetrieveAudioLength();
   // create new video stream from sgml structure and return
   CVideoStream *CreateVideoStream();
   // Read from Sgml: The second offset entry is the video offset
   int GetVideoOffset();
   // create new clip stream from sgml structure and return
   CClipStream *CreateClipStream();
   // create new whiteboard stream from sgml structure and return
   CWhiteboardStream *CreateWhiteboardStream(bool bOpen = true);
   // create new metadata from sgml structure and return
   CMetadataStream *CreateMetadata();

   // PZI: set whiteboard helper values; adds whiteboard helper if helper does not exist
   bool SetWhiteboardHelper(CString evqFilename, CString objFilename, int resolutionWidth, int resolutionHeight);

   // PZI: returns true if document contains pure screengrabbing only 
   bool IsDenverDocument();
   
   // Returns true if document will be displayed in simulation mode
   bool IsSimulationDocument();

public:
   // get and set functions for member variables
   LPCTSTR GetFilename() { return m_csFilename; }
   void SetFilename(LPCTSTR filename) { m_csFilename = filename; }

   LPCTSTR GetRecordPath() { return m_csRecordPath; }
   void SetRecordPath(LPCTSTR recordPath) { m_csRecordPath = recordPath; }
   
   LPCTSTR GetMetadataFilename() { return m_csMetadataFilename; }
   // PZI: set metadata filename (in member variable and SGML)
   void SetMetadataFilename(LPCTSTR metadataFilename);

   LPCTSTR GetAudioFilename() { return m_csAudioFilename; }
   // PZI: set audio filename (in member variable and SGML)
   void SetAudioFilename(LPCTSTR audioFilename);

   LPCTSTR GetVideoFilename() { return m_csVideoFilename; }
   // PZI: set video filename (in member variable and SGML)
   // note: will not video entry if not existent
   void SetVideoFilename(LPCTSTR tszVideoFilename);

   LPCTSTR GetStillImageFilename() { return m_csStillImageFilename; }
   // GG: set still image filename (in member variable and SGML)
   // note: will not still image entry if not existent
   void SetStillImageName(LPCTSTR tszStillImageName);

   // PZI: set clip filename of clip at specified position (in member variable and SGML) - starting at position 1
   void SetClipFilename(int iClipPosition, LPCTSTR csClipFilename);
   // PZI: set filename of still image
   void SetStillImage(LPCTSTR stillImageFilename);

   LPCTSTR GetEvqFilename() { return m_csEvqFilename; }
   // PZI: set EVQ filename (in member variable and SGML)
   // note: will not add whiteboard helper if not existent
   void SetEvqFilename(LPCTSTR evqFilename);

   LPCTSTR GetObjFilename() { return m_csObjFilename; }
   // PZI: set OBJ filename (in member variable and SGML)
   // note: will not add whiteboard helper if not existent
   void SetObjFilename(LPCTSTR objFilename);
   
   // return the CodePage used while recording
   int GetCodePage() { return m_nCodePage; }

   // 'standard navigation' states
   // (0: enabled; 1: disabled; 2: hidden)
   int* GetNavigationStates() {return m_anNavigationStates;}
   // "Put" method returns true if successful, otherwise false.
   bool PutNavigationStates(int* anNavigationStates);

   // PZI: get filename prefix used to create other filenames
   CString GetFilenamePrefix();

   // PZI: setter/getter for clip structuring process status
   // clipNr starting at 1 for first clip
   // setter will manipulate SGML
   void SetClipStructureAvailable(int clipNr, bool available);
   bool GetClipStructureAvailable(int clipNr);
   void SetClipImagesAvailable(int clipNr, bool available);
   bool GetClipImagesAvailable(int clipNr);
   void SetClipFulltextAvailable(int clipNr, bool available);
   bool GetClipFulltextAvailable(int clipNr);
   int GetNumberOfClips();
   HRESULT CopyDocumentTo(CString csTargetLrd, ExportProgressListener *pProgress = NULL);
   HRESULT DeleteFiles();
   
private:
   void SetClipFlagAvailable(int iClipNr, LPCTSTR tszFlagName, bool bAvailable);
   bool GetClipFlagAvailable(int iClipNr, LPCTSTR tszGlagName);

   // functions needed for complete document copy
   HRESULT ChangeTags(CArray<CopyFileStruct *, CopyFileStruct *> &aCopyFile);
   HRESULT AdaptRelativePathsInObj(CString csOldLrdPath, CString csNewLrdPath, CString csNewLrdPrefix);
   HRESULT AdaptRelativePathsInObj(SgmlFile &objFile, LPCTSTR tszOldlrdPath, LPCTSTR tszNewLrdPath, 
                                bool &bSomethingChanged);
   HRESULT AdaptThumbEntries(CString csNewLrdPrefix);
   HRESULT FillCopyFilesStruct(CString csOldLrdPath, CString csNewLrdPath, CString csNewLrdPrefix, 
                            CArray<CopyFileStruct *, CopyFileStruct *> &aCopyFiles, ExportProgressListener *pProgress);
   
   HRESULT FillFileArray(CArray<CString, CString> &aDeleteFiles);
   HRESULT CopyFileWithProgress(CopyFileStruct *pFileToCopy, __int64 nTotalSize, __int64 nBytesOffset, 
                             ExportProgressListener *pProgress);
   HRESULT CopyFilesOfType(CArray<CopyFileStruct *, CopyFileStruct *> &aCopyFiles, 
                        ExportProgressListener *pProgress, UINT nType);


private:
   // parse helper list to extract streams
   void ParseHelperList(SgmlElement *helperTag);
   void ParseVideoAndClipList(SgmlElement *videoTag);

protected:
 
   CLrdSgml m_lrdSgml;  // lrd's sgml structure

   // file name and path
   CString m_csFilename;
   CString m_csRecordPath;

   CString m_csMetadataFilename;
   CString m_csAudioFilename;
   CString m_csVideoFilename;
   CString m_csStillImageFilename;
   CString m_csEvqFilename;
   CString m_csObjFilename;
   CString m_csPageDimension;
   CStringArray m_aClipFilenames;
   CArray<int, int> m_aClipTimestamps;
   int     m_nCodePage;
   int*    m_anNavigationStates;
};


#endif
