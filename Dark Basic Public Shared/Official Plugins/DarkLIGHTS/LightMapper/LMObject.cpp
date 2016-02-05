#include "DBPro Functions.h"

#include "LMObject.h"
#include "LMPolyGroup.h"
#include "LMPoly.h"
#include "LMTexture.h"
#include "Light.h"
#include "CollisionTree.h"

LMObject::LMObject( sObject *pParentObject, sFrame *pParentFrame, sMesh *pNewMesh )
{
	pMesh = pNewMesh;
	pFrame = pParentFrame;
	pObject = pParentObject;

	pNextObject = 0;
	pPolyList = 0;
	pPolyGroupList = 0;
	iNumPolys = 0;
	iBaseStage = 0;
	iDynamicLightMode = 0;
	fQuality = 0;
	bShaded = false;

	pLMTexture = 0;
	iID = 0;
	iLimbID = 0;
}

LMObject::~LMObject( )
{
	LMPolyGroup *pTempGroup;

	while ( pPolyGroupList )
	{
		pTempGroup = pPolyGroupList;
		pPolyGroupList = pPolyGroupList->pNextGroup;
		delete pTempGroup;
	}
}

sObject* LMObject::GetObjectPtr( )
{
	return pObject;
}

sFrame* LMObject::GetFramePtr( )
{
	return pFrame;
}

sMesh* LMObject::GetMeshPtr( )
{
	return pMesh;
}

void LMObject::SetBaseStage( int iNewStage )
{
	if ( iNewStage < 0 || iNewStage > 7 ) iNewStage = -1;
	iBaseStage = iNewStage;
}

void LMObject::SetShaded( int iLightMapStage )
{
	bShaded = true;
	iBaseStage = iLightMapStage;
}

bool LMObject::GetIsShaded( )
{
	return bShaded;
}

void LMObject::SetDynamicLightMode( int iNewMode )
{
	if ( iNewMode < 0 ) iNewMode = 0;
	iDynamicLightMode = iNewMode;
}

LMPolyGroup* LMObject::GetFirstGroup( )
{
	return pPolyGroupList;
}

int LMObject::GetNumPolys( )
{
	return iNumPolys;
}

float LMObject::GetQuality( )
{
	return fQuality;
}

bool LMObject::WillFit( int iTexSizeU, int iTexSizeV )
{
	int iAvailArea = iTexSizeU*iTexSizeV;
	int iTotalArea = 0;

	LMPolyGroup *pGroup = pPolyGroupList;
	int iMaxU=0, iMaxV=0;

	while ( pGroup )
	{
		iTotalArea += pGroup->GetScaledSizeU( ) * pGroup->GetScaledSizeV( );

		if ( pGroup->GetScaledSizeU( ) > iMaxU ) iMaxU = pGroup->GetScaledSizeU( );
		if ( pGroup->GetScaledSizeV( ) > iMaxV ) iMaxV = pGroup->GetScaledSizeV( );

		pGroup = pGroup->pNextGroup;
	}

	bool bWillFit = iTotalArea*1.01 < iAvailArea;
	bWillFit = bWillFit && iMaxU < iTexSizeU && iMaxV < iTexSizeV;

	return bWillFit;
}

float LMObject::RecomendedQaulity( int iTexSizeU, int iTexSizeV )
{
	float fAvailArea = (float) iTexSizeU*iTexSizeV;
	float fTotalArea = 0;

	LMPolyGroup *pGroup = pPolyGroupList;

	while ( pGroup )
	{
		fTotalArea += pGroup->GetSizeU( ) * pGroup->GetSizeV( );

		pGroup = pGroup->pNextGroup;
	}

	//unfinished

	return 1.0;
}

void LMObject::BuildPolyList( bool bFlatShaded )
{
	g_CalculateAbsoluteWorldMatrix ( pObject, pFrame, pMesh );
	g_ConvertLocalMeshToVertsOnly( pMesh );
	if ( pMesh->pOriginalVertexData )
	{
		g_ConvertToFVF( pMesh, pMesh->dwFVF );
	}

	BYTE *pVertexData = pMesh->pVertexData;
	DWORD dwFVFSize = pMesh->dwFVFSize;
	int iNumVertices = pMesh->dwVertexCount;
	
	Point p1,p2,p3;
	int dwIndex;
	D3DXVECTOR3 vecVert;

	fObjCenX = 0; fObjCenY = 0; fObjCenZ = 0;
	float fPosX = 0;
	float fPosY = 0;
	float fPosZ = 0;
	iNumPolys = 0;
	float *pU1, *pU2, *pU3;
	float *pV1, *pV2, *pV3;
	Vector normV1, normV2, normV3;

	sOffsetMap cOffsetMap;
	g_GetFVFOffsetMap( pMesh, &cOffsetMap );

	int iLMStage = 1;
	if ( bShaded ) iLMStage = iBaseStage;

	//int iFlat = 0;
	//int iCurved = 0;

	D3DXMATRIX * pmatTransInvAbsoluteWorld = new D3DXMATRIX;
	D3DXMatrixInverse( pmatTransInvAbsoluteWorld, NULL, &pFrame->matAbsoluteWorld );
	D3DXMatrixTranspose( pmatTransInvAbsoluteWorld, pmatTransInvAbsoluteWorld );

	for ( int i = 0; i < iNumVertices; i+=3 )
	{
		dwIndex = i*dwFVFSize;

		vecVert.x = *( (float*) ( pVertexData + dwIndex )     );
		vecVert.y = *( (float*) ( pVertexData + dwIndex + 4 ) );
		vecVert.z = *( (float*) ( pVertexData + dwIndex + 8 ) );
		normV1.x = *( (float*) ( pVertexData + dwIndex + 12 ) );
		normV1.y = *( (float*) ( pVertexData + dwIndex + 16 ) );
		normV1.z = *( (float*) ( pVertexData + dwIndex + 20 ) );
		pU1 = (float*) ( pVertexData + dwIndex + cOffsetMap.dwTU [ iLMStage ]*4 );
		pV1 = (float*) ( pVertexData + dwIndex + cOffsetMap.dwTV [ iLMStage ]*4 );

		D3DXVec3TransformCoord( &vecVert, &vecVert, &pFrame->matAbsoluteWorld );
		p1.x = vecVert.x; p1.y = vecVert.y; p1.z = vecVert.z;
		
		vecVert.x = normV1.x; vecVert.y = normV1.y; vecVert.z = normV1.z;
		D3DXVec3TransformNormal( &vecVert, &vecVert, pmatTransInvAbsoluteWorld );
		normV1.x = vecVert.x; normV1.y = vecVert.y; normV1.z = vecVert.z;

		dwIndex = (i+1)*dwFVFSize;

		vecVert.x = *( (float*) ( pVertexData + dwIndex )     );
		vecVert.y = *( (float*) ( pVertexData + dwIndex + 4 ) );
		vecVert.z = *( (float*) ( pVertexData + dwIndex + 8 ) );
		normV2.x = *( (float*) ( pVertexData + dwIndex + 12 ) );
		normV2.y = *( (float*) ( pVertexData + dwIndex + 16 ) );
		normV2.z = *( (float*) ( pVertexData + dwIndex + 20 ) );
		pU2 = (float*) ( pVertexData + dwIndex + cOffsetMap.dwTU [ iLMStage ]*4 );
		pV2 = (float*) ( pVertexData + dwIndex + cOffsetMap.dwTV [ iLMStage ]*4 );

		D3DXVec3TransformCoord( &vecVert, &vecVert, &pFrame->matAbsoluteWorld );
		p2.x = vecVert.x; p2.y = vecVert.y; p2.z = vecVert.z;

		vecVert.x = normV2.x; vecVert.y = normV2.y; vecVert.z = normV2.z;
		D3DXVec3TransformNormal( &vecVert, &vecVert, pmatTransInvAbsoluteWorld );
		normV2.x = vecVert.x; normV2.y = vecVert.y; normV2.z = vecVert.z;

		dwIndex = (i+2)*dwFVFSize;

		vecVert.x = *( (float*) ( pVertexData + dwIndex )     );
		vecVert.y = *( (float*) ( pVertexData + dwIndex + 4 ) );
		vecVert.z = *( (float*) ( pVertexData + dwIndex + 8 ) );
		normV3.x = *( (float*) ( pVertexData + dwIndex + 12 ) );
		normV3.y = *( (float*) ( pVertexData + dwIndex + 16 ) );
		normV3.z = *( (float*) ( pVertexData + dwIndex + 20 ) );
		pU3 = (float*) ( pVertexData + dwIndex + cOffsetMap.dwTU [ iLMStage ]*4 );
		pV3 = (float*) ( pVertexData + dwIndex + cOffsetMap.dwTV [ iLMStage ]*4 );

		D3DXVec3TransformCoord( &vecVert, &vecVert, &pFrame->matAbsoluteWorld );
		p3.x = vecVert.x; p3.y = vecVert.y; p3.z = vecVert.z;

		vecVert.x = normV3.x; vecVert.y = normV3.y; vecVert.z = normV3.z;
		D3DXVec3TransformNormal( &vecVert, &vecVert, pmatTransInvAbsoluteWorld );
		normV3.x = vecVert.x; normV3.y = vecVert.y; normV3.z = vecVert.z;

		normV1.normalise( );
		normV2.normalise( );
		normV3.normalise( );
		float fDotP1 = normV1.dotProduct( &normV2 );
		float fDotP2 = normV1.dotProduct( &normV3 );

		Vector v1(&p1,&p3);
		Vector v2(&p1,&p2);

		v1 = v1.crossProduct(&v2);
		float length = v1.size();

		//zero area polygons will have an undefined normal, which will cause problems later
		//so if any exist remove them
		if (length<0.0001f) continue;

		v1.mult(1.0f/length);

		float fDotP3 = normV1.dotProduct( &v1 );

		LMPoly* pNewPoly;
		if ( bFlatShaded || ( fDotP1 > 0.995f && fDotP2 > 0.995f && fDotP3 > 0.995f ) )
		{
			//flat polygon
			pNewPoly = new LMPoly( );
			//iFlat++;
		}
		else
		{
			pNewPoly = new LMCurvedPoly( );
			((LMCurvedPoly*)pNewPoly)->SetVertexNormals( &normV1, &normV2, &normV3 );
			//iCurved++;
		}

		pNewPoly->pU1 = pU1; pNewPoly->pV1 = pV1;
		pNewPoly->pU2 = pU2; pNewPoly->pV2 = pV2;
		pNewPoly->pU3 = pU3; pNewPoly->pV3 = pV3;

		pNewPoly->SetVertices( &p1, &p2, &p3 );

		//Vector v1(&p1,&p3);
		//Vector v2(&p1,&p2);

		//v1 = v1.crossProduct(&v2);
		//float length = v1.size();

		//zero area polygons will have an undefined normal, which will cause problems later
		//so if any exist remove them
		//if (length<0.00001f) { delete pNewPoly; continue; }

		pNewPoly->GetAvgPoint( &fPosX, &fPosY, &fPosZ );
		fObjCenX += fPosX;
		fObjCenY += fPosY;
		fObjCenZ += fPosZ;

		//v1.mult(1.0f/length);

		pNewPoly->SetNormal( &v1 );
		v2.set(p2.x,p2.y,p2.z);
        
		pNewPoly->d = -1.0f*(v1.dotProduct(&v2));

		pNewPoly->Flatten( );
		//pNewPoly->Scale( fQuality );
		//pNewPoly->fQuality = fQuality;

		pNewPoly->colour = 0xff000000 | ((rand() % 255) << 16) | ((rand() % 255) << 8) | ((rand() % 255));

		pNewPoly->pNextPoly = pPolyList;
		pPolyList = pNewPoly;

		iNumPolys++;
	}

	delete pmatTransInvAbsoluteWorld;

	//char str[256];
	//sprintf_s( str, 255, "Flat: %d, Curved: %d", iFlat, iCurved );
	//MessageBox( NULL, str, "Info", 0 );

	if ( iNumPolys <= 0 ) return;

	fObjCenX /= iNumPolys;
	fObjCenY /= iNumPolys;
	fObjCenZ /= iNumPolys;

	fObjRadius = 0;
	float fNewRadius;
	LMPoly *pPoly = pPolyList;

	while ( pPoly )
	{
		fNewRadius = pPoly->GetMaxRadius( fObjCenX, fObjCenY, fObjCenZ );
		if ( fNewRadius > fObjRadius ) fObjRadius = fNewRadius;
		
		pPoly = pPoly->pNextPoly;
	}

	fObjRadius = sqrt( fObjRadius );

	//GroupPolys( fQuality );
}

void LMObject::GroupPolys( float fNewQuality )
{
	fQuality = fNewQuality;

	LMPoly *pPoly = pPolyList;
	LMPoly *pNextPoly;

	while ( pPoly )
	{
		pNextPoly = pPoly->pNextPoly;

		LMPolyGroup *pNewGroup = new LMPolyGroup( );
		pNewGroup->AddPoly( pPoly );

		int iNumIterations = 0;
		float fUsedSpace = pNewGroup->GetUsedSpace( );
		bool bFound = true;

		while ( fUsedSpace < 0.999f && iNumIterations < 20 && bFound ) 
		{ 
			bFound = false;

			LMPoly *pPoly2 = pNextPoly;
			LMPoly *pLastPoly2 = 0;
		
			LMPoly *pPolyIndex = 0;
			LMPoly *pLastPolyIndex = 0;

			//search for another poly to add to this group
			while ( pPoly2 )
			{
				if ( pNewGroup->Joined( pPoly2 ) )
				{
					float fNewUsedSpace = pNewGroup->GetNewUsedSpace( pPoly2 );

					if ( fNewUsedSpace > fUsedSpace )
					{
						fUsedSpace = fNewUsedSpace;
						pPolyIndex = pPoly2;
						pLastPolyIndex = pLastPoly2;
					}

					bFound = true;
				}
				
				pLastPoly2 = pPoly2;
				pPoly2 = pPoly2->pNextPoly;
			}

			if ( pPolyIndex )
			{
				if ( pPolyIndex == pNextPoly )
				{
					pNextPoly = pPolyIndex->pNextPoly;
				}
				else
				{
					pLastPolyIndex->pNextPoly = pPolyIndex->pNextPoly;
				}

				pNewGroup->AddPoly( pPolyIndex );
			}

			iNumIterations++;
		}
		
		
		pNewGroup->Scale( fQuality );
		pNewGroup->pNextGroup = pPolyGroupList;
		pPolyGroupList = pNewGroup;

		pPoly = pNextPoly;
	}

	pPolyList = 0;
}

void LMObject::SortGroups( )
{
	if ( !pPolyGroupList ) return;
	
	LMPolyGroup *pLastGroup = 0;
	LMPolyGroup *pGroup = pPolyGroupList;

	while ( pGroup )
	{
		int iMaxSize = pGroup->GetScaledSizeU( ) * pGroup->GetScaledSizeV( );

		LMPolyGroup *pLastGroup2 = 0;
		LMPolyGroup *pLastLargestGroup2 = 0;
		LMPolyGroup *pGroup2 = pGroup->pNextGroup;
		LMPolyGroup *pLargestGroup = 0;

		while ( pGroup2 )
		{
			int iNewSize = pGroup2->GetScaledSizeU( ) * pGroup2->GetScaledSizeV( );

			if ( iNewSize > iMaxSize )
			{
				iMaxSize = iNewSize;
				pLargestGroup = pGroup2;
				pLastLargestGroup2 = pLastGroup2;
			}

			pLastGroup2 = pGroup2;
			pGroup2 = pGroup2->pNextGroup;
		}

		if ( pLargestGroup )
		{
			LMPolyGroup *pTemp = pGroup->pNextGroup;
			LMPolyGroup *pTemp2 = pLargestGroup->pNextGroup;

			if ( pTemp == pLargestGroup ) pTemp = pGroup;

			pLargestGroup->pNextGroup = pTemp;
			pGroup->pNextGroup = pTemp2;

			if ( pLastLargestGroup2 ) pLastLargestGroup2->pNextGroup = pGroup;
			
			if ( !pLastGroup )
				pPolyGroupList = pLargestGroup;
			else
				pLastGroup->pNextGroup = pLargestGroup;

			pLastGroup = pLargestGroup;
			pGroup = pLargestGroup->pNextGroup;
		}
		else
		{
			pLastGroup = pGroup;
			pGroup = pGroup->pNextGroup;
		}
	}
}

void LMObject::ReScaleGroups( float fNewQuality )
{
	fQuality = fNewQuality;
	
	LMPolyGroup *pGroup = pPolyGroupList;

	while ( pGroup )
	{
		pGroup->Scale( fQuality );

		pGroup = pGroup->pNextGroup;
	}
}

void LMObject::SetLocalValues( const Light *pLightList, const CollisionTree *pColTree, int iBlur, HANDLE pSemaphore )
{
	pLocalLightList = pLightList;
	pLocalColTree = pColTree;
	iLocalBlur = iBlur;
	pLocalSemaphore = pSemaphore;
}

void LMObject::WaitForThreads( )
{
	LMPolyGroup *pGroup = pPolyGroupList;

	while ( pGroup )
	{
//		pGroup->Join( );

		pGroup = pGroup->pNextGroup;
	}
}

unsigned int LMObject::Run( )
{
	CalculateLight( pLocalLightList, pLocalColTree, iLocalBlur, 0 );
	ApplyToTexture( );

	ReleaseSemaphore( pLocalSemaphore, 1, NULL );

	return 0;
}

void LMObject::CalculateLight( const Light *pLightList, const CollisionTree *pColTree, int iBlur, int iThreadLimit )
{
	LMPolyGroup *pGroup = pPolyGroupList;
	while ( pGroup )
	{
		pGroup->CalculateLight( pLightList, pColTree, iBlur );
		pGroup = pGroup->pNextGroup;
	}
}

void LMObject::ApplyToTexture( )
{
	if ( !pLMTexture ) return ;

	LMPolyGroup *pGroup = pPolyGroupList;

	while ( pGroup )
	{
		pGroup->ApplyToTexture( pLMTexture );

		pGroup = pGroup->pNextGroup;
	}
}

void LMObject::CalculateVertexUV( int iTexSizeU, int iTexSizeV )
{
	LMPolyGroup *pPolyGroup = pPolyGroupList;

	while ( pPolyGroup )
	{
		pPolyGroup->CalculateTexUV( iTexSizeU, iTexSizeV );

		pPolyGroup = pPolyGroup->pNextGroup;
	}
}

void LMObject::UpdateObject( int iBlendMode )
{
	if ( !pLMTexture )return;
	if ( !pMesh ) return;

	if ( !pMesh->pTextures )
	{
		MessageBox( NULL, "Invalid Texture List", "Error", 0 );
		return;
	}

	//if ( pMesh->dwTextureCount < 2 ) MessageBox( NULL, "Not enough textures", "Error", 0 );
	//if ( !pLMTexture->GetDXTextureRef( ) ) MessageBox( NULL, "Invalid Texture Pointer", "Error", 0 );

	/*
	if ( pMesh->dwTextureCount < 2 )
	{
		sTexture* pNewTextureList = new sTexture [ 2 ];
		memset( (void*)pNewTextureList, 0, sizeof(sTexture)*2 );

		if ( pMesh->dwTextureCount >= 1 )
		{
			pNewTextureList [ 0 ] = pMesh->pTextures [ 1 ];
		}

		delete [] ( pMesh->pTextures );

		pMesh->pTextures = pNewTextureList;
		pMesh->dwTextureCount = 2;
	}
	*/

	if ( pMesh->dwTextureCount < 2 && !bShaded )
	{
		char str[256];
		sprintf_s(str,255,"Not Enough Texture Stages (%d)", pMesh->dwTextureCount );
		MessageBox( NULL, str, "Error", 0 );
		return;
	}

	int iLMStage = 0;

	if ( !bShaded )
	{
		if ( iBaseStage >= 0 ) pMesh->pTextures [ 1 ]					= pMesh->pTextures [ iBaseStage ];
	}
	else
		iLMStage = iBaseStage;

	//pMesh->mMaterial.Diffuse.r = 0.0f;
	//pMesh->mMaterial.Diffuse.g = 0.0f;
	//pMesh->mMaterial.Diffuse.b = 0.0f;

	// setup lightmap as FIRST stage (diffuse+lightmap)
	pMesh->pTextures [ iLMStage ].dwStage			= iLMStage;
	pMesh->pTextures [ iLMStage ].iImageID			= 0;
	pMesh->pTextures [ iLMStage ].pTexturesRef		= pLMTexture->GetDXTextureRef( );
	strcpy_s ( pMesh->pTextures [ iLMStage ].pName, 255   , pLMTexture->GetFilename( ) );
	
	if ( !bShaded )
	{
		if ( iDynamicLightMode == 0 )
		{
			pMesh->pTextures [ iLMStage ].dwBlendMode		= D3DTOP_SELECTARG1;
			pMesh->bLight = false;
		}
		else
		{
			pMesh->pTextures [ iLMStage ].dwBlendMode		= D3DTOP_ADD;
			pMesh->bLight = true;
		}
	}

	pMesh->pTextures [ iLMStage ].dwBlendArg1		= D3DTA_TEXTURE;
	if ( !bShaded ) pMesh->pTextures [ iLMStage ].dwBlendArg2 = D3DTA_DIFFUSE;
	else pMesh->pTextures [ iLMStage ].dwBlendArg2 = D3DTA_CURRENT;
	pMesh->pTextures [ iLMStage ].dwAddressU		= D3DTADDRESS_CLAMP;
	pMesh->pTextures [ iLMStage ].dwAddressV		= D3DTADDRESS_CLAMP;
	pMesh->pTextures [ iLMStage ].dwMagState		= D3DTEXF_LINEAR;//2x2 sample
	pMesh->pTextures [ iLMStage ].dwMinState		= D3DTEXF_LINEAR;
//		pMesh->pTextures [ iLMStage ].dwMagState		= D3DTEXF_NONE;//test-nofilter
//		pMesh->pTextures [ iLMStage ].dwMinState		= D3DTEXF_NONE;
	pMesh->pTextures [ iLMStage ].dwMipState		= D3DTEXF_NONE;
	pMesh->pTextures [ iLMStage ].dwTexCoordMode	= 10 + iLMStage;

	// copy base texture to SECOND stage (current*basetexture)
	if ( !bShaded )
	{
		pMesh->pTextures [ 1 ].dwStage			= 1;
		pMesh->pTextures [ 1 ].dwBlendMode		= iBlendMode;
		pMesh->pTextures [ 1 ].dwBlendArg1		= D3DTA_TEXTURE;
		pMesh->pTextures [ 1 ].dwBlendArg2		= D3DTA_CURRENT;
		pMesh->pTextures [ 1 ].dwTexCoordMode	= 10;
		pMesh->pTextures [ 0 ].dwTexCoordMode	= 11;
	}
	
	if ( pMesh->dwTextureCount > 2 && !bShaded )
	{
		for ( DWORD i = 2; i < pMesh->dwTextureCount; i++ )
		{
			if ( !pMesh->pTextures [ i ].pTexturesRef ) 
			{
			//	pMesh->pTextures [ i ].dwBlendMode		= D3DTOP_DISABLE;
			//	pMesh->pTextures [ i ].dwBlendArg2		= D3DTA_CURRENT;
			}

			//char str[256];
			//sprintf_s(str,255,"Stage: %d", pMesh->pTextures [ i ].dwStage );
			//MessageBox( NULL, str, "Info", 0 );

			pMesh->pTextures [ i ].dwBlendMode		= D3DTOP_DISABLE;
			pMesh->pTextures [ i ].dwBlendArg2		= D3DTA_CURRENT;
			pMesh->pTextures [ i ].dwBlendArg1		= D3DTA_TEXTURE;
			pMesh->pTextures [ i ].dwStage			= i;
			pMesh->pTextures [ i ].iImageID			= -1;
			pMesh->pTextures [ i ].pTexturesRef		= 0;
		}
	}

	// signal vb refresh
	pMesh->bVBRefreshRequired = true;
}