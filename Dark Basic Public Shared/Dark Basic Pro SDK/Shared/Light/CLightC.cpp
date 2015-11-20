
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDK3DMaths.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKBasic3D.h"
//	#include ".\..\DBOFormat\DBOData.h"         // For sObject* / GetObject
	#undef DARKSDK
	#undef DBPRO_GLOBAL
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#else
	#include ".\..\DBOFormat\DBOData.h"         // For sObject* / GetObject
#endif

#include "CLightC.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


namespace
{
    float EulerWrapAngle(float Angle)
    {
        float NewAngle = fmod(Angle, 360.0f);
        if (NewAngle < 0.0f)
            NewAngle+=360.0f;
        return NewAngle;
    }

    D3DXVECTOR3 GetAngleFromDirectionVector(D3DXVECTOR3 Vector)
    {
	    // Find Y and then X axis rotation
	    double yangle=atan2(Vector.x, Vector.z);
	    if(yangle<0.0) yangle+=D3DXToRadian(360.0);
	    if(yangle>=D3DXToRadian(360.0)) yangle-=D3DXToRadian(360.0);

	    D3DXMATRIX yrotate;
	    D3DXMatrixRotationY ( &yrotate, (float)-yangle );
	    D3DXVec3TransformCoord ( &Vector, &Vector, &yrotate );

	    double xangle=-atan2(Vector.y, Vector.z);
	    if(xangle<0.0) xangle+=D3DXToRadian(360.0);
	    if(xangle>=D3DXToRadian(360.0)) xangle-=D3DXToRadian(360.0);

        return D3DXVECTOR3( EulerWrapAngle(D3DXToDegree((float)xangle)),
                	        EulerWrapAngle(D3DXToDegree((float)yangle)),
                    	    0.0f);
    }
}

//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DBPRO_GLOBAL GlobStruct*			g_pGlob	= NULL;

namespace
{
    typedef IDirect3DDevice9*		( *GFX_GetDirect3DDevicePFN )	( void );
    typedef D3DXVECTOR3				( *GetVectorPFN )				( int );
    typedef void					( *SetVectorPFN )				( int, float, float, float );
    typedef int						( *GetExistPFN )				( int );
    typedef sObject*            	( *GetObjectPFN )               ( int );

    GetVectorPFN					g_Types_GetVector;
    SetVectorPFN					g_Types_SetVector;
    GetExistPFN						g_Types_GetExist;
    GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;
    GetObjectPFN                   	g_Basic3D_GetObject;
    CLightManager					m_LightManager;
    tagLightData*					m_ptr;
    LPDIRECT3DDEVICE9				m_pD3D;
    int								m_iAmbientParcentage;
    int								m_iAmbientRed;
    int								m_iAmbientGreen;
    int								m_iAmbientBlue;
    float							g_fFogStartDistance;
    float							g_fFogEndDistance;
    DWORD                          	g_dwFogColor;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// FUNCTION LISTINGS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void ConstructorD3D ( HINSTANCE hSetup )
{
	// setup the DLL
	
	bool bFromCore = false;

	if(hSetup) bFromCore=true;

	
	#ifndef DARKSDK_COMPILE
	// check the handles to the DLLs are ok
	if ( !hSetup )
	{
		// if they aren't attempt to load the DLLs manually
		hSetup = LoadLibrary ( "DBProSetupDebug.dll" );
	}
	#endif


	#ifndef DARKSDK_COMPILE
		g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	#else
		bFromCore = true;
		g_GFX_GetDirect3DDevice = dbGetDirect3DDevice;
	#endif

	m_pD3D  = g_GFX_GetDirect3DDevice ( );

	// Default Ambient Settings
	m_iAmbientParcentage	= 25;
	m_iAmbientRed			= 255;
	m_iAmbientGreen			= 255;
	m_iAmbientBlue			= 255;

	// Default Fog Settings
	g_fFogStartDistance = 0.0f;
	g_fFogEndDistance   = 3000.0f;
    g_dwFogColor        = 0;

	// Default Light
	if(bFromCore)
	{
		// Light zero is a directional light
		int iID=0;
		Make ( iID );

		// Set Ambient State
		float fR = m_iAmbientRed * (m_iAmbientParcentage/100.0f);
		float fG = m_iAmbientGreen * (m_iAmbientParcentage/100.0f);
		float fB = m_iAmbientBlue * (m_iAmbientParcentage/100.0f);
		m_pD3D->SetRenderState ( D3DRS_AMBIENT, D3DCOLOR_ARGB ( 255, (DWORD)fR, (DWORD)fG, (DWORD)fB ) );

		// Set Fog State
		m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );
	}
}

DARKSDK void Constructor ( HINSTANCE hSetup )
{
	ConstructorD3D(hSetup);
}

DARKSDK void SetSpecularOn ( void )
{
	m_pD3D->SetRenderState ( D3DRS_SPECULARENABLE, TRUE );
}

DARKSDK void SetSpecularOff ( void )
{
	m_pD3D->SetRenderState ( D3DRS_SPECULARENABLE, FALSE );
}

DARKSDK void DestructorD3D ( void )
{
    m_LightManager.DeleteAll( m_pD3D );
}

DARKSDK void Destructor ( void )
{
	DestructorD3D();
}

DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

DARKSDK void PassCoreData( LPVOID pGlobPtr )
{
	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;

	// Pass Default Ambient Percentage (for material construction)
	g_pGlob->iFogState = 0;

	// Assign function ptrs for vector handling
	#ifndef DARKSDK_COMPILE
		g_Types_GetVector					= ( GetVectorPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?GetVector3@@YA?AUD3DXVECTOR3@@H@Z" );
		g_Types_SetVector					= ( SetVectorPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?SetVector3@@YAXHMMM@Z" );
		g_Types_GetExist					= ( GetExistPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?GetExist@@YAHH@Z" );
        g_Basic3D_GetObject                 = ( GetObjectPFN )                  GetProcAddress ( g_pGlob->g_Basic3D, "?GetObjectA@@YAPAUsObject@@H@Z" );
	#else
		g_Types_GetVector = dbGetVector3;
		g_Types_SetVector = dbSetVector3;
		g_Types_GetExist = dbGet3DMathsExist;
        g_Basic3D_GetObject = dbGetObject;
	#endif
}
//DBPRO_GLOBAL GetObjectPFN                   g_Basic3D_GetObject;

DARKSDK void RefreshD3D ( int iMode )
{
	if(iMode==0)
	{
		// Remove all traces of old D3D usage
		DestructorD3D();
	}
	if(iMode==1)
	{
		// Get new D3D and recreate everything D3D related
		ConstructorD3D ( g_pGlob->g_GFX );
		PassCoreData ( g_pGlob );
	}
}

DARKSDK bool UpdatePtr ( int iID )
{
	// update internal data for the sprite
	m_ptr  = m_LightManager.GetData ( iID );

	if ( m_ptr == NULL )
		return false;

	return true;
}

DARKSDK void* GetLightData( int iID )
{
    UpdatePtr( iID );
    return (void*)m_ptr;
}

DARKSDK void Make ( int iID )
{
	// variable declarations
	tagLightData	m_Data;

	// clean out structure
	memset ( &m_Data,       0, sizeof ( m_Data    ) );
	memset ( &m_Data.light, 0, sizeof ( D3DLIGHT9 ) );

	// set structure data
	m_Data.eType = eDirectionalLight;
	m_Data.bEnable = true;
	m_Data.fRange = 1000.0f;
	m_Data.dwColor = D3DCOLOR_ARGB ( 255,255,255,255 );
	m_LightManager.Add ( &m_Data, iID );

	// default new light is point (lee - 230306 - u6b4 - aligned default from basic3d)
	if(iID==0)
		SetDirectional ( 0, 0.1f, -0.7f, 0.2f ); // SetDirectional ( 0, 0.1f, -0.5f, 0.5f );
	else
		SetPoint( iID, 0.0f, 0.0f, 0.0f );

	// light on by default
	m_pD3D->LightEnable ( iID, TRUE );
}

DARKSDK void Delete ( int iID )
{
	// Delete Light
	m_LightManager.Delete ( iID, m_pD3D );
}

DARKSDK bool GetExist ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return false;

	return true;
}

DARKSDK int GetType ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return -1;

	return m_ptr->light.Type;
}

DARKSDK float GetXPosition ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0.0f;

	return m_ptr->light.Position.x;
}

DARKSDK float GetYPosition ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0.0f;

	return m_ptr->light.Position.y;
}

DARKSDK float GetZPosition ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0.0f;

	return m_ptr->light.Position.z;
}

DARKSDK float GetXDirection ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0.0f;

	return m_ptr->light.Direction.x;
}

DARKSDK float GetYDirection ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0.0f;

	return m_ptr->light.Direction.y;
}

DARKSDK float GetZDirection ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0.0f;

	return m_ptr->light.Direction.z;
}

DARKSDK bool GetVisible ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return false;

	return m_ptr->bEnable;
}

DARKSDK int GetRange ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0;

	return ( int ) m_ptr->light.Range;
}

DARKSDK void MakeEx ( int iID )
{
	// create a new light
	if ( iID==0 )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOZERO );
		return;
	}
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTALREADYEXISTS );
		return;
	}

	// Make light for real
	Make ( iID );
}

DARKSDK void DeleteEx ( int iID )
{
	// delete a light
	if ( iID==0 )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOZERO );
		return;
	}
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	// Delete Light
	Delete ( iID );
}

DARKSDK void SetPoint ( int iID, float fX, float fY, float fZ )
{
	// setup a point light
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	// fill in data
	m_ptr->eType = ePointLight;
	memset ( &m_ptr->light, 0, sizeof ( D3DLIGHT9 ) );
	m_ptr->light.Type         = D3DLIGHT_POINT;						// set to point light
	m_ptr->light.Diffuse.r    = 1.0f;								// diffuse colour to reflect all
	m_ptr->light.Diffuse.g    = 1.0f;								// diffuse colour to reflect all
	m_ptr->light.Diffuse.b    = 1.0f;								// diffuse colour to reflect all
	m_ptr->light.Position     = D3DXVECTOR3 ( fX, fY, fZ );			// position of light
	m_ptr->light.Range        = m_ptr->fRange;								// light range
	m_ptr->light.Attenuation0 = 1.0f;								// light intensity over distance
	m_ptr->light.Attenuation2 = 1.0f/(float)pow((m_ptr->fRange/3.0f),2);;								// light intensity over distance

	// set standard specular light
	m_ptr->light.Specular.r  = 1.0f;
	m_ptr->light.Specular.g  = 1.0f;
	m_ptr->light.Specular.b  = 1.0f;
	m_ptr->light.Specular.a  = 1.0f;

	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void SetSpot ( int iID, float fInner, float fOuter )
{
	// setup a spot light
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	// fill in data
	m_ptr->eType = eSpotLight;
	memset ( &m_ptr->light, 0, sizeof ( D3DLIGHT9 ) );
	m_ptr->light.Type         = D3DLIGHT_SPOT;						// set to spot light
	m_ptr->light.Diffuse.r    = 1.0f;								// diffuse colour to reflect all
	m_ptr->light.Diffuse.g    = 1.0f;								// diffuse colour to reflect all
	m_ptr->light.Diffuse.b    = 1.0f;								// diffuse colour to reflect all
	m_ptr->light.Position     = D3DXVECTOR3 ( 0.0f,  0.0f, 0.0f );	// position of light
	m_ptr->light.Direction    = D3DXVECTOR3 ( 0.0f, -1.0f, 0.0f );	// direction of light
	m_ptr->light.Range        = m_ptr->fRange;							// light range
	m_ptr->light.Attenuation0 = 1.0f;								// light intensity over distance
	m_ptr->light.Theta        = D3DXToRadian ( fInner );			// angle of inner cone of spot light
	m_ptr->light.Phi          = D3DXToRadian ( fOuter );			// angle of outer cone of spot light

	// lee - 050406 - u6rc6 - added falloff to make it work
	m_ptr->light.Falloff	  = 1.0f;
	
	// set standard specular light
	m_ptr->light.Specular.r  = 1.0f;
	m_ptr->light.Specular.g  = 1.0f;
	m_ptr->light.Specular.b  = 1.0f;
	m_ptr->light.Specular.a  = 1.0f;

	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void SetDirectional ( int iID, float fX, float fY, float fZ )
{
	// setup a directional light
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	// lee - 230306 - u6b4 - these settings updated from Basic3D which seemed to overrule the old default start (ie no specular)
	m_ptr->eType = eDirectionalLight;
	memset ( &m_ptr->light, 0, sizeof ( D3DLIGHT9 ) );
	m_ptr->light.Type       = D3DLIGHT_DIRECTIONAL;				// set to directional light
	m_ptr->light.Diffuse.r  = 1.0f;								// diffuse colour to reflect all
	m_ptr->light.Diffuse.g  = 1.0f;								// diffuse colour to reflect all
	m_ptr->light.Diffuse.b  = 1.0f;								// diffuse colour to reflect all
	m_ptr->light.Direction  = D3DXVECTOR3 ( fX, fY, fZ );		// light direction
	m_ptr->light.Range		= 5000.0f;							// defaul range
	m_ptr->light.Attenuation0 = 1.0f;							// light intensity over distance

	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void SetStaticMode ( int iID, int iFlag )
{
	// MessageBox ( NULL, "DX10 ONLY", "SetStaticMode", MB_OK );
}

DARKSDK void SetRange ( int iID, float fRange )
{
	// sets the range of the specified light
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	m_ptr->fRange = fRange;
	m_ptr->light.Range = fRange;
	m_ptr->light.Attenuation0 = 1.0f;
	m_ptr->light.Attenuation2 = 1.0f/(float)pow((m_ptr->fRange/3.0f),2);

	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void SetSpecular ( int iID, float fA, float fR, float fG, float fB )
{
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	D3DCOLORVALUE color = { fR, fG, fB, fA };
	m_ptr->light.Specular = color;
}

DARKSDK void SetAmbient ( int iID, float fA, float fR, float fG, float fB )
{
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	D3DCOLORVALUE color = { fR, fG, fB, fA };
	m_ptr->light.Ambient = color;
}

DARKSDK void SetFalloff ( int iID, float fValue )
{
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	m_ptr->light.Falloff = fValue;
}

DARKSDK void SetAttenuation0 ( int iID, float fValue )
{
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	m_ptr->light.Attenuation0 = fValue;
}

DARKSDK void SetAttenuation1 ( int iID, float fValue )
{
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	m_ptr->light.Attenuation1 = fValue;
}

DARKSDK void SetAttenuation2 ( int iID, float fValue )
{
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	m_ptr->light.Attenuation2 = fValue;
}

DARKSDK void SetToObject ( int iID, int iObjectID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}
    sObject* Object = g_Basic3D_GetObject ( iObjectID );
    if ( !Object )
    {
        RunTimeError ( RUNTIMEERROR_B3DMODELNOTEXISTS );
        return;
    }

    // Position the light at the same point as the object
	m_ptr->light.Position = Object->position.vecPosition;

	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void SetToObjectOrientation ( int iID, int iObjectID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}
    sObject* Object = g_Basic3D_GetObject ( iObjectID );
    if ( !Object )
    {
        RunTimeError ( RUNTIMEERROR_B3DMODELNOTEXISTS );
        return;
    }

    // Rotate the light down the objects look vector
    m_ptr->light.Direction = Object->position.vecLook;

	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void SetColor ( int iID, DWORD dwColor )
{
	// Set color of light
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	// fill in properties
	int iR = (dwColor>>16) & 255;
	int iG = (dwColor>>8) & 255;
	int iB = dwColor & 255;
	m_ptr->light.Diffuse.r  = ( float ) iR / 255.0f;
	m_ptr->light.Diffuse.g  = ( float ) iG / 255.0f;
	m_ptr->light.Diffuse.b  = ( float ) iB / 255.0f;
	
	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void SetColorEx ( int iID, int iR, int iG, int iB )
{
	// Set color of light
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	// fill in properties
	m_ptr->light.Diffuse.r  = ( float ) iR / 255.0f;
	m_ptr->light.Diffuse.g  = ( float ) iG / 255.0f;
	m_ptr->light.Diffuse.b  = ( float ) iB / 255.0f;
	
	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void Position ( int iID, float fX, float fY, float fZ )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	m_ptr->light.Position = D3DXVECTOR3 ( fX, fY, fZ );

	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void Rotate ( int iID, float fX, float fY, float fZ )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	D3DXMATRIX	matRotateX,
				matRotateY,
				matRotateZ, 
				matRotate;

	D3DXMatrixRotationX   ( &matRotate, D3DXToRadian(fX) );
	D3DXMatrixRotationY   ( &matRotateY, D3DXToRadian(fY) );
	D3DXMatrixMultiply    ( &matRotate, &matRotate, &matRotateY );
	D3DXMatrixRotationZ   ( &matRotateZ, D3DXToRadian(fZ) );
	D3DXMatrixMultiply    ( &matRotate, &matRotate, &matRotateZ );

	m_ptr->light.Direction.x=0.0f;
	m_ptr->light.Direction.y=0.0f;
	m_ptr->light.Direction.z=1.0f;
	D3DXVec3TransformCoord ( ( D3DXVECTOR3* ) &m_ptr->light.Direction, ( D3DXVECTOR3* ) &m_ptr->light.Direction, &matRotate );

	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void Point ( int iID, float fX, float fY, float fZ )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	// Create Direction From relative offset
	m_ptr->light.Direction = m_ptr->light.Position;
	m_ptr->light.Direction.x = fX - m_ptr->light.Direction.x;
	m_ptr->light.Direction.y = fY - m_ptr->light.Direction.y;
	m_ptr->light.Direction.z = fZ - m_ptr->light.Direction.z;
	D3DXVec3Normalize( (D3DXVECTOR3*)&m_ptr->light.Direction, (D3DXVECTOR3*)&m_ptr->light.Direction );

	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void Hide ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	m_pD3D->LightEnable ( iID, false );
	m_ptr->bEnable = false;
}

DARKSDK void Show ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}

	m_pD3D->LightEnable ( iID, true );
	m_ptr->bEnable = true;
}

DARKSDK void SetNormalizationOn ( void )
{
	m_pD3D->SetRenderState ( D3DRS_NORMALIZENORMALS, TRUE );
}

DARKSDK void SetNormalizationOff ( void )
{
	m_pD3D->SetRenderState ( D3DRS_NORMALIZENORMALS, FALSE );
}

DARKSDK void SetAmbient ( int iPercent )
{
	// set global value
	m_iAmbientParcentage = iPercent;

	// Set Ambient State
	float fR = m_iAmbientRed * (m_iAmbientParcentage/100.0f);
	float fG = m_iAmbientGreen * (m_iAmbientParcentage/100.0f);
	float fB = m_iAmbientBlue * (m_iAmbientParcentage/100.0f);
	m_pD3D->SetRenderState ( D3DRS_AMBIENT, D3DCOLOR_ARGB ( 255, (DWORD)fR, (DWORD)fG, (DWORD)fB ) );
}

DARKSDK int GetAmbient ( void )
{
	// 301007 - return ambient light percentage
	return m_iAmbientParcentage;
}

DARKSDK void SetAmbientColor ( DWORD dwColor )
{
	int iR = (dwColor>>16) & 255;
	int iG = (dwColor>>8) & 255;
	int iB = dwColor & 255;
	m_iAmbientRed = iR;
	m_iAmbientGreen = iG;
	m_iAmbientBlue = iB;

	// Set the ambience
	float fR = m_iAmbientRed * (m_iAmbientParcentage/100.0f);
	float fG = m_iAmbientGreen * (m_iAmbientParcentage/100.0f);
	float fB = m_iAmbientBlue * (m_iAmbientParcentage/100.0f);
	m_pD3D->SetRenderState ( D3DRS_AMBIENT, D3DCOLOR_ARGB ( 255, (DWORD)fR, (DWORD)fG, (DWORD)fB ) );
}

DARKSDK void FogOn ( void )
{
	if(g_pGlob) g_pGlob->iFogState=1;
	m_pD3D->SetRenderState ( D3DRS_FOGENABLE, TRUE );
	m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, g_dwFogColor );
	m_pD3D->SetRenderState ( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
	m_pD3D->SetRenderState ( D3DRS_FOGSTART, *( DWORD * ) ( &g_fFogStartDistance ) );
	m_pD3D->SetRenderState ( D3DRS_FOGEND,   *( DWORD * ) ( &g_fFogEndDistance   ) );
}

DARKSDK void FogOff ( void )
{
	if(g_pGlob) g_pGlob->iFogState=0;
	m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );
}

DARKSDK void SetFogColor ( DWORD dwColor )
{
    g_dwFogColor = dwColor;
	m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, dwColor );
}

DARKSDK void SetFogColorEx ( int iR, int iG, int iB, int iA )
{
    SetFogColor( D3DCOLOR_RGBA ( iR, iG, iB, iA ) );
}

DARKSDK void SetFogColorEx ( int iR, int iG, int iB )
{
    SetFogColor( D3DCOLOR_RGBA ( iR, iG, iB, 0 ) );
}

DARKSDK void SetFogDistance ( int iDistance )
{
	g_fFogEndDistance = ( float ) iDistance;
	m_pD3D->SetRenderState ( D3DRS_FOGSTART, *( DWORD * ) ( &g_fFogStartDistance ) );
	m_pD3D->SetRenderState ( D3DRS_FOGEND,   *( DWORD * ) ( &g_fFogEndDistance   ) );
}

DARKSDK void SetFogDistance ( int iStartDistance, int iFinishDistance )
{
	g_fFogStartDistance = ( float ) iStartDistance;
	g_fFogEndDistance = ( float ) iFinishDistance;
	m_pD3D->SetRenderState ( D3DRS_FOGSTART, *( DWORD * ) ( &g_fFogStartDistance ) );
	m_pD3D->SetRenderState ( D3DRS_FOGEND,   *( DWORD * ) ( &g_fFogEndDistance   ) );
}

DARKSDK void  SetPositionVector3 ( int iID, int iVector )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	m_ptr->light.Position = g_Types_GetVector ( iVector );

	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void GetPositionVector3 ( int iVector, int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	g_Types_SetVector ( iVector, m_ptr->light.Position.x, m_ptr->light.Position.y, m_ptr->light.Position.z );
}

DARKSDK void  SetRotationVector3 ( int iID, int iVector )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	D3DXVECTOR3 vec = g_Types_GetVector ( iVector );

	D3DXMATRIX	matRotateX,
				matRotateY,
				matRotateZ, 
				matRotate;

	D3DXMatrixRotationX   ( &matRotate, D3DXToRadian(vec.x) );
	D3DXMatrixRotationY   ( &matRotateY, D3DXToRadian(vec.y) );
	D3DXMatrixMultiply    ( &matRotate, &matRotate, &matRotateY );
	D3DXMatrixRotationZ   ( &matRotateZ, D3DXToRadian(vec.z) );
	D3DXMatrixMultiply    ( &matRotate, &matRotate, &matRotateZ );

	m_ptr->light.Direction.x=0.0f;
	m_ptr->light.Direction.y=0.0f;
	m_ptr->light.Direction.z=1.0f;
	D3DXVec3TransformCoord ( ( D3DXVECTOR3* ) &m_ptr->light.Direction, ( D3DXVECTOR3* ) &m_ptr->light.Direction, &matRotate );

	// update light
	m_pD3D->SetLight ( iID, &m_ptr->light );
}

DARKSDK void GetRotationVector3 ( int iVector, int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	D3DXVECTOR3 vec = GetAngleFromDirectionVector( m_ptr->light.Direction );
	g_Types_SetVector ( iVector, vec.x, vec.y, vec.z );
}

//
// Command Expression Functions
//

DARKSDK int GetExistEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
		return 0;

	return 1;
}

DARKSDK int GetTypeEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return -1;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return -1;
	}
	return m_ptr->eType;
}

DARKSDK DWORD GetXPositionEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->light.Position.x;
}

DARKSDK DWORD GetYPositionEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->light.Position.y;
}

DARKSDK DWORD GetZPositionEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->light.Position.z;
}

DARKSDK DWORD GetXDirectionEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->light.Direction.x;
}

DARKSDK DWORD GetYDirectionEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->light.Direction.y;
}

DARKSDK DWORD GetZDirectionEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->light.Direction.z;
}

DARKSDK int GetVisibleEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return 0;
	}
	if(m_ptr->bEnable==true)
		return 1;
	else
		return 0;
}

DARKSDK DWORD GetRangeEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DLIGHTNOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->fRange;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorLight ( HINSTANCE hSetup )
{
	Constructor ( hSetup );
}

void DestructorLight  ( void )
{
	Destructor ( );
}

void SetErrorHandlerLight ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataLight( LPVOID pGlobPtr )
{
	PassCoreData( pGlobPtr );
}

void RefreshD3DLight ( int iMode )
{
	RefreshD3D ( iMode );
}

void dbMakeLight ( int iID )
{
	MakeEx ( iID );
}

void dbDeleteLight ( int iID )
{
	DeleteEx ( iID );
}

void dbSetPointLight ( int iID, float fX, float fY, float fZ )
{
	SetPoint ( iID, fX, fY, fZ );
}

void dbSetSpotLight ( int iID, float fInner, float fOuter )
{
	SetSpot ( iID, fInner, fOuter );
}

void dbSetDirectionalLight ( int iID, float fX, float fY, float fZ )
{
	SetDirectional ( iID, fX, fY, fZ );
}

void dbSetLightRange ( int iID, float fRange )
{
	SetRange ( iID, fRange );
}

void dbPositionLight ( int iID, float fX, float fY, float fZ )
{
	Position ( iID, fX, fY, fZ );
}

void dbRotateLight ( int iID, float fX, float fY, float fZ )
{
	Rotate ( iID, fX, fY, fZ );
}

void dbPointLight ( int iID, float fX, float fY, float fZ )
{
	Point ( iID, fX, fY, fZ );
}

void dbHideLight ( int iID )
{
	Hide ( iID );
}

void dbShowLight ( int iID )
{
	Show ( iID );
}

void dbSetLightToObject ( int iID, int iObjectID )
{
	SetToObject ( iID, iObjectID );
}

void dbSetLightToObjectOrientation ( int iID, int iObjectID )
{
	SetToObjectOrientation ( iID, iObjectID );
}

void dbColorLight ( int iID, DWORD dwColor )
{
	SetColor ( iID, dwColor );
}

void dbColorLight ( int iID, int iR, int iG, int iB )
{
	SetColorEx ( iID, iR, iG, iB );
}

void dbSetNormalizationOn ( void )
{
	SetNormalizationOn ( );
}

void dbSetNormalizationOff ( void )
{
	SetNormalizationOff ( );
}

void dbSetAmbientLight ( int iPercent )
{
	SetAmbient ( iPercent );
}

void dbColorAmbientLight ( DWORD dwColor )
{
	SetAmbientColor ( dwColor );
}

void dbFogOn ( void )
{
	FogOn ( );
}

void dbFogOff ( void )
{
	FogOff ( );
}

void dbFogColor ( DWORD dwColor )
{
	SetFogColor ( dwColor );
}

void dbFogColor ( int iR, int iG, int iB )
{
	SetFogColorEx ( iR, iG, iB );
}

void dbFogDistance ( int iDistance )
{
	SetFogDistance ( iDistance );
}

void dbFogDistance ( int iStartDistance, int iFinishDistance )
{
	SetFogDistance ( iStartDistance, iFinishDistance );
}

void dbPositionLight ( int iID, int iVector )
{
	SetPositionVector3 ( iID, iVector );
}

void dbSetVector3ToLightPosition ( int iVector, int iID )
{
	GetPositionVector3 ( iVector, iID );
}

void dbRotateLight ( int iID, int iVector )
{
	SetRotationVector3 ( iID, iVector );
}

void dbSetVector3ToLightRotation ( int iVector, int iID )
{
	GetRotationVector3 ( iVector, iID );
}

int dbLightExist ( int iID )
{
	return GetExistEx ( iID );
}

int dbLightType ( int iID )
{
	return GetTypeEx ( iID );
}

float dbLightPositionX ( int iID )
{
	DWORD dwReturn = GetXPositionEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbLightPositionY ( int iID )
{
	DWORD dwReturn = GetYPositionEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbLightPositionZ ( int iID )
{
	DWORD dwReturn = GetZPositionEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbLightDirectionX ( int iID )
{
	DWORD dwReturn = GetXDirectionEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbLightDirectionY ( int iID )
{
	DWORD dwReturn = GetYDirectionEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbLightDirectionZ ( int iID )
{
	DWORD dwReturn = GetZDirectionEx ( iID );
	
	return *( float* ) &dwReturn;
}

int dbLightVisible ( int iID )
{
	return GetVisibleEx ( iID );
}

float dbLightRange ( int iID )
{
	DWORD dwReturn = GetRangeEx ( iID );
	
	return *( float* ) &dwReturn;
}

void dbSetLightSpecular ( int iID, float fA, float fR, float fG, float fB )
{
	SetSpecular ( iID, fA, fR, fG, fB );
}

void dbSetLightAmbient ( int iID, float fA, float fR, float fG, float fB )
{
	SetAmbient ( iID, fA, fR, fG, fB );
}

void dbSetLightFalloff ( int iID, float fValue )
{
	SetFalloff ( iID, fValue );
}

void dbSetLightAttenuation0 ( int iID, float fValue )
{
	SetAttenuation0 ( iID, fValue );
}

void dbSetLightAttenuation1 ( int iID, float fValue )
{
	SetAttenuation1 ( iID, fValue );
}

void dbSetLightAttenuation2 ( int iID, float fValue )
{
	SetAttenuation2 ( iID, fValue );
}

void dbSetLightSpecularOn ( void )
{
	SetSpecularOn ( );
}

void dbSetLightSpecularOff ( void )
{
	SetSpecularOff ( );
}

// lee - 300706 - GDK fixes
void dbSetLightToObjectPosition ( int iID, int iObjectID ) { dbSetLightToObject ( iID, iObjectID ); }

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////