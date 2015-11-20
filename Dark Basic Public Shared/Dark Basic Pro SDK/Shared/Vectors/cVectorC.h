#ifndef _CVECTOR_H_
#define _CVECTOR_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#pragma comment ( lib, "d3dx9.lib" )
#include <D3DX9.h>

#include "cVectorManagerC.h"
#include "cVectorDataC.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifndef DARKSDK_COMPILE
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
#else
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#endif

#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

#define SDK_BOOL int
#define SDK_TRUE 1
#define SDK_FALSE 0
#define SDK_FLOAT DWORD
#define SDK_LPSTR DWORD
#define SDK_RETFLOAT(f) *(DWORD*)&f 
#define SDK_RETSTR DWORD pDestStr,

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
		void			Constructor3DMaths					( HINSTANCE hSetup );
		void			Destructor3DMaths					( void );
		void			SetErrorHandler3DMaths				( LPVOID pErrorHandlerPtr );
		void			PassCoreData3DMaths					( LPVOID pGlobPtr );
		void			RefreshD3D3DMaths					( int iMode );
#endif

DARKSDK void			Constructor 						( HINSTANCE hSetup );
DARKSDK void			Destructor  						( void );
DARKSDK void			SetErrorHandler 					( LPVOID pErrorHandlerPtr );
DARKSDK void			PassCoreData						( LPVOID pGlobPtr );
DARKSDK void			RefreshD3D 							( int iMode );

DARKSDK D3DXVECTOR2		GetVector2 							( int iID );
DARKSDK D3DXVECTOR3		GetVector3 							( int iID );
DARKSDK D3DXVECTOR4		GetVector4 							( int iID );
DARKSDK D3DXMATRIX		GetMatrix							( int iID );
DARKSDK int				GetExist							( int iID );

DARKSDK bool			CheckTypeIsValid					( int iID, int iType );

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////
// COMMAND FUNCTIONS //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

DARKSDK SDK_BOOL		MakeVector2							( int iID );
DARKSDK SDK_BOOL		DeleteVector2						( int iID );
DARKSDK void			SetVector2							( int iID, float fX, float fY );
DARKSDK void			CopyVector2							( int iSource, int iDestination );
DARKSDK void			AddVector2							( int iResult, int iA, int iB );
DARKSDK void			SubtractVector2						( int iResult, int iA, int iB );
DARKSDK void			MultiplyVector2						( int iID, float fValue );
DARKSDK void			DivideVector2						( int iID, float fValue );
DARKSDK SDK_BOOL		IsEqualVector2						( int iA, int iB );
DARKSDK SDK_BOOL		IsNotEqualVector2					( int iA, int iB );
DARKSDK SDK_FLOAT		GetXVector2							( int iID );
DARKSDK SDK_FLOAT		GetYVector2							( int iID );
DARKSDK void			GetBaryCentricCoordinatesVector2	( int iResult, int iA, int iB, int iC, float f, float g );
DARKSDK void			CatmullRomVector2					( int iResult, int iA, int iB, int iC, int iD, float s );
DARKSDK SDK_FLOAT		GetCCWVector2						( int iA, int iB );
DARKSDK SDK_FLOAT		DotProductVector2					( int iA, int iB );
DARKSDK void			HermiteVector2						( int iResult, int iA, int iB, int iC, int iD, float s );
DARKSDK SDK_FLOAT		GetLengthVector2					( int iID );
DARKSDK SDK_FLOAT		GetLengthSquaredVector2				( int iID );
DARKSDK void			LinearInterpolationVector2			( int iResult, int iA, int iB, float s );
DARKSDK void			MaximizeVector2						( int iResult, int iA, int iB );
DARKSDK void			MinimizeVector2						( int iResult, int iA, int iB );
DARKSDK void			NormalizeVector2					( int iResult, int iSource );
DARKSDK void			ScaleVector2						( int iResult, int iSource, float s );
DARKSDK void			TransformVectorCoordinates2			( int iResult, int iSource, int iMatrix );

DARKSDK SDK_BOOL		MakeVector3							( int iID );
DARKSDK SDK_BOOL		DeleteVector3						( int iID );
DARKSDK void			SetVector3							( int iID, float fX, float fY, float fZ );
DARKSDK void			CopyVector3							( int iSource, int iDestination );
DARKSDK void			AddVector3							( int iResult, int iA, int iB );
DARKSDK void			SubtractVector3						( int iResult, int iA, int iB );
DARKSDK void			MultiplyVector3						( int iID, float fValue );
DARKSDK void			DivideVector3						( int iID, float fValue );
DARKSDK SDK_BOOL		IsEqualVector3						( int iA, int iB );
DARKSDK SDK_BOOL		IsNotEqualVector3					( int iA, int iB );
DARKSDK void			GetBaryCentricCoordinatesVector3	( int iResult, int iA, int iB, int iC, float f, float g );
DARKSDK void			CatmullRomVector3					( int iResult, int iA, int iB, int iC, int iD, float s );
DARKSDK void  			CrossProductVector3					( int iResult, int iA, int iB );
DARKSDK void  			HermiteVector3						( int iResult, int iA, int iB, int iC, int iD, float s );
DARKSDK void  			LinearInterpolationVector3			( int iResult, int iA, int iB, float s );
DARKSDK void  			MaximizeVector3						( int iResult, int iA, int iB );
DARKSDK void  			MinimizeVector3						( int iResult, int iA, int iB );
DARKSDK void  			NormalizeVector3					( int iResult, int iSource );
DARKSDK void  			ScaleVector3						( int iResult, int iSource, float s );
DARKSDK SDK_FLOAT 		DotProductVector3					( int iA, int iB );
DARKSDK SDK_FLOAT		GetLengthVector3					( int iID );
DARKSDK SDK_FLOAT		GetLengthSquaredVector3				( int iID );
DARKSDK void  			ProjectVector3						( int iResult, int iSource, int iProjectionMatrix, int iViewMatrix, int iWorldMatrix );
DARKSDK void  			TransformVectorCoordinates3			( int iResult, int iSource, int iMatrix );
DARKSDK void  			TransformVectorNormalCoordinates3	( int iResult, int iSource, int iMatrix );
DARKSDK SDK_FLOAT		GetXVector3							( int iID );
DARKSDK SDK_FLOAT		GetYVector3							( int iID );
DARKSDK SDK_FLOAT		GetZVector3							( int iID );

DARKSDK SDK_BOOL		MakeVector4							( int iID );
DARKSDK SDK_BOOL		DeleteVector4						( int iID );
DARKSDK void  			SetVector4							( int iID, float fX, float fY, float fZ, float fW );
DARKSDK void  			CopyVector4							( int iSource, int iDestination );
DARKSDK void  			AddVector4							( int iResult, int iA, int iB );
DARKSDK void  			SubtractVector4						( int iResult, int iA, int iB );
DARKSDK void  			MultiplyVector4						( int iID, float fValue );
DARKSDK void  			DivideVector4						( int iID, float fValue );
DARKSDK SDK_BOOL		IsEqualVector4						( int iA, int iB );
DARKSDK SDK_BOOL		IsNotEqualVector4					( int iA, int iB );
DARKSDK SDK_FLOAT		GetXVector4							( int iID );
DARKSDK SDK_FLOAT		GetYVector4							( int iID );
DARKSDK SDK_FLOAT		GetZVector4							( int iID );
DARKSDK SDK_FLOAT		GetWVector4							( int iID );
DARKSDK void  			GetBaryCentricCoordinatesVector4	( int iResult, int iA, int iB, int iC, float f, float g );
DARKSDK void  			CatmullRomVector4					( int iResult, int iA, int iB, int iC, int iD, float s );
DARKSDK void  			HermiteVector4						( int iResult, int iA, int iB, int iC, int iD, float s );
DARKSDK SDK_FLOAT		GetLengthVector4					( int iID );
DARKSDK SDK_FLOAT		GetLengthSquaredVector4				( int iID );
DARKSDK void  			LinearInterpolationVector4			( int iResult, int iA, int iB, float s );
DARKSDK void  			MaximizeVector4						( int iResult, int iA, int iB );
DARKSDK void  			MinimizeVector4						( int iResult, int iA, int iB );
DARKSDK void  			NormalizeVector4					( int iResult, int iSource );
DARKSDK void  			ScaleVector4						( int iResult, int iSource, float s );
DARKSDK void  			TransformVector4					( int iResult, int iSource, int iMatrix );

DARKSDK SDK_BOOL		MakeMatrix							( int iID );
DARKSDK SDK_BOOL		DeleteMatrix						( int iID );
DARKSDK SDK_BOOL		Matrix4Exist						( int iID );
DARKSDK void  			CopyMatrix							( int iSource, int iDestination );
DARKSDK void  			AddMatrix							( int iResult, int iA, int iB );
DARKSDK void  			SubtractMatrix						( int iResult, int iA, int iB );
DARKSDK void  			MultiplyMatrix						( int iResult, int iA, int iB );
DARKSDK void  			MultiplyMatrix						( int iID, float fValue );
DARKSDK void  			DivideMatrix						( int iID, float fValue );
DARKSDK SDK_BOOL		IsEqualMatrix						( int iA, int iB );
DARKSDK SDK_BOOL		IsNotEqualMatrix					( int iA, int iB );
DARKSDK void  			SetIdentityMatrix					( int iID );
DARKSDK SDK_FLOAT		InverseMatrix						( int iResult, int iSource );
DARKSDK SDK_BOOL		IsIdentityMatrix					( int iID );
DARKSDK void  			BuildLookAtRHMatrix					( int iResult, int iVectorEye, int iVectorAt, int iVectorUp );
DARKSDK void  			BuildLookAtLHMatrix					( int iResult, int iVectorEye, int iVectorAt, int iVectorUp );
DARKSDK void  			BuildOrthoRHMatrix					( int iResult, float fWidth, float fHeight, float fZNear, float fZFar );
DARKSDK void  			BuildOrthoLHMatrix					( int iResult, float fWidth, float fHeight, float fZNear, float fZFar );
DARKSDK void  			BuildPerspectiveRHMatrix			( int iResult, float fWidth, float fHeight, float fZNear, float fZFar );
DARKSDK void  			BuildPerspectiveLHMatrix			( int iResult, float fWidth, float fHeight, float fZNear, float fZFar );
DARKSDK void  			BuildPerspectiveFovRHMatrix			( int iResult, float fFOV, float fAspect, float fZNear, float fZFar );
DARKSDK void  			BuildPerspectiveFovLHMatrix			( int iResult, float fFOV, float fAspect, float fZNear, float fZFar );
DARKSDK void  			BuildReflectionMatrix				( int iResult, float a, float b, float c, float d );
DARKSDK void  			BuildRotationAxisMatrix				( int iResult, int iVectorAxis, float fAngle );
DARKSDK void  			RotateXMatrix						( int iID, float fAngle );
DARKSDK void  			RotateYMatrix						( int iID, float fAngle );
DARKSDK void  			RotateZMatrix						( int iID, float fAngle );
DARKSDK void  			RotateYawPitchRollMatrix			( int iID, float fYaw, float fPitch, float fRoll );
DARKSDK void  			ScaleMatrix							( int iID, float fX, float fY, float fZ );
DARKSDK void  			TranslateMatrix						( int iID, float fX, float fY, float fZ );
DARKSDK void  			TransposeMatrix						( int iResult, int iSource );
DARKSDK void  			GetWorldMatrix						( int iID );
DARKSDK void  			GetViewMatrix						( int iID );
DARKSDK void  			GetProjectionMatrix					( int iID );
DARKSDK void			SetProjectionMatrix					( int iID );

#ifdef DARKSDK_COMPILE
		bool			dbMakeVector2						( int iID );
		bool			dbDeleteVector2						( int iID );
		void			dbSetVector2						( int iID, float fX, float fY );
		void			dbCopyVector2						( int iSource, int iDestination );
		void			dbAddVector2						( int iResult, int iA, int iB );
		void			dbSubtractVector2					( int iResult, int iA, int iB );
		void			dbMultiplyVector2					( int iID, float fValue );
		void			dbDivideVector2						( int iID, float fValue );
		bool			dbIsEqualVector2					( int iA, int iB );
		bool			dbIsNotEqualVector2					( int iA, int iB );
		float			dbXVector2							( int iID );
		float			dbYVector2							( int iID );
		void			dbBCCVector2						( int iResult, int iA, int iB, int iC, float f, float g );
		void			dbCatmullRomVector2					( int iResult, int iA, int iB, int iC, int iD, float s );
		float			dbCCWVector2						( int iA, int iB );
		float			dbDotProductVector2					( int iA, int iB );
		void			dbHermiteVector2					( int iResult, int iA, int iB, int iC, int iD, float s );
		float			dbLengthVector2						( int iID );
		float			dbSquaredLengthVector2				( int iID );
		void			dbLinearInterpolateVector2			( int iResult, int iA, int iB, float s );
		void			dbMaximizeVector2					( int iResult, int iA, int iB );
		void			dbMinimizeVector2					( int iResult, int iA, int iB );
		void			dbNormalizeVector2					( int iResult, int iSource );
		void			dbScaleVector2						( int iResult, int iSource, float s );
		void			dbTransformCoordsVector2			( int iResult, int iSource, int iMatrix );

		bool			dbMakeVector3						( int iID );
		bool			dbDeleteVector3						( int iID );
		void			dbSetVector3						( int iID, float fX, float fY, float fZ );
		void			dbCopyVector3						( int iSource, int iDestination );
		void			dbAddVector3						( int iResult, int iA, int iB );
		void			dbSubtractVector3					( int iResult, int iA, int iB );
		void			dbMultiplyVector3					( int iID, float fValue );
		void			dbDivideVector3						( int iID, float fValue );
		bool			dbIsEqualVector3					( int iA, int iB );
		bool			dbIsNotEqualVector3					( int iA, int iB );
		void			dbBCCVector3						( int iResult, int iA, int iB, int iC, float f, float g );
		void			dbCatmullRomVector3					( int iResult, int iA, int iB, int iC, int iD, float s );
		void  			dbCrossProductVector3				( int iResult, int iA, int iB );
		void  			dbHermiteVector3					( int iResult, int iA, int iB, int iC, int iD, float s );
		void  			dbLinearInterpolateVector3			( int iResult, int iA, int iB, float s );
		void  			dbMaximizeVector3					( int iResult, int iA, int iB );
		void  			dbMinimizeVector3					( int iResult, int iA, int iB );
		void  			dbNormalizeVector3					( int iResult, int iSource );
		void  			dbScaleVector3						( int iResult, int iSource, float s );
		float 			dbDotProductVector3					( int iA, int iB );
		float			dbLengthVector3						( int iID );
		float			dbSquaredLengthVector3				( int iID );
		void  			dbProjectVector3					( int iResult, int iSource, int iProjectionMatrix, int iViewMatrix, int iWorldMatrix );
		void  			dbTransformCoordsVector3			( int iResult, int iSource, int iMatrix );
		void  			dbTransformNormalsVector3			( int iResult, int iSource, int iMatrix );
		float			dbXVector3							( int iID );
		float			dbYVector3							( int iID );
		float			dbZVector3							( int iID );

		bool			dbMakeVector4						( int iID );
		bool			dbDeleteVector4						( int iID );
		void  			dbSetVector4						( int iID, float fX, float fY, float fZ, float fW );
		void  			dbCopyVector4						( int iSource, int iDestination );
		void  			dbAddVector4						( int iResult, int iA, int iB );
		void  			dbSubtractVector4					( int iResult, int iA, int iB );
		void  			dbMultiplyVector4					( int iID, float fValue );
		void  			dbDivideVector4						( int iID, float fValue );
		bool			dbIsEqualVector4					( int iA, int iB );
		bool			dbIsNotEqualVector4					( int iA, int iB );
		float			dbXVector4							( int iID );
		float			dbYVector4							( int iID );
		float			dbZVector4							( int iID );
		float			dbWVector4							( int iID );
		void  			dbBCCVector4						( int iResult, int iA, int iB, int iC, float f, float g );
		void  			dbCatmullRomVector4					( int iResult, int iA, int iB, int iC, int iD, float s );
		void  			dbHermiteVector4					( int iResult, int iA, int iB, int iC, int iD, float s );
		float			dbLengthVector4						( int iID );
		float			dbSquaredLengthVector4				( int iID );
		void  			dbLinearInterpolateVector4			( int iResult, int iA, int iB, float s );
		void  			dbMaximizeVector4					( int iResult, int iA, int iB );
		void  			dbMinimizeVector4					( int iResult, int iA, int iB );
		void  			dbNormalizeVector4					( int iResult, int iSource );
		void  			dbScaleVector4						( int iResult, int iSource, float s );
		void  			dbTransformVector4					( int iResult, int iSource, int iMatrix );

		bool			dbMakeMatrix4						( int iID );
		bool			dbDeleteMatrix4						( int iID );
		void  			dbCopyMatrix4						( int iSource, int iDestination );
		void  			dbAddMatrix4						( int iResult, int iA, int iB );
		void  			dbSubtractMatrix4					( int iResult, int iA, int iB );
		void  			dbMultiplyMatrix4					( int iResult, int iA, int iB );
		void  			dbMultiplyMatrix4					( int iID, float fValue );
		void  			dbDivideMatrix4						( int iID, float fValue );
		bool			dbIsEqualMatrix4					( int iA, int iB );
		bool			dbIsNotEqualMatrix4					( int iA, int iB );
		void  			dbSetIdentityMatrix4				( int iID );
		float			dbInverseMatrix4					( int iResult, int iSource );
		bool			dbIsIdentityMatrix4					( int iID );
		void  			dbBuildLookAtRHMatrix4				( int iResult, int iVectorEye, int iVectorAt, int iVectorUp );
		void  			dbBuildLookAtLHMatrix4				( int iResult, int iVectorEye, int iVectorAt, int iVectorUp );
		void  			dbBuildOrthoRHMatrix4				( int iResult, float fWidth, float fHeight, float fZNear, float fZFar );
		void  			dbBuildOrthoLHMatrix4				( int iResult, float fWidth, float fHeight, float fZNear, float fZFar );
		void  			dbBuildPerspectiveRHMatrix4			( int iResult, float fWidth, float fHeight, float fZNear, float fZFar );
		void  			dbBuildPerspectiveLHMatrix4			( int iResult, float fWidth, float fHeight, float fZNear, float fZFar );
		void  			dbBuildFovRHMatrix4					( int iResult, float fFOV, float fAspect, float fZNear, float fZFar );
		void  			dbBuildFovLHMatrix4					( int iResult, float fFOV, float fAspect, float fZNear, float fZFar );
		void  			dbBuildReflectionMatrix4			( int iResult, float a, float b, float c, float d );
		void  			dbBuildRotationAxisMatrix4			( int iResult, int iVectorAxis, float fAngle );
		void  			dbRotateXMatrix4					( int iID, float fAngle );
		void  			dbRotateYMatrix4					( int iID, float fAngle );
		void  			dbRotateZMatrix4					( int iID, float fAngle );
		void  			dbRotateYPRMatrix4					( int iID, float fYaw, float fPitch, float fRoll );
		void  			dbScaleMatrix4						( int iID, float fX, float fY, float fZ );
		void  			dbTranslateMatrix4					( int iID, float fX, float fY, float fZ );
		void  			dbTransposeMatrix4					( int iResult, int iSource );
		void  			dbWorldMatrix4						( int iID );
		void  			dbViewMatrix4						( int iID );
		void  			dbProjectionMatrix4					( int iID );

		D3DXVECTOR2		dbGetVector2 						( int iID );
		D3DXVECTOR3		dbGetVector3 						( int iID );
		D3DXVECTOR4		dbGetVector4 						( int iID );
		D3DXMATRIX		dbGetMatrix							( int iID );
		int				dbGet3DMathsExist					( int iID );
#endif

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

#endif _CVECTOR_H_
