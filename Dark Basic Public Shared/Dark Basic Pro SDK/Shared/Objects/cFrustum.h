#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include <D3DX8.h>

#include "cobjectmanagerc.h"
#include "cobjectdatac.h"
#include ".\..\position\cpositionc.h"
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
//////////////////////////////////////////////////////////////////////////////////



/*
void SetupFrustum   ( float fZDistance );
bool CheckPoint     ( float fX, float fY, float fZ );
bool CheckCube      ( float fX, float fY, float fZ, float fSize );
bool CheckRectangle ( float fX, float fY, float fZ, float fXSize, float fYSize, float fZSize );
bool CheckSphere    ( float fX, float fY, float fZ, float fRadius );
*/

/*
void      UpdateCullInfo     ( CULLINFO* pCullInfo, D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj );
CULLSTATE CullObject         ( CULLINFO* pCullInfo, D3DXVECTOR3* pVecBounds, D3DXPLANE* pPlaneBounds );
BOOL      EdgeIntersectsFace ( D3DXVECTOR3* pEdges, D3DXVECTOR3* pFaces, D3DXPLANE* pPlane );
*/

#endif
