#ifndef _CLIGHTMANAGER_H_
#define _CLIGHTMANAGER_H_

//#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "ddraw.lib" )
#pragma comment ( lib, "d3d9.lib" )
#pragma comment ( lib, "d3dx9.lib" )
#pragma comment ( lib, "dxerr9.lib" )

#include <ddraw.h>
#include <d3d9.h>
#include <Dxerr9.h>
#include <D3dx9tex.h>
#include <D3dx9core.h>
#include <basetsd.h>
#include <stdio.h>
#include <math.h>
#include <D3DX9.h>
#include <d3d9types.h>

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>   
#include <windowsx.h>

#include "CLightDataC.h"
#include <map>

class CLightManager
{
    public:
        typedef std::map<int, tagLightData*>    LightList_t;
        typedef LightList_t::iterator           LightPtr;

	private:
        LightList_t     m_List;
        tagLightData*   CurrentPtr;
        int             CurrentID;

    public:
        CLightManager	( ) : CurrentPtr(NULL), CurrentID(-1) { }

		bool Add	( tagLightData* pData, int iID );
		bool Delete ( int iID, LPDIRECT3DDEVICE9 pD3D = NULL );

        void DeleteAll ( LPDIRECT3DDEVICE9 pD3D = NULL );

		tagLightData* GetData ( int iID );
};

#endif _CLIGHTMANAGER_H_