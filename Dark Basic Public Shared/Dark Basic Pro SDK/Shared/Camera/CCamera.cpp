#include "ccamera.h"

CCamera::CCamera ( CCameraManager* pCameraManager )
{
	m_CameraManager = pCameraManager;
	m_ptr           = NULL;
}

CCamera::~CCamera ( )
{

}

bool CCamera::UpdatePtr ( int iID )
{
	m_ptr  = NULL;
	m_ptr  = m_CameraManager->GetData ( iID );

	if ( m_ptr == NULL )
		return false;

	return true;
}

void CCamera::SetDevice ( LPDIRECT3DDEVICE8 lpD3D )
{
	m_pD3D = lpD3D;
}

void CCamera::Create ( int iID )
{
	tagCameraData	cData;		// internal camera data

	// clear out structures
	memset ( &cData, 0, sizeof ( cData ) );
	
	// add data to list
	m_CameraManager->Add ( &cData, iID );

	// setup default values
	this->SetDefaultValues ( iID );
}

void CCamera::Destroy ( int iID )
{

}

void CCamera::InternalUpdate ( void )
{
	D3DXMatrixLookAtLH
						( 
							&m_ptr->matView,
							&D3DXVECTOR3 ( m_ptr->fX, m_ptr->fY, m_ptr->fZ ),
							&D3DXVECTOR3 ( 0.0f, 0.0f,  0.0f ),
							&D3DXVECTOR3 ( 0.0f, 1.0f,  0.0f )
						);

	//D3DXMatrixScaling   ( &m_ptr->matRotateX, 1.0, 1.0, 1.0 );
	//D3DXMatrixRotationX ( &m_ptr->matRotateX, m_ptr->fX );
	//D3DXMatrixMultiply  ( &m_ptr->matView, &m_ptr->matView, &m_ptr->matRotateX );

	//D3DXMatrixRotationY ( &m_ptr->matRotateY, m_ptr->fY );
	//D3DXMatrixMultiply  ( &m_ptr->matView, &m_ptr->matView, &m_ptr->matRotateY );

	//D3DXMatrixRotationZ ( &m_ptr->matRotateZ, m_ptr->fZ );
	//D3DXMatrixMultiply  ( &m_ptr->matView, &m_ptr->matView, &m_ptr->matRotateZ );
	
	m_pD3D->SetTransform ( D3DTS_VIEW, &m_ptr->matView );
}

void CCamera::SetDefaultValues ( int iID )
{
	// setup all default values for the camera

	// update internal pointer
	if ( !this->UpdatePtr ( iID ) )
		return;

	D3DVIEWPORT8	viewport;	// current viewport
	float			fAspect;	// aspect ration
	float			fFOV;		// field of view

	// clear out any structures
	memset ( &viewport, 0, sizeof ( viewport ) );
	
	// get the current viewport
	m_pD3D->GetViewport ( &viewport );

	// work out values for perspective, fov, clipping and positions
	m_ptr->fAspect = ( float ) viewport.Width / ( float ) viewport.Height;	// aspect ratio
	m_ptr->fFOV    = D3DX_PI / 4;											// field of view
	m_ptr->fZNear  = 1.0;
	m_ptr->fZFar   = 3000.0;
	m_ptr->fX      = 0.0;
	m_ptr->fY      = 0.0;
	m_ptr->fZ      = 0.0;
	m_ptr->fXRot   = 0.0;
	m_ptr->fYRot   = 0.0;
	m_ptr->fZRot   = 0.0;
	
	// create a perspective matrix
	D3DXMatrixPerspectiveFovLH ( &m_ptr->matProj, m_ptr->fFOV, m_ptr->fAspect, m_ptr->fZNear, m_ptr->fZFar );

	// set the transform
	m_pD3D->SetTransform ( D3DTS_PROJECTION, &m_ptr->matProj );

	this->SetAutoCamOff  ( iID );															// auto cam is off
	this->SetRotationXYZ ( iID );															// default rotation
	this->SetRange       ( iID, 1, 3000 );													// default range
	this->SetView        ( iID, viewport.X, viewport.Y, viewport.Width, viewport.Height );	// default view
	this->Position       ( iID, 0.0, 0.0, 0.0 );											// default position
}

void CCamera::SetCurrentCamera ( int iID )
{

}

void CCamera::Position ( int iID, float fX, float fY, float fZ )
{
	// sets the position of the camera

	// update internal pointer
	if ( !this->UpdatePtr ( iID ) )
		return;

	m_ptr->fX = fX;
	m_ptr->fY = fY;
	m_ptr->fZ = fZ;

	D3DXMatrixLookAtLH 
						( 
							&m_ptr->matView,
							&D3DXVECTOR3 (   fX,   fY,    fZ ),
							&D3DXVECTOR3 ( 0.0f, 0.0f,  0.0f ),
							&D3DXVECTOR3 ( 0.0f, 1.0f,  0.0f )
						);

	m_pD3D->SetTransform ( D3DTS_VIEW, &m_ptr->matView );
}

void CCamera::Rotate ( int iID, float fX, float fY, float fZ )
{

}

void CCamera::XRotate ( int iID, float fX )
{
	// rotate the camera on it's x axis

	// update internal pointer
	if ( !this->UpdatePtr ( iID ) )
		return;

	D3DXMatrixLookAtLH 
						( 
							&m_ptr->matView,
							&D3DXVECTOR3 ( m_ptr->fX,   m_ptr->fY,    m_ptr->fZ ),
							&D3DXVECTOR3 ( 0.0f, 0.0f,  0.0f ),
							&D3DXVECTOR3 ( 0.0f, 1.0f,  0.0f )
						);

	D3DXMatrixRotationX ( &m_ptr->matRotateX, fX );
	D3DXMatrixMultiply  ( &m_ptr->matView, &m_ptr->matView, &m_ptr->matRotateX );

	m_pD3D->SetTransform ( D3DTS_VIEW, &m_ptr->matView );
}

void CCamera::YRotate ( int iID, float fY )
{
	// rotate the camera on it's y axis

	// update internal pointer
	if ( !this->UpdatePtr ( iID ) )
		return;

	D3DXMatrixLookAtLH 
						( 
							&m_ptr->matView,
							&D3DXVECTOR3 ( m_ptr->fX,   m_ptr->fY,    m_ptr->fZ ),
							&D3DXVECTOR3 ( 0.0f, 0.0f,  0.0f ),
							&D3DXVECTOR3 ( 0.0f, 1.0f,  0.0f )
						);

	D3DXMatrixRotationY ( &m_ptr->matRotateY, fY );
	D3DXMatrixMultiply  ( &m_ptr->matView, &m_ptr->matView, &m_ptr->matRotateY );

	m_pD3D->SetTransform ( D3DTS_VIEW, &m_ptr->matView );
}

void CCamera::ZRotate ( int iID, float fZ )
{

}

void CCamera::Point ( int iID, float fX, float fY, float fZ )
{

}

void CCamera::Move ( int iID, float fStep )
{
	// update internal pointer
	if ( !this->UpdatePtr ( iID ) )
		return;

	m_ptr->fX += fStep;

	this->InternalUpdate ( );

	/*
	D3DXMatrixLookAtLH 
					( 
						&m_ptr->matView,
						&D3DXVECTOR3 ( m_ptr->fX,   m_ptr->fY,    m_ptr->fZ ),
						&D3DXVECTOR3 ( 0.0f, 0.0f,  0.0f ),
						&D3DXVECTOR3 ( 0.0f, 1.0f,  0.0f )
					);

	m_pD3D->SetTransform ( D3DTS_VIEW, &m_ptr->matView );
	*/
}

void CCamera::SetRange ( int iID, int iFront, int iBack )
{
	// sets the near and far z ranges for the camera

	// update internal pointer
	if ( !this->UpdatePtr ( iID ) )
		return;

	m_ptr->fZNear = iFront;
	m_ptr->fZFar  = iBack;
	
	// create a perspective matrix
	D3DXMatrixPerspectiveFovLH ( &m_ptr->matProj, m_ptr->fFOV, m_ptr->fAspect, m_ptr->fZNear, m_ptr->fZFar );

	// set the transform
	m_pD3D->SetTransform ( D3DTS_PROJECTION, &m_ptr->matProj );
}

void CCamera::SetView ( int iID, int iLeft, int iTop, int iRight, int iBottom )
{
	// set up the viewing area for the 3D scene
	
	if ( !this->UpdatePtr ( iID ) )
		return;

	D3DVIEWPORT8 viewport;
	
	memset ( &viewport, 0, sizeof ( viewport ) );
	
	viewport.X      = iLeft;
	viewport.Y      = iTop;
	viewport.Width  = iRight;
	viewport.Height = iBottom;
	viewport.MinZ   = 0.0;
	viewport.MaxZ   = 1.0;
	
	m_pD3D->SetViewport ( &viewport );
}

void CCamera::ClearView ( int iID, int iRed, int iGreen, int iBlue )
{
	// clears the camera view to the specified colour

	if ( !this->UpdatePtr ( iID ) )
		return;

	m_pD3D->Clear ( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB ( iRed, iGreen, iBlue ), 1.0f, 0 );
}

void CCamera::SetRotationXYZ ( int iID )
{

}

void CCamera::SetRotationZYX ( int iID )
{

}

void CCamera::SetFOV ( int iID, float fAngle )
{

}

void CCamera::Follow ( int iID, float fX, float fY, float fZ, float fAngle, int iDistance, int iHeight, int iSmooth, bool bCollision )
{

}

void CCamera::SetAutoCamOn ( int iID )
{

}

void CCamera::SetAutoCamOff ( int iID )
{

}

void CCamera::TurnLeft ( int iID, float fAngle )
{

}

void CCamera::TurnRight ( int iID, float fAngle )
{

}

void CCamera::PitchUp ( int iID, float fAngle )
{

}

void CCamera::PitchDown ( int iID, float fAngle )
{

}

void CCamera::RollLeft ( int iID, float fAngle )
{

}

void CCamera::RollRight ( int iID, float fAngle )
{

}

void CCamera::SetToObjectOrientation ( int iID, int iObjectID )
{

}

float CCamera::GetXPosition ( int iID )
{
	return 0.0;
}

float CCamera::GetYPosition ( int iID )
{
	return 0.0;
}

float CCamera::GetZPosition ( int iID )
{
	return 0.0;
}

float CCamera::GetXAngle ( int iID )
{
	return 0.0;
}

float CCamera::GetYAngle ( int iID )
{
	return 0.0;
}

float CCamera::GetZAngle ( int iID )
{
	return 0.0;
}