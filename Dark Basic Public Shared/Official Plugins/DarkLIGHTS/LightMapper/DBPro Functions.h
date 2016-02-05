#ifndef H_DBPRO_FUNC
#define H_DBPRO_FUNC

#include "DBOData.h"

#ifndef DARKSDK_COMPILE
	#include "globstruct.h"
	extern GlobStruct *g_pGlob;
#endif

extern LPDIRECT3DDEVICE9 g_pD3D;

typedef sObject*	( *GetObjectPFN )					( int );
typedef int			( *ObjectExistPFN )					( int );
typedef bool		( *CalcObjectWorldPFN )				( sObject *pObject );
typedef void		( *CalculateAbsoluteWorldMatrixPFN )( sObject *pObject, sFrame *pFrame, sMesh *pMesh );
typedef void		( *ConvertLocalMeshToVertsOnlyPFN ) ( sMesh *pMesh );
typedef void		( *ConvertLocalMeshToFVFPFN )		( sMesh *pMesh, DWORD );
typedef	IDirect3DDevice9*	( *GetDirect3DDevicePFN )	( void );
typedef void		( *SetBlendMappingPFN )				( int, int, int, int, int );
typedef void		( *HideLimbPFN )					( int, int );
typedef bool		( *GetFVFOffsetMapPFN )				( sMesh *pMesh, sOffsetMap *pOffsetMap );
typedef void		( *SmoothNormalsPFN )				( sMesh *pMesh, float fValue );
typedef void		( *PositionObjectPFN )				( int, float, float, float );
typedef void		( *ConvertToFVFPFN )				( sMesh *pMesh, DWORD dwFVF );
typedef void		( *CloneMeshToNewFormatPFN )		( int, DWORD );

extern GetObjectPFN						g_GetObject;
extern ObjectExistPFN					g_ObjectExist;
extern CalcObjectWorldPFN				g_CalcObjectWorld;
extern CalculateAbsoluteWorldMatrixPFN	g_CalculateAbsoluteWorldMatrix;
extern ConvertLocalMeshToVertsOnlyPFN	g_ConvertLocalMeshToVertsOnly;
extern ConvertLocalMeshToFVFPFN			g_ConvertLocalMeshToFVF;
extern GetDirect3DDevicePFN				g_GetDirect3DDevice;
extern SetBlendMappingPFN				g_SetBlendMapping;
extern HideLimbPFN						g_HideLimb;
extern GetFVFOffsetMapPFN				g_GetFVFOffsetMap;
extern SmoothNormalsPFN					g_SmoothNormals;
extern PositionObjectPFN				g_PositionObject;
extern ConvertToFVFPFN					g_ConvertToFVF;

#endif