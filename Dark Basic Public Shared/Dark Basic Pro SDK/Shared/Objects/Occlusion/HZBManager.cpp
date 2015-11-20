// Main include
#include "HZBManager.h"

// Helper structures

struct VPos
{
	D3DXVECTOR4 Pos;  
};

struct Plane
{
    float a, b, c, d;
};

union Frustum
{
    struct
    {
        Plane Left;
        Plane Right;
        Plane Top;
        Plane Bottom;
        Plane Near;
        Plane Far;
    };
    Plane Planes[6];
};

struct VBoundPixel
{
	D3DXVECTOR4 Bound;
    D3DXVECTOR2 Pixel;
};

// Helper functions

void NormalizePlane(Plane& plane)
{
    float mag;
    mag = sqrt(plane.a * plane.a + plane.b * plane.b + plane.c * plane.c);

    plane.a = plane.a / mag;
    plane.b = plane.b / mag;
    plane.c = plane.c / mag;
    plane.d = plane.d / mag;
}

void ExtractFrustum(Frustum& frustum, const D3DXMATRIX& viewProjMatrix)
{
    frustum.Left.a = viewProjMatrix._14 + viewProjMatrix._11;
    frustum.Left.b = viewProjMatrix._24 + viewProjMatrix._21;
    frustum.Left.c = viewProjMatrix._34 + viewProjMatrix._31;
    frustum.Left.d = viewProjMatrix._44 + viewProjMatrix._41;

    frustum.Right.a = viewProjMatrix._14 - viewProjMatrix._11;
    frustum.Right.b = viewProjMatrix._24 - viewProjMatrix._21;
    frustum.Right.c = viewProjMatrix._34 - viewProjMatrix._31;
    frustum.Right.d = viewProjMatrix._44 - viewProjMatrix._41;

    frustum.Top.a = viewProjMatrix._14 - viewProjMatrix._12;
    frustum.Top.b = viewProjMatrix._24 - viewProjMatrix._22;
    frustum.Top.c = viewProjMatrix._34 - viewProjMatrix._32;
    frustum.Top.d = viewProjMatrix._44 - viewProjMatrix._42;

    frustum.Bottom.a = viewProjMatrix._14 + viewProjMatrix._12;
    frustum.Bottom.b = viewProjMatrix._24 + viewProjMatrix._22;
    frustum.Bottom.c = viewProjMatrix._34 + viewProjMatrix._32;
    frustum.Bottom.d = viewProjMatrix._44 + viewProjMatrix._42;

    frustum.Near.a = viewProjMatrix._13;
    frustum.Near.b = viewProjMatrix._23;
    frustum.Near.c = viewProjMatrix._33;
    frustum.Near.d = viewProjMatrix._43;

    frustum.Far.a = viewProjMatrix._14 - viewProjMatrix._13;
    frustum.Far.b = viewProjMatrix._24 - viewProjMatrix._23;
    frustum.Far.c = viewProjMatrix._34 - viewProjMatrix._33;
    frustum.Far.d = viewProjMatrix._44 - viewProjMatrix._43;

    NormalizePlane(frustum.Left);
    NormalizePlane(frustum.Right);
    NormalizePlane(frustum.Top);
    NormalizePlane(frustum.Bottom);
    NormalizePlane(frustum.Near);
    NormalizePlane(frustum.Far);
}

HRESULT LoadEffectFromFile(LPDIRECT3DDEVICE9 pDevice, LPCWSTR pSrcFile, LPD3DXEFFECT* ppEffect)
{
    DWORD dwShaderFlags = 0;
	//#if defined( DEBUG ) || defined( _DEBUG )
	//	dwShaderFlags |= D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
	//#endif

    LPD3DXBUFFER pCompilationErrors;
    HRESULT hr = D3DXCreateEffectFromFileExW(pDevice, pSrcFile, 
        NULL, NULL, NULL, dwShaderFlags, NULL, ppEffect, &pCompilationErrors);

    if (FAILED(hr))
	{
		if (pCompilationErrors)
		{
			LPVOID error = pCompilationErrors->GetBufferPointer();
			//HZBDebugPrintf("Failed to compile effect. %s\n", (char*)error);
		}
		else
		{
			//HZBDebugPrintf("Failed to compile effect. Code: %d\n", HRESULT_CODE(hr));
		}
        SAFE_RELEASE( pCompilationErrors );
		return E_FAIL;
	}
    return S_OK;
}

// Main Class Functions

HZBManager::HZBManager(IDirect3DDevice9* pDevice)
    : m_pDevice(pDevice)
    , m_pDepthEffect(NULL)
	, m_bTriggerDepthOccluderVertUpdate(false)
    , m_dwVertexToRenderCount(0)
	, m_dwMaxVertexNumberRequired(0)
    , m_pVerticesForNextOcclusionDepthDraw(NULL)
    , m_pDepthVertexDecl(NULL)
    , m_pOccluderVertexBuffer(NULL)
	, m_pOcclusionZBuffer(NULL)
    , m_pHizBufferEven(NULL)
    , m_pMipLevelEvenSurfaces(NULL)
    , m_pHizBufferOdd(NULL)
    , m_pMipLevelOddSurfaces(NULL)
    , m_pCullingBoundsBuffer(NULL)
    , m_pCullingResultsBuffer(NULL)
    , m_pCullingResultsSurface(NULL)
    , m_pCullingResultsBufferSys(NULL)
    , m_pCullingResultsSurfaceSys(NULL)
    , m_cullingResultsWidth(0)
    , m_cullingResultsHeight(0)
    , m_pHizMipmapEffect(NULL)
    , m_pHizMipmapVertexDecl(NULL)
    , m_pHizMipmapVertexBuffer(NULL)
    , m_pHizCullEffect(NULL)
    , m_pCullingBoundsVertexDecl(NULL)
    , m_boundsSize(0)
{
    LoadDepthEffect();
    LoadMipmapEffect();
    LoadCullEffect();
	m_vObjectOccluderList.clear();
	m_vObjectOccludeeList.clear();
}

HZBManager::~HZBManager(void)
{
    Reset();

    SAFE_RELEASE(m_pDepthEffect);
    SAFE_DELETE(m_pVerticesForNextOcclusionDepthDraw);
    SAFE_RELEASE(m_pDepthVertexDecl);
    SAFE_RELEASE(m_pOccluderVertexBuffer);
  
	SAFE_RELEASE(m_pOcclusionZBuffer);

    SAFE_RELEASE(m_pHizMipmapEffect);
    SAFE_RELEASE(m_pHizMipmapVertexDecl);
    SAFE_RELEASE(m_pHizMipmapVertexBuffer);

    SAFE_RELEASE(m_pHizCullEffect);
    SAFE_RELEASE(m_pCullingBoundsVertexDecl);

    SAFE_RELEASE(m_pCullingBoundsBuffer);
    SAFE_RELEASE(m_pCullingResultsSurface);
    SAFE_RELEASE(m_pCullingResultsBuffer);
    SAFE_RELEASE(m_pCullingResultsSurfaceSys);
    SAFE_RELEASE(m_pCullingResultsBufferSys);
}

void HZBManager::AddOccluder(sObject* pObject)
{
	sOccluderObject occobj;
	occobj.dwStartVertex = 0;
	occobj.dwVertexToRenderCount = 0;
	occobj.pObject = pObject;
	m_vObjectOccluderList.push_back(occobj);
}

void HZBManager::AddOccludee(sObject* pObject)
{
	m_vObjectOccludeeList.push_back(pObject);
}

void HZBManager::DeleteOccludite(sObject* pObject)
{
	// remove from occluder list (if any)
    for ( DWORD iObjOccluderList = 0; iObjOccluderList < m_vObjectOccluderList.size(); ++iObjOccluderList )
    {
        sObject* pTryObject = m_vObjectOccluderList [ iObjOccluderList ].pObject;
		if ( pTryObject==pObject )
		{
			m_vObjectOccluderList.erase(m_vObjectOccluderList.begin() + iObjOccluderList);
			break;
		}
	}

	// remove from occludee list (if any)
    for ( DWORD iObjOccludeeList = 0; iObjOccludeeList < m_vObjectOccludeeList.size(); ++iObjOccludeeList )
    {
        sObject* pTryObject = m_vObjectOccludeeList [ iObjOccludeeList ];
		if ( pTryObject==pObject )
		{
			m_vObjectOccludeeList.erase(m_vObjectOccludeeList.begin() + iObjOccludeeList);
			break;
		}
	}
}

void HZBManager::Render(const D3DXMATRIX& view, const D3DXMATRIX& projection)
{
    RenderOccluders(view, projection);
    RenderMipmap();
}

void HZBManager::RenderOccluders(const D3DXMATRIX& view, const D3DXMATRIX& projection)
{
	// camera zero to work out distance for occluder chooser
	tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( 0 );
	D3DXVECTOR3* pvecCamPos = &m_Camera_Ptr->vecPosition;

	// lock and fill dynamic VB part (this NEEDS to be fast as possible - perhaps some sort'n'keep order thingie)
    VPos* v = NULL;
    HRESULT hr = m_pOccluderVertexBuffer->Lock(0, 0, (void**)&v, D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE);
	float* pWritePtr = (float*)v;
	DWORD dwWriteVertCount = 0;
    assert(SUCCEEDED(hr));
    for ( DWORD iObjOccluderList = 0; iObjOccluderList < m_vObjectOccluderList.size(); ++iObjOccluderList )
    {
        sOccluderObject* pOccObject = &m_vObjectOccluderList [ iObjOccluderList ];
		if ( pOccObject )
		{
			// only use occluders which are near the main camera (mayb increase this when not filled up 60K polys worth)
			D3DXVECTOR3* pObjPos = &pOccObject->pObject->position.vecPosition;
			float fDX = pObjPos->x - pvecCamPos->x;
			float fDZ = pObjPos->z - pvecCamPos->z;
			float fThisDistance = sqrt(fabs(fDX*fDX)+fabs(fDZ*fDZ));
			if ( fThisDistance < 5000.0f )
			{
				DWORD dwVertexToRenderCount = pOccObject->dwVertexToRenderCount;
				if ( dwVertexToRenderCount>0 )
				{
					if ( dwWriteVertCount + dwVertexToRenderCount < m_dwOccluderVertexBufferMaxSize )
					{
						float* pFromHere = m_pVerticesForNextOcclusionDepthDraw + (pOccObject->dwStartVertex*4);
						DWORD dwSizeOfBlock = dwVertexToRenderCount*4;
						memcpy(pWritePtr, pFromHere, dwSizeOfBlock*sizeof(float));
						pWritePtr += dwSizeOfBlock;
						dwWriteVertCount += dwVertexToRenderCount;
					}
				}
			}
		}
	}
    hr = m_pOccluderVertexBuffer->Unlock();
    assert(SUCCEEDED(hr));

	// render part
    D3DVIEWPORT9 vp;
    vp.Width = m_width;
    vp.Height = m_height;
    vp.MinZ = 0;
    vp.MaxZ = 1;
    vp.X = 0;
    vp.Y = 0;
    hr = m_pDevice->SetViewport(&vp);
    assert(SUCCEEDED(hr));

    hr = m_pDevice->SetRenderTarget(0, m_pMipLevelEvenSurfaces[0]);
    assert(SUCCEEDED(hr));

    hr = m_pDevice->SetDepthStencilSurface(m_pOcclusionZBuffer);
    assert(SUCCEEDED(hr));

    // Clear the render target and the zbuffer 
    hr = m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 255, 255, 255, 255 ), 1.0f, 0 );
    assert(SUCCEEDED(hr));

	hr = m_pDevice->SetStreamSource(0, m_pOccluderVertexBuffer, 0, sizeof(VPos));
	assert(SUCCEEDED(hr));

	hr = m_pDevice->SetVertexDeclaration(m_pDepthVertexDecl);
	assert(SUCCEEDED(hr));

	hr = m_pDepthEffect->SetTechnique(m_hDepthEffectTech);
	assert(SUCCEEDED(hr));

	D3DXMATRIX occluderWorld;
	D3DXMatrixIdentity(&occluderWorld);
	D3DXMATRIX WVP = occluderWorld * view * projection;

	hr = m_pDepthEffect->SetMatrix(m_hDepthEffectParamWVP, &WVP);
	assert(SUCCEEDED(hr));

	D3DXMATRIX WV = occluderWorld * view;
	hr = m_pDepthEffect->SetMatrix(m_hDepthEffectParamWV, &WV);
	assert(SUCCEEDED(hr));

	UINT numPasses = 0;
	hr = m_pDepthEffect->Begin(&numPasses, 0);
	assert(SUCCEEDED(hr));

	for (UINT i = 0; i < numPasses; ++i)
	{
		hr = m_pDepthEffect->BeginPass(i);
		assert(SUCCEEDED(hr));

		hr = m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, dwWriteVertCount/3);
		assert(SUCCEEDED(hr));

		hr = m_pDepthEffect->EndPass();
		assert(SUCCEEDED(hr));
	}

	hr = m_pDepthEffect->End();
	assert(SUCCEEDED(hr));

	// Zbuffer not need for rest of process
    hr = m_pDevice->SetDepthStencilSurface(NULL);
    assert(SUCCEEDED(hr));

	// debug mode which detects if 'internalocclusiondebugimagestart' image exists and copies depth surface to it
	if ( 1 )
	{
		LPDIRECT3DTEXTURE9 pDebugSee = g_Image_GetPointer ( 59961 );
		if ( pDebugSee )
		{
			for ( int mips=0; mips<3; mips++ )
			{
				LPDIRECT3DSURFACE9 pOccluderDebugImage;
				pDebugSee->GetSurfaceLevel ( mips, &pOccluderDebugImage );
				if ( pOccluderDebugImage  )
				{
					HRESULT hRes = D3DXLoadSurfaceFromSurface ( pOccluderDebugImage, NULL, NULL, m_pMipLevelEvenSurfaces[0], NULL, NULL, D3DX_DEFAULT, 0 );
				}
			}
		}
	}
}

void HZBManager::RenderMipmap()
{
    D3DVIEWPORT9 vp;
    vp.Width = m_width;
    vp.Height = m_height;
    vp.MinZ = 0;
    vp.MaxZ = 1;
    vp.X = 0;
    vp.Y = 0;
    HRESULT hr = m_pDevice->SetViewport(&vp);
    assert(SUCCEEDED(hr));

    for (UINT miplevel = 1; miplevel < m_mipLevels; miplevel++)
    {
        IDirect3DSurface9* pTargetSurface = 
            ((miplevel % 2) == 0) ? m_pMipLevelEvenSurfaces[miplevel] : m_pMipLevelOddSurfaces[miplevel];

        hr = m_pDevice->SetRenderTarget(0, pTargetSurface);
        assert(SUCCEEDED(hr));

        UINT width = m_width >> miplevel;
        UINT height = m_height >> miplevel;
        if (width == 0) width = 1;
        if (height == 0) height = 1;

        D3DVIEWPORT9 vp;
        vp.Width = width;
        vp.Height = height;
        vp.MinZ = 0;
        vp.MaxZ = 1;
        vp.X = 0;
        vp.Y = 0;
        HRESULT hr = m_pDevice->SetViewport(&vp);
        assert(SUCCEEDED(hr));

        hr = m_pDevice->SetStreamSource(0, m_pHizMipmapVertexBuffer, 0, sizeof(VPos));
        assert(SUCCEEDED(hr));

        hr = m_pDevice->SetVertexDeclaration(m_pHizMipmapVertexDecl);
        assert(SUCCEEDED(hr));

        hr = m_pHizMipmapEffect->SetTechnique(m_hMipmapTech);
        assert(SUCCEEDED(hr));

        UINT lastWidth = m_width >> (miplevel - 1);
        UINT lastHeight = m_height >> (miplevel - 1);
        if (lastWidth == 0) lastWidth = 1;
        if (lastHeight == 0) lastHeight = 1;

        IDirect3DTexture9* pLastMipTexture = 
            ((miplevel - 1) % 2 == 0) ? m_pHizBufferEven : m_pHizBufferOdd;

        D3DXVECTOR4 lastMipInfo(lastWidth, lastHeight, miplevel - 1, 0);
        hr = m_pHizMipmapEffect->SetVector(m_hMipmapParamLastMipInfo, &lastMipInfo);
        assert(SUCCEEDED(hr));

        hr = m_pHizMipmapEffect->SetTexture(m_hMipmapParamLastMip, pLastMipTexture);
        assert(SUCCEEDED(hr));

        UINT numPasses = 0;
        hr = m_pHizMipmapEffect->Begin(&numPasses, 0);
        assert(SUCCEEDED(hr));

        for (UINT i = 0; i < numPasses; ++i)
        {
	        hr = m_pHizMipmapEffect->BeginPass(i);
            assert(SUCCEEDED(hr));

            hr = m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
            assert(SUCCEEDED(hr));

	        hr = m_pHizMipmapEffect->EndPass();
            assert(SUCCEEDED(hr));
        }

        hr = m_pHizMipmapEffect->End();
        assert(SUCCEEDED(hr));

        hr = m_pHizMipmapEffect->SetTexture(m_hMipmapParamLastMip, NULL);
        assert(SUCCEEDED(hr));
    }
}

void HZBManager::BeginCullingQuery(
    UINT boundsSize,
    const D3DXVECTOR4* bounds,
    const D3DXMATRIX& view,
    const D3DXMATRIX& projection)
{
    EnsureCullingBuffers(boundsSize);

    //
    VBoundPixel* v = NULL;
    HRESULT hr = m_pCullingBoundsBuffer->Lock(0, 0, (void**)&v, D3DLOCK_DISCARD);
    assert(SUCCEEDED(hr));

    for(int i = 0; i < (int)boundsSize; i++)
    {
        v[i].Bound = bounds[i];
        v[i].Pixel = D3DXVECTOR2(i % m_cullingResultsWidth, i / m_cullingResultsWidth);
    }

    hr = m_pCullingBoundsBuffer->Unlock();
    assert(SUCCEEDED(hr));

    hr = m_pDevice->SetRenderTarget(0, m_pCullingResultsSurface);
    assert(SUCCEEDED(hr));

    hr = m_pDevice->SetDepthStencilSurface(NULL);
    assert(SUCCEEDED(hr));

    //
    D3DVIEWPORT9 vp;
    vp.Width = m_cullingResultsWidth;
    vp.Height = m_cullingResultsHeight;
    vp.MinZ = 0;
    vp.MaxZ = 1;
    vp.X = 0;
    vp.Y = 0;
    hr = m_pDevice->SetViewport(&vp);
    assert(SUCCEEDED(hr));

    hr = m_pDevice->SetStreamSource(0, m_pCullingBoundsBuffer, 0, sizeof( VBoundPixel ));
    assert(SUCCEEDED(hr));

    hr = m_pDevice->SetVertexDeclaration(m_pCullingBoundsVertexDecl);
    assert(SUCCEEDED(hr));

    hr = m_pHizCullEffect->SetTechnique(m_hCullingTech);
    assert(SUCCEEDED(hr));

    hr = m_pHizCullEffect->SetMatrix(m_hCullingParamView, &view);
    assert(SUCCEEDED(hr));

	// Query shader does NOT use view matrix, but it's own eye, up, look form
	D3DXMATRIX matQueryViewMatrix;
	D3DXMatrixInverse ( &matQueryViewMatrix, NULL, &view );
    hr = m_pHizCullEffect->SetMatrix(m_hCullingParamViewInv, &matQueryViewMatrix);
    assert(SUCCEEDED(hr));
    
    hr = m_pHizCullEffect->SetMatrix(m_hCullingParamProjection, &projection);
    assert(SUCCEEDED(hr));

	D3DXMATRIX VP = view * projection;
    hr = m_pHizCullEffect->SetMatrix(m_hCullingParamViewProjection, &VP);
    assert(SUCCEEDED(hr));

    Frustum frustum;
    ExtractFrustum(frustum, VP);
    hr = m_pHizCullEffect->SetVectorArray(m_hCullingParamFrustumPlanes, (D3DXVECTOR4*)&frustum, 6);
    assert(SUCCEEDED(hr));

    D3DXVECTOR4 viewport(m_width, m_height, 0, 0);
    hr = m_pHizCullEffect->SetVector(m_hCullingParamViewportSize, &viewport);
    assert(SUCCEEDED(hr));

    D3DXVECTOR4 resultsSize(m_cullingResultsWidth, m_cullingResultsHeight, 0, 0);
    hr = m_pHizCullEffect->SetVector(m_hCullingParamResultsSize, &resultsSize);
    assert(SUCCEEDED(hr));

    hr = m_pHizCullEffect->SetTexture(m_hCullingParamHiZMapEven, m_pHizBufferEven);
    assert(SUCCEEDED(hr));

    hr = m_pHizCullEffect->SetTexture(m_hCullingParamHiZMapOdd, m_pHizBufferOdd);
    assert(SUCCEEDED(hr));

    UINT numPasses = 0;
    hr = m_pHizCullEffect->Begin(&numPasses, 0);
    assert(SUCCEEDED(hr));

    for (UINT i = 0; i < numPasses; ++i)
    {
        hr = m_pHizCullEffect->BeginPass(i);
        assert(SUCCEEDED(hr));

        hr = m_pDevice->DrawPrimitive(D3DPT_POINTLIST, 0, boundsSize);
        assert(SUCCEEDED(hr));

        hr = m_pHizCullEffect->EndPass();
        assert(SUCCEEDED(hr));
    }

    hr = m_pHizCullEffect->End();
    assert(SUCCEEDED(hr));

    hr = m_pHizCullEffect->SetTexture(m_hCullingParamHiZMapEven, NULL);
    assert(SUCCEEDED(hr));

    hr = m_pHizCullEffect->SetTexture(m_hCullingParamHiZMapOdd, NULL);
    assert(SUCCEEDED(hr));
}

void HZBManager::EndCullingQuery(UINT boundsSize, float* pResults)
{
	// NOTE: This IS a bottleneck! In engine tests, my 50 building render was 194fps. When this function
	// was commented in, it dropped to 129fps. Not earthshaking but considerable enough to be worried.
	// Still, we will proceed with optimising the depth rendering stage for 'best occluders' and see if
	// this cost is consumed with the engine running as a whole (as 129fps is still very workable)...
	
	// NOTE NOTE: One idea will be to have a new feature in all in-game object shaders which can read in
	// the 'm_pCullingResultsSurface' texture into the vertex shader (vertex texture fetch) and then
	// modify the vertex position to 'behind the camera' if the texture says the object should be invisible
	// this avoiding any GPU stall and making the occlusion entirely automatic (i.e. no CPU involvement)

    HRESULT hr = m_pDevice->GetRenderTargetData(m_pCullingResultsSurface, m_pCullingResultsSurfaceSys);
    assert(SUCCEEDED(hr));

    D3DLOCKED_RECT pLockedRect;
    hr = m_pCullingResultsSurfaceSys->LockRect(&pLockedRect, NULL, D3DLOCK_READONLY);
    assert(SUCCEEDED(hr));

    BYTE* pBuffer = (BYTE*)pLockedRect.pBits;

    bool done = false;
    for (UINT y = 0; y < m_cullingResultsHeight; y++)
    {
        BYTE* pBufferRow = (BYTE*)pBuffer;
        for (UINT x = 0; x < m_cullingResultsWidth; x++)
        {
            UINT index = (y * m_cullingResultsWidth) + x;
            if (index > boundsSize - 1)
                goto done;

            pResults[index] = (pBufferRow[x] == 255) ? 1 : 0;
        }

        pBuffer += pLockedRect.Pitch;
    }

done:

    hr = m_pCullingResultsSurfaceSys->UnlockRect();
    assert(SUCCEEDED(hr));
}

void HZBManager::Reset()
{
	m_vObjectOccluderList.clear();
	m_vObjectOccludeeList.clear();

    if (m_pMipLevelEvenSurfaces)
    {
        for (int i = 0; i < (int)m_mipLevels; i++)
            SAFE_RELEASE(m_pMipLevelEvenSurfaces[i]);

        delete[] m_pMipLevelEvenSurfaces;
        m_pMipLevelEvenSurfaces = NULL;
    }

    if (m_pMipLevelOddSurfaces)
    {
        for (int i = 0; i < (int)m_mipLevels; i++)
            SAFE_RELEASE(m_pMipLevelOddSurfaces[i]);

        delete[] m_pMipLevelOddSurfaces;
        m_pMipLevelOddSurfaces = NULL;
    }

    SAFE_RELEASE(m_pHizBufferEven);
    SAFE_RELEASE(m_pHizBufferOdd);
}

void HZBManager::Initialize(UINT width, UINT height, UINT initialBoundsBufferSize)
{
    Reset();

    m_width = width;
    m_height = height;

    // HiZ Buffer
    //---------------------------------------------------------------------------------------
    HRESULT hr = m_pDevice->CreateTexture(m_width, m_height, 0, D3DUSAGE_RENDERTARGET, 
        D3DFMT_R32F, D3DPOOL_DEFAULT, &m_pHizBufferEven, NULL);
    assert(SUCCEEDED(hr));

    hr = m_pDevice->CreateTexture(m_width, m_height, 0, D3DUSAGE_RENDERTARGET, 
        D3DFMT_R32F, D3DPOOL_DEFAULT, &m_pHizBufferOdd, NULL);
    assert(SUCCEEDED(hr));

    // HiZ Depth Buffer
    //---------------------------------------------------------------------------------------


    // Generate the Render target and shader resource views that we'll need for rendering
    // the mip levels in the downsampling step.
    //---------------------------------------------------------------------------------------
    m_mipLevels = m_pHizBufferEven->GetLevelCount();
    m_pMipLevelEvenSurfaces = new IDirect3DSurface9*[m_mipLevels];
    m_pMipLevelOddSurfaces = new IDirect3DSurface9*[m_mipLevels];

    for (UINT miplevel = 0; miplevel < m_mipLevels; miplevel++)
    {
        IDirect3DSurface9* pMipSurfaceEven;
        hr = m_pHizBufferEven->GetSurfaceLevel(miplevel, &pMipSurfaceEven);
        assert(SUCCEEDED(hr));

        IDirect3DSurface9* pMipSurfaceOdd;
        hr = m_pHizBufferOdd->GetSurfaceLevel(miplevel, &pMipSurfaceOdd);
        assert(SUCCEEDED(hr));

        m_pMipLevelEvenSurfaces[miplevel] = pMipSurfaceEven;
        m_pMipLevelOddSurfaces[miplevel] = pMipSurfaceOdd;
    }

    // Initialize the culling read back and submission buffers to the initial size.
    EnsureCullingBuffers(initialBoundsBufferSize);
}

void HZBManager::EnsureCullingBuffers(UINT desiredSize)
{
    if (desiredSize > m_boundsSize)
    {
        SAFE_RELEASE(m_pCullingBoundsBuffer);
        SAFE_RELEASE(m_pCullingResultsSurface);
        SAFE_RELEASE(m_pCullingResultsBuffer);
        SAFE_RELEASE(m_pCullingResultsSurfaceSys);
        SAFE_RELEASE(m_pCullingResultsBufferSys);

        m_cullingResultsWidth = 256;
        m_cullingResultsHeight = floor((desiredSize / 256.0f) + 1.0f);

        m_boundsSize = m_cullingResultsWidth * m_cullingResultsHeight;

        HRESULT hr = m_pDevice->CreateTexture(m_cullingResultsWidth, m_cullingResultsHeight, 
            1, D3DUSAGE_RENDERTARGET, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pCullingResultsBuffer, NULL);
        assert(SUCCEEDED(hr));
        // Probably need to use D3DFMT_A8R8G8B8 if D3DFMT_L8 isn't supported, possibly try
        // D3DFMT_P8 also?

        hr = m_pCullingResultsBuffer->GetSurfaceLevel(0, &m_pCullingResultsSurface);
        assert(SUCCEEDED(hr));

        hr = m_pDevice->CreateTexture(m_cullingResultsWidth, m_cullingResultsHeight, 
            1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_SYSTEMMEM, &m_pCullingResultsBufferSys, NULL);
        assert(SUCCEEDED(hr));

        hr = m_pCullingResultsBufferSys->GetSurfaceLevel(0, &m_pCullingResultsSurfaceSys);
        assert(SUCCEEDED(hr));

        hr = m_pDevice->CreateVertexBuffer(m_boundsSize * sizeof( VBoundPixel ), 
            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
	        0, D3DPOOL_DEFAULT, &m_pCullingBoundsBuffer, 0);
        assert(SUCCEEDED(hr));
    }
}

void HZBManager::LoadDepthEffect()
{
    HRESULT hr = LoadEffectFromFile(m_pDevice, L"effectbank\\reloaded\\HZBDepth.fx", &m_pDepthEffect);
    assert(SUCCEEDED(hr));

    m_hDepthEffectTech = m_pDepthEffect->GetTechniqueByName("Render");
    m_hDepthEffectParamWV = m_pDepthEffect->GetParameterByName(0, "WorldView");
    m_hDepthEffectParamWVP = m_pDepthEffect->GetParameterByName(0, "WorldViewProjection");

    D3DVERTEXELEMENT9 pElements[] = {
        {0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        D3DDECL_END()
    };

    hr = m_pDevice->CreateVertexDeclaration((D3DVERTEXELEMENT9*)&pElements, &m_pDepthVertexDecl);
    assert(SUCCEEDED(hr));

    // Create maximum vertex buffer we would need for occluder render (15*4000*3) (can be expanded later)
	//m_dwMaxVertexNumberRequired = 15 * 4000 * 3;
    //hr = m_pDevice->CreateVertexBuffer(m_dwMaxVertexNumberRequired * sizeof( VPos ), D3DUSAGE_WRITEONLY,
	//    0, D3DPOOL_DEFAULT, &m_pOccluderVertexBuffer, 0);
	m_dwMaxVertexNumberRequired = 80000 * 3; // this one can expand as more occluders are added and transformed into vert block data
	m_dwOccluderVertexBufferMaxSize = 80000 * 3; // fix dynamic VB to 80K polygons
    hr = m_pDevice->CreateVertexBuffer(m_dwOccluderVertexBufferMaxSize * sizeof( VPos ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
	    0, D3DPOOL_DEFAULT, &m_pOccluderVertexBuffer, 0);
    assert(SUCCEEDED(hr));

	// clear for later filling (between start and end cull query to use up some CPU)
	m_dwVertexToRenderCount = 0;
    m_pVerticesForNextOcclusionDepthDraw = NULL;

	// Create Z buffer 
	D3DSURFACE_DESC backbufferdesc;
	g_pGlob->pHoldBackBufferPtr->GetDesc(&backbufferdesc);
	D3DFORMAT CommonFormat = backbufferdesc.Format;
	D3DFORMAT DepthFormat = m_ObjectManager.GetValidStencilBufferFormat(CommonFormat);
	HRESULT hRes = m_pD3D->CreateDepthStencilSurface(	512, 256,
														DepthFormat, D3DMULTISAMPLE_NONE, 0, TRUE,
														&m_pOcclusionZBuffer, NULL );
}

void HZBManager::FillDepthRenderStart ( void )
{
	// Done on the fly, but good occluders DO NOT MOVE
	// so we can pre-calculate the vertex transformations once and just tap into them
	// takes memory but 2MB buys us 60,000 polygons so it could be good that way (and fast)
	m_dwVertexToRenderCount = 0;
}

bool HZBManager::FillDepthRenderWithVerts ( sObject* pObject, DWORD* pdwVertexStart, DWORD* pdwVertexCount )
{
	// try again flag
	bool bTryAgain = false;

	// start of vertex write into collection
	*pdwVertexStart = m_dwVertexToRenderCount;

	// ensure create our dump memory buffer before we start off (matches size of standby VB)
	if ( m_pVerticesForNextOcclusionDepthDraw==NULL )
	{
		// create memory block to store all depth occluder verts for next draw pass
		// NOTE: Could be several memory blocks spacially positioned so we only use the
		// depth areas we are near (instead of all of them from everywhere)
		m_pVerticesForNextOcclusionDepthDraw = new float[m_dwMaxVertexNumberRequired*4];
	}
	if ( m_pVerticesForNextOcclusionDepthDraw )
	{
		// get ALL mesh data to populate the depthdrawverts with
		sObject* pActualObject = pObject;
		if ( pObject->pInstanceOfObject ) pActualObject = pObject->pInstanceOfObject;

		// special objects have built-in LOD handling
		int iLowestLOD = -1, iLODLevel = -1;
		for ( DWORD iFrameScan = 0; iFrameScan < (DWORD)pActualObject->iFrameCount; iFrameScan++ )
		{
			LPSTR pFrameName = pActualObject->ppFrameList[iFrameScan]->szName;
			if ( pFrameName[0]=='l' )
			{
				if ( iLODLevel<0 && stricmp ( pFrameName, "lod_0" )==NULL ) { iLODLevel=0; iLowestLOD=iFrameScan; }
				if ( iLODLevel<1 && stricmp ( pFrameName, "lod_1" )==NULL ) { iLODLevel=1; iLowestLOD=iFrameScan; }
				if ( iLODLevel<2 && stricmp ( pFrameName, "lod_2" )==NULL ) { iLODLevel=2; iLowestLOD=iFrameScan; }
			}
		}

		// add meshes to occlusion depth render
		for ( DWORD iFrame = 0; iFrame < (DWORD)pActualObject->iFrameCount; iFrame++ )
		{
			sFrame* pFrame = pActualObject->ppFrameList[iFrame];
			if ( pFrame && (iLowestLOD==-1 || iLowestLOD==iFrame) )
			{
				sMesh* pMesh = pFrame->pMesh;
				if ( pMesh )
				{
					DWORD dwVertCount = pMesh->dwVertexCount;
					if ( pMesh->dwIndexCount>0 ) dwVertCount = pMesh->dwIndexCount;
					if ( m_dwVertexToRenderCount + dwVertCount >= m_dwMaxVertexNumberRequired )
					{
						// can ran out of memory, create larger vertex buffer and try again
						if ( m_dwMaxVertexNumberRequired<20000000 )
						{
							// NOTE 210214 - we can continue from where we left off here instead of
							// trying from scratch, just increase the size of the array (better memory usage too)
							m_dwMaxVertexNumberRequired += (1600000);
							SAFE_DELETE(m_pVerticesForNextOcclusionDepthDraw);
							//SAFE_RELEASE(m_pOccluderVertexBuffer);
							m_pVerticesForNextOcclusionDepthDraw = new float[m_dwMaxVertexNumberRequired*4];
							//HRESULT hr = m_pDevice->CreateVertexBuffer(m_dwMaxVertexNumberRequired * sizeof( VPos ), D3DUSAGE_WRITEONLY,
							//	0, D3DPOOL_DEFAULT, &m_pOccluderVertexBuffer, 0);
							//assert(SUCCEEDED(hr));
							bTryAgain = true;
						}
						return bTryAgain;
					}
					else
					{
						float* pSrcPtrAt = (float*)pMesh->pVertexData;
						float* pDestPtrAt = m_pVerticesForNextOcclusionDepthDraw + (m_dwVertexToRenderCount*4);
						DWORD dwFVFSize = pMesh->dwFVFSize;
						DWORD dwSizeInFloats = dwFVFSize/4;
						D3DXMATRIX matFinal;
						if ( pMesh->pBones )
						{
							matFinal = pMesh->pBones[0].matTranslation;
							D3DXMatrixMultiply ( &matFinal, &matFinal, &pFrame->matCombined );
						}
						else
						{
							matFinal = pFrame->matCombined;
						}
						D3DXMATRIX matScale;
						D3DXMatrixScaling ( &matScale, pObject->position.vecScale.x, pObject->position.vecScale.y, pObject->position.vecScale.z );
						D3DXMatrixMultiply ( &matFinal, &matFinal, &matScale );
						D3DXMATRIX matRotateItem;
						D3DXMatrixRotationYawPitchRoll ( &matRotateItem, D3DXToRadian(pObject->position.vecRotate.y), D3DXToRadian(pObject->position.vecRotate.x), D3DXToRadian(pObject->position.vecRotate.z) );
						D3DXMatrixMultiply ( &matFinal, &matFinal, &matRotateItem );
						DWORD dwIndexCount = pMesh->dwIndexCount;
						if ( dwIndexCount>0 )
						{
							// index data points to vertex data
							WORD* pIndexLookup = pMesh->pIndices;
							for ( DWORD i=0; i<dwIndexCount; i++ )
							{
								D3DXVECTOR3 vec = *(D3DXVECTOR3*)(pSrcPtrAt+(pIndexLookup[i]*dwSizeInFloats));
								D3DXVec3TransformCoord ( &vec, &vec, &matFinal );
								vec.x += pObject->position.vecPosition.x;
								vec.y += pObject->position.vecPosition.y;
								vec.z += pObject->position.vecPosition.z;
								*(pDestPtrAt+0) = vec.x;
								*(pDestPtrAt+1) = vec.y;
								*(pDestPtrAt+2) = vec.z;
								*(pDestPtrAt+3) = 1.0f;
								m_dwVertexToRenderCount++;
								pDestPtrAt+=4;
							}
						}
						else
						{
							// vertex data only
							for ( DWORD v=0; v<dwVertCount; v++ )
							{
								D3DXVECTOR3 vec = *(D3DXVECTOR3*)pSrcPtrAt;
								D3DXVec3TransformCoord ( &vec, &vec, &matFinal );
								vec.x += pObject->position.vecPosition.x;
								vec.y += pObject->position.vecPosition.y;
								vec.z += pObject->position.vecPosition.z;
								*(pDestPtrAt+0) = vec.x;
								*(pDestPtrAt+1) = vec.y;
								*(pDestPtrAt+2) = vec.z;
								*(pDestPtrAt+3) = 1.0f;
								m_dwVertexToRenderCount++;
								pSrcPtrAt+=dwSizeInFloats;
								pDestPtrAt+=4;
							}
						}
					}
				}
			}
		}
	}

	// when finished writing vertices, record size
	*pdwVertexCount = m_dwVertexToRenderCount - (*pdwVertexStart);

	// complete
	return bTryAgain;
}

void HZBManager::FillDepthRenderDone ( void )
{
	/* now done dynamically in main depth render code
    VPos* v = NULL;
    HRESULT hr = m_pOccluderVertexBuffer->Lock(0, 0, (void**)&v, 0);
    assert(SUCCEEDED(hr));
    memcpy((void*)v, m_pVerticesForNextOcclusionDepthDraw, m_dwVertexToRenderCount*4*sizeof(float));
    hr = m_pOccluderVertexBuffer->Unlock();
    assert(SUCCEEDED(hr));
	*/
}

void HZBManager::LoadMipmapEffect()
{
    HRESULT hr = LoadEffectFromFile(m_pDevice, L"effectbank\\reloaded\\HZBMipmap.fx", &m_pHizMipmapEffect);
    assert(SUCCEEDED(hr));

    m_hMipmapTech = m_pHizMipmapEffect->GetTechniqueByName("Render");
    m_hMipmapParamLastMipInfo = m_pHizMipmapEffect->GetParameterByName(0, "LastMipInfo");
    m_hMipmapParamLastMip =     m_pHizMipmapEffect->GetParameterByName(0, "LastMip");

    // 
    D3DVERTEXELEMENT9 pElements[] = {
        {0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        D3DDECL_END()
    };

    hr = m_pDevice->CreateVertexDeclaration((D3DVERTEXELEMENT9*)&pElements, &m_pHizMipmapVertexDecl);
    assert(SUCCEEDED(hr));

    // 
    hr = m_pDevice->CreateVertexBuffer(4 * sizeof( VPos ), D3DUSAGE_WRITEONLY,
	    0, D3DPOOL_MANAGED, &m_pHizMipmapVertexBuffer, 0);
    assert(SUCCEEDED(hr));

    VPos vertices[] =
    {
        { D3DXVECTOR4(-1.0f,  1.0f, 0.0f, 1.0f) },
        { D3DXVECTOR4( 1.0f,  1.0f, 0.0f, 1.0f) },
        { D3DXVECTOR4(-1.0f, -1.0f, 0.0f, 1.0f) },
        { D3DXVECTOR4( 1.0f, -1.0f, 0.0f, 1.0f) },
    };

    VPos* v = NULL;
    hr = m_pHizMipmapVertexBuffer->Lock(0, 0, (void**)&v, 0);
    assert(SUCCEEDED(hr));

    memcpy((void*)v, &vertices, sizeof(vertices));

    hr = m_pHizMipmapVertexBuffer->Unlock();
    assert(SUCCEEDED(hr));
}

void HZBManager::LoadCullEffect()
{
    HRESULT hr = LoadEffectFromFile(m_pDevice, L"effectbank\\reloaded\\HZBCull.fx", &m_pHizCullEffect);
    assert(SUCCEEDED(hr));

    m_hCullingTech = m_pHizCullEffect->GetTechniqueByName("Render");
    m_hCullingParamView = m_pHizCullEffect->GetParameterByName(0, "View");
    m_hCullingParamViewInv = m_pHizCullEffect->GetParameterByName(0, "ViewInv");
    m_hCullingParamProjection = m_pHizCullEffect->GetParameterByName(0, "Projection");
    m_hCullingParamViewProjection = m_pHizCullEffect->GetParameterByName(0, "ViewProjection");
    m_hCullingParamFrustumPlanes = m_pHizCullEffect->GetParameterByName(0, "FrustumPlanes");
    m_hCullingParamViewportSize = m_pHizCullEffect->GetParameterByName(0, "ViewportSize");
    m_hCullingParamResultsSize = m_pHizCullEffect->GetParameterByName(0, "ResultsSize");
    m_hCullingParamHiZMapEven = m_pHizCullEffect->GetParameterByName(0, "HiZMapEven");
    m_hCullingParamHiZMapOdd = m_pHizCullEffect->GetParameterByName(0, "HiZMapOdd");

    // 
    D3DVERTEXELEMENT9 pElements[] = {
        {0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()
    };

    hr = m_pDevice->CreateVertexDeclaration((D3DVERTEXELEMENT9*)&pElements, &m_pCullingBoundsVertexDecl);
    assert(SUCCEEDED(hr));
}