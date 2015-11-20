//
// ConvX Functions Header
//

//////////////////////////////////////////////////////////////////////////////////
// INCLUDE COMMON FILES //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <mmsystem.h>
#include <objbase.h>
#include <malloc.h> // _alloca
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <eh.h>
#include <dxfile.h>




	

//////////////////////////////////////////////////////////////////////////////////////
// DBOFORMAT INCLUDE /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#include "..\\DBOFormat\\DBOBlock.h"
#include ".\..\DBOFormat\DBOFormat.h"
#include ".\..\DBOFormat\DBOFrame.h"
#include ".\..\DBOFormat\DBOMesh.h"
#include ".\..\DBOFormat\DBORawMesh.h"
#include ".\..\DBOFormat\DBOEffects.h"
#include ".\..\DBOFormat\DBOFile.h"
#include ".\..\core\globstruct.h"

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
    #undef DARKSDK
    #undef DBPRO_GLOBAL
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#else
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
	#define DARKSDK_DLL 
#endif

//#define DARKSDK __declspec ( dllexport )
//#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
//#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
//#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

//////////////////////////////////////////////////////////////////////////////////
// GUIDS NOT PART OF RM GUI DLIST ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
DEFINE_GUID(TID_D3DXSkinMeshHeader,
0x3cf169ce, 0xff7c, 0x44ab, 0x93, 0xc0, 0xf7, 0x8f, 0x62, 0xd1, 0x72, 0xe2);
DEFINE_GUID(TID_D3DVertexDuplicationIndices,
0xb8d65549, 0xd7c9, 0x4995, 0x89, 0xcf, 0x53, 0xa9, 0xa8, 0xb0, 0x31, 0xe3);
DEFINE_GUID(TID_D3DSkinWeights,
0x6f0d123b, 0xbad2, 0x4167, 0xa0, 0xd0, 0x80, 0x22, 0x4f, 0x25, 0xfa, 0xbb);

//////////////////////////////////////////////////////////////////////////////////////
// STRUCTURES ////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// main mesh data structure (pos,norm,uv)
struct sVertex
{
	float  x,  y,  z;				// position
	float nx, ny, nz;				// normal
	float tu1, tv1;					// texture coordinates 0
};

// structures of vertex color (index,rgba)
struct sColorRGBA
{
	float red;
	float green;
	float blue;
	float alpha;
};

struct sVertexColor
{
	DWORD index;
	sColorRGBA color;
};

//////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#ifndef DARKSDK_COMPILE
extern "C"
{
	DARKSDK void	SetLegacyModeOn		( void );
	DARKSDK void	SetLegacyModeOff	( void );
	DARKSDK	bool	Convert				( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize );
	DARKSDK void	Free				( LPSTR );
}
#else
	bool	ConvertX		( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize );
	void	FreeX			( LPSTR );
#endif

