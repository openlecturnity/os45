// ( C ) 2008 ABBYY Software. All rights reserved.
// ABBYY FineReader Engine 9.0 Sample

// Simple exception class

#ifndef AbbyyException_h
#define AbbyyException_h

#include "BstrWrap.h"
#include "SafePtr.h"

class CAbbyyException {
public:
	// Constructor
	CAbbyyException( const wchar_t* _description ) { description = _description ; }
	CAbbyyException( const CAbbyyException& other ) : description( other.description ) {}

	// Returns error information.
	const wchar_t* Description() { return description; }

	// Copy operator
	CAbbyyException& operator = ( const CAbbyyException& other ) { description = other.description; } 

private:
	CBstr description;
};

// Check HRESULT. If not succeed try to obtain error description and throw exception
inline void CheckResult( HRESULT res )
{
	if( FAILED( res ) ) {
		// Try to obtain error info
		CSafePtr<IErrorInfo> errorInfo;
		if( ::GetErrorInfo( 0, &errorInfo ) == S_OK ) {
			// Get error description and throw exception
			CBstr description;
			errorInfo->GetDescription( &description );
			throw CAbbyyException( description );
		} else {
			// Throw exception without description
			throw CAbbyyException( L"Unknown error" );
		}
	}
}

#endif // AbbyyException_h