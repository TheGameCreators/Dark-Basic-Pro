#ifndef _CCAMERA_H_
#define _CCAMERA_H_

#include ".\..\data\cdata.h"

#include "CCameraData.h"
#include "CCameraManager.h"

#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "d3d8.lib"   )
#pragma comment ( lib, "d3dx8.lib"  )
#pragma comment ( lib, "dxerr8.lib" )

#include <d3d8.h>
#include <Dxerr8.h>
#include <D3dx8tex.h>
#include <D3dx8core.h>
#include <basetsd.h>
#include <stdio.h>
#include <math.h>
#include <D3DX8.h>
#include <d3d8types.h>

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>   
#include <windowsx.h> 

class CCamera
{
	private:
		CCameraManager*		m_CameraManager;	// pointer to camera manager
		tagCameraData*		m_ptr;				// pointer to camera data
		LPDIRECT3DDEVICE8	m_pD3D;				// pointer to d3d device

		bool UpdatePtr      ( int iID );		// update internal data pointer
		void InternalUpdate ( void );			// update any changes to camera
		
	public:
		CCamera  ( CCameraManager* pCameraManager );								// constructor
		~CCamera ( );																// destructor

		void SetDevice ( LPDIRECT3DDEVICE8 lpD3D );									// set d3d device

		/////////////////////////////////////////////
		// creation /////////////////////////////////
		/////////////////////////////////////////////
		void Create           ( int iID );											// create a camera
		void Destroy          ( int iID );											// destroy a camera
		void SetCurrentCamera ( int iID );											// set the current camera
		void SetDefaultValues ( int iID );											// sets all default values for camera
		/////////////////////////////////////////////
		
		/////////////////////////////////////////////
		// general control //////////////////////////
		/////////////////////////////////////////////
		void Position ( int iID, float fX, float fY, float fZ );					// setup position
		void Rotate   ( int iID, float fX, float fY, float fZ );					// rotate on all axis
		void XRotate  ( int iID, float fX );										// x rotate
		void YRotate  ( int iID, float fY );										// y rotate
		void ZRotate  ( int iID, float fZ );										// z rotate
		void Point    ( int iID, float fX, float fY, float fZ );					// point to position
		void Move     ( int iID, float fStep );										// move in current direction

		void SetRange  ( int iID, int iFront, int iBack );							// set front and back viewing distances
		void SetView   ( int iID, int iLeft, int iTop, int iRight, int iBottom );	// set where display is drawn on screen
		void ClearView ( int iID, int iRed, int iGreen, int iBlue );				// clear camera to specified colour

		void SetRotationXYZ ( int iID );											// set order of rotation to xyz
		void SetRotationZYX ( int iID );											// set order of rotation to zyx
		void SetFOV         ( int iID, float fAngle );								// set field of view

		void Follow ( int iID, float fX, float fY, float fZ, float fAngle, int iDistance, int iHeight, int iSmooth, bool bCollision );	// follow object

		void SetAutoCamOn  ( int iID );												// automatically reposition camera when new objects are created
		void SetAutoCamOff ( int iID );												// no auto camera changes

		void TurnLeft  ( int iID, float fAngle );									// turn left
		void TurnRight ( int iID, float fAngle );									// turn right
		void PitchUp   ( int iID, float fAngle );									// pitch up
		void PitchDown ( int iID, float fAngle );									// pitch down
		void RollLeft  ( int iID, float fAngle );									// roll left
		void RollRight ( int iID, float fAngle );									// roll right

		void SetToObjectOrientation ( int iID, int iObjectID );						// set to same orientation as specified object
		/////////////////////////////////////////////

		/////////////////////////////////////////////
		// information //////////////////////////////
		/////////////////////////////////////////////
		float GetXPosition ( int iID );												// get x pos
		float GetYPosition ( int iID );												// get y pos
		float GetZPosition ( int iID );												// get z pos
		float GetXAngle    ( int iID );												// get x angle
		float GetYAngle    ( int iID );												// get y angle
		float GetZAngle    ( int iID );												// get z angle
		/////////////////////////////////////////////
};

#endif _CCAMERA_H_