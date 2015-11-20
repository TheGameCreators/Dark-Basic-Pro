
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include "cVectorC.h"
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"
#include ".\..\camera\ccameradatac.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKCamera.h"
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

typedef tagCameraData*		( *CAMERA3D_GetInternalDataPFN )	( int );
typedef IDirect3DDevice9*	( *GFX_GetDirect3DDevicePFN )		( void );

DBPRO_GLOBAL GlobStruct*					g_pGlob						= NULL;

namespace
{
    bool							g_bRTE					= false;
    CAMERA3D_GetInternalDataPFN	g_Camera3D_GetInternalData	= NULL;
    cDataManager					m_DataManager;
    LPDIRECT3DDEVICE9				m_pD3D					= NULL;
    HINSTANCE						g_GFX;
    GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

namespace
{
    template <typename T>
    inline T* GetCorrectedPtr( int iID )
    {
        T* pVector = m_DataManager.GetData<T>( iID );

		// Confirm it exists and is of the correct type
		//if (!pVector)
		//{
			//silent error as 3DMATH has no way to determine if vector exists in code
			//RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
			//return 0;
		//}

        // Cast to the proper type and return the pointer
        return pVector;
    }

    inline Vector2* GetVector2Ptr( int iID )
    {
        return GetCorrectedPtr<Vector2>( iID );
    }

    inline Vector3* GetVector3Ptr( int iID )
    {
        return GetCorrectedPtr<Vector3>( iID );
    }

    inline Vector4* GetVector4Ptr( int iID )
    {
        return GetCorrectedPtr<Vector4>( iID );
    }

    inline Matrix* GetMatrixPtr( int iID )
    {
        return GetCorrectedPtr<Matrix>( iID );
    }
}


//////////////////////////////////////////////////////////////////////////////////
// FUNCTION LISTINGS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void Constructor ( HINSTANCE hSetup )
{
	#ifndef DARKSDK_COMPILE
	if ( !hSetup )
	{
		hSetup = LoadLibrary ( "DBProSetupDebug.dll" );		// load the setup library
	}
	#endif

	#ifndef DARKSDK_COMPILE
		g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	#else
		g_GFX_GetDirect3DDevice = dbGetDirect3DDevice;
	#endif
	
	m_pD3D = g_GFX_GetDirect3DDevice ( );
}

DARKSDK void Destructor ( void ) 
{
    m_DataManager.ClearAll();
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
	g_bRTE = true;

	// Get camera data function ptr (for view and projection matrix commands)
	#ifndef DARKSDK_COMPILE
		g_Camera3D_GetInternalData = ( CAMERA3D_GetInternalDataPFN )	GetProcAddress ( g_pGlob->g_Camera3D, "?GetInternalData@@YAPAXH@Z" );
	#else
		g_Camera3D_GetInternalData = ( CAMERA3D_GetInternalDataPFN ) dbGetCameraInternalData;
	#endif
}

DARKSDK void RefreshD3D ( int iMode )
{
	if(iMode==0)
	{
		// Remove all traces of old D3D usage
		Destructor();
	}
	if(iMode==1)
	{
		// Get new D3D and recreate everything D3D related
		Constructor ( g_pGlob->g_GFX );
		PassCoreData ( g_pGlob );
	}
}


DARKSDK D3DXVECTOR2 GetVector2 ( int iID )
{
    Vector2* pVector = GetVector2Ptr( iID );

    if (!pVector)
        return D3DXVECTOR2( 0.0f, 0.0f );

    return pVector->Get();
}

DARKSDK D3DXVECTOR3 GetVector3 ( int iID )
{
    Vector3* pVector = GetVector3Ptr( iID );

    if (!pVector)
        return D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

    return pVector->Get();
}

DARKSDK D3DXVECTOR4 GetVector4 ( int iID )
{
    Vector4* pVector = GetVector4Ptr( iID );

    if (!pVector)
        return D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f );

    return pVector->Get();
}

DARKSDK D3DXMATRIX GetMatrix ( int iID )
{
    Matrix* pVector = GetMatrixPtr( iID );

    if (!pVector)
        return D3DXMATRIX();

    return pVector->Get();
}

DARKSDK int GetExist ( int iID )
{
    return m_DataManager.Exist( iID ) ? 1 : 0;
}

DARKSDK bool CheckTypeIsValid ( int iID, int iType )
{
    BaseVector* pVectorBase = m_DataManager.GetData<BaseVector>( iID );

    // Confirm it exists and is of the correct type
    if (!pVectorBase || pVectorBase->GetType() != iType)
        return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// VECTOR2 HANDLING ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DARKSDK SDK_BOOL MakeVector2 ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return false;
    }

    m_DataManager.Add( new Vector2, iID );
    return true;
}

DARKSDK SDK_BOOL DeleteVector2 ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return false;
    }

    Vector2* pVector = GetVector2Ptr( iID );

    if (!pVector)
    {
		//silent fail - no way to detect if vector exists in DBP code!
        //RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return false;
	}

    m_DataManager.Delete ( iID );
	return true;
}

DARKSDK void SetVector2 ( int iID, float fX, float fY )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pVector = GetVector2Ptr( iID );

    if (!pVector)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pVector->Get() = D3DXVECTOR2( fX, fY );
}

DARKSDK void CopyVector2 ( int iDestination, int iSource  )
{
    if (iDestination < 1 || iDestination > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pSource = GetVector2Ptr( iSource );
    Vector2* pDest   = GetVector2Ptr( iDestination );

    if (!pSource || !pDest)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pDest->Get() = pSource->Get();
}

DARKSDK void AddVector2 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pResult = GetVector2Ptr( iResult );
    Vector2* pA      = GetVector2Ptr( iA );
    Vector2* pB      = GetVector2Ptr( iB );

    if (!pA || !pB || !pResult)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pResult->Get() = pA->Get() + pB->Get();
}

DARKSDK void SubtractVector2 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pResult = GetVector2Ptr( iResult );
    Vector2* pA      = GetVector2Ptr( iA );
    Vector2* pB      = GetVector2Ptr( iB );

    if (!pA || !pB || !pResult)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pResult->Get() = pA->Get() - pB->Get();
}

DARKSDK void MultiplyVector2 ( int iID, float fValue )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pDest = GetVector2Ptr( iID );

    if (!pDest)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pDest->Get() *= fValue;
}

DARKSDK void DivideVector2 ( int iID, float fValue )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pDest = GetVector2Ptr( iID );

    if (!pDest)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pDest->Get() /= fValue;
}

DARKSDK SDK_BOOL IsEqualVector2 ( int iA, int iB )
{
    Vector2* pA = GetVector2Ptr( iA );
    Vector2* pB = GetVector2Ptr( iB );

    if (!pA || !pB)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return false;
	}

    if (pA->Get() == pB->Get())
        return true;

    return false;
}

DARKSDK SDK_BOOL IsNotEqualVector2 ( int iA, int iB )
{
    return !IsEqualVector2(iA, iB);
}

DARKSDK SDK_FLOAT GetXVector2 ( int iID )
{
    Vector2* pID = GetVector2Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
        float f = 0.0f;
		return SDK_RETFLOAT( f );
	}

    return SDK_RETFLOAT( pID->Get().x );
}

DARKSDK SDK_FLOAT GetYVector2 ( int iID )
{
    Vector2* pID = GetVector2Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
        float f = 0.0f;
		return SDK_RETFLOAT( f );
	}

    return SDK_RETFLOAT( pID->Get().y );
}

DARKSDK void GetBaryCentricCoordinatesVector2 ( int iResult, int iA, int iB, int iC, float f, float g )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pResult = GetVector2Ptr( iResult );
    Vector2* pA      = GetVector2Ptr( iA );
    Vector2* pB      = GetVector2Ptr( iB );
    Vector2* pC      = GetVector2Ptr( iC );

    if (!pResult || !pA || !pB || !pC)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	// now perform the operation
	D3DXVec2BaryCentric ( &pResult->Get(), &pA->Get(), &pB->Get(), &pC->Get(), f, g ); 
}

DARKSDK void CatmullRomVector2 ( int iResult, int iA, int iB, int iC, int iD, float s )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pResult = GetVector2Ptr( iResult );
    Vector2* pA      = GetVector2Ptr( iA );
    Vector2* pB      = GetVector2Ptr( iB );
    Vector2* pC      = GetVector2Ptr( iC );
    Vector2* pD      = GetVector2Ptr( iD );

    if (!pResult || !pA || !pB || !pC || !pD)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    D3DXVec2CatmullRom ( &pResult->Get(), &pA->Get(), &pB->Get(), &pC->Get(), &pD->Get(), s ); 
}

DARKSDK SDK_FLOAT GetCCWVector2 ( int iA, int iB )
{
    Vector2* pA      = GetVector2Ptr( iA );
    Vector2* pB      = GetVector2Ptr( iB );

    if (!pA || !pB)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)(0.0f);
	}

	float result = D3DXVec2CCW ( &pA->Get(), &pB->Get() );
	return SDK_RETFLOAT(result);
}

DARKSDK SDK_FLOAT DotProductVector2 ( int iA, int iB )
{
    Vector2* pA      = GetVector2Ptr( iA );
    Vector2* pB      = GetVector2Ptr( iB );

    if (!pA || !pB)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)(0.0f);
	}

	float result = D3DXVec2Dot ( &pA->Get(), &pB->Get() );
	return SDK_RETFLOAT(result);
}

DARKSDK void HermiteVector2 ( int iResult, int iA, int iB, int iC, int iD, float s )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pResult = GetVector2Ptr( iResult );
    Vector2* pA      = GetVector2Ptr( iA );
    Vector2* pB      = GetVector2Ptr( iB );
    Vector2* pC      = GetVector2Ptr( iC );
    Vector2* pD      = GetVector2Ptr( iD );

    if (!pResult || !pA || !pB || !pC || !pD)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    D3DXVec2Hermite ( &pResult->Get(), &pA->Get(), &pB->Get(), &pC->Get(), &pD->Get(), s ); 
}

DARKSDK SDK_FLOAT GetLengthVector2 ( int iID )
{
    Vector2* pID = GetVector2Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)0.0f;
	}

    float result = D3DXVec2Length ( &pID->Get() );
	return SDK_RETFLOAT(result);
}

DARKSDK SDK_FLOAT GetLengthSquaredVector2 ( int iID )
{
    Vector2* pID = GetVector2Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)0.0f;
	}

    float result = D3DXVec2LengthSq ( &pID->Get() );
	return SDK_RETFLOAT(result);
}

DARKSDK void LinearInterpolationVector2 ( int iResult, int iA, int iB, float s )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pResult = GetVector2Ptr( iResult );
    Vector2* pA      = GetVector2Ptr( iA );
    Vector2* pB      = GetVector2Ptr( iB );

    if (!pResult || !pA || !pB)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    D3DXVec2Lerp ( &pResult->Get(), &pA->Get(), &pB->Get(), s ); 
}

DARKSDK void MaximizeVector2 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pResult = GetVector2Ptr( iResult );
    Vector2* pA      = GetVector2Ptr( iA );
    Vector2* pB      = GetVector2Ptr( iB );

    if (!pResult || !pA || !pB)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    D3DXVec2Maximize ( &pResult->Get(), &pA->Get(), &pB->Get() ); 
}

DARKSDK void MinimizeVector2 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pResult = GetVector2Ptr( iResult );
    Vector2* pA      = GetVector2Ptr( iA );
    Vector2* pB      = GetVector2Ptr( iB );

    if (!pResult || !pA || !pB)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    D3DXVec2Minimize ( &pResult->Get(), &pA->Get(), &pB->Get() ); 
}

DARKSDK void NormalizeVector2 ( int iResult, int iSource )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pResult = GetVector2Ptr( iResult );
    Vector2* pSource = GetVector2Ptr( iSource );

    if (!pResult || !pSource)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	
	D3DXVec2Normalize ( &pResult->Get(), &pSource->Get() ); 
}

DARKSDK void ScaleVector2 ( int iResult, int iSource, float s )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pResult = GetVector2Ptr( iResult );
    Vector2* pSource = GetVector2Ptr( iSource );

    if (!pResult || !pSource)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    D3DXVec2Scale ( &pResult->Get(), &pSource->Get(), s ); 
}

DARKSDK void TransformVectorCoordinates2 ( int iResult, int iSource, int iMatrix )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector2* pResult = GetVector2Ptr( iResult );
    Vector2* pSource = GetVector2Ptr( iSource );
    Matrix*  pMatrix = GetMatrixPtr ( iMatrix );

    if (!pResult || !pSource)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
    if (!pMatrix)
    {
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    D3DXVec2TransformCoord ( &pResult->Get(), &pSource->Get(), &pMatrix->Get() );
}

////////////////////////////////////////////////////////////////////////////////
// VECTOR3 HANDLING ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DARKSDK SDK_BOOL MakeVector3 ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return false;
    }

    m_DataManager.Add( new Vector3, iID );
    return true;
}

DARKSDK SDK_BOOL DeleteVector3 ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return false;
    }

    Vector3* pVector = GetVector3Ptr( iID );

    if (!pVector)
    {
		//silent fail - no way to detect if vector exists in DBP code!
        //RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return false;
	}

	m_DataManager.Delete ( iID );
	return true;
}

DARKSDK void SetVector3 ( int iID, float fX, float fY, float fZ )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pID = GetVector3Ptr( iID );

	if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	pID->Get() = D3DXVECTOR3 ( fX, fY, fZ );
}

DARKSDK SDK_FLOAT GetXVector3 ( int iID )
{
    Vector3* pID = GetVector3Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)(0.0f);
	}

    return SDK_RETFLOAT( pID->Get().x );
}

DARKSDK SDK_FLOAT GetYVector3 ( int iID )
{
    Vector3* pID = GetVector3Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)(0.0f);
	}

    return SDK_RETFLOAT( pID->Get().y );
}

DARKSDK SDK_FLOAT GetZVector3 ( int iID )
{
    Vector3* pID = GetVector3Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)(0.0f);
	}

    return SDK_RETFLOAT( pID->Get().z );
}

DARKSDK void CopyVector3 ( int iDestination, int iSource )
{
    if (iDestination < 1 || iDestination > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pSource = GetVector3Ptr( iSource );
    Vector3* pDest   = GetVector3Ptr( iDestination );

    if (!pSource || !pDest)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pDest->Get() = pSource->Get();
}

DARKSDK void AddVector3 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pA      = GetVector3Ptr( iA );
    Vector3* pB      = GetVector3Ptr( iB );

    if (!pA || !pB || !pResult)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pResult->Get() = pA->Get() + pB->Get();
}

DARKSDK void SubtractVector3 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pA      = GetVector3Ptr( iA );
    Vector3* pB      = GetVector3Ptr( iB );

    if (!pA || !pB || !pResult)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pResult->Get() = pA->Get() - pB->Get();
}

DARKSDK void MultiplyVector3 ( int iID, float fValue )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pDest = GetVector3Ptr( iID );

    if (!pDest)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pDest->Get() *= fValue;
}

DARKSDK void DivideVector3 ( int iID, float fValue )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pDest = GetVector3Ptr( iID );

    if (!pDest)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pDest->Get() /= fValue;
}

DARKSDK SDK_BOOL IsEqualVector3 ( int iA, int iB )
{
    Vector3* pA = GetVector3Ptr( iA );
    Vector3* pB = GetVector3Ptr( iB );

    if (!pA || !pB)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return false;
	}

    if (pA->Get() == pB->Get())
        return true;

    return false;
}

DARKSDK SDK_BOOL IsNotEqualVector3 ( int iA, int iB )
{
    return !IsEqualVector3( iA, iB );
}

DARKSDK void GetBaryCentricCoordinatesVector3 ( int iResult, int iA, int iB, int iC, float f, float g )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pA      = GetVector3Ptr( iA );
    Vector3* pB      = GetVector3Ptr( iB );
    Vector3* pC      = GetVector3Ptr( iC );

    if (!pResult || !pA || !pB || !pC)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	// now perform the operation
	D3DXVec3BaryCentric ( &pResult->Get(), &pA->Get(), &pB->Get(), &pC->Get(), f, g ); 
}

DARKSDK void ProjectVector3 ( int iResult, int iSource, int iProjectionMatrix, int iViewMatrix, int iWorldMatrix )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pSource = GetVector3Ptr( iSource );
    Matrix*  pProjM  = GetMatrixPtr ( iProjectionMatrix );
    Matrix*  pViewM  = GetMatrixPtr ( iViewMatrix );
    Matrix*  pWorldM = GetMatrixPtr ( iWorldMatrix );

    if (!pResult || !pSource)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
    if (!pProjM || !pViewM || !pWorldM)
    {
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DVIEWPORT9	viewport;
	m_pD3D->GetViewport ( &viewport );

    D3DXVec3Project ( &pResult->Get(), &pSource->Get(), &viewport,
                      &pProjM->Get(), &pViewM->Get(), &pWorldM->Get() ); 
}

DARKSDK void TransformVectorCoordinates3 ( int iResult, int iSource, int iMatrix )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pSource = GetVector3Ptr( iSource );
    Matrix*  pMatrix = GetMatrixPtr ( iMatrix );

    if (!pResult || !pSource)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
    if (!pMatrix)
    {
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}
	
	D3DXVec3TransformCoord ( &pResult->Get(), &pSource->Get(), &pMatrix->Get() );
}

DARKSDK void TransformVectorNormalCoordinates3 ( int iResult, int iSource, int iMatrix )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pSource = GetVector3Ptr( iSource );
    Matrix*  pMatrix = GetMatrixPtr ( iMatrix );

    if (!pResult || !pSource)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
    if (!pMatrix)
    {
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}
	
	D3DXVec3TransformNormal ( &pResult->Get(), &pSource->Get(), &pMatrix->Get() );
}

DARKSDK void CatmullRomVector3 ( int iResult, int iA, int iB, int iC, int iD, float s )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pA      = GetVector3Ptr( iA );
    Vector3* pB      = GetVector3Ptr( iB );
    Vector3* pC      = GetVector3Ptr( iC );
    Vector3* pD      = GetVector3Ptr( iD );

    if (!pResult || !pA || !pB || !pC || !pD)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	D3DXVec3CatmullRom ( &pResult->Get(), &pA->Get(), &pB->Get(), &pC->Get(), &pD->Get(), s ); 
}

DARKSDK void CrossProductVector3 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pA      = GetVector3Ptr( iA );
    Vector3* pB      = GetVector3Ptr( iB );

    if (!pResult || !pA || !pB)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	D3DXVec3Cross ( &pResult->Get(), &pA->Get(), &pB->Get() );
}

DARKSDK SDK_FLOAT DotProductVector3 ( int iA, int iB )
{
    Vector3* pA      = GetVector3Ptr( iA );
    Vector3* pB      = GetVector3Ptr( iB );

    if (!pA || !pB)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)0.0f;
	}

    float result = D3DXVec3Dot ( &pA->Get(), &pB->Get() );
	return SDK_RETFLOAT(result);
}

DARKSDK void HermiteVector3 ( int iResult, int iA, int iB, int iC, int iD, float s )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pA      = GetVector3Ptr( iA );
    Vector3* pB      = GetVector3Ptr( iB );
    Vector3* pC      = GetVector3Ptr( iC );
    Vector3* pD      = GetVector3Ptr( iD );

    if (!pResult || !pA || !pB || !pC || !pD)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	D3DXVec3Hermite ( &pResult->Get(), &pA->Get(), &pB->Get(), &pC->Get(), &pD->Get(), s ); 
}

DARKSDK SDK_FLOAT GetLengthVector3 ( int iID )
{
    Vector3* pID = GetVector3Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)0.0f;
	}

    float result = D3DXVec3Length ( &pID->Get() );
	return SDK_RETFLOAT(result);
}

DARKSDK SDK_FLOAT GetLengthSquaredVector3 ( int iID )
{
    Vector3* pID = GetVector3Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)0.0f;
	}

    float result = D3DXVec3LengthSq ( &pID->Get() );
	return SDK_RETFLOAT(result);
}

DARKSDK void LinearInterpolationVector3 ( int iResult, int iA, int iB, float s )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pA      = GetVector3Ptr( iA );
    Vector3* pB      = GetVector3Ptr( iB );

    if (!pResult || !pA || !pB)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	D3DXVec3Lerp ( &pResult->Get(), &pA->Get(), &pB->Get(), s ); 
}

DARKSDK void MaximizeVector3 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pA      = GetVector3Ptr( iA );
    Vector3* pB      = GetVector3Ptr( iB );

    if (!pResult || !pA || !pB)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    D3DXVec3Maximize ( &pResult->Get(), &pA->Get(), &pB->Get() ); 
}

DARKSDK void MinimizeVector3 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pA      = GetVector3Ptr( iA );
    Vector3* pB      = GetVector3Ptr( iB );

    if (!pResult || !pA || !pB)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    D3DXVec3Minimize ( &pResult->Get(), &pA->Get(), &pB->Get() ); 
}

DARKSDK void NormalizeVector3 ( int iResult, int iSource )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pSource = GetVector3Ptr( iSource );

    if (!pResult || !pSource)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	
	D3DXVec3Normalize ( &pResult->Get(), &pSource->Get() ); 
}

DARKSDK void ScaleVector3 ( int iResult, int iSource, float s )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector3* pResult = GetVector3Ptr( iResult );
    Vector3* pSource = GetVector3Ptr( iSource );

    if (!pResult || !pSource)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    D3DXVec3Scale ( &pResult->Get(), &pSource->Get(), s ); 
}

////////////////////////////////////////////////////////////////////////////////
// VECTOR4 HANDLING ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DARKSDK SDK_BOOL MakeVector4 ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return false;
    }

    m_DataManager.Add( new Vector4, iID );
    return true;
}

DARKSDK SDK_BOOL DeleteVector4 ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return false;
    }

    Vector4* pVector = GetVector4Ptr( iID );

    if (!pVector)
    {
		//silent fail - no way to detect if vector exists in DBP code!
        //RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return false;
	}

	m_DataManager.Delete ( iID );
	return true;
}

DARKSDK void SetVector4 ( int iID, float fX, float fY, float fZ, float fW )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pID = GetVector4Ptr( iID );

	if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	pID->Get() = D3DXVECTOR4 ( fX, fY, fZ, fW );
}

DARKSDK void CopyVector4 ( int iDestination, int iSource )
{
    if (iDestination < 1 || iDestination > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pSource = GetVector4Ptr( iSource );
    Vector4* pDest   = GetVector4Ptr( iDestination );

    if (!pSource || !pDest)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pDest->Get() = pSource->Get();
}

DARKSDK void AddVector4 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pResult = GetVector4Ptr( iResult );
    Vector4* pA      = GetVector4Ptr( iA );
    Vector4* pB      = GetVector4Ptr( iB );

    if (!pA || !pB || !pResult)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pResult->Get() = pA->Get() + pB->Get();
}

DARKSDK void SubtractVector4 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pResult = GetVector4Ptr( iResult );
    Vector4* pA      = GetVector4Ptr( iA );
    Vector4* pB      = GetVector4Ptr( iB );

    if (!pA || !pB || !pResult)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pResult->Get() = pA->Get() - pB->Get();
}

DARKSDK void MultiplyVector4 ( int iID, float fValue )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pDest = GetVector4Ptr( iID );

    if (!pDest)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pDest->Get() *= fValue;
}

DARKSDK void DivideVector4 ( int iID, float fValue )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pDest = GetVector4Ptr( iID );

    if (!pDest)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    pDest->Get() /= fValue;
}

DARKSDK SDK_BOOL IsEqualVector4 ( int iA, int iB )
{
    Vector4* pA = GetVector4Ptr( iA );
    Vector4* pB = GetVector4Ptr( iB );

    if (!pA || !pB)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return false;
	}

    if (pA->Get() == pB->Get())
        return true;

    return false;
}

DARKSDK SDK_BOOL IsNotEqualVector4 ( int iA, int iB )
{
    return !IsEqualVector4( iA, iB );
}

DARKSDK SDK_FLOAT GetXVector4 ( int iID )
{
    Vector4* pID = GetVector4Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)(0.0f);
	}

    return SDK_RETFLOAT( pID->Get().x );
}

DARKSDK SDK_FLOAT GetYVector4 ( int iID )
{
    Vector4* pID = GetVector4Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)(0.0f);
	}

    return SDK_RETFLOAT( pID->Get().y );
}

DARKSDK SDK_FLOAT GetZVector4 ( int iID )
{
    Vector4* pID = GetVector4Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)(0.0f);
	}

    return SDK_RETFLOAT( pID->Get().z );
}

DARKSDK SDK_FLOAT GetWVector4 ( int iID )
{
    Vector4* pID = GetVector4Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)(0.0f);
	}

    return SDK_RETFLOAT( pID->Get().w );
}

DARKSDK void GetBaryCentricCoordinatesVector4 ( int iResult, int iA, int iB, int iC, float f, float g )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pResult = GetVector4Ptr( iResult );
    Vector4* pA      = GetVector4Ptr( iA );
    Vector4* pB      = GetVector4Ptr( iB );
    Vector4* pC      = GetVector4Ptr( iC );

    if (!pResult || !pA || !pB || !pC)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	D3DXVec4BaryCentric ( &pResult->Get(), &pA->Get(), &pB->Get(), &pC->Get(), f, g ); 
}

DARKSDK void CatmullRomVector4 ( int iResult, int iA, int iB, int iC, int iD, float s )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pResult = GetVector4Ptr( iResult );
    Vector4* pA      = GetVector4Ptr( iA );
    Vector4* pB      = GetVector4Ptr( iB );
    Vector4* pC      = GetVector4Ptr( iC );
    Vector4* pD      = GetVector4Ptr( iD );

    if (!pResult || !pA || !pB || !pC || !pD)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	D3DXVec4CatmullRom ( &pResult->Get(), &pA->Get(), &pB->Get(), &pC->Get(), &pD->Get(), s ); 
}

DARKSDK void HermiteVector4 ( int iResult, int iA, int iB, int iC, int iD, float s )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pResult = GetVector4Ptr( iResult );
    Vector4* pA      = GetVector4Ptr( iA );
    Vector4* pB      = GetVector4Ptr( iB );
    Vector4* pC      = GetVector4Ptr( iC );
    Vector4* pD      = GetVector4Ptr( iD );

    if (!pResult || !pA || !pB || !pC || !pD)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	D3DXVec4Hermite ( &pResult->Get(), &pA->Get(), &pB->Get(), &pC->Get(), &pD->Get(), s ); 
}

DARKSDK SDK_FLOAT GetLengthVector4 ( int iID )
{
    Vector4* pID = GetVector4Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)0.0f;
	}

    float result = D3DXVec4Length ( &pID->Get() );
	return SDK_RETFLOAT(result);
}

DARKSDK SDK_FLOAT GetLengthSquaredVector4 ( int iID )
{
    Vector4* pID = GetVector4Ptr( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return (DWORD)0.0f;
	}

    float result = D3DXVec4LengthSq ( &pID->Get() );
	return SDK_RETFLOAT(result);
}

DARKSDK void LinearInterpolationVector4 ( int iResult, int iA, int iB, float s )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pResult = GetVector4Ptr( iResult );
    Vector4* pA      = GetVector4Ptr( iA );
    Vector4* pB      = GetVector4Ptr( iB );

    if (!pResult || !pA || !pB)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	D3DXVec4Lerp ( &pResult->Get(), &pA->Get(), &pB->Get(), s ); 
}

DARKSDK void MaximizeVector4 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pResult = GetVector4Ptr( iResult );
    Vector4* pA      = GetVector4Ptr( iA );
    Vector4* pB      = GetVector4Ptr( iB );

    if (!pResult || !pA || !pB)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    D3DXVec4Maximize ( &pResult->Get(), &pA->Get(), &pB->Get() ); 
}

DARKSDK void MinimizeVector4 ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pResult = GetVector4Ptr( iResult );
    Vector4* pA      = GetVector4Ptr( iA );
    Vector4* pB      = GetVector4Ptr( iB );

    if (!pResult || !pA || !pB)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    D3DXVec4Minimize ( &pResult->Get(), &pA->Get(), &pB->Get() ); 
}

DARKSDK void NormalizeVector4 ( int iResult, int iSource )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pResult = GetVector4Ptr( iResult );
    Vector4* pSource = GetVector4Ptr( iSource );

    if (!pResult || !pSource)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	
	D3DXVec4Normalize ( &pResult->Get(), &pSource->Get() ); 
}

DARKSDK void ScaleVector4 ( int iResult, int iSource, float s )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pResult = GetVector4Ptr( iResult );
    Vector4* pSource = GetVector4Ptr( iSource );

    if (!pResult || !pSource)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

    D3DXVec4Scale ( &pResult->Get(), &pSource->Get(), s ); 
}

DARKSDK void TransformVector4 ( int iResult, int iSource, int iMatrix )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_VECTORNUMBERILLEGAL );
        return;
    }

    Vector4* pResult = GetVector4Ptr( iResult );
    Vector4* pSource = GetVector4Ptr( iSource );
    Matrix*  pMatrix = GetMatrixPtr ( iMatrix );

    if (!pResult || !pSource)
    {
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
    if (!pMatrix)
    {
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
    }

    D3DXVec4Transform ( &pResult->Get(), &pSource->Get(), &pMatrix->Get() );
}

////////////////////////////////////////////////////////////////////////////////
// Matrix Handling /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DARKSDK SDK_BOOL MakeMatrix ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return false;
    }

    Matrix* pMatrix = new Matrix;
    m_DataManager.Add( pMatrix, iID );
	return true;
}


DARKSDK void GetWorldMatrix ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pMatrix = GetMatrixPtr( iID );

    if (!pMatrix)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DXMatrixIdentity ( &pMatrix->Get() );
}

DARKSDK void GetViewMatrix ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pMatrix = GetMatrixPtr( iID );

    if (!pMatrix)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	if ( g_Camera3D_GetInternalData )
	{
		tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );
		pMatrix->Get() = m_Camera_Ptr->matView;
	}
}

DARKSDK void GetProjectionMatrix ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pMatrix = GetMatrixPtr( iID );

    if (!pMatrix)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    if ( g_Camera3D_GetInternalData )
	{
		tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );
		pMatrix->Get() = m_Camera_Ptr->matProjection;
		m_pD3D->SetTransform ( D3DTS_PROJECTION, &m_Camera_Ptr->matProjection );
	}
}

DARKSDK void SetProjectionMatrix ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pMatrix = GetMatrixPtr( iID );
    if (!pMatrix)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    if ( g_Camera3D_GetInternalData )
	{
		tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );
		m_Camera_Ptr->matProjection = pMatrix->Get();
	}
}

DARKSDK SDK_BOOL DeleteMatrix ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return false;
    }

    Matrix* pMatrix = GetMatrixPtr( iID );

    if (!pMatrix)
    {
		//silent fail - no way to detect if matrix exists in DBP code!
		//RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return false;
	}

	m_DataManager.Delete ( iID );
	return SDK_TRUE;
}

DARKSDK SDK_BOOL Matrix4Exist ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
        return SDK_FALSE;

    Matrix* pMatrix = GetMatrixPtr( iID );
    if (!pMatrix)
    {
		return SDK_FALSE;
	}
	else
	{
		return SDK_TRUE;
	}
}

DARKSDK void CopyMatrix ( int iDestination, int iSource )
{
    if (iDestination < 1 || iDestination > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pSource = GetMatrixPtr( iSource );
    Matrix* pDest   = GetMatrixPtr( iDestination );

    if (!pSource || !pDest)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    pDest->Get() = pSource->Get();
}

DARKSDK void AddMatrix ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pResult = GetMatrixPtr( iResult );
    Matrix* pA      = GetMatrixPtr( iA );
    Matrix* pB      = GetMatrixPtr( iB );

    if (!pA || !pB || !pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    pResult->Get() = pA->Get() + pB->Get();
}

DARKSDK void SubtractMatrix ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pResult = GetMatrixPtr( iResult );
    Matrix* pA      = GetMatrixPtr( iA );
    Matrix* pB      = GetMatrixPtr( iB );

    if (!pA || !pB || !pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    pResult->Get() = pA->Get() - pB->Get();
}

DARKSDK void MultiplyMatrix ( int iResult, int iA, int iB )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pResult = GetMatrixPtr( iResult );
    Matrix* pA      = GetMatrixPtr( iA );
    Matrix* pB      = GetMatrixPtr( iB );

    if (!pA || !pB || !pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    pResult->Get() = pA->Get() * pB->Get();
}

DARKSDK void MultiplyMatrix ( int iID, float fValue )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pDest = GetMatrixPtr( iID );

    if (!pDest)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    pDest->Get() *= fValue;
}

DARKSDK void DivideMatrix ( int iID, float fValue )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pDest = GetMatrixPtr( iID );

    if (!pDest)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    pDest->Get() /= fValue;
}

DARKSDK SDK_BOOL IsEqualMatrix ( int iA, int iB )
{
    Matrix* pA = GetMatrixPtr( iA );
    Matrix* pB = GetMatrixPtr( iB );

    if (!pA || !pB)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return false;
	}

    if (pA->Get() == pB->Get())
        return true;

    return false;
}

DARKSDK SDK_BOOL IsNotEqualMatrix ( int iA, int iB )
{
    return !IsEqualMatrix( iA, iB );
}

DARKSDK void SetIdentityMatrix ( int iID )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pID = GetMatrixPtr( iID );

    if (!pID)
    {
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}
	
	D3DXMatrixIdentity ( &pID->Get() );
}

DARKSDK SDK_FLOAT InverseMatrix ( int iResult, int iSource )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return 0;
    }

    Matrix* pResult = GetMatrixPtr( iResult );
    Matrix* pSource = GetMatrixPtr( iSource );

    if (!pResult || !pSource)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return false;
	}

    float fDeterminant = 0.0f;
	D3DXMatrixInverse ( &pResult->Get(), &fDeterminant, &pSource->Get() ); 

	// return the determinant
	return SDK_RETFLOAT(fDeterminant);
}

DARKSDK SDK_BOOL IsIdentityMatrix ( int iID )
{
    Matrix* pID = GetMatrixPtr( iID );

    if (!pID)
    {
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return false;
	}

	if ( D3DXMatrixIsIdentity ( &pID->Get() ) )
		return true;

	return false;
}

DARKSDK void BuildLookAtRHMatrix ( int iResult, int iVectorEye, int iVectorAt, int iVectorUp )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix*  pResult    = GetMatrixPtr ( iResult );
    Vector3* pVectorEye = GetVector3Ptr( iVectorEye );
    Vector3* pVectorAt  = GetVector3Ptr( iVectorAt );
    Vector3* pVectorUp  = GetVector3Ptr( iVectorUp );

    if (!pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    if (!pVectorEye || !pVectorAt || !pVectorUp)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	D3DXMatrixLookAtRH ( &pResult->Get(), &pVectorEye->Get(), &pVectorAt->Get(), &pVectorUp->Get() ); 
}

DARKSDK void BuildLookAtLHMatrix ( int iResult, int iVectorEye, int iVectorAt, int iVectorUp )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix*  pResult    = GetMatrixPtr ( iResult );
    Vector3* pVectorEye = GetVector3Ptr( iVectorEye );
    Vector3* pVectorAt  = GetVector3Ptr( iVectorAt );
    Vector3* pVectorUp  = GetVector3Ptr( iVectorUp );

    if (!pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    if (!pVectorEye || !pVectorAt || !pVectorUp)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	D3DXMatrixLookAtLH ( &pResult->Get(), &pVectorEye->Get(), &pVectorAt->Get(), &pVectorUp->Get() ); 
}

DARKSDK void BuildOrthoRHMatrix ( int iResult, float fWidth, float fHeight, float fZNear, float fZFar )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix*  pResult    = GetMatrixPtr ( iResult );

    if (!pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DXMatrixOrthoRH ( &pResult->Get(), fWidth, fHeight, fZNear, fZFar ); 
}

DARKSDK void BuildOrthoLHMatrix ( int iResult, float fWidth, float fHeight, float fZNear, float fZFar )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix*  pResult    = GetMatrixPtr ( iResult );

    if (!pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DXMatrixOrthoLH ( &pResult->Get(), fWidth, fHeight, fZNear, fZFar ); 
}

DARKSDK void BuildPerspectiveRHMatrix ( int iResult, float fWidth, float fHeight, float fZNear, float fZFar )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix*  pResult    = GetMatrixPtr ( iResult );

    if (!pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DXMatrixPerspectiveRH ( &pResult->Get(), fWidth, fHeight, fZNear, fZFar ); 
}

DARKSDK void BuildPerspectiveLHMatrix ( int iResult, float fWidth, float fHeight, float fZNear, float fZFar )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix*  pResult    = GetMatrixPtr ( iResult );

    if (!pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DXMatrixPerspectiveLH ( &pResult->Get(), fWidth, fHeight, fZNear, fZFar ); 
}

DARKSDK void BuildPerspectiveFovRHMatrix ( int iResult, float fFOV, float fAspect, float fZNear, float fZFar )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix*  pResult    = GetMatrixPtr ( iResult );

    if (!pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DXMatrixPerspectiveFovRH ( &pResult->Get(), fFOV, fAspect, fZNear, fZFar ); 
}

DARKSDK void BuildPerspectiveFovLHMatrix ( int iResult, float fFOV, float fAspect, float fZNear, float fZFar )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix*  pResult    = GetMatrixPtr ( iResult );

    if (!pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DXMatrixPerspectiveFovLH ( &pResult->Get(), fFOV, fAspect, fZNear, fZFar ); 
}

DARKSDK void BuildReflectionMatrix ( int iResult, float a, float b, float c, float d )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix*  pResult    = GetMatrixPtr ( iResult );

    if (!pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DXMatrixReflect ( &pResult->Get(), &D3DXPLANE ( a, b, c, d ) ); 
}

DARKSDK void BuildRotationAxisMatrix ( int iResult, int iVectorAxis, float fAngle )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix*  pResult     = GetMatrixPtr ( iResult );
    Vector3* pVectorAxis = GetVector3Ptr( iVectorAxis );

    if (!pResult)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}
    if (!pVectorAxis)
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}

	D3DXMatrixRotationAxis ( &pResult->Get(), &pVectorAxis->Get(), fAngle );
}

DARKSDK void RotateXMatrix ( int iID, float fAngle )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pID = GetMatrixPtr ( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    D3DXMatrixRotationX ( &pID->Get(), fAngle );
}

DARKSDK void RotateYMatrix ( int iID, float fAngle )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pID = GetMatrixPtr ( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    D3DXMatrixRotationY ( &pID->Get(), fAngle );
}

DARKSDK void RotateZMatrix ( int iID, float fAngle )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pID = GetMatrixPtr ( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

    D3DXMatrixRotationZ ( &pID->Get(), fAngle );
}

DARKSDK void RotateYawPitchRollMatrix ( int iID, float fYaw, float fPitch, float fRoll )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pID = GetMatrixPtr ( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DXMatrixRotationYawPitchRoll ( &pID->Get(), fYaw, fPitch, fRoll );
}

DARKSDK void ScaleMatrix ( int iID, float fX, float fY, float fZ )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pID = GetMatrixPtr ( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DXMatrixScaling ( &pID->Get(), fX, fY, fZ );
}

DARKSDK void TranslateMatrix ( int iID, float fX, float fY, float fZ )
{
    if (iID < 1 || iID > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pID = GetMatrixPtr ( iID );

    if (!pID)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DXMatrixTranslation ( &pID->Get(), fX, fY, fZ );
}

DARKSDK void TransposeMatrix ( int iResult, int iSource )
{
    if (iResult < 1 || iResult > MAXIMUMVALUE)
    {
        RunTimeError ( RUNTIMEERROR_MATRIX4NUMBERILLEGAL );
        return;
    }

    Matrix* pResult = GetMatrixPtr( iResult );
    Matrix* pSource = GetMatrixPtr( iSource );

	if (!pResult || !pSource)
	{
		RunTimeError ( RUNTIMEERROR_MATRIX4NOTEXIST );
		return;
	}

	D3DXMatrixTranspose ( &pResult->Get(), &pSource->Get() );
}

DARKSDK SDK_FLOAT GetMatrixElement ( int iMatrix, int iElement )
{
    Matrix*  pMatrix = GetMatrixPtr ( iMatrix );

    if (!pMatrix)
    {
		RunTimeError(RUNTIMEERROR_MATRIX4NOTEXIST);
		return 0;
	}

    D3DXMATRIX mat = pMatrix->Get();

    float res = 0.0;
	if (iElement>=0 && iElement<=15)
        res = mat[iElement];

	return SDK_RETFLOAT(res);
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void Constructor3DMaths ( HINSTANCE hSetup )
{
	Constructor ( hSetup );
}

void Destructor3DMaths  ( void )
{
	Destructor ( );
}

void SetErrorHandler3DMaths ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreData3DMaths ( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshD3D3DMaths ( int iMode )
{
	RefreshD3D ( iMode );
}

bool dbMakeVector2 ( int iID )
{
	return 0 != MakeVector2 ( iID );
}

bool dbDeleteVector2 ( int iID )
{
	return 0 != DeleteVector2 ( iID );
}

void dbSetVector2 ( int iID, float fX, float fY )
{
	SetVector2 ( iID, fX, fY );
}

void dbCopyVector2 ( int iSource, int iDestination )
{
	CopyVector2 ( iSource, iDestination );
}

void dbAddVector2 ( int iResult, int iA, int iB )
{
	AddVector2 ( iResult, iA, iB );
}

void dbSubtractVector2 ( int iResult, int iA, int iB )
{
	// mike - 190107 - made sure call is correct
	SubtractVector2 ( iResult, iA, iB );
}

void dbMultiplyVector2 ( int iID, float fValue )
{
	MultiplyVector2 ( iID, fValue );
}

void dbDivideVector2 ( int iID, float fValue )
{
	DivideVector2 ( iID, fValue );
}

bool dbIsEqualVector2 ( int iA, int iB )
{
	return 0 != IsEqualVector2 ( iA, iB );
}

bool dbIsNotEqualVector2 ( int iA, int iB )
{
	return 0 != IsNotEqualVector2 ( iA, iB );
}

float dbXVector2 ( int iID )
{
	DWORD dwReturn = GetXVector2 ( iID );
	
	return *( float* ) &dwReturn;
}

float dbYVector2 ( int iID )
{
	DWORD dwReturn = GetYVector2 ( iID );
	
	return *( float* ) &dwReturn;
}

void dbBCCVector2 ( int iResult, int iA, int iB, int iC, float f, float g )
{
	GetBaryCentricCoordinatesVector2 ( iResult, iA, iB, iC, f, g );
}

void dbCatmullRomVector2 ( int iResult, int iA, int iB, int iC, int iD, float s )
{
	CatmullRomVector2 ( iResult, iA, iB, iC, iD, s );
}

float dbCCWVector2 ( int iA, int iB )
{
	DWORD dwReturn = GetCCWVector2 ( iA, iB );
	
	return *( float* ) &dwReturn;
}

float dbDotProductVector2 ( int iA, int iB )
{
	DWORD dwReturn = DotProductVector2 ( iA, iB );
	
	return *( float* ) &dwReturn;
}

void dbHermiteVector2 ( int iResult, int iA, int iB, int iC, int iD, float s )
{
	HermiteVector2 ( iResult, iA, iB, iC, iD, s );
}

float dbLengthVector2 ( int iID )
{
	DWORD dwReturn = GetLengthVector2 ( iID );
	
	return *( float* ) &dwReturn;
}

float dbSquaredLengthVector2 ( int iID )
{
	DWORD dwReturn = GetLengthSquaredVector2 ( iID );
	
	return *( float* ) &dwReturn;
}

void dbLinearInterpolateVector2 ( int iResult, int iA, int iB, float s )
{
	LinearInterpolationVector2 ( iResult, iA, iB, s );
}

void dbMaximizeVector2 ( int iResult, int iA, int iB )
{
	MaximizeVector2 ( iResult, iA, iB );
}

void dbMinimizeVector2 ( int iResult, int iA, int iB )
{
	MinimizeVector2 ( iResult, iA, iB );
}

void dbNormalizeVector2 ( int iResult, int iSource )
{
	NormalizeVector2 ( iResult, iSource );
}

void dbScaleVector2 ( int iResult, int iSource, float s )
{
	ScaleVector2 ( iResult, iSource, s );
}

void dbTransformCoordsVector2 ( int iResult, int iSource, int iMatrix )
{
	TransformVectorCoordinates2 ( iResult, iSource, iMatrix );
}

bool dbMakeVector3 ( int iID )
{
	return 0 != MakeVector3 ( iID );
}

bool dbDeleteVector3 ( int iID )
{
	return 0 != DeleteVector3 ( iID );
}

void dbSetVector3 ( int iID, float fX, float fY, float fZ )
{
	SetVector3 ( iID, fX, fY, fZ );
}

void dbCopyVector3 ( int iSource, int iDestination )
{
	CopyVector3 ( iSource, iDestination );
}

void dbAddVector3 ( int iResult, int iA, int iB )
{
	AddVector3 ( iResult, iA, iB );
}

void dbSubtractVector3 ( int iResult, int iA, int iB )
{
	SubtractVector3 ( iResult, iA, iB );
}

void dbMultiplyVector3 ( int iID, float fValue )
{
	MultiplyVector3 ( iID, fValue );
}

void dbDivideVector3 ( int iID, float fValue )
{
	DivideVector3 ( iID, fValue );
}

bool dbIsEqualVector3 ( int iA, int iB )
{
	return 0 != IsEqualVector3 ( iA, iB );
}

bool dbIsNotEqualVector3 ( int iA, int iB )
{
	return 0 != IsNotEqualVector3 ( iA, iB );
}

void dbBCCVector3 ( int iResult, int iA, int iB, int iC, float f, float g )
{
	GetBaryCentricCoordinatesVector3 ( iResult, iA, iB, iC, f, g );
}

void dbCatmullRomVector3 ( int iResult, int iA, int iB, int iC, int iD, float s )
{
	CatmullRomVector3 ( iResult, iA, iB, iC, iD, s );
}

void dbCrossProductVector3 ( int iResult, int iA, int iB )
{
	CrossProductVector3 ( iResult, iA, iB );
}

void dbHermiteVector3 ( int iResult, int iA, int iB, int iC, int iD, float s )
{
	HermiteVector3 ( iResult, iA, iB, iC, iD, s );
}

void dbLinearInterpolateVector3 ( int iResult, int iA, int iB, float s )
{
	LinearInterpolationVector3 ( iResult, iA, iB, s );
}

void dbMaximizeVector3 ( int iResult, int iA, int iB )
{
	MaximizeVector3 ( iResult, iA, iB );
}

void dbMinimizeVector3 ( int iResult, int iA, int iB )
{
	MinimizeVector3 ( iResult, iA, iB );
}

void dbNormalizeVector3 ( int iResult, int iSource )
{
	NormalizeVector3 ( iResult, iSource );
}

void dbScaleVector3 ( int iResult, int iSource, float s )
{
	ScaleVector3 ( iResult, iSource, s );
}

float dbDotProductVector3 ( int iA, int iB )
{
	DWORD dwReturn = DotProductVector3 ( iA, iB );
	
	return *( float* ) &dwReturn;
}

float dbLengthVector3 ( int iID )
{
	DWORD dwReturn = GetLengthVector3 ( iID );
	
	return *( float* ) &dwReturn;
}

float dbSquaredLengthVector3 ( int iID )
{
	DWORD dwReturn = GetLengthSquaredVector3 ( iID );
	
	return *( float* ) &dwReturn;
}

void dbProjectVector3 ( int iResult, int iSource, int iProjectionMatrix, int iViewMatrix, int iWorldMatrix )
{
	ProjectVector3 ( iResult, iSource, iProjectionMatrix, iViewMatrix, iWorldMatrix );
}

void dbTransformCoordsVector3 ( int iResult, int iSource, int iMatrix )
{
	TransformVectorCoordinates3 ( iResult, iSource, iMatrix );
}

void dbTransformNormalsVector3 ( int iResult, int iSource, int iMatrix )
{
	TransformVectorNormalCoordinates3 ( iResult, iSource, iMatrix );
}

float dbXVector3 ( int iID )
{
	DWORD dwReturn = GetXVector3 ( iID );
	
	return *( float* ) &dwReturn;
}

float dbYVector3 ( int iID )
{
	DWORD dwReturn = GetYVector3 ( iID );
	
	return *( float* ) &dwReturn;
}

float dbZVector3 ( int iID )
{
	DWORD dwReturn = GetZVector3 ( iID );
	
	return *( float* ) &dwReturn;
}

bool dbMakeVector4 ( int iID )
{
	return 0 != MakeVector4 ( iID );
}

bool dbDeleteVector4 ( int iID )
{
	return 0 != DeleteVector4 ( iID );
}

void dbSetVector4 ( int iID, float fX, float fY, float fZ, float fW )
{
	SetVector4 ( iID, fX, fY, fZ, fW );
}

void dbCopyVector4 ( int iSource, int iDestination )
{
	CopyVector4 ( iSource, iDestination );
}

void dbAddVector4 ( int iResult, int iA, int iB )
{
	AddVector4 ( iResult, iA, iB );
}

void dbSubtractVector4 ( int iResult, int iA, int iB )
{
	SubtractVector4 ( iResult, iA, iB );
}

void dbMultiplyVector4 ( int iID, float fValue )
{
	MultiplyVector4 ( iID, fValue );
}

void dbDivideVector4 ( int iID, float fValue )
{
	DivideVector4 ( iID, fValue );
}

bool dbIsEqualVector4 ( int iA, int iB )
{
	return 0 != IsEqualVector4 ( iA, iB );
}

bool dbIsNotEqualVector4 ( int iA, int iB )
{
	return 0 != IsNotEqualVector4 ( iA, iB );
}

float dbXVector4 ( int iID )
{
	DWORD dwReturn = GetXVector4 ( iID );
	
	return *( float* ) &dwReturn;
}

float dbYVector4 ( int iID )
{
	DWORD dwReturn = GetYVector4 ( iID );
	
	return *( float* ) &dwReturn;
}

float dbZVector4 ( int iID )
{
	DWORD dwReturn = GetZVector4 ( iID );
	
	return *( float* ) &dwReturn;
}

float dbWVector4 ( int iID )
{
	DWORD dwReturn = GetWVector4 ( iID );
	
	return *( float* ) &dwReturn;
}

void dbBCCVector4 ( int iResult, int iA, int iB, int iC, float f, float g )
{
	GetBaryCentricCoordinatesVector4 ( iResult, iA, iB, iC, f, g );
}

void dbCatmullRomVector4 ( int iResult, int iA, int iB, int iC, int iD, float s )
{
	CatmullRomVector4 ( iResult, iA, iB, iC, iD, s );
}

void dbHermiteVector4 ( int iResult, int iA, int iB, int iC, int iD, float s )
{
	HermiteVector4 ( iResult, iA, iB, iC, iD, s );
}

float dbLengthVector4 ( int iID )
{
	DWORD dwReturn = GetLengthVector4 ( iID );
	
	return *( float* ) &dwReturn;
}

float dbSquaredLengthVector4 ( int iID )
{
	DWORD dwReturn = GetLengthSquaredVector4 ( iID );
	
	return *( float* ) &dwReturn;
}

void dbLinearInterpolateVector4 ( int iResult, int iA, int iB, float s )
{
	LinearInterpolationVector4 ( iResult, iA, iB, s );
}

void dbMaximizeVector4 ( int iResult, int iA, int iB )
{
	MaximizeVector4 ( iResult, iA, iB );
}

void dbMinimizeVector4 ( int iResult, int iA, int iB )
{
	MinimizeVector4 ( iResult, iA, iB );
}

void dbNormalizeVector4 ( int iResult, int iSource )
{
	NormalizeVector4 ( iResult, iSource );
}

void dbScaleVector4 ( int iResult, int iSource, float s )
{
	ScaleVector4 ( iResult, iSource, s );
}

void dbTransformVector4 ( int iResult, int iSource, int iMatrix )
{
	TransformVector4 ( iResult, iSource, iMatrix );
}

bool dbMakeMatrix4 ( int iID )
{
	return 0 != MakeMatrix ( iID );
}

bool dbDeleteMatrix4 ( int iID )
{
	return 0 != DeleteMatrix ( iID );
}

void dbCopyMatrix4 ( int iSource, int iDestination )
{
	CopyMatrix ( iSource, iDestination );
}

void dbAddMatrix4 ( int iResult, int iA, int iB )
{
	AddMatrix ( iResult, iA, iB );
}

void dbSubtractMatrix4 ( int iResult, int iA, int iB )
{
	SubtractMatrix ( iResult, iA, iB );
}

void dbMultiplyMatrix4 ( int iResult, int iA, int iB )
{
	MultiplyMatrix ( iResult, iA, iB );
}

void dbMultiplyMatrix4 ( int iID, float fValue )
{
	MultiplyMatrix ( iID, fValue );
}

void dbDivideMatrix4 ( int iID, float fValue )
{
	DivideMatrix ( iID, fValue );
}

bool dbIsEqualMatrix4 ( int iA, int iB )
{
	return 0 != IsEqualMatrix ( iA, iB );
}

bool dbIsNotEqualMatrix4 ( int iA, int iB )
{
	return 0 != IsNotEqualMatrix ( iA, iB );
}

void dbSetIdentityMatrix4 ( int iID )
{
	SetIdentityMatrix ( iID );
}

float dbInverseMatrix4 ( int iResult, int iSource )
{
	DWORD dwReturn = InverseMatrix ( iResult, iSource );
	
	return *( float* ) &dwReturn;
}

bool dbIsIdentityMatrix4 ( int iID )
{
	return 0 != IsIdentityMatrix ( iID );
}

void dbBuildLookAtRHMatrix4 ( int iResult, int iVectorEye, int iVectorAt, int iVectorUp )
{
	BuildLookAtRHMatrix ( iResult, iVectorEye, iVectorAt, iVectorUp );
}

void dbBuildLookAtLHMatrix4 ( int iResult, int iVectorEye, int iVectorAt, int iVectorUp )
{
	BuildLookAtLHMatrix ( iResult, iVectorEye, iVectorAt, iVectorUp );
}

void dbBuildOrthoRHMatrix4 ( int iResult, float fWidth, float fHeight, float fZNear, float fZFar )
{
	BuildOrthoRHMatrix ( iResult, fWidth, fHeight, fZNear, fZFar );
}

void dbBuildOrthoLHMatrix4 ( int iResult, float fWidth, float fHeight, float fZNear, float fZFar )
{
	BuildOrthoLHMatrix ( iResult, fWidth, fHeight, fZNear, fZFar );
}

void dbBuildPerspectiveRHMatrix4 ( int iResult, float fWidth, float fHeight, float fZNear, float fZFar )
{
	BuildPerspectiveRHMatrix ( iResult, fWidth, fHeight, fZNear, fZFar );
}

void dbBuildPerspectiveLHMatrix4 ( int iResult, float fWidth, float fHeight, float fZNear, float fZFar )
{
	BuildPerspectiveLHMatrix ( iResult, fWidth, fHeight, fZNear, fZFar );
}

void dbBuildFovRHMatrix4 ( int iResult, float fFOV, float fAspect, float fZNear, float fZFar )
{
	BuildPerspectiveFovRHMatrix ( iResult, fFOV, fAspect, fZNear, fZFar );
}

void dbBuildFovLHMatrix4 ( int iResult, float fFOV, float fAspect, float fZNear, float fZFar )
{
	BuildPerspectiveFovLHMatrix ( iResult, fFOV, fAspect, fZNear, fZFar );
}

void dbBuildReflectionMatrix4 ( int iResult, float a, float b, float c, float d )
{
	BuildReflectionMatrix ( iResult, a, b, c, d );
}

void dbBuildRotationAxisMatrix4 ( int iResult, int iVectorAxis, float fAngle )
{
	BuildRotationAxisMatrix ( iResult, iVectorAxis, fAngle );
}

void dbRotateXMatrix4 ( int iID, float fAngle )
{
	RotateXMatrix ( iID, fAngle );
}

void dbRotateYMatrix4 ( int iID, float fAngle )
{
	RotateYMatrix ( iID, fAngle );
}

void dbRotateZMatrix4 ( int iID, float fAngle )
{
	RotateZMatrix ( iID, fAngle );
}

void dbRotateYPRMatrix4 ( int iID, float fYaw, float fPitch, float fRoll )
{
	RotateYawPitchRollMatrix ( iID, fYaw, fPitch, fRoll );
}

void dbScaleMatrix4 ( int iID, float fX, float fY, float fZ )
{
	ScaleMatrix ( iID, fX, fY, fZ );
}

void dbTranslateMatrix4 ( int iID, float fX, float fY, float fZ )
{
	TranslateMatrix ( iID, fX, fY, fZ );
}

void dbTransposeMatrix4 ( int iResult, int iSource )
{
	TransposeMatrix ( iResult, iSource );
}

void dbWorldMatrix4 ( int iID )
{
	GetWorldMatrix ( iID );
}

void dbViewMatrix4 ( int iID )
{
	GetViewMatrix ( iID );
}

void dbProjectionMatrix4 ( int iID )
{
	GetProjectionMatrix ( iID );
}

D3DXVECTOR2	dbGetVector2 ( int iID )
{
	return GetVector2 ( iID );
}

D3DXVECTOR3	dbGetVector3 ( int iID )
{
	return GetVector3 ( iID );
}

D3DXVECTOR4	dbGetVector4 ( int iID )
{
	return GetVector4 ( iID );
}

D3DXMATRIX dbGetMatrix ( int iID )
{
	return GetMatrix ( iID );
}

int	dbGet3DMathsExist ( int iID )
{
	return GetExist ( iID );
}

float dbGetMatrixElement ( int iID, int iElementIndex )
{
	DWORD dwReturn = GetMatrixElement ( iID, iElementIndex );
	
	return *( float* ) &dwReturn;
}

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////