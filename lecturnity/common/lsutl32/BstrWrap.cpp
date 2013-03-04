// ( C ) 2007 ABBYY Software. All rights reserved.
// ABBYY FineReader Engine 9.0 Sample

// Helper functions for BSTR handling

#include "BstrWrap.h"

void CBstr::freeBuffer()
{
	if( string != 0 ) {
		::SysFreeString( string );
	}
}

BSTR Concatenate( const wchar_t* str1, const wchar_t* str2 )
{
	BSTR bstr = ::SysAllocStringLen( 0, wcslen( str1 ) + wcslen( str2 ) ); 
	if( bstr != 0 ) {
		wcscpy( bstr, str1 );
		wcscat( bstr, str2 );
	}
	return bstr;
}



