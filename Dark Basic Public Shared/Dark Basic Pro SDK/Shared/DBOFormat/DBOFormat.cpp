
//
// DBOFormat Functions Implementation
//

// mike - 120307 - comment out as it causes linker problems
/*
// mem leak dump check
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
*/

//////////////////////////////////////////////////////////////////////////////////////
// INCLUDES //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//#define _CRT_SECURE_NO_DEPRECATE
#include "DBOFormat.h"
#include "DBOFrame.h"
#include "DBORawMesh.h"
#include "DBOMesh.h"
#include "DBOFile.h"

//////////////////////////////////////////////////////////////////////////////////////
// ADDITIONAL INCLUDES ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4786)
#include "Extras\NVMeshMender.h"
#include "mmsystem.h"
#include "direct.h"
#include "string.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\ConvX\ConvX.h"
	#include ".\..\Conv3DS\Conv3DS.h"
	#include ".\..\ConvMD2\ConvMD2.h"
	#include ".\..\ConvMD3\ConvMD3.h"
	#include ".\..\ConvMDL\ConvMDL.h"
#endif

#include ".\..\Objects\CObjectManagerC.h"

//////////////////////////////////////////////////////////////////////////////////////
// EXTERNALS /////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
extern		LPDIRECT3DDEVICE9					m_pD3D;				// d3d device
extern		sEffectConstantData					g_EffectConstant;
extern		sObject**							g_ObjectList;
extern      float								g_fShrinkObjectsTo;
extern		CObjectManager						m_ObjectManager;

// Global
bool		g_bSwitchLegacyOn					= false;
float		g_fLastDeltaTime					= 0.0f;
bool		g_bFastBoundsCalculation			= true;
D3DXMATRIX	g_matThisViewProj;
D3DXMATRIX	g_matThisViewProjInverse;
D3DXMATRIX	g_matPreviousViewProj;

//////////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
D3DXPLANE				g_Planes [ 20 ][ NUM_CULLPLANES ];			// list of planes for frustum culling
DBPRO_GLOBAL D3DXVECTOR3			g_PlaneVector [ 20 ] [ NUM_CULLPLANES ];
DBPRO_GLOBAL sEffectConstantData	g_EffectConstant;							// used to store shader constants data
DWORD					g_dwEffectErrorMsgSize=0;
LPSTR					g_pEffectErrorMsg=NULL;
DWORD*					g_pConversionMap=NULL;						// single conversion array ptr held
char					g_WindowsTempDirectory[_MAX_PATH];

//////////////////////////////////////////////////////////////////////////////////////
// SPECIAL EFFECT FUNCTIONS //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

cSpecialEffect::cSpecialEffect ( )
{
	// reset effect ptr
	memset ( this, 0, sizeof(cSpecialEffect) );
}

cSpecialEffect::~cSpecialEffect ( )
{
	// free any RT textures we created
	/*
	int iParamTexArrayLimit = m_dwTextureCount;
	if ( iParamTexArrayLimit>32 ) iParamTexArrayLimit=32;
	for ( DWORD t=0; t<(DWORD)iParamTexArrayLimit; t++ )
	{
		DWORD dwThisTextureBit = 1 << t;
		if ( m_dwCreatedRTTextureMask & dwThisTextureBit )
		{
			// only RT textures are flagged in bitmask
			int iParam = m_iParamOfTexture[t];
			IDirect3DBaseTexture9* pRTTex = NULL;
			m_pEffect->GetTexture( m_pEffect->GetParameter( NULL, iParam ), &pRTTex );
			if ( pRTTex ) pRTTex->Release();
		}
	}
	*/
	// 080414 - Ravey reported old system not work, so we just track all allocations and then free them here
	if ( m_dwRTTexCount>0 )
	{
		for ( DWORD t=0; t<m_dwRTTexCount; t++ )
		{
			if ( m_pRTTex[t] )
			{
				m_pRTTex[t]->Release();
			}
		}
	}

	// free default zfile mesh
	SAFE_DELETE ( m_pXFileMesh );

	// free effect
	SAFE_RELEASE ( m_pEffect );
	SAFE_DELETE ( m_pDefaultXFile );
}

bool cSpecialEffect::CorrectFXFile ( LPSTR pFile, LPSTR pModifiedFile )
{
	// result var
	bool bResult=false;

	// read in original file
	HANDLE hreadfile = CreateFile(pFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hreadfile!=INVALID_HANDLE_VALUE)
	{
		// read file into memory
		DWORD bytesread;
		int filebuffersize = GetFileSize(hreadfile, NULL);	
		char* filebuffer = (char*)GlobalAlloc(GMEM_FIXED, filebuffersize);
		ReadFile(hreadfile, filebuffer, filebuffersize, &bytesread, NULL); 
		CloseHandle(hreadfile);		

		// Items an FX file may be missing
		bool bDCLTokensExist=false;
		bool bInvalidTargetTokenUsed=false;

		// scan and modify for corrections
		LPSTR pPtr = filebuffer;
		LPSTR pPtrEnd = filebuffer+filebuffersize;
		while ( pPtr<pPtrEnd )
		{
			// check for existance of commonly missing data
			if ( _strnicmp ( pPtr, "dcl_", 4 )==NULL ) bDCLTokensExist=true;
			if ( _strnicmp ( pPtr, "target[", 7 )==NULL ) bInvalidTargetTokenUsed=true;

			// next byte
			pPtr++;
		}

		// write corrections in new data
		int newbuffersize = filebuffersize*2;	
		char* newbuffer = (char*)GlobalAlloc(GMEM_FIXED, newbuffersize);
		LPSTR pWritePtr = newbuffer;
		LPSTR pWritePtrEnd = newbuffer+newbuffersize;

		// go through data again
		pPtr = filebuffer;
		while ( pPtr<pPtrEnd )
		{
			// change any VS to include standard declarations
			if ( bDCLTokensExist==false )
			{
				// from vs.1.1 to vs.1.1 dcl_position v0 dcl_normal v3 etc
				if ( _strnicmp ( pPtr, "vs.", 3 )==NULL )
				{
					// vs.x.x
					memcpy ( pWritePtr, pPtr, 6 ); //vs.x.x
					pWritePtr+=6;
					pPtr+=6;

					// add dcls
					memcpy ( pWritePtr, (LPSTR)" dcl_position v0", 16 ); pWritePtr+=16;
					memcpy ( pWritePtr, (LPSTR)" dcl_normal v3", 14 ); pWritePtr+=14;
					memcpy ( pWritePtr, (LPSTR)" dcl_color v6", 13 ); pWritePtr+=13;
					memcpy ( pWritePtr, (LPSTR)" dcl_texcoord v7", 16 ); pWritePtr+=16;
				}
			}

			// change any target tokens by commenting them out
			if ( bInvalidTargetTokenUsed==true )
			{
				if ( _strnicmp ( pPtr, "target[", 7 )==NULL )
				{
					*pWritePtr = '/'; pWritePtr++;
					*pWritePtr = '/'; pWritePtr++;
					pPtr+=2;
				}
			}

			// write from original data to new buffer
			*pWritePtr = *pPtr;

			// next bytes
			pWritePtr++;
			pPtr++;
		}

		// write new temp file
		DWORD actualnewdatasize = pWritePtr-newbuffer;
		HANDLE hwritefile = CreateFile(pModifiedFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hwritefile!=INVALID_HANDLE_VALUE)
		{
			// write new data
			DWORD byteswritten;
			WriteFile(hwritefile, newbuffer, actualnewdatasize, &byteswritten, NULL); 
			CloseHandle(hwritefile);

			// success
			bResult=true;
		}

		// free usages
		if ( filebuffer ) { GlobalFree ( filebuffer ); }
		if ( newbuffer ) { GlobalFree ( newbuffer ); }
	}

	// failed
	return bResult;
}

bool cSpecialEffect::Load ( LPSTR pEffectFile, bool bUseXFile, bool bUseTextures )
{
	// result
	HRESULT hr;

	// record effect name
	strcpy ( m_pEffectName, pEffectFile );

	// split off path and switch for local resource loading
	char pPath[_MAX_PATH];
	char pFile[_MAX_PATH];
	strcpy ( pPath, "" );
	strcpy ( pFile, pEffectFile );
	for ( int n=strlen(pEffectFile); n>0; n--)
	{
		if ( pEffectFile[n]=='\\' ||  pEffectFile[n]=='/' )
		{
			// get path and file
			strcpy ( pFile, pEffectFile+n+1 );
			strcpy ( pPath, pEffectFile );
			pPath[n]=0;
			break;
		}
	}

	// buffer to hold error
	ID3DXBuffer* pErrorBuffer = NULL;

	// store old directory and set local one
	char pOldDir[_MAX_PATH];
	_getcwd(pOldDir, _MAX_PATH);
	if ( strlen(pPath)>0 ) _chdir(pPath);

	// Shader Legacy Mode for later DXSDK
	DWORD dwShaderLegacyMode = 0;
	#ifdef DARKSDK_COMPILE
        #ifdef D3DXSHADER_USE_LEGACY_D3DX9_31_DLL
		    dwShaderLegacyMode = D3DXSHADER_USE_LEGACY_D3DX9_31_DLL;
        #else
			// lee - 131010 - DarkGDK should support The October 2006 SDK (for legacy users)
			// previous DarkGDK builds may have used Aug 2007 (35.DLL)
            // #error You should be using DX SDK Aug 2007 or later
        #endif
	#endif

	// 131213 - improve shader loading for Reloaded
	dwShaderLegacyMode = D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY;

	// 160214 - so I can debug in PIX
	//#if defined( DEBUG ) || defined( _DEBUG )
	//	dwShaderLegacyMode |= D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
	//#endif

	// load effect from local file first
	if(FAILED(hr = D3DXCreateEffectFromFileA(m_pD3D, pFile,
					NULL, NULL, dwShaderLegacyMode, NULL, &m_pEffect, &pErrorBuffer )))
	{
		// calculate temp folder location
		DBOCalculateLoaderTempFolder();

		// leefix - 310305 - alter path if in debugmode(plguins last), swiotch to effect
		char pFinalFile[_MAX_PATH];
		strcpy ( pFinalFile, g_WindowsTempDirectory );
		if ( _strnicmp ( "plugins\\", (g_WindowsTempDirectory + strlen(g_WindowsTempDirectory)) - 8, 8 )==NULL ) 
		{
			// switch from plugins to effects (for finding an internal FX file)
			strcpy ( pFinalFile, g_WindowsTempDirectory );
			pFinalFile [ strlen(pFinalFile) - 8 ] = 0;
			strcat ( pFinalFile, "effects\\" );
		}

		// try from dbpdata folder
		strcat ( pFinalFile, pFile );

		// load effect from internal file second
		if(FAILED(hr = D3DXCreateEffectFromFileA(m_pD3D, pFinalFile,
						NULL, NULL, dwShaderLegacyMode, NULL, &m_pEffect, &pErrorBuffer )))
		{
			// third possibility is that it is in the TEMP media folder
			strcpy ( pFinalFile, g_WindowsTempDirectory );
			strcat ( pFinalFile, "media\\" );
			strcat ( pFinalFile, pEffectFile );
			if(FAILED(hr = D3DXCreateEffectFromFileA(m_pD3D, pFinalFile,
							NULL, NULL, dwShaderLegacyMode, NULL, &m_pEffect, &pErrorBuffer )))
			{
				// try from dbpdata folder
				char pModifiedFile[_MAX_PATH];
				strcpy ( pModifiedFile, g_WindowsTempDirectory );
				strcat ( pModifiedFile, "_modified_fx.fx" );

				// forth possibility is that the local FX file cannot be parsed (nvidia)
				if ( CorrectFXFile ( pFile, pModifiedFile ) )
				{
					// load effect from local file first
					if(FAILED(hr = D3DXCreateEffectFromFileA(m_pD3D, pModifiedFile,
									NULL, NULL, dwShaderLegacyMode, NULL, &m_pEffect, &pErrorBuffer )))
					{
						// simply cannot payse the FX file
						DeleteFile ( pModifiedFile );
						goto failed;
					}

					// Remove temp file
					DeleteFile ( pModifiedFile );
				}
				else
				{
					// cannot correct FX file
					goto failed;
				}
			}
			else
			{
				// The TEMP\MEDIA was the right folder, need to switch CWD to it
				strcpy ( pFinalFile, g_WindowsTempDirectory );
				strcat ( pFinalFile, "media\\" );
				strcat ( pFinalFile, pPath );
				_chdir(pFinalFile);
			}
		}
	}

	// Associate data in effect with app data
	ParseEffect ( bUseXFile, bUseTextures );
	m_bUseShaderTextures = bUseTextures;

	// find valid technique
	D3DXHANDLE hTechnique;
	if(FAILED(hr = m_pEffect->FindNextValidTechnique(NULL, &hTechnique)))
	{
		SAFE_RELEASE(pErrorBuffer);
		_chdir(pOldDir);
		return false;
	}

	// set the first valid technique (DBPro FX files orders best to worst)
	if ( hTechnique )
		m_pEffect->SetTechnique(hTechnique);

	// restores
	SAFE_RELEASE(pErrorBuffer);
	_chdir(pOldDir);

	// complete
	return true;

	failed:

	// handle error buffer
	if ( pErrorBuffer ) 
	{
		g_dwEffectErrorMsgSize = pErrorBuffer->GetBufferSize();
		SAFE_DELETE(g_pEffectErrorMsg);
		g_pEffectErrorMsg = new char[g_dwEffectErrorMsgSize+1];
		memcpy ( g_pEffectErrorMsg, pErrorBuffer->GetBufferPointer(), g_dwEffectErrorMsgSize );
		MessageBox ( NULL, g_pEffectErrorMsg, g_pEffectErrorMsg, MB_OK );
	}

	// restores
	SAFE_RELEASE(pErrorBuffer);
	_chdir(pOldDir);

	// failure
	return false;
}

bool cSpecialEffect::Setup ( sMesh* pMesh )
{
	// alter mesh with default xfile mesh
	if ( m_pXFileMesh ) MakeLocalMeshFromOtherLocalMesh ( pMesh, m_pXFileMesh );

	// complete
	return true;
}

void cSpecialEffect::Mesh ( sMesh* pMesh )
{
	// change mesh to suit effect
	// leeadd - 200204 - add bone data to mesh FVF if shader requests it
	// leeadd - 121208 - U71 - was (==0) now go into generate if 0 to 2 (first bit zero)
	if ( m_bDoNotGenerateExtraData==0 || m_bDoNotGenerateExtraData==2 )
	{
		// leeadd - 050906 - auto-generate if not flagged off to keep object pure
		GenerateExtraDataForMeshEx ( pMesh, m_bGenerateNormals, m_bUsesTangents, m_bUsesBinormals, m_bUsesDiffuse, m_bUsesBoneData, m_bDoNotGenerateExtraData );
	}

	// lee - 230306 - u6b4 - also generate a new 'original vertexdata store', otherwise CPU bone animate will be out of phase and crash
	if ( pMesh )
	{
		SAFE_DELETE ( pMesh->pOriginalVertexData );
		CollectOriginalVertexData ( pMesh );
	}
}

DWORD cSpecialEffect::Start	( sMesh* pMesh, D3DXMATRIX matObject )
{
	// if a valid FX effect
	if ( m_pEffect )
	{
		// now replace effect textures (usually none) with mesh textures
		// LEELEE : Performance warning - is this slow or fast - good flexibility though!
		// LEELEE : This seems to cause massive slowdown when texturing NODETREE meshes!
		// Probably because it is swapping the same texture in the effect many times.
		// Solution is we should be sorting by EFFECT, then by TEXTURE..TODO!
		int iParamTexArrayLimit = m_dwTextureCount;
		if ( iParamTexArrayLimit>32 ) iParamTexArrayLimit=32;
		for ( DWORD t=0; t<(DWORD)iParamTexArrayLimit; t++ )
		{
			int iParam = m_iParamOfTexture[t];
			if ( t<pMesh->dwTextureCount )
			{
				D3DXHANDLE pParam = m_pEffect->GetParameter( NULL, iParam );
				if ( pParam )
				{
					LPDIRECT3DTEXTURE9 pTexToUse = pMesh->pTextures [ t ].pTexturesRef;
					m_pEffect->SetTexture( pParam, pTexToUse );
				}
			}
		}

		// var
		UINT uPasses;

		// begin effect and return number of required passes
		m_pEffect->Begin(&uPasses, 0 );

		// Apply associations to effect using latest application data
		ApplyEffect ( pMesh );

		// passes count
		return (DWORD)uPasses;
	}
	else
		return 0;
}

void cSpecialEffect::End ( void )
{
	// if a valid FX effect
	if ( m_pEffect )
	{
		m_pEffect->End();
	}
}

bool cSpecialEffect::AssignValueHookCore ( LPSTR pName, D3DXHANDLE hParam, DWORD dwClass, bool bRemove )
{
	#define ASSIGNNAME(a,b)	if ( bRemove ) { if ( b==hParam ) { b=NULL; return true; } } else { if ( _stricmp ( pName, a )==0 ) { if ( hParam ) { b=hParam; return true; } else { if ( b ) return true;  }; } };

	// auto-fail if no name
	if ( bRemove==false && pName==NULL ) return false;

	// COMMON UNTWEAKABLES
	ASSIGNNAME ( "world", m_MatWorldEffectHandle );
	ASSIGNNAME ( "view", m_MatViewEffectHandle );
	ASSIGNNAME ( "projection", m_MatProjEffectHandle );
	ASSIGNNAME ( "worldview", m_MatWorldViewEffectHandle );
	ASSIGNNAME ( "viewprojection", m_MatViewProjEffectHandle );
	ASSIGNNAME ( "worldviewprojection", m_MatWorldViewProjEffectHandle );

	// MS UNTWEAKABLES
	ASSIGNNAME ( "worldviewit", m_MatWorldViewInverseEffectHandle );
	ASSIGNNAME ( "worldit", m_MatWorldInverseEffectHandle );
	ASSIGNNAME ( "viewit", m_MatViewInverseEffectHandle );

	// NVIDIA UNTWEAKABLES
	ASSIGNNAME ( "WorldInverse", m_MatWorldInverseEffectHandle );
	ASSIGNNAME ( "WorldTranspose", m_MatWorldTEffectHandle );
	ASSIGNNAME ( "WorldInverseTranspose", m_MatWorldInverseTEffectHandle );
	ASSIGNNAME ( "ViewInverse", m_MatViewInverseEffectHandle );
	ASSIGNNAME ( "ViewTranspose", m_MatViewTEffectHandle );
	ASSIGNNAME ( "ViewInverseTranspose", m_MatViewInverseTEffectHandle );
	ASSIGNNAME ( "ProjectionTranspose", m_MatProjTEffectHandle );
	ASSIGNNAME ( "WorldViewTranspose", m_MatWorldViewTEffectHandle );
	ASSIGNNAME ( "ViewProjectionTranspose", m_MatViewProjTEffectHandle );
	ASSIGNNAME ( "WorldViewProjectionTranspose", m_MatWorldViewProjTEffectHandle );
	ASSIGNNAME ( "WorldViewInverse", m_MatWorldViewInverseEffectHandle );

	// vectors
	ASSIGNNAME ( "cameraposition", m_VecCameraPosEffectHandle );
	ASSIGNNAME ( "eyeposition", m_VecEyePosEffectHandle );
	ASSIGNNAME ( "clipplane", m_VecClipPlaneEffectHandle );

	// MS lighting
	ASSIGNNAME ( "UIDirectional", m_LightDirHandle );
	ASSIGNNAME ( "UIDirectionalInv", m_LightDirInvHandle );
	ASSIGNNAME ( "UIPosition", m_LightPosHandle );

	// NVIDIA lighting
	ASSIGNNAME ( "directionalight", m_LightDirHandle );
	ASSIGNNAME ( "pointlight", m_LightPosHandle );
	ASSIGNNAME ( "spotlight", m_LightPosHandle );

	// rogue scalars
	ASSIGNNAME ( "time", m_TimeEffectHandle );
	ASSIGNNAME ( "sintime", m_SinTimeEffectHandle );
	ASSIGNNAME ( "deltatime", m_DeltaTimeEffectHandle );
	ASSIGNNAME ( "uvscaling", m_UVScalingHandle );
	
	//
	// DBPRO UNTWEAKABLES
	//

	ASSIGNNAME ( "alphaoverride", m_AlphaOverrideHandle );
	ASSIGNNAME ( "bonecount", m_BoneCountHandle );
	ASSIGNNAME ( "bonematrixpalette", m_BoneMatrixPaletteHandle );

	// non-handle hook values
	if ( pName )
		if ( _strcmpi ( pName, "xfile" )==0 )
			return true;

	// could not find name match
	return false;
}

bool cSpecialEffect::AssignValueHook ( LPSTR pName, D3DXHANDLE hParam )
{
	return AssignValueHookCore ( pName, hParam, 0, false );
}

bool cSpecialEffect::ParseEffect ( bool bUseEffectXFile, bool bUseEffectTextures )
{
	// if no effect, skip
	if( m_pEffect == NULL )
		return false;

	// Used to assign from mesh textures
	m_dwTextureCount = 0;

	// get effect description
	m_pEffect->GetDesc( &m_EffectDesc );

	// u64 - 180107 - new mask to hold dynamic tecture flags
	m_dwUseDynamicTextureMask = 0; // default is effect uses NO dynamic textures
	DWORD dwCountTexturesInEffect = 0;

	// U75 - 200310 - clear RT mask as well
	m_dwCreatedRTTextureMask = 0; 
	m_bUsesAtLeastOneRT = false;

	// Look at parameters for semantics and annotations that we know how to interpret
	D3DXPARAMETER_DESC ParamDesc;
	D3DXHANDLE hParam;
	for( UINT iParam = 0; iParam < m_EffectDesc.Parameters; iParam++ )
	{
		// temp vars
		LPCSTR pstrName = NULL;
		LPCSTR pstrFunction = NULL;
		LPCSTR pstrTarget = NULL;
		LPCSTR pstrTextureType = NULL;
		INT Width = D3DX_DEFAULT;
		INT Height= D3DX_DEFAULT;
		INT Depth = D3DX_DEFAULT;

		// get this parameter handle and description
		hParam = m_pEffect->GetParameter ( NULL, iParam );
		m_pEffect->GetParameterDesc( hParam, &ParamDesc );

		// light handles from DX9 FX files
	    D3DXHANDLE hAnnot = m_pEffect->GetAnnotationByName( hParam, "UIDirectional" );
        if( hAnnot != NULL ) AssignValueHook ( "UIDirectional", hParam );
	    hAnnot = m_pEffect->GetAnnotationByName( hParam, "UIDirectionalInv" );
        if( hAnnot != NULL ) AssignValueHook ( "UIDirectionalInv", hParam );
	    hAnnot = m_pEffect->GetAnnotationByName( hParam, "UIPosition" );
        if( hAnnot != NULL ) AssignValueHook ( "UIPosition", hParam );

		// light handles from NVIDIA FX files
	    hAnnot = m_pEffect->GetAnnotationByName( hParam, "Object" );
	    if( hAnnot == NULL ) hAnnot = m_pEffect->GetAnnotationByName( hParam, "UIObject" );
        if( hAnnot != NULL )
		{
			// light type
			LPCSTR pstrLightType = NULL;
			if( hAnnot != NULL ) m_pEffect->GetString( hAnnot, &pstrLightType );

			// light space
			LPCSTR pstrLightSpace = NULL;
		    hAnnot = m_pEffect->GetAnnotationByName( hParam, "Space" );
			if( hAnnot != NULL ) m_pEffect->GetString( hAnnot, &pstrLightSpace );

			// assign light position hanle
			AssignValueHook ( (char*)pstrLightType, hParam );
		}

		// basic matrix semantics
		if( ParamDesc.Semantic != NULL && ( ParamDesc.Class == D3DXPC_MATRIX_ROWS || ParamDesc.Class == D3DXPC_MATRIX_COLUMNS ) )
			AssignValueHookCore ( (char*)ParamDesc.Semantic, hParam, ParamDesc.Class, false );

		// basic vector semantics
		if( ParamDesc.Semantic != NULL && ( ParamDesc.Class == D3DXPC_VECTOR ))
			AssignValueHook ( (char*)ParamDesc.Semantic, hParam );

		// basic value semantics
		if( ParamDesc.Semantic != NULL && ( ParamDesc.Class == D3DXPC_SCALAR ))
			AssignValueHook ( (char*)ParamDesc.Semantic, hParam );

		D3DXPARAMETER_DESC AnnotDesc;
		for( UINT iAnnot = 0; iAnnot < ParamDesc.Annotations; iAnnot++ )
		{
			hAnnot = m_pEffect->GetAnnotation ( hParam, iAnnot );
			m_pEffect->GetParameterDesc( hAnnot, &AnnotDesc );

			// texture name
			if ( _strcmpi( AnnotDesc.Name, "resourcename" ) == 0 )		m_pEffect->GetString( hAnnot, &pstrName );
			if ( _strcmpi( AnnotDesc.Name, "name" ) == 0 )		m_pEffect->GetString( hAnnot, &pstrName );
			if ( _strcmpi( AnnotDesc.Name, "file" ) == 0 )		m_pEffect->GetString( hAnnot, &pstrName );

			// texture type
			if ( _strcmpi( AnnotDesc.Name, "resourcetype" ) == 0 )		m_pEffect->GetString( hAnnot, &pstrTextureType );
			if ( _strcmpi( AnnotDesc.Name, "type" ) == 0 )		m_pEffect->GetString( hAnnot, &pstrTextureType );
			if ( _strcmpi( AnnotDesc.Name, "texturetype" ) == 0 )m_pEffect->GetString( hAnnot, &pstrTextureType );

			// texture details
			if ( _strcmpi( AnnotDesc.Name, "function" ) == 0 )	m_pEffect->GetString( hAnnot, &pstrFunction );
			if ( _strcmpi( AnnotDesc.Name, "target" ) == 0 )		m_pEffect->GetString( hAnnot, &pstrTarget );
			if ( _strcmpi( AnnotDesc.Name, "width" ) == 0 )		m_pEffect->GetInt( hAnnot, &Width );
			if ( _strcmpi( AnnotDesc.Name, "height" ) == 0 )		m_pEffect->GetInt( hAnnot, &Height );
			if ( _strcmpi( AnnotDesc.Name, "depth" ) == 0 )		m_pEffect->GetInt( hAnnot, &Depth );
		}
		if( pstrName != NULL )
		{
			// U75 - 200310 - detect RENDERCOLORTARGET semantic here
			bool bIsThisAnRT = false;
			if ( ParamDesc.Semantic != NULL ) if ( _strcmpi ( ParamDesc.Semantic, "RENDERCOLORTARGET" ) == 0 ) bIsThisAnRT = true;
			if ( bIsThisAnRT==true )
			{
				// this indicates an RT texture we want our shader to render to during the passes, so we need to create a render target for it
				IDirect3DTexture9* pRTTex = NULL;
				D3DSURFACE_DESC desc;
				IDirect3DSurface9 *pCurrentRenderTarget = NULL;
				m_pD3D->GetRenderTarget(0,&pCurrentRenderTarget);
				if ( pCurrentRenderTarget )
				{
					pCurrentRenderTarget->GetDesc( &desc );
					if ( Width==D3DX_DEFAULT ) Width=desc.Width;
					if ( Height==D3DX_DEFAULT ) Height=desc.Height;
				}
				else
				{
					if ( Width==D3DX_DEFAULT ) Width=256;
					if ( Height==D3DX_DEFAULT ) Height=256;
				}
				D3DXCreateTexture( m_pD3D, Width, Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, (IDirect3DTexture9**)&pRTTex );

				// assigns RT texture directly to effect
				m_pEffect->SetTexture( m_pEffect->GetParameter( NULL, iParam ), pRTTex );

				// set flag to indicate this specialeffect object uses at least one RT (render target)
				m_bUsesAtLeastOneRT = true;

				// mark in a bitfield which textures are RT (so we can release them when this shader is deleted)
				DWORD dwCorrectBitForThisTexture = 1 << m_dwTextureCount;
				m_dwCreatedRTTextureMask = m_dwCreatedRTTextureMask | dwCorrectBitForThisTexture;

				// record this texture and step through texture indexes
				if ( m_dwTextureCount<=31 ) m_iParamOfTexture [ m_dwTextureCount ] = iParam;
				m_dwTextureCount++;

				// record this now for later release
				if ( m_dwRTTexCount<=31 ) m_pRTTex [ m_dwRTTexCount ] = pRTTex;
				m_dwRTTexCount++;
			}
			else
			{
				// texture from effect or mesh
				if ( bUseEffectTextures )
				{
					// texture holder
					LPDIRECT3DBASETEXTURE9 pTex = NULL;

					// 2D texture is stadnard texture
					if (pstrTextureType != NULL) 
						if( _strcmpi( pstrTextureType, "2d" ) == 0 )
							pstrTextureType=NULL;

					// assign effect texture from FX file
					if (pstrTextureType != NULL) 
					{
						if( _strcmpi( pstrTextureType, "volume" ) == 0 )
						{
							// support for internal volume textures
							LPDIRECT3DVOLUMETEXTURE9 pVolumeTex = NULL;
							if( SUCCEEDED( D3DXCreateVolumeTextureFromFileEx( m_pD3D, pstrName, 
								Width, Height, Depth, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
								D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pVolumeTex ) ) )
							{
								// iTextureStage
								pTex = pVolumeTex;
							}
						}
						else if( _strcmpi( pstrTextureType, "cube" ) == 0 )
						{
							// support for internal cube textures
							LPDIRECT3DCUBETEXTURE9 pCubeTex = NULL;
							if( SUCCEEDED( D3DXCreateCubeTextureFromFileEx( m_pD3D, pstrName, 
								Width, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
								D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pCubeTex ) ) )
							{
								// iTextureStage
								pTex = pCubeTex;
							}
						}

						// record this now for later release
						if ( m_dwRTTexCount<=31 ) m_pRTTex [ m_dwRTTexCount ] = pTex;
						m_dwRTTexCount++;
					}
					else
					{
						// support for internal basic textures
						int iImageIndex = LoadOrFindTextureAsImage ( (char*)pstrName, "" );
						pTex = g_Image_GetPointer ( iImageIndex );
					}

					// assign texture to effect
					if ( pTex )
					{
						// assigns effect texture directly to effect
						m_pEffect->SetTexture( m_pEffect->GetParameter( NULL, iParam ), pTex );
					}
					else
					{
						// u64 - 180107 - set the bit to say this texture stage should use a dynamic texture from texture object command
						DWORD dwCorrectBitForThisStage = 1 << dwCountTexturesInEffect;
						m_dwUseDynamicTextureMask = m_dwUseDynamicTextureMask | dwCorrectBitForThisStage;
					}
					dwCountTexturesInEffect++;
				}
				else
				{
					// record this texture and step through texture indexes
					if ( m_dwTextureCount<=31 ) m_iParamOfTexture [ m_dwTextureCount ] = iParam;
					m_dwTextureCount++;
				}
			}
		}
	}

	// Look for default mesh
	if ( bUseEffectXFile )
	{
		D3DXPARAMETER_DESC Desc;
		if( NULL != m_pEffect->GetParameterByName( NULL, "XFile" ) &&
			SUCCEEDED( m_pEffect->GetParameterDesc( "XFile", &Desc ) ) )
		{
			// Store default xfile name
			const char* pStr = NULL;
			m_pEffect->GetString("XFile", &pStr);
			if ( pStr )
			{
				// get default xfile name
				m_pDefaultXFile = new char[strlen(pStr)+1];
				strcpy ( m_pDefaultXFile, pStr );

				// change current mesh with
				LoadRawMesh ( m_pDefaultXFile, &m_pXFileMesh );
			}
		}
	}

	// Look for normals/diffuse/tangents/binormals semantic
	// leefix - 131103 - works once more with DX9SUMMER
	D3DXEFFECT_DESC EffectDesc;
	D3DXHANDLE hTechnique;
	D3DXTECHNIQUE_DESC TechniqueDesc;
	D3DXHANDLE hPass;
	D3DXPASS_DESC PassDesc;
	m_bUsesNormals = FALSE;
	m_bUsesDiffuse = FALSE;
	m_bUsesTangents = FALSE;
	m_bUsesBinormals = FALSE;

	// leeadd - 030304 - determine if shader 'skins' be existence of the matrix palette
	if ( m_BoneMatrixPaletteHandle )
		m_bUsesBoneData = TRUE;
	else
		m_bUsesBoneData = FALSE;

	m_pEffect->GetDesc( &EffectDesc );
	for( UINT iTech = 0; iTech < EffectDesc.Techniques; iTech++ )
	{
		hTechnique = m_pEffect->GetTechnique( iTech );
		m_pEffect->GetTechniqueDesc( hTechnique, &TechniqueDesc );
		for( UINT iPass = 0; iPass < TechniqueDesc.Passes; iPass++ )
		{
			hPass = m_pEffect->GetPass( hTechnique, iPass );
			m_pEffect->GetPassDesc( hPass, &PassDesc );

            UINT NumVSSemanticsUsed;
            D3DXSEMANTIC pVSSemantics[MAXD3DDECLLENGTH];

            #ifndef __GNUC__
            if( !PassDesc.pVertexShaderFunction || FAILED( D3DXGetShaderInputSemantics( PassDesc.pVertexShaderFunction, pVSSemantics, &NumVSSemanticsUsed ) ) )
                continue;

			for( UINT iSem = 0; iSem < NumVSSemanticsUsed; iSem++ )
			{
				if( pVSSemantics[iSem].Usage == D3DDECLUSAGE_NORMAL ) m_bUsesNormals = TRUE;
				if( pVSSemantics[iSem].Usage == D3DDECLUSAGE_COLOR ) m_bUsesDiffuse = TRUE;
				if( pVSSemantics[iSem].Usage == D3DDECLUSAGE_TANGENT ) m_bUsesTangents = TRUE;
				if( pVSSemantics[iSem].Usage == D3DDECLUSAGE_BINORMAL ) m_bUsesBinormals = TRUE;
			}
			#endif
		}
	}

	// complete
	return true;
}

void cSpecialEffect::ApplyEffect ( sMesh* pMesh )
{
	// Gather and calculate required constants data
    m_pD3D->GetTransform( D3DTS_WORLD, &g_EffectConstant.matWorld );
    m_pD3D->GetTransform( D3DTS_VIEW, &g_EffectConstant.matView );
    m_pD3D->GetTransform( D3DTS_PROJECTION, &g_EffectConstant.matProj );
    g_EffectConstant.matWorldView = g_EffectConstant.matWorld * g_EffectConstant.matView;
    g_EffectConstant.matViewProj = g_EffectConstant.matView * g_EffectConstant.matProj;
    g_EffectConstant.matWorldViewProj = g_EffectConstant.matWorld * g_EffectConstant.matView * g_EffectConstant.matProj;
 
	// Calculate inverse matrices
	D3DXMatrixInverse( &g_EffectConstant.matWorldInv, NULL, &g_EffectConstant.matWorld );
	D3DXMatrixInverse( &g_EffectConstant.matViewInv, NULL, &g_EffectConstant.matView );
	D3DXMatrixInverse( &g_EffectConstant.matWorldViewInv, NULL, &g_EffectConstant.matWorldView );

	// Get raw light data
	D3DLIGHT9 d3dLight;
	m_pD3D->GetLight(0,&d3dLight);
	if ( d3dLight.Type==D3DLIGHT_DIRECTIONAL )
	{
		// get direction directly from structure
	    g_EffectConstant.vecLightDir = D3DXVECTOR4( d3dLight.Direction.x, d3dLight.Direction.y, d3dLight.Direction.z, 0.0f );
	    g_EffectConstant.vecLightPos = D3DXVECTOR4( d3dLight.Direction.x*-1000.0f, d3dLight.Direction.y*-1000.0f, d3dLight.Direction.z*-1000.0f, 1.0f );
	}
	else
	{
		// calculate direction from world position
		g_EffectConstant.vecLightDir.x = g_EffectConstant.matWorld._41-d3dLight.Position.x;
		g_EffectConstant.vecLightDir.y = g_EffectConstant.matWorld._42-d3dLight.Position.y;
		g_EffectConstant.vecLightDir.z = g_EffectConstant.matWorld._43-d3dLight.Position.z;
		g_EffectConstant.vecLightDir.w = 1.0f;

		// calculate light position (in object space)
	    g_EffectConstant.vecLightPos.x = d3dLight.Position.x;
	    g_EffectConstant.vecLightPos.y = d3dLight.Position.y;
	    g_EffectConstant.vecLightPos.z = d3dLight.Position.z;
	    g_EffectConstant.vecLightPos.w = 1.0f;
	}

	// Calculate light and object-space light(inv)
	D3DXVec4Transform ( &g_EffectConstant.vecLightDirInv, &g_EffectConstant.vecLightDir, &g_EffectConstant.matWorldInv );
	D3DXVec4Normalize ( &g_EffectConstant.vecLightDirInv, &g_EffectConstant.vecLightDirInv );
	D3DXVec4Normalize ( &g_EffectConstant.vecLightDir, &g_EffectConstant.vecLightDir );

	// Get camera psition
    g_EffectConstant.vecCameraPosition = D3DXVECTOR4( g_EffectConstant.matViewInv._41, g_EffectConstant.matViewInv._42, g_EffectConstant.matViewInv._43, 1.0f );
	g_EffectConstant.vecEyePos = g_EffectConstant.vecCameraPosition;

	// Alpha override component
	if ( m_AlphaOverrideHandle )
	{
		float fPercentage = 1.0f;
		if ( pMesh->bAlphaOverride==true )
			fPercentage = (float)(pMesh->dwAlphaOverride>>24)/255.0f;
		m_pEffect->SetFloat( m_AlphaOverrideHandle, fPercentage );
	}

	// prepare tranposed matrices for column major matrices
	if ( m_bTranposeToggle )
	{
		D3DXMatrixTranspose( &g_EffectConstant.matWorld, &g_EffectConstant.matWorld );
		D3DXMatrixTranspose( &g_EffectConstant.matView, &g_EffectConstant.matView );
		D3DXMatrixTranspose( &g_EffectConstant.matProj, &g_EffectConstant.matProj );
		D3DXMatrixTranspose( &g_EffectConstant.matWorldView, &g_EffectConstant.matWorldView );
		D3DXMatrixTranspose( &g_EffectConstant.matViewProj, &g_EffectConstant.matViewProj );
		D3DXMatrixTranspose( &g_EffectConstant.matWorldViewProj, &g_EffectConstant.matWorldViewProj );
		D3DXMatrixTranspose( &g_EffectConstant.matWorldInv, &g_EffectConstant.matWorldInv );
		D3DXMatrixTranspose( &g_EffectConstant.matViewInv, &g_EffectConstant.matViewInv );
		D3DXMatrixTranspose( &g_EffectConstant.matWorldViewInv, &g_EffectConstant.matWorldViewInv );
	}

	// leeadd - 290104 - addition of tranposed matrices for effects that use them
	D3DXMatrixTranspose( &g_EffectConstant.matWorldT, &g_EffectConstant.matWorld );
	D3DXMatrixTranspose( &g_EffectConstant.matViewT, &g_EffectConstant.matView );
	D3DXMatrixTranspose( &g_EffectConstant.matProjT, &g_EffectConstant.matProj );
	//D3DXMatrixTranspose( &g_EffectConstant.matWorldViewT, &g_EffectConstant.matWorldView );
	//D3DXMatrixTranspose( &g_EffectConstant.matViewProjT, &g_EffectConstant.matViewProj );
	//D3DXMatrixTranspose( &g_EffectConstant.matWorldViewProjT, &g_EffectConstant.matWorldViewProj ); // 270515 - could be overridden below
	D3DXMatrixTranspose( &g_EffectConstant.matWorldInvT, &g_EffectConstant.matWorldInv );
	D3DXMatrixTranspose( &g_EffectConstant.matViewInvT, &g_EffectConstant.matViewInv );
	D3DXMatrixTranspose( &g_EffectConstant.matWorldViewInvT, &g_EffectConstant.matWorldViewInv );

	// 270515 - for depth texture motion blur we need the previous worldviewproj
	// and related matrices (from end of frame NOT from this post process camera!)
	g_EffectConstant.matWorldViewT = g_matThisViewProj;
	g_EffectConstant.matViewProjT = g_matThisViewProjInverse;
	g_EffectConstant.matWorldViewProjT = g_matPreviousViewProj;

	// apply latest data to effect
    if( m_pEffect != NULL )
    {
		// main matrices (row major)
        if( m_MatWorldEffectHandle != NULL )
		{
            m_pEffect->SetMatrix( m_MatWorldEffectHandle, &g_EffectConstant.matWorld );
        }
        if( m_MatViewEffectHandle != NULL )
		{
            m_pEffect->SetMatrix( m_MatViewEffectHandle, &g_EffectConstant.matView );
        }
        if( m_MatProjEffectHandle != NULL )
		{
            m_pEffect->SetMatrix( m_MatProjEffectHandle, &g_EffectConstant.matProj );
        }
        if( m_MatWorldViewEffectHandle != NULL )
        {
            m_pEffect->SetMatrix( m_MatWorldViewEffectHandle, &g_EffectConstant.matWorldView );
        }
        if( m_MatViewProjEffectHandle != NULL )
        {
            m_pEffect->SetMatrix( m_MatViewProjEffectHandle, &g_EffectConstant.matViewProj );
        }
        if( m_MatWorldViewProjEffectHandle != NULL )
        {
            m_pEffect->SetMatrix( m_MatWorldViewProjEffectHandle, &g_EffectConstant.matWorldViewProj );
        }
        if( m_MatWorldInverseEffectHandle != NULL )
        {
            m_pEffect->SetMatrix( m_MatWorldInverseEffectHandle, &g_EffectConstant.matWorldInv );
        }		
        if( m_MatViewInverseEffectHandle != NULL )
        {
            m_pEffect->SetMatrix( m_MatViewInverseEffectHandle, &g_EffectConstant.matViewInv );
        }		
        if( m_MatWorldViewInverseEffectHandle != NULL )
        {
            m_pEffect->SetMatrix( m_MatWorldViewInverseEffectHandle, &g_EffectConstant.matWorldViewInv );
        }

		// tranposed matrices (column major)
        if( m_MatWorldTEffectHandle != NULL )
		{
            m_pEffect->SetMatrix( m_MatWorldTEffectHandle, &g_EffectConstant.matWorldT );
        }
        if( m_MatViewTEffectHandle != NULL )
		{
            m_pEffect->SetMatrix( m_MatViewTEffectHandle, &g_EffectConstant.matViewT );
        }
        if( m_MatProjTEffectHandle != NULL )
		{
            m_pEffect->SetMatrix( m_MatProjTEffectHandle, &g_EffectConstant.matProjT );
        }
        if( m_MatWorldViewTEffectHandle != NULL )
        {
            m_pEffect->SetMatrix( m_MatWorldViewTEffectHandle, &g_EffectConstant.matWorldViewT );
        }
        if( m_MatViewProjTEffectHandle != NULL )
        {
            m_pEffect->SetMatrix( m_MatViewProjTEffectHandle, &g_EffectConstant.matViewProjT );
        }
        if( m_MatWorldViewProjTEffectHandle != NULL )
        {
            m_pEffect->SetMatrix( m_MatWorldViewProjTEffectHandle, &g_EffectConstant.matWorldViewProjT );
        }
        if( m_MatWorldInverseTEffectHandle != NULL )
        {
            m_pEffect->SetMatrix( m_MatWorldInverseTEffectHandle, &g_EffectConstant.matWorldInvT );
        }		
        if( m_MatViewInverseTEffectHandle != NULL )
        {
            m_pEffect->SetMatrix( m_MatViewInverseTEffectHandle, &g_EffectConstant.matViewInvT );
        }		
        if( m_MatWorldViewInverseTEffectHandle != NULL )
        {
            m_pEffect->SetMatrix( m_MatWorldViewInverseTEffectHandle, &g_EffectConstant.matWorldViewInvT );
        }

		// main vectors
		if ( m_LightDirHandle != NULL )
		{
            m_pEffect->SetVector( m_LightDirHandle, &g_EffectConstant.vecLightDir );
		}
		if ( m_LightDirInvHandle != NULL )
		{
            m_pEffect->SetVector( m_LightDirInvHandle, &g_EffectConstant.vecLightDirInv );
		}
		if ( m_LightPosHandle != NULL )
		{
            m_pEffect->SetVector( m_LightPosHandle, &g_EffectConstant.vecLightPos );
		}
        if( m_VecCameraPosEffectHandle != NULL )
        {
            m_pEffect->SetVector( m_VecCameraPosEffectHandle, &g_EffectConstant.vecCameraPosition );
        }
		if ( m_VecEyePosEffectHandle != NULL )
		{
            m_pEffect->SetVector( m_VecEyePosEffectHandle, &g_EffectConstant.vecEyePos );
		}

		// misclanious values
		if( m_TimeEffectHandle != NULL )
		{
			// TIME DATA IN SECONDS
			float fTime = (float)(timeGetTime() / 1000.0);
			m_pEffect->SetFloat( m_TimeEffectHandle, fTime );
		}
		if( m_SinTimeEffectHandle != NULL )
		{
			// TIME DATA IN SECONDS
			float fSinTime = (float)sin((timeGetTime() / 1000.0));
			m_pEffect->SetFloat( m_SinTimeEffectHandle, fSinTime );
		}
		if( m_DeltaTimeEffectHandle != NULL )
		{
			// DELTA TIME DATA IN SECONDS
			float fTimeNow = (float)(timeGetTime() / 1000.0);
			float fDeltaTime = fTimeNow - g_fLastDeltaTime;
			m_pEffect->SetFloat( m_DeltaTimeEffectHandle, fDeltaTime );
			g_fLastDeltaTime = fTimeNow;
		}
		if ( m_UVScalingHandle != NULL )
		{
			g_EffectConstant.vecUVScaling = D3DXVECTOR4 ( pMesh->fUVScalingU, pMesh->fUVScalingV, 0, 0 );
            m_pEffect->SetVector( m_UVScalingHandle, &g_EffectConstant.vecUVScaling );
		}

		// set bone matrix palette if required
		if ( m_BoneMatrixPaletteHandle )
		{
			// update all bone matrices
			DWORD dwBoneMax = pMesh->dwBoneCount;
			if ( dwBoneMax > 60 ) dwBoneMax = 60;

			// send bone count to shader (so can skip bone anim if nothing in palette)
			g_EffectConstant.fBoneCount = (float)dwBoneMax;
			m_pEffect->SetFloat( m_BoneCountHandle, g_EffectConstant.fBoneCount );

			// update matrix palette if any
			if ( pMesh->dwForceCPUAnimationMode==1 )
			{
				// CPU does animation
				for ( DWORD dwMatrixIndex = 0; dwMatrixIndex < dwBoneMax; dwMatrixIndex++ )
					D3DXMatrixIdentity ( &g_EffectConstant.matBoneMatrixPalette [ dwMatrixIndex ] );
			}
			else
			{
				// GPU needs matrices to do animation
				for ( DWORD dwMatrixIndex = 0; dwMatrixIndex < dwBoneMax; dwMatrixIndex++ )
					D3DXMatrixMultiply ( &g_EffectConstant.matBoneMatrixPalette [ dwMatrixIndex ], &pMesh->pBones [ dwMatrixIndex ].matTranslation, pMesh->pFrameMatrices [ dwMatrixIndex ] );
			}

			// send matrix array to effect (column-based is default by FX compiler)
            m_pEffect->SetMatrixTransposeArray ( m_BoneMatrixPaletteHandle, g_EffectConstant.matBoneMatrixPalette, dwBoneMax );
		}
    }
}

DARKSDK_DLL bool CreateMesh ( sObject** pObject, LPSTR pName )
{
	// create a new, empty mesh

	// the pointer must be valid
	SAFE_MEMORY ( pObject );

	// create a new object and check allocation
	*pObject = new sObject;
	SAFE_MEMORY ( pObject );

	// create a new frame and check allocation
	pObject [ 0 ]->pFrame = new sFrame;
	SAFE_MEMORY ( pObject [ 0 ]->pFrame );

	// finally create the mesh object
	pObject [ 0 ]->pFrame->pMesh = new sMesh;
	SAFE_MEMORY ( pObject [ 0 ]->pFrame->pMesh );

	// give it a name for reference
	if ( pName )
	{
		if ( strlen(pName) < MAX_STRING )
			strcpy(pObject [ 0 ]->pFrame->szName, pName);
	}

	// all went okay
	return true;
}

DARKSDK_DLL bool DeleteMesh ( sObject** pObject )
{
	// Before we delete object, remove from any temp lists
    if ( !m_ObjectManager.m_vVisibleObjectList.empty() )
    {
        for ( DWORD iIndex = 0; iIndex < m_ObjectManager.m_vVisibleObjectList.size(); ++iIndex )
        {
            sObject* pThisObject = m_ObjectManager.m_vVisibleObjectList [ iIndex ];
			if ( pThisObject==pThisObject )
			{
				//m_ObjectManager.m_vVisibleObjectList [ iIndex ] = NULL;
				m_ObjectManager.m_vVisibleObjectList.erase(m_ObjectManager.m_vVisibleObjectList.begin() + iIndex);
			}
		}
	}
    if ( !m_ObjectManager.m_vVisibleObjectEarly.empty() )
    {
        for ( DWORD iIndex = 0; iIndex < m_ObjectManager.m_vVisibleObjectEarly.size(); ++iIndex )
        {
            sObject* pThisObject = m_ObjectManager.m_vVisibleObjectEarly [ iIndex ];
			if ( pThisObject==pThisObject )
			{
				//m_ObjectManager.m_vVisibleObjectEarly [ iIndex ] = NULL;
				m_ObjectManager.m_vVisibleObjectEarly.erase(m_ObjectManager.m_vVisibleObjectEarly.begin() + iIndex);
			}
		}
	}
    if ( !m_ObjectManager.m_vVisibleObjectTransparent.empty() )
    {
        for ( DWORD iIndex = 0; iIndex < m_ObjectManager.m_vVisibleObjectTransparent.size(); ++iIndex )
        {
            sObject* pThisObject = m_ObjectManager.m_vVisibleObjectTransparent [ iIndex ];
			if ( pThisObject==pThisObject )
			{
				//m_ObjectManager.m_vVisibleObjectTransparent [ iIndex ] = NULL;
				m_ObjectManager.m_vVisibleObjectTransparent.erase(m_ObjectManager.m_vVisibleObjectTransparent.begin() + iIndex);
			}
		}
	}
    if ( !m_ObjectManager.m_vVisibleObjectNoZDepth.empty() )
    {
        for ( DWORD iIndex = 0; iIndex < m_ObjectManager.m_vVisibleObjectNoZDepth.size(); ++iIndex )
        {
            sObject* pThisObject = m_ObjectManager.m_vVisibleObjectNoZDepth [ iIndex ];
			if ( pThisObject==pThisObject )
			{
				//m_ObjectManager.m_vVisibleObjectNoZDepth [ iIndex ] = NULL;
				m_ObjectManager.m_vVisibleObjectNoZDepth.erase(m_ObjectManager.m_vVisibleObjectNoZDepth.begin() + iIndex);
			}
		}
	}
    if ( !m_ObjectManager.m_vVisibleObjectStandard.empty() )
    {
        for ( DWORD iIndex = 0; iIndex < m_ObjectManager.m_vVisibleObjectStandard.size(); ++iIndex )
        {
            sObject* pThisObject = m_ObjectManager.m_vVisibleObjectStandard [ iIndex ];
			if ( pThisObject==pThisObject )
			{
				//m_ObjectManager.m_vVisibleObjectStandard [ iIndex ] = NULL;
				m_ObjectManager.m_vVisibleObjectStandard.erase(m_ObjectManager.m_vVisibleObjectStandard.begin() + iIndex);
			}
		}
	}

	// Delete allocations
	SAFE_DELETE( *pObject );

	// all went okay
	return true;
}

DARKSDK_DLL void UpdateEulerRotation ( sObject* pObject )
{
	// euler rotation 
	D3DXMatrixRotationX ( &pObject->position.matRotateX, D3DXToRadian ( pObject->position.vecRotate.x ) );
	D3DXMatrixRotationY ( &pObject->position.matRotateY, D3DXToRadian ( pObject->position.vecRotate.y ) );
	D3DXMatrixRotationZ ( &pObject->position.matRotateZ, D3DXToRadian ( pObject->position.vecRotate.z ) );

	// choose rotation order for euler matrix
	switch ( pObject->position.dwRotationOrder )
	{
		case ROTORDER_XYZ :	
				pObject->position.matRotation =	pObject->position.matRotateX *
												pObject->position.matRotateY *
												pObject->position.matRotateZ;
				break;

		case ROTORDER_ZYX :	
				pObject->position.matRotation =	pObject->position.matRotateZ *
												pObject->position.matRotateY *
												pObject->position.matRotateX;
				break;

		case ROTORDER_ZXY :
				pObject->position.matRotation = pObject->position.matRotateZ *
												pObject->position.matRotateX *
												pObject->position.matRotateY;
				break;
	}
}

DARKSDK_DLL void UpdateObjectRotation ( sObject* pObject )
{
	if ( pObject->position.bFreeFlightRotation==false )
		UpdateEulerRotation ( pObject );
	else
		pObject->position.matRotation = pObject->position.matFreeFlightRotate;
}

DARKSDK_DLL bool CalcObjectWorld ( sObject* pObject )
{
	// leefix - 011103 - moved this function from manager as glue is a common process
	// if glued to object, get obj ptr
	sFrame* pGluedToFramePtr = NULL;
	if ( pObject->position.iGluedToObj )
	{
		if ( g_ObjectList [ pObject->position.iGluedToObj ] != NULL )
		{
			sObject* pOriginalGlueToObj = g_ObjectList [ pObject->position.iGluedToObj ];
			sObject* pChildObject = pOriginalGlueToObj;
			if ( pChildObject )
			{
				if ( pChildObject->ppFrameList == NULL ) 
				{
					pChildObject = pChildObject->pInstanceOfObject;
				}
				if ( pChildObject && pChildObject->ppFrameList != NULL )
				{
					// Must update any object glued to (for current absolute world data) -recurse!
					CalcObjectWorld ( pChildObject );

					// 051205 - if mode 1, wipe out frame orient, leaving position only
					int iFrame = pObject->position.iGluedToMesh;
					int iMode = 0; if ( iFrame < 0 ) { iFrame *= -1; iMode=1; }

					// Identify and extra frame ptr
					if ( iFrame < pChildObject->iFrameCount )
					{
						// get actual frame ptr
						pGluedToFramePtr = pChildObject->ppFrameList[ iFrame ];

						// Mode 1 is set by issuing a negative mesh id
						if ( iMode==1 )
						{
							D3DXVECTOR3 vecPos = D3DXVECTOR3 ( pGluedToFramePtr->matCombined._41, pGluedToFramePtr->matCombined._42, pGluedToFramePtr->matCombined._43 );
							D3DXMatrixIdentity ( &pGluedToFramePtr->matCombined );
							pGluedToFramePtr->matCombined._41 = vecPos.x;
							pGluedToFramePtr->matCombined._42 = vecPos.y;
							pGluedToFramePtr->matCombined._43 = vecPos.z;
						}

						// leefix - 100303 - Calculate correct absolute world matrix
						CalculateAbsoluteWorldMatrix ( pOriginalGlueToObj, pGluedToFramePtr, pGluedToFramePtr->pMesh );
					}
				}
			}
		}
	}

	// setup the world matrix for the object
	CalculateObjectWorld ( pObject, pGluedToFramePtr );

	// success
	return true;
}

DARKSDK_DLL bool CalculateObjectWorld ( sObject* pObject, sFrame* pGluedToFramePtr )
{
	// mike - 020903 - ignore world matrix as it's set elsewhere
	// MIKE - 021203 - comment this out for physics DLLs
	if ( pObject->position.bCustomWorldMatrix == true )
	{
		// leefix - 030306 - u60 - if rotate object calls this whilst it is a physics object
		// leefix - 10306 - physx - if this used by PHYSX, ANY change in matWorld messes up the physics representation! 
		// and calling position or rotate on a physics object under PHYSX should result in nothing happening!
		/// still need basic things like rotation, position in the world matrix
		//D3DXMatrixTranslation ( &pObject->position.matTranslation, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );
		//// handle rotation as either euler or freeflight
		//UpdateObjectRotation ( pObject );
		//// build up final rotation and world matrix
		//pObject->position.matWorld = pObject->position.matRotation * pObject->position.matTranslation;
		// return with success
		return true;
	}

	// create a scaling and position matrix
	D3DXMatrixScaling ( &pObject->position.matScale, pObject->position.vecScale.x, pObject->position.vecScale.y, pObject->position.vecScale.z );
	D3DXMatrixTranslation ( &pObject->position.matTranslation, pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );

	// GLobal setting to shrink each object (to defeat shadow map self shadowing effect)
	if ( g_fShrinkObjectsTo > 0.0f )
	{
		D3DXMATRIX matShrinkScale;
		D3DXMatrixScaling ( &matShrinkScale, g_fShrinkObjectsTo, g_fShrinkObjectsTo, g_fShrinkObjectsTo );
		D3DXMatrixMultiply ( &pObject->position.matScale, &pObject->position.matScale, &matShrinkScale );
	}
 
	LPVOID pWhenThisCHanges = (LPVOID)&pObject->position.vecPosition.x;

	// handle rotation as either euler or freeflight
	UpdateObjectRotation ( pObject );

	// Apply pivot if any
	if ( pObject->position.bApplyPivot )
	{
		// modify current rotation
		pObject->position.matRotation = pObject->position.matPivot * pObject->position.matRotation;
	}

	// build up final rotation and world matrix
	pObject->position.matObjectNoTran = pObject->position.matScale * pObject->position.matRotation;
	pObject->position.matWorld = pObject->position.matObjectNoTran * pObject->position.matTranslation;

	// Apply glue world-matrix if any
	if ( pObject->position.bGlued && pGluedToFramePtr )
	{
		// find target object:mesh
		sFrame* pTargetFrame = pGluedToFramePtr;
		if ( pTargetFrame )
		{
			// apply object world then limb world
			pObject->position.matWorld *= pTargetFrame->matAbsoluteWorld;

			// no trans taken from target, then clear translation data
			pObject->position.matObjectNoTran = pObject->position.matWorld;
			pObject->position.matObjectNoTran._41 = 0.0f;
			pObject->position.matObjectNoTran._42 = 0.0f;
			pObject->position.matObjectNoTran._43 = 0.0f;
		}
	}

	// all frames should be flagged for recalc of vectors (need data prior to sync!)
	if ( pObject->ppFrameList )
	{
		for ( int iCurrentFrame = 0; iCurrentFrame < pObject->iFrameCount; iCurrentFrame++ )
		{
			sFrame* pFrame = pObject->ppFrameList [ iCurrentFrame ];
			if ( pFrame ) pFrame->bVectorsCalculated = false;
		}
	}

	// success
	return true;
}

DARKSDK_DLL void CalculateAbsoluteWorldMatrix ( sObject* pObject, sFrame* pFrame, sMesh* pMesh )
{
	// bone or frame animation (bone anim includes frame matrix adjustment)
	bool bBoneAnimation = false;
	if ( pMesh )
	{
		// leefix - 110303 - even static bone models must apply the combined factor
		if ( pMesh->dwBoneCount )//&& (pObject->bAnimPlaying || pObject->bAnimManualSlerp) )
		{
			bBoneAnimation = true;
		}
	}

	// set the absolute matrix for the frame
	// 151003 - i have now added this to 'UpdateRealtimeFrameVectors' in dboframe.cpp
	if ( bBoneAnimation )
		pFrame->matAbsoluteWorld = pObject->position.matWorld;
	else
		pFrame->matAbsoluteWorld = pFrame->matCombined * pObject->position.matWorld;
}

DARKSDK_DLL bool CreateSingleMeshFromObjectCore ( sMesh** ppMesh, sObject* pObject, int iLimbNumberOptional, int iIgnoreMode )
{
	// get untranslated world matrix of object (includes rotation and scale)
	CalculateObjectWorld ( pObject, NULL );

	// create a new mesh
	*ppMesh = new sMesh;

	// count total number of vertices and indexes in object
	DWORD dwTotalVertices=0;
	DWORD dwTotalIndices=0;

	// leefix - 210703 - any FVF
	bool bGotFVFFromFirstMesh=false;
	bool bVertexOnlyBuffer=false;
	DWORD dwNewMeshFVF=0;

	// leeadd - 151008 - u70 - meshes should all be trilist from this process
	int iNewPrimitiveType=D3DPT_TRIANGLELIST;

	// leefix - 081208 - U71 - ensure if ANY mesh has no indices, we go for vertex only	
	for ( int iCurrentFrame = 0; iCurrentFrame < pObject->iFrameCount; iCurrentFrame++ )
	{
		sFrame* pFrame = pObject->ppFrameList [ iCurrentFrame ];
		if ( pFrame && (iLimbNumberOptional==-1 || iLimbNumberOptional==iCurrentFrame) )
		{
			sMesh* pFrameMesh = pFrame->pMesh;
			if ( pFrameMesh )
			{
				if ( pFrameMesh->dwIndexCount==0 )
					bVertexOnlyBuffer=true;

				// U75 - 010410 - moved here from belo as both pFrameMesh->iPrimitiveType and iNewPrimitiveType known here
				if ( pFrameMesh->iPrimitiveType != iNewPrimitiveType ) 
				{
					// leeadd - 151008 - u70 - use verts if not trilist
					bVertexOnlyBuffer=true;
				}
			}
		}
	}

	// run through all of the frames within the object
	for ( int iPass = 1; iPass <= 2; iPass++ )
	{
		for ( int iCurrentFrame = 0; iCurrentFrame < pObject->iFrameCount; iCurrentFrame++ )
		{
			// find frame within object
			sFrame* pFrame = pObject->ppFrameList [ iCurrentFrame ];
			if ( pFrame && (iLimbNumberOptional==-1 || iLimbNumberOptional==iCurrentFrame) )
			{
				// mesh within frame
				sMesh* pFrameMesh = pFrame->pMesh;
				if ( pFrameMesh )
				{
					// ignore modes
					if ( iIgnoreMode==1 )
					{
						// 1 - ignore all meshes that have a NO-CULL status (typically leaves, grass, etc)
						if ( pFrameMesh->bCull==false )
							continue;
					}
					if ( iIgnoreMode==2 )
					{
						// 2 - ignore all meshes that have been HIDDEN
						if ( pFrameMesh->bVisible==false )
							continue;
					}

					// get new mesh fvf
					if ( bGotFVFFromFirstMesh==false )
					{
						dwNewMeshFVF = pFrameMesh->dwFVF;
						if ( dwNewMeshFVF==0 )
						{
							dwNewMeshFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
						}
						bGotFVFFromFirstMesh = true;
					}

					// calculate world matrix of frame
					D3DXMATRIX matWorld = pFrame->matCombined * pObject->position.matObjectNoTran;

					// convert mesh to standard temp mesh-format
					sMesh* pStandardMesh = new sMesh;
					MakeLocalMeshFromOtherLocalMesh ( pStandardMesh, pFrameMesh );
					ConvertLocalMeshToFVF ( pStandardMesh, dwNewMeshFVF );

					if ( iIgnoreMode==11 )
					{
						if ( pFrameMesh->dwBoneCount>0 )
						{
							int lee=42;
						}

						// 11 - delete any BONE data in resulting mesh
						if ( pStandardMesh->dwBoneCount>0 )
						{
							int lee=42;
						}
					}

					// U75 - 010410 - this is redundant as the check is performed earlier in this function
					// leeadd - 081208 - U71 - if mesh has NO index data, all mesh must have NO index data, so switch to vertex only
					//if ( pFrameMesh->dwIndexCount==0 )
					//	bVertexOnlyBuffer=true;

					// just verts
					if ( bVertexOnlyBuffer==true ) ConvertLocalMeshToVertsOnly ( pStandardMesh ); 

					// pass one - count all verts/indexes
					if ( iPass==1 )
					{
						// total size of data in this mesh
						dwTotalVertices += pStandardMesh->dwVertexCount;
						dwTotalIndices += pStandardMesh->dwIndexCount;
					}

					// pass two - copy data to single mesh
					if ( iPass==2 )
					{
						// copy vertex data from mesh to single-mesh
						BYTE* pDestVertexData = (BYTE*)((*ppMesh)->pVertexData+(dwTotalVertices*pStandardMesh->dwFVFSize));
						BYTE* pDestNormalData = (BYTE*)((*ppMesh)->pVertexData+(dwTotalVertices*pStandardMesh->dwFVFSize)) + ( sizeof(float)*3 );
						if ( dwNewMeshFVF & D3DFVF_XYZ )
							memcpy ( pDestVertexData, pStandardMesh->pVertexData, pStandardMesh->dwVertexCount * pStandardMesh->dwFVFSize );

						// copy index data from mesh to single-mesh
						WORD* pDestIndexData = NULL;
						if ( (*ppMesh)->pIndices )
						{
							pDestIndexData = (WORD*)((*ppMesh)->pIndices + dwTotalIndices);
							memcpy ( pDestIndexData, pStandardMesh->pIndices, pStandardMesh->dwIndexCount * sizeof(WORD) );
						}

						// transform vertex data by world matrix of frame
						for ( DWORD v=0; v<pStandardMesh->dwVertexCount; v++ )
						{
							// get pointer to vertex and normal
							D3DXVECTOR3* pVertex = (D3DXVECTOR3*)(pDestVertexData+(v*pStandardMesh->dwFVFSize));
							D3DXVECTOR3* pNormal = (D3DXVECTOR3*)(pDestNormalData+(v*pStandardMesh->dwFVFSize));

							// transform with current combined frame matrix
							if ( dwNewMeshFVF & D3DFVF_XYZ )
							{
								D3DXVec3TransformCoord ( pVertex, pVertex, &matWorld );
							}
							if ( dwNewMeshFVF & D3DFVF_NORMAL )
							{
								D3DXVec3TransformNormal ( pNormal, pNormal, &matWorld );
								D3DXVec3Normalize ( pNormal, pNormal );
							}
						}

						// increment index data to reference correct vertex area
						if ( pDestIndexData )
						{
							for ( DWORD i=0; i<pStandardMesh->dwIndexCount; i++ )
							{
								pDestIndexData[i] += (WORD)dwTotalVertices;
							}
						}

						// advance counters
						dwTotalVertices += pStandardMesh->dwVertexCount;
						dwTotalIndices += pStandardMesh->dwIndexCount;
					}

					// delete standard temp mesh
					SAFE_DELETE ( pStandardMesh );
				}
			}
		}

		// end of passes
		if ( iPass==1 )
		{
			// leefix - 280305 - if index list too big, switch to using pre vertex buffer
			if ( dwTotalIndices > 0x0000FFFF )
			{
				bVertexOnlyBuffer = true;
				dwTotalVertices = dwTotalIndices * 3;
				dwTotalIndices = 0;
				iPass--; //redo count!
			}

			// make vertex and index buffers for single mesh
			SetupMeshFVFData ( *ppMesh, dwNewMeshFVF, dwTotalVertices, dwTotalIndices );

			// reset counters to fill single mesh
			dwTotalVertices = 0;
			dwTotalIndices = 0;
		}
		if ( iPass==2 )
		{
			// setup mesh drawing properties when single mesh data transfered
			(*ppMesh)->iPrimitiveType   = iNewPrimitiveType;
			(*ppMesh)->iDrawVertexCount = dwTotalVertices;
			if ( dwTotalIndices==0 )
				(*ppMesh)->iDrawPrimitives  = dwTotalVertices/3;
			else
				(*ppMesh)->iDrawPrimitives  = dwTotalIndices/3;
		}
	}

	if ( iLimbNumberOptional!=-1 )
	{
		// for limb specific creations, also copy texture info over
		sFrame* pFrame = pObject->ppFrameList [ iLimbNumberOptional ];
		if ( pFrame )
		{
			sMesh* pMesh = pFrame->pMesh;
			if ( pMesh )
			{
				(*ppMesh)->dwTextureCount = pMesh->dwTextureCount; 
				(*ppMesh)->pTextures = new sTexture [ (*ppMesh)->dwTextureCount ]; 
				CloneInternalTextures ( (*ppMesh), pMesh );
			}
		}
	}
	else
	{
		// for whole object conversions, use first texture of master object
		if ( pObject->ppMeshList )
		{
			sMesh* pMesh = pObject->ppMeshList [ 0 ];
			if ( pMesh )
			{
				(*ppMesh)->dwTextureCount = pMesh->dwTextureCount; 
				(*ppMesh)->pTextures = new sTexture [ (*ppMesh)->dwTextureCount ]; 
				CloneInternalTextures ( (*ppMesh), pMesh );
			}
		}
	}

	// all went okay
	return true;
}

DARKSDK_DLL bool CreateSingleMeshFromObject ( sMesh** ppMesh, sObject* pObject, int iIgnoreMode )
{
	return CreateSingleMeshFromObjectCore ( ppMesh, pObject, -1, iIgnoreMode );
}

DARKSDK_DLL bool CreateSingleMeshFromLimb ( sMesh** ppMesh, sObject* pObject, int iLimbNumber, int iIgnoreMode )
{
	return CreateSingleMeshFromObjectCore ( ppMesh, pObject, iLimbNumber, iIgnoreMode );
}

/* new version but messes with FPSC assumtpions of vertex/index layout

  bool CreateSingleMeshFromObject ( sMesh** ppMesh, sObject* pObject )
{
	// get untranslated world matrix of object (includes rotation and scale)
	CalculateObjectWorld ( pObject, NULL );

	// create a new mesh
	*ppMesh = new sMesh;

	// count total number of vertices and indexes in object
	DWORD dwTotalVertices=0;

	// leefix - 210703 - any FVF
	bool bGotFVFFromFirstMesh=false;
	DWORD dwNewMeshFVF=0;

	// run through all of the frames within the object
	for ( int iPass = 1; iPass <= 2; iPass++ )
	{
		for ( int iCurrentFrame = 0; iCurrentFrame < pObject->iFrameCount; iCurrentFrame++ )
		{
			// find frame within object
			sFrame* pFrame = pObject->ppFrameList [ iCurrentFrame ];
			if ( pFrame )
			{
				// mesh within frame
				sMesh* pFrameMesh = pFrame->pMesh;
				if ( pFrameMesh )
				{
					// get new mesh fvf
					if ( bGotFVFFromFirstMesh==false )
					{
						dwNewMeshFVF = pFrameMesh->dwFVF;
						if ( dwNewMeshFVF==0 )
						{
							// cannot create nonFVF mesh at the moment
							SAFE_DELETE(*ppMesh);
							return false;
						}
						bGotFVFFromFirstMesh = true;
					}

					// calculate world matrix of frame
					D3DXMATRIX matWorld = pFrame->matCombined * pObject->position.matObjectNoTran;

					// convert mesh to standard temp mesh-format
					sMesh* pStandardMesh = new sMesh;
					MakeLocalMeshFromOtherLocalMesh ( pStandardMesh, pFrameMesh );
					ConvertLocalMeshToFVF ( pStandardMesh, dwNewMeshFVF );
					ConvertLocalMeshToVertsOnly ( pStandardMesh );

					// pass one - count all verts/indexes
					if ( iPass==1 )
					{
						// total size of data in this mesh
						dwTotalVertices += pStandardMesh->dwVertexCount;
					}

					// pass two - copy data to single mesh
					if ( iPass==2 )
					{
						// copy vertex data from mesh to single-mesh
						BYTE* pDestVertexData = (BYTE*)((*ppMesh)->pVertexData+(dwTotalVertices*pStandardMesh->dwFVFSize));
						BYTE* pDestNormalData = (BYTE*)((*ppMesh)->pVertexData+(dwTotalVertices*pStandardMesh->dwFVFSize)) + ( sizeof(float)*3 );
						if ( dwNewMeshFVF & D3DFVF_XYZ )
							memcpy ( pDestVertexData, pStandardMesh->pVertexData, pStandardMesh->dwVertexCount * pStandardMesh->dwFVFSize );

						// transform vertex data by world matrix of frame
						for ( DWORD v=0; v<pStandardMesh->dwVertexCount; v++ )
						{
							// get pointer to vertex and normal
							D3DXVECTOR3* pVertex = (D3DXVECTOR3*)(pDestVertexData+(v*pStandardMesh->dwFVFSize));
							D3DXVECTOR3* pNormal = (D3DXVECTOR3*)(pDestNormalData+(v*pStandardMesh->dwFVFSize));

							// transform with current combined frame matrix
							if ( dwNewMeshFVF & D3DFVF_XYZ )
							{
								D3DXVec3TransformCoord ( pVertex, pVertex, &matWorld );
							}
							if ( dwNewMeshFVF & D3DFVF_NORMAL )
							{
								D3DXVec3TransformNormal ( pNormal, pNormal, &matWorld );
								D3DXVec3Normalize ( pNormal, pNormal );
							}
						}

						// advance counters
						dwTotalVertices += pStandardMesh->dwVertexCount;
					}

					// delete standard temp mesh
					SAFE_DELETE ( pStandardMesh );
				}
			}
		}

		// end of passes
		if ( iPass==1 )
		{
			// 010804 - verts only due to size
			SetupMeshFVFData ( *ppMesh, dwNewMeshFVF, dwTotalVertices, 0 );

			// reset counters to fill single mesh
			dwTotalVertices = 0;
		}
		if ( iPass==2 )
		{
			// setup mesh drawing properties when single mesh data transfered
			(*ppMesh)->iPrimitiveType   = D3DPT_TRIANGLELIST;
			(*ppMesh)->iDrawVertexCount = dwTotalVertices;
			(*ppMesh)->iDrawPrimitives  = dwTotalVertices/3;
		}
	}

	// all went okay
	return true;
}
*/

// DXMesh implementations

DARKSDK_DLL LPD3DXMESH LocalMeshToDXMesh ( sMesh* pMesh, CONST LPD3DVERTEXELEMENT9 pDeclarationOverride, DWORD dwFVFOverride )
{
	// result var
	LPD3DXMESH pNewMesh = NULL;

	// create a mesh
	DWORD dwFaces = pMesh->iDrawPrimitives;
	DWORD dwVertices = pMesh->dwVertexCount;
	DWORD dwFVF = pMesh->dwFVF;
	LPD3DXMESH pOriginalMesh = NULL;
	IDirect3DIndexBuffer9* pIB = NULL;
	IDirect3DVertexBuffer9* pVB = NULL;

	// leefix - 060306 - u60 - cannot
	// 240315 - allows this using 32 bit flag below
	//if ( dwVertices > 65535 )
	//{
	//	RunTimeError ( RUNTIMEERROR_B3DMEMORYERROR );
	//	return NULL;
	//}

	// if too big, create a 32bit index buffer XMesh
	if ( dwFaces*3 > 0x0000FFFF )
	{
		// create dx mesh
		if ( dwFVF==0 )
			D3DXCreateMesh ( dwFaces, dwVertices, D3DXMESH_MANAGED | D3DXMESH_32BIT, pMesh->pVertexDeclaration, m_pD3D, &pOriginalMesh );
		else
			D3DXCreateMeshFVF ( dwFaces, dwVertices, D3DXMESH_MANAGED | D3DXMESH_32BIT, dwFVF, m_pD3D, &pOriginalMesh );

		// if failed to create dx mesh
		if ( pOriginalMesh==NULL)
			return NULL;

		// must prepare mesh index data as basic triangle list indices, not strips
		ConvertLocalMeshToTriList ( pMesh );

		// if original input mesh has no index data, create some for process
		if ( pMesh->dwIndexCount==0 )
		{
			pMesh->dwIndexCount = dwFaces * 3;
			DWORD* pDWORDPtr = new DWORD [ pMesh->dwIndexCount ];
			pMesh->pIndices = (WORD*)pDWORDPtr;
			for ( DWORD dwI=0; dwI<pMesh->dwIndexCount; dwI++ )
				pDWORDPtr [ dwI ] = dwI;
		}
		else
		{
			// if it does have data, it's in WORDs so need to re-create them as DWORDs
			pMesh->dwIndexCount = dwFaces * 3;
			DWORD* pDWORDPtr = new DWORD [ pMesh->dwIndexCount ];
			memset(pDWORDPtr,0,pMesh->dwIndexCount*sizeof(DWORD));
			for ( DWORD dwI=0; dwI<pMesh->dwIndexCount; dwI++ )
				pDWORDPtr [ dwI ] = pMesh->pIndices[dwI];
			// 281114 - changed to SAFE_DELETE_ARRAY
			SAFE_DELETE_ARRAY(pMesh->pIndices);
			pMesh->pIndices = (WORD*)pDWORDPtr;
		}

		// fill mesh with input mesh data
		pOriginalMesh->GetIndexBuffer(&pIB);
		pOriginalMesh->GetVertexBuffer(&pVB);
		if(pIB) CopyDWORDMeshDataToDWORDIndexBuffer ( pMesh, pIB, 0 );
		if(pVB) CopyMeshDataToVertexBufferSameFVF ( pMesh, pVB, 0 );
	}
	else
	{
		// support for 32bit meshes
		if ( dwVertices > 65535 )
		{
			// create LARGE dx mesh
			if ( dwFVF==0 )
				D3DXCreateMesh ( dwFaces, dwVertices, D3DXMESH_MANAGED | D3DXMESH_32BIT, pMesh->pVertexDeclaration, m_pD3D, &pOriginalMesh );
			else
				D3DXCreateMeshFVF ( dwFaces, dwVertices, D3DXMESH_MANAGED | D3DXMESH_32BIT, dwFVF, m_pD3D, &pOriginalMesh );
		}
		else
		{
			// create dx mesh
			if ( dwFVF==0 )
				D3DXCreateMesh ( dwFaces, dwVertices, D3DXMESH_MANAGED, pMesh->pVertexDeclaration, m_pD3D, &pOriginalMesh );
			else
				D3DXCreateMeshFVF ( dwFaces, dwVertices, D3DXMESH_MANAGED, dwFVF, m_pD3D, &pOriginalMesh );
		}

		// if failed to create dx mesh
		if ( pOriginalMesh==NULL)
			return NULL;

		// must prepare mesh index data as basic triangle list indices, not strips
		ConvertLocalMeshToTriList ( pMesh );

		// if original input mesh has no index data, create some for process
		if ( pMesh->dwIndexCount==0 )
		{
			pMesh->dwIndexCount = dwFaces * 3;
			pMesh->pIndices = new WORD [ pMesh->dwIndexCount ];
			for ( WORD dwI=0; dwI<(WORD)pMesh->dwIndexCount; dwI++ )
				pMesh->pIndices [ dwI ] = dwI;
		}

		// fill mesh with input mesh data
		pOriginalMesh->GetIndexBuffer(&pIB);
		pOriginalMesh->GetVertexBuffer(&pVB);
		if(pIB) CopyMeshDataToIndexBuffer ( pMesh, pIB, 0 );
		if(pVB) CopyMeshDataToVertexBufferSameFVF ( pMesh, pVB, 0 );
	}

	// modify mesh if vertex format overwrite in place
	if ( dwFVFOverride > 0 )
	{
		// When converting mesh to FVF standard
		pOriginalMesh->CloneMeshFVF ( 0, dwFVFOverride, m_pD3D, &pNewMesh );
		SAFE_RELEASE ( pOriginalMesh );
	}
	else if ( pDeclarationOverride!=NULL )
	{
		// When converting mesh to shader Declaration
		pOriginalMesh->CloneMesh ( 0, pDeclarationOverride, m_pD3D, &pNewMesh );
		SAFE_RELEASE ( pOriginalMesh );
	}
	else
	{
		// keep mesh - it is okay
		pNewMesh = pOriginalMesh;
	}

	// release buffers
	SAFE_RELEASE(pIB);
	SAFE_RELEASE(pVB);

	// return DX mesh
	return pNewMesh;
}

DARKSDK_DLL void UpdateLocalMeshWithDXMesh ( sMesh* pMesh, LPD3DXMESH pDXMesh )
{
	// do not update if dxptr invalid
	if ( pDXMesh==NULL )
		return;

	// If index count exceeds 16bit..
	bool bConvertIndexedDataToVertexOnly=false;
	DWORD dwVertexCount = pDXMesh->GetNumVertices();
	DWORD dwIndexCount = pDXMesh->GetNumFaces ( ) * 3;
	if ( dwIndexCount > 0 )
	{
		if ( dwIndexCount > 0x0000FFFF )
		{
			// create a vertex only mesh
			bConvertIndexedDataToVertexOnly=true;
			dwVertexCount = dwIndexCount;
			dwIndexCount = 0;
		}
	}

	// mesh can hold regular FVF and custom declarations
	if ( pDXMesh->GetFVF()==0 )
	{
		// now create new mesh data from new Declaration
		D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
		pDXMesh->GetDeclaration( Declaration );
		if ( SetupMeshDeclarationData ( pMesh, Declaration, pDXMesh->GetNumBytesPerVertex(), dwVertexCount, dwIndexCount )==false )
		{
			// need to fully restore the object mid-conversion
			return;
		}
	}
	else
	{
		// now create new mesh data from new FVF
		SetupMeshFVFData ( pMesh, pDXMesh->GetFVF(), dwVertexCount, dwIndexCount );
	}

	// get vertex and index buffer
	LPDIRECT3DVERTEXBUFFER9 m_pMeshVertexBuffer;
	LPDIRECT3DINDEXBUFFER9  m_pMeshIndexBuffer;
	pDXMesh->GetVertexBuffer ( &m_pMeshVertexBuffer );
	pDXMesh->GetIndexBuffer  ( &m_pMeshIndexBuffer );

	// copy dx mesh to dbpro mesh
	if ( bConvertIndexedDataToVertexOnly==true )
	{	
		// destination data
		BYTE* pDestVertPtr = (BYTE*)pMesh->pVertexData;

		// source data
		WORD* pIndices = NULL;
		if ( SUCCEEDED ( m_pMeshIndexBuffer->Lock ( 0, 0, ( VOID** ) &pIndices, 0 ) ) )
		{
			BYTE* pSrcVertexData = NULL;
			if ( SUCCEEDED ( m_pMeshVertexBuffer->Lock ( 0, 0, ( VOID** ) &pSrcVertexData, 0 ) ) )
			{
				// go through all faces data of dx mesh
				DWORD dwIndex = 0;
				for ( DWORD f=0; f<pDXMesh->GetNumFaces ( ); f++ )
				{
					// three vertx per face
					for ( int n=0; n<3; n++ )
					{
						// source data - index data gives us src vertex position
						DWORD dwBufferOffset = pIndices[ dwIndex++ ];

						// copy across to the dest buffer and advance to next vertex
						memcpy ( pDestVertPtr, pSrcVertexData+(dwBufferOffset*pMesh->dwFVFSize), pMesh->dwFVFSize );
						pDestVertPtr+=pMesh->dwFVFSize;
					}
				}

				// unlock the vertex buffer
				m_pMeshVertexBuffer->Unlock ( );
			}

			// unlock the index buffer
			m_pMeshIndexBuffer->Unlock ( );
		}
	}
	else
	{
		// fill mesh data from vertex and index buffers
		if(m_pMeshIndexBuffer) CopyIndexBufferToMeshData ( pMesh, m_pMeshIndexBuffer, 0 );
		if(m_pMeshVertexBuffer) CopyVertexBufferToMeshDataSameFVF ( pMesh, m_pMeshVertexBuffer, 0 );
	}

	// release buffers
	SAFE_RELEASE(m_pMeshVertexBuffer);
	SAFE_RELEASE(m_pMeshIndexBuffer);

	// when mesh changes, must flag it
	pMesh->bMeshHasBeenReplaced = true;
}

DARKSDK_DLL void RestoreLocalMesh ( sMesh* pMesh )
{
	LPD3DXMESH pDXMesh = LocalMeshToDXMesh ( pMesh, NULL, pMesh->dwFVFOriginal );
	UpdateLocalMeshWithDXMesh ( pMesh, pDXMesh );
	// lee - 240306 - u6b5 - must not hold onto this if restored mesh
	SAFE_DELETE ( pMesh->pOriginalVertexData );
	SAFE_RELEASE(pDXMesh);
}

DARKSDK_DLL void ConvertLocalMeshToFVF ( sMesh* pMesh, DWORD dwFVF )
{
	// leefix - 061008 - old DBC functionality allows large vertex objects
	// and this code will insert an error runtime even if FVF are identical, so..
	if ( pMesh->dwFVF != dwFVF )
	{
		LPD3DXMESH pDXMesh = LocalMeshToDXMesh ( pMesh, NULL, dwFVF );
		UpdateLocalMeshWithDXMesh ( pMesh, pDXMesh );
		SAFE_RELEASE(pDXMesh);
	}
}

DARKSDK_DLL void ConvertLocalMeshToVertsOnly ( sMesh* pMesh )
{
	// mesh 'can' store 32bit index data temporarily for later conversion to vertex only.
	// and should only use 32bit if going to do a vertex expanding (due to >16bit verts)
	bool b32BITIndexData=false;
	if ( pMesh->dwVertexCount > 0xFFFF )
		b32BITIndexData=true;

	// ensure it is a trilist first
	if ( b32BITIndexData==false )
		ConvertLocalMeshToTriList ( pMesh );

	// convert from index to vertex only
	if ( pMesh->iPrimitiveType==D3DPT_TRIANGLELIST)
	{
		if ( pMesh->pIndices )
		{
			// new vertex data
			DWORD dwVertSize = pMesh->dwFVFSize;
			DWORD dwNewVertexCount = pMesh->dwIndexCount;
			BYTE* pNewVertexData = (BYTE*)new char [ dwNewVertexCount * dwVertSize ];

			// recreate conversion map
			SAFE_DELETE_ARRAY ( g_pConversionMap );
			if ( g_pConversionMap==NULL )
			{
				// size of old vertex buffer (where old face data referenced)
				g_pConversionMap = new DWORD[pMesh->dwVertexCount];
			}

			// go through all indices (leefix-161003-fixed wFaceIndex to dwFaceIndex as this function used to expand vertex data so no index data is required (16bit+)
			DWORD dwFaceIndex = 0;
			BYTE* pBase = pMesh->pVertexData;
			DWORD* pDWORDIndexPtr = (DWORD*)pMesh->pIndices;
			for ( DWORD i=0; i<pMesh->dwIndexCount; i+=3 )
			{
				// read face
				DWORD dwFace0, dwFace1, dwFace2;
				if ( b32BITIndexData )
				{
					// special temporary DWORD index data
					dwFace0 = pDWORDIndexPtr[i+0];
					dwFace1 = pDWORDIndexPtr[i+1];
					dwFace2 = pDWORDIndexPtr[i+2];
				}
				else
				{
					// normal WORD index data
					dwFace0 = pMesh->pIndices[i+0];
					dwFace1 = pMesh->pIndices[i+1];
					dwFace2 = pMesh->pIndices[i+2];
				}

				// get vert data
				float* pFromVert1 = (float*)((BYTE*)pBase+(dwFace0*dwVertSize));
				float* pFromVert2 = (float*)((BYTE*)pBase+(dwFace1*dwVertSize));
				float* pFromVert3 = (float*)((BYTE*)pBase+(dwFace2*dwVertSize));
				float* pToVert1 = (float*)((BYTE*)pNewVertexData+((dwFaceIndex+0)*dwVertSize));
				float* pToVert2 = (float*)((BYTE*)pNewVertexData+((dwFaceIndex+1)*dwVertSize));
				float* pToVert3 = (float*)((BYTE*)pNewVertexData+((dwFaceIndex+2)*dwVertSize));

				// record destination indexes in conversion map
				g_pConversionMap[dwFace0]=dwFaceIndex+0;
				g_pConversionMap[dwFace1]=dwFaceIndex+1;
				g_pConversionMap[dwFace2]=dwFaceIndex+2;
				dwFaceIndex+=3;

				// write to new vert data
				memcpy ( pToVert1, pFromVert1, dwVertSize );
				memcpy ( pToVert2, pFromVert2, dwVertSize );
				memcpy ( pToVert3, pFromVert3, dwVertSize );
			}

			// delete index data and old vertex data
			// 281114 - changed to SAFE_DELETE_ARRAY
			SAFE_DELETE_ARRAY(pMesh->pIndices);
			SAFE_DELETE_ARRAY(pMesh->pVertexData);

			// replace mesh ptrs
			pMesh->dwIndexCount = 0;
			pMesh->dwVertexCount = dwNewVertexCount;
			pMesh->pVertexData = pNewVertexData;
			pMesh->iDrawVertexCount = dwNewVertexCount;
			pMesh->iDrawPrimitives  = dwNewVertexCount/3;
		}
	}
}

DARKSDK_DLL bool ConvertLocalMeshToTriList ( sMesh* pMesh )
{
	// was action taken
	bool bActionTaken=false;

	// convert from tristrip
	if ( pMesh->iPrimitiveType==D3DPT_TRIANGLESTRIP )
	{
		WORD* pNewIndex = NULL;
		DWORD dwNewIndexCount = 0;
		if ( pMesh->pIndices==NULL )
		{
			// generate new mesh without indices
			WORD wIndexSeq = 0;
			dwNewIndexCount = (pMesh->dwVertexCount-2) * 3;
			pNewIndex = new WORD [ dwNewIndexCount ];
			WORD wFace0 = wIndexSeq; wIndexSeq++;
			WORD wFace1 = 0;
			WORD wFace2 = wIndexSeq; wIndexSeq++;
			int iToggle = 0;
			DWORD dwIndex = 0;
			for ( DWORD i = 2; i < pMesh->dwVertexCount; i++ )
			{
				// face assignments
				if ( iToggle==0 )
				{
					wFace0 = wFace0;
					wFace1 = wFace2;
					wFace2 = wIndexSeq; wIndexSeq++;
				}
				else
				{
					wFace0 = wFace2;
					wFace1 = wFace1;
					wFace2 = wIndexSeq; wIndexSeq++;
				}
				iToggle=1-iToggle;

				// get face vectors
				pNewIndex [ dwIndex++ ] = wFace0;
				pNewIndex [ dwIndex++ ] = wFace1;
				pNewIndex [ dwIndex++ ] = wFace2;
			}
		}
		else
		{
			// generate new mesh from indices
			dwNewIndexCount = (pMesh->dwIndexCount-2) * 3;
			pNewIndex = new WORD [ dwNewIndexCount ];
			WORD wFace0 = pMesh->pIndices [ 0 ];
			WORD wFace1 = 0;
			WORD wFace2 = pMesh->pIndices [ 1 ];
			int iToggle = 0;
			DWORD dwIndex = 0;
			for ( DWORD i = 2; i < pMesh->dwIndexCount; i++ )
			{
				// face assignments
				if ( iToggle==0 )
				{
					wFace0 = wFace0;
					wFace1 = wFace2;
					wFace2 = pMesh->pIndices [ i ];
				}
				else
				{
					wFace0 = wFace2;
					wFace1 = wFace1;
					wFace2 = pMesh->pIndices [ i ];
				}
				iToggle=1-iToggle;

				// get face vectors
				pNewIndex [ dwIndex++ ] = wFace0;
				pNewIndex [ dwIndex++ ] = wFace1;
				pNewIndex [ dwIndex++ ] = wFace2;
			}
		}

		// delete old index data
		// 281114 - changed to SAFE_DELETE_ARRAY
		SAFE_DELETE_ARRAY(pMesh->pIndices);

		// replace mesh ptrs
		pMesh->iPrimitiveType = 4;
		pMesh->dwIndexCount = dwNewIndexCount;
		pMesh->pIndices = pNewIndex;

		// complete mesh replace - slow
		pMesh->bMeshHasBeenReplaced = true;
		bActionTaken = true;
	}

	// convert from trifan
	if ( pMesh->iPrimitiveType==D3DPT_TRIANGLEFAN )
	{
		WORD* pNewIndex = NULL;
		DWORD dwNewIndexCount = 0;
		if ( pMesh->pIndices==NULL )
		{
			// generate new indices
			WORD wIndexSeq = 0;
			dwNewIndexCount = (pMesh->dwVertexCount-2) * 3;
			pNewIndex = new WORD [ dwNewIndexCount ];
			WORD wFace0 = wIndexSeq; wIndexSeq++;
			WORD wFace1 = 0;
			WORD wFace2 = wIndexSeq; wIndexSeq++;
			DWORD dwIndex = 0;
			for ( DWORD i = 2; i < pMesh->dwVertexCount; i++ )
			{
				// face assignments
				wFace0 = wFace0;
				wFace1 = wFace2;
				wFace2 = wIndexSeq; wIndexSeq++;

				// get face vectors
				pNewIndex [ dwIndex++ ] = wFace0;
				pNewIndex [ dwIndex++ ] = wFace1;
				pNewIndex [ dwIndex++ ] = wFace2;
			}
		}

		// delete old index data
		// 281114 - changed to SAFE_DELETE_ARRAY
		SAFE_DELETE_ARRAY(pMesh->pIndices);

		// replace mesh ptrs
		pMesh->iPrimitiveType = 4;
		pMesh->dwIndexCount = dwNewIndexCount;
		pMesh->pIndices = pNewIndex;

		// complete mesh replace - slow
		pMesh->bMeshHasBeenReplaced = true;
		bActionTaken = true;
	}

	// return action state
	return bActionTaken;
}

DARKSDK_DLL void ConvertToSharedVerts ( sMesh* pMesh, float fEpsilon )
{
	if ( pMesh->pIndices )
	{
		DWORD dwFaceIndex = 0;
		BYTE* pBase = pMesh->pVertexData;
		DWORD dwVertSize = pMesh->dwFVFSize;
		for ( DWORD i=0; i<pMesh->dwIndexCount; i+=3 )
		{
			// read face
			DWORD dwFace0, dwFace1, dwFace2;
			dwFace0 = pMesh->pIndices[i+0];
			dwFace1 = pMesh->pIndices[i+1];
			dwFace2 = pMesh->pIndices[i+2];

			// get vert data
			D3DXVECTOR3* pV0 = (D3DXVECTOR3*)((BYTE*)pBase+(dwFace0*dwVertSize));
			D3DXVECTOR3* pV1 = (D3DXVECTOR3*)((BYTE*)pBase+(dwFace1*dwVertSize));
			D3DXVECTOR3* pV2 = (D3DXVECTOR3*)((BYTE*)pBase+(dwFace2*dwVertSize));

			// find any previous instance of each vert position
			for ( DWORD facedone=0; facedone<i; facedone++ )
			{
				// read previous faces
				DWORD dwFaceD0 = pMesh->pIndices[facedone+0];
				DWORD dwFaceD1 = pMesh->pIndices[facedone+1];
				DWORD dwFaceD2 = pMesh->pIndices[facedone+2];
				D3DXVECTOR3* pVD0 = (D3DXVECTOR3*)((BYTE*)pBase+(dwFaceD0*dwVertSize));
				D3DXVECTOR3* pVD1 = (D3DXVECTOR3*)((BYTE*)pBase+(dwFaceD1*dwVertSize));
				D3DXVECTOR3* pVD2 = (D3DXVECTOR3*)((BYTE*)pBase+(dwFaceD2*dwVertSize));

				// check against current face, and re-use older face index if found
				D3DXVECTOR3 vec0 = *pV0 - *pVD0;
				D3DXVECTOR3 vec1 = *pV0 - *pVD1;
				D3DXVECTOR3 vec2 = *pV0 - *pVD2;
				if ( D3DXVec3Length ( &vec0 ) < fEpsilon ) pMesh->pIndices[i+0] = (WORD)dwFaceD0;
				if ( D3DXVec3Length ( &vec1 ) < fEpsilon ) pMesh->pIndices[i+0] = (WORD)dwFaceD1;
				if ( D3DXVec3Length ( &vec2 ) < fEpsilon ) pMesh->pIndices[i+0] = (WORD)dwFaceD2;
				vec0 = *pV1 - *pVD0;
				vec1 = *pV1 - *pVD1;
				vec2 = *pV1 - *pVD2;
				if ( D3DXVec3Length ( &vec0 ) < fEpsilon ) pMesh->pIndices[i+1] = (WORD)dwFaceD0;
				if ( D3DXVec3Length ( &vec1 ) < fEpsilon ) pMesh->pIndices[i+1] = (WORD)dwFaceD1;
				if ( D3DXVec3Length ( &vec2 ) < fEpsilon ) pMesh->pIndices[i+1] = (WORD)dwFaceD2;
				vec0 = *pV2 - *pVD0;
				vec1 = *pV2 - *pVD1;
				vec2 = *pV2 - *pVD2;
				if ( D3DXVec3Length ( &vec0 ) < fEpsilon ) pMesh->pIndices[i+2] = (WORD)dwFaceD0;
				if ( D3DXVec3Length ( &vec1 ) < fEpsilon ) pMesh->pIndices[i+2] = (WORD)dwFaceD1;
				if ( D3DXVec3Length ( &vec2 ) < fEpsilon ) pMesh->pIndices[i+2] = (WORD)dwFaceD2;
			}
		}
	}
}

DARKSDK_DLL bool MakeLocalMeshFromOtherLocalMesh ( sMesh* pMesh, sMesh* pOtherMesh, DWORD dwIndexCount, DWORD dwVertexCount )
{
	// get details from other mesh
	DWORD dwFVF				= pOtherMesh->dwFVF;
	DWORD dwFVFSize			= pOtherMesh->dwFVFSize;

	// mesh can hold regular FVF and custom declarations
	if ( dwFVF==0 )
	{
		// now create new mesh data from declaration
		if ( !SetupMeshDeclarationData ( pMesh, pOtherMesh->pVertexDeclaration, dwFVFSize, dwVertexCount, dwIndexCount ) )
			return false;
	}
	else
	{
		// create new mesh from FVF
		if ( !SetupMeshFVFData ( pMesh, dwFVF, dwVertexCount, dwIndexCount ) )
			return false;
	}

	// copy vertex data
	DWORD dwVertexDataSize = pOtherMesh->dwFVFSize * pOtherMesh->dwVertexCount;
	memcpy ( pMesh->pVertexData, pOtherMesh->pVertexData, dwVertexDataSize );

	// copy index data
	DWORD dwIndiceDataSize = sizeof(WORD) * pOtherMesh->dwIndexCount;
	if ( pMesh->pIndices ) memcpy ( pMesh->pIndices, pOtherMesh->pIndices, dwIndiceDataSize );

	// setup mesh drawing properties
	pMesh->iPrimitiveType   = pOtherMesh->iPrimitiveType;
	pMesh->iDrawVertexCount = pOtherMesh->iDrawVertexCount;
	pMesh->iDrawPrimitives  = pOtherMesh->iDrawPrimitives;

	// leeadd - 030306 - u60 - if mesh from rawmesh, no prim to draw from
	if ( pMesh->iDrawPrimitives==0 )
	{
		// calculate a value that makes most sense
		pMesh->iDrawPrimitives=pMesh->iDrawVertexCount/3;
	}

	// okay
	return true;
}

DARKSDK_DLL bool MakeLocalMeshFromOtherLocalMesh ( sMesh* pMesh, sMesh* pOtherMesh )
{
	DWORD dwIndexCount = pOtherMesh->dwIndexCount;
	DWORD dwVertexCount = pOtherMesh->dwVertexCount;
	return MakeLocalMeshFromOtherLocalMesh ( pMesh, pOtherMesh, dwIndexCount, dwVertexCount );
}

DARKSDK_DLL bool MakeLocalMeshFromPureMeshData ( sMesh* pMesh, DWORD dwFVF, DWORD dwFVFSize, float* pMeshData, DWORD dwVertexCount, DWORD dwPrimType )
{
	// create new mesh
	if ( !SetupMeshFVFData ( pMesh, dwFVF, dwVertexCount, 0 ) )
		return false;

	// copy vertex data
	DWORD dwVertexDataSize = dwFVFSize * dwVertexCount;
	memcpy ( pMesh->pVertexData, pMeshData, dwVertexDataSize );

	// setup mesh drawing properties
	pMesh->iPrimitiveType   = dwPrimType;
	pMesh->iDrawVertexCount = dwVertexCount;
	pMesh->iDrawPrimitives  = dwVertexCount/3;

	// okay
	return true;
}

DARKSDK_DLL LPD3DXMESH ComputeTangentBasisEx ( LPD3DXMESH gMasterMesh, bool bMakeNormals, bool bMakeTangents, bool bMakeBinormals, bool bFixTangents, bool bCylTexGen, bool bWeightNormalsByFace )
{
	// define raw input data type for this computation
	typedef struct {
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 texCoord;
	} MeshVertex;

	// input/output ptrs for conversion
	std::vector<float> position;
	std::vector<float> normal;
	std::vector<float> texCoord;
	std::vector<float> binormal;
	std::vector<float> tangent;

	// Get declaration of mesh to convert
	D3DVERTEXELEMENT9 End = D3DDECL_END();
	D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
	gMasterMesh->GetDeclaration( Declaration );

	// Create temporary mesh to hold required input data XYZ,NORMAL,TEX
	// leefix - 050906 - changed 'gMesh' to 'gMasterMesh' and 'pNewMesh' to 'pInputDataMesh'
	LPD3DXMESH pInputDataMesh = NULL;
	DWORD dwSysteMemFlag = D3DXMESH_SYSTEMMEM;
	if ( gMasterMesh->GetNumVertices() > 65535 ) dwSysteMemFlag = D3DXMESH_SYSTEMMEM | D3DXMESH_32BIT;
	gMasterMesh->CloneMeshFVF ( dwSysteMemFlag, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, m_pD3D, &pInputDataMesh );
	if ( pInputDataMesh==NULL ) return gMasterMesh;

	// Retrieve data from the temp mesh, put in input/output ptrs
	MeshVertex* vertexBuffer = NULL;
	DWORD numVertices = pInputDataMesh->GetNumVertices();
	pInputDataMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&vertexBuffer);
	if ( vertexBuffer==NULL ) return gMasterMesh;
	unsigned int i;
	for (i = 0; i < numVertices; ++i) {
		position.push_back(vertexBuffer[i].position.x);
		position.push_back(vertexBuffer[i].position.y);
		position.push_back(vertexBuffer[i].position.z);
		normal.push_back(vertexBuffer[i].normal.x);
		normal.push_back(vertexBuffer[i].normal.y);
		normal.push_back(vertexBuffer[i].normal.z);
		texCoord.push_back(vertexBuffer[i].texCoord.x);
		texCoord.push_back(vertexBuffer[i].texCoord.y);
		texCoord.push_back(0);
	}
	pInputDataMesh->UnlockVertexBuffer();

	// Retrieve triangle indices from the temp mesh, put in input/output ptr
	WORD (*indexBuffer)[3];
	std::vector<int> index;
	DWORD numTriangles = pInputDataMesh->GetNumFaces();
	pInputDataMesh->LockIndexBuffer(D3DLOCK_READONLY, (VOID**)&indexBuffer);
	if ( indexBuffer==NULL ) return gMasterMesh;
	for (i = 0; i < numTriangles; ++i) {
		index.push_back(indexBuffer[i][0]);
		index.push_back(indexBuffer[i][1]);
		index.push_back(indexBuffer[i][2]);
	}
	pInputDataMesh->UnlockIndexBuffer();

	// finished with input mesh, release this temp mesh now
	SAFE_RELEASE ( pInputDataMesh );

	// Specify conversion options from flags
	NVMeshMender::Option _FixTangents = NVMeshMender::FixTangents;
	NVMeshMender::Option _FixCylindricalTexGen = NVMeshMender::FixCylindricalTexGen;
	NVMeshMender::Option _WeightNormalsByFaceSize = NVMeshMender::WeightNormalsByFaceSize;
	if ( bFixTangents==false ) _FixTangents = NVMeshMender::DontFixTangents;
	if ( bCylTexGen==false ) _FixCylindricalTexGen = NVMeshMender::DontFixCylindricalTexGen;
	if ( bWeightNormalsByFace==false ) _WeightNormalsByFaceSize = NVMeshMender::DontWeightNormalsByFaceSize;

	// Setup INPUT Components
	NVMeshMender::VertexAttribute positionAtt;
	positionAtt.Name_ = "position";
	positionAtt.floatVector_ = position;
	NVMeshMender::VertexAttribute normalAtt;
	normalAtt.Name_ = "normal";
	normalAtt.floatVector_ = normal;
	NVMeshMender::VertexAttribute indexAtt;
	indexAtt.Name_ = "indices";
	indexAtt.intVector_ = index;
	NVMeshMender::VertexAttribute texCoordAtt;
	texCoordAtt.Name_ = "tex0";
	texCoordAtt.floatVector_ = texCoord;

	// Create INPUT Attribute
	std::vector<NVMeshMender::VertexAttribute> inputAtts;
	inputAtts.push_back(positionAtt);
	inputAtts.push_back(indexAtt);
	inputAtts.push_back(texCoordAtt);
	inputAtts.push_back(normalAtt);
	
	// Add In OUTPUT Components
	NVMeshMender::VertexAttribute tangentAtt;
	tangentAtt.Name_ = "tangent";
	NVMeshMender::VertexAttribute binormalAtt;
	binormalAtt.Name_ = "binormal";

	// Create OUTPUT Attribute
	unsigned int n = 0;
	std::vector<NVMeshMender::VertexAttribute> outputAtts;
	outputAtts.push_back(positionAtt); ++n;
	outputAtts.push_back(indexAtt); ++n;
	outputAtts.push_back(texCoordAtt); ++n;
	outputAtts.push_back(normalAtt); ++n;
	outputAtts.push_back(tangentAtt); ++n;
	outputAtts.push_back(binormalAtt); ++n;

	// Uses MeshMenderD3DX from NVIDIA
	NVMeshMender mender;
	if (!mender.MungeD3DX(
					inputAtts,								// input attributes
					outputAtts,								// outputs attributes
					3.141592654f / 3.0f,					// tangent space smooth angle
					0,										// no texture matrix applied to my texture coordinates
					_FixTangents,							// fix degenerate bases & texture mirroring
					_FixCylindricalTexGen,					// low poly and quad meshes
					_WeightNormalsByFaceSize				// weigh vertex normals by the triangle's size
					))
	{
		// Failed to convert mesh over
		return gMasterMesh;
	}

	// Get output ptrs after conversion
	--n; binormal = outputAtts[n].floatVector_; 
	--n; tangent = outputAtts[n].floatVector_; 
	--n; normal = outputAtts[n].floatVector_; 
	--n; texCoord = outputAtts[n].floatVector_;
	--n; index = outputAtts[n].intVector_;
	--n; position = outputAtts[n].floatVector_;

	///* SOMEONE is corrupting the heap (near here, is it the code below?)

	// ensure vertex data and index data size is unchanged
	LPD3DXMESH pOutputMesh = NULL;
	DWORD dwNewVertexCount = position.size()/3;
	DWORD dwNewFaceCount = index.size()/3;
	if ( dwNewVertexCount==numVertices && dwNewFaceCount==numTriangles )
	{
		// create mesh from new declaration
		gMasterMesh->CloneMesh( dwSysteMemFlag, Declaration, m_pD3D, &pOutputMesh );
	}
	else
	{
		// create new mesh resized for new output data
		D3DXCreateMesh ( dwNewFaceCount, dwNewVertexCount, dwSysteMemFlag, Declaration, m_pD3D, &pOutputMesh);
	}
	if ( pOutputMesh==NULL ) return gMasterMesh;

	// Lock the vertex buffer
	LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
	DWORD dwVSize = pOutputMesh->GetNumBytesPerVertex();
	DWORD dwVertexCount = pOutputMesh->GetNumVertices();
	DWORD dwFaceCount = pOutputMesh->GetNumFaces();
	HRESULT hr = pOutputMesh->GetVertexBuffer( &pVB );
	if( SUCCEEDED(hr) )
	{
		BYTE* pVertices = NULL;
		hr = pVB->Lock( 0, 0, (VOID**)&pVertices, 0 );
		if( SUCCEEDED(hr) )
		{
			// Find Offsets
			int iPosOffset = -1;
			int iDiffuseOffset = -1;
			int iTexOffset = -1;
			int iNormalOffset = -1;
			int iTangentOffset = -1;
			int iBinormalOffset = -1;
			for( int iElem=0; Declaration[iElem].Stream != End.Stream; iElem++ )
			{   
				if( Declaration[iElem].Usage == D3DDECLUSAGE_POSITION ) iPosOffset = Declaration[iElem].Offset;
				if( Declaration[iElem].Usage == D3DDECLUSAGE_NORMAL ) iNormalOffset = Declaration[iElem].Offset;
				if( Declaration[iElem].Usage == D3DDECLUSAGE_COLOR ) iDiffuseOffset = Declaration[iElem].Offset;
				if( Declaration[iElem].Usage == D3DDECLUSAGE_TEXCOORD && Declaration[iElem].UsageIndex==0 ) iTexOffset = Declaration[iElem].Offset; //leefix - 050906 - only take first stage TEX UV data
				if( Declaration[iElem].Usage == D3DDECLUSAGE_TANGENT ) iTangentOffset = Declaration[iElem].Offset;
				if( Declaration[iElem].Usage == D3DDECLUSAGE_BINORMAL ) iBinormalOffset = Declaration[iElem].Offset;
			}

			// Binormal makers
			BYTE* pPtr = pVertices;
			for ( DWORD v=0; v<dwVertexCount; ++v)
			{
				// obtain component ptrs
				D3DXVECTOR3* vecPos = (D3DXVECTOR3*)(pPtr+iPosOffset);
				D3DXVECTOR3* vecNormal = (D3DXVECTOR3*)(pPtr+iNormalOffset);
				D3DXCOLOR*   colDiffuse = (D3DXCOLOR*)(pPtr+iDiffuseOffset);
				D3DXVECTOR2* vecTex = (D3DXVECTOR2*)(pPtr+iTexOffset);
				D3DXVECTOR3* vecTangent = (D3DXVECTOR3*)(pPtr+iTangentOffset);
				D3DXVECTOR3* vecBinormal = (D3DXVECTOR3*)(pPtr+iBinormalOffset);

				// fill data of components in output mesh
				if ( iPosOffset!=-1 )
				{
					vecPos->x = position[3 * v + 0];
					vecPos->y = position[3 * v + 1];
					vecPos->z = position[3 * v + 2];
				}
				if ( iNormalOffset!=-1 )
				{
					vecNormal->x = normal[3 * v + 0];
					vecNormal->y = normal[3 * v + 1];
					vecNormal->z = normal[3 * v + 2];
				}
				if ( iTexOffset!=-1 )
				{
					vecTex->x = texCoord[3 * v + 0];
					vecTex->y = texCoord[3 * v + 1];
				}
				if ( iDiffuseOffset!=-1 )
				{
					*colDiffuse = D3DXCOLOR(255,255,255,255);
				}
				if ( iTangentOffset!=-1 )
				{
					vecTangent->x = tangent[3 * v + 0];
					vecTangent->y = tangent[3 * v + 1];
					vecTangent->z = tangent[3 * v + 2];
				}
				if ( iBinormalOffset!=-1 )
				{
					vecBinormal->x = binormal[3 * v + 0];
					vecBinormal->y = binormal[3 * v + 1];
					vecBinormal->z = binormal[3 * v + 2];
				}

				// next vertex
				pPtr+=dwVSize;
			}

			// unlock buffer
			pVB->Unlock();
		}

		// release buffer
		SAFE_RELEASE( pVB );
	}

	// Lock the index buffer
	LPDIRECT3DINDEXBUFFER9 pIB = NULL;
	hr = pOutputMesh->GetIndexBuffer( &pIB );
	if( SUCCEEDED(hr) )
	{
		hr = pIB->Lock( 0, 0, (VOID**)&indexBuffer, 0 );
		if( SUCCEEDED(hr) )
		{
			for (DWORD i = 0; i < dwFaceCount; ++i)
			{
				indexBuffer[i][0] = index[3 * i + 0];
				indexBuffer[i][1] = index[3 * i + 1];
				indexBuffer[i][2] = index[3 * i + 2];
			}

			// unlock buffer
			pIB->Unlock();
		}

		// release buffer
		SAFE_RELEASE( pIB );
	}
	
	// free old mesh
	SAFE_RELEASE ( gMasterMesh );

	// complete
	return pOutputMesh;
	//*/
	//return gMasterMesh;
}

DARKSDK_DLL LPD3DXMESH ComputeTangentBasis( LPD3DXMESH gMesh, bool bFixTangents, bool bCylTexGen, bool bWeightNormalsByFace )
{
	// leeadd - 050906 - added extra boolean controls for normal, tangent and binormal generation (darkshader control)
	return ComputeTangentBasisEx ( gMesh, true, true, true, bFixTangents, bCylTexGen, bWeightNormalsByFace );
}

DARKSDK_DLL void ComputeBoneDataInsideVertex ( sMesh* pMesh, LPD3DXMESH pVertexShaderMesh, DWORD dwWeightOffsetInBytes, DWORD dwIndicesOffsetInBytes, DWORD dwVertSizeInBytes )
{
	// do not bo if no bones (catched earlier when making bonedata in declaration)
	if ( pMesh->dwBoneCount==0 )
		return;

	// leeadd - 200204 - put animation bone data ( weights and indices ) into the vertex data (for VS skinning)
	DWORD dwNumVertices = pVertexShaderMesh->GetNumVertices();
	DWORD dwNumBones = pMesh->dwBoneCount;

	// create maximum of bones-per-vertex for collection purposes
	struct sBoneForVertInfo
	{
		float fWorkWeight;
		float fFinalWeight;
	};
	sBoneForVertInfo* pBFVI = new sBoneForVertInfo [ dwNumVertices * dwNumBones ];
	memset ( pBFVI, 0, sizeof( sBoneForVertInfo ) * dwNumVertices * dwNumBones );

	// collect bonesdata for all verts
	for ( int iBone = 0; iBone < ( int ) dwNumBones; iBone++ )
	{
		// go through all influenced bones
		for ( int iVert = 0; iVert < ( int ) pMesh->pBones [ iBone ].dwNumInfluences; iVert++ )
		{
			// get the vertex and weight
			int iIndexToVertex = pMesh->pBones [ iBone ].pVertices [ iVert ];
			float fWeight = pMesh->pBones [ iBone ].pWeights [ iVert ];

			// add this influence of weight to the pool under the correct bone index
			pBFVI [ (iIndexToVertex*dwNumBones)+iBone ].fWorkWeight = fWeight;
		}
	}

	// prepare an array to hold the best four
	int* piUseBone = new int [ dwNumVertices * 4 ];
	memset ( piUseBone, 0, sizeof( int ) * dwNumVertices * 4 );

	// work out the best four per vertex
	for ( DWORD iIndexToVertex = 0; iIndexToVertex < dwNumVertices; iIndexToVertex++ )
	{
		// fill with minus ones to indicate no bone at all
		for ( int iFillFlag = 0; iFillFlag<4; iFillFlag++ )
			piUseBone [ (iIndexToVertex*4)+iFillFlag ] = -1;

		// mark out best four
		for ( int iBestFour = 0; iBestFour<4; iBestFour++ )
		{
			// find best
			int iBestBone = -1;
			float fBest = 0.0f;
			for ( int iBone = 0; iBone < ( int ) dwNumBones; iBone++ )
			{
				// get weight and compare for best one
				float fWeight = pBFVI [ (iIndexToVertex*dwNumBones)+iBone ].fWorkWeight;
				if ( fWeight>fBest ) { fBest=fWeight; iBestBone=iBone; }
			}
			if ( iBestBone!=-1)
			{
				// confirm bone as best (in order)
				pBFVI [ (iIndexToVertex*dwNumBones)+iBestBone ].fFinalWeight=fBest;

				// record its position so we can refer to the correct bone
				piUseBone [ (iIndexToVertex*4)+iBestFour ] = iBestBone;

				// clear so not included in next iteration
				pBFVI [ (iIndexToVertex*dwNumBones)+iBestBone ].fWorkWeight=0.0f;
			}
			else
			{
				// no more weights, can break out
				break;
			}
		}
	}

	// lock vertex data and populate with best four
	BYTE* pVertexDataPtr = NULL;
	pVertexShaderMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&pVertexDataPtr);
	if ( pVertexDataPtr )
	{
		// get actual DWORD offsets
		DWORD dwWeightOffset = dwWeightOffsetInBytes/4;
		DWORD dwIndicesOffset = dwIndicesOffsetInBytes/4;

		// go through vertex data
		for ( DWORD v = 0; v < dwNumVertices; v++ )
		{
			// base ptr for vertex
			DWORD* pVertexPtr = (DWORD*)(pVertexDataPtr+(v*dwVertSizeInBytes));

			// clear weight and indices memarea
			memset ( pVertexPtr+dwWeightOffset, 0, sizeof ( float ) * 8 );

			// indexes to the bones we will use
			int iBoneA = piUseBone [ (v*4)+0 ];
			int iBoneB = piUseBone [ (v*4)+1 ];
			int iBoneC = piUseBone [ (v*4)+2 ];
			int iBoneD = piUseBone [ (v*4)+3 ];

			// indices for this vertex (references to bone index)
			if ( iBoneA>=0 ) *(float*)(pVertexPtr+dwIndicesOffset+0) = (float)iBoneA;
			if ( iBoneB>=0 ) *(float*)(pVertexPtr+dwIndicesOffset+1) = (float)iBoneB;
			if ( iBoneC>=0 ) *(float*)(pVertexPtr+dwIndicesOffset+2) = (float)iBoneC;
			if ( iBoneD>=0 ) *(float*)(pVertexPtr+dwIndicesOffset+3) = (float)iBoneD;

			// weights for this vertex (the weight to use against the bone)
			if ( iBoneA>=0 ) *(float*)(pVertexPtr+dwWeightOffset+0) = pBFVI [ (v*dwNumBones)+iBoneA ].fFinalWeight;
			if ( iBoneB>=0 ) *(float*)(pVertexPtr+dwWeightOffset+1) = pBFVI [ (v*dwNumBones)+iBoneB ].fFinalWeight;
			if ( iBoneC>=0 ) *(float*)(pVertexPtr+dwWeightOffset+2) = pBFVI [ (v*dwNumBones)+iBoneC ].fFinalWeight;
			if ( iBoneD>=0 ) *(float*)(pVertexPtr+dwWeightOffset+3) = pBFVI [ (v*dwNumBones)+iBoneD ].fFinalWeight;
		}

		// unlock vertex data
		pVertexShaderMesh->UnlockVertexBuffer();
	}

	// free usages
	SAFE_DELETE ( pBFVI );
	SAFE_DELETE ( piUseBone );

	// complete
	return;
}

DARKSDK_DLL void GenerateNewNormalsForMesh	( sMesh* pMesh )
{
	LPD3DXMESH pDXMesh = LocalMeshToDXMesh ( pMesh, NULL, 0 );
	if ( pDXMesh )
	{
		D3DXComputeNormals ( pDXMesh, NULL );
		UpdateLocalMeshWithDXMesh ( pMesh, pDXMesh );
		SAFE_RELEASE(pDXMesh);
	}
}

DARKSDK_DLL void GenerateNormalsForMesh ( sMesh* pMesh )
{
	// work vars
	D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
	D3DVERTEXELEMENT9 End = D3DDECL_END();
	int iElem;

	// 16 or 32 bit mesh size
	DWORD dwSysteMemFlag = D3DXMESH_SYSTEMMEM;
	if ( pMesh->dwVertexCount > 65535 ) dwSysteMemFlag = D3DXMESH_SYSTEMMEM | D3DXMESH_32BIT;

	// get DX mesh from sMesh
	LPD3DXMESH pDXMesh = LocalMeshToDXMesh ( pMesh, NULL, 0 );
	if ( pDXMesh==NULL )
		return;

	// extract declaration from mesh
	pDXMesh->GetDeclaration( Declaration );

	// check if mesh already has a normal component
	BOOL bHasNormals = FALSE;
	for( iElem=0; Declaration[iElem].Stream != End.Stream; iElem++ )
	{   
		if( Declaration[iElem].Usage == D3DDECLUSAGE_NORMAL )
		{
			bHasNormals = TRUE;
			break;
		}
	}

	// Update Mesh Semantics if does not have normals
	if( !bHasNormals ) 
	{
		Declaration[iElem].Stream = 0;
		Declaration[iElem].Offset = (WORD)pDXMesh->GetNumBytesPerVertex();
		Declaration[iElem].Type = D3DDECLTYPE_FLOAT3;
		Declaration[iElem].Method = D3DDECLMETHOD_DEFAULT;
		Declaration[iElem].Usage = D3DDECLUSAGE_NORMAL;
		Declaration[iElem].UsageIndex = 0;
		Declaration[iElem+1] = End;
		LPD3DXMESH pTempMesh;
		HRESULT hr = pDXMesh->CloneMesh( dwSysteMemFlag, Declaration, m_pD3D, &pTempMesh );
		if( SUCCEEDED( hr ) )
		{
			SAFE_RELEASE( pDXMesh );
			pDXMesh = pTempMesh;
			D3DXComputeNormals ( pDXMesh, NULL );
		}
	}

	// update sMesh with new DX mesh
	UpdateLocalMeshWithDXMesh ( pMesh, pDXMesh );

	// free usages
	SAFE_RELEASE(pDXMesh);
}

DARKSDK_DLL void GenerateExtraDataForMeshEx ( sMesh* pMesh, BOOL bNormals, BOOL bTangents, BOOL bBinormals, BOOL bDiffuse, BOOL bBones, DWORD dwGenerateMode )
{
	// work vars
	D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
	D3DVERTEXELEMENT9 End = D3DDECL_END();
	int iElem;

	// deactivate bone flag if no bones in source mesh
	if ( pMesh->dwBoneCount==0 ) bBones=FALSE;

	// get DX mesh from sMesh
	LPD3DXMESH pDXMesh = LocalMeshToDXMesh ( pMesh, NULL, 0 );
	if ( pDXMesh )
	{
		// extract declaration and vertex size from mesh
		pDXMesh->GetDeclaration( Declaration );
		WORD wNumBytesPerVertex = (WORD)pDXMesh->GetNumBytesPerVertex();

		// check if mesh already has a component
		BOOL bHasNormals = FALSE;
		BOOL bHasDiffuse = FALSE;
		BOOL bHasTangents = FALSE;
		BOOL bHasBinormals = FALSE;
		BOOL bHasBlendWeights = FALSE;
		BOOL bHasBlendIndices = FALSE;
		BOOL bHasSecondaryUVs = FALSE;
		for( iElem=0; Declaration[iElem].Stream != End.Stream; iElem++ )
		{   
			if( Declaration[iElem].Usage == D3DDECLUSAGE_NORMAL ) bHasNormals = TRUE;
			if( Declaration[iElem].Usage == D3DDECLUSAGE_COLOR ) bHasDiffuse = TRUE;
			if( Declaration[iElem].Usage == D3DDECLUSAGE_TANGENT ) bHasTangents = TRUE;
			if( Declaration[iElem].Usage == D3DDECLUSAGE_BINORMAL ) bHasBinormals = TRUE;
			if( Declaration[iElem].Usage == D3DDECLUSAGE_TEXCOORD && Declaration[iElem].UsageIndex>0 ) bHasSecondaryUVs = TRUE;
		}

		// leefix - 071208 - U71 - objects that have TWO UV channels cause declaration problems when generating data
		// so we strip out any secondary UV channels from declaration (if generate bit two is one)
		if ( bHasSecondaryUVs==TRUE && dwGenerateMode==2 )
		{
			for(; iElem>=0; iElem-- )
			{
				if( Declaration[iElem].Usage == D3DDECLUSAGE_TEXCOORD && Declaration[iElem].UsageIndex>0 )
				{
					Declaration[iElem] = Declaration[iElem+1];
					wNumBytesPerVertex -= 8; // assumes 8 bytes = texture coord
				}
			}
			for( iElem=0; Declaration[iElem].Stream != End.Stream; iElem++ ) {}
		}

		// Update Mesh Semantics if does not have components
		bool bGiveMeNormals = false;
		bool bGiveMeDiffuse = false;
		bool bGiveMeTangents = false;
		bool bGiveMeBinormals = false;
		bool bGiveMeBlendWeights = false;
		bool bGiveMeBlendIndices = false;
		if( !bHasNormals && bNormals ) 
		{
			bGiveMeNormals = true;
			Declaration[iElem].Stream = 0;
			Declaration[iElem].Offset = wNumBytesPerVertex;
			Declaration[iElem].Type = D3DDECLTYPE_FLOAT3;
			Declaration[iElem].Method = D3DDECLMETHOD_DEFAULT;
			Declaration[iElem].Usage = D3DDECLUSAGE_NORMAL;
			Declaration[iElem].UsageIndex = 0;
			Declaration[iElem+1] = End;
			wNumBytesPerVertex+=12;
			iElem++;
		}
		if( !bHasDiffuse && bDiffuse ) 
		{
			bGiveMeDiffuse = true;
			Declaration[iElem].Stream = 0;
			Declaration[iElem].Offset = wNumBytesPerVertex;
			Declaration[iElem].Type = D3DDECLTYPE_FLOAT1;
			Declaration[iElem].Method = D3DDECLMETHOD_DEFAULT;
			Declaration[iElem].Usage = D3DDECLUSAGE_COLOR;
			Declaration[iElem].UsageIndex = 0;
			Declaration[iElem+1] = End;
			wNumBytesPerVertex+=4;
			iElem++;
		}
		if( !bHasTangents && bTangents ) 
		{
			bGiveMeTangents = true;
			Declaration[iElem].Stream = 0;
			Declaration[iElem].Offset = wNumBytesPerVertex;
			Declaration[iElem].Type = D3DDECLTYPE_FLOAT3;
			Declaration[iElem].Method = D3DDECLMETHOD_DEFAULT;
			Declaration[iElem].Usage = D3DDECLUSAGE_TANGENT;
			Declaration[iElem].UsageIndex = 0;
			Declaration[iElem+1] = End;
			wNumBytesPerVertex+=12;
			iElem++;
		}
		//if( !bHasBinormals && bTangents ) // leefix - 050906 - now more specific for darkshader corrections
		if( !bHasBinormals && bBinormals ) 
		{
			bGiveMeBinormals = true;
			Declaration[iElem].Stream = 0;
			Declaration[iElem].Offset = wNumBytesPerVertex;
			Declaration[iElem].Type = D3DDECLTYPE_FLOAT3;
			Declaration[iElem].Method = D3DDECLMETHOD_DEFAULT;
			Declaration[iElem].Usage = D3DDECLUSAGE_BINORMAL;
			Declaration[iElem].UsageIndex = 0;
			Declaration[iElem+1] = End;
			wNumBytesPerVertex+=12;
			iElem++;
		}
		DWORD dwOffsetToWeights = wNumBytesPerVertex;
		if( !bHasBlendWeights && bBones ) 
		{
			bGiveMeBlendWeights = true;
			Declaration[iElem].Stream = 0;
			Declaration[iElem].Offset = wNumBytesPerVertex;
			Declaration[iElem].Type = D3DDECLTYPE_FLOAT4;
			Declaration[iElem].Method = D3DDECLMETHOD_DEFAULT;
			Declaration[iElem].Usage = D3DDECLUSAGE_TEXCOORD;
			Declaration[iElem].UsageIndex = 1;
			Declaration[iElem+1] = End;
			wNumBytesPerVertex+=16;
			iElem++;
		}
		DWORD dwOffsetToIndices = wNumBytesPerVertex;
		if( !bHasBlendIndices && bBones ) 
		{
			bGiveMeBlendIndices = true;
			Declaration[iElem].Stream = 0;
			Declaration[iElem].Offset = wNumBytesPerVertex;
			Declaration[iElem].Type = D3DDECLTYPE_FLOAT4;
			Declaration[iElem].Method = D3DDECLMETHOD_DEFAULT;
			Declaration[iElem].Usage = D3DDECLUSAGE_TEXCOORD;
			Declaration[iElem].UsageIndex = 2;
			Declaration[iElem+1] = End;
			wNumBytesPerVertex+=16;
			iElem++;
		}

		// create mesh from new declaration
		LPD3DXMESH pTempMesh = NULL;
		HRESULT hr;
		if ( pMesh->dwVertexCount>65535 )
			hr = pDXMesh->CloneMesh( D3DXMESH_SYSTEMMEM | D3DXMESH_32BIT, Declaration, m_pD3D, &pTempMesh );
		else
			hr = pDXMesh->CloneMesh( D3DXMESH_SYSTEMMEM, Declaration, m_pD3D, &pTempMesh );
		if( SUCCEEDED( hr ) )
		{
			// free old mesh and switch to new cloned mesh
			SAFE_RELEASE( pDXMesh );
			pDXMesh = pTempMesh;

			// Compute any tangent basis data (no not attempt to correct mesh too much)
			if ( bGiveMeNormals || bGiveMeTangents || bGiveMeBinormals )
			{
				// leefix - 050906 - this 'used' to EAT UV1 & other data, giving back a basic POS+NORM+TEX+TANGENT mesh
				pDXMesh = ComputeTangentBasisEx ( pTempMesh, bGiveMeNormals, bGiveMeTangents, bGiveMeBinormals, false, false, true );
			}

			// Fill blend data (weight and indices) if required 
			if ( bGiveMeBlendWeights || bGiveMeBlendIndices )
			{
				// fills mesh with additional data
				ComputeBoneDataInsideVertex ( pMesh, pDXMesh, dwOffsetToWeights, dwOffsetToIndices, wNumBytesPerVertex );

				// flag mesh as now being vertex skinned by a shader
				pMesh->bShaderBoneSkinning = true;
			}
		}

		// update sMesh with new DX mesh
		UpdateLocalMeshWithDXMesh ( pMesh, pDXMesh );

		// free usages
		SAFE_RELEASE(pDXMesh);
	}
}

DARKSDK_DLL void GenerateExtraDataForMeshEx ( sMesh* pMesh, BOOL bNormals, BOOL bTangents, BOOL bBinormals, BOOL bDiffuse, BOOL bBones )
{
	GenerateExtraDataForMeshEx ( pMesh, bNormals, bTangents, bBinormals, bDiffuse, bBones, 0 );
}

DARKSDK_DLL void GenerateExtraDataForMesh ( sMesh* pMesh, BOOL bNormals, BOOL bTangents, BOOL bDiffuse, BOOL bBones )
{
	// leeadd - 050906 - added binormal seperation for U63 (for new darkshader)
	GenerateExtraDataForMeshEx ( pMesh, bNormals, bTangents, FALSE, bDiffuse, bBones );
}

DARKSDK_DLL void CopyReferencesToShaderEffects ( sMesh* pNewMesh, sMesh* pMesh )
{
	pNewMesh->bUseVertexShader = pMesh->bUseVertexShader;
	pNewMesh->pVertexShader = pMesh->pVertexShader;
	pNewMesh->pVertexDec = pMesh->pVertexDec;
	pNewMesh->bOverridePixelShader = pMesh->bOverridePixelShader;
	pNewMesh->pPixelShader = pMesh->pPixelShader;
	pNewMesh->pVertexShaderEffect = pMesh->pVertexShaderEffect;
	pNewMesh->bVertexShaderEffectRefOnly = true;
	pNewMesh->bShaderBoneSkinning = pMesh->bShaderBoneSkinning;
	pNewMesh->dwForceCPUAnimationMode = pMesh->dwForceCPUAnimationMode; // 160415
	strcpy ( pNewMesh->pEffectName, pMesh->pEffectName );
}

DARKSDK_DLL void CloneShaderEffects ( sMesh* pNewMesh, sMesh* pMesh )
{
	// Copy references as normal
	CopyReferencesToShaderEffects ( pNewMesh, pMesh );

	// Also make source a reference so effect cannot be deleted 9as now ised twice..)
	pMesh->bVertexShaderEffectRefOnly = true;
}

// buffer functions

DARKSDK_DLL bool CopyMeshDataToIndexBuffer ( sMesh* pMesh, IDirect3DIndexBuffer9* pIndexBufferRef, DWORD dwBufferOffset )
{
	// copy index data from mesh to buffer
	WORD* pIndices = NULL;
	pIndexBufferRef->Lock ( 0, 0, ( VOID** ) &pIndices, 0 );

	// copy across to the buffer
	memcpy ( &pIndices[ dwBufferOffset ], pMesh->pIndices, sizeof ( WORD ) * pMesh->dwIndexCount );

	// unlock the index buffer
	pIndexBufferRef->Unlock ( );

	// all went okay
	return true;
}

DARKSDK_DLL bool CopyMeshDataToDWORDIndexBuffer ( sMesh* pMesh, IDirect3DIndexBuffer9* pIndexBufferRef, DWORD dwBufferOffset )
{
	// copy index data from mesh to buffer
	DWORD* pIndices = NULL;
	pIndexBufferRef->Lock ( 0, 0, ( VOID** ) &pIndices, 0 );

	// copy across WORD source data to the DWORD buffer
	DWORD dwIter = dwBufferOffset;
	for ( DWORD i=0; i<pMesh->dwIndexCount; i++ )
		pIndices[ dwIter++ ] = pMesh->pIndices [ i ];

	// unlock the index buffer
	pIndexBufferRef->Unlock ( );

	// all went okay
	return true;
}

DARKSDK_DLL bool CopyDWORDMeshDataToDWORDIndexBuffer ( sMesh* pMesh, IDirect3DIndexBuffer9* pIndexBufferRef, DWORD dwBufferOffset )
{
	// copy index data from mesh to buffer
	DWORD* pIndices = NULL;
	pIndexBufferRef->Lock ( 0, 0, ( VOID** ) &pIndices, 0 );

	// copy across DWORD source data to the DWORD buffer
	DWORD dwIter = dwBufferOffset;
	DWORD* pDWORDPtr = (DWORD*)pMesh->pIndices;
	for ( DWORD i=0; i<pMesh->dwIndexCount; i++ )
		pIndices[ dwIter++ ] = pDWORDPtr [ i ];

	// unlock the index buffer
	pIndexBufferRef->Unlock ( );

	// all went okay
	return true;
}

DARKSDK_DLL bool CopyMeshDataToVertexBufferSameFVF ( sMesh* pMesh, IDirect3DVertexBuffer9* pVertexBufferRef, DWORD dwBufferOffset )
{
	// lock the vertex buffer
	float* pfData = NULL;
	if ( FAILED ( pVertexBufferRef->Lock ( 0, 0, ( VOID** ) &pfData, 0 ) ) )
		return false;

	// copy across to the buffer
	memcpy ( &pfData[ dwBufferOffset ], pMesh->pVertexData, pMesh->dwFVFSize * pMesh->dwVertexCount );

	// unlock the vertex buffer
	pVertexBufferRef->Unlock ( );

	// all went okay
	return true;
}

DARKSDK_DLL bool CopyIndexBufferToMeshData ( sMesh* pMesh, IDirect3DIndexBuffer9* pIndexBufferRef, DWORD dwBufferOffset )
{
	// copy index data from mesh to buffer
	WORD* pIndices = NULL;
	pIndexBufferRef->Lock ( 0, 0, ( VOID** ) &pIndices, 0 );

	// copy across to the buffer
	memcpy ( pMesh->pIndices, &pIndices[ dwBufferOffset ], sizeof ( WORD ) * pMesh->dwIndexCount );

	// unlock the index buffer
	pIndexBufferRef->Unlock ( );

	// all went okay
	return true;
}

DARKSDK_DLL bool CopyVertexBufferToMeshDataSameFVF ( sMesh* pMesh, IDirect3DVertexBuffer9* pVertexBufferRef, DWORD dwBufferOffset )
{
	// lock the vertex buffer
	float* pfData = NULL;
	if ( FAILED ( pVertexBufferRef->Lock ( 0, 0, ( VOID** ) &pfData, 0 ) ) )
		return false;

	// copy across to the buffer
	memcpy ( pMesh->pVertexData, &pfData[ dwBufferOffset ], pMesh->dwFVFSize * pMesh->dwVertexCount );

	// unlock the vertex buffer
	pVertexBufferRef->Unlock ( );

	// all went okay
	return true;
}

DARKSDK_DLL bool CopyIndexMeshData ( sMesh* pDstMesh, sMesh* pSrcMesh, DWORD dwOffset, DWORD dwIndexCount )
{
	// copy across
	memcpy ( pDstMesh->pIndices + dwOffset, pSrcMesh->pIndices, sizeof(WORD) * dwIndexCount );
	return true;
}

DARKSDK_DLL bool IncrementIndexMeshData ( sMesh* pDstMesh, DWORD dwOffset, DWORD dwIndexCount, DWORD dwIncrement )
{
	// increment index data (used when adding a mesh to end of another mesh)
	WORD* pIndexData = pDstMesh->pIndices + dwOffset;
	for ( int iIndex=0; iIndex<(int)dwIndexCount; iIndex++ )
	{
		*pIndexData = (WORD)(*pIndexData + dwIncrement);
		pIndexData++;
	}
	return true;
}

DARKSDK_DLL bool CopyVertexMeshDataSameFVF ( sMesh* pDstMesh, sMesh* pSrcMesh, DWORD dwOffset, DWORD dwVertexCount )
{
	// copy across
	memcpy ( pDstMesh->pVertexData + ( dwOffset * pSrcMesh->dwFVFSize ), pSrcMesh->pVertexData, pSrcMesh->dwFVFSize * dwVertexCount );
	return true;
}

DARKSDK_DLL void SplitMeshSide ( int iSide, sMesh* pMesh, sMesh* pSplitMesh )
{
	// determine direction of side
	D3DXVECTOR3 vecDirection;
	switch ( iSide )
	{
		case 0 : vecDirection = D3DXVECTOR3 ( 0.0f, 0.0f, 1.0f );
		case 1 : vecDirection = D3DXVECTOR3 ( 0.0f, 0.0f,-1.0f );
		case 2 : vecDirection = D3DXVECTOR3 ( 0.0f,-1.0f, 0.0f );
		case 3 : vecDirection = D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f );
		case 4 : vecDirection = D3DXVECTOR3 (-1.0f, 0.0f, 0.0f );
		case 5 : vecDirection = D3DXVECTOR3 ( 1.0f, 0.0f, 0.0f );
	}

	// copy index data from mesh to single-mesh
	BYTE* pVertexData = (BYTE*)pMesh->pVertexData;
	BYTE* pNormalData = pVertexData + ( sizeof(float)*3 );
	WORD* pIndexData = (WORD*)pMesh->pIndices;

	// transform vertex data by world matrix of frame
	for ( DWORD i=0; i<pMesh->dwIndexCount; i+=3 )
	{
		// get poly vertex normals
		DWORD v0 = pIndexData[i+0];
		DWORD v1 = pIndexData[i+1];
		DWORD v2 = pIndexData[i+2];
		D3DXVECTOR3* pNormal0 = (D3DXVECTOR3*)(pNormalData+(v0*pMesh->dwFVFSize));
		D3DXVECTOR3* pNormal1 = (D3DXVECTOR3*)(pNormalData+(v1*pMesh->dwFVFSize));
		D3DXVECTOR3* pNormal2 = (D3DXVECTOR3*)(pNormalData+(v2*pMesh->dwFVFSize));
		
		// work out poly normal
		D3DXVECTOR3 vecPolyNormal = *pNormal0 + *pNormal1 + *pNormal2;
		D3DXVec3Normalize ( &vecPolyNormal, &vecPolyNormal );

		// if facing node side, split it
		if ( D3DXVec3Dot ( &vecPolyNormal, &vecDirection ) > 0.0f )
		{
			// hide polygon fpr test
			*((D3DXVECTOR3*)(pVertexData+(v1*pMesh->dwFVFSize))) = *((D3DXVECTOR3*)(pVertexData+(v0*pMesh->dwFVFSize)));
			*((D3DXVECTOR3*)(pVertexData+(v2*pMesh->dwFVFSize))) = *((D3DXVECTOR3*)(pVertexData+(v0*pMesh->dwFVFSize)));
		}
	}
}

DARKSDK_DLL bool GetFrameCount ( sFrame* pFrame, int* piCount )
{
	// check if the frame and counter is okay is ok
	SAFE_MEMORY ( pFrame );
	SAFE_MEMORY ( piCount );

	// increment the count
	//*piCount += 1;
	// get frames for the sibling and child
	//GetFrameCount ( pFrame->pChild,   piCount );
	//GetFrameCount ( pFrame->pSibling, piCount );

	// avoids recirsion issue
	sFrame* pThis = pFrame;
	while ( pThis )
	{
		*piCount += 1;
		GetFrameCount ( pThis->pChild, piCount );
		pThis = pThis->pSibling;
	}

	return true;
}

DARKSDK_DLL bool BuildFrameList ( sFrame** pFrameList, sFrame* pFrame, int* iStart )
{
	// check if the pointers are valid
	SAFE_MEMORY ( pFrameList );
	SAFE_MEMORY ( pFrame );
	SAFE_MEMORY ( iStart );

	//pFrameList [ *iStart ] = pFrame;
	//pFrame->iID = *iStart;
	//*iStart += 1;
	// keep on calling build, we need to run through all items in the list
	//BuildFrameList ( pFrameList, pFrame->pChild, iStart );
	//BuildFrameList ( pFrameList, pFrame->pSibling, iStart );

	// avoids recirsion issue
	sFrame* pThis = pFrame;
	while ( pThis )
	{
		pFrameList [ *iStart ] = pThis;
		pThis->iID = *iStart;
		*iStart += 1;

		// and next down to tackle children (if any)
		BuildFrameList ( pFrameList, pThis->pChild, iStart );

		// next one
		pThis = pThis->pSibling;
	}

	return true;
}

DARKSDK_DLL bool GetMeshCount ( sFrame* pFrame, int* piCount )
{
	// check if the pointers are valids
	SAFE_MEMORY ( pFrame );
	SAFE_MEMORY ( piCount );

	// see if we have a mesh
	//if ( pFrame->pMesh ) *piCount += 1;
	//GetMeshCount ( pFrame->pChild,   piCount );
	//GetMeshCount ( pFrame->pSibling, piCount );

	// avoids recirsion issue
	sFrame* pThis = pFrame;
	while ( pThis )
	{
		if ( pThis->pMesh ) *piCount += 1;
		GetMeshCount ( pThis->pChild, piCount );
		pThis = pThis->pSibling;
	}

	return true;
}

DARKSDK_DLL bool BuildMeshList ( sMesh** pMeshList, sFrame* pFrame, int* iStart )
{
	// check if the pointers are valid
	SAFE_MEMORY ( pMeshList );
	SAFE_MEMORY ( pFrame );
	SAFE_MEMORY ( iStart );

	// see if we have a mesh
	//if ( pFrame->pMesh )
	//{
	//	pMeshList [ *iStart ] = pFrame->pMesh;
	//	pFrame->iID = *iStart;
	//	*iStart += 1;
	//}
	//BuildMeshList ( pMeshList, pFrame->pChild, iStart );
	//BuildMeshList ( pMeshList, pFrame->pSibling, iStart );

	// recursive causes stack overflow on large linklists, do old fashioned way
	// delete all pointers which will in turn delete all sub frames
	// keep on calling build, we need to run through all items in the list
	//BuildMeshList ( pMeshList, pFrame->pChild, iStart );
	//BuildMeshList ( pMeshList, pFrame->pSibling, iStart );
	sFrame* pThis = pFrame;
	while ( pThis )
	{
		// see if we have a mesh
		if ( pThis->pMesh )
		{
			pMeshList [ *iStart ] = pThis->pMesh;
			pThis->iID = *iStart;
			*iStart += 1;
		}

		// and next down to tackle children (if any)
		BuildMeshList ( pMeshList, pThis->pChild, iStart );

		// next one
		pThis = pThis->pSibling;
	}

	return true;
}

DARKSDK_DLL sFrame* FindFrame ( char* szName, sFrame* pFrame )
{
	// finds a frame with a given name, this function will
	// search through any sub frames of the frame passed in

	// make sure the frame is valid
	if ( !pFrame )
		return NULL;

	// if no name is specified return this frame
	if ( !szName )
		return pFrame;

	// set up a sub frame for more searches
	sFrame* pSubFrame = NULL;

	// compare names and return if exact match
	if ( pFrame->szName != NULL && !strcmp ( szName, pFrame->szName ) )
		return pFrame;

	// see if there is a child frame we can check
	if ( pFrame->pChild )
	{
		// see if we have a match with the child frame
		if ( ( pSubFrame = FindFrame ( szName, pFrame->pChild ) ) != NULL )
			return pSubFrame;
	}

	// see if there is a sibling frame
	if ( pFrame->pSibling )
	{
		// see if we have a match with the sibling frame
		if ( ( pSubFrame = FindFrame ( szName, pFrame->pSibling ) ) != NULL )
			return pSubFrame;
	}

	// no match found in the hiearachy
	return NULL;
}

DARKSDK_DLL sAnimation* FindAnimation ( sObject* pObject, sFrame* pOriginalFrame )
{
	// use original frame name to find pAnim and duplicate for new frame
	if ( pObject->pAnimationSet )
	{
		sAnimation* pCurrentAnim = pObject->pAnimationSet->pAnimation;
		if ( pCurrentAnim )
		{
			// go through all anims
			while ( pCurrentAnim )
			{
				if ( _stricmp ( pCurrentAnim->szName, pOriginalFrame->szName )==NULL )
				{
					// Found animation
					return pCurrentAnim;
				}
				pCurrentAnim = pCurrentAnim->pNext;
			}
		}
	}

	// could not find
	return NULL;
}

DARKSDK_DLL sAnimation* CopyAnimation ( sAnimation* pCurrentAnim, LPSTR szNewName )
{
	// found original anim data - copy it
	sAnimation* pNewAnim = new sAnimation;

	// copy over values
	strcpy ( pNewAnim->szName,		  szNewName );
	pNewAnim->bLoop					= pCurrentAnim->bLoop;
	pNewAnim->bLinear				= pCurrentAnim->bLinear;
	pNewAnim->dwNumPositionKeys		= pCurrentAnim->dwNumPositionKeys;
	pNewAnim->dwNumRotateKeys		= pCurrentAnim->dwNumRotateKeys;
	pNewAnim->dwNumScaleKeys		= pCurrentAnim->dwNumScaleKeys;
	pNewAnim->dwNumMatrixKeys		= pCurrentAnim->dwNumMatrixKeys;

	// create arrays and copy data
	pNewAnim->pPositionKeys = new sPositionKey[pCurrentAnim->dwNumPositionKeys];
	memcpy ( pNewAnim->pPositionKeys, pCurrentAnim->pPositionKeys, sizeof(sPositionKey)*pCurrentAnim->dwNumPositionKeys );
	pNewAnim->pRotateKeys = new sRotateKey[pCurrentAnim->dwNumRotateKeys];
	memcpy ( pNewAnim->pRotateKeys, pCurrentAnim->pRotateKeys, sizeof(sRotateKey)*pCurrentAnim->dwNumRotateKeys );
	pNewAnim->pScaleKeys = new sScaleKey[pCurrentAnim->dwNumScaleKeys];
	memcpy ( pNewAnim->pScaleKeys, pCurrentAnim->pScaleKeys, sizeof(sScaleKey)*pCurrentAnim->dwNumScaleKeys );
	pNewAnim->pMatrixKeys = new sMatrixKey[pCurrentAnim->dwNumMatrixKeys];
	memcpy ( pNewAnim->pMatrixKeys, pCurrentAnim->pMatrixKeys, sizeof(sMatrixKey)*pCurrentAnim->dwNumMatrixKeys );

	// add new anim to existing list
	while ( pCurrentAnim->pNext ) pCurrentAnim = pCurrentAnim->pNext;
	pCurrentAnim->pNext = pNewAnim;

	return pNewAnim;
}

DARKSDK_DLL bool SortAnimationPositionByTime ( sAnimation* pAnim )
{
	// leefix - 270203 - some animation data is not time sorted (required for keyframe finder)
	SAFE_MEMORY ( pAnim->pPositionKeys );

	// store the number of keys
	DWORD dwNumKeys = pAnim->dwNumPositionKeys;

	/* 121113 - Reloaded assumes all keyframes sequential for performance
	// bubble sort into time ascending order (or key-frame select gets messed up)
	for ( int iKeyA = 0; iKeyA < ( int ) dwNumKeys; iKeyA++ )
	{
		for ( int iKeyB = iKeyA; iKeyB < ( int ) dwNumKeys; iKeyB++ )
		{
			if ( iKeyA!=iKeyB )
			{
				if ( pAnim->pPositionKeys [ iKeyB ].dwTime < pAnim->pPositionKeys [ iKeyA ].dwTime ) 
				{
					// swap A and B
					sPositionKey sStoreA = pAnim->pPositionKeys [ iKeyA ];
					pAnim->pPositionKeys [ iKeyA ] = pAnim->pPositionKeys [ iKeyB ];
					pAnim->pPositionKeys [ iKeyB ] = sStoreA;
				}
			}
		}
	}
	*/

	// work out interpolation data after keyframes sorted
	if ( dwNumKeys > 1 )
	{
		for ( int iKey = 0; iKey < ( int ) dwNumKeys - 1; iKey++ )
		{
			DWORD dwTime;

			pAnim->pPositionKeys [ iKey ].vecPosInterpolation = pAnim->pPositionKeys [ iKey + 1 ].vecPos - pAnim->pPositionKeys [ iKey ].vecPos;
			dwTime                                            = pAnim->pPositionKeys [ iKey + 1 ].dwTime - pAnim->pPositionKeys [ iKey ].dwTime;
			
			if ( !dwTime )
				dwTime = 1;

			pAnim->pPositionKeys [ iKey ].vecPosInterpolation /= ( float ) dwTime;
		}
	}

	// success
	return true;
}

DARKSDK_DLL bool SortAnimationRotationByTime ( sAnimation* pAnim )
{
	// leefix - 270203 - some animation data is not time sorted (required for keyframe finder)
	SAFE_MEMORY ( pAnim->pRotateKeys );

	// store the number of keys
	DWORD dwNumKeys = pAnim->dwNumRotateKeys;
	
	/* 121113 - Reloaded assumes all keyframes sequential for performance
	// bubble sort into time ascending order (or key-frame select gets messed up)
	for ( int iKeyA = 0; iKeyA < ( int ) dwNumKeys; iKeyA++ )
	{
		for ( int iKeyB = iKeyA; iKeyB < ( int ) dwNumKeys; iKeyB++ )
		{
			if ( iKeyA!=iKeyB )
			{
				if ( pAnim->pRotateKeys [ iKeyB ].dwTime < pAnim->pRotateKeys [ iKeyA ].dwTime ) 
				{
					// swap A and B
					sRotateKey sStoreA = pAnim->pRotateKeys [ iKeyA ];
					pAnim->pRotateKeys [ iKeyA ] = pAnim->pRotateKeys [ iKeyB ];
					pAnim->pRotateKeys [ iKeyB ] = sStoreA;
				}
			}
		}
	}
	*/

	// success
	return true;
}

DARKSDK_DLL bool SortAnimationScaleByTime ( sAnimation* pAnim )
{
	// leefix - 270203 - some animation data is not time sorted (required for keyframe finder)
	SAFE_MEMORY ( pAnim->pScaleKeys );

	// store the number of keys
	DWORD dwNumKeys = pAnim->dwNumScaleKeys;
	
	/* 121113 - Reloaded assumes all keyframes sequential for performance
	// bubble sort into time ascending order (or key-frame select gets messed up)
	for ( int iKeyA = 0; iKeyA < ( int ) dwNumKeys; iKeyA++ )
	{
		for ( int iKeyB = iKeyA; iKeyB < ( int ) dwNumKeys; iKeyB++ )
		{
			if ( iKeyA!=iKeyB )
			{
				if ( pAnim->pScaleKeys [ iKeyB ].dwTime < pAnim->pScaleKeys [ iKeyA ].dwTime ) 
				{
					// swap A and B
					sScaleKey sStoreA = pAnim->pScaleKeys [ iKeyA ];
					pAnim->pScaleKeys [ iKeyA ] = pAnim->pScaleKeys [ iKeyB ];
					pAnim->pScaleKeys [ iKeyB ] = sStoreA;
				}
			}
		}
	}
	*/

	// work out interpolation data after keyframes sorted
	if ( dwNumKeys > 1 )
	{
		for ( int iKey = 0; iKey < ( int ) dwNumKeys - 1; iKey++ )
		{
			DWORD dwTime = 0;

			pAnim->pScaleKeys [ iKey ].vecScaleInterpolation = pAnim->pScaleKeys [ iKey + 1 ].vecScale - pAnim->pScaleKeys [ iKey ].vecScale;
			dwTime                                           = pAnim->pScaleKeys [ iKey + 1 ].dwTime   - pAnim->pScaleKeys [ iKey ].dwTime;
			
			if ( !dwTime )
				dwTime = 1;

			pAnim->pScaleKeys [ iKey ].vecScaleInterpolation /= ( float ) dwTime;
		}
	}

	// success
	return true;
}

DARKSDK_DLL bool SortAnimationDataByTime ( sAnimation* pAnim )
{
	// sort position, rotation and scale data and calc interpolations
	SortAnimationPositionByTime	( pAnim );
	SortAnimationRotationByTime	( pAnim );
	SortAnimationScaleByTime	( pAnim );

	// success
	return true;
}

bool MapFramesToAnimations ( sObject* pObject )
{
	// go through the animation and find the frames which are used
	// we then store a pointer to the frame in the anim structure

	// leefix - 171203 - for MD*, we can first for sublist count
	pObject->fAnimTotalFrames = 0.0f;
	for ( int iMesh=0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		sMesh* pMesh = pObject->ppMeshList [ iMesh ];
		if ( pMesh )
		{
			float fNewMax = ( float ) pMesh->dwSubMeshListCount;
			if ( fNewMax > pObject->fAnimTotalFrames )
			{
				pObject->fAnimTotalFrames = fNewMax;
				pObject->fAnimFrameEnd = fNewMax;
			}
		}
	}

	// check the memory we need to access
	SAFE_MEMORY ( pObject );
	SAFE_MEMORY ( pObject->pAnimationSet );
	SAFE_MEMORY ( pObject->pAnimationSet->pAnimation );

	// setup some local variables
	sAnimationSet* pAnimSet = NULL;
	sAnimation*    pAnim    = NULL;

	// MIKE 240303
	// LEEFIX - 171203 - this cannot work as some MD* do not have 'pAnimationSet'
	if ( !pObject->pAnimationSet->pAnimation->bBoneType )
	{
		pObject->fAnimTotalFrames = ( float ) pObject->pAnimationSet->pAnimation->iBoneFrameA - 1;
		pObject->fAnimFrameEnd    = pObject->fAnimTotalFrames;
		return true;
	}

	// get a pointer to the animation set
	pAnimSet = pObject->pAnimationSet;

	// get total length of animation
	DWORD dwTotalLength=0;

	// we need to run through all of the set
	LONG lStartPerf = timeGetTime();
	while ( pAnimSet != NULL )
	{
		// get a pointer to the animation data
		pAnim = pAnimSet->pAnimation;

		// run through all animation sequences
		int iCountMe = 0;
		LONG lStartPerf2 = timeGetTime();
		while ( pAnim != NULL )
		{
			// scans all animation data and creates the interpolation vectors between all keyframes (vital)
			if ( pAnim ) SortAnimationDataByTime ( pAnim );

			// find the frame which matches the animation
			pAnim->pFrame = FindFrame ( pAnim->szName, pObject->pFrame );	// find matching frame
			pAnim         = pAnim->pNext;									// move to next animation sequence
			iCountMe++;
		}
		lStartPerf2 = timeGetTime() - lStartPerf2;

		// update total length
		if ( pAnimSet->ulLength > dwTotalLength ) dwTotalLength = pAnimSet->ulLength;

		// move to next animation set
		pAnimSet = pAnimSet->pNext;
	}
	lStartPerf = timeGetTime() - lStartPerf;

	// store total frames in object
	pObject->fAnimTotalFrames = (float)dwTotalLength;
	pObject->fAnimFrameEnd = pObject->fAnimTotalFrames;

	// return back to caller
	return true;
}

void InitOneMeshFramesToBones ( sMesh* pMesh )
{
	// create the frame mapping matrix array and clear out
	pMesh->pFrameRef = new sFrame* [ pMesh->dwBoneCount ];
	pMesh->pFrameMatrices = new D3DXMATRIX* [ pMesh->dwBoneCount ];

	// set all matrix pointers to null
	for ( int i = 0; i < ( int ) pMesh->dwBoneCount; i++ )
	{
		pMesh->pFrameRef [ i ] = NULL;
		pMesh->pFrameMatrices [ i ] = NULL;
	}
}

DARKSDK_DLL bool InitFramesToBones ( sMesh** pMeshList, int iMeshCount )
{
	// check mesh list
	SAFE_MEMORY ( *pMeshList );

	// get first mesh
	for ( int iMesh=0; iMesh < iMeshCount; iMesh++ )
	{
		sMesh* pMesh = pMeshList [ iMesh ];
		if ( pMesh )
		{
			// create the frame mapping matrix array and clear out
			InitOneMeshFramesToBones ( pMesh );
		}
	}

	// okay
	return true;
}

bool MapFramesToBones ( sMesh** pMesh, sFrame *pFrame, int iCount )
{
	// check mesh and frame
	SAFE_MEMORY ( *pMesh );
	SAFE_MEMORY ( pFrame );

	// current position
	int iPos = 0;

	// get first mesh
	sMesh* pMain = pMesh [ iPos ];

	// scan through meshes looking for bone matches
	if ( pMain != NULL )
	{
		// run through all meshes in list
		while ( 1 )
		{
			// get mesh and advance
			pMain = pMesh [ iPos++ ];

			// only update if we are dealing with bones
			if ( pMain->dwBoneCount )
			{
				// get list of bone names
				for ( DWORD i = 0; i < pMain->dwBoneCount; i++ )
				{
					// find the frame which matches the bone
					if ( strcmp ( pFrame->szName, pMain->pBones [ i ].szName ) == 0 )
					{
						// leeadd - 180204 - also store reference to frame (for bone-anim model limb based collision)
						pMain->pFrameRef [ i ] = pFrame;

						// lee - 021114 - record WHICH bone is referenced so UpdateFrame can use SkinOffset local transform matrix from bone
						pFrame->pmatBoneLocalTransform = &pMain->pBones [ i ].matTranslation;

						// store the matrix
						pMain->pFrameMatrices [ i ] = &pFrame->matCombined;

						// done
						break;
					}
				}
			}

			if ( iPos == iCount )
				break;
		}
	}

	// scan through child frames
	MapFramesToBones ( pMesh, pFrame->pChild, iCount );
	
	// scan through sibling frames
	MapFramesToBones ( pMesh, pFrame->pSibling, iCount );

	return true;
}

bool MapOneMeshFramesToBones ( sMesh* pMain, sFrame* pFrame )
{
	// only update if we are dealing with bones
	if ( pMain->dwBoneCount )
	{
		// get list of bone names
		for ( DWORD i = 0; i < pMain->dwBoneCount; i++ )
		{
			// find the frame which matches the bone
			if ( strcmp ( pFrame->szName, pMain->pBones [ i ].szName ) == 0 )
			{
				pMain->pFrameRef [ i ] = pFrame;
				pMain->pFrameMatrices [ i ] = &pFrame->matCombined;
				break;
			}
		}
	}

	// scan through child frames
	if ( pFrame->pChild ) MapOneMeshFramesToBones ( pMain, pFrame->pChild );
	
	// scan through sibling frames
	if ( pFrame->pSibling ) MapOneMeshFramesToBones ( pMain, pFrame->pSibling );

	return true;
}

DARKSDK_DLL void UpdateObjectCamDistance ( sObject* pObject )
{
	// using mesh LOD style ONLY
	if ( pObject->iUsingWhichLOD!=-1000 )
	{
		// store current LOD index
		int iStoreLOD = pObject->iUsingWhichLOD;

		// handle any LOD assigning (only if not in transition)
		if ( pObject->iUsingOldLOD==-1 )
		{
			if ( pObject->position.fCamDistance < pObject->fLODDistance [ 0 ] || pObject->fLODDistance [ 0 ]==0 )
				pObject->iUsingWhichLOD = 0;
			else
			{
				if ( pObject->position.fCamDistance < pObject->fLODDistance [ 1 ] || pObject->fLODDistance [ 1 ]==0 )
					pObject->iUsingWhichLOD = 1;
				else
				{
					if ( pObject->position.fCamDistance < pObject->fLODDistanceQUAD || pObject->fLODDistanceQUAD==0 )
						pObject->iUsingWhichLOD = 2;
					else
						pObject->iUsingWhichLOD = 3;
				}
			}

			// leeadd - U71 - add transitional code
			if ( iStoreLOD != pObject->iUsingWhichLOD )
			{
				// LOD level has changed due to distance, so initiate transition effect (handled in objectmanager)
				pObject->iUsingOldLOD = iStoreLOD;
				pObject->fLODTransition = 0.0f;
			}
		}
	}
}

DARKSDK_DLL void UpdateObjectAnimation ( sObject* pObject )
{
	// handle vertex level animation (even if not animating)
	if ( pObject->pAnimationSet )
	{
		// only need to go for first frame for MDL models
		if ( !pObject->pAnimationSet->pAnimation->bBoneType )
		{
			// just animate the first mesh MDL
			AnimateBoneMesh ( pObject, pObject->ppFrameList [ 0 ] );
			pObject->fAnimLastFrame = pObject->fAnimFrame;
			pObject->fAnimSlerpLastTime = pObject->fAnimSlerpTime;
		}
	}

	// U74 - 120409 - not NECESSARILY animation data uses below (remove if ( pObject->pAnimationSet ))
	if ( 1 )
	{		
		// all meshes in object
		for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
		{
			// get a pointer to the frame
			sFrame* pFrame = pObject->ppFrameList [ iFrame ];
			if ( pFrame==NULL ) continue;

			// determine if we need to animate
			bool bProceedToAnimate = false;
			if ( pObject->pAnimationSet && pObject->fAnimTotalFrames > 0.0f ) bProceedToAnimate = true;
			if ( pObject->bAnimUpdateOnce ) bProceedToAnimate = true;

			// 130513 - no need to animate limbs we have hidden (useful when hiding LOD meshes)
			if ( pFrame->pMesh )
				if ( pFrame->pMesh->bVisible==false )
					bProceedToAnimate = false;

			// if object is parent of instance, must animate ALL (main mesh and LOD versions)
			if ( pObject->position.bParentOfInstance )
			{
				if ( bProceedToAnimate )
				{
					// animate all meshes
					sMesh* pMesh = NULL;
					pMesh = pFrame->pMesh;
					if ( pMesh && pMesh->dwBoneCount > 0 ) AnimateBoneMesh ( pObject, pFrame, pMesh );
					pMesh = pFrame->pLOD[0];
					if ( pMesh && pMesh->dwBoneCount > 0 ) AnimateBoneMesh ( pObject, pFrame, pMesh );
					pMesh = pFrame->pLOD[1];
					if ( pMesh && pMesh->dwBoneCount > 0 ) AnimateBoneMesh ( pObject, pFrame, pMesh );
					pMesh = pFrame->pLODForQUAD;
					if ( pMesh && pMesh->dwBoneCount > 0 ) AnimateBoneMesh ( pObject, pFrame, pMesh );

					// Update regular bounds
					UpdateBoundBoxMesh ( pFrame );
					UpdateBoundSphereMesh ( pFrame );
				}
			}
			else
			{
				// choose mesh based on any LOD distance
				sMesh* pMesh = NULL;
				if ( pObject->iUsingWhichLOD==-1000 )
				{
					// uses built-in distance based LOD mesh selector (only animate LOD being used/visible)
					if ( pFrame->pMesh )
					{
						if ( pFrame->pMesh->bVisible==true )
						{
							pMesh = pFrame->pMesh;
						}
					}
				}
				else
				{
					if ( pObject->iUsingWhichLOD==0 )
					{
						// normal object-mesh rendering
						pMesh = pFrame->pMesh;
					}
					else
					{
						if ( pObject->iUsingWhichLOD==1 )
						{
							// LOD2
							pMesh = pFrame->pLOD[0];
						}
						else
						{
							if ( pObject->iUsingWhichLOD==2 )
							{
								// LOD3
								pMesh = pFrame->pLOD[1];
							}
							else
							{
								// LODQUAD
								pMesh = pFrame->pLODForQUAD;
							}
						}
					}
				}

				// for each mesh
				if ( pMesh )
				{
					// use mesh bone animation
					if ( bProceedToAnimate )
					{
						// anim can have matrix data and NO bones
						if ( pMesh->dwBoneCount > 0 )
						{
							// animate bones with matrix animation data
							AnimateBoneMesh ( pObject, pFrame, pMesh );

							// Update regular bounds
							UpdateBoundBoxMesh ( pFrame );
							UpdateBoundSphereMesh ( pFrame );
						}
					}
				}

				// also animate any old LOD mesh in transition
				sMesh* pOldLODMesh = NULL;
				if ( pObject->iUsingOldLOD==0 )
					pOldLODMesh = pFrame->pMesh;
				else
				{
					if ( pObject->iUsingOldLOD==1 )
						pOldLODMesh = pFrame->pLOD[0];
					else
					{
						if ( pObject->iUsingOldLOD==2 )
							pOldLODMesh = pFrame->pLOD[1];
						else
							if ( pObject->iUsingOldLOD==3 )
								pOldLODMesh = pFrame->pLODForQUAD;
					}
				}
				if ( pOldLODMesh )
					if ( pObject->fAnimTotalFrames > 0.0f || pObject->bAnimUpdateOnce )
						if ( pOldLODMesh->dwBoneCount > 0 )
							AnimateBoneMesh ( pObject, pFrame, pOldLODMesh );
			}
		}
		
		// reset value to here
		// pObject->bAnimUpdateOnce = false;	// U75 - 051209 - caused DarkPHYSICS ragdoll rope demo to fail!

		// Store frame for next quick reject check
		pObject->fAnimLastFrame = pObject->fAnimFrame;
		pObject->fAnimSlerpLastTime = pObject->fAnimSlerpTime;
	}
}

/* U72 - 100109 - seems the old pAnimationSet check seperated two blocks of code that
// did exactly the same thing. pAnimationSet should only be needed for the very top part (so see above)
DARKSDK_DLL void UpdateObjectAnimation ( sObject* pObject )
{
	// handle vertex level animation (even if not animating)
	if ( pObject->pAnimationSet )
	{
		// MIKE 240303 - only need to go for first frame for MDL models
		if ( !pObject->pAnimationSet->pAnimation->bBoneType )
		{
			// just animate the first mesh MDL
			AnimateBoneMesh ( pObject, pObject->ppFrameList [ 0 ] );
			pObject->fAnimLastFrame = pObject->fAnimFrame;
			pObject->fAnimSlerpLastTime = pObject->fAnimSlerpTime;
			return;
		}
		
		// all meshes in object
		for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
		{
			// get a pointer to the frame
			sFrame* pFrame = pObject->ppFrameList [ iFrame ];

			// leeadd - 240106 - choose mesh based on any LOD distance
			sMesh* pMesh = NULL;
			if ( pObject->iUsingWhichLOD==0 )
			{
				// normal object-mesh rendering
				pMesh = pFrame->pMesh;
			}
			else
			{
				if ( pObject->iUsingWhichLOD==1 )
				{
					// LOD2
					pMesh = pFrame->pLOD[0];
				}
				else
				{
					if ( pObject->iUsingWhichLOD==2 )
					{
						// LOD3
						pMesh = pFrame->pLOD[1];
					}
					else
					{
						// LODQUAD
						pMesh = pFrame->pLODForQUAD;
					}
				}
			}

			// for each mesh
			if ( pMesh )
			{
				// use mesh bone animation
				if ( pObject->fAnimTotalFrames > 0.0f || pObject->bAnimUpdateOnce )
				{
					// leeadd - 080908 - new TS76 anim can have matrix data and NO bones
					if ( pMesh->dwBoneCount > 0 )
					{
						// animate bones with matrix animation data
						AnimateBoneMesh ( pObject, pFrame, pMesh );
					}
				}
					
				// Update regular bounds
				UpdateBoundBoxMesh ( pFrame );
				UpdateBoundSphereMesh ( pFrame );
			}

			// leeadd - U71 - also animate any old LOD mesh in transition
			sMesh* pOldLODMesh = NULL;
			if ( pObject->iUsingOldLOD==0 )
				pOldLODMesh = pFrame->pMesh;
			else
			{
				if ( pObject->iUsingOldLOD==1 )
					pOldLODMesh = pFrame->pLOD[0];
				else
				{
					if ( pObject->iUsingOldLOD==2 )
						pOldLODMesh = pFrame->pLOD[1];
					else
						if ( pObject->iUsingOldLOD==3 )
							pOldLODMesh = pFrame->pLODForQUAD;
				}
			}
			if ( pOldLODMesh )
				if ( pObject->fAnimTotalFrames > 0.0f || pObject->bAnimUpdateOnce )
					if ( pOldLODMesh->dwBoneCount > 0 )
						AnimateBoneMesh ( pObject, pFrame, pOldLODMesh );
		}
		
		// mike - 140207 - change the reset value to here
		pObject->bAnimUpdateOnce = false;
	}
	else
	{
		// MIKE 120503 - for animating other meshes
		// leefix - 190503 - instance objects have no mesh and would crash this..
		if ( pObject->ppFrameList )
		{
			if ( pObject->ppFrameList [ 0 ] && (pObject->bAnimPlaying || pObject->bAnimUpdateOnce) )
			{
				for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
				{
					// get a pointer to the frame
					sFrame* pFrame = pObject->ppFrameList [ iFrame ];

					// leeadd - 041208 - U71 - choose mesh based on any LOD distance (forgot about limb changes)
					sMesh* pMesh = NULL;
					if ( pObject->iUsingWhichLOD==0 )
					{
						// normal object-mesh rendering
						pMesh = pFrame->pMesh;
					}
					else
					{
						if ( pObject->iUsingWhichLOD==1 )
						{
							// LOD2
							pMesh = pFrame->pLOD[0];
						}
						else
						{
							if ( pObject->iUsingWhichLOD==2 )
							{
								// LOD3
								pMesh = pFrame->pLOD[1];
							}
							else
							{
								// LODQUAD
								pMesh = pFrame->pLODForQUAD;
							}
						}
					}

					// for each mesh
					if ( pMesh )
					{
						AnimateBoneMesh ( pObject, pFrame, pMesh );
					}

					// leeadd - U71 - also animate any old LOD mesh in transition
					sMesh* pOldLODMesh = NULL;
					if ( pObject->iUsingOldLOD==0 )
						pOldLODMesh = pFrame->pMesh;
					else
					{
						if ( pObject->iUsingOldLOD==1 )
							pOldLODMesh = pFrame->pLOD[0];
						else
						{
							if ( pObject->iUsingOldLOD==2 )
								pOldLODMesh = pFrame->pLOD[1];
							else
								if ( pObject->iUsingOldLOD==3 )
									pOldLODMesh = pFrame->pLODForQUAD;
						}
					}
					if ( pOldLODMesh )
						AnimateBoneMesh ( pObject, pFrame, pOldLODMesh );
				}
			}
		}
	}

	// Store frame for next quick reject check
	pObject->fAnimLastFrame = pObject->fAnimFrame;
	pObject->fAnimSlerpLastTime = pObject->fAnimSlerpTime;
}
*/

DARKSDK_DLL bool SetupShortVertex ( DWORD dwFVF, BYTE* pVertex, int iOffset, float x, float y, float z, float tu, float tv )
{
	// check the memory pointer is valid
	SAFE_MEMORY ( pVertex );

	// get the offset map
	sOffsetMap	offsetMap;
	GetFVFValueOffsetMap ( dwFVF, &offsetMap );

	// we can only work with any valid formats
	if ( dwFVF & D3DFVF_XYZ )
	{
		*( ( float* ) pVertex + offsetMap.dwX       + ( offsetMap.dwSize * iOffset ) ) = x;
		*( ( float* ) pVertex + offsetMap.dwY       + ( offsetMap.dwSize * iOffset ) ) = y;
		*( ( float* ) pVertex + offsetMap.dwZ       + ( offsetMap.dwSize * iOffset ) ) = z;
	}
	if ( dwFVF & D3DFVF_TEX1 )
	{
		*( ( float* ) pVertex + offsetMap.dwTU[0]      + ( offsetMap.dwSize * iOffset ) ) = tu;
		*( ( float* ) pVertex + offsetMap.dwTV[0]      + ( offsetMap.dwSize * iOffset ) ) = tv;
	}

	// okay
	return true;
}

DARKSDK_DLL bool SetupStandardVertex ( DWORD dwFVF, BYTE* pVertex, int iOffset, float x, float y, float z, float nx, float ny, float nz, DWORD dwDiffuseColour, float tu, float tv )
{
	// setup a standard lit vertex

	// check the memory pointer is valid
	SAFE_MEMORY ( pVertex );

	// get the offset map
	sOffsetMap	offsetMap;
	GetFVFValueOffsetMap ( dwFVF, &offsetMap );

	// we can only work with any valid formats
	if ( dwFVF & D3DFVF_XYZ )
	{
		*( ( float* ) pVertex + offsetMap.dwX       + ( offsetMap.dwSize * iOffset ) ) = x;
		*( ( float* ) pVertex + offsetMap.dwY       + ( offsetMap.dwSize * iOffset ) ) = y;
		*( ( float* ) pVertex + offsetMap.dwZ       + ( offsetMap.dwSize * iOffset ) ) = z;
	}
	if ( dwFVF & D3DFVF_NORMAL )
	{
		*( ( float* ) pVertex + offsetMap.dwNX      + ( offsetMap.dwSize * iOffset ) ) = nx;
		*( ( float* ) pVertex + offsetMap.dwNY      + ( offsetMap.dwSize * iOffset ) ) = ny;
		*( ( float* ) pVertex + offsetMap.dwNZ      + ( offsetMap.dwSize * iOffset ) ) = nz;
	}
	if ( dwFVF & D3DFVF_DIFFUSE )
	{
		*( ( DWORD* ) pVertex + offsetMap.dwDiffuse + ( offsetMap.dwSize * iOffset ) ) = dwDiffuseColour;
	}
	if ( dwFVF & D3DFVF_TEX1 )
	{
		*( ( float* ) pVertex + offsetMap.dwTU[0]      + ( offsetMap.dwSize * iOffset ) ) = tu;
		*( ( float* ) pVertex + offsetMap.dwTV[0]      + ( offsetMap.dwSize * iOffset ) ) = tv;
	}

	// mike - 160903 - point size support, set to 1.0f by default
	if ( dwFVF & D3DFVF_PSIZE )
	{
		*( ( float* ) pVertex + offsetMap.dwPointSize + ( offsetMap.dwSize * iOffset ) ) = 1.0f;
	}

	// okay
	return true;
}

DARKSDK_DLL bool SetupStandardVertexDec ( sMesh* pMesh, BYTE* pVertex, int iOffset, float x, float y, float z, float nx, float ny, float nz, DWORD dwDiffuseColour, float tu, float tv )
{
	// setup a standard lit vertex

	// check the memory pointer is valid
	SAFE_MEMORY ( pVertex );

	// get the offset map
	sOffsetMap	offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// we can only work with any valid formats
	if ( offsetMap.dwZ>0 )
	{
		*( ( float* ) pVertex + offsetMap.dwX       + ( offsetMap.dwSize * iOffset ) ) = x;
		*( ( float* ) pVertex + offsetMap.dwY       + ( offsetMap.dwSize * iOffset ) ) = y;
		*( ( float* ) pVertex + offsetMap.dwZ       + ( offsetMap.dwSize * iOffset ) ) = z;
	}
	if ( offsetMap.dwNZ>0 )
	{
		*( ( float* ) pVertex + offsetMap.dwNX      + ( offsetMap.dwSize * iOffset ) ) = nx;
		*( ( float* ) pVertex + offsetMap.dwNY      + ( offsetMap.dwSize * iOffset ) ) = ny;
		*( ( float* ) pVertex + offsetMap.dwNZ      + ( offsetMap.dwSize * iOffset ) ) = nz;
	}
	if ( offsetMap.dwDiffuse>0 )
	{
		*( ( DWORD* ) pVertex + offsetMap.dwDiffuse + ( offsetMap.dwSize * iOffset ) ) = dwDiffuseColour;
	}
	if ( offsetMap.dwTV[0]>0 )
	{
		*( ( float* ) pVertex + offsetMap.dwTU[0]      + ( offsetMap.dwSize * iOffset ) ) = tu;
		*( ( float* ) pVertex + offsetMap.dwTV[0]      + ( offsetMap.dwSize * iOffset ) ) = tv;
	}

	// mike - 160903 - point size support, set to 1.0f by default
	if ( offsetMap.dwPointSize>0 )
	{
		*( ( float* ) pVertex + offsetMap.dwPointSize + ( offsetMap.dwSize * iOffset ) ) = 1.0f;
	}

	// okay
	return true;
}

DARKSDK_DLL bool CreateFrameAndMeshList ( sObject* pObject )
{
	// 130213 - used to traverse 10,000 nests (no more) (avoids recursive stack overflow)
	int iNestCountMax = 9999;
	sFrame* pFrameBeforeNest[9999];
	sFrame* pThisFrame = NULL;
	int iNestCount = 0;

	// leeadd - 221105 - can make count negative efore obj call to protect mesh list
	if ( pObject->iMeshCount>=0 )
	{
		// find the number of meshes
		pObject->iMeshCount = 0;
		if ( 1 ) 
		{
			if ( !GetMeshCount ( pObject->pFrame, &pObject->iMeshCount ) )
				return false;
		}
		else
		{
			// does NOT produce same lists!!
			pThisFrame = pObject->pFrame;
			iNestCount = 0;
			while ( pThisFrame && iNestCount==0 )
			{	
				// act on frames
				if ( pThisFrame )
				{
					// count mesh
					if ( pThisFrame->pMesh ) pObject->iMeshCount++;

					// next item
					if ( pThisFrame->pChild )
					{
						pFrameBeforeNest[iNestCount] = pThisFrame;
						pThisFrame = pThisFrame->pChild;
						iNestCount++;
					}
					else
					{
						pThisFrame = pThisFrame->pSibling;
					}
				}
				if ( pThisFrame==NULL && iNestCount>0 )
				{
					iNestCount--;
					pThisFrame = pFrameBeforeNest[iNestCount];
					pThisFrame = pThisFrame->pSibling;
				}
			}
		}

		// allocate a list of frame which matches the number of meshes
		SAFE_DELETE_ARRAY ( pObject->ppMeshList );
		pObject->ppMeshList = new sMesh* [ pObject->iMeshCount ];
		SAFE_MEMORY ( pObject->ppMeshList );
	
		// get a list of frames which have meshes
		pObject->iMeshCount = 0;
		if ( 1 ) 
		{
			if ( !BuildMeshList ( pObject->ppMeshList, pObject->pFrame, &pObject->iMeshCount ) )
				return false;
		}
		else
		{
			// does NOT produce same lists!!
			pThisFrame = pObject->pFrame;
			iNestCount = 0;
			while ( pThisFrame && iNestCount==0 )
			{	
				// act on frames
				if ( pThisFrame )
				{
					// create mesh list entry
					if ( pThisFrame->pMesh )
					{
						pObject->ppMeshList [ pObject->iMeshCount ] = pThisFrame->pMesh;
						pThisFrame->iID = pObject->iMeshCount;
						pObject->iMeshCount++;
					}

					// next item
					if ( pThisFrame->pChild )
					{
						pFrameBeforeNest[iNestCount] = pThisFrame;
						pThisFrame = pThisFrame->pChild;
						iNestCount++;
					}
					else
					{
						pThisFrame = pThisFrame->pSibling;
					}
				}
				if ( pThisFrame==NULL && iNestCount>0 )
				{
					iNestCount--;
					pThisFrame = pFrameBeforeNest[iNestCount];
					pThisFrame = pThisFrame->pSibling;
				}
			}
		}
	}
	else
	{
		// negate count back - shows we have skipped this (kept old)
		pObject->iMeshCount *= -1;
	}

	// leeadd - 221105 - can make count negative efore obj call to protect frame list
	if ( pObject->iFrameCount>=0 )
	{
		pObject->iFrameCount = 0;
		if ( 1 ) 
		{
			if ( !GetFrameCount ( pObject->pFrame, &pObject->iFrameCount ) )
				return false;
		}
		else
		{
			// does NOT produce same lists!!
			pThisFrame = pObject->pFrame;
			iNestCount = 0;
			while ( pThisFrame && iNestCount==0 )
			{	
				// act on frames
				if ( pThisFrame )
				{
					// count frame
					pObject->iFrameCount++;

					// next item
					if ( pThisFrame->pChild )
					{
						pFrameBeforeNest[iNestCount] = pThisFrame;
						pThisFrame = pThisFrame->pChild;
						iNestCount++;
					}
					else
					{
						pThisFrame = pThisFrame->pSibling;
					}
				}
				if ( pThisFrame==NULL && iNestCount>0 )
				{
					iNestCount--;
					pThisFrame = pFrameBeforeNest[iNestCount];
					pThisFrame = pThisFrame->pSibling;
				}
			}
		}

		// allocate a list of frame which matches the number of frames
		SAFE_DELETE_ARRAY ( pObject->ppFrameList );
		pObject->ppFrameList = new sFrame* [ pObject->iFrameCount ];
		SAFE_MEMORY ( pObject->ppFrameList );

		// build up a list of frames
		pObject->iFrameCount = 0;
		if ( 1 )
		{
			if ( !BuildFrameList ( pObject->ppFrameList, pObject->pFrame, &pObject->iFrameCount ) )
				return false;
		}
		else
		{
			// does NOT produce same lists!!
			pThisFrame = pObject->pFrame;
			iNestCount = 0;
			while ( pThisFrame && iNestCount==0 )
			{	
				// act on frames
				if ( pThisFrame )
				{
					// create mesh list entry
					pObject->ppFrameList [ pObject->iFrameCount ] = pThisFrame;
					pThisFrame->iID = pObject->iFrameCount;
					pObject->iFrameCount++;

					// next item
					if ( pThisFrame->pChild )
					{
						pFrameBeforeNest[iNestCount] = pThisFrame;
						pThisFrame = pThisFrame->pChild;
						iNestCount++;
					}
					else
					{
						pThisFrame = pThisFrame->pSibling;
					}
				}
				if ( pThisFrame==NULL && iNestCount>0 )
				{
					iNestCount--;
					pThisFrame = pFrameBeforeNest[iNestCount];
					pThisFrame = pThisFrame->pSibling;
				}
			}
		}
	}
	else
	{
		// negate count back - shows we have skipped this (kept old)
		pObject->iFrameCount *= -1;
	}

	// if object has no meshes
	if ( pObject->iMeshCount==0 )
	{
		// do not attempt to draw it
		SAFE_DELETE ( pObject->ppMeshList );
		pObject->bNoMeshesInObject=true;
	}
	else
	{
		// default object does have meshes
		pObject->bNoMeshesInObject=false;
	}

	// okay
	return true;
}

DARKSDK_DLL bool SetupObjectsGenericProperties ( sObject* pObject )
{
	// check the object is valid
	SAFE_MEMORY ( pObject );
	SAFE_MEMORY ( pObject->pFrame );

	// create frame and mesh lists for object
	CreateFrameAndMeshList ( pObject );

	// calculate any user matrices from limb offset/rotate/scale data
	for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
	{
		sFrame* pFrame = pObject->ppFrameList [ iFrame ];
		if ( pFrame ) UpdateUserMatrix(pFrame);
	}

	// reset hierarchy and calculate combined frame matrix data
	D3DXMATRIX matrix;
	D3DXMatrixIdentity ( &matrix );
	ResetFrameMatrices ( pObject->pFrame );
	UpdateFrame ( pObject->pFrame, &matrix );

	// success
	return true;
}

DARKSDK_DLL void ComputeBoundValues ( int iPass, D3DXVECTOR3 vecXYZ, D3DXVECTOR3* pvecMin, D3DXVECTOR3* pvecMax, D3DXVECTOR3* pvecCenter, float* pfRadius )
{
	// passes
	// leeadd - 080305 - added pass one as the animated boundbox needs bounds calc
	if ( iPass==0 || iPass==1 )
	{
		// check to see if the vertices are within the bounds and set the appriopriate values
		if ( vecXYZ.x < pvecMin->x ) pvecMin->x = vecXYZ.x;
		if ( vecXYZ.y < pvecMin->y ) pvecMin->y = vecXYZ.y;
		if ( vecXYZ.z < pvecMin->z ) pvecMin->z = vecXYZ.z;
		if ( vecXYZ.x > pvecMax->x ) pvecMax->x = vecXYZ.x;
		if ( vecXYZ.y > pvecMax->y ) pvecMax->y = vecXYZ.y;
		if ( vecXYZ.z > pvecMax->z ) pvecMax->z = vecXYZ.z;
	}
	if ( iPass==0 )
	{
		// add to the centre vector
		*pvecCenter += vecXYZ;
	}
	if ( iPass!=0 )
	{
		// relative to center of sphere
		vecXYZ = vecXYZ - *pvecCenter;

		// get the square length of the vector
		float fDistSq = D3DXVec3LengthSq ( &vecXYZ );

		// see if it's larger than the current radius
		if ( fDistSq > *pfRadius ) *pfRadius = fDistSq;
	}
}

DARKSDK_DLL bool CalculateMeshBounds ( sMesh* pMesh )
{
	// ensure that the pMesh is valid
	SAFE_MEMORY ( pMesh );

	// set the initial min and max vectors to defaults
	pMesh->Collision.vecMin = D3DXVECTOR3 (  1000000.0f,  1000000.0f,  1000000.0f );
	pMesh->Collision.vecMax = D3DXVECTOR3 ( -1000000.0f, -1000000.0f, -1000000.0f );

	// set the initial values of the bounding sphere
	pMesh->Collision.vecCentre = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	pMesh->Collision.fRadius   = 0.0f;

	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// for each mesh go through all of the vertices
	for ( int iPass=0; iPass < 2; iPass++ )
	{
		for ( int iCurrentVertex = 0; iCurrentVertex < ( int ) pMesh->dwVertexCount; iCurrentVertex++ )
		{
			// make sure we have position data in the vertices
			if ( offsetMap.dwZ>0 )
			{
				// get the x, y and z components
				D3DXVECTOR3 vecXYZ;
				vecXYZ.x = *( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) );
				vecXYZ.y = *( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iCurrentVertex ) );
				vecXYZ.z = *( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iCurrentVertex ) );

				// compute bound box, center and radius
				ComputeBoundValues ( iPass, vecXYZ, &pMesh->Collision.vecMin,
													&pMesh->Collision.vecMax,
													&pMesh->Collision.vecCentre,
													&pMesh->Collision.fRadius		);
			}
		}
		if ( iPass==0 )
		{
			// divide the centre radius by the number of vertices and we'll get our centre position
			pMesh->Collision.vecCentre = pMesh->Collision.vecMin + ((pMesh->Collision.vecMax - pMesh->Collision.vecMin)/2.0f); 
		}
		else
		{
			// set the final radius value which is the square root of the current radius
			pMesh->Collision.fRadius = ( float ) sqrt ( pMesh->Collision.fRadius );
		}
	}

	// okay
	return true;
}

DARKSDK_DLL bool CalculateObjectBounds ( int iPass, sMesh* pMesh, D3DXMATRIX* pMatrix, D3DXVECTOR3* pvecMin, D3DXVECTOR3* pvecMax, D3DXVECTOR3* pvecCenter, float* pfRadius )
{
	// ensure that the pMesh is valid
	SAFE_MEMORY ( pMesh );

	// get the offset map for the FVF
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// minimum data for quick mesh test - better system is calculate and store offsetmap in sMesh (fvf or declaration based)
	if ( pMesh->dwFVF==0 )
	{
		offsetMap.dwX = 0;
		offsetMap.dwY = 1;
		offsetMap.dwZ = 2;
		offsetMap.dwSize = pMesh->dwFVFSize/4;
	}

	// for each mesh go through all of the vertices
	for ( int iCurrentVertex = 0; iCurrentVertex < ( int ) pMesh->dwVertexCount; iCurrentVertex++ )
	{
		// make sure we have position data in the vertices
		if ( (offsetMap.dwZ>0) || pMesh->dwFVF==0 )
		{
			// get the x, y and z components
			D3DXVECTOR3 vecXYZ;
			vecXYZ.x = *( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) );
			vecXYZ.y = *( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iCurrentVertex ) );
			vecXYZ.z = *( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iCurrentVertex ) );

			// transform the vector to world coords
			D3DXVec3TransformCoord( &vecXYZ, &vecXYZ, pMatrix );

			// compute bound box, center and radius
			ComputeBoundValues ( iPass, vecXYZ, pvecMin, pvecMax, pvecCenter, pfRadius );
		}
	}

	// okay
	return true;
}

DARKSDK_DLL bool CalculateAllBounds ( sObject* pObject, bool bNotUsed )
{
	// ensure that the object is valid
	SAFE_MEMORY ( pObject );

	// u74b7 - get the largest scale
	float fBiggestScale = max (
		pObject->position.vecScale.x, max (
			pObject->position.vecScale.y,
			pObject->position.vecScale.z)
		);

	// set the initial min and max vectors to defaults
	pObject->collision.vecMin = D3DXVECTOR3 (  1000000.0f,  1000000.0f,  1000000.0f );
	pObject->collision.vecMax = D3DXVECTOR3 ( -1000000.0f, -1000000.0f, -1000000.0f );

	// set the initial values of the bounding sphere
	D3DXVECTOR3 vecRealObjectCenter;
	pObject->collision.vecCentre = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	pObject->collision.fRadius = 0.0f;

	// leeadd - 080305 - array to store bounboxes per frame
	if ( pObject->pAnimationSet )
	{
		// lee - 280306 - u6rc3 - can have animset tag but no frames data
		if ( pObject->pAnimationSet->ulLength>0 )
		{
			// lee - 150206 - u60 - always have at least one slot for bounds vectors
			DWORD dwTotalFrames = pObject->pAnimationSet->ulLength;
			if ( dwTotalFrames==0 ) dwTotalFrames=1;

			// create dynamic boundbox arrays
			SAFE_DELETE(pObject->pAnimationSet->pvecBoundMin);
			pObject->pAnimationSet->pvecBoundMin = new D3DXVECTOR3 [ dwTotalFrames ];
			SAFE_DELETE(pObject->pAnimationSet->pvecBoundMax);
			pObject->pAnimationSet->pvecBoundMax = new D3DXVECTOR3 [ dwTotalFrames ];
			SAFE_DELETE(pObject->pAnimationSet->pvecBoundCenter);
			pObject->pAnimationSet->pvecBoundCenter = new D3DXVECTOR3 [ dwTotalFrames ];
			SAFE_DELETE(pObject->pAnimationSet->pfBoundRadius);
			pObject->pAnimationSet->pfBoundRadius = new float [ dwTotalFrames ];

			// reset boundboxes
			for ( int iKeyframe=0; iKeyframe<(int)dwTotalFrames; iKeyframe+=1 )
			{
				pObject->pAnimationSet->pvecBoundMin [ iKeyframe ] = D3DXVECTOR3 (  1000000.0f,  1000000.0f,  1000000.0f );
				pObject->pAnimationSet->pvecBoundMax [ iKeyframe ] = D3DXVECTOR3 ( -1000000.0f, -1000000.0f, -1000000.0f );
				pObject->pAnimationSet->pvecBoundCenter [ iKeyframe ] = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
				pObject->pAnimationSet->pfBoundRadius [ iKeyframe ] = 0.0f;
			}

			// U74 - 210409 - FPSCV115 - must animate before first pass or 'center' calc wrong (as animation can SCALE vertex data)
			if ( 1 )
			{
				// store anim flag
				bool bStoreAnimFlag = pObject->bAnimPlaying;
				float fStoreFrame = pObject->fAnimFrame;

				// perform animation processing of first keyframe (in case anim scales vertex data)
				for ( int iCurrentFrame = 0; iCurrentFrame < pObject->iFrameCount; iCurrentFrame++ )
				{
					// find frame within object
					sFrame* pFrame = pObject->ppFrameList [ iCurrentFrame ];
					if ( pFrame )
					{
						sMesh* pMesh = pFrame->pMesh;
						if ( pMesh )
						{
							pObject->bAnimPlaying=false;
							pObject->fAnimFrame=0.0f;
							D3DXMATRIX matrix;
							D3DXMatrixIdentity ( &matrix );
							UpdateAllFrameData ( pObject, pObject->fAnimFrame );
							UpdateFrame ( pObject->pFrame, &matrix );
							AnimateBoneMesh ( pObject, pFrame );
						}
					}
				}

				// restore animation flag
				pObject->bAnimPlaying = bStoreAnimFlag;
				pObject->fAnimFrame = fStoreFrame;
			}
		}
	}

	// bound vectir coutn var
	DWORD dwOverallObjBoundVectorCount = 0;

	// run through two passes (center calc, largest radius calc and radius calc)
	for ( int iMainPass = 0; iMainPass < 3; iMainPass++ )
	{
		// run through all of the frames within the object
		for ( int iCurrentFrame = 0; iCurrentFrame < pObject->iFrameCount; iCurrentFrame++ )
		{
			// find frame within object
			sFrame* pFrame = pObject->ppFrameList [ iCurrentFrame ];
			if ( pFrame )
			{
				sMesh* pMesh = pFrame->pMesh;
				if ( pMesh )
				{
					// calculate bounds for mesh (and animate on third pass)
					if ( iMainPass==0 ) CalculateMeshBounds ( pMesh );

					// world matrix (includes any scaling or orienting from frame data)
					// for example DarkMATTER models that have a 0.2 scaling at frame 1
					D3DXMATRIX WorldMatrix = pFrame->matCombined;

					// U74 - 210409 - FPSCV115 - animated vertex data earlier which applied scale so can use an identity
					// matrix here so the pFrameCombined does not interfere with vertex data MIN MAX boundbox
					// combined matrix required by many unanimated models so ODE physics can align box with
					// geometry so if animation is in effect, reset matrix, otherwise leave be, and ensure
					// BoneCount check so only vertexdata animated do not use frame combined but other animating
					// models such as chests which rotate/scale/translate can use combined for scale adjust
					if ( pObject->pAnimationSet )
					{
						if ( pObject->pAnimationSet->ulLength>0 )
							if ( pMesh->dwBoneCount>0 )
								D3DXMatrixIdentity ( &WorldMatrix );
					}
					else
					{
						// 241113 - discovered can have skinweight matrix on objects with no anim data but pBones data!
						// and will throw out proper position of object if not accounted for
						if ( pMesh->pBones )
							D3DXMatrixMultiply ( &WorldMatrix, &pMesh->pBones[0].matTranslation, &WorldMatrix );
					}

					// accumilate mesh data for object bounds calc
					if ( iMainPass==1 )
					{
						// object is animatable or static
						D3DXMATRIX matrix;
						if ( pObject->pAnimationSet )
						{
							// store anim flag
							bool bStoreAnimFlag = pObject->bAnimPlaying;
							float fStoreFrame = pObject->fAnimFrame;

							// calculate from all keyframes of animated mesh
							pObject->bAnimPlaying=true;
							int iLength = pObject->pAnimationSet->ulLength;
							sAnimation* pAnim = pObject->pAnimationSet->pAnimation;

							// keyframe ised to balance bounds of animation with speed code takes
							int iKeyStep = iLength/1000;

							// leefix - 170605 - this was added from a years old suggestion (animating all frames to get true bounds)
							// but it is slow (ie 13 seconds to load a character model in FPSC-V1), so do the quick version
							// BE AWARE that this will mean the keyframe-based bounbox data is not useful!
							if ( g_bFastBoundsCalculation==true )
								iKeyStep = iLength - 1;
							
							// go through all animations to get total bounds
							if ( iKeyStep<1 ) iKeyStep=1;
							for ( int iKeyframe=0; iKeyframe<iLength; iKeyframe+=iKeyStep )
							{
								// U74 - 210409 - FPSCV115 - need combined for all non-bone-animating models
								matrix = pFrame->matCombined;
								if ( pObject->pAnimationSet->ulLength>0 )
									if ( pMesh->dwBoneCount>0 )
										D3DXMatrixIdentity ( &matrix );

								// set frame, animate and use mesh to get largest bounds
								pObject->fAnimFrame=(float)iKeyframe;
								UpdateAllFrameData ( pObject, pObject->fAnimFrame );
								UpdateFrame ( pObject->pFrame, &matrix );
								AnimateBoneMesh ( pObject, pFrame );

								// lee - 040306 - u6rc5 - matrix is just the work matrix for UpdateFrame, use actual frame matrix (as use in other bounds calcs)
								// matrix = pFrame->matCombined; // U74 - taken care of above

								// Calculate object bounds that animate
								if ( pObject->pAnimationSet )
								{
									// Calculate object bounds and store in bounds array
									CalculateObjectBounds ( iMainPass, pMesh, &matrix,
															&pObject->pAnimationSet->pvecBoundMin [ iKeyframe ], &pObject->pAnimationSet->pvecBoundMax [ iKeyframe ],
															&pObject->pAnimationSet->pvecBoundCenter [ iKeyframe ], &pObject->pAnimationSet->pfBoundRadius [ iKeyframe ] );

									// U74 - 210409 - FPSCV115 - store boundbox locally in case animationset bounds data not used
									if ( iKeyframe==0 )
									{
										pObject->collision.vecMin = pObject->pAnimationSet->pvecBoundMin [ iKeyframe ];
										pObject->collision.vecMax = pObject->pAnimationSet->pvecBoundMax [ iKeyframe ];
										pObject->collision.vecCentre = pObject->pAnimationSet->pvecBoundCenter [ iKeyframe ];
										pObject->collision.fRadius = pObject->pAnimationSet->pfBoundRadius [ iKeyframe ];
									}
								}
							}

							// restore mesh from animation parse
							pObject->bAnimPlaying=false;
							pObject->fAnimFrame=0.0f;
							D3DXMatrixIdentity ( &matrix );
							UpdateAllFrameData ( pObject, pObject->fAnimFrame );
							UpdateFrame ( pObject->pFrame, &matrix );
							AnimateBoneMesh ( pObject, pFrame );

							// restore animation flag
							pObject->bAnimPlaying = bStoreAnimFlag;
							pObject->fAnimFrame = fStoreFrame;
						}
						else
						{
							// update static model if has bones
							D3DXMatrixIdentity ( &matrix );
							UpdateAllFrameData ( pObject, pObject->fAnimFrame );
							UpdateFrame ( pObject->pFrame, &matrix );
							AnimateBoneMesh ( pObject, pFrame );

							// leefix - 280403 - calculate bound from non-animated mesh (ie cloned objects, etc)
							CalculateObjectBounds ( iMainPass, pMesh, &WorldMatrix,
													&pObject->collision.vecMin,	&pObject->collision.vecMax,
													&pObject->collision.vecCentre, &pObject->collision.fRadius	);
						}
					}
					else
					{
						// calculate from static mesh
						CalculateObjectBounds ( iMainPass, pMesh, &WorldMatrix,
												&pObject->collision.vecMin,	&pObject->collision.vecMax,
												&pObject->collision.vecCentre, &pObject->collision.fRadius	);
					}

					// increment bound vector count
					dwOverallObjBoundVectorCount+=pMesh->dwVertexCount;
				}
			}
		}
		if ( iMainPass==0 )
		{
			// finalise object center
			pObject->collision.vecCentre = pObject->collision.vecMin + ((pObject->collision.vecMax - pObject->collision.vecMin)/2.0f);
			vecRealObjectCenter = pObject->collision.vecCentre;
		}
		if ( iMainPass==1 )
		{
			// work out largest object radius from object bound box
			pObject->collision.fLargestRadius = ( float ) sqrt ( pObject->collision.fRadius );
			pObject->collision.vecCentre = vecRealObjectCenter;
			pObject->collision.fRadius = 0.0f;

			// lee - 140307 - I think this was added without a date, but it stops scaledradius from being set as it is zero by default
			// and this condition allows the computebounds to be called while keeping scaledradius at zero if it was zero (so the no-culling feature remains viable)
			// so to combat this, we set the fScaledLargestRadius field to one before we call the object create common functions
			// u74b7 - Use the scaled radius
			if ( pObject->collision.fScaledLargestRadius > 0.0f ) pObject->collision.fScaledLargestRadius = pObject->collision.fLargestRadius * fBiggestScale;

			// lee - 060406 - u6rc6 - shadow casters have a larger visual cull radius
			if ( pObject->bCastsAShadow==true )
			{
				// increase largest range to encompass possible shadow
				if ( pObject->collision.fScaledLargestRadius > 0.0f )
				{
					pObject->collision.fScaledLargestRadius = pObject->collision.fLargestRadius;
					pObject->collision.fScaledLargestRadius += 3000.0f;
				}
			}
		}
		if ( iMainPass==2 )
		{
			// work out final object radius from object bound box
			pObject->collision.fRadius = ( float ) sqrt ( pObject->collision.fRadius );
			// u74b7 - Use the scaled radius
			pObject->collision.fScaledRadius = pObject->collision.fRadius * fBiggestScale;
		}
	}

	// leefix - 140306 - u60b3 - of course by reducing the keyframe scan, the array is not filled
	if ( pObject->pAnimationSet )
	{
		// lee - 280306 - u6rc3 - can have animset tag but no frames data
		if ( pObject->pAnimationSet->ulLength>0 )
		{
			// fill the array elements that have not been filled
			for ( int iKeyframe=0; iKeyframe<(int)pObject->pAnimationSet->ulLength; iKeyframe++ )
			{
				if ( pObject->pAnimationSet->pfBoundRadius [ iKeyframe ]==0.0f )
				{
					// fill from master collision shape
					pObject->pAnimationSet->pvecBoundMin [ iKeyframe ] = pObject->collision.vecMin;
					pObject->pAnimationSet->pvecBoundMax [ iKeyframe ] = pObject->collision.vecMax;
					pObject->pAnimationSet->pfBoundRadius [ iKeyframe ] = pObject->collision.fRadius;
					pObject->pAnimationSet->pvecBoundCenter [ iKeyframe ] = pObject->collision.vecCentre;
				}
			}
		}
	}

	// leadd - 080305 - apply boundbox from current frame in animation
	if ( pObject->pAnimationSet )
	{
		// lee - 280306 - u6rc3 - can have animset tag but no frames data
		if ( pObject->pAnimationSet->ulLength>0 )
		{
			if ( pObject->bUpdateOnlyCurrentFrameBounds==false )
			{
				// work out center and radius
				for ( int iKeyframe=0; iKeyframe<(int)pObject->pAnimationSet->ulLength; iKeyframe+=1 )
				{
					pObject->pAnimationSet->pvecBoundCenter [ iKeyframe ] = pObject->pAnimationSet->pvecBoundMin [ iKeyframe ] + ((pObject->pAnimationSet->pvecBoundMax [ iKeyframe ] - pObject->pAnimationSet->pvecBoundMin [ iKeyframe ])/2.0f);
					D3DXVECTOR3 vecXYZ1 = pObject->pAnimationSet->pvecBoundMin [ iKeyframe ] - pObject->pAnimationSet->pvecBoundCenter [ iKeyframe ];
					D3DXVECTOR3 vecXYZ2 = pObject->pAnimationSet->pvecBoundMax [ iKeyframe ] - pObject->pAnimationSet->pvecBoundCenter [ iKeyframe ];
					float fDistSq1 = D3DXVec3Length ( &vecXYZ1 );
					float fDistSq2 = D3DXVec3Length ( &vecXYZ2 );
					if ( fDistSq2>fDistSq1 ) fDistSq1=fDistSq2;
					pObject->pAnimationSet->pfBoundRadius [ iKeyframe ] = fDistSq1;
				}

				// object bounds updated, and copied to first mesh collision bounds (for visual hack)
				pObject->collision.vecMin = pObject->pAnimationSet->pvecBoundMin [ (int)pObject->fAnimFrame ];
				pObject->collision.vecMax = pObject->pAnimationSet->pvecBoundMax [ (int)pObject->fAnimFrame ];
				pObject->collision.vecCentre = pObject->pAnimationSet->pvecBoundCenter [ (int)pObject->fAnimFrame ];
				pObject->collision.fRadius = pObject->pAnimationSet->pfBoundRadius [ (int)pObject->fAnimFrame ];
				pObject->ppMeshList [ 0 ]->Collision.vecMin = pObject->collision.vecMin;
				pObject->ppMeshList [ 0 ]->Collision.vecMax = pObject->collision.vecMax;
				pObject->ppMeshList [ 0 ]->Collision.vecCentre = pObject->collision.vecCentre;
				pObject->ppMeshList [ 0 ]->Collision.fRadius = pObject->collision.fRadius;
			}
		}
	}

	// okay
	return true;
}

DARKSDK_DLL bool SetupMeshData ( sMesh* pMesh, DWORD dwVertexCount, DWORD dwIndexCount )
{
	// if index size exceeds 16bit, cannot allow index buffer
	if ( dwIndexCount > 0 )
		if ( dwIndexCount > 0x0000FFFF )
			return false;

	// ensure the mesh is valid
	SAFE_MEMORY ( pMesh );

	// ensure we free old data
	// 281114 - changed to SAFE_DELETE_ARRAY
	SAFE_DELETE_ARRAY(pMesh->pVertexData);
	SAFE_DELETE_ARRAY(pMesh->pIndices);

	// setup mesh properties
	pMesh->dwVertexCount	= dwVertexCount;									// vertex count assigned
	pMesh->pVertexData		= new BYTE [ pMesh->dwFVFSize * dwVertexCount ];	// allocate vertex memory

	// create new index mesh data
	if ( dwIndexCount>0 )
	{
		pMesh->dwIndexCount		= dwIndexCount;
		pMesh->pIndices			= new WORD [ pMesh->dwIndexCount ];
	}
	else
	{
		pMesh->dwIndexCount		= 0;
		pMesh->pIndices			= NULL;
	}

	// check the memory was allocated correctly
	SAFE_MEMORY ( pMesh->pVertexData );

	// okay
	return true;
}

DARKSDK_DLL bool SetupMeshDeclarationData ( sMesh* pMesh, CONST D3DVERTEXELEMENT9* pDeclaration, DWORD dwVertexSize, DWORD dwVertexCount, DWORD dwIndexCount )
{
	// create a new vertex declaration object
	LPDIRECT3DVERTEXDECLARATION9 pNewVertexDec;	
	if ( FAILED ( m_pD3D->CreateVertexDeclaration ( pDeclaration, &pNewVertexDec ) ) )
		return false;

	// setup mesh properties
	pMesh->dwFVF			= 0;
	pMesh->dwFVFSize		= dwVertexSize;

	// store declaration for later reversal
	memcpy ( pMesh->pVertexDeclaration, pDeclaration, sizeof(pMesh->pVertexDeclaration) );

	// free any previous association with vertex dec handle
	SAFE_RELEASE ( pMesh->pVertexDec );
	pMesh->pVertexDec = pNewVertexDec;

	// now setup the data
	if ( !SetupMeshData ( pMesh, dwVertexCount, dwIndexCount ) )
		return false;

	// complete
	return true;
}

DARKSDK_DLL bool SetupMeshFVFData ( sMesh* pMesh, DWORD dwFVF, DWORD dwVertexCount, DWORD dwIndexCount )
{
	// set up mesh properties for the given FVF
	pMesh->dwFVF = dwFVF;
	sOffsetMap	offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// setup mesh properties
	pMesh->dwFVFSize = offsetMap.dwByteSize;

	// now setup the data
	if ( !SetupMeshData ( pMesh, dwVertexCount, dwIndexCount ) )
		return false;

	// complete
	return true;
}

DARKSDK_DLL bool SetupFrustum ( float fZDistance )
{
	// setup the planes for the viewing frustum

	// variable declarations
	D3DXMATRIX	Matrix;
	float		ZMin,
				Q;

	// check d3d is ok
	SAFE_MEMORY ( m_pD3D );
	
	// get the projection matrix
	D3DXMATRIX matProj;
	m_pD3D->GetTransform ( D3DTS_PROJECTION, &matProj );
	
	// get the view matrix
	D3DXMATRIX matView;
	m_pD3D->GetTransform ( D3DTS_VIEW, &matView );

	if ( fZDistance != 0.0f )
	{
		// calculate new projection matrix based on distance provided
		ZMin        = -matProj._43 / matProj._33;
		Q           = fZDistance / ( fZDistance - ZMin );
		matProj._33 = Q;
		matProj._43 = -Q * ZMin;
	}

	// multiply with the projection matrix
	D3DXMatrixMultiply ( &Matrix, &matView, &matProj );

	// and now calculate the planes

	// near plane
	g_Planes [ 0 ][ 0 ].a = Matrix._14 + Matrix._13;
	g_Planes [ 0 ][ 0 ].b = Matrix._24 + Matrix._23;
	g_Planes [ 0 ][ 0 ].c = Matrix._34 + Matrix._33;
	g_Planes [ 0 ][ 0 ].d = Matrix._44 + Matrix._43;
	D3DXPlaneNormalize ( &g_Planes [ 0 ][ 0 ], &g_Planes [ 0 ][ 0 ] );

	// far plane
	g_Planes [ 0 ][ 1 ].a = Matrix._14 - Matrix._13;
	g_Planes [ 0 ][ 1 ].b = Matrix._24 - Matrix._23;
	g_Planes [ 0 ][ 1 ].c = Matrix._34 - Matrix._33;
	g_Planes [ 0 ][ 1 ].d = Matrix._44 - Matrix._43;
	D3DXPlaneNormalize ( &g_Planes [ 0 ][ 1 ], &g_Planes [ 0 ][ 1 ] );

	// left plane
	g_Planes [ 0 ][ 2 ].a = Matrix._14 + Matrix._11;
	g_Planes [ 0 ][ 2 ].b = Matrix._24 + Matrix._21;
	g_Planes [ 0 ][ 2 ].c = Matrix._34 + Matrix._31;
	g_Planes [ 0 ][ 2 ].d = Matrix._44 + Matrix._41;
	D3DXPlaneNormalize ( &g_Planes [ 0 ][ 2 ], &g_Planes [ 0 ][ 2 ] );

	// right plane
	g_Planes [ 0 ][ 3 ].a = Matrix._14 - Matrix._11;
	g_Planes [ 0 ][ 3 ].b = Matrix._24 - Matrix._21;
	g_Planes [ 0 ][ 3 ].c = Matrix._34 - Matrix._31;
	g_Planes [ 0 ][ 3 ].d = Matrix._44 - Matrix._41;
	D3DXPlaneNormalize ( &g_Planes [ 0 ][ 3 ], &g_Planes [ 0 ][ 3 ] );

	// top plane
	g_Planes [ 0 ][ 4 ].a = Matrix._14 - Matrix._12;
	g_Planes [ 0 ][ 4 ].b = Matrix._24 - Matrix._22;
	g_Planes [ 0 ][ 4 ].c = Matrix._34 - Matrix._32;
	g_Planes [ 0 ][ 4 ].d = Matrix._44 - Matrix._42;
	D3DXPlaneNormalize ( &g_Planes [ 0 ][ 4 ], &g_Planes [ 0 ][ 4 ] );

	// bottom plane
	g_Planes [ 0 ][ 5 ].a = Matrix._14 + Matrix._12;
	g_Planes [ 0 ][ 5 ].b = Matrix._24 + Matrix._22;
	g_Planes [ 0 ][ 5 ].c = Matrix._34 + Matrix._32;
	g_Planes [ 0 ][ 5 ].d = Matrix._44 + Matrix._42;
	D3DXPlaneNormalize ( &g_Planes [ 0 ][ 5 ], &g_Planes [ 0 ][ 5 ] );

	// complete
	return true;
}

DARKSDK_DLL bool SetupPortalFrustum ( DWORD dwFrustumCount, D3DXVECTOR3* pvecStart, D3DXVECTOR3* pvecA, D3DXVECTOR3* pvecB, D3DXVECTOR3* pvecC, D3DXVECTOR3* pvecD, bool bFrustrumZeroIsValid )
{
	// near plane
	D3DXPlaneFromPoints ( &g_Planes [ dwFrustumCount ][ 0 ], pvecA, pvecB, pvecC );
	g_PlaneVector [ dwFrustumCount ][ 0 ] = *pvecA;

	// far plane
	g_Planes [ dwFrustumCount ][ 1 ] = g_Planes [ 0 ][ 1 ];

	// left plane
	D3DXPlaneFromPoints ( &g_Planes [ dwFrustumCount ][ 2 ], pvecStart, pvecB, pvecC );
	g_PlaneVector [ dwFrustumCount ][ 2 ] = *pvecB;

	// right plane
	D3DXPlaneFromPoints ( &g_Planes [ dwFrustumCount ][ 3 ], pvecStart, pvecD, pvecA );
	g_PlaneVector [ dwFrustumCount ][ 3 ] = *pvecD;

	// top plane
	D3DXPlaneFromPoints ( &g_Planes [ dwFrustumCount ][ 4 ], pvecStart, pvecA, pvecB );
	g_PlaneVector [ dwFrustumCount ][ 4 ] = *pvecA;

	// bottom plane
	D3DXPlaneFromPoints ( &g_Planes [ dwFrustumCount ][ 5 ], pvecStart, pvecC, pvecD );
	g_PlaneVector [ dwFrustumCount ][ 5 ] = *pvecC;

	// make sure child is clipped by parent frustrum
	if ( (dwFrustumCount > 0 && bFrustrumZeroIsValid) || dwFrustumCount > 1 )
	{
		for ( int iP=0; iP<NUM_CULLPLANES; iP++ )
		{
			// not back plane
			if ( iP!=1 )
			{
				// if vector of child is outside parent plane, must clip child
				D3DXVECTOR3 vecChildVector = g_PlaneVector [ dwFrustumCount ][ iP ];
				float fChildOutside = D3DXPlaneDotCoord ( &g_Planes [ dwFrustumCount-1 ][ iP ], &vecChildVector );
				if ( (int)fChildOutside < 0.0f )
				{
					// child is outside parent frustrum at this plane, adjust child plane
					g_Planes [ dwFrustumCount ][ iP ] = g_Planes [ dwFrustumCount-1 ][ iP ];
				}
			}
		}
	}

	// complete
	return true;
}

DARKSDK_DLL bool SetupCastFrustum ( DWORD dwFrustumCount, D3DXVECTOR3* pvecStart, D3DXVECTOR3* pvecFinish )
{
	// useful to create a frustrum that immitates a ray cast for finding nodes from a line

	// complete
	return true;
}

DARKSDK_DLL bool CheckPoint ( float fX, float fY, float fZ )
{
	// make sure point is in frustum
	for ( int iPlaneIndex = 0; iPlaneIndex < NUM_CULLPLANES; iPlaneIndex++ ) 
	{
		if ( D3DXPlaneDotCoord ( &g_Planes [ 0 ][ iPlaneIndex ], &D3DXVECTOR3 ( fX, fY, fZ ) ) < 0.0f )
			return false;
	}
	return true;
}

DARKSDK_DLL bool CheckPoint ( D3DXPLANE* pPlanes, D3DXVECTOR3* pvecPoint )
{
	// is point in frustum?
//	for ( int iPlaneIndex = 0; iPlaneIndex < NUM_CULLPLANES; iPlaneIndex++ ) 
	for ( int iPlaneIndex = 2; iPlaneIndex < NUM_CULLPLANES; iPlaneIndex++ ) 
		if ( D3DXPlaneDotCoord ( &pPlanes [ iPlaneIndex ], pvecPoint ) < 0.0f )
			return false;

	// yes
	return true;
}

DARKSDK_DLL bool CheckCube ( float fX, float fY, float fZ, float fSize )
{
	// this does not always work when using very long ranges or small plane data
	for ( int iPlaneIndex = 0; iPlaneIndex < NUM_CULLPLANES; iPlaneIndex++ )
	{
		if ( D3DXPlaneDotCoord ( &g_Planes [ 0 ][ iPlaneIndex ], &D3DXVECTOR3 ( fX - fSize, fY - fSize, fZ - fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &g_Planes [ 0 ][ iPlaneIndex ], &D3DXVECTOR3 ( fX + fSize, fY - fSize, fZ - fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &g_Planes [ 0 ][ iPlaneIndex ], &D3DXVECTOR3 ( fX - fSize, fY + fSize, fZ - fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &g_Planes [ 0 ][ iPlaneIndex ], &D3DXVECTOR3 ( fX + fSize, fY + fSize, fZ - fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &g_Planes [ 0 ][ iPlaneIndex ], &D3DXVECTOR3 ( fX - fSize, fY - fSize, fZ + fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &g_Planes [ 0 ][ iPlaneIndex ], &D3DXVECTOR3 ( fX + fSize, fY - fSize, fZ + fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &g_Planes [ 0 ][ iPlaneIndex ], &D3DXVECTOR3 ( fX - fSize, fY + fSize, fZ + fSize ) ) >= 0.0f )
			continue;

		if ( D3DXPlaneDotCoord ( &g_Planes [ 0 ][ iPlaneIndex ], &D3DXVECTOR3 ( fX + fSize, fY + fSize, fZ + fSize ) ) >= 0.0f )
			continue;

		return false;
	}
	return true;
}

DARKSDK_DLL bool CheckRectangleEx ( DWORD iFrustumIndex, float fX, float fY, float fZ, float fXSize, float fYSize, float fZSize )
{
	bool bBoxFoundInsideOneOfTheFrustrums=false;
	bool bBoxOutsideFrustrum=false;
	for ( int iPlaneIndex = 0; iPlaneIndex < NUM_CULLPLANES; iPlaneIndex++ )
	{
		// if ALL eight points are outside the plane, it is not inside
		int iCountPointsOutsidePlane=0;

		// FPGC - 180909 - disregard near plane clip on frustrum zero checks (player camera can get nose right upto a portal)
		// if ( iFrustumIndex!=0 ) if ( D3DXPlaneDotCoord ( &g_Planes [ iFrustumIndex ][ iPlaneIndex ], &D3DXVECTOR3 ( fX - fXSize, fY - fYSize, fZ - fZSize ) ) < -0.01f ) iCountPointsOutsidePlane++;
		// 18113 - the above line would NEVER reject a boundbox for camera zero frustum (not intended behaviour!)
		if ( D3DXPlaneDotCoord ( &g_Planes [ iFrustumIndex ][ iPlaneIndex ], &D3DXVECTOR3 ( fX - fXSize, fY - fYSize, fZ - fZSize ) ) < -0.01f ) iCountPointsOutsidePlane++;
		if ( D3DXPlaneDotCoord ( &g_Planes [ iFrustumIndex ][ iPlaneIndex ], &D3DXVECTOR3 ( fX + fXSize, fY - fYSize, fZ - fZSize ) ) < -0.01f ) iCountPointsOutsidePlane++;
		if ( D3DXPlaneDotCoord ( &g_Planes [ iFrustumIndex ][ iPlaneIndex ], &D3DXVECTOR3 ( fX - fXSize, fY + fYSize, fZ - fZSize ) ) < -0.01f ) iCountPointsOutsidePlane++;
		if ( D3DXPlaneDotCoord ( &g_Planes [ iFrustumIndex ][ iPlaneIndex ], &D3DXVECTOR3 ( fX + fXSize, fY + fYSize, fZ - fZSize ) ) < -0.01f ) iCountPointsOutsidePlane++;
		if ( D3DXPlaneDotCoord ( &g_Planes [ iFrustumIndex ][ iPlaneIndex ], &D3DXVECTOR3 ( fX - fXSize, fY - fYSize, fZ + fZSize ) ) < -0.01f ) iCountPointsOutsidePlane++;
		if ( D3DXPlaneDotCoord ( &g_Planes [ iFrustumIndex ][ iPlaneIndex ], &D3DXVECTOR3 ( fX + fXSize, fY - fYSize, fZ + fZSize ) ) < -0.01f ) iCountPointsOutsidePlane++;
		if ( D3DXPlaneDotCoord ( &g_Planes [ iFrustumIndex ][ iPlaneIndex ], &D3DXVECTOR3 ( fX - fXSize, fY + fYSize, fZ + fZSize ) ) < -0.01f ) iCountPointsOutsidePlane++;
		if ( D3DXPlaneDotCoord ( &g_Planes [ iFrustumIndex ][ iPlaneIndex ], &D3DXVECTOR3 ( fX + fXSize, fY + fYSize, fZ + fZSize ) ) < -0.01f ) iCountPointsOutsidePlane++;
		if ( iCountPointsOutsidePlane==8 ) 
		{
			bBoxOutsideFrustrum=true;
			break;
		}
	}
	if ( bBoxOutsideFrustrum==false )
	{
		// box was inside one of the frustrums
		bBoxFoundInsideOneOfTheFrustrums=true;
	}

	// compare if box was outside ALL frustrums
	if ( bBoxFoundInsideOneOfTheFrustrums )
	{
		// at least one corner striding one of the frustrums
		return true;
	}
	else
	{
		// all corners outside all frustrums
		return false;
	}
}

DARKSDK_DLL bool CheckRectangle ( float fX, float fY, float fZ, float fXSize, float fYSize, float fZSize )
{
	return CheckRectangleEx ( 0, fX, fY, fZ, fXSize, fYSize, fZSize );
}

DARKSDK_DLL bool CheckSphere ( float fX, float fY, float fZ, float fRadius )
{
	// make sure radius is in frustum
	fRadius *= 1.25f; // leefix - 190307 - added an epsilon so object does not disappear too early
	for ( int iPlaneIndex = 0; iPlaneIndex < NUM_CULLPLANES; iPlaneIndex++ )
	{
		if ( D3DXPlaneDotCoord ( &g_Planes [ 0 ][ iPlaneIndex ], &D3DXVECTOR3 ( fX, fY, fZ ) ) < -fRadius )
			return false;
	}

	return true;
}

DARKSDK_DLL bool CheckSphere ( DWORD dwFrustumMax, float fX, float fY, float fZ, float fRadius )
{
	// make sure radius is in frustum
	for ( int iFrustumIndex = 0; iFrustumIndex <= (int)dwFrustumMax; iFrustumIndex++ )
	{	
		for ( int iPlaneIndex = 0; iPlaneIndex < NUM_CULLPLANES; iPlaneIndex++ )
		{
			if ( D3DXPlaneDotCoord ( &g_Planes [ iFrustumIndex ][ iPlaneIndex ], &D3DXVECTOR3 ( fX, fY, fZ ) ) < -fRadius )
				return false;
		}
	}
	return true;
}

DARKSDK_DLL bool CheckPolygon ( D3DXPLANE* pPlanes, D3DXVECTOR3* pvec0, D3DXVECTOR3* pvec1, D3DXVECTOR3* pvec2 )
{
	// check all six planes of frustrum
	bool bBoxOutsideFrustrum=false;
	for ( int iPlaneIndex = 0; iPlaneIndex < NUM_CULLPLANES; iPlaneIndex++ )
	{
		// if polygon completely outside one of the planes, absolutely not in frustrum!
		int iCountPointsOutsidePlane=0;
		if ( D3DXPlaneDotCoord ( &pPlanes [ iPlaneIndex ], pvec0 ) < -0.01f ) iCountPointsOutsidePlane++;
		if ( D3DXPlaneDotCoord ( &pPlanes [ iPlaneIndex ], pvec1 ) < -0.01f ) iCountPointsOutsidePlane++;
		if ( D3DXPlaneDotCoord ( &pPlanes [ iPlaneIndex ], pvec2 ) < -0.01f ) iCountPointsOutsidePlane++;
		if ( iCountPointsOutsidePlane==3 ) 
		{
			bBoxOutsideFrustrum=true;
			break;
		}
	}
	if ( bBoxOutsideFrustrum )
		return false;
	else
		return true;
}

DARKSDK_DLL bool QuickSortArray ( int* array, int low, int high )
{
	// this function takes an array of any size
	// and will resort it into a correct order
	// e.g. sending the function a list of 4,2,3,1
	// will result in a list of 1,2,3,4
	// the low and high values are used to specify
	// the start and end point for the search

	if ( !array )
		return false;

	// initialize pointers
	int top    = low;
	int	bottom = high - 1;
	int part_index;
	int part_value;

	// do nothing if low >= high
	if ( low < high )
	{
		// check if elements are sequential
		if ( high == ( low + 1 ) )
		{
			if ( array [ low ] > array [ high ] )
				SwapInts ( array, high, low );
		}
		else
		{
			// choose a partition element and swap 
			// it with the last value in the array
			part_index = ( int ) ( ( low + high ) / 2 );
			part_value = array [ part_index ];

			SwapInts ( array, high, part_index );
		
			do
			{
				// increment the top pointer
				while ( ( array [ top ] <= part_value ) && ( top <= bottom ) )
					top++;
				
				// decrement the bottom pointer
				while ( ( array [ bottom ] > part_value ) && ( top <= bottom ) )
					bottom--;
				
				// swap elements if pointers have not met
				if ( top < bottom )
					SwapInts ( array, top, bottom );

			} while ( top < bottom );

			// put the partition element back where it belongs
			SwapInts ( array, top, high );

			// recursive calls
			QuickSortArray ( array, low,     top - 1 );
			QuickSortArray ( array, top + 1, high    );
		}
	}

	return true;
}

// swaps array n1 with array n2
DARKSDK_DLL bool SwapInts ( int* array, int n1, int n2 )
{
	if ( !array )
		return false;

	int temp;

	temp         = array [ n1 ];
	array [ n1 ] = array [ n2 ];
	array [ n2 ] = temp;

	return true;
}

DARKSDK_DLL bool GetFVFValueOffsetMap ( DWORD dwFVF, sOffsetMap* psOffsetMap )
{
	SAFE_MEMORY ( psOffsetMap );

	memset ( psOffsetMap, 0, sizeof ( sOffsetMap ) );

	int iOffset   = 0;
	int iPosition = 0;

	DWORD dwFVFSize = D3DXGetFVFVertexSize ( dwFVF );

	if ( dwFVF & D3DFVF_XYZ )
	{
		psOffsetMap->dwX         = iOffset + 0;
		psOffsetMap->dwY         = iOffset + 1;
		psOffsetMap->dwZ         = iOffset + 2;
		iOffset += 3;
	}

	if ( dwFVF & D3DFVF_XYZRHW )
	{
		psOffsetMap->dwRWH = iOffset + 0;
		iOffset += 1;
	}

	if ( dwFVF & D3DFVF_NORMAL )
	{
		psOffsetMap->dwNX        = iOffset + 0;
		psOffsetMap->dwNY        = iOffset + 1;
		psOffsetMap->dwNZ        = iOffset + 2;
		iOffset += 3;
	}

	if ( dwFVF & D3DFVF_PSIZE )
	{
		psOffsetMap->dwPointSize = iOffset + 0;
		iOffset += 1;
	}

	if ( dwFVF & D3DFVF_DIFFUSE )
	{
		psOffsetMap->dwDiffuse   = iOffset + 0;
		iOffset += 1;
	}

	if ( dwFVF & D3DFVF_SPECULAR )
	{
		psOffsetMap->dwSpecular   = iOffset + 0;
		iOffset += 1;
	}

	DWORD dwTexCount = 0;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX1 ) dwTexCount=1;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX2 ) dwTexCount=2;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX3 ) dwTexCount=3;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX4 ) dwTexCount=4;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX5 ) dwTexCount=5;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX6 ) dwTexCount=6;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX7 ) dwTexCount=7;
	if ( (dwFVF & D3DFVF_TEXCOUNT_MASK) == D3DFVF_TEX8 ) dwTexCount=8;
	for ( DWORD dwTexCoordSet=0; dwTexCoordSet<dwTexCount; dwTexCoordSet++ )
	{
		DWORD dwTexCoord = dwFVF & D3DFVF_TEXCOORDSIZE1(dwTexCoordSet);
		if ( dwTexCoord==(DWORD)D3DFVF_TEXCOORDSIZE1(dwTexCoordSet) )
		{
			//psOffsetMap->dwTU[0] = iOffset + 0;
			psOffsetMap->dwTU[dwTexCoordSet] = iOffset + 0;
			iOffset += 1;
		}
		if ( dwTexCoord==(DWORD)D3DFVF_TEXCOORDSIZE2(dwTexCoordSet) )
		{
			//psOffsetMap->dwTU[0] = iOffset + 0;
			//psOffsetMap->dwTV[0] = iOffset + 1;
			psOffsetMap->dwTU[dwTexCoordSet] = iOffset + 0;
			psOffsetMap->dwTV[dwTexCoordSet] = iOffset + 1;
			iOffset += 2;
		}
		if ( dwTexCoord==(DWORD)D3DFVF_TEXCOORDSIZE3(dwTexCoordSet) )
		{
			//psOffsetMap->dwTU[0] = iOffset + 0;
			//psOffsetMap->dwTV[0] = iOffset + 1;
			//psOffsetMap->dwTZ[0] = iOffset + 2;
			psOffsetMap->dwTU[dwTexCoordSet] = iOffset + 0;
			psOffsetMap->dwTV[dwTexCoordSet] = iOffset + 1;
			psOffsetMap->dwTZ[dwTexCoordSet] = iOffset + 2;
			iOffset += 3;
		}
		if ( dwTexCoord==(DWORD)D3DFVF_TEXCOORDSIZE4(dwTexCoordSet) )
		{
			//psOffsetMap->dwTU[0] = iOffset + 0;
			//psOffsetMap->dwTV[0] = iOffset + 1;
			//psOffsetMap->dwTZ[0] = iOffset + 2;
			//psOffsetMap->dwTW[0] = iOffset + 3;
			psOffsetMap->dwTU[dwTexCoordSet] = iOffset + 0;
			psOffsetMap->dwTV[dwTexCoordSet] = iOffset + 1;
			psOffsetMap->dwTZ[dwTexCoordSet] = iOffset + 2;
			psOffsetMap->dwTW[dwTexCoordSet] = iOffset + 3;
			iOffset += 4;
		}
	}
	
	// calculate byte offset
	psOffsetMap->dwByteSize = sizeof ( DWORD ) * iOffset;

	// store number of offsets
	psOffsetMap->dwSize = iOffset;

	// check if matches byte size of actual FVF
	if ( dwFVFSize != psOffsetMap->dwByteSize )
	{
		// Offsets not being calculated correctly!
		return false;
	}

	// complete
	return true;
}

DARKSDK_DLL bool GetFVFOffsetMap ( sMesh* pMesh, sOffsetMap* psOffsetMap )
{
	// clear to begin with
	memset ( psOffsetMap, 0, sizeof(sOffsetMap) );

	// FVF or declaration
	if ( pMesh->dwFVF==0 )
	{
		// Define end declaration token
		D3DVERTEXELEMENT9 End = D3DDECL_END();

		// Find Offsets
		for( int iElem=0; pMesh->pVertexDeclaration[iElem].Stream != End.Stream; iElem++ )
		{   
			int iIndex = pMesh->pVertexDeclaration[iElem].UsageIndex;
			int iElementOffset = pMesh->pVertexDeclaration[iElem].Offset / sizeof(DWORD);
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_POSITION )
			{
				psOffsetMap->dwX = iElementOffset + 0;
				psOffsetMap->dwY = iElementOffset + 1;
				psOffsetMap->dwZ = iElementOffset + 2;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_POSITIONT )
			{
				psOffsetMap->dwX = iElementOffset + 0;
				psOffsetMap->dwY = iElementOffset + 1;
				psOffsetMap->dwZ = iElementOffset + 2;
				psOffsetMap->dwRWH = iElementOffset + 3;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_PSIZE )
			{
				psOffsetMap->dwPointSize = iElementOffset + 0;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_NORMAL )
			{
				psOffsetMap->dwNX = iElementOffset + 0;
				psOffsetMap->dwNY = iElementOffset + 1;
				psOffsetMap->dwNZ = iElementOffset + 2;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_COLOR && iIndex==0 )
			{
				psOffsetMap->dwDiffuse = iElementOffset + 0;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_COLOR && iIndex==1 )
			{
				psOffsetMap->dwSpecular = iElementOffset + 0;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_TEXCOORD )
			{
				psOffsetMap->dwTU[iIndex] = iElementOffset + 0;
				if ( pMesh->pVertexDeclaration[iElem].Type==D3DDECLTYPE_FLOAT2)
				{
					psOffsetMap->dwTV[iIndex] = iElementOffset + 1;
				}
				if ( pMesh->pVertexDeclaration[iElem].Type==D3DDECLTYPE_FLOAT3)
				{
					psOffsetMap->dwTV[iIndex] = iElementOffset + 1;
					psOffsetMap->dwTZ[iIndex] = iElementOffset + 2;
				}
				if ( pMesh->pVertexDeclaration[iElem].Type==D3DDECLTYPE_FLOAT4)
				{
					psOffsetMap->dwTV[iIndex] = iElementOffset + 1;
					psOffsetMap->dwTZ[iIndex] = iElementOffset + 2;
					psOffsetMap->dwTW[iIndex] = iElementOffset + 3;
				}
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_TANGENT )
			{
				psOffsetMap->dwTU[1] = iElementOffset + 0;
				psOffsetMap->dwTV[1] = iElementOffset + 1;
				psOffsetMap->dwTZ[1] = iElementOffset + 2;
				psOffsetMap->dwTW[1] = iElementOffset + 3;
			}
			if( pMesh->pVertexDeclaration[iElem].Usage == D3DDECLUSAGE_BINORMAL )
			{
				psOffsetMap->dwTU[2] = iElementOffset + 0;
				psOffsetMap->dwTV[2] = iElementOffset + 1;
				psOffsetMap->dwTZ[2] = iElementOffset + 2;
				psOffsetMap->dwTW[2] = iElementOffset + 3;
			}
		}

		// calculate byte offset
		psOffsetMap->dwByteSize = pMesh->dwFVFSize;

		// store number of offsets
		psOffsetMap->dwSize = pMesh->dwFVFSize/sizeof(DWORD);

		// complete
		return true;
	}
	else
	{
		return GetFVFValueOffsetMap ( pMesh->dwFVF, psOffsetMap );
	}
}

DARKSDK_DLL bool CreateVertexShaderFromFVF ( DWORD dwFVF, DWORD* pdwShader )
{
	// create the vertex shader handle

	// check all of the memory
	SAFE_MEMORY ( m_pD3D );

	// all okay
	return true;
}

DARKSDK_DLL void DBOCalculateLoaderTempFolder ( void )
{
	if ( g_pGlob )
	{
		// Add the DBPDATA folder to temp
		strcpy ( g_WindowsTempDirectory, g_pGlob->pEXEUnpackDirectory );
		if ( g_WindowsTempDirectory [ strlen(g_WindowsTempDirectory)-1 ]!='\\' )
 			strcat ( g_WindowsTempDirectory, "\\" );
	}
	else
	{
		// Current directory
		char CurrentDirectory[_MAX_PATH];
		_getcwd(CurrentDirectory, _MAX_PATH);

		// Find temporary directory (C:\WINDOWS\Temp)
		GetTempPath(_MAX_PATH, g_WindowsTempDirectory);
		if(_strcmpi(g_WindowsTempDirectory, CurrentDirectory)==NULL)
		{
			// Pre-XP Temp Folder
			GetWindowsDirectory(g_WindowsTempDirectory, _MAX_PATH);
			strcat(g_WindowsTempDirectory, "\\temp\\");
		}

		// Create DBPDATA folder in any event
		_chdir(g_WindowsTempDirectory);
		_mkdir("dbpdata");
		_chdir(CurrentDirectory);

		// Add the DBPDATA folder to temp
		strcat ( g_WindowsTempDirectory, "\\dbpdata\\" );
	}
}

DARKSDK_DLL bool DBOFileExist ( LPSTR pFilename )
{
	HANDLE hfile = CreateFile ( pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hfile != INVALID_HANDLE_VALUE )
	{
		CloseHandle ( hfile );
		return true;
	}
	return false;
}

DARKSDK_DLL bool ConvertToDBOBlock ( LPSTR pFilename, LPSTR pExtension, DWORD* ppDBOBlock, DWORD* pdwBlockSize )
{
	// result var
	bool bResult=false;

	// clear return block
	*ppDBOBlock = NULL;
	*pdwBlockSize = 0;

	// Build DLL name
	char pDLLName[256];
	strcpy ( pDLLName, "conv" );
	strcat ( pDLLName, pExtension );
	strcat ( pDLLName, ".dll" );
	_strlwr ( pDLLName );

	#ifdef DARKSDK_COMPILE
	{
		DWORD pTempBlock=0, dwTempSize=0;
		bool bOk = false;

		// if ( pPassCoreFn ) pPassCoreFn ( g_pGlob );

		if ( strnicmp ( pExtension, "x", 1 ) == 0 )
			bOk = ConvertX ( pFilename, &pTempBlock, &dwTempSize );

		if ( strnicmp ( pExtension, "md2", 3 ) == 0 )
			bOk = ConvertMD2 ( pFilename, &pTempBlock, &dwTempSize );

		if ( strnicmp ( pExtension, "md3", 3 ) == 0 )
			bOk = ConvertMD3 ( pFilename, &pTempBlock, &dwTempSize );

		if ( strnicmp ( pExtension, "3ds", 3 ) == 0 )
		{
			PassCoreData3DS ( g_pGlob );
			bOk = Convert3DS ( pFilename, &pTempBlock, &dwTempSize );
		}

		if ( strnicmp ( pExtension, "mdl", 3 ) == 0 )
		{
			PassCoreDataMDL ( g_pGlob );
			bOk = ConvertMDL ( pFilename, &pTempBlock, &dwTempSize );
		}

		if ( bOk )
		{
			// Create local memory for block
			*pdwBlockSize = dwTempSize;
			*ppDBOBlock = (DWORD) new char [ dwTempSize ];
			memcpy ( (LPSTR)*ppDBOBlock, (LPSTR)pTempBlock, dwTempSize );

			// success
			bResult=true;
		}

		if ( strnicmp ( pExtension, "x", 1 ) == 0 )
			FreeX( (LPSTR)pTempBlock );

		if ( strnicmp ( pExtension, "md2", 3 ) == 0 )
			FreeMD2( (LPSTR)pTempBlock );

		if ( strnicmp ( pExtension, "md3", 3 ) == 0 )
			FreeMD3( (LPSTR)pTempBlock );

		if ( strnicmp ( pExtension, "3ds", 3 ) == 0 )
			Free3DS( (LPSTR)pTempBlock );

		if ( strnicmp ( pExtension, "mdl", 3 ) == 0 )
			FreeMDL( (LPSTR)pTempBlock );

		return bResult;
	}
	#endif

	// load DLL
	HMODULE hDLLModule = LoadLibrary ( pDLLName );

	// check if DLL exists
	if ( hDLLModule )
	{
		// Get Convert Functions
		typedef void ( *VOIDVOID ) ( void );
		typedef void ( *PASSCORE ) ( LPVOID );
		typedef void ( *FREEFUNCTION ) ( LPSTR );
		typedef bool ( *CONVERTFUNCTION ) ( LPSTR, DWORD*, DWORD* );
		PASSCORE pPassCoreFn = (PASSCORE) GetProcAddress ( hDLLModule, "PassCoreData" );
		VOIDVOID pSetLegacyModeOn = (VOIDVOID) GetProcAddress ( hDLLModule, "SetLegacyModeOn" );
		VOIDVOID pSetLegacyModeOff = (VOIDVOID) GetProcAddress ( hDLLModule, "SetLegacyModeOff" );
		CONVERTFUNCTION pConvertFn = (CONVERTFUNCTION) GetProcAddress ( hDLLModule, "Convert" );
		FREEFUNCTION pFree = (FREEFUNCTION) GetProcAddress ( hDLLModule, "Free" );

		// Pass Core Ptr
		if ( pPassCoreFn ) pPassCoreFn ( g_pGlob );

		// Set LEGACY ON
		if ( g_bSwitchLegacyOn==true && pSetLegacyModeOn ) pSetLegacyModeOn();

		// Call Convert Function
		DWORD pTempBlock=0, dwTempSize=0;
		if ( pConvertFn ( pFilename, &pTempBlock, &dwTempSize ) )
		{
			// Create local memory for block
			*pdwBlockSize = dwTempSize;
			*ppDBOBlock = (DWORD) new char [ dwTempSize ];
			memcpy ( (LPSTR)*ppDBOBlock, (LPSTR)pTempBlock, dwTempSize );

			// Call Free Function
			pFree( (LPSTR)pTempBlock );

			// success
			bResult=true;
		}
		else
		{
			// could not convert
			bResult=false;
		}
	}
	else
	{
		// Could not locate suitable converter
		return bResult;
	}

	// Free DLL
	FreeLibrary ( hDLLModule );

	// okay
	return bResult;
}

//////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// DBO Import/Export Functions

DARKSDK_DLL bool LoadDBO ( LPSTR pFilename, sObject** ppObject )
{
	// DBOBlock pointer
	DWORD dwBlockSize = 0;
	DWORD* pDBOBlock = NULL;

	// No object to start with
	*ppObject = NULL;

	// Obtain extension
	char pExtension[256];
	strcpy(pExtension, "");
	for ( int n=strlen(pFilename); n>0; n-- )
	{
		if ( pFilename[n]=='.' )
		{
			strcpy(pExtension, pFilename+n+1);
			break;
		}
	}

	// if file MD3 format - permit filecheck skip
	if ( _stricmp ( pExtension, "MD3" )!=NULL )
	{
		// does file exist
		if ( !DBOFileExist ( pFilename ) )
		{
			RunTimeError ( RUNTIMEERROR_FILENOTEXIST );
			return false;
		}
	}

	// if file native DBO format
	if ( _stricmp ( pExtension, "DBO" )==NULL )
	{
		// load DBO object directly
		if ( !DBOLoadBlockFile ( pFilename, (DWORD*)&pDBOBlock, &dwBlockSize ) )
		{
			RunTimeError ( RUNTIMEERROR_B3DOBJECTLOADFAILED );
			return false;
		}
	}
	else
	{
		// call converter DLL (ConvX.dll)
		if ( !ConvertToDBOBlock ( pFilename, pExtension, (DWORD*)&pDBOBlock, &dwBlockSize ) )
		{
			RunTimeError ( RUNTIMEERROR_B3DOBJECTLOADFAILED );
			return false;
		}
	}

	// construct the object
	if ( !DBOConvertBlockToObject ( (DWORD)pDBOBlock, dwBlockSize, ppObject ) )
	{
		RunTimeError ( RUNTIMEERROR_B3DOBJECTLOADFAILED );
		return false;
	}

	// free block when done
	SAFE_DELETE_ARRAY(pDBOBlock);

	// okay
	return true;
}

DARKSDK_DLL bool SaveDBO ( LPSTR pFilename, sObject* pObject )
{
	// DBOBlock ptr
	DWORD dwBlockSize = 0;
	DWORD* pDBOBlock = NULL;

	// does file exist
	if ( DBOFileExist ( pFilename ) )
	{
		RunTimeError ( RUNTIMEERROR_FILEEXISTS );
		return false;
	}

	// leefix - 171203 - before save a DBO, ensure vertex data is original (and not modified by animation activity)
	ResetVertexDataInMesh ( pObject );

	// convert pObject to DBOBlock
	if ( !DBOConvertObjectToBlock ( pObject, (DWORD*)&pDBOBlock, &dwBlockSize ) )
		return false;
		
	// save DBOBlock to file
	if ( !DBOSaveBlockFile ( pFilename, (DWORD)pDBOBlock, dwBlockSize ) )
		return false;

	// free block when done
	SAFE_DELETE(pDBOBlock);

	// okay
	return true;
}

DARKSDK_DLL bool CloneDBO ( sObject** ppDestObject, sObject* pSrcObject )
{
	// DBOBlock ptr
	DWORD dwBlockSize = 0;
	DWORD* pDBOBlock = NULL;

	// convert pObject to DBOBlock
	if ( !DBOConvertObjectToBlock ( pSrcObject, (DWORD*)&pDBOBlock, &dwBlockSize ) )
		return false;
		
	// construct the new destination object
	if ( !DBOConvertBlockToObject ( (DWORD)pDBOBlock, dwBlockSize, ppDestObject ) )
		return false;

	// free block when done
	SAFE_DELETE_ARRAY(pDBOBlock);

	// okay
	return true;
}

// 310305 - mike - new function for allocating custom memory
DARKSDK_DLL void CreateCustomDataArrayForObject	( sObject* pObject, int iSize )
{
	pObject->pCustomData = new BYTE [ iSize ];
}
