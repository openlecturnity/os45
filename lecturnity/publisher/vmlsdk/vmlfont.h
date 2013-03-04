//////////////////////////////////////////////////////////////////////
//
// vmlfont.h: Interface for the class CVmlFont.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VMLFONT_H__INCLUDED_)
#define AFX_VMLFONT_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////

class CVmlFont
{
public:
   // Constructor/Destructor
   CVmlFont();
   CVmlFont( CString strFontName );
   CVmlFont( CString strFontName, 
             CString strTtfName );
   CVmlFont( CString strFontName, 
             CString strTtfName, 
             CString strFontWeight, 
             CString strFontStyle );
   ~CVmlFont();

   //methods:
   void Init();
   void Clear();

   bool Equals( CVmlFont* pVmlFont );

   // Set/Get methods
   void SetFontName(CString strFontName) { m_strFontName = strFontName; }
   void SetTtfName(CString strTtfName) { m_strTtfName = strTtfName; }
   void SetFontWeight(CString strFontWeight) { m_strFontWeight = strFontWeight; }
   void SetFontStyle(CString strFontStyle) { m_strFontStyle = strFontStyle; }
   CString GetFontName() { return m_strFontName; }
   CString GetTtfName() { return m_strTtfName; }
   CString GetFontWeight() { return m_strFontWeight; }
   CString GetFontStyle() { return m_strFontStyle; }

private:
   // member variables
   CString  m_strFontName;
   CString  m_strTtfName;
   CString  m_strFontWeight;
   CString  m_strFontStyle;
};

#endif // !defined(AFX_VMLFONT_H__INCLUDED_)
