#ifndef _CSOUNDMANAGER_H_
#define _CSOUNDMANAGER_H_

#include ".\..\data\cdatac.h"
#include "CSoundDataC.h"

//#define DARKSDK __declspec ( dllexport )

class CSDKSoundManager : private CData
{
	private:
		CData	m_List;	

	public:
		CSDKSoundManager  ( );			// constructor
		~CSDKSoundManager ( );			// destructor

		bool Add    ( sSoundData* pData, int iID );
		bool Delete ( int iID );

		sSoundData* GetData ( int iID );

		void Update ( void );

		void ShutDown ( void );

		link* GetList() { return m_List.m_start; }
};

#endif _CSOUNDMANAGER_H_