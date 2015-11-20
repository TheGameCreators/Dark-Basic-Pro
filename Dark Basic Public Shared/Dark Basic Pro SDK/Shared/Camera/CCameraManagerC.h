#ifndef _CCAMERAMANAGER_H_
#define _CCAMERAMANAGER_H_

#include <map>

// 20120311 IanM - Use std::map instead of cData class.
//               - Removal of dead code.

struct tagCameraData;

class CCameraManager
{
	public:
		typedef std::map<int, tagCameraData*>   mStore;
		typedef mStore::iterator				mitStore;

	private:
		mStore m_List;

	public:
		CCameraManager  ( );
		~CCameraManager ( );

		bool Add    ( tagCameraData* pData, int iID );
		bool Delete ( int iID );

		tagCameraData* GetData ( int iID );
		int GetNextID ( int iIDHave );
};

#endif _CCAMERAMANAGER_H_