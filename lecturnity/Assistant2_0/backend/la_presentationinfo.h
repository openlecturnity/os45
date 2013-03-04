#ifndef __ASSISTANT_PRESENTATIONINFO__
#define __ASSISTANT_PRESENTATIONINFO__

#include "la_cliplist.h"
#include "la_thumbs.h"
#include "..\ProgressDialog.h"
#include "LmdSgml.h"
#include "LrdSgml.h"

namespace ASSISTANT
{

   class PresentationInfo
   {
   private:
      
      CString title_;
      CString author_;
      CString creator_;
      CStringArray keywords_;
      CString recorddate_;
      CString recordtime_;
      CString recordlength_;
      CString audioSamplerate_;
      CString audioSamplewidth_;
      float m_fVideoFramerate;
      CString videoCodec_;
      CString videoQuality_;
      CString videoKeyframes_;
      CString videoDatarate_;
      unsigned int iCodepage_;

      bool m_bClipIsStructured;
      bool m_bClipIsSearchable;
      bool m_bIsDenverDocument;
      
      PLAYER::Thumbs 
         *thumbs_;

      bool isInitialized_;
      CString filename_;
      ClipList *clipList_;
      int presentationCount_;
      bool isVideo_;

      CStringArray m_slFileList;

      void ExtractMetadataFromLmd(CString &csLmdFile);
      void ExtractGeneralMetadata(CLmdSgml *pLmdSgml);
      void ExtractInfosFromClip(CLmdSgml *pLmdSgml);
      void ExtractInfosFromAudio(CLmdSgml *pLmdSgml);
      void ExtractInfosFromVideo(CLmdSgml *pLmdSgml);
      void ExtractMetadataFromLrd(CString &csLmdFile);
      void ExtractMetadataFromLep(CString &csLmdFile);

      bool GetImageFileNames(LPCTSTR szObjectFilename);

   public:
      PresentationInfo();
      ~PresentationInfo();
      CString &GetFilename();
      void SetFilename(LPCTSTR filename);
      int GetCount();
      void SetCount(int presentationCount);

      bool IsModified();

      void SetCodepage(unsigned int iCodepage) {iCodepage_ = iCodepage;}
      unsigned int GetCodepage() {return iCodepage_;}

      CString &GetTitle();
      void SetTitle(LPCTSTR title);
      CString &GetAuthor();
      void SetAuthor(LPCTSTR author);
      CString &GetCreator();
      void SetCreator(LPCTSTR creator);
      CString &GetKeywords();
      void SetKeywords(LPCTSTR tszKeywords);
      CString &GetRecorddate();
      void SetRecorddate(LPCTSTR tszRecorddate);
      CString &GetRecordtime();
      void SetRecordtime(LPCTSTR tszRecordtime);
      CString &GetRecordlength();
      void SetRecordlength(LPCTSTR tszRecordlength);
      CString &GetAudioSamplerate();
      void SetAudioSamplerate(LPCTSTR tszAudioSamplerate);
      CString &GetAudioSamplewidth();
      void SetAudioSamplewidth(LPCTSTR tszAudioSamplewidth);
      float GetVideoFramerate();
      void SetVideoFramerate(float fVideoFramerate);
      CString &GetVideoCodec();
      void SetVideoCodec(LPCTSTR videoCodec);
      CString &GetVideoQuality();
      void SetVideoQuality(LPCTSTR videoQuality);
      CString &GetVideoKeyframes();
      void SetVideoKeyframes(LPCTSTR videoKeyframes);
      CString &GetVideoDatarate();
      void SetVideoDatarate(LPCTSTR videoDatarate);
      void SetVideoEnabled(bool isVideo) {isVideo_ = isVideo;}

      bool IsDenverDocument() { return m_bIsDenverDocument; }
      bool IsClipStructured() { return m_bClipIsStructured; }
      bool IsClipSearchable() { return m_bClipIsSearchable; }
      void SetDenverDocument( bool bIsDenverDocument )
      { 
         m_bIsDenverDocument = bIsDenverDocument;
         if( bIsDenverDocument )
         {
            m_bClipIsStructured = false;
            m_bClipIsSearchable = false;
         }
      }
      void SetClipStructured( bool bClipIsStructured )
      {
         m_bClipIsStructured = bClipIsStructured;
         if( bClipIsStructured )
            m_bIsDenverDocument = false;
         else
            m_bClipIsSearchable = false;
      }
      void SetClipSearchable( bool bClipIsSearchable )
      {
         m_bClipIsSearchable = bClipIsSearchable;
         if( bClipIsSearchable ) {
            m_bClipIsStructured = true;
            m_bIsDenverDocument = false;
         }
      }

      ClipList *GetClipList();
      bool IsClipListEmpty();

      void Initialize();

      bool IsFilenameEqual(LPCTSTR filename);
      void FillMetainfoList(CString &metainfoList);
      void ReWriteLmdFile();
      void ReWriteLrdFile();
      void ParseAndReplaceLmdMetainfo(CLmdSgml *pLmdSgml);
      void ParseAndReplaceLrdMetainfo(CLrdSgml *pLrdSgml);
      SgClip *FindClip(int clipNumber);
      SgClip *FindClip(LPCTSTR clipName, int startAt);
      void RemoveClip(int clipNumber);
      void AddClip(SgClip *clip);
      void ReOrderClipList();
      void MoveThumbs(PLAYER::Thumbs *thumbs);
      int GetNextClipNumber();

      bool FillFileList();
      bool FillFileList(CStringArray &aFileList);
      void EmptyFileList();
      int GetFileCount();
      bool CopyFiles(LPCTSTR destDirectory, float fFileStep, float &fActualPosition, bool &bOverwriteAll, bool &bCancelCopy, CProgressDialogA *pProgress);

      
      void StartReplay();
      void StartConvert(bool bDirectMode=false, bool bSelectFormat=false);
      void StartEdit();
      void StartExport();

private:
      static UINT FinalizeThreadLauncher(LPVOID pParam);
      HRESULT FinalizeThread(bool bUseProfile=false, bool bSelectFormat=false, int iSelectedFormat = -1);
   };
}

#endif