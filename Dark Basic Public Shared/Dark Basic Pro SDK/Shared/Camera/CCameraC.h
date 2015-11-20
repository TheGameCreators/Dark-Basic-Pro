#ifndef _CCAMERA_H_
#define _CCAMERA_H_

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include ".\..\data\cdatac.h"
#include "CCameraDatac.h"
#include "CCameraManagerc.h"

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




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
		void		UpdateCamera					( void );
		void		ConstructorCamera 				( HINSTANCE hSetup, HINSTANCE hImage );
		void		DestructorCamera				( void );
		void		SetErrorHandlerCamera			( LPVOID pErrorHandlerPtr );
		void		PassCoreDataCamera				( LPVOID pGlobPtr );
		void		RefreshD3DCamera 				( int iMode );
#endif

DARKSDK void  		Update 							( void );
DARKSDK void  		Constructor 					( HINSTANCE hSetup, HINSTANCE hImage );
DARKSDK void  		Destructor  					( void );
DARKSDK void  		SetErrorHandler					( LPVOID pErrorHandlerPtr );
DARKSDK void  		PassCoreData					( LPVOID pGlobPtr );
DARKSDK void  		RefreshD3D 						( int iMode );

DARKSDK IDirect3DTexture9* GetStereoscopicFinalTexture ( void ); // U70 - 180608 - WOW autostereo

DARKSDK bool 		UpdatePtr      					( int iID );
DARKSDK void 		InternalUpdate					( int iID );

DARKSDK void  		StartScene						( void );
DARKSDK void  		StartSceneEx					( int );
DARKSDK int   		FinishScene						( void );
DARKSDK int			FinishSceneEx					( bool bKnowInAdvanceCameraIsUsed );
DARKSDK int	  		GetRenderCamera					( void );
DARKSDK void  		RunCode							( int );

DARKSDK void  		Rotate    						( int iID, float fX, float fY, float fZ );
DARKSDK void  		XRotate   						( int iID, float fX );
DARKSDK void  		YRotate   						( int iID, float fY );
DARKSDK void  		ZRotate   						( int iID, float fZ );

DARKSDK float 		GetXPosition 					( int iID );
DARKSDK float 		GetYPosition 					( int iID );
DARKSDK float 		GetZPosition 					( int iID );
DARKSDK float 		GetXAngle    					( int iID );
DARKSDK float 		GetYAngle    					( int iID );
DARKSDK float 		GetZAngle    					( int iID );
DARKSDK float 		GetLookX     					( int iID );
DARKSDK float 		GetLookY     					( int iID );
DARKSDK float		GetLookZ						( int iID );

DARKSDK void*		GetInternalData					( int iID );
DARKSDK D3DXMATRIX	GetMatrix						( int iID );

DARKSDK void		SetAutoCam						( float fX, float fY, float fZ, float fRadius );
DARKSDK void		Create							( int iID );
DARKSDK void		Destroy							( int iID );

DARKSDK void		SetCameraMatrix					( int iID, D3DXMATRIX* pMatrix );
DARKSDK void		ReleaseCameraMatrix				( int iID );

// mike - 280305 - new functions to extra view and projection matrix
DARKSDK D3DXMATRIX	GetViewMatrix					( int iID );
DARKSDK D3DXMATRIX	GetProjectionMatrix				( int iID );
DARKSDK void		SetProjectionMatrix				( int iID, D3DXMATRIX* pMatrix );

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// COMMAND FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void		Position						( float fX, float fY, float fZ );
DARKSDK void		SetRotate						( float fX, float fY, float fZ );
DARKSDK void		SetXRotate						( float fX );
DARKSDK void		SetYRotate						( float fY );
DARKSDK void		SetZRotate						( float fZ );
DARKSDK void		Point 							( float fX, float fY, float fZ );
DARKSDK void		Move 							( float fStep );
DARKSDK void		SetRange 						( float fFront, float fBack );
DARKSDK void		SetView 						( int iLeft, int iTop, int iRight, int iBottom );
DARKSDK void		ClearView 						( DWORD dwColorValue );
DARKSDK void		SetRotationXYZ 					( void );
DARKSDK void		SetRotationZYX					( void );
DARKSDK void		SetFOV 							( float fAngle );
DARKSDK void		SetAspect 						( float fAspect );
DARKSDK void		Follow 							( float fX, float fY, float fZ, float fAngle, float fDistance, float fHeight, float fSmooth, int iCollision );
DARKSDK void		SetAutoCamOn					( void );
DARKSDK void		SetAutoCamOff 					( void );
DARKSDK void		TurnLeft 						( float fAngle );
DARKSDK void		TurnRight 						( float fAngle );
DARKSDK void		PitchUp 						( float fAngle );
DARKSDK void		PitchDown 						( float fAngle );
DARKSDK void		RollLeft 						( float fAngle );
DARKSDK void		RollRight						( float fAngle );
DARKSDK void		SetToObjectOrientation			( int iObjectID );

DARKSDK void		BackdropOn						( void ); 
DARKSDK void		BackdropOff						( void ); 
DARKSDK void		BackdropColor					( DWORD dwColor ); 
DARKSDK void		BackdropTexture					( int iImage ); 
DARKSDK void		BackdropScroll					( int iU, int iV ); 

DARKSDK DWORD		GetXPositionEx					( void );
DARKSDK DWORD		GetYPositionEx					( void );
DARKSDK DWORD		GetZPositionEx					( void );
DARKSDK DWORD		GetXAngleEx						( void );
DARKSDK DWORD		GetYAngleEx						( void );
DARKSDK DWORD		GetZAngleEx						( void );

DARKSDK void		CreateEx						( int iID );
DARKSDK void		DestroyEx						( int iID );
DARKSDK void		SetCurrentCamera				( int iID );
DARKSDK void		SetCameraToImageEx				( int iID, int iImage, int iWidth, int iHeight, int iGenerateCameraAlpha, DWORD dwOwnD3DFMTValue );
DARKSDK void		SetCameraToImageEx				( int iID, int iImage, int iWidth, int iHeight, int iGenerateCameraAlpha );
DARKSDK void		SetCameraToImage				( int iID, int iImage, int iWidth, int iHeight );
DARKSDK void		Reset							( int iID );
DARKSDK void		MoveLeft  						( int iID, float fStep );
DARKSDK void		MoveRight 						( int iID, float fStep );
DARKSDK void		MoveUp    						( int iID, float fStep );
DARKSDK void		MoveDown						( int iID, float fStep );

DARKSDK void		ControlWithArrowKeys			( int iID, float fVelocity, float fTurnSpeed );

DARKSDK void		SetPositionVector3				( int iID, int iVector );
DARKSDK void		GetPositionVector3				( int iVector, int iID );
DARKSDK void		SetRotationVector3				( int iID, int iVector );
DARKSDK void		GetRotationVector3				( int iVector, int iID );

DARKSDK void		Position						( int iID, float fX, float fY, float fZ );
DARKSDK void		SetRotate    					( int iID, float fX, float fY, float fZ );
DARKSDK void		SetXRotate   					( int iID, float fX );
DARKSDK void		SetYRotate   					( int iID, float fY );
DARKSDK void		SetZRotate   					( int iID, float fZ );
DARKSDK void		Point     						( int iID, float fX, float fY, float fZ );
DARKSDK void		Move      						( int iID, float fStep );
DARKSDK void		SetRange  						( int iID, float fFront, float fBack );
DARKSDK void		SetView							( int iID, int iLeft, int iTop, int iRight, int iBottom );
DARKSDK void		ClearViewEx						( int iID, DWORD dwColorValue );
DARKSDK void		ClearView						( int iID, int iRed, int iGreen, int iBlue );
DARKSDK void		SetRotationXYZ					( int iID );
DARKSDK void		SetRotationZYX					( int iID );
DARKSDK void		SetRotationZXY					( int iID );
DARKSDK void		SetFOV							( int iID, float fAngle );
DARKSDK void		SetAspect						( int iID, float fAspect );
DARKSDK void		SetClip							( int iID, int iOnOff, float fX, float fY, float fZ, float fNX, float fNY, float fNZ );
DARKSDK void		Follow 							( int iID, float fX, float fY, float fZ, float fAngle, float fDistance, float fHeight, float fSmooth, int iCollision );
DARKSDK void		TurnLeft  						( int iID, float fAngle );
DARKSDK void		TurnRight 						( int iID, float fAngle );
DARKSDK void		PitchUp   						( int iID, float fAngle );
DARKSDK void		PitchDown 						( int iID, float fAngle );
DARKSDK void		RollLeft  						( int iID, float fAngle );
DARKSDK void		RollRight						( int iID, float fAngle );
DARKSDK void		SetToObjectOrientation			( int iID, int iObjectID );

DARKSDK void		BackdropOn						( int iID );
DARKSDK void		BackdropOff						( int iID );
DARKSDK void		BackdropColor					( int iID, DWORD dwColor );
DARKSDK void		BackdropTexture					( int iID, int iImage );
DARKSDK void		BackdropScroll					( int iID, int iU, int iV );

DARKSDK void		SetCameraDepth					( int iID, int iSourceID );
DARKSDK void		BackdropOff						( int iID, int iNoDepthClear );
DARKSDK void		CopyCameraToImage				( int iID, int iImage );
DARKSDK void		SetCamerasToStereoscopic		( int iStereoscopicMode, int iCameraL, int iCameraR, int iBack, int iFront );

DARKSDK int 		GetExistEx	 					( int iID );
DARKSDK DWORD		GetXPositionEx					( int iID );
DARKSDK DWORD		GetYPositionEx 					( int iID );
DARKSDK DWORD		GetZPositionEx 					( int iID );
DARKSDK DWORD		GetXAngleEx    					( int iID );
DARKSDK DWORD		GetYAngleEx    					( int iID );
DARKSDK DWORD		GetZAngleEx    					( int iID );
DARKSDK DWORD		GetLookXEx 						( int iID );
DARKSDK DWORD		GetLookYEx 						( int iID );
DARKSDK DWORD		GetLookZEx 						( int iID );

#ifdef DARKSDK_COMPILE
		void		dbPositionCamera				( float fX, float fY, float fZ );
		void		dbRotateCamera					( float fX, float fY, float fZ );
		void		dbXRotateCamera					( float fX );
		void		dbYRotateCamera					( float fY );
		void		dbZRotateCamera					( float fZ );
		void		dbPointCamera					( float fX, float fY, float fZ );
		void		dbMoveCamera					( float fStep );
		void		dbSetCameraRange 				( float fFront, float fBack );
		void		dbSetCameraView 				( int iLeft, int iTop, int iRight, int iBottom );
		void		dbClearCameraView 				( DWORD dwColorValue );
		void		dbSetCameraRotationXYZ 			( void );
		void		dbSetCameraRotationZYX			( void );
		void		dbSetCameraFOV 					( float fAngle );
		void		dbSetCameraAspect 				( float fAspect );
		void		dbSetCameraToFollow 			( float fX, float fY, float fZ, float fAngle, float fDistance, float fHeight, float fSmooth, int iCollision );
		void		dbAutoCamOn						( void );
		void		dbAutoCamOff 					( void );
		void		dbTurnCameraLeft 				( float fAngle );
		void		dbTurnCameraRight 				( float fAngle );
		void		dbPitchCameraUp 				( float fAngle );
		void		dbPitchCameraDown 				( float fAngle );
		void		dbRollCameraLeft 				( float fAngle );
		void		dbRollCameraRight				( float fAngle );
		void		dbSetCameraToObjectOrientation	( int iObjectID );

		void		dbBackdropOn					( void ); 
		void		dbBackdropOff					( void ); 
		void		dbColorBackdrop					( DWORD dwColor ); 
		void		dbColorBackdrop					( int iID, DWORD dwColor ); 
		void		dbBackdropTexture				( int iImage ); 
		void		dbBackdropScroll				( int iU, int iV ); 

		float		dbCameraPositionX				( void );
		float		dbCameraPositionY				( void );
		float		dbCameraPositionZ				( void );
		float		dbCameraAngleX					( void );
		float		dbCameraAngleY					( void );
		float		dbCameraAngleZ					( void );

		void		dbMakeCamera					( int iID );
		void		dbDeleteCamera					( int iID );
		void		dbSetCurrentCamera				( int iID );
		void		dbSetCameraToImage				( int iID, int iImage, int iWidth, int iHeight );
		void		dbSetCameraToImage				( int iID, int iImage, int iWidth, int iHeight, int iGenAlpha );
		void		dbResetCamera					( int iID );
		void		dbMoveCameraLeft  				( int iID, float fStep );
		void		dbMoveCameraRight 				( int iID, float fStep );
		void		dbMoveCameraUp    				( int iID, float fStep );
		void		dbMoveCameraDown				( int iID, float fStep );

		void		dbControlCameraUsingArrowKeys	( int iID, float fVelocity, float fTurnSpeed );

		void		dbPositionCamera				( int iID, int iVector );
		void		dbSetVector3ToCameraPosition	( int iVector, int iID );
		void		dbRotateCamera					( int iID, int iVector );
		void		dbSetVector3ToCameraRotation	( int iVector, int iID );

		void		dbPositionCamera				( int iID, float fX, float fY, float fZ );
		void		dbRotateCamera    				( int iID, float fX, float fY, float fZ );
		void		dbXRotateCamera   				( int iID, float fX );
		void		dbYRotateCamera   				( int iID, float fY );
		void		dbZRotateCamera   				( int iID, float fZ );
		void		dbPointCamera     				( int iID, float fX, float fY, float fZ );
		void		dbMoveCamera      				( int iID, float fStep );
		void		dbSetCameraRange  				( int iID, float fFront, float fBack );
		void		dbSetCameraView					( int iID, int iLeft, int iTop, int iRight, int iBottom );
		void		dbClearCameraView				( int iID, DWORD dwColorValue );
		void		dbClearCameraView				( int iID, int iRed, int iGreen, int iBlue );
		void		dbSetCameraRotationXYZ			( int iID );
		void		dbSetCameraRotationZYX			( int iID );
		void		dbSetCameraFOV					( int iID, float fAngle );
		void		dbSetCameraAspect				( int iID, float fAspect );
		void		dbSetCameraToFollow 			( int iID, float fX, float fY, float fZ, float fAngle, float fDistance, float fHeight, float fSmooth, int iCollision );
		void		dbTurnCameraLeft  				( int iID, float fAngle );
		void		dbTurnCameraRight 				( int iID, float fAngle );
		void		dbPitchCameraUp   				( int iID, float fAngle );
		void		dbPitchCameraDown 				( int iID, float fAngle );
		void		dbRollCameraLeft  				( int iID, float fAngle );
		void		dbRollCameraRight				( int iID, float fAngle );
		void		dbSetCameraToObjectOrientation	( int iID, int iObjectID );

		void		dbBackdropOn					( int iID );
		void		dbBackdropOff					( int iID );
		void		dbBackdropColor					( int iID, DWORD dwColor );
		void		dbBackdropTexture				( int iID, int iImage );
		void		dbBackdropScroll				( int iID, int iU, int iV );

		float		dbCameraPositionX				( int iID );
		float		dbCameraPositionY 				( int iID );
		float		dbCameraPositionZ	 			( int iID );
		float		dbCameraAngleX		  			( int iID );
		float		dbCameraAngleY	    			( int iID );
		float		dbCameraAngleZ    				( int iID );
		float		dbCameraLookX	 				( int iID );
		float		dbCameraLookY	 				( int iID );
		float		dbCameraLookZ 					( int iID );

		void*		dbGetCameraInternalData			( int iID );

		void		dbSetAutoCam					( float fX, float fY, float fZ, float fRadius );
		void 		dbCameraInternalUpdate			( int iID );

		void  		dbStartScene					( void );
		int   		dbFinishScene					( void );

		void  		dbStartSceneEx					( int iMode );
		int	  		dbGetRenderCamera				( void );

		D3DXMATRIX	dbGetViewMatrix					( int iID );
		D3DXMATRIX	dbGetProjectionMatrix			( int iID );

		// lee - 300706 - GDK fixes
		void		dbTextureBackdrop				( int iImage ); 
		void		dbScrollBackdrop				( int iU, int iV ); 
		void		dbTextureBackdrop				( int iID, int iImage );
		void		dbScrollBackdrop				( int iID, int iU, int iV );

		// mike - 131106
		int			dbFinishSceneEx					( bool bKnowInAdvanceCameraIsUsed );

		// mike - 220107
		void		dbSetCameraClip					( int iID, int iOnOff, float fX, float fY, float fZ, float fNX, float fNY, float fNZ );

		// mike - 060207
		int 		dbCameraExist 					( int iID );

		// lee - 081108 
		void		dbRunCode						( int iMode );

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#endif _CCAMERA_H_