#ifndef _CLIGHT_H_
#define _CLIGHT_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include "CLightManagerC.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifndef DARKSDK_COMPILE
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
#else
	#undef DARKSDK
	#undef DBPRO_GLOBAL
	#define DARKSDK static
	#define DBPRO_GLOBAL static
	// global.h refines DBPRO_GLOBAL in Basic3D.h call, see later
#endif

#ifndef SAFE_DELETE
# define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#endif
#ifndef SAFE_RELEASE
# define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
# define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
		void	ConstructorLight				( HINSTANCE hSetup );
		void	DestructorLight					( void );
		void	SetErrorHandlerLight			( LPVOID pErrorHandlerPtr );
		void	PassCoreDataLight				( LPVOID pGlobPtr );
		void	RefreshD3DLight					( int iMode );
#endif

DARKSDK void	Constructor						( HINSTANCE hSetup );
DARKSDK void	Destructor						( void );
DARKSDK void	SetErrorHandler					( LPVOID pErrorHandlerPtr );
DARKSDK void	PassCoreData					( LPVOID pGlobPtr );
DARKSDK void	RefreshD3D						( int iMode );

DARKSDK	void	Make							( int iID );
DARKSDK void	Delete							( int iID );
DARKSDK bool	GetExist						( int iID );
DARKSDK int		GetType							( int iID );
DARKSDK float	GetXPosition					( int iID );
DARKSDK float	GetYPosition					( int iID );
DARKSDK float	GetZPosition					( int iID );
DARKSDK float	GetXDirection					( int iID );
DARKSDK float	GetYDirection					( int iID );
DARKSDK float	GetZDirection					( int iID );
DARKSDK bool	GetVisible						( int iID );
DARKSDK int		GetRange						( int iID );
DARKSDK	bool	UpdatePtr						( int iID );
DARKSDK void*	GetLightData					( int iID );

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////
// COMMAND FUNCTIONS //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

DARKSDK void	MakeEx							( int iID );
DARKSDK void	DeleteEx						( int iID );

DARKSDK void	SetPoint						( int iID, float fX, float fY, float fZ );
DARKSDK void	SetSpot							( int iID, float fInner, float fOuter );
DARKSDK void	SetDirectional					( int iID, float fX, float fY, float fZ );
DARKSDK void	SetRange						( int iID, float fRange );
DARKSDK void	SetStaticMode					( int iID, int iFlag );

DARKSDK void	Position						( int iID, float fX, float fY, float fZ );
DARKSDK void	Rotate							( int iID, float fX, float fY, float fZ );
DARKSDK void	Point							( int iID, float fX, float fY, float fZ );

DARKSDK void	Hide							( int iID );
DARKSDK void	Show							( int iID );

DARKSDK void	SetToObject						( int iID, int iObjectID );
DARKSDK void	SetToObjectOrientation			( int iID, int iObjectID );
DARKSDK void	SetColor						( int iID, DWORD dwColor );
DARKSDK void	SetColorEx						( int iID, int iR, int iG, int iB );

DARKSDK void	SetNormalizationOn				( void );
DARKSDK void	SetNormalizationOff				( void );

DARKSDK void	SetAmbient						( int iPercent );
DARKSDK int		GetAmbient						( void );
DARKSDK void	SetAmbientColor					( DWORD dwColor );

DARKSDK void	FogOn							( void );
DARKSDK void	FogOff							( void );
DARKSDK void	SetFogColor						( DWORD dwColor );
DARKSDK void	SetFogColorEx					( int iR, int iG, int iB );
DARKSDK void	SetFogColorEx					( int iR, int iG, int iB, int iA );
DARKSDK void	SetFogDistance					( int iDistance );
DARKSDK void	SetFogDistance					( int iStartDistance, int iFinishDistance );

DARKSDK void	SetPositionVector3				( int iID, int iVector );
DARKSDK void	GetPositionVector3				( int iVector, int iID );
DARKSDK void	SetRotationVector3				( int iID, int iVector );
DARKSDK void	GetRotationVector3				( int iVector, int iID );

DARKSDK int		GetExistEx						( int iID );
DARKSDK int		GetTypeEx						( int iID );
DARKSDK DWORD	GetXPositionEx					( int iID );
DARKSDK DWORD	GetYPositionEx					( int iID );
DARKSDK DWORD	GetZPositionEx					( int iID );
DARKSDK DWORD	GetXDirectionEx					( int iID );
DARKSDK DWORD	GetYDirectionEx					( int iID );
DARKSDK DWORD	GetZDirectionEx					( int iID );
DARKSDK int		GetVisibleEx					( int iID );
DARKSDK DWORD	GetRangeEx						( int iID );

DARKSDK void	SetSpecular						( int iID, float fA, float fR, float fG, float fB );
DARKSDK void	SetAmbient						( int iID, float fA, float fR, float fG, float fB );
DARKSDK void	SetFalloff						( int iID, float fValue );
DARKSDK void	SetAttenuation0					( int iID, float fValue );
DARKSDK void	SetAttenuation1					( int iID, float fValue );
DARKSDK void	SetAttenuation2					( int iID, float fValue );

DARKSDK void	SetSpecularOn					( void );
DARKSDK void	SetSpecularOff					( void );

#ifdef DARKSDK_COMPILE
		void	dbMakeLight						( int iID );
		void	dbDeleteLight					( int iID );

		void	dbSetPointLight					( int iID, float fX, float fY, float fZ );
		void	dbSetSpotLight					( int iID, float fInner, float fOuter );
		void	dbSetDirectionalLight			( int iID, float fX, float fY, float fZ );
		void	dbSetLightRange					( int iID, float fRange );

		void	dbPositionLight					( int iID, float fX, float fY, float fZ );
		void	dbRotateLight					( int iID, float fX, float fY, float fZ );
		void	dbPointLight					( int iID, float fX, float fY, float fZ );

		void	dbHideLight						( int iID );
		void	dbShowLight						( int iID );

		void	dbSetLightToObject				( int iID, int iObjectID );
		void	dbSetLightToObjectOrientation	( int iID, int iObjectID );
		void	dbColorLight					( int iID, DWORD dwColor );
		void	dbColorLight					( int iID, int iR, int iG, int iB );

		void	dbSetNormalizationOn			( void );
		void	dbSetNormalizationOff			( void );

		void	dbSetAmbientLight				( int iPercent );
		void	dbColorAmbientLight				( DWORD dwColor );

		void	dbFogOn							( void );
		void	dbFogOff						( void );
		void	dbFogColor						( DWORD dwColor );
		void	dbFogColor						( int iR, int iG, int iB );
		void	dbFogDistance					( int iDistance );
		void	dbFogDistance					( int iStartDistance, int iFinishDistance );

		void	dbPositionLight					( int iID, int iVector );
		void	dbSetVector3ToLightPosition		( int iVector, int iID );
		void	dbRotateLight					( int iID, int iVector );
		void	dbSetVector3ToLightRotation		( int iVector, int iID );

		int		dbLightExist					( int iID );
		int		dbLightType						( int iID );
		float	dbLightPositionX				( int iID );
		float	dbLightPositionY				( int iID );
		float	dbLightPositionZ				( int iID );
		float	dbLightDirectionX				( int iID );
		float	dbLightDirectionY				( int iID );
		float	dbLightDirectionZ				( int iID );
		int		dbLightVisible					( int iID );
		float	dbLightRange					( int iID );

		void	dbSetLightSpecular				( int iID, float fA, float fR, float fG, float fB );
		void	dbSetLightAmbient				( int iID, float fA, float fR, float fG, float fB );
		void	dbSetLightFalloff				( int iID, float fValue );
		void	dbSetLightAttenuation0			( int iID, float fValue );
		void	dbSetLightAttenuation1			( int iID, float fValue );
		void	dbSetLightAttenuation2			( int iID, float fValue );

		void	dbSetLightSpecularOn			( void );
		void	dbSetLightSpecularOff			( void );

		// lee - 300706 - GDK fixes
		void	dbSetLightToObjectPosition		( int iID, int iObjectID );

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




#endif _CLIGHT_H_
