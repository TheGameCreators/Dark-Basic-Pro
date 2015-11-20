#ifndef _CSYSTEM_H_
#define _CSYSTEM_H_

#include "windows.h"

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifndef DARKSDK_COMPILE
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
#else
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#endif

#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void Constructor ( void );
DARKSDK void Destructor ( void );
DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr );
DARKSDK void PassCoreData( LPVOID pGlobPtr );
DARKSDK void RefreshD3D ( int iMode );

#ifdef DARKSDK_COMPILE
	void ConstructorSystem ( void );
	void DestructorSystem ( void );
	void SetErrorHandlerSystem ( LPVOID pErrorHandlerPtr );
	void PassCoreDataSystem( LPVOID pGlobPtr );
	void RefreshD3DSystem ( int iMode );
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// COMMANDS FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// memory available
DARKSDK int TMEMAvailable(void);
DARKSDK int DMEMAvailable(void);
DARKSDK int SMEMAvailable(void);

// global activity
DARKSDK void DisableEscapeKey(void);
DARKSDK void EnableEscapeKey(void);
DARKSDK void DisableSystemKeys(void);
DARKSDK void EnableSystemKeys(void);
DARKSDK void ExitPrompt(DWORD pString, DWORD pString2);

// checklist commands
DARKSDK void EmptyChecklist( void );
DARKSDK int ChecklistQuantity( void );
DARKSDK int ChecklistValueA( int iIndex );
DARKSDK int ChecklistValueB( int iIndex );
DARKSDK int ChecklistValueC( int iIndex );
DARKSDK int ChecklistValueD( int iIndex );
DARKSDK DWORD ChecklistFValueA( int iIndex );
DARKSDK DWORD ChecklistFValueB( int iIndex );
DARKSDK DWORD ChecklistFValueC( int iIndex );
DARKSDK DWORD ChecklistFValueD( int iIndex );
DARKSDK DWORD ChecklistString( DWORD pDestStr, int iIndex );

// dll commands
DARKSDK void DLLLoad( DWORD pDLLFile, int dllid );
DARKSDK void DLLDelete( int dllid );
DARKSDK int DLLExist( int dllid );
DARKSDK int DLLCallExist( int dllid, DWORD pDLLFunction );
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction);
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1);
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2);
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3);
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4);
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5);
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6);
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7);
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8);
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8,DWORD P9 );
DARKSDK DWORD CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8,DWORD P9,DWORD P10 );
DARKSDK DWORD CallDLLX( int dllid, DWORD pDLLFunction, DWORD pDataPtr, DWORD dwNumberOfDWORDS );

// lee - 140206 - u60 - ensures zero return corruption
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction );
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1 );
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2 );
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3 );
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4 );
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5 );
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6 );
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7 );
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8 );
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8,DWORD P9 );
DARKSDK void _CallDLL( int dllid, DWORD pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8,DWORD P9,DWORD P10 );
DARKSDK void _CallDLLX( int dllid, DWORD pDLLFunction, DWORD pDataPtr, DWORD dwNumberOfDWORDS );


DARKSDK void* GetGlobalStructure ( void );

#ifdef DARKSDK_COMPILE
		int		dbSystemTMEMAvailable	(void);
		int		dbSystemDMEMAvailable	(void);
		int		dbSystemSMEMAvailable	(void);

		void	dbDisableEscapeKey		(void);
		void	dbEnableEscapeKey		(void);
		void	dbDisableSystemKeys		(void);
		void	dbEnableSystemKeys		(void);
		void	dbExitPrompt			(char* pString, char* pString2);

		void	dbEmptyChecklist		( void );
		int		dbChecklistQuantity		( void );
		int		dbChecklistValueA		( int iIndex );
		int		dbChecklistValueB		( int iIndex );
		int		dbChecklistValueC		( int iIndex );
		int		dbChecklistValueD		( int iIndex );
		float	dbChecklistFValueA		( int iIndex );
		float	dbChecklistFValueB		( int iIndex );
		float	dbChecklistFValueC		( int iIndex );
		float	dbChecklistFValueD		( int iIndex );
		char*	dbChecklistString		( int iIndex );

		void	dbDLLLoad				( char* pDLLFile, int dllid );
		void	dbDLLDelete				( int dllid );
		int		dbDLLExist				( int dllid );
		int		dbDLLCallExist			( int dllid, char* pDLLFunction );

		DWORD	dbCallDLL				( int dllid, char* pDLLFunction);
		DWORD	dbCallDLL				( int dllid, char* pDLLFunction, DWORD P1);
		DWORD	dbCallDLL				( int dllid, char* pDLLFunction, DWORD P1,DWORD P2);
		DWORD	dbCallDLL				( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3);
		DWORD	dbCallDLL				( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4);
		DWORD	dbCallDLL				( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5);
		DWORD	dbCallDLL				( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6);
		DWORD	dbCallDLL				( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7);
		DWORD	dbCallDLL				( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8);
		DWORD	dbCallDLL				( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8,DWORD P9 );
		DWORD	dbCallDLL				( int dllid, char* pDLLFunction, DWORD P1,DWORD P2,DWORD P3,DWORD P4,DWORD P5,DWORD P6,DWORD P7,DWORD P8,DWORD P9,DWORD P10 );
		DWORD	dbCallDLLX				( int dllid, char* pDLLFunction, DWORD pDataPtr, DWORD dwNumberOfDWORDS );

		void*	dbGetGlobalStructure ( void );

		// lee - 300706 - GDK fixes
		void	dbLoadDLL				( DWORD pDLLFile, int dllid );
		void	dbDeleteDLL				( int dllid );
		int		dbExistDLL				( int dllid );
		int		dbDLLCallExist			( int dllid, char* pDLLFunction );
		int		dbSystemTmemAvailable	(void);
		int		dbSystemDmemAvailable	(void);
		int		dbSystemSmemAvailable	(void);
		void	dbDisableEscapekey		( void );
		void	dbEnableEscapekey		( void );

#endif

//////////////////////////////////////////////////////////////////////////////////
#endif _CSYSTEM_H_