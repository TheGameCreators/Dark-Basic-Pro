// include definition
#include "cmusicmanagerc.h"

bool CMusicManager::Add ( DBP_Music::BasePlayer* pData, int iID )
{
    Delete( iID );
    m_List.insert( std::make_pair(iID, pData) );
	return true;
}

bool CMusicManager::Delete ( int iID )
{
    MusicPtr p = m_List.find( iID );
    if (p != m_List.end())
    {
        delete p->second;
        m_List.erase( p );
    }
	return true;
}

DBP_Music::BasePlayer* CMusicManager::GetData ( int iID )
{
    MusicPtr p = m_List.find( iID );
    return p == m_List.end() ? 0 : p->second;
}

void CMusicManager::ShutDown ( void )
{
    for (MusicPtr p = m_List.begin(); p != m_List.end(); ++p)
    {
        delete p->second;
    }
    m_List.clear();
}

void CMusicManager::Update ( void )
{
    for (MusicPtr p = m_List.begin(); p != m_List.end(); ++p)
    {
        p->second->Update();
    }
}
