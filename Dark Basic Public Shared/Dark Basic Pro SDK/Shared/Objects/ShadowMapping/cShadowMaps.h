#ifndef _SHADOWMAPS_H_
#define _SHADOWMAPS_H_

#include "..\CommonC.h"
#include "DepthTexture.h"

#define MAX_CASCADES 8
// Dave - added a define so we can check against the max size in object manager
#define EFFECT_INDEX_SIZE 1300

// Used to do selection of the shadow buffer format.
enum SHADOW_TEXTURE_FORMAT 
{
    CASCADE_DXGI_FORMAT_R32_TYPELESS,
    CASCADE_DXGI_FORMAT_R24G8_TYPELESS,
    CASCADE_DXGI_FORMAT_R16_TYPELESS,
    CASCADE_DXGI_FORMAT_R8_TYPELESS
};

enum SCENE_SELECTION 
{
    POWER_PLANT_SCENE,
    TEST_SCENE
};

enum FIT_PROJECTION_TO_CASCADES 
{
    FIT_TO_CASCADES,
    FIT_TO_SCENE
};

enum FIT_TO_NEAR_FAR 
{
    FIT_NEARFAR_PANCAKING,
    FIT_NEARFAR_ZERO_ONE,
    FIT_NEARFAR_AABB,
    FIT_NEARFAR_SCENE_AABB
};

enum CASCADE_SELECTION 
{
    CASCADE_SELECTION_MAP,
    CASCADE_SELECTION_INTERVAL
};

enum CAMERA_SELECTION 
{
    EYE_CAMERA,
    LIGHT_CAMERA,
    ORTHO_CAMERA1,
    ORTHO_CAMERA2,
    ORTHO_CAMERA3,
    ORTHO_CAMERA4,
    ORTHO_CAMERA5,
    ORTHO_CAMERA6,
    ORTHO_CAMERA7,
    ORTHO_CAMERA8
};

struct CascadeConfig 
{
    INT m_nCascadeLevels;
    SHADOW_TEXTURE_FORMAT m_ShadowBufferFormat;
    INT m_iBufferSize;
};

struct CB_ALL_SHADOW_DATA
{
    D3DXMATRIX  m_WorldViewProj;
    D3DXMATRIX  m_World;
    D3DXMATRIX  m_WorldView;
    D3DXMATRIX  m_Shadow;
    D3DXVECTOR4 m_vCascadeOffset[8];
    D3DXVECTOR4 m_vCascadeScale[8];

    INT         m_nCascadeLevels; // Number of Cascades
    INT         m_iVisualizeCascades; // 1 is to visualize the cascades in different colors. 0 is to just draw the scene.
    INT         m_iPCFBlurForLoopStart; // For loop begin value. For a 5x5 kernal this would be -2.
    INT         m_iPCFBlurForLoopEnd; // For loop end value. For a 5x5 kernel this would be 3.

    // For Map based selection scheme, this keeps the pixels inside of the the valid range.
    // When there is no boarder, these values are 0 and 1 respectivley.
    FLOAT       m_fMinBorderPadding;     
    FLOAT       m_fMaxBorderPadding;
    FLOAT       m_fShadowBiasFromGUI;  // A shadow map offset to deal with self shadow artifacts.  
                                        //These artifacts are aggravated by PCF.
    FLOAT       m_fCascadeBlendArea; // Amount to overlap when blending between cascades.
    FLOAT       m_fTexelSize; // Shadow map texel size.
    FLOAT       m_fCascadeFrustumsEyeSpaceDepths[8]; // The values along Z that seperate the cascades.
    D3DXVECTOR4 m_fCascadeFrustumsEyeSpaceDepthsFloat4[8];// the values along Z that separte the cascades.  
                                                          // Wastefully stored in float4 so they are array indexable :(
    D3DXVECTOR4 m_vLightDir;
};

class CFirstPersonCamera
{
	public:
    CFirstPersonCamera();

    // Call these from client and use Get*Matrix() to read new matrices
    //virtual void    FrameMove( FLOAT fElapsedTime );

    // Functions to change behavior
    //void            SetRotateButtons( bool bLeft, bool bMiddle, bool bRight, bool bRotateWithoutButtonDown = false );

    // Functions to get state
    D3DXMATRIX* GetWorldMatrix()
    {
        return &m_mCameraWorld;
    }
    D3DXMATRIX* GeViewMatrix()
    {
        return &m_mCameraView;
    }

    const D3DXVECTOR3* GetWorldRight() const
    {
        return ( D3DXVECTOR3* )&m_mCameraWorld._11;
    }
    const D3DXVECTOR3* GetWorldUp() const
    {
        return ( D3DXVECTOR3* )&m_mCameraWorld._21;
    }
    const D3DXVECTOR3* GetWorldAhead() const
    {
        return ( D3DXVECTOR3* )&m_mCameraWorld._31;
    }
    const D3DXVECTOR3* GetEyePt() const
    {
        return ( D3DXVECTOR3* )&m_mCameraWorld._41;
    }

	const float GetNearClip(void) { return m_fNear; }
	const float GetFarClip(void) { return m_fFar; }

	public:
	float m_fNear;
	float m_fFar;
    D3DXMATRIX m_mCameraWorld;       // World matrix of the camera (inverse of the view matrix)
    D3DXMATRIX m_mCameraView;
};

//
// Shadow Mapping Object Class
//

class CascadedShadowsManager 
{
public:
    CascadedShadowsManager();
    ~CascadedShadowsManager();
    
    // This runs when the application is initialized.
    HRESULT Init( CFirstPersonCamera* pViewerCamera,
                  CFirstPersonCamera* pLightCamera,
                  CascadeConfig* pCascadeConfig );
    
    HRESULT DestroyAndDeallocateShadowResources();

    // This runs per frame.  This data could be cached when the cameras do not move.
    HRESULT InitFrame( LPD3DXEFFECT pEffectPtr ) ;

    HRESULT RenderShadowsForAllCascades( LPD3DXEFFECT pEffectPtr );

    HRESULT RenderScene ( int iEffectID, LPD3DXEFFECT pEffectPtr,
					      LPVOID prtvBackBuffer,//ID3D9RenderTargetView* prtvBackBuffer, 
                          LPVOID pdsvBackBuffer,//ID3D9DepthStencilView* pdsvBackBuffer, 
                          CFirstPersonCamera* pActiveCamera,
						  BOOL bVisualize
                        );

    D3DXVECTOR3 GetSceneAABBMin()		{ return D3DXVECTOR3(m_vSceneAABBMin.x,m_vSceneAABBMin.y,m_vSceneAABBMin.z); };
    D3DXVECTOR3 GetSceneAABBMax()		{ return D3DXVECTOR3(m_vSceneAABBMax.x,m_vSceneAABBMax.y,m_vSceneAABBMax.z); };
    
    INT                                 m_iCascadePartitionsMax;
    FLOAT                               m_fCascadePartitionsFrustum[MAX_CASCADES]; // Values are  between near and far
    INT                                 m_iCascadePartitionsZeroToOne[MAX_CASCADES]; // Values are 0 to 100 and represent a percent of the frstum
    INT                                 m_iPCFBlurSize;
    FLOAT                               m_fPCFOffset;
    INT                                 m_iDerivativeBasedOffset;
    INT                                 m_iBlurBetweenCascades;
    FLOAT                               m_fBlurBetweenCascadesAmount;

    BOOL                                m_bMoveLightTexelSize;
    CAMERA_SELECTION                    m_eSelectedCamera;
    FIT_PROJECTION_TO_CASCADES          m_eSelectedCascadesFit;
    FIT_TO_NEAR_FAR                     m_eSelectedNearFarFit;

	DWORD								m_dwMask;

	struct sEffectParam
	{
		D3DXHANDLE m_iPCFBlurForLoopStart;
		D3DXHANDLE m_iPCFBlurForLoopEnd;
		D3DXHANDLE m_fCascadeBlendArea;
		D3DXHANDLE m_fTexelSize;
		D3DXHANDLE m_fShadowBiasFromGUI;
		D3DXHANDLE m_mShadow;
		D3DXHANDLE m_vCascadeScale;
		D3DXHANDLE m_vCascadeOffset;
		D3DXHANDLE m_fCascadeFrustumsEyeSpaceDepths;
		D3DXHANDLE m_fMaxBorderPadding;
		D3DXHANDLE m_fMinBorderPadding;
		D3DXHANDLE m_nCascadeLevels;
		D3DXHANDLE DepthMapTX1;
		D3DXHANDLE DepthMapTX2;
		D3DXHANDLE DepthMapTX3;
		D3DXHANDLE DepthMapTX4;
	};
	sEffectParam*						m_pEffectParam[EFFECT_INDEX_SIZE]; //Dave added define EFFECT_INDEX_SIZE

public:

    // Compute the near and far plane by intersecting an Ortho Projection with the Scenes AABB.
    void ComputeNearAndFar( FLOAT& fNearPlane, 
                            FLOAT& fFarPlane, 
                            D3DXVECTOR4 vLightCameraOrthographicMin, 
                            D3DXVECTOR4 vLightCameraOrthographicMax, 
                            D3DXVECTOR4* pvPointsInCameraView 
                          );
    

    void CreateFrustumPointsFromCascadeInterval ( FLOAT fCascadeIntervalBegin, 
                                                  FLOAT fCascadeIntervalEnd, 
                                                  D3DXMATRIX& vProjection,
                                                  D3DXVECTOR4* pvCornerPointsWorld
                                                );


    void CreateAABBPoints( D3DXVECTOR4* vAABBPoints, D3DXVECTOR4 vCenter, D3DXVECTOR4 vExtents );

    HRESULT ReleaseAndAllocateNewShadowResources( LPD3DXEFFECT pEffectPtr );

    D3DXVECTOR4                         m_vSceneAABBMin;
    D3DXVECTOR4                         m_vSceneAABBMax;
                                                                               // For example: when the shadow buffer size changes.
    char                                m_cvsModel[31];
    char                                m_cpsModel[31];
    char                                m_cgsModel[31];
    D3DXMATRIX                          m_matShadowProj[MAX_CASCADES]; 
    D3DXMATRIX                          m_matShadowView;
    CascadeConfig                       m_CopyOfCascadeConfig;      // This copy is used to determine when settings change. 
                                                                    //Some of these settings require new buffer allocations.
    CascadeConfig*                      m_pCascadeConfig;           // Pointer to the most recent setting.

	/*
	// D3D11 variables
    ID3D11InputLayout*                  m_pVertexLayoutMesh;
    ID3D11VertexShader*                 m_pvsRenderOrthoShadow;
    ID3DBlob*                           m_pvsRenderOrthoShadowBlob;
    ID3D11VertexShader*                 m_pvsRenderScene[MAX_CASCADES];
    ID3DBlob*                           m_pvsRenderSceneBlob[MAX_CASCADES];
    ID3D11PixelShader*                  m_ppsRenderSceneAllShaders[MAX_CASCADES][2][2][2];
    ID3DBlob*                           m_ppsRenderSceneAllShadersBlob[MAX_CASCADES][2][2][2];
	*/

	// color and depth surfaces for cascade render
	LPDIRECT3DTEXTURE9					m_pCascadedShadowMap;
	LPDIRECT3DSURFACE9					m_pCascadedShadowMapTexture;
	LPDIRECT3DSURFACE9					m_pCascadedShadowMapDSV;
	bool								m_bDepthStencilTexturesSupportedSM;
	bool								m_bNULLRenderTargetSupported;

	DepthTexture*						m_depthTexture[4];
	D3DXHANDLE							m_depthHandle[4];

	/*
    ID3D11Buffer*                       m_pcbGlobalConstantBuffer; // All VS and PS constants are in the same buffer.  
                                                          // An actual title would break this up into multiple 
                                                          // buffers updated based on frequency of variable changes
    ID3D11RasterizerState*              m_prsScene;
    ID3D11RasterizerState*              m_prsShadow;
    ID3D11RasterizerState*              m_prsShadowPancake;
    D3D11_VIEWPORT                      m_RenderVP[MAX_CASCADES];
    D3D11_VIEWPORT                      m_RenderOneTileVP;
    ID3D11SamplerState*                 m_pSamLinear;
    ID3D11SamplerState*                 m_pSamShadowPCF;
    ID3D11SamplerState*                 m_pSamShadowPoint;
	*/
    CFirstPersonCamera*                 m_pViewerCamera;         
    CFirstPersonCamera*                 m_pLightCamera;         
};

struct Frustum
{
    D3DXVECTOR3 Origin;            // Origin of the frustum (and projection).
    D3DXVECTOR4 Orientation;       // Unit quaternion representing rotation.

    FLOAT RightSlope;           // Positive X slope (X/Z).
    FLOAT LeftSlope;            // Negative X slope.
    FLOAT TopSlope;             // Positive Y slope (Y/Z).
    FLOAT BottomSlope;          // Negative Y slope.
    FLOAT Near, Far;            // Z of the near plane and far plane.
};

VOID ComputeFrustumFromProjection( Frustum* pOut, D3DXMATRIX* pProjection );

//
// XNA Derived 3D Math Help Functions
//

// structures
struct XMVECTORU32
{
	DWORD x;
	DWORD y;
	DWORD z;
	DWORD w;
};

// inline functions
inline D3DXVECTOR4 XMVectorReciprocal ( D3DXVECTOR4 V )
{
	D3DXVECTOR4 vResult;
	vResult.x = 1 / V.x;
	vResult.y = 1 / V.y;
	vResult.z = 1 / V.z;
	vResult.w = 1 / V.w;
	return vResult;
}
inline D3DXVECTOR4 XMVectorSplatX ( D3DXVECTOR4 V )
{
    D3DXVECTOR4 vResult;
	vResult.x = vResult.y = vResult.z = vResult.y = V.x;
    return vResult;
}
inline D3DXVECTOR4 XMVectorSplatY ( D3DXVECTOR4 V )
{
    D3DXVECTOR4 vResult;
	vResult.x = vResult.y = vResult.z = vResult.y = V.y;
    return vResult;
}
inline D3DXVECTOR4 XMVectorSplatZ ( D3DXVECTOR4 V )
{
    D3DXVECTOR4 vResult;
	vResult.x = vResult.y = vResult.z = vResult.y = V.z;
    return vResult;
}
inline D3DXVECTOR4 XMVectorSplatW ( D3DXVECTOR4 V )
{
    D3DXVECTOR4 vResult;
	vResult.x = vResult.y = vResult.z = vResult.y = V.w;
    return vResult;
}
inline D3DXVECTOR4 XMVectorMultiply ( D3DXVECTOR4 V, D3DXVECTOR4 A )
{
    D3DXVECTOR4 vResult;
	vResult.x = V.x * A.x;
	vResult.y = V.y * A.y;
	vResult.z = V.z * A.z;
	vResult.w = V.w * A.w;
    return vResult;
}
inline D3DXVECTOR4 XMVectorMultiplyAdd
(
    D3DXVECTOR4 V1, 
    D3DXVECTOR4 V2, 
    D3DXVECTOR4 V3
)
{
    D3DXVECTOR4 vResult;
	vResult.x = (V1.x * V2.x) + V3.x;
	vResult.y = (V1.y * V2.y) + V3.y;
	vResult.z = (V1.z * V2.z) + V3.z;
	vResult.w = (V1.w * V2.w) + V3.w;
    return vResult;
}
inline D3DXVECTOR4 XMVectorDivide ( D3DXVECTOR4 V, D3DXVECTOR4 A )
{
    D3DXVECTOR4 vResult;
	vResult.x = V.x / A.x;
	vResult.y = V.y / A.y;
	vResult.z = V.z / A.z;
	vResult.w = V.w / A.w;
    return vResult;
}

inline float XMVectorGetX ( D3DXVECTOR4 V )	{ return V.x; }
inline float XMVectorGetY ( D3DXVECTOR4 V )	{ return V.y; }
inline float XMVectorGetZ ( D3DXVECTOR4 V )	{ return V.z; }
inline float XMVectorGetW ( D3DXVECTOR4 V )	{ return V.w; }

inline D3DXVECTOR4 XMVectorSelect
(
    D3DXVECTOR4 V1, 
    D3DXVECTOR4 V2, 
    XMVECTORU32 Control
)
{
    XMVECTORU32 VV1 = *(XMVECTORU32*)&V1;
    XMVECTORU32 VV2 = *(XMVECTORU32*)&V2;

    D3DXVECTOR4 Result;
	Result.x = (VV1.x & ~Control.x) | (VV2.x & Control.x);
    Result.y = (VV1.y & ~Control.y) | (VV2.y & Control.y);
    Result.z = (VV1.z & ~Control.z) | (VV2.z & Control.z);
    Result.w = (VV1.w & ~Control.w) | (VV2.w & Control.w);
    return Result;
}

// My version without all that messing about with unsigned balh blah
inline D3DXVECTOR4 XMVectorSelectLee
(
    D3DXVECTOR4 V1, 
    D3DXVECTOR4 V2, 
    int elementindex
)
{
    D3DXVECTOR4 Result = V1;
	if ( elementindex==0 ) Result.x = V2.x;
	if ( elementindex==1 ) Result.y = V2.y;
	if ( elementindex==2 ) Result.z = V2.z;
	if ( elementindex==3 ) Result.w = V2.w;
    return Result;
}

// Return a floating point value via an index. This is not a recommended
// function to use due to performance loss.
inline FLOAT XMVectorGetByIndex(D3DXVECTOR4 V, UINT i)
{
    assert( i <= 3 );
    return V[i];//.vector4_f32[i];
}

// Rounding and clamping operations
inline D3DXVECTOR4 XMVectorMin
(
    D3DXVECTOR4 V1, 
    D3DXVECTOR4 V2
)
{
    D3DXVECTOR4 Result;
    Result.x = (V1.x < V2.x) ? V1.x : V2.x;
    Result.y = (V1.y < V2.y) ? V1.y : V2.y;
    Result.z = (V1.z < V2.z) ? V1.z : V2.z;
    Result.w = (V1.w < V2.w) ? V1.w : V2.w;
    return Result;
}

inline D3DXVECTOR4 XMVectorMax
(
    D3DXVECTOR4 V1, 
    D3DXVECTOR4 V2
)
{
    D3DXVECTOR4 Result;
    Result.x = (V1.x > V2.x) ? V1.x : V2.x;
    Result.y = (V1.y > V2.y) ? V1.y : V2.y;
    Result.z = (V1.z > V2.z) ? V1.z : V2.z;
    Result.w = (V1.w > V2.w) ? V1.w : V2.w;
    return Result;
}

inline float XMVector3LengthSq
(
    D3DXVECTOR3 V
)
{
    return D3DXVec3Dot(&V, &V);
}

//------------------------------------------------------------------------------

inline float XMVector4LengthSq
(
    D3DXVECTOR4 V
)
{
    return D3DXVec4Dot(&V, &V);
}

inline D3DXVECTOR4 XMVectorZero()
{
    D3DXVECTOR4 vResult = D3DXVECTOR4(0.0f,0.0f,0.0f,0.0f);
    return vResult;
}

// Initialize a vector with four floating point values
inline D3DXVECTOR4 XMVectorSet
(
    float x, 
    float y, 
    float z, 
    float w
)
{
    D3DXVECTOR4 vResult = D3DXVECTOR4(x,y,z,w);
    return vResult;
}

inline D3DXVECTOR4 XMVectorFloor
(
    D3DXVECTOR4 V
)
{
    D3DXVECTOR4 vResult = D3DXVECTOR4(
        floorf(V.x),
        floorf(V.y),
        floorf(V.z),
        floorf(V.w)
    );
    return vResult;
}



#endif
