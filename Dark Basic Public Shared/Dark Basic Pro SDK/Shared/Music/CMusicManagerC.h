#ifndef _CMUSICMANAGER_H_
#define _CMUSICMANAGER_H_

#include "BasePlayer.h"

#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

#include <map>

class CMusicManager
{
private:
    typedef std::map<int, DBP_Music::BasePlayer*>      MusicStore;
    typedef MusicStore::iterator            MusicPtr;

    MusicStore  m_List;

public:
	bool Add    ( DBP_Music::BasePlayer* pData, int iID );
	bool Delete ( int iID );

	DBP_Music::BasePlayer* GetData ( int iID );

	void Update ( void );

	void ShutDown ( void );
};

#endif _CMUSICMANAGER_H_