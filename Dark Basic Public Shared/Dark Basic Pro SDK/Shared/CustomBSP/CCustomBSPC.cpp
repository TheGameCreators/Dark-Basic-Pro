#include <direct.h>
#include "CCustomBSPC.h"
#include "collision.h"
#include ".\..\error\cerror.h"

// Include Global Structure
#include ".\..\core\globstruct.h"

// Global Shared Data Pointer (passed in from core)
GlobStruct*						g_pGlob							= NULL;

RetD3DFunctionPointerPFN	g_Setup_GetDirect3DDevice;
CAMERA_Int1Float3PFN		g_Camera_Position;
CAMERA_GetFloatPFN			g_Camera_GetXPosition;
CAMERA_GetFloatPFN			g_Camera_GetYPosition;
CAMERA_GetFloatPFN			g_Camera_GetZPosition;
CAMERA_Int1Float3PFN		g_Object_Position;
CAMERA_GetFloatPFN			g_Object_GetXPosition;
CAMERA_GetFloatPFN			g_Object_GetYPosition;
CAMERA_GetFloatPFN			g_Object_GetZPosition;



LPDIRECT3DDEVICE9	 m_pD3D;

PLANE2				FrustumPlanes [ 6 ];
BOOL				DontFrustumReject;
POLYGON*			PolygonArray;
NODE*				NodeArray;
LEAF*				LeafArray;
PLANE*				PlaneArray;
BYTE*				PVSData;
LPDIRECT3DTEXTURE9*	lpTextureSurface;
WORD				NumberOfTextures;
POLYGON**			pTexturePolygonList;

long				BytesPerSet;
long				NumberOfPolygons;
long				NumberOfNodes;
long				NumberOfLeafs;
long				NumberOfPlanes;
long				MAXNUMBEROFNODES;
long				MAXNUMBEROFPLANES;
long				MAXNUMBEROFPOLYGONS;
long				MAXNUMBEROFLEAFS;

long				PVSCompressedSize = 0;
char				( *TextureLUT ) [ 21 ];


// Internal Data Globals
CCollisionManager			g_Col;
int							gCurrentCamera	= 0;
DWORD						g_dwCameraCullMode;

void Constructor ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hCamera, HINSTANCE hObject )
{
	g_Setup_GetDirect3DDevice		= ( RetD3DFunctionPointerPFN )  GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	g_Camera_Position               = ( CAMERA_Int1Float3PFN )		GetProcAddress ( hCamera, "?Position@@YAXHMMM@Z" );
	g_Camera_GetXPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hCamera, "?GetXPosition@@YAMH@Z" );
	g_Camera_GetYPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hCamera, "?GetYPosition@@YAMH@Z" );
	g_Camera_GetZPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hCamera, "?GetZPosition@@YAMH@Z" );
	g_Object_Position				= ( CAMERA_Int1Float3PFN )		GetProcAddress ( hObject, "?ColPosition@@YAXHMMM@Z" );
	g_Object_GetXPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hObject, "?GetXColPosition@@YAMH@Z" );
	g_Object_GetYPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hObject, "?GetYColPosition@@YAMH@Z" );
	g_Object_GetZPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hObject, "?GetZColPosition@@YAMH@Z" );
	
	m_pD3D = g_Setup_GetDirect3DDevice ( );

	PolygonArray        = NULL;
	NodeArray           = NULL;
	LeafArray           = NULL;
	PlaneArray          = NULL;
	PVSData             = NULL;
	lpTextureSurface    = NULL;
	pTexturePolygonList = NULL;

	// DBpro uses camera zero
	gCurrentCamera = 0;
}

void Destructor  ( void )
{
	// MIKE FIX 190303 - modified clean up code

	DeletePolygonArray ( );

	if ( NodeArray )
		free ( NodeArray );

	if ( LeafArray )
		free ( LeafArray );

	if ( PlaneArray )
		free ( PlaneArray );

	if ( PVSData )
		free ( PVSData );

	NodeArray = NULL;
	LeafArray = NULL;
	PlaneArray = NULL;
	PVSData = NULL;

	/*
	DeletePortalArray ( );
	FreeDuplicatePortals ( );
	*/
	for ( int i = 0; i < NumberOfTextures; i++ )
	{
		SAFE_RELEASE(lpTextureSurface[i]);
	}

	NumberOfTextures = 0;

	SAFE_DELETE_ARRAY ( lpTextureSurface );
	SAFE_DELETE_ARRAY ( TextureLUT );
	SAFE_DELETE_ARRAY ( pTexturePolygonList );

	//delete [ ] lpTextureSurface;
	//delete [ ] TextureLUT;
	//delete [ ] pTexturePolygonList;
}

void PassCoreData( LPVOID pGlobPtr )
{
	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;
}

bool Load ( char* szFilename )
{
	// Split file and folder
	char lpBSPFile[256];
	char lpBSPPath[256];
	strcpy(lpBSPPath, "");
	strcpy(lpBSPFile, szFilename);
	for(DWORD i=strlen(szFilename); i>0; i--)
	{
		if(szFilename[i]=='\\' || szFilename[i]=='/')
		{
			// found path so split
			strcpy(lpBSPFile, szFilename+i+1);
			strcpy(lpBSPPath, szFilename);
			lpBSPPath[i]=0;
			break;
		}
	}

	// Record current directory
	char olddir[256];
	getcwd(olddir, 256);

	// Switch to BSP folder
	chdir(lpBSPPath);

	// Parse BSP
	long	a;
	FILE*	stream;
	stream = fopen ( lpBSPFile, "rb" );
	if( stream==NULL )
		return false;

	// read node array array - we can Read the array as one block 
	fread ( &NumberOfNodes, sizeof ( long ), 1, stream );
	NodeArray = ( NODE * ) malloc ( NumberOfNodes * sizeof ( NODE ) );
	
	NODE* n	= NodeArray;
	
	for ( a = 0; a < NumberOfNodes; a++)
	{
		fread ( &n->IsLeaf, sizeof ( unsigned char ), 1, stream );
		fread ( &n->Plane,  sizeof ( unsigned long ), 1, stream );
		fread ( &n->Front,  sizeof ( unsigned long ), 1, stream );
		fread ( &n->Back,   sizeof ( signed long   ), 1, stream );
		n++;
	}

	// read the plane array
	fread ( &NumberOfPlanes, sizeof ( long ), 1, stream );
	PlaneArray = ( PLANE* ) malloc ( NumberOfPlanes * sizeof ( PLANE ) );
	fread ( PlaneArray, sizeof ( PLANE ), NumberOfPlanes, stream );

	// read leaf array.
	fread ( &NumberOfLeafs, sizeof ( long ), 1, stream );
	LeafArray = ( LEAF* ) malloc ( NumberOfLeafs * sizeof ( LEAF ) );
	
	LEAF* l = LeafArray;

	for ( a = 0; a < NumberOfLeafs; a++ )
	{
		fread ( &l->StartPolygon, sizeof ( long        ), 1, stream );
		fread ( &l->EndPolygon,   sizeof ( long        ), 1, stream );
		fread ( &l->PVSIndex,     sizeof ( long        ), 1, stream );
		fread ( &l->BoundingBox,  sizeof ( BOUNDINGBOX ), 1, stream );
		l++;
	}

	// read polygon array
	fread ( &NumberOfPolygons, sizeof ( long ), 1, stream );
	PolygonArray = ( POLYGON* ) malloc ( NumberOfPolygons * sizeof ( POLYGON ) );
	
	POLYGON* p = PolygonArray;

	for ( a = 0; a < NumberOfPolygons; a++ )
	{
		fread ( &p->NumberOfVertices, sizeof ( WORD ), 1, stream );
		p->VertexList = new D3DVERTEX [ p->NumberOfVertices ];

		// Read in LVERTEX, and copy to (full)VERTEX
		D3DLVERTEX* vertTemp = new D3DLVERTEX [ p->NumberOfVertices ];
		fread (  vertTemp,      sizeof ( D3DLVERTEX ), p->NumberOfVertices, stream );
		for(int c=0; c<p->NumberOfVertices; c++)
		{
			p->VertexList[c].x		= vertTemp[c].x;
			p->VertexList[c].y		= vertTemp[c].y;
			p->VertexList[c].z		= vertTemp[c].z;
			p->VertexList[c].color	= vertTemp[c].color;
			p->VertexList[c].tu		= vertTemp[c].tu;
			p->VertexList[c].tv		= vertTemp[c].tv;
		}
		delete vertTemp;

		fread ( &p->NumberOfIndices, sizeof ( WORD       ), 1,                   stream );
		p->Indices = new WORD [ p->NumberOfIndices ];

		fread (  p->Indices,      sizeof ( WORD        ), p->NumberOfIndices, stream );
		fread ( &p->Normal,       sizeof ( D3DXVECTOR3 ), 1,                  stream );
		fread ( &p->TextureIndex, sizeof ( WORD        ), 1,                  stream );

		// copy over normal to vertex (needs to be part of vertex packet)
		for(int d=0; d<p->NumberOfVertices; d++)
		{
			p->VertexList[d].nx		= p->Normal.x;
			p->VertexList[d].ny		= p->Normal.y;
			p->VertexList[d].nz		= p->Normal.z;
		}

		p++;
	}


	// now all we have to do is read the PVS Data itself
	fread ( &PVSCompressedSize, sizeof ( long ), 1, stream );
	PVSData	= ( BYTE* ) malloc ( PVSCompressedSize * sizeof ( BYTE ) );
	fread ( PVSData, sizeof ( BYTE ), PVSCompressedSize, stream );

	// and then read in texture look up table
	fread ( &NumberOfTextures, sizeof ( WORD ), 1, stream );				// read number of textures used
	TextureLUT = new char [ NumberOfTextures ] [ 21 ];						// allocate a set of 21 character strings.
	fread ( TextureLUT, sizeof ( char ) * 21, NumberOfTextures, stream );	// read all sets of 21 char strings

	fclose ( stream );

	LoadTextures ( );

	g_Col.enableCollisions ( );
	g_Col.setEllipsoid ( VECTOR3 ( 10, 39, 10 ) );
	//g_Col.setGravity   ( VECTOR3 ( 0, -1, 0 ) );
	g_Col.setGravity   ( VECTOR3 ( 0, 0, 0 ) );

	// Restore current folder
	chdir(olddir);

	return true;
}

void DeleteEx ( void )
{
	// MIKE FIX 190303 - delete BSP data
	Destructor ( );
}

enum eCollisionType
{
	OBJECT_COLLISION,
	CAMERA_COLLISION,
};

struct sCollision
{
	D3DXVECTOR3		vecOldPosition;
	D3DXVECTOR3		vecNewPosition;
	D3DXVECTOR3		vecRadius;
	float			fRadius;
	eCollisionType	eType;
	int				iID;

	int				iResponse;
	bool			bRespImpact;
	D3DXVECTOR3		vecResp;

	D3DXVECTOR3		vecEllipse;
	float			fAdjustY;
};

sCollision gCollision       [ 25 ];
bool	   gCollisionSwitch [ 25 ];
bool	   bActivate = true;

void SetOldPos ( int iTemp )
{
	if ( gCollision [ iTemp ].eType == CAMERA_COLLISION )
	{
		gCollision [ iTemp ].vecOldPosition = D3DXVECTOR3 ( 
																g_Camera_GetXPosition ( gCollision [ iTemp ].iID ),
																g_Camera_GetYPosition ( gCollision [ iTemp ].iID )-gCollision[iTemp].fAdjustY,
																g_Camera_GetZPosition ( gCollision [ iTemp ].iID )
														  );
	}
	else
	{
		gCollision [ iTemp ].vecOldPosition = D3DXVECTOR3 ( 
																g_Object_GetXPosition ( gCollision [ iTemp ].iID ),
																g_Object_GetYPosition ( gCollision [ iTemp ].iID )-gCollision[iTemp].fAdjustY,
																g_Object_GetZPosition ( gCollision [ iTemp ].iID )
														  );
	}
}

void SetCameraCollisionRadius ( int iID, int iEntity, float fX, float fY, float fZ )
{
	if ( bActivate )
	{
		memset ( gCollisionSwitch, 0, sizeof ( gCollisionSwitch ) );
		bActivate = false;
	}

	gCollision [ iID ].eType   = CAMERA_COLLISION;
	gCollision [ iID ].fRadius = -1.0f;
	gCollision [ iID ].iID     = iEntity;
	gCollision [ iID ].vecEllipse = D3DXVECTOR3 ( fX, fY, fZ );

	gCollisionSwitch [ iID ] = true;
	SetOldPos ( iID );
}

void SetObjectCollisionRadius ( int iID, int iEntity, float fX, float fY, float fZ )
{
	if ( bActivate )
	{
		memset ( gCollisionSwitch, 0, sizeof ( gCollisionSwitch ) );
		bActivate = false;
	}

	gCollision [ iID ].eType   = OBJECT_COLLISION;
	gCollision [ iID ].fRadius = -1.0f;
	gCollision [ iID ].iID     = iEntity;
	gCollision [ iID ].vecEllipse = D3DXVECTOR3 ( fX, fY, fZ );

	gCollisionSwitch [ iID ] = true;
	SetOldPos ( iID );
}

void SetupCameraCollision ( int iID, int iEntitiy, float fRadius, int iResponse )
{
	if ( bActivate )
	{
		memset ( gCollisionSwitch, 0, sizeof ( gCollisionSwitch ) );
		bActivate = false;
	}

	gCollision [ iID ].eType   = CAMERA_COLLISION;
	gCollision [ iID ].fRadius = fRadius;
	gCollision [ iID ].iID     = iEntitiy;
	gCollision [ iID ].vecEllipse = D3DXVECTOR3 ( fRadius, fRadius*2, fRadius );
	
	gCollision [ iID ].iResponse     = iResponse;
	gCollision [ iID ].bRespImpact   = false;

	gCollisionSwitch [ iID ] = true;
	SetOldPos ( iID );
}

void SetupObjectCollision ( int iID, int iEntitiy, float fRadius, int iResponse )
{
	if ( bActivate )
	{
		memset ( gCollisionSwitch, 0, sizeof ( gCollisionSwitch ) );
		bActivate = false;
	}

	gCollision [ iID ].eType   = OBJECT_COLLISION;
	gCollision [ iID ].fRadius = fRadius;
	gCollision [ iID ].iID     = iEntitiy;
	gCollision [ iID ].vecEllipse = D3DXVECTOR3 ( fRadius, fRadius*2, fRadius );

	gCollision [ iID ].iResponse     = iResponse;
	gCollision [ iID ].bRespImpact   = false;

	gCollisionSwitch [ iID ] = true;
	SetOldPos ( iID );
}

int GetCollisionResponse ( int iID )
{
	if ( gCollision [ iID ].bRespImpact )
	{
		gCollision [ iID ].bRespImpact=false;
		return 1;
	}
	else
		return 0;
}

float GetCollisionX ( int iID )
{
	return gCollision [ iID ].vecResp.x;
}

float GetCollisionY ( int iID )
{
	return gCollision [ iID ].vecResp.y;
}

float GetCollisionZ ( int iID )
{
	return gCollision [ iID ].vecResp.z;
}

void Start ( void )
{
	for ( int iTemp = 0; iTemp < 25; iTemp++ )
	{
		if ( gCollisionSwitch [ iTemp ] )
		{
			SetOldPos ( iTemp );
		}
	}
}

void ProcessCollision( int iTemp )
{
	if ( gCollisionSwitch [ iTemp ] )
	{
		if ( gCollision [ iTemp ].eType == CAMERA_COLLISION )
		{
			gCollision [ iTemp ].vecNewPosition = D3DXVECTOR3 ( 
																	g_Camera_GetXPosition ( gCollision [ iTemp ].iID ),
																	g_Camera_GetYPosition ( gCollision [ iTemp ].iID )-gCollision[iTemp].fAdjustY,
																	g_Camera_GetZPosition ( gCollision [ iTemp ].iID )
															  );
		}
		else
		{
			gCollision [ iTemp ].vecNewPosition = D3DXVECTOR3 ( 
																	g_Object_GetXPosition ( gCollision [ iTemp ].iID ),
																	g_Object_GetYPosition ( gCollision [ iTemp ].iID )-gCollision[iTemp].fAdjustY,
																	g_Object_GetZPosition ( gCollision [ iTemp ].iID )
															  );
		}

		// Snapshot where entity would have been
		D3DXVECTOR3 vecLatestPos;
		vecLatestPos = gCollision [ iTemp ].vecNewPosition;

		// Test for collision clip
		BOOL bClipped = Collision::World
							( 
								gCollision [ iTemp ].vecOldPosition, 
								gCollision [ iTemp ].vecNewPosition, 
								gCollision [ iTemp ].vecEllipse,
								&gCollision [ iTemp ].vecNewPosition, 
								5
						   );
		
		gCollision [ iTemp ].vecResp = gCollision [ iTemp ].vecNewPosition - vecLatestPos;
		if ( bClipped==TRUE )
		{
			// Set for response detector
			gCollision [ iTemp ].bRespImpact = true;

			// Adjust position if mode correctly set
			if ( gCollision [ iTemp ].eType == CAMERA_COLLISION )
			{
				
				if ( gCollision [ iTemp ].iResponse == 0 )
				{
					g_Camera_Position ( 
											gCollision [ iTemp ].iID,
											gCollision [ iTemp ].vecNewPosition.x,
											gCollision [ iTemp ].vecNewPosition.y+gCollision[iTemp].fAdjustY, 
											gCollision [ iTemp ].vecNewPosition.z
									  );
				}
				if ( gCollision [ iTemp ].iResponse == 1 )
				{
					g_Camera_Position ( 
											gCollision [ iTemp ].iID,
											gCollision [ iTemp ].vecOldPosition.x,
											gCollision [ iTemp ].vecOldPosition.y+gCollision[iTemp].fAdjustY, 
											gCollision [ iTemp ].vecOldPosition.z
									  );
				}
			}
			else
			{
				if ( gCollision [ iTemp ].iResponse == 0 )
				{
					g_Object_Position ( 
											gCollision [ iTemp ].iID,
											gCollision [ iTemp ].vecNewPosition.x,
											gCollision [ iTemp ].vecNewPosition.y+gCollision[iTemp].fAdjustY, 
											gCollision [ iTemp ].vecNewPosition.z
									  );
				}
				if ( gCollision [ iTemp ].iResponse == 1 )
				{
					g_Object_Position ( 
											gCollision [ iTemp ].iID,
											gCollision [ iTemp ].vecOldPosition.x,
											gCollision [ iTemp ].vecOldPosition.y+gCollision[iTemp].fAdjustY, 
											gCollision [ iTemp ].vecOldPosition.z
									  );
				}
			}
		}
	}
}

void End ( void )
{
	for ( int iTemp = 0; iTemp < 25; iTemp++ )
	{
		ProcessCollision ( iTemp );
	}
}

void Update ( void )
{
	//////
	D3DXMATRIX	matTranslation,
				matRotation,
				matPitch;
	D3DXMatrixIdentity  ( &matTranslation );

	D3DXMatrixRotationX ( &matRotation, 0.0 );
	D3DXMatrixMultiply  ( &matTranslation, &matRotation, &matTranslation );
	
	D3DXMatrixRotationY ( &matRotation, 0.0 );
	D3DXMatrixMultiply  ( &matTranslation, &matRotation, &matTranslation );

	D3DXMatrixRotationZ ( &matRotation, 0.0 );
	D3DXMatrixMultiply  ( &matTranslation, &matRotation, &matTranslation );

	D3DXMatrixRotationYawPitchRoll ( &matPitch, 0.0, 0.0, 0.0 ); 
	D3DXMatrixMultiply             ( &matTranslation, &matTranslation, &matPitch );
	
	matTranslation._41 = 0.0;
	matTranslation._42 = 0.0;
	matTranslation._43 = 0.0;
	m_pD3D->SetTransform ( D3DTS_WORLD, &matTranslation );

	// initial renderstates
	m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_pD3D->SetRenderState ( D3DRS_ZENABLE, TRUE );
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, false );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );

	// LEEFIX - 210703 - NO SPECULAR AT THIS TIME
	m_pD3D->SetRenderState ( D3DRS_SPECULARENABLE, FALSE );

	m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSU, 1 );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSV, 1 );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );	
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );	// mip mapping

	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	// set material
	D3DMATERIAL9 mtrl;
	memset ( &mtrl, 0, sizeof ( mtrl ) );
	mtrl.Ambient.r = 1.0f;
	mtrl.Ambient.g = 1.0f;
	mtrl.Ambient.b = 1.0f;
	mtrl.Ambient.a = 1.0f;
	mtrl.Diffuse.r = 1.0f;
	mtrl.Diffuse.g = 1.0f;
	mtrl.Diffuse.b = 1.0f;
	mtrl.Diffuse.a = 1.0f;
	m_pD3D->SetMaterial ( &mtrl );
	m_pD3D->SetRenderState ( D3DRS_LIGHTING, TRUE );

	/////

	// LEEFIX - 171002 - Render direct view or reflected stencil view
	float fX = g_Camera_GetXPosition ( gCurrentCamera );
	float fY = g_Camera_GetYPosition ( gCurrentCamera );
	float fZ = g_Camera_GetZPosition ( gCurrentCamera );
	g_dwCameraCullMode = D3DCULL_CCW;
	if(g_pGlob)
	{
		if(g_pGlob->dwStencilMode==2)
		{
			if(g_pGlob->dwRedrawPhase==1)
			{
				// View is a reflection
				fX = g_pGlob->fReflectionPlaneX;
				fY = g_pGlob->fReflectionPlaneY;
				fZ = g_pGlob->fReflectionPlaneZ;				
				g_dwCameraCullMode = D3DCULL_CW;
			}
		}
	}

	ExtractFrustumPlanes ( FrustumPlanes );
	D3DXVECTOR3 vecPos = D3DXVECTOR3 ( fX, fY, fZ );
	RenderTree ( vecPos );

	// Restore
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );	
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );	// mip mapping

	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, false );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
	m_pD3D->SetRenderState ( D3DRS_ZENABLE, TRUE );
	m_pD3D->SetRenderState ( D3DRS_SPECULARENABLE, TRUE );
}

int ClassifyPoly ( PLANE* Plane, POLYGON* Poly )
{
	int			Infront = 0;
	int			Behind  = 0;
	int			OnPlane = 0;

	float			result  = 0;
	D3DXVECTOR3*	vec1    = &Plane->PointOnPlane;

	// loop round each vertex
	for ( int a = 0; a < Poly->NumberOfVertices; a++ )
	{
		D3DXVECTOR3* vec2		= ( D3DXVECTOR3* ) &Poly->VertexList [ a ];
		D3DXVECTOR3  Direction	= ( *vec1 ) - ( *vec2 );

		result = D3DXVec3Dot ( &Direction, &Plane->Normal );

		// check the position
		if ( result > g_EPSILON )
		{
			Behind++;
		} 
		else 
			if ( result < -g_EPSILON )
			{
				Infront++;
			} 
			else
			{
				OnPlane++;
				Infront++;
				Behind++;
			}
	}

	if ( OnPlane == Poly->NumberOfVertices ) 
		return CP_ONPLANE;

	if ( Behind  == Poly->NumberOfVertices ) 
		return CP_BACK;

	if ( Infront == Poly->NumberOfVertices ) 
		return CP_FRONT;

	return CP_SPANNING;
}

void RenderTree ( D3DXVECTOR3 pos )
{
	int	Node = 0;
	int	leaf = 0;

	// MIKE FIX 190303 - make sure we have some valid data
	if ( !NodeArray )
		return;
	
	while ( 1 )
	{
		switch ( ClassifyPoint ( &pos, &PlaneArray [ NodeArray [ Node ].Plane ] ) )
		{
			case CP_FRONT:
			{
				if ( NodeArray [ Node ].IsLeaf != 0 )
				{
					leaf = NodeArray [ Node ].Front;
					DrawTree ( leaf );
					return;
				}
				else 
					Node = NodeArray [ Node ].Front;
			}
			break;

			case CP_BACK: 
			{
				if ( NodeArray [ Node ].Back == -1 )
					return;
				else 
					Node = NodeArray [ Node ].Back;
			}
			break;

			case CP_ONPLANE:
			{
				if ( NodeArray [ Node ].IsLeaf != 0 )
				{
					leaf = NodeArray [ Node ].Front;
					DrawTree ( leaf );
					return;
				}
				else 
					Node = NodeArray [ Node ].Front;
			}
			break;

			default:
				break;
		}
	}
}

WORD		wIndices [ 10000 ];
D3DVERTEX	vertices [ 10000 ];


void DrawTree ( long leaf )
{
	// draw the PVS aet
	int			i;
	long		currentleaf		= 0;
	long		PVSOFFSET		= LeafArray [ leaf ].PVSIndex;
	BYTE*		PVSPointer		= PVSData;
	POLYGON*	CurrentPoly		= NULL;
	
	PVSPointer += PVSOFFSET;
	
	for ( i = 0; i < NumberOfTextures; i++ )
		pTexturePolygonList [ i ] = NULL;

	// loop around leaves
	while ( currentleaf < NumberOfLeafs )
	{
		// if this is a non 0 PVS byte
		if ( *PVSPointer != 0 )
		{
			for ( i = 0; i < 8; i++ )
			{
				BYTE mask	= 1 << i;
				BYTE pvs	= *PVSPointer;
				
				if ( pvs & mask )
				{
					if ( LeafInFrustum ( currentleaf ) || DontFrustumReject )
					{
						unsigned long start	 = LeafArray [ currentleaf ].StartPolygon;
						unsigned long finish = LeafArray [ currentleaf ].EndPolygon; 
						unsigned long Count  = 0;

						// collect the polys from this leaf and add them to the correct texture list
						for ( Count = start; Count < finish; Count++ )
						{
							CurrentPoly		  = &PolygonArray [ Count ];
							CurrentPoly->Next = pTexturePolygonList [ CurrentPoly->TextureIndex ];

							pTexturePolygonList [ CurrentPoly->TextureIndex ] = CurrentPoly;
						}
					}
				}

				currentleaf++;
			}

			PVSPointer++;
		} 
		else
		{
			// we have hit a zero so read in the next byte and see how long the run of zeros is
			PVSPointer++;
			BYTE RunLength = *PVSPointer;
			PVSPointer++;
			currentleaf += RunLength * 8;
		}
	}

// LEEFIX - 220902 - MUST HAVE RESPONSE TO SCENE LIGHTING!
//	m_pD3D->SetVertexShader ( D3DFVF_LVERTEX );
//	m_pD3D->SetRenderState ( D3DRS_LIGHTING, FALSE       );
	m_pD3D->SetVertexShader ( NULL );
	m_pD3D->SetFVF ( D3DFVF_VERTEX );
// LEEFIX - 210703 - NO SPECULAR AT THIS TIME
	m_pD3D->SetRenderState ( D3DRS_SPECULARENABLE, FALSE );


// LEEFIX - 171002 - For Reflection rendering the BSP culling must be reversed
//	m_pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
	m_pD3D->SetRenderState ( D3DRS_CULLMODE, g_dwCameraCullMode );

	// we now have all the polygons in linked lists grouped by texture so just set each 
	// texture and render all the polygons in that textures list
	for ( i = 0; i < NumberOfTextures; i++ )
	{
		if ( pTexturePolygonList [ i ] != NULL )
		{
			int			iIndexPos  = 0;
			int			iVertexPos = 0;
			int			iOffset    = 0;
			int			iIndexCount = 0;

			memset ( wIndices, 0, sizeof ( wIndices ) );
			memset ( vertices, 0, sizeof ( vertices ) );

			if ( lpTextureSurface [ i ] ) 
				m_pD3D->SetTexture ( 0, lpTextureSurface [ i ] );
			else 
				m_pD3D->SetTexture ( 0, NULL );
			
			CurrentPoly = pTexturePolygonList [ i ];
			
			// traverse linked list
			while ( CurrentPoly != NULL )
			{
				/*
				m_pD3D->DrawIndexedPrimitiveUP
												(
													D3DPT_TRIANGLELIST,
													0,
													CurrentPoly->NumberOfVertices,
													CurrentPoly->NumberOfIndices / 3,
													&CurrentPoly->Indices [ 0 ],
													D3DFMT_INDEX16,
													&CurrentPoly->VertexList [ 0 ],
													sizeof ( D3DVERTEX )
												);
				*/

				memcpy ( &vertices [ iVertexPos ], &CurrentPoly->VertexList [ 0 ], sizeof ( D3DVERTEX ) * CurrentPoly->NumberOfVertices );
				iVertexPos += CurrentPoly->NumberOfVertices;

				for ( int iTemp = 0; iTemp < CurrentPoly->NumberOfIndices; iTemp++ )
					wIndices [ iIndexPos++ ] = CurrentPoly->Indices [ iTemp ] + iOffset;
				
				iIndexCount += CurrentPoly->NumberOfIndices / 3;
				iOffset     += CurrentPoly->NumberOfVertices;

				CurrentPoly = CurrentPoly->Next;
			}

			m_pD3D->DrawIndexedPrimitiveUP (
								D3DPT_TRIANGLELIST,
								0,
								iVertexPos,
								iIndexPos / 3,
								&wIndices [ 0 ],
								D3DFMT_INDEX16,
								&vertices [ 0 ],
								sizeof ( D3DVERTEX )
							 );
		}


	}

	m_pD3D->SetTexture ( 0, NULL );
}

BOOL LineOfSight ( D3DXVECTOR3* Start, D3DXVECTOR3* End, long Node )
{
	float		temp;
	D3DXVECTOR3 intersection;

	NODE*	CurrNode	= &NodeArray  [ Node ];
	PLANE*	Plane		= &PlaneArray [ CurrNode->Plane ];

	int PointA = ClassifyPoint ( Start,	Plane );
	int PointB = ClassifyPoint ( End,	Plane );

	
	if ( PointA == CP_ONPLANE && PointB == CP_ONPLANE )
	{
		if ( CurrNode->IsLeaf == 0 )
			return LineOfSight ( Start, End, CurrNode->Front );
		else
			// this is a front leaf - front leafs are 
			// always empty so this is an empty space
			return TRUE;		
	}

	if ( PointA == CP_FRONT && PointB == CP_BACK )
	{
		if ( CurrNode->Back == -1 )
			return FALSE; 

		Get_Intersect ( Start, End, &Plane->PointOnPlane, &Plane->Normal, &intersection, &temp );
		
		if ( CurrNode->IsLeaf == 0 )
			return LineOfSight ( Start, &intersection, CurrNode->Front ) && LineOfSight ( End, &intersection, CurrNode->Back );
		else
			return TRUE && LineOfSight ( End, &intersection, CurrNode->Back );
	}

	if ( PointA == CP_BACK && PointB == CP_FRONT )
	{
		if ( CurrNode->Back == -1 )
			return FALSE;

		Get_Intersect ( Start, End, &Plane->PointOnPlane, &Plane->Normal, &intersection, &temp );
 
		if ( CurrNode->IsLeaf == 0 )
			return LineOfSight ( End, &intersection, CurrNode->Front ) && LineOfSight ( Start, &intersection, CurrNode->Back );
		else
			return TRUE && LineOfSight ( Start, &intersection, CurrNode->Back );
	}

	// if we get here one of the points is on the plane
	if ( PointA == CP_FRONT || PointB == CP_FRONT )
	{
		if ( CurrNode->IsLeaf == 0 )
			return LineOfSight ( Start, End, CurrNode->Front );
		else
			return TRUE;
	} 
	else
	{
		if ( CurrNode->Back == -1 )
			return FALSE;
		else
			return LineOfSight ( Start, End, CurrNode->Back );
	}

	return TRUE;
}

BOOL CollideSphere ( D3DXVECTOR3* SphereCenter, float SphereRadius, long Node )
{  
	float	temp;
	int		PointA;
	int		PointB;

	D3DXVECTOR3		intersection;
	D3DXVECTOR3		SphereRadiusEnd;

	PLANE*	Plane	 = &PlaneArray [ NodeArray [ Node ].Plane ];
	NODE*	CurrNode = &NodeArray  [ Node ];

	PointA = ClassifyPoint ( SphereCenter, Plane );

	if ( PointA == CP_BACK )
		SphereRadiusEnd =* SphereCenter + ( Plane->Normal * SphereRadius );
	else
		SphereRadiusEnd =* SphereCenter + ( -Plane->Normal * SphereRadius );
	
	PointB = ClassifyPoint ( &SphereRadiusEnd, Plane );

	
	if ( PointA == CP_ONPLANE && PointB == CP_ONPLANE )
	{
 		if ( CurrNode->IsLeaf == 0 )
		{
			// this is not a leaf so recurse
			return CollideSphere ( SphereCenter, SphereRadius, CurrNode->Front );
		} 
		else
		{
			// this is a front leaf - front leafs are always empty so this is empty space
			return FALSE;
		}
	}

	// spanning front to back
	if ( PointA == CP_FRONT && PointB == CP_BACK )
	{
		if ( CurrNode->Back == -1 )
			return TRUE;

		Get_Intersect ( SphereCenter, &SphereRadiusEnd, &Plane->PointOnPlane, &Plane->Normal, &intersection, &temp );

		if ( CurrNode->IsLeaf == 0 )
		{
			return CollideSphere
								(
									SphereCenter,
									D3DXVec3Length (&( intersection-*SphereCenter ) ), CurrNode->Front ) || CollideSphere ( &SphereRadiusEnd, D3DXVec3Length (&( intersection-SphereRadiusEnd ) ), CurrNode->Back );
		} else 
			return CollideSphere ( &SphereRadiusEnd, D3DXVec3Length (& ( intersection-SphereRadiusEnd ) ), CurrNode->Back );
	}

	// spanning back to front
	if ( PointA == CP_BACK && PointB == CP_FRONT )
	{
		if ( CurrNode->Back == -1 )
			return TRUE;
			
		Get_Intersect ( &SphereRadiusEnd, SphereCenter, &Plane->PointOnPlane, &Plane->Normal, &intersection, &temp );
 
		if ( CurrNode->IsLeaf == 0 )
			return CollideSphere ( &SphereRadiusEnd, D3DXVec3Length ( &( intersection-SphereRadiusEnd ) ), CurrNode->Front ) || CollideSphere ( SphereCenter, D3DXVec3Length ( &( intersection -* SphereCenter ) ), CurrNode->Back );
		else 
			return CollideSphere ( SphereCenter, D3DXVec3Length ( &( intersection -* SphereCenter ) ), CurrNode->Back );
	}

	// if we get here one of the points is on the plane
	if ( PointA == CP_FRONT || PointB == CP_FRONT )
	{
		if ( CurrNode->IsLeaf == 0 )
			return CollideSphere ( SphereCenter, SphereRadius, CurrNode->Front );
		else
			return FALSE;
	}
	else 
	{
		if ( CurrNode->Back == -1 )
			return TRUE;
		else
			return CollideSphere ( SphereCenter, SphereRadius, CurrNode->Back );
	}

	return FALSE;
}

BOOL Get_Intersect ( D3DXVECTOR3* linestart, D3DXVECTOR3* lineend, D3DXVECTOR3* vertex, D3DXVECTOR3* normal, D3DXVECTOR3* intersection, float* percentage )
{
	D3DXVECTOR3 direction;
	D3DXVECTOR3	L1;

	float	    linelength;
	float		dist_from_plane;

	direction.x = lineend->x - linestart->x;
	direction.y = lineend->y - linestart->y;
	direction.z = lineend->z - linestart->z;

	linelength  = D3DXVec3Dot ( &direction, normal );

	if ( fabsf ( linelength ) < g_EPSILON )
		return FALSE; 

	L1.x = vertex->x - linestart->x;
	L1.y = vertex->y - linestart->y;
	L1.z = vertex->z - linestart->z;

	dist_from_plane = D3DXVec3Dot ( &L1, normal );

	// how far from linestart, intersection is as a percentage of 0 to 1 
	*percentage = dist_from_plane / linelength; 

	// the plane is behind the start of the line or
	// the line does not reach the plane
	if ( *percentage < 0.0f || *percentage > 1.0f ) 
		return FALSE;

	// add the percentage of the line to line start
	intersection->x = linestart->x + direction.x * ( *percentage );
	intersection->y = linestart->y + direction.y * ( *percentage );
	intersection->z = linestart->z + direction.z * ( *percentage );

	return TRUE;
}

void ExtractFrustumPlanes ( PLANE2 *Planes )
{
	
	D3DXMATRIX	ViewMatrix;
	D3DXMATRIX	ProjMatrix;
	D3DXMATRIX	ViewProj;
	D3DXVECTOR3 vecPos;

	vecPos.x = g_Camera_GetXPosition ( 0 );
	vecPos.y = g_Camera_GetXPosition ( 1 );
	vecPos.z = g_Camera_GetXPosition ( 2 );

	m_pD3D->GetTransform ( D3DTS_PROJECTION, &ProjMatrix );	// get the current projection matrix

	m_pD3D->GetTransform ( D3DTS_VIEW, &ViewMatrix );	// get the current projection matrix

	//D3DXMatrixIdentity ( &ViewMatrix );							// make view an identity matrix to start with
	
	//ViewMatrix._11 = vecRight.x;	ViewMatrix._12 = vecUp.x;	ViewMatrix._13 = vecLook.x;
	//ViewMatrix._21 = vecRight.y;	ViewMatrix._22 = vecUp.y;	ViewMatrix._23 = vecLook.y;
	//ViewMatrix._31 = vecRight.z;	ViewMatrix._32 = vecUp.z;	ViewMatrix._33 = vecLook.z;
	
	//ViewMatrix._41 =- D3DXVec3Dot (	&vecPos, &vecRight );
	//ViewMatrix._42 =- D3DXVec3Dot (	&vecPos, &vecUp    );
	//ViewMatrix._43 =- D3DXVec3Dot (	&vecPos, &vecLook  );
	
	// combine view and projection matrices
	D3DXMatrixMultiply ( &ViewProj, &ViewMatrix, &ProjMatrix );

	// left clipping plane
	Planes [ 0 ].Normal.x = -( ViewProj._14 + ViewProj._11 );
	Planes [ 0 ].Normal.y = -( ViewProj._24 + ViewProj._21 );
	Planes [ 0 ].Normal.z = -( ViewProj._34 + ViewProj._31 );
	Planes [ 0 ].Distance = -( ViewProj._44 + ViewProj._41 );

	// right clipping plane
	Planes [ 1 ].Normal.x = -( ViewProj._14 - ViewProj._11 );
	Planes [ 1 ].Normal.y = -( ViewProj._24 - ViewProj._21 );
	Planes [ 1 ].Normal.z = -( ViewProj._34 - ViewProj._31 );
	Planes [ 1 ].Distance = -( ViewProj._44 - ViewProj._41 );

	// top clipping plane
	Planes [ 2 ].Normal.x = -( ViewProj._14 - ViewProj._12 );
	Planes [ 2 ].Normal.y = -( ViewProj._24 - ViewProj._22 );
	Planes [ 2 ].Normal.z = -( ViewProj._34 - ViewProj._32 );
	Planes [ 2 ].Distance = -( ViewProj._44 - ViewProj._42 );

	// bottom clipping plane
	Planes [ 3 ].Normal.x = -( ViewProj._14 + ViewProj._12 );
	Planes [ 3 ].Normal.y = -( ViewProj._24 + ViewProj._22 );
	Planes [ 3 ].Normal.z = -( ViewProj._34 + ViewProj._32 );
	Planes [ 3 ].Distance = -( ViewProj._44 + ViewProj._42 );

	// near clipping plane
	Planes [ 4 ].Normal.x = -( ViewProj._14 + ViewProj._13 );
	Planes [ 4 ].Normal.y = -( ViewProj._24 + ViewProj._23 );
	Planes [ 4 ].Normal.z = -( ViewProj._34 + ViewProj._33 );
	Planes [ 4 ].Distance = -( ViewProj._44 + ViewProj._43 );

	// far clipping plane
	Planes [ 5 ].Normal.x = -( ViewProj._14 - ViewProj._13 );
	Planes [ 5 ].Normal.y = -( ViewProj._24 - ViewProj._23 );
	Planes [ 5 ].Normal.z = -( ViewProj._34 - ViewProj._33 );
	Planes [ 5 ].Distance = -( ViewProj._44 - ViewProj._43 );

	for ( int i = 0; i < 6; i++ )
	{
		// get magnitude of vector
		float denom = 1.0f / D3DXVec3Length ( &Planes [ i ].Normal );

		Planes [ i ].Normal.x *= denom;
		Planes [ i ].Normal.y *= denom;
		Planes [ i ].Normal.z *= denom;
		Planes [ i ].Distance *= denom;
	}
	

	/*
	// setup the planes for the viewing frustum

	// variable declarations
	D3DXMATRIX	Matrix,
				matView,
				matProj;

	float		ZMin,
				Q;

	// check d3d is ok
	if ( !m_pD3D )
		return;

	// get the projection matrix
	m_pD3D->GetTransform ( D3DTS_PROJECTION, &matProj );

	// get the view matrix
	m_pD3D->GetTransform ( D3DTS_VIEW, &matView );

	// multiply with the projection matrix
	D3DXMatrixMultiply ( &Matrix, &matView, &matProj );

	// left clipping plane
	Planes [ 0 ].Normal.x = -( matView._14 + matView._11 );
	Planes [ 0 ].Normal.y = -( matView._24 + matView._21 );
	Planes [ 0 ].Normal.z = -( matView._34 + matView._31 );
	Planes [ 0 ].Distance = -( matView._44 + matView._41 );

	// right clipping plane
	Planes [ 1 ].Normal.x = -( matView._14 - matView._11 );
	Planes [ 1 ].Normal.y = -( matView._24 - matView._21 );
	Planes [ 1 ].Normal.z = -( matView._34 - matView._31 );
	Planes [ 1 ].Distance = -( matView._44 - matView._41 );

	// top clipping plane
	Planes [ 2 ].Normal.x = -( matView._14 - matView._12 );
	Planes [ 2 ].Normal.y = -( matView._24 - matView._22 );
	Planes [ 2 ].Normal.z = -( matView._34 - matView._32 );
	Planes [ 2 ].Distance = -( matView._44 - matView._42 );

	// bottom clipping plane
	Planes [ 3 ].Normal.x = -( matView._14 + matView._12 );
	Planes [ 3 ].Normal.y = -( matView._24 + matView._22 );
	Planes [ 3 ].Normal.z = -( matView._34 + matView._32 );
	Planes [ 3 ].Distance = -( matView._44 + matView._42 );

	// near clipping plane
	Planes [ 4 ].Normal.x = -( matView._14 + matView._13 );
	Planes [ 4 ].Normal.y = -( matView._24 + matView._23 );
	Planes [ 4 ].Normal.z = -( matView._34 + matView._33 );
	Planes [ 4 ].Distance = -( matView._44 + matView._43 );

	// far clipping plane
	Planes [ 5 ].Normal.x = -( matView._14 - matView._13 );
	Planes [ 5 ].Normal.y = -( matView._24 - matView._23 );
	Planes [ 5 ].Normal.z = -( matView._34 - matView._33 );
	Planes [ 5 ].Distance = -( matView._44 - matView._43 );

	for ( int i = 0; i < 6; i++ )
	{
		// get magnitude of vector
		float denom = 1.0f / D3DXVec3Length ( &Planes [ i ].Normal );

		Planes [ i ].Normal.x *= denom;
		Planes [ i ].Normal.y *= denom;
		Planes [ i ].Normal.z *= denom;
		Planes [ i ].Distance *= denom;
	}
	*/
}

BOOL LeafInFrustum ( long Leaf )
{
	D3DXVECTOR3		bMax = LeafArray [ Leaf ].BoundingBox.BoxMax;
	D3DXVECTOR3		bMin = LeafArray [ Leaf ].BoundingBox.BoxMin;

	D3DXVECTOR3		NearPoint;
	D3DXVECTOR3		FarPoint;

	PLANE2*			Plane = FrustumPlanes;
	
	for ( int i = 0; i < 6; i++ )
	{
		if ( Plane->Normal.x > 0.0f )
		{
			if ( Plane->Normal.y > 0.0f )
			{
				if ( Plane->Normal.z > 0.0f )
				{
					NearPoint.x = bMin.x;
					NearPoint.y = bMin.y;
					NearPoint.z = bMin.z;
				}
				else
				{
					NearPoint.x = bMin.x;
					NearPoint.y = bMin.y;
					NearPoint.z = bMax.z;
				}
			} 
			else
			{
				if ( Plane->Normal.z > 0.0f ) 
				{
					NearPoint.x = bMin.x;
					NearPoint.y = bMax.y;
					NearPoint.z = bMin.z;
				} 
				else
				{
					NearPoint.x = bMin.x;
					NearPoint.y = bMax.y; 
					NearPoint.z = bMax.z;
				}
			}
		} 
		else 
		{
			if ( Plane->Normal.y > 0.0f )
			{
				if ( Plane->Normal.z > 0.0f )
				{
					NearPoint.x = bMax.x;
					NearPoint.y = bMin.y; 
					NearPoint.z = bMin.z;
				}
				else 
				{
					NearPoint.x = bMax.x;
					NearPoint.y = bMin.y;
					NearPoint.z = bMax.z;
				}
			} 
			else
			{
				if ( Plane->Normal.z > 0.0f )
				{
					NearPoint.x = bMax.x; 
					NearPoint.y = bMax.y;
					NearPoint.z = bMin.z;
				}
				else
				{
					NearPoint.x = bMax.x;
					NearPoint.y = bMax.y;
					NearPoint.z = bMax.z;
				}
			}
		}

		// near extreme point is outside, and thus
		// the AABB is totally outside the polyhedron
		if ( D3DXVec3Dot ( &Plane->Normal, &NearPoint ) + Plane->Distance > 0 )
			return FALSE;

		Plane++;
	}
	
	return TRUE;
}

void DeletePolygonArray ( void )
{
	for ( long i = 0; i < NumberOfPolygons; i++ )
	{
		delete PolygonArray [ i ].VertexList;
		delete PolygonArray [ i ].Indices;
	}

	if ( NumberOfPolygons )
		free ( PolygonArray );

	NumberOfPolygons = 0;
}


void LoadTextures ( void )
{
	char TextureName [ 25 ];
	
	lpTextureSurface    = new LPDIRECT3DTEXTURE9 [ NumberOfTextures ];
	pTexturePolygonList = new POLYGON*           [ NumberOfTextures ];

	ZeroMemory ( lpTextureSurface, NumberOfTextures * sizeof ( LPDIRECT3DTEXTURE9 ) );

	for ( UINT i = 0; i < NumberOfTextures; i++ )
	{
		strcpy ( TextureName, TextureLUT [ i ] );


		D3DXCreateTextureFromFile ( m_pD3D, TextureName, &lpTextureSurface [ i ] );
	}
}

void SetupCullingCamera ( int iID )
{
	gCurrentCamera=iID;
}

void SetupCollisionOff ( int iID )
{
	if ( gCollisionSwitch [ iID ]==true )
	{
		gCollisionSwitch [ iID ] = false;
		SetOldPos ( iID );
	}
}

void SetCollisionThreshhold ( int iID, float fSensitivity )
{
	// stop that slow sliding gravity decrement - if Ymod less than this senitivity value!
}

void SetCollisionHeightAdjustment ( int iID, float fHeight )
{
	gCollision [ iID ].fAdjustY = fHeight;
	SetOldPos ( iID );
}
