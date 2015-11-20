#ifndef _CMATRIXMANAGER_H_
#define _CMATRIXMANAGER_H_

#include ".\..\data\cdatac.h"
#include "cmatrixdatac.h"

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
//////////////////////////////////////////////////////////////////////////////////

extern LPDIRECT3DDEVICE9 m_pD3D;

class CMatrixManager : private CData
{
	public:
		CData	m_List;	

	public:
		CMatrixManager  ( );			// constructor
		~CMatrixManager ( );			// destructor

		bool Add    ( tagMatrixData* pData, int iID );
		bool Delete ( int iID );

		tagMatrixData* GetData ( int iID );

		void Update ( int iMode );
};

#endif _CMATRIXMANAGER_H_