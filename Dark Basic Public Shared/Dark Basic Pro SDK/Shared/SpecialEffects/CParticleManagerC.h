#ifndef _CPARTICLEMANAGER_H_
#define _CPARTICLEMANAGER_H_

#include ".\..\data\cdatac.h"
#include "cparticledatac.h"

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>   
#include <windowsx.h> 

class CParticleManager : private CData
{
	private:
		CData	m_List;	

	public:
		CParticleManager  ( );
		~CParticleManager ( );

		inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

		bool Add    ( tagParticleData* pData, int iID );
		bool Delete ( int iID );
		void DeleteAll ( void ) { m_List.DeleteAll(); }

		tagParticleData* GetData ( int iID );
		CData* GetList ( void ) { return &m_List; }

		void Update ( void );
        void UpdateInner ( void );
};

#endif _CPARTICLEMANAGER_H_