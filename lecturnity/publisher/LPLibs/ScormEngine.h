// ScormEngine.h: Schnittstelle für die Klasse CScormEngine.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCORMENGINE_H__5731B95B_51D9_4ECD_9400_FE336305C6F8__INCLUDED_)
#define AFX_SCORMENGINE_H__5731B95B_51D9_4ECD_9400_FE336305C6F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZipArchive.h" // ZipArchive: CZipArchive

#define SCORM_LANGUAGE_SIZE            8
#define SCORM_VERSION_SIZE             32
#define SCORM_STATUS_SIZE              32
#define SCORM_GNRL_CE_CATALOG_SIZE     500  // CE = CatalogEntry
#define SCORM_GNRL_CE_ENTRY_SIZE       500
#define SCORM_CLSF_DESCRIPTION_SIZE    2000
#define SCORM_CLSF_KEYWORD_SIZE        2000
#define SCORM_PURPOSE_ARRAY_SIZE       8
#define SCORM_MAX_IDENTIFIER_LENGTH    100 // identifiers should not be longer
#define SCORM_CLSF_IDX_DISCIPLINE                  0
#define SCORM_CLSF_IDX_IDEA                        1
#define SCORM_CLSF_IDX_PREREQUISITE                2
#define SCORM_CLSF_IDX_EDUCATIONAL_OBJECTIVE       3
#define SCORM_CLSF_IDX_ACCESSIBILITY_RESTRICTIONS  4
#define SCORM_CLSF_IDX_EDUCATIONAL_LEVEL           5
#define SCORM_CLSF_IDX_SKILL_LEVEL                 6
#define SCORM_CLSF_IDX_SECURITY_LEVEL              7


// SCORMSETTINGS contains all the general settings which
// are not document-dependant. These are the settings which
// can be made in the GUI.
// Note: The char* members in this struct are meant to
// be char*, not _TCHAR* or wchar_t* (in order to always keep
// the size of the struct equal). This has to be taken into 
// account.
typedef struct
{
   // The size of the structure
   DWORD cbSize;
   // Language of the document to be packaged
   char szLanguage[SCORM_LANGUAGE_SIZE];
   // Version of the document to be packaged
   char szVersion[SCORM_VERSION_SIZE];
   // Status of the document to be packaged
   char szStatus[SCORM_STATUS_SIZE];
   // Should the full text be included as keywords in the SCORM package?
   bool bFulltextAsKeyword;
   // Does it cost anything to view the SCORM SCO?
   bool bCost;
   // Is the SCO copyrighted?
   bool bCopyright;
} SCORMSETTINGS;


/**
 *
 */
class CScormSettings2
{
public:
   CScormSettings2();
   ~CScormSettings2();

   HRESULT CopyClassificationDataTo(CScormSettings2 &pcormSettings2);

   HRESULT ReadRegistry();
   HRESULT WriteRegistry();

   CString GetRepresentation();
   HRESULT ParseFrom(CString& csRepresentation);

   // make ourself compatible to SCORMSETTINGS
   // the strings (pointers) will point to the locations in m_scormSettingsOld
   char *szLanguage;
   char *szVersion;
   char *szStatus;
   bool bFulltextAsKeyword;
   bool bCost;
   bool bCopyright;

   // The "catalog" string from "general" of the document to be packaged
   _TCHAR m_tszGnrlCeCatalog[SCORM_GNRL_CE_CATALOG_SIZE];
   // The "entry" string from "general" of the document to be packaged
   _TCHAR m_tszGnrlCeEntry[SCORM_GNRL_CE_ENTRY_SIZE];
   
   // The selected "purpose" index from "classification" of the document to be packaged
   int  m_nPurposeIndex;
   // The "description" string array from "classification" of the document to be packaged
   _TCHAR m_atszClsfDescription[SCORM_PURPOSE_ARRAY_SIZE][SCORM_CLSF_DESCRIPTION_SIZE];
   // The "keyword" string array from "classification" of the document to be packaged
   _TCHAR m_atszClsfKeyword[SCORM_PURPOSE_ARRAY_SIZE][SCORM_CLSF_KEYWORD_SIZE];


private:
   SCORMSETTINGS m_ScormSettingsOld;

   _TCHAR m_atszRegistryClsfNames[SCORM_PURPOSE_ARRAY_SIZE][SCORM_MAX_IDENTIFIER_LENGTH];
};


class CScormEngine;

class CScormZipCallback : public CZipActionCallback
{
public:
   CScormZipCallback(CScormEngine *pScormEngine);

   virtual void Init(LPCTSTR lpszFileInZip = NULL, LPCTSTR lpszExternalFile = NULL);
   virtual void SetTotal(DWORD uTotalToDo);
   virtual void CallbackEnd() {}
   virtual inline bool Callback(int iProgress);

private:
   CScormEngine *m_pScormEngine;
};

class CScormEngine
{
   friend class CScormZipCallback;

public:
	CScormEngine();
	virtual ~CScormEngine();

   UINT Init();
   UINT CleanUp();
   UINT Configure(HWND hWndParent, bool bDoNotSave);

   UINT ReadSettings();
   UINT WriteSettings();

   UINT GetSettings(_TCHAR *tszSettings, DWORD *pdwSize);
   UINT UseSettings(const _TCHAR *tszSettings);
   

   //
   // the get and set methods are used from (native) java side
   // internally (i.e. the dialogs) use direct access to the settings
   //

   UINT SetTargetFileName(const _TCHAR *tszTargetFile);
   UINT GetTargetFilePath(_TCHAR *tszTargetPath, DWORD *pdwSize);
   UINT SetIndexFileName(const _TCHAR *tszRealFileName, const _TCHAR *tszFileName);
   UINT AddResourceFile(const _TCHAR *tszRealFileName, const _TCHAR *tszFileName);
   UINT AddMetadataKeyword(const _TCHAR *tszKeyword);
   UINT AddGeneralKeyword(const _TCHAR *tszKey, const _TCHAR *tszValue);
   UINT AddFullTextString(const _TCHAR *tszFulltext);

   UINT GetLanguage(_TCHAR *tszLanguage, DWORD *pdwSize);
   UINT GetVersion(_TCHAR *tszVersion, DWORD *pdwSize);
   UINT GetStatus(_TCHAR *tszStatus, DWORD *pdwSize);
   UINT GetFullTextAsKeywords(BOOL *pbFullText);

   UINT CreatePackage(HWND hWndParent);
   UINT GetProgress(DWORD *pdwProgress);
   UINT Cancel();

public:
   CScormSettings2 m_scormSettings;

   CString m_csTargetFile;
   CString m_csRealIndexFileName;
   CString m_csIndexFileName;
   // Temporary directory
   CString m_csBaseTempPath;
   // Relative path to m_csBaseTempPath
   CString m_csRelTempPath;
   // m_csBaseTempPath + m_csRelTempPath
   CString m_csTargetDir;
   CArray<CString, CString> m_aSourceFiles;
   CArray<CString, CString> m_aTargetFiles;

   // Yes, I know, this is naughty. But: This class is not
   // exported, and there is no MFC equivalent to the set
   // class, so this does not do any harm at this position.
   std::set<CString> m_setFulltextWords;

   // Metadata keywords
   CArray<CString, CString> m_aKeywords;

   // General keywords for replacement in the XML templates
   CArray<CString, CString> m_aGeneralKeywords;
   CArray<CString, CString> m_aGeneralValues;

private:

   bool m_bSaveToRegistry; // in profile mode nothing should be saved

   UINT TCharFromCString(CString &csString, _TCHAR *tszTarget, DWORD *pdwSize);
   CString ConvertSlashes(const _TCHAR *tszFileName);
   void DefineScormKeywords();

   bool LoadXmlResource(CString &csXml, UINT nResource);

   DWORD m_dwProgress;
   DWORD m_dwCurrentFile;

   bool m_bCancelRequested;
};

#endif // !defined(AFX_SCORMENGINE_H__5731B95B_51D9_4ECD_9400_FE336305C6F8__INCLUDED_)
