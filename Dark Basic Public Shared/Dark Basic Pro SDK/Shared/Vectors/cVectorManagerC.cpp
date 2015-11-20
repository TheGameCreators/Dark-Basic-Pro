#include "cVectorManagerC.h"


cDataManager::cDataManager ( )
{
}

cDataManager::~cDataManager ( )
{
    ClearAll();
}

void cDataManager::ClearAll ( )
{
    for (ListPtr pCheck = m_List.begin(); pCheck != m_List.end(); ++pCheck)
    {
        // Release the texture and texture name
        delete pCheck->second;
    }

    // Now clear the list
    m_List.clear();
}

bool cDataManager::Add ( BaseVector* pData, int iID )
{
    // Attempt to insert
    ListInsertStatus s = m_List.insert( std::make_pair( iID, pData ) );

    // If insert fails due to already existing, replace instead
    if (!s.second)
    {
        delete s.first->second;
        s.first->second = pData;
    }

	return true;
}

bool cDataManager::Delete ( int iID )
{
    ListPtr p = m_List.find( iID );
    if (p != m_List.end())
    {
        delete p->second;
        m_List.erase( p );
    }
	return true;
}
