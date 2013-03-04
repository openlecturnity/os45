// ( C ) 2007 ABBYY Software. All rights reserved.
// ABBYY FineReader Engine 9.0 Sample

// Helper functions for BSTR handling

#ifndef StringHelpers_h
#define StringHelpers_h

#include <wtypes.h>

// Simple wrapper class for BSTR
class CBstr {
public:
	// Constructors
	CBstr() { string = SysAllocString( L"" ); }
	CBstr( const wchar_t* str ) { string = ::SysAllocString( str ); }
	CBstr( const CBstr& other ) { string = ::SysAllocString( other ); }
	
	// Destructor
	~CBstr() { freeBuffer(); }
	
	// Length of the string
	int Length() const { return wcslen( string ); }

	// Returns pointer to the internal buffer
	BSTR* GetBuffer() { freeBuffer(); return &string; }

	// Extractors
	operator wchar_t*() const { return string; }
	BSTR* operator &() { return &string; }

	// Operators
	CBstr& operator +=( const CBstr& other );
	CBstr operator + ( const CBstr& other );

	// Assignment operators
	CBstr& operator = ( const wchar_t* str );
	CBstr& operator = ( const CBstr& other );
	
	// Comparison operators
	bool operator != ( const CBstr& other ) { return wcscmp( *this, other ) != 0; }
	bool operator == ( const CBstr& other ) { return wcscmp( *this, other ) == 0; }

private:
	// Free string buffer
	void freeBuffer();
	BSTR string;
};

// Concatenates two wide-character strings ( e.g. BSTRs ) and returns result as BSTR. 
BSTR Concatenate( const wchar_t* str1, const wchar_t* str2 );

inline CBstr& CBstr::operator +=( const CBstr& other ) 
{ 
	*this = Concatenate( this->string, other ); 
	return *this; 
}

inline CBstr CBstr::operator + ( const CBstr& other ) 
{ 
	return Concatenate( *this, other ); 
}

inline CBstr operator + ( const CBstr& first, const CBstr& second )
{
	return Concatenate( first, second );
}

inline CBstr& CBstr::operator = ( const wchar_t* str ) 
{ 
	freeBuffer(); 
	string = ::SysAllocString( str ); 
	return *this;
}

inline CBstr& CBstr::operator = ( const CBstr& other ) 
{ 
	freeBuffer(); 
	string = ::SysAllocString( other ); 
	return *this;
}

#endif // StringHelpers_h

