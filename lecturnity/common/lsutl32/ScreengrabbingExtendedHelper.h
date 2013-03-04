#pragma once

// Why is it needed to specify this in every header??
#ifdef _LSUTL32_DLL
   #define LSUTL32_EXPORT __declspec(dllexport)
#else 
   #ifdef _LSUTL32_STATIC
      #define LSUTL32_EXPORT 
   #else
      #define LSUTL32_EXPORT __declspec(dllimport)
   #endif
#endif

#include "LrdFile.h"
#include "lutils.h"
#include "LiveContextElementInfo.h"

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

// copied from imc\native\iohook\iohook.h to avoid package dependencies
#define MH_POINTER         0x00000000
#define MH_LEFT_BUTTON     0x00000001
#define MH_RIGHT_BUTTON    0x00000002
#define MH_MIDDLE_BUTTON   0x00000004
#define MH_BUTTON_DOWN     0x00000001
#define MH_BUTTON_UP       0x00000002
#define MH_BUTTON_DBLCLICK 0x00000004
#define MH_MOVE            0x00000010
#define MH_DRAG            0x00000020

#define MH_TELE            0x00000100
#define MH_REM             0x00000101

// user actions to create click pages and add telepointer
typedef struct{
   // frame number in avi clip
   int iFrameNr;
   // timestamp
   int iTimeMsec;

   // click coordinate
   int iXcoord;
   int iYcoord;

   // type of action
   // mouse move: MH_POINTER
   // mouse click: MH_LEFT_BUTTON, MH_RIGHT_BUTTON, MH_MIDDLE_BUTTON
   DWORD dwEventType;

   // action data
   // mouse move: MH_MOVE, MH_DRAG
   // mouse click: MH_BUTTON_DOWN, MH_BUTTON_UP, MH_BUTTON_DBLCLICK
   // undefined: -1
   DWORD dwAction;

   // LiveContext data
   CLiveContextElementInfo *pElementInfo;

   // upper left coordinate of recorded area
   // used as offset to translate absolute LiveContext coordinates to relative ones
   int iLiveContextXoffset;
   int iLiveContextYoffset;
} UserAction;

// one element of structure including time and list of words for full text search
class LSUTL32_EXPORT CStructureElement {
public:
   CStructureElement(int timestampMsec, bool bClickPage=false, UserAction *pAction = NULL) {  
      timestamp = timestampMsec;
      m_bClickPage = bClickPage;
      if (pAction) {
         m_action.dwAction = pAction->dwAction;
         m_action.dwEventType = pAction->dwEventType;
         m_action.iFrameNr = pAction->iFrameNr;
         m_action.iTimeMsec = pAction->iTimeMsec;
         m_action.iXcoord = pAction->iXcoord;
         m_action.iYcoord = pAction->iYcoord;
         m_action.iLiveContextXoffset = pAction->iLiveContextXoffset;
         m_action.iLiveContextYoffset = pAction->iLiveContextYoffset;
         m_action.pElementInfo = pAction->pElementInfo;
      } else {
         m_action.dwAction = -1;
      }

      textObjects = new CList<DrawSdk::Text*,DrawSdk::Text*>();
   }
   ~CStructureElement() {
      // TODO: check cleanup
      if(textObjects)
      {
         POSITION pos = textObjects->GetHeadPosition();
         while(pos)
         {
            DrawSdk::Text *pText = textObjects->GetNext(pos);
            SAFE_DELETE(pText);
         }
         SAFE_DELETE(textObjects);
      }
   }
   bool hasAction() { return m_action.dwAction != -1; }

   int timestamp; // msec
   bool m_bClickPage; // page create as result of mouse click action
   UserAction m_action; // page create as result of mouse click action
   CList<DrawSdk::Text*,DrawSdk::Text*> *textObjects;
};

// clip info with structure
class LSUTL32_EXPORT CClipInfoStructured {
public:
   CClipInfoStructured(CClipInfo *pClipInfo, int clipId) {
      clipInfo = pClipInfo;
      size.cx = 0;
      size.cy = 0;
      m_iClipId = clipId;
   }
   ~CClipInfoStructured()
   {
      POSITION pos = structure.GetHeadPosition();
      while (pos != NULL) { 
         delete structure.GetNext(pos);
      }
   }

   int GetId() { return m_iClipId; }
   int GetOffset() { return clipInfo->GetTimestamp(); }
   int GetWidth() {
      if(size.cx == 0)
         VideoInformation::GetVideoSize(clipInfo->GetFilename(), size);
      return size.cx;
   }
   int GetHeight() {
      if(size.cy == 0)
         VideoInformation::GetVideoSize(clipInfo->GetFilename(), size);
      return size.cy;
   }

public:
   CClipInfo *clipInfo;

   CList<CStructureElement*,CStructureElement*> structure;

private:
   // clip resolution
   SIZE size;
   int m_iClipId;
};

// read and write structuring data from sge-file
CArray<LMisc::StructuringValues>* ReadStructuringValues(CString csFilename);
void WriteStructuringValues(CString csFilename, CArray<LMisc::StructuringValues> *paStructuringValues );

// read user actions from sga-file
CArray<UserAction>* ReadUserActions(CString csFilename);

// read Live Context Data from sga-file
CArray<CLiveContextElementInfo*, CLiveContextElementInfo*>* ReadLiveContextElementInfos(CString csFilename);

// TODO maybe transferred to DrawSdk::Text
DrawSdk::Text *CreateTextObject(CString text, int posX=0, int posY=0, int width=0, int height=0, int fontSize=0);


// Sampler renders a clip stream and computes structuring values
class Sampler : public CBaseVideoRenderer{
protected:
   unsigned char *prevImage;
   BITMAPINFOHEADER bmih;; 

public:
   int m_iFrameCounter;
   CArray<LMisc::StructuringValues> *m_paStructuringValues;

public:
   Sampler( IUnknown* unk, HRESULT *hr);
   ~Sampler();
   HRESULT CheckMediaType(const CMediaType *media );
   // renders a frame and computes structuring values
   HRESULT DoRenderSample(IMediaSample *sample);
};

// write poster frames of video at given timestamps
// [in] tszVideoFileName: clip filename with path
// [in] tszImageFileNamePrefix: prefix of image filename with path
// [in] timestamps: list of timestamps in msec
// [in] offset: clip offset related to recording (used to create filenames with timestamps related to recording instead of clip)
HRESULT  LSUTL32_EXPORT WritePosterFrames(LPCTSTR tszVideoFileName, LPCTSTR tszImageFileNamePrefix, CList<int> *timestamps, int clipOffset, HRESULT (*fnProgress)() = NULL );
