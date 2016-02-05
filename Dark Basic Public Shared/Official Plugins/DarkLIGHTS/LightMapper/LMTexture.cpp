
#include "LMGlobal.h"
#include "DBPro Functions.h"

#include "LMTexture.h"
#include "Lumel.h"
#include "LMObject.h"
#include "LMPolyGroup.h"
#include "LMTexNode.h"
#include "Light.h"
#include "CollisionTree.h"

LMTexture::LMTexture( int sizeU, int sizeV )
{
	iSizeU = sizeU;
	iSizeV = sizeV;
	bEmpty = true;

	ppTexLumel = new Lumel* [ iSizeU ];

	for ( int i = 0; i < iSizeU; i++ ) ppTexLumel [ i ] = new Lumel [ iSizeV ];

	pNextLMTex = 0;
	pNodeSpace = new LMTexNode( 0, 0, sizeU, sizeV );
	pNodeSpace->Finalise( );
	
	pPixels = 0;
	iPitch = 0;
	iWidth = 0;
	iHeight = 0;

	pTexture = 0;
	pTextureDDS = 0;

	sprintf_s( pFilename, 255, "0" );

	//pLocalLightList = 0;
	//pLocalColTree = 0;
}

LMTexture::~LMTexture( )
{
	if ( ppTexLumel ) 
	{
		for ( int i = 0; i < iSizeU; i++ ) delete [] ( ppTexLumel [ i ] );
		delete [] ppTexLumel;
	}

	if ( pNodeSpace ) delete pNodeSpace;
	if ( pNextLMTex ) delete pNextLMTex;
}

bool LMTexture::IsEmpty( ) { return bEmpty; }

int LMTexture::GetSizeU( ) { return iSizeU; }
int LMTexture::GetSizeV( ) { return iSizeV; }

bool LMTexture::AddLMObject( LMObject *pLMObject )
{
	LMPolyGroup* pPoly = pLMObject->GetFirstGroup( );

	//FILE* pData = fopen("TextureData.txt","w");
	//char infoStr[256];
		
	bool bFits = true;

	while ( pPoly && bFits )
	{
		int iPolySizeU = pPoly->GetScaledSizeU( );
		int iPolySizeV = pPoly->GetScaledSizeV( );

		LMTexNode* pNode = pNodeSpace->AddPoly( iPolySizeU, iPolySizeV );

		if ( !pNode )
		{
			//polygon didn't fit
			bFits = false;
			pNodeSpace->BackTrack( );
			//sprintf(infoStr,"Didn't Fit, SizeU: %d, SizeV: %d\n", iPolySizeU, iPolySizeV );
			//fputs(infoStr,pData);
		}
		else
		{
			int iPolyStartU = pNode->GetStartU( );
			int iPolyStartV = pNode->GetStartV( );
			
			pPoly->SetStartPoint( iPolyStartU, iPolyStartV );
			//pPoly->fQuality = fQuality;

			//sprintf(infoStr,"Fits, StartU: %d, StartV: %d, SizeU: %d, SizeV: %d\n", iPolyStartU, iPolyStartV, iPolySizeU, iPolySizeV );
			//fputs(infoStr,pData);
			
			//assign polygon to Lumels
			/*
			for ( int u = iPolyStartU; u < iPolyStartU + iPolySizeU; u++ )
			{
				for ( int v = iPolyStartV; v < iPolyStartV + iPolySizeV; v++ )
				{
					ppTexLumel[u][v].pRefPoly = pPoly;
				}
			}
			*/

		}

		pPoly = pPoly->pNextGroup;
	}

	//fclose(pData);

	if ( !bFits )
	{
		//object did't fit in texture
		//if this texture was empty it'll never fit
		if ( bEmpty ) return false;
		
		if ( pNextLMTex )
		{
			return pNextLMTex->AddLMObject( pLMObject );
		}
		else
		{
			pNextLMTex = new LMTexture( iSizeU, iSizeV );
			return pNextLMTex->AddLMObject( pLMObject );
		}
	}
	else
	{
		//object completely within lightmap
		pNodeSpace->Finalise( );
		bEmpty = false;

		pLMObject->pLMTexture = this;

		return true;
	}
}
/*
void LMTexture::CalculateLight( Light *pLightList, CollisionTree *pColTree )
{
	if ( !pLightList ) return;

	Light *pCurrLight;
	float fRed, fGreen, fBlue;
	float fPosX=0, fPosY=0, fPosZ=0;
	float fLightPosX=0, fLightPosY=0, fLightPosZ=0;
	float fNormX=0, fNormY=0, fNormZ=0;
	float fDotP=0, fLength=0;
	Vector vec, vecN, vecI;
	Point p;

	//for each light
	pCurrLight = pLightList;
	while ( pCurrLight )
	{

		for ( int u = 0; u < iSizeU; u++ )
		{
			for ( int v = 0; v < iSizeV; v++ )
			{
				if ( ppTexLumel[u][v].pRefPoly )
				{
					ppTexLumel[u][v].pRefPoly->GetPoint( u, v, &fPosX, &fPosY, &fPosZ );
					ppTexLumel[u][v].pRefPoly->GetNormal( u, v, &fNormX, &fNormY, &fNormZ );

					pCurrLight->GetOrgin( fPosX, fPosY, fPosZ, &fLightPosX, &fLightPosY, &fLightPosZ );
					vec.set( fPosX-fLightPosX, fPosY-fLightPosY, fPosZ-fLightPosZ );
					fDotP = vec.x*fNormX + vec.y*fNormY + vec.z*fNormZ;

					if ( fDotP >= 0.0f || !pCurrLight->GetColorFromPoint( fPosX, fPosY, fPosZ, &fRed, &fGreen, &fBlue ) )
					{
						//pCurrLight = pCurrLight->pNextLight;
						continue;
					}
					
					//calculate + normalise the light->lumel vector
					fLength = vec.size( );
					vec.mult(1.0f/fLength);
					vecN = vec;
					vec.mult(fLength-0.1f);
					vecI.set( 1/vec.x, 1/vec.y, 1/vec.z );
					p.set ( fLightPosX, fLightPosY, fLightPosZ );
					
					//check collision
					if ( pColTree && pColTree->intersects( &p, &vec, &vecN, &vecI, true ) )
					{
					//	pCurrLight = pCurrLight->pNextLight;
						continue;
					}

					fDotP	/= fLength;
					fDotP	 = 0.0f - fDotP;
					fRed	*= fDotP;
					fGreen	*= fDotP;
					fBlue	*= fDotP;

					//add color to lumel
					ppTexLumel[u][v].AddCol( fRed, fGreen, fBlue );

					
				}
			}
		}

		pCurrLight = pCurrLight->pNextLight;
	}

	for ( int u = 0; u < iSizeU; u++ )
	{
		for ( int v = 0; v < iSizeV; v++ )
		{
			ppTexLumel[u][v].Ambient( Light::fAmbientR, Light::fAmbientG, Light::fAmbientB );
		}
	}
}
*/

void LMTexture::SetLumel( int u, int v, float red, float green, float blue )
{
	if ( u < 0 || v < 0 || u >= iSizeU || v >= iSizeV ) return;

	ppTexLumel[u][v].SetCol( red, green ,blue );
}

void LMTexture::CopyToTexture( )
{
	if ( FAILED ( g_pD3D->CreateTexture ( iSizeU, iSizeV, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL ) ) )
	{
		MessageBox( NULL, "Error Creating Light Map Texture", "Error", 0 ); exit(1);
	}

	D3DLOCKED_RECT	d3dlr;
	D3DSURFACE_DESC desc;

	if ( FAILED ( pTexture->GetLevelDesc ( 0, &desc ) ) )
	{
		MessageBox( NULL, "Error Getting Texture Description", "Error", 0 ); exit(1);
	}

	if ( FAILED ( pTexture->LockRect ( 0, &d3dlr, 0, 0 ) ) )
	{
		MessageBox( NULL, "Error Locking Texture", "Error", 0 ); exit(1);
	}

	pPixels = ( DWORD* ) d3dlr.pBits;
	iPitch  = d3dlr.Pitch / 4;
	iWidth  = desc.Width;
	iHeight = desc.Height;

	DWORD dwIndex;

	for ( int v = 0; v < iSizeV; v++ )
	{
		for ( int u = 0; u < iSizeU; u++ )
		{
			dwIndex = u + ( v * iPitch );

			//if ( ppTexLumel[u][v].pRefPoly )//&& ppTexLumel[u][v].pRefPoly->PointInGroup( u, v ) )
			{
				*( pPixels + dwIndex ) = ppTexLumel[u][v].GetColor( );
				//*( pPixels + dwIndex ) = ppTexLumel[u][v].pRefPoly->GetColour( u, v );
			}
			//else
			{
			//	*( pPixels + dwIndex ) = 0xff000000;
			}
		}
	}

	pTexture->UnlockRect ( NULL );

	
	for ( int i = 0; i < iSizeU; i++ ) delete [] ( ppTexLumel [ i ] );
	delete [] ppTexLumel;
	ppTexLumel = 0;

	if ( pNodeSpace ) delete pNodeSpace;
	pNodeSpace = 0;
	

	if ( FAILED ( g_pD3D->CreateTexture ( iSizeU, iSizeV, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pTextureDDS, NULL ) ) )
	{
		MessageBox( NULL, "Failed To Create DDS Texture", "Error", 0 );
		exit(1);
	}

	LPDIRECT3DSURFACE9 pDDSSurface;
	pTextureDDS->GetSurfaceLevel ( 0, &pDDSSurface );
	if ( pDDSSurface )
	{
		LPDIRECT3DSURFACE9 pMasterSurface;
		pTexture->GetSurfaceLevel ( 0, &pMasterSurface );
		if ( pMasterSurface )
		{
			D3DXLoadSurfaceFromSurface ( pDDSSurface, NULL, NULL, pMasterSurface, NULL, NULL, D3DX_DEFAULT, 0 );
			pMasterSurface->Release( );
		}
		pDDSSurface->Release( );
	}

	pTexture->Release( );
}

void LMTexture::SaveTexture( char* pNewFilename )
{
	strcpy_s( pFilename, 255, pNewFilename );
	
	switch( g_iLightmapFileFormat )
	{
		case 0: D3DXSaveTextureToFile( pFilename, D3DXIFF_PNG, pTextureDDS, NULL ); break;
		case 1: D3DXSaveTextureToFile( pFilename, D3DXIFF_DDS, pTextureDDS, NULL ); break;
		case 2: D3DXSaveTextureToFile( pFilename, D3DXIFF_BMP, pTextureDDS, NULL ); break;
		default: D3DXSaveTextureToFile( pFilename, D3DXIFF_PNG, pTextureDDS, NULL ); break;
	}
}

IDirect3DTexture9* LMTexture::GetDXTextureRef( )
{
	return pTextureDDS;
}

char* LMTexture::GetFilename( )
{
	return pFilename;
}

/*
unsigned int LMTexture::Run( )
{
	if ( !pLocalLightList || ! pLocalColTree ) return 1;

	CalculateLight( pLocalLightList, pLocalColTree );

	return 0;
}
*/