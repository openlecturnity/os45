// DocumentStructure.h
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCUMENT_STRUCTURE_H__FF4DC366_939F_4F4B_84F9_246F884E9BDD__INCLUDED_)
#define AFX_DOCUMENT_STRUCTURE_H__FF4DC366_939F_4F4B_84F9_246F884E9BDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

enum DocumentStructureLayout{
   Thumbnail = 0,
   Compact = 1
};

// maybe I should rename it in CDocumentStructureHelper
class CDocumentStructure
{
public:
   virtual void SetName(void *pData, CString csText) = 0;
   virtual CString GetName(void *pData) = 0;
   virtual UINT GetType(void *pData) = 0;
   virtual void DrawThumbWhiteboard(void *pData, CRect rcThumb, CDC* pDC) = 0;
   virtual UINT GetID(void *pData) = 0;
   virtual void GetKeywords(void *pData, CString & csKeywords) = 0;
   virtual void SetKeywords(void *pData, CString csKeywords) = 0;
   virtual void LoadDocument(void *pData) = 0;
   virtual bool ShowClickIcon(void *pData) {return false;}


   //virtual void SetName(void* pContainer, CString csText) = 0;
   //virtual CString GetName(void* pContainer) = 0;
   //virtual void DrawThumbWhiteboard(void* pContainer, CRect rcThumb, CDC* pDC) = 0;
   //virtual UINT GetID(void* pContainer) = 0;
   //virtual int GetRowHeight(CDC* pDC, CXTPReportRow* pRow) = 0;
   //virtual void GetKeywords(CString & csKeywords) = 0;
   //virtual void SetKeywords(CString csKeywords) = 0;
};

#endif //AFX_DOCUMENT_STRUCTURE_H__FF4DC366_939F_4F4B_84F9_246F884E9BDD__INCLUDED_