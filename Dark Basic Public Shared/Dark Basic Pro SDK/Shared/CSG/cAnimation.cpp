//////////////////////////////////////////////////////////////////////////////////
// LOGS //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*

01/05/02	0928	mj	created new parameter for load model data, this allows us to
						switch animation loading on / off
01/05/02	0934	mj	defaults to object material only when no texture, otherwise
						uses a full material ( all white )
*/
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Common Includes
#include "cAnimation.h"
#include <stdio.h>
#include "rmxfguid.h"
#include "rmxftmpl.h"

// external functions
typedef bool ( *IMAGE_LoadDirectPFN )  ( char* szFilename, LPDIRECT3DTEXTURE8* pImage );
extern IMAGE_LoadDirectPFN g_Image_LoadDirect;

// externals
extern D3DMATERIAL8 gWhiteMaterial;

cAnimation::cAnimation ( )
{
	m_NumAnimations = 0;
	m_AnimationSet  = NULL;
}

cAnimation::~cAnimation ( )
{
	Free ( );
}

BOOL cAnimation::Load ( char* Filename, sFrame* pFrame )
{
	IDirectXFile*			pDXFile = NULL;
	IDirectXFileEnumObject*	pDXEnum = NULL;
	IDirectXFileData*		pDXData = NULL;
  
	m_pFrame = pFrame;

	// free a prior animation
	Free ( );

	// error checking
	if ( Filename == NULL )
		return FALSE;

	// create the file object
	if ( FAILED ( DirectXFileCreate ( &pDXFile ) ) )
		return FALSE;

	// register the templates
	if ( FAILED ( pDXFile->RegisterTemplates ( ( LPVOID ) D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES ) ) )
	{
		pDXFile->Release ( );
		return FALSE;
	}

	// create an enumeration object
	if ( FAILED ( pDXFile->CreateEnumObject ( ( LPVOID ) Filename, DXFILELOAD_FROMFILE, &pDXEnum ) ) )
	{
		pDXFile->Release ( );
		return FALSE;
	}

	// loop through all objects looking for the animation
	while ( SUCCEEDED ( pDXEnum->GetNextDataObject ( &pDXData ) ) )
	{
		ParseXFileData ( pDXData, NULL, NULL );
		SAFE_RELEASE ( pDXData );
	}

	SAFE_RELEASE ( pDXEnum );
	SAFE_RELEASE ( pDXFile );

	MapToMesh ( );

	return TRUE;
}

void cAnimation::ParseXFileData ( IDirectXFileData *pDataObj, sAnimationSet *ParentAnim, sAnimation *CurrentAnim )
{
	IDirectXFileObject*			pSubObj  = NULL;
	IDirectXFileData*			pSubData = NULL;
	IDirectXFileDataReference*	pDataRef = NULL;
	const GUID*					Type     = NULL;
	char*						Name     = NULL;
	DWORD						Size;
	PBYTE*						DataPtr;

	DWORD						i;

	DWORD						KeyType,
								NumKeys, 
								Time;

	sXFileRotateKey*			RotKey;
	sXFileScaleKey*				ScaleKey;
	sXFilePositionKey*			PosKey;
	sXFileMatrixKey*			MatKey;

	sAnimationSet*				SubAnimSet = NULL;
	sAnimation*					SubAnim    = NULL;

	sAnimation*					Anim    = NULL;
	sAnimationSet*				AnimSet = NULL;
  
	// get the template type
	if ( FAILED ( pDataObj->GetType ( &Type ) ) )
		return;

	// get the template name ( if any )
	if ( FAILED ( pDataObj->GetName ( NULL, &Size ) ) )
		return;

	if ( Size )
	{
		if ( ( Name = new char [ Size ] ) != NULL )
			pDataObj->GetName ( Name, &Size );
	}

	// give template a default name if none found
	if ( Name == NULL )
	{
		if ( ( Name = new char [ 9 ] ) == NULL )
			return;
		
		strcpy ( Name, "$NoName$" );
	}

	// set sub frame parent
	SubAnimSet = ParentAnim;
	SubAnim    = CurrentAnim;

	// process the templates

	// process an animation set
	if ( *Type == TID_D3DRMAnimationSet )
	{
		// create an animation set structure
		if ( ( AnimSet = new sAnimationSet ( ) ) == NULL )
		{
			delete [ ] Name;
			return;
		}

		// set the name
		AnimSet->m_Name = Name;
		Name            = NULL;

		// link into the animation set list
		AnimSet->m_Next = m_AnimationSet;
		m_AnimationSet  = AnimSet;

		// set as new parent
		SubAnimSet = AnimSet;
	}

	// process an animation
	if ( *Type == TID_D3DRMAnimation && ParentAnim != NULL ) 
	{
		// create an animation structure
		if ( ( Anim = new sAnimation ( ) ) == NULL )
		{
			delete [ ] Name;
			return;
		}

		// set the name
		Anim->m_Name = Name;
		Name         = NULL;

		// link into the animation list
		Anim->m_Next            = ParentAnim->m_Animation;
		ParentAnim->m_Animation = Anim;

		SubAnim = Anim;
	}

	// process an animation key
	if ( *Type == TID_D3DRMAnimationKey && CurrentAnim != NULL )
	{
		// load in this animation's key data
		if ( FAILED ( pDataObj->GetData ( NULL, &Size, ( PVOID* ) &DataPtr ) ) )
		{
			delete [ ] Name;
			return;
		}

		KeyType = ( ( DWORD* ) DataPtr ) [ 0 ];
		NumKeys = ( ( DWORD* ) DataPtr ) [ 1 ];

		switch ( KeyType )
		{
			case 0:
			{
				delete [ ] CurrentAnim->m_RotateKeys;

				if ( ( CurrentAnim->m_RotateKeys = new sRotateKey [ NumKeys ] ) == NULL )
				{
					delete [ ] Name;
					return;
				}

				CurrentAnim->m_NumRotateKeys = NumKeys;
				RotKey                       = ( sXFileRotateKey* ) ( ( char* ) DataPtr + ( sizeof ( DWORD ) * 2 ) );
				
				for ( i = 0; i < NumKeys; i++ )
				{
					CurrentAnim->m_RotateKeys [ i ].Time         = RotKey->Time;
					CurrentAnim->m_RotateKeys [ i ].Quaternion.x = -RotKey->x;
					CurrentAnim->m_RotateKeys [ i ].Quaternion.y = -RotKey->y;
					CurrentAnim->m_RotateKeys [ i ].Quaternion.z = -RotKey->z;
					CurrentAnim->m_RotateKeys [ i ].Quaternion.w =  RotKey->w;

					if ( RotKey->Time > ParentAnim->m_Length )
						ParentAnim->m_Length = RotKey->Time;

					RotKey += 1;
				}
			}
			break;

			case 1:
			{
				delete [ ] CurrentAnim->m_ScaleKeys;

				if ( ( CurrentAnim->m_ScaleKeys = new sScaleKey [ NumKeys ] ) == NULL )
					return;

				CurrentAnim->m_NumScaleKeys = NumKeys;
				ScaleKey                    = ( sXFileScaleKey* ) ( ( char* ) DataPtr + ( sizeof ( DWORD ) * 2 ) );
				
				for ( i = 0; i < NumKeys; i++ )
				{
					CurrentAnim->m_ScaleKeys [ i ].Time  = ScaleKey->Time;
					CurrentAnim->m_ScaleKeys [ i ].Scale = ScaleKey->Scale;

					if ( ScaleKey->Time > ParentAnim->m_Length )
						ParentAnim->m_Length = ScaleKey->Time;

					ScaleKey += 1;
				}

				// calculate the interpolation values
				if ( NumKeys > 1 )
				{
					for ( i = 0; i < NumKeys - 1; i++ )
					{
						CurrentAnim->m_ScaleKeys [ i ].ScaleInterpolation = CurrentAnim->m_ScaleKeys [ i + 1 ].Scale - CurrentAnim->m_ScaleKeys [ i ].Scale;
						Time                                              = CurrentAnim->m_ScaleKeys [ i + 1 ].Time  - CurrentAnim->m_ScaleKeys [ i ].Time;
						
						if ( !Time )
							Time = 1;

						CurrentAnim->m_ScaleKeys [ i ].ScaleInterpolation /= ( float ) Time;
					}
				}
			}
			break;
    
			case 2:
			{
				delete [ ] CurrentAnim->m_PositionKeys;

				if ( ( CurrentAnim->m_PositionKeys = new sPositionKey [ NumKeys ] ) == NULL )
				{
					delete [ ] Name;
					return;
				}

				CurrentAnim->m_NumPositionKeys = NumKeys;
				PosKey                         = ( sXFilePositionKey* ) ( ( char* ) DataPtr + ( sizeof ( DWORD ) * 2 ) );
				
				for ( i = 0; i < NumKeys; i++ )
				{
					CurrentAnim->m_PositionKeys [ i ].Time = PosKey->Time;
					CurrentAnim->m_PositionKeys [ i ].Pos  = PosKey->Pos;

					if ( PosKey->Time > ParentAnim->m_Length )
						ParentAnim->m_Length = PosKey->Time;

					PosKey += 1;
				}

				// calculate the interpolation values
				if ( NumKeys > 1 )
				{
					for(  i = 0; i < NumKeys - 1; i++ )
					{
						CurrentAnim->m_PositionKeys [ i ].PosInterpolation = CurrentAnim->m_PositionKeys [ i + 1 ].Pos  - CurrentAnim->m_PositionKeys [ i ].Pos;
						Time                                               = CurrentAnim->m_PositionKeys [ i + 1 ].Time - CurrentAnim->m_PositionKeys [ i ].Time;
						
						if ( !Time )
							Time = 1;

						CurrentAnim->m_PositionKeys [ i ].PosInterpolation /= ( float ) Time;
					}
				}
			}
			break;

			case 4:
			{
				delete [ ] CurrentAnim->m_MatrixKeys;

				if ( ( CurrentAnim->m_MatrixKeys = new sMatrixKey [ NumKeys ] ) == NULL )
				{
					delete [ ] Name;
					return;
				}

				CurrentAnim->m_NumMatrixKeys = NumKeys;
				MatKey                       = ( sXFileMatrixKey* ) ( ( char* ) DataPtr + ( sizeof ( DWORD ) * 2 ) );

				for ( i = 0; i < NumKeys; i++ )
				{
					CurrentAnim->m_MatrixKeys [ i ].Time   = MatKey->Time;
					CurrentAnim->m_MatrixKeys [ i ].Matrix = MatKey->Matrix;

					if ( MatKey->Time > ParentAnim->m_Length )
						ParentAnim->m_Length = MatKey->Time;

					MatKey += 1;
				}

				// calculate the interpolation matrices
				if ( NumKeys > 1 )
				{
					for ( i = 0; i < NumKeys - 1; i++ )
					{
						CurrentAnim->m_MatrixKeys [ i ].MatInterpolation = CurrentAnim->m_MatrixKeys [ i + 1 ].Matrix - CurrentAnim->m_MatrixKeys [ i ].Matrix;
						Time                                             = CurrentAnim->m_MatrixKeys [ i + 1 ].Time   - CurrentAnim->m_MatrixKeys [ i ].Time;

						if ( !Time )
							Time = 1;

						CurrentAnim->m_MatrixKeys [ i ].MatInterpolation /= ( float ) Time;
					}
				}
			}
			break;
		}
	}

	// process animation options
	if ( *Type == TID_D3DRMAnimationOptions && CurrentAnim != NULL )
	{
		// load in this animation's options
		if ( FAILED ( pDataObj->GetData ( NULL, &Size, ( PVOID* ) &DataPtr ) ) )
		{
			delete [ ] Name;
			return;
		}

		// process looping information
		if ( !( ( DWORD* ) DataPtr ) [ 0 ] )
		{
			CurrentAnim->m_Loop = TRUE;
		}
		else
		{
			CurrentAnim->m_Loop = FALSE;
		}

		// process linear information
		if ( !( ( DWORD* ) DataPtr ) [ 1 ] )
		{
			CurrentAnim->m_Linear = FALSE;
		}
		else
		{
			CurrentAnim->m_Linear = TRUE;
		}
	}

	// process a frame reference
	if ( *Type == TID_D3DRMFrame && CurrentAnim != NULL )
	{
		CurrentAnim->m_FrameName = Name;
		Name                     = NULL;

		// don't enumerate child templates
		return;
	}

	// release name buffer
	delete [ ] Name;

	// scan for embedded templates
	while ( SUCCEEDED ( pDataObj->GetNextObject ( &pSubObj ) ) )
	{
		// process embedded references
		if ( SUCCEEDED ( pSubObj->QueryInterface ( IID_IDirectXFileDataReference, ( void** ) &pDataRef ) ) ) 
		{
			if ( SUCCEEDED ( pDataRef->Resolve ( &pSubData ) ) )
			{
				ParseXFileData ( pSubData, SubAnimSet, SubAnim );
				SAFE_RELEASE ( pSubData );
			}
		
			SAFE_RELEASE ( pDataRef );
		}

		// process non - referenced embedded templates
		if ( SUCCEEDED ( pSubObj->QueryInterface ( IID_IDirectXFileData, ( void** ) &pSubData ) ) )
		{
			ParseXFileData ( pSubData, SubAnimSet, SubAnim );
			SAFE_RELEASE ( pSubData );
		}

		SAFE_RELEASE ( pSubObj );
	}
}

BOOL cAnimation::Free ( )
{
	SAFE_DELETE (  m_AnimationSet );

	m_AnimationSet  = NULL;
	m_NumAnimations = 0;

	return TRUE;
}

BOOL cAnimation::IsLoaded ( )
{
	if ( m_AnimationSet == NULL )
		return FALSE;

	return TRUE;
}

BOOL cAnimation::MapToMesh ( )
{
	sAnimationSet* AnimSet;
	sAnimation*    Anim;

	// assign links to frames by name
	if ( ( AnimSet = m_AnimationSet ) == NULL )
		return FALSE;

	// scan through all animation sets
	while ( AnimSet != NULL )
	{
		// scan through all animations
		Anim = AnimSet->m_Animation;

		while ( Anim != NULL )
		{
			// find the matching frame from mesh
			Anim->m_Frame = m_pFrame->FindFrame ( Anim->m_FrameName );
			Anim          = Anim->m_Next;
		}
	
		AnimSet = AnimSet->m_Next;
	}

	return TRUE;
}

long cAnimation::GetNumAnimations ( )
{
	return m_NumAnimations;
}

sAnimationSet* cAnimation::GetAnimationSet ( char* Name )
{
	if ( m_AnimationSet == NULL )
		return NULL;

	return m_AnimationSet->FindSet ( Name );
}

BOOL cAnimation::SetLoop ( BOOL ToLoop, char* Name )
{
	sAnimationSet* AnimSet;
	sAnimation*    Anim;

	if ( ( AnimSet = GetAnimationSet ( Name ) ) == NULL )
		return FALSE;

	Anim = AnimSet->m_Animation;

	while ( Anim != NULL )
	{
		Anim->m_Loop = ToLoop;
		Anim         = Anim->m_Next;
	}

	return TRUE;
}

unsigned long cAnimation::GetLength ( char* Name )
{
	sAnimationSet* AnimSet;

	if ( ( AnimSet = GetAnimationSet ( Name ) ) == NULL )
		return 0;

	return AnimSet->m_Length;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int g_iGlobalLimbCount = 0;

bool LoadModelData ( sObjectData* pData, char* szFilename, char* TexturePath, bool bAnim )
{
	IDirectXFile*			pDXFile   = NULL;
	IDirectXFileEnumObject* pDXEnum   = NULL;
	IDirectXFileData*		pDXData   = NULL;
	sFrame*					TempFrame = NULL;
	sFrame*					FramePtr  = NULL;
	sMesh*					Mesh;
	D3DXMATRIX*				Matrix;
	DWORD                   i;

	// create the file object
	if ( FAILED ( DirectXFileCreate ( &pDXFile ) ) )
		return FALSE;

	// register the templates
	if ( FAILED ( pDXFile->RegisterTemplates ( ( LPVOID ) D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES ) ) )
	{
		pDXFile->Release ( );
		return FALSE;
	}

	// create an enumeration object
	if ( FAILED ( pDXFile->CreateEnumObject ( ( LPVOID ) szFilename, DXFILELOAD_FROMFILE, &pDXEnum ) ) )
	{
		pDXFile->Release ( );
		return FALSE;
	}

	// create a temporary frame
	TempFrame = new sFrame ( );

	// loop through all objects looking for the frames and meshes
	while ( SUCCEEDED ( pDXEnum->GetNextDataObject ( &pDXData ) ) )
	{
		ParseXFileData ( pData, pDXData, TempFrame, TexturePath );
		SAFE_RELEASE ( pDXData );
	}

//	// release used COM objects
//	SAFE_RELEASE ( pDXEnum );
//	SAFE_RELEASE ( pDXFile );
//	SAFE_DELETE( TempFrame );
//	return false;

	// see if we should keep the tempframe as root
	if ( TempFrame->m_MeshList != NULL )
	{
		pData->m_Frames         = TempFrame;
		pData->m_Frames->m_Name = new char [ 7 ];
	
		strcpy ( pData->m_Frames->m_Name, "%ROOT%" );
	}
	else 
	{
		// assign the root frame and release temporary frame
		pData->m_Frames = TempFrame->m_Child;
		FramePtr        = pData->m_Frames;

		while ( FramePtr != NULL )
		{
			FramePtr->m_Parent = NULL;
			FramePtr           = FramePtr->m_Sibling;
		}

		TempFrame->m_Child = NULL;
		delete TempFrame;
	}

	// match frames to bones ( for matrices )
	MapFramesToBones ( pData, pData->m_Frames );

	// generate default mesh ( if contains a skin mesh )
	if ( ( Mesh = pData->m_Meshes ) != NULL )
	{
		while ( Mesh != NULL )
		{
			if ( Mesh->m_SkinMesh != NULL )
			{
				Matrix = new D3DXMATRIX [ Mesh->m_NumBones ];

				for ( i = 0; i < Mesh->m_NumBones; i++ )
					D3DXMatrixIdentity ( &Matrix [ i ] );

				Mesh->m_SkinMesh->UpdateSkinnedMesh ( Matrix, NULL, Mesh->m_Mesh );
				delete [ ] Matrix;
			}

			// Loaded meshes are always lists
			Mesh->m_iPrimType = D3DPT_TRIANGLELIST;

			Mesh = Mesh->m_Next;
		}
	}

	// calculate bounding box and sphere
	if ( ( Mesh = pData->m_Meshes ) != NULL )
	{
		while ( Mesh != NULL )
		{
			pData->m_Min.x  = min ( pData->m_Min.x,  Mesh->m_Min.x  );
			pData->m_Min.y  = min ( pData->m_Min.y,  Mesh->m_Min.y  );
			pData->m_Min.z  = min ( pData->m_Min.z,  Mesh->m_Min.z  );
			pData->m_Max.x  = max ( pData->m_Max.x,  Mesh->m_Max.x  );
			pData->m_Max.y  = max ( pData->m_Max.y,  Mesh->m_Max.y  );
			pData->m_Max.z  = max ( pData->m_Max.z,  Mesh->m_Max.z  );
			pData->m_Radius = max ( pData->m_Radius, Mesh->m_Radius );

			Mesh = Mesh->m_Next;
		}
	}
	
	if ( bAnim )
	{
		pData->m_Animations.Load ( szFilename, pData->m_Frames );
		pData->m_Animations.SetLoop ( TRUE, NULL );
	}

	g_iGlobalLimbCount = 0;

	return true;
}

bool AppendAnimationData ( sObjectData* pData, char* szFilename, int iFrameStart )
{
	// Load in animation from file
	cAnimation* pAnim = new cAnimation;
	pAnim->Load ( szFilename, pData->m_Frames );
	if ( pAnim->m_AnimationSet==NULL ) return false;
	if ( pAnim->m_AnimationSet->m_Animation==NULL ) return false;

	// Get number of new frames
	DWORD dwNewFrames = pAnim->m_AnimationSet->m_Length;

	// Add animation data to original model animation data
	sAnimation* pOrigCurrent = pData->m_Animations.m_AnimationSet->m_Animation;
	sAnimation* pCurrent = pAnim->m_AnimationSet->m_Animation;
	while(pCurrent)
	{
		// ensure animnames match
		if ( pCurrent->m_Name && pOrigCurrent->m_Name )
		{
			if ( stricmp( pCurrent->m_Name, pOrigCurrent->m_Name )==NULL )
			{
				// origianl animation data
				DWORD           dwOrigNumPositionKeys	= pOrigCurrent->m_NumPositionKeys;
				sPositionKey*	pOrigPositionKeys		= pOrigCurrent->m_PositionKeys;
				DWORD           dwOrigNumRotateKeys		= pOrigCurrent->m_NumRotateKeys;
				sRotateKey*		pOrigRotateKeys			= pOrigCurrent->m_RotateKeys;
				DWORD           dwOrigNumScaleKeys		= pOrigCurrent->m_NumScaleKeys;
				sScaleKey*		pOrigScaleKeys			= pOrigCurrent->m_ScaleKeys;

				// new animation data
				DWORD           dwAppNumPositionKeys	= pCurrent->m_NumPositionKeys;
				sPositionKey*	pAppPositionKeys		= pCurrent->m_PositionKeys;
				DWORD           dwAppNumRotateKeys		= pCurrent->m_NumRotateKeys;
				sRotateKey*		pAppRotateKeys			= pCurrent->m_RotateKeys;
				DWORD           dwAppNumScaleKeys		= pCurrent->m_NumScaleKeys;
				sScaleKey*		pAppScaleKeys			= pCurrent->m_ScaleKeys;

				// create new animation arrays
				sPositionKey*	pNewPositionKeys				= NULL;
				DWORD           dwNewNumPositionKeys			= dwOrigNumPositionKeys + dwAppNumPositionKeys;
				if(dwNewNumPositionKeys>0) pNewPositionKeys		= new sPositionKey[dwNewNumPositionKeys];
				sRotateKey*		pNewRotateKeys					= NULL;
				DWORD           dwNewNumRotateKeys				= dwOrigNumRotateKeys + dwAppNumRotateKeys;
				if(dwNewNumRotateKeys>0) pNewRotateKeys			= new sRotateKey[dwNewNumRotateKeys];
				sScaleKey*		pNewScaleKeys					= NULL;
				DWORD           dwNewNumScaleKeys				= dwOrigNumScaleKeys + dwAppNumScaleKeys;
				if(dwNewNumScaleKeys>0) pNewScaleKeys			= new sScaleKey[dwNewNumScaleKeys];
	
				// modify additional data to account for keyframe-start-shift
				DWORD dwTimeOffset = (DWORD)iFrameStart;
				for ( DWORD p=0; p<dwAppNumPositionKeys; p++) pAppPositionKeys[p].Time+=dwTimeOffset;
				for ( DWORD r=0; r<dwAppNumRotateKeys; r++) pAppRotateKeys[r].Time+=dwTimeOffset;
				for ( DWORD s=0; s<dwAppNumScaleKeys; s++) pAppScaleKeys[s].Time+=dwTimeOffset;

				// copy old animation
				if(pNewPositionKeys) memcpy( pNewPositionKeys, pOrigPositionKeys, dwOrigNumPositionKeys*sizeof(sPositionKey) );
				if(pNewRotateKeys) memcpy( pNewRotateKeys, pOrigRotateKeys, dwOrigNumRotateKeys*sizeof(sRotateKey) );
				if(pNewScaleKeys) memcpy( pNewScaleKeys, pOrigScaleKeys, dwOrigNumScaleKeys*sizeof(sScaleKey) );

				// copy appended animation
				LPSTR pMiddleP = (LPSTR)pNewPositionKeys + dwOrigNumPositionKeys*sizeof(sPositionKey);
				if(pNewPositionKeys) memcpy( pMiddleP, pAppPositionKeys, dwAppNumPositionKeys*sizeof(sPositionKey) );
				LPSTR pMiddleR = (LPSTR)pNewRotateKeys + dwOrigNumRotateKeys*sizeof(sRotateKey);
				if(pNewRotateKeys) memcpy( pMiddleR, pAppRotateKeys, dwAppNumRotateKeys*sizeof(sRotateKey) );
				LPSTR pMiddleS = (LPSTR)pNewScaleKeys + dwOrigNumScaleKeys*sizeof(sScaleKey);
				if(pNewScaleKeys) memcpy( pMiddleS, pAppScaleKeys, dwAppNumScaleKeys*sizeof(sScaleKey) );

				// delete original animation arrays
				if(pOrigPositionKeys) SAFE_DELETE_ARRAY(pOrigPositionKeys);
				if(pOrigRotateKeys) SAFE_DELETE_ARRAY(pOrigRotateKeys);
				if(pOrigScaleKeys) SAFE_DELETE_ARRAY(pOrigScaleKeys);

				// assign new ptrs and totals
				pOrigCurrent->m_NumPositionKeys = dwNewNumPositionKeys;
				pOrigCurrent->m_PositionKeys = pNewPositionKeys;
				pOrigCurrent->m_NumRotateKeys = dwNewNumRotateKeys;
				pOrigCurrent->m_RotateKeys = pNewRotateKeys;
				pOrigCurrent->m_NumScaleKeys = dwNewNumScaleKeys;
				pOrigCurrent->m_ScaleKeys = pNewScaleKeys;

				// must tie old and new animations by creating interpolation for penultimate keyframe
				if ( dwNewNumPositionKeys > 1 )
				{
					for( DWORD i = 0; i < dwNewNumPositionKeys - 1; i++ )
					{
						pNewPositionKeys[i].PosInterpolation = pNewPositionKeys[i+1].Pos  - pNewPositionKeys[i].Pos;
						DWORD Time = pNewPositionKeys[i+1].Time  - pNewPositionKeys[i].Time;
						if ( !Time ) Time = 1;
						pNewPositionKeys[i].PosInterpolation/=(float)Time;
					}
				}				
				if ( dwNewNumScaleKeys > 1 )
				{
					for( DWORD i = 0; i < dwNewNumScaleKeys - 1; i++ )
					{
						pNewScaleKeys[i].ScaleInterpolation = pNewScaleKeys[i+1].Scale  - pNewScaleKeys[i].Scale;
						DWORD Time = pNewScaleKeys[i+1].Time  - pNewScaleKeys[i].Time;
						if ( !Time ) Time = 1;
						pNewScaleKeys[i].ScaleInterpolation/=(float)Time;
					}
				}				
			}
		}

		// next anim
		pOrigCurrent=pOrigCurrent->m_Next;
		pCurrent=pCurrent->m_Next;
	}

	// Update total number of frames
	pData->m_Animations.m_AnimationSet->m_Length += dwNewFrames;

	return true;
}

void ParseXFileData ( sObjectData* pData, IDirectXFileData* pDataObj, sFrame* ParentFrame, char* TexturePath )
{
	IDirectXFileObject*			pSubObj  = NULL;
	IDirectXFileData*			pSubData = NULL;
	IDirectXFileDataReference*	pDataRef = NULL;
	const GUID*					Type     = NULL;
	char*						Name     = NULL;
	DWORD						Size;
	sFrame*						SubFrame = NULL;
//	char						Path [ MAX_PATH ];

	sFrame*						Frame       = NULL;
	D3DXMATRIX*					FrameMatrix = NULL;

	sMesh*						Mesh           = NULL;
	ID3DXBuffer*				MaterialBuffer = NULL;
	D3DXMATERIAL*				Materials      = NULL;
	ID3DXBuffer*				Adjacency      = NULL;
	DWORD*						AdjacencyIn    = NULL;
	DWORD*						AdjacencyOut   = NULL;

	DWORD						i;
	BYTE**						Ptr;

	// get the template type
	if ( FAILED ( pDataObj->GetType ( &Type ) ) )
		return;

	// get the template name ( if any )
	if ( FAILED ( pDataObj->GetName ( NULL, &Size ) ) )
		return;

	if ( Size )
	{
		if ( ( Name = new char [ Size ] ) != NULL )
			pDataObj->GetName ( Name, &Size );
	}

	// give template a default name if none found
	if ( Name == NULL )
	{
		if ( ( Name = new char [ 9 ] ) == NULL )
			return;

		strcpy ( Name, "$NoName$" );
	}

	// set sub frame
	SubFrame = ParentFrame;

	// process the templates

	// frame
	if ( *Type == TID_D3DRMFrame )
	{
		// create a new frame structure
		Frame = new sFrame ( );

		// store the name
		Frame->m_Name = Name;
		Name          = NULL;

		Frame->m_iID = g_iGlobalLimbCount++;

		// add to parent frame
		Frame->m_Parent      = ParentFrame;
		Frame->m_Sibling     = ParentFrame->m_Child;
		ParentFrame->m_Child = Frame;

		// increase frame count
		pData->m_NumFrames++;

		// set sub frame parent
		SubFrame = Frame;
	}

	// frame transformation matrix
	if ( *Type == TID_D3DRMFrameTransformMatrix )
	{
		if ( FAILED ( pDataObj->GetData ( NULL, &Size, ( PVOID* ) &FrameMatrix ) ) )
		{
			delete [ ] Name;
			return;
		}
		ParentFrame->m_matOriginal = *FrameMatrix;
	}

	// mesh
	if ( *Type == TID_D3DRMMesh )
	{

		//if ( m_Meshes == NULL || m_Meshes->FindMesh ( Name ) == NULL )
		if ( 1 )
		{
			// create a new mesh structure
			Mesh = new sMesh ( );

			// store the name
			Mesh->m_Name = Name;
			Name         = NULL;

			// load mesh data
			if ( FAILED ( D3DXLoadSkinMeshFromXof ( 
														pDataObj, 
														0,
														m_pD3D, 
														&Adjacency,
														&MaterialBuffer,
														&Mesh->m_NumMaterials, 
														&Mesh->m_BoneNames,
														&Mesh->m_BoneTransforms,
														&Mesh->m_SkinMesh
												  ) ) )
			{
				delete [ ] Name;
				delete Mesh;
				return;
			}

			// calculate the bounding box and sphere
			if ( SUCCEEDED ( Mesh->m_SkinMesh->LockVertexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &Ptr ) ) )
			{
				// create a bounding box
				D3DXComputeBoundingBox ( 
											( void* ) Ptr,
											Mesh->m_SkinMesh->GetNumVertices ( ),
											Mesh->m_SkinMesh->GetFVF ( ),
											&Mesh->m_Min, 
											&Mesh->m_Max
									);

				// create a bounding sphere
				D3DXComputeBoundingSphere (
											( void* ) Ptr,
											Mesh->m_SkinMesh->GetNumVertices ( ),
											Mesh->m_SkinMesh->GetFVF ( ),
											&D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f ),
											&Mesh->m_Radius
										  );
			
				Mesh->m_SkinMesh->UnlockVertexBuffer ( );

			}

			// convert to regular mesh if no bones
			if ( !( Mesh->m_NumBones = Mesh->m_SkinMesh->GetNumBones ( ) ) ) 
			{
				// make clones of mesh
				Mesh->m_SkinMesh->GetOriginalMesh ( &Mesh->m_Mesh );
//				Mesh->m_SkinMesh->GetOriginalMesh ( &pData->m_MainMesh );

				// convert mesh
				LPD3DXMESH pNewMesh = NULL;
				Mesh->m_Mesh->CloneMeshFVF ( 0, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, m_pD3D, &pNewMesh );
				SAFE_RELEASE(Mesh->m_Mesh);
				Mesh->m_Mesh = pNewMesh;

				LPD3DXMESH pNewOptMesh = NULL;
				Mesh->m_Mesh->Optimize ( D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL, &pNewOptMesh );
				SAFE_RELEASE(Mesh->m_Mesh);
				Mesh->m_Mesh = pNewOptMesh;

				Mesh->m_Mesh->GetAttributeTable ( NULL, &Mesh->m_dwAttributeTableSize );
				Mesh->m_pAttributeTable = new D3DXATTRIBUTERANGE [ Mesh->m_dwAttributeTableSize ];
				Mesh->m_Mesh->GetAttributeTable ( Mesh->m_pAttributeTable, NULL );

				//pData->m_dwFVFSize = ( sizeof ( float ) * 3 ) + ( sizeof ( float ) * 3 ) + ( sizeof ( float ) * 2 );
				
				SAFE_RELEASE ( Mesh->m_SkinMesh );

				DWORD dwNumVertices = Mesh->m_Mesh->GetNumVertices ( );
				DWORD dwNumFaces    = Mesh->m_Mesh->GetNumFaces    ( );

				pData->m_dwFVF = Mesh->m_Mesh->GetFVF ( );

				pData->m_dwFVFSize = 0;

				if ( pData->m_dwFVF & D3DFVF_XYZ )
					pData->m_dwFVFSize += sizeof ( float ) * 3;

				if ( pData->m_dwFVF & D3DFVF_NORMAL )
					pData->m_dwFVFSize += sizeof ( float ) * 3;

				if ( pData->m_dwFVF & D3DFVF_DIFFUSE )
					pData->m_dwFVFSize += sizeof ( DWORD );

				if ( pData->m_dwFVF & D3DFVF_TEX1 )
					pData->m_dwFVFSize += sizeof ( float ) * 2;
			} 
			else 
			{
				// create the bone matrix array and clear it out
				Mesh->m_Matrices = new D3DXMATRIX [ Mesh->m_NumBones ];
				
				for ( i = 0; i <Mesh->m_NumBones; i++ )
					D3DXMatrixIdentity ( &Mesh->m_Matrices [ i ] );

				// create the frame mapping matrix array and clear out
				Mesh->m_FrameMatrices = new D3DXMATRIX* [ Mesh->m_NumBones ];

				for ( i = 0; i < Mesh->m_NumBones; i++ )
					Mesh->m_FrameMatrices [ i ] = NULL;

				// get a pointer to bone matrices
				Mesh->m_BoneMatrices = ( D3DXMATRIX* ) Mesh->m_BoneTransforms->GetBufferPointer ( );

				AdjacencyIn  = ( DWORD* ) Adjacency->GetBufferPointer ( );
				AdjacencyOut = new DWORD [ Mesh->m_SkinMesh->GetNumFaces ( ) * 3 ];

				// generate the skin mesh object
				if ( FAILED ( Mesh->m_SkinMesh->GenerateSkinnedMesh ( 0, 0.0f, AdjacencyIn, AdjacencyOut, NULL, NULL, &Mesh->m_Mesh ) ) )
				{
					// convert to a regular mesh if error
					Mesh->m_SkinMesh->GetOriginalMesh ( &Mesh->m_Mesh );
					SAFE_RELEASE ( Mesh->m_SkinMesh );
					Mesh->m_NumBones = 0;
				}
				
				// get the attribute information for drawing
				ID3DXMesh*	pMesh;

				// get the original mesh
				Mesh->m_SkinMesh->GetOriginalMesh ( &pMesh );

				// sort the mesh and get the attribute table
				LPD3DXMESH pNewOptMesh = NULL;
				pMesh->Optimize ( D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL, &pNewOptMesh );
				SAFE_RELEASE(pMesh);
				pMesh=pNewOptMesh;

				// setup memory to hold the table
				pMesh->GetAttributeTable ( NULL, &Mesh->m_dwAttributeTableSize );
				Mesh->m_pAttributeTable = new D3DXATTRIBUTERANGE [ Mesh->m_dwAttributeTableSize ];
				pMesh->GetAttributeTable ( Mesh->m_pAttributeTable, NULL );

				// release the temporary mesh
				SAFE_RELEASE ( pMesh );

				delete [ ] AdjacencyOut;
				SAFE_RELEASE ( Adjacency );

				pData->m_dwFVF = Mesh->m_SkinMesh->GetFVF ( );

				pData->m_dwFVFSize = 0;

				if ( pData->m_dwFVF & D3DFVF_XYZ )
					pData->m_dwFVFSize += sizeof ( float ) * 3;

				if ( pData->m_dwFVF & D3DFVF_NORMAL )
					pData->m_dwFVFSize += sizeof ( float ) * 3;

				if ( pData->m_dwFVF & D3DFVF_DIFFUSE )
					pData->m_dwFVFSize += sizeof ( DWORD );

				if ( pData->m_dwFVF & D3DFVF_TEX1 )
					pData->m_dwFVFSize += sizeof ( float ) * 2;
			}

			// Loaded meshes are always lists
			Mesh->m_iPrimType = D3DPT_TRIANGLELIST;

			// load materials or create a default one if none
			if ( !Mesh->m_NumMaterials )
			{
				// create a default one
				Mesh->m_Materials = new D3DMATERIAL8       [ 1 ];
				Mesh->m_Textures  = new LPDIRECT3DTEXTURE8 [ 1 ];

				ZeroMemory ( Mesh->m_Materials, sizeof ( D3DMATERIAL8 ) );
				Mesh->m_Materials [ 0 ].Diffuse.r = 1.0f;
				Mesh->m_Materials [ 0 ].Diffuse.g = 1.0f;
				Mesh->m_Materials [ 0 ].Diffuse.b = 1.0f;
				Mesh->m_Materials [ 0 ].Diffuse.a = 1.0f;
				Mesh->m_Materials [ 0 ].Ambient   = Mesh->m_Materials [ 0 ].Diffuse;
				Mesh->m_Materials [ 0 ].Specular  = Mesh->m_Materials [ 0 ].Diffuse;
				Mesh->m_Textures  [ 0 ]           = NULL;

				Mesh->m_NumMaterials = 1;
			} 
			else 
			{
				// load the materials
				Materials = ( D3DXMATERIAL* ) MaterialBuffer->GetBufferPointer ( );

				Mesh->m_Materials = new D3DMATERIAL8       [ Mesh->m_NumMaterials ];
				Mesh->m_Textures  = new LPDIRECT3DTEXTURE8 [ Mesh->m_NumMaterials ];

				for ( i = 0; i < Mesh->m_NumMaterials; i++ )
				{
					Mesh->m_Materials [ i ] = Materials [ i ].MatD3D;
					Mesh->m_Materials [ i ].Ambient = Mesh->m_Materials [ i ].Diffuse;
					Mesh->m_Textures [ i ] = 0;

					// if texture file exists, try different loads...
					if(Materials [ i ].pTextureFilename)
					{
						strcpy ( Mesh->m_TextureName, Materials [ i ].pTextureFilename );

						// get filename only
						char pNoPath [ _MAX_PATH ];
						strcpy(pNoPath, Materials [ i ].pTextureFilename);
						strrev(pNoPath);
						for(DWORD n=0; n<strlen(pNoPath); n++)
							if(pNoPath[n]=='\\' || pNoPath[n]=='/')
								pNoPath[n]=0;
						strrev(pNoPath);

						// build a texture path and load it
						char Path [ 256 ];
						sprintf ( Path, "%s\\%s", TexturePath, pNoPath );

						/*
						// CSG comment out
						// texture load a : default file
						if ( g_Image_LoadDirect(Path, &Mesh->m_Textures[i])==false )
						{
							// texture load b : file as DDS
							char pDDSFile [ _MAX_PATH ];
							strcpy(pDDSFile, pNoPath);
							DWORD dwLenDot = strlen(pDDSFile);
							pDDSFile[dwLenDot-4]=0;
							strcat(pDDSFile, ".dds");
							sprintf ( Path, "%s\\%s", TexturePath, pDDSFile );
							if ( g_Image_LoadDirect(Path, &Mesh->m_Textures[i])==false )
							{
								// texture load c : original file
								if ( g_Image_LoadDirect(Materials [ i ].pTextureFilename, &Mesh->m_Textures[i])==false )
								{
									// texture load d : no path just file
									if ( g_Image_LoadDirect(pNoPath, &Mesh->m_Textures[i])==false )
									{
										// if this doesn't work then set the texture to null
										Mesh->m_Textures [ i ] = NULL;
									}
								}
							}
						}
						*/
					}
					else
					{
						// if this doesn't work then set the texture to null
						Mesh->m_Textures [ i ] = NULL;
					}
				}
			}

			SAFE_RELEASE ( Adjacency );
			SAFE_RELEASE ( MaterialBuffer );

			// link in mesh
			Mesh->m_Next    = pData->m_Meshes;
			pData->m_Meshes = Mesh;
			pData->m_NumMeshes++;
		}
		else
		{
			// find mesh in list
			Mesh = pData->m_Meshes->FindMesh ( Name );
		}

		// add mesh to frame 
		if ( Mesh != NULL )
			ParentFrame->AddMesh ( Mesh );
	}

	// skip animation sets and animations
	if ( *Type == TID_D3DRMAnimationSet || *Type == TID_D3DRMAnimation || *Type == TID_D3DRMAnimationKey )
	{
		delete [ ] Name;
		return;
	}

	// release name buffer
	delete [ ] Name;

	// scan for embedded templates
	while ( SUCCEEDED ( pDataObj->GetNextObject ( &pSubObj ) ) )
	{
		// process embedded references
		if ( SUCCEEDED ( pSubObj->QueryInterface ( IID_IDirectXFileDataReference, ( void** ) &pDataRef ) ) )
		{
			if ( SUCCEEDED ( pDataRef->Resolve ( &pSubData ) ) )
			{
				ParseXFileData ( pData, pSubData, SubFrame, TexturePath );
				SAFE_RELEASE ( pSubData );
			}

			SAFE_RELEASE ( pDataRef );
		}

		// process non - referenced embedded templates
		if ( SUCCEEDED ( pSubObj->QueryInterface ( IID_IDirectXFileData, ( void** ) &pSubData ) ) )
		{
			ParseXFileData ( pData, pSubData, SubFrame, TexturePath );
			SAFE_RELEASE ( pSubData );
		}

		SAFE_RELEASE ( pSubObj );
	}
}

float* CreatePureTriangleMeshData( float* pMeshDataRaw, DWORD* pdwVertsSoFar, DWORD dwNumVert, DWORD dwFVFSize, WORD* pIndiceData, DWORD dwNumPoly )
{
	// prepare temp mesh data array
	
	DWORD dwVertMax = dwNumVert;
	float* pTempMeshData = (float*)new char[dwVertMax*dwFVFSize];
	float* pMeshDataPtr = pTempMeshData;

	// Run through indice data
	DWORD dwIndex = 0;
	while(dwIndex<dwNumPoly*3)
	{
		// determine size to copy and whether temp is bg enough
		if ( *pdwVertsSoFar > dwVertMax - 32 )
		{
			// Resize temp meshdata
			DWORD dwNewVertMax = dwVertMax*2;
			float* pNewTemp = (float*)new char[dwNewVertMax*dwFVFSize];
			memcpy( pNewTemp, pTempMeshData, dwVertMax*dwFVFSize );
			SAFE_DELETE(pTempMeshData);
			pTempMeshData=pNewTemp;
			pMeshDataPtr=pTempMeshData+(*pdwVertsSoFar*(dwFVFSize/4));
			dwVertMax=dwNewVertMax;
		}

		// read index from poly
		WORD wA = pIndiceData[dwIndex++];
		WORD wB = pIndiceData[dwIndex++];
		WORD wC = pIndiceData[dwIndex++];

		// reference old vertex data
		float* pVertDataA = (pMeshDataRaw+(wA*(dwFVFSize/4)));
		float* pVertDataB = (pMeshDataRaw+(wB*(dwFVFSize/4)));
		float* pVertDataC = (pMeshDataRaw+(wC*(dwFVFSize/4)));

		// create new vertex data
		memcpy(pMeshDataPtr, pVertDataA, dwFVFSize);
		pMeshDataPtr+=dwFVFSize/4;
		memcpy(pMeshDataPtr, pVertDataB, dwFVFSize);
		pMeshDataPtr+=dwFVFSize/4;
		memcpy(pMeshDataPtr, pVertDataC, dwFVFSize);
		pMeshDataPtr+=dwFVFSize/4;
		*pdwVertsSoFar+=3;
	}
	
	// return new meshdata
	return pTempMeshData;
}

sMesh* MakeMeshFromData (	DWORD dwInFVF, DWORD dwInFVFSize, float* pInMesh, DWORD dwInNumVert, DWORD dwPrimType )
{
	// create a new mesh structure
	sMesh* Mesh = new sMesh ( );

	// Index data created from scratch
	DWORD dwInNumPoly=0;
	if(dwPrimType==D3DPT_TRIANGLELIST) dwInNumPoly=dwInNumVert/3;
	if(dwPrimType==D3DPT_TRIANGLESTRIP) dwInNumPoly=dwInNumVert-2;

	// create mesh
	float* DstPtr;
	D3DXCreateMeshFVF (	dwInNumPoly, dwInNumVert, 0, dwInFVF, m_pD3D, &Mesh->m_Mesh );

	// copy out vertex data
	if ( SUCCEEDED ( Mesh->m_Mesh->LockVertexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &DstPtr ) ) )
	{
		memcpy(DstPtr, pInMesh, dwInFVFSize*dwInNumVert);
		Mesh->m_Mesh->UnlockVertexBuffer ( );
	}

	// create plain-old index data
	WORD* pIndex;
	DWORD dwVert=0;
	if ( SUCCEEDED ( Mesh->m_Mesh->LockIndexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &pIndex ) ) )
	{
		for(DWORD dwVert=0; dwVert<dwInNumVert; dwVert++)
			*(pIndex++)=(WORD)dwVert;

		Mesh->m_Mesh->UnlockIndexBuffer ( );
	}

	// calculate the bounding box and sphere
	BYTE** Ptr;
	if ( SUCCEEDED ( Mesh->m_Mesh->LockVertexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &Ptr ) ) )
	{
		// create a bounding box
		D3DXComputeBoundingBox ( 
									( void* ) Ptr,
									dwInNumVert,
									dwInFVF,
									&Mesh->m_Min, 
									&Mesh->m_Max
							   );

		// create a bounding sphere
		D3DXComputeBoundingSphere (
									( void* ) Ptr,
									dwInNumVert,
									dwInFVF,
									&D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f ),
									&Mesh->m_Radius
								  );

		// unlock
		Mesh->m_Mesh->UnlockVertexBuffer ( );
	}

	// Constructed meshes use passed in primtype
	Mesh->m_iPrimType = (D3DPRIMITIVETYPE)dwPrimType;

	// create new attrib info
	Mesh->m_dwAttributeTableSize=1;
	Mesh->m_pAttributeTable = new D3DXATTRIBUTERANGE [ Mesh->m_dwAttributeTableSize ];
	Mesh->m_pAttributeTable [ 0 ].VertexStart = 0;
	Mesh->m_pAttributeTable [ 0 ].VertexCount = dwInNumVert;
	Mesh->m_pAttributeTable [ 0 ].FaceStart = 0;
	Mesh->m_pAttributeTable [ 0 ].FaceCount = dwInNumPoly;

	// create a default one
	Mesh->m_Materials = new D3DMATERIAL8       [ 1 ];
	ZeroMemory ( Mesh->m_Materials, sizeof ( D3DMATERIAL8 ) );

	Mesh->m_Textures  = new LPDIRECT3DTEXTURE8 [ 1 ];
	ZeroMemory ( Mesh->m_Textures, sizeof ( LPDIRECT3DTEXTURE8 ) );

	Mesh->m_Materials [ 0 ].Diffuse.r = 1.0f;
	Mesh->m_Materials [ 0 ].Diffuse.g = 1.0f;
	Mesh->m_Materials [ 0 ].Diffuse.b = 1.0f;
	Mesh->m_Materials [ 0 ].Diffuse.a = 1.0f;
	Mesh->m_Materials [ 0 ].Ambient   = Mesh->m_Materials [ 0 ].Diffuse;
	Mesh->m_Materials [ 0 ].Specular  = Mesh->m_Materials [ 0 ].Diffuse;
	Mesh->m_Textures  [ 0 ]           = NULL;
	Mesh->m_NumMaterials = 1;

	return Mesh;
}

bool MakeModelData (	sObjectData* pData, DWORD dwInFVF, DWORD dwInFVFSize,
						float* pInMesh, DWORD dwInNumPoly, DWORD dwInNumVert, DWORD dwInPrimType )
{
	// create a new frame structure
	sFrame* ParentFrame = new sFrame ( );

	pData->m_Frames = ParentFrame;
	pData->m_Frames->m_Name = new char [ 7 ];
	strcpy ( pData->m_Frames->m_Name, "%ROOT%" );
	pData->m_Frames->m_iID=0;

	// increase frame count
	pData->m_NumFrames++;

	// default matrix
	D3DXMatrixIdentity( &ParentFrame->m_matOriginal );

	// make single mesh
	sMesh* Mesh = MakeMeshFromData ( dwInFVF, dwInFVFSize, pInMesh, dwInNumVert, dwInPrimType );

	// Copy mesh bound to main bound
	if(Mesh)
	{
		pData->m_Min = Mesh->m_Min;
		pData->m_Max = Mesh->m_Max;
		pData->m_Radius = Mesh->m_Radius;
	}
	pData->m_iTriangleCount = dwInNumPoly;

	// vertex info
	pData->m_dwFVFSize = dwInFVFSize;
	pData->m_dwFVF = dwInFVF;

	// no animation
	pData->m_StartTime                 = 0;
	pData->m_Animations.m_AnimationSet = NULL;

	// link in mesh
	ParentFrame->AddMesh(Mesh);
	pData->m_Meshes = Mesh;
	pData->m_NumMeshes++;

	// Complete
	return true;
}

void MapFramesToBones ( sObjectData* pData, sFrame *Frame )
{
	sMesh* Mesh;
	DWORD  i;
	char** Name;

	// return if no more frames to map
	if ( Frame == NULL || Frame->m_Name == NULL )
		return;

	// scan through meshes looking for bone matches
	Mesh = pData->m_Meshes;

	while ( Mesh != NULL )
	{
		if ( Mesh->m_NumBones && Mesh->m_BoneNames != NULL && Mesh->m_Matrices != NULL && Mesh->m_FrameMatrices != NULL )
		{
			Name = ( char** ) Mesh->m_BoneNames->GetBufferPointer ( );

			for ( i = 0; i < Mesh->m_NumBones; i++ )
			{
				if ( !strcmp ( Frame->m_Name, Name [ i ] ) )
				{
					Mesh->m_FrameMatrices [ i ] = &Frame->m_matCombined;
					break;
				}
			}
		}
		
		Mesh = Mesh->m_Next;
	}

	// scan through child frames
	MapFramesToBones ( pData, Frame->m_Child );

	// scan through sibling frames
	MapFramesToBones ( pData, Frame->m_Sibling );
}

void UpdateFrame ( sObjectData* pData, sFrame *Frame, D3DXMATRIX *Matrix )
{
	// return if no more frames
	if ( Frame == NULL )
		return;

	// calculate frame matrix based on animation or not
	if ( pData->m_Animations.m_AnimationSet == NULL )
	{
		D3DXMatrixMultiply ( &Frame->m_matCombined, &Frame->m_matOriginal, Matrix );
	}
	else
	{
		D3DXMatrixMultiply ( &Frame->m_matCombined, &Frame->m_matTransformed, Matrix );
	}

	// update child frames
	UpdateFrame ( pData, Frame->m_Child, &Frame->m_matCombined );

	// update sibling frames
	UpdateFrame ( pData, Frame->m_Sibling, Matrix );
}

void DrawFrame ( sObjectData* pData, sFrame* Frame, int iTIndex, D3DXMATRIX* pmatHierarchy )
{
	sFrameMeshList* List;
	sMesh*			Mesh;
	D3DXMATRIX      matWorld;
	DWORD           i;

	if ( Frame == NULL )
		return;

	// matrices used for limb properties
	D3DXMATRIX	matTranslation;										// translation ( position )
	D3DXMATRIX	matRotation, matRotateX, matRotateY, matRotateZ;	// rotation
	D3DXMATRIX	matScale;											// scale
	
	// apply scaling to the limb
	D3DXMatrixScaling ( &matScale, Frame->m_Limb.vecScale.x, Frame->m_Limb.vecScale.y, Frame->m_Limb.vecScale.z );

	// apply translation to the limb
	D3DXMatrixTranslation ( &matTranslation, Frame->m_Limb.vecOffset.x, Frame->m_Limb.vecOffset.y, Frame->m_Limb.vecOffset.z );

	// setup rotation matrices
	D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( Frame->m_Limb.vecRotate.x ) );	// x rotation
	D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( Frame->m_Limb.vecRotate.y ) );	// y rotation
	D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( Frame->m_Limb.vecRotate.z ) );	// z rotation

	// build final rotation matrix
	matRotation = matRotateX * matRotateY * matRotateZ;

	// finally build up all matrices, the order in which we do this is important
	// by applying the rotation first we will get the limb to rotate around it's
	// centre point
	D3DXMATRIX matLimbMatrix = matRotation * matScale * matTranslation;
	
	// hierarchy matrix
	matLimbMatrix = matLimbMatrix * (*pmatHierarchy);

	// final world matrix
//	matWorld = matRotation * matScale * matTranslation * Frame->m_matCombined * ( *pData->m_matObject );
	matWorld = matLimbMatrix * Frame->m_matCombined * ( *pData->m_matObject );

	if ( ( List = Frame->m_MeshList ) != NULL )
	{
		while ( List != NULL )
		{
			// see if there's a mesh to draw
			if ( ( Mesh = List->m_Mesh ) != NULL )
			{
				// generate the mesh if using bones and set world matrix
				if ( Mesh->m_NumBones && Mesh->m_SkinMesh != NULL )
				{
					Mesh->m_SkinMesh->UpdateSkinnedMesh ( Mesh->m_Matrices, NULL, Mesh->m_Mesh );

					// set object world transformation
					m_pD3D->SetTransform ( D3DTS_WORLD, pData->m_matObject );
				}
				else 
				{
					// set the world transformation matrix for this frame
					m_pD3D->SetTransform ( D3DTS_WORLD, &matWorld );
				}

				sMesh* pMesh = Mesh;

				// loop through materials and draw the mesh
				for ( i = 0; i < Mesh->m_NumMaterials; i++ )
				{
					// don't draw materials with no alpha ( 0.0 )
					//if ( Mesh->m_Materials [ i ].Diffuse.a != 0.0f )
					{
						// only if frame visible
						if ( Frame->m_Limb.bVisible )
						{
							// set material for texture
							bool bGotMaterial=false;
							if ( Mesh->m_Materials )
							{
								m_pD3D->SetMaterial ( &Mesh->m_Materials [ i ] );
								bGotMaterial=true;
							}
							if(bGotMaterial==false)
								m_pD3D->SetMaterial ( &gWhiteMaterial );

							// DBV1 Legacy Mode (however can omit this as a feature in the future)
							if ( Mesh->m_Textures [ i ] )
								m_pD3D->SetMaterial ( &gWhiteMaterial );

							if(iTIndex>=0 && iTIndex<=1)

							//iTIndex = 0;
							{
								m_pD3D->SetTexture  ( iTIndex, Mesh->m_Textures [ i ] );
								if( Mesh->m_Textures [ i ] )
								{
									// Texture
									m_pD3D->SetTextureStageState( iTIndex, D3DTSS_COLOROP, D3DTOP_MODULATE );
									m_pD3D->SetTextureStageState( iTIndex, D3DTSS_COLORARG1, D3DTA_TEXTURE );
									if(iTIndex==0)
										m_pD3D->SetTextureStageState( iTIndex, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
									else
										m_pD3D->SetTextureStageState( iTIndex, D3DTSS_COLORARG2, D3DTA_CURRENT );
								}
								else
								{
									/*
									// mj - not sure if this is needed?
									// Colour
									if(iTIndex==0)
									{
										m_pD3D->SetTextureStageState( iTIndex, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
										m_pD3D->SetTextureStageState( iTIndex, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
									}
									else
									{
										m_pD3D->SetTextureStageState( iTIndex, D3DTSS_COLOROP, D3DTOP_MODULATE );
										m_pD3D->SetTextureStageState( iTIndex, D3DTSS_COLORARG1, D3DTA_TFACTOR );
										m_pD3D->SetTextureStageState( iTIndex, D3DTSS_COLORARG2, D3DTA_CURRENT );
									}
									*/
								}
							}

							// prepare shader
							LPDIRECT3DVERTEXBUFFER8 m_pVB;
							LPDIRECT3DINDEXBUFFER8  m_pIB;
							Mesh->m_Mesh->GetVertexBuffer ( &m_pVB );
							Mesh->m_Mesh->GetIndexBuffer  ( &m_pIB );

							if ( pData->m_bOverrideVertexShader )
							{
								m_pD3D->SetVertexShader ( pData->m_dwVertexShader );
							}
							else
								m_pD3D->SetVertexShader ( pData->m_dwFVF );

							m_pD3D->SetStreamSource ( 0, m_pVB, pData->m_dwFVFSize );
							m_pD3D->SetIndices      ( m_pIB, 0 );

							// each mesh can have a different primitive type
							D3DPRIMITIVETYPE dwPrimType = Mesh->m_iPrimType;

							// indexed polygons
							m_pD3D->DrawIndexedPrimitive(	dwPrimType,
															Mesh->m_pAttributeTable [ i ].VertexStart,
															Mesh->m_pAttributeTable [ i ].VertexCount, 
															Mesh->m_pAttributeTable [ i ].FaceStart * 3,
															Mesh->m_pAttributeTable [ i ].FaceCount );

							// THIS release swine had me fooled for days!!
							if(m_pVB) m_pVB->Release();
							if(m_pIB) m_pIB->Release();
							
							#if DEBUG_MODE_MESH
							if ( Mesh->m_pMeshBox )
							{
								D3DXMATRIX matOriginal;
								m_pD3D->GetTransform ( D3DTS_WORLD, &matOriginal );
								
								//D3DXMATRIX mat = Mesh->m_matBox * *pData->m_matObject;
								//D3DXMATRIX mat = Mesh->m_matBox * matWorld;
								//D3DXMATRIX mat = Mesh->m_matBox * matRotation * matScale * matTranslation * Frame->m_matCombined * ( *pData->m_matObject );
								//D3DXMATRIX mat = matOriginal * Mesh->m_matBox;

								DWORD dwFill;
								m_pD3D->GetRenderState ( D3DRS_FILLMODE, &dwFill );


								D3DXMATRIX mat = Mesh->m_matBox * matWorld;

								m_pD3D->SetTransform( D3DTS_WORLD, &mat );
								/*
								//m_pD3D->SetRenderState( D3DRS_LIGHTING, FALSE );
								m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
								m_pD3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
								m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
								m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
								m_pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
								Mesh->m_pMeshBox->DrawSubset( 0 );
								m_pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
								m_pD3D->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
								//m_pD3D->SetRenderState( D3DRS_LIGHTING, TRUE );
								*/

								m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
								//m_pD3D->SetRenderState( D3DRS_LIGHTING, FALSE );
								m_pD3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
								Mesh->m_pMeshBox->DrawSubset( 0 );
								m_pD3D->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
								//m_pD3D->SetRenderState( D3DRS_LIGHTING, TRUE );

								if ( dwFill & D3DFILL_SOLID )
									m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
							
								m_pD3D->SetTransform( D3DTS_WORLD, &matOriginal );
							}
							#endif
						}
					}
				}
			}

			// mext mesh in list
			List = List->m_Next;
		}
	}

	// next sybling
	D3DXMATRIX matFromNewLimbPosition;
	matFromNewLimbPosition = matLimbMatrix;
	DrawFrame ( pData, Frame->m_Child, iTIndex, &matFromNewLimbPosition );

	// next child mesh
	DrawFrame ( pData, Frame->m_Sibling, iTIndex, pmatHierarchy );
}

ID3DXMesh* ConvertMeshStripToList ( sMesh* pMesh, DWORD dwInFVF, DWORD dwFVFSize )
{
	// new mesh stats
	ID3DXMesh* pStripMesh = pMesh->m_Mesh;
	DWORD dwSrcNumVert = pMesh->m_pAttributeTable [ 0 ].VertexCount;
	DWORD dwSrcNumPoly = pMesh->m_pAttributeTable [ 0 ].FaceCount;
	DWORD dwInNumPoly = dwSrcNumPoly;
	DWORD dwInNumVert = dwSrcNumPoly*3;

	// create new mesh
	ID3DXMesh* pWorkMesh = NULL;
	D3DXCreateMeshFVF (	dwInNumPoly, dwInNumVert, 0, dwInFVF, m_pD3D, &pWorkMesh );
	if(pWorkMesh)
	{
		// copy data from "strip mesh" to "new list mesh"
		WORD *DstIPtr, *SrcIPtr;
		float *DstVPtr, *SrcVPtr;
		if ( SUCCEEDED ( pWorkMesh->LockVertexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &DstVPtr ) ) )
		{
			float* pCurrentDestVertPtr = DstVPtr;
			if ( SUCCEEDED ( pWorkMesh->LockIndexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &DstIPtr ) ) )
			{
				DWORD dwIndexCount=0;
				WORD* pCurrentDestIndexPtr = DstIPtr;
				if ( SUCCEEDED ( pStripMesh->LockVertexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &SrcVPtr ) ) )
				{
					int iToggle=0;
					DWORD dwSrcIndex=0;
					WORD wA=0, wB=0, wC=0;
					if ( SUCCEEDED ( pStripMesh->LockIndexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &SrcIPtr ) ) )
					{
						// for each item in strip mesh
						while(dwSrcIndex<dwSrcNumVert)
						{
							// read index from poly
							if(dwSrcIndex==0)
							{
								// start of strip
								wA = SrcIPtr[dwSrcIndex+0];
								wB = SrcIPtr[dwSrcIndex+1];
								wC = SrcIPtr[dwSrcIndex+2];
								dwSrcIndex+=3;
							}
							else
							{
								// next in strip - figure out how read 'many' strips in one mesh!
								wA = wB;
								wB = wC;
								wC = SrcIPtr[dwSrcIndex];
								dwSrcIndex+=1;

								// strips toggle their cull order
								if(iToggle==1)
								{
									WORD wS=wB;
									wB=wC;
									wC=wS;
								}
								iToggle=1-iToggle;
							}

							// reference old vertex data
							float* pVertDataA = (SrcVPtr+(wA*(dwFVFSize/4)));
							float* pVertDataB = (SrcVPtr+(wB*(dwFVFSize/4)));
							float* pVertDataC = (SrcVPtr+(wC*(dwFVFSize/4)));

							// produce whole triangle
							memcpy(pCurrentDestVertPtr, pVertDataA, dwFVFSize);
							pCurrentDestVertPtr+=dwFVFSize/4;
							memcpy(pCurrentDestVertPtr, pVertDataB, dwFVFSize);
							pCurrentDestVertPtr+=dwFVFSize/4;
							memcpy(pCurrentDestVertPtr, pVertDataC, dwFVFSize);
							pCurrentDestVertPtr+=dwFVFSize/4;

							// produce indices for whole triangle
							*(pCurrentDestIndexPtr+0)=(WORD)(dwIndexCount+0);
							*(pCurrentDestIndexPtr+1)=(WORD)(dwIndexCount+1);
							*(pCurrentDestIndexPtr+2)=(WORD)(dwIndexCount+2);
							pCurrentDestIndexPtr+=3;
							dwIndexCount+=3;
						}

						// unlocks
						pStripMesh->UnlockIndexBuffer ( );
					}		
					pStripMesh->UnlockVertexBuffer ( );
				}		
				pWorkMesh->UnlockIndexBuffer ( );
			}		
			pWorkMesh->UnlockVertexBuffer ( );
		}
	}

	// return new mesh
	return pWorkMesh;
}

void BuildMesh (	sObjectData* pData, sFrame* Frame, DWORD dwFVF, DWORD dwFVFSize,
					DWORD* pdwVertMax, float** ppTempMeshData, float** ppMeshDataPtr, DWORD* pdwVertsSoFar,
					DWORD* pdwPolyMax, WORD** ppTempIndexData, WORD** ppIndexDataPtr, DWORD* pdwPolysSoFar, int iLimbPart )
{
	sFrameMeshList* List;
	sMesh*			Mesh;
	D3DXMATRIX      matWorld;
	DWORD           i;

	if ( Frame == NULL )
		return;

	// matrices used for limb properties
	D3DXMATRIX	matTranslation;										// translation ( position )
	D3DXMATRIX	matRotation, matRotateX, matRotateY, matRotateZ;	// rotation
	D3DXMATRIX	matScale;											// scale
	
	// apply scaling to the limb
	D3DXMatrixScaling ( &matScale, Frame->m_Limb.vecScale.x, Frame->m_Limb.vecScale.y, Frame->m_Limb.vecScale.z );

	// apply translation to the limb
	D3DXMatrixTranslation ( &matTranslation, Frame->m_Limb.vecOffset.x, Frame->m_Limb.vecOffset.y, Frame->m_Limb.vecOffset.z );

	// setup rotation matrices
	D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( Frame->m_Limb.vecRotate.x ) );	// x rotation
	D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( Frame->m_Limb.vecRotate.y ) );	// y rotation
	D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( Frame->m_Limb.vecRotate.z ) );	// z rotation

	// build final rotation matrix
	matRotation = matRotateX * matRotateY * matRotateZ;
	matWorld = matRotation * matScale * matTranslation * Frame->m_matCombined * ( *pData->m_matObject );

	// if part grab in force, only do mesh with correct limb number
	bool bProcessMesh=false;
	if ( iLimbPart==-1 ) bProcessMesh=true;
	if ( iLimbPart==Frame->m_iID )
	{
		// when grabbing single limb from object
		matWorld._41=0.0f; 
		matWorld._42=0.0f; 
		matWorld._43=0.0f; 
		bProcessMesh=true;
	}
	if(bProcessMesh)
	{
		if ( ( List = Frame->m_MeshList ) != NULL )
		{
			while ( List != NULL )
			{
				// see if there's a mesh to draw
				if ( ( Mesh = List->m_Mesh ) != NULL )
				{
					// generate the mesh if using bones and set world matrix
					if ( Mesh->m_NumBones && Mesh->m_SkinMesh != NULL )
					{
						Mesh->m_SkinMesh->UpdateSkinnedMesh ( Mesh->m_Matrices, NULL, Mesh->m_Mesh );
					}
					
					// only if frame visible
					if ( Frame->m_Limb.bVisible )
					{
						// loop through materials and draw the mesh
						sMesh* pMesh = Mesh;

						//for ( i = 0; i < Mesh->m_NumMaterials; i++ )
						for ( i = 0; i < Mesh->m_dwAttributeTableSize; i++ )
						{
							// mesh primitive type (list or strip)
							int iPrimType = Mesh->m_iPrimType;

							// add transformed data to temp meshdata
							DWORD dwVertsInObjMesh = Mesh->m_pAttributeTable [ i ].VertexCount; 
							DWORD dwPolysInObjMesh = Mesh->m_pAttributeTable [ i ].FaceCount;

							// if strip, convert mesh to list first
							ID3DXMesh* pWorkMesh = pMesh->m_Mesh;
							bool bTempMesh=false;
							if ( iPrimType==D3DPT_TRIANGLESTRIP )
							{
								pWorkMesh = ConvertMeshStripToList ( pMesh, dwFVF, dwFVFSize );
								bTempMesh=true;

								// new data new count
								dwVertsInObjMesh = dwPolysInObjMesh*3;
							}

							// lock mesh from object and copy to temp
							float* pDataFromMesh;
							DWORD dwVertOffset = *pdwVertsSoFar;
							if ( SUCCEEDED ( pWorkMesh->LockVertexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &pDataFromMesh ) ) )
							{
								// determine size to copy and whether temp is bg enough
								DWORD dwSizeToCopy = dwVertsInObjMesh*dwFVFSize;
								DWORD dwDataPos = (LPSTR)*ppMeshDataPtr - (LPSTR)*ppTempMeshData;
								if ( dwDataPos+dwSizeToCopy > (*pdwVertMax-32)*dwFVFSize )
								{
									// Resize temp meshdata
									DWORD dwNewVertMax = (*pdwVertMax*2)+dwVertsInObjMesh;
									float* pNewTemp = (float*)new char[dwNewVertMax*dwFVFSize];
									memcpy( pNewTemp, *ppTempMeshData, *pdwVertMax*dwFVFSize );
									SAFE_DELETE(*ppTempMeshData);
									*ppTempMeshData=pNewTemp;
									*ppMeshDataPtr=*ppTempMeshData+(*pdwVertsSoFar*(dwFVFSize/4));
									*pdwVertMax=dwNewVertMax;
								}

								// copy to temp
								memcpy(*ppMeshDataPtr, pDataFromMesh, dwSizeToCopy);

								// transform all XYZ in meshdata
								float* pVecData = *ppMeshDataPtr;
								for(DWORD v=0; v<dwVertsInObjMesh; v++)
								{
									// original vertex
									D3DXVECTOR3 pVec = D3DXVECTOR3(pVecData[0], pVecData[1], pVecData[2]);
									D3DXVECTOR3 pNor = D3DXVECTOR3(pVecData[3], pVecData[4], pVecData[5]);

									// transform with world matrix
									D3DXVec3TransformCoord(&pVec, &pVec, &matWorld);
									D3DXVec3TransformNormal(&pNor, &pNor, &matWorld);

									// put transformed vertex back
									pVecData[0]=pVec.x;
									pVecData[1]=pVec.y;
									pVecData[2]=pVec.z;
									pVecData[3]=pNor.x;
									pVecData[4]=pNor.y;
									pVecData[5]=pNor.z;
									pVecData+=dwFVFSize/4;
								}

								// advance past this vertex data
								*pdwVertsSoFar+=dwVertsInObjMesh;
								*ppMeshDataPtr+=dwSizeToCopy/sizeof(float);

								// unlock VB
								pWorkMesh->UnlockVertexBuffer ( );
							}

							// lock indice from object and copy to indice temp
							WORD* pIndexFromMesh;
							if ( SUCCEEDED ( pWorkMesh->LockIndexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &pIndexFromMesh ) ) )
							{
								// determine size to copy and whether temp is bg enough
								DWORD dwSizeToCopy = dwPolysInObjMesh*3*sizeof(WORD);
								DWORD dwDataPos = (LPSTR)*ppIndexDataPtr - (LPSTR)*ppTempIndexData;
								if ( dwDataPos+dwSizeToCopy > (*pdwPolyMax-32)*3*sizeof(WORD) )
								{
									// Resize temp meshdata
									DWORD dwNewMax = (*pdwPolyMax*2)+dwPolysInObjMesh;
									WORD* pNewTemp = new WORD[dwNewMax*3];
									memcpy( pNewTemp, *ppTempIndexData, *pdwPolyMax*3*sizeof(WORD) );
									SAFE_DELETE(*ppTempIndexData);
									*ppTempIndexData=pNewTemp;
									*ppIndexDataPtr=*ppTempIndexData+(*pdwPolysSoFar*3);
									*pdwPolyMax=dwNewMax;
								}
								if ( *pdwPolysSoFar + dwPolysInObjMesh > 32000 )
								{
									// Facetable overrun
									pWorkMesh->UnlockIndexBuffer ();
									return;
								}

								// copy to temp
								memcpy(*ppIndexDataPtr, pIndexFromMesh, dwSizeToCopy);

								// add vertexdataoffset to add face indexes (as mesh is being cumilatively built)
								for(DWORD f=0; f<dwPolysInObjMesh*3; f++)
									*(*ppIndexDataPtr+f) = *(*ppIndexDataPtr+f) + (WORD)dwVertOffset;

								// advance past this index data
								*pdwPolysSoFar+=dwPolysInObjMesh;
								*ppIndexDataPtr+=dwSizeToCopy/sizeof(WORD);

								// unlock VB
								pWorkMesh->UnlockIndexBuffer ( );
							}

							// temp mesh must be released
							if ( bTempMesh )
							{
								SAFE_RELEASE(pWorkMesh);
								bTempMesh=false;
							}
						}
					}
				}

				// mext mesh in list
				List = List->m_Next;
			}
		}
	}

	// next child mesh
	BuildMesh ( pData, Frame->m_Child, dwFVF, dwFVFSize, pdwVertMax, ppTempMeshData, ppMeshDataPtr, pdwVertsSoFar, pdwPolyMax, ppTempIndexData, ppIndexDataPtr, pdwPolysSoFar, iLimbPart );
	BuildMesh ( pData, Frame->m_Sibling, dwFVF, dwFVFSize, pdwVertMax, ppTempMeshData, ppMeshDataPtr, pdwVertsSoFar, pdwPolyMax, ppTempIndexData, ppIndexDataPtr, pdwPolysSoFar, iLimbPart );
}

BOOL SetAnimation ( sObjectData* pData, char* Name, unsigned long StartTime )
{
	pData->m_StartTime                 = StartTime;
	pData->m_Animations.m_AnimationSet = pData->m_Animations.GetAnimationSet ( Name );

	return TRUE;
}

BOOL UpdateAnimation ( sObjectData* pData, float Time, BOOL Smooth )
{
	if ( pData->m_Animations.m_AnimationSet != NULL )
	{
		if(pData->m_Frames) pData->m_Frames->ResetMatrices ( );
		pData->m_Animations.m_AnimationSet->Update ( Time - pData->m_StartTime, Smooth );
	}
	return TRUE;
}

BOOL GetBounds ( sObjectData* pData, float* MinX, float* MinY, float* MinZ, float* MaxX, float* MaxY, float* MaxZ, float* Radius )
{
	if ( MinX != NULL )
		*MinX = pData->m_Min.x;

	if ( MinY != NULL )
		*MinY = pData->m_Min.y;

	if ( MinZ != NULL )
		*MinZ = pData->m_Min.z;

	if ( MaxX != NULL )
		*MaxX = pData->m_Max.x;
 
	if ( MaxY != NULL )
		*MaxY = pData->m_Max.y;

	if ( MaxZ != NULL )
		*MaxZ = pData->m_Max.z;

	if ( Radius != NULL )
		*Radius = pData->m_Radius;

	return true;
}