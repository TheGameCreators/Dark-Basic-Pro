//
// ConvMDL Functions Header
//

//////////////////////////////////////////////////////////////////////////////////
// INCLUDE COMMON FILES //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include <windows.h>

//////////////////////////////////////////////////////////////////////////////////////
// DBOFORMAT INCLUDE /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#include "..\\DBOFormat\\DBOBlock.h"
#include ".\..\DBOFormat\DBOFormat.h"
#include ".\..\DBOFormat\DBOFrame.h"
#include ".\..\DBOFormat\DBOMesh.h"
#include ".\..\DBOFormat\DBORawMesh.h"
#include ".\..\DBOFormat\DBOEffects.h"
#include ".\..\DBOFormat\DBOFile.h"
#include ".\..\core\globstruct.h"

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE
	#undef DARKSDK
	#undef DBPRO_GLOBAL
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#else
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
	#define DARKSDK_DLL 
#endif

//#define DARKSDK __declspec ( dllexport )
//#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
//#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
//#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

//////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#ifndef DARKSDK_COMPILE
extern "C"
{
	DARKSDK void	PassCoreData	( LPVOID pGlobPtr );
	DARKSDK	bool	Convert			( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize );
	DARKSDK void	Free			( LPSTR );
}
#else
	void	PassCoreDataMDL	( LPVOID pGlobPtr );
	bool	ConvertMDL		( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize );
	void	FreeMDL			( LPSTR );
#endif


//////////////////////////////////////////////////////////////////////////////////
// MDL STRUCTURES ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct sMDLHeader
{
	int			iID;				// file id
	int			iVersion;			// version number

	char		sName [ 64 ];		// model name
	int			iLength;			// length of file

	D3DXVECTOR3 vEyePos;			// ideal eye position
	D3DXVECTOR3	vMin;				// ideal movement hull size
	D3DXVECTOR3	vMax;			

	D3DXVECTOR3	bbmin;
	D3DXVECTOR3	bbmax;		

	int			iFlags;

	int			iNumBones;
	int			iBoneIndex;

	int			iNumBoneControllers;
	int			iBoneControllerIndex;

	int			iNumHitBoxes;
	int			iHitBoxIndex;			

	int			iNumSeq;
	int			iSeqIndex;

	int			iNumSeqGroups;
	int			iSeqGroupIndex;

	int			iNumTextures;
	int			iTextureIndex;
	int			iTextureDataIndex;

	int			iNumSkinRef;
	int			iNumSkinFamilies;
	int			iSkinIndex;

	int			iNumBodyParts;		
	int			iBodyPartIndex;

	int			iNumAttachments;
	int			iAttachmentIndex;

	int			iSoundTable;
	int			iSoundIndex;
	int			iSoundGroups;
	int			iSoundGroupIndex;

	int			iNumTransitions;
	int			iTransitionIndex;
};

struct sMDLTexture
{
	char		sName [ 64 ];
	int			iFlags;
	int			iWidth;
	int			iHeight;
	int			iIndex;
};

struct sMDLBodyPart
{
	char	sName [ 64 ];
	
	int		iNumModels;
	int		iBase;
	int		iModelIndex;
};

struct sMDLModel
{
	char	szName [ 64 ];

	int		iType;

	float	fBoundingRadius;

	int		iNumMesh;
	int		iMeshIndex;

	int		iNumVerts;
	int		iVertInfoIndex;
	int		iVertIndex;
	int		iNumNorms;
	int		iNormInfoIndex;
	int		iNormIndex;

	int		iNumGroups;
	int		iGroupIndex;
};

struct sMDLMesh
{
	int		iNumTris;
	int		iTriIndex;
	int		iSkinRef;
	int		iNumNorms;
	int		iNormIndex;
};

struct sMDLVertex
{
	float	 x,  y,	 z;
	float	nx, ny, nz;
	float	tu, tv;

	sMDLVertex ( )
	{
		memset ( this, 0, sizeof ( sMDLVertex ) );
	}
};

struct sMDLDraw
{
	sMDLVertex*			pVertices;
	D3DPRIMITIVETYPE	iType;		// type of vertices e.g. strip or fan
	int					iCount;		// number of vertices
	int					iTexture;	// texture reference
	char				szTexture [ 256 ];

	sMDLDraw ( )
	{
		memset ( this, 0, sizeof ( sMDLDraw ) );
	}

	~sMDLDraw ( )
	{
		SAFE_DELETE_ARRAY ( pVertices );
	}
};

struct sMDLBone
{
	char		szName [ 32 ];

	int		 	iParent;
	int			iFlags;

	int			iBoneControl [ 6 ];

	float		fValue [ 6 ];
	float		fScale [ 6 ];
};

struct sMDLSequence
{
	char		szLabel [ 32 ];

	float		fFPS;
	
	int			iFlags;

	int			iActivity;
	int			iWeight;

	int			iNumEvents;
	int			iEventIndex;

	int			iNumFrames;

	int			iNumPivots;
	int			iPivotIndex;

	int			iMotionType;	
	int			iMotionBone;
	D3DXVECTOR3	iLinearMovement;
	int			iAutomovePosIndex;
	int			iAutomoveAngleIndex;

	D3DXVECTOR3	iBBMmin;
	D3DXVECTOR3	iBBMax;		

	int			iNumBlends;
	int			iAnimIndex;

	int			iBlendType  [ 2 ];
	float		fBlendStart [ 2 ];
	float		fBlendEnd   [ 2 ];
	int			iBlendParent;

	int			iSeqGroup;

	int			iEntryNode;
	int			iExitNode;
	int			iNodeFlags;

	int			iNextSeq;
};

struct sMDLAnimation
{
	unsigned short	usOffset [ 6 ];
};

struct sMDLSequenceGroup
{
	char		szLabel [ 32 ];
	char		szName  [ 64 ];

	void*		pvCache;

	int			iData;
};

union unAnimValue
{
	struct
	{
		byte	byValid;
		byte	byTotal;
	} num;

	short	value;
};
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////