//////////////////////////////////////////////////////////////////////
//
// vmlfont.cpp: Implementation of the class CVmlFont.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "vmlfont.h"

//////////////////////////////////////////////////////////////////////
// class CVmlFont: Constructor/Destructor
//////////////////////////////////////////////////////////////////////

CVmlFont::CVmlFont()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   Init();
}

//////////////////////////////////////////////////////////////////////

CVmlFont::CVmlFont( CString strFontName )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   Init();

   m_strFontName.Format( strFontName );
}

//////////////////////////////////////////////////////////////////////

CVmlFont::CVmlFont( CString strFontName,
                    CString strTtfName )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   Init();

   m_strFontName.Format( strFontName );
   m_strTtfName.Format( strTtfName );
}

//////////////////////////////////////////////////////////////////////

CVmlFont::CVmlFont( CString strFontName,
                    CString strTtfName,
                    CString strFontWeight, 
                    CString strFontStyle )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   Init();

   m_strFontName.Format( strFontName );
   m_strTtfName.Format( strTtfName );
   m_strFontWeight.Format( strFontWeight );
   m_strFontStyle.Format( strFontStyle );
}

//////////////////////////////////////////////////////////////////////

CVmlFont::~CVmlFont()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   Clear();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CVmlFont::Init()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   m_strFontName.Format( "none" );
   m_strTtfName.Format( "none" );
   m_strFontWeight.Format( "normal" );
   m_strFontStyle.Format( "normal" );
}

//////////////////////////////////////////////////////////////////////

void CVmlFont::Clear()
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   m_strFontName.Empty();
   m_strTtfName.Empty();
   m_strFontWeight.Empty();
   m_strFontStyle.Empty();
}

//////////////////////////////////////////////////////////////////////

bool CVmlFont::Equals( CVmlFont* pVmlFont )
{
   AFX_MANAGE_STATE( AfxGetStaticModuleState() );

   if ( ( pVmlFont->GetFontName() == m_strFontName )
        && ( pVmlFont->GetTtfName() == m_strTtfName )
        && ( pVmlFont->GetFontWeight() == m_strFontWeight )
        && ( pVmlFont->GetFontStyle() == m_strFontStyle ) )
   {
      return true;
   }
   else
   {
      return false;
   }

}

//////////////////////////////////////////////////////////////////////
