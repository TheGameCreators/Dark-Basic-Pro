
//
// DBOMesh Functions Implementation
//

//////////////////////////////////////////////////////////////////////////////////
// DBOMESH HEADER ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//#define _CRT_SECURE_NO_DEPRECATE
#include "DBOMesh.h"
#include "DBOEffects.h"
#include "stdio.h"
#include <mmsystem.h>			// multimedia functions

// Externals for DBO/Manager relationship
#include <vector>
extern std::vector< sMesh* >		g_vRefreshMeshList;

//////////////////////////////////////////////////////////////////////////////////
// LOCAL CLASS TO HELP WITH SHADOW CALCULATION ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define ADJACENCY_EPSILON 0.0001f
#define EXTRUDE_EPSILON 0.1f

struct SHADOWVERT
{
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Normal;
    const static D3DVERTEXELEMENT9 Decl[3];
};

const D3DVERTEXELEMENT9 SHADOWVERT::Decl[3] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
    D3DDECL_END()
};

struct CEdgeMapping
{
    int m_anOldEdge[2];  // vertex index of the original edge
    int m_aanNewEdge[2][2]; // vertex indexes of the new edge
                            // First subscript = index of the new edge
                            // Second subscript = index of the vertex for the edge

public:
    CEdgeMapping()
    {
        FillMemory( m_anOldEdge, sizeof(m_anOldEdge), -1 );
        FillMemory( m_aanNewEdge, sizeof(m_aanNewEdge), -1 );
    }
};

//////////////////////////////////////////////////////////////////////////////////
// INTERNAL MESH HELPER FUNCTIONS ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK_DLL bool EnsureTextureStageValid ( sMesh* pMesh, int iTextureStage )
{
//  260105 - it is OK if the texture stage is BIGGER than current, modified below
//	// Only if not shader, shaders are left alone and in tact and they will know to use the second UV stage
//	if ( !pMesh->bUseVertexShader )
//	{
		// textureindex
		DWORD dwTextureIndex = 1+iTextureStage;

		// create texture array if not present or too small
		if ( pMesh->dwTextureCount < dwTextureIndex )
		{
			sTexture* pNewTextureArray = NULL;
			pNewTextureArray = new sTexture [ dwTextureIndex ];
			g_pGlob->dwInternalFunctionCode=11013;
			if ( pMesh->pTextures ) memcpy ( pNewTextureArray, pMesh->pTextures, sizeof(sTexture) * pMesh->dwTextureCount );

			// Remove old array
			SAFE_DELETE_ARRAY( pMesh->pTextures );

			// Assign new texture array
			pMesh->pTextures = pNewTextureArray;
			pMesh->dwTextureCount = dwTextureIndex;
		}

		/*
		// create texture array if not present
		if ( pMesh->dwTextureCount != dwTextureIndex )
		{
			sTexture* pNewTextureArray = NULL;
			if ( pMesh->dwTextureCount < dwTextureIndex )
			{
				// Create larger array
				pNewTextureArray = new sTexture [ dwTextureIndex ];

				// Copy contents to new array
				if ( pMesh->pTextures ) memcpy ( pNewTextureArray, pMesh->pTextures, sizeof(sTexture) * pMesh->dwTextureCount );
			}
			else
			{
				// Create smaller array 
				pNewTextureArray = new sTexture [ dwTextureIndex ];

				// Copy contents to new array
				if ( pMesh->pTextures ) memcpy ( pNewTextureArray, pMesh->pTextures, sizeof(sTexture) * dwTextureIndex );
			}

			// Remove old array
			SAFE_DELETE_ARRAY( pMesh->pTextures );

			// Assign new texture array
			pMesh->pTextures = pNewTextureArray;
			pMesh->dwTextureCount = dwTextureIndex;
		}
		*/
//	}

	// okay
	return true;
}

DARKSDK_DLL float AlphaFromRGBA ( DWORD dwRGB )
{
	return ((dwRGB & (255 << 24)) >> 24) / 255.0f;
}

DARKSDK_DLL float RedFromRGBA ( DWORD dwRGB )
{
	return ((dwRGB & (255 << 16)) >> 16) / 255.0f;
}

DARKSDK_DLL float GreenFromRGBA ( DWORD dwRGB )
{
	return ((dwRGB & (255 <<  8)) >>  8) / 255.0f;
}

DARKSDK_DLL float BlueFromRGBA ( DWORD dwRGB )
{
	return ((dwRGB & (255 <<  0)) >>  0) / 255.0f;
}

DARKSDK_DLL void ResetMaterial ( D3DMATERIAL9* pMaterial )
{
	pMaterial->Diffuse.r		= 1.0f;
	pMaterial->Diffuse.g		= 1.0f;
	pMaterial->Diffuse.b		= 1.0f;
	pMaterial->Diffuse.a		= 1.0f;
	pMaterial->Ambient.r		= 1.0f;
	pMaterial->Ambient.g		= 1.0f;
	pMaterial->Ambient.b		= 1.0f;
	pMaterial->Ambient.a		= 1.0f;
	pMaterial->Specular.r		= 0.0f;
	pMaterial->Specular.g		= 0.0f;
	pMaterial->Specular.b		= 0.0f;
	pMaterial->Specular.a		= 0.0f;
	pMaterial->Emissive.r		= 0.0f;
	pMaterial->Emissive.g		= 0.0f;
	pMaterial->Emissive.b		= 0.0f;
	pMaterial->Emissive.a		= 0.0f;
	pMaterial->Power			= 10.0f;
}

DARKSDK_DLL void ColorMaterial ( D3DMATERIAL9* pMaterial, DWORD dwRGBA )
{
	// Set Diffuse Of Material
	pMaterial->Diffuse.r = RedFromRGBA		( dwRGBA );
	pMaterial->Diffuse.g = GreenFromRGBA	( dwRGBA );
	pMaterial->Diffuse.b = BlueFromRGBA		( dwRGBA );
	pMaterial->Diffuse.a = AlphaFromRGBA	( dwRGBA );
}

DARKSDK_DLL D3DXVECTOR3 MultiplyVectorAndMatrix ( D3DXVECTOR3 &vec, D3DXMATRIX &m )
{
	D3DXVECTOR3 vecFinal;

	vecFinal.x = vec.x * m._11 + vec.y * m._21 + vec.z * m._31 + m._41;
	vecFinal.y = vec.x * m._12 + vec.y * m._22 + vec.z * m._32 + m._42;
	vecFinal.z = vec.x * m._13 + vec.y * m._23 + vec.z * m._33 + m._43;

	return vecFinal;
}

DARKSDK_DLL void LightEval(D3DXVECTOR4 *col,D3DXVECTOR2 *input, D3DXVECTOR2 *sampSize,void *pfPower)
{
    float fPower = (float) pow(input->y,*((float*)pfPower));
    col->x = fPower;
    col->y = fPower;
    col->z = fPower;
    col->w = input->x;
}

//////////////////////////////////////////////////////////////////////////////////
// INTERNAL MESH SHADER FUNCTIONS ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK_DLL BOOL SupportsVertexShaderV11()
{
    D3DCAPS9 d3dCaps;
    m_pD3D->GetDeviceCaps( &d3dCaps );
    if ( d3dCaps.VertexShaderVersion >= D3DVS_VERSION ( 1,1 ) )
	    return TRUE;

    return FALSE;
}

DARKSDK_DLL BOOL SupportsPixelShaderV11()
{
    D3DCAPS9 d3dCaps;
    m_pD3D->GetDeviceCaps( &d3dCaps );
    if ( d3dCaps.PixelShaderVersion >= D3DPS_VERSION ( 1,1 ) )
	    return TRUE;

    return FALSE;
}

DARKSDK_DLL void FreeVertexShaderMesh ( sMesh* pMesh ) 
{
	// store FVF before making shader
	if ( pMesh->pVertexShaderEffect )
	{
		// restore FVF format (if original known)
		if ( pMesh->dwFVF==0 && pMesh->dwFVFOriginal!=0 )
			RestoreLocalMesh ( pMesh );

		// if reference, simply blank effectptr
		if ( pMesh->bVertexShaderEffectRefOnly==true )
		{
			// clear reference from mesh
			pMesh->pVertexShaderEffect = NULL;

			// leefix - 011013 - should at least wipe ptrs out
			pMesh->pVertexShader = NULL;

			// 160616 - keep hold of this in case a shader replaces another that uses SAME vert dec (GG character shader)
			// as a fundamental problem is that the orig vertdata is WIPED OUT during the shader/clone process so you cannot
			// generate the shader verts again to create pVertexDec a fresh, and it gets wiped out
			//pMesh->pVertexDec = NULL;

			// 221114 - and also wipe any reference to shader use
			pMesh->bUseVertexShader = false;
			pMesh->bOverridePixelShader = false;
		}
		else
		{
			// release any previous shader
			SAFE_DELETE ( pMesh->pVertexShaderEffect );
			SAFE_RELEASE ( pMesh->pVertexShader );
			SAFE_RELEASE ( pMesh->pVertexDec );

			// deactivate shader usage
			pMesh->bUseVertexShader = false;
			pMesh->bOverridePixelShader = false;
		}

		// clear from mesh
		pMesh->bVertexShaderEffectRefOnly = false;
		strcpy ( pMesh->pEffectName, "" );
	}
}

DARKSDK_DLL void ClearTextureSettings ( sMesh* pMesh )
{
	// create texture array if not present
	if ( !EnsureTextureStageValid ( pMesh, 0 ) )
		return;

	// clear alpha factor of mesh
	pMesh->dwAlphaOverride = 0;
	pMesh->bAlphaOverride = false;

	// get texture ptr
	sTexture* pTexture = &pMesh->pTextures [ 0 ];

	// free resource
	SAFE_RELEASE ( pTexture->pCubeTexture );

	// set base texture to defaults
	pTexture->dwStage=0;
	pTexture->dwBlendMode=D3DTOP_SELECTARG1;	
	pTexture->dwBlendArg1=D3DTA_DIFFUSE;
	pTexture->dwBlendArg2=D3DTA_DIFFUSE;
	pTexture->dwTexCoordMode=0;

	// Delete any vertex shader being used
	FreeVertexShaderMesh ( pMesh );
}

DARKSDK_DLL bool ValidateMeshForShader ( sMesh* pMesh, DWORD dwStagesRequired )
{
	// Delete any vertex shader being used
	FreeVertexShaderMesh ( pMesh );

	// check support
	if(!SupportsVertexShaderV11())
		return false;

	// create a two stage texture array
	if ( !EnsureTextureStageValid ( pMesh, dwStagesRequired-1 ) )
		return false;

	// store original FVF before making shader
	if ( pMesh->dwFVFOriginal==0 && pMesh->dwFVF>0 )
		pMesh->dwFVFOriginal = pMesh->dwFVF;

	// okay
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
// MESH FUNCTIONS ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Mesh Setting Functions

DARKSDK_DLL void SetWireframe ( sMesh* pMesh, bool bWireframe )
{
	pMesh->bWireframe = bWireframe;
}

DARKSDK_DLL void SetTransparency ( sMesh* pMesh, bool bTransparency )
{
	pMesh->bTransparency = bTransparency;
}

DARKSDK_DLL void SetAlphaTest ( sMesh* pMesh, DWORD dwAlphaTestValue )
{
	pMesh->dwAlphaTestValue = dwAlphaTestValue;
}

DARKSDK_DLL void SetCullCWCCW ( sMesh* pMesh, int iCullMode )
{
	if ( iCullMode>0 )
	{
		pMesh->iCullMode = iCullMode;
		pMesh->bCull = true;
	}
	else
		pMesh->bCull = false;
}

DARKSDK_DLL void SetCull ( sMesh* pMesh, bool bCull )
{
	pMesh->bCull = bCull;
}

DARKSDK_DLL void SetZRead ( sMesh* pMesh, bool bZRead )
{
	pMesh->bZRead = bZRead;
}

DARKSDK_DLL void SetZWrite ( sMesh* pMesh, bool bZWrite )
{
	pMesh->bZWrite = bZWrite;
}

DARKSDK_DLL void SetZBias ( sMesh* pMesh, bool bZBias, float fSlopeScale, float fDepth )
{
	pMesh->bZBiasActive = bZBias;
	pMesh->fZBiasSlopeScale = fSlopeScale;
	pMesh->fZBiasDepth = fDepth;
}

DARKSDK_DLL void SetFilter ( sMesh* pMesh, int iStage, int iFilter )
{
	if ( (DWORD)iStage<pMesh->dwTextureCount )
	{
		iFilter++; // DBV1 compatable.
		//    D3DTEXF_NONE = 0,
		//    D3DTEXF_POINT = 1,
		//    D3DTEXF_LINEAR = 2,
		//    D3DTEXF_ANISOTROPIC = 3,
		//    D3DTEXF_PYRAMIDALQUAD = 6,
		//    D3DTEXF_GAUSSIANQUAD = 7,
		sTexture* pTexture = &pMesh->pTextures[iStage];
		if(pTexture)
		{
			// confirm existence of filter in hardware, else use default
		    D3DCAPS9 d3dCaps;
			m_pD3D->GetDeviceCaps( &d3dCaps );
			DWORD dwMinCapsFlag = 0, dwMagCapsFlag = 0;
			if ( iFilter==D3DTEXF_LINEAR ) { dwMinCapsFlag = D3DPTFILTERCAPS_MINFLINEAR; dwMagCapsFlag = D3DPTFILTERCAPS_MAGFLINEAR; }
			if ( iFilter==D3DTEXF_ANISOTROPIC ) { dwMinCapsFlag = D3DPTFILTERCAPS_MINFANISOTROPIC; dwMagCapsFlag = D3DPTFILTERCAPS_MAGFANISOTROPIC; }
			if ( iFilter==D3DTEXF_PYRAMIDALQUAD ) { dwMinCapsFlag = D3DPTFILTERCAPS_MINFPYRAMIDALQUAD; dwMagCapsFlag = D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD; }
			if ( iFilter==D3DTEXF_GAUSSIANQUAD ) { dwMinCapsFlag = D3DPTFILTERCAPS_MINFGAUSSIANQUAD; dwMagCapsFlag = D3DPTFILTERCAPS_MAGFGAUSSIANQUAD; }
			(d3dCaps.TextureFilterCaps & dwMinCapsFlag) ? pTexture->dwMinState = iFilter : pTexture->dwMinState = D3DTEXF_POINT;
			(d3dCaps.TextureFilterCaps & dwMagCapsFlag) ? pTexture->dwMagState = iFilter : pTexture->dwMagState = D3DTEXF_POINT;
		}
	}
}

DARKSDK_DLL void SetFilter ( sMesh* pMesh, int iFilter )
{
	SetFilter ( pMesh, 0, iFilter );
}

DARKSDK_DLL void SetLight ( sMesh* pMesh, bool bLight )
{
	pMesh->bLight = bLight;
}

DARKSDK_DLL void SetFog ( sMesh* pMesh, bool bFog )
{
	pMesh->bFog = bFog;
}

DARKSDK_DLL void SetAmbient ( sMesh* pMesh, bool bAmbient )
{
	pMesh->bAmbient = bAmbient;
}

// Mesh Component Data Functions

DARKSDK_DLL void SetDiffuseEx ( sMesh* pMesh, DWORD dwRGB )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// make sure we have data in the vertices
	if ( pMesh->dwFVF & D3DFVF_DIFFUSE )
	{
		// go through all of the vertices
		for ( int iCurrentVertex = 0; iCurrentVertex < (int)pMesh->dwVertexCount; iCurrentVertex++ )
		{
			// dwDiffuse = RGB
			*( ( DWORD* ) pMesh->pVertexData + offsetMap.dwDiffuse + ( offsetMap.dwSize * iCurrentVertex ) ) = dwRGB;
		}
	}

	// flag mesh for a VB update
	pMesh->bVBRefreshRequired=true;
	g_vRefreshMeshList.push_back ( pMesh );
}

DARKSDK_DLL void SetDiffuse	( sMesh* pMesh, float fPercentage )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// make sure we have data in the vertices
	if ( pMesh->dwFVF & D3DFVF_DIFFUSE )
	{
		// calculate an RGB from iPercentage
		DWORD dwColor = (DWORD)(fPercentage*255);
		DWORD dwRGB = D3DCOLOR_ARGB ( dwColor, dwColor, dwColor, dwColor );
		SetDiffuseEx ( pMesh, dwRGB );
	}
	else
	{
		// else apply diffuse to material and activate
		pMesh->bUsesMaterial=true;
		pMesh->mMaterial.Diffuse.r = fPercentage;
		pMesh->mMaterial.Diffuse.g = fPercentage;
		pMesh->mMaterial.Diffuse.b = fPercentage;
		pMesh->mMaterial.Diffuse.a = 1.0f;
		pMesh->mMaterial.Ambient.r = fPercentage;
		pMesh->mMaterial.Ambient.g = fPercentage;
		pMesh->mMaterial.Ambient.b = fPercentage;
		pMesh->mMaterial.Ambient.a = 1.0f;
		pMesh->mMaterial.Specular.r = 0.0f;
		pMesh->mMaterial.Specular.g = 0.0f;
		pMesh->mMaterial.Specular.b = 0.0f;
		pMesh->mMaterial.Specular.a = 0.0f;
		pMesh->mMaterial.Emissive.r = 0.0f;
		pMesh->mMaterial.Emissive.g = 0.0f;
		pMesh->mMaterial.Emissive.b = 0.0f;
		pMesh->mMaterial.Emissive.a = 0.0f;
	}
}

DARKSDK_DLL void ScrollTexture ( sMesh* pMesh, int iStage, float fU, float fV )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// permission
	bool bGoAhead = false;
	DWORD dwTB = pMesh->dwFVF & D3DFVF_TEXCOUNT_MASK;
	if ( iStage==0 && ( (dwTB==D3DFVF_TEX1) || (dwTB==D3DFVF_TEX2) || (dwTB==D3DFVF_TEX3) ) ) bGoAhead = true;
	if ( iStage==1 && ( (dwTB==D3DFVF_TEX2) || (dwTB==D3DFVF_TEX3) ) ) bGoAhead = true;
	if ( iStage==2 && ( (dwTB==D3DFVF_TEX3) ) ) bGoAhead = true;

	// make sure we have data in the vertices
	if ( bGoAhead )
	{
		// go through all of the vertices
		for ( int iCurrentVertex = 0; iCurrentVertex < (int)pMesh->dwVertexCount; iCurrentVertex++ )
		{
			*( ( float* ) pMesh->pVertexData + offsetMap.dwTU[iStage] + ( offsetMap.dwSize * iCurrentVertex ) ) += fU;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwTV[iStage] + ( offsetMap.dwSize * iCurrentVertex ) ) += fV;
		}
	}

	// flag mesh for a VB update
	pMesh->bVBRefreshRequired=true;
	g_vRefreshMeshList.push_back ( pMesh );
}

DARKSDK_DLL void ScrollTexture ( sMesh* pMesh, float fU, float fV )
{
	ScrollTexture ( pMesh, 0, fU, fV );
}

DARKSDK_DLL void ScaleTexture ( sMesh* pMesh, int iStage, float fU, float fV )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// permission
	bool bGoAhead = false;
	DWORD dwTB = pMesh->dwFVF & D3DFVF_TEXCOUNT_MASK;
	if ( iStage==0 && ( (dwTB==D3DFVF_TEX1) || (dwTB==D3DFVF_TEX2) || (dwTB==D3DFVF_TEX3) ) ) bGoAhead = true;
	if ( iStage==1 && ( (dwTB==D3DFVF_TEX2) || (dwTB==D3DFVF_TEX3) ) ) bGoAhead = true;
	if ( iStage==2 && ( (dwTB==D3DFVF_TEX3) ) ) bGoAhead = true;

	// make sure we have data in the vertices
	if ( bGoAhead )
	{
		// go through all of the vertices
		for ( int iCurrentVertex = 0; iCurrentVertex < (int)pMesh->dwVertexCount; iCurrentVertex++ )
		{
			*( ( float* ) pMesh->pVertexData + offsetMap.dwTU[iStage] + ( offsetMap.dwSize * iCurrentVertex ) ) *= fU;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwTV[iStage] + ( offsetMap.dwSize * iCurrentVertex ) ) *= fV;
		}
	}

	// flag mesh for a VB update
	pMesh->bVBRefreshRequired=true;
	g_vRefreshMeshList.push_back ( pMesh );
}

DARKSDK_DLL void ScaleTexture ( sMesh* pMesh, float fU, float fV )
{
	// modify stage zero UV data only
	ScaleTexture ( pMesh, 0, fU, fV );
}

DARKSDK_DLL void SmoothNormals ( sMesh* pMesh, float fPercentage )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// assume no more than 32 shared vertex points
	DWORD dwSharedVertexMax=32;
	DWORD dwNumberOfVertices=pMesh->dwVertexCount;
	BYTE* NormalCount = new BYTE [ dwNumberOfVertices ];
	ZeroMemory ( NormalCount, dwNumberOfVertices*sizeof(BYTE) );
	D3DXVECTOR3* fNormals = new D3DXVECTOR3 [dwNumberOfVertices*dwSharedVertexMax];

	// make sure we have data in the vertices
	if ( offsetMap.dwZ>0 && offsetMap.dwNZ>0 )
	{
		// go through all of the vertices
		for ( int iCurrentVertex = 0; iCurrentVertex < (int)dwNumberOfVertices; iCurrentVertex++ )
		{
			// see how many other vertices share this space
			for ( int iScanVert = 0; iScanVert < (int)dwNumberOfVertices; iScanVert++ )
			{
				// get vertex position and scan vertex position
				D3DXVECTOR3 vecVert = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) );
				D3DXVECTOR3 vecScanVert = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iScanVert ) );

				// if they match, add normal to table
				if ( iCurrentVertex!=iScanVert && vecVert==vecScanVert )
				{
					// get normal from the scanned vertex
					D3DXVECTOR3 vecScannedNormal = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwNX + ( offsetMap.dwSize * iScanVert ) );

					// add normal to table
					BYTE Index = NormalCount [ iCurrentVertex ];
					if ( Index < 32 )
					{
						fNormals [ (iCurrentVertex*dwSharedVertexMax)+Index ] = vecScannedNormal;
						NormalCount [ iCurrentVertex ] = NormalCount [ iCurrentVertex ] + 1;
					}
				}
			}
		}
	}

	// for each vertex, choose new normal that fits the smoothing rule
	for ( int iCurrentVertex = 0; iCurrentVertex < (int)dwNumberOfVertices; iCurrentVertex++ )
	{
		// get normal from vertex
		D3DXVECTOR3 vecOrigNormal = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwNX + ( offsetMap.dwSize * iCurrentVertex ) );
				
		// for any vertices that share this space, consider their normals
		DWORD dwOtherNormalsCount = NormalCount [ iCurrentVertex ];
		if ( dwOtherNormalsCount > 0 )
		{
			// clear averaging vector
			DWORD dwAverageCount = 1;
			D3DXVECTOR3 vecAverageNormal = vecOrigNormal;

			// only include normals whos angle is within rule
			for ( DWORD iIndex=0; iIndex<dwOtherNormalsCount; iIndex++ )
			{
				// get other normal
				D3DXVECTOR3 vecOtherNormal = fNormals [ (iCurrentVertex*dwSharedVertexMax)+iIndex ];

				// calculate difference
				float fDiffX = (float)fabs(vecOrigNormal.x - vecOtherNormal.x);
				float fDiffY = (float)fabs(vecOrigNormal.y - vecOtherNormal.y);
				float fDiffZ = (float)fabs(vecOrigNormal.z - vecOtherNormal.z);

				// if all are within rule, add to averaging normal
				float fDifference = (fDiffX+fDiffY+fDiffZ) / 3.0f;
				if ( fDifference <= fPercentage )
				{
					vecAverageNormal += vecOtherNormal;
					dwAverageCount++;
				}
			}

			// apply new normal to data
			vecAverageNormal /= (float)dwAverageCount;
			*(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwNX + ( offsetMap.dwSize * iCurrentVertex ) ) = vecAverageNormal;
		}
	}

	// free usages
	SAFE_DELETE ( NormalCount );
	SAFE_DELETE ( fNormals );

	// flag mesh for a VB update
	pMesh->bVBRefreshRequired=true;
	g_vRefreshMeshList.push_back ( pMesh );
}

DARKSDK_DLL void GenerateNormals ( sMesh* pMesh )
{
	// calculate new normals from current mesh data
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	if ( offsetMap.dwZ>0 && offsetMap.dwNZ>0 )
		GenerateNewNormalsForMesh ( pMesh );
}

DARKSDK_DLL void AddBoxToMesh ( sMesh* pMesh, DWORD* pdwVertexOffset, DWORD* pdwIndexOffset, float fWidth1, float fHeight1, float fDepth1, float fWidth2, float fHeight2, float fDepth2, DWORD dwColor,
					bool bL, bool bR, bool bU, bool bD, bool bB, bool bF )
{
	// vertex offset
	int iVertIndexOffset = *pdwVertexOffset;
	int iIndiceIndexOffset = *pdwIndexOffset;

	// include faces that are flagged
	if ( bF )
	{
		SetupStandardVertexDec ( pMesh, pMesh->pVertexData, iVertIndexOffset +  0, fWidth1, fHeight2, fDepth1,  0.0f,  0.0f, -1.0f, dwColor, 0.00f, 0.00f );	// front
		SetupStandardVertexDec ( pMesh, pMesh->pVertexData, iVertIndexOffset +  1, fWidth2, fHeight2, fDepth1,  0.0f,  0.0f, -1.0f, dwColor, 1.00f, 0.00f );
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  2, fWidth2, fHeight1, fDepth1,  0.0f,  0.0f, -1.0f, dwColor, 1.00f, 1.00f );
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  3, fWidth1, fHeight1, fDepth1,  0.0f,  0.0f, -1.0f, dwColor, 0.00f, 1.00f );
		pMesh->pIndices [ iIndiceIndexOffset +  0 ] = iVertIndexOffset +  0;		pMesh->pIndices [ iIndiceIndexOffset +  1 ] = iVertIndexOffset +  1;		pMesh->pIndices [ iIndiceIndexOffset +  2 ] = iVertIndexOffset +  2;
		pMesh->pIndices [ iIndiceIndexOffset +  3 ] = iVertIndexOffset +  2;		pMesh->pIndices [ iIndiceIndexOffset +  4 ] = iVertIndexOffset +  3;		pMesh->pIndices [ iIndiceIndexOffset +  5 ] = iVertIndexOffset +  0;
		iIndiceIndexOffset+=6;
		iVertIndexOffset+=4;
	}
	if ( bB )
	{
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  0, fWidth1, fHeight2, fDepth2,  0.0f,  0.0f,  1.0f, dwColor, 1.00f, 0.00f );	// back
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  1, fWidth1, fHeight1, fDepth2,  0.0f,  0.0f,  1.0f, dwColor, 1.00f, 1.00f );
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  2, fWidth2, fHeight1, fDepth2,  0.0f,  0.0f,  1.0f, dwColor, 0.00f, 1.00f );
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  3, fWidth2, fHeight2, fDepth2,  0.0f,  0.0f,  1.0f, dwColor, 0.00f, 0.00f );
		pMesh->pIndices [ iIndiceIndexOffset +  0 ] = iVertIndexOffset +  0;		pMesh->pIndices [ iIndiceIndexOffset +  1 ] = iVertIndexOffset +  1;		pMesh->pIndices [ iIndiceIndexOffset +  2 ] = iVertIndexOffset +  2;
		pMesh->pIndices [ iIndiceIndexOffset +  3 ] = iVertIndexOffset +  2;		pMesh->pIndices [ iIndiceIndexOffset +  4 ] = iVertIndexOffset +  3;		pMesh->pIndices [ iIndiceIndexOffset +  5 ] = iVertIndexOffset +  0;
		iIndiceIndexOffset+=6;
		iVertIndexOffset+=4;
	}
	if ( bD )
	{
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  0, fWidth1, fHeight2, fDepth2,	 0.0f,  1.0f,  0.0f, dwColor, 0.00f, 0.00f );	// top
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  1, fWidth2, fHeight2, fDepth2,	 0.0f,  1.0f,  0.0f, dwColor, 1.00f, 0.00f );
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  2, fWidth2, fHeight2, fDepth1,	 0.0f,  1.0f,  0.0f, dwColor, 1.00f, 1.00f );
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  3, fWidth1, fHeight2, fDepth1,	 0.0f,  1.0f,  0.0f, dwColor, 0.00f, 1.00f );
		pMesh->pIndices [ iIndiceIndexOffset +  0 ] = iVertIndexOffset +  0;		pMesh->pIndices [ iIndiceIndexOffset +  1 ] = iVertIndexOffset +  1;		pMesh->pIndices [ iIndiceIndexOffset +  2 ] = iVertIndexOffset +  2;
		pMesh->pIndices [ iIndiceIndexOffset +  3 ] = iVertIndexOffset +  2;		pMesh->pIndices [ iIndiceIndexOffset +  4 ] = iVertIndexOffset +  3;		pMesh->pIndices [ iIndiceIndexOffset +  5 ] = iVertIndexOffset +  0;
		iIndiceIndexOffset+=6;
		iVertIndexOffset+=4;
	}
	if ( bU )
	{
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  0, fWidth1, fHeight1, fDepth2,  0.0f, -1.0f,  0.0f, dwColor, 0.00f, 1.00f );	// bottom
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  1, fWidth1, fHeight1, fDepth1,	 0.0f, -1.0f,  0.0f, dwColor, 0.00f, 0.00f );
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  2, fWidth2, fHeight1, fDepth1,	 0.0f, -1.0f,  0.0f, dwColor, 1.00f, 0.00f );
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  3, fWidth2, fHeight1, fDepth2,	 0.0f, -1.0f,  0.0f, dwColor, 1.00f, 1.00f );
		pMesh->pIndices [ iIndiceIndexOffset +  0 ] = iVertIndexOffset +  0;		pMesh->pIndices [ iIndiceIndexOffset +  1 ] = iVertIndexOffset +  1;		pMesh->pIndices [ iIndiceIndexOffset +  2 ] = iVertIndexOffset +  2;
		pMesh->pIndices [ iIndiceIndexOffset +  3 ] = iVertIndexOffset +  2;		pMesh->pIndices [ iIndiceIndexOffset +  4 ] = iVertIndexOffset +  3;		pMesh->pIndices [ iIndiceIndexOffset +  5 ] = iVertIndexOffset +  0;
		iIndiceIndexOffset+=6;
		iVertIndexOffset+=4;
	}
	if ( bR )
	{
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  0, fWidth2, fHeight2, fDepth1,	 1.0f,  0.0f,  0.0f, dwColor, 0.00f, 0.00f );	// right
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  1, fWidth2, fHeight2, fDepth2,	 1.0f,  0.0f,  0.0f, dwColor, 1.00f, 0.00f );
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  2, fWidth2, fHeight1, fDepth2,	 1.0f,  0.0f,  0.0f, dwColor, 1.00f, 1.00f );
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  3, fWidth2, fHeight1, fDepth1,	 1.0f,  0.0f,  0.0f, dwColor, 0.00f, 1.00f );
		pMesh->pIndices [ iIndiceIndexOffset +  0 ] = iVertIndexOffset +  0;		pMesh->pIndices [ iIndiceIndexOffset +  1 ] = iVertIndexOffset +  1;		pMesh->pIndices [ iIndiceIndexOffset +  2 ] = iVertIndexOffset +  2;
		pMesh->pIndices [ iIndiceIndexOffset +  3 ] = iVertIndexOffset +  2;		pMesh->pIndices [ iIndiceIndexOffset +  4 ] = iVertIndexOffset +  3;		pMesh->pIndices [ iIndiceIndexOffset +  5 ] = iVertIndexOffset +  0;
		iIndiceIndexOffset+=6;
		iVertIndexOffset+=4;
	}
	if ( bL )
	{
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  0, fWidth1, fHeight2, fDepth1,	-1.0f,  0.0f,  0.0f, dwColor, 1.00f, 0.00f );	// left
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  1, fWidth1, fHeight1, fDepth1,	-1.0f,  0.0f,  0.0f, dwColor, 1.00f, 1.00f );
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  2, fWidth1, fHeight1, fDepth2,	-1.0f,  0.0f,  0.0f, dwColor, 0.00f, 1.00f );
		SetupStandardVertexDec ( pMesh,	pMesh->pVertexData, iVertIndexOffset +  3, fWidth1, fHeight2, fDepth2,	-1.0f,  0.0f,  0.0f, dwColor, 0.00f, 0.00f );
		pMesh->pIndices [ iIndiceIndexOffset +  0 ] = iVertIndexOffset +  0;		pMesh->pIndices [ iIndiceIndexOffset +  1 ] = iVertIndexOffset +  1;		pMesh->pIndices [ iIndiceIndexOffset +  2 ] = iVertIndexOffset +  2;
		pMesh->pIndices [ iIndiceIndexOffset +  3 ] = iVertIndexOffset +  2;		pMesh->pIndices [ iIndiceIndexOffset +  4 ] = iVertIndexOffset +  3;		pMesh->pIndices [ iIndiceIndexOffset +  5 ] = iVertIndexOffset +  0;
		iIndiceIndexOffset+=6;
		iVertIndexOffset+=4;
	}
	
	// update counters
	*pdwVertexOffset = iVertIndexOffset;
	*pdwIndexOffset = iIndiceIndexOffset;
}

DARKSDK_DLL void ReduceMeshPolygons ( sMesh* pOriginalMesh, int iBlockMode, int iGridDimension, int iGX, int iGY, int iGZ )
{
	// limits of this process
	if ( iGX>50 || iGY>50 || iGZ>50 )
		return;

	// minimum values
	if ( iGX<3 ) iGX=3;
	if ( iGY<1 ) iGY=1;
	if ( iGZ<3 ) iGZ=3;
	
	int iScanZ = 0;

	// the collision area is a block of wood, that we are going to chip away 
	unsigned char bBlock[50][50][50];
	for ( iScanZ=0; iScanZ<iGZ; iScanZ++ )
		for ( int iScanY=0; iScanY<iGY; iScanY++ )
			for ( int iScanX=0; iScanX<iGX; iScanX++ )
				bBlock[iScanX][iScanY][iScanZ]=255;

	// get mesh verttype and size
	sOffsetMap offsetMap;
 	GetFVFOffsetMap ( pOriginalMesh, &offsetMap );

	// create a work mesh
	sMesh* pMesh = new sMesh;
	MakeLocalMeshFromOtherLocalMesh ( pMesh, pOriginalMesh );
	ConvertLocalMeshToVertsOnly ( pMesh );
	DWORD dwNumberOfVertices=pMesh->dwVertexCount;

	// boundary of mesh
	CalculateMeshBounds ( pMesh );
	D3DXVECTOR3 vecMin = pMesh->Collision.vecMin;
	D3DXVECTOR3 vecMax = pMesh->Collision.vecMax;

	// create grid system around mesh bounds
	float fGapX = (vecMax.x-vecMin.x)/iGX;
	float fGapY = (vecMax.y-vecMin.y)/iGY;
	float fGapZ = (vecMax.z-vecMin.z)/iGZ;

	// gap must be AT least one for the step through to work (objects so small would have a single pass anyway)
	if ( fGapX<1.0f ) fGapX=1.0f;
	if ( fGapY<1.0f ) fGapY=1.0f;
	if ( fGapZ<1.0f ) fGapZ=1.0f;

	// grid work varriables
	float fGapThirdX = fGapX/3.0f;
	float fGapThirdY = fGapY/3.0f;
	float fGapThirdZ = fGapZ/3.0f;
	float fGapHalfX = fGapX/2.0f;
	float fGapHalfY = fGapY/2.0f;
	float fGapHalfZ = fGapZ/2.0f;

	// scan mesh and chizel from all sides
	for ( int iSide=0; iSide<2; iSide++ )
	{
		// chizel values
		int iChZ, iChAdd;
		float fChZHalf, fChZDir, fGridForDist;
		if ( iSide==0 )
		{ 
			iChZ=0; fChZHalf=-fGapHalfZ; fChZDir=1.0f; iChAdd=1; fGridForDist=fGapZ;
		}
		if ( iSide==1 )
		{ 
			iChZ=iGZ-1; fChZHalf=fGapZ+fGapHalfZ; fChZDir=-1.0f; iChAdd=-1; fGridForDist=fGapZ;
		}

		// begin chizelling from a side
		iScanZ=iChZ;
		for ( int iScanY=0; iScanY<iGY; iScanY++ )
		{
			for ( int iScanX=0; iScanX<iGX; iScanX++ )
			{
				// originate a ray
				D3DXVECTOR3 vecRay = D3DXVECTOR3 ( vecMin.x+(iScanX*fGapX)+fGapHalfX, vecMin.y+(iScanY*fGapY)+fGapHalfY, vecMin.z+(iScanZ*fGapZ)+fChZHalf );
				D3DXVECTOR3 vecRayDir = D3DXVECTOR3 ( 0, 0, fChZDir );

				// check each poly
				int iChizelDistance=iGZ-1;

				// get average of point tests on grid square
				int iDistCount=0;
				float fDistAverage=0.0f;
				for ( float fBitX=-fGapThirdX; fBitX<=fGapThirdX; fBitX+=fGapThirdX )
				{
					for ( float fBitY=-fGapThirdY; fBitY<=fGapThirdY; fBitY+=fGapThirdY )
					{
						// sub-ray for clarity
						D3DXVECTOR3 vecRayBit = vecRay + D3DXVECTOR3(fBitX,fBitY,0);

						// go through each polygon and find CLOSEST polygon hit
						bool bPolygonHitDetected=false;
						float fU, fV, fDistance, fBestCollision=99999.0f;
						for ( DWORD dwCurrentVertex = 0; dwCurrentVertex < dwNumberOfVertices; dwCurrentVertex+=3 )
						{
							// polygon
							D3DXVECTOR3* pVec0 = (D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (dwCurrentVertex+0) ) );
							D3DXVECTOR3* pVec1 = (D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (dwCurrentVertex+1) ) );
							D3DXVECTOR3* pVec2 = (D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (dwCurrentVertex+2) ) );
							if ( D3DXIntersectTri ( pVec0, pVec1, pVec2, &vecRayBit, &vecRayDir, &fU, &fV, &fDistance )==TRUE )
							{
								if ( fDistance<fBestCollision)
								{
									bPolygonHitDetected=true;
									fBestCollision=fDistance;
								}
							}
						}
						if ( bPolygonHitDetected )
						{
							if ( fBestCollision > fGapHalfZ+0.1f )
							{
								fDistAverage += fBestCollision-fGapHalfZ;
								iDistCount++;
							}
							else
							{
								// no chizelling
								iChizelDistance=-1;
							}
						}
					}
				}

				// test ray against polygon
				if ( iDistCount > 0 )
				{
					// average the distance for best match
					float fDistance = (fDistAverage / iDistCount)-0.5f;

					// grid lock it to the nearest line
					int iLine = (int)(fDistance/fGridForDist);

					// somewhere in between
					if ( iLine < iChizelDistance ) 
						iChizelDistance = iLine;
				}

				// ray struck a surface, chizzel away path
				int iChizelCount=0;
				int iChizel = iScanZ;
				while ( iChizelCount <= iChizelDistance )
				{
					bBlock[iScanX][iScanY][iChizel] = 253;
					iChizelCount++;
					iChizel+=iChAdd;
				}
			}
		}
	}

	// free usages
	SAFE_DELETE ( pMesh );

	// create logical boxes from blockdata
	struct logicalboxtype
	{
		int iX1, iY1, iZ1;
		int iX2, iY2, iZ2;
		bool bLeft, bRight, bUp, bDown, bBack, bFore;
	};
	DWORD dwLogicalBoxesMax=32;
	DWORD dwLogicalBoxesCount=0;
	logicalboxtype* pLogicalBoxes = new logicalboxtype[dwLogicalBoxesMax];

	// go through all blocks
	for ( iScanZ=0; iScanZ<iGZ; iScanZ++ )
	{
		for ( int iScanY=0; iScanY<iGY; iScanY++ )
		{
			for ( int iScanX=0; iScanX<iGX; iScanX++ )
			{
				if ( bBlock[iScanX][iScanY][iScanZ]==255 )
				{
					// block exists - expand a logic-box here
					logicalboxtype sBox;
					sBox.iX1=iScanX;
					sBox.iY1=iScanY;
					sBox.iZ1=iScanZ;
					sBox.iX2=iScanX;
					sBox.iY2=iScanY;
					sBox.iZ2=iScanZ;

					// expand in all areas where blocks permit
					bool bExpand = true;
					while ( bExpand )
					{
						// count expansions
						int iExpandCount=0;

						// try left expand
						int iX=sBox.iX1-1;
						bool bCanExpand=true;
						if ( iX==-1 ) bCanExpand=false;
						if ( bCanExpand )
							for ( int iY=sBox.iY1; iY<=sBox.iY2; iY++ )
								for ( int iZ=sBox.iZ1; iZ<=sBox.iZ2; iZ++ )
									if ( bBlock[iX][iY][iZ]<255 )
										bCanExpand=false;
						if ( bCanExpand ) { sBox.iX1=iX; iExpandCount++; }

						// try right expand
						iX=sBox.iX2+1;
						bCanExpand=true;
						if ( iX==iGX ) bCanExpand=false;
						if ( bCanExpand )
							for ( int iY=sBox.iY1; iY<=sBox.iY2; iY++ )
								for ( int iZ=sBox.iZ1; iZ<=sBox.iZ2; iZ++ )
									if ( bBlock[iX][iY][iZ]<255 )
										bCanExpand=false;
						if ( bCanExpand ) { sBox.iX2=iX; iExpandCount++; }

						// try up expand
						int iY=sBox.iY1-1;
						bCanExpand=true;
						if ( iY==-1 ) bCanExpand=false;
						if ( bCanExpand )
							for ( int iX=sBox.iX1; iX<=sBox.iX2; iX++ )
								for ( int iZ=sBox.iZ1; iZ<=sBox.iZ2; iZ++ )
									if ( bBlock[iX][iY][iZ]<255 )
										bCanExpand=false;
						if ( bCanExpand ) { sBox.iY1=iY; iExpandCount++; }

						// try down expand
						iY=sBox.iY2+1;
						bCanExpand=true;
						if ( iY==iGY ) bCanExpand=false;
						if ( bCanExpand )
							for ( int iX=sBox.iX1; iX<=sBox.iX2; iX++ )
								for ( int iZ=sBox.iZ1; iZ<=sBox.iZ2; iZ++ )
									if ( bBlock[iX][iY][iZ]<255 )
										bCanExpand=false;
						if ( bCanExpand ) { sBox.iY2=iY; iExpandCount++; }

						// try back expand
						int iZ=sBox.iZ1-1;
						bCanExpand=true;
						if ( iZ==-1 ) bCanExpand=false;
						if ( bCanExpand )
							for ( int iX=sBox.iX1; iX<=sBox.iX2; iX++ )
								for ( int iY=sBox.iY1; iY<=sBox.iY2; iY++ )
									if ( bBlock[iX][iY][iZ]<255 )
										bCanExpand=false;
						if ( bCanExpand ) { sBox.iZ1=iZ; iExpandCount++; }

						// try fore expand
						iZ=sBox.iZ2+1;
						bCanExpand=true;
						if ( iZ==iGZ ) bCanExpand=false;
						if ( bCanExpand )
							for ( int iX=sBox.iX1; iX<=sBox.iX2; iX++ )
								for ( int iY=sBox.iY1; iY<=sBox.iY2; iY++ )
									if ( bBlock[iX][iY][iZ]<255 )
										bCanExpand=false;
						if ( bCanExpand ) { sBox.iZ2=iZ; iExpandCount++; }

						// if cannot expand anywhere, exit
						if ( iExpandCount==0 ) bExpand=false;
					}
					
					int iX = 0;

					// once expanded as far as we can, delete all blocks within it
					for ( iX=sBox.iX1; iX<=sBox.iX2; iX++ )
						for ( int iY=sBox.iY1; iY<=sBox.iY2; iY++ )
							for ( int iZ=sBox.iZ1; iZ<=sBox.iZ2; iZ++ )
								bBlock[iX][iY][iZ] = 254;

					// see if logicbox sides completely hidden by more blocks (for HSR)
					iX=sBox.iX1-1;
					sBox.bLeft=false;
					if ( iX==-1 ) sBox.bLeft=true;
					if ( !sBox.bLeft )
						for ( int iY=sBox.iY1; iY<=sBox.iY2; iY++ )
							for ( int iZ=sBox.iZ1; iZ<=sBox.iZ2; iZ++ )
								if ( bBlock[iX][iY][iZ]==253 )
									sBox.bLeft=true;

					// check right
					iX=sBox.iX2+1;
					sBox.bRight=false;
					if ( iX==iGX ) sBox.bRight=true;
					if ( !sBox.bRight )
						for ( int iY=sBox.iY1; iY<=sBox.iY2; iY++ )
							for ( int iZ=sBox.iZ1; iZ<=sBox.iZ2; iZ++ )
								if ( bBlock[iX][iY][iZ]==253 )
									sBox.bRight=true;

					// check up
					int iY=sBox.iY1-1;
					sBox.bUp=false;
					if ( iY==-1 ) sBox.bUp=true;
					if ( !sBox.bUp )
						for ( iX=sBox.iX1; iX<=sBox.iX2; iX++ )
							for ( int iZ=sBox.iZ1; iZ<=sBox.iZ2; iZ++ )
								if ( bBlock[iX][iY][iZ]==253 )
									sBox.bUp=true;

					// check down
					iY=sBox.iY2+1;
					sBox.bDown=false;
					if ( iY==iGY ) sBox.bDown=true;
					if ( !sBox.bDown )
						for ( iX=sBox.iX1; iX<=sBox.iX2; iX++ )
							for ( int iZ=sBox.iZ1; iZ<=sBox.iZ2; iZ++ )
								if ( bBlock[iX][iY][iZ]==253 )
									sBox.bDown=true;

					// check back
					int iZ=sBox.iZ1-1;
					sBox.bBack=false;
					if ( iZ==-1 ) sBox.bBack=true;
					if ( !sBox.bBack )
						for ( iX=sBox.iX1; iX<=sBox.iX2; iX++ )
							for ( int iY=sBox.iY1; iY<=sBox.iY2; iY++ )
								if ( bBlock[iX][iY][iZ]==253 )
									sBox.bBack=true;

					// check fore
					iZ=sBox.iZ2+1;
					sBox.bFore=false;
					if ( iZ==iGZ ) sBox.bFore=true;
					if ( !sBox.bFore )
						for ( iX=sBox.iX1; iX<=sBox.iX2; iX++ )
							for ( int iY=sBox.iY1; iY<=sBox.iY2; iY++ )
								if ( bBlock[iX][iY][iZ]==253 )
									sBox.bFore=true;

					// expand list if too small
					if ( dwLogicalBoxesCount+1>=dwLogicalBoxesMax )
					{
						DWORD dwNewBoxMax = dwLogicalBoxesMax*2;
						logicalboxtype* pNewLogicalBoxes = new logicalboxtype[dwNewBoxMax];
						memcpy ( pNewLogicalBoxes, pLogicalBoxes, dwLogicalBoxesMax * sizeof(logicalboxtype) );
						SAFE_DELETE(pLogicalBoxes);
						pLogicalBoxes=pNewLogicalBoxes;
						dwLogicalBoxesMax=dwNewBoxMax;
					}

					// and add to logic box array
					pLogicalBoxes [ dwLogicalBoxesCount ] = sBox;
					dwLogicalBoxesCount++;
				}
			}
		}
	}

	// work out actual usage of new boxes
	DWORD dwVertexCount=0, dwIndexCount=0;
	for ( DWORD dwCurrentBox=0; dwCurrentBox<dwLogicalBoxesCount; dwCurrentBox++ )
	{
		// Box dimensions
		bool bL = pLogicalBoxes [ dwCurrentBox ].bLeft;
		bool bR = pLogicalBoxes [ dwCurrentBox ].bRight;
		bool bU = pLogicalBoxes [ dwCurrentBox ].bUp;
		bool bD = pLogicalBoxes [ dwCurrentBox ].bDown;
		bool bB = pLogicalBoxes [ dwCurrentBox ].bBack;
		bool bF = pLogicalBoxes [ dwCurrentBox ].bFore;
		if ( bL ) { dwVertexCount+=4; dwIndexCount+=6; }
		if ( bR ) { dwVertexCount+=4; dwIndexCount+=6; }
		if ( bU ) { dwVertexCount+=4; dwIndexCount+=6; }
		if ( bD ) { dwVertexCount+=4; dwIndexCount+=6; }
		if ( bB ) { dwVertexCount+=4; dwIndexCount+=6; }
		if ( bF ) { dwVertexCount+=4; dwIndexCount+=6; }
	}

	// create new mesh
	DWORD dwVertexOffset=0, dwIndexOffset=0;
	if ( SetupMeshData ( pOriginalMesh, dwVertexCount, dwIndexCount ) )
	{
		// default values
		DWORD dwColor = D3DXCOLOR(1,1,1,1);

		// fill new mesh with new meshdata
		for ( DWORD dwCurrentBox=0; dwCurrentBox<dwLogicalBoxesCount; dwCurrentBox++ )
		{
			// Box dimensions
			int iScanX = pLogicalBoxes [ dwCurrentBox ].iX1;
			int iScanY = pLogicalBoxes [ dwCurrentBox ].iY1;
			int iScanZ = pLogicalBoxes [ dwCurrentBox ].iZ1;
			int iScanWX = 1+(pLogicalBoxes [ dwCurrentBox ].iX2-iScanX);
			int iScanWY = 1+(pLogicalBoxes [ dwCurrentBox ].iY2-iScanY);
			int iScanWZ = 1+(pLogicalBoxes [ dwCurrentBox ].iZ2-iScanZ);
			bool bL = pLogicalBoxes [ dwCurrentBox ].bLeft;
			bool bR = pLogicalBoxes [ dwCurrentBox ].bRight;
			bool bU = pLogicalBoxes [ dwCurrentBox ].bUp;
			bool bD = pLogicalBoxes [ dwCurrentBox ].bDown;
			bool bB = pLogicalBoxes [ dwCurrentBox ].bBack;
			bool bF = pLogicalBoxes [ dwCurrentBox ].bFore;

			// calculate box size
			D3DXVECTOR3 vecBox = D3DXVECTOR3 ( vecMin.x+(iScanX*fGapX), vecMin.y+(iScanY*fGapY), vecMin.z+(iScanZ*fGapZ) );
			float fWidth1 = vecBox.x;
			float fHeight1 = vecBox.y;
			float fDepth1 = vecBox.z;
			float fWidth2 = fWidth1+(fGapX*iScanWX);
			float fHeight2 = fHeight1+(fGapY*iScanWY);
			float fDepth2 = fDepth1+(fGapZ*iScanWZ);
			
			// create box
			AddBoxToMesh ( pOriginalMesh, &dwVertexOffset, &dwIndexOffset, fWidth1, fHeight1, fDepth1, fWidth2, fHeight2, fDepth2, dwColor, bL, bR, bU, bD, bB, bF );
		}

		// setup mesh values
		pOriginalMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
		pOriginalMesh->iDrawVertexCount = pOriginalMesh->dwVertexCount;
		pOriginalMesh->iDrawPrimitives  = pOriginalMesh->dwIndexCount  / 3;
	}

	// free usages
	SAFE_DELETE(pLogicalBoxes);

	// flag mesh for a VB update
	pOriginalMesh->bMeshHasBeenReplaced=true;
}

DARKSDK_DLL int CheckIfMeshSolid ( sMesh* pMesh, int iGX, int iGY, int iGZ )
{
	// limits of this process
	if ( iGX>50 || iGY>50 || iGZ>50 )
		return false;

	// minimum values
	if ( iGX<3 ) iGX=3;
	if ( iGY<3 ) iGY=3;
	if ( iGZ<3 ) iGZ=3;

	// cound cast hits and overall
	int iCountCastHits=0;
	int iOverallCasts=0;

	// get mesh verttype and size
	sOffsetMap offsetMap;
 	GetFVFOffsetMap ( pMesh, &offsetMap );

	// boundary of mesh
	D3DXVECTOR3 vecMin = pMesh->Collision.vecMin;
	D3DXVECTOR3 vecMax = pMesh->Collision.vecMax;

	// determine best side to scan from
	D3DXVECTOR3 vecSize = vecMax - vecMin;
	int iSideScan=0;
	float fSmallest=vecSize.x;
	if ( vecSize.y<fSmallest ) { fSmallest=vecSize.y; iSideScan=1; }
	if ( vecSize.z<fSmallest ) { fSmallest=vecSize.z; iSideScan=2; }

	// create grid system around mesh bounds
	float fGapX = (vecSize.x)/iGX;
	float fGapY = (vecSize.y)/iGY;
	float fGapZ = (vecSize.z)/iGZ;
	float fGapEdgeX = fGapX;
	float fGapEdgeY = fGapY;
	float fGapEdgeZ = fGapZ;

	// setup ray direction and plane
	D3DXVECTOR3 vecRayDir;
	if ( iSideScan==0 ) { vecRayDir = D3DXVECTOR3 ( 1.0f, 0, 0 ); fGapX=vecSize.x*1.01f; fGapEdgeX=0.0f; } 
	if ( iSideScan==1 ) { vecRayDir = D3DXVECTOR3 ( 0, 1.0f, 0 ); fGapY=vecSize.y*1.01f; fGapEdgeY=0.0f; } 
	if ( iSideScan==2 ) { vecRayDir = D3DXVECTOR3 ( 0, 0, 1.0f ); fGapZ=vecSize.z*1.01f; fGapEdgeZ=0.0f; } 

	// gap must be AT least one for the step through to work (objects so small would have a single pass anyway)
	if ( fGapX<1.0f ) fGapX=1.0f;
	if ( fGapY<1.0f ) fGapY=1.0f;
	if ( fGapZ<1.0f ) fGapZ=1.0f;

	// scan a grid
	for ( float fX=vecMin.x+fGapEdgeX; fX<=vecMax.x-fGapEdgeX; fX+=fGapX )
	{
		for ( float fY=vecMin.y+fGapEdgeY; fY<=vecMax.y-fGapEdgeY; fY+=fGapY )
		{
			for ( float fZ=vecMin.z+fGapEdgeZ; fZ<=vecMax.z-fGapEdgeZ; fZ+=fGapZ )
			{
				// originate a ray
				float fRayDist=0.0f;
				D3DXVECTOR3 vecRayStart;
				if ( iSideScan==0 ) { vecRayStart = D3DXVECTOR3 ( vecMin.x+(float)fabs(vecSize.x*2)*-1.0f, fY, fZ ); fRayDist=(float)fabs(vecMin.x-vecRayStart.x); }
				if ( iSideScan==1 ) { vecRayStart = D3DXVECTOR3 ( fX, vecMin.y+(float)fabs(vecSize.y*2)*-1.0f, fZ ); fRayDist=(float)fabs(vecMin.y-vecRayStart.y); }
				if ( iSideScan==2 ) { vecRayStart = D3DXVECTOR3 ( fX, fY, vecMin.z+(float)fabs(vecSize.z*2)*-1.0f ); fRayDist=(float)fabs(vecMin.z-vecRayStart.z); }

				// go through each polygon and find CLOSEST polygon hit
				bool bPolygonHitDetected=false;
				float fU, fV, fDistance, fBestCollision=99999.0f;

				// with indexed data
				if ( pMesh->pIndices )
				{
					DWORD dwNumberOfIndices = pMesh->dwIndexCount;
					for ( DWORD dwCurrentIndex = 0; dwCurrentIndex < dwNumberOfIndices; dwCurrentIndex+=3 )
					{
						// use indice data to find correct verts for poly
						DWORD dwV1 = pMesh->pIndices [ dwCurrentIndex+0 ];
						DWORD dwV2 = pMesh->pIndices [ dwCurrentIndex+1 ];
						DWORD dwV3 = pMesh->pIndices [ dwCurrentIndex+2 ];

						// polygon
						D3DXVECTOR3* pVec0 = (D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * dwV1 ) );
						D3DXVECTOR3* pVec1 = (D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * dwV2 ) );
						D3DXVECTOR3* pVec2 = (D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * dwV3 ) );
						if ( D3DXIntersectTri ( pVec0, pVec1, pVec2, &vecRayStart, &vecRayDir, &fU, &fV, &fDistance )==TRUE )
						{
							// I hit one, make sire it is right on the edge of the boundbox (or else it has a gap as though from a curve that we can see through)
							if ( fDistance<fRayDist+12.5f )
							{
								// can leave now
								bPolygonHitDetected=true;
								break;
							}
						}
					}
				}
				else
				{
					// pure vertex data
					DWORD dwNumberOfVertices = pMesh->dwVertexCount;
					for ( DWORD dwCurrentVertex = 0; dwCurrentVertex < dwNumberOfVertices; dwCurrentVertex+=3 )
					{
						// polygon
						D3DXVECTOR3* pVec0 = (D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (dwCurrentVertex+0) ) );
						D3DXVECTOR3* pVec1 = (D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (dwCurrentVertex+1) ) );
						D3DXVECTOR3* pVec2 = (D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * (dwCurrentVertex+2) ) );
						if ( D3DXIntersectTri ( pVec0, pVec1, pVec2, &vecRayStart, &vecRayDir, &fU, &fV, &fDistance )==TRUE )
						{
							// I hit one, make sire it is right on the edge of the boundbox (or else it has a gap as though from a curve that we can see through)
							if ( fDistance<fRayDist+12.5f )
							{
								// can leave now
								bPolygonHitDetected=true;
								break;
							}
						}
					}
				}
				if ( bPolygonHitDetected )
				{
					// hole found, can leave now
					iCountCastHits++;
				}
				iOverallCasts++;
			}
		}
	}

	// check if solid, partially solid or not even there
	int iMarginForError = iOverallCasts/20;
	if ( iCountCastHits>iOverallCasts-iMarginForError )
	{
		// entirely solid (no holes - solid wall)
		return 2;
	}
	else
	{
		if ( iCountCastHits>0 )
		{
			// partially solid (has hole in it)
			return 1;
		}
		else
		{
			// not solid
			return 0;
		}
	}
}

DARKSDK_DLL bool CheckIfMeshBlocking ( sMesh* pMesh, float X1, float Y1, float Z1, float X2, float Y2, float Z2 )
{
	// calculate bounds of mesh
	float boundboxX1 = pMesh->Collision.vecMin.x;
	float boundboxY1 = pMesh->Collision.vecMin.y;
	float boundboxZ1 = pMesh->Collision.vecMin.z;
	float boundboxX2 = pMesh->Collision.vecMax.x;
	float boundboxY2 = pMesh->Collision.vecMax.y;
	float boundboxZ2 = pMesh->Collision.vecMax.z;

	// ensure mesh encountering plane being detected
	if ( X1 >= boundboxX1 && X2 <= boundboxX2 )
	{
        if ( Y1 >= boundboxY1 && Y2 <= boundboxY2 )
		{
           if ( Z1 >= boundboxZ1 && Z2 <= boundboxZ2 )
		   {
				if ( CheckIfMeshSolid ( pMesh, 10, 10, 10 )>0 )
				{
					// blocking
					return true;
				}
		   }
		}
	}
	
	// not blocking
	return false;
}


// Mesh Vertex Data Functions

DARKSDK_DLL void SetPositionData ( sMesh* pMesh, int iCurrentVertex, float fX, float fY, float fZ )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// make sure we have data in the vertices
	if ( offsetMap.dwZ>0 )
	{
		// set component
		*( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) ) = fX;
		*( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iCurrentVertex ) ) = fY;
		*( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iCurrentVertex ) ) = fZ;
	}

	// flag mesh for a VB update
	pMesh->bVBRefreshRequired=true;
	g_vRefreshMeshList.push_back ( pMesh );
}

DARKSDK_DLL void SetNormalsData ( sMesh* pMesh, int iCurrentVertex, float fNX, float fNY, float fNZ )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// make sure we have data in the vertices
	if ( pMesh->dwFVF & D3DFVF_NORMAL )
	{
		// set normals vector component
		*( ( float* ) pMesh->pVertexData + offsetMap.dwNX + ( offsetMap.dwSize * iCurrentVertex ) ) = fNX;
		*( ( float* ) pMesh->pVertexData + offsetMap.dwNY + ( offsetMap.dwSize * iCurrentVertex ) ) = fNY;
		*( ( float* ) pMesh->pVertexData + offsetMap.dwNZ + ( offsetMap.dwSize * iCurrentVertex ) ) = fNZ;
	}

	// flag mesh for a VB update
	pMesh->bVBRefreshRequired=true;
	g_vRefreshMeshList.push_back ( pMesh );
}

DARKSDK_DLL void SetDiffuseData ( sMesh* pMesh, int iCurrentVertex, DWORD dwDiffuse )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// make sure we have data in the vertices
	if ( pMesh->dwFVF & D3DFVF_DIFFUSE )
	{
		// set component
		*( ( DWORD* ) pMesh->pVertexData + offsetMap.dwDiffuse + ( offsetMap.dwSize * iCurrentVertex ) ) = dwDiffuse;
	}

	// flag mesh for a VB update
	pMesh->bVBRefreshRequired=true;
	g_vRefreshMeshList.push_back ( pMesh );
}

DARKSDK_DLL void SetUVData ( sMesh* pMesh, int iCurrentVertex, float fU, float fV )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// make sure we have data in the vertices
	if ( pMesh->dwFVF & D3DFVF_TEX1 )
	{
		// set single UV vertex component
		*( ( float* ) pMesh->pVertexData + offsetMap.dwTU[0] + ( offsetMap.dwSize * iCurrentVertex ) ) = fU;
		*( ( float* ) pMesh->pVertexData + offsetMap.dwTV[0] + ( offsetMap.dwSize * iCurrentVertex ) ) = fV;
	}

	// flag mesh for a VB update
	pMesh->bVBRefreshRequired=true;
	g_vRefreshMeshList.push_back ( pMesh );
}

DARKSDK_DLL bool AddMeshToData ( sMesh* pFinalMesh, sMesh* pMeshToAdd )
{
	// ensure meshes valid
	SAFE_MEMORY ( pFinalMesh );
	SAFE_MEMORY ( pMeshToAdd );

	// convert addmesh to standard
	sMesh* pOriginalMesh = new sMesh;
	MakeLocalMeshFromOtherLocalMesh ( pOriginalMesh, pFinalMesh );

	// convert addmesh to standard
	sMesh* pStandardMesh = new sMesh;
	MakeLocalMeshFromOtherLocalMesh ( pStandardMesh, pMeshToAdd );
	ConvertLocalMeshToFVF ( pStandardMesh, pOriginalMesh->dwFVF );
	
	// make vertex and index buffers for final
	DWORD dwTotalVertices = pOriginalMesh->dwVertexCount + pStandardMesh->dwVertexCount;
	DWORD dwTotalIndices = pOriginalMesh->dwIndexCount + pStandardMesh->dwIndexCount;

	// if creation successful, continue
	if ( SetupMeshFVFData ( pFinalMesh, pOriginalMesh->dwFVF, dwTotalVertices, dwTotalIndices ) )
	{
		// copy over original to final
		memcpy ( pFinalMesh->pVertexData, pOriginalMesh->pVertexData, pOriginalMesh->dwVertexCount * pOriginalMesh->dwFVFSize );
		memcpy ( pFinalMesh->pIndices, pOriginalMesh->pIndices, pOriginalMesh->dwIndexCount * sizeof(WORD) );

		// new vertex data in index list (word=65535max)
		WORD dwVertexStart = (WORD)pOriginalMesh->dwVertexCount;

		// copy over standard to final
		BYTE* pDestVertexData = (BYTE*)pFinalMesh->pVertexData + ( dwVertexStart * pOriginalMesh->dwFVFSize );
		BYTE* pDestIndexData = (BYTE*)pFinalMesh->pIndices + ( pOriginalMesh->dwIndexCount * sizeof(WORD) );
		memcpy ( pDestVertexData, pStandardMesh->pVertexData, pStandardMesh->dwVertexCount * pStandardMesh->dwFVFSize );
		memcpy ( pDestIndexData, pStandardMesh->pIndices, pStandardMesh->dwIndexCount * sizeof(WORD) );

		// increment index values to allign to vertex entries
		WORD* pIndexArray = (WORD*)pDestIndexData;
		for ( DWORD i=0; i<pStandardMesh->dwIndexCount; i++ )
			pIndexArray[i] += dwVertexStart;

		// update values of the mesh
		pFinalMesh->iDrawVertexCount = dwTotalVertices;
		pFinalMesh->iDrawPrimitives  = dwTotalIndices/3;

		// flag mesh for a VB replacement
		pFinalMesh->bMeshHasBeenReplaced=true;
	}

	// free usages
	SAFE_DELETE(pOriginalMesh);
	SAFE_DELETE(pStandardMesh);

	// success
	return true;
}

DARKSDK_DLL bool DeleteMeshFromData ( sMesh* pMesh, int iVertex1, int iVertex2, int iIndex1, int iIndex2 )
{
	// ensure mesh valid
	SAFE_MEMORY ( pMesh );

	// check ranges
  	if ( iVertex2<iVertex1 ) return false;
	if ( iIndex2<iIndex1 ) return false;
	if ( iVertex2<0 ) return false;
	if ( iIndex2<0 ) return false;
	if ( iVertex2>(int)pMesh->dwVertexCount ) return false;
	if ( iIndex2>(int)pMesh->dwIndexCount ) return false;

	// calculate new vertex and index arrays
	DWORD dwNewVertexCount = pMesh->dwVertexCount - (iVertex2-iVertex1);
	DWORD dwNewIndexCount = pMesh->dwIndexCount - (iIndex2-iIndex1);

	// create new arrays
	BYTE* pNewVertexData = new BYTE [ dwNewVertexCount * pMesh->dwFVFSize ];
	BYTE* pNewIndexData = new BYTE [ dwNewIndexCount * sizeof(WORD) ];

	// copy 'before' data
	DWORD dwVBeforeCount = iVertex1;
	DWORD dwIBeforeCount = iIndex1;
	if ( dwVBeforeCount > 0 ) memcpy ( pNewVertexData, pMesh->pVertexData, dwVBeforeCount * pMesh->dwFVFSize );
	if ( dwIBeforeCount > 0 ) memcpy ( pNewIndexData, pMesh->pIndices, dwIBeforeCount * sizeof(WORD) );

	// copy 'after' data
	DWORD dwVAfterCount = pMesh->dwVertexCount - iVertex2;
	DWORD dwIAfterCount = pMesh->dwIndexCount - iIndex2;
	if ( dwVAfterCount > 0 )
	{
		BYTE* pVertStart = (BYTE*)pNewVertexData + ( dwVBeforeCount * pMesh->dwFVFSize );
		BYTE* pVertEnd = (BYTE*)pMesh->pVertexData + ( iVertex2 * pMesh->dwFVFSize );
		memcpy ( pVertStart, pVertEnd, dwVAfterCount * pMesh->dwFVFSize );
	}
	if ( dwIAfterCount > 0 )
	{
		BYTE* pIndexStart = (BYTE*)pNewIndexData + ( dwIBeforeCount * sizeof(WORD) );
		BYTE* pIndexEnd = (BYTE*)pMesh->pIndices + ( iIndex2 * sizeof(WORD) );
		memcpy ( pIndexStart, pIndexEnd, dwIAfterCount * sizeof(WORD) );
	}

	// reduce 'indice' data after vertex data shuffle
	WORD wVGap = iVertex2 - iVertex1;
	WORD* pThisIndexData = (WORD*)pNewIndexData;
	for ( DWORD i=0; i<dwNewIndexCount; i++)
		if(pThisIndexData[i]>=iVertex2)
			pThisIndexData[i]=pThisIndexData[i]-wVGap;

	// remove old arraus
	SAFE_DELETE_ARRAY(pMesh->pVertexData);
	SAFE_DELETE_ARRAY(pMesh->pIndices);

	// replace with new arrays
	pMesh->dwVertexCount = dwNewVertexCount;
	pMesh->pVertexData = pNewVertexData;
	pMesh->dwIndexCount = dwNewIndexCount;
	pMesh->pIndices = (WORD*)pNewIndexData;
	pMesh->iDrawVertexCount = dwNewVertexCount;
	pMesh->iDrawPrimitives = dwNewIndexCount/3;

	// flag mesh for a VB replacement
	pMesh->bMeshHasBeenReplaced=true;

	// success
	return true;
}

DARKSDK_DLL int GetVertexCount ( sMesh* pMesh )
{
	return pMesh->dwVertexCount;
}

DARKSDK_DLL int GetIndexCount ( sMesh* pMesh )
{
	return pMesh->dwIndexCount;
}

DARKSDK_DLL float GetDataPositionX ( sMesh* pMesh, int iCurrentVertex )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	if ( offsetMap.dwZ>0 )
		return *( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) );

	return 0.0f;
}

DARKSDK_DLL float GetDataPositionY ( sMesh* pMesh, int iCurrentVertex )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	if ( offsetMap.dwZ>0 )
		return *( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iCurrentVertex ) );

	return 0.0f;
}

DARKSDK_DLL float GetDataPositionZ ( sMesh* pMesh, int iCurrentVertex )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	if ( offsetMap.dwZ>0 )
		return *( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iCurrentVertex ) );

	return 0.0f;
}

DARKSDK_DLL float GetDataNormalsX ( sMesh* pMesh, int iCurrentVertex )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	if ( offsetMap.dwNZ>0 )
		return *( ( float* ) pMesh->pVertexData + offsetMap.dwNX + ( offsetMap.dwSize * iCurrentVertex ) );

	return 0.0f;
}

DARKSDK_DLL float GetDataNormalsY ( sMesh* pMesh, int iCurrentVertex )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	if ( offsetMap.dwNZ>0 )
		return *( ( float* ) pMesh->pVertexData + offsetMap.dwNY + ( offsetMap.dwSize * iCurrentVertex ) );

	return 0.0f;
}

DARKSDK_DLL float GetDataNormalsZ ( sMesh* pMesh, int iCurrentVertex )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	if ( offsetMap.dwNZ>0 )
		return *( ( float* ) pMesh->pVertexData + offsetMap.dwNZ + ( offsetMap.dwSize * iCurrentVertex ) );

	return 0.0f;
}

DARKSDK_DLL DWORD GetDataDiffuse ( sMesh* pMesh, int iCurrentVertex )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	if ( offsetMap.dwDiffuse>0 )
		return *( ( DWORD* ) pMesh->pVertexData + offsetMap.dwDiffuse + ( offsetMap.dwSize * iCurrentVertex ) );

	return 0;
}

DARKSDK_DLL float GetDataU ( sMesh* pMesh, int iCurrentVertex )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	if ( offsetMap.dwTU[0]>0 )
		return *( ( float* ) pMesh->pVertexData + offsetMap.dwTU[0] + ( offsetMap.dwSize * iCurrentVertex ) );

	return 0.0f;
}

DARKSDK_DLL float GetDataV ( sMesh* pMesh, int iCurrentVertex )
{
	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	if ( offsetMap.dwTV[0]>0 )
		return *( ( float* ) pMesh->pVertexData + offsetMap.dwTV[0] + ( offsetMap.dwSize * iCurrentVertex ) );

	return 0.0f;
}

DARKSDK_DLL float GetDataU ( sMesh* pMesh, int iCurrentVertex, int iIndex )
{
	// mike - 050803 - new command to get another set of texture coords

	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	if ( offsetMap.dwTU[iIndex]>0 )
		return *( ( float* ) pMesh->pVertexData + offsetMap.dwTU[iIndex] + ( offsetMap.dwSize * iCurrentVertex ) );

	return 0.0f;
}

DARKSDK_DLL float GetDataV ( sMesh* pMesh, int iCurrentVertex, int iIndex )
{
	// mike - 050803 - new command to get another set of texture coords

	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );
	if ( offsetMap.dwTV[iIndex]>0 )
		return *( ( float* ) pMesh->pVertexData + offsetMap.dwTV[iIndex] + ( offsetMap.dwSize * iCurrentVertex ) );

	return 0.0f;
}

// Mesh Visual Functions

DARKSDK_DLL void Hide ( sMesh* pMesh )
{
	pMesh->bVisible = false;
}

DARKSDK_DLL void Show ( sMesh* pMesh )
{
	pMesh->bVisible = true;
}

DARKSDK_DLL void SetGhost ( sMesh* pMesh, bool bGhost, int iGhostMode )
{
	pMesh->bGhost = bGhost;
	if ( iGhostMode != -1 ) pMesh->iGhostMode = iGhostMode;
}

DARKSDK_DLL void SetTextureMode ( sMesh* pMesh, int iStage, int iMode, int iMipMode )
{
	if ( (DWORD)iStage<pMesh->dwTextureCount )
	{
		sTexture* pTexture = &pMesh->pTextures[iStage];
		if(pTexture)
		{
			pTexture->dwAddressU = iMode;
			pTexture->dwAddressV = iMode;
			pTexture->dwMipState = iMipMode;
		}
	}
}

DARKSDK_DLL void SetTextureMode ( sMesh* pMesh, int iMode, int iMipMode )
{
	SetTextureMode ( pMesh, 0, iMode, iMipMode );
}

// Mesh Texture Functions

DARKSDK_DLL int LoadOrFindTextureAsImage ( LPSTR pTextureName, LPSTR TexturePath, int iDivideTextureSize )
{
	// load texture
	int iImageIndex = 0;

	// does we have a texture name
	if ( strlen(pTextureName)==0 )
		return 0;

	// get filename only
	char pNoPath [ _MAX_PATH ];
	strcpy(pNoPath, pTextureName );
	_strrev(pNoPath);
	for(DWORD n=0; n<strlen(pNoPath); n++)
		if(pNoPath[n]=='\\' || pNoPath[n]=='/' || (unsigned char)(pNoPath[n])<32)
			pNoPath[n]=0;
	_strrev(pNoPath);

	// build a texture path and load it
	char Path [ _MAX_PATH*2 ];
	sprintf ( Path, "%s%s", TexturePath, pNoPath );
	if ( strlen ( Path ) >= _MAX_PATH ) Path[_MAX_PATH]=0;

	// texture load a : default file
	iImageIndex = g_Image_InternalLoadEx ( Path, iDivideTextureSize );
	if ( iImageIndex==0 )
	{
		// texture load b : file as DDS
		char pDDSFile [ _MAX_PATH*2 ];
		strcpy(pDDSFile, pNoPath);
		DWORD dwLenDot = strlen(pDDSFile);
		if ( dwLenDot>4 )
		{
			pDDSFile[dwLenDot-4]=0;
			strcat(pDDSFile, ".dds");
			sprintf ( Path, "%s%s", TexturePath, pDDSFile );
			iImageIndex = g_Image_InternalLoadEx ( Path, iDivideTextureSize );
			if ( iImageIndex==0 )
			{
				// texture load b2 : try as _D.dds
				pDDSFile[dwLenDot-4]=0;
				strcat(pDDSFile, "_D.dds");
				sprintf ( Path, "%s%s", TexturePath, pDDSFile );
				iImageIndex = g_Image_InternalLoadEx ( Path, iDivideTextureSize );
				if ( iImageIndex==0 )
				{
					// texture load c : original file
					iImageIndex = g_Image_InternalLoadEx ( pTextureName, iDivideTextureSize );
					if ( iImageIndex==0 )
					{
						// texture load c2 : try as dds
						strcpy ( Path, pTextureName );
						Path[strlen(Path)-4]=0;
						strcat(Path, ".dds");
						iImageIndex = g_Image_InternalLoadEx ( Path, iDivideTextureSize );
						if ( iImageIndex==0 )
						{
							// texture load d : no path just file
							iImageIndex = g_Image_InternalLoadEx ( pNoPath, iDivideTextureSize );
							if ( iImageIndex==0 )
							{
								// texture load e : 031208 - U71 - absolute path with relative path in model file
								sprintf ( Path, "%s%s", TexturePath, pTextureName );
								if ( strlen ( Path ) >= _MAX_PATH ) Path[_MAX_PATH]=0;
								iImageIndex = g_Image_InternalLoadEx ( Path, iDivideTextureSize );
								if ( iImageIndex==0 )
								{
									// texture load f : 031208 - U71 - as above, but as DDS
									strcpy(pDDSFile, pTextureName);
									DWORD dwLenDot = strlen(pDDSFile);
									if ( dwLenDot>4 )
									{
										pDDSFile[dwLenDot-4]=0;
										strcat(pDDSFile, ".dds");
										sprintf ( Path, "%s%s", TexturePath, pDDSFile );
										iImageIndex = g_Image_InternalLoadEx ( Path, iDivideTextureSize );
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return iImageIndex;
}

DARKSDK_DLL int LoadOrFindTextureAsImage ( LPSTR pTextureName, LPSTR TexturePath )
{
	return LoadOrFindTextureAsImage ( pTextureName, TexturePath, 0 );
}

DARKSDK_DLL void LoadInternalTextures ( sMesh* pMesh, LPSTR TexturePath, int iDBProMode, int iDivideTextureSize, LPSTR pOptionalLightmapNoReduce )
{
	// iDBProMode : 0-DBV1 / 1-DBPro defaults / 2-leave all states alone (for internal textureloaduse)
	// 0-DBV1 legacy behaviour
	// 1-DBPro : out of the box new pro standard
	// 2-Leave states alone to keep material/diffuse effects
	// 3-Leave states alone to keep material/texture effects
	// 4-Ensure object blends texture and diffuse at stage zero
	// 5-Leave states alone to keep multi-material effects

	// load multimaterial textures from internal name
	if ( pMesh->bUseMultiMaterial==true )
	{
		// Currrent texture if any used
		sTexture* pTexture = NULL;

		// Define textures for multi material array
		DWORD dwMultiMatCount = pMesh->dwMultiMaterialCount;
		for ( DWORD m=0; m<dwMultiMatCount; m++ )
		{
			// get multimat at index
			sMultiMaterial* pMultiMat = &(pMesh->pMultiMaterial [ m ]);

			// load texture
			int iImageIndex = LoadOrFindTextureAsImage ( pMultiMat->pName, TexturePath, iDivideTextureSize );

			// 121214 - if successful, add correct folder to multimat (for later copying)
			//char pTmpName[MAX_STRING];
			//strcpy ( pTmpName, pMultiMat->pName );
			//strcpy ( pMultiMat->pName, TexturePath );
			//strcat ( pMultiMat->pName, "\\" );
			//strcat ( pMultiMat->pName, pTmpName );

			// store texture ref
			if ( iImageIndex != 0 )
			{
				// maximum diffuse with texture (for DBV1 compatibility)
				if ( iDBProMode==0 )
				{
					// 191203 - added flag to LOAD OBJECT (DBV1 by default)
					pMultiMat->mMaterial.Diffuse.r = 1.0f;
					pMultiMat->mMaterial.Diffuse.g = 1.0f;
					pMultiMat->mMaterial.Diffuse.b = 1.0f;
					pMultiMat->mMaterial.Diffuse.a = 1.0f;
				}

				// and the texture itself
				pMultiMat->pTexturesRef = g_Image_GetPointer ( iImageIndex );
			}
			else
			{
				// no texture (uses diffuse colour for coloured model)
				pMultiMat->pTexturesRef = NULL;
			}

			// in order to let users use ambient, force this!
			if ( iDBProMode!=2 && iDBProMode!=3 && iDBProMode!=5 ) pMultiMat->mMaterial.Ambient = pMultiMat->mMaterial.Diffuse;

			// leefix - 100303 - Define master texture for blending (first texture if any)
			if ( iImageIndex != 0 && pTexture==NULL )
			{
				// free any existing texture data
				SAFE_DELETE_ARRAY ( pMesh->pTextures );

				// create single texture for mesh
				pTexture = new sTexture[1];

				// single texture for mesh
				pMesh->dwTextureCount = 1;
				pMesh->pTextures = pTexture;

				// image index for sorting
				pTexture->iImageID = iImageIndex;
				pTexture->pTexturesRef = NULL;

				// 121214 - need to track location of multimaterial texture file
				//strcpy ( pTexture->pName, pMultiMat->pName );

				// Blending states
				pTexture->dwBlendMode = D3DTOP_MODULATE;
				pTexture->dwBlendArg1 = D3DTA_TEXTURE;
			}
		}

		// lee - 040306 - u6rc5 - force this mode on load
		if ( iDBProMode==5 ) pMesh->bUsesMaterial = true;
	}
	else
	{
		// load standard textures for internal name
		DWORD dwTextureCount = pMesh->dwTextureCount;
		for ( DWORD t=0; t<dwTextureCount; t++ )
		{
			// get texture at index
			sTexture* pTexture = &(pMesh->pTextures [ t ]);

			// divide or not to divide
			int iLocalDivideValueForStage = iDivideTextureSize;
			if ( pOptionalLightmapNoReduce )
				if ( _strnicmp ( pTexture->pName, pOptionalLightmapNoReduce, strlen(pOptionalLightmapNoReduce))==NULL ) 
					iLocalDivideValueForStage=0;

			// load texture
			int iImageIndex = LoadOrFindTextureAsImage ( pTexture->pName, TexturePath, iLocalDivideValueForStage );

			// store image in texture
			if ( iImageIndex != 0 )
			{
				// setup texture settings
				pTexture->iImageID = iImageIndex;
				pTexture->pTexturesRef = g_Image_GetPointer ( iImageIndex );

				// lee - 200306 - u6b4 - new mode to blend texture and diffuse at stage zero (load object with material alpha setting)
				if ( iDBProMode==4 )
				{
					// Mode 4 - retains material settings and blends with texture
					if ( t==0 )
					{
						pTexture->dwBlendMode = D3DTOP_MODULATE;
						pTexture->dwBlendArg1 = D3DTA_TEXTURE;
						pTexture->dwBlendArg2 = D3DTA_DIFFUSE;
					}
				}
				else
				{
					// Mode 0,1,2,3
					// to alter texture behaviour from basic color to texture
					if ( iDBProMode!=2 && iDBProMode!=3 )
					{
						// Force a MODULATE for default behaviours of [0] and [1]
						pTexture->dwBlendMode = D3DTOP_MODULATE;
					}

					// Only force this for [0] [1] and [3] where we are expecting texture results
					if ( iDBProMode!=2 ) pTexture->dwBlendArg1 = D3DTA_TEXTURE;

					// maximum diffuse with texture (for DBV1 compatibility)
					if ( iDBProMode!=2 && iDBProMode!=3 )
					{
						pMesh->mMaterial.Diffuse.r = 1.0f;
						pMesh->mMaterial.Diffuse.g = 1.0f;
						pMesh->mMaterial.Diffuse.b = 1.0f;
						pMesh->mMaterial.Diffuse.a = 1.0f;
					}
				}
			}
			else
			{
				pTexture->iImageID = 0;
				pTexture->pTexturesRef = NULL;
			}

			//240203-added more defaults for better backward compat. with Patch 3 and earlier
			if ( iDBProMode!=2 && iDBProMode!=3 && iDBProMode!=4 ) pMesh->mMaterial.Ambient = pMesh->mMaterial.Diffuse;
		}
	}
}

DARKSDK_DLL void LoadInternalTextures ( sMesh* pMesh, LPSTR TexturePath, int iDBProMode, int iDivideTextureSize )
{
	LoadInternalTextures ( pMesh, TexturePath, iDBProMode, iDivideTextureSize, 0 );
}

DARKSDK_DLL void LoadInternalTextures ( sMesh* pMesh, LPSTR TexturePath, int iDBProMode )
{
	LoadInternalTextures ( pMesh, TexturePath, iDBProMode, 0 );
}

DARKSDK_DLL void FreeInternalTextures ( sMesh* pMesh )
{
	if ( pMesh )
	{
		if ( pMesh->bUseMultiMaterial==true )
		{
			if ( pMesh->pTextures )
			{
				int iImageIndex = pMesh->pTextures->iImageID;
				if ( g_Image_Delete && iImageIndex!=0 ) g_Image_Delete ( iImageIndex );
			}
		}
		else
		{
			DWORD dwTextureCount = pMesh->dwTextureCount;
			for ( DWORD t=0; t<dwTextureCount; t++ )
			{
				sTexture* pTexture = &(pMesh->pTextures [ t ]);
				if ( pTexture )
				{
					int iImageIndex = pTexture->iImageID;
					if ( g_Image_Delete && iImageIndex!=0 )
					{
						// FPSC-RC5 - internal textures are NEGATIVE values only!
						if ( iImageIndex < 0 )
							g_Image_Delete ( iImageIndex );
					}
				}
			}
		}
	}
}

DARKSDK_DLL void CloneInternalTextures ( sMesh* pMeshDest, sMesh* pMeshSrc )
{
	// copy texture info from src to dest
	if ( pMeshDest && pMeshSrc )
	{
		// get texture lists
		sTexture* pTextureSrc = pMeshSrc->pTextures;
		sTexture* pTextureDest = pMeshDest->pTextures;

		// copy all texture stages data over to dest
		if ( pTextureSrc && pTextureDest && pMeshSrc->dwTextureCount>0 )
		{
			// 110406 - u6rc7 - if src bigger than dst, recreate dst so can use entire src texture data
			if ( pMeshDest->dwTextureCount < pMeshSrc->dwTextureCount )
			{
				// same size as src texture data
				pMeshDest->dwTextureCount = pMeshSrc->dwTextureCount;
				pTextureDest = new sTexture [ pMeshDest->dwTextureCount ];
				pMeshDest->pTextures = pTextureDest;
			}

			DWORD dwTextureCount = pMeshDest->dwTextureCount;
			memcpy ( pTextureDest, pTextureSrc, sizeof(sTexture)*dwTextureCount );
		}
	}
}

DARKSDK_DLL void CopyBaseMaterialToMultiMaterial ( sMesh* pMesh )
{
	// multi-material
	for ( DWORD dwMaterialIndex=0; dwMaterialIndex<pMesh->dwMultiMaterialCount; dwMaterialIndex++ )
		pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial = pMesh->mMaterial;
}

void CopyMeshSettings ( sMesh* pDestMesh, sMesh* pSrcMesh )
{
	// exit if a ptr null
	if ( !pDestMesh || !pSrcMesh )
		return;

	// copy states across
	pDestMesh->bAlphaOverride = pSrcMesh->bAlphaOverride;
	pDestMesh->bAmbient = pSrcMesh->bAmbient;
	pDestMesh->bCull = pSrcMesh->bCull;
	pDestMesh->bFog = pSrcMesh->bFog;
	pDestMesh->bGhost = pSrcMesh->bGhost;
	pDestMesh->bLight = pSrcMesh->bLight;
	pDestMesh->bOverridePixelShader = pSrcMesh->bOverridePixelShader;
	pDestMesh->bShaderBoneSkinning = pSrcMesh->bShaderBoneSkinning;
	pDestMesh->bTransparency = pSrcMesh->bTransparency;
	pDestMesh->bUseMultiMaterial = pSrcMesh->bUseMultiMaterial;
	pDestMesh->bUsesMaterial = pSrcMesh->bUsesMaterial;
	pDestMesh->mMaterial = pSrcMesh->mMaterial;
	pDestMesh->bUseVertexShader = pSrcMesh->bUseVertexShader;
	pDestMesh->bVertexShaderEffectRefOnly = pSrcMesh->bVertexShaderEffectRefOnly;
	pDestMesh->bWireframe = pSrcMesh->bWireframe;
	pDestMesh->bZBiasActive = pSrcMesh->bZBiasActive;
	pDestMesh->bZRead = pSrcMesh->bZRead;
	pDestMesh->bZWrite = pSrcMesh->bZWrite;
	pDestMesh->dwAlphaOverride = pSrcMesh->dwAlphaOverride;
	pDestMesh->dwAlphaTestValue = pSrcMesh->dwAlphaTestValue;
	pDestMesh->dwMultiMaterialCount = pSrcMesh->dwMultiMaterialCount;
	pDestMesh->dwSubMeshListCount = pSrcMesh->dwSubMeshListCount;
	pDestMesh->dwThisTime = pSrcMesh->dwThisTime;
	pDestMesh->fMipMapLODBias = pSrcMesh->fMipMapLODBias;
	pDestMesh->fZBiasDepth = pSrcMesh->fZBiasDepth;
	pDestMesh->fZBiasSlopeScale = pSrcMesh->fZBiasSlopeScale;
	pDestMesh->iCastShadowIfStatic = pSrcMesh->iCastShadowIfStatic;
	pDestMesh->iCurrentFrame = pSrcMesh->iCurrentFrame;
	pDestMesh->iSolidForVisibility = pSrcMesh->iSolidForVisibility;
	CopyBaseMaterialToMultiMaterial ( pDestMesh );
}

DARKSDK_DLL void SetBaseTexture ( sMesh* pMesh, int iStage, int iImage )
{
	// means DB=NO STAGE SPECIFIED
	if ( iStage==-1 )
	{
		// reset effects on object
		ClearTextureSettings( pMesh );
		iStage=0;
	}

	// Set base texture at correct stage
	SetBaseTextureStage ( pMesh, iStage, iImage );
}

DARKSDK_DLL void SetBaseTextureStage ( sMesh* pMesh, int iStage, int iImage )
{
	// when specify a stage, assume not to clear texture first
	if ( iStage==-1 ) iStage=0;

	// create texture array if not present
	g_pGlob->dwInternalFunctionCode=11012;
	if ( !EnsureTextureStageValid ( pMesh, iStage ) )
		return;

	// u64 - when set a new texture stage, also set the stage itself (otherwise it stays zero!)
	pMesh->pTextures [ iStage ].dwStage = iStage;

	// set texture stage zero (base texture)
	g_pGlob->dwInternalFunctionCode=11014;
	pMesh->pTextures [ iStage ].iImageID  = iImage;
	pMesh->pTextures [ iStage ].pTexturesRef = g_Image_GetPointer ( iImage );
	//if ( pMesh->pTextures [ iStage ].pTexturesRef==NULL )
	//{
	//	if ( iImage>=550 && iImage<=800) 
	//	{
	//		int lee=42;
	//	}
	//}
	if ( pMesh->pTextures [ iStage ].pTexturesRef )
	{
		pMesh->pTextures [ iStage ].dwBlendMode = D3DTOP_MODULATE;
		pMesh->pTextures [ iStage ].dwBlendArg1 = D3DTA_TEXTURE;
		pMesh->pTextures [ iStage ].dwBlendArg2 = D3DTA_DIFFUSE;
	}
	else
	{
		pMesh->pTextures [ iStage ].dwBlendMode = D3DTOP_SELECTARG1;
		pMesh->pTextures [ iStage ].dwBlendArg1 = D3DTA_DIFFUSE;
	}
	pMesh->pTextures [ iStage ].dwTexCoordMode = 0;

	// 250704 - add name if image holds it
	g_pGlob->dwInternalFunctionCode=11015;
	if ( iImage>0 )
	{
		LPSTR pImageFilename = g_Image_GetName ( iImage );
		if ( pImageFilename )
			if ( strlen(pImageFilename) < MAX_STRING )
				strcpy ( pMesh->pTextures [ iStage ].pName, pImageFilename );
	}

	// clear material
	pMesh->bUsesMaterial = false;
	g_pGlob->dwInternalFunctionCode=11016;
}

DARKSDK_DLL void SetAlphaOverride ( sMesh* pMesh, float fPercentage )
{
	if ( fPercentage<100.0f )
	{
		fPercentage/=100.0f;
		DWORD dwAlpha = (DWORD)(fPercentage*255);
		D3DCOLOR dwAlphaValueOnly = D3DCOLOR_ARGB ( dwAlpha, 0, 0, 0 );
		pMesh->dwAlphaOverride = dwAlphaValueOnly;
		pMesh->bAlphaOverride = true;
		CopyBaseMaterialToMultiMaterial(pMesh);
	}
	else
	{
		// leefix - 041105 - can switch off if 100 percent used
		pMesh->bAlphaOverride = false;
	}
}

DARKSDK_DLL void SetDiffuseMaterial	( sMesh* pMesh, DWORD dwRGB )
{
	// lee - 040306 - u6rc5 - apply changes to base material OR multi-material!
	pMesh->bUsesMaterial = true;
	if ( pMesh->dwMultiMaterialCount > 0 )
	{
		// multi-material
		for ( DWORD dwMaterialIndex=0; dwMaterialIndex<pMesh->dwMultiMaterialCount; dwMaterialIndex++ )
		{
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Diffuse.r = ((dwRGB & 0x00FF0000)>>16)/255.0f;
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Diffuse.g = ((dwRGB & 0x0000FF00)>>8)/255.0f;
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Diffuse.b = ((dwRGB & 0x000000FF)>>0)/255.0f;
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Diffuse.a = ((dwRGB & 0xFF000000)>>24)/255.0f;
		}
	}
	else
	{
		// base material
		pMesh->mMaterial.Diffuse.r = ((dwRGB & 0x00FF0000)>>16)/255.0f;
		pMesh->mMaterial.Diffuse.g = ((dwRGB & 0x0000FF00)>>8)/255.0f;
		pMesh->mMaterial.Diffuse.b = ((dwRGB & 0x000000FF)>>0)/255.0f;
		pMesh->mMaterial.Diffuse.a = ((dwRGB & 0xFF000000)>>24)/255.0f;
		// CopyBaseMaterialToMultiMaterial(pMesh);
	}
}

DARKSDK_DLL void SetAmbienceMaterial ( sMesh* pMesh, DWORD dwRGB )
{
	// lee - 040306 - u6rc5 - apply changes to base material OR multi-material!
	pMesh->bUsesMaterial = true;
	if ( pMesh->dwMultiMaterialCount > 0 )
	{
		// multi-material
		for ( DWORD dwMaterialIndex=0; dwMaterialIndex<pMesh->dwMultiMaterialCount; dwMaterialIndex++ )
		{
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Ambient.r = ((dwRGB & 0x00FF0000)>>16)/255.0f;
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Ambient.g = ((dwRGB & 0x0000FF00)>>8)/255.0f;
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Ambient.b = ((dwRGB & 0x000000FF)>>0)/255.0f;
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Ambient.a = ((dwRGB & 0xFF000000)>>24)/255.0f;
		}
	}
	else
	{
		// base material
		pMesh->mMaterial.Ambient.r = ((dwRGB & 0x00FF0000)>>16)/255.0f;
		pMesh->mMaterial.Ambient.g = ((dwRGB & 0x0000FF00)>>8)/255.0f;
		pMesh->mMaterial.Ambient.b = ((dwRGB & 0x000000FF)>>0)/255.0f;
		pMesh->mMaterial.Ambient.a = ((dwRGB & 0xFF000000)>>24)/255.0f;
		//CopyBaseMaterialToMultiMaterial(pMesh);
	}
}

DARKSDK_DLL void SetSpecularMaterial ( sMesh* pMesh, DWORD dwRGB )
{
	// lee - 040306 - u6rc5 - apply changes to base material OR multi-material!
	pMesh->bUsesMaterial = true;
	if ( pMesh->dwMultiMaterialCount > 0 )
	{
		// multi-material
		for ( DWORD dwMaterialIndex=0; dwMaterialIndex<pMesh->dwMultiMaterialCount; dwMaterialIndex++ )
		{
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Specular.r = ((dwRGB & 0x00FF0000)>>16)/255.0f;
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Specular.g = ((dwRGB & 0x0000FF00)>>8)/255.0f;
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Specular.b = ((dwRGB & 0x000000FF)>>0)/255.0f;
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Specular.a = ((dwRGB & 0xFF000000)>>24)/255.0f;
		}
	}
	else
	{
		// base material
		pMesh->mMaterial.Specular.r = ((dwRGB & 0x00FF0000)>>16)/255.0f;
		pMesh->mMaterial.Specular.g = ((dwRGB & 0x0000FF00)>>8)/255.0f;
		pMesh->mMaterial.Specular.b = ((dwRGB & 0x000000FF)>>0)/255.0f;
		pMesh->mMaterial.Specular.a = ((dwRGB & 0xFF000000)>>24)/255.0f;
		// CopyBaseMaterialToMultiMaterial(pMesh);
	}
}

DARKSDK_DLL void SetEmissiveMaterial ( sMesh* pMesh, DWORD dwRGB )
{
	// lee - 040306 - u6rc5 - apply changes to base material OR multi-material!
	pMesh->bUsesMaterial = true;
	if ( pMesh->dwMultiMaterialCount > 0 )
	{
		// multi-material
		for ( DWORD dwMaterialIndex=0; dwMaterialIndex<pMesh->dwMultiMaterialCount; dwMaterialIndex++ )
		{
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Emissive.r = ((dwRGB & 0x00FF0000)>>16)/255.0f;
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Emissive.g = ((dwRGB & 0x0000FF00)>>8)/255.0f;
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Emissive.b = ((dwRGB & 0x000000FF)>>0)/255.0f;
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Emissive.a = ((dwRGB & 0xFF000000)>>24)/255.0f;
		}
	}
	else
	{
		// base material
		pMesh->mMaterial.Emissive.r = ((dwRGB & 0x00FF0000)>>16)/255.0f;
		pMesh->mMaterial.Emissive.g = ((dwRGB & 0x0000FF00)>>8)/255.0f;
		pMesh->mMaterial.Emissive.b = ((dwRGB & 0x000000FF)>>0)/255.0f;
		pMesh->mMaterial.Emissive.a = ((dwRGB & 0xFF000000)>>24)/255.0f;
		// CopyBaseMaterialToMultiMaterial(pMesh);
	}
}

DARKSDK_DLL void SetSpecularPower ( sMesh* pMesh, float fPower )
{
	// lee - 040306 - u6rc5 - apply changes to base material OR multi-material!
	pMesh->bUsesMaterial = true;
	if ( pMesh->dwMultiMaterialCount > 0 )
	{
		// multi-material
		for ( DWORD dwMaterialIndex=0; dwMaterialIndex<pMesh->dwMultiMaterialCount; dwMaterialIndex++ )
			pMesh->pMultiMaterial [ dwMaterialIndex ].mMaterial.Power = fPower;
	}
	else
	{
		// base material
		pMesh->mMaterial.Power = fPower;
		// CopyBaseMaterialToMultiMaterial(pMesh);
	}
}

DARKSDK_DLL void RemoveTextureRefFromMesh  ( sMesh* pMesh, LPDIRECT3DTEXTURE9 pTextureRef )
{
	DWORD dwTextureCount = pMesh->dwTextureCount;
	for ( DWORD t=0; t<dwTextureCount; t++ )
	{
		sTexture* pTexture = &(pMesh->pTextures [ t ]);
		if ( pTexture->pTexturesRef==pTextureRef )
		{
			// remove reference to texture
			pTexture->pTexturesRef = NULL;
		}
	}
}

DARKSDK_DLL void SetMultiTexture ( sMesh* pMesh, int iStage, DWORD dwBlendMode, DWORD dwTexCoordMode, int iImage )
{
	// create texture array if not present
	if ( !EnsureTextureStageValid ( pMesh, iStage ) )
		return;

	// free any previous cube map
	SAFE_RELEASE ( pMesh->pTextures [ iStage ].pCubeTexture );

	// set texture stage
	pMesh->pTextures [ iStage ].dwStage = iStage;
	pMesh->pTextures [ iStage ].iImageID  = iImage;
	pMesh->pTextures [ iStage ].pTexturesRef = g_Image_GetPointer ( iImage );
	pMesh->pTextures [ iStage ].dwBlendMode = dwBlendMode;
	pMesh->pTextures [ iStage ].dwBlendArg1 = D3DTA_TEXTURE;
	pMesh->pTextures [ iStage ].dwBlendArg2 = D3DTA_CURRENT;
	pMesh->pTextures [ iStage ].dwTexCoordMode = dwTexCoordMode;
}

DARKSDK_DLL void SetCubeTexture ( sMesh* pMesh, int iStage, LPDIRECT3DCUBETEXTURE9 pCubeTexture )
{
	// create texture array if not present
	if ( !EnsureTextureStageValid ( pMesh, iStage ) )
		return;

	// free any previous cube map
	SAFE_RELEASE ( pMesh->pTextures [ iStage ].pCubeTexture );

	// set cube texture stage
	pMesh->pTextures [ iStage ].dwStage = iStage;
	pMesh->pTextures [ iStage ].iImageID  = 0;
	pMesh->pTextures [ iStage ].pTexturesRef = NULL;
	pMesh->pTextures [ iStage ].pCubeTexture = pCubeTexture;
	pMesh->pTextures [ iStage ].dwBlendMode = D3DTOP_MODULATE;
	pMesh->pTextures [ iStage ].dwBlendArg1 = D3DTA_TEXTURE;
	pMesh->pTextures [ iStage ].dwBlendArg2 = D3DTA_CURRENT;
	pMesh->pTextures [ iStage ].dwTexCoordMode = 2;
}

DARKSDK_DLL void SetBaseColor ( sMesh* pMesh, DWORD dwRGB )
{
	// reset effects on object
	ClearTextureSettings( pMesh );

	// additionally clear texture reference
	sTexture* pTexture = &pMesh->pTextures [ 0 ];
	pTexture->iImageID = 0;
	pTexture->pTexturesRef = NULL;
	pTexture->dwBlendMode = D3DTOP_SELECTARG1;
	pTexture->dwBlendArg1 = D3DTA_DIFFUSE;
	pTexture->dwBlendArg2 = D3DTA_DIFFUSE;
	pTexture->dwTexCoordMode = 0;

	// Assign a colour to the material
	pMesh->bUsesMaterial = true;
	ResetMaterial ( &pMesh->mMaterial );
	ColorMaterial ( &pMesh->mMaterial, dwRGB );
}

// Mesh Shader Functions

DARKSDK_DLL bool InitEffectSystem ( sMesh* pMesh, DWORD dwTextureCount, cSpecialEffect* pEffectObj )
{
	// validate for shader
	// leefix - 010204 - some FX files do not require pixel shaders!
	if(!ValidateMeshForShader ( pMesh, dwTextureCount ))
		return false;

	// vertex shader effect
	pMesh->bUseVertexShader = true;
	pMesh->pVertexShaderEffect = pEffectObj;
	if ( pMesh->pVertexShaderEffect==NULL )
		return false;

	// pixel shader effect (completed by Setup) (redundant)
	pMesh->bOverridePixelShader = true;
	pMesh->pPixelShader = NULL;

	// lee - 230306 - u6b4 - ensure the mesh is unaltered by animation (so correct mesh conversion can happen)
	ResetVertexDataInMeshPerMesh ( pMesh );

	// complete
	return true;
}

DARKSDK_DLL void SetBumpTexture ( sMesh* pMesh, int iBumpMap, int iGenerateNormalMap )
{
	// initialise FX effect
	cSpecialEffect* pEffectObj = new cBumpMapping;
	if ( !InitEffectSystem ( pMesh, 2, pEffectObj ) )
	{
		SAFE_DELETE(pEffectObj);
		return;
	}

	// switch effect flags
	if ( iGenerateNormalMap==1 ) pMesh->pVertexShaderEffect->m_bGenerateNormals = true;

	// set texture references in mesh
	pMesh->pTextures [ 1 ].iImageID  = iBumpMap;
	pMesh->pTextures [ 1 ].pTexturesRef = g_Image_GetPointer ( iBumpMap );

	// prepare model for shader
	pMesh->pVertexShaderEffect->Load ( "bump.fx", false, false );
	pMesh->pVertexShaderEffect->Setup ( pMesh );
	pMesh->pVertexShaderEffect->Mesh ( pMesh );
}

DARKSDK_DLL void SetCartoonTexture	( sMesh* pMesh, int iImage, int iEdge )
{
	// initialise FX effect
	cSpecialEffect* pEffectObj = new cCartoonMapping;
	if ( !InitEffectSystem ( pMesh, 2, pEffectObj ) )
	{
		SAFE_DELETE(pEffectObj);
		return;
	}

	// switch effect flags
	pMesh->pVertexShaderEffect->m_bGenerateNormals = true;

	// set texture references in mesh
	pMesh->pTextures [ 0 ].iImageID  = iImage;
	pMesh->pTextures [ 0 ].pTexturesRef = g_Image_GetPointer ( iImage );
	pMesh->pTextures [ 1 ].iImageID  = iEdge;
	pMesh->pTextures [ 1 ].pTexturesRef = g_Image_GetPointer ( iEdge );

	// convert to standard FVF
	ConvertToFVF ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE );

	// prepare model for shader
	pMesh->pVertexShaderEffect->Load ( "cartoon.fx", false, false );
	pMesh->pVertexShaderEffect->Setup ( pMesh );
	pMesh->pVertexShaderEffect->Mesh ( pMesh );
}

DARKSDK_DLL void SetRainbowTexture ( sMesh* pMesh, int iImage )
{
	// initialise FX effect
	cSpecialEffect* pEffectObj = new cRainbowMapping;
	if ( !InitEffectSystem ( pMesh, 2, pEffectObj ) )
	{
		SAFE_DELETE(pEffectObj);
		return;
	}

	// effect must have normals
	pMesh->pVertexShaderEffect->m_bGenerateNormals = true;

	// set texture references in mesh
	pMesh->pTextures [ 0 ].iImageID  = iImage;
	pMesh->pTextures [ 0 ].pTexturesRef = g_Image_GetPointer ( iImage );
	pMesh->pTextures [ 1 ].iImageID  = iImage;
	pMesh->pTextures [ 1 ].pTexturesRef = g_Image_GetPointer ( iImage );

	// convert to standard FVF
	ConvertToFVF ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL );

	// prepare model for shader
	pMesh->pVertexShaderEffect->Load ( "rainbow.fx", false, false );
	pMesh->pVertexShaderEffect->Setup ( pMesh );
	pMesh->pVertexShaderEffect->Mesh ( pMesh );
}

// Mesh Effect Functions

DARKSDK_DLL bool SetSpecialEffect ( sMesh* pMesh, cSpecialEffect* pEffectObj, bool bChangeMesh )
{
	// On or Off
	if ( pEffectObj )
	{
		// lee - 300914 - early out if effect shader uses BONES but this mesh has NO BONES, do not use this shader!
		if ( pEffectObj->m_bUsesBoneData==TRUE && pMesh->dwBoneCount==0 )
		{
			// silent fail, simply leaves this mesh unaffected by shader
			return false;
		}

		// initialise FX effect
		if ( !InitEffectSystem ( pMesh, pMesh->dwTextureCount, pEffectObj ) )
			return false;

		// give all effects normals (if not got them)
		if ( pMesh->pVertexShaderEffect->m_bDoNotGenerateExtraData==0 )
		{
			// leeadd - 050906 - only auto-generate if not switched off
			pMesh->pVertexShaderEffect->m_bGenerateNormals = true;
		}

		// wipe out original mesh data as mesh may be changed now
		// 220214 - SAFE_DELETE_ARRAY ( pMesh->pOriginalVertexData );
		SAFE_DELETE_ARRAY ( pMesh->pOriginalVertexData );

		// mike - 011005 - more than 8 textures causes a crash
		// lee - 270314 - no longer the case!
		//if ( pMesh->pVertexShaderEffect->m_dwTextureCount > 8 )
		//	return false;

		// load effect with settings
		if ( !pMesh->pVertexShaderEffect->Setup ( pMesh ) )
		{
			pMesh->pVertexShaderEffect=NULL;
			return false;
		}

		// prepare model for effect
		if ( bChangeMesh ) pMesh->pVertexShaderEffect->Mesh ( pMesh );

		// record effect name
		strcpy ( pMesh->pEffectName, pEffectObj->m_pEffectName );
	}
	else
	{
		// Delete any vertex shader being used
		FreeVertexShaderMesh ( pMesh );
	}

	// complete
	return true;
}

DARKSDK_DLL bool SetSpecialEffect ( sMesh* pMesh, cSpecialEffect* pEffectObj )
{
	return SetSpecialEffect ( pMesh, pEffectObj, true );
}

// Mesh Custom Vertex Shader Functions

DARKSDK_DLL void ConvertToFVF ( sMesh* pMesh, DWORD dwFVF )
{
	// leefix - 150306 - u60b3 - when mesh changes FVF, really need to erase old orig data
	// simply because it will attempt to 'copy' when asked to reset, and it will copy the wrong FVF pattern
	SAFE_DELETE_ARRAY ( pMesh->pOriginalVertexData );

	// Use main FVF converter function
	ConvertLocalMeshToFVF ( pMesh, dwFVF );
}

DARKSDK_DLL void CombineSubsetPolygonsInMesh ( sMesh* pMesh )
{
	// go through each polygon
//	for ()
	{
		// gather plane and vector info (V1,V2,V3) for tagretpoly

		// go through all other polygons
//		for ()
		{
			// if two vertex indexes match the tagretpoly
			if ( 0 )
			{
				// consider VA and VB being the odd vertex indices
				// VM1 and VM2 being the matchers

				// create a normalised vector from VA to VM1
				// if identical to VM1 to VB, go as VM1
				// create a normalised vector from VA to VM2
				// if identical to VM2 to VB, go as VM2
				if ( 0 )
				{
					// if VM1, VM1 is removed to produce tri from quad
					// if VM2, VM2 is removed to produce tri from quad
				}
			}
		}
	}
}

DARKSDK_DLL void SetCustomShader ( sMesh* pMesh, LPDIRECT3DVERTEXSHADER9 pVertexShader, LPDIRECT3DVERTEXDECLARATION9 pVertexDec, DWORD dwStagesRequired )
{
	// reset effects on object
	ClearTextureSettings( pMesh );

	// validate for shader
	if(!ValidateMeshForShader ( pMesh, dwStagesRequired ))
		return;

	// vertex shader effect active
	pMesh->bUseVertexShader = true;
	pMesh->pVertexShader = pVertexShader;
	pMesh->pVertexDec = pVertexDec;

	// set texture stages for this shader
	for (DWORD dwIndex=0; dwIndex<dwStagesRequired; dwIndex++)
	{
		pMesh->pTextures [ dwIndex ].dwStage = dwIndex;
		pMesh->pTextures [ dwIndex ].dwBlendMode = D3DTOP_MODULATE;
		pMesh->pTextures [ dwIndex ].dwBlendArg1 = D3DTA_TEXTURE;
		pMesh->pTextures [ dwIndex ].dwTexCoordMode = 0;
		if ( dwIndex==0 )
			pMesh->pTextures [ dwIndex ].dwBlendArg2 = D3DTA_DIFFUSE;
		else
			pMesh->pTextures [ dwIndex ].dwBlendArg2 = D3DTA_CURRENT;
	}
}

DARKSDK_DLL void SetNoShader ( sMesh* pMesh )
{
	// vertex shader effect deactivate
	pMesh->bUseVertexShader = false;
	if ( pMesh->pVertexShader )
	{
		pMesh->pVertexShader->Release();
		pMesh->pVertexShader=NULL;
	}
	if ( pMesh->pVertexDec )
	{
		pMesh->pVertexDec->Release();
		pMesh->pVertexDec=NULL;
	}
}

// Mesh Custom Pixel Shader Functions

DARKSDK_DLL void SetCustomPixelShader ( sMesh* pMesh, LPDIRECT3DPIXELSHADER9 pPixelShader )
{
	// pixel shader effect active
	pMesh->bOverridePixelShader = true;
	pMesh->pPixelShader = pPixelShader;
}

DARKSDK_DLL void SetNoPixelShader ( sMesh* pMesh )
{
	// pixel shader effect deactivate
	pMesh->bOverridePixelShader = false;
	pMesh->pPixelShader = NULL;
}

// Mesh Animation Functions

DARKSDK_DLL void VectorTransform ( const D3DXVECTOR3 in1, const D3DXMATRIX matrix, D3DXVECTOR3 &out )
{
	float in2 [ 3 ] [ 4 ];

	memcpy ( &in2, matrix, sizeof ( in2 ) );

	#define DotProduct( x, y ) ( ( x ) [ 0 ] * ( y ) [ 0 ] + ( x ) [ 1 ] * ( y ) [ 1 ] + ( x ) [ 2 ] * ( y ) [ 2 ] )

	out [ 0 ] = DotProduct ( in1, in2 [ 0 ] ) + in2 [ 0 ] [ 3 ];
	out [ 1 ] = DotProduct ( in1, in2 [ 1 ] ) +	in2 [ 1 ] [ 3 ];
	out [ 2 ] = DotProduct ( in1, in2 [ 2 ] ) +	in2 [ 2 ] [ 3 ];
}

DARKSDK_DLL void AnimateBoneMeshMDL ( sObject* pObject, sFrame* pFrame )
{
	// MIKE 240303 - MDL ANIMATION STYLE /////////////////////////////////////////////////////////
	int iFrame = ( int ) pObject->fAnimFrame;
	int iPos = 0;

	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		sMesh* pMesh = pObject->ppMeshList [ iMesh ];

		// first time around, copy vertex data
		if ( pMesh->pOriginalVertexData==NULL ) CollectOriginalVertexData ( pMesh );

		// get the offset map for the vertex data
		sOffsetMap offsetMap;
		GetFVFOffsetMap ( pMesh, &offsetMap );

		int iVertexPosition = 0;
		
		for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
		{
			D3DXVECTOR3 vecInput = D3DXVECTOR3 (
													*( ( float* ) pMesh->pOriginalVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pMesh->pOriginalVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pMesh->pOriginalVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
											   );

			D3DXVECTOR3 vecOutput;

			VectorTransform ( 
								vecInput,
								pObject->pAnimationSet->pAnimation->ppBoneFrames
										[ iFrame ]
										[ pObject->pAnimationSet->pAnimation->piBoneOffsetList [ iPos++ ] ],
								vecOutput
						 );

			// copy data across
			*( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ) = vecOutput.x;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ) = vecOutput.y;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) ) = vecOutput.z;
		}

		pMesh->bVBRefreshRequired = true;
		g_vRefreshMeshList.push_back ( pMesh );
	}
}

DARKSDK_DLL void AnimateBoneMeshMDX ( sObject* pObject, sFrame* pFrame )
{
	// get mesh ptr
	sMesh* pMesh = pFrame->pMesh;
	if ( pFrame->pMesh->dwSubMeshListCount <= 1 )
		return;

	// and object animating
	if ( !pObject->bAnimPlaying && !pObject->bAnimUpdateOnce )
		return;

	// leeadd -240604 - u54 - MD2 only once update required
	pObject->bAnimUpdateOnce = false;

	/* leefix - 240604 - u54 - replaced with below new unified animation data
	// move to the next frame
	pFrame->pMesh->iNextFrame = ( pFrame->pMesh->iCurrentFrame + 1 ) % pFrame->pMesh->dwSubMeshListCount;

	// make sure anim frame is updated
	pObject->fAnimFrame = ( float ) pFrame->pMesh->iNextFrame;

	// interpolation code
	float fTime			  = ( float ) GetTickCount ( );						// get current time in milliseconds
	float fElapsedTime	  = fTime - pMesh->fLastInterp;						// get the elapsed time
	int   iAnimationSpeed = (int)(pObject->fAnimSpeed*10.0f);				// frame rate for animation
	float fInterp		  = fElapsedTime / ( 1000.0f / iAnimationSpeed );	// find out how far we are from the current frame to the next ( between 0 and 1 )

	// if our elapsed time goes over the desired time
	// segment start over and go to the next key frame
	if ( fElapsedTime >= ( 1000.0f / iAnimationSpeed ) )
	{
		// set current frame to the next frame
		pFrame->pMesh->iCurrentFrame = pFrame->pMesh->iNextFrame;

		// store the time
		pMesh->fLastInterp = fTime;
	}
	
	// get the offset map for the vertex data
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// store link to matrix
	pFrame->matOriginal = pMesh->pSubFrameList [ pFrame->pMesh->iNextFrame ].matOriginal;

	// make sure the frame numbers are valid
	if ( pFrame->pMesh->iNextFrame + 1 >= (int)pFrame->pMesh->dwSubMeshListCount )
	{
		// set next and current frame to 0
		pFrame->pMesh->iNextFrame    = 0;
		pFrame->pMesh->iCurrentFrame = 0;
	}
	*/

	// get the offset map for the vertex data
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// store the nearest frame of the animation data
	pFrame->pMesh->iNextFrame = (int)pObject->fAnimFrame;
	float fInterp = pObject->fAnimFrame - (float)pFrame->pMesh->iNextFrame;
	if ( pFrame->pMesh->iNextFrame + 1 >= (int)pFrame->pMesh->dwSubMeshListCount )
	{
		pFrame->pMesh->iNextFrame    = 0;
		fInterp = 0.0f;
	}

	// store link to matrix
	pFrame->matOriginal = pMesh->pSubFrameList [ pFrame->pMesh->iNextFrame ].matOriginal;

	// copy the vertices across
	for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
	{
		if ( offsetMap.dwZ>0 )
		{
			// find the frame
			sFrame*	pTheFrame     = pMesh->pSubFrameList;
			sFrame* pCurrentFrame = NULL;
			sFrame* pLastFrame    = NULL;
			sFrame* pNextFrame    = NULL;
			int     iFrame        = 0;

			D3DXVECTOR3 vecNextPos = D3DXVECTOR3 (
													*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iNextFrame + 1 ].pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iNextFrame + 1 ].pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iNextFrame + 1 ].pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
												 );

			D3DXVECTOR3 vecLastPos = D3DXVECTOR3 (
													*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iNextFrame ].pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iNextFrame ].pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iNextFrame ].pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
												 );

			D3DXVECTOR3 vecFinal = vecLastPos + ( vecNextPos - vecLastPos ) * fInterp;

			*( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ) = vecFinal.x;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ) = vecFinal.y;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) ) = vecFinal.z;
		}
	}

	// trigger mesh to VB update
	pMesh->bVBRefreshRequired = true;
	g_vRefreshMeshList.push_back ( pMesh );
}

void AnimateBoneMeshBONE ( sObject* pObject, sFrame* pFrame, sMesh* pMesh )
{
	// first time around, copy vertex data to original-store (131214 - external to the NEVERSTOREORIGINALVERTICES define as it is NEEDED here)
	if ( pMesh->pOriginalVertexData==NULL )
	{
		// first time around, copy vertex data to original-store
		DWORD dwTotalVertSize = pMesh->dwVertexCount * pMesh->dwFVFSize;
		pMesh->pOriginalVertexData = (BYTE*)new char [ dwTotalVertSize ];
		memcpy ( pMesh->pOriginalVertexData, pMesh->pVertexData, dwTotalVertSize );
	}

	// 010303 - new vertex blending for bones (to take advantage of multiple weights)
	D3DXMATRIX* matrices = new D3DXMATRIX [ pMesh->dwBoneCount ];
	BYTE* weighttable = new BYTE [ pMesh->dwVertexCount ];
	memset ( weighttable, 0, pMesh->dwVertexCount );

	// update all bone matrices
	for ( DWORD dwMatrixIndex = 0; dwMatrixIndex < pMesh->dwBoneCount; dwMatrixIndex++ )
	{
		if ( pMesh->pFrameMatrices [ dwMatrixIndex ] ) // lee - 180406 - u6rc10 - not all bones connect to animating frame
			D3DXMatrixMultiply ( &matrices [ dwMatrixIndex ], &pMesh->pBones [ dwMatrixIndex ].matTranslation, pMesh->pFrameMatrices [ dwMatrixIndex ] );
			// attempt to fix issue of animation system using frame 1 instead of bone pose rig
			// memcpy ( &matrices [ dwMatrixIndex ], &pMesh->pFrameMatrices [ dwMatrixIndex ], sizeof(D3DXMATRIX) );
		else
			memcpy ( &matrices [ dwMatrixIndex ], &pMesh->pBones [ dwMatrixIndex ].matTranslation, sizeof(D3DXMATRIX) );
	}
	// run through all bones
	for ( int iBone = 0; iBone < ( int ) pMesh->dwBoneCount; iBone++ )
	{
		// go through all influenced bones
		for ( int iLoop = 0; iLoop < ( int ) pMesh->pBones [ iBone ].dwNumInfluences; iLoop++ )
		{
			// get the correct vertex and weight
			int iOffset = pMesh->pBones [ iBone ].pVertices [ iLoop ];
			float fWeight = pMesh->pBones [ iBone ].pWeights [ iLoop ];

			// Vertex Data Ptrs
			float* pDestVertexBase = (float*)(pMesh->pVertexData + ( pMesh->dwFVFSize * iOffset ));
			float* pVertexBase = (float*)(pMesh->pOriginalVertexData + ( pMesh->dwFVFSize * iOffset ));

			// clear new vertex if changing it
			if ( weighttable [ iOffset ]==0 )
			{
				memset ( pDestVertexBase, 0, 12 );
				if ( pMesh->dwFVF | D3DFVF_NORMAL )
					memset ( pDestVertexBase+3, 0, 12 );
			}
			weighttable [ iOffset ] = 1;
			
			// get original vertex position
			D3DXVECTOR3 vec = D3DXVECTOR3 ( *(pVertexBase+0), *(pVertexBase+1), *(pVertexBase+2) );

			// multiply the vector and the bone matrix with weight
			D3DXVECTOR3 newVec = MultiplyVectorAndMatrix ( vec, matrices [ iBone ] ) * fWeight;
			
			// accumilate vertex for final result
			*(pDestVertexBase+0) += newVec.x;
			*(pDestVertexBase+1) += newVec.y;
			*(pDestVertexBase+2) += newVec.z;

			// transform and normalise the normals vector (if present)
			if ( pMesh->dwFVF | D3DFVF_NORMAL )
			{
				D3DXVECTOR3 norm = D3DXVECTOR3 ( *(pVertexBase+3), *(pVertexBase+4), *(pVertexBase+5) );
				D3DXVECTOR3 newNorm;
				D3DXVec3TransformNormal ( &newNorm, &norm, &matrices [ iBone ] );
				newNorm = newNorm * fWeight;
					
				// update the normal with the new normal values
				*(pDestVertexBase+3) += newNorm.x;
				*(pDestVertexBase+4) += newNorm.y;
				*(pDestVertexBase+5) += newNorm.z;
			}
		}
	}

	// free matrix bank
	SAFE_DELETE_ARRAY ( matrices );
	SAFE_DELETE_ARRAY ( weighttable );

	// trigger mesh to VB update
	pMesh->bVBRefreshRequired = true;
	g_vRefreshMeshList.push_back ( pMesh );
}

DARKSDK_DLL void AnimateBoneMeshBONE ( sObject* pObject, sFrame* pFrame )
{
	AnimateBoneMeshBONE ( pObject, pFrame, pFrame->pMesh );
}

bool AnimateBoneMesh ( sObject* pObject, sFrame* pFrame, sMesh* pMesh )
{
	// check if mesh valid
	if ( !pMesh ) return true;

	// see if we need to deal with bones
	if ( pMesh->dwBoneCount )
	{
		// leefix - 211103 - Can reject right away if frame unchanged
		// leefix - 101203 - Fixed the logic here
		bool bQuickAccept = false;
		if ( pObject->fAnimFrame != pObject->fAnimLastFrame )
			bQuickAccept = true;

		// leefix - 140504 - once only update passes
		if ( pObject->bAnimUpdateOnce )
			bQuickAccept = true;

		// leeadd - 120204 - additional accept if slerp changed
		if ( pObject->bAnimManualSlerp==true )
			if ( pObject->fAnimSlerpTime != pObject->fAnimSlerpLastTime )
				bQuickAccept = true;

		// leefix - 300305 - objects with multiple mesh-animations would not work
		// because these two lines would suggest it was finished even after the first mesh
//		// Store frame for next quick reject check
//		pObject->fAnimLastFrame = pObject->fAnimFrame;
//		pObject->fAnimSlerpLastTime = pObject->fAnimSlerpTime;

		// Quick reject check
		if ( bQuickAccept )
		{
			// check if MDL
			if ( pObject->pAnimationSet )
			{
				if ( pObject->pAnimationSet->pAnimation )
				{
					if ( !pObject->pAnimationSet->pAnimation->bBoneType )
					{
						// Animate model as MDL animation
						AnimateBoneMeshMDL ( pObject, pFrame );

						// complete early
						return true;
					}
				}
			}

			// failing MDL, Animate model as BONE animation (if not done in HW)
			if ( pMesh->bShaderBoneSkinning==false || pMesh->dwForceCPUAnimationMode==1 )
				AnimateBoneMeshBONE ( pObject, pFrame, pMesh );
		}
	}
	else
	{
		// Animate model as MDX animation
		AnimateBoneMeshMDX ( pObject, pFrame );
	}

	// return always animated
	return true;
}

DARKSDK_DLL bool AnimateBoneMesh ( sObject* pObject, sFrame* pFrame )
{
	// normal object-mesh bone animation
	return AnimateBoneMesh ( pObject, pFrame, pFrame->pMesh );
}

DARKSDK_DLL void ResetVertexDataInMeshPerMesh ( sMesh* pMesh )
{
	// valid mesh
	if ( !pMesh )
		return;

	// if original data available and same size, restore with it
	if ( pMesh->pOriginalVertexData )
	{
		DWORD dwTotalVertSize = pMesh->dwVertexCount * pMesh->dwFVFSize;
		memcpy ( pMesh->pVertexData, pMesh->pOriginalVertexData, dwTotalVertSize );
		pMesh->bVBRefreshRequired = true;
		g_vRefreshMeshList.push_back ( pMesh );
	}
}

DARKSDK_DLL void CollectOriginalVertexData ( sMesh* pMesh )
{
	#ifndef NEVERSTOREORIGINALVERTICES
	if ( pMesh->pOriginalVertexData==NULL )
	{
		// first time around, copy vertex data to original-store
		DWORD dwTotalVertSize = pMesh->dwVertexCount * pMesh->dwFVFSize;
		pMesh->pOriginalVertexData = (BYTE*)new char [ dwTotalVertSize ];
		memcpy ( pMesh->pOriginalVertexData, pMesh->pVertexData, dwTotalVertSize );
	}
	#endif
}

DARKSDK_DLL void ResetVertexDataInMesh ( sObject* pObject )
{
	// valid object
	if ( !pObject )
		return;

	// go through all meshes of object
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		// get mesh ptr
		sMesh* pMesh = pObject->ppMeshList [ iMesh ];
		ResetVertexDataInMeshPerMesh ( pMesh );
	}
}

DARKSDK_DLL void UpdateVertexDataInMesh ( sObject* pObject )
{
	// valid object
	if ( !pObject )
		return;

	// go through all meshes of object
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		// get mesh ptr
		sMesh* pMesh = pObject->ppMeshList [ iMesh ];

		// delete old data and re-capture latest vertex data (new UV data changes mostly)
		SAFE_DELETE_ARRAY ( pMesh->pOriginalVertexData );
		CollectOriginalVertexData ( pMesh );
	}
}

// Mesh Shadow Meshes

//--------------------------------------------------------------------------------------
// Takes an array of CEdgeMapping objects, then returns an index for the edge in the
// table if such entry exists, or returns an index at which a new entry for the edge
// can be written.
// nV1 and nV2 are the vertex indexes for the old edge.
// nCount is the number of elements in the array.
// The function returns -1 if an available entry cannot be found.  In reality,
// this should never happens as we should have allocated enough memory.
int FindEdgeInMappingTable( int nV1, int nV2, CEdgeMapping *pMapping, int nCount )
{
    for( int i = 0; i < nCount; ++i )
    {
        // If both vertex indexes of the old edge in mapping entry are -1, then
        // we have searched every valid entry without finding a match.  Return
        // this index as a newly created entry.
        if( ( pMapping[i].m_anOldEdge[0] == -1 && pMapping[i].m_anOldEdge[1] == -1 ) ||

            // Or if we find a match, return the index.
            ( pMapping[i].m_anOldEdge[1] == nV1 && pMapping[i].m_anOldEdge[0] == nV2 ) )
        {
            return i;
        }
    }

    return -1;  // We should never reach this line
}

//--------------------------------------------------------------------------------------
// Takes a mesh and generate a new mesh from it that contains the degenerate invisible
// quads for shadow volume extrusion.
// LEE: Okay, but any bone animation influence data gets lost when extra verts are made
// so we now have a table to keep track of the original vertex index responsible for new'n
// Global used to store a reference table for shadow generation
DWORD* g_pdwKeepTrackOfOriginalVertIndex	= NULL;

HRESULT GenerateDXShadowVolumeMeshForShader( ID3DXMesh *pMesh, ID3DXMesh **ppOutMesh )
{
    HRESULT hr = S_OK;
    ID3DXMesh *pInputMesh;

    if( !ppOutMesh )
        return E_INVALIDARG;
    *ppOutMesh = NULL;

    // Convert the input mesh to a format same as the output mesh using 32-bit index.
    hr = pMesh->CloneMesh( D3DXMESH_32BIT, SHADOWVERT::Decl, m_pD3D, &pInputMesh );
    if( FAILED( hr ) )
        return hr;

//    DXUTTRACE( L"Input mesh has %u vertices, %u faces\n", pInputMesh->GetNumVertices(), pInputMesh->GetNumFaces() );

    // Generate adjacency information
    DWORD *pdwAdj = new DWORD[3 * pInputMesh->GetNumFaces()];
    DWORD *pdwPtRep = new DWORD[pInputMesh->GetNumVertices()];
    if( !pdwAdj || !pdwPtRep )
    {
        delete[] pdwAdj; delete[] pdwPtRep;
        pInputMesh->Release();
        return E_OUTOFMEMORY;
    }

    hr = pInputMesh->GenerateAdjacency( ADJACENCY_EPSILON, pdwAdj );
    if( FAILED( hr ) )
    {
        delete[] pdwAdj; delete[] pdwPtRep;
        pInputMesh->Release();
        return hr;
    }

    pInputMesh->ConvertAdjacencyToPointReps( pdwAdj, pdwPtRep );
    delete[] pdwAdj;

    SHADOWVERT *pVBData = NULL;
    DWORD *pdwIBData = NULL;

    pInputMesh->LockVertexBuffer( 0, (LPVOID*)&pVBData );
    pInputMesh->LockIndexBuffer( 0, (LPVOID*)&pdwIBData );

    if( pVBData && pdwIBData )
    {
        // Maximum number of unique edges = Number of faces * 3
        DWORD dwNumEdges = pInputMesh->GetNumFaces() * 3;
        CEdgeMapping *pMapping = new CEdgeMapping[dwNumEdges];
        if( pMapping )
        {
            int nNumMaps = 0;  // Number of entries that exist in pMapping

			// leeadd - 220506 - u62 - start reference table
			DWORD dwVertexToCreate = pInputMesh->GetNumFaces() + dwNumEdges * 2;
			if ( g_pdwKeepTrackOfOriginalVertIndex ) delete g_pdwKeepTrackOfOriginalVertIndex;
			g_pdwKeepTrackOfOriginalVertIndex = new DWORD [ dwVertexToCreate ];
			memset ( g_pdwKeepTrackOfOriginalVertIndex, 6553600, dwVertexToCreate*sizeof(DWORD) );

            // Create a new mesh
            ID3DXMesh *pNewMesh;
            hr = D3DXCreateMesh( dwVertexToCreate,
                                 pInputMesh->GetNumFaces() * 3,
                                 D3DXMESH_32BIT,
                                 SHADOWVERT::Decl,
                                 m_pD3D,
                                 &pNewMesh );
            if( SUCCEEDED( hr ) )
            {
                SHADOWVERT *pNewVBData = NULL;
                DWORD *pdwNewIBData = NULL;

                pNewMesh->LockVertexBuffer( 0, (LPVOID*)&pNewVBData );
                pNewMesh->LockIndexBuffer( 0, (LPVOID*)&pdwNewIBData );

                // nNextIndex is the array index in IB that the next vertex index value
                // will be store at.
                int nNextIndex = 0;

                if( pNewVBData && pdwNewIBData )
                {
                    ZeroMemory( pNewVBData, pNewMesh->GetNumVertices() * pNewMesh->GetNumBytesPerVertex() );
                    ZeroMemory( pdwNewIBData, sizeof(DWORD) * pNewMesh->GetNumFaces() * 3 );

                    // pNextOutVertex is the location to write the next
                    // vertex to.
                    SHADOWVERT *pNextOutVertex = pNewVBData;
					DWORD dwKTTheVertPos = 0;

                    // Iterate through the faces.  For each face, output new
                    // vertices and face in the new mesh, and write its edges
                    // to the mapping table.

                    for( UINT f = 0; f < pInputMesh->GetNumFaces(); ++f )
                    {
                        // Copy the vertex data for all 3 vertices
                        CopyMemory( pNextOutVertex, pVBData + pdwIBData[f * 3], sizeof(SHADOWVERT) );
                        CopyMemory( pNextOutVertex + 1, pVBData + pdwIBData[f * 3 + 1], sizeof(SHADOWVERT) );
                        CopyMemory( pNextOutVertex + 2, pVBData + pdwIBData[f * 3 + 2], sizeof(SHADOWVERT) );

						// leeadd - 220506 - u62 - keep track
						g_pdwKeepTrackOfOriginalVertIndex [ dwKTTheVertPos ]	 = pdwIBData[f * 3];
						g_pdwKeepTrackOfOriginalVertIndex [ dwKTTheVertPos + 1 ] = pdwIBData[f * 3 + 1];
						g_pdwKeepTrackOfOriginalVertIndex [ dwKTTheVertPos + 2 ] = pdwIBData[f * 3 + 2];

                        // Write out the face
                        pdwNewIBData[nNextIndex++] = f * 3;
                        pdwNewIBData[nNextIndex++] = f * 3 + 1;
                        pdwNewIBData[nNextIndex++] = f * 3 + 2;

                        // Compute the face normal and assign it to
                        // the normals of the vertices.
                        D3DXVECTOR3 v1, v2;  // v1 and v2 are the edge vectors of the face
                        D3DXVECTOR3 vNormal;
                        v1 = *(D3DXVECTOR3*)(pNextOutVertex + 1) - *(D3DXVECTOR3*)pNextOutVertex;
                        v2 = *(D3DXVECTOR3*)(pNextOutVertex + 2) - *(D3DXVECTOR3*)(pNextOutVertex + 1);
                        D3DXVec3Cross( &vNormal, &v1, &v2 );
                        D3DXVec3Normalize( &vNormal, &vNormal );

                        pNextOutVertex->Normal = vNormal;
                        (pNextOutVertex + 1)->Normal = vNormal;
                        (pNextOutVertex + 2)->Normal = vNormal;

                        pNextOutVertex += 3;
						dwKTTheVertPos += 3;

                        // Add the face's edges to the edge mapping table

                        // Edge 1
                        int nIndex;
                        int nVertIndex[3] = { pdwPtRep[pdwIBData[f * 3]],
                                              pdwPtRep[pdwIBData[f * 3 + 1]],
                                              pdwPtRep[pdwIBData[f * 3 + 2]] };
                        nIndex = FindEdgeInMappingTable( nVertIndex[0], nVertIndex[1], pMapping, dwNumEdges );

                        // If error, we are not able to proceed, so abort.
                        if( -1 == nIndex )
                        {
                            hr = E_INVALIDARG;
                            goto cleanup;
                        }

                        if( pMapping[nIndex].m_anOldEdge[0] == -1 && pMapping[nIndex].m_anOldEdge[1] == -1 )
                        {
                            // No entry for this edge yet.  Initialize one.
                            pMapping[nIndex].m_anOldEdge[0] = nVertIndex[0];
                            pMapping[nIndex].m_anOldEdge[1] = nVertIndex[1];
                            pMapping[nIndex].m_aanNewEdge[0][0] = f * 3;
                            pMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 1;

                            ++nNumMaps;
                        } else
                        {
                            // An entry is found for this edge.  Create
                            // a quad and output it.
//                            assert( nNumMaps > 0 );

                            pMapping[nIndex].m_aanNewEdge[1][0] = f * 3;      // For clarity
                            pMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 1;

                            // First triangle
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];

                            // Second triangle
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];

                            // pMapping[nIndex] is no longer needed. Copy the last map entry
                            // over and decrement the map count.

                            pMapping[nIndex] = pMapping[nNumMaps-1];
                            FillMemory( &pMapping[nNumMaps-1], sizeof( pMapping[nNumMaps-1] ), 0xFF );
                            --nNumMaps;
                        }

                        // Edge 2
                        nIndex = FindEdgeInMappingTable( nVertIndex[1], nVertIndex[2], pMapping, dwNumEdges );

                        // If error, we are not able to proceed, so abort.
                        if( -1 == nIndex )
                        {
                            hr = E_INVALIDARG;
                            goto cleanup;
                        }

                        if( pMapping[nIndex].m_anOldEdge[0] == -1 && pMapping[nIndex].m_anOldEdge[1] == -1 )
                        {
                            pMapping[nIndex].m_anOldEdge[0] = nVertIndex[1];
                            pMapping[nIndex].m_anOldEdge[1] = nVertIndex[2];
                            pMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 1;
                            pMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 2;

                            ++nNumMaps;
                        } else
                        {
                            // An entry is found for this edge.  Create
                            // a quad and output it.
//                            assert( nNumMaps > 0 );

                            pMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 1;
                            pMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 2;

                            // First triangle
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];

                            // Second triangle
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];

                            // pMapping[nIndex] is no longer needed. Copy the last map entry
                            // over and decrement the map count.

                            pMapping[nIndex] = pMapping[nNumMaps-1];
                            FillMemory( &pMapping[nNumMaps-1], sizeof( pMapping[nNumMaps-1] ), 0xFF );
                            --nNumMaps;
                        }

                        // Edge 3
                        nIndex = FindEdgeInMappingTable( nVertIndex[2], nVertIndex[0], pMapping, dwNumEdges );

                        // If error, we are not able to proceed, so abort.
                        if( -1 == nIndex )
                        {
                            hr = E_INVALIDARG;
                            goto cleanup;
                        }

                        if( pMapping[nIndex].m_anOldEdge[0] == -1 && pMapping[nIndex].m_anOldEdge[1] == -1 )
                        {
                            pMapping[nIndex].m_anOldEdge[0] = nVertIndex[2];
                            pMapping[nIndex].m_anOldEdge[1] = nVertIndex[0];
                            pMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 2;
                            pMapping[nIndex].m_aanNewEdge[0][1] = f * 3;

                            ++nNumMaps;
                        } else
                        {
                            // An entry is found for this edge.  Create
                            // a quad and output it.
//                            assert( nNumMaps > 0 );

                            pMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 2;
                            pMapping[nIndex].m_aanNewEdge[1][1] = f * 3;

                            // First triangle
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];

                            // Second triangle
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];

                            // pMapping[nIndex] is no longer needed. Copy the last map entry
                            // over and decrement the map count.

                            pMapping[nIndex] = pMapping[nNumMaps-1];
                            FillMemory( &pMapping[nNumMaps-1], sizeof( pMapping[nNumMaps-1] ), 0xFF );
                            --nNumMaps;
                        }
                    }

                    // Now the entries in the edge mapping table represent
                    // non-shared edges.  What they mean is that the original
                    // mesh has openings (holes), so we attempt to patch them.
                    // First we need to recreate our mesh with a larger vertex
                    // and index buffers so the patching geometry could fit.

   //                DXUTTRACE( L"Faces to patch: %d\n", nNumMaps );

                    // Create a mesh with large enough vertex and
                    // index buffers.

                    SHADOWVERT *pPatchVBData = NULL;
                    DWORD *pdwPatchIBData = NULL;

                    ID3DXMesh *pPatchMesh = NULL;
                    // Make enough room in IB for the face and up to 3 quads for each patching face
                    hr = D3DXCreateMesh( nNextIndex / 3 + nNumMaps * 7,
                                         ( pInputMesh->GetNumFaces() + nNumMaps ) * 3,
                                         D3DXMESH_32BIT,
                                         SHADOWVERT::Decl,
                                         m_pD3D,
                                         &pPatchMesh );

                    if( FAILED( hr ) )
                        goto cleanup;

                    hr = pPatchMesh->LockVertexBuffer( 0, (LPVOID*)&pPatchVBData );
                    if( SUCCEEDED( hr ) )
                        hr = pPatchMesh->LockIndexBuffer( 0, (LPVOID*)&pdwPatchIBData );

                    if( pPatchVBData && pdwPatchIBData )
                    {
                        ZeroMemory( pPatchVBData, sizeof(SHADOWVERT) * ( pInputMesh->GetNumFaces() + nNumMaps ) * 3 );
                        ZeroMemory( pdwPatchIBData, sizeof(DWORD) * ( nNextIndex + 3 * nNumMaps * 7 ) );

                        // Copy the data from one mesh to the other

                        CopyMemory( pPatchVBData, pNewVBData, sizeof(SHADOWVERT) * pInputMesh->GetNumFaces() * 3 );
                        CopyMemory( pdwPatchIBData, pdwNewIBData, sizeof(DWORD) * nNextIndex );
                    } else
                    {
                        // Some serious error is preventing us from locking.
                        // Abort and return error.

                        pPatchMesh->Release();
                        goto cleanup;
                    }

                    // Replace pNewMesh with the updated one.  Then the code
                    // can continue working with the pNewMesh pointer.

                    pNewMesh->UnlockVertexBuffer();
                    pNewMesh->UnlockIndexBuffer();
                    pNewVBData = pPatchVBData;
                    pdwNewIBData = pdwPatchIBData;
                    pNewMesh->Release();
                    pNewMesh = pPatchMesh;

                    // Now, we iterate through the edge mapping table and
                    // for each shared edge, we generate a quad.
                    // For each non-shared edge, we patch the opening
                    // with new faces.

                    // nNextVertex is the index of the next vertex.
                    int nNextVertex = pInputMesh->GetNumFaces() * 3;

                    for( int i = 0; i < nNumMaps; ++i )
                    {
                        if( pMapping[i].m_anOldEdge[0] != -1 &&
                            pMapping[i].m_anOldEdge[1] != -1 )
                        {
                            // If the 2nd new edge indexes is -1,
                            // this edge is a non-shared one.
                            // We patch the opening by creating new
                            // faces.
                            if( pMapping[i].m_aanNewEdge[1][0] == -1 ||  // must have only one new edge
                                pMapping[i].m_aanNewEdge[1][1] == -1 )
                            {
                                // Find another non-shared edge that
                                // shares a vertex with the current edge.
                                for( int i2 = i + 1; i2 < nNumMaps; ++i2 )
                                {
                                    if( pMapping[i2].m_anOldEdge[0] != -1 &&       // must have a valid old edge
                                        pMapping[i2].m_anOldEdge[1] != -1 &&
                                        ( pMapping[i2].m_aanNewEdge[1][0] == -1 || // must have only one new edge
                                        pMapping[i2].m_aanNewEdge[1][1] == -1 ) )
                                    {
                                        int nVertShared = 0;
                                        if( pMapping[i2].m_anOldEdge[0] == pMapping[i].m_anOldEdge[1] )
                                            ++nVertShared;
                                        if( pMapping[i2].m_anOldEdge[1] == pMapping[i].m_anOldEdge[0] )
                                            ++nVertShared;

                                        if( 2 == nVertShared )
                                        {
                                            // These are the last two edges of this particular
                                            // opening. Mark this edge as shared so that a degenerate
                                            // quad can be created for it.

                                            pMapping[i2].m_aanNewEdge[1][0] = pMapping[i].m_aanNewEdge[0][0];
                                            pMapping[i2].m_aanNewEdge[1][1] = pMapping[i].m_aanNewEdge[0][1];
                                            break;
                                        }
                                        else
                                        if( 1 == nVertShared )
                                        {
                                            // nBefore and nAfter tell us which edge comes before the other.
                                            int nBefore, nAfter;
                                            if( pMapping[i2].m_anOldEdge[0] == pMapping[i].m_anOldEdge[1] )
                                            {
                                                nBefore = i;
                                                nAfter = i2;
                                            } else
                                            {
                                                nBefore = i2;
                                                nAfter = i;
                                            }

                                            // Found such an edge. Now create a face along with two
                                            // degenerate quads from these two edges.
                                            pNewVBData[nNextVertex] = pNewVBData[pMapping[nAfter].m_aanNewEdge[0][1]];
                                            pNewVBData[nNextVertex+1] = pNewVBData[pMapping[nBefore].m_aanNewEdge[0][1]];
                                            pNewVBData[nNextVertex+2] = pNewVBData[pMapping[nBefore].m_aanNewEdge[0][0]];

											// leeadd - 220506 - u62 - keep track
											int iIndexIntoNewVertData0 = pMapping[nAfter].m_aanNewEdge[0][1];
											int iIndexIntoNewVertData1 = pMapping[nBefore].m_aanNewEdge[0][1];
											int iIndexIntoNewVertData2 = pMapping[nBefore].m_aanNewEdge[0][0];
											int iOriginalVertFrom0 = g_pdwKeepTrackOfOriginalVertIndex [ iIndexIntoNewVertData0 ];
											int iOriginalVertFrom1 = g_pdwKeepTrackOfOriginalVertIndex [ iIndexIntoNewVertData1 ];
											int iOriginalVertFrom2 = g_pdwKeepTrackOfOriginalVertIndex [ iIndexIntoNewVertData2 ];
											g_pdwKeepTrackOfOriginalVertIndex[dwKTTheVertPos]   = iOriginalVertFrom0;
											g_pdwKeepTrackOfOriginalVertIndex[dwKTTheVertPos+1] = iOriginalVertFrom1;
											g_pdwKeepTrackOfOriginalVertIndex[dwKTTheVertPos+2] = iOriginalVertFrom2;

                                            // Recompute the normal
                                            D3DXVECTOR3 v1 = pNewVBData[nNextVertex+1].Position - pNewVBData[nNextVertex].Position;
                                            D3DXVECTOR3 v2 = pNewVBData[nNextVertex+2].Position - pNewVBData[nNextVertex+1].Position;
                                            D3DXVec3Normalize( &v1, &v1 );
                                            D3DXVec3Normalize( &v2, &v2 );
                                            D3DXVec3Cross( &pNewVBData[nNextVertex].Normal, &v1, &v2 );
                                            pNewVBData[nNextVertex+1].Normal = pNewVBData[nNextVertex+2].Normal = pNewVBData[nNextVertex].Normal;

                                            pdwNewIBData[nNextIndex] = nNextVertex;
                                            pdwNewIBData[nNextIndex+1] = nNextVertex + 1;
                                            pdwNewIBData[nNextIndex+2] = nNextVertex + 2;

                                            // 1st quad

                                            pdwNewIBData[nNextIndex+3] = pMapping[nBefore].m_aanNewEdge[0][1];
                                            pdwNewIBData[nNextIndex+4] = pMapping[nBefore].m_aanNewEdge[0][0];
                                            pdwNewIBData[nNextIndex+5] = nNextVertex + 1;

                                            pdwNewIBData[nNextIndex+6] = nNextVertex + 2;
                                            pdwNewIBData[nNextIndex+7] = nNextVertex + 1;
                                            pdwNewIBData[nNextIndex+8] = pMapping[nBefore].m_aanNewEdge[0][0];

                                            // 2nd quad

                                            pdwNewIBData[nNextIndex+9] = pMapping[nAfter].m_aanNewEdge[0][1];
                                            pdwNewIBData[nNextIndex+10] = pMapping[nAfter].m_aanNewEdge[0][0];
                                            pdwNewIBData[nNextIndex+11] = nNextVertex;

                                            pdwNewIBData[nNextIndex+12] = nNextVertex + 1;
                                            pdwNewIBData[nNextIndex+13] = nNextVertex;
                                            pdwNewIBData[nNextIndex+14] = pMapping[nAfter].m_aanNewEdge[0][0];

                                            // Modify mapping entry i2 to reflect the third edge
                                            // of the newly added face.

                                            if( pMapping[i2].m_anOldEdge[0] == pMapping[i].m_anOldEdge[1] )
                                            {
                                                pMapping[i2].m_anOldEdge[0] = pMapping[i].m_anOldEdge[0];
                                            } else
                                            {
                                                pMapping[i2].m_anOldEdge[1] = pMapping[i].m_anOldEdge[1];
                                            }
                                            pMapping[i2].m_aanNewEdge[0][0] = nNextVertex + 2;
                                            pMapping[i2].m_aanNewEdge[0][1] = nNextVertex;

                                            // Update next vertex/index positions

                                            nNextVertex += 3;
											dwKTTheVertPos += 3;
                                            nNextIndex += 15;

                                            break;
                                        }
                                    }
                                }
                            } else
                            {
                                // This is a shared edge.  Create the degenerate quad.

                                // First triangle
                                pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[0][1];
                                pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[0][0];
                                pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[1][0];

                                // Second triangle
                                pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[1][1];
                                pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[1][0];
                                pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[0][0];
                            }
                        }
                    }
                }

cleanup:;
                if( pNewVBData )
                {
                    pNewMesh->UnlockVertexBuffer();
                    pNewVBData = NULL;
                }
                if( pdwNewIBData )
                {
                    pNewMesh->UnlockIndexBuffer();
                    pdwNewIBData = NULL;
                }

                if( SUCCEEDED( hr ) )
                {
                    // At this time, the output mesh may have an index buffer
                    // bigger than what is actually needed, so we create yet
                    // another mesh with the exact IB size that we need and
                    // output it.  This mesh also uses 16-bit index if
                    // 32-bit is not necessary.

   //                 DXUTTRACE( L"Shadow volume has %u vertices, %u faces.\n", ( pInputMesh->GetNumFaces() + nNumMaps ) * 3, nNextIndex / 3 );

                    bool bNeed32Bit = ( pInputMesh->GetNumFaces() + nNumMaps ) * 3 > 65535;
                    ID3DXMesh *pFinalMesh;
                    hr = D3DXCreateMesh( nNextIndex / 3,  // Exact number of faces
                                         ( pInputMesh->GetNumFaces() + nNumMaps ) * 3,
                                         D3DXMESH_WRITEONLY | ( bNeed32Bit ? D3DXMESH_32BIT : 0 ),
                                         SHADOWVERT::Decl,
                                         m_pD3D,
                                         &pFinalMesh );
                    if( SUCCEEDED( hr ) )
                    {
                        pNewMesh->LockVertexBuffer( 0, (LPVOID*)&pNewVBData );
                        pNewMesh->LockIndexBuffer( 0, (LPVOID*)&pdwNewIBData );

                        SHADOWVERT *pFinalVBData = NULL;
                        WORD *pwFinalIBData = NULL;

                        pFinalMesh->LockVertexBuffer( 0, (LPVOID*)&pFinalVBData );
                        pFinalMesh->LockIndexBuffer( 0, (LPVOID*)&pwFinalIBData );

                        if( pNewVBData && pdwNewIBData && pFinalVBData && pwFinalIBData )
                        {
                            CopyMemory( pFinalVBData, pNewVBData, sizeof(SHADOWVERT) * ( pInputMesh->GetNumFaces() + nNumMaps ) * 3 );

                            if( bNeed32Bit )
                                CopyMemory( pwFinalIBData, pdwNewIBData, sizeof(DWORD) * nNextIndex );
                            else
                            {
                                for( int i = 0; i < nNextIndex; ++i )
                                    pwFinalIBData[i] = (WORD)pdwNewIBData[i];
                            }
                        }

                        if( pNewVBData )
                            pNewMesh->UnlockVertexBuffer();
                        if( pdwNewIBData )
                            pNewMesh->UnlockIndexBuffer();
                        if( pFinalVBData )
                            pFinalMesh->UnlockVertexBuffer();
                        if( pwFinalIBData )
                            pFinalMesh->UnlockIndexBuffer();

                        // Release the old
                        pNewMesh->Release();
                        pNewMesh = pFinalMesh;
                    }

                    *ppOutMesh = pNewMesh;
                }
                else
                    pNewMesh->Release();
            }
            delete[] pMapping;
        } else
            hr = E_OUTOFMEMORY;
    } else
        hr = E_FAIL;

    if( pVBData )
        pInputMesh->UnlockVertexBuffer();

    if( pdwIBData )
        pInputMesh->UnlockIndexBuffer();

    delete[] pdwPtRep;
    pInputMesh->Release();

    return hr;
}

DARKSDK_DLL bool CreateShadowMesh ( sMesh* pMesh, sMesh** ppShadowMesh, int iUseShader )
{
	// get mesh
	if ( pMesh )
	{
		// reset mesh before submit for shadow generation (must match original vertex positions when done)
		ResetVertexDataInMeshPerMesh ( pMesh );

		// convert mesh to DXmesh
		LPD3DXMESH pMeshIn = LocalMeshToDXMesh ( pMesh, NULL, pMesh->dwFVFOriginal );
		if ( pMeshIn==NULL )
			return false;

		// create a shadow volume mesh for shader stencil shadows
		ID3DXMesh* pMeshOut = NULL;
		if ( GenerateDXShadowVolumeMeshForShader ( pMeshIn, &pMeshOut )!=S_OK )
		{
			SAFE_RELEASE ( pMeshIn );
			return false;
		}

		// convert DXmesh to mesh
		if ( *ppShadowMesh==NULL ) *ppShadowMesh = new sMesh;
		UpdateLocalMeshWithDXMesh ( *ppShadowMesh, pMeshOut );

		// delete dxmesh
		SAFE_RELEASE ( pMeshOut );
		SAFE_RELEASE ( pMeshIn );

		// setup mesh drawing properties
		if ( *ppShadowMesh )
		{
			(*ppShadowMesh)->iPrimitiveType   = D3DPT_TRIANGLELIST;
			(*ppShadowMesh)->iDrawVertexCount = (*ppShadowMesh)->dwVertexCount;
			if ( (*ppShadowMesh)->dwIndexCount==0 )
				(*ppShadowMesh)->iDrawPrimitives  = (*ppShadowMesh)->dwVertexCount/3;
			else
				(*ppShadowMesh)->iDrawPrimitives  = (*ppShadowMesh)->dwIndexCount/3;
		}

		// shadow FX static mesh only
		if ( iUseShader==1 )
		{
			// standard static no bone shader for shadows
			LPSTR pStencilShadowFX = "stencilshadow.fx";

			// if original mesh has 'bone' animation, reproduce in shadow mesh
			if ( pMesh->dwBoneCount > 0 )
			{
				// get offset map of original
				sOffsetMap offsetMap;
				GetFVFOffsetMap ( pMesh, &offsetMap );
				sOffsetMap offsetNewMap;
				GetFVFOffsetMap ( (*ppShadowMesh), &offsetNewMap );

				// make a bone array for the shadow mesh
				float fEpsilon = 0.01f;
				(*ppShadowMesh)->dwBoneCount = pMesh->dwBoneCount;
				(*ppShadowMesh)->pBones = new sBone [ pMesh->dwBoneCount ];
				for ( DWORD b=0; b<pMesh->dwBoneCount; b++ )
				{
					// for each bone, count pass and create pass
					for ( int pass=0; pass<2; pass++ )
					{
						// work out size of influences in shadow mesh
						DWORD dwNumNewInfluences = 0;
						DWORD dwInfluences = pMesh->pBones [ b ].dwNumInfluences;
						for ( DWORD i=0; i<dwInfluences; i++ )
						{
							// get vertex index from the original mesh (which the bone anim performed on)
							int iCurrentVertex = pMesh->pBones [ b ].pVertices [ i ];

							// leeadd - 220506 - u62 - uses 'g_pdwKeepTrackOfOriginalVertIndex' ref table
							D3DXVECTOR3 vPos = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) );
							for ( DWORD v=0; v<(*ppShadowMesh)->dwVertexCount; v++ )
							{
								// we can look for all the verts in the shadow mesh which ONCE where verts from the original model
								if ( v < pMesh->dwVertexCount )
								{
									// fast and predictable finding the raw model geom
									if ( (int)(g_pdwKeepTrackOfOriginalVertIndex[v]) == iCurrentVertex )
									{
										// vertex in shadow mesh matches original vertex
										if ( pass==1 )
										{
											// fill in on second pass
											(*ppShadowMesh)->pBones [ b ].pVertices [ dwNumNewInfluences ] = v;
											(*ppShadowMesh)->pBones [ b ].pWeights [ dwNumNewInfluences ] = pMesh->pBones [ b ].pWeights [ i ];
										}
										dwNumNewInfluences++;
									}
								}
								else
								{
									// count occurances of this vertex position in new shadow mesh
									// look for vertex position match
									D3DXVECTOR3 vNewPos = *(D3DXVECTOR3*)( ( float* ) (*ppShadowMesh)->pVertexData + offsetNewMap.dwX + ( offsetNewMap.dwSize * v ) );
									if ( fabs(vNewPos.x-vPos.x) < fEpsilon
									&&   fabs(vNewPos.y-vPos.y) < fEpsilon
									&&   fabs(vNewPos.z-vPos.z) < fEpsilon )
									{
										// vertex in shadow mesh matches original vertex
										if ( pass==1 )
										{
											// fill in on second pass
											(*ppShadowMesh)->pBones [ b ].pVertices [ dwNumNewInfluences ] = v;
											(*ppShadowMesh)->pBones [ b ].pWeights [ dwNumNewInfluences ] = pMesh->pBones [ b ].pWeights [ i ];
										}
										dwNumNewInfluences++;
									}
								}
							}

							/* pre-220506 - u60 method - very slow indeed
							// get vertex index (old)
							int iCurrentVertex = pMesh->pBones [ b ].pVertices [ i ];
							D3DXVECTOR3 vPos = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) );
							// count occurances of this vertex position in new shadow mesh
							for ( DWORD v=0; v<(*ppShadowMesh)->dwVertexCount; v++ )
							{
								// look for vertex position match
								D3DXVECTOR3 vNewPos = *(D3DXVECTOR3*)( ( float* ) (*ppShadowMesh)->pVertexData + offsetNewMap.dwX + ( offsetNewMap.dwSize * v ) );
								if ( fabs(vNewPos.x-vPos.x) < fEpsilon
								&&   fabs(vNewPos.y-vPos.y) < fEpsilon
								&&   fabs(vNewPos.z-vPos.z) < fEpsilon )
								{
									// vertex in shadow mesh matches original vertex
									if ( pass==1 )
									{
										// fill in on second pass
										(*ppShadowMesh)->pBones [ b ].pVertices [ dwNumNewInfluences ] = v;
										(*ppShadowMesh)->pBones [ b ].pWeights [ dwNumNewInfluences ] = pMesh->pBones [ b ].pWeights [ i ];
									}
									dwNumNewInfluences++;
								}
							}
							*/
						}
						if ( pass==0 )
						{
							// copy new larger influence count into shadow mesh influence count
							(*ppShadowMesh)->pBones [ b ].dwNumInfluences = dwNumNewInfluences;
							(*ppShadowMesh)->pBones [ b ].pVertices = new DWORD [ dwNumNewInfluences ];
							(*ppShadowMesh)->pBones [ b ].pWeights = new float [ dwNumNewInfluences ];
						}
						if ( pass==1 )
						{
							// complete data within new bone array
							strcpy ( (*ppShadowMesh)->pBones [ b ].szName, pMesh->pBones [ b ].szName );
							(*ppShadowMesh)->pBones [ b ].matTranslation = pMesh->pBones [ b ].matTranslation;
						}
					}
				}

				// finished with table
				SAFE_DELETE ( g_pdwKeepTrackOfOriginalVertIndex );

				// use bone animated shader instead
				pStencilShadowFX = "stencilshadowbone.fx";
			}

			// Create external shadow FX effect
			if ( *ppShadowMesh )
			{
				cSpecialEffect* pEffectObj = new cExternalEffect;
				pEffectObj->Load ( pStencilShadowFX, false, false );
				if ( !SetSpecialEffect ( *ppShadowMesh, pEffectObj ) )
				{
					SAFE_DELETE(pEffectObj);
					iUseShader = 0;
				}
			}
		}

		// if shader failed for any reason CPU backup
		if ( iUseShader==0 )
		{
			// range of -1 means no shader
			(*ppShadowMesh)->Collision.fLargestRadius = -1;

			// store shadow once calculated
			SAFE_DELETE ( (*ppShadowMesh)->pOriginalVertexData );
			CollectOriginalVertexData ( *ppShadowMesh );

			// update shadow mesh as a default
			D3DXMATRIX matDefault;
			D3DXMatrixIdentity ( &matDefault );
			if ( !UpdateShadowMesh ( 0.0f, 0.0f, 0.0f, -1, pMesh, *ppShadowMesh, &matDefault, NULL ) )
				return false;
		}
	}

	// okay
	return true;
}

/* pre-2006 old CPU/GPU shadow division - now just GPU based even in software
DARKSDK_DLL bool CreateShadowMesh ( sMesh* pMesh, sMesh** ppShadowMesh, int iUseShader )
{
	// get mesh
	if ( pMesh )
	{
		// shader shadow
		if ( iUseShader==1 )
		{
			// convert mesh to DXmesh
			LPD3DXMESH pMeshIn = LocalMeshToDXMesh ( pMesh, NULL, pMesh->dwFVFOriginal );
			if ( pMeshIn==NULL )
				return false;

			// create a shadow volume mesh for shader stencil shadows
			ID3DXMesh* pMeshOut = NULL;
			if ( GenerateDXShadowVolumeMeshForShader ( pMeshIn, &pMeshOut )!=S_OK )
			{
				SAFE_RELEASE ( pMeshIn );
				return false;
			}

			// convert DXmesh to mesh
			*ppShadowMesh = new sMesh;
			UpdateLocalMeshWithDXMesh ( *ppShadowMesh, pMeshOut );

			// delete dxmesh
			SAFE_RELEASE ( pMeshOut );
			SAFE_RELEASE ( pMeshIn );

			// setup mesh drawing properties
			if ( *ppShadowMesh )
			{
				(*ppShadowMesh)->iPrimitiveType   = D3DPT_TRIANGLELIST;
				(*ppShadowMesh)->iDrawVertexCount = (*ppShadowMesh)->dwVertexCount;
				if ( (*ppShadowMesh)->dwIndexCount==0 )
					(*ppShadowMesh)->iDrawPrimitives  = (*ppShadowMesh)->dwVertexCount/3;
				else
					(*ppShadowMesh)->iDrawPrimitives  = (*ppShadowMesh)->dwIndexCount/3;
			}

			// shadow FX static mesh only
			LPSTR pStencilShadowFX = "stencilshadow.fx";
			// if original mesh has 'bone' animation, reproduce in shadow mesh
			if ( pMesh->dwBoneCount == 12345 )
			{
				// disabled until animated shadows added to DX SDK
				// get offset map of original
				sOffsetMap offsetMap;
				GetFVFOffsetMap ( pMesh, &offsetMap );
				sOffsetMap offsetNewMap;
				GetFVFOffsetMap ( (*ppShadowMesh), &offsetNewMap );

				// make a bone array for the shadow mesh
				float fEpsilon = 0.01f;
				(*ppShadowMesh)->dwBoneCount = pMesh->dwBoneCount;
				(*ppShadowMesh)->pBones = new sBone [ pMesh->dwBoneCount ];
				for ( DWORD b=0; b<pMesh->dwBoneCount; b++ )
				{
					// for each bone, count pass and create pass
					for ( int pass=0; pass<2; pass++ )
					{
						// work out size of influences in shadow mesh
						DWORD dwNumNewInfluences = 0;
						DWORD dwInfluences = pMesh->pBones [ b ].dwNumInfluences;
						for ( DWORD i=0; i<dwInfluences; i++ )
						{
							// get vertex index (old)
							int iCurrentVertex = pMesh->pBones [ b ].pVertices [ i ];
							D3DXVECTOR3 vPos = *(D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) );

							// count occurances of this vertex position in new shadow mesh
							for ( DWORD v=0; v<(*ppShadowMesh)->dwVertexCount; v++ )
							{
								// look for vertex position match
								D3DXVECTOR3 vNewPos = *(D3DXVECTOR3*)( ( float* ) (*ppShadowMesh)->pVertexData + offsetNewMap.dwX + ( offsetNewMap.dwSize * v ) );
								if ( fabs(vNewPos.x-vPos.x) < fEpsilon
								&&   fabs(vNewPos.y-vPos.y) < fEpsilon
								&&   fabs(vNewPos.z-vPos.z) < fEpsilon )
								{
									// vertex in shadow mesh matches original vertex
									if ( pass==1 )
									{
										// fill in on second pass
										(*ppShadowMesh)->pBones [ b ].pVertices [ dwNumNewInfluences ] = v;
										(*ppShadowMesh)->pBones [ b ].pWeights [ dwNumNewInfluences ] = pMesh->pBones [ b ].pWeights [ i ];
									}
									dwNumNewInfluences++;
								}
							}
						}
						if ( pass==0 )
						{
							// copy new larger influence count into shadow mesh influence count
							(*ppShadowMesh)->pBones [ b ].dwNumInfluences = dwNumNewInfluences;
							(*ppShadowMesh)->pBones [ b ].pVertices = new DWORD [ dwNumNewInfluences ];
							(*ppShadowMesh)->pBones [ b ].pWeights = new float [ dwNumNewInfluences ];
						}
						if ( pass==1 )
						{
							// complete data within new bone array
							strcpy ( (*ppShadowMesh)->pBones [ b ].szName, pMesh->pBones [ b ].szName );
							(*ppShadowMesh)->pBones [ b ].matTranslation = pMesh->pBones [ b ].matTranslation;
						}
					}
				}

				// use bone animated shader instead
				pStencilShadowFX = "stencilshadowbone.fx";
			}

			// Create external shadow FX effect
			if ( *ppShadowMesh )
			{
				cSpecialEffect* pEffectObj = new cExternalEffect;
				pEffectObj->Load ( pStencilShadowFX, false, false );
				if ( !SetSpecialEffect ( *ppShadowMesh, pEffectObj ) )
				{
					SAFE_DELETE(pEffectObj);
					return false;
				}
			}
		}
		else
		{
			// first modify model mesh to regular tri-list format
			ConvertLocalMeshToTriList ( pMesh );

			// determine size of new shadow vertex buffer
			DWORD dwIndexCount		= pMesh->dwIndexCount;
			DWORD dwFaces			= dwIndexCount / 3;
			DWORD dwEdges			= dwFaces * 3;
			DWORD dwVertexCount		= dwEdges * 6;

			// leefix-040803-if no index data, proceed with vertex data only
			if ( dwIndexCount==0 )
			{
				dwFaces			= pMesh->dwVertexCount / 3;
				dwEdges			= dwFaces * 3;
				dwVertexCount	= dwEdges * 6;
			}

			// add extra vertices for capping of shadow mesh
			dwVertexCount		+= (dwEdges-2); // front cap
			dwVertexCount		+= (dwEdges-2); // back cap

			// reject if cannot support mesh size
			D3DCAPS9 caps;
			m_pD3D->GetDeviceCaps ( &caps );
			if ( dwVertexCount > caps.MaxVertexIndex )
				return false;

			// shadow mesh format
			DWORD dwFVF				= D3DFVF_XYZ;

			// free any existing shadow mesh
			SAFE_DELETE( *ppShadowMesh );

			// create a shadow mesh
			*ppShadowMesh = new sMesh;
			if ( !*ppShadowMesh )
				return false;

			// setup the shadow mesh (no index data required)
			if ( !SetupMeshFVFData ( *ppShadowMesh, dwFVF, dwVertexCount, 0 ) )
				return false;

			// range of -1 means no shader
			(*ppShadowMesh)->Collision.fLargestRadius = -1;

			// update shadow mesh as a default
			D3DXMATRIX matDefault;
			D3DXMatrixIdentity ( &matDefault );
			if ( !UpdateShadowMesh ( 0.0f, 0.0f, 0.0f, -1, pMesh, *ppShadowMesh, &matDefault, NULL ) )
				return false;
		}
	}

	// okay
	return true;
}

/* pre-2006 using DX shadow-volume even in CPU now
DARKSDK_DLL VOID AddEdge ( WORD* pEdges, DWORD& dwNumEdges, WORD v0, WORD v1 )
{
	for ( DWORD i = 0; i < dwNumEdges; i++ )
	{
		if ( ( pEdges [ 2 * i + 0 ] == v0 && pEdges [ 2 * i + 1 ] == v1 ) ||
			 ( pEdges [ 2 * i + 0 ] == v1 && pEdges [ 2 * i + 1 ] == v0 ) )
		{
			if ( dwNumEdges > 1 )
			{
				pEdges [ 2 * i + 0 ] = pEdges [ 2 * ( dwNumEdges - 1 ) + 0 ];
				pEdges [ 2 * i + 1 ] = pEdges [ 2 * ( dwNumEdges - 1 ) + 1 ];
			}

			dwNumEdges--;
			return;
		}
	}

	pEdges [ 2 * dwNumEdges + 0 ] = v0;
	pEdges [ 2 * dwNumEdges + 1 ] = v1;

	dwNumEdges++;
}

DARKSDK_DLL bool CastShadowGeometryToShadowMesh ( D3DXVECTOR3 vecLightPos, sMesh* pCasterMesh, float fShadowRange, sMesh* pShadowMesh, DWORD* pdwStartAtVertex, D3DXMATRIX* pmatWorld, D3DXMATRIX* pmatBoneMatrix )
{
	// world mesh (transformed)
	if ( pCasterMesh && pShadowMesh )
	{
		// if fShadowRange is -1, default shadow CPU range is 1000
		if ( fShadowRange==-1 ) fShadowRange=2000.0f;

		// position of the world light
		float Lx, Ly, Lz;
		Lx = vecLightPos.x;
		Ly = vecLightPos.y;
		Lz = vecLightPos.z;
		
		// transform the light vector to be in object space
		D3DXVECTOR3 vLight = D3DXVECTOR3 ( Lx, Ly, Lz );
		if ( pmatWorld )
		{
			D3DXMATRIX  m;
			D3DXMatrixInverse ( &m, NULL, pmatWorld );
			vLight.x = Lx * m._11 + Ly * m._21 + Lz * m._31 + m._41;
			vLight.y = Lx * m._12 + Ly * m._22 + Lz * m._32 + m._42;
			vLight.z = Lx * m._13 + Ly * m._23 + Lz * m._33 + m._43;
		}

		// get the offset map for the FVF
		sOffsetMap offsetMap;
		GetFVFOffsetMap ( pCasterMesh, &offsetMap );

		// determine number of faces
		DWORD dwNumFaces = pCasterMesh->dwIndexCount/3;
		if ( pCasterMesh->dwIndexCount==0 )
			dwNumFaces = pCasterMesh->dwVertexCount/3;

		// generate new shadow mesh vertex data
		WORD* pEdges = new WORD [ dwNumFaces * 6 ];
		DWORD dwNumEdges = 0;

		// creates edges
		float* pBase = (float*)pCasterMesh->pVertexData+offsetMap.dwX;
		
		DWORD i = 0;
		
		for ( i = 0; i < dwNumFaces; i++ )
		{
			// get face vectors
			WORD wFace0, wFace1, wFace2;
			if ( pCasterMesh->pIndices )
			{
				// from index list
				wFace0 = pCasterMesh->pIndices [ 3 * i + 0 ];
				wFace1 = pCasterMesh->pIndices [ 3 * i + 1 ];
				wFace2 = pCasterMesh->pIndices [ 3 * i + 2 ];
			}
			else
			{
				// from sequence of vertex triangles
				wFace0 = (WORD)(3 * i + 0);
				wFace1 = (WORD)(3 * i + 1);
				wFace2 = (WORD)(3 * i + 2);
			}

			D3DXVECTOR3 v0 = *(D3DXVECTOR3*)((float*)pBase+(wFace0*offsetMap.dwSize));
			D3DXVECTOR3 v1 = *(D3DXVECTOR3*)((float*)pBase+(wFace1*offsetMap.dwSize));
			D3DXVECTOR3 v2 = *(D3DXVECTOR3*)((float*)pBase+(wFace2*offsetMap.dwSize));

			// calculate edge from face vectors
			D3DXVECTOR3 vNormal;
			D3DXVec3Cross ( &vNormal, &( v2 - v1 ), &( v1 - v0 ) );
			float fDotProduct = D3DXVec3Dot ( &vNormal, &vLight );
			if ( fDotProduct > 0.0f )
			{
				AddEdge ( pEdges, dwNumEdges, wFace0, wFace1 );
				AddEdge ( pEdges, dwNumEdges, wFace1, wFace2 );
				AddEdge ( pEdges, dwNumEdges, wFace2, wFace0 );
			}
		}

		// create front shadow cap hook by finding furthest vector from light
		D3DXVECTOR3 vFrontFirst = pCasterMesh->Collision.vecColCenter;
		if ( pmatBoneMatrix ) D3DXVec3TransformCoord ( &vFrontFirst, &vFrontFirst, pmatBoneMatrix );

		// create back shadow cap from front
		D3DXVECTOR3 vBackFirst = (vFrontFirst - vLight);
		D3DXVec3Normalize ( &vBackFirst, &vBackFirst );
		vBackFirst = vBackFirst * fShadowRange;

		// vertex filling
		DWORD dwNumVertices = *pdwStartAtVertex;

		// create shadow from edges
		for ( i = 0; i < dwNumEdges; i++ )
		{
			// determine edge vectors
			WORD wFace = pEdges [ 2 * i + 0 ];
			D3DXVECTOR3 v1 = *(D3DXVECTOR3*)((float*)pBase+(wFace*offsetMap.dwSize));
			wFace = pEdges [ 2 * i + 1 ];
			D3DXVECTOR3 v2 = *(D3DXVECTOR3*)((float*)pBase+(wFace*offsetMap.dwSize));

			// transform vector by frame matrix (only for limb based frames)
			if ( pmatBoneMatrix )
			{
				D3DXVec3TransformCoord ( &v1, &v1, pmatBoneMatrix );
				D3DXVec3TransformCoord ( &v2, &v2, pmatBoneMatrix );
			}

			// transform v3 and v4 by light to get streaming line for shadow
			D3DXVECTOR3 v3 = (v1 - vLight);
			D3DXVECTOR3 v4 = (v2 - vLight);
			D3DXVec3Normalize ( &v3, &v3 );
			D3DXVec3Normalize ( &v4, &v4 );
			v3 = v3 * fShadowRange;
			v4 = v4 * fShadowRange;

			// creates shadow vertex data
			*((D3DXVECTOR3*)pShadowMesh->pVertexData+dwNumVertices) = v1;
			dwNumVertices++;
			*((D3DXVECTOR3*)pShadowMesh->pVertexData+dwNumVertices) = v2;
			dwNumVertices++;
			*((D3DXVECTOR3*)pShadowMesh->pVertexData+dwNumVertices) = v3;
			dwNumVertices++;
			*((D3DXVECTOR3*)pShadowMesh->pVertexData+dwNumVertices) = v2;
			dwNumVertices++;
			*((D3DXVECTOR3*)pShadowMesh->pVertexData+dwNumVertices) = v4;
			dwNumVertices++;
			*((D3DXVECTOR3*)pShadowMesh->pVertexData+dwNumVertices) = v3;
			dwNumVertices++;

			// creates cap at front end (prevents obj being shadowed)
			*((D3DXVECTOR3*)pShadowMesh->pVertexData+dwNumVertices) = vFrontFirst;
			dwNumVertices++;
			*((D3DXVECTOR3*)pShadowMesh->pVertexData+dwNumVertices) = v2;
			dwNumVertices++;
			*((D3DXVECTOR3*)pShadowMesh->pVertexData+dwNumVertices) = v1;
			dwNumVertices++;

			// creates cap at back end (allows depth-X system to work)
			*((D3DXVECTOR3*)pShadowMesh->pVertexData+dwNumVertices) = vBackFirst;
			dwNumVertices++;
			*((D3DXVECTOR3*)pShadowMesh->pVertexData+dwNumVertices) = v3;
			dwNumVertices++;
			*((D3DXVECTOR3*)pShadowMesh->pVertexData+dwNumVertices) = v4;
			dwNumVertices++;
		}

		// end of vertex filling
		*pdwStartAtVertex = dwNumVertices;

		// store number of renderable vertices
		pShadowMesh->bVBRefreshRequired = true;
		pShadowMesh->iDrawVertexCount = dwNumVertices;
		pShadowMesh->iDrawPrimitives = dwNumVertices/3;

		// free usages
		SAFE_DELETE_ARRAY ( pEdges );
	}

	// complete
	return true;
}

DARKSDK_DLL bool UpdateShadowMesh ( float fX, float fY, float fZ, float fRange, sMesh* pMesh, sMesh* pShadowMesh, D3DXMATRIX* pmatWorld, D3DXMATRIX* pmatCombined )
{
	// get frame mesh (CPU only for the moment)
	if ( pMesh && pShadowMesh )
	{
		// update shadow to entire frame mesh
		DWORD dwVertexStartFrom = 0;
		D3DXVECTOR3 LightDir = D3DXVECTOR3 ( fX, fY, fZ );
		CastShadowGeometryToShadowMesh ( LightDir, pMesh, fRange, pShadowMesh, &dwVertexStartFrom, pmatWorld, pmatCombined );
	}

	// okay
	return true;
}
*/

DARKSDK_DLL bool CastShadowGeometryToShadowMesh ( D3DXVECTOR3 vecLightPos, sMesh* pOrigMesh, float fShadowRange, sMesh** ppMesh, DWORD* pdwStartAtVertex, D3DXMATRIX* pmatWorld, D3DXMATRIX* pmatBoneMatrix )
{
	// copy all orig vertex data (being animated) to shadow mesh
	sMesh* pMesh = *ppMesh;
	sOffsetMap offsetOrigMap;
	GetFVFOffsetMap ( pOrigMesh, &offsetOrigMap );
	sOffsetMap offsetShadowMap;
	GetFVFOffsetMap ( pMesh, &offsetShadowMap );

	// animate shadow mesh (based on the true pOrigMesh mesh)
	if ( pOrigMesh->dwBoneCount > 0 )
	{
		// matrice temps for bone calculations
		D3DXMATRIX* matrices = new D3DXMATRIX [ pOrigMesh->dwBoneCount ];

		// weight table for making shadow mesh weights properly
		BYTE* weighttable = new BYTE [ pMesh->dwVertexCount ];
		memset ( weighttable, 0, pMesh->dwVertexCount );

		// for each bone, the iterative process should only affect the dest vert once!
		BYTE* affectvertonceperbone = new BYTE [ pMesh->dwVertexCount ];

		// update all bone matrices
		for ( DWORD dwMatrixIndex = 0; dwMatrixIndex < pOrigMesh->dwBoneCount; dwMatrixIndex++ )
			D3DXMatrixMultiply ( &matrices [ dwMatrixIndex ], &pOrigMesh->pBones [ dwMatrixIndex ].matTranslation, pOrigMesh->pFrameMatrices [ dwMatrixIndex ] );

		// run through all bones
		for ( int iBone = 0; iBone < ( int ) pOrigMesh->dwBoneCount; iBone++ )
		{
			// for each bone, dest verts can only be dealt with once
			memset ( affectvertonceperbone, 0, pMesh->dwVertexCount );

			// go through all influenced bones
			for ( int iLoop = 0; iLoop < ( int ) pOrigMesh->pBones [ iBone ].dwNumInfluences; iLoop++ )
			{
				// get the correct vertex and weight
				int iOffset = pOrigMesh->pBones [ iBone ].pVertices [ iLoop ];
				float fWeight = pOrigMesh->pBones [ iBone ].pWeights [ iLoop ];

				// at this point, we want to change the SHADOW MESH, not the original mesh again, 
				// so find ALL vertices in shadow mesh that represent this original vertex position
				D3DXVECTOR3* pOriginalVertexBase = (D3DXVECTOR3*)((float*)(pOrigMesh->pOriginalVertexData + ( pOrigMesh->dwFVFSize * iOffset )));
				for ( DWORD v=0; v<pMesh->dwVertexCount; v++ )
				{
					// look for match
					D3DXVECTOR3 vNewPos = *(D3DXVECTOR3*)( ( float* ) pMesh->pOriginalVertexData + offsetShadowMap.dwX + ( offsetShadowMap.dwSize * v ) );
					if ( vNewPos.x==pOriginalVertexBase->x && vNewPos.y==pOriginalVertexBase->y && vNewPos.z==pOriginalVertexBase->z )
					{
						if ( affectvertonceperbone [ v ]==0 )
						{
							// only deal once per bone for dest verts
							affectvertonceperbone [ v ] = 1;

							// this shadow vert matches animating original vert
							float* pDestVertexBase = (float*)(pMesh->pVertexData + ( pMesh->dwFVFSize * v ));
							float* pVertexBase = (float*)(pMesh->pOriginalVertexData + ( pMesh->dwFVFSize * v ));

							// clear new vertex if changing it
							if ( weighttable [ v ]==0 )
							{
								memset ( pDestVertexBase, 0, 12 );
								if ( pOrigMesh->dwFVF | D3DFVF_NORMAL )
									memset ( pDestVertexBase+3, 0, 12 );
							}
							weighttable [ v ] = 1;
						
							// get original vertex position
							D3DXVECTOR3 vec = D3DXVECTOR3 ( *(pVertexBase+0), *(pVertexBase+1), *(pVertexBase+2) );

							// multiply the vector and the bone matrix with weight
							D3DXVECTOR3 newVec = MultiplyVectorAndMatrix ( vec, matrices [ iBone ] ) * fWeight;
							
							// accumilate vertex for final result
							*(pDestVertexBase+0) += newVec.x;
							*(pDestVertexBase+1) += newVec.y;
							*(pDestVertexBase+2) += newVec.z;

							// transform and normalise the normals vector (if present)
							if ( pMesh->dwFVF | D3DFVF_NORMAL )
							{
								D3DXVECTOR3 norm = D3DXVECTOR3 ( *(pVertexBase+3), *(pVertexBase+4), *(pVertexBase+5) );
								D3DXVECTOR3 newNorm;
								D3DXVec3TransformNormal ( &newNorm, &norm, &matrices [ iBone ] );
								newNorm = newNorm * fWeight;
									
								// update the normal with the new normal values
								*(pDestVertexBase+3) += newNorm.x;
								*(pDestVertexBase+4) += newNorm.y;
								*(pDestVertexBase+5) += newNorm.z;
							}
						}
					}
				}
			}
		}

		// free matrix bank
		SAFE_DELETE_ARRAY ( matrices );
		SAFE_DELETE_ARRAY ( weighttable );
		SAFE_DELETE_ARRAY ( affectvertonceperbone );
	}
	else
	{
		// Simply copy original for new shadow modify
		ResetVertexDataInMeshPerMesh ( pMesh );
	}

	// fShadowRange is -1, default shadow CPU range is 1000
	if ( fShadowRange==-1 ) fShadowRange=2000.0f;

	// position of the world light
	float Lx, Ly, Lz;
	Lx = vecLightPos.x;
	Ly = vecLightPos.y;
	Lz = vecLightPos.z;
	
	// transform the light vector to be in object space
	D3DXVECTOR3 vLight = D3DXVECTOR3 ( Lx, Ly, Lz );
	if ( pmatWorld )
	{
		D3DXMATRIX  m;
		D3DXMatrixInverse ( &m, NULL, pmatWorld );
		vLight.x = Lx * m._11 + Ly * m._21 + Lz * m._31 + m._41;
		vLight.y = Lx * m._12 + Ly * m._22 + Lz * m._32 + m._42;
		vLight.z = Lx * m._13 + Ly * m._23 + Lz * m._33 + m._43;
	}

	// simulates stencilshadow.fx on the CPU
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// process vertex extrude
	for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
	{
		if ( offsetMap.dwZ>0 )
		{
			// get vertex
			D3DXVECTOR3* pVertexPos = (D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) );
			D3DXVECTOR3* pNormalPos = (D3DXVECTOR3*)( ( float* ) pMesh->pVertexData + offsetMap.dwNX + ( offsetMap.dwSize * iVertex ) );

//			// transform vector by frame matrix (only for limb based frames)
//			if ( pmatBoneMatrix )
//				D3DXVec3TransformCoord ( pVertexPos, pVertexPos, pmatBoneMatrix );

			// extrude or near-caster
			D3DXVECTOR3 LightVec = vLight;
			D3DXVECTOR3 N = *pNormalPos;
			D3DXVec3Normalize ( &N, &N );
			D3DXVec3Normalize ( &LightVec, &LightVec );
			if ( D3DXVec3Dot ( &N, &(-LightVec) ) < 0.0f )
			{
				// stretch out
				pVertexPos->x -= LightVec.x * fShadowRange;
				pVertexPos->y -= LightVec.y * fShadowRange;
				pVertexPos->z -= LightVec.z * fShadowRange;
			}
			else
			{
				// defeat self-shadow zclash
				pVertexPos->x -= LightVec.x * 0.2f;
				pVertexPos->y -= LightVec.y * 0.2f;
				pVertexPos->z -= LightVec.z * 0.2f;
			}
		}
	}

	// trigger mesh to VB update
	pMesh->bVBRefreshRequired = true;
	pMesh->bMeshHasBeenReplaced = false;
	g_vRefreshMeshList.push_back ( pMesh );

	// success
	return true;
}

DARKSDK_DLL bool UpdateShadowMesh ( float fX, float fY, float fZ, float fRange, sMesh* pMesh, sMesh* pShadowMesh, D3DXMATRIX* pmatWorld, D3DXMATRIX* pmatCombined )
{
	// get frame mesh (CPU only for the moment)
	if ( pMesh && pShadowMesh )
	{
		// update shadow to entire frame mesh
		DWORD dwVertexStartFrom = 0;
		D3DXVECTOR3 LightDir = D3DXVECTOR3 ( fX, fY, fZ );
		CastShadowGeometryToShadowMesh ( LightDir, pMesh, fRange, &pShadowMesh, &dwVertexStartFrom, pmatWorld, pmatCombined );
	}

	// okay
	return true;
}

// Mesh Construction Functions

DARKSDK_DLL bool MakeMeshPlain ( bool bCreateNew, sMesh* pMesh, float fWidth, float fHeight, DWORD dwFVF, DWORD dwColor )
{
	// create memory
	DWORD dwVertexCount = 6;									// store number of vertices
	DWORD dwIndexCount  = 0;									// store number of indices
	if ( !SetupMeshFVFData ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, dwVertexCount, dwIndexCount ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
		return false;
	}
	
	// leefix-210703-corrected UV coords of plain ( they where -1.0f, etc?? )
	// and DBPro rotates it Y=180 so it faces the camera (for compatibility and correct plain)
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  0,  -fWidth,  fHeight, 0.0f,  0.0f,  0.0f,  1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 1.0f, 0.0f );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  1,   fWidth,  fHeight, 0.0f,  0.0f,  0.0f,  1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 0.0f, 0.0f );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  2,  -fWidth, -fHeight, 0.0f,  0.0f,  0.0f,  1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 1.0f, 1.0f );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  3,   fWidth,  fHeight, 0.0f,  0.0f,  0.0f,  1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 0.0f, 0.0f );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  4,   fWidth, -fHeight, 0.0f,  0.0f,  0.0f,  1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 0.0f, 1.0f );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  5,  -fWidth, -fHeight, 0.0f,  0.0f,  0.0f,  1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 1.0f, 1.0f );

	// setup mesh drawing properties
	pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pMesh->iDrawVertexCount = pMesh->dwVertexCount;
	pMesh->iDrawPrimitives  = 2;

	// okay
	return true;
}

// mike - 021005 - alternative make plane function
DARKSDK_DLL bool MakeMeshPlainEx ( bool bCreateNew, sMesh* pMesh, float fWidth, float fHeight, DWORD dwFVF, DWORD dwColor )
{
	// create memory
	DWORD dwVertexCount = 6;									// store number of vertices
	DWORD dwIndexCount  = 0;									// store number of indices
	if ( !SetupMeshFVFData ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, dwVertexCount, dwIndexCount ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
		return false;
	}

	float pos_pos_x = ( fWidth );
	float neg_pos_x = 0 - ( fWidth );
	float pos_pos_y = ( fHeight );
	float neg_pos_y = 0 - ( fHeight );

	
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  0,  neg_pos_x,  pos_pos_y, 0.0f,  0.0f,  0.0f,  -1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 0.0f, 0.0f );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  1,   pos_pos_x,  pos_pos_y, 0.0f,  0.0f,  0.0f,  -1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 1.0f, 0.0f );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  2,  pos_pos_x, neg_pos_y, 0.0f,  0.0f,  0.0f,  -1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 1.0f, 1.0f );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  3,   pos_pos_x,  neg_pos_y, 0.0f,  0.0f,  0.0f, -1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 1.0f, 1.0f );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  4,   neg_pos_x, neg_pos_y, 0.0f,  0.0f,  0.0f,  -1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 0.0f, 1.0f );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  5,   neg_pos_x, pos_pos_y, 0.0f,  0.0f,  0.0f,  -1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 0.0f, 0.0f );

	// setup mesh drawing properties
	pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pMesh->iDrawVertexCount = pMesh->dwVertexCount;
	pMesh->iDrawPrimitives  = 2;

	// okay
	return true;
}

DARKSDK_DLL bool MakeMeshBox ( bool bCreateNew, sMesh* pMesh, float fWidth1, float fHeight1, float fDepth1, float fWidth2, float fHeight2, float fDepth2, DWORD dwFVF, DWORD dwColor )
{
	// create vertex memory for box
	DWORD dwVertexCount = 24;
	DWORD dwIndexCount  = 36;
	if ( bCreateNew )
	{
		if ( !SetupMeshFVFData ( pMesh, dwFVF, dwVertexCount, dwIndexCount ) )
		{
			RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
			return false;
		}
	}

	// leefix - 080403 - adjusted UV data use accurate coordinates (slight texture alignment can now be done usinfg vertexdata commands)
	// setup the vertices, we're using a standard FVF here so call the standard utility function
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  0, fWidth1, fHeight2, fDepth1,  0.0f,  0.0f, -1.0f, dwColor, 0.00f, 0.00f );	// front
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  1, fWidth2, fHeight2, fDepth1,  0.0f,  0.0f, -1.0f, dwColor, 1.00f, 0.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  2, fWidth2, fHeight1, fDepth1,  0.0f,  0.0f, -1.0f, dwColor, 1.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  3, fWidth1, fHeight1, fDepth1,  0.0f,  0.0f, -1.0f, dwColor, 0.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  4, fWidth1, fHeight2, fDepth2,  0.0f,  0.0f,  1.0f, dwColor, 1.00f, 0.00f );	// back
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  5, fWidth1, fHeight1, fDepth2,  0.0f,  0.0f,  1.0f, dwColor, 1.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  6, fWidth2, fHeight1, fDepth2,  0.0f,  0.0f,  1.0f, dwColor, 0.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  7, fWidth2, fHeight2, fDepth2,  0.0f,  0.0f,  1.0f, dwColor, 0.00f, 0.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  8, fWidth1, fHeight2, fDepth2,	 0.0f,  1.0f,  0.0f, dwColor, 0.00f, 0.00f );	// top
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  9, fWidth2, fHeight2, fDepth2,	 0.0f,  1.0f,  0.0f, dwColor, 1.00f, 0.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 10, fWidth2, fHeight2, fDepth1,	 0.0f,  1.0f,  0.0f, dwColor, 1.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 11, fWidth1, fHeight2, fDepth1,	 0.0f,  1.0f,  0.0f, dwColor, 0.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 12, fWidth1, fHeight1, fDepth2,  0.0f, -1.0f,  0.0f, dwColor, 0.00f, 1.00f );	// bottom
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 13, fWidth1, fHeight1, fDepth1,	 0.0f, -1.0f,  0.0f, dwColor, 0.00f, 0.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 14, fWidth2, fHeight1, fDepth1,	 0.0f, -1.0f,  0.0f, dwColor, 1.00f, 0.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 15, fWidth2, fHeight1, fDepth2,	 0.0f, -1.0f,  0.0f, dwColor, 1.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 16, fWidth2, fHeight2, fDepth1,	 1.0f,  0.0f,  0.0f, dwColor, 0.00f, 0.00f );	// right
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 17, fWidth2, fHeight2, fDepth2,	 1.0f,  0.0f,  0.0f, dwColor, 1.00f, 0.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 18, fWidth2, fHeight1, fDepth2,	 1.0f,  0.0f,  0.0f, dwColor, 1.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 19, fWidth2, fHeight1, fDepth1,	 1.0f,  0.0f,  0.0f, dwColor, 0.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 20, fWidth1, fHeight2, fDepth1,	-1.0f,  0.0f,  0.0f, dwColor, 1.00f, 0.00f );	// left
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 21, fWidth1, fHeight1, fDepth1,	-1.0f,  0.0f,  0.0f, dwColor, 1.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 22, fWidth1, fHeight1, fDepth2,	-1.0f,  0.0f,  0.0f, dwColor, 0.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 23, fWidth1, fHeight2, fDepth2,	-1.0f,  0.0f,  0.0f, dwColor, 0.00f, 0.00f );
	
	// and now fill in the index list
	pMesh->pIndices [  0 ] =  0;		pMesh->pIndices [  1 ] =  1;		pMesh->pIndices [  2 ] =  2;
	pMesh->pIndices [  3 ] =  2;		pMesh->pIndices [  4 ] =  3;		pMesh->pIndices [  5 ] =  0;
	pMesh->pIndices [  6 ] =  4;		pMesh->pIndices [  7 ] =  5;		pMesh->pIndices [  8 ] =  6;
	pMesh->pIndices [  9 ] =  6;		pMesh->pIndices [ 10 ] =  7;		pMesh->pIndices [ 11 ] =  4;
	pMesh->pIndices [ 12 ] =  8;		pMesh->pIndices [ 13 ] =  9;		pMesh->pIndices [ 14 ] = 10;
	pMesh->pIndices [ 15 ] = 10;		pMesh->pIndices [ 16 ] = 11;		pMesh->pIndices [ 17 ] =  8;
	pMesh->pIndices [ 18 ] = 12;		pMesh->pIndices [ 19 ] = 13;		pMesh->pIndices [ 20 ] = 14;
	pMesh->pIndices [ 21 ] = 14;		pMesh->pIndices [ 22 ] = 15;		pMesh->pIndices [ 23 ] = 12;
	pMesh->pIndices [ 24 ] = 16;		pMesh->pIndices [ 25 ] = 17;		pMesh->pIndices [ 26 ] = 18;
	pMesh->pIndices [ 27 ] = 18;		pMesh->pIndices [ 28 ] = 19;		pMesh->pIndices [ 29 ] = 16;
	pMesh->pIndices [ 30 ] = 20;		pMesh->pIndices [ 31 ] = 21;		pMesh->pIndices [ 32 ] = 22;
	pMesh->pIndices [ 33 ] = 22;		pMesh->pIndices [ 34 ] = 23;		pMesh->pIndices [ 35 ] = 20;
	
	// setup mesh drawing properties
	pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pMesh->iDrawVertexCount = pMesh->dwVertexCount;
	pMesh->iDrawPrimitives  = pMesh->dwIndexCount  / 3;

	// okay
	return true;
}

DARKSDK_DLL bool MakeMeshPyramid ( bool bCreateNew, sMesh* pMesh, float fSize, DWORD dwFVF, DWORD dwColor )
{
	// create vertex memory for box
	DWORD dwVertexCount = 24;
	DWORD dwIndexCount  = 36;
	if ( bCreateNew )
	{
		if ( !SetupMeshFVFData ( pMesh, dwFVF, dwVertexCount, dwIndexCount ) )
		{
			RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
			return false;
		}
	}

	// calculate some dimensions
	float fWidth1 = -fSize;
	float fWidth2 = fSize;
	float fHeight1 = -fSize;
	float fHeight2 = fSize;
	float fDepth1 = -fSize;
	float fDepth2 = fSize;

	// generate vrtex data
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  0, 0, fHeight2, 0,  0.0f,  0.0f, -1.0f, dwColor, 0.00f, 0.00f );	// front
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  1, 0, fHeight2, 0,  0.0f,  0.0f, -1.0f, dwColor, 1.00f, 0.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  2, fWidth2, fHeight1, fDepth1,  0.0f,  0.0f, -1.0f, dwColor, 1.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  3, fWidth1, fHeight1, fDepth1,  0.0f,  0.0f, -1.0f, dwColor, 0.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  4, 0, fHeight2, 0,  0.0f,  0.0f,  1.0f, dwColor, 1.00f, 0.00f );	// back
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  5, fWidth1, fHeight1, fDepth2,  0.0f,  0.0f,  1.0f, dwColor, 1.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  6, fWidth2, fHeight1, fDepth2,  0.0f,  0.0f,  1.0f, dwColor, 0.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  7, 0, fHeight2, 0,  0.0f,  0.0f,  1.0f, dwColor, 0.00f, 0.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  8, 0, fHeight2, 0,	 0.0f,  1.0f,  0.0f, dwColor, 0.00f, 0.00f );	// top
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  9, 0, fHeight2, 0,	 0.0f,  1.0f,  0.0f, dwColor, 1.00f, 0.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 10, 0, fHeight2, 0,	 0.0f,  1.0f,  0.0f, dwColor, 1.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 11, 0, fHeight2, 0,	 0.0f,  1.0f,  0.0f, dwColor, 0.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 12, fWidth1, fHeight1, fDepth2,  0.0f, -1.0f,  0.0f, dwColor, 0.00f, 1.00f );	// bottom
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 13, fWidth1, fHeight1, fDepth1,	 0.0f, -1.0f,  0.0f, dwColor, 0.00f, 0.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 14, fWidth2, fHeight1, fDepth1,	 0.0f, -1.0f,  0.0f, dwColor, 1.00f, 0.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 15, fWidth2, fHeight1, fDepth2,	 0.0f, -1.0f,  0.0f, dwColor, 1.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 16, 0, fHeight2, 0,	 1.0f,  0.0f,  0.0f, dwColor, 0.00f, 0.00f );	// right
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 17, 0, fHeight2, 0,	 1.0f,  0.0f,  0.0f, dwColor, 1.00f, 0.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 18, fWidth2, fHeight1, fDepth2,	 1.0f,  0.0f,  0.0f, dwColor, 1.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 19, fWidth2, fHeight1, fDepth1,	 1.0f,  0.0f,  0.0f, dwColor, 0.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 20, 0, fHeight2, 0,	-1.0f,  0.0f,  0.0f, dwColor, 1.00f, 0.00f );	// left
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 21, fWidth1, fHeight1, fDepth1,	-1.0f,  0.0f,  0.0f, dwColor, 1.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 22, fWidth1, fHeight1, fDepth2,	-1.0f,  0.0f,  0.0f, dwColor, 0.00f, 1.00f );
	SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData, 23, 0, fHeight2, 0,	-1.0f,  0.0f,  0.0f, dwColor, 0.00f, 0.00f );
	
	// and now fill in the index list
	pMesh->pIndices [  0 ] =  0;		pMesh->pIndices [  1 ] =  1;		pMesh->pIndices [  2 ] =  2;
	pMesh->pIndices [  3 ] =  2;		pMesh->pIndices [  4 ] =  3;		pMesh->pIndices [  5 ] =  0;
	pMesh->pIndices [  6 ] =  4;		pMesh->pIndices [  7 ] =  5;		pMesh->pIndices [  8 ] =  6;
	pMesh->pIndices [  9 ] =  6;		pMesh->pIndices [ 10 ] =  7;		pMesh->pIndices [ 11 ] =  4;
	pMesh->pIndices [ 12 ] =  8;		pMesh->pIndices [ 13 ] =  9;		pMesh->pIndices [ 14 ] = 10;
	pMesh->pIndices [ 15 ] = 10;		pMesh->pIndices [ 16 ] = 11;		pMesh->pIndices [ 17 ] =  8;
	pMesh->pIndices [ 18 ] = 12;		pMesh->pIndices [ 19 ] = 13;		pMesh->pIndices [ 20 ] = 14;
	pMesh->pIndices [ 21 ] = 14;		pMesh->pIndices [ 22 ] = 15;		pMesh->pIndices [ 23 ] = 12;
	pMesh->pIndices [ 24 ] = 16;		pMesh->pIndices [ 25 ] = 17;		pMesh->pIndices [ 26 ] = 18;
	pMesh->pIndices [ 27 ] = 18;		pMesh->pIndices [ 28 ] = 19;		pMesh->pIndices [ 29 ] = 16;
	pMesh->pIndices [ 30 ] = 20;		pMesh->pIndices [ 31 ] = 21;		pMesh->pIndices [ 32 ] = 22;
	pMesh->pIndices [ 33 ] = 22;		pMesh->pIndices [ 34 ] = 23;		pMesh->pIndices [ 35 ] = 20;
	
	// setup mesh drawing properties
	pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pMesh->iDrawVertexCount = pMesh->dwVertexCount;
	pMesh->iDrawPrimitives  = pMesh->dwIndexCount  / 3;

	// okay
	return true;
}

DARKSDK_DLL bool MakeMeshSphere ( bool bCreateNew, sMesh* pMesh, D3DXVECTOR3 vecCentre, float fRadius, int iRings, int iSegments, DWORD dwFVF, DWORD dwColor )
{
	// create sphere mesh
	DWORD dwIndexCount		= 2 * iRings * ( iSegments + 1 );
	DWORD dwVertexCount		= ( iRings + 1 ) * ( iSegments + 1 );
	if ( bCreateNew )
	{
		if ( !SetupMeshFVFData ( pMesh, dwFVF, dwVertexCount, dwIndexCount ) )
		{
			RunTimeError ( RUNTIMEERROR_B3DMEMORYERROR );
			return false;
		}
	}

	// now we can fill in the vertex and index data to form the mesh
	float		fDeltaRingAngle		= ( D3DX_PI / iRings );
	float		fDeltaSegAngle		= ( 2.0f * D3DX_PI / iSegments );
	int			iVertex				= 0;
	int			iIndex				= 0;
	WORD		wVertexIndex		= 0;
	D3DXVECTOR3 vNormal;

	// generate the group of rings for the sphere
	for ( int iCurrentRing = 0; iCurrentRing < iRings + 1; iCurrentRing++ )
	{
		float r0 = sinf ( iCurrentRing * fDeltaRingAngle );
		float y0 = cosf ( iCurrentRing * fDeltaRingAngle );

		// generate the group of segments for the current ring
		for ( int iCurrentSegment = 0; iCurrentSegment < iSegments + 1; iCurrentSegment++ )
		{
			float x0 = r0 * sinf ( iCurrentSegment * fDeltaSegAngle );
			float z0 = r0 * cosf ( iCurrentSegment * fDeltaSegAngle );

			vNormal.x = x0;
			vNormal.y = y0;
			vNormal.z = z0;
	
			D3DXVec3Normalize ( &vNormal, &vNormal );

			// add one vertex to the strip which makes up the sphere
			SetupStandardVertex (	pMesh->dwFVF, pMesh->pVertexData,  iVertex,
									vecCentre.x+(x0*fRadius), vecCentre.y+(y0*fRadius), vecCentre.z+(z0*fRadius), 
									vNormal.x, vNormal.y, vNormal.z,
									dwColor,
									1.0f - ( ( float ) iCurrentSegment / ( float ) iSegments ),
									( float ) iCurrentRing / ( float ) iRings );

			// increment vertex
			iVertex++;
			
			// add two indices except for the last ring 
			if ( iCurrentRing != iRings )
			{
				pMesh->pIndices [ iIndex ] = wVertexIndex;
				iIndex++;
				
				pMesh->pIndices [ iIndex ] = wVertexIndex + ( WORD ) ( iSegments + 1 ); 
				iIndex++;
				
				wVertexIndex++; 
			}
		}
	}

	// setup mesh drawing properties
	pMesh->iPrimitiveType   = D3DPT_TRIANGLESTRIP;
	pMesh->iDrawVertexCount = pMesh->dwVertexCount;
	pMesh->iDrawPrimitives  = pMesh->dwIndexCount - 2;

	// okay
	return true;
}

DARKSDK_DLL bool MakeMeshFromOtherMesh ( bool bCreateNew, sMesh* pMesh, sMesh* pOtherMesh, D3DXMATRIX* pmatWorld, DWORD dwIndexCount, DWORD dwVertexCount )
{
	// make new mesh from existing other mesh
	MakeLocalMeshFromOtherLocalMesh ( pMesh, pOtherMesh, dwIndexCount, dwVertexCount );

	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// copy vertex data from mesh to single-mesh
	BYTE* pDestVertexData = NULL;
	BYTE* pDestNormalData = NULL;

	// make sure we have data and ptrs
	if ( offsetMap.dwZ>0 )
	{
		pDestVertexData = (BYTE*)((float*)(pMesh->pVertexData));
	}
	if ( offsetMap.dwNZ>0 )
	{
		pDestNormalData = (BYTE*)((float*)pMesh->pVertexData + offsetMap.dwNX);
	}

	// establish world matrix if any
	D3DXMATRIX matNoTransWorld;
	if ( *pmatWorld )
		matNoTransWorld = *pmatWorld;
	else
		D3DXMatrixIdentity ( &matNoTransWorld );

	// remove all translation from world matrix
	matNoTransWorld._41 = 0.0f;
	matNoTransWorld._42 = 0.0f;
	matNoTransWorld._43 = 0.0f;

	// transform vertex data by world matrix of frame
	for ( DWORD v=0; v<pOtherMesh->dwVertexCount; v++ )
	{
		// handle vertex data
		if ( pDestVertexData )
		{
			D3DXVECTOR3* pVertex = (D3DXVECTOR3*)(pDestVertexData+(v*pMesh->dwFVFSize));
			D3DXVec3TransformCoord ( pVertex, pVertex, &matNoTransWorld );
		}

		// handle normals data
		if ( pDestNormalData )
		{
			D3DXVECTOR3* pNormal = (D3DXVECTOR3*)(pDestNormalData+(v*pMesh->dwFVFSize));
			D3DXVec3TransformNormal ( pNormal, pNormal, &matNoTransWorld );
			D3DXVec3Normalize ( pNormal, pNormal );
		}
	}

	// 151003 - add computation of bound box in collision structure (for limb collision)
	CalculateMeshBounds ( pMesh );

	// okay
	return true;
}

DARKSDK_DLL bool MakeMeshFromOtherMesh ( bool bCreateNew, sMesh* pMesh, sMesh* pOtherMesh, D3DXMATRIX* pmatWorld )
{
	DWORD dwIndexCount = pOtherMesh->dwIndexCount;
	DWORD dwVertexCount = pOtherMesh->dwVertexCount;
	return MakeMeshFromOtherMesh ( bCreateNew, pMesh, pOtherMesh, pmatWorld, dwIndexCount, dwVertexCount );
}

bool MakeLODMeshFromMesh ( sMesh* pMeshIn, int iVertexNum, sMesh** ppMeshOut )
{
	// vars
	bool bSuccess = false;
    LPD3DXPMESH  m_pPMeshFull = NULL;
    LPD3DXBUFFER pAdjacencyBuffer = NULL;
    LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = NULL;
    LPD3DXMESH   pMesh = NULL;
    LPD3DXPMESH  pPMesh = NULL;
    LPD3DXMESH   pTempMesh;
    HRESULT      hr;
    DWORD        dw32BitFlag;
    DWORD        cVerticesMin;
    DWORD        cVerticesMax;
    D3DXWELDEPSILONS Epsilons;
    DWORD        i;

	// load DX mesh from DBO mesh
	pMesh = LocalMeshToDXMesh ( pMeshIn, NULL, 0 );

	// create an adjacency buffer if not got one
	if ( pAdjacencyBuffer==NULL )
	{
		// Generate adjacency information
		DWORD NumAdjDWORDS = 3 * pMesh->GetNumFaces();
		DWORD* pdwAdj = new DWORD[NumAdjDWORDS];
		hr = pMesh->GenerateAdjacency( ADJACENCY_EPSILON, pdwAdj );
		if( !FAILED( hr ) )
		{
			D3DXCreateBuffer( NumAdjDWORDS*4, &pAdjacencyBuffer );
			DWORD* pAdjPtr = (DWORD*)pAdjacencyBuffer->GetBufferPointer();
			for ( i=0; i<NumAdjDWORDS; i++ ) pAdjPtr [ i ] = pdwAdj [ i ];
			SAFE_DELETE ( pdwAdj );
		}
		else
		{
			// failed to create adj buffer
			SAFE_DELETE ( pdwAdj );
		}
	}

	// setup 32 bit flag
    dw32BitFlag = (pMesh->GetOptions() & D3DXMESH_32BIT);

    // perform simple cleansing operations on mesh
	// mike - 020206 - addition for vs8 - lee change the first param to one of the other clean modes if you need to
    //if( FAILED( hr = D3DXCleanMesh( pMesh, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), &pTempMesh, 
      //                                     (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL ) ) )
	if( FAILED( hr = D3DXCleanMesh( 
										D3DXCLEAN_BACKFACING,
										pMesh,
										(DWORD*)pAdjacencyBuffer->GetBufferPointer(),
										&pTempMesh, 
                                        (DWORD*)pAdjacencyBuffer->GetBufferPointer(),
										NULL
									) ) )
    {
		// failed
		SAFE_RELEASE( pAdjacencyBuffer );
		SAFE_RELEASE( pMesh );
		return false;
    }
    SAFE_RELEASE(pMesh);
    pMesh = pTempMesh;

    //  Perform a weld to try and remove excess vertices like the model bigship1.x in the DX9.0 SDK (current model is fixed)
    //    Weld the mesh using all epsilons of 0.0f.  A small epsilon like 1e-6 works well too
    memset(&Epsilons, 0, sizeof(D3DXWELDEPSILONS));
    if( FAILED( hr = D3DXWeldVertices( pMesh, 0, &Epsilons, 
                                                (DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
                                                (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL ) ) )
    {
		// failed
		SAFE_RELEASE( pAdjacencyBuffer );
		SAFE_RELEASE( pMesh );
		return false;
    }

    // verify validity of mesh for simplification
    if( FAILED( hr = D3DXValidMesh( pMesh, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL ) ) )
    {
		// failed
		SAFE_RELEASE( pAdjacencyBuffer );
		SAFE_RELEASE( pMesh );
		return false;
    }

	// seems PM needs normals?!?
    if ( !(pMesh->GetFVF() & D3DFVF_NORMAL) )
    {
        hr = pMesh->CloneMeshFVF( dw32BitFlag|D3DXMESH_MANAGED, pMesh->GetFVF() | D3DFVF_NORMAL, m_pD3D, &pTempMesh );
        if (FAILED(hr))
		{
			// failed
			SAFE_RELEASE( pAdjacencyBuffer );
			SAFE_RELEASE( pMesh );
			return false;
		}
        D3DXComputeNormals( pTempMesh, NULL );
        pMesh->Release();
        pMesh = pTempMesh;
    }

	// weights controls how LOD produces its results
	D3DXATTRIBUTEWEIGHTS aweights;
	memset ( &aweights, 0, sizeof ( aweights ) );
	aweights.Boundary = 2.0f;
	aweights.Position = 1.0f;

	// do the PM conversion
    hr = D3DXGeneratePMesh( pMesh, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), &aweights, NULL, 1, D3DXMESHSIMP_VERTEX, &pPMesh);
    if( FAILED(hr) )
    {
		// failed
		SAFE_RELEASE( pAdjacencyBuffer );
		SAFE_RELEASE( pMesh );
		return false;
    }

	// get results of progressive generation
    cVerticesMin = pPMesh->GetMinVertices();
    cVerticesMax = pPMesh->GetMaxVertices();

    // clone pmesh
    hr = pPMesh->ClonePMeshFVF( D3DXMESH_MANAGED | D3DXMESH_VB_SHARE, pPMesh->GetFVF(), m_pD3D, &m_pPMeshFull );
    if (FAILED(hr))
    {
		// failed
		SAFE_RELEASE( pAdjacencyBuffer );
		SAFE_RELEASE( pMesh );
		SAFE_RELEASE( pPMesh );
		return false;
    }

	// apply reduction specified by input param
	if ( iVertexNum > (int)cVerticesMax ) iVertexNum = cVerticesMax;
	if ( iVertexNum < (int)cVerticesMin ) iVertexNum = cVerticesMin;
    hr = m_pPMeshFull->SetNumVertices(iVertexNum);
    if (FAILED(hr))
    {
		// failed
		SAFE_RELEASE( pAdjacencyBuffer );
		SAFE_RELEASE( pMesh );
		SAFE_RELEASE( pPMesh );
		SAFE_RELEASE( m_pPMeshFull )
		return false;
    }

	// release usages
    SAFE_RELEASE( pAdjacencyBuffer );
    SAFE_RELEASE( pMesh );
    SAFE_RELEASE( pPMesh );

	// create DBO mesh from DX mesh
	if ( ppMeshOut )
	{
		m_pPMeshFull->CloneMeshFVF ( 0, m_pPMeshFull->GetFVF(), m_pD3D, &pMesh );
		SAFE_RELEASE ( m_pPMeshFull );
		*ppMeshOut = new sMesh;
		UpdateLocalMeshWithDXMesh ( *ppMeshOut, pMesh );
		if ( *ppMeshOut )
		{
			// provide settings for drawing
			(*ppMeshOut)->iPrimitiveType   = D3DPT_TRIANGLELIST;
			(*ppMeshOut)->iDrawVertexCount = (*ppMeshOut)->dwVertexCount;
			if ( (*ppMeshOut)->dwIndexCount==0 )
				(*ppMeshOut)->iDrawPrimitives  = (*ppMeshOut)->dwVertexCount/3;
			else
				(*ppMeshOut)->iDrawPrimitives  = (*ppMeshOut)->dwIndexCount/3;
		}
		SAFE_RELEASE(pMesh);
		bSuccess = true;
	}

	// leelee - 121205 - optional extra for terrain LOD (drop edges)
	bool bDropTerrainLODEdges = true;
	if ( bDropTerrainLODEdges )
	{
		if ( ppMeshOut )
		{
			if ( *ppMeshOut )
			{
				// access DBO mes
				sOffsetMap offsetMap;
				sMesh* pDBOMesh = *ppMeshOut;
				GetFVFOffsetMap ( pDBOMesh, &offsetMap );
				if ( pDBOMesh->dwFVF & D3DFVF_XYZ )
				{
					// find lowest point of all terrain verts, and edge bounds
					D3DXVECTOR3 vecXZMin = D3DXVECTOR3 (  99999, 0,  99999 );
					D3DXVECTOR3 vecXZMax = D3DXVECTOR3 ( -99999, 0, -99999 );
					float fLowestYPos = 999999.0f;
					for ( int iCurrentVertex = 0; iCurrentVertex < (int)pDBOMesh->dwVertexCount; iCurrentVertex++ )
					{
						// work out lowest Y
						D3DXVECTOR3 vecPos = *(D3DXVECTOR3*)( ( float* ) pDBOMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) );
						if ( vecPos.y < fLowestYPos ) fLowestYPos = vecPos.y;

						// work out min and max point
						if ( vecPos.x < vecXZMin.x ) vecXZMin.x = vecPos.x;
						if ( vecPos.z < vecXZMin.z ) vecXZMin.z = vecPos.z;
						if ( vecPos.x > vecXZMax.x ) vecXZMax.x = vecPos.x;
						if ( vecPos.z > vecXZMax.z ) vecXZMax.z = vecPos.z;
					}

					// prepare skirt verts
					DWORD dwSkirtMax = 200;
					DWORD* pSkirtN = new DWORD [ dwSkirtMax ];
					DWORD* pSkirtS = new DWORD [ dwSkirtMax ];
					DWORD* pSkirtE = new DWORD [ dwSkirtMax ];
					DWORD* pSkirtW = new DWORD [ dwSkirtMax ];
					DWORD dwSkirtIndexN = 0;
					DWORD dwSkirtIndexS = 0;
					DWORD dwSkirtIndexE = 0;
					DWORD dwSkirtIndexW = 0;
					DWORD dwNewPolysNeeded = 0;

					// mike - 020206 - addition for vs8
					// scan for any edge verts, and create skirts around LOD mesh
					//for ( iCurrentVertex = 0; iCurrentVertex < (int)pDBOMesh->dwVertexCount; iCurrentVertex++ )
					for ( int iCurrentVertex = 0; iCurrentVertex < (int)pDBOMesh->dwVertexCount; iCurrentVertex++ )
					{
						// ptr to vert
						D3DXVECTOR3* pvecPos = (D3DXVECTOR3*)( ( DWORD* ) pDBOMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) );

						// gather side vert refs
						if ( fabs ( pvecPos->z - vecXZMax.z ) < 0.25f )
						{
							// North Z+
							pSkirtN [ dwSkirtIndexN++ ] = iCurrentVertex;
							if ( dwSkirtIndexN>=dwSkirtMax ) dwSkirtIndexN=dwSkirtMax-1;
							dwNewPolysNeeded++;
						}
						if ( fabs ( pvecPos->x - vecXZMax.x ) < 0.25f )
						{
							// East X+
							pSkirtE [ dwSkirtIndexE++ ] = iCurrentVertex;
							if ( dwSkirtIndexE>=dwSkirtMax ) dwSkirtIndexE=dwSkirtMax-1;
							dwNewPolysNeeded++;
						}
						if ( fabs ( pvecPos->z - vecXZMin.z ) < 0.25f )
						{
							// South Z-
							pSkirtS [ dwSkirtIndexS++ ] = iCurrentVertex;
							if ( dwSkirtIndexS>=dwSkirtMax ) dwSkirtIndexS=dwSkirtMax-1;
							dwNewPolysNeeded++;
						}
						if ( fabs ( pvecPos->x - vecXZMin.x ) < 0.25f )
						{
							// West X-
							pSkirtW [ dwSkirtIndexW++ ] = iCurrentVertex;
							if ( dwSkirtIndexW>=dwSkirtMax ) dwSkirtIndexW=dwSkirtMax-1;
							dwNewPolysNeeded++;
						}
					}

					// Bubble sort side vert references in ascending order (for clockwise ordered poly sequence)
					// north
					for ( DWORD iRefA = 0; iRefA<dwSkirtIndexN; iRefA++ )
					{
						for ( DWORD iRefB = 0; iRefB<dwSkirtIndexN; iRefB++ )
						{
							if ( iRefA != iRefB )
							{
								int iRefAVertexIndex = pSkirtN [ iRefA ];
								float fPosA = *( ( float* ) pDBOMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iRefAVertexIndex ) );
								int iRefBVertexIndex = pSkirtN [ iRefB ];
								float fPosB = *( ( float* ) pDBOMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iRefBVertexIndex ) );
								if ( fPosB < fPosA )
								{
									DWORD dwStoreA = pSkirtN [ iRefA ];
									pSkirtN [ iRefA ] = pSkirtN [ iRefB ];
									pSkirtN [ iRefB ] = dwStoreA;
								}
							}
						}
					}
					// east
					// mike - 020206 - addition for vs8
					//for ( iRefA = 0; iRefA<dwSkirtIndexE; iRefA++ )
					for ( DWORD iRefA = 0; iRefA<dwSkirtIndexE; iRefA++ )
					{
						for ( DWORD iRefB = 0; iRefB<dwSkirtIndexE; iRefB++ )
						{
							if ( iRefA != iRefB )
							{
								int iRefAVertexIndex = pSkirtE [ iRefA ];
								float fPosA = *( ( float* ) pDBOMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iRefAVertexIndex ) );
								int iRefBVertexIndex = pSkirtE [ iRefB ];
								float fPosB = *( ( float* ) pDBOMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iRefBVertexIndex ) );
								if ( fPosB > fPosA )
								{
									DWORD dwStoreA = pSkirtE [ iRefA ];
									pSkirtE [ iRefA ] = pSkirtE [ iRefB ];
									pSkirtE [ iRefB ] = dwStoreA;
								}
							}
						}
					}
					// south
					// mike - 020206 - addition for vs8
					//for ( iRefA = 0; iRefA<dwSkirtIndexS; iRefA++ )
					for ( int iRefA = 0; iRefA<(int)dwSkirtIndexS; iRefA++ )
					{
						for ( DWORD iRefB = 0; iRefB<(int)dwSkirtIndexS; iRefB++ )
						{
							if ( iRefA != iRefB )
							{
								int iRefAVertexIndex = pSkirtS [ iRefA ];
								float fPosA = *( ( float* ) pDBOMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iRefAVertexIndex ) );
								int iRefBVertexIndex = pSkirtS [ iRefB ];
								float fPosB = *( ( float* ) pDBOMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iRefBVertexIndex ) );
								if ( fPosB > fPosA )
								{
									DWORD dwStoreA = pSkirtS [ iRefA ];
									pSkirtS [ iRefA ] = pSkirtS [ iRefB ];
									pSkirtS [ iRefB ] = dwStoreA;
								}
							}
						}
					}

					// west
					for ( int iRefA = 0; iRefA<(int)dwSkirtIndexW; iRefA++ )
					{
						for ( DWORD iRefB = 0; iRefB<dwSkirtIndexW; iRefB++ )
						{
							if ( iRefA != iRefB )
							{
								int iRefAVertexIndex = pSkirtW [ iRefA ];
								float fPosA = *( ( float* ) pDBOMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iRefAVertexIndex ) );
								int iRefBVertexIndex = pSkirtW [ iRefB ];
								float fPosB = *( ( float* ) pDBOMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iRefBVertexIndex ) );
								if ( fPosB < fPosA )
								{
									DWORD dwStoreA = pSkirtW [ iRefA ];
									pSkirtW [ iRefA ] = pSkirtW [ iRefB ];
									pSkirtW [ iRefB ] = dwStoreA;
								}
							}
						}
					}

					// Create Space for new verts and indices
					DWORD dwExtraVerts = dwNewPolysNeeded;
					DWORD dwExtraIndices = (dwNewPolysNeeded-4) * 6;
					DWORD dwTotalVertices = pDBOMesh->dwVertexCount + dwExtraVerts;
					DWORD dwTotalIndices = pDBOMesh->dwIndexCount + dwExtraIndices;

					// create a mesh
					BYTE* pNewVertData = (BYTE*)new char [ dwTotalVertices * pDBOMesh->dwFVFSize ];
					WORD* pNewIndexData = new WORD [ dwTotalIndices ];
					memset ( pNewIndexData, 0, sizeof(WORD)*dwTotalIndices );

					// copy over original to final
					memcpy ( pNewVertData, pDBOMesh->pVertexData, pDBOMesh->dwVertexCount * pDBOMesh->dwFVFSize );
					memcpy ( pNewIndexData, pDBOMesh->pIndices, pDBOMesh->dwIndexCount * sizeof(WORD) );

					// make new verticess (NSEW)
					WORD dwVertexStart = (WORD)pDBOMesh->dwVertexCount;
					// north
					int* iVertN = new int [ dwSkirtIndexN ];
					for ( int iN=0; iN<(int)dwSkirtIndexN; iN++ )
					{
						iVertN [ iN ] = dwVertexStart;	dwVertexStart++;
						float* pSrc = ( ( float* ) pNewVertData + offsetMap.dwX + ( offsetMap.dwSize * pSkirtN [ iN ] ) );
						float* pDest = ( ( float* ) pNewVertData + offsetMap.dwX + ( offsetMap.dwSize * iVertN [ iN ] ) );
						memcpy ( pDest, pSrc, pDBOMesh->dwFVFSize );
						*( ( float* ) pNewVertData + offsetMap.dwY + ( offsetMap.dwSize * iVertN [ iN ] ) ) = -1;
					}
					// east
					int* iVertE = new int [ dwSkirtIndexE ];
					for ( int iE=0; iE<(int)dwSkirtIndexE; iE++ )
					{
						iVertE [ iE ] = dwVertexStart;	dwVertexStart++;
						float* pSrc = ( ( float* ) pNewVertData + offsetMap.dwX + ( offsetMap.dwSize * pSkirtE [ iE ] ) );
						float* pDest = ( ( float* ) pNewVertData + offsetMap.dwX + ( offsetMap.dwSize * iVertE [ iE ] ) );
						memcpy ( pDest, pSrc, pDBOMesh->dwFVFSize );
						*( ( float* ) pNewVertData + offsetMap.dwY + ( offsetMap.dwSize * iVertE [ iE ] ) ) = -1;
					}
					// south
					int* iVertS = new int [ dwSkirtIndexS ];
					for ( int iS=0; iS<(int)dwSkirtIndexS; iS++ )
					{
						iVertS [ iS ] = dwVertexStart;	dwVertexStart++;
						float* pSrc = ( ( float* ) pNewVertData + offsetMap.dwX + ( offsetMap.dwSize * pSkirtS [ iS ] ) );
						float* pDest = ( ( float* ) pNewVertData + offsetMap.dwX + ( offsetMap.dwSize * iVertS [ iS ] ) );
						memcpy ( pDest, pSrc, pDBOMesh->dwFVFSize );
						*( ( float* ) pNewVertData + offsetMap.dwY + ( offsetMap.dwSize * iVertS [ iS ] ) ) = -1;
					}
					// west
					int* iVertW = new int [ dwSkirtIndexW ];
					for ( int iW=0; iW<(int)dwSkirtIndexW; iW++ )
					{
						iVertW [ iW ] = dwVertexStart;	dwVertexStart++;
						float* pSrc = ( ( float* ) pNewVertData + offsetMap.dwX + ( offsetMap.dwSize * pSkirtW [ iW ] ) );
						float* pDest = ( ( float* ) pNewVertData + offsetMap.dwX + ( offsetMap.dwSize * iVertW [ iW ] ) );
						memcpy ( pDest, pSrc, pDBOMesh->dwFVFSize );
						*( ( float* ) pNewVertData + offsetMap.dwY + ( offsetMap.dwSize * iVertW [ iW ] ) ) = -1;
					}

					// create polygons for skirt sides
					DWORD dwIndexStart = pDBOMesh->dwIndexCount;
					DWORD dwCurrentIndex = dwIndexStart;
					// north
					for (i=0; i<dwSkirtIndexN-1; i++ )
					{
						pNewIndexData [ dwCurrentIndex+0 ] = (WORD)iVertN [ i+0 ];
						pNewIndexData [ dwCurrentIndex+1 ] = (WORD)pSkirtN [ i+0 ];
						pNewIndexData [ dwCurrentIndex+2 ] = (WORD)pSkirtN [ i+1 ];
						dwCurrentIndex+=3;
						pNewIndexData [ dwCurrentIndex+0 ] = (WORD)iVertN [ i+0 ];
						pNewIndexData [ dwCurrentIndex+1 ] = (WORD)pSkirtN [ i+1 ];
						pNewIndexData [ dwCurrentIndex+2 ] = (WORD)iVertN [ i+1 ];
						dwCurrentIndex+=3;
					}
					// east
					for (i=0; i<dwSkirtIndexE-1; i++ )
					{
						pNewIndexData [ dwCurrentIndex+0 ] = (WORD)iVertE [ i+0 ];
						pNewIndexData [ dwCurrentIndex+1 ] = (WORD)pSkirtE [ i+0 ];
						pNewIndexData [ dwCurrentIndex+2 ] = (WORD)pSkirtE [ i+1 ];
						dwCurrentIndex+=3;
						pNewIndexData [ dwCurrentIndex+0 ] = (WORD)iVertE [ i+0 ];
						pNewIndexData [ dwCurrentIndex+1 ] = (WORD)pSkirtE [ i+1 ];
						pNewIndexData [ dwCurrentIndex+2 ] = (WORD)iVertE [ i+1 ];
						dwCurrentIndex+=3;
					}
					// south
					for (i=0; i<dwSkirtIndexS-1; i++ )
					{
						pNewIndexData [ dwCurrentIndex+0 ] = (WORD)iVertS [ i+0 ];
						pNewIndexData [ dwCurrentIndex+1 ] = (WORD)pSkirtS [ i+0 ];
						pNewIndexData [ dwCurrentIndex+2 ] = (WORD)pSkirtS [ i+1 ];
						dwCurrentIndex+=3;
						pNewIndexData [ dwCurrentIndex+0 ] = (WORD)iVertS [ i+0 ];
						pNewIndexData [ dwCurrentIndex+1 ] = (WORD)pSkirtS [ i+1 ];
						pNewIndexData [ dwCurrentIndex+2 ] = (WORD)iVertS [ i+1 ];
						dwCurrentIndex+=3;
					}
					// west
					for (i=0; i<dwSkirtIndexW-1; i++ )
					{
						pNewIndexData [ dwCurrentIndex+0 ] = (WORD)iVertW [ i+0 ];
						pNewIndexData [ dwCurrentIndex+1 ] = (WORD)pSkirtW [ i+0 ];
						pNewIndexData [ dwCurrentIndex+2 ] = (WORD)pSkirtW [ i+1 ];
						dwCurrentIndex+=3;
						pNewIndexData [ dwCurrentIndex+0 ] = (WORD)iVertW [ i+0 ];
						pNewIndexData [ dwCurrentIndex+1 ] = (WORD)pSkirtW [ i+1 ];
						pNewIndexData [ dwCurrentIndex+2 ] = (WORD)iVertW [ i+1 ];
						dwCurrentIndex+=3;
					}

					// replace old vert/index data with new ones
					SAFE_DELETE ( pDBOMesh->pVertexData );
					SAFE_DELETE ( pDBOMesh->pIndices );
					pDBOMesh->dwVertexCount = dwTotalVertices;
					pDBOMesh->dwIndexCount = dwTotalIndices;
					pDBOMesh->pVertexData = pNewVertData;
					pDBOMesh->pIndices = pNewIndexData;

					// update values of the mesh
					pDBOMesh->iDrawVertexCount = dwTotalVertices;
					pDBOMesh->iDrawPrimitives  = dwTotalIndices/3;

					// flag mesh for a VB replacement
					pDBOMesh->bMeshHasBeenReplaced=true;

					// free usages
					SAFE_DELETE ( pSkirtN );
					SAFE_DELETE ( pSkirtS );
					SAFE_DELETE ( pSkirtE );
					SAFE_DELETE ( pSkirtW );
					SAFE_DELETE ( iVertN );
					SAFE_DELETE ( iVertE );
					SAFE_DELETE ( iVertS );
					SAFE_DELETE ( iVertW );
				}
			}
		}
	}

	// return result
	return bSuccess;
}

