     
#include "globals.h"
#include "error.h"
#include <windows.h>

namespace PhysX
{
	cErrorStream m_ErrorStream;
}

bool g_bIgnoreErrors = false;

void PhysX::cErrorStream::reportError ( NxErrorCode code, const char* szMessage, const char* szFile, int iLine )
{	return;

	if ( g_bIgnoreErrors )
		return;

	MessageBox ( NULL, szMessage, "Dark Physics Runtime Error", MB_OK );
	assert ( 0 );
//vs9	exit ( 1 );
}

NxAssertResponse PhysX::cErrorStream::reportAssertViolation ( const char* szMessage, const char* szFile, int iLine )
{
	assert ( 0 );
	return NX_AR_CONTINUE;
}

void PhysX::cErrorStream::print ( const char* szMessage )
{
	MessageBox ( NULL, szMessage, "Dark Physics Runtime Error", MB_OK );
	assert ( 0 );
//vs9 exit ( 1 );
}

bool dbPhyCheckScene ( void )
{
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid scene for the simulation", "", 0 );
		return false;
	}

	return true;
}