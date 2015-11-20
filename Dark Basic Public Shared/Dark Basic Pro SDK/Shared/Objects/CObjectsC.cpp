
//
// CObjectsC Functions Implementation
//

//////////////////////////////////////////////////////////////////////////////////
// INCLUDE OBJECTC ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable:4800)
#include "CObjectsNewC.h"
#include ".\..\Core\SteamCheckForWorkshop.h"

// Occlusion object global
#include "Occlusion\cOcclusion.h"
extern COcclusion g_Occlusion;

// External Globals
extern bool g_bSwitchLegacyOn;
extern bool g_bFastBoundsCalculation;
extern D3DXHANDLE g_pMainCameraDepthHandle;
extern LPD3DXEFFECT g_pMainCameraDepthEffect;

// Global Lists
std::vector< D3DXHANDLE > g_EffectParamHandleList;

// Global Intersect All Helpers
struct OrderByCamDistance
{
    bool operator()(sObject* pObjectA, sObject* pObjectB)
    {
        if (pObjectA->position.fCamDistance < pObjectB->position.fCamDistance)
            return true;
        if (pObjectA->position.fCamDistance == pObjectB->position.fCamDistance)
            return (pObjectA->dwObjectNumber < pObjectB->dwObjectNumber);
        return false;
    }
};
std::vector< sObject* > g_pIntersectShortList;

// Global to store a second range of objects for IntersectAll special mode
int g_iIntersectAllSecondStart = 0;
int g_iIntersectAllSecondEnd = 0;
// Globals for a third rane of objects
int g_iIntersectAllThirdStart = 0;
int g_iIntersectAllThirdEnd = 0;
//#define SKIPGRIDUSED
#ifdef SKIPGRIDUSED
float g_fIntersectAllSkipGridX = 0;
float g_fIntersectAllSkipGridZ = 0;
DWORD g_dwSkipGrid[1024][1024];
int g_iSkipGridResult[1024][1024];
#endif

// Global mem management
//extern DWORD g_dwMemoryConsumed;

// For DB_ObjectScreenData for more accurate reporting of 'in screen' setting.
namespace
{
    // u74b7 - Generate the frustum planes from the transformation matrix
    void ExtractFrustumPlanes(D3DXPLANE p_Planes[6], const D3DXMATRIX & matCamera)
    {
        // Left clipping plane
        p_Planes[0].a = matCamera._14 + matCamera._11;
        p_Planes[0].b = matCamera._24 + matCamera._21;
        p_Planes[0].c = matCamera._34 + matCamera._31;
        p_Planes[0].d = matCamera._44 + matCamera._41;

        // Right clipping plane
        p_Planes[1].a = matCamera._14 - matCamera._11;
        p_Planes[1].b = matCamera._24 - matCamera._21;
        p_Planes[1].c = matCamera._34 - matCamera._31;
        p_Planes[1].d = matCamera._44 - matCamera._41;
        
        // Top clipping plane
        p_Planes[2].a = matCamera._14 - matCamera._12;
        p_Planes[2].b = matCamera._24 - matCamera._22;
        p_Planes[2].c = matCamera._34 - matCamera._32;
        p_Planes[2].d = matCamera._44 - matCamera._42;
        
        // Bottom clipping plane
        p_Planes[3].a = matCamera._14 + matCamera._12;
        p_Planes[3].b = matCamera._24 + matCamera._22;
        p_Planes[3].c = matCamera._34 + matCamera._32;
        p_Planes[3].d = matCamera._44 + matCamera._42;
        
        // Near clipping plane
        p_Planes[4].a = matCamera._13;
        p_Planes[4].b = matCamera._23;
        p_Planes[4].c = matCamera._33;
        p_Planes[4].d = matCamera._43;
        
        // Far clipping plane
        p_Planes[5].a = matCamera._14 - matCamera._13;
        p_Planes[5].b = matCamera._24 - matCamera._23;
        p_Planes[5].c = matCamera._34 - matCamera._33;
        p_Planes[5].d = matCamera._44 - matCamera._43;
        
        // Normalise the planes
        for (int i = 0; i < 6; ++i)
            D3DXPlaneNormalize(&p_Planes[i], &p_Planes[i]);
    }

    // Calculate the minimum signed distance from the plane to a point
    inline float DistancePlaneToPoint(const D3DXPLANE & Plane, const D3DXVECTOR3 & pt)
    {
        return Plane.a*pt.x + Plane.b*pt.y + Plane.c*pt.z + Plane.d;
    }

    bool ContainsSphere(const D3DXPLANE p_Planes[6], const D3DXVECTOR3& vecCentre, const float fRadius)
    {
	    // calculate if sphere is on the correct 'side' of each plane
	    for(int i = 0; i < 6; ++i)
        {
            // If sphere on the wrong side, we're done
            if (DistancePlaneToPoint(p_Planes[i], vecCentre) < -fRadius)
                 return false;
	    }

	    // otherwise we are fully in view
	    return true;
    }
    
    /*
    inline bool ContainsPoint(const D3DXPLANE p_Plane[6], const D3DXVECTOR3& vecPoint)
    {
        return ContainsSphere(p_Plane, vecPoint, 0.0);
    }
    */

    float ApplyPivot ( sObject* pObject, int iMode, D3DXVECTOR3 vecValue, float fValue )
    {
	    if ( pObject->position.bApplyPivot )
	    {
		    D3DXVec3TransformCoord ( &vecValue, &vecValue, &pObject->position.matPivot );

		    if ( iMode == 0 ) return vecValue.x;
		    if ( iMode == 1 ) return vecValue.y;
		    if ( iMode == 2 ) return vecValue.z;
	    }

	    return fValue;
    }

    SDK_FLOAT GetAxisSizeFromVectorOffset ( int iID, int iActualSize, int iVectorOffset )
    {
        // iActualSize is 0 = unscaled, 1 = scaled
        // iVectorOffset is 0 = x, 1 = y, 2 = z

	    // Check the object exists
	    if ( !ConfirmObjectInstance ( iID ) )
		    return 0;

        // Get the object pointer
	    sObject* pObject = g_ObjectList [ iID ];

        // If the object is an instance, grab the scaling from the instance (to be applied later)
        // and move on to the object itself
        float fAdjustScale = 1.0;
	    if ( pObject->pInstanceOfObject )
        {
            fAdjustScale = pObject->position.vecScale[ iVectorOffset ];
            pObject = pObject->pInstanceOfObject;
        }

        // Get the precomputed size of the objects x dimension
        float fValue = (pObject->collision.vecMax[ iVectorOffset ] - pObject->collision.vecMin[ iVectorOffset ]);
    	
	    // Apply pivot if needed
	    //fValue = ApplyPivot ( pObject, 0, D3DXVECTOR3 ( pObject->collision.vecMax - pObject->collision.vecMin ), fValue );
	    fValue = ApplyPivot ( pObject, iVectorOffset, D3DXVECTOR3 ( pObject->collision.vecMax - pObject->collision.vecMin ), fValue );

	    // Ensure size is reported as positive
	    fValue = fabs ( fValue );

	    // Adjusts to scale now
	    if ( iActualSize==1 )
            fValue = fValue * pObject->position.vecScale[ iVectorOffset ] * fAdjustScale;

	    return *(DWORD*)&fValue;
    }

}

//////////////////////////////////////////////////////////////////////////////////
// COMMANDS //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK_DLL void RefreshMeshShortList ( sMesh* pMesh )
{
	if ( pMesh==NULL ) return;
	g_vRefreshMeshList.push_back ( pMesh );
}

DARKSDK_DLL void LoadCore ( SDK_LPSTR szFilename, int iID, int iDBProMode, int iDivideTextureSize )
{
	// ensure the object is okay to use
	ConfirmNewObject ( iID );

	// check memory allocation
	ID_ALLOCATION ( iID );

	// load the object
	if ( !LoadDBO ( (LPSTR)szFilename, &g_ObjectList [ iID ] ) )
		return;

	// setup new object and introduce to buffers
	if ( !SetNewObjectFinalProperties ( iID, -1.0f ) )
		return;

	if ( g_ObjectList [ iID ]->iMeshCount == 0 )
		return;

	// add object id to shortlist
	AddObjectToObjectListRef ( iID );

	// calculate path from filename
	char szPath [ MAX_PATH ];
	if ( _strnicmp ( (char*)szFilename+strlen((char*)szFilename)-4, ".mdl", 4 )==NULL )
	{
		// MDL models store their textures in the temp folder
		DBOCalculateLoaderTempFolder();
		strcpy ( szPath, g_WindowsTempDirectory );
	}
	else
	{
		// Path is current model location
		strcpy( szPath, "" );
		LPSTR pFile = (LPSTR)szFilename;
		DWORD dwLength = strlen(pFile);
		for ( int n=dwLength; n>0; n-- )
		{
			if ( pFile[n]=='\\' || pFile[n]=='/' )
			{
				strcpy ( szPath, pFile );
				szPath[n+1]=0;
				break;
			}
		}	
	}

	// prepare textures for all meshes (load them)
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		LoadInternalTextures ( pObject->ppMeshList [ iMesh ], szPath, iDBProMode, iDivideTextureSize );

	// 250704 - prepare effects for all meshes (load them)
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		// get mesh ptr
		sMesh* pMesh = pObject->ppMeshList [ iMesh ];
		if ( pMesh->bUseVertexShader )
		{
			// Search if effect already loaded (else create a new)
			CreateNewOrSharedEffect ( pMesh, true );
		}
	}
}

DARKSDK_DLL void Load ( SDK_LPSTR szFilename, int iID )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, (LPSTR)szFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	// store current folder
	char pStoreCurrentDir[_MAX_PATH];
	GetCurrentDirectory ( _MAX_PATH, pStoreCurrentDir );

	bool bTempFolderChangeForEncrypt = CheckForWorkshopFile (VirtualFilename);

	char pPathToOriginalFile[_MAX_PATH];
	if ( bTempFolderChangeForEncrypt )
	{
		strcpy ( pPathToOriginalFile, "" );
		FILE* tempFile = NULL;
		tempFile = fopen ( VirtualFilename ,"r" );
		if ( tempFile )
		{
			// get relative path from current
			strcpy ( pPathToOriginalFile, VirtualFilename );
			for(DWORD n=strlen(pPathToOriginalFile)-1; n>0; n--)
			{
				if(pPathToOriginalFile[n]=='\\' || pPathToOriginalFile[n]=='/' || (unsigned char)(pPathToOriginalFile[n])<32)
				{
					pPathToOriginalFile[n]=0;
					break;
				}
			}

			fclose ( tempFile );
		}
	}

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );

	// if encrypting model file (and model MAY load internal textures, ensure current directory is temporarily in model file folder
	if ( bTempFolderChangeForEncrypt==true )
	{
		// assign new one (at original model file location)
		SetCurrentDirectory ( pPathToOriginalFile );
	}

	// Load media
	LoadCore ( (SDK_LPSTR)VirtualFilename, iID, 0, 0 );

	// restore current directory
	if ( bTempFolderChangeForEncrypt==true )
	{
		SetCurrentDirectory ( pStoreCurrentDir );
		bTempFolderChangeForEncrypt = false;
	}

	// Re-encrypt
	g_pGlob->Encrypt( (DWORD)VirtualFilename );

}

DARKSDK_DLL void Load ( SDK_LPSTR szFilename, int iID, int iDBProMode )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, (LPSTR)szFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	CheckForWorkshopFile (VirtualFilename);

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	LoadCore ( (SDK_LPSTR)VirtualFilename, iID, iDBProMode, 0 );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );
}

DARKSDK_DLL void Load ( SDK_LPSTR szFilename, int iID, int iDBProMode, int iDivideTextureSize )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, (LPSTR)szFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	CheckForWorkshopFile (VirtualFilename);

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	LoadCore ( (SDK_LPSTR)VirtualFilename, iID, iDBProMode, iDivideTextureSize );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );
}

DARKSDK_DLL void Save ( SDK_LPSTR szFilename, int iID )
{
	// ensure the object is present
	if ( !ConfirmObject ( iID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// check ptr valid
	LPSTR pDBPFilename = (LPSTR)szFilename;
	if ( pDBPFilename )
	{
		// U78 - if OBJ extension detected, switch to OBJ exporter
		// OBJ is a static format (no transforms, no animation, not much really)
		if ( strnicmp ( pDBPFilename + strlen(pDBPFilename) - 4, ".obj", 4 )==NULL )
		{
			// vertex indices are global to the file
			DWORD dwStartOfVertexBatch = 1;

			// Get just the name
			char pJustObjName[512];
			strcpy ( pJustObjName, pDBPFilename );
			pJustObjName[strlen(pJustObjName)-4]=0;

			// MTL file
			char pMTLFilename[512];
			strcpy ( pMTLFilename, pJustObjName );
			strcat ( pMTLFilename, ".mtl" );

			// open MTL file for writing
			HANDLE hMTLfile = CreateFile ( pMTLFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
			if ( hMTLfile != INVALID_HANDLE_VALUE )
			{
				// write OBJ format
				LPSTR pLine = 0;
				DWORD byteswritten=0;
				HANDLE hfile = CreateFile ( pDBPFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
				if ( hfile != INVALID_HANDLE_VALUE )
				{
					// header
					pLine = "# OBJ Model File converted by the mighty Game Creators\n";
					WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 
					pLine = "# more tools found at www.thegamecreators.com\n";
					WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 
					pLine = "\n";
					WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 

					// material file name
					pLine = "# Material library\n";
					WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 
					char pDynLine[512];
					sprintf ( pDynLine, "mtllib %s\n\n", pMTLFilename );
					WriteFile( hfile, pDynLine, strlen(pDynLine), &byteswritten, NULL );

					// object name
					pLine = "# Object\n";
					WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 
					sprintf ( pDynLine, "o %s\n", pJustObjName );
					WriteFile( hfile, pDynLine, strlen(pDynLine), &byteswritten, NULL );
					pLine = "\n";
					WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 

					// only one mesh
					for ( int iMeshIndex=0; iMeshIndex<pObject->iMeshCount; iMeshIndex++ )
					{
						// make a new mesh from the original mesh, and ensure it's verts only
						sMesh* pVertOnlyMesh = new sMesh;
						sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
						MakeMeshFromOtherMesh       ( true, pVertOnlyMesh, pMesh, NULL );
						ConvertLocalMeshToVertsOnly ( pVertOnlyMesh );

						// group name
						pLine = "# Mesh\n";
						WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 
						pLine = "g mesh\n";
						WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 
						pLine = "\n";
						WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 

						// vertices
						pLine = "# Vertex list\n";
						WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 

						// for each vertex
						BYTE* pVertData = pVertOnlyMesh->pVertexData;
						for ( DWORD dwV=0; dwV<pVertOnlyMesh->dwVertexCount; dwV++ )
						{
							sprintf ( pDynLine, "v %f %f %f\n", (float)*((float*)pVertData+0), (float)*((float*)pVertData+1), (float)*((float*)pVertData+2) );
							WriteFile( hfile, pDynLine, strlen(pDynLine), &byteswritten, NULL );
							pVertData+=pVertOnlyMesh->dwFVFSize;
						}
						pVertData = pVertOnlyMesh->pVertexData;
						for ( DWORD dwV=0; dwV<pVertOnlyMesh->dwVertexCount; dwV++ )
						{
							float fReverseVCoordForOBJ = -(float)*((float*)pVertData+7);
							sprintf ( pDynLine, "vt %f %f\n", (float)*((float*)pVertData+6), fReverseVCoordForOBJ );
							WriteFile( hfile, pDynLine, strlen(pDynLine), &byteswritten, NULL );
							pVertData+=pVertOnlyMesh->dwFVFSize;
						}
						pVertData = pVertOnlyMesh->pVertexData;
						for ( DWORD dwV=0; dwV<pVertOnlyMesh->dwVertexCount; dwV++ )
						{
							sprintf ( pDynLine, "vn %f %f %f\n", (float)*((float*)pVertData+3), (float)*((float*)pVertData+4), (float)*((float*)pVertData+5) );
							WriteFile( hfile, pDynLine, strlen(pDynLine), &byteswritten, NULL );
							pVertData+=pVertOnlyMesh->dwFVFSize;
						}
						pLine = "\n";
						WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 

						// faces
						pLine = "# Face list\n";
						WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL );

						// texture filename
						// no guarentee its in the X file - but we try first
						// else we use the name of the model file
						char pFileOnlyNoExt[512];
						for ( int iTry=0; iTry<2; iTry++ )
						{
							LPSTR pOrigPathAndFile = NULL;
							if ( iTry==0 ) pOrigPathAndFile = pMesh->pTextures[0].pName;
							if ( iTry==1 ) pOrigPathAndFile = pDBPFilename;
							strcpy ( pFileOnlyNoExt, "" );
							if ( pOrigPathAndFile )
							{
								// trim off any paths first
								strcpy ( pFileOnlyNoExt, pOrigPathAndFile );
								for ( int n=strlen(pOrigPathAndFile)-1; n>0; n-- )
								{
									if ( pOrigPathAndFile[n]=='\\' || pOrigPathAndFile[n]=='/' )
									{
										strcpy ( pFileOnlyNoExt, pOrigPathAndFile+n+1 );
										n=0; break;
									}
								}

								// now we see if any variations of this filename exists
								if ( strlen(pFileOnlyNoExt)>4 )
								{
									pFileOnlyNoExt[strlen(pFileOnlyNoExt)-4]=0;
									strcat ( pFileOnlyNoExt, ".png" );
									HANDLE hExists = CreateFile( pFileOnlyNoExt, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
									if ( hExists!=INVALID_HANDLE_VALUE )
									{
										// this texture file exists - we have our texture name
										CloseHandle( hExists );
										iTry=99;
									}
									else
									{
										// the PNG of the named texture does not exist, but sometimes
										// texture names are truncated (chair_a.x) so need to be sliced
										// up to find which part of the end is the actual texture (up to 32 chars)
										char pSlicedVariant[512];
										for ( int n=32; n>5; n-- )
										{
											strcpy ( pSlicedVariant, pFileOnlyNoExt+strlen(pFileOnlyNoExt)-n );
											hExists = CreateFile( pSlicedVariant, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
											if ( hExists!=INVALID_HANDLE_VALUE )
											{
												// this texture file exists - we have our texture name
												strcpy ( pFileOnlyNoExt, pSlicedVariant);
												CloseHandle( hExists );
												iTry=99;
												n=0;
											}
										}
									}
								}
							}
						}

						// remove spaces from material record
						char pNoSpacesFile[512];
						strcpy ( pNoSpacesFile, pFileOnlyNoExt );
						for ( DWORD n=0; n<strlen(pNoSpacesFile); n++ )
							if ( pNoSpacesFile[n]==' ' ) pNoSpacesFile[n]='_';

						// write material(texture) for this collecion of faces(mesh)
						sprintf ( pDynLine, "usemtl %s\n", pNoSpacesFile );
						WriteFile( hfile, pDynLine, strlen(pDynLine), &byteswritten, NULL ); 

						// also write this into the MTL file
						sprintf ( pDynLine, "newmtl %s\n", pNoSpacesFile );
						WriteFile( hMTLfile, pDynLine, strlen(pDynLine), &byteswritten, NULL ); 
						sprintf ( pDynLine, "    Ns 20\n" );
						WriteFile( hMTLfile, pDynLine, strlen(pDynLine), &byteswritten, NULL ); 
						sprintf ( pDynLine, "    d 1\n" );
						WriteFile( hMTLfile, pDynLine, strlen(pDynLine), &byteswritten, NULL ); 
						sprintf ( pDynLine, "    illum 2\n" );
						WriteFile( hMTLfile, pDynLine, strlen(pDynLine), &byteswritten, NULL ); 
						sprintf ( pDynLine, "    Kd 1.0 1.0 1.0\n" );
						WriteFile( hMTLfile, pDynLine, strlen(pDynLine), &byteswritten, NULL ); 
						sprintf ( pDynLine, "    Ks 0 0 0\n" );
						WriteFile( hMTLfile, pDynLine, strlen(pDynLine), &byteswritten, NULL ); 
						sprintf ( pDynLine, "    Ka 0 0 0\n" );
						WriteFile( hMTLfile, pDynLine, strlen(pDynLine), &byteswritten, NULL ); 
						sprintf ( pDynLine, "    map_Kd %s\n\n", pNoSpacesFile );
						WriteFile( hMTLfile, pDynLine, strlen(pDynLine), &byteswritten, NULL ); 

						// for each face
						for ( DWORD dwV=0; dwV<pVertOnlyMesh->dwVertexCount; dwV+=3 )
						{
							int iA = dwV+dwStartOfVertexBatch+0;
							int iB = dwV+dwStartOfVertexBatch+1;
							int iC = dwV+dwStartOfVertexBatch+2;
							sprintf ( pDynLine, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", iA, iA, iA, iB, iB, iB, iC, iC, iC );
							WriteFile( hfile, pDynLine, strlen(pDynLine), &byteswritten, NULL );
						}
						pLine = "\n";
						WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 

						// Advance global start marker for vertice indices
						dwStartOfVertexBatch+=pVertOnlyMesh->dwVertexCount;

						// free temp mesh
						SAFE_DELETE ( pVertOnlyMesh );
					}

					// End of file marker
					pLine = "# End of file\n";
					WriteFile( hfile, pLine, strlen(pLine), &byteswritten, NULL ); 
					
					// finish file
					CloseHandle ( hfile );
				}

				// finish file
				CloseHandle ( hMTLfile );
			}
		}
		else
		{
			// ensure filename uses DBO extension
			if ( strnicmp ( pDBPFilename + strlen(pDBPFilename) - 4, ".dbo", 4 )!=NULL )
			{
				RunTimeError ( RUNTIMEERROR_B3DMUSTUSEDBOEXTENSION );
				return;
			}

			// save the object as DBO
			if ( !SaveDBO ( pDBPFilename, pObject ) )
				return;
		}
	}
}

DARKSDK_DLL void SetDeleteCallBack ( int iID, ON_OBJECT_DELETE_CALLBACK pfn, int userData )
{
	// mike - 050803 - delete object override

	// ensure the object is present
	if ( !ConfirmObject ( iID ) )
		return;

	if ( g_ObjectList [ iID ]->iDeleteCount == 0 )
	{
		g_ObjectList [ iID ]->iDeleteCount += 25;
		g_ObjectList [ iID ]->pDelete       = new sObject::sDelete [ g_ObjectList [ iID ]->iDeleteCount ];
	}
	
	if ( g_ObjectList [ iID ]->iDeleteID < g_ObjectList [ iID ]->iDeleteCount )
	{
		g_ObjectList [ iID ]->pDelete [ g_ObjectList [ iID ]->iDeleteID ].onDelete = pfn;
		g_ObjectList [ iID ]->pDelete [ g_ObjectList [ iID ]->iDeleteID ].userData = userData;	

		g_ObjectList [ iID ]->iDeleteID++;
	}

	//g_ObjectList [ iID ]->onDelete = pfn;
	//g_ObjectList [ iID ]->userData = userData;
}

DARKSDK_DLL void SetDisableTransform ( int iID, bool bTransform )
{
	// mike - 050803 - can stop DB Pro transforming an object

	// ensure the object is present
	if ( !ConfirmObject ( iID ) )
		return;

	g_ObjectList [ iID ]->bDisableTransform = bTransform;
}

DARKSDK_DLL void CreateMeshForObject ( int iID, DWORD dwFVF, DWORD dwVertexCount, DWORD dwIndexCount )
{
	// mike - 050803 - create a new mesh for an object

	// ensure the object is present
	if ( !ConfirmObject ( iID ) )
		return;

	sObject* pObject = g_ObjectList [ iID ];

	if ( pObject->pFrame )
		SAFE_DELETE ( pObject->pFrame );
	
	pObject->pFrame = new sFrame;

	if ( !pObject->pFrame )
	{
		RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
		return;
	}

	pObject->pFrame->pMesh = new sMesh;

	if ( !pObject->pFrame->pMesh )
	{
		RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
		return;
	}
	
	if ( !SetupMeshFVFData ( pObject->pFrame->pMesh, dwFVF, dwVertexCount, dwIndexCount ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
		return;
	}

	pObject->pFrame->pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pObject->pFrame->pMesh->iDrawVertexCount = pObject->pFrame->pMesh->dwVertexCount;
	pObject->pFrame->pMesh->iDrawPrimitives  = pObject->pFrame->pMesh->dwIndexCount / 3;

	SetNewObjectFinalProperties ( iID, 100 );

	// setup new object and introduce to buffers
	//SetNewObjectFinalProperties ( iID, (100.0f)/2 );

	// box collision for box shapes
	SetColToBoxes ( g_ObjectList [ iID ] );

	// give the object a default texture
	SetTexture ( iID, 0 );
}

DARKSDK_DLL void DeleteEx ( int iID )
{
	// mike - 101005 - excluded objects could not previously by delete
	if ( !CheckObjectExist ( iID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];
	bool bObjectusedUniqueNotSharedBuffers = pObject->bUsesItsOwnBuffers;

	// ensure delete calls ondelete code
	for ( int i = 0; i < g_ObjectList [ iID ]->iDeleteID; i++ )
	{
		if ( pObject->pDelete [ i ].onDelete )
		{
			pObject->pDelete [ i ].onDelete ( iID, pObject->pDelete [ i ].userData );
		}
	}

	// delete object
	DeleteObject ( iID );

	// leespeed - 140307 - if the object only used unique VBIB buffers, it did not share any VB IB
	// which means there will be nothing to add back in, so we can skip this step
	if ( bObjectusedUniqueNotSharedBuffers==false )
	{
		m_ObjectManager.AddFlaggedObjectsBackToBuffers ();
	}

	// update
	m_ObjectManager.UpdateTextures();
}

DARKSDK_DLL void Deletes ( int iFrom, int iTo )
{
	// some simple checks
	if ( iTo==0 || iFrom==0 ) return;
	if ( iTo<iFrom ) return;

	// delete multiple objects
	for ( int iID=iFrom; iID<=iTo; iID++ )
	{
		// ensure it exists first
		if ( !CheckObjectExist ( iID ) )
			continue;

		// get object ptr
		sObject* pObject = g_ObjectList [ iID ];
		if ( pObject )
		{
			// ensure delete calls ondelete code
			for ( int i = 0; i < g_ObjectList [ iID ]->iDeleteID; i++ )
			{
				if ( pObject->pDelete [ i ].onDelete )
				{
					pObject->pDelete [ i ].onDelete ( iID, pObject->pDelete [ i ].userData );
				}
			}

			// delete object
			DeleteObject ( iID );
		}
	}

	// upon buffer removal, some object where flagged for re-creation
	m_ObjectManager.AddFlaggedObjectsBackToBuffers ();

	// update texture list when introduce new object(s)
	m_ObjectManager.UpdateTextures();
}

DARKSDK_DLL void ClearObjectsOfTextureRef ( LPDIRECT3DTEXTURE9 pTextureRef )
{
	// go through all objects being managed and remove texture ref
	m_ObjectManager.RemoveTextureRefFromAllObjects ( pTextureRef );
}

DARKSDK_DLL void Set ( int iID, SDK_BOOL bWireframe, int iTransparency, SDK_BOOL bCull )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// new transparency mode
	SDK_BOOL bTransparency=TRUE;
	if ( iTransparency==0 ) bTransparency=FALSE;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		SetWireframe	( pObject->ppMeshList [ iMesh ], bWireframe==FALSE		);
		SetTransparency	( pObject->ppMeshList [ iMesh ], bTransparency==TRUE	);
		SetCull			( pObject->ppMeshList [ iMesh ], bCull==TRUE			);
	}

	// apply transparency as object overlay
	SetObjectTransparency ( pObject, iTransparency );
}

DARKSDK_DLL void Set ( int iID, SDK_BOOL bWireframe, int iTransparency, SDK_BOOL bCull, int iFilter )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// new transparency mode
	SDK_BOOL bTransparency=TRUE;
	if ( iTransparency==0 ) bTransparency=FALSE;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		//SetWireframe	( pObject->ppMeshList [ iMesh ], bWireframe==TRUE		);
		// mike - 011005 - state must be false
		SetWireframe	( pObject->ppMeshList [ iMesh ], bWireframe==FALSE		);
		SetTransparency	( pObject->ppMeshList [ iMesh ], bTransparency==TRUE	);
		SetCull			( pObject->ppMeshList [ iMesh ], bCull==TRUE			);
		SetFilter		( pObject->ppMeshList [ iMesh ], iFilter				);
	}

	// apply transparency as object overlay
	SetObjectTransparency ( pObject, iTransparency );
}

DARKSDK_DLL void Set ( int iID, SDK_BOOL bWireframe, int iTransparency, SDK_BOOL bCull, int iFilter, SDK_BOOL bLight )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// new transparency mode
	SDK_BOOL bTransparency=TRUE;
	if ( iTransparency==0 ) bTransparency=FALSE;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		SetWireframe	( pObject->ppMeshList [ iMesh ], bWireframe==FALSE		);
		SetTransparency	( pObject->ppMeshList [ iMesh ], bTransparency==TRUE	);
		SetCull			( pObject->ppMeshList [ iMesh ], bCull==TRUE			);
		SetFilter		( pObject->ppMeshList [ iMesh ], iFilter				);
		SetLight		( pObject->ppMeshList [ iMesh ], bLight==TRUE			);
	}

	// apply transparency as object overlay
	SetObjectTransparency ( pObject, iTransparency );
}

DARKSDK_DLL void Set ( int iID, SDK_BOOL bWireframe, int iTransparency, SDK_BOOL bCull, int iFilter, SDK_BOOL bLight, SDK_BOOL bFog )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// new transparency mode
	SDK_BOOL bTransparency=TRUE;
	if ( iTransparency==0 ) bTransparency=FALSE;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		SetWireframe	( pObject->ppMeshList [ iMesh ], bWireframe==FALSE		);
		SetTransparency	( pObject->ppMeshList [ iMesh ], bTransparency==TRUE	);
		SetCull			( pObject->ppMeshList [ iMesh ], bCull==TRUE			);
		SetFilter		( pObject->ppMeshList [ iMesh ], iFilter				);
		SetLight		( pObject->ppMeshList [ iMesh ], bLight==TRUE			);
		SetFog			( pObject->ppMeshList [ iMesh ], bFog==TRUE				);
	}

	// apply transparency as object overlay
	SetObjectTransparency ( pObject, iTransparency );
}

DARKSDK_DLL void Set ( int iID, SDK_BOOL bWireframe, int iTransparency, SDK_BOOL bCull, int iFilter, SDK_BOOL bLight, SDK_BOOL bFog, SDK_BOOL bAmbient )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// new transparency mode
	SDK_BOOL bTransparency=TRUE;
	if ( iTransparency==0 ) bTransparency=FALSE;
	if ( iTransparency==7 ) bTransparency=FALSE;  // U75 - 051209 - Deal with early-rendered objects correctly

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		SetWireframe	( pObject->ppMeshList [ iMesh ], bWireframe==FALSE		);
		SetTransparency	( pObject->ppMeshList [ iMesh ], bTransparency==TRUE	);
		SetCull			( pObject->ppMeshList [ iMesh ], bCull==TRUE			);
		SetFilter		( pObject->ppMeshList [ iMesh ], iFilter				);
		SetLight		( pObject->ppMeshList [ iMesh ], bLight==TRUE			);
		SetFog			( pObject->ppMeshList [ iMesh ], bFog==TRUE				);
		SetAmbient		( pObject->ppMeshList [ iMesh ], bAmbient==TRUE			);
	}

	// apply transparency as object overlay
	SetObjectTransparency ( pObject, iTransparency );
}

DARKSDK_DLL void SetWireframe ( int iID, SDK_BOOL bWireframe )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetWireframe ( pObject->ppMeshList [ iMesh ], bWireframe==TRUE );
}

DARKSDK_DLL void SetTransparency ( int iID, int iTransparency )
{
	// Transparency Modes
	// 0 - first-phase no alpha
	// 1 - first-phase with alpha masking
	// 2 and 3 - second-phase which overlaps solid geometry
	// 4 - alpha test (only render beyond 0x000000CF alpha values)
	// 5 - water line object (seperates depth sort automatically)
	// 6 - combination of 3 and 4 (second phase render with alpha blend AND alpha test, used for fading LOD leaves)
	// 7 - very early draw phase no alpha

	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// new transparency mode
	SDK_BOOL bTransparency=TRUE;
	if ( iTransparency==0 ) bTransparency=FALSE;
	if ( iTransparency==7 ) bTransparency=FALSE;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		SetTransparency ( pObject->ppMeshList [ iMesh ], bTransparency==TRUE );
		SetAlphaTest ( pObject->ppMeshList [ iMesh ], 0x0 ); 
		if ( iTransparency==4 || iTransparency==6 )
		{
			SetAlphaTest ( pObject->ppMeshList [ iMesh ], 0x000000CF );
		}
	}

	// apply transparency as object overlay
	SetObjectTransparency ( pObject, iTransparency );
}

DARKSDK_DLL void SetCull ( int iID, SDK_BOOL bCull )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// lee - 040306 - u6rc5 - solve CW/CCW issue with some model imports
	int iCullMode = bCull;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetCullCWCCW ( pObject->ppMeshList [ iMesh ], iCullMode );
}

DARKSDK_DLL void SetFilterStage ( int iID, int iStage, int iFilter )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetFilter ( pObject->ppMeshList [ iMesh ], iStage, iFilter );
}

DARKSDK_DLL void SetFilter ( int iID, int iFilter )
{
	// iFilter
	// D3DTEXF_POINT = 1 (use 0)
	// D3DTEXF_LINEAR = 2 (use 1)
	// D3DTEXF_ANISOTROPIC = 3 (use 2)
	// D3DTEXF_PYRAMIDALQUAD = 6 (use 5)
	// D3DTEXF_GAUSSIANQUAD = 7 (use 6)
	// When assigned, MeshCPP (iFilter++)
	SetFilterStage ( iID, 0, iFilter );
}

DARKSDK_DLL void SetLight ( int iID, SDK_BOOL bLight )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetLight ( pObject->ppMeshList [ iMesh ], bLight==TRUE );
}

DARKSDK_DLL void SetFog ( int iID, SDK_BOOL bFog )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetFog ( pObject->ppMeshList [ iMesh ], bFog==TRUE );
}

DARKSDK_DLL void SetAmbient ( int iID, SDK_BOOL bAmbient )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetAmbient ( pObject->ppMeshList [ iMesh ], bAmbient==TRUE );
}

DARKSDK_DLL void SetMask ( int iID, int iMASK )
{
	// check the object exists
	// leefix - 211006 - allow instanced objects to be masked
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	pObject->dwCameraMaskBits = (DWORD)iMASK; // u63 - 0-30 camera bits in mask
}

DARKSDK_DLL int RgbR ( DWORD iRGB )
{
	return (int)((iRGB & 0x00FF0000) >> 16);
}

DARKSDK_DLL int RgbG ( DWORD iRGB )
{
	return (int)((iRGB & 0x0000FF00) >> 8);
}

DARKSDK_DLL int RgbB ( DWORD iRGB )
{
	return (int)((iRGB & 0x000000FF) );
}

DARKSDK_DLL void Color ( int iID, DWORD dwRGB )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		SetDiffuse ( pObject->ppMeshList [ iMesh ], 1.0f );
		SetBaseColor ( pObject->ppMeshList [ iMesh ], dwRGB );
	}

	// u74b7 - Removed 'Delete old stencil effect (if any)'
	// m_ObjectManager.DeleteStencilEffect ( pObject );

	// trigger a re-new and re-sort
	m_ObjectManager.RenewReplacedMeshes ( pObject );
	m_ObjectManager.UpdateTextures ( );
}

DARKSDK_DLL void SetDiffuseMaterialEx ( int iID, DWORD dwRGB, int iMaterialOrVertexData )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// lee - 240206 - u60 - if object is instance, use custom-data-slot to store diffuse colour
	sObject* pActualObject = pObject->pInstanceOfObject;
	if ( pActualObject )
	{
		// only if have at least one mesh
		if ( pActualObject->iMeshCount>0 )
		{
			// mesh ptr
			sMesh* pMesh = pActualObject->ppMeshList [ 0 ];

			// this object is an instance
			SetObjectStatisticsInteger(iID,0,dwRGB);
			if ( pObject->dwCustomSize==0 )
			{
				// create custom slot
				DWORD dwStatisticsDataSize = 8;
				pObject->dwCustomSize = dwStatisticsDataSize*-1;
				pObject->pCustomData = (LPVOID)new DWORD[dwStatisticsDataSize];
				for ( DWORD i=0; i<dwStatisticsDataSize; i++ )
					*(((DWORD*)pObject->pCustomData)+i) = 0;
			}
			if ( pObject->dwCustomSize>4000000000 )
			{
				// set diffuse colour
				*(((DWORD*)pObject->pCustomData)+0) = dwRGB;
			}
		}
	}
	else
	{
		// apply setting to all meshes
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		{
			// lee - 240206 - u60 - do both to ensure diffuse is written (SetDiffuseEx added)
			if ( iMaterialOrVertexData==1 )
				SetDiffuseEx ( pObject->ppMeshList [ iMesh ], dwRGB );
			else
				SetDiffuseMaterial ( pObject->ppMeshList [ iMesh ], dwRGB );
		}
	}
}

DARKSDK_DLL void SetDiffuseMaterial ( int iID, DWORD dwRGB )
{
	// see above
	SetDiffuseMaterialEx ( iID, dwRGB, 0 );
}

DARKSDK_DLL void SetAmbienceMaterial ( int iID, DWORD dwRGB )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetAmbienceMaterial ( pObject->ppMeshList [ iMesh ], dwRGB );
}

DARKSDK_DLL void SetSpecularMaterial ( int iID, DWORD dwRGB )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetSpecularMaterial ( pObject->ppMeshList [ iMesh ], dwRGB );
}

DARKSDK_DLL void SetSpecularPower ( int iID, float fPower )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetSpecularPower ( pObject->ppMeshList [ iMesh ], fPower );
}

DARKSDK_DLL void SetSpecularMaterial ( int iID, DWORD dwRGB, float fPower )
{
	// U73 - 230309 - helper extra function parameter
	SetSpecularMaterial ( iID, dwRGB );
	SetSpecularPower ( iID, fPower );
}

DARKSDK_DLL void SetEmissiveMaterial ( int iID, DWORD dwRGB )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetEmissiveMaterial ( pObject->ppMeshList [ iMesh ], dwRGB );
}

DARKSDK_DLL void SetArbitaryValue ( int iID, DWORD dwArbValue )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		pObject->ppMeshList [ iMesh ]->Collision.dwArbitaryValue = dwArbValue;
}

DARKSDK_DLL void MakeObject ( int iID, int iMeshID, int iImageID )
{
	// check the mesh exists
	if ( !ConfirmMesh ( iMeshID ) )
		return;

	// attempt to create a new object
	if ( !CreateNewObject ( iID, "mesh" ) )
		return;

	// no transform of new limb
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity ( &matWorld );

	// setup general object data
	sMesh* pMesh = g_ObjectList [ iID ]->pFrame->pMesh;
	MakeMeshFromOtherMesh ( true, pMesh, g_RawMeshList [ iMeshID ], &matWorld );

	// setup new object and introduce to buffers
	SetNewObjectFinalProperties ( iID, -1.0f );

	// give the object a texture (optional)
	if ( iImageID==-1 )
	{
		pMesh->dwTextureCount = g_RawMeshList [ iMeshID ]->dwTextureCount; 
		pMesh->pTextures = new sTexture [ pMesh->dwTextureCount ]; 
		CloneInternalTextures ( pMesh, g_RawMeshList [ iMeshID ] );
	}
	else
		SetTexture ( iID, iImageID );
}

// Recursive Support Function (move to DBO when solid)
sFrame* MakeObjectFromLimbRec ( sFrame* pCurrentFrameToCopy, sFrame* pDstParentFrame )
{
	sFrame* pDstFrameRoot = NULL;
	sFrame* pDstFrame = NULL;
	while ( pCurrentFrameToCopy )
	{
		// frame hierarchy being created
		sFrame* pThisDstFrame = new sFrame;

		// first frame is root frame
		if ( pDstFrameRoot==NULL )
		{
			// first frame of list holds parent ref
			pDstFrameRoot = pThisDstFrame;
			pDstFrameRoot->pParent = pDstParentFrame;
		}

		// last dst frame is sybling to this new one
		if ( pDstFrame ) pDstFrame->pSibling = pThisDstFrame;

		// new dst current frame for copy
		pDstFrame = pThisDstFrame;

		// copy src frame data to dst frame data
		memcpy ( pDstFrame, pCurrentFrameToCopy, sizeof(sFrame) );
		pDstFrame->pChild = NULL;
		pDstFrame->pSibling = NULL;
		pDstFrame->pMesh = NULL;
		pDstFrame->pBoundBox = NULL;
		pDstFrame->pBoundSphere = NULL;
		pDstFrame->pShadowMesh = NULL;

		// go into children of this frame
		pDstFrame->pChild = MakeObjectFromLimbRec ( pCurrentFrameToCopy->pChild, pDstFrame );

		// next sybling
		pCurrentFrameToCopy = pCurrentFrameToCopy->pSibling;
	}

	// return created and filled frame
	return pDstFrameRoot;
}

DARKSDK_DLL void MakeObjectFromLimbEx ( int iNewID, int iSrcID, int iLimbID, int iCopyAllFromLimb )
{
	// check the mesh exists
	if ( !ConfirmObjectAndLimbInstance ( iSrcID, iLimbID ) )
		return;

	// attempt to create a new object
	if ( !CreateNewObject ( iNewID, "limbmesh" ) )
		return;

	// make object from mesh contained within object
	sObject* pNewObject = g_ObjectList [ iNewID ];
	sObject* pSrcObject = g_ObjectList [ iSrcID ];
	sObject* pActualSrcObject = pSrcObject;
	if ( pSrcObject->pInstanceOfObject ) pActualSrcObject = pSrcObject->pInstanceOfObject;
	sFrame* pSrcFrame = pActualSrcObject->ppFrameList [ iLimbID ];

	// leefix - 181105 - reset original before copy
	ResetVertexDataInMesh ( pSrcObject );

	// leefix - 181105 - can copy limbs even if no mesh there (copy hierarchy and
	// lee - 030406 - u6rc5 - added new command to make one-limb vs all-limb-hierarchy legacy U59 compat.
	if ( iCopyAllFromLimb==1 )
	{
		// Delete frame and mesh from new-object (start from scratch)
		SAFE_DELETE ( pNewObject->pFrame->pMesh );
		SAFE_DELETE ( pNewObject->pFrame );

		// trace through pFrame (limb) selected and copy all hierarchy from it
		sFrame* pDstFrameRoot = NULL;
		pDstFrameRoot = MakeObjectFromLimbRec ( pSrcFrame, NULL );

		// copy frame reference to new object
		pNewObject->pFrame = pDstFrameRoot;

		// force obj to make framelist
		CreateFrameAndMeshList ( pNewObject );

		// copy meshes of src object to dst object
		DWORD dwMeshCount = pActualSrcObject->iMeshCount;
		pNewObject->iMeshCount = dwMeshCount;
		if ( dwMeshCount>0 )
		{
			pNewObject->ppMeshList = new sMesh* [ dwMeshCount ];
			for ( int m=0; m<(int)dwMeshCount; m++ )
			{
				sMesh* pSrcMesh = pActualSrcObject->ppMeshList [ m ];
				sMesh* pDestMesh = new sMesh;

				// root matrix
				D3DXMATRIX matWorld;
				D3DXMatrixIdentity ( &matWorld );

				// mesh copy
				MakeMeshFromOtherMesh ( true, pDestMesh, pSrcMesh, &matWorld );
				pNewObject->ppMeshList [ m ] = pDestMesh;

				// bone data copy
				DWORD dwBoneCount = pSrcMesh->dwBoneCount;
				if ( dwBoneCount>0 )
				{
					DWORD dwNewBone = 0;
					pDestMesh->pBones = new sBone [ dwBoneCount ];
					for ( DWORD b=0; b<dwBoneCount; b++ )
					{
						bool bNeedThisBone = false;
						for ( DWORD l=0; l<(DWORD)pNewObject->iFrameCount; l++ )
							if ( strcmp ( pSrcMesh->pBones [ b ].szName, pNewObject->ppFrameList [ l ]->szName )==NULL )
								{ bNeedThisBone = true; break; }

						if ( bNeedThisBone==true )
						{
							memcpy ( &pDestMesh->pBones [ dwNewBone ], &pSrcMesh->pBones [ b ], sizeof ( sBone ) );
							DWORD dwNumInfluences = pDestMesh->pBones [ dwNewBone ].dwNumInfluences;
							pDestMesh->pBones [ dwNewBone ].pVertices = new DWORD [ dwNumInfluences ];
							pDestMesh->pBones [ dwNewBone ].pWeights = new float [ dwNumInfluences ];
							memcpy ( pDestMesh->pBones [ dwNewBone ].pVertices, pSrcMesh->pBones [ b ].pVertices, sizeof(DWORD)*dwNumInfluences );
							memcpy ( pDestMesh->pBones [ dwNewBone ].pWeights, pSrcMesh->pBones [ b ].pWeights, sizeof(float)*dwNumInfluences );
							dwNewBone=dwNewBone+1;
						}
					}
					pDestMesh->dwBoneCount = dwNewBone;
				}
			}
		}

		// only one main mesh exists for bone animation, so we assign it here
		if ( pNewObject->pFrame && pNewObject->ppMeshList )
		{
			// lee - 270306 - u6b5 - first look for mesh from original frame, if exist
			int iMeshBest = -1;
			sMesh* pMeshUsedByOldFrame = pSrcFrame->pMesh;
			for ( int iM=0; iM<(int)pSrcObject->iMeshCount; iM++ )
				if ( pSrcObject->ppMeshList [ iM ]==pMeshUsedByOldFrame )
					iMeshBest = iM;

			// lee - 270306 - u6b5 - pre-U6b5 code..
			if ( iMeshBest==-1 )
			{
				// make SURE the main bone mesh stolen is the BIGGEST
				DWORD dwVertCountLargest = 0;
				for ( int iM=0; iM<(int)dwMeshCount; iM++ )
				{
					if ( pNewObject->ppMeshList [iM ]->dwVertexCount > dwVertCountLargest )
					{
						dwVertCountLargest = pNewObject->ppMeshList [iM ]->dwVertexCount;
						iMeshBest = iM;
					}
				}
			}

			// assign chosen mesh to frame now
			pNewObject->pFrame->pMesh = pNewObject->ppMeshList [ iMeshBest ];
		}

		// copy all animation keyframe data for frames we have taken (if any)
		sAnimationSet* pOrigAnimSet = pSrcObject->pAnimationSet;
		if ( pOrigAnimSet )
		{
			sAnimation* pOrigAnim = pOrigAnimSet->pAnimation;
			if ( pOrigAnim )
			{
				pNewObject->pAnimationSet = new sAnimationSet;
				memcpy ( pNewObject->pAnimationSet, pOrigAnimSet, sizeof ( sAnimationSet ) );
				pNewObject->pAnimationSet->pAnimation = NULL;
				pNewObject->pAnimationSet->pvecBoundCenter = NULL;
				pNewObject->pAnimationSet->pfBoundRadius = NULL;
				pNewObject->pAnimationSet->pvecBoundMax = NULL;
				pNewObject->pAnimationSet->pvecBoundMin = NULL;
				sAnimation* pWorkAnim = NULL;
				while ( pOrigAnim != NULL )
				{
					// if animation name same as any frame in new object, copy anim from src obj to new obj
					bool bNeedThisAnim = false;
					for ( DWORD l=0; l<(DWORD)pNewObject->iFrameCount; l++ )
						if ( strcmp ( pOrigAnim->szName, pNewObject->ppFrameList [ l ]->szName )==NULL )
							{ bNeedThisAnim = true; break; }

					// add this one?
					if ( bNeedThisAnim==true )
					{
						// new animation 
						sAnimation* pCurrentAnim = new sAnimation;		

						// link new anim with previous item
						if ( pWorkAnim ) pWorkAnim->pNext = pCurrentAnim;

						// assign first animation 
						if ( pNewObject->pAnimationSet->pAnimation==NULL )
							pNewObject->pAnimationSet->pAnimation = pCurrentAnim;

						// work animation
						pWorkAnim = pCurrentAnim;

						// fill with name
						memcpy ( pWorkAnim->szName, pOrigAnim->szName, sizeof(pWorkAnim->szName) );

						// create animation and copy orig data
						pWorkAnim->dwNumMatrixKeys = pOrigAnim->dwNumMatrixKeys;
						pWorkAnim->pMatrixKeys = new sMatrixKey [ pWorkAnim->dwNumMatrixKeys ];
						memcpy ( pWorkAnim->pMatrixKeys, pOrigAnim->pMatrixKeys, sizeof ( sMatrixKey )*pWorkAnim->dwNumMatrixKeys );
					}

					// move to the next sequence
					pOrigAnim = pOrigAnim->pNext;
				}
			}
		}

		// setup new object and introduce to buffers
		pNewObject->iMeshCount *= -1; // negated within function CreateFrameAndMeshList
		pNewObject->iFrameCount *= -1; // negated within function CreateFrameAndMeshList
		SetNewObjectFinalProperties ( iNewID, -1.0f );

		// give the object a default texture
		SetTexture ( iNewID, 0 );

		// leeadd - 181105 - many meshes are possible
		for ( int m=0; m<(int)dwMeshCount; m++ )
			CloneInternalTextures ( pNewObject->ppMeshList [ m ], pActualSrcObject->ppMeshList [ m ] );
	}
	else
	{
		// src and destination
		sMesh* pSrcMesh = pActualSrcObject->ppMeshList [ 0 ];
		if ( pSrcFrame ) if ( pSrcFrame->pMesh ) pSrcMesh = pSrcFrame->pMesh;
		sFrame* pDestFrame = g_ObjectList [ iNewID ]->pFrame;
		sMesh* pDestMesh = pDestFrame->pMesh;

		// just copy one limb mesh from object
		if ( pSrcMesh==NULL )
		{
			// failed
			RunTimeError ( RUNTIMEERROR_LIMBNOTEXIST );
			return;
		}

		// work out the world transform to apply to the captured mesh
		CalculateObjectWorld ( pSrcObject, NULL );
		D3DXMATRIX matWorld = pSrcFrame->matCombined * pSrcObject->position.matObjectNoTran;

		// create new mesh from existing mesh
		MakeMeshFromOtherMesh ( true, pDestMesh, pSrcMesh, &matWorld );

		// setup new object and introduce to buffers
		SetNewObjectFinalProperties ( iNewID, -1.0f );

		// give the object a default texture
		SetTexture ( iNewID, 0 );

		// leeadd - 240604 - u54 - copy texture from original limb to new object
		if ( pDestMesh && pSrcMesh )
			CloneInternalTextures ( pDestMesh, pSrcMesh );
	}
}

DARKSDK_DLL void MakeObjectFromLimb ( int iNewID, int iSrcID, int iLimbID )
{
	// see above
	MakeObjectFromLimbEx ( iNewID, iSrcID, iLimbID, 0 );
}

void SetObjectLOD ( int iCurrentID, int iLODLevel, float fDistanceOfLOD )
{
	// if object exists
	if ( !ConfirmObjectInstance ( iCurrentID ) )
		return;

	// identify real object (if instance)
	sObject* pObject = g_ObjectList [ iCurrentID ];
	sObject* pRealObject = pObject;
	if ( pObject->pInstanceOfObject ) pRealObject = pObject->pInstanceOfObject;

	// initially scan object limb names for LOD_0 LOD_1 LOD_2
	int iObjLOD0LimbIndex = -1;
	int iObjLOD1LimbIndex = -1;
	int iObjLOD2LimbIndex = -1;
	PerformCheckListForLimbs(pRealObject->dwObjectNumber);
	for(int c=0; c<g_pGlob->checklistqty; c++)
	{
		if ( strcmp ( g_pGlob->checklist[c].string, "LOD_0" )==NULL ) iObjLOD0LimbIndex = c;
		if ( strcmp ( g_pGlob->checklist[c].string, "LOD_1" )==NULL ) iObjLOD1LimbIndex = c;
		if ( strcmp ( g_pGlob->checklist[c].string, "LOD_2" )==NULL ) iObjLOD2LimbIndex = c;
	}

	// ensue we fill in missing LOD indices based on availability
	if ( iObjLOD1LimbIndex==-1 )
	{
		iObjLOD1LimbIndex = iObjLOD2LimbIndex;
		if ( iObjLOD1LimbIndex==-1 ) iObjLOD1LimbIndex = iObjLOD0LimbIndex;
	}
	if ( iObjLOD2LimbIndex==-1 )
	{
		iObjLOD2LimbIndex = iObjLOD1LimbIndex;
		if ( iObjLOD2LimbIndex==-1 ) iObjLOD2LimbIndex = iObjLOD0LimbIndex;
	}

	// use this command to assign per-object LOD distances
	if ( iLODLevel >= 1 && iLODLevel <= 2 )
		pObject->fLODDistance [ iLODLevel-1 ] = fDistanceOfLOD;

	// find first mesh (as fallback of LOD_X missing)
	int iFirstFrameWithMesh = 0;
	for (; iFirstFrameWithMesh<pRealObject->iFrameCount; iFirstFrameWithMesh++ )
	{
		if ( pRealObject->ppFrameList[iFirstFrameWithMesh]->pMesh )
			break;
	}

	// special mode to disregard 'AddLODToObject' style mesh LOD, and use limb visibility style
	pObject->iUsingWhichLOD = -1000;
	pObject->bHadLODNeedCamDistance = true;
	pObject->iLOD0LimbIndex = iObjLOD0LimbIndex;
	pObject->iLOD1LimbIndex = iObjLOD1LimbIndex;
	pObject->iLOD2LimbIndex = iObjLOD2LimbIndex;
	if ( pObject->iLOD0LimbIndex==-1 ) pObject->iLOD0LimbIndex = iFirstFrameWithMesh;
	if ( pObject->iLOD1LimbIndex==-1 ) pObject->iLOD1LimbIndex = iFirstFrameWithMesh;
	if ( pObject->iLOD2LimbIndex==-1 ) pObject->iLOD2LimbIndex = iFirstFrameWithMesh;
}

void AddLODToObject ( int iCurrentID, int iLODModelID, int iLODLevel, float fDistanceOfLOD )
{
	// takes all meshes of lodmodel and adds them to a special lod alternative meshes of specified object
	if ( !ConfirmObject ( iCurrentID ) || !ConfirmObject ( iLODModelID ) )
		return;

	// make object from mesh contained within object
	sObject* pObject = g_ObjectList [ iCurrentID ];
	sObject* pLODObject = g_ObjectList [ iLODModelID ];

	// leeadd - 061208 - U71 - limit lod levels (allow 0,1 and since U71 use 2 for last QUAD/DECAL level)
	if ( iLODLevel<0 || iLODLevel>2 )
		return;

	// leefix - 250106 - so important when copying meshes!
	ResetVertexDataInMesh ( pLODObject );

	// flag as a LOD object
	pObject->iUsingWhichLOD = 0;
	pObject->bHadLODNeedCamDistance = true;
	if ( iLODLevel < 2 )
		pObject->fLODDistance [ iLODLevel ] = fDistanceOfLOD;
	else
		pObject->fLODDistanceQUAD = fDistanceOfLOD;

	// leeadd - 061208 - new alpha fade feature of LOD system
	pObject->iUsingOldLOD = -1;
	pObject->fLODTransition = 0.0f;

	// U74 - 120409 - if quad level used, default for object is at QUAD (furthest first)
	if ( iLODLevel==2 )
	{
		// start off as QUAD level, and adjust as required (copied by instance command too)
		pObject->iUsingOldLOD = 3;
		pObject->iUsingWhichLOD = 3;
	}

	// copy meshes of src object to dst object
	DWORD dwLODMeshCount = pLODObject->iMeshCount;
	if ( dwLODMeshCount>0 )
	{
		// go through all meshes in LOD model
		for ( int iLODFrame = 0; iLODFrame < pLODObject->iFrameCount; iLODFrame++ )
		{
			sFrame* pLODFrame = pLODObject->ppFrameList [ iLODFrame ];
			if ( pLODFrame )
			{
				if ( pLODFrame->pMesh )
				{
					// root frame used to scan obj heirarchy to make bone matrix assignments
					sFrame* pRootFrame = pObject->ppFrameList [ 0 ];

					// find this mesh in main object
					for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
					{
						// get frame ptr
						sFrame* pFrame = pObject->ppFrameList [ iFrame ];
						if ( pFrame )
						{
							// leeadd - U71 - where the mesh is going to go
							sMesh* pDestinationLODMesh = NULL;
							if ( iLODLevel < 2 )
								pDestinationLODMesh = pFrame->pLOD [ iLODLevel ];
							else
								pDestinationLODMesh = pFrame->pLODForQUAD;

							if ( pFrame->pMesh && pDestinationLODMesh==NULL )
							{
								// when found match (or proving a final QUAD model)
								if ( _stricmp ( pLODFrame->szName, pFrame->szName )==NULL || iLODLevel==2 )
								{
									// create mesh within main object, copying lod data across
									sMesh* pSrcMesh = pLODFrame->pMesh;
									sMesh* pDestMesh = new sMesh;

									// root matrix
									D3DXMATRIX matWorld;
									D3DXMatrixIdentity ( &matWorld );

									// mesh copy
									MakeMeshFromOtherMesh ( true, pDestMesh, pSrcMesh, &matWorld );
									if ( iLODLevel < 2 )
										pFrame->pLOD [ iLODLevel ] = pDestMesh;
									else
										pFrame->pLODForQUAD = pDestMesh;

									// ensure work from original mesh base
									CollectOriginalVertexData ( pDestMesh );

									// copy over settings from main mesh (transparency, etc) - cannot change after LOD in place!
									CopyMeshSettings ( pDestMesh, pSrcMesh );

									// bone data copy
									DWORD dwBoneCount = pSrcMesh->dwBoneCount;
									if ( dwBoneCount>0 )
									{
										pDestMesh->pBones = new sBone [ dwBoneCount ];
										for ( DWORD b=0; b<dwBoneCount; b++ )
										{
											DWORD dwNewBone = b;
											memcpy ( &pDestMesh->pBones [ dwNewBone ], &pSrcMesh->pBones [ b ], sizeof ( sBone ) );
											DWORD dwNumInfluences = pDestMesh->pBones [ dwNewBone ].dwNumInfluences;
											pDestMesh->pBones [ dwNewBone ].pVertices = new DWORD [ dwNumInfluences ];
											pDestMesh->pBones [ dwNewBone ].pWeights = new float [ dwNumInfluences ];
											memcpy ( pDestMesh->pBones [ dwNewBone ].pVertices, pSrcMesh->pBones [ b ].pVertices, sizeof(DWORD)*dwNumInfluences );
											memcpy ( pDestMesh->pBones [ dwNewBone ].pWeights, pSrcMesh->pBones [ b ].pWeights, sizeof(float)*dwNumInfluences );
										}
										pDestMesh->dwBoneCount = dwBoneCount;
									}

									// now map frames to bones
									InitOneMeshFramesToBones ( pDestMesh );
									MapOneMeshFramesToBones ( pDestMesh, pRootFrame );

									// share textures and shaders
									if ( iLODLevel<2 )
									{
										// copy texture from pFrame->pMesh (not srcmesh) as we want a LOD mesh only for geometry
										pDestMesh->dwTextureCount = pFrame->pMesh->dwTextureCount; 
										pDestMesh->pTextures = new sTexture [ pDestMesh->dwTextureCount ]; 
										CloneInternalTextures ( pDestMesh, pFrame->pMesh );

										// leefix - 041208 - U71 - copy over shader influence too
										CloneShaderEffects ( pDestMesh, pFrame->pMesh );

										// add mesh to buffers
										m_ObjectManager.AddObjectMeshToBuffers ( pFrame->pLOD [ iLODLevel ], false );
									}
									else
									{
										// copy over texture from pSrcMesh
										pDestMesh->dwTextureCount = pSrcMesh->dwTextureCount; 
										pDestMesh->pTextures = new sTexture [ pDestMesh->dwTextureCount ]; 
										CloneInternalTextures ( pDestMesh, pSrcMesh );
										CloneShaderEffects ( pDestMesh, pSrcMesh );
										m_ObjectManager.AddObjectMeshToBuffers ( pFrame->pLODForQUAD, false );
									}

									// 220513 - finished adding this LOD to parent object
									iFrame = pObject->iFrameCount;
								}
							}
						}
					}
				}
			}
		}
	}
}

DARKSDK_DLL void CloneShared ( int iDestinationID, int iSourceID, int iCloneSharedData )
{
	// iCloneSharedData modes:
	// 101 = use existing object in destination and JUST CLONE TEXTURES OVER

	// check if dest object not exists
	if ( iCloneSharedData!=101 )
	{
		// except for when dest already exists via mode 101
		if ( !ConfirmNewObject ( iDestinationID ) ) return;
		ID_ALLOCATION ( iDestinationID );
	}

	// check if src object exists
	if ( !ConfirmObjectInstance ( iSourceID ) )
		return;

	// first restore original vertex data in object 
	sObject* pObject = g_ObjectList [ iSourceID ];

	// mike - 011005 - see if the original source object is an instance
	if ( pObject->pInstanceOfObject )
		pObject = pObject->pInstanceOfObject;

	// clone the object
	if ( iCloneSharedData!=101 )
	{
		// create new object in destination
		ResetVertexDataInMesh ( pObject );
		if ( !CloneDBO ( &g_ObjectList [ iDestinationID ], pObject ) )
			return;

		// Special clone parameter shares an original object not deleted (good for lots of bone anim objects)
		sObject* pNewObject = g_ObjectList [ iDestinationID ];
		if ( iCloneSharedData!=0 )
		{
			// work on cloned object (cut out potential huge anim data)
			// Update dependency details
			// Do this whether or not there is animation data, to ensure that is predictable
			pObject->dwDependencyCount++;
			pNewObject->pObjectDependency = pObject;

			if ( pNewObject->pAnimationSet )
			{
				sAnimation* pOrigAnim = pObject->pAnimationSet->pAnimation;
				sAnimation* pAnim = pNewObject->pAnimationSet->pAnimation;
				if ( pAnim && pOrigAnim )
				{
					while ( pAnim != NULL )
					{
						// Erase Animation Data (matrix data usually huge)
						SAFE_DELETE(pAnim->pMatrixKeys);
						pAnim->dwNumMatrixKeys=0;

						// substitute with original objects anim data
						pAnim->pSharedReadAnim = pOrigAnim;

						// move to the next sequence
						pOrigAnim = pOrigAnim->pNext;
						pAnim = pAnim->pNext;
					}
				}
			}
		}

		// and put back the latest frame of the source object
		pObject->fAnimLastFrame=-1.0f;
		UpdateObjectAnimation ( pObject );

		// setup new object and introduce to buffers
		SetNewObjectFinalProperties ( iDestinationID, -1.0f );

		// lee - 160415 - also copy keyframe bounds (quicker than generating if use slow mode)
		if ( pObject->pAnimationSet )
		{
			if ( pObject->pAnimationSet->pvecBoundMin )
			{
				DWORD dwTotalFrames = pObject->pAnimationSet->ulLength;
				if ( dwTotalFrames==0 ) dwTotalFrames=1;
				SAFE_DELETE(pNewObject->pAnimationSet->pvecBoundMin);
				pNewObject->pAnimationSet->pvecBoundMin = new D3DXVECTOR3 [ dwTotalFrames ];
				SAFE_DELETE(pNewObject->pAnimationSet->pvecBoundMax);
				pNewObject->pAnimationSet->pvecBoundMax = new D3DXVECTOR3 [ dwTotalFrames ];
				SAFE_DELETE(pNewObject->pAnimationSet->pvecBoundCenter);
				pNewObject->pAnimationSet->pvecBoundCenter = new D3DXVECTOR3 [ dwTotalFrames ];
				SAFE_DELETE(pNewObject->pAnimationSet->pfBoundRadius);
				pNewObject->pAnimationSet->pfBoundRadius = new float [ dwTotalFrames ];
				for ( int iKeyframe=0; iKeyframe<(int)dwTotalFrames; iKeyframe+=1 )
				{
					pNewObject->pAnimationSet->pvecBoundMin [ iKeyframe ] = pObject->pAnimationSet->pvecBoundMin [ iKeyframe ];
					pNewObject->pAnimationSet->pvecBoundMax [ iKeyframe ] = pObject->pAnimationSet->pvecBoundMax [ iKeyframe ];
					pNewObject->pAnimationSet->pvecBoundCenter [ iKeyframe ] = pObject->pAnimationSet->pvecBoundCenter [ iKeyframe ];
					pNewObject->pAnimationSet->pfBoundRadius [ iKeyframe ] = pObject->pAnimationSet->pfBoundRadius [ iKeyframe ];
				}
			}
		}

		// lee - 180406 - u6rc10 - clone all collision data
		pNewObject->collision = g_ObjectList [ iSourceID ]->collision;
	}

	// transfer references within meshes that cannot be DBO cloned (post-setup)
	sObject* pNewObject = g_ObjectList [ iDestinationID ];
	DWORD dwMeshCount = pObject->iMeshCount;
	if ( pNewObject->iMeshCount<(int)dwMeshCount ) dwMeshCount = pNewObject->iMeshCount;
	for ( int iMesh = 0; iMesh < (int)dwMeshCount; iMesh++ )
	{
		sMesh* pMesh = NULL; if ( iMesh<pObject->iMeshCount ) pMesh = pObject->ppMeshList [ iMesh ];
		sMesh* pNewMesh = NULL; if ( iMesh<pNewObject->iMeshCount ) pNewMesh = pNewObject->ppMeshList [ iMesh ];
		if ( pMesh && pNewMesh )
		{
			CloneShaderEffects ( pNewMesh, pMesh );
		}
	}

	if ( iCloneSharedData!=101 )
	{
		// some shader settings take over vertex control (so reset to original data)
		ResetVertexDataInMesh ( pNewObject );

		// add object id to shortlist
		AddObjectToObjectListRef ( iDestinationID );
	}

	// clone textures for all meshes (clone references)
	if ( g_ObjectList [ iDestinationID ]->ppMeshList )
	{
		for ( int iMesh = 0; iMesh < g_ObjectList [ iDestinationID ]->iMeshCount; iMesh++ )
		{
			// get mesh ptr
			sMesh* pMesh = NULL; if ( iMesh<g_ObjectList [ iDestinationID ]->iMeshCount ) pMesh = g_ObjectList [ iDestinationID ]->ppMeshList [ iMesh ];
			sMesh* pOrigMesh = NULL; if ( iMesh<pObject->iMeshCount ) pOrigMesh = pObject->ppMeshList [ iMesh ];
			if ( pMesh && pOrigMesh )
			{
				// lee - 230206 - handle if multimaterial or regular mesh
				//pMesh->bUseMultiMaterial = pOrigMesh->bUseMultiMaterial;
				DWORD dwMultiMatCount = pOrigMesh->dwMultiMaterialCount;
				if ( pMesh->bUseMultiMaterial==true && pOrigMesh->bUseMultiMaterial==true )
				{
					// Currrent texture if any used
					sTexture* pTexture = NULL;

					// if multimaterial not exist (clone texture only)
					if ( pMesh->pMultiMaterial==NULL )
					{
						// create it now
						pMesh->pMultiMaterial = new sMultiMaterial [ dwMultiMatCount ];
						memset ( pMesh->pMultiMaterial, 0, sizeof(sMultiMaterial) * dwMultiMatCount );
					}

					// Define textures for multi material array
					pMesh->dwMultiMaterialCount = dwMultiMatCount;
					for ( DWORD m=0; m<dwMultiMatCount; m++ )
					{
						// get multimat at index
						sMultiMaterial* pMultiMat = &(pMesh->pMultiMaterial [ m ]);
						sMultiMaterial* pOrigMultiMat = &(pOrigMesh->pMultiMaterial [ m ]);

						// copy references over (clone)
						strcpy ( pMultiMat->pName, pOrigMultiMat->pName );
						pMultiMat->dwIndexCount		= pOrigMultiMat->dwIndexCount;
						pMultiMat->dwIndexStart		= pOrigMultiMat->dwIndexStart;
						pMultiMat->mMaterial		= pOrigMultiMat->mMaterial;
						pMultiMat->pTexturesRef		= pOrigMultiMat->pTexturesRef;
					}

					// multimaterial still uses texture)
					CloneInternalTextures ( pMesh, pOrigMesh );
				}
				else
				{
					// if original multimesh but dest regular, copy multi to reg (first material wins)
					if ( pMesh->bUseMultiMaterial==false && pOrigMesh->bUseMultiMaterial==true )
					{
						if ( dwMultiMatCount>=1 )
						{
							//SAFE_DELETE(pMesh->pTextures);//crashes, mem leak, fuind out whY!!
							if ( pMesh->dwTextureCount!=2 )
							{
								pMesh->dwTextureCount = 2;
								pMesh->pTextures = new sTexture [ pMesh->dwTextureCount ];
								memset ( pMesh->pTextures, 0, sizeof(sTexture)*pMesh->dwTextureCount );
							}
							pMesh->pTextures [ 0 ].iImageID  = pOrigMesh->pTextures [ 0 ].iImageID;;
							pMesh->pTextures [ 0 ].pTexturesRef = pOrigMesh->pMultiMaterial [ 0 ].pTexturesRef;
							strcpy ( pMesh->pTextures [ 0 ].pName, pOrigMesh->pMultiMaterial [ 0 ].pName );
							pMesh->pTextures [ 0 ].dwBlendMode = D3DTOP_MODULATE;
							pMesh->pTextures [ 0 ].dwBlendArg1 = D3DTA_TEXTURE;
							pMesh->pTextures [ 0 ].dwBlendArg2 = D3DTA_DIFFUSE;
							pMesh->bUseMultiMaterial = false;
							pMesh->bUsesMaterial = false;

							// also wipe out shader as it will screw up lightmapper
							SetSpecialEffect ( pMesh, NULL );
						}
					}
					else
					{
						// regular mesh clone texture
						CloneInternalTextures ( pMesh, pOrigMesh );
					}
				}
			}
		}
	}

	if ( iCloneSharedData!=101 )
	{
		// position data of cloned object must match source
		memcpy ( &g_ObjectList [ iDestinationID ]->position, &pObject->position, sizeof ( sPositionData ) );
	}
}

DARKSDK_DLL void Clone ( int iDestinationID, int iSourceID )
{
	CloneShared ( iDestinationID, iSourceID, 0 );
}

DARKSDK_DLL void Instance ( int iDestinationID, int iSourceID )
{
    int iActualSourceID = iSourceID;

	// check if dest object not exists
	if ( !ConfirmNewObject ( iDestinationID ) )
		return;

    // u74b7 - Allow instancing of an instance
    //         Actually instance the original instead.

	// check if src object exists, either as an original or an instance
	if ( !ConfirmObjectInstance ( iSourceID ) )
		return;

    // If src is an instance, use that instances original for mesh data
    // but copy the other details from the instance
    if (g_ObjectList [ iSourceID ]->pInstanceOfObject)
    {
        iActualSourceID = g_ObjectList [ iSourceID ]->pInstanceOfObject->dwObjectNumber;
    }

	// check memory allocation
	ID_ALLOCATION ( iDestinationID );

	// create pure instance of source object
	g_ObjectList [ iDestinationID ] = new sObject;
	g_ObjectList [ iDestinationID ]->pInstanceOfObject = g_ObjectList [ iActualSourceID ];
    g_ObjectList [ iDestinationID ]->pObjectDependency = g_ObjectList [ iActualSourceID ];
    g_ObjectList [ iActualSourceID ]->dwDependencyCount++;

	// U72 - 100109 - flag parent so it knows to animate even if not visible (irreversable)
	g_ObjectList [ iSourceID ]->position.bParentOfInstance = true;

	// lee - 250307 - store object number for reference
	g_ObjectList [ iDestinationID ]->dwObjectNumber = iDestinationID;

	// copy over basic info such as collision (so instances can have instant collision)
	g_ObjectList [ iDestinationID ]->collision = g_ObjectList [ iSourceID ]->collision;

	// lee - 310306 - u6rc5 - must carry LOD flag info across
	g_ObjectList [ iDestinationID ]->fLODDistance[0] = g_ObjectList [ iSourceID ]->fLODDistance[0];
	g_ObjectList [ iDestinationID ]->fLODDistance[1] = g_ObjectList [ iSourceID ]->fLODDistance[1];
	g_ObjectList [ iDestinationID ]->fLODDistanceQUAD = g_ObjectList [ iSourceID ]->fLODDistanceQUAD;
	g_ObjectList [ iDestinationID ]->bHadLODNeedCamDistance = g_ObjectList [ iSourceID ]->bHadLODNeedCamDistance;

	// U74 - 120409 - copy current state of parent LOD as well (so instancing when as a QUAD, recreate QUAD)
	g_ObjectList [ iDestinationID ]->iUsingOldLOD = g_ObjectList [ iSourceID ]->iUsingOldLOD;
	g_ObjectList [ iDestinationID ]->iUsingWhichLOD = g_ObjectList [ iSourceID ]->iUsingWhichLOD;
	g_ObjectList [ iDestinationID ]->fLODTransition = g_ObjectList [ iSourceID ]->fLODTransition;

	// create limb visibility array
	g_ObjectList [ iDestinationID ]->pInstanceMeshVisible = new bool [ g_ObjectList[iSourceID]->iFrameCount ];
	memset ( g_ObjectList [ iDestinationID ]->pInstanceMeshVisible, 255, g_ObjectList[iSourceID]->iFrameCount * sizeof(bool) );

	// mike - 021005 - retain pivot from source
	g_ObjectList [ iDestinationID ]->position.bApplyPivot = g_ObjectList [ iSourceID ]->position.bApplyPivot;
	g_ObjectList [ iDestinationID ]->position.matPivot = g_ObjectList [ iSourceID ]->position.matPivot;

	// add object id to shortlist
	AddObjectToObjectListRef ( iDestinationID );
}

//
// STAMP COMMANDS
//

DARKSDK_DLL void InitInstanceStamp ( int iDestinationID, int iX, int iY, int iZ, int iViewX, int iViewY, int iViewZ )
{
	// check to ensure destination object exists
	if ( !CheckObjectExist ( iDestinationID ) ) return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iDestinationID ];
	if ( pObject==NULL ) return;

	// create stamp memory
	sInstanceStamp* pStampPtr = new sInstanceStamp;
	memset ( pStampPtr, 0, sizeof(sInstanceStamp) );
	pObject->dwCustomSize = 4000000000+1;
	pObject->pCustomData = pStampPtr;

	// set camera view zone
	pStampPtr->dwViewXSize = iViewX;
	pStampPtr->dwViewYSize = iViewY;
	pStampPtr->dwViewZSize = iViewZ;

	// instead of empty space in the grid reference, we use viewsize as the step values (and aligns refgrid to HIGH spacing perfectly)
	iX /= iViewX;
	iY /= iViewY;
	iZ /= iViewZ;

	// increment by one for dimensional array multiplers (when need Y, also need to look at this one!!)
	//iX+=1;
	//iY+=1;
	//iZ+=1;

	// prepare stamp
	pStampPtr->dwXSize = iX;
	pStampPtr->dwYSize = iY;
	pStampPtr->dwZSize = iZ;
	pStampPtr->map = new sInstanceStampTileData*[iX*iY*iZ];
	memset ( pStampPtr->map, 0, iX*iY*iZ*sizeof(sInstanceStampTileData*) );
	for ( int lod=0; lod<INSTANCESTAMPLODMAX; lod++ )
	{
		pStampPtr->buffer[lod] = new sInstanceStampBuffer*[iX*iY*iZ];
		memset ( pStampPtr->buffer[lod], 0, iX*iY*iZ*sizeof(sInstanceStampBuffer*) );
	}
}

DARKSDK_DLL void FreeInstanceStamp ( int iID )
{
	// check to ensure iID object exists
	if ( !CheckObjectExist ( iID ) ) return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject==NULL ) return;

	// Free instance stamp memory
	if ( pObject->dwCustomSize>4000000000 )
	{
		sInstanceStamp* pStampPtr = (sInstanceStamp*)pObject->pCustomData;
		if ( pStampPtr->map )
		{
			// free items from grid map
			for ( DWORD dwIndexOffset=0; dwIndexOffset<(pStampPtr->dwXSize*pStampPtr->dwYSize*pStampPtr->dwZSize); dwIndexOffset++ )
			{
				sInstanceStampTileData* pItem = *(pStampPtr->map+dwIndexOffset);
				if ( pItem==NULL ) continue;
				while ( pItem )
				{
					sInstanceStampTileData* pToDelete = pItem;
					pItem = pItem->pNext;
					for ( int bb=0; bb<INSTANCESTAMPCAMERAMAX; bb++ )
					{
						if ( pToDelete->pImposter[bb] ) delete pToDelete->pImposter[bb];
					}
					delete pToDelete;
				}
			}
			delete pStampPtr->map;
			pStampPtr->map = NULL;
		}
		if ( pStampPtr->buffer )
		{
			// free items from grid map
			for ( DWORD dwIndexOffset=0; dwIndexOffset<(pStampPtr->dwXSize*pStampPtr->dwYSize*pStampPtr->dwZSize); dwIndexOffset++ )
			{
				for ( int lod=0; lod<INSTANCESTAMPLODMAX; lod++ )
				{
					sInstanceStampBuffer* pItem = *(pStampPtr->buffer[lod]+dwIndexOffset);
					if ( pItem==NULL ) continue;
					while ( pItem )
					{
						sInstanceStampBuffer* pToDelete = pItem;
						pItem = pItem->pNext;
						if ( pToDelete->pVBRef ) pToDelete->pVBRef->Release();
						if ( pToDelete->pIBRef ) pToDelete->pIBRef->Release();
						for ( int bb=0; bb<INSTANCESTAMPCAMERAMAX; bb++ )
						{
							if ( pToDelete->d3dQuery[bb] )
							{
								SAFE_RELEASE ( pToDelete->d3dQuery[bb] );
							}
							if ( pToDelete->d3dTex[bb] )
							{
								SAFE_RELEASE ( pToDelete->d3dTex[bb] );
							}
						}
						delete pToDelete;
					}
				}
			}
			// finally free buffer per-LOD item list buffers
			for ( int lod=0; lod<INSTANCESTAMPLODMAX; lod++ )
				SAFE_DELETE(pStampPtr->buffer[lod]);
		}
		if ( pObject->pCustomData )
		{
			delete pObject->pCustomData;
			pObject->pCustomData = NULL;
		}
		pObject->dwCustomSize = 0;
	}
}

DARKSDK_DLL void InstanceStamp ( int iDestinationID, int iSourceInstanceID, int iMode )
{
	// add ref data to grid
	if ( iMode==1 )
	{
		// check to ensure destination and parent object exists
		if ( !CheckObjectExist ( iDestinationID ) ) return;
		if ( !CheckObjectExist ( iSourceInstanceID ) ) return;

		// instance and parent object ptr
		sObject* pObject = g_ObjectList [ iDestinationID ];
		if ( pObject==NULL ) return;
		sObject* pInstanceObject = g_ObjectList [ iSourceInstanceID ];
		if ( pInstanceObject==NULL ) return;
		sObject* pParentObject = pInstanceObject->pInstanceOfObject;
		if ( pParentObject==NULL ) return;

		// extract essential data
		sInstanceStampTileData* stamptile = new sInstanceStampTileData;
		memset ( stamptile, 0, sizeof(sInstanceStampTileData) );
		stamptile->dwParentObjNumber = pParentObject->dwObjectNumber;
		stamptile->fPosX = pInstanceObject->position.vecPosition.x;
		stamptile->fPosY = pInstanceObject->position.vecPosition.y;
		stamptile->fPosZ = pInstanceObject->position.vecPosition.z;
		stamptile->fRotX = D3DXToRadian(pInstanceObject->position.vecRotate.x);
		stamptile->fRotY = D3DXToRadian(pInstanceObject->position.vecRotate.y);
		stamptile->fRotZ = D3DXToRadian(pInstanceObject->position.vecRotate.z);
		stamptile->fScaleX = pInstanceObject->position.vecScale.x;
		stamptile->fScaleY = pInstanceObject->position.vecScale.y;
		stamptile->fScaleZ = pInstanceObject->position.vecScale.z;
		stamptile->dwMeshVisibility = 0;
		for ( int n=0; n<pParentObject->iFrameCount; n++ )
		{
			bool bFlag = pInstanceObject->pInstanceMeshVisible[n];
			if ( bFlag )
			{
				stamptile->dwMeshVisibility = stamptile->dwMeshVisibility | (1<<n);
			}
		}

		// blank ptr until this item added to a LOD2 buffer
		stamptile->pLOD2BufferCollectionNext = NULL;

		// place data at specific reference point
		sInstanceStamp* pStampPtr = (sInstanceStamp*)pObject->pCustomData;
		sInstanceStampTileData** pStampMapPtr = pStampPtr->map;
		float fStepX = 100.0f * pStampPtr->dwViewXSize;
		float fStepY = 100.0f * pStampPtr->dwViewYSize;
		float fStepZ = 100.0f * pStampPtr->dwViewZSize;
		int iX = fabs(stamptile->fPosX/fStepX);
		int iY = 0;
		int iZ = fabs(stamptile->fPosZ/fStepZ);
		DWORD dwXX = 1;
		DWORD dwYY = pStampPtr->dwXSize;
		DWORD dwZZ = pStampPtr->dwXSize * pStampPtr->dwYSize;
		DWORD dwIndexOffset = (iX*dwXX)+(iY*dwYY)+(iZ*dwZZ);
		if ( dwIndexOffset<(pStampPtr->dwXSize*pStampPtr->dwYSize*pStampPtr->dwZSize) )
		{
			// locate the ref position storing link list ptr
			pStampMapPtr += dwIndexOffset;

			// add
			if ( *pStampMapPtr==NULL )
			{
				// new
				*pStampMapPtr = stamptile;
			}
			else
			{
				// add to list (can have multiple instanced seg/ent in grid ref)
				stamptile->pNext = *pStampMapPtr;
				*pStampMapPtr = stamptile;
			}
		}
	}
	if ( iMode==2 )
	{
		// add all QUADs to LOD2 layer
		if ( !CheckObjectExist ( iDestinationID ) ) return;

		// get stamp ptr
		sObject* pObject = g_ObjectList [ iDestinationID ];
		if ( pObject==NULL ) return;
		sInstanceStamp* pStampPtr = (sInstanceStamp*)pObject->pCustomData;
		sInstanceStampTileData** pStampMapPtr = pStampPtr->map;

		// apply instance stamp master object texture to quad mesh (stores sample QUAD texture)
		SetBaseTexture ( g_pQUADMesh, -1, pObject->ppMeshList[0]->pTextures[0].iImageID );
	
		// first job is to apply the quad shader to the global quad
		int iEffectID = iSourceInstanceID;
		if ( iEffectID!=0 )
			if ( !ConfirmEffect ( iEffectID ) )
				return;

		// remove any old shader
		SetSpecialEffect ( g_pQUADMesh, NULL );

		// reset vertex data before apply special effect
		ResetVertexDataInMeshPerMesh ( g_pQUADMesh );

		// remove effect if zero
		if ( iEffectID>0 )
		{
			// apply to specific mesh
			sEffectItem* pEffectItem = m_EffectList [ iEffectID ];
			pEffectItem->pEffectObj->m_bDoNotGenerateExtraData=1;
			SetSpecialEffect ( g_pQUADMesh, pEffectItem->pEffectObj );
			g_pQUADMesh->bVertexShaderEffectRefOnly = true;
			g_pQUADMesh->dwForceCPUAnimationMode = 0;
		}

		// go through all ref grid
		int lod = 2;
		DWORD dwXX = 1;
		DWORD dwYY = pStampPtr->dwXSize;
		DWORD dwZZ = pStampPtr->dwXSize * pStampPtr->dwYSize;
		for ( DWORD dwX=0; dwX<pStampPtr->dwXSize; dwX++ )
		{
			for ( DWORD dwY=0; dwY<pStampPtr->dwYSize; dwY++ )
			{
				for ( DWORD dwZ=0; dwZ<pStampPtr->dwZSize; dwZ++ )
				{
					DWORD dwIndexOffset = (dwX*dwXX)+(dwY*dwYY)+(dwZ*dwZZ);
					sInstanceStampTileData* pItem = *(pStampMapPtr + dwIndexOffset);
					while ( pItem!=NULL )
					{
						m_ObjectManager.AddInstanceStampObjectToBuffer(pObject,pItem,lod,dwX,dwY,dwZ);
						pItem = pItem->pNext;
					}
				}
			}
		}
	}
	if ( iMode==3 )
	{
		// add all QUADs to LOD2 layer
		if ( !CheckObjectExist ( iDestinationID ) ) return;

		// get stamp ptr
		sObject* pObject = g_ObjectList [ iDestinationID ];
		if ( pObject==NULL ) return;
		sInstanceStamp* pStampPtr = (sInstanceStamp*)pObject->pCustomData;

		// go through all LOD2 buffers and request their regeneration
		sInstanceStampBuffer* pActiveBufferList = pStampPtr->pActiveBufferList[2];
		while ( pActiveBufferList )
		{
			// trigger quad texture to regenerate
			pActiveBufferList->fQuadTextureLastAngleFromCamera[0] = pActiveBufferList->fAngleFromCamera[0] + 45.0f;
			if ( pActiveBufferList->fQuadTextureLastAngleFromCamera[0] >= 360.0f )
				pActiveBufferList->fQuadTextureLastAngleFromCamera[0] = pActiveBufferList->fQuadTextureLastAngleFromCamera[0] - 360.0f;
			pActiveBufferList = pActiveBufferList->pActiveListNext;
		}
	}
}

DARKSDK_DLL void MakeSphere ( int iID, float fRadius, int iRings, int iSegments )
{
	// attempt to create a new object
	if ( !CreateNewObject ( iID, "sphere" ) )
		return;

	// DBV1 size=diameter
	fRadius/=2;

	// setup general object data
	sMesh* pMesh = g_ObjectList [ iID ]->pFrame->pMesh;
	if ( MakeMeshSphere ( true, pMesh, D3DXVECTOR3(0,0,0), fRadius, iRings, iSegments, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DCOLOR_ARGB(255,255,255,255) )==false )
		return;

	// setup new object and introduce to buffers
	SetNewObjectFinalProperties ( iID, fRadius );

	// give the object a default texture
	SetTexture ( iID, 0 );
}

DARKSDK_DLL void MakeSphere ( int iID, float fRadius )
{
	MakeSphere ( iID, fRadius, 12, 12 );
}

DARKSDK_DLL void MakeCube ( int iID, float fSize )
{
	MakeBox ( iID, fSize, fSize, fSize );
}

DARKSDK_DLL GlobStruct* GetGlobalStructure ( void )
{
	return g_pGlob;
}

DARKSDK_DLL void GetPositionData ( int iID, sPositionData** ppPosition )
{
	if ( g_ObjectList [ iID ] )
	{
		CalculateObjectWorld ( g_ObjectList [ iID ], NULL );
		*ppPosition = &g_ObjectList [ iID ]->position;
	}
}

DARKSDK_DLL void GetEmitterData ( int iID, BYTE** ppVertices, DWORD* pdwVertexCount, int** ppiDrawCount )
{
	if ( g_ObjectList [ iID ] )
	{
		if ( g_ObjectList [ iID ]->pFrame->pMesh )
		{
			sMesh* pMesh = g_ObjectList [ iID ]->pFrame->pMesh;

			*ppVertices     = pMesh->pVertexData;
			*pdwVertexCount = pMesh->dwVertexCount;
			*ppiDrawCount   = &pMesh->iDrawPrimitives;
		}
	}
}

DARKSDK_DLL void UpdateEmitter ( int iID )
{
	if ( g_ObjectList [ iID ] )
	{
		if ( g_ObjectList [ iID ]->pFrame->pMesh )
		{
			g_ObjectList [ iID ]->pFrame->pMesh->bVBRefreshRequired = true;
			g_vRefreshMeshList.push_back ( g_ObjectList [ iID ]->pFrame->pMesh );
		}
	}
}

DARKSDK_DLL void MakeEmitter ( int iID, int iSize )
{
	MakePlane ( iID, iSize, iSize );

	/*
	// attempt to create a new object
	if ( !CreateNewObject ( iID, "emitter" ) )
	{
		// failed
		RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
		return;
	}

	int iObjectSize = 2;

	// create box mesh for object
	sMesh* pMesh = g_ObjectList [ iID ]->pFrame->pMesh;

	//if ( !SetupMeshFVFData ( pMesh, D3DFVF_XYZ | D3DFVF_DIFFUSE, iSize, 0 ) )
	if ( !SetupMeshFVFData ( pMesh, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_PSIZE, iSize, 0 ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
		return;
	}

	for ( int iParticle = 0; iParticle < iSize; iParticle++ )
	{
		SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData, iParticle, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 0.0f, 0.0f );
	}
	
	// setup mesh drawing properties
	pMesh->iPrimitiveType   = D3DPT_POINTLIST;
	pMesh->iDrawVertexCount = iSize;
	pMesh->iDrawPrimitives  = iSize;
	
	// setup new object and introduce to buffers
	SetNewObjectFinalProperties ( iID, (float)iObjectSize / 2.0f );

	// box collision for box shapes
	SetColToBoxes ( g_ObjectList [ iID ] );

	// give the object a default texture
	SetTexture ( iID, 0 );

	// special settings
	SetCull ( iID, false );
	*/
}

// New construction commands for multiplayer nameplates
DARKSDK int MakeNewObjectPanel	( int iID , int iNumberOfCharacters )
{
	// attempt to create a new object
	if ( !CreateNewObject ( iID, "nameplate" ) )
		return 0;

	sMesh* pMesh = g_ObjectList [ iID ]->pFrame->pMesh;

	// create memory
	DWORD dwVertexCount = 6 * iNumberOfCharacters; // store number of vertices
	DWORD dwIndexCount  = 0; // store number of indices
	if ( !SetupMeshFVFData ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1, dwVertexCount, dwIndexCount ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
		return 0;
	}

	// setup mesh drawing properties
	pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pMesh->iDrawVertexCount = pMesh->dwVertexCount;
	pMesh->iDrawPrimitives  = iNumberOfCharacters * 2;

	return 1;
}

DARKSDK void SetObjectPanelQuad	( int iID, int index, float fX, float fY, float fWidth, float fHeight, float fU1, float fV1, float fU2, float fV2, int r , int g , int b )
{

	index *= 6;

	// DB compatability
	fWidth  /= 2.0f;
	fHeight /= 2.0f;
	fX /= 2.0f;
	fY /= 2.0f;

	float pos_pos_x = fX + fWidth;
	float neg_pos_x = fX;
	float pos_pos_y = fY + fHeight;
	float neg_pos_y = fY;

	sMesh* pMesh = g_ObjectList [ iID ]->pFrame->pMesh;

	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  index + 0,  neg_pos_x,  pos_pos_y, 0.0f,  0.0f,  0.0f,  -1.0f, D3DCOLOR_ARGB ( 255, r, g, b ), fU1, fV1 );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  index + 1,   pos_pos_x,  pos_pos_y, 0.0f,  0.0f,  0.0f,  -1.0f, D3DCOLOR_ARGB ( 255, r, g, b ), fU2, fV1 );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  index + 2,  pos_pos_x, neg_pos_y, 0.0f,  0.0f,  0.0f,  -1.0f, D3DCOLOR_ARGB ( 255, r, g, b ), fU2, fV2 );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  index + 3,   pos_pos_x,  neg_pos_y, 0.0f,  0.0f,  0.0f, -1.0f, D3DCOLOR_ARGB ( 255, r, g, b ), fU2, fV2 );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  index + 4,   neg_pos_x, neg_pos_y, 0.0f,  0.0f,  0.0f,  -1.0f, D3DCOLOR_ARGB ( 255, r, g, b ), fU1, fV2 );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  index + 5,   neg_pos_x, pos_pos_y, 0.0f,  0.0f,  0.0f,  -1.0f, D3DCOLOR_ARGB ( 255, r, g, b ), fU1, fV1 );

}

DARKSDK void FinishObjectPanel	( int iID, float fWidth, float fHeight )
{
	// DB compatability
	fWidth  /= 2.0f;
	fHeight /= 2.0f;

	// setup new object and introduce to buffers
	SetNewObjectFinalProperties ( iID, (fWidth+fHeight)/2 );

	// give the object a default texture
	SetTexture ( iID, 0 );

}



DARKSDK_DLL void MakePlane ( int iID, float fWidth, float fHeight )
{
	// attempt to create a new object
	if ( !CreateNewObject ( iID, "plane" ) )
		return;

	// DB compatability
	fWidth  /= 2.0f;
	fHeight /= 2.0f;

	// create box mesh for object
	sMesh* pMesh = g_ObjectList [ iID ]->pFrame->pMesh;
	MakeMeshPlain ( true, pMesh, fWidth, fHeight, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DCOLOR_ARGB (255,255,255,255) );

	// setup new object and introduce to buffers
	SetNewObjectFinalProperties ( iID, (fWidth+fHeight)/2 );

	// set object Y=180 for compatibility with correct plain object
	YRotate ( iID, 180 );

	// box collision for box shapes
	SetColToBoxes ( g_ObjectList [ iID ] );

	// give the object a default texture
	SetTexture ( iID, 0 );

	// special settings
	SetCull ( iID, false );
}

DARKSDK_DLL void MakePlane ( int iID, float fWidth, float fHeight, int iFlag )
{
    if (iFlag == 0)
    {
        MakePlane ( iID, fWidth, fHeight );
        return;
    }

	// mike - 021005 - alternative make plane function

	// attempt to create a new object
	if ( !CreateNewObject ( iID, "plane" ) )
		return;

	// DB compatability
	fWidth  /= 2.0f;
	fHeight /= 2.0f;

	// create box mesh for object
	sMesh* pMesh = g_ObjectList [ iID ]->pFrame->pMesh;
	MakeMeshPlainEx ( true, pMesh, fWidth, fHeight, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DCOLOR_ARGB (255,255,255,255) );

	// setup new object and introduce to buffers
	SetNewObjectFinalProperties ( iID, (fWidth+fHeight)/2 );

	// set object Y=180 for compatibility with correct plain object
	//YRotate ( iID, 180 );

	// box collision for box shapes
	SetColToBoxes ( g_ObjectList [ iID ] );

	// give the object a default texture
	SetTexture ( iID, 0 );

	// special settings
	SetCull ( iID, false );
}

DARKSDK_DLL void MakeTriangle ( int iID, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3 )
{
	// attempt to create a new object
	if ( !CreateNewObject ( iID, "triangle" ) )
		return;

	// setup general object data
	sMesh* pMesh         = g_ObjectList [ iID ]->pFrame->pMesh;	// get a pointer to the mesh ( easier to access now )

	// create vertex memory
	DWORD dwVertexCount = 3;									// store number of vertices
	DWORD dwIndexCount  = 0;									// store number of indices
	if ( !SetupMeshFVFData ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, dwVertexCount, dwIndexCount ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
		return;
	}
	
	// create vertices for plane
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  0,  x1, y1, z1,  0.0f,  0.0f, -1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 0.0f, 0.0f );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  1,  x2, y2, z2,  0.0f,  0.0f, -1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 1.0f, 0.0f );
	SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData,  2,  x3, y3, z3,  0.0f,  0.0f, -1.0f, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 1.0f, 1.0f );

	// setup mesh drawing properties
	pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pMesh->iDrawVertexCount = pMesh->dwVertexCount;
	pMesh->iDrawPrimitives  = 1;

	// setup new object and introduce to buffers
	SetNewObjectFinalProperties ( iID, x2-x1 );

	// box collision for box shapes
	SetColToBoxes ( g_ObjectList [ iID ] );

	// give the object a default texture
	SetTexture ( iID, 0 );

	// special settings
	SetCull ( iID, false );
}

DARKSDK_DLL void MakeBox ( int iID, float fWidth, float fHeight, float fDepth )
{
	// attempt to create a new object
	if ( !CreateNewObject ( iID, "box" ) )
		return;

	// first off divide the size by 2 to keep compatibility with DB
	fWidth  /= 2.0f;
	fHeight /= 2.0f;
	fDepth  /= 2.0f;

	// create box mesh for object
	sMesh* pMesh = g_ObjectList [ iID ]->pFrame->pMesh;
	MakeMeshBox ( true, pMesh, -fWidth, -fHeight, -fDepth, fWidth, fHeight, fDepth, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DCOLOR_ARGB (255,255,255,255) );

	// setup new object and introduce to buffers
	SetNewObjectFinalProperties ( iID, (fWidth+fHeight)/2 );

	// box collision for box shapes
	SetColToBoxes ( g_ObjectList [ iID ] );

	// give the object a default texture
	SetTexture ( iID, 0 );
}

DARKSDK_DLL void MakePyramid ( int iID, float fSize )
{
	// attempt to create a new object
	if ( !CreateNewObject ( iID, "pyramid" ) )
		return;

	// first off divide the size by 2
	fSize  /= 2.0f;

	// create mesh for object
	sMesh* pMesh = g_ObjectList [ iID ]->pFrame->pMesh;
// lee - 150306 - u60b3 - added DIFFUSE so that latest DX can handle the undeclared format of this model (aniso)
//	MakeMeshPyramid ( true, pMesh, fSize, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DCOLOR_ARGB (255,255,255,255) );
	MakeMeshPyramid ( true, pMesh, fSize, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DCOLOR_ARGB (255,255,255,255) );

	// setup new object and introduce to buffers
	SetNewObjectFinalProperties ( iID, fSize );

	// box collision for box shapes
	SetColToBoxes ( g_ObjectList [ iID ] );

	// give the object a default texture
	SetTexture ( iID, 0 );
}

DARKSDK_DLL void MakeCylinder ( int iID, float fSize )
{
	// attempt to create a new object
	if ( !CreateNewObject ( iID, "cylinder" ) )
		return;

	float fHeight   = fSize;
	float fRadius   = fSize / 2;
	int   iSegments = 30;

	// setup general object data
	sMesh* pMesh         = g_ObjectList [ iID ]->pFrame->pMesh;		// get a pointer to the mesh ( easier to access now )

	// create vrtex memory
	DWORD dwVertexCount = ( iSegments + 1 ) * 2;					// store number of vertices
	DWORD dwIndexCount  = 0;										// store number of indices
	if ( !SetupMeshFVFData ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, dwVertexCount, dwIndexCount ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
		return;
	}

	float fDeltaSegAngle = ( 2.0f * D3DX_PI / iSegments );
	float fSegmentLength = 1.0f / ( float ) iSegments;
	int	  iVertex        = 0;

	// create the sides triangle strip
	for ( int iCurrentSegment = 0; iCurrentSegment <= iSegments; iCurrentSegment++ )
	{
		float x0 = fRadius * sinf ( iCurrentSegment * fDeltaSegAngle );
		float z0 = fRadius * cosf ( iCurrentSegment * fDeltaSegAngle );

		// Calculate normal
		D3DXVECTOR3 Normal = D3DXVECTOR3 ( x0, 0.0f, z0 );
		D3DXVec3Normalize ( &Normal, &Normal ); 

		// set vertex A
		SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  iVertex, x0, 0.0f+(fHeight/2.0f), z0, Normal.x, Normal.y, Normal.z, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 1.0f - ( fSegmentLength * ( float ) iCurrentSegment ), 0.0f );

		// increment vertex index
		iVertex++;

		// set vertex B
		SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  iVertex, x0, 0.0f-(fHeight/2.0f), z0, Normal.x, Normal.y, Normal.z, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 1.0f - ( fSegmentLength * ( float ) iCurrentSegment ), 1.0f );

		// increment vertex index
		iVertex++;
	}

	// setup mesh drawing properties
	pMesh->iPrimitiveType   = D3DPT_TRIANGLESTRIP;
	pMesh->iDrawVertexCount = pMesh->dwVertexCount;
	pMesh->iDrawPrimitives  = pMesh->dwVertexCount - 2;

	// setup new object and introduce to buffers
	SetNewObjectFinalProperties ( iID, fSize );

	// box collision for box shapes
	SetColToBoxes ( g_ObjectList [ iID ] );

	// give the object a default texture
	SetTexture ( iID, 0 );
}

DARKSDK_DLL void MakeCone ( int iID, float fSize )
{
	// make a cone

	// attempt to create a new object
	if ( !CreateNewObject ( iID, "cone" ) )
		return;

	float fHeight   = fSize;
	int   iSegments = 11;

	// correct cone size
	fSize/=2.0f;
	
	// setup general object data
	sMesh* pMesh         = g_ObjectList [ iID ]->pFrame->pMesh;		// get a pointer to the mesh ( easier to access now )

	// create vrtex memory
	DWORD dwVertexCount = (iSegments * 2) + 1;						// store number of vertices
	DWORD dwIndexCount  = iSegments * 3;							// store number of indices
	if ( !SetupMeshFVFData ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, dwVertexCount, dwIndexCount ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
		return;
	}

	float fDeltaSegAngle = ( (2.0f * D3DX_PI) / iSegments );
	float fSegmentLength = 1.0f / ( float ) iSegments;
	float fy0            = ( 90.0f - ( float ) D3DXToDegree ( atan ( fHeight / fSize ) ) ) / 90.0f;
	int	  iVertex        = 0;
	int	  iIndex         = 0;
	WORD  wVertexIndex   = 0;

	// for each segment, add a triangle to the sides triangle list
	for ( int iCurrentSegment = 0; iCurrentSegment <= iSegments; iCurrentSegment++ )
	{
		float x0 = fSize * sinf ( iCurrentSegment * fDeltaSegAngle );
		float z0 = fSize * cosf ( iCurrentSegment * fDeltaSegAngle );

		// Calculate normal
		D3DXVECTOR3 Normal = D3DXVECTOR3 ( x0, fy0, z0 );
		D3DXVec3Normalize ( &Normal, &Normal ); 

		// not the last segment though
		if ( iCurrentSegment < iSegments )
		{
			// set vertex A
			SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  iVertex, 0.0f, 0.0f+(fHeight/2.0f), 0.0f, Normal.x, Normal.y, Normal.z, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 1.0f - ( fSegmentLength * ( float ) iCurrentSegment ), 0.0f );

			// increment vertex index
			iVertex++;

			// set vertex B
			SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  iVertex, x0, 0.0f-(fHeight/2.0f), z0, Normal.x, Normal.y, Normal.z, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 1.0f - ( fSegmentLength * ( float ) iCurrentSegment ), 1.0f );

			// increment vertex index
			iVertex++;
		}
		else
		{
			// set last vertex
			SetupStandardVertex ( pMesh->dwFVF,	pMesh->pVertexData,  iVertex, x0, 0.0f - ( fHeight / 2.0f ), z0, Normal.x, Normal.y, Normal.z, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), 0.0f, 1.0f );

			// increment vertex index
			iVertex++;
		}

		// not the last segment though
		if ( iCurrentSegment < iSegments )
		{
			// set three indices per segment
			pMesh->pIndices [ iIndex ] = wVertexIndex;
			iIndex++;
			wVertexIndex++;
			
			pMesh->pIndices [ iIndex ] = wVertexIndex;
			iIndex++;

			if ( iCurrentSegment == iSegments-1 )
				wVertexIndex += 1;
			else
				wVertexIndex += 2;
			
			pMesh->pIndices [ iIndex ] = wVertexIndex;
			iIndex++;
			wVertexIndex--;	
		}
	}

	// setup mesh drawing properties
	pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pMesh->iDrawVertexCount = ( iSegments * 2 ) + 1;
	pMesh->iDrawPrimitives  = iSegments;
	
	// setup new object and introduce to buffers
	SetNewObjectFinalProperties ( iID, fSize );

	// give the object a default texture
	SetTexture ( iID, 0 );
}

// Animation Commands

DARKSDK_DLL void Append ( SDK_LPSTR pString, int iID, int iFrame )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject->pAnimationSet==NULL )
	{
		RunTimeError(RUNTIMEERROR_B3DKEYFRAMENOTEXIST);
		return;
	}

	// must be end of object
	int iEndFrame = pObject->pAnimationSet->ulLength;
	if ( iFrame < iEndFrame )
	{
		RunTimeError(RUNTIMEERROR_B3DOBJECTAPPENDTOOLOW);
		return;
	}
	if ( iFrame > iEndFrame+1 )
	{
		RunTimeError(RUNTIMEERROR_B3DKEYFRAMENOTEXIST);
		return;
	}

	// Append animation from file to model
	if ( !AppendAnimationFromFile ( pObject, (LPSTR)pString, iFrame ) )
	{
		RunTimeError(RUNTIMEERROR_B3DOBJECTAPPENDFAILED);
		return;
	}
}

DARKSDK_DLL void Play ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// control animation
	sObject* pObject = g_ObjectList [ iID ];
	pObject->bAnimPlaying = true;
	pObject->bAnimLooping = false;

	// leefix - 240604 - u54 - added to align functionality to rest of system (sounds)
	pObject->fAnimFrame = 0.0f;
}

DARKSDK_DLL void Play ( int iID, int iStart )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// control animation
	sObject* pObject = g_ObjectList [ iID ];
	pObject->bAnimPlaying = true;
	pObject->bAnimLooping = false;
	pObject->fAnimFrame = (float)iStart;

	// leefix - 240604 - u54 - try to play frame after end marker, set new marker
	if ( pObject->fAnimFrame > pObject->fAnimFrameEnd )
		pObject->fAnimFrameEnd=pObject->fAnimTotalFrames;
}

DARKSDK_DLL void Play ( int iID, int iStart, int iEnd )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// control animation
	sObject* pObject = g_ObjectList [ iID ];
	pObject->bAnimPlaying = true;
	pObject->bAnimLooping = false;
	pObject->fAnimFrame = (float)iStart;
	pObject->fAnimFrameEnd = (float)iEnd;
}

DARKSDK_DLL void Loop ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// control animation
	sObject* pObject = g_ObjectList [ iID ];
	pObject->bAnimPlaying = true;
	pObject->bAnimLooping = true;

	// leefix - 240604 - u54 - added to align functionality to rest of system (sounds)
	pObject->fAnimFrame = 0.0f;
}

DARKSDK_DLL void Loop ( int iID, int iStart )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// control animation
	sObject* pObject = g_ObjectList [ iID ];
	pObject->bAnimPlaying = true;
	pObject->bAnimLooping = true;
	pObject->fAnimLoopStart = (float)iStart;

	// loop brings frame to first frame if outside
	if ( pObject->fAnimFrame < pObject->fAnimLoopStart )
		pObject->fAnimFrame = pObject->fAnimLoopStart;
}

DARKSDK_DLL void Loop ( int iID, int iStart, int iEnd )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// control animation
	sObject* pObject = g_ObjectList [ iID ];
	pObject->bAnimPlaying = true;
	pObject->bAnimLooping = true;
	pObject->fAnimLoopStart = (float)iStart;
	pObject->fAnimFrameEnd = (float)iEnd;

	// loop brings frame to first frame if outside
	if ( pObject->fAnimFrame < pObject->fAnimLoopStart )
		pObject->fAnimFrame = pObject->fAnimLoopStart;

	// leeadd - 300605 - support looping frames backwards with speed minus value
	if ( pObject->fAnimFrame > pObject->fAnimFrameEnd )
		pObject->fAnimFrame = pObject->fAnimLoopStart;
}

DARKSDK_DLL void Stop ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// control animation
	sObject* pObject = g_ObjectList [ iID ];
	pObject->bAnimPlaying = false;

	// mike - 021005 - make sure anim looping is also updated
	pObject->bAnimLooping = false;
}

DARKSDK_DLL void SetSpeed ( int iID, int iSpeed )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// control animation
	sObject* pObject = g_ObjectList [ iID ];
	pObject->fAnimSpeed = (float)iSpeed/100.0f;
}

DARKSDK_DLL void SetFrameEx ( int iID, float fFrame, int iRecalculateBounds )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// control animation
	if ( pObject->fAnimInterp==1.0f || pObject->bAnimPlaying )
	{
		// direct frame set if animating or not manually slerping
		pObject->fAnimFrame = fFrame;
	}
	else
	{
		// use manual slerp for frame interpolation
		pObject->bAnimManualSlerp = true;
		pObject->fAnimSlerpStartFrame = pObject->fAnimFrame;
		pObject->fAnimSlerpEndFrame = fFrame;
		pObject->fAnimSlerpTime = 0.0f;
	}

	// ensure object is updated
	pObject->bAnimUpdateOnce = true;

	// leeadd - 211008 - u71 - whether frame zero shifts overall object bounds
	DWORD dwDoesNotShiftFrameZero = 0;

	// leeadd - 070305 - if recalc, perform update and recalc bounds
	if ( iRecalculateBounds==1 )
	{
		// triggers bounds to be recalculated
		pObject->bUpdateOnlyCurrentFrameBounds = true;
		pObject->bUpdateOverallBounds = true;
	}

	// leefix - 260806 - u63 - replaced else which forced a bounds recalc from the SET FRAME obj,frm command..
	if ( iRecalculateBounds==2 )
	{
		// can reverse the setting
		if ( pObject->bUpdateOnlyCurrentFrameBounds )
		{
			pObject->bUpdateOnlyCurrentFrameBounds = false;
			pObject->bUpdateOverallBounds = true;
		}
	}

	// leeadd - 211008 - u71 - recalc, but flag frame zero so it does not shift overall object bounds
	if ( iRecalculateBounds==3 )
	{
		// triggers bounds to be recalculated
		pObject->bUpdateOnlyCurrentFrameBounds = false;
		pObject->bUpdateOverallBounds = true;
		dwDoesNotShiftFrameZero = 1;
	}

	// flag frame to NOT shift object bounds
	if ( pObject->ppFrameList )
	{
		if ( pObject->ppFrameList [ 0 ] )
		{
			if ( dwDoesNotShiftFrameZero==1 )
				pObject->ppFrameList [ 0 ]->dwStatusBits |= 1; // bit 1
			else
				pObject->ppFrameList [ 0 ]->dwStatusBits &= 1; // bit 1
		}
	}
}

DARKSDK_DLL void SetFrameEx ( int iID, float fFrame )
{
	// by default it does not recalculate bounds as it is slow
	SetFrameEx ( iID, fFrame, 0 );
}

DARKSDK_DLL void SetFrame ( int iID, int iFrame )
{
	SetFrameEx ( iID, (float)iFrame );
}

DARKSDK_DLL void SetFrameEx ( int iID, int iLimbID, float fFrame, int iEnableOverride )
{
	// check the object exists (and limb)
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// can set an animation frame override per limb
	if ( pObject->pfAnimLimbFrame==NULL )
	{
		// must allocate it if not currently created
		pObject->pfAnimLimbFrame = new float [ pObject->iFrameCount ];
		for ( int iN=0; iN<pObject->iFrameCount; iN++ )
			pObject->pfAnimLimbFrame [ iN ] = -1.0f;
	}

	// set the per limb animation frame
	switch ( iEnableOverride )
	{
		case 0 : pObject->pfAnimLimbFrame [ iLimbID ] = -1.0f;	break; // normal
		case 1 : pObject->pfAnimLimbFrame [ iLimbID ] = fFrame;	break; // override with new frame
		case 2 : pObject->pfAnimLimbFrame [ iLimbID ] = -2.0f;	break; // disable all animation (manual angles only)
	}

	// ensure object is updated
	pObject->bAnimUpdateOnce = true;
}

DARKSDK_DLL void SetInterpolation ( int iID, int iJump )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// control animation
	sObject* pObject = g_ObjectList [ iID ];
	pObject->fAnimInterp = (float)iJump/100.0f;

	// leefix - 210303 - to switch off interpolation
	if ( iJump==100 ) pObject->bAnimManualSlerp=false;
}

DARKSDK_DLL void SaveAnimation ( int iID, SDK_LPSTR pFilename )
{
	// Not Implemented in DBPRO V1 RELEASE
	RunTimeError(RUNTIMEERROR_COMMANDNOWOBSOLETE);
}

DARKSDK_DLL void AppendAnimation ( int iID, SDK_LPSTR pFilename )
{
	// Not Implemented in DBPRO V1 RELEASE
	RunTimeError(RUNTIMEERROR_COMMANDNOWOBSOLETE);
}

DARKSDK_DLL void SetAnimationMode ( int iID, int iMode )
{
	// Not Implemented in DBPRO V1 RELEASE
	RunTimeError(RUNTIMEERROR_COMMANDNOWOBSOLETE);
}

// Visual Commands

DARKSDK_DLL void AddVisibilityListMask ( int iID )
{
	m_ObjectManager.m_dwSkipVisibilityListMask |= (1<<iID);
}

DARKSDK_DLL void Hide ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// set object to visibility
	sObject* pObject = g_ObjectList [ iID ];
	pObject->bVisible = false;
}

DARKSDK_DLL void Show ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// set object to visibility
	sObject* pObject = g_ObjectList [ iID ];
	pObject->bVisible = true;
}

DARKSDK_DLL void SetRotationXYZ ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// object rotation setting
	sObject* pObject = g_ObjectList [ iID ];
	pObject->position.dwRotationOrder = ROTORDER_XYZ;
}

DARKSDK_DLL void SetRotationZYX ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// object rotation setting
	sObject* pObject = g_ObjectList [ iID ];
	pObject->position.dwRotationOrder = ROTORDER_ZYX;
}

DARKSDK_DLL void SetRotationZXY ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// object rotation setting
	sObject* pObject = g_ObjectList [ iID ];
	pObject->position.dwRotationOrder = ROTORDER_ZXY;
}

DARKSDK_DLL void GhostOn ( int iID, int iFlag )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject->pInstanceOfObject==NULL )
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
			SetGhost ( pObject->ppMeshList [ iMesh ], true, iFlag );

	// promote to overlay layer
	pObject->bGhostedObject = true;

	UpdateOverlayFlag ( pObject );
}

DARKSDK_DLL void GhostOn ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// use higher function
	GhostOn ( iID, -1 );
}

DARKSDK_DLL void GhostOff ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetGhost ( pObject->ppMeshList [ iMesh ], false, -1 );

	// promote to overlay layer
	pObject->bGhostedObject = false;
	UpdateOverlayFlag ( pObject );
}

DARKSDK_DLL void Fade ( int iID, float fPercentage )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// knock down perc
	fPercentage/=100.0f;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetDiffuse ( pObject->ppMeshList [ iMesh ], fPercentage );
}

void GlueToLimbEx ( int iSource, int iTarget, int iLimbID, int iMode )
{
	// check the object exists
	if ( !ConfirmObject ( iSource ) )
		return;

	//if ( !ConfirmObjectInstance ( iSource ) )
	//	return;

	// leefix - 100304 - check the object exists (and limb)
	if ( !ConfirmObjectAndLimb ( iTarget, iLimbID ) )
		return;

	//if ( !ConfirmObjectAndLimbInstance ( iTarget, iLimbID ) )
	//	return;

	// get object pointers
	sObject* pSourceObject = g_ObjectList [ iSource ];

	// assign target limb to override source position data
	pSourceObject->position.bGlued			= true;
	pSourceObject->position.iGluedToObj		= iTarget;

	// glue mode
	if ( iMode==1 )
	{
		// mode 1 - wipe out frame orient, leaving position only (avoid hierarchy frame problems)
		pSourceObject->position.iGluedToMesh	= iLimbID * -1;
	}
	else
	{
		if ( iMode==2 )
		{
			// mode 2 - wipe out child object position for accurate limb location placement
			pSourceObject->position.iGluedToMesh	= iLimbID;

			// leeadd - 150306 - u60b3 - reset position
			pSourceObject->position.vecPosition = D3DXVECTOR3 ( 0, 0, 0 );
		}
		else
		{
			// mode 0 - regular glue object to a limb (default behaviour)
			pSourceObject->position.iGluedToMesh	= iLimbID;
		}
	}
}

DARKSDK_DLL void GlueToLimb ( int iSource, int iTarget, int iLimbID )
{
	GlueToLimbEx ( iSource, iTarget, iLimbID, 0 );
}

DARKSDK_DLL void UnGlue ( int iSource )
{
	// check the object exists
	if ( !ConfirmObject ( iSource ) )
		return;

	// get object pointer
	sObject* pSourceObject = g_ObjectList [ iSource ];

	// get target object pointer
	int iTarget = pSourceObject->position.iGluedToObj;
	if ( iTarget > 0 )
	{
		sObject* pTargetObject = g_ObjectList [ iTarget ];
		if ( pTargetObject )
		{
			// set new position of source to target
			int iFrameIndex = abs ( pSourceObject->position.iGluedToMesh );
			if ( iFrameIndex < pTargetObject->iFrameCount )
			{
				pSourceObject->position.vecPosition.x	= pTargetObject->ppFrameList [ iFrameIndex ]->matAbsoluteWorld._41;
				pSourceObject->position.vecPosition.y	= pTargetObject->ppFrameList [ iFrameIndex ]->matAbsoluteWorld._42;
				pSourceObject->position.vecPosition.z	= pTargetObject->ppFrameList [ iFrameIndex ]->matAbsoluteWorld._43;
			}
			
			// wipe out glue assignment
			pSourceObject->position.bGlued			= false;
			pSourceObject->position.iGluedToObj		= 0;
			pSourceObject->position.iGluedToMesh	= 0;
		}
	}
}

DARKSDK_DLL void UnGlueAllObjects ( void )
{
	// mike - 051005 - new function for ravey

	// check the object exists
	for ( int iSource = 0; iSource < g_iObjectListCount; iSource++ )
	{
		if ( !ConfirmObject ( iSource ) )
			return;

		// get object pointer
		sObject* pSourceObject = g_ObjectList [ iSource ];

		// get target object pointer
		int iTarget = pSourceObject->position.iGluedToObj;
		if ( iTarget > 0 )
		{
			sObject* pTargetObject = g_ObjectList [ iTarget ];
			if ( pTargetObject )
			{
				// set new position of source to target
				int iFrameIndex = abs ( pSourceObject->position.iGluedToMesh );
				
				CalcObjectWorld ( pSourceObject );
				CalcObjectWorld ( pTargetObject );

				DWORD x = GetLimbXDirection ( iSource, 0 );
				DWORD y = GetLimbYDirection ( iSource, 0 );
				DWORD z = GetLimbZDirection ( iSource, 0 );

				float fX = *( float* ) &x;
				float fY = *( float* ) &y;
				float fZ = *( float* ) &z;

				Rotate ( iSource, fX, fY, fZ );

				pSourceObject->position.vecPosition.x = pSourceObject->position.matWorld._41;
				pSourceObject->position.vecPosition.y = pSourceObject->position.matWorld._42;
				pSourceObject->position.vecPosition.z = pSourceObject->position.matWorld._43;

				// wipe out glue assignment
				pSourceObject->position.bGlued			= false;
				pSourceObject->position.iGluedToObj		= 0;
				pSourceObject->position.iGluedToMesh	= 0;
			}
		}
	}
}

DARKSDK_DLL void LockOn ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];

	// promote to overlay layer
	pObject->bLockedObject = true;
	UpdateOverlayFlag ( pObject );
}

DARKSDK_DLL void LockOff ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];

	// promote to overlay layer
	pObject->bLockedObject = false;
	UpdateOverlayFlag ( pObject );
}

DARKSDK_DLL void DisableZDepth ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];

	// promote to overlay layer
	pObject->bNewZLayerObject = true;
	UpdateOverlayFlag ( pObject );
}

DARKSDK_DLL void EnableZDepth ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];

	// promote to overlay layer
	pObject->bNewZLayerObject = false;
	UpdateOverlayFlag ( pObject );
}

DARKSDK_DLL void DisableZRead ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetZRead ( pObject->ppMeshList [ iMesh ], false );
}

DARKSDK_DLL void EnableZRead ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetZRead ( pObject->ppMeshList [ iMesh ], true );
}

DARKSDK_DLL void DisableZWrite ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetZWrite ( pObject->ppMeshList [ iMesh ], false );
}

DARKSDK_DLL void EnableZWrite ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetZWrite ( pObject->ppMeshList [ iMesh ], true );
}

DARKSDK_DLL void DisableZBias ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetZBias ( pObject->ppMeshList [ iMesh ], false, 0.0f, 0.0f );
}

DARKSDK_DLL void EnableZBias ( int iID, float fSlopeScale, float fDepth )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetZBias ( pObject->ppMeshList [ iMesh ], true, fSlopeScale, fDepth );
}

DARKSDK_DLL void ReverseObjectFrames ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject ) 
	{
		int iFrameCount = pObject->iFrameCount;
		sFrame** pTempList = new sFrame*[iFrameCount];
		if ( pTempList )
		{
			for ( int iFrame=0; iFrame<iFrameCount; iFrame++ )
			{
				pTempList[iFrame] = pObject->ppFrameList[iFrame];
			}
			for ( int iFrame=0; iFrame<iFrameCount; iFrame++ )
			{
				pObject->ppFrameList[iFrameCount-1-iFrame] = pTempList[iFrame];
			}
			delete pTempList;
		}
	}
}

DARKSDK_DLL void SetObjectFOV ( int iID, float fFOV )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to object
	sObject* pObject = g_ObjectList [ iID ];
	pObject->fFOV = fFOV;
}

DARKSDK_DLL void FixPivot ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];

	// update to latest rotation
	UpdateObjectRotation ( pObject );

	// rotation to apply
	D3DXMATRIX matRotation;
	if ( pObject->position.bFreeFlightRotation )
		matRotation = pObject->position.matFreeFlightRotate;
	else
		matRotation = pObject->position.matRotation;

	// copy rotation to pivot and activate
	if ( pObject->position.bApplyPivot==false )
	{
		// first pivot capture
		pObject->position.bApplyPivot = true;
		pObject->position.matPivot = matRotation;
	}
	else
	{
		// compounded pivot capture
		pObject->position.matPivot = matRotation * pObject->position.matPivot;
	}

	// reset rotation
	pObject->position.bFreeFlightRotation = false;
	pObject->position.vecRotate = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	D3DXMatrixIdentity ( &pObject->position.matRotation );

	// regenerate look vectors
	RegenerateLookVectors( pObject );
}

void ResetPivot ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];

	// reset pivot and anim update flags
	pObject->position.bApplyPivot = false;
	pObject->bAnimUpdateOnce = true;

	// also reset user matrix
	for ( int iLimbID=0; iLimbID<pObject->iFrameCount; iLimbID++ )
	{
		D3DXMatrixIdentity ( &pObject->ppFrameList[iLimbID]->matUserMatrix );
	}
}

void SetToObjectOrientationEx ( int iID, int iWhichID, int iLimbID, int iMode )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// and possible limb
	if ( iLimbID!=-1 )
		if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
			return;

	// check the object exists
	if ( !ConfirmObject ( iWhichID ) )
		return;

	// get object pointers
	sObject* pObject = g_ObjectList [ iID ];
	sObject* pWhichObject = g_ObjectList [ iWhichID ];

	// copy data from 'which-object'
	if ( iLimbID!=-1 )
	{
		// FROM LIMB
		sFrame* pWhichFrame = pWhichObject->ppFrameList [ iLimbID ];
		if ( pWhichFrame )
		{
			pObject->position.bFreeFlightRotation	= true;
			pObject->position.matFreeFlightRotate	= pWhichFrame->matCombined;
		}
	}
	else
	{
		// FROM OBJECT
		pObject->position.bFreeFlightRotation	= pWhichObject->position.bFreeFlightRotation;
		pObject->position.matFreeFlightRotate	= pWhichObject->position.matFreeFlightRotate;
		pObject->position.dwRotationOrder		= pWhichObject->position.dwRotationOrder;
		pObject->position.vecRotate				= pWhichObject->position.vecRotate;
		pObject->position.matRotation			= pWhichObject->position.matRotation;
		pObject->position.matRotateX			= pWhichObject->position.matRotateX;
		pObject->position.matRotateY			= pWhichObject->position.matRotateY;
		pObject->position.matRotateZ			= pWhichObject->position.matRotateZ;
		
		// mike - 011005 - use or leave pivot
		if ( iMode == 1 )
		{
			pObject->position.bApplyPivot		= pWhichObject->position.bApplyPivot;
			pObject->position.matPivot			= pWhichObject->position.matPivot;
		}

		pObject->position.vecLook				= pWhichObject->position.vecLook;
		pObject->position.vecUp					= pWhichObject->position.vecUp;
		pObject->position.vecRight				= pWhichObject->position.vecRight;
	}
}

DARKSDK_DLL void SetToObjectOrientation ( int iID, int iWhichID )
{
	SetToObjectOrientationEx ( iID, iWhichID, -1, 0 );
}

DARKSDK_DLL void SetToObjectOrientation ( int iID, int iWhichID, int iMode )
{
	SetToObjectOrientationEx ( iID, iWhichID, -1, iMode );
}

DARKSDK_DLL void SetToCameraOrientation ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];

	// get camera pointer
	tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );

	// copy data from camera zero
	pObject->position.vecLook				= m_Camera_Ptr->vecLook;
	pObject->position.vecUp					= m_Camera_Ptr->vecUp;
	pObject->position.vecRight				= m_Camera_Ptr->vecRight;
	pObject->position.bFreeFlightRotation	= m_Camera_Ptr->bUseFreeFlightRotation;

    // camera and object free flights are inverse of each other
    // pObject->position.matFreeFlightRotate = m_Camera_Ptr->matFreeFlightRotate;
	FLOAT fDeterminant;
	D3DXMatrixInverse ( &pObject->position.matFreeFlightRotate, &fDeterminant, &m_Camera_Ptr->matFreeFlightRotate );

	pObject->position.vecRotate.x			= m_Camera_Ptr->fXRotate;
	pObject->position.vecRotate.y			= m_Camera_Ptr->fYRotate;
	pObject->position.vecRotate.z			= m_Camera_Ptr->fZRotate;
	if ( m_Camera_Ptr->bRotate )
		pObject->position.dwRotationOrder		= ROTORDER_XYZ;
	else
		pObject->position.dwRotationOrder		= ROTORDER_ZYX;

	// update object with new rotation
}

// Texture commands

DARKSDK_DLL void SetTexture ( int iID, int iImage )
{
	// check the object exists
	g_pGlob->dwInternalFunctionCode=12001;
	if ( !ConfirmObject ( iID ) )
		return;

	// apply to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetBaseTexture ( pObject->ppMeshList [ iMesh ], -1, iImage );

	// u74b7 - Removed 'Delete old stencil effect (if any)'
	// m_ObjectManager.DeleteStencilEffect ( pObject );

	// trigger a ew-new and re-sort
	m_ObjectManager.RenewReplacedMeshes ( pObject );
	m_ObjectManager.UpdateTextures ( );
	g_pGlob->dwInternalFunctionCode=12002;
}

DARKSDK_DLL void SetTextureStageEx ( int iID, int iStage, int iImage, int iDoNotSortTextures )
{
	// check the object exists
	g_pGlob->dwInternalFunctionCode=11001;
	if ( !ConfirmObject ( iID ) )
		return;

	// apply to all meshes
	g_pGlob->dwInternalFunctionCode=11011;
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetBaseTextureStage ( pObject->ppMeshList [ iMesh ], iStage, iImage );

	// u74b7 - Removed 'Delete old stencil effect (if any)'
	// g_pGlob->dwInternalFunctionCode=11021;
	// m_ObjectManager.DeleteStencilEffect ( pObject );

	// trigger a ew-new
	g_pGlob->dwInternalFunctionCode=11022;
	m_ObjectManager.RenewReplacedMeshes ( pObject );

	// res-sort textures only if flagged
	if ( iDoNotSortTextures==0 ) m_ObjectManager.UpdateTextures ( );
	g_pGlob->dwInternalFunctionCode=11023;
}

DARKSDK_DLL void SetTextureStage ( int iID, int iStage, int iImage )
{
	SetTextureStageEx ( iID, iStage, iImage, 0 );
}

DARKSDK_DLL void ScrollTexture ( int iID, int iStage, float fU, float fV )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		ScrollTexture ( pObject->ppMeshList [ iMesh ], iStage, fU, fV );

	// lee - 130206 - update 'original' data to reflect this UV change
	UpdateVertexDataInMesh ( pObject );
}

DARKSDK_DLL void ScrollTexture ( int iID, float fU, float fV )
{
	// refers to core function above
	ScrollTexture ( iID, 0, fU, fV );
}

DARKSDK_DLL void ScaleTexture ( int iID, int iStage, float fU, float fV )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// if object has effect applied, skip UV vertex write and pass into vars for later use when setting effect constant
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject==NULL ) return;

	cSpecialEffect* pEff = NULL;
	if ( pObject->ppMeshList[0] ) pEff = pObject->ppMeshList[0]->pVertexShaderEffect;
	if ( pEff!=NULL )
	{
		// apply to all meshes
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		{
			sMesh* pMesh = pObject->ppMeshList [ iMesh ];
			if ( pMesh )
			{
				pMesh->fUVScalingU = fU;
				pMesh->fUVScalingV = fV;
			}
		}
	}
	else
	{
		// apply to all meshes
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
			ScaleTexture ( pObject->ppMeshList [ iMesh ], iStage, fU, fV );

		// lee - 130206 - update 'original' data to reflect this UV change
		UpdateVertexDataInMesh ( pObject );
	}
}

DARKSDK_DLL void ScaleTexture ( int iID, float fU, float fV )
{
	// refers to core function above
	ScaleTexture ( iID, 0, fU, fV );
}


DARKSDK_DLL void SetSmoothing ( int iID, float fPercentage )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// limit percentage range
	if ( fPercentage<0.0f ) fPercentage=0.0f;
	if ( fPercentage>100.0f ) fPercentage=100.0f;

	// apply to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SmoothNormals ( pObject->ppMeshList [ iMesh ], fPercentage/100.0f );

	// lee - 130206 - update 'original' data to reflect this UV change
	// lee - 140306 - not needed for normal-data changes
//	UpdateVertexDataInMesh ( pObject );
}

DARKSDK_DLL void SetNormals ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		GenerateNormals ( pObject->ppMeshList [ iMesh ] );

	// lee - 130206 - update 'original' data to reflect this UV change
	UpdateVertexDataInMesh ( pObject );
}

DARKSDK_DLL void SetTextureModeStage ( int iID, int iStage, int iMode, int iMipGeneration )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply setting to all meshes (for one stage only)
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetTextureMode ( pObject->ppMeshList [ iMesh ], iStage, iMode, iMipGeneration );
}

DARKSDK_DLL void SetTextureMode ( int iID, int iMode, int iMipGeneration )
{
	// iMode (AddressU and AddressV)
	// D3DTADDRESS_WRAP = 1
    // D3DTADDRESS_MIRROR = 2
    // D3DTADDRESS_CLAMP = 3
    // D3DTADDRESS_BORDER = 4
    // D3DTADDRESS_MIRRORONCE = 5
	// iMipGeneration (MipStage)
	// D3DTEXF_NONE = 0
	// D3DTEXF_POINT = 1
	// D3DTEXF_LINEAR = 2
	// D3DTEXF_ANISOTROPIC = 3
	// D3DTEXF_PYRAMIDALQUAD = 6
	// D3DTEXF_GAUSSIANQUAD = 7
	SetTextureModeStage ( iID, 0, iMode, iMipGeneration );
}

DARKSDK_DLL void SetLightMap ( int iID, int iImage, int iAddDIffuseToStageZero )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// added for the benefit of FPSXC104RC7 (used for darklight replacement to built-in lightmapper)
	if ( iAddDIffuseToStageZero==0 )
	{
		// regular light mapping blend
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
			SetMultiTexture ( pObject->ppMeshList [ iMesh ], 1, D3DTOP_MODULATE, 3, iImage );
	}
	else
	{
		// the idea is we want DIFFUSE+lightmap in stage zero, and texture on stage two
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		{
			sMesh* pMesh = pObject->ppMeshList [ iMesh ];
			if ( pMesh->dwTextureCount>=2 )
			{
				// switch them 
				pMesh->pTextures [ 0 ].dwBlendMode		= D3DTOP_ADD;
				pMesh->pTextures [ 0 ].dwBlendArg1		= D3DTA_TEXTURE;
				pMesh->pTextures [ 0 ].dwBlendArg2		= D3DTA_DIFFUSE;
				pMesh->pTextures [ 1 ].dwBlendMode		= D3DTOP_MODULATE;
				pMesh->pTextures [ 1 ].dwBlendArg1		= D3DTA_TEXTURE;
				pMesh->pTextures [ 1 ].dwBlendArg2		= D3DTA_CURRENT;
			}
		}
	}

	// trigger a ew-new and re-sort
	m_ObjectManager.RenewReplacedMeshes ( pObject );
	m_ObjectManager.UpdateTextures ( );
}

DARKSDK_DLL void SetLightMap ( int iID, int iImage )
{
	SetLightMap ( iID, iImage, 0 );
}

DARKSDK_DLL void SetSphereMap ( int iID, int iSphereImage )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetMultiTexture ( pObject->ppMeshList [ iMesh ], 1, D3DTOP_MODULATE, 1, iSphereImage );

	// trigger a ew-new and re-sort
	m_ObjectManager.RenewReplacedMeshes ( pObject );
	m_ObjectManager.UpdateTextures ( );
}

DARKSDK_DLL void SetCubeMap ( int iID, int i1, int i2, int i3, int i4, int i5, int i6 )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// Cube is on stage one
	int iStage = 1;

	// generate cube map 
	LPDIRECT3DCUBETEXTURE9 pCubeTexture = NULL;
	pCubeTexture = CreateNewImageCubeMap ( i1, i2, i3, i4, i5, i6 );

	// These six images are sources for a cubemap, so if the images are used as
	// camera render targets, we can use a cube face instead
	if ( g_Image_SetCubeFace )
	{
		g_Image_SetCubeFace ( i1, pCubeTexture, 0 );
		g_Image_SetCubeFace ( i2, pCubeTexture, 1 );
		g_Image_SetCubeFace ( i3, pCubeTexture, 2 );
		g_Image_SetCubeFace ( i4, pCubeTexture, 3 );
		g_Image_SetCubeFace ( i5, pCubeTexture, 4 );
		g_Image_SetCubeFace ( i6, pCubeTexture, 5 );
	}

	// apply to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		// mesh ptr
		sMesh* pMesh = pObject->ppMeshList [ iMesh ];

		// apply cube map reference to mesh
		SetCubeTexture ( pMesh, iStage, pCubeTexture );

		// U75 - 120809 - also, if currently using shader, ensure this dynamic cube map is allowed in manager
		if ( pMesh->pVertexShaderEffect )
		{
			DWORD dwCorrectBitForThisStage = 1 << iStage;
			pMesh->pVertexShaderEffect->m_dwUseDynamicTextureMask = pMesh->pVertexShaderEffect->m_dwUseDynamicTextureMask | dwCorrectBitForThisStage;
		}
	}

	// trigger a ew-new and re-sort
	m_ObjectManager.RenewReplacedMeshes ( pObject );
	m_ObjectManager.UpdateTextures ( );
}

DARKSDK_DLL void SetDetailMap ( int iID, int iImage )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetMultiTexture ( pObject->ppMeshList [ iMesh ], 1, D3DTOP_ADD, 3, iImage );

	// trigger a ew-new and re-sort
	m_ObjectManager.RenewReplacedMeshes ( pObject );
	m_ObjectManager.UpdateTextures ( );
}

DARKSDK_DLL void SetBlendMap ( int iID, int iLimbID, int iStage, int iImage, int iTexCoordMode, int iMode, int iA, int iB, int iC, int iR )
{
	// BLEND MODE PARAMS
	// iStage 0-7
	// iImage Index
	// iTexCoordMode
	// 0 - Regular UV Stage Match
	// 1 - Sphere Mapping UV Data
	// 2 - Cube Mapping UV Data
	// 3 - Steal UV Data From Stage Zero
	// 10-17 - Take UV Data From Stage.. (10=0,11-1,etc)
	// iBlendMode
	// D3DTOP_DISABLE = 1,
	// D3DTOP_SELECTARG1 = 2,
	// D3DTOP_SELECTARG2 = 3,
	// D3DTOP_MODULATE = 4,
	// D3DTOP_MODULATE2X = 5,
	// D3DTOP_MODULATE4X = 6,
	// D3DTOP_ADD = 7,
	// D3DTOP_ADDSIGNED = 8,
	// D3DTOP_ADDSIGNED2X = 9,
	// D3DTOP_SUBTRACT = 10,
	// D3DTOP_ADDSMOOTH = 11,
	// D3DTOP_BLENDDIFFUSEALPHA = 12,
	// D3DTOP_BLENDTEXTUREALPHA = 13,
	// D3DTOP_BLENDFACTORALPHA = 14,
	// D3DTOP_BLENDTEXTUREALPHAPM = 15,
	// D3DTOP_BLENDCURRENTALPHA = 16,
	// D3DTOP_PREMODULATE = 17,
	// D3DTOP_MODULATEALPHA_ADDCOLOR = 18,
	// D3DTOP_MODULATECOLOR_ADDALPHA = 19,
	// D3DTOP_MODULATEINVALPHA_ADDCOLOR = 20,
	// D3DTOP_MODULATEINVCOLOR_ADDALPHA = 21,
	// D3DTOP_BUMPENVMAP = 22,
	// D3DTOP_BUMPENVMAPLUMINANCE = 23,
	// D3DTOP_DOTPRODUCT3 = 24,
	// D3DTOP_MULTIPLYADD = 25,
	// D3DTOP_LERP = 26,
	// iA, iB, iC, iR: D3DTA's : default is D3DTA_TEXTURE,D3DTA_CURRENT,-1,D3DTA_CURRENT
	// iForceArgX
	// [forcably change the COLORARG values]
	// D3DTA_DIFFUSE = 0          0x00000000  // select diffuse color (read only)
	// D3DTA_CURRENT = 1          0x00000001  // select stage destination register (read/write)
	// D3DTA_TEXTURE = 2          0x00000002  // select texture color (read only)
	// D3DTA_TFACTOR = 3          0x00000003  // select D3DRS_TEXTUREFACTOR (read only)
	// D3DTA_SPECULAR = 4         0x00000004  // select specular color (read only)
	// D3DTA_TEMP = 5             0x00000005  // select temporary register color (read/write)
	// D3DTA_CONSTANT = 6         0x00000006  // select texture stage constant
	// D3DTA_COMPLEMENT = 16      0x00000010  // take 1.0 - x (read modifier)
	// D3DTA_ALPHAREPLICATE = 32  0x00000020  // replicate alpha to color components (read modifier)

	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// U74 - 080509 - is limb number ok
	if ( iLimbID!=-1 )
		if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
			return;

	// apply to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	if ( iLimbID==-1 )
	{
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
			SetMultiTexture ( pObject->ppMeshList [ iMesh ], iStage, (DWORD)iMode, iTexCoordMode, iImage );

		// U73 - 210309 - apply D3DTA values to all meshes (if applicable)
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		{
			if ( iA!=-1 ) pObject->ppMeshList [ iMesh ]->pTextures [ iStage ].dwBlendArg1 = iA;
			if ( iB!=-1 ) pObject->ppMeshList [ iMesh ]->pTextures [ iStage ].dwBlendArg2 = iB;
			if ( iC!=-1 ) pObject->ppMeshList [ iMesh ]->pTextures [ iStage ].dwBlendArg0 = iC;
			if ( iR!=-1 ) pObject->ppMeshList [ iMesh ]->pTextures [ iStage ].dwBlendArgR = iR;
		}
	}
	else
	{
		// U74 - 080509 - single limb
		sFrame* pFrame = pObject->ppFrameList[iLimbID];
		if ( pFrame )
		{
			if ( pFrame->pMesh )
			{
				SetMultiTexture ( pFrame->pMesh, iStage, (DWORD)iMode, iTexCoordMode, iImage );
				if ( iA!=-1 ) pFrame->pMesh->pTextures [ iStage ].dwBlendArg1 = iA;
				if ( iB!=-1 ) pFrame->pMesh->pTextures [ iStage ].dwBlendArg2 = iB;
				if ( iC!=-1 ) pFrame->pMesh->pTextures [ iStage ].dwBlendArg0 = iC;
				if ( iR!=-1 ) pFrame->pMesh->pTextures [ iStage ].dwBlendArgR = iR;
			}
		}
	}

	// trigger a ew-new and re-sort
	m_ObjectManager.RenewReplacedMeshes ( pObject );
	m_ObjectManager.UpdateTextures ( );
}

DARKSDK_DLL void SetBlendMap ( int iID, int iStage, int iImage, int iTexCoordMode, int iMode, int iA, int iB, int iC, int iR )
{
	// U75 - 080509 - default all meshes, specify -1 for mesh param
	SetBlendMap ( iID, -1, iStage, iImage, iTexCoordMode, iMode, iA, iB, iC, iR );
}

DARKSDK_DLL void SetBlendMap ( int iID, int iStage, int iImage, int iTexCoordMode, int iMode )
{
	// default blend mapping command up to U73 - 210309 - replaced with larger blending command for lerping
	SetBlendMap ( iID, iStage, iImage, iTexCoordMode, iMode, -1, -1, -1, -1 );
}

DARKSDK_DLL void SetBlendMap ( int iID, int iImage, int iMode )
{
	SetBlendMap ( iID, 1, iImage, 3, iMode );
}

DARKSDK_DLL void SetTextureMD3 ( int iID, int iH0, int iH1, int iL0, int iL1, int iL2, int iU0 )
{
	// MIKEMIKE : Fits in with MD3 format of what DBO will make of it..[MD3]
}

DARKSDK_DLL int SwitchRenderTargetToDepthTexture ( int iFlag )
{
	return m_ObjectManager.SwitchRenderTargetToDepth(iFlag);
}


// New Texture Functions

DARKSDK_DLL void SetAlphaFactor ( int iID, float fPercentage )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// get object ptr
	sObject* pActualObject = g_ObjectList [ iID ];

	// new mode to apply color for highlighting 
	if ( fPercentage > 100.0f )
	{
		D3DCOLOR dwColorValueOnly = D3DCOLOR_ARGB ( 0, 0, 0, 0 );
		if ( fPercentage <= 102.0f )
		{
			if ( fPercentage==101.0f )
			{
				dwColorValueOnly = D3DCOLOR_ARGB ( 255, 128, 0, 0 );
			}
			else
			{
				dwColorValueOnly = D3DCOLOR_ARGB ( 255, 128, 0, 64 );
			}
		}
		else
		{
			if ( fPercentage==103.0f )
			{
				dwColorValueOnly = D3DCOLOR_ARGB ( 255, 0, 128, 0 );
			}
			else
			{
				dwColorValueOnly = D3DCOLOR_ARGB ( 255, 0, 128, 64 );
			}
		}
		pActualObject->dwInstanceAlphaOverride = dwColorValueOnly;
		pActualObject->bInstanceAlphaOverride = true;
		pActualObject->bTransparentObject = true;
	}
	else
	{
		// apply alpha factor
		if ( pActualObject->pInstanceOfObject )
		{
			// direct alpha factor effect on instance
			if ( fPercentage!=100.0f )
			{
				// some level of alpha, make transparent and set alpha value
				fPercentage/=100.0f;
				DWORD dwAlpha = (DWORD)(fPercentage*255);
				D3DCOLOR dwAlphaValueOnly = D3DCOLOR_ARGB ( dwAlpha, 0, 0, 0 );
				pActualObject->dwInstanceAlphaOverride = dwAlphaValueOnly;
				pActualObject->bInstanceAlphaOverride = true;
				pActualObject->bTransparentObject = true;
			}
			else
			{
				// no alpha, so make solid
				pActualObject->dwInstanceAlphaOverride = 0;
				pActualObject->bInstanceAlphaOverride = false;
				pActualObject->bTransparentObject = false;
			}
		}
		else
		{
			// apply to all meshes
			for ( int iMesh = 0; iMesh < pActualObject->iMeshCount; iMesh++ )
			{
				SetAlphaOverride ( pActualObject->ppMeshList [ iMesh ], fPercentage );
				SetTransparency ( pActualObject->ppMeshList [ iMesh ], true );
			}
		}
	}
}

// Built-in shader effects

DARKSDK_DLL void SetBumpMap ( int iID, int iBumpMap )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetBumpTexture ( pObject->ppMeshList [ iMesh ], iBumpMap, 1 );

	// as shader recreates mesh format, must regenerate buffer instance
	m_ObjectManager.RemoveObjectFromBuffers ( pObject );
	m_ObjectManager.AddObjectToBuffers ( pObject );
}

DARKSDK_DLL void SetCartoonShadingOn ( int iID, int iStandardImage, int iEdgeImage )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetCartoonTexture ( pObject->ppMeshList [ iMesh ], iStandardImage, iEdgeImage );

	// as shader recreates mesh format, must regenerate buffer instance
	m_ObjectManager.RemoveObjectFromBuffers ( pObject );
	m_ObjectManager.AddObjectToBuffers ( pObject );
}

DARKSDK_DLL void SetRainbowRenderingOn ( int iID, int iStandardImage )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetRainbowTexture ( pObject->ppMeshList [ iMesh ], iStandardImage );

	// as shader recreates mesh format, must regenerate buffer instance
	m_ObjectManager.RemoveObjectFromBuffers ( pObject );
	m_ObjectManager.AddObjectToBuffers ( pObject );
}

DARKSDK_DLL void SetEffectOn ( int iID, SDK_LPSTR pFilename, int iUseDefaultTextures )
{
	// check the object exists or not
	bool bUseDefaultModel = false;
	if ( !CheckObjectExist ( iID ) )
	{
		// create blank object to hold 'default model'
		MakePyramid ( iID, 1.0f );
		bUseDefaultModel = true;
	}

	// determine if we should use default textures
	bool bUseDefaultTextures = false;
	if ( iUseDefaultTextures==1 )
		bUseDefaultTextures = true;

	// Create external effect obj
	cSpecialEffect* pEffectObj = new cExternalEffect;
	pEffectObj->Load ( (char*)pFilename, bUseDefaultModel, bUseDefaultTextures );

	// reset vertex data before apply special effect
	sObject* pObject = g_ObjectList [ iID ];
	ResetVertexDataInMesh ( pObject );

	// apply to all meshes
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		// get mesh ptr
		sMesh* pMesh = pObject->ppMeshList [ iMesh ];

		// apply unique effect
		if ( !SetSpecialEffect ( pMesh, pEffectObj ) )
		{
			// failed to setup effect
			if ( bUseDefaultModel ) DeleteObject ( iID );
			RunTimeError ( RUNTIMEERROR_B3DVSHADERCANNOTCREATE );
			return;
		}

		// first mesh is original, rest are references only
		if ( iMesh==0 )
			pMesh->bVertexShaderEffectRefOnly = false;
		else
			pMesh->bVertexShaderEffectRefOnly = true;
	}

	// full or quick update of object
	if ( bUseDefaultModel )
	{
		// setup new object and introduce to buffers
		SetNewObjectFinalProperties ( iID, -1.0f );
	}
	else
	{
		// as shader recreates mesh format, must regenerate buffer instance
		m_ObjectManager.RemoveObjectFromBuffers ( pObject );
		m_ObjectManager.AddObjectToBuffers ( pObject );
	}
}

// FX effects commands

DARKSDK_DLL void LoadEffectEx ( SDK_LPSTR pFilename, int iEffectID, int iUseDefaultTextures, int iDoNotGenerateExtraData )
{
	// check the effect exists
	if ( !ConfirmNewEffect ( iEffectID ) )
		return;

	// determine if file even exists
	if ( !DoesFileExist ( (LPSTR)pFilename ) )
	{
		RunTimeError ( RUNTIMEERROR_FILENOTEXIST );
		return;
	}

	// determine if we should use default textures
	bool bUseDefaultTextures = false;
	if ( iUseDefaultTextures==1 )
		bUseDefaultTextures = true;

	// load the effect into array
	m_EffectList [ iEffectID ] = new sEffectItem;
	if ( m_EffectList [ iEffectID ]->pEffectObj )
	{
		// assign generate extra data flag then load the effect
		m_EffectList [ iEffectID ]->pEffectObj->m_dwEffectIndex = iEffectID;
		m_EffectList [ iEffectID ]->pEffectObj->m_bDoNotGenerateExtraData = (DWORD)iDoNotGenerateExtraData;
		if ( !m_EffectList [ iEffectID ]->pEffectObj->Load ( (char*)pFilename, false, bUseDefaultTextures ) )
		{
			// leefix - 200906 - u63 - if effect failed, still keep it in mem for delete effect clearup (can still checklist for errors on it)
			//SAFE_DELETE ( m_EffectList [ iEffectID ] );
		}
	}
}

DARKSDK_DLL void LoadEffect ( SDK_LPSTR pFilename, int iEffectID, int iUseDefaultTextures )
{
	// default call generates extra data such as normals, tangents/binormals, etc
	LoadEffectEx ( pFilename, iEffectID, iUseDefaultTextures, 0 );
}

DARKSDK_DLL void DeleteEffect ( int iEffectID )
{
	// check the effect exists
	if ( !ConfirmEffect ( iEffectID ) )
		return;

	// decouple depth render system if used
	sEffectItem* pEffectItem = m_EffectList [ iEffectID ];
	if ( pEffectItem )
	{
		if ( pEffectItem->pEffectObj )
		{
			if ( g_pMainCameraDepthEffect==pEffectItem->pEffectObj->m_pEffect )
			{
				g_pMainCameraDepthEffect = NULL;
				g_pMainCameraDepthHandle = NULL;
			}
		}
	}

	// check all other objects that reference this effect, if they reference - it then remove the link so it wont cause a crash later on
	for ( DWORD dwObject = 0; dwObject < (DWORD)g_iObjectListCount; dwObject++ )
	{
		sObject* pObject = g_ObjectList [ dwObject ];
		if ( pObject )
		{
			for ( DWORD dwMesh = 0; dwMesh < (DWORD)pObject->iMeshCount; dwMesh++ )
			{
				if ( pObject->ppMeshList [ dwMesh ]->pVertexShaderEffect == m_EffectList [ iEffectID ]->pEffectObj )
				{
					strcpy ( pObject->ppMeshList [ dwMesh ]->pEffectName, "" );
					pObject->ppMeshList [ dwMesh ]->pVertexShaderEffect = NULL;
				}
			}
		}
	}

	// delete effect from array
	SAFE_DELETE ( m_EffectList [ iEffectID ] );
}

// globals for now
#include "ShadowMapping\cShadowMaps.h"
int							g_PrimaryShadowEffect = 0;
CascadedShadowsManager      g_CascadedShadow;
CascadeConfig               g_CascadeConfig;
bool                        g_bMoveLightTexelSize = TRUE;
CFirstPersonCamera          g_ViewerCamera;          
CFirstPersonCamera          g_LightCamera;         
CFirstPersonCamera*         g_pActiveCamera = &g_ViewerCamera;

DARKSDK_DLL void SetEffectToShadowMapping ( int iEffectID )
{
	// setup effect to support shadow mapping
	g_PrimaryShadowEffect = iEffectID;
    g_CascadeConfig.m_nCascadeLevels = 4;
    g_CascadeConfig.m_iBufferSize = 1024;
    g_CascadedShadow.m_iCascadePartitionsZeroToOne[0] = 10;
    g_CascadedShadow.m_iCascadePartitionsZeroToOne[1] = 20;
    g_CascadedShadow.m_iCascadePartitionsZeroToOne[2] = 50;
    g_CascadedShadow.m_iCascadePartitionsZeroToOne[3] = 100;
    g_CascadedShadow.m_iCascadePartitionsZeroToOne[4] = 100;
    g_CascadedShadow.m_iCascadePartitionsZeroToOne[5] = 100;
    g_CascadedShadow.m_iCascadePartitionsZeroToOne[6] = 100;
    g_CascadedShadow.m_iCascadePartitionsZeroToOne[7] = 100;
    g_CascadedShadow.m_iCascadePartitionsMax = 100;
    SHADOW_TEXTURE_FORMAT sbt = (SHADOW_TEXTURE_FORMAT)0;
    g_CascadeConfig.m_ShadowBufferFormat = sbt;
    g_CascadedShadow.m_bMoveLightTexelSize = g_bMoveLightTexelSize;
    g_CascadedShadow.m_eSelectedCascadesFit = FIT_TO_SCENE;
    g_CascadedShadow.m_eSelectedNearFarFit =  FIT_NEARFAR_SCENE_AABB;
    //g_CascadedShadow.m_eSelectedNearFarFit =  FIT_NEARFAR_PANCAKING;
	
	g_CascadedShadow.m_fBlurBetweenCascadesAmount = 0.25f;

	// create resources for shadow mapper
    g_CascadedShadow.Init(	&g_ViewerCamera, &g_LightCamera, 
							&g_CascadeConfig );

	// cascade render mask (upto eight cascades)
	g_CascadedShadow.m_dwMask = 0xF;

	// complete
	return;
}

DARKSDK_DLL void SetEffectShadowMappingMode ( int iMode )
{
	// Can set the mask for which cascades get rendered
	g_CascadedShadow.m_dwMask = iMode;
}

int giActiveHack = 0;

DARKSDK_DLL void RenderEffectShadowMapping ( int iEffectID )
{
	// renders shadow maps for effect

	// check the effect exists
	if ( iEffectID!=0 )
		if ( !ConfirmEffect ( iEffectID ) )
			return;

	// effect pointer
	LPD3DXEFFECT pEffectPtr = NULL;
	cSpecialEffect* pEffectObject = m_EffectList [ iEffectID ]->pEffectObj;
	if ( pEffectObject )
		if ( pEffectObject->m_pEffect )
			pEffectPtr = pEffectObject->m_pEffect;

	// primary shadow map must produce the shadow
	if ( iEffectID==g_PrimaryShadowEffect )
	{
		// assign this effect as primary
		pEffectObject->m_bPrimaryEffectForCascadeShadowMapping = true;

		// set a clear color
		FLOAT ClearColor[4] = { 0.0f, 0.25f, 0.25f, 0.55f };

		// process shadow mapping frame
		//D3DMATRIX matStoreProj;
		//m_pD3D->GetTransform ( D3DTS_PROJECTION, &matStoreProj );
		//tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( 0 );
		//D3DXMatrixPerspectiveFovLH ( &m_Camera_Ptr->matProjection, D3DXToRadian(110.0f), m_Camera_Ptr->fAspect, m_Camera_Ptr->fZNear, m_Camera_Ptr->fZFar );
		//m_pD3D->SetTransform ( D3DTS_PROJECTION, &m_Camera_Ptr->matProjection );
		g_CascadedShadow.InitFrame( pEffectPtr );
		//m_pD3D->SetTransform ( D3DTS_PROJECTION, &matStoreProj );

		// set technique for depth rendering
		D3DXHANDLE hOldTechnique = pEffectPtr->GetCurrentTechnique();
		if ( pEffectPtr )
		{
			D3DXHANDLE hTechnique = pEffectPtr->GetTechniqueByName ( "DepthMap" );
			if ( hTechnique )
				pEffectPtr->SetTechnique(hTechnique);
		}

		// render all shadows in cascades (multiple shadow maps based on frustrum slices)
		g_CascadedShadow.RenderShadowsForAllCascades(pEffectPtr);// pd3dDevice, pd3dImmediateContext, g_pSelectedMesh);

		// Restore technique after depth renders
		if ( pEffectPtr && hOldTechnique )
			pEffectPtr->SetTechnique(hOldTechnique);
	}

	// set shaodw mapping settings for final render (for all effects that call this command inc. primary)
    g_CascadedShadow.RenderScene( iEffectID, pEffectPtr, NULL, NULL, NULL, false );

	// complete
	return;
}

DARKSDK_DLL void SetObjectEffectCore ( int iID, int iEffectID, int iEffectNoBoneID, int iForceCPUAnimationMode )
{
	// iForceCPUAnimationMode:
	// 0-GPU Animation
	// 1-Force CPU Animation
	// 2-GPU Anim + Hide all meshes with no bone data

	// check the object exists
	g_pGlob->dwInternalFunctionCode=10001;
	if ( !ConfirmObject ( iID ) )
		return;

	// check the effect exists
	if ( iEffectID!=0 )
		if ( !ConfirmEffect ( iEffectID ) )
			return;

	// check the effectnobone exists
	if ( iEffectNoBoneID!=0 )
		if ( !ConfirmEffect ( iEffectNoBoneID ) )
			return;

	// get object ptr
	g_pGlob->dwInternalFunctionCode=10002;
	sObject* pObject = g_ObjectList [ iID ];

	// leefix - 040805 - if object ALREADY has effect, must remove it first
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		sMesh* pMesh = pObject->ppMeshList [ iMesh ];
		SetSpecialEffect ( pMesh, NULL );
	}

	// reset vertex data before apply special effect
	ResetVertexDataInMesh ( pObject );

	// remove effect if zero
	if ( iEffectID>0 )
	{
		// apply to specific mesh
		g_pGlob->dwInternalFunctionCode=11000;
		sEffectItem* pEffectItem = m_EffectList [ iEffectID ];
		sEffectItem* pEffectNoBoneItem = NULL;
		if ( iEffectNoBoneID>0 ) pEffectNoBoneItem = m_EffectList [ iEffectNoBoneID ];

		// apply setting to all meshes
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		{
			// for each mesh
			g_pGlob->dwInternalFunctionCode=11001+iMesh;
			sMesh* pMesh = pObject->ppMeshList [ iMesh ];

			// if nobone effect index submitted, means we want to assign two effect types based on whether mesh has bones
			bool bShaderApplySuccess = false;
			if ( iEffectNoBoneID>0 && pEffectNoBoneItem!=NULL && pMesh->dwBoneCount==0 )
			{
				// apply specific NON-BONE effect passed in as optional parameter
				bShaderApplySuccess = SetSpecialEffect ( pMesh, pEffectNoBoneItem->pEffectObj );
			}
			else
			{
				// apply REGULAR (BONE) shared effect
				bShaderApplySuccess = SetSpecialEffect ( pMesh, pEffectItem->pEffectObj );
			}
			if ( bShaderApplySuccess==true )
			{
				pMesh->bVertexShaderEffectRefOnly = true;
				pMesh->dwForceCPUAnimationMode = (DWORD)iForceCPUAnimationMode;
				if ( pMesh->dwForceCPUAnimationMode>0 ) pMesh->dwForceCPUAnimationMode = 1;
				if ( iForceCPUAnimationMode==2 && pMesh->dwBoneCount==0 )
				{
					pMesh->bVisible = false;
				}
			}
			else
			{
				// lee - 300914 - maybe replace this with substitute technique?
				pMesh->bVisible = false;
			}
		}
	}
	else
	{
		// reset setting to all meshes
		g_pGlob->dwInternalFunctionCode=12001;
		sObject* pObject = g_ObjectList [ iID ];
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		{
			g_pGlob->dwInternalFunctionCode=12001+iMesh;
			sMesh* pMesh = pObject->ppMeshList [ iMesh ];
			SetSpecialEffect ( pMesh, NULL );
			pMesh->dwForceCPUAnimationMode = 0;
		}
	}

	// as recreates mesh format, must regenerate buffer instance
	g_pGlob->dwInternalFunctionCode=13001;
	m_ObjectManager.RemoveObjectFromBuffers ( pObject );
	m_ObjectManager.AddObjectToBuffers ( pObject );
	g_pGlob->dwInternalFunctionCode=13002;
}

DARKSDK_DLL void SetObjectEffectCore ( int iID, int iEffectID, int iForceCPUAnimationMode )
{
	SetObjectEffectCore ( iID, iEffectID, 0, iForceCPUAnimationMode );
}

DARKSDK_DLL void SetOcclusionMode ( int iOcclusionMode )
{
	// 0 - none
	// 1 - HOQ - Hardware Occlusion Queries
	// 2 - HZB - Hierarchical Z Buffer
	g_Occlusion.SetOcclusionMode ( iOcclusionMode );
}

DARKSDK_DLL void SetObjectOcclusion ( int iID, int iOcclusionShape, int iMeshOrLimbID, int iA, int iIsOccluder, int iDeleteFromOccluder )
{
	// iOcclusionShape & iMeshOrLimbID ignored for now

	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// HOQ Method
	if ( g_Occlusion.GetOcclusionMode()==1 )
	{
		// only if not already an occludabler
		if ( g_Occlusion.d3dQuery[pObject->dwObjectNumber] )
			return;

		// create query object
		m_pD3D->CreateQuery( D3DQUERYTYPE_OCCLUSION, &g_Occlusion.d3dQuery[pObject->dwObjectNumber] );
		g_Occlusion.d3dQuery[pObject->dwObjectNumber]->Issue( D3DISSUE_BEGIN );
		g_Occlusion.d3dQuery[pObject->dwObjectNumber]->Issue( D3DISSUE_END );
	}

	// HZB Method
	if ( g_Occlusion.GetOcclusionMode()==2 )
	{
		// check if need to delete
		if ( iDeleteFromOccluder==1 )
		{
			g_Occlusion.DeleteOccludite(pObject);
		}
		else
		{
			// this object will be used in the initial depth render to block other objects
			if ( iIsOccluder==2 )
			{
				g_Occlusion.AddOccluder(pObject);
			}
			else
			{
				g_Occlusion.AddOccludee(pObject);
				if ( iIsOccluder==1 )
				{
					g_Occlusion.AddOccluder(pObject);
				}
			}
		}
	}

	// complete
	return;
}

DARKSDK_DLL int GetObjectOcclusionValue ( int iID )
{
	// return var
	int iReturnValue = 0;

	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	if ( g_Occlusion.GetOcclusionMode()==1 )
	{
		// can get how many of its pixels got rendered
		sObject* pObject = g_ObjectList [ iID ];
		iReturnValue = g_Occlusion.dwQueryValue[pObject->dwObjectNumber];

		// and trigger the next query (does not happen each cycle as expensive for some GPUs)
		if ( g_Occlusion.iQueryBusyStage[pObject->dwObjectNumber]==0 )
			g_Occlusion.iQueryBusyStage[pObject->dwObjectNumber] = 99;
	}
	if ( g_Occlusion.GetOcclusionMode()==2 )
	{
		// 0-shown or 1-occluded
		iReturnValue = 0;
	}

	// return value
	return iReturnValue;
}

DARKSDK_DLL void SetObjectEffect ( int iID, int iEffectID )
{
	// call master core function for this
	SetObjectEffectCore ( iID, iEffectID, 0 );
}

DARKSDK_DLL void SetLimbEffect ( int iID, int iLimbID, int iEffectID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// check the effect exists
	if ( iEffectID!=0 )
		if ( !ConfirmEffect ( iEffectID ) )
			return;

	// ensure limb has mesh
	sObject* pObject = g_ObjectList [ iID ];
	sMesh* pMesh = pObject->ppFrameList [ iLimbID ]->pMesh;
	if ( !pMesh )
		return;

	// ensure mesh is reset
	ResetVertexDataInMeshPerMesh ( pMesh );

	// apply to specific mesh
	if ( iEffectID>0 )
	{
		sEffectItem* pEffectItem = m_EffectList [ iEffectID ];
		SetSpecialEffect ( pMesh, pEffectItem->pEffectObj );
		pMesh->bVertexShaderEffectRefOnly = true;
	}
	else
		SetSpecialEffect ( pMesh, NULL );

	// as recreates mesh format, must regenerate buffer instance
	m_ObjectManager.RemoveObjectFromBuffers ( pObject );
	m_ObjectManager.AddObjectToBuffers ( pObject );
}

DARKSDK_DLL void PerformChecklistForEffectValues ( int iEffectID )
{
	// check the effect exists
	if ( !ConfirmEffect ( iEffectID ) )
		return;

	// Get effect ptr and desc
	LPD3DXEFFECT pEffectPtr = NULL;
	cSpecialEffect* pEffectObj = NULL;
	if ( m_EffectList [ iEffectID ]->pEffectObj )
	{
		pEffectObj = m_EffectList [ iEffectID ]->pEffectObj;
		if ( pEffectObj->m_pEffect ) pEffectPtr = pEffectObj->m_pEffect;
	}

	// if effect tr valid
	if ( !pEffectPtr )
		return;

	// build checklist from all top-level params
	D3DXEFFECT_DESC	EffectDesc;
	pEffectPtr->GetDesc( &EffectDesc );

	// Generate Checklist
	DWORD dwMaxStringSizeInEnum=0;
	bool bCreateChecklistNow=false;
	g_pGlob->checklisthasvalues=true;
	g_pGlob->checklisthasstrings=true;
	for(int pass=0; pass<2; pass++)
	{
		if(pass==1)
		{
			// Ensure checklist is large enough
			bCreateChecklistNow=true;
			for(int c=0; c<g_pGlob->checklistqty; c++)
				GlobExpandChecklist(c, dwMaxStringSizeInEnum);
		}

		// Look at parameters
		g_pGlob->checklistqty=0;
		for( UINT iParam = 0; iParam < EffectDesc.Parameters; iParam++ )
		{
			// get this parameter handle and description
			D3DXPARAMETER_DESC ParamDesc;
			D3DXHANDLE hParam = pEffectPtr->GetParameter ( NULL, iParam );
			pEffectPtr->GetParameterDesc( hParam, &ParamDesc );

			// Add to checklist
			DWORD dwSize=0;
			if(ParamDesc.Name) dwSize=strlen(ParamDesc.Name);
			if(dwSize>dwMaxStringSizeInEnum) dwMaxStringSizeInEnum=dwSize;
			if(bCreateChecklistNow)
			{
				// New checklist item
				if(ParamDesc.Name==NULL)
					strcpy(g_pGlob->checklist[g_pGlob->checklistqty].string, "<noname>");
				else
					strcpy(g_pGlob->checklist[g_pGlob->checklistqty].string, ParamDesc.Name);

				// class and type form var type id (dbpro usage)
				int iVarTypeIdentity = 0;
				if ( ParamDesc.Class==D3DXPC_SCALAR )
				{
					if ( ParamDesc.Type==D3DXPT_BOOL ) iVarTypeIdentity = 1;
					if ( ParamDesc.Type==D3DXPT_INT ) iVarTypeIdentity = 2;
					if ( ParamDesc.Type==D3DXPT_FLOAT ) iVarTypeIdentity = 3;
				}
				if ( ParamDesc.Class==D3DXPC_VECTOR )
				{
					iVarTypeIdentity = 4;
				}
				if ( ParamDesc.Class==D3DXPC_MATRIX_ROWS
				||   ParamDesc.Class==D3DXPC_MATRIX_COLUMNS )
				{
					iVarTypeIdentity = 5;
				}
				g_pGlob->checklist[g_pGlob->checklistqty].valuea = iVarTypeIdentity;

				// whether app-hooked (dbpro providing the value)
				if ( pEffectObj->AssignValueHook ( (char*)ParamDesc.Semantic, NULL)==true )
					g_pGlob->checklist[g_pGlob->checklistqty].valueb = 1;
				else
				{
					// non semantic hook identities
					bool bHookIsValid = false;

					// by annotation
					D3DXHANDLE hAnnot = pEffectPtr->GetAnnotationByName( hParam, "UIDirectional" );
					if( hAnnot != NULL && pEffectObj->m_LightDirHandle ) bHookIsValid=true;
					hAnnot = pEffectPtr->GetAnnotationByName( hParam, "UIDirectionalInv" );
					if( hAnnot != NULL && pEffectObj->m_LightDirInvHandle ) bHookIsValid=true;
					hAnnot = pEffectPtr->GetAnnotationByName( hParam, "UIPosition" );
					if( hAnnot != NULL && pEffectObj->m_LightPosHandle ) bHookIsValid=true;
					hAnnot = pEffectPtr->GetAnnotationByName( hParam, "UIObject" );
					if( hAnnot != NULL )
					{
						// light type
						LPCSTR pstrLightType = NULL;
						if ( hAnnot != NULL ) pEffectPtr->GetString( hAnnot, &pstrLightType );
						if ( pstrLightType )
						{
							if ( _stricmp((char*)pstrLightType,"directionalight")==NULL && pEffectObj->m_LightDirHandle ) bHookIsValid=true;
							if ( _stricmp((char*)pstrLightType,"pointlight")==NULL && pEffectObj->m_LightPosHandle ) bHookIsValid=true;
							if ( _stricmp((char*)pstrLightType,"spotlight")==NULL && pEffectObj->m_LightPosHandle ) bHookIsValid=true;
						}
					}

					// special cases
					if( _stricmp ( ParamDesc.Name, "XFile" )==NULL ) bHookIsValid=true;
					if ( ParamDesc.Type>=D3DXPT_TEXTURE ) bHookIsValid=true;

					// assign result
					if ( bHookIsValid==true )
						g_pGlob->checklist[g_pGlob->checklistqty].valueb = 1;
					else
						g_pGlob->checklist[g_pGlob->checklistqty].valueb = 0;
				}

				// class
				g_pGlob->checklist[g_pGlob->checklistqty].valuec = ParamDesc.Class;

				// type
				g_pGlob->checklist[g_pGlob->checklistqty].valued = ParamDesc.Type;
			}
			g_pGlob->checklistqty++;
		}
	}
 
	// Determine if checklist has any contents
	if(g_pGlob->checklistqty>0)
		g_pGlob->checklistexists=true;
	else
		g_pGlob->checklistexists=false;
}

DARKSDK_DLL void PerformChecklistForEffectErrors ( void )
{
	// Generate Checklist
	DWORD dwMaxStringSizeInEnum=0;
	bool bCreateChecklistNow=false;
	g_pGlob->checklisthasvalues=false;
	g_pGlob->checklisthasstrings=true;
	for(int pass=0; pass<2; pass++)
	{
		if(pass==1)
		{
			// Ensure checklist is large enough
			bCreateChecklistNow=true;
			for(int c=0; c<g_pGlob->checklistqty; c++)
				GlobExpandChecklist(c, dwMaxStringSizeInEnum);
		}

		// Look at error buffer (if any)
		if ( g_pEffectErrorMsg ) 
		{
			LPSTR pPtr = g_pEffectErrorMsg;
			LPSTR pPtrEnd = g_pEffectErrorMsg + g_dwEffectErrorMsgSize;
			LPSTR pLastByte = pPtr;
			g_pGlob->checklistqty=0;
			while ( 1 )
			{
				if ( pPtr>=pPtrEnd || ( *(unsigned char*)pPtr==13 || *(unsigned char*)(pPtr+1)==10 ) )
				{
					// determine error line
					DWORD dwSize = (pPtr-pLastByte)+1;
					LPSTR pErrorLine = new char[dwSize];
					memcpy ( pErrorLine, pLastByte, dwSize );
					pErrorLine[dwSize]=0;

					// skip the colons
					int nn=-1; int iCount=2;
					for ( int n=0; n<(int)dwSize; n++)
					{
						if ( pErrorLine[n]==':' )
						{
							if ( iCount>0 )
							{
								nn=n+2;
								iCount--;
							}
							else
								break;
						}
					}
					if ( nn!=-1 )
					{
						_strrev ( pErrorLine );
						pErrorLine[dwSize-nn]=0;
						_strrev ( pErrorLine );
						dwSize = strlen(pErrorLine)+1;
					}

					// Add to checklist at end of line or buffer
					if(dwSize>dwMaxStringSizeInEnum) dwMaxStringSizeInEnum=dwSize;
					if(bCreateChecklistNow)
					{
						// New checklist item
						strcpy(g_pGlob->checklist[g_pGlob->checklistqty].string, pErrorLine);
					}
					g_pGlob->checklistqty++;

					// go for next line
					if ( pPtr>=pPtrEnd )
					{
						// exit buffer scan
						break;
					}
					else
					{
						// next line
						pPtr+=2; pLastByte=pPtr;
					}
				}
				else
				{
					// next byte
					pPtr++;
				}
			}
		}
	}
 
	// Determine if checklist has any contents
	if(g_pGlob->checklistqty>0)
		g_pGlob->checklistexists=true;
	else
		g_pGlob->checklistexists=false;
}

DARKSDK_DLL void PerformChecklistForEffectErrors ( int iEffectID )
{
	PerformChecklistForEffectErrors();
}

DARKSDK_DLL void SetEffectTranspose ( int iEffectID, int iTransposeFlag )
{
	// check the effect exists
	if ( !ConfirmEffect ( iEffectID ) )
		return;

	// Get effect ptr
	cSpecialEffect* pEffectObject = m_EffectList [ iEffectID ]->pEffectObj;

	// apply flag
	if ( iTransposeFlag==1 )
		pEffectObject->m_bTranposeToggle = true;
	else
		pEffectObject->m_bTranposeToggle = false;
}

DARKSDK_DLL int GetEffectParameterIndex ( int iEffectID, SDK_LPSTR pConstantName )
{
	// return unique index
	int iParameterIndex = -1;

	// check the effect exists
	if ( !ConfirmEffect ( iEffectID ) )
		return -1;

	// get effect ptr
	LPD3DXEFFECT pEffectPtr = NULL;
	cSpecialEffect* pEffectObject = m_EffectList [ iEffectID ]->pEffectObj;
	if ( pEffectObject )
		if ( pEffectObject->m_pEffect )
			pEffectPtr = pEffectObject->m_pEffect;

	// prevent system from overriding manual change by removing hook
	if ( pEffectObject && pEffectPtr )
	{
		D3DXHANDLE hConstantParamHandle = pEffectPtr->GetParameterByName ( NULL, (char*)pConstantName );
		if ( !g_EffectParamHandleList.empty() )
		{
			for ( DWORD iIndex = 0; iIndex < g_EffectParamHandleList.size(); ++iIndex )
			{
				D3DXHANDLE hThisHandle = g_EffectParamHandleList [ iIndex ];
				if ( hThisHandle==hConstantParamHandle )
				{
					// found param already in list, return unique index
					iParameterIndex = iIndex;
					hConstantParamHandle=NULL;
					break;
				}
			}
		}
		if ( hConstantParamHandle!=NULL )
		{
			pEffectObject->AssignValueHookCore ( NULL, hConstantParamHandle, 0, true ); //remove handle
			g_EffectParamHandleList.push_back ( hConstantParamHandle );
			iParameterIndex = g_EffectParamHandleList.size() - 1;
		}
	}

	// return unique index to parameter
	return iParameterIndex;
}

DARKSDK_DLL LPD3DXEFFECT SetEffectConstantCore ( int iEffectID, SDK_LPSTR pConstantName, int iOptionalParamIndex )
{
	// check the effect exists
	if ( !ConfirmEffect ( iEffectID ) )
		return NULL;

	// Get effect ptr
	LPD3DXEFFECT pEffectPtr = NULL;
	cSpecialEffect* pEffectObject = m_EffectList [ iEffectID ]->pEffectObj;
	if ( pEffectObject )
		if ( pEffectObject->m_pEffect )
			pEffectPtr = pEffectObject->m_pEffect;

	// special hook constant names for internal shader texture hooks
	if ( pConstantName )
	{
		if ( strnicmp ( (char*)pConstantName, "[hook-depth-data]", strlen((char*)pConstantName) )==NULL )
		{
			// find shader handle, assign to texture in effect
			g_pMainCameraDepthEffect = pEffectPtr;
			if ( g_pMainCameraDepthEffect!=NULL )
			{
				if ( g_pMainCameraDepthHandle==NULL )
				{
					g_pMainCameraDepthHandle = g_pMainCameraDepthEffect->GetParameterByName( NULL, "DepthTex" );
				}
			}
		}
	}

	// prevent system from overriding manual change by removing hook
	if ( pEffectObject && pEffectPtr )
	{
		if ( pConstantName==NULL )
		{
			// already removed hook for paramindex based constant setting
		}
		else
		{
			D3DXHANDLE hParam = pEffectPtr->GetParameterByName ( NULL, (char*)pConstantName );
			pEffectObject->AssignValueHookCore ( NULL, hParam, 0, true ); //remove handle
		}
	}

	// return effect ptr (if any)
	return pEffectPtr;
}

DARKSDK_DLL LPD3DXEFFECT SetEffectConstantCore ( int iEffectID, SDK_LPSTR pConstantName )
{
	return SetEffectConstantCore ( iEffectID, pConstantName, -1 );
}

DARKSDK_DLL void SetEffectConstantB ( int iEffectID, SDK_LPSTR pConstantName, int iOptionalParamIndex, int iValue )
{
	// get constant ptr
	LPD3DXEFFECT pEffectPtr = SetEffectConstantCore ( iEffectID, pConstantName );
	if ( pEffectPtr )
	{
		// apply value to constant
		D3DXHANDLE hParam;
		if ( pConstantName==NULL )
			hParam = g_EffectParamHandleList [ iOptionalParamIndex ];
		else
			hParam = pEffectPtr->GetParameterByName ( NULL, (char*)pConstantName );

		pEffectPtr->SetBool ( hParam, iValue );
	}
}

DARKSDK_DLL void SetEffectConstantB ( int iEffectID, SDK_LPSTR pConstantName, int iValue )
{
	SetEffectConstantB ( iEffectID, pConstantName, -1, iValue );
}

DARKSDK_DLL void SetEffectConstantBEx ( int iEffectID, int iParamIndex, int iValue )
{
	SetEffectConstantB ( iEffectID, NULL, iParamIndex, iValue );
}

DARKSDK_DLL void SetEffectConstantI ( int iEffectID, SDK_LPSTR pConstantName, int iOptionalParamIndex, int iValue )
{
	// get constant ptr
	LPD3DXEFFECT pEffectPtr = SetEffectConstantCore ( iEffectID, pConstantName );
	if ( pEffectPtr )
	{
		// apply value to constant
		D3DXHANDLE hParam;
		if ( pConstantName==NULL )
			hParam = g_EffectParamHandleList [ iOptionalParamIndex ];
		else
			hParam = pEffectPtr->GetParameterByName ( NULL, (char*)pConstantName );

		pEffectPtr->SetInt ( hParam, iValue );
	}
}

DARKSDK_DLL void SetEffectConstantI ( int iEffectID, SDK_LPSTR pConstantName, int iValue )
{
	SetEffectConstantI ( iEffectID, pConstantName, -1, iValue );
}

DARKSDK_DLL void SetEffectConstantIEx ( int iEffectID, int iParamIndex, int iValue )
{
	SetEffectConstantI ( iEffectID, NULL, iParamIndex, iValue );
}

DARKSDK_DLL void SetEffectConstantF ( int iEffectID, SDK_LPSTR pConstantName, int iOptionalParamIndex, float fValue )
{
	// get constant ptr
	LPD3DXEFFECT pEffectPtr = SetEffectConstantCore ( iEffectID, pConstantName );
	if ( pEffectPtr )
	{
		// apply value to constant
		D3DXHANDLE hParam;
		if ( pConstantName==NULL )
			hParam = g_EffectParamHandleList [ iOptionalParamIndex ];
		else
			hParam = pEffectPtr->GetParameterByName ( NULL, (char*)pConstantName );

		pEffectPtr->SetFloat ( hParam, fValue );
	}
}

DARKSDK_DLL void SetEffectConstantF ( int iEffectID, SDK_LPSTR pConstantName, float fValue )
{
	SetEffectConstantF ( iEffectID, pConstantName, -1, fValue );
}

DARKSDK_DLL void SetEffectConstantFEx ( int iEffectID, int iParamIndex, float fValue )
{
	SetEffectConstantF ( iEffectID, NULL, iParamIndex, fValue );
}

DARKSDK_DLL void SetEffectConstantV ( int iEffectID, SDK_LPSTR pConstantName, int iOptionalParamIndex, int iVector )
{
	// early out if no valid  param index
	if ( pConstantName==NULL && iOptionalParamIndex==-1 )
		return;

	// get constant ptr
	LPD3DXEFFECT pEffectPtr = SetEffectConstantCore ( iEffectID, pConstantName );
	if ( pEffectPtr )
	{
		// apply value to constant
		D3DXHANDLE hParam;
		if ( pConstantName==NULL )
			hParam = g_EffectParamHandleList [ iOptionalParamIndex ];
		else
			hParam = pEffectPtr->GetParameterByName ( NULL, (char*)pConstantName );

		D3DXVECTOR4 vecData = g_Types_GetVector ( iVector );
		pEffectPtr->SetVector ( hParam, &vecData );
	}
}

DARKSDK_DLL void SetEffectConstantV ( int iEffectID, SDK_LPSTR pConstantName, int iVector )
{
	SetEffectConstantV ( iEffectID, pConstantName, -1, iVector );
}

DARKSDK_DLL void SetEffectConstantVEx ( int iEffectID, int iParamIndex, int iValue )
{
	SetEffectConstantV ( iEffectID, NULL, iParamIndex, iValue );
}

DARKSDK_DLL void SetEffectConstantM ( int iEffectID, SDK_LPSTR pConstantName, int iOptionalParamIndex, int iMatrix )
{
	// early out if no valid  param index
	if ( pConstantName==NULL && iOptionalParamIndex==-1 )
		return;

	// get constant ptr
	LPD3DXEFFECT pEffectPtr = SetEffectConstantCore ( iEffectID, pConstantName );
	if ( pEffectPtr )
	{
		// apply value to constant
		D3DXHANDLE hParam;
		if ( pConstantName==NULL )
			hParam = g_EffectParamHandleList [ iOptionalParamIndex ];
		else
			hParam = pEffectPtr->GetParameterByName ( NULL, (char*)pConstantName );

		D3DXMATRIX matData = g_Types_GetMatrix ( iMatrix );
		pEffectPtr->SetMatrix ( hParam, &matData );
	}
}

DARKSDK_DLL void SetEffectConstantM ( int iEffectID, SDK_LPSTR pConstantName, int iMatrix )
{
	SetEffectConstantM ( iEffectID, pConstantName, -1, iMatrix );
}

DARKSDK_DLL void SetEffectConstantMEx ( int iEffectID, int iParamIndex, int iValue )
{
	SetEffectConstantM ( iEffectID, NULL, iParamIndex, iValue );
}

DARKSDK_DLL void SetEffectTechnique	( int iEffectID, SDK_LPSTR pTechniqueName )
{
	// check the effect exists
	if ( !ConfirmEffect ( iEffectID ) )
		return;

	// Get effect ptr
	LPD3DXEFFECT pEffectPtr = NULL;
	cSpecialEffect* pEffectObject = m_EffectList [ iEffectID ]->pEffectObj;
	if ( pEffectObject )
		if ( pEffectObject->m_pEffect )
			pEffectPtr = pEffectObject->m_pEffect;

	// Choose technique based on name
	if ( pEffectPtr )
	{
		D3DXHANDLE hTechnique = pEffectPtr->GetTechniqueByName ( (LPSTR)pTechniqueName );
		if ( hTechnique ) pEffectPtr->SetTechnique(hTechnique);
	}
	else
		return;
}

DARKSDK_DLL void SetEffectLODTechnique	( int iEffectID, SDK_LPSTR pTechniqueName )
{
	/* experimental idea
	// check the effect exists
	if ( !ConfirmEffect ( iEffectID ) )
		return;

	// Get effect ptr
	LPD3DXEFFECT pEffectPtr = NULL;
	cSpecialEffect* pEffectObject = m_EffectList [ iEffectID ]->pEffectObj;
	if ( pEffectObject )
		if ( pEffectObject->m_pEffect )
			pEffectPtr = pEffectObject->m_pEffect;

	// Choose technique based on name
	if ( pEffectPtr && pTechniqueName )
	{
		if ( strcmp ( (LPSTR)pTechniqueName, "" )!=NULL )
		{
			// use this when object at LOD distance
			D3DXHANDLE hTechnique = pEffectPtr->GetTechniqueByName ( (LPSTR)pTechniqueName );
			pEffectObject->m_hLODTechnique = hTechnique;
		}
		else
		{
			// do not override effect based on object distance
			pEffectObject->m_hLODTechnique = NULL;
		}
	}
	else
		return;
	*/
}

DARKSDK_DLL int GetEffectExist ( int iEffectID )
{
	// check the effect exists
	if ( iEffectID < 1 || iEffectID > MAX_EFFECTS )
	{ 
		RunTimeError ( RUNTIMEERROR_B3DEFFECTNUMBERILLEGAL );
		return 0;
	}
	if ( m_EffectList [ iEffectID ] )
		return 1;
	else
		return 0;
}

DARKSDK int GetObjectPolygonCount ( int iObjectNumber )
{
	// total count
	int iPolygonTotal = 0;

	// check the object exists
	if ( !ConfirmObject ( iObjectNumber ) )
		return 0;

	// return object information
	sObject* pObject = g_ObjectList [ iObjectNumber ];
	if ( pObject )
	{
		if ( pObject->iMeshCount>0 )
		{
			for ( int iM=0; iM<pObject->iMeshCount; iM++ )
			{
				sMesh* pMesh = pObject->ppMeshList[iM];
				if ( pMesh )
				{
					iPolygonTotal = iPolygonTotal + pMesh->iDrawPrimitives;
				}
			}
		}
	}

	// return total
	return iPolygonTotal;
}

int GetObjectVertexCount ( int iObjectNumber )
{
	// total count
	int iVertexCountTotal = 0;

	// check the object exists
	if ( !ConfirmObject ( iObjectNumber ) )
		return 0;

	// return object information
	sObject* pObject = g_ObjectList [ iObjectNumber ];
	if ( pObject )
	{
		if ( pObject->iMeshCount>0 )
		{
			for ( int iM=0; iM<pObject->iMeshCount; iM++ )
			{
				sMesh* pMesh = pObject->ppMeshList[iM];
				if ( pMesh )
				{
					if ( (int)pMesh->dwVertexCount>iVertexCountTotal )
					{
						iVertexCountTotal = pMesh->dwVertexCount;
					}
				}
			}
		}
	}

	// return total
	return iVertexCountTotal;
}

// Built-in stencil object effects

DARKSDK_DLL void SetShadowOnEx ( int iID, int iMesh, int iRange, int iUseShader )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// if already created shadow, show it
	if ( pObject->bHideShadow==true )
	{
		// show shadow again
		pObject->bHideShadow=false;

		// range of shadow
		if ( iUseShader==1 ) 
		{
			if ( iRange != -1 )
			{
				for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
				{
					sFrame* pFrame = pObject->ppFrameList [ iFrame ];
					if ( pFrame )
						if ( pFrame->pShadowMesh )
						{
							pFrame->pShadowMesh->Collision.fRadius=(float)iRange;
							pFrame->pShadowMesh->Collision.fLargestRadius=(float)iRange;
						}
				}
			}
		}
	}
	else
	{
		// need to check if we support stencil buffer operations
		if(SupportsStencilBuffer())
		{
			// Delete old stencil effect (if any)
			m_ObjectManager.DeleteStencilEffect ( pObject );

			// Create new stencil effect
			if(pObject->bCastsAShadow==false)
			{
				// single mesh imported in or objects own meshes
				if ( iMesh>0 )
				{
					// first frame carries shadow
					if ( pObject->ppFrameList )
					{
						// get frame ptr
						sFrame* pFrame = pObject->ppFrameList [ 0 ];

						// single externally imported mesh
						sMesh* pImportedMesh = g_RawMeshList [ iMesh ];
						SAFE_DELETE ( pFrame->pShadowMesh );

						// create shadow mesh
						if ( CreateShadowMesh ( pImportedMesh, &pFrame->pShadowMesh, iUseShader ) )
						{
							// range of shadow
							if ( iUseShader==1 )
							{
								// now map shadow mesh so frames link to bones
								if ( pFrame->pShadowMesh->dwBoneCount > 0 )
								{
									InitOneMeshFramesToBones ( pFrame->pShadowMesh );
									MapOneMeshFramesToBones ( pFrame->pShadowMesh, pObject->pFrame );
								}

								// apply range value
								if ( iRange != -1 )
									pFrame->pShadowMesh->Collision.fLargestRadius=(float)iRange;
								else
									pFrame->pShadowMesh->Collision.fLargestRadius=1000.0f;
							}
							else
							{
								pFrame->pShadowMesh->Collision.fRadius=(float)iRange;
								pFrame->pShadowMesh->Collision.fLargestRadius=-1;
							}

							// add shadow mesh to drawrender list
							m_ObjectManager.AddObjectMeshToBuffers ( pFrame->pShadowMesh, pObject->bUsesItsOwnBuffers );
						}
					}
				}
				else
				{
					// leeadd - 210506 - u62 - if a shared-clone, can use its shadow mesh (replace with better flag)
					// sObject* pOriginalBeforeSharedClone = pObject->pSharedCloneFrom; - to complete..

					// apply shadow mesh to all object frames
					for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
					{
						// get frame ptr
						sFrame* pFrame = pObject->ppFrameList [ iFrame ];
						if ( pFrame )
						{
							if ( pFrame->pMesh )
							{
								// iMeshID can provide an alternate shadow mesh (reduced/preanimated/etc)

								// create shadow mesh
								if ( !CreateShadowFrame ( pFrame, iUseShader ) )
								{
									// leefix - 110405 - cannot create shadow, skip to next frame
									pFrame->pShadowMesh = NULL;
									continue;
								}

								// range of shadow
								if ( iUseShader==1 )
								{
									// now map shadow mesh so frames link to bones
									if ( pFrame->pShadowMesh->dwBoneCount > 0 )
									{
										InitOneMeshFramesToBones ( pFrame->pShadowMesh );
										MapOneMeshFramesToBones ( pFrame->pShadowMesh, pObject->pFrame );
									}

									// apply range value
									if ( iRange != -1 )
										pFrame->pShadowMesh->Collision.fLargestRadius=(float)iRange;
									else
										pFrame->pShadowMesh->Collision.fLargestRadius=1000.0f;
								}
								else
								{
									pFrame->pShadowMesh->Collision.fRadius=(float)iRange;
									pFrame->pShadowMesh->Collision.fLargestRadius=-1;
								}

								// add shadow mesh to drawrender list
								m_ObjectManager.AddObjectMeshToBuffers ( pFrame->pShadowMesh, pObject->bUsesItsOwnBuffers );
							}
						}
					}
				}

				// object to cast a shadow
				pObject->bCastsAShadow=true;
				pObject->bHideShadow=false;

				// increase largest range to encompass possible shadow
				if ( pObject->collision.fScaledLargestRadius > 0.0f )
				{
					pObject->collision.fScaledLargestRadius = pObject->collision.fLargestRadius;
					pObject->collision.fScaledLargestRadius += 3000.0f;
				}

				// increment shadow object use
				g_pGlob->dwStencilShadowCount++;
			}

			// Activate stencil effect
			g_pGlob->dwStencilMode=1;
			g_pGlob->dwRedrawCount=1;
		}

		// update any shadow meshes
		if ( iUseShader==0 )
		{
			if ( pObject->bCastsAShadow )
				if ( g_pGlob->dwStencilMode==1 )
					for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
						UpdateShadowFrame ( pObject->ppFrameList [ iFrame ], &pObject->position.matWorld, g_iShadowPositionFixed, g_vecShadowPosition );
		}

		// as maybe recreated mesh format, must regenerate buffer instance
		m_ObjectManager.RemoveObjectFromBuffers ( pObject );

		// if fails, shadow was too big
		if ( m_ObjectManager.AddObjectToBuffers ( pObject )==false )
		{
			// reverse stencil effect
			m_ObjectManager.DeleteStencilEffect ( pObject );
			m_ObjectManager.RemoveObjectFromBuffers ( pObject );
			m_ObjectManager.AddObjectToBuffers ( pObject );
		}
	}
}

DARKSDK_DLL void SetShadowOn ( int iID )
{
	// default shadow
	SetShadowOnEx ( iID, 0, -1, 0 );
}

DARKSDK_DLL void SetShadowOff ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// if have shadow
	if(pObject->bCastsAShadow==true)
	{
		// hide shadow
		pObject->bHideShadow=true;
	}
}

DARKSDK_DLL void SetShadowPosition ( int iMode, float fX, float fY, float fZ )
{
	// global variable storing a fixed shadow position
	g_iShadowPositionFixed = iMode;
	if ( iMode==-1 ) 
	{
		g_vecShadowPosition.x = fX;
		g_vecShadowPosition.y = fY;
		g_vecShadowPosition.z = fZ;
	}
}

DARKSDK_DLL void SetShadowClipping ( int iMode, float fMin, float fMax )
{
	// leeadd - 301008 - set globals to control global shadow clipping
	g_iShadowClipping = iMode;
	g_fShadowClippingMin = fMin;
	g_fShadowClippingMax = fMax;
}

DARKSDK_DLL void SetReflectionOn ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// need to check if we support stencil buffer operations
	if(SupportsStencilBuffer())
	{
		// Delete old stencil effect (if any)
		m_ObjectManager.DeleteStencilEffect ( pObject );

		// Activate stencil effect
		g_pGlob->dwStencilMode=2;
		g_pGlob->dwRedrawCount=2;

		// Create new stencil effect
		if(pObject->bReflectiveObject==false)
		{
			// object to become reflective
			pObject->bReflectiveObject=true;

			// ensure card handles clipping planes or reflection not used
			if(SupportsUserClippingPlane())
				pObject->bReflectiveClipping=true;
			else
				pObject->bReflectiveClipping=false;

			// increment reflective object use
			g_pGlob->dwStencilReflectionCount++;
		}
	}
}

DARKSDK_DLL void SetShadingOff ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// Delete old stencil effect (if any)
	m_ObjectManager.DeleteStencilEffect ( pObject );
}

DARKSDK_DLL void SetShadowColor ( int iRed, int iGreen, int iBlue, int iAlphaLevel )
{
	m_ObjectManager.m_StencilStates.dwShadowStrength = iAlphaLevel;
	m_ObjectManager.m_StencilStates.dwShadowColor = D3DCOLOR_ARGB(0, iRed, iGreen, iBlue);
}

DARKSDK_DLL void SetShadowShades ( int iShades )
{
	if ( iShades<1 ) iShades=1;
	if ( iShades>64 ) iShades=64;
	m_ObjectManager.m_StencilStates.dwShadowShades = iShades;
}

DARKSDK_DLL void SetReflectionColor ( int iRed, int iGreen, int iBlue, int iAlphaLevel )
{
	m_ObjectManager.m_StencilStates.dwReflectionColor = D3DCOLOR_ARGB(iAlphaLevel, iRed, iGreen, iBlue);
}


// Custom vertex shaders

DARKSDK_DLL void SetVertexShaderOn ( int iID, int iShader )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// validate shader
	if ( iShader < 0 || iShader > MAX_VERTEX_SHADERS )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERNUMBERILLEGAL);
		return;
	}
	if ( m_VertexShaders [ iShader ].pVertexDec==NULL )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERINVALID);
		return;
	}

	// shader to set
	LPDIRECT3DVERTEXSHADER9 pVertexShader = m_VertexShaders [ iShader ].pVertexShader;
	LPDIRECT3DVERTEXDECLARATION9 pVertexDec = m_VertexShaders [ iShader ].pVertexDec;

	// apply shader to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetCustomShader ( pObject->ppMeshList [ iMesh ], pVertexShader, pVertexDec, 1 );
}

DARKSDK_DLL void SetVertexShaderOff ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply shader off to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetNoShader ( pObject->ppMeshList [ iMesh ] );
}

DARKSDK_DLL void CloneMeshToNewFormat ( int iID, DWORD dwFVF, DWORD dwEraseBones )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// object ref
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject==NULL )
		return;

	// create new mesh list to store ALL new meshes (erase bones means lightmapper process)
	if ( dwEraseBones==1 )
	{
		DWORD dwTotalMaterialCount = 0;
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		{
			sMesh* pMesh = pObject->ppMeshList [ iMesh ];
			if ( pMesh )
				if ( pMesh->bUseMultiMaterial==true )
					dwTotalMaterialCount+=pMesh->dwMultiMaterialCount;
		}

		// convert object if found to use multimaterial heracy
		if ( dwTotalMaterialCount>0 )
		{
			// new mesh list and ref to connect old frame ptrs to new meshes
			sMesh** pTotalMeshListRef = new sMesh*[dwTotalMaterialCount];
			sMesh** pTotalMeshList = new sMesh*[dwTotalMaterialCount];

			// start off new frame list contents
			int iNewFrameCount = pObject->iFrameCount+dwTotalMaterialCount;
			sFrame** pTotalFrameList = new sFrame*[iNewFrameCount];
			memset ( pTotalFrameList, 0, sizeof(sFrame*)*iNewFrameCount );
			for ( int iFrameIndex=0; iFrameIndex<pObject->iFrameCount; iFrameIndex++ )
				pTotalFrameList [ iFrameIndex ] = pObject->ppFrameList [ iFrameIndex ];
			int iFrameCurrentIndex = pObject->iFrameCount;

			DWORD dwMaterialMeshIndex = 0;
			for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
			{
				sMesh* pMesh = pObject->ppMeshList [ iMesh ];
				if ( pMesh )
				{
					if ( pMesh->bUseMultiMaterial==true )
					{
						// for each material, create a new mesh
						DWORD dwMaterialCount = pMesh->dwMultiMaterialCount;
						sMultiMaterial* pMultiMaterial = pMesh->pMultiMaterial;
						for ( DWORD dwMaterialIndex=0; dwMaterialIndex<dwMaterialCount; dwMaterialIndex++ )
						{
							sMesh* pNewMesh = new sMesh();
							if ( pNewMesh )
							{
								// duplicate mesh
								D3DXMATRIX matWorld;
								D3DXMatrixIdentity ( &matWorld );
								MakeMeshFromOtherMesh ( true, pNewMesh, pMesh, &matWorld );

								// copy in correct texture
								if ( pNewMesh->pTextures==NULL )
								{
									pNewMesh->dwTextureCount = 2;
									pNewMesh->pTextures = new sTexture [ pNewMesh->dwTextureCount ];
									memset ( pNewMesh->pTextures, 0, sizeof(sTexture)*pNewMesh->dwTextureCount );
								}
								pNewMesh->pTextures [ 0 ].iImageID  = pMesh->pTextures [ 0 ].iImageID;
								pNewMesh->pTextures [ 0 ].pTexturesRef = pMultiMaterial [ dwMaterialIndex ].pTexturesRef;
								strcpy ( pNewMesh->pTextures [ 0 ].pName, pMultiMaterial [ dwMaterialIndex ].pName );
								pNewMesh->pTextures [ 0 ].dwBlendMode = D3DTOP_MODULATE;
								pNewMesh->pTextures [ 0 ].dwBlendArg1 = D3DTA_TEXTURE;
								pNewMesh->pTextures [ 0 ].dwBlendArg2 = D3DTA_DIFFUSE;
								pNewMesh->bUseMultiMaterial = false;
								pNewMesh->bUsesMaterial = false;
								// pMultiMaterial [ dwMaterialIndex ].mMaterial ignored

								// modify index data so mesh only points to revelant polygons
								DWORD dwPolyCount = pMultiMaterial [ dwMaterialIndex ].dwPolyCount;
								pNewMesh->dwIndexCount = dwPolyCount*3;
								pNewMesh->iDrawVertexCount = pMesh->iDrawVertexCount;
								pNewMesh->iDrawPrimitives  = dwPolyCount;
								memcpy ( pNewMesh->pIndices, pMesh->pIndices + pMultiMaterial [ dwMaterialIndex ].dwIndexStart, dwPolyCount*3*sizeof(WORD) );

								// add to new mesh list
								pTotalMeshList [ dwMaterialMeshIndex ] = pNewMesh;
								pTotalMeshListRef [ dwMaterialMeshIndex ] = pMesh;
								dwMaterialMeshIndex++;
							}
						}
					}
				}
			}

			// and replace old mesh list and references with new
			for ( int iMeshIndex=0; iMeshIndex<pObject->iMeshCount; iMeshIndex++ )
			{
				SAFE_DELETE(pObject->ppMeshList[iMeshIndex]);
			}
			SAFE_DELETE(pObject->ppMeshList);
			pObject->iMeshCount = dwTotalMaterialCount;
			pObject->ppMeshList = pTotalMeshList;

			// and update frame references
			for ( int iFrameIndex=0; iFrameIndex<pObject->iFrameCount; iFrameIndex++ )
			{
				sFrame* pFrame = pObject->ppFrameList[iFrameIndex];
				if ( pFrame )
				{
					sMesh* pMeshToReplace = pFrame->pMesh;
					sMesh* pMeshToReplaceWith = NULL;
					if ( pMeshToReplace )
					{
						for ( int iScanMatIndex=0; iScanMatIndex<(int)dwTotalMaterialCount; iScanMatIndex++ )
						{
							if ( pMeshToReplace==pTotalMeshListRef[iScanMatIndex] )
							{
								// found reference to OLD mesh
								pMeshToReplaceWith = pTotalMeshList[iScanMatIndex];
								pTotalMeshListRef[iScanMatIndex] = NULL;
								break;
							}
						}
					}
					pFrame->pMesh = pMeshToReplaceWith;
					if ( pMeshToReplaceWith )
					{
						// also tag 'additional' meshes onto the end as sybling frames
						sFrame* pThisFrame = pFrame;
						for ( int iScanMatIndex=0; iScanMatIndex<(int)dwTotalMaterialCount; iScanMatIndex++ )
						{
							if ( pTotalMeshListRef[iScanMatIndex]==pMeshToReplace )
							{
								while ( pThisFrame->pSibling ) pThisFrame = pThisFrame->pSibling;
								pThisFrame->pSibling = new sFrame();
								pThisFrame->pSibling->matOriginal = pFrame->matOriginal;
								pThisFrame->pSibling->matTransformed = pFrame->matTransformed;
								pThisFrame->pSibling->matCombined = pFrame->matCombined;
								strcpy ( pThisFrame->pSibling->szName, pFrame->szName );
								pThisFrame->pSibling->pMesh = pTotalMeshList[iScanMatIndex];
								// NOTE: Ensure this does not wipe critical REF needed if more meshes are looking for this hierarchy slot
								pTotalMeshListRef[iScanMatIndex] = NULL;
								pTotalFrameList [ iFrameCurrentIndex ] = pThisFrame->pSibling;
								iFrameCurrentIndex++;
								//break;
							}
						}
					}
				}
			}
			SAFE_DELETE ( pObject->ppFrameList );
			pObject->ppFrameList = pTotalFrameList;
			pObject->iFrameCount = iFrameCurrentIndex;

			// free usages
			SAFE_DELETE(pTotalMeshListRef);
		}
	}

	/*
	// Flexible vertex format bits
	#define D3DFVF_RESERVED0        0x001
	#define D3DFVF_POSITION_MASK    0x00E
	#define D3DFVF_XYZ              0x002
	#define D3DFVF_XYZRHW           0x004
	#define D3DFVF_XYZB1            0x006
	#define D3DFVF_XYZB2            0x008
	#define D3DFVF_XYZB3            0x00a
	#define D3DFVF_XYZB4            0x00c
	#define D3DFVF_XYZB5            0x00e

	#define D3DFVF_NORMAL           0x010
	#define D3DFVF_PSIZE            0x020
	#define D3DFVF_DIFFUSE          0x040
	#define D3DFVF_SPECULAR         0x080

	#define D3DFVF_TEXCOUNT_MASK    0xf00
	#define D3DFVF_TEXCOUNT_SHIFT   8
	#define D3DFVF_TEX0             0x000
	#define D3DFVF_TEX1             0x100
	#define D3DFVF_TEX2             0x200
	#define D3DFVF_TEX3             0x300
	#define D3DFVF_TEX4             0x400
	#define D3DFVF_TEX5             0x500
	#define D3DFVF_TEX6             0x600
	#define D3DFVF_TEX7             0x700
	#define D3DFVF_TEX8             0x800
	#define D3DFVF_LASTBETA_UBYTE4  0x1000

    // Typical
    model         = D3DFVF_XYZ + D3DFVF_NORMAL + D3DFVF_TEX1
					0x002 + 0x010 + 0x100 = 0x152 (274)
    model+diffuse = D3DFVF_XYZ + D3DFVF_NORMAL + D3DFVF_TEX1 + D3DFVF_DIFFUSE
					0x002 + 0x010 + 0x100 + 0x040 = 0x152 (338)
    model-normal  = D3DFVF_XYZ + D3DFVF_DIFFUSE + D3DFVF_TEX1
					0x002 + 0x040 + 0x100 = (332)
	*/

	// clone mesh to the specific format
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		ConvertToFVF ( pObject->ppMeshList [ iMesh ], dwFVF );

	// lee - 050914 - also remove any bone animation data as converted object cannot animate without correct FVF skinning
	if ( dwEraseBones==1 )
	{
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		{
			sMesh* pMesh = pObject->ppMeshList [ iMesh ];
			if ( pMesh->dwBoneCount > 0 )
			{
				// erase the bone data
				SAFE_DELETE_ARRAY ( pMesh->pBones );
				pMesh->dwBoneCount = 0;
			}
		}
	}

	// regenerate buffer instance
	m_ObjectManager.RemoveObjectFromBuffers ( pObject );
	m_ObjectManager.AddObjectToBuffers ( pObject );
}

DARKSDK void CloneMeshToNewFormat ( int iID, DWORD dwFVF )
{
	CloneMeshToNewFormat ( iID, dwFVF, 0 );
}

DARKSDK_DLL void SetVertexShaderStreamCount ( int iID, int iCount )
{
	// set the size of the stream buffer

	// check the id is valid
	if ( iID < 0 || iID > MAX_VERTEX_SHADERS )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERNUMBERILLEGAL);
		return;
	}

	// make sure the count is ok
	if ( iCount < 1 )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERCOUNTILLEGAL);
		return;
	}

	// store the count
	m_VertexShaders [ iID ].dwDecArrayCount = iCount;
	
	// allocate memory for the begining and end addons
	DWORD dwSize = iCount + 1;
	if ( ! ( m_VertexShaders [ iID ].pDecArray = new D3DVERTEXELEMENT9 [ dwSize ] ) )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERCANNOTCREATE);
		return;
	}

	// clear declaration data
	memset ( m_VertexShaders [ iID ].pDecArray, 0, sizeof(D3DVERTEXELEMENT9)*dwSize );

	// add the start and end values
	m_VertexShaders [ iID ].pDecArray [ iCount ].Stream = 0xFF;
	m_VertexShaders [ iID ].pDecArray [ iCount ].Offset  = 0;
	m_VertexShaders [ iID ].pDecArray [ iCount ].Type  = D3DDECLTYPE_UNUSED;
	m_VertexShaders [ iID ].pDecArray [ iCount ].Method  = 0;
	m_VertexShaders [ iID ].pDecArray [ iCount ].Usage = 0;
	m_VertexShaders [ iID ].pDecArray [ iCount ].UsageIndex = 0;
}

DARKSDK_DLL void SetVertexShaderStream ( int iID, int iPos, int iDataUsage, int iDataType )
{
	// check the id is valid
	if ( iID < 0 || iID > MAX_VERTEX_SHADERS )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERNUMBERILLEGAL);
		return;
	}

	// make sure the dec pos is ok
	if ( iPos < 0 || iPos > (int)m_VertexShaders [ iID ].dwDecArrayCount )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERSTREAMPOSINVALID);
		return;
	}

	/* usages
	typedef enum _D3DDECLUSAGE {
		D3DDECLUSAGE_POSITION = 0,
		D3DDECLUSAGE_BLENDWEIGHT = 1,
		D3DDECLUSAGE_BLENDINDICES = 2,
		D3DDECLUSAGE_NORMAL = 3,
		D3DDECLUSAGE_PSIZE = 4,
		D3DDECLUSAGE_TEXCOORD = 5,
		D3DDECLUSAGE_TANGENT = 6,
		D3DDECLUSAGE_BINORMAL = 7,
		D3DDECLUSAGE_TESSFACTOR = 8,
		D3DDECLUSAGE_POSITIONT = 9,
		D3DDECLUSAGE_COLOR = 10,
		D3DDECLUSAGE_FOG = 11,
		D3DDECLUSAGE_DEPTH = 12,
		D3DDECLUSAGE_SAMPLE = 13
	} D3DDECLUSAGE;
	*/

	/* type values
	typedef enum _D3DDECLTYPE
	{
		D3DDECLTYPE_FLOAT1    =  0,  // 1D float expanded to (value, 0., 0., 1.)
		D3DDECLTYPE_FLOAT2    =  1,  // 2D float expanded to (value, value, 0., 1.)
		D3DDECLTYPE_FLOAT3    =  2,  // 3D float expanded to (value, value, value, 1.)
		D3DDECLTYPE_FLOAT4    =  3,  // 4D float
		D3DDECLTYPE_D3DCOLOR  =  4,  // 4D packed unsigned bytes mapped to 0. to 1. range
									 // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
		D3DDECLTYPE_UBYTE4    =  5,  // 4D unsigned byte
		D3DDECLTYPE_SHORT2    =  6,  // 2D signed short expanded to (value, value, 0., 1.)
		D3DDECLTYPE_SHORT4    =  7,  // 4D signed short
		// The following types are valid only with vertex shaders >= 2.0
		D3DDECLTYPE_UBYTE4N   =  8,  // Each of 4 bytes is normalized by dividing to 255.0
		D3DDECLTYPE_SHORT2N   =  9,  // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
		D3DDECLTYPE_SHORT4N   = 10,  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
		D3DDECLTYPE_USHORT2N  = 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
		D3DDECLTYPE_USHORT4N  = 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
		D3DDECLTYPE_UDEC3     = 13,  // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
		D3DDECLTYPE_DEC3N     = 14,  // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
		D3DDECLTYPE_FLOAT16_2 = 15,  // Two 16-bit floating point values, expanded to (value, value, 0, 1)
		D3DDECLTYPE_FLOAT16_4 = 16,  // Four 16-bit floating point values
		D3DDECLTYPE_UNUSED    = 17,  // When the type field in a decl is unused.
	} D3DDECLTYPE;
	*/

	// check data usage 0-13
	if ( iDataUsage < 0 || iDataUsage > 13 )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERDATAINVALID);
		return;
	}

	DWORD dwOffset=0;
	int iIndex=0;
	while ( iIndex<(iPos-1) )
	{
		DWORD dwSize=0;
		switch ( m_VertexShaders [ iID ].pDecArray [ iIndex ].Type )
		{
			case D3DDECLTYPE_FLOAT1 :
			case D3DDECLTYPE_D3DCOLOR :
				dwSize=1;
				break;

			case D3DDECLTYPE_FLOAT2 :
			case D3DDECLTYPE_SHORT2 :
				dwSize=2;
				break;

			case D3DDECLTYPE_FLOAT3 :
				dwSize=3;
				break;

			case D3DDECLTYPE_FLOAT4 :
			case D3DDECLTYPE_UBYTE4 :
			case D3DDECLTYPE_SHORT4 :
				dwSize=4;
				break;
		}
		dwOffset+=dwSize*4;
		iIndex++;
	}

	// add the dec
	m_VertexShaders [ iID ].pDecArray [ iPos-1 ].Stream = 0;
	m_VertexShaders [ iID ].pDecArray [ iPos-1 ].Offset = dwOffset;
	m_VertexShaders [ iID ].pDecArray [ iPos-1 ].Type = iDataType;
	m_VertexShaders [ iID ].pDecArray [ iPos-1 ].Method = D3DDECLMETHOD_DEFAULT;
	m_VertexShaders [ iID ].pDecArray [ iPos-1 ].Usage = iDataUsage;
	m_VertexShaders [ iID ].pDecArray [ iPos-1 ].UsageIndex = 0;
}

DARKSDK_DLL void CreateVertexShaderFromFile ( int iID, SDK_LPSTR szFile )
{
	// check the id is valid
	if ( iID < 0 || iID > MAX_VERTEX_SHADERS )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERNUMBERILLEGAL);
		return;
	}

	// free any previous shaders
	SAFE_RELEASE ( m_VertexShaders [ iID ].pVertexShader );
	SAFE_RELEASE ( m_VertexShaders [ iID ].pVertexDec );

	// compile and create shader
	LPD3DXBUFFER pCode;
	LPD3DXBUFFER pErrorMsg;
	if ( FAILED ( D3DXAssembleShaderFromFile ( (LPSTR)szFile, NULL, NULL, 0, &pCode, &pErrorMsg ) ) )
	{
		//LPSTR pSee = (LPSTR)pErrorMsg->GetBufferPointer();
		//MessageBox ( NULL, pSee, "DirectX Error", MB_OK );
		//pErrorMsg->Release();
		RunTimeError(RUNTIMEERROR_B3DVSHADERCANNOTASSEMBLE);
		return;
	}
	if ( FAILED ( m_pD3D->CreateVertexShader ( (DWORD*)pCode->GetBufferPointer ( ), &m_VertexShaders [ iID ].pVertexShader ) ) )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERCANNOTCREATE);
		return;
	}

	// free shader buffer
	pCode->Release ( );

	// create vertex declaration object
	if ( FAILED ( m_pD3D->CreateVertexDeclaration ( m_VertexShaders [ iID ].pDecArray, &m_VertexShaders [ iID ].pVertexDec ) ) )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERCANNOTCREATE);
		return;
	}
}

DARKSDK_DLL void SetVertexShaderVector ( int iID, DWORD dwRegister, int iVector, DWORD dwConstantCount )
{
	// vertify shader valid
	if ( iID < 0 || iID > MAX_VERTEX_SHADERS )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERNUMBERILLEGAL);
		return;
	}
	
	// set constant
	D3DXVECTOR4 vecData = g_Types_GetVector ( iVector );
	m_pD3D->SetVertexShaderConstantF ( dwRegister, (float*)&vecData, 1 );
}

DARKSDK_DLL void SetVertexShaderMatrix ( int iID, DWORD dwRegister, int iMatrix, DWORD dwConstantCount )
{
	// vertify shader valid
	if ( iID < 0 || iID > MAX_VERTEX_SHADERS )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERNUMBERILLEGAL);
		return;
	}

	// set constant
	D3DXMATRIX matData = g_Types_GetMatrix ( iMatrix );
	m_pD3D->SetVertexShaderConstantF ( dwRegister, (float*)&matData, 4 );
}

DARKSDK_DLL void DeleteVertexShader ( int iShader )
{
	// vertify shader valid
	if ( iShader < 0 || iShader > MAX_VERTEX_SHADERS )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERNUMBERILLEGAL);
		return;
	}

	// free vertex dec array
	SAFE_DELETE_ARRAY ( m_VertexShaders [ iShader ].pDecArray );

	// free vertex shader objects
	if ( m_VertexShaders [ iShader ].pVertexShader )
	{
		m_VertexShaders [ iShader ].pVertexShader->Release();
		m_VertexShaders [ iShader ].pVertexShader=NULL;
	}
	if ( m_VertexShaders [ iShader ].pVertexDec )
	{
		m_VertexShaders [ iShader ].pVertexDec->Release();
		m_VertexShaders [ iShader ].pVertexDec=NULL;
	}
}

DARKSDK_DLL void SetPixelShaderOn ( int iID, int iShader )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// verify shader
	if ( iShader < 0 || iShader > MAX_VERTEX_SHADERS )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERNUMBERILLEGAL);
		return;
	}

	// apply shader to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetCustomPixelShader ( pObject->ppMeshList [ iMesh ], m_PixelShaders [ iShader ].pPixelShader );
}

DARKSDK_DLL void SetPixelShaderOff ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// apply shader off to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetNoPixelShader ( pObject->ppMeshList [ iMesh ] );
}

DARKSDK_DLL void SetPixelShaderTexture ( int iShaderObject, int iSlot, int iTexture )
{
	// check the object exists
	if ( !ConfirmObject ( iShaderObject ) )
		return;

	// apply shader to all meshes
	sObject* pObject = g_ObjectList [ iShaderObject ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		SetMultiTexture ( pObject->ppMeshList [ iMesh ], iSlot, D3DTOP_MODULATE, 0, iTexture );

	// trigger a ew-new and re-sort
	m_ObjectManager.RenewReplacedMeshes ( pObject );
	m_ObjectManager.UpdateTextures ( );
}

DARKSDK_DLL void CreatePixelShaderFromFile ( int iID, SDK_LPSTR szFile )
{
	// check the id is valid
	if ( iID < 0 || iID > MAX_VERTEX_SHADERS )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERNUMBERILLEGAL);
		return;
	}

	// free any previous shaders
	SAFE_RELEASE ( m_PixelShaders [ iID ].pPixelShader );

	// compile and create shader
	LPD3DXBUFFER pCode;
	if ( FAILED ( D3DXAssembleShaderFromFile ( (LPSTR)szFile, 0, NULL, 0, &pCode, NULL ) ) )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERCANNOTASSEMBLE);
		return;
	}
	if ( FAILED ( m_pD3D->CreatePixelShader ( (DWORD*)pCode->GetBufferPointer(), &m_PixelShaders [ iID ].pPixelShader ) ) )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERCANNOTCREATE);
		return;
	}

	// free buffer
	pCode->Release ( );
}

DARKSDK_DLL void DeletePixelShader ( int iShader )
{
	// vertify shader valid
	if ( iShader < 0 || iShader > MAX_VERTEX_SHADERS )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERNUMBERILLEGAL);
		return;
	}

	// delete pixel shader
	if ( m_PixelShaders [ iShader ].pPixelShader )
	{
		m_PixelShaders [ iShader ].pPixelShader->Release();
		m_PixelShaders [ iShader ].pPixelShader=NULL;
	}
}

// Static Object Commands (NODETREE)

DARKSDK_DLL void CreateNodeTree ( float fX, float fY, float fZ )
{
	// Ensure old nodetree is freed
	DeleteNodeTree();

	// Create new universe
	if ( g_pUniverse==NULL )
	{
		// new universe data structure
		g_pUniverse = new cUniverse;
		g_iAreaBoxCount = 0;
		g_iAreaBox = 0;

		// NEW - sets the size of the first node - nothing can exist outside of this node
		if ( g_pUniverse )
		{
			g_pUniverse->Make ( fX, fY, fZ );
		}
	}
}

DARKSDK_DLL void AddNodeTreeObject ( int iID, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker )
{
	// make static object from object
	if ( g_pUniverse )
		g_pUniverse->Add ( iID, iType, iArbitaryValue, iCastShadow, iPortalBlocker );
}

DARKSDK_DLL void AddNodeTreeLimb ( int iID, int iLimb, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker )
{
	// make static object from limb
	if ( g_pUniverse )
		g_pUniverse->Add ( iID, iLimb, iType, iArbitaryValue, iCastShadow, iPortalBlocker );
}

DARKSDK_DLL void RemoveNodeTreeObject ( int iID )
{
	// remove static object
	if ( g_pUniverse )
		g_pUniverse->Remove ( iID );
}

DARKSDK_DLL void DeleteNodeTree ( void )
{
	// delete all texture lists, meshes and nodes of the universe
	if ( g_pUniverse )
	{
		delete g_pUniverse;
		g_pUniverse=NULL;
	}

	// delete static collision boxes
	FreeStaticColBoxes();
}

DARKSDK_DLL void AttachObjectToNodeTree ( int iID )
{
	// check the object exists
//	if ( !ConfirmObject ( iID ) ) // 190506 - u61 - can use instanced objects
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// add object to vis linked list
	if ( g_pUniverse )
	{
		sObject* pObject = g_ObjectList [ iID ];
		g_pUniverse->Attach ( pObject );
	}
}

DARKSDK_DLL void DetachObjectFromNodeTree ( int iID )
{
	// check the object exists
//	if ( !ConfirmObject ( iID ) ) // 190506 - u61 - can use instanced objects
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// remove object from universe
	if ( g_pUniverse )
	{
		sObject* pObject = g_ObjectList [ iID ];
		g_pUniverse->Detach ( pObject );
	}
}

DARKSDK_DLL void SetNodeTreePortalsOn ( void )
{
	if ( !g_pUniverse )
		return;

	g_pUniverse->SetPortalsOn ( );
}

DARKSDK_DLL void SetNodeTreeCulling ( int iFlag )
{
	if ( !g_pUniverse )
		return;

	g_pUniverse->m_iFullPortalRecurse = iFlag;
}

DARKSDK_DLL void SetNodeTreePortalsOff ( void )
{
	if ( !g_pUniverse )
		return;

	g_pUniverse->SetPortalsOff ( );
}

DARKSDK_DLL void BuildNodeTreePortals ( void )
{
	if ( !g_pUniverse )
		return;

	g_pUniverse->BuildPortals ( );
}

DARKSDK_DLL void MakeNodeTreeCollisionBox ( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2 )
{
	// make one static collision box
	SetStaticColBox ( fX1, fY1, fZ1, fX2, fY2, fZ2 );
}

DARKSDK_DLL void SetNodeTreeWireframeOn ( void )
{
	// debug on to see wireframe and last ray cast
	if ( g_pUniverse )
	{
		g_pUniverse->SetDebugOn ( );
	}
}

DARKSDK_DLL void SetNodeTreeWireframeOff ( void )
{
	// debug off
	if ( g_pUniverse )
	{
		g_pUniverse->SetDebugOff ( );
	}
}

DARKSDK_DLL void SetNodeTreeScorchTexture ( int iImageID, int iWidth, int iHeight )
{
	if ( g_pUniverse )
	{
		g_pUniverse->SetScorchTexture ( iImageID, iWidth, iHeight );
	}
}

DARKSDK_DLL void AddNodeTreeScorch ( float fSize, int iType )
{
	if ( g_pUniverse )
	{
		g_pUniverse->AddScorch ( fSize, iType );
	}
}

DARKSDK_DLL void AddNodeTreeLight ( int iLightIndex, float fX, float fY, float fZ, float fRange )
{
	if ( g_pUniverse )
	{
		g_pUniverse->SetLight ( iLightIndex, fX, fY, fZ, fRange, true );
	}
}

DARKSDK_DLL void SetNodeTreeTextureMode ( int iMode ){}
DARKSDK_DLL void EnableNodeTreeOcclusion ( void ){}
DARKSDK_DLL void DisableNodeTreeOcclusion ( void ){}

DARKSDK_DLL void LoadNodeTreeObjects ( SDK_LPSTR pFilename, int iDivideTextureSize )
{
	// free old universe
	if ( g_pUniverse )
	{
		delete g_pUniverse;
		g_pUniverse=NULL;
	}
	FreeStaticColBoxes();

	// create new universe
	g_pUniverse = new cUniverse;
	if ( g_pUniverse ) g_pUniverse->Load((LPSTR)pFilename,iDivideTextureSize);
}

DARKSDK_DLL void SaveNodeTreeObjects ( SDK_LPSTR pFilename )
{
	if ( g_pUniverse ) g_pUniverse->Save((LPSTR)pFilename);
}

DARKSDK_DLL void SetNodeTreeEffectTechnique	( SDK_LPSTR pTechniqueName )
{
	if ( g_pUniverse ) g_pUniverse->SetEffectTechnique ( (LPSTR)pTechniqueName );
}

// CSG Commands (CSG)

DARKSDK_DLL void PeformObjectCSG ( int iObjectA, int iObjectB, int iMode )
{
	// csg work
	cCSG csg;

	// check the objects exist
	if ( !ConfirmObject ( iObjectA ) ) return;
	if ( !ConfirmObject ( iObjectB ) ) return;

	// update world matrices
	sObject* pObjectA = g_ObjectList [ iObjectA ];
	sObject* pObjectB = g_ObjectList [ iObjectB ];
	CalcObjectWorld ( pObjectA );
	CalcObjectWorld ( pObjectB );

	// apply to all meshes
	for ( int iLimbA = 0; iLimbA < pObjectA->iMeshCount; iLimbA++ )
	{
		// target object will be modified
		sMesh* pMeshA = pObjectA->ppMeshList [ iLimbA ];
		sFrame* pFrame = pObjectA->ppFrameList [ iLimbA ];
		CalculateAbsoluteWorldMatrix ( pObjectA, pFrame, pMeshA );
		D3DXMATRIX matMeshA = pFrame->matAbsoluteWorld;

		// apply to all meshes
		for ( int iMesh = 0; iMesh < pObjectB->iMeshCount; iMesh++ )
		{
			// get punch info
			sMesh* pMeshB = pObjectB->ppMeshList [ iMesh ];
			sFrame* pFrameB = pObjectB->ppFrameList [ iMesh ];

			// only if punch visible
			if ( pMeshB->bVisible )
			{
				CalculateAbsoluteWorldMatrix ( pObjectB, pFrameB, pMeshB );
				D3DXMATRIX matMeshB = pFrameB->matAbsoluteWorld;
				switch ( iMode )
				{
					case 1 : csg.Union ( pMeshA, matMeshA, pMeshB, matMeshB );			break;
					case 2 : csg.Difference ( pMeshA, matMeshA, pMeshB, matMeshB );		break;
					case 3 : csg.Intersection ( pMeshA, matMeshA, pMeshB, matMeshB );	break;
					case 4 : csg.Clip ( pMeshA, matMeshA, pMeshB, matMeshB );	break;
				}

				// ensure old data of pMeshA is wiped
				SAFE_DELETE ( pMeshA->pOriginalVertexData );
			}
		}
	}
	
	// update mesh(es) of object
	m_ObjectManager.RefreshObjectInBuffer ( pObjectA );
}

DARKSDK_DLL void PeformCSGUnion ( int iObjectA, int iObjectB )
{
	// now call the union function
	PeformObjectCSG ( iObjectA, iObjectB, 1 );
}

DARKSDK_DLL void PeformCSGDifference ( int iObjectA, int iObjectB )
{
	// call the difference function
	PeformObjectCSG ( iObjectA, iObjectB, 2 );
}

DARKSDK_DLL void PeformCSGIntersection ( int iObjectA, int iObjectB )
{
	// call the intersection function
	PeformObjectCSG ( iObjectA, iObjectB, 3 );
}

DARKSDK_DLL void PeformCSGClip ( int iObjectA, int iObjectB )
{
	// call the intersection function
	PeformObjectCSG ( iObjectA, iObjectB, 4 );
}

// Collision Commands

DARKSDK_DLL void SetCollisionOn ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];

	// call collision command
	SetColOn ( pObject );
}

DARKSDK_DLL void SetCollisionOff ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];

	// call collision command
	SetColOff ( pObject );
}

DARKSDK_DLL void MakeCollisionBox ( int iID, float iX1, float iY1, float iZ1, float iX2, float iY2, float iZ2, int iRotatedBoxFlag )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];
	SetColBox( pObject, iX1, iY1, iZ1, iX2, iY2, iZ2, iRotatedBoxFlag );
}

DARKSDK_DLL void DeleteCollisionBox ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];
	FreeColBox ( pObject );
}

DARKSDK_DLL void SetCollisionToSpheres ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];
	SetColToSpheres ( pObject );
}

DARKSDK_DLL void SetCollisionToBoxes ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];
	SetColToBoxes ( pObject );
}

DARKSDK_DLL void SetCollisionToPolygons ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];
	SetColToPolygons ( pObject );
}

DARKSDK_DLL void SetSphereRadius ( int iID, float fRadius )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iID ];
	SetSphereRadius ( pObject, fRadius );
}

DARKSDK_DLL void SetGlobalCollisionOn ( void )
{
	GlobalColOn();
}

DARKSDK_DLL void SetGlobalCollisionOff ( void )
{
	GlobalColOff();
}

DARKSDK_DLL float IntersectObjectCore ( sObject* pObject, float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ )
{
	// object must have its world data calculated
	CalcObjectWorld ( pObject );

	// do intersect check
	return CheckIntersectObject ( pObject, fX, fY, fZ, fNewX, fNewY, fNewZ );
}

DARKSDK_DLL SDK_FLOAT IntersectObject ( int iObjectID, float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iObjectID ) )
		return 0;

	// get object pointer
	sObject* pObject = g_ObjectList [ iObjectID ];

	// do intersect check
	float fDistance=IntersectObjectCore ( pObject, fX, fY, fZ, fNewX, fNewY, fNewZ );

	// and must be within distance of specified vectors
	float fDistanceBetweenPoints;
	D3DXVECTOR3 vec3value = D3DXVECTOR3(fX,fY,fZ) - D3DXVECTOR3(fNewX,fNewY,fNewZ);
	fDistanceBetweenPoints = D3DXVec3Length(&vec3value);
	if ( fDistance > fDistanceBetweenPoints ) fDistance=0.0f;
	return SDK_RETFLOAT(fDistance);
}

struct IntersectRay
{
	float direction[3];
	float origin[3];
};
struct IntersectBox
{
	float min[3];
	float max[3];
};

bool intersectRayAABox2(const IntersectRay &ray, const IntersectBox &box, int& tnear, int& tfar)
{
    D3DXVECTOR3 T_1, T_2; // vectors to hold the T-values for every direction
    double t_near = -DBL_MAX; // maximums defined in float.h
    double t_far = DBL_MAX;

    for (int i = 0; i < 3; i++)
	{ 
		//we test slabs in every direction
        if (ray.direction[i] == 0)
		{ 
			// ray parallel to planes in this direction
            if ((ray.origin[i] < box.min[i]) || (ray.origin[i] > box.max[i])) 
			{
                return false; // parallel AND outside box : no intersection possible
            }
        } 
		else 
		{ 
			// ray not parallel to planes in this direction
            T_1[i] = (box.min[i] - ray.origin[i]) / ray.direction[i];
            T_2[i] = (box.max[i] - ray.origin[i]) / ray.direction[i];

            if(T_1[i] > T_2[i])
			{ 
				// we want T_1 to hold values for intersection with near plane
                swap(T_1,T_2);
            }
            if (T_1[i] > t_near)
			{
                t_near = T_1[i];
            }
            if (T_2[i] < t_far)
			{
                t_far = T_2[i];
            }
            if( (t_near > t_far) || (t_far < 0) )
			{
                return false;
            }
        }
    }
    tnear = t_near; tfar = t_far; // put return values in place
    return true; // if we made it here, there was an intersection - YAY
}

DARKSDK_DLL int IntersectAll ( int iPrimaryStart, int iPrimaryEnd, float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ, int iIgnoreObjNo )
{
	// special iIgnoreObjNo mode
	if ( iIgnoreObjNo==-123 || iIgnoreObjNo==-124 )
	{
		if ( iIgnoreObjNo==-123 )
		{
			// obtain second range of objects to check
			g_iIntersectAllSecondStart = iPrimaryStart;
			g_iIntersectAllSecondEnd = iPrimaryEnd;
			#ifdef SKIPGRIDUSED
			g_fIntersectAllSkipGridX = fX;
			g_fIntersectAllSkipGridZ = fZ;
			#endif
			return 0;
		}
		else
		{
			// obtain third range of objects to check
			g_iIntersectAllThirdStart = iPrimaryStart;
			g_iIntersectAllThirdEnd = iPrimaryEnd;
			#ifdef SKIPGRIDUSED
			g_fIntersectAllSkipGridX = fX;
			g_fIntersectAllSkipGridZ = fZ;
			#endif
			return 0;
		}
	}
	else
	{
		// detect if the ray is recorded in skipgrid as blocked
		#ifdef SKIPGRIDUSED
		if ( g_fIntersectAllSkipGridX>0 )
		{
			int iSkipGridRefFromX = fX/50.0f;
			int iSkipGridRefFromZ = fZ/50.0f;
			if ( iSkipGridRefFromX < 0 ) iSkipGridRefFromX=0;
			if ( iSkipGridRefFromX > 1023 ) iSkipGridRefFromX=1023;
			if ( iSkipGridRefFromZ < 0 ) iSkipGridRefFromZ=0;
			if ( iSkipGridRefFromZ > 1023 ) iSkipGridRefFromZ=1023;
			WORD wTargetX = fNewX/10.0f;
			WORD wTargetZ = fNewZ/10.0f;
			DWORD dwSkipGridValue = g_dwSkipGrid[iSkipGridRefFromX][iSkipGridRefFromZ];
			if ( dwSkipGridValue > 0 )
			{
				WORD wRefX = (dwSkipGridValue & 0xFFFF0000) >> 16;
				WORD wRefZ = (dwSkipGridValue & 0x0000FFFF);
				if ( wRefX==wTargetX && wRefZ==wTargetZ )
				{
					// this target was found to be blocked from this coordinate
					return g_iSkipGridResult[iSkipGridRefFromX][iSkipGridRefFromZ];
				}
			}
		}
		#endif
	}

	// return value (0=no hit, >0=object number, -1=other geometry)
	int iHitValue = 0;

	// work out length of ray
	D3DXVECTOR3 vec3value = D3DXVECTOR3(fX,fY,fZ) - D3DXVECTOR3(fNewX,fNewY,fNewZ);
	float fDistanceBetweenPoints = D3DXVec3Length(&vec3value);

	// LEE: Dave, now you commented back in the old code, this entire routine is redundant, nes pa?
	// go through all objects and collect a shortlist of boxes intersected by ray
	g_pIntersectShortList.clear();
	for ( int iPass=0; iPass<3; iPass++ )
	{
		int iStart, iEnd;
		if ( iPass==0 ) { iStart=iPrimaryStart; iEnd=iPrimaryEnd; }
		if ( iPass==1 ) { iStart=g_iIntersectAllSecondStart; iEnd=g_iIntersectAllSecondEnd; }
		if ( iPass==2 ) { iStart=g_iIntersectAllThirdStart; iEnd=g_iIntersectAllThirdEnd; if ( iStart == 0 ) break; }
		for ( int iObjectID = iStart; iObjectID <= iEnd; iObjectID++ )
		{
			// make sure we have a valid object
			sObject* pObject = g_ObjectList [ iObjectID ];
			if ( !pObject ) 
				continue;

			// check if object is excluded
			// check if object in dead state (non collisin detectable)
			if ( pObject->dwObjectNumber==iIgnoreObjNo || pObject->collision.dwCollisionPropertyValue==1 || !pObject->bVisible )
				continue;

			// check if object in same 'region' as ray
			float fDX=0, fDY=0, fDZ=0;
			if ( pObject->position.iGluedToObj>0 )
			{
				// use parent object instead
				sObject* pParentObject = g_ObjectList [ pObject->position.iGluedToObj ];
				if ( pParentObject )
				{
					fDX = pParentObject->position.vecPosition.x - fX;
					fDY = pParentObject->position.vecPosition.y - fY;
					fDZ = pParentObject->position.vecPosition.z - fZ;
				}
			}
			else
			{
				fDX = pObject->position.vecPosition.x - fX;
				fDY = pObject->position.vecPosition.y - fY;
				fDZ = pObject->position.vecPosition.z - fZ;
			}
			float fDist = sqrt(fabs(fDX*fDX)+fabs(fDY*fDY)+fabs(fDZ*fDZ));
			if ( fDist <= ((pObject->collision.fLargestRadius*3)+fDistanceBetweenPoints) )
			{
				// instead of transforming box to object world orientation, transform ray
				// on a per object basis back into object space, for quicker box checking
				D3DXMATRIX matInvWorld;
				float fDet;
				D3DXMatrixInverse(&matInvWorld,&fDet,&pObject->position.matWorld);
				D3DXVECTOR3 vecFrom = D3DXVECTOR3(fX,fY,fZ);
				D3DXVECTOR3 vecTo = D3DXVECTOR3(fNewX,fNewY,fNewZ);
				D3DXVec3TransformCoord(&vecFrom,&vecFrom,&matInvWorld);
				D3DXVec3TransformCoord(&vecTo,&vecTo,&matInvWorld);
				IntersectRay transformedray;
				transformedray.origin[0] = vecFrom.x;
				transformedray.origin[1] = vecFrom.y;
				transformedray.origin[2] = vecFrom.z;
				transformedray.direction[0] = vecTo.x-vecFrom.x;
				transformedray.direction[1] = vecTo.y-vecFrom.y;
				transformedray.direction[2] = vecTo.z-vecFrom.z;
				transformedray.direction[0] /= fDistanceBetweenPoints;
				transformedray.direction[1] /= fDistanceBetweenPoints;
				transformedray.direction[2] /= fDistanceBetweenPoints;

				// check if ray intersects object bound box (ray vs box) [using object space]
				IntersectBox box;
				box.min[0] = pObject->collision.vecMin.x;
				box.min[1] = pObject->collision.vecMin.y;
				box.min[2] = pObject->collision.vecMin.z;
				box.max[0] = pObject->collision.vecMax.x;
				box.max[1] = pObject->collision.vecMax.y;
				box.max[2] = pObject->collision.vecMax.z;
				int tnear, tfar;
				if ( intersectRayAABox2(transformedray, box, tnear, tfar)==true )
				{
					g_pIntersectShortList.push_back ( pObject );
				}
			}
		}
	}

	// for shortlist of object(boxes) that touch ray, check closest with full polygon check
	float fBestDistance = 999999.9f;
	GlobChecklistStruct pBestHit[10];
	std::sort(g_pIntersectShortList.begin(), g_pIntersectShortList.end(), OrderByCamDistance() );
	int iIntersectShortListMax = g_pIntersectShortList.size ( );
	// DAVE 9/12/2014 Commented this out as sometimes the closer object isnt the one that gets hit by the ray first
	// E.G. in the middle of a building which counts as closer but the player who is inside the building would be the one getting hit as he is infront of the wall that will get tested first
	// Since the building counts as being closer, the test is a success and the player is never checked despite the polygon that would be hit of the player being closer
	/*for ( int iShortListIndex=0; iShortListIndex<iIntersectShortListMax; iShortListIndex++ )
	{
		sObject* pObject = g_pIntersectShortList [ iShortListIndex ];
		int iObjectID = pObject->dwObjectNumber;
		float fDistance = IntersectObjectCore ( pObject, fX, fY, fZ, fNewX, fNewY, fNewZ );
		if ( fDistance > 0 && fDistance < fBestDistance && fDistance < fDistanceBetweenPoints )
		{
			// object was intersected, return obj number
			fBestDistance = fDistance;
			iHitValue = iObjectID;

			// populate checklist with extra hit info
			// 0 - frame indexes (A = mesh number, B = related frame number (if A is bone mesh))
			// 1 - vertex indexes
			// 2 - object-space coordinate of V0
			// 3 - object-space coordinate of V1
			// 4 - object-space coordinate of V2
			// 5 - world space coordinate where the collision struck!
			// 6 - normal of polygon struck (from vertA)
			// 7 - reflection normal based on angle of impact
			// 8 - arbitary value from mesh collision structure (set using SetObjectArbitary)
			for ( int iI=0; iI<9; iI++ ) pBestHit [ iI ] = g_pGlob->checklist [ iI ];

			// first polygon hit wins (unless it has no camdistance such as consolidated LM objects)
			if ( pObject->position.fCamDistance > 0.0f )
			{
				break;
			}
		}
	}*/

	
	// DAVE 9122014 This block was commented out, but put back in so it checks every object in the list
	// go through all objects presently in scene
	for ( int iPass=0; iPass<3; iPass++ )
	{
		int iStart, iEnd;
		if ( iPass==0 ) { iStart=iPrimaryStart; iEnd=iPrimaryEnd; }
		if ( iPass==1 ) { iStart=g_iIntersectAllSecondStart; iEnd=g_iIntersectAllSecondEnd; }
		if ( iPass==2 ) { iStart=g_iIntersectAllThirdStart; iEnd=g_iIntersectAllThirdEnd; if ( iStart == 0 ) break; }
		for ( int iObjectID = iStart; iObjectID <= iEnd; iObjectID++ )
		{
			// make sure we have a valid object
			sObject* pObject = g_ObjectList [ iObjectID ];
			if ( pObject )
			{
				// check if object is excluded
				// check if object in dead state (non collisin detectable)
				//Dave added a skip for hidden objects
				if ( pObject->dwObjectNumber==iIgnoreObjNo || pObject->collision.dwCollisionPropertyValue==1 || !pObject->bVisible )
				{
					// ignore this object (usually the caster)
				}
				else
				{
					// check if object in same 'region' as ray
					float fDX=0, fDY=0, fDZ=0;
					if ( pObject->position.iGluedToObj>0 )
					{
						// use parent object instead
						sObject* pParentObject = g_ObjectList [ pObject->position.iGluedToObj ];
						if ( pParentObject )
						{
							fDX = pParentObject->position.vecPosition.x - fX;
							fDY = pParentObject->position.vecPosition.y - fY;
							fDZ = pParentObject->position.vecPosition.z - fZ;
						}
					}
					else
					{
						fDX = pObject->position.vecPosition.x - fX;
						fDY = pObject->position.vecPosition.y - fY;
						fDZ = pObject->position.vecPosition.z - fZ;
					}
					float fDist = sqrt(fabs(fDX*fDX)+fabs(fDY*fDY)+fabs(fDZ*fDZ));
					if ( fDist <= ((pObject->collision.fLargestRadius*3)+fDistanceBetweenPoints) )
					{
						// check if ray intersects object bound box (ray vs box)
						IntersectBox box;
						box.min[0] = pObject->position.vecPosition.x + pObject->collision.vecMin.x;
						box.min[1] = pObject->position.vecPosition.y + pObject->collision.vecMin.y;
						box.min[2] = pObject->position.vecPosition.z + pObject->collision.vecMin.z;
						box.max[0] = pObject->position.vecPosition.x + pObject->collision.vecMax.x;
						box.max[1] = pObject->position.vecPosition.y + pObject->collision.vecMax.y;
						box.max[2] = pObject->position.vecPosition.z + pObject->collision.vecMax.z;
						if ( true )//intersectRayAABox2(ray, box, tnear, tfar)==true )
						{
							// do intersect check
							float fDistance = IntersectObjectCore ( pObject, fX, fY, fZ, fNewX, fNewY, fNewZ );
							if ( fDistance > 0 && fDistance < fBestDistance )
							{
								// and must be within distance of specified vectors
								if ( fDistance <= fDistanceBetweenPoints )
								{
									// only if object in detection range
									int iObjectHit = pObject->dwObjectNumber;
									if ( iObjectHit >= iStart && iObjectHit <= iEnd )
									{
										// object was intersected, return obj number
										iHitValue = iObjectHit;

										// populate checklist with extra hit info
										// 0 - frame indexes (A = mesh number, B = related frame number (if A is bone mesh))
										// 1 - vertex indexes
										// 2 - object-space coordinate of V0
										// 3 - object-space coordinate of V1
										// 4 - object-space coordinate of V2
										// 5 - world space coordinate where the collision struck!
										// 6 - normal of polygon struck (from vertA)
										// 7 - reflection normal based on angle of impact
										// 8 - normal vector?
										for ( int iI=0; iI<9; iI++ ) pBestHit [ iI ] = g_pGlob->checklist [ iI ];

										// find closest distance
										fBestDistance = fDistance;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	

	// if a hit was detected
	if ( iHitValue!=0 )
	{
		// copy best hit info back to checklist
		for ( int iI=0; iI<9; iI++ )
		{
			LPSTR pSaveStr = g_pGlob->checklist [ iI ].string;
			g_pGlob->checklist [ iI ] = pBestHit [ iI ];
			g_pGlob->checklist [ iI ].string = pSaveStr;
			if ( pSaveStr ) strcpy ( pSaveStr, "" );
		}
	}

	#ifdef SKIPGRIDUSED
	// also record in skipgrid (for optimized future collisions)
	int iSkipGridRefFromX = fX/50.0f;
	int iSkipGridRefFromZ = fZ/50.0f;
	if ( iSkipGridRefFromX < 0 ) iSkipGridRefFromX=0;
	if ( iSkipGridRefFromX > 1023 ) iSkipGridRefFromX=1023;
	if ( iSkipGridRefFromZ < 0 ) iSkipGridRefFromZ=0;
	if ( iSkipGridRefFromZ > 1023 ) iSkipGridRefFromZ=1023;
	WORD wTargetX = fNewX/10.0f;
	WORD wTargetZ = fNewZ/10.0f;
	DWORD dwSkipGridValue = (wTargetX<<16) + (wTargetZ);
	g_dwSkipGrid[iSkipGridRefFromX][iSkipGridRefFromZ] = dwSkipGridValue;
	g_iSkipGridResult[iSkipGridRefFromX][iSkipGridRefFromZ] = iHitValue;
	#endif

	// incase we dont want the third list next time
	g_iIntersectAllThirdStart = 0;

	// return hit value depending on what was hit
	return iHitValue;
}

DARKSDK void SetObjectCollisionProperty ( int iObjectID, int iPropertyValue )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iObjectID ) )
		return;

	// assign property value
	sObject* pObject = g_ObjectList [ iObjectID ];
	pObject->collision.dwCollisionPropertyValue = iPropertyValue;
}

// Cool Auto-Collision Commands

DARKSDK_DLL void AutomaticObjectCollision ( int iObjectID, float fRadius, int iResponse )
{
	// check the object exists
	if ( !ConfirmObject ( iObjectID ) )
		return;

	// get object pointer
	sObject* pObject = g_ObjectList [ iObjectID ];
	AutoObjectCol ( pObject, fRadius, iResponse );
}

DARKSDK_DLL void AutomaticCameraCollision ( int iCameraID, float fRadius, int iResponse, int iStandGroundMode )
{
	AutoCameraCol ( iCameraID, fRadius, iResponse, iStandGroundMode );
}

DARKSDK_DLL void AutomaticCameraCollision ( int iCameraID, float fRadius, int iResponse )
{
	AutoCameraCol ( iCameraID, fRadius, iResponse );
}

DARKSDK_DLL void ForceAutomaticEnd ( void )
{
	// leeadd - 080604 - required to find new camera/obj position before sync!
	AutomaticEnd ();
}

DARKSDK_DLL void HideBounds ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// set object bounds visibility
	sObject* pObject = g_ObjectList [ iID ];
	pObject->bDrawBounds = false;

	// mike 160505 - clear all mesh properties

	DWORD dwLimbMax = pObject->iFrameCount;
	for(DWORD iLimbID=0; iLimbID<dwLimbMax; iLimbID++)
	{
		sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
		if ( pFrame )
		{
			if ( pFrame->pMesh )
				pFrame->pMesh->bDrawBounds = false;
		}
	}

	
}

DARKSDK_DLL void ShowBoundsEx ( int iID, int iBoxOnly )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// create mesh bounds now
	sObject* pObject = g_ObjectList [ iID ];
	DWORD dwLimbMax = pObject->iFrameCount;
	for(DWORD iLimbID=0; iLimbID<dwLimbMax; iLimbID++)
	{
		sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
		if ( pFrame )
		{
			CreateBoundBoxMesh ( pFrame );
			if ( iBoxOnly==0 )
				CreateBoundSphereMesh ( pFrame );
			else
				SAFE_DELETE ( pFrame->pBoundSphere );
		}
	}

	// set object bounds visibility
	pObject->bDrawBounds = true;

	// make sure object is updated in buffer else it won't draw
	m_ObjectManager.RefreshObjectInBuffer ( pObject );
}

DARKSDK_DLL void ShowBounds ( int iID )
{
	ShowBoundsEx ( iID, 0 );
}

DARKSDK_DLL void ShowBounds ( int iID, int iLimb )
{
	// mike 160505 - new function for specifying a limb bound box drae

	// run through the normal process
	ShowBoundsEx ( iID, 1 );

	// now extra work
	sObject* pObject = g_ObjectList [ iID ];

	// check limb is valid
	if ( iLimb >= pObject->iFrameCount )
		return;

	// turn on draw bounds for this mesh
	if ( pObject->ppFrameList [ iLimb ]->pMesh )
		pObject->ppFrameList [ iLimb ]->pMesh->bDrawBounds = true;

	// switch off draw bounds for rest of object
	pObject->bDrawBounds = false;
}

//
// LIMB Commands
//

DARKSDK_DLL void PerformCheckListForLimbs ( int iID )
{
	// check the object limb exists
	if ( !ConfirmObject ( iID ) )
		return;

	// Generate Checklist
	DWORD dwMaxStringSizeInEnum=0;
	bool bCreateChecklistNow=false;
	g_pGlob->checklisthasvalues=true;
	g_pGlob->checklisthasstrings=true;

	for(int pass=0; pass<2; pass++)
	{
		if(pass==1)
		{
			// Ensure checklist is large enough
			bCreateChecklistNow=true;
			for(int c=0; c<g_pGlob->checklistqty; c++)
			{
				GlobExpandChecklist(c, dwMaxStringSizeInEnum);
			}
		}

		// Run through total list
		g_pGlob->checklistqty=0;
		DWORD dwLimbMax = g_ObjectList [ iID ]->iFrameCount;
		for(DWORD iLimbID=0; iLimbID<dwLimbMax; iLimbID++)
		{
			// Get limb name
			LPSTR pName = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->szName;

			// Add to checklist
			DWORD dwSize=0;
			if(pName) dwSize=strlen(pName);
			if(dwSize>dwMaxStringSizeInEnum) dwMaxStringSizeInEnum=dwSize;
			if(bCreateChecklistNow)
			{
				// New checklist item
				g_pGlob->checklist[g_pGlob->checklistqty].valuea=iLimbID;
				if(pName==NULL)
					strcpy(g_pGlob->checklist[g_pGlob->checklistqty].string, "");
				else
					strcpy(g_pGlob->checklist[g_pGlob->checklistqty].string, pName);

				// calculate parent and child id
				sFrame* pCurrent = g_ObjectList [ iID ]->ppFrameList [ iLimbID ];
				sFrame* pParent = pCurrent->pParent;
				sFrame* pSibling = pCurrent->pSibling;
				sFrame* pChild = pCurrent->pChild;
				int childid=-1, siblingid=-1, parentid=-1;
				if ( pParent ) parentid = pParent->iID;
				if ( pSibling ) siblingid = pSibling->iID;
				if ( pChild ) childid = pChild->iID;

				// record id of frame index
				g_pGlob->checklist[g_pGlob->checklistqty].valuea = iLimbID;

				// record id of parent
				g_pGlob->checklist[g_pGlob->checklistqty].valueb = parentid;

				// record if of sibling
				g_pGlob->checklist[g_pGlob->checklistqty].valuec = siblingid;

				// record if of child
				g_pGlob->checklist[g_pGlob->checklistqty].valued = childid;
			}
			g_pGlob->checklistqty++;
		}
	}
 
	// Determine if checklist has any contents
	if(g_pGlob->checklistqty>0)
		g_pGlob->checklistexists=true;
	else
		g_pGlob->checklistexists=false;
}

DARKSDK_DLL void PerformCheckListForOnscreenObjects ( int iMode )
{
	// 301007 - new command
	bool bCreateChecklistNow=false;
	g_pGlob->checklisthasvalues=true;
	g_pGlob->checklisthasstrings=false;
	for(int pass=0; pass<2; pass++)
	{
		// Ensure checklist is large enough
		if(pass==1)
		{
			bCreateChecklistNow=true;
			for(int c=0; c<g_pGlob->checklistqty; c++)
				GlobExpandChecklist(c, 256);
		}

		// Run through total list
		g_pGlob->checklistqty=0;
		int iVisibleObjectCount = m_ObjectManager.GetVisibleObjectCount();
		sObject** ppSortedObjectVisibleList = m_ObjectManager.GetSortedObjectVisibleList();
		for ( int iObject = 0; iObject < iVisibleObjectCount; iObject++ )
		{
			// the object to draw
			sObject* pObject = ppSortedObjectVisibleList [ iObject ];
			if ( pObject==NULL )
				continue;

			// Add to checklist
			if(bCreateChecklistNow)
			{
				// record id of frame index
				g_pGlob->checklist[g_pGlob->checklistqty].valuea = (int)pObject->dwObjectNumber;

				// reserved
				g_pGlob->checklist[g_pGlob->checklistqty].valueb = 0;
				g_pGlob->checklist[g_pGlob->checklistqty].valuec = 0;
				g_pGlob->checklist[g_pGlob->checklistqty].valued = 0;
			}
			g_pGlob->checklistqty++;
		}
	}
 
	// Determine if checklist has any contents
	if(g_pGlob->checklistqty>0)
		g_pGlob->checklistexists=true;
	else
		g_pGlob->checklistexists=false;
}

DARKSDK_DLL void HideLimb ( int iID, int iLimbID )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimbInstance ( iID, iLimbID ) )
		return;

	// actual object or instance of object
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject->pInstanceMeshVisible )
	{
		// record hide state in instance-mesh-visibility-array
		sObject* pActualObject = pObject->pInstanceOfObject;
		if ( iLimbID>=0 && iLimbID<pActualObject->iFrameCount)
			pObject->pInstanceMeshVisible [ iLimbID ] = false;
	}
	else
	{
		// ensure limb has mesh
		sMesh* pMesh = pObject->ppFrameList [ iLimbID ]->pMesh;
		if ( !pMesh )
			return;

		// apply to specific mesh
		Hide ( pMesh );
	}
}

DARKSDK_DLL void ShowLimb ( int iID, int iLimbID )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimbInstance ( iID, iLimbID ) )
		return;

	// actual object or instance of object
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject->pInstanceMeshVisible )
	{
		// record hide state in instance-mesh-visibility-array
		sObject* pActualObject = pObject->pInstanceOfObject;
		if ( iLimbID>=0 && iLimbID<pActualObject->iFrameCount)
			pObject->pInstanceMeshVisible [ iLimbID ] = true;
	}
	else
	{
		// ensure limb has mesh
		sMesh* pMesh = pObject->ppFrameList [ iLimbID ]->pMesh;
		if ( !pMesh )
			return;

		// apply to specific mesh
		Show ( pMesh );
	}
}

DARKSDK_DLL void TextureLimb ( int iID, int iLimbID, int iImageID )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// ensure limb has mesh
	sMesh* pMesh = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->pMesh;
	if ( !pMesh )
		return;

	// apply to specific mesh
	SetBaseTexture ( pMesh, -1, iImageID );
}

DARKSDK_DLL void TextureLimbStage ( int iID, int iLimbID, int iStage, int iImageID )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// ensure limb has mesh
	sMesh* pMesh = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->pMesh;
	if ( !pMesh )
		return;

	// apply to specific mesh
	SetBaseTextureStage ( pMesh, iStage, iImageID );
}

DARKSDK_DLL void ColorLimb ( int iID, int iLimbID, DWORD dwColor )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// ensure limb has mesh
	sMesh* pMesh = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->pMesh;
	if ( !pMesh )
		return;

	// apply to specific mesh
	SetBaseColor ( pMesh, dwColor );
}

DARKSDK_DLL void ScrollLimbTexture ( int iID, int iLimbID, int iStage, float fU, float fV )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// ensure limb has mesh
	sMesh* pMesh = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->pMesh;
	if ( !pMesh )
		return;

	// apply to specific mesh
	ScrollTexture ( pMesh, iStage, fU, fV );
}

DARKSDK_DLL void ScrollLimbTexture ( int iID, int iLimbID, float fU, float fV )
{
	// refers to core function above
	ScrollLimbTexture ( iID, iLimbID, 0, fU, fV );
}

DARKSDK_DLL void ScaleLimbTexture ( int iID, int iLimbID, int iStage, float fU, float fV )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// ensure limb has mesh
	sMesh* pMesh = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->pMesh;
	if ( !pMesh )
		return;

	// apply to specific mesh
	ScaleTexture ( pMesh, iStage, fU, fV );
}

DARKSDK_DLL void ScaleLimbTexture ( int iID, int iLimbID, float fU, float fV )
{
	// refers to core function above
	ScaleLimbTexture ( iID, iLimbID, 0, fU, fV );
}

DARKSDK_DLL void SetLimbSmoothing ( int iID, int iLimbID, float fPercentage )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// limit percentage range
	if ( fPercentage<0.0f ) fPercentage=0.0f;
	if ( fPercentage>100.0f ) fPercentage=100.0f;

	// ensure limb has mesh
	sMesh* pMesh = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->pMesh;
	if ( !pMesh )
		return;

	// apply to specific mesh
	SmoothNormals ( pMesh, fPercentage/100.0f );
}

DARKSDK_DLL void SetLimbNormals ( int iID, int iLimbID )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// ensure limb has mesh
	sMesh* pMesh = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->pMesh;
	if ( !pMesh )
		return;

	// apply to specific mesh
	GenerateNormals ( pMesh );
}

DARKSDK_DLL void OffsetLimb ( int iID, int iLimbID, float fX, float fY, float fZ, int iBoundsFlag )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// apply to specific frame
	Offset ( g_ObjectList [ iID ]->ppFrameList [ iLimbID ], fX, fY, fZ );
	g_ObjectList [ iID ]->bAnimUpdateOnce = true;

	// leefix - 230604 - u54 - copy user matrix to combined matrix
	g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->matCombined = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->matUserMatrix;

	// u55 - 080704 - under flag for best of both worlds
	if ( iBoundsFlag==1 ) CalculateAllBounds ( g_ObjectList [ iID ], false );

//	// mike - 011005 - call scale so we update radius for offset limbs, if we don't do this they may not get drawn
//	Scale ( iID, g_ObjectList [ iID ]->position.vecScale.x * 100, g_ObjectList [ iID ]->position.vecScale.y * 100, g_ObjectList [ iID ]->position.vecScale.z  * 100 );
}

DARKSDK_DLL void OffsetLimb ( int iID, int iLimbID, float fX, float fY, float fZ )
{
	OffsetLimb ( iID, iLimbID, fX, fY, fZ, 0 );
}

DARKSDK_DLL void RotateLimb ( int iID, int iLimbID, float fX, float fY, float fZ, int iBoundsFlag )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// apply to specific frame
	Rotate ( g_ObjectList [ iID ]->ppFrameList [ iLimbID ], fX, fY, fZ );

	// leefix - 230604 - u54 - copy user matrix to combined matrix
    if ( g_ObjectList [ iID ]->position.bApplyPivot==false )
	{
		// 270614 - used applypivot to prevent matCombined getting wiped out during ragdoll creation
		g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->matCombined = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->matUserMatrix;
	}
	g_ObjectList [ iID ]->bAnimUpdateOnce = true;

	// u55 - 080704 - under flag for best of both worlds
	if ( iBoundsFlag==1 ) CalculateAllBounds ( g_ObjectList [ iID ], false );
}

DARKSDK_DLL void RotateLimb ( int iID, int iLimbID, float fX, float fY, float fZ )
{
	RotateLimb ( iID, iLimbID, fX, fY, fZ, 0 );
}

DARKSDK_DLL void ScaleLimb ( int iID, int iLimbID, float fX, float fY, float fZ, int iBoundsFlag )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// scale down
	fX /= 100.0f;
	fY /= 100.0f;
	fZ /= 100.0f;

	// apply to specific frame
	Scale ( g_ObjectList [ iID ]->ppFrameList [ iLimbID ], fX, fY, fZ );

	// leefix - 230604 - u54 - copy user matrix to combined matrix
	g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->matCombined = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->matUserMatrix;
	g_ObjectList [ iID ]->bAnimUpdateOnce = true;

	// u55 - 080704 - under flag for best of both worlds
	if ( iBoundsFlag==1 ) CalculateAllBounds ( g_ObjectList [ iID ], false );
}

DARKSDK_DLL void ScaleLimb ( int iID, int iLimbID, float fX, float fY, float fZ )
{
	ScaleLimb ( iID, iLimbID, fX, fY, fZ, 0 );
}

DARKSDK_DLL void AddLimb ( int iID, int iLimbID, int iMeshID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// check the mesh exists
	if ( !ConfirmMesh ( iMeshID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// next available frame is
	int iAvailableFrame = pObject->iFrameCount;

	// if frame specified, must be correct
	if ( iLimbID > -1 )
	{
		if ( iLimbID!=iAvailableFrame )
		{
			// must be new consecutive limb 
			RunTimeError ( RUNTIMEERROR_LIMBMUSTCHAININSEQUENCE );
		}
	}

	// and the mesh to create frame with
	sMesh* pNewMesh = new sMesh;
	if ( pNewMesh==NULL )
		return;

	// no transform of new limb
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity ( &matWorld );

	// make a copy of the mesh
	MakeMeshFromOtherMesh ( true, pNewMesh, g_RawMeshList [ iMeshID ], &matWorld );

	// add new frame to end of 
	if ( !AddNewFrame ( pObject, pNewMesh, "new limb" ) )
	{
		// could not make limb
		RunTimeError ( RUNTIMEERROR_B3DMESHTOOLARGE );
		SAFE_DELETE ( pNewMesh );
	}

	// recreate all mesh and frame lists
	CreateFrameAndMeshList ( pObject );

	// ensure bounds are recalculated
	pObject->bUpdateOverallBounds=true;

	// update mesh(es) of object
	m_ObjectManager.RefreshObjectInBuffer ( pObject );
}

DARKSDK_DLL void RemoveLimb ( int iID, int iLimbID )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// cannot remove root frame
	if ( iLimbID==0 )
	{
		// could not remove limb zero
		RunTimeError ( RUNTIMEERROR_LIMBNUMBERILLEGAL );
		return;
	}

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// free mesh resources
	if ( pObject->ppFrameList [ iLimbID ] )
	{
		if ( pObject->ppFrameList [ iLimbID ]->pMesh )
		{
			// frees buffers used to render mesh to backbuffer
			m_ObjectManager.RemoveBuffersUsedByObjectMesh ( pObject->ppFrameList [ iLimbID ]->pMesh );
		}
	}

	// remove frame from object
	if ( !RemoveFrame ( pObject, pObject->ppFrameList [ iLimbID ] ) )
	{
		// could not remove limb
		RunTimeError ( RUNTIMEERROR_LIMBNOTEXIST );
		return;
	}

	// recreate all meshand frame lists
	CreateFrameAndMeshList ( pObject );

	// ensure bounds are recalculated
	pObject->bUpdateOverallBounds=true;

	// update mesh(es) of object
	m_ObjectManager.RefreshObjectInBuffer ( pObject );
}

DARKSDK_DLL void LinkLimb ( int iID, int iParentID, int iLimbID )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iParentID ) )
		return;

	// cannot move root frame
	if ( iLimbID==0 )
	{
		// could not remove limb zero
		RunTimeError ( RUNTIMEERROR_LIMBNUMBERILLEGAL );
		return;
	}

	// cannot to ones self
	if ( iLimbID==iParentID )
	{
		// could not remove limb zero
		RunTimeError ( RUNTIMEERROR_LIMBNUMBERILLEGAL );
		return;
	}

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// link frame from object
	if ( !LinkFrame ( pObject, pObject->ppFrameList [ iLimbID ], pObject->ppFrameList [ iParentID ] ) )
	{
		// could not link limb
		RunTimeError ( RUNTIMEERROR_LIMBNOTEXIST );
		return;
	}

	// recreate all meshand frame lists
	CreateFrameAndMeshList ( pObject );

	// ensure bounds are recalculated
	pObject->bUpdateOverallBounds=true;
}

//
// MESH Commands
//

DARKSDK_DLL void LoadMeshCore ( SDK_LPSTR szFilename, int iMeshID )
{
	// ensure the mesh is okay to use
	ConfirmNewMesh ( iMeshID );

	// check memory allocation
	ID_MESH_ALLOCATION ( iMeshID );

	// load the mesh from an x file
	if ( !LoadRawMesh ( (LPSTR)szFilename, &g_RawMeshList [ iMeshID ] ) )
	{
		// pass mesh filename
		char pExtraErr[512];
		wsprintf ( pExtraErr, "LOAD MESH '%s',%d", szFilename, iMeshID );
		RunTimeError ( RUNTIMEERROR_B3DMESHNOTEXIST, pExtraErr );
		return;
	}
}

DARKSDK_DLL void LoadMesh ( SDK_LPSTR szFilename, int iMeshID )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, (LPSTR)szFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	CheckForWorkshopFile (VirtualFilename);

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	LoadMeshCore ( (SDK_LPSTR)VirtualFilename, iMeshID );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );
}

DARKSDK_DLL void DeleteMesh ( int iMeshID )
{
	// check the mesh exists
	if ( !ConfirmMesh ( iMeshID ) )
		return;

	// delete the mesh
	if ( !DeleteRawMesh ( g_RawMeshList [ iMeshID ] ) )
		return;

	// free the mesh from the list
	g_RawMeshList [ iMeshID ] = NULL;
}

DARKSDK_DLL void SaveMesh ( SDK_LPSTR pFilename, int iMeshID )
{
	// check the mesh exists
	if ( !ConfirmMesh ( iMeshID ) )
		return;

	// save mesh as an x file
	if ( !SaveRawMesh ( (LPSTR)pFilename, g_RawMeshList [ iMeshID ] ) )
		return;
}

DARKSDK_DLL void ChangeMesh ( int iObjectID, int iLimbID, int iMeshID )
{
	// check the mesh exists
	if ( !ConfirmMesh ( iMeshID ) )
		return;

	// check the limb exists
	if ( !ConfirmObjectAndLimb ( iObjectID, iLimbID ) )
		return;

	// create new mesh
	sMesh* pNewMesh = new sMesh;
	if ( pNewMesh==NULL )
		return;

	// no transform of new limb
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity ( &matWorld );

	// make a copy of the mesh
	MakeMeshFromOtherMesh ( true, pNewMesh, g_RawMeshList [ iMeshID ], &matWorld );

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iObjectID ];
	sFrame* pFrameOfMeshToReplace = pObject->ppFrameList [ iLimbID ];
	sMesh* pOldMesh = pFrameOfMeshToReplace->pMesh;

	// lee - 280306 - u6rc2 - if specify limb with no mesh, exit now
	if ( pOldMesh==NULL )
	{
		// lee - 150909 - U75 - no way to detect if mesh in limb, so silent fail this
		// no mesh exists for this limb(frame)
		//RunTimeError ( RUNTIMEERROR_B3DLIMBBUTNOMESH );
		RunTimeWarning ( RUNTIMEERROR_B3DLIMBBUTNOMESH );
		SAFE_DELETE ( pNewMesh );
		return;
	}

	// create a texture-set for new mesh
	DWORD dwTextureCount = pOldMesh->dwTextureCount;
	pNewMesh->pTextures = new sTexture [ dwTextureCount ];
	pNewMesh->dwTextureCount = dwTextureCount;

	// lee - 200206 - u60 - extract all material and texture information from old mesh
	CloneInternalTextures ( pNewMesh, pOldMesh );
	CopyMeshSettings ( pNewMesh, pFrameOfMeshToReplace->pMesh );

	// 200603 - remove drawbuffer from mesh to be replaced
	m_ObjectManager.RemoveBuffersUsedByObjectMesh ( pFrameOfMeshToReplace->pMesh );

	// replace mesh
	if ( !ReplaceFrameMesh ( pFrameOfMeshToReplace, pNewMesh ) )
	{
		// failed to change mesh
		SAFE_DELETE ( pNewMesh );
	}

	/* old pre-2006
	// use material mode from original mesh
	bool bStoreMaterialMode = pFrameOfMeshToReplace->pMesh->bUsesMaterial;
	// give mesh basic texture (for visibility sorting) - maybe copy old texture over..
	SetBaseColor ( pNewMesh, D3DCOLOR_ARGB(255,255,255,255) );
	// use material mode from original mesh
	pNewMesh->bUsesMaterial = bStoreMaterialMode;
	*/

	// recreate all meshand frame lists
	CreateFrameAndMeshList ( pObject );

	// calculate bounding areas of object
	CalculateAllBounds ( pObject, false );
	
	// update mesh(es) of object
	m_ObjectManager.RenewReplacedMeshes ( pObject );
}

DARKSDK_DLL void ConvertMeshToVertexData ( int iMeshID )
{
	// leeadd - 140405 - check the mesh not exists
	if ( !ConfirmMesh ( iMeshID ) )
		return;

	// do the conversion
	if ( g_RawMeshList ) ConvertLocalMeshToVertsOnly ( g_RawMeshList [ iMeshID ] ); 
}

DARKSDK_DLL void MakeMeshFromObject ( int iMeshID, int iObjectID, int iIgnoreMode )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iObjectID ) )
		return;

	// check the mesh not exists
	if ( !ConfirmNewMesh ( iMeshID ) )
		return;

    // Get a pointer to the object (or if an instance, that instances object)
    sObject* pObject = g_ObjectList [ iObjectID ];
    if (pObject->pInstanceOfObject)
        pObject = pObject->pInstanceOfObject;

	// create new mesh
	sMesh* pNewMesh = NULL;
	if ( !CreateSingleMeshFromObject ( &pNewMesh, pObject, iIgnoreMode ) )
		return;

	// leeadd - 080405 - convert final mesh to vert only (for ODE trimesh support)
	// lee, interferes with fpsc-mapeditor blueprint, needs index data (and should keep it)
	// so move this to later in the trimesh making, as only ODE needs this!
	// maybe a new ObjectDLL command CONVERT OBJECT TO VERTEXDATA Object Number
	// ConvertLocalMeshToVertsOnly ( pNewMesh ); 
	// leeadd - 141008 - u70 - also make mesh from sphere and cylinder object need this - so put back in this case!
	if ( pNewMesh->iPrimitiveType!=D3DPT_TRIANGLELIST )
		ConvertLocalMeshToVertsOnly ( pNewMesh ); 

	// check memory allocation
	ID_MESH_ALLOCATION ( iMeshID );

	// if full, delete contents
	if ( g_RawMeshList [ iMeshID ] )
	{
		SAFE_DELETE ( g_RawMeshList [ iMeshID ] );
	}

	// assign new mesh to rawmeshlist
	g_RawMeshList[iMeshID] = pNewMesh;
}

DARKSDK_DLL void MakeMeshFromLimb ( int iMeshID, int iObjectID, int iLimbNumber )
{
	// check the object exists
	if ( !ConfirmObjectAndLimbInstance ( iObjectID, iLimbNumber ) )
		return;

	// check the mesh not exists
	if ( !ConfirmNewMesh ( iMeshID ) )
		return;

    // Get a pointer to the object (or if an instance, that instances object)
    sObject* pObject = g_ObjectList [ iObjectID ];
    if (pObject->pInstanceOfObject)
        pObject = pObject->pInstanceOfObject;

	// early out if no mesh associated with the limb specified
	if ( pObject->ppFrameList )
		if ( pObject->ppFrameList[iLimbNumber] )
			if ( pObject->ppFrameList[iLimbNumber]->pMesh==NULL )
				return;

	// create new mesh
	sMesh* pNewMesh = NULL;
	if ( !CreateSingleMeshFromLimb ( &pNewMesh, pObject, iLimbNumber, 0 ) )
		return;

	// leeadd - 080405 - convert final mesh to vert only (for ODE trimesh support)
	// lee, interferes with fpsc-mapeditor blueprint, needs index data (and should keep it)
	// so move this to later in the trimesh making, as only ODE needs this!
	// maybe a new ObjectDLL command CONVERT OBJECT TO VERTEXDATA Object Number
	// ConvertLocalMeshToVertsOnly ( pNewMesh ); 
	// leeadd - 141008 - u70 - also make mesh from sphere and cylinder object need this - so put back in this case!
	if ( pNewMesh->iPrimitiveType!=D3DPT_TRIANGLELIST )
		ConvertLocalMeshToVertsOnly ( pNewMesh ); 

	// check memory allocation
	ID_MESH_ALLOCATION ( iMeshID );

	// if full, delete contents
	if ( g_RawMeshList [ iMeshID ] )
	{
		SAFE_DELETE ( g_RawMeshList [ iMeshID ] );
	}

	// assign new mesh to rawmeshlist
	g_RawMeshList[iMeshID] = pNewMesh;
}

DARKSDK_DLL void MakeMeshFromObject ( int iMeshID, int iObjectID )
{
	// ignore nothing
	MakeMeshFromObject ( iMeshID, iObjectID, 0 );
}

DARKSDK_DLL void ReduceMesh ( int iMeshID, int iBlockMode, int iNearMode, int iGX, int iGY, int iGZ )
{
	//	reduce mesh 1,blockmode,nearmode,gx,gy,gz

	// check the mesh exists
	if ( !ConfirmMesh ( iMeshID ) )
		return;

	// reduce mesh as instructed
	ReduceMeshPolygons ( g_RawMeshList [ iMeshID ], iBlockMode, iNearMode, iGX, iGY, iGZ );
}

DARKSDK_DLL void MakeLODFromMesh ( int iMeshID, int iVertexNum, int iMeshNewID )
{
	// check the mesh exists
	if ( !ConfirmMesh ( iMeshID ) )
		return;

	// ensure mesh list is valid
	if ( !g_RawMeshList )
	{
		g_iRawMeshListCount = 0;
		ResizeRawMeshList ( iMeshNewID );
	}

	// check the mesh not exists
	if ( !ConfirmNewMesh ( iMeshNewID ) )
		return;

	// reduce mesh as instructed
	sMesh* pNewMesh = NULL;
	if ( MakeLODMeshFromMesh ( g_RawMeshList [ iMeshID ], iVertexNum, &pNewMesh ) )
	{
		// check memory allocation
		ID_MESH_ALLOCATION ( iMeshNewID );

		// if full, delete contents
		if ( g_RawMeshList [ iMeshNewID ] )
		{
			SAFE_DELETE ( g_RawMeshList [ iMeshNewID ] );
		}
		
		// assign new mesh to rawmeshlist
		g_RawMeshList[iMeshNewID] = pNewMesh;
	}
}

// Mesh/Limb Manipulation Commands

DBPRO_GLOBAL sObject*	g_pCurrentVertexDataObject		= NULL;
DBPRO_GLOBAL sMesh*		g_pCurrentVertexDataMesh		= NULL;
DBPRO_GLOBAL int		g_iCurrentVertexDataUpdateMode	= 0;

DARKSDK_DLL void LockVertexDataForLimbCore ( int iID, int iLimbID, int iReplaceOrUpdate )
{
	// check the object limb exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return;

	// ensure limb has mesh
	sMesh* pMesh = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->pMesh;
	if ( !pMesh )
		return;

	// assign mesh of limb for editing
	g_pCurrentVertexDataMesh = pMesh;
	g_pCurrentVertexDataObject = g_ObjectList [ iID ];
	g_iCurrentVertexDataUpdateMode = iReplaceOrUpdate;
}

DARKSDK_DLL void LockVertexDataForLimb ( int iID, int iLimbID )
{
	// lock vertex in basic replace mode
	LockVertexDataForLimbCore ( iID, iLimbID, 1 );
}

DARKSDK_DLL void LockVertexDataForMesh ( int iMeshID )
{
	// check the mesh exists
	if ( !ConfirmMesh ( iMeshID ) )
		return;

	// assign mesh of rawmesh for editing
	g_pCurrentVertexDataMesh = g_RawMeshList [ iMeshID ];
	g_pCurrentVertexDataObject = NULL;
	g_iCurrentVertexDataUpdateMode = 0;
}

DARKSDK_DLL void UnlockVertexData ( void )
{
	// mike - 010903 - need to update bounds or object may not be visible
//	CalculateAllBounds ( g_pCurrentVertexDataObject, true ); //slow slow slow

	// MIKE - 040204 - temporary fix for physics
	// LEE - 190204 - allow manual control over '1update' or '0replace' (until find physics issue)
	if ( g_pCurrentVertexDataObject )
	{
		if ( g_iCurrentVertexDataUpdateMode==2 )
		{
			// leeadd - 010306 - u60 - update original copy if flagged
			SAFE_DELETE ( g_pCurrentVertexDataMesh->pOriginalVertexData );
			CollectOriginalVertexData ( g_pCurrentVertexDataMesh );

			// not quite as fast as mode 1, but good for saving final results
			g_pCurrentVertexDataMesh->bVBRefreshRequired = true;
			g_vRefreshMeshList.push_back ( g_pCurrentVertexDataMesh );
		}
		else
		{
			if ( g_iCurrentVertexDataUpdateMode==1 )
			{
				// mesh VB update - same size, just adjusted data - fast
				g_pCurrentVertexDataMesh->bVBRefreshRequired = true;
				g_vRefreshMeshList.push_back ( g_pCurrentVertexDataMesh );
			}
			else
			{
				// complete mesh replace - slow
				g_pCurrentVertexDataObject->bReplaceObjectFromBuffers = true;
				m_ObjectManager.g_bObjectReplacedUpdateBuffers = true;
			}
		}
	}

	// end edit session
	g_pCurrentVertexDataMesh = NULL;
	g_pCurrentVertexDataObject = NULL;
	g_iCurrentVertexDataUpdateMode = 0;
}

DARKSDK_DLL void SetVertexDataPosition ( int iVertex, float fX, float fY, float fZ )
{
	// write directly to mesh
	if ( !g_pCurrentVertexDataMesh ) return;
	if ( iVertex<0 || iVertex>=(int)g_pCurrentVertexDataMesh->dwVertexCount ) return;

	SetPositionData ( g_pCurrentVertexDataMesh, iVertex, fX, fY, fZ );
}

DARKSDK_DLL void SetVertexDataNormals ( int iVertex, float fNX, float fNY, float fNZ )
{
	// write directly to mesh
	if ( !g_pCurrentVertexDataMesh ) return;
	if ( iVertex<0 || iVertex>=(int)g_pCurrentVertexDataMesh->dwVertexCount ) return;
	SetNormalsData ( g_pCurrentVertexDataMesh, iVertex, fNX, fNY, fNZ );
}

DARKSDK_DLL void SetVertexDataDiffuse	( int iVertex, DWORD dwDiffuse )
{
	// write directly to mesh
	if ( !g_pCurrentVertexDataMesh ) return;
	if ( iVertex<0 || iVertex>=(int)g_pCurrentVertexDataMesh->dwVertexCount ) return;
	SetDiffuseData ( g_pCurrentVertexDataMesh, iVertex, dwDiffuse );
}

DARKSDK_DLL void SetIndexData ( int iIndex, int iValue )
{
	// write directly to mesh
	if ( !g_pCurrentVertexDataMesh ) return;
	if ( iIndex<0 || iIndex>=(int)g_pCurrentVertexDataMesh->dwIndexCount ) return;
	g_pCurrentVertexDataMesh->pIndices [ iIndex ] = iValue;
}

DARKSDK_DLL void SetVertexDataUV ( int iVertex, float fU, float fV )
{
	// write directly to mesh
	if ( !g_pCurrentVertexDataMesh ) return;
	if ( iVertex<0 || iVertex>=(int)g_pCurrentVertexDataMesh->dwVertexCount ) return;

	SetUVData ( g_pCurrentVertexDataMesh, iVertex, fU, fV );
}

DARKSDK_DLL void SetVertexDataUV ( int iVertex, int iIndex, float fU, float fV )
{
	// write directly to mesh
	if ( !g_pCurrentVertexDataMesh ) return;
	if ( iVertex<0 || iVertex>=(int)g_pCurrentVertexDataMesh->dwVertexCount ) return;

	// only stages 0 to 7
	if ( iIndex >= 8 )
		return;

	// convert mesh if not supporting the stage
	bool bOkay=false;
	for ( int iI=0; iI<=iIndex; iI++ )
	{
		if ( iI==0 && iIndex<=iI && g_pCurrentVertexDataMesh->dwFVF & D3DFVF_TEX1 )  bOkay=true;
		if ( iI==1 && iIndex<=iI && g_pCurrentVertexDataMesh->dwFVF & D3DFVF_TEX2 )  bOkay=true;
		if ( iI==2 && iIndex<=iI && g_pCurrentVertexDataMesh->dwFVF & D3DFVF_TEX3 )  bOkay=true;
		if ( iI==3 && iIndex<=iI && g_pCurrentVertexDataMesh->dwFVF & D3DFVF_TEX4 )  bOkay=true;
		if ( iI==4 && iIndex<=iI && g_pCurrentVertexDataMesh->dwFVF & D3DFVF_TEX5 )  bOkay=true;
		if ( iI==5 && iIndex<=iI && g_pCurrentVertexDataMesh->dwFVF & D3DFVF_TEX6 )  bOkay=true;
		if ( iI==6 && iIndex<=iI && g_pCurrentVertexDataMesh->dwFVF & D3DFVF_TEX7 )  bOkay=true;
		if ( iI==7 && iIndex<=iI && g_pCurrentVertexDataMesh->dwFVF & D3DFVF_TEX8 )  bOkay=true;
	}
	if ( bOkay==false )
	{
		// convert to correct format
		DWORD dwFVF = g_pCurrentVertexDataMesh->dwFVF;
		if ( iIndex==0 ) dwFVF = dwFVF | D3DFVF_TEX1;
		if ( iIndex==1 ) dwFVF = dwFVF | D3DFVF_TEX2;
		if ( iIndex==2 ) dwFVF = dwFVF | D3DFVF_TEX3;
		if ( iIndex==3 ) dwFVF = dwFVF | D3DFVF_TEX4;
		if ( iIndex==4 ) dwFVF = dwFVF | D3DFVF_TEX5;
		if ( iIndex==5 ) dwFVF = dwFVF | D3DFVF_TEX6;
		if ( iIndex==6 ) dwFVF = dwFVF | D3DFVF_TEX7;
		if ( iIndex==7 ) dwFVF = dwFVF | D3DFVF_TEX8;
		ConvertLocalMeshToFVF ( g_pCurrentVertexDataMesh, dwFVF );
	}

	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( g_pCurrentVertexDataMesh, &offsetMap );

	// make sure we have an offset to write UVs
	if ( offsetMap.dwTU[iIndex]>0 )
	{
		// set single UV vertex component
		*( ( float* ) g_pCurrentVertexDataMesh->pVertexData + offsetMap.dwTU[iIndex] + ( offsetMap.dwSize * iVertex ) ) = fU;
		*( ( float* ) g_pCurrentVertexDataMesh->pVertexData + offsetMap.dwTV[iIndex] + ( offsetMap.dwSize * iVertex ) ) = fV;
	}

	// flag mesh for a VB update
	g_pCurrentVertexDataMesh->bVBRefreshRequired=true;
	g_vRefreshMeshList.push_back ( g_pCurrentVertexDataMesh );
}

DARKSDK_DLL void SetVertexDataSize ( int iVertex, float fSize )
{
	// mike - 160903 - set size for point sprites

	if ( !g_pCurrentVertexDataMesh ) return;

	sOffsetMap offsetMap;
	GetFVFOffsetMap ( g_pCurrentVertexDataMesh, &offsetMap );

	// make sure we have data in the vertices
	if ( g_pCurrentVertexDataMesh->dwFVF & D3DFVF_PSIZE )
	{
		if ( iVertex < (int)g_pCurrentVertexDataMesh->dwVertexCount )
			*( ( float* ) g_pCurrentVertexDataMesh->pVertexData + offsetMap.dwPointSize + ( offsetMap.dwSize * iVertex ) ) = fSize;
	}

	// flag mesh for a VB update
	g_pCurrentVertexDataMesh->bVBRefreshRequired=true;
	g_vRefreshMeshList.push_back ( g_pCurrentVertexDataMesh );
}

DARKSDK_DLL void AddMeshToVertexData ( int iMeshID )
{
	// check the mesh exists
	if ( !ConfirmMesh ( iMeshID ) )
		return;

	// write directly to mesh
	if ( g_pCurrentVertexDataMesh )
		if ( !AddMeshToData ( g_pCurrentVertexDataMesh, g_RawMeshList [ iMeshID ] ) )
			return;

	// must renew mesh
	if ( g_pCurrentVertexDataObject )
		m_ObjectManager.RenewReplacedMeshes ( g_pCurrentVertexDataObject );
}

DARKSDK_DLL void DeleteMeshFromVertexData ( int iVertex1, int iVertex2, int iIndex1, int iIndex2 )
{
	// write directly to mesh
	if ( g_pCurrentVertexDataMesh )
		if ( !DeleteMeshFromData ( g_pCurrentVertexDataMesh, iVertex1, iVertex2, iIndex1, iIndex2 ) )
			return;

	// must renew mesh
	if ( g_pCurrentVertexDataObject )
		m_ObjectManager.RenewReplacedMeshes ( g_pCurrentVertexDataObject );
}

DARKSDK_DLL void PeformMeshCSGOnVertexData ( int iBrushMeshID, int iMode )
{
	// csg work
	cCSG csg;

	// check the mesh exists
	if ( !ConfirmMesh ( iBrushMeshID ) )
		return;

	// write directly to mesh
	if ( g_pCurrentVertexDataMesh )
	{
		// set identity matrix
		D3DXMATRIX matIdentityMatrix;
		D3DXMatrixIdentity ( &matIdentityMatrix );

		// target and brush mesh
		sMesh* pMeshA = g_pCurrentVertexDataMesh;
		sMesh* pMeshB = g_RawMeshList [ iBrushMeshID ];

		// perform csg on single mesh
		switch ( iMode )
		{
			case 1 : csg.Union ( pMeshA, matIdentityMatrix, pMeshB, matIdentityMatrix );			break;
			case 2 : csg.Difference ( pMeshA, matIdentityMatrix, pMeshB, matIdentityMatrix );		break;
			case 3 : csg.Intersection ( pMeshA, matIdentityMatrix, pMeshB, matIdentityMatrix );		break;
		}

		// ensure old data of pMeshA is wiped
		SAFE_DELETE ( pMeshA->pOriginalVertexData );
	}

	// must renew mesh
	if ( g_pCurrentVertexDataObject )
	{
		m_ObjectManager.RenewReplacedMeshes ( g_pCurrentVertexDataObject );
	}
}

DARKSDK_DLL void PeformCSGUnionOnVertexData ( int iBrushMeshID )
{
	// Union against mesh
	PeformMeshCSGOnVertexData ( iBrushMeshID, 1 );
}

DARKSDK_DLL void PeformCSGDifferenceOnVertexData ( int iBrushMeshID )
{
	// Difference against mesh
	PeformMeshCSGOnVertexData ( iBrushMeshID, 2 );
}

DARKSDK_DLL void PeformCSGIntersectionOnVertexData ( int iBrushMeshID )
{
	// Intersection against mesh
	PeformMeshCSGOnVertexData ( iBrushMeshID, 3 );
}

DARKSDK_DLL int ObjectBlocking	( int iID, float X1, float Y1, float Z1, float X2, float Y2, float Z2 )
{
	// default is not blocking
	bool bResult = false;

	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return false;

	// check ALL meshes
	sObject* pObject = g_ObjectList [ iID ];
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
		if ( CheckIfMeshBlocking ( pObject->ppMeshList [ iMesh ], X1, Y1, Z1, X2, Y2, Z2 )==true )
			bResult=true;

	// result
	return bResult;
}

DARKSDK_DLL void AddMemblockToObject ( int iMemblock, int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// check memblock DLL exists
	if ( !g_Memblock_GetMemblockExist )
		return;

	// check memblock X exists
	if ( !g_Memblock_GetMemblockExist ( iMemblock ) )
		return;

	// release any previous custom data
	SAFE_DELETE ( pObject->pCustomData );

	// create custom data from memblock
	DWORD dwMemBlockSize = g_Memblock_GetMemblockSize ( iMemblock );
	pObject->dwCustomSize = dwMemBlockSize + 8;
	pObject->pCustomData = new char [ pObject->dwCustomSize ];
	LPSTR pMemData = (LPSTR)g_Memblock_GetMemblockPtr ( iMemblock );
	BYTE* pBytePtr = (BYTE*)pObject->pCustomData;
	*((DWORD*)pBytePtr+0 ) = DBOBLOCK_OBJECT_CUSTOMDATA; // custom token
	memcpy ( pBytePtr+4, pMemData, dwMemBlockSize ); // memblock data
	*((DWORD*)(pBytePtr+4+dwMemBlockSize) ) = 0; // terminator
}

DARKSDK_DLL void GetMemblockFromObject ( int iMemblock, int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// ensure we have custom data
	if ( !pObject->pCustomData )
		return;

	// check call ptr to memblock DLL exists
	if ( !g_Memblock_GetMemblockExist )
		return;

	// 250413 - must be real membloc, not instance stamp custom data
	if ( pObject->dwCustomSize>4000000000 )
		return;

	// get custom data and get the data from it
	DWORD dwMemblockToken = *((DWORD*)(LPSTR)pObject->pCustomData+0 );
	LPSTR pMemblockData = (LPSTR)pObject->pCustomData+4;
	DWORD dwMemBlockSize = pObject->dwCustomSize-8;

	// token must be correct
	if ( dwMemblockToken!=DBOBLOCK_OBJECT_CUSTOMDATA )
		return;

	// check memblock X exists
	if ( g_Memblock_GetMemblockExist ( iMemblock ) )
	{
		// delete it as we are creating a new one
		g_Memblock_DeleteMemblock ( iMemblock );
	}	

	// make memblock
	g_Memblock_MakeMemblock ( iMemblock, dwMemBlockSize );

	// read data into memblock from custom data in object
	LPSTR pMemData = (LPSTR)g_Memblock_GetMemblockPtr ( iMemblock );
	BYTE* pBytePtr = (BYTE*)pObject->pCustomData;
	memcpy ( pMemData, pBytePtr+4, dwMemBlockSize );
}

DARKSDK_DLL void DeleteMemblockFromObject ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// ensure we have custom data
	if ( !pObject->pCustomData )
		return;

	// clear custom data
	SAFE_DELETE ( pObject->pCustomData );
	pObject->dwCustomSize = 0;
}

void SetObjectStatisticsInteger ( int iID, int iIndex, int dwValue )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// object ptr
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject->dwCustomSize==0 )
	{
		// create custom slot
		DWORD dwStatisticsDataSize = 8;
		pObject->dwCustomSize = dwStatisticsDataSize*-1;
		pObject->pCustomData = (LPVOID)new DWORD[dwStatisticsDataSize];
		for ( DWORD i=0; i<dwStatisticsDataSize; i++ )
			*(((DWORD*)pObject->pCustomData)+i) = 0;
	}
	if ( pObject->dwCustomSize>4000000000 )
	{
		*(((DWORD*)pObject->pCustomData)+iIndex) = dwValue;
	}
}

DARKSDK_DLL int ObjectStatisticsInteger ( int iID, int iIndex )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return 0;

	// object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// ensure we have custom data
	int iResult = 0;
	if ( pObject->dwCustomSize>4000000000 )
	{
		if ( pObject->pCustomData )
		{
			iResult = *(((int*)pObject->pCustomData)+iIndex);
		}
	}
	return iResult;
}


// Mesh/Limb Manipulation Expressions

DARKSDK_DLL int GetVertexDataVertexCount ( void )
{
	// return vertexdata value
	if ( !g_pCurrentVertexDataMesh ) return 0;
	return GetVertexCount ( g_pCurrentVertexDataMesh );
}

DARKSDK_DLL int GetVertexDataIndexCount ( void )
{
	// return vertexdata value
	if ( !g_pCurrentVertexDataMesh ) return 0;
	return GetIndexCount ( g_pCurrentVertexDataMesh );
}

DARKSDK_DLL SDK_FLOAT GetVertexDataPositionX ( int iVertex )
{
	// return vertexdata value
	if ( !g_pCurrentVertexDataMesh ) return 0;
	float fValue = GetDataPositionX ( g_pCurrentVertexDataMesh, iVertex );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetVertexDataPositionY ( int iVertex )
{
	// return vertexdata value
	if ( !g_pCurrentVertexDataMesh ) return 0;
	float fValue = GetDataPositionY ( g_pCurrentVertexDataMesh, iVertex );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetVertexDataPositionZ ( int iVertex )
{
	// return vertexdata value
	if ( !g_pCurrentVertexDataMesh ) return 0;
	float fValue = GetDataPositionZ ( g_pCurrentVertexDataMesh, iVertex );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetVertexDataNormalsX	( int iVertex )
{
	// return vertexdata value
	if ( !g_pCurrentVertexDataMesh ) return 0;
	float fValue = GetDataNormalsX ( g_pCurrentVertexDataMesh, iVertex );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetVertexDataNormalsY	( int iVertex )
{
	// return vertexdata value
	if ( !g_pCurrentVertexDataMesh ) return 0;
	float fValue = GetDataNormalsY ( g_pCurrentVertexDataMesh, iVertex );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetVertexDataNormalsZ	( int iVertex )
{
	// return vertexdata value
	if ( !g_pCurrentVertexDataMesh ) return 0;
	float fValue = GetDataNormalsZ ( g_pCurrentVertexDataMesh, iVertex );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL DWORD GetVertexDataDiffuse ( int iVertex )
{
	// return vertexdata value
	if ( !g_pCurrentVertexDataMesh ) return 0;
	return GetDataDiffuse ( g_pCurrentVertexDataMesh, iVertex );
}

DARKSDK_DLL SDK_FLOAT GetVertexDataU ( int iVertex )
{
	// return vertexdata value
	if ( !g_pCurrentVertexDataMesh ) return 0;
	float fValue = GetDataU ( g_pCurrentVertexDataMesh, iVertex );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetVertexDataV ( int iVertex )
{
	// return vertexdata value
	if ( !g_pCurrentVertexDataMesh ) return 0;
	float fValue = GetDataV ( g_pCurrentVertexDataMesh, iVertex );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetVertexDataU ( int iVertex, int iIndex )
{
	if ( !g_pCurrentVertexDataMesh ) return 0;
	float fValue = GetDataU ( g_pCurrentVertexDataMesh, iVertex, iIndex );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetVertexDataV ( int iVertex, int iIndex )
{
	if ( !g_pCurrentVertexDataMesh ) return 0;
	float fValue = GetDataV ( g_pCurrentVertexDataMesh, iVertex, iIndex );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL int GetIndexData ( int iIndex )
{
	if ( !g_pCurrentVertexDataMesh ) return 0;
	if ( iIndex<0 || iIndex>=(int)g_pCurrentVertexDataMesh->dwIndexCount ) return 0;
	return g_pCurrentVertexDataMesh->pIndices [ iIndex ];
}

DARKSDK_DLL DWORD GetVertexDataPtr ( void )
{
	return (DWORD)g_pCurrentVertexDataMesh;
}

// Misc Commands

DARKSDK_DLL void SetFastBoundsCalculation ( int iMode )
{
	if ( iMode==1 )
		g_bFastBoundsCalculation = true;
	else
		g_bFastBoundsCalculation = false;
}

DARKSDK_DLL void SetMipmapMode ( int iMode )
{
	// OBSOLETE
}

DARKSDK_DLL void FlushVideoMemory ( void )
{
	// OBSOLETE
}

DARKSDK_DLL void DisableTNL ( void )
{
	// OBSOLETE
}

DARKSDK_DLL void EnableTNL ( void )
{
	// OBSOLETE
}

DARKSDK_DLL void Convert3DStoX ( DWORD pFilename1, DWORD pFilename2 )
{
	// OBSOLETE
}

DARKSDK_DLL int PickScreenObjectEx ( int iX, int iY, int iObjectStart, int iObjectEnd, int iIgnoreCamera )
{
	// result hit
	int iObjectHit=0;
    bool bPickingLocked = false;

	// leefix - 010306 - u60 - camera X (actually checked in U6 and it is; dwCurrentSetCameraID )
	tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );

    // u74b7 - Save the FOV as may need to reset it later
    float fCurrentFov = m_Camera_Ptr->fFOV;

	// calculate line from camera to diatant point through projected XY
	D3DXVECTOR3 vecFrom = m_Camera_Ptr->vecPosition;

	// Calculate inverse to take 2D into 3D
	D3DXMATRIX matInvertedView = m_Camera_Ptr->matView;
	D3DXMATRIX matInvertedProjection = m_Camera_Ptr->matProjection;
	D3DXMatrixInverse ( &matInvertedView, NULL, &matInvertedView );
	D3DXMatrixInverse ( &matInvertedProjection, NULL, &matInvertedProjection );

	// leeadd - 130306 - u60 - great god lee adds camera independence
	if ( iIgnoreCamera == 1 )
	{
        // u74b7 - In this mode, we are looking for locked objects only using the default FOV
        bPickingLocked = true;
        if (fCurrentFov != D3DXToDegree(3.14159265358979323846f/2.905f) )
        {
    		g_Camera3D_SetFOV ( D3DXToDegree(3.14159265358979323846f/2.905f) );
        }

		// null camera (facing forward)
		vecFrom = D3DXVECTOR3(0,0,0);
		D3DXMatrixIdentity ( &matInvertedView );
		matInvertedProjection = m_Camera_Ptr->matProjection;
		D3DXMatrixInverse ( &matInvertedView, NULL, &matInvertedView );
		D3DXMatrixInverse ( &matInvertedProjection, NULL, &matInvertedProjection );
	}

	// Transform destination vector into screen
	float fHWidth = m_Camera_Ptr->viewPort3D.Width/2;
	float fHHeight = m_Camera_Ptr->viewPort3D.Height/2;
	iX = iX - m_Camera_Ptr->viewPort3D.X;
	iY = iY - m_Camera_Ptr->viewPort3D.Y;
	iX = iX - fHWidth; iY = fHHeight - iY;
	D3DXVECTOR3 vecTo = D3DXVECTOR3 ( (float)iX/fHWidth, (float)iY/fHHeight, 1.0f );
	D3DXVec3TransformCoord ( &vecTo, &vecTo, &matInvertedProjection );
	D3DXVec3TransformCoord ( &vecTo, &vecTo, &matInvertedView );
	vecTo = vecTo-vecFrom;
	D3DXVec3Normalize ( &vecTo, &vecTo );
	vecTo *= 15000.0f;
	vecTo = vecFrom + vecTo;

	// do intersect test against all objects in shortlist
	float fBestDistance = 999999.9f;
	for ( int iShortList = 0; iShortList < g_iObjectListRefCount; iShortList++ )
	{
		// get index from shortlist
		int iObjectID = g_ObjectListRef [ iShortList ];

		// only those within range
		if ( iObjectID>=iObjectStart && iObjectID<=iObjectEnd )
		{
			// actual object or instance of object
			sObject* pObject = g_ObjectList [ iObjectID ];

			// leeadd - 160504 - u6 - only pick a visible object
            // u74b7 - Allow locked if looking for locked, and vice-versa
			if ( !pObject->bVisible || pObject->bLockedObject != bPickingLocked )
				continue;

			// see if we have a valid list
			float fThisDistance = IntersectObjectCore ( pObject, vecFrom.x, vecFrom.y, vecFrom.z, vecTo.x, vecTo.y, vecTo.z );
			if ( fThisDistance>0.0f && fThisDistance < fBestDistance )
			{
				fBestDistance = fThisDistance;
				iObjectHit = iObjectID;
			}
		}
	}

	// generate useful data from pick
	if ( iObjectHit>0 )
	{
		// store normal vector with distance
		g_DBPROPickResult.iObjectID = iObjectHit;
		g_DBPROPickResult.fPickDistance = fBestDistance;
		g_DBPROPickResult.vecPickVector = vecTo - vecFrom;
		D3DXVec3Normalize ( &g_DBPROPickResult.vecPickVector, &g_DBPROPickResult.vecPickVector );
		g_DBPROPickResult.vecPickVector *= fBestDistance;
	}

    // Reset the FOV if necessary
    if (fCurrentFov != m_Camera_Ptr->fFOV)
    {
		g_Camera3D_SetFOV ( fCurrentFov );
    }

	/// return result
	return iObjectHit;
}

DARKSDK_DLL int PickScreenObject ( int iX, int iY, int iObjectStart, int iObjectEnd )
{
	// see abive - this is the default behaviour
	return PickScreenObjectEx ( iX, iY, iObjectStart, iObjectEnd, 0 );
}

DARKSDK_DLL void PickScreen2D23D ( int iX, int iY, float fDistance )
{
	// camera zero only
	tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );

	// calculate line from camera to diatant point through projected XY
	D3DXVECTOR3 vecFrom = m_Camera_Ptr->vecPosition;

	// Calculate inverse to take 2D into 3D
	D3DXMATRIX matInvertedView = m_Camera_Ptr->matView;
	D3DXMATRIX matInvertedProjection = m_Camera_Ptr->matProjection;
	D3DXMatrixInverse ( &matInvertedView, NULL, &matInvertedView );
	D3DXMatrixInverse ( &matInvertedProjection, NULL, &matInvertedProjection );

	// Transform destination vector into screen
	float fHWidth = m_Camera_Ptr->viewPort3D.Width/2;
	float fHHeight = m_Camera_Ptr->viewPort3D.Height/2;
	iX = iX - m_Camera_Ptr->viewPort3D.X;
	iY = iY - m_Camera_Ptr->viewPort3D.Y;
	iX = iX - fHWidth; iY = fHHeight - iY;
	D3DXVECTOR3 vecTo = D3DXVECTOR3 ( (float)iX/fHWidth, (float)iY/fHHeight, 1.0f );
	D3DXVec3TransformCoord ( &vecTo, &vecTo, &matInvertedProjection );
	D3DXVec3TransformCoord ( &vecTo, &vecTo, &matInvertedView );
	vecTo = vecTo-vecFrom;
	vecTo *= 3000.0f;

	// store normal vector with distance
	g_DBPROPickResult.iObjectID = 0;
	g_DBPROPickResult.fPickDistance = fDistance;
	g_DBPROPickResult.vecPickVector = vecTo - vecFrom;
	D3DXVec3Normalize ( &g_DBPROPickResult.vecPickVector, &g_DBPROPickResult.vecPickVector );
	g_DBPROPickResult.vecPickVector *= fDistance;
}

DARKSDK_DLL SDK_FLOAT GetPickDistance ( void )
{
	float fValue = g_DBPROPickResult.fPickDistance;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetPickVectorX ( void )
{
	float fValue = g_DBPROPickResult.vecPickVector.x;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetPickVectorY ( void )
{
	float fValue = g_DBPROPickResult.vecPickVector.y;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetPickVectorZ ( void )
{
	float fValue = g_DBPROPickResult.vecPickVector.z;
	return *(DWORD*)&fValue;
}

//////////////////////////////////////////////////////////////////////////////////
// EXPRESSIONS ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK_DLL int GetExist ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{ 
		RunTimeError ( RUNTIMEERROR_B3DMODELNUMBERILLEGAL );
		return 0;
	}
	if ( iID < g_iObjectListCount )
	{ 
		if ( g_ObjectList [ iID ] )
		{
			if ( g_ObjectList [ iID ]->pFrame )
			{
				return 1;
			}
			if ( g_ObjectList [ iID ]->pInstanceOfObject )
			{
				return 1;
			}
		}
	}
	return 0;
}

DARKSDK_DLL int GetVisible ( int iID )
{
	// check the object exists
//	if ( !ConfirmObject ( iID ) ) // 190506 - u61 - instances have visibility
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// return object information
// lee - 101004 - U5.8 added universe state to visibility result
// V111 - 110608 - universe visibility messes up visibility checks in engine
//	if ( g_ObjectList [ iID ]->bUniverseVisible==true
//	&&   g_ObjectList [ iID ]->bVisible==true )
	if ( g_ObjectList [ iID ]->bVisible==true )
		return 1;
	else
		return 0;		
}


DARKSDK_DLL SDK_FLOAT GetSizeEx ( int iID, int iActualSize )
{
    // check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// return object information
	sObject* pObject = g_ObjectList [ iID ];
	float fValue = pObject->collision.fRadius;

	// if zero, still NEED a dimension, so calc now
	if ( fValue==0.0f )
	{
		float fDX = fabs(pObject->collision.vecMax.x - pObject->collision.vecMin.x);
		float fDY = fabs(pObject->collision.vecMax.y - pObject->collision.vecMin.y);
		float fDZ = fabs(pObject->collision.vecMax.z - pObject->collision.vecMin.z);
		fValue = fDX;
		if ( fDY>fValue ) fValue = fDY;
		if ( fDZ>fValue ) fValue = fDZ;
	}

	// scale optional
	if ( iActualSize==1 )
	{
		//050803 - adjusts to averaged scale now
		float fAvScale = (pObject->position.vecScale.x+pObject->position.vecScale.y+pObject->position.vecScale.z)/3.0f;
		fValue *= fAvScale;
	}
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetXSizeEx ( int iID, int iActualSize )
{
    return GetAxisSizeFromVectorOffset( iID, iActualSize, 0 /* 0 = x part of vector */);
}

DARKSDK_DLL SDK_FLOAT GetYSizeEx ( int iID, int iActualSize )
{
    return GetAxisSizeFromVectorOffset( iID, iActualSize, 1 /* 1 = y part of vector */ );
}

DARKSDK_DLL SDK_FLOAT GetZSizeEx ( int iID, int iActualSize )
{
    return GetAxisSizeFromVectorOffset ( iID, iActualSize, 2 /* 2 = z part of vector */ );
}

DARKSDK_DLL SDK_FLOAT GetSize ( int iID )
{
	return GetSizeEx ( iID, 0 );
}

DARKSDK_DLL SDK_FLOAT GetXSize ( int iID )
{
	return GetXSizeEx ( iID, 0 );
}

DARKSDK_DLL SDK_FLOAT GetYSize ( int iID )
{
	return GetYSizeEx ( iID, 0 );
}

DARKSDK_DLL SDK_FLOAT GetZSize ( int iID )
{
	return GetZSizeEx ( iID, 0 );
}

DARKSDK_DLL SDK_FLOAT GetScaleX ( int iID )
{
    // check the object exists
	if ( !ConfirmObjectInstance ( iID ) ) return 0;
	sObject* pObject = g_ObjectList [ iID ];
	float fValue = pObject->position.vecScale[ 0 ] * 100;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetScaleY ( int iID )
{
    // check the object exists
	if ( !ConfirmObjectInstance ( iID ) ) return 0;
	sObject* pObject = g_ObjectList [ iID ];
	float fValue = pObject->position.vecScale[ 1 ] * 100;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetScaleZ ( int iID )
{
    // check the object exists
	if ( !ConfirmObjectInstance ( iID ) ) return 0;
	sObject* pObject = g_ObjectList [ iID ];
	float fValue = pObject->position.vecScale[ 2 ] * 100;
	return *(DWORD*)&fValue;
}

// Animation Expressions

DARKSDK_DLL int GetNumberOfFrames ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return 0;

	// leefix - 150503 - use better value, was pObject->pAnimationSet->ulLength;
	// return object information
	sObject* pObject = g_ObjectList [ iID ];
	return (int)pObject->fAnimTotalFrames;
}

DARKSDK_DLL int GetPlaying ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return 0;

	// return object information
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject->bAnimPlaying )
		return 1;
	else
		return 0;
}

DARKSDK_DLL int GetLooping ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return 0;

	// return object information
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject->bAnimLooping )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_FLOAT GetFrame ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return 0;

	// return object information
	sObject* pObject = g_ObjectList [ iID ];
	float fValue = pObject->fAnimFrame;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetFrameEx ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];
	float fValue = pObject->fAnimFrame;

	// can set an animation frame override per limb
	if ( pObject->pfAnimLimbFrame )
	{
		float fViewValue = pObject->pfAnimLimbFrame [ iLimbID ];
		if ( fViewValue >= 0.0f ) fValue = fViewValue;
	}

	// return object information
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetSpeed ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return 0;

	// return object information (leefix - 260604-u54-float conversion looses .000009)
	sObject* pObject = g_ObjectList [ iID ];
	float fValue = pObject->fAnimSpeed*100.00001f;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetInterpolation ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return 0;

	// return object information (leefix - 260604-u54-float conversion looses .000009)
	sObject* pObject = g_ObjectList [ iID ];
	float fValue = pObject->fAnimInterp*100.00001f;
	return *(DWORD*)&fValue;
}

// ScreenData Expressions
DARKSDK_DLL void DB_ObjectScreenData( sObject* pObject, int* x, int* y )
{
	D3DXVECTOR3 vecBob = pObject->position.vecPosition;

	// camera ptr
	tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );

	// get current camera transformation matrices
	D3DXMATRIX matTransform = m_Camera_Ptr->matView * m_Camera_Ptr->matProjection;

	// Transform object position from world-space to screen-space
	D3DXVec3TransformCoord(&vecBob, &vecBob, &matTransform);

	// Screen data
	*x=(int)((vecBob.x+1.0f)*(g_pGlob->iScreenWidth/2.0f));
	*y=(int)((1.0f-vecBob.y)*(g_pGlob->iScreenHeight/2.0f));

	// leefix - 280305 - adjust coordinates using viewport of the target camera
	float fVPWidth = m_Camera_Ptr->viewPort3D.Width;
	float fVPHeight = m_Camera_Ptr->viewPort3D.Height;
	float fRealX = ( fVPWidth / g_pGlob->iScreenWidth ) * (*x);
	float fRealY = ( fVPHeight / g_pGlob->iScreenHeight ) * (*y);
	*x = fRealX + m_Camera_Ptr->viewPort3D.X;
	*y = fRealY + m_Camera_Ptr->viewPort3D.Y;
}

DARKSDK_DLL int GetScreenX ( int iID )
{
	// check the object exists
//	if ( !ConfirmObject ( iID ) ) // 190506 - u61 - can get instanced objects
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// calculate screendata
	int x, y;
	sObject* pObject = g_ObjectList [ iID ];
	DB_ObjectScreenData ( pObject, &x, &y );
	return x;
}

DARKSDK_DLL int GetScreenY ( int iID )
{
	// check the object exists
//	if ( !ConfirmObject ( iID ) ) // 190506 - u61 - can get instanced objects
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// calculate screendata
	int x, y;
	sObject* pObject = g_ObjectList [ iID ];
	DB_ObjectScreenData ( pObject, &x, &y );
	return y;
}

DARKSDK_DLL int GetInScreen ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	sObject* pObject = g_ObjectList [ iID ];

    // u74b7 - Object centre needs to be adjusted by the mesh centre to give correct results
	D3DXVECTOR3 vBob = pObject->position.vecPosition + pObject->collision.vecCentre;

	// camera ptr
	tagCameraData* pCamera = (tagCameraData*)g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );

	// get current camera transformation matrices
	D3DXMATRIX matCamera = pCamera->matView * pCamera->matProjection;

    // u74b7 - Test whether the object 'sphere' in viewing frustum
    D3DXPLANE p_Planes[6];
    ExtractFrustumPlanes(p_Planes, matCamera);
    if (ContainsSphere(p_Planes, vBob, pObject->collision.fScaledRadius))
        return 1;
    else
        return 0;
}

// Collision Expressions

DARKSDK_DLL int GetCollision ( int iObjectA, int iObjectB )
{
	return CheckCol ( iObjectA, iObjectB );
}

DARKSDK_DLL int GetHit ( int iObjectA, int iObjectB )
{
	return CheckHit ( iObjectA, iObjectB );
}

DARKSDK_DLL int GetLimbCollision ( int iObjectA, int iLimbA, int iObjectB, int iLimbB )
{
	return CheckLimbCol ( iObjectA, iLimbA, iObjectB, iLimbB );
}

DARKSDK_DLL int GetLimbHit ( int iObjectA, int iLimbA, int iObjectB, int iLimbB )
{
	return CheckLimbHit ( iObjectA, iLimbA, iObjectB, iLimbB );
}

DARKSDK_DLL SDK_FLOAT GetObjectCollisionRadius ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return 0;

	// return object information
	sObject* pObject = g_ObjectList [ iID ];
	float fValue = GetColRadius ( pObject );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetObjectCollisionCenterX ( int iID )
{
	// check the object exists
	//if ( !ConfirmObject ( iID ) )
	//	return 0;

	// mike - 120307 - allow instanced objects
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// return object information
	sObject* pObject = g_ObjectList [ iID ];
	float fValue = GetColCenterX ( pObject );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetObjectCollisionCenterY ( int iID )
{
	// check the object exists
	//if ( !ConfirmObject ( iID ) )
	//	return 0;

	// mike - 120307 - allow instanced objects
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// return object information
	sObject* pObject = g_ObjectList [ iID ];
	float fValue = GetColCenterY ( pObject );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetObjectCollisionCenterZ ( int iID )
{
	// check the object exists
	//if ( !ConfirmObject ( iID ) )
	//	return 0;

	// mike - 120307 - allow instanced objects
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// return object information
	sObject* pObject = g_ObjectList [ iID ];
	float fValue = GetColCenterZ ( pObject );
	return *(DWORD*)&fValue;
}

DARKSDK_DLL int GetStaticHit (	float fOldX1, float fOldY1, float fOldZ1, float fOldX2, float fOldY2, float fOldZ2,
					float fNX1,   float fNY1,   float fNZ1,   float fNX2,   float fNY2,   float fNZ2    )
{
	return GetStaticHitEx (	fOldX1, fOldY1, fOldZ1, fOldX2, fOldY2, fOldZ2,
							fNX1,   fNY1,   fNZ1,   fNX2,   fNY2,   fNZ2    );
}

DARKSDK_DLL int GetStaticLineOfSight ( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz, float fWidth, float fAccuracy )
{
	return GetStaticLineOfSightEx( fSx, fSy, fSz, fDx, fDy, fDz, fWidth, fAccuracy );
}

DARKSDK_DLL int GetStaticRayCast ( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz )
{
	// returns a one if collision, details in checklist
	if ( g_pUniverse )
	{
		return g_pUniverse->RayCast ( fSx, fSy, fSz, fDx, fDy, fDz );
	}
	else
		return 0;
}

DARKSDK_DLL int GetStaticVolumeCast ( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ, float fSize )
{
	if ( g_pUniverse )
	{
		if ( g_pUniverse->RayVolume ( fX, fY, fZ, fNewX, fNewY, fNewZ, fSize )==true)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

DARKSDK_DLL SDK_FLOAT GetStaticX ( void )
{
	float result = GetStaticColX();
	return *(DWORD*)&result;
}

DARKSDK_DLL SDK_FLOAT GetStaticY ( void )
{
	float result = GetStaticColY();
	return *(DWORD*)&result;
}

DARKSDK_DLL SDK_FLOAT GetStaticZ ( void )
{
	float result = GetStaticColZ();
	return *(DWORD*)&result;
}

DARKSDK_DLL int GetStaticFloor ( void )
{
	return GetCollidedAgainstFloor();
}

DARKSDK_DLL int GetStaticColCount ( void )
{
	return GetStaticColPolysChecked();
}

DARKSDK_DLL int GetStaticColValue ( void )
{
	return (int)GetStaticColArbitaryValue();
}

DARKSDK_DLL SDK_FLOAT GetStaticLineOfSightX ( void )
{
	float result = GetStaticLineOfSightExX();
	return *(DWORD*)&result;
}

DARKSDK_DLL SDK_FLOAT GetStaticLineOfSightY ( void )
{
	float result = GetStaticLineOfSightExY();
	return *(DWORD*)&result;
}

DARKSDK_DLL SDK_FLOAT GetStaticLineOfSightZ ( void )
{
	float result = GetStaticLineOfSightExZ();
	return *(DWORD*)&result;
}

DARKSDK_DLL SDK_FLOAT GetCollisionX ( void )
{
	float result = GetColX();
	return *(DWORD*)&result;
}

DARKSDK_DLL SDK_FLOAT GetCollisionY ( void )
{
	float result = GetColY();
	return *(DWORD*)&result;
}

DARKSDK_DLL SDK_FLOAT GetCollisionZ ( void )
{
	float result = GetColZ();
	return *(DWORD*)&result;
}

// Limb Expressions

DARKSDK_DLL int GetLimbExist ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// check limb range
	if ( iLimbID < 0 || iLimbID > MAXIMUMVALUE )
	{ 
		RunTimeError ( RUNTIMEERROR_LIMBNUMBERILLEGAL );
		return 0;
	}

	// actual object or instance of object
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject->pInstanceOfObject )
	{
		// record hide state in instance-mesh-visibility-array
		sObject* pActualObject = pObject->pInstanceOfObject;
		if ( iLimbID>=0 && iLimbID<pActualObject->iFrameCount)
			return 1;
		else
			return 0;
	}
	else
	{
		if ( iLimbID < g_ObjectList [ iID ]->iFrameCount )
			return 1;
		else
			return 0;
	}
}

DARKSDK_DLL SDK_FLOAT GetLimbOffsetX ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iID ];
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	return *(DWORD*)&pFrame->vecOffset.x;
}

DARKSDK_DLL SDK_FLOAT GetLimbOffsetY ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iID ];
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	return *(DWORD*)&pFrame->vecOffset.y;
}

DARKSDK_DLL SDK_FLOAT GetLimbOffsetZ ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iID ];
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	return *(DWORD*)&pFrame->vecOffset.z;
}

DARKSDK_DLL SDK_FLOAT GetLimbAngleX ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iID ];
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	return *(DWORD*)&pFrame->vecRotation.x;
}

DARKSDK_DLL SDK_FLOAT GetLimbAngleY ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iID ];
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	return *(DWORD*)&pFrame->vecRotation.y;
}

DARKSDK_DLL SDK_FLOAT GetLimbAngleZ ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iID ];
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	return *(DWORD*)&pFrame->vecRotation.z;
}

DARKSDK_DLL SDK_FLOAT GetLimbXPosition ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimbInstance ( iID, iLimbID ) )
		return 0;

	// actual or instanced
	sObject* pObject = g_ObjectList [ iID ];
	sObject* pActualObject = pObject;
	if ( pObject->pInstanceOfObject )
		pActualObject = pObject->pInstanceOfObject;

	// get frame of object
	sFrame* pFrame = pActualObject->ppFrameList [ iLimbID ];
	if ( pObject->pInstanceOfObject ) pFrame->bVectorsCalculated = false;

	// specific limb/frame information
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	float fValue = pFrame->vecPosition.x;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetLimbYPosition ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimbInstance ( iID, iLimbID ) )
		return 0;

	// actual or instanced
	sObject* pObject = g_ObjectList [ iID ];
	sObject* pActualObject = pObject;
	if ( pObject->pInstanceOfObject )
		pActualObject = pObject->pInstanceOfObject;

	// get frame of object
	sFrame* pFrame = pActualObject->ppFrameList [ iLimbID ];
	if ( pObject->pInstanceOfObject ) pFrame->bVectorsCalculated = false;

	// specific limb/frame information
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	float fValue = pFrame->vecPosition.y;
	return *(DWORD*)&fValue;

}

DARKSDK_DLL SDK_FLOAT GetLimbZPosition ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimbInstance ( iID, iLimbID ) )
		return 0;

	// actual or instanced
	sObject* pObject = g_ObjectList [ iID ];
	sObject* pActualObject = pObject;
	if ( pObject->pInstanceOfObject )
		pActualObject = pObject->pInstanceOfObject;

	// get frame of object
	sFrame* pFrame = pActualObject->ppFrameList [ iLimbID ];
	if ( pObject->pInstanceOfObject ) pFrame->bVectorsCalculated = false;

	// specific limb/frame information
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	float fValue = pFrame->vecPosition.z;
	return *(DWORD*)&fValue;

}

DARKSDK_DLL SDK_FLOAT GetLimbXDirection ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iID ];
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	float fValue = pFrame->vecDirection.x;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetLimbYDirection ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iID ];
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	float fValue = pFrame->vecDirection.y;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetLimbZDirection ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iID ];
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	float fValue = pFrame->vecDirection.z;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetLimbScaleX ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iID ];
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	float fValue = pFrame->vecScale.x*100.0f;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetLimbScaleY ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iID ];
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	float fValue = pFrame->vecScale.y*100.0f;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL SDK_FLOAT GetLimbScaleZ ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// specific limb/frame information
	sObject* pObject = g_ObjectList [ iID ];
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	UpdateRealtimeFrameVectors ( pObject, pFrame );
	float fValue = pFrame->vecScale.z*100.0f;
	return *(DWORD*)&fValue;
}

DARKSDK_DLL int GetLimbTextureEx ( int iID, int iLimbID, int iTextureStage )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// ensure limb has mesh
	sMesh* pMesh = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->pMesh;
	if ( !pMesh )
		return 0;

	// Ensure iTextureStage is valid
	if ( iTextureStage < 0 && iTextureStage >= (int)pMesh->dwTextureCount )
		return NULL;

	// return stage zero texture image
	if ( pMesh->pTextures )
		return pMesh->pTextures [ iTextureStage ].iImageID;

	// no texture
	return 0;
}

DARKSDK_DLL int GetLimbTexture ( int iID, int iLimbID )
{
	return GetLimbTextureEx ( iID, iLimbID, 0 );
}

DARKSDK_DLL int GetLimbTexturePtr ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// ensure limb has mesh
	sMesh* pMesh = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->pMesh;
	if ( !pMesh )
		return 0;

	// return ptr as int (for determining texture matching)
	int iPtrValue = 0;
	if ( pMesh->pTextures )
		iPtrValue = (int)pMesh->pTextures [ 0 ].pTexturesRef;
	return iPtrValue;
}

DARKSDK_DLL int GetLimbVisible ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimbInstance ( iID, iLimbID ) )
		return 0;

	// actual object or instance of object
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject->pInstanceMeshVisible )
	{
		// record hide state in instance-mesh-visibility-array
		sObject* pActualObject = pObject->pInstanceOfObject;
		if ( iLimbID>=0 && iLimbID<pActualObject->iFrameCount)
		{
			if ( pObject->pInstanceMeshVisible [ iLimbID ] )
				return 1;
			else
				return 0;
		}
		else
			return 0;
	}
	else
	{
		// ensure limb has mesh
		sMesh* pMesh = g_ObjectList [ iID ]->ppFrameList [ iLimbID ]->pMesh;
		if ( !pMesh )
			return 0;

		// specific limb/frame information
		if ( pMesh->bVisible )
			return 1;
		else
			return 0;
	}
}

DARKSDK_DLL int GetLimbLink ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return 0;

	// can always modify links in limbs now
	return 1;
}

DARKSDK_DLL int GetLimbPolygonCount ( int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimbInstance ( iID, iLimbID ) )
		return 0;

	// actual object
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject==NULL ) return 0;

	// get frame from object
	sFrame** pFrameList = pObject->ppFrameList;
	if ( pFrameList==NULL ) return 0;
	if ( iLimbID>=pObject->iFrameCount ) return 0;

	// get mesh from object
	sMesh* pMesh = pFrameList[ iLimbID ]->pMesh;
	if ( pMesh==NULL ) return 0;

	// return polygon count for this limb
	return pMesh->iDrawPrimitives;
}

DARKSDK_DLL int GetMultiMaterialCount ( int iID )
{
	// check the object exists
	if ( !ConfirmObjectInstance ( iID ) )
		return 0;

	// actual object
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject==NULL ) return 0;

	// add all multiaterial counts
	if ( pObject->pInstanceOfObject ) pObject = pObject->pInstanceOfObject;
	DWORD dwTotalMaterialCount = 0;
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		sMesh* pMesh = pObject->ppMeshList [ iMesh ];
		if ( pMesh )
			if ( pMesh->bUseMultiMaterial==true )
				dwTotalMaterialCount+=pMesh->dwMultiMaterialCount;
	}

	// return total
	return dwTotalMaterialCount;
}

DARKSDK_DLL SDK_LPSTR GetLimbTextureNameEx ( SDK_RETSTR int iID, int iLimbID, int iTextureStage )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return NULL;

	// Free old string
	if(lpStr) g_pGlob->CreateDeleteString((DWORD*)&lpStr, 0);

	// texture name to return
	LPSTR pTextureLimbName = NULL;

	// get ptrs
	sObject* pObject = g_ObjectList [ iID ];
	sMesh* pMesh = pObject->ppFrameList [ iLimbID ]->pMesh;
	if ( pMesh )
	{
		// lee - 220306 - u6b4 - if multimaterial, texturestage becomes index to material
		if ( pMesh->dwMultiMaterialCount > 0 && pMesh->pMultiMaterial )
		{
			// ensure materiual inde xis valid
			int iMaterialIndex = iTextureStage ; // re-use
			//if ( iMaterialIndex < 0 && iMaterialIndex >= (int)pMesh->dwMultiMaterialCount ) // leefix - 210806 - fixed condition
			if ( iMaterialIndex < 0 || iMaterialIndex >= (int)pMesh->dwMultiMaterialCount )
			{
				// skip
				pTextureLimbName = "";
			}
			else
			{
				// get name of texturename of material in framemesh
				pTextureLimbName = pMesh->pMultiMaterial [ iMaterialIndex ].pName;
			}
		}
		else
		{
			// Ensure iTextureStage is valid
			//if ( iTextureStage < 0 && iTextureStage >= (int)pMesh->dwTextureCount ) // leefix - 210806 - fixed condition
			if ( iTextureStage < 0 || iTextureStage >= (int)pMesh->dwTextureCount )
			{
				// skip
				pTextureLimbName = "";
			}
			else
			{
				// get name of texture in framemesh
				pTextureLimbName = "";
				if ( pMesh->pTextures )
					pTextureLimbName = pMesh->pTextures [ iTextureStage ].pName;
			}
		}
	}
	else
		pTextureLimbName = "";

	// Allocate new size
	LPSTR pString = NULL;
	DWORD dwSize = strlen ( pTextureLimbName );
	g_pGlob->CreateDeleteString((DWORD*)&pString, dwSize+1);
	ZeroMemory ( pString, dwSize+1 );
	memcpy ( pString, pTextureLimbName, dwSize );

	// Return String
	return (DWORD)pString;
}

DARKSDK_DLL SDK_LPSTR GetLimbTextureName ( SDK_RETSTR int iID, int iLimbID )
{
	#ifdef SDK_RETSTR
	 return GetLimbTextureNameEx ( lpStr, iID, iLimbID, 0 );
	#else
	 return GetLimbTextureNameEx ( iID, iLimbID, 0 );
	#endif
}

DARKSDK_DLL SDK_LPSTR GetLimbName ( SDK_RETSTR int iID, int iLimbID )
{
	// check the object exists
	if ( !ConfirmObjectAndLimb ( iID, iLimbID ) )
		return NULL;

	// Free old string
	if(lpStr) g_pGlob->CreateDeleteString((DWORD*)&lpStr, 0);

	// get name of frame
	sObject* pObject = g_ObjectList [ iID ];
	LPSTR pLimbName = pObject->ppFrameList [ iLimbID ]->szName;

	// Allocate new size
	LPSTR pString = NULL;
	DWORD dwSize = strlen ( pLimbName );
	g_pGlob->CreateDeleteString((DWORD*)&pString, dwSize+1);
	ZeroMemory ( pString, dwSize+1 );
	memcpy ( pString, pLimbName, dwSize );

	// Return String
	return (DWORD)pString;
}

DARKSDK_DLL int GetLimbCount ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return 0;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	// specific limb/frame information
	return pObject->iFrameCount;
}

// Mesh Expressions

DARKSDK_DLL int GetMeshExist ( int iID )
{
	if ( iID < g_iRawMeshListCount )
		if ( g_RawMeshList [ iID ] )
			return 1;

	return 0;
}

// Shader Expressions

DARKSDK_DLL SDK_BOOL PixelShaderExist ( int iShader )
{
	// if shader value valid
	if ( iShader < 0 || iShader > MAX_VERTEX_SHADERS )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERNUMBERILLEGAL);
		return 0;
	}

	// if shader exists
	if ( m_PixelShaders [ iShader ].pPixelShader )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL VertexShaderExist ( int iShader )
{
	// if shader value valid
	if ( iShader < 0 || iShader > MAX_VERTEX_SHADERS )
	{
		RunTimeError(RUNTIMEERROR_B3DVSHADERNUMBERILLEGAL);
		return 0;
	}

	// if shader exists
	if ( m_VertexShaders [ iShader ].pVertexShader )
		return 1;
	else
		return 0;
}

// Caps Expressions

DARKSDK_DLL int Get3DBLITAvailable ( void )
{
	// OBSOLETE
	return 1;
}

DARKSDK_DLL int GetStatistic ( int iCode )
{
	// Active for hidden-values
	switch(iCode)
	{
		case 1 :	// Polycount monitored from scene geometry
					if ( g_pGlob )
						return g_pGlob->dwNumberOfPolygonsDrawn;
					else
						return 0;

		case 2 :	// Stencil buffer available in current mode
					if(SupportsStencilBuffer())
						return 1;
					else
						return 0;

		case 3:		// Current Universe Area Box Of Camera Zero
					return g_iAreaBox;

		case 4:		// Total Number Of Universe Area Boxes
					return g_iAreaBoxCount;

		case 5 :	// DrawPrimitive calls monitored from scene geometry
					if ( g_pGlob )
						return g_pGlob->dwNumberOfPrimCalls;
					else
						return 0;

		case 6 :	// Polygons from current areabox (universe)
					if ( g_pUniverse )
						return g_pUniverse->m_dwNumberOfCurrentAreaBoxPolygonsDrawn;
					else
						return 0;

		case 7:		// DrawPrimitive calls from current areabox (universe)
					if ( g_pUniverse )
						return g_pUniverse->m_dwNumberOfCurrentAreaBoxPrimCalls;
					else
						return 0;

		case 8:		// Polygons tested for collision
					if ( g_pUniverse )
					{
						DWORD dwCount = g_pUniverse->m_dwPolygonsForCollision;
						g_pUniverse->m_dwPolygonsForCollision=0;
						return dwCount;
					}
					else
						return 0;

		case 9:		// Volumes tested for collision
					if ( g_pUniverse )
					{
						DWORD dwCount = g_pUniverse->m_dwVolumesTestedForCollision;
						g_pUniverse->m_dwVolumesTestedForCollision=0;
						return dwCount;
					}
					else
						return 0;	

		case 10:	// Occluded Objects
					if ( g_pUniverse )
					{
						DWORD dwCount = g_Occlusion.GetOccludedObjects();
						return dwCount;
					}
					else
						return 0;	
	}
	return 0;
}

DARKSDK_DLL int GetTNLAvailable ( void )
{
	// OBSOLETE
	return 1;
}

DARKSDK_DLL int GetAlphaBlending ( void )
{
	if ( m_Caps.PrimitiveMiscCaps & D3DPMISCCAPS_BLENDOP )
		return 1;
	else
		return 0;
}

DARKSDK_DLL int GetBlending ( void )
{
	if ( m_Caps.LineCaps & D3DLINECAPS_BLEND )
		return 1;
	else
		return 0;
}

DARKSDK_DLL int GetFog ( void )
{
	if ( m_Caps.LineCaps & D3DLINECAPS_FOG )
		return 1;
	else
		return 0;
}

DARKSDK_DLL int GetDeviceType ( void )
{
	return ( int ) m_Caps.DeviceType;
}

DARKSDK_DLL SDK_BOOL GetCalibrateGamma ( void )
{
	if ( m_Caps.Caps2 & D3DCAPS2_CANCALIBRATEGAMMA )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetRenderWindowed ( void )
{
	// dx8->dx9
//	if ( m_Caps.Caps2 & D3DCAPS2_CANRENDERWINDOWED )
	return 1;
//	else
//	return 0;
}

DARKSDK_DLL SDK_BOOL GetFullScreenGamma ( void )
{
	if ( m_Caps.Caps2 & D3DCAPS2_FULLSCREENGAMMA )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetBlitSysOntoLocal ( void )
{
	if ( m_Caps.DevCaps & D3DDEVCAPS_CANBLTSYSTONONLOCAL )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetRenderAfterFlip ( void )
{
	if ( m_Caps.DevCaps & D3DDEVCAPS_CANRENDERAFTERFLIP )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetDrawPrimTLVertex ( void )
{
	if ( m_Caps.DevCaps & D3DDEVCAPS_DRAWPRIMTLVERTEX )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetHWTransformAndLight ( void )
{
	if ( m_Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetSeparateTextureMemories ( void )
{
	if ( m_Caps.DevCaps & D3DDEVCAPS_SEPARATETEXTUREMEMORIES )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetTextureSystemMemory ( void )
{
	if ( m_Caps.DevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetTextureVideoMemory ( void )
{
	if ( m_Caps.DevCaps & D3DDEVCAPS_TEXTUREVIDEOMEMORY )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetTextureNonLocalVideoMemory ( void )
{
	if ( m_Caps.DevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetTLVertexSystemMemory ( void )
{
	if ( m_Caps.DevCaps & D3DDEVCAPS_TLVERTEXSYSTEMMEMORY )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetTLVertexVideoMemory ( void )
{
	if ( m_Caps.DevCaps & D3DDEVCAPS_TLVERTEXVIDEOMEMORY )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetClipAndScalePoints ( void )
{
	if ( m_Caps.PrimitiveMiscCaps & D3DPMISCCAPS_CLIPPLANESCALEDPOINTS )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetClipTLVerts ( void )
{
	if ( m_Caps.PrimitiveMiscCaps & D3DPMISCCAPS_CLIPTLVERTS )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetColorWriteEnable ( void )
{
	if ( m_Caps.PrimitiveMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetCullCCW ( void )
{
	if ( m_Caps.PrimitiveMiscCaps & D3DPMISCCAPS_CULLCCW )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetCullCW ( void )
{
	if ( m_Caps.PrimitiveMiscCaps & D3DPMISCCAPS_CULLCW )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetAnisotropicFiltering ( void )
{
	if ( m_Caps.PrimitiveMiscCaps & D3DPRASTERCAPS_ANISOTROPY )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetAntiAliasEdges ( void )
{
	// dx8->dx9
//	if ( m_Caps.RasterCaps & D3DPRASTERCAPS_ANTIALIASEDGES )
	return 1;
//	else
//	return 0;
}

DARKSDK_DLL SDK_BOOL GetColorPerspective ( void )
{
	if ( m_Caps.RasterCaps & D3DPRASTERCAPS_COLORPERSPECTIVE )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetDither ( void )
{
	if ( m_Caps.RasterCaps & D3DPRASTERCAPS_DITHER )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetFogRange ( void )
{
	if ( m_Caps.RasterCaps & D3DPRASTERCAPS_FOGRANGE )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetFogTable ( void )
{
	if ( m_Caps.RasterCaps & D3DPRASTERCAPS_FOGTABLE )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetFogVertex ( void )
{
	if ( m_Caps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetMipMapLODBias ( void )
{
	if ( m_Caps.RasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetWBuffer ( void )
{
	if ( m_Caps.RasterCaps & D3DPRASTERCAPS_WBUFFER )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetWFog ( void )
{
	if ( m_Caps.RasterCaps & D3DPRASTERCAPS_WFOG )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetZFog ( void )
{
	if ( m_Caps.RasterCaps & D3DPRASTERCAPS_ZFOG )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetAlpha ( void )
{
	if ( m_Caps.TextureCaps & D3DPTEXTURECAPS_ALPHA )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetCubeMap ( void )
{
	if ( m_Caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetMipCubeMap ( void )
{
	if ( m_Caps.TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetMipMap ( void )
{
	if ( m_Caps.TextureCaps & D3DPTEXTURECAPS_MIPMAP )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetMipMapVolume ( void )
{
	if ( m_Caps.TextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetNonPowerOf2Textures ( void )
{
	if ( m_Caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetPerspective ( void )
{
	if ( m_Caps.TextureCaps & D3DPTEXTURECAPS_PERSPECTIVE )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetProjected ( void )
{
	if ( m_Caps.TextureCaps & D3DPTEXTURECAPS_PROJECTED )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetSquareOnly ( void )
{
	if ( m_Caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetVolumeMap ( void )
{
	if ( m_Caps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetAlphaComparision ( void )
{
	if ( m_Caps.LineCaps & D3DLINECAPS_ALPHACMP )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetTexture ( void )
{
	if ( m_Caps.LineCaps & D3DLINECAPS_TEXTURE )
		return 1;
	else
		return 0;
}

DARKSDK_DLL SDK_BOOL GetZBuffer ( void )
{
	if ( m_Caps.LineCaps & D3DLINECAPS_ZTEST )
		return 1;
	else
		return 0;
}

DARKSDK_DLL int GetMaxTextureWidth ( void )
{
	return m_Caps.MaxTextureWidth;
}

DARKSDK_DLL int GetMaxTextureHeight ( void )
{
	return m_Caps.MaxTextureHeight;
}

DARKSDK_DLL int GetMaxVolumeExtent ( void )
{
	return m_Caps.MaxVolumeExtent;
}

DARKSDK_DLL SDK_FLOAT GetVertexShaderVersion ( void )
{
	float fVersion = 0.0f;
	if(m_Caps.MaxStreams>0)
	{
		unsigned char sub = *((LPSTR)(&m_Caps.VertexShaderVersion));
		fVersion = *((LPSTR)(&m_Caps.VertexShaderVersion)+1);
		fVersion += ((float)sub/10.0f);
	}
	return SDK_RETFLOAT ( fVersion );
}

DARKSDK_DLL SDK_FLOAT GetPixelShaderVersion ( void )
{
	float fVersion = 0.0f;
	if(m_Caps.MaxStreams>0)
	{
		unsigned char sub = *((LPSTR)(&m_Caps.PixelShaderVersion));
		fVersion = *((LPSTR)(&m_Caps.PixelShaderVersion)+1);
		fVersion += ((float)sub/10.0f);
	}
	return SDK_RETFLOAT ( fVersion );
}

DARKSDK_DLL int GetMaxVertexShaderConstants ( void )
{
	return m_Caps.MaxVertexShaderConst;
}

DARKSDK_DLL SDK_FLOAT GetMaxPixelShaderValue ( void )
{
	return SDK_RETFLOAT ( m_Caps.PixelShader1xMaxValue );
}

DARKSDK_DLL int GetMaxLights ( void )
{
	return m_Caps.MaxActiveLights;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

DARKSDK_DLL void AddObjectToLightMapPool ( int iID )
{
	g_LightMaps.AddObject ( g_ObjectList [ iID ] );
}

DARKSDK_DLL void AddLimbToLightMapPool ( int iID, int iLimb )
{
	g_LightMaps.AddLimb ( g_ObjectList [ iID ], iLimb );
}

DARKSDK_DLL void AddStaticObjectsToLightMapPool ( void )
{
	g_LightMaps.AddUniverse ( g_pUniverse );
}

DARKSDK_DLL void AddLightMapLight ( float fX, float fY, float fZ, float fRadius, float fRed, float fGreen, float fBlue, float fBrightness, SDK_BOOL bCastShadowOrig )
{
	bool bCastShadow = false;
	if ( bCastShadowOrig==1 ) bCastShadow = true;
	g_LightMaps.AddLight ( fX, fY, fZ, fRadius, fRed, fGreen, fBlue, fBrightness, bCastShadow );
}

DARKSDK_DLL void FlushLightMapLights ( void )
{
	g_LightMaps.FlushAll ( );
}

DARKSDK_DLL void CreateLightMaps ( int iLMSize, int iLMQuality, SDK_LPSTR dwPathForLightMaps )
{
	g_LightMaps.Create ( iLMSize, iLMQuality, (LPSTR)dwPathForLightMaps );
}

//
// Shadows
//

DARKSDK_DLL void SetGlobalShadowsOn ( void )
{
	m_ObjectManager.SetGlobalShadowsOn();
}

DARKSDK_DLL void SetGlobalShadowsOff ( void )
{
	m_ObjectManager.SetGlobalShadowsOff();
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

// mike 090903 - now uses a pointer
DARKSDK_DLL void SetWorldMatrix ( int iID, D3DXMATRIX* pMatrix )
{
	if ( !ConfirmObject ( iID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];
	memcpy ( &pObject->position.matWorld, pMatrix, sizeof ( D3DXMATRIX ) );
	pObject->position.bCustomWorldMatrix = true;
}

// mike - 040903 - updates a structure
DARKSDK_DLL void UpdatePositionStructure ( sPositionData* pPosition )
{
	sObject object;

	object.position = *pPosition;

	CalculateObjectWorld ( &object, NULL );
}

DARKSDK_DLL void GetWorldMatrix ( int iID, int iLimb, D3DXMATRIX* pMatrix )
{
	if ( !ConfirmObject ( iID ) )
		return;

	sObject* pObject = g_ObjectList [ iID ];

	CalculateObjectWorld ( pObject, NULL );

	//CalculateAbsoluteWorldMatrix ( pObject, pObject->ppFrameList [ iLimb ], pObject->ppFrameList [ iLimb ]->pMesh );

	*pMatrix = pObject->ppFrameList [ iLimb ]->matCombined * pObject->position.matWorld;
}

DARKSDK_DLL D3DXVECTOR3 GetCameraLook ( void )
{
	tagCameraData* pCameraData = ( tagCameraData* ) g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );

	return pCameraData->vecLook;
}

DARKSDK_DLL D3DXVECTOR3 GetCameraPosition ( void )
{
	tagCameraData* pCameraData = ( tagCameraData* ) g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );

	return pCameraData->vecPosition;
}

DARKSDK_DLL D3DXVECTOR3 GetCameraUp ( void )
{
	tagCameraData* pCameraData = ( tagCameraData* ) g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );

	return pCameraData->vecUp;
}

DARKSDK_DLL D3DXVECTOR3 GetCameraRight ( void )
{
	tagCameraData* pCameraData = ( tagCameraData* ) g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );

	return pCameraData->vecRight;
}

DARKSDK_DLL D3DXMATRIX GetCameraMatrix ( void )
{
	tagCameraData* pCameraData = ( tagCameraData* ) g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );

	return pCameraData->matView;
}

DARKSDK_DLL void ExcludeOn ( int iID )
{
	if ( !CheckObjectExist ( iID ) )
		return;

	sObject* pObject = g_ObjectList [ iID ];
	pObject->bExcluded = true;
	if ( pObject->pInstanceOfObject ) pObject->pInstanceOfObject->bExcluded = true; // 131107 - added as seemd to be missing?

	m_ObjectManager.UpdateTextures ( );
}

DARKSDK_DLL void ExcludeOff ( int iID )
{
	if ( !CheckObjectExist ( iID ) )
		return;

	sObject* pObject = g_ObjectList [ iID ];
	pObject->bExcluded = false;
	if ( pObject->pInstanceOfObject ) pObject->pInstanceOfObject->bExcluded = false;

	m_ObjectManager.UpdateTextures ( );
}

DARKSDK_DLL void ExcludeLimbOn ( int iID, int iLimbID )
{
	// 301007 - new command
	if ( !CheckObjectExist ( iID ) )
		return;

	// exclude limb if exists
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject )
		if ( pObject->ppFrameList )
			if ( iLimbID < pObject->iFrameCount )
				if ( pObject->ppFrameList [ iLimbID ] )
					pObject->ppFrameList [ iLimbID ]->bExcluded = true;
}

DARKSDK_DLL void ExcludeLimbOff	( int iID, int iLimbID )
{
	// 301007 - new command
	if ( !CheckObjectExist ( iID ) )
		return;

	// exclude limb if exists
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject )
		if ( pObject->ppFrameList )
			if ( iLimbID < pObject->iFrameCount )
				if ( pObject->ppFrameList [ iLimbID ] )
					pObject->ppFrameList [ iLimbID ]->bExcluded = false;
}

DARKSDK_DLL void SetGlobalObjectCreationMode ( int iMode )
{
    // If bit 1 isn't set, then allow individual buffers
    g_bGlobalVBIBUsageFlag = ! (iMode & 1);

    // Set the rendering order
	switch ( iMode )
    {
    // By Texture
    case 0:
    case 1:
        g_eGlobalSortOrder = E_SORT_BY_TEXTURE;
		break;

	// No particular order
    case 2:
    case 3:
        g_eGlobalSortOrder = E_SORT_BY_NONE;
		break;

	// By Object number
    case 4:
    case 5:
        g_eGlobalSortOrder = E_SORT_BY_OBJECT;
		break;

	// By Reverse Distance
    case 6:
    case 7:
        g_eGlobalSortOrder = E_SORT_BY_DEPTH;
		break;

    // Ignore anything else
    default:
        break;
	}
	m_ObjectManager.UpdateTextures ( );
}

DARKSDK_DLL sObject* GetObject ( int iID )
{
	// MIKE - 050104 - function to access an object
//	if ( !ConfirmObject ( iID ) ) // 180506 - u61 - tpc can get instanced objects
//	if ( !ConfirmObjectInstance ( iID ) )
//		return NULL;

	// lee - 040914 - needs to be silent fail
	if ( iID < 1 || iID > MAXIMUMVALUE )
		return NULL;

	if ( iID < g_iObjectListCount )
		return g_ObjectList [ iID ];
	else
		return NULL;
}

// mike - 080305 - function to extract the universe object
DARKSDK_DLL void GetUniverseMeshList ( vector < sMesh* > *pMeshList )
{
	if ( g_pUniverse )
	{
		g_pUniverse->GetMasterMeshList ( pMeshList );
	}
}


// mike - 230505 - need to be able to set mip map LOD bias on a per mesh basis
void SetObjectMipMapLODBias	( int iID, int iLimb, float fBias )
{
	// ensure the object is present
	if ( !ConfirmObject ( iID ) )
		return;

	// get object ptr
	sObject* pObject = g_ObjectList [ iID ];

	if ( iLimb == -1 )
	{
		for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
			pObject->ppMeshList [ iMesh ]->fMipMapLODBias = fBias;

		return;
	}

	if ( iLimb >= pObject->iMeshCount || iLimb < 0 )
		return;

	pObject->ppMeshList [ iLimb ]->fMipMapLODBias = fBias;
}

// mike - 230505 - need to be able to set mip map LOD bias on a per mesh basis
void SetObjectMipMapLODBias	( int iID, float fBias )
{
	SetObjectMipMapLODBias ( iID, -1, fBias );
}

// mike - 300905 - command to update object bounds
void CalculateObjectBounds ( int iID )
{
	// check the object limb exists
	if ( !ConfirmObject ( iID ) )
		return;

	CalculateAllBounds ( g_ObjectList [ iID ], false );
}

void CalculateObjectBoundsEx ( int iID, int iOnlyUpdateFrames )
{
	// check the object limb exists
	if ( !ConfirmObject ( iID ) )
		return;

	// only update frame matices OR update ALL BOUNDS (performance intense)
	if ( iOnlyUpdateFrames==1 )
	{
		// get object ptr
		sObject* pObject = g_ObjectList [ iID ];

		// store current animation states
		bool bStoreAnimPlaying = pObject->bAnimPlaying;
		float bStoreAnimFrame = pObject->fAnimFrame;
		bool bStoreAnimLooping = pObject->bAnimLooping;
		float bStoreAnimSlerpTime = pObject->fAnimSlerpTime;
		bool bStoreAnimManualSlerp = pObject->bAnimManualSlerp;

		// advance animation frame (only done at SYNC normally)
		// NOTE: This also updates the latest limb bounds for FAST ACCURATE limb detection :)
		if ( pObject->bExcluded==false && pObject->pAnimationSet )
		{
			// conditions moved outside of function to speed up (CPU 'call' is more expensive)
			m_ObjectManager.UpdateAnimationCyclePerObject ( pObject );
		}

		// simple update
		D3DXMATRIX matrix;
		D3DXMatrixIdentity ( &matrix );
		UpdateAllFrameData ( pObject, pObject->fAnimFrame );
		UpdateFrame ( pObject->pFrame, &matrix );

		// restore animation states
		pObject->bAnimPlaying = bStoreAnimPlaying;
		pObject->fAnimFrame = bStoreAnimFrame;
		pObject->bAnimLooping = bStoreAnimLooping;
		pObject->fAnimSlerpTime = bStoreAnimSlerpTime;
		pObject->bAnimManualSlerp = bStoreAnimManualSlerp;
	}
	else
	{
		// full bounds box/sphere calculation
		CalculateAllBounds ( g_ObjectList [ iID ], false );
	}
}

// lee - 140108 - x10 compat.
DARKSDK void SetMask					( int iID, int iMASK, int iShadowMASK )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void SetMask					( int iID, int iMASK, int iShadowMASK, int iCubeMapMASK )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void SetMask ( int iID, int iMASK, int iShadowMASK, int iCubeMapMASK, int iForeColorWipe )
{
	// check the object exists
	// leefix - 211006 - allow instanced objects to be masked
	if ( !ConfirmObjectInstance ( iID ) )
		return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];

	// set regular mask, then the following
	SetMask ( iID, iMASK );

	// set fore color wipe to all meshes in object
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		sMesh* pMesh = pObject->ppMeshList [ iMesh ];
		if ( pMesh )
			if ( pMesh->pDrawBuffer )
				pMesh->pDrawBuffer->dwImmuneToForeColorWipe = (DWORD)iForeColorWipe;
	}

	// 210214 - the mask flag can remove object from sorted list (effectively removing it from all engine render considerations)
	m_ObjectManager.UpdateTextures();
}


DARKSDK void SetArrayMap				( int iID, int iStage, int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9, int i10 )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void SetArrayMapEx				( int iID, int iStage, int iSrcObject, int iSrcStage )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void Instance					( int iDestinationID, int iSourceID, int iInstanceValue )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void SetNodeTreeEffect			( int iEffectID )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void DrawSingle					( int iObjectID, int iCameraID )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void ResetStaticLights			( void )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void AddStaticLight				( int iIndex, float fX, float fY, float fZ, float fRange )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void UpdateStaticLights			( void )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

// lee - 071108 - u71 - post processing screen commands (from SetupDLL)

bool CheckCreateScreenQuad ( void )
{
	// if camera zero is an image, rendering has been seperated, otherwise ignore
	tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( 0 );
	if ( m_Camera_Ptr->iCameraToImage>0 )
	{
		// create quad (if not exist)
		m_ObjectManager.CreateScreenQUAD ( m_Camera_Ptr->iCameraToImage );

		// success, quad ready and waiting
		return true;
	}

	// not due to use quad
	return false;
}

DARKSDK void TextureScreen ( int iStageIndex, int iImageID )
{
	// if quad not exist, create it or check if exists
	if ( CheckCreateScreenQuad()==false )
		return;

	// texture the quad
	m_ObjectManager.SetScreenTextureQUAD( iStageIndex, iImageID );
}

DARKSDK void SetScreenEffect ( int iEffectID )
{
	// if quad not exist, create it or check if exists
	if ( CheckCreateScreenQuad()==false )
		return;

	// set the quad effect
	m_ObjectManager.SetScreenEffectQUAD(iEffectID);
}

DARKSDK int RenderQuad ( int iActualRender )
{
	// leeadd - 131107 - u71 - needed control within core to know if quad before do render target swaps
	if ( iActualRender==1 )
	{
		// render the quad directly
		m_ObjectManager.RenderScreenQUAD();
		return 1;
	}
	else
	{
		// report if quad should be used
		if ( m_ObjectManager.m_pQUAD )
			return 1;
		else
			return 0;
	}
}

// Character creator - Dave - 070515
DARKSDK void SetCharacterCreatorTones ( int iID, int index, float red, float green, float blue, float mix )
{

	if ( index < 0 || index > 3 ) return;

	// check the object exists
	//if ( !ConfirmObjectInstance ( iID ) )
		//return;

	// apply setting to all meshes
	sObject* pObject = g_ObjectList [ iID ];
	if ( !pObject ) return;
	
	if ( !pObject->pCharacterCreator )
	{
		pObject->pCharacterCreator = new sObjectCharacterCreator;
		for ( int c = 0 ; c < 4 ; c++ )
		{
			pObject->pCharacterCreator->ColorTone[c][0] = -1;
			pObject->pCharacterCreator->ColorTone[c][1] = 0;
			pObject->pCharacterCreator->ColorTone[c][2] = 0;
			pObject->pCharacterCreator->ToneMix[c] = 0.0;
		}
	}

	pObject->pCharacterCreator->ColorTone[index][0] = red / 255.0f;
	pObject->pCharacterCreator->ColorTone[index][1] = green / 255.0f;
	pObject->pCharacterCreator->ColorTone[index][2] = blue / 255.0f;

	pObject->pCharacterCreator->ToneMix[index] = mix;

}


DARKSDK void SetLegacyMode ( int iUseLegacy )
{
	if ( iUseLegacy==0 )
		g_bSwitchLegacyOn = false;
	else
		g_bSwitchLegacyOn = true;
}

#ifdef DARKSDK_COMPILE

void dbLoadObject ( char* szFilename, int iID )
{
	Load ( ( DWORD ) szFilename, iID );
}

void dbLoadObject ( char* szFilename, int iID, int iDBProMode )
{
	Load ( ( DWORD ) szFilename, iID, iDBProMode );
}

// mike - 181206 - expose full load object function for GDK
void dbLoadObject ( char* szFilename, int iID, int iDBProMode, int iDivideTextureSize )
{
	Load ( ( DWORD ) szFilename, iID, iDBProMode, iDivideTextureSize );
}

void dbSaveObject ( char* szFilename, int iID )
{
	Save ( ( DWORD ) szFilename, iID );
}

void dbDeleteObject ( int iID )
{
	DeleteEx ( iID );
}

void dbSetObject ( int iID, bool bWireframe, bool bTransparency, bool bCull )
{
	Set ( iID, bWireframe, bTransparency, bCull );
}

void dbSetObject ( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter )
{
	Set ( iID, bWireframe, bTransparency, bCull, iFilter );
}

void dbSetObject ( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter, bool bLight )
{
	Set ( iID, bWireframe, bTransparency, bCull, iFilter, bLight );
}

void dbSetObject ( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter, bool bLight, bool bFog )
{
	Set ( iID, bWireframe, bTransparency, bCull, iFilter, bLight, bFog );
}

void dbSetObject ( int iID, bool bWireframe, bool bTransparency, bool bCull, int iFilter, bool bLight, bool bFog, bool bAmbient )
{
	Set ( iID, bWireframe, bTransparency, bCull, iFilter, bLight, bFog, bAmbient );
}

void dbSetObjectWireframe ( int iID, bool bFlag )
{
	SetWireframe ( iID, bFlag );
}

void dbSetObjectTransparency ( int iID, int iFlag )
{
	SetTransparency ( iID, iFlag );
}

void dbSetObjectCull ( int iID, bool bCull )
{
	SetCull ( iID, bCull );
}

void dbSetObjectFilter ( int iID, int iFilter )
{
	SetFilter ( iID, iFilter );
}

void dbSetObjectFilterStage ( int iID, int iStage, int iFilter )
{
	SetFilterStage ( iID, iStage, iFilter );
}

void dbSetObjectLight ( int iID, bool bLight )
{
	SetLight ( iID, bLight );
}

void dbSetObjectFog ( int iID, bool bFog )
{
	SetFog ( iID, bFog );
}

void dbSetObjectAmbient ( int iID, bool bAmbient )
{
	SetAmbient ( iID, bAmbient );
}

void dbMakeObject ( int iID, int iMeshID, int iTextureID )
{
	MakeObject ( iID, iMeshID, iTextureID );
}

void dbMakeObjectSphere ( int iID, float fRadius, int iRings, int iSegments )
{
	MakeSphere ( iID, fRadius, iRings, iSegments );
}

void dbCloneShared ( int iDestinationID, int iSourceID, int iCloneSharedData )
{
	CloneShared ( iDestinationID, iSourceID, iCloneSharedData );
}

void dbCloneObject ( int iDestinationID, int iSourceID )
{
	Clone ( iDestinationID, iSourceID );
}

void dbInstanceObject ( int iDestinationID, int iSourceID )
{
	Instance ( iDestinationID, iSourceID );
}

void dbMakeObjectSphere ( int iID, float fRadius )
{
	MakeSphere ( iID, fRadius );
}

void dbMakeObjectCube ( int iID, float iSize )
{
	MakeCube ( iID, iSize );
}

void dbMakeObjectBox ( int iID, float fWidth, float fHeight, float fDepth )
{
	MakeBox ( iID, fWidth, fHeight, fDepth );
}

void dbMakeObjectPyramid ( int iID, float fSize )
{
	MakePyramid ( iID, fSize );
}

void dbMakeObjectCylinder ( int iID, float fSize )
{
	MakeCylinder ( iID, fSize );
}

void dbMakeObjectCone ( int iID, float fSize )
{
	MakeCone ( iID, fSize );
}

void dbMakeObjectPlane ( int iID, float fWidth, float fHeight )
{
	MakePlane ( iID, fWidth, fHeight );
}

void dbMakeObjectTriangle ( int iID, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3 )
{
	MakeTriangle ( iID, x1, y1, z1, x2, y2, z2, x3, y3, z3 );
}

void dbAppendObject ( char* pString, int iID, int iFrame )
{
	Append ( ( DWORD ) pString, iID, iFrame );
}

void dbPlayObject ( int iID )
{
	Play ( iID );
}

void dbPlayObject ( int iID, int iStart )
{
	Play ( iID, iStart );
}

void dbPlayObject ( int iID, int iStart, int iEnd )
{
	Play ( iID, iStart, iEnd );
}

void dbLoopObject ( int iID )
{
	Loop ( iID );
}

void dbLoopObject ( int iID, int iStart )
{
	Loop ( iID, iStart );
}

void dbLoopObject ( int iID, int iStart, int iEnd )
{
	Loop ( iID, iStart, iEnd );
}

void dbStopObject ( int iID )
{
	Stop ( iID );
}

void dbHideObject ( int iID )
{
	Hide ( iID );
}

void dbShowObject ( int iID )
{
	Show ( iID );
}

void dbColorObject ( int iID, DWORD dwRGB )
{
	Color ( iID, dwRGB );
}

void dbSetObjectFrame ( int iID, int iFrame )
{
	SetFrame ( iID, iFrame );
}

void dbSetObjectFrame ( int iID, float fFrame )
{
	SetFrameEx ( iID, fFrame );
}

void dbSetObjectFrame ( int iID, float fFrame, int iRecalculateBounds )
{
	SetFrameEx ( iID, fFrame, iRecalculateBounds );
}

void dbSetObjectFrame ( int iID, int iLimbID, float fFrame, int iEnableOverride )
{
	SetFrameEx( iID, iLimbID, fFrame, iEnableOverride );
}

void dbSetObjectSpeed ( int iID, int iSpeed )
{
	SetSpeed ( iID, iSpeed );
}

void dbSetObjectInterpolation ( int iID, int iJump )
{
	SetInterpolation ( iID, iJump );
}

void dbSetObjectRotationXYZ ( int iID )
{
	SetRotationXYZ ( iID );
}

void dbSetObjectRotationZYX ( int iID )
{
	SetRotationZYX ( iID );
}

void dbSetObjectRotationZXY ( int iID )
{
	SetRotationZXY ( iID );
}

void dbGhostObjectOn ( int iID )
{
	GhostOn ( iID );
}

void dbGhostObjectOn ( int iID, int iFlag )
{
	GhostOn ( iID, iFlag );
}

void dbGhostObjectOff ( int iID )
{
	GhostOff ( iID );
}

void dbFadeObject ( int iID, float iPercentage )
{
	Fade ( iID, iPercentage );
}

void dbGlueObjectToLimb ( int iSource, int iTarget, int iLimb )
{
	GlueToLimbEx ( iSource, iTarget, iLimb, 0 );
}

void dbGlueObjectToLimb ( int iSource, int iTarget, int iLimb, int iMode )
{
	GlueToLimbEx ( iSource, iTarget, iLimb, iMode );
}

void dbUnGlueObject ( int iID )
{
	UnGlue ( iID );
}

void dbLockObjectOn ( int iID )
{
	LockOn ( iID );
}

void dbLockObjectOff ( int iID )
{
	LockOff ( iID );
}

void dbDisableObjectZDepth ( int iID )
{
	DisableZDepth ( iID );
}

void dbEnableObjectZDepth ( int iID )
{
	EnableZDepth ( iID );
}

void dbDisableObjectZRead ( int iID )
{
	DisableZRead ( iID );
}

void dbEnableObjectZRead ( int iID )
{
	EnableZRead ( iID );
}

void dbDisableObjectZWrite ( int iID )
{
	DisableZWrite ( iID );
}

void dbEnableObjectZWrite ( int iID )
{
	EnableZWrite ( iID );
}

void dbSetObjectFOV ( int iID, float fRadius )
{
	SetObjectFOV ( iID, fRadius );
}

void dbDeleteObjects ( int iFrom, int iTo )
{
	Deletes ( iFrom, iTo );
}

void dbClearObjectsOfTextureRef ( LPDIRECT3DTEXTURE9 pTextureRef )
{
	ClearObjectsOfTextureRef ( pTextureRef );
}

void dbDisableObjectBias ( int iID )
{
	DisableZBias (  iID );
}

void dbEnableObjectZBias ( int iID, float fSlopeScale, float fDepth )
{
	EnableZBias ( iID, fSlopeScale, fDepth );
}

void dbSetObjectDiffuse ( int iID, DWORD dwRGB )
{
	SetDiffuseMaterial ( iID, dwRGB );
}

void dbSetObjectAmbience ( int iID, DWORD dwRGB )
{
	SetAmbienceMaterial ( iID, dwRGB );
}

void dbSetObjectSpecular ( int iID, DWORD dwRGB )
{
	SetSpecularMaterial ( iID, dwRGB );
}

void dbSetObjectSpecularPower ( int iID, float fPower )
{
	SetSpecularPower ( iID, fPower );
}

void dbSetObjectEmissive ( int iID, DWORD dwRGB )
{
	SetEmissiveMaterial ( iID, dwRGB );
}

void dbFixObjectPivot ( int iID )
{
	FixPivot ( iID );
}

void dbSetObjectToObjectOrientation ( int iID, int iWhichID )
{
	SetToObjectOrientationEx ( iID, iWhichID, -1, 0 );
}

void dbSetObjectToObjectOrientation ( int iID, int iWhichID, int iWhichLimbID )
{
	SetToObjectOrientationEx ( iID, iWhichID, iWhichLimbID, 0 );
}

void dbSetObjectToCameraOrientation ( int iID )
{
	SetToCameraOrientation ( iID );
}

void dbTextureObject ( int iID, int iImage )
{
	SetTexture ( iID, iImage );
}

void dbTextureObject ( int iID, int iStage, int iImage )
{
	SetTextureStage ( iID, iStage, iImage );
}

void dbScrollObjectTexture ( int iID, float fU, float fV )
{
	ScrollTexture ( iID, fU, fV );
}

void dbScaleObjectTexture ( int iID, float fU, float fV )
{
	ScaleTexture ( iID, fU, fV );
}

void dbScaleObjectTexture ( int iID, int iStage, float fU, float fV )
{
	ScaleTexture ( iID, iStage, fU, fV );
}

void dbSetObjectSmoothing ( int iID, float fAngle )
{
	SetSmoothing ( iID, fAngle );
}

void dbSetObjectNormals ( int iID )
{
	SetNormals ( iID );
}

void dbSetObjectTexture ( int iID, int iMode, int iMipGeneration )
{
	SetTextureMode ( iID, iMode, iMipGeneration );
}

void dbSetObjectTexture ( int iID, int iStage, int iMode, int iMipGeneration )
{
	SetTextureModeStage ( iID, iStage, iMode, iMipGeneration );
}

void dbSetLightMappingOn ( int iID, int iImage )
{
	SetLightMap ( iID, iImage );
}

void dbSetSphereMappingOn ( int iID, int iSphereImage )
{
	SetSphereMap ( iID, iSphereImage );
}

void dbSetCubeMappingOn ( int iID, int i1, int i2, int i3, int i4, int i5, int i6 )
{
	SetCubeMap ( iID, i1, i2, i3, i4, i5, i6 );
}

void dbSetDetailMappingOn ( int iID, int iImage )
{
	SetDetailMap ( iID, iImage );
}

void dbSetBlendMappingOn ( int iID, int iImage, int iMode )
{
	SetBlendMap ( iID, iImage, iMode );
}

void dbSetBlendMappingOn ( int iID, int iStage, int iImage, int iTexCoordMode, int iMode )
{
	SetBlendMap ( iID, iStage, iImage, iTexCoordMode, iMode );
}

void dbSetTextureMD3 ( int iID, int iH0, int iH1, int iL0, int iL1, int iL2, int iU0 )
{

}

void dbSetAlphaMappingOn ( int iID, float fPercentage )
{
	SetAlphaFactor ( iID, fPercentage );
}

void dbSetBumpMappingOn ( int iID, int iBumpMap )
{
	SetBumpMap ( iID, iBumpMap );
}

void dbSetCartoonShadingOn ( int iID, int iStandardImage, int iEdgeImage )
{
	SetCartoonShadingOn ( iID, iStandardImage, iEdgeImage );
}

void dbSetRainbowShadingOn ( int iID, int iStandardImage )
{
	SetRainbowRenderingOn ( iID, iStandardImage );
}

void dbSetEffectOn ( int iID, char* pFilename, int iUseDefaultTextures )
{
	SetEffectOn ( iID, ( DWORD ) pFilename, iUseDefaultTextures );
}

void dbLoadEffect ( char* pFilename, int iEffectID, int iUseDefaultTextures )
{
	LoadEffect ( ( DWORD ) pFilename, iEffectID, iUseDefaultTextures );
}

void dbDeleteEffect ( int iEffectID )
{
	DeleteEffect ( iEffectID );
}

void dbSetObjectEffect ( int iID, int iEffectID )
{
	SetObjectEffect ( iID, iEffectID );
}

void dbSetLimbEffect ( int iID, int iLimbID, int iEffectID )
{
	SetLimbEffect ( iID, iLimbID, iEffectID );
}

void dbPerformChecklistForEffectValues ( int iEffectID )
{
	PerformChecklistForEffectValues ( iEffectID );
}

void dbPerformChecklistForEffectErrors ( void )
{
	PerformChecklistForEffectErrors ( );
}

void dbPerformChecklistForEffectErrors ( int iEffectID )
{
	PerformChecklistForEffectErrors ( iEffectID );
}

void dbSetEffectTranspose ( int iEffectID, int iTransposeFlag )
{
	SetEffectTranspose ( iEffectID, iTransposeFlag );
}

void dbSetEffectConstantBoolean ( int iEffectID, char* pConstantName, int iValue )
{
	SetEffectConstantB ( iEffectID, ( DWORD ) pConstantName, iValue );
}

void dbSetEffectConstantInteger ( int iEffectID, char* pConstantName, int iValue )
{
	SetEffectConstantI ( iEffectID, ( DWORD ) pConstantName, iValue );
}

void dbSetEffectConstantFloat ( int iEffectID, char* pConstantName, float fValue )
{
	SetEffectConstantF ( iEffectID, ( DWORD ) pConstantName, fValue );
}

void dbSetEffectConstantVector ( int iEffectID, char* pConstantName, int iValue )
{
	SetEffectConstantV ( iEffectID, ( DWORD ) pConstantName, iValue );
}

void dbSetEffectConstantMatrix ( int iEffectID, char* pConstantName, int iValue )
{
	SetEffectConstantM ( iEffectID, ( DWORD ) pConstantName, iValue );
}

void dbSetEffectTechnique ( int iEffectID, char* pTechniqueName )
{
	SetEffectTechnique ( iEffectID, ( DWORD ) pTechniqueName );
}

int dbEffectExist ( int iEffectID )
{
	return GetEffectExist ( iEffectID );
}

void dbSetShadowShadingOn ( int iID )
{
	SetShadowOn ( iID );
}

void dbSetShadowShadingOn ( int iID, int iMesh, int iRange, int iUseShader )
{
	SetShadowOnEx ( iID, iMesh, iRange, iUseShader );
}

void dbSetShadowClipping ( int iMode, float fMin, float fMax )
{
	SetShadowClipping ( iMode, fMin, fMax );
}

void dbSetReflectionShadingOn ( int iID )
{
	SetReflectionOn ( iID );
}

void dbSetGlobalShadowColor ( int iRed, int iGreen, int iBlue, int iAlphaLevel )
{
	SetShadowColor ( iRed, iGreen, iBlue, iAlphaLevel );
}

void dbSetGlobalShadowShades ( int iShades )
{
	SetShadowShades ( iShades );
}

void dbSetGlobalReflectionColor ( int iRed, int iGreen, int iBlue, int iAlphaLevel )
{
	SetReflectionColor ( iRed, iGreen, iBlue, iAlphaLevel );
}

// mike - 120307 - change function name to dbSetShadowShadingOff from dbSetShadingOff
void dbSetShadowShadingOff ( int iID )
{
	SetShadingOff ( iID );
}

void dbSetShadowPosition ( int iMode, float fX, float fY, float fZ )
{
	SetShadowPosition ( iMode, fX, fY, fZ );
}

void dbSetVertexShaderOn ( int iID, int iShader )
{
	SetVertexShaderOn ( iID, iShader );
}

void dbSetVertexShaderOff ( int iID )
{
	SetVertexShaderOff ( iID );
}

void dbConvertObjectFVF ( int iID, DWORD dwFVF )
{
	CloneMeshToNewFormat ( iID, dwFVF );
}

void dbSetVertexShaderStreamCount ( int iID, int iCount )
{
	SetVertexShaderStreamCount ( iID, iCount );
}

void dbSetVertexShaderStream ( int iID, int iStreamPos, int iData, int iDataType )
{
	SetVertexShaderStream ( iID, iStreamPos, iData, iDataType );
}

void dbCreateVertexShaderFromFile ( int iID, char* szFile )
{
	CreateVertexShaderFromFile ( iID, ( DWORD ) szFile );
}

void dbSetVertexShaderVector ( int iID, DWORD dwRegister, int iVector, DWORD dwConstantCount )
{
	SetVertexShaderVector ( iID, dwRegister, iVector, dwConstantCount );
}

void dbSetVertexShaderMatrix ( int iID, DWORD dwRegister, int iMatrix, DWORD dwConstantCount )
{
	SetVertexShaderMatrix ( iID, dwRegister, iMatrix, dwConstantCount );
}

void dbDeleteVertexShader ( int iShader )
{
	DeleteVertexShader ( iShader );
}

void dbSetPixelShaderOn ( int iID, int iShader )
{
	SetPixelShaderOn ( iID, iShader );
}

void dbSetPixelShaderOff ( int iID )
{
	SetPixelShaderOff ( iID );
}

void dbCreatePixelShaderFromFile ( int iID, char* szFile )
{
	CreatePixelShaderFromFile ( iID, ( DWORD ) szFile );
}

void dbDeletePixelShader ( int iShader )
{
	DeletePixelShader ( iShader );
}

void dbSetPixelShaderTexture ( int iShader, int iSlot, int iTexture )
{
	SetPixelShaderTexture ( iShader, iSlot, iTexture );
}

void dbSaveObjectAnimation ( int iID, char* pFilename )
{
	SaveAnimation ( iID, ( DWORD ) pFilename );
}

void dbAppendObjectAnimation ( int iID, char* pFilename )
{
	AppendAnimation ( iID, ( DWORD ) pFilename );
}

void dbClearAllObjectKeyFrames ( int iID )
{
	//ClearAllKeyFrames ( iID );
}

void dbClearObjectKeyFrame ( int iID, int iFrame )
{
	//ClearKeyFrame ( iID, iFrame );
}

void dbSetObjectKeyFrame ( int iID, int iFrame )
{
	//SetKeyFrame ( iID, iFrame );
}

void dbSetStaticUniverse ( float fX, float fY, float fZ )
{
	CreateNodeTree ( fX, fY, fZ );
}

void dbMakeStaticObject ( int iID, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker )
{
	AddNodeTreeObject ( iID, iType, iArbitaryValue, iCastShadow, iPortalBlocker );
}

void dbMakeStaticLimb ( int iID, int iLimb, int iType, int iArbitaryValue, int iCastShadow, int iPortalBlocker )
{
	AddNodeTreeLimb ( iID, iLimb, iType, iArbitaryValue, iCastShadow, iPortalBlocker );
}

void dbDeleteStaticObject ( int iID )
{
	RemoveNodeTreeObject ( iID );
}

void dbDeleteStaticObjects ( void )
{
	DeleteNodeTree ( );
}

void dbSetStaticObjectsWireframeOn ( void )
{
	SetNodeTreeWireframeOn ( );
}

void dbSetStaticObjectsWireframeOff ( void )
{
	SetNodeTreeWireframeOff ( );
}

void dbMakeStaticCollisionBox ( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2 )
{
	MakeNodeTreeCollisionBox ( fX1, fY1, fZ1, fX2, fY2, fZ2 );
}

void dbSetSetStaticObjectsTexture ( int iMode )
{
	SetNodeTreeTextureMode ( iMode );
}

void dbDisableStaticOcclusion ( void )
{
	DisableNodeTreeOcclusion ( );
}

void dbEnableStaticOcclusion ( void )
{
	EnableNodeTreeOcclusion ( );
}

void dbSaveStaticObjects ( char* pFilename )
{
	SaveNodeTreeObjects ( ( DWORD ) pFilename );
}

void dbLoadStaticObjects ( char* pFilename, int iDivideTextureSize )
{
	LoadNodeTreeObjects ( ( DWORD ) pFilename, iDivideTextureSize );
}

void dbAttachObjectToStatic ( int iID )
{
	AttachObjectToNodeTree ( iID );
}

void dbDetachObjectFromStatic ( int iID )
{
	DetachObjectFromNodeTree ( iID );
}

void dbSetStaticPortalsOn ( void )
{
	SetNodeTreePortalsOn ( );
}

void dbSetNodeTreeCulling ( int iFlag )
{
	SetNodeTreeCulling ( iFlag );
}

void dbSetStaticPortalsOff ( void )
{
	SetNodeTreePortalsOff ( );
}

void dbBuildStaticPortals ( void )
{
	BuildNodeTreePortals ( );
}

void dbSetStaticScorch ( int iImageID, int iWidth, int iHeight )
{
	SetNodeTreeScorchTexture ( iImageID, iWidth, iHeight );
}

void dbAddStaticScorch ( float fSize, int iType )
{
	AddNodeTreeScorch ( fSize, iType );
}

void dbAddStaticLight ( int iLightIndex, float fX, float fY, float fZ, float fRange )
{
	AddNodeTreeLight ( iLightIndex, fX, fY, fZ, fRange );
}

void dbPeformCSGUnion ( int iObjectA, int iObjectB )
{
	PeformCSGUnion ( iObjectA, iObjectB );
}

void dbPeformCSGDifference ( int iObjectA, int iObjectB )
{
	PeformCSGDifference ( iObjectA, iObjectB );
}

void dbPeformCSGIntersection ( int iObjectA, int iObjectB )
{
	PeformCSGIntersection ( iObjectA, iObjectB );
}

void dbPeformCSGClip ( int iObjectA, int iObjectB )
{
	PeformCSGClip ( iObjectA, iObjectB );
}

int dbObjectBlocking ( int iID, float X1, float Y1, float Z1, float X2, float Y2, float Z2 )
{
	// lee - 300706 - fixed recursive overflow
	return ObjectBlocking ( iID, X1, Y1, Z1, X2, Y2, Z2 );
}

void dbAddObjectToLightMapPool ( int iID )
{
	AddObjectToLightMapPool ( iID );
}

void dbAddLimbToLightMapPool ( int iID, int iLimb )
{
	AddLimbToLightMapPool ( iID, iLimb );
}

void dbAddStaticObjectsToLightMapPool ( void )
{
	AddStaticObjectsToLightMapPool ( );
}

void dbAddLightMapLight ( float fX, float fY, float fZ, float fRadius, float fRed, float fGreen, float fBlue, float fBrightness, bool bCastShadow )
{
	// lee - 300706 - fixed recursive overflow
	AddLightMapLight ( fX, fY, fZ, fRadius, fRed, fGreen, fBlue, fBrightness, bCastShadow );
}

void dbDeleteLightMapLights ( void )
{
	FlushLightMapLights ( );
}

void dbCreateLightMaps ( int iLMSize, int iLMQuality, char* dwPathForLightMaps )
{
	CreateLightMaps ( iLMSize, iLMQuality, ( DWORD ) dwPathForLightMaps );
}

void dbSetGlobalShadowsOn ( void )
{
	SetGlobalShadowsOn ( );
}

void dbSetGlobalShadowsOff ( void )
{
	SetGlobalShadowsOff ( );
}

void dbSetObjectCollisionOn ( int iID )
{
	SetCollisionOn ( iID );
}

void dbSetObjectCollisionOff ( int iID )
{
	SetCollisionOff ( iID );
}

void dbMakeObjectCollisionBox ( int iID, float iX1, float iY1, float iZ1, float iX2, float iY2, float iZ2, int iRotatedBoxFlag )
{
	MakeCollisionBox ( iID, iX1, iY1, iZ1, iX2, iY2, iZ2, iRotatedBoxFlag );
}

void dbDeleteObjectCollisionBox ( int iID )
{
	DeleteCollisionBox ( iID );
}

void dbSetObjectCollisionToSpheres ( int iID )
{
	SetCollisionToSpheres ( iID );
}

void dbSetObjectCollisionToBoxes ( int iID )
{
	SetCollisionToBoxes ( iID );
}

void dbSetObjectCollisionToPolygons ( int iID )
{
	SetCollisionToPolygons ( iID );
}

void dbSetGlobalCollisionOn ( void )
{
	SetGlobalCollisionOn ( );
}

void dbSetGlobalCollisionOff ( void )
{
	SetGlobalCollisionOff ( );
}

void dbSetObjectRadius ( int iID, float fRadius )
{
	SetSphereRadius ( iID, fRadius );
}

float dbIntersectObject ( int iObjectID, float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ )
{
	DWORD dwReturn = IntersectObject ( iObjectID, fX, fY, fZ, fNewX, fNewY, fNewZ );
	
	return *( float* ) &dwReturn;
}

void dbAutomaticObjectCollision ( int iObjectID, float fRadius, int iResponse )
{
	AutomaticObjectCollision ( iObjectID, fRadius, iResponse );
}

void dbAutomaticCameraCollision ( int iCameraID, float fRadius, int iResponse )
{
	AutomaticCameraCollision ( iCameraID, fRadius, iResponse );
}

void dbCalculateAutomaticCollision ( void )
{
	ForceAutomaticEnd ( );
}

void dbHideObjectBounds ( int iID )
{
	HideBounds ( iID );
}

void dbShowObjectBounds ( int iID, int iBoxOnly )
{
	ShowBoundsEx ( iID, iBoxOnly );
}

void dbShowObjectBounds ( int iID )
{
	ShowBounds ( iID );
}

void dbShowObjectLimbBounds ( int iID, int iLimb )
{
	// mike 160505 - new function for display limb box
	ShowBounds ( iID, iLimb );
}


void dbPerformCheckListForObjectLimbs ( int iID )
{
	PerformCheckListForLimbs ( iID );
}

void dbHideLimb ( int iID, int iLimbID )
{
	HideLimb ( iID, iLimbID );
}

void dbShowLimb ( int iID, int iLimbID )
{
	ShowLimb ( iID, iLimbID );
}

void dbOffsetLimb ( int iID, int iLimbID, float fX, float fY, float fZ )
{
	OffsetLimb ( iID, iLimbID, fX, fY, fZ );
}

void dbOffsetLimb ( int iID, int iLimbID, float fX, float fY, float fZ, int iBoundFlag )
{
	OffsetLimb ( iID, iLimbID, fX, fY, fZ );
}

void dbRotateLimb ( int iID, int iLimbID, float fX, float fY, float fZ )
{
	RotateLimb ( iID, iLimbID, fX, fY, fZ );
}

void dbRotateLimb ( int iID, int iLimbID, float fX, float fY, float fZ, int iBoundFlag )
{
	RotateLimb ( iID, iLimbID, fX, fY, fZ, iBoundFlag );
}

void dbScaleLimb ( int iID, int iLimbID, float fX, float fY, float fZ )
{
	ScaleLimb ( iID, iLimbID, fX, fY, fZ );
}

void dbScaleLimb ( int iID, int iLimbID, float fX, float fY, float fZ, int iBoundFlag )
{
	ScaleLimb ( iID, iLimbID, fX, fY, fZ, iBoundFlag );
}

void dbAddLimb ( int iID, int iLimbID, int iMeshID )
{
	AddLimb ( iID, iLimbID, iMeshID );
}

void dbRemoveLimb ( int iID, int iLimbID )
{
	RemoveLimb ( iID, iLimbID );
}

void dbLinkLimb ( int iID, int iParentID, int iLimbID )
{
	LinkLimb ( iID, iParentID, iLimbID );
}

void dbTextureLimb ( int iID, int iLimbID, int iImageID )
{
	TextureLimb ( iID, iLimbID, iImageID );
}

void dbTextureLimb ( int iID, int iLimbID, int iStage, int iImageID )
{
	TextureLimbStage ( iID, iLimbID, iStage, iImageID );
}

void dbColorLimb ( int iID, int iLimbID, DWORD dwColor )
{
	ColorLimb ( iID, iLimbID, dwColor );
}

void dbScrollLimbTexture ( int iID, int iLimbID, float fU, float fV )
{
	ScrollLimbTexture ( iID, iLimbID, fU, fV );
}

void dbScaleLimbTexture ( int iID, int iLimbID, float fU, float fV )
{
	ScaleLimbTexture ( iID, iLimbID, fU, fV );
}

void dbSetLimbSmoothing ( int iID, int iLimbID, float fPercentage )
{
	SetLimbSmoothing ( iID, iLimbID, fPercentage );
}

void dbSetLimbNormals ( int iID, int iLimbID )
{
	SetLimbNormals ( iID, iLimbID );
}

void dbMakeObjectFromLimb ( int iID, int iSrcObj, int iLimbID )
{
	MakeObjectFromLimb ( iID, iSrcObj, iLimbID );
}

void dbAddLODToObject ( int iCurrentID, int iLODModelID, int iLODLevel, float fDistanceOfLOD )
{
	AddLODToObject ( iCurrentID, iLODModelID, iLODLevel, fDistanceOfLOD );
}

void dbLoadMesh ( char* pFilename, int iID )
{
	LoadMesh ( ( DWORD ) pFilename, iID );
}

void dbDeleteMesh ( int iID )
{
	DeleteMesh ( iID );
}

void dbSaveMesh ( char* pFilename, int iMeshID )
{
	SaveMesh ( ( DWORD ) pFilename, iMeshID );
}

void dbChangeMesh ( int iObjectID, int iLimbID, int iMeshID )
{
	ChangeMesh ( iObjectID, iLimbID, iMeshID );
}

void dbMakeMeshFromObject ( int iID, int iObjectID )
{
	MakeMeshFromObject ( iID, iObjectID );
}

void dbReduceMesh ( int iMeshID, int iBlockMode, int iNearMode, int iGX, int iGY, int iGZ )
{
	ReduceMesh ( iMeshID, iBlockMode, iNearMode, iGX, iGY, iGZ );
}

void dbMakeLODFromMesh ( int iMeshID, int iVertNum, int iNewMeshID )
{
	MakeLODFromMesh ( iMeshID, iVertNum, iNewMeshID );
}

void dbLockVertexDataForLimb ( int iID, int iLimbID, int iReplaceOrUpdate )
{
	LockVertexDataForLimbCore ( iID, iLimbID, iReplaceOrUpdate );
}

void dbLockVertexDataForLimb ( int iID, int iLimbID )
{
	LockVertexDataForLimb ( iID, iLimbID );
}

void dbLockVertexDataForMesh ( int iID )
{
	LockVertexDataForMesh ( iID );
}

void dbUnlockVertexData ( void )
{
	UnlockVertexData ( );
}

void dbSetVertexDataPosition ( int iVertex, float fX, float fY, float fZ )
{
	SetVertexDataPosition ( iVertex, fX, fY, fZ );
}

void dbSetVertexDataNormals ( int iVertex, float fNX, float fNY, float fNZ )
{
	SetVertexDataNormals ( iVertex, fNX, fNY, fNZ );
}

void dbSetVertexDataDiffuse ( int iVertex, DWORD dwDiffuse )
{
	SetVertexDataDiffuse ( iVertex, dwDiffuse );
}

void dbSetVertexDataUV ( int iVertex, float fU, float fV )
{
	SetVertexDataUV ( iVertex, fU, fV );
}

void dbSetVertexDataSize ( int iVertex, float fSize )
{
	SetVertexDataSize ( iVertex, fSize );
}

void dbSetIndexData ( int iIndex, int iValue )
{
	SetIndexData ( iIndex, iValue );
}

int dbGetIndexData ( int iIndex )
{
	return GetIndexData ( iIndex );
}

void dbSetVertexDataUV ( int iVertex, int iIndex, float fU, float fV )
{
	SetVertexDataUV ( iVertex, iIndex, fU, fV );
}

void dbAddMeshToVertexData ( int iMeshID )
{
	AddMeshToVertexData ( iMeshID );
}

void dbDeleteMeshFromVertexData ( int iVertex1, int iVertex2, int iIndex1, int iIndex2 )
{
	DeleteMeshFromVertexData ( iVertex1, iVertex2, iIndex1, iIndex2 );
}

void dbPeformCSGUnionOnVertexData ( int iBrushMeshID )
{
	PeformCSGUnionOnVertexData ( iBrushMeshID );
}

void dbPeformCSGDifferenceOnVertexData ( int iBrushMeshID )
{
	PeformCSGDifferenceOnVertexData ( iBrushMeshID );
}

void dbPeformCSGIntersectionOnVertexData ( int iBrushMeshID )
{
	PeformCSGIntersectionOnVertexData ( iBrushMeshID );
}

int dbGetVertexDataVertexCount ( void )
{
	return GetVertexDataVertexCount ( );
}

int dbGetVertexDataIndexCount ( void )
{
	return GetVertexDataIndexCount ( );
}

float dbGetVertexDataPositionX ( int iVertex )
{
	DWORD dwReturn = GetVertexDataPositionX ( iVertex );
	
	return *( float* ) &dwReturn;
}

float dbGetVertexDataPositionY ( int iVertex )
{
	DWORD dwReturn = GetVertexDataPositionY ( iVertex );
	
	return *( float* ) &dwReturn;
}

float dbGetVertexDataPositionZ ( int iVertex )
{
	DWORD dwReturn = GetVertexDataPositionZ ( iVertex );
	
	return *( float* ) &dwReturn;
}

float dbGetVertexDataNormalsX ( int iVertex )
{
	DWORD dwReturn = GetVertexDataNormalsX ( iVertex );
	
	return *( float* ) &dwReturn;
}

float dbGetVertexDataNormalsY ( int iVertex )
{
	DWORD dwReturn = GetVertexDataNormalsY ( iVertex );
	
	return *( float* ) &dwReturn;
}

float dbGetVertexDataNormalsZ ( int iVertex )
{
	DWORD dwReturn = GetVertexDataNormalsZ ( iVertex );
	
	return *( float* ) &dwReturn;
}

DWORD dbGetVertexDataDiffuse ( int iVertex )
{
	return GetVertexDataDiffuse ( iVertex );
}

float dbGetVertexDataU ( int iVertex )
{
	DWORD dwReturn = GetVertexDataU ( iVertex );
	
	return *( float* ) &dwReturn;
}

float dbGetVertexDataV ( int iVertex )
{
	DWORD dwReturn = GetVertexDataV ( iVertex );
	
	return *( float* ) &dwReturn;
}

float dbGetVertexDataU ( int iVertex, int iIndex )
{
	DWORD dwReturn = GetVertexDataU ( iVertex, iIndex );
	
	return *( float* ) &dwReturn;
}

float dbGetVertexDataV ( int iVertex, int iIndex )
{
	DWORD dwReturn = GetVertexDataV ( iVertex, iIndex );
	
	return *( float* ) &dwReturn;
}

void dbSetMipmapMode ( int iMode )
{
	SetMipmapMode ( iMode );
}

void dbFlushVideoMemory ( void )
{
	FlushVideoMemory ( );
}

void dbDisableTNL ( void )
{
	DisableTNL ( );
}

void dbEnableTNL ( void )
{
	EnableTNL ( );
}

void dbConvert3DStoX ( char* pFilename1, char* pFilename2 )
{
	Convert3DStoX ( ( DWORD ) pFilename1, ( DWORD ) pFilename2 );
}

int dbPickObject ( int iX, int iY, int iObjectStart, int iObjectEnd )
{
	return PickScreenObject ( iX, iY, iObjectStart, iObjectEnd );
}

void dbPickScreen ( int iX, int iY, float fDistance )
{
	PickScreen2D23D ( iX, iY, fDistance );
}

float dbGetPickDistance ( void )
{
	DWORD dwReturn = GetPickDistance ( );
	
	return *( float* ) &dwReturn;
}

float dbGetPickVectorX ( void )
{
	DWORD dwReturn = GetPickVectorX ( );
	
	return *( float* ) &dwReturn;
}

float dbGetPickVectorY ( void )
{
	DWORD dwReturn = GetPickVectorY ( );
	
	return *( float* ) &dwReturn;
}

float dbGetPickVectorZ ( void )
{
	DWORD dwReturn = GetPickVectorZ ( );
	
	return *( float* ) &dwReturn;
}

void dbMakeEmitter ( int iID, int iSize )
{
	MakeEmitter ( iID, iSize );
}

void dbGetEmitterData ( int iID, BYTE** ppVertices, DWORD* pdwVertexCount, int** ppiDrawCount )
{
	GetEmitterData ( iID, ppVertices, pdwVertexCount, ppiDrawCount );
}

void dbUpdateEmitter ( int iID )
{
	UpdateEmitter ( iID );
}

void dbGetPositionData ( int iID, sPositionData** ppPosition )
{
	GetPositionData ( iID, ppPosition );
}

void dbExcludeObjectOn ( int iID )
{
	ExcludeOn ( iID );
}

void dbExcludeObjectOff ( int iID )
{
	ExcludeOff ( iID );
}

int dbObjectExist ( int iID )
{
	return GetExist ( iID );
}

int dbTotalObjectFrames ( int iID )
{
	return GetNumberOfFrames ( iID );
}

float dbObjectSize ( int iID )
{
	DWORD dwReturn = GetSize ( iID );
	
	return *( float* ) &dwReturn;
}

// mike - 120307 - extra function for scaling
float dbObjectSize ( int iID, int iUseScaling )
{
	DWORD dwReturn = GetSizeEx ( iID, iUseScaling );
	
	return *( float* ) &dwReturn;
}

float dbObjectSizeX ( int iID )
{
	DWORD dwReturn = GetXSize ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectSizeY ( int iID )
{
	DWORD dwReturn = GetYSize ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectSizeZ ( int iID )
{
	DWORD dwReturn = GetZSize ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectSizeX ( int iID, int iUseScaling )
{
	DWORD dwReturn = GetXSizeEx ( iID, iUseScaling );
	return *( float* ) &dwReturn;
}

float dbObjectSizeY ( int iID, int iUseScaling )
{
	DWORD dwReturn = GetYSizeEx ( iID, iUseScaling );
	return *( float* ) &dwReturn;
}

float dbObjectSizeZ ( int iID, int iUseScaling )
{
	DWORD dwReturn = GetZSizeEx ( iID, iUseScaling );
	return *( float* ) &dwReturn;
}

int dbObjectVisible ( int iID )
{
	return GetVisible ( iID );
}

int dbObjectPlaying ( int iID )
{
	return GetPlaying ( iID );
}

int dbObjectLooping ( int iID )
{
	return GetLooping ( iID );
}

float dbObjectFrame ( int iID )
{
	DWORD dwReturn = GetFrame ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectSpeed ( int iID )
{
	DWORD dwReturn = GetSpeed ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectInterpolation ( int iID )
{
	DWORD dwReturn = GetInterpolation ( iID );
	
	return *( float* ) &dwReturn;
}

int dbObjectScreenX ( int iID )
{
	return GetScreenX ( iID );
}

int dbObjectScreenY ( int iID )
{
	return GetScreenY ( iID );
}

int dbObjectInScreen ( int iID )
{
	return GetInScreen ( iID );
}

float dbObjectCollisionRadius ( int iID )
{
	DWORD dwReturn = GetObjectCollisionRadius ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectCollisionCenterX ( int iID )
{
	DWORD dwReturn = GetObjectCollisionCenterX ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectCollisionCenterY ( int iID )
{
	DWORD dwReturn = GetObjectCollisionCenterY ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectCollisionCenterZ ( int iID )
{
	DWORD dwReturn = GetObjectCollisionCenterZ ( iID );
	
	return *( float* ) &dwReturn;
}

int dbObjectCollision ( int iObjectA, int iObjectB )
{
	DWORD dwReturn = GetCollision ( iObjectA, iObjectB );
	
	// mike - 270405 - changed the cast on this because I screwed up

	return dwReturn;

	//return *( float* ) &dwReturn;
}

int dbObjectHit ( int iObjectA, int iObjectB )
{
	return GetHit ( iObjectA, iObjectB );
}

int dbLimbCollision ( int iObjectA, int iLimbA, int iObjectB, int iLimbB )
{
	return GetLimbCollision ( iObjectA, iLimbA, iObjectB, iLimbB );
}

int dbLimbHit ( int iObjectA, int iLimbA, int iObjectB, int iLimbB )
{
	return GetLimbHit ( iObjectA, iLimbA, iObjectB, iLimbB );
}

int dbGetStaticCollisionHit ( float fOldX1, float fOldY1, float fOldZ1, float fOldX2, float fOldY2, float fOldZ2, float fNX1, float fNY1, float fNZ1, float fNX2, float fNY2, float fNZ2 )
{
	return GetStaticHit ( fOldX1, fOldY1, fOldZ1, fOldX2, fOldY2, fOldZ2, fNX1, fNY1, fNZ1, fNX2, fNY2, fNZ2 );
}

int dbStaticLineOfSight ( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz, float fWidth, float fAccuracy )
{
	return GetStaticLineOfSight ( fSx, fSy, fSz, fDx, fDy, fDz, fWidth, fAccuracy );
}

int dbStaticRayCast ( float fSx, float fSy, float fSz, float fDx, float fDy, float fDz )
{
	return GetStaticRayCast ( fSx, fSy, fSz, fDx, fDy, fDz );
}

int dbStaticVolume ( float fX, float fY, float fZ, float fNewX, float fNewY, float fNewZ, float fSize )
{
	return GetStaticVolumeCast ( fX, fY, fZ, fNewX, fNewY, fNewZ, fSize );
}

float dbGetStaticCollisionX ( void )
{
	DWORD dwReturn = GetStaticX ( );
	
	return *( float* ) &dwReturn;
}

float dbGetStaticCollisionY ( void )
{
	DWORD dwReturn = GetStaticY ( );
	
	return *( float* ) &dwReturn;
}

float dbGetStaticCollisionZ ( void )
{
	DWORD dwReturn = GetStaticZ ( );
	
	return *( float* ) &dwReturn;
}

int dbGetStaticCollisionFloor ( void )
{
	return GetStaticFloor ( );
}

int dbGetStaticCollisionCount ( void )
{
	return GetStaticColCount ( );
}

int dbGetStaticCollisionValue ( void )
{
	return GetStaticColValue ( );
}

float dbStaticLineOfSightX ( void )
{
	DWORD dwReturn = GetStaticLineOfSightX ( );
	
	return *( float* ) &dwReturn;
}

float dbStaticLineOfSightY ( void )
{
	DWORD dwReturn = GetStaticLineOfSightY ( );
	
	return *( float* ) &dwReturn;
}

float dbStaticLineOfSightZ ( void )
{
	DWORD dwReturn = GetStaticLineOfSightZ ( );
	
	return *( float* ) &dwReturn;
}

float dbGetObjectCollisionX ( void )
{
	DWORD dwReturn = GetCollisionX ( );
	
	return *( float* ) &dwReturn;
}

float dbGetObjectCollisionY ( void )
{
	DWORD dwReturn = GetCollisionY ( );
	
	return *( float* ) &dwReturn;
}

float dbGetObjectCollisionZ ( void )
{
	DWORD dwReturn = GetCollisionZ ( );
	
	return *( float* ) &dwReturn;
}

int dbLimbExist ( int iID, int iLimbID )
{
	return GetLimbExist ( iID, iLimbID );
}

float dbLimbOffsetX ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbOffsetX ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbOffsetY ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbOffsetY ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbOffsetZ ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbOffsetZ ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbAngleX ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbAngleX ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbAngleY ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbAngleY ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbAngleZ ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbAngleZ ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbPositionX ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbXPosition ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbPositionY ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbYPosition ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbPositionZ ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbZPosition ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbDirectionX ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbXDirection ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbDirectionY ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbYDirection ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbDirectionZ ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbZDirection ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

int dbLimbTexture ( int iID, int iLimbID )
{
	return GetLimbTexture ( iID, iLimbID );
}

int dbLimbVisible ( int iID, int iLimbID )
{
	return GetLimbVisible ( iID, iLimbID );
}

int dbCheckLimbLink ( int iID, int iLimbID )
{
	return GetLimbLink ( iID, iLimbID );
}

char* dbLimbTextureName ( SDK_RETSTR int iID, int iLimbID )
{
	return ( char* ) GetLimbTextureName ( NULL, iID, iLimbID );
}

int dbMeshExist ( int iID )
{
	return GetMeshExist ( iID );
}

int dbAlphaBlendingAvailable ( void )
{
	return GetAlphaBlending ( );
}

int dbFilteringAvailable ( void )
{
	return GetBlending ( );
}

int dbFogAvailable ( void )
{
	return GetFog ( );
}

int dbGet3DBLITAvailable ( void )
{
	return Get3DBLITAvailable ( );
}

int dbStatistic ( int iCode )
{
	return GetStatistic ( iCode );
}

int dbGetTNLAvailable ( void )
{
	return GetTNLAvailable ( );
}

char* dbLimbName ( DWORD pDestStr, int iID, int iLimbID )
{
	return ( char* ) GetLimbName ( pDestStr, iID, iLimbID );
}

int dbLimbCount ( int iID )
{
	return GetLimbCount ( iID );
}

float dbLimbScaleX ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbScaleX ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbScaleY ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbScaleY ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

float dbLimbScaleZ ( int iID, int iLimbID )
{
	DWORD dwReturn = GetLimbScaleZ ( iID, iLimbID );
	
	return *( float* ) &dwReturn;
}

bool dbVertexShaderExist ( int iShader )
{
	return ( bool ) VertexShaderExist ( iShader );
}

bool dbPixelShaderExist ( int iShader )
{
	return ( bool ) PixelShaderExist ( iShader );
}

int dbGetDeviceType ( void )
{
	return GetDeviceType ( );
}

bool dbCalibrateGammaAvailable ( void )
{
	return GetCalibrateGamma ( );
}

bool dbRenderWindowedAvailable ( void )
{
	return GetRenderWindowed ( );
}

bool dbFullScreenGammaAvailable ( void )
{
	return GetFullScreenGamma ( );
}

bool dbBlitSysOntoLocalAvailable ( void )
{
	return GetBlitSysOntoLocal ( );
}

bool dbRenderAfterFlipAvailable ( void )
{
	return GetRenderAfterFlip ( );
}

bool dbTNLAvailable ( void )
{
	return GetDrawPrimTLVertex ( );
}

bool dbGetHWTransformAndLight ( void )
{
	return GetHWTransformAndLight ( );
}

bool dbSeparateTextureMemoriesAvailable ( void )
{
	return GetSeparateTextureMemories ( );
}

bool dbTextureSystemMemoryAvailable ( void )
{
	return GetTextureSystemMemory ( );
}

bool dbTextureVideoMemoryAvailable ( void )
{
	return GetTextureVideoMemory ( );
}

bool dbNonLocalVideoMemoryAvailable ( void )
{
	return GetTextureNonLocalVideoMemory ( );
}

bool dbTLVertexSystemMemoryAvailable ( void )
{
	// mike - 220107 - fixed function name
	return GetTLVertexSystemMemory ( );
}

bool dbTLVertexVideoMemoryAvailable ( void )
{
	return GetTLVertexVideoMemory ( );
}

bool dbClipAndScalePointsAvailable ( void )
{
	return GetClipAndScalePoints ( );
}

bool dbClipTLVertsAvailable ( void )
{
	return GetClipTLVerts ( );
}

bool dbColorWriteEnableAvailable ( void )
{
	return GetColorWriteEnable ( );
}

bool dbCullCCWAvailable ( void )
{
	return GetCullCCW ( );
}

bool dbCullCWAvailable ( void )
{
	return GetCullCW ( );
}

bool dbAnisotropicFilteringAvailable ( void )
{
	return GetAnisotropicFiltering ( );
}

bool dbAntiAliasAvailable ( void )
{
	return GetAntiAliasEdges ( );
}

bool dbColorPerspectiveAvailable ( void )
{
	return GetColorPerspective ( );
}

bool dbDitherAvailable ( void )
{
	return GetDither ( );
}

bool dbFogRangeAvailable ( void )
{
	return GetFogRange ( );
}

bool dbFogTableAvailable ( void )
{
	return GetFogTable ( );
}

bool dbFogVertexAvailable ( void )
{
	return GetFogVertex ( );
}

bool dbMipMapLODBiasAvailable ( void )
{
	return GetMipMapLODBias ( );
}

bool dbWBufferAvailable ( void )
{
	return GetWBuffer ( );
}

bool dbWFogAvailable ( void )
{
	return GetWFog ( );
}

bool dbZFogAvailable ( void )
{
	return GetZFog ( );
}

bool dbAlphaAvailable ( void )
{
	return GetAlpha ( );
}

bool dbCubeMapAvailable ( void )
{
	return GetCubeMap ( );
}

bool dbMipCubeMapAvailable ( void )
{
	return GetMipCubeMap ( );
}

bool dbMipMapAvailable ( void )
{
	return GetMipMap ( );
}

bool dbMipMapVolumeAvailable ( void )
{
	return GetMipMapVolume ( );
}

bool dbNonPowTexturesAvailable ( void )
{
	return GetNonPowerOf2Textures ( );
}

bool dbPerspectiveTexturesAvailable ( void )
{
	return GetPerspective ( );
}

bool dbProjectedTexturesAvailable ( void )
{
	return GetProjected ( );
}

bool dbOnlySquareTexturesAvailable ( void )
{
	return GetSquareOnly ( );
}

bool dbVolumeMapAvailable ( void )
{
	return GetVolumeMap ( );
}

bool dbAlphaComparisionAvailable ( void )
{
	return GetAlphaComparision ( );
}

bool dbTextureAvailable ( void )
{
	return GetTexture ( );
}

bool dbZBufferAvailable ( void )
{
	return GetZBuffer ( );
}

int dbGetMaximumTextureWidth ( void )
{
	return GetMaxTextureWidth ( );
}

int dbGetMaximumTextureHeight ( void )
{
	return GetMaxTextureHeight ( );
}

int dbGetMaximumVolumeExtent ( void )
{
	return GetMaxVolumeExtent ( );
}

float dbGetMaximumVertexShaderVersion ( void )
{
	DWORD dwReturn = GetVertexShaderVersion ( );
	
	return *( float* ) &dwReturn;
}

float dbGetMaximumPixelShaderVersion ( void )
{
	DWORD dwReturn = GetPixelShaderVersion ( );
	
	return *( float* ) &dwReturn;
}

int dbGetMaximumVertexShaderConstants ( void )
{
	return GetMaxVertexShaderConstants ( );
}

float dbGetMaximumPixelShaderValue ( void )
{
	DWORD dwReturn = GetMaxPixelShaderValue ( );
	
	return *( float* ) &dwReturn;
}

int dbGetMaximumLights ( void )
{
	return GetMaxLights ( );
}

void dbSetObjectDeleteCallBack ( int iID, ON_OBJECT_DELETE_CALLBACK pfn, int userData )
{
	SetDeleteCallBack ( iID, pfn, userData );
}

void dbSetObjectDisableTransform ( int iID, bool bTransform )
{
	SetDisableTransform ( iID, bTransform );
}

void dbCreateMeshForObject ( int iID, DWORD dwFVF, DWORD dwVertexCount, DWORD dwIndexCount )
{
	CreateMeshForObject ( iID, dwFVF, dwVertexCount, dwIndexCount );
}

void dbSetObjectWorldMatrix ( int iID, D3DXMATRIX* pMatrix )
{
	SetWorldMatrix ( iID, pMatrix );
}

void dbUpdateObjectPositionStructure ( sPositionData* pPosition )
{
	UpdatePositionStructure ( pPosition );
}

void dbGetObjectWorldMatrix ( int iID, int iLimb, D3DXMATRIX* pMatrix )
{
	GetWorldMatrix ( iID, iLimb, pMatrix );
}

D3DXVECTOR3	dbGetObjectCameraLook ( void )
{
	return GetCameraLook ( );
}

D3DXVECTOR3	dbGetObjectCameraPosition ( void )
{
	return GetCameraPosition ( );
}

D3DXVECTOR3	dbGetObjectCameraUp ( void )
{
	return GetCameraUp ( );
}

D3DXVECTOR3	dbGetObjectCameraRight ( void )
{
	return GetCameraRight ( );
}

D3DXMATRIX dbGetObjectCameraMatrix ( void )
{
	return GetCameraMatrix ( );
}

sObject* dbGetObject ( int iID )
{
	return GetObject ( iID );
}

void dbCalculateObjectBounds ( int iID )
{
	CalculateObjectBounds ( iID );
}

void dbGetUniverseMeshList ( vector < sMesh* > *pMeshList )
{
	GetUniverseMeshList	( pMeshList );
}


void dbPositionObject ( int iID, float fX, float fY, float fZ )
{
	Position ( iID, fX, fY, fZ );
}

void dbScaleObject ( int iID, float fX, float fY, float fZ )
{
	Scale ( iID, fX, fY, fZ );
}

void dbRotateObject ( int iID, float fX, float fY, float fZ )
{
	Rotate ( iID, fX, fY, fZ );
}

void dbMoveObject ( int iID, float fStep )
{
	Move ( iID, fStep );
}

void dbPointObject ( int iID, float fX, float fY, float fZ )
{
	Point ( iID, fX, fY, fZ );
}

void dbMoveObjectDown ( int iID, float fStep )
{
	MoveDown ( iID, fStep );
}

void dbMoveObjectLeft ( int iID, float fStep )
{
	MoveLeft ( iID, fStep );
}

void dbMoveObjectRight ( int iID, float fStep )
{
	MoveRight ( iID, fStep );
}

void dbMoveObjectUp ( int iID, float fStep )
{
	MoveUp ( iID, fStep );
}

void dbXRotateObject ( int iID, float fX )
{
	XRotate ( iID, fX );
}

void dbYRotateObject ( int iID, float fY )
{
	YRotate ( iID, fY );
}

void dbZRotateObject ( int iID, float fZ )
{
	ZRotate ( iID, fZ );
}

void dbTurnObjectLeft ( int iID, float fAngle )
{
	TurnLeft ( iID, fAngle );
}

void dbTurnObjectRight ( int iID, float fAngle )
{
	TurnRight ( iID, fAngle );
}

void dbPitchObjectUp ( int iID, float fAngle )
{
	PitchUp ( iID, fAngle );
}

void dbPitchObjectDown ( int iID, float fAngle )
{
	PitchDown ( iID, fAngle );
}

void dbRollObjectLeft ( int iID, float fAngle )
{
	RollLeft ( iID, fAngle );
}

void dbRollObjectRight ( int iID, float fAngle )
{
	RollRight ( iID, fAngle );
}

float dbObjectPositionX ( int iID )
{
	DWORD dwReturn = GetXPositionEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectPositionY ( int iID )
{
	DWORD dwReturn = GetYPositionEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectPositionZ ( int iID )
{
	DWORD dwReturn = GetZPositionEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectAngleX ( int iID )
{
	DWORD dwReturn = GetXRotationEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectAngleY ( int iID )
{
	DWORD dwReturn = GetYRotationEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbObjectAngleZ ( int iID )
{
	DWORD dwReturn = GetZRotationEx ( iID );
	
	return *( float* ) &dwReturn;
}

void dbSetGlobalObjectCreationMode ( int iMode )
{
	SetGlobalObjectCreationMode ( iMode );
}

// mike - 230505 - need to be able to set mip map LOD bias on a per mesh basis
void dbSetObjectMipMapLODBias ( int iID, float fBias )
{
	SetObjectMipMapLODBias ( iID, fBias );
}

void dbSetObjectMipMapLODBias ( int iID, int iLimb, float fBias )
{
	SetObjectMipMapLODBias ( iID, iLimb, fBias );
}

// lee - 300706 - GDK fixes
void dbMakeObjectPlain ( int iID, float fWidth, float fHeight ) { return dbMakeObjectPlane (iID, fWidth, fHeight); }
void dbFadeObject ( int iID, int iPercentage ) { return dbFadeObject (iID, (float)iPercentage); }
void dbSetObjectSmoothing ( int iID, int iPercentage ) { return dbSetObjectSmoothing (iID, (float)iPercentage); }
void dbUnglueObject	( int iID ) { return dbUnGlueObject (iID); }
void dbSetAlphaMappingOn ( int iID, int iPercentage ) { return dbSetAlphaMappingOn (iID, (float)iPercentage); }
void dbSetLimbSmoothing	( int iID, int iLimbID, int iPercentage ) { return dbSetLimbSmoothing (iID, iLimbID, (float)iPercentage); }
bool dbAnistropicfilteringAvailable ( void ) { return GetAnisotropicFiltering (); }
bool dbBlitSysToLocalAvailable ( void ) { return GetBlitSysOntoLocal (); }
char* dbLimbTextureName	( int iID, int iLimbID ) { char* pStr=NULL; return (char*)GetLimbTextureNameEx ( (DWORD)pStr, iID, iLimbID, 0 ); }

// lee - 020906 - GDK tweaks
void dbPerformChecklistForObjectLimbs ( int iID ) { dbPerformCheckListForObjectLimbs ( iID ); }
char* dbLimbName ( int iID, int iLimbID ) { return ( char* ) GetLimbName ( NULL, iID, iLimbID ); }

// lee - 081108 - U71 - new built-in quad for post process shading
void dbTextureScreen ( int iStageIndex, int iImageID ) { TextureScreen(iStageIndex, iImageID); }
void dbSetScreenEffect ( int iEffectID ) { SetScreenEffect(iEffectID); }
//void dbRenderQuad ( void ) { RenderQuad(); }
int dbRenderQuad ( int iMode ) { return RenderQuad(iMode); }

#endif
